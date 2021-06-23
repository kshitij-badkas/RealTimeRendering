//global variables
var canvas = null;
var gl = null;
var canvas_original_width;
var canvas_original_height;
var bFullscreen = false;

//declare function pointer
var requestAnimationFrame = window.requestAnimationFrame || 
                            window.webkitRequestAnimationFrame ||
                            window.mozRequestAnimationFrame ||
                            window.oRequestAnimationFrame || 
                            window.msRequestAnimationFrame;

//new
//when whole 'WebGLMacros' is 'const', all insisde it are automatically const.
const WebGLMacros = 
{
    KAB_ATTRIBUTE_POSITION:0,
    KAB_ATTRIBUTE_COLOR:1,
    KAB_ATTRIBUTE_NORMAL:2,
    KAB_ATTRIBUTE_TEXCOORD:3
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

//cube
var vao_cube;
var vbo_position_cube;
var vbo_color_cube;
var vbo_normal_cube;

//uniform
var modelViewMatrixUniform;
var projectionMatrixUniform;
var LKeyPressedUniform;
var LdUniform;
var KdUniform;
var lightPositionUniform;

var perspectiveProjectionMatrix;

//animation
var angleCube = 0.0;
var isCubeAnimated = 0;
var isLightEnabled = 0;

/*
//not required in webgl//////////////////////////////////////////////////////////////////////////////////////////
var cancelAnimationFrame = window.cancelAnimationFrame ||
                            window.webkitCancelRequestAnimationFrame || window.mozCancelRequestAnimationFrame ||
                            window.oCancelRequestAnimationFrame || window.oCancelAnimationFrame ||
                            window.msCancelRequestAnimationFrame || window.msCancelAnimationFrame;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

function main()
{
    //Get Canvas from DOM - Document Object Model; here HTML
    //type-inference : automatically guess the type of right hand side and use 'var' on left hand side
    canvas = document.getElementById("KAB"); 
    if(!canvas)
    {
        console.log("Obtaining Canvas Failed! \n");
    }
    else
    {
        console.log("Obtaining Canvas Succeeded \n");
    }

    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;

    //Get opengl Context from the Canvas
    gl = canvas.getContext("webgl2");
    if(!gl)
    {
        console.log("webGL2 Context Failed! \n");
    }
    else
    {
        console.log("webGL2 Context Succeeded \n");
    }

    //Add Event Listener
    //'window' is not declared beacuse it is an in-built variable
    //'window' is inherited from 'document' --> so, it is also DOM
    // "keydown" , "click" -- in build registered event of JavaScript
    // 'false'  - follow bubble propagation instead of capture
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
    window.addEventListener("resize", resize, false);

    init();

    resize(); //warm-up resize

    display(); //warm-up display

}

//init()
function init()
{
    //code
    //to maintain our code consistency, create context in initialize() 
    //get WebGL 2.0 context
    gl = canvas.getContext("webgl2");
    if(gl == null)
    {
        console.log("Failed to get the rendering context for WebGL");
        return;
    }
    
    gl.viewPortWidth = canvas.width;
    gl.viewPortHeight = canvas.height;

    ////////////////////////////////////////////////////////////////
    //
    //                      VERTEX SHADER
    //
    ////////////////////////////////////////////////////////////////
    var vertexShaderSourceCode = 
    "#version 300 es"+
    "\n"+
    "precision mediump float;"+
    "in vec4 vPosition;"+
    "in vec3 vNormal;"+
    "uniform mat4 u_model_view_matrix;"+
    "uniform mat4 u_projection_matrix;"+
    "uniform mediump int u_L_KEY_PRESSED;"+
    "uniform vec3 u_LD;"+
    "uniform vec3 u_KD;"+
    "uniform vec4 u_light_position;"+
    "out vec3 diffuse_light;"+
    "void main(void)"+
    "{"+
    "if(u_L_KEY_PRESSED == 1)"+
	"{"+
        "vec4 eye_coordinates = u_model_view_matrix * vPosition;"+
        "mat3 normal_matrix = mat3(transpose(inverse(u_model_view_matrix)));"+
        "vec3 tnorm = normalize(normal_matrix * vNormal);"+
        "vec3 s = normalize(vec3(u_light_position - eye_coordinates));"+
        "diffuse_light = u_LD * u_KD * max(dot(s, tnorm), 0.0f);"+
    "}"+
    "gl_Position =  u_projection_matrix * u_model_view_matrix * vPosition;"+
    "}";

    vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);
    if(gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if(error.length > 0)
        {
            alert("VERTEX SHADER ERROR LOG : "+error);
            uninitialize();
        }
    }

    ////////////////////////////////////////////////////////////////
    //
    //                      FRAGMENT SHADER
    //
    ////////////////////////////////////////////////////////////////
    var fragmentShaderSourceCode = 
    "#version 300 es"+
    "\n"+
    "precision highp float;"+
    "in vec3 diffuse_light;"+
    "uniform mediump int u_L_KEY_PRESSED;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
        "vec4 color;"+
        "if(u_L_KEY_PRESSED == 1)"+
        "{"+
        "color = vec4(diffuse_light, 1.0f);"+
        "}"+
        "else"+
        "{"+
        "color = vec4(1.0f, 1.0f, 1.0f, 1.0f);"+
        "}"+
        "FragColor = color;"+
    "}";

    fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);
    if(gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if(error.length > 0)
        {
            alert("FRAGMENT SHADER ERROR LOG : "+error);
            uninitialize();
        }
    }

    ////////////////////////////////////////////////////////////////
    //
    //                      SHADER PROGRAM
    //
    ////////////////////////////////////////////////////////////////
    //create
    shaderProgramObject = gl.createProgram();
    
    //attach
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);

    //pre-link binding of shader program object with vertex shader attributes
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.KAB_ATTRIBUTE_POSITION, "vPosition");
    
    //pre-link binding of shader program object with fragment shader attributes
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.KAB_ATTRIBUTE_NORMAL, "vNormal");

    //linking
    gl.linkProgram(shaderProgramObject);
    if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS))
    {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if(error.length > 0)
        {
            alert("SHADER PROGRAM ERROR LOG : "+error);
            uninitialize();
        }
    }

    //get MVP uniform
    modelViewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_model_view_matrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");
	LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "u_L_KEY_PRESSED");
	LdUniform = gl.getUniformLocation(shaderProgramObject, "u_LD");
	KdUniform = gl.getUniformLocation(shaderProgramObject, "u_KD");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");
    
    //vertices, colors, shaders attribs, vao, vbo init
    //----------------------------------------------------------------//
    //CUBE
    var cubeVertices = new Float32Array([
                                        //FRONT
                                        1.0, 1.0, 1.0, //right-top
                                        -1.0, 1.0, 1.0, //left-top
                                        -1.0, -1.0, 1.0, //left-bottom
                                        1.0, -1.0, 1.0, //right-bottom
                                        //RIGHT
                                        1.0, 1.0, -1.0,
                                        1.0, 1.0, 1.0,
                                        1.0, -1.0, 1.0,
                                        1.0, -1.0, -1.0,
                                        //BACK
                                        -1.0, 1.0, -1.0,
                                        1.0, 1.0, -1.0,
                                        1.0, -1.0, -1.0,
                                        -1.0, -1.0, -1.0,
                                        //LEFT
                                        -1.0, 1.0, 1.0,
                                        -1.0, 1.0, -1.0,
                                        -1.0, -1.0, -1.0,
                                        -1.0, -1.0, 1.0,
                                        //TOP
                                        1.0, 1.0, -1.0,
                                        -1.0, 1.0, -1.0,
                                        -1.0, 1.0, 1.0,
                                        1.0, 1.0, 1.0,
                                        //BOTTOM
                                        1.0, -1.0, -1.0,
                                        -1.0, -1.0, -1.0,
                                        -1.0, -1.0, 1.0,
                                        1.0, -1.0, 1.0
                                        ]);

    var cubeNormals = new Float32Array([
                                        // top surface
                                        0.0, 1.0, 0.0,  // top-right of top
                                        0.0, 1.0, 0.0, // top-left of top
                                        0.0, 1.0, 0.0, // bottom-left of top
                                        0.0, 1.0, 0.0,  // bottom-right of top
                                        // bottom surface
                                        0.0, -1.0, 0.0,  // top-right of bottom
                                        0.0, -1.0, 0.0,  // top-left of bottom
                                        0.0, -1.0, 0.0,  // bottom-left of bottom
                                        0.0, -1.0, 0.0,   // bottom-right of bottom
                                        // front surface
                                        0.0, 0.0, 1.0,  // top-right of front
                                        0.0, 0.0, 1.0, // top-left of front
                                        0.0, 0.0, 1.0, // bottom-left of front
                                        0.0, 0.0, 1.0,  // bottom-right of front
                                        // back surface
                                        0.0, 0.0, -1.0,  // top-right of back
                                        0.0, 0.0, -1.0, // top-left of back
                                        0.0, 0.0, -1.0, // bottom-left of back
                                        0.0, 0.0, -1.0,  // bottom-right of back
                                        // left surface
                                        -1.0, 0.0, 0.0, // top-right of left
                                        -1.0, 0.0, 0.0, // top-left of left
                                        -1.0, 0.0, 0.0, // bottom-left of left
                                        -1.0, 0.0, 0.0, // bottom-right of left
                                        // right surface
                                        1.0, 0.0, 0.0,  // top-right of right
                                        1.0, 0.0, 0.0,  // top-left of right
                                        1.0, 0.0, 0.0,  // bottom-left of right
                                        1.0, 0.0, 0.0  // bottom-right of right
                                        ]);

    //create vao
    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);
    
    //----------------------------------------------------------------//
    //create vbo for position
    vbo_position_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_cube);
    gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    
    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //----------------------------------------------------------------//
    //create vbo for normal
    vbo_normal_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_normal_cube);
    gl.bufferData(gl.ARRAY_BUFFER, cubeNormals, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_NORMAL,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_NORMAL);
    
    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //----------------------------------------------------------------//
    //unbind vao
    gl.bindVertexArray(null);
    
    //----------------------------------------------------------------//

    //3D & Depth Code
    gl.clearDepth(1.0);	//Default value passed to the Function
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	
    //set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0); //black

    //init projection matrix
    perspectiveProjectionMatrix = mat4.create();

}

//resize()
function resize()
{
    //code
    if(bFullscreen == true)
    {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else
    {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    gl.viewport(0, 0, canvas.width, canvas.height);

    perspectiveProjectionMatrix = mat4.perspective(perspectiveProjectionMatrix,
                                                    45.0, 
                                                    parseFloat(canvas.width) / parseFloat(canvas.height),
                                                    0.1, 
                                                    100.0);


}

//display()
function display()
{
    //code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    if(isLightEnabled == 1)
    {
        gl.uniform1i(LKeyPressedUniform, 1);
        gl.uniform3f(LdUniform, 1.0, 1.0, 1.0); //white
        gl.uniform3f(KdUniform, 0.5, 0.5, 0.5); //grey
        var lightPosition = [0.0, 0.0, 2.0, 1.0];
        gl.uniform4fv(lightPositionUniform, lightPosition);
    }
    else
    {
        gl.uniform1i(LKeyPressedUniform, 0);
    }

    var translationMatrix = mat4.create();
    var scaleMatrix = mat4.create();
    var rotationMatrix = mat4.create();
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();
     
    //----------------------------------------------------------------//
    //CUBE
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    scaleMatrix = mat4.identity(scaleMatrix);
    rotationMatrix = mat4.identity(rotationMatrix);
    modelViewMatrix = mat4.identity(modelViewMatrix);
    modelViewProjectionMatrix = mat4.identity(modelViewProjectionMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -6.0]);
    scaleMatrix = mat4.scale(scaleMatrix, scaleMatrix, [0.85, 0.85, 0.85]);
    rotationMatrix = mat4.rotateX(rotationMatrix, rotationMatrix, degToRad(angleCube));
    rotationMatrix = mat4.rotateY(rotationMatrix, rotationMatrix, degToRad(angleCube));
    rotationMatrix = mat4.rotateZ(rotationMatrix, rotationMatrix, degToRad(angleCube));
    
    // MVP = T * S * R
    mat4.multiply(modelViewMatrix, translationMatrix, scaleMatrix);
    mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(modelViewMatrixUniform, false, modelViewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    gl.bindVertexArray(vao_cube);

    //drawing
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

    //unbind
    gl.bindVertexArray(null);
    
    //----------------------------------------------------------------//
    //unuse 
    gl.useProgram(null);


    ////////////////////////////////////////////////////////////////
    //
    //                      ANIMATION LOGIC
    //
    ////////////////////////////////////////////////////////////////    
    if(isCubeAnimated == 1)
    {
        angleCube = angleCube + 1.0;
        if(angleCube >= 360.0)
        {
            angleCube = 0.0;
        }
    }

    //double buffer animation loop
    requestAnimationFrame(display, canvas);
}

function degToRad(degrees)
{
    //code
    return (degrees * Math.PI / 180.0);
}

//function Fullscreen
function toggleFullscreen()
{
    //code
    var fullscreen_element = document.fullscreenElement || 
                            document.webkitFullscreenElement ||
                            document.mozFullScreenElement ||
                            document.msFullscreenElement ||
                                null;

    if(fullscreen_element == null)
    {
        //do fullscreen
        if(canvas.requestFullscreen) //check if function pointer is null or not
        {
            canvas.requestFullscreen(); //call function pointer
        }
        else if(canvas.webkitRequestFullscreen) //check if function pointer is null or not
        {
            canvas.webkitRequestFullscreen(); //call function pointer
        }
        else if(canvas.mozRequestFullScreen) //check if function pointer is null or not
        {
            canvas.mozRequestFullScreen(); //call function pointer
        }
        else if(canvas.msRequestFullscreen) //check if function pointer is null or not
        {
            canvas.msRequestFullscreen(); //call function pointer
        }

        bFullscreen = true;
    }
    else
    {
        if(document.exitFullscreen)
        {
            document.exitFullscreen();
        }
        else if(document.webkitExitFullscreen)
        {
            document.webkitExitFullscreen();
        }
        else if(document.mozCancelFullScreen)
        {
            document.mozCancelFullScreen();
        }
        else if(document.msExitFullscreen)
        {
            document.msExitFullscreen();
        }

        bFullscreen = false;

    }

}

function uninitialize()
{
    //code
    if(vao)
    {
        gl.deleteVertexArray(vao);
        vao = null;
    }

    if(vbo)
    {
        gl.deleteBuffer(vbo);
        vbo = null;
    }

    if(shaderProgramObject)
    {
        if(fragmentShaderObject)
        {
            gl.detachShader(shaderProgramObject, fragmentShaderObject);
            gl.deleteShader(fragmentShaderObject);
            fragmentShaderObject = null;
        }

        if(vertexShaderObject)
        {
            gl.detachShader(shaderProgramObject, vertexShaderObject);
            gl.deleteShader(vertexShaderObject);
            vertexShaderObject = null;
        }

        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }
}

function keyDown(event)
{
    //code
    switch(event.keyCode)
    {
        case 27:
            //escape
            uninitialize();
            //close app's tab in browser
            window.close(); // may not work in Firefox but works in other
            break;
        
        case 70:
            //'F' or 'f'
            toggleFullscreen();
            break;
        
        case 65:
            // 'A' or 'a'
            if (isCubeAnimated == 0)
            {
                isCubeAnimated = 1;
            }
            else if(isCubeAnimated == 1)
            {
                isCubeAnimated = 0;
            }
            break;
        
        case 76:
            // 'L' or 'l'
            if (isLightEnabled == 0)
            {
                isLightEnabled = 1;
            }
            else if(isLightEnabled == 1)
            {
                isLightEnabled = 0;
            }
            break;
        
    }
}

function mouseDown()
{
    //code
}
