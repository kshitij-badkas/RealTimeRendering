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

mat4 gPerspectiveProjectionMatrix;

GLuint gVao_Square;
GLuint gVbo_Position_Square;
//NEW
GLuint gVbo_Color_Square;
GLuint gVbo_Texcoord_Square;
GLuint smiley_texture;
GLuint gTextureSamplerUniform;
GLuint gOtherKeyPressedUniform;

unsigned int pressed_key = 0;

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
		TEXT("PP: Assignment_019_Tweaked_Smiley : By Kshitij Badkas"),
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
			fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
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
		"uniform int u_other_key_pressed;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec4 color;" \
		"if(u_other_key_pressed == 1)" \
		"{" \
		"color = vec4(1.0f, 1.0f, 1.0f, 1.0f);" \
		"}" \
		"else" \
		"{" \
		"color = texture(u_texture_sampler, out_texcoord);" \
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

	//get MVP Uniform Location
	gMVPMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");

	//get Texture Sampler Uniform Location
	gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");

	//if other key is pressed
	gOtherKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_other_key_pressed");

	//vertices, colours, shader attribs, vbo, vao initializations
	//SQUARE
	const GLfloat squareVertices[] =
	{
		1.0f, 1.0f, 1.0f, //right-top
		-1.0f, 1.0f, 1.0f, //left-top
		-1.0f, -1.0f, 1.0f, //left-bottom
		1.0f, -1.0f, 1.0f //right-bottom
	};

	//FOR SQUARE
	//GEN START
	glGenVertexArrays(1, &gVao_Square);
	//BIND START
	glBindVertexArray(gVao_Square);

	//FOR SQUARE POSITION
	glGenBuffers(1, &gVbo_Position_Square);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//FOR SQUARE TEXCOORD
	glGenBuffers(1, &gVbo_Texcoord_Square);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texcoord_Square);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); //new
	glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//BIND END
	glBindVertexArray(0);


	//3D & Depth Code
	glShadeModel(GL_SMOOTH);	//anti-aliasing
	glClearDepth(1.0f);		//Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//we will always cull back faces for beter performance
	glEnable(GL_CULL_FACE);

	//Loading Textures
	LoadGLTexture(&smiley_texture, MAKEINTRESOURCE(SMILEY_BITMAP));

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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
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
	static GLfloat texCoord[8] = { 0 };

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	//OpenGL Drawing
	mat4 translationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	//SQUARE
	translationMatrix = translate(0.0f, 0.0f, -6.0f);
	modelViewMatrix = translationMatrix;
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, smiley_texture);
	glUniform1i(gTextureSamplerUniform, 0);

	//bind vao - vertex array object
	glBindVertexArray(gVao_Square);

	//draw
	//other key not pressed and 0 < pressed_key < 4
	glUniform1i(gOtherKeyPressedUniform, 0);
	if (pressed_key == 1)
	{
		texCoord[0] = 1.0f;
		texCoord[1] = 1.0f;
		texCoord[2] = 0.0f;
		texCoord[3] = 1.0f;
		texCoord[4] = 0.0f;
		texCoord[5] = 0.0f;
		texCoord[6] = 1.0f;
		texCoord[7] = 0.0f;

	}
	else if (pressed_key == 2)
	{
		texCoord[0] = 0.5f;
		texCoord[1] = 0.5f;
		texCoord[2] = 0.0f;
		texCoord[3] = 0.5f;
		texCoord[4] = 0.0f;
		texCoord[5] = 0.0f;
		texCoord[6] = 0.5f;
		texCoord[7] = 0.0f;
	}
	else if (pressed_key == 3)
	{
		texCoord[0] = 2.0f;
		texCoord[1] = 2.0f;
		texCoord[2] = 0.0f;
		texCoord[3] = 2.0f;
		texCoord[4] = 0.0f;
		texCoord[5] = 0.0f;
		texCoord[6] = 2.0f;
		texCoord[7] = 0.0f;
	}
	else if (pressed_key == 4)
	{
		texCoord[0] = 0.5f;
		texCoord[1] = 0.5f;
		texCoord[2] = 0.5f;
		texCoord[3] = 0.5f;
		texCoord[4] = 0.5f;
		texCoord[5] = 0.5f;
		texCoord[6] = 0.5f;
		texCoord[7] = 0.5f;
	}
	else
	{
		//other key pressed and pressed_key = 0 or other than  1, 2, 3, 4
		glUniform1i(gOtherKeyPressedUniform, 1);
	}

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texcoord_Square);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), texCoord, GL_DYNAMIC_DRAW); //new
	glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//unbind vao
	glBindVertexArray(0);

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
	if (gVao_Square)
	{
		glDeleteVertexArrays(1, &gVao_Square);
		gVao_Square = 0;
	}

	//destroy vbo of position
	if (gVbo_Position_Square)
	{
		glDeleteVertexArrays(1, &gVbo_Position_Square);
		gVbo_Position_Square = 0;
	}

	//destroy vbo of texcoord
	if (gVbo_Texcoord_Square)
	{
		glDeleteVertexArrays(1, &gVbo_Texcoord_Square);
		gVbo_Texcoord_Square = 0;
	}

	//destroy vbo of color
	if (gVbo_Color_Square)
	{
		glDeleteVertexArrays(1, &gVbo_Color_Square);
		gVbo_Color_Square = 0;
	}

	//delete texture
	if (smiley_texture)
	{
		glDeleteTextures(1, &smiley_texture);
		smiley_texture = 0;
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
