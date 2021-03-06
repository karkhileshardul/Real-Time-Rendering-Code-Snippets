#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>	
#include<X11/XKBlib.h>
#include<X11/keysym.h> /*For the Virtual Keycodes to Symbols */

bool gbFullscreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;

int main(void){
	
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize(void);
	
	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;
	
	CreateWindow();

	XEvent event;
	KeySym keysym;

	while(1){
		XNextEvent(gpDisplay,&event);
		switch(event.type){
			case MapNotify:
				break;
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
				break;
			case Expose:
				break;	
			case DestroyNotify:
				break;
			case 33:
				unintialize();
				exit(0);
			default:
				break;
		}
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

	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL){
		printf("ERROR: Unable to open X Display.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}
	
	defaultScreen=XDefaultScreen(gpDisplay);

	defaultDepth=DefaultDepth(gpDisplay,defaultScreen);

	gpXVisualInfo=(XVisualInfo *)malloc(sizeof(XVisualInfo));
	if(gpXVisualInfo==NULL){
		printf("ERROR: Unable to Allocate Memory for Visual Info.\nExiting Now...\n");
		uninitialize();
		exit(1);
	}

	if(XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo)==0){
		printf("ERROR: Unable to get a visual.\nExiting Now....\n");
		unintialize();
		exit(1);
	}

	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen,gpXVisualInfo->visual,AllocNone);

	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);

	winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;

	styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColorMap;

	gWindow=XCreateWindow(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),0,0,giWindowWidth,giWindowHeight,0,gpXVisualInfo->depth,InputOutput,gpXVisualInfo->visual,styleMask,&winAttribs);

	if(!gWindow){
		printf("ERROR : Failed To Create Main Window.\nExiting Now.....\n");
		uninitialize();
		exit(1);
	}

	XStoreNmae(gpDisplay,gWindow,"01-First X Window");
	
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
	
	XSetWMProtocol(gpDisplay,gWwindow,&windowManagerDelete,1);

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
	xev.xclient.data.l[0]=bFullscreen?0:1;

	XSendEvent(gpDisplay,RootWindow(gpDisplay,gpxVisualInfo->screen),False,StructureNotifyMask,&xev);

}

void uninitialize(void){
	
	if(gWindow){
		XDestroyWindow(gpDisplay,gWindow);
	}
	if(gColormap){
		XFreeColormap(gpDisplay,gColorMap);
	}
	if(gpxVisualInfo){
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	if(gpDisplay){
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
}


}

}

	
	

}
