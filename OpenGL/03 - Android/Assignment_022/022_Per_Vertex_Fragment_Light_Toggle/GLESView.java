package com.example.per_vertex_fragment_light_toggle;

import android.content.Context; // for drawing context related
import android.opengl.GLSurfaceView; //for OPenGL Surface View and all related
import javax.microedition.khronos.opengles.GL10; //for OpenGLES 1.0 needed as param type GL10
import javax.microedition.khronos.egl.EGLConfig; // for EGLConfig needed as param type EGLConfig
import android.opengl.GLES32; //OpenGL ES 3.2

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer; //for Sphere
import android.opengl.Matrix; //for Matrix math

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
    //fields
    private final Context context;
    private GestureDetector gestureDetector;

    //----------------------------------------------//
    //PER-VERTEX LIGHTING
    //shader objects 
    private int vertexShaderObject_PV;
    private int fragmentShaderObject_PV;
    private int shaderProgramObject_PV;
    //uniform
    //mvp matrix
    private int modelMatrixUniform_PV;
    private int viewMatrixUniform_PV;
    private int projectionMatrixUniform_PV;
    //light
    private int LaUniform_PV;
    private int LdUniform_PV;
    private int LsUniform_PV;
    private int lightPositionUniform_PV;
    //material
    private int KaUniform_PV;
    private int KdUniform_PV;
    private int KsUniform_PV;
    private int materialShininessUniform_PV;
    //light enabled
    private int lightEnabledUniform_PV;

    //----------------------------------------------//
    //PER-FRAGMENT LIGHTING
    //shader objects 
    private int vertexShaderObject_PF;
    private int fragmentShaderObject_PF;
    private int shaderProgramObject_PF;
    //uniform
    //mvp matrix
    private int modelMatrixUniform_PF;
    private int viewMatrixUniform_PF;
    private int projectionMatrixUniform_PF;
    //light
    private int LaUniform_PF;
    private int LdUniform_PF;
    private int LsUniform_PF;
    private int lightPositionUniform_PF;
    //material
    private int KaUniform_PF;
    private int KdUniform_PF;
    private int KsUniform_PF;
    private int materialShininessUniform_PF;
    //light enabled
    private int lightEnabledUniform_PF;

    //----------------------------------------------//
    //sphere
    private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
    int numElements;
    int numVertices;
        
    private float perspectiveProjectionMatrix[] = new float[16]; //4x4 matrix

    //lights
    private int isLightEnabled = 0; //double_tap toggle
    private int isPerVertexEnabled = 0; //vertex
    private int isPerFragmentEnabled = 0; //fragment
    
    //code
    public GLESView(Context drawingContext)
    {
        super(drawingContext);
        context = drawingContext;

        //set EGLCOntext to current supported version of OPENGL ES
        setEGLContextClientVersion(3);

        //set Renderer for drawing on the GLSurfaceView
        setRenderer(this);

        //Render the view only when there is a change in the drawing data
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        //this means 'handler' i.e who is going to handle
        gestureDetector = new GestureDetector(context, this, null, false); 
        gestureDetector.setOnDoubleTapListener(this);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        //OpenGL ES Version check
        String version = gl.glGetString(GL10.GL_VERSION); //instanve funtion - called with object (gl)
        System.out.println("KAB :"+version);

        String GLSLversion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION); //instanve funtion - called with object (gl)
        System.out.println("KAB :"+GLSLversion);

        initialize(gl);

    }

    //overriden method of GLSurfaceView.Renderer (change size code)
    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height)
    {
        resize(width, height);
    }

    //overriden method of GLSUrfaceView.Renderer (Rendering code)
    @Override
    public void onDrawFrame(GL10 unused)
    {
        display();

        update();

        //double buffers
        requestRender();
    }

    //HANDLING 'onTouchEvent' IS IMPORTANT BECAUSE IT TRIGGERS ALL GESTURES AND TAP EVENTS
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        //code
        //CREATING int eventaction variable, THOUGH NOT USED BY US BUT USED IN INDUSRTY, HENCE IMPORTANT
        int eventaction = event.getAction();
        if(!gestureDetector.onTouchEvent(event))
        {
            super.onTouchEvent(event);
        }
        return(true);
    }

    //ABSTRACT METHOD FROM onDoubleTapListener SO MUST BE IMPLEMENTED
    @Override
    public boolean onDoubleTap(MotionEvent e)
    {   
        if(isLightEnabled == 0)
        {
            isLightEnabled = 1;
            isPerVertexEnabled = 1;
            isPerFragmentEnabled = 0;
        }
        else
        {
            isLightEnabled = 0;
        }
        return(true);
    }

    //ABSTRACT METHOD FROM onDoubleTapListener SO MUST BE IMPLEMENTED
    @Override
    public boolean onDoubleTapEvent(MotionEvent e)
    {
        //DO NOT WRITE ANY CODE HERE BECAUSE ALREADY WRITTEN onDoubleTap return(true);
        return(true);
    }

    //ABSTRACT METHOD FROM onDoubleTapListener SO MUST BE IMPLEMENTED
    @Override
    public boolean onSingleTapConfirmed(MotionEvent e)
    {
        if(isPerVertexEnabled == 0)
        {
            isPerVertexEnabled = 1;
            isPerFragmentEnabled = 0;
        }
        else if(isPerVertexEnabled == 1)
        {
            isPerVertexEnabled = 0;
        }
        return(true);
    }

    //ABSTRACT METHOD FROM onGestureListener SO MUST BE IMPLEMENTED
    @Override
    public boolean onDown(MotionEvent e)
    {
        //DO NOT WRITE ANY CODE HERE BECAUSE ALREADY WRITTEN onSingleTapConfirmed return(true);
        return(true);
    }
    
    //ABSTRACT METHOD FROM onGestureListener SO MUST BE IMPLEMENTED
    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
    {
        return(true);
    }

    //ABSTRACT METHOD FROM onGestureListener SO MUST BE IMPLEMENTED
    @Override
    public void onLongPress(MotionEvent e)
    {
        if(isPerFragmentEnabled == 0)
        {
            isPerVertexEnabled = 0;
            isPerFragmentEnabled = 1;
        }
        else if(isPerFragmentEnabled == 1)
        {
            isPerFragmentEnabled = 0;
        }
    }

    //ABSTRACT METHOD FROM onGestureListener SO MUST BE IMPLEMENTED
    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
    {
        uninitialize();

        System.out.println("KAB :"+" Scrooll");
        System.exit(0);
        return(true);

        //NOTE: NOT REQUIRED IN REAL WORLD 
        //BUT AS A DISCPLINE WE WILL ALWAYS UNINITIALIZE ALLOCATED MEMORY BY OURSELVES
    }
    
    //ABSTRACT METHOD FROM onGestureListener SO MUST BE IMPLEMENTED
    @Override
    public void onShowPress(MotionEvent e)
    {
    }

    //ABSTRACT METHOD FROM onGestureListener SO MUST BE IMPLEMENTED
    @Override
    public boolean onSingleTapUp(MotionEvent e)
    {
        return(true);
    }

    private void initialize(GL10 gl)
    {
        getVertexShader_PV();
        getFragmentShader_PV();
        getShaderObject_PV();
        getUniformLocation_PV();

        getVertexShader_PF();
        getFragmentShader_PF();
        getShaderObject_PF();
        getUniformLocation_PF();

        //vertices, colours, shader attribs, vbo, vao initializations
        //--------------------------------------------------------//
        //SPHERE
        //--------------------------------------------------------//
        Sphere sphere = new Sphere();
        float sphere_vertices[] = new float[1146];
        float sphere_normals[] = new float[1146];
        float sphere_textures[] = new float[764];
        short sphere_elements[] = new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

        // vao for SPHERE
        GLES32.glGenVertexArrays(1, vao_sphere, 0);
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        //--------------------------------------------------------//
        // position vbo for SPHERE
        GLES32.glGenBuffers(1, vbo_sphere_position, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_position[0]);

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_vertices.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false, 0, 0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        //--------------------------------------------------------//
        // normal vbo for SPHERE
        GLES32.glGenBuffers(1, vbo_sphere_normal, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_normal[0]);
        
        byteBuffer = ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_normals.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_NORMAL,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false, 0, 0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_NORMAL);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //--------------------------------------------------------//
        // element vbo for SPHERE
        GLES32.glGenBuffers(1, vbo_sphere_element, 0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        
        byteBuffer = ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer = byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                            sphere_elements.length * 2,
                            elementsBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, 0);
        
        //--------------------------------------------------------//

        //Unbind for SPEHERE
        GLES32.glBindVertexArray(0);

        //depth
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        //disable cull (default) for animation
        GLES32.glDisable(GLES32.GL_CULL_FACE);

        //Set background colour
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black

        //set projectionMatrix to identity matrix
        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
    }

    private void getVertexShader_PV()
    {
        // ****************************************************
        // PER-VERTEX VERTEX SHADER
        // ****************************************************
        //create shader
        vertexShaderObject_PV = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

        //use 'OpenGL ES 3.20'
        // static function - called with Class (String)
        final String vertexShaderSourceCode_PV = String.format 
        (
        "#version 320 es"+
        "\n"+
        "in vec4 vPosition;"+
        "in vec3 vNormal;"+
        "uniform mat4 u_model_matrix;"+
        "uniform mat4 u_view_matrix;"+
        "uniform mat4 u_projection_matrix;"+
        "uniform int u_light_enabled;"+
        "uniform vec3 u_LA_PV;"+
        "uniform vec3 u_LD_PV;"+
        "uniform vec3 u_LS_PV;"+
        "uniform vec4 u_light_position_PV;"+
        "uniform vec3 u_KA_PV;"+
        "uniform vec3 u_KD_PV;"+
        "uniform vec3 u_KS_PV;"+
        "uniform float u_material_shininess_PV;"+
        "out vec3 phong_ads_light_PV;"+
        "void main(void)"+
        "{"+
            "if(u_light_enabled == 1)"+
            "{"+
                "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
                "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
                "vec3 light_direction = normalize(vec3(u_light_position_PV - eye_coordinates));"+
                "vec3 reflection_vector = reflect(-light_direction, transformed_normal);"+
                //SWIZZLING
                "vec3 view_vector = normalize(-eye_coordinates.xyz);"+
                "vec3 ambient_PV = u_LA_PV * u_KA_PV;"+
                "vec3 diffuse_PV = u_LD_PV * u_KD_PV * max(dot(light_direction, transformed_normal), 0.0f);"+
                "vec3 specular_PV = u_LS_PV * u_KS_PV * pow(max(dot(reflection_vector, view_vector), 0.0f), u_material_shininess_PV);"+
                "phong_ads_light_PV = ambient_PV + diffuse_PV + specular_PV;"+
            "}"+
            "else"+
            "{"+
                "phong_ads_light_PV = vec3(1.0f, 1.0f, 1.0f);"+
            "}"+
            "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
        "}"
        );

        //provide source code to shader
        GLES32.glShaderSource(vertexShaderObject_PV, vertexShaderSourceCode_PV);

        //compile shader
        GLES32.glCompileShader(vertexShaderObject_PV);

        //error checking
        int iShaderCompiledStatus[] = new int[1];
        int iInfoLogLength[] = new int[1];
        String szInfoLog = null;
        GLES32.glGetShaderiv(vertexShaderObject_PV, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObject_PV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject_PV);
                System.out.println("KAB : PER-VERTEX Vertex Shader Complilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        } 
    }

    private void getFragmentShader_PV()
    {
        // ****************************************************
        // PER-VERTEX FRAGMENT SHADER
        // ****************************************************
        //create shader
        fragmentShaderObject_PV = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

	    final String fragmentShaderSourceCode_PV = String.format
        (
		"#version 320 es"+
		"\n"+
        "precision highp float;"+
        "in vec3 phong_ads_light_PV;"+
        "out vec4 FragColor;"+
        "void main(void)"+
        "{"+
        "FragColor = vec4(phong_ads_light_PV, 1.0f);"+
        "}"
        );

        //provide source code to shader
        GLES32.glShaderSource(fragmentShaderObject_PV, fragmentShaderSourceCode_PV);

        //compile shader
        GLES32.glCompileShader(fragmentShaderObject_PV);

        //error checking
        int iShaderCompiledStatus[] = new int[1];
        int iInfoLogLength[] = new int[1];
        String szInfoLog = null;
        GLES32.glGetShaderiv(fragmentShaderObject_PV, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObject_PV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject_PV);
                System.out.println("KAB : PER-VERTEX Fragment Shader Complilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
    }

    private void getShaderObject_PV()
    {
        // ****************************************************
        // PER-VERTEX SHADER PROGRAM OBJECT
        // ****************************************************
        //create shader program
        shaderProgramObject_PV = GLES32.glCreateProgram(); 

        //attach vertex shader to shader program
        GLES32.glAttachShader(shaderProgramObject_PV, vertexShaderObject_PV);

        //attach fragment shader to shader program
        GLES32.glAttachShader(shaderProgramObject_PV, fragmentShaderObject_PV);

        //pre-link binding of shader program object with vertex shader attributes
        GLES32.glBindAttribLocation(shaderProgramObject_PV, GLESMacros.KAB_ATTRIBUTE_POSITION, "vPosition");

        //pre-link binding of shader program object with vertex shader attributes
        GLES32.glBindAttribLocation(shaderProgramObject_PV, GLESMacros.KAB_ATTRIBUTE_NORMAL, "vNormal");
        
        //link the two shaders together to shader program object
        GLES32.glLinkProgram(shaderProgramObject_PV);

        //error checking
        int iShaderProgramLinkStatus[] = new int[1];
        int iInfoLogLength[] = new int[1];
        String szInfoLog = null;
        GLES32.glGetShaderiv(shaderProgramObject_PV, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
        if(iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(shaderProgramObject_PV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(shaderProgramObject_PV);
                System.out.println("KAB : PER-VERTEX Shader Program Object Link Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
    }

    private void getUniformLocation_PV()
    {
        //get uniform location
        modelMatrixUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_model_matrix");
        viewMatrixUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_view_matrix");
        projectionMatrixUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_projection_matrix");

        lightEnabledUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_light_enabled");
        LaUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_LA_PV");
        LdUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_LD_PV");
        LsUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_LS_PV");
        lightPositionUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_light_position_PV");

        KaUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_KA_PV");
        KdUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_KD_PV");
        KsUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_KS_PV");
        materialShininessUniform_PV = GLES32.glGetUniformLocation(shaderProgramObject_PV, "u_material_shininess_PV");
    }

    private void getVertexShader_PF()
    {
        // ****************************************************
        // PER-FRAGMENT VERTEX SHADER
        // ****************************************************
        //create shader
        vertexShaderObject_PF = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

        //use 'OpenGL ES 3.20'
        // static function - called with Class (String)
        final String vertexShaderSourceCode_PF = String.format 
        (
        "#version 320 es"+
        "\n"+
        "\n"+
        "precision mediump float;"+
        "in vec4 vPosition;"+
        "in vec3 vNormal;"+
        "uniform mat4 u_model_matrix;"+
        "uniform mat4 u_view_matrix;"+
        "uniform mat4 u_projection_matrix;"+
        "uniform mediump int u_light_enabled;"+
        "uniform vec4 u_light_position_PF;"+
        "out vec3 transformed_normal;"+
        "out vec3 light_direction;"+
        "out vec3 view_vector;"+
        "void main(void)"+
        "{"+
        "if(u_light_enabled == 1)"+
        "{"+
        "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
        "transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
        "light_direction = vec3(u_light_position_PF - eye_coordinates);"+
        //SWIZZLING
        "view_vector = -eye_coordinates.xyz;"+
        "}"+
        "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
        "}"
        );

        //provide source code to shader
        GLES32.glShaderSource(vertexShaderObject_PF, vertexShaderSourceCode_PF);

        //compile shader
        GLES32.glCompileShader(vertexShaderObject_PF);

        //error checking
        int iShaderCompiledStatus[] = new int[1];
        int iInfoLogLength[] = new int[1];
        String szInfoLog = null;
        GLES32.glGetShaderiv(vertexShaderObject_PF, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObject_PF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject_PF);
                System.out.println("KAB : PER-FRAGMENT Vertex Shader Complilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
    }

    private void getFragmentShader_PF()
    {
        // ****************************************************
        // PER-FRAGMENT FRAGMENT SHADER
        // ****************************************************
        //create shader
        fragmentShaderObject_PF = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

	    final String fragmentShaderSourceCode_PF = String.format
        (
		"#version 320 es"+
		"\n"+
        "precision highp float;"+
        "in vec3 transformed_normal;"+
        "in vec3 light_direction;"+
        "in vec3 view_vector;"+
        "uniform int u_light_enabled;"+
        "uniform vec3 u_LA_PF;"+
        "uniform vec3 u_LD_PF;"+
        "uniform vec3 u_LS_PF;"+
        "uniform vec3 u_KA_PF;"+
        "uniform vec3 u_KD_PF;"+
        "uniform vec3 u_KS_PF;"+
        "uniform float u_material_shininess_PF;"+
        "out vec4 FragColor;"+
        "void main(void)"+
        "{"+
        "vec3 phong_ads_light_PF;"+
        "if(u_light_enabled == 1)"+
        "{"+
        "vec3 normalized_transformed_normal = normalize(transformed_normal);"+
        "vec3 normalized_light_direction = normalize(light_direction);"+
        "vec3 normalized_view_vector = normalize(view_vector);"+
        "vec3 ambient_PF = u_LA_PF * u_KA_PF;"+
        "vec3 diffuse_PF = u_LD_PF * u_KD_PF * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);"+
        "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);"+
        "vec3 specular_PF = u_LS_PF * u_KS_PF * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_material_shininess_PF);"+
        "phong_ads_light_PF = ambient_PF + diffuse_PF + specular_PF;"+
        "}"+
        "else"+
        "{"+
        "phong_ads_light_PF = vec3(1.0f, 1.0f, 1.0f);"+
        "}"+
        "FragColor = vec4(phong_ads_light_PF, 1.0f);"+ 
        "}"
        );

        //provide source code to shader
        GLES32.glShaderSource(fragmentShaderObject_PF, fragmentShaderSourceCode_PF);

        //compile shader
        GLES32.glCompileShader(fragmentShaderObject_PF);

        //error checking
        int iShaderCompiledStatus[] = new int[1];
        int iInfoLogLength[] = new int[1];
        String szInfoLog = null;
        GLES32.glGetShaderiv(fragmentShaderObject_PF, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObject_PF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject_PF);
                System.out.println("KAB : PER-FRAGMENT Fragment Shader Complilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
    }

    private void getShaderObject_PF()
    {
        // ****************************************************
        // PER-FRAGMENT SHADER PROGRAM OBJECT
        // ****************************************************
        //create shader program
        shaderProgramObject_PF = GLES32.glCreateProgram(); 

        //attach vertex shader to shader program
        GLES32.glAttachShader(shaderProgramObject_PF, vertexShaderObject_PF);

        //attach fragment shader to shader program
        GLES32.glAttachShader(shaderProgramObject_PF, fragmentShaderObject_PF);

        //pre-link binding of shader program object with vertex shader attributes
        GLES32.glBindAttribLocation(shaderProgramObject_PF, GLESMacros.KAB_ATTRIBUTE_POSITION, "vPosition");

        //pre-link binding of shader program object with vertex shader attributes
        GLES32.glBindAttribLocation(shaderProgramObject_PF, GLESMacros.KAB_ATTRIBUTE_NORMAL, "vNormal");
        
        //link the two shaders together to shader program object
        GLES32.glLinkProgram(shaderProgramObject_PF);

        //error checking
        int iShaderProgramLinkStatus[] = new int[1];
        int iInfoLogLength[] = new int[1];
        String szInfoLog = null;
        GLES32.glGetShaderiv(shaderProgramObject_PF, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
        if(iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(shaderProgramObject_PF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(shaderProgramObject_PF);
                System.out.println("KAB : PER-FRAGMENT Shader Program Object Link Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
    }

    private void getUniformLocation_PF()
    {
        //get uniform location
        modelMatrixUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_model_matrix");
        viewMatrixUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_view_matrix");
        projectionMatrixUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_projection_matrix");

        lightEnabledUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_light_enabled");
        LaUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_LA_PF");
        LdUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_LD_PF");
        LsUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_LS_PF");
        lightPositionUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_light_position_PF");

        KaUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_KA_PF");
        KdUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_KD_PF");
        KsUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_KS_PF");
        materialShininessUniform_PF = GLES32.glGetUniformLocation(shaderProgramObject_PF, "u_material_shininess_PF");
    }

    private void resize(int width, int height)
    {
        //Adjust viewport on gemoetry changes such as screen rotation
        GLES32.glViewport(0, 0, width, height);

        Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 
                                            45.0f, 
											(float)width / (float)height,
                                            0.1f,
                                            100.0f);
    }

    private void display()
    {
        //variables
        //light and maetrial values
        float lightAmbient[] = new float[] { 0.1f, 0.1f, 0.1f, 1.0f };
        float lightDiffuse[] = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
        float lightSpecular[] = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
        float lightPosition[] = new float[] { 100.0f, 100.0f, 100.0f, 1.0f };

        float materialAmbient[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };
        float materialDiffuse[] = new float[] { 0.5f, 0.2f, 0.7f, 1.0f };
        float materialSpecular[] = new float[] { 0.7f, 0.7f, 0.7f, 1.0f };
        float materialShininess = 128.0f;
        
        //Draw background colour
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        //for OPENGL- ES Drawing
        float translationMatrix[] = new float[16];
        float modelMatrix[] = new float[16];
        float viewMatrix[] = new float[16];
        float projectionMatrix[] = new float[16];

        //--------------------------------------------------------//
        //SPHERE
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -3.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //use shader program
        //per-vertex default
        GLES32.glUseProgram(shaderProgramObject_PV);
        if(isPerVertexEnabled == 1)
        {
            GLES32.glUniform4fv(lightPositionUniform_PV, 1, lightPosition, 0);
            if(isLightEnabled == 1)
            {
                GLES32.glUniform1i(lightEnabledUniform_PV, 1);
                GLES32.glUniform3fv(LaUniform_PV, 1, lightAmbient, 0);
                GLES32.glUniform3fv(LdUniform_PV, 1, lightDiffuse, 0);
                GLES32.glUniform3fv(LsUniform_PV, 1, lightSpecular, 0);
                GLES32.glUniform3fv(KaUniform_PV, 1, materialAmbient, 0);
                GLES32.glUniform3fv(KdUniform_PV, 1, materialDiffuse, 0);
                GLES32.glUniform3fv(KsUniform_PV, 1, materialSpecular, 0);
                GLES32.glUniform1f(materialShininessUniform_PV, materialShininess);
            }
            else
            {
                GLES32.glUniform1i(lightEnabledUniform_PV, 0);
            }
            
            //sending MODEL, VIEW, PROJECTION Uniforms separately.
            GLES32.glUniformMatrix4fv(modelMatrixUniform_PV, 1, false, modelMatrix, 0);
            GLES32.glUniformMatrix4fv(viewMatrixUniform_PV, 1, false, viewMatrix, 0);
            GLES32.glUniformMatrix4fv(projectionMatrixUniform_PV, 1, false, projectionMatrix, 0);
        }
        //per-fragment
        else if(isPerFragmentEnabled == 1)
        {
            GLES32.glUseProgram(shaderProgramObject_PF);
            GLES32.glUniform4fv(lightPositionUniform_PF, 1, lightPosition, 0);
            if(isLightEnabled == 1)
            {
                GLES32.glUniform1i(lightEnabledUniform_PF, 1);
                GLES32.glUniform3fv(LaUniform_PF, 1, lightAmbient, 0);
                GLES32.glUniform3fv(LdUniform_PF, 1, lightDiffuse, 0);
                GLES32.glUniform3fv(LsUniform_PF, 1, lightSpecular, 0);
                GLES32.glUniform3fv(KaUniform_PF, 1, materialAmbient, 0);
                GLES32.glUniform3fv(KdUniform_PF, 1, materialDiffuse, 0);
                GLES32.glUniform3fv(KsUniform_PF, 1, materialSpecular, 0);
                GLES32.glUniform1f(materialShininessUniform_PF, materialShininess);
            }
            else
            {
                GLES32.glUniform1i(lightEnabledUniform_PF, 0);
            }
            
            //sending MODEL, VIEW, PROJECTION Uniforms separately.
            GLES32.glUniformMatrix4fv(modelMatrixUniform_PF, 1, false, modelMatrix, 0);
            GLES32.glUniformMatrix4fv(viewMatrixUniform_PF, 1, false, viewMatrix, 0);
            GLES32.glUniformMatrix4fv(projectionMatrixUniform_PF, 1, false, projectionMatrix, 0);
        }

        else
        {
            //sending MODEL, VIEW, PROJECTION Uniforms separately.
            GLES32.glUniformMatrix4fv(modelMatrixUniform_PV, 1, false, modelMatrix, 0);
            GLES32.glUniformMatrix4fv(viewMatrixUniform_PV, 1, false, viewMatrix, 0);
            GLES32.glUniformMatrix4fv(projectionMatrixUniform_PV, 1, false, projectionMatrix, 0);
        }
       

        //bind vao
        GLES32.glBindVertexArray(vao_sphere[0]);

        //openGL ES drawing
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //unuse shader program
        GLES32.glUseProgram(0);

        //double buffers is called in Game Loop -> onDrawFrame()
    }

    private void update()
    {
        //code
    }

    private void uninitialize()
    {
        // code
        // destroy vao of SPHERE
        if(vao_sphere[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0]=0;
        }
        
        // destroy position vbo of SPHERE
        if(vbo_sphere_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0]=0;
        }
        
        // destroy normal vbo of SPHERE
        if(vbo_sphere_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0]=0;
        }
        
        // destroy element vbo of SPHERE
        if(vbo_sphere_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0]=0;
        }

        //per-vertex
        if(shaderProgramObject_PV != 0)
        {
            if(vertexShaderObject_PV != 0)
            {
                //detach
                GLES32.glDetachShader(shaderProgramObject_PV, vertexShaderObject_PV);

                //delete
                GLES32.glDeleteShader(vertexShaderObject_PV);
                vertexShaderObject_PV = 0;
            }

            if(fragmentShaderObject_PV != 0)
            {
                //detach
                GLES32.glDetachShader(shaderProgramObject_PV, fragmentShaderObject_PV);

                //delete
                GLES32.glDeleteShader(fragmentShaderObject_PV);
                fragmentShaderObject_PV = 0;
            }

        }

        if(shaderProgramObject_PV != 0)
        {
            GLES32.glDeleteProgram(shaderProgramObject_PV);
            shaderProgramObject_PV = 0;
        }

        //per-fragment
        if(shaderProgramObject_PF != 0)
        {
            if(vertexShaderObject_PF != 0)
            {
                //detach
                GLES32.glDetachShader(shaderProgramObject_PF, vertexShaderObject_PF);

                //delete
                GLES32.glDeleteShader(vertexShaderObject_PF);
                vertexShaderObject_PF = 0;
            }

            if(fragmentShaderObject_PF != 0)
            {
                //detach
                GLES32.glDetachShader(shaderProgramObject_PF, fragmentShaderObject_PF);

                //delete
                GLES32.glDeleteShader(fragmentShaderObject_PF);
                fragmentShaderObject_PF = 0;
            }

        }

        if(shaderProgramObject_PF != 0)
        {
            GLES32.glDeleteProgram(shaderProgramObject_PF);
            shaderProgramObject_PF = 0;
        }
    }
}

