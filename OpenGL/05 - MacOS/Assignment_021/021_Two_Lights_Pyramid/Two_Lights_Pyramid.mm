// mkdir -p Two_Lights_Pyramid.app/Contents/MacOS
// clang++ -Wno-deprecated-declarations -o Two_Lights_Pyramid.app/Contents/MacOS/Two_Lights_Pyramid Two_Lights_Pyramid.mm -framework cocoa -framework QuartzCore -framework OpenGL

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
    
    [window setTitle:@"KAB:Assignment_021_Two_Lights_on_Pyramid"];
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

    mat4 perspectiveProjectionMatrix;

    //pyramid Vao and Vbo
    GLuint vao_pyramid;
    GLuint vbo_position_pyramid;
    GLuint vbo_normal_pyramid;

    //MATRIX UNIFORMS
    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    //LIGHT UNIFORMS
    GLuint LaUniform[2]; //ambient light component
    GLuint LdUniform[2]; //diffuse light component
    GLuint LsUniform[2]; //specular light component
    GLuint lightPositionUniform[2]; //light position

    //MATERIAL UNIFORMS
    GLuint KaUniform; //ambient material component
    GLuint KdUniform; //diffuse material component
    GLuint KsUniform; //specular material component
    GLuint materialShininessUniform; //shininess material

    GLuint LKeyPressedUniform;

    //DECLARE ARRAY OF STRUCTURE
    struct Light
    {
        vec4 lightAmbient;
        vec4 lightDiffuse;
        vec4 lightSpecular;
        vec4 lightPosition;
    };
    struct Light light[2];

    vec4 materialAmbient;
    vec4 materialDiffuse;
    vec4 materialSpecular;
    GLfloat materialShininess;

    bool bLights;

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
        "in vec3 vNormal;" \
        "uniform mat4 u_model_matrix;" \
        "uniform mat4 u_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform int u_L_KEY_PRESSED;" \
        "uniform vec3 u_LA[2];" \
        "uniform vec3 u_LD[2];" \
        "uniform vec3 u_LS[2];" \
        "uniform vec4 u_light_position[2];" \
        "uniform vec3 u_KA;" \
        "uniform vec3 u_KD;" \
        "uniform vec3 u_KS;" \
        "uniform float u_material_shininess;" \
        "out vec3 phong_ads_light;" \
        "void main(void)" \
        "{" \
        "if(u_L_KEY_PRESSED == 1)" \
        "{" \
        "vec3 ambient[2];" \
        "vec3 diffuse[2];" \
        "vec3 specular[2];" \
        "vec3 light_direction[2];" \
        "vec3 reflection_vector[2];" \
        "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
        "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
        "vec3 view_vector = normalize(-eye_coordinates.xyz);" \
        "vec3 temp;" \
        "for(int i = 0; i < 2; i++)" \
        "{" \
        "light_direction[i] = normalize(vec3(u_light_position[i] - eye_coordinates));" \
        "reflection_vector[i] = reflect(-light_direction[i], transformed_normal);" \
        "ambient[i] = u_LA[i] * u_KA;" \
        "diffuse[i] = u_LD[i] * u_KD * max(dot(light_direction[i], transformed_normal), 0.0f);" \
        "specular[i] = u_LS[i] * u_KS * pow(max(dot(reflection_vector[i], view_vector), 0.0f), u_material_shininess);" \
        //"phong_ads_light =  ambient[i] + diffuse[i] + specular[i];" \
        
        "phong_ads_light =  ambient[0] + diffuse[0] + specular[0] + ambient[1] + diffuse[1] + specular[1];" \


        "}" \
        "}" \
        "else" \
        "{" \
        "phong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
        "}" \
        "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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
        "in vec3 phong_ads_light;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = vec4(phong_ads_light, 1.0f);" \
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
    
    //pre-link binding of shader program object with vertex shader attribute
    glBindAttribLocation(shaderProgramObject, KAB_ATTRIBUTE_NORMAL, "vNormal");
    
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
    LKeyPressedUniform = glGetUniformLocation(shaderProgramObject, "u_L_KEY_PRESSED");
    
    LaUniform[0] = glGetUniformLocation(shaderProgramObject, "u_LA[0]");
    LdUniform[0] = glGetUniformLocation(shaderProgramObject, "u_LD[0]");
    LsUniform[0] = glGetUniformLocation(shaderProgramObject, "u_LS[0]");
    lightPositionUniform[0] = glGetUniformLocation(shaderProgramObject, "u_light_position[0]");
    
    LaUniform[1] = glGetUniformLocation(shaderProgramObject, "u_LA[1]");
    LdUniform[1] = glGetUniformLocation(shaderProgramObject, "u_LD[1]");
    LsUniform[1] = glGetUniformLocation(shaderProgramObject, "u_LS[1]");
    lightPositionUniform[1] = glGetUniformLocation(shaderProgramObject, "u_light_position[1]");

    KaUniform = glGetUniformLocation(shaderProgramObject, "u_KA");
    KdUniform = glGetUniformLocation(shaderProgramObject, "u_KD");
    KsUniform = glGetUniformLocation(shaderProgramObject, "u_KS");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
    
    //vertices, colours, shader attribs, vbo, vao initializations
    const GLfloat pyramidVertices[] =
    {
        //FRONT
        0.0f, 1.0f, 0.0f, //apex
        -1.0f, -1.0f, 1.0f, //left-bottom
        1.0f, -1.0f, 1.0f, //right-bottom
        //RIGHT
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        //BACK
        0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        //LEFT
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f
    };

    const GLfloat pyramidNormals[] =
    {
        0.0f, 0.447214f, 0.894427f,// front-top
        0.0f, 0.447214f, 0.894427f,// front-left
        0.0f, 0.447214f, 0.894427f,// front-right

        0.894427f, 0.447214f, 0.0f, // right-top
        0.894427f, 0.447214f, 0.0f, // right-left
        0.894427f, 0.447214f, 0.0f, // right-right

        0.0f, 0.447214f, -0.894427f, // back-top
        0.0f, 0.447214f, -0.894427f, // back-left
        0.0f, 0.447214f, -0.894427f, // back-right

        -0.894427f, 0.447214f, 0.0f, // left-top
        -0.894427f, 0.447214f, 0.0f, // left-left
        -0.894427f, 0.447214f, 0.0f // left-right

    };  

    
    //Initialize Light attributes
    light[0].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    light[0].lightDiffuse = vec4(1.0f, 0.0f, 0.0f, 1.0f); //RED
    light[0].lightSpecular = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    light[0].lightPosition = vec4(-2.0f, 0.0f, 0.0f, 1.0f);

    light[1].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    light[1].lightDiffuse = vec4(0.0f, 0.0f, 1.0f, 1.0f); //BLUE
    light[1].lightSpecular = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    light[1].lightPosition = vec4(2.0f, 0.0f, 0.0f, 1.0f);
    

    //Initalize material attributes
    materialAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    materialDiffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    materialSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    materialShininess = 50.0f;

    //GEN START
    glGenVertexArrays(1, &vao_pyramid);
    //BIND START
    glBindVertexArray(vao_pyramid);

    //FOR POSITION
    glGenBuffers(1, &vbo_position_pyramid);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //FOR PYRAMID NORMAL
    glGenBuffers(1, &vbo_normal_pyramid);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_pyramid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(KAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //BIND END
    glBindVertexArray(0);


    //3D & Depth Code
    glClearDepth(1.0f);     //Default value passed to the Function
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //Disable CULLing while animation
    glDisable(GL_CULL_FACE);

    //SetglColor
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);//Black

    bLights = false;

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
    //variables
    static GLfloat anglePyramid = 0.0f;

    [[self openGLContext] makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Display() here
    
    //start using OpenGL program object
    glUseProgram(shaderProgramObject);

    if (bLights == true)
    {
        glUniform1i(LKeyPressedUniform, 1);
        for (int i = 0; i < 2; i++)
        {
            glUniform4fv(lightPositionUniform[i], 1, light[i].lightPosition);
            glUniform3fv(LaUniform[i], 1, light[i].lightAmbient);
            glUniform3fv(LdUniform[i], 1, light[i].lightDiffuse);
            glUniform3fv(LsUniform[i], 1, light[i].lightSpecular);
        }
        glUniform3fv(KaUniform, 1, materialAmbient);
        glUniform3fv(KdUniform, 1, materialDiffuse);
        glUniform3fv(KsUniform, 1, materialSpecular);
        glUniform1f(materialShininessUniform, materialShininess);
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }

    //OpenGL Drawing
    mat4 translationMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 rotationMatrix;

    //PYRAMID
    translationMatrix = mat4::identity();
    rotationMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();

    translationMatrix = translate(0.0f, 0.0f, -6.0f);
    rotationMatrix = rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
    translationMatrix = translationMatrix * rotationMatrix;
    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, translationMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    //bind vao - vertex array object
    glBindVertexArray(vao_pyramid);

    //draw
    glDrawArrays(GL_TRIANGLES, 0, 12); //PYRAMID HAS 4 FACES, HENCE 12 VERTICES

    //unbind vao
    glBindVertexArray(0);

    //stop using OpenGL program object
    glUseProgram(0);


    //ANIMATION CODE
    anglePyramid += 1.0f;
    if (anglePyramid >= 360.0f)
    {
        anglePyramid = 0.0f;
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

        case 'L':
        case 'l':
            if (bLights == false)
            {
                bLights = true;
            }
            else if (bLights == true)
            {
                bLights = false;
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
    if (vao_pyramid)
    {
        glDeleteVertexArrays(1, &vao_pyramid);
        vao_pyramid = 0;
    }

    //destroy vbo
    if (vbo_position_pyramid)
    {
        glDeleteVertexArrays(1, &vbo_position_pyramid);
        vbo_position_pyramid = 0;
    }

    //destroy vbo
    if (vbo_normal_pyramid)
    {
        glDeleteVertexArrays(1, &vbo_normal_pyramid);
        vbo_normal_pyramid = 0;
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

