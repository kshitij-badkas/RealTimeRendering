package com.example.twenty_four_spheres;

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
import android.opengl.Matrix; //for Matrix

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
    //fields
    private final Context context;
    private GestureDetector gestureDetector;

    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
    
    private float perspectiveProjectionMatrix[] = new float[16]; //4x4 matrix

    //light values
    float lightAmbient[] = new float[] { 0.1f, 0.1f, 0.1f, 1.0f };
    float lightDiffuse[] = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
    float lightSpecular[] = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
    float lightPosition[] = new float[] { 100.0f, 100.0f, 100.0f, 1.0f };

    //uniform
    //mvp matrix
    private int modelMatrixUniform;
    private int viewMatrixUniform;
    private int projectionMatrixUniform;
    //light
    private int LaUniform;
    private int LdUniform;
    private int LsUniform;
    private int lightPositionUniform;

    //light enabled
    private int lightEnabledUniform;
    
    //material
    private int KaUniform1; //ambient material component
    private int KdUniform1; //diffuse material component
    private int KsUniform1; //specular material component
    private int materialShininessUniform1; //shininess material

    private int KaUniform2; //ambient material component
    private int KdUniform2; //diffuse material component
    private int KsUniform2; //specular material component
    private int materialShininessUniform2; //shininess material

    private int KaUniform3; //ambient material component
    private int KdUniform3; //diffuse material component
    private int KsUniform3; //specular material component
    private int materialShininessUniform3; //shininess material

    private int KaUniform4; //ambient material component
    private int KdUniform4; //diffuse material component
    private int KsUniform4; //specular material component
    private int materialShininessUniform4; //shininess material

    private int KaUniform5; //ambient material component
    private int KdUniform5; //diffuse material component
    private int KsUniform5; //specular material component
    private int materialShininessUniform5; //shininess material

    private int KaUniform6; //ambient material component
    private int KdUniform6; //diffuse material component
    private int KsUniform6; //specular material component
    private int materialShininessUniform6; //shininess material

    private int KaUniform7; //ambient material component
    private int KdUniform7; //diffuse material component
    private int KsUniform7; //specular material component
    private int materialShininessUniform7; //shininess material

    private int KaUniform8; //ambient material component
    private int KdUniform8; //diffuse material component
    private int KsUniform8; //specular material component
    private int materialShininessUniform8; //shininess material

    private int KaUniform9; //ambient material component
    private int KdUniform9; //diffuse material component
    private int KsUniform9; //specular material component
    private int materialShininessUniform9; //shininess material

    private int KaUniform10; //ambient material component
    private int KdUniform10; //diffuse material component
    private int KsUniform10; //specular material component
    private int materialShininessUniform10; //shininess material

    private int KaUniform11; //ambient material component
    private int KdUniform11; //diffuse material component
    private int KsUniform11; //specular material component
    private int materialShininessUniform11; //shininess material

    private int KaUniform12; //ambient material component
    private int KdUniform12; //diffuse material component
    private int KsUniform12; //specular material component
    private int materialShininessUniform12; //shininess material

    private int KaUniform13; //ambient material component
    private int KdUniform13; //diffuse material component
    private int KsUniform13; //specular material component
    private int materialShininessUniform13; //shininess material

    private int KaUniform14; //ambient material component
    private int KdUniform14; //diffuse material component
    private int KsUniform14; //specular material component
    private int materialShininessUniform14; //shininess material

    private int KaUniform15; //ambient material component
    private int KdUniform15; //diffuse material component
    private int KsUniform15; //specular material component
    private int materialShininessUniform15; //shininess material

    private int KaUniform16; //ambient material component
    private int KdUniform16; //diffuse material component
    private int KsUniform16; //specular material component
    private int materialShininessUniform16; //shininess material

    private int KaUniform17; //ambient material component
    private int KdUniform17; //diffuse material component
    private int KsUniform17; //specular material component
    private int materialShininessUniform17; //shininess material

    private int KaUniform18; //ambient material component
    private int KdUniform18; //diffuse material component
    private int KsUniform18; //specular material component
    private int materialShininessUniform18; //shininess material

    private int KaUniform19; //ambient material component
    private int KdUniform19; //diffuse material component
    private int KsUniform19; //specular material component
    private int materialShininessUniform19; //shininess material

    private int KaUniform20; //ambient material component
    private int KdUniform20; //diffuse material component
    private int KsUniform20; //specular material component
    private int materialShininessUniform20; //shininess material

    private int KaUniform21; //ambient material component
    private int KdUniform21; //diffuse material component
    private int KsUniform21; //specular material component
    private int materialShininessUniform21; //shininess material

    private int KaUniform22; //ambient material component
    private int KdUniform22; //diffuse material component
    private int KsUniform22; //specular material component
    private int materialShininessUniform22; //shininess material

    private int KaUniform23; //ambient material component
    private int KdUniform23; //diffuse material component
    private int KsUniform23; //specular material component
    private int materialShininessUniform23; //shininess material

    private int KaUniform24; //ambient material component
    private int KdUniform24; //diffuse material component
    private int KsUniform24; //specular material component
    private int materialShininessUniform24; //shininess materia

    // material will have 24 different values
    float materialAmbient1[] = new float[] { 0.0215f, 0.1745f, 0.0215f, 1.0f };
    float materialDiffuse1[] = new float[] { 0.07568f, 0.61424f, 0.07568f, 1.0f };
    float materialSpecular1[] = new float[] { 0.633f, 0.727811f, 0.633f, 1.0f };
    float materialShininess1 = 0.6f * 128.0f;

    float materialAmbient2[] = new float[] { 0.135f, 0.225f, 0.1575f, 1.0f };
    float materialDiffuse2[] = new float[] { 0.54f, 0.89f, 0.63f, 1.0f };
    float materialSpecular2[] = new float[] { 0.316228f, 0.316228f, 0.316228f, 1.0f };
    float materialShininess2 = 0.1f * 128.0f;

    float materialAmbient3[] = new float[] { 0.05375f, 0.05f, 0.06625f, 1.0f };
    float materialDiffuse3[] = new float[] { 0.18275f, 0.17f, 0.22525f, 1.0f };
    float materialSpecular3[] = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
    float materialShininess3 = 0.3f * 128.0f;

    float materialAmbient4[] = new float[] { 0.25f, 0.20725f, 0.20725f, 1.0f };
    float materialDiffuse4[] = new float[] { 1.0f, 0.829f, 0.829f, 1.0f };
    float materialSpecular4[] = new float[] { 0.296648f, 0.296648f, 0.296648f, 1.0f };
    float materialShininess4 = 0.088f * 128.0f;

    float materialAmbient5[] = new float[] { 0.1745f, 0.01175f, 0.01175f, 1.0f };
    float materialDiffuse5[] = new float[] { 0.61424f, 0.04136f, 0.04136f, 1.0f };
    float materialSpecular5[] = new float[] { 0.727811f, 0.626959f, 0.626959f, 1.0f };
    float materialShininess5 = 0.6f * 128.0f;

    float materialAmbient6[] = new float[] { 0.1f, 0.18725f, 0.1745f, 1.0f };
    float materialDiffuse6[] = new float[] { 0.396f, 0.74151f, 0.69102f, 1.0f };
    float materialSpecular6[] = new float[] { 0.297254f, 0.30829f, 0.306678f, 1.0f };
    float materialShininess6 = 0.1f * 128.0f;

    float materialAmbient7[] = new float[] { 0.329412f, 0.223529f, 0.027451f, 1.0f };
    float materialDiffuse7[] = new float[] { 0.780392f, 0.568627f, 0.113725f, 1.0f };
    float materialSpecular7[] = new float[] { 0.992157f, 0.941176f, 0.807843f, 1.0f };
    float materialShininess7 = 0.21794872f * 128.0f;

    float materialAmbient8[] = new float[] { 0.2125f, 0.1275f, 0.054f, 1.0f };
    float materialDiffuse8[] = new float[] { 0.714f, 0.4284f, 0.18144f, 1.0f };
    float materialSpecular8[] = new float[] { 0.393548f, 0.271906f, 0.166721f, 1.0f };
    float materialShininess8 = 0.2f * 128.0f;

    float materialAmbient9[] = new float[] { 0.25f, 0.25f, 0.25f, 1.0f };
    float materialDiffuse9[] = new float[] { 0.4f, 0.4f, 0.4f, 1.0f };
    float materialSpecular9[] = new float[] { 0.774597f, 0.774597f, 0.774597f, 1.0f };
    float materialShininess9 = 0.6f * 128.0f;

    float materialAmbient10[] = new float[] { 0.19125f, 0.0735f, 0.0225f, 1.0f };
    float materialDiffuse10[] = new float[] { 0.7038f, 0.27048f, 0.0828f, 1.0f };
    float materialSpecular10[] = new float[] { 0.256777f, 0.137622f, 0.086014f, 1.0f };
    float materialShininess10 = 0.1f * 128.0f;

    float materialAmbient11[] = new float[] { 0.24725f, 0.1995f, 0.0745f, 1.0f };
    float materialDiffuse11[] = new float[] { 0.75164f, 0.60648f, 0.22648f, 1.0f };
    float materialSpecular11[] = new float[] { 0.628281f, 0.555802f, 0.366065f, 1.0f };
    float materialShininess11 = 0.4f * 128.0f;

    float materialAmbient12[] = new float[] { 0.19225f, 0.19225f, 0.19225f, 1.0f };
    float materialDiffuse12[] = new float[] { 0.50754f, 0.50754f, 0.50754f, 1.0f };
    float materialSpecular12[] = new float[] { 0.508273f, 0.508273f, 0.508273f, 1.0f };
    float materialShininess12 = 0.4f * 128.0f;

    float materialAmbient13[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };
    float materialDiffuse13[] = new float[] { 0.01f, 0.01f, 0.01f, 1.0f };
    float materialSpecular13[] = new float[] { 0.50f, 0.50f, 0.50f, 1.0f };
    float materialShininess13 = 0.25f * 128.0f;

    float materialAmbient14[] = new float[] { 0.0f, 0.1f, 0.06f, 1.0f };
    float materialDiffuse14[] = new float[] { 0.0f, 0.50980392f, 0.50980392f, 1.0f };
    float materialSpecular14[] = new float[] { 0.50196078f, 0.50196078f, 0.50196078f, 1.0f };
    float materialShininess14 = 0.25f * 128.0f;

    float materialAmbient15[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };
    float materialDiffuse15[] = new float[] { 0.1f, 0.35f, 0.1f, 1.0f };
    float materialSpecular15[] = new float[] { 0.45f, 0.55f, 0.45f, 1.0f };
    float materialShininess15 = 0.25f * 128.0f;

    float materialAmbient16[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };
    float materialDiffuse16[] = new float[] { 0.5f, 0.0f, 0.0f, 1.0f };
    float materialSpecular16[] = new float[] { 0.7f, 0.6f, 0.6f, 1.0f };
    float materialShininess16 = 0.25f * 128.0f;

    float materialAmbient17[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };
    float materialDiffuse17[] = new float[] { 0.55f, 0.55f, 0.55f, 1.0f };
    float materialSpecular17[] = new float[] { 0.70f, 0.70f, 0.70f, 1.0f };
    float materialShininess17 = 0.25f * 128.0f;

    float materialAmbient18[] = new float[] { 0.0f, 0.0f, 0.0f, 1.0f };
    float materialDiffuse18[] = new float[] { 0.5f, 0.5f, 0.0f, 1.0f };
    float materialSpecular18[] = new float[] { 0.60f, 0.60f, 0.50f, 1.0f };
    float materialShininess18 = 0.25f * 128.0f;

    float materialAmbient19[] = new float[] { 0.02f, 0.02f, 0.02f, 1.0f };
    float materialDiffuse19[] = new float[] { 0.01f, 0.01f, 0.01f, 1.0f };
    float materialSpecular19[] = new float[] { 0.4f, 0.4f, 0.4f, 1.0f };
    float materialShininess19 = 0.078125f * 128.0f;

    float materialAmbient20[] = new float[] { 0.0f, 0.05f, 0.05f, 1.0f };
    float materialDiffuse20[] = new float[] { 0.4f, 0.5f, 0.5f, 1.0f };
    float materialSpecular20[] = new float[] { 0.04f, 0.7f, 0.7f, 1.0f };
    float materialShininess20 = 0.078125f * 128.0f;

    float materialAmbient21[] = new float[] { 0.0f, 0.05f, 0.0f, 1.0f };
    float materialDiffuse21[] = new float[] { 0.4f, 0.5f, 0.4f, 1.0f };
    float materialSpecular21[] = new float[] { 0.04f, 0.7f, 0.04f, 1.0f };
    float materialShininess21 = 0.078125f * 128.0f;

    float materialAmbient22[] = new float[] { 0.05f, 0.0f, 0.0f, 1.0f };
    float materialDiffuse22[] = new float[] { 0.5f, 0.4f, 0.4f, 1.0f };
    float materialSpecular22[] = new float[] { 0.7f, 0.04f, 0.04f, 1.0f };
    float materialShininess22 = 0.078125f * 128.0f;

    float materialAmbient23[] = new float[] { 0.05f, 0.05f, 0.05f, 1.0f };
    float materialDiffuse23[] = new float[] { 0.5f, 0.5f, 0.5f, 1.0f };
    float materialSpecular23[] = new float[] { 0.7f, 0.7f, 0.7f, 1.0f };
    float materialShininess23 = 0.078125f * 128.0f;

    float materialAmbient24[] = new float[] { 0.5f, 0.05f, 0.0f, 1.0f };
    float materialDiffuse24[] = new float[] { 0.5f, 0.5f, 0.4f, 1.0f };
    float materialSpecular24[] = new float[] { 0.7f, 0.7f, 0.04f, 1.0f };
    float materialShininess24 = 0.078125f * 128.0f;

    //sphere
    private int[] vao_sphere1 = new int[1];
    private int[] vbo_sphere_position1 = new int[1];
    private int[] vbo_sphere_normal1 = new int[1];
    private int[] vbo_sphere_element1 = new int[1];

    int numElements;
    int numVertices;

    //lights
    private int isLightEnabled = 0;

    float updated_width = 0.0f;
    float updated_height = 0.0f;

    //Light Rotation
    float XRotation = 0.0f;
    float YRotation = 0.0f;
    float ZRotation = 0.0f;

    int key_pressed = 0; // = 1 for X Axis; = 2 for Y Axis; = 3 for Z Axis.

    float radius = 10.0f;
    
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
        if(key_pressed < 4)
        {
			key_pressed++;
            if(key_pressed == 1)	
                XRotation = 0.0f;
            else if(key_pressed == 2)
                YRotation = 0.0f;
            else if(key_pressed == 3)
                ZRotation = 0.0f;
        }
        else
        {
            key_pressed = 1;
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
        "uniform vec4 u_light_position;"+
        "out vec3 transformed_normal;"+
        "out vec3 light_direction;"+
        "out vec3 view_vector;"+
        "void main(void)"+
        "{"+
        "if(u_light_enabled == 1)"+
        "{"+
        "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
        "transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
        "light_direction = vec3(u_light_position - eye_coordinates);"+
        //SWIZZLING
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
        "in vec3 light_direction;"+
        "in vec3 view_vector;"+
        "uniform int u_light_enabled;"+
        "uniform vec3 u_LA;"+
        "uniform vec3 u_LD;"+
        "uniform vec3 u_LS;"+
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
        "vec3 normalized_light_direction = normalize(light_direction);"+
        "vec3 normalized_view_vector = normalize(view_vector);"+
        "vec3 ambient = u_LA * u_KA;"+
        "vec3 diffuse = u_LD * u_KD * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);"+
        "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);"+
        "vec3 specular = u_LS * u_KS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_material_shininess);"+
        "phong_ads_light = ambient + diffuse + specular;"+
        "}"+
        "else"+
        "{"+
        "phong_ads_light = vec3(1.0f,1.0f,1.0f);"+
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
        LaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_LA");
        LdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_LD");
        LsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_LS");
        lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position");

       	KaUniform1 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform1 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform1 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform1 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform2 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform2 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform2 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform2 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform3 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform3 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform3 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform3 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform4 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform4 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform4 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform4 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform5 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform5 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform5 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform5 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform6 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform6 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform6 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform6 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform7 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform7 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform7 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform7 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform8 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform8 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform8 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform8 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform9 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform9 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform9 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform9 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform10 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform10 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform10 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform10 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform11 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform11 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform11 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform11 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform12 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform12 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform12 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform12 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform13 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform13 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform13 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform13 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform14 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform14 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform14 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform14 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform15 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform15 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform15 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform15 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform16 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform16 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform16 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform16 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform17 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform17 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform17 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform17 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform18 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform18 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform18 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform18 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform19 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform19 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform19 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform19 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform20 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform20 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform20 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform20 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform21 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform21 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform21 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform21 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform22 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform22 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform22 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform22 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform23 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform23 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform23 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform23 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        KaUniform24 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KA");
        KdUniform24 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        KsUniform24 = GLES32.glGetUniformLocation(shaderProgramObject, "u_KS");
        materialShininessUniform24 = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        //vertices, colours, shader attribs, vbo, vao initializations
        //--------------------------------------------------------//
        //SPHERE INIT
        //--------------------------------------------------------//
        Sphere sphere = new Sphere();
        float sphere_vertices[] = new float[1146];
        float sphere_normals[] = new float[1146];
        float sphere_textures[] = new float[764];
        short sphere_elements[] = new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

        //--------------------------------------------------------//
        //SPHERE - 1
        //--------------------------------------------------------//

        // vao for SPHERE
        GLES32.glGenVertexArrays(1, vao_sphere1, 0);
        GLES32.glBindVertexArray(vao_sphere1[0]);
        
        //--------------------------------------------------------//
        // position vbo for SPHERE
        GLES32.glGenBuffers(1, vbo_sphere_position1, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_position1[0]);

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
        GLES32.glGenBuffers(1, vbo_sphere_normal1, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_normal1[0]);
        
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
        GLES32.glGenBuffers(1, vbo_sphere_element1, 0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        
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
        //--------------------------------------------------------//

        //depth
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        //disable cull (default) for animation
        GLES32.glDisable(GLES32.GL_CULL_FACE);

        //Set background colour
        GLES32.glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

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

        updated_width = width;
        updated_height = height;

    }

    private void display()
    {        
        
        //Draw background colour
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        //use shader program
        GLES32.glUseProgram(shaderProgramObject);

        //for OPENGL- ES Drawing
        float translationMatrix[] = new float[16];
        float modelMatrix[] = new float[16];
        float viewMatrix[] = new float[16];
        float projectionMatrix[] = new float[16];

        GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);
        if(isLightEnabled == 1)
        {
            if(key_pressed == 1)
            {
                lightPosition[0] = radius * (float)Math.sin(YRotation); //x
                lightPosition[1] = 0.0f; //y
                lightPosition[2] = radius * (float)Math.cos(YRotation); //z
                lightPosition[3] = 1.0f; //w
            }
            else if(key_pressed == 2)
            {
                lightPosition[0] = 0.0f; //x
                lightPosition[1] = radius * (float)Math.sin(YRotation);  //y
                lightPosition[2] = radius * (float)Math.cos(YRotation); //z
                lightPosition[3] = 1.0f; //w
            }
            else if(key_pressed == 3)
            {
                lightPosition[0] = radius * (float)Math.sin(YRotation); //x
                lightPosition[1] = radius * (float)Math.cos(YRotation); //y
                lightPosition[2] = 0.0f;//z
                lightPosition[3] = 1.0f; //w
            }
            GLES32.glUniform1i(lightEnabledUniform, 1);
            GLES32.glUniform3fv(LaUniform, 1, lightAmbient, 0);
            GLES32.glUniform3fv(LdUniform, 1, lightDiffuse, 0);
            GLES32.glUniform3fv(LsUniform, 1, lightSpecular, 0);
        }
        else
        {
            GLES32.glUniform1i(lightEnabledUniform, 0);
        }

        //--------------------------------------------------------//
        //SPHERE - 1
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport(0, (int)(updated_height * 4.65f / 6.0f), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform1, 1, materialAmbient1, 0);
        GLES32.glUniform3fv(KdUniform1, 1, materialDiffuse1, 0);
        GLES32.glUniform3fv(KsUniform1, 1, materialSpecular1, 0);
        GLES32.glUniform1f(materialShininessUniform1, materialShininess1);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 2
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)updated_width / 4, (int)(updated_height * 4.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform2, 1, materialAmbient2, 0);
        GLES32.glUniform3fv(KdUniform2, 1, materialDiffuse2, 0);
        GLES32.glUniform3fv(KsUniform2, 1, materialSpecular2, 0);
        GLES32.glUniform1f(materialShininessUniform2, materialShininess2);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 3
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)updated_width / 2, (int)(updated_height * 4.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform3, 1, materialAmbient3, 0);
        GLES32.glUniform3fv(KdUniform3, 1, materialDiffuse3, 0);
        GLES32.glUniform3fv(KsUniform3, 1, materialSpecular3, 0);
        GLES32.glUniform1f(materialShininessUniform3, materialShininess3);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 4
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width * 3.0 / 4), (int)(updated_height * 4.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform4, 1, materialAmbient4, 0);
        GLES32.glUniform3fv(KdUniform4, 1, materialDiffuse4, 0);
        GLES32.glUniform3fv(KsUniform4, 1, materialSpecular4, 0);
        GLES32.glUniform1f(materialShininessUniform4, materialShininess4);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 5
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport(0, (int)(updated_height * 3.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform5, 1, materialAmbient5, 0);
        GLES32.glUniform3fv(KdUniform5, 1, materialDiffuse5, 0);
        GLES32.glUniform3fv(KsUniform5, 1, materialSpecular5, 0);
        GLES32.glUniform1f(materialShininessUniform5, materialShininess5);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 6
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)updated_width / 4, (int)(updated_height * 3.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform6, 1, materialAmbient6, 0);
        GLES32.glUniform3fv(KdUniform6, 1, materialDiffuse6, 0);
        GLES32.glUniform3fv(KsUniform6, 1, materialSpecular6, 0);
        GLES32.glUniform1f(materialShininessUniform6, materialShininess6);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 7
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)updated_width / 2, (int)(updated_height * 3.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform7, 1, materialAmbient7, 0);
        GLES32.glUniform3fv(KdUniform7, 1, materialDiffuse7, 0);
        GLES32.glUniform3fv(KsUniform7, 1, materialSpecular7, 0);
        GLES32.glUniform1f(materialShininessUniform7, materialShininess7);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 8
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width * 3.0 / 4.0), (int)(updated_height * 3.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform8, 1, materialAmbient8, 0);
        GLES32.glUniform3fv(KdUniform8, 1, materialDiffuse8, 0);
        GLES32.glUniform3fv(KsUniform8, 1, materialSpecular8, 0);
        GLES32.glUniform1f(materialShininessUniform8, materialShininess8);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 9
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport(0, (int)(updated_height * 2.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform9, 1, materialAmbient9, 0);
        GLES32.glUniform3fv(KdUniform9, 1, materialDiffuse9, 0);
        GLES32.glUniform3fv(KsUniform9, 1, materialSpecular9, 0);
        GLES32.glUniform1f(materialShininessUniform9, materialShininess9);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 10
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)updated_width / 4, (int)(updated_height * 2.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform10, 1, materialAmbient10, 0);
        GLES32.glUniform3fv(KdUniform10, 1, materialDiffuse10, 0);
        GLES32.glUniform3fv(KsUniform10, 1, materialSpecular10, 0);
        GLES32.glUniform1f(materialShininessUniform10, materialShininess10);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 11
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)updated_width / 2, (int)(updated_height * 2.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform11, 1, materialAmbient11, 0);
        GLES32.glUniform3fv(KdUniform11, 1, materialDiffuse11, 0);
        GLES32.glUniform3fv(KsUniform11, 1, materialSpecular11, 0);
        GLES32.glUniform1f(materialShininessUniform11, materialShininess11);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 12
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width * 3.0 / 4.0), (int)(updated_height * 2.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform12, 1, materialAmbient12, 0);
        GLES32.glUniform3fv(KdUniform12, 1, materialDiffuse12, 0);
        GLES32.glUniform3fv(KsUniform12, 1, materialSpecular12, 0);
        GLES32.glUniform1f(materialShininessUniform12, materialShininess12);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 13
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport(0, (int)(updated_height * 1.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform13, 1, materialAmbient13, 0);
        GLES32.glUniform3fv(KdUniform13, 1, materialDiffuse13, 0);
        GLES32.glUniform3fv(KsUniform13, 1, materialSpecular13, 0);
        GLES32.glUniform1f(materialShininessUniform13, materialShininess13);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 14
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width / 4), (int)(updated_height * 1.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform14, 1, materialAmbient14, 0);
        GLES32.glUniform3fv(KdUniform14, 1, materialDiffuse14, 0);
        GLES32.glUniform3fv(KsUniform14, 1, materialSpecular14, 0);
        GLES32.glUniform1f(materialShininessUniform14, materialShininess14);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 15
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width / 2), (int)(updated_height * 1.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform15, 1, materialAmbient15, 0);
        GLES32.glUniform3fv(KdUniform15, 1, materialDiffuse15, 0);
        GLES32.glUniform3fv(KsUniform15, 1, materialSpecular15, 0);
        GLES32.glUniform1f(materialShininessUniform15, materialShininess15);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 16
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width * 3.0 / 4.0), (int)(updated_height * 1.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform16, 1, materialAmbient16, 0);
        GLES32.glUniform3fv(KdUniform16, 1, materialDiffuse16, 0);
        GLES32.glUniform3fv(KsUniform16, 1, materialSpecular16, 0);
        GLES32.glUniform1f(materialShininessUniform16, materialShininess16);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 17
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport(0, (int)(updated_height * 0.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform17, 1, materialAmbient17, 0);
        GLES32.glUniform3fv(KdUniform17, 1, materialDiffuse17, 0);
        GLES32.glUniform3fv(KsUniform17, 1, materialSpecular17, 0);
        GLES32.glUniform1f(materialShininessUniform17, materialShininess17);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 18
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width / 4), (int)(updated_height * 0.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform18, 1, materialAmbient18, 0);
        GLES32.glUniform3fv(KdUniform18, 1, materialDiffuse18, 0);
        GLES32.glUniform3fv(KsUniform18, 1, materialSpecular18, 0);
        GLES32.glUniform1f(materialShininessUniform18, materialShininess18);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 19
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width / 2), (int)(updated_height * 0.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform19, 1, materialAmbient19, 0);
        GLES32.glUniform3fv(KdUniform19, 1, materialDiffuse19, 0);
        GLES32.glUniform3fv(KsUniform19, 1, materialSpecular19, 0);
        GLES32.glUniform1f(materialShininessUniform19, materialShininess19);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 20
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width * 3.0 / 4.0), (int)(updated_height * 0.65 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform20, 1, materialAmbient20, 0);
        GLES32.glUniform3fv(KdUniform20, 1, materialDiffuse20, 0);
        GLES32.glUniform3fv(KsUniform20, 1, materialSpecular20, 0);
        GLES32.glUniform1f(materialShininessUniform20, materialShininess20);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 21
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport(0, (int)(updated_height * -0.3 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform21, 1, materialAmbient21, 0);
        GLES32.glUniform3fv(KdUniform21, 1, materialDiffuse21, 0);
        GLES32.glUniform3fv(KsUniform21, 1, materialSpecular21, 0);
        GLES32.glUniform1f(materialShininessUniform21, materialShininess21);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 22
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width / 4), (int)(updated_height * -0.3 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform22, 1, materialAmbient22, 0);
        GLES32.glUniform3fv(KdUniform22, 1, materialDiffuse22, 0);
        GLES32.glUniform3fv(KsUniform22, 1, materialSpecular22, 0);
        GLES32.glUniform1f(materialShininessUniform22, materialShininess22);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 23
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width / 2), (int)(updated_height * -0.3 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform23, 1, materialAmbient23, 0);
        GLES32.glUniform3fv(KdUniform23, 1, materialDiffuse23, 0);
        GLES32.glUniform3fv(KsUniform23, 1, materialSpecular23, 0);
        GLES32.glUniform1f(materialShininessUniform23, materialShininess23);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        //unbind vao
        GLES32.glBindVertexArray(0);

        //--------------------------------------------------------//

        //--------------------------------------------------------//
        //SPHERE - 24
        //--------------------------------------------------------//        
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);
        Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, projectionMatrix, 0, perspectiveProjectionMatrix, 0);
    	
        //sending MODEL, VIEW, PROJECTION Uniforms separately.
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, projectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_sphere1[0]);

        //openGL ES drawing
        GLES32.glViewport((int)(updated_width * 3.0 / 4.0), (int)(updated_height * -0.3 / 6.0), (int)updated_width / 4, (int)updated_height / 4);
        GLES32.glUniform3fv(KaUniform24, 1, materialAmbient24, 0);
        GLES32.glUniform3fv(KdUniform24, 1, materialDiffuse24, 0);
        GLES32.glUniform3fv(KsUniform24, 1, materialSpecular24, 0);
        GLES32.glUniform1f(materialShininessUniform24, materialShininess24);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1[0]);
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
    }

    private void uninitialize()
    {
        // code
        // destroy vao of SPHERE
        if(vao_sphere1[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_sphere1, 0);
            vao_sphere1[0]=0;
        }
        
        // destroy position vbo of SPHERE
        if(vbo_sphere_position1[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_position1, 0);
            vbo_sphere_position1[0]=0;
        }
        
        // destroy normal vbo of SPHERE
        if(vbo_sphere_normal1[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal1, 0);
            vbo_sphere_normal1[0]=0;
        }
        
        // destroy element vbo of SPHERE
        if(vbo_sphere_element1[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_element1, 0);
            vbo_sphere_element1[0]=0;
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

