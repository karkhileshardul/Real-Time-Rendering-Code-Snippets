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
bool gbLightPerVertex=false;
bool gbLightPerFragment=false;
bool gXRotate=0.0f;
bool gYRotate=0.0f;
bool gZRotate=0.0f;
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

GLuint gWindowWidth=0;
GLuint gWindowHeight=0;

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

GLfloat one_material_ambient[]={0.0215f,0.1745f,0.0215f,1.0f};
GLfloat one_material_diffuse[]={0.07568f,0.61424f,0.07568f,1.0f};
GLfloat one_material_specular[]={0.633f,0.727811f,0.633f,1.0f};
GLfloat one_material_shininess=76.8f;

GLfloat two_material_ambient[]={0.135f,0.2225f,0.1575f,1.0f};
GLfloat two_material_diffuse[]={0.54f,0.89f,0.63f,1.0f};
GLfloat two_material_specular[]={0.316228f,0.316228f,0.316228f,1.0f};
GLfloat two_material_shininess=12.8f;

GLfloat three_material_ambient[]={0.05375f,0.05f,0.06625f,1.0f};
GLfloat three_material_diffuse[]={0.18275f,0.17f,0.22525f,1.0f};
GLfloat three_material_specular[]={0.332741f,0.328634f,0.346435f,1.0f};
GLfloat three_material_shininess=38.4f;

GLfloat four_material_ambient[]={0.25f,0.20725f,0.20725f,1.0f};
GLfloat four_material_diffuse[]={1.0f,0.829f,0.829f,1.0f};
GLfloat four_material_specular[]={0.296648f,0.296648f,0.296648f,1.0f};
GLfloat four_material_shininess=11.264f;

GLfloat five_material_ambient[]={0.1745f,0.01175f,0.01175f,1.0f};
GLfloat five_material_diffuse[]={0.61424f,0.04136f,0.04136f,1.0f};
GLfloat five_material_specular[]={0.727811f,0.626959f,0.626959f,1.0f};
GLfloat five_material_shininess=76.8f;

GLfloat six_material_ambient[]={0.1f,0.18725f,0.1745f,1.0f};
GLfloat six_material_diffuse[]={0.396f,0.74151f,0.69102f,1.0f};
GLfloat six_material_specular[]={0.297254f,0.30829f,0.306678f,1.0f};
GLfloat six_material_shininess=12.8f;

GLfloat seven_material_ambient[]={0.329412f,0.223529f,0.027451f,1.0f};
GLfloat seven_material_diffuse[]={0.780392f,0.568627f,0.113725f,1.0f};
GLfloat seven_material_specular[]={0.992157f,0.941176f,0.807843f,1.0f};
GLfloat seven_material_shininess=27.89743616f;

GLfloat eight_material_ambient[]={0.2125f,0.1275f,0.054f,1.0f};
GLfloat eight_material_diffuse[]={0.714f,0.4284f,0.18144f,1.0f};
GLfloat eight_material_specular[]={0.393548f,0.271906f,0.166721f,1.0f};
GLfloat eight_material_shininess=25.6f;

GLfloat nine_material_ambient[]={0.25f,0.25f,0.25f,1.0f};
GLfloat nine_material_diffuse[]={0.4f,0.4f,0.4f,1.0f};
GLfloat nine_material_specular[]={0.774597f,0.774597f,0.774597f,1.0f};
GLfloat nine_material_shininess=76.8f;

GLfloat ten_material_ambient[]={0.19125f,0.0735f,0.0225f,1.0f};
GLfloat ten_material_diffuse[]={0.7038f,0.27048f,0.0828f,1.0f};
GLfloat ten_material_specular[]={0.256777f,0.137622f,0.086014f,1.0f};
GLfloat ten_material_shininess=12.8f;

GLfloat eleven_material_ambient[]={0.24725f,0.1995f,0.0745f,1.0f};
GLfloat eleven_material_diffuse[]={0.75164f,0.60648f,0.22648f,1.0f};
GLfloat eleven_material_specular[]={0.628281f,0.555802f,0.366065f,1.0f};
GLfloat eleven_material_shininess=51.2f;

GLfloat twelve_material_ambient[]={0.19225f,0.19225f,0.19225f,1.0f};
GLfloat twelve_material_diffuse[]={0.50754f,0.50754f,0.50754f,1.0f};
GLfloat twelve_material_specular[]={0.508273f,0.508273f,0.508273f,1.0f};
GLfloat twelve_material_shininess=51.2f;

