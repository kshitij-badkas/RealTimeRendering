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


#define BUFFER_SIZE 1024 //MAX LENGHT OF STRING IN MESH FILE
#define S_EQUAL 0 //RETURN VALUE OF STRCMP WHEN STRINGS ARE EQUAL

#define NR_POINT_COORDS 3
#define NR_TEXTURE_COORDS 2
#define NR_NORMAL_COORDS 3
#define NR_FACE_TOKENS 3
#define NR_TRIANGLE_VERTICES 3


enum
{
	KAB_ATTRIBUTE_POSITION = 0,
	KAB_ATTRIBUTE_COLOR,
	KAB_ATTRIBUTE_NORMAL,
	KAB_ATTRIBUTE_TEXCOORD
};


//namspaces
using namespace vmath;
using namespace std;

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

GLuint gVao;
GLuint gVbo_Position;
GLuint gMVPMatrixUniform;
vmath::mat4 gPerspectiveProjectionMatrix;

GLuint gElementBuffer;
char buffer[BUFFER_SIZE];

//Mesh file
FILE* g_fp_meshfile = NULL;

struct vec_int
{
	int* p;
	int size;
};

struct vec_float
{
	float* pf;
	int size;
};


//Vector of vector of floats to hold vertex data
struct vec_float* gp_vertex;
struct vec_float* gp_texture;
struct vec_float* gp_normal;

struct vec_float* gp_vertex_sorted;
struct vec_float* gp_texture_sorted;
struct vec_float* gp_normal_sorted;

//Vector of vector of ints to hold index data in g_vertices
struct vec_int* gp_vertex_indices;
struct vec_int* gp_texture_indices;
struct vec_int* gp_normal_indices;

