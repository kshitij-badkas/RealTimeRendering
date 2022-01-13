#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<math.h>

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

//global variables
bool bFullscreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColorMap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;

GLXContext gGlxContext;	//HGLRC

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
	static int frameBufferAttribute[] = { GLX_RGBA,
										  GLX_RED_SIZE, 1,
										  GLX_GREEN_SIZE, 1,
										  GLX_BLUE_SIZE, 1,
										  GLX_ALPHA_SIZE, 1,
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

    XStoreName(gpDisplay,gWindow,"FFP : Assignment_013_Graph_Paper_Shapes : by Kshitij Badkas");

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
	glClearColor(0.0f,0.0f,0.0f,1.0f); //BLACK 
	
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
	//prototypes
	void Draw_GraphPaper(void);
	void Draw_SquareAndCircumcircle(void);
	void Draw_TriangleAndIncircle(void);

	glClear(GL_COLOR_BUFFER_BIT);
	
	Draw_GraphPaper();
	Draw_SquareAndCircumcircle();
	Draw_TriangleAndIncircle();

	//Double Buffer
	glXSwapBuffers(gpDisplay, gWindow);
}


void Draw_GraphPaper(void)
{
	//code
	//variables
	float x_coord;
	float y_coord;
	float inc = 0.05f;

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	//	GRAPH PAPER
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glBegin(GL_LINES);
		
	// 20 VERTICAL LINES : BLUE
	glColor3f(0.0f, 0.0f, 1.0f);
	//LOOP +ve X-AXIS
	for (x_coord = inc; x_coord <= 1.05f; x_coord = x_coord + inc)
	{
		glVertex3f(x_coord, 1.0f, 0.0f);
		glVertex3f(x_coord, -1.0f, 0.0f);
	}

	// 20 VERTICAL LINES : BLUE 
	glColor3f(0.0f, 0.0f, 1.0f);
	//LOOP -ve X-AXIS
	for (x_coord = inc; x_coord <= 1.05f; x_coord = x_coord + inc)
	{
		glVertex3f(-x_coord, -1.0f, 0.0f);
		glVertex3f(-x_coord, 1.0f, 0.0f);
	}

	// 20 HORIZONTAL LINES : BLUE
	glColor3f(0.0f, 0.0f, 1.0f);
	//LOOP +ve Y-AXIS
	for (y_coord = inc; y_coord <= 1.05f; y_coord = y_coord + inc)
	{
		glVertex3f(-1.0f, y_coord, 0.0f);
		glVertex3f(1.0f, y_coord, 0.0f);
	}

	// HORIZONTAL RED 
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	

	// 20 HORIZONTAL LINES : BLUE
	glColor3f(0.0f, 0.0f, 1.0f);
	//LOOP -ve Y-AXIS
	for (y_coord = inc; y_coord <= 1.05f; y_coord = y_coord + inc)
	{
		glVertex3f(-1.0f, -y_coord, 0.0f);
		glVertex3f(1.0f, -y_coord, 0.0f);
	}
	
	// VERTICAL GREEN
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	
	glEnd();
	
}

void Draw_SquareAndCircumcircle()
{
	GLfloat radius = 0.0f;
	GLfloat cAngle = 0.0f;

	// YELLOW SQUARE
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);

	glColor3f(1.0f, 1.0f, 0.0f);

	glVertex3f(0.7f, 0.7f, 0.0f);
	glVertex3f(0.7f, -0.7f, 0.0f);

	glVertex3f(0.7f, 0.7f, 0.0f);
	glVertex3f(-0.7f, 0.7f, 0.0f);

	glVertex3f(-0.7f, 0.7f, 0.0f);
	glVertex3f(-0.7f, -0.7f, 0.0f);

	glVertex3f(-0.7f, -0.7f, 0.0f);
	glVertex3f(0.7f, -0.7f, 0.0f);

	glEnd();

	//radius = (0.7f / (2 * sin(3.14f / 4))) * 2;
	radius = sqrt(pow(0.7f, 2) + pow(0.7f, 2));

	// CIRCUMCIRCLE
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 0.0f);
	for (cAngle = 0.0f; cAngle <= (2 * 3.14f); cAngle = cAngle + 0.01f)
	{
		glVertex3f(cos(cAngle) * radius, (sin(cAngle) * radius), 0.0f);
	}
	glEnd();

}

void Draw_TriangleAndIncircle()
{
	GLfloat distance_A = 0.0f;
	GLfloat distance_B = 0.0f;
	GLfloat distance_C = 0.0f;
	GLfloat perimeter = 0.0f;
	GLfloat semi = 0.0f;
	GLfloat tArea = 0.0f;
	GLfloat cAngle = 0.0f;
	GLfloat iX = 0.0f;
	GLfloat iY = 0.0f;
	GLfloat radius = 0.0f;

	//	TRIANGLE	
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 0.0f);

	glVertex3f(0.0f, 0.7f, 0.0f);	//A
	glVertex3f(0.7f, -0.7f, 0.0f);

	glVertex3f(0.7f, -0.7f, 0.0f);	//B
	glVertex3f(-0.7f, -0.7f, 0.0f);

	glVertex3f(-0.7f, -0.7f, 0.0f);	//C
	glVertex3f(0.0f, 0.7f, 0.0f);

	//	DISTANCE FORMULA
	distance_A = (GLfloat)sqrt(pow((-0.7f - 0.7f), 2) + pow((-0.7f - -0.7f), 2));
	distance_B = (GLfloat)sqrt(pow((0.0f - -0.7f), 2) + pow((0.7f - -0.7f), 2));
	distance_C = (GLfloat)sqrt(pow((0.7f - 0.0f), 2) + pow((-0.7f - 0.7f), 2));

	//	SEMIPERIMETER
	perimeter = (GLfloat)(distance_A + distance_B + distance_C);
	semi = (GLfloat)(perimeter / 2);

	//	AREA OF TRIANGLE
	tArea = (GLfloat)sqrt(semi * (semi - distance_A) * (semi - distance_B) * (semi - distance_C));

	//	INCIRCLE COORDINATES
	iX = (GLfloat)(distance_A * (0.0f) + distance_B * (0.7f) + distance_C * (-0.7f)) / perimeter;
	iY = (GLfloat)(distance_A * (0.7f) + distance_B * (-0.7f) + distance_C * (-0.7f)) / perimeter;

	//	RADIUS
	radius = (GLfloat)(tArea / semi);

	glEnd();

	//	INCIRCLE 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(iX, iY, -3.0f);
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 0.0f);
	for (cAngle = 0.0f; cAngle <= (2 * 3.14f); cAngle = cAngle + 0.01f)
	{
		glVertex3f(cos(cAngle) * radius, (sin(cAngle) * radius), 0.0f);
	}
	glEnd();
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

    if(gpDisplay)
    {
        XCloseDisplay(gpDisplay);
        gpDisplay=NULL;
    }
}


