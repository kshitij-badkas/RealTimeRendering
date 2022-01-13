    //
    //  GLESView.m
    //  GraphPaper
    //
    //  Created by STC on 25/11/21.
    //

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "vmath.h"

using namespace vmath;


@implementation GLESView
{
@private
    enum
    {
        KAB_ATTRIBUTE_POSITION = 0,
        KAB_ATTRIBUTE_COLOR,
        KAB_ATTRIBUTE_NORMAL,
        KAB_ATTRIBUTE_TEXCOORD
    };
    
    EAGLContext *eaglContext;
    
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    GLuint vao_Lines;
    GLuint vbo_Position_Lines;
    GLuint vbo_Color_Lines;
    
    GLuint mvpMatrixUniform;
    GLuint colorUniform;
    
    mat4 perspectiveProjectionMatrix;
    
    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
    
    
    
}

-(id)initWithFrame:(CGRect)frame
{
        //prototype
    
        // code
    self = [super initWithFrame:frame];
    if(self)
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)[super layer];
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
        "precision mediump int;" \
        "precision highp float;" \
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
                    printf("Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
                    
                }
            }
        }
        
            //get Uniform Location
        mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
        
        colorUniform = glGetUniformLocation(shaderProgramObject, "u_Color");
        
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
        glClearDepthf(1.0f);     //Default value passed to the Function
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
            //Disable CULLing while animation
        glDisable(GL_CULL_FACE);
        
            //SetglColor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black
        
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


-(void)drawView:(id)sender
{
        //variables
        //CO-ORDINATES
    GLfloat lines[2000];  //each line has 4 coordinates x 20 - 20 lines drawn - they will change coordinates
    GLfloat horizontalCentreLine[] = {-1.0f, 0.0f, 1.0f, 0.0f}; //central horizontal line
    GLfloat verticalCentreLine[] = { 0.0f, 1.0f, 0.0f, -1.0f }; //central vertical line
    
    GLfloat redColor[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat greenColor[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat blueColor[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat whiteColor[] = { 1.0, 1.0, 1.0, 1.0 };
    
        // code
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
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
    glDrawArrays(GL_LINES, 0, 629); //1000 indices to render
    
    glUniform4fv(colorUniform, 1, (GLfloat*)whiteColor);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Color_Lines);
    glBufferData(GL_ARRAY_BUFFER, 1300 * 2 * sizeof(GLfloat), lines, GL_DYNAMIC_DRAW); // 40 vertices with (x, y)
    glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_LINES, 0, 629); //40 indices to render for 20 lines
    
    
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
    
        //  SEMIPERIMETER
    perimeter = (GLfloat)(distance_A + distance_B + distance_C);
    semi = (GLfloat)(perimeter / 2);
    
        //  AREA OF TRIANGLE
    tArea = (GLfloat)sqrt(semi * (semi - distance_A) * (semi - distance_B) * (semi - distance_C));
    
        //  INCIRCLE COORDINATES
    iX = (GLfloat)(distance_A * (0.0f) + distance_B * (0.7f) + distance_C * (-0.7f)) / perimeter;
    iY = (GLfloat)(distance_A * (0.7f) + distance_B * (-0.7f) + distance_C * (-0.7f)) / perimeter;
    
    inCircleRadius = (GLfloat)(tArea / semi);
    
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
    glDrawArrays(GL_LINES, 0, 600); //40 indices to render for 20 lines
    
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


