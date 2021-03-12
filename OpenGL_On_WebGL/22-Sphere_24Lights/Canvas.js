var canvas=null;
var gl=null;
var bFullscreen=false;
var canvas_original_width;
var canvas_original_height;

var gbLightPerVertex = false;
var gbLightPerFragment = false;

var bIsLKeyPressed=false;
var bIsVKeyPressed = false;
var bIsFKeyPressed = false;

var gWindowWidth=0;
var gWindowHeight=0;

const WebGLMacros={
	SSK_ATTRIBUTE_VERTEX:0,
	SSK_ATTRIBUTE_COLOR:1,
	SSK_ATTRIBUTE_NORMAL:2,
	SSK_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var light_ambient_red=[0.0,0.0,0.0];
var light_diffuse_red=[1.0,0.0,0.0];
var light_specular_red=[1.0,1.0,1.0];
var light_position_one=[0.0,0.0,0.0,1.0];

var light_ambient_green=[0.0,0.0,0.0];
var light_diffuse_green=[0.0,1.0,0.0];
var light_specular_green=[1.0,1.0,1.0];
var light_position_two=[0.0,0.0,0.0,1.0];

var light_ambient_blue=[0.0,0.0,0.0];
var light_diffuse_blue=[0.0,0.0,1.0];
var light_specular_blue=[1.0,1.0,1.0];
var light_position_three=[0.0,0.0,0.0,1.0];

var material_ambient= [0.0,0.0,0.0];
var material_diffuse= [1.0,1.0,1.0];
var material_specular= [1.0,1.0,1.0];
var material_shininess= 50.0;

var one_material_ambient=[0.0215,0.1745,0.0215];
var one_material_diffuse=[0.07568,0.61424,0.07568];
var one_material_specular=[0.633,0.727811,0.633];
var one_material_shininess=76.8;

var two_material_ambient=[0.135,0.2225,0.1575];
var two_material_diffuse=[0.54,0.89,0.63];
var two_material_specular=[0.316228,0.316228,0.316228];
var two_material_shininess=12.8;

var three_material_ambient=[0.05375,0.05,0.06625];
var three_material_diffuse=[0.18275,0.17,0.22525];
var three_material_specular=[0.332741,0.328634,0.346435];
var three_material_shininess=38.4;

var four_material_ambient=[0.25,0.20725,0.20725];
var four_material_diffuse=[1.0,0.829,0.829];
var four_material_specular=[0.296648,0.296648,0.296648];
var four_material_shininess=11.264;

var five_material_ambient=[0.1745,0.01175,0.01175];
var five_material_diffuse=[0.61424,0.04136,0.04136];
var five_material_specular=[0.727811,0.626959,0.626959];
var five_material_shininess=76.8;

var six_material_ambient=[0.1,0.18725,0.1745];
var six_material_diffuse=[0.396,0.74151,0.69102];
var six_material_specular=[0.297254,0.30829,0.306678];
var six_material_shininess=12.8;

var seven_material_ambient=[0.329412,0.223529,0.027451];
var seven_material_diffuse=[0.780392,0.568627,0.113725];
var seven_material_specular=[0.992157,0.941176,0.807843];
var seven_material_shininess=27.89743616;

var eight_material_ambient=[0.2125,0.1275,0.054];
var eight_material_diffuse=[0.714,0.4284,0.18144];
var eight_material_specular=[0.393548,0.271906,0.166721];
var eight_material_shininess=25.6;

var nine_material_ambient=[0.25,0.25,0.25];
var nine_material_diffuse=[0.4,0.4,0.4];
var nine_material_specular=[0.774597,0.774597,0.774597];
var nine_material_shininess=76.8;

var ten_material_ambient=[0.19125,0.0735,0.0225];
var ten_material_diffuse=[0.7038,0.27048,0.0828];
var ten_material_specular=[0.256777,0.137622,0.086014];
var ten_material_shininess=12.8;

var eleven_material_ambient=[0.24725,0.1995,0.0745];
var eleven_material_diffuse=[0.75164,0.60648,0.22648];
var eleven_material_specular=[0.628281,0.555802,0.366065];
var eleven_material_shininess=51.2;

var twelve_material_ambient=[0.19225,0.19225,0.19225];
var twelve_material_diffuse=[0.50754,0.50754,0.50754];
var twelve_material_specular=[0.508273,0.508273,0.508273];
var twelve_material_shininess=51.2;

var thirteen_material_ambient=[0.0,0.0,0.0];
var thirteen_material_diffuse=[0.01,0.01,0.01];
var thirteen_material_specular=[0.50,0.50,0.50];
var thirteen_material_shininess=32.0;

var fourteen_material_ambient=[0.0,0.1,0.06];
var fourteen_material_diffuse=[0.0,0.50980392,0.50980392];
var fourteen_material_specular=[0.50196078,0.50196078,0.50196078];
var fourteen_material_shininess=32.0;

var fifteen_material_ambient=[0.0,0.0,0.0];
var fifteen_material_diffuse=[0.1,0.35,0.1];
var fifteen_material_specular=[0.45,0.55,0.45];
var fifteen_material_shininess=32.0;

var sixteen_material_ambient=[0.0,0.0,0.0];
var sixteen_material_diffuse=[0.5,0.0,0.0];
var sixteen_material_specular=[0.7,0.6,0.6];
var sixteen_material_shininess=32.0;

var seventeen_material_ambient=[0.0,0.0,0.0];
var seventeen_material_diffuse=[0.55,0.55,0.55];
var seventeen_material_specular=[0.70,0.70,0.70];
var seventeen_material_shininess=32.0;

var eighteen_material_ambient=[0.0,0.0,0.0];
var eighteen_material_diffuse=[0.5,0.5,0.0];
var eighteen_material_specular=[0.60,0.60,0.50];
var eighteen_material_shininess=32.0;

var nineteen_material_ambient=[0.02,0.02,0.02];
var nineteen_material_diffuse=[0.01,0.01,0.01];
var nineteen_material_specular=[0.4,0.4,0.4];
var nineteen_material_shininess=10.0;

var twenty_material_ambient=[0.0,0.05,0.05];
var twenty_material_diffuse=[0.4,0.5,0.5];
var twenty_material_specular=[0.04,0.7,0.7];
var twenty_material_shininess=10.0;

var twentyone_material_ambient=[0.0,0.05,0.0];
var twentyone_material_diffuse=[0.4,0.5,0.4];
var twentyone_material_specular=[0.04,0.7,0.04];
var twentyone_material_shininess=10.0;

var twentytwo_material_ambient=[0.05,0.0,0.0];
var twentytwo_material_diffuse=[0.5,0.4,0.4];
var twentytwo_material_specular=[0.7,0.04,0.04];
var twentytwo_material_shininess=10.0;

var twentythree_material_ambient=[0.05,0.05,0.05];
var twentythree_material_diffuse=[0.5,0.5,0.5];
var twentythree_material_specular=[0.7,0.7,0.7];
var twentythree_material_shininess=10.0;

var twentyfour_material_ambient=[0.05,0.05,0.0];
var twentyfour_material_diffuse=[0.5,0.5,0.4];
var twentyfour_material_specular=[0.7,0.7,0.04];
var twentyfour_material_shininess=10.0;

var sphere=null;

var perspectiveProjectionMatrix;

var modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;
var La_uniform_red, Ld_uniform_red, Ls_uniform_red, light_position_uniform_red;
var La_uniform_green, Ld_uniform_green, Ls_uniform_green, light_position_uniform_green;
var La_uniform_blue, Ld_uniform_blue, Ls_uniform_blue, light_position_uniform_blue;
var Ka_uniform, Kd_uniform, Ks_uniform, materialShininessUniform;

var L_KeyPressed_uniform;

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
		"uniform int u_per_vertex_lighting_enabled, u_per_fragment_lighting_enabled;"+
		"precision highp float;"+
        "in vec4 vPosition;"+
		"in vec3 vNormal;"+
		"uniform mat4 u_model_matrix;"+
		"uniform mat4 u_view_matrix;"+
		"uniform mat4 u_projection_matrix;"+		
		"uniform int u_lighting_enabled;"+
		"uniform vec3 u_La_red, u_Ld_red, u_Ls_red;"+
		"uniform vec3 u_La_green, u_Ld_green, u_Ls_green;"+
		"uniform vec3 u_La_blue, u_Ld_blue, u_Ls_blue;"+
		"uniform vec3 u_Ka;"+
		"uniform vec3 u_Kd;"+
		"uniform vec3 u_Ks;"+
		"uniform vec4 u_light_position_red, u_light_position_green, u_light_position_blue;"+
		"uniform float u_material_shininess;"+
		"out vec3 phong_ads_color;"+
		"out vec3 transformed_normals;"+
		"out vec3 light_direction_red, light_direction_green, light_direction_blue;"+
		"out vec3 viewer_vector;"+
		"vec3 SettingLightProperties(vec3 u_La, vec3 u_Ld, vec3 u_Ls, vec4 u_light_position)"+
		"{"+
			"vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;"+
			"vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
			"vec3 light_direction = normalize(vec3(u_light_position) - eyeCoordinates.xyz);"+
			"float tn_dot_ld = max(dot(transformed_normals, light_direction), 0.0);"+
			"vec3 ambient = u_La * u_Kd;"+
			"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
			"vec3 reflection_vector = reflect(-light_direction, transformed_normals);"+
			"viewer_vector = normalize(-eyeCoordinates.xyz);"+
			"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);"+
			"phong_ads_color = ambient + diffuse + specular;"+
			"return phong_ads_color;"+
		"}"+		 
		"void main(void)"+
		"{"+
		"if (u_lighting_enabled==1)"+
		"{" +
			"if(u_per_vertex_lighting_enabled == 1)"+
			"{"+
				"vec3 red_light = SettingLightProperties(u_La_red, u_Ld_red, u_Ls_red, u_light_position_red);"+
				"vec3 green_light = SettingLightProperties(u_La_green, u_Ld_green, u_Ls_green, u_light_position_green);"+
				"vec3 blue_light = SettingLightProperties(u_La_blue, u_Ld_blue, u_Ls_blue, u_light_position_blue);"+
				"phong_ads_color += red_light + green_light + blue_light;"+
			"}"+
			"else if(u_per_fragment_lighting_enabled == 1)"+
			"{"+
				"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;"+
				"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;"+
				"light_direction_red = vec3(u_light_position_red) - eye_coordinates.xyz;"+
				"light_direction_green= vec3(u_light_position_green) - eye_coordinates.xyz;"+
				"light_direction_blue= vec3(u_light_position_blue) - eye_coordinates.xyz;"+
			"}"+
			"else"+
			"{"+
				"phong_ads_color = vec3(1.0, 1.0, 1.0);"+
			"}"+
		"}"+
		"else"+
		"{"+
			"phong_ads_color = vec3(1.0, 1.0, 1.0);"+
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
		"uniform int u_per_vertex_lighting_enabled;"+
		"uniform int u_per_fragment_lighting_enabled;"+
		"precision highp float;"+
        "in vec3 transformed_normals;"+
		"in vec3 light_direction_red, light_direction_green, light_direction_blue;"+
		"in vec3 viewer_vector;"+
		"in vec3 phong_ads_color;"+
		"out vec4 FragColor;"+
		"uniform vec3 u_La_red, u_Ld_red, u_Ls_red;"+
		"uniform vec3 u_La_green, u_Ld_green, u_Ls_green;"+
		"uniform vec3 u_La_blue, u_Ld_blue, u_Ls_blue;"+
		"uniform vec3 u_Ka;"+
		"uniform vec3 u_Kd;"+
		"uniform vec3 u_Ks;"+
		"uniform float u_material_shininess;"+
		"uniform int u_lighting_enabled;"+		
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
			"phong_ads_color += ambient + diffuse + specular;"+
			"return phong_ads_color;"+
		"}"+
		"void main(void)"+
		"{"+
			"if (u_lighting_enabled == 1)"+
			"{"+
				"if(u_per_vertex_lighting_enabled == 1)"+
				"{"+
					"FragColor = vec4(phong_ads_color, 1.0);"+
				"}"+
				"else if(u_per_fragment_lighting_enabled == 1)"+
				"{"+
					"vec3 phong_ads_color;"+
					"vec3 red_light = SettingLightProperties(u_La_red, u_Ld_red, u_Ls_red, light_direction_red);"+
					"vec3 green_light = SettingLightProperties(u_La_green, u_Ld_green, u_Ls_green, light_direction_green);"+
					"vec3 blue_light = SettingLightProperties(u_La_blue, u_Ld_blue, u_Ls_blue, light_direction_blue);"+
					"phong_ads_color += red_light + green_light + blue_light;"+
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

    modelMatrixUniform=gl.getUniformLocation(shaderProgramObject,"u_model_matrix");
    viewMatrixUniform=gl.getUniformLocation(shaderProgramObject,"u_view_matrix");
    projectionMatrixUniform=gl.getUniformLocation(shaderProgramObject,"u_projection_matrix");

    L_KeyPressed_uniform=gl.getUniformLocation(shaderProgramObject,"u_lighting_enabled");    
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
        gWindowWidth=canvas.width;
        gWindowHeight=canvas.height;
    gl.viewport(0, 0, canvas.width, canvas.height);
    
    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width)/parseFloat(canvas.height), 0.1, 100.0);
}

