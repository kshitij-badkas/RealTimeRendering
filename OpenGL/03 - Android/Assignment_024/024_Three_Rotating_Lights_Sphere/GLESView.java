package com.example.three_rotating_lights_sphere;

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

    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
    
    private float perspectiveProjectionMatrix[] = new float[16]; //4x4 matrix

    //uniform
    //mvp matrix
    private int modelMatrixUniform;
    private int viewMatrixUniform;
    private int projectionMatrixUniform;
    //light
    private int LaUniform_Red;
    private int LdUniform_Red;
    private int LsUniform_Red;
    private int lightPositionUniform_Red;
    private int LaUniform_Green;
    private int LdUniform_Green;
    private int LsUniform_Green;
    private int lightPositionUniform_Green;
    private int LaUniform_Blue;
    private int LdUniform_Blue;
    private int LsUniform_Blue;
    private int lightPositionUniform_Blue;
    //material
    private int KaUniform;
    private int KdUniform;
    private int KsUniform;
    private int materialShininessUniform;
    //light enabled
    private int lightEnabledUniform;

    //sphere
    private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
    int numElements;
    int numVertices;

    //lights
    private int isLightEnabled = 0;
    private float radius = 6.0f;

    //animation
    private static float lightAngle_Red = 0.0f;
    private static float lightAngle_Green = 0.0f;
    private static float lightAngle_Blue = 0.0f;
    
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
        // ****************************************************
        // VERTEX SHADER
        // ****************************************************
        //create shader
        vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

        //use 'OpenGL ES 3.20'
        // static function - called with Class (String)
        final String vertexShaderSourceCode = String.format 
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
        "uniform vec4 u_light_position_Red;"+
        "uniform vec4 u_light_position_Green;"+
        "uniform vec4 u_light_position_Blue;"+
        "out vec3 transformed_normal;"+
        "out vec3 light_direction_Red;"+
        "out vec3 light_direction_Green;"+
        "out vec3 light_direction_Blue;"+
        "out vec3 view_vector;"+
        "void main(void)"+
        "{"+
        "if(u_light_enabled == 1)"+
        "{"+
        "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
        "transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
        "light_direction_Red = vec3(u_light_position_Red - eye_coordinates);"+
        "light_direction_Green = vec3(u_light_position_Green - eye_coordinates);"+
        "light_direction_Blue = vec3(u_light_position_Blue - eye_coordinates);"+
        "view_vector = -eye_coordinates.xyz;"+
        "}"+
        "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
        "}"
        );

        //provide source code to shader
        GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);

        //compile shader
        GLES32.glCompileShader(vertexShaderObject);

        //error checking
        int iShaderCompiledStatus[] = new int[1];
        int iInfoLogLength[] = new int[1];
        String szInfoLog = null;
        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("KAB : Vertex Shader Complilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        } 

        // ****************************************************
        // FRAGMENT SHADER
        // ****************************************************
        //create shader
        fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

	    final String fragmentShaderSourceCode = String.format
        (
		"#version 320 es"+
		"\n"+
        "precision highp float;"+
        "in vec3 transformed_normal;"+
        "in vec3 light_direction_Red;"+
        "in vec3 light_direction_Green;"+
        "in vec3 light_direction_Blue;"+        
        "in vec3 view_vector;"+
        "uniform int u_light_enabled;"+
        "uniform vec3 u_LA_Red;"+
        "uniform vec3 u_LD_Red;"+
        "uniform vec3 u_LS_Red;"+
        "uniform vec3 u_LA_Green;"+
        "uniform vec3 u_LD_Green;"+
        "uniform vec3 u_LS_Green;"+
        "uniform vec3 u_LA_Blue;"+
        "uniform vec3 u_LD_Blue;"+
        "uniform vec3 u_LS_Blue;"+
        "uniform vec3 u_KA;"+
        "uniform vec3 u_KD;"+
        "uniform vec3 u_KS;"+
        "uniform float u_material_shininess;"+
        "out vec4 FragColor;"+
        "void main(void)"+
        "{"+
        "vec3 phong_ads_light;"+
        "if(u_light_enabled == 1)"+
        "{"+
        "vec3 normalized_transformed_normal = normalize(transformed_normal);"+
        "vec3 normalized_light_direction_Red = normalize(light_direction_Red);"+
        "vec3 normalized_light_direction_Green = normalize(light_direction_Green);"+
        "vec3 normalized_light_direction_Blue = normalize(light_direction_Blue);"+
        "vec3 normalized_view_vector = normalize(view_vector);"+
        
        "vec3 ambient_Red = u_LA_Red * u_KA;"+
        "vec3 diffuse_Red = u_LD_Red * u_KD * max(dot(normalized_light_direction_Red, normalized_transformed_normal), 0.0f);"+
        "vec3 reflection_vector_Red = reflect(-normalized_light_direction_Red, normalized_transformed_normal);"+
        "vec3 specular_Red = u_LS_Red * u_KS * pow(max(dot(reflection_vector_Red, normalized_view_vector), 0.0f), u_material_shininess);"+

        "vec3 ambient_Green = u_LA_Green * u_KA;"+
        "vec3 diffuse_Green = u_LD_Green * u_KD * max(dot(normalized_light_direction_Green, normalized_transformed_normal), 0.0f);"+
        "vec3 reflection_vector_Green = reflect(-normalized_light_direction_Green, normalized_transformed_normal);"+
        "vec3 specular_Green = u_LS_Green * u_KS * pow(max(dot(reflection_vector_Green, normalized_view_vector), 0.0f), u_material_shininess);"+

        "vec3 ambient_Blue = u_LA_Blue * u_KA;"+
        "vec3 diffuse_Blue = u_LD_Blue * u_KD * max(dot(normalized_light_direction_Blue, normalized_transformed_normal), 0.0f);"+
        "vec3 reflection_vector_Blue = reflect(-normalized_light_direction_Blue, normalized_transformed_normal);"+
        "vec3 specular_Blue = u_LS_Blue * u_KS * pow(max(dot(reflection_vector_Blue, normalized_view_vector), 0.0f), u_material_shininess);"+

        "phong_ads_light = phong_ads_light + ambient_Red + diffuse_Red + specular_Red;"+
        "phong_ads_light = phong_ads_light + ambient_Green + diffuse_Green + specular_Green;"+
        "phong_ads_light = phong_ads_light + ambient_Blue + diffuse_Blue + specular_Blue;"+
        "}"+
        "else"+
        "{"+
        "phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"+
        "}"+
        "FragColor = vec4(phong_ads_light, 1.0f);"+
        "}"
        );

        //provide source code to shader
        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);

        //compile shader
        GLES32.glCompileShader(fragmentShaderObject);

        //error checking
        iShaderCompiledStatus[0] = 0;
        iInfoLogLength[0] = 0;
        szInfoLog = null;
        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("KAB : Fragment Shader Complilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
        // ****************************************************
        // SHADER PROGRAM OBJECT
        // ****************************************************
        //create shader program
        shaderProgramObject = GLES32.glCreateProgram(); 

        //attach vertex shader to shader program
        GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);

        //attach fragment shader to shader program
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

        //pre-link binding of shader program object with vertex shader attributes
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.KAB_ATTRIBUTE_POSITION, "vPosition");

        //pre-link binding of shader program object with vertex shader attributes
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.KAB_ATTRIBUTE_NORMAL, "vNormal");
        
        //link the two shaders together to shader program object
        GLES32.glLinkProgram(shaderProgramObject);

        //error checking
        int iShaderProgramLinkStatus[] = new int[1];
        iInfoLogLength[0] = 0;
        szInfoLog = null;
        GLES32.glGetShaderiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderCompiledStatus, 0);
        if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(shaderProgramObject);
                System.out.println("KAB : Shader Program Object Link Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        //get MVP uniform location
        modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_model_matrix");
        viewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_view_matrix");
        projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

        lightEnabledUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_enabled");
        
        LaUniform_Red = GLES32.glGetUniformLocation(shaderProgramObject, "u_LA_Red");
        LdUniform_Red = GLES32.glGetUniformLocation(shaderProgramObject, "u_LD_Red");
        LsUniform_Red = GLES32.glGetUniformLocation(shaderProgramObject, "u_LS_Red");
        lightPositionUniform_Red = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position_Red");
        LaUniform_Green = GLES32.glGetUniformLocation(shaderProgramObject, "u_LA_Green");
        LdUniform_Green = GLES32.glGetUniformLocation(shaderProgramObject, "u_LD_Green");
        LsUniform_Green = GLES32.glGetUniformLocation(shaderProgramObject, "u_LS_Green");
        lightPositionUniform_Green = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position_Green");
        LaUniform_Blue = GLES32.glGetUniformLocation(shaderProgramObject, "u_LA_Blue");
        LdUniform_Blue = GLES32.glGetUniformLocation(shaderProgramObject, "u_LD_Blue");
        LsUniform_Blue = GLES32.glGetUniformLocation(shaderProgramObject, "u_LS_Blue");
        lightPositionUniform_Blue = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position_Blue");

        KaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

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
        float lightAmbient_Red[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };
        float lightDiffuse_Red[] = new float[] { 1.0f, 0.0f, 0.0f, 1.0f }; //RED
        float lightSpecular_Red[] = new float[] { 1.0f, 0.0f, 0.0f, 1.0f };
        float lightPosition_Red[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };

        float lightAmbient_Green[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };
        float lightDiffuse_Green[] = new float[] { 0.0f, 1.0f, 0.0f, 1.0f }; //GREEN
        float lightSpecular_Green[] = new float[] { 0.0f, 1.0f, 0.0f, 1.0f };
        float lightPosition_Green[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };

        float lightAmbient_Blue[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };
        float lightDiffuse_Blue[] = new float[] { 0.0f, 0.0f, 1.0f, 1.0f }; //BLUE
        float lightSpecular_Blue[] = new float[] { 0.0f, 0.0f, 1.0f, 1.0f };
        float lightPosition_Blue[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };

        float materialAmbient[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };
        float materialDiffuse[] = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
        float materialSpecular[] = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
        float materialShininess = 128.0f;
        
        //Draw background colour
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        //use shader program
        GLES32.glUseProgram(shaderProgramObject);

        //for OPENGL- ES Drawing
        float translationMatrix[] = new float[16];
        float modelMatrix[] = new float[16];
        float viewMatrix[] = new float[16];
        float projectionMatrix[] = new float[16];

        if(isLightEnabled == 1)
        {
            lightPosition_Red[0] = 0.0f; //x
            lightPosition_Red[1] = radius * (float)Math.sin(lightAngle_Red); //y
            lightPosition_Red[2] = radius * (float)Math.cos(lightAngle_Red); //z
            lightPosition_Red[3] = 1.0f; //w

            lightPosition_Green[0] = radius * (float)Math.sin(lightAngle_Green); //x
            lightPosition_Green[1] = 0.0f; //y
            lightPosition_Green[2] = radius * (float)Math.cos(lightAngle_Green); //z
            lightPosition_Green[3] = 1.0f; //w

            lightPosition_Blue[0] = radius * (float)Math.sin(lightAngle_Blue); //x
            lightPosition_Blue[1] = radius * (float)Math.cos(lightAngle_Blue); //y
            lightPosition_Blue[2] = 0.0f; //z
            lightPosition_Blue[3] = 1.0f; //w
            
            GLES32.glUniform1i(lightEnabledUniform, 1);
            GLES32.glUniform3fv(LaUniform_Red, 1, lightAmbient_Red, 0);
            GLES32.glUniform3fv(LdUniform_Red, 1, lightDiffuse_Red, 0);
            GLES32.glUniform3fv(LsUniform_Red, 1, lightSpecular_Red, 0);
            GLES32.glUniform4fv(lightPositionUniform_Red, 1, lightPosition_Red, 0);
            GLES32.glUniform3fv(LaUniform_Green, 1, lightAmbient_Green, 0);
            GLES32.glUniform3fv(LdUniform_Green, 1, lightDiffuse_Green, 0);
            GLES32.glUniform3fv(LsUniform_Green, 1, lightSpecular_Green, 0);
            GLES32.glUniform4fv(lightPositionUniform_Green, 1, lightPosition_Green, 0);
            GLES32.glUniform3fv(LaUniform_Blue, 1, lightAmbient_Blue, 0);
            GLES32.glUniform3fv(LdUniform_Blue, 1, lightDiffuse_Blue, 0);
            GLES32.glUniform3fv(LsUniform_Blue, 1, lightSpecular_Blue, 0);
            GLES32.glUniform4fv(lightPositionUniform_Blue, 1, lightPosition_Blue, 0);
            GLES32.glUniform3fv(KaUniform, 1, materialAmbient, 0);
            GLES32.glUniform3fv(KdUniform, 1, materialDiffuse, 0);
            GLES32.glUniform3fv(KsUniform, 1, materialSpecular, 0);
            GLES32.glUniform1f(materialShininessUniform, materialShininess);
        }
        else
        {
            GLES32.glUniform1i(lightEnabledUniform, 0);
        }

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
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

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
        lightAngle_Red += 0.025f;
        lightAngle_Green += 0.025f;
        lightAngle_Blue += 0.025f;
        if (lightAngle_Red >= 360.0f)
        {
            lightAngle_Red = 0.0f;
        }
        if (lightAngle_Blue >= 360.0f)
        {
            lightAngle_Blue = 0.0f;
        }
        if (lightAngle_Green >= 360.0f)
        {
            lightAngle_Green = 0.0f;
        }
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

        if(shaderProgramObject != 0)
        {
            if(vertexShaderObject != 0)
            {
                //detach
                GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);

                //delete
                GLES32.glDeleteShader(vertexShaderObject);
                vertexShaderObject = 0;
            }

            if(fragmentShaderObject != 0)
            {
                //detach
                GLES32.glDetachShader(shaderProgramObject, fragmentShaderObject);

                //delete
                GLES32.glDeleteShader(fragmentShaderObject);
                fragmentShaderObject = 0;
            }

        }

        if(shaderProgramObject != 0)
        {
            GLES32.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }
    }
}
