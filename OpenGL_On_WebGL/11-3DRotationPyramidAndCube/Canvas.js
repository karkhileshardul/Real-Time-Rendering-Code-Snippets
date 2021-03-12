var canvas=null;
var gl=null;
var bFullscreen=false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros={
    SSK_ATTRIBUTE_VERTEX:0,
    SSK_ATTRIBUTE_COLOR:1,
    SSK_ATTRIBUTE_NORMAL:2,
    SSK_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao;
var vbo;
var vbo_cube_position;
var vbo_cube_color;
var mvpUniform;
var perspectiveProjectionMatrix;
var anglePyramid=0.0;
var angleCube=0.0;

var requestAnimationFrame=  window.requestAnimationFrame || window.webkitRequestAnimationFrame ||
                            window.mozRequestAnimationFrame ||  window.oRequestAnimationFrame ||
                            window.msRequestAnimationFrame;

var cancelAnimationFrame=   window.cancelAnimationFrame ||  window.webkitCancelRequestAnimationFrame    ||
                            window.webkitCancelAnimationFrame  ||   window.mozCancelRequestAnimationFrame   ||
                            window.mozCancelAnimationFrame  ||   window.oCancelRequestAnimationFrame    ||
                            window.msCancelRequestAnimationFrame    ||  window.msCancelAnimationnFrame;

function main(){
    canvas=document.getElementById("AMC");
    if(!canvas)
        console.log("Obtaining Canvas Failed\n");
    else
        console.log("Obtaining Canvas Succeeded\n");
    canvas_original_width=canvas.width;
    canvas_original_height=canvas.height;

    window.addEventListener("keydown",keyDown,false);
    window.addEventListener("click",mouseDown,false);
    window.addEventListener("resize",resize,false);

    init();

    resize();
    draw();
}

function toggleFullscreen(){
    var fullscreen_element= document.fullscreenElement  ||  document.webkitFullscreenElement    ||
                            document.mozFullScreenElement   ||  document.msFullscreenElement;

    if(fullscreen_element==null){
        if(canvas.requestFullscreen)
            canvas.requestFullscreen();
        else if(canvas.mozRequestFullScreen)
            canvas.mozRequestFullScreen();
        else if(canvas.webkitRequestFullScreen)
            canvas.webkitRequestFullScreen();
        else if(canvas.msRequestFullscreen)
            canvas.msRequestFullscreen();
        bFullscreen=true;
    }else{
        if(document.exitFullscreen)
            document.exitFullscreen();
        else if(document.mozCancaelFullScreen)
            document.mozCancelFullScreen();
        else if(document.webkitExitFullscreen)
            document.webkitExitFullscreen();
        else if(document.msExitFullscreen)
            document.msExitFullscreen();
        bFullscreen=false;
    }
}

function init(){
    gl = canvas.getContext("webgl2");
    if(gl==null){
        console.log("Failed to get the rendering context for WebGL");
        return;
    }
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;
    
    var vertexShaderSourceCode=
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
    "}";

    vertexShaderObject=gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject,vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);
    if(gl.getShaderParameter(vertexShaderObject,gl.COMPILE_STATUS)==false){
        var error=gl.getShaderInfoLog(vertexShaderObject);
        if(error.length > 0){
            alert(error);
            uninitialize();
        }
    }
    
    var fragmentShaderSourceCode=
    "#version 300 es"+
    "\n"+
    "precision highp float;"+
    "in vec4 out_color;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "FragColor = out_color;"+
    "}"
    fragmentShaderObject=gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject,fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);
    if(gl.getShaderParameter(fragmentShaderObject,gl.COMPILE_STATUS)==false){
        var error=gl.getShaderInfoLog(fragmentShaderObject);
        if(error.length > 0){
            alert(error);
            uninitialize();
        }
    }
    
    shaderProgramObject=gl.createProgram();
    gl.attachShader(shaderProgramObject,vertexShaderObject);
    gl.attachShader(shaderProgramObject,fragmentShaderObject);
    
    gl.bindAttribLocation(shaderProgramObject,WebGLMacros.SSK_ATTRIBUTE_VERTEX,"vPosition");
    gl.bindAttribLocation(shaderProgramObject,WebGLMacros.SSK_ATTRIBUTE_COLOR,"vColor");

    gl.linkProgram(shaderProgramObject);
    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)){
        var error=gl.getProgramInfoLog(shaderProgramObject);
        if(error.length > 0){
            alert(error);
            uninitialize();
        }
    }

    mvpUniform=gl.getUniformLocation(shaderProgramObject,"u_mvp_matrix");
    
    var pyramidVertices=new Float32Array([      
        0.0,1.0,0.0,
        -1.0,-1.0,1.0,
        1.0,-1.0,1.0,
        0.0,1.0,0.0,
        1.0,-1.0,1.0,
        1.0,-1.0,-1.0,
        0.0,1.0,0.0,
        1.0,-1.0,-1.0,
        -1.0,-1.0,-1.0,
        0.0,1.0,0.0,
        -1.0,-1.0,-1.0,
        -1.0,-1.0,1.0
                                        ]);

    var pyramidColors=new Float32Array([   
        1.0,0.0,0.0,
        0.0,1.0,0.0,
        0.0,0.0,1.0,
        1.0,0.0,0.0,
        0.0,0.0,1.0,
        0.0,1.0,0.0,
        1.0,0.0,0.0,
        0.0,1.0,0.0,
        0.0,0.0,1.0,
        1.0,0.0,0.0,
        0.0,0.0,1.0,
        0.0,1.0,0.0

    ]);


    vao=gl.createVertexArray();
    gl.bindVertexArray(vao);
    
        vbo = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER,vbo);
        gl.bufferData(gl.ARRAY_BUFFER,pyramidVertices,gl.STATIC_DRAW);
        gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_VERTEX,
                            3,gl.FLOAT,false,0,0);
        gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_VERTEX);
        gl.bindBuffer(gl.ARRAY_BUFFER,null);

        vbo = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER,vbo);
        gl.bufferData(gl.ARRAY_BUFFER,pyramidColors,gl.STATIC_DRAW);
        gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR,
                               3,gl.FLOAT,false,0,0);
        gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
        gl.bindBuffer(gl.ARRAY_BUFFER,null);
    gl.bindVertexArray(null);

    var CubeVertices=new Float32Array([   
        1.0,1.0,-1.0,
        -1.0,1.0,-1.0,
        -1.0,1.0,1.0,
        1.0,1.0,1.0,

        1.0,-1.0,-1.0,
        -1.0,-1.0,-1.0,
        -1.0,-1.0,1.0,
        1.0,-1.0,1.0,
        
        1.0,1.0,1.0,
        -1.0,1.0,1.0,
        -1.0,-1.0,1.0,
        1.0,-1.0,1.0,
        
        1.0,1.0,-1.0,
        -1.0,1.0,-1.0,
        -1.0,-1.0,-1.0,
        1.0,-1.0,-1.0,
        
        1.0,1.0,-1.0,
        1.0,1.0,1.0,
        1.0,-1.0,1.0,
        1.0,-1.0,-1.0,
        
        -1.0,1.0,1.0,
        -1.0,1.0,-1.0,
        -1.0,-1.0,-1.0,
        -1.0,-1.0,1.0

         ]);

    for(var i=0;i<(24*3);i++){
        if(CubeVertices[i]<0.0){
            CubeVertices[i]=CubeVertices[i]+0.25;
        }else if(CubeVertices[i]>0.0){
            CubeVertices[i]=CubeVertices[i]-0.25;
        }else{
            CubeVertices[i]=CubeVertices[i];
        }
    }


    var CubeColors=new Float32Array([  
        0.0,1.0,0.0,
        0.0,1.0,0.0,
        0.0,1.0,0.0,
        0.0,1.0,0.0,
    
        1.0,0.5,0.0,
        1.0,0.5,0.0,
        1.0,0.5,0.0,
        1.0,0.5,0.0,
        
        1.0,0.0,0.0,
        1.0,0.0,0.0,
        1.0,0.0,0.0,
        1.0,0.0,0.0,

        1.0,1.0,0.0,
        1.0,1.0,0.0,
        1.0,1.0,0.0,
        1.0,1.0,0.0,
    
        0.0,0.0,1.0,
        0.0,0.0,1.0,
        0.0,0.0,1.0,
        0.0,0.0,1.0,
    
        1.0,0.0,1.0,
        1.0,0.0,1.0,
        1.0,0.0,1.0,
        1.0,0.0,1.0,
    ]);

    vao1=gl.createVertexArray();
    gl.bindVertexArray(vao1);
    
        vbo= gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER,vbo);
        gl.bufferData(gl.ARRAY_BUFFER,CubeVertices,gl.STATIC_DRAW);
        gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_VERTEX,
                            3,gl.FLOAT,false,0,0);
        gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_VERTEX);
        gl.bindBuffer(gl.ARRAY_BUFFER,null);


        vbo= gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER,vbo);
        gl.bufferData(gl.ARRAY_BUFFER,CubeColors,gl.STATIC_DRAW);
        gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_COLOR,
                            3,gl.FLOAT,false,0,0);
        gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_COLOR);
        gl.bindBuffer(gl.ARRAY_BUFFER,null);
    gl.bindVertexArray(null);

    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    
    perspectiveProjectionMatrix=mat4.create();
}


