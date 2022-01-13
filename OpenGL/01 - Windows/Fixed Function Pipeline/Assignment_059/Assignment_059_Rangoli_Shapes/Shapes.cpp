//Header files
#include "Headers.h"
#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/GLU.h>
#include <math.h>

//PRAGMA
#pragma comment(lib, "OpenGL32.LIB")
#pragma comment(lib, "GLU32.LIB")

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

// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{

	//PROTOTYPES
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
		TEXT("FFP: Assignment_059_Rangoli_Shapes : By Kshitij Badkas"),
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

	///////////////////////////////////////////////////
	///				GAME LOOP START				    ///
	///////////////////////////////////////////////////
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
	//Local function declaration
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
void Initialize(void)
{
	//function declaration
	void Resize(int, int);

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

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

	//SetglColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Resize(WIN_WIDTH, WIN_HEIGHT);
	//WARM-UP CALL TO Resize();
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
	//Prototypes
	void Draw_Points(void);
	void Draw_Triangles(void);
	void Draw_Squares(void);
	void Draw_Tri_Squares(void);
	void Draw_Lines(void);
	void Draw_Colours(void);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	Draw_Points();
	Draw_Triangles();
	Draw_Squares();
	Draw_Tri_Squares();
	Draw_Lines();
	Draw_Colours();


	//Double Buffer
	SwapBuffers(ghdc);

}

void Draw_Points(void)
{
	//code
	GLfloat vertex_inc = 0.25f;
	GLfloat x_coord = 0.0f;
	GLfloat y_coord = 1.0f;
	
	glLoadIdentity();
	glTranslatef(-2.0f, 0.0f, -3.0f);
	glPointSize(3.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 4; i++) // 4 ROWS
	{
		for (x_coord = vertex_inc; x_coord <= 1.0f; x_coord = x_coord + vertex_inc)
		{
			glVertex2f(x_coord, y_coord);
		}
		y_coord -= 0.25f;
	}
	glEnd();
	
}

void Draw_Triangles(void)
{
	
	//code
	//OUTER SQUARE
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.25f);
	glVertex2f(0.0f, 1.0f);

	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.75f, 1.0f);
	glEnd();

	//2 PARALLEL LINES VERTICAL
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.25f, 0.25f);
	glVertex2f(0.25f, 1.0f);
	glEnd();

	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.5f, 0.25f);
	glVertex2f(0.5f, 1.0f);
	glEnd();

	//2 PARALLEL LINES HORIZONTAL
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.75f);
	glVertex2f(0.75f, 0.75f);
	glEnd();

	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.75f, 0.5f);
	glEnd();

	//SLANTING LINES - LEFT TO RIGHT
	//LINE -1
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.75f);
	glVertex2f(0.25f, 1.0f);
	glEnd();

	//LINE -2
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.5f, 1.0f);
	glEnd();

	//LINE -3
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.25f);
	glVertex2f(0.75f, 1.0f);
	glEnd();

	//LINE -4
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.25f, 0.25f);
	glVertex2f(0.75f, 0.75f);
	glEnd();
	
	//LINE -5
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.5f, 0.25f);
	glVertex2f(0.75f, 0.5f);
	glEnd();
	
}

void Draw_Squares(void)
{
	glLoadIdentity();

	//OUTER SQUARE
	glLineWidth(2.0f);
	glTranslatef(1.0f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.75f, 1.0f);
	glVertex2f(0.75f, 0.25f);
	
	glVertex2f(0.75f, 0.25f);
	glVertex2f(0.0f, 0.25f);
	
	glVertex2f(0.0f, 0.25f);
	glVertex2f(0.0f, 1.0f);

	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.75f, 1.0f);
	glEnd();

	//2 PARALLEL LINES VERTICAL
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(1.0f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.25f, 0.25f);
	glVertex2f(0.25f, 1.0f);
	glEnd();

	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(1.0f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.5f, 0.25f);
	glVertex2f(0.5f, 1.0f);
	glEnd();

	//2 PARALLEL LINES HORIZONTAL
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(1.0f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.75f);
	glVertex2f(0.75f, 0.75f);
	glEnd();

	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(1.0f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.75f, 0.5f);
	glEnd();

}

