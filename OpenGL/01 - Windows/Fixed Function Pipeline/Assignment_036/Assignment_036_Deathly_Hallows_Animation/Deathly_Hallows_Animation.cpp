//Header files
#include "MyIcon.h"
#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/GLU.h> // GLU : GRAPHIC LIBRARY UTILITY
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
		TEXT("FFP: Assignment_036_Deathly_Hallows_Animation : By Kshitij Badkas"),
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
	//animation
	void Animate_Triangle();
	void Animate_Perpendicular();
	void Animate_Incircle(GLfloat, GLfloat, GLfloat);

	//variables
	GLfloat distance_A = 0.0f;
	GLfloat distance_B = 0.0f;
	GLfloat distance_C = 0.0f;
	GLfloat perimeter = 0.0f;
	GLfloat semi = 0.0f;
	GLfloat tArea = 0.0f;
	
	GLfloat iX = 0.0f;
	GLfloat iY = 0.0f;
	GLfloat radius = 0.0f;
	
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*	DEATHLY HALLOWS	 */

	//	TRIANGLE
	Animate_Triangle();
	
		
	//	PERPENDICULAR
	Animate_Perpendicular();
			
	//	DISTANCE FORMULA
	distance_A = (GLfloat)sqrt(pow((-1.0f - 1.0f), 2) + pow((-1.0f - -1.0f), 2));
	distance_B = (GLfloat)sqrt(pow((0.0f - -1.0f), 2) + pow((1.0f - -1.0f), 2));
	distance_C = (GLfloat)sqrt(pow((1.0f - 0.0f), 2) + pow((-1.0f - 1.0f), 2));

	fprintf(gpFile, "a- %f\n", distance_A);
	fprintf(gpFile, "b- %f\n", distance_B);
	fprintf(gpFile, "c- %f\n", distance_C);

	//	SEMIPERIMETER
	perimeter = (GLfloat)(distance_A + distance_B + distance_C);
	fprintf(gpFile, "perimeter- %f\n", perimeter);
	semi = (GLfloat)(perimeter / 2);
	fprintf(gpFile, "semiperimeter- %f\n", semi);

	//	AREA OF TRIANGLE
	tArea = (GLfloat)sqrt(semi * (semi - distance_A) * (semi - distance_B) * (semi - distance_C));
	fprintf(gpFile, "Triangle Area- %f\n", tArea);

	//	INCIRCLE COORDINATES
	iX = (GLfloat)(distance_A * (0.0f) + distance_B * (1.0f) + distance_C * (-1.0f)) / perimeter;
	iY = (GLfloat)(distance_A * (1.0f) + distance_B * (-1.0f) + distance_C * (-1.0f)) / perimeter;
	fprintf(gpFile, "iX- %f\n", iX);
	fprintf(gpFile, "iY- %f\n", iY);

	//	RADIUS
	radius = (GLfloat)(tArea / semi);
	fprintf(gpFile, "radius- %f\n", radius);


	/*	INCIRCLE  */
	Animate_Incircle(radius, iX, iY);
	
	//Double Buffer
	SwapBuffers(ghdc);

}

void Animate_Triangle()
{
	static GLfloat rotate = 0.0f;
	static GLfloat tx = -2.0f;
	//static GLfloat ty = 0.0f;

	glTranslatef(tx, 0.0f, -3.0f);
	fprintf(gpFile, "in glEnd() - tx= %f\n", tx);
	glRotatef(rotate, 0.0f, 1.0f, 0.0f);
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	//	TRIANGLE
	glColor3f(1.0f, 1.0f, 0.0f);

	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();
	
	fprintf(gpFile, "out of glEnd() - tx= %f\n", tx);
	if (tx <= 0.0f)
	{
		tx = tx + 0.005f;
		//ty = ty + 0.005f;
		fprintf(gpFile, "inside if - tx= %f\n", tx);
	}

	rotate = rotate + 1.0f;
	if (rotate >= 360.f)
	{
		rotate = 0.0f;
	}
}


void Animate_Incircle(GLfloat radius, GLfloat iX, GLfloat iY)
{
	GLfloat cAngle = 0.0f;
	static GLfloat rotate = 0.0f;
	static GLfloat tcx = 2.0f;
	static GLfloat tcy = iY;

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glTranslatef(tcx, tcy, 0.0f); //	2.0f, -0.38f, 0.0f
	glRotatef(rotate, 0.0f, 1.0f, 0.0f);
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 0.0f);
	for (cAngle = 0.0f; cAngle <= (2 * 3.14f); cAngle = cAngle + 0.01f)
	{
		glVertex3f(cos(cAngle) * radius, (sin(cAngle) * radius), 0.0f);
	}
	glEnd();
	
	if (tcx >= iX)
	{
		tcx = tcx - 0.005f;
		//ty = ty + 0.005f;
		fprintf(gpFile, "tcx- %f\n", tcx);
	}

	rotate = rotate + 1.0f;
	if (rotate >= 360.f)
	{
		rotate = 0.0f;
	}
}

void Animate_Perpendicular()
{
	static GLfloat py1_Point = 3.0f;
	static GLfloat py2_Point = 1.0f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, py1_Point, 0.0f);
	glVertex3f(0.0f, py2_Point, 0.0f);
	glEnd();

	if (py1_Point >= 1.0f)
	{
		py1_Point = py1_Point - 0.005f;
		py2_Point = py2_Point - 0.005f;
	}
}

/*void Animate_Perpendicular()	//DONE USING TRANSLATION
{
	static GLfloat tpy = 3.0f;
	//static GLfloat tpy2 = 1.0f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, tpy, -3.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glEnd();

	if (tpy >= 1.0f)
	{
		tpy = tpy - 0.005f;
	}
}*/

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
