#include<windows.h>
#include<stdio.h>
#include<gl\glew.h>
#include<gl/GL.h>
#include"vmath.h"
#include"Header.h"
#define WIN_WIDTH	800
#define WIN_HEIGHT	600
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

FILE *gpFile=NULL;
bool gbFullscreen=false;
bool gbActiveWindow=false;
bool gbEscapeKeyIsPressed=false;
HWND ghwnd=NULL;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
vmath::mat4 gPerspectiveProjectionMatrix;
GLuint gMVPUniform;
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gVao_square;
GLuint gVbo_square_position;
GLuint gVbo_square_texture;
GLuint gTexture_sampler_uniform;
GLuint gTexture_Smiley;
int digitIsPressed=0;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

enum{
	SSK_ATTRIBUTE_VERTEX,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXTURE0,
};

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	void display(void);
	void initialize(void);
	void uninitialize(void);

	WNDCLASSEX wndclassex;
	MSG msg;
	TCHAR szClassName[]=TEXT("11-Smiley Window");
	HWND hwnd;
	bool bDone=false;

	if(fopen_s(&gpFile,"SSK_LogFile.txt","w")!=0){
		MessageBox(NULL,TEXT("Log File cannot be created\n...Exitting!!!"),
					TEXT("!!ERROR!!"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(EXIT_FAILURE);
	}else{
		fprintf(gpFile,"Log File created and opened Successfully!!!\n");
	}

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.style=CS_HREDRAW | CS_VREDRAW |
					 CS_OWNDC;
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.lpszClassName=szClassName;
	wndclassex.lpszMenuName=NULL;
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.hInstance=hInstance;

	RegisterClassEx(&wndclassex);

	hwnd=CreateWindow(szClassName,szClassName,
					WS_OVERLAPPEDWINDOW,
					100,100,
					WIN_WIDTH,WIN_HEIGHT,
					NULL,NULL,
					hInstance,NULL);
	ghwnd=hwnd;
	if(!hwnd){
		MessageBox(NULL,TEXT("CreateWindowEx() failed!!!!"),
					TEXT("ERROR!!!"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(EXIT_FAILURE);
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
	void resize(int,int);
	void ToggleFullscreen(void);
	void uninitialize(void);	


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
				case 0x31:case VK_NUMPAD1:
					digitIsPressed=1;
					display();
					break;
				case 0x32:case VK_NUMPAD2:
					digitIsPressed=2;
					display();
					break;
				case 0x33:case VK_NUMPAD3:
					digitIsPressed=3;
					display();
					break;
				case 0x34:case VK_NUMPAD4:
					digitIsPressed=4;
					display();
					break;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CLOSE:
			uninitialize();
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
		if(dwStyle &  WS_OVERLAPPEDWINDOW){
			mi={sizeof(MONITORINFO)};
			bGetWindowPlacement=GetWindowPlacement(ghwnd,&wpPrev);
			bGetMonitorInfo=GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),
											&mi);
			if(bGetWindowPlacement && bGetMonitorInfo){
				SetWindowLong(ghwnd,GWL_STYLE,
							dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,
							mi.rcMonitor.left,mi.rcMonitor.top,
							mi.rcMonitor.right-mi.rcMonitor.left,
							mi.rcMonitor.bottom-mi.rcMonitor.top,
							SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}else{
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,
					0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE | 
					SWP_NOOWNERZORDER | SWP_NOZORDER |
					SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}	
}

void initialize(void){
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	int LoadGLTextures(GLuint *,TCHAR[]);
	void uninitialize(void);
	void resize(int,int);
	
	ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion=1;
	pfd.dwFlags=PFD_DRAW_TO_WINDOW | 
				PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL ;
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
		"in vec2 vTexture0_Coord;" \
		"out vec2 out_texture0_coord;" \
		"uniform mat4 u_mvp_matrix;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_mvp_matrix * vPosition;" \
		"out_texture0_coord=vTexture0_Coord;" \
		"}";

	glShaderSource(gVertexShaderObject,1,(const GLchar**)
					&vertexShaderSourceCode,NULL);
	glCompileShader(gVertexShaderObject);
	GLint iInfoLogLength=0;
	GLint iShaderCompiledStatus=0;
	char *szInfoLog=NULL;
	glGetShaderiv(gVertexShaderObject,
				GL_COMPILE_STATUS,&iShaderCompiledStatus);
	if(iShaderCompiledStatus==GL_FALSE){
		glGetShaderiv(gVertexShaderObject,
						GL_INFO_LOG_LENGTH,&iInfoLogLength);
		if(iInfoLogLength>0){
			szInfoLog=(char *)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject,iInfoLogLength,
									&written,szInfoLog);
				fprintf(gpFile,"Vertex Shader Compilation Log: %s\n",szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gFragmentShaderObject=glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar *fragmentShaderSourceCode=
		"#version 430" \
		"\n" \
		"in vec2 out_texture0_coord;" \
		"out vec4 FragColor;" \
		"uniform sampler2D u_texture0_sampler;" \
		"void main(void)" \
		"{" \
		"FragColor= texture(u_texture0_sampler,out_texture0_coord);" \
		"}";

	glShaderSource(gFragmentShaderObject,1,(const GLchar**)
					&fragmentShaderSourceCode,NULL);
	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject,GL_COMPILE_STATUS,
					&iShaderCompiledStatus);
	if(iShaderCompiledStatus==GL_FALSE){
		glGetShaderiv(gFragmentShaderObject,GL_INFO_LOG_LENGTH,
					&iInfoLogLength);
		if(iInfoLogLength > 0){
			szInfoLog=(char*)malloc(iInfoLogLength);
			if(szInfoLog!=NULL){
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject,iInfoLogLength,
									&written,szInfoLog);
				fprintf(gpFile,"Fragment Shader Compilation Log: %s \n",
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
	glBindAttribLocation(gShaderProgramObject,SSK_ATTRIBUTE_TEXTURE0,
						"vTexture0_Coord");

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
				glGetProgramInfoLog(gShaderProgramObject,iInfoLogLength,&written,
									szInfoLog);
				fprintf(gpFile,"Shader Program Link Log : %s\n",szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(EXIT_FAILURE);
			}
		}
	}

	gMVPUniform=glGetUniformLocation(gShaderProgramObject,"u_mvp_matrix");
	gTexture_sampler_uniform=glGetUniformLocation(gShaderProgramObject,
												"u_texture0_sampler");

	const GLfloat squareVertices[]=	{	
		1.0f,1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f

	};

	const GLfloat squareTexcoords[]={
		1.0f,1.0f,
		0.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f,
	};

	glGenVertexArrays(1,&gVao_square);
		glBindVertexArray(gVao_square);
			glGenBuffers(1,&gVbo_square_position);
			glBindBuffer(GL_ARRAY_BUFFER,gVbo_square_position);
			glBufferData(GL_ARRAY_BUFFER,sizeof(squareVertices),
						squareVertices,GL_STATIC_DRAW);
			glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX,3,GL_FLOAT,
								GL_FALSE,0,NULL);
			glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
		glBindBuffer(GL_ARRAY_BUFFER,0);

		glGenBuffers(1,&gVbo_square_texture);
		glBindBuffer(GL_ARRAY_BUFFER,gVbo_square_texture);
			glBufferData(GL_ARRAY_BUFFER,sizeof(squareTexcoords),
						squareTexcoords,GL_STATIC_DRAW);
			glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0,2,GL_FLOAT,
								GL_FALSE,0,NULL);
			glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);


	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	LoadGLTextures(&gTexture_Smiley,MAKEINTRESOURCE(IDBITMAP_SMILEY));
	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	gPerspectiveProjectionMatrix=vmath::mat4::identity();
	resize(WIN_WIDTH,WIN_HEIGHT);

}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);
		vmath::mat4 modelViewMatrix;
		vmath::mat4 modelViewProjectionMatrix;

		modelViewMatrix=vmath::mat4::identity();
		modelViewMatrix=vmath::translate(0.0f,0.0f,-6.0f);
		modelViewProjectionMatrix=gPerspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,gTexture_Smiley);
		glUniform1i(gTexture_sampler_uniform,0);

		glBindVertexArray(gVao_square);
			glDrawArrays(GL_TRIANGLE_FAN,0,4);
		glBindVertexArray(0);
	glUseProgram(0);

	SwapBuffers(ghdc);
}