GLfloat thirteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat thirteen_material_diffuse[]={0.01f,0.01f,0.01f,1.0f};
GLfloat thirteen_material_specular[]={0.50f,0.50f,0.50f,1.0f};
GLfloat thirteen_material_shininess=32.0f;

GLfloat fourteen_material_ambient[]={0.0f,0.1f,0.06f,1.0f};
GLfloat fourteen_material_diffuse[]={0.0f,0.50980392f,0.50980392f,1.0f};
GLfloat fourteen_material_specular[]={0.50196078f,0.50196078f,0.50196078f,1.0f};
GLfloat fourteen_material_shininess=32.0f;

GLfloat fifteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat fifteen_material_diffuse[]={0.1f,0.35f,0.1f,1.0f};
GLfloat fifteen_material_specular[]={0.45f,0.55f,0.45f,1.0f};
GLfloat fifteen_material_shininess=32.0f;

GLfloat sixteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat sixteen_material_diffuse[]={0.5f,0.0f,0.0f,1.0f};
GLfloat sixteen_material_specular[]={0.7f,0.6f,0.6f,1.0f};
GLfloat sixteen_material_shininess=32.0f;

GLfloat seventeen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat seventeen_material_diffuse[]={0.55f,0.55f,0.55f,1.0f};
GLfloat seventeen_material_specular[]={0.70f,0.70f,0.70f,1.0f};
GLfloat seventeen_material_shininess=32.0f;

GLfloat eighteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
GLfloat eighteen_material_diffuse[]={0.5f,0.5f,0.0f,1.0f};
GLfloat eighteen_material_specular[]={0.60f,0.60f,0.50f,1.0f};
GLfloat eighteen_material_shininess=32.0f;

GLfloat nineteen_material_ambient[]={0.02f,0.02f,0.02f,1.0f};
GLfloat nineteen_material_diffuse[]={0.01f,0.01f,0.01f,1.0f};
GLfloat nineteen_material_specular[]={0.4f,0.4f,0.4f,1.0f};
GLfloat nineteen_material_shininess=10.0f;

GLfloat twenty_material_ambient[]={0.0f,0.05f,0.05f,1.0f};
GLfloat twenty_material_diffuse[]={0.4f,0.5f,0.5f,1.0f};
GLfloat twenty_material_specular[]={0.04f,0.7f,0.7f,1.0f};
GLfloat twenty_material_shininess=10.0f;

GLfloat twentyone_material_ambient[]={0.0f,0.05f,0.0f,1.0f};
GLfloat twentyone_material_diffuse[]={0.4f,0.5f,0.4f,1.0f};
GLfloat twentyone_material_specular[]={0.04f,0.7f,0.04f,1.0f};
GLfloat twentyone_material_shininess=10.0f;

GLfloat twentytwo_material_ambient[]={0.05f,0.0f,0.0f,1.0f};
GLfloat twentytwo_material_diffuse[]={0.5f,0.4f,0.4f,1.0f};
GLfloat twentytwo_material_specular[]={0.7f,0.04f,0.04f,1.0f};
GLfloat twentytwo_material_shininess=10.0f;

GLfloat twentythree_material_ambient[]={0.05f,0.05f,0.05f,1.0f};
GLfloat twentythree_material_diffuse[]={0.5f,0.5f,0.5f,1.0f};
GLfloat twentythree_material_specular[]={0.7f,0.7f,0.7f,1.0f};
GLfloat twentythree_material_shininess=10.0f;

