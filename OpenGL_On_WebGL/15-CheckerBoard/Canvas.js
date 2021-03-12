var canvas = null;
var context = null;
var gl = null;
var bFullScreen = false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros = {
	SSK_ATTRIBUTE_VERTEX:0,
	SSK_ATTRIBUTE_COLOR:1,
	SSK_ATTRIBUTE_NORMAL:2,
	SSK_ATTRIBUTE_TEXTURE0:3
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao_square;
var vbo_square_position;
var vbo_texture;

var vao_square1;
var vbo_square1_position;

var square_texture = 0;
var square_texture1 = 0;

var uniform_texture0_sampler;
var mvpUniform;
var checkImageHeight = 64;
var checkImageWidth = 64;

var checkImage = [];

var perspectiveProjectionMatrix;

var requestAnimationFrame = window.requestAnimationFrame || window.webkitRequestAnimationFrame || window.mozRequestAnimationFrame || winodow.oRequestAnimationFrame || window.msRequestAnimationFrame;

function main()
{
	canvas = document.getElementById("AMC");
	if(!canvas){
		console.log("Obtaining Canvas Failed \n");
	}else{
		console.log("Obtaining Canvas Success \n");
	}

	canvas_original_height = canvas.height;
	canvas_original_width = canvas.width;

	window.addEventListener("keydown",keyDownFunction,false);
	window.addEventListener("click",mouseDownFunction,false);
	window.addEventListener("resize",resize,false);
	init();
	resize();
	draw();
}


function toggleFullScreen(){
	var fullScreen_Element = document.fullscreenElement || document.webkitFullscreenElement || document.mozFullScreenElement || document.msFullscreenElement || null;
	if(fullScreen_Element == null)
	{
		if(canvas.requestFullscreen)
			canvas.requestFullscreen();
		else if(canvas.mozRequestFullScreen)
			canvas.mozRequestFullScreen();
		else if(canvas.webkitRequestFullscreen)
			canvas.webkitRequestFullscreen();
		else if(canvas.msRequestFullscreen)
			canvas.msRequestFullscreen();
		bFullScreen = true;
	}else{
		if(document.exitFullscreen)
			document.exitFullscreen();
		else if(document.mozCancelFullScreen)
			document.mozCancelFullScreen();
		else if(document.webkitExitFullscreen)
			document.webkitExitFullscreen();
		else if(document.msExitFullscreen)
			document.msExitFullscreen
		bFullScreen = false;
	}
}

function init(){
	gl = canvas.getContext("webgl2");
	if(gl == null){
		console.log("Failed to get rendering context  for webgl");
		return;
	}else{
		console.log("Succeed to get rendering context  for webgl");
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
		"gl_Position = u_mvp_matrix * vPosition;"+
		"out_texture0_coord = vTexture0_Coord;"+
	"}";

	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject ,vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);
	if(gl.getShaderParameter(vertexShaderObject,gl.COMPILE_STATUS)==false){
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	var fragmentShaderSourceCode =
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec2 out_texture0_coord;"+
	"uniform highp sampler2D u_texture0_sampler;"+
	"out vec4 frag_color;"+
	"void main(void)"+
	"{"+
	"frag_color = texture(u_texture0_sampler,out_texture0_coord);"+
	"}";

	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject ,fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);
	if(gl.getShaderParameter(fragmentShaderObject,gl.COMPILE_STATUS)==false){
		var error = gl.getShaderInfoLog(fragmentShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	shaderProgramObject = gl.createProgram();
	gl.attachShader(shaderProgramObject,vertexShaderObject);
	gl.attachShader(shaderProgramObject,fragmentShaderObject);

	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.SSK_ATTRIBUTE_VERTEX,"vPosition");
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.SSK_ATTRIBUTE_TEXTURE0,"vTexture0_Coord");

	gl.linkProgram(shaderProgramObject);
	if(!gl.getProgramParameter(shaderProgramObject,gl.LINK_STATUS)){
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if(error.length > 0){
			alert(error);
			uninitialize();
		}
	}

	//Load smiley Textures
	makeCheckImage();
	square_texture = gl.createTexture();

		gl.bindTexture(gl.TEXTURE_2D,square_texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL,true);
		gl.texImage2D(gl.TEXTURE_2D,0,gl.LUMINANCE,checkImageWidth,checkImageHeight,0,gl.LUMINANCE,gl.UNSIGNED_BYTE,checkImage);
		gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_S,gl.REPEAT);
		gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_T,gl.REPEAT);


	mvpUniform = gl.getUniformLocation(shaderProgramObject,"u_mvp_matrix");

	var squareVertices = new Float32Array([
		-1.0,1.0,0.0,
		-1.0,-1.0,0.0,
		 1.0,-1.0,0.0,
		 1.0,1.0,0.0
	]);

	var square1Vertices = new Float32Array([
		1.0,-1.0,0.0,
		1.0,1.0,0.0,
		2.41421,1.0,-1.41421,
		2.41421,-1.0,-1.41421
	]);

	vao_square = gl.createVertexArray();
	gl.bindVertexArray(vao_square);

	vbo_square_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_square_position);
	gl.bufferData(gl.ARRAY_BUFFER,squareVertices,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_VERTEX,3,gl.FLOAT,false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	vbo_texture = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_texture);
	gl.bufferData(gl.ARRAY_BUFFER,4*2*64,gl.DYNAMIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0,2,gl.FLOAT,false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);
	gl.bindVertexArray(null);

	vao_square1 = gl.createVertexArray();
	gl.bindVertexArray(vao_square1);

	vbo_square1_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_square1_position);
	gl.bufferData(gl.ARRAY_BUFFER,square1Vertices,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_VERTEX,3,gl.FLOAT,false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	//vbo_texture = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_texture);
	gl.bufferData(gl.ARRAY_BUFFER,4*2*64,gl.DYNAMIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0,2,gl.FLOAT,false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.SSK_ATTRIBUTE_TEXTURE0);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);
	gl.bindVertexArray(null);

	gl.enable(gl.DEPTH_TEST);

	gl.clearColor(0.0,0.0,0.0,1.0);

	perspectiveProjectionMatrix = mat4.create();
}

