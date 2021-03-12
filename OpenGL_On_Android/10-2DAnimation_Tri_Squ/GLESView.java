package com.astromedicomp.win_2danimation_tri_squ;

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
	private int[] vao_triangle=new int[1];
	private int[] vbo_triangle_position=new int[1];
	private int[] vbo_triangle_color=new int[1];
	private int[] vao_square=new int[1];
	private int[] vbo_square_position=new int[1];
	private int mvpUniform;
	private float tri_angle=0.0f;
	private float squ_angle=0.0f;

	private float perspectiveProjectionMatrix[]=new float[16];

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
		return (true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e){
		return (true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e){
		return (true);
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

		final String vertexShaderSourceCode=String.format
		(
			"#version 300 es"+
			"\n"+
			"in vec4 vPosition;"+
			"in vec4 vColor;"+
			"out vec4 out_color;"+
			"uniform mat4 u_mvp_matrix;"+
			"void main(void)"+
			"{"+
			"gl_Position=u_mvp_matrix*vPosition;"+
			"out_color=vColor;"+
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

		final String fragmentShaderSourceCode=String.format
		(
			"#version 300 es"+
			"\n"+
			"precision highp float;"+
			"in vec4 out_color;"+
			"out vec4 FragColor;"+
			"void main(void)"+
			"{"+
			"FragColor=out_color;"+
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

		mvpUniform=GLES30.glGetUniformLocation(shaderProgramObject,"u_mvp_matrix");

		final float triangleVertices[]=new float[]
		{
			-1.0f,-1.0f,0.0f,
			1.0f,-1.0f,0.0f,
			0.0f,1.0f,0.0f
		};

		final float triangleColors[]=new float[]
		{	
			1.0f,0.0f,0.0f,
			0.0f,1.0f,0.0f,
			0.0f,0.0f,1.0f
		};


		GLES30.glGenVertexArrays(1,vao_triangle,0);
		GLES30.glBindVertexArray(vao_triangle[0]);
		GLES30.glGenBuffers(1,vbo_triangle_position,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_triangle_position[0]);

		ByteBuffer byteBuffer=ByteBuffer.allocateDirect(triangleVertices.length*4);
									/*No sizeof() operator so Multiplied by 4*/
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
		verticesBuffer.put(triangleVertices);
		verticesBuffer.position(0);

		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,triangleVertices.length*4,
							verticesBuffer,GLES30.GL_STATIC_DRAW);

		GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,3,
									GLES30.GL_FLOAT,false,0,0);
		GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);

		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);


		GLES30.glGenBuffers(1,vbo_triangle_color ,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_triangle_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(triangleColors.length*4);//
									/*No sizeof() operator so Multiplied by 4*/
		byteBuffer.order(ByteOrder.nativeOrder());
		
		verticesBuffer=byteBuffer.asFloatBuffer();//
		verticesBuffer.put(triangleColors);
		verticesBuffer.position(0);

		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,triangleColors.length*4,
							verticesBuffer,GLES30.GL_STATIC_DRAW);

		GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_COLOR,3,
									GLES30.GL_FLOAT,false,0,0);
		GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_COLOR);

		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);		

		final float squareVertices[]=
		{	
			1.0f,1.0f,0.0f,
			-1.0f,1.0f,0.0f,
			-1.0f,-1.0f,0.0f,
			1.0f,-1.0f,0.0f
		};

		GLES30.glGenVertexArrays(1,vao_square,0);
		GLES30.glBindVertexArray(vao_square[0]);
		GLES30.glGenBuffers(1,vbo_square_position,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_square_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(squareVertices.length*4);
									/*No sizeof() operator so Multiplied by 4*/
		byteBuffer.order(ByteOrder.nativeOrder());
		verticesBuffer=byteBuffer.asFloatBuffer();
		verticesBuffer.put(squareVertices);
		verticesBuffer.position(0);

		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,squareVertices.length*4,
							verticesBuffer,GLES30.GL_STATIC_DRAW);

		GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,3,
									GLES30.GL_FLOAT,false,0,0);
		GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);

		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);

		GLES30.glVertexAttrib3f(GLESMacros.SSK_ATTRIBUTE_COLOR,0.61f,0.79f,0.66f);

		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);		

		GLES30.glBindVertexArray(0);

		GLES30.glEnable(GLES30.GL_DEPTH_TEST);
		GLES30.glDepthFunc(GLES30.GL_LEQUAL);
//		GLES30.glEnable(GLES30.GL_CULL_FACE);
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

		float modelViewMatrix[]=new float[16];
		float rotationMatrix[]=new float[16];
		float modelViewProjectionMatrix[]=new float[16];
		float resultMatrix[]=new float[16];

		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(rotationMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);

		Matrix.translateM(modelViewMatrix,0,-1.5f,0.0f,-6.0f);
		Matrix.rotateM(rotationMatrix,0,tri_angle,0.0f,1.0f,0.0f);

		Matrix.multiplyMM(resultMatrix,0,
							modelViewMatrix,0,
							rotationMatrix,0);

		Matrix.multiplyMM(modelViewProjectionMatrix,0,
							perspectiveProjectionMatrix,0,
							resultMatrix,0);

		GLES30.glUniformMatrix4fv(mvpUniform,1,false,
									modelViewProjectionMatrix,0);

		GLES30.glBindVertexArray(vao_triangle[0]);
			GLES30.glDrawArrays(GLES30.GL_TRIANGLES,0,3);
		GLES30.glBindVertexArray(0);

		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(rotationMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);
		Matrix.translateM(modelViewMatrix,0,1.5f,0.0f,-6.0f);
		Matrix.rotateM(rotationMatrix,0,squ_angle,1.0f,0.0f,0.0f);
		Matrix.multiplyMM(resultMatrix,0,
							modelViewMatrix,0,
							rotationMatrix,0);

		Matrix.multiplyMM(modelViewProjectionMatrix,0,
							perspectiveProjectionMatrix,0,
							resultMatrix,0);
		GLES30.glUniformMatrix4fv(mvpUniform,1,false,
									modelViewProjectionMatrix,0);

		GLES30.glBindVertexArray(vao_square[0]);
			GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,0,4);
		GLES30.glBindVertexArray(0);

		GLES30.glUseProgram(0);

		update();
		requestRender();
	}

	void uninitialize(){
		if(vao_triangle[0]!=0){
			GLES30.glDeleteVertexArrays(1,vao_triangle,0);
			vao_triangle[0]=0;
		}

		if(vao_square[0]!=0){
			GLES30.glDeleteVertexArrays(1,vao_square,0);
			vao_square[0]=0;
		}

		if(vbo_triangle_color[0]!=0){
			GLES30.glDeleteBuffers(1,vbo_triangle_color,0);
			vbo_triangle_color[0]=0;
		}

		if(vbo_triangle_position[0]!=0){
			GLES30.glDeleteBuffers(1,vbo_triangle_position,0);
			vbo_triangle_position[0]=0;
		}

		if(vbo_square_position[0]!=0){
			GLES30.glDeleteBuffers(1,vbo_square_position,0);
			vbo_square_position[0]=0;
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
	tri_angle=tri_angle+2.0f;
	if(tri_angle>=360.0f){
		tri_angle=tri_angle-360.0f;
	}

	squ_angle=squ_angle+2.0f;
	if(squ_angle>=360.0f){
		squ_angle=squ_angle-360.0f;
	}

}
}