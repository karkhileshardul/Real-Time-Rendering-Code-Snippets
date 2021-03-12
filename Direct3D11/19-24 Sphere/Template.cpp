#include<windows.h>
#include<stdio.h>
#include<d3d11.h>
#include<d3dcompiler.h>
#include"Sphere.h"
#pragma warning(disable:4838)
#include"XNAMath/xnamath.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"Sphere.lib")
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
bool gbLightPerVertex;
bool gbLightPerFragment;
bool bIsLKeyPressed=false;
float gAngle=0.0f;
unsigned int gWindowWidth=0;
unsigned int gWindowHeight=0;


WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
IDXGISwapChain *gpIDXGISwapChain=NULL;
ID3D11Device *gpID3D11Device=NULL;
ID3D11VertexShader *gpID3D11VertexShader=NULL;
ID3D11PixelShader *gpID3D11PixelShader=NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBufferPosition_Sphere=NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBufferNormal_Sphere=NULL;
ID3D11InputLayout *gpID3D11InputLayout=NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer=NULL;

struct  CBUFFER{
	XMMATRIX WorldViewMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;
	unsigned int  L_KeyPressed_uniform;
	XMVECTOR La_uniform_red;
	XMVECTOR Ld_uniform_red;
	XMVECTOR Ls_uniform_red;
	XMVECTOR light_position_uniform_red;
	XMVECTOR La_uniform_green;
	XMVECTOR Ld_uniform_green;
	XMVECTOR Ls_uniform_green;
	XMVECTOR light_position_uniform_green;
	XMVECTOR La_uniform_blue;
	XMVECTOR Ld_uniform_blue;
	XMVECTOR Ls_uniform_blue;
	XMVECTOR light_position_uniform_blue;		
	XMVECTOR Ka_uniform;
	XMVECTOR Kd_uniform;
	XMVECTOR Ks_uniform;
	float material_shininess;
	unsigned int V_KeyPressed_uniform;
	unsigned int F_KeyPressed_uniform;
};

float gClearColor[4];
XMMATRIX gPerspectiveProjectionMatrix;
ID3D11RenderTargetView *gpID3D11RenderTargetView=NULL;
ID3D11RasterizerState *gpID3D11RasterizerState=NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView=NULL;

bool gbLight=false;
ID3D11Buffer *gpID3D11Buffer_IndexBuffer = NULL;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumElements;
unsigned int gNumVertices;

float lightAmbient_red[]={0.0f,0.0f,0.0f,1.0f};
float lightDiffuse_red[]={1.0f,0.0f,0.0f,1.0f};
float lightSpecular_red[]={1.0f,1.0f,1.0f,1.0f};
float lightPosition_red[]={-2.0f,1.0f,-1.0f,1.0f};

float lightAmbient_green[]={0.0f,0.0f,0.0f,1.0f};
float lightDiffuse_green[]={0.0f,1.0f,0.0f,1.0f};
float lightSpecular_green[]={1.0f,1.0f,1.0f,1.0f};
float lightPosition_green[]={2.0f,1.0f,-1.0f,1.0f};

float lightAmbient_blue[]={0.0f,0.0f,0.0f,1.0f};
float lightDiffuse_blue[]={0.0f,0.0f,1.0f,1.0f};
float lightSpecular_blue[]={1.0f,1.0f,1.0f,1.0f};
float lightPosition_blue[]={0.0f,0.0f,-1.0f,1.0f};

float one_material_ambient[]={0.0215f,0.1745f,0.0215f,1.0f};
float one_material_diffuse[]={0.07568f,0.61424f,0.07568f,1.0f};
float one_material_specular[]={0.633f,0.727811f,0.633f,1.0f};
float one_material_shininess=76.8f;

float two_material_ambient[]={0.135f,0.2225f,0.1575f,1.0f};
float two_material_diffuse[]={0.54f,0.89f,0.63f,1.0f};
float two_material_specular[]={0.316228f,0.316228f,0.316228f,1.0f};
float two_material_shininess=12.8f;

float three_material_ambient[]={0.05375f,0.05f,0.06625f,1.0f};
float three_material_diffuse[]={0.18275f,0.17f,0.22525f,1.0f};
float three_material_specular[]={0.332741f,0.328634f,0.346435f,1.0f};
float three_material_shininess=38.4f;

float four_material_ambient[]={0.25f,0.20725f,0.20725f,1.0f};
float four_material_diffuse[]={1.0f,0.829f,0.829f,1.0f};
float four_material_specular[]={0.296648f,0.296648f,0.296648f,1.0f};
float four_material_shininess=11.264f;

float five_material_ambient[]={0.1745f,0.01175f,0.01175f,1.0f};
float five_material_diffuse[]={0.61424f,0.04136f,0.04136f,1.0f};
float five_material_specular[]={0.727811f,0.626959f,0.626959f,1.0f};
float five_material_shininess=76.8f;

float six_material_ambient[]={0.1f,0.18725f,0.1745f,1.0f};
float six_material_diffuse[]={0.396f,0.74151f,0.69102f,1.0f};
float six_material_specular[]={0.297254f,0.30829f,0.306678f,1.0f};
float six_material_shininess=12.8f;

float seven_material_ambient[]={0.329412f,0.223529f,0.027451f,1.0f};
float seven_material_diffuse[]={0.780392f,0.568627f,0.113725f,1.0f};
float seven_material_specular[]={0.992157f,0.941176f,0.807843f,1.0f};
float seven_material_shininess=27.89743616f;

