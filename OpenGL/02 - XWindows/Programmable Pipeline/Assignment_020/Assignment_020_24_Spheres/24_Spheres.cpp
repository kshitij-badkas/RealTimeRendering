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

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_sphere1;
GLuint gVbo_sphere_position1;
GLuint gVbo_sphere_normal1;
GLuint gVbo_sphere_element1;

mat4 gPerspectiveProjectionMatrix;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint gNumVertices;
GLuint gNumElements; //Elements a.k.a Indices

bool gbLights = false;

GLuint LKeyPressedUniform; //L key is pressed

//light and material properties
GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };


//MODEL-VIEW-PROJECTION UNIFORMS
GLuint gModelMatrixUniform;
GLuint gViewMatrixUniform;
GLuint gProjectionMatrixUniform;

//LIGHT UNIFORMS - ALL ARRAYS
GLuint LaUniform; //ambient light component
GLuint LdUniform; //diffuse light component
GLuint LsUniform; //specular light component
GLuint lightPositionUniform; //light position

//MATERIAL UNIFORMS
GLuint KaUniform1; //ambient material component
GLuint KdUniform1; //diffuse material component
GLuint KsUniform1; //specular material component
GLuint materialShininessUniform1; //shininess material

GLuint KaUniform2; //ambient material component
GLuint KdUniform2; //diffuse material component
GLuint KsUniform2; //specular material component
GLuint materialShininessUniform2; //shininess material

GLuint KaUniform3; //ambient material component
GLuint KdUniform3; //diffuse material component
GLuint KsUniform3; //specular material component
GLuint materialShininessUniform3; //shininess material

GLuint KaUniform4; //ambient material component
GLuint KdUniform4; //diffuse material component
GLuint KsUniform4; //specular material component
GLuint materialShininessUniform4; //shininess material

GLuint KaUniform5; //ambient material component
GLuint KdUniform5; //diffuse material component
GLuint KsUniform5; //specular material component
GLuint materialShininessUniform5; //shininess material

GLuint KaUniform6; //ambient material component
GLuint KdUniform6; //diffuse material component
GLuint KsUniform6; //specular material component
GLuint materialShininessUniform6; //shininess material

GLuint KaUniform7; //ambient material component
GLuint KdUniform7; //diffuse material component
GLuint KsUniform7; //specular material component
GLuint materialShininessUniform7; //shininess material

GLuint KaUniform8; //ambient material component
GLuint KdUniform8; //diffuse material component
GLuint KsUniform8; //specular material component
GLuint materialShininessUniform8; //shininess material

GLuint KaUniform9; //ambient material component
GLuint KdUniform9; //diffuse material component
GLuint KsUniform9; //specular material component
GLuint materialShininessUniform9; //shininess material

GLuint KaUniform10; //ambient material component
GLuint KdUniform10; //diffuse material component
GLuint KsUniform10; //specular material component
GLuint materialShininessUniform10; //shininess material

GLuint KaUniform11; //ambient material component
GLuint KdUniform11; //diffuse material component
GLuint KsUniform11; //specular material component
GLuint materialShininessUniform11; //shininess material

GLuint KaUniform12; //ambient material component
GLuint KdUniform12; //diffuse material component
GLuint KsUniform12; //specular material component
GLuint materialShininessUniform12; //shininess material

GLuint KaUniform13; //ambient material component
GLuint KdUniform13; //diffuse material component
GLuint KsUniform13; //specular material component
GLuint materialShininessUniform13; //shininess material

GLuint KaUniform14; //ambient material component
GLuint KdUniform14; //diffuse material component
GLuint KsUniform14; //specular material component
GLuint materialShininessUniform14; //shininess material

GLuint KaUniform15; //ambient material component
GLuint KdUniform15; //diffuse material component
GLuint KsUniform15; //specular material component
GLuint materialShininessUniform15; //shininess material

