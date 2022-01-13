//Header files
#include "vmath.h"
#include "Headers.h"
#include "Sphere.h"

#include <windows.h>
#include <stdio.h>
#include <gl\glew.h>
#include <gl\GL.h>

//PRAGMA
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"Sphere.lib")

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

GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

mat4 gPerspectiveProjectionMatrix;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint gNumVertices;
GLuint gNumElements; //Elements a.k.a Indices

bool gbLights = false;

GLuint VKeyIsPressed = 0; //Per-Vertex Lightling Toggle
GLuint FKeyIsPressed = 0; //Per-Fragment Lightling Toggle

//--------------------- PER-VERTEX LIGHTING -------------------------//
GLuint gVertexShaderObject_PV;
GLuint gFragmentShaderObject_PV;
GLuint gShaderProgramObject_PV;

//MATRIX UNIFORMS
GLuint gModelMatrixUniform_PV;
GLuint gViewMatrixUniform_PV;
GLuint gProjectionMatrixUniform_PV;

//LIGHT UNIFORMS
GLuint LaUniform_PV[3]; //ambient light component
GLuint LdUniform_PV[3]; //diffuse light component
GLuint LsUniform_PV[3]; //specular light component
GLuint lightPositionUniform_PV[3]; //light position

//MATERIAL UNIFORMS
GLuint KaUniform_PV; //ambient material component
GLuint KdUniform_PV; //diffuse material component
GLuint KsUniform_PV; //specular material component
GLuint materialShininessUniform_PV; //shininess material

GLuint LKeyPressedUniform_PV; //Light toggle

//--------------------- PER-FRAGMENT LIGHTING -------------------------//
GLuint gVertexShaderObject_PF;
GLuint gFragmentShaderObject_PF;
GLuint gShaderProgramObject_PF;

//MATRIX UNIFORMS
GLuint gModelMatrixUniform_PF;
GLuint gViewMatrixUniform_PF;
GLuint gProjectionMatrixUniform_PF;

//LIGHT UNIFORMS
GLuint LaUniform_PF[3]; //ambient light component
GLuint LdUniform_PF[3]; //diffuse light component
GLuint LsUniform_PF[3]; //specular light component
GLuint lightPositionUniform_PF[3]; //light position

//MATERIAL UNIFORMS
GLuint KaUniform_PF; //ambient material component
GLuint KdUniform_PF; //diffuse material component
GLuint KsUniform_PF; //specular material component
GLuint materialShininessUniform_PF; //shininess material

GLuint LKeyPressedUniform_PF; //Light toggle

//----------------------------------------------//


struct Light
{
	vec4 lightAmbient;
	vec4 lightDiffuse;
	vec4 lightSpecular;
	vec4 lightPosition;
};
struct Light light[3]; //3 Lights

