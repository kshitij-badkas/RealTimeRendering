#include <windows.h>
#include <stdio.h>

#include "VK.h"

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
const char* gpszAppName = "ARTR";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{

	// Function Prototype
	int Initialize(void);
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

	iResult = Initialize();

	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("initialize() Failed!"), TEXT("Error"), MB_OK | MB_ICONERROR);
		DestroyWindow(hwnd);
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

int Initialize(void)
{
	// code
	return (0);
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