//
//  GLESView.m
//  PerVertex
//
//  Created by STC on 23/11/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "vmath.h"

#import "Sphere.h"

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
    
    GLuint vao_sphere;
    GLuint vbo_sphere_position;
    GLuint vbo_sphere_normal;
    GLuint vbo_sphere_element;
        
    float sphere_vertices[1146];
    float sphere_normals[1146];
    float sphere_textures[764];
    short sphere_elements[2280];
    int numVertices;
    int numElements; //Elements a.k.a Indices
    
    vmath::mat4 perspectiveProjectionMatrix;
    
    unsigned int single_tap;

    GLuint LKeyPressedUniform; //double tap
    
        //MODEL-VIEW-PROJECTION UNIFORMS
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
    
    GLfloat lightAmbient[4];
    GLfloat lightDiffuse[4];
    GLfloat lightSpecular[4];
    GLfloat lightPosition[4];
    
    GLfloat materialAmbient[4];
    GLfloat materialDiffuse[4];
    GLfloat materialSpecular[4];
    GLfloat materialShininess;
    
    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
    
    
    
}

-(id)initWithFrame:(CGRect)frame
{
        //prototypes
    void getSphereVertexData(float[], float[], float[], short[]);
    int getNumberOfSphereVertices(void);
    int getNumberOfSphereElements(void);
    
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
        "uniform vec3 u_LA;" \
        "uniform vec3 u_LD;" \
        "uniform vec3 u_LS;" \
        "uniform vec4 u_light_position;" \
        "uniform vec3 u_KA;" \
        "uniform vec3 u_KD;" \
        "uniform vec3 u_KS;" \
        "uniform float u_material_shininess;" \
        "out vec3 phong_ads_light;" \
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
        LaUniform = glGetUniformLocation(shaderProgramObject, "u_LA");
        LdUniform = glGetUniformLocation(shaderProgramObject, "u_LD");
        LsUniform = glGetUniformLocation(shaderProgramObject, "u_LS");
        lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position");
        KaUniform = glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform = glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform = glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
        
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
        glClearDepthf(1.0f);     //Default value passed to the Function
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
            //Disable CULLing while animation
        glDisable(GL_CULL_FACE);
                
            //variables
        lightAmbient[0] = 0.1f;
        lightAmbient[1] = 0.1f;
        lightAmbient[2] = 0.1f;
        lightAmbient[3] = 0.1f;
        
        lightDiffuse[0] = 1.0f;
        lightDiffuse[1] = 1.0f;
        lightDiffuse[2] = 1.0f;
        lightDiffuse[3] = 1.0f;
        
        lightSpecular[0] = 1.0f;
        lightSpecular[1] = 1.0f;
        lightSpecular[2] = 1.0f;
        lightSpecular[3] = 1.0f;
        
        lightPosition[0] = 100.0f;
        lightPosition[1] = 100.0f;
        lightPosition[2] = 100.0f;
        lightPosition[3] = 1.0f;

        
        materialAmbient[0] = 0.0f;
        materialAmbient[1] = 0.0f;
        materialAmbient[2] = 0.0f;
        materialAmbient[3] = 1.0f;
        
        materialDiffuse[0] = 0.5;
        materialDiffuse[1] = 0.2f;
        materialDiffuse[2] = 0.7f;
        materialDiffuse[3] = 1.0f;
        
        materialSpecular[0] = 0.7f;
        materialSpecular[1] = 0.7f;
        materialSpecular[2] = 0.7f;
        materialSpecular[3] = 1.0f;

        materialShininess = 50.0f;
        
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
        // code
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        //start using OpenGL program object
    glUseProgram(shaderProgramObject);
    
    glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);
    if (single_tap == 1)
    {
        glUniform1i(LKeyPressedUniform, 1);
        glUniform3fv(LaUniform, 1, (GLfloat*)lightAmbient);
        glUniform3fv(LdUniform, 1, (GLfloat*)lightDiffuse);
        glUniform3fv(LsUniform, 1, (GLfloat*)lightSpecular);
        glUniform3fv(KaUniform, 1, (GLfloat*)materialAmbient);
        glUniform3fv(KdUniform, 1, (GLfloat*)materialDiffuse);
        glUniform3fv(KsUniform, 1, (GLfloat*)materialSpecular);
        glUniform1f(materialShininessUniform, materialShininess);
        
            //bind vao - vertex array object
        glBindVertexArray(vao_sphere);
        
            //draw
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
        glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
        
            //unbind
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
            //unbind vao
        glBindVertexArray(0);
    
    }
    else
    {
        glUniform1i(LKeyPressedUniform, 0);
    }
    
        //OpenGL Drawing
    mat4 translationMatrix = mat4::identity(); //model
    mat4 viewMatrix = mat4::identity(); //view
    mat4 projectionMatrix = mat4::identity(); //projection
    
    translationMatrix = translate(0.0f, 0.0f, -3.0f);
    projectionMatrix = perspectiveProjectionMatrix;
    
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, translationMatrix); //translation and model are same
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);
    
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
