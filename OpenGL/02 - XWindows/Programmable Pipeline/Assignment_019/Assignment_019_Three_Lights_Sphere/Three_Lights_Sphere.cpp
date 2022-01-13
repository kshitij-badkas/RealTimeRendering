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

    XStoreName(gpDisplay,gWindow,"PP : Assignment_019_Three_Rotating_Lights_On_Sphere : by Kshitij Badkas");

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

void getPerVertexLighting(void)
{
	//prototype
	void Uninitialize(void);
	
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
		"phong_ads_light = ambient[i] + diffuse[i] + specular[i];" \
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
				Uninitialize();
				exit(1);
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
				Uninitialize();
				exit(1);
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
	//prototype
	void Uninitialize(void);

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
				Uninitialize();
				exit(1);
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
		"phong_ads_light_PF = ambient_PF[i] + diffuse_PF[i] + specular_PF[i];" \
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
				Uninitialize();
				exit(1);
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