float eight_material_ambient[]={0.2125f,0.1275f,0.054f,1.0f};
float eight_material_diffuse[]={0.714f,0.4284f,0.18144f,1.0f};
float eight_material_specular[]={0.393548f,0.271906f,0.166721f,1.0f};
float eight_material_shininess=25.6f;

float nine_material_ambient[]={0.25f,0.25f,0.25f,1.0f};
float nine_material_diffuse[]={0.4f,0.4f,0.4f,1.0f};
float nine_material_specular[]={0.774597f,0.774597f,0.774597f,1.0f};
float nine_material_shininess=76.8f;

float ten_material_ambient[]={0.19125f,0.0735f,0.0225f,1.0f};
float ten_material_diffuse[]={0.7038f,0.27048f,0.0828f,1.0f};
float ten_material_specular[]={0.256777f,0.137622f,0.086014f,1.0f};
float ten_material_shininess=12.8f;

float eleven_material_ambient[]={0.24725f,0.1995f,0.0745f,1.0f};
float eleven_material_diffuse[]={0.75164f,0.60648f,0.22648f,1.0f};
float eleven_material_specular[]={0.628281f,0.555802f,0.366065f,1.0f};
float eleven_material_shininess=51.2f;

float twelve_material_ambient[]={0.19225f,0.19225f,0.19225f,1.0f};
float twelve_material_diffuse[]={0.50754f,0.50754f,0.50754f,1.0f};
float twelve_material_specular[]={0.508273f,0.508273f,0.508273f,1.0f};
float twelve_material_shininess=51.2f;

float thirteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
float thirteen_material_diffuse[]={0.01f,0.01f,0.01f,1.0f};
float thirteen_material_specular[]={0.50f,0.50f,0.50f,1.0f};
float thirteen_material_shininess=32.0f;

float fourteen_material_ambient[]={0.0f,0.1f,0.06f,1.0f};
float fourteen_material_diffuse[]={0.0f,0.50980392f,0.50980392f,1.0f};
float fourteen_material_specular[]={0.50196078f,0.50196078f,0.50196078f,1.0f};
float fourteen_material_shininess=32.0f;

float fifteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
float fifteen_material_diffuse[]={0.1f,0.35f,0.1f,1.0f};
float fifteen_material_specular[]={0.45f,0.55f,0.45f,1.0f};
float fifteen_material_shininess=32.0f;

float sixteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
float sixteen_material_diffuse[]={0.5f,0.0f,0.0f,1.0f};
float sixteen_material_specular[]={0.7f,0.6f,0.6f,1.0f};
float sixteen_material_shininess=32.0f;

float seventeen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
float seventeen_material_diffuse[]={0.55f,0.55f,0.55f,1.0f};
float seventeen_material_specular[]={0.70f,0.70f,0.70f,1.0f};
float seventeen_material_shininess=32.0f;

float eighteen_material_ambient[]={0.0f,0.0f,0.0f,1.0f};
float eighteen_material_diffuse[]={0.5f,0.5f,0.0f,1.0f};
float eighteen_material_specular[]={0.60f,0.60f,0.50f,1.0f};
float eighteen_material_shininess=32.0f;

float nineteen_material_ambient[]={0.02f,0.02f,0.02f,1.0f};
float nineteen_material_diffuse[]={0.01f,0.01f,0.01f,1.0f};
float nineteen_material_specular[]={0.4f,0.4f,0.4f,1.0f};
float nineteen_material_shininess=10.0f;

float twenty_material_ambient[]={0.0f,0.05f,0.05f,1.0f};
float twenty_material_diffuse[]={0.4f,0.5f,0.5f,1.0f};
float twenty_material_specular[]={0.04f,0.7f,0.7f,1.0f};
float twenty_material_shininess=10.0f;

float twentyone_material_ambient[]={0.0f,0.05f,0.0f,1.0f};
float twentyone_material_diffuse[]={0.4f,0.5f,0.4f,1.0f};
float twentyone_material_specular[]={0.04f,0.7f,0.04f,1.0f};
float twentyone_material_shininess=10.0f;

float twentytwo_material_ambient[]={0.05f,0.0f,0.0f,1.0f};
float twentytwo_material_diffuse[]={0.5f,0.4f,0.4f,1.0f};
float twentytwo_material_specular[]={0.7f,0.04f,0.04f,1.0f};
float twentytwo_material_shininess=10.0f;

float twentythree_material_ambient[]={0.05f,0.05f,0.05f,1.0f};
float twentythree_material_diffuse[]={0.5f,0.5f,0.5f,1.0f};
float twentythree_material_specular[]={0.7f,0.7f,0.7f,1.0f};
float twentythree_material_shininess=10.0f;

