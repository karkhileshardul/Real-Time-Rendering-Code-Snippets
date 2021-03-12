#include<windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#define WIN_WIDTH	800
#define WIN_HEIGHT	600

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

HWND ghwnd=NULL;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
bool gbEscapeKeyIsPressed=false;
bool gbActiveWindow=false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};

bool gbFullscreen=false;
bool gLighting=false;
GLboolean bPyramid=GL_TRUE;
GLboolean bCube=GL_FALSE;
GLboolean bSphere=GL_FALSE;
GLfloat CubeAngle=0.0f;
GLfloat SphereAngle=0.0f;
GLfloat PyramidAngle=0.0f;
GLUquadric *quadric=NULL;

GLfloat one_light_ambient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat one_light_diffuse[]={1.0f,0.0f,0.0f,0.0f};
GLfloat one_light_specular[]={1.0f,0.0f,0.0f,0.0f};
GLfloat one_light_position[]={2.0f,1.0f,1.0f,0.0f};

GLfloat two_light_ambient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat two_light_diffuse[]={0.0f,0.0f,1.0f,0.0f};
GLfloat two_light_specular[]={0.0f,0.0f,1.0f,0.0f};
GLfloat two_light_position[]={-2.0f,1.0f,1.0f,0.0f};

GLfloat one_material_ambient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat one_material_diffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat one_material_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat one_material_shininess[]={50.0f};

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszClassName,int iCmdShow){
	void initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	WNDCLASSEX wndclassex;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[]=TEXT("50-3DObjectsTwoLights");
	int nxIndex,nyIndex;
	bool bDone=false;

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.style=CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.hInstance=hInstance;
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.lpszClassName=szAppName;
	wndclassex.lpszMenuName=NULL;

	RegisterClassEx(&wndclassex);

	nxIndex=GetSystemMetrics(SM_CXSCREEN);
	nyIndex=GetSystemMetrics(SM_CYSCREEN);


	hwnd=CreateWindowEx(WS_EX_APPWINDOW,szAppName,TEXT("50-3DObjectsTwoLights Window"),
						WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
						(nxIndex/2-(WIN_WIDTH/2)),(nyIndex/2-(WIN_HEIGHT/2)),WIN_WIDTH,
						WIN_HEIGHT,NULL,NULL,hInstance,NULL);
	ghwnd=hwnd;
	if(hwnd==NULL){
		MessageBox(hwnd,TEXT("CreateWindowEx() FAILED!!!!"),TEXT("ERROR!!!"),MB_OK);
		exit(EXIT_FAILURE);
	}

	initialize();
	ShowWindow(hwnd,SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	while(bDone==false){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
			if(msg.message==WM_QUIT){
				bDone=true;
			}else{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}else{
			if(gbActiveWindow==true){
				if(gbEscapeKeyIsPressed==true){
					bDone=true;
				}
				display();
				update();
			}
		}
	}
	uninitialize();
	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	void ToggleFullscreen(void);
	void uninitialize(void);
	void resize(int,int);	

	switch(iMsg){
		case WM_ACTIVATE:
			if(HIWORD(wParam)==0){
				gbActiveWindow=true;
			}else{
				gbActiveWindow=false;
			}
			break;
		case WM_SIZE:
			resize(LOWORD(lParam),HIWORD(lParam));
			break;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_ESCAPE:
					if(gbEscapeKeyIsPressed==false){
						gbEscapeKeyIsPressed=true;
					}
					break;
				case 0x43://C key
					if(bCube==GL_FALSE){
						bCube=GL_TRUE;
						bPyramid=GL_FALSE;
						bSphere=GL_FALSE;
					}else{
						bCube=GL_FALSE;
						bPyramid=GL_TRUE;
						bSphere=GL_TRUE;
					}
					break;
				case 0x46://F key
					if(gbFullscreen==false){
						ToggleFullscreen();
						gbFullscreen=true;
					}else{
						ToggleFullscreen();
						gbFullscreen=false;
					}
					break;
				case 0x4C: //L key
					if(gLighting==false){
						glEnable(GL_LIGHTING);
						gLighting=true;
					}else{
						glDisable(GL_LIGHTING);
						gLighting=false;
					}
					break;
				case 0x50://P key
					if(bPyramid==GL_FALSE){
						bPyramid=GL_TRUE;
						bCube=GL_FALSE;
						bSphere=GL_FALSE;
					}else{
						bPyramid=GL_FALSE;
						bCube=GL_TRUE;
						bSphere=GL_TRUE;
					}
					break;
				case 0x53://S key
					if(bSphere==GL_FALSE){
						bSphere=GL_TRUE;
						bCube=GL_FALSE;
						bPyramid=GL_FALSE;
					}else{
						bSphere=GL_FALSE;
						bCube=GL_TRUE;
						bPyramid=GL_TRUE;
					}
					break;
			}
			break;
		case WM_CLOSE:
			uninitialize();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			break;
	}
	return (DefWindowProc(hwnd,iMsg,wParam,lParam));
}

