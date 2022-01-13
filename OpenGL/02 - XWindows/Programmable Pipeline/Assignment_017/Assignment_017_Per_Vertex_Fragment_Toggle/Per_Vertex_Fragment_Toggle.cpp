#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

//PATH -> /usr/include/GL
#include<GL/glew.h>	//FOR GLEW APIs - Programmable Pipeline Shaders
#include<GL/gl.h>	//FOR OPENGL APIs
#include<GL/glx.h>	//FOR GLX APIs

#include "vmath.h" 

#include "Sphere.h"

enum
{
	KAB_ATTRIBUTE_POSITION = 0,
	KAB_ATTRIBUTE_COLOR,
	KAB_ATTRIBUTE_NORMAL,
	KAB_ATTRIBUTE_TEXCOORD
};

//namespaces
using namespace std;
using namespace vmath;

//global variable declaration
bool bFullscreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColorMap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;
FILE* gpFile = NULL;

GLXContext gGlxContext;	//HGLRC

typedef GLXContext (*glXCreateContextAttribsARBProc) (Display *, GLXFBConfig, GLXContext, Bool, const int *);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL; //like int num;

GLXFBConfig gGLXFBConfig; //FB Config - FrameBuffer Configuration

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

bool gbLights = false; //Lighting Toggle
GLuint VKeyIsPressed = 0; //Per-Vertex Lightling Toggle
GLuint FKeyIsPressed = 0; //Per-Fragment Lightling Toggle

//----------------------------------------------//
//PER-VERTEX LIGHTING 
//MODEL-VIEW-PROJECTION UNIFORMS
GLuint gModelMatrixUniform_PV;
GLuint gViewMatrixUniform_PV;
GLuint gProjectionMatrixUniform_PV;

//LIGHT UNIFORMS - ALL ARRAYS
GLuint LaUniform_PV; //ambient light component
GLuint LdUniform_PV; //diffuse light component
GLuint LsUniform_PV; //specular light component
GLuint lightPositionUniform_PV; //light position

//MATERIAL UNIFORMS - Ka, Kd, Ks ARRAYS, shininess is a single value
GLuint KaUniform_PV; //ambient material component
GLuint KdUniform_PV; //diffuse material component
GLuint KsUniform_PV; //specular material component
GLuint materialShininessUniform_PV; //shininess material

//per-vertex shader objects
GLuint gVertexShaderObject_PV;
GLuint gFragmentShaderObject_PV;
GLuint gShaderProgramObject_PV;

GLuint LKeyPressedUniform_PV; //L key is pressed

//----------------------------------------------//

//----------------------------------------------//
//PER-FRAGMENT LIGHTING
//MODEL-VIEW-PROJECTION UNIFORMS
GLuint gModelMatrixUniform_PF;
GLuint gViewMatrixUniform_PF;
GLuint gProjectionMatrixUniform_PF;

//LIGHT UNIFORMS - ALL ARRAYS
GLuint LaUniform_PF; //ambient light component
GLuint LdUniform_PF; //diffuse light component
GLuint LsUniform_PF; //specular light component
GLuint lightPositionUniform_PF; //light position

//MATERIAL UNIFORMS - Ka, Kd, Ks ARRAYS, shininess is a single value
GLuint KaUniform_PF; //ambient material component
GLuint KdUniform_PF; //diffuse material component
GLuint KsUniform_PF; //specular material component
GLuint materialShininessUniform_PF; //shininess material

//per-fragemnt shader objects
GLuint gVertexShaderObject_PF;
GLuint gFragmentShaderObject_PF;
GLuint gShaderProgramObject_PF;

GLuint LKeyPressedUniform_PF; //L key is pressed

//----------------------------------------------//

