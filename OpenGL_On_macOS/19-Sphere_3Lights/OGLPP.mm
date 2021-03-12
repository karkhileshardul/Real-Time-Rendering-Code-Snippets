// headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>
#import "vmath.h"
#import "Sphere.h"

enum
{
    VDG_ATTRIBUTE_VERTEX = 0,
    VDG_ATTRIBUTE_COLOR,
    VDG_ATTRIBUTE_NORMAL,
    VDG_ATTRIBUTE_TEXTURE0,
};

// 'C' style global function declarations
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef,const CVTimeStamp *,const CVTimeStamp *,CVOptionFlags,CVOptionFlags *,void *);

// global variables
FILE *gpFile=NULL;

GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat lightAmbient1[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse1[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat lightSpecular1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition1[] = { 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat lightAmbient2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse2[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat lightSpecular2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition2[] = { 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f;

GLfloat sphere_vertices[]={1146};
GLfloat sphere_normals[]={1146};
GLfloat sphere_textures[]={764};
short sphere_elements[]={2280};

Sphere sphere;

// interface declarations
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

// Entry-point function
int main(int argc, const char * argv[])
{
    // code
    NSAutoreleasePool *pPool=[[NSAutoreleasePool alloc]init];
    
    NSApp=[NSApplication sharedApplication];

    [NSApp setDelegate:[[AppDelegate alloc]init]];
    
    [NSApp run];
    
    [pPool release];
    
    return(0);
}

// interface implementations
@implementation AppDelegate
{
@private
    NSWindow *window;
    GLView *glView;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // code
    // log file
    NSBundle *mainBundle=[NSBundle mainBundle];
    NSString *appDirName=[mainBundle bundlePath];
    NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath=[NSString stringWithFormat:@"%@/Log.txt",parentDirPath];
    const char *pszLogFileNameWithPath=[logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    gpFile=fopen(pszLogFileNameWithPath,"w");
    if(gpFile==NULL)
    {
        printf("Can Not Create Log File.\nExitting ...\n");
        [self release];
        [NSApp terminate:self];
    }
    fprintf(gpFile, "Program Is Started Successfully\n");
    
    // window
    NSRect win_rect;
    win_rect=NSMakeRect(0.0,0.0,800.0,600.0);
    
    // create simple window
    window=[[NSWindow alloc] initWithContentRect:win_rect
                                       styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                         backing:NSBackingStoreBuffered
                                           defer:NO];
    [window setTitle:@"macOS OpenGL Window"];
    [window center];
    
    glView=[[GLView alloc]initWithFrame:win_rect];
    
    [window setContentView:glView];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    // code
    fprintf(gpFile, "Program Is Terminated Successfully\n");
    
    if(gpFile)
    {
        fclose(gpFile);
        gpFile=NULL;
    }
}

- (void)windowWillClose:(NSNotification *)notification
{
    // code
    [NSApp terminate:self];
}

- (void)dealloc
{
    // code
    [glView release];
    
    [window release];
    
    [super dealloc];
}
@end

@implementation GLView
{
@private
    CVDisplayLinkRef displayLink;
        
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    GLuint vao_Sphere;
	GLuint vbo_Sphere_Position;
	GLuint vbo_Sphere_Normal;
	GLuint vbo_sphere_element;
    
    int  modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;
	int  La_uniform, Ld_uniform, Ls_uniform, light_position_uniform;
	int  La_uniform1, Ld_uniform1, Ls_uniform1, light_position_uniform1;
	int  La_uniform2, Ld_uniform2, Ls_uniform2, light_position_uniform2;
	int  Ka_uniform, Kd_uniform, Ks_uniform, material_shininess_uniform;

	GLuint model_matrix_uniform, view_matrix_uniform, projection_matrix_uniform;

	GLuint L_KeyPressed_uniform;

    vmath::mat4 orthographicProjectionMatrix;

bool gbAnimate;
bool gbLight;

}

-(id)initWithFrame:(NSRect)frame;
{
    // code
    self=[super initWithFrame:frame];
    
    if(self)
    {
        [[self window]setContentView:self];
        
        NSOpenGLPixelFormatAttribute attrs[]=
        {
            // Must specify the 4.1 Core Profile to use OpenGL 4.1
            NSOpenGLPFAOpenGLProfile,
            NSOpenGLProfileVersion3_2Core,
            // Specify the display ID to associate the GL context with (main display for now)
            NSOpenGLPFAScreenMask,CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize,24,
            NSOpenGLPFADepthSize,24,
            NSOpenGLPFAAlphaSize,8,
            NSOpenGLPFADoubleBuffer,
            0}; // last 0 is must
        
        NSOpenGLPixelFormat *pixelFormat=[[[NSOpenGLPixelFormat alloc]initWithAttributes:attrs] autorelease];
        
        if(pixelFormat==nil)
        {
            fprintf(gpFile, "No Valid OpenGL Pixel Format Is Available. Exitting ...");
            [self release];
            [NSApp terminate:self];
        }
        
        NSOpenGLContext *glContext=[[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
        
        [self setPixelFormat:pixelFormat];
        
        [self setOpenGLContext:glContext]; // it automatically releases the older context, if present, and sets the newer one
    }
    return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime
{
    // code
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc]init];
    
    [self drawView];
    
    [pool release];
    return(kCVReturnSuccess);
}

-(void)prepareOpenGL
{
    // code
    // OpenGL Info
    fprintf(gpFile, "OpenGL Version  : %s\n",glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version    : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    [[self openGLContext]makeCurrentContext];
    
    GLint swapInt=1;
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    // *** VERTEX SHADER ***
    // create shader
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    
    // provide source code to shader
    const GLchar *vertexShaderSourceCode =
    "#version 330 core" \
    "\n"+
		"precision highp int;"+
		"uniform int u_perVertex, u_perFragment;"+
		"precision highp float;"+
        "in vec4 vPosition;"+
		"in vec3 vNormal;"+
		"uniform mat4 u_model_matrix;"+
		"uniform mat4 u_view_matrix;"+
		"uniform mat4 u_projection_matrix;"+		
		"uniform int u_LKeyPressed;"+
		"uniform vec3 u_La, u_Ld, u_Ls;"+
		"uniform vec3 u_La1, u_Ld1, u_Ls1;"+
		"uniform vec3 u_La2, u_Ld2, u_Ls2;"+
		"uniform vec3 u_Ka;"+
		"uniform vec3 u_Kd;"+
		"uniform vec3 u_Ks;"+
		"uniform vec4 u_light_position, u_light_position1, u_light_position2;"+
		"uniform float u_material_shininess;"+
		"out vec3 out_phong_ads_color;"+
		"out vec3 transformed_normals;"+
		"out vec3 light_direction, light_direction1, light_direction2;"+
		"out vec3 viewer_vector;"+
		 
		"uniform int u_double_tap;"+
		 
		 "vec3 calculateLight(vec3 u_La, vec3 u_Ld, vec3 u_Ls, vec4 u_light_position)"+
		"{"+
			"vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;"+
			"vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
			"vec3 light_direction = normalize(vec3(u_light_position) - eyeCoordinates.xyz);"+
			"float tn_dot_ld = max(dot(transformed_normals, light_direction), 0.0);"+
			"vec3 ambient = u_La * u_Kd;"+
			"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
			"vec3 reflection_vector = reflect(-light_direction, transformed_normals);"+
			"vec3 viewer_vector = normalize(-eyeCoordinates.xyz);"+
			"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);"+
			"out_phong_ads_color = ambient + diffuse + specular;"+
			"return out_phong_ads_color;"+
		"}"+
		 
		"void main(void)"+
		"{"+
		"if (u_double_tap == 1)"+
		"{" +
			"if(u_perVertex == 1)"+
			"{"+
				"vec3 light0 = calculateLight(u_La, u_Ld, u_Ls, u_light_position);"+
				"vec3 light1 = calculateLight(u_La1, u_Ld1, u_Ls1, u_light_position1);"+
				"vec3 light2 = calculateLight(u_La2, u_Ld2, u_Ls2, u_light_position2);"+
				"out_phong_ads_color += light0 + light1 + light2;"+
			"}"+

			"else if(u_perFragment == 1)"+
			"{"+
				"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;"+
				"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;"+
				"light_direction = vec3(u_light_position) - eye_coordinates.xyz;"+
				"light_direction1 = vec3(u_light_position1) - eye_coordinates.xyz;"+
				"light_direction2 = vec3(u_light_position2) - eye_coordinates.xyz;"+
			"}"+

			"else"+
			"{"+
				"out_phong_ads_color = vec3(1.0, 1.0, 1.0);"+
			"}"+
		"}"+
		"else"+
		"{"+
			"out_phong_ads_color = vec3(1.0, 1.0, 1.0);"+
		"}"+
		"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
		"}"
    
    glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
    
    // compile shader
    glCompileShader(vertexShaderObject);
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char *szInfoLog = NULL;
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // *** FRAGMENT SHADER ***
    // re-initialize
    iInfoLogLength = 0;
    iShaderCompiledStatus = 0;
    szInfoLog = NULL;
    
    // create shader
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    
    // provide source code to shader
    const GLchar *fragmentShaderSourceCode =
    "#version 330 core" \
    "\n"+
	"uniform int u_perVertex;"+
		"uniform int u_perFragment;"+
        "precision highp float;"+		 
        "in vec3 transformed_normals;"+
		"in vec3 light_direction, light_direction1, light_direction2;"+
		"in vec3 viewer_vector;"+
		"in vec3 out_phong_ads_color;"+
		"out vec4 FragColor;"+
		"uniform vec3 u_La, u_Ld, u_Ls;"+
		"uniform vec3 u_La1, u_Ld1, u_Ls1;"+
		"uniform vec3 u_La2, u_Ld2, u_Ls2;"+
		"uniform vec3 u_Ka;"+
		"uniform vec3 u_Kd;"+
		"uniform vec3 u_Ks;"+
		"uniform float u_material_shininess;"+
		"uniform int u_double_tap;" +		
		
		"vec3 calculateLight(vec3 u_La, vec3 u_Ld, vec3 u_Ls, vec3 light_direction)"+
		"{"+
			"vec3 phong_ads_color;"+
			"vec3 normalized_transformed_normals=normalize(transformed_normals);"+
			"vec3 normalized_light_direction=normalize(light_direction);"+
			"vec3 normalized_viewer_vector=normalize(viewer_vector);"+
			"vec3 ambient = u_La * u_Ka;"+
			"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);"+
			"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
			"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);"+
			"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);"+
			"phong_ads_color=ambient + diffuse + specular;"+
			"return phong_ads_color;"+
		"}"+
		
		"void main(void)"+
		"{"+
			"vec3 phong_ads_color;"+
			"if (u_double_tap == 1)"+
			"{"+
				"if(u_perVertex == 1)"+
				"{"+
					"FragColor = vec4(out_phong_ads_color, 1.0);"+
				"}"+

				"else if(u_perFragment == 1)"+
				"{"+
					"vec3 light0 = calculateLight(u_La, u_Ld, u_Ls, light_direction);"+
					"vec3 light1 = calculateLight(u_La1, u_Ld1, u_Ls1, light_direction1);"+
					"vec3 light2 = calculateLight(u_La2, u_Ld2, u_Ls2, light_direction2);"+
					"phong_ads_color += light0 + light1 + light2;"+
					"FragColor = vec4(phong_ads_color, 1.0f);"+
				"}"+

				"else"+
				"{"+
					"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"+
				"}"+
			"}"+

			"else"+
			"{"+
				"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"+
			"}"+
		"}"
    glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
    
    // compile shader
    glCompileShader(fragmentShaderObject);
    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // *** SHADER PROGRAM ***
    // create
    shaderProgramObject = glCreateProgram();
    
    // attach vertex shader to shader program
    glAttachShader(shaderProgramObject, vertexShaderObject);
    
    // attach fragment shader to shader program
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    
    // pre-link binding of shader program object with vertex shader Position attribute
    glBindAttribLocation(shaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");
    
    // Per-link binding of vertex shader program object with vertex shader color attribute
    glBindAttribLocation(shaderProgramObject, VDG_ATTRIBUTE_NORMAL, "vNormal");
    
    // link shader
    glLinkProgram(shaderProgramObject);
    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength>0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // get MVP uniform location
    

        model_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
	view_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
	projection_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

	L_KeyPressed_uniform = glGetUniformLocation(shaderProgramObject, "u_lighting_enabled");

	// Ambience color intensity of the light
	La_uniform =glGetUniformLocation(shaderProgramObject, "u_La");
	// Diffuse color intensity of the light
	Ld_uniform =glGetUniformLocation(shaderProgramObject, "u_Ld");
	// Specular color intensity of light
	Ls_uniform =glGetUniformLocation(shaderProgramObject, "u_Ls");
	// Light position
	light_position_uniform =glGetUniformLocation(shaderProgramObject, "u_light_position");		

	// Ambience color intensity of the light
	La_uniform1 =glGetUniformLocation(shaderProgramObject, "u_La1");
	// Diffuse color intensity of the light
	Ld_uniform1 =glGetUniformLocation(shaderProgramObject, "u_Ld1");
	// Specular color intensity of light
	Ls_uniform1 =glGetUniformLocation(shaderProgramObject, "u_Ls1");
	// Light position
	light_position_uniform1 =glGetUniformLocation(shaderProgramObject, "u_light_position1");

	// Ambience color intensity of the light
	La_uniform2 =glGetUniformLocation(shaderProgramObject, "u_La2");
	// Diffuse color intensity of the light
	Ld_uniform2 =glGetUniformLocation(shaderProgramObject, "u_Ld2");
	// Specular color intensity of light
	Ls_uniform2 =glGetUniformLocation(shaderProgramObject, "u_Ls2");
	// Light position
	light_position_uniform2 =glGetUniformLocation(shaderProgramObject, "u_light_position2");

	Ka_uniform = glGetUniformLocation(shaderProgramObject, "u_Ka");
	Kd_uniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
	Ks_uniform = glGetUniformLocation(shaderProgramObject, "u_Ks");
	material_shininess_uniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
    
	[sphere getSphereVertexData:sphere_vertices :sphere_normals :sphere_textures :sphere_elements];	
	numVertices = [sphere getNumberOfSphereVertices];
	numElements = [sphere getNumberOfSphereElements];
	
        
    // Vertex Array Object
	glGenVertexArrays(1, &vao_Sphere);
	glBindVertexArray(vao_Sphere);

	// Vertex Buffer Object
	/*****Vertex Buffer Object Position Start******/
	glGenBuffers(1, &vbo_Sphere_Position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(PND_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(PND_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*****Vertex Buffer Object Position End******/

	/*****Vertex Buffer Object Normal Start******/
	glGenBuffers(1, &vbo_Sphere_Normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

	glVertexAttribPointer(PND_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(PND_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*****Vertex Buffer Object Normal End******/

	/*****Vertex Buffer Object for Element Start******/
	glGenBuffers(1, &vbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	/*****Vertex Buffer Object for Element End******/

	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);
      
    // set background color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // blue
    
    // set projection matrix to identity matrix
    orthographicProjectionMatrix = vmath::mat4::identity();

    gbLight = false;
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);
    CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

-(void)reshape
{
    // code
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    NSRect rect=[self bounds];
    
    GLfloat width=rect.size.width;
    GLfloat height=rect.size.height;

    if(height==0)
        height=1;
    
    glViewport(0,0,(GLsizei)width,(GLsizei)height);
    
    
    orthographicProjectionMatrix = vmath::perspective(45.0f, width/height, 0.1f, 100.0f);

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect
{
    // code
    [self drawView];
}

- (void)drawView
{
    float lightYRotateZAxis = (float) (Math.cos(3.1415 * gLightAngleY / 180.0) * 10.0);
	float lightYRotateXAxis = (float) (Math.sin(3.1415 * gLightAngleY / 180.0) * 10.0);

	float lightXRotateZAxis = (float) (Math.sin(3.1415 * gLightAngleY / 180.0) * 10.0);
	float lightXRotateYAxis = (float) (Math.sin(3.1415 * gLightAngleY / 180.0) * 10.0);

	float lightZRotateYAxis = (float) (Math.sin(3.1415 * gLightAngleY / 180.0) * 10.0);
	float lightZRotateXAxis = (float) (Math.sin(3.1415 * gLightAngleY / 180.0) * 10.0);
    
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Start using the shaderProgramObject
    glUseProgram(shaderProgramObject);
    
    /** Here goes the code for OpenGL drawing**/
    
    /**************Triangle drawing start**************/
    
    // Set ModelView and ModelViewProjection matrices to identity
    if (gbLight == true)
	{
		glUniform1i(L_KeyPressed_uniform, 1);

		lightPosition[0] = 0.0f;
		lightPosition[1] = lightXRotateYAxis;
		lightPosition[2] = lightXRotateZAxis;
		lightPosition[3] = 1.0f;

		lightPosition1[0] = lightYRotateXAxis;
		lightPosition1[1] = 0.0f;
		lightPosition1[2] = lightYRotateZAxis;
		lightPosition1[3] = 1.0f;

		lightPosition2[0] = lightZRotateXAxis;
		lightPosition2[1] = lightZRotateYAxis;
		lightPosition2[2] = 0.0f;
		lightPosition2[3] = 1.0f;
		
		// Set light
		glUniform3fv(La_uniform, 1, lightAmbient, 0);
		glUniform3fv(Ld_uniform, 1, lightDiffuse, 0);
		glUniform3fv(Ls_uniform, 1, lightSpecular, 0);
		glUniform4fv(light_position_uniform, 1, lightPosition, 0);

		glUniform3fv(La_uniform1, 1, lightAmbient1, 0);
		glUniform3fv(Ld_uniform1, 1, lightDiffuse1, 0);
		glUniform3fv(Ls_uniform1, 1, lightSpecular1, 0);
		glUniform4fv(light_position_uniform1, 1, lightPosition1, 0);

		glUniform3fv(La_uniform2, 1, lightAmbient2, 0);
		glUniform3fv(Ld_uniform2, 1, lightDiffuse2, 0);
		glUniform3fv(Ls_uniform2, 1, lightSpecular2, 0);
		glUniform4fv(light_position_uniform2, 1, lightPosition2, 0);

		// Set material properties
		glUniform3fv(Ka_uniform, 1, material_ambient, 0);
		glUniform3fv(Kd_uniform, 1, material_diffuse, 0);
		glUniform3fv(Ks_uniform, 1, material_specular, 0);
		glUniform1f(material_shininess_uniform, material_shininess);
	}
	else
	{
		glUniform1i(L_KeyPressed_uniform, 0);
	}

	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();

	// Translate the triangle to make it visible
	modelMatrix = translate(0.0f, 0.0f, -2.0f);

	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	// Bind vao
	glBindVertexArray(vao_Sphere);

	// Draw the sphere
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	// Unbind vao
	glBindVertexArray(0);

	// Stop using the shaderProgramObject
	glUseProgram(0);
	
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
}

-(BOOL)acceptsFirstResponder
{
    // code
    [[self window]makeFirstResponder:self];
    return(YES);
}

-(void)keyDown:(NSEvent *)theEvent
{
    // code
    int key=(int)[[theEvent characters]characterAtIndex:0];    
    bool static bIsLKeyPressed = false;

    switch(key)
    {
        case 27: // Esc key
            [ self release];
            [NSApp terminate:self];
            break;
        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self];
            break;
        case 'l':
        case 'L':
            if (!bIsLKeyPressed) {
		gbLight = true;
		bIsLKeyPressed = true;
            }
	    else {
		gbLight = false;
		bIsLKeyPressed = false;
	    }
	    break;
        default:
            break;
    }
}

-(void)mouseDown:(NSEvent *)theEvent
{
    // code
}

-(void)mouseDragged:(NSEvent *)theEvent
{
    // code
}

-(void)rightMouseDown:(NSEvent *)theEvent
{
    // code
}

- (void) dealloc
{
    // code
    // destroy vao
    if (vao_Pyramid)
    {
        glDeleteVertexArrays(1, &vao_Pyramid);
        vao_Pyramid = 0;
    }
    
    // destroy vbo_Position
    if (vbo_Position)
    {
        glDeleteBuffers(1, &vbo_Position);
        vbo_Position = 0;
    }
    
    // detach vertex shader from shader program object
    glDetachShader(shaderProgramObject, vertexShaderObject);
    // detach fragment  shader from shader program object
    glDetachShader(shaderProgramObject, fragmentShaderObject);
    
    // delete vertex shader object
    glDeleteShader(vertexShaderObject);
    vertexShaderObject = 0;
    // delete fragment shader object
    glDeleteShader(fragmentShaderObject);
    fragmentShaderObject = 0;
    
    // delete shader program object
    glDeleteProgram(shaderProgramObject);
    shaderProgramObject = 0;

    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);

    [super dealloc];
}

@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,const CVTimeStamp *pNow,const CVTimeStamp *pOutputTime,CVOptionFlags flagsIn,
                               CVOptionFlags *pFlagsOut,void *pDisplayLinkContext)
{
    CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
    return(result);
}
