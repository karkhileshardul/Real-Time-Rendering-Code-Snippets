#include<windows.h>
#include<stdio.h>
#include<gl\glew.h>
#include<gl/GL.h>
#include"vmath.h"

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
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

FILE *gpFile=NULL;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

HWND ghwnd=NULL;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
DWORD dwStyle;
bool gbActiveWindow=false;
bool gbEscapeKeyIsPressed=false;
bool gbFullscreen=false;

vmath::mat4 gPerspectiveProjectionMatrix;
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gVao_pyramid;
GLuint gVbo_pyramid_position;
GLuint gVbo_pyramid_color;
GLuint gVao_cube;
GLuint gVbo_cube_position;
GLuint gVbo_cube_color;
GLuint gMVPUniform;
GLfloat cube_angle=0.0f;
GLfloat pyramid_angle=0.0f;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	WNDCLASSEX wndclassex;
	TCHAR szClassName[]=TEXT("09-3D Animation Triangle & Square Window");
	MSG msg;
	HWND hwnd;
	bool bDone=false;

	void initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);
	
	if(fopen_s(&gpFile,"SSK_Log File.txt","w")!=0){
		MessageBox(NULL,TEXT("Log File cannot be created\n..Exiting!!!"),
					TEXT("ERROR!!!"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
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

	hwnd=CreateWindow(szClassName,szClassName,
						WS_OVERLAPPEDWINDOW,
						100,100,
						WIN_WIDTH,WIN_HEIGHT,
						NULL,NULL,
						hInstance,NULL);
	ghwnd=hwnd;
	if(!hwnd){
		MessageBox(NULL,TEXT("CreateWindow() failed!!!1"),
					TEXT("ERROR!!!!"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
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
			update();
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
	pfd.cBlueBits=8;
	pfd.cRedBits=8;
	pfd.cGreenBits=8;
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
	const GLfloat pyramidVertices[]=
	{	
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,1.0f,
		1.0f,-1.0f,1.0f,
		0.0f,1.0f,0.0f,
		1.0f,-1.0f,1.0f,
		1.0f,-1.0f,-1.0f,
		0.0f,1.0f,0.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,1.0f
	};

	const GLfloat pyramidColors[]=
	{	
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,
		1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,
		0.0f,1.0f,0.0f,
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,
		1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,
		0.0f,1.0f,0.0f
	};
/*pyramid ___START___*/
	glGenVertexArrays(1,&gVao_pyramid);
	glBindVertexArray(gVao_pyramid);
		glGenBuffers(1,&gVbo_pyramid_position);
		glBindBuffer(GL_ARRAY_BUFFER,gVbo_pyramid_position);
		glBufferData(GL_ARRAY_BUFFER,sizeof(pyramidVertices),
						pyramidVertices,GL_STATIC_DRAW);
		glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX,3,GL_FLOAT,
								GL_FALSE,0,NULL);
		glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
		glBindBuffer(GL_ARRAY_BUFFER,0);

		glGenBuffers(1,&gVbo_pyramid_color);
		glBindBuffer(GL_ARRAY_BUFFER,gVbo_pyramid_color);
		glBufferData(GL_ARRAY_BUFFER,sizeof(pyramidColors),
						pyramidColors,GL_STATIC_DRAW);
		glVertexAttribPointer(SSK_ATTRIBUTE_COLOR,3,GL_FLOAT,
								GL_FALSE,0,NULL);
		glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER,0);
/*pyramid ___END___*/

	GLfloat cubeVertices[]=
	{	
		1.0f,1.0f,-1.0f,
		-1.0f,1.0f,-1.0f,
		-1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,

		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,1.0f,
		1.0f,-1.0f,1.0f,
		
		1.0f,1.0f,1.0f,
		-1.0f,1.0f,1.0f,
		-1.0f,-1.0f,1.0f,
		1.0f,-1.0f,1.0f,
		
		1.0f,1.0f,-1.0f,
		-1.0f,1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		
		1.0f,1.0f,-1.0f,
		1.0f,1.0f,1.0f,
		1.0f,-1.0f,1.0f,
		1.0f,-1.0f,-1.0f,
		
		-1.0f,1.0f,1.0f,
		-1.0f,1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,1.0f
	};

	for(GLuint i=0;i<(24*3);i++){
		if(cubeVertices[i]<0.0f){
			cubeVertices[i]=cubeVertices[i]+0.25f;
		}else if(cubeVertices[i]>0.0f){
			cubeVertices[i]=cubeVertices[i]-0.25f;
		}else{
			cubeVertices[i]=cubeVertices[i];
		}
	}

	const GLfloat cubeColors[]=
	{	
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
	
		1.0f,0.5f,0.0f,
		1.0f,0.5f,0.0f,
		1.0f,0.5f,0.0f,
		1.0f,0.5f,0.0f,
		
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,

		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
	
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
	
		1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,
	};

/*Cube ___START___*/
	glGenVertexArrays(1,&gVao_cube);
	glBindVertexArray(gVao_cube);

		glGenBuffers(1,&gVbo_cube_position);
		glBindBuffer(GL_ARRAY_BUFFER,gVbo_cube_position);
		glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),
					 cubeVertices,GL_STATIC_DRAW);
		glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX,3,GL_FLOAT,
								GL_FALSE,0,NULL);
		glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);

		glBindBuffer(GL_ARRAY_BUFFER,0);
	
		glGenBuffers(1,&gVbo_cube_color);
		glBindBuffer(GL_ARRAY_BUFFER,gVbo_cube_color);
		glBufferData(GL_ARRAY_BUFFER,sizeof(cubeColors),
						cubeColors,GL_STATIC_DRAW);
		glVertexAttribPointer(SSK_ATTRIBUTE_COLOR,3,GL_FLOAT,
								GL_FALSE,0,NULL);
		glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);	
	glBindBuffer(GL_ARRAY_BUFFER,0);