float twentyfour_material_ambient[]={0.05f,0.05f,0.0f,1.0f};
float twentyfour_material_diffuse[]={0.5f,0.5f,0.4f,1.0f};
float twentyfour_material_specular[]={0.7f,0.7f,0.04f,1.0f};
float twentyfour_material_shininess=10.0f;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	HRESULT initialize(void);
	void display(void);
	void uninitialize(void);
	void update(void);
	
	WNDCLASSEX wndclassex;
	bool bDone=false;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[]=TEXT("17-Direct3D11 per vertex and per pixel Sphere Window");
	


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
	//static bool bIsLKeyPressed=false;
	
	static bool bIsVKeyPressed=false;
	static bool bIsFKeyPressed = false;

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
				case 0x51:
					if(gbEscapeKeyIsPressed==false){
						gbEscapeKeyIsPressed=true;
					}
					break;
				case VK_ESCAPE:
					if(gbFullscreen==false){
						ToggleFullscreen();
						gbFullscreen=true;
					}else{
						ToggleFullscreen();
						gbFullscreen=false;
					}
					break;
				case 0x4C://L for Light
					if(bIsLKeyPressed==false){
						gbLightPerVertex = true;
						bIsLKeyPressed = true;
						gbLightPerFragment = false;
					}else{
						gbLightPerVertex = false;
						bIsLKeyPressed = false;
						gbLightPerFragment = false;
					}
					break;
				case 0x56: //'V' for Vertex
					if (bIsVKeyPressed == false ){
						gbLightPerVertex = true;
						gbLightPerFragment = false;
						bIsVKeyPressed = true;
					}else{
						gbLightPerVertex = false;
						gbLightPerFragment = true;
						bIsVKeyPressed = false;
					}
					break;
				case 0x46: //'F' for Fragment
					if (bIsFKeyPressed == false ){
						gbLightPerFragment = true;
						gbLightPerVertex = false;
						bIsFKeyPressed = true;
					}else{
						gbLightPerFragment = false;
						gbLightPerVertex = true;
						bIsFKeyPressed = false;
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
			"float4 u_La_red;"\
			"float4 u_Ld_red;"\
			"float4 u_Ls_red;"\
			"float4 u_light_position_red;"\
			"float4 u_La_green;"\
			"float4 u_Ld_green;"\
			"float4 u_Ls_green;"\
			"float4 u_light_position_green;"\
			"float4 u_La_blue;"\
			"float4 u_Ld_blue;"\
			"float4 u_Ls_blue;"\
			"float4 u_light_position_blue;"\
			"float4 u_Ka;"\
			"float4 u_Kd;"\
			"float4 u_Ks;"\
			"float u_material_shininess;"\
			"uint u_per_vertex_lighting_enabled;"\
			"uint u_per_fragment_lighting_enabled;"\
		"}"\
		"float4 SettingLightProperties(float4 La,float4 Ld,float4 Ls,float4 light_position,float4 pos,float3 normal);"\
		"struct vertex_output"\
		"{"\
			"float4 position:SV_POSITION;"\
			"float3 transformed_normals:NORMAL0;"\
			"float3 light_position_red:COLOR1;"\
			"float3 light_position_green:COLOR2;"\
			"float3 light_position_blue:COLOR3;"\
			"float3 view_vector:NORMAL2;"\
			"float4 phong_ads_color:COLOR;"\
			"float4 normal:MY_NORMAL;"\
			"float4 pos:POSITION;"\
		"};"\
		"vertex_output main(float4 pos:POSITION,float4 normal:NORMAL)"\
		"{"\
			"vertex_output output;"\
			"if(u_lighting_enabled==1)"\
			"{"\
				"if(u_per_vertex_lighting_enabled==1)"\
				"{"\
					"float4 red_light=SettingLightProperties(u_La_red,u_Ld_red,u_Ls_red,u_light_position_red,pos,(float3)normal);"\
					"float4 green_light=SettingLightProperties(u_La_green,u_Ld_green,u_Ls_green,u_light_position_green,pos,(float3)normal);"\
					"float4 blue_light=SettingLightProperties(u_La_blue,u_Ld_blue,u_Ls_blue,u_light_position_blue,pos,(float3)normal);"\
					"output.phong_ads_color=red_light + green_light + blue_light;"\
				"}"\
				"if(u_per_fragment_lighting_enabled==1)"\
				"{"\
					"float4x4 myeye_coordinates=mul(u_view_matrix, u_world_matrix);" \
					"float4 eye_coordinates = mul(myeye_coordinates, pos);" \
					"float4x4 myworldmulview=mul(u_world_matrix,u_view_matrix);"\
					"float3 transformed_normals=mul((float3x3)(myworldmulview),(float3)normal);"\
                   	"float3 light_position_red=(float3)(u_light_position_red) - eye_coordinates.xyz;"\
					"float3 light_position_green=(float3)(u_light_position_green) - eye_coordinates.xyz;"\
					"float3 light_position_blue=(float3)(u_light_position_blue) - eye_coordinates.xyz;"\
					"float3 viewer_vector=(-eye_coordinates.xyz);"\
					"output.transformed_normals=transformed_normals;"
					"output.light_position_red=light_position_red;"\
					"output.light_position_green=light_position_green;"\
					"output.light_position_blue=light_position_blue;"\
					"output.view_vector=viewer_vector;"\
					"output.pos=pos;"\
					"output.normal=normal;"\
				"}"\
			"}"\
			"else"\
			"{"\
				"output.phong_ads_color=float4(1.0,1.0,1.0,1.0);"\
			"}"\
			"output.position=mul(u_projection_matrix, mul(u_view_matrix, mul(u_world_matrix,pos)));"\
			"return output;"\
		"}"\
		"float4 SettingLightProperties(float4 La,float4 Ld,float4 Ls,float4 light_position,float4 pos,float3 normal)"\
		"{"\
			"float4 phong_ads_color;"\
			"float4x4 myeye_coordinates=mul(u_view_matrix, u_world_matrix);" \
			"float4 eye_coordinates = mul(myeye_coordinates, pos);" \
			"float4x4 myworldmulview=mul(u_world_matrix,u_view_matrix);"\
			"float3 transformed_normals=normalize(mul((float3x3)(myworldmulview),(float3)normal));"\
            "float3 light_direction=normalize(((float3)(light_position) - eye_coordinates.xyz));"\
			"float tn_dot_ld=max(dot(transformed_normals,light_direction),0.0);"\
			"float4 ambient= La * u_Ka;"\
			"float4 diffuse= Ld * u_Kd * tn_dot_ld;"\
			"float3 reflection_vector=reflect(-light_direction,transformed_normals);"\
			"float3 viewer_vector=normalize(-eye_coordinates.xyz);"\
			"float4 specular=Ls * u_Ks * pow(max(dot(reflection_vector,viewer_vector),0.0),u_material_shininess);"\
			"phong_ads_color=(float4)ambient + diffuse + specular;"\
			"return phong_ads_color;"\
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
			"float4 u_La_red;"\
			"float4 u_Ld_red;"\
			"float4 u_Ls_red;"\
			"float4 u_light_position_red;"\
			"float4 u_La_green;"\
			"float4 u_Ld_green;"\
			"float4 u_Ls_green;"\
			"float4 u_light_position_green;"\
			"float4 u_La_blue;"\
			"float4 u_Ld_blue;"\
			"float4 u_Ls_blue;"\
			"float4 u_light_position_blue;"\
			"float4 u_Ka;"\
			"float4 u_Kd;"\
			"float4 u_Ks;"\
			"float u_material_shininess;"\
			"uint u_per_vertex_lighting_enabled;"\
			"uint u_per_fragment_lighting_enabled;"\
		"}"\
		"float4 SettingLightProperties(float4 La,float4 Ld,float4 Ls,float4 light_position,float4 pos,float3 normal);"\
		"struct vertex_output"\
		"{"\
			"float4 position:SV_POSITION;"\
			"float3 transformed_normals:NORMAL0;"\
			"float3 light_position_red:COLOR1;"\
			"float3 light_position_green:COLOR2;"\
			"float3 light_position_blue:COLOR3;"\
			"float3 view_vector:NORMAL2;"\
			"float4 phong_ads_color:COLOR;"\
			"float4 normal:MY_NORMAL;"\
			"float4 pos:POSITION;"\
		"};"\
		"float4 main(float4 pos:SV_POSITION,vertex_output input):SV_TARGET"\
		"{"\
			"float4 color;"\
			"if(u_lighting_enabled==1)"\
			"{"\
				"if(u_per_vertex_lighting_enabled==1)"\
				"{"\
					"color=input.phong_ads_color;"\
				"}"\
				"else if(u_per_fragment_lighting_enabled==1)"\
				"{"\
					"float4 red_light=SettingLightProperties(u_La_red,u_Ld_red,u_Ls_red,u_light_position_red,input.pos,input.normal);"\
					"float4 green_light=SettingLightProperties(u_La_green,u_Ld_green,u_Ls_green,u_light_position_green,input.pos,input.normal);"\
					"float4 blue_light=SettingLightProperties(u_La_blue,u_Ld_blue,u_Ls_blue,u_light_position_blue,input.pos,input.normal);"\
					"color=red_light + green_light + blue_light;"\
				"}"\
			"}"\
			"else"\
			"{"\
				"color=float4(1.0,1.0,1.0,1.0);"\
			"}"\
			"return color;"\
		"}"\
		"float4 SettingLightProperties(float4 La,float4 Ld,float4 Ls,float4 light_position,float4 pos,float3 normal)"\
		"{"\
			"float4 phong_ads_color;"\
			"float4x4 myeye_coordinates=mul(u_view_matrix, u_world_matrix);" \
			"float4 eye_coordinates = mul(myeye_coordinates, pos);" \
			"float4x4 myworldmulview=mul(u_world_matrix,u_view_matrix);"\
			"float3 transformed_normals=normalize(mul((float3x3)(myworldmulview),(float3)normal));"\
            "float3 light_direction=normalize(((float3)(light_position) - eye_coordinates.xyz));"\
			"float tn_dot_ld=max(dot(transformed_normals,light_direction),0.0);"\
			"float4 ambient= La * u_Ka;"\
			"float4 diffuse= Ld * u_Kd * tn_dot_ld;"\
			"float3 reflection_vector=reflect(-light_direction,transformed_normals);"\
			"float3 viewer_vector=normalize(-eye_coordinates.xyz);"\
			"float4 specular=Ls * u_Ks * pow(max(dot(reflection_vector,viewer_vector),0.0),u_material_shininess);"\
			"phong_ads_color=(float4)ambient + diffuse + specular;"\
			"return phong_ads_color;"\
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


    getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();


	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth=sizeof(float)*ARRAYSIZE(sphere_vertices);
//	bufferDesc.ByteWidth=gNumVertices*sizeof(float);
	bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	hr=gpID3D11Device->CreateBuffer(&bufferDesc,NULL,&gpID3D11Buffer_VertexBufferPosition_Sphere);
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
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferPosition_Sphere,NULL,
								D3D11_MAP_WRITE_DISCARD,NULL,
								&mappedSubresource);	
	memcpy(mappedSubresource.pData,sphere_vertices,sizeof(sphere_vertices));
