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

GLuint gVao_Cube; 
GLuint gVbo_Position_Cube; 
GLuint gVbo_Normal_Cube;

GLuint modelViewMatrixUniform;
GLuint projectionMatrixUniform;
GLuint LKeyPressedUniform;
GLuint LdUniform;
GLuint KdUniform;
GLuint lightPositionUniform;
bool gbAnimate, gbLights;

GLuint gMVPMatrixUniform;
mat4 gPerspectiveProjectionMatrix;

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

    XStoreName(gpDisplay,gWindow,"PP : Assignment_013_3D_Diffuse_Light_On_Cube : by Kshitij Badkas");

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
	
	////////////////////////
	//   VERTEX SHADER    //
	////////////////////////
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
	
	//error-checking
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char * szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if(iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if(szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "VERTEX SHADER COMILATION LOG : %s\n", szInfoLog);
				free(szInfoLog);
				Uninitialize();
				exit(1);
			}
		}
	}
	
	
	//////////////////////////
	//   FRAGMENT SHADER    //
	//////////////////////////
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

	//error-checking
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if(iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if(szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "FRAGMENT SHADER COMILATION LOG : %s\n", szInfoLog);
				free(szInfoLog);
				Uninitialize();
				exit(1);
			}
		}
	}
	//////////////////////////
	//    SHADER OBJECT     //
	//////////////////////////
	//create 
	gShaderProgramObject = glCreateProgram();
	
	//attach
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
	
	//pre-link binding of vertex shader
	glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_POSITION, "vPosition");
	
	//pre-link binding of fragment shader
	glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_NORMAL, "vNormal");
	
	
	//link shader
	glLinkProgram(gShaderProgramObject);

	//error-checking
	iInfoLogLength = 0;
	GLint iShaderProgramLinkStatus = 0;
	szInfoLog = NULL;
	glGetShaderiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if(iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if(szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "SHADER PROGRAM LINK LOG : %s\n", szInfoLog);
				free(szInfoLog);
				Uninitialize();
				exit(1);
			}
		}
	}
	
	//get Uniform Location
	modelViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_view_matrix");
	projectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_L_KEY_PRESSED");
	LdUniform = glGetUniformLocation(gShaderProgramObject, "u_LD");
	KdUniform = glGetUniformLocation(gShaderProgramObject, "u_KD");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
	

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
		// front surface
		0.0f, 0.0f, 1.0f,  // top-right of front
		0.0f, 0.0f, 1.0f, // top-left of front
		0.0f, 0.0f, 1.0f, // bottom-left of front
		0.0f, 0.0f, 1.0f,  // bottom-right of front
		
		// right surface
		1.0f, 0.0f, 0.0f,  // top-right of right
		1.0f, 0.0f, 0.0f,  // top-left of right
		1.0f, 0.0f, 0.0f,  // bottom-left of right
		1.0f, 0.0f, 0.0f,  // bottom-right of right
		
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

		// top surface
		0.0f, 1.0f, 0.0f,  // top-right of top
		0.0f, 1.0f, 0.0f, // top-left of top
		0.0f, 1.0f, 0.0f, // bottom-left of top
		0.0f, 1.0f, 0.0f,  // bottom-right of top

		// bottom surface
		0.0f, -1.0f, 0.0f,  // top-right of bottom
		0.0f, -1.0f, 0.0f,  // top-left of bottom
		0.0f, -1.0f, 0.0f,  // bottom-left of bottom
		0.0f, -1.0f, 0.0f   // bottom-right of bottom
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

	//FOR CUBE NORMALS
	glGenBuffers(1, &gVbo_Normal_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Normal_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//BIND END
	glBindVertexArray(0);

	/////////////////////////////////////////////////////////////////////////////////
	
	//3D & Depth Code
	glClearDepth(1.0f);		//Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//Disable culling of back faces (default)
	glDisable(GL_CULL_FACE);
	
	glClearColor(0.0f,0.0f,0.0f,1.0f); //Black 
	
	//NEW
	gbAnimate = false;
	gbLights = false;
	
	//set Identity matrix to identity
	gPerspectiveProjectionMatrix = vmath::mat4::identity();
	
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
	//variable
	static GLfloat angleCube = 0.0f;
	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//use OpenGL Program Object
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
	glXSwapBuffers(gpDisplay, gWindow);
	
}

void Uninitialize(void)
{
	//code
	//CUBE
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