//main()
int main(void)
{
    //prototypes
    void CreateWindow(void);
    void ToggleFullscreen(void);
    void Uninitialize(void);
    void Initialize(void);
    void Resize(int, int);
    void display(void);
    
    //variable declarations
    int winWidth=giWindowWidth;
    int winHeight=giWindowHeight;
	bool bDone = false;

	//for File IO
	gpFile = fopen("KAB_Log.txt", "w");
	if(gpFile == NULL)
	{
		printf("\n Cannot Create Desired File.");
		exit(-1); //erroneous exit
	}
	
    //code
    CreateWindow();
    
    Initialize();
	
	//////////////////
    //GAME LOOP START
    //////////////////
    XEvent event;
    KeySym keysym;

    while(bDone == false)
    {
    	while(XPending(gpDisplay)) //PeekMessage()
    	{
			XNextEvent(gpDisplay,&event);
			switch (event.type)
			{
			case MapNotify:
				break;
			
			case KeyPress:
				keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
				switch (keysym)
				{
				case XK_Escape:
				    if(bFullscreen==false)
				    {
				        ToggleFullscreen();
				        bFullscreen=true;
				    }
				    else
				    {
				        ToggleFullscreen();
				        bFullscreen=false;
				    }
				    break;
				
				case 'Q': //QUIT
				case 'q':
					bDone = true;

				case 'F' : //PER-FRAGMENT SHADING TOGGLE
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

			case ButtonPress:
				switch (event.xbutton.button)
				{
				case 1:
				    break;
				case 2:
				    break;
				case 3:
				    break;
				default:
				    break;
				}
				break;

			case MotionNotify:
				break;
			
			case ConfigureNotify:	//WM_SIZE
				winWidth=event.xconfigure.width;
				winHeight=event.xconfigure.height;
				Resize(winWidth, winHeight);
				break;
			
			case Expose:
				break;

			case DestroyNotify:
				break;
			
			case 33:
				bDone = true;

			default:
				break;
			}
		
		} // XPending
		
		//CALL display() FUNCTION FOR OPENGL
		display();
		
    } // bDone

    Uninitialize();

    return(0);

}


void CreateWindow(void)
{
    //prototype
    void Uninitialize(void);

    //variable declarations
    XSetWindowAttributes winAttribs;
    int defaultScreen;
    int styleMask;
    
    //for FB Config - lowest level of Data Structure used by X Server graphics driver
    GLXFBConfig *pGLXFBConfig = NULL;
    GLXFBConfig bestGLXFBConfig;
    XVisualInfo *pTempXVisualInfo = NULL;
    int numFBConfig = 0;

	//PixelFormatAttribute
	static int frameBufferAttribute[] = { //NEW - PP
										  GLX_X_RENDERABLE, True,
										  GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
										  GLX_RENDER_TYPE, GLX_RGBA_BIT,
										  GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
										  //same from FFP
										  GLX_DOUBLEBUFFER, True, 
										  GLX_RGBA,
										  GLX_RED_SIZE, 8,
										  GLX_GREEN_SIZE, 8,
										  GLX_BLUE_SIZE, 8,
										  GLX_ALPHA_SIZE, 8,
										  GLX_DEPTH_SIZE, 24,
										  //NEW -PP
										  GLX_STENCIL_SIZE, 8,
										  None //Here we can say 0 as well										  
										};
	//static because it is a convention
	
	gpDisplay=XOpenDisplay(NULL);
    if(gpDisplay==NULL)
    {
        printf("ERROR : Unable to open X Display. \n Exitting Now...\n");
        Uninitialize();
        exit(1);
    }

    defaultScreen=XDefaultScreen(gpDisplay);
	
	////////////////////////////////////////////////////////////////////////////
	
	//Removing glxChooseVisual
	pGLXFBConfig = glXChooseFBConfig(gpDisplay, XDefaultScreen(gpDisplay), frameBufferAttribute, &numFBConfig);
	printf("\nNumber of FB Config = %d \n", numFBConfig);
	if(pGLXFBConfig == NULL)
	{
		printf("ERROR : Unable to create FB Config.\n Exitting Now...\n");
		Uninitialize();
		exit(1);
	}
	
	int bestFrameBufferConfig = -1;
	int worstFrameBufferConfig = -1;
	int bestNumberOfSamples = -1;
	int worstNumberOfSamples = 999;
	
	for(int i = 0; i < numFBConfig; i++)
	{
		pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfig[i]);
		
		if(pTempXVisualInfo)
		{
			int sampleBuffers, samples;
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLES, &samples);
			
			if(bestFrameBufferConfig < 0 || sampleBuffers && samples > bestNumberOfSamples)
			{
				bestFrameBufferConfig = i;
				bestNumberOfSamples = samples;
			}
			if(worstFrameBufferConfig < 0 || !sampleBuffers || samples < worstNumberOfSamples)
			{
				worstFrameBufferConfig = i;
				worstNumberOfSamples = samples;
			}
			
			printf("\n When i = %d, samples = %d, sampleBuffers = %d, visualid = 0x%lu", i, samples, sampleBuffers, pTempXVisualInfo->visualid);
		}
		
		XFree(pTempXVisualInfo);
	} //for
	
	bestGLXFBConfig = pGLXFBConfig[bestFrameBufferConfig];
	gGLXFBConfig = bestGLXFBConfig;
	XFree(pGLXFBConfig);
	gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, bestGLXFBConfig);
	if(gpXVisualInfo==NULL)
    {
        printf("ERROR : Unable to Allocate Memory for Visual Info.\n Exitting Now...\n");
        Uninitialize();
        exit(1);
    }
    
	////////////////////////////////////////////////////////////////////////////
	

    winAttribs.border_pixel=0;
    winAttribs.border_pixmap=0;
    winAttribs.colormap=XCreateColormap(gpDisplay,
                                        RootWindow(gpDisplay,
                                        gpXVisualInfo->screen),
                                        gpXVisualInfo->visual,
                                        AllocNone);

    gColorMap=winAttribs.colormap;

    winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);

    winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask |
    KeyPressMask | PointerMotionMask | StructureNotifyMask;

    styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

    gWindow=XCreateWindow(gpDisplay,
    RootWindow(gpDisplay,gpXVisualInfo->screen),
                0,
                0,
                giWindowWidth,
                giWindowHeight,
                0,
                gpXVisualInfo->depth,
                InputOutput,
                gpXVisualInfo->visual,
                styleMask,
                &winAttribs);

    if(!gWindow)
    {
        printf("ERROR : Failed to Create Main Window.\n Exitting Now...\n");
        Uninitialize();
        exit(1);
    }

    XStoreName(gpDisplay,gWindow,"PP : Assignment_017_Per_Vertex_And_Per_Fragment_Light_Toggle : by Kshitij Badkas");

    Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW", True);
    XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);

    XMapWindow(gpDisplay,gWindow);

}

