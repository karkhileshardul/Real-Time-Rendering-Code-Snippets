#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 
#include <memory.h> 
#include <X11/Xlib.h> 
#include <X11/Xutil.h> 
#include <X11/XKBlib.h> 
#include <X11/keysym.h> 
#include <GL/glew.h> 
#include <GL/gl.h>
#include <GL/glx.h> 
#include "vmath.h"
#include "Sphere.h"

int giWindowWidth=800;
int giWindowHeight=600;
FILE *gpFile = NULL;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB=NULL;
GLXFBConfig gGLXFBConfig;
GLXContext gGLXContext; 
bool gbFullscreen = false;
enum{
	SSK_ATTRIBUTE_VERTEX=0,
	SSK_ATTRIBUTE_COLOR,
	SSK_ATTRIBUTE_NORMAL,
	SSK_ATTRIBUTE_TEXTURE0,
};

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
vmath::mat4 gPerspectiveProjectionMatrix;
GLfloat gAngle=0.0f;
bool gbAnimate;
bool gbLight;


GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;
GLuint gNumElements;
GLuint gNumVertices;

GLuint model_matrix_uniform,view_matrix_uniform,projection_matrix_uniform;
GLuint L_KeyPressed_uniform;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

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


int main(int argc, char *argv[])
{
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void initialize(void);
	void resize(int,int);
	void display(void);
	void uninitialize(void);
	void update(void);

	XEvent event; 
	KeySym keySym;
	int winWidth;
	int winHeight;
	bool bDone=false;
	static bool bIsAKeyPressed=false;
	static bool bIsLKeyPressed=false;

	gpFile=fopen("SSKLogFile.txt", "w");
	if (gpFile==NULL)
	{
		printf("Log File Can Not Be Created. EXitting Now ...\n");
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened.\n");
	}
	
	CreateWindow();
	initialize();
	
	while(bDone==false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay,&event); 
			switch(event.type)
			{
				case MapNotify: 
					break;
				case KeyPress: 
					keySym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
					switch(keySym)
					{
						case XK_Escape:
							bDone=true;
							break;
						case XK_F:
						case XK_f:
							if(gbFullscreen==false)
							{
								ToggleFullscreen();
								gbFullscreen=true;
							}
							else
							{
								ToggleFullscreen();
								gbFullscreen=false;
							}
							break;
						case XK_A:
						case XK_a:
							if(bIsAKeyPressed==false){
								gbAnimate=true;
								bIsAKeyPressed=true;
							}else{
								gbAnimate=false;
								bIsAKeyPressed=false;
							}
							break;
						case XK_L:
						case XK_l:
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
				case ConfigureNotify: 
					winWidth=event.xconfigure.width;
					winHeight=event.xconfigure.height;
					resize(winWidth,winHeight);
					break;
				case 33: 
					bDone=true;
					break;
				default:
					break;
			}
		}
					display();
		if(gbAnimate==true){
			update();
		}
		display();
	}
	
	uninitialize();
	return(0);
}

