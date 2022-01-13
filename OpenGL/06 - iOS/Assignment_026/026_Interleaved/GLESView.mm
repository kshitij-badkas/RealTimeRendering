//
//  GLESView.m
//  Interleaved
//
//  Created by STC on 31/07/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "vmath.h"

using namespace vmath;

    //Global Variables
enum
{
    KAB_ATTRIBUTE_POSITION = 0,
    KAB_ATTRIBUTE_COLOR,
    KAB_ATTRIBUTE_NORMAL,
    KAB_ATTRIBUTE_TEXCOORD
};

@implementation GLESView
{
@private
    EAGLContext *eaglContext;
    
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
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
    
    GLuint doubleTapUniform; //LKeyPressedUniform;
    
        //TEXTURES
    GLuint marble_texture;
    GLuint textureSamplerUniform;
    
    mat4 perspectiveProjectionMatrix;
    
        //ANIMATION AND LIGHT TOGGLE
    GLuint single_tap; //= 0;
    GLuint double_tap; //= 0;
    
    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
    
}

-(id)initWithFrame:(CGRect)frame
{
        // code
    self = [super initWithFrame:frame];
    if(self)
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)[super layer];
        
        /* eaglLayer ALTERNATE CALL SYNTAX
         // caglLayer.opaque = YES;
         // '=' sign chya davya bajula - internally SET funciton la call hota
         // '=' sign chya ujvya bajula - internally GET funciton la call hota
         */
        
        [eaglLayer setOpaque:YES];
        [eaglLayer setDrawableProperties:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                          kEAGLColorFormatRGBA8,kEAGLDrawablePropertyColorFormat, nil]];
        
        eaglContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        
        if(eaglContext == nil)
        {
            printf("OpenGL-ES Context Creation Failed.\n");
            return (nil);
        }
        
        [EAGLContext setCurrentContext:eaglContext];
        
            //
            // FRAME BUFFER CODE HERE
            //
        
        glGenFramebuffers(1, &defaultFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        
        GLint backingWidth;
        GLint backingHeight;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Framebuffer is not complete.\n");
            [self uninitialize];
            return (nil);
        }
        
        printf("%s\n", glGetString(GL_RENDERER));
        printf("%s\n", glGetString(GL_VERSION));
        printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        
        
        
        animationFrameInterval = 60; //60 FPS - default 60 from iOS 8.2
        isAnimating = NO;
        
            // prepareOpenGL here
            // from vertexShaderObject to glClearColor()
        
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
            //provide source code to shader
            //use 'core' profile i.e PROGRAMMABLE PIPELINE
        const GLchar* vertexShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "precision mediump float;" \
        "in vec4 vPosition;" \
        "in vec4 vColor;" \
        "in vec3 vNormal;" \
        "in vec2 vTexCoord;" \
        "uniform mat4 u_model_matrix;" \
        "uniform mat4 u_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform int u_double_tap;" \
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
        "if(u_double_tap == 1)" \
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
                    printf("Vertex Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
                }
            }
        }
        
            //FRAGMENT SHADER
            //create shader
        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        
            //provide source code to shader
        const GLchar* fragmentShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "precision highp float;" \
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
                    printf("Fragment Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
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
                    printf("Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
                    
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
        doubleTapUniform = glGetUniformLocation(shaderProgramObject, "u_double_tap");
        
            //vertices, colours, shader attribs, vbo, vao initializations
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
        glClearDepthf(1.0f);     //Default value passed to the Function
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
            //Disable CULLing while animation
        glDisable(GL_CULL_FACE);
        
            //SetglColor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black
        
            //Load textures
        marble_texture = [self loadGLTexture:@"marble" :@"bmp"];
        
            //Enable TExtures
        glEnable(GL_TEXTURE_2D);
        
            //SetglColor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black
        
            //lights and animation
        single_tap = 0;
        double_tap = 0;
        
            //set identity matrix to identity matrix
        perspectiveProjectionMatrix = vmath::mat4::identity();
        
            // Gestures
        UITapGestureRecognizer *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [swipeGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        UILongPressGestureRecognizer *longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [longPressGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:longPressGestureRecognizer];
    }
    
    return (self);
}

+(Class)layerClass
{
        // code
    return ([CAEAGLLayer class]);
}

    // comment out in iPhone - drawRect is only used in immediate mode
/*
 -(void)drawRect:(CGRect)rect
 {
 // Drawing code
 }
 */

-(void)layoutSubviews
{
        // code
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)[self layer]];
    
    GLint width;
    GLint height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("In layoutSubViews (Resize) Framebuffer is not complete.\n");
    }
    
    if(height < 0)
    {
        height = 1;
    }
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f,
                                                     (GLfloat)width / (GLfloat)height,
                                                     0.1f,
                                                     100.0f);
    
    [self drawView:nil];
}


