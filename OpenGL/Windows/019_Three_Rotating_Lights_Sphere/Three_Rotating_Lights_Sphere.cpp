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

//----------------------------------------------//
//PER-VERTEX LIGHTING 
GLuint gVertexShaderObject_PV;
GLuint gFragmentShaderObject_PV;
GLuint gShaderProgramObject_PV;

//MATRIX UNIFORMS
GLuint gModelMatrixUniform_PV;
GLuint gViewMatrixUniform_PV;
GLuint gProjectionMatrixUniform_PV;

//LIGHT UNIFORMS
GLuint LaUniform_Red_PV; //ambient light component
GLuint LdUniform_Red_PV; //diffuse light component
GLuint LsUniform_Red_PV; //specular light component
GLuint lightPositionUniform_Red_PV; //light position

GLuint LaUniform_Green_PV; //ambient light component
GLuint LdUniform_Green_PV; //diffuse light component
GLuint LsUniform_Green_PV; //specular light component
GLuint lightPositionUniform_Green_PV; //light position

GLuint LaUniform_Blue_PV; //ambient light component
GLuint LdUniform_Blue_PV; //diffuse light component
GLuint LsUniform_Blue_PV; //specular light component
GLuint lightPositionUniform_Blue_PV; //light position

//MATERIAL UNIFORMS
GLuint KaUniform_PV; //ambient material component
GLuint KdUniform_PV; //diffuse material component
GLuint KsUniform_PV; //specular material component
GLuint materialShininessUniform_PV; //shininess material

GLuint LKeyPressedUniform_PV; //Light toggle

//----------------------------------------------//
//PER-FRAGMENT LIGHTING
GLuint gVertexShaderObject_PF;
GLuint gFragmentShaderObject_PF;
GLuint gShaderProgramObject_PF;

//MATRIX UNIFORMS
GLuint gModelMatrixUniform_PF;
GLuint gViewMatrixUniform_PF;
GLuint gProjectionMatrixUniform_PF;

//LIGHT UNIFORMS
GLuint LaUniform_Red_PF; //ambient light component
GLuint LdUniform_Red_PF; //diffuse light component
GLuint LsUniform_Red_PF; //specular light component
GLuint lightPositionUniform_Red_PF; //light position

GLuint LaUniform_Green_PF; //ambient light component
GLuint LdUniform_Green_PF; //diffuse light component
GLuint LsUniform_Green_PF; //specular light component
GLuint lightPositionUniform_Green_PF; //light position

GLuint LaUniform_Blue_PF; //ambient light component
GLuint LdUniform_Blue_PF; //diffuse light component
GLuint LsUniform_Blue_PF; //specular light component
GLuint lightPositionUniform_Blue_PF; //light position

//MATERIAL UNIFORMS
GLuint KaUniform_PF; //ambient material component
GLuint KdUniform_PF; //diffuse material component
GLuint KsUniform_PF; //specular material component
GLuint materialShininessUniform_PF; //shininess material

GLuint LKeyPressedUniform_PF; //Light toggle

//----------------------------------------------//

