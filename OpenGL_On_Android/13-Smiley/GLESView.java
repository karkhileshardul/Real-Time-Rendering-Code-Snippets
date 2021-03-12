package com.astromedicomp.win_smiley;
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
import android.graphics.BitmapFactory; 
import android.graphics.Bitmap; 
import android.opengl.GLUtils; 
import android.opengl.Matrix; 

public class GLESView extends GLSurfaceView
                     implements GLSurfaceView.Renderer, 
                     OnGestureListener, OnDoubleTapListener{
    private final Context context;
    private GestureDetector gestureDetector;
    
    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
    private int[] vao_square = new int[1];
    private int[] vbo_square_position = new int[1];
    private int[] vbo_square_texture = new int[1];
    private int[] texture_smiley = new int[1]; 
    private int mvpUniform;
    private int texture0_sampler_uniform;  
    private float perspectiveProjectionMatrix[]=new float[16]; 
    
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
        return(true);
    }
    
    @Override
    public boolean onDoubleTapEvent(MotionEvent e){
        return(true);
    }    
    @Override
    public boolean onSingleTapConfirmed(MotionEvent e){
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
         "in vec2 vTexture0_Coord;"+
         "out vec2 out_texture0_coord;"+
         "uniform mat4 u_mvp_matrix;"+
         "void main(void)"+
         "{"+
         "gl_Position = u_mvp_matrix * vPosition;"+
         "out_texture0_coord = vTexture0_Coord;"+
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
         "in vec2 out_texture0_coord;"+
         "uniform highp sampler2D u_texture0_sampler;"+
         "out vec4 FragColor;"+
         "void main(void)"+
         "{"+
         "FragColor = texture(u_texture0_sampler, out_texture0_coord);"+
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
                                    GLESMacros.SSK_ATTRIBUTE_TEXTURE0,
                                    "vTexture0_Coord");

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

        mvpUniform=GLES30.glGetUniformLocation(shaderProgramObject,"u_mvp_matrix");
        texture0_sampler_uniform=GLES30.glGetUniformLocation(shaderProgramObject,
                                                            "u_texture0_sampler");
        texture_smiley[0]=loadGLTexture(R.raw.smiley);
        
        float squareVertices[]= {   
            1.0f,1.0f,0.0f,
            -1.0f,1.0f,0.0f,
            -1.0f,-1.0f,0.0f,
            1.0f,-1.0f,0.0f
        };

        float squareTexcoords[]={
            1.0f,1.0f,
            0.0f,1.0f,
            0.0f,0.0f,
            1.0f,0.0f,
        };

        GLES30.glGenVertexArrays(1,vao_square,0);
        GLES30.glBindVertexArray(vao_square[0]);
            GLES30.glGenBuffers(1,vbo_square_position,0);
            GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_square_position[0]);
        
                ByteBuffer byteBuffer=ByteBuffer.allocateDirect(squareVertices.length * 4);
                byteBuffer.order(ByteOrder.nativeOrder());
                FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
                verticesBuffer.put(squareVertices);
                verticesBuffer.position(0);
        
                GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                                    squareVertices.length * 4,
                                    verticesBuffer,
                                    GLES30.GL_STATIC_DRAW);
            
                GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_VERTEX,
                                            3,GLES30.GL_FLOAT,
                                            false,0,0);
                GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_VERTEX);
            GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
        
        
            GLES30.glGenBuffers(1,vbo_square_texture,0);
            GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_square_texture[0]);
        
                byteBuffer=ByteBuffer.allocateDirect(squareTexcoords.length * 4);
                byteBuffer.order(ByteOrder.nativeOrder());
                verticesBuffer=byteBuffer.asFloatBuffer();
                verticesBuffer.put(squareTexcoords);
                verticesBuffer.position(0);
                GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                                    squareTexcoords.length * 4,
                                    verticesBuffer,
                                    GLES30.GL_STATIC_DRAW);
        
                GLES30.glVertexAttribPointer(GLESMacros.SSK_ATTRIBUTE_TEXTURE0,
                                            2,GLES30.GL_FLOAT,
                                            false,0,0);
        
                GLES30.glEnableVertexAttribArray(GLESMacros.SSK_ATTRIBUTE_TEXTURE0);
            GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
        GLES30.glBindVertexArray(0);

        GLES30.glEnable(GLES30.GL_DEPTH_TEST);
        GLES30.glDepthFunc(GLES30.GL_LEQUAL);
        GLES30.glEnable(GLES30.GL_CULL_FACE);
        GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        Matrix.setIdentityM(perspectiveProjectionMatrix,0);
    }
    
    private int loadGLTexture(int imageFileResourceID){
        int[] texture=new int[1];        
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;
        Bitmap bitmap=BitmapFactory.decodeResource(context.getResources(),
                                                    imageFileResourceID, options);
        GLES30.glGenTextures(1, texture, 0);
        GLES30.glPixelStorei(GLES30.GL_UNPACK_ALIGNMENT,1);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D,texture[0]);
        GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D,GLES30.GL_TEXTURE_MAG_FILTER,GLES30.GL_LINEAR);
        GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D,GLES30.GL_TEXTURE_MIN_FILTER,GLES30.GL_LINEAR_MIPMAP_LINEAR);
        GLUtils.texImage2D(GLES30.GL_TEXTURE_2D, 0, bitmap, 0);
        GLES30.glGenerateMipmap(GLES30.GL_TEXTURE_2D);
        return(texture[0]);
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
        float modelViewMatrix[]=new float[16];
        float modelViewProjectionMatrix[]=new float[16];
        
        Matrix.setIdentityM(modelViewMatrix,0);
        Matrix.setIdentityM(modelViewProjectionMatrix,0);

        Matrix.translateM(modelViewMatrix,0,0.0f,0.0f,-5.0f);     
        Matrix.multiplyMM(modelViewProjectionMatrix,0,
                        perspectiveProjectionMatrix,0,modelViewMatrix,0);
        GLES30.glUniformMatrix4fv(mvpUniform,1,false,
                                modelViewProjectionMatrix,0);
        GLES30.glBindVertexArray(vao_square[0]);
        GLES30.glActiveTexture(GLES30.GL_TEXTURE0);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D,texture_smiley[0]);
        GLES30.glUniform1i(texture0_sampler_uniform, 0);

        GLES30.glBindVertexArray(vao_square[0]);
            GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,0,4); 
        GLES30.glBindVertexArray(0);
        
        GLES30.glUseProgram(0);
        requestRender();
    }
    
    void uninitialize(){
        if(vao_square[0] != 0){
            GLES30.glDeleteVertexArrays(1, vao_square, 0);
            vao_square[0]=0;
        }
        
        if(vbo_square_position[0] != 0){
            GLES30.glDeleteBuffers(1, vbo_square_position, 0);
            vbo_square_position[0]=0;
        }
        
        if(vbo_square_texture[0] != 0){
            GLES30.glDeleteBuffers(1, vbo_square_texture, 0);
            vbo_square_texture[0]=0;
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
        
        if (texture_smiley[0] != 0){
            GLES30.glDeleteTextures(1, texture_smiley, 0);
            texture_smiley[0] = 0;
        }
    }
}