void ToggleFullscreen(void)
{
    //variables
    Atom wm_state;
    Atom fullscreen;
    XEvent xev={0};

    //code
    wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False);
    memset(&xev,0,sizeof(xev));

    xev.type=ClientMessage;
    xev.xclient.window=gWindow;
    xev.xclient.message_type=wm_state;
    xev.xclient.format=32;
    xev.xclient.data.l[0]=bFullscreen ? 0 : 1;

    fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
    xev.xclient.data.l[1]=fullscreen;

    XSendEvent(gpDisplay,
                RootWindow(gpDisplay,gpXVisualInfo->screen),
                False,
                StructureNotifyMask,
                &xev);

}

//Initialize Function
void Initialize(void)
{
	//prototypes
	void Resize(int, int);	//FOR WARM-UP CALL
	void Uninitialize(void);
	void getPerVertexLighting(void);
	void getPerFragmentLighting(void);
	
	//code
	//////////////////////////////////////////////////////////
	glXCreateContextAttribsARB = 	 		(glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB"); 
	 		
	const int attribs[] = 
	{
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 5,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None					
	};
	
	gGlxContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
	
	if(!gGlxContext)
	{
		const int attribs[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			None
		};
		gGlxContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
		printf("\n FB Config Attributes version = 1.0 \n");
	}
	else
	{
		printf("\n FB Config Attributes version = 4.5 \n");
	}

	Bool bIsDirectContext = glXIsDirect(gpDisplay, gGlxContext);

	if(bIsDirectContext == True)
	{
		printf("\n The Obtained Rendering Context is Hardware Rendering Context.");
	}	
	else
	{
		printf("\n The Obtained Rendering Context is Software Rendering Context.");
	}

	/////////////////////////////////////////////////////////////////////////////////
	
	glXMakeCurrent(gpDisplay, gWindow, gGlxContext);
	
	
	//////////////////////////////////////////////////////////
	//	GLEW INIT CODE HERE	
	//////////////////////////////////////////////////////////
	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK)
	{
		glXDestroyContext(gpDisplay, gGlxContext); 	//Delete Context
		gGlxContext = NULL;
		free(gpXVisualInfo); //Release DC
        gpXVisualInfo=NULL;
	}
	
	//OPENGL RELATED LOG
	fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version : %s\n" ,glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	//OPENGL ENABLED EXTENSIONS
	GLint numExt;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
	for(int i = 0; i < numExt; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
	
	getPerVertexLighting();
	getPerFragmentLighting();
	
	//vertices, colours, vbo, vao
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
	
	/////////////////////////////////////////////////////////////////////////////////
	
	//3D & Depth Code
	glClearDepth(1.0f);    //Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//Disable culling of back faces (default)
	glDisable(GL_CULL_FACE);
	
	glClearColor(0.0f,0.0f,0.0f,1.0f); 
	
	gbLights = false;
	
	//set Identity matrix to identity
	gPerspectiveProjectionMatrix = mat4::identity();
	
	//WARM-UP CALL
	Resize(giWindowWidth, giWindowHeight);
}

void getPerVertexLighting(void)
{
	//prototype
	void Uninitialize(void);
	
	//code
	//VERTEX SHADER FOR PER-VERTEX LIGHTING
	//create shader
	gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	//use 'core' profile i.e PROGRAMMABLE PIPELINE
	const GLchar* vertexShaderSourceCode_PV =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec3 u_LA_PV;" \
		"uniform vec3 u_LD_PV;" \
		"uniform vec3 u_LS_PV;" \
		"uniform vec4 u_light_position_PV;" \
		"uniform vec3 u_KA_PV;" \
		"uniform vec3 u_KD_PV;" \
		"uniform vec3 u_KS_PV;" \
		"uniform float u_material_shininess_PV;" \
		"out vec3 phong_ads_light_PV;" \
		"void main(void)" \
		"{" \
		"if(u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
		"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
		"vec3 light_direction = normalize(vec3(u_light_position_PV - eye_coordinates));" \
		"vec3 reflection_vector = reflect(-light_direction, transformed_normal);" \
		//SWIZZLING
		"vec3 view_vector = normalize(-eye_coordinates.xyz);" \
		"vec3 ambient_PV = u_LA_PV * u_KA_PV;" \
		"vec3 diffuse_PV = u_LD_PV * u_KD_PV * max(dot(light_direction, transformed_normal), 0.0f);" \
		"vec3 specular_PV = u_LS_PV * u_KS_PV * pow(max(dot(reflection_vector, view_vector), 0.0f), u_material_shininess_PV);" \
		"phong_ads_light_PV = ambient_PV + diffuse_PV + specular_PV;" \
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
				fprintf(gpFile, "PER-VERTEX-> Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				Uninitialize();
				exit(1);
			}
		}
	}
	
	
	//FRAGMENT SHADER FOR PER-VERTEX LIGHTING
	//create shader
	gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
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
				Uninitialize();
				exit(1);
			}
		}
	}
	
	//SHADER PROGRAM FOR PER-VERTEX LIGHTING
	//create
	gShaderProgramObject_PV = glCreateProgram();

	//attach vertex shader object to program 
	glAttachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);

	//attach fragment shader object to program
	glAttachShader(gShaderProgramObject_PV, gFragmentShaderObject_PV);

	//pre-link binding of shader program object with vertex shader attribute
	glBindAttribLocation(gShaderProgramObject_PV, KAB_ATTRIBUTE_POSITION, "vPosition");

	//pre-link binding of shader program object with fragment shader attribute
	glBindAttribLocation(gShaderProgramObject_PV, KAB_ATTRIBUTE_NORMAL, "vNormal");

	//link shader
	glLinkProgram(gShaderProgramObject_PV);

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
				fprintf(gpFile, "PER-VERTEX-> Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				Uninitialize();
				exit(1);
			}
		}
	}
	
	//get Uniform Location 
	gModelMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_model_matrix");
	gViewMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_view_matrix");
	gProjectionMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_projection_matrix");
	LKeyPressedUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_L_KEY_PRESSED");
	//for PER-VERTEX LIGHTING
	LaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LA_PV");
	LdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LD_PV");
	LsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LS_PV");
	lightPositionUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_PV");
	KaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KA_PV");
	KdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KD_PV");
	KsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KS_PV");
	materialShininessUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_material_shininess_PV");
	
}

