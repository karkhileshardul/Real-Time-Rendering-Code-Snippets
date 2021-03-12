//
//  GLESView.m
//  SmileyTexture
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


@implementation GLESView{
    EAGLContext *eaglContext;
    
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    GLuint gVao_square;
    GLuint gVbo_square_position;
    GLuint gVbo_square_texture;
    GLuint mvpUniform;
    
    GLuint gTexture_sampler_uniform;
    GLuint gTexture_Smiley;

    
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
        "\n"\
        "in vec4 vPosition;"\
        "in vec2 vTexture0_Coord;"\
        "out vec2 out_texture0_coord;"\
        "uniform mat4 u_mvp_matrix;"\
        "void main(void)"\
        "{"\
        "gl_Position=u_mvp_matrix * vPosition;"\
        "out_texture0_coord=vTexture0_Coord;"\
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
        "in vec2 out_texture0_coord;"\
        "uniform sampler2D u_texture0_sampler;"\
        "out vec4 FragColor;"\
        "void main(void)"\
        "{"\
        "vec3 tex=vec3(texture(u_texture0_sampler,out_texture0_coord));"\
        "FragColor=vec4(tex,1.0f);"\
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
        glBindAttribLocation(shaderProgramObject, SSK_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
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
        
        mvpUniform = glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
        gTexture_sampler_uniform=glGetUniformLocation(shaderProgramObject,"u_texture0_sampler");
        
        gTexture_Smiley = [self loadTextureFromBMPFile:@"Smiley" :@"bmp"];
        
        const GLfloat squareVertices[]={
            1.0f,1.0f,0.0f,
            -1.0f,1.0f,0.0f,
            -1.0f,-1.0f,0.0f,
            1.0f,-1.0f,0.0f
        };
        const GLfloat squareTexcoords[]={
            1.0f,1.0f,
            0.0f,1.0f,
            0.0f,0.0f,
            1.0f,0.0f,
        };
        
        glGenVertexArrays(1,&gVao_square);
        glBindVertexArray(gVao_square);
        glGenBuffers(1,&gVbo_square_position);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_square_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices,GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX, 3,GL_FLOAT,GL_FALSE,0,NULL);
        glEnableVertexAttribArray (SSK_ATTRIBUTE_VERTEX);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        
        glGenBuffers(1,&gVbo_square_texture);
        glBindBuffer(GL_ARRAY_BUFFER,gVbo_square_texture);
        glBufferData(GL_ARRAY_BUFFER,sizeof(squareTexcoords),squareTexcoords,GL_STATIC_DRAW);
        glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0,2,GL_FLOAT,GL_FALSE,0,NULL);
        glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
        

        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
    
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
    
    vmath::mat4 modelViewMatrix;
    vmath::mat4 rotationMatrix;
    vmath::mat4 modelViewProjectionMatrix;
    
    modelViewMatrix=vmath::mat4::identity();
    modelViewMatrix=vmath::translate(-0.0f, 0.0f, -6.0f);
    
    modelViewProjectionMatrix= perspectiveProjectionMatrix *modelViewMatrix;
    
    glUniformMatrix4fv(mvpUniform,1,GL_FALSE,modelViewProjectionMatrix);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,gTexture_sampler_uniform);
    glBindTexture(GL_TEXTURE_2D,gTexture_Smiley);
    glUniform1i(gTexture_sampler_uniform,0);
    
    glBindVertexArray(gVao_square);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glBindVertexArray(0);
    glUseProgram(0);
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
    
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr{
    
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr{
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr{
    
}

- (void)dealloc{
    if(gVao_square){
        glDeleteVertexArrays(1, &gVao_square);
        gVao_square=0;
    }
    
    if(gVbo_square_position){
        glDeleteBuffers(1, &gVbo_square_position);
        gVbo_square_position=0;
    }
    
    if(gVbo_square_texture){
        glDeleteBuffers(1, &gVbo_square_texture);
        gVbo_square_texture=0;
    }
    
    if(gTexture_sampler_uniform){
        glDeleteTextures(1,&gTexture_sampler_uniform);
        gTexture_sampler_uniform=0;
    }
    
    if(gTexture_Smiley){
        glDeleteBuffers(1, &gTexture_Smiley);
        gTexture_Smiley=0;
    }
    
    if(gTexture_Smiley){
        glDeleteTextures(1,&gTexture_Smiley);
        gTexture_Smiley=0;
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



-(GLuint)loadTextureFromBMPFile:(NSString*)texFileName :(NSString*)extention{
    NSString *textureFileNameWithPath = [[NSBundle mainBundle]pathForResource:texFileName ofType:extention];
    
    UIImage *bmpImage=[[UIImage alloc]initWithContentsOfFile:textureFileNameWithPath];
    
    if (!bmpImage)
    {
        NSLog(@"Can't Find %@",textureFileNameWithPath);
        return(0);
    }
    
    CGImageRef cgImage=bmpImage.CGImage;
    
    int w=(int)CGImageGetWidth(cgImage);
    int h=(int)CGImageGetHeight(cgImage);
    
    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    
    void *pixels=(void*)CFDataGetBytePtr(imageData);
    
    GLuint bmpTexture;
    glGenTextures(1, &bmpTexture);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, bmpTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    CFRelease(imageData);
    
    return bmpTexture;
}

@end
