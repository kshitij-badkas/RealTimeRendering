//Header files
#include <windows.h>
#include <stdio.h>
#include <d3d11.h>
#include <math.h>
#include "Headers.h"

//PRAGMA
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib") // DirectX Graphics Infrastructure -  analogous to 'wgl'

//MACROS
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global function prototype
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variables
IDXGISwapChain* gpIDXGISwapChain = NULL;
ID3D11Device* gpID3D11Device = NULL;
ID3D11DeviceContext* gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView* gpID3D11RenderTargetView = NULL;
float gClearColor[4]; //RGBA

HWND ghwnd = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbFullscreen = false;
bool gbActiveWindow = false;

IDXGIFactory* gpIDXGIFactory = NULL;
IDXGIAdapter* gpIDXGIAdapater = NULL;

FILE* gpFile = NULL;
char gpszLogFileName[] = "KAB_Log.txt";

// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{

	//NEW PROTOTYPES
	HRESULT Initialize(void);
	void Display(void);
	void Uninitialize(void);

	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("D3D11");
	HRESULT hr;
	RECT rc;
	bool bDone = false;

	// Centered Window
	// Get Screen Resolution - Width and Height
	int iScreenWidth;
	int iScreenHeight;

	//New Window Position - For Centering of New Window
	int iCreateWindowPositionX;
	int iCreateWindowPositionY;

	// Get Work Area (Resolution excluding Taskbar area)
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

	// Screen Resolution - Width and Height
	iScreenWidth = (int)rc.right;
	iScreenHeight = (int)rc.bottom;

	//New Window Position - For Centering of New Window
	iCreateWindowPositionX = (iScreenWidth / 2) - (WIN_WIDTH / 2);
	iCreateWindowPositionY = (iScreenHeight / 2) - (WIN_HEIGHT / 2);

	//Code for Debugging
	//Open file for writing
	int iFileOpen;
	iFileOpen = fopen_s(&gpFile, gpszLogFileName, "w");
	if (iFileOpen != 0)
	{
		//MessageBox 
		MessageBox(NULL, TEXT("Cannot Create Desired File."), TEXT("Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile, "Log File Is Successfully Opened.\n\n");

		fprintf_s(gpFile, "cl.exe /c /EHsc 001_Blue_Window.cpp\n");
		fprintf_s(gpFile, "rc.exe Resources.rc\n");
		fprintf_s(gpFile, "link.exe 001_Blue_Window.obj USER32.LIB GDI32.LIB Resources.res /SUBSYSTEM:WINDOWS\n");
		fprintf_s(gpFile, "001_Blue_Window.exe\n");

		fclose(gpFile);
	}

	//WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	RegisterClassEx(&wndclass);

	//CreateWindowEx
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName,
		TEXT("D3D11: Assignment_001_Blue_Window : By Kshitij Badkas"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		iCreateWindowPositionX,
		iCreateWindowPositionY,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	//INIT CALL
	hr = Initialize();
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "Initialize() Failed.\n");
		fclose(gpFile);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "Initialize() Succeeded.\n");
		fclose(gpFile);
	}

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	
	/*************************
	* 
	*	GAME LOOP START
	* 
	*************************/
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				//HERE YOU SHOULD CALL Update Function FOR D3D11 RENDERING

				//HERE YOU SHOULD CALL Display Function FOR D3D11 RENDERING
				Display();

			}
		}
	}
	/*************************
	*
	*	GAME LOOP END
	*
	*************************/

	//clean-up
	Uninitialize();

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Local function declaration
	void ToggleFullscreen(void);
	HRESULT Resize(int, int);
	void Uninitialize(void);
	
	//Variables
	HRESULT hr;

	switch (iMsg)
	{

	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_SIZE:
		if (gpID3D11DeviceContext)
		{
			hr = Resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
			{
				fopen_s(&gpFile, gpszLogFileName, "a+");
				fprintf_s(gpFile, "Resize() Failed.\n");
				fclose(gpFile);
				return(hr);
			}
			else
			{
				fopen_s(&gpFile, gpszLogFileName, "a+");
				fprintf_s(gpFile, "Resize() Succeeded.\n");
				fclose(gpFile);
			}
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 0x46: //'F'
		case 0x66: //'f'
			ToggleFullscreen();
			break;
		default:
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);

	case WM_DESTROY:
		Uninitialize();
		PostQuitMessage(0);
		break;

	default:
		break;
	}


	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

//Function ToggleFullscreen()
void ToggleFullscreen(void)
{
	// local variables
	MONITORINFO mi = { sizeof(MONITORINFO) };

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, (dwStyle & ~WS_OVERLAPPEDWINDOW));
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullscreen = true;
	}
	else
	{
		//toggle back to normal size
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));

		SetWindowPlacement(ghwnd, &wpPrev);

		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullscreen = false;
	}
}

