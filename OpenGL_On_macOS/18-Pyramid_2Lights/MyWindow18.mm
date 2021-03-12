
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


GLfloat one_lightAmbient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat one_lightDiffuse[]={1.0f,0.0f,0.0f,0.0f};
GLfloat one_lightSpecular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat one_lightPosition[]={100.0f,100.0f,100.0f,1.0f};

GLfloat two_lightAmbient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat two_lightDiffuse[]={0.0f,0.0f,1.0f,0.0f};
GLfloat two_lightSpecular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat two_lightPosition[]={-100.0f,100.0f,100.0f,1.0f};


GLfloat material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat material_diffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_shininess=50.0f;



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
    if(gpFile==NULL){
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
    [window setTitle:@"18-Pyramid Two Lights Window"];
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
    vmath::mat4 perspectiveProjectionMatrix;

    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;
    GLuint gVao_pyramid;
    GLuint gVbo_pyramid_position;
    GLuint gVbo_pyramid_normal;
    
    GLfloat gAngle;
    bool gbAnimate;
    bool gbLight;
    GLuint model_matrix_uniform,view_matrix_uniform,projection_matrix_uniform;
    GLuint L_KeyPressed_uniform;
    
    
    GLuint La_uniform_one;
    GLuint Ld_uniform_one;
    GLuint Ls_uniform_one;
    GLuint light_position_uniform_one;
    GLuint Ka_uniform;
    GLuint Kd_uniform;
    GLuint Ks_uniform;
    GLuint La_uniform_two;
    GLuint Ld_uniform_two;
    GLuint Ls_uniform_two;
    GLuint light_position_uniform_two;
    GLuint material_shininess_uniform;

}

