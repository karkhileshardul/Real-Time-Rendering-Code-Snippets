#include<stdio.h>
#include<stdlib.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>
#include<memory.h>
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

Colormap gColormap;
Window gWindow;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
GLXContext gGLXContext;
bool gbFullscreen=false;

int main(void){
	void CreateWindow(void);
	void initialize(void);
	void uninitialize(void);
	void display(void);
	void ToggleFullscreen(void);
	void resize(int,int);

	int winWidth=WIN_WIDTH;
	int winHeight=WIN_HEIGHT;
	bool bDone=false;
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
							break;
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

	styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	gColormap=winAttribs.colormap;
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
	winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask |
				KeyPressMask | PointerMotionMask | StructureNotifyMask;


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
	
	XStoreName(gpDisplay,gWindow,"01-HorizontalLine Window");
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

	resize(WIN_WIDTH,WIN_HEIGHT);
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

void display(void){
	void disp_triangle();
	void disp_vertical();
	void disp_horizontal();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLineWidth(3.0f);
	glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,-1.0f,0.0f);
	glEnd();
	disp_vertical();
	disp_horizontal();
	disp_triangle();
	glXSwapBuffers(gpDisplay,gWindow);
}

void resize(int width,int height){
	if(height==0){
		height=1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
}

void disp_vertical(void) {

	glLineWidth(1.0f);
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.05f, -1.0f, 0.0f);
		glVertex3f(0.05f, 1.0f, 0.0f);
			glVertex3f(0.10f, -1.0f, 0.0f);
			glVertex3f(0.10f, 1.0f, 0.0f);
		glVertex3f(0.15f, -1.0f, 0.0f);
		glVertex3f(0.15f, 1.0f, 0.0f);
			glVertex3f(0.20f, -1.0f, 0.0f);
			glVertex3f(0.20f, 1.0f, 0.0f);
		glVertex3f(0.25f, -1.0f, 0.0f);
		glVertex3f(0.25f, 1.0f, 0.0f);
			glVertex3f(0.30f, -1.0f, 0.0f);
			glVertex3f(0.30f, 1.0f, 0.0f);
		glVertex3f(0.35f, -1.0f, 0.0f);
		glVertex3f(0.35f, 1.0f, 0.0f);
			glVertex3f(0.40f, -1.0f, 0.0f);
			glVertex3f(0.40f, 1.0f, 0.0f);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.45f, 1.0f, 0.0f);
			glVertex3f(0.50f, -1.0f, 0.0f);
			glVertex3f(0.50f, 1.0f, 0.0f);
		glVertex3f(0.55f, -1.0f, 0.0f);
		glVertex3f(0.55f, 1.0f, 0.0f);
			glVertex3f(0.60f, -1.0f, 0.0f);
			glVertex3f(0.60f, 1.0f, 0.0f);
		glVertex3f(0.65f, -1.0f, 0.0f);
		glVertex3f(0.65f, 1.0f, 0.0f);
			glVertex3f(0.70f, -1.0f, 0.0f);
			glVertex3f(0.70f, 1.0f, 0.0f);
		glVertex3f(0.75f, -1.0f, 0.0f);
		glVertex3f(0.75f, 1.0f, 0.0f);
			glVertex3f(0.80f, -1.0f, 0.0f);
			glVertex3f(0.80f, 1.0f, 0.0f);
		glVertex3f(0.85f, -1.0f, 0.0f);
		glVertex3f(0.85f, 1.0f, 0.0f);
			glVertex3f(0.90f, -1.0f, 0.0f);
			glVertex3f(0.90f, 1.0f, 0.0f);
		glVertex3f(0.95f, -1.0f, 0.0f);
		glVertex3f(0.95f, 1.0f, 0.0f);
			glVertex3f(0.99999710f, -1.0f, 0.0f);
			glVertex3f(0.99999710f, 1.0f, 0.0f);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
		glVertex3f(-0.05f, -1.0f, 0.0f);
		glVertex3f(-0.05f, 1.0f, 0.0f);
			glVertex3f(-0.10f, -1.0f, 0.0f);
			glVertex3f(-0.10f, 1.0f, 0.0f);
		glVertex3f(-0.15f, -1.0f, 0.0f);
		glVertex3f(-0.15f, 1.0f, 0.0f);
			glVertex3f(-0.20f, -1.0f, 0.0f);
			glVertex3f(-0.20f, 1.0f, 0.0f);
		glVertex3f(-0.25f, -1.0f, 0.0f);
		glVertex3f(-0.25f, 1.0f, 0.0f);
			glVertex3f(-0.30f, -1.0f, 0.0f);
			glVertex3f(-0.30f, 1.0f, 0.0f);
		glVertex3f(-0.35f, -1.0f, 0.0f);
		glVertex3f(-0.35f, 1.0f, 0.0f);
			glVertex3f(-0.40f, -1.0f, 0.0f);
			glVertex3f(-0.40f, 1.0f, 0.0f);
		glVertex3f(-0.45f, -1.0f, 0.0f);
		glVertex3f(-0.45f, 1.0f, 0.0f);
			glVertex3f(-0.50f, -1.0f, 0.0f);
			glVertex3f(-0.50f, 1.0f, 0.0f);
		glVertex3f(-0.55f, -1.0f, 0.0f);
		glVertex3f(-0.55f, 1.0f, 0.0f);
			glVertex3f(-0.60f, -1.0f, 0.0f);
			glVertex3f(-0.60f, 1.0f, 0.0f);
		glVertex3f(-0.65f, -1.0f, 0.0f);
		glVertex3f(-0.65f, 1.0f, 0.0f);
			glVertex3f(-0.70f, -1.0f, 0.0f);
			glVertex3f(-0.70f, 1.0f, 0.0f);
		glVertex3f(-0.75f, -1.0f, 0.0f);
		glVertex3f(-0.75f, 1.0f, 0.0f);
			glVertex3f(-0.80f, -1.0f, 0.0f);
			glVertex3f(-0.80f, 1.0f, 0.0f);
		glVertex3f(-0.85f, -1.0f, 0.0f);
		glVertex3f(-0.85f, 1.0f, 0.0f);
			glVertex3f(-0.90f, -1.0f, 0.0f);
			glVertex3f(-0.90f, 1.0f, 0.0f);
		glVertex3f(-0.95f, -1.0f, 0.0f);
		glVertex3f(-0.95f, 1.0f, 0.0f);
			glVertex3f(-0.99999710f, -1.0f, 0.0f);
			glVertex3f(-0.99999710f, 1.0f, 0.0f);
	glEnd();

}

