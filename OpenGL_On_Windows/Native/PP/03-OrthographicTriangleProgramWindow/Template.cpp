#include<windows.h>
#include<stdio.h>
#include<gl\glew.h>
#include<gl/GL.h>
#include"vmath.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH	800
#define WIN_HEIGHT	600

enum{
	SSK_ATTRIBUTE_VERTEX=0,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXTURE0,
};

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

FILE *gpFile=NULL;
HWND ghwnd=NULL;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};

bool gbActiveWindow=false;
bool gbEscapeKeyIsPressed=false;
bool gbFullscreen=false;

vmath::mat4 gOrthographicProjectMatrix;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gMVPUniform;
GLuint gVao;
GLuint gVbo;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLines,int iCmdShow){
	bool bDone=false;
	WNDCLASSEX wndclassex;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[]=TEXT("03-Orthographic Triangle Window");

	void initialize(void);
	void uninitialize(void);
	void display(void);

	if(fopen_s(&gpFile,"SSK_Log.txt","w")!=0){
		MessageBox(NULL,TEXT("Log File cannot be created\nExiting Now..."),
				TEXT("!!!ERROR!!!"),MB_OK |MB_TOPMOST |MB_ICONSTOP);
		exit(EXIT_FAILURE);
	}else{
		fprintf(gpFile,"Log File is Successfully opened\n");
	}

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.style=CS_HREDRAW |CS_VREDRAW |CS_OWNDC;
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.lpszClassName=szClassName;
	wndclassex.lpszMenuName=NULL;
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.hInstance=hInstance;
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;

	RegisterClassEx(&wndclassex);

	hwnd=CreateWindow(szClassName,szClassName,WS_OVERLAPPEDWINDOW,
					100,100,WIN_WIDTH,WIN_HEIGHT,
					NULL,NULL,hInstance,NULL);
	ghwnd=hwnd;

	ShowWindow(hwnd,iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	initialize();

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
	void uninitialize(void);
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
				default:
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
	bool bGetWindowPlacement=false;
	bool bGetMonitorInfo=false;
	if(gbFullscreen==false){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW){
			mi={sizeof(MONITORINFO)};
			bGetWindowPlacement=GetWindowPlacement(ghwnd,&wpPrev);
			bGetMonitorInfo=GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi);
			if(bGetWindowPlacement && bGetMonitorInfo){
				SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,
							mi.rcMonitor.right-mi.rcMonitor.left,mi.rcMonitor.bottom-mi.rcMonitor.top,
							SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}		
		ShowCursor(FALSE);
	}else{
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE | 
					SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

void resize(int width,int height){
	if(height==0){
		height=1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	//glOrtho(left,right,bottom,top,near,far)
	if(width<=height){
		gOrthographicProjectMatrix=vmath::ortho(-100.0f,100.0f,(-100.0f *(height/width)),
												(100.0f*(height/width)),-100.0f,100.0f);
	}else{
		gOrthographicProjectMatrix=vmath::ortho(-100.0f,100.0f,(-100.0f*(width/height)),
												(100.0f*(width/height)),-100.0f,100.0f);
	}
}

void initialize(void){
	void uninitialize(void);
	void resize(int,int);

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
	pfd.cDepthBits=32;

	ghdc=GetDC(ghwnd);

	iPixelFormatIndex=ChoosePixelFormat(ghdc,&pfd);
	if(iPixelFormatIndex==0){
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	if(SetPixelFormat(ghdc,iPixelFormatIndex,&pfd)==false){
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	ghrc=wglCreateContext(ghdc);
	if(ghrc==NULL){
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	if(wglMakeCurrent(ghdc,ghrc)==false){
		wglDeleteContext(ghrc);
		ghrc=NULL;
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	/*
		GLEW Initialization COde for GLSL
	*/

	GLenum glew_error=glewInit();
	if(glew_error!=GLEW_OK){
		wglDeleteContext(ghrc);
		ghrc=NULL;
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	/*
		VERTEX SHADER  (___START___)
	*/
	gVertexShaderObject=glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode=
	"#version 440" \
	"\n" \
	"in vec4 vPosition;" \
	"uniform mat4 u_mvp_matrix;" \
	"void main(void)" \
	"{" \
	"gl_Position = u_mvp_matrix * vPosition;" \
	"}";

	glShaderSource(gVertexShaderObject,1,(const GLchar **)&vertexShaderSourceCode,NULL);

	glCompileShader(gVertexShaderObject);
	GLint iInfoLogLength=0;
	GLint iShaderCompiledStatus=0;
	char *szInfoLog=NULL;

	glGetShaderiv(gVertexShaderObject,GL_COMPILE_STATUS,&iShaderCompiledStatus);
	if(iShaderCompiledStatus==GL_FALSE){
		glGetShaderiv(gVertexShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
		if(iInfoLogLength > 0){
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject,iInfoLogLength,&written,szInfoLog);
				fprintf(gpFile,"Vertex Shader Compilation Log : %s \n",szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	/*
		VERTEX SHADER (___END___) 
	*/

	/* 
		FRAGMENT SHADER (___START___)
	*/
	gFragmentShaderObject=glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar *fragmentShaderSourceCode=
		"#version 440" \
		"\n" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor=vec4(1.0,1.0,1.0,1.0);" \
		"}";

	glShaderSource(gFragmentShaderObject,1,(const GLchar **)&fragmentShaderSourceCode,NULL);
	glCompileShader(gFragmentShaderObject);

	glGetShaderiv(gFragmentShaderObject,GL_COMPILE_STATUS,&iShaderCompiledStatus);
	if(iShaderCompiledStatus==GL_FALSE){
		glGetShaderiv(gFragmentShaderObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
		if(iInfoLogLength > 0){
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject,iInfoLogLength,&written,szInfoLog);
				fprintf(gpFile,"Fragment Shader Compilation Log : %s \n",szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(EXIT_FAILURE);
			}
		}
	}
	/*
		FRAGMENT SHADER (___END___)
	*/


	/*
		SHADER PROGRAM (___START___)
	*/
	gShaderProgramObject=glCreateProgram();
	glAttachShader(gShaderProgramObject,gVertexShaderObject);
	glAttachShader(gShaderProgramObject,gFragmentShaderObject);

	glBindAttribLocation(gShaderProgramObject,SSK_ATTRIBUTE_VERTEX,"vPosition");

	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus=0;
	glGetProgramiv(gShaderProgramObject,GL_LINK_STATUS,&iShaderProgramLinkStatus);
	if(iShaderProgramLinkStatus==GL_FALSE){
		glGetProgramiv(gShaderProgramObject,GL_INFO_LOG_LENGTH,&iInfoLogLength);
		if(iInfoLogLength > 0){
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject,iInfoLogLength,&written,szInfoLog);
				fprintf(gpFile,"Shader Program Link Log: %s\n",szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(EXIT_FAILURE);
			}
		}
	}
	/*
		SHADER PROGRAM (___END___)
	*/

	gMVPUniform=glGetUniformLocation(gShaderProgramObject,"u_mvp_matrix");

	const GLfloat triangleVertices[]=
	{ 	0.0f,50.0f,0.0f,	/* apex */
		-50.0f,-50.0f,0.0f,	/* left-bottom	*/
		50.0f,-50.0f,0.0f	/*	right-bottom	*/
	};

	glGenVertexArrays(1,&gVao);
	glBindVertexArray(gVao);

	glGenBuffers(1,&gVbo);
	glBindBuffer(GL_ARRAY_BUFFER,gVbo);
	glBufferData(GL_ARRAY_BUFFER,sizeof(triangleVertices),triangleVertices,GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.0f,0.0f,0.0f,0.0f);

	gOrthographicProjectMatrix=vmath::mat4::identity();

	resize(WIN_WIDTH,WIN_HEIGHT);
}


void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

		vmath::mat4 modelViewMatrix=vmath::mat4::identity();
		vmath::mat4 modelViewProjectionMatrix=vmath::mat4::identity();

		modelViewProjectionMatrix=gOrthographicProjectMatrix * modelViewMatrix;	/* Multiplication Order Important */

		glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);

		glBindVertexArray(gVao);
			glDrawArrays(GL_TRIANGLES,0,3);
		glBindVertexArray(0);

	glUseProgram(0);
	SwapBuffers(ghdc);
}

void uninitialize(void){
	if(gbFullscreen==true){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER |
					SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if(gVao){
		glDeleteVertexArrays(1,&gVao);
		gVao=0;
	}

	if(gVbo){
		glDeleteBuffers(1,&gVbo);
		gVbo=0;
	}

	glDetachShader(gShaderProgramObject,gVertexShaderObject);
	glDetachShader(gShaderProgramObject,gFragmentShaderObject);

	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject=0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject=0;

	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject=0;

	glUseProgram(0);

	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(ghrc);
	ghrc=NULL;

	ReleaseDC(ghwnd,ghdc);
	ghdc=NULL;

	if(gpFile){
		fprintf(gpFile,"Lof File Successfully Closed.\n");
		fclose(gpFile);
		gpFile=NULL;
	}
}
