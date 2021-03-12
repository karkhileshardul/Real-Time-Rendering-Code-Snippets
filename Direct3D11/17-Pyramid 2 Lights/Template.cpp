#include<windows.h>
#include<stdio.h>
#include<d3d11.h>
#include<d3dcompiler.h>
#pragma warning(disable:4838)
#include"XNAMath/xnamath.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3dcompiler.lib")

#define WIN_WIDTH	800
#define WIN_HEIGHT	600

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

FILE *gpFile=NULL;
char gszLogFileName[]="SSK_Log.txt";
HWND ghwnd;
bool gbActiveWindow=false;
ID3D11DeviceContext *gpID3D11DeviceContext=NULL;
bool gbEscapeKeyIsPressed=false;
bool gbFullscreen=false;
DWORD dwStyle;
bool bIsLKeyPressed=false;
float gAngle=0.0f;
bool gbAnimate;

WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
IDXGISwapChain *gpIDXGISwapChain=NULL;
ID3D11Device *gpID3D11Device=NULL;
ID3D11VertexShader *gpID3D11VertexShader=NULL;
ID3D11PixelShader *gpID3D11PixelShader=NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBufferPosition_Pyramid=NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBufferNormal_Pyramid=NULL;
ID3D11InputLayout *gpID3D11InputLayout=NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer=NULL;

struct  CBUFFER{
	XMMATRIX WorldViewMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;
	unsigned int  L_KeyPressed_uniform;
	XMVECTOR La_uniform_one;
	XMVECTOR Ld_uniform_one;
	XMVECTOR Ls_uniform_one;
	XMVECTOR light_position_uniform_one;
	XMVECTOR La_uniform_two;
	XMVECTOR Ld_uniform_two;
	XMVECTOR Ls_uniform_two;
	XMVECTOR light_position_uniform_two;
	XMVECTOR Ka_uniform;
	XMVECTOR Kd_uniform;
	XMVECTOR Ks_uniform;
	float material_shininess_uniform;
};

float gClearColor[4];
XMMATRIX gPerspectiveProjectionMatrix;
ID3D11RenderTargetView *gpID3D11RenderTargetView=NULL;
ID3D11RasterizerState *gpID3D11RasterizerState=NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView=NULL;

bool gbLight=false;

float one_lightAmbient[]={0.0f,0.0f,0.0f,1.0f};
float one_lightDiffuse[]={1.0f,0.0f,0.0f,0.0f};
float one_lightSpecular[]={1.0f,1.0f,1.0f,1.0f};
float one_lightPosition[]={100.0f,100.0f,-100.0f,1.0f};

float two_lightAmbient[]={0.0f,0.0f,0.0f,1.0f};
float two_lightDiffuse[]={0.0f,0.0f,1.0f,0.0f};
float two_lightSpecular[]={1.0f,1.0f,1.0f,1.0f};
float two_lightPosition[]={-100.0f,100.0f,-100.0f,1.0f};

