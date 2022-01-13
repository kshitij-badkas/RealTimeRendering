#include <windows.h>
#include <stdio.h>

//Global function prototype
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable
FILE *gpFile = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");

	//Code for Debugging
	//Open file for writing
	int iFileOpen;
	iFileOpen = fopen_s(&gpFile, "KAB_Log.txt", "w");
	if (iFileOpen != 0)
	{
		//MessageBox 
		MessageBox(NULL, TEXT("Cannot Create Desired File."), TEXT("Error"), MB_OK);
		exit(0);
	}

	else if (iFileOpen == 0) //success
	{
		MessageBox(NULL, TEXT("File Created Successfully."), TEXT("Success"), MB_OK);
	}


	//initialization of WNDCLASSEX
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

	//register the above class 
	RegisterClassEx(&wndclass);

	//create window
	hwnd = CreateWindow(szAppName,
		TEXT("FFP: Assignment_005_Part_B_FileIO_GUI : By Kshitij Badkas"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	//message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//Close File 
	fclose(gpFile);

	//Clean File Pointer
	gpFile = NULL;

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_CREATE:
		fprintf(gpFile, "Bharat Is My Country.\n");
		MessageBox(hwnd, TEXT("WM_CREATE is received."), TEXT("My Message"), MB_OK);
		break;

	case WM_DESTROY:
		fprintf(gpFile, "Jai Hind.\n");
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