-(id)initWithFrame:(NSRect)frame;{
    self=[super initWithFrame:frame];
    
    if(self){
        [[self window]setContentView:self];
        
        NSOpenGLPixelFormatAttribute attrs[]={
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
            NSOpenGLPFADoubleBuffer,0}; //last 0 is must
        
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
    
    
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    
    
    const GLchar *vertexShaderSourceCode =
    "#version 410" \
    "\n" \
    "in vec4 vPosition;" \
    "in vec3 vNormal;" \
    "uniform mat4 u_model_matrix;" \
    "uniform mat4 u_view_matrix;" \
    "uniform mat4 u_projection_matrix;" \
    "uniform vec4 u_light_position_one;" \
    "uniform vec4 u_light_position_two;" \
    "uniform int u_lighting_enabled;" \
    "out vec3 transformed_normals;" \
    "out vec3 light_direction_one;" \
    "out vec3 light_direction_two;" \
    "out vec3 viewer_vector;" \
    "void main(void)" \
    "{" \
        "if(u_lighting_enabled==1)" \
        "{" \
            "vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
            "transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
            "light_direction_one = vec3(u_light_position_one) - eye_coordinates.xyz;" \
            "light_direction_two = vec3(u_light_position_two) - eye_coordinates.xyz;" \
            "viewer_vector = -eye_coordinates.xyz;" \
        "}" \
        "gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
    "}";

    glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
    
    
    glCompileShader(gVertexShaderObject);
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char *szInfoLog = NULL;
    glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE){
        glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0){
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL){
                GLsizei written;
                glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
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
    
  gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    
    const GLchar *fragmentShaderSourceCode =
    "#version 410" \
    "\n" \
    "in vec3 transformed_normals;" \
    "in vec3 light_direction_one;" \
    "in vec3 light_direction_two;" \
    "in vec3 viewer_vector;" \
    "out vec4 FragColor;" \
    "uniform vec3 u_La_one;" \
    "uniform vec3 u_Ld_one;" \
    "uniform vec3 u_Ls_one;" \
    "uniform vec3 u_La_two;" \
    "uniform vec3 u_Ld_two;" \
    "uniform vec3 u_Ls_two;" \
    "uniform vec3 u_Ka;" \
    "uniform vec3 u_Kd;" \
    "uniform vec3 u_Ks;" \
    "uniform float u_material_shininess;" \
    "uniform int u_lighting_enabled;" \
    "void main(void)" \
    "{" \
        "vec3 phong_ads_color_one;" \
        "vec3 phong_ads_color_two;" \
        "if(u_lighting_enabled==1)" \
        "{" \
            "vec3 normalized_transformed_normals=normalize(transformed_normals);" \
            "vec3 normalized_light_direction_one=normalize(light_direction_one);" \
            "vec3 normalized_viewer_vector=normalize(viewer_vector);" \
            "vec3 ambient_one = u_La_one * u_Ka;" \
            "float tn_dot_ld1 = max(dot(normalized_transformed_normals, normalized_light_direction_one),0.0);" \
            "vec3 diffuse_one = u_Ld_one * u_Kd * tn_dot_ld1;" \
            "vec3 reflection_vector1 = reflect(-normalized_light_direction_one, normalized_transformed_normals);" \
            "vec3 specular_one = u_Ls_one * u_Ks * pow(max(dot(reflection_vector1, normalized_viewer_vector), 0.0), u_material_shininess);" \
            "phong_ads_color_one=ambient_one + diffuse_one + specular_one;" \
            "vec3 normalized_light_direction_two=normalize(light_direction_two);" \
            "vec3 ambient_two = u_La_two * u_Ka;" \
            "float tn_dot_ld2 = max(dot(normalized_transformed_normals, normalized_light_direction_two),0.0);" \
            "vec3 diffuse_two = u_Ld_two * u_Kd * tn_dot_ld2;" \
            "vec3 reflection_vector2 = reflect(-normalized_light_direction_two, normalized_transformed_normals);" \
            "vec3 specular_two = u_Ls_two * u_Ks * pow(max(dot(reflection_vector2, normalized_viewer_vector), 0.0), u_material_shininess);" \
            "phong_ads_color_two=ambient_two + diffuse_two + specular_two;" \
            "phong_ads_color_one = phong_ads_color_one + phong_ads_color_two;" \
        "}" \
        "else" \
        "{" \
            "phong_ads_color_one = vec3(1.0, 1.0, 1.0);" \
        "}" \
        "FragColor = vec4(phong_ads_color_one, 1.0);" \
    "}";

    glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
    
  
    glCompileShader(gFragmentShaderObject);
    glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE){
        glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0){
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL){
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    gShaderProgramObject = glCreateProgram();
    
    glAttachShader(gShaderProgramObject, gVertexShaderObject);
       
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);
    
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_VERTEX, "vPosition");
    
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");
    
 
    glLinkProgram(gShaderProgramObject);
    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE){
        glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength>0){
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL){
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
        model_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	view_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

	L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject, "u_lighting_enabled");

	La_uniform_one = glGetUniformLocation(gShaderProgramObject, "u_La_one");
	Ld_uniform_one = glGetUniformLocation(gShaderProgramObject, "u_Ld_one");
	Ls_uniform_one = glGetUniformLocation(gShaderProgramObject, "u_Ls_one");
	light_position_uniform_one = glGetUniformLocation(gShaderProgramObject, "u_light_position_one");

    Ka_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
    Kd_uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
    Ks_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");

    
	La_uniform_two = glGetUniformLocation(gShaderProgramObject, "u_La_two");
	Ld_uniform_two = glGetUniformLocation(gShaderProgramObject, "u_Ld_two");
	Ls_uniform_two = glGetUniformLocation(gShaderProgramObject, "u_Ls_two");
	light_position_uniform_two = glGetUniformLocation(gShaderProgramObject, "u_light_position_two");

	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
    
   
    const GLfloat pyramidVertices[] = {
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f
    };
    
    
    const GLfloat pyramidNormals[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0,
		1.0f, 0.0f, 0.0,
		1.0f, 0.0f, 0.0,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		-1.0f, 0.0f, 0.0,
		-1.0f, 0.0f, 0.0,
		-1.0f, 0.0f, 0.0
	};
    
   
    glGenVertexArrays(1, &gVao_pyramid);
    glBindVertexArray(gVao_pyramid);
        glGenBuffers(1, &gVbo_pyramid_position);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_pyramid_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    
    glGenBuffers(1, &gVbo_pyramid_normal);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_pyramid_normal);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // blue
    
    perspectiveProjectionMatrix=vmath::mat4::identity();
    gbAnimate = false;
    gbLight = false;
    gAngle=0.0f;
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

    if(height==0)
        height=1;
    
    glViewport(0,0,(GLsizei)width,(GLsizei)height);
    
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, width/height, 0.1f, 100.0f);

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect{
    [self drawView];
}

