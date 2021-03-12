#include<windows.h>
#include<stdio.h>
#include<gl\glew.h>
#include<gl/GL.h>
#include "vmath.h"
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

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gVao_pyramid;
GLuint gVbo_pyramid_position;
GLuint gVbo_pyramid_normal;
vmath::mat4 gPerspectiveProjectionMatrix;

GLfloat gAngle=0.0f;
bool gbAnimate;
bool gbLight;
GLuint model_matrix_uniform,view_matrix_uniform,projection_matrix_uniform;
GLuint L_KeyPressed_uniform;


GLuint La_uniform_one;
GLuint Ld_uniform_one;
GLuint Ls_uniform_one;
GLuint light_position_uniform_one;
GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint La_uniform_two;
GLuint Ld_uniform_two;
GLuint Ls_uniform_two;
GLuint light_position_uniform_two;
GLuint material_shininess_uniform;

GLfloat one_lightAmbient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat one_lightDiffuse[]={1.0f,0.0f,0.0f,0.0f};
GLfloat one_lightSpecular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat one_lightPosition[]={100.0f,100.0f,100.0f,1.0f};

GLfloat two_lightAmbient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat two_lightDiffuse[]={0.0f,0.0f,1.0f,0.0f};
GLfloat two_lightSpecular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat two_lightPosition[]={-100.0f,100.0f,100.0f,1.0f};


GLfloat material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat material_diffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_shininess=50.0f;

/*
GLfloat one_lightAmbient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat one_lightDiffuse[]={1.0f,0.0f,0.0f,0.0f};
GLfloat one_lightSpecular[]={1.0f,0.0f,0.0f,0.0f};
GLfloat one_lightPosition[]={2.0f,1.0f,1.0f,0.0f};

GLfloat two_lightAmbient[]={0.0f,0.0f,0.0f,0.0f};
GLfloat two_lightDiffuse[]={0.0f,0.0f,1.0f,0.0f};
GLfloat two_lightSpecular[]={0.0f,0.0f,1.0f,0.0f};
GLfloat two_lightPosition[]={-2.0f,1.0f,1.0f,0.0f};
*/



