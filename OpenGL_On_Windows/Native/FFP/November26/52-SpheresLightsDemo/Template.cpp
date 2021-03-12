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
DWORD dwStyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
bool gbFullscreen=false;
bool gbEscapeKeyIsPressed=false;
bool gbActiveWindow=false;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
bool gLighting=false;
GLfloat XAngle=0.0f;
GLfloat YAngle=0.0f;
GLfloat ZAngle=0.0f;
GLboolean gXRotate=GL_FALSE;
GLboolean gYRotate=GL_FALSE;
GLboolean gZRotate=GL_FALSE;
GLUquadric *quadric=NULL;

GLfloat one_light_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat one_light_diffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat one_light_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat one_light_position[]={0.0f,0.0f,0.0f,0.0f};

GLfloat one_material_ambient[]={0.0215f,0.1745f,0.0215f,1.0f};
GLfloat one_material_diffuse[]={0.07568f,0.61424f,0.07568f,1.0f};
GLfloat one_material_specular[]={0.633f,0.727811f,0.633f,1.0f};
GLfloat one_material_shininess[]={76.8f};

GLfloat two_material_ambient[]={0.135f,0.2225f,0.1575f,1.0f};
GLfloat two_material_diffuse[]={0.54f,0.89f,0.63f,1.0f};
GLfloat two_material_specular[]={0.316228f,0.316228f,0.316228f,1.0f};
GLfloat two_material_shininess[]={12.8f};

GLfloat three_material_ambient[]={0.05375f,0.05f,0.06625f,1.0f};
GLfloat three_material_diffuse[]={0.18275f,0.17f,0.22525f,1.0f};
GLfloat three_material_specular[]={0.332741f,0.328634f,0.346435f,1.0f};
GLfloat three_material_shininess[]={38.4f};

GLfloat four_material_ambient[]={0.25f,0.20725f,0.20725f,1.0f};
GLfloat four_material_diffuse[]={1.0f,0.829f,0.829f,1.0f};
GLfloat four_material_specular[]={0.296648f,0.296648f,0.296648f,1.0f};
GLfloat four_material_shininess[]={11.264f};

GLfloat five_material_ambient[]={0.1745f,0.01175f,0.01175f,1.0f};
GLfloat five_material_diffuse[]={0.61424f,0.04136f,0.04136f,1.0f};
GLfloat five_material_specular[]={0.727811f,0.626959f,0.626959f,1.0f};
GLfloat five_material_shininess[]={76.8f};

GLfloat six_material_ambient[]={0.1f,0.18725f,0.1745f,1.0f};
GLfloat six_material_diffuse[]={0.396f,0.74151f,0.69102f,1.0f};
GLfloat six_material_specular[]={0.297254f,0.30829f,0.306678f,1.0f};
GLfloat six_material_shininess[]={12.8f};

GLfloat seven_material_ambient[]={0.329412f,0.223529f,0.027451f,1.0f};
GLfloat seven_material_diffuse[]={0.780392f,0.568627f,0.113725f,1.0f};
GLfloat seven_material_specular[]={0.992157f,0.941176f,0.807843f,1.0f};
GLfloat seven_material_shininess[]={27.89743616f};

GLfloat eight_material_ambient[]={0.2125f,0.1275f,0.054f,1.0f};
GLfloat eight_material_diffuse[]={0.714f,0.4284f,0.18144f,1.0f};
GLfloat eight_material_specular[]={0.393548f,0.271906f,0.166721f,1.0f};
GLfloat eight_material_shininess[]={25.6f};

GLfloat nine_material_ambient[]={0.25f,0.25f,0.25f,1.0f};
GLfloat nine_material_diffuse[]={0.4f,0.4f,0.4f,1.0f};
GLfloat nine_material_specular[]={0.774597f,0.774597f,0.774597f,1.0f};
GLfloat nine_material_shininess[]={76.8f};

GLfloat ten_material_ambient[]={0.19125f,0.0735f,0.0225f,1.0f};
GLfloat ten_material_diffuse[]={0.7038f,0.27048f,0.0828f,1.0f};
GLfloat ten_material_specular[]={0.256777f,0.137622f,0.086014f,1.0f};
GLfloat ten_material_shininess[]={12.8f};

