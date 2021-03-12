

#include<Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

HWND ghwnd;
bool bDone = false;
bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(wpPrev) };
HDC ghdc;
HGLRC ghrc;
GLfloat anglePyaramid = 0.0f;
GLfloat angleCube = 0.0f;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void disp_Pyaramid(void);
void disp_Cube(void);
void update(void);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("36 - 3dCubeAndPyramid");
	int nxIndex, nyIndex;

	void initialize(void);
	void uninitialize(void);
	void display(void);

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wndclass);

	nxIndex = GetSystemMetrics(SM_CXSCREEN);
	nyIndex = GetSystemMetrics(SM_CYSCREEN);
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szClassName, TEXT("36 - 3dCubeAndPyramid Window"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, (nxIndex / 2 - (WIN_WIDTH / 2)), (nyIndex / 2 - (WIN_HEIGHT / 2)), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
	ghwnd = hwnd;
	if (hwnd == NULL) {
		MessageBox(hwnd, TEXT("CreateWindowEx() FAILED!!!"), TEXT("ERROR"), MB_OKCANCEL);
		exit(EXIT_FAILURE);
	}

	initialize();
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);


	while (bDone == false) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				bDone = true;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			if (gbActiveWindow == true) {
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
				update();
				display();
			}
		}
	}


	return((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	void display(void);
	void resize(int, int);

	void uninitialize(void);

	void ToggleFullscreen(void);

	switch (iMsg) {
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (LOWORD(wParam)) {
		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;
			break;
		case 0x46:
			if (gbFullscreen == false) {
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else {
				ToggleFullscreen();
				gbFullscreen = false;
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
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}


void ToggleFullscreen(void) {

	BOOL bWindowPlacement = FALSE;
	BOOL bMonitorInfo = FALSE;
	MONITORINFO mi = { sizeof(mi) };
	HMONITOR hMonitor;
	if (gbFullscreen == false) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		bWindowPlacement = GetWindowPlacement(ghwnd, &wpPrev);
		hMonitor = MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY);
		bMonitorInfo = GetMonitorInfo(hMonitor, &mi);
		if (dwStyle & WS_OVERLAPPEDWINDOW) {
			if (bMonitorInfo && bWindowPlacement) {
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
	}
}

void initialize(void) {
	void resize(int, int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	BOOL bPixelFormat = FALSE;
	BOOL bMakeCurrent = FALSE;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		ghwnd = NULL;
	}

	bPixelFormat = SetPixelFormat(ghdc, iPixelFormatIndex, &pfd);

	if (bPixelFormat == FALSE) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		ghwnd = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		ghwnd = NULL;
	}

	bMakeCurrent = wglMakeCurrent(ghdc, ghrc);
	if (bMakeCurrent == FALSE) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		ghwnd = NULL;
	}

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_LEQUAL);

	resize(WIN_WIDTH, WIN_HEIGHT);
}


void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -6.0f);
	glRotatef(anglePyaramid, 0.0f, 1.0f, 0.0f);
	disp_Pyaramid();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 0.0f, -6.0f);
	glScalef(0.75f, 0.75f, 0.75f);
	glRotatef(angleCube, 1.0f, 1.0f, 1.0f);
	disp_Cube();
	SwapBuffers(ghdc);
}

void update(void) {

	anglePyaramid = anglePyaramid + 0.1f;

	if (anglePyaramid >= 360.0f) {

		anglePyaramid = 0.0f;
	}

	angleCube = angleCube + 0.1f;

	if (angleCube >= 360.0f) {

		angleCube = 0.0f;
	}



}

void resize(int width, int height) {
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void uninitialize(void) {
	if (gbFullscreen == true) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;
	ghwnd = NULL;
	DestroyWindow(ghwnd);
}


void disp_Pyaramid(void) {
	glBegin(GL_TRIANGLES);
	//FRONT FACE
	glColor3f(1.0f, 0.0f, 0.0f); //red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex

	glColor3f(0.0f, 1.0f, 0.0f); //green
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-corner of front face

	glColor3f(0.0f, 0.0f, 1.0f); //blue
	glVertex3f(1.0f, -1.0f, 1.0f); //right-corner of front face

								   //RIGHT FACE
	glColor3f(1.0f, 0.0f, 0.0f); //red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex

	glColor3f(0.0f, 0.0f, 1.0f); //blue
	glVertex3f(1.0f, -1.0f, 1.0f); //left-corner of right face

	glColor3f(0.0f, 1.0f, 0.0f); //green
	glVertex3f(1.0f, -1.0f, -1.0f); //right-corner of right face

									//BACK FACE
	glColor3f(1.0f, 0.0f, 0.0f); //red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex

	glColor3f(0.0f, 1.0f, 0.0f); //green
	glVertex3f(1.0f, -1.0f, -1.0f); //left-corner of back face

	glColor3f(0.0f, 0.0f, 1.0f); //blue
	glVertex3f(-1.0f, -1.0f, -1.0f); //right-corner of back face

									 //LEFT FACE
	glColor3f(1.0f, 0.0f, 0.0f); //red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex

	glColor3f(0.0f, 0.0f, 1.0f); //blue
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-corner of left face

	glColor3f(0.0f, 1.0f, 0.0f); //green
	glVertex3f(-1.0f, -1.0f, 1.0f); //right-corner of left face
	glEnd();

}

void disp_Cube(void) {

	glBegin(GL_QUADS);
	//TOP FACE
	glColor3f(1.0f, 0.0f, 0.0f); //RED
	glVertex3f(1.0f, 1.0f, -1.0f);  //right-top corner of top face
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-top corner of top face
	glVertex3f(-1.0f, 1.0f, 1.0f); //left-bottom corner of top face
	glVertex3f(1.0f, 1.0f, 1.0f); //right-bottom corner of top face

								  //BOTTOM FACE
	glColor3f(0.0f, 1.0f, 0.0f); //GREEN
	glVertex3f(1.0f, -1.0f, -1.0f); //right-top corner of bottom face
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-top corner of bottom face
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-bottom corner of bottom face
	glVertex3f(1.0f, -1.0f, 1.0f); //right-bottom corner of bottom face

								   //FRONT FACE
	glColor3f(0.0f, 0.0f, 1.0f); //BLUE
	glVertex3f(1.0f, 1.0f, 1.0f); //right-top corner of front face
	glVertex3f(-1.0f, 1.0f, 1.0f); //left-top corner of front face
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-bottom corner of front face
	glVertex3f(1.0f, -1.0f, 1.0f); //right-bottom corner of front face

								   //BACK FACE
	glColor3f(0.0f, 1.0f, 1.0f); //CYAN
	glVertex3f(1.0f, 1.0f, -1.0f); //right-top of back face
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-top of back face
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-bottom of back face
	glVertex3f(1.0f, -1.0f, -1.0f); //right-bottom of back face

									//RIGHT FACE
	glColor3f(1.0f, 0.0f, 1.0f); //MAGENTA
	glVertex3f(1.0f, 1.0f, -1.0f); //right-top of right face
	glVertex3f(1.0f, 1.0f, 1.0f); //left-top of right face
	glVertex3f(1.0f, -1.0f, 1.0f); //left-bottom of right face
	glVertex3f(1.0f, -1.0f, -1.0f); //right-bottom of right face

									//LEFT FACE
	glColor3f(1.0f, 1.0f, 0.0f); //YELLOW
	glVertex3f(-1.0f, 1.0f, 1.0f); //right-top of left face
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-top of left face
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-bottom of left face
	glVertex3f(-1.0f, -1.0f, 1.0f); //right-bottom of left face

	glEnd();


}