//light and material properties
GLfloat lightAmbient_Red[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse_Red[] = { 1.0f, 0.0f, 0.0f, 1.0f }; //RED
GLfloat lightSpecular_Red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightPosition_Red[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLfloat lightAmbient_Green[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse_Green[] = { 0.0f, 1.0f, 0.0f, 1.0f }; //BLUE
GLfloat lightSpecular_Green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat lightPosition_Green[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLfloat lightAmbient_Blue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse_Blue[] = { 0.0f, 0.0f, 1.0f, 1.0f }; //GREEN
GLfloat lightSpecular_Blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat lightPosition_Blue[] = { 0.0f, 0.0f, 0.0f, 1.0f };

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
			}
			else if (FKeyIsPressed == 1)
			{
				FKeyIsPressed = 0;
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

	void getVertexShader_PV(void);
	void getFragmentShader_PV(void);
	void getUniformLocation_PV(void);
	void getShaderProgramObject_PV(void);

	void getVertexShader_PF(void);
	void getFragmentShader_PF(void);
	void getShaderProgramObject_PF(void);	
	void getUniformLocation_PF(void);


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


	getVertexShader_PV();
	getFragmentShader_PV();
	getShaderProgramObject_PV();
	getUniformLocation_PV();

	getVertexShader_PF();
	getFragmentShader_PF();
	getShaderProgramObject_PF();
	getUniformLocation_PF();


	//vertices, colours, shader attribs, vbo, vao initializations
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

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

void getVertexShader_PV(void)
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
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec3 u_LA_Red_PV;" \
		"uniform vec3 u_LD_Red_PV;" \
		"uniform vec3 u_LS_Red_PV;" \
		"uniform vec4 u_light_position_Red_PV;" \
		"uniform vec3 u_LA_Green_PV;" \
		"uniform vec3 u_LD_Green_PV;" \
		"uniform vec3 u_LS_Green_PV;" \
		"uniform vec4 u_light_position_Green_PV;" \
		"uniform vec3 u_LA_Blue_PV;" \
		"uniform vec3 u_LD_Blue_PV;" \
		"uniform vec3 u_LS_Blue_PV;" \
		"uniform vec4 u_light_position_Blue_PV;" \
		"uniform vec3 u_KA_PV;" \
		"uniform vec3 u_KD_PV;" \
		"uniform vec3 u_KS_PV;" \
		"uniform float u_material_shininess_PV;" \
		"out vec3 phong_ads_light_PV;" \
		"void main(void)" \
		"{" \
		"if (u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec3 ambient_Red_PV;" \
		"vec3 diffuse_Red_PV;" \
		"vec3 specular_Red_PV;" \
		"vec3 light_direction_Red;" \
		"vec3 reflection_vector_Red;" \

		"vec3 ambient_Green_PV;" \
		"vec3 diffuse_Green_PV;" \
		"vec3 specular_Green_PV;" \
		"vec3 light_direction_Green;" \
		"vec3 reflection_vector_Green;" \

		"vec3 ambient_Blue_PV;" \
		"vec3 diffuse_Blue_PV;" \
		"vec3 specular_Blue_PV;" \
		"vec3 light_direction_Blue;" \
		"vec3 reflection_vector_Blue;" \

		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
		"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
		"vec3 view_vector = normalize(-eye_coordinates.xyz);" \

		"light_direction_Red = normalize(vec3(u_light_position_Red_PV - eye_coordinates));" \
		"reflection_vector_Red = reflect(-light_direction_Red, transformed_normal);" \
		"ambient_Red_PV = u_LA_Red_PV * u_KA_PV;" \
		"diffuse_Red_PV = u_LD_Red_PV * u_KD_PV * max(dot(light_direction_Red, transformed_normal), 0.0f);" \
		"specular_Red_PV = u_LS_Red_PV * u_KS_PV * pow(max(dot(reflection_vector_Red, view_vector), 0.0f), u_material_shininess_PV);" \

		"light_direction_Green = normalize(vec3(u_light_position_Green_PV - eye_coordinates));" \
		"reflection_vector_Green = reflect(-light_direction_Green, transformed_normal);" \
		"ambient_Green_PV = u_LA_Green_PV * u_KA_PV;" \
		"diffuse_Green_PV = u_LD_Green_PV * u_KD_PV * max(dot(light_direction_Green, transformed_normal), 0.0f);" \
		"specular_Green_PV = u_LS_Green_PV * u_KS_PV * pow(max(dot(reflection_vector_Green, view_vector), 0.0f), u_material_shininess_PV);" \

		"light_direction_Blue = normalize(vec3(u_light_position_Blue_PV - eye_coordinates));" \
		"reflection_vector_Blue = reflect(-light_direction_Blue, transformed_normal);" \
		"ambient_Blue_PV = u_LA_Blue_PV * u_KA_PV;" \
		"diffuse_Blue_PV = u_LD_Blue_PV * u_KD_PV * max(dot(light_direction_Blue, transformed_normal), 0.0f);" \
		"specular_Blue_PV = u_LS_Blue_PV * u_KS_PV * pow(max(dot(reflection_vector_Blue, view_vector), 0.0f), u_material_shininess_PV);" \

		"phong_ads_light_PV = ambient_Red_PV + diffuse_Red_PV + specular_Red_PV" \
		" + ambient_Green_PV + diffuse_Green_PV + specular_Green_PV" \
		" + ambient_Blue_PV + diffuse_Blue_PV + specular_Blue_PV;" \
		//"phong_ads_light_PV = phong_ads_light_PV + ambient_Green_PV + diffuse_Green_PV + specular_Green_PV;" \
		//"phong_ads_light_PV = phong_ads_light_PV + ambient_Blue_PV + diffuse_Blue_PV + specular_Blue_PV;" \
		
		"}" \
		"else" \
		"{" \
		"phong_ads_light_PV = vec3(1.0f, 1.0f, 1.0f);" \
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

}

void getFragmentShader_PV(void)
{
	//code
	//create shader
	gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader

	// -> PER-FRAGMENT VERTEX SHADER
	const GLchar* fragmentShaderSourceCode_PV =
		"#version 450 core" \
		"\n" \
		"in vec3 phong_ads_light_PV;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phong_ads_light_PV, 1.0f);" \
		"}";

	glShaderSource(gFragmentShaderObject_PV, 1, (const GLchar**)&fragmentShaderSourceCode_PV, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject_PV);

	//Shader Compilation Error Checking
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

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
}

void getShaderProgramObject_PV(void)
{
	//code
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

	//error-checking
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	//Shader Program Link Error Checking
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
				fprintf(gpFile, "PER-VERTEX Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				DestroyWindow(ghwnd);
			}
		}
	}
}

void getUniformLocation_PV(void)
{
	//code
	//get Uniform Location
	gModelMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_model_matrix");
	gViewMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_view_matrix");
	gProjectionMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_projection_matrix");

	LKeyPressedUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_L_KEY_PRESSED");

	LaUniform_Red_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LA_Red_PV");
	LdUniform_Red_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LD_Red_PV");
	LsUniform_Red_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LS_Red_PV");
	lightPositionUniform_Red_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_Red_PV");

	LaUniform_Green_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LA_Green_PV");
	LdUniform_Green_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LD_Green_PV");
	LsUniform_Green_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LS_Green_PV");
	lightPositionUniform_Green_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_Green_PV");

	LaUniform_Blue_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LA_Blue_PV");
	LdUniform_Blue_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LD_Blue_PV");
	LsUniform_Blue_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LS_Blue_PV");
	lightPositionUniform_Blue_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_Blue_PV");

	KaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KA_PV");
	KdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KD_PV");
	KsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KS_PV");
	materialShininessUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_material_shininess_PV");
}

void getVertexShader_PF(void)
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
		"uniform vec4 u_light_position_Red_PF;" \
		"uniform vec4 u_light_position_Green_PF;" \
		"uniform vec4 u_light_position_Blue_PF;" \
		"out vec3 transformed_normal;" \
		"out vec3 light_direction_Red;" \
		"out vec3 light_direction_Green;" \
		"out vec3 light_direction_Blue;" \
		"out vec3 view_vector;" \
		"void main(void)" \
		"{" \
		"if(u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
		"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"light_direction_Red = vec3(u_light_position_Red_PF - eye_coordinates);" \
		"light_direction_Green = vec3(u_light_position_Green_PF - eye_coordinates);" \
		"light_direction_Blue = vec3(u_light_position_Blue_PF - eye_coordinates);" \
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
}

void getFragmentShader_PF(void)
{
	//code
	//code
	//create shader
	gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSourceCode_PF =
		"#version 450 core" \
		"\n" \
		"in vec3 transformed_normal;" \
		"in vec3 light_direction_Red;" \
		"in vec3 light_direction_Green;" \
		"in vec3 light_direction_Blue;" \
		"in vec3 view_vector;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec3 u_LA_Red_PF;" \
		"uniform vec3 u_LD_Red_PF;" \
		"uniform vec3 u_LS_Red_PF;" \
		"uniform vec3 u_LA_Green_PF;" \
		"uniform vec3 u_LD_Green_PF;" \
		"uniform vec3 u_LS_Green_PF;" \
		"uniform vec3 u_LA_Blue_PF;" \
		"uniform vec3 u_LD_Blue_PF;" \
		"uniform vec3 u_LS_Blue_PF;" \
		"uniform vec3 u_KA_PF;" \
		"uniform vec3 u_KD_PF;" \
		"uniform vec3 u_KS_PF;" \
		"uniform float u_material_shininess_PF;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_light_PF;" \
		"if(u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec3 normalized_transformed_normal = normalize(transformed_normal);" \
		"vec3 normalized_light_direction_Red = normalize(light_direction_Red);" \
		"vec3 normalized_light_direction_Green = normalize(light_direction_Green);" \
		"vec3 normalized_light_direction_Blue = normalize(light_direction_Blue);" \
		"vec3 normalized_view_vector = normalize(view_vector);" \
		
		"vec3 ambient_Red_PF = u_LA_Red_PF * u_KA_PF;" \
		"vec3 diffuse_Red_PF = u_LD_Red_PF * u_KD_PF * max(dot(normalized_light_direction_Red, normalized_transformed_normal), 0.0f);" \
		"vec3 reflection_vector_Red = reflect(-normalized_light_direction_Red, normalized_transformed_normal);" \
		"vec3 specular_Red_PF = u_LS_Red_PF * u_KS_PF * pow(max(dot(reflection_vector_Red, normalized_view_vector), 0.0f), u_material_shininess_PF);" \

		"vec3 ambient_Green_PF = u_LA_Green_PF * u_KA_PF;" \
		"vec3 diffuse_Green_PF = u_LD_Green_PF * u_KD_PF * max(dot(normalized_light_direction_Green, normalized_transformed_normal), 0.0f);" \
		"vec3 reflection_vector_Green = reflect(-normalized_light_direction_Green, normalized_transformed_normal);" \
		"vec3 specular_Green_PF = u_LS_Green_PF * u_KS_PF * pow(max(dot(reflection_vector_Green, normalized_view_vector), 0.0f), u_material_shininess_PF);" \

		"vec3 ambient_Blue_PF = u_LA_Blue_PF * u_KA_PF;" \
		"vec3 diffuse_Blue_PF = u_LD_Blue_PF * u_KD_PF * max(dot(normalized_light_direction_Blue, normalized_transformed_normal), 0.0f);" \
		"vec3 reflection_vector_Blue = reflect(-normalized_light_direction_Blue, normalized_transformed_normal);" \
		"vec3 specular_Blue_PF = u_LS_Blue_PF * u_KS_PF * pow(max(dot(reflection_vector_Blue, normalized_view_vector), 0.0f), u_material_shininess_PF);" \

		"phong_ads_light_PF = ambient_Red_PF + diffuse_Red_PF + specular_Red_PF;" \
		"phong_ads_light_PF = phong_ads_light_PF + ambient_Green_PF + diffuse_Green_PF + specular_Green_PF;" \
		"phong_ads_light_PF = phong_ads_light_PF + ambient_Blue_PF + diffuse_Blue_PF + specular_Blue_PF;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light_PF = vec3(1.0f,1.0f,1.0f);" \
		"}" \
		"FragColor = vec4(phong_ads_light_PF, 1.0f);" \
		"}";

	glShaderSource(gFragmentShaderObject_PF, 1, (const GLchar**)&fragmentShaderSourceCode_PF, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject_PF);

	//Shader Compilation Error Checking
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

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
}

void getShaderProgramObject_PF(void)
{
	//code
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
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

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
}

void getUniformLocation_PF(void)
{
	//code
	//get Uniform Location
	gModelMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_model_matrix");
	gViewMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_view_matrix");
	gProjectionMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_projection_matrix");

	LKeyPressedUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_L_KEY_PRESSED");

	LaUniform_Red_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_Red_PF");
	LdUniform_Red_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_Red_PF");
	LsUniform_Red_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_Red_PF");
	lightPositionUniform_Red_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_Red_PF");

	LaUniform_Green_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_Green_PF");
	LdUniform_Green_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_Green_PF");
	LsUniform_Green_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_Green_PF");
	lightPositionUniform_Green_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_Green_PF");

	LaUniform_Blue_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_Blue_PF");
	LdUniform_Blue_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_Blue_PF");
	LsUniform_Blue_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_Blue_PF");
	lightPositionUniform_Blue_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_Blue_PF");

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
			lightPosition_Red[0] = 0.0f; //x
			lightPosition_Red[1] = (GLfloat)(radius * sin(lightAngle_Red)); //y
			lightPosition_Red[2] = (GLfloat)(radius * cos(lightAngle_Red)); //z
			lightPosition_Red[3] = 1.0f; //w

			lightPosition_Green[0] = (GLfloat)(radius * sin(lightAngle_Green)); //x
			lightPosition_Green[1] = 0.0f; //y
			lightPosition_Green[2] = (GLfloat)(radius * cos(lightAngle_Green)); //z
			lightPosition_Green[3] = 1.0f; //w

			lightPosition_Blue[0] = (GLfloat)(radius * sin(lightAngle_Blue)); //x
			lightPosition_Blue[1] = (GLfloat)(radius * cos(lightAngle_Blue)); //y
			lightPosition_Blue[2] = 0.0f; //z
			lightPosition_Blue[3] = 1.0f; //w

			glUniform1i(LKeyPressedUniform_PV, 1);
			glUniform3fv(LaUniform_Red_PV, 1, (GLfloat*)lightAmbient_Red);
			glUniform3fv(LdUniform_Red_PV, 1, (GLfloat*)lightDiffuse_Red);
			glUniform3fv(LsUniform_Red_PV, 1, (GLfloat*)lightSpecular_Red);
			glUniform4fv(lightPositionUniform_Red_PV, 1, (GLfloat*)lightPosition_Red);

			glUniform3fv(LaUniform_Green_PV, 1, (GLfloat*)lightAmbient_Green);
			glUniform3fv(LdUniform_Green_PV, 1, (GLfloat*)lightDiffuse_Green);
			glUniform3fv(LsUniform_Green_PV, 1, (GLfloat*)lightSpecular_Green);
			glUniform4fv(lightPositionUniform_Green_PV, 1, (GLfloat*)lightPosition_Green);

			glUniform3fv(LaUniform_Blue_PV, 1, (GLfloat*)lightAmbient_Blue);
			glUniform3fv(LdUniform_Blue_PV, 1, (GLfloat*)lightDiffuse_Blue);
			glUniform3fv(LsUniform_Blue_PV, 1, (GLfloat*)lightSpecular_Blue);
			glUniform4fv(lightPositionUniform_Blue_PV, 1, (GLfloat*)lightPosition_Blue);

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
			lightPosition_Red[0] = 0.0f; //x
			lightPosition_Red[1] = (GLfloat)(radius * sin(lightAngle_Red)); //y
			lightPosition_Red[2] = (GLfloat)(radius * cos(lightAngle_Red)); //z
			lightPosition_Red[3] = 1.0f; //w

			lightPosition_Green[0] = (GLfloat)(radius * sin(lightAngle_Green)); //x
			lightPosition_Green[1] = 0.0f; //y
			lightPosition_Green[2] = (GLfloat)(radius * cos(lightAngle_Green)); //z
			lightPosition_Green[3] = 1.0f; //w

			lightPosition_Blue[0] = (GLfloat)(radius * sin(lightAngle_Blue)); //x
			lightPosition_Blue[1] = (GLfloat)(radius * cos(lightAngle_Blue)); //y
			lightPosition_Blue[2] = 0.0f; //z
			lightPosition_Blue[3] = 1.0f; //w

			glUniform1i(LKeyPressedUniform_PF, 1);
			glUniform3fv(LaUniform_Red_PF, 1, (GLfloat*)lightAmbient_Red);
			glUniform3fv(LdUniform_Red_PF, 1, (GLfloat*)lightDiffuse_Red);
			glUniform3fv(LsUniform_Red_PF, 1, (GLfloat*)lightSpecular_Red);
			glUniform4fv(lightPositionUniform_Red_PF, 1, (GLfloat*)lightPosition_Red);

			glUniform3fv(LaUniform_Green_PF, 1, (GLfloat*)lightAmbient_Green);
			glUniform3fv(LdUniform_Green_PF, 1, (GLfloat*)lightDiffuse_Green);
			glUniform3fv(LsUniform_Green_PF, 1, (GLfloat*)lightSpecular_Green);
			glUniform4fv(lightPositionUniform_Green_PF, 1, (GLfloat*)lightPosition_Green);

			glUniform3fv(LaUniform_Blue_PF, 1, (GLfloat*)lightAmbient_Blue);
			glUniform3fv(LdUniform_Blue_PF, 1, (GLfloat*)lightDiffuse_Blue);
			glUniform3fv(LsUniform_Blue_PF, 1, (GLfloat*)lightSpecular_Blue);
			glUniform4fv(lightPositionUniform_Blue_PF, 1, (GLfloat*)lightPosition_Blue);

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