GLfloat eleven_material_ambient[]={0.24725f,0.1995f,0.0745f,1.0f};
GLfloat eleven_material_diffuse[]={0.75164f,0.60648f,0.22648f,1.0f};
GLfloat eleven_material_specular[]={0.628281f,0.555802f,0.366065f,1.0f};
GLfloat eleven_material_shininess[]={51.2f};

GLfloat twelve_material_ambient[]={0.19225f,0.19225f,0.19225f,1.0f};
GLfloat twelve_material_diffuse[]={0.50754f,0.50754f,0.50754f,1.0f};
GLfloat twelve_material_specular[]={0.508273f,0.508273f,0.508273f,1.0f};
GLfloat twelve_material_shininess[]={51.2f};

GLfloat thirteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat thirteen_material_diffuse[]={0.01f,0.01f,0.01f,1.0f};
GLfloat thirteen_material_specular[]={0.50f,0.50f,0.50f,1.0f};
GLfloat thirteen_material_shininess[]={32.0f};

GLfloat fourteen_material_ambient[]={0.0f,0.1f,0.06f,1.0f};
GLfloat fourteen_material_diffuse[]={0.0f,0.50980392f,0.50980392f,1.0f};
GLfloat fourteen_material_specular[]={0.50196078f,0.50196078f,0.50196078f,1.0f};
GLfloat fourteen_material_shininess[]={32.0f};

GLfloat fifteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat fifteen_material_diffuse[]={0.1f,0.35f,0.1f,1.0f};
GLfloat fifteen_material_specular[]={0.45f,0.55f,0.45f,1.0f};
GLfloat fifteen_material_shininess[]={32.0f};

GLfloat sixteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat sixteen_material_diffuse[]={0.5f,0.0f,0.0f,1.0f};
GLfloat sixteen_material_specular[]={0.7f,0.6f,0.6f,1.0f};
GLfloat sixteen_material_shininess[]={32.0f};

GLfloat seventeen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat seventeen_material_diffuse[]={0.55f,0.55f,0.55f,1.0f};
GLfloat seventeen_material_specular[]={0.70f,0.70f,0.70f,1.0f};
GLfloat seventeen_material_shininess[]={32.0f};

GLfloat eighteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat eighteen_material_diffuse[]={0.5f,0.5f,0.0f,1.0f};
GLfloat eighteen_material_specular[]={0.60f,0.60f,0.50f,1.0f};
GLfloat eighteen_material_shininess[]={32.0f};

GLfloat nineteen_material_ambient[]={0.02f,0.02f,0.02f,1.0f};
GLfloat nineteen_material_diffuse[]={0.01f,0.01f,0.01f,1.0f};
GLfloat nineteen_material_specular[]={0.4f,0.4f,0.4f,1.0f};
GLfloat nineteen_material_shininess[]={10.0f};

GLfloat twenty_material_ambient[]={0.0f,0.05f,0.05f,1.0f};
GLfloat twenty_material_diffuse[]={0.4f,0.5f,0.5f,1.0f};
GLfloat twenty_material_specular[]={0.04f,0.7f,0.7f,1.0f};
GLfloat twenty_material_shininess[]={10.0f};

GLfloat twentyone_material_ambient[]={0.0f,0.05f,0.0f,1.0f};
GLfloat twentyone_material_diffuse[]={0.4f,0.5f,0.4f,1.0f};
GLfloat twentyone_material_specular[]={0.04f,0.7f,0.04f,1.0f};
GLfloat twentyone_material_shininess[]={10.0f};

GLfloat twentytwo_material_ambient[]={0.05f,0.0f,0.0f,1.0f};
GLfloat twentytwo_material_diffuse[]={0.5f,0.4f,0.4f,1.0f};
GLfloat twentytwo_material_specular[]={0.7f,0.04f,0.04f,1.0f};
GLfloat twentytwo_material_shininess[]={10.0f};

GLfloat twentythree_material_ambient[]={0.05f,0.05f,0.05f,1.0f};
GLfloat twentythree_material_diffuse[]={0.5f,0.5f,0.5f,1.0f};
GLfloat twentythree_material_specular[]={0.7f,0.7f,0.7f,1.0f};
GLfloat twentythree_material_shininess[]={10.0f};

GLfloat twentyfour_material_ambient[]={0.05f,0.05f,0.0f,1.0f};
GLfloat twentyfour_material_diffuse[]={0.5f,0.5f,0.4f,1.0f};
GLfloat twentyfour_material_specular[]={0.7f,0.7f,0.04f,1.0f};
GLfloat twentyfour_material_shininess[]={10.0f};