//	memcpy(mappedSubresource.pData,sphere_vertices,gNumVertices* sizeof(float));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferPosition_Sphere,NULL);

	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth=sizeof(float)*ARRAYSIZE(sphere_normals);
//	bufferDesc.ByteWidth=sizeof(float)*gNumVertices;
	bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	hr=gpID3D11Device->CreateBuffer(&bufferDesc,NULL,&gpID3D11Buffer_VertexBufferNormal_Sphere);
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
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferNormal_Sphere,NULL,
								D3D11_MAP_WRITE_DISCARD,NULL,
								&mappedSubresource);	
	memcpy(mappedSubresource.pData,sphere_normals,sizeof(sphere_normals));
//	memcpy(mappedSubresource.pData,sphere_normals,gNumVertices*sizeof(float));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferNormal_Sphere,NULL);

/*New Code IB for Elements  ___START___*/
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // write access access by CPU and GPU
	bufferDesc.ByteWidth = gNumElements * sizeof(unsigned short);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allow CPU to write into this buffer
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_IndexBuffer);
	if (FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed for Index Buffer for Sphere.\n");
		fclose(gpFile);
		gpFile=NULL;
		return (hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeded for Index Buffer  for Sphere.\n");
		fclose(gpFile);
	}

	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_IndexBuffer, NULL,
							 D3D11_MAP_WRITE_DISCARD, NULL,
							 &mappedSubresource); // map buffer
	memcpy(mappedSubresource.pData, sphere_elements, gNumElements * sizeof(unsigned short));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_IndexBuffer, NULL); // unmap buffer

