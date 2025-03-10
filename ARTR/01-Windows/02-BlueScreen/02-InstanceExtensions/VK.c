#include <windows.h>
#include <stdio.h>

#include "VK.h"

// Vulkan related header files
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>	// dont worry if error - build.bat is used.

// Vulkan related library
#pragma comment(lib, "vulkan-1.lib")

#define WINWIDTH 800
#define WINHEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variables
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};
BOOL gbFullscreen = FALSE;
HWND ghwnd = NULL;
FILE *gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
const char *gpszAppName = "ARTR";

// Vulkan related global variable
uint32_t enabledInstanceExtensionCount = 0;

// Instance extension related variable
// VK_KHR_SURFACE_EXTENSION_NAME & VK_KHR_WIN32_SURFACE_EXTENSION_NAME
const char *enabledInstanceExtensionName_array[2];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Prototype
	VkResult Initialize(void);
	void Display(void);
	void Update();
	void UnInitialize(void);

	// Local variables
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[255];
	int iResult = 0;
	BOOL bDone = FALSE;
	VkResult vkresult = VK_SUCCESS;

	// Code
	if ((gpFile = fopen("Log.txt", "w")) == NULL)
	{
		printf("Error : Can't Open File. ");
		MessageBox(NULL, TEXT("Can't Open File"), TEXT("ERROR !"), MB_OK);
		exit(0);
	}

	fprintf(gpFile, "\nWinMain() -> Program started successfully.\n");

	wsprintf(szAppName, TEXT("%s"), gpszAppName);
	fprintf(gpFile, "\nWinMain() -> szAppName : %s ", szAppName);

	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);

	int xmid = x / 2;
	int ymid = y / 2;

	x = xmid - (800 / 2);
	y = ymid - (600 / 2);

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

	hwnd = CreateWindowEx(WS_EX_ACCEPTFILES, szAppName,
												TEXT("KAB: Vulkan"),
												WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
												x,
												y,
												WINWIDTH,
												WINHEIGHT,
												NULL,
												NULL,
												hInstance,
												NULL);

	ghwnd = hwnd;

	vkresult = Initialize();

	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nWinMain() -> Initialize() failed.");
		MessageBox(hwnd, TEXT("WinMain() -> Initialize() Failed!"), TEXT("Error"), MB_OK | MB_ICONERROR);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fprintf(gpFile, "\nWinMain() -> Initialize() Succeeded.");
		MessageBox(hwnd, TEXT("WinMain() -> Initialize() Succeeded!"), TEXT("Success"), MB_OK | MB_ICONINFORMATION);
	}

	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd);

	SetFocus(hwnd);

	// GAME LOOP
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				// rendering function
				Display();
				Update();
			}
		}
	}

	UnInitialize();

	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function prototype
	void Resize(int, int);
	void UnInitialize(void);
	void ToggleFullScreen(void);

	// Local Variable
	TCHAR str[] = TEXT("HELLO WORLD !!! ");

	switch (iMsg)
	{

	case WM_CREATE:
		fprintf(gpFile, "\n WndProc() -> Program Started Successfully\n");
		// memset
		// memset()
		// wpPrev = sizeof()
		break;
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND:
		return (0);

	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		default:
			break;
		}
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		// Case W - activating & deactivating the WIREFRAME MODE
		case 'f':
		case 'F':
			ToggleFullScreen();
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		fprintf(gpFile, "\nWndProc() -> Program Terminated Successfully\n");
		UnInitialize();
		PostQuitMessage(0);
		break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{

	MONITORINFO mi = {sizeof(MONITORINFO)};

	if (gbFullscreen == FALSE)
	{

		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

				SetWindowPos(ghwnd,
										 HWND_TOP,
										 mi.rcMonitor.left,
										 mi.rcMonitor.top,
										 mi.rcMonitor.right - mi.rcMonitor.left,
										 mi.rcMonitor.bottom - mi.rcMonitor.top,
										 SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullscreen = TRUE;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
								 HWND_TOP,
								 0,
								 0,
								 0,
								 0,
								 SWP_NOMOVE | SWP_NOSIZE |
										 SWP_NOOWNERZORDER | SWP_NOZORDER |
										 SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullscreen = FALSE;
	}
}

VkResult Initialize(void)
{
	// Function declaration
	VkResult FillInstanceExtensionName(void);

	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	// code
	vkresult = FillInstanceExtensionName();

	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nInitialize() -> FillInstanceExtensionName() failed.");
	}
	else
	{
		fprintf(gpFile, "\nInitialize() -> FillInstanceExtensionName() Succeeded.");
	}

	return (vkresult);
}

void Resize(int width, int height)
{
	// code
	if (height == 0)
	{
		width = 1;
	}
}

void Display(void)
{
	// code
}

void Update()
{
	// code
}

