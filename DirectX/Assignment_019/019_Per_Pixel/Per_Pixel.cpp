/*


			----------------< DEPTH IS SIZE DEPENDANT >----------------

*/



/**
*
*  IMPLICIT CONVERSION IN HLSL WORKS FOR '*' OPERATOR , NOT FOR '+'
*
*		RECORDING - 24 mins onwards (18/9/21)
*
		CHECK SHADER CODE AND RECORDING - 18-09-2021
*
*/


//Header files
#include <windows.h>
#include <stdio.h>
#include <d3d11.h>
#include "Headers.h"

#include <d3dcompiler.h>

#include "Sphere.h"


//PRAGMA to suppress warnings
#pragma warning(disable: 4838)
#include "XNAMath\xnamath.h" 

//PRAGMA
#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "dxgi.lib") // DirectX Graphics Infrastructure -  analogous to 'wgl'
#pragma comment(lib, "D3dcompiler.lib")

#pragma comment(lib,"Sphere.lib")


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

ID3D11VertexShader* gpID3D11VertexShader = NULL;
ID3D11PixelShader* gpID3D11PixelShader = NULL;
ID3D11Buffer* gpID3D11Buffer_VertexBuffer_Position = NULL;
ID3D11Buffer* gpID3D11Buffer_VertexBuffer_Normal = NULL;
ID3D11Buffer* gpID3D11Buffer_IndexBuffer = NULL;
ID3D11InputLayout* gpID3D11InputLayout = NULL;
ID3D11Buffer* gpID3D11Buffer_ConstantBuffer = NULL;

//culling
ID3D11RasterizerState* gpID3D11RasterizerState = NULL;

//depth
ID3D11DepthStencilView* gpID3D11DepthStencilView = NULL;


struct CBUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

	XMVECTOR la;
	XMVECTOR ld;
	XMVECTOR ls;
	XMVECTOR lightPosition;

	XMVECTOR ka;
	XMVECTOR kd;
	XMVECTOR ks;
	float materialShininess;

	unsigned int keyPressed;
};

XMMATRIX gPerspectiveProjectionMatrix;

//Lights
bool gbLights = false;

//Sphere
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumVertices;
unsigned int gNumElements; //Elements a.k.a Indices

//properties
float lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightPosition[] = { 100.0f, 100.0f, -100.0f, 1.0f };

float materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float materialDiffuse[] = { 0.5f, 0.2f, 0.7f, 1.0f };
float materialSpecular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
float materialShininess = 128.0f;

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
		TEXT("D3D11: Assignment_020_Per_Pixel : By Kshitij Badkas"),
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
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "F KEY PRESSED.\n");
			fclose(gpFile);
			break;
		default:
			break;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{
			//Light
		case 'L':
		case 'l':
			if (gbLights == true)
			{
				gbLights = false;
				fopen_s(&gpFile, gpszLogFileName, "a+");
				fprintf_s(gpFile, "gbLights  = %d\n", gbLights);
				fclose(gpFile);

			}
			else if (gbLights == false)
			{
				gbLights = true;
				fopen_s(&gpFile, gpszLogFileName, "a+");
				fprintf_s(gpFile, "gbLights  = %d\n", gbLights);
				fclose(gpFile);

			}
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

	//1. decalre vertex shader source code

	const char* vertexShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 worldMatrix;" \
		"float4x4 viewMatrix;" \
		"float4x4 projectionMatrix;" \
		"float4 la;" \
		"float4 ld;" \
		"float4 ls;" \
		"float4 lightPosition;" \
		"float4 ka;" \
		"float4 kd;" \
		"float4 ks;" \
		"float materialShininess;" \
		"uint keyPressed;" \
		"}" \
		"struct vertex_output" \
		"{" \
		//INTER- SHADER SEMANTICS NOT FROM MAIN ->
		"float4 position: SV_POSITION;" \
		"float3 transformed_normals: NORMAL0;" \
		"float3 light_direction: NORMAL1;" \
		"float3 viewer_vector: NORMAL2;" \
		"};" \
		"vertex_output main(float4 pos : POSITION, float4 normal : NORMAL)" \
		"{" \
		"vertex_output output;" \
		"if(keyPressed == 1)" \
		"{" \
		"float4 eye_coordinates = mul(worldMatrix, pos);" \
		"eye_coordinates = mul(viewMatrix, eye_coordinates);" \
		"output.transformed_normals = (float3)mul((float3x3)worldMatrix, (float3)normal);" \
		"output.light_direction = (float3)lightPosition - eye_coordinates.xyz;" \
		"output.viewer_vector = -eye_coordinates.xyz;" \
		"}" \
		"float4 position = mul(worldMatrix, pos);" \
		"position = mul(viewMatrix, position);" \
		"position = mul(projectionMatrix, position);" \
		"output.position = position;" \
		"return (output);" \
		"}";

	//2. compile vertex shader source code
	ID3DBlob* pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob* pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() for Vertex Shader Failed. : %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
		else
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "Some COM Error for Vertex Shader.\n");
			fclose(gpFile);
		}
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() for Vertex Shader Succeeded.\n");
		fclose(gpFile);
	}


	//3. create Vertex shader
	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateVertexShader() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateVertexShader() Succeeded.\n");
		fclose(gpFile);
	}


	//4. Set vertex shader into pipeline
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);

	//Repeat above 4 steps for Pixel Shader
	//5. decalre pixel shader source code

	const char* pixelShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 worldMatrix;" \
		"float4x4 viewMatrix;" \
		"float4x4 projectionMatrix;" \
		"float4 la;" \
		"float4 ld;" \
		"float4 ls;" \
		"float4 lightPosition;" \
		"float4 ka;" \
		"float4 kd;" \
		"float4 ks;" \
		"float materialShininess;" \
		"uint keyPressed;" \
		"}" \
		"struct vertex_output" \
		"{" \
		//INTER- SHADER SEMANTICS NOT FROM MAIN ->
		"float4 position: SV_POSITION;" \
		"float3 transformed_normals: NORMAL0;" \
		"float3 light_direction: NORMAL1;" \
		"float3 viewer_vector: NORMAL2;" \
		"};" \
		"float4 main(vertex_output input) : SV_TARGET" \
		"{" \
		"float3 phong_ads_light;" \
		"if(keyPressed == 1)" \
		"{" \
		"float3 normalized_transformed_normals = normalize(input.transformed_normals);" \
		"float3 normalized_light_direction = normalize(input.light_direction);" \
		"float3 normalized_viewer_vector = normalize(input.viewer_vector);" \
		"float3 ambient = (float3)(la * ka);" \
		"float3 diffuse = (float3)(ld * kd) * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0f);" \
		"float3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
		"float3 specular = (float3)(ls * ks) * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0f), materialShininess);" \
		"phong_ads_light = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = float3(1.0f, 1.0f, 1.0f);" \
		"}" \
		"float4 color = float4(phong_ads_light, 1.0f);" \
		"return(color);" \
		"}";

	ID3DBlob* pID3DBlob_PixelShaderCode = NULL;
	pID3DBlob_Error = NULL;

	//6. compile pixel shader source code
	hr = D3DCompile(pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() for Pixel Shader Failed. : %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
		else
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "Some COM Error for Pixel Shader.\n");
			fclose(gpFile);
		}
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() for Pixel Shader Succeeded.\n");
		fclose(gpFile);
	}

	//7. create Pixel shader
	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreatePixelShader() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreatePixelShader() Succeeded.\n");
		fclose(gpFile);
	}

	//8. set  Pixel shader into pipeline
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);
	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;

	//9. Initialize, create and set input layout.
	//9.a Initialize input layout structure
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
	//POSITION
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0; //if you are going to send multiple geometry to same semantic name 
										//then they are separated by indices. 
										//Hence it is 0 for 1 geometry.
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; //glVertexAttribPointer() 2nd paramater
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlot = 0; //enum KAB_ATTRIBUTE_POSITION, COLOR, NORMAL
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	//NORMAL
	inputElementDesc[1].SemanticName = "NORMAL";
	inputElementDesc[1].SemanticIndex = 0; //if you are going to send multiple geometry to same semantic name 
										//then they are separated by indices. 
										//Hence it is 0 for 1 geometry.
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT; //glVertexAttribPointer() 2nd paramater
	inputElementDesc[1].AlignedByteOffset = 0;
	inputElementDesc[1].InputSlot = 1; //enum KAB_ATTRIBUTE_POSITION, COLOR, NORMAL
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	//9.b Create input layout
	hr = gpID3D11Device->CreateInputLayout(inputElementDesc, _ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateInputLayout() Failed.\n");
		fclose(gpFile);
		if (pID3DBlob_VertexShaderCode)
		{
			pID3DBlob_VertexShaderCode->Release();
			pID3DBlob_VertexShaderCode = NULL;
		}
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateInputLayout() Succeeded.\n");
		fclose(gpFile);
		if (pID3DBlob_VertexShaderCode)
		{
			pID3DBlob_VertexShaderCode->Release();
			pID3DBlob_VertexShaderCode = NULL;
		}
	}

	//9.c Set input layout in the pipeline
	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

	//10. 
	//DirectX is Left hand rule (CLOCKWISE)
	//vertices, colours, shader attribs, vbo, vao initializations
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	//11. a Initialize buffer description structure and create the buffer
	D3D11_BUFFER_DESC bufferDesc;
	//POSITION
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;  //glBindBuffer() , vbo
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //DYNAMIC_DRAW - glBufferData() 4th or last paramater

	// because we are not using Static Draw we will not use 2nd param
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_Position);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Failed for Vertex Buffer. - POSITION\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Succeeded for Vertex Buffer.  - POSITION \n");
		fclose(gpFile);
	}

	//NORMALS
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float); //glBufferData() 2nd param
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;  //glBindBuffer() , vbo
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //DYNAMIC_DRAW - glBufferData() 4th or last paramater

	// because we are not using Static Draw we will not use 2nd param
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_Normal);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Failed for Vertex Buffer. - NORMAL\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Succeeded for Vertex Buffer.  - NORMAL\n");
		fclose(gpFile);
	}

	//11. b Push the data in this buffer  
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Position, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, sphere_vertices, gNumVertices * 3 * sizeof(float)); //push data from cpu buffer to gpu buffer
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Position, 0);

	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Normal, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, sphere_normals, gNumVertices * 3 * sizeof(float)); //push data from cpu buffer to gpu buffer
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Normal, 0);

	//////////////////////////////
	// 
	//		INDEX BUFFER
	// 
	//////////////////////////////
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = gNumElements * sizeof(short);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_IndexBuffer);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Failed for Index Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Succeeded for Index Buffer.\n");
		fclose(gpFile);
	}

	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, sphere_elements, gNumElements * sizeof(short)); //push data from cpu buffer to gpu buffer
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_IndexBuffer, 0);

	//WE WILL NOT SET THIS BUFFER IN PIPELINE BECAUSE WE WILL DO IT IN DRAW() / DISPLAY() [DYNAMIC_DRAW]

	//12. Initialize and create
	ZeroMemory((void*)&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_ConstantBuffer);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Failed for Constant Buffer .\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Succeeded for Constant Buffer.\n");
		fclose(gpFile);
	}

	//13. set the Constant Buffer into the pipeline
	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

	gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);


	//CREATE AND SET RASTERIZER STATE
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory((void*)&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; //can be WIREFRAME also - default is SOLID
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	hr = gpID3D11Device->CreateRasterizerState(&rasterizerDesc, &gpID3D11RasterizerState);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateRasterizerState() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateRasterizerState() Succeeded.\n");
		fclose(gpFile);
	}

	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

	// clear color BLACK
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	//14. Initialize projection matrix
	gPerspectiveProjectionMatrix = XMMatrixIdentity();

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

	if (height == 0)
	{
		height = 1;
	}

	//RELEASE PREVIOUS DEPTH STENCIL VIEW
	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

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


	//DEPTH CODE
	//JUST LIKE NEEEDED FOR RENDER TARGET VIEW(RTV) - DEPTH STENCIL VIEW(DSV) ALSO NEEDS TEXTURE BUFFER.
	D3D11_TEXTURE2D_DESC texture2DDesc;
	ZeroMemory((void*)&texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2DDesc.Width = (UINT)width;
	texture2DDesc.Height = (UINT)height;
	texture2DDesc.Format = DXGI_FORMAT_D32_FLOAT;
	texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2DDesc.SampleDesc.Count = 1; //THIS CAN BE 1 TO 4 AS PER NEEDED QUALITY
	texture2DDesc.SampleDesc.Quality = 0; //DEDAULT
	texture2DDesc.ArraySize = 1;
	texture2DDesc.MipLevels = 1; //DEFAULT
	texture2DDesc.CPUAccessFlags = 0; //DEFAULT
	texture2DDesc.MiscFlags = 0; //DEFAULT

	ID3D11Texture2D* pID3D11Texture2D_DepthBuffer = NULL;
	hr = gpID3D11Device->CreateTexture2D(&texture2DDesc, NULL, &pID3D11Texture2D_DepthBuffer);
	// **** NULL => THIS IS D3D11_SubResource_Data - used when you have some data right now.

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateTexture2D() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateTexture2D() Succeeded.\n");
		fclose(gpFile);
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory((void*)&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS; //MS - Multi Sampling

	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer, &depthStencilViewDesc, &gpID3D11DepthStencilView);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateDepthStencilView() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateDepthStencilView() Succeeded.\n");
		fclose(gpFile);
	}


	//set render target view as render target ---> DepthStencilView included
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView);

	//set viewport
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

	//set perspective matrix
	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	return(hr);
}

