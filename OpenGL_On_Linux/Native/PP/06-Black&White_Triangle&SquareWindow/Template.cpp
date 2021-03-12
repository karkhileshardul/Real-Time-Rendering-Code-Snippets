#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 
#include <memory.h> 
#include <X11/Xlib.h> 
#include <X11/Xutil.h> 
#include <X11/XKBlib.h> 
#include <X11/keysym.h> 
#include <GL/glew.h> 
#include <GL/gl.h>
#include <GL/glx.h> 
#include "vmath.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

FILE *gpFile = NULL;

Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB=NULL;
GLXFBConfig gGLXFBConfig;
GLXContext gGLXContext; 
bool gbFullscreen = false;

enum{
	SSK_ATTRIBUTE_VERTEX=0,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXTURE0,
};

vmath::mat4 gPerspectiveProjectionMatrix;
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gVao_square;
GLuint gVbo_square_position;
GLuint gVao_triangle;
GLuint gVbo_triangle_position;
GLuint gVbo_color;
GLuint gMVPUniform;


int main(int argc, char *argv[])
{
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void initialize(void);
	void resize(int,int);
	void display(void);
	void uninitialize(void);
	
	XEvent event; 
	KeySym keySym;
	int winWidth;
	int winHeight;
	bool bDone=false;

	gpFile=fopen("SSKLogFile.txt", "w");
	if (gpFile==NULL)
	{
		printf("Log File Can Not Be Created. EXitting Now ...\n");
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened.\n");
	}
	
	CreateWindow();
	initialize();
	
	while(bDone==false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay,&event); 
			switch(event.type)
			{
				case MapNotify: 
					break;
				case KeyPress: 
					keySym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
					switch(keySym)
					{
						case XK_Escape:
							bDone=true;
							break;
						case XK_F:
						case XK_f:
							if(gbFullscreen==false)
							{
								ToggleFullscreen();
								gbFullscreen=true;
							}
							else
							{
								ToggleFullscreen();
								gbFullscreen=false;
							}
							break;
						default:
							break;
					}
					break;
				case ConfigureNotify: 
					winWidth=event.xconfigure.width;
					winHeight=event.xconfigure.height;
					resize(winWidth,winHeight);
					break;
				case 33: 
					bDone=true;
					break;
				default:
					break;
			}
		}
		
		display();
	}
	
	uninitialize();
	return(0);
}

