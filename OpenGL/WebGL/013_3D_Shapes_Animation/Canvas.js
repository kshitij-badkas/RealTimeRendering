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

//pyramid
var vao_pyramid;
var vbo_position_pyramid;
var vbo_color_pyramid;
//cube
var vao_cube;
var vbo_position_cube;
var vbo_color_cube;

var mvpUniform;
var perspectiveProjectionMatrix;

//animation
var anglePyramid = 0.0;
var angleCube = 0.0;
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
    "in vec4 vPosition;"+
    "in vec4 vColor;"+
    "uniform mat4 u_mvpMatrix;"+
    "out vec4 out_color;"+
    "void main(void)"+
    "{"+
    "gl_Position = u_mvpMatrix * vPosition;"+
    "out_color = vColor;"+
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
    "in vec4 out_color;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "FragColor = out_color;"+
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
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.KAB_ATTRIBUTE_COLOR, "vColor");

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
    mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvpMatrix");
    
    //vertices, colors, shaders attribs, vao, vbo init
    //----------------------------------------------------------------//
    //PYRAMID
    var pyramidVertices = new Float32Array([
                                            //FRONT
                                            0.0, 1.0, 0.0, //apex
                                            -1.0, -1.0, 1.0, //left-bottom
                                            1.0, -1.0, 1.0, //right-bottom
                                            //RIGHT
                                            0.0, 1.0, 0.0,
                                            1.0, -1.0, 1.0,
                                            1.0, -1.0, -1.0,
                                            //BACK
                                            0.0, 1.0, 0.0,
                                            1.0, -1.0, -1.0,
                                            -1.0, -1.0, -1.0,
                                            //LEFT
                                            0.0, 1.0, 0.0,
                                            -1.0, -1.0, -1.0,
                                            -1.0, -1.0, 1.0
                                            ]);

    var pyramidColors = new Float32Array([
                                            //FRONT
                                            1.0, 0.0, 0.0, //R
                                            0.0, 1.0, 0.0, //G
                                            0.0, 0.0, 1.0, //B
                                            //RIGHT
                                            1.0, 0.0, 0.0, //R
                                            0.0, 0.0, 1.0, //B
                                            0.0, 1.0, 0.0, //G
                                            //BACK
                                            1.0, 0.0, 0.0, //R
                                            0.0, 1.0, 0.0, //G
                                            0.0, 0.0, 1.0, //B
                                            //LEFT
                                            1.0, 0.0, 0.0, //R
                                            0.0, 0.0, 1.0, //B
                                            0.0, 1.0, 0.0  //G
                                            ]);

    //create vao
    vao_pyramid = gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);

    //----------------------------------------------------------------//
    //create vbo for position
    vbo_position_pyramid = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_pyramid);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);

    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //----------------------------------------------------------------//
    //create vbo for color
    vbo_color_pyramid = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_pyramid);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidColors, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);

    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //unbind vao
    gl.bindVertexArray(null);

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

    var cubeColors = new Float32Array([
                                        //FRONT
                                        1.0, 0.0, 0.0, //R
                                        1.0, 0.0, 0.0,
                                        1.0, 0.0, 0.0,
                                        1.0, 0.0, 0.0,
                                        //RIGHT
                                        0.0, 1.0, 0.0, //G
                                        0.0, 1.0, 0.0,
                                        0.0, 1.0, 0.0,
                                        0.0, 1.0, 0.0,
                                        //BACK
                                        0.0, 0.0, 1.0, //B
                                        0.0, 0.0, 1.0,
                                        0.0, 0.0, 1.0,
                                        0.0, 0.0, 1.0,
                                        //LEFT
                                        0.0, 1.0, 1.0, //C
                                        0.0, 1.0, 1.0,
                                        0.0, 1.0, 1.0,
                                        0.0, 1.0, 1.0,
                                        //TOP
                                        1.0, 0.0, 1.0, //M
                                        1.0, 0.0, 1.0,
                                        1.0, 0.0, 1.0,
                                        1.0, 0.0, 1.0,
                                        //BOTTOM
                                        1.0, 1.0, 0.0, //Y
                                        1.0, 1.0, 0.0,
                                        1.0, 1.0, 0.0,
                                        1.0, 1.0, 0.0
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
    //create vbo for color
    vbo_color_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_cube);
    gl.bufferData(gl.ARRAY_BUFFER, cubeColors, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    
    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
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

    var translationMatrix = mat4.create();
    var scaleMatrix = mat4.create();
    var rotationMatrix = mat4.create();
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();
     
    //----------------------------------------------------------------//
    //PYRAMID
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    rotationMatrix = mat4.identity(rotationMatrix);
    modelViewMatrix = mat4.identity(modelViewMatrix);
    modelViewProjectionMatrix = mat4.identity(modelViewProjectionMatrix);

    //transformation
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [-1.5, 0.0, -6.0]);
    rotationMatrix = mat4.rotateY(rotationMatrix, rotationMatrix, degToRad(anglePyramid));

    mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    //bind 
    gl.bindVertexArray(vao_pyramid);

    //drawing
    gl.drawArrays(gl.TRIANGLES, 0, 12);

    //unbind
    gl.bindVertexArray(null);
    
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
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [1.5, 0.0, -6.0]);
    scaleMatrix = mat4.scale(scaleMatrix, scaleMatrix, [0.85, 0.85, 0.85]);
    rotationMatrix = mat4.rotateX(rotationMatrix, rotationMatrix, degToRad(angleCube));
    rotationMatrix = mat4.rotateY(rotationMatrix, rotationMatrix, degToRad(angleCube));
    rotationMatrix = mat4.rotateZ(rotationMatrix, rotationMatrix, degToRad(angleCube));
    
    // MVP = T * S * R
    mat4.multiply(modelViewMatrix, translationMatrix, scaleMatrix);
    mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

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
    anglePyramid = anglePyramid + 1.0;
    angleCube = angleCube + 1.0;
    if(anglePyramid >= 360.0)
    {
        anglePyramid = 0.0;
    }
    if(angleCube >= 360.0)
    {
        angleCube = 0.0;
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
    }
}

function mouseDown()
{
    //code
}