float material_ambient[]={0.0f,0.0f,0.0f,1.0f};
float material_diffuse[]={1.0f,1.0f,1.0f,1.0f};
float material_specular[]={1.0f,1.0f,1.0f,1.0f};
float material_shininess=50.0f;


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	HRESULT initialize(void);
	void display(void);
	void uninitialize(void);
	void update(void);
	
	WNDCLASSEX wndclassex;
	bool bDone=false;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[]=TEXT("18-Direct3D11 Pyramid 2 Lights Window");

	if(fopen_s(&gpFile,gszLogFileName,"w")!=0){
		MessageBox(NULL,TEXT("Log File Can Not be Created\nExiting...."),TEXT("Error"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(EXIT_FAILURE);
	}else{
		fprintf_s(gpFile,"Log File Is Successfully Opened..\n");
		fclose(gpFile);
	}

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.hInstance=hInstance;
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.lpszClassName=szClassName;
	wndclassex.lpszMenuName=NULL;
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.style=CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	RegisterClassEx(&wndclassex);

	hwnd=CreateWindow(szClassName,szClassName,WS_OVERLAPPEDWINDOW,
					100,100,WIN_WIDTH,WIN_HEIGHT,
					NULL,NULL,hInstance,NULL);

	ghwnd=hwnd;

	ShowWindow(hwnd,iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	HRESULT hr;
	hr=initialize();
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"initialize() Failed..Exitting Now...\n");
		fclose(gpFile);
		DestroyWindow(hwnd);
		hwnd=NULL;
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"initialize() Succeeded..\n");
		fclose(gpFile);
	}

	while(bDone==false){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
			if(msg.message==WM_QUIT){
				bDone=true;
			}else{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}else{
			update();
			display();
			if(gbActiveWindow==true){
				if(gbEscapeKeyIsPressed==true){
					bDone=true;
				}
			}
		}
	}
	uninitialize();
	return ((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	HRESULT hr;
	HRESULT resize(int,int);
	void ToggleFullscreen(void);
	void uninitialize(void);
	static bool bIsAKeyPressed=false;
	static bool bIsLKeyPressed=false;

	switch(iMsg){
		case WM_ACTIVATE:
			if(HIWORD(wParam)==0){
				gbActiveWindow=true;
			}else{
				gbActiveWindow=false;
			}
			break;
		case WM_SIZE:
			if(gpID3D11DeviceContext){
				hr=resize(LOWORD(lParam),HIWORD(lParam));
				if(FAILED(hr)){
					fopen_s(&gpFile,gszLogFileName,"a+");
					fprintf_s(gpFile,"resize() Failed..\n");
					fclose(gpFile);
					gpFile=NULL;
					return(hr);
				}else{
					fopen_s(&gpFile,gszLogFileName,"a+");
					fprintf_s(gpFile,"resize() Succeeded\n");
					fclose(gpFile);
				}
			}
			break;
		case WM_KEYDOWN:
			switch(wParam){
				case VK_ESCAPE:
					if(gbEscapeKeyIsPressed==false){
						gbEscapeKeyIsPressed=true;
					}
					break;
				case 0x46:
					if(gbFullscreen==false){
						ToggleFullscreen();
						gbFullscreen=true;
					}else{
						ToggleFullscreen();
						gbFullscreen=false;
					}					
					break;
				case 0x41://a
					if(bIsAKeyPressed==false){
						gbAnimate=true;
						bIsAKeyPressed=true;
					}else{
						gbAnimate=false;
						bIsAKeyPressed=false;
					}
				case 0x4C:
					if(bIsLKeyPressed==false){
						bIsLKeyPressed=true;
						gbLight=true;
					}else{
						bIsLKeyPressed=false;
						gbLight=false;
					}
					break;
				default:
					break;			
			}
			break;
		case WM_CLOSE:
			uninitialize();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			break;
	}
	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}

void ToggleFullscreen(void){
	MONITORINFO mi;
	bool bGetWindowPlacement=false;
	bool bGetMonitorInfo=false;

	if(gbFullscreen==false){
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW){
			mi={sizeof(MONITORINFO)};
			bGetWindowPlacement=GetWindowPlacement(ghwnd,&wpPrev);
			bGetMonitorInfo=GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi);
			if(bGetWindowPlacement && bGetMonitorInfo){
				SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,
								mi.rcMonitor.right-mi.rcMonitor.left,
								mi.rcMonitor.bottom-mi.rcMonitor.top,
								SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}else{
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER |
					SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

HRESULT initialize(void){
	void uninitialize(void);
	HRESULT resize(int,int);
	HRESULT hr;

	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[]={D3D_DRIVER_TYPE_HARDWARE,
										D3D_DRIVER_TYPE_WARP,
										D3D_DRIVER_TYPE_REFERENCE,};
	D3D_FEATURE_LEVEL d3dFeatureLevel_required=D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired=D3D_FEATURE_LEVEL_10_0;

	UINT createDeviceFlags=0;
	UINT numDriverTypes=0;
	UINT numFeatureLevels=1;

	numDriverTypes=sizeof(d3dDriverTypes)/sizeof(d3dDriverTypes[0]);

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void *)&dxgiSwapChainDesc,sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferDesc.Width=WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height=WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator=60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator=1;
/*	If we change Quality=1 and Count=4 in
 		texture2ddesc.SampleDesc.Count=4;
		texture2ddesc.SampleDesc.Quality=1;
 	then we have to change Count and Quality for 	
 		dxgiSwapChainDesc.SampleDesc.Count=4;
 		dxgiSwapChainDesc.SampleDesc.Quality=1;
*/
//	dxgiSwapChainDesc.SampleDesc.Count=4;
//	dxgiSwapChainDesc.SampleDesc.Quality=1;
	dxgiSwapChainDesc.SampleDesc.Count=1;
	dxgiSwapChainDesc.SampleDesc.Quality=0;
	dxgiSwapChainDesc.BufferCount=1;
	dxgiSwapChainDesc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow=ghwnd;	
	dxgiSwapChainDesc.Windowed=TRUE;

	for(UINT driverTypeIndex=0; driverTypeIndex < numDriverTypes; driverTypeIndex++){
		d3dDriverType=d3dDriverTypes[driverTypeIndex];
		hr=D3D11CreateDeviceAndSwapChain(NULL,
										d3dDriverType,
										NULL,
										createDeviceFlags,
										&d3dFeatureLevel_required,
										numFeatureLevels,
										D3D11_SDK_VERSION,
										&dxgiSwapChainDesc,
										&gpIDXGISwapChain,
										&gpID3D11Device,
										&d3dFeatureLevel_acquired,
										&gpID3D11DeviceContext);
		if(SUCCEEDED(hr)){
			break;
		}
	}
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"D3D11CreateDeviceAndSwapChain() Failed....\n");
		fclose(gpFile);
		return(hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"D3D11CreateDeviceAndSwapChain() Succeeded..\n");
		fprintf_s(gpFile,"The Choosen Driver is of  ");

		if(d3dDriverType==D3D_DRIVER_TYPE_HARDWARE){
			fprintf_s(gpFile,"Hardware Type\n");
		}else if(d3dDriverType==D3D_DRIVER_TYPE_WARP){
			fprintf_s(gpFile,"Warp Type.\n");
		}else if(d3dDriverType==D3D_DRIVER_TYPE_REFERENCE){
			fprintf_s(gpFile,"Reference type\n");
		}else{
			fprintf_s(gpFile,"Unknown Type\n");
		}

		fprintf_s(gpFile,"The Supported Highest Feature Level is ");
		if(d3dFeatureLevel_acquired==D3D_FEATURE_LEVEL_11_0){
			fprintf_s(gpFile,"11.0\n");
		}else if(d3dFeatureLevel_acquired==D3D_FEATURE_LEVEL_10_1){
			fprintf_s(gpFile,"10.1\n");
		}else if(d3dFeatureLevel_acquired==D3D_FEATURE_LEVEL_10_0){
			fprintf_s(gpFile,"10.0\n");
		}else{
			fprintf_s(gpFile,"Unknown..\n");
		}
		fclose(gpFile);
	}

	const char *vertexShaderSourceCode=
		"cbuffer ConstantBuffer"\
		"{"\
			"float4x4 u_world_matrix;"\
			"float4x4 u_view_matrix;"\
			"float4x4 u_projection_matrix;"\
			"uint u_lighting_enabled;"\
			"float4 u_La_one;"\
			"float4 u_Ld_one;"\
			"float4 u_Ls_one;"\
			"float4 u_light_position_one;"\
			"float4 u_La_two;"\
			"float4 u_Ld_two;"\
			"float4 u_Ls_two;"\
			"float4 u_light_position_two;"\
			"float4 u_Ka;"\
			"float4 u_Kd;"\
			"float4 u_Ks;"\
			"float u_material_shininess;"\
			"uint u_per_vertex_lighting_enabled;"\
			"uint u_per_fragment_lighting_enabled;"\
		"}"\
		"struct vertex_output"\
		"{"\
			"float4 position:SV_POSITION;"\
			"float3 transformed_normals:NORMAL0;"\
			"float3 light_position_one:COLOR1;"\
			"float3 light_position_two:COLOR2;"\
			"float3 view_vector:NORMAL2;"\
			"float4 normal:MY_NORMAL;"\
			"float4 pos:POSITION;"\
		"};"\
		"vertex_output main(float4 pos:POSITION,float4 normal:NORMAL)"\
		"{"\
			"vertex_output output;"\
			"if(u_lighting_enabled==1)"\
			"{"\
				"float4x4 myeye_coordinates=mul(u_view_matrix, u_world_matrix);" \
				"float4 eye_coordinates = mul(myeye_coordinates, pos);" \
				"float4x4 myworldmulview=mul(u_world_matrix,u_view_matrix);"\
				"float3 transformed_normals=mul((float3x3)(myworldmulview),(float3)normal);"\
                "float3 light_direction_one=(float3)(u_light_position_one) - eye_coordinates.xyz;"\
				"float3 light_direction_two=(float3)(u_light_position_two) - eye_coordinates.xyz;"\
				"float3 viewer_vector=(-eye_coordinates.xyz);"\
				"output.transformed_normals=transformed_normals;"\
				"output.light_position_one=light_direction_one;"\
				"output.light_position_two=light_direction_two;"\
				"output.view_vector=viewer_vector;"\
				"output.pos=pos;"\
				"output.normal=normal;"\
			"}"\
			"output.position=mul(u_projection_matrix, mul(u_view_matrix, mul(u_world_matrix,pos)));"\
			"return output;"\
		"}";

	ID3DBlob *pID3DBlob_VertexShaderCode=NULL;
	ID3DBlob *pID3DBlob_Error=NULL;

	hr=D3DCompile(vertexShaderSourceCode,
				lstrlenA(vertexShaderSourceCode)+1,
				"VS",
				NULL,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"main",
				"vs_5_0",
				0,0,
				&pID3DBlob_VertexShaderCode,
				&pID3DBlob_Error);

	if(FAILED(hr)){
		if(pID3DBlob_Error!=NULL){
			fopen_s(&gpFile,gszLogFileName,"a+");
			fprintf_s(gpFile,"D3DCompile() Failed For Vertex Shader:%s.\n",(char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error=NULL;
			return(hr);			
		}
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"D3DCompile() Succeeded For Vertex Shader..\n");
		fclose(gpFile);
	}

	hr=gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
											pID3DBlob_VertexShaderCode->GetBufferSize(),
											NULL,&gpID3D11VertexShader);

	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateVertexShader() Failed\n");
		fclose(gpFile);
		return(hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateVertexShader() Succeded..\n");
		fclose(gpFile);		
	}
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader,0,0);

	const char *pixelShaderSourceCode=
		"cbuffer ConstantBuffer"\
		"{"\
			"float4x4 u_world_matrix;"\
			"float4x4 u_view_matrix;"\
			"float4x4 u_projection_matrix;"\
			"uint u_lighting_enabled;"\
			"float4 u_La_one;"\
			"float4 u_Ld_one;"\
			"float4 u_Ls_one;"\
			"float4 u_light_position_one;"\
			"float4 u_La_two;"\
			"float4 u_Ld_two;"\
			"float4 u_Ls_two;"\
			"float4 u_light_position_two;"\
			"float4 u_Ka;"\
			"float4 u_Kd;"\
			"float4 u_Ks;"\
			"float u_material_shininess;"\
			"uint u_per_vertex_lighting_enabled;"\
			"uint u_per_fragment_lighting_enabled;"\
		"}"\
		"struct vertex_output"\
		"{"\
			"float4 position:SV_POSITION;"\
			"float3 transformed_normals:NORMAL0;"\
			"float3 light_position_one:COLOR1;"\
			"float3 light_position_two:COLOR2;"\
			"float3 view_vector:NORMAL2;"\
			"float4 normal:MY_NORMAL;"\
			"float4 pos:POSITION;"\
		"};"\
		"float4 main(float4 pos:SV_POSITION,vertex_output input):SV_TARGET"\
		"{"\
			"float4 color;"\
			"float4 phong_ads_color_one;"\
			"float4 phong_ads_color_two;"\
			"if(u_lighting_enabled==1)"\
			"{"\
				"float3 normalized_transformed_normals=normalize(input.transformed_normals);"\
				"float3 normalized_light_direction_one=normalize(input.light_position_one);"\
				"float3 normalized_viewer_vector=normalize(input.view_vector);"\
				"float4 ambient1=u_La_one * u_Ka;"\
				"float tn_dot_ld1=max(dot(normalized_transformed_normals, normalized_light_direction_one),0.0);"\
				"float4 diffuse_one=u_Ld_one * u_Kd * tn_dot_ld1;"\
				"float3 reflection_vector1=reflect(-normalized_light_direction_one,normalized_transformed_normals);"\
				"float4 specular_one=u_Ls_one * u_Ks * pow(max(dot(reflection_vector1,normalized_viewer_vector),0.0),u_material_shininess);"\
				"phong_ads_color_one=ambient1 + diffuse_one + specular_one;"\
				"float3 normalized_light_direction_two=normalize(input.light_position_two);"\
				"float4 ambient_two=u_La_two * u_Ka;"\
				"float tn_dot_ld2=max(dot(normalized_transformed_normals, normalized_light_direction_two),0.0);"\
				"float4 diffuse_two=u_Ld_two * u_Kd * tn_dot_ld2;"\
				"float3 reflection_vector2=reflect(-normalized_light_direction_two,normalized_transformed_normals);"\
				"float4 specular_two=u_Ls_two * u_Ks * pow(max(dot(reflection_vector2,normalized_viewer_vector),0.0),u_material_shininess);"\
				"phong_ads_color_two=ambient_two + diffuse_two + specular_two;"\
				"phong_ads_color_one=phong_ads_color_one + phong_ads_color_two;"\
				"color=phong_ads_color_one;"\
			"}"\
			"else"\
			"{"\
				"color=float4(1.0,1.0,1.0,1.0);"\
			"}"\
			"return color;"\
		"}";
		//SV_TARGET==>Target can be depth Buffer,Color Buffer(frameBuffer),Stencil Buffer and so on...

	ID3DBlob *pID3DBlob_PixelShaderCode=NULL;
	pID3DBlob_Error=NULL;

	hr=D3DCompile(pixelShaderSourceCode,
				lstrlenA(pixelShaderSourceCode)+1,
				"PS",
				NULL,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"main",
				"ps_5_0",
				0,0,
				&pID3DBlob_PixelShaderCode,
				&pID3DBlob_Error);

	if(FAILED(hr)){
		if(pID3DBlob_Error!=NULL){
			fopen_s(&gpFile,gszLogFileName,"a+");
			fprintf_s(gpFile,"D3DCompile() Failed For Pixel Shader :%s..\n",
					(char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error=NULL;
			return (hr);			
		}
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"D3DCompile() Succeeded For Pixel Shader..\n");
		fclose(gpFile);
	}

	hr=gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
										pID3DBlob_PixelShaderCode->GetBufferSize(),
										NULL,
										&gpID3D11PixelShader);

	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreatePixelShader() Failed..\n");
		fclose(gpFile);
		return (hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreatePixelShader() Succeeded...\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader,0,0);
	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode=NULL;



	float pyramidVertices[]={
		//front side
		0.0f,1.0f,0.0f,			//top
		1.0f,-1.0f,1.0f,		//left
		-1.0f,-1.0f,1.0f,		//right
	
		//right side
		0.0f,1.0f,0.0f,			//top
		1.0f,-1.0f,1.0f,		//left
		1.0f,-1.0f,-1.0f,		//right

		//back side
		0.0f,1.0f,0.0f,			//top
		1.0f,-1.0f,-1.0f,		//right
		-1.0f,-1.0f,-1.0f,		//left

		//left side
		0.0f,1.0f,0.0f,			//top
		-1.0f,-1.0f,-1.0f,		//left
		-1.0f,-1.0f,1.0f		//right

	};

	float pyramid_normals[] =
	{
    	// front
    	0.0f, 0.447214f, 0.894427f,
    	0.0f, 0.447214f, 0.894427f,
    	0.0f, 0.447214f, 0.894427f,
    
    	// right
    	0.894427f, 0.447214f, 0.0f,
    	0.894427f, 0.447214f, 0.0f,
    	0.894427f, 0.447214f, 0.0f,
	    
    	// back
    	0.0f, 0.447214f, -0.894427f,
    	0.0f, 0.447214f, -0.894427f,
    	0.0f, 0.447214f, -0.894427f,
    
	    // left
	    -0.894427f, 0.447214f, 0.0f,
	    -0.894427f, 0.447214f, 0.0f,
	    -0.894427f, 0.447214f, 0.0f
	};


	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth=sizeof(float)*ARRAYSIZE(pyramidVertices);
	bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	hr=gpID3D11Device->CreateBuffer(&bufferDesc,NULL,&gpID3D11Buffer_VertexBufferPosition_Pyramid);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed for Vertex Buffer Position for Sphere\n");
		fclose(gpFile);
		return(hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer Position for Sphere\n");
		fclose(gpFile);
	}

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferPosition_Pyramid,NULL,
								D3D11_MAP_WRITE_DISCARD,NULL,
								&mappedSubresource);	
	memcpy(mappedSubresource.pData,pyramidVertices,sizeof(pyramidVertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferPosition_Pyramid,NULL);

	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth=sizeof(float)*ARRAYSIZE(pyramid_normals);
	bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	hr=gpID3D11Device->CreateBuffer(&bufferDesc,NULL,&gpID3D11Buffer_VertexBufferNormal_Pyramid);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed for Vertex Buffer Normal for Sphere.\n");
		fclose(gpFile);
		gpFile=NULL;
		return (hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeded for Vertex Buffer Normal for Sphere.\n");
		fclose(gpFile);
	}

	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferNormal_Pyramid,NULL,
								D3D11_MAP_WRITE_DISCARD,NULL,
								&mappedSubresource);	
	memcpy(mappedSubresource.pData,pyramid_normals,sizeof(pyramid_normals));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferNormal_Pyramid,NULL);


	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
	inputElementDesc[0].SemanticName="POSITION";
	inputElementDesc[0].SemanticIndex=0;	//yaa index la tyachaya madhe kitva maanu(0 la 0th Mann)
	inputElementDesc[0].Format=DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot=0;
	inputElementDesc[0].AlignedByteOffset=0;
	inputElementDesc[0].InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate=0;

	inputElementDesc[1].SemanticName="NORMAL";
	inputElementDesc[1].SemanticIndex=0;	//yaa index la tyachaya madhe kitva maanu(1 la 0th Mann)
	inputElementDesc[1].Format=DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].InputSlot=1;		//here value changes as per need that is if NORMAL then 3 and if TEXTURE0 then 4 and so on...
	inputElementDesc[1].AlignedByteOffset=0;
	inputElementDesc[1].InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate=0;

	hr=gpID3D11Device->CreateInputLayout(inputElementDesc,2,
										pID3DBlob_VertexShaderCode->GetBufferPointer(),
										pID3DBlob_VertexShaderCode->GetBufferSize(),
										&gpID3D11InputLayout);

	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateInputLayout() Failed..\n");
		fclose(gpFile);
		gpFile=NULL;
		return(hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateInputLayout() Succedded\n");
		fclose(gpFile);		
	}

	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);
	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;


	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer,sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage=D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth=sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
	hr=gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer,
									nullptr,&gpID3D11Buffer_ConstantBuffer);

	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed For Constant Buffer..\n");
		fclose(gpFile);
		return(hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded For Constant Buffer..\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->VSSetConstantBuffers(0,1,&gpID3D11Buffer_ConstantBuffer);

	gpID3D11DeviceContext->PSSetConstantBuffers(0,1,&gpID3D11Buffer_ConstantBuffer);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc,sizeof(rasterizerDesc));
	rasterizerDesc.AntialiasedLineEnable=FALSE;
	rasterizerDesc.MultisampleEnable=FALSE;
	rasterizerDesc.DepthBias=0;						//Shadow and Wall Example(savali bhinti var disavi mhanun,bias )
	rasterizerDesc.DepthBiasClamp=0.0f;				//Shadow and Wall Example(savali bhinti var disavi mhanun,bias)
	rasterizerDesc.SlopeScaledDepthBias=0.0f;
	rasterizerDesc.CullMode=D3D11_CULL_NONE;		// 3 Option ==>D3D11_CULL_FRONT, D3D11_CULL_BACK, D3D11_CULL_NONE
	rasterizerDesc.DepthClipEnable=TRUE;			
	rasterizerDesc.FillMode=D3D11_FILL_SOLID;		// 2 Option ==>D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	rasterizerDesc.FrontCounterClockwise=FALSE;		
	rasterizerDesc.ScissorEnable=FALSE;

	gpID3D11Device->CreateRasterizerState(&rasterizerDesc,&gpID3D11RasterizerState);
	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);


	gClearColor[0]=0.0f;
	gClearColor[1]=0.0f;
	gClearColor[2]=0.0f;
	gClearColor[3]=1.0f;

	gPerspectiveProjectionMatrix=XMMatrixIdentity();


	hr=resize(WIN_WIDTH,WIN_HEIGHT);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"resize() failed..\n");
		fclose(gpFile);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"resize() Succeeded..\n");
		fclose(gpFile);
	}

	return(S_OK);
}

