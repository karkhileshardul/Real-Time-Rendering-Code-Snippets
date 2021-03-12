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

var light_ambient=[0.0,0.0,0.0];
var light_diffuse=[1.0,1.0,1.0];
var light_specular=[1.0,1.0,1.0];
var light_position=[100.0,100.0,100.0,1.0];

var material_ambient= [0.0,0.0,0.0];
var material_diffuse= [1.0,1.0,1.0];
var material_specular= [1.0,1.0,1.0];
var material_shininess= 50.0;

var sphere=null;

var modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;
var laUniform, ldUniform, lsUniform, lightPositionUniform;
var kaUniform, kdUniform, ksUniform, materialShininessUniform;
var LKeyPressedUniform;
var bLKeyPressed=false;
var perspectiveProjectionMatrix;

var requestAnimationFrame =
    window.requestAnimationFrame ||
    window.webkitRequestAnimationFrame ||
    window.mozRequestAnimationFrame ||
    window.oRequestAnimationFrame ||
    window.msRequestAnimationFrame;

var cancelAnimationFrame =
    window.cancelAnimationFrame ||
    window.webkitCancelRequestAnimationFrame || window.webkitCancelAnimationFrame ||
    window.mozCancelRequestAnimationFrame || window.mozCancelAnimationFrame ||
    window.oCancelRequestAnimationFrame || window.oCancelAnimationFrame ||
    window.msCancelRequestAnimationFrame || window.msCancelAnimationFrame;

function main(){
    canvas = document.getElementById("AMC");
    if(!canvas)
        console.log("Obtaining Canvas Failed\n");
    else
        console.log("Obtaining Canvas Succeeded\n");
    canvas_original_width=canvas.width;
    canvas_original_height=canvas.height;
    
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
    window.addEventListener("resize", resize, false);

    init();
    
    resize();
    draw();
}

