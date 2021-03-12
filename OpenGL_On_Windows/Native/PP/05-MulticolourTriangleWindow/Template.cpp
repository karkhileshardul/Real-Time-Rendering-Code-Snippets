#include<windows.h>
#include<stdio.h>
#include<gl\glew.h>
#include<gl/GL.h>
#include"vmath.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH	800
#define WIN_HEIGHT	600

enum{
	SSK_ATTRIBUTE_VERTEX=0,	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,	SSK_ATTRIBUTE_TEXTURE0,
};

FILE *gpFile=NULL;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

HWND ghwnd=NULL;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
bool gbActiveWindow=false;
bool gbEscapeKeyIsPressed=false;
bool gbFullscreen=false;
DWORD dwStyle;
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gVao_triangle;
GLuint gVbo_position;
GLuint gVbo_color;
GLuint gMVPUniform;
vmath::mat4 gPerspectiveProjectionMatrix;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	void display(void);
	void initialize(void);
	void uninitialize(void);

	WNDCLASSEX wndclassex;
	TCHAR szClassName[]=TEXT("05-Multicolour Triangle Window");
	MSG msg;
	HWND hwnd;
	bool bDone=false;

	if(fopen_s(&gpFile,"SSK_Log.txt","w")!=0){
		MessageBox(NULL,TEXT("Log File cannot be created\n..Exiting!!!"),TEXT("ERROR!!!"),
					MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(EXIT_SUCCESS);
	}else{
		fprintf(gpFile,"Log File is created successfully Opened\n");
	}

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.hInstance=hInstance;
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.style=CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclassex.lpszClassName=szClassName;
	wndclassex.lpszMenuName=NULL;

	RegisterClassEx(&wndclassex);

	hwnd=CreateWindow(szClassName,szClassName,WS_OVERLAPPEDWINDOW,
						100,100,
						WIN_WIDTH,WIN_HEIGHT,
						NULL,NULL,
						hInstance,NULL);
	ghwnd=hwnd;
	if(!hwnd){
		MessageBox(NULL,TEXT("CreateWindow() failed!!!1"),
					TEXT("ERROR!!!!"),
					MB_OK | MB_TOPMOST | MB_ICONSTOP);
	}

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

	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	void initialize(void);
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
					}else{
						gbEscapeKeyIsPressed=false;
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
			bGetMonitorInfo=GetMonitorInfo(
							MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi);
			if(bGetWindowPlacement && bGetMonitorInfo){
				SetWindowLong(ghwnd,GWL_STYLE,
							dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,
							mi.rcMonitor.top,
							mi.rcMonitor.right-mi.rcMonitor.left,
							mi.rcMonitor.bottom-mi.rcMonitor.top,
							SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}else{
		SetWindowLong(ghwnd,GWL_STYLE,
						dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE |
					SWP_NOOWNERZORDER | SWP_NOZORDER |
					SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}


void initialize(void){
	void resize(int,int);
	void uninitialize(void);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion=1;
	pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
				PFD_DOUBLEBUFFER;
	pfd.iPixelType=PFD_TYPE_RGBA;
	pfd.cColorBits=32;
	pfd.cGreenBits=8;
	pfd.cBlueBits=8;
	pfd.cRedBits=8;
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

	GLenum glew_error=glewInit();
	if(glew_error!=GLEW_OK){
		wglDeleteContext(ghrc);
		ghrc=NULL;
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	/*VERTEX SHADER*/
	gVertexShaderObject=glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode=
		"#version 430" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"out vec4 out_color;" \
		"uniform mat4 u_mvp_matrix;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_mvp_matrix*vPosition;" \
		"out_color=vColor;" \
		"}";
	
	glShaderSource(gVertexShaderObject,1,
				(const GLchar **)&vertexShaderSourceCode,NULL);		

	glCompileShader(gVertexShaderObject);

	GLint iInfoLogLength=0;
	GLint iShaderCompiledStatus=0;
	char *szInfoLog=NULL;

	glGetShaderiv(gVertexShaderObject,GL_COMPILE_STATUS,
				&iShaderCompiledStatus);

	if(iShaderCompiledStatus==GL_FALSE){
		glGetShaderiv(gVertexShaderObject,GL_INFO_LOG_LENGTH,
					&iInfoLogLength);
		if(iInfoLogLength>0){
			szInfoLog=(char*)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject,iInfoLogLength,
								&written,szInfoLog);
				fprintf(gpFile,"Vertex Shader Compilaton Log: %s\n",szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(EXIT_FAILURE);
			}
		}
	}

	/*FRAGMENT SHADER    */
	gFragmentShaderObject=glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCode=
	"#version 430" \
	"\n" \
	"in vec4 out_color;" \
	"out vec4 FragColor;" \
	"void main(void)" \
	"{" \
	"FragColor=out_color;" \
	"}";

	glShaderSource(gFragmentShaderObject,1,
				(const GLchar**)&fragmentShaderSourceCode,NULL);

	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject,GL_COMPILE_STATUS,
				&iShaderCompiledStatus);

	if(iShaderCompiledStatus==GL_FALSE){
		glGetShaderiv(gFragmentShaderObject,GL_INFO_LOG_LENGTH,
					&iInfoLogLength);
		if(iInfoLogLength>0){
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject,iInfoLogLength,
								&written,szInfoLog);
				fprintf(gpFile,"Fragment Shader Compilation Log :%s\n",
						szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(EXIT_FAILURE);
			}
		}
	}

	gShaderProgramObject=glCreateProgram();

	glAttachShader(gShaderProgramObject,gVertexShaderObject);
	glAttachShader(gShaderProgramObject,gFragmentShaderObject);
	glBindAttribLocation(gShaderProgramObject,SSK_ATTRIBUTE_VERTEX,
						"vPosition");
	glBindAttribLocation(gShaderProgramObject,SSK_ATTRIBUTE_COLOR,
						"vColor");

	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus=0;
	glGetProgramiv(gShaderProgramObject,GL_LINK_STATUS,
					&iShaderProgramLinkStatus);
	if(iShaderProgramLinkStatus==GL_FALSE){
		glGetProgramiv(gShaderProgramObject,GL_INFO_LOG_LENGTH,
						&iInfoLogLength);
		if(iInfoLogLength>0){
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject,iInfoLogLength,
									&written,szInfoLog);
				fprintf(gpFile,"Shader Program Link Log : %s\n",szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(EXIT_FAILURE);
			}
		}
	}

	gMVPUniform=glGetUniformLocation(gShaderProgramObject,"u_mvp_matrix");

	const GLfloat triangleVertices[]=
	{	
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		0.0f,1.0f,0.0f
	};

	const GLfloat triangleColors[]=
	{	
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f
	};

	glGenVertexArrays(1,&gVao_triangle);
	glBindVertexArray(gVao_triangle);

	glGenBuffers(1,&gVbo_position);
	glBindBuffer(GL_ARRAY_BUFFER,gVbo_position);
	glBufferData(GL_ARRAY_BUFFER,sizeof(triangleVertices),
				triangleVertices,GL_STATIC_DRAW);
	
	glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX,3,GL_FLOAT,
						GL_FALSE,0,NULL);

	glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	glGenBuffers(1,&gVbo_color);
	glBindBuffer(GL_ARRAY_BUFFER,gVbo_color);
	glBufferData(GL_ARRAY_BUFFER,sizeof(triangleColors),
				triangleColors,GL_STATIC_DRAW);
	glVertexAttribPointer(SSK_ATTRIBUTE_COLOR,3,GL_FLOAT,
							GL_FALSE,0,NULL);

	glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	gPerspectiveProjectionMatrix=vmath::mat4::identity();
	resize(WIN_WIDTH,WIN_HEIGHT);
}

