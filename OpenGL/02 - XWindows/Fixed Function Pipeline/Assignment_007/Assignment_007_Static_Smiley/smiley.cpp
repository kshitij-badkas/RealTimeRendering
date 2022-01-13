#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

//NEW
//PATH -> /usr/include/GL
#include<GL/gl.h>	//FOR OPENGL APIs
#include<GL/glx.h>	//FOR GLX APIs

//ortho
#include<GL/glu.h>

//SOIL 
#include<SOIL/SOIL.h>

//namespaces
using namespace std;

//global variable declaration
bool bFullscreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColorMap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;

//NEW
GLXContext gGlxContext;	//HGLRC

//Textures
GLuint smiley_texture;

//main()
int main(void)
{
    //prototypes
    void CreateWindow(void);
    void ToggleFullscreen(void);
    void Unitialize(void);

    //NEW
    void Initialize(void);
    void Resize(int, int);
    void display(void);
    
    //variable declarations
    int winWidth=giWindowWidth;
    int winHeight=giWindowHeight;
	
	//NEW
	bool bDone = false;

    //code
    CreateWindow();
    
    //NEW
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
					//NEW
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
				//NEW
				Resize(winWidth, winHeight);
				break;
			
			case Expose:
				break;

			case DestroyNotify:
				break;
			
			case 33:
				//NEW
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

	//NEW
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

    XStoreName(gpDisplay,gWindow,"FFP: Assignment_007_Static_Smiley : by Kshitij Badkas");

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
	
	//texture
	GLuint LoadBitmapAsTexture(const char *);
	
	//code
	//NEW
	gGlxContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGlxContext);
	
	//3D & Depth Code
	glShadeModel(GL_SMOOTH);  //anti-aliasing
	glClearDepth(1.0f);    //Default value passed to the Function
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	//texture
	glEnable(GL_TEXTURE_2D);
	smiley_texture = LoadBitmapAsTexture("Smiley.bmp");
	
	glClearColor(0.0f,0.0f,1.0f,1.0f); //BLUE 
	
	//WARM-UP CALL
	Resize(giWindowWidth, giWindowHeight);
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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	//Setting texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	
	SOIL_free_image_data(imageData);
	
	return textureID;
	
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
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  	
  	glMatrixMode(GL_MODELVIEW);
  	glLoadIdentity();

	glTranslatef(0.0f,0.0f,-3.0f);
	//glBindTexture(GL_TEXTURE_2D, smiley_texture);
	
  	//FULL SMILEY
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();

	//Double Buffer
	glXSwapBuffers(gpDisplay, gWindow);
}

void Unitialize(void)
{
	//code
	//NEW 
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
    
    if(smiley_texture)
    {
    	glDeleteTextures(1,&smiley_texture);
    	smiley_texture=0;
    }
}

