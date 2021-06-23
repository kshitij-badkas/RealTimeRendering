package com.example.diffuse_cube;

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
    
    //cube
    private int vao_cube[] = new int[1];
    private int vbo_position_cube[] = new int[1];
    private int vbo_normal_cube[] = new int[1];

    //Uniforms
    private int modelViewMatrixUniform;
    private int projectionMatrixUniform;
    private int doubleTapUniform; //LKeyPressedUniform
    private int LdUniform;
    private int KdUniform;
    private int lightPositionUniform;
    
    //animation
    private static float angleCube = 0.0f;
    private int single_tap = 0;
    private int double_tap = 0;

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
        double_tap ++;
        if(double_tap > 1)
        {
            double_tap = 0;
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
        single_tap ++;
        if(single_tap > 1)
        {
            single_tap = 0;
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
        "precision mediump float;"+
        "in vec4 vPosition;"+
        "in vec3 vNormal;"+
        "uniform mat4 u_model_view_matrix;"+
        "uniform mat4 u_projection_matrix;"+
        "uniform mediump int u_double_tap;"+
        "uniform vec3 u_LD;"+
        "uniform vec3 u_KD;"+
        "uniform vec4 u_light_position;"+
        "out vec3 diffuse_light;"+
        "void main(void)"+
        "{"+
            "if(u_double_tap == 1)"+
            "{"+
            "vec4 eye_coordinates = u_model_view_matrix * vPosition;"+
			"mat3 normal_matrix = mat3(transpose(inverse(u_model_view_matrix)));"+
			"vec3 tnorm = normalize(normal_matrix * vNormal);"+
			"vec3 s = normalize(vec3(u_light_position - eye_coordinates));"+
			"diffuse_light = u_LD * u_KD * max(dot(s, tnorm), 0.0f);"+
			"}"+
			"gl_Position = u_projection_matrix * u_model_view_matrix * vPosition;"+
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
        "in vec3 diffuse_light;"+
        "uniform int u_double_tap;"+
		"out vec4 FragColor;"+
		"void main(void)"+
		"{"+
            "vec4 color;"+
            "if(u_double_tap == 1)"+
            "{"+
            "color = vec4(diffuse_light, 1.0f);"+
            "}"+
            "else"+
		    "{"+
			"color = vec4(1.0f, 1.0f, 1.0f, 1.0f);"+
			"}"+
			"FragColor = color;"+
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
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.KAB_ATTRIBUTE_POSTION, "vPosition");

        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.KAB_ATTRIBUTE_NORMAL, "vNormal");

        
        //link the two shaders together to shader program object
        GLES32.glLinkProgram(shaderProgramObject);

        //error checking
        int iShaderProgramLinkStatus[] = new int[1];
        iInfoLogLength[0] = 0;
        szInfoLog = null;
        GLES32.glGetShaderiv(shaderProgramObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
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

        //get uniform location
        modelViewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_model_view_matrix");
        projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
        doubleTapUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_double_tap");
        LdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_LD");
        KdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_KD");
        lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position");

        //vertices, colours, shader attribs, vbo, vao initialization
        //--------------------------------------------------------//
        //CUBE
        //--------------------------------------------------------//
        final float cubeVertices[] = new float[]
        {
            //FRONT
            1.0f, 1.0f, 1.0f, //right-top
            -1.0f, 1.0f, 1.0f, //left-top
            -1.0f, -1.0f, 1.0f, //left-bottom
            1.0f, -1.0f, 1.0f, //right-bottom
            //RIGHT
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
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
            //TOP
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            //BOTTOM
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
        };

        final float cubeNormals[] = new float[]
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
        
        //Generate and Bind for Cube 
        GLES32.glGenVertexArrays(1, vao_cube, 0);
        GLES32.glBindVertexArray(vao_cube[0]);

        //////////////////////////////////////////////////
        //--------------- CUBE POSITION ----------------//
        //////////////////////////////////////////////////
        GLES32.glGenBuffers(1, vbo_position_cube, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_cube[0]);

        ByteBuffer byteBuffer_vertices = ByteBuffer.allocateDirect(cubeVertices.length * 4);
        byteBuffer_vertices.order(ByteOrder.nativeOrder());
        FloatBuffer cubeVerticesBuffer = byteBuffer_vertices.asFloatBuffer();
        cubeVerticesBuffer.put(cubeVertices);
        cubeVerticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            cubeVertices.length * 4, 
                            cubeVerticesBuffer, 
                            GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSTION, 
                                    3, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSTION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        //////////////////////////////////////////////////
        //--------------- CUBE NORMAL ------------------//
        //////////////////////////////////////////////////
        GLES32.glGenBuffers(1, vbo_normal_cube, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_normal_cube[0]);

        ByteBuffer byteBuffer_normals = ByteBuffer.allocateDirect(cubeNormals.length * 4);
        byteBuffer_normals.order(ByteOrder.nativeOrder());
        FloatBuffer cubeNormalsBuffer = byteBuffer_normals.asFloatBuffer();
        cubeNormalsBuffer.put(cubeNormals);
        cubeNormalsBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            cubeNormals.length * 4, 
                            cubeNormalsBuffer, 
                            GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_NORMAL, 
                                    3, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_NORMAL);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //Unbind for Cube
        GLES32.glBindVertexArray(0);
        
        //--------------------------------------------------------//

        //depth
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        //disable cull (default) for animation
        GLES32.glDisable(GLES32.GL_CULL_FACE);

        //Set background colour
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black

        //set perspectiveProjectionMatrix to identity matrix
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
        //Draw background colour
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        //use shader program
        GLES32.glUseProgram(shaderProgramObject);

        if(double_tap == 1) //lights on
        {
            GLES32.glUniform1i(doubleTapUniform, 1);
            GLES32.glUniform3f(LdUniform, 1.0f, 1.0f, 1.0f); //white
            GLES32.glUniform3f(KdUniform, 0.5f, 0.5f, 0.5f); //grey
            float lightPosition[] = new float[] 
            { 
                0.0f, 0.0f, 2.0f, 1.0f 
            };
            GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);
        }
        else
        {
            GLES32.glUniform1i(doubleTapUniform, 0);
        }

        //for OPENGL- ES Drawing
        float translationMatrix[] = new float[16];
        float scaleMatrix[] = new float[16];
        float rotationMatrix[] = new float[16];
        float modelViewMatrix[] = new float[16];

        //--------------------------------------------------------//
        //CUBE
        //--------------------------------------------------------//
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(scaleMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -6.0f);
        Matrix.scaleM(scaleMatrix, 0, 0.85f, 0.85f, 0.85f);
        Matrix.rotateM(rotationMatrix, 0, angleCube, 1.0f, 0.0f, 0.0f); //X-AXIS Rotation
        Matrix.rotateM(rotationMatrix, 0, angleCube, 0.0f, 1.0f, 0.0f); //Y-AXIS Rotation
        Matrix.rotateM(rotationMatrix, 0, angleCube, 0.0f, 0.0f, 1.0f); //Z-AXIS Rotation
        
        //
        //ORDER IS ModelView = Tranlate * Scale * Rotate
        //
        //multiply translation and scale matrix firstly
        Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, scaleMatrix, 0);
        //multiply the above result with rotation matrix secondly
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);
        
        //Model-View matrix and Projection matrix will be sent separately. 
        GLES32.glUniformMatrix4fv(modelViewMatrixUniform, 1, false, modelViewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_cube[0]);

        //openGL ES drawing
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);


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
        if(single_tap == 1) //animation on
        {
            angleCube += 1.0f;
            if (angleCube >= 360.0f)
            {
                angleCube = 0.0f;
            }
        }
		
    }

    private void uninitialize()
    {
        //code
        //destroy vao of cube
        if(vao_cube[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_cube, 0);
            vao_cube[0] = 0;
        }
        
        //destroy vbo of cube position
        if(vbo_position_cube[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vbo_position_cube, 0);
            vbo_position_cube[0] = 0;
        }

        //destroy vbo of cube normal
        if(vbo_normal_cube[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vbo_normal_cube, 0);
            vbo_normal_cube[0] = 0;
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

