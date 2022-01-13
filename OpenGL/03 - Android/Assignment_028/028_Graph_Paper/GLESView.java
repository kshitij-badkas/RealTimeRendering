package com.example.graph_paper;

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

    private int vao_lines[] = new int[1]; //or private int[] vao
    private int vbo_position_lines[] = new int[1];
    private int vbo_color_lines[] = new int[1];
    private int mvpUniform;

    private int colorUniform;

    private float perspectiveProjectionMatrix[] = new float[16]; //4x4 matrix
    
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
        //or draw();

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
        "in vec4 vColor;"+
        "uniform mat4 u_mvpMatrix;"+
        "void main(void)"+
        "{"+
        "gl_Position = u_mvpMatrix * vPosition;"+
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
		"out vec4 FragColor;"+
        "uniform vec4 u_Color;"+
		"void main(void)"+
		"{"+
		"FragColor = u_Color;"+
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
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.KAB_ATTRIBUTE_COLOR, "vColor");

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
        colorUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Color");


        //vertices, colours, shader attribs, vbo, vao initializations
        final float triangleVertices[] = new float[]
        {
            0.0f, 1.0f, 0.0f, //apex
            -1.0f, -1.0f, 0.0f, //left-bottom
            1.0f, -1.0f, 0.0f //right-bottom
        };

        final float triangleColors[] = new float[]
		{
            1.0f, 0.0f, 0.0f, //R
            0.0f, 1.0f, 0.0f, //G
            0.0f, 0.0f, 1.0f  //B
		};

        GLES32.glGenVertexArrays(1, vao_lines, 0);
        GLES32.glBindVertexArray(vao_lines[0]);

        //vbo for position or vertices
        GLES32.glGenBuffers(1, vbo_position_lines, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        /*ByteBuffer byteBuffer = ByteBuffer.allocateDirect(triangleVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(triangleVertices);
        verticesBuffer.position(0);*/

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            1300 * 2 * Float.BYTES, 
                            null, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //vbo for colors
        GLES32.glGenBuffers(1, vbo_color_lines, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        
        /*byteBuffer = ByteBuffer.allocateDirect(triangleColors.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer colorsBuffer = byteBuffer.asFloatBuffer();
        colorsBuffer.put(triangleColors);
        colorsBuffer.position(0);*/

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            3 * 2 * Float.BYTES, 
                            null, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_COLOR, 
                                    3, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_COLOR);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        //unbind
        GLES32.glBindVertexArray(0);

        //depth
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        //cull
        GLES32.glEnable(GLES32.GL_CULL_FACE); //only for non-animating objects

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
        float lines[] = new float[2000];
        float horizontalCentreLine[] = {-1.0f, 0.0f, 1.0f, 0.0f}; //central horizontal line
        float verticalCentreLine[] = {0.0f, 1.0f, 0.0f, -1.0f}; //central vertical line

        float redColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
        float greenColor[] = {0.0f, 1.0f, 0.0f, 1.0f};
        float blueColor[] = {0.0f, 0.0f, 1.0f, 1.0f};
        float whiteColor[] = {1.0f, 1.0f, 1.0f, 1.0f};

        //Draw background colour
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        //use shader program
        GLES32.glUseProgram(shaderProgramObject);

        //for OPENGL- ES Drawing
        float modelViewMatrix[] = new float[16];
        float modelViewProjectionMatrix[] = new float[16];
        float translationMatrix[] = new float[16];

        //identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        Matrix.setIdentityM(translationMatrix, 0);

        //Translate
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -3.0f);
        modelViewMatrix = translationMatrix;

        //multiply modelview and projection matrix to get modelviewProjection matrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        //pass above modelviewProjection matrix to vertex shader in 'u_mvp_matrix' shader variable
        //we have alreay calculated this in initWithFrame() by using glGetUniformLocation()
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_lines[0]);

        //openGL ES drawing
        //HORIZONTAL LINES TOP
        int i = 0;
        float inc = 0.05f;
        for (i = 0; i < 80; i += 4, inc += 0.05f)
        {
            lines[i] = -1.0f; //x1
            lines[i + 1] = inc; //y1
            lines[i + 2] = 1.0f; //x2
            lines[i + 3] = inc; //y2
        }
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(lines.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(lines);
        verticesBuffer.position(0);        

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            40 * 2 * Float.BYTES, 
                            verticesBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //colours
        GLES32.glUniform4fv(colorUniform, 1, blueColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 40);

        
        //HORIZONTAL LINE CENTRE
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        byteBuffer = ByteBuffer.allocateDirect(horizontalCentreLine.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(horizontalCentreLine);
        verticesBuffer.position(0);        

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            2 * 2 * Float.BYTES, 
                            verticesBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //colours
        GLES32.glUniform4fv(colorUniform, 1, redColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
        

        //HORIZONTAL LINES BOTTOM
        i = 0;
        inc = 0.05f;
        for (i = 0; i < 80; i += 4, inc += 0.05f)
        {
            lines[i] = -1.0f; //x1
            lines[i + 1] = -inc; //y1
            lines[i + 2] = 1.0f; //x2
            lines[i + 3] = -inc; //y2
        }
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        byteBuffer = ByteBuffer.allocateDirect(lines.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(lines);
        verticesBuffer.position(0);        

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            40 * 2 * Float.BYTES, 
                            verticesBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //colours
        GLES32.glUniform4fv(colorUniform, 1, blueColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 40);


	    //VERTICAL LINES LEFT
        i = 0;
        inc = 0.05f;
        for (i = 0; i < 80; i += 4, inc += 0.05f)
        {
            lines[i] = -inc; //x1
            lines[i + 1] = 1.0f; //y1
            lines[i + 2] = -inc; //x2
            lines[i + 3] = -1.0f; //y2
        }
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        byteBuffer = ByteBuffer.allocateDirect(lines.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(lines);
        verticesBuffer.position(0);        

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            40 * 2 * Float.BYTES, 
                            verticesBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //colours
        GLES32.glUniform4fv(colorUniform, 1, blueColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 40);

        //VERTICAL LINE CENTRE
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        byteBuffer = ByteBuffer.allocateDirect(verticalCentreLine.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(verticalCentreLine);
        verticesBuffer.position(0);        

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            2 * 2 * Float.BYTES, 
                            verticesBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //colours
        GLES32.glUniform4fv(colorUniform, 1, greenColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);

        //VERTICAL LINES RIGHT
        i = 0;
        inc = 0.05f;
        for (i = 0; i < 80; i += 4, inc += 0.05f)
        {
            lines[i] = inc; //x1
            lines[i + 1] = 1.0f; //y1
            lines[i + 2] = inc; //x2
            lines[i + 3] = -1.0f; //y2

        }
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        byteBuffer = ByteBuffer.allocateDirect(lines.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(lines);
        verticesBuffer.position(0);        

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            40 * 2 * Float.BYTES, 
                            verticesBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //colours
        GLES32.glUniform4fv(colorUniform, 1, blueColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 40);


        //TRAINGLE USING LINES
        lines[0] = 0.0f;
        lines[1] = 0.7f;
        lines[2] = 0.7f;
        lines[3] = -0.7f;

        lines[4] = 0.7f;
        lines[5] = -0.7f;
        lines[6] = -0.7f;
        lines[7] = -0.7f;
        
        lines[8] = -0.7f;
        lines[9] = -0.7f;
        lines[10] = 0.0f;
        lines[11] = 0.7f;
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        byteBuffer = ByteBuffer.allocateDirect(lines.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(lines);
        verticesBuffer.position(0);        

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            6 * 2 * Float.BYTES, 
                            verticesBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //colours
        GLES32.glUniform4fv(colorUniform, 1, whiteColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 6);


        //RECTANGLE USING LINES
        lines[0] = 0.7f;
        lines[1] = 0.7f;
        lines[2] = 0.7f;
        lines[3] = -0.7f;

        lines[4] = 0.7f;
        lines[5] = 0.7f;
        lines[6] = -0.7f;
        lines[7] = 0.7f;

        lines[8] = -0.7f;
        lines[9] = 0.7f;
        lines[10] = -0.7f;
        lines[11] = -0.7f;

        lines[12] = -0.7f;
        lines[13] = -0.7f;
        lines[14] = 0.7f;
        lines[15] = -0.7f;

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        byteBuffer = ByteBuffer.allocateDirect(lines.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(lines);
        verticesBuffer.position(0);        

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            8 * 2 * Float.BYTES, 
                            verticesBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //colours
        GLES32.glUniform4fv(colorUniform, 1, whiteColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 8);


        //CIRCUMCIRCLE USING POINTS
        double circumCentreRadius = 0.0f;
        circumCentreRadius = Math.sqrt(Math.pow(0.7f, 2) + Math.pow(0.7f, 2));

        i = 0;
        double cAngle = 0.0f;
        for (cAngle = 0.0f; cAngle < (2.0f * 3.14f); cAngle = cAngle + 0.01f, i += 2)
        {
            lines[i] = (float)Math.cos(cAngle) * (float)circumCentreRadius; //xn
            lines[i + 1] = (float)Math.sin(cAngle) * (float)circumCentreRadius; //yn
        }
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        byteBuffer = ByteBuffer.allocateDirect(lines.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(lines);
        verticesBuffer.position(0);        

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            650 * 2 * Float.BYTES, 
                            verticesBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //colours
        GLES32.glUniform4fv(colorUniform, 1, whiteColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glDrawArrays(GLES32.GL_POINTS, 0, 650);


        //INCIRCLE USING POINTS
        float inCircleRadius = 0.0f;
        float distance_A = 0.0f;
        float distance_B = 0.0f;
        float distance_C = 0.0f;
        float perimeter = 0.0f;
        float semi = 0.0f;
        float tArea = 0.0f;
        //centre of circle
        float iX = 0.0f;
        float iY = 0.0f;

        //	DISTANCE FORMULA
        distance_A = (float) Math.sqrt(Math.pow((-0.7f - 0.7f), 2) + Math.pow((-0.7f - -0.7f), 2));
        distance_B = (float) Math.sqrt(Math.pow((0.0f - -0.7f), 2) + Math.pow((0.7f - -0.7f), 2));
        distance_C = (float) Math.sqrt(Math.pow((0.7f - 0.0f), 2) + Math.pow((-0.7f - 0.7f), 2));

        //	SEMIPERIMETER
        perimeter = (float)(distance_A + distance_B + distance_C);
        semi = (float)(perimeter / 2);

        //	AREA OF TRIANGLE
        tArea = (float) Math.sqrt(semi * (semi - distance_A) * (float)(semi - distance_B) * (float)(semi - distance_C));

        //	INCIRCLE COORDINATES
        iX = (float)(distance_A * (0.0f) + distance_B * (0.7f) + distance_C * (-0.7f)) / perimeter;
        iY = (float)(distance_A * (0.7f) + distance_B * (-0.7f) + distance_C * (-0.7f)) / perimeter;

        inCircleRadius = (float)(tArea / semi);

        //OUTPUT ->
        
        i = 0;
        cAngle = 0.0f;
        for (cAngle = 0.0f; cAngle < (2.0f * 3.14f); cAngle = cAngle + 0.01f, i += 2)
        {
            lines[i] = iX + (float)Math.cos(cAngle) * (float)inCircleRadius; //xn
            lines[i + 1] = iY + (float)Math.sin(cAngle) * (float)inCircleRadius; //yn
        }

                GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

        byteBuffer = ByteBuffer.allocateDirect(lines.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder()); //little-endian or big-endian depends on CPU
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(lines);
        verticesBuffer.position(0);        

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            650 * 2 * Float.BYTES, 
                            verticesBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSITION, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //colours
        GLES32.glUniform4fv(colorUniform, 1, whiteColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glDrawArrays(GLES32.GL_POINTS, 0, 650);



        // --------------------------------------------- //
        //unbind vao
        GLES32.glBindVertexArray(0);

        //ususe shader program
        GLES32.glUseProgram(0);

        //double buffers is called in Game Loop -> onDrawFrame()
    }

    private void update()
    {
        //code
    }

    private void uninitialize()
    {
        //code
        //destroy vao
        if(vao_lines[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_lines, 0);
            vao_lines[0] = 0;
        }

        //destroy vbo of position
        if(vbo_position_lines[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vbo_position_lines, 0);
            vbo_position_lines[0] = 0;
        }

        //destroy vbo of color
        if(vbo_color_lines[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vbo_color_lines, 0);
            vbo_color_lines[0] = 0;
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

