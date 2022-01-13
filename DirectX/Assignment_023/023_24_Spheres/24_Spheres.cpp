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

//Per Vertex
ID3D11VertexShader* gpID3D11VertexShader_PV = NULL;
ID3D11PixelShader* gpID3D11PixelShader_PV = NULL;
ID3D11InputLayout* gpID3D11InputLayout_PV = NULL;
ID3D11Buffer* gpID3D11Buffer_constantBuffer = NULL;

//Per Pixel
ID3D11VertexShader* gpID3D11VertexShader_PP = NULL;
ID3D11PixelShader* gpID3D11PixelShader_PP = NULL;
ID3D11InputLayout* gpID3D11InputLayout_PP = NULL;
//ID3D11Buffer* gpID3D11Buffer_constantBuffer = NULL; // ONE CONSTANT BUFFER IS ENOUGH. HENCE COMMENTING OUT THIS LINE.

ID3D11Buffer* gpID3D11Buffer_VertexBuffer_Position = NULL;
ID3D11Buffer* gpID3D11Buffer_VertexBuffer_Normal = NULL;
ID3D11Buffer* gpID3D11Buffer_IndexBuffer = NULL;


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
float lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };

// material will have 24 different values
float materialAmbient1[] = { 0.0215f, 0.1745f, 0.0215f, 1.0f };
float materialDiffuse1[] = { 0.07568f, 0.61424f, 0.07568f, 1.0f };
float materialSpecular1[] = { 0.633f, 0.727811f, 0.633f, 1.0f };
float materialShininess1 = 0.6f * 128.0f;

float materialAmbient2[] = { 0.135f, 0.225f, 0.1575f, 1.0f };
float materialDiffuse2[] = { 0.54f, 0.89f, 0.63f, 1.0f };
float materialSpecular2[] = { 0.316228f, 0.316228f, 0.316228f, 1.0f };
float materialShininess2 = 0.1f * 128.0f;