//Function Display
void Display(void)
{
	//code
	//clear render target view to a chosen colour
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	//clear depth stencil view 
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//15. set vertex buffer into pipeline
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Position, &stride, &offset);


	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Normal, &stride, &offset);

	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	//16. set primitive topology
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//17. transformations
	XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 1.5f);
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX wv = worldMatrix * viewMatrix;

	//18. push Transformation matrix into shader
	CBUFFER constantBuffer;
	ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = viewMatrix;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

	if (gbLights == true)
	{
		constantBuffer.keyPressed = 1;

		constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
		constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
		constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
		constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);
		constantBuffer.ka = XMVectorSet(materialAmbient[0], materialAmbient[1], materialAmbient[2], materialAmbient[3]);
		constantBuffer.kd = XMVectorSet(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2], materialDiffuse[3]);
		constantBuffer.ks = XMVectorSet(materialSpecular[0], materialSpecular[1], materialSpecular[2], materialSpecular[3]);
		constantBuffer.materialShininess = materialShininess;
	}
	else
	{
		constantBuffer.keyPressed = 0;
	}

	//glUniformMatrix4fv //3rd , 5th, 6th Bounding Box params (used in Collision Detection)
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);


	//19. Draw
	gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);


	//switch between front and back buffers 
	// SWAP BUFFERS //
	gpIDXGISwapChain->Present(0, 0);

}

//Function Uninit
void Uninitialize(void)
{
	//code
	if (gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if (gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}

	if (gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}

	if (gpID3D11Buffer_IndexBuffer)
	{
		gpID3D11Buffer_IndexBuffer->Release();
		gpID3D11Buffer_IndexBuffer = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Normal)
	{
		gpID3D11Buffer_VertexBuffer_Normal->Release();
		gpID3D11Buffer_VertexBuffer_Normal = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_Position)
	{
		gpID3D11Buffer_VertexBuffer_Position->Release();
		gpID3D11Buffer_VertexBuffer_Position = NULL;
	}

	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if (gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}

	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
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