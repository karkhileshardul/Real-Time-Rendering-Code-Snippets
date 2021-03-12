package com.astromedicomp.win_3lights_sphere;
import android.content.Context; 
import android.opengl.GLSurfaceView; 
import javax.microedition.khronos.opengles.GL10; 
import javax.microedition.khronos.egl.EGLConfig; 
import android.opengl.GLES30;
import android.view.MotionEvent;
import android.view.GestureDetector; 
import android.view.GestureDetector.OnGestureListener; 
import android.view.GestureDetector.OnDoubleTapListener; 
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import android.opengl.Matrix; 

public class GLESView extends GLSurfaceView
                     implements GLSurfaceView.Renderer, 
                     OnGestureListener, OnDoubleTapListener{
    private final Context context;
    private GestureDetector gestureDetector;
    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
    private int numElements;
    private int numVertices;   
    private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
    private float light_ambient_red[] = {0.0f,0.0f,0.0f,1.0f};
    private float light_diffuse_red[] = {1.0f,0.0f,0.0f,1.0f};
    private float light_specular_red[] = {1.0f,0.0f,0.0f,1.0f};
    private float light_position_red[] = { -2.0f,0.0f,1.0f,1.0f };
	private float light_ambient_green[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	private float light_diffuse_green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	private float light_specular_green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	private float light_position_green[] = { 2.0f, 0.0f, 1.0f, 0.0f };
	private float light_ambient_blue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	private float light_diffuse_blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	private float light_specular_blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	private float light_position_blue[] = { 0.0f, 0.0f, 1.0f, 0.0f };
    private float material_ambient[] = {0.0f,0.0f,0.0f,1.0f};
    private float material_diffuse[] = {1.0f,1.0f,1.0f,1.0f};
    private float material_specular[] = {1.0f,1.0f,1.0f,1.0f};
    private float material_shininess = 50.0f;
    private int  modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;
    private int  laUniformRed, ldUniformRed, lsUniformRed, lightPositionUniformRed;
    private int  laUniformGreen, ldUniformGreen, lsUniformGreen, lightPositionUniformGreen;
    private int  laUniformBlue, ldUniformBlue, lsUniformBlue, lightPositionUniformBlue;
    private int  kaUniform, kdUniform, ksUniform, materialShininessUniform;
    private int doubleTapUniform;
   	private int singleTapUniform;
    private int u_perVertex, u_perFragment;
    private float perspectiveProjectionMatrix[]=new float[16];     
    private int doubleTap; 
	private int singleTap;
	private float gLightAngleY = 0.0f;
    
    public GLESView(Context drawingContext){
        super(drawingContext);
        context=drawingContext;
        setEGLContextClientVersion(3);
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        
        gestureDetector = new GestureDetector(context, this, null, false);
        gestureDetector.setOnDoubleTapListener(this);
    }
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config){
        String glesVersion = gl.glGetString(GL10.GL_VERSION);
        System.out.println("SSK: OpenGL-ES Version = "+glesVersion);
        String glslVersion=gl.glGetString(GLES30.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("SSK: GLSL Version = "+glslVersion);
        initialize(gl);
    }
 
    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height){
        resize(width, height);
    }
    @Override
    public void onDrawFrame(GL10 unused){
        display();
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent e){
        int eventaction = e.getAction();
        if(!gestureDetector.onTouchEvent(e))
            super.onTouchEvent(e);
        return(true);
    }
    @Override
    public boolean onDoubleTap(MotionEvent e){
        doubleTap++;
        if(doubleTap > 1){
            doubleTap=0;
        }
        return(true);
    }
    
    @Override
    public boolean onDoubleTapEvent(MotionEvent e){
        return(true);
    }    
    @Override
    public boolean onSingleTapConfirmed(MotionEvent e){
   		singleTap++;
        if(singleTap > 2)
            singleTap=0;
        return(true);

    }
    @Override
    public boolean onDown(MotionEvent e){
        return(true);
    }
    
    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2,
                            float velocityX, float velocityY){
        return(true);
    }
    
    @Override
    public void onLongPress(MotionEvent e){
    }
    
    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2,
                            float distanceX, float distanceY){
        uninitialize();
        System.exit(0);
        return(true);
    }
    
    @Override
    public void onShowPress(MotionEvent e){
    }
    
    @Override
    public boolean onSingleTapUp(MotionEvent e){
        return(true);
    }

    private void initialize(GL10 gl){
        vertexShaderObject=GLES30.glCreateShader(GLES30.GL_VERTEX_SHADER);
        
        final String vertexShaderSourceCode =String.format(
        "#version 300 es"+
        "\n"+
        "in vec4 vPosition;"+
        "in vec3 vNormal;"+
        "uniform mat4 u_model_matrix;"+
        "uniform mat4 u_view_matrix;"+
        "uniform mat4 u_projection_matrix;"+
        "uniform int u_double_tap;"+
        "uniform vec3 u_La_Red, u_La_Green,u_La_Blue;"+
        "uniform vec3 u_Ld_Red, u_Ld_Green,u_Ld_Blue;"+
        "uniform vec3 u_Ls_Red, u_Ls_Green,u_Ls_Blue;"+
        "uniform vec4 u_light_position_red, u_light_position_green, u_light_position_blue;"+
        "uniform vec3 u_Ka;"+
        "uniform vec3 u_Kd;"+
        "uniform vec3 u_Ks;"+
        "uniform float u_material_shininess;"+
		"uniform int u_perVertex, u_perFragment;"+
		"out vec3 out_phong_ads_color;"+
		"out vec3 light_direction_red, light_direction_green, light_direction_blue;"+
        "out vec3 transformed_normals;"+
        "out vec3 viewer_vector;"+
        "vec3 calculateLight(vec3 u_La, vec3 u_Ld, vec3 u_Ls, vec4 u_light_position)"+
		"{"+
			"vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;"+
			"vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
			"vec3 light_direction = normalize(vec3(u_light_position) - eyeCoordinates.xyz);"+
			"float tn_dot_ld = max(dot(transformed_normals, light_direction), 0.0);"+
			"vec3 ambient = u_La * u_Kd;"+
			"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
			"vec3 reflection_vector = reflect(-light_direction, transformed_normals);"+
			"vec3 viewer_vector = normalize(-eyeCoordinates.xyz);"+
			"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);"+
			"vec3 phong_ads_color = ambient + diffuse + specular;"+
			"return phong_ads_color;"+
		"}"+
        "void main(void)"+
        "{"+
        	"if (u_double_tap == 1)"+
        	"{"+
				"if(u_perVertex == 1)"+
				"{"+
					"vec3 lightRed = calculateLight(u_La_Red, u_Ld_Red, u_Ls_Red, u_light_position_red);"+
					"vec3 lightGreen = calculateLight(u_La_Green, u_Ld_Green, u_Ls_Green, u_light_position_green);"+
					"vec3 lightBlue = calculateLight(u_La_Blue, u_Ld_Blue, u_Ls_Blue, u_light_position_blue);"+
					"out_phong_ads_color = lightRed + lightGreen + lightBlue;"+
				"}"+
				"else if(u_perFragment == 1)"+
				"{"+
					"vec4 eyeCoordinates=u_view_matrix * u_model_matrix * vPosition;"+
					"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;"+
					"light_direction_red = vec3(u_light_position_red) - eyeCoordinates.xyz;"+
					"light_direction_green = vec3(u_light_position_green) - eyeCoordinates.xyz;"+
					"light_direction_blue = vec3(u_light_position_blue) - eyeCoordinates.xyz;"+
					"viewer_vector = -eyeCoordinates.xyz;"+
				"}"+
        	"}"+
        	"else"+
        	"{"+
	    		"out_phong_ads_color = vec3(1.0, 1.0, 1.0);"+
        	"}"+
        	"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
        "}"
        );

        
        GLES30.glShaderSource(vertexShaderObject,vertexShaderSourceCode);
        GLES30.glCompileShader(vertexShaderObject);
        int[] iShaderCompiledStatus = new int[1];
        int[] iInfoLogLength = new int[1];
        String szInfoLog=null;
        GLES30.glGetShaderiv(vertexShaderObject,GLES30.GL_COMPILE_STATUS,
                            iShaderCompiledStatus,0);
        if (iShaderCompiledStatus[0] == GLES30.GL_FALSE){
            GLES30.glGetShaderiv(vertexShaderObject, 
                                GLES30.GL_INFO_LOG_LENGTH,
                                iInfoLogLength, 0);
            if (iInfoLogLength[0]>0){
                szInfoLog = GLES30.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("SSK: Vertex Shader Compilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
           }
        }

        fragmentShaderObject=GLES30.glCreateShader(GLES30.GL_FRAGMENT_SHADER);

        final String fragmentShaderSourceCode =String.format(
         "#version 300 es"+
         "\n"+
         "precision highp int;"+
         "uniform vec3 u_La_Red, u_La_Green,u_La_Blue;"+
         "uniform vec3 u_Ld_Red, u_Ld_Green,u_Ld_Blue;"+
         "uniform vec3 u_Ls_Red, u_Ls_Green,u_Ls_Blue;"+
         "uniform vec4 u_light_position_red, u_light_position_green, u_light_position_blue;"+
         "uniform vec3 u_Ka;"+
         "uniform vec3 u_Kd;"+
         "uniform vec3 u_Ks;"+
         "uniform float u_material_shininess;"+
         "uniform int u_double_tap;"+
         "uniform int u_perVertex, u_perFragment;"+
         "in vec3 transformed_normals;"+
         "in vec3 light_direction_red, light_direction_green, light_direction_blue;"+
         "in vec3 viewer_vector;"+
         "in vec3 out_phong_ads_color;"+
         "out vec4 FragColor;"+
         "vec3 calculateLight(vec3 u_La, vec3 u_Ld, vec3 u_Ls, vec3 light_direction)"+
		 "{"+
		 	"vec3 phong_ads_color;"+
		 	"vec3 normalized_transformed_normals=normalize(transformed_normals);"+
		 	"vec3 normalized_light_direction=normalize(light_direction);"+
		 	"vec3 normalized_viewer_vector=normalize(viewer_vector);"+
		 	"vec3 ambient = u_La * u_Ka;"+
		 	"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);"+
		 	"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
		 	"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);"+
		 	"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);"+
		 	"phong_ads_color=ambient + diffuse + specular;"+
		 	"return phong_ads_color;"+
		 "}"+
         "void main(void)"+
         "{"+
		 	"if (u_double_tap == 1)"+
			"{"+
				"if(u_perVertex == 1)"+
				"{"+
					"FragColor = vec4(out_phong_ads_color, 1.0);"+
				"}"+
				"else if(u_perFragment == 1)"+
				"{"+
					"vec3 lightRed = calculateLight(u_La_Red, u_Ld_Red, u_Ls_Red, light_direction_red);"+
					"vec3 lightGreen = calculateLight(u_La_Green, u_Ld_Green, u_Ls_Green, light_direction_green);"+
					"vec3 lightBlue = calculateLight(u_La_Blue, u_Ld_Blue, u_Ls_Blue, light_direction_blue);"+
					"vec3 phong_ads_color = lightRed + lightGreen + lightBlue;"+
					"FragColor = vec4(phong_ads_color, 1.0f);"+
				"}"+
				"else"+
				"{"+
					"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"+
				"}"+
		    "}"+
			"else"+
			"{"+
				"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"+
			"}"+
         "}"
        );
        
        GLES30.glShaderSource(fragmentShaderObject,fragmentShaderSourceCode);
        GLES30.glCompileShader(fragmentShaderObject);
        iShaderCompiledStatus[0]=0; 
        iInfoLogLength[0]=0; 
        szInfoLog=null;
        GLES30.glGetShaderiv(fragmentShaderObject,
                            GLES30.GL_COMPILE_STATUS,
                            iShaderCompiledStatus, 0);
        if (iShaderCompiledStatus[0] == GLES30.GL_FALSE){
            GLES30.glGetShaderiv(fragmentShaderObject,
                                GLES30.GL_INFO_LOG_LENGTH,
                                iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0){
                szInfoLog = GLES30.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("SSK: Fragment Shader Compilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
        
        shaderProgramObject=GLES30.glCreateProgram();
        
        GLES30.glAttachShader(shaderProgramObject,vertexShaderObject);   
        GLES30.glAttachShader(shaderProgramObject,fragmentShaderObject);
        GLES30.glBindAttribLocation(shaderProgramObject,
                                    GLESMacros.SSK_ATTRIBUTE_VERTEX,
                                    "vPosition");
        GLES30.glBindAttribLocation(shaderProgramObject,
                                    GLESMacros.SSK_ATTRIBUTE_NORMAL,
                                    "vNormal");

        GLES30.glLinkProgram(shaderProgramObject);
        int[] iShaderProgramLinkStatus = new int[1];
        iInfoLogLength[0] = 0; 
        szInfoLog=null; 
        GLES30.glGetProgramiv(shaderProgramObject,
                            GLES30.GL_LINK_STATUS,
                            iShaderProgramLinkStatus,0);
        if (iShaderProgramLinkStatus[0] == GLES30.GL_FALSE){
            GLES30.glGetProgramiv(shaderProgramObject,
                                GLES30.GL_INFO_LOG_LENGTH,
                                iInfoLogLength, 0);
            if (iInfoLogLength[0]>0){
                szInfoLog = GLES30.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("SSK: Shader Program Link Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        modelMatrixUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_model_matrix");
        viewMatrixUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_view_matrix");
        projectionMatrixUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
        
        doubleTapUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_double_tap");
        singleTapUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_single_tap");
		u_perVertex =GLES30.glGetUniformLocation(shaderProgramObject, "u_perVertex");
		u_perFragment =GLES30.glGetUniformLocation(shaderProgramObject, "u_perFragment");
        
        
        laUniformRed = GLES30.glGetUniformLocation(shaderProgramObject, "u_La_Red");
        ldUniformRed = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ld_Red");
        lsUniformRed = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ls_Red");
        lightPositionUniformRed = GLES30.glGetUniformLocation(shaderProgramObject, "u_light_position_red");;

		laUniformGreen = GLES30.glGetUniformLocation(shaderProgramObject, "u_La_Green");
        ldUniformGreen = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ld_Green");
        lsUniformGreen = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ls_Green");
        lightPositionUniformGreen = GLES30.glGetUniformLocation(shaderProgramObject, "u_light_position_green");;

		laUniformBlue = GLES30.glGetUniformLocation(shaderProgramObject, "u_La_Blue");
        ldUniformBlue = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ld_Blue");
        lsUniformBlue = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ls_Blue");
        lightPositionUniformBlue = GLES30.glGetUniformLocation(shaderProgramObject, "u_light_position_blue");;


        kaUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ka");
        kdUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Kd");
        ksUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ks");
        materialShininessUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_material_shininess");
        
        Sphere sphere=new Sphere();
        float sphere_vertices[]=new float[1146];
        float sphere_normals[]=new float[1146];
        float sphere_textures[]=new float[764];
        short sphere_elements[]=new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();


        GLES30.glGenVertexArrays(1,vao_sphere,0);
        GLES30.glBindVertexArray(vao_sphere[0]);
        
        GLES30.glGenBuffers(1,vbo_sphere_position,0);
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
        
        ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                            sphere_vertices.length * 4,
                            verticesBuffer,
                            GLES30.GL_STATIC_DRAW);
        
        GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,
                                     3,
                                     GLES30.GL_FLOAT,
                                     false,0,0);
        
        GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);
        
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
        
        GLES30.glGenBuffers(1,vbo_sphere_normal,0);
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                            sphere_normals.length * 4,
                            verticesBuffer,
                            GLES30.GL_STATIC_DRAW);
        
        GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_NORMAL,
                                     3,
                                     GLES30.GL_FLOAT,
                                     false,0,0);
        
        GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_NORMAL);
        
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
        
        // element vbo
        GLES30.glGenBuffers(1,vbo_sphere_element,0);
        GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES30.glBufferData(GLES30.GL_ELEMENT_ARRAY_BUFFER,
                            sphere_elements.length * 2,
                            elementsBuffer,
                            GLES30.GL_STATIC_DRAW);
        
        GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER,0);

        GLES30.glBindVertexArray(0);

        GLES30.glEnable(GLES30.GL_DEPTH_TEST);
        GLES30.glDepthFunc(GLES30.GL_LEQUAL);
        GLES30.glEnable(GLES30.GL_CULL_FACE);
        GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        doubleTap=0;
        Matrix.setIdentityM(perspectiveProjectionMatrix,0);
    }
    
    private void resize(int width, int height){
        GLES30.glViewport(0, 0, width, height);
        Matrix.perspectiveM(perspectiveProjectionMatrix,0,
                            45.0f,(float)width/(float)height,
                            0.1f,100.0f); 
    }
    
    public void display(){
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT |
                        GLES30.GL_DEPTH_BUFFER_BIT |
                        GLES30.GL_STENCIL_BUFFER_BIT);

		float lightYRotateZAxis = (float) (Math.cos(3.1415 * gLightAngleY / 180.0) * 10.0);
		float lightYRotateXAxis = (float) (Math.sin(3.1415 * gLightAngleY / 180.0) * 10.0);

		float lightXRotateZAxis = (float) (Math.cos(3.1415 * gLightAngleY / 180.0) * 10.0);
		float lightXRotateYAxis = (float) (Math.sin(3.1415 * gLightAngleY / 180.0) * 10.0);

		float lightZRotateYAxis = (float) (Math.cos(3.1415 * gLightAngleY / 180.0) * 10.0);
		float lightZRotateXAxis = (float) (Math.sin(3.1415 * gLightAngleY / 180.0) * 10.0);
		
       	GLES30.glUseProgram(shaderProgramObject);

        	if(doubleTap==1){
            	GLES30.glUniform1i(doubleTapUniform, 1);

				if (singleTap==1) {
					GLES30.glUniform1i(u_perVertex, 1);
					GLES30.glUniform1i(u_perFragment, 0);	
				}else if (singleTap==2) {
					GLES30.glUniform1i(u_perFragment, 1);
					GLES30.glUniform1i(u_perVertex, 0);
				}

				light_position_red[0] = 0.0f;
				light_position_red[1] = lightXRotateYAxis;
				light_position_red[2] = lightXRotateZAxis;
				light_position_red[3] = 1.0f;

				light_position_green[0] = lightYRotateXAxis;
				light_position_green[1] = 0.0f;
				light_position_green[2] = lightYRotateZAxis;
				light_position_green[3] = 1.0f;

				light_position_blue[0] = lightZRotateXAxis;
				light_position_blue[1] = lightZRotateYAxis;
				light_position_blue[2] = 0.0f;
				light_position_blue[3] = 1.0f;
            
            	GLES30.glUniform3fv(laUniformRed, 1, light_ambient_red, 0);
            	GLES30.glUniform3fv(ldUniformRed, 1, light_diffuse_red, 0);
            	GLES30.glUniform3fv(lsUniformRed, 1, light_specular_red, 0);
    	        GLES30.glUniform4fv(lightPositionUniformRed, 1, light_position_red, 0);

				GLES30.glUniform3fv(laUniformGreen, 1, light_ambient_green, 0);
            	GLES30.glUniform3fv(ldUniformGreen, 1, light_diffuse_green, 0);
            	GLES30.glUniform3fv(lsUniformGreen, 1, light_specular_green, 0);
	            GLES30.glUniform4fv(lightPositionUniformGreen, 1, light_position_green, 0);

				GLES30.glUniform3fv(laUniformBlue, 1, light_ambient_blue, 0);
            	GLES30.glUniform3fv(ldUniformBlue, 1, light_diffuse_blue, 0);
            	GLES30.glUniform3fv(lsUniformBlue, 1, light_specular_blue, 0);
            	GLES30.glUniform4fv(lightPositionUniformBlue, 1, light_position_blue, 0);
	            
            	GLES30.glUniform3fv(kaUniform, 1, material_ambient, 0);
            	GLES30.glUniform3fv(kdUniform, 1, material_diffuse, 0);
            	GLES30.glUniform3fv(ksUniform, 1, material_specular, 0);
	            GLES30.glUniform1f(materialShininessUniform, material_shininess);
        	}
        	else{
            	GLES30.glUniform1i(doubleTapUniform, 0);
        	}
        
        	float modelMatrix[]=new float[16];
        	float viewMatrix[]=new float[16];
	        
        	Matrix.setIdentityM(modelMatrix,0);
        	Matrix.setIdentityM(viewMatrix,0);
	        Matrix.translateM(modelMatrix,0,0.0f,0.0f,-1.5f);
        
        	GLES30.glUniformMatrix4fv(modelMatrixUniform,1,false,modelMatrix,0);
        	GLES30.glUniformMatrix4fv(viewMatrixUniform,1,false,viewMatrix,0);
	        GLES30.glUniformMatrix4fv(projectionMatrixUniform,1,false,perspectiveProjectionMatrix,0);
        
        	GLES30.glBindVertexArray(vao_sphere[0]);
                	GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
    			    GLES30.glDrawElements(GLES30.GL_TRIANGLES, numElements, GLES30.GL_UNSIGNED_SHORT, 0);
            GLES30.glBindVertexArray(0);
        
        GLES30.glUseProgram(0);

		gLightAngleY = gLightAngleY + 1.0f;

		if (gLightAngleY >= 360.0f){
			gLightAngleY = gLightAngleY - 360.0f;
		}

        requestRender();
    }
    
    void uninitialize(){
        if(vao_sphere[0] != 0)
        {
            GLES30.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0]=0;
        }
        
        // destroy position vbo
        if(vbo_sphere_position[0] != 0)
        {
            GLES30.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0]=0;
        }
        
        // destroy normal vbo
        if(vbo_sphere_normal[0] != 0)
        {
            GLES30.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0]=0;
        }
        
        // destroy element vbo
        if(vbo_sphere_element[0] != 0)
        {
            GLES30.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0]=0;
        }

        if(shaderProgramObject != 0){
            if(vertexShaderObject != 0){
                GLES30.glDetachShader(shaderProgramObject, vertexShaderObject);
                GLES30.glDeleteShader(vertexShaderObject);
                vertexShaderObject = 0;
            }
            
            if(fragmentShaderObject != 0){
                GLES30.glDetachShader(shaderProgramObject, fragmentShaderObject);
                GLES30.glDeleteShader(fragmentShaderObject);
                fragmentShaderObject = 0;
            }
        }
        if(shaderProgramObject != 0){
            GLES30.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }        
    }
}
