#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"Winmm.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;
void Alphabet_I_1(void);
void Alphabet_N(void);
void Alphabet_D(void);
void Alphabet_I_2(void);
void Alphabet_A(void);
void update(void);
void Strips(void);
void Strips_2(void);
GLfloat i = -1.1000f;	//used in Aplhabet_I1
GLfloat j = 6.0f;		//used in Alphabet_N
GLfloat k = -16.5f;		//used in Alphabet_I2
GLfloat o = -27.5f;		//used in Strips
GLfloat m = 2.7f;		//used in Alphabet_A
GLfloat n = -25.0f;		//used in Strips
GLfloat pqr = -8.5f;	//used in Alphabet_D
GLint Flag = 0;
GLfloat z = -8.8f;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	void initialize(void);
	void display(void);
	void uninitialize(void);

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("India Dynamic Assigment");
	bool bDone = false;

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

	int nxIndex, nyIndex;

	nxIndex = GetSystemMetrics(SM_CXSCREEN);
	nyIndex = GetSystemMetrics(SM_CYSCREEN);


	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("India Dynamic Assignment Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(nxIndex/2 -(WIN_WIDTH/2)),
		(nyIndex/2-(WIN_HEIGHT/2)),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	
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
				update();
				display();
			}
		}
	}

	uninitialize();
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	switch (iMsg)
	{
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
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed == false)
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

void initialize(void)
{
	int iPixelFormatIndex;
	void resize(int, int);

	PIXELFORMATDESCRIPTOR pfd;


	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits=32;

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

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	
	PlaySound(TEXT("Aye Mere Pyare Watan.wav"), 0, SND_LOOP |SND_ASYNC);
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void update(void) {

	if (i <= -0.85f) {
		i = i + 0.00001f;
	}

	if (j >= -0.85f) {
		j = j - 0.0001f;
	}

	if (k <= -0.85f) {
		k = k + 0.0001f;
	}

	if (m >= 0.35f) {
		m = m - 0.00001f;
	}
	if (pqr <= 1.0f) {
		pqr = pqr + 0.0001f;
	}
	if (n <= 0.663f) {
		n = n + 0.0001f;
	}

	if (o <= -0.199f) {
		o = o + 0.0001f;
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	Alphabet_I_1();
	
	Alphabet_N();
	Alphabet_D();
	Alphabet_I_2();
	Alphabet_A();
	Strips();
	SwapBuffers(ghdc);
}

void resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if (width <= height)
	{
		gluPerspective(0.0f, (GLfloat)height / (GLfloat)width, 0.1f, 100.0f);
	}
	else
	{
		gluPerspective(0.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void uninitialize(void)
{
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
	
	DestroyWindow(ghwnd);
}

void Alphabet_I_1(void) {
	glLoadIdentity();
	glTranslatef(i, -0.85f, 0.0f);

	/*Vertical Strip of Alphabet I __START__*/
	glBegin(GL_QUADS);

	glColor3f(0.0f, 0.5f, 0.0f);		/*Green Color*/
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);		/*Orange Color*/
	glVertex3f(0.0f, 1.70f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.02f, 1.70f, 0.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.02f, 0.0f, 0.0f);

	glEnd();
	/*Vertical Strip of Alphabet I __END__*/
}

void Alphabet_N(void) {
	glLoadIdentity();
	glTranslatef(-0.70f, j, 0.0f);

	/*First Vertical Strip of Alphabet N __START__*/
	glBegin(GL_QUADS);

	glColor3f(0.0f, 0.5f, 0.0f);		/*Green Color*/
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);		/*Orange Color*/
	glVertex3f(0.0f, 1.70f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.02f, 1.70f, 0.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.02f, 0.0f, 0.0f);

	glEnd();
	/*First Vertical Strip of Alphabet N __END__*/

	glLoadIdentity();
	
	glTranslatef(-0.70f, j, 0.0f);

	/*Slanting Strip of Alphabet N __START__*/
	glBegin(GL_QUAD_STRIP);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, 1.70f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.02f, 1.70f, 0.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.30f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.32f, 0.0f, 0.0f);

	glEnd();
	/*Slanting Strip of Alphabet N __END__*/

	glLoadIdentity();
	glTranslatef(-0.40f, j, -0.0f);

	/*Second Vertical Strip of Alphabet N __START__*/
	glBegin(GL_QUADS);
	
	glColor3f(0.0f, 0.5f, 0.0f);		/*Green Color*/
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);		/*Orange Color*/
	glVertex3f(0.0f, 1.70f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.02f, 1.70f, 0.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.02f, 0.0f, 0.0f);

	glEnd();
	/*Second Vertical Strip of Alphabet N __END__*/
}

