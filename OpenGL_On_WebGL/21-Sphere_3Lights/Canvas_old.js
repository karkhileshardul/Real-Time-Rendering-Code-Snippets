var canvas=null;
var gl=null;
var bFullscreen=false;
var canvas_original_width;
var canvas_original_height;

var gbLightPerVertex = false;
var gbLightPerFragment = false;

var bIsVKeyPressed = false;
var bIsFKeyPressed = false;

const WebGLMacros={
	VDG_ATTRIBUTE_VERTEX:0,
	VDG_ATTRIBUTE_COLOR:1,
	VDG_ATTRIBUTE_NORMAL:2,
	VDG_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var light_ambient_red=[0.0,0.0,0.0];
var light_diffuse_red=[1.0,0.0,0.0];
var light_specular_red=[1.0,0.0,0.0];
var light_position_one=[0.0,0.0,0.0,1.0];

var light_ambient_green=[0.0,0.0,0.0];
var light_diffuse_green=[0.0,1.0,0.0];
var light_diffuse_green=[0.0,1.0,0.0];
var light_position_two=[0.0,0.0,0.0,1.0];

var light_ambient_blue=[0.0,0.0,0.0];
var light_diffuse_blue=[0.0,0.0,1.0];
var light_specular_blue=[0.0,0.0,1.0];
var light_position_three=[0.0,0.0,0.0,1.0];

var material_ambient= [0.0,0.0,0.0];
var material_diffuse= [1.0,1.0,1.0];
var material_specular= [1.0,1.0,1.0];
var material_shininess= 50.0;

var sphere=null;
var F_KeyPressed_uniform;
var V_KeyPressed_uniform;
var perspectiveProjectionMatrix;

var modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;
var La_uniform_red, Ld_uniform_red, Ls_uniform_red, light_position_uniform_red;
var La_uniform_green, Ld_uniform_green, Ls_uniform_green, light_position_uniform_green;
var La_uniform_blue, Ld_uniform_blue, Ls_uniform_blue, light_position_uniform_blue;
var Ka_uniform, Kd_uniform, Ks_uniform, materialShininessUniform;
var LKeyPressedUniform;

var bLKeyPressed=false;

var gAngle = 0.0;

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
	"precision highp int;"+
	"precision highp float;"+
    "in vec4 vPosition;"+
    "in vec3 vNormal;"+
    "uniform mat4 u_model_matrix;"+
    "uniform mat4 u_view_matrix;"+
    "uniform mat4 u_projection_matrix;"+
    "uniform int u_per_vertex_lighting_enabled;"+
    "uniform int u_per_fragment_lighting_enabled;"+
    "uniform vec3 u_La_red;"+
    "uniform vec3 u_Ld_red;"+
    "uniform vec3 u_Ls_red;"+
    "uniform vec4 u_light_position_red;"+
    "uniform vec3 u_La_green;"+
    "uniform vec3 u_Ld_green;"+
    "uniform vec3 u_Ls_green;"+
    "uniform vec4 u_light_position_green;"+
    "uniform vec3 u_La_blue;"+
    "uniform vec3 u_Ld_blue;"+
    "uniform vec3 u_Ls_blue;"+
    "uniform vec4 u_light_position_blue;"+
    "uniform vec3 u_Ka;"+
    "uniform vec3 u_Kd;"+
    "uniform vec3 u_Ks;"+
    "uniform float u_material_shininess;"+
    "vec3 phong_ads_color;"+
    "out vec3 out_phong_ads_color;"+
    "out vec3 transformed_normals;"+
    "out vec3 light_direction_red;"+
    "out vec3 light_direction_green;"+
    "out vec3 light_direction_blue;"+
    "out vec3 viewer_vector;"+
    "vec3 SettingLightProperties(vec3 La, vec3 Ld, vec3 Ls, vec4 light_position)"+
    "{"+
        "vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;"+
        "vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
        "vec3 light_direction = normalize(vec3(light_position) - eyeCoordinates.xyz);"+
        "float tn_dot_ld = max(dot(transformed_normals, light_direction), 0.0);"+
        "vec3 ambient = La * u_Kd;"+
        "vec3 diffuse = Ld * u_Kd * tn_dot_ld;"+
        "vec3 reflection_vector = reflect(-light_direction, transformed_normals);"+
        "vec3 viewer_vector = normalize(-eyeCoordinates.xyz);"+
        "vec3 specular = Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);"+
        "phong_ads_color = ambient + diffuse + specular;"+
        "return phong_ads_color;"+
    "}"+
    "void main(void)"+
    "{"+
        "if(u_per_vertex_lighting_enabled==1)"+
        "{"+
            "vec3 red_light=SettingLightProperties(u_La_red,u_Ld_red,u_Ls_red,u_light_position_red);"+
            "vec3 green_light=SettingLightProperties(u_La_green,u_Ld_green,u_Ls_green,u_light_position_green);"+
            "vec3 blue_light=SettingLightProperties(u_La_blue,u_Ld_blue,u_Ls_blue,u_light_position_blue);"+
            "out_phong_ads_color+=red_light+green_light+blue_light;"+
        "}"+
        "if(u_per_fragment_lighting_enabled==1)"+
        "{"+
            "vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;"+
            "transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;"+
            "light_direction_red = vec3(u_light_position_red) - eye_coordinates.xyz;"+
            "light_direction_green = vec3(u_light_position_green) - eye_coordinates.xyz;"+
            "light_direction_blue = vec3(u_light_position_blue) - eye_coordinates.xyz;"+
            "viewer_vector=-eye_coordinates.xyz;"+
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
	"precision highp int;"+
	"precision highp float;"+
    "in vec3 viewer_vector;"+
    "in vec3 transformed_normals;"+
    "in vec3 out_phong_ads_color;"+
    "in vec3 light_direction_red;"+
    "in vec3 light_direction_green;"+
    "in vec3 light_direction_blue;"+
    "out vec4 FragColor;"+
    "uniform vec3 u_La_red;"+
    "uniform vec3 u_Ld_red;"+
    "uniform vec3 u_Ls_red;"+
    "uniform vec3 u_La_green;"+
    "uniform vec3 u_Ld_green;"+
    "uniform vec3 u_Ls_green;"+
    "uniform vec3 u_La_blue;"+
    "uniform vec3 u_Ld_blue;"+
    "uniform vec3 u_Ls_blue;"+
    "uniform vec3 u_Ka;"+
    "uniform vec3 u_Kd;"+
    "uniform vec3 u_Ks;"+
    "uniform float u_material_shininess;"+
    "uniform int u_per_vertex_lighting_enabled;"+
    "uniform int u_per_fragment_lighting_enabled;"+
    "vec3 SettingLightProperties(vec3 u_La, vec3 u_Ld, vec3 u_Ls, vec3 light_direction)"+
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
        "if(u_per_vertex_lighting_enabled==1)"+
        "{"+
            "FragColor = vec4(out_phong_ads_color, 1.0);"+
        "}"+
        "if(u_per_fragment_lighting_enabled==1)"+
        "{"+
            "vec3 red_light=SettingLightProperties(u_La_red,u_Ld_red,u_Ls_red,light_direction_red);"+
            "vec3 green_light=SettingLightProperties(u_La_green,u_Ld_green,u_Ls_green,light_direction_green);"+
            "vec3 blue_light=SettingLightProperties(u_La_blue,u_Ld_blue,u_Ls_blue,light_direction_blue);"+
            "vec3 phong_ads_color=red_light+green_light+blue_light;"+
            "FragColor = vec4(phong_ads_color, 1.0);"+
        "}"+
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
    
    gl.bindAttribLocation(shaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_VERTEX,"vPosition");
    gl.bindAttribLocation(shaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_NORMAL,"vNormal");

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

	F_KeyPressed_uniform=gl.getUniformLocation(shaderProgramObject,"u_per_fragment_lighting_enabled");
	V_KeyPressed_uniform=gl.getUniformLocation(shaderProgramObject,"u_per_vertex_lighting_enabled");
    
    La_uniform_red=gl.getUniformLocation(shaderProgramObject,"u_La_red");
    Ld_uniform_red=gl.getUniformLocation(shaderProgramObject,"u_Ld_red");
    Ls_uniform_red=gl.getUniformLocation(shaderProgramObject,"u_Ls_red");
    light_position_uniform_red=gl.getUniformLocation(shaderProgramObject,"u_light_position_red");

    La_uniform_green=gl.getUniformLocation(shaderProgramObject,"u_La_green");
    Ld_uniform_green=gl.getUniformLocation(shaderProgramObject,"u_Ld_green");
    Ls_uniform_green=gl.getUniformLocation(shaderProgramObject,"u_Ls_green");
    light_position_uniform_green=gl.getUniformLocation(shaderProgramObject,"u_light_position_green");

    La_uniform_blue=gl.getUniformLocation(shaderProgramObject,"u_La_blue");
    Ld_uniform_blue=gl.getUniformLocation(shaderProgramObject,"u_Ld_blue");
    Ls_uniform_blue=gl.getUniformLocation(shaderProgramObject,"u_Ls_blue");
	light_position_uniform_blue=gl.getUniformLocation(shaderProgramObject,"u_light_position_blue");
    
	 
    Ka_uniform=gl.getUniformLocation(shaderProgramObject,"u_Ka");
    Kd_uniform=gl.getUniformLocation(shaderProgramObject,"u_Kd");
    Ks_uniform=gl.getUniformLocation(shaderProgramObject,"u_Ks");
    materialShininessUniform=gl.getUniformLocation(shaderProgramObject,"u_material_shininess");
    
    sphere=new Mesh();
    makeSphere(sphere,2.0,30,30);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.CULL_FACE);
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    
    perspectiveProjectionMatrix=mat4.create();
    gbLightPerVertex=false;
    gbLightPerFragment=false;
}