void initialize(void){
	PIXELFORMATDESCRIPTOR pfd;
	void resize(int,int);
	int iPixelFormatIndex;

	ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion=5;
	pfd.dwFlags= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType=PFD_TYPE_RGBA;
	pfd.cDepthBits=24;
	pfd.cColorBits=32;
	pfd.cRedBits=8;
	pfd.cBlueBits=8;
	pfd.cGreenBits=8;

	ghdc=GetDC(ghwnd);

	iPixelFormatIndex=ChoosePixelFormat(ghdc,&pfd);
	if(iPixelFormatIndex==0){
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
		ghwnd=NULL;
	}
	if(SetPixelFormat(ghdc,iPixelFormatIndex,&pfd)==false){
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
		ghwnd=NULL;
	}

	ghrc=wglCreateContext(ghdc);
	if(ghrc==NULL){
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
		ghwnd=NULL;
	}

	if(wglMakeCurrent(ghdc,ghrc)==false){
		wglDeleteContext(ghrc);
		ghrc=NULL;
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
		ghwnd=NULL;
	}

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0,GL_AMBIENT,one_light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,one_light_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,one_light_specular);
	glLightfv(GL_LIGHT0,GL_POSITION,one_light_position);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1,GL_AMBIENT,two_light_ambient);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,two_light_diffuse);
	glLightfv(GL_LIGHT1,GL_SPECULAR,two_light_specular);
	glLightfv(GL_LIGHT1,GL_POSITION,two_light_position);
	glEnable(GL_LIGHT1);

	glMaterialfv(GL_FRONT,GL_AMBIENT,one_material_ambient);
	glMaterialfv(GL_FRONT,GL_SPECULAR,one_material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,one_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SHININESS,one_material_shininess);

	quadric=gluNewQuadric();

	resize(WIN_WIDTH,WIN_HEIGHT);
}

void ToggleFullscreen(void){
	MONITORINFO mi;
	if(gbFullscreen==false){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW){
			mi={sizeof(MONITORINFO)};
			if(GetWindowPlacement(ghwnd,&wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi)){
				SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,
							mi.rcMonitor.right-mi.rcMonitor.left,
							mi.rcMonitor.bottom-mi.rcMonitor.top,
							SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}else{
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE |
					SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

void resize(int width,int height){
	if(height==0){
		height=1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
}



void uninitialize(void){
	if(gbFullscreen==false){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER |
					SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(ghrc);
	ghrc=NULL;
	ReleaseDC(ghwnd,ghdc);
	ghdc=NULL;
	ghwnd=NULL;
	DestroyWindow(ghwnd);
}

void display(void){

	void disp_Cube(void);
	void disp_Sphere(void);
	void disp_Pyramid(void);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(bPyramid==GL_TRUE){
	 	disp_Pyramid();
	}else if(bCube==GL_TRUE){
	 	disp_Cube();
	}else if(bSphere==GL_TRUE){
		disp_Sphere();
	}
	SwapBuffers(ghdc);
}

void disp_Pyramid(void){

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.9f,0.9f,-6.0f);
	glRotatef(PyramidAngle,0.0f,1.0f,0.0f);
	
	glBegin(GL_TRIANGLES);
		glNormal3f(0.0f,0.447214f,0.894427f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);

		glNormal3f(0.894427f,0.447214f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);

		glNormal3f(0.0f,0.447214f,-0.894427f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);

		glNormal3f(-0.894427f,0.447214f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);
	glEnd();
}

void disp_Cube(void){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.9f,0.9f,-6.0f);
	glScalef(0.80f,0.80f,0.80f);
	glRotatef(CubeAngle,0.0f,1.0f,0.0f);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(1.0f,1.0f,-1.0f);
		glVertex3f(-1.0f,1.0f,-1.0f);
		glVertex3f(-1.0f,1.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);

		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);

		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(-1.0f,1.0f,1.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);

		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3f(1.0f,1.0f,-1.0f);
		glVertex3f(-1.0f,1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);

		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3f(-1.0f,1.0f,1.0f);
		glVertex3f(-1.0f,1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);

		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3f(1.0f,1.0f,-1.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);
	glEnd();
}

void disp_Sphere(void){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,-0.7f,-4.0f);
	glRotatef(CubeAngle,0.0f,1.0f,0.0f);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	gluSphere(quadric,0.75,30,30);
}

void update(void){
	PyramidAngle=PyramidAngle+0.1f;
	if(PyramidAngle>=360.0f){
		PyramidAngle=0.0f;
	}
	CubeAngle=CubeAngle+0.1f;
	if(CubeAngle>=360.0f){
		CubeAngle=0.0f;
	}
	SphereAngle=SphereAngle+0.1f;
	if(SphereAngle>=360.0f){
		SphereAngle=0.0f;
	} 
}