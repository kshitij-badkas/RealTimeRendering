package com.example.tweaked_smiley;

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

    private float perspectiveProjectionMatrix[] = new float[16]; //4x4 matrix

    //square
    private int vao_square[] = new int[1];
    private int vbo_position_square[] = new int[1];
    private int vbo_color_square[] = new int[1];
    private int vbo_texcoord_square[] = new int[1];
    
    //uniform
    private int mvpUniform;
    private int textureSamplerUniform;
    private int otherEventOccurredUniform;

    //textures
    private int smiley_texture[] = new int[1];

    //gestures 
    private int single_tap = 0;

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
        //Single Tap Event Counter
        single_tap ++;
        System.out.println("KAB :"+"single_tap = "+single_tap);
        if(single_tap > 4)
        {
            single_tap = 0;
            System.out.println("KAB :"+" Inside if() single_tap = "+single_tap);

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

        System.out.println("KAB :"+" Scroll");
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
		"uniform int u_other_event_occurred;"+
        "out vec4 FragColor;"+
        "void main(void)"+
        "{"+
            "vec4 color;"+
            "if(u_other_event_occurred == 1)"+
            "{"+
            "color = vec4(1.0f, 1.0f, 1.0f, 1.0f);"+
            "}"+
            "else"+
            "{"+
            "color = texture(u_texture_sampler, out_texcoord);"+
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

        //get Texture Sampler uniform location
        textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_texture_sampler");

        otherEventOccurredUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_other_event_occurred");

        //vertices, colours, shader attribs, vbo, vao initializations
        //square
        final float squareVertices[] = new float[]
        {
            1.0f, 1.0f, 0.0f, //right-top
            -1.0f, 1.0f, 0.0f, //left-top
            -1.0f, -1.0f, 0.0f, //left-bottom
            1.0f, -1.0f, 0.0f //right-bottom
        };

        //--------------------------------------------------------//
        //SQUARE
        //--------------------------------------------------------//
        //Generate and Bind for Square 
        GLES32.glGenVertexArrays(1, vao_square, 0);
        GLES32.glBindVertexArray(vao_square[0]);

        //vbo for square position        
        GLES32.glGenBuffers(1, vbo_position_square, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_square[0]);

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(squareVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer squareVerticesBuffer = byteBuffer.asFloatBuffer();
        squareVerticesBuffer.put(squareVertices);
        squareVerticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            squareVertices.length * 4, 
                            squareVerticesBuffer, 
                            GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_POSTION, 
                                    3, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_POSTION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        //--------------------------------------------------------//
        //vbo for square tex-coords        
        GLES32.glGenBuffers(1, vbo_texcoord_square, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord_square[0]);

        //ByteBuffer allocation will be done dynamically
    
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            4 * 2 * 4, 
                            null, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_TEXCOORD, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_TEXCOORD);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        //--------------------------------------------------------//
        //Unbind for Square
        GLES32.glBindVertexArray(0);
        
        //--------------------------------------------------------//
        //depth
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        //cull
        //GLES32.glEnable(GLES32.GL_CULL_FACE); //only for non-animating objects
        
        //Loading TExtures
        smiley_texture[0] = loadGLTexture(R.raw.smiley);

        //Enable Textures
        GLES32.glEnable(GLES32.GL_TEXTURE_2D);

        //Set background colour
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black

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
    }

    private void display()
    {
        //variables
        float squareTexCoords[] = new float[8];
        
        //Draw background colour
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        //use shader program
        GLES32.glUseProgram(shaderProgramObject);

        //for OPENGL- ES Drawing
        float translationMatrix[] = new float[16];
        float modelViewMatrix[] = new float[16];
        float modelViewProjectionMatrix[] = new float[16];

        //--------------------------------------------------------//
        //SQUARE
        //--------------------------------------------------------//
        //identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        //Translate
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -6.0f);
        modelViewMatrix = translationMatrix;

        //multiply modelview and projection matrix to get modelviewProjection matrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        //pass above modelviewProjection matrix to vertex shader in 'u_mvp_matrix' shader variable
        //we have alreay calculated this in initWithFrame() by using glGetUniformLocation()
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //textures
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, smiley_texture[0]);
        GLES32.glUniform1i(textureSamplerUniform, 0);

        //bind vao
        GLES32.glBindVertexArray(vao_square[0]);
        
        //draw
        //other event has not occurred and 0 < tap count < 4
        GLES32.glUniform1i(otherEventOccurredUniform, 0); 
        if (single_tap == 1)
        {
            squareTexCoords[0] = 1.0f;
            squareTexCoords[1] = 1.0f;
            squareTexCoords[2] = 0.0f;
            squareTexCoords[3] = 1.0f;
            squareTexCoords[4] = 0.0f;
            squareTexCoords[5] = 0.0f;
            squareTexCoords[6] = 1.0f;
            squareTexCoords[7] = 0.0f;
        }
        
        else if (single_tap == 2)
        {
            squareTexCoords[0] = 0.5f;
            squareTexCoords[1] = 0.5f;
            squareTexCoords[2] = 0.0f;
            squareTexCoords[3] = 0.5f;
            squareTexCoords[4] = 0.0f;
            squareTexCoords[5] = 0.0f;
            squareTexCoords[6] = 0.5f;
            squareTexCoords[7] = 0.0f;
        }
        else if (single_tap == 3)
        {
            squareTexCoords[0] = 2.0f;
            squareTexCoords[1] = 2.0f;
            squareTexCoords[2] = 0.0f;
            squareTexCoords[3] = 2.0f;
            squareTexCoords[4] = 0.0f;
            squareTexCoords[5] = 0.0f;
            squareTexCoords[6] = 2.0f;
            squareTexCoords[7] = 0.0f;
        }
        else if (single_tap == 4)
        {
            squareTexCoords[0] = 0.5f;
            squareTexCoords[1] = 0.5f;
            squareTexCoords[2] = 0.5f;
            squareTexCoords[3] = 0.5f;
            squareTexCoords[4] = 0.5f;
            squareTexCoords[5] = 0.5f;
            squareTexCoords[6] = 0.5f;
            squareTexCoords[7] = 0.5f;
        }
        else
        {
            //other event has occurred and tap count = 0 or greater than 4           
            GLES32.glUniform1i(otherEventOccurredUniform, 1);
        }
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord_square[0]);

        //ByteBuffer allocation will be done dynamically
        ByteBuffer byteBuffer_texcoord = ByteBuffer.allocateDirect(squareTexCoords.length * 4);
        byteBuffer_texcoord.order(ByteOrder.nativeOrder());
        FloatBuffer squareTexCoordsBuffer = byteBuffer_texcoord.asFloatBuffer();
        squareTexCoordsBuffer.put(squareTexCoords);
        squareTexCoordsBuffer.position(0);

       
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 
                            4 * 2 * 4, 
                            squareTexCoordsBuffer, 
                            GLES32.GL_DYNAMIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.KAB_ATTRIBUTE_TEXCOORD, 
                                    2, 
                                    GLES32.GL_FLOAT, 
                                    false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.KAB_ATTRIBUTE_TEXCOORD);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        //openGL ES drawing
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

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
        //code
        //destroy vao of square
        if(vao_square[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_square, 0);
            vao_square[0] = 0;
        }
        
        //destroy vbo of square position
        if(vbo_position_square[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vbo_position_square, 0);
            vbo_position_square[0] = 0;
        }

        //destroy vbo of sqaure color
        if(vbo_color_square[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vbo_color_square, 0);
            vbo_color_square[0] = 0;
        }  
        
        //destroy vbo of square texcoords
        if(vbo_texcoord_square[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vbo_texcoord_square, 0);
            vbo_texcoord_square[0] = 0;
        }

        //destroy texture
        if(smiley_texture[0] != 0)
        {
            GLES32.glDeleteTextures(1, smiley_texture, 0);
            smiley_texture[0] = 0;
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

