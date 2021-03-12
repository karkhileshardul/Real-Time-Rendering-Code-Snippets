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
    [window setTitle:@"10-Cube and Pyramid Texture Window"];
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
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    GLuint vao_pyramid;
    GLuint vao_cube;
    GLuint vbo_pyramid_position;
    GLuint vbo_pyramid_texture;
    GLuint vbo_cube_position;
    GLuint vbo_cube_texture;
    GLuint mvpUniform;
    
    GLuint pyramid_texture;
    GLuint cube_texture;
    
    vmath::mat4 perspectiveProjectionMatrix;
    
    GLuint texture_sampler_uniform;
    
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
            NSOpenGLPFADoubleBuffer,
            0};
        
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
    
    vertexShaderObject=glCreateShader(GL_VERTEX_SHADER);
    
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
    
    glShaderSource(vertexShaderObject,1,(const GLchar **)&vertexShaderSourceCode,NULL);
    
    glCompileShader(vertexShaderObject);
    GLint iInfoLogLength=0;
    GLint iShaderCompileStatus=0;
    char *szInfoLog=NULL;
    glGetShaderiv(vertexShaderObject,GL_COMPILE_STATUS,
                  &iShaderCompileStatus);
    if(iShaderCompileStatus==GL_FALSE){
        glGetShaderiv(vertexShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
        if(iInfoLogLength>0){
            szInfoLog=(char *)malloc(iInfoLogLength);
            if(szInfoLog!=NULL){
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject,iInfoLogLength,&written,szInfoLog);
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
    
    fragmentShaderObject=glCreateShader(GL_FRAGMENT_SHADER);
    
    const GLchar *fragmentShaderSourceCode=
    "#version 410"\
    "\n"\
    "in vec2 out_texture0_coord;"\
    "uniform sampler2D u_texture0_sampler;"\
    "out vec4 FragColor;"\
    "void main(void)"\
    "{"\
    "vec3 tex=vec3(texture(u_texture0_sampler,out_texture0_coord));"\
    "FragColor=vec4(tex,1.0f);"\
    "}";
    
    glShaderSource(fragmentShaderObject,1,(const GLchar **)&fragmentShaderSourceCode,NULL);
    
    glCompileShader(fragmentShaderObject);
    glGetShaderiv(fragmentShaderObject,GL_COMPILE_STATUS,&iShaderCompileStatus);
    if(iShaderCompileStatus==GL_FALSE){
        glGetShaderiv(fragmentShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
        if(iInfoLogLength>0){
            szInfoLog=(char *)malloc(iInfoLogLength);
            if(szInfoLog!=NULL){
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject,iInfoLogLength,&written,szInfoLog);
                fprintf(gpFile,"Fragment Shader Compilation Log:%s\n",szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    shaderProgramObject=glCreateProgram();
    
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    glBindAttribLocation(shaderProgramObject, SSK_ATTRIBUTE_VERTEX,"vPosition");
    
    glBindAttribLocation(shaderProgramObject, SSK_ATTRIBUTE_TEXTURE0,"vTexture0_Coord");
    
    glLinkProgram(shaderProgramObject);
    GLint iShaderProgramLinkStatus=0;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS,&iShaderProgramLinkStatus);
    if(iShaderProgramLinkStatus==GL_FALSE){
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH,&iInfoLogLength);
        if(iInfoLogLength>0){
            szInfoLog=(char *)malloc(iInfoLogLength);
            if(szInfoLog!=NULL){
                GLsizei written;
                glGetProgramInfoLog( shaderProgramObject, iInfoLogLength ,&written,szInfoLog);
                fprintf(gpFile,"Shader Program Link Log:%s\n",szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    mvpUniform=glGetUniformLocation( shaderProgramObject, "u_mvp_matrix");
    
    texture_sampler_uniform= glGetUniformLocation( shaderProgramObject, "u_texture0_sampler");
    
    pyramid_texture=[self loadTextureFromBMPFile:"Stone.bmp"];
    cube_texture=[self loadTextureFromBMPFile:"Vijay_Kundali.bmp"];
    
    const GLfloat pyramidVertices[]={
        //front side
        0,1,0,  //front-top
        -1,-1,1, //front-left
        1,-1,1, //front-right
        
        //right side
        0,1,0,
        1,-1,1,
        1,-1,-1,
        
        //back side
        0,1,0,
        1,-1,-1,
        -1,-1,-1,
        
        //left side
        0,1,0,
        -1,-1,-1,
        -1,-1,1
    };
    
    const GLfloat pyramidTexcoords[]={
        //front side
        0.5f,1.0f,  //front-top
        0.0f,0.0f,  //front-left
        1.0f,0.0f,  //front-right
        
        //Right side
        0.5f,1.0f,
        1.0f,0.0f,
        0.0f,0.0f,
        
        //back side
        0.5f,1.0f,
        1.0f,0.0f,
        0.0f,0.0f,
        
        //left side
        0.5f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f,
    };
    
    GLfloat cubeVertices[]={
        // top surface
        1.0f, 1.0f,-1.0f,  // top-right of top
        -1.0f, 1.0f,-1.0f, // top-left of top
        -1.0f, 1.0f, 1.0f, // bottom-left of top
        1.0f, 1.0f, 1.0f,  // bottom-right of top
        
        // bottom surface
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        
        // front surface
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        
        // back surface
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        
        // left surface
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        
        // right surface
        1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
    };

    for(int i=0;i<72;i++){
        if(cubeVertices[i]<0.0f){
            cubeVertices[i]=cubeVertices[i]+0.25f;
        }else if(cubeVertices[i]>0.0f){
            cubeVertices[i]=cubeVertices[i]-0.25f;
        }else{
            cubeVertices[i]=cubeVertices[i];
        }
    }
    
    const GLfloat cubeTexcoords[]={
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f,
        
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f,
        
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f,
        
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f,
        
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f,
        
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f,
        
    };
    
    glGenVertexArrays(1,&vao_pyramid);
    glBindVertexArray(vao_pyramid);
        glGenBuffers(1,&vbo_pyramid_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_position);
            glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices,GL_STATIC_DRAW);
            glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX, 3,GL_FLOAT,GL_FALSE,0,NULL);
            glEnableVertexAttribArray (SSK_ATTRIBUTE_VERTEX);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    
        glGenBuffers(1,&vbo_pyramid_texture);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_pyramid_texture);
            glBufferData(GL_ARRAY_BUFFER,sizeof(pyramidTexcoords),pyramidTexcoords,GL_STATIC_DRAW);
            glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0,2,GL_FLOAT,GL_FALSE,0,NULL);
            glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    glGenVertexArrays(1,&vao_cube);
    glBindVertexArray(vao_cube);
    
        glGenBuffers(1,&vbo_cube_position);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_cube_position);
            glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);
            glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX,3,GL_FLOAT,GL_FALSE,0,NULL);
            glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glGenBuffers(1,&vbo_cube_texture);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_cube_texture);
            glBufferData(GL_ARRAY_BUFFER,sizeof(cubeTexcoords),cubeTexcoords,GL_STATIC_DRAW);
            glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0,2,GL_FLOAT,GL_FALSE,0,NULL);
            glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
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
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
        vmath::mat4 modelViewMatrix;
        vmath::mat4 rotationMatrix;
        vmath::mat4 modelViewperspectiveProjectionMatrix;
    
        modelViewMatrix=vmath::mat4::identity();
        modelViewMatrix=vmath::translate(-1.5f, 0.0f, -6.0f);
        rotationMatrix= vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
        modelViewMatrix= modelViewMatrix * rotationMatrix;
        modelViewperspectiveProjectionMatrix=perspectiveProjectionMatrix * modelViewMatrix;
    
        glUniformMatrix4fv(mvpUniform,1,GL_FALSE,modelViewperspectiveProjectionMatrix);
    
   
        glBindTexture(GL_TEXTURE_2D,pyramid_texture);
    
   
        glBindVertexArray(vao_pyramid);
    
        glDrawArrays(GL_TRIANGLES,0,12);
    
    glBindVertexArray(0);
        modelViewMatrix=vmath::mat4::identity();
        modelViewMatrix=vmath::translate(1.5f, 0.0f, -6.0f);
        rotationMatrix= vmath::rotate(angleCube, angleCube, angleCube);
    
        modelViewMatrix= modelViewMatrix * rotationMatrix;
        modelViewperspectiveProjectionMatrix=perspectiveProjectionMatrix * modelViewMatrix;
        glUniformMatrix4fv(mvpUniform,1,GL_FALSE,modelViewperspectiveProjectionMatrix);
 
        glBindTexture(GL_TEXTURE_2D,cube_texture);
        glBindVertexArray(vao_cube);
            glDrawArrays(GL_TRIANGLE_FAN,0,4);
            glDrawArrays(GL_TRIANGLE_FAN,4,4);
            glDrawArrays(GL_TRIANGLE_FAN,8,4);
            glDrawArrays(GL_TRIANGLE_FAN,12,4);
            glDrawArrays(GL_TRIANGLE_FAN,16,4);
            glDrawArrays(GL_TRIANGLE_FAN,20,4);
        glBindVertexArray(0);
  
    glUseProgram(0);
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    anglePyramid = anglePyramid + 1.0f;
    angleCube = angleCube + 1.0f;

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

    if(vao_pyramid){
        glDeleteVertexArrays(1, &vao_pyramid);
        vao_pyramid=0;
    }
  
    if(vbo_pyramid_position){
        glDeleteBuffers(1, &vbo_pyramid_position);
        vbo_pyramid_position=0;
    }
    
    if(vbo_pyramid_texture){
        glDeleteBuffers(1, &vbo_pyramid_texture);
        vbo_pyramid_texture=0;
    }
  
    if(pyramid_texture){
        glDeleteTextures(1,&pyramid_texture);
        pyramid_texture=0;
    }
    
    if(vao_cube){
        glDeleteVertexArrays(1, &vao_cube);
        vao_cube=0;
    }
    
    if(vbo_cube_position){
        glDeleteBuffers(1, &vbo_cube_position);
        vbo_cube_position=0;
    }
    
    if(vbo_cube_texture){
        glDeleteBuffers(1, &vbo_cube_texture);
        vbo_cube_texture=0;
    }
    
    if(cube_texture){
        glDeleteTextures(1,&cube_texture);
        cube_texture=0;
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
    
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,const CVTimeStamp *pNow,
                                   const CVTimeStamp*pOutputTime,CVOptionFlags flagsIn,
                                   CVOptionFlags *pFlagsOut,void *pDisplayLinkContext){

    CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
    return(result);
}
