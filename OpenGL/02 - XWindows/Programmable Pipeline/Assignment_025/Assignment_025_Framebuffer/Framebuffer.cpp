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

//SOIL 
#include<SOIL/SOIL.h>

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

GLuint gVertexShaderObject_color;
GLuint gFragmentShaderObject_color;
GLuint gShaderProgramObject_color;

GLuint gVao_Cube; 
GLuint gVbo_Position_Cube; 
GLuint gVbo_Texture_Cube;

GLuint gVao_Cube_2;
GLuint gVbo_Position_Cube_2;
GLuint gVbo_Color_Cube;

GLuint gMVPMatrixUniform;
GLuint gMVPMatrixUniform_color;

vmath::mat4 gPerspectiveProjectionMatrix;

//TEXTURES
GLuint kundali_texture;
GLuint gTextureSamplerUniform;

//FRAMEBUFFER
GLuint gFbo;
GLuint color_texture;
GLuint depth_texture;

int gWidth;
int gHeight;


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

    XStoreName(gpDisplay,gWindow,"PP : Assignment_010_3D_Shapes_Textures : by Kshitij Badkas");

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
	
	//texture
	GLuint LoadBitmapAsTexture(const char *);

	
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
		printf("The Obtained Rendering Context is Hardware Rendering Context. \n");
	}	
	else
	{
		printf("The Obtained Rendering Context is Software Rendering Context. \n");
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
	
	//pre-link binding of shader
	glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_POSITION, "vPosition");
	
	//pre-link binding of shader program object with fragment shader attribute
	glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_TEXCOORD, "vTexCoord");

	
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
	
	//get MVP Matrix Uniform
	gMVPMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");
	
		//get Texture Sampler Uniform Location
	gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");


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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
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
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		//RIGHT
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		//BACK
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		//LEFT
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		//TOP
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		//BOTTOM
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
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

	//Loading Textures
	kundali_texture = LoadBitmapAsTexture("Vijay_Kundali.bmp");

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
	
	GLenum fbComplete;
	fbComplete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbComplete == GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FRAMEBUFFER IS COMPLETE - %d\n", fbComplete);
	}
	else
	{
		printf("FRAMEBUFFER IS INCOMPLETE. %d\n", fbComplete);
		Uninitialize();
	}
	
	////////////////////////////////////////////////
	
	//SetglColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black

	//set identity matrix to identity matrix
	gPerspectiveProjectionMatrix = vmath::mat4::identity();

	Resize(giWindowWidth, giWindowHeight);
	//WARM-UP CALL TO Resize();

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

GLuint LoadBitmapAsTexture(const char * path)
{
	//variables
	int width, height;
	unsigned char * imageData = NULL;
	GLuint textureID;
	
	//code
	imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);
	
	//OPENGL TEXTURE CODE STARTS HERE
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	//Setting texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	//gluBuild2DMipmaps = glTexImage2D + glGenerateMipmap
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
	
	//gluBuild2DMipmaps = glTexImage2D + glGenerateMipmap
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	SOIL_free_image_data(imageData);
	
	return textureID;
	
}


void display(void)
{
/*
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


	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_texture);
	//glBindTexture(GL_TEXTURE_2D, kundali_texture);
	//glUniform1i(gTextureSamplerUniform, 0);

	
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
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);

	//ANIMATION CODE
	angleCube += 1.0f;
	if (angleCube >= 360.0f)
	{
		angleCube = 0.0f;
	}
*/

	//variable
	static GLfloat anglePyramid = 0.0f;
	static GLfloat angleCube = 0.0f;
	
		//OpenGL Drawing
	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 scaleMatrix = mat4::identity(); //NEW
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//use OpenGL Program Object
	glUseProgram(gShaderProgramObject);
	
	//CUBE
	translationMatrix = vmath::mat4::identity();
	rotationMatrix = vmath::mat4::identity();
	modelViewMatrix = vmath::mat4::identity();
	modelViewProjectionMatrix = vmath::mat4::identity();
	
	translationMatrix = vmath::translate(1.5f, 0.0f, -6.0f);
	rotationMatrix = vmath::rotate(angleCube, angleCube, angleCube);
	modelViewMatrix = translationMatrix * rotationMatrix;
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//NEW
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, kundali_texture);
	glUniform1i(gTextureSamplerUniform, 0);

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

	//ANIMATION
	anglePyramid += 1.0f;
	angleCube += 1.0f;
	
	if (anglePyramid >= 360.0f)
	{
		anglePyramid = 0.0f;
	}
	if (angleCube >= 360.0f)
	{
		angleCube = 0.0f;
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

	//destroy vbo of texture
	if (gVbo_Texture_Cube)
	{
		glDeleteVertexArrays(1, &gVbo_Texture_Cube);
		gVbo_Texture_Cube = 0;
	}
	
	//delete textures
	
	if(kundali_texture)
	{
		glDeleteTextures(1, &kundali_texture);
		kundali_texture = 0;
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


