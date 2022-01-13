// mkdir -p Per_Vertex_Fragment_Toggle.app/Contents/MacOS
// clang++ -Wno-deprecated-declarations -o Per_Vertex_Fragment_Toggle.app/Contents/MacOS/Per_Vertex_Fragment_Toggle Per_Vertex_Fragment_Toggle.mm -framework cocoa -framework QuartzCore -framework OpenGL

//Header files
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> //analogous to Windows.h and Xlib.h
#import <QuartzCore/CVDisplayLink.h> //audio-video core : CV-> Core Video
#import <OpenGL/gl3.h> // gl.h
#import "vmath.h"

#import "Sphere.m"

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
    
    [window setTitle:@"KAB:Assignment_020_Per_Vertex_Fragment_Toggle_Light_Shading"];
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
    GLuint vao_sphere;
    GLuint vbo_sphere_position;
    GLuint vbo_sphere_normal;
    GLuint vbo_sphere_element;

    GLuint mvpMatrixUniform;

    mat4 perspectiveProjectionMatrix;

    float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[764];
    short sphere_elements[2280];
    int numVertices;
    int numElements; //Elements a.k.a Indices

    bool bLights;
    
    GLuint VKeyIsPressed; //Per-Vertex Lightling Toggle
    GLuint FKeyIsPressed; //Per-Fragment Lightling Toggle

    //----------------------------------------------//
    //PER-VERTEX LIGHTING 
    //MODEL-VIEW-PROJECTION UNIFORMS
    GLuint modelMatrixUniform_PV;
    GLuint viewMatrixUniform_PV;
    GLuint projectionMatrixUniform_PV;

    //LIGHT UNIFORMS - ALL ARRAYS
    GLuint LaUniform_PV; //ambient light component
    GLuint LdUniform_PV; //diffuse light component
    GLuint LsUniform_PV; //specular light component
    GLuint lightPositionUniform_PV; //light position

    //MATERIAL UNIFORMS - Ka, Kd, Ks ARRAYS, shininess is a single value
    GLuint KaUniform_PV; //ambient material component
    GLuint KdUniform_PV; //diffuse material component
    GLuint KsUniform_PV; //specular material component
    GLuint materialShininessUniform_PV; //shininess material

    //per-vertex shader objects
    GLuint vertexShaderObject_PV;
    GLuint fragmentShaderObject_PV;
    GLuint shaderProgramObject_PV;

    GLuint LKeyPressedUniform_PV; //L key is pressed

    //----------------------------------------------//

    //----------------------------------------------//
    //PER-FRAGMENT LIGHTING
    //MODEL-VIEW-PROJECTION UNIFORMS
    GLuint modelMatrixUniform_PF;
    GLuint viewMatrixUniform_PF;
    GLuint projectionMatrixUniform_PF;

    //LIGHT UNIFORMS - ALL ARRAYS
    GLuint LaUniform_PF; //ambient light component
    GLuint LdUniform_PF; //diffuse light component
    GLuint LsUniform_PF; //specular light component
    GLuint lightPositionUniform_PF; //light position

    //MATERIAL UNIFORMS - Ka, Kd, Ks ARRAYS, shininess is a single value
    GLuint KaUniform_PF; //ambient material component
    GLuint KdUniform_PF; //diffuse material component
    GLuint KsUniform_PF; //specular material component
    GLuint materialShininessUniform_PF; //shininess material

    //per-fragemnt shader objects
    GLuint vertexShaderObject_PF;
    GLuint fragmentShaderObject_PF;
    GLuint shaderProgramObject_PF;

    GLuint LKeyPressedUniform_PF; //L key is pressed

    //----------------------------------------------//

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
    //prototypes
    void getSphereVertexData(float[], float[], float[], short[]);
    int getNumberOfSphereVertices(void);
    int getNumberOfSphereElements(void);

    //void getPerVertexLighting(void);
    //void getPerFragmentLighting(void);

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

    //PER-VERTEX
    [self getPerVertexLighting];
    //PER-FRAGMENT
    [self getPerFragmentLighting];


    //vertices, colours, shader attribs, vbo, vao initializations
    getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    numVertices = getNumberOfSphereVertices();
    numElements = getNumberOfSphereElements();

    glGenVertexArrays(1, &vao_sphere);
    glBindVertexArray(vao_sphere);
    
    // position vbo of sphere
    glGenBuffers(1, &vbo_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // normal vbo of sphere
    glGenBuffers(1, &vbo_sphere_normal);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(KAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // element vbo of sphere
    glGenBuffers(1, &vbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // unbind vao
    glBindVertexArray(0);


    //3D & Depth Code
    glClearDepth(1.0f);     //Default value passed to the Function
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //Disable CULLing while animation
    glDisable(GL_CULL_FACE);

    //SetglColor
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black

    bLights = false; //Lighting Toggle
    VKeyIsPressed = 0; //Per-Vertex Lightling Toggle
    FKeyIsPressed = 0; //Per-Fragment Lightling Toggle

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

-(void)getPerVertexLighting
{
    //code
    //create shader
    vertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexShaderSourceCode_PV =
        "#version 410 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_model_matrix;" \
        "uniform mat4 u_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform int u_L_KEY_PRESSED;" \
        "uniform vec3 u_LA_PV;" \
        "uniform vec3 u_LD_PV;" \
        "uniform vec3 u_LS_PV;" \
        "uniform vec4 u_light_position_PV;" \
        "uniform vec3 u_KA_PV;" \
        "uniform vec3 u_KD_PV;" \
        "uniform vec3 u_KS_PV;" \
        "uniform float u_material_shininess_PV;" \
        "out vec3 phong_ads_light_PV;" \
        "void main(void)" \
        "{" \
        "if(u_L_KEY_PRESSED == 1)" \
        "{" \
        "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
        "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
        "vec3 light_direction = normalize(vec3(u_light_position_PV - eye_coordinates));" \
        "vec3 reflection_vector = reflect(-light_direction, transformed_normal);" \
        //SWIZZLING
        "vec3 view_vector = normalize(-eye_coordinates.xyz);" \
        "vec3 ambient_PV = u_LA_PV * u_KA_PV;" \
        "vec3 diffuse_PV = u_LD_PV * u_KD_PV * max(dot(light_direction, transformed_normal), 0.0f);" \
        "vec3 specular_PV = u_LS_PV * u_KS_PV * pow(max(dot(reflection_vector, view_vector), 0.0f), u_material_shininess_PV);" \
        "phong_ads_light_PV = ambient_PV + diffuse_PV + specular_PV;" \
        "}" \
        "else" \
        "{" \
        "phong_ads_light_PV = vec3(1.0f, 1.0f, 1.0f);" \
        "}" \
        "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
        "}";

    glShaderSource(vertexShaderObject_PV, 1, (const GLchar**)&vertexShaderSourceCode_PV, NULL);

    //compile shader
    glCompileShader(vertexShaderObject_PV);
    
    //Shader Compilation Error Checking
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char* szInfoLog = NULL;
    glGetShaderiv(vertexShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)  //if Shader Compiled Status Failed
    {
        glGetShaderiv(vertexShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject_PV, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "PER-VERTEX-> Vertex Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release]; //Uninit()
                [NSApp terminate:self]; //exit()
            }
        }
    }

    //FRAGMENT SHADER
    //create shader
    fragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

    //provide source code to shader
    const GLchar* fragmentShaderSourceCode_PV =
        "#version 410 core" \
        "\n" \
        "in vec3 phong_ads_light_PV;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = vec4(phong_ads_light_PV, 1.0f);" \
        "}";

    glShaderSource(fragmentShaderObject_PV, 1, (const GLchar**)&fragmentShaderSourceCode_PV, NULL);

    //compile shader
    glCompileShader(fragmentShaderObject_PV);

    //Shader Compilation Error Checking
    iInfoLogLength = 0;
    iShaderCompiledStatus = 0;
    szInfoLog = NULL;

    glGetShaderiv(fragmentShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject_PV, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "PER-VERTEX-> Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release]; //Uninit()
                [NSApp terminate:self]; //exit()
            }
        }
    }

    //SHADER PROGRAM
    //create
    shaderProgramObject_PV = glCreateProgram();

    //attach vertex shader object to program 
    glAttachShader(shaderProgramObject_PV, vertexShaderObject_PV);

    //attach fragment shader object to program
    glAttachShader(shaderProgramObject_PV, fragmentShaderObject_PV);

    //pre-link binding of shader program object with vertex shader attribute
    glBindAttribLocation(shaderProgramObject_PV, KAB_ATTRIBUTE_POSITION, "vPosition");

    //pre-link binding of shader program object with fragment shader attribute
    glBindAttribLocation(shaderProgramObject_PV, KAB_ATTRIBUTE_NORMAL, "vNormal");

    //link shader
    glLinkProgram(shaderProgramObject_PV);

    //Shader Program Link Error Checking
    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(shaderProgramObject_PV, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject_PV, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "PER-VERTEX-> Shader Program Link Log : %s\n", szInfoLog);                free(szInfoLog);
                [self release]; //Uninit()
                [NSApp terminate:self]; //exit()
            }
        }
    }

    //get Uniform Location 
    modelMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_model_matrix");
    viewMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_view_matrix");
    projectionMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_projection_matrix");
    LKeyPressedUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_L_KEY_PRESSED");
    //for PER-VERTEX LIGHTING
    LaUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_LA_PV");
    LdUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_LD_PV");
    LsUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_LS_PV");
    lightPositionUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_light_position_PV");
    KaUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_KA_PV");
    KdUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_KD_PV");
    KsUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_KS_PV");
    materialShininessUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_material_shininess_PV");
} 

