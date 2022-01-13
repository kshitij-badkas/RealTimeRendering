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

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

mat4 gPerspectiveProjectionMatrix;

//pyramid Vao and Vbo
GLuint gVao_Pyramid;
GLuint gVbo_Position_Pyramid;
GLuint gVbo_Normal_Pyramid;

//MATRIX UNIFORMS
GLuint gModelMatrixUniform;
GLuint gViewMatrixUniform;
GLuint gProjectionMatrixUniform;

//LIGHT UNIFORMS
GLuint LaUniform[2]; //ambient light component
GLuint LdUniform[2]; //diffuse light component
GLuint LsUniform[2]; //specular light component
GLuint lightPositionUniform[2]; //light position

//MATERIAL UNIFORMS
GLuint KaUniform; //ambient material component
GLuint KdUniform; //diffuse material component
GLuint KsUniform; //specular material component
GLuint materialShininessUniform; //shininess material

GLuint LKeyPressedUniform;

//DECLARE ARRAY OF STRUCTURE
struct Light
{
	vec4 lightAmbient;
	vec4 lightDiffuse;
	vec4 lightSpecular;
	vec4 lightPosition;
};
struct Light light[2];

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 50.0f;

bool gbLights = false;


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
				    bDone = true;
				
				case XK_F:
				case XK_f:
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
				    
				case 'L':
				case 'l':
					if (gbLights == false)
					{
						gbLights = true;
					}
					else if (gbLights == true)
					{
						gbLights = false;
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

    XStoreName(gpDisplay,gWindow,"PP : Assignment_018_Two_Steady_Lights_On_Pyramid : by Kshitij Badkas");

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
		printf("The Obtained Rendering Context is Hardware Rendering Context.\n");
	}	
	else
	{
		printf("The Obtained Rendering Context is Software Rendering Context.\n");
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
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec3 u_LA[2];" \
		"uniform vec3 u_LD[2];" \
		"uniform vec3 u_LS[2];" \
		"uniform vec4 u_light_position[2];" \
		"uniform vec3 u_KA;" \
		"uniform vec3 u_KD;" \
		"uniform vec3 u_KS;" \
		"uniform float u_material_shininess;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
		"if(u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec3 ambient[2];" \
		"vec3 diffuse[2];" \
		"vec3 specular[2];" \
		"vec3 light_direction[2];" \
		"vec3 reflection_vector[2];" \
		"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
		"vec3 view_vector = normalize(-eye_coordinates.xyz);" \
		"vec3 temp;" \
		"for(int i = 0; i < 2; i++)" \
		"{" \
		"light_direction[i] = normalize(vec3(u_light_position[i] - eye_coordinates));" \
		"reflection_vector[i] = reflect(-light_direction[i], transformed_normal);" \
		"ambient[i] = u_LA[i] * u_KA;" \
		"diffuse[i] = u_LD[i] * u_KD * max(dot(light_direction[i], transformed_normal), 0.0f);" \
		"specular[i] = u_LS[i] * u_KS * pow(max(dot(reflection_vector[i], view_vector), 0.0f), u_material_shininess);" \
		"phong_ads_light = ambient[i] + diffuse[i] + specular[i];" \
		"}" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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
				Uninitialize();
				exit(1);
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
		"in vec3 phong_ads_light;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phong_ads_light, 1.0f);" \
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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
			}
		}
	}


	//get Uniform Location 
	gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_L_KEY_PRESSED");
	
	LaUniform[0] = glGetUniformLocation(gShaderProgramObject, "u_LA[0]");
	LdUniform[0] = glGetUniformLocation(gShaderProgramObject, "u_LD[0]");
	LsUniform[0] = glGetUniformLocation(gShaderProgramObject, "u_LS[0]");
	lightPositionUniform[0] = glGetUniformLocation(gShaderProgramObject, "u_light_position[0]");
	
	LaUniform[1] = glGetUniformLocation(gShaderProgramObject, "u_LA[1]");
	LdUniform[1] = glGetUniformLocation(gShaderProgramObject, "u_LD[1]");
	LsUniform[1] = glGetUniformLocation(gShaderProgramObject, "u_LS[1]");
	lightPositionUniform[1] = glGetUniformLocation(gShaderProgramObject, "u_light_position[1]");

	KaUniform = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
	
	//vertices, colours, shader attribs, vbo, vao initializations
	const GLfloat pyramidVertices[] =
	{
		//FRONT
		0.0f, 1.0f, 0.0f, //apex
		-1.0f, -1.0f, 1.0f, //left-bottom
		1.0f, -1.0f, 1.0f, //right-bottom
		//RIGHT
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		//BACK
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		//LEFT
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f
	};

	const GLfloat pyramidNormals[] =
	{
		0.0f, 0.447214f, 0.894427f,// front-top
		0.0f, 0.447214f, 0.894427f,// front-left
		0.0f, 0.447214f, 0.894427f,// front-right

		0.894427f, 0.447214f, 0.0f, // right-top
		0.894427f, 0.447214f, 0.0f, // right-left
		0.894427f, 0.447214f, 0.0f, // right-right

		0.0f, 0.447214f, -0.894427f, // back-top
		0.0f, 0.447214f, -0.894427f, // back-left
		0.0f, 0.447214f, -0.894427f, // back-right

		-0.894427f, 0.447214f, 0.0f, // left-top
		-0.894427f, 0.447214f, 0.0f, // left-left
		-0.894427f, 0.447214f, 0.0f // left-right

	};


	//Initialize Light attributes
	light[0].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[0].lightDiffuse = vec4(1.0f, 0.0f, 0.0f, 1.0f); //RED
	light[0].lightSpecular = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light[0].lightPosition = vec4(-2.0f, 0.0f, 0.0f, 1.0f);

	light[1].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[1].lightDiffuse = vec4(0.0f, 0.0f, 1.0f, 1.0f); //BLUE
	light[1].lightSpecular = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light[1].lightPosition = vec4(2.0f, 0.0f, 0.0f, 1.0f);


	//GEN START
	glGenVertexArrays(1, &gVao_Pyramid);
	//BIND START
	glBindVertexArray(gVao_Pyramid);

	//FOR PYRAMID POSITION
	glGenBuffers(1, &gVbo_Position_Pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//FOR PYRAMID NORMAL
	glGenBuffers(1, &gVbo_Normal_Pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Normal_Pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
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

	//set identity matrix to identity matrix
	gPerspectiveProjectionMatrix = mat4::identity();
	
	//WARM-UP CALL
	Resize(giWindowWidth, giWindowHeight);
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
	static GLfloat anglePyramid = 0.0f;

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	if (gbLights == true)
	{
		glUniform1i(LKeyPressedUniform, 1);
		for (int i = 0; i < 2; i++)
		{
			glUniform4fv(lightPositionUniform[i], 1, light[i].lightPosition);
			glUniform3fv(LaUniform[i], 1, light[i].lightAmbient);
			glUniform3fv(LdUniform[i], 1, light[i].lightDiffuse);
			glUniform3fv(LsUniform[i], 1, light[i].lightSpecular);
			//printf("INSIDE glUniform4fv() FOR LOOP : i = %d \n", i);
		}
		glUniform3fv(KaUniform, 1, materialAmbient);
		glUniform3fv(KdUniform, 1, materialDiffuse);
		glUniform3fv(KsUniform, 1, materialSpecular);
		glUniform1f(materialShininessUniform, materialShininess);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	//OpenGL Drawing
	mat4 translationMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 rotationMatrix;

	//PYRAMID
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -6.0f);
	rotationMatrix = rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
	translationMatrix = translationMatrix * rotationMatrix;
	projectionMatrix = gPerspectiveProjectionMatrix;

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, translationMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	//bind vao - vertex array object
	glBindVertexArray(gVao_Pyramid);

	//draw
	glDrawArrays(GL_TRIANGLES, 0, 12); //PYRAMID HAS 4 FACES, HENCE 12 VERTICES

	//unbind vao
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);


	//ANIMATION CODE
	anglePyramid += 1.0f;
	if (anglePyramid >= 360.0f)
	{
		anglePyramid = 0.0f;
	}

	//Double Buffer
	glXSwapBuffers(gpDisplay, gWindow);
	
}

void Uninitialize(void)
{
	//code
	//destroy vao
	if (gVao_Pyramid)
	{
		glDeleteVertexArrays(1, &gVao_Pyramid);
		gVao_Pyramid = 0;
	}

	//destroy vbo of position
	if (gVbo_Position_Pyramid)
	{
		glDeleteVertexArrays(1, &gVbo_Position_Pyramid);
		gVbo_Position_Pyramid = 0;
	}

	//destroy vbo of normal
	if (gVbo_Normal_Pyramid)
	{
		glDeleteVertexArrays(1, &gVbo_Normal_Pyramid);
		gVbo_Normal_Pyramid = 0;
	}
	

	//SAFE SHADER CLEANUP
	if(gShaderProgramObject)
	{
		glUseProgram(gShaderProgramObject);
		
		GLsizei shaderCount;
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		
		GLuint* pShader;
		pShader = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
		
		if(pShader == NULL)
		{
			fprintf(gpFile, "\nCould Not Get Shader Count. Exitting Now!\n");
			exit(1);
		}
		
		glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShader);
		
		for(GLsizei i = 0; i < shaderCount; i++)
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


