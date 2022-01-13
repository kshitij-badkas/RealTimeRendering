// mkdir -p Graph_Paper.app/Contents/MacOS
// clang++ -Wno-deprecated-declarations -o Graph_Paper.app/Contents/MacOS/Graph_Paper Graph_Paper.mm -framework cocoa -framework QuartzCore -framework OpenGL

//Header files
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> //analogous to Windows.h and Xlib.h
#import <QuartzCore/CVDisplayLink.h> //audio-video core : CV-> Core Video
#import <OpenGL/gl3.h> // gl.h
#import "vmath.h"

//Callback function
CVReturn myDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *,
                                CVOptionFlags, CVOptionFlags *, void *);

using namespace vmath;

//Global variables
FILE *gpFile = NULL;

enum
{
    KAB_ATTRIBUTE_POSITION = 0,
    KAB_ATTRIBUTE_COLOR,
    KAB_ATTRIBUTE_NORMAL,
    KAB_ATTRIBUTE_TEXCOORD
};

//Forward Declaration
@interface AppDelegate:NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

//main
int main(int argc, char* arv[])
{
    //code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc] init]];

    //Message Loop or Run Loop or Event Loop
    [NSApp run];

    [pool release];

    return 0;
}

//When Application gets terminated ApplicationWillTerminate -> like WM_DESTROY


@interface MyOpenGLView:NSOpenGLView
@end

@implementation AppDelegate
{
    @private 
    NSWindow *window;
    MyOpenGLView *myOpenGLView;
}

//instance method
-(void) applicationDidFinishLaunching:(NSNotification *)aNotification
{
    //code
    //Create Log File
    NSBundle *appBundle = [NSBundle mainBundle]; //Class method //Application's package '.app'
    NSString *appDirPath = [appBundle bundlePath]; //instance method
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath = [NSString stringWithFormat:@"%@/Log.txt", parentDirPath];
    const char *pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    gpFile = fopen(pszLogFileNameWithPath, "w");
    
    if(gpFile == NULL)
    {
        [self release];
        [NSApp terminate:self];
    }
    
    fprintf(gpFile, "Program Started Successfully.\n\n");
    
    //NSPoint:x,y NSSize:w, h
    NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0); //AS it is a C library, it follows C syntax
    window = [[NSWindow alloc] initWithContentRect:win_rect
                styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | 
                NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable 
                backing:NSBackingStoreBuffered
                defer:NO];
    
    [window setTitle:@"KAB:Assignment_005_Perspective_Triangle"];
    [window center];
    
    myOpenGLView = [[MyOpenGLView alloc]initWithFrame:win_rect];
    [window setContentView:myOpenGLView];
    //was commented first
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

-(void) applicationWillTerminate:(NSNotification *)aNotification
{
    //code
    if(gpFile)
    {
        fprintf(gpFile, "\nProgram Terminated Successfully.\n");
    }
    fclose(gpFile);
    gpFile = NULL; 
}

-(void) windowWillClose:(NSNotification *)aNotification
{
    //code
    [NSApp terminate:self];
}

-(void) dealloc
{
    //code
    [myOpenGLView release];
    [window release];
    [super dealloc];
}
@end
//AppDelegate end


@implementation MyOpenGLView
{
    @private
    CVDisplayLinkRef displayLink;

    //OpenGL variables here
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;

    GLuint vao_Lines;
    GLuint vbo_Position_Lines;
    GLuint vbo_Color_Lines;

    GLuint mvpMatrixUniform;
    GLuint colorUniform;


    mat4 perspectiveProjectionMatrix;
}

-(id) initWithFrame:(NSRect)frame
{
    //code
    self = [super initWithFrame:frame];
    if(self)
    {
        //PixelFormatDescriptor - Win, FrameAttributes - *nix
        NSOpenGLPixelFormatAttribute attributes[] =
        {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery, //don't allow to choose software rendering automatically if hardware rendering is not possible
            NSOpenGLPFAAccelerated, //hardware rendering
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADoubleBuffer, 
            0
        };
        
        //ChoosePixelFormat
        NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
        if(pixelFormat == nil)
        {
            fprintf(gpFile, "Cannot get Pixel Format. Exitting.\n");
            [self release];
            [NSApp terminate:self];
        }
        
        //get openGL context
        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil] autorelease];
        
        //set
        [self setPixelFormat:pixelFormat];
        [self setOpenGLContext:glContext];
            
    }
    return self;
}