GLfloat twentyfour_material_ambient[]={0.05f,0.05f,0.0f,1.0f};
GLfloat twentyfour_material_diffuse[]={0.5f,0.5f,0.4f,1.0f};
GLfloat twentyfour_material_specular[]={0.7f,0.7f,0.04f,1.0f};
GLfloat twentyfour_material_shininess=10.0f;

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
	static bool bIsVKeyPressed=false;
	static bool bIsFKeyPressed=false;
	
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
							if(gbFullscreen==false){
								ToggleFullscreen();
								gbFullscreen=true;
							}else{
								ToggleFullscreen();
								gbFullscreen=false;
							}
							break;
						case XK_F:
						case XK_f:
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
						case XK_V:
						case XK_v:
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
						case XK_Q: 
						case XK_q:
							bDone=true;
							break;			
						case XK_X:
						case XK_x:
							if(gXRotate==false){
								gXRotate=true;
								gYRotate=false;
								gZRotate=false;
							}else{
								gXRotate=false;
								gYRotate=false;
								gZRotate=false;
							}
							break;
						case XK_Y:
						case XK_y:
							if(gYRotate==false){
								gYRotate=true;
								gXRotate=false;
								gZRotate=false;
							}else{
								gYRotate=false;
								gXRotate=false;
								gZRotate=false;
							}
							break;
						case XK_Z:
						case XK_z:
							if(gZRotate==false){
								gZRotate=true;
								gYRotate=false;
								gXRotate=false;
							}else{
								gZRotate=false;
								gYRotate=false;
								gXRotate=false;
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

		update();
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
	
	XStoreName(gpDisplay,gWindow,"20-24Sphere Lights Window");
	
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
		"#version 130" \
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

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	gPerspectiveProjectionMatrix=vmath::mat4::identity();

	resize(giWindowWidth, giWindowHeight);
}