function makeCheckImage(){
	var i,j,c=0 ;
	var cnt = 0;
	checkImage = new Uint8Array(64 * 64 * 4);
	for(i=0;i<checkImageHeight;i++){
		for(j=0;j<checkImageWidth;j++){
			var c = (((i&0x8)==0) ^ ((j&0x8)==0)) * 255;

			checkImage[i*checkImageWidth +j + 0] = c;
			checkImage[i*checkImageWidth +j + 1] = c;
			checkImage[i*checkImageWidth +j + 2] = c;
			checkImage[i*checkImageWidth +j + 3] = 255;
		}
	}
}

function resize(){
	if(bFullScreen == true){
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}else{
		canvas.width = canvas_original_width;
		canvas.height = canvas_original_height;
	}
	gl.viewport(0,0,canvas.width,canvas.height);

	mat4.perspective(perspectiveProjectionMatrix,45.0,parseFloat(canvas.width)/parseFloat(canvas.height),0.1,100.0);
}


function draw(){
	gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);
	gl.useProgram(shaderProgramObject);
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix  = mat4.create();
	var squareTextureCoords = new Float32Array([
		0.0,0.0,
		0.0,1.0,
		1.0,1.0,
		1.0,0.0
	]);

	mat4.translate(modelViewMatrix,modelViewMatrix,[-1.5,0.0,-4.0]);

	mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform,false,modelViewProjectionMatrix);

	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_texture);
	gl.bufferData(gl.ARRAY_BUFFER,squareTextureCoords,gl.DYNAMIC_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D,square_texture);
	gl.uniform1i(uniform_texture0_sampler,0);

	gl.bindVertexArray(vao_square);
	gl.drawArrays(gl.TRIANGLE_FAN,0,4);
	gl.bindVertexArray(null);

// tild checkerboard --

	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);

	mat4.translate(modelViewMatrix,modelViewMatrix,[1.0,0.0,-5.0]);

	mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform,false,modelViewProjectionMatrix);

	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_texture);
	gl.bufferData(gl.ARRAY_BUFFER,squareTextureCoords,gl.DYNAMIC_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D,square_texture);
	gl.uniform1i(uniform_texture0_sampler,0);

	gl.bindVertexArray(vao_square1);
	gl.drawArrays(gl.TRIANGLE_FAN,0,4);
	gl.bindVertexArray(null);


//----------------------
	gl.useProgram(null);

	requestAnimationFrame(draw,canvas);
}

function keyDownFunction(event){

	switch(event.keyCode)
	{
		case 27: // Escape
			uninitialize();
			window.close();
			break;
		case 70: //
			toggleFullScreen();
			drawText();
		break;
	}
}

function mouseDownFunction(){
	//alert("mouse is clicked");
}

function uninitialize(){

		if(vao_square)
		{
			gl.deleteVertexArray(vao_square);
			vao_square = null;
		}

		if(vbo_square_position){
			gl.deleteBuffer(vbo_square_position);
			vbo_square_position = null;
		}
		if(vbo_texture){
			gl.deleteBuffer(vbo_texture);
			vbo_texture = null;
		}

		if(shaderProgramObject){
			if(fragmentShaderObject){
					gl.detachShader(shaderProgramObject,fragmentShaderObject);
					gl.deleteShader(fragmentShaderObject);
					fragmentShaderObject = null;
			}
			if(vertexShaderObject){
				gl.detachShader(shaderProgramObject,vertexShaderObject);
				gl.deleteShader(vertexShaderObject);
				vertexShaderObject = null;
			}

			gl.deleteProgram(shaderProgramObject);
			shaderProgramObject = null;
		}
}
