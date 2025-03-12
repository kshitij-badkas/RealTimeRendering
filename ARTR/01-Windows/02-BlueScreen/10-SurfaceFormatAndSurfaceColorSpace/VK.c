// USING SIMILAR STEPS OF INSTANCE EXTENSIONS
// WE ARE GOING TO GET DEVICE EXTENSIONS

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

// Vulkan physical device realted
VkPhysicalDevice vkphysicalDevice_selected = VK_NULL_HANDLE;
uint32_t graphicsQueueFamilyIndex_selected = UINT32_MAX;
VkPhysicalDeviceMemoryProperties vkphysicalDeviceMemoryProperties;
uint32_t physicalDeviceCount = 0;
VkPhysicalDevice *vkphysicalDevice_array = NULL;

// Device extension related variable
uint32_t enabledDeviceExtensionCount = 0;
// VK_KHR_SWAPCHAIN_EXTENSION_NAME
const char *enabledDeviceExtensionName_array[1]; // Array size will change in Mac, hence keep arr of size 1 here.

// Vulkan device
VkDevice vkdevice = VK_NULL_HANDLE;

// Device queue
VkQueue vkqueue = VK_NULL_HANDLE;

// color format and color space
VkFormat vkformat;
VkFormat vkformat_color = VK_FORMAT_UNDEFINED;
VkColorSpaceKHR vkcolorSpaceKHR = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

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
	// case WM_ERASEBKGND:
	// 	return (0);

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
		// fprintf(gpFile, "\nWndProc() -> Program Terminated Successfully\n");
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
	VkResult getPhysicalDevice(void);
	VkResult printVkInfo(void);
	VkResult createVulkanDevice(void);
	void getDeviceQueue(void);
	VkResult getPhysicalDeviceSurfaceFormatAndColorSpace(void);

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

	// enumaret and select  required physical device and its queue family index
	vkresult = getPhysicalDevice();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nInitialize() -> getPhysicalDevice() failed : %d.\n", vkresult);
	}
	else
	{
		fprintf(gpFile, "\nInitialize() -> getPhysicalDevice() Succeeded: %d.\n", vkresult);
	}

	// printVkInfo print vulkan info
	vkresult = printVkInfo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nInitialize() -> printVkInfo() failed : %d.\n", vkresult);
	}
	else
	{
		fprintf(gpFile, "\nInitialize() -> printVkInfo() Succeeded: %d.\n", vkresult);
	}

	// create vulkan device (logical)
	vkresult = createVulkanDevice();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nInitialize() -> createVulkanDevice() failed : %d.\n", vkresult);
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\nInitialize() -> createVulkanDevice() Succeeded: %d.\n", vkresult);
	}

	// get device queue
	getDeviceQueue();

	// color format and color space
	vkresult = getPhysicalDeviceSurfaceFormatAndColorSpace();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nInitialize() -> getPhysicalDeviceSurfaceFormatAndColorSpace() failed : %d.\n", vkresult);
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\nInitialize() -> getPhysicalDeviceSurfaceFormatAndColorSpace() Succeeded: %d.\n", vkresult);
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

	// No need to destroy / uninitialize Device Queue.

	// Destroy vulkan device
	if (vkdevice)
	{
		vkDeviceWaitIdle(vkdevice);
		fprintf(gpFile, "\nUnInitialize() -> vkDeviceWaitIdle() IS DONE.");
		vkDestroyDevice(vkdevice, NULL);
		vkdevice = VK_NULL_HANDLE;
		fprintf(gpFile, "\nUnInitialize() -> vkDestroyDevice() IS DONE.");
	}

	// NO NEED TO DESTROY SELECTED PHYSICAL DEVICE

	if (vksurfaceKHR)
	{
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

VkResult getPhysicalDevice(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;
	VkBool32 bFound = VK_FALSE;
	VkQueueFamilyProperties *vkQueueFamilyProperties_array = NULL;
	VkBool32 *isQueueSurfaceSupported_array = NULL;
	VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures;

	vkresult = vkEnumeratePhysicalDevices(vkinstance, &physicalDeviceCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\ngetPhysicalDevice() -> 1st call to vkEnumeratePhysicalDevices() failed : %d.\n", vkresult);
		return (vkresult);
	}
	else if (physicalDeviceCount == 0)
	{
		fprintf(gpFile, "\ngetPhysicalDevice() -> vkEnumeratePhysicalDevices() resulted in 0 devices.\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\ngetPhysicalDevice() -> 1st call to vkEnumeratePhysicalDevices() Succeeded : %d.\n", vkresult);
		// return (vkresult);
	}

	vkphysicalDevice_array = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	vkresult = vkEnumeratePhysicalDevices(vkinstance, &physicalDeviceCount, vkphysicalDevice_array);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\ngetPhysicalDevice() -> 2nd call to vkEnumeratePhysicalDevices() failed : %d.\n", vkresult);
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\ngetPhysicalDevice() -> 2nd call to vkEnumeratePhysicalDevices() Succeeded : %d.\n", vkresult);
		// return (vkresult);
	}

	// if physical device is present then it must support at least 1 queue family
	for (uint32_t i = 0; i < physicalDeviceCount; i++)
	{
		uint32_t queueCount = UINT32_MAX;
		vkGetPhysicalDeviceQueueFamilyProperties(
				vkphysicalDevice_array[i],
				&queueCount,
				NULL);

		vkQueueFamilyProperties_array = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * queueCount);

		vkGetPhysicalDeviceQueueFamilyProperties(
				vkphysicalDevice_array[i],
				&queueCount,
				vkQueueFamilyProperties_array);

		isQueueSurfaceSupported_array = (VkBool32 *)malloc(sizeof(VkBool32) * queueCount);

		// 1st nested loop
		for (uint32_t j = 0; j < queueCount; j++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(
					vkphysicalDevice_array[i],
					j,
					vksurfaceKHR,
					&isQueueSurfaceSupported_array[j]);
		}

		// 2nd nested loop
		for (uint32_t j = 0; j < queueCount; j++)
		{
			// has - Bitwise &
			if (vkQueueFamilyProperties_array[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (isQueueSurfaceSupported_array[j] == VK_TRUE)
				{
					vkphysicalDevice_selected = vkphysicalDevice_array[i];
					graphicsQueueFamilyIndex_selected = j;
					bFound = VK_TRUE;
					break;
				}
			}
		}

		if (isQueueSurfaceSupported_array)
		{
			free(isQueueSurfaceSupported_array);
			isQueueSurfaceSupported_array = NULL;
			fprintf(gpFile, "\ngetPhysicalDevice() -> isQueueSurfaceSupported_array FREED.\n");
		}
		if (vkQueueFamilyProperties_array)
		{
			free(vkQueueFamilyProperties_array);
			vkQueueFamilyProperties_array = NULL;
			fprintf(gpFile, "\ngetPhysicalDevice() -> vkQueueFamilyProperties_array FREED.\n");
		}

		if (bFound == VK_TRUE)
		{
			break;
		}
	}

	if (bFound == VK_TRUE)
	{
		fprintf(gpFile, "\ngetPhysicalDevice() is SUCCEEDED to select required physical device with Grpahics Enabled.\n");
		// if (vkphysicalDevice_array)
		// {
		// 	free(vkphysicalDevice_array);
		// 	vkphysicalDevice_array = NULL;
		// 	fprintf(gpFile, "\ngetPhysicalDevice() -> vkphysicalDevice_array FREED.\n");
		// }
	}
	else
	{
		free(vkphysicalDevice_array);
		vkphysicalDevice_array = NULL;
		fprintf(gpFile, "\ngetPhysicalDevice() -> in ELSE : vkphysicalDevice_array FREED.\n");
		fprintf(gpFile, "\ngetPhysicalDevice() -> in ELSE : is FAILED to obtain Grpahics suppotrted physical device.\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return (vkresult);
	}

	memset((void *)&vkphysicalDeviceMemoryProperties, 0, sizeof(VkPhysicalDeviceMemoryProperties));
	vkGetPhysicalDeviceMemoryProperties(vkphysicalDevice_selected, &vkphysicalDeviceMemoryProperties);

	memset((void *)&vkPhysicalDeviceFeatures, 0, sizeof(VkPhysicalDeviceFeatures));
	vkGetPhysicalDeviceFeatures(vkphysicalDevice_selected, &vkPhysicalDeviceFeatures);

	if (vkPhysicalDeviceFeatures.tessellationShader)
	{
		fprintf(gpFile, "\ngetPhysicalDevice() : selected phycisal device supports TESSELLATION SHADER.\n");
	}
	else
	{
		fprintf(gpFile, "\ngetPhysicalDevice() : selected phycisal device DOES NOT support TESSELLATION SHADER.\n");
	}

	if (vkPhysicalDeviceFeatures.geometryShader)
	{
		fprintf(gpFile, "\ngetPhysicalDevice() : selected phycisal device supports GEOMETRY SHADER.\n");
	}
	else
	{
		fprintf(gpFile, "\ngetPhysicalDevice() : selected phycisal device DOES NOT support GEOMETRY SHADER.\n");
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

VkResult printVkInfo(void)
{
	// variable
	VkResult vkresult = VK_SUCCESS;
	VkPhysicalDeviceProperties vkphysicalDeviceProperties;

	// may be depracated so use VK_API_**
	// uint32_t majorVersion = VK_VERSION_MAJOR(vkphysicalDeviceProperties.apiVersion);
	// uint32_t minorVersion = VK_VERSION_MINOR(vkphysicalDeviceProperties.apiVersion);
	// uint32_t patchVersion = VK_VERSION_PATCH(vkphysicalDeviceProperties.apiVersion);

	// code
	fprintf(gpFile, "\n***************VULKAN INFO***************\n");
	for (uint32_t i = 0; i < physicalDeviceCount; i++)
	{
		memset((void *)&vkphysicalDeviceProperties, 0, sizeof(VkPhysicalDeviceProperties));
		vkGetPhysicalDeviceProperties(vkphysicalDevice_array[i], &vkphysicalDeviceProperties);

		uint32_t majorVersion = VK_API_VERSION_MAJOR(vkphysicalDeviceProperties.apiVersion);
		uint32_t minorVersion = VK_API_VERSION_MINOR(vkphysicalDeviceProperties.apiVersion);
		uint32_t patchVersion = VK_API_VERSION_PATCH(vkphysicalDeviceProperties.apiVersion);

		fprintf(gpFile, "Api Version : %d.%d.%d\n", majorVersion, minorVersion, patchVersion);
		fprintf(gpFile, "Device Name : %s\n", vkphysicalDeviceProperties.deviceName);
		fprintf(gpFile, "Device Type : %d\n", vkphysicalDeviceProperties.deviceType);

		switch (vkphysicalDeviceProperties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			fprintf(gpFile, "Device Type : Integrated GPU (iGPU)\n");
			break;

		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			fprintf(gpFile, "Device Type : Discrete GPU (dGPU)\n");
			break;

		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			fprintf(gpFile, "Device Type : Virtual GPU (vGPU)\n");
			break;

		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			fprintf(gpFile, "Device Type : CPU\n");
			break;

		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			fprintf(gpFile, "Device Type : Other\n");
			break;

		default:
			fprintf(gpFile, "Device Type : UNKNOWN\n");
			break;
		} // switch

		fprintf(gpFile, "Vendor ID : 0x%04x\n", vkphysicalDeviceProperties.vendorID);
		fprintf(gpFile, "Device ID : 0x%04x\n", vkphysicalDeviceProperties.deviceID);
	}
	// free physical device array
	if (vkphysicalDevice_array)
	{
		free(vkphysicalDevice_array);
		vkphysicalDevice_array = NULL;
		fprintf(gpFile, "\nprintVkInfo() -> vkphysicalDevice_array FREED.\n");
	}

	return (vkresult);
}

VkResult FillDeviceExtensionName(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	// STEP 1:
	uint32_t deviceExtensionCount = 0;
	vkresult = vkEnumerateDeviceExtensionProperties(
			vkphysicalDevice_selected,
			NULL,
			&deviceExtensionCount,
			NULL);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nFillDeviceExtensionName() -> 1st call to vkEnumerateDeviceExtensionProperties() failed.");
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\nFillDeviceExtensionName() -> 1st call to vkEnumerateDeviceExtensionProperties() Succeeded.");
	}

	// STEP 2 : Allocate and fill struct VkExtensionProperties array correspoding to above count
	VkExtensionProperties *vkExtensionProperties_array = NULL;
	vkExtensionProperties_array = (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);

	vkresult = vkEnumerateDeviceExtensionProperties(
			vkphysicalDevice_selected,
			NULL,
			&deviceExtensionCount,
			vkExtensionProperties_array);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\nFillDeviceExtensionName() -> 2nd call to vkEnumerateDeviceExtensionProperties() failed.");
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\nFillDeviceExtensionName() -> 2nd call to vkEnumerateDeviceExtensionProperties() Succeeded.");
	}

	// STEP 3 : Fill & display a local string array of extension name obtained from VkExtesionProperties array
	char **deviceExtensionNames_array = NULL;
	deviceExtensionNames_array = (char **)malloc(sizeof(char *) * deviceExtensionCount);
	// error checking malloc

	for (uint32_t i = 0; i < deviceExtensionCount; i++)
	{
		deviceExtensionNames_array[i] = (char *)malloc(sizeof(char) * strlen(vkExtensionProperties_array[i].extensionName) + 1);
		memcpy(deviceExtensionNames_array[i], vkExtensionProperties_array[i].extensionName, strlen(vkExtensionProperties_array[i].extensionName) + 1);
		fprintf(gpFile, "\nFillDeviceExtensionName() -> Vulkan Device Extension Name = %s\n", deviceExtensionNames_array[i]);
	}

	// STEP 4: As not required here onwards, free vkExtensionProperties_array
	free(vkExtensionProperties_array);
	vkExtensionProperties_array = NULL;

	// STEP 5: Find whether above extension names contain our required 2 extensions -
	VkBool32 vulkanSwapchainExtensionFound = VK_FALSE;

	for (uint32_t i = 0; i < deviceExtensionCount; i++)
	{
		if (strcmp(deviceExtensionNames_array[i], VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
		{
			vulkanSwapchainExtensionFound = VK_TRUE;
			enabledDeviceExtensionName_array[enabledDeviceExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
		}
	} // for loop

	// STEP 6 : Free local string array
	for (uint32_t i = 0; i < deviceExtensionCount; i++)
	{
		free(deviceExtensionNames_array[i]);
	}
	free(deviceExtensionNames_array);

	// STEP 7 : Print whether required extension names are supoorted or not
	if (vulkanSwapchainExtensionFound == VK_FALSE)
	{
		vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hard coded failure
		fprintf(gpFile, "\nFillDeviceExtensionName() -> VK_KHR_SWAPCHAIN_EXTENSION_NAME NOT FOUND!");
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\nFillDeviceExtensionName() -> VK_KHR_SWAPCHAIN_EXTENSION_NAME FOUND!");
	}

	// STEP 8 : Print only supported device exstension names
	for (uint32_t i = 0; i < enabledDeviceExtensionCount; i++)
	{
		fprintf(gpFile, "\nFillDeviceExtensionName() -> enabledDeviceExtensionCount = %s", enabledDeviceExtensionName_array[i]);
	}

	return (vkresult);
}

VkResult createVulkanDevice(void)
{
	// function declaration
	VkResult FillDeviceExtensionName(void);

	// variable declaration
	VkResult vkresult = VK_SUCCESS;

	// code
	// Fill Device Extensions
	vkresult = FillDeviceExtensionName();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\ncreateVulkanDevice() -> FillDeviceExtensionName() failed.");
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\ncreateVulkanDevice() -> FillDeviceExtensionName() Succeeded.");
	}

	// newly added code
	float queuePriorities[1];
	queuePriorities[0] = 1.0;

	VkDeviceQueueCreateInfo vkdeviceQueueCreateInfo;
	memset((void *)&vkdeviceQueueCreateInfo, 0, sizeof(VkDeviceQueueCreateInfo));
	vkdeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	vkdeviceQueueCreateInfo.pNext = NULL;
	vkdeviceQueueCreateInfo.flags = 0;
	vkdeviceQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex_selected;
	vkdeviceQueueCreateInfo.queueCount = 1;
	vkdeviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	// initialize VkDeviceCreateInfo struct
	VkDeviceCreateInfo vkDeviceCreateInfo;
	memset((void *)&vkDeviceCreateInfo, 0, sizeof(VkDeviceCreateInfo));
	vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vkDeviceCreateInfo.pNext = NULL;
	vkDeviceCreateInfo.flags = 0;
	vkDeviceCreateInfo.enabledExtensionCount = enabledDeviceExtensionCount;
	vkDeviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensionName_array;
	vkDeviceCreateInfo.enabledLayerCount = 0;
	vkDeviceCreateInfo.ppEnabledLayerNames = NULL;
	vkDeviceCreateInfo.pEnabledFeatures = NULL;
	vkDeviceCreateInfo.queueCreateInfoCount = 1;
	vkDeviceCreateInfo.pQueueCreateInfos = &vkdeviceQueueCreateInfo;

	vkresult = vkCreateDevice(
			vkphysicalDevice_selected,
			&vkDeviceCreateInfo,
			NULL,
			&vkdevice);

	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\ncreateVulkanDevice() -> vkCreateDevice() failed.");
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\ncreateVulkanDevice() -> vkCreateDevice() Succeeded.");
	}

	return (vkresult);
}

void getDeviceQueue(void)
{
	// code
	vkGetDeviceQueue(
			vkdevice,
			graphicsQueueFamilyIndex_selected,
			0,
			&vkqueue);

	if (vkqueue == VK_NULL_HANDLE)
	{
		fprintf(gpFile, "getDeviceQueue() : vkGetDeviceQueue() returned NULL for vkqueue.\n");
		return;
	}
	else
	{
		fprintf(gpFile, "getDeviceQueue() : vkGetDeviceQueue() SUCCEEDED for vkqueue.\n");
	}
}

VkResult getPhysicalDeviceSurfaceFormatAndColorSpace(void)
{
	// variable declaration
	VkResult vkresult = VK_SUCCESS;
	uint32_t formatCount = 0;

	// code
	// get the count of supported color format
	vkGetPhysicalDeviceSurfaceFormatsKHR(
			vkphysicalDevice_selected,
			vksurfaceKHR,
			&formatCount,
			NULL);

	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\ngetPhysicalDeviceSurfaceFormatAndColorSpace() -> vkGetPhysicalDeviceSurfaceFormatsKHR() failed : %d.\n", vkresult);
		return (vkresult);
	}
	else if (formatCount == 0)
	{
		fprintf(gpFile, "\ngetPhysicalDeviceSurfaceFormatAndColorSpace() -> formatCount = 0 : return.\n");
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\ngetPhysicalDeviceSurfaceFormatAndColorSpace() -> vkGetPhysicalDeviceSurfaceFormatsKHR() Succeeded: %d.\n", vkresult);
	}

	// declare and allocate VkSurfaceFormatKHR
	VkSurfaceFormatKHR *vksurfaceFormatKHR_array = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));

	// 2nd call
	vkresult = vkGetPhysicalDeviceSurfaceFormatsKHR(
			vkphysicalDevice_selected,
			vksurfaceKHR,
			&formatCount,
			vksurfaceFormatKHR_array);

	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "\ngetPhysicalDeviceSurfaceFormatAndColorSpace() -> 2nd call : vkGetPhysicalDeviceSurfaceFormatsKHR() failed : %d.\n", vkresult);
		return (vkresult);
	}
	else
	{
		fprintf(gpFile, "\ngetPhysicalDeviceSurfaceFormatAndColorSpace() -> 2nd call : vkGetPhysicalDeviceSurfaceFormatsKHR() Succeeded: %d.\n", vkresult);
	}

	// decide the surfce clolor format first
	if (formatCount == 1 && vksurfaceFormatKHR_array[0].format == VK_FORMAT_UNDEFINED)
	{
		vkformat_color=VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		vkformat_color=vksurfaceFormatKHR_array[0].format;
	}

	// decide color space
	vkcolorSpaceKHR=vksurfaceFormatKHR_array[0].colorSpace;

	if(vksurfaceFormatKHR_array)
	{
		free(vksurfaceFormatKHR_array);
		vksurfaceFormatKHR_array=NULL;
		fprintf(gpFile, "\ngetPhysicalDeviceSurfaceFormatAndColorSpace() -> vksurfaceFormatKHR_array Freed.");
	}

	return (vkresult);
}