/*Cube ___END___*/
	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
//	glEnable(GL_CULL_FACE); /*Appears transparent so keept commented*/
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

	if(gVao_pyramid){
		glDeleteVertexArrays(1,&gVao_pyramid);
		gVao_pyramid=0;
	}
	if(gVbo_pyramid_position){
		glDeleteBuffers(1,&gVbo_pyramid_position);
		gVbo_pyramid_position=0;
	}
	if(gVbo_pyramid_color){
		glDeleteBuffers(1,&gVbo_pyramid_color);
		gVbo_pyramid_color=0;
	}
	if(gVao_cube){
		glDeleteVertexArrays(1,&gVao_cube);
		gVao_cube=0;
	}
	if(gVbo_cube_position){
		glDeleteBuffers(1,&gVbo_cube_position);
		gVbo_cube_position=0;
	}
	if(gVbo_cube_color){
		glDeleteBuffers(1,&gVbo_cube_color);
		gVbo_cube_color=0;
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

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | 
			GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);
		vmath::mat4 modelViewMatrix=vmath::mat4::identity();
		vmath::mat4 rotationMatrix=vmath::mat4::identity();
		vmath::mat4 modelViewProjectionMatrix=vmath::mat4::identity();

		modelViewMatrix=vmath::translate(-1.5f,0.0f,-6.0f);
		rotationMatrix=vmath::rotate(pyramid_angle,0.0f,1.0f,0.0f);
		modelViewProjectionMatrix=gPerspectiveProjectionMatrix
									*(modelViewMatrix*rotationMatrix);

		glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);
		glBindVertexArray(gVao_pyramid);
			glDrawArrays(GL_TRIANGLES,0,12);
		glBindVertexArray(0);

		modelViewMatrix=vmath::mat4::identity();
		rotationMatrix=vmath::mat4::identity();
		modelViewMatrix=vmath::translate(1.5f,0.0f,-6.0f);
		rotationMatrix=vmath::rotate(cube_angle,cube_angle,cube_angle);
		
		modelViewProjectionMatrix=gPerspectiveProjectionMatrix
									*(modelViewMatrix*rotationMatrix);

		glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);
		glBindVertexArray(gVao_cube);
			glDrawArrays(GL_TRIANGLE_FAN,0,4);
			glDrawArrays(GL_TRIANGLE_FAN,4,4);
			glDrawArrays(GL_TRIANGLE_FAN,8,4);
			glDrawArrays(GL_TRIANGLE_FAN,12,4);
			glDrawArrays(GL_TRIANGLE_FAN,16,4);
			glDrawArrays(GL_TRIANGLE_FAN,20,4);
		glBindVertexArray(0);
	glUseProgram(0);
	SwapBuffers(ghdc);
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

void update(void){
	pyramid_angle=pyramid_angle+0.1f;
	if(pyramid_angle>=360.0f){
		pyramid_angle=pyramid_angle-360.0f;
	}
	cube_angle=cube_angle+0.1f;
	if(cube_angle>=360.0f){
		cube_angle=cube_angle-360.0f;
	}
}