HRESULT resize(int width,int height){
	HRESULT hr=S_OK;

	if(gpID3D11DepthStencilView){
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView=NULL;
	}

	if(gpID3D11RenderTargetView){
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView=NULL;
	}

	gpIDXGISwapChain->ResizeBuffers(1,width,height,DXGI_FORMAT_R8G8B8A8_UNORM,0);

	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),(LPVOID*)&pID3D11Texture2D_BackBuffer);

	hr=gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer,NULL,&gpID3D11RenderTargetView);	
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateRenderTargetView() Failed..\n");
		fclose(gpFile);
		return(hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateRenderTargetView() Succeeded..\n");
		fclose(gpFile);
	}
	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer=NULL;


	D3D11_TEXTURE2D_DESC texture2ddesc;
	ZeroMemory(&texture2ddesc,sizeof(D3D11_TEXTURE2D_DESC));
	texture2ddesc.Width=(UINT)width;
	texture2ddesc.Height=(UINT)height;
	texture2ddesc.ArraySize=1;
	texture2ddesc.MipLevels=1;

/*	If we change Quality=1 and Count=4 in
 		dxgiSwapChainDesc.SampleDesc.Count=4;
 		dxgiSwapChainDesc.SampleDesc.Quality=1;
 	then we have to change Count and Quality for 
		texture2ddesc.SampleDesc.Count=4;
		texture2ddesc.SampleDesc.Quality=1;
	
*/
/*	texture2ddesc.SampleDesc.Count=4;
	texture2ddesc.SampleDesc.Quality=1;
*/
	texture2ddesc.SampleDesc.Count=1;
	texture2ddesc.SampleDesc.Quality=0;


	texture2ddesc.Format=DXGI_FORMAT_D32_FLOAT;
	texture2ddesc.Usage=D3D11_USAGE_DEFAULT;
	texture2ddesc.BindFlags=D3D11_BIND_DEPTH_STENCIL;
	texture2ddesc.CPUAccessFlags=0;
	texture2ddesc.MiscFlags=0;

	ID3D11Texture2D *pID3D11Texture2D;
	hr=gpID3D11Device->CreateTexture2D(&texture2ddesc,NULL,&pID3D11Texture2D);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateTexture2D() Failed..\n");
		fclose(gpFile);
		return(hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateTexture2D() Succeeded..\n");
		fclose(gpFile);
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthstencilviewdesc;
	ZeroMemory(&depthstencilviewdesc,sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthstencilviewdesc.Format=DXGI_FORMAT_D32_FLOAT;
	depthstencilviewdesc.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr=gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D,&depthstencilviewdesc,&gpID3D11DepthStencilView);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateDepthStencilView() Failed..\n");
		fclose(gpFile);
		return(hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateDepthStencilView() Succeeded..\n");
		fclose(gpFile);
	}
	pID3D11Texture2D->Release();
	pID3D11Texture2D = NULL;


	gpID3D11DeviceContext->OMSetRenderTargets(1,&gpID3D11RenderTargetView,gpID3D11DepthStencilView);

	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX=0;
	d3dViewPort.TopLeftY=0;
	d3dViewPort.Width=(float)width;
	d3dViewPort.Height=(float)height;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	if(width<=height){
		gPerspectiveProjectionMatrix=XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f),(float)height/(float)width,
																0.1f,100.0f);
	}else{
		gPerspectiveProjectionMatrix=XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f),(float)width/(float)height,
																0.1f,100.0f);
	}
	return (hr);
}