void disp_horizontal(void) {
	glLineWidth(1.0f);
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, 0.05f, 0.0f);
		glVertex3f(1.0f, 0.05f, 0.0f);
			glVertex3f(-1.0f, 0.10f, 0.0f);
			glVertex3f(1.0f, 0.10f, 0.0f);
		glVertex3f(-1.0f, 0.15f, 0.0f);
		glVertex3f(1.0f, 0.15f, 0.0f);
			glVertex3f(-1.0f, 0.20f, 0.0f);
			glVertex3f(1.0f, 0.20f, 0.0f);
		glVertex3f(-1.0f, 0.25f, 0.0f);
		glVertex3f(1.0f, 0.25f, 0.0f);
			glVertex3f(-1.0f, 0.30f, 0.0f);
			glVertex3f(1.0f, 0.30f, 0.0f);
		glVertex3f(-1.0f, 0.35f, 0.0f);
		glVertex3f(1.0f, 0.35f, 0.0f);
			glVertex3f(-1.0f, 0.40f, 0.0f);
			glVertex3f(1.0f, 0.40f, 0.0f);
		glVertex3f(-1.0f, 0.45f, 0.0f);
		glVertex3f(1.0f, 0.45f, 0.0f);
			glVertex3f(-1.0f, 0.50f, 0.0f);
			glVertex3f(1.0f, 0.50f, 0.0f);
		glVertex3f(-1.0f, 0.55f, 0.0f);
		glVertex3f(1.0f, 0.55f, 0.0f);
			glVertex3f(-1.0f, 0.60f, 0.0f);
			glVertex3f(1.0f, 0.60f, 0.0f);
		glVertex3f(-1.0f, 0.65f, 0.0f);
		glVertex3f(1.0f, 0.65f, 0.0f);
			glVertex3f(-1.0f, 0.70f, 0.0f);
			glVertex3f(1.0f, 0.70f, 0.0f);
		glVertex3f(-1.0f, 0.75f, 0.0f);
		glVertex3f(1.0f, 0.75f, 0.0f);
			glVertex3f(-1.0f, 0.80f, 0.0f);
			glVertex3f(1.0f, 0.80f, 0.0f);
		glVertex3f(-1.0f, 0.85f, 0.0f);
		glVertex3f(1.0f, 0.85f, 0.0f);
			glVertex3f(-1.0f, 0.90f, 0.0f);
			glVertex3f(1.0f, 0.90f, 0.0f);
		glVertex3f(-1.0f, 0.95f, 0.0f);
		glVertex3f(1.0f, 0.95f, 0.0f);
			glVertex3f(-1.0f, 0.9999949f, 0.0f);
			glVertex3f(1.0f, 0.9999949f, 0.0f);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
		glVertex3f(-1.0f, -0.05f, 0.0f);
		glVertex3f(1.0f, -0.05f, 0.0f);
			glVertex3f(-1.0f, -0.10f, 0.0f);
			glVertex3f(1.0f, -0.10f, 0.0f);
		glVertex3f(-1.0f, -0.15f, 0.0f);
		glVertex3f(1.0f, -0.15f, 0.0f);
			glVertex3f(-1.0f, -0.20f, 0.0f);
			glVertex3f(1.0f, -0.20f, 0.0f);
		glVertex3f(-1.0f, -0.25f, 0.0f);
		glVertex3f(1.0f, -0.25f, 0.0f);
			glVertex3f(-1.0f, -0.30f, 0.0f);
			glVertex3f(1.0f, -0.30f, 0.0f);
		glVertex3f(-1.0f, -0.35f, 0.0f);
		glVertex3f(1.0f, -0.35f, 0.0f);
			glVertex3f(-1.0f, -0.40f, 0.0f);
			glVertex3f(1.0f, -0.40f, 0.0f);
		glVertex3f(-1.0f, -0.45f, 0.0f);
		glVertex3f(1.0f, -0.45f, 0.0f);
			glVertex3f(-1.0f, -0.50f, 0.0f);
			glVertex3f(1.0f, -0.50f, 0.0f);
		glVertex3f(-1.0f, -0.55f, 0.0f);
		glVertex3f(1.0f, -0.55f, 0.0f);
			glVertex3f(-1.0f, -0.60f, 0.0f);
			glVertex3f(1.0f, -0.60f, 0.0f);
		glVertex3f(-1.0f, -0.65f, 0.0f);
		glVertex3f(1.0f, -0.65f, 0.0f);
			glVertex3f(-1.0f, -0.70f, 0.0f);
			glVertex3f(1.0f, -0.70f, 0.0f);
		glVertex3f(-1.0f, -0.75f, 0.0f);
		glVertex3f(1.0f, -0.75f, 0.0f);
			glVertex3f(-1.0f, -0.80f, 0.0f);
			glVertex3f(1.0f, -0.80f, 0.0f);
		glVertex3f(-1.0f, -0.85f, 0.0f);
		glVertex3f(1.0f, -0.85f, 0.0f);
			glVertex3f(-1.0f, -0.90f, 0.0f);
			glVertex3f(1.0f, -0.90f, 0.0f);
		glVertex3f(-1.0f, -0.95f, 0.0f);
		glVertex3f(1.0f, -0.95f, 0.0f);
			glVertex3f(-1.0f, -0.99999710f, 0.0f);
			glVertex3f(1.0f, -0.99999710f, 0.0f);
	glEnd();
}

void disp_triangle(void) {

	glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.5f, 0.0f);
		glVertex3f(0.5f, -0.5f, 0.0f);
		glVertex3f(-0.5f, -0.5f, 0.0f);
		glVertex3f(0.0f, 0.5f, 0.0f);
		glVertex3f(-0.5f, -0.5f, 0.0f);
		glVertex3f(0.5f, -0.5f, 0.0f);
	glEnd();

}