/*New Code IB for Elements  ___END___*/

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

	gbLightPerVertex = false;
	gbLightPerFragment = false;


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


	gWindowWidth=width;
	gWindowHeight=height;

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
	unsigned int currentWidth=0;
	unsigned int currentHeight=0;
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView,gClearColor);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView,D3D11_CLEAR_DEPTH,1.0f,0.0f);

	D3D11_VIEWPORT d3dViewPort;
	XMMATRIX worldMatrix=XMMatrixIdentity();
	XMMATRIX viewMatrix=XMMatrixIdentity();
	XMMATRIX wvpMatrix=XMMatrixIdentity();
	CBUFFER constantBuffer;

	
	UINT stride=sizeof(float)*3;
	UINT offset=0;
	gpID3D11DeviceContext->IASetVertexBuffers(0,1,&gpID3D11Buffer_VertexBufferPosition_Sphere,&stride,&offset);

	stride=sizeof(float)*3;
	offset=0;
	gpID3D11DeviceContext->IASetVertexBuffers(1,1,&gpID3D11Buffer_VertexBufferNormal_Sphere,&stride,&offset);

	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer, DXGI_FORMAT_R16_UINT, 0); // R16 maps with 'short'

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	if(gbLightPerVertex==true){
		constantBuffer.L_KeyPressed_uniform=1;	
		constantBuffer.V_KeyPressed_uniform=1;
		constantBuffer.F_KeyPressed_uniform=0;

		constantBuffer.La_uniform_red=XMVectorSet(lightAmbient_red[0], lightAmbient_red[1],lightAmbient_red[2], lightAmbient_red[3]);
		constantBuffer.Ld_uniform_red=XMVectorSet(lightDiffuse_red[0],lightDiffuse_red[1],lightDiffuse_red[2],lightDiffuse_red[3]);
		constantBuffer.Ls_uniform_red=XMVectorSet(lightSpecular_red[0],lightSpecular_red[1],lightSpecular_red[2],lightSpecular_red[3]);
		lightPosition_red[0]=cos(3.1415*gAngle/180.0f)*10.f;
		lightPosition_red[1]=0.0f;
		lightPosition_red[2]=sin(3.1415*gAngle/180.0f)*10.f;
		constantBuffer.light_position_uniform_red=XMVectorSet(lightPosition_red[0],lightPosition_red[1],lightPosition_red[2],lightPosition_red[3]);
		
		constantBuffer.La_uniform_green=XMVectorSet(lightAmbient_green[0], lightAmbient_green[1],lightAmbient_green[2], lightAmbient_green[3]);
		constantBuffer.Ld_uniform_green=XMVectorSet(lightDiffuse_green[0],lightDiffuse_green[1],lightDiffuse_green[2],lightDiffuse_green[3]);
		constantBuffer.Ls_uniform_green=XMVectorSet(lightSpecular_green[0],lightSpecular_green[1],lightSpecular_green[2],lightSpecular_green[3]);
		lightPosition_green[0]=0.0f;
		lightPosition_green[1]=cos(3.1415*gAngle/180.0f)*10.f;
		lightPosition_green[2]=sin(3.1415*gAngle/180.0f)*10.f;
		constantBuffer.light_position_uniform_green=XMVectorSet(lightPosition_green[0],lightPosition_green[1],lightPosition_green[2],lightPosition_green[3]);
	
		constantBuffer.La_uniform_blue=XMVectorSet(lightAmbient_blue[0], lightAmbient_blue[1],lightAmbient_blue[2], lightAmbient_blue[3]);
		constantBuffer.Ld_uniform_blue=XMVectorSet(lightDiffuse_blue[0],lightDiffuse_blue[1],lightDiffuse_blue[2],lightDiffuse_blue[3]);
		constantBuffer.Ls_uniform_blue=XMVectorSet(lightSpecular_blue[0],lightSpecular_blue[1],lightSpecular_blue[2],lightSpecular_blue[3]);
		lightPosition_blue[0]=cos(3.1415*gAngle/180.0f)*10.f;
		lightPosition_blue[1]=sin(3.1415*gAngle/180.0f)*10.f;
		lightPosition_blue[2]=0.0f;
		constantBuffer.light_position_uniform_blue=XMVectorSet(lightPosition_blue[0],lightPosition_blue[1],lightPosition_blue[2],lightPosition_blue[3]);

	}else if(gbLightPerFragment==true){
		constantBuffer.L_KeyPressed_uniform=1;		
		constantBuffer.V_KeyPressed_uniform=0;
		constantBuffer.F_KeyPressed_uniform=1;
		
		constantBuffer.La_uniform_red=XMVectorSet(lightAmbient_red[0], lightAmbient_red[1],lightAmbient_red[2], lightAmbient_red[3]);
		constantBuffer.Ld_uniform_red=XMVectorSet(lightDiffuse_red[0],lightDiffuse_red[1],lightDiffuse_red[2],lightDiffuse_red[3]);
		constantBuffer.Ls_uniform_red=XMVectorSet(lightSpecular_red[0],lightSpecular_red[1],lightSpecular_red[2],lightSpecular_red[3]);
		lightPosition_red[0]=cos(3.1415*gAngle/180.0f)*10.f;
		lightPosition_red[1]=0.0f;
		lightPosition_red[2]=sin(3.1415*gAngle/180.0f)*10.f;
		constantBuffer.light_position_uniform_red=XMVectorSet(lightPosition_red[0],lightPosition_red[1],lightPosition_red[2],lightPosition_red[3]);
		
		constantBuffer.La_uniform_green=XMVectorSet(lightAmbient_green[0], lightAmbient_green[1],lightAmbient_green[2], lightAmbient_green[3]);
		constantBuffer.Ld_uniform_green=XMVectorSet(lightDiffuse_green[0],lightDiffuse_green[1],lightDiffuse_green[2],lightDiffuse_green[3]);
		constantBuffer.Ls_uniform_green=XMVectorSet(lightSpecular_green[0],lightSpecular_green[1],lightSpecular_green[2],lightSpecular_green[3]);
		lightPosition_green[0]=0.0f;
		lightPosition_green[1]=cos(3.1415*gAngle/180.0f)*10.f;
		lightPosition_green[2]=sin(3.1415*gAngle/180.0f)*10.f;
		constantBuffer.light_position_uniform_green=XMVectorSet(lightPosition_green[0],lightPosition_green[1],lightPosition_green[2],lightPosition_green[3]);
	
		constantBuffer.La_uniform_blue=XMVectorSet(lightAmbient_blue[0], lightAmbient_blue[1],lightAmbient_blue[2], lightAmbient_blue[3]);
		constantBuffer.Ld_uniform_blue=XMVectorSet(lightDiffuse_blue[0],lightDiffuse_blue[1],lightDiffuse_blue[2],lightDiffuse_blue[3]);
		constantBuffer.Ls_uniform_blue=XMVectorSet(lightSpecular_blue[0],lightSpecular_blue[1],lightSpecular_blue[2],lightSpecular_blue[3]);
		lightPosition_blue[0]=cos(3.1415*gAngle/180.0f)*10.f;
		lightPosition_blue[1]=sin(3.1415*gAngle/180.0f)*10.f;
		lightPosition_blue[2]=0.0f;
		constantBuffer.light_position_uniform_blue=XMVectorSet(lightPosition_blue[0],lightPosition_blue[1],lightPosition_blue[2],lightPosition_blue[3]);

	}
	else{
		constantBuffer.L_KeyPressed_uniform=0;
		constantBuffer.V_KeyPressed_uniform=0;
		constantBuffer.F_KeyPressed_uniform=0;		
	}



	worldMatrix=XMMatrixTranslation(0.0f,0.0f,2.0f);
	
	wvpMatrix=worldMatrix*viewMatrix;//*gPerspectiveProjectionMatrix;
	
	constantBuffer.WorldViewMatrix=worldMatrix;
	constantBuffer.ViewMatrix=viewMatrix;
	constantBuffer.ProjectionMatrix=gPerspectiveProjectionMatrix;