void display(void){
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView,gClearColor);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView,D3D11_CLEAR_DEPTH,1.0f,0.0f);

	XMMATRIX worldMatrix=XMMatrixIdentity();
	XMMATRIX viewMatrix=XMMatrixIdentity();
	XMMATRIX wvpMatrix=XMMatrixIdentity();
	XMMATRIX rotationMatrix=XMMatrixIdentity();
	CBUFFER constantBuffer;

	
	UINT stride=sizeof(float)*3;
	UINT offset=0;
	gpID3D11DeviceContext->IASetVertexBuffers(0,1,&gpID3D11Buffer_VertexBufferPosition_Pyramid,&stride,&offset);

	stride=sizeof(float)*3;
	offset=0;
	gpID3D11DeviceContext->IASetVertexBuffers(1,1,&gpID3D11Buffer_VertexBufferNormal_Pyramid,&stride,&offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if(gbLight==true){
		constantBuffer.L_KeyPressed_uniform=1;		
		constantBuffer.La_uniform_one=XMVectorSet(one_lightAmbient[0],one_lightAmbient[1],one_lightAmbient[2],one_lightAmbient[3]);
		constantBuffer.Ld_uniform_one=XMVectorSet(one_lightDiffuse[0],one_lightDiffuse[1],one_lightDiffuse[2],one_lightDiffuse[3]);
		constantBuffer.Ls_uniform_one=XMVectorSet(one_lightSpecular[0],one_lightSpecular[1],one_lightSpecular[2],one_lightSpecular[3]);
		constantBuffer.light_position_uniform_one =XMVectorSet(one_lightPosition[0],one_lightPosition[1],one_lightPosition[2],one_lightPosition[3]);
		
		constantBuffer.La_uniform_two=XMVectorSet(two_lightAmbient[0],two_lightAmbient[1],two_lightAmbient[2],two_lightAmbient[3]);
		constantBuffer.Ld_uniform_two=XMVectorSet(two_lightDiffuse[0],two_lightDiffuse[1],two_lightDiffuse[2],two_lightDiffuse[3]);
		constantBuffer.Ls_uniform_two=XMVectorSet(two_lightSpecular[0],two_lightSpecular[1],two_lightSpecular[2],two_lightSpecular[3]);
		constantBuffer.light_position_uniform_two =XMVectorSet(two_lightPosition[0],two_lightPosition[1],two_lightPosition[2],two_lightPosition[3]);

		constantBuffer.Ka_uniform=XMVectorSet(material_ambient[0],material_ambient[1],material_ambient[2],material_ambient[3]);
		constantBuffer.Kd_uniform=XMVectorSet(material_diffuse[0],material_diffuse[1],material_diffuse[2],material_diffuse[3]);
		constantBuffer.Ks_uniform=XMVectorSet(material_specular[0],material_specular[1],material_specular[2],material_specular[3]);
		constantBuffer.material_shininess_uniform=material_shininess;
	}else{
		constantBuffer.L_KeyPressed_uniform=0;
	}



	worldMatrix=XMMatrixTranslation(0.0f,0.0f,6.0f);
	rotationMatrix=XMMatrixRotationY(XMConvertToRadians(-gAngle));
	worldMatrix=rotationMatrix * worldMatrix;
	wvpMatrix=worldMatrix*viewMatrix;//*gPerspectiveProjectionMatrix;
		
	constantBuffer.WorldViewMatrix=worldMatrix;
	constantBuffer.ViewMatrix=viewMatrix;
	constantBuffer.ProjectionMatrix=gPerspectiveProjectionMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);

		gpID3D11DeviceContext->Draw(12,0);

	gpIDXGISwapChain->Present(0,0);	
}

