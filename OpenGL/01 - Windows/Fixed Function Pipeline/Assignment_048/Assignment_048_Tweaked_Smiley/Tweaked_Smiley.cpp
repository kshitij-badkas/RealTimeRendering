//Header files
#include "Headers.h"
#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/GLU.h>

//PRAGMA
#pragma comment(lib, "OpenGL32.LIB")
#pragma comment(lib, "GLU32.LIB")
#pragma comment(lib, "USER32.LIB")
#pragma comment(lib, "GDI32.LIB")

//MACROS
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global function prototype
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variables
DWORD dwStyle;
WINDOWPLACEMENT wpPrev;
bool gbFullscreen = false;
HWND ghwnd = NULL;
FILE* gpFile = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;

GLuint smiley_texture; //texture
unsigned int pressed_key;

// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{

	//Function Prototypes
	void Initialize(void);
	void Display(void);

	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");
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
	iFileOpen = fopen_s(&gpFile, "KAB_Log.txt", "w");
	if (iFileOpen != 0)
	{
		//MessageBox 
		MessageBox(NULL, TEXT("Cannot Create Desired File."), TEXT("Error"), MB_OK);
		exit(0);
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
		TEXT("FFP: Assignment_048_Tweaked_Smiley : By Kshitij Badkas"),
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
	Initialize();

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	///////////////////////////////////////////
	///				GAME LOOP START			///
	///////////////////////////////////////////
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
				//HERE YOU SHOULD CALL Update Function FOR OPENGL RENDERING

				//HERE YOU SHOULD CALL Display Function FOR OPENGL RENDERING
				Display();

			}
		}
	}
	///////////////////////////////////////////////////
	///				GAME LOOP END				    ///
	///////////////////////////////////////////////////

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Function protorypes
	void ToggleFullscreen(void);
	void Resize(int, int);
	void Uninitialize(void);

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
		Resize(LOWORD(lParam), HIWORD(lParam));
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

		case 49: //'1 is pressed
			pressed_key = 1;
			fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
			glEnable(GL_TEXTURE_2D);
			break;

		case 50: //'2' is pressed
			pressed_key = 2;
			fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
			glEnable(GL_TEXTURE_2D);
			break;

		case 51:
			//'3' is pressed
			pressed_key = 3;
			fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
			glEnable(GL_TEXTURE_2D);
			break;

		case 52:
			//'4' is pressed
			pressed_key = 4;
			fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
			glEnable(GL_TEXTURE_2D);
			break;

		default:
			pressed_key = wParam;
			fprintf(gpFile, "\n Pressed Digit = %d\n", wParam);
			glDisable(GL_TEXTURE_2D);
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
void Initialize(void)
{
	//function declaration
	void Resize(int, int);
	bool LoadGLTexture(GLuint*, TCHAR[]);

	//local variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ghdc = GetDC(ghwnd);
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat() Failed.\n");
		DestroyWindow(ghwnd);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() Failed.\n");
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() Failed.\n");
		DestroyWindow(ghwnd);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() Failed.\n");
		DestroyWindow(ghwnd);
	}

	//Loading Textures
	LoadGLTexture(&smiley_texture, MAKEINTRESOURCE(SMILEY_BITMAP));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Resize(WIN_WIDTH, WIN_HEIGHT);
	//WARM-UP CALL TO Resize();
}

//Function LoadGLTexture
bool LoadGLTexture(GLuint* texture, TCHAR resourceID[])
{
	//variable declaration
	bool bResult = false;
	HBITMAP hBitmap = NULL; //image loading
	BITMAP bmp; //image loading

	//code
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), resourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		bResult = true;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);

		//OPENGL TEXTURE CODE STARTS HERE
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		//Setting texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//Push the Data into Graphics Memory with the Help of Graphics Driver
		//gluBuild2DMipmaps = glTexImage2D + glGenerateMipmap
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

		// image loading
		DeleteObject(hBitmap);
	}

	return bResult;
}

//Function Resize
void Resize(int width, int height)
{
	//code
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

//Function Display
void Display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	if (pressed_key == 1)
	{
		//FULL SMILEY
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -3.0f);

		glBegin(GL_QUADS);
		glTexCoord2d(1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2d(0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2d(1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	}
	else if (pressed_key == 2)
	{
		//LEFT BOTTOM SECTOR - WRAPPING AND CLAMPLING
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -3.0f);

		glBegin(GL_QUADS);
		glTexCoord2d(0.5f, 0.5f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2d(0.0f, 0.5f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2d(0.5f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	}
	else if (pressed_key == 3)
	{
		//TILED SMILEYS - TILING OR REPEAT
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -3.0f);

		glBegin(GL_QUADS);
		glTexCoord2d(2.0f, 2.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2d(0.0f, 2.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2d(2.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	}
	else if (pressed_key == 4)
	{
		//ONE PIXEL OF IMAGE
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -3.0f);

		glBegin(GL_QUADS);
		glTexCoord2d(0.5f, 0.5f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2d(0.5f, 0.5f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2d(0.5f, 0.5f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2d(0.5f, 0.5f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	}
	else
	{
		//WHITE RECTANGLE
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -3.0f);

		glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();
	}
	

	//Double Buffer
	SwapBuffers(ghdc);

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

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glDeleteTextures(1, &smiley_texture);

	//File Close Code Here
	if (gpFile)
	{
		fclose(gpFile);
		gpFile = NULL;
	}
}