- (void)drawView{
    vmath::mat4 modelMatrix;
    vmath::mat4 rotationMatrix;
    vmath::mat4 viewMatrix;
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
            GL_STENCIL_BUFFER_BIT);
    
    
    glUseProgram(gShaderProgramObject);
    
 
     if (gbLight == true){
		glUniform1i(L_KeyPressed_uniform, 1);

		glUniform3fv(La_uniform_one, 1, one_lightAmbient);
		glUniform3fv(Ld_uniform_one, 1, one_lightDiffuse);
		glUniform3fv(Ls_uniform_one, 1, one_lightSpecular);
		glUniform4fv(light_position_uniform_one, 1, one_lightPosition);

		glUniform3fv(La_uniform_two, 1, two_lightAmbient);
		glUniform3fv(Ld_uniform_two, 1, two_lightDiffuse);
		glUniform3fv(Ls_uniform_two, 1, two_lightSpecular);
		glUniform4fv(light_position_uniform_two, 1, two_lightPosition);

		
		glUniform3fv(Ka_uniform, 1, material_ambient);
		glUniform3fv(Kd_uniform, 1, material_diffuse);
		glUniform3fv(Ks_uniform, 1, material_specular);
		glUniform1f(material_shininess_uniform, material_shininess);
	}else{
		glUniform1i(L_KeyPressed_uniform, 0);
	}

    modelMatrix = vmath::mat4::identity();
	viewMatrix = vmath::mat4::identity();
    modelMatrix=vmath::translate(0.0f,0.0f,-6.0f);
    rotationMatrix=vmath::rotate(gAngle,0.0f,1.0f,0.0f);
    modelMatrix = modelMatrix * rotationMatrix;

	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glBindVertexArray(gVao_pyramid);
        glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);

	glUseProgram(0);
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    if(gbAnimate==true){
        gAngle=gAngle+1.0f;
        if(gAngle>=360.0f){
            gAngle=gAngle-360.0f;
        }
    }
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
        case 27: // Esc key
            [ self release];
            [NSApp terminate:self];
            break;
        case 'A':
        case 'a':
            if(bIsAKeyPressed==false){
                gbAnimate=true;
                bIsAKeyPressed=true;
            }else{
                gbAnimate=false;
                bIsAKeyPressed=false;
            }
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

-(void)mouseDown:(NSEvent *)theEvent{
    
}

-(void)mouseDragged:(NSEvent *)theEvent{
   
}

-(void)rightMouseDown:(NSEvent *)theEvent{
}

- (void) dealloc{

    if (gVao_pyramid){
        glDeleteVertexArrays(1, &gVao_pyramid);
        gVao_pyramid = 0;
    }
    
    if (gVbo_pyramid_position){
        glDeleteBuffers(1, &gVbo_pyramid_position);
        gVbo_pyramid_position = 0;
    }
    
    if (gVbo_pyramid_normal){
        glDeleteBuffers(1, &gVbo_pyramid_normal);
        gVbo_pyramid_normal = 0;
    }

    glDetachShader(gShaderProgramObject, gVertexShaderObject);
    
    glDetachShader(gShaderProgramObject, gFragmentShaderObject);
    
    glDeleteShader(gVertexShaderObject);
    gVertexShaderObject = 0;
    glDeleteShader(gFragmentShaderObject);
    gFragmentShaderObject = 0;
    
    
    glDeleteProgram(gShaderProgramObject);
    gShaderProgramObject = 0;

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
