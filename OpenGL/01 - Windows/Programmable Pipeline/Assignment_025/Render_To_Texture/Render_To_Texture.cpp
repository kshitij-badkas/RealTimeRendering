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

GLuint gVertexShaderObject_color;
GLuint gFragmentShaderObject_color;
GLuint gShaderProgramObject_color;
GLuint gMVPMatrixUniform_color;


mat4 gPerspectiveProjectionMatrix;

GLuint gVao_Cube; 
GLuint gVbo_Position_Cube; 
GLuint gVbo_Texture_Cube;

GLuint gVao_Cube_2;
GLuint gVbo_Position_Cube_2;
GLuint gVbo_Color_Cube;

//TEXTURES
GLuint kundali_texture;

GLuint gTextureSamplerUniform;


//FRAMEBUFFER
GLuint gFbo;
GLuint color_texture;
GLuint depth_texture;

int gWidth;
int gHeight;

// WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{

	//NEW PROTOTYPES
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
		TEXT("PP: Assignment_024_Framebuffer : By Kshitij Badkas"),
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
	bool LoadGLTexture(GLuint*, TCHAR[]); //NEW

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
		"in vec2 vTexCoord;" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec2 out_texcoord;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvpMatrix * vPosition;" \
		"out_texcoord = vTexCoord;" \
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
		"in vec2 out_texcoord;" \
		"uniform sampler2D u_texture_sampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(u_texture_sampler, out_texcoord);" \
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
	glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_TEXCOORD, "vTexCoord");

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

	//VERTEX SHADER AND FRAGMENT SHADER FOR COLOR
	// 
	//VERTEX SHADER
	//create shader
	gVertexShaderObject_color = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	//use 'core' profile i.e PROGRAMMABLE PIPELINE
	const GLchar* vertexShaderSourceCode_color =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvpMatrix * vPosition;" \
		"out_color = vColor;" \
		"}";

	glShaderSource(gVertexShaderObject_color, 1, (const GLchar**)&vertexShaderSourceCode_color, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject_color);

	//Shader Compilation Error Checking
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject_color, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)	//if Shader Compiled Status Failed
	{
		glGetShaderiv(gVertexShaderObject_color, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_color, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log ---> COLOR: %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	//FRAGMENT SHADER
	//create shader
	gFragmentShaderObject_color = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar* fragmentShaderSourceCode_color =
		"#version 450 core" \
		"\n" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = out_color;" \
		"}";

	glShaderSource(gFragmentShaderObject_color, 1, (const GLchar**)&fragmentShaderSourceCode_color, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject_color);

	//Shader Compilation Error Checking
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject_color, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_color, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_color, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log ---> COLOR : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}


	//SHADER PROGRAM FOR COLOR
	//create
	gShaderProgramObject_color = glCreateProgram();

	//attach vertex shader object to program 
	glAttachShader(gShaderProgramObject_color, gVertexShaderObject_color);

	//attach fragment shader object to program
	glAttachShader(gShaderProgramObject_color, gFragmentShaderObject_color);

	//pre-link binding of shader program object with vertex shader attribute
	glBindAttribLocation(gShaderProgramObject_color, KAB_ATTRIBUTE_POSITION, "vPosition");

	//pre-link binding of shader program object with fragment shader attribute
	glBindAttribLocation(gShaderProgramObject_color, KAB_ATTRIBUTE_COLOR, "vColor");

	//link shader
	glLinkProgram(gShaderProgramObject_color);

	//Shader Program Link Error Checking
	iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject_color, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_color, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_color, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log ---> COLOR : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get MVP Uniform Location
	gMVPMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");

	//get Texture Sampler Uniform Location
	gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");

	//get Uniform Location for COLOR
	gMVPMatrixUniform_color = glGetUniformLocation(gShaderProgramObject_color, "u_mvpMatrix");

	//vertices, colours, shader attribs, vbo, vao initializations

	//CUBE 
	const GLfloat cubeVertices[] =
	{
		//FRONT
		1.0f, 1.0f, 1.0f, //right-top
		-1.0f, 1.0f, 1.0f, //left-top
		-1.0f, -1.0f, 1.0f, //left-bottom
		1.0f, -1.0f, 1.0f, //right-bottom
		//RIGHT
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		//BACK
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		//LEFT
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		//TOP
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		//BOTTOM
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	const GLfloat cubeTexCoords[] =
	{
		//FRONT
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		//RIGHT
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		//BACK
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		//LEFT
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		//TOP
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		//BOTTOM
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	const GLfloat cubeColors[] =
	{
		//FRONT
		1.0f, 0.0f, 0.0f, //R
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//RIGHT
		0.0f, 1.0f, 0.0f, //G
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//BACK
		0.0f, 0.0f, 1.0f, //B
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//LEFT
		0.0f, 1.0f, 1.0f, //C
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		//TOP
		1.0f, 0.0f, 1.0f, //M
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		//BOTTOM
		1.0f, 1.0f, 0.0f, //Y
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};


	//FOR CUBE
	//GEN START
	glGenVertexArrays(1, &gVao_Cube);
	//BIND START
	glBindVertexArray(gVao_Cube);

	//FOR CUBE POSITION
	glGenBuffers(1, &gVbo_Position_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//FOR CUBE TEXTURE
	glGenBuffers(1, &gVbo_Texture_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoords), cubeTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//BIND END
	glBindVertexArray(0);

	//FOR COLOR
	//GEN START
	glGenVertexArrays(1, &gVao_Cube_2);
	//BIND START
	glBindVertexArray(gVao_Cube_2);

	//FOR CUBE POSITION
	glGenBuffers(1, &gVbo_Position_Cube_2);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube_2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//FOR CUBE COLOR
	glGenBuffers(1, &gVbo_Color_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//BIND END
	glBindVertexArray(0);


	//3D & Depth Code
	glClearDepth(1.0f);		//Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//Disable culling of back faces (default)
	glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);

	//Loading Textures
	LoadGLTexture(&kundali_texture, MAKEINTRESOURCE(KUNDALI_BITMAP));

	//Enable Textures
	glEnable(GL_TEXTURE_2D);


	////////////////////////////////////////////////
	//FRAMEBUFFER
	glGenFramebuffers(1, &gFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, gFbo);

	glGenTextures(1, &color_texture);
	glBindTexture(GL_TEXTURE_2D, color_texture);
	glTexStorage2D(GL_TEXTURE_2D, 9, GL_RGBA8, 512, 512);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexStorage2D(GL_TEXTURE_2D, 9, GL_DEPTH_COMPONENT32F, 512, 512);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);

	////////////////////////////////////////////////

	//SetglColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black

	//set identity matrix to identity matrix
	gPerspectiveProjectionMatrix = mat4::identity();

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
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //make 2nd parameter '1' in PP instead of '4' for Faster Performance.

		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		//Setting texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//Push the Data into Graphics Memory with the Help of Graphics Driver
		//gluBuild2DMipmaps = glTexImage2D + glGenerateMipmap
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);

		// image loading
		DeleteObject(hBitmap);
	}

	return bResult;
}


