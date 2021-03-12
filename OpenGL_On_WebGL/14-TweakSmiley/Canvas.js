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
var perspectiveProjectionMatrix;

var vao_square;
var vbo_square_position;
var vbo_square_texture;
var mvpUniform;
var texture_sampler_uniform;
var texture_smiley;
var texture_gen;
var quad_texture=[];
var digitIsPressed=1;

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
    "in vec2 vTexture0_Coord;"+
    "out vec2 out_texture0_coord;"+
    "uniform mat4 u_mvp_matrix;"+
    "void main(void)"+
    "{"+
        "gl_Position=u_mvp_matrix*vPosition;"+
        "out_texture0_coord=vTexture0_Coord;"+
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
    "in vec2 out_texture0_coord;"+
    "uniform highp sampler2D u_texture0_sampler;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
        "FragColor=texture(u_texture0_sampler,out_texture0_coord);"+
    "}";
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
    gl.bindAttribLocation(shaderProgramObject,WebGLMacros.SSK_ATTRIBUTE_TEXTURE0,"vTexture0_Coord");

    gl.linkProgram(shaderProgramObject);
    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)){
        var error=gl.getProgramInfoLog(shaderProgramObject);
        if(error.length > 0){
            alert(error);
            uninitialize();
        }
    }

    mvpUniform=gl.getUniformLocation(shaderProgramObject,"u_mvp_matrix");
    texture_sampler_uniform=gl.getUniformLocation(shaderProgramObject,"u_texture0_sampler");

    var squareVertices=new Float32Array([      
    1.0,-1.0,0.0,
    1.0,1.0,0.0,
    -1.0,1.0,0.0,
    -1.0,-1.0,0.0
                                        ]);

    var squareTexcoords=new Float32Array([
    0.0,0.0,
    1.0,0.0,
    1.0,1.0,
    0.0,1.0
                                        ]);

    
    texture_smiley=gl.createTexture();
    texture_smiley.image=new Image();
    texture_smiley.image.src="Smiley.png";
    texture_smiley.image.onload=function(){
        gl.bindTexture(gl.TEXTURE_2D,texture_smiley);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL,true);
        gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,gl.RGBA,gl.UNSIGNED_BYTE,texture_smiley.image);
        gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.NEAREST);
        gl.bindTexture(gl.TEXTURE_2D,null);
    }


    vao_square=gl.createVertexArray();
    gl.bindVertexArray(vao_square);
    
        vbo_square_position=gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER,vbo_square_position);
            gl.bufferData(gl.ARRAY_BUFFER,squareVertices,gl.STATIC_DRAW);
            gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_VERTEX,
                                3,gl.FLOAT,false,0,0);
            gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_VERTEX);
        gl.bindBuffer(gl.ARRAY_BUFFER,null);


        vbo_square_texture= gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER,vbo_square_texture);
            gl.bufferData(gl.ARRAY_BUFFER,null,gl.DYNAMIC_DRAW);
            gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0,
                                2,gl.FLOAT,false,0,0);
            gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0);
        gl.bindBuffer(gl.ARRAY_BUFFER,null);
    gl.bindVertexArray(null);


    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    //gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.CULL_FACE);
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

    var modelViewMatrix=mat4.create();
    var modelViewProjectionMatrix=mat4.create();
    
    gl.useProgram(shaderProgramObject);

        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        mat4.translate(modelViewMatrix,modelViewMatrix,[0.0,0.0,-6.0]);
        mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,
                        modelViewMatrix);
        gl.uniformMatrix4fv(mvpUniform,false,modelViewProjectionMatrix);


        gl.bindVertexArray(vao_square);
            if(digitIsPressed==1){
                quad_texture[0]=1.0;
                quad_texture[1]=0.5;
                quad_texture[2]=0.5;
                quad_texture[3]=0.5;
                quad_texture[4]=0.5;
                quad_texture[5]=0.0;
                quad_texture[6]=1.0;
                quad_texture[7]=0.0;
            }else if(digitIsPressed==2){
                quad_texture[0]=1.0;
                quad_texture[1]=1.0;
                quad_texture[2]=0.0;
                quad_texture[3]=1.0;
                quad_texture[4]=0.0;
                quad_texture[5]=0.0;
                quad_texture[6]=1.0;
                quad_texture[7]=0.0;
            }else if(digitIsPressed==3){
                quad_texture[0]=2.0;
                quad_texture[1]=2.0;
                quad_texture[2]=0.0;
                quad_texture[3]=2.0;
                quad_texture[4]=0.0;
                quad_texture[5]=0.0;
                quad_texture[6]=2.0;
                quad_texture[7]=0.0;
            }else if(digitIsPressed==4){
                quad_texture[0]=0.5;
                quad_texture[1]=0.5;
                quad_texture[2]=0.5;
                quad_texture[3]=0.5;
                quad_texture[4]=0.5;
                quad_texture[5]=0.5;
                quad_texture[6]=0.5;
                quad_texture[7]=0.5;                
            }

        gl.bindTexture(gl.TEXTURE_2D,texture_smiley);
        gl.uniform1i(texture_sampler_uniform, 0);
            gl.bindBuffer(gl.ARRAY_BUFFER,vbo_square_texture);
                gl.bufferData(gl.ARRAY_BUFFER,
                            new Float32Array(quad_texture),
                            gl.DYNAMIC_DRAW);
                gl.drawArrays(gl.TRIANGLE_FAN,0,4);    
            gl.bindBuffer(gl.ARRAY_BUFFER,null);
        gl.bindVertexArray(null);

    gl.useProgram(null);

    requestAnimationFrame(draw,canvas);
}

function uninitialize(){
    if(vao_square){
        gl.deleteVertexArray(vao_square);
        vao_square=null;
    }

    if(vbo_square_position){
        gl.deleteBuffer(vbo_square_position);
        vbo_square_position=null;
    }

    if(vbo_square_texture){
        gl.deleteBuffer(vbo_square_texture);
        vbo_square_texture=null;
    }

    if(texture_smiley){
        gl.deleteBuffer(texture_smiley);
        texture_smiley=null;
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
        case 27://Esc key
            uninitialize();
            window.close();
            break;
        case 70://F key
            toggleFullscreen();
            break;
        case 0x31://1 Key
            digitIsPressed=1;
            break;
        case 0x32://2 Key
            digitIsPressed=2;
            break;
        case 0x33://3 Key
            digitIsPressed=3;
            break;
        case 0x34://4 Key
            digitIsPressed=4;
            break;
    }
}

function mouseDown(){

}

function degToRad(degrees){
    return(degrees * Math.PI / 180);
}