float materialAmbient3[] = { 0.05375f, 0.05f, 0.06625f, 1.0f };
float materialDiffuse3[] = { 0.18275f, 0.17f, 0.22525f, 1.0f };
float materialSpecular3[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialShininess3 = 0.3f * 128.0f;

float materialAmbient4[] = { 0.25f, 0.20725f, 0.20725f, 1.0f };
float materialDiffuse4[] = { 1.0f, 0.829f, 0.829f, 1.0f };
float materialSpecular4[] = { 0.296648f, 0.296648f, 0.296648f, 1.0f };
float materialShininess4 = 0.088f * 128.0f;

float materialAmbient5[] = { 0.1745f, 0.01175f, 0.01175f, 1.0f };
float materialDiffuse5[] = { 0.61424f, 0.04136f, 0.04136f, 1.0f };
float materialSpecular5[] = { 0.727811f, 0.626959f, 0.626959f, 1.0f };
float materialShininess5 = 0.6f * 128.0f;

float materialAmbient6[] = { 0.1f, 0.18725f, 0.1745f, 1.0f };
float materialDiffuse6[] = { 0.396f, 0.74151f, 0.69102f, 1.0f };
float materialSpecular6[] = { 0.297254f, 0.30829f, 0.306678f, 1.0f };
float materialShininess6 = 0.1f * 128.0f;

float materialAmbient7[] = { 0.329412f, 0.223529f, 0.027451f, 1.0f };
float materialDiffuse7[] = { 0.780392f, 0.568627f, 0.113725f, 1.0f };
float materialSpecular7[] = { 0.992157f, 0.941176f, 0.807843f, 1.0f };
float materialShininess7 = 0.21794872f * 128.0f;

float materialAmbient8[] = { 0.2125f, 0.1275f, 0.054f, 1.0f };
float materialDiffuse8[] = { 0.714f, 0.4284f, 0.18144f, 1.0f };
float materialSpecular8[] = { 0.393548f, 0.271906f, 0.166721f, 1.0f };
float materialShininess8 = 0.2f * 128.0f;

float materialAmbient9[] = { 0.25f, 0.25f, 0.25f, 1.0f };
float materialDiffuse9[] = { 0.4f, 0.4f, 0.4f, 1.0f };
float materialSpecular9[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
float materialShininess9 = 0.6f * 128.0f;

float materialAmbient10[] = { 0.19125f, 0.0735f, 0.0225f, 1.0f };
float materialDiffuse10[] = { 0.7038f, 0.27048f, 0.0828f, 1.0f };
float materialSpecular10[] = { 0.256777f, 0.137622f, 0.086014f, 1.0f };
float materialShininess10 = 0.1f * 128.0f;

float materialAmbient11[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
float materialDiffuse11[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
float materialSpecular11[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
float materialShininess11 = 0.4f * 128.0f;

float materialAmbient12[] = { 0.19225f, 0.19225f, 0.19225f, 1.0f };
float materialDiffuse12[] = { 0.50754f, 0.50754f, 0.50754f, 1.0f };
float materialSpecular12[] = { 0.508273f, 0.508273f, 0.508273f, 1.0f };
float materialShininess12 = 0.4f * 128.0f;

float materialAmbient13[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float materialDiffuse13[] = { 0.01f, 0.01f, 0.01f, 1.0f };
float materialSpecular13[] = { 0.50f, 0.50f, 0.50f, 1.0f };
float materialShininess13 = 0.25f * 128.0f;

float materialAmbient14[] = { 0.0f, 0.1f, 0.06f, 1.0f };
float materialDiffuse14[] = { 0.0f, 0.50980392f, 0.50980392f, 1.0f };
float materialSpecular14[] = { 0.50196078f, 0.50196078f, 0.50196078f, 1.0f };
float materialShininess14 = 0.25f * 128.0f;

float materialAmbient15[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float materialDiffuse15[] = { 0.1f, 0.35f, 0.1f, 1.0f };
float materialSpecular15[] = { 0.45f, 0.55f, 0.45f, 1.0f };
float materialShininess15 = 0.25f * 128.0f;

float materialAmbient16[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float materialDiffuse16[] = { 0.5f, 0.0f, 0.0f, 1.0f };
float materialSpecular16[] = { 0.7f, 0.6f, 0.6f, 1.0f };
float materialShininess16 = 0.25f * 128.0f;

float materialAmbient17[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float materialDiffuse17[] = { 0.55f, 0.55f, 0.55f, 1.0f };
float materialSpecular17[] = { 0.70f, 0.70f, 0.70f, 1.0f };
float materialShininess17 = 0.25f * 128.0f;

float materialAmbient18[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float materialDiffuse18[] = { 0.5f, 0.5f, 0.0f, 1.0f };
float materialSpecular18[] = { 0.60f, 0.60f, 0.50f, 1.0f };
float materialShininess18 = 0.25f * 128.0f;

float materialAmbient19[] = { 0.02f, 0.02f, 0.02f, 1.0f };
float materialDiffuse19[] = { 0.01f, 0.01f, 0.01f, 1.0f };
float materialSpecular19[] = { 0.4f, 0.4f, 0.4f, 1.0f };
float materialShininess19 = 0.078125f * 128.0f;

float materialAmbient20[] = { 0.0f, 0.05f, 0.05f, 1.0f };
float materialDiffuse20[] = { 0.4f, 0.5f, 0.5f, 1.0f };
float materialSpecular20[] = { 0.04f, 0.7f, 0.7f, 1.0f };
float materialShininess20 = 0.078125f * 128.0f;

float materialAmbient21[] = { 0.0f, 0.05f, 0.0f, 1.0f };
float materialDiffuse21[] = { 0.4f, 0.5f, 0.4f, 1.0f };
float materialSpecular21[] = { 0.04f, 0.7f, 0.04f, 1.0f };
float materialShininess21 = 0.078125f * 128.0f;

float materialAmbient22[] = { 0.05f, 0.0f, 0.0f, 1.0f };
float materialDiffuse22[] = { 0.5f, 0.4f, 0.4f, 1.0f };
float materialSpecular22[] = { 0.7f, 0.04f, 0.04f, 1.0f };
float materialShininess22 = 0.078125f * 128.0f;

float materialAmbient23[] = { 0.05f, 0.05f, 0.05f, 1.0f };
float materialDiffuse23[] = { 0.5f, 0.5f, 0.5f, 1.0f };
float materialSpecular23[] = { 0.7f, 0.7f, 0.7f, 1.0f };
float materialShininess23 = 0.078125f * 128.0f;

float materialAmbient24[] = { 0.5f, 0.05f, 0.0f, 1.0f };
float materialDiffuse24[] = { 0.5f, 0.5f, 0.4f, 1.0f };
float materialSpecular24[] = { 0.7f, 0.7f, 0.04f, 1.0f };
float materialShininess24 = 0.078125f * 128.0f;

unsigned int VKeyIsPressed = 1; //Per-Vertex Lightling Toggle
unsigned int PKeyIsPressed = 0; //Per-Pixel Lightling Toggle

float updated_width = 0.0f;
float updated_height = 0.0f;

//Light Rotation
float XRotation = 0.0f;
float YRotation = 0.0f;
float ZRotation = 0.0f;

float radius = 5.0f;

unsigned int key_pressed = 0; // = 1 for X Axis; = 2 for Y Axis; = 3 for Z Axis.

bool gbLights = false;

D3D11_VIEWPORT d3dViewPort;


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
		TEXT("D3D11: Assignment_019_Per_Vertex : By Kshitij Badkas"),
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

		case 'l':
		case 'L':
			if (gbLights == false)
			{
				gbLights = true;
				fopen_s(&gpFile, gpszLogFileName, "a+");
				fprintf_s(gpFile, "LIGHTS = %d \n", gbLights);
				fclose(gpFile);
			}
			else if (gbLights == true)
			{
				gbLights = false;
				fopen_s(&gpFile, gpszLogFileName, "a+");
				fprintf_s(gpFile, "LIGHTS = %d \n", gbLights);
				fclose(gpFile);
			}
			break;

		case 'x':
		case 'X':
			key_pressed = 1;
			XRotation = 0.0f;
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "LIGHTS = %d \n", gbLights);
			fclose(gpFile);
			break;

		case 'y':
		case 'Y':
			key_pressed = 2;
			YRotation = 0.0f;
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "WNDPROC Y - key_pressed = %d \n", key_pressed);
			fclose(gpFile);
			break;


		case 'z':
		case 'Z':
			key_pressed = 3;
			ZRotation = 0.0f;
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "WNDPROC Z - key_pressed = %d \n", key_pressed);
			fclose(gpFile);
			break;

		case 'P': //PER-FRAGMENT SHADING TOGGLE
		case 'p':
			if (PKeyIsPressed == 0)
			{
				PKeyIsPressed = 1;
				VKeyIsPressed = 0;
				fopen_s(&gpFile, gpszLogFileName, "a+");
				fprintf_s(gpFile, "PER_PIXEL -> P pressed key  = %d\n", PKeyIsPressed);
				fclose(gpFile);
			}
			else if (PKeyIsPressed == 1)
			{
				PKeyIsPressed = 0;
				fopen_s(&gpFile, gpszLogFileName, "a+");
				fprintf_s(gpFile, "PER_PIXEL -> P pressed key  = %d\n", PKeyIsPressed);
				fclose(gpFile);
			}
			break;

		case 'V': //PER-VERTEX SHADING TOGGLE
		case 'v':
			if (VKeyIsPressed == 0)
			{
				VKeyIsPressed = 1;
				PKeyIsPressed = 0;
				fopen_s(&gpFile, gpszLogFileName, "a+");
				fprintf_s(gpFile, "PER_VERTEX -> V pressed key  = %d\n", VKeyIsPressed);
				fclose(gpFile);
			}
			else if (VKeyIsPressed == 1)
			{
				VKeyIsPressed = 0;
				fopen_s(&gpFile, gpszLogFileName, "a+");
				fprintf_s(gpFile, "PER_VERTEX -> V pressed key  = %d\n", VKeyIsPressed);
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

	///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// PER VERTEX - VERTEX SHADER ////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////

	//1. decalre vertex shader source code
	const char* vertexShaderSourceCode_PV =
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
		"float4 position: SV_POSITION;" \
		"float3 phong_ads_light: COLOR;" \
		"};" \
		"vertex_output main(float4 pos : POSITION, float4 normal : NORMAL)" \
		"{" \
		"vertex_output output;" \
		"if(keyPressed == 1)" \
		"{" \
		"float4 eye_coordinates = mul(worldMatrix, pos);" \
		"eye_coordinates = mul(viewMatrix, eye_coordinates);" \
		"float3 transformed_normals = (float3)normalize(mul((float3x3)worldMatrix, (float3)normal));" \
		"float3 light_direction = (float3)normalize(lightPosition - eye_coordinates);" \
		"float3 ambient = la * ka;" \
		"float3 diffuse = ld * kd * max(dot(light_direction, transformed_normals), 0.0f);" \
		"float3 reflection_vector = reflect(-light_direction, transformed_normals);" \
		"float3 viewer_vector = normalize(-eye_coordinates.xyz);" \
		"float3 specular = ls * ks * pow(max(dot(reflection_vector, viewer_vector), 0.0f), materialShininess);" \
		"output.phong_ads_light = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"output.phong_ads_light = float3(1.0f, 1.0f, 1.0f);" \
		"}" \
		"float4 position = mul(worldMatrix, pos);" \
		"position = mul(viewMatrix, position);" \
		"position = mul(projectionMatrix, position);" \
		"output.position = position;" \
		"return (output);" \
		"}";

	//2. compile vertex shader source code
	ID3DBlob* pID3DBlob_VertexShaderCode_PV = NULL;
	ID3DBlob* pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode_PV,
		lstrlenA(vertexShaderSourceCode_PV) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode_PV,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() for Vertex Shader Failed. - PER VERTEX: %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
		else
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "Some COM Error for Vertex Shader. - PER VERTEX \n");
			fclose(gpFile);
		}
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() for Vertex Shader Succeeded. - PER VERTEX\n");
		fclose(gpFile);
	}


	//3. create Vertex shader
	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode_PV->GetBufferPointer(),
		pID3DBlob_VertexShaderCode_PV->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader_PV);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateVertexShader() Failed. - PER VERTEX\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateVertexShader() Succeeded. - PER VERTEX\n");
		fclose(gpFile);
	}


	//4. Set vertex shader into pipeline
	//gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0); 
	//CALLED IN DISPLAY

	//Repeat above 4 steps for Pixel Shader

	///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// PER VERTEX - PIXEL SHADER ////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	//5. decalre pixel shader source code
	const char* pixelShaderSourceCode_PV =
		"struct vertex_output" \
		"{" \
		"float4 position: SV_POSITION;" \
		"float3 phong_ads_light: COLOR;" \
		"};" \
		"float4 main(vertex_output input) : SV_TARGET" \
		"{" \
		"float4 color = float4(input.phong_ads_light, 1.0f);" \
		"return(color);" \
		"}";

	ID3DBlob* pID3DBlob_PixelShaderCode_PV = NULL;
	pID3DBlob_Error = NULL;

	//6. compile pixel shader source code
	hr = D3DCompile(pixelShaderSourceCode_PV,
		lstrlenA(pixelShaderSourceCode_PV) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode_PV,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() for Pixel Shader Failed. - PER VERTEX : %s. \n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
		else
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "Some COM Error for Pixel Shader. - PER VERTEX\n");
			fclose(gpFile);
		}
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() for Pixel Shader Succeeded.- PER VERTEX\n");
		fclose(gpFile);
	}

	//7. create Pixel shader
	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode_PV->GetBufferPointer(),
		pID3DBlob_PixelShaderCode_PV->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader_PV);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreatePixelShader() Failed. - PER VERTEX\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreatePixelShader() Succeeded. - PER VERTEX\n");
		fclose(gpFile);
	}

	//8. set  Pixel shader into pipeline
	//gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0); //CALLED IN DISPLAY
	//pID3DBlob_PixelShaderCode->Release();
	//pID3DBlob_PixelShaderCode = NULL;

	///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// PER PIXEL - VERTEX SHADER ////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////

	const char* vertexShaderSourceCode_PP =
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
	ID3DBlob* pID3DBlob_VertexShaderCode_PP = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode_PP,
		lstrlenA(vertexShaderSourceCode_PP) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode_PP,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() for Vertex Shader Failed. - PER PIXEL: %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
		else
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "Some COM Error for Vertex Shader. - PER PIXEL \n");
			fclose(gpFile);
		}
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() for Vertex Shader Succeeded. - PER PIXEL \n");
		fclose(gpFile);
	}


	//3. create Vertex shader
	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode_PP->GetBufferPointer(),
		pID3DBlob_VertexShaderCode_PP->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader_PP);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateVertexShader() Failed. - PER PIXEL\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateVertexShader() Succeeded. - PER PIXEL\n");
		fclose(gpFile);
	}

	///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// PER PIXEL - PIXEL SHADER //////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////

	const char* pixelShaderSourceCode_PP =
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


	ID3DBlob* pID3DBlob_PixelShaderCode_PP = NULL;
	pID3DBlob_Error = NULL;

	//6. compile pixel shader source code
	hr = D3DCompile(pixelShaderSourceCode_PP,
		lstrlenA(pixelShaderSourceCode_PP) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode_PP,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() for Pixel Shader Failed. - PER PIXEL : %s. \n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
		else
		{
			fopen_s(&gpFile, gpszLogFileName, "a+");
			fprintf_s(gpFile, "Some COM Error for Pixel Shader. - PER PIXEL\n");
			fclose(gpFile);
		}
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() for Pixel Shader Succeeded.- PER PIXEL\n");
		fclose(gpFile);
	}

	//7. create Pixel shader
	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode_PP->GetBufferPointer(),
		pID3DBlob_PixelShaderCode_PP->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader_PP);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreatePixelShader() Failed. - PER PIXEL\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreatePixelShader() Succeeded. - PER PIXEL\n");
		fclose(gpFile);
	}

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

	//PER - VERTEX
	//9.b Create input layout
	hr = gpID3D11Device->CreateInputLayout(inputElementDesc, _ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode_PV->GetBufferPointer(),
		pID3DBlob_VertexShaderCode_PV->GetBufferSize(),
		&gpID3D11InputLayout_PV);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateInputLayout() Failed. - PER VERTEX\n");
		fclose(gpFile);
		if (pID3DBlob_VertexShaderCode_PV)
		{
			pID3DBlob_VertexShaderCode_PV->Release();
			pID3DBlob_VertexShaderCode_PV = NULL;
		}
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateInputLayout() Succeeded. - PER VERTEX\n");
		fclose(gpFile);
		if (pID3DBlob_VertexShaderCode_PV)
		{
			pID3DBlob_VertexShaderCode_PV->Release();
			pID3DBlob_VertexShaderCode_PV = NULL;
		}
	}

	//PER - PIXEL
	hr = gpID3D11Device->CreateInputLayout(inputElementDesc, _ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode_PP->GetBufferPointer(),
		pID3DBlob_VertexShaderCode_PP->GetBufferSize(),
		&gpID3D11InputLayout_PP);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateInputLayout() Failed. - PER PIXEL\n");
		fclose(gpFile);
		if (pID3DBlob_VertexShaderCode_PP)
		{
			pID3DBlob_VertexShaderCode_PP->Release();
			pID3DBlob_VertexShaderCode_PP = NULL;
		}
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateInputLayout() Succeeded. - PER PIXEL\n");
		fclose(gpFile);
		if (pID3DBlob_VertexShaderCode_PP)
		{
			pID3DBlob_VertexShaderCode_PP->Release();
			pID3DBlob_VertexShaderCode_PP = NULL;
		}
	}

	//9.c Set input layout in the pipeline
	//gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout); //CALLED IN DISPLAY

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

	//PER - VERTEX
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_constantBuffer);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Failed for Constant Buffer . - PER VERTEX\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Succeeded for Constant Buffer. - PER VERTEX\n");
		fclose(gpFile);
	}

	//PER - PIXEL
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_constantBuffer);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Failed for Constant Buffer . - PER PIXEL\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "gpID3D11Device::CreateBuffer() Succeeded for Constant Buffer. - PER PIXEL\n");
		fclose(gpFile);
	}

	//13. set the Constant Buffer into the pipeline
	//gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer); //CALLED IN DISPLAY

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
	gClearColor[0] = 0.25f;
	gClearColor[1] = 0.25f;
	gClearColor[2] = 0.25f;
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
	updated_width = width;
	updated_height = height;

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
	//D3D11_VIEWPORT d3dViewPort;
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

	if (VKeyIsPressed == 1)
	{
		/*
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "IN DISPLAY PER VERTEX IS 1  VKeyIsPressed = %d\n", VKeyIsPressed);
		fclose(gpFile);
		*/
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_PV, NULL, 0);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_PV, NULL, 0);
		gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout_PV);
		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer);

	}
	else if (PKeyIsPressed == 1)
	{
		/*
		fopen_s(&gpFile, gpszLogFileName, "a+");
		fprintf_s(gpFile, "IN DISPLAY PER VERTEX IS 0  VKeyIsPressed = %d\n", VKeyIsPressed);
		fclose(gpFile);
		*/

		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_PP, NULL, 0);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_PP, NULL, 0);
		gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout_PP);
		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer);
		gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer);

	}

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
	XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX wv = worldMatrix * viewMatrix;

	//18. push Transformation matrix into shader
	CBUFFER constantBuffer;
		
	if (gbLights == true)
	{
		if (VKeyIsPressed == 1)
		{
			if (key_pressed == 1)
			{
				//X AXIS ROATION
				lightPosition[0] = (float)(radius * sin(YRotation)); //x
				lightPosition[1] = 0.0f; //y
				lightPosition[2] = (float)(radius * cos(YRotation)); //z
				lightPosition[3] = 1.0f; //w
			}
			else if (key_pressed == 2)
			{
				//Y AXIS ROATION
				lightPosition[0] = 0.0f; //x
				lightPosition[1] = (float)(radius * sin(YRotation)); //y
				lightPosition[2] = (float)(radius * cos(YRotation)); //z
				lightPosition[3] = 1.0f; //w
			}
			else if (key_pressed == 3)
			{
				//Z AXIS ROATION
				lightPosition[0] = (float)(radius * sin(ZRotation)); //x
				lightPosition[1] = (float)(radius * cos(ZRotation)); //y
				lightPosition[2] = 0.0f; //z
				lightPosition[3] = 1.0f; //w
			}
			
			////////////////////////////  SPHERE 1  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * -0.3f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;
			
			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient1[0], materialAmbient1[1], materialAmbient1[2], materialAmbient1[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse1[0], materialDiffuse1[1], materialDiffuse1[2], materialDiffuse1[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular1[0], materialSpecular1[1], materialSpecular1[2], materialSpecular1[3]);
			constantBuffer.materialShininess = materialShininess1;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);
			

			////////////////////////////  SPHERE 2  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * -0.3f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient2[0], materialAmbient2[1], materialAmbient2[2], materialAmbient2[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse2[0], materialDiffuse2[1], materialDiffuse2[2], materialDiffuse2[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular2[0], materialSpecular2[1], materialSpecular2[2], materialSpecular2[3]);
			constantBuffer.materialShininess = materialShininess2;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 3  ////////////////////////////////
			
			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * -0.3f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient3[0], materialAmbient3[1], materialAmbient3[2], materialAmbient3[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse3[0], materialDiffuse3[1], materialDiffuse3[2], materialDiffuse3[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular3[0], materialSpecular3[1], materialSpecular3[2], materialSpecular3[3]);
			constantBuffer.materialShininess = materialShininess3;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 4  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * -0.3f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient4[0], materialAmbient4[1], materialAmbient4[2], materialAmbient4[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse4[0], materialDiffuse4[1], materialDiffuse4[2], materialDiffuse4[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular4[0], materialSpecular4[1], materialSpecular4[2], materialSpecular4[3]);
			constantBuffer.materialShininess = materialShininess4;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 5  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * 0.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient5[0], materialAmbient5[1], materialAmbient5[2], materialAmbient5[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse5[0], materialDiffuse5[1], materialDiffuse5[2], materialDiffuse5[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular5[0], materialSpecular5[1], materialSpecular5[2], materialSpecular5[3]);
			constantBuffer.materialShininess = materialShininess5;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 6  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * 0.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient6[0], materialAmbient6[1], materialAmbient6[2], materialAmbient6[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse6[0], materialDiffuse6[1], materialDiffuse6[2], materialDiffuse6[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular6[0], materialSpecular6[1], materialSpecular6[2], materialSpecular6[3]);
			constantBuffer.materialShininess = materialShininess6;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 7  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * 0.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient7[0], materialAmbient7[1], materialAmbient7[2], materialAmbient7[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse7[0], materialDiffuse7[1], materialDiffuse7[2], materialDiffuse7[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular7[0], materialSpecular7[1], materialSpecular7[2], materialSpecular7[3]);
			constantBuffer.materialShininess = materialShininess7;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 8  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * 0.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient8[0], materialAmbient8[1], materialAmbient8[2], materialAmbient8[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse8[0], materialDiffuse8[1], materialDiffuse8[2], materialDiffuse8[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular8[0], materialSpecular8[1], materialSpecular8[2], materialSpecular8[3]);
			constantBuffer.materialShininess = materialShininess8;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 9  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * 1.65 / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient9[0], materialAmbient9[1], materialAmbient9[2], materialAmbient9[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse9[0], materialDiffuse9[1], materialDiffuse9[2], materialDiffuse9[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular9[0], materialSpecular9[1], materialSpecular9[2], materialSpecular9[3]);
			constantBuffer.materialShininess = materialShininess9;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 10  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * 1.65 / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient10[0], materialAmbient10[1], materialAmbient10[2], materialAmbient10[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse10[0], materialDiffuse10[1], materialDiffuse10[2], materialDiffuse10[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular10[0], materialSpecular10[1], materialSpecular10[2], materialSpecular10[3]);
			constantBuffer.materialShininess = materialShininess10;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 11  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * 1.65 / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient11[0], materialAmbient11[1], materialAmbient11[2], materialAmbient11[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse11[0], materialDiffuse11[1], materialDiffuse11[2], materialDiffuse11[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular11[0], materialSpecular11[1], materialSpecular11[2], materialSpecular11[3]);
			constantBuffer.materialShininess = materialShininess11;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 12  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * 1.65 / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient12[0], materialAmbient12[1], materialAmbient12[2], materialAmbient12[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse12[0], materialDiffuse12[1], materialDiffuse12[2], materialDiffuse12[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular12[0], materialSpecular12[1], materialSpecular12[2], materialSpecular12[3]);
			constantBuffer.materialShininess = materialShininess12;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 13  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * 2.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient13[0], materialAmbient13[1], materialAmbient13[2], materialAmbient13[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse13[0], materialDiffuse13[1], materialDiffuse13[2], materialDiffuse13[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular13[0], materialSpecular13[1], materialSpecular13[2], materialSpecular13[3]);
			constantBuffer.materialShininess = materialShininess13;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 14  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * 2.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient14[0], materialAmbient14[1], materialAmbient14[2], materialAmbient14[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse14[0], materialDiffuse14[1], materialDiffuse14[2], materialDiffuse14[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular14[0], materialSpecular14[1], materialSpecular14[2], materialSpecular14[3]);
			constantBuffer.materialShininess = materialShininess14;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 15  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * 2.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient15[0], materialAmbient15[1], materialAmbient15[2], materialAmbient15[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse15[0], materialDiffuse15[1], materialDiffuse15[2], materialDiffuse15[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular15[0], materialSpecular15[1], materialSpecular15[2], materialSpecular15[3]);
			constantBuffer.materialShininess = materialShininess15;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 16  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * 2.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient16[0], materialAmbient16[1], materialAmbient16[2], materialAmbient16[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse16[0], materialDiffuse16[1], materialDiffuse16[2], materialDiffuse16[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular16[0], materialSpecular16[1], materialSpecular16[2], materialSpecular16[3]);
			constantBuffer.materialShininess = materialShininess16;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 17  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * 3.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient17[0], materialAmbient17[1], materialAmbient17[2], materialAmbient17[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse17[0], materialDiffuse17[1], materialDiffuse17[2], materialDiffuse17[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular17[0], materialSpecular17[1], materialSpecular17[2], materialSpecular17[3]);
			constantBuffer.materialShininess = materialShininess17;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 18  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * 3.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient18[0], materialAmbient18[1], materialAmbient18[2], materialAmbient18[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse18[0], materialDiffuse18[1], materialDiffuse18[2], materialDiffuse18[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular18[0], materialSpecular18[1], materialSpecular18[2], materialSpecular18[3]);
			constantBuffer.materialShininess = materialShininess18;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 19  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * 3.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient19[0], materialAmbient19[1], materialAmbient19[2], materialAmbient19[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse19[0], materialDiffuse19[1], materialDiffuse19[2], materialDiffuse19[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular19[0], materialSpecular19[1], materialSpecular19[2], materialSpecular19[3]);
			constantBuffer.materialShininess = materialShininess19;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 20  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * 3.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient20[0], materialAmbient20[1], materialAmbient20[2], materialAmbient20[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse20[0], materialDiffuse20[1], materialDiffuse20[2], materialDiffuse20[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular20[0], materialSpecular20[1], materialSpecular20[2], materialSpecular20[3]);
			constantBuffer.materialShininess = materialShininess20;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 21  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * 4.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient21[0], materialAmbient21[1], materialAmbient21[2], materialAmbient21[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse21[0], materialDiffuse21[1], materialDiffuse21[2], materialDiffuse21[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular21[0], materialSpecular21[1], materialSpecular21[2], materialSpecular21[3]);
			constantBuffer.materialShininess = materialShininess21;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 22  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * 4.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient22[0], materialAmbient22[1], materialAmbient22[2], materialAmbient22[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse22[0], materialDiffuse22[1], materialDiffuse22[2], materialDiffuse22[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular22[0], materialSpecular22[1], materialSpecular22[2], materialSpecular22[3]);
			constantBuffer.materialShininess = materialShininess22;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 23  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * 4.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient23[0], materialAmbient23[1], materialAmbient23[2], materialAmbient23[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse23[0], materialDiffuse23[1], materialDiffuse23[2], materialDiffuse23[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular23[0], materialSpecular23[1], materialSpecular23[2], materialSpecular23[3]);
			constantBuffer.materialShininess = materialShininess23;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 24  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * 4.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient24[0], materialAmbient24[1], materialAmbient24[2], materialAmbient24[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse24[0], materialDiffuse24[1], materialDiffuse24[2], materialDiffuse24[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular24[0], materialSpecular24[1], materialSpecular24[2], materialSpecular24[3]);
			constantBuffer.materialShininess = materialShininess24;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);



		}
		else if (PKeyIsPressed == 1)
		{
			if (key_pressed == 1)
			{
				//X AXIS ROATION
				lightPosition[0] = (float)(radius * sin(YRotation)); //x
				lightPosition[1] = 0.0f; //y
				lightPosition[2] = (float)(radius * cos(YRotation)); //z
				lightPosition[3] = 1.0f; //w
			}
			else if (key_pressed == 2)
			{
				//Y AXIS ROATION
				lightPosition[0] = 0.0f; //x
				lightPosition[1] = (float)(radius * sin(YRotation)); //y
				lightPosition[2] = (float)(radius * cos(YRotation)); //z
				lightPosition[3] = 1.0f; //w
			}
			else if (key_pressed == 3)
			{
				//Z AXIS ROATION
				lightPosition[0] = (float)(radius * sin(ZRotation)); //x
				lightPosition[1] = (float)(radius * cos(ZRotation)); //y
				lightPosition[2] = 0.0f; //z
				lightPosition[3] = 1.0f; //w
			}

			////////////////////////////  SPHERE 1  ////////////////////////////////
			
			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * -0.3f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient1[0], materialAmbient1[1], materialAmbient1[2], materialAmbient1[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse1[0], materialDiffuse1[1], materialDiffuse1[2], materialDiffuse1[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular1[0], materialSpecular1[1], materialSpecular1[2], materialSpecular1[3]);
			constantBuffer.materialShininess = materialShininess1;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 2  ////////////////////////////////
			
			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * -0.3f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient2[0], materialAmbient2[1], materialAmbient2[2], materialAmbient2[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse2[0], materialDiffuse2[1], materialDiffuse2[2], materialDiffuse2[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular2[0], materialSpecular2[1], materialSpecular2[2], materialSpecular2[3]);
			constantBuffer.materialShininess = materialShininess2;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 3  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * -0.3f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient3[0], materialAmbient3[1], materialAmbient3[2], materialAmbient3[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse3[0], materialDiffuse3[1], materialDiffuse3[2], materialDiffuse3[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular3[0], materialSpecular3[1], materialSpecular3[2], materialSpecular3[3]);
			constantBuffer.materialShininess = materialShininess3;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 4  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * -0.3f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient4[0], materialAmbient4[1], materialAmbient4[2], materialAmbient4[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse4[0], materialDiffuse4[1], materialDiffuse4[2], materialDiffuse4[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular4[0], materialSpecular4[1], materialSpecular4[2], materialSpecular4[3]);
			constantBuffer.materialShininess = materialShininess4;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 5  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * 0.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient5[0], materialAmbient5[1], materialAmbient5[2], materialAmbient5[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse5[0], materialDiffuse5[1], materialDiffuse5[2], materialDiffuse5[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular5[0], materialSpecular5[1], materialSpecular5[2], materialSpecular5[3]);
			constantBuffer.materialShininess = materialShininess5;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 6  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * 0.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient6[0], materialAmbient6[1], materialAmbient6[2], materialAmbient6[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse6[0], materialDiffuse6[1], materialDiffuse6[2], materialDiffuse6[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular6[0], materialSpecular6[1], materialSpecular6[2], materialSpecular6[3]);
			constantBuffer.materialShininess = materialShininess6;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 7  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * 0.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient7[0], materialAmbient7[1], materialAmbient7[2], materialAmbient7[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse7[0], materialDiffuse7[1], materialDiffuse7[2], materialDiffuse7[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular7[0], materialSpecular7[1], materialSpecular7[2], materialSpecular7[3]);
			constantBuffer.materialShininess = materialShininess7;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 8  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * 0.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient8[0], materialAmbient8[1], materialAmbient8[2], materialAmbient8[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse8[0], materialDiffuse8[1], materialDiffuse8[2], materialDiffuse8[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular8[0], materialSpecular8[1], materialSpecular8[2], materialSpecular8[3]);
			constantBuffer.materialShininess = materialShininess8;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 9  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * 1.65 / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient9[0], materialAmbient9[1], materialAmbient9[2], materialAmbient9[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse9[0], materialDiffuse9[1], materialDiffuse9[2], materialDiffuse9[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular9[0], materialSpecular9[1], materialSpecular9[2], materialSpecular9[3]);
			constantBuffer.materialShininess = materialShininess9;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 10  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * 1.65 / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient10[0], materialAmbient10[1], materialAmbient10[2], materialAmbient10[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse10[0], materialDiffuse10[1], materialDiffuse10[2], materialDiffuse10[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular10[0], materialSpecular10[1], materialSpecular10[2], materialSpecular10[3]);
			constantBuffer.materialShininess = materialShininess10;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 11  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * 1.65 / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient11[0], materialAmbient11[1], materialAmbient11[2], materialAmbient11[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse11[0], materialDiffuse11[1], materialDiffuse11[2], materialDiffuse11[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular11[0], materialSpecular11[1], materialSpecular11[2], materialSpecular11[3]);
			constantBuffer.materialShininess = materialShininess11;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 12  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * 1.65 / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient12[0], materialAmbient12[1], materialAmbient12[2], materialAmbient12[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse12[0], materialDiffuse12[1], materialDiffuse12[2], materialDiffuse12[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular12[0], materialSpecular12[1], materialSpecular12[2], materialSpecular12[3]);
			constantBuffer.materialShininess = materialShininess12;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 13  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * 2.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient13[0], materialAmbient13[1], materialAmbient13[2], materialAmbient13[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse13[0], materialDiffuse13[1], materialDiffuse13[2], materialDiffuse13[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular13[0], materialSpecular13[1], materialSpecular13[2], materialSpecular13[3]);
			constantBuffer.materialShininess = materialShininess13;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 14  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * 2.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient14[0], materialAmbient14[1], materialAmbient14[2], materialAmbient14[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse14[0], materialDiffuse14[1], materialDiffuse14[2], materialDiffuse14[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular14[0], materialSpecular14[1], materialSpecular14[2], materialSpecular14[3]);
			constantBuffer.materialShininess = materialShininess14;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 15  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * 2.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient15[0], materialAmbient15[1], materialAmbient15[2], materialAmbient15[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse15[0], materialDiffuse15[1], materialDiffuse15[2], materialDiffuse15[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular15[0], materialSpecular15[1], materialSpecular15[2], materialSpecular15[3]);
			constantBuffer.materialShininess = materialShininess15;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 16  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * 2.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient16[0], materialAmbient16[1], materialAmbient16[2], materialAmbient16[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse16[0], materialDiffuse16[1], materialDiffuse16[2], materialDiffuse16[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular16[0], materialSpecular16[1], materialSpecular16[2], materialSpecular16[3]);
			constantBuffer.materialShininess = materialShininess16;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 17  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * 3.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient17[0], materialAmbient17[1], materialAmbient17[2], materialAmbient17[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse17[0], materialDiffuse17[1], materialDiffuse17[2], materialDiffuse17[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular17[0], materialSpecular17[1], materialSpecular17[2], materialSpecular17[3]);
			constantBuffer.materialShininess = materialShininess17;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 18  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * 3.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient18[0], materialAmbient18[1], materialAmbient18[2], materialAmbient18[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse18[0], materialDiffuse18[1], materialDiffuse18[2], materialDiffuse18[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular18[0], materialSpecular18[1], materialSpecular18[2], materialSpecular18[3]);
			constantBuffer.materialShininess = materialShininess18;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 19  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * 3.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient19[0], materialAmbient19[1], materialAmbient19[2], materialAmbient19[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse19[0], materialDiffuse19[1], materialDiffuse19[2], materialDiffuse19[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular19[0], materialSpecular19[1], materialSpecular19[2], materialSpecular19[3]);
			constantBuffer.materialShininess = materialShininess19;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 20  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * 3.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient20[0], materialAmbient20[1], materialAmbient20[2], materialAmbient20[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse20[0], materialDiffuse20[1], materialDiffuse20[2], materialDiffuse20[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular20[0], materialSpecular20[1], materialSpecular20[2], materialSpecular20[3]);
			constantBuffer.materialShininess = materialShininess20;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 21  ////////////////////////////////

			d3dViewPort.TopLeftX = 0;
			d3dViewPort.TopLeftY = updated_height * 4.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient21[0], materialAmbient21[1], materialAmbient21[2], materialAmbient21[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse21[0], materialDiffuse21[1], materialDiffuse21[2], materialDiffuse21[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular21[0], materialSpecular21[1], materialSpecular21[2], materialSpecular21[3]);
			constantBuffer.materialShininess = materialShininess21;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 22  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 4;
			d3dViewPort.TopLeftY = updated_height * 4.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient22[0], materialAmbient22[1], materialAmbient22[2], materialAmbient22[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse22[0], materialDiffuse22[1], materialDiffuse22[2], materialDiffuse22[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular22[0], materialSpecular22[1], materialSpecular22[2], materialSpecular22[3]);
			constantBuffer.materialShininess = materialShininess22;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


			////////////////////////////  SPHERE 23  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width / 2;
			d3dViewPort.TopLeftY = updated_height * 4.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient23[0], materialAmbient23[1], materialAmbient23[2], materialAmbient23[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse23[0], materialDiffuse23[1], materialDiffuse23[2], materialDiffuse23[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular23[0], materialSpecular23[1], materialSpecular23[2], materialSpecular23[3]);
			constantBuffer.materialShininess = materialShininess23;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

			////////////////////////////  SPHERE 24  ////////////////////////////////

			d3dViewPort.TopLeftX = updated_width * 3.0f / 4.0f;
			d3dViewPort.TopLeftY = updated_height * 4.65f / 6.0f;
			d3dViewPort.Width = (float)updated_width / 4;
			d3dViewPort.Height = (float)updated_height / 4;
			d3dViewPort.MinDepth = 0.0f;
			d3dViewPort.MaxDepth = 1.0f;
			gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort); // Rasterization Set - hint for 24 spheres

			ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
			constantBuffer.WorldMatrix = worldMatrix;
			constantBuffer.ViewMatrix = viewMatrix;
			constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

			constantBuffer.keyPressed = 1;

			constantBuffer.la = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
			constantBuffer.ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
			constantBuffer.ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
			constantBuffer.lightPosition = XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

			constantBuffer.ka = XMVectorSet(materialAmbient24[0], materialAmbient24[1], materialAmbient24[2], materialAmbient24[3]);
			constantBuffer.kd = XMVectorSet(materialDiffuse24[0], materialDiffuse24[1], materialDiffuse24[2], materialDiffuse24[3]);
			constantBuffer.ks = XMVectorSet(materialSpecular24[0], materialSpecular24[1], materialSpecular24[2], materialSpecular24[3]);
			constantBuffer.materialShininess = materialShininess24;

			gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
			gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

		}
	}
	else
	{
		ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
		constantBuffer.WorldMatrix = worldMatrix;
		constantBuffer.ViewMatrix = viewMatrix;
		constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

		constantBuffer.keyPressed = 0;
		constantBuffer.keyPressed = 0;

		gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);
		gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);

	}

	

	//glUniformMatrix4fv //3rd , 5th, 6th Bounding Box params (used in Collision Detection)

	//gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_constantBuffer, 0, NULL, &constantBuffer, 0, 0);


	//19. Draw
	gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);


	//ANIMATION LOGIC
	XRotation += 0.00015f;
	YRotation += 0.00015f;
	ZRotation += 0.00015f;
	if (XRotation >= 360.0f)
	{
		XRotation = 0.0f;
	}
	if (YRotation >= 360.0f)
	{
		YRotation = 0.0f;
	}
	if (ZRotation >= 360.0f)
	{
		ZRotation = 0.0f;
	}

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

	if (gpID3D11Buffer_constantBuffer)
	{
		gpID3D11Buffer_constantBuffer->Release();
		gpID3D11Buffer_constantBuffer = NULL;
	}

	if (gpID3D11InputLayout_PV)
	{
		gpID3D11InputLayout_PV->Release();
		gpID3D11InputLayout_PV = NULL;
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

	if (gpID3D11PixelShader_PV)
	{
		gpID3D11PixelShader_PV->Release();
		gpID3D11PixelShader_PV = NULL;
	}

	if (gpID3D11VertexShader_PV)
	{
		gpID3D11VertexShader_PV->Release();
		gpID3D11VertexShader_PV = NULL;
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