void getPerFragmentLighting(void)
{
	//prototype
	void Uninitialize(void);
	
	//code
	//VERTEX SHADER
	//create shader
	gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	//use 'core' profile i.e PROGRAMMABLE PIPELINE
	const GLchar* vertexShaderSourceCode_PF =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec4 u_light_position_PF;" \
		"out vec3 transformed_normal;" \
		"out vec3 light_direction;" \
		"out vec3 view_vector;" \
		"void main(void)" \
		"{" \
		"if(u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
		"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"light_direction = vec3(u_light_position_PF - eye_coordinates);" \
		//SWIZZLING
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
				fprintf(gpFile, "PER-FRAGMENT-> Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				Uninitialize();
				exit(1);
			}
		}
	}
	
	//FRAGMENT SHADER
	//create shader
	gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar* fragmentShaderSourceCode_PF =
		"#version 450 core" \
		"\n" \
		"in vec3 transformed_normal;" \
		"in vec3 light_direction;" \
		"in vec3 view_vector;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec3 u_LA_PF;" \
		"uniform vec3 u_LD_PF;" \
		"uniform vec3 u_LS_PF;" \
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
		"vec3 normalized_light_direction = normalize(light_direction);" \
		"vec3 normalized_view_vector = normalize(view_vector);" \
		"vec3 ambient_PF = u_LA_PF * u_KA_PF;" \
		"vec3 diffuse_PF = u_LD_PF * u_KD_PF * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);" \
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);" \
		"vec3 specular_PF = u_LS_PF * u_KS_PF * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_material_shininess_PF);" \
		"phong_ads_light_PF = ambient_PF + diffuse_PF + specular_PF;" \
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
				fprintf(gpFile, "PER-FRAGMENT-> Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				Uninitialize();
				exit(1);
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

	//pre-link binding of shader program object with fragment shader attribute
	glBindAttribLocation(gShaderProgramObject_PF, KAB_ATTRIBUTE_NORMAL, "vNormal");

	//link shader
	glLinkProgram(gShaderProgramObject_PF);

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
				fprintf(gpFile, "PER-FRAGMENT-> Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				Uninitialize();
				exit(1);
			}
		}
	}
	
	//get Uniform Location for PER-FRAGMENT LIGHTING
	gModelMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_model_matrix");
	gViewMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_view_matrix");
	gProjectionMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_projection_matrix");
	//for PER-FRAGMENT LIGHTING
	LKeyPressedUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_L_KEY_PRESSED");
	LaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_PF");
	LdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_PF");
	LsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_PF");
	lightPositionUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF");
	KaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KA_PF");
	KdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KD_PF");
	KsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KS_PF");
	materialShininessUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_material_shininess_PF");
}

