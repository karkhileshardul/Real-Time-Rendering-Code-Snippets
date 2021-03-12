#include<stdio.h>
#include<stdlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>
#include<memory.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

GLXContext gGLXContext;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
bool gbFullscreen=false;
bool gbLighting=false;
GLUquadric *quadric=NULL;
GLfloat light_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat light_diffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat light_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat light_position[]={0.0f,0.0f,1.0f,0.0f};
GLfloat material_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_shininess[]={50.0f};

int main(void){
	void CreateWindow(void);
	void display(void);
	void ToggleFullscreen(void);
	void resize(int,int);
	void initialize(void);
	void uninitialize(void);

	bool bDone=false;
	int winWidth=WIN_WIDTH;
	int winHeight=WIN_HEIGHT;

	XEvent event;
	KeySym keysym;

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
						case XK_L:
						case XK_l:
							if(gbLighting==false){
								glEnable(GL_LIGHTING);
								gbLighting=true;
							}else{
								glDisable(GL_LIGHTING);
								gbLighting=false;
							}
						default:
							break;
					}
					break;
				case ButtonPress:
					switch(event.xbutton.button){
						case 1: break;
						default: break;
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
	void uninitialize(void);
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;
	Atom windowManagerDelete;

	static int frameBufferAttributes[]={
		GLX_RGBA,
		GLX_RED_SIZE,1,
		GLX_GREEN_SIZE,1,
		GLX_BLUE_SIZE,1,
		GLX_ALPHA_SIZE,1,
		None
	};

	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL){
		printf("Error : Unable To open X Display.\n Exitting Now..\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	defaultScreen=XDefaultScreen(gpDisplay);

	gpXVisualInfo=glXChooseVisual(gpDisplay,defaultScreen,frameBufferAttributes);

	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,
						RootWindow(gpDisplay,gpXVisualInfo->screen),
						gpXVisualInfo->visual,
						AllocNone);

	gColormap=winAttribs.colormap;
	
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);

	winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask |
				KeyPressMask | PointerMotionMask | StructureNotifyMask;

	styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

	gWindow=XCreateWindow(gpDisplay,
				RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,0,
				WIN_WIDTH,WIN_HEIGHT,
				0,gpXVisualInfo->depth,
				InputOutput,
				gpXVisualInfo->visual,
				styleMask,&winAttribs);
	
	if(!gWindow){
		printf("ERROR: Failed to create Main Window.\nExitting Now...\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}
	
	XStoreName(gpDisplay,gWindow,"18-Lightning Window");
	windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
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
	xev.xclient.data.l[0]=gbFullscreen ? 0 : 1;

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
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHT0);


	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);

	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);


	resize(WIN_WIDTH,WIN_HEIGHT);
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0,-4.0f);

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	quadric=gluNewQuadric();
	glColor3f(1.0f,1.0f,0.0f);
	gluSphere(quadric,0.75f,30,30);


	glXSwapBuffers(gpDisplay,gWindow);
}

void resize(int width,int height){
	if(height==0){
		height=1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/ (GLfloat)height ,0.1f,100.0f);
}

void uninitialize(void){
	GLXContext currentGLXContext;
	currentGLXContext=glXGetCurrentContext();

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