GLuint KaUniform16; //ambient material component
GLuint KdUniform16; //diffuse material component
GLuint KsUniform16; //specular material component
GLuint materialShininessUniform16; //shininess material

GLuint KaUniform17; //ambient material component
GLuint KdUniform17; //diffuse material component
GLuint KsUniform17; //specular material component
GLuint materialShininessUniform17; //shininess material

GLuint KaUniform18; //ambient material component
GLuint KdUniform18; //diffuse material component
GLuint KsUniform18; //specular material component
GLuint materialShininessUniform18; //shininess material

GLuint KaUniform19; //ambient material component
GLuint KdUniform19; //diffuse material component
GLuint KsUniform19; //specular material component
GLuint materialShininessUniform19; //shininess material

GLuint KaUniform20; //ambient material component
GLuint KdUniform20; //diffuse material component
GLuint KsUniform20; //specular material component
GLuint materialShininessUniform20; //shininess material

GLuint KaUniform21; //ambient material component
GLuint KdUniform21; //diffuse material component
GLuint KsUniform21; //specular material component
GLuint materialShininessUniform21; //shininess material

GLuint KaUniform22; //ambient material component
GLuint KdUniform22; //diffuse material component
GLuint KsUniform22; //specular material component
GLuint materialShininessUniform22; //shininess material

GLuint KaUniform23; //ambient material component
GLuint KdUniform23; //diffuse material component
GLuint KsUniform23; //specular material component
GLuint materialShininessUniform23; //shininess material

GLuint KaUniform24; //ambient material component
GLuint KdUniform24; //diffuse material component
GLuint KsUniform24; //specular material component
GLuint materialShininessUniform24; //shininess material

// material will have 24 different values
GLfloat materialAmbient1[] = { 0.0215f, 0.1745f, 0.0215f, 1.0f };
GLfloat materialDiffuse1[] = { 0.07568f, 0.61424f, 0.07568f, 1.0f };
GLfloat materialSpecular1[] = { 0.633f, 0.727811f, 0.633f, 1.0f };
GLfloat materialShininess1 = 0.6f * 128.0f;

GLfloat materialAmbient2[] = { 0.135f, 0.225f, 0.1575f, 1.0f };
GLfloat materialDiffuse2[] = { 0.54f, 0.89f, 0.63f, 1.0f };
GLfloat materialSpecular2[] = { 0.316228f, 0.316228f, 0.316228f, 1.0f };
GLfloat materialShininess2 = 0.1f * 128.0f;

