package com.astromedicomp.win_2lights_pyramid;

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
import android.opengl.Matrix; 

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener,OnDoubleTapListener{

	private final Context context;
	private GestureDetector gestureDetector;
	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;
	private int[] vao_pyramid=new int[1];
	private int[] vbo_pyramid_position=new int[1];
	private int[] vbo_pyramid_normal=new int[1];
	
	private int La_uniform_one;
	private int Ld_uniform_one;
	private int Ls_uniform_one;
	private int light_position_uniform_one;
	private int Ka_uniform;
	private int Kd_uniform;
	private int Ks_uniform;
	private int La_uniform_two;
	private int Ld_uniform_two;
	private int Ls_uniform_two;
	private int light_position_uniform_two;
	private int material_shininess_uniform;

	private int doubleTap; 
	private int singleTap; 
	private int model_matrix_uniform,view_matrix_uniform,projection_matrix_uniform;
	private int L_KeyPressed_uniform;

	private float perspectiveProjectionMatrix[]=new float[16];
	private float one_lightAmbient[]={0.0f,0.0f,0.0f,1.0f};
	private float one_lightDiffuse[]={1.0f,0.0f,0.0f,0.0f};
	private float one_lightSpecular[]={1.0f,1.0f,1.0f,1.0f};
	private float one_lightPosition[]={100.0f,100.0f,100.0f,1.0f};
	private float two_lightAmbient[]={0.0f,0.0f,0.0f,1.0f};
	private float two_lightDiffuse[]={0.0f,0.0f,1.0f,0.0f};
	private float two_lightSpecular[]={1.0f,1.0f,1.0f,1.0f};
	private float two_lightPosition[]={-100.0f,100.0f,100.0f,1.0f};
	private float material_ambient[]={0.0f,0.0f,0.0f,1.0f};
	private float material_diffuse[]={1.0f,1.0f,1.0f,1.0f};
	private float material_specular[]={1.0f,1.0f,1.0f,1.0f};
	private float material_shininess=50.0f;

	private float gAngle=0.0f;
	public GLESView(Context drawingContext){
		super(drawingContext);
		context=drawingContext;

		setEGLContextClientVersion(3);
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);	//onscreen rendering pahije offscreen nahi

		gestureDetector=new GestureDetector(context,this,null,false);

		gestureDetector.setOnDoubleTapListener(this);
	}
	
	@Override
	public void onSurfaceCreated(GL10 gl,EGLConfig config){
		String glesVersion=gl.glGetString(GL10.GL_VERSION);
		System.out.println("SSK: OpenGL-ES version ="+glesVersion);

		String glslVersion=gl.glGetString(GLES30.GL_SHADING_LANGUAGE_VERSION);
		System.out.println("SSK: GLSL Version ="+glslVersion);

		initialize(gl);
	}

	@Override
	public void onSurfaceChanged(GL10 unused,int width,int height){
		resize(width,height);
	}

	@Override
	public void onDrawFrame(GL10 unused){
		display();
	}

	@Override
	public boolean onTouchEvent(MotionEvent e){
		int eventaction=e.getAction();
		if(!gestureDetector.onTouchEvent(e)){
			super.onTouchEvent(e);
		}
		return (true);
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
		return (true);
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
		return (true);
	}

	@Override
	public boolean onFling(MotionEvent e1,MotionEvent e2,float velocityX,float velocityY){
		return (true);
	}

	@Override
	public void onLongPress(MotionEvent e){

	}

	@Override
	public boolean onScroll(MotionEvent e1,MotionEvent e2,float distanceX,float distanceY){
		uninitialize();
		System.exit(0);
		return (true);
	}

	@Override
	public void onShowPress(MotionEvent e){

	}

	@Override
	public boolean onSingleTapUp(MotionEvent e){
		return (true);
	}

	private void initialize(GL10 gl){
		vertexShaderObject=GLES30.glCreateShader(GLES30.GL_VERTEX_SHADER);

		final String vertexShaderSourceCode=String.format(
			"#version 300 es"+
			"\n"+
			"in vec4 vPosition;"+
			"in vec3 vNormal;"+
			"uniform mat4 u_model_matrix;"+
			"uniform mat4 u_view_matrix;"+
			"uniform mat4 u_projection_matrix;"+
			"uniform vec4 u_light_position_one;"+
			"uniform vec4 u_light_position_two;"+
			"uniform int u_lighting_enabled;"+
			"out vec3 transformed_normals;"+
			"out vec3 light_direction_one;"+
			"out vec3 light_direction_two;"+
			"out vec3 viewer_vector;"+
			"void main(void)"+
			"{"+
			"if(u_lighting_enabled==1)"+
			"{"+
			"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;"+
			"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;"+
			"light_direction_one = vec3(u_light_position_one) - eye_coordinates.xyz;"+
			"light_direction_two = vec3(u_light_position_two) - eye_coordinates.xyz;"+
			"viewer_vector = -eye_coordinates.xyz;"+
			"}"+
			"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
			"}"
		);

		GLES30.glShaderSource(vertexShaderObject,vertexShaderSourceCode);
		GLES30.glCompileShader(vertexShaderObject);
		int[] iShaderCompiledStatus=new int[1];
		int[] iInfoLogLength=new int[1];
		String szInfoLog=null;

		GLES30.glGetShaderiv(vertexShaderObject,GLES30.GL_COMPILE_STATUS,
							iShaderCompiledStatus,0);

		if(iShaderCompiledStatus[0]==GLES30.GL_FALSE){
			GLES30.glGetShaderiv(vertexShaderObject,GLES30.GL_INFO_LOG_LENGTH,
								iInfoLogLength,0);
			if(iInfoLogLength[0]>0){
				szInfoLog=GLES30.glGetShaderInfoLog(vertexShaderObject);
				System.out.println("SSK: Vertex Shader Compilation Log ="+szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}

		fragmentShaderObject=GLES30.glCreateShader(GLES30.GL_FRAGMENT_SHADER);

		final String fragmentShaderSourceCode=String.format(
			"#version 300 es"+
			"\n"+
			"in vec3 transformed_normals;"+
			"in vec3 light_direction_one;"+
			"in vec3 light_direction_two;"+
			"in vec3 viewer_vector;"+
			"out vec4 FragColor;"+
			"uniform vec3 u_La_one;"+
			"uniform vec3 u_Ld_one;"+
			"uniform vec3 u_Ls_one;"+
			"uniform vec3 u_La_two;"+
			"uniform vec3 u_Ld_two;"+
			"uniform vec3 u_Ls_two;"+
			"uniform vec3 u_Ka;"+
			"uniform vec3 u_Kd;"+
			"uniform vec3 u_Ks;"+
			"uniform float u_material_shininess;"+
			"uniform int u_lighting_enabled;"+
			"void main(void)"+
			"{"+
				"vec3 phong_ads_color_one;"+
				"vec3 phong_ads_color_two;"+
				"if(u_lighting_enabled==1)"+
				"{"+
					"vec3 normalized_transformed_normals=normalize(transformed_normals);"+
					"vec3 normalized_light_direction_one=normalize(light_direction_one);"+
					"vec3 normalized_viewer_vector=normalize(viewer_vector);"+
					"vec3 ambient1 = u_La_one * u_Ka;"+
					"float tn_dot_ld1 = max(dot(normalized_transformed_normals, normalized_light_direction_one),0.0);"+
					"vec3 diffuse_one = u_Ld_one * u_Kd * tn_dot_ld1;"+
					"vec3 reflection_vector1 = reflect(-normalized_light_direction_one, normalized_transformed_normals);"+
					"vec3 specular_one = u_Ls_one * u_Ks * pow(max(dot(reflection_vector1, normalized_viewer_vector), 0.0), u_material_shininess);"+
					"phong_ads_color_one=ambient1 + diffuse_one + specular_one;"+
					"vec3 normalized_light_direction_two=normalize(light_direction_two);"+
					"vec3 ambient_two = u_La_two * u_Ka;"+
					"float tn_dot_ld2 = max(dot(normalized_transformed_normals, normalized_light_direction_two),0.0);"+
					"vec3 diffuse_two = u_Ld_two * u_Kd * tn_dot_ld2;"+
					"vec3 reflection_vector2 = reflect(-normalized_light_direction_two, normalized_transformed_normals);"+
					"vec3 specular_two = u_Ls_two * u_Ks * pow(max(dot(reflection_vector2, normalized_viewer_vector), 0.0), u_material_shininess);"+
					"phong_ads_color_two=ambient_two + diffuse_two + specular_two;"+
					"phong_ads_color_one = phong_ads_color_one + phong_ads_color_two;"+
				"}"+
				"else"+
				"{"+
					"phong_ads_color_one = vec3(1.0, 1.0, 1.0);"+
				"}"+
				"FragColor = vec4(phong_ads_color_one, 1.0);"+
			"}"
		);

		GLES30.glShaderSource(fragmentShaderObject,fragmentShaderSourceCode);
		GLES30.glCompileShader(fragmentShaderObject);
		iShaderCompiledStatus[0]=0; /*reintializing it to 0,,,A good discipline*/
		iInfoLogLength[0]=0; 		/*reintializing it to 0,,,A good discipline*/
		szInfoLog=null;				/*reintializing it to null,,,A good discipline*/
		GLES30.glGetShaderiv(fragmentShaderObject,GLES30.GL_COMPILE_STATUS,
							iShaderCompiledStatus,0);
		if(iShaderCompiledStatus[0]==GLES30.GL_FALSE){
			GLES30.glGetShaderiv(fragmentShaderObject,GLES30.GL_INFO_LOG_LENGTH,
								iInfoLogLength,0);
			if(iInfoLogLength[0]>0){
				szInfoLog=GLES30.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("SSK : Shader Program Link Log ="+szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		shaderProgramObject=GLES30.glCreateProgram();

		GLES30.glAttachShader(shaderProgramObject,vertexShaderObject);
		GLES30.glAttachShader(shaderProgramObject,fragmentShaderObject);
		GLES30.glBindAttribLocation(shaderProgramObject,GLESMacros.SSK_ATTRIBUTE_VERTEX,
									"vPosition");
		GLES30.glBindAttribLocation(shaderProgramObject,GLESMacros.SSK_ATTRIBUTE_COLOR,
									"vColor");

		GLES30.glLinkProgram(shaderProgramObject);
		int[] iShaderProgramLinkStatus=new int[1];
		iInfoLogLength[0]=0;/*reintializing it to 0,,,A good discipline*/
		szInfoLog=null;		/*reintializing it to null,,,A good discipline*/
		GLES30.glGetProgramiv(shaderProgramObject,GLES30.GL_LINK_STATUS,
								iShaderProgramLinkStatus,0);
		if(iShaderProgramLinkStatus[0]==GLES30.GL_FALSE){
			GLES30.glGetProgramiv(shaderProgramObject,GLES30.GL_INFO_LOG_LENGTH,
									iInfoLogLength,0);
			if(iInfoLogLength[0]>0){
				szInfoLog=GLES30.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("SSK: Shader Program Link Log"+szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}

		model_matrix_uniform=GLES30.glGetUniformLocation(shaderProgramObject,
												"u_model_matrix");
		view_matrix_uniform=GLES30.glGetUniformLocation(shaderProgramObject,
												"u_view_matrix");
		projection_matrix_uniform=GLES30.glGetUniformLocation(shaderProgramObject,
													"u_projection_matrix");

		L_KeyPressed_uniform=GLES30.glGetUniformLocation(shaderProgramObject,"u_lighting_enabled");
		La_uniform_one=GLES30.glGetUniformLocation(shaderProgramObject,"u_La_one");
		Ld_uniform_one=GLES30.glGetUniformLocation(shaderProgramObject,"u_Ld_one");
		Ls_uniform_one=GLES30.glGetUniformLocation(shaderProgramObject,"u_Ls_one");
		light_position_uniform_one=GLES30.glGetUniformLocation(shaderProgramObject,"u_light_position_one");

		Ka_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ka");
		Kd_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Kd");
		Ks_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ks");

		La_uniform_two=GLES30.glGetUniformLocation(shaderProgramObject,"u_La_two");
		Ld_uniform_two=GLES30.glGetUniformLocation(shaderProgramObject,"u_Ld_two");
		Ls_uniform_two=GLES30.glGetUniformLocation(shaderProgramObject,"u_Ls_two");

		light_position_uniform_two = GLES30.glGetUniformLocation(shaderProgramObject, "u_light_position_two");

		material_shininess_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_material_shininess");


		final float pyramidVertices[]=new float[]
		{	
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

		final float pyramidNormals[]=new float[]
		{	
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
		
		};

		GLES30.glGenVertexArrays(1,vao_pyramid,0);
		GLES30.glBindVertexArray(vao_pyramid[0]);
		GLES30.glGenBuffers(1,vbo_pyramid_position,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_pyramid_position[0]);

		ByteBuffer byteBuffer=ByteBuffer.allocateDirect(pyramidVertices.length*4);
									/*No sizeof() operator so Multiplied by 4*/
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
		verticesBuffer.put(pyramidVertices);
		verticesBuffer.position(0);

		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,pyramidVertices.length*4,
							verticesBuffer,GLES30.GL_STATIC_DRAW);

		GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,3,
									GLES30.GL_FLOAT,false,0,0);
		GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);

		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);


		GLES30.glGenBuffers(1,vbo_pyramid_normal ,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_pyramid_normal[0]);

			byteBuffer=ByteBuffer.allocateDirect(pyramidNormals.length*4);//
									/*No sizeof() operator so Multiplied by 4*/
			byteBuffer.order(ByteOrder.nativeOrder());
			verticesBuffer=byteBuffer.asFloatBuffer();//
			verticesBuffer.put(pyramidNormals);
			verticesBuffer.position(0);

			GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,pyramidNormals.length*4,
								verticesBuffer,GLES30.GL_STATIC_DRAW);

		GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_NORMAL,3,
									GLES30.GL_FLOAT,false,0,0);
		GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_NORMAL);

		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);		

		GLES30.glBindVertexArray(0);

		GLES30.glEnable(GLES30.GL_DEPTH_TEST);
		GLES30.glDepthFunc(GLES30.GL_LEQUAL);
		GLES30.glClearColor(0.0f,0.0f,0.0f,0.0f);
		Matrix.setIdentityM(perspectiveProjectionMatrix,0);
	}

	private void resize(int width,int height){
		GLES30.glViewport(0,0,width,height);

		if(width<=height){
			Matrix.perspectiveM(perspectiveProjectionMatrix,0,45.0f,(float)height/(float)width,0.1f,100.0f);
		}else{
			Matrix.perspectiveM(perspectiveProjectionMatrix,0,45.0f,(float)width/(float)height,0.1f,100.0f);
		}
	}

	public void display(){
		GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT |
						 GLES30.GL_STENCIL_BUFFER_BIT);
		
		GLES30.glUseProgram(shaderProgramObject);

		float modelMatrix[]=new float[16];
		float rotationMatrix[]=new float[16];
		float viewMatrix[]=new float[16];

		Matrix.setIdentityM(modelMatrix,0);
		Matrix.setIdentityM(rotationMatrix,0);
		Matrix.setIdentityM(viewMatrix,0);

		if(doubleTap==1){
				//set 'u_lighting_enabled' uniform
			GLES30.glUniform1i(L_KeyPressed_uniform, 1);
				//setting light's properties
			GLES30.glUniform3fv(La_uniform_one, 1, one_lightAmbient,0);
			GLES30.glUniform3fv(Ld_uniform_one, 1, one_lightDiffuse,0);
			GLES30.glUniform3fv(Ls_uniform_one, 1, one_lightSpecular,0);
			GLES30.glUniform4fv(light_position_uniform_one, 1, one_lightPosition,0);
				// setting material's properties

			GLES30.glUniform3fv(La_uniform_two, 1, two_lightAmbient,0);
			GLES30.glUniform3fv(Ld_uniform_two, 1, two_lightDiffuse,0);
			GLES30.glUniform3fv(Ls_uniform_two, 1, two_lightSpecular,0);
			GLES30.glUniform4fv(light_position_uniform_two, 1, two_lightPosition,0);

			GLES30.glUniform3fv(Ka_uniform, 1, material_ambient,0);
			GLES30.glUniform3fv(Kd_uniform, 1, material_diffuse,0);
			GLES30.glUniform3fv(Ks_uniform, 1, material_specular,0);
			GLES30.glUniform1f(material_shininess_uniform, material_shininess);

		}else{
			GLES30.glUniform1i(L_KeyPressed_uniform, 0);
		}
		

		Matrix.translateM(modelMatrix,0,0.0f,0.0f,-6.0f);
		Matrix.rotateM(rotationMatrix,0,gAngle,0.0f,1.0f,0.0f);

		Matrix.multiplyMM(modelMatrix,0,
							modelMatrix,0,
							rotationMatrix,0);

		GLES30.glUniformMatrix4fv(model_matrix_uniform,1,false,
									modelMatrix,0);
		GLES30.glUniformMatrix4fv(view_matrix_uniform,1,false,
									viewMatrix,0);
		GLES30.glUniformMatrix4fv(projection_matrix_uniform,1,false,
									perspectiveProjectionMatrix,0);

		GLES30.glBindVertexArray(vao_pyramid[0]);
			GLES30.glDrawArrays(GLES30.GL_TRIANGLES,0,12);
		GLES30.glBindVertexArray(0);


		GLES30.glUseProgram(0);

		update();
		requestRender();
	}

	void uninitialize(){
		if(vao_pyramid[0]!=0){
			GLES30.glDeleteVertexArrays(1,vao_pyramid,0);
			vao_pyramid[0]=0;
		}

		if(vbo_pyramid_normal[0]!=0){
			GLES30.glDeleteBuffers(1,vbo_pyramid_normal,0);
			vbo_pyramid_normal[0]=0;
		}

		if(vbo_pyramid_position[0]!=0){
			GLES30.glDeleteBuffers(1,vbo_pyramid_position,0);
			vbo_pyramid_position[0]=0;
		}

		if(shaderProgramObject!=0){
			if(vertexShaderObject!=0){
				GLES30.glDetachShader(shaderProgramObject,vertexShaderObject);
				GLES30.glDeleteShader(vertexShaderObject);
				vertexShaderObject=0;
			}
			if(fragmentShaderObject!=0){
				GLES30.glDetachShader(shaderProgramObject,fragmentShaderObject);
				GLES30.glDeleteShader(fragmentShaderObject);
				fragmentShaderObject=0;
			}
		}

		if(shaderProgramObject!=0){
			GLES30.glDeleteProgram(shaderProgramObject);
			shaderProgramObject=0;
		}
	}

	private void update(){
	gAngle=gAngle+2.0f;
	if(gAngle>=360.0f){
		gAngle=gAngle-360.0f;
	}

}
}