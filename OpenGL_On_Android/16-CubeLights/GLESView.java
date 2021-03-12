package com.astromedicomp.win_cube_light;
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

public class GLESView extends GLSurfaceView
                     implements GLSurfaceView.Renderer, 
                     OnGestureListener, OnDoubleTapListener{
    private final Context context;
    
    private GestureDetector gestureDetector;
    
    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;

    private int[] vao_cube = new int[1];
    private int[] vbo_cube_position = new int[1];
    private int[] vbo_cube_normal = new int[1];
    
    private int  modelViewMatrixUniform, projectionMatrixUniform;
    private int  ldUniform, kdUniform, lightPositionUniform;
    
    private int doubleTapUniform;

    private float perspectiveProjectionMatrix[]=new float[16]; // 4x4 matrix
    
    private float angleCube= 0.0f;    
    private int singleTap; // for animation
    private int doubleTap; // for lights

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
        if(singleTap > 1){
            singleTap=0;
        }
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
         "uniform mat4 u_model_view_matrix;"+
         "uniform mat4 u_projection_matrix;"+
         "uniform mediump int u_double_tap;"+
         "uniform vec3 u_Ld;"+
         "uniform vec3 u_Kd;"+
         "uniform vec4 u_light_position;"+
         "out vec3 diffuse_light;"+
         "void main(void)"+
         "{"+
         "if (u_double_tap == 1)"+
         "{"+
         "vec4 eyeCoordinates = u_model_view_matrix * vPosition;"+
         "vec3 tnorm = normalize(mat3(u_model_view_matrix) * vNormal);"+
         "vec3 s = normalize(vec3(u_light_position - eyeCoordinates));"+
         "diffuse_light = u_Ld * u_Kd * max(dot(s, tnorm), 0.0);"+
         "}"+
         "gl_Position = u_projection_matrix * u_model_view_matrix * vPosition;"+
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
         "precision highp float;"+
         "in vec3 diffuse_light;"+
         "out vec4 FragColor;"+
         "uniform int u_double_tap;"+
         "void main(void)"+
         "{"+
         "vec4 color;"+
         "if (u_double_tap == 1)"+
         "{"+
         "color = vec4(diffuse_light,1.0);"+
         "}"+
         "else"+
         "{"+
         "color = vec4(1.0, 1.0, 1.0, 1.0);"+
         "}"+
         "FragColor = color;"+
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

        modelViewMatrixUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_model_view_matrix");
        projectionMatrixUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
        doubleTapUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_double_tap");        
        ldUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ld");
        kdUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Kd");
        lightPositionUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_light_position");;

        float cubeVertices[]= new float[]{
            //top 
            1.0f, 1.0f,-1.0f,  // top-right
            -1.0f, 1.0f,-1.0f, // top-left 
            -1.0f, 1.0f, 1.0f, // bottom-left
            1.0f, 1.0f, 1.0f,  // bottom-right
            
            //bottom
            1.0f,-1.0f, 1.0f,  // top-right
            -1.0f,-1.0f, 1.0f, // top-left 
            -1.0f,-1.0f,-1.0f, // bottom-left 
            1.0f,-1.0f,-1.0f,  // bottom-right
            
            //front 
            1.0f, 1.0f, 1.0f,  // top-right
            -1.0f, 1.0f, 1.0f, // top-left 
            -1.0f,-1.0f, 1.0f, // bottom-left 
            1.0f,-1.0f, 1.0f,  // bottom-right
            
            //back 
            1.0f,-1.0f,-1.0f,  // top-right
            -1.0f,-1.0f,-1.0f, // top-left 
            -1.0f, 1.0f,-1.0f, // bottom-left 
            1.0f, 1.0f,-1.0f,  // bottom-right
            
            //left 
            -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, 1.0f,-1.0f, // top-left 
            -1.0f,-1.0f,-1.0f, // bottom-left 
            -1.0f,-1.0f, 1.0f, // bottom-right
            
            //right 
            1.0f, 1.0f,-1.0f,  // top-right 
            1.0f, 1.0f, 1.0f,  // top-left 
            1.0f,-1.0f, 1.0f,  // bottom-left 
            1.0f,-1.0f,-1.0f,  // bottom-right
        };
        
        for(int i=0;i<72;i++)
        {
            if(cubeVertices[i]<0.0f)
                cubeVertices[i]=cubeVertices[i]+0.25f;
            else if(cubeVertices[i]>0.0f)
                cubeVertices[i]=cubeVertices[i]-0.25f;
            else
                cubeVertices[i]=cubeVertices[i]; // no change
        }
        
        float cubeNormals[]= new float[]{
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f
        };

        GLES30.glGenVertexArrays(1,vao_cube,0);
        GLES30.glBindVertexArray(vao_cube[0]);       
        	GLES30.glGenBuffers(1,vbo_cube_position,0);
        	GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_cube_position[0]);        
	        ByteBuffer byteBuffer=ByteBuffer.allocateDirect(cubeVertices.length * 4);
        	byteBuffer.order(ByteOrder.nativeOrder());
        	FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        	verticesBuffer.put(cubeVertices);
        	verticesBuffer.position(0);	        
        	GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                            	cubeVertices.length * 4,
                            	verticesBuffer,GLES30.GL_STATIC_DRAW);	        
        	GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,
                                     	3,GLES30.GL_FLOAT,
                                     	false,0,0);
        	GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);
        
        	GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
        	GLES30.glGenBuffers(1,vbo_cube_normal,0);
        		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_cube_normal[0]);
	        	
		        byteBuffer=ByteBuffer.allocateDirect(cubeNormals.length * 4);
        		byteBuffer.order(ByteOrder.nativeOrder());
        		verticesBuffer=byteBuffer.asFloatBuffer();
        		verticesBuffer.put(cubeNormals);
        		verticesBuffer.position(0);
	       		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                            		cubeNormals.length * 4,
	            	                verticesBuffer,GLES30.GL_STATIC_DRAW);
    	    	GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_NORMAL,
                    	                 	3,GLES30.GL_FLOAT,
        	    	                         false,0,0);
    	    	GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_NORMAL);
	        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);        
        GLES30.glBindVertexArray(0);

        GLES30.glEnable(GLES30.GL_DEPTH_TEST);
        GLES30.glDepthFunc(GLES30.GL_LEQUAL);
        GLES30.glEnable(GLES30.GL_CULL_FACE);
        GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        singleTap=0;
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
        
        GLES30.glUseProgram(shaderProgramObject);
        
        if(doubleTap==1){
            GLES30.glUniform1i(doubleTapUniform, 1);
            GLES30.glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f); 
            GLES30.glUniform3f(kdUniform, 0.5f, 0.5f, 0.5f); 
            float[] lightPosition = {0.0f, 0.0f, 2.0f, 1.0f};
            GLES30.glUniform4fv(lightPositionUniform, 1, lightPosition,0); // light position
        }else{
            GLES30.glUniform1i(doubleTapUniform, 0);
        }
        
        float modelMatrix[]=new float[16];
        float modelViewMatrix[]=new float[16];
        float rotationMatrix[]=new float[16];
        
        Matrix.setIdentityM(modelMatrix,0);
        Matrix.setIdentityM(modelViewMatrix,0);
        Matrix.setIdentityM(rotationMatrix,0);

        Matrix.translateM(modelMatrix,0,0.0f,0.0f,-5.0f);        
        Matrix.setRotateM(rotationMatrix,0,angleCube,1.0f,1.0f,1.0f);
        Matrix.multiplyMM(modelViewMatrix,0,modelMatrix,0,rotationMatrix,0);
        
        GLES30.glUniformMatrix4fv(modelViewMatrixUniform,1,false,modelViewMatrix,0);
        GLES30.glUniformMatrix4fv(projectionMatrixUniform,1,false,perspectiveProjectionMatrix,0);
        
        GLES30.glBindVertexArray(vao_cube[0]);
        	GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,0,4);
        	GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,4,4);
        	GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,8,4);
        	GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,12,4);
    	    GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,16,4);
	        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,20,4);
        GLES30.glBindVertexArray(0);
        
        GLES30.glUseProgram(0);

        if(singleTap==1){
            angleCube = angleCube - 0.75f;
        }
	
        requestRender();
    }
    
    void uninitialize(){
        if(vao_cube[0] != 0){
            GLES30.glDeleteVertexArrays(1, vao_cube, 0);
            vao_cube[0]=0;
        }
        
        if(vbo_cube_position[0] != 0){
            GLES30.glDeleteBuffers(1, vbo_cube_position, 0);
            vbo_cube_position[0]=0;
        }
        
        if(vbo_cube_normal[0] != 0){
            GLES30.glDeleteBuffers(1, vbo_cube_normal, 0);
            vbo_cube_normal[0]=0;
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
