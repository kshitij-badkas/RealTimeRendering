//Header files
#include "MyIcon.h"
#include "Music.h"
#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/GLU.h> // GLU : GRAPHIC LIBRARY UTILITY

//PRAGMA
#pragma comment(lib, "OpenGL32.LIB")
#pragma comment(lib, "GLU32.LIB")
#pragma comment(lib, "Winmm.lib")	//	For Adding Soundtrack/Music

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

// Planes Speed
GLfloat speed = 0.004f;

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
		TEXT("FFP: Assignment_038_Dynamic_India : By Kshitij Badkas"),
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

	//	PLAY AUDIO 
	PlaySound(MAKEINTRESOURCE(MUSIC), hInstance, SND_RESOURCE | SND_ASYNC);

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
	//variables

	//prototypes
	void Draw_I(void);
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	Draw_I();

	//Double Buffer
	SwapBuffers(ghdc);

}

void Draw_I()
{
	void Draw_A(void);
	//	I	
	static GLfloat tx_I = -2.2f;
	static GLfloat point_A = 0.0f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_I, 0.0f, -3.0f);
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(0.08f, 0.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(-0.08f, 0.4f, 0.0f);

	glColor3f(0.0f, 0.5f, 0.0f);	//GREEN
	glVertex3f(-0.08f, -0.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);	//GREEN
	glVertex3f(0.08f, -0.4f, 0.0f);

	glEnd();

	if (tx_I <= -1.4f)
	{
		tx_I = tx_I + 0.0035f;
	}

	//	DRAW 'A' CALL 
	if (tx_I > -1.4f)
	{
		point_A = point_A + 0.025f;
		if (point_A >= 1.5f)
			Draw_A();
	}

}

