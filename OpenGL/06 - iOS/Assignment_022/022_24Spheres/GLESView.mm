
//
//  GLESView.mm
//  TwnetyFourSpheres
//
//  Created by STC on 25/11/21.
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
    
    unsigned int lightEnabled; // light toggle
    
    GLuint perVertex; //Per-Vertex Lightling Toggle
    GLuint perFragment; //Per-Fragment Lightling Toggle
    
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
    
    GLfloat updated_width;
    GLfloat updated_height;
    
        //Light Rotation
    GLfloat XRotation;
    GLfloat YRotation;
    GLfloat ZRotation;
    
    GLfloat radius;
    
    GLuint single_tap; // = 1 for X Axis; = 2 for Y Axis; = 3 for Z Axis.
    
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
        
            ///////////////////////////////////////////
            //          PER - VERTEX SHADER
            ///////////////////////////////////////////
        
        vertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);
        
            //provide source code to shader
            //use 'core' profile i.e PROGRAMMABLE PIPELINE
        const GLchar* vertexShaderSourceCode_PV =
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
                    printf("PER-VERTEX - Vertex Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
                }
            }
        }
        
            //FRAGMENT SHADER
            //create shader
        fragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);
        
            //provide source code to shader
        const GLchar* fragmentShaderSourceCode_PV =
        "#version 300 es" \
        "\n" \
        "precision highp float;" \
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
                    printf("PER-VERTEX  - Fragment Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
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
                    printf(" PER-VERTEX -  Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
                    
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
        
            ///////////////////////////////////////////
            //          PER - FRAGMENT SHADER
            ///////////////////////////////////////////
        
        vertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);
        
            //provide source code to shader
            //use 'core' profile i.e PROGRAMMABLE PIPELINE
        const GLchar* vertexShaderSourceCode_PF =
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
        iInfoLogLength = 0;
        iShaderCompiledStatus = 0;
        szInfoLog = NULL;
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
                    printf("PER-FRAGMENT -  Vertex Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
                }
            }
        }
        
            //FRAGMENT SHADER
            //create shader
        fragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);
        
            //provide source code to shader
        const GLchar* fragmentShaderSourceCode_PF =
        "#version 300 es" \
        "\n" \
        "precision highp float;" \
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
                    printf("PER-FRAGMNET  - Fragment Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
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
        
        glBindAttribLocation(shaderProgramObject_PF, KAB_ATTRIBUTE_NORMAL, "vNormal");
        
        
            //link shader
        glLinkProgram(shaderProgramObject_PF);
        
            //Shader Program Link Error Checking
        iShaderProgramLinkStatus = 0;
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
                    printf("PER-FRAGMENT - Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self uninitialize];
                    [self release];
                    exit(0);
                    
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
    
        
        lightEnabled = 0;
        perVertex = 1;
        perFragment = 0;
        
        updated_width = 0.0f;
        updated_height = 0.0f;
        
            //Light Rotation
        XRotation = 0.0f;
        YRotation = 0.0f;
        ZRotation = 0.0f;
        
        single_tap = 0; // = 1 for X Axis; = 2 for Y Axis; = 3 for Z Axis.
        
        radius = 5.0f;
        
        
            //SetglColor
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);//Black
        
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
    
    updated_width = width;
    updated_height = height;
    
    [self drawView:nil];
}