GLfloat g_rotate = 0.0f;


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

    XStoreName(gpDisplay,gWindow,"PP : Assignment_026_Model_Loading : by Kshitij Badkas");

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
	
	void load_mesh(void);
	
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
	const GLchar* vertexShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"uniform mat4 u_mvpMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvpMatrix * vPosition;" \
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
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" \
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
	
	load_mesh();

	//vertices, colours, shader attribs, vbo, vao initializations
	
	glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVao);

	glGenBuffers(1, &gVbo_Position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
	glBufferData(GL_ARRAY_BUFFER, gp_vertex_sorted->size * sizeof(GLfloat), gp_vertex_sorted->pf, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	//Elements
	glGenBuffers(1, &gElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gp_vertex_indices->size * sizeof(int), gp_vertex_indices->p, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	//3D & Depth Code
	glShadeModel(GL_SMOOTH);	//anti-aliasing
	glClearDepth(1.0f);		//Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	//we will always cull back faces for beter performance
	//glEnable(GL_CULL_FACE);
	
	glClearColor(0.0f,0.0f,0.0f,1.0f); //BLACK 
	
	//set Identity matrix to identity
	gPerspectiveProjectionMatrix = vmath::mat4::identity();
	
	//WARM-UP CALL
	Resize(giWindowWidth, giWindowHeight);
}

void load_mesh(void)
{
	//prototype
	void Uninitialize(void);

	//code
	struct vec_int* create_vec_int();
	struct vec_float* create_vec_float();
	int push_back_vec_int(struct vec_int* p_vec_int, int data);
	int push_back_vec_float(struct vec_float* p_vec_int, float data);
	int destroy_vec_float(struct vec_float* p_vec_float);

	const char* space = " ";
	const char* slash = "/";
	char* first_token = NULL;
	char* token = NULL;
	char* f_entries[3] = { NULL,NULL,NULL };

	int nr_pos_cords = 0;
	int nr_tex_cords = 0;
	int nr_normal_cords = 0;
	int nr_faces = 0;

	int i, vi;

	//open mesh file
	g_fp_meshfile = fopen("MonkeyHead.obj", "r");
	if (!g_fp_meshfile)
		Uninitialize();

	gp_vertex = create_vec_float();
	gp_texture = create_vec_float();
	gp_normal = create_vec_float();

	gp_vertex_indices = create_vec_int();
	gp_texture_indices = create_vec_int();
	gp_normal_indices = create_vec_int();

	while (fgets(buffer, BUFFER_SIZE, g_fp_meshfile) != NULL)
	{
		first_token = strtok(buffer, space);

		if (strcmp(first_token, "v") == S_EQUAL)
		{
			//GLfloat* pvec_point_coord = (GLfloat*)xcalloc(NR_POINT_COORDS, sizeof(GLfloat));
			//for (int i = 0; i != NR_POINT_COORDS; i++)
			nr_pos_cords++;
			while ((token = strtok(NULL, space)) != NULL)
				push_back_vec_float(gp_vertex, atof(token));

		}

		else if (strcmp(first_token, "vt") == S_EQUAL)
		{
			//GLfloat* pvec_texture_coord = (GLfloat*)xcalloc(NR_TEXTURE_COORDS, sizeof(GLfloat));
			//for (int i = 0; i != NR_TEXTURE_COORDS; i++)
			nr_tex_cords++;
			while ((token = strtok(NULL, space)) != NULL)
				push_back_vec_float(gp_texture, atof(token));
		}

		else if (strcmp(first_token, "vn") == S_EQUAL)
		{
			//GLfloat* pvec_normal_coord = (GLfloat*)xcalloc(NR_NORMAL_COORDS, sizeof(GLfloat));
			//for (int i = 0; i != NR_NORMAL_COORDS; i++)
			nr_normal_cords++;
			while ((token = strtok(NULL, space)) != NULL)
				push_back_vec_float(gp_normal, atof(token));
		}

		else if (strcmp(first_token, "f") == S_EQUAL)
		{
			//GLint* pvec_vertex_indices = (GLint*)xcalloc(3, sizeof(GLint));
			//GLint* pvec_texture_indices = (GLint*)xcalloc(3, sizeof(GLint));
			//GLint* pvec_normal_indices = (GLint*)xcalloc(3, sizeof(GLint));

			nr_faces++;
			for (i = 0; i < 3; i++)
			{
				f_entries[i] = strtok(NULL, space);
			}


			for (i = 0; i < 3; i++)
			{
				token = strtok(f_entries[i], slash);
				push_back_vec_int(gp_vertex_indices, atoi(token) - 1);

				token = strtok(NULL, slash);
				push_back_vec_int(gp_texture_indices, atoi(token) - 1);

				token = strtok(NULL, slash);
				push_back_vec_int(gp_normal_indices, atoi(token) - 1);

			}
		}

	}

	//sorted vertices, etc. code here
	gp_vertex_sorted = create_vec_float();
	for (int i = 0; i < gp_vertex_indices->size; i++)
	{
		push_back_vec_float(gp_vertex_sorted, gp_vertex->pf[i]);
	}

	gp_texture_sorted = create_vec_float();
	for (int i = 0; i < gp_texture_indices->size; i++)
	{
		push_back_vec_float(gp_texture_sorted, gp_vertex->pf[i]);
	}

	gp_normal_sorted = create_vec_float();
	for (int i = 0; i < gp_normal_indices->size; i++)
	{
		push_back_vec_float(gp_normal_sorted, gp_vertex->pf[i]);
	}

	fclose(g_fp_meshfile);
	g_fp_meshfile = NULL;

	//fprintf(gpFile, "gp_vertices->size: %llu gp_texture->size :%llu gp_normals :%llu gp_face_tri :%llu \n", gp_vertices->size, gp_texture->size, gp_normals->size, gp_face_tri->size);

}

struct vec_int* create_vec_int()
{
	struct vec_int* p = (struct vec_int*)malloc(sizeof(struct vec_int));
	memset(p, 0, sizeof(struct vec_int));
	return p;
}

struct vec_float* create_vec_float()
{
	struct vec_float* p = (struct vec_float*)malloc(sizeof(struct vec_float));
	memset(p, 0, sizeof(struct vec_float));
	return p;
}

int push_back_vec_int(struct vec_int* p_vec_int, int data)
{
	p_vec_int->p = (int*)realloc(p_vec_int->p, (p_vec_int->size + 1) * sizeof(int));
	p_vec_int->size = p_vec_int->size + 1;
	p_vec_int->p[p_vec_int->size - 1] = data;
	return 0;
}

int push_back_vec_float(struct vec_float* p_vec_float, float data)
{
	p_vec_float->pf = (float*)realloc(p_vec_float->pf, (p_vec_float->size + 1) * sizeof(float));
	p_vec_float->size = p_vec_float->size + 1;
	p_vec_float->pf[p_vec_float->size - 1] = data;
	return 0;
}
/*
int destroy_vec_int(struct vec_int* p_vec_int)
{
	free(p_vec_int->p);
	free(p_vec_int);
	return 0;
}
*/
int destroy_vec_float(struct vec_float* p_vec_float)
{
	free(p_vec_float->pf);
	free(p_vec_float);
	return 0;
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
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);


	//OpenGL Drawing
	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	rotationMatrix = rotate(g_rotate, 0.0f, 1.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//bind vao - vertex array object
	glBindVertexArray(gVao);

	//draw
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer);
	glDrawElements(GL_TRIANGLES, (gp_vertex_indices->size), GL_UNSIGNED_INT, NULL);

	//unbind vao
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);

	g_rotate = g_rotate + 1.0f;
	if (g_rotate >= 360.0f)
	{
		g_rotate = 0.0f;
	}
	
	//Double Buffer
	glXSwapBuffers(gpDisplay, gWindow);
}

void Uninitialize(void)
{
	//code
	
	//destroy vao
	if(gVao)
	{
		glDeleteVertexArrays(1, &gVao);
		gVao = 0;
	}
	//destroy vbo
	if(gVbo_Position)
	{
		glDeleteVertexArrays(1, &gVbo_Position);
		gVbo_Position = 0;	
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
    
    //clean vectors
	destroy_vec_float(gp_vertex);
	destroy_vec_float(gp_texture);
	destroy_vec_float(gp_normal);

	//destroy_vec_int(gp_vertex_indices);
	//destroy_vec_int(gp_texture_indices);
	//destroy_vec_int(gp_normal_indices);

    
    //close file
    if(gpFile)
    {
    	fclose(gpFile);
    	gpFile = NULL;
    }
}