void Draw_A()
{
	void Draw_N(void);

	static GLfloat tx_A_Left = 2.3f;
	static GLfloat tx_A_Right = 2.7f;
	static GLfloat tx_A_Top = 2.457f;
	static GLfloat tx_A_Centre = 2.5f;

	static GLfloat point_N = 0.0f;
	//	A - LEFT	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(1.0f, 0.0f, -3.0f);
	glTranslatef(tx_A_Left, 0.0f, -3.0f);
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(0.04f, 0.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(-0.04f, 0.4f, 0.0f);

	glColor3f(0.0f, 0.5f, 0.0f);	//GREEN
	glVertex3f(-0.04f, -0.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);	//GREEN
	glVertex3f(0.04f, -0.4f, 0.0f);

	glEnd();

	//	A - TOP	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(1.2f, 0.357f, -3.0f);
	glTranslatef(tx_A_Top, 0.357f, -3.0f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	//glVertex3f(0.075f, 0.03f, 0.0f);
	//glVertex3f(-0.075f, 0.03f, 0.0f);
	glVertex3f(0.166f, 0.03f, 0.0f);
	glVertex3f(-0.166f, 0.03f, 0.0f);

	glEnd();

	if (tx_A_Top >= 1.2f)
	{
		tx_A_Top = tx_A_Top - 0.0055f;
	}

		
	//	A - RIGHT	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(1.4f, 0.0f, -3.0f);
	glTranslatef(tx_A_Right, 0.0f, -3.0f);
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(0.08f, 0.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(-0.08f, 0.4f, 0.0f);

	glColor3f(0.0f, 0.5f, 0.0f);	//GREEN
	glVertex3f(-0.08f, -0.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);	//GREEN
	glVertex3f(0.08f, -0.4f, 0.0f);

	glEnd();

	if (tx_A_Left >= 1.0f)
	{
		tx_A_Left = tx_A_Left - 0.0055f;
		tx_A_Right = tx_A_Right - 0.0055f;
	}


	//	 DRAW 'N' CALL
	if (tx_A_Left < 1.0f)
	{
		//fprintf(gpFile, "\n tx_A_Left ======== %f ", tx_A_Left);
		point_N = point_N + 0.025f;
		if (point_N >= 1.5f)
			Draw_N();
	}

}

void Draw_N()
{
	void Draw_I2(void);

	static GLfloat ty_N_left = 1.8f;
	static GLfloat ty_N_right = 1.8f;
	static GLfloat ty_N_top = 2.157f;

	//static GLfloat point_N = -3.2f;
	static GLfloat point_I2 = 0.0f;
	//fprintf(gpFile, "INSIDE IF -> point_N : %f\n", point_N);
	//	N - LEFT
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(-1.0f, 0.0f, -3.0f);
	glTranslatef(-0.95f, ty_N_left, -3.0f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(0.08f, 0.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(-0.08f, 0.4f, 0.0f);

	glColor3f(0.0f, 0.5f, 0.0f);	//GREEN
	glVertex3f(-0.08f, -0.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);	//GREEN
	glVertex3f(0.08f, -0.4f, 0.0f);
	glEnd();
	
	//	N - TOP
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.75f, ty_N_top, -3.0f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(0.166f, 0.03f, 0.0f);
	glVertex3f(-0.166f, 0.03f, 0.0f);
	glEnd();

	if (ty_N_top >= 0.357)
	{
		ty_N_top = ty_N_top - 0.0035f;
	}

	//	N - RIGHT
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(-0.6f, 0.0f, -3.0f);
	glTranslatef(-0.55f, ty_N_right, -3.0f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(0.04f, 0.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(-0.04f, 0.4f, 0.0f);

	glColor3f(0.0f, 0.5f, 0.0f);	//GREEN
	glVertex3f(-0.04f, -0.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);	//GREEN
	glVertex3f(0.04f, -0.4f, 0.0f);
	glEnd();

	if (ty_N_left >= 0.0f)
	{
		ty_N_left = ty_N_left - 0.0035f;
		ty_N_right = ty_N_right - 0.0035f;
	}

	if (ty_N_left < 0.0f)
	{
		//fprintf(gpFile, "\n Value of ty_N_Left = = = %f \n", ty_N_left);
		point_I2 = point_I2 + 0.03f;
		if (point_I2 >= 1.5f)
			Draw_I2();
	}

}


void Draw_I2()
{
	void Draw_D(void);

	static GLfloat ty_I2 = -2.0f;
	static GLfloat point_D = 0.0f;

	//	I	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.6f, 0.0f, -3.0f);
	glTranslatef(0.6f, ty_I2, -3.0f);
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(0.08f, 0.4f, 0.0f);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(-0.08f, 0.4f, 0.0f);

	glColor3f(0.0f, 0.5f, 0.0f);	//GREEN
	glVertex3f(-0.08f, -0.4f, 0.0f);

	glColor3f(0.07f, 0.53f, 0.03f);	//GREEN
	glVertex3f(0.08f, -0.4f, 0.0f);

	glEnd();

	if (ty_I2 <= 0.0f)
	{
		ty_I2 = ty_I2 + 0.0035f;
	}

	if (ty_I2 > 0.0f)
	{
		point_D = point_D + 0.03f;
		if (point_D >= 1.5f)
			Draw_D();
	}

}



void Draw_D()
{
	void Animate_Leader(void);
	void Animate_TopMember(void);
	void Animate_BottomMember(void);

	static GLfloat cx1_D_left = 0.0f, cx2_D_left = 0.0f, cx3_D_left = 0.0f, cx4_D_left = 0.0f;
	static GLfloat cy1_D_left = 0.0f, cy2_D_left = 0.0f, cy3_D_left = 0.0f, cy4_D_left = 0.0f;
	static GLfloat cz1_D_left = 0.0f, cz2_D_left = 0.0f, cz3_D_left = 0.0f, cz4_D_left = 0.0f;

	static GLfloat cx1_D_right = 0.0f, cx2_D_right = 0.0f, cx3_D_right = 0.0f, cx4_D_right = 0.0f;
	static GLfloat cy1_D_right = 0.0f, cy2_D_right = 0.0f, cy3_D_right = 0.0f, cy4_D_right = 0.0f;
	static GLfloat cz1_D_right = 0.0f, cz2_D_right = 0.0f, cz3_D_right = 0.0f, cz4_D_right = 0.0f;

	static GLfloat cx1_D_top = 0.0f, cy1_D_top = 0.0f, cz1_D_top = 0.0f;

	static GLfloat cx1_D_bottom = 0.0f, cy1_D_bottom = 0.0f, cz1_D_bottom = 0.0f;

	static GLfloat point_Planes = 0.0f;

	//	D - LEFT
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.2f, 0.0f, -3.0f);
	glBegin(GL_QUADS);
	
	glColor3f(cx1_D_left, cy1_D_left, cz1_D_left);	//BHAGWA
	glVertex3f(0.04f, 0.4f, 0.0f);

	glColor3f(cx2_D_left, cy2_D_left, cz2_D_left);	//BHAGWA
	glVertex3f(-0.04f, 0.4f, 0.0f);

	glColor3f(0.0f, cy3_D_left, 0.2f);	//GREEN
	glVertex3f(-0.04f, -0.4f, 0.0f);

	glColor3f(cx4_D_left, cy4_D_left, cz4_D_left);	//GREEN
	glVertex3f(0.04f, -0.4f, 0.0f);
	glEnd();

	if (cx1_D_left <= 1.0f)
		cx1_D_left = cx1_D_left + 0.003f;
	if (cy1_D_left <= 0.6f)
		cy1_D_left = cy1_D_left + 0.003f;
	if (cz1_D_left <= 0.2f)
		cz1_D_left = cz1_D_left + 0.003f;

	if (cx2_D_left <= 1.0f)
		cx2_D_left = cx2_D_left + 0.003f;
	if (cy2_D_left <= 0.6f)
		cy2_D_left = cy2_D_left + 0.003f;
	if (cz2_D_left <= 0.2f)
		cz2_D_left = cz2_D_left + 0.003f;

	if (cy3_D_left <= 0.5f)
		cy3_D_left = cy3_D_left + 0.003f;

	if (cx4_D_left <= 0.07f)
		cx4_D_left = cx4_D_left + 0.003f;
	if (cy4_D_left <= 0.53f)
		cy4_D_left = cy4_D_left + 0.003f;
	if (cz4_D_left <= 0.03f)
		cz4_D_left = cz4_D_left + 0.003f;

	//	D - TOP	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.357f, -3.0f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);

	//glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glColor3f(cx1_D_top, cy1_D_top, cz1_D_top);
	glVertex3f(0.166f, 0.03f, 0.0f);
	glVertex3f(-0.166f, 0.03f, 0.0f);

	glEnd();

	if (cx1_D_top <= 1.0f)
		cx1_D_top = cx1_D_top + 0.003f;
	if (cy1_D_top <= 0.6f)
		cy1_D_top = cy1_D_top + 0.003f;
	if (cz1_D_top <= 0.2f)
		cz1_D_top = cz1_D_top + 0.003f;

	//	D - BOTTOM	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, -0.42f, -3.0f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);

	glColor3f(cx1_D_bottom, cy1_D_bottom, cz1_D_bottom);
	glVertex3f(0.166f, 0.03f, 0.0f);
	glVertex3f(-0.166f, 0.03f, 0.0f);

	glEnd();

	if (cx1_D_bottom <= 0.07f)
		cx1_D_bottom = cx1_D_bottom + 0.003f;
	if (cy1_D_bottom <= 0.53f)
		cy1_D_bottom = cy1_D_top + 0.003f;
	if (cz1_D_bottom <= 0.03f)
		cz1_D_bottom = cz1_D_bottom + 0.003f;

	//	D - RIGHT	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.2f, 0.0f, -3.0f);
	glBegin(GL_QUADS);

	glColor3f(cx1_D_right, cy1_D_right, cz1_D_right);
	glVertex3f(0.08f, 0.4f, 0.0f);

	glColor3f(cx2_D_right, cy2_D_right, cz2_D_right);
	glVertex3f(-0.08f, 0.4f, 0.0f);

	glColor3f(0.0f, cy3_D_right, 0.2f);
	glVertex3f(-0.08f, -0.4f, 0.0f);

	glColor3f(cx4_D_right, cy4_D_right, cz4_D_right);
	glVertex3f(0.08f, -0.4f, 0.0f);

	glEnd();

	if (cx1_D_right <= 1.0f)
		cx1_D_right = cx1_D_right + 0.003f;
	if (cy1_D_right <= 0.6f)
		cy1_D_right = cy1_D_right + 0.003f;
	if (cz1_D_right <= 0.2f)
		cz1_D_right = cz1_D_right + 0.003f;

	if (cx2_D_right <= 1.0f)
		cx2_D_right = cx2_D_right + 0.003f;
	if (cy2_D_right <= 0.6f)
		cy2_D_right = cy2_D_right + 0.003f;
	if (cz2_D_right <= 0.2f)
		cz2_D_right = cz2_D_right + 0.003f;

	if (cy3_D_right <= 0.5f)
		cy3_D_right = cy3_D_right + 0.003f;

	if (cx4_D_right <= 0.07f)
		cx4_D_right = cx4_D_right + 0.003f;
	if (cy4_D_right <= 0.53f)
		cy4_D_right = cy4_D_right + 0.003f;
	if (cz4_D_right <= 0.03f)
		cz4_D_right = cz4_D_right + 0.003f;

	if (cx1_D_top > 1.0f)
	{
		point_Planes = point_Planes + 0.01f;
		if (point_Planes >= 1.5f)
		{
			Animate_Leader();
			Animate_TopMember();
			Animate_BottomMember();
		}

	}


}

void Animate_Leader()
{
	void Draw_A_Centre(void);

	static GLfloat tx_leader = -2.5f;
	static GLfloat tx_front_rad = -2.315f;
	static GLfloat tx_vertical = -2.78f;

	static GLfloat tx_I = -2.7f;
	static GLfloat tx_A = -2.65f;
	static GLfloat tx_F = -2.55f;

	static GLfloat tx_leader_Flag = -3.095f;

	//	LEADER POSITION
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_leader, 0.0f, -3.0f);
	glScalef(0.2f, 0.2f, 0.2f);
	glBegin(GL_POLYGON);

	//	FINAL DESIGN
	glColor3f(0.5f, 0.7f, 0.7f);	

	glVertex3f(0.0f, 0.15f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.2f, 1.0f, 0.0f);

	glVertex3f(-1.3f, 0.2f, 0.0f);
	glVertex3f(-1.4f, 0.15f, 0.0f);
	glVertex3f(-1.4f, -0.15f, 0.0f);
	glVertex3f(-1.3f, -0.2f, 0.0f);

	glVertex3f(-1.2f, -1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, -0.15f, 0.0f);
	glVertex3f(0.4f, -0.15f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.4f, 0.15f, 0.0f);

	glEnd();

	if (tx_leader <= 3.0f)
	{
		tx_leader = tx_leader + speed;
	}

	//	LEADER FRONT RADAR
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_front_rad, 0.0f, -3.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex3f(0.05f, 0.0f, 0.0f);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glEnd();

	if (tx_front_rad <= 3.815)
	{
		tx_front_rad = tx_front_rad + speed;
	}

	//	LEADER VERTICAL STABILIZER
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_vertical, 0.0f, -3.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//BLACK
	glVertex3f(0.05f, 0.0f, 0.0f);
	glColor3f(0.5f, 0.7f, 0.7f);	//
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glEnd();

	if (tx_vertical <= 3.28)
	{
		tx_vertical = tx_vertical + speed;
	}

	//	DRAW WHTIE SMOKE
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_leader_Flag, 0.0f, -3.0f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);

	glColor3f(0.9f, 0.9f, 0.9f);	//WHITE
	glVertex3f(0.3f, 0.00f, 0.0f);
	glColor3f(0.1f, 0.1f, 0.1f);
	glVertex3f(-0.3f, 0.00f, 0.0f);

	glEnd();

	if (tx_leader_Flag <= 3.28f)
	{
		tx_leader_Flag = tx_leader_Flag + speed;
	}

	//	DRAW IAF
	//	I
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_I, 0.0f, -3.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//	BLACK
	glVertex3f(0.0f, 0.05f, 0.0f);
	glVertex3f(0.0f, -0.05f, 0.0f);
	glEnd();

	if (tx_I <= 3.28)
	{
		tx_I = tx_I + speed;
	}

	//	A
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_A, 0.0f, -3.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//	BLACK
	glVertex3f(0.03f, -0.05f, 0.0f);
	glVertex3f(0.0f, 0.05f, 0.0f);
	glVertex3f(0.0f, 0.05f, 0.0f);
	glVertex3f(-0.03f, -0.05f, 0.0f);
	glVertex3f(-0.01f, 0.0f, 0.0f);
	glVertex3f(0.01f, 0.0f, 0.0f);
	glEnd();

	if (tx_A <= 3.28)
	{
		tx_A = tx_A + speed;
	}

	//	F
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_F, 0.0f, -3.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//	BLACK
	glVertex3f(-0.05f, 0.05f, 0.0f);
	glVertex3f(-0.05f, -0.05f, 0.0f);
	glVertex3f(-0.020f, 0.045f, 0.0f);
	glVertex3f(-0.05f, 0.045f, 0.0f);
	glVertex3f(-0.025f, 0.0f, 0.0f);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glEnd();

	if (tx_F <= 3.28)
	{
		tx_F = tx_F + speed;
	}

	if (tx_leader >= 1.9f)
	{
		Draw_A_Centre();
	}
}

void Animate_TopMember()
{
	//	TOP MEMBER
	static GLfloat tx_top_mem = -2.5f;
	static GLfloat tx_top_mem_front_rad = -2.3f;
	static GLfloat tx_top_mem_vertical = -2.78f;

	static GLfloat ty_top_mem = 2.4f;
	static GLfloat ty_top_mem_front_rad = 2.4f;
	static GLfloat ty_top_mem_vertical = 2.4f;

	static GLfloat ty_top_mem_UP = 0.0f;
	static GLfloat ty_top_mem_front_rad_UP = 0.0f;
	static GLfloat ty_top_mem_vertical_UP = 0.0f;

	static GLfloat tx_top_mem_I = -2.7f;
	static GLfloat tx_top_mem_A = -2.65f;
	static GLfloat tx_top_mem_F = -2.55f;

	static GLfloat ty_top_mem_I = 2.4f;
	static GLfloat ty_top_mem_A = 2.4f;
	static GLfloat ty_top_mem_F = 2.4f;

	static GLfloat ty_top_mem_I_UP = 0.0f;
	static GLfloat ty_top_mem_A_UP = 0.0f;
	static GLfloat ty_top_mem_F_UP = 0.0f;

	static GLfloat tx_top_mem_Flag = -3.095f;
	static GLfloat ty_top_mem_Flag = 2.4f;

	static GLfloat tx_top_mem_Flag_UP = 0.0f;
	static GLfloat ty_top_mem_Flag_UP = 0.0f;

	//	TOP MEMBER POSITION
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_top_mem, ty_top_mem, -3.0f);
	glScalef(0.2f, 0.2f, 0.2f);
	glBegin(GL_POLYGON);

	//	TOP MEMBER FINAL DESIGN 
	glColor3f(0.5f, 0.7f, 0.7f);	

	glVertex3f(0.0f, 0.15f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.2f, 1.0f, 0.0f);

	glVertex3f(-1.3f, 0.2f, 0.0f);
	glVertex3f(-1.4f, 0.15f, 0.0f);
	glVertex3f(-1.4f, -0.15f, 0.0f);
	glVertex3f(-1.3f, -0.2f, 0.0f);

	glVertex3f(-1.2f, -1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, -0.15f, 0.0f);
	glVertex3f(0.4f, -0.15f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.4f, 0.15f, 0.0f);

	glEnd();

	if (tx_top_mem <= 3.0f)
	{
		tx_top_mem = tx_top_mem + speed;
	}

	if (ty_top_mem >= 0.0f)
	{
		ty_top_mem = ty_top_mem - speed;
	}

	if (tx_top_mem >= 1.9f)
	{
		ty_top_mem_UP = ty_top_mem_UP + speed;
		ty_top_mem = ty_top_mem_UP;

		ty_top_mem_front_rad_UP = ty_top_mem_front_rad_UP + speed;
		ty_top_mem_front_rad = ty_top_mem_front_rad_UP;

		ty_top_mem_vertical_UP = ty_top_mem_vertical_UP + speed;
		ty_top_mem_vertical = ty_top_mem_vertical_UP;

		ty_top_mem_Flag_UP = ty_top_mem_Flag_UP + speed;
		ty_top_mem_Flag = ty_top_mem_Flag_UP;
	}

	//	TOP MEMBER FRONT RADAR
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_top_mem_front_rad, ty_top_mem_front_rad, -3.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex3f(0.05f, 0.0f, 0.0f);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glEnd();

	if (tx_top_mem_front_rad <= 3.815f)
	{
		tx_top_mem_front_rad = tx_top_mem_front_rad + speed;
	}

	if (ty_top_mem_front_rad >= 0.0f)
	{
		ty_top_mem_front_rad = ty_top_mem_front_rad - speed;
	}

	//	TOP MEMBER VERTICAL STABILIZER
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_top_mem_vertical, ty_top_mem_vertical, -3.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//BLACK
	glVertex3f(0.05f, 0.0f, 0.0f);
	glColor3f(0.5f, 0.7f, 0.7f);	//
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glEnd();

	if (tx_top_mem_vertical <= 3.28f)
	{
		tx_top_mem_vertical = tx_top_mem_vertical + speed;
	}

	if (ty_top_mem_vertical >= 0.0f)
	{
		ty_top_mem_vertical = ty_top_mem_vertical - speed;
	}

	//	DRAW SAFFRON SMOKE
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_top_mem_Flag, ty_top_mem_Flag, -3.0f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.6f, 0.2f);	//BHAGWA
	glVertex3f(0.3f, 0.02f, 0.0f);
	glColor3f(0.1f, 0.1f, 0.1f);
	glVertex3f(-0.3f, 0.02f, 0.0f);

	glEnd();

	if (tx_top_mem_Flag <= 3.28f)
	{
		tx_top_mem_Flag = tx_top_mem_Flag + speed;
	}

	if (ty_top_mem_Flag >= 0.002f)
	{
		ty_top_mem_Flag = ty_top_mem_Flag - speed;
	}

	//	DRAW IAF
	//	I
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_top_mem_I, ty_top_mem_I, -3.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//	BLACK
	glVertex3f(0.0f, 0.05f, 0.0f);
	glVertex3f(0.0f, -0.05f, 0.0f);
	glEnd();

	if (tx_top_mem_I <= 3.28f)
	{
		tx_top_mem_I = tx_top_mem_I + speed;
	}

	if (ty_top_mem_I >= 0.0f)
	{
		ty_top_mem_I = ty_top_mem_I - speed;
	}

	if (tx_top_mem >= 1.9f)
	{
		ty_top_mem_I_UP = ty_top_mem_I_UP + speed;
		ty_top_mem_I = ty_top_mem_I_UP;
	}

	//	A
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_top_mem_A, ty_top_mem_A, -3.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//	BLACK
	glVertex3f(0.03f, -0.05f, 0.0f);
	glVertex3f(0.0f, 0.05f, 0.0f);
	glVertex3f(0.0f, 0.05f, 0.0f);
	glVertex3f(-0.03f, -0.05f, 0.0f);
	glVertex3f(-0.01f, 0.0f, 0.0f);
	glVertex3f(0.01f, 0.0f, 0.0f);
	glEnd();

	if (tx_top_mem_A <= 3.28f)
	{
		tx_top_mem_A = tx_top_mem_A + speed;
	}

	if (ty_top_mem_A >= 0.0f)
	{
		ty_top_mem_A = ty_top_mem_A - speed;
	}

	if (tx_top_mem >= 1.9f)
	{
		ty_top_mem_A_UP = ty_top_mem_A_UP + speed;
		ty_top_mem_A = ty_top_mem_A_UP;
	}

	//	F
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_top_mem_F, ty_top_mem_F, -3.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//	BLACK
	glVertex3f(-0.05f, 0.05f, 0.0f);
	glVertex3f(-0.05f, -0.05f, 0.0f);
	glVertex3f(-0.020f, 0.045f, 0.0f);
	glVertex3f(-0.05f, 0.045f, 0.0f);
	glVertex3f(-0.025f, 0.0f, 0.0f);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glEnd();

	if (tx_top_mem_F <= 3.28f)
	{
		tx_top_mem_F = tx_top_mem_F + speed;
	}

	if (ty_top_mem_F >= 0.0f)
	{
		ty_top_mem_F = ty_top_mem_F - speed;
	}

	if (tx_top_mem >= 1.9f)
	{
		ty_top_mem_F_UP = ty_top_mem_F_UP + speed;
		ty_top_mem_F = ty_top_mem_F_UP;
	}
}

void Animate_BottomMember()
{
	//	BOTTOM MEMBER
	static GLfloat tx_bottom_mem = -2.5f;
	static GLfloat tx_bottom_mem_front_rad = -2.3f;
	static GLfloat tx_bottom_mem_vertical = -2.78f;

	static GLfloat ty_bottom_mem = -2.4f;
	static GLfloat ty_bottom_mem_front_rad = -2.4f;
	static GLfloat ty_bottom_mem_vertical = -2.4f;

	static GLfloat ty_bottom_mem_DOWN = 0.0f;
	static GLfloat ty_bottom_mem_front_rad_DOWN = 0.0f;
	static GLfloat ty_bottom_mem_vertical_DOWN = 0.0f;

	static GLfloat tx_bottom_mem_I = -2.7f;
	static GLfloat tx_bottom_mem_A = -2.65f;
	static GLfloat tx_bottom_mem_F = -2.55f;

	static GLfloat ty_bottom_mem_I = -2.4f;
	static GLfloat ty_bottom_mem_A = -2.4f;
	static GLfloat ty_bottom_mem_F = -2.4f;

	static GLfloat ty_bottom_mem_I_DOWN = 0.0f;
	static GLfloat ty_bottom_mem_A_DOWN = 0.0f;
	static GLfloat ty_bottom_mem_F_DOWN = 0.0f;

	static GLfloat tx_bottom_mem_Flag = -3.095f;
	static GLfloat ty_bottom_mem_Flag = -2.4f;

	static GLfloat tx_bottom_mem_Flag_DOWN = 0.0f;
	static GLfloat ty_bottom_mem_Flag_DOWN = 0.0f;

	//	BOTTOM MEMBER POSITION
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_bottom_mem, ty_bottom_mem, -3.0f);
	glScalef(0.2f, 0.2f, 0.2f);
	glBegin(GL_POLYGON);
	//	BOTTOM MEMBER FINAL DESIGN 
	glColor3f(0.5f, 0.7f, 0.7f);	//	

	glVertex3f(0.0f, 0.15f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.2f, 1.0f, 0.0f);

	glVertex3f(-1.3f, 0.2f, 0.0f);
	glVertex3f(-1.4f, 0.15f, 0.0f);
	glVertex3f(-1.4f, -0.15f, 0.0f);
	glVertex3f(-1.3f, -0.2f, 0.0f);

	glVertex3f(-1.2f, -1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, -0.15f, 0.0f);
	glVertex3f(0.4f, -0.15f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.4f, 0.15f, 0.0f);
	glEnd();

	if (tx_bottom_mem <= 3.0f)
	{
		tx_bottom_mem = tx_bottom_mem + speed;
	}

	if (ty_bottom_mem <= 0.0f)
	{
		ty_bottom_mem = ty_bottom_mem + speed;
	}

	if (tx_bottom_mem >= 1.9f)
	{
		ty_bottom_mem_DOWN = ty_bottom_mem_DOWN - speed;
		ty_bottom_mem = ty_bottom_mem_DOWN;

		ty_bottom_mem_front_rad_DOWN = ty_bottom_mem_front_rad_DOWN - speed;
		ty_bottom_mem_front_rad = ty_bottom_mem_front_rad_DOWN;

		ty_bottom_mem_vertical_DOWN = ty_bottom_mem_vertical_DOWN - speed;
		ty_bottom_mem_vertical = ty_bottom_mem_vertical_DOWN;

		ty_bottom_mem_Flag_DOWN = ty_bottom_mem_Flag_DOWN - speed;
		ty_bottom_mem_Flag = ty_bottom_mem_Flag_DOWN;
	}

	//	BOTTOM MEMBER FRONT RADAR
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_bottom_mem_front_rad, ty_bottom_mem_front_rad, -3.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex3f(0.05f, 0.0f, 0.0f);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glEnd();

	if (tx_bottom_mem_front_rad <= 3.815f)
	{
		tx_bottom_mem_front_rad = tx_bottom_mem_front_rad + speed;
	}

	if (ty_bottom_mem_front_rad <= 0.0f)
	{
		ty_bottom_mem_front_rad = ty_bottom_mem_front_rad + speed;
	}

	//	BOTTOM MEMBER VERTICAL STABILIZER
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_bottom_mem_vertical, ty_bottom_mem_vertical, -3.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//BLACK
	glVertex3f(0.05f, 0.0f, 0.0f);
	glColor3f(0.5f, 0.7f, 0.7f);	//
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glEnd();

	if (tx_bottom_mem_vertical <= 3.28f)
	{
		tx_bottom_mem_vertical = tx_bottom_mem_vertical + speed;
	}

	if (ty_bottom_mem_vertical <= 0.0f)
	{
		ty_bottom_mem_vertical = ty_bottom_mem_vertical + speed;
	}

	//	DRAW GREEN SMOKE
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_bottom_mem_Flag, ty_bottom_mem_Flag, -3.0f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);

	glColor3f(0.07f, 0.53f, 0.03f);	//GREEN
	glVertex3f(0.3f, -0.02f, 0.0f);
	glColor3f(0.1f, 0.1f, 0.1f);
	glVertex3f(-0.301f, -0.02f, 0.0f);

	glEnd();

	if (tx_bottom_mem_Flag <= 3.28f)
	{
		tx_bottom_mem_Flag = tx_bottom_mem_Flag + speed;
	}

	if (ty_bottom_mem_Flag <= -0.002f)
	{
		ty_bottom_mem_Flag = ty_bottom_mem_Flag + speed;
	}

	//	DRAW IAF
	//	I
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_bottom_mem_I, ty_bottom_mem_I, -3.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//	BLACK
	glVertex3f(0.0f, 0.05f, 0.0f);
	glVertex3f(0.0f, -0.05f, 0.0f);
	glEnd();

	if (tx_bottom_mem_I <= 3.28f)
	{
		tx_bottom_mem_I = tx_bottom_mem_I + speed;
	}

	if (ty_bottom_mem_I <= 0.0f)
	{
		ty_bottom_mem_I = ty_bottom_mem_I + speed;
	}

	if (tx_bottom_mem >= 1.9f)
	{
		ty_bottom_mem_I_DOWN = ty_bottom_mem_I_DOWN - speed;
		ty_bottom_mem_I = ty_bottom_mem_I_DOWN;
	}

	//	A
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_bottom_mem_A, ty_bottom_mem_A, -3.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//	BLACK
	glVertex3f(0.03f, -0.05f, 0.0f);
	glVertex3f(0.0f, 0.05f, 0.0f);
	glVertex3f(0.0f, 0.05f, 0.0f);
	glVertex3f(-0.03f, -0.05f, 0.0f);
	glVertex3f(-0.01f, 0.0f, 0.0f);
	glVertex3f(0.01f, 0.0f, 0.0f);
	glEnd();

	if (tx_bottom_mem_A <= 3.28f)
	{
		tx_bottom_mem_A = tx_bottom_mem_A + speed;
	}

	if (ty_bottom_mem_A <= 0.0f)
	{
		ty_bottom_mem_A = ty_bottom_mem_A + speed;
	}

	if (tx_bottom_mem >= 1.9f)
	{
		ty_bottom_mem_A_DOWN = ty_bottom_mem_A_DOWN - speed;
		ty_bottom_mem_A = ty_bottom_mem_A_DOWN;
	}

	//	F
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(tx_bottom_mem_F, ty_bottom_mem_F, -3.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);	//	BLACK
	glVertex3f(-0.05f, 0.05f, 0.0f);
	glVertex3f(-0.05f, -0.05f, 0.0f);
	glVertex3f(-0.020f, 0.045f, 0.0f);
	glVertex3f(-0.05f, 0.045f, 0.0f);
	glVertex3f(-0.025f, 0.0f, 0.0f);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glEnd();

	if (tx_bottom_mem_F <= 3.28f)
	{
		tx_bottom_mem_F = tx_bottom_mem_F + speed;
	}

	if (ty_bottom_mem_F <= 0.0f)
	{
		ty_bottom_mem_F = ty_bottom_mem_F + speed;
	}

	if (tx_bottom_mem >= 1.9f)
	{
		ty_bottom_mem_F_DOWN = ty_bottom_mem_F_DOWN - speed;
		ty_bottom_mem_F = ty_bottom_mem_F_DOWN;
	}
}

