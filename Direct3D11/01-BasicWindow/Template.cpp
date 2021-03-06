#include<windows.h>
#include<stdio.h>
#include<d3d11.h>

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"d3d11.lib")

#define WIN_WIDTH	800
#define WIN_HEIGHT	600

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

FILE *gpFile=NULL;
char gszLogFileName[]="SSK_Log.txt";

HWND ghwnd=NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};

bool gbFullscreen=false;
bool gbActiveWindow=false;
bool gbEscapeKeyIsPressed=false;

float gClearColor[4];
IDXGISwapChain *gpIDXGISwapChain=NULL;
ID3D11Device *gpID3D11Device=NULL;
ID3D11DeviceContext *gpID3D11DeviceContext=NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView=NULL;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow){

	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);

	WNDCLASSEX wndclassex;
	HWND hwnd;
	TCHAR szClassName[]=TEXT("01-Direct3D11 Basic Window");
	MSG msg;
	bool bDone=false;

	if(fopen_s(&gpFile,gszLogFileName,"w")!=0){
		MessageBox(NULL,TEXT("Log File Can Not be created\n Exitting Now..."),
					TEXT("Error"),MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(EXIT_FAILURE);
	}else{
		fprintf_s(gpFile,"Log File Is Sucessfully Opened\n");
		fclose(gpFile);
	}

	wndclassex.cbSize=sizeof(WNDCLASSEX);
	wndclassex.cbClsExtra=0;
	wndclassex.cbWndExtra=0;
	wndclassex.style=CS_HREDRAW |CS_VREDRAW |CS_OWNDC;
	wndclassex.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	wndclassex.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclassex.lpszClassName=szClassName;
	wndclassex.lpszMenuName=NULL;
	wndclassex.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.lpfnWndProc=WndProc;
	wndclassex.hInstance=hInstance;

	RegisterClassEx(&wndclassex);

	hwnd=CreateWindow(szClassName,szClassName,WS_OVERLAPPEDWINDOW,
					100,100,WIN_WIDTH,WIN_HEIGHT,NULL,NULL,
					hInstance,NULL);
	ghwnd=hwnd;

	ShowWindow(hwnd,iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	HRESULT hr;
	hr=initialize();
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"initialize() Failed.. Exitting Now!!!!\n");
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

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam){
	HRESULT resize(int,int);
	void ToggleFullscreen(void);
	void uninitialize(void);
	HRESULT hr;

	switch(iMsg){
		case WM_ACTIVATE:
			if(HIWORD(wParam)==0){
				gbActiveWindow=true;
			}else{
				gbActiveWindow=false;
			}
			break;
		case WM_ERASEBKGND:
			return(0);
			break;
		case WM_SIZE:
			if(gpID3D11DeviceContext){
				hr=resize(LOWORD(lParam),HIWORD(lParam));
				if(FAILED(hr)){
					fopen_s(&gpFile,gszLogFileName,"a+");
					fprintf_s(gpFile,"reize() Failed..\n");
					fclose(gpFile);
				}else{
					fopen_s(&gpFile,gszLogFileName,"a+");
					fprintf_s(gpFile,"resize() Succeeded..\n");
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
	D3D_DRIVER_TYPE d3dDriverTypes[]={D3D_DRIVER_TYPE_HARDWARE,D3D_DRIVER_TYPE_WARP,D3D_DRIVER_TYPE_REFERENCE,};
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
	dxgiSwapChainDesc.BufferCount=1;
	dxgiSwapChainDesc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow=ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count=1;
	dxgiSwapChainDesc.SampleDesc.Quality=0;
	dxgiSwapChainDesc.Windowed=TRUE;

	for(UINT driverTypeIndex=0;driverTypeIndex<numDriverTypes; driverTypeIndex++){
		d3dDriverType=d3dDriverTypes[driverTypeIndex];
		hr=D3D11CreateDeviceAndSwapChain(
			NULL,
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
		fprintf_s(gpFile,"D3D11CreateDeviceAndSwapChain() FAILED..\n");
		fclose(gpFile);
		return (hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"D3D11CreateDeviceAndSwapChain() SUCCEEDED...\n");
		fprintf_s(gpFile,"The Chosen Driver Is Of ");
		if(d3dDriverType==D3D_DRIVER_TYPE_HARDWARE){
			fprintf_s(gpFile,"Hardware Type..\n");
		}else if(d3dDriverType==D3D_DRIVER_TYPE_WARP){
			fprintf_s(gpFile,"Warp Type..\n");
		}else if(d3dDriverType==D3D_DRIVER_TYPE_REFERENCE){
			fprintf_s(gpFile,"Reference Type...\n");
		}else{
			fprintf_s(gpFile,"Unknown Type..\n");
		}

		fprintf_s(gpFile,"The Supported Highest Feature Level Is ");
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

	gClearColor[0]=0.0f;
	gClearColor[1]=0.0f;
	gClearColor[2]=1.0f;
	gClearColor[3]=1.0f;

	hr=resize(WIN_WIDTH,WIN_HEIGHT);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"resize() Failed..\n");
		fclose(gpFile);
		return (hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"resize() SUCCEEDED....\n");
		fclose(gpFile);
	}
	return (S_OK);
}


HRESULT resize(int width,int height){
	HRESULT hr=S_OK;

	if(gpID3D11RenderTargetView){
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView=NULL;
	}

	gpIDXGISwapChain->ResizeBuffers(1,width,height,DXGI_FORMAT_R8G8B8A8_UNORM,0);

	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0,__uuidof(pID3D11Texture2D_BackBuffer),(LPVOID*)&pID3D11Texture2D_BackBuffer);

	hr=gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer,NULL,&gpID3D11RenderTargetView);
	if(FAILED(hr)){
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"ID3D11Device::CreateRenderTargetView() Failed..\n");
		fclose(gpFile);
		return (hr);
	}else{
		fopen_s(&gpFile,gszLogFileName,"a+");
		fprintf_s(gpFile,"gpID3D11Device::CreateRenderTargetView() SUCCEEDED..\n");
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

	return (hr);
}

void display(void){
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView,gClearColor);


	gpIDXGISwapChain->Present(0,0);
}

void uninitialize(void){
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
		fprintf_s(gpFile,"uninitialize() SUCCEEDED...\n");
		fprintf_s(gpFile,"Log File Successfully Closed..\n");
		fclose(gpFile);
	}
}