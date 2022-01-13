//
//  GLESView.m
//  DiffuseCube
//
//  Created by STC on 30/07/21.
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
    
    GLuint vao_cube;
    GLuint vbo_position_cube;
    GLuint vbo_normal_cube;
    
    GLuint modelViewMatrixUniform;
    GLuint projectionMatrixUniform;
    GLuint doubleTapUniform; //LKeyPressedUniform;
    GLuint LdUniform;
    GLuint KdUniform;
    GLuint lightPositionUniform;
    
    mat4 perspectiveProjectionMatrix;
        
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
        "in vec3 vNormal;" \
        "uniform mat4 u_model_view_matrix;" \
        "uniform mat4 u_projection_matrix;" \
        "uniform mediump int u_double_tap;" \
        "uniform vec3 u_LD;" \
        "uniform vec3 u_KD;" \
        "uniform vec4 u_light_position;" \
        "out vec3 diffuse_light;" \
        "void main(void)" \
        "{" \
            //LIGHT ENABLED
        "if(u_double_tap == 1)" \
        "{" \
            //STEPS -
            //1. CALCUALTE EYE COORDINATES -> by multiplying position coordinates and model-view matrix
            //2. CALCUALTE NORMAL MATRIX -> by inverse of transpose of upper 3x3 of model-view matrix
            //3. CALCULATE TRANSFORMED NORMALS ->
            //4. CALCULATE 'S' SOURCE OF LIGHT ->
            //5. CALCULATE DIFFUSE LIGHT USING LIGHT EQUATION ->
        
        "vec4 eye_coordinates = u_model_view_matrix * vPosition;" \
        "mat3 normal_matrix = mat3(transpose(inverse(u_model_view_matrix)));" \
        "vec3 tnorm = normalize(normal_matrix * vNormal);" \
        "vec3 s = normalize(vec3(u_light_position - eye_coordinates));" \
        "diffuse_light = u_LD * u_KD * max(dot(s, tnorm), 0.0f);" \
        "}" \
        "gl_Position =  u_projection_matrix * u_model_view_matrix * vPosition;" \
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
        "in vec3 diffuse_light;" \
        "uniform int u_double_tap;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "vec4 color;" \
        "if(u_double_tap == 1)" \
        "{" \
        "color = vec4(diffuse_light, 1.0f);" \
        "}" \
        "else" \
        "{" \
        "color = vec4(1.0f, 1.0f, 1.0f, 1.0f);" \
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
                    printf("Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
                    
                }
            }
        }
        
            //get Uniform Location
        modelViewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_model_view_matrix");
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
        doubleTapUniform = glGetUniformLocation(shaderProgramObject, "u_double_tap");
        LdUniform = glGetUniformLocation(shaderProgramObject, "u_LD");
        KdUniform = glGetUniformLocation(shaderProgramObject, "u_KD");
        lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position");
        
            //vertices, colours, shader attribs, vbo, vao initializations
            //CUBE
        const GLfloat cubeVertices[] =
        {
                //TOP
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
                //BOTTOM
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
                //FRONT
            1.0f, 1.0f, 1.0f, //right-top
            -1.0f, 1.0f, 1.0f, //left-top
            -1.0f, -1.0f, 1.0f, //left-bottom
            1.0f, -1.0f, 1.0f, //right-bottom
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
                //RIGHT
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f
            
        };
        
        const GLfloat cubeNormals[] =
        {
                // top surface
            0.0f, 1.0f, 0.0f,  // top-right of top
            0.0f, 1.0f, 0.0f, // top-left of top
            0.0f, 1.0f, 0.0f, // bottom-left of top
            0.0f, 1.0f, 0.0f,  // bottom-right of top
            
                // bottom surface
            0.0f, -1.0f, 0.0f,  // top-right of bottom
            0.0f, -1.0f, 0.0f,  // top-left of bottom
            0.0f, -1.0f, 0.0f,  // bottom-left of bottom
            0.0f, -1.0f, 0.0f,   // bottom-right of bottom
            
                // front surface
            0.0f, 0.0f, 1.0f,  // top-right of front
            0.0f, 0.0f, 1.0f, // top-left of front
            0.0f, 0.0f, 1.0f, // bottom-left of front
            0.0f, 0.0f, 1.0f,  // bottom-right of front
            
                // back surface
            0.0f, 0.0f, -1.0f,  // top-right of back
            0.0f, 0.0f, -1.0f, // top-left of back
            0.0f, 0.0f, -1.0f, // bottom-left of back
            0.0f, 0.0f, -1.0f,  // bottom-right of back
            
                // left surface
            -1.0f, 0.0f, 0.0f, // top-right of left
            -1.0f, 0.0f, 0.0f, // top-left of left
            -1.0f, 0.0f, 0.0f, // bottom-left of left
            -1.0f, 0.0f, 0.0f, // bottom-right of left
            
                // right surface
            1.0f, 0.0f, 0.0f,  // top-right of right
            1.0f, 0.0f, 0.0f,  // top-left of right
            1.0f, 0.0f, 0.0f,  // bottom-left of right
            1.0f, 0.0f, 0.0f  // bottom-right of right
        };
        
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
        
            //FOR CUBE NORMALS
        glGenBuffers(1, &vbo_normal_cube);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_cube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
        glVertexAttribPointer(KAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(KAB_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);
        
            //3D & Depth Code
        glClearDepthf(1.0f);     //Default value passed to the Function
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
            //Disable CULLing while animation
        glDisable(GL_CULL_FACE);
        
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


-(void)drawView:(id)sender
{
        // code
        //variables
    static GLfloat angleCube = 0.0f;
    
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        //start using OpenGL program object
    glUseProgram(shaderProgramObject);
    
    
    if (double_tap == 1)
    {
        glUniform1i(doubleTapUniform, 1);
        glUniform3f(LdUniform, 1.0f, 1.0f, 1.0f); //white
        glUniform3f(KdUniform, 0.5f, 0.5f, 0.5f); //grey
        GLfloat lightPosition[] = { 0.0f,0.0f,2.0f,1.0f };
        glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);
    }
    else
    {
        glUniform1i(doubleTapUniform, 0);
    }
    
        //OpenGL Drawing
    mat4 translationMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
    
        //CUBE
    translationMatrix = translate(0.0f, 0.0f, -6.0f);
    scaleMatrix = scale(0.85f, 0.85f, 0.85f);
    rotationMatrix = rotate(angleCube, angleCube, angleCube);
    modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    
    glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    
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
    
        //Animation
    if(single_tap == 1) //animation on
    {
        printf("INSIDE ANIMATION : single tap = %d\n", single_tap);
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
    if (vbo_normal_cube)
    {
        glDeleteVertexArrays(1, &vbo_normal_cube);
        vbo_normal_cube = 0;
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



