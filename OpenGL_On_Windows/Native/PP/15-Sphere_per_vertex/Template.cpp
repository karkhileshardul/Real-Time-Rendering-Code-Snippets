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
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
vmath::mat4 gPerspectiveProjectionMatrix;
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

GLuint model_matrix_uniform,view_matrix_uniform,projection_matrix_uniform;
bool gbLight;
GLuint L_KeyPressed_uniform;

float sphere_normals[1146];
float sphere_vertices[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint gNumElements;
GLuint gNumVertices;

GLuint La_uniform;
GLuint Ld_uniform;
GLuint Ls_uniform;
GLuint light_position_uniform;
GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;

GLfloat lightAmbient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat lightDiffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat lightSpecular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat lightPosition[]={100.0f,100.0f,100.0f,1.0f};
GLfloat material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat material_diffuse[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_specular[]={1.0f,1.0f,1.0f,1.0f};
GLfloat material_shininess=50.0f;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	void initialize(void);
	void uninitialize(void);
	void display(void);

	WNDCLASSEX wndclassex;
	HWND hwnd;
	TCHAR szClassName[]=TEXT("15-Sphere_PerVertex Window");
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
				case 0x4C:
					if(bIsLKeyPressed==false){
						gbLight=true;
						bIsLKeyPressed=true;
					}else{
						gbLight=false;
						bIsLKeyPressed=false;
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
		"uniform vec3 u_La;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Ls;" \
		"uniform vec3 u_light_position;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"out vec3 phong_ads_color;" \
		"void main(void)" \
		"{" \
		"if(u_lighting_enabled==1)" \
		"{" \
		"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
		"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
		"vec3 light_direction=normalize(vec3(u_light_position) - eye_coordinates.xyz);" \
		"float tn_dot_ld=max(dot(transformed_normals,light_direction),0.0);" \
		"vec3 ambient=u_La * u_Ka;" \
		"vec3 diffuse= u_Ld * u_Kd *tn_dot_ld;" \
		"vec3 reflection_vector=reflect(-light_direction,transformed_normals);" \
		"vec3 viewer_vector=normalize(-eye_coordinates.xyz);" \
		"vec3 specular= u_Ls * u_Ks * pow(max(dot(reflection_vector,viewer_vector),0.0),u_material_shininess);" \
		"phong_ads_color=ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_color=vec3(1.0,1.0,1.0f);" \
		"}" \
		"gl_Position=u_projection_matrix *u_view_matrix *u_model_matrix * vPosition;" \
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
		"in vec3 phong_ads_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phong_ads_color, 1.0);" \
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

	L_KeyPressed_uniform=glGetUniformLocation(gShaderProgramObject,"u_lighting_enabled");
	//ambient color intensity of light
	La_uniform=glGetUniformLocation(gShaderProgramObject,"u_La");
	//diffuse color intensity of light
	Ld_uniform=glGetUniformLocation(gShaderProgramObject,"u_Ld");
	//specular color intensity of light
	Ls_uniform=glGetUniformLocation(gShaderProgramObject,"u_Ls");
	//position of light
	light_position_uniform=glGetUniformLocation(gShaderProgramObject,"u_light_position");

	//ambient reflective color intensity of material
	Ka_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	//diffuse reflective color intensity of material
	Kd_uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	//specular reflective color intensity of material
	Ks_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	//shininess of material
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
	gbLight = false;
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

		if(gbLight==true){
			//set 'u_lighting_enabled' uniform
			glUniform1i(L_KeyPressed_uniform, 1);

			//setting light's properties
			glUniform3fv(La_uniform, 1, lightAmbient);
			glUniform3fv(Ld_uniform, 1, lightDiffuse);
			glUniform3fv(Ls_uniform, 1, lightSpecular);
			glUniform4fv(light_position_uniform, 1, lightPosition);

			// setting material's properties
			glUniform3fv(Ka_uniform, 1, material_ambient);
			glUniform3fv(Kd_uniform, 1, material_diffuse);
			glUniform3fv(Ks_uniform, 1, material_specular);
			glUniform1f(material_shininess_uniform, material_shininess);
		}else{
			// set 'u_lighting_enabled' uniform
			glUniform1i(L_KeyPressed_uniform, 0);
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