int LoadGLTextures(GLuint *texture,TCHAR imageResourceId[]){
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus=FALSE;

	glGenTextures(1,texture);
	hBitmap=(HBITMAP)LoadImage(GetModuleHandle(NULL),
								imageResourceId,
								IMAGE_BITMAP,0,0,
								LR_CREATEDIBSECTION);
	if(hBitmap){
		iStatus=TRUE;
		GetObject(hBitmap,sizeof(bmp),&bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glBindTexture(GL_TEXTURE_2D,*texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
						GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
						GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,
					bmp.bmWidth,bmp.bmHeight,0,
					GL_BGR,GL_UNSIGNED_BYTE,bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}
	return(iStatus);
}


void uninitialize(void){
	if(gbFullscreen==false){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,
					0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE |
					SWP_NOOWNERZORDER | SWP_NOZORDER |
					SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if(gVao_square){
		glDeleteVertexArrays(1,&gVao_square);
		gVao_square=0;
	}

	if(gTexture_Smiley){
		glDeleteTextures(1,&gTexture_Smiley);
		gTexture_Smiley=0;
	}

	if(gVbo_square_position){
		glDeleteBuffers(1,&gVbo_square_position);
		gVbo_square_position=0;
	}

	if(gVbo_square_texture){
		glDeleteBuffers(1,&gVbo_square_texture);
		gVbo_square_texture=0;
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
	ghwnd=NULL;

	if(gpFile){
		fprintf(gpFile,"Log file is Successfully closed!!\n");
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