-(GLuint)loadGLTexture :(NSString *)imageFilename :(NSString *)extension
{
        //code
    NSString *imageFilenameWithPath = [[NSBundle mainBundle] pathForResource:imageFilename ofType:extension];
    
        //1
        //Get NSImage representattion of image file, do error check
    UIImage *bmpImage = [[UIImage alloc] initWithContentsOfFile:imageFilenameWithPath];
    if(!bmpImage)
    {
        printf("UIImage Conversion of Image File Failed.\n");
        return (0);
    }
    
        //2
        //Get CGImage representation of NSImage
    CGImageRef cgImage = [bmpImage CGImage];
    if(!cgImage)
    {
        printf("CGImage REpresentation of UIImage Failed.\n");
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
        printf("CFData Represented data of CGImage Failed.\n");
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


-(void)drawView:(id)sender
{
        // code
        // variables
    GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f ,1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };
    GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f ,1.0f };
    GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f ,1.0f };
    GLfloat materialShininess = 50.0f;
    static GLfloat angleCube = 0.0f;
    
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        //start using OpenGL program object
    glUseProgram(shaderProgramObject);
    
    
    if (double_tap == 1)
    {
        glUniform1i(doubleTapUniform, 1);
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
        glUniform1i(doubleTapUniform, 0);
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
    if (single_tap == 1)
    {
        angleCube += 1.0f;
        if (angleCube >= 360.0f)
        {
            angleCube = 0.0f;
        }
    }
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    
        //swap buffer
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}

-(void)startAnimation
{
        // code
    if(isAnimating == NO)
    {
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFramesPerSecond:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        isAnimating = YES;
    }
}

-(void)stopAnimation
{
        // code
    if(isAnimating == YES)
    {
        [displayLink invalidate];
        displayLink = nil;
        
        isAnimating = NO;
    }
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{
        // code
    single_tap ++;
    printf("single tap = %d\n", single_tap);
    if(single_tap > 1)
    {
        single_tap = 0;
        printf("single tap > 1, hence single tap = %d\n", single_tap);
    }
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
        // code
    double_tap ++;
    printf("double tap = %d\n", double_tap);
    if(double_tap > 1)
    {
        double_tap = 0;
        printf("double tap tap > 1, hence double tap = %d\n", double_tap);
    }
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
        // code
    [self uninitialize];
    [self release];
    exit(0);
}

-(void)onLongPress:(UISwipeGestureRecognizer *)gr
{
        // code
}

-(void)uninitialize
{
        // code
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
    
    if(marble_texture)
    {
        glDeleteTextures(1, &marble_texture);
        marble_texture = 0;
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
            printf("Error Creating pShader.\n Exitting Now!!\n");
            [self uninitialize];
            [self release];
            exit(0);
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
    
    if(depthRenderbuffer)
    {
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
    
    if(colorRenderbuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
    
    if(defaultFramebuffer)
    {
        glDeleteFramebuffers(1, &defaultFramebuffer);
    }
    
    if(eaglContext)
    {
        if([EAGLContext currentContext] == eaglContext)
        {
            [EAGLContext setCurrentContext:nil];
            [eaglContext release];
            eaglContext = nil;
        }
    }
}

-(void)dealloc
{
        // code
    [self uninitialize];
    [super dealloc];
}

@end



