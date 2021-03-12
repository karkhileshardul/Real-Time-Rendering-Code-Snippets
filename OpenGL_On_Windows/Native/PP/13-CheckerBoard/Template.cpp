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
#define checkImageWidth		64
#define checkImageHeight	64
#define WIN_WIDTH	800
#define WIN_HEIGHT	600

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

enum{
	SSK_ATTRIBUTE_VERTEX,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXTURE0,
};
bool gbFullscreen=false;
bool gbEscapeKeyIsPressed=false;
bool gbActiveWindow=false;
HWND ghwnd=NULL;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
FILE *gpFile=NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
vmath::mat4 gPerspectiveProjectionMatrix;
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gMVPUniform;
GLuint gVao_square;
GLuint gVbo_square_position;
GLuint gVbo_square_texture;
GLuint gTexture_sampler_uniform;
GLubyte checkImage[checkImageHeight][checkImageHeight][4];
GLuint gTexture_Gen;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szClassName[]=TEXT("12-TweakSmiley Window");
	bool bDone=false;

	void display(void);
	void initialize(void);
	void uninitialize(void);

	if(fopen_s(&gpFile,"SSK_LogFile.txt","w")!=0){
		MessageBox(NULL,TEXT("Log File cannot be created\n...Exiting!!"),
					TEXT("ERROR!!"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(EXIT_FAILURE);
	}else{
		fprintf(gpFile,"Log File created and opened Successfully!!!\n");
	}

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.style=CS_HREDRAW | CS_VREDRAW |CS_OWNDC;
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.lpszClassName=szClassName;
	wndclassex.lpszMenuName=NULL;
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.hInstance=hInstance;
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);

	RegisterClassEx(&wndclassex);

	hwnd=CreateWindow(szClassName,szClassName,
						WS_OVERLAPPEDWINDOW,
						100,100,
						WIN_WIDTH,WIN_HEIGHT,
						NULL,NULL,
						hInstance,NULL);
	ghwnd=hwnd;
	if(!hwnd){
		MessageBox(NULL,TEXT("CreateWindow() failed!!!.."),
					TEXT("!!ERROR!!"),MB_OK |
					MB_ICONSTOP | MB_TOPMOST);
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
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	void resize(int,int);
	void ToggleFullscreen(void);
	void uninitialize(void);	
	void display(void);

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
		-2.0f,-1.0f,0.0f,
		-2.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,-1.0f,0.0f,

		1.0f,-1.0f,0.0f,
		1.0f,1.0f,0.0f,
		2.41421f,1.0f,-1.41421f,
		2.41421f,-1.0f,-1.41421f

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
			glBufferData(GL_ARRAY_BUFFER,sizeof((4*2)*(sizeof(GL_FLOAT))),
						NULL,GL_DYNAMIC_DRAW);
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

	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	gPerspectiveProjectionMatrix=vmath::mat4::identity();
	resize(WIN_WIDTH,WIN_HEIGHT);

}

void display(void){
	void LoadGLtextures(void);
	GLfloat quad_texture[16]; 
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);
		vmath::mat4 modelViewMatrix;
		vmath::mat4 modelViewProjectionMatrix;

		modelViewMatrix=vmath::mat4::identity();
		modelViewMatrix=vmath::translate(0.0f,0.0f,-3.6f);
		modelViewProjectionMatrix=gPerspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);

		glBindVertexArray(gVao_square);
				LoadGLtextures();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,gTexture_Gen);
				glUniform1i(gTexture_sampler_uniform,0);
				glDrawArrays(GL_TRIANGLE_FAN,0,4);
				glDrawArrays(GL_TRIANGLE_FAN,4,4);
				quad_texture[0]=0.0f;
				quad_texture[1]=0.0f;
				quad_texture[2]=0.0f;
				quad_texture[3]=1.0f;
				quad_texture[4]=1.0f;
				quad_texture[5]=1.0f;
				quad_texture[6]=1.0f;
				quad_texture[7]=0.0f;
				quad_texture[8]=0.0f;
				quad_texture[9]=0.0f;
				quad_texture[10]=0.0f;
				quad_texture[11]=1.0f;
				quad_texture[12]=1.0f;
				quad_texture[13]=1.0f;
				quad_texture[14]=1.0f;
				quad_texture[15]=0.0f;
				glBindBuffer(GL_ARRAY_BUFFER,gVbo_square_texture);
					glBufferData(GL_ARRAY_BUFFER,sizeof(quad_texture),
								quad_texture,GL_STATIC_DRAW);
					glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0,2,GL_FLOAT,
										GL_FALSE,0,NULL);
					glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);
				glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(0);
	glUseProgram(0);

	SwapBuffers(ghdc);
}


void LoadGLtextures(void){
	void MakeCheckImage(void);
	MakeCheckImage();

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1,&gTexture_Gen);
	glBindTexture(GL_TEXTURE_2D,gTexture_Gen);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	/*glTexImage2D();
		target,mipmaplevel,internalimageformat(1-4 digit and remaining special symbol),
		width,height,borderwidth(0 border nahhi 1 border ahe),
		pixelformat(amche ranga so 4 dya),typeofdataof9thparameter,(GLUvoid*) imagedata
	*/
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,64,64,0,
				GL_RGBA,GL_UNSIGNED_BYTE,checkImage);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
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

	if(gTexture_Gen){
		glDeleteTextures(1,&gTexture_Gen);
		gTexture_Gen=0;
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


void MakeCheckImage(void){
	int c;

	for(int i=0;i<checkImageHeight;i++){
		for(int j=0;j<checkImageWidth;j++){
			c=(((i&0x8)==0) ^((j&0x8)==0)) * 255;
//c for constant=   ()XOR()*255
			checkImage[i][j][0]=(GLubyte)c;
			checkImage[i][j][1]=(GLubyte)c;
			checkImage[i][j][2]=(GLubyte)c;
			checkImage[i][j][3]=(GLubyte)255;
		//255*255*255*=white
		//0*0*0*255=black
		}
	}
}

