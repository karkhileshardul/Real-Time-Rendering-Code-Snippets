#include<windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#define WIN_WIDTH	800
#define WIN_HEIGHT	600

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

HWND ghwnd=NULL;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
DWORD dwStyle;
bool gbFullscreen=false;
bool gbEscapeKeyIsPressed=false;
bool gbActiveWindow=false;
HDC ghdc=NULL;
HGLRC ghrc=NULL;

bool gLighting=false;
GLUquadric *quadric=NULL;
GLboolean bRedLight=GL_FALSE;
GLboolean bGreenLight=GL_FALSE;
GLboolean bBlueLight=GL_FALSE;
GLfloat AngleRedLight=0.0f;
GLfloat AngleBlueLight=0.0f;
GLfloat AngleGreenLight=0.0f;

GLfloat one_light_ambient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat one_light_diffuse[]={1.0f,0.0f,0.0f,0.0f};
GLfloat one_light_specular[]={1.0f,0.0f,0.0f,0.0f};
GLfloat one_light_position[]={0.0f,0.0f,0.0f,0.0f};

GLfloat two_light_ambient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat two_light_diffuse[]={0.0f,0.0f,1.0f,0.0f};
GLfloat two_light_specular[]={0.0f,0.0f,1.0f,0.0f};
GLfloat two_light_position[]={0.0f,0.0f,0.0f,0.0f};

GLfloat three_light_ambient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat three_light_diffuse[]={0.0f,1.0f,0.0f,0.0f};
GLfloat three_light_specular[]={0.0f,1.0f,0.0f,0.0f};
GLfloat three_light_position[]={0.0f,0.0f,0.0f,0.0f};

GLfloat one_material_ambient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat one_material_diffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat one_material_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat one_material_shininess[]={50.0f};

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	
	void uninitialize(void);
	void initialize(void);
	void display(void);
	void update(void);	
	WNDCLASSEX wndclassex;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[]=TEXT("51-3DSphere with 3 Lights");
	bool bDone=false;
	int nxIndex,nyIndex;
	
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

	hwnd=CreateWindowEx(WS_EX_APPWINDOW,szAppName,TEXT("51-3DSphere with 3 Lights Window"),
						WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
						(nxIndex/2-(WIN_WIDTH/2)),(nyIndex/2-(WIN_HEIGHT/2)),
						WIN_WIDTH,WIN_HEIGHT,NULL,NULL,hInstance,NULL);
	ghwnd=hwnd;
	if(hwnd==NULL){
		MessageBox(hwnd,TEXT("CreateWindowEx() has FAILED!!!!"),TEXT("!*!*!ERROR!*!*!"),MB_OK);
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
	void uninitialize(void);
	void resize(int,int);
	void ToggleFullscreen(void);
	
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
						glEnable(GL_LIGHTING);
						gLighting=true;
					}else{
						glDisable(GL_LIGHTING);
						gLighting=false;
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
	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}


void ToggleFullscreen(void){
	MONITORINFO mi;
	if(gbFullscreen==false){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW){
			mi={sizeof(MONITORINFO)};
			if(GetWindowPlacement(ghwnd,&wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi)){
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
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | 
					SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
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
	pfd.cDepthBits=24;
	pfd.cColorBits=32;
	pfd.iPixelType=PFD_TYPE_RGBA;
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
	}

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glDepthFunc(GL_LEQUAL);

	glLightfv(GL_LIGHT0,GL_AMBIENT,one_light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,one_light_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,one_light_specular);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1,GL_AMBIENT,two_light_ambient);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,two_light_diffuse);
	glLightfv(GL_LIGHT1,GL_SPECULAR,two_light_specular);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2,GL_AMBIENT,three_light_ambient);
	glLightfv(GL_LIGHT2,GL_DIFFUSE,three_light_diffuse);
	glLightfv(GL_LIGHT2,GL_SPECULAR,three_light_specular);
	glEnable(GL_LIGHT2);

	glMaterialfv(GL_FRONT,GL_AMBIENT,one_material_ambient);
	glMaterialfv(GL_FRONT,GL_SPECULAR,one_material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,one_material_diffuse);
	glMaterialfv(GL_FRONT,GL_SHININESS,one_material_shininess);

	quadric=gluNewQuadric();

	resize(WIN_WIDTH,WIN_HEIGHT);
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

void uninitialize(void){
	if(gbFullscreen==true){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE |
					SWP_NOOWNERZORDER |	SWP_NOZORDER | SWP_FRAMECHANGED);
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
		gluLookAt(0.0f,0.0f,0.1f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
		glPushMatrix();
			glRotatef(AngleRedLight,1.0f,0.0f,0.0f);
			one_light_position[1]=AngleRedLight;
			glLightfv(GL_LIGHT0,GL_POSITION,one_light_position);
		glPopMatrix();
		glPushMatrix();
			glRotatef(AngleGreenLight,0.0f,1.0f,0.0f);
			two_light_position[0]=AngleGreenLight;
			glLightfv(GL_LIGHT1,GL_POSITION,two_light_position);
		glPopMatrix();		
		glPushMatrix();
			glRotatef(AngleBlueLight,0.0f,0.0f,1.0f);
			three_light_position[0]=AngleBlueLight;
			glLightfv(GL_LIGHT2,GL_POSITION,three_light_position);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.0f,0.0f,-3.0f);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			gluSphere(quadric,0.75,30,30);
		glPopMatrix();
	glPopMatrix();

	SwapBuffers(ghdc);
}

void update(void){
	AngleRedLight=AngleRedLight+0.1f;
	if(AngleRedLight>=360.0f){
		AngleRedLight=0.0f;
	}
	AngleGreenLight=AngleGreenLight+0.1f;
	if(AngleGreenLight>=360.0f){
		AngleGreenLight=0.0f;
	}
	AngleBlueLight=AngleBlueLight+0.1f;
	if(AngleBlueLight>=360.0f){
		AngleBlueLight=0.0f;
	} 
}

