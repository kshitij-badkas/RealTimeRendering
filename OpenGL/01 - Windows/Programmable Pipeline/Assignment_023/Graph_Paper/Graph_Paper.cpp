//Header files
#include "Headers.h"
#include <windows.h>
#include <stdio.h>
#include <gl\glew.h>
#include <gl\GL.h>
#include "vmath.h"


//PRAGMA
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

//MACROS
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
//new
#define CHECK_IMAGE_WIDTH 64
#define CHECK_IMAGE_HEIGHT 64

using namespace vmath;

enum
{
	KAB_ATTRIBUTE_POSITION = 0,
	KAB_ATTRIBUTE_COLOR,
	KAB_ATTRIBUTE_NORMAL,
	KAB_ATTRIBUTE_TEXCOORD
};

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

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gMVPMatrixUniform;

GLuint gColorUniform;

mat4 gPerspectiveProjectionMatrix;

GLuint gVao_Lines;
GLuint gVbo_Position_Lines;
GLuint gVbo_Color_Lines;


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
		TEXT("PP: Assignment_023_Graph_Paper_All_Shapes : By Kshitij Badkas"),
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
	pfd.cDepthBits = 32;	//3D & Depth

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

	//GLEW Initialization
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//OPENGL RELATED LOG
	fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	//OPENGL ENABLE EXTENSIONS
	GLint numExt;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
	for (int i = 0; i < numExt; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	//VERTEX SHADER
	//create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	//use 'core' profile i.e PROGRAMMABLE PIPELINE
	const GLchar* vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvpMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvpMatrix * vPosition;" \
		"}";


	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);

	//Shader Compilation Error Checking
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)	//if Shader Compiled Status Failed
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	//FRAGMENT SHADER
	//create shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"out vec4 FragColor;" \
		"uniform vec4 u_Color;" \
		"void main(void)" \
		"{" \
		"FragColor = u_Color;" \
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject);

	//Shader Compilation Error Checking
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	//SHADER PROGRAM
	//create
	gShaderProgramObject = glCreateProgram();

	//attach vertex shader object to program 
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	//attach fragment shader object to program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	//pre-link binding of shader program object with vertex shader attribute
	glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_POSITION, "vPosition");

	//pre-link binding of shader program object with fragment shader attribute
	glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_COLOR, "vColor");

	//link shader
	glLinkProgram(gShaderProgramObject);

	//Shader Program Link Error Checking
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get Uniform Location
	gMVPMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");
	gColorUniform = glGetUniformLocation(gShaderProgramObject, "u_Color");
	

	//vertices, colours, shader attribs, vbo, vao initialization
	
	//LINES
	//GEN START
	glGenVertexArrays(1, &gVao_Lines);
	//BIND START
	glBindVertexArray(gVao_Lines);

	//FOR LINES POSITION
	glGenBuffers(1, &gVbo_Position_Lines);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); // 2 vertices with (x,y)
	
	//FOR LINES POSITION
	glGenBuffers(1, &gVbo_Color_Lines);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 3 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); // 2 vertices with (x,y)

	//BIND END
	glBindVertexArray(0);


	//3D & Depth Code
	glClearDepth(1.0f);		//Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//we remove culling of faces
	glDisable(GL_CULL_FACE);

	//SetglColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black

	//set identity matrix to identity matrix
	gPerspectiveProjectionMatrix = mat4::identity();

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

	//gluPerspective
	gPerspectiveProjectionMatrix = perspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f);
}

