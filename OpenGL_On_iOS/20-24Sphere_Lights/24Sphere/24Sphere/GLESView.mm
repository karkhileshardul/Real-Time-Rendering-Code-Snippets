//
//  GLESView.m
//  24Sphere
//
//  Created by DATTATRAY BADHE on 24/05/1940 Saka.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import "GLESView.h"
#import "vmath.h"
#import "Sphere.h"

enum{
    SSK_ATTRIBUTE_VERTEX = 0,
    SSK_ATTRIBUTE_COLOR,
    SSK_ATTRIBUTE_NORMAL,
    SSK_ATTRIBUTE_TEXTURE0,
};


float sphere_normals[1146];
float sphere_vertices[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];


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


GLfloat gAngle=0.0f;
bool gbLightPerVertex=false;
bool gbLightPerFragment=false;

bool bIsLKeyPressed=false;
@implementation GLESView{
    EAGLContext *eaglContext;
    
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    GLuint gVao_sphere;
    GLuint gVbo_sphere_position;
    GLuint gVbo_sphere_normal;
    GLuint gVbo_sphere_element;
    
    GLuint gNumElements;
    GLuint gNumVertices;
    
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
    

    vmath::mat4 perspectiveProjectionMatrix;
    
    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
    
}


-(id)initWithFrame:(CGRect)frame;{
    // code
    self=[super initWithFrame:frame];
    
    if(self){
        CAEAGLLayer *eaglLayer=(CAEAGLLayer *)super.layer;
        eaglLayer.opaque=YES;
        eaglLayer.drawableProperties=[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE],
                                      kEAGLDrawablePropertyRetainedBacking,kEAGLColorFormatRGBA8,kEAGLDrawablePropertyColorFormat,nil];
        
        eaglContext=[[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if(eaglContext==nil){
            [self release];
            return(nil);
        }
        [EAGLContext setCurrentContext:eaglContext];
        
        glGenFramebuffers(1,&defaultFramebuffer);
        glGenRenderbuffers(1,&colorRenderbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
        
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,colorRenderbuffer);
        
        GLint backingWidth;
        GLint backingHeight;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_WIDTH,&backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_HEIGHT,&backingHeight);
        
        glGenRenderbuffers(1,&depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER,depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,backingWidth,backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRenderbuffer);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){
            printf("Failed To Create Complete Framebuffer Object %x\n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
            glDeleteFramebuffers(1,&defaultFramebuffer);
            glDeleteRenderbuffers(1,&colorRenderbuffer);
            glDeleteRenderbuffers(1,&depthRenderbuffer);
            
            return(nil);
        }
        
        printf("Renderer : %s | GL Version : %s | GLSL Version : %s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION),glGetString(GL_SHADING_LANGUAGE_VERSION));
        
        // hard coded initializations
        isAnimating=NO;
        animationFrameInterval=60; // default since iOS 8.2
        
        // *** VERTEX SHADER ***
        // create shader
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
        // provide source code to shader
        const GLchar *vertexShaderSourceCode =
        "#version 300 es"\
        "\n"\
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

        
        glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
        
        // compile shader
        glCompileShader(vertexShaderObject);
        GLint iInfoLogLength = 0;
        GLint iShaderCompiledStatus = 0;
        char *szInfoLog = NULL;
        glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
        if (iShaderCompiledStatus == GL_FALSE){
            glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength > 0){
                szInfoLog = (char *)malloc(iInfoLogLength);
                if (szInfoLog != NULL){
                    GLsizei written;
                    glGetShaderInfoLog(vertexShaderObject, iInfoLogLength, &written, szInfoLog);
                    printf("Vertex Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        
        iInfoLogLength = 0;
        iShaderCompiledStatus = 0;
        szInfoLog = NULL;
        
        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        
        const GLchar *fragmentShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "precision highp float;" \
        "precision highp int;"\
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
        
        glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
        
        
        glCompileShader(fragmentShaderObject);
        glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
        if (iShaderCompiledStatus == GL_FALSE){
            glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength > 0){
                szInfoLog = (char *)malloc(iInfoLogLength);
                if (szInfoLog != NULL){
                    GLsizei written;
                    glGetShaderInfoLog(fragmentShaderObject, iInfoLogLength, &written, szInfoLog);
                    printf("Fragment Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        shaderProgramObject = glCreateProgram();
        
        glAttachShader(shaderProgramObject, vertexShaderObject);
        
        glAttachShader(shaderProgramObject, fragmentShaderObject);
        
        glBindAttribLocation(shaderProgramObject, SSK_ATTRIBUTE_VERTEX, "vPosition");
        glBindAttribLocation(shaderProgramObject, SSK_ATTRIBUTE_NORMAL, "vNormal");
        glLinkProgram(shaderProgramObject);
        GLint iShaderProgramLinkStatus = 0;
        glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
        if (iShaderProgramLinkStatus == GL_FALSE){
            glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength>0){
                szInfoLog = (char *)malloc(iInfoLogLength);
                if (szInfoLog != NULL){
                    GLsizei written;
                    glGetProgramInfoLog(shaderProgramObject, iInfoLogLength, &written, szInfoLog);
                    printf("Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        model_matrix_uniform=glGetUniformLocation(shaderProgramObject,
                                                  "u_model_matrix");
        view_matrix_uniform=glGetUniformLocation(shaderProgramObject,
                                                 "u_view_matrix");
        projection_matrix_uniform=glGetUniformLocation(shaderProgramObject,
                                                       "u_projection_matrix");
        
        V_KeyPressed_uniform=glGetUniformLocation(shaderProgramObject,"u_per_vertex_lighting_enabled");
        F_KeyPressed_uniform=glGetUniformLocation(shaderProgramObject,"u_per_fragment_lighting_enabled");
        
        La_uniform_red=glGetUniformLocation(shaderProgramObject,"u_La_red");
        Ld_uniform_red=glGetUniformLocation(shaderProgramObject,"u_Ld_red");
        Ls_uniform_red=glGetUniformLocation(shaderProgramObject,"u_Ls_red");
        light_position_uniform_red=glGetUniformLocation(shaderProgramObject,"u_light_position_red");
        
        La_uniform_green=glGetUniformLocation(shaderProgramObject,"u_La_green");
        Ld_uniform_green=glGetUniformLocation(shaderProgramObject,"u_Ld_green");
        Ls_uniform_green=glGetUniformLocation(shaderProgramObject,"u_Ls_green");
        light_position_uniform_green=glGetUniformLocation(shaderProgramObject,"u_light_position_green");
        
        La_uniform_blue=glGetUniformLocation(shaderProgramObject,"u_La_blue");
        Ld_uniform_blue=glGetUniformLocation(shaderProgramObject,"u_Ld_blue");
        Ls_uniform_blue=glGetUniformLocation(shaderProgramObject,"u_Ls_blue");
        light_position_uniform_blue=glGetUniformLocation(shaderProgramObject,"u_light_position_blue");
        
        
        Ka_uniform = glGetUniformLocation(shaderProgramObject, "u_Ka");
        Kd_uniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
        Ks_uniform = glGetUniformLocation(shaderProgramObject, "u_Ks");
        material_shininess_uniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
        
        
        Sphere *sphere= new Sphere();
        sphere->getSphereVertexData(sphere_vertices,sphere_normals,sphere_textures,sphere_elements);
        
        gNumVertices=sphere->getNumberOfSphereVertices();
        gNumElements=sphere->getNumberOfSphereElements();
        
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

        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        
        perspectiveProjectionMatrix = vmath::mat4::identity();
        gbLightPerVertex = false;
        gbLightPerFragment = false;
        
        UITapGestureRecognizer *singleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1]; // 1 finger touch
        
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1]; // touch of 1 finger
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        UISwipeGestureRecognizer *swipeGestureRecognizer=[[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        UILongPressGestureRecognizer *longPressGestureRecognizer=[[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [self addGestureRecognizer:longPressGestureRecognizer];
    }
    return(self);
}


+(Class)layerClass{
    return([CAEAGLLayer class]);
}

-(void)drawView:(id)sender{
    GLuint currentWidth=0;
    GLuint currentHeight=0;
    
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
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
    modelMatrix=vmath::translate(0.0f,0.0f,-4.0f);
    
    glUniformMatrix4fv(model_matrix_uniform,1,GL_FALSE,modelMatrix);
    glUniformMatrix4fv(view_matrix_uniform,1,GL_FALSE,viewMatrix);
    glUniformMatrix4fv(projection_matrix_uniform,1,
                       GL_FALSE,perspectiveProjectionMatrix);
    
    
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
    
    [self update];

    glUseProgram(0);
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}

-(void)layoutSubviews{
    GLint width;
    GLint height;
    
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_WIDTH,&width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_HEIGHT,&height);
    
    glGenRenderbuffers(1,&depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRenderbuffer);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("Failed To Create Complete Framebuffer Object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    gWindowWidth=width;
    gWindowHeight=height;
    glViewport(0,0,(GLsizei)width,(GLsizei)height);
    
    GLfloat fwidth = (GLfloat)width;
    GLfloat fheight = (GLfloat)height;
    perspectiveProjectionMatrix = vmath::perspective(45.0f,fwidth/fheight,0.1f,100.0f);
    
    [self drawView:nil];
}

-(void)startAnimation{
    if (!isAnimating){
        displayLink=[NSClassFromString(@"CADisplayLink")displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFramesPerSecond:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        isAnimating=YES;
    }
}

-(void)stopAnimation{
    if(isAnimating){
        [displayLink invalidate];
        displayLink=nil;
        isAnimating=NO;
    }
}


-(BOOL)acceptsFirstResponder{
    return(YES);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event{
    
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr{
    if(!bIsLKeyPressed){
        gbLightPerFragment= true;
        gbLightPerVertex=false;
        bIsLKeyPressed = true;
    }else{
        gbLightPerFragment = false;
        gbLightPerVertex=true;
        bIsLKeyPressed = false;
    }
    [self drawView:nil];
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr{
    
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr{
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr{
    
}

- (void)dealloc{
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
    
    
    glDetachShader(shaderProgramObject, vertexShaderObject);
    glDetachShader(shaderProgramObject, fragmentShaderObject);
    glDeleteShader(vertexShaderObject);
    vertexShaderObject = 0;
    glDeleteShader(fragmentShaderObject);
    fragmentShaderObject = 0;
    
    glDeleteProgram(shaderProgramObject);
    shaderProgramObject = 0;
    
    if(depthRenderbuffer){
        glDeleteRenderbuffers(1,&depthRenderbuffer);
        depthRenderbuffer=0;
    }
    
    if(colorRenderbuffer){
        glDeleteRenderbuffers(1,&colorRenderbuffer);
        colorRenderbuffer=0;
    }
    
    if(defaultFramebuffer){
        glDeleteFramebuffers(1,&defaultFramebuffer);
        defaultFramebuffer=0;
    }
    
    if([EAGLContext currentContext]==eaglContext){
        [EAGLContext setCurrentContext:nil];
    }
    [eaglContext release];
    eaglContext=nil;
    
    [super dealloc];
}
-(void)update{
    gAngle=gAngle+1.0f;
    if (gAngle >= 360.0f){
        gAngle = 0.0f;
    }
}
@end