void Alphabet_D(void) {
	
	glLoadIdentity();
	glTranslatef(-0.23f, -0.85f, 0.0f);
	glBegin(GL_QUADS);
	/*First Vertical Strip of Alphabet D __START__*/
	
	glColor4f(0.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor4f(0.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.02f, 0.0f, 0.0f);
	glColor4f(1.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.02f, 1.70f, 0.0f);
	glColor4f(1.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.0f, 1.70f, 0.0f);
	/*First Vertical Strip of Alphabet D __END__*/
	glEnd();
	
	glLoadIdentity();
	glTranslatef(0.05f, -0.85f, 0.0f);
	glBegin(GL_QUADS);
	/*Second Vertical Strip of Alphabet D __START__*/
	glColor4f(0.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor4f(0.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.02f, 0.0f, 0.0f);
	glColor4f(1.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.02f, 1.70f, 0.0f);
	glColor4f(1.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.0f, 1.70f, 0.0f);

	/*Second Vertical Strip of Alphabet D __END__*/
	glEnd();
		
	glLoadIdentity();
	glTranslatef(-0.25f, -0.85f, 0.0f);
	
	glBegin(GL_QUADS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/*Downer Horizontal Strip of Alphabet D __START__*/
	glColor4f(0.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor4f(0.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.0f, 0.02f, 0.0f);
	glColor4f(0.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.32f, 0.02f, 0.0f);
	glColor4f(0.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.32f, 0.0f, 0.0f);
	/*Downer Horizontal Strip of Alphabet D __END__*/
	glEnd();


	glLoadIdentity();
	glTranslatef(-0.25f, -0.85f, 0.0f);
	glBegin(GL_QUADS);
	/*Upper Horizontal Strip of Alphabet D __START__*/
	glColor4f(1.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.0f, 1.68f, 0.0f);
	glColor4f(1.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.0f, 1.70f, 0.0f);
	glColor4f(1.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.32f, 1.70f, 0.0f);
	glColor4f(1.0f, 0.5f, 0.0f,pqr);
	glVertex3f(0.32f, 1.68f, 0.0f);
	/*Upper Horizontal Strip of Alphabet D __END__*/
	glEnd();

}

void Alphabet_I_2(void) {

	glLoadIdentity();
	glTranslatef(0.20f, k, 0.0f);

	/*Vertical Strip of Alphabet I __START__*/
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.5f, 0.0f);		/*Green Color*/
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);		/*Orange Color*/
	glVertex3f(0.0f, 1.70f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.02f, 1.70f, 0.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.02f, 0.0f, 0.0f);
	/*Vertical Strip of Alphabet I __END__*/
	glEnd();

}

void Alphabet_A(void) {
	
	glLoadIdentity();
	glTranslatef(m, -0.85f, 0.0f);
	//glTranslatef(0.35f, -0.85f, 0.0f);
	
	/*Slanting Strips of Alphabet A __START__*/
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.02f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.19f, 1.70f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.21f, 1.70f, 0.0f);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.42f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.44f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.20f, 1.70f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.22f, 1.70f, 0.0f);
	glEnd();
	/*Slanting Strips of Alphabet A __END__*/

}

void Strips(void) {

	/*Horizontal Strip(Orange Colour) of Alphabet A __START__*/
	glLoadIdentity();
	glTranslatef(n, -0.85f, 0.0f);
	
	glLineWidth(12.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, 0.85f, 0.0f);
	glVertex3f(o,0.85f, 0.0f);
	glEnd();
	/*Horizontal Strip(Orange Colour) of Alphabet A __END__*/

	glLineWidth(8.0f);
	/*Horizontal Strip(White Colour) of Alphabet A __START__*/
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.84f, 0.0f);
	glVertex3f(o, 0.84f, 0.0f);
	glEnd();
	/*Horizontal Strip(White Colour) of Alphabet A __END__*/

	glLineWidth(4.0f);
	/*Horizontal Strip(Green Colour) of Alphabet A __START__*/
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, 0.83f, 0.0f);
	glVertex3f(o, 0.83f, 0.0f);
	glEnd();
	/*Horizontal Strip(Green Colour) of Alphabet A __END__*/
}
