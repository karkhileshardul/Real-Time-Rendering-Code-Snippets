#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#define checkImageWidth		64
#define checkImageHeight	64

bool gbFullscreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;
bool bDone=false;
GLXContext gGLXContext;
GLubyte checkImage[checkImageHeight][checkImageHeight][4];
GLuint texName;
void LoadGLTextures();

int main(void){
	
	int winWidth=giWindowWidth,winHeight=giWindowHeight;
	XEvent event;
	KeySym keysym;
	
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void initialize(void);
	void display(void);
	void resize(int,int);
	void uninitialize(void);

	CreateWindow();
	initialize();
	
	while(bDone==false){
		while(XPending(gpDisplay)){
			XNextEvent(gpDisplay,&event);
			switch(event.type){
				case KeyPress:
					keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
					switch(keysym){
						case XK_Escape:
							uninitialize();
							exit(0);
						case XK_F:
						case XK_f:
							if(gbFullscreen==false){
								ToggleFullscreen();
								gbFullscreen=true;
							}else{
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
	return 0;
}

void CreateWindow(void){

	static int frameBufferAttributes[]={
		GLX_RGBA,
		GLX_RED_SIZE,1,
		GLX_GREEN_SIZE,1,
		GLX_BLUE_SIZE,1,
		GLX_ALPHA_SIZE,1,
		GLX_DEPTH_SIZE,24,
		None
	};

	void uninitialize(void);
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;

	
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL){
		printf("ERROR : Unable to Open X Display.\nExitting Now.......\n");
		uninitialize();
		exit(1);
	}

	defaultScreen=XDefaultScreen(gpDisplay);
	gpXVisualInfo=glXChooseVisual(gpDisplay,defaultScreen,frameBufferAttributes);
	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),
						gpXVisualInfo->visual,
						AllocNone);
	gColormap=winAttribs.colormap;
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
	winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask |
				 KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask= CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

	gWindow=XCreateWindow(gpDisplay,
				RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,
				0,
				giWindowWidth,
				giWindowHeight,
				0,
				gpXVisualInfo->depth,
				InputOutput,
				gpXVisualInfo->visual,
				styleMask,
				&winAttribs);

	if(!gWindow){
		printf("ERROR : Failed to Create Main Window. \nExitting Now......\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay,gWindow,"17-CheckerBoard Windowww");
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	XMapWindow(gpDisplay,gWindow);
}


void ToggleFullscreen(void){
	Atom wm_state;
	Atom fullscreen;
	XEvent xev={0};

	wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	memset(&xev,0,sizeof(xev));
	xev.type=ClientMessage;
	xev.xclient.window=gWindow;
	xev.xclient.message_type=wm_state;
	xev.xclient.format=32;
	xev.xclient.data.l[0]=gbFullscreen?0:1;
	fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
	xev.xclient.data.l[1]=fullscreen;
	XSendEvent(gpDisplay,
			RootWindow(gpDisplay,gpXVisualInfo->screen),
			False,
			StructureNotifyMask,
			&xev);
}

void initialize(void){
	void resize(int,int);
	gGLXContext=glXCreateContext(gpDisplay,gpXVisualInfo,NULL,GL_TRUE);
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	
	glShadeModel(GL_FLAT);	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
		
	glEnable(GL_TEXTURE_2D);
	LoadGLTextures();
	resize(giWindowWidth,giWindowHeight);
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-3.6f);
	glBegin(GL_QUADS);
		
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(-2.0f,-1.0f,0.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(-2.0f,1.0f,0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(0.0f,-1.0f,0.0f);

		glTexCoord2f(0.0f,0.0f);
		glVertex3f(1.0f,-1.0f,0.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(2.41421f,1.0f,-1.41421f);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(2.41421f,-1.0f,-1.41421f);
	glEnd();

	glXSwapBuffers(gpDisplay,gWindow);
}

void resize(int width,int height){
	if(height==0){
		height=1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
}

void uninitialize(void){
	GLXContext currentGLXContext;
	currentGLXContext=glXGetCurrentContext();

	glEnable(GL_TEXTURE_2D);
	glDeleteTextures(1,&texName);

	if(currentGLXContext!=NULL && currentGLXContext==gGLXContext){
		glXMakeCurrent(gpDisplay,0,0);
	}
	if(gGLXContext){
		glXDestroyContext(gpDisplay,gGLXContext);
	}
	if(gWindow){
		XDestroyWindow(gpDisplay,gWindow);
	}
	if(gColormap){
		XFreeColormap(gpDisplay,gColormap);
	}
	if(gpXVisualInfo){
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	if(gpDisplay){
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}

}

void MakeCheckImage(void){
	int i,j,c;

	for(i=0;i<checkImageHeight;i++){
		for(j=0;j<checkImageWidth;j++){
			c=(((i&0x8)==0) ^((j&0x8)==0)) * 255;
			checkImage[i][j][0]=(GLubyte)c;
			checkImage[i][j][1]=(GLubyte)c;
			checkImage[i][j][2]=(GLubyte)c;
			checkImage[i][j][3]=(GLubyte)255;
		}
	}
}

void LoadGLTextures(void){
	void MakeCheckImage(void);		
		
	MakeCheckImage();
		
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&texName);
	glBindTexture(GL_TEXTURE_2D,texName);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,checkImageWidth,checkImageHeight,0,
				GL_RGBA,GL_UNSIGNED_BYTE,checkImage);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

}

