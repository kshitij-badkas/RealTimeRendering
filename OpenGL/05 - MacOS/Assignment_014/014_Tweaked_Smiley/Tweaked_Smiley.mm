// mkdir -p Tweaked_Smiley.app/Contents/MacOS
// clang++ -Wno-deprecated-declarations -o Tweaked_Smiley.app/Contents/MacOS/Tweaked_Smiley Tweaked_Smiley.mm -framework cocoa -framework QuartzCore -framework OpenGL

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
int main(int argc, char* argv[])
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
    
    [window setTitle:@"KAB:Assignment_014_Tweaked_Smiley"];
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

    GLuint vao_square;
    GLuint vbo_position_square;
    GLuint vbo_color_square;
    GLuint vbo_texture_square;
    
    GLuint smiley_texture;

    GLuint textureSamplerUniform;
    GLuint otherKeyPressedUniform;
    GLuint mvpMatrixUniform;

    mat4 perspectiveProjectionMatrix;

    unsigned int pressed_key;// = 0;
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
        "in vec2 vTexCoord;" \
        "uniform mat4 u_mvpMatrix;" \
        "out vec2 out_texcoord;" \
        "void main(void)" \
        "{" \
        "gl_Position = u_mvpMatrix * vPosition;" \
        "out_texcoord = vTexCoord;" \
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
    glBindAttribLocation(shaderProgramObject, KAB_ATTRIBUTE_TEXCOORD, "vTexCoord");
    
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

    //get Texture Sampler Uniform Location
    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_texture_sampler");

    //if other key is pressed
    otherKeyPressedUniform = glGetUniformLocation(shaderProgramObject, "u_other_key_pressed");

    //vertices, colours, shader attribs, vbo, vao initializations
    //SQUARE
    const GLfloat squareVertices[] =
    {
        1.0f, 1.0f, 0.0f, //right-top
        -1.0f, 1.0f, 0.0f, //left-top
        -1.0f, -1.0f, 0.0f, //left-bottom
        1.0f, -1.0f, 0.0f //right-bottom
    };
 
    //GEN START
    glGenVertexArrays(1, &vao_square);
    //BIND START
    glBindVertexArray(vao_square);

    //FOR PYRAMID POSITION
    glGenBuffers(1, &vbo_position_square);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //FOR SQUARE TEXCOORD
    glGenBuffers(1, &vbo_texture_square);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_square);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); //new
    glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
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

    //Load textures
    smiley_texture = [self loadGLTexture:"Smiley.bmp"];

    //Enable TExtures
    //glEnable(GL_TEXTURE_2D);

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

-(GLuint)loadGLTexture :(const char *)imageFilename
{
    //code
    NSBundle *appBundle = [NSBundle mainBundle];
    NSString *appDirPath = [appBundle bundlePath];
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *imageFilenameWithPath = [NSString stringWithFormat:@"%@/%s", parentDirPath, imageFilename];

    //1 
    //Get NSImage representattion of image file, do error check
    NSImage *bmpImage = [[NSImage alloc] initWithContentsOfFile:imageFilenameWithPath];
    if(!bmpImage)
    {
        fprintf(gpFile, "NSImage Conversion of Image File Failed.\n");
        return (0);
    }

    //2
    //Get CGImage representation of NSImage
    CGImageRef cgImage = [bmpImage CGImageForProposedRect:nil context:nil hints:nil];
    if(!cgImage)
    {
        fprintf(gpFile, "CGImage REpresentation of NSImage Failed.\n");
        return (0);
    }

    //3
    //Get width and height of CGImage representation of the image
    int imageWidth = (int)CGImageGetWidth(cgImage);
    int imageHeight = (int)CGImageGetHeight(cgImage);

    //4
    //Get CFData represnted data of image
    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    if(!imageData)
    {
        fprintf(gpFile, "CFData Represented data of CGImage Failed.\n");
        return (0);
    }
    //5
    //Convert CFDataRef data into void* generic format data
    void *pixels = (void *)CFDataGetBytePtr(imageData); //ptr to CFDataRef imageData

    GLuint texture;
    //OPENGL TEXTURE CODE STARTS HERE
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //make 2nd parameter '1' in PP instead of '4' for Faster Performance.

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //Setting texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //Push the Data into Graphics Memory with the Help of Graphics Driver
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    //6
    //release imageData
    CFRelease (imageData);

    //7
    //return the texture
    return texture;
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
    //variables
    static GLfloat texCoord[8] = { 0 };

    [[self openGLContext] makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Display() here
    
    //start using OpenGL program object
    glUseProgram(shaderProgramObject);

    //OpenGL Drawing
    mat4 translationMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    //SQUARE
    translationMatrix = translate(0.0f, 0.0f, -6.0f);
    modelViewMatrix = translationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, smiley_texture);
    glUniform1i(textureSamplerUniform, 0);

    //bind vao - vertex array object
    glBindVertexArray(vao_square);

    //draw
    //other key not pressed and 0 < pressed_key < 4
    glUniform1i(otherKeyPressedUniform, 0);
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
        glUniform1i(otherKeyPressedUniform, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_square);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), texCoord, GL_DYNAMIC_DRAW); //new
    glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //drawing
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //unbind vao
    glBindVertexArray(0);

    //stop using OpenGL program object
    glUseProgram(0);

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

        case 51: //'3' is pressed
            pressed_key = 3;
            fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
            glEnable(GL_TEXTURE_2D);
            break;

        case 52: //'4' is pressed
            pressed_key = 4;
            fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
            glEnable(GL_TEXTURE_2D);
            break;

        case 'f':
        case 'F':
            [[self window] toggleFullScreen:self];
            break;

        default:
            pressed_key = key;
            fprintf(gpFile, "\n Pressed Digit = %d\n glDisable Texture", pressed_key);
            glDisable(GL_TEXTURE_2D);
            break;
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
    if (vao_square)
    {
        glDeleteVertexArrays(1, &vao_square);
        vao_square = 0;
    }

    //destroy vbo
    if (vbo_position_square)
    {
        glDeleteVertexArrays(1, &vbo_position_square);
        vbo_position_square = 0;
    }

    //destroy vbo od texture
    if (vbo_texture_square)
    {
        glDeleteVertexArrays(1, &vbo_texture_square);
        vbo_texture_square = 0;
    }

    //delete texture
    if (smiley_texture)
    {
        glDeleteTextures(1, &smiley_texture);
        smiley_texture = 0;
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