-(void)getPerFragmentLighting
{
    //code
    //GLSL #version 410 core
    vertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

    //provide source code to shader
    //use 'core' profile i.e PROGRAMMABLE PIPELINE
    const GLchar* vertexShaderSourceCode_PF =
        "#version 410 core" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_model_matrix;" \
        "uniform mat4 u_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform int u_L_KEY_PRESSED;" \
        "uniform vec4 u_light_position_PF;" \
        "out vec3 transformed_normal;" \
        "out vec3 light_direction;" \
        "out vec3 view_vector;" \
        "void main(void)" \
        "{" \
        "if(u_L_KEY_PRESSED == 1)" \
        "{" \
        "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
        "transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
        "light_direction = vec3(u_light_position_PF - eye_coordinates);" \
        //SWIZZLING
        "view_vector = -eye_coordinates.xyz;" \
        "}" \
        "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
        "}";

    glShaderSource(vertexShaderObject_PF, 1, (const GLchar**)&vertexShaderSourceCode_PF, NULL);

    //compile shader
    glCompileShader(vertexShaderObject_PF);
    
    //Shader Compilation Error Checking
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char* szInfoLog = NULL;
    glGetShaderiv(vertexShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)  //if Shader Compiled Status Failed
    {
        glGetShaderiv(vertexShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject_PF, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "PER-FRAGMENT-> Vertex Shader Compilation Log : %s\n", szInfoLog);                free(szInfoLog);
                [self release]; //Uninit()
                [NSApp terminate:self]; //exit()
            }
        }
    }

    //FRAGMENT SHADER
    //create shader
    fragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

    //provide source code to shader
    const GLchar* fragmentShaderSourceCode_PF =
        "#version 410 core" \
        "\n" \
        "in vec3 transformed_normal;" \
        "in vec3 light_direction;" \
        "in vec3 view_vector;" \
        "uniform int u_L_KEY_PRESSED;" \
        "uniform vec3 u_LA_PF;" \
        "uniform vec3 u_LD_PF;" \
        "uniform vec3 u_LS_PF;" \
        "uniform vec3 u_KA_PF;" \
        "uniform vec3 u_KD_PF;" \
        "uniform vec3 u_KS_PF;" \
        "uniform float u_material_shininess_PF;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "vec3 phong_ads_light_PF;" \
        "if(u_L_KEY_PRESSED == 1)" \
        "{" \
        "vec3 normalized_transformed_normal = normalize(transformed_normal);" \
        "vec3 normalized_light_direction = normalize(light_direction);" \
        "vec3 normalized_view_vector = normalize(view_vector);" \
        "vec3 ambient_PF = u_LA_PF * u_KA_PF;" \
        "vec3 diffuse_PF = u_LD_PF * u_KD_PF * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);" \
        "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);" \
        "vec3 specular_PF = u_LS_PF * u_KS_PF * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_material_shininess_PF);" \
        "phong_ads_light_PF = ambient_PF + diffuse_PF + specular_PF;" \
        "}" \
        "else" \
        "{" \
        "phong_ads_light_PF = vec3(1.0f,1.0f,1.0f);" \
        "}" \
        "FragColor = vec4(phong_ads_light_PF, 1.0f);" \
        "}";

    glShaderSource(fragmentShaderObject_PF, 1, (const GLchar**)&fragmentShaderSourceCode_PF, NULL);

    //compile shader
    glCompileShader(fragmentShaderObject_PF);

    //Shader Compilation Error Checking
    iInfoLogLength = 0;
    iShaderCompiledStatus = 0;
    szInfoLog = NULL;

    glGetShaderiv(fragmentShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject_PF, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "PER-FRAGMENT-> Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release]; //Uninit()
                [NSApp terminate:self]; //exit()
            }
        }
    }

    //SHADER PROGRAM
    //create
    shaderProgramObject_PF = glCreateProgram();

    //attach vertex shader object to program 
    glAttachShader(shaderProgramObject_PF, vertexShaderObject_PF);

    //attach fragment shader object to program
    glAttachShader(shaderProgramObject_PF, fragmentShaderObject_PF);

    //pre-link binding of shader program object with vertex shader attribute
    glBindAttribLocation(shaderProgramObject_PF, KAB_ATTRIBUTE_POSITION, "vPosition");

    //pre-link binding of shader program object with fragment shader attribute
    glBindAttribLocation(shaderProgramObject_PF, KAB_ATTRIBUTE_NORMAL, "vNormal");

    //link shader
    glLinkProgram(shaderProgramObject_PF);

    //Shader Program Link Error Checking
    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(shaderProgramObject_PF, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject_PF, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "PER-FRAGMENT-> Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release]; //Uninit()
                [NSApp terminate:self]; //exit()
            }
        }
    }

    //get Uniform Location for PER-FRAGMENT LIGHTING
    modelMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_model_matrix");
    viewMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_view_matrix");
    projectionMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_projection_matrix");
    //for PER-FRAGMENT LIGHTING
    LKeyPressedUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_L_KEY_PRESSED");
    LaUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_LA_PF");
    LdUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_LD_PF");
    LsUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_LS_PF");
    lightPositionUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_light_position_PF");
    KaUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_KA_PF");
    KdUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_KD_PF");
    KsUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_KS_PF");
    materialShininessUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_material_shininess_PF");

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
    //ALBEDO
    GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f ,1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

    GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f ,1.0f };
    GLfloat materialDiffuse[] = { 0.5f, 0.2f, 0.7f ,1.0f };
    GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f ,1.0f };
    GLfloat materialShininess = 128.0f;

    [[self openGLContext] makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Display() here
    
    //OpenGL Drawing
    mat4 translationMatrix = mat4::identity(); //model
    mat4 viewMatrix = mat4::identity(); //view
    mat4 projectionMatrix = mat4::identity(); //projection

    translationMatrix = translate(0.0f, 0.0f, -3.0f);
    //viewMatrix must be identity
    //viewMatrix is assigned to identity above 
    //hence, not writing again
    projectionMatrix = perspectiveProjectionMatrix;

    //start using OpenGL program object
    glUseProgram(shaderProgramObject_PV); //using Per-vertex by default

    //USING PER-VERTEX LIGHITNG
    if (VKeyIsPressed == 1)
    {
        //glUseProgram(gShaderProgramObject_PV);

        glUniform4fv(lightPositionUniform_PV, 1, (GLfloat*)lightPosition);
        if (bLights == true)
        {
            glUniform1i(LKeyPressedUniform_PV, 1);
            glUniform3fv(LaUniform_PV, 1, (GLfloat*)lightAmbient);
            glUniform3fv(LdUniform_PV, 1, (GLfloat*)lightDiffuse);
            glUniform3fv(LsUniform_PV, 1, (GLfloat*)lightSpecular);
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular);
            glUniform1f(materialShininessUniform_PV, materialShininess);
            
        }
        else
        {
            glUniform1i(LKeyPressedUniform_PV, 0);

        }

        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        glUniformMatrix4fv(modelMatrixUniform_PV, 1, GL_FALSE, translationMatrix); //translation and model are same
        glUniformMatrix4fv(viewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
    }

    //USING PER-FRAGMENT LIGHITNG
    else if (FKeyIsPressed == 1)
    {
        //start using OpenGL program object
        glUseProgram(shaderProgramObject_PF);

        glUniform4fv(lightPositionUniform_PF, 1, (GLfloat*)lightPosition);
        if (bLights == true)
        {
            glUniform1i(LKeyPressedUniform_PF, 1);
            glUniform3fv(LaUniform_PF, 1, (GLfloat*)lightAmbient);
            glUniform3fv(LdUniform_PF, 1, (GLfloat*)lightDiffuse);
            glUniform3fv(LsUniform_PF, 1, (GLfloat*)lightSpecular);
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular);
            glUniform1f(materialShininessUniform_PF, materialShininess);
        }
        else
        {
            glUniform1i(LKeyPressedUniform_PF, 0);
        }

        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        glUniformMatrix4fv(modelMatrixUniform_PF, 1, GL_FALSE, translationMatrix); //translation and model are same
        glUniformMatrix4fv(viewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_PF, 1, GL_FALSE, projectionMatrix);

    }

    else
    {
        //PER-VERTEX SHOULD BE ENABLED BY DEFAULT - HENCE, WRTING OUT OF IF-ELSE BLOCK
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        glUniformMatrix4fv(modelMatrixUniform_PV, 1, GL_FALSE, translationMatrix); //translation and model are same
        glUniformMatrix4fv(viewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
    }
    
    

    //bind vao - vertex array object
    glBindVertexArray(vao_sphere);

    //draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

    //unbind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
        case 'Q': //QUIT
        case 'q':
            [self release];
            [NSApp terminate:self];            
            break;

        case 27: //ESC to fullscreen
            [[self window] toggleFullScreen:self];
            break;

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
            if (bLights == false)
            {
                bLights = true;
                VKeyIsPressed = 1; //PER-VERTEX SHADING WILL BE ENABLED BY DEFAULT
                FKeyIsPressed = 0;
                fprintf(gpFile, "\n -> gbLights was false - now - L pressed key = %d", bLights);

            }
            else if (bLights == true)
            {
                bLights = false;
                fprintf(gpFile, "\n -> gbLights was true - now - L pressed key = %d", bLights);
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
    if (vao_sphere)
    {
        glDeleteVertexArrays(1, &vao_sphere);
        vao_sphere = 0;
    }

    //destroy vbo
    if (vbo_sphere_position)
    {
        glDeleteVertexArrays(1, &vbo_sphere_position);
        vbo_sphere_position = 0;
    }

    //destroy vbo
    if (vbo_sphere_normal)
    {
        glDeleteVertexArrays(1, &vbo_sphere_normal);
        vbo_sphere_normal = 0;
    }

    //destroy vbo
    if (vbo_sphere_element)
    {
        glDeleteVertexArrays(1, &vbo_sphere_element);
        vbo_sphere_element = 0;
    }

    //SAFE SHADER CLEANUP
    //PER_VERTEX
    if (shaderProgramObject_PV)
    {
        glUseProgram(shaderProgramObject_PV);

        GLsizei shaderCount_PV;
        glGetProgramiv(shaderProgramObject_PV, GL_ATTACHED_SHADERS, &shaderCount_PV);

        GLuint* pShader_PV = NULL;
        pShader_PV = (GLuint*)malloc(sizeof(GLuint) * shaderCount_PV);

        if (pShader_PV == NULL)
        {
            //error checking
            fprintf(gpFile, "Error Creating pShader_PV.\n Exitting Now!!\n");
            [self release]; //Uninit()
            [NSApp terminate:self]; //exit()
        }

        glGetAttachedShaders(shaderProgramObject_PV, shaderCount_PV, &shaderCount_PV, pShader_PV);

        for (GLsizei i = 0; i < shaderCount_PV; i++)
        {
            glDetachShader(shaderProgramObject_PV, pShader_PV[i]);
            glDeleteShader(pShader_PV[i]);
            pShader_PV[i] = 0;
        }

        free(pShader_PV);

        glDeleteProgram(shaderProgramObject_PV);
        shaderProgramObject_PV = 0;

        //unlink
        glUseProgram(0);
    }

    //SAFE SHADER CLEANUP
    //PER_FRAGMENT
    if (shaderProgramObject_PF)
    {
        glUseProgram(shaderProgramObject_PF);

        GLsizei shaderCount_PF;
        glGetProgramiv(shaderProgramObject_PF, GL_ATTACHED_SHADERS, &shaderCount_PF);

        GLuint* pShader_PF = NULL;
        pShader_PF = (GLuint*)malloc(sizeof(GLuint) * shaderCount_PF);

        if (pShader_PF == NULL)
        {
            //error checking
            fprintf(gpFile, "Error Creating pShader.\n Exitting Now!!\n");
            [self release]; //Uninit()
            [NSApp terminate:self]; //exit()
        }

        glGetAttachedShaders(shaderProgramObject_PF, shaderCount_PF, &shaderCount_PF, pShader_PF);

        for (GLsizei i = 0; i < shaderCount_PF; i++)
        {
            glDetachShader(shaderProgramObject_PF, pShader_PF[i]);
            glDeleteShader(pShader_PF[i]);
            pShader_PF[i] = 0;
        }

        free(pShader_PF);

        glDeleteProgram(shaderProgramObject_PF);
        shaderProgramObject_PF = 0;

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

