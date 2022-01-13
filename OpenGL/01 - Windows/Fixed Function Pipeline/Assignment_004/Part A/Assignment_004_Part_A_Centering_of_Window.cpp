#include <windows.h>

// function prototype
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");

	// Screen Resolution - Width and Height
	int iScreenWidth;
	int iScreenHeight;

	// Screen Width's centre and Screen Height's centre
	int iScreenWidthCentre;
	int iScreenHeightCentre;

	// New Window Resolution - Width and Height
	int iWindowWidth = 800;
	int iWindowHeight = 600;

	//New Window Position - For Centering of New Window
	int iCreateWindowPositionX;
	int iCreateWindowPositionY;


	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	// Screen Resolution - Width and Height
	iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Screen Resolution - Width's centre and Screen Height's centre
	iScreenWidthCentre = (iScreenWidth / 2);
	iScreenHeightCentre = (iScreenHeight / 2);

	//New Window Position - For Centering of New Window
	iCreateWindowPositionX = iScreenWidthCentre - (iWindowWidth / 2);
	iCreateWindowPositionY = iScreenHeightCentre - (iWindowHeight / 2);

	hwnd = CreateWindow(szAppName,
		TEXT("FFP: Assignment_004_Part_A_Centering_of_Window : By Kshitij Badkas"),
		WS_OVERLAPPEDWINDOW,
		iCreateWindowPositionX,
		iCreateWindowPositionY,
		iWindowWidth,
		iWindowHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	/*
		szAppName,
		Caption Text,
		Window Style,
		X,
		Y,
		Width,
		Height,
		is Child Window?,
		Menu,
		hInstance.
		Creation Parameter
	*/

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,
	UINT iMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}