// mkdir -p Interleaved.app/Contents/MacOS
// clang++ -Wno-deprecated-declarations -o Interleaved.app/Contents/MacOS/Interleaved Interleaved.mm -framework cocoa -framework QuartzCore -framework OpenGL

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
    
    [window setTitle:@"KAB:Assignment_027_Interleaved"];
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

    GLuint vao;
    GLuint vbo;

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    //LIGHT UNIFORMS - ALL ARRAYS
    GLuint LaUniform; //ambient light component
    GLuint LdUniform; //diffuse light component
    GLuint LsUniform; //specular light component
    GLuint lightPositionUniform; //light position

    //MATERIAL UNIFORMS - Ka, Kd, Ks ARRAYS, shininess is a single value
    GLuint KaUniform; //ambient material component
    GLuint KdUniform; //diffuse material component
    GLuint KsUniform; //specular material component
    GLuint materialShininessUniform; //shininess material
    
    //LIGHT KEY IS PRESSED UNIFORM
    GLuint LKeyPressedUniform;

    //TEXTURES
    GLuint marble_texture;

    GLuint textureSamplerUniform;
    
    mat4 perspectiveProjectionMatrix;

    bool bAnimate, bLights;

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
        "in vec4 vColor;" \
        "in vec3 vNormal;" \
        "in vec2 vTexCoord;" \
        "uniform mat4 u_model_matrix;" \
        "uniform mat4 u_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform int u_L_KEY_PRESSED;" \
        "uniform vec3 u_LA;" \
        "uniform vec3 u_LD;" \
        "uniform vec3 u_LS;" \
        "uniform vec4 u_light_position;" \
        "uniform vec3 u_KA;" \
        "uniform vec3 u_KD;" \
        "uniform vec3 u_KS;" \
        "uniform float u_material_shininess;" \
        "out vec4 out_color;" \
        "out vec3 phong_ads_light;" \
        "out vec2 out_texcoord;" \
        "void main(void)" \
        "{" \
            "if(u_L_KEY_PRESSED == 1)" \
            "{" \
                "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
                "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
                "vec3 light_direction = normalize(vec3(u_light_position - eye_coordinates));" \
                "vec3 reflection_vector = reflect(-light_direction, transformed_normal);" \
                //SWIZZLING
                "vec3 view_vector = normalize(-eye_coordinates.xyz);" \
                "vec3 ambient = u_LA * u_KA;" \
                "vec3 diffuse = u_LD * u_KD * max(dot(light_direction, transformed_normal), 0.0f);" \
                "vec3 specular = u_LS * u_KS * pow(max(dot(reflection_vector, view_vector), 0.0f), u_material_shininess);" \
                "phong_ads_light = ambient + diffuse + specular;" \
            "}" \
            "else" \
            "{" \
                "phong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
            "}" \
            "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
            "out_color = vColor;" \
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
        "in vec4 out_color;" \
        "in vec3 phong_ads_light;" \
        "in vec2 out_texcoord;" \
        "uniform sampler2D u_texture_sampler;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "vec3 tex = vec3(texture(u_texture_sampler, out_texcoord));" \
        "FragColor = vec4(vec3(out_color) * phong_ads_light * tex, 1.0f);" \
        "}";

        // chnages - recording - 1:45:00 onwards
        //for normals - phong_ads_light
        //FragColor = vec4(vec3(out_color * phong_ads_light), 1.0f);

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
    
    //pre-link binding of shader program object with fragment shader attribute
    glBindAttribLocation(shaderProgramObject, KAB_ATTRIBUTE_NORMAL, "vNormal");

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

    //get Uniform Location
    modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
    LaUniform = glGetUniformLocation(shaderProgramObject, "u_LA");
    LdUniform = glGetUniformLocation(shaderProgramObject, "u_LD");
    LsUniform = glGetUniformLocation(shaderProgramObject, "u_LS");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position");
    KaUniform = glGetUniformLocation(shaderProgramObject, "u_KA");
    KdUniform = glGetUniformLocation(shaderProgramObject, "u_KD");
    KsUniform = glGetUniformLocation(shaderProgramObject, "u_KS");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
    LKeyPressedUniform = glGetUniformLocation(shaderProgramObject, "u_L_KEY_PRESSED");

    //vertices, colours, shader attribs, vbo, vao initializations
    //CUBE
    //24 rows and 11 columns  = 264 member arrays
    const GLfloat cubePCNT[24][11] = 
    {
        //x,y,z, r,g,b, x,y,z, s,t

        //FRONT
        {1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},

        //RIGHT
        {1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        {1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
        
        //BACK
        {-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f},
        {-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f},
        
        //LEFT
        {-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        {-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
        
        //TOP
        {1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
        {-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
        {-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        
        //BOTTOM
        {1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f},
        {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f}
    }; 

    //GEN START
    glGenVertexArrays(1, &vao);
    //BIND START
    glBindVertexArray(vao);

    //FOR VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * 11 * sizeof(GLfloat), cubePCNT, GL_STATIC_DRAW);
    
    //1. POSITION
    //11 chya dhanga takaychya - 2nd vertices cha data 11 cha nanatar milnar
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);

    //2. COLOR
    //11 chya dhanga takaychya - 2nd colors cha data 11 cha nanatar milnar - 6th param 3*sizeof(Glflaot) sodla ki colors cha rgb milel
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);

    //3. NORMAL
    //11 chya dhanga takaychya - 2nd normal cha data 11 cha nanatar milnar - 6th param 6*sizeof(Glflaot) sodla ki normals cha rgb milel
    glVertexAttribPointer(KAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(KAB_ATTRIBUTE_NORMAL);
    
    //4. TEXCOORDS
    //11 chya dhanga takaychya - 2nd texcoords cha data 11 cha nanatar milnar - 6th param 9*sizeof(Glflaot) sodla ki texcoords cha rgb milel
    glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(9 * sizeof(GLfloat)));
    glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);

    //UNBIND VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //UNBIND VAO
    glBindVertexArray(0);


    //3D & Depth Code
    glClearDepth(1.0f);     //Default value passed to the Function
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //Disable CULLing while animation
    glDisable(GL_CULL_FACE);

    //SetglColor
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black

    //lights and animation
    bAnimate = false;
    bLights = false;

    //Load textures
    marble_texture = [self loadGLTexture:"marble.bmp"];

    //Enable TExtures
    glEnable(GL_TEXTURE_2D);

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
    GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f ,1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };
    GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f ,1.0f };
    GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f ,1.0f };
    GLfloat materialShininess = 50.0f;
    static GLfloat angleCube = 0.0f;

    [[self openGLContext] makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Display() here
    
    //start using OpenGL program object
    glUseProgram(shaderProgramObject);

    if (bLights == true)
    {
        glUniform1i(LKeyPressedUniform, 1);
        glUniform3fv(LaUniform, 1, (GLfloat*)lightAmbient);
        glUniform3fv(LdUniform, 1, (GLfloat*)lightDiffuse);
        glUniform3fv(LsUniform, 1, (GLfloat*)lightSpecular);
        glUniform3fv(KaUniform, 1, (GLfloat*)materialAmbient);
        glUniform3fv(KdUniform, 1, (GLfloat*)materialDiffuse);
        glUniform3fv(KsUniform, 1, (GLfloat*)materialSpecular);
        glUniform1f(materialShininessUniform, materialShininess);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    //OpenGL Drawing
    mat4 modelMatrix = mat4::identity();
    mat4 translationMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();


    //CUBE
    translationMatrix = translate(0.0f, 0.0f, -6.0f);
    scaleMatrix = scale(0.85f, 0.85f, 0.85f);
    rotationMatrix = rotate(angleCube, angleCube, angleCube);
    modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, marble_texture);
    glUniform1i(textureSamplerUniform, 0);

    //bind vao - vertex array object
    glBindVertexArray(vao);

    //draw
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

    //Update() here
    //Animation
    if (bAnimate == true)
    {
        angleCube += 1.0f;
        if (angleCube >= 360.0f)
        {
            angleCube = 0.0f;
        }
    }


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
            break;

        //Animation
        case 'A':
        case 'a':
            if (bAnimate == true)
            {
                bAnimate = false;
            }
            else if (bAnimate == false)
            {
                bAnimate = true;
            }
            break;
        
        //Light
        case 'L':
        case 'l':
            if (bLights == true)
            {
                bLights = false;
            }
            else if (bLights == false)
            {
                bLights = true;
            }
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
    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    //destroy vbo
    if (vbo)
    {
        glDeleteVertexArrays(1, &vbo);
        vbo = 0;
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

