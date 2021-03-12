#include<windows.h>
#include<stdio.h>
#include<gl\glew.h>
#include<gl/GL.h>
#include"Header.h"
#include"vmath.h"
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#define WIN_WIDTH	800
#define WIN_HEIGHT	600

enum{
	SSK_ATTRIBUTE_VERTEX,
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
bool gbEscapeKeyIsPressed=false;
bool gbActiveWindow=false;
bool gbFullscreen=false;
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gMVPUniform;
vmath::mat4 gPerspectiveProjectionMatrix;
GLuint gVao_pyramid;
GLuint gVbo_pyramid_position;
GLuint gVbo_pyramid_texture;
GLuint gVao_cube;
GLuint gVbo_cube_position;
GLuint gVbo_cube_texture;
GLfloat pyramid_angle=0.0f;
GLfloat cube_angle=0.0f;
GLuint gTexture_sampler_uniform;
GLuint gTexture_Kundali;
GLuint gTexture_Stone;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	WNDCLASSEX wndclassex;
	MSG msg;
	HWND hwnd;
	TCHAR szClassName[]=TEXT("10-Stone and Kundali Window");
	bool bDone=false;

	void display(void);
	void update(void);
	void initialize(void);
	void uninitialize(void);

	if(fopen_s(&gpFile,"SSK_Log.txt","w")!=0){
		MessageBox(NULL,TEXT("Log File cannot be created\nExiting Now...."),
					TEXT("!!ERROR!!"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(EXIT_FAILURE);
	}else{
		fprintf(gpFile,"LofFile Created Successfully opened\n");
	}

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.lpszClassName=szClassName;
	wndclassex.lpszMenuName=NULL;
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.style=CS_HREDRAW | CS_VREDRAW |CS_OWNDC;
	wndclassex.lpfnWndProc=WndProc;
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
		MessageBox(NULL,TEXT("CreateWindowEx()!!! failed!!!"),
					TEXT("ERROR!!!"),MB_OK |MB_ICONSTOP | MB_TOPMOST);
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
		update();
		}
	}

	uninitialize();
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	void ToggleFullscreen(void);
	void resize(int,int);
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
				SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,
							mi.rcMonitor.left,mi.rcMonitor.top,
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

		void resize(int,int);
		void uninitialize(void);
		int LoadGLTextures(GLuint *,TCHAR[]);

		ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));

		pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion=1;
		pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER |
					PFD_SUPPORT_OPENGL;
		pfd.iPixelType=PFD_TYPE_RGBA;
		pfd.cColorBits=32;
		pfd.cRedBits=8;
		pfd.cBlueBits=8;
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
					fprintf(gpFile,"Shader Program ink Log : %s\n",szInfoLog);
					free(szInfoLog);
					uninitialize();
					exit(EXIT_FAILURE);
				}
			}
		}

		gMVPUniform=glGetUniformLocation(gShaderProgramObject,"u_mvp_matrix");
		gTexture_sampler_uniform=glGetUniformLocation(gShaderProgramObject,
														"u_texture0_sampler");

		const GLfloat pyramidVertices[]={
			/*frontside */
			0,1,0,
			-1,-1,1,
			1,-1,1,

			/*rightside*/
			0,1,0,
			1,-1,1,
			1,-1,-1,

			/*backside*/
			0,1,0,
			1,-1,-1,
			-1,-1,-1,

			/*leftside*/
			0,1,0,
			-1,-1,-1,
			-1,-1,1
		};

		const GLfloat pyramidTexcoords[]={
			/*frontside*/
			0.5,1.0,
			0.0,0.0,
			1.0,0.0,

			/*rightside*/
			0.5,1.0,
			1.0,0.0,
			0.0,0.0,

			/*backside*/
			0.5,1.0,
			1.0,0.0,
			0.0,0.0,

			/*leftside*/
			0.5,1.0,
			0.0,0.0,
			1.0,0.0,
		};

		GLfloat cubeVertices[]={
			/*topsurface*/
			1.0f,1.0f,-1.0f,/*topright*/
			-1.0f,1.0f,-1.0f,/*topleft*/
			-1.0f,1.0f,1.0f,/*bottomleft*/
			1.0f,1.0f,1.0f,/*bottomright*/

			/*bottomsurface*/
			1.0f,-1.0f,1.0f,/*topright*/
			-1.0f,-1.0f,1.0f,/*topleft*/
			-1.0f,-1.0f,-1.0f,/*bottomleft*/
			1.0f,-1.0f,-1.0f,/*bottomright*/

			/*frontsurface*/
			1.0f,1.0f,1.0f,/*topright*/
			-1.0f,1.0f,1.0f,/*topleft*/
			-1.0f,-1.0f,1.0f,/*bottomleft*/
			1.0f,-1.0f,1.0f,/*bottomright*/

			/*backsurface*/
			1.0f,-1.0f,-1.0f,/*topright*/
			-1.0f,-1.0f,-1.0f,/*topleft*/
			-1.0f,1.0f,-1.0f,/*bottomleft*/
			1.0f,1.0f,-1.0f,/*bottomright*/

			/*leftsurface*/
			-1.0f,1.0f,1.0f,/*topright*/
			-1.0f,1.0f,-1.0f,/*topleft*/
			-1.0f,-1.0f,-1.0f,/*bottomleft*/
			-1.0f,-1.0f,1.0f,/*bottomright*/

			/*rightsurface*/
			1.0f,1.0f,-1.0f,/*topright*/
			1.0f,1.0f,1.0f,/*topleft*/
			1.0f,-1.0f,1.0f,/*bottomleft*/
			1.0f,-1.0f,-1.0f,/*bottomright*/
		};
		

		for(int i=0;i<(24*3);i++){
			if(cubeVertices[i]<0.0f){
				cubeVertices[i]=cubeVertices[i]+0.25f;
			}else if(cubeVertices[i]>0.0f){
				cubeVertices[i]=cubeVertices[i]-0.25f;
			}else{
				cubeVertices[i]=cubeVertices[i];
			}
		}


		const GLfloat cubeTexcoords[]={
			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,

			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,

			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,

			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,

			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
		
			0.0f,0.0f,
			1.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
		};

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

			glGenBuffers(1,&gVbo_pyramid_texture);
			glBindBuffer(GL_ARRAY_BUFFER,gVbo_pyramid_texture);
				glBufferData(GL_ARRAY_BUFFER,sizeof(pyramidTexcoords),
							pyramidTexcoords,GL_STATIC_DRAW);
				glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0,2,GL_FLOAT,
									GL_FALSE,0,NULL);
				glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);
			glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(0);

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

			glGenBuffers(1,&gVbo_cube_texture);
			glBindBuffer(GL_ARRAY_BUFFER,gVbo_cube_texture);
				glBufferData(GL_ARRAY_BUFFER,sizeof(cubeTexcoords),
							cubeTexcoords,GL_STATIC_DRAW);
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
		glEnable(GL_CULL_FACE);

		LoadGLTextures(&gTexture_Kundali,MAKEINTRESOURCE(IDBITMAP_KUNDALI));
		LoadGLTextures(&gTexture_Stone,MAKEINTRESOURCE(IDBITMAP_STONE));
		glEnable(GL_TEXTURE_2D);

		glClearColor(0.0f,0.0f,0.0f,0.0f);
		gPerspectiveProjectionMatrix=vmath::mat4::identity();
		resize(WIN_WIDTH,WIN_HEIGHT);
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
			vmath::mat4 modelViewMatrix;
			vmath::mat4 rotationMatrix;
			vmath::mat4 modelViewProjectionMatrix;

			modelViewMatrix=vmath::mat4::identity();
			modelViewMatrix=vmath::translate(-1.5f,0.0f,-6.0f);
			rotationMatrix=vmath::rotate(pyramid_angle,0.0f,1.0f,0.0f);
			modelViewMatrix=modelViewMatrix * rotationMatrix;
			modelViewProjectionMatrix=gPerspectiveProjectionMatrix * modelViewMatrix;
			glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,gTexture_Stone);
			glUniform1i(gTexture_sampler_uniform,0);

			glBindVertexArray(gVao_pyramid);
				glDrawArrays(GL_TRIANGLES,0,12);
			glBindVertexArray(0);

			modelViewMatrix=vmath::mat4::identity();
			modelViewMatrix=vmath::translate(1.5f,0.0f,-6.0f);
			rotationMatrix=vmath::rotate(cube_angle,cube_angle,cube_angle);
			modelViewMatrix=modelViewMatrix * rotationMatrix;
			modelViewProjectionMatrix=gPerspectiveProjectionMatrix * modelViewMatrix;

			glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelViewProjectionMatrix);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,gTexture_Kundali);
			glUniform1i(gTexture_sampler_uniform,0);

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

