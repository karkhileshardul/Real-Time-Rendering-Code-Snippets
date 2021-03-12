#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>
#import "vmath.h"
#import"Sphere.h"

enum{
    SSK_ATTRIBUTE_VERTEX = 0,
    SSK_ATTRIBUTE_COLOR,
    SSK_ATTRIBUTE_NORMAL,
    SSK_ATTRIBUTE_TEXTURE0,
};

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef,const CVTimeStamp *,const CVTimeStamp *,CVOptionFlags,CVOptionFlags *,void *);

FILE *gpFile=NULL;
bool gbLightPerVertex=false;
bool gbLightPerFragment=false;
GLfloat gAngle=0.0f;

GLfloat lightAmbient_red[]={0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse_red[]={1.0f,0.0f,0.0f,1.0f};
GLfloat lightSpecular_red[]={1.0f,0.0f,0.0f,1.0f};
GLfloat lightPosition_red[]={-2.0f,1.0f,1.0f,1.0f};

GLfloat lightAmbient_green[]={0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse_green[]={0.0f,1.0f,0.0f,1.0f};
GLfloat lightSpecular_green[]={0.0f,1.0f,0.0f,1.0f};
GLfloat lightPosition_green[]={2.0f,1.0f,1.0f,1.0f};

GLfloat lightAmbient_blue[]={0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse_blue[]={0.0f,0.0f,1.0f,1.0f};
GLfloat lightSpecular_blue[]={0.0f,0.0f,1.0f,1.0f};
GLfloat lightPosition_blue[]={0.0f,0.0f,1.0f,1.0f};
/*Lights Properties (___END___) */

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
        printf("Can Not Create Log File.\nExitting...\n");
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
    [window setTitle:@"04-Perspective Triangle Window"];
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
    
    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;
    GLuint gShaderProgramObject;
    
    GLuint gVao_sphere;
    GLuint gVbo_sphere_position;
    GLuint gVbo_sphere_normal;
    GLuint gVbo_sphere_element;
    
    float sphere_normals[1146];
    float sphere_vertices[1146];
    float sphere_textures[764];
    unsigned short sphere_elements[2280];
    GLuint gNumElements;
    GLuint gNumVertices;
    
    GLuint gMVPUniform;
    
    GLuint model_matrix_uniform,projection_matrix_uniform,view_matrix_uniform;
    GLuint gL_KeyPressed_uniform;
    
    GLuint F_KeyPressed_uniform;
    GLuint V_KeyPressed_uniform;
    
    GLuint La_uniform_red;
    GLuint Ld_uniform_red;
    GLuint Ls_uniform_red;
    GLuint light_position_uniform_red;
    
    GLuint La_uniform_green;
    GLuint Ld_uniform_green;
    GLuint Ls_uniform_green;
    GLuint light_position_uniform_green;
    
    GLuint La_uniform_blue;
    GLuint Ld_uniform_blue;
    GLuint Ls_uniform_blue;
    GLuint light_position_uniform_blue;
    
    GLuint Ka_uniform;
    GLuint Kd_uniform;
    GLuint Ks_uniform;
    GLuint material_shininess_uniform;
    
    vmath::mat4 gPerspectiveProjectionMatrix;}

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
            NSOpenGLPFADoubleBuffer,
            0}; // last 0 is must
        
        NSOpenGLPixelFormat *pixelFormat=[[[NSOpenGLPixelFormat alloc]initWithAttributes:attrs] autorelease];
        
        if(pixelFormat==nil){
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
    "uniform int u_per_vertex_lighting_enabled;" \
    "uniform int u_per_fragment_lighting_enabled;" \
    "uniform vec3 u_La_red;" \
    "uniform vec3 u_Ld_red;" \
    "uniform vec3 u_Ls_red;" \
    "uniform vec4 u_light_position_red;" \
    "uniform vec3 u_La_green;" \
    "uniform vec3 u_Ld_green;" \
    "uniform vec3 u_Ls_green;" \
    "uniform vec4 u_light_position_green;" \
    "uniform vec3 u_La_blue;" \
    "uniform vec3 u_Ld_blue;" \
    "uniform vec3 u_Ls_blue;" \
    "uniform vec4 u_light_position_blue;" \
    "uniform vec3 u_Ka;" \
    "uniform vec3 u_Kd;" \
    "uniform vec3 u_Ks;" \
    "uniform float u_material_shininess;" \
    "vec3 phong_ads_color;" \
    "out vec3 out_phong_ads_color;" \
    "out vec3 transformed_normals;" \
    "out vec3 light_position_red;" \
    "out vec3 light_position_green;" \
    "out vec3 light_position_blue;" \
    "out vec3 viewer_vector;" \
    "vec3 SettingLightProperties(vec3 La, vec3 Ld, vec3 Ls, vec4 light_position)" \
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
    "if(u_per_vertex_lighting_enabled==1)" \
    "{" \
    "vec3 red_light=SettingLightProperties(u_La_red,u_Ld_red,u_Ls_red,u_light_position_red);" \
    "vec3 green_light=SettingLightProperties(u_La_green,u_Ld_green,u_Ls_green,u_light_position_green);" \
    "vec3 blue_light=SettingLightProperties(u_La_blue,u_Ld_blue,u_Ls_blue,u_light_position_blue);" \
    "out_phong_ads_color=red_light+green_light+blue_light;" \
    "}" \
    "if(u_per_fragment_lighting_enabled==1)" \
    "{" \
    "vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
    "transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
    "light_position_red = vec3(u_light_position_red) - eye_coordinates.xyz;" \
    "light_position_green = vec3(u_light_position_green) - eye_coordinates.xyz;" \
    "light_position_blue = vec3(u_light_position_blue) - eye_coordinates.xyz;" \
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
    "in vec3 viewer_vector;" \
    "in vec3 transformed_normals;" \
    "in vec3 out_phong_ads_color;" \
    "in vec3 light_position_red;" \
    "in vec3 light_position_green;" \
    "in vec3 light_position_blue;" \
    "out vec4 FragColor;" \
    "uniform vec3 u_La_red;" \
    "uniform vec3 u_Ld_red;" \
    "uniform vec3 u_Ls_red;" \
    "uniform vec3 u_La_green;" \
    "uniform vec3 u_Ld_green;" \
    "uniform vec3 u_Ls_green;" \
    "uniform vec3 u_La_blue;" \
    "uniform vec3 u_Ld_blue;" \
    "uniform vec3 u_Ls_blue;" \
    "uniform vec3 u_Ka;" \
    "uniform vec3 u_Kd;" \
    "uniform vec3 u_Ks;" \
    "uniform float u_material_shininess;" \
    "uniform int u_per_vertex_lighting_enabled;" \
    "uniform int u_per_fragment_lighting_enabled;" \
    "vec3 SettingLightProperties(vec3 u_La, vec3 u_Ld, vec3 u_Ls, vec3 light_direction)" \
    "{" \
    "vec3 phong_ads_color;" \
    "vec3 normalized_transformed_normals=normalize(transformed_normals);" \
    "vec3 normalized_light_direction=normalize(light_direction);" \
    "vec3 normalized_viewer_vector=normalize(viewer_vector);" \
    "vec3 ambient = u_La * u_Ka;" \
    "float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);" \
    "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
    "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
    "vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" \
    "phong_ads_color=ambient + diffuse + specular;" \
    "return phong_ads_color;" \
    "}" \
    "void main(void)" \
    "{" \
    "if(u_per_vertex_lighting_enabled==1)" \
    "{" \
    "FragColor = vec4(out_phong_ads_color, 1.0);" \
    "}" \
    "if(u_per_fragment_lighting_enabled==1)" \
    "{" \
    "vec3 red_light=SettingLightProperties(u_La_red,u_Ld_red,u_Ls_red,light_position_red);" \
    "vec3 green_light=SettingLightProperties(u_La_green,u_Ld_green,u_Ls_green,light_position_green);" \
    "vec3 blue_light=SettingLightProperties(u_La_blue,u_Ld_blue,u_Ls_blue,light_position_blue);" \
    "vec3 phong_ads_color=red_light+green_light+blue_light;" \
    "FragColor = vec4(phong_ads_color, 1.0);" \
    "}" \
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
    glBindAttribLocation(gShaderProgramObject,SSK_ATTRIBUTE_NORMAL,"vNormal");
    
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
    
    model_matrix_uniform=glGetUniformLocation(gShaderProgramObject,
                                              "u_model_matrix");
    view_matrix_uniform=glGetUniformLocation(gShaderProgramObject,
                                             "u_view_matrix");
    projection_matrix_uniform=glGetUniformLocation(gShaderProgramObject,
                                                   "u_projection_matrix");
    
    V_KeyPressed_uniform=glGetUniformLocation(gShaderProgramObject,"u_per_vertex_lighting_enabled");
    F_KeyPressed_uniform=glGetUniformLocation(gShaderProgramObject,"u_per_fragment_lighting_enabled");
    
    La_uniform_red=glGetUniformLocation(gShaderProgramObject,"u_La_red");
    Ld_uniform_red=glGetUniformLocation(gShaderProgramObject,"u_Ld_red");
    Ls_uniform_red=glGetUniformLocation(gShaderProgramObject,"u_Ls_red");
    light_position_uniform_red=glGetUniformLocation(gShaderProgramObject,"u_light_position_red");
    
    La_uniform_green=glGetUniformLocation(gShaderProgramObject,"u_La_green");
    Ld_uniform_green=glGetUniformLocation(gShaderProgramObject,"u_Ld_green");
    Ls_uniform_green=glGetUniformLocation(gShaderProgramObject,"u_Ls_green");
    light_position_uniform_green=glGetUniformLocation(gShaderProgramObject,"u_light_position_green");
    
    La_uniform_blue=glGetUniformLocation(gShaderProgramObject,"u_La_blue");
    Ld_uniform_blue=glGetUniformLocation(gShaderProgramObject,"u_Ld_blue");
    Ls_uniform_blue=glGetUniformLocation(gShaderProgramObject,"u_Ls_blue");
    light_position_uniform_blue=glGetUniformLocation(gShaderProgramObject,"u_light_position_blue");
    
    
    Ka_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
    Kd_uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
    Ks_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
    material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
    
   
    Sphere *sphere= new Sphere();
    sphere->getSphereVertexData(sphere_vertices,sphere_normals,sphere_textures,sphere_elements);
    gNumVertices=sphere->getNumberOfSphereVertices();
    gNumElements=sphere->getNumberOfSphereElements();
    
    
    glGenVertexArrays(1, &gVao_sphere);
    glBindVertexArray(gVao_sphere);
    glGenBuffers(1, &gVbo_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER,gVbo_sphere_position);
    glBufferData(GL_ARRAY_BUFFER,sizeof(sphere_vertices),
                 sphere_vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX,3,
                          GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    
    glGenBuffers(1, &gVbo_sphere_normal);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals),
                 sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL,
                          3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    
    glGenBuffers(1, &gVbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,gVbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(sphere_elements),
                 sphere_elements,GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gPerspectiveProjectionMatrix = vmath::mat4::identity();
    
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
    
    gPerspectiveProjectionMatrix = vmath::perspective(45.0f, width/height, 0.1f, 100.0f);
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect{
    [self drawView];
}

- (void)drawView{
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
            GL_STENCIL_BUFFER_BIT);
    
    glUseProgram(gShaderProgramObject);
    if(gbLightPerFragment==true){
        //set 'u_lighting_enabled' uniform
        glUniform1i(F_KeyPressed_uniform, 1);
        glUniform1i(V_KeyPressed_uniform, 0);
        
        glUniform3fv(La_uniform_red, 1, lightAmbient_red);
        glUniform3fv(Ld_uniform_red, 1, lightDiffuse_red);
        glUniform3fv(Ls_uniform_red, 1, lightSpecular_red);
        lightPosition_red[0] = cos(3.1415*gAngle/180.0f)*10.0f;
        lightPosition_red[1] = 0.0f;
        lightPosition_red[2] = sin(3.1415*gAngle/180.0f)*10.0f;
        glUniform4fv(light_position_uniform_red, 1, lightPosition_red);
        
        glUniform3fv(La_uniform_green, 1, lightAmbient_green);
        glUniform3fv(Ld_uniform_green, 1, lightDiffuse_green);
        glUniform3fv(Ls_uniform_green, 1, lightSpecular_green);
        lightPosition_green[0] = 0.0f;
        lightPosition_green[1] = cos(3.1415*gAngle/180.0f)*10.0f;
        lightPosition_green[2] = sin(3.1415*gAngle/180.0f)*10.0f;
        glUniform4fv(light_position_uniform_green, 1, lightPosition_green);
        
        glUniform3fv(La_uniform_blue, 1, lightAmbient_blue);
        glUniform3fv(Ld_uniform_blue, 1, lightDiffuse_blue);
        glUniform3fv(Ls_uniform_blue, 1, lightSpecular_blue);
        lightPosition_blue[0] = cos(3.1415*gAngle/180.0f)*10.0f;
        lightPosition_blue[1] = sin(3.1415*gAngle/180.0f)*10.0f;
        lightPosition_blue[2] = 0.0f;
        glUniform4fv(light_position_uniform_blue, 1, lightPosition_blue);
        /*setting light's properties (___END___) */
        
        // setting material's properties
        glUniform3fv(Ka_uniform, 1, material_ambient);
        glUniform3fv(Kd_uniform, 1, material_diffuse);
        glUniform3fv(Ks_uniform, 1, material_specular);
        glUniform1f(material_shininess_uniform, material_shininess);
        
    }else{
        glUniform1i(F_KeyPressed_uniform, 0);
        glUniform1i(V_KeyPressed_uniform, 1);
        
        glUniform3fv(La_uniform_red, 1, lightAmbient_red);
        glUniform3fv(Ld_uniform_red, 1, lightDiffuse_red);
        glUniform3fv(Ls_uniform_red, 1, lightSpecular_red);
        lightPosition_red[0] = cos(3.1415*gAngle/180.0f)*10.0f;
        lightPosition_red[1] = 0.0f;
        lightPosition_red[2] = sin(3.1415*gAngle/180.0f)*10.0f;
        glUniform4fv(light_position_uniform_red, 1, lightPosition_red);
        
        glUniform3fv(La_uniform_green, 1, lightAmbient_green);
        glUniform3fv(Ld_uniform_green, 1, lightDiffuse_green);
        glUniform3fv(Ls_uniform_green, 1, lightSpecular_green);
        lightPosition_green[0] = 0.0f;
        lightPosition_green[1] = cos(3.1415*gAngle/180.0f)*10.0f;
        lightPosition_green[2] = sin(3.1415*gAngle/180.0f)*10.0f;
        glUniform4fv(light_position_uniform_green, 1, lightPosition_green);
        
        glUniform3fv(La_uniform_blue, 1, lightAmbient_blue);
        glUniform3fv(Ld_uniform_blue, 1, lightDiffuse_blue);
        glUniform3fv(Ls_uniform_blue, 1, lightSpecular_blue);
        lightPosition_blue[0] = cos(3.1415*gAngle/180.0f)*10.0f;
        lightPosition_blue[1] = sin(3.1415*gAngle/180.0f)*10.0f;
        lightPosition_blue[2] = 0.0f;
        glUniform4fv(light_position_uniform_blue, 1, lightPosition_blue);
        /*setting light's properties (___END___) */
        
        // setting material's properties
        glUniform3fv(Ka_uniform, 1, material_ambient);
        glUniform3fv(Kd_uniform, 1, material_diffuse);
        glUniform3fv(Ks_uniform, 1, material_specular);
        glUniform1f(material_shininess_uniform, material_shininess);
    }
    
    vmath::mat4 modelMatrix =vmath:: mat4::identity();
    vmath::mat4 viewMatrix =vmath::mat4::identity();
    modelMatrix=vmath::translate(0.0f,0.0f,-2.0f);
    
    glUniformMatrix4fv(model_matrix_uniform,1,GL_FALSE,modelMatrix);
    glUniformMatrix4fv(view_matrix_uniform,1,GL_FALSE,viewMatrix);
    glUniformMatrix4fv(projection_matrix_uniform,1,
                       GL_FALSE,gPerspectiveProjectionMatrix);
    
    glBindVertexArray(gVao_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
    glBindVertexArray(0);
    
    glUseProgram(0);
    
    gAngle=gAngle+1.0f;
    if (gAngle >= 360.0f){
        gAngle = 0.0f;
    }

    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(BOOL)acceptsFirstResponder{
    [[self window]makeFirstResponder:self];
    return(YES);
}

-(void)keyDown:(NSEvent *)theEvent{
    static bool bIsVKeyPressed=false;
    static bool bIsFKeyPressed=false;
    int key=(int)[[theEvent characters]characterAtIndex:0];
    switch(key){
        case 27:
            [[self window]toggleFullScreen:self];
            break;
        case 'q':
        case 'Q':
            [ self release];
            [NSApp terminate:self];
            break;
        case 'F':
        case 'f':
            if(bIsFKeyPressed==false){
                gbLightPerVertex = false;
                gbLightPerFragment = true;
                bIsFKeyPressed = true;
            }else{
                gbLightPerVertex = false;
                gbLightPerFragment = false;
                bIsFKeyPressed = false;
            }
            break;
        case 'v':
        case 'V':
            if (bIsVKeyPressed == false){
                gbLightPerVertex = true;
                gbLightPerFragment = false;
                bIsVKeyPressed = true;
            }else{
                gbLightPerVertex = false;
                gbLightPerFragment = false;
                bIsVKeyPressed = false;
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
    if (gVao_sphere){
        glDeleteVertexArrays(1, &gVao_sphere);
        gVao_sphere = 0;
    }
    
    if (gVbo_sphere_position){
        glDeleteBuffers(1, &gVbo_sphere_position);
        gVbo_sphere_position = 0;
    }
    
    if (gVbo_sphere_normal){
        glDeleteBuffers(1, &gVbo_sphere_normal);
        gVbo_sphere_normal = 0;
    }
    
    if (gVbo_sphere_element){
        glDeleteBuffers(1, &gVbo_sphere_element);
        gVbo_sphere_element = 0;
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