function resize(){
    if(bFullscreen==true){
        canvas.width=window.innerWidth;
        canvas.height=window.innerHeight;
    }
    else{
        canvas.width=canvas_original_width;
        canvas.height=canvas_original_height;
    }
   
    gl.viewport(0, 0, canvas.width, canvas.height);
    
    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width)/parseFloat(canvas.height), 0.1, 100.0);
}

function draw(){
    var modelMatrix=mat4.create();
    var viewMatrix=mat4.create();

    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT | gl.STENCIL_BUFFER_BIT);
    
    gl.useProgram(shaderProgramObject);
    		
	   if(gbLightPerFragment==true){
            gl.uniform1i(F_KeyPressed_uniform, 1);
            gl.uniform1i(V_KeyPressed_uniform, 0);

            gl.uniform3fv(La_uniform_red, light_ambient_red);
            gl.uniform3fv(Ld_uniform_red, light_diffuse_red);
            gl.uniform3fv(Ls_uniform_red, light_specular_red);
            light_position_one[0] = 0.0;
            light_position_one[1] = Math.sin(3.1415 * gAngle / 180.0) * 10.0;
            light_position_one[2] = Math.cos(3.1415 * gAngle / 180.0) * 10.0;
            light_position_one[3] = 1.0;
            gl.uniform4fv(light_position_uniform_red, light_position_one);

            gl.uniform3fv(La_uniform_green, light_ambient_green);
            gl.uniform3fv(Ld_uniform_green, light_diffuse_green);
            gl.uniform3fv(Ls_uniform_green, light_diffuse_green);
            light_position_two[0] = Math.sin(3.1415 * gAngle / 180.0) * 10.0;
            light_position_two[1] = 0.0;
            light_position_two[2] = Math.cos(3.1415 * gAngle / 180.0) * 10.0;
            light_position_two[3] = 1.0;
            gl.uniform4fv(light_position_uniform_green, light_position_two);
            
            gl.uniform3fv(La_uniform_blue, light_ambient_blue);
            gl.uniform3fv(Ld_uniform_blue, light_diffuse_blue);
            gl.uniform3fv(Ls_uniform_blue, light_specular_blue);
            light_position_three[0] = Math.sin(3.1415 * gAngle / 180.0) * 10.0;
            light_position_three[1] = Math.cos(3.1415 * gAngle / 180.0) * 10.0;
            light_position_three[2] = 0.0;
            light_position_three[3] = 1.0;
            gl.uniform4fv(light_position_uniform_blue, light_position_three);
                        
            gl.uniform3fv(Ka_uniform, material_ambient);
            gl.uniform3fv(Kd_uniform, material_diffuse);
            gl.uniform3fv(Ks_uniform, material_specular);
            gl.uniform1f(materialShininessUniform, material_shininess);
        }else{
            gl.uniform1i(F_KeyPressed_uniform, 0);
            gl.uniform1i(V_KeyPressed_uniform, 1);

            gl.uniform3fv(La_uniform_red, light_ambient_red);
            gl.uniform3fv(Ld_uniform_red, light_diffuse_red);
            gl.uniform3fv(Ls_uniform_red, light_specular_red);
            light_position_one[0] = 0.0;
            light_position_one[1] = Math.sin(3.1415 * gAngle / 180.0) * 10.0;
            light_position_one[2] = Math.cos(3.1415 * gAngle / 180.0) * 10.0;
            light_position_one[3] = 1.0;
            gl.uniform4fv(light_position_uniform_red, light_position_one);

            gl.uniform3fv(La_uniform_green, light_ambient_green);
            gl.uniform3fv(Ld_uniform_green, light_diffuse_green);
            gl.uniform3fv(Ls_uniform_green, light_diffuse_green);
            light_position_two[0] = Math.sin(3.1415 * gAngle / 180.0) * 10.0;
            light_position_two[1] = 0.0;
            light_position_two[2] = Math.cos(3.1415 * gAngle / 180.0) * 10.0;
            light_position_two[3] = 1.0;
            gl.uniform4fv(light_position_uniform_green, light_position_two);
            
            gl.uniform3fv(La_uniform_blue, light_ambient_blue);
            gl.uniform3fv(Ld_uniform_blue, light_diffuse_blue);
            gl.uniform3fv(Ls_uniform_blue, light_specular_blue);
            light_position_three[0] = Math.sin(3.1415 * gAngle / 180.0) * 10.0;
            light_position_three[1] = Math.cos(3.1415 * gAngle / 180.0) * 10.0;
            light_position_three[2] = 0.0;
            light_position_three[3] = 1.0;
            gl.uniform4fv(light_position_uniform_blue, light_position_three);
            
            gl.uniform3fv(Ka_uniform, material_ambient);
            gl.uniform3fv(Kd_uniform, material_diffuse);
            gl.uniform3fv(Ks_uniform, material_specular);
            gl.uniform1f(materialShininessUniform, material_shininess);
        }
           
        mat4.translate(modelMatrix, modelMatrix, [0.0,0.0,-6.0]);    
        gl.uniformMatrix4fv(modelMatrixUniform,false,modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform,false,viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform,false,perspectiveProjectionMatrix);

        sphere.draw();   
    
    gl.useProgram(null);
        
	update();
	
    requestAnimationFrame(draw, canvas);
}

function update() {
	gAngle = gAngle + 1.0;
	if (gAngle >= 360.0) {
		gAngle = 0.0;
	}	
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
            toggleFullScreen();
            break;
        case 70://F Key
            if (bIsFKeyPressed==false) {
				gbLightPerVertex = false;
                gbLightPerFragment = true;
				bIsFKeyPressed = true;
			}
			else{
                gbLightPerVertex = false;
				gbLightPerFragment = false;
				bIsFKeyPressed = false;
			}
			break;
		case 86://V Key
			if (bIsVKeyPressed==false) {
				gbLightPerFragment = false;
                gbLightPerVertex = true;
				bIsVKeyPressed = true;
			}
			else {
                gbLightPerFragment = false;
				gbLightPerVertex = false;
				bIsVKeyPressed = false;
			}
			break;
		case 81://Q Key
            uninitialize();
            window.close();
            break;
    }
}


function mouseDown(){
}