//Function Init
HRESULT Initialize(void)
{
	//Function declaration
	void getD3D11Log(void); // D3D11_log.cpp here
	void Uninitialize(void);
	HRESULT Resize(int, int);

	//GET GRAPHIC CARD INFO
	getD3D11Log();

	//local variable declarations
	HRESULT hr;
	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0; //default
	UINT numFeatureLevels = 1;
	UINT numDriverTypes = 0;
	UINT createDeviceFlags = 0;
	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE
	};

	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]); // calculate sizeof array

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	//Multi-Sampling & Anti-Aliasing (MSAA) (Performance degrades if value is higher)
	dxgiSwapChainDesc.SampleDesc.Count = 1; // Maximum 8 (bit)
	dxgiSwapChainDesc.SampleDesc.Quality = 0; // Default by OS
	dxgiSwapChainDesc.Windowed = TRUE; // Windowed or Fullscreen as Default?

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = d3dDriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,						// Adapter
			d3dDriverType,				// Driver Type
			NULL,						// Software
			createDeviceFlags,			// Flags
			&d3dFeatureLevel_required,	// Feature Level
			numFeatureLevels,			// Num Feature Levels
			D3D11_SDK_VERSION,			// SDK Version
			&dxgiSwapChainDesc,			// Swap Chain Desc
			&gpIDXGISwapChain,			// Swap Chain
			&gpID3D11Device,			// Device
			&d3dFeatureLevel_acquired,	// Feature Level acquired
			&gpID3D11DeviceContext);	// Device Context

		if (SUCCEEDED(hr))
		{
			break;
		}
	}
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "D3D11CreateDeviceAndSwapChain() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "D3D11CreateDeviceAndSwapChain() Succeeded.\n");
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpFile, "Hardware Type Chosen.\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpFile, "WARP Type Chosen.\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpFile, "Refernce Type Chosen.\n");
		}
		else
		{
			fprintf_s(gpFile, "Unknown Type Chosen.\n");
		}

		fprintf_s(gpFile, "The Supported Highest Feature Level Is : ");
		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf_s(gpFile, "11.0\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf_s(gpFile, "10.1\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf_s(gpFile, "10.0\n");
		}
		else
		{
			fprintf_s(gpFile, "Unknown.\n");
		}
		fclose(gpFile);
	}

	// clear color BLUE
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 1.0f;
	gClearColor[3] = 1.0f;

	//call Resize for first time
	hr = Resize(WIN_WIDTH, WIN_HEIGHT);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "Resize() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "Resize() Succeeded.\n");
		fclose(gpFile);
	}

	return(S_OK);
	
}

// GET GRAPHIC CARD INFO
void getD3D11Log(void)
{
	//prototypes
	void Uninitialize(void);
	
	// code
	DXGI_ADAPTER_DESC dxgiAdapterDesc;
	HRESULT hr;
	char str[255];

	// code 
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&gpIDXGIFactory);	// Universally Unique Identifier (uuid)
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "CreateDXGIFactory() Failed.\n");
		fclose(gpFile);
		Uninitialize();
	}

	if (gpIDXGIFactory->EnumAdapters(0, &gpIDXGIAdapater) == DXGI_ERROR_NOT_FOUND) // 0 means 1st graphics card eg. Intel or NVIDIA
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "DXGIAdapter Cannot Be Found.\n");
		fclose(gpFile);
		Uninitialize();
	}

	ZeroMemory((void*)&dxgiAdapterDesc, sizeof(DXGI_ADAPTER_DESC));
	hr = gpIDXGIAdapater->GetDesc(&dxgiAdapterDesc);

	WideCharToMultiByte(CP_ACP, 0, dxgiAdapterDesc.Description, 255, str, 255, NULL, NULL);
	fopen_s(&gpFile, gpszLogFileName, "a+");
	fprintf_s(gpFile, "\n********* My Graphic Card Info *********\n");
	fprintf_s(gpFile, "My Graphic Card Name = %s\n", str);
	fprintf_s(gpFile, "My Graphic Card VRAM = %I64d Bytes\n", (__int64)dxgiAdapterDesc.DedicatedVideoMemory);
	fprintf_s(gpFile, "My Graphic Card VRAM in GB = %d GB\n", (int)(ceil(dxgiAdapterDesc.DedicatedVideoMemory / 1024.0 / 1024.0 / 1024.0)));
	fprintf_s(gpFile, "\n********* ********* ********* *********\n");
	fclose(gpFile);
}

//Function Resize
HRESULT Resize(int width, int height)
{
	//code
	HRESULT hr = S_OK;

	//free any size-dependant resources
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	//resize swap-chain buffers accordingly
	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	//again get back buffer from swap chain
	ID3D11Texture2D* pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);

	//again get render target view from d3d11 device using above back buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRenderTargetView() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRenderTargetView() Succeeded.\n");
		fclose(gpFile);
	}

	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	//set render target view as render target
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, NULL);

	//set viewport
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);// Rasterization Set - hint for 24 spheres

	return(hr);
}

//Function Display
void Display(void)
{
	//code
	//clear render target view to a chosen colour
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	//switch between front and back buffers 
	// SWAP BUFFERS //
	gpIDXGISwapChain->Present(0, 0);

}

//Function Uninit
void Uninitialize(void)
{
	//code
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}

	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}

	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}

	if (gpIDXGIAdapater)
	{
		gpIDXGIAdapater->Release();
		gpIDXGIAdapater = NULL;
	}

	if (gpIDXGIFactory)
	{
		gpIDXGIFactory->Release();
		gpIDXGIFactory = NULL;
	}
	
	//File Close Code Here
	if (gpFile)
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "Uninitialize() Succeeded.\n");
		fprintf_s(gpFile, "Log File Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
