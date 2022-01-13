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

//triangle
var vao_triangle;
var vbo_position_triangle;
var vbo_color_triangle;
//square
var vao_square;
var vbo_position_square;
var vbo_color_square;

var mvpUniform;
var perspectiveProjectionMatrix;

//animation
var angleTriangle = 0.0;
var angleSquare = 0.0;
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
    //TRIANGLE
    var triangleVertices = new Float32Array([
                                            0.0, 1.0, 0.0, //apex
                                            -1.0, -1.0, 0.0, //left-bottom
                                            1.0, -1.0, 0.0 //right-bottom
                                            ]);

    var triangleColors = new Float32Array([
                                            1.0, 0.0, 0.0, //red
                                            0.0, 1.0, 0.0, //green
                                            0.0, 0.0, 1.0 //blue
                                            ]);

    //create vao
    vao_triangle = gl.createVertexArray();
    gl.bindVertexArray(vao_triangle);

    //----------------------------------------------------------------//
    //create vbo for position
    vbo_position_triangle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_triangle);
    gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);

    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //----------------------------------------------------------------//
    //create vbo for color
    vbo_color_triangle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_triangle);
    gl.bufferData(gl.ARRAY_BUFFER, triangleColors, gl.STATIC_DRAW);
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
    //SQUARE
    var squareVertices = new Float32Array([
                                            1.0, 1.0, 0.0, //right-top
                                            -1.0, 1.0, 0.0, //left-top
                                            -1.0, -1.0, 0.0, //left-bottom
                                            1.0, -1.0, 0.0 //right-bottom
                                            ]);

    var squareColors = new Float32Array([
                                            0.0, 0.0, 1.0, //blue
                                            0.0, 0.0, 1.0, //blue
                                            0.0, 0.0, 1.0, //blue
                                            0.0, 0.0, 1.0 //blue
                                            ]);

    //create vao
    vao_square = gl.createVertexArray();
    gl.bindVertexArray(vao_square);
    
    //----------------------------------------------------------------//
    //create vbo for position
    vbo_position_square = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_square);
    gl.bufferData(gl.ARRAY_BUFFER, squareVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    
    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //----------------------------------------------------------------//
    //create vbo for color
    vbo_color_square = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_square);
    gl.bufferData(gl.ARRAY_BUFFER, squareColors, gl.STATIC_DRAW);
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
    var rotationMatrix = mat4.create();
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();
     
    //----------------------------------------------------------------//
    //TRIANGLE
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    rotationMatrix = mat4.identity(rotationMatrix);
    modelViewMatrix = mat4.identity(modelViewMatrix);
    modelViewProjectionMatrix = mat4.identity(modelViewProjectionMatrix);

    //transformation
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [-1.5, 0.0, -4.0]);
    rotationMatrix = mat4.rotateY(rotationMatrix, rotationMatrix, degToRad(angleTriangle));

    mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    //bind 
    gl.bindVertexArray(vao_triangle);

    //drawing
    gl.drawArrays(gl.TRIANGLES, 0, 3);

    //unbind
    gl.bindVertexArray(null);
    
    //----------------------------------------------------------------//
    //SQUARE
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    rotationMatrix = mat4.identity(rotationMatrix);
    modelViewMatrix = mat4.identity(modelViewMatrix);
    modelViewProjectionMatrix = mat4.identity(modelViewProjectionMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(modelViewMatrix, translationMatrix, [1.5, 0.0, -4.0]);
    rotationMatrix = mat4.rotateX(rotationMatrix, rotationMatrix, degToRad(angleSquare));
    
    mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vao_square);

    //drawing
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

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
    angleTriangle = angleTriangle + 1.0;
    angleSquare = angleSquare + 1.0;
    if(angleTriangle >= 360.0)
    {
        angleTriangle = 0.0;
    }
    if(angleSquare >= 360.0)
    {
        angleSquare = 0.0;
    }

    //if needed, call update() here

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
