//
//  GLESView.m
//  Checkerboard
//
//  Created by STC on 30/07/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "vmath.h"

#define CHECK_IMAGE_WIDTH 64
#define CHECK_IMAGE_HEIGHT 64

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
    
    GLuint vao_square;
    GLuint vbo_position_square;
    GLuint vbo_texture_square;
    
    GLuint textureSamplerUniform;
    GLuint otherKeyPressedUniform;
    GLuint mvpMatrixUniform;
    
    mat4 perspectiveProjectionMatrix;
    
    //Checkerboard
    GLuint tex_image; //texture
    GLubyte check_image[CHECK_IMAGE_WIDTH][CHECK_IMAGE_HEIGHT][4];
    
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
        
            //get Texture Sampler Uniform Location
        textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_texture_sampler");
            
            //vertices, colours, shader attribs, vbo, vao initializations
            //SQUARE
        const GLfloat squareTexCoords[] =
        {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
        
            //GEN START
        glGenVertexArrays(1, &vao_square);
            //BIND START
        glBindVertexArray(vao_square);
        
            //FOR PYRAMID POSITION
        glGenBuffers(1, &vbo_position_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
        glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); //new
        glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
            //FOR SQUARE TEXCOORD
        glGenBuffers(1, &vbo_texture_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_square);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), squareTexCoords, GL_STATIC_DRAW);
        glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
            //BIND END
        glBindVertexArray(0);
        
            //3D & Depth Code
        glClearDepthf(1.0f);     //Default value passed to the Function
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
            //we remove culling of faces for 3D
        glDisable(GL_CULL_FACE);
        
            //SetglColor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black
        
            //Load textures
        [self loadGLTexture];
        
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


-(void)loadGLTexture
{
        //code
    [self MakeCheckImage];
    
    glGenTextures(1, &tex_image);
    glBindTexture(GL_TEXTURE_2D, tex_image);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //make 2nd parameter '1' in PP instead of '4' for Faster Performance.
    
        //Setting texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECK_IMAGE_WIDTH, CHECK_IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, check_image);
    
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}


-(void)MakeCheckImage
{
        //code
    int i, j, c;
    for (i = 0; i < CHECK_IMAGE_HEIGHT; i++)
    {
        for (j = 0; j < CHECK_IMAGE_WIDTH; j++)
        {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
            check_image[i][j][0] = (GLubyte)c;
            check_image[i][j][1] = (GLubyte)c;
            check_image[i][j][2] = (GLubyte)c;
            check_image[i][j][3] = 255;
        }
    }
}


-(void)drawView:(id)sender
{
        // code
        //variables
    GLfloat squareVertices[12];
    
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        //start using OpenGL program object
    glUseProgram(shaderProgramObject);
    
    
        //OpenGL Drawing
    mat4 translationMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();
    
        //SQUARE
    translationMatrix = translate(0.0f, 0.0f, -3.6f);
    modelViewMatrix = translationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT
    
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_image);
    glUniform1i(textureSamplerUniform, 0);
    
        //bind vao - vertex array object
    glBindVertexArray(vao_square);
    
        //draw
    //------------------------------draw start-------------------------------------//
    
        //FRONT-FACING QUAD
    squareVertices[0] = -2.0f;
    squareVertices[1] = -1.0f;
    squareVertices[2] = 0.0f;
    
    squareVertices[3] = -2.0f;
    squareVertices[4] = 1.0f;
    squareVertices[5] = 0.0f;
    
    squareVertices[6] = 0.0f;
    squareVertices[7] = 1.0f;
    squareVertices[8] = 0.0f;
    
    squareVertices[9] = 0.0f;
    squareVertices[10] = -1.0f;
    squareVertices[11] = 0.0f;
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), squareVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    //-------------------------------------------------------------------//
    
        //ANGLUAR QUAD
    squareVertices[0] = 1.0f;
    squareVertices[1] = -1.0f;
    squareVertices[2] = 0.0f;
    
    squareVertices[3] = 1.0f;
    squareVertices[4] = 1.0f;
    squareVertices[5] = 0.0f;
    
    squareVertices[6] = 2.41421f;
    squareVertices[7] = 1.0f;
    squareVertices[8] = -1.41421f;
    
    squareVertices[9] = 2.41421f;
    squareVertices[10] = -1.0f;
    squareVertices[11] = -1.41421f;
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), squareVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
        //unbind vao
    glBindVertexArray(0);
    
    //-------------------------------draw end-------------------------------------//

        //stop using OpenGL program object
    glUseProgram(0);
    
    
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
    
        //destroy vbo
    if (vbo_texture_square)
    {
        glDeleteVertexArrays(1, &vbo_texture_square);
        vbo_texture_square = 0;
    }
    
    if(tex_image)
    {
        glDeleteTextures(1, &tex_image);
        tex_image = 0;
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


