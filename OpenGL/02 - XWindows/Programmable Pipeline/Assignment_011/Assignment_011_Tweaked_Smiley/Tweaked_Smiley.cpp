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

GLuint gVao_Square;
GLuint gVbo_Position_Square;
GLuint gVbo_Texcoord_Square;
GLuint smiley_texture;
GLuint gTextureSamplerUniform;
GLuint gOtherKeyPressedUniform;

GLuint gMVPMatrixUniform;
mat4 gPerspectiveProjectionMatrix;

unsigned int pressed_key = 0;

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
					pressed_key = keysym;
					fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
					glDisable(GL_TEXTURE_2D);
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

    XStoreName(gpDisplay,gWindow,"PP : Assignment_011_Tweaked_Smiley : by Kshitij Badkas");

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
	glClearDepth(1.0f);		//Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//we will always cull back faces for beter performance
	glEnable(GL_CULL_FACE);

	//Loading Textures
	smiley_texture = LoadBitmapAsTexture("Smiley.bmp");

	//SetglColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black

	//set identity matrix to identity matrix
	gPerspectiveProjectionMatrix = mat4::identity();

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
	//variable
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

	//Double Buffer
	glXSwapBuffers(gpDisplay, gWindow);
	
}

void Uninitialize(void)
{
	//code
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

	//delete texture
	if (smiley_texture)
	{
		glDeleteTextures(1, &smiley_texture);
		smiley_texture = 0;
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