void CreateWindow(void)
{
	void uninitialize(void);

	XSetWindowAttributes winAttribs;
	GLXFBConfig *pGLXFBConfigs=NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *pTempXVisualInfo=NULL;
	int iNumFBConfigs=0;
	int styleMask;
	int i;
	
	static int frameBufferAttributes[]={
		GLX_X_RENDERABLE,True,
		GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE,GLX_TRUE_COLOR,
		GLX_RED_SIZE,8,
		GLX_GREEN_SIZE,8,
		GLX_BLUE_SIZE,8,
		GLX_ALPHA_SIZE,8,
		GLX_DEPTH_SIZE,24,
		GLX_STENCIL_SIZE,8,
		GLX_DOUBLEBUFFER,True,
		GLX_SAMPLE_BUFFERS,1,
		GLX_SAMPLES,4,
		None};
	
	
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
		printf("ERROR : Unable To Obtain X Display.\n");
		uninitialize();
		exit(1);
	}
	
	pGLXFBConfigs=glXChooseFBConfig(gpDisplay,DefaultScreen(gpDisplay),frameBufferAttributes,&iNumFBConfigs);
	if(pGLXFBConfigs==NULL)
	{
		printf( "Failed To Get Valid Framebuffer Config. Exitting Now ...\n");
		uninitialize();
		exit(1);
	}
	printf("%d Matching FB Configs Found.\n",iNumFBConfigs);
	
	
	int bestFramebufferconfig=-1,worstFramebufferConfig=-1,bestNumberOfSamples=-1,worstNumberOfSamples=999;
	for(i=0;i<iNumFBConfigs;i++)
	{
		pTempXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,pGLXFBConfigs[i]);
		if(pTempXVisualInfo)
		{
			int sampleBuffer,samples;
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLE_BUFFERS,&sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLES,&samples);
			printf("Matching Framebuffer Config=%d : Visual ID=0x%lu : SAMPLE_BUFFERS=%d : SAMPLES=%d\n",i,pTempXVisualInfo->visualid,sampleBuffer,samples);
			if(bestFramebufferconfig < 0 || sampleBuffer && samples > bestNumberOfSamples)
			{
				bestFramebufferconfig=i;
				bestNumberOfSamples=samples;
			}
			if( worstFramebufferConfig < 0 || !sampleBuffer || samples < worstNumberOfSamples)
			{
				worstFramebufferConfig=i;
			    worstNumberOfSamples=samples;
			}
		}
		XFree(pTempXVisualInfo);
	}
	bestGLXFBConfig = pGLXFBConfigs[bestFramebufferconfig];
	
	gGLXFBConfig=bestGLXFBConfig;
	
	XFree(pGLXFBConfigs);
	
	gpXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,bestGLXFBConfig);
	printf("Chosen Visual ID=0x%lu\n",gpXVisualInfo->visualid );
	

	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen), 
						gpXVisualInfo->visual,AllocNone); 
										
	winAttribs.event_mask=StructureNotifyMask | KeyPressMask | ButtonPressMask |
						  ExposureMask | VisibilityChangeMask | PointerMotionMask;
	
	styleMask=CWBorderPixel | CWEventMask | CWColormap;
	gColormap=winAttribs.colormap;										           
	
	gWindow=XCreateWindow(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,0,WIN_WIDTH,WIN_HEIGHT,0,gpXVisualInfo->depth,
				InputOutput,gpXVisualInfo->visual,styleMask,&winAttribs);
	if(!gWindow){
		printf("Failure In Window Creation.\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay,gWindow,"06-BlackandWhite Triangle&Square Window");
	
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_WINDOW_DELETE",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	
	XMapWindow(gpDisplay,gWindow);
}

void ToggleFullscreen(void)
{

	Atom wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False); 
	
	XEvent event;
	memset(&event,0,sizeof(XEvent));
	
	event.type=ClientMessage;
	event.xclient.window=gWindow;
	event.xclient.message_type=wm_state;
	event.xclient.format=32; 
	event.xclient.data.l[0]=gbFullscreen ? 0 : 1;

	Atom fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);	
	event.xclient.data.l[1]=fullscreen;
	
	XSendEvent(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),
		  False,StructureNotifyMask,&event);	
}