/*First Row ____START____*/
	d3dViewPort.TopLeftX=0;
	d3dViewPort.TopLeftY=0;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;

	constantBuffer.Ka_uniform=XMVectorSet(one_material_ambient[0],one_material_ambient[1],one_material_ambient[2],one_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(one_material_diffuse[0],one_material_diffuse[1],one_material_diffuse[2],one_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(one_material_specular[0],one_material_specular[1],one_material_specular[2],one_material_specular[3]);
	constantBuffer.material_shininess=one_material_shininess;
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth =currentWidth+ gWindowWidth / 6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=0;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;

	constantBuffer.Ka_uniform=XMVectorSet(two_material_ambient[0],two_material_ambient[1],two_material_ambient[2],two_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(two_material_diffuse[0],two_material_diffuse[1],two_material_diffuse[2],two_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(two_material_specular[0],two_material_specular[1],two_material_specular[2],two_material_specular[3]);
	constantBuffer.material_shininess=two_material_shininess;

	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);


	currentWidth =currentWidth+ gWindowWidth / 6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=0;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;

	constantBuffer.Ka_uniform=XMVectorSet(three_material_ambient[0],three_material_ambient[1],three_material_ambient[2],three_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(three_material_diffuse[0],three_material_diffuse[1],three_material_diffuse[2],three_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(three_material_specular[0],three_material_specular[1],three_material_specular[2],three_material_specular[3]);
	constantBuffer.material_shininess=three_material_shininess;

	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth =currentWidth+ gWindowWidth / 6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=0;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;

	constantBuffer.Ka_uniform=XMVectorSet(four_material_ambient[0],four_material_ambient[1],four_material_ambient[2],four_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(four_material_diffuse[0],four_material_diffuse[1],four_material_diffuse[2],four_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(four_material_specular[0],four_material_specular[1],four_material_specular[2],four_material_specular[3]);
	constantBuffer.material_shininess=four_material_shininess;

	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth =currentWidth+ gWindowWidth / 6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=0;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;

	constantBuffer.Ka_uniform=XMVectorSet(five_material_ambient[0],five_material_ambient[1],five_material_ambient[2],five_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(five_material_diffuse[0],five_material_diffuse[1],five_material_diffuse[2],five_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(five_material_specular[0],five_material_specular[1],five_material_specular[2],five_material_specular[3]);
	constantBuffer.material_shininess=five_material_shininess;

	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);


	currentWidth =currentWidth+ gWindowWidth / 6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=0;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;

	constantBuffer.Ka_uniform=XMVectorSet(six_material_ambient[0],six_material_ambient[1],six_material_ambient[2],six_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(six_material_diffuse[0],six_material_diffuse[1],six_material_diffuse[2],six_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(six_material_specular[0],six_material_specular[1],six_material_specular[2],six_material_specular[3]);
	constantBuffer.material_shininess=six_material_shininess;

	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
/*First Row ____END____*/

/*Second Row ____START____*/

	currentHeight=currentHeight+gWindowHeight/4;
	currentWidth=0;
	
	d3dViewPort.TopLeftX=0;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;

	constantBuffer.Ka_uniform=XMVectorSet(seven_material_ambient[0],seven_material_ambient[1],seven_material_ambient[2],seven_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(seven_material_diffuse[0],seven_material_diffuse[1],seven_material_diffuse[2],seven_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(seven_material_specular[0],seven_material_specular[1],seven_material_specular[2],seven_material_specular[3]);
	constantBuffer.material_shininess=seven_material_shininess;

	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth =currentWidth+ gWindowWidth / 6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;

	constantBuffer.Ka_uniform=XMVectorSet(eight_material_ambient[0],eight_material_ambient[1],eight_material_ambient[2],eight_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(eight_material_diffuse[0],eight_material_diffuse[1],eight_material_diffuse[2],eight_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(eight_material_specular[0],eight_material_specular[1],eight_material_specular[2],eight_material_specular[3]);
	constantBuffer.material_shininess=eight_material_shininess;

	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth =currentWidth+ gWindowWidth / 6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;

	constantBuffer.Ka_uniform=XMVectorSet(nine_material_ambient[0],nine_material_ambient[1],nine_material_ambient[2],nine_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(nine_material_diffuse[0],nine_material_diffuse[1],nine_material_diffuse[2],nine_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(nine_material_specular[0],nine_material_specular[1],nine_material_specular[2],nine_material_specular[3]);
	constantBuffer.material_shininess=nine_material_shininess;

	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth =currentWidth+ gWindowWidth / 6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(ten_material_ambient[0],ten_material_ambient[1],ten_material_ambient[2],ten_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(ten_material_diffuse[0],ten_material_diffuse[1],ten_material_diffuse[2],ten_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(ten_material_specular[0],ten_material_specular[1],ten_material_specular[2],ten_material_specular[3]);
	constantBuffer.material_shininess=ten_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth =currentWidth+ gWindowWidth / 6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(eleven_material_ambient[0],eleven_material_ambient[1],eleven_material_ambient[2],eleven_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(eleven_material_diffuse[0],eleven_material_diffuse[1],eleven_material_diffuse[2],eleven_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(eleven_material_specular[0],eleven_material_specular[1],eleven_material_specular[2],eleven_material_specular[3]);
	constantBuffer.material_shininess=eleven_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth =currentWidth+ gWindowWidth / 6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(twelve_material_ambient[0],twelve_material_ambient[1],twelve_material_ambient[2],twelve_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(twelve_material_diffuse[0],twelve_material_diffuse[1],twelve_material_diffuse[2],twelve_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(twelve_material_specular[0],twelve_material_specular[1],twelve_material_specular[2],twelve_material_specular[3]);
	constantBuffer.material_shininess=twelve_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
/*Second Row ____END____*/

/*Third Row ____START____*/
	currentHeight=currentHeight+gWindowHeight/4;
	currentWidth=0;

	d3dViewPort.TopLeftX=0;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(thirteen_material_ambient[0],thirteen_material_ambient[1],thirteen_material_ambient[2],thirteen_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(thirteen_material_diffuse[0],thirteen_material_diffuse[1],thirteen_material_diffuse[2],thirteen_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(thirteen_material_specular[0],thirteen_material_specular[1],thirteen_material_specular[2],thirteen_material_specular[3]);
	constantBuffer.material_shininess=thirteen_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth=currentWidth+gWindowWidth/6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(fourteen_material_ambient[0],fourteen_material_ambient[1],fourteen_material_ambient[2],fourteen_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(fourteen_material_diffuse[0],fourteen_material_diffuse[1],fourteen_material_diffuse[2],fourteen_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(fourteen_material_specular[0],fourteen_material_specular[1],fourteen_material_specular[2],fourteen_material_specular[3]);
	constantBuffer.material_shininess=fourteen_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth=currentWidth+gWindowWidth/6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(fifteen_material_ambient[0],fifteen_material_ambient[1],fifteen_material_ambient[2],fifteen_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(fifteen_material_diffuse[0],fifteen_material_diffuse[1],fifteen_material_diffuse[2],fifteen_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(fifteen_material_specular[0],fifteen_material_specular[1],fifteen_material_specular[2],fifteen_material_specular[3]);
	constantBuffer.material_shininess=fifteen_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);


	currentWidth=currentWidth+gWindowWidth/6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(sixteen_material_ambient[0],sixteen_material_ambient[1],sixteen_material_ambient[2],sixteen_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(sixteen_material_diffuse[0],sixteen_material_diffuse[1],sixteen_material_diffuse[2],sixteen_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(sixteen_material_specular[0],sixteen_material_specular[1],sixteen_material_specular[2],sixteen_material_specular[3]);
	constantBuffer.material_shininess=sixteen_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth=currentWidth+gWindowWidth/6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(seventeen_material_ambient[0],seventeen_material_ambient[1],seventeen_material_ambient[2],seventeen_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(seventeen_material_diffuse[0],seventeen_material_diffuse[1],seventeen_material_diffuse[2],seventeen_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(seventeen_material_specular[0],seventeen_material_specular[1],seventeen_material_specular[2],seventeen_material_specular[3]);
	constantBuffer.material_shininess=seventeen_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);


	currentWidth=currentWidth+gWindowWidth/6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(eighteen_material_ambient[0],eighteen_material_ambient[1],eighteen_material_ambient[2],eighteen_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(eighteen_material_diffuse[0],eighteen_material_diffuse[1],eighteen_material_diffuse[2],eighteen_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(eighteen_material_specular[0],eighteen_material_specular[1],eighteen_material_specular[2],eighteen_material_specular[3]);
	constantBuffer.material_shininess=eighteen_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

/*Third Row ____END____*/		

/*Four Row ____START____*/

	currentHeight=currentHeight+gWindowHeight/4;
	currentWidth=0;

	d3dViewPort.TopLeftX=0;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(nineteen_material_ambient[0],nineteen_material_ambient[1],nineteen_material_ambient[2],nineteen_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(nineteen_material_diffuse[0],nineteen_material_diffuse[1],nineteen_material_diffuse[2],nineteen_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(nineteen_material_specular[0],nineteen_material_specular[1],nineteen_material_specular[2],nineteen_material_specular[3]);
	constantBuffer.material_shininess=nineteen_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth=currentWidth+gWindowWidth/6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(twenty_material_ambient[0],twenty_material_ambient[1],twenty_material_ambient[2],twenty_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(twenty_material_diffuse[0],twenty_material_diffuse[1],twenty_material_diffuse[2],twenty_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(twenty_material_specular[0],twenty_material_specular[1],twenty_material_specular[2],twenty_material_specular[3]);
	constantBuffer.material_shininess=twenty_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);


	currentWidth=currentWidth+gWindowWidth/6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(twentyone_material_ambient[0],twentyone_material_ambient[1],twentyone_material_ambient[2],twentyone_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(twentyone_material_diffuse[0],twentyone_material_diffuse[1],twentyone_material_diffuse[2],twentyone_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(twentyone_material_specular[0],twentyone_material_specular[1],twentyone_material_specular[2],twentyone_material_specular[3]);
	constantBuffer.material_shininess=twentyone_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);


	currentWidth=currentWidth+gWindowWidth/6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(twentytwo_material_ambient[0],twentytwo_material_ambient[1],twentytwo_material_ambient[2],twentytwo_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(twentytwo_material_diffuse[0],twentytwo_material_diffuse[1],twentytwo_material_diffuse[2],twentytwo_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(twentytwo_material_specular[0],twentytwo_material_specular[1],twentytwo_material_specular[2],twentytwo_material_specular[3]);
	constantBuffer.material_shininess=twentytwo_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);


	currentWidth=currentWidth+gWindowWidth/6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(twentythree_material_ambient[0],twentythree_material_ambient[1],twentythree_material_ambient[2],twentythree_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(twentythree_material_diffuse[0],twentythree_material_diffuse[1],twentythree_material_diffuse[2],twentythree_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(twentythree_material_specular[0],twentythree_material_specular[1],twentythree_material_specular[2],twentythree_material_specular[3]);
	constantBuffer.material_shininess=twentythree_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	currentWidth=currentWidth+gWindowWidth/6;

	d3dViewPort.TopLeftX=currentWidth;
	d3dViewPort.TopLeftY=currentHeight;
	d3dViewPort.Width=(float)gWindowWidth/6;
	d3dViewPort.Height=(float)gWindowHeight/4;
	d3dViewPort.MinDepth=0.0f;
	d3dViewPort.MaxDepth=1.0f;
	constantBuffer.Ka_uniform=XMVectorSet(twentyfour_material_ambient[0],twentyfour_material_ambient[1],twentyfour_material_ambient[2],twentyfour_material_ambient[3]);
	constantBuffer.Kd_uniform=XMVectorSet(twentyfour_material_diffuse[0],twentyfour_material_diffuse[1],twentyfour_material_diffuse[2],twentyfour_material_diffuse[3]);
	constantBuffer.Ks_uniform=XMVectorSet(twentyfour_material_specular[0],twentyfour_material_specular[1],twentyfour_material_specular[2],twentyfour_material_specular[3]);
	constantBuffer.material_shininess=twentyfour_material_shininess;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

/*Four Row ____END____*/		

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

	if (gpID3D11Buffer_IndexBuffer)
	{
		gpID3D11Buffer_IndexBuffer->Release();
		gpID3D11Buffer_IndexBuffer = NULL;
	}

	if(gpID3D11Buffer_VertexBufferPosition_Sphere){
		gpID3D11Buffer_VertexBufferPosition_Sphere->Release();
		gpID3D11Buffer_VertexBufferPosition_Sphere=NULL;
	}

	if(gpID3D11Buffer_VertexBufferNormal_Sphere){
		gpID3D11Buffer_VertexBufferNormal_Sphere->Release();
		gpID3D11Buffer_VertexBufferNormal_Sphere=NULL;
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
		gAngle = 0.0f;
	}
}