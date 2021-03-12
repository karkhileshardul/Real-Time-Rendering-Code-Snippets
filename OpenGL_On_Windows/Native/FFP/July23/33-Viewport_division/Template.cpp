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
int sx, sy;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void disp_triangle(void);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("33-Viewport_division");
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
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szClassName, TEXT("33-Viewport_division Window"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, (nxIndex / 2 - (WIN_WIDTH / 2)), (nyIndex / 2 - (WIN_HEIGHT / 2)), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
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
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
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
		case 0x31:
		case VK_NUMPAD1:
			glViewport(0, (GLsizei)sy / 2, (GLsizei)sx / 2, (GLsizei)sy / 2);
			break;
		case 0x32:
		case VK_NUMPAD2:
			glViewport((GLsizei)sx / 2,(GLsizei)sy/2, (GLsizei)sx / 2, (GLsizei)sy / 2);
			break;
		case 0x33:
		case VK_NUMPAD3:
			glViewport(0,0, (GLsizei)sx/2, (GLsizei)sy/2 );
			break;
		case 0x34:
		case VK_NUMPAD4:
			glViewport((GLsizei)sx / 2, 0, (GLsizei)sx / 2, (GLsizei)sy / 2);
			break;
		case 0x35:
		case VK_NUMPAD5:
			glViewport(0, 0, (GLsizei)sx / 2, (GLsizei)sy);
			break;
		case 0x36:
		case VK_NUMPAD6:
			glViewport((GLsizei)sx/2, 0 , (GLsizei)sx/2, (GLsizei)sy);
			break;
		case 0x37:
		case VK_NUMPAD7:
			glViewport(0, (GLsizei)sy / 2, (GLsizei)sx, (GLsizei)sy / 2);
			break;
		case 0x38:
		case VK_NUMPAD8:
			glViewport(0, 0, (GLsizei)sx, (GLsizei)sy / 2);
			break;
		case 0x39:
		case VK_NUMPAD9:
			glViewport((GLsizei)sx/3, (GLsizei)sy / 3, (GLsizei)sx / 3, (GLsizei)sy / 3);
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

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	resize(WIN_WIDTH, WIN_HEIGHT);
}


void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	disp_triangle();

	SwapBuffers(ghdc);
}

void resize(int width, int height) {
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
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

void  disp_triangle(void) {


	glBegin(GL_TRIANGLES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glEnd();

}