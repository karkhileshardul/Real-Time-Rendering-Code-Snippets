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


bool gbFullscreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth=800;
GLfloat AngleForPyramid=0.0f;
GLfloat AngleForCube=0.0f;
int giWindowHeight=600;
FILE *gpFile=NULL;
GLXContext gGLXContext;


int main(void){

	gpFile=fopen("Template8-Log-SSK","w");
	if(gpFile==NULL){
		exit(1);
	}	
	fprintf(gpFile,"Log File Created Successfully\n");

	
	bool bDone=false;
	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;

	void CreateWindow(void);	
	void initialize(void);
	void display(void);
	void resize(int ,int);
	void uninitialize(void);
	void update(void);
	void ToggleFullscreen(void);
	
	CreateWindow();

	initialize();

	XEvent event;
	KeySym keysym;
	fprintf(gpFile," Game Loop Started\n");	
	while(bDone==false){
		while(XPending(gpDisplay)){
			XNextEvent(gpDisplay,&event);
			switch(event.type){
				case MapNotify:
					break;		
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
					break;
				case ButtonPress: 
					switch(event.xbutton.button){
						case 1: 
						    break;
						case 2: 
						    break;
						case 3: 
						    break;
						default:
						    break;
					}
					break;
				case MotionNotify: 
					break;
				case ConfigureNotify:
					winWidth=event.xconfigure.width;
					winHeight=event.xconfigure.height;
					resize(winWidth,winHeight);
					break;
				case Expose: 
					break;
				case DestroyNotify:
					break;
				case 33:
					bDone=true;
					break;	
				default:
					break;					
			}
		}
		update();
		display();
	}
	fprintf(gpFile,"Game Loop terminated\n");
	uninitialize();
	return 0;
}

void CreateWindow(void){
	fprintf(gpFile,"Entering in CreateWindow()\n");
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
		GLX_DOUBLEBUFFER,True,
		GLX_DEPTH_SIZE,24,
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
	winAttribs.colormap=XCreateColormap(gpDisplay,
						RootWindow(gpDisplay,gpXVisualInfo->screen),
						gpXVisualInfo->visual,
						AllocNone);
	gColormap=winAttribs.colormap;
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);

	winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask |
				KeyPressMask | KeyPressMask | PointerMotionMask |
				StructureNotifyMask;

	styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	
	gWindow=XCreateWindow(gpDisplay,
				RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,0,giWindowWidth,giWindowHeight,
				0,gpXVisualInfo->depth,
				InputOutput,gpXVisualInfo->visual,
				styleMask,&winAttribs);
				
	if(!gWindow){
		printf("ERROR : Failed to create Main Window.\n Exiting Now....\n");
		uninitialize();
		exit(1);
	}

	XStoreName(gpDisplay,gWindow,"08-3D rendering using Log File Window");

	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	XMapWindow(gpDisplay,gWindow);
	fprintf(gpFile,"Leaving CreateWindow()\n");
}	



void ToggleFullscreen(void){
	fprintf(gpFile,"Entering in ToggleFullscreen()\n");
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
			False,
			StructureNotifyMask,
			&xev);
	fprintf(gpFile,"Leaving ToggleFullscreen()\n");
}


void initialize(void){
	fprintf(gpFile,"Entering in initialize()\n");
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
	fprintf(gpFile,"Leaving initialize()\n");
}

void display(void){
	fprintf(gpFile,"Entering display()\n");
	void disp_Cube(void);
	void disp_Pyramid(void);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	disp_Pyramid();
	
	disp_Cube();

	glXSwapBuffers(gpDisplay,gWindow);
	fprintf(gpFile,"Leaving display()\n");
}

void resize(int width,int height){
	fprintf(gpFile,"Entering in resize()\n");
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
	fprintf(gpFile,"Leaving resize()\n");
}

void uninitialize(void){
	fprintf(gpFile,"Entering in uninitialize()\n");
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
	fprintf(gpFile,"Leaving uninitialize()\n");
	fclose(gpFile);
	gpFile=NULL;
}

void update(void){
	fprintf(gpFile,"Entering in update()\n");

	AngleForPyramid = AngleForPyramid + 0.3f;

	if (AngleForPyramid >= 360.0f) {

		AngleForPyramid = 0.0f;
	}
	


	AngleForCube = AngleForCube +0.3f;

	if (AngleForCube >= 360.0f) {

		AngleForCube = 0.0f;
	}		
	fprintf(gpFile,"Leaving update()\n");
}


void disp_Pyramid(void){
	fprintf(gpFile,"Entering in disp_Pyramid()\n");
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-2.0f,0.0f,-6.0f);
	glRotatef(AngleForPyramid,0.0f,1.0f,0.0f);
	
	glBegin(GL_TRIANGLES);

		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glColor3f(0.0f,1.0f,0.0f);	
		glVertex3f(-1.0f,-1.0f,1.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);

		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(1.0f,-1.0f,1.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);

		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		
		glColor3f(1.0f,0.0f,0.0f);	
		glVertex3f(0.0f,1.0f,0.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);	
	
	glEnd();
	fprintf(gpFile,"Leaving disp_Pyramid()\n");
}


void disp_Cube(void){
	fprintf(gpFile,"Entering in disp_Cube()\n");
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(2.0f,0.0f,-6.0f);
	glRotatef(AngleForCube,1.0f,0.0f,1.0f);

	glBegin(GL_QUADS);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(1.0f,1.0f,-1.0f);
		glVertex3f(-1.0f,1.0f,-1.0f);		
		glVertex3f(-1.0f,1.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);

		glColor3f(1.0f,1.0f,0.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);		
		glVertex3f(-1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);

		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(-1.0f,1.0f,1.0f);		
		glVertex3f(-1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);

		glColor3f(0.0f,1.0f,1.0f);
		glVertex3f(1.0f,1.0f,-1.0f);
		glVertex3f(-1.0f,1.0f,-1.0f);		
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);
	
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(-1.0f,1.0f,1.0f);
		glVertex3f(-1.0f,1.0f,-1.0f);		
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);

		glColor3f(1.0f,0.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(1.0f,1.0f,-1.0f);		
		glVertex3f(1.0f,-1.0f,-1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);
	
	glEnd();
	fprintf(gpFile,"Leaving disp_Cube()\n");
}
