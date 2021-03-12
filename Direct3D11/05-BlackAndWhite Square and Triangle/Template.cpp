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
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
IDXGISwapChain *gpIDXGISwapChain=NULL;
ID3D11Device *gpID3D11Device=NULL;
ID3D11VertexShader *gpID3D11VertexShader=NULL;
ID3D11PixelShader *gpID3D11PixelShader=NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBufferPosition_Triangle=NULL;		//1
ID3D11Buffer *gpID3D11Buffer_VertexBufferColor_Triangle=NULL;		///2
ID3D11InputLayout *gpID3D11InputLayout=NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer=NULL;

struct  CBUFFER{
	XMMATRIX WorldViewProjectionMatrix;
};

float gClearColor[4];

XMMATRIX gPerspectiveProjectionMatrix;

ID3D11RenderTargetView *gpID3D11RenderTargetView=NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBufferPosition_Square=NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBufferColor_Square=NULL;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);

	WNDCLASSEX wndclassex;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[]=TEXT("05-Direct3D11 Black and White Triangle and Square Window");
	bool bDone=false;


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
			"float4x4 worldViewProjectionMatrix;"\
		"}"\
		"struct vertex_output"\
		"{"\
			"float4 position:SV_POSITION;"\
			"float4 color:COLOR;"\
		"};"\
		"vertex_output main(float4 pos:POSITION,float4 color:COLOR)"\
		"{"\
			"vertex_output output;"\
			"output.position=mul(worldViewProjectionMatrix,pos);"\
			"output.color=color;"\
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
	"float4 main(float4 pos:SV_POSITION,float4 color:COLOR):SV_TARGET"\
	"{"\
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


	float triangleVertices[]={
		0.0f,1.0f,0.0f,		//apex
		1.0f,-1.0f,0.0f,		//right
		-1.0f,-1.0f,0.0f,		//left
	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth=sizeof(float)*ARRAYSIZE(triangleVertices);
	bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	hr=gpID3D11Device->CreateBuffer(&bufferDesc,NULL,&gpID3D11Buffer_VertexBufferPosition_Triangle);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed for Vertex Buffer Position for Triangle\n");
		fclose(gpFile);
		return(hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer Position for Triangle\n");
		fclose(gpFile);
	}


	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferPosition_Triangle,NULL,
								D3D11_MAP_WRITE_DISCARD,NULL,
								&mappedSubresource);	
	memcpy(mappedSubresource.pData,triangleVertices,sizeof(triangleVertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferPosition_Triangle,NULL);


	float triangleColor[]={		
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
	};

	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth=sizeof(float)*ARRAYSIZE(triangleColor);
	bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	hr=gpID3D11Device->CreateBuffer(&bufferDesc,NULL,&gpID3D11Buffer_VertexBufferColor_Triangle);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed for Vertex Buffer Color for Triangle..\n");
		fclose(gpFile);
		gpFile=NULL;
		return (hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeded for Vertex Buffer Color for Triangle\n");
		fclose(gpFile);
	}

	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferColor_Triangle,NULL,
								D3D11_MAP_WRITE_DISCARD,NULL,
								&mappedSubresource);	
	memcpy(mappedSubresource.pData,triangleColor,sizeof(triangleColor));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferColor_Triangle,NULL);

