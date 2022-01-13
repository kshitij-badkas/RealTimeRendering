//
//  GLESView.m
//  PerVertexFragmentToggle
//
//  Created by STC on 24/11/21.
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
    vec3 lightAmbient;
    vec3 lightDiffuse;
    vec3 lightSpecular;
    vec4 lightPosition;
    
    vec3 materialAmbient;
    vec3 materialDiffuse;
    vec3 materialSpecular;
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
        
            //variables
        lightAmbient = vec3(0.1f, 0.1f, 0.1f);
        lightDiffuse = vec3(1.0f, 1.0f, 1.0f);
        lightSpecular = vec3(1.0f, 1.0f, 1.0f);
        lightPosition = vec4(100.0f, 100.0f, 100.0f, 1.0f);
        
        materialAmbient = vec3(0.0f, 0.0f, 0.0f);
        materialDiffuse = vec3(0.5f, 0.2f, 0.7f);
        materialSpecular = vec3(0.7f, 0.7f, 0.7f);
        materialShininess = 128.0f;
        
        lightEnabled = 0;
        perVertex = 1;
        perFragment = 0;
        
        
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
    
        //OpenGL Drawing
    mat4 translationMatrix = mat4::identity(); //model
    mat4 viewMatrix = mat4::identity(); //view
    mat4 projectionMatrix = mat4::identity(); //projection
    
    //translationMatrix = translate(0.0f, 0.0f, -3.0f);
    //projectionMatrix = perspectiveProjectionMatrix;
    
        //start using OpenGL program object
    //glUseProgram(shaderProgramObject_PV); //using Per-vertex by default
    
        //USING PER-VERTEX LIGHITNG
    if (perVertex == 1)
    {
        glUseProgram(shaderProgramObject_PV);
        
        translationMatrix = translate(0.0f, 0.0f, -3.0f);
        projectionMatrix = perspectiveProjectionMatrix;
        
        glUniform4fv(lightPositionUniform_PV, 1, lightPosition);
        if (lightEnabled == 1)
        {
            glUniform1i(LKeyPressedUniform_PV, 1);
            glUniform3fv(LaUniform_PV, 1, lightAmbient);
            glUniform3fv(LdUniform_PV, 1, lightDiffuse);
            glUniform3fv(LsUniform_PV, 1, lightSpecular);
            glUniform3fv(KaUniform_PV, 1, materialAmbient);
            glUniform3fv(KdUniform_PV, 1, materialDiffuse);
            glUniform3fv(KsUniform_PV, 1, materialSpecular);
            glUniform1f(materialShininessUniform_PV, materialShininess);
            
        }
        else
        {
            glUniform1i(LKeyPressedUniform_PV, 0);
            
        }
        
            //sending MODEL, VIEW, PROJECTION Uniforms separately.
        glUniformMatrix4fv(modelMatrixUniform_PV, 1, GL_FALSE, translationMatrix); //translation and model are same
        glUniformMatrix4fv(viewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
        
        glBindVertexArray(vao_sphere);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
        glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
        
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        
    }
    
        //USING PER-FRAGMENT LIGHITNG
    else if (perFragment == 1)
    {
            //start using OpenGL program object
        glUseProgram(shaderProgramObject_PF);
        
        translationMatrix = translate(0.0f, 0.0f, -3.0f);
        projectionMatrix = perspectiveProjectionMatrix;
        
        glUniform4fv(lightPositionUniform_PF, 1, lightPosition);
        if (lightEnabled == 1)
        {
            glUniform1i(LKeyPressedUniform_PF, 1);
            glUniform3fv(LaUniform_PF, 1, lightAmbient);
            glUniform3fv(LdUniform_PF, 1, lightDiffuse);
            glUniform3fv(LsUniform_PF, 1, lightSpecular);
            glUniform3fv(KaUniform_PF, 1, materialAmbient);
            glUniform3fv(KdUniform_PF, 1, materialDiffuse);
            glUniform3fv(KsUniform_PF, 1, materialSpecular);
            glUniform1f(materialShininessUniform_PF, materialShininess);
        }
        else
        {
            glUniform1i(LKeyPressedUniform_PF, 0);
        }
        
            //sending MODEL, VIEW, PROJECTION Uniforms separately.
        glUniformMatrix4fv(modelMatrixUniform_PF, 1, GL_FALSE, translationMatrix); //translation and model are same
        glUniformMatrix4fv(viewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_PF, 1, GL_FALSE, projectionMatrix);
        
        glBindVertexArray(vao_sphere);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
        glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
        
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
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
    //single_tap++;
    //printf("single_tap = %d\n", single_tap);
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
    /*
    if(single_tap > 1)
    {
        single_tap = 0;
        printf("single_tap > 1, hence single_tap = %d\n", single_tap);
    }
     */
    
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