int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
					LPSTR lpszCmdLine,int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	WNDCLASSEX wndclassex;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[]=TEXT("18-Pyramid_2Lights Window");
	bool bDone=false;

	if(fopen_s(&gpFile,"SSK_Log.txt","w")!=0){
		MessageBox(NULL,TEXT("Log File can not be created\n Exitting..."),
				TEXT("!!!ERROR!!!"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(EXIT_FAILURE);
	}else{
		fprintf(gpFile,"Log File is Successfully Opened..\n");
	}

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.style=CS_HREDRAW |CS_VREDRAW |CS_OWNDC;
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.lpszClassName=szClassName;
	wndclassex.lpszMenuName=NULL;
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.hInstance=hInstance;
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);

	RegisterClassEx(&wndclassex);

	hwnd=CreateWindow(szClassName,szClassName,
						WS_OVERLAPPEDWINDOW,100,100,
						WIN_WIDTH,WIN_HEIGHT,
						NULL,NULL,
						hInstance,NULL);
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
			display();
			if(gbAnimate==true){
				update();
			}
			if(gbActiveWindow==true){
				if(gbEscapeKeyIsPressed==true){
					bDone=true;
				}
			}
		}
	}
	uninitialize();
	return ((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	void resize(int,int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	static bool bIsAKeyPressed=false;
	static bool bIsLKeyPressed=false;

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
				case 0x41://a
					if(bIsAKeyPressed==false){
						gbAnimate=true;
						bIsAKeyPressed=true;
					}else{
						gbAnimate=false;
						bIsAKeyPressed=false;
					}
				case 0x4C:
					if(bIsLKeyPressed==false){
						bIsLKeyPressed=true;
						gbLight=true;
					}else{
						bIsLKeyPressed=false;
						gbLight=false;
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
	return (DefWindowProc(hwnd,iMsg,wParam,lParam));
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
			bGetMonitorInfo=GetMonitorInfo(MonitorFromWindow(ghwnd,
											MONITORINFOF_PRIMARY),&mi);
			if(bGetWindowPlacement && bGetMonitorInfo){
				SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,
							mi.rcMonitor.left,mi.rcMonitor.top,
							mi.rcMonitor.right-mi.rcMonitor.left,
							mi.rcMonitor.bottom-mi.rcMonitor.top,
							SWP_NOZORDER |SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}else{
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,
					0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE |
					SWP_NOZORDER |SWP_NOOWNERZORDER|
					SWP_FRAMECHANGED);
		ShowCursor(TRUE);
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

void initialize(void){
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

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
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light_position_one;" \
		"uniform vec4 u_light_position_two;" \
		"uniform int u_lighting_enabled;" \
		"out vec3 transformed_normals;" \
		"out vec3 light_direction_one;" \
		"out vec3 light_direction_two;" \
		"out vec3 viewer_vector;" \
		"void main(void)" \
		"{" \
		"if(u_lighting_enabled==1)" \
		"{" \
		"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
		"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"light_direction_one = vec3(u_light_position_one) - eye_coordinates.xyz;" \
		"light_direction_two = vec3(u_light_position_two) - eye_coordinates.xyz;" \
		"viewer_vector = -eye_coordinates.xyz;" \
		"}" \
		"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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
		"in vec3 transformed_normals;" \
		"in vec3 light_direction_one;" \
		"in vec3 light_direction_two;" \
		"in vec3 viewer_vector;" \
		"out vec4 FragColor;" \
		"uniform vec3 u_La_one;" \
		"uniform vec3 u_Ld_one;" \
		"uniform vec3 u_Ls_one;" \
		"uniform vec3 u_La_two;" \
		"uniform vec3 u_Ld_two;" \
		"uniform vec3 u_Ls_two;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"uniform int u_lighting_enabled;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_color_one;" \
		"vec3 phong_ads_color_two;" \
		"if(u_lighting_enabled==1)" \
		"{" \
		"vec3 normalized_transformed_normals=normalize(transformed_normals);" \
		"vec3 normalized_light_direction_one=normalize(light_direction_one);" \
		"vec3 normalized_viewer_vector=normalize(viewer_vector);" \
		"vec3 ambient1 = u_La_one * u_Ka;" \
		"float tn_dot_ld1 = max(dot(normalized_transformed_normals, normalized_light_direction_one),0.0);" \
		"vec3 diffuse_one = u_Ld_one * u_Kd * tn_dot_ld1;" \
		"vec3 reflection_vector1 = reflect(-normalized_light_direction_one, normalized_transformed_normals);" \
		"vec3 specular_one = u_Ls_one * u_Ks * pow(max(dot(reflection_vector1, normalized_viewer_vector), 0.0), u_material_shininess);" \
		"phong_ads_color_one=ambient1 + diffuse_one + specular_one;" \
		"vec3 normalized_light_direction_two=normalize(light_direction_two);" \
		"vec3 ambient_two = u_La_two * u_Ka;" \
		"float tn_dot_ld2 = max(dot(normalized_transformed_normals, normalized_light_direction_two),0.0);" \
		"vec3 diffuse_two = u_Ld_two * u_Kd * tn_dot_ld2;" \
		"vec3 reflection_vector2 = reflect(-normalized_light_direction_two, normalized_transformed_normals);" \
		"vec3 specular_two = u_Ls_two * u_Ks * pow(max(dot(reflection_vector2, normalized_viewer_vector), 0.0), u_material_shininess);" \
		"phong_ads_color_two=ambient_two + diffuse_two + specular_two;" \
		"phong_ads_color_one = phong_ads_color_one + phong_ads_color_two;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_color_one = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"FragColor = vec4(phong_ads_color_one, 1.0);" \
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
					glGetProgramInfoLog(gShaderProgramObject,iInfoLogLength,&written,
										szInfoLog);
					fprintf(gpFile,"Shader Program ink Log : %s\n",szInfoLog);
					free(szInfoLog);
					uninitialize();
					exit(EXIT_FAILURE);
				}
			}
		}

		model_matrix_uniform=glGetUniformLocation(gShaderProgramObject,
												"u_model_matrix");
		view_matrix_uniform=glGetUniformLocation(gShaderProgramObject,
												"u_view_matrix");
		projection_matrix_uniform=glGetUniformLocation(gShaderProgramObject,
													"u_projection_matrix");

		L_KeyPressed_uniform=glGetUniformLocation(gShaderProgramObject,"u_lighting_enabled");
		La_uniform_one=glGetUniformLocation(gShaderProgramObject,"u_La_one");
		Ld_uniform_one=glGetUniformLocation(gShaderProgramObject,"u_Ld_one");
		Ls_uniform_one=glGetUniformLocation(gShaderProgramObject,"u_Ls_one");
		light_position_uniform_one=glGetUniformLocation(gShaderProgramObject,"u_light_position_one");

		Ka_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
		Kd_uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
		Ks_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");

		La_uniform_two=glGetUniformLocation(gShaderProgramObject,"u_La_two");
		Ld_uniform_two=glGetUniformLocation(gShaderProgramObject,"u_Ld_two");
		Ls_uniform_two=glGetUniformLocation(gShaderProgramObject,"u_Ls_two");

		light_position_uniform_two = glGetUniformLocation(gShaderProgramObject, "u_light_position_two");

		material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

		const GLfloat pyramidVertices[]={
			/*frontside */
			0.0f,1.0f,0.0f,// front-top
			-1.0f,-1.0f,1.0f, // front-left
			1.0f,-1.0f,1.0f,// front-right

			/*rightside*/
			0.0f,1.0f,0.0f,// right-top
			1.0f,-1.0f,1.0f,// right-left
			1.0f,-1.0f,-1.0f,// right-right

			/*backside*/
			0.0f,1.0f,0.0f,// back-top
			1.0f,-1.0f,-1.0f,// back-left
			-1.0f,-1.0f,-1.0f,// back-right

			/*leftside*/
			0.0f,1.0f,0.0f,// left-top
			-1.0f,-1.0f,-1.0f,// left-left
			-1.0f,-1.0f,1.0f// left-right
		};

	
		const GLfloat pyramidNormals[]={
		//0.0f,0.0f,1.0f,
		
		//0.0f, 1.0f, 0.0f,
		
        //-1.0f, 0.0f, 0.0f,
        
        //0.0f, -1.0f, 0.0f,
		//front face
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		//right face
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	
		//back face
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		//left face
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

/*
		0.0f,0.447214f,0.894427f,
		0.0f,0.447214f,-0.894427f,
		0.894427f,0.447214f,0.0f,
		-0.894427f,0.447214f,0.0f,

*/
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

		glGenBuffers(1, &gVbo_pyramid_normal);
		glBindBuffer(GL_ARRAY_BUFFER, gVbo_pyramid_normal);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals),
							pyramidNormals, GL_STATIC_DRAW);
			glVertexAttribPointer(SSK_ATTRIBUTE_COLOR,
								3,GL_FLOAT,GL_FALSE,0,NULL);
			glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	gPerspectiveProjectionMatrix=vmath::mat4::identity();
	gbAnimate = false;
	gbLight = false;
	resize(WIN_WIDTH,WIN_HEIGHT);
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

	if (gVao_pyramid){
		glDeleteVertexArrays(1, &gVao_pyramid);
		gVao_pyramid= 0;
	}

	if (gVbo_pyramid_position){
		glDeleteBuffers(1, &gVbo_pyramid_position);
		gVbo_pyramid_position = 0;
	}

	if (gVbo_pyramid_normal){
		glDeleteBuffers(1, &gVbo_pyramid_normal);
		gVbo_pyramid_normal = 0;
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

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

	vmath::mat4 modelMatrix;
	vmath::mat4 rotationMatrix;
	vmath::mat4 viewMatrix;

		glUseProgram(gShaderProgramObject);

		if(gbLight==true){
				//set 'u_lighting_enabled' uniform
			glUniform1i(L_KeyPressed_uniform, 1);
				//setting light's properties
			glUniform3fv(La_uniform_one, 1, one_lightAmbient);
			glUniform3fv(Ld_uniform_one, 1, one_lightDiffuse);
			glUniform3fv(Ls_uniform_one, 1, one_lightSpecular);
			glUniform4fv(light_position_uniform_one, 1, one_lightPosition);
				// setting material's properties

			glUniform3fv(La_uniform_two, 1, two_lightAmbient);
			glUniform3fv(Ld_uniform_two, 1, two_lightDiffuse);
			glUniform3fv(Ls_uniform_two, 1, two_lightSpecular);
			glUniform4fv(light_position_uniform_two, 1, two_lightPosition);

			glUniform3fv(Ka_uniform, 1, material_ambient);
			glUniform3fv(Kd_uniform, 1, material_diffuse);
			glUniform3fv(Ks_uniform, 1, material_specular);
			glUniform1f(material_shininess_uniform, material_shininess);

		}else{
			glUniform1i(L_KeyPressed_uniform, 0);
		}
		
			modelMatrix=vmath::mat4::identity();
			viewMatrix=vmath::mat4::identity();
			modelMatrix=vmath::translate(0.0f,0.0f,-6.0f);
			rotationMatrix=vmath::rotate(gAngle,0.0f,1.0f,0.0f);
			modelMatrix = modelMatrix * rotationMatrix;
			glUniformMatrix4fv(model_matrix_uniform,1,GL_FALSE,modelMatrix);
			glUniformMatrix4fv(view_matrix_uniform,1,GL_FALSE,viewMatrix);
			glUniformMatrix4fv(projection_matrix_uniform,1,
								GL_FALSE,gPerspectiveProjectionMatrix);

			glBindVertexArray(gVao_pyramid);
				glDrawArrays(GL_TRIANGLES,0,12);
			glBindVertexArray(0);
			
	glUseProgram(0);
	SwapBuffers(ghdc);
}

void update(void){
	gAngle=gAngle+0.1f;
	if(gAngle>=360.0f){
		gAngle=gAngle-360.0f;
	}
}
