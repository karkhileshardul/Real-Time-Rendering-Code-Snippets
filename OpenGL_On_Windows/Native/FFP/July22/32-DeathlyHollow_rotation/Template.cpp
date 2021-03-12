#include<windows.h>
#include<stdio.h>
#include<math.h>

#include<gl/GL.h>
#include <stdio.h>

#define PI 3.1415

#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

void update(void);
float angleTri = 0.0f;

typedef struct color {
	float red, green, blue;
}color_t;

GLfloat ax = -1.0f, ay = -1.0f;
GLfloat bx = 1.0f, by = -1.0f;
GLfloat cx = 0.0f, cy = 1.0f;

GLint circle_points = 10000;
GLfloat angle;



GLfloat distance_A = 0.0f;
GLfloat distance_B = 0.0f;
GLfloat distance_C = 0.0f;

GLfloat incenter_x = 0.0f;
GLfloat incenter_y = 0.0f;

GLfloat perimeter = 0.0f;
GLfloat semi_perimeter = 0.0f;
GLfloat area_of_triangle = 0.0f;

GLfloat radius = 0.0f;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
FILE *gpFile = NULL;

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{


	void initialize(void);
	void uninitialize(void);
	void display(void);


	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("32-DeathlyHollow_Rotation");
	bool bDone = false;

	//code

	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExiting!!!\n\n"), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened\n\n");
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("32-DeathlyHollow_Rotation Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);

	SetFocus(hwnd);

	initialize();

	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			

			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}

			update();
			display();
		}
	}

	uninitialize();

	return((int)msg.wParam);
}


//wndproc()

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function prototype
	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	//code

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:

			gbEscapeKeyIsPressed = true;
			break;
		case 0x46:
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
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
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	//variable declaration
	MONITORINFO mi = { sizeof(MONITORINFO) };

	dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
	if (gbFullscreen == false)
	{
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left,

					mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		//code
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}



void initialize(void)
{
	//function prototype
	void uninitialize(void);
	void resize(int, int);

	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearColor(0.50f, 0.25f, 0.25f, 0.0f);

	resize(WIN_WIDTH,WIN_HEIGHT);
}

void update() {

	angleTri = angleTri + 0.1f;

	if (angleTri >= 360.0f) {

		angleTri = 0.0f;
	}
}


void display(void)
{
	//function prototypes
	void DrawTriangle(void);
	void DrawLine(void);
	void DrawCircle(void);



	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glScalef(0.2f, 0.2f, 0.0f);

	DrawLine();

	glRotatef(angleTri,0.0f, 1.0f, 0.0f);
	DrawTriangle();
	DrawCircle();


	SwapBuffers(ghdc);

}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}


void uninitialize(void)
{
	//code
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	if (gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed. \n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

void DrawTriangle(void)
{
	glLineWidth(3.0f);

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f); //black color
	glVertex3f(0.0f, 1.0f, 0.0f); // x1 , y1 
	glVertex3f(-1.0f, -1.0f, 0.0f); // x2, y2 

	glVertex3f(-1.0f, -1.0f, 0.0f); // x1,y1
	glVertex3f(1.0f, -1.0f, 0.0f); // x2,y2

	glVertex3f(1.0f, -1.0f, 0.0f); //x1,y1
	glVertex3f(0.0f, 1.0f, 0.0f); //x2,y2

	glEnd();

}

void DrawLine(void)
{
	glLineWidth(3.0f);

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glEnd();
}

void DrawCircle()
{
	//code



	distance_A = sqrt(pow(ax - bx, 2) + pow(ay - by, 2));
	distance_B = sqrt(pow(bx - cx, 2) + pow(by - cy, 2));
	distance_C = sqrt(pow(ax - cx, 2) + pow(ay - cy, 2));

	incenter_x = (distance_A * cx + distance_B * ax + distance_C * bx) / (distance_A + distance_B + distance_C);
	incenter_y = (distance_A * cy + distance_B * ay + distance_C * by) / (distance_A + distance_B + distance_C);

	perimeter = distance_A + distance_B + distance_C;
	semi_perimeter = (distance_A + distance_B + distance_C) / 2;

	radius = sqrt(semi_perimeter * (semi_perimeter - distance_A) * (semi_perimeter - distance_B) * (semi_perimeter - distance_C)) / semi_perimeter;
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < circle_points; i++)
	{
		angle = 2 * PI * i / circle_points;
		glColor3f(1.0f, 0.0f, 0.0f); //black color
		glVertex3f((radius * cos(angle)) + incenter_x, (radius * sin(angle)) + incenter_y, 0.0f);
	}

	glEnd();

	fprintf(gpFile, "distance A = %f distance B = %f distance C = %f perimeter = %f incenter X = %f incenter Y = %f semiperimeter = %f radius = %f\n", distance_A, distance_B, distance_C, perimeter, incenter_x, incenter_y, semi_perimeter, radius);
}