function toggleFullScreen(){
    var fullscreen_element =
        document.fullscreenElement ||
        document.webkitFullscreenElement ||
        document.mozFullScreenElement ||
        document.msFullscreenElement ||
        null;

    if(fullscreen_element==null){
        if(canvas.requestFullscreen)
            canvas.requestFullscreen();
        else if(canvas.mozRequestFullScreen)
            canvas.mozRequestFullScreen();
        else if(canvas.webkitRequestFullscreen)
            canvas.webkitRequestFullscreen();
        else if(canvas.msRequestFullscreen)
            canvas.msRequestFullscreen();
        bFullscreen=true;
    }else{
        if(document.exitFullscreen)
            document.exitFullscreen();
        else if(document.mozCancelFullScreen)
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
    "uniform mediump int u_LKeyPressed;"+
    "uniform vec4 u_light_position;"+
    "out vec3 transformed_normals;"+
    "out vec3 light_direction;"+
    "out vec3 viewer_vector;"+
    "void main(void)"+
    "{"+
        "if(u_LKeyPressed == 1)"+
        "{"+
            "vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;"+
            "transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;"+
            "light_direction = vec3(u_light_position) - eye_coordinates.xyz;"+
            "viewer_vector = -eye_coordinates.xyz;"+
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
    "in vec3 light_direction;"+
    "in vec3 viewer_vector;"+
    "out vec4 FragColor;"+
    "uniform vec3 u_La;"+
    "uniform vec3 u_Ld;"+
    "uniform vec3 u_Ls;"+
    "uniform vec3 u_Ka;"+
    "uniform vec3 u_Kd;"+
    "uniform vec3 u_Ks;"+
    "uniform float u_material_shininess;"+
    "uniform int u_LKeyPressed;"+
    "void main(void)"+
    "{"+
        "vec3 phong_ads_color;"+
        "if(u_LKeyPressed == 1)"+
        "{"+
            "vec3 normalized_transformed_normals=normalize(transformed_normals);"+
            "vec3 normalized_light_direction=normalize(light_direction);"+
            "vec3 normalized_viewer_vector=normalize(viewer_vector);"+
            "vec3 ambient = u_La * u_Ka;"+
            "float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);"+
            "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
            "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);"+
            "vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);"+
            "phong_ads_color=ambient + diffuse + specular;"+
        "}"+
        "else"+
        "{"+
            "phong_ads_color = vec3(1.0, 1.0, 1.0);"+
        "}"+
        "FragColor = vec4(phong_ads_color, 1.0);"+
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
    gl.bindAttribLocation(shaderProgramObject,WebGLMacros.SSK_ATTRIBUTE_NORMAL,"vNormal");

    gl.linkProgram(shaderProgramObject);
    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)){
        var error=gl.getProgramInfoLog(shaderProgramObject);
        if(error.length > 0){
            alert(error);
            uninitialize();
        }
    }

    modelMatrixUniform=gl.getUniformLocation(shaderProgramObject,"u_model_matrix");
    viewMatrixUniform=gl.getUniformLocation(shaderProgramObject,"u_view_matrix");
    projectionMatrixUniform=gl.getUniformLocation(shaderProgramObject,"u_projection_matrix");
    
    LKeyPressedUniform=gl.getUniformLocation(shaderProgramObject,"u_LKeyPressed");    
    
    laUniform=gl.getUniformLocation(shaderProgramObject,"u_La");
    ldUniform=gl.getUniformLocation(shaderProgramObject,"u_Ld");
    lsUniform=gl.getUniformLocation(shaderProgramObject,"u_Ls");
    lightPositionUniform=gl.getUniformLocation(shaderProgramObject,"u_light_position");
    
    kaUniform=gl.getUniformLocation(shaderProgramObject,"u_Ka");
    kdUniform=gl.getUniformLocation(shaderProgramObject,"u_Kd");
    ksUniform=gl.getUniformLocation(shaderProgramObject,"u_Ks");
    materialShininessUniform=gl.getUniformLocation(shaderProgramObject,"u_material_shininess");
    
    sphere=new Mesh();
    makeSphere(sphere,2.0,30,30);

    gl.enable(gl.DEPTH_TEST);
    
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.CULL_FACE);
    gl.clearColor(0.0, 0.0, 0.0, 1.0); // black
    
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
   
    gl.viewport(0, 0, canvas.width, canvas.height);
    
    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width)/parseFloat(canvas.height), 0.1, 100.0);
}

function draw(){
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    var modelMatrix=mat4.create();
    var viewMatrix=mat4.create();

    gl.useProgram(shaderProgramObject);
    
        if(bLKeyPressed==true){
            gl.uniform1i(LKeyPressedUniform, 1);
            
            gl.uniform3fv(laUniform, light_ambient); 
            gl.uniform3fv(ldUniform, light_diffuse); 
            gl.uniform3fv(lsUniform, light_specular);
            gl.uniform4fv(lightPositionUniform, light_position);
            
            gl.uniform3fv(kaUniform, material_ambient); 
            gl.uniform3fv(kdUniform, material_diffuse); 
            gl.uniform3fv(ksUniform, material_specular);
            gl.uniform1f(materialShininessUniform, material_shininess); 
        }else{
            gl.uniform1i(LKeyPressedUniform, 0);
        }
    
        mat4.translate(modelMatrix, modelMatrix, [0.0,0.0,-6.0]);
    
        gl.uniformMatrix4fv(modelMatrixUniform,false,modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform,false,viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform,false,perspectiveProjectionMatrix);
        
        sphere.draw();
   
    gl.useProgram(null);
    requestAnimationFrame(draw, canvas);
}

function uninitialize(){
    if(sphere){
        sphere.deallocate();
        sphere=null;
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
        case 76: 
            if(bLKeyPressed==false)
                bLKeyPressed=true;
            else
                bLKeyPressed=false;
            break;
        case 70: 
            toggleFullScreen();
            break;
    }
}

function mouseDown(){

}

