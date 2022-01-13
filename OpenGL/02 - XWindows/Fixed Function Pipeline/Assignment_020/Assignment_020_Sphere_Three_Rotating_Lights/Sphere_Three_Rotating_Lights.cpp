#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

//PATH -> /usr/include/GL
#include<GL/gl.h>	//FOR OPENGL APIs
#include<GL/glx.h>	//FOR GLX APIs
#include<GL/glu.h>

//namespaces
using namespace std;

//global variable declarations
bool bFullscreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColorMap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;
GLXContext gGlxContext;	//HGLRC

//Lights
bool gbLight = false;

GLfloat lightAmbient_Zero[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse_Zero[] = { 1.0f,0.0f,0.0f,1.0f }; //Red
GLfloat lightSpecular_Zero[] = { 1.0f,0.0f,0.0f,1.0f }; //Red
GLfloat lightPosition_Zero[] = { 0.0f,0.0f,0.0f,1.0f }; //Positional Light

GLfloat lightAmbient_One[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse_One[] = { 0.0f,1.0f,0.0f,1.0f }; //Green
GLfloat lightSpecular_One[] = { 0.0f,1.0f,0.0f,1.0f }; //Green
GLfloat lightPosition_One[] = { 0.0f,0.0f,0.0f,1.0f }; //Positional Light

GLfloat lightAmbient_Two[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse_Two[] = { 0.0f,0.0f,1.0f,1.0f }; //Blue
GLfloat lightSpecular_Two[] = { 0.0f,0.0f,1.0f,1.0f }; //Blue
GLfloat lightPosition_Two[] = { 0.0f,0.0f,0.0f,1.0f }; //Positional Light

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f }; //White Material
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 128.0f;

GLfloat lightAngle0 = 0.0f;
GLfloat lightAngle1 = 0.0f;
GLfloat lightAngle2 = 0.0f;

GLUquadric* quadric = NULL;

//main()
int main(void)
{
    //prototypes
    void CreateWindow(void);
    void ToggleFullscreen(void);
    void Unitialize(void);
    void Initialize(void);
    void Resize(int, int);
    void display(void);
    
    //variable declarations
    int winWidth=giWindowWidth;
    int winHeight=giWindowHeight;
	bool bDone = false;

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
				    
				 //
				case XK_L:
				case XK_l:
					if (gbLight == false)
					{
						gbLight = true;
						glEnable(GL_LIGHTING);
					}
					else
					{
						if (gbLight == true)
						{
							gbLight = false;
							glDisable(GL_LIGHTING);
						}
						
					}
					break;	 
				 //  
				
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

    Unitialize();

    return(0);

}


void CreateWindow(void)
{
    //prototype
    void Unitialize(void);

    //variable declarations
    XSetWindowAttributes winAttribs;
    int defaultScreen;
    int styleMask;

	static int frameBufferAttribute[] = { GLX_DOUBLEBUFFER, True, 
										  GLX_RGBA,
										  GLX_RED_SIZE, 8,
										  GLX_GREEN_SIZE, 8,
										  GLX_BLUE_SIZE, 8,
										  GLX_ALPHA_SIZE, 8,
										  //Depth
										  GLX_DEPTH_SIZE, 24, //video for Linux (V4L) recommends 24 instead of 32 
										  None //Here we can say 0 as well										  
										}; //PixelFormatAttribute
	//static because it is a convention
	
    //code
    gpDisplay=XOpenDisplay(NULL);
    if(gpDisplay==NULL)
    {
        printf("ERROR : Unable to open X Display. \n Exitting Now...\n");
        Unitialize();
        exit(1);
    }

    defaultScreen=XDefaultScreen(gpDisplay);

	//glxChooseVisual -> Bridging API
    gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttribute);
    if(gpXVisualInfo==NULL)
    {
        printf("ERROR : Unable to Allocate Memory for Visual Info.\n Exitting Now...\n");
        Unitialize();
        exit(1);
    }

    //XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo);

    if(gpXVisualInfo==NULL)
    {
        printf("ERROR : Unable to Get A Visual.\n Exitting Now...\n");
        Unitialize();
        exit(1);
    }

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
        Unitialize();
        exit(1);
    }

    XStoreName(gpDisplay,gWindow,"FFP : Assignment_020_Sphere_Three_Rotating_Lights : by Kshitij Badkas");

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
	
	//code
	gGlxContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGlxContext);
	
	//3D & Depth Code
	glShadeModel(GL_SMOOTH);  //anti-aliasing
	glClearDepth(1.0f);    //Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	
	//Initialization of light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient_Zero);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse_Zero);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular_Zero);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient_One);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse_One);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular_One);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient_Two);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse_Two);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular_Two);
	glEnable(GL_LIGHT2);

	//Material Initialization
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
	glEnable(GL_LIGHT1);
	
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f,100.0f);
}

void display(void)
{
	//code
	//Gaurad's shading
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//3D & Depth Code
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	
	gluLookAt(0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();

	glRotatef(lightAngle0, 1.0f, 0.0f, 0.0f);
	lightPosition_Zero[1] = lightAngle0;
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition_Zero);
	glPopMatrix();
	glPushMatrix();

	glRotatef(lightAngle1, 0.0f, 1.0f, 0.0f);
	lightPosition_One[0] = lightAngle1;
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition_One);
	glPopMatrix();
	glPushMatrix();

	glRotatef(lightAngle2, 0.0f, 0.0f, 1.0f);
	lightPosition_Two[0] = lightAngle2;
	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition_Two);
	glPopMatrix();

	glTranslatef(0.0f, 0.0f, -1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	quadric = gluNewQuadric();
	gluSphere(quadric, 0.5f, 30, 30);
	glPopMatrix();
	//Normals are calculated using gluSphere itself so we do not need glBegin and glEnd

	lightAngle0 = lightAngle0 + 0.5f;
	if (lightAngle0 >= 360.0f)
	{
		lightAngle0 = 0.5f;
	}

	lightAngle1 = lightAngle1 + 0.5f;
	if (lightAngle1 >= 360.0f)
	{
		lightAngle1 = 0.5f;
	}

	lightAngle2 = lightAngle2 + 0.5f;
	if (lightAngle2 >= 360.0f)
	{
		lightAngle2 = 0.5f;
	}
	
	//Double Buffer
	glXSwapBuffers(gpDisplay, gWindow);
}


void Unitialize(void)
{
	//code
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
    
    if (quadric)
	{
		gluDeleteQuadric(quadric);
		quadric = NULL;
	}

    if(gpDisplay)
    {
        XCloseDisplay(gpDisplay);
        gpDisplay=NULL;
    }
}