GLfloat materialAmbient3[] = { 0.05375f, 0.05f, 0.06625f, 1.0f };
GLfloat materialDiffuse3[] = { 0.18275f, 0.17f, 0.22525f, 1.0f };
GLfloat materialSpecular3[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess3 = 0.3f * 128.0f;

GLfloat materialAmbient4[] = { 0.25f, 0.20725f, 0.20725f, 1.0f };
GLfloat materialDiffuse4[] = { 1.0f, 0.829f, 0.829f, 1.0f };
GLfloat materialSpecular4[] = { 0.296648f, 0.296648f, 0.296648f, 1.0f };
GLfloat materialShininess4 = 0.088f * 128.0f;

GLfloat materialAmbient5[] = { 0.1745f, 0.01175f, 0.01175f, 1.0f };
GLfloat materialDiffuse5[] = { 0.61424f, 0.04136f, 0.04136f, 1.0f };
GLfloat materialSpecular5[] = { 0.727811f, 0.626959f, 0.626959f, 1.0f };
GLfloat materialShininess5 = 0.6f * 128.0f;

GLfloat materialAmbient6[] = { 0.1f, 0.18725f, 0.1745f, 1.0f };
GLfloat materialDiffuse6[] = { 0.396f, 0.74151f, 0.69102f, 1.0f };
GLfloat materialSpecular6[] = { 0.297254f, 0.30829f, 0.306678f, 1.0f };
GLfloat materialShininess6 = 0.1f * 128.0f;

GLfloat materialAmbient7[] = { 0.329412f, 0.223529f, 0.027451f, 1.0f };
GLfloat materialDiffuse7[] = { 0.780392f, 0.568627f, 0.113725f, 1.0f };
GLfloat materialSpecular7[] = { 0.992157f, 0.941176f, 0.807843f, 1.0f };
GLfloat materialShininess7 = 0.21794872f * 128.0f;

GLfloat materialAmbient8[] = { 0.2125f, 0.1275f, 0.054f, 1.0f };
GLfloat materialDiffuse8[] = { 0.714f, 0.4284f, 0.18144f, 1.0f };
GLfloat materialSpecular8[] = { 0.393548f, 0.271906f, 0.166721f, 1.0f };
GLfloat materialShininess8 = 0.2f * 128.0f;

GLfloat materialAmbient9[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat materialDiffuse9[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat materialSpecular9[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat materialShininess9 = 0.6f * 128.0f;

GLfloat materialAmbient10[] = { 0.19125f, 0.0735f, 0.0225f, 1.0f };
GLfloat materialDiffuse10[] = { 0.7038f, 0.27048f, 0.0828f, 1.0f };
GLfloat materialSpecular10[] = { 0.256777f, 0.137622f, 0.086014f, 1.0f };
GLfloat materialShininess10 = 0.1f * 128.0f;

GLfloat materialAmbient11[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
GLfloat materialDiffuse11[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
GLfloat materialSpecular11[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
GLfloat materialShininess11 = 0.4f * 128.0f;

GLfloat materialAmbient12[] = { 0.19225f, 0.19225f, 0.19225f, 1.0f };
GLfloat materialDiffuse12[] = { 0.50754f, 0.50754f, 0.50754f, 1.0f };
GLfloat materialSpecular12[] = { 0.508273f, 0.508273f, 0.508273f, 1.0f };
GLfloat materialShininess12 = 0.4f * 128.0f;

GLfloat materialAmbient13[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse13[] = { 0.01f, 0.01f, 0.01f, 1.0f };
GLfloat materialSpecular13[] = { 0.50f, 0.50f, 0.50f, 1.0f };
GLfloat materialShininess13 = 0.25f * 128.0f;

GLfloat materialAmbient14[] = { 0.0f, 0.1f, 0.06f, 1.0f };
GLfloat materialDiffuse14[] = { 0.0f, 0.50980392f, 0.50980392f, 1.0f };
GLfloat materialSpecular14[] = { 0.50196078f, 0.50196078f, 0.50196078f, 1.0f };
GLfloat materialShininess14 = 0.25f * 128.0f;

GLfloat materialAmbient15[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse15[] = { 0.1f, 0.35f, 0.1f, 1.0f };
GLfloat materialSpecular15[] = { 0.45f, 0.55f, 0.45f, 1.0f };
GLfloat materialShininess15 = 0.25f * 128.0f;

GLfloat materialAmbient16[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse16[] = { 0.5f, 0.0f, 0.0f, 1.0f };
GLfloat materialSpecular16[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat materialShininess16 = 0.25f * 128.0f;

GLfloat materialAmbient17[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse17[] = { 0.55f, 0.55f, 0.55f, 1.0f };
GLfloat materialSpecular17[] = { 0.70f, 0.70f, 0.70f, 1.0f };
GLfloat materialShininess17 = 0.25f * 128.0f;

GLfloat materialAmbient18[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse18[] = { 0.5f, 0.5f, 0.0f, 1.0f };
GLfloat materialSpecular18[] = { 0.60f, 0.60f, 0.50f, 1.0f };
GLfloat materialShininess18 = 0.25f * 128.0f;

GLfloat materialAmbient19[] = { 0.02f, 0.02f, 0.02f, 1.0f };
GLfloat materialDiffuse19[] = { 0.01f, 0.01f, 0.01f, 1.0f };
GLfloat materialSpecular19[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat materialShininess19 = 0.078125f * 128.0f;

GLfloat materialAmbient20[] = { 0.0f, 0.05f, 0.05f, 1.0f };
GLfloat materialDiffuse20[] = { 0.4f, 0.5f, 0.5f, 1.0f };
GLfloat materialSpecular20[] = { 0.04f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess20 = 0.078125f * 128.0f;

GLfloat materialAmbient21[] = { 0.0f, 0.05f, 0.0f, 1.0f };
GLfloat materialDiffuse21[] = { 0.4f, 0.5f, 0.4f, 1.0f };
GLfloat materialSpecular21[] = { 0.04f, 0.7f, 0.04f, 1.0f };
GLfloat materialShininess21 = 0.078125f * 128.0f;

GLfloat materialAmbient22[] = { 0.05f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse22[] = { 0.5f, 0.4f, 0.4f, 1.0f };
GLfloat materialSpecular22[] = { 0.7f, 0.04f, 0.04f, 1.0f };
GLfloat materialShininess22 = 0.078125f * 128.0f;

GLfloat materialAmbient23[] = { 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat materialDiffuse23[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat materialSpecular23[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess23 = 0.078125f * 128.0f;

GLfloat materialAmbient24[] = { 0.5f, 0.05f, 0.0f, 1.0f };
GLfloat materialDiffuse24[] = { 0.5f, 0.5f, 0.4f, 1.0f };
GLfloat materialSpecular24[] = { 0.7f, 0.7f, 0.04f, 1.0f };
GLfloat materialShininess24 = 0.078125f * 128.0f;


GLfloat updated_width = 0.0f;
GLfloat updated_height = 0.0f;

//new
//Light Rotation
GLfloat XRotation = 0.0f;
GLfloat YRotation = 0.0f;
GLfloat ZRotation = 0.0f;

GLuint key_pressed = 0; // = 1 for X Axis; = 2 for Y Axis; = 3 for Z Axis.



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
					
				case 'x':
				case 'X':
					key_pressed = 1;
					XRotation = 0.0f;
					fprintf(gpFile, "WNDPROC X - key_pressed = %d \n", key_pressed);
					break;

				case 'y':
				case 'Y':
					key_pressed = 2;
					YRotation = 0.0f;
					fprintf(gpFile, "WNDPROC Y - key_pressed = %d \n", key_pressed);
					break;


				case 'z':
				case 'Z':
					key_pressed = 3;
					ZRotation = 0.0f;
					fprintf(gpFile, "WNDPROC Z - key_pressed = %d \n", key_pressed);
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

    XStoreName(gpDisplay,gWindow,"PP : Assignment_020_24_Spheres : by Kshitij Badkas");

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
	const GLchar* vertexShaderSourceCode = 
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec4 u_light_position;" \
		"out vec3 transformed_normal;" \
		"out vec3 light_direction;" \
		"out vec3 view_vector;" \
		"void main(void)" \
		"{" \
		"if(u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
		"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"light_direction = vec3(u_light_position - eye_coordinates);" \
		//SWIZZLING
		"view_vector = -eye_coordinates.xyz;" \
		
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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
		"in vec3 transformed_normal;" \
		"in vec3 light_direction;" \
		"in vec3 view_vector;" \
		"uniform int u_L_KEY_PRESSED;" \
		"uniform vec3 u_LA;" \
		"uniform vec3 u_LD;" \
		"uniform vec3 u_LS;" \
		"uniform vec3 u_KA;" \
		"uniform vec3 u_KD;" \
		"uniform vec3 u_KS;" \
		"uniform float u_material_shininess;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_light;" \
		"if(u_L_KEY_PRESSED == 1)" \
		"{" \
		"vec3 normalized_transformed_normal = normalize(transformed_normal);" \
		"vec3 normalized_light_direction = normalize(light_direction);" \
		"vec3 normalized_view_vector = normalize(view_vector);" \
		"vec3 ambient = u_LA * u_KA;" \
		"vec3 diffuse = u_LD * u_KD * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);" \
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);" \
		"vec3 specular = u_LS * u_KS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_material_shininess);" \
		"phong_ads_light = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
		"}" \
		"FragColor = vec4(phong_ads_light, 1.0f);" \
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
	gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_L_KEY_PRESSED");

	LaUniform = glGetUniformLocation(gShaderProgramObject, "u_LA");
	LdUniform = glGetUniformLocation(gShaderProgramObject, "u_LD");
	LsUniform = glGetUniformLocation(gShaderProgramObject, "u_LS");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

	KaUniform1 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform1 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform1 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform1 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform2 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform2 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform2 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform2 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform3 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform3 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform3 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform3 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform4 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform4 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform4 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform4 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform5 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform5 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform5 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform5 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform6 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform6 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform6 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform6 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform7 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform7 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform7 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform7 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform8 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform8 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform8 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform8 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform9 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform9 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform9 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform9 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform10 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform10 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform10 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform10 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform11 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform11 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform11 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform11 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform12 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform12 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform12 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform12 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform13 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform13 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform13 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform13 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform14 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform14 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform14 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform14 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform15 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform15 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform15 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform15 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform16 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform16 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform16 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform16 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform17 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform17 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform17 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform17 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform18 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform18 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform18 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform18 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform19 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform19 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform19 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform19 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform20 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform20 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform20 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform20 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform21 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform21 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform21 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform21 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform22 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform22 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform22 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform22 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform23 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform23 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform23 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform23 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	KaUniform24 = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform24 = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform24 = glGetUniformLocation(gShaderProgramObject, "u_KS");
	materialShininessUniform24 = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	
	//vertices, colours, vbo, vao
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	////////////////////////////  SPHERE 1   ////////////////////////////////

	//vao 1
	glGenVertexArrays(1, &gVao_sphere1);
	glBindVertexArray(gVao_sphere1);

	// position vbo of sphere
	glGenBuffers(1, &gVbo_sphere_position1);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo of sphere
	glGenBuffers(1, &gVbo_sphere_normal1);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo of sphere
	glGenBuffers(1, &gVbo_sphere_element1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
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
	
	glClearColor(0.25f,0.25f,0.25f,1.0f); 
	
	gbLights = false;
	
	//set Identity matrix to identity
	gPerspectiveProjectionMatrix = mat4::identity();
	
	//WARM-UP CALL
	Resize(giWindowWidth, giWindowHeight);
}

void Resize(int width, int height)
{
	//code
	updated_width = width;
	updated_height = height;
	
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
	GLfloat radius = 5.0f;

	mat4 translationMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 projectionMatrix = mat4::identity();

	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//OpenGL Drawing
	//MAKE IDENTITY
	translationMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	projectionMatrix = gPerspectiveProjectionMatrix;


	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	if (gbLights == true)
	{
		if (key_pressed == 1)
		{
			fprintf(gpFile, "XROtation - key_pressed = %d \n", key_pressed);
			
			//X AXIS ROATION
			lightPosition[0] = (GLfloat)(radius * sin(YRotation)); //x
			lightPosition[1] = 0.0f; //y
			lightPosition[2] = (GLfloat)(radius * cos(YRotation)); //z
			lightPosition[3] = 1.0f; //w
		}
		else if (key_pressed == 2)
		{
			fprintf(gpFile, "YROtation - key_pressed = %d \n", key_pressed);
			
			//Y AXIS ROATION
			lightPosition[0] = 0.0f; //x
			lightPosition[1] = (GLfloat)(radius * sin(YRotation)); //y
			lightPosition[2] = (GLfloat)(radius * cos(YRotation)); //z
			lightPosition[3] = 1.0f; //w
		}
		else if (key_pressed == 3)
		{
			fprintf(gpFile, "ZROtation - key_pressed = %d \n", key_pressed);

			//Z AXIS ROATION
			lightPosition[0] = (GLfloat)(radius * sin(ZRotation)); //x
			lightPosition[1] = (GLfloat)(radius * cos(ZRotation)); //y
			lightPosition[2] = 0.0f; //z
			lightPosition[3] = 1.0f; //w
		}

		glUniform1i(LKeyPressedUniform, 1);
		glUniform3fv(LaUniform, 1, (GLfloat*)lightAmbient);
		glUniform3fv(LdUniform, 1, (GLfloat*)lightDiffuse);
		glUniform3fv(LsUniform, 1, (GLfloat*)lightSpecular);
		glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	//bind vao - vertex array object
	glBindVertexArray(gVao_sphere1);

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, translationMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);
	
	////////////////////////////  SPHERE 1  ////////////////////////////////

	//draw
	glViewport(0, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform1, 1, (GLfloat*)materialAmbient1);
	glUniform3fv(KdUniform1, 1, (GLfloat*)materialDiffuse1);
	glUniform3fv(KsUniform1, 1, (GLfloat*)materialSpecular1);
	glUniform1f(materialShininessUniform1, materialShininess1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	////////////////////////////  SPHERE 2  ////////////////////////////////

	//draw
	glViewport(updated_width / 4, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform2, 1, (GLfloat*)materialAmbient2);
	glUniform3fv(KdUniform2, 1, (GLfloat*)materialDiffuse2);
	glUniform3fv(KsUniform2, 1, (GLfloat*)materialSpecular2);
	glUniform1f(materialShininessUniform2, materialShininess2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 3  ////////////////////////////////

	glViewport(updated_width / 2, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform3, 1, (GLfloat*)materialAmbient3);
	glUniform3fv(KdUniform3, 1, (GLfloat*)materialDiffuse3);
	glUniform3fv(KsUniform3, 1, (GLfloat*)materialSpecular3);
	glUniform1f(materialShininessUniform3, materialShininess3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	////////////////////////////  SPHERE 4  ////////////////////////////////

	glViewport(updated_width * 3.0f / 4.0f, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform4, 1, (GLfloat*)materialAmbient4);
	glUniform3fv(KdUniform4, 1, (GLfloat*)materialDiffuse4);
	glUniform3fv(KsUniform4, 1, (GLfloat*)materialSpecular4);
	glUniform1f(materialShininessUniform4, materialShininess4);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	
	////////////////////////////  SPHERE 5  ////////////////////////////////

	glViewport(0, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform5, 1, (GLfloat*)materialAmbient5);
	glUniform3fv(KdUniform5, 1, (GLfloat*)materialDiffuse5);
	glUniform3fv(KsUniform5, 1, (GLfloat*)materialSpecular5);
	glUniform1f(materialShininessUniform5, materialShininess5);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	
	////////////////////////////  SPHERE 6  ////////////////////////////////

	glViewport(updated_width / 4, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform6, 1, (GLfloat*)materialAmbient6);
	glUniform3fv(KdUniform6, 1, (GLfloat*)materialDiffuse6);
	glUniform3fv(KsUniform6, 1, (GLfloat*)materialSpecular6);
	glUniform1f(materialShininessUniform6, materialShininess6);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 7  ////////////////////////////////

	glViewport(updated_width / 2, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform7, 1, (GLfloat*)materialAmbient7);
	glUniform3fv(KdUniform7, 1, (GLfloat*)materialDiffuse7);
	glUniform3fv(KsUniform7, 1, (GLfloat*)materialSpecular7);
	glUniform1f(materialShininessUniform7, materialShininess7);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 8  ////////////////////////////////

	glViewport(updated_width * 3.0f / 4.0f, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform8, 1, (GLfloat*)materialAmbient8);
	glUniform3fv(KdUniform8, 1, (GLfloat*)materialDiffuse8);
	glUniform3fv(KsUniform8, 1, (GLfloat*)materialSpecular8);
	glUniform1f(materialShininessUniform8, materialShininess8);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	
	////////////////////////////  SPHERE 9  ////////////////////////////////

	glViewport(0, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform9, 1, (GLfloat*)materialAmbient9);
	glUniform3fv(KdUniform9, 1, (GLfloat*)materialDiffuse9);
	glUniform3fv(KsUniform9, 1, (GLfloat*)materialSpecular9);
	glUniform1f(materialShininessUniform9, materialShininess9);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 10  ////////////////////////////////

	//draw
	glViewport(updated_width / 4, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform10, 1, (GLfloat*)materialAmbient10);
	glUniform3fv(KdUniform10, 1, (GLfloat*)materialDiffuse10);
	glUniform3fv(KsUniform10, 1, (GLfloat*)materialSpecular10);
	glUniform1f(materialShininessUniform10, materialShininess10);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 11  ////////////////////////////////

	glViewport(updated_width / 2, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform11, 1, (GLfloat*)materialAmbient11);
	glUniform3fv(KdUniform11, 1, (GLfloat*)materialDiffuse11);
	glUniform3fv(KsUniform11, 1, (GLfloat*)materialSpecular11);
	glUniform1f(materialShininessUniform11, materialShininess11);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	////////////////////////////  SPHERE 12  ////////////////////////////////

	glViewport(updated_width * 3.0f / 4.0f, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform12, 1, (GLfloat*)materialAmbient12);
	glUniform3fv(KdUniform12, 1, (GLfloat*)materialDiffuse12);
	glUniform3fv(KsUniform12, 1, (GLfloat*)materialSpecular12);
	glUniform1f(materialShininessUniform12, materialShininess12);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	
	////////////////////////////  SPHERE 13  ////////////////////////////////

	//draw
	glViewport(0, updated_height * 1.65 / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform13, 1, (GLfloat*)materialAmbient13);
	glUniform3fv(KdUniform13, 1, (GLfloat*)materialDiffuse13);
	glUniform3fv(KsUniform13, 1, (GLfloat*)materialSpecular13);
	glUniform1f(materialShininessUniform13, materialShininess13);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 14  ////////////////////////////////

	//draw
	glViewport(updated_width / 4, updated_height * 1.65 / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform14, 1, (GLfloat*)materialAmbient14);
	glUniform3fv(KdUniform14, 1, (GLfloat*)materialDiffuse14);
	glUniform3fv(KsUniform14, 1, (GLfloat*)materialSpecular14);
	glUniform1f(materialShininessUniform14, materialShininess14);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	////////////////////////////  SPHERE 15  ////////////////////////////////

	glViewport(updated_width / 2, updated_height * 1.65 / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform15, 1, (GLfloat*)materialAmbient15);
	glUniform3fv(KdUniform15, 1, (GLfloat*)materialDiffuse15);
	glUniform3fv(KsUniform15, 1, (GLfloat*)materialSpecular15);
	glUniform1f(materialShininessUniform15, materialShininess15);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 16  ////////////////////////////////

	
	//draw
	glViewport(updated_width * 3.0f / 4.0f, updated_height * 1.65 / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform16, 1, (GLfloat*)materialAmbient16);
	glUniform3fv(KdUniform16, 1, (GLfloat*)materialDiffuse16);
	glUniform3fv(KsUniform16, 1, (GLfloat*)materialSpecular16);
	glUniform1f(materialShininessUniform16, materialShininess16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	
	////////////////////////////  SPHERE 17  ////////////////////////////////

	glViewport(0, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform17, 1, (GLfloat*)materialAmbient17);
	glUniform3fv(KdUniform17, 1, (GLfloat*)materialDiffuse17);
	glUniform3fv(KsUniform17, 1, (GLfloat*)materialSpecular17);
	glUniform1f(materialShininessUniform17, materialShininess17);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 18  ////////////////////////////////

	glViewport(updated_width / 4, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform18, 1, (GLfloat*)materialAmbient18);
	glUniform3fv(KdUniform18, 1, (GLfloat*)materialDiffuse18);
	glUniform3fv(KsUniform18, 1, (GLfloat*)materialSpecular18);
	glUniform1f(materialShininessUniform18, materialShininess18);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	////////////////////////////  SPHERE 19  ////////////////////////////////

	glViewport(updated_width / 2, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform19, 1, (GLfloat*)materialAmbient19);
	glUniform3fv(KdUniform19, 1, (GLfloat*)materialDiffuse19);
	glUniform3fv(KsUniform19, 1, (GLfloat*)materialSpecular19);
	glUniform1f(materialShininessUniform19, materialShininess19);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 20  ////////////////////////////////

	glViewport(updated_width * 3.0f / 4.0f, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform20, 1, (GLfloat*)materialAmbient20);
	glUniform3fv(KdUniform20, 1, (GLfloat*)materialDiffuse20);
	glUniform3fv(KsUniform20, 1, (GLfloat*)materialSpecular20);
	glUniform1f(materialShininessUniform20, materialShininess20);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	
	////////////////////////////  SPHERE 21  ////////////////////////////////

	glViewport(0, updated_height *  -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform21, 1, (GLfloat*)materialAmbient21);
	glUniform3fv(KdUniform21, 1, (GLfloat*)materialDiffuse21);
	glUniform3fv(KsUniform21, 1, (GLfloat*)materialSpecular21);
	glUniform1f(materialShininessUniform21, materialShininess21);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 22  ////////////////////////////////

	glViewport(updated_width / 4, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform22, 1, (GLfloat*)materialAmbient22);
	glUniform3fv(KdUniform22, 1, (GLfloat*)materialDiffuse22);
	glUniform3fv(KsUniform22, 1, (GLfloat*)materialSpecular22);
	glUniform1f(materialShininessUniform22, materialShininess22);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 23  ////////////////////////////////

	glViewport(updated_width / 2, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform23, 1, (GLfloat*)materialAmbient23);
	glUniform3fv(KdUniform23, 1, (GLfloat*)materialDiffuse23);
	glUniform3fv(KsUniform23, 1, (GLfloat*)materialSpecular23);
	glUniform1f(materialShininessUniform23, materialShininess23);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	////////////////////////////  SPHERE 24  ////////////////////////////////

	//draw
	glViewport(updated_width * 3.0f / 4.0f, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
	glUniform3fv(KaUniform24, 1, (GLfloat*)materialAmbient24);
	glUniform3fv(KdUniform24, 1, (GLfloat*)materialDiffuse24);
	glUniform3fv(KsUniform24, 1, (GLfloat*)materialSpecular24);
	glUniform1f(materialShininessUniform24, materialShininess24);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element1);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//unbind vao
	glBindVertexArray(0);


	//stop using OpenGL program object
	glUseProgram(0);

	//ANIMATION LOGIC
	XRotation += 0.005f;
	YRotation += 0.005f;
	ZRotation += 0.005f;
	if (XRotation >= 360.0f)
	{
		XRotation = 0.0f;
	}
	if (YRotation >= 360.0f)
	{
		YRotation = 0.0f;
	}
	if (ZRotation >= 360.0f)
	{
		ZRotation = 0.0f;
	}
	
	//Double Buffer
	glXSwapBuffers(gpDisplay, gWindow);
}

void Uninitialize(void)
{
	//code
	//destroy vao
	if (gVao_sphere1)
	{
		glDeleteVertexArrays(1, &gVao_sphere1);
		gVao_sphere1 = 0;
	}

	//destroy vbo of position
	if (gVbo_sphere_position1)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_position1);
		gVbo_sphere_position1 = 0;
	}

	//destroy vbo of normal
	if (gVbo_sphere_normal1)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_normal1);
		gVbo_sphere_normal1 = 0;
	}

	//destroy vbo of element
	if (gVbo_sphere_element1)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_element1);
		gVbo_sphere_element1 = 0;
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