function resize(){
    if(bFullscreen==true){
        canvas.width=window.innerWidth;
        canvas.height=window.innerHeight;
    }else{
        canvas.width=canvas_original_width;
        canvas.height=canvas_original_height;
    }
    gl.viewport(0,0,canvas.width,canvas.height);
    mat4.perspective(perspectiveProjectionMatrix,45.0,
                        parseFloat(canvas.width)/parseFloat(canvas.height),0.1,100.0);
}

function draw(){
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT |
             gl.STENCIL_BUFFER_BIT);
    
    gl.useProgram(shaderProgramObject);
        var modelViewMatrix=mat4.create();
        var rotationMatrix=mat4.create();
        var modelViewProjectionMatrix=mat4.create();

        mat4.translate(modelViewMatrix,modelViewMatrix,[-1.5,0.0,-6.0]);
        mat4.rotateY(modelViewMatrix ,modelViewMatrix, degToRad(anglePyramid));
        mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,
                        modelViewMatrix);
        gl.uniformMatrix4fv(mvpUniform,false,modelViewProjectionMatrix);
        gl.bindVertexArray(vao);
            gl.drawArrays(gl.TRIANGLES,0,12);
        gl.bindVertexArray(null);


        mat4.identity(modelViewMatrix); 
        mat4.identity(rotationMatrix);
        mat4.identity(modelViewProjectionMatrix); 

        mat4.translate(modelViewMatrix,modelViewMatrix,[1.5,0.0,-6.0]);
        //mat4.rotateX(modelViewMatrix ,modelViewMatrix, degToRad(angleCube));
        mat4.rotateX(modelViewMatrix ,modelViewMatrix, degToRad(angleCube));
        mat4.rotateY(modelViewMatrix ,modelViewMatrix, degToRad(angleCube));
        mat4.rotateZ(modelViewMatrix ,modelViewMatrix, degToRad(angleCube));
        mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,
                        modelViewMatrix);
        gl.uniformMatrix4fv(mvpUniform,false,modelViewProjectionMatrix);
        gl.bindVertexArray(vao1);
            gl.drawArrays(gl.TRIANGLE_FAN,0,4);
            gl.drawArrays(gl.TRIANGLE_FAN,4,4);
            gl.drawArrays(gl.TRIANGLE_FAN,8,4);
            gl.drawArrays(gl.TRIANGLE_FAN,12,4);
            gl.drawArrays(gl.TRIANGLE_FAN,16,4);
            gl.drawArrays(gl.TRIANGLE_FAN,20,4);
        gl.bindVertexArray(null);

    gl.useProgram(null);

    anglePyramid=anglePyramid+2.0;
    if(anglePyramid>=360.0){
        anglePyramid=anglePyramid-360.0;
    }
    
    angleCube=angleCube+2.0;
    if(angleCube>=360.0){
        angleCube=angleCube-360.0;
    }



    requestAnimationFrame(draw,canvas);
}

function uninitialize(){
    if(vao){
        gl.deleteVertexArray(vao);
        vao=null;
    }

    if(vbo){
        gl.deleteBuffer(vbo);
        vbo=null;
    }

    if(shaderProgramObject){
        if(fragmentShaderObject){
            gl.detachShader(shaderProgramObject,fragmentShaderObject);
            gl.deleteShader(fragmentShaderObject);
            fragmentShaderObject=null;
        }
        if(vertexShaderObject){
            gl.detachShader(shaderProgramObject,vertexShaderObject);
            gl.deleteShader(vertexShaderObject);
            vertexShaderObject=null;
        }
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject=null;
    }
}

function keyDown(event){
    switch(event.keyCode){
        case 27:
            uninitialize();
            window.close();
            break;
        case 70:
            toggleFullscreen();
            break;
    }
}

function mouseDown(){

}

function degToRad(degrees){
    return(degrees * Math.PI / 180);
}
