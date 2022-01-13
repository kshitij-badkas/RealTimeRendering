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

mat4 gPerspectiveProjectionMatrix;

GLuint gVao_Cube, gVbo_Position_Cube; //CUBE

//NEW
//GLuint gVbo_Color_Cube;
GLuint gVbo_Normal_Cube;
GLuint modelViewMatrixUniform;
GLuint projectionMatrixUniform;
GLuint LKeyPressedUniform;
GLuint LdUniform;
GLuint KdUniform;
GLuint lightPositionUniform;
bool gbAnimate, gbLights;

//GLfloat angleCube = 0.0f;

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


	//CreateWindowEx
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName,
		TEXT("PP: Assignment_011_3D_Diffuse_Light_On_Cube : By Kshitij Badkas"),
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

	case WM_CHAR:
		switch (wParam)
		{
		//Animation
		case 'A':
		case 'a':
			if (gbAnimate == true)
			{
				gbAnimate = false;
			}
			else if (gbAnimate == false)
			{
				gbAnimate = true;
			}
			break;
		
		//Light
		case 'L':
		case 'l':
			if (gbLights == true)
			{
				gbLights = false;
			}
			else if (gbLights == false)
			{
				gbLights = true;
			}
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
		"in vec3 vNormal;" \
		"uniform mat4 u_model_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec3 u_LD;" \
		"uniform vec3 u_KD;" \
		"uniform vec4 u_light_position;" \
		"out vec3 diffuse_light;" \
		"void main(void)" \
		"{" \
			//LIGHT ENABLED
			"if(u_L_KEY_PRESSED == 1)" \
			"{" \
			//STEPS -
			//1. CALCUALTE EYE COORDINATES -> by multiplying position coordinates and model-view matrix
			//2. CALCUALTE NORMAL MATRIX -> by inverse of transpose of upper 3x3 of model-view matrix
			//3. CALCULATE TRANSFORMED NORMALS ->
			//4. CALCULATE 'S' SOURCE OF LIGHT ->
			//5. CALCULATE DIFFUSE LIGHT USING LIGHT EQUATION ->

			"vec4 eye_coordinates = u_model_view_matrix * vPosition;" \
			"mat3 normal_matrix = mat3(transpose(inverse(u_model_view_matrix)));" \
			"vec3 tnorm = normalize(normal_matrix * vNormal);" \
			"vec3 s = normalize(vec3(u_light_position - eye_coordinates));" \
			"diffuse_light = u_LD * u_KD * max(dot(s, tnorm), 0.0f);" \
			"}" \
			"gl_Position =  u_projection_matrix * u_model_view_matrix * vPosition;" \
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
		"in vec3 diffuse_light;" \
		"uniform int u_L_KEY_PRESSED;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
			"vec4 color;" \
			"if(u_L_KEY_PRESSED == 1)" \
			"{" \
			"color = vec4(diffuse_light, 1.0f);" \
			"}" \
			"else" \
			"{" \
			"color = vec4(1.0f, 1.0f, 1.0f, 1.0f);" \
			"}" \
			"FragColor = color;" \
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

	//NEW
	glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_NORMAL, "vNormal");

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

	//NEW
	modelViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_view_matrix");
	projectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_L_KEY_PRESSED");
	LdUniform = glGetUniformLocation(gShaderProgramObject, "u_LD");
	KdUniform = glGetUniformLocation(gShaderProgramObject, "u_KD");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

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

	const GLfloat cubeNormals[] =
	{
		// top surface
		0.0f, 1.0f, 0.0f,  // top-right of top
		0.0f, 1.0f, 0.0f, // top-left of top
		0.0f, 1.0f, 0.0f, // bottom-left of top
		0.0f, 1.0f, 0.0f,  // bottom-right of top

		// bottom surface
		0.0f, -1.0f, 0.0f,  // top-right of bottom
		0.0f, -1.0f, 0.0f,  // top-left of bottom
		0.0f, -1.0f, 0.0f,  // bottom-left of bottom
		0.0f, -1.0f, 0.0f,   // bottom-right of bottom

		// front surface
		0.0f, 0.0f, 1.0f,  // top-right of front
		0.0f, 0.0f, 1.0f, // top-left of front
		0.0f, 0.0f, 1.0f, // bottom-left of front
		0.0f, 0.0f, 1.0f,  // bottom-right of front

		// back surface
		0.0f, 0.0f, -1.0f,  // top-right of back
		0.0f, 0.0f, -1.0f, // top-left of back
		0.0f, 0.0f, -1.0f, // bottom-left of back
		0.0f, 0.0f, -1.0f,  // bottom-right of back

		// left surface
		-1.0f, 0.0f, 0.0f, // top-right of left
		-1.0f, 0.0f, 0.0f, // top-left of left
		-1.0f, 0.0f, 0.0f, // bottom-left of left
		-1.0f, 0.0f, 0.0f, // bottom-right of left

		// right surface
		1.0f, 0.0f, 0.0f,  // top-right of right
		1.0f, 0.0f, 0.0f,  // top-left of right
		1.0f, 0.0f, 0.0f,  // bottom-left of right
		1.0f, 0.0f, 0.0f  // bottom-right of right
	};

	//FOR CUBE
	//GEN START
	glGenVertexArrays(1, &gVao_Cube);
	//BIND START
	glBindVertexArray(gVao_Cube);

	//FOR SQUARE POSITION
	glGenBuffers(1, &gVbo_Position_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//NEW
	//FOR CUBE NORMALS
	glGenBuffers(1, &gVbo_Normal_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Normal_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//BIND END
	glBindVertexArray(0);


	//3D & Depth Code
	glClearDepth(1.0f);		//Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//Disable culling of back faces (default)
	glDisable(GL_CULL_FACE);
	
	//SetglColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black
	
	//NEW
	gbAnimate = false;
	gbLights = false;
	
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
	static GLfloat angleCube = 0.0f;
	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	//NEW
	if (gbLights == true)
	{
		glUniform1i(LKeyPressedUniform, 1);
		glUniform3f(LdUniform, 1.0f, 1.0f, 1.0f); //white
		glUniform3f(KdUniform, 0.5f, 0.5f, 0.5f); //grey
		GLfloat lightPosition[] = { 0.0f,0.0f,2.0f,1.0f };
		glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	//OpenGL Drawing
	mat4 translationMatrix;
	mat4 scaleMatrix;
	mat4 rotationMatrix;
	mat4 modelViewMatrix;

	//CUBE
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -6.0f);
	rotationMatrix = rotate(angleCube, angleCube, angleCube);
	scaleMatrix = scale(0.75f, 0.75f, 0.75f);
	modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;

	//NEW
	glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

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

	//unbind vao
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);


	//ANIMATION CODE
	if (gbAnimate == true)
	{
		angleCube += 1.0f;
		if (angleCube >= 360.0f)
		{
			angleCube = 0.0f;
		}
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

	//NEW
	//destroy vbo of normal
	if (gVbo_Normal_Cube)
	{
		glDeleteVertexArrays(1, &gVbo_Normal_Cube);
		gVbo_Normal_Cube = 0;
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

