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

var vao_pyramid;
var vbo_pyramid_position;
var vbo_pyramid_normal;
var L_KeyPressed_uniform;
var bIsLKeyPressed = false;


var La_uniform_one;
var Ld_uniform_one;
var Ls_uniform_one;
var light_position_uniform_one;
var Ka_uniform;
var Kd_uniform;
var Ks_uniform;
var La_uniform_two;
var Ld_uniform_two;
var Ls_uniform_two;
var light_position_uniform_two;
var material_shininess_uniform;

var one_lightAmbient=[0.0,0.0,0.0];
var one_lightDiffuse=[1.0,0.0,0.0];
var one_lightSpecular=[1.0,1.0,1.0];
var one_lightPosition=[100.0,100.0,100.0,1.0];

var two_lightAmbient=[0.0,0.0,0.0];
var two_lightDiffuse=[0.0,0.0,1.0];
var two_lightSpecular=[1.0,1.0,1.0];
var two_lightPosition=[-100.0,100.0,100.0,1.0];

var material_ambient=[0.0,0.0,0.0];
var material_diffuse=[1.0,1.0,1.0];
var material_specular=[1.0,1.0,1.0];
var material_shininess=50.0;

var model_matrix_uniform;
var view_matrix_uniform;
var projection_matrix_uniform;
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
    "in vec3 vNormal;"+
    "uniform mat4 u_model_matrix;"+
    "uniform mat4 u_view_matrix;"+
    "uniform mat4 u_projection_matrix;"+
    "uniform vec4 u_light_position_one;"+
    "uniform vec4 u_light_position_two;"+
    "uniform mediump int u_LIsKeyPressed;"+
    "out vec3 transformed_normals;"+
    "out vec3 light_direction_one;"+
    "out vec3 light_direction_two;"+
    "out vec3 viewer_vector;"+
    "void main(void)"+
    "{"+
        "if(u_LIsKeyPressed==1)"+
        "{"+
            "vec4 eye_coordinates=u_view_matrix *u_model_matrix *vPosition;"+
            "transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;"+
            "light_direction_one=vec3(u_light_position_one)-eye_coordinates.xyz;"+
            "light_direction_two=vec3(u_light_position_two)-eye_coordinates.xyz;"+
            "viewer_vector= -eye_coordinates.xyz;"+
        "}"+
        "gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
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
    "uniform int u_LIsKeyPressed;"+
    "vec3 SettingLightProperties(vec3 u_La,vec3 u_Ld,vec3 u_Ls,vec3 light_direction)"+
    "{"+
        "vec3 normalized_transformed_normals=normalize(transformed_normals);"+
        "vec3 normalized_light_direction=normalize(light_direction);"+
        "vec3 normalized_viewer_vector=normalize(viewer_vector);"+
        "vec3 ambient=u_La*u_Ka;"+
        "float tn_dot_ld=max(dot(normalized_transformed_normals,normalized_light_direction),0.0);"+
        "vec3 diffuse=u_Ld*u_Kd*tn_dot_ld;"+
        "vec3 reflection_vector=reflect(-normalized_light_direction,normalized_transformed_normals);"+
        "vec3 specular=u_Ls*u_Ks*pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);"+
        "vec3 phong_ads_color=ambient+diffuse+specular;"+
        "return phong_ads_color;"+
    "}"+
    "void main(void)"+
    "{"+
        "vec3 phong_ads_color;"+
        "if(u_LIsKeyPressed==1)"+
        "{"+
            "vec3 light_one=SettingLightProperties(u_La_one,u_Ld_one,u_Ls_one,light_direction_one);"+
            "vec3 light_two=SettingLightProperties(u_La_two,u_Ld_two,u_Ls_two,light_direction_two);"+
            "phong_ads_color=light_one+light_two;"+
        "}"+
        "else"+
        "{"+
            "phong_ads_color=vec3(1.0,1.0,1.0);"+
        "}"+
        "FragColor=vec4(phong_ads_color,1.0);"+
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
    gl.bindAttribLocation(shaderProgramObject,WebGLMacros.SSK_ATTRIBUTE_NORMAL,"vNormal");

    gl.linkProgram(shaderProgramObject);
    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)){
        var error=gl.getProgramInfoLog(shaderProgramObject);
        if(error.length > 0){
            alert(error);
            uninitialize();
        }
    }

    model_matrix_uniform=gl.getUniformLocation(shaderProgramObject,
                                                "u_model_matrix");

    view_matrix_uniform=gl.getUniformLocation(shaderProgramObject,
                                                "u_view_matrix");

    projection_matrix_uniform=gl.getUniformLocation(shaderProgramObject,
                                                    "u_projection_matrix");

    L_KeyPressed_uniform=gl.getUniformLocation(shaderProgramObject,"u_LIsKeyPressed");
    La_uniform_one=gl.getUniformLocation(shaderProgramObject,"u_La_one");
    Ld_uniform_one=gl.getUniformLocation(shaderProgramObject,"u_Ld_one");
    Ls_uniform_one=gl.getUniformLocation(shaderProgramObject,"u_Ls_one");
    light_position_uniform_one=gl.getUniformLocation(shaderProgramObject,"u_light_position_one");


    La_uniform_two=gl.getUniformLocation(shaderProgramObject,"u_La_two");
    Ld_uniform_two=gl.getUniformLocation(shaderProgramObject,"u_Ld_two");
    Ls_uniform_two=gl.getUniformLocation(shaderProgramObject,"u_Ls_two");
    light_position_uniform_two=gl.getUniformLocation(shaderProgramObject,"u_light_position_two");

    Ka_uniform=gl.getUniformLocation(shaderProgramObject,"u_Ka");
    Kd_uniform=gl.getUniformLocation(shaderProgramObject,"u_Kd");
    Ks_uniform=gl.getUniformLocation(shaderProgramObject,"u_Ks");
    material_shininess_uniform=gl.getUniformLocation(shaderProgramObject,"u_material_shininess");
    
    var pyramidVertices=new Float32Array([      
        
        0.0,1.0,0.0,    //front-top
        -1.0,-1.0,1.0,  //front-left
        1.0,-1.0,1.0,   //front-right

        0.0,1.0,0.0,    //right-top
        1.0,-1.0,1.0,   //right-left
        1.0,-1.0,-1.0,  //right-right
        
        0.0,1.0,0.0,    //back-top
        1.0,-1.0,-1.0,  //back-left
        -1.0,-1.0,-1.0, //back-right
        
        0.0,1.0,0.0,    //left-top
        -1.0,-1.0,-1.0, //left-left
        -1.0,-1.0,1.0   //left-right
                                        ]);



    var pyramidNormals=new Float32Array([
        //front-face
        0.0,0.0,1.0,
        0.0,0.0,1.0,
        0.0,0.0,1.0,

        //right-face
        1.0,0.0,0.0,
        1.0,0.0,0.0,
        1.0,0.0,0.0,

        //back face
        0.0,0.0,-1.0,
        0.0,0.0,-1.0,
        0.0,0.0,-1.0,

        //left-face
        -1.0,0.0,0.0,
        -1.0,0.0,0.0,
        -1.0,0.0,0.0

                                        ]);
    
    vao_pyramid=gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);
        vbo_pyramid_position = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER,vbo_pyramid_position);
        gl.bufferData(gl.ARRAY_BUFFER,pyramidVertices,gl.STATIC_DRAW);
        gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_VERTEX,
                            3,gl.FLOAT,false,0,0);
        gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_VERTEX);
        gl.bindBuffer(gl.ARRAY_BUFFER,null);

        vbo_pyramid_normal = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER,vbo_pyramid_normal);
        gl.bufferData(gl.ARRAY_BUFFER,pyramidNormals,gl.STATIC_DRAW);
        gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_NORMAL,
                               3,gl.FLOAT,false,0,0);
        gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_NORMAL);
        gl.bindBuffer(gl.ARRAY_BUFFER,null);
    gl.bindVertexArray(null);



    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.disable(gl.CULL_FACE);
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

    var modelMatrix=mat4.create();
    var viewMatrix=mat4.create();

    
    gl.useProgram(shaderProgramObject);

        if(bIsLKeyPressed==true){
            gl.uniform1i(L_KeyPressed_uniform,1);

            gl.uniform3fv(La_uniform_one,one_lightAmbient);
            gl.uniform3fv(Ld_uniform_one,one_lightDiffuse);
            gl.uniform3fv(Ls_uniform_one,one_lightSpecular);
            gl.uniform4fv(light_position_uniform_one,one_lightPosition);

            gl.uniform3fv(La_uniform_two,two_lightAmbient);
            gl.uniform3fv(Ld_uniform_two,two_lightDiffuse);
            gl.uniform3fv(Ls_uniform_two,two_lightSpecular);
            gl.uniform4fv(light_position_uniform_two,two_lightPosition);

            gl.uniform3fv(Ka_uniform,material_ambient);
            gl.uniform3fv(Kd_uniform,material_diffuse);
            gl.uniform3fv(Ks_uniform,material_specular);
            gl.uniform1f(material_shininess_uniform,material_shininess);
            }else{
                gl.uniform1i(L_KeyPressed_uniform,0);
            }

            mat4.identity(modelMatrix); 
            mat4.identity(viewMatrix); 
            mat4.translate(modelMatrix,modelMatrix,[0.0,0.0,-4.0]);
            mat4.rotateY(modelMatrix ,modelMatrix, degToRad(anglePyramid));
            //mat4.multiply(modelMatrix,modelMatrix,rotationMatrix);
            gl.uniformMatrix4fv(model_matrix_uniform,false,modelMatrix);
            gl.uniformMatrix4fv(view_matrix_uniform,false,viewMatrix);
            gl.uniformMatrix4fv(projection_matrix_uniform,false,perspectiveProjectionMatrix);
            gl.bindVertexArray(vao_pyramid);
                gl.drawArrays(gl.TRIANGLES,0,12);
            gl.bindVertexArray(null);
    gl.useProgram(null);

    anglePyramid=anglePyramid+1.0;
    if(anglePyramid>=360.0){
        anglePyramid=anglePyramid-360.0;
    }
    
    requestAnimationFrame(draw,canvas);
}

function uninitialize(){
    if(vao_pyramid){
        gl.deleteVertexArray(vao_pyramid);
        vao_pyramid=null;
    }

    if(vbo_pyramid_position){
        gl.deleteBuffer(vbo_pyramid_position);
        vbo_pyramid_position=null;
    }

    if(vbo_pyramid_normal){
        gl.deleteBuffer(vbo_pyramid_normal);
        vbo_pyramid_normal=null;
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
        case 27://Esc Key
            uninitialize();
            window.close();
            break;
        case 70://F key
            toggleFullscreen();
            break;
        case 76://L Key
            if (!bIsLKeyPressed) {
                bIsLKeyPressed=true;
            }
            else {
                bIsLKeyPressed=false;
            }
            break;
    }   
}

function mouseDown(){

}

function degToRad(degrees){
    return(degrees * Math.PI / 180);
}