void Resize(int width, int height)
{
	if(height == 0)
	{
		height = 1;
	}
	glViewport(0,0,(GLsizei)width, (GLsizei)height);
	
	gPerspectiveProjectionMatrix = vmath::perspective(45.0f, 
										(GLfloat)width / (GLfloat)height, 
										0.1f, 
										100.0f);

}

void display(void)
{
	//variables
	//ALBEDO
	GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f ,1.0f };
	GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f ,1.0f };
	GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f ,1.0f };
	GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

	GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f ,1.0f };
	GLfloat materialDiffuse[] = { 0.5f, 0.2f, 0.7f ,1.0f };
	GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f ,1.0f };
	GLfloat materialShininess = 128.0f;
	

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mat4 translationMatrix = mat4::identity(); //model
	
	//
	mat4 rotationMatrix = mat4::identity();
	//
	
	mat4 viewMatrix = mat4::identity(); //view
	mat4 projectionMatrix = mat4::identity(); //projection

	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	
	//
	static GLfloat rAngle = 0.0f;
	rotationMatrix = rotate(rAngle, 0.0f, 0.0f, 1.0f);
	//
	
	projectionMatrix = gPerspectiveProjectionMatrix;
	
	//
	//translationMatrix = translationMatrix * rotationMatrix;
	rAngle += 1.0f;
	//

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject_PV); //using Per-vertex by default

	//USING PER-VERTEX LIGHITNG
	if (VKeyIsPressed == 1)
	{
		//glUseProgram(gShaderProgramObject_PV);

		glUniform4fv(lightPositionUniform_PV, 1, (GLfloat*)lightPosition);
		if (gbLights == true)
		{
			glUniform1i(LKeyPressedUniform_PV, 1);
			glUniform3fv(LaUniform_PV, 1, (GLfloat*)lightAmbient);
			glUniform3fv(LdUniform_PV, 1, (GLfloat*)lightDiffuse);
			glUniform3fv(LsUniform_PV, 1, (GLfloat*)lightSpecular);
			glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient);
			glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse);
			glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular);
			glUniform1f(materialShininessUniform_PV, materialShininess);
			
		}
		else
		{
			glUniform1i(LKeyPressedUniform_PV, 0);

		}

		//sending MODEL, VIEW, PROJECTION Uniforms separately.
		glUniformMatrix4fv(gModelMatrixUniform_PV, 1, GL_FALSE, translationMatrix); //translation and model are same
		glUniformMatrix4fv(gViewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(gProjectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
	}

	//USING PER-FRAGMENT LIGHITNG
	else if (FKeyIsPressed == 1)
	{
		//start using OpenGL program object
		glUseProgram(gShaderProgramObject_PF);

		glUniform4fv(lightPositionUniform_PF, 1, (GLfloat*)lightPosition);
		if (gbLights == true)
		{
			glUniform1i(LKeyPressedUniform_PF, 1);
			glUniform3fv(LaUniform_PF, 1, (GLfloat*)lightAmbient);
			glUniform3fv(LdUniform_PF, 1, (GLfloat*)lightDiffuse);
			glUniform3fv(LsUniform_PF, 1, (GLfloat*)lightSpecular);
			glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient);
			glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse);
			glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular);
			glUniform1f(materialShininessUniform_PF, materialShininess);
		}
		else
		{
			glUniform1i(LKeyPressedUniform_PF, 0);
		}

		//sending MODEL, VIEW, PROJECTION Uniforms separately.
		glUniformMatrix4fv(gModelMatrixUniform_PF, 1, GL_FALSE, translationMatrix); //translation and model are same
		glUniformMatrix4fv(gViewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(gProjectionMatrixUniform_PF, 1, GL_FALSE, projectionMatrix);

	}

	else
	{
		//PER-VERTEX SHOULD BE ENABLED BY DEFAULT - HENCE, WRTING OUT OF IF-ELSE BLOCK
		//sending MODEL, VIEW, PROJECTION Uniforms separately.
		glUniformMatrix4fv(gModelMatrixUniform_PV, 1, GL_FALSE, translationMatrix); //translation and model are same
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
	
	//Double Buffer
	glXSwapBuffers(gpDisplay, gWindow);
}