-(CVReturn) getFrameForTime:(const CVTimeStamp *)outputTime
{
    //code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; //create a new thread ->rendering thread -Multithreading
    [self drawView];
    [pool release];
    return kCVReturnSuccess;
}


//initialize
-(void) prepareOpenGL
{
    //code
    [super prepareOpenGL];

    //Objective C madhe get karaycha asel te direct lihaycha 'get' lihinyachi garaz nahi
    [[self openGLContext] makeCurrentContext];

    //OPENGL RELATED LOG
    fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
    fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
    fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    //swap interval - to avoid image tearing - monitor vertical sync and refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; //OpenGLCP - OpenGL Context Parameter

    //
    //from shaderSourceCode Initialize() code here
    //

    //GLSL #version 410 core
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    //provide source code to shader
    //use 'core' profile i.e PROGRAMMABLE PIPELINE
    const GLchar* vertexShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec4 vPosition;" \
        "uniform mat4 u_mvpMatrix;" \
        "void main(void)" \
        "{" \
        "gl_Position = u_mvpMatrix * vPosition;" \
        "}";

    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

    //compile shader
    glCompileShader(vertexShaderObject);
    
    //Shader Compilation Error Checking
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char* szInfoLog = NULL;
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)  //if Shader Compiled Status Failed
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release]; //Uninit()
                [NSApp terminate:self]; //exit()
            }
        }
    }

    //FRAGMENT SHADER
    //create shader
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    //provide source code to shader
    const GLchar* fragmentShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "out vec4 FragColor;" \
        "uniform vec4 u_Color;" \
        "void main(void)" \
        "{" \
        "FragColor = u_Color;" \
        "}";

    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

    //compile shader
    glCompileShader(fragmentShaderObject);

    //Shader Compilation Error Checking
    iInfoLogLength = 0;
    iShaderCompiledStatus = 0;
    szInfoLog = NULL;

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release]; //Uninit()
                [NSApp terminate:self]; //exit()
            }
        }
    }

    //SHADER PROGRAM
    //create
    shaderProgramObject = glCreateProgram();

    //attach vertex shader object to program 
    glAttachShader(shaderProgramObject, vertexShaderObject);

    //attach fragment shader object to program
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    //pre-link binding of shader program object with vertex shader attribute
    glBindAttribLocation(shaderProgramObject, KAB_ATTRIBUTE_POSITION, "vPosition");

    //pre-link binding of shader program object with fragment shader attribute
    glBindAttribLocation(shaderProgramObject, KAB_ATTRIBUTE_COLOR, "vColor");
    
    //link shader
    glLinkProgram(shaderProgramObject);

    //Shader Program Link Error Checking
    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release]; //Uninit()
                [NSApp terminate:self]; //exit()
            }
        }
    }

    //get MVP Uniform Location
    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");

    colorUniform = glGetUniformLocation(shaderProgramObject, "u_Color");


    //vertices, colours, shader attribs, vbo, vao initializations
    
    //vertices, colours, shader attribs, vbo, vao initialization
    
    //LINES
    //GEN START
    glGenVertexArrays(1, &vao_Lines);
    //BIND START
    glBindVertexArray(vao_Lines);

    //FOR LINES POSITION
    glGenBuffers(1, &vbo_Position_Lines);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); // 2 vertices with (x,y)

    //FOR LINES POSITION
    glGenBuffers(1, &vbo_Color_Lines);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 3 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); // 2 vertices with (x,y)
    
    //BIND END
    glBindVertexArray(0);


    //3D & Depth Code
    glClearDepth(1.0f);     //Default value passed to the Function
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //we will always cull back faces for beter performance
    glEnable(GL_CULL_FACE);

    //SetglColor
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black

    //set identity matrix to identity matrix
    perspectiveProjectionMatrix = mat4::identity();

    //Core Video and Core Graphics Library related code - CV and CGL
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &myDisplayLinkCallback, self);
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