void resize(int width,int height)
{
	if(height==0)
		height=1;
	gWindowWidth=width;
	gWindowHeight=height;
		
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
	GLuint currentWidth=0;
	GLuint currentHeight=0;
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
			/*First Row (___START___)*/
			glViewport(0,0,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,one_material_ambient);
			glUniform3fv(Kd_uniform,1,one_material_diffuse);
			glUniform3fv(Ks_uniform,1,one_material_specular);
			glUniform1f(material_shininess_uniform,one_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth =currentWidth+ gWindowWidth / 6;

			glViewport(currentWidth,0,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,two_material_ambient);
			glUniform3fv(Kd_uniform,1,two_material_diffuse);
			glUniform3fv(Ks_uniform,1,two_material_specular);
			glUniform1f(material_shininess_uniform,two_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth =currentWidth+ gWindowWidth / 6;

			glViewport(currentWidth,0,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,three_material_ambient);
			glUniform3fv(Kd_uniform,1,three_material_diffuse);
			glUniform3fv(Ks_uniform,1,three_material_specular);
			glUniform1f(material_shininess_uniform,three_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth =currentWidth+ gWindowWidth / 6;

			glViewport(currentWidth,0,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,four_material_ambient);
			glUniform3fv(Kd_uniform,1,four_material_diffuse);
			glUniform3fv(Ks_uniform,1,four_material_specular);
			glUniform1f(material_shininess_uniform,four_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth =currentWidth+ gWindowWidth / 6;

			glViewport(currentWidth,0,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,five_material_ambient);
			glUniform3fv(Kd_uniform,1,five_material_diffuse);
			glUniform3fv(Ks_uniform,1,five_material_specular);
			glUniform1f(material_shininess_uniform,five_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth =currentWidth+ gWindowWidth / 6;

			glViewport(currentWidth,0,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,six_material_ambient);
			glUniform3fv(Kd_uniform,1,six_material_diffuse);
			glUniform3fv(Ks_uniform,1,six_material_specular);
			glUniform1f(material_shininess_uniform,six_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
			/*First Row (___END___)*/

			/*Second Row (___START___)*/
			currentHeight=currentHeight+gWindowHeight/4;
			currentWidth=0;

			glViewport(0,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,seven_material_ambient);
			glUniform3fv(Kd_uniform,1,seven_material_diffuse);
			glUniform3fv(Ks_uniform,1,seven_material_specular);
			glUniform1f(material_shininess_uniform,seven_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth+ gWindowWidth/ 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,eight_material_ambient);
			glUniform3fv(Kd_uniform,1,eight_material_diffuse);
			glUniform3fv(Ks_uniform,1,eight_material_specular);
			glUniform1f(material_shininess_uniform,eight_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth + gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,nine_material_ambient);
			glUniform3fv(Kd_uniform,1,nine_material_diffuse);
			glUniform3fv(Ks_uniform,1,nine_material_specular);
			glUniform1f(material_shininess_uniform,nine_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth + gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,ten_material_ambient);
			glUniform3fv(Kd_uniform,1,ten_material_diffuse);
			glUniform3fv(Ks_uniform,1,ten_material_specular);
			glUniform1f(material_shininess_uniform,ten_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth + gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,eleven_material_ambient);
			glUniform3fv(Kd_uniform,1,eleven_material_diffuse);
			glUniform3fv(Ks_uniform,1,eleven_material_specular);
			glUniform1f(material_shininess_uniform,eleven_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth =currentWidth+gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,twelve_material_ambient);
			glUniform3fv(Kd_uniform,1,twelve_material_diffuse);
			glUniform3fv(Ks_uniform,1,twelve_material_specular);
			glUniform1f(material_shininess_uniform,twelve_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
			/*Second Row (___END___)*/

			/*Third Row (___START___)*/
			currentHeight=currentHeight+gWindowHeight/4;
			currentWidth=0;

			glViewport(0,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,thirteen_material_ambient);
			glUniform3fv(Kd_uniform,1,thirteen_material_diffuse);
			glUniform3fv(Ks_uniform,1,thirteen_material_specular);
			glUniform1f(material_shininess_uniform,thirteen_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth + gWindowWidth/ 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,fourteen_material_ambient);
			glUniform3fv(Kd_uniform,1,fourteen_material_diffuse);
			glUniform3fv(Ks_uniform,1,fourteen_material_specular);
			glUniform1f(material_shininess_uniform,fourteen_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth + gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,fifteen_material_ambient);
			glUniform3fv(Kd_uniform,1,fifteen_material_diffuse);
			glUniform3fv(Ks_uniform,1,fifteen_material_specular);
			glUniform1f(material_shininess_uniform,fifteen_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth+gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,sixteen_material_ambient);
			glUniform3fv(Kd_uniform,1,sixteen_material_diffuse);
			glUniform3fv(Ks_uniform,1,sixteen_material_specular);
			glUniform1f(material_shininess_uniform,sixteen_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth+gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,seventeen_material_ambient);
			glUniform3fv(Kd_uniform,1,seventeen_material_diffuse);
			glUniform3fv(Ks_uniform,1,seventeen_material_specular);
			glUniform1f(material_shininess_uniform,seventeen_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth =currentWidth+gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,eighteen_material_ambient);
			glUniform3fv(Kd_uniform,1,eighteen_material_diffuse);
			glUniform3fv(Ks_uniform,1,eighteen_material_specular);
			glUniform1f(material_shininess_uniform,eighteen_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);

			/*Third Row (___END___)*/

			/*Fourth Row (___START___)*/
			currentHeight=currentHeight+gWindowHeight/4;
			currentWidth=0;

			glViewport(0,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,nineteen_material_ambient);
			glUniform3fv(Kd_uniform,1,nineteen_material_diffuse);
			glUniform3fv(Ks_uniform,1,nineteen_material_specular);
			glUniform1f(material_shininess_uniform,nineteen_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth + gWindowWidth/ 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,twenty_material_ambient);
			glUniform3fv(Kd_uniform,1,twenty_material_diffuse);
			glUniform3fv(Ks_uniform,1,twenty_material_specular);
			glUniform1f(material_shininess_uniform,twenty_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth + gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,twentyone_material_ambient);
			glUniform3fv(Kd_uniform,1,twentyone_material_diffuse);
			glUniform3fv(Ks_uniform,1,twentyone_material_specular);
			glUniform1f(material_shininess_uniform,twentyone_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth+gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,twentytwo_material_ambient);
			glUniform3fv(Kd_uniform,1,twentytwo_material_diffuse);
			glUniform3fv(Ks_uniform,1,twentytwo_material_specular);
			glUniform1f(material_shininess_uniform,twentytwo_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth = currentWidth+gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,twentythree_material_ambient);
			glUniform3fv(Kd_uniform,1,twentythree_material_diffuse);
			glUniform3fv(Ks_uniform,1,twentythree_material_specular);
			glUniform1f(material_shininess_uniform,twentythree_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);
		
			currentWidth =currentWidth+gWindowWidth / 6;

			glViewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
			glUniform3fv(Ka_uniform,1,twentyfour_material_ambient);
			glUniform3fv(Kd_uniform,1,twentyfour_material_diffuse);
			glUniform3fv(Ks_uniform,1,twentyfour_material_specular);
			glUniform1f(material_shininess_uniform,twentyfour_material_shininess);
			glDrawElements(GL_TRIANGLES,gNumElements,GL_UNSIGNED_SHORT,0);

			/*Four Row (___END___)*/



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