-(void)drawView:(id)sender
{
    
        //variables
        // material will have 24 different values
    GLfloat materialAmbient1[] = { 0.0215f, 0.1745f, 0.0215f, 1.0f };
    GLfloat materialDiffuse1[] = { 0.07568f, 0.61424f, 0.07568f, 1.0f };
    GLfloat materialSpecular1[] = { 0.633f, 0.727811f, 0.633f, 1.0f };
    GLfloat materialShininess1 = 0.6f * 128.0f;
    
    GLfloat materialAmbient2[] = { 0.135f, 0.225f, 0.1575f, 1.0f };
    GLfloat materialDiffuse2[] = { 0.54f, 0.89f, 0.63f, 1.0f };
    GLfloat materialSpecular2[] = { 0.316228f, 0.316228f, 0.316228f, 1.0f };
    GLfloat materialShininess2 = 0.1f * 128.0f;
    
    GLfloat materialAmbient3[] = { 0.05375f, 0.05f, 0.06625f, 1.0f };
    GLfloat materialDiffuse3[] = { 0.18275f, 0.17f, 0.22525f, 1.0f };
    GLfloat materialSpecular3[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat materialShininess3 = 0.3f * 128.0f;
    
    GLfloat materialAmbient4[] = { 0.25f, 0.20725f, 0.20725f, 1.0f };
    GLfloat materialDiffuse4[] = { 1.0f, 0.829f, 0.829f, 1.0f };
    GLfloat materialSpecular4[] = { 0.296648f, 0.296648f, 0.296648f, 1.0f };
    GLfloat materialShininess4 = 0.088f * 128.0f;
    
    GLfloat materialAmbient5[] = { 0.1745f, 0.01175f, 0.01175f, 1.0f };
    GLfloat materialDiffuse5[] = { 0.61424f, 0.04136f, 0.04136f, 1.0f };
    GLfloat materialSpecular5[] = { 0.727811f, 0.626959f, 0.626959f, 1.0f };
    GLfloat materialShininess5 = 0.6f * 128.0f;
    
    GLfloat materialAmbient6[] = { 0.1f, 0.18725f, 0.1745f, 1.0f };
    GLfloat materialDiffuse6[] = { 0.396f, 0.74151f, 0.69102f, 1.0f };
    GLfloat materialSpecular6[] = { 0.297254f, 0.30829f, 0.306678f, 1.0f };
    GLfloat materialShininess6 = 0.1f * 128.0f;
    
    GLfloat materialAmbient7[] = { 0.329412f, 0.223529f, 0.027451f, 1.0f };
    GLfloat materialDiffuse7[] = { 0.780392f, 0.568627f, 0.113725f, 1.0f };
    GLfloat materialSpecular7[] = { 0.992157f, 0.941176f, 0.807843f, 1.0f };
    GLfloat materialShininess7 = 0.21794872f * 128.0f;
    
    GLfloat materialAmbient8[] = { 0.2125f, 0.1275f, 0.054f, 1.0f };
    GLfloat materialDiffuse8[] = { 0.714f, 0.4284f, 0.18144f, 1.0f };
    GLfloat materialSpecular8[] = { 0.393548f, 0.271906f, 0.166721f, 1.0f };
    GLfloat materialShininess8 = 0.2f * 128.0f;
    
    GLfloat materialAmbient9[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat materialDiffuse9[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat materialSpecular9[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
    GLfloat materialShininess9 = 0.6f * 128.0f;
    
    GLfloat materialAmbient10[] = { 0.19125f, 0.0735f, 0.0225f, 1.0f };
    GLfloat materialDiffuse10[] = { 0.7038f, 0.27048f, 0.0828f, 1.0f };
    GLfloat materialSpecular10[] = { 0.256777f, 0.137622f, 0.086014f, 1.0f };
    GLfloat materialShininess10 = 0.1f * 128.0f;
    
    GLfloat materialAmbient11[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
    GLfloat materialDiffuse11[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
    GLfloat materialSpecular11[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
    GLfloat materialShininess11 = 0.4f * 128.0f;
    
    GLfloat materialAmbient12[] = { 0.19225f, 0.19225f, 0.19225f, 1.0f };
    GLfloat materialDiffuse12[] = { 0.50754f, 0.50754f, 0.50754f, 1.0f };
    GLfloat materialSpecular12[] = { 0.508273f, 0.508273f, 0.508273f, 1.0f };
    GLfloat materialShininess12 = 0.4f * 128.0f;
    
    GLfloat materialAmbient13[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat materialDiffuse13[] = { 0.01f, 0.01f, 0.01f, 1.0f };
    GLfloat materialSpecular13[] = { 0.50f, 0.50f, 0.50f, 1.0f };
    GLfloat materialShininess13 = 0.25f * 128.0f;
    
    GLfloat materialAmbient14[] = { 0.0f, 0.1f, 0.06f, 1.0f };
    GLfloat materialDiffuse14[] = { 0.0f, 0.50980392f, 0.50980392f, 1.0f };
    GLfloat materialSpecular14[] = { 0.50196078f, 0.50196078f, 0.50196078f, 1.0f };
    GLfloat materialShininess14 = 0.25f * 128.0f;
    
    GLfloat materialAmbient15[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat materialDiffuse15[] = { 0.1f, 0.35f, 0.1f, 1.0f };
    GLfloat materialSpecular15[] = { 0.45f, 0.55f, 0.45f, 1.0f };
    GLfloat materialShininess15 = 0.25f * 128.0f;
    
    GLfloat materialAmbient16[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat materialDiffuse16[] = { 0.5f, 0.0f, 0.0f, 1.0f };
    GLfloat materialSpecular16[] = { 0.7f, 0.6f, 0.6f, 1.0f };
    GLfloat materialShininess16 = 0.25f * 128.0f;
    
    GLfloat materialAmbient17[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat materialDiffuse17[] = { 0.55f, 0.55f, 0.55f, 1.0f };
    GLfloat materialSpecular17[] = { 0.70f, 0.70f, 0.70f, 1.0f };
    GLfloat materialShininess17 = 0.25f * 128.0f;
    
    GLfloat materialAmbient18[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat materialDiffuse18[] = { 0.5f, 0.5f, 0.0f, 1.0f };
    GLfloat materialSpecular18[] = { 0.60f, 0.60f, 0.50f, 1.0f };
    GLfloat materialShininess18 = 0.25f * 128.0f;
    
    GLfloat materialAmbient19[] = { 0.02f, 0.02f, 0.02f, 1.0f };
    GLfloat materialDiffuse19[] = { 0.01f, 0.01f, 0.01f, 1.0f };
    GLfloat materialSpecular19[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat materialShininess19 = 0.078125f * 128.0f;
    
    GLfloat materialAmbient20[] = { 0.0f, 0.05f, 0.05f, 1.0f };
    GLfloat materialDiffuse20[] = { 0.4f, 0.5f, 0.5f, 1.0f };
    GLfloat materialSpecular20[] = { 0.04f, 0.7f, 0.7f, 1.0f };
    GLfloat materialShininess20 = 0.078125f * 128.0f;
    
    GLfloat materialAmbient21[] = { 0.0f, 0.05f, 0.0f, 1.0f };
    GLfloat materialDiffuse21[] = { 0.4f, 0.5f, 0.4f, 1.0f };
    GLfloat materialSpecular21[] = { 0.04f, 0.7f, 0.04f, 1.0f };
    GLfloat materialShininess21 = 0.078125f * 128.0f;
    
    GLfloat materialAmbient22[] = { 0.05f, 0.0f, 0.0f, 1.0f };
    GLfloat materialDiffuse22[] = { 0.5f, 0.4f, 0.4f, 1.0f };
    GLfloat materialSpecular22[] = { 0.7f, 0.04f, 0.04f, 1.0f };
    GLfloat materialShininess22 = 0.078125f * 128.0f;
    
    GLfloat materialAmbient23[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat materialDiffuse23[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat materialSpecular23[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat materialShininess23 = 0.078125f * 128.0f;
    
    GLfloat materialAmbient24[] = { 0.5f, 0.05f, 0.0f, 1.0f };
    GLfloat materialDiffuse24[] = { 0.5f, 0.5f, 0.4f, 1.0f };
    GLfloat materialSpecular24[] = { 0.7f, 0.7f, 0.04f, 1.0f };
    GLfloat materialShininess24 = 0.078125f * 128.0f;
    
        //Light values
    static GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f ,1.0f };
    static GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    static GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    static GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };
        
        // code
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        //OpenGL Drawing
    mat4 translationMatrix = mat4::identity(); //model
    mat4 viewMatrix = mat4::identity(); //view
    mat4 projectionMatrix = mat4::identity(); //projection
    
    translationMatrix = translate(0.0f, 0.0f, -3.0f);
    projectionMatrix = perspectiveProjectionMatrix;
    
        //start using OpenGL program object
    glUseProgram(shaderProgramObject_PV); //using Per-vertex by default
    
        //USING PER-VERTEX LIGHITNG
    if (perVertex == 1)
    {
        glUseProgram(shaderProgramObject_PV);
        
    
        glUniform4fv(lightPositionUniform_PV, 1, lightPosition);
        if (lightEnabled == 1)
        {
            if (single_tap == 1)
            {
                
                    //X AXIS ROATION
                lightPosition[0] = (GLfloat)(radius * sin(YRotation)); //x
                lightPosition[1] = 0.0f; //y
                lightPosition[2] = (GLfloat)(radius * cos(YRotation)); //z
                lightPosition[3] = 1.0f; //w
            }
            else if (single_tap == 2)
            {
                
                    //Y AXIS ROATION
                lightPosition[0] = 0.0f; //x
                lightPosition[1] = (GLfloat)(radius * sin(YRotation)); //y
                lightPosition[2] = (GLfloat)(radius * cos(YRotation)); //z
                lightPosition[3] = 1.0f; //w
            }
            else if (single_tap == 3)
            {
                
                    //Z AXIS ROATION
                lightPosition[0] = (GLfloat)(radius * sin(ZRotation)); //x
                lightPosition[1] = (GLfloat)(radius * cos(ZRotation)); //y
                lightPosition[2] = 0.0f; //z
                lightPosition[3] = 1.0f; //w
            }
            
            glUniform1i(LKeyPressedUniform_PV, 1);
            glUniform3fv(LaUniform_PV, 1, (GLfloat*)lightAmbient);
            glUniform3fv(LdUniform_PV, 1, (GLfloat*)lightDiffuse);
            glUniform3fv(LsUniform_PV, 1, (GLfloat*)lightSpecular);
            
                //bind vao - vertex array object
            glBindVertexArray(vao_sphere);
            
                //draw - sphere 1
            glViewport(0, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient1);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse1);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular1);
            glUniform1f(materialShininessUniform_PV, materialShininess1);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 2
            glViewport(updated_width / 4, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient2);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse2);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular2);
            glUniform1f(materialShininessUniform_PV, materialShininess2);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 3
            glViewport(updated_width / 2, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient3);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse3);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular3);
            glUniform1f(materialShininessUniform_PV, materialShininess3);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 4
            glViewport(updated_width * 3.0f / 4.0f, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient4);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse4);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular4);
            glUniform1f(materialShininessUniform_PV, materialShininess4);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 5
            glViewport(0, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient5);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse5);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular5);
            glUniform1f(materialShininessUniform_PV, materialShininess5);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 6
            glViewport(updated_width / 4, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient6);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse6);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular6);
            glUniform1f(materialShininessUniform_PV, materialShininess6);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 7
            glViewport(updated_width / 2, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient7);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse7);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular7);
            glUniform1f(materialShininessUniform_PV, materialShininess7);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 8
            glViewport(updated_width * 3.0f / 4.0f, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient8);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse8);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular8);
            glUniform1f(materialShininessUniform_PV, materialShininess8);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 9
            glViewport(0, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient9);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse9);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular9);
            glUniform1f(materialShininessUniform_PV, materialShininess9);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 10
            glViewport(updated_width / 4, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient10);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse10);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular10);
            glUniform1f(materialShininessUniform_PV, materialShininess10);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 11
            glViewport(updated_width / 2, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient11);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse11);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular11);
            glUniform1f(materialShininessUniform_PV, materialShininess11);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 12
            glViewport(updated_width * 3.0f / 4.0f, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient12);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse12);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular12);
            glUniform1f(materialShininessUniform_PV, materialShininess12);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 13
            glViewport(0, updated_height * 1.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient13);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse13);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular13);
            glUniform1f(materialShininessUniform_PV, materialShininess13);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 14
            glViewport(updated_width / 4, updated_height * 1.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient14);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse14);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular14);
            glUniform1f(materialShininessUniform_PV, materialShininess14);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 15
            glViewport(updated_width / 2, updated_height * 1.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient15);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse15);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular15);
            glUniform1f(materialShininessUniform_PV, materialShininess15);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 16
            glViewport(updated_width * 3.0f / 4.0f, updated_height * 1.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient16);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse16);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular16);
            glUniform1f(materialShininessUniform_PV, materialShininess16);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 17
            glViewport(0, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient17);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse17);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular17);
            glUniform1f(materialShininessUniform_PV, materialShininess17);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 18
            glViewport(updated_width / 4, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient18);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse18);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular18);
            glUniform1f(materialShininessUniform_PV, materialShininess18);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 19
            glViewport(updated_width / 2, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient19);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse19);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular19);
            glUniform1f(materialShininessUniform_PV, materialShininess19);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 20
            glViewport(updated_width * 3.0f / 4.0f, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient20);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse20);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular20);
            glUniform1f(materialShininessUniform_PV, materialShininess20);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 21
            glViewport(0, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient21);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse21);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular21);
            glUniform1f(materialShininessUniform_PV, materialShininess21);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 22
            glViewport(updated_width / 4, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient22);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse22);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular22);
            glUniform1f(materialShininessUniform_PV, materialShininess22);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 23
            glViewport(updated_width / 2, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient23);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse23);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular23);
            glUniform1f(materialShininessUniform_PV, materialShininess23);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 24
            glViewport(updated_width * 3.0f / 4.0f, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient24);
            glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse24);
            glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular24);
            glUniform1f(materialShininessUniform_PV, materialShininess24);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //unbind
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            
                //unbind vao
            glBindVertexArray(0);
            
        }
        else
        {
            glUniform1i(LKeyPressedUniform_PV, 0);
            
        }
        
            //sending MODEL, VIEW, PROJECTION Uniforms separately.
        glUniformMatrix4fv(modelMatrixUniform_PV, 1, GL_FALSE, translationMatrix); //translation and model are same
        glUniformMatrix4fv(viewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
        
        //glBindVertexArray(vao_sphere);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
        //glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
        
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        //glBindVertexArray(0);
        //glUseProgram(0);
        
    }
    
        //USING PER-FRAGMENT LIGHITNG
    else if (perFragment == 1)
    {
            //start using OpenGL program object
        glUseProgram(shaderProgramObject_PF);
        
        glUniform4fv(lightPositionUniform_PF, 1, (GLfloat*)lightPosition);
        if (lightEnabled == 1)
        {
            if (single_tap == 1)
            {
                
                    //X AXIS ROATION
                lightPosition[0] = (GLfloat)(radius * sin(YRotation)); //x
                lightPosition[1] = 0.0f; //y
                lightPosition[2] = (GLfloat)(radius * cos(YRotation)); //z
                lightPosition[3] = 1.0f; //w
            }
            else if (single_tap == 2)
            {
                
                    //Y AXIS ROATION
                lightPosition[0] = 0.0f; //x
                lightPosition[1] = (GLfloat)(radius * sin(YRotation)); //y
                lightPosition[2] = (GLfloat)(radius * cos(YRotation)); //z
                lightPosition[3] = 1.0f; //w
            }
            else if (single_tap == 3)
            {
                    //Z AXIS ROATION
                lightPosition[0] = (GLfloat)(radius * sin(ZRotation)); //x
                lightPosition[1] = (GLfloat)(radius * cos(ZRotation)); //y
                lightPosition[2] = 0.0f; //z
                lightPosition[3] = 1.0f; //w
            }
            
            glUniform1i(LKeyPressedUniform_PF, 1);
            glUniform3fv(LaUniform_PF, 1, (GLfloat*)lightAmbient);
            glUniform3fv(LdUniform_PF, 1, (GLfloat*)lightDiffuse);
            glUniform3fv(LsUniform_PF, 1, (GLfloat*)lightSpecular);
            
                //bind vao - vertex array object
            glBindVertexArray(vao_sphere);
            
                //draw - sphere 1
            glViewport(0, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient1);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse1);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular1);
            glUniform1f(materialShininessUniform_PF, materialShininess1);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 2
            glViewport(updated_width / 4, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient2);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse2);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular2);
            glUniform1f(materialShininessUniform_PF, materialShininess2);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 3
            glViewport(updated_width / 2, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient3);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse3);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular3);
            glUniform1f(materialShininessUniform_PF, materialShininess3);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 4
            glViewport(updated_width * 3.0f / 4.0f, updated_height * 4.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient4);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse4);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular4);
            glUniform1f(materialShininessUniform_PF, materialShininess4);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 5
            glViewport(0, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient5);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse5);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular5);
            glUniform1f(materialShininessUniform_PF, materialShininess5);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 6
            glViewport(updated_width / 4, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient6);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse6);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular6);
            glUniform1f(materialShininessUniform_PF, materialShininess6);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 7
            glViewport(updated_width / 2, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient7);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse7);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular7);
            glUniform1f(materialShininessUniform_PF, materialShininess7);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 8
            glViewport(updated_width * 3.0f / 4.0f, updated_height * 3.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient8);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse8);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular8);
            glUniform1f(materialShininessUniform_PF, materialShininess8);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 9
            glViewport(0, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient9);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse9);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular9);
            glUniform1f(materialShininessUniform_PF, materialShininess9);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 10
            glViewport(updated_width / 4, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient10);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse10);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular10);
            glUniform1f(materialShininessUniform_PF, materialShininess10);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 11
            glViewport(updated_width / 2, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient11);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse11);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular11);
            glUniform1f(materialShininessUniform_PF, materialShininess11);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 12
            glViewport(updated_width * 3.0f / 4.0f, updated_height * 2.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient12);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse12);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular12);
            glUniform1f(materialShininessUniform_PF, materialShininess12);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 13
            glViewport(0, updated_height * 1.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient13);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse13);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular13);
            glUniform1f(materialShininessUniform_PF, materialShininess13);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 14
            glViewport(updated_width / 4, updated_height * 1.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient14);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse14);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular14);
            glUniform1f(materialShininessUniform_PF, materialShininess14);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 15
            glViewport(updated_width / 2, updated_height * 1.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient15);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse15);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular15);
            glUniform1f(materialShininessUniform_PF, materialShininess15);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 16
            glViewport(updated_width * 3.0f / 4.0f, updated_height * 1.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient16);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse16);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular16);
            glUniform1f(materialShininessUniform_PF, materialShininess16);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 17
            glViewport(0, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient17);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse17);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular17);
            glUniform1f(materialShininessUniform_PF, materialShininess17);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 18
            glViewport(updated_width / 4, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient18);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse18);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular18);
            glUniform1f(materialShininessUniform_PF, materialShininess18);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 19
            glViewport(updated_width / 2, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient19);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse19);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular19);
            glUniform1f(materialShininessUniform_PF, materialShininess19);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 20
            glViewport(updated_width * 3.0f / 4.0f, updated_height * 0.65f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient20);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse20);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular20);
            glUniform1f(materialShininessUniform_PF, materialShininess20);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 21
            glViewport(0, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient21);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse21);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular21);
            glUniform1f(materialShininessUniform_PF, materialShininess21);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 22
            glViewport(updated_width / 4, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient22);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse22);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular22);
            glUniform1f(materialShininessUniform_PF, materialShininess22);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 23
            glViewport(updated_width / 2, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient23);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse23);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular23);
            glUniform1f(materialShininessUniform_PF, materialShininess23);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //draw - sphere 24
            glViewport(updated_width * 3.0f / 4.0f, updated_height * -0.3f / 6.0f, GLsizei(updated_width / 4), GLsizei(updated_height / 4));
            glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient24);
            glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse24);
            glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular24);
            glUniform1f(materialShininessUniform_PF, materialShininess24);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
            
                //unbind
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            
                //unbind vao
            glBindVertexArray(0);
        }
        else
        {
            glUniform1i(LKeyPressedUniform_PF, 0);
        }
        
            //sending MODEL, VIEW, PROJECTION Uniforms separately.
        glUniformMatrix4fv(modelMatrixUniform_PF, 1, GL_FALSE, translationMatrix); //translation and model are same
        glUniformMatrix4fv(viewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_PF, 1, GL_FALSE, projectionMatrix);
        
        //glBindVertexArray(vao_sphere);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
        //glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
        
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        //glBindVertexArray(0);
        //glUseProgram(0);
    }
    /*
     else
     {
     //PER-VERTEX SHOULD BE ENABLED BY DEFAULT - HENCE, WRTING OUT OF IF-ELSE BLOCK
     //sending MODEL, VIEW, PROJECTION Uniforms separately.
     glUniformMatrix4fv(modelMatrixUniform_PV, 1, GL_FALSE, translationMatrix); //translation and model are same
     glUniformMatrix4fv(viewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
     glUniformMatrix4fv(projectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
     
     glBindVertexArray(vao_sphere);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
     glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
     
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
     glBindVertexArray(0);
     //glUseProgram(0);
     }
     */
    glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    
        //ANIMATION LOGIC
    XRotation += 0.05f;
    YRotation += 0.05f;
    ZRotation += 0.05f;
    if (XRotation >= 360.0f)
    {
        XRotation = 0.0f;
    }
    if (YRotation >= 360.0f)
    {
        YRotation = 0.0f;
    }
    if (ZRotation >= 360.0f)
    {
        ZRotation = 0.0f;
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
    
    if(single_tap > 4)
    {
        single_tap = 0;
        printf("single_tap > 4, hence single_tap = %d\n", single_tap);
    }
    
    /*
     if(perVertex == 0)
     {
     perVertex = 1;
     //perFragment = 0;
     }
     else if (perVertex == 1)
     {
     perVertex = 0;
     }
     */
    if(perFragment == 0)
    {
        perVertex = 0;
        perFragment = 1;
    }
    else if (perFragment == 1)
    {
        perFragment = 0;
    }
    
     
     
    
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
        // code
    if(lightEnabled == 0)
    {
        lightEnabled = 1;
        perVertex = 1;
        perFragment = 0;
    }
    else
    {
        lightEnabled = 0;
    }
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
        // code
    [self uninitialize];
    [self release];
    exit(0);
}

