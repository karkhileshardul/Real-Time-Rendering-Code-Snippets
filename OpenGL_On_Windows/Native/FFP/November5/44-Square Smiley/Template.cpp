#include<windows.h>
#include<gl/Gl.h>
#include<gl/GLU.h>
#include"Header.h"

#define WIN_WIDTH	800
#define WIN_HEIGHT	600

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"gdi32.lib")

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

HWND ghwnd=NULL;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
GLuint Texture_Smiley;
bool gbActiveWindow=false;
bool gbEscapeKeyIsPressed=false;
bool gbFullscreen=false;



int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	
	void initialize(void);
	void display(void);
	void uninitialize(void);

	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szClassName[]=TEXT("44-Square Smiley");
	bool bDone=false;
	int nxIndex, nyIndex;
	

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.style=CS_HREDRAW |CS_VREDRAW |CS_OWNDC;
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.hInstance=hInstance;
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.lpszClassName=szClassName;
	wndclassex.lpszMenuName=NULL;
	

	RegisterClassEx(&wndclassex);

	nxIndex = GetSystemMetrics(SM_CXSCREEN);
	nyIndex = GetSystemMetrics(SM_CYSCREEN);

	hwnd=CreateWindowEx(WS_EX_APPWINDOW,szClassName,TEXT("44-Square Smile Texture Window"),
						WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
						(nxIndex / 2 - (WIN_WIDTH / 2)), (nyIndex / 2 - (WIN_HEIGHT / 2)),
						WIN_WIDTH,WIN_HEIGHT,NULL,NULL,hInstance,NULL);
	ghwnd=hwnd;
	if (hwnd == NULL) {
		MessageBox(hwnd, TEXT("CreateWindowEx() FAILED!!!"), TEXT("ERROR"), MB_OKCANCEL);
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
	 		}
	 		display();

	 	}
	}

	uninitialize();
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	void resize(int,int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	switch(iMsg){
		case WM_ACTIVATE:
			if(HIWORD(wParam)==0){
				gbActiveWindow=true;
			}
			else{
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
				default:
					break; 
			}
			break;
		case WM_LBUTTONDOWN:
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

void ToggleFullscreen(void){
	MONITORINFO mi;
	if(gbFullscreen==false){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW){
			mi={sizeof(MONITORINFO)};
			if(GetWindowPlacement(ghwnd,&wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi)){
				SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,
							mi.rcMonitor.right-mi.rcMonitor.left,mi.rcMonitor.bottom-mi.rcMonitor.top,
							SWP_NOZORDER |SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else{
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER |
					SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

void initialize(void){

	void resize(int,int);
	int LoadGLtextures(GLuint *,TCHAR[]);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion=1;
	pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType=PFD_TYPE_RGBA;
	pfd.cColorBits=32;
	pfd.cRedBits=8;
	pfd.cGreenBits=8;
	pfd.cBlueBits=8;
	pfd.cAlphaBits=8;
	pfd.cDepthBits=24;

	ghdc=GetDC(ghwnd);

	iPixelFormatIndex=ChoosePixelFormat(ghdc,&pfd);
	if(iPixelFormatIndex==0){
		ReleaseDC(ghwnd,ghdc);
		ghwnd=NULL;
		ghdc=NULL;
	}
	if(SetPixelFormat(ghdc,iPixelFormatIndex,&pfd)==false){
		ReleaseDC(ghwnd,ghdc);
		ghwnd=NULL;
		ghdc=NULL;
	}
	ghrc=wglCreateContext(ghdc);
	if(ghrc==NULL){
		ReleaseDC(ghwnd,ghdc);
		ghwnd=NULL;
		ghdc=NULL;
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
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	LoadGLtextures(&Texture_Smiley,MAKEINTRESOURCE(IDBITMAP_SMILEY));
	resize(WIN_WIDTH,WIN_HEIGHT);
}

int LoadGLtextures(GLuint *texture,TCHAR imageResourceId[]){
	HBITMAP hBitmap;
	BITMAP bmp;
		
		hBitmap=(HBITMAP)LoadImage(GetModuleHandle(NULL),imageResourceId,
								IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION);
		GetObject(hBitmap,sizeof(bmp),&bmp);
		glGenTextures(1,texture);
		glBindTexture(GL_TEXTURE_2D,*texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D,3,bmp.bmWidth,bmp.bmHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE,bmp.bmBits);
		DeleteObject(hBitmap);

	return (TRUE);
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

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.0f);

	glBindTexture(GL_TEXTURE_2D,Texture_Smiley);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
			
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(1.0f,-1.0f,0.0f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(1.0f,1.0f,0.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(-1.0f,1.0f,0.0f);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(-1.0f,-1.0f,0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	SwapBuffers(ghdc);
}

void uninitialize(void){
	if(Texture_Smiley){
		glDeleteTextures(1,&Texture_Smiley);
		Texture_Smiley=0;
	}

	if(gbFullscreen==true){
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