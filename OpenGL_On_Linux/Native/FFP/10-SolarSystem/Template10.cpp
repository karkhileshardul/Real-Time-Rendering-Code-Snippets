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


Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
bool gbFullscreen=false;
int giWindowWidth=800;
int giWindowHeight=600;
GLfloat AngleForPyramid=0.0f;
GLfloat AngleForCube=0.0f;
GLXContext gGLXContext;
GLUquadric *quadric=NULL;
static int year = 0;
static int day = 0;

int main(void){
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void initialize(void);
	void display(void);
	void resize(int ,int);
	void uninitialize(void);

	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;
	char ArrayOfAscii[32];
	bool bDone=false;
	
	CreateWindow();
	
	initialize();

	XEvent event;
	KeySym keysym;

	while(bDone==false){
		while(XPending(gpDisplay)){
			XNextEvent(gpDisplay,&event);
			switch(event.type){
				case KeyPress:
					keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
					switch(keysym){
						case XK_Escape:
							bDone=true;
							break;
						case XK_F:
						case XK_f:
							if(gbFullscreen==false){
								ToggleFullscreen();
								gbFullscreen=true;
							}
							else{
								ToggleFullscreen();
								gbFullscreen=false;
							}
							break;
						default:
							break;
					}
					XLookupString(&event.xkey,ArrayOfAscii,sizeof(ArrayOfAscii),NULL,NULL);
					switch(ArrayOfAscii[0]){
						case 'D':
							day = (day + 6) % 360;
							break;
						case 'd':
							day = (day - 6) % 360;
							break;
						case 'Y':
							year = (year + 3) % 360;
							break;
						case 'y':
							year = (year - 3) % 360;
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
	return 0;
}

void CreateWindow(void){

	void uninitialize(void);
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;

	static int frameBufferAttributes[]={
		GLX_RGBA,1,
		GLX_RED_SIZE,8,
		GLX_GREEN_SIZE,8,
		GLX_BLUE_SIZE,8,
		GLX_ALPHA_SIZE,8,
		GLX_DEPTH_SIZE,24,
		GLX_DOUBLEBUFFER,True,
		None
	};

	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL){
		printf("ERROR : Unable to Open X Display.\n Exitting Now...\n");
		uninitialize();
		exit(1);
	}
	
	defaultScreen=XDefaultScreen(gpDisplay);
	
	gpXVisualInfo=glXChooseVisual(gpDisplay,defaultScreen,frameBufferAttributes);
	
	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),
						gpXVisualInfo->visual,AllocNone);
	gColormap=winAttribs.colormap;
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);

	winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask |
				KeyPressMask | KeyPressMask | PointerMotionMask |
				StructureNotifyMask;

	styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	gWindow=XCreateWindow(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,0,giWindowWidth,giWindowHeight,
				0,gpXVisualInfo->depth,
				InputOutput,
				gpXVisualInfo->visual,
				styleMask,&winAttribs);
				
	if(!gWindow){
		printf("ERROR : Failed to create Main Window.\n Exiting Now....\n");
		uninitialize();
		exit(1);
	}

	XStoreName(gpDisplay,gWindow,"Solar System Window");
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
	xev.xclient.data.l[0]=gbFullscreen ? 0 : 1;

	fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
	xev.xclient.data.l[1]=fullscreen;

	XSendEvent(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),
			False,StructureNotifyMask,&xev);
}


void initialize(void){
	void resize(int, int);
	gGLXContext=glXCreateContext(gpDisplay,gpXVisualInfo,NULL,GL_TRUE);
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glShadeModel(GL_SMOOTH);
	resize(giWindowWidth,giWindowHeight);
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 

	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	glColor3f(1.0f, 1.0f, 0.0f);
	gluSphere(quadric, 0.75, 30, 30);

	glPopMatrix();
	glPushMatrix();
	glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);
	glTranslatef(1.5f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	quadric = gluNewQuadric();
	glColor3f(0.4f, 0.9f, 1.0f);
	gluSphere(quadric, 0.2, 20, 20);
	glPopMatrix();

	glXSwapBuffers(gpDisplay,gWindow);
}

void resize(int width,int height){
	if (height == 0) {
		height = 1;
	}
	if(width==0){
		width=0;
	}	
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (width <= height) {
		gluPerspective(45.0f, (GLfloat)height / (GLfloat)width, 0.1f, 100.0f);
	}
	else {
		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	}

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
	gluDeleteQuadric(quadric);
	quadric=NULL;
}