void Uninitialize(void)
{
	//code
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
	//PER-VERTEX
	if (gShaderProgramObject_PV)
	{
		glUseProgram(gShaderProgramObject_PV);

		GLsizei shaderCount_PV;
		glGetProgramiv(gShaderProgramObject_PV, GL_ATTACHED_SHADERS, &shaderCount_PV);

		GLuint* pShader_PV = NULL;
		pShader_PV = (GLuint*)malloc(sizeof(GLuint) * shaderCount_PV);

		if (pShader_PV == NULL)
		{
			//error checking
			fprintf(gpFile, "Error Creating pShader.\n Exitting Now!!\n");
			exit(1);
		}

		glGetAttachedShaders(gShaderProgramObject_PV, shaderCount_PV, &shaderCount_PV, pShader_PV);

		for (GLsizei i = 0; i < shaderCount_PV; i++)
		{
			glDetachShader(gShaderProgramObject_PV, pShader_PV[i]);
			glDeleteShader(pShader_PV[i]);
			pShader_PV[i] = 0;
		}

		free(pShader_PV);

		glDeleteProgram(gShaderProgramObject_PV);
		gShaderProgramObject_PV = 0;

		//unlink
		glUseProgram(0);
	}
	////
	//PER-FRAGMENT
	if (gShaderProgramObject_PF)
	{
		glUseProgram(gShaderProgramObject_PF);

		GLsizei shaderCount_PF;
		glGetProgramiv(gShaderProgramObject_PF, GL_ATTACHED_SHADERS, &shaderCount_PF);

		GLuint* pShader_PF = NULL;
		pShader_PF = (GLuint*)malloc(sizeof(GLuint) * shaderCount_PF);

		if (pShader_PF == NULL)
		{
			//error checking
			fprintf(gpFile, "Error Creating pShader.\n Exitting Now!!\n");
			exit(1);
		}

		glGetAttachedShaders(gShaderProgramObject_PF, shaderCount_PF, &shaderCount_PF, pShader_PF);

		for (GLsizei i = 0; i < shaderCount_PF; i++)
		{
			glDetachShader(gShaderProgramObject_PF, pShader_PF[i]);
			glDeleteShader(pShader_PF[i]);
			pShader_PF[i] = 0;
		}

		free(pShader_PF);

		glDeleteProgram(gShaderProgramObject_PF);
		gShaderProgramObject_PF = 0;

		//unlink
		glUseProgram(0);
	}
	
	GLXContext currentGlxContext;
	currentGlxContext = glXGetCurrentContext();
	
	if(currentGlxContext == gGlxContext)
	{
		glXMakeCurrent(gpDisplay, 0, 0);
	}
	
	if(gGlxContext)
	{
		glXDestroyContext(gpDisplay, gGlxContext);
	}
	
    if(gWindow)
    {
        XDestroyWindow(gpDisplay,gWindow);
    }

    if(gColorMap)
    {
        XFreeColormap(gpDisplay,gColorMap);
    }

    if(gpXVisualInfo)
    {
        free(gpXVisualInfo);
        gpXVisualInfo=NULL;
    }

    if(gpDisplay)
    {
        XCloseDisplay(gpDisplay);
        gpDisplay=NULL;
    }
    
    //close file
    if(gpFile)
    {
    	fclose(gpFile);
    	gpFile = NULL;
    }
}