-(void)onLongPress:(UITapGestureRecognizer *)gr
{
        // code
    if(perFragment == 0)
    {
        perVertex = 0;
        perFragment = 1;
    }
    else if (perFragment == 1)
    {
        perFragment = 0;
    }
    
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
    
        //SAFE SHADER CLEANUP - PER VERTEX
    if (shaderProgramObject_PV)
    {
        glUseProgram(shaderProgramObject_PV);
        
        GLsizei shaderCount;
        glGetProgramiv(shaderProgramObject_PV, GL_ATTACHED_SHADERS, &shaderCount);
        
        GLuint* pShader = NULL;
        pShader = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
        
        if (pShader == NULL)
        {
                //error checking
            printf("PER VERTEX  - Error Creating pShader.\n Exitting Now!!\n");
            [self uninitialize];
            [self release];
            exit(0);
        }
        
        glGetAttachedShaders(shaderProgramObject_PV, shaderCount, &shaderCount, pShader);
        
        for (GLsizei i = 0; i < shaderCount; i++)
        {
            glDetachShader(shaderProgramObject, pShader[i]);
            glDeleteShader(pShader[i]);
            pShader[i] = 0;
        }
        
        free(pShader);
        
        glDeleteProgram(shaderProgramObject_PV);
        shaderProgramObject_PV = 0;
        
            //unlink
        glUseProgram(0);
    }
    
        //SAFE SHADER CLEANUP - PER FRAGMENT
    if (shaderProgramObject_PF)
    {
        glUseProgram(shaderProgramObject_PF);
        
        GLsizei shaderCount;
        glGetProgramiv(shaderProgramObject_PF, GL_ATTACHED_SHADERS, &shaderCount);
        
        GLuint* pShader = NULL;
        pShader = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
        
        if (pShader == NULL)
        {
                //error checking
            printf("PER - FRAGMENT Error Creating pShader.\n Exitting Now!!\n");
            [self uninitialize];
            [self release];
            exit(0);
        }
        
        glGetAttachedShaders(shaderProgramObject_PF, shaderCount, &shaderCount, pShader);
        
        for (GLsizei i = 0; i < shaderCount; i++)
        {
            glDetachShader(shaderProgramObject_PF, pShader[i]);
            glDeleteShader(pShader[i]);
            pShader[i] = 0;
        }
        
        free(pShader);
        
        glDeleteProgram(shaderProgramObject_PF);
        shaderProgramObject_PF = 0;
        
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