int LoadGLTextures(GLuint *texture,TCHAR imageResourceId[]){
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus=FALSE;

	glGenTextures(1,texture);
	hBitmap=(HBITMAP)LoadImage(GetModuleHandle(NULL),
								imageResourceId,IMAGE_BITMAP,
								0,0,LR_CREATEDIBSECTION);
	if(hBitmap){
		iStatus=TRUE;
		GetObject(hBitmap,sizeof(bmp),&bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glBindTexture(GL_TEXTURE_2D,*texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
						GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
						GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,
					GL_RGB,bmp.bmWidth,bmp.bmHeight,
					0,GL_BGR,GL_UNSIGNED_BYTE,bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}
	return(iStatus);
}

void uninitialize(void){
	if(gbFullscreen==true){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,
					0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE |
					SWP_NOZORDER | SWP_NOOWNERZORDER |
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

	if(gVbo_pyramid_texture){
		glDeleteBuffers(1,&gVbo_pyramid_texture);
		gVbo_pyramid_texture=0;
	}

	if(gTexture_Stone){
		glDeleteTextures(1,&gTexture_Stone);
		gTexture_Stone=0;
	}

	if(gVao_cube){
		glDeleteVertexArrays(1,&gVao_cube);
		gVao_cube=0;
	}

	if(gVbo_cube_position){
		glDeleteBuffers(1,&gVbo_cube_position);
		gVbo_cube_position=0;
	}

	if(gVbo_cube_texture){
		glDeleteBuffers(1,&gVbo_cube_texture);
		gVbo_cube_texture=0;
	}

	if(gTexture_Kundali){
		glDeleteTextures(1,&gTexture_Kundali);
		gTexture_Kundali=0;
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

	ReleaseDC(ghwnd	,ghdc);
	ghdc=NULL;

	if(gpFile){
		fprintf(gpFile,"Log File Successfully Closed!!!!\n");
		fclose(gpFile);
		gpFile=NULL;
	}
}

void update(void){
	cube_angle=cube_angle+0.1f;
	if(cube_angle>=360.0f){
		cube_angle=cube_angle-360.0f;
	}
	pyramid_angle=pyramid_angle+0.1f;
	if(pyramid_angle>=360.0f){
		pyramid_angle=pyramid_angle-360.0f;
	}
}