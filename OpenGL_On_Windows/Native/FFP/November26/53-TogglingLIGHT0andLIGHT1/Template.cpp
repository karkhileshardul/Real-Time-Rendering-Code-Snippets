#include<windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#define WIN_WIDTH 800
#define WIN_HEIGHT	600
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

HWND ghwnd=NULL;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
bool gbFullscreen=false;
bool gbEscapeKeyIsPressed=false;
bool gbActiveWindow=false;
bool gLighting=false;
bool gLight0=false;
bool gLight1=false;
GLUquadric *quadric=NULL;

GLfloat light0_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat light0_diffuse[]={1.0f,1.0f,0.0f,0.0f};
GLfloat light0_position[]={1.0f,0.5f,1.0f,0.0f};

GLfloat light1_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat light1_diffuse[]={0.0f,1.0f,0.0f,0.0f};
GLfloat light1_position[]={0.0f,1.0f,0.0f,0.0f};
GLfloat light1_direction[]={-1.0f,-1.0f,0.0f,0.0f};

GLfloat material_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_shinyness[]={50.0f};

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	WNDCLASSEX wndclassex;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[]=TEXT("53-Lights Toggling");
	int nxIndex,nyIndex;
	bool bDone=false;

	void display(void);
	void initialize(void);
	void uninitialize(void);
	
	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.style= CS_HREDRAW |CS_VREDRAW | CS_OWNDC;
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.hInstance=hInstance;
	wndclassex.lpszClassName=szAppName;
	wndclassex.lpszMenuName=NULL;

	RegisterClassEx(&wndclassex);

	nxIndex=GetSystemMetrics(SM_CXSCREEN);
	nyIndex=GetSystemMetrics(SM_CYSCREEN);

	hwnd=CreateWindowEx(WS_EX_APPWINDOW,szAppName,TEXT("53-Lights Toggling code Window"),
						WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
						(nxIndex/2-(WIN_WIDTH/2)),(nyIndex/2-(WIN_HEIGHT/2)),
						WIN_WIDTH,WIN_HEIGHT,NULL,NULL,hInstance,NULL);
	ghwnd=hwnd;
	if(hwnd==NULL){
		MessageBox(hwnd,TEXT("CreateWindowEx() FAILED!!!!!!"),TEXT("!*!*!*ERROR*!*!*!"),MB_OK);
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

			}
		}
	}
	uninitialize();
	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	void resize(int,int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	switch(iMsg){
		case WM_ACTIVATE:
			if(HIWORD(wParam)==0){
				gbActiveWindow=true;
			}else{
				gbActiveWindow=false;
			}
			break;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_ESCAPE:
					if(gbEscapeKeyIsPressed==false){
						gbEscapeKeyIsPressed=true;
					}
					break;
				case 0x46:
					if(gbFullscreen==false){
						ToggleFullscreen();
						gbFullscreen=true;
					}else{
						ToggleFullscreen();
						gbFullscreen=false;
					}
					break;
				case 0x4C:
					if(gLighting==false){
						gLighting=true;
						gLight0=true;
						gLight1=false;
					}
					else{
						gLight0=false;
						gLight1=true;
						gLighting=false;	
					}
					break;
				default:
					break;
			}
			break;
		case WM_SIZE:
			resize(LOWORD(lParam),HIWORD(lParam));
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
	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
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
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}else{
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | 
					SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}


void initialize(void){
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	void resize(int,int);

	ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion=1;
	pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType=PFD_TYPE_RGBA;
	pfd.cDepthBits=24;
	pfd.cColorBits=32;
	pfd.cRedBits=8;
	pfd.cGreenBits=8;
	pfd.cBlueBits=8;

	ghdc=GetDC(ghwnd);

	iPixelFormatIndex=ChoosePixelFormat(ghdc,&pfd);
	if(iPixelFormatIndex==0){
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
		ghwnd=NULL;
	}
	if(SetPixelFormat(ghdc,iPixelFormatIndex,&pfd)==false){
		ReleaseDC(ghwnd,ghdc);
		ghwnd=NULL;
		ghdc=NULL;
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
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		
	glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
	glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
	glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1,GL_AMBIENT,light1_ambient);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,light1_diffuse);
	glLightfv(GL_LIGHT1,GL_POSITION,light1_position);
	glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,light1_direction);
	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,45.0f);	//light_direction
	glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,0.2f);//light_exponent
	glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,0.5f);//light_constant
	glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,0.2f);//light_linear
	glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,0.1f);//light_quadratic	
	glEnable(GL_LIGHT1);

	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,material_shinyness);

	resize(WIN_WIDTH,WIN_HEIGHT);
}

void uninitialize(void){
	if(gbFullscreen==true){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOMOVE |
					SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
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

void resize(int width,int height){
	if(height==0){
		height=1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width / (GLfloat)height,0.1f,100.0f);
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-3.0f);
	if(gLight0==false){
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT0);
	}else{
		glDisable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
	}
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	quadric=gluNewQuadric();
	gluSphere(quadric,0.75,30,30);
	SwapBuffers(ghdc);
}
