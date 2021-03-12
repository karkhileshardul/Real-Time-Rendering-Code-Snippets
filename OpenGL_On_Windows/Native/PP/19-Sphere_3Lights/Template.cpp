#include<windows.h>
#include<stdio.h>
#include<gl\glew.h>
#include<gl\GL.h>
#include"vmath.h"
#include"Sphere.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"Sphere.lib")
#define WIN_WIDTH 	800
#define WIN_HEIGHT	600
enum{
	SSK_ATTRIBUTE_VERTEX=0,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXTURE0,
};

FILE *gpFile=NULL;
HWND ghwnd=NULL;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
bool gbActiveWindow=false;
bool gbFullscreen=false;
bool gbEscapeKeyIsPressed=false;

GLfloat gAngle=0.0f;
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
vmath::mat4 gPerspectiveProjectionMatrix;
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

bool gbLightPerVertex=false;
bool gbLightPerFragment=false;

/* Sphere Loading related declaration (___START___)*/
GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

float sphere_normals[1146];
float sphere_vertices[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint gNumElements;
GLuint gNumVertices;
/* Sphere Loading related declaration (___END___)*/

/* Unifrom declarations (___START___) */
GLuint gMVPUniform;

GLuint model_matrix_uniform,gview_matrix_uniform,projection_matrix_uniform,view_matrix_uniform;
GLuint gL_KeyPressed_uniform;
GLuint gLdUniform,gKdUniform,glightPosition_RedUniform;

GLuint F_KeyPressed_uniform;
GLuint V_KeyPressed_uniform;

GLuint La_uniform_red;
GLuint Ld_uniform_red;
GLuint Ls_uniform_red;
GLuint light_position_uniform_red;

GLuint La_uniform_green;
GLuint Ld_uniform_green;
GLuint Ls_uniform_green;
GLuint light_position_uniform_green;

GLuint La_uniform_blue;
GLuint Ld_uniform_blue;
GLuint Ls_uniform_blue;
GLuint light_position_uniform_blue;

GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;

/* Unifrom declarations (___END___) */


/*Lights Properties (___START___) */
GLfloat lightAmbient_red[]={0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse_red[]={1.0f,0.0f,0.0f,1.0f};
GLfloat lightSpecular_red[]={1.0f,1.0f,1.0f,1.0f};
GLfloat lightPosition_red[]={-2.0f,1.0f,1.0f,1.0f};

GLfloat lightAmbient_green[]={0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse_green[]={0.0f,1.0f,0.0f,1.0f};
GLfloat lightSpecular_green[]={1.0f,1.0f,1.0f,1.0f};
GLfloat lightPosition_green[]={2.0f,1.0f,1.0f,1.0f};

GLfloat lightAmbient_blue[]={0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse_blue[]={0.0f,0.0f,1.0f,1.0f};
GLfloat lightSpecular_blue[]={1.0f,1.0f,1.0f,1.0f};
GLfloat lightPosition_blue[]={0.0f,0.0f,1.0f,1.0f};
/*Lights Properties (___END___) */

GLfloat material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat material_diffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_shininess=50.0f;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	WNDCLASSEX wndclassex;
	HWND hwnd;
	TCHAR szClassName[]=TEXT("19-Sphere 3 Lights Window");
	MSG msg;
	bool bDone=false;

	if(fopen_s(&gpFile,"SSK_Log.txt","w")!=0){
		MessageBox(NULL,TEXT("Log File cannot be created\n Exitting..."),TEXT("!!!ERROR!!"),
					MB_OK| MB_TOPMOST | MB_ICONSTOP);
		exit(EXIT_FAILURE);
	}else{
		fprintf(gpFile,"Log File is Sucessfully opened\n");
	}

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.style=CS_HREDRAW |CS_VREDRAW |CS_OWNDC;
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.hInstance=hInstance;
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.lpszClassName=szClassName;
	wndclassex.lpszMenuName=NULL;
	wndclassex.lpfnWndProc=WndProc;
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
				bDone=false;
			}else{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}else{
			update();
			display();
			if(gbActiveWindow==true){
				if(gbEscapeKeyIsPressed==true){
					bDone=true;
				}
			}
		}
	}
	uninitialize();
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	void uninitialize(void);
	void resize(int,int);
	void ToggleFullscreen(void);

	static bool bIsVKeyPressed=false;
	static bool bIsFKeyPressed=false;
	

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
					if(gbFullscreen==false){
						ToggleFullscreen();
						gbFullscreen=true;
					}else{
						ToggleFullscreen();
						gbFullscreen=false;
					}
					break;
				case 0x46:
					if(bIsFKeyPressed==false){
						gbLightPerVertex = false;
						gbLightPerFragment = true;
						bIsFKeyPressed = true;
					}else{
						gbLightPerVertex = false;
						gbLightPerFragment = false;
						bIsFKeyPressed = false;
					}
					break;
				case 0x51: //For q key
				 		if(gbEscapeKeyIsPressed==false){
						gbEscapeKeyIsPressed=true;
					}
					break;
				case 0x56: // For v key
					if (bIsVKeyPressed == false){
						gbLightPerVertex = true;
						gbLightPerFragment = false;
						bIsVKeyPressed = true;
					}else{
						gbLightPerVertex = false;
						gbLightPerFragment = false;
						bIsVKeyPressed = false;
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
		if(dwStyle&WS_OVERLAPPEDWINDOW){
			mi={sizeof(MONITORINFO)};
			bGetWindowPlacement=GetWindowPlacement(ghwnd,&wpPrev);
			bGetMonitorInfo=GetMonitorInfo(
							MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),
							&mi);
			if(bGetWindowPlacement && bGetMonitorInfo){
				SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,
							mi.rcMonitor.top,
							mi.rcMonitor.right - mi.rcMonitor.left,
							mi.rcMonitor.bottom - mi.rcMonitor.top,
							SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}else{
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE |
					SWP_NOZORDER | SWP_NOOWNERZORDER |
					SWP_FRAMECHANGED);
		ShowCursor(TRUE);
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
		"uniform int u_lighting_enabled;" \
		"uniform int u_per_vertex_lighting_enabled;" \
		"uniform int u_per_fragment_lighting_enabled;" \
		"uniform vec3 u_La_red;" \
		"uniform vec3 u_Ld_red;" \
		"uniform vec3 u_Ls_red;" \
		"uniform vec4 u_light_position_red;" \
		"uniform vec3 u_La_green;" \
		"uniform vec3 u_Ld_green;" \
		"uniform vec3 u_Ls_green;" \
		"uniform vec4 u_light_position_green;" \
		"uniform vec3 u_La_blue;" \
		"uniform vec3 u_Ld_blue;" \
		"uniform vec3 u_Ls_blue;" \
		"uniform vec4 u_light_position_blue;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"vec3 phong_ads_color;" \
		"out vec3 out_phong_ads_color;" \
		"out vec3 transformed_normals;" \
		"out vec3 light_position_red;" \
		"out vec3 light_position_green;" \
		"out vec3 light_position_blue;" \
		"out vec3 viewer_vector;" \
		"vec3 SettingLightProperties(vec3 La, vec3 Ld, vec3 Ls, vec4 light_position)" \
		"{" \
			"vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;" \
			"vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
			"vec3 light_direction = normalize(vec3(light_position) - eyeCoordinates.xyz);" \
			"float tn_dot_ld = max(dot(transformed_normals, light_direction), 0.0);" \
			"vec3 ambient = La * u_Kd;" \
			"vec3 diffuse = Ld * u_Kd * tn_dot_ld;" \
			"vec3 reflection_vector = reflect(-light_direction, transformed_normals);" \
			"vec3 viewer_vector = normalize(-eyeCoordinates.xyz);" \
			"vec3 specular = Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" \
			"phong_ads_color = ambient + diffuse + specular;" \
			"return phong_ads_color;" \
		"}" \
		"void main(void)" \
		"{" \
			"if(u_per_vertex_lighting_enabled==1)" \
			"{" \
				"vec3 red_light=SettingLightProperties(u_La_red,u_Ld_red,u_Ls_red,u_light_position_red);" \
				"vec3 green_light=SettingLightProperties(u_La_green,u_Ld_green,u_Ls_green,u_light_position_green);" \
				"vec3 blue_light=SettingLightProperties(u_La_blue,u_Ld_blue,u_Ls_blue,u_light_position_blue);" \
				"out_phong_ads_color=red_light+green_light+blue_light;" \
			"}" \
			"if(u_per_fragment_lighting_enabled==1)" \
			"{" \
				"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
				"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
				"light_position_red = vec3(u_light_position_red) - eye_coordinates.xyz;" \
				"light_position_green = vec3(u_light_position_green) - eye_coordinates.xyz;" \
				"light_position_blue = vec3(u_light_position_blue) - eye_coordinates.xyz;" \
				"viewer_vector = -eye_coordinates.xyz;" \
			"}" \
			"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";

	glShaderSource(gVertexShaderObject,1,
					(const GLchar**)&vertexShaderSourceCode,NULL);
	glCompileShader(gVertexShaderObject);
	GLint iInfoLogLength=0;
	GLint iShaderCompiledStatus=0;
	char *szInfoLog=NULL;
	glGetShaderiv(gVertexShaderObject,GL_COMPILE_STATUS,
					&iShaderCompiledStatus);
	if(iShaderCompiledStatus==GL_FALSE){
		glGetShaderiv(gVertexShaderObject,GL_INFO_LOG_LENGTH,
					&iInfoLogLength);
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
		"#version 430" \
		"\n" \
		"in vec3 viewer_vector;" \
		"in vec3 transformed_normals;" \
		"in vec3 out_phong_ads_color;" \
		"in vec3 light_position_red;" \
		"in vec3 light_position_green;" \
		"in vec3 light_position_blue;" \
		"out vec4 FragColor;" \
		"uniform vec3 u_La_red;" \
		"uniform vec3 u_Ld_red;" \
		"uniform vec3 u_Ls_red;" \
		"uniform vec3 u_La_green;" \
		"uniform vec3 u_Ld_green;" \
		"uniform vec3 u_Ls_green;" \
		"uniform vec3 u_La_blue;" \
		"uniform vec3 u_Ld_blue;" \
		"uniform vec3 u_Ls_blue;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"uniform int u_lighting_enabled;" \
		"uniform int u_per_vertex_lighting_enabled;" \
		"uniform int u_per_fragment_lighting_enabled;" \
		"vec3 SettingLightProperties(vec3 u_La, vec3 u_Ld, vec3 u_Ls, vec3 light_direction)" \
		"{" \
			"vec3 phong_ads_color;" \
			"vec3 normalized_transformed_normals=normalize(transformed_normals);" \
			"vec3 normalized_light_direction=normalize(light_direction);" \
			"vec3 normalized_viewer_vector=normalize(viewer_vector);" \
			"vec3 ambient = u_La * u_Ka;" \
			"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);" \
			"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
			"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
			"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" \
			"phong_ads_color=ambient + diffuse + specular;" \
			"return phong_ads_color;" \
		"}" \
		"void main(void)" \
		"{" \
			"if(u_per_vertex_lighting_enabled==1)" \
			"{" \
				"FragColor = vec4(out_phong_ads_color, 1.0);" \
			"}" \
			"if(u_per_fragment_lighting_enabled==1)" \
			"{" \
				"vec3 red_light=SettingLightProperties(u_La_red,u_Ld_red,u_Ls_red,light_position_red);" \
				"vec3 green_light=SettingLightProperties(u_La_green,u_Ld_green,u_Ls_green,light_position_green);" \
				"vec3 blue_light=SettingLightProperties(u_La_blue,u_Ld_blue,u_Ls_blue,light_position_blue);" \
				"vec3 phong_ads_color=red_light+green_light+blue_light;" \
				"FragColor = vec4(phong_ads_color, 1.0);" \
			"}" \
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

	model_matrix_uniform=glGetUniformLocation(gShaderProgramObject,
													"u_model_matrix");
	view_matrix_uniform=glGetUniformLocation(gShaderProgramObject,
													"u_view_matrix");
	projection_matrix_uniform=glGetUniformLocation(gShaderProgramObject,
															"u_projection_matrix");

	V_KeyPressed_uniform=glGetUniformLocation(gShaderProgramObject,"u_per_vertex_lighting_enabled");
	F_KeyPressed_uniform=glGetUniformLocation(gShaderProgramObject,"u_per_fragment_lighting_enabled");

	La_uniform_red=glGetUniformLocation(gShaderProgramObject,"u_La_red");
	Ld_uniform_red=glGetUniformLocation(gShaderProgramObject,"u_Ld_red");
	Ls_uniform_red=glGetUniformLocation(gShaderProgramObject,"u_Ls_red");
	light_position_uniform_red=glGetUniformLocation(gShaderProgramObject,"u_light_position_red");

	La_uniform_green=glGetUniformLocation(gShaderProgramObject,"u_La_green");
	Ld_uniform_green=glGetUniformLocation(gShaderProgramObject,"u_Ld_green");
	Ls_uniform_green=glGetUniformLocation(gShaderProgramObject,"u_Ls_green");
	light_position_uniform_green=glGetUniformLocation(gShaderProgramObject,"u_light_position_green");

	La_uniform_blue=glGetUniformLocation(gShaderProgramObject,"u_La_blue");
	Ld_uniform_blue=glGetUniformLocation(gShaderProgramObject,"u_Ld_blue");
	Ls_uniform_blue=glGetUniformLocation(gShaderProgramObject,"u_Ls_blue");
	light_position_uniform_blue=glGetUniformLocation(gShaderProgramObject,"u_light_position_blue");


	Ka_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	Kd_uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	Ks_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	getSphereVertexData(sphere_vertices,sphere_normals,sphere_textures,sphere_elements);
	gNumVertices=getNumberOfSphereVertices();
	gNumElements=getNumberOfSphereElements();

	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);
		glGenBuffers(1, &gVbo_sphere_position);
		glBindBuffer(GL_ARRAY_BUFFER,gVbo_sphere_position);
		glBufferData(GL_ARRAY_BUFFER,sizeof(sphere_vertices),
					sphere_vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(SSK_ATTRIBUTE_VERTEX,3,
							GL_FLOAT,GL_FALSE,0,NULL);
		glEnableVertexAttribArray(SSK_ATTRIBUTE_VERTEX);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glGenBuffers(1, &gVbo_sphere_normal);
		glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals),
					sphere_normals, GL_STATIC_DRAW);
		glVertexAttribPointer(SSK_ATTRIBUTE_NORMAL,
							3,GL_FLOAT,GL_FALSE,0,NULL);
		glEnableVertexAttribArray(SSK_ATTRIBUTE_NORMAL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glGenBuffers(1, &gVbo_sphere_element);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,gVbo_sphere_element);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(sphere_elements),
					sphere_elements,GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	gPerspectiveProjectionMatrix=vmath::mat4::identity();
	gbLightPerVertex = false;
	gbLightPerFragment = false;
	resize(WIN_WIDTH,WIN_HEIGHT);
}

void uninitialize(void){
	
	if (gbFullscreen == true){
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE | 
					SWP_NOOWNERZORDER | SWP_NOZORDER |
					SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (gVao_sphere){
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	if (gVbo_sphere_position){
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	if (gVbo_sphere_normal){
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	if (gVbo_sphere_element){
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}

	glDetachShader(gShaderProgramObject, gVertexShaderObject);
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;
	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(ghrc);
	ghrc = NULL;
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	if(gpFile){
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);
	
	glUseProgram(gShaderProgramObject);		
		if(gbLightPerFragment==true){
			//set 'u_lighting_enabled' uniform
			glUniform1i(F_KeyPressed_uniform, 1);
			glUniform1i(V_KeyPressed_uniform, 0);

			glUniform3fv(La_uniform_red, 1, lightAmbient_red);
			glUniform3fv(Ld_uniform_red, 1, lightDiffuse_red);
			glUniform3fv(Ls_uniform_red, 1, lightSpecular_red);
			lightPosition_red[0] = cos(3.1415*gAngle/180.0f)*10.0f;
			lightPosition_red[1] = 0.0f;
			lightPosition_red[2] = sin(3.1415*gAngle/180.0f)*10.0f;
			glUniform4fv(light_position_uniform_red, 1, lightPosition_red);

			glUniform3fv(La_uniform_green, 1, lightAmbient_green);
			glUniform3fv(Ld_uniform_green, 1, lightDiffuse_green);
			glUniform3fv(Ls_uniform_green, 1, lightSpecular_green);
			lightPosition_green[0] = 0.0f;
			lightPosition_green[1] = cos(3.1415*gAngle/180.0f)*10.0f;
			lightPosition_green[2] = sin(3.1415*gAngle/180.0f)*10.0f;
			glUniform4fv(light_position_uniform_green, 1, lightPosition_green);

			glUniform3fv(La_uniform_blue, 1, lightAmbient_blue);
			glUniform3fv(Ld_uniform_blue, 1, lightDiffuse_blue);
			glUniform3fv(Ls_uniform_blue, 1, lightSpecular_blue);
			lightPosition_blue[0] = cos(3.1415*gAngle/180.0f)*10.0f;
			lightPosition_blue[1] = sin(3.1415*gAngle/180.0f)*10.0f;
			lightPosition_blue[2] = 0.0f;
			glUniform4fv(light_position_uniform_blue, 1, lightPosition_blue);
			/*setting light's properties (___END___) */

			// setting material's properties
			glUniform3fv(Ka_uniform, 1, material_ambient);
			glUniform3fv(Kd_uniform, 1, material_diffuse);
			glUniform3fv(Ks_uniform, 1, material_specular);
			glUniform1f(material_shininess_uniform, material_shininess);

		}else{
			glUniform1i(F_KeyPressed_uniform, 0);
			glUniform1i(V_KeyPressed_uniform, 1);

			glUniform3fv(La_uniform_red, 1, lightAmbient_red);
			glUniform3fv(Ld_uniform_red, 1, lightDiffuse_red);
			glUniform3fv(Ls_uniform_red, 1, lightSpecular_red);
			lightPosition_red[0] = cos(3.1415*gAngle/180.0f)*10.0f;
			lightPosition_red[1] = 0.0f;
			lightPosition_red[2] = sin(3.1415*gAngle/180.0f)*10.0f;
			glUniform4fv(light_position_uniform_red, 1, lightPosition_red);

			glUniform3fv(La_uniform_green, 1, lightAmbient_green);
			glUniform3fv(Ld_uniform_green, 1, lightDiffuse_green);
			glUniform3fv(Ls_uniform_green, 1, lightSpecular_green);
			lightPosition_green[0] = 0.0f;
			lightPosition_green[1] = cos(3.1415*gAngle/180.0f)*10.0f;
			lightPosition_green[2] = sin(3.1415*gAngle/180.0f)*10.0f;
			glUniform4fv(light_position_uniform_green, 1, lightPosition_green);

			glUniform3fv(La_uniform_blue, 1, lightAmbient_blue);
			glUniform3fv(Ld_uniform_blue, 1, lightDiffuse_blue);
			glUniform3fv(Ls_uniform_blue, 1, lightSpecular_blue);
			lightPosition_blue[0] = cos(3.1415*gAngle/180.0f)*10.0f;
			lightPosition_blue[1] = sin(3.1415*gAngle/180.0f)*10.0f;
			lightPosition_blue[2] = 0.0f;
			glUniform4fv(light_position_uniform_blue, 1, lightPosition_blue);
			/*setting light's properties (___END___) */

			// setting material's properties
			glUniform3fv(Ka_uniform, 1, material_ambient);
			glUniform3fv(Kd_uniform, 1, material_diffuse);
			glUniform3fv(Ks_uniform, 1, material_specular);
			glUniform1f(material_shininess_uniform, material_shininess);
		}
	

	
		vmath::mat4 modelMatrix =vmath:: mat4::identity();
		vmath::mat4 viewMatrix =vmath::mat4::identity();
		modelMatrix=vmath::translate(0.0f,0.0f,-2.0f);

		glUniformMatrix4fv(model_matrix_uniform,1,GL_FALSE,modelMatrix);
		glUniformMatrix4fv(view_matrix_uniform,1,GL_FALSE,viewMatrix);
		glUniformMatrix4fv(projection_matrix_uniform,1,
							GL_FALSE,gPerspectiveProjectionMatrix);

		glBindVertexArray(gVao_sphere);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,gVbo_sphere_element);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
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
	gAngle=gAngle+0.1f;
	if (gAngle >= 360.0f){
		gAngle = 0.0f;
	}
}