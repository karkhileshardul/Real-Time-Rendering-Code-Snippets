#include<Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<math.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define M_PI 3.1458

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
GLfloat anglePyramid_degree =0.0f;
GLfloat anglePyramid_radian = 0.0f;

GLfloat translationMatrix[16];
GLfloat scaleMatrix[16];
GLfloat identityMatrix[16];
GLfloat rotation_x_Matrix[16];
GLfloat rotation_y_Matrix[16];
GLfloat rotation_z_Matrix[16];


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void disp_Pyaramid(void);
void update(void);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("37-3dPyramid_2");
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
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szClassName, TEXT("37-3dPyramid_2 Window"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, (nxIndex / 2 - (WIN_WIDTH / 2)), (nyIndex / 2 - (WIN_HEIGHT / 2)), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
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

	
		identityMatrix[0] = 1.0f;
		identityMatrix[1] = 0.0f;
		identityMatrix[2] = 0.0f;
		identityMatrix[3] = 0.0f;
		identityMatrix[4] = 0.0f;
		identityMatrix[5] = 1.0f;
		identityMatrix[6] = 0.0f;
		identityMatrix[7] = 0.0f;
		identityMatrix[8] = 0.0f;
		identityMatrix[9] = 0.0f;
		identityMatrix[10] = 1.0f;
		identityMatrix[11] = 0.0f;
		identityMatrix[12] = 0.0f;
		identityMatrix[13] = 0.0f;
		identityMatrix[14] = 0.0f;
		identityMatrix[15] = 1.0f;

	
		translationMatrix[0] = 1.0f;
		translationMatrix[1] = 0.0f;
		translationMatrix[2] = 0.0f;
		translationMatrix[3] = 0.0f;
		translationMatrix[4] = 0.0f;
		translationMatrix[5] = 1.0f;
		translationMatrix[6] = 0.0f;
		translationMatrix[7] = 0.0f;
		translationMatrix[8] = 0.0f;
		translationMatrix[9] = 0.0f;
		translationMatrix[10] = 1.0f;
		translationMatrix[11] = 0.0f;
		translationMatrix[12]= 0.0f;
		translationMatrix[13]= 0.0f;
		translationMatrix[14] = -6.0f;
		translationMatrix[15] = 1.0f;
	
	
		scaleMatrix[0] = 0.75f;
		scaleMatrix[1] = 0.0f;
		scaleMatrix[2]= 0.0f;
		scaleMatrix[3]= 0.0f;
		scaleMatrix[4]= 0.0f;
		scaleMatrix[5] = 0.75f;
		scaleMatrix[6] = 0.0f;
		scaleMatrix[7]= 0.0f;
		scaleMatrix[8]= 0.0f;
		scaleMatrix[9]= 0.0f;
		scaleMatrix[10] = 0.75f;
		scaleMatrix[11]= 0.0f;
		scaleMatrix[12]= 0.0f;
		scaleMatrix[13]= 0.0f;
		scaleMatrix[14]= 0.0f;
		scaleMatrix[15] = 1.0f;
	
		
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

	glLoadMatrixf(identityMatrix);
	glMultMatrixf(translationMatrix);
	glMultMatrixf(scaleMatrix);

	anglePyramid_radian = anglePyramid_degree*(M_PI / 180.0f);
	
		rotation_x_Matrix[0] = 1.0f;
		rotation_x_Matrix[1]= 0.0f;
		rotation_x_Matrix[2] = 0.0f;
		rotation_x_Matrix[3]= 0.0f;
		rotation_x_Matrix[4]= 0.0f;
		rotation_x_Matrix[5] = 1.0f;
		rotation_x_Matrix[6] = 0.0f;
		rotation_x_Matrix[7]= 0.0f;
		rotation_x_Matrix[8] = 0.0f;
		rotation_x_Matrix[9] = 0.0f;
		rotation_x_Matrix[10] = 1.0f;
		rotation_x_Matrix[11]= 0.0f;
		rotation_x_Matrix[12]= 0.0f;
		rotation_x_Matrix[13]= 0.0f;
		rotation_x_Matrix[14]= 0.0f;
		rotation_x_Matrix[15] = 1.0f;

		glMultMatrixf(rotation_x_Matrix);

		rotation_y_Matrix[0] = cos(anglePyramid_radian);
		rotation_y_Matrix[1]= 0.0f;
		rotation_y_Matrix[2] = -sin(anglePyramid_radian);
		rotation_y_Matrix[3] = 0.0f;
		rotation_y_Matrix[4]= 0.0f;
		rotation_y_Matrix[5] = 1.0f;
		rotation_y_Matrix[6]= 0.0f;
		rotation_y_Matrix[7]= 0.0f;
		rotation_y_Matrix[8] = sin(anglePyramid_radian);
		rotation_y_Matrix[9]= 0.0f;
		rotation_y_Matrix[10] = cos(anglePyramid_radian);
		rotation_y_Matrix[11]= 0.0f;
		rotation_y_Matrix[12]= 0.0f;
		rotation_y_Matrix[13]= 0.0f;
		rotation_y_Matrix[14]= 0.0f;
		rotation_y_Matrix[15] = 1.0f;
	

		glMultMatrixf(rotation_y_Matrix);

	
		rotation_z_Matrix[0] = 1.0f;
		rotation_z_Matrix[1] = 0.0f;
		rotation_z_Matrix[2] = 0.0f;
		rotation_z_Matrix[3] = 0.0f;
		rotation_z_Matrix[4] = 0.0f;
		rotation_z_Matrix[5] = 1.0f;
		rotation_z_Matrix[6] = 0.0f;
		rotation_z_Matrix[7] = 0.0f;
		rotation_z_Matrix[8] = 0.0f;
		rotation_z_Matrix[9] = 0.0f;
		rotation_z_Matrix[10] = 1.0f;
		rotation_z_Matrix[11] = 0.0f;
		rotation_z_Matrix[12] = 0.0f;
		rotation_z_Matrix[13] = 0.0f;
		rotation_z_Matrix[14] = 0.0f;
		rotation_z_Matrix[15] = 1.0f;

		glMultMatrixf(rotation_z_Matrix);
	
	disp_Pyaramid();

	SwapBuffers(ghdc);
}

void update(void) {
	
	anglePyramid_degree = anglePyramid_degree + 0.1f;

	if (anglePyramid_degree >= 360.0f) {

		anglePyramid_degree = 0.0f;
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