// material properties
GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 128.0f;

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
		TEXT("PP: Assignment_017_Three_Rotating_Lights_On_Sphere : By Kshitij Badkas"),
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
			ToggleFullscreen();
			break;
		default:
			break;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'Q': //QUIT
		case 'q':
			DestroyWindow(hwnd);
			break;

		case 'F': //PER-FRAGMENT SHADING TOGGLE
		case 'f':
			if (FKeyIsPressed == 0)
			{
				FKeyIsPressed = 1;
				VKeyIsPressed = 0;
				fprintf(gpFile, "\nPER_FRAGMENT -> F pressed key = %d", FKeyIsPressed);

			}
			else if (FKeyIsPressed == 1)
			{
				FKeyIsPressed = 0;
				fprintf(gpFile, "\nPER_FRAGMENT -> F pressed key = %d", FKeyIsPressed);
			}
			break;

		case 'V': //PER-VERTEX SHADING TOGGLE
		case 'v':
			if (VKeyIsPressed == 0)
			{
				VKeyIsPressed = 1;
				FKeyIsPressed = 0;
				fprintf(gpFile, "\nPER_VERTEX -> V pressed key = %d", VKeyIsPressed);
			}
			else if (VKeyIsPressed == 1)
			{
				VKeyIsPressed = 0;
				fprintf(gpFile, "\nPER_VERTEX -> V pressed key = %d", VKeyIsPressed);
			}
			break;

		case 'L':
		case 'l':
			if (gbLights == false)
			{
				gbLights = true;
				VKeyIsPressed = 1; //PER-VERTEX SHADING WILL BE ENABLED BY DEFAULT
				FKeyIsPressed = 0;
				fprintf(gpFile, "\n -> gbLights was false - now - L pressed key = %d", gbLights);

			}
			else if (gbLights == true)
			{
				gbLights = false;
				fprintf(gpFile, "\n -> gbLights was true - now - L pressed key = %d", gbLights);
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
	void getPerVertexLighting(void);
	void getPerFragmentLighting(void);


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

	// PER VERTEX LIGHT 
	getPerVertexLighting();

	// PER FRAGMENT LIGHT 
	getPerFragmentLighting();

	//vertices, colours, shader attribs, vbo, vao initializations
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	//Initialize Light attributes
	light[0].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[0].lightDiffuse = vec4(1.0f, 0.0f, 0.0f, 1.0f); //RED
	light[0].lightSpecular = vec4(1.0f, 0.0f, 0.0f, 1.0f);

	light[1].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[1].lightDiffuse = vec4(0.0f, 1.0f, 0.0f, 1.0f); //GREEN
	light[1].lightSpecular = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	light[2].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[2].lightDiffuse = vec4(0.0f, 0.0f, 1.0f, 1.0f); //BLUE
	light[2].lightSpecular = vec4(0.0f, 0.0f, 1.0f, 1.0f);

	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	// position vbo of sphere
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo of sphere
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo of sphere
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao
	glBindVertexArray(0);

	//3D & Depth Code
	glShadeModel(GL_SMOOTH);	//anti-aliasing
	glClearDepth(1.0f);		//Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//Disable culling of back faces (default)
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

void getPerVertexLighting(void)
{
	//code
	//create shader
	gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	//use 'core' profile i.e PROGRAMMABLE PIPELINE

	// -> PER-VERTEX VERTEX SHADER
	const GLchar* vertexShaderSourceCode_PV =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;\n" \
		"in vec3 vNormal;\n" \
		"uniform mat4 u_model_matrix;\n" \
		"uniform mat4 u_view_matrix;\n" \
		"uniform mat4 u_projection_matrix;\n" \
		"uniform int u_L_KEY_PRESSED;\n" \
		"uniform vec3 u_LA[3];\n" \
		"uniform vec3 u_LD[3];\n" \
		"uniform vec3 u_LS[3];\n" \
		"uniform vec4 u_light_position[3];\n" \
		"uniform vec3 u_KA;\n" \
		"uniform vec3 u_KD;\n" \
		"uniform vec3 u_KS;\n" \
		"uniform float u_material_shininess;\n" \
		"out vec3 phong_ads_light;\n" \
		"void main(void)" \
		"{" \
		"if(u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec3 ambient[3];" \
		"vec3 diffuse[3];" \
		"vec3 specular[3];" \
		"vec3 light_direction[3];" \
		"vec3 reflection_vector[3];" \
		"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
		"vec3 view_vector = normalize(-eye_coordinates.xyz);" \
		"for(int i = 0; i < 3; i++)" \
		"{" \
		"light_direction[i] = normalize(vec3(u_light_position[i] - eye_coordinates));" \
		"reflection_vector[i] = reflect(-light_direction[i], transformed_normal);" \
		"ambient[i] = u_LA[i] * u_KA;" \
		"diffuse[i] = u_LD[i] * u_KD * max(dot(light_direction[i], transformed_normal), 0.0f);" \
		"specular[i] = u_LS[i] * u_KS * pow(max(dot(reflection_vector[i], view_vector), 0.0f), u_material_shininess);" \
		"phong_ads_light = phong_ads_light + ambient[i] + diffuse[i] + specular[i];" \
		"}" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";

	glShaderSource(gVertexShaderObject_PV, 1, (const GLchar**)&vertexShaderSourceCode_PV, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject_PV);

	//Shader Compilation Error Checking
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)	//if Shader Compiled Status Failed
	{
		glGetShaderiv(gVertexShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_PV, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "PER-VERTEX Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}


	//create shader
	gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader

	// -> PER-VERTEX FRAGMENT SHADER
	const GLchar* fragmentShaderSourceCode_PV =
		"#version 450 core" \
		"\n" \
		"in vec3 phong_ads_light;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phong_ads_light, 1.0f);" \
		"}";

	glShaderSource(gFragmentShaderObject_PV, 1, (const GLchar**)&fragmentShaderSourceCode_PV, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject_PV);

	//Shader Compilation Error Checking
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(gFragmentShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_PV, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "PER-VERTEX-> Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	//SHADER PROGRAM
	//create
	gShaderProgramObject_PV = glCreateProgram();

	//attach vertex shader object to program 
	glAttachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);

	//attach fragment shader object to program
	glAttachShader(gShaderProgramObject_PV, gFragmentShaderObject_PV);

	//pre-link binding of shader program object with vertex shader attribute
	glBindAttribLocation(gShaderProgramObject_PV, KAB_ATTRIBUTE_POSITION, "vPosition");

	glBindAttribLocation(gShaderProgramObject_PV, KAB_ATTRIBUTE_NORMAL, "vNormal");


	//link shader
	glLinkProgram(gShaderProgramObject_PV);

	//Shader Program Link Error Checking
	//iInfoLogLength = 0;
	//iShaderCompiledStatus = 0;
	//szInfoLog = NULL;
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject_PV, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_PV, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "PER-VERTEX-> Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}


	//get Uniform Location
	gModelMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_model_matrix");
	gViewMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_view_matrix");
	gProjectionMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_projection_matrix");

	LKeyPressedUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_L_KEY_PRESSED");

	LaUniform_PV[0] = glGetUniformLocation(gShaderProgramObject_PV, "u_LA[0]");
	LdUniform_PV[0] = glGetUniformLocation(gShaderProgramObject_PV, "u_LD[0]");
	LsUniform_PV[0] = glGetUniformLocation(gShaderProgramObject_PV, "u_LS[0]");
	lightPositionUniform_PV[0] = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position[0]");

	LaUniform_PV[1] = glGetUniformLocation(gShaderProgramObject_PV, "u_LA[1]");
	LdUniform_PV[1] = glGetUniformLocation(gShaderProgramObject_PV, "u_LD[1]");
	LsUniform_PV[1] = glGetUniformLocation(gShaderProgramObject_PV, "u_LS[1]");
	lightPositionUniform_PV[1] = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position[1]");

	LaUniform_PV[2] = glGetUniformLocation(gShaderProgramObject_PV, "u_LA[2]");
	LdUniform_PV[2] = glGetUniformLocation(gShaderProgramObject_PV, "u_LD[2]");
	LsUniform_PV[2] = glGetUniformLocation(gShaderProgramObject_PV, "u_LS[2]");
	lightPositionUniform_PV[2] = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position[2]");

	KaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KA");
	KdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KD");
	KsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KS");
	materialShininessUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_material_shininess");

}

void getPerFragmentLighting(void)
{
	//code
	//create shader
	gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

	// -> PER-FRAGMENT VERTEX SHADER
	const GLchar* vertexShaderSourceCode_PF =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec4 u_light_position_PF[3];" \
		"out vec3 transformed_normal;" \
		"out vec3 light_direction[3];" \
		"out vec3 view_vector;" \
		"void main(void)" \
		"{" \
		"if(u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
		"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"for(int i = 0; i < 3; i++)" \
		"{" \
		"light_direction[i] = vec3(u_light_position_PF[i] - eye_coordinates);" \
		"}" \
		"view_vector = -eye_coordinates.xyz;" \
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";

	glShaderSource(gVertexShaderObject_PF, 1, (const GLchar**)&vertexShaderSourceCode_PF, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject_PF);

	//Shader Compilation Error Checking
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)	//if Shader Compiled Status Failed
	{
		glGetShaderiv(gVertexShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_PF, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "PER-FRAGMENT Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	//create shader
	gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	// -> PER-FRAGMENT FRAGMENT SHADER
	const GLchar* fragmentShaderSourceCode_PF =
		"#version 450 core" \
		"\n" \
		"in vec3 transformed_normal;" \
		"in vec3 light_direction[3];" \
		"in vec3 view_vector;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec3 u_LA_PF[3];" \
		"uniform vec3 u_LD_PF[3];" \
		"uniform vec3 u_LS_PF[3];" \
		"uniform vec3 u_KA_PF;" \
		"uniform vec3 u_KD_PF;" \
		"uniform vec3 u_KS_PF;" \
		"uniform float u_material_shininess_PF;" \
		"out vec4 FragColor;" \
		"vec3 phong_ads_light_PF;" \
		"void main(void)" \
		"{" \
		"if(u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec3 ambient_PF[3];" \
		"vec3 diffuse_PF[3];" \
		"vec3 specular_PF[3];" \
		"vec3 reflection_vector[3];" \
		"vec3 normalized_transformed_normal = normalize(transformed_normal);" \
		"vec3 normalized_view_vector = normalize(view_vector);" \
		"vec3 normalized_light_direction[3];" \
		"for(int i = 0; i < 3; i++)" \
		"{" \
		"normalized_light_direction[i] = normalize(light_direction[i]);" \
		"ambient_PF[i] = u_LA_PF[i] * u_KA_PF;" \
		"diffuse_PF[i] = u_LD_PF[i] * u_KD_PF * max(dot(normalized_light_direction[i], normalized_transformed_normal), 0.0f);" \
		"reflection_vector[i] = reflect(-normalized_light_direction[i], normalized_transformed_normal);" \
		"specular_PF[i] = u_LS_PF[i] * u_KS_PF * pow(max(dot(reflection_vector[i], normalized_view_vector), 0.0f), u_material_shininess_PF);" \
		"phong_ads_light_PF += ambient_PF[i] + diffuse_PF[i] + specular_PF[i];" \
		"}" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light_PF = vec3(1.0f, 1.0f, 1.0f);" \
		"}" \
		"FragColor = vec4(phong_ads_light_PF, 1.0f);" \
		"}";

	glShaderSource(gFragmentShaderObject_PF, 1, (const GLchar**)&fragmentShaderSourceCode_PF, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject_PF);

	//Shader Compilation Error Checking
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_PF, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "PER-FRAGMENT Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}

	//SHADER PROGRAM
	//create
	gShaderProgramObject_PF = glCreateProgram();

	//attach vertex shader object to program 
	glAttachShader(gShaderProgramObject_PF, gVertexShaderObject_PF);

	//attach fragment shader object to program
	glAttachShader(gShaderProgramObject_PF, gFragmentShaderObject_PF);

	//pre-link binding of shader program object with vertex shader attribute
	glBindAttribLocation(gShaderProgramObject_PF, KAB_ATTRIBUTE_POSITION, "vPosition");

	glBindAttribLocation(gShaderProgramObject_PF, KAB_ATTRIBUTE_NORMAL, "vNormal");


	//link shader
	glLinkProgram(gShaderProgramObject_PF);

	//error-checking
	//GLint iInfoLogLength = 0;
	//GLint iShaderCompiledStatus = 0;
	//char* szInfoLog = NULL;

	//Shader Program Link Error Checking
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject_PF, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_PF, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "PER-FRAGMENT Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}


	//get Uniform Location
	gModelMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_model_matrix");
	gViewMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_view_matrix");
	gProjectionMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_projection_matrix");

	LKeyPressedUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_L_KEY_PRESSED");

	LaUniform_PF[0] = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_PF[0]");
	LdUniform_PF[0] = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_PF[0]");
	LsUniform_PF[0] = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_PF[0]");
	lightPositionUniform_PF[0] = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF[0]");

	LaUniform_PF[1] = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_PF[1]");
	LdUniform_PF[1] = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_PF[1]");
	LsUniform_PF[1] = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_PF[1]");
	lightPositionUniform_PF[1] = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF[1]");

	LaUniform_PF[2] = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_PF[2]");
	LdUniform_PF[2] = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_PF[2]");
	LsUniform_PF[2] = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_PF[2]");
	lightPositionUniform_PF[2] = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF[2]");

	KaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KA_PF");
	KdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KD_PF");
	KsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KS_PF");
	materialShininessUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_material_shininess_PF");

}

//Function Display
void Display(void)
{
	//variables
	static GLfloat lightAngle_Red = 0.0f;
	static GLfloat lightAngle_Green = 0.0f;
	static GLfloat lightAngle_Blue = 0.0f;
	static GLfloat radius = 6.0f;

	//OpenGL Drawing
	mat4 translationMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 projectionMatrix = mat4::identity();

	//code
	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	projectionMatrix = gPerspectiveProjectionMatrix;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject_PV);

	if (VKeyIsPressed == 1)
	{
		if (gbLights == true)
		{
			light[0].lightPosition[0] = 0.0f; //x
			light[0].lightPosition[1] = (GLfloat)(radius * sin(lightAngle_Red)); //y
			light[0].lightPosition[2] = (GLfloat)(radius * cos(lightAngle_Red)); //z
			light[0].lightPosition[3] = 1.0f; //w

			light[1].lightPosition[0] = (GLfloat)(radius * sin(lightAngle_Green)); //x
			light[1].lightPosition[1] = 0.0f; //y
			light[1].lightPosition[2] = (GLfloat)(radius * cos(lightAngle_Green)); //z
			light[1].lightPosition[3] = 1.0f; //w

			light[2].lightPosition[0] = (GLfloat)(radius * sin(lightAngle_Blue)); //x
			light[2].lightPosition[1] = (GLfloat)(radius * cos(lightAngle_Blue)); //y
			light[2].lightPosition[2] = 0.0f; //z
			light[2].lightPosition[3] = 1.0f; //w
			
			glUniform1i(LKeyPressedUniform_PV, 1);

			for (int i = 0; i < 3; i++)
			{
				glUniform3fv(LaUniform_PV[i], 1, light[i].lightAmbient);
				glUniform3fv(LdUniform_PV[i], 1, light[i].lightDiffuse);
				glUniform3fv(LsUniform_PV[i], 1, light[i].lightSpecular);
				glUniform4fv(lightPositionUniform_PV[i], 1, light[i].lightPosition);
			}
			
			glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient);
			glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse);
			glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular);
			glUniform1f(materialShininessUniform_PV, materialShininess);

		}
		else
		{
			glUniform1i(LKeyPressedUniform_PV, 0);
		}

		glUniformMatrix4fv(gModelMatrixUniform_PV, 1, GL_FALSE, translationMatrix);
		glUniformMatrix4fv(gViewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(gProjectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
	}

	else if (FKeyIsPressed == 1)
	{
		glUseProgram(gShaderProgramObject_PF);

		if (gbLights == true)
		{
			light[0].lightPosition[0] = 0.0f; //x
			light[0].lightPosition[1] = (GLfloat)(radius * sin(lightAngle_Red)); //y
			light[0].lightPosition[2] = (GLfloat)(radius * cos(lightAngle_Red)); //z
			light[0].lightPosition[3] = 1.0f; //w

			light[1].lightPosition[0] = (GLfloat)(radius * sin(lightAngle_Green)); //x
			light[1].lightPosition[1] = 0.0f; //y
			light[1].lightPosition[2] = (GLfloat)(radius * cos(lightAngle_Green)); //z
			light[1].lightPosition[3] = 1.0f; //w

			light[2].lightPosition[0] = (GLfloat)(radius * sin(lightAngle_Blue)); //x
			light[2].lightPosition[1] = (GLfloat)(radius * cos(lightAngle_Blue)); //y
			light[2].lightPosition[2] = 0.0f; //z
			light[2].lightPosition[3] = 1.0f; //w

			glUniform1i(LKeyPressedUniform_PF, 1);
			for (int i = 0; i < 3; i++)
			{
				glUniform3fv(LaUniform_PF[i], 1, light[i].lightAmbient);
				glUniform3fv(LdUniform_PF[i], 1, light[i].lightDiffuse);
				glUniform3fv(LsUniform_PF[i], 1, light[i].lightSpecular);
				glUniform4fv(lightPositionUniform_PF[i], 1, light[i].lightPosition);
			}

			glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient);
			glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse);
			glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular);
			glUniform1f(materialShininessUniform_PF, materialShininess);

		}
		else
		{
			glUniform1i(LKeyPressedUniform_PF, 0);
		}

		glUniformMatrix4fv(gModelMatrixUniform_PF, 1, GL_FALSE, translationMatrix);
		glUniformMatrix4fv(gViewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(gProjectionMatrixUniform_PF, 1, GL_FALSE, projectionMatrix);
	}

	else
	{
		glUniformMatrix4fv(gModelMatrixUniform_PV, 1, GL_FALSE, translationMatrix);
		glUniformMatrix4fv(gViewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(gProjectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
	}


	//bind vao - vertex array object
	glBindVertexArray(gVao_sphere);

	//draw
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//unbind vao
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);

	//ANIMATION LOGIC
	lightAngle_Red += 0.005f;
	lightAngle_Green += 0.005f;
	lightAngle_Blue += 0.005f;
	if (lightAngle_Red >= 360.0f)
	{
		lightAngle_Red = 0.0f;
	}
	if (lightAngle_Blue >= 360.0f)
	{
		lightAngle_Blue = 0.0f;
	}
	if (lightAngle_Green >= 360.0f)
	{
		lightAngle_Green = 0.0f;
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

	//destroy vao
	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	//destroy vbo of position
	if (gVbo_sphere_position)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	//destroy vbo of normal
	if (gVbo_sphere_normal)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	//destroy vbo of element
	if (gVbo_sphere_element)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}

	//SAFE SHADER CLEANUP
	if (gShaderProgramObject_PV)
	{
		glUseProgram(gShaderProgramObject_PV);

		GLsizei shaderCount;
		glGetProgramiv(gShaderProgramObject_PV, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShader = NULL;
		pShader = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		if (pShader == NULL)
		{
			//error checking
			fprintf(gpFile, "Error Creating pShader.\n Exitting Now!!\n");
			DestroyWindow(ghwnd);
		}

		glGetAttachedShaders(gShaderProgramObject_PV, shaderCount, &shaderCount, pShader);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject_PV, pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i] = 0;
		}

		free(pShader);

		glDeleteProgram(gShaderProgramObject_PV);
		gShaderProgramObject_PV = 0;

		//unlink
		glUseProgram(0);
	}

	if (gShaderProgramObject_PF)
	{
		glUseProgram(gShaderProgramObject_PF);

		GLsizei shaderCount;
		glGetProgramiv(gShaderProgramObject_PF, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShader = NULL;
		pShader = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		if (pShader == NULL)
		{
			//error checking
			fprintf(gpFile, "Error Creating pShader.\n Exitting Now!!\n");
			DestroyWindow(ghwnd);
		}

		glGetAttachedShaders(gShaderProgramObject_PF, shaderCount, &shaderCount, pShader);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject_PF, pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i] = 0;
		}

		free(pShader);

		glDeleteProgram(gShaderProgramObject_PF);
		gShaderProgramObject_PF = 0;

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