GLfloat light_model_ambient[]={0.2f,0.2f,0.2f,0.0f};
GLfloat light_model_local_viewer[]={0.0f};

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void Horizontal_One(void);
void Horizontal_Two(void);
void Horizontal_Three(void);
void Horizontal_Four(void);
void Horizontal_Five(void);
void Horizontal_Six(void);
void KeyRotation(void);

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){

	WNDCLASSEX wndclassex;
	MSG msg;
	HWND hwnd;
	TCHAR szAppName[]=TEXT("52-SphereLightsDemo");
	int nxIndex,nyIndex;
	bool bDone=false;

	void initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.style=CS_HREDRAW |CS_VREDRAW | CS_OWNDC;
	wndclassex.hInstance=hInstance;
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.lpszClassName=szAppName;
	wndclassex.lpszMenuName=NULL;
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);

	RegisterClassEx(&wndclassex);

	nxIndex=GetSystemMetrics(SM_CXSCREEN);
	nyIndex=GetSystemMetrics(SM_CYSCREEN);

	hwnd=CreateWindowEx(WS_EX_APPWINDOW,szAppName,TEXT("52-SphereLightsDemo Window"),
						WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
						(nxIndex/2-(WIN_WIDTH/2)),(nyIndex/2-(WIN_HEIGHT/2)),
						WIN_WIDTH,WIN_HEIGHT,NULL,NULL,hInstance,NULL);
	ghwnd=hwnd;
	if(hwnd==NULL){
		MessageBox(hwnd,TEXT("CreateWindowEx() Failed!!!!"),TEXT("#!!#ERROR#!!#"),MB_OK);
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
	return((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){

	void ToggleFullscreen(void);
	void resize(int,int);
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
						glEnable(GL_LIGHTING);
						gLighting=true;
					}else{
						glDisable(GL_LIGHTING);
						gLighting=false;
					}
					break;
				case 0x58://X key
					if(gXRotate==GL_FALSE){
						gXRotate=GL_TRUE;
						gYRotate=GL_FALSE;
						gZRotate=GL_FALSE;
					}else{
						gXRotate=GL_FALSE;
						gYRotate=GL_FALSE;
						gZRotate=GL_FALSE;
					}
					break;
				case 0x59://Y key
					if(gYRotate==GL_FALSE){
						gYRotate=GL_TRUE;
						gXRotate=GL_FALSE;
						gZRotate=GL_FALSE;
					}else{
						gYRotate=GL_FALSE;
						gXRotate=GL_FALSE;
						gZRotate=GL_FALSE;
					}
					break;
				case 0x5A://Z key
					if(gZRotate==GL_FALSE){
						gZRotate=GL_TRUE;
						gYRotate=GL_FALSE;
						gXRotate=GL_FALSE;
					}else{
						gZRotate=GL_FALSE;
						gYRotate=GL_FALSE;
						gXRotate=GL_FALSE;
					}
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

void initialize(void){
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	void resize(int,int);

	ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion=2;
	pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cDepthBits=24;
	pfd.cColorBits=32;
	pfd.cRedBits=8;
	pfd.cGreenBits=8;
	pfd.cBlueBits=8;
	pfd.iPixelType=PFD_TYPE_RGBA;

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

	glClearColor(0.25f,0.25f,0.25f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glDepthFunc(GL_LEQUAL);

	glLightfv(GL_LIGHT0,GL_AMBIENT,one_light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,one_light_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,one_light_specular);
	glEnable(GL_LIGHT0);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER,light_model_local_viewer);


	quadric=gluNewQuadric();
	resize(WIN_WIDTH,WIN_HEIGHT);
}

void uninitialize(void){
	if(gbFullscreen==true){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE |
					SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(ghrc);
	ghrc=NULL;
	ReleaseDC(ghwnd,ghdc);
	ghwnd=NULL;
	ghdc=NULL;
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

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Horizontal_One();
	Horizontal_Two();
	Horizontal_Three();
	Horizontal_Four();
	Horizontal_Five();
	Horizontal_Six();
	SwapBuffers(ghdc);
}


void Horizontal_One(void){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();			
		KeyRotation();
		glPushMatrix();
			glTranslatef(-0.6f,1.0f,-3.0f);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			glMaterialfv(GL_FRONT,GL_AMBIENT,one_material_ambient);
			glMaterialfv(GL_FRONT,GL_SPECULAR,one_material_specular);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,one_material_diffuse);
			glMaterialfv(GL_FRONT,GL_SHININESS,one_material_shininess);
			gluSphere(quadric,0.15,30,30);
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();	
			glPushMatrix();	
				glTranslatef(-0.2f,1.0f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,seven_material_ambient);
				glMaterialfv(GL_FRONT,GL_SPECULAR,seven_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,seven_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,seven_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();
			glPushMatrix();
				glTranslatef(0.2f,1.0f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,thirteen_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,thirteen_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,thirteen_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,thirteen_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();
		glPushMatrix();
			glLightfv(GL_LIGHT0,GL_DIFFUSE,one_light_diffuse);
			glPushMatrix();	
				glTranslatef(0.6f,1.0f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,nineteen_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,nineteen_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,nineteen_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,nineteen_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void Horizontal_Two(void){
	glLoadIdentity();
	glPushMatrix();	
		KeyRotation();
		glPushMatrix();
			glTranslatef(-0.6f,0.6f,-3.0f);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			glMaterialfv(GL_FRONT,GL_AMBIENT,two_material_ambient);
			glMaterialfv(GL_FRONT,GL_SPECULAR,two_material_specular);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,two_material_diffuse);
			glMaterialfv(GL_FRONT,GL_SHININESS,two_material_shininess);
			gluSphere(quadric,0.15,30,30);
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();	
			glPushMatrix();	
				glTranslatef(-0.2f,0.6f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,eight_material_ambient);
				glMaterialfv(GL_FRONT,GL_SPECULAR,eight_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,eight_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,eight_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();
			glPushMatrix();
				glTranslatef(0.2f,0.6f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,fourteen_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,fourteen_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,fourteen_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,fourteen_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();
		glPushMatrix();
			glLightfv(GL_LIGHT0,GL_DIFFUSE,one_light_diffuse);
			glPushMatrix();	
				glTranslatef(0.6f,0.6f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,twenty_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,twenty_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,twenty_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,twenty_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void Horizontal_Three(void){
	glLoadIdentity();
	glPushMatrix();
		KeyRotation();
		glPushMatrix();
			glTranslatef(-0.6f,0.20f,-3.0f);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			glMaterialfv(GL_FRONT,GL_AMBIENT,three_material_ambient);
			glMaterialfv(GL_FRONT,GL_SPECULAR,three_material_specular);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,three_material_diffuse);
			glMaterialfv(GL_FRONT,GL_SHININESS,three_material_shininess);
			gluSphere(quadric,0.15,30,30);
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();	
			glPushMatrix();	
				glTranslatef(-0.2f,0.20f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,nine_material_ambient);
				glMaterialfv(GL_FRONT,GL_SPECULAR,nine_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,nine_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,nine_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();
			glPushMatrix();
				glTranslatef(0.2f,0.20f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,fifteen_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,fifteen_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,fifteen_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,fifteen_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();
		glPushMatrix();
			glLightfv(GL_LIGHT0,GL_DIFFUSE,one_light_diffuse);
			glPushMatrix();	
				glTranslatef(0.6f,0.20f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,twentyone_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,twentyone_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,twentyone_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,twentyone_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void Horizontal_Four(void){
	glLoadIdentity();
	glPushMatrix();
		KeyRotation();
		glPushMatrix();
			glTranslatef(-0.6f,-0.2f,-3.0f);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			glMaterialfv(GL_FRONT,GL_AMBIENT,four_material_ambient);
			glMaterialfv(GL_FRONT,GL_SPECULAR,four_material_specular);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,four_material_diffuse);
			glMaterialfv(GL_FRONT,GL_SHININESS,four_material_shininess);
			gluSphere(quadric,0.15,30,30);
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();	
			glPushMatrix();	
				glTranslatef(-0.2f,-0.2f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,ten_material_ambient);
				glMaterialfv(GL_FRONT,GL_SPECULAR,ten_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,ten_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,ten_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();
			glPushMatrix();
				glTranslatef(0.2f,-0.2f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,sixteen_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,sixteen_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,sixteen_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,sixteen_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();
		glPushMatrix();
			glLightfv(GL_LIGHT0,GL_DIFFUSE,one_light_diffuse);
			glPushMatrix();	
				glTranslatef(0.6f,-0.2f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,twentytwo_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,twentytwo_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,twentytwo_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,twentytwo_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void Horizontal_Five(void){
	glLoadIdentity();
	glPushMatrix();	
		KeyRotation();
		glPushMatrix();
			glTranslatef(-0.6f,-0.60f,-3.0f);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			glMaterialfv(GL_FRONT,GL_AMBIENT,five_material_ambient);
			glMaterialfv(GL_FRONT,GL_SPECULAR,five_material_specular);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,five_material_diffuse);
			glMaterialfv(GL_FRONT,GL_SHININESS,five_material_shininess);
			gluSphere(quadric,0.15,30,30);
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();	
			glPushMatrix();	
				glTranslatef(-0.2f,-0.60f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,eleven_material_ambient);
				glMaterialfv(GL_FRONT,GL_SPECULAR,eleven_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,eleven_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,eleven_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();
			glPushMatrix();
				glTranslatef(0.2f,-0.60f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,seventeen_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,seventeen_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,seventeen_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,seventeen_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();
		glPushMatrix();
			glLightfv(GL_LIGHT0,GL_DIFFUSE,one_light_diffuse);
			glPushMatrix();	
				glTranslatef(0.6f,-0.60f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,twentythree_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,twentythree_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,twentythree_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,twentythree_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void Horizontal_Six(void){
	glLoadIdentity();
	glPushMatrix();	
		KeyRotation();
		glPushMatrix();
			glTranslatef(-0.6f,-1.0f,-3.0f);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			glMaterialfv(GL_FRONT,GL_AMBIENT,six_material_ambient);
			glMaterialfv(GL_FRONT,GL_SPECULAR,six_material_specular);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,six_material_diffuse);
			glMaterialfv(GL_FRONT,GL_SHININESS,six_material_shininess);
			gluSphere(quadric,0.15,30,30);
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();	
			glPushMatrix();	
				glTranslatef(-0.2f,-1.0f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,twelve_material_ambient);
				glMaterialfv(GL_FRONT,GL_SPECULAR,twelve_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,twelve_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,twelve_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();	
		glPushMatrix();
			glPushMatrix();
				glTranslatef(0.2f,-1.0f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,eighteen_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,eighteen_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,eighteen_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,eighteen_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
		glLoadIdentity();
		glPushMatrix();
			glLightfv(GL_LIGHT0,GL_DIFFUSE,one_light_diffuse);
			glPushMatrix();	
				glTranslatef(0.6f,-1.0f,-3.0f);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glMaterialfv(GL_FRONT,GL_AMBIENT,twentyfour_material_ambient);	
				glMaterialfv(GL_FRONT,GL_SPECULAR,twentyfour_material_specular);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,twentyfour_material_diffuse);
				glMaterialfv(GL_FRONT,GL_SHININESS,twentyfour_material_shininess);
				gluSphere(quadric,0.15,30,30);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void update(void){
	XAngle=XAngle+0.8f;
	if(XAngle>=360.0f){
		XAngle=0.0f;
	}

	YAngle=YAngle+0.8f;
	if(YAngle>=360.0f){
		YAngle=0.0f;
	}

	ZAngle=ZAngle+0.8f;
	if(ZAngle>=360.0f){
		ZAngle=0.0f;
	}
}

void KeyRotation(void){
	if(gXRotate==GL_TRUE){
			glPushMatrix();
			glRotatef(XAngle,1.0f,0.0f,0.0f);
			one_light_position[1]=XAngle;
			glLightfv(GL_LIGHT0,GL_POSITION,one_light_position);
			glPopMatrix();
		}else if(gYRotate==GL_TRUE){
			glPushMatrix();
			glRotatef(YAngle,0.0f,1.0f,0.0f);
			one_light_position[0]=YAngle;
			glLightfv(GL_LIGHT0,GL_POSITION,one_light_position);	
			glPopMatrix();
		}else if(gZRotate==GL_TRUE){
			glPushMatrix();
			glRotatef(ZAngle,0.0f,0.0f,1.0f);
			one_light_position[0]=ZAngle;
			glLightfv(GL_LIGHT0,GL_POSITION,one_light_position);
			glPopMatrix();
		}
}