void CreateWindow(void)
{
	void uninitialize(void);

	XSetWindowAttributes winAttribs;
	GLXFBConfig *pGLXFBConfigs=NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *pTempXVisualInfo=NULL;
	int iNumFBConfigs=0;
	int styleMask;
	int i;
	
	static int frameBufferAttributes[]={
		GLX_X_RENDERABLE,True,
		GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE,GLX_TRUE_COLOR,
		GLX_RED_SIZE,8,
		GLX_GREEN_SIZE,8,
		GLX_BLUE_SIZE,8,
		GLX_ALPHA_SIZE,8,
		GLX_DEPTH_SIZE,24,
		GLX_STENCIL_SIZE,8,
		GLX_DOUBLEBUFFER,True,
		GLX_SAMPLE_BUFFERS,1,
		GLX_SAMPLES,4,
		None};
	
	
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
		printf("ERROR : Unable To Obtain X Display.\n");
		uninitialize();
		exit(1);
	}
	
	pGLXFBConfigs=glXChooseFBConfig(gpDisplay,DefaultScreen(gpDisplay),frameBufferAttributes,&iNumFBConfigs);
	if(pGLXFBConfigs==NULL)
	{
		printf( "Failed To Get Valid Framebuffer Config. Exitting Now ...\n");
		uninitialize();
		exit(1);
	}
	printf("%d Matching FB Configs Found.\n",iNumFBConfigs);
	
	
	int bestFramebufferconfig=-1,worstFramebufferConfig=-1,bestNumberOfSamples=-1,worstNumberOfSamples=999;
	for(i=0;i<iNumFBConfigs;i++)
	{
		pTempXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,pGLXFBConfigs[i]);
		if(pTempXVisualInfo)
		{
			int sampleBuffer,samples;
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLE_BUFFERS,&sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLES,&samples);
			printf("Matching Framebuffer Config=%d : Visual ID=0x%lu : SAMPLE_BUFFERS=%d : SAMPLES=%d\n",i,pTempXVisualInfo->visualid,sampleBuffer,samples);
			if(bestFramebufferconfig < 0 || sampleBuffer && samples > bestNumberOfSamples)
			{
				bestFramebufferconfig=i;
				bestNumberOfSamples=samples;
			}
			if( worstFramebufferConfig < 0 || !sampleBuffer || samples < worstNumberOfSamples)
			{
				worstFramebufferConfig=i;
			    worstNumberOfSamples=samples;
			}
		}
		XFree(pTempXVisualInfo);
	}
	bestGLXFBConfig = pGLXFBConfigs[bestFramebufferconfig];
	
	gGLXFBConfig=bestGLXFBConfig;
	
	XFree(pGLXFBConfigs);
	
	gpXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,bestGLXFBConfig);
	printf("Chosen Visual ID=0x%lu\n",gpXVisualInfo->visualid );
	

	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen), 
						gpXVisualInfo->visual,AllocNone); 
										
	winAttribs.event_mask=StructureNotifyMask | KeyPressMask | ButtonPressMask |
						  ExposureMask | VisibilityChangeMask | PointerMotionMask;
	
	styleMask=CWBorderPixel | CWEventMask | CWColormap;
	gColormap=winAttribs.colormap;										           
	
	gWindow=XCreateWindow(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),
				0,0,giWindowWidth,giWindowHeight,0,gpXVisualInfo->depth,
				InputOutput,gpXVisualInfo->visual,styleMask,&winAttribs);
	if(!gWindow){
		printf("Failure In Window Creation.\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay,gWindow,"15-Sphere_PerVertex Window");
	
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_WINDOW_DELETE",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	
	XMapWindow(gpDisplay,gWindow);
}

void ToggleFullscreen(void)
{

	Atom wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False); 
	
	XEvent event;
	memset(&event,0,sizeof(XEvent));
	
	event.type=ClientMessage;
	event.xclient.window=gWindow;
	event.xclient.message_type=wm_state;
	event.xclient.format=32; 
	event.xclient.data.l[0]=gbFullscreen ? 0 : 1;

	Atom fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);	
	event.xclient.data.l[1]=fullscreen;
	
	XSendEvent(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),
		  False,StructureNotifyMask,&event);	
}

void initialize(void)
{
	void LoadGLTextures(GLuint *,const char*);
	void uninitialize(void);
	void resize(int,int);	

	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");
	
	GLint attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB,1,
		GLX_CONTEXT_MINOR_VERSION_ARB,3,
		GLX_CONTEXT_PROFILE_MASK_ARB,
		GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0 
	}; 
		
	gGLXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);

	if(!gGLXContext){

		GLint attribs[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB,1,
			GLX_CONTEXT_MINOR_VERSION_ARB,0,
			0 }; 
		printf("Failed To Create GLX 4.5 context. Hence Using Old-Style GLX Context\n");
		gGLXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);
	}
	else 
	{
		printf("OpenGL Context Is Created.\n");
	}
	
	if(!glXIsDirect(gpDisplay,gGLXContext))
	{
		printf("Indirect GLX Rendering Context Obtained\n");
	}
	else
	{
		printf("Direct GLX Rendering Context Obtained\n" );
	}
	
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		printf("Failure To Initialize GLEW. Exitting Now ...\n");
		uninitialize();
		exit(1);
	}

	gVertexShaderObject=glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCode=
		"#version 130" \
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
		"#version 130" \
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

    getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

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

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	gPerspectiveProjectionMatrix=vmath::mat4::identity();
	gbAnimate = false;
	gbLight = false;
	resize(giWindowWidth, giWindowHeight);
}

void resize(int width,int height)
{
	if(height==0)
		height=1;
		
	glViewport(0,0,(GLsizei)width,(GLsizei)height);

	if(width<=height){
		gPerspectiveProjectionMatrix=vmath::perspective(45.0f,(GLfloat)height/(GLfloat)width,
								0.1f,100.0f);
	}else{
		gPerspectiveProjectionMatrix=vmath::perspective(45.0f,(GLfloat)width/(GLfloat)height,
								0.1f,100.0f);
	}
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
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
	glXSwapBuffers(gpDisplay,gWindow);
}

void uninitialize(void)
{
	GLXContext currentContext=glXGetCurrentContext();
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

	glDetachShader(gShaderProgramObject,gVertexShaderObject);
	glDetachShader(gShaderProgramObject,gFragmentShaderObject);

	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject=0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject=0;

	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject=0;

	glUseProgram(0);

	if(currentContext!=NULL && currentContext==gGLXContext)
	{
		glXMakeCurrent(gpDisplay,0,0);
	}
	
	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay,gGLXContext);
	}
	
	if(gWindow)
	{
		XDestroyWindow(gpDisplay,gWindow);
	}
	
	if(gColormap)
	{
		XFreeColormap(gpDisplay,gColormap);
	}
	
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

void update(void){
	gAngle=gAngle+0.3f;
	if(gAngle>=360.0f){
		gAngle=gAngle-360.0f;
	}
}