function draw(){
    var currentWidth=0;
    var currentHeight=0;
    var modelMatrix=mat4.create();
    var viewMatrix=mat4.create();

    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT | gl.STENCIL_BUFFER_BIT);
    
    gl.useProgram(shaderProgramObject);
    		
	   if(bLKeyPressed==true){
            gl.uniform1i(L_KeyPressed_uniform,1);
            if(gbLightPerVertex){
                gl.uniform1i(V_KeyPressed_uniform, 1);
                gl.uniform1i(F_KeyPressed_uniform, 0);
            }else if(gbLightPerFragment){
                gl.uniform1i(F_KeyPressed_uniform, 1);
                gl.uniform1i(V_KeyPressed_uniform, 0);
	        }


            light_position_one[0] = 0.0;
            light_position_one[1] = Math.sin(3.1415 * gAngle / 180.0) * 10.0;
            light_position_one[2] = Math.cos(3.1415 * gAngle / 180.0) * 10.0;
            light_position_one[3] = 1.0;


            light_position_two[0] = Math.sin(3.1415 * gAngle / 180.0) * 10.0;
            light_position_two[1] = 0.0;
            light_position_two[2] = Math.cos(3.1415 * gAngle / 180.0) * 10.0;
            light_position_two[3] = 1.0;
            
            light_position_three[0] = Math.sin(3.1415 * gAngle / 180.0) * 10.0;
            light_position_three[1] = Math.cos(3.1415 * gAngle / 180.0) * 10.0;
            light_position_three[2] = 0.0;
            light_position_three[3] = 1.0;
            
            gl.uniform3fv(La_uniform_red, light_ambient_red);
            gl.uniform3fv(Ld_uniform_red, light_diffuse_red);
            gl.uniform3fv(Ls_uniform_red, light_specular_red);
            gl.uniform1f(light_position_uniform_red, light_position_one);

            gl.uniform3fv(La_uniform_green, light_ambient_green);
            gl.uniform3fv(Ld_uniform_green, light_diffuse_green);
            gl.uniform3fv(Ls_uniform_green, light_specular_green);
            gl.uniform1f(light_position_uniform_green, light_position_two);

            gl.uniform3fv(La_uniform_blue, light_ambient_blue);
            gl.uniform3fv(Ld_uniform_blue, light_diffuse_blue);
            gl.uniform3fv(Ls_uniform_blue, light_specular_blue);
            gl.uniform1f(light_position_uniform_blue, light_position_three);

        }else{
            gl.uniform1i(L_KeyPressed_uniform,0);
        }
           

        mat4.translate(modelMatrix, modelMatrix, [0.0,0.0,-6.0]);    
        gl.uniformMatrix4fv(modelMatrixUniform,false,modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform,false,viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform,false,perspectiveProjectionMatrix);

        gl.viewport(0,0,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,one_material_ambient);
        gl.uniform3fv(Kd_uniform,one_material_diffuse);
        gl.uniform3fv(Ks_uniform,one_material_specular);
        gl.uniform1f(materialShininessUniform,one_material_shininess);
        sphere.draw();
        
        currentWidth=currentWidth+gWindowWidth/6;      

        gl.viewport(currentWidth,0,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,two_material_ambient);
        gl.uniform3fv(Kd_uniform,two_material_diffuse);
        gl.uniform3fv(Ks_uniform,two_material_specular);
        gl.uniform1f(materialShininessUniform,two_material_shininess);
        sphere.draw();
        
        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,0,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,three_material_ambient);
        gl.uniform3fv(Kd_uniform,three_material_diffuse);
        gl.uniform3fv(Ks_uniform,three_material_specular);
        gl.uniform1f(materialShininessUniform,three_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,0,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,four_material_ambient);
        gl.uniform3fv(Kd_uniform,four_material_diffuse);
        gl.uniform3fv(Ks_uniform,four_material_specular);
        gl.uniform1f(materialShininessUniform,four_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,0,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,five_material_ambient);
        gl.uniform3fv(Kd_uniform,five_material_diffuse);
        gl.uniform3fv(Ks_uniform,five_material_specular);
        gl.uniform1f(materialShininessUniform,five_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,0,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,six_material_ambient);
        gl.uniform3fv(Kd_uniform,six_material_diffuse);
        gl.uniform3fv(Ks_uniform,six_material_specular);
        gl.uniform1f(materialShininessUniform,six_material_shininess);
        sphere.draw();

        currentHeight=currentHeight+gWindowHeight/4;
        currentWidth=0;
        gl.viewport(0,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,seven_material_ambient);
        gl.uniform3fv(Kd_uniform,seven_material_diffuse);
        gl.uniform3fv(Ks_uniform,seven_material_specular);
        gl.uniform1f(materialShininessUniform,seven_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,eight_material_ambient);
        gl.uniform3fv(Kd_uniform,eight_material_diffuse);
        gl.uniform3fv(Ks_uniform,eight_material_specular);
        gl.uniform1f(materialShininessUniform,eight_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,nine_material_ambient);
        gl.uniform3fv(Kd_uniform,nine_material_diffuse);
        gl.uniform3fv(Ks_uniform,nine_material_specular);
        gl.uniform1f(materialShininessUniform,nine_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,ten_material_ambient);
        gl.uniform3fv(Kd_uniform,ten_material_diffuse);
        gl.uniform3fv(Ks_uniform,ten_material_specular);
        gl.uniform1f(materialShininessUniform,ten_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,eleven_material_ambient);
        gl.uniform3fv(Kd_uniform,eleven_material_diffuse);
        gl.uniform3fv(Ks_uniform,eleven_material_specular);
        gl.uniform1f(materialShininessUniform,eleven_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,twelve_material_ambient);
        gl.uniform3fv(Kd_uniform,twelve_material_diffuse);
        gl.uniform3fv(Ks_uniform,twelve_material_specular);
        gl.uniform1f(materialShininessUniform,twelve_material_shininess);
        sphere.draw();

        currentHeight=currentHeight+gWindowHeight/4;
        currentWidth=0;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,thirteen_material_ambient);
        gl.uniform3fv(Kd_uniform,thirteen_material_diffuse);
        gl.uniform3fv(Ks_uniform,thirteen_material_specular);
        gl.uniform1f(materialShininessUniform,thirteen_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,fourteen_material_ambient);
        gl.uniform3fv(Kd_uniform,fourteen_material_diffuse);
        gl.uniform3fv(Ks_uniform,fourteen_material_specular);
        gl.uniform1f(materialShininessUniform,fourteen_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,fifteen_material_ambient);
        gl.uniform3fv(Kd_uniform,fifteen_material_diffuse);
        gl.uniform3fv(Ks_uniform,fifteen_material_specular);
        gl.uniform1f(materialShininessUniform,fifteen_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,sixteen_material_ambient);
        gl.uniform3fv(Kd_uniform,sixteen_material_diffuse);
        gl.uniform3fv(Ks_uniform,sixteen_material_specular);
        gl.uniform1f(materialShininessUniform,sixteen_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,seventeen_material_ambient);
        gl.uniform3fv(Kd_uniform,seventeen_material_diffuse);
        gl.uniform3fv(Ks_uniform,seventeen_material_specular);
        gl.uniform1f(materialShininessUniform,seventeen_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,eighteen_material_ambient);
        gl.uniform3fv(Kd_uniform,eighteen_material_diffuse);
        gl.uniform3fv(Ks_uniform,eighteen_material_specular);
        gl.uniform1f(materialShininessUniform,eighteen_material_shininess);
        sphere.draw();

        currentHeight=currentHeight+gWindowHeight/4;
        currentWidth=0;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,nineteen_material_ambient);
        gl.uniform3fv(Kd_uniform,nineteen_material_diffuse);
        gl.uniform3fv(Ks_uniform,nineteen_material_specular);
        gl.uniform1f(materialShininessUniform,nineteen_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,twenty_material_ambient);
        gl.uniform3fv(Kd_uniform,twenty_material_diffuse);
        gl.uniform3fv(Ks_uniform,twenty_material_specular);
        gl.uniform1f(materialShininessUniform,twenty_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,twentyone_material_ambient);
        gl.uniform3fv(Kd_uniform,twentyone_material_diffuse);
        gl.uniform3fv(Ks_uniform,twentyone_material_specular);
        gl.uniform1f(materialShininessUniform,twentyone_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,twentytwo_material_ambient);
        gl.uniform3fv(Kd_uniform,twentytwo_material_diffuse);
        gl.uniform3fv(Ks_uniform,twentytwo_material_specular);
        gl.uniform1f(materialShininessUniform,twentytwo_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,twentythree_material_ambient);
        gl.uniform3fv(Kd_uniform,twentythree_material_diffuse);
        gl.uniform3fv(Ks_uniform,twentythree_material_specular);
        gl.uniform1f(materialShininessUniform,twentythree_material_shininess);
        sphere.draw();

        currentWidth=currentWidth+gWindowWidth/6;

        gl.viewport(currentWidth,currentHeight,gWindowWidth/6,gWindowHeight/4);
        gl.uniform3fv(Ka_uniform,twentyfour_material_ambient);
        gl.uniform3fv(Kd_uniform,twentyfour_material_diffuse);
        gl.uniform3fv(Ks_uniform,twentyfour_material_specular);
        gl.uniform1f(materialShininessUniform,twentyfour_material_shininess);
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
        case 76:
            if(bLKeyPressed==false){
                bLKeyPressed=true;
            }else{
                bLKeyPressed=false;
            }
            break;
        case 70://F Key
            if (!bIsFKeyPressed) {
				gbLightPerVertex = false;
                gbLightPerFragment = true;
				bIsFKeyPressed = true;
			}
			else{
				gbLightPerFragment = false;
				bIsFKeyPressed = false;
			}
			break;
		case 86://V Key
			if (!bIsVKeyPressed) {
				gbLightPerFragment = false;
                gbLightPerVertex = true;
				bIsVKeyPressed = true;
			}
			else {
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
