#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>
#import "vmath.h"
#import "Sphere.h"

enum{
    SSK_ATTRIBUTE_VERTEX = 0,
    SSK_ATTRIBUTE_COLOR,
    SSK_ATTRIBUTE_NORMAL,
    SSK_ATTRIBUTE_TEXTURE0,
};

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef,const CVTimeStamp *,const CVTimeStamp *,CVOptionFlags,CVOptionFlags *,void *);

FILE *gpFile=NULL;

float sphere_normals[1146];
float sphere_vertices[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

GLfloat lightAmbient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat lightSpecular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat lightPosition[]={100.0f,100.0f,100.0f,1.0f};
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
    [window setTitle:@"17-Toggling per_vertex and per_fragment Sphere Window"];
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
    
    GLuint gVao_sphere;
    GLuint gVbo_sphere_position;
    GLuint gVbo_sphere_normal;
    GLuint gVbo_sphere_element;
    
    GLuint model_matrix_uniform,view_matrix_uniform,projection_matrix_uniform;
    bool gbLight;
    GLuint L_KeyPressed_uniform;
    GLuint V_KeyPressed_uniform;
    GLuint F_KeyPressed_uniform;

    GLuint gNumElements;
    GLuint gNumVertices;
    bool gbLightPerVertex;
    bool gbLightPerFragment;
    bool bIsVKeyPressed;
    bool bIsLKeyPressed;
    bool bIsFKeyPressed;
    GLuint La_uniform;
    GLuint Ld_uniform;
    GLuint Ls_uniform;
    GLuint light_position_uniform;
    GLuint Ka_uniform;
    GLuint Kd_uniform;
    GLuint Ks_uniform;
    GLuint material_shininess_uniform;
    vmath::mat4 perspectiveProjectionMatrix;
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
   
    /*___(Vertex Shader)_START___*/
    const GLchar *vertexShaderSourceCode =
    "#version 410" \
    "\n" \
    "in vec4 vPosition;" \
    "in vec3 vNormal;" \
    "uniform mat4 u_model_matrix;" \
    "uniform mat4 u_view_matrix;" \
    "uniform mat4 u_projection_matrix;" \
    "uniform int u_lighting_enabled;" \
    "uniform vec3 u_La;" \
    "uniform vec3 u_Ld;" \
    "uniform vec3 u_Ls;" \
    "uniform vec3 u_light_position;" \
    "uniform vec3 u_Ka;" \
    "uniform vec3 u_Kd;" \
    "uniform vec3 u_Ks;" \
    "uniform float u_material_shininess;" \
    "out vec3 phong_ads_color;" \
    "uniform int u_per_vertex_lighting_enabled;" \
    "uniform int u_per_fragment_lighting_enabled;" \
    "out vec3 transformed_normals;" \
    "out vec3 light_direction;" \
    "out vec3 viewer_vector;" \
    "void main(void)" \
    "{" \
        "if(u_lighting_enabled==1)" \
        "{" \
            "if(u_per_vertex_lighting_enabled==1)" \
            "{"\
                "vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
                "vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
                "vec3 light_direction=normalize(vec3(u_light_position) - eye_coordinates.xyz);" \
                "float tn_dot_ld=max(dot(transformed_normals,light_direction),0.0);" \
                "vec3 ambient=u_La * u_Ka;" \
                "vec3 diffuse= u_Ld * u_Kd *tn_dot_ld;" \
                "vec3 reflection_vector=reflect(-light_direction,transformed_normals);" \
                "vec3 viewer_vector=normalize(-eye_coordinates.xyz);" \
                "vec3 specular= u_Ls * u_Ks * pow(max(dot(reflection_vector,viewer_vector),0.0),u_material_shininess);" \
                "phong_ads_color=ambient + diffuse + specular;" \
            "}" \
            "else if(u_per_fragment_lighting_enabled==1)" \
            "{" \
                "vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
                "transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
                "light_direction = vec3(u_light_position) - eye_coordinates.xyz;" \
                "viewer_vector = -eye_coordinates.xyz;" \
            "}" \
        "}" \
        "else" \
        "{" \
            "phong_ads_color=vec3(1.0,1.0,1.0f);" \
        "}" \
        "gl_Position=u_projection_matrix *u_view_matrix *u_model_matrix * vPosition;" \
    "}";
    
    /*___(Vertex Shader)_END___*/
    
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
    /*___(Fragment Shader)_START___*/
    const GLchar *fragmentShaderSourceCode =
    "#version 410" \
    "\n" \
    "in vec3 transformed_normals;" \
    "in vec3 light_direction;" \
    "in vec3 viewer_vector;" \
    "in vec3 phong_ads_color;" \
    "out vec4 FragColor;" \
    "uniform vec3 u_La;" \
    "uniform vec3 u_Ld;" \
    "uniform vec3 u_Ls;" \
    "uniform vec3 u_Ka;" \
    "uniform vec3 u_Kd;" \
    "uniform vec3 u_Ks;" \
    "uniform float u_material_shininess;" \
    "uniform int u_lighting_enabled;" \
    "uniform int u_per_vertex_lighting_enabled;" \
    "uniform int u_per_fragment_lighting_enabled;" \
    "void main(void)" \
    "{" \
        "if(u_lighting_enabled==1)" \
        "{" \
            "if(u_per_vertex_lighting_enabled==1)" \
            "{" \
                "FragColor = vec4(phong_ads_color, 1.0);" \
            "}"\
            "else if(u_per_fragment_lighting_enabled==1)" \
            "{" \
                "vec3 normalized_transformed_normals=normalize(transformed_normals);" \
                "vec3 normalized_light_direction=normalize(light_direction);" \
                "vec3 normalized_viewer_vector=normalize(viewer_vector);" \
                "vec3 ambient = u_La * u_Ka;" \
                "float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);" \
                "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
                "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
                "vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" \
                "vec3 phong_ads_color=ambient + diffuse + specular;" \
                "FragColor = vec4(phong_ads_color, 1.0);" \
            "}" \
        "}" \
        "else" \
        "{" \
            "vec3 phong_ads_color = vec3(1.0, 1.0, 1.0);" \
            "FragColor = vec4(phong_ads_color, 1.0);" \
        "}" \
    "}";
    
    /*___(Fragment Shader)_END___*/
    
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

    
    model_matrix_uniform=glGetUniformLocation(shaderProgramObject,
                                              "u_model_matrix");
    view_matrix_uniform=glGetUniformLocation(shaderProgramObject,
                                             "u_view_matrix");
    projection_matrix_uniform=glGetUniformLocation(shaderProgramObject,
                                                   "u_projection_matrix");
    
    L_KeyPressed_uniform=glGetUniformLocation(shaderProgramObject,"u_lighting_enabled");
    V_KeyPressed_uniform = glGetUniformLocation(shaderProgramObject, "u_per_vertex_lighting_enabled");
    F_KeyPressed_uniform = glGetUniformLocation(shaderProgramObject, "u_per_fragment_lighting_enabled");
    
    
    //ambient color intensity of light
    La_uniform=glGetUniformLocation(shaderProgramObject,"u_La");
    //diffuse color intensity of light
    Ld_uniform=glGetUniformLocation(shaderProgramObject,"u_Ld");
    //specular color intensity of light
    Ls_uniform=glGetUniformLocation(shaderProgramObject,"u_Ls");
    //position of light
    light_position_uniform=glGetUniformLocation(shaderProgramObject,"u_light_position");
    
    //ambient reflective color intensity of material
    Ka_uniform = glGetUniformLocation(shaderProgramObject, "u_Ka");
    //diffuse reflective color intensity of material
    Kd_uniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
    //specular reflective color intensity of material
    Ks_uniform = glGetUniformLocation(shaderProgramObject, "u_Ks");
    //shininess of material
    material_shininess_uniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");

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

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    perspectiveProjectionMatrix = vmath::mat4::identity();
    
    bIsVKeyPressed=false;
    bIsLKeyPressed=false;
    bIsFKeyPressed = false;
    gbLightPerVertex = false;
    gbLightPerFragment = false;
    

    
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
    
   [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
            GL_STENCIL_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
    
    if (gbLightPerVertex == true){
        glUniform1i(L_KeyPressed_uniform, 1);
        glUniform1i(V_KeyPressed_uniform, 1);
        glUniform1i(F_KeyPressed_uniform, 0);
        
        glUniform3fv(La_uniform, 1, lightAmbient);
        glUniform3fv(Ld_uniform, 1, lightDiffuse);
        glUniform3fv(Ls_uniform, 1, lightSpecular);
        glUniform4fv(light_position_uniform, 1, lightPosition);
        
        glUniform3fv(Ka_uniform, 1, material_ambient);
        glUniform3fv(Kd_uniform, 1, material_diffuse);
        glUniform3fv(Ks_uniform, 1, material_specular);
        glUniform1f(material_shininess_uniform, material_shininess);
    }else if (gbLightPerFragment == true){
        glUniform1i(L_KeyPressed_uniform, 1);
        glUniform1i(F_KeyPressed_uniform, 1);
        glUniform1i(V_KeyPressed_uniform, 0);
        
        glUniform3fv(La_uniform, 1, lightAmbient);
        glUniform3fv(Ld_uniform, 1, lightDiffuse);
        glUniform3fv(Ls_uniform, 1, lightSpecular);
        glUniform4fv(light_position_uniform, 1, lightPosition);
        
        glUniform3fv(Ka_uniform, 1, material_ambient);
        glUniform3fv(Kd_uniform, 1, material_diffuse);
        glUniform3fv(Ks_uniform, 1, material_specular);
        glUniform1f(material_shininess_uniform, material_shininess);
    }else{
        glUniform1i(L_KeyPressed_uniform, 0);
        glUniform1i(V_KeyPressed_uniform, 0);
        glUniform1i(F_KeyPressed_uniform, 0);
    }
    
    vmath::mat4 modelMatrix =vmath:: mat4::identity();
    vmath::mat4 viewMatrix =vmath::mat4::identity();
    modelMatrix=vmath::translate(0.0f,0.0f,-2.0f);
    
    glUniformMatrix4fv(model_matrix_uniform,1,GL_FALSE,modelMatrix);
    glUniformMatrix4fv(view_matrix_uniform,1,GL_FALSE,viewMatrix);
    glUniformMatrix4fv(projection_matrix_uniform,1,
                       GL_FALSE,perspectiveProjectionMatrix);
    
    glBindVertexArray(gVao_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,gVbo_sphere_element);
    glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
    glBindVertexArray(0);
    

	glUseProgram(0);
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
}

-(BOOL)acceptsFirstResponder{
    [[self window]makeFirstResponder:self];
    return(YES);
}

-(void)keyDown:(NSEvent *)theEvent{
    int key=(int)[[theEvent characters]characterAtIndex:0];

    switch(key){
        case 27:
            [[self window]toggleFullScreen:self];
            break;
        case 'F':
        case 'f':
            if (bIsFKeyPressed == false){
                gbLightPerFragment = true;
                gbLightPerVertex = false;
                bIsFKeyPressed = true;
            }else{
                gbLightPerFragment = false;
                gbLightPerVertex = false;
                bIsFKeyPressed = false;
            }
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
        case 'q':
        case 'Q':
            [ self release];
            [NSApp terminate:self];
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