void initialize(void)
{
	void uninitialize(void);
	void resize(int,int);	

	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");
	
	GLint attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB,1,
		GLX_CONTEXT_MINOR_VERSION_ARB,3,
		GLX_CONTEXT_PROFILE_MASK_ARB,
		GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0 
	}; 
		
	gGLXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);

	if(!gGLXContext){

		GLint attribs[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB,1,
			GLX_CONTEXT_MINOR_VERSION_ARB,0,
			0 }; 
		printf("Failed To Create GLX 4.5 context. Hence Using Old-Style GLX Context\n");
		gGLXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);
	}
	else 
	{
		printf("OpenGL Context Is Created.\n");
	}
	
	if(!glXIsDirect(gpDisplay,gGLXContext))
	{
		printf("Indirect GLX Rendering Context Obtained\n");
	}
	else
	{
		printf("Direct GLX Rendering Context Obtained\n" );
	}
	
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		printf("Failure To Initialize GLEW. Exitting Now ...\n");
		uninitialize();
		exit(1);
	}

	gVertexShaderObject=glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode=
		"#version 130" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"out vec4 out_color;" \
		"uniform mat4 u_mvp_matrix;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_mvp_matrix*vPosition;" \
		"out_color=vColor;" \
		"}";
	
	glShaderSource(gVertexShaderObject,1,
				(const GLchar **)&vertexShaderSourceCode,NULL);		

	glCompileShader(gVertexShaderObject);

	GLint iInfoLogLength=0;
	GLint iShaderCompiledStatus=0;
	char *szInfoLog=NULL;

	glGetShaderiv(gVertexShaderObject,GL_COMPILE_STATUS,
				&iShaderCompiledStatus);

	if(iShaderCompiledStatus==GL_FALSE){
		glGetShaderiv(gVertexShaderObject,GL_INFO_LOG_LENGTH,
					&iInfoLogLength);
		if(iInfoLogLength>0){
			szInfoLog=(char*)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject,iInfoLogLength,
								&written,szInfoLog);
				fprintf(gpFile,"Vertex Shader Compilaton Log: %s\n",szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(EXIT_FAILURE);
			}
		}
	}


	gFragmentShaderObject=glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCode=
	"#version 130" \
	"\n" \
	"in vec4 out_color;" \
	"out vec4 FragColor;" \
	"void main(void)" \
	"{" \
	"FragColor=out_color;" \
	"}";

	glShaderSource(gFragmentShaderObject,1,
				(const GLchar**)&fragmentShaderSourceCode,NULL);
	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject,GL_COMPILE_STATUS,
				&iShaderCompiledStatus);

	if(iShaderCompiledStatus==GL_FALSE){
		glGetShaderiv(gFragmentShaderObject,GL_INFO_LOG_LENGTH,
					&iInfoLogLength);
		if(iInfoLogLength>0){
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject,iInfoLogLength,
								&written,szInfoLog);
				fprintf(gpFile,"Fragment Shader Compilation Log :%s\n",
						szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(EXIT_FAILURE);
			}
		}
	}

	gShaderProgramObject=glCreateProgram();

	glAttachShader(gShaderProgramObject,gVertexShaderObject);
	glAttachShader(gShaderProgramObject,gFragmentShaderObject);

	glBindAttribLocation(gShaderProgramObject,SSK_ATTRIBUTE_VERTEX,
						"vPosition");
	glBindAttribLocation(gShaderProgramObject,SSK_ATTRIBUTE_COLOR,
						"vColor");

	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus=0;
	glGetProgramiv(gShaderProgramObject,GL_LINK_STATUS,
					&iShaderProgramLinkStatus);
	if(iShaderProgramLinkStatus==GL_FALSE){
		glGetProgramiv(gShaderProgramObject,GL_INFO_LOG_LENGTH,
						&iInfoLogLength);
		if(iInfoLogLength>0){
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject,iInfoLogLength,
									&written,szInfoLog);
				fprintf(gpFile,"Shader Program Link Log : %s\n",szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(EXIT_FAILURE);
			}
		}
	}

	gMVPUniform=glGetUniformLocation(gShaderProgramObject,"u_mvp_matrix");
	const GLfloat triangleVertices[]=
	{	
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		0.0f,1.0f,0.0f
	};

	const GLfloat triangleColors[]=
	{	
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f
	};

	glGenVertexArrays(1,&gVao_triangle);
	glBindVertexArray(gVao_triangle);
	glGenBuffers(1,&gVbo_triangle_position);
	glBindBuffer(GL_ARRAY_BUFFER,gVbo_triangle_position);
	glBufferData(GL_ARRAY_BUFFER,sizeof(triangleVertices),
				triangleVertices,GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX,3,GL_FLOAT,
						GL_FALSE,0,NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	glGenBuffers(1,&gVbo_color);
	glBindBuffer(GL_ARRAY_BUFFER,gVbo_color);
	glBufferData(GL_ARRAY_BUFFER,sizeof(triangleColors),
				triangleColors,GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_COLOR,3,GL_FLOAT,
							GL_FALSE,0,NULL);

	glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	const GLfloat squareVertices[]=
	{	
		1.0f,1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f
	};

	const GLfloat squareColors[]=
	{	
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f
	};

	glGenVertexArrays(1,&gVao_square);
	glBindVertexArray(gVao_square);

	glGenBuffers(1,&gVbo_square_position);
	glBindBuffer(GL_ARRAY_BUFFER,gVbo_square_position);
	glBufferData(GL_ARRAY_BUFFER,sizeof(squareVertices),
				squareVertices,GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX,3,GL_FLOAT,
							GL_FALSE,0,NULL);

	glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	glGenBuffers(1,&gVbo_color);
	glBindBuffer(GL_ARRAY_BUFFER,gVbo_color);
	glBufferData(GL_ARRAY_BUFFER,sizeof(squareColors),
				squareColors,GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_COLOR,3,GL_FLOAT,
							GL_FALSE,0,NULL);

	glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	glBindVertexArray(0);


	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.0f,0.0f,0.0f,0.0f); 
	gPerspectiveProjectionMatrix=vmath::mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);
}

