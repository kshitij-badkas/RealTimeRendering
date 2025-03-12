#include <windows.h>
#include <stdio.h>

#include "VK.h"

// Vulkan related header files
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h> // dont worry if error - build.bat is used.

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

// Instance
VkInstance vkinstance = VK_NULL_HANDLE;

// Vulkan presentation surface
VkSurfaceKHR vksurfaceKHR = VK_NULL_HANDLE;

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
	// FUnction declaration
	VkResult createVulkanInstance(void);
	VkResult getSupportedSurface(void);

	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	// code
	vkresult = createVulkanInstance();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nInitialize() -> createVulkanInstance() failed.");
	}
	else
	{
		fprintf(gpFile, "\nInitialize() -> createVulkanInstance() Succeeded.");
	}

	// create vulkan presentation surface
	vkresult = getSupportedSurface();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nInitialize() -> getSupportedSurface() failed.");
	}
	else
	{
		fprintf(gpFile, "\nInitialize() -> getSupportedSurface() Succeeded.");
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

	if(vksurfaceKHR){
		// destroy fun() is generic not; plaform specific
		vkDestroySurfaceKHR(vkinstance, vksurfaceKHR, NULL);
		vksurfaceKHR = VK_NULL_HANDLE;
		fprintf(gpFile, "\nUnInitialize() -> vkDestroySurfaceKHR() Succeeded.");
	}

	// STEP 5 : Destroy Vulkan instance
	if (vkinstance)
	{
		vkDestroyInstance(vkinstance, NULL);
		vkinstance = VK_NULL_HANDLE;
		fprintf(gpFile, "\nUnInitialize() -> vkDestroyInstance() Succeeded.");
	}

	if (gpFile)
	{
		fclose(gpFile);
		gpFile = NULL;
	}
}

////////////////////////////////////////// VULKAN RELATED FUNCTIONS //////////////////////////////////////////

VkResult createVulkanInstance(void)
{
	// Function declaration
	VkResult FillInstanceExtensionName(void);

	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	// code
	// STEP 1 :
	vkresult = FillInstanceExtensionName();

	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\ncreateVulkanInstance() -> FillInstanceExtensionName() failed.");
	}
	else
	{
		fprintf(gpFile, "\ncreateVulkanInstance() -> FillInstanceExtensionName() Succeeded.");
	}

	// STEP 2: Insitialze struct VkApplicationInfo
	VkApplicationInfo vkApplicationInfo;
	memset((void *)&vkApplicationInfo, 0, sizeof(VkApplicationInfo));

	vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkApplicationInfo.pNext = NULL;
	vkApplicationInfo.pApplicationName = gpszAppName;
	vkApplicationInfo.applicationVersion = 1;
	vkApplicationInfo.pEngineName = gpszAppName;
	vkApplicationInfo.engineVersion = 1;
	vkApplicationInfo.apiVersion = VK_API_VERSION_1_3;

	// STEP 3: Initialize struct VkInstanceCreateInfo
	VkInstanceCreateInfo vkInstanceCreateInfo;
	memset((void *)&vkInstanceCreateInfo, 0, sizeof(VkInstanceCreateInfo));

	vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vkInstanceCreateInfo.pNext = NULL;
	vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
	vkInstanceCreateInfo.enabledExtensionCount = enabledInstanceExtensionCount;
	vkInstanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensionName_array;

	// STEP 4: call vkCreateInstance() to get VkInstance
	vkresult = vkCreateInstance(&vkInstanceCreateInfo, NULL, &vkinstance);
	if (vkresult == VK_ERROR_INCOMPATIBLE_DRIVER)
	{
		fprintf(gpFile, "\ncreateVulkanInstance() -> vkCreateInstance() Failed due to incompatibl driver (%d).", vkresult);
		return (vkresult);
	}
	else if (vkresult == VK_ERROR_EXTENSION_NOT_PRESENT)
	{
		fprintf(gpFile, "\ncreateVulkanInstance() -> vkCreateInstance() Failed due to required extension not present (%d).", vkresult);
		return (vkresult);
	}
	else if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\ncreateVulkanInstance() -> vkCreateInstance() Failed due to unknown reason (%d).", vkresult);
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\ncreateVulkanInstance() -> vkCreateInstance() Succeeded!");
		return (vkresult);
	}
}

VkResult getSupportedSurface(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;
	VkWin32SurfaceCreateInfoKHR vkwin32SurfaceCreateInfoKHR;
	memset((void *)&vkwin32SurfaceCreateInfoKHR, 0, sizeof(VkWin32SurfaceCreateInfoKHR));

	vkwin32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	vkwin32SurfaceCreateInfoKHR.pNext = NULL;
	vkwin32SurfaceCreateInfoKHR.flags = 0;
	vkwin32SurfaceCreateInfoKHR.hinstance = (HINSTANCE)GetWindowLongPtr(ghwnd, GWLP_HINSTANCE); // can also work with getModuleHandler()
	vkwin32SurfaceCreateInfoKHR.hwnd = ghwnd;

	vkresult = vkCreateWin32SurfaceKHR(vkinstance, &vkwin32SurfaceCreateInfoKHR, NULL, &vksurfaceKHR);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\ngetSupportedSurface() -> vkWin32SurfaceCreateFlagsKHR() failed.");
	}
	else
	{
		fprintf(gpFile, "\ngetSupportedSurface() -> vkWin32SurfaceCreateFlagsKHR() Succeeded.");
	}
	
	return (vkresult);

}

VkResult FillInstanceExtensionName(void)
{
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