//Function Resize
void Resize(int width, int height)
{
	//code
	gWidth = width;
	gHeight = height;

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
	static GLfloat angleCube = 0.0f;
	static const GLfloat green[] = { 0.0f, 0.5f, 0.0f, 1.0f };
	static const GLfloat blue[] = { 0.0f, 0.0f, 0.3f, 1.0f };
	static const GLfloat one = 1.0f;


	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//OpenGL Drawing
	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 scaleMatrix = mat4::identity(); //NEW
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	//CUBE
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -6.0f);
	rotationMatrix = rotate(angleCube, angleCube, angleCube);
	scaleMatrix = scale(0.85f, 0.85f, 0.85f);
	modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix; //NEW 
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

	//FRAMEBUFFER
	glBindFramebuffer(GL_FRAMEBUFFER, gFbo);

	glViewport(0, 0, 512, 512);
	glClearBufferfv(GL_COLOR, 0, green);
	//glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
	glClearBufferfv(GL_DEPTH, 0, &one);


	//start using OpenGL program object
	glUseProgram(gShaderProgramObject_color); // non textured object
	
	glUniformMatrix4fv(gMVPMatrixUniform_color, 1, GL_FALSE, modelViewProjectionMatrix);

	//bind vao - vertex array object
	glBindVertexArray(gVao_Cube_2);

	//draw
	//CUBE HAS 6 FACES, HENCE CALLING SIX TIMES - "2ND PARAMETER WILL CHANGE ACCORDINGLY"
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//unbind vao
	//glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);

	glViewport(0, 0, (GLsizei)gWidth, (GLsizei)gHeight);
	glClearBufferfv(GL_COLOR, 0, blue);
	glClearBufferfv(GL_DEPTH, 0, &one);

	//glBindTexture(GL_TEXTURE_2D, color_texture);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_texture);
	//glBindTexture(GL_TEXTURE_2D, kundali_texture);
	glUniform1i(gTextureSamplerUniform, 0);

	
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//bind vao - vertex array object
	glBindVertexArray(gVao_Cube);

	//draw
	//CUBE HAS 6 FACES, HENCE CALLING SIX TIMES - "2ND PARAMETER WILL CHANGE ACCORDINGLY"
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindTexture(GL_TEXTURE_2D, 0);

	//unbind vao
	//glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);

	//ANIMATION CODE
	angleCube += 1.0f;
	if (angleCube >= 360.0f)
	{
		angleCube = 0.0f;
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

	//SQUARE
	//destroy vao
	if (gVao_Cube)
	{
		glDeleteVertexArrays(1, &gVao_Cube);
		gVao_Cube = 0;
	}

	//destroy vbo of position
	if (gVbo_Position_Cube)
	{
		glDeleteVertexArrays(1, &gVbo_Position_Cube);
		gVbo_Position_Cube = 0;
	}

	//destroy vbo of color
	if (gVbo_Texture_Cube)
	{
		glDeleteVertexArrays(1, &gVbo_Texture_Cube);
		gVbo_Texture_Cube = 0;
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