//Function Display
void Display(void)
{
	//variables
	//CO-ORDINATES
	GLfloat lines[2000];  //each line has 4 coordinates x 20 - 20 lines drawn - they will change coordinates
	GLfloat horizontalCentreLine[] = {-1.0f, 0.0f, 1.0f, 0.0f}; //central horizontal line
	GLfloat verticalCentreLine[] = { 0.0f, 1.0f, 0.0f, -1.0f }; //central vertical line

	GLfloat redColor[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat greenColor[] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat blueColor[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat whiteColor[] = { 1.0, 1.0, 1.0, 1.0 };


	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	//OpenGL Drawing
	mat4 translationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	modelViewMatrix = translationMatrix;
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//
	// 
	//
	
	//bind vao - vertex array object
	glBindVertexArray(gVao_Lines);

	//HORIZONTAL LINES TOP
	GLint i = 0;
	GLfloat inc = 0.05f;
	for (i = 0; i < 80; i += 4, inc += 0.05f)
	{
		lines[i] = -1.0f; //x1
		lines[i + 1] = inc; //y1
		lines[i + 2] = 1.0f; //x2
		lines[i + 3] = inc; //y2
	}
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUniform4fv(gColorUniform, 1, (GLfloat*)blueColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 40); //40 indices to render for 20 lines


	//HORIZONTAL LINE CENTRE
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(GLfloat), horizontalCentreLine, GL_DYNAMIC_DRAW); // 2 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUniform4fv(gColorUniform, 1, (GLfloat*)redColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 2); //2 indices to render

	
	//HORIZONTAL LINES BOTTOM
	i = 0;
	inc = 0.05f;
	for (i = 0; i < 80; i += 4, inc += 0.05f)
	{
		lines[i] = -1.0f; //x1
		lines[i + 1] = -inc; //y1
		lines[i + 2] = 1.0f; //x2
		lines[i + 3] = -inc; //y2

	}
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUniform4fv(gColorUniform, 1, (GLfloat*)blueColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 40); //40 indices to render for 20 lines

	///////////////////////
	//VERTICAL LINES LEFT
	i = 0;
	inc = 0.05f;
	for (i = 0; i < 80; i += 4, inc += 0.05f)
	{
		lines[i] = -inc; //x1
		lines[i + 1] = 1.0; //y1
		lines[i + 2] = -inc; //x2
		lines[i + 3] = -1.0; //y2
	}
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUniform4fv(gColorUniform, 1, (GLfloat*)blueColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 40); //40 indices to render for 20 lines

	//VERTICAL LINE CENTRE - green
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(GLfloat), verticalCentreLine, GL_DYNAMIC_DRAW); // 2 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glUniform4fv(gColorUniform, 1, (GLfloat*)greenColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 2); //40 indices to render for 20 lines


	//VERTICAL LINES RIGHT
	i = 0;
	inc = 0.05f;
	for (i = 0; i < 80; i += 4, inc += 0.05f)
	{
		lines[i] = inc; //x1
		lines[i + 1] = 1.0; //y1
		lines[i + 2] = inc; //x2
		lines[i + 3] = -1.0; //y2

	}
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 40);

	glUniform4fv(gColorUniform, 1, (GLfloat*)blueColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 40); //40 indices to render for 20 lines

	//
	// 
	//

	//TRAINGLE USING LINES
	lines[0] = 0.0f;
	lines[1] = 0.7f;
	lines[2] = 0.7f;
	lines[3] = -0.7f;

	lines[4] = 0.7f;
	lines[5] = -0.7f;
	lines[6] = -0.7f;
	lines[7] = -0.7f;
	
	lines[8] = -0.7f;
	lines[9] = -0.7f;
	lines[10] = 0.0f;
	lines[11] = 0.7f;

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 2 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 6); //6 indices to render

	glUniform4fv(gColorUniform, 1, (GLfloat*)whiteColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 6); //40 indices to render for 20 lines

	//
	// 
	//

	//RECTANGLE USING LINES
	lines[0] = 0.7f;
	lines[1] = 0.7f;
	lines[2] = 0.7f;
	lines[3] = -0.7f;

	lines[4] = 0.7f;
	lines[5] = 0.7f;
	lines[6] = -0.7f;
	lines[7] = 0.7f;

	lines[8] = -0.7f;
	lines[9] = 0.7f;
	lines[10] = -0.7f;
	lines[11] = -0.7f;

	lines[12] = -0.7f;
	lines[13] = -0.7f;
	lines[14] = 0.7f;
	lines[15] = -0.7f;

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 8 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 8 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 8); //8 indices to render

	glUniform4fv(gColorUniform, 1, (GLfloat*)whiteColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 8 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 8); //40 indices to render for 20 lines


	//
	// 
	//
	
	//CIRCUMCIRCLE USING POINTS
	GLfloat circumCentreRadius = 0.0f;

	circumCentreRadius = (GLfloat)sqrt(pow(0.7f, 2) + pow(0.7f, 2));
	fprintf(gpFile, "THE RADIUS OF CIRCUMCIRCLE IS = %f \n", circumCentreRadius);
	fprintf(gpFile, "THE CIRCUMFERENCE OF CIRCUMCIRCLE IS = %f \n", circumCentreRadius * 2.0f * 3.14f);
	//OUTPUT ->
	//THE RADIUS OF CIRCUMCIRCLE IS = 0.989949 
	//THE CIRCUMFERENCE OF CIRCUMCIRCLE IS = 6.216883


	i = 0;
	GLfloat cAngle = 0.0f;
	for (cAngle = 0.0f; cAngle < (2.0f * 3.14f); cAngle = cAngle + 0.01f, i += 2)
	{
		lines[i] = (GLfloat)cos(cAngle) * (GLfloat)circumCentreRadius; //xn
		lines[i + 1] = (GLfloat)sin(cAngle) * (GLfloat)circumCentreRadius; //yn
	}

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 2000 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_POINTS, 0, 650); //1000 indices to render

	glUniform4fv(gColorUniform, 1, (GLfloat*)whiteColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 650); //40 indices to render for 20 lines

	
	//
	// 
	//

	//INCIRCLE USING POINTS
	GLfloat inCircleRadius = 0.0f;
	GLfloat distance_A = 0.0f;
	GLfloat distance_B = 0.0f;
	GLfloat distance_C = 0.0f;
	GLfloat perimeter = 0.0f;
	GLfloat semi = 0.0f;
	GLfloat tArea = 0.0f;
	//centre of circle
	GLfloat iX = 0.0f;
	GLfloat iY = 0.0f;

	//	DISTANCE FORMULA
	distance_A = (GLfloat)sqrt(pow((-0.7f - 0.7f), 2) + pow((-0.7f - -0.7f), 2));
	distance_B = (GLfloat)sqrt(pow((0.0f - -0.7f), 2) + pow((0.7f - -0.7f), 2));
	distance_C = (GLfloat)sqrt(pow((0.7f - 0.0f), 2) + pow((-0.7f - 0.7f), 2));

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
	iX = (GLfloat)(distance_A * (0.0f) + distance_B * (0.7f) + distance_C * (-0.7f)) / perimeter;
	iY = (GLfloat)(distance_A * (0.7f) + distance_B * (-0.7f) + distance_C * (-0.7f)) / perimeter;

	fprintf(gpFile, "iX- %f\n", iX);
	fprintf(gpFile, "iY- %f\n", iY);


	inCircleRadius = (GLfloat)(tArea / semi);
	fprintf(gpFile, "THE RADIUS OF INCIRCLE IS = %f \n", inCircleRadius);
	fprintf(gpFile, "THE CIRCUMFERENCE OF INCIRCLE IS = %f \n", inCircleRadius * 2.0f * 3.14f);

	//OUTPUT ->
	
	i = 0;
	cAngle = 0.0f;
	for (cAngle = 0.0f; cAngle < (2.0f * 3.14f); cAngle = cAngle + 0.01f, i += 2)
	{
		lines[i] = iX + (GLfloat)cos(cAngle) * (GLfloat)inCircleRadius; //xn
		lines[i + 1] = iY + (GLfloat)sin(cAngle) * (GLfloat)inCircleRadius; //yn
	}

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Lines);
	glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 2000 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_POINTS, 0, 650); //1000 indices to render

	glUniform4fv(gColorUniform, 1, (GLfloat*)whiteColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Lines);
	glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_LINES, 0, 650); //40 indices to render for 20 lines

	glBindVertexArray(0);
	
	//-------------------------------draw end-------------------------------------//

	//stop using OpenGL program object
	glUseProgram(0);

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

	//SQUARE
	//destroy vao
	if (gVao_Lines)
	{
		glDeleteVertexArrays(1, &gVao_Lines);
		gVao_Lines = 0;
	}

	//destroy vbo of position
	if (gVbo_Position_Lines)
	{
		glDeleteVertexArrays(1, &gVbo_Position_Lines);
		gVbo_Position_Lines = 0;
	}

	if (gVbo_Color_Lines)
	{
		glDeleteVertexArrays(1, &gVbo_Color_Lines);
		gVbo_Color_Lines = 0;
	}

	//SAFE SHADER CLEANUP
	if (gShaderProgramObject)
	{
		glUseProgram(gShaderProgramObject);

		GLsizei shaderCount;
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShader = NULL;
		pShader = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		if (pShader == NULL)
		{
			//error checking
			fprintf(gpFile, "Error Creating pShader.\n Exitting Now!!\n");
			DestroyWindow(ghwnd);
		}

		glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShader);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject, pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i] = 0;
		}

		free(pShader);

		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;

		//unlink
		glUseProgram(0);
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

