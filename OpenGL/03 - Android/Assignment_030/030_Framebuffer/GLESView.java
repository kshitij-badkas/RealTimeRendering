package com.example.framebuffer;

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

//new
//for Textures
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
    //fields
    private final Context context;
    private GestureDetector gestureDetector;

    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;

    private int vertexShaderObject_2;
    private int fragmentShaderObject_2;
    private int shaderProgramObject_2;
    
    //cube
    private int vao_cube[] = new int[1];
    private int vbo_position_cube[] = new int[1];
    private int vbo_texcoord_cube[] = new int[1];

    private int vao_cube_2[] = new int[1];
    private int vbo_position_cube_2[] = new int[1];
    private int vbo_color_cube_2[] = new int[1];
    
    private int mvpUniform;
    private int mvpUniform_2;

    private float perspectiveProjectionMatrix[] = new float[16]; //4x4 matrix
    
    //animation
    private static float angleCube = 0.0f;

    //textures
    private int kundali_texture[] = new int[1];
    private int textureSamplerUniform;

    //framebuffer
    private int fbo[] = new int[1];
    private int color_texture[] = new int[1];
    private int depth_texture[] = new int[1];
    private int fbWidth;
    private int fbHeight;


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
        "in vec4 vPosition;"+
        "in vec2 vTexCoord;"+
        "uniform mat4 u_mvpMatrix;"+
        "out vec2 out_texcoord;"+
        "void main(void)"+
        "{"+
        "gl_Position = u_mvpMatrix * vPosition;"+
        "out_texcoord = vTexCoord;"+
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
        "in vec2 out_texcoord;"+
        "uniform highp sampler2D u_texture_sampler;"+
		"out vec4 FragColor;"+
		"void main(void)"+
		"{"+
		"FragColor = texture(u_texture_sampler, out_texcoord);"+
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
        
        //pre-link binding of shader program object with fragment shader attribute
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.KAB_ATTRIBUTE_TEXCOORD, "vTexCoord");

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

        //get MVP uniform location
        mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
        textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_texture_sampler");

        //2nd cube
        // ****************************************************
        // VERTEX SHADER FOR COLOR
        // ****************************************************
        //create shader
        vertexShaderObject_2 = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

        //use 'OpenGL ES 3.20'
        // static function - called with Class (String)
        final String vertexShaderSourceCode_2 = String.format 
        (
        "#version 320 es"+
        "\n"+
        "in vec4 vPosition;"+
        "in vec4 vColor;"+
        "uniform mat4 u_mvpMatrix;"+
        "out vec4 out_color;"+
        "void main(void)"+
        "{"+
        "gl_Position = u_mvpMatrix * vPosition;"+
        "out_color = vColor;"+
        "}"
        );

        //provide source code to shader
        GLES32.glShaderSource(vertexShaderObject_2, vertexShaderSourceCode_2);

        //compile shader
        GLES32.glCompileShader(vertexShaderObject_2);

        //error checking
        iShaderCompiledStatus[0] = 0;
        iInfoLogLength[0] = 0;
        szInfoLog = null;
        GLES32.glGetShaderiv(vertexShaderObject_2, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObject_2, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject_2);
                System.out.println("KAB : Vertex Shader Complilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        } 

        // ****************************************************
        // FRAGMENT SHADER FOR COLOR
        // ****************************************************
        //create shader
        fragmentShaderObject_2 = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

	    final String fragmentShaderSourceCode_2 = String.format
        (
		"#version 320 es"+
		"\n"+
        "precision highp float;"+
        "in vec4 out_color;"+
		"out vec4 FragColor;"+
		"void main(void)"+
		"{"+
		"FragColor = out_color;"+
		"}"
        );

        //provide source code to shader
        GLES32.glShaderSource(fragmentShaderObject_2, fragmentShaderSourceCode_2);

        //compile shader
        GLES32.glCompileShader(fragmentShaderObject_2);

        //error checking
        iShaderCompiledStatus[0] = 0;
        iInfoLogLength[0] = 0;
        szInfoLog = null;
        GLES32.glGetShaderiv(fragmentShaderObject_2, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObject_2, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject_2);
                System.out.println("KAB : Fragment Shader Complilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
        // ****************************************************
        // SHADER PROGRAM OBJECT 2
        // ****************************************************
        //create shader program
        shaderProgramObject_2 = GLES32.glCreateProgram(); 

        //attach vertex shader to shader program
        GLES32.glAttachShader(shaderProgramObject_2, vertexShaderObject_2);

        //attach fragment shader to shader program
        GLES32.glAttachShader(shaderProgramObject_2, fragmentShaderObject_2);

        //pre-link binding of shader program object with vertex shader attributes
        GLES32.glBindAttribLocation(shaderProgramObject_2, GLESMacros.KAB_ATTRIBUTE_POSITION, "vPosition");
        
        //pre-link binding of shader program object with fragment shader attribute
        GLES32.glBindAttribLocation(shaderProgramObject_2, GLESMacros.KAB_ATTRIBUTE_TEXCOORD, "vTexCoord");

        //link the two shaders together to shader program object
        GLES32.glLinkProgram(shaderProgramObject_2);

        //error checking
        iShaderProgramLinkStatus[0] = 0;
        iInfoLogLength[0] = 0;
        szInfoLog = null;
        GLES32.glGetShaderiv(shaderProgramObject_2, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if(iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(shaderProgramObject_2, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if(iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(shaderProgramObject_2);
                System.out.println("KAB : Shader Program Object Link Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        //get MVP uniform location
        mvpUniform_2 = GLES32.glGetUniformLocation(shaderProgramObject_2, "u_mvpMatrix");

        //vertices, colours, shader attribs, vbo, vao initializations        
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

        final float cubeTexCoords[] = new float[]
        {
            //FRONT
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            //RIGHT
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            //BACK
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            //LEFT
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            //TOP
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            //BOTTOM
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };      

        
        final float cubeColors[] = new float[]
        {
            //FRONT
            1.0f, 0.0f, 0.0f, //R
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            //RIGHT
            0.0f, 1.0f, 0.0f, //G
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            //BACK
            0.0f, 0.0f, 1.0f, //B
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            //LEFT
            0.0f, 1.0f, 1.0f, //C
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            //TOP
            1.0f, 0.0f, 1.0f, //M
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            //BOTTOM
            1.0f, 1.0f, 0.0f, //Y
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
        };        
        


        //Generate and Bind for Cube 
        GLES32.glGenVertexArrays(1, vao_cube, 0);
        GLES32.glBindVertexArray(vao_cube[0]);

        GLES32.glGenBuffers(1, vbo_position_cube, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_cube[0]);

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(cubeVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer cubeVerticesBuffer = byteBuffer.asFloatBuffer();
        cubeVerticesBuffer.put(cubeVertices);
        cubeVerticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            cubeVertices.length * 4, 
                            cubeVerticesBuffer, 
                            GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    3, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //--------------------------------------------------------//
        //vbo for cube tex-coords        
        GLES32.glGenBuffers(1, vbo_texcoord_cube, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord_cube[0]);

        byteBuffer = ByteBuffer.allocateDirect(cubeTexCoords.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer cubeTexCoordsBuffer = byteBuffer.asFloatBuffer();
        cubeTexCoordsBuffer.put(cubeTexCoords);
        cubeTexCoordsBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            cubeTexCoords.length * 4, 
                            cubeTexCoordsBuffer, 
                            GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_TEXCOORD, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_TEXCOORD);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        //Unbind for Cube
        GLES32.glBindVertexArray(0);
        

        //2nd cube
        //--------------------------------------------------------//

        //Generate and Bind fro color
        GLES32.glGenVertexArrays(1, vao_cube_2, 0);
        GLES32.glBindVertexArray(vao_cube_2[0]);

        GLES32.glGenBuffers(1, vbo_position_cube_2, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_cube_2[0]);

        byteBuffer = ByteBuffer.allocateDirect(cubeVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        cubeVerticesBuffer = byteBuffer.asFloatBuffer();
        cubeVerticesBuffer.put(cubeVertices);
        cubeVerticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            cubeVertices.length * 4, 
                            cubeVerticesBuffer, 
                            GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    3, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //--------------------------------------------------------//
        //vbo for cube color        
        GLES32.glGenBuffers(1, vbo_color_cube_2, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_cube_2[0]);

        byteBuffer = ByteBuffer.allocateDirect(cubeColors.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer cubeColorsBuffer = byteBuffer.asFloatBuffer();
        cubeColorsBuffer.put(cubeColors);
        cubeColorsBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            cubeColors.length * 4, 
                            cubeColorsBuffer, 
                            GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_COLOR, 
                                    3, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_COLOR);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        //Unbind for Cube
        GLES32.glBindVertexArray(0);
        
        //--------------------------------------------------------//

        //depth
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        //disable cull (default) for animation
        GLES32.glDisable(GLES32.GL_CULL_FACE);

        //Loading TExtures
        kundali_texture[0] = loadGLTexture(R.raw.vijay_kundali);

        //Enable Textures
        GLES32.glEnable(GLES32.GL_TEXTURE_2D);

        //--------------------------------------------------------//

        
        //FRAMEBUFFER
        GLES32.glGenFramebuffers(1, fbo, 0);
        GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER ,fbo[0]);
        
        
        GLES32.glGenTextures(1, color_texture, 0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, color_texture[0]);


        GLES32.glTexImage2D(GLES32.GL_TEXTURE_2D, 0, GLES32.GL_RGBA, 512, 512, 0, GLES32.GL_RGBA, GLES32.GL_UNSIGNED_BYTE, null);
        
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, 
                                GLES32.GL_TEXTURE_MIN_FILTER, 
                                GLES32.GL_LINEAR);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D,
                                GLES32.GL_TEXTURE_MAG_FILTER, 
                                GLES32.GL_LINEAR);

        
        GLES32.glGenRenderbuffers(1, depth_texture, 0);
        GLES32.glBindRenderbuffer(GLES32.GL_RENDERBUFFER, depth_texture[0]);
        GLES32.glRenderbufferStorage(GLES32.GL_RENDERBUFFER, GLES32.GL_DEPTH_COMPONENT16, 512, 512);

        GLES32.glFramebufferTexture2D(GLES32.GL_FRAMEBUFFER, GLES32.GL_COLOR_ATTACHMENT0, GLES32.GL_TEXTURE_2D, 
                                                                                                    color_texture[0], 0);

        GLES32.glFramebufferRenderbuffer(GLES32.GL_FRAMEBUFFER, GLES32.GL_DEPTH_ATTACHMENT, GLES32.GL_RENDERBUFFER,
                                                                                                    depth_texture[0]);

        //error check
        int e = GLES32.glCheckFramebufferStatus(GLES32.GL_FRAMEBUFFER);
        if(e == GLES32.GL_FRAMEBUFFER_COMPLETE)
        {
            System.out.println("KAB : FBO IS COMPLETE.");
        }
        else
        {
            System.out.println("KAB : FBO IS 'INCOMPLETE'.");
        }
        //----------------------------------------------------------------//
        

        //Set background colour
        GLES32.glClearColor(0.8f, 0.8f, 0.8f, 1.0f); //black

        //set projectionMatrix to identity matrix
        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
    }

    private int loadGLTexture(int imageFileResourceID)
    {
        //code
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;
        Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imageFileResourceID, options);

        int texture[] = new int[1];

        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);

        GLES32.glGenTextures(1, texture, 0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);

        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, 
                                GLES32.GL_TEXTURE_MAG_FILTER, 
                                GLES32.GL_LINEAR);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D,
                                GLES32.GL_TEXTURE_MIN_FILTER, 
                                GLES32.GL_LINEAR_MIPMAP_LINEAR);

        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
        GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);

        return texture[0];
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

        fbWidth = width;
        fbHeight = height;
    }

    private void display()
    {
        //for OPENGL- ES Drawing
        float translationMatrix[] = new float[16];
        float scaleMatrix[] = new float[16];
        float rotationMatrix[] = new float[16];
        float modelViewMatrix[] = new float[16];
        float modelViewProjectionMatrix[] = new float[16];
        
        //--------------------------------------------------------//
        //CUBE - 1
        //--------------------------------------------------------//
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(scaleMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        
        //Transformation
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -6.0f);
        Matrix.scaleM(scaleMatrix, 0, 0.85f, 0.85f, 0.85f);
        Matrix.rotateM(rotationMatrix, 0, angleCube, 1.0f, 0.0f, 0.0f); //X-AXIS Rotation
        Matrix.rotateM(rotationMatrix, 0, angleCube, 0.0f, 1.0f, 0.0f); //Y-AXIS Rotation
        Matrix.rotateM(rotationMatrix, 0, angleCube, 0.0f, 0.0f, 1.0f); //Z-AXIS Rotation
        
        Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, scaleMatrix, 0);
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        //FRAMEBUFFER
        GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER ,fbo[0]);

        GLES32.glViewport(0, 0, 512, 512);
        GLES32.glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        GLES32.glUseProgram(shaderProgramObject_2);
        GLES32.glUniformMatrix4fv(mvpUniform_2, 1, false, modelViewProjectionMatrix, 0);
        GLES32.glBindVertexArray(vao_cube_2[0]);
        
        //openGL ES drawing
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

        //unbind FRAMEBUFFFER
        GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);        

        //unuse shader program
        GLES32.glUseProgram(0);

        ///////////////////////////////////

        GLES32.glViewport(0, 0, fbWidth, fbHeight);
        GLES32.glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
        GLES32.glUseProgram(shaderProgramObject);

        
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, color_texture[0]);
        
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
        GLES32.glBindVertexArray(vao_cube[0]);
        
        //openGL ES drawing
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

        GLES32.glBindVertexArray(0);

        //unuse shader program
        GLES32.glUseProgram(0);

        

        //double buffers is called in Game Loop -> onDrawFrame()
    }

    private void update()
    {
        //code
		angleCube += 1.0f;
        if (angleCube >= 360.0f)
        {
            angleCube = 0.0f;
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

        //destroy vbo of cube color
        if(vbo_texcoord_cube[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vbo_texcoord_cube, 0);
            vbo_texcoord_cube[0] = 0;
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

