#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>
#import "vmath.h"

enum{
    SSK_ATTRIBUTE_VERTEX=0,
    SSK_ATTRIBUTE_COLOR,
    SSK_ATTRIBUTE_NORMAL,
    SSK_ATTRIBUTE_TEXTURE0,
};

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef,const CVTimeStamp *,const CVTimeStamp *,CVOptionFlags,CVOptionFlags *,void *);

FILE *gpFile=NULL;

@interface AppDelegate:NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

int main(int argc,const char *argv[]){
    NSAutoreleasePool *pPool=[[NSAutoreleasePool alloc]init];
    NSApp=[NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc]init]];
    [NSApp run];
    [pPool release];
    return (0);
}

@implementation AppDelegate{
@private
    NSWindow *window;
    GLView *glView;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification{
    NSBundle *mainBundle=[NSBundle mainBundle];
    NSString *appDirName=[mainBundle bundlePath];
    NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath=[NSString stringWithFormat:@"%@/Log.txt",parentDirPath];
    const char *pszLogFileNameWithPath=[logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    gpFile=fopen(pszLogFileNameWithPath,"w");
    if(gpFile==NULL){
        printf("Can Not Create Log File.\nExitting....\n");
        [self release];
        [NSApp terminate:self];
    }
    fprintf(gpFile,"Program Is Started Successfully\n");
    NSRect win_rect;
    win_rect=NSMakeRect(0.0,0.0,800.0,600.0);
    window=[[NSWindow alloc]initWithContentRect:win_rect
                            styleMask:NSWindowStyleMaskTitled |
                            NSWindowStyleMaskClosable |
                            NSWindowStyleMaskMiniaturizable |
                            NSWindowStyleMaskResizable
         backing:NSBackingStoreBuffered defer:NO];
    [window setTitle:@"11-Smiley Texture Window"];
    [window center];
    glView=[[GLView alloc]initWithFrame:win_rect];
        
    [window setContentView:glView];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

-(void)applicationWillTerminate:(NSNotification *)notification{
    fprintf(gpFile,"Program Is Terminated Successfully\n");
    if(gpFile){
        fclose(gpFile);
        gpFile=NULL;
    }
}

-(void)windowWillClose:(NSNotification *)notification{
    [NSApp terminate:self];
}

-(void)dealloc{
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

    GLuint gVao_square;
    GLuint gVbo_square_position;
    GLuint gVbo_square_texture;
    GLuint gMVPUniform;
    
    GLuint gTexture_sampler_uniform;
    GLuint gTexture_Smiley;
    
    vmath::mat4 perspectiveProjectionMatrix;
}

-(id)initWithFrame:(NSRect)frame;{
    self=[super initWithFrame:frame];
    if(self){
        [[self window]setContentView:self];
        NSOpenGLPixelFormatAttribute attrs[]={
            NSOpenGLPFAOpenGLProfile,
            NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAScreenMask,CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize,24,
            NSOpenGLPFADepthSize,24,
            NSOpenGLPFAAlphaSize,8,
            NSOpenGLPFADoubleBuffer,0};
        
        NSOpenGLPixelFormat *pixelFormat=[[[NSOpenGLPixelFormat alloc]initWithAttributes:attrs] autorelease];
        if(pixelFormat==nil){
            fprintf(gpFile,"No Valid OpenGl Pixel Format Is Available. Exitting...");
            [self release];
            [NSApp terminate:self];
        }
        
        NSOpenGLContext *glContext=[[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
        [self setPixelFormat:pixelFormat];
        [self setOpenGLContext:glContext];
    }
    return (self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime{
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc]init];
    [self drawView];
    [pool release];
    return (kCVReturnSuccess);
}

-(void)prepareOpenGL{
    fprintf(gpFile,"OpenGL Version: %s\n",glGetString(GL_VERSION));
    fprintf(gpFile,"GLSL Version:%s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    [[self openGLContext]makeCurrentContext];
    GLint swapInt=1;
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    gVertexShaderObject=glCreateShader(GL_VERTEX_SHADER);
    const GLchar *vertexShaderSourceCode=
    "#version 410"\
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
    
    glShaderSource(gVertexShaderObject,1,(const GLchar **)&vertexShaderSourceCode,NULL);
    glCompileShader(gVertexShaderObject);
    GLint iInfoLogLength=0;
    GLint iShaderCompileStatus=0;
    char *szInfoLog=NULL;
    glGetShaderiv(gVertexShaderObject,GL_COMPILE_STATUS,
                  &iShaderCompileStatus);
    if(iShaderCompileStatus==GL_FALSE){
        glGetShaderiv(gVertexShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
        if(iInfoLogLength>0){
            szInfoLog=(char *)malloc(iInfoLogLength);
            if(szInfoLog!=NULL){
                GLsizei written;
                glGetShaderInfoLog(gVertexShaderObject,iInfoLogLength,&written,szInfoLog);
                fprintf(gpFile,"Vertex Shader Compilation Log: %s\n",szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    iInfoLogLength=0;
    iShaderCompileStatus=0;
    szInfoLog=NULL;

    gFragmentShaderObject=glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar *fragmentShaderSourceCode=
    "#version 410"\
    "\n"\
    "in vec2 out_texture0_coord;"\
    "uniform sampler2D u_texture0_sampler;"\
    "out vec4 FragColor;"\
    "void main(void)"\
    "{"\
    "FragColor=texture(u_texture0_sampler,out_texture0_coord);"\
    "}";
    
    glShaderSource(gFragmentShaderObject,1,(const GLchar **)&fragmentShaderSourceCode,NULL);
    glCompileShader(gFragmentShaderObject);
    glGetShaderiv(gFragmentShaderObject,GL_COMPILE_STATUS,&iShaderCompileStatus);
    if(iShaderCompileStatus==GL_FALSE){
        glGetShaderiv(gFragmentShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
        if(iInfoLogLength>0){
            szInfoLog=(char *)malloc(iInfoLogLength);
            if(szInfoLog!=NULL){
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObject,iInfoLogLength,&written,szInfoLog);
                fprintf(gpFile,"Fragment Shader Compilation Log:%s\n",szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    gShaderProgramObject=glCreateProgram();
    
    glAttachShader(gShaderProgramObject, gVertexShaderObject);
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);
    
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_VERTEX,"vPosition");
    glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_TEXTURE0,"vTexture0_Coord");
    
    glLinkProgram(gShaderProgramObject);
    GLint iShaderProgramLinkStatus=0;
    glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS,&iShaderProgramLinkStatus);
    if(iShaderProgramLinkStatus==GL_FALSE){
        glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH,&iInfoLogLength);
        if(iInfoLogLength>0){
            szInfoLog=(char *)malloc(iInfoLogLength);
            if(szInfoLog!=NULL){
                GLsizei written;
                glGetProgramInfoLog( gShaderProgramObject, iInfoLogLength ,&written,szInfoLog);
                fprintf(gpFile,"Shader Program Link Log:%s\n",szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    gMVPUniform=glGetUniformLocation( gShaderProgramObject, "u_mvp_matrix");
    gTexture_sampler_uniform= glGetUniformLocation( gShaderProgramObject, "u_texture0_sampler");
    gTexture_Smiley=[self loadTextureFromBMPFile:"Smiley.bmp"];
    
    
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
    
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);
    CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

-(GLuint)loadTextureFromBMPFile:(const char *)texFileName{
    NSBundle *mainBundle=[NSBundle mainBundle];
    NSString *appDirName=[mainBundle bundlePath];
    NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
    NSString *textureFileNameWithPath=[NSString stringWithFormat:@"%@/%s",parentDirPath,texFileName];
    
    NSImage *bmpImage=[[NSImage alloc]initWithContentsOfFile:textureFileNameWithPath];
    if(!bmpImage){
        NSLog(@"can't find %@",textureFileNameWithPath);
        return(0);
    }
    
    CGImageRef cgImage=[bmpImage CGImageForProposedRect:nil context:nil hints:nil];
    
    int w=(int)CGImageGetWidth(cgImage);
    int h=(int)CGImageGetHeight(cgImage);
    CFDataRef imageData=CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    void* pixels=(void *)CFDataGetBytePtr(imageData);
    
    GLuint bmpTexture;
    glGenTextures(1,&bmpTexture);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        glBindTexture(GL_TEXTURE_2D,bmpTexture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    
            glGenerateMipmap(GL_TEXTURE_2D);
    CFRelease(imageData);
    return(bmpTexture);
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
    perspectiveProjectionMatrix=vmath::perspective(45.0f,width/height,0.1f,100.0f);
    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}
    
-(void)drawRect:(NSRect)dirtyRect{
    [self drawView];
}
    
-(void)drawView{
    static float anglePyramid=0.0f;
    static float angleCube=0.0f;
    
    [[self openGLContext]makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|
            GL_STENCIL_BUFFER_BIT);
    
    glUseProgram(gShaderProgramObject);
        vmath::mat4 modelViewMatrix;
        vmath::mat4 rotationMatrix;
        vmath::mat4 modelViewProjectionMatrix;
    
        modelViewMatrix=vmath::mat4::identity();
        modelViewMatrix=vmath::translate(-0.0f, 0.0f, -6.0f);

        modelViewProjectionMatrix= perspectiveProjectionMatrix *modelViewMatrix;
    
        glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,gTexture_sampler_uniform);
        glBindTexture(GL_TEXTURE_2D,gTexture_Smiley);
        glUniform1i(gTexture_sampler_uniform,0);
   
        glBindVertexArray(gVao_square);
            glDrawArrays(GL_TRIANGLE_FAN,0,4);
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
            [self release];
            [NSApp terminate:self];
            break;
        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self];
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
    
-(void)dealloc{
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
    
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,const CVTimeStamp *pNow,
                                   const CVTimeStamp*pOutputTime,CVOptionFlags flagsIn,
                                   CVOptionFlags *pFlagsOut,void *pDisplayLinkContext){

    CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
    return(result);
}