//resize
-(void) reshape
{
    //code
    [super reshape];
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    NSRect rect = [self bounds];
    if(rect.size.height < 0)
    {
        rect.size.height = 1;
    }
    glViewport(0.0f, 0.0f, (GLsizei)rect.size.width, (GLsizei)rect.size.height);

    perspectiveProjectionMatrix = perspective(45.0f, 
                                            (GLfloat)rect.size.width / (GLfloat)rect.size.height, 
                                            0.1f, 
                                            100.0f);

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

//InvalidateRect - WM_PAINT - we use this for supporting multithreading 
-(void) drawRect:(NSRect)dirtyRect
{
    //code
    [self drawView];
}

//Display
-(void) drawView
{
    //code
    [[self openGLContext] makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    //variables
    //CO-ORDINATES
    GLfloat lines[2000];  //each line has 4 coordinates x 20 - 20 lines drawn - they will change coordinates
    GLfloat horizontalCentreLine[] = {-1.0f, 0.0f, 1.0f, 0.0f}; //central horizontal line
    GLfloat verticalCentreLine[] = { 0.0f, 1.0f, 0.0f, -1.0f }; //central vertical line

    GLfloat redColor[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat greenColor[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat blueColor[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat whiteColor[] = { 1.0, 1.0, 1.0, 1.0 };


    //code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //start using OpenGL program object
    glUseProgram(shaderProgramObject);

    //OpenGL Drawing
    mat4 translationMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    translationMatrix = translate(0.0f, 0.0f, -3.0f);
    modelViewMatrix = translationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //
    // 
    //
    
    //bind vao - vertex array object
    glBindVertexArray(vao_Lines);

    //HORIZONTAL LINES TOP
    GLint i = 0;
    GLfloat inc = 0.05f;
    for (i = 0; i < 80; i += 4, inc += 0.05f)
    {
        lines[i] = -1.0f; //x1
        lines[i + 1] = inc; //y1
        lines[i + 2] = 1.0f; //x2
        lines[i + 3] = inc; //y2
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUniform4fv(colorUniform, 1, (GLfloat*)blueColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 40); //40 indices to render for 20 lines


    //HORIZONTAL LINE CENTRE
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(GLfloat), horizontalCentreLine, GL_DYNAMIC_DRAW); // 2 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUniform4fv(colorUniform, 1, (GLfloat*)redColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 2); //2 indices to render

    
    //HORIZONTAL LINES BOTTOM
    i = 0;
    inc = 0.05f;
    for (i = 0; i < 80; i += 4, inc += 0.05f)
    {
        lines[i] = -1.0f; //x1
        lines[i + 1] = -inc; //y1
        lines[i + 2] = 1.0f; //x2
        lines[i + 3] = -inc; //y2

    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUniform4fv(colorUniform, 1, (GLfloat*)blueColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 40); //40 indices to render for 20 lines

    ///////////////////////
    //VERTICAL LINES LEFT
    i = 0;
    inc = 0.05f;
    for (i = 0; i < 80; i += 4, inc += 0.05f)
    {
        lines[i] = -inc; //x1
        lines[i + 1] = 1.0; //y1
        lines[i + 2] = -inc; //x2
        lines[i + 3] = -1.0; //y2
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUniform4fv(colorUniform, 1, (GLfloat*)blueColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 40); //40 indices to render for 20 lines

    //VERTICAL LINE CENTRE - green
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(GLfloat), verticalCentreLine, GL_DYNAMIC_DRAW); // 2 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glUniform4fv(colorUniform, 1, (GLfloat*)greenColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 2); //40 indices to render for 20 lines


    //VERTICAL LINES RIGHT
    i = 0;
    inc = 0.05f;
    for (i = 0; i < 80; i += 4, inc += 0.05f)
    {
        lines[i] = inc; //x1
        lines[i + 1] = 1.0; //y1
        lines[i + 2] = inc; //x2
        lines[i + 3] = -1.0; //y2

    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 40);

    glUniform4fv(colorUniform, 1, (GLfloat*)blueColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 40 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 40); //40 indices to render for 20 lines

    //
    // 
    //

    //TRAINGLE USING LINES
    lines[0] = 0.0f;
    lines[1] = 0.7f;
    lines[2] = 0.7f;
    lines[3] = -0.7f;

    lines[4] = 0.7f;
    lines[5] = -0.7f;
    lines[6] = -0.7f;
    lines[7] = -0.7f;
    
    lines[8] = -0.7f;
    lines[9] = -0.7f;
    lines[10] = 0.0f;
    lines[11] = 0.7f;

    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 2 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 6); //6 indices to render

    glUniform4fv(colorUniform, 1, (GLfloat*)whiteColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 6); //40 indices to render for 20 lines

    //
    // 
    //

    //RECTANGLE USING LINES
    lines[0] = 0.7f;
    lines[1] = 0.7f;
    lines[2] = 0.7f;
    lines[3] = -0.7f;

    lines[4] = 0.7f;
    lines[5] = 0.7f;
    lines[6] = -0.7f;
    lines[7] = 0.7f;

    lines[8] = -0.7f;
    lines[9] = 0.7f;
    lines[10] = -0.7f;
    lines[11] = -0.7f;

    lines[12] = -0.7f;
    lines[13] = -0.7f;
    lines[14] = 0.7f;
    lines[15] = -0.7f;

    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 8 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 8 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 8); //8 indices to render

    glUniform4fv(colorUniform, 1, (GLfloat*)whiteColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 8 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 8); //40 indices to render for 20 lines


    //
    // 
    //
    
    //CIRCUMCIRCLE USING POINTS
    GLfloat circumCentreRadius = 0.0f;

    circumCentreRadius = (GLfloat)sqrt(pow(0.7f, 2) + pow(0.7f, 2));
    fprintf(gpFile, "THE RADIUS OF CIRCUMCIRCLE IS = %f \n", circumCentreRadius);
    fprintf(gpFile, "THE CIRCUMFERENCE OF CIRCUMCIRCLE IS = %f \n", circumCentreRadius * 2.0f * 3.14f);
    //OUTPUT ->
    //THE RADIUS OF CIRCUMCIRCLE IS = 0.989949 
    //THE CIRCUMFERENCE OF CIRCUMCIRCLE IS = 6.216883


    i = 0;
    GLfloat cAngle = 0.0f;
    for (cAngle = 0.0f; cAngle < (2.0f * 3.14f); cAngle = cAngle + 0.01f, i += 2)
    {
        lines[i] = (GLfloat)cos(cAngle) * (GLfloat)circumCentreRadius; //xn
        lines[i + 1] = (GLfloat)sin(cAngle) * (GLfloat)circumCentreRadius; //yn
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 2000 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_POINTS, 0, 650); //1000 indices to render

    glUniform4fv(colorUniform, 1, (GLfloat*)whiteColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 650); //40 indices to render for 20 lines

    
    //
    // 
    //

    //INCIRCLE USING POINTS
    GLfloat inCircleRadius = 0.0f;
    GLfloat distance_A = 0.0f;
    GLfloat distance_B = 0.0f;
    GLfloat distance_C = 0.0f;
    GLfloat perimeter = 0.0f;
    GLfloat semi = 0.0f;
    GLfloat tArea = 0.0f;
    //centre of circle
    GLfloat iX = 0.0f;
    GLfloat iY = 0.0f;

    //  DISTANCE FORMULA
    distance_A = (GLfloat)sqrt(pow((-0.7f - 0.7f), 2) + pow((-0.7f - -0.7f), 2));
    distance_B = (GLfloat)sqrt(pow((0.0f - -0.7f), 2) + pow((0.7f - -0.7f), 2));
    distance_C = (GLfloat)sqrt(pow((0.7f - 0.0f), 2) + pow((-0.7f - 0.7f), 2));

    fprintf(gpFile, "a- %f\n", distance_A);
    fprintf(gpFile, "b- %f\n", distance_B);
    fprintf(gpFile, "c- %f\n", distance_C);

    //  SEMIPERIMETER
    perimeter = (GLfloat)(distance_A + distance_B + distance_C);
    fprintf(gpFile, "perimeter- %f\n", perimeter);
    semi = (GLfloat)(perimeter / 2);
    fprintf(gpFile, "semiperimeter- %f\n", semi);

    //  AREA OF TRIANGLE
    tArea = (GLfloat)sqrt(semi * (semi - distance_A) * (semi - distance_B) * (semi - distance_C));
    fprintf(gpFile, "Triangle Area- %f\n", tArea);

    //  INCIRCLE COORDINATES
    iX = (GLfloat)(distance_A * (0.0f) + distance_B * (0.7f) + distance_C * (-0.7f)) / perimeter;
    iY = (GLfloat)(distance_A * (0.7f) + distance_B * (-0.7f) + distance_C * (-0.7f)) / perimeter;

    fprintf(gpFile, "iX- %f\n", iX);
    fprintf(gpFile, "iY- %f\n", iY);


    inCircleRadius = (GLfloat)(tArea / semi);
    fprintf(gpFile, "THE RADIUS OF INCIRCLE IS = %f \n", inCircleRadius);
    fprintf(gpFile, "THE CIRCUMFERENCE OF INCIRCLE IS = %f \n", inCircleRadius * 2.0f * 3.14f);

    //OUTPUT ->
    
    i = 0;
    cAngle = 0.0f;
    for (cAngle = 0.0f; cAngle < (2.0f * 3.14f); cAngle = cAngle + 0.01f, i += 2)
    {
        lines[i] = iX + (GLfloat)cos(cAngle) * (GLfloat)inCircleRadius; //xn
        lines[i + 1] = iY + (GLfloat)sin(cAngle) * (GLfloat)inCircleRadius; //yn
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_Position_Lines);
    glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 2000 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_POINTS, 0, 650); //1000 indices to render

    glUniform4fv(colorUniform, 1, (GLfloat*)whiteColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 650); //40 indices to render for 20 lines

    glBindVertexArray(0);
    
    //-------------------------------draw end-------------------------------------//
    //stop using OpenGL program object
    glUseProgram(0);

    //Update() here

    //Swap Buffers / Double Buffers
    CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

//Respond to Keyboard and Mouse
-(BOOL) acceptsFirstResponder
{
    //code
    [[self window] makeFirstResponder:self];
    return YES;
}

-(void) keyDown:(NSEvent *)theEvent
{
    //code 
    int key = [[theEvent characters] characterAtIndex:0];
    switch(key)
    {
        case 27: 
            [self release];
            [NSApp terminate:self];
            break;
        
        case 'f':
        case 'F':
            [[self window] toggleFullScreen:self];
    }   
}

-(void) mouseDown:(NSEvent *)theEvent
{
    //code
}

-(void) rightMouseDown:(NSEvent *)theEvent
{
    //code
}

-(void) otherMouseDown:(NSEvent *)theEvent
{
    //code
}

-(void) dealloc
{
    //code
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);

    //Uninit() code here

    //destroy vao
    if (vao_Lines)
    {
        glDeleteVertexArrays(1, &vao_Lines);
        vao_Lines = 0;
    }

    //destroy vbo
    if (vbo_Position_Lines)
    {
        glDeleteVertexArrays(1, &vbo_Position_Lines);
        vbo_Position_Lines = 0;
    }

    //destroy vbo
    if (vbo_Color_Lines)
    {
        glDeleteVertexArrays(1, &vbo_Color_Lines);
        vbo_Color_Lines = 0;
    }

    //SAFE SHADER CLEANUP
    if (shaderProgramObject)
    {
        glUseProgram(shaderProgramObject);

        GLsizei shaderCount;
        glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

        GLuint* pShader = NULL;
        pShader = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

        if (pShader == NULL)
        {
            //error checking
            fprintf(gpFile, "Error Creating pShader.\n Exitting Now!!\n");
            [self release]; //Uninit()
            [NSApp terminate:self]; //exit()
        }

        glGetAttachedShaders(shaderProgramObject, shaderCount, &shaderCount, pShader);

        for (GLsizei i = 0; i < shaderCount; i++)
        {
            glDetachShader(shaderProgramObject, pShader[i]);
            glDeleteShader(pShader[i]);
            pShader[i] = 0;
        }

        free(pShader);

        glDeleteProgram(shaderProgramObject);
        shaderProgramObject = 0;

        //unlink
        glUseProgram(0);
    }

    [super dealloc];
}
@end


//As Callback fucntion is C style function it should be writtenin global space.
//Therefore we are wrting it after @end

//last parameter is MyOpenView class's pointer -recording : -1:05:00 to -1:15:00
CVReturn myDisplayLinkCallback(CVDisplayLinkRef displayLink, 
                                const CVTimeStamp *now, const CVTimeStamp *outputTime, 
                                CVOptionFlags flagsIn, CVOptionFlags *flagsOut, 
                                void *displayLinkContext)
{
    //code
    CVReturn result = [(MyOpenGLView *)displayLinkContext getFrameForTime:outputTime];
    return result;
}