void Draw_A_Centre()
{
	//	A - CENTRE	
	static GLfloat cx1_A_Centre = 0.0f, cy1_A_Centre = 0.0f, cz1_A_Centre = 0.0f;
	static GLfloat cx2_A_Centre = 0.0f, cy2_A_Centre = 0.0f, cz2_A_Centre = 0.0f;
	static GLfloat cx3_A_Centre = 0.0f, cy3_A_Centre = 0.0f, cz3_A_Centre = 0.0f;

	//	DRAW TRI-COLOUR SMOKE AT CENTRE
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.191f, 0.0f, -3.0f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);
	glColor3f(cx1_A_Centre, cy1_A_Centre, cz1_A_Centre);	//BHAGWA
	glVertex3f(0.125f, 0.02f, 0.0f);
	glVertex3f(-0.155f, 0.02f, 0.0f);

	glColor3f(cx2_A_Centre, cy2_A_Centre, cz2_A_Centre);
	glVertex3f(0.125f, 0.00f, 0.0f);
	glVertex3f(-0.155f, 0.00f, 0.0f);

	glColor3f(cx3_A_Centre, cy3_A_Centre, cz3_A_Centre);
	glVertex3f(0.125f, -0.02f, 0.0f);
	glVertex3f(-0.155f, -0.02f, 0.0f);

	glEnd();

	if (cx1_A_Centre <= 1.0f)
		cx1_A_Centre = cx1_A_Centre + 0.003f;
	if (cy1_A_Centre <= 0.6f)
		cy1_A_Centre = cy1_A_Centre + 0.003f;
	if (cz1_A_Centre <= 0.2f)
		cz1_A_Centre = cz1_A_Centre + 0.003f;

	if (cx2_A_Centre <= 0.9f)
		cx2_A_Centre = cx2_A_Centre + 0.003f;
	if (cy2_A_Centre <= 0.9f)
		cy2_A_Centre = cy2_A_Centre + 0.003f;
	if (cz2_A_Centre <= 0.9f)
		cz2_A_Centre = cz2_A_Centre + 0.003f;

	if (cx3_A_Centre <= 0.07f)
		cx3_A_Centre = cx3_A_Centre + 0.003f;
	if (cy3_A_Centre <= 0.53f)
		cy3_A_Centre = cy3_A_Centre + 0.003f;
	if (cz3_A_Centre <= 0.03f)
		cz3_A_Centre = cz3_A_Centre + 0.003f;

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