/*
	If we give square vertices as 	 -1,1,0,		1,1,0,		-1,-1,0,		-1,-1,0,		1,1,0,		1,-1,0
	and use PRIMITIVE as TRIANGLELIST then in display() the Draw() will be Draw(6,0)
													AND
	If we give square vertices as 	 -1,1,0,		1,1,0,		-1,-1,0,		1,-1,0,		-1,-1,0,		1,1,0
	and use PRIMITIVE as TRIANGLESTRIP then in display() the Draw() will be Draw(4,0)

*/

	float squareVertices[]={
		-1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,		
		-1.0f,-1.0f,0.0f,

		-1.0f,-1.0f,0.0f,	
		1.0f,1.0f,0.0f,
		1.0f,-1.0f,0.0f,	
	};

	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth=sizeof(float)*ARRAYSIZE(squareVertices);
	bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	hr=gpID3D11Device->CreateBuffer(&bufferDesc,NULL,&gpID3D11Buffer_VertexBufferPosition_Square);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed for Vertex Buffer Position for Square\n");
		fclose(gpFile);
		return(hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer Position for Square\n");
		fclose(gpFile);
	}


	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferPosition_Square,NULL,
								D3D11_MAP_WRITE_DISCARD,NULL,
								&mappedSubresource);	
	memcpy(mappedSubresource.pData,squareVertices,sizeof(squareVertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferPosition_Square,NULL);


	float squareColor[]={		
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
	};

	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth=sizeof(float)*ARRAYSIZE(squareColor);
	bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	hr=gpID3D11Device->CreateBuffer(&bufferDesc,NULL,&gpID3D11Buffer_VertexBufferColor_Square);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Failed for Vertex Buffer Color for Square..\n");
		fclose(gpFile);
		gpFile=NULL;
		return (hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateBuffer() Succeded for Vertex Buffer Color for Square..\n");
		fclose(gpFile);
	}

	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferColor_Square,NULL,
								D3D11_MAP_WRITE_DISCARD,NULL,
								&mappedSubresource);	
	memcpy(mappedSubresource.pData,squareColor,sizeof(squareColor));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferColor_Square,NULL);


	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
	inputElementDesc[0].SemanticName="POSITION";
	inputElementDesc[0].SemanticIndex=0;	//yaa index la tyachaya madhe kitva maanu(0 la 0th Mann)
	inputElementDesc[0].Format=DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot=0;
	inputElementDesc[0].AlignedByteOffset=0;
	inputElementDesc[0].InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate=0;

	inputElementDesc[1].SemanticName="COLOR";
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

	gpID3D11DeviceContext->OMSetRenderTargets(1,&gpID3D11RenderTargetView,NULL);

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

	UINT stride=sizeof(float)*3;	//x,y,z so 3 ni multiply kelai
	UINT offset=0;
	gpID3D11DeviceContext->IASetVertexBuffers(0,1,&gpID3D11Buffer_VertexBufferPosition_Triangle,&stride,&offset);
	//2nd Parameter is Input slot
	
	stride=sizeof(float)*3;			//r,g,b so 3 ni Multiply kelai
	offset=0;
	gpID3D11DeviceContext->IASetVertexBuffers(1,1,&gpID3D11Buffer_VertexBufferColor_Triangle,&stride,&offset);
	//2nd Parameter is Input slot

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX worldMatrix=XMMatrixIdentity();
	XMMATRIX viewMatrix=XMMatrixIdentity();
	viewMatrix=XMMatrixTranslation(-1.5f,0.0f,6.0f);
	XMMATRIX wvpMatrix=worldMatrix*viewMatrix*gPerspectiveProjectionMatrix;

	CBUFFER constantBuffer;
	constantBuffer.WorldViewProjectionMatrix=wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);
		gpID3D11DeviceContext->Draw(3,0);





	stride=sizeof(float)*3;
	offset=0;
	gpID3D11DeviceContext->IASetVertexBuffers(0,1,&gpID3D11Buffer_VertexBufferPosition_Square,&stride,&offset);

	stride=sizeof(float)*3;
	offset=0;
	gpID3D11DeviceContext->IASetVertexBuffers(1,1,&gpID3D11Buffer_VertexBufferColor_Square,&stride,&offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldMatrix=XMMatrixIdentity();
	viewMatrix=XMMatrixIdentity();
	viewMatrix=XMMatrixTranslation(1.5f,0.0f,6.0f);
	wvpMatrix=worldMatrix*viewMatrix*gPerspectiveProjectionMatrix;
	constantBuffer.WorldViewProjectionMatrix=wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,0,
											NULL,&constantBuffer,0,0);

		gpID3D11DeviceContext->Draw(6,0);


	gpIDXGISwapChain->Present(0,0);	
}

void uninitialize(void){
	if(gpID3D11Buffer_ConstantBuffer){
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer=NULL;
	}

	if(gpID3D11InputLayout){
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout=NULL;
	}

	if(gpID3D11Buffer_VertexBufferPosition_Triangle){
		gpID3D11Buffer_VertexBufferPosition_Triangle->Release();
		gpID3D11Buffer_VertexBufferPosition_Triangle=NULL;
	}

	if(gpID3D11Buffer_VertexBufferColor_Triangle){
		gpID3D11Buffer_VertexBufferColor_Triangle->Release();
		gpID3D11Buffer_VertexBufferColor_Triangle=NULL;
	}

	if(gpID3D11Buffer_VertexBufferPosition_Square){
		gpID3D11Buffer_VertexBufferPosition_Square->Release();
		gpID3D11Buffer_VertexBufferPosition_Square=NULL;
	}

	if(gpID3D11Buffer_VertexBufferColor_Square){
		gpID3D11Buffer_VertexBufferColor_Square->Release();
		gpID3D11Buffer_VertexBufferColor_Square=NULL;
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