void resize(int width,int height)
{
	if(height==0)
		height=1;
		
	glViewport(0,0,(GLsizei)width,(GLsizei)height);

	if(width<=height){
		gPerspectiveProjectionMatrix=vmath::perspective(45.0f,(GLfloat)height/(GLfloat)width,
								0.1f,100.0f);
	}else{
		gPerspectiveProjectionMatrix=vmath::perspective(45.0f,(GLfloat)width/(GLfloat)height,
								0.1f,100.0f);
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);
	vmath::mat4 modelViewMatrix=vmath::mat4::identity();
	vmath::mat4 modelViewProjectionMatrix=vmath::mat4::identity();
	modelViewMatrix=vmath::translate(-1.5f,0.0f,-6.0f);
	modelViewProjectionMatrix=gPerspectiveProjectionMatrix*modelViewMatrix;

	glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);
	glBindVertexArray(gVao_triangle);
		glDrawArrays(GL_TRIANGLES,0,3);
	glBindVertexArray(0);

	modelViewMatrix=vmath::mat4::identity();
	modelViewProjectionMatrix=vmath::mat4::identity();
	modelViewMatrix=vmath::translate(1.5f,0.0f,-6.0f);
	modelViewProjectionMatrix=gPerspectiveProjectionMatrix*modelViewMatrix;

	glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);
	glBindVertexArray(gVao_square);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBindVertexArray(0);

	glUseProgram(0);
	glXSwapBuffers(gpDisplay,gWindow);
}

void uninitialize(void)
{
	GLXContext currentContext=glXGetCurrentContext();
	if(gVao_triangle){
		glDeleteVertexArrays(1,&gVao_triangle);
		gVao_triangle=0;
	}


	if(gVao_square){
		glDeleteVertexArrays(1,&gVao_square);
		gVao_square=0;
	}

	if(gVbo_square_position){
		glDeleteBuffers(1,&gVbo_square_position);
		gVbo_square_position=0;
	}

	if(gVbo_color){
		glDeleteBuffers(1,&gVbo_color);
		gVbo_color=0;
	}

	if(gVbo_triangle_position){
		glDeleteBuffers(1,&gVbo_triangle_position);
		gVbo_triangle_position=0;
	}

	glDetachShader(gShaderProgramObject,gVertexShaderObject);
	glDetachShader(gShaderProgramObject,gFragmentShaderObject);

	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject=0;

	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject=0;

	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject=0;

	glUseProgram(0);

	if(currentContext!=NULL && currentContext==gGLXContext)
	{
		glXMakeCurrent(gpDisplay,0,0);
	}
	
	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay,gGLXContext);
	}
	
	if(gWindow)
	{
		XDestroyWindow(gpDisplay,gWindow);
	}
	
	if(gColormap)
	{
		XFreeColormap(gpDisplay,gColormap);
	}
	
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}


