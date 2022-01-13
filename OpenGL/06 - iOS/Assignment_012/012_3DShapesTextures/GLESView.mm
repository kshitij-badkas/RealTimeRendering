//
//  GLESView.m
//  ThreeDShapesTextures
//
//  Created by STC on 29/07/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "vmath.h"

//using namespace vmath; -> not required if vmath:: is used.

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
    
    GLuint vao_pyramid;
    GLuint vbo_position_pyramid;
    GLuint vbo_texture_pyramid;
    
    GLuint vao_cube;
    GLuint vbo_position_cube;
    GLuint vbo_texture_cube;
    
    GLuint stone_texture;
    GLuint kundali_texture;
    
    GLuint textureSamplerUniform;
    GLuint mvpMatrixUniform;
    
    vmath::mat4 perspectiveProjectionMatrix;
    
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
        "in vec2 out_texcoord;" \
        "uniform sampler2D u_texture_sampler;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = texture(u_texture_sampler, out_texcoord);" \
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
        
            //get MVP Uniform Location
        mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
        
            //PYRAMID
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
        
        const GLfloat pyramidTexCoords[] =
        {
                //FRONT
            0.5f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
                //RIGHT
            0.5f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
                //BACK
            0.5f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
                //LEFT
            0.5f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f
        };
        
            //CUBE
        const GLfloat cubeVertices[] =
        {
                //FRONT
            1.0f, 1.0f, 1.0f, //right-top
            -1.0f, 1.0f, 1.0f, //left-top
            -1.0f, -1.0f, 1.0f, //left-bottom
            1.0f, -1.0f, 1.0f, //right-bottom
                               //RIGHT
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
                //BACK
            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
                //LEFT
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
                //TOP
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
                //BOTTOM
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
        };
        
        const GLfloat cubeTexCoords[] =
        {
                //FRONT
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
                //RIGHT
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
                //BACK
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
                //LEFT
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
                //TOP
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
                //BOTTOM
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };
        
            //FOR PYRAMID
            //GEN START
        glGenVertexArrays(1, &vao_pyramid);
            //BIND START
        glBindVertexArray(vao_pyramid);
        
            //FOR PYRAMID POSITION
        glGenBuffers(1, &vbo_position_pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
            //FOR PYRAMID TEXTURES
        glGenBuffers(1, &vbo_texture_pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexCoords), pyramidTexCoords, GL_STATIC_DRAW);
        glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
            //BIND END
        glBindVertexArray(0);
        
            //FOR CUBE
            //GEN START
        glGenVertexArrays(1, &vao_cube);
            //BIND START
        glBindVertexArray(vao_cube);
        
            //FOR CUBE POSITION
        glGenBuffers(1, &vbo_position_cube);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
            //FOR CUBE TEXTURE
        glGenBuffers(1, &vbo_position_cube);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoords), cubeTexCoords, GL_STATIC_DRAW);
        glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);

        
            //3D & Depth Code
        glClearDepthf(1.0f);     //Default value passed to the Function
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
            //Disable CULLING while Animation
        glDisable(GL_CULL_FACE);
        
            //SetglColor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black
        
            //Load textures
        stone_texture = [self loadGLTexture:@"Stone" :@"bmp"];
        kundali_texture = [self loadGLTexture:@"Vijay_Kundali" :@"bmp"];
        
            //Enable TExtures
        glEnable(GL_TEXTURE_2D);

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
    //NSBundle *appBundle = [NSBundle mainBundle];
    //NSString *appDirPath = [appBundle bundlePath];
    //NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    
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
        //variables
    static GLfloat anglePyramid = 0.0f;
    static GLfloat angleCube = 0.0f;
    
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        //start using OpenGL program object
    glUseProgram(shaderProgramObject);
    
        //OpenGL Drawing
    vmath::mat4 translationMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    
        //TRIANGLE
    translationMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
    rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
    modelViewMatrix = translationMatrix * rotationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT
    
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, stone_texture);
    glUniform1i(textureSamplerUniform, 0);
    
        //bind vao - vertex array object
    glBindVertexArray(vao_pyramid);
    
        //draw
    glDrawArrays(GL_TRIANGLES, 0, 12);
    
        //unbind vao
    glBindVertexArray(0);
    
        //CUBE
    translationMatrix = vmath::mat4::identity();
    vmath::mat4 scaleMatrix = vmath::mat4::identity();
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();
    
    translationMatrix = vmath::translate(1.5f, 0.0f, -6.0f);
    scaleMatrix = vmath::scale(0.85f, 0.85f, 0.85f);
    rotationMatrix = vmath::rotate(angleCube, angleCube, angleCube);
    modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT
    
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, kundali_texture);
    glUniform1i(textureSamplerUniform, 0);
    
        //bind vao - vertex array object
    glBindVertexArray(vao_cube);
    
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
        //Animation logic
    anglePyramid += 1.0f;
    angleCube += 1.0f;
    if(anglePyramid >= 360.0f)
    {
        anglePyramid = 0.0f;
    }
    if(angleCube >= 360.0f)
    {
        angleCube = 0.0f;
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
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
        // code
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
    if (vbo_texture_pyramid)
    {
        glDeleteVertexArrays(1, &vbo_texture_pyramid);
        vbo_texture_pyramid = 0;
    }
    
        //destroy vao
    if (vao_cube)
    {
        glDeleteVertexArrays(1, &vao_cube);
        vao_cube = 0;
    }
    
        //destroy vbo
    if (vbo_position_cube)
    {
        glDeleteVertexArrays(1, &vbo_position_cube);
        vbo_position_cube = 0;
    }
    
        //destroy vbo
    if (vbo_texture_cube)
    {
        glDeleteVertexArrays(1, &vbo_texture_cube);
        vbo_texture_cube = 0;
    }
    
    if(stone_texture)
    {
        glDeleteTextures(1, &stone_texture);
        stone_texture = 0;
    }
    
    if(kundali_texture)
    {
        glDeleteTextures(1, &kundali_texture);
        kundali_texture = 0;
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