void uninitialize(void){

	if (gpID3D11DepthStencilView){
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	if(gpID3D11RasterizerState){
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState=NULL;
	}


	if(gpID3D11Buffer_ConstantBuffer){
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer=NULL;
	}

	if(gpID3D11InputLayout){
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout=NULL;
	}


	if(gpID3D11Buffer_VertexBufferPosition_Pyramid){
		gpID3D11Buffer_VertexBufferPosition_Pyramid->Release();
		gpID3D11Buffer_VertexBufferPosition_Pyramid=NULL;
	}

	if(gpID3D11Buffer_VertexBufferNormal_Pyramid){
		gpID3D11Buffer_VertexBufferNormal_Pyramid->Release();
		gpID3D11Buffer_VertexBufferNormal_Pyramid=NULL;
	}

	if(gpID3D11PixelShader){
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader=NULL;
	}

	if(gpID3D11VertexShader){
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader=NULL;
	}

	if(gpID3D11RenderTargetView){
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView=NULL;
	}

	if(gpIDXGISwapChain){
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain=NULL;
	}

	if(gpID3D11DeviceContext){
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext=NULL;
	}

	if(gpID3D11Device){
		gpID3D11Device->Release();
		gpID3D11Device=NULL;
	}

	if(gpFile){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"uninitialize() Succeeded\n");
		fprintf_s(gpFile,"Log File Is Successfully Closed...\n");
		fclose(gpFile);
	}
}

void update(void){
	gAngle=gAngle+0.1f;
	if (gAngle >= 360.0f){
		gAngle = gAngle-360.0f;
	}
}