void uninitialize(void){
	if(gbFullscreen==true){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle |
					WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE |
					SWP_NOOWNERZORDER | SWP_NOZORDER |
					SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if(gVao_triangle){
		glDeleteVertexArrays(1,&gVao_triangle);
		gVao_triangle=0;
	}
	if(gVbo_position){
		glDeleteBuffers(1,&gVbo_position);
		gVbo_position=0;
	}

	if(gVbo_color){
		glDeleteBuffers(1,&gVbo_color);
		gVbo_color=0;
	}	

	glDetachShader(gShaderProgramObject,gVertexShaderObject);
	glDetachShader(gShaderProgramObject,gFragmentShaderObject);

	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject=0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject=0;

	glDeleteShader(gShaderProgramObject);
	gShaderProgramObject=0;

	glUseProgram(0);

	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(ghrc);
	ghrc=NULL;

	ReleaseDC(ghwnd,ghdc);
	ghdc=NULL;
	ghwnd=NULL;

	if(gpFile){
		fprintf(gpFile,"Log File closed successfully.\n");
		fclose(gpFile);
		gpFile=NULL;
	}
}

void resize(int width,int height){
	if(height==0){
		height=1;
	}
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	if(width<=height){
		gPerspectiveProjectionMatrix=vmath::perspective(45.0f,
														(GLfloat)height/(GLfloat)width,
														0.1f,100.0f);	
	}else{
		gPerspectiveProjectionMatrix=vmath::perspective(45.0f,
														(GLfloat)width/(GLfloat)height,
														0.1f,100.0f);
	}
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);
		vmath::mat4 modelViewMatrix=vmath::mat4::identity();
		vmath::mat4 modelViewProjectionMatrix=vmath::mat4::identity();

		modelViewMatrix=vmath::translate(0.0f,0.0f,-6.0f);
		modelViewProjectionMatrix=gPerspectiveProjectionMatrix*modelViewMatrix;

		glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);
		glBindVertexArray(gVao_triangle);
			glDrawArrays(GL_TRIANGLES,0,3);
		glBindVertexArray(0);

	glUseProgram(0);
	SwapBuffers(ghdc);
}