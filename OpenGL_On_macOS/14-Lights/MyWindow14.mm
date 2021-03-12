#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>
#import "vmath.h"

enum{
    SSK_ATTRIBUTE_VERTEX = 0,
    SSK_ATTRIBUTE_COLOR,
    SSK_ATTRIBUTE_NORMAL,
    SSK_ATTRIBUTE_TEXTURE0,
};

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef,const CVTimeStamp *,const CVTimeStamp *,CVOptionFlags,CVOptionFlags *,void *);

FILE *gpFile=NULL;

GLfloat lightAmbient_left[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse_left[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular_left[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition_left[] = { -2.0f, 1.0f, 1.0f, 0.0f };

GLfloat lightAmbient_right[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse_right[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular_right[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition_right[] = { 2.0f, 1.0f, 1.0f, 0.0f };

GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse[] = { 0.5f,0.5f,0.5f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f;


@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

int main(int argc, const char * argv[]){
    NSAutoreleasePool *pPool=[[NSAutoreleasePool alloc]init];
    NSApp=[NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc]init]];
    [NSApp run];
    [pPool release];
    return(0);
}

@implementation AppDelegate{
@private
    NSWindow *window;
    GLView *glView;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification{
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
    
    
    NSRect win_rect;
    win_rect=NSMakeRect(0.0,0.0,800.0,600.0);
    
    
    window=[[NSWindow alloc] initWithContentRect:win_rect
                                       styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                         backing:NSBackingStoreBuffered
                                           defer:NO];
    [window setTitle:@"14-Lights on Cube Window"];
    [window center];
    
    glView=[[GLView alloc]initWithFrame:win_rect];
    
    [window setContentView:glView];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

- (void)applicationWillTerminate:(NSNotification *)notification{

    fprintf(gpFile, "Program Is Terminated Successfully\n");
    if(gpFile){
        fclose(gpFile);
        gpFile=NULL;
    }
}

- (void)windowWillClose:(NSNotification *)notification{
    [NSApp terminate:self];
}

- (void)dealloc{
    [glView release];
    [window release];
    [super dealloc];
}
@end

@implementation GLView{
@private
    CVDisplayLinkRef displayLink;
        
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    GLuint vao_Cube;
    GLuint vbo_Position;
    GLuint vbo_Normals;
    GLuint La_uniform_left;
    GLuint Ld_uniform_left;
    GLuint Ls_uniform_left;
    GLuint left_light_position_uniform;
    GLuint La_uniform_right;
    GLuint Ld_uniform_right;
    GLuint Ls_uniform_right;
    GLuint right_light_position_uniform;
    GLuint Ka_uniform;
    GLuint Kd_uniform;
    GLuint Ks_uniform;
    GLuint material_shininess_uniform;

    GLuint model_matrix_uniform, view_matrix_uniform, projection_matrix_uniform;
    GLuint L_KeyPressed_uniform;
    vmath::mat4 perspectiveProjectionMatrix;
    bool gbAnimate;
    bool gbLight;

}

-(id)initWithFrame:(NSRect)frame;{

    self=[super initWithFrame:frame];
    if(self){
        [[self window]setContentView:self];
        
        NSOpenGLPixelFormatAttribute attrs[]={
            NSOpenGLPFAOpenGLProfile,
            NSOpenGLProfileVersion3_2Core,
            NSOpenGLPFAScreenMask,CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize,24,
            NSOpenGLPFADepthSize,24,
            NSOpenGLPFAAlphaSize,8,
            NSOpenGLPFADoubleBuffer,
            0};
        
        NSOpenGLPixelFormat *pixelFormat=[[[NSOpenGLPixelFormat alloc]initWithAttributes:attrs] autorelease];
        
        if(pixelFormat==nil){
            fprintf(gpFile, "No Valid OpenGL Pixel Format Is Available. Exitting ...");
            [self release];
            [NSApp terminate:self];
        }
        
        NSOpenGLContext *glContext=[[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
        
        [self setPixelFormat:pixelFormat];
        [self setOpenGLContext:glContext];
    }
    return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime{

    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc]init];
    [self drawView];
    [pool release];
    return(kCVReturnSuccess);
}

-(void)prepareOpenGL{

    fprintf(gpFile, "OpenGL Version  : %s\n",glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version    : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    [[self openGLContext]makeCurrentContext];
    GLint swapInt=1;
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    
    const GLchar *vertexShaderSourceCode =
    "#version 410" \
    "\n" \
    "in vec4 vPosition;" \
    "in vec3 vNormal;" \
    "uniform mat4 u_model_matrix;" \
    "uniform mat4 u_view_matrix;" \
    "uniform mat4 u_projection_matrix;" \
    "uniform int u_lighting_enabled;" \
    "uniform int u_LKeyPressed;" \
    "uniform vec3 u_La, u_Ld, u_Ls, u_Ka, u_Kd, u_Ks;" \
    "uniform vec3 u_La2, u_Ld2, u_Ls2;" \
    "uniform vec4 u_light_position, u_light_position2;" \
    "uniform float u_material_shininess;" \
    "out vec3 out_phong_ads_color;" \
    "vec3 phong_ads_color;" \
    "vec3 CalculateLight(vec3 La, vec3 Ld, vec3 Ls, vec4 light_position)"
    "{" \
        "vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;" \
        "vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
        "vec3 light_direction = normalize(vec3(light_position) - eyeCoordinates.xyz);" \
        "float tn_dot_ld = max(dot(transformed_normals, light_direction), 0.0);" \
        "vec3 ambient = La * u_Kd;" \
        "vec3 diffuse = Ld * u_Kd * tn_dot_ld;" \
        "vec3 reflection_vector = reflect(-light_direction, transformed_normals);" \
        "vec3 viewer_vector = normalize(-eyeCoordinates.xyz);" \
        "vec3 specular = Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" \
        "phong_ads_color = ambient + diffuse + specular;" \
        "return phong_ads_color;" \
    "}" \
    "void main(void)" \
    "{" \
        "if (u_lighting_enabled == 1)" \
        "{" \
            "vec3 lightleft = CalculateLight(u_La, u_Ld, u_Ls, u_light_position);" \
            "vec3 lightright = CalculateLight(u_La2, u_Ld2, u_Ls2, u_light_position2);" \
            "out_phong_ads_color = lightleft + lightright;" \
        "}" \
        "else" \
        "{" \
            "out_phong_ads_color = vec3(1.0, 1.0, 1.0);" \
        "}" \
            "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";
    
    glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

    glCompileShader(vertexShaderObject);
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char *szInfoLog = NULL;
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE){
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0){
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL){
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    iInfoLogLength = 0;
    iShaderCompiledStatus = 0;
    szInfoLog = NULL;
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    
    const GLchar *fragmentShaderSourceCode =
    "#version 410" \
    "\n" \
    "in vec3 out_phong_ads_color;" \
    "out vec4 FragColor;" \
    "void main(void)" \
    "{" \
        "FragColor = vec4(out_phong_ads_color, 1.0f);" \
    "}";
    glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

    glCompileShader(fragmentShaderObject);
    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE){
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0){
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL){
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    shaderProgramObject = glCreateProgram();
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    glBindAttribLocation(shaderProgramObject, SSK_ATTRIBUTE_VERTEX, "vPosition");
    glBindAttribLocation(shaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");
    
    glLinkProgram(shaderProgramObject);
    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE){
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength>0){
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL){
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    model_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
	view_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
	projection_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

	L_KeyPressed_uniform = glGetUniformLocation(shaderProgramObject, "u_lighting_enabled");

	La_uniform_right = glGetUniformLocation(shaderProgramObject, "u_La");
	Ld_uniform_right = glGetUniformLocation(shaderProgramObject, "u_Ld");
	Ls_uniform_right = glGetUniformLocation(shaderProgramObject, "u_Ls");
	right_light_position_uniform = glGetUniformLocation(shaderProgramObject, "u_light_position");

	La_uniform_left = glGetUniformLocation(shaderProgramObject, "u_La2");
	Ld_uniform_left = glGetUniformLocation(shaderProgramObject, "u_Ld2");
	Ls_uniform_left = glGetUniformLocation(shaderProgramObject, "u_Ls2");
	left_light_position_uniform = glGetUniformLocation(shaderProgramObject, "u_light_position2");

	Ka_uniform = glGetUniformLocation(shaderProgramObject, "u_Ka");
	Kd_uniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
	Ks_uniform = glGetUniformLocation(shaderProgramObject, "u_Ks");
	material_shininess_uniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
    
    GLfloat CubeVertices[] ={
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,

		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f
	};

	const GLfloat CubeNormals[] ={
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};    

    glGenVertexArrays(1, &vao_Cube);
        glBindVertexArray(vao_Cube);
            glGenBuffers(1, &vbo_Position);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_Position);
                glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);
                glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glGenBuffers(1, &vbo_Normals);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_Normals);
                glBufferData(GL_ARRAY_BUFFER, sizeof(CubeNormals), CubeNormals, GL_STATIC_DRAW);
                glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    glBindVertexArray(0);
    
   
    glClearDepth(1.0f);
   
    glEnable(GL_DEPTH_TEST);
 
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    perspectiveProjectionMatrix = vmath::mat4::identity();
    gbLight = false;
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);
    CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

-(void)reshape{
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    NSRect rect=[self bounds];
    
    GLfloat width=rect.size.width;
    GLfloat height=rect.size.height;
    if(height==0){
        height=1;
    }
    glViewport(0,0,(GLsizei)width,(GLsizei)height);
    perspectiveProjectionMatrix = vmath::perspective(45.0f, width/height, 0.1f, 100.0f);
    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect{
    [self drawView];
}

- (void)drawView{
    
    static float angleCube=0.0f;
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
            GL_STENCIL_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
    
        if(gbLight == true){
            glUniform1i(L_KeyPressed_uniform, 1);

            glUniform3fv(La_uniform_right, 1, lightAmbient_right);
            glUniform3fv(Ld_uniform_right, 1, lightDiffuse_right);
            glUniform3fv(Ls_uniform_right, 1, lightSpecular_right);
            glUniform4fv(right_light_position_uniform, 1, lightPosition_right);

            glUniform3fv(La_uniform_left, 1, lightAmbient_left);
            glUniform3fv(Ld_uniform_left, 1, lightDiffuse_left);
            glUniform3fv(Ls_uniform_left, 1, lightSpecular_left);
            glUniform4fv(left_light_position_uniform, 1, lightPosition_left);

            glUniform3fv(Ka_uniform, 1, material_ambient);
            glUniform3fv(Kd_uniform, 1, material_diffuse);
            glUniform3fv(Ks_uniform, 1, material_specular);
            glUniform1f(material_shininess_uniform, material_shininess);
        }else{
            glUniform1i(L_KeyPressed_uniform, 0);
        }

        vmath::mat4 modelMatrix = vmath::mat4::identity();
        vmath::mat4 viewMatrix = vmath::mat4::identity();
        vmath::mat4 rotationMatrix = vmath::mat4::identity();

        modelMatrix = vmath::translate(0.0f, 0.0f, -6.0f);

        rotationMatrix = vmath::rotate(angleCube, angleCube, angleCube	);

        modelMatrix = viewMatrix * modelMatrix * rotationMatrix;

        glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, perspectiveProjectionMatrix);
        glBindVertexArray(vao_Cube);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
            glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
            glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
            glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
            glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
        glBindVertexArray(0);
	glUseProgram(0);
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    angleCube = angleCube + 1.0f;
}

-(BOOL)acceptsFirstResponder{
    [[self window]makeFirstResponder:self];
    return(YES);
}

-(void)keyDown:(NSEvent *)theEvent{
    int key=(int)[[theEvent characters]characterAtIndex:0];
    bool static bIsAKeyPressed = false;
    bool static bIsLKeyPressed = false;
    switch(key){
        case 27:
            [ self release];
            [NSApp terminate:self];
            break;
        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self];
            break;
        case 'l':
        case 'L':
            if(!bIsLKeyPressed){
                gbLight = true;
                bIsLKeyPressed = true;
            }else{
                gbLight = false;
                bIsLKeyPressed = false;
            }
	    break;
        default:
            break;
    }
}

-(void)mouseDown:(NSEvent *)theEvent{
    
}

-(void)mouseDragged:(NSEvent *)theEvent{
    
}

-(void)rightMouseDown:(NSEvent *)theEvent{
 
}

- (void) dealloc{
    if (vao_Cube){
        glDeleteVertexArrays(1, &vao_Cube);
        vao_Cube = 0;
    }
    
   
    if (vbo_Position){
        glDeleteBuffers(1, &vbo_Position);
        vbo_Position = 0;
    }
    
    glDetachShader(shaderProgramObject, vertexShaderObject);
    glDetachShader(shaderProgramObject, fragmentShaderObject);
    glDeleteShader(vertexShaderObject);
    vertexShaderObject = 0;
    glDeleteShader(fragmentShaderObject);
    fragmentShaderObject = 0;
    
    glDeleteProgram(shaderProgramObject);
    shaderProgramObject = 0;

    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);

    [super dealloc];
}
@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,const CVTimeStamp *pNow,const CVTimeStamp *pOutputTime,CVOptionFlags flagsIn,
                               CVOptionFlags *pFlagsOut,void *pDisplayLinkContext){
    CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
    return(result);
}
