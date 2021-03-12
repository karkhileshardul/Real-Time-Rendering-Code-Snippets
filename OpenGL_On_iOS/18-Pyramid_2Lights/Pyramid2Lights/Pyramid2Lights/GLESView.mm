//
//  GLESView.m
//  Pyramid2Lights
//
//  Created by DATTATRAY BADHE on 22/05/1940 Saka.
//



#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import "GLESView.h"
#import "vmath.h"

enum{
    SSK_ATTRIBUTE_VERTEX = 0,
    SSK_ATTRIBUTE_COLOR,
    SSK_ATTRIBUTE_NORMAL,
    SSK_ATTRIBUTE_TEXTURE0,
};

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
bool bIsAKeyPressed = false;
bool bIsLKeyPressed = false;

@implementation GLESView{
    EAGLContext *eaglContext;
    
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
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

    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
    
    vmath::mat4 perspectiveProjectionMatrix;
}


-(id)initWithFrame:(CGRect)frame;{
    // code
    self=[super initWithFrame:frame];
    
    if(self){
        CAEAGLLayer *eaglLayer=(CAEAGLLayer *)super.layer;
        eaglLayer.opaque=YES;
        eaglLayer.drawableProperties=[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE],
                                      kEAGLDrawablePropertyRetainedBacking,kEAGLColorFormatRGBA8,kEAGLDrawablePropertyColorFormat,nil];
        
        eaglContext=[[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if(eaglContext==nil){
            [self release];
            return(nil);
        }
        [EAGLContext setCurrentContext:eaglContext];
        
        glGenFramebuffers(1,&defaultFramebuffer);
        glGenRenderbuffers(1,&colorRenderbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
        
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,colorRenderbuffer);
        
        GLint backingWidth;
        GLint backingHeight;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_WIDTH,&backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_HEIGHT,&backingHeight);
        
        glGenRenderbuffers(1,&depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER,depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,backingWidth,backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRenderbuffer);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){
            printf("Failed To Create Complete Framebuffer Object %x\n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
            glDeleteFramebuffers(1,&defaultFramebuffer);
            glDeleteRenderbuffers(1,&colorRenderbuffer);
            glDeleteRenderbuffers(1,&depthRenderbuffer);
            
            return(nil);
        }
        
        printf("Renderer : %s | GL Version : %s | GLSL Version : %s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION),glGetString(GL_SHADING_LANGUAGE_VERSION));
        
        // hard coded initializations
        isAnimating=NO;
        animationFrameInterval=60; // default since iOS 8.2
        
        // *** VERTEX SHADER ***
        // create shader
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
        // provide source code to shader
        const GLchar *vertexShaderSourceCode =
        "#version 300 es" \
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
        
      
        glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
        
        // compile shader
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
                    printf("Vertex Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        
        iInfoLogLength = 0;
        iShaderCompiledStatus = 0;
        szInfoLog = NULL;
        
        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        
        const GLchar *fragmentShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "precision highp float;" \
        "precision highp int;" \
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
                    printf("Fragment Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
                    printf("Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        model_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
        view_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
        projection_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
        
        L_KeyPressed_uniform = glGetUniformLocation(shaderProgramObject, "u_lighting_enabled");
        
        La_uniform_one = glGetUniformLocation(shaderProgramObject, "u_La_one");
        Ld_uniform_one = glGetUniformLocation(shaderProgramObject, "u_Ld_one");
        Ls_uniform_one = glGetUniformLocation(shaderProgramObject, "u_Ls_one");
        light_position_uniform_one = glGetUniformLocation(shaderProgramObject, "u_light_position_one");
        
        Ka_uniform = glGetUniformLocation(shaderProgramObject, "u_Ka");
        Kd_uniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
        Ks_uniform = glGetUniformLocation(shaderProgramObject, "u_Ks");
        
        
        La_uniform_two = glGetUniformLocation(shaderProgramObject, "u_La_two");
        Ld_uniform_two = glGetUniformLocation(shaderProgramObject, "u_Ld_two");
        Ls_uniform_two = glGetUniformLocation(shaderProgramObject, "u_Ls_two");
        light_position_uniform_two = glGetUniformLocation(shaderProgramObject, "u_light_position_two");
        
        material_shininess_uniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
        
        
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
        
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gbAnimate = false;
        gbLight = false;
        perspectiveProjectionMatrix = vmath::mat4::identity();
        
        UITapGestureRecognizer *singleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1]; // 1 finger touch
        
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1]; // touch of 1 finger
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        UISwipeGestureRecognizer *swipeGestureRecognizer=[[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        UILongPressGestureRecognizer *longPressGestureRecognizer=[[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [self addGestureRecognizer:longPressGestureRecognizer];
    }
    return(self);
}


+(Class)layerClass{
    return([CAEAGLLayer class]);
}

-(void)drawView:(id)sender{
    vmath::mat4 modelMatrix;
    vmath::mat4 rotationMatrix;
    vmath::mat4 viewMatrix;
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
    
   
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
    
    if(gbAnimate==true){
        gAngle=gAngle+1.0f;
        if(gAngle>=360.0f){
            gAngle=gAngle-360.0f;
        }
    }
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}

-(void)layoutSubviews{
    GLint width;
    GLint height;
    
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_WIDTH,&width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_HEIGHT,&height);
    
    glGenRenderbuffers(1,&depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRenderbuffer);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("Failed To Create Complete Framebuffer Object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    
    glViewport(0,0,(GLsizei)width,(GLsizei)height);
    
    GLfloat fwidth = (GLfloat)width;
    GLfloat fheight = (GLfloat)height;
    perspectiveProjectionMatrix = vmath::perspective(45.0f,fwidth/fheight,0.1f,100.0f);
    
    [self drawView:nil];
}

-(void)startAnimation{
    if (!isAnimating){
        displayLink=[NSClassFromString(@"CADisplayLink")displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFramesPerSecond:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        isAnimating=YES;
    }
}

-(void)stopAnimation{
    if(isAnimating){
        [displayLink invalidate];
        displayLink=nil;
        isAnimating=NO;
    }
}


-(BOOL)acceptsFirstResponder{
    return(YES);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event{
    
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr{
    if (!bIsLKeyPressed) {
        gbLight = true;
        bIsLKeyPressed = true;
    }
    else {
        gbLight = false;
        bIsLKeyPressed = false;
    }
    [self drawView:nil];
    
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr{
    if(bIsAKeyPressed==false){
        gbAnimate=true;
        bIsAKeyPressed=true;
    }else{
        gbAnimate=false;
        bIsAKeyPressed=false;
    }
    [self drawView:nil];
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr{
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr{
    
}

- (void)dealloc{
   
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
    

    glDetachShader(shaderProgramObject, vertexShaderObject);
    glDetachShader(shaderProgramObject, fragmentShaderObject);
    glDeleteShader(vertexShaderObject);
    vertexShaderObject = 0;
    glDeleteShader(fragmentShaderObject);
    fragmentShaderObject = 0;
    
    glDeleteProgram(shaderProgramObject);
    shaderProgramObject = 0;
    
    if(depthRenderbuffer){
        glDeleteRenderbuffers(1,&depthRenderbuffer);
        depthRenderbuffer=0;
    }
    
    if(colorRenderbuffer){
        glDeleteRenderbuffers(1,&colorRenderbuffer);
        colorRenderbuffer=0;
    }
    
    if(defaultFramebuffer){
        glDeleteFramebuffers(1,&defaultFramebuffer);
        defaultFramebuffer=0;
    }
    
    if([EAGLContext currentContext]==eaglContext){
        [EAGLContext setCurrentContext:nil];
    }
    [eaglContext release];
    eaglContext=nil;
    
    [super dealloc];
}



@end
