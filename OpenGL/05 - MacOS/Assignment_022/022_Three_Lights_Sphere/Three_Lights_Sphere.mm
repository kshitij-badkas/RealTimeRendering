// mkdir -p Three_Lights_Sphere.app/Contents/MacOS
// clang++ -Wno-deprecated-declarations -o Three_Lights_Sphere.app/Contents/MacOS/Three_Lights_Sphere Three_Lights_Sphere.mm -framework cocoa -framework QuartzCore -framework OpenGL

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
    
    [window setTitle:@"KAB:Assignment_021_Three_Rotating_Lights_on_Sphere"];
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
    GLuint LaUniform_PV[3]; //ambient light component
    GLuint LdUniform_PV[3]; //diffuse light component
    GLuint LsUniform_PV[3]; //specular light component
    GLuint lightPositionUniform_PV[3]; //light position

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
    GLuint LaUniform_PF[3]; //ambient light component
    GLuint LdUniform_PF[3]; //diffuse light component
    GLuint LsUniform_PF[3]; //specular light component
    GLuint lightPositionUniform_PF[3]; //light position

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

    struct Light
    {
        vec4 lightAmbient;
        vec4 lightDiffuse;
        vec4 lightSpecular;
        vec4 lightPosition;
    };
    struct Light light[3]; //3 Lights

    // material properties
    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
    GLfloat materialShininess;

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

    materialAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    materialDiffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    materialSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    materialShininess = 128.0f;

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
        "phong_ads_light = ambient[0] + diffuse[0] + specular[0] + ambient[1] + diffuse[1] + specular[1] + ambient[2] + diffuse[2] + specular[2];" \
        "}" \
        "}" \
        "else" \
        "{" \
        "phong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
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
        "in vec3 phong_ads_light;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = vec4(phong_ads_light, 1.0f);" \
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
                fprintf(gpFile, "PER-VERTEX-> Shader Program Link Log : %s\n", szInfoLog);                
                free(szInfoLog);
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
    
    LaUniform_PV[0] = glGetUniformLocation(shaderProgramObject_PV, "u_LA[0]");
    LdUniform_PV[0] = glGetUniformLocation(shaderProgramObject_PV, "u_LD[0]");
    LsUniform_PV[0] = glGetUniformLocation(shaderProgramObject_PV, "u_LS[0]");
    lightPositionUniform_PV[0] = glGetUniformLocation(shaderProgramObject_PV, "u_light_position[0]");

    LaUniform_PV[1] = glGetUniformLocation(shaderProgramObject_PV, "u_LA[1]");
    LdUniform_PV[1] = glGetUniformLocation(shaderProgramObject_PV, "u_LD[1]");
    LsUniform_PV[1] = glGetUniformLocation(shaderProgramObject_PV, "u_LS[1]");
    lightPositionUniform_PV[1] = glGetUniformLocation(shaderProgramObject_PV, "u_light_position[1]");

    LaUniform_PV[2] = glGetUniformLocation(shaderProgramObject_PV, "u_LA[2]");
    LdUniform_PV[2] = glGetUniformLocation(shaderProgramObject_PV, "u_LD[2]");
    LsUniform_PV[2] = glGetUniformLocation(shaderProgramObject_PV, "u_LS[2]");
    lightPositionUniform_PV[2] = glGetUniformLocation(shaderProgramObject_PV, "u_light_position[2]");

    KaUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_KA");
    KdUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_KD");
    KsUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_KS");
    materialShininessUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "u_material_shininess");

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
                fprintf(gpFile, "PER-FRAGMENT-> Vertex Shader Compilation Log : %s\n", szInfoLog);                
                free(szInfoLog);
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
        "phong_ads_light_PF = ambient_PF[0] + diffuse_PF[0] + specular_PF[0] + ambient_PF[1] + diffuse_PF[1] + specular_PF[1] + ambient_PF[2] + diffuse_PF[2] + specular_PF[2];" \
        "}" \
        "}" \
        "else" \
        "{" \
        "phong_ads_light_PF = vec3(1.0f, 1.0f, 1.0f);" \
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
    LKeyPressedUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "u_L_KEY_PRESSED");

    LaUniform_PF[0] = glGetUniformLocation(shaderProgramObject_PF, "u_LA_PF[0]");
    LdUniform_PF[0] = glGetUniformLocation(shaderProgramObject_PF, "u_LD_PF[0]");
    LsUniform_PF[0] = glGetUniformLocation(shaderProgramObject_PF, "u_LS_PF[0]");
    lightPositionUniform_PF[0] = glGetUniformLocation(shaderProgramObject_PF, "u_light_position_PF[0]");

    LaUniform_PF[1] = glGetUniformLocation(shaderProgramObject_PF, "u_LA_PF[1]");
    LdUniform_PF[1] = glGetUniformLocation(shaderProgramObject_PF, "u_LD_PF[1]");
    LsUniform_PF[1] = glGetUniformLocation(shaderProgramObject_PF, "u_LS_PF[1]");
    lightPositionUniform_PF[1] = glGetUniformLocation(shaderProgramObject_PF, "u_light_position_PF[1]");

    LaUniform_PF[2] = glGetUniformLocation(shaderProgramObject_PF, "u_LA_PF[2]");
    LdUniform_PF[2] = glGetUniformLocation(shaderProgramObject_PF, "u_LD_PF[2]");
    LsUniform_PF[2] = glGetUniformLocation(shaderProgramObject_PF, "u_LS_PF[2]");
    lightPositionUniform_PF[2] = glGetUniformLocation(shaderProgramObject_PF, "u_light_position_PF[2]");

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
    static GLfloat lightAngle_Red = 0.0f;
    static GLfloat lightAngle_Green = 0.0f;
    static GLfloat lightAngle_Blue = 0.0f;
    static GLfloat radius = 6.0f;

    [[self openGLContext] makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Display() here
    
    //OpenGL Drawing
    mat4 translationMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
    mat4 projectionMatrix = mat4::identity();

    //code
    translationMatrix = translate(0.0f, 0.0f, -3.0f);
    projectionMatrix = perspectiveProjectionMatrix;

    if (VKeyIsPressed == 1)
    {
        //start using OpenGL program object
    glUseProgram(shaderProgramObject_PV);
        if (bLights == true)
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
            
            glUniform3fv(KaUniform_PV, 1, materialAmbient);
            glUniform3fv(KdUniform_PV, 1, materialDiffuse);
            glUniform3fv(KsUniform_PV, 1, materialSpecular);
            glUniform1f(materialShininessUniform_PV, materialShininess);

        }
        else
        {
            glUniform1i(LKeyPressedUniform_PV, 0);
        }

        glUniformMatrix4fv(modelMatrixUniform_PV, 1, GL_FALSE, translationMatrix);
        glUniformMatrix4fv(viewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
    }

    else if (FKeyIsPressed == 1)
    {

        glUseProgram(shaderProgramObject_PF);

        if (bLights == true)
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

            glUniform3fv(KaUniform_PF, 1, materialAmbient);
            glUniform3fv(KdUniform_PF, 1, materialDiffuse);
            glUniform3fv(KsUniform_PF, 1, materialSpecular);
            glUniform1f(materialShininessUniform_PF, materialShininess);

        }
        else
        {
            glUniform1i(LKeyPressedUniform_PF, 0);
        }

        glUniformMatrix4fv(modelMatrixUniform_PF, 1, GL_FALSE, translationMatrix);
        glUniformMatrix4fv(viewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_PF, 1, GL_FALSE, projectionMatrix);
    }

    else
    {
        glUniformMatrix4fv(modelMatrixUniform_PV, 1, GL_FALSE, translationMatrix);
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

    //ANIMATION LOGIC
    lightAngle_Red += 0.025f;
    lightAngle_Green += 0.025f;
    lightAngle_Blue += 0.025f;
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
                fprintf(gpFile, "\nPER_FRAGMENT -> F pressed key = %d", FKeyIsPressed);
            }
            else if (FKeyIsPressed == 1)
            {
                FKeyIsPressed = 0;
                fprintf(gpFile, "\nPER_FRAGMENT -> F pressed key = %d", FKeyIsPressed);

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