void UnInitialize(void)
{
	// Fullscreen window to be restored first (Toggle fullscreen) and uninitialze
	if (gbFullscreen == TRUE)
	{

		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
								 HWND_TOP,
								 0,
								 0,
								 0,
								 0,
								 SWP_NOMOVE | SWP_NOSIZE |
										 SWP_NOOWNERZORDER | SWP_NOZORDER |
										 SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	if (gpFile)
	{
		fclose(gpFile);
		gpFile = NULL;
	}
}

////////////////////////////////////////// VULKAN RELATED FUNCTIONS //////////////////////////////////////////

VkResult FillInstanceExtensionName(void)
{
	// Function declaration
	// VkResult FillInstanceExtensionName(void);

	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	// STEP 1:
	uint32_t instanceExtensionCount = 0;
	vkresult = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nFillInstanceExtensionName() -> 1st call to vkEnumerateInstanceExtensionProperties() failed.");
	}
	else
	{
		fprintf(gpFile, "\nFillInstanceExtensionName() -> 1st call to vkEnumerateInstanceExtensionProperties() Succeeded.");
	}


	// STEP 2 : Allocate and fill struct VkExtensionProperties array correspoding to above count
	VkExtensionProperties *vkExtensionProperties_array = NULL;
	vkExtensionProperties_array = (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * instanceExtensionCount);
	// Error checking for malloc (Avoiding for the sake of brewity)
	// ToDo

	vkresult = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, vkExtensionProperties_array);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nFillInstanceExtensionName() -> 2nd call to vkEnumerateInstanceExtensionProperties() failed.");
	}
	else
	{
		fprintf(gpFile, "\nFillInstanceExtensionName() -> 2nd call to vkEnumerateInstanceExtensionProperties() Succeeded.");
	}

	// STEP 3 : Fill & display a local string array of extension name obtained from VkExtesionProperties array
	char **instanceExtensionNames_array = NULL;
	instanceExtensionNames_array = (char **)malloc(sizeof(char *) * instanceExtensionCount);
	// error checking malloc

	for (uint32_t i = 0; i < instanceExtensionCount; i++)
	{
		instanceExtensionNames_array[i] = (char *)malloc(sizeof(char) * strlen(vkExtensionProperties_array[i].extensionName) + 1);
		memcpy(instanceExtensionNames_array[i], vkExtensionProperties_array[i].extensionName, strlen(vkExtensionProperties_array[i].extensionName) + 1);
		fprintf(gpFile, "\nFillInstanceExtensionName() -> Vulkan Extions Name = %s\n", instanceExtensionNames_array[i]);
	}

	// STEP 4: As not required here onwards, free vkExtensionProperties_array
	free(vkExtensionProperties_array);
	vkExtensionProperties_array = NULL;

	// STEP 5: Find whether above extension names contain our required 2 extensions -
	VkBool32 vulkanSurfaceExtensionFound = VK_FALSE;
	VkBool32 win32SurfaceExtensionFound = VK_FALSE;

	for (uint32_t i = 0; i < instanceExtensionCount; i++)
	{
		if (strcmp(instanceExtensionNames_array[i], VK_KHR_SURFACE_EXTENSION_NAME) == 0)
		{
			vulkanSurfaceExtensionFound = VK_TRUE;
			enabledInstanceExtensionName_array[enabledInstanceExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
		}

		if (strcmp(instanceExtensionNames_array[i], VK_KHR_WIN32_SURFACE_EXTENSION_NAME) == 0)
		{
			win32SurfaceExtensionFound = VK_TRUE;
			enabledInstanceExtensionName_array[enabledInstanceExtensionCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
		}
	} // for loop

	// STEP 6 : Free local string array
	for (uint32_t i = 0; i < instanceExtensionCount; i++)
	{
		free(instanceExtensionNames_array[i]);
	}

	free(instanceExtensionNames_array);

	// STEP 7 : Print whether required extension names are supoorted or not
	if (vulkanSurfaceExtensionFound == VK_FALSE)
	{
		vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hard coded failure
		fprintf(gpFile, "\nFillInstanceExtensionName() -> VK_KHR_SURFACE_EXTENSION_NAME NOT FOUND!");
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\nFillInstanceExtensionName() -> VK_KHR_SURFACE_EXTENSION_NAME FOUND!");
	}
	if (win32SurfaceExtensionFound == VK_FALSE)
	{
		vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hard coded failure
		fprintf(gpFile, "\nFillInstanceExtensionName() -> VK_KHR_WIN32_SURFACE_EXTENSION_NAME NOT FOUND!");
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\nFillInstanceExtensionName() -> VK_KHR_WIN32_SURFACE_EXTENSION_NAME FOUND!");
	}

	// STEP 8 : Print only supported exstension names
	for (uint32_t i = 0; i < enabledInstanceExtensionCount; i++)
	{
		fprintf(gpFile, "\nFillInstanceExtensionName() -> enabledInstanceExtensionName = %s", enabledInstanceExtensionName_array[i]);
	}

	return (vkresult);
}
