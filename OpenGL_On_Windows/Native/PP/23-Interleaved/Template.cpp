#include<windows.h>
#include<stdio.h>
#include"GL/glew.h"
//#include<GL/glew.h>
#include<gl/GL.h>
#include "vmath.h"
#include"Header.h"
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
GLuint gVao_cube;
GLuint gVbo_cube_position;
GLuint gVbo_cube_texture;
GLuint gModelMatrixUniform,gViewMatrixUniform,gProjectionMatrixUniform;
GLuint gLaUniform,gLdUniform,gLsUniform,gKaUniform,gKdUniform,gKsUniform,gLightPositionUniform;
GLuint material_shininess_uniform;
GLuint gLKeyPressedUniform;

GLfloat lightAmbient[] = { 0.25f,0.25f,0.25f,0.25f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 128.0f;

vmath::mat4 gPerspectiveProjectionMatrix;

GLfloat gAngle=0.0f;
bool gbAnimate;
bool gbLight;

GLuint gTexture_sampler_uniform;
GLuint gTexture_marble;
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
					LPSTR lpszCmdLine,int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	WNDCLASSEX wndclassex;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[]=TEXT("14-Lights Window");
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
	int LoadGLTextures(GLuint *,TCHAR[]);
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


	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode =
		"#version 410" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec2 vTexture0_coord;"\
		"in vec4 vColor;"\
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;"\
		"uniform mat4 u_view_matrix;"\
		"uniform mat4 u_projection_matrix;"\
		"uniform vec4 u_light_position;"\
		"uniform int u_LKeyPressed;" \
		"out vec3 transformed_normals;"\
		"out vec3 light_direction;"\
		"out vec3 viewer_vector;"\
		"out vec2 out_texture0_coord;" \
		"out vec4 out_color;"\
		"void main(void)" \
		"{" \
			"if (u_LKeyPressed == 1)" \
			"{" \
				"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"\
				"transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;"\
				"light_direction = vec3(u_light_position)-eye_coordinates.xyz;"\
				"viewer_vector = -eye_coordinates.xyz;"\
			"}"\
			"out_color = vColor;" \
			"out_texture0_coord = vTexture0_coord;" \
			"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"\
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
	glCompileShader(gVertexShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE){
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0){
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL){
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCode =
		"#version 410" \
		"\n" \
		"in vec2 out_texture0_coord;"\
		"in vec4 out_color;"\
		"in vec3 transformed_normals;"\
		"in vec3 light_direction;"\
		"in vec3 viewer_vector;"\
		"uniform vec3 u_La;"\
		"uniform vec3 u_Ld;"\
		"uniform vec3 u_Ls;"\
		"uniform vec3 u_Ka;"\
		"uniform vec3 u_Kd;"\
		"uniform vec3 u_Ks;"\
		"uniform float u_material_shininess;"\
		"uniform int u_LKeyPressed;" \
		"uniform sampler2D u_texture0_sampler;"\
		"out vec4 FragColor;"\
		"void main(void)" \
		"{" \
			"vec3 phong_ads_color;"\
			"if (u_LKeyPressed == 1)" \
			"{"\
				"vec3 normalized_transformed_normals = normalize(transformed_normals);"\
				"vec3 normalized_light_direction = normalize(light_direction);"\
				"vec3 normalized_viewer_vector = normalize(viewer_vector);"\
				"vec3 ambient = u_La * u_Ka;"\
				"float tn_dot_ld = max(dot(normalized_transformed_normals,normalized_light_direction),0.0);"\
				"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"\
				"vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normals);"\
				"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);"\
				"phong_ads_color = ambient + diffuse + specular;"\
			"}"\
			"else"\
			"{"\
				"phong_ads_color = vec3(1.0,1.0,1.0);"\
			"}"\
			"FragColor = texture(u_texture0_sampler,out_texture0_coord) * out_color * vec4(phong_ads_color,1.0);"\
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE){
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0){
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL){
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gShaderProgramObject = glCreateProgram();

	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");

	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_VERTEX, "vTexture0_coord");
	glBindAttribLocation(gShaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vColor");


	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE){
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength>0){
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL){
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	gLKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");
	gLaUniform= glGetUniformLocation(gShaderProgramObject, "u_La");
	gLdUniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	gLsUniform=glGetUniformLocation(gShaderProgramObject, "u_Ls");
	gKaUniform= glGetUniformLocation(gShaderProgramObject, "u_Ka");
	gKdUniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	gKsUniform= glGetUniformLocation(gShaderProgramObject, "u_Ks");
	gLightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
	gTexture_sampler_uniform=glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");


	const GLfloat cubeVerticesNormalsTextures[] = {
		 1.0f,1.0f,-1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,1.0f,
		 -1.0f,1.0f,-1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,
		 -1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,0.0f,
		 1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,

		 1.0f,-1.0f,1.0f,0.0f,1.0f,0.0f,0.0f,0.0f,-1.0f,1.0f,1.0f,
		 -1.0f,-1.0f,1.0f,0.0f,1.0f,0.0f,0.0f,0.0f,-1.0f,0.0f,1.0f,
		 -1.0f,-1.0f,-1.0f,0.0f,1.0f,0.0f,0.0f,0.0f,-1.0f,0.0f,0.0f,
		 1.0f,-1.0f,-1.0f,0.0f,1.0f,0.0f,0.0f,0.0f,-1.0f,1.0f,0.0f,

		 1.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,
		 -1.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f,
		 -1.0f,-1.0f,1.0,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,
		 1.0f,-1.0f,1.0f,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,0.0f,1.0f,

		 1.0f,1.0f,-1.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,-1.0f,1.0f,0.0f,
		 -1.0f,1.0f,-1.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,-1.0f,1.0f,1.0f,
		 -1.0f,-1.0f,-1.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,-1.0f,0.0f,1.0f,
		 1.0f,-1.0f,-1.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,-1.0f,0.0f,0.0f,

		 -1.0f,1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,0.0f,0.0f,1.0f,
		 -1.0f,1.0f,-1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,0.0f,1.0f,0.0f,
		 -1.0f,-1.0f,-1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,0.0f,1.0f,1.0f,
		 -1.0f,-1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,0.0f,0.0f,0.0f,

		 1.0f,1.0f,-1.0f,0.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,1.0f,0.0f,
		 1.0f,1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,1.0f,1.0f,
		 1.0f,-1.0f,1.0f,0.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,0.0f,1.0f,
		 1.0f,-1.0f,-1.0f,0.0f,1.0f,1.0f,0.0f,-1.0f,0.0f,0.0f,0.0f
	};
	

	glGenVertexArrays(1, &gVao_cube);
	glBindVertexArray(gVao_cube);
		glGenBuffers(1, &gVbo_cube_position);
		glBindBuffer(GL_ARRAY_BUFFER, gVbo_cube_position);
		glBufferData(GL_ARRAY_BUFFER, 24*11* sizeof(float), cubeVerticesNormalsTextures, GL_STATIC_DRAW);
		glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void * )0);
		glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);

		glVertexAttribPointer(SSK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE,11 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(SSK_ATTRIBUTE_COLOR);

		glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE,11 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);

		glVertexAttribPointer(SSK_ATTRIBUTE_TEXTURE0, 3, GL_FLOAT, GL_FALSE,11 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(SSK_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER,0);

	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	LoadGLTextures(&gTexture_marble,MAKEINTRESOURCE(IDBITMAP_MARBLE));
	glEnable(GL_TEXTURE_2D);
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

	if (gVao_cube){
		glDeleteVertexArrays(1, &gVao_cube);
		gVao_cube = 0;
	}

	if (gVbo_cube_position){
		glDeleteBuffers(1, &gVbo_cube_position);
		gVbo_cube_position = 0;
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

void update(void){
	gAngle=gAngle+0.1f;
	if(gAngle>=360.0f){
		gAngle=gAngle-360.0f;
	}
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);
		if(gbLight == true){
			glUniform1i(gLKeyPressedUniform, 1);

			glUniform3fv(gLdUniform, 1,lightDiffuse);
			glUniform3fv(gLaUniform,1,lightAmbient);
			glUniform3fv(gLsUniform,1,lightSpecular);

			glUniform4fv(gLightPositionUniform,1,lightPosition);

			glUniform3fv(gKdUniform, 1,materialDiffuse);
			glUniform3fv(gKaUniform, 1,materialAmbient);
			glUniform3fv(gKsUniform,1,materialSpecular);

			glUniform1i(material_shininess_uniform,materialShininess);
		}else{
			glUniform1i(gLKeyPressedUniform, 0);
		}

		vmath::mat4 modelMatrix = vmath::mat4::identity();
		vmath::mat4 viewMatrix = vmath::mat4::identity();
		vmath::mat4 rotationMatrix = vmath::mat4::identity();
		modelMatrix = vmath::translate(0.0f, 0.0f, -5.0f);
		rotationMatrix = vmath::rotate(gAngle, gAngle, gAngle);
		modelMatrix = modelMatrix * rotationMatrix;

		glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTexture_marble);
		glUniform1i(gTexture_sampler_uniform, 0);

		glBindVertexArray(gVao_cube);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
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
