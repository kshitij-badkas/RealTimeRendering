//
//  GLESView.m
//  TwoLightsPyramid
//
//  Created by STC on 26/11/21.
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
    
    
    vmath::mat4 perspectiveProjectionMatrix;
    
    unsigned int single_tap;

    
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
            //"temp = ambient[0] + diffuse[0] + specular[0];" \
            //"phong_ads_light =  temp + ambient[1] + diffuse[1] + specular[1];" \
            
        "phong_ads_light =  ambient[0] + diffuse[0] + specular[0] + ambient[1] + diffuse[1] + specular[1];" \
        //"phong_ads_light =  ambient[i] + diffuse[i] + specular[i];" \
        
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
        glClearDepthf(1.0f);     //Default value passed to the Function
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
            //Disable CULLing while animation
        glDisable(GL_CULL_FACE);
        
  
        single_tap = 0;
        
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
    static GLfloat anglePyramid = 0.0f;
    
        // code
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        //start using OpenGL program object
    glUseProgram(shaderProgramObject);
    
    if (single_tap == 1)
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
    
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    
        //ANIMATION CODE
    anglePyramid += 1.0f;
    if (anglePyramid >= 360.0f)
    {
        anglePyramid = 0.0f;
    }
    
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
    single_tap++;
    printf("single_tap = %d\n", single_tap);
    
    if(single_tap > 1)
    {
        single_tap = 0;
        printf("single_tap > 1, hence single_tap = %d\n", single_tap);
    }
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