void Draw_Tri_Squares(void)
{

	//code
	//OUTER SQUARE
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-1.75f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.75f, 1.0f);
	glVertex2f(0.75f, 0.25f);

	glVertex2f(0.75f, 0.25f);
	glVertex2f(0.0f, 0.25f);

	glVertex2f(0.0f, 0.25f);
	glVertex2f(0.0f, 1.0f);

	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.75f, 1.0f);
	glEnd();

	//2 PARALLEL LINES VERTICAL
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-1.75f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.25f, 0.25f);
	glVertex2f(0.25f, 1.0f);
	glEnd();

	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-1.75f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.5f, 0.25f);
	glVertex2f(0.5f, 1.0f);
	glEnd();

	//2 PARALLEL LINES HORIZONTAL
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-1.75f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.75f);
	glVertex2f(0.75f, 0.75f);
	glEnd();

	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-1.75f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.75f, 0.5f);
	glEnd();

	//SLANTING LINES - LEFT TO RIGHT
	//LINE -1
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-1.75f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.75f);
	glVertex2f(0.25f, 1.0f);
	glEnd();

	//LINE -2
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-1.75f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.5f, 1.0f);
	glEnd();

	//LINE -3
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-1.75f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.25f);
	glVertex2f(0.75f, 1.0f);
	glEnd();

	//LINE -4
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-1.75f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.25f, 0.25f);
	glVertex2f(0.75f, 0.75f);
	glEnd();

	//LINE -5
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-1.75f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.5f, 0.25f);
	glVertex2f(0.75f, 0.5f);
	glEnd();

}

void Draw_Lines(void)
{
	glLoadIdentity();

	//OUTER SQUARE
	glLineWidth(2.0f);
	glTranslatef(-0.35f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.75f, 1.0f);
	glVertex2f(0.75f, 0.25f);

	glVertex2f(0.75f, 0.25f);
	glVertex2f(0.0f, 0.25f);

	glVertex2f(0.0f, 0.25f);
	glVertex2f(0.0f, 1.0f);

	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.75f, 1.0f);
	glEnd();

	//LINES
	//LINE -1
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.75f, 0.75f);
	glEnd();

	//LINE -2
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.75f, 0.5f);
	glEnd();

	//LINE -3
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.75f, 0.25f);
	glEnd();

	//LINE -4
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.5f, 0.25f);
	glEnd();

	//LINE -5
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(-0.35f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.25f, 0.25f);
	glEnd();


}

void Draw_Colours(void)
{
	//COLOURS
	//RED
	glLoadIdentity();
	glTranslatef(1.0f, -1.25f, -3.0f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(0.25f, 1.0f);
	glVertex2f(0.25f, 0.25f);
	glVertex2f(0.0f, 0.25f);
	glVertex2f(0.0f, 1.0f);
	glEnd();

	//GREEN
	glLoadIdentity();
	glTranslatef(1.0f, -1.25f, -3.0f);
	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.5f, 1.0f);
	glVertex2f(0.5f, 0.25f);
	glVertex2f(0.25f, 0.25f);
	glVertex2f(0.25f, 1.0f);
	glEnd();

	//BLUE
	glLoadIdentity();
	glTranslatef(1.0f, -1.25f, -3.0f);
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(0.75f, 1.0f);
	glVertex2f(0.75f, 0.25f);
	glVertex2f(0.5f, 0.25f);
	glVertex2f(0.5f, 1.0f);
	glEnd();

	//2 PARALLEL LINES VERTICAL
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(1.0f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.25f, 0.25f);
	glVertex2f(0.25f, 1.0f);
	glEnd();

	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(1.0f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.5f, 0.25f);
	glVertex2f(0.5f, 1.0f);
	glEnd();

	//2 PARALLEL LINES HORIZONTAL
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(1.0f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.75f);
	glVertex2f(0.75f, 0.75f);
	glEnd();

	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(1.0f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.75f, 0.5f);
	glEnd();

	//OUTER SQUARE
	glLoadIdentity();
	glLineWidth(2.0f);
	glTranslatef(1.0f, -1.25f, -3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.75f, 1.0f);
	glVertex2f(0.75f, 0.25f);

	glVertex2f(0.75f, 0.25f);
	glVertex2f(0.0f, 0.25f);

	glVertex2f(0.0f, 0.25f);
	glVertex2f(0.0f, 1.0f);

	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.75f, 1.0f);
	glEnd();


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

	//File Close Code Here
	if (gpFile)
	{
		fclose(gpFile);
		gpFile = NULL;
	}
}
