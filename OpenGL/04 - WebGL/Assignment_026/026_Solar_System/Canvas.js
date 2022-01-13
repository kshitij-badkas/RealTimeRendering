// STEPS FOR DRAWING SPHERE -
//  1. globally var sphere = null; 
//  2. in init() - sphere = new Mesh(); //hidden constructor
//  3. in init() - below new Mesh(); call -> makeSphere(sphere, 2.0, 30, 30);
//  4. draw() - sphere.draw();
//  5. uniniti() - sphere.deallocate();

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

//sphere
var sun = null;
var earth = null;
var moon = null;

//uniform
var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

//colour uniforms
var colorUniform;

//animation
var day = 0;
var year = 0;
var moonDay = 0;

//push-pop
var stackMatrix = new Array();

//Matrices
var perspectiveProjectionMatrix;
var modelMatrix;
var viewMatrix;

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
    "uniform mat4 u_model_matrix;"+
    "uniform mat4 u_view_matrix;"+
    "uniform mat4 u_projection_matrix;"+
    "void main(void)"+
    "{"+
    "gl_Position =  u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
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
    "out vec4 FragColor;"+
    "uniform vec4 u_Color;"+
    "void main(void)"+
    "{"+
    "FragColor = u_Color;"+
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
    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");
    colorUniform = gl.getUniformLocation(shaderProgramObject, "u_Color");



    //vertices, colors, shaders attribs, vao, vbo init
    //----------------------------------------------------------------//
    //SPHERE
    //SUN
    sun = new Mesh();
    makeSphere(sun, 0.75, 30, 30);

    //EARTH
    earth = new Mesh();
    makeSphere(earth, 0.2, 30, 30);

    //MOON
    moon = new Mesh();
    makeSphere(moon, 0.05, 30, 30);

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
    //variables
    //colours
    var sunColor = new Float32Array([1.0, 1.0, 0.0, 1.0]);
    var earthColor = new Float32Array([0.0, 1.0, 1.0, 1.0]);
    var moonColor = new Float32Array([0.8, 0.8, 0.8, 1.0]);
    
    //code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);
 
    modelMatrix = mat4.create();
    viewMatrix = mat4.create();

    //----------------------------------------------------------------//
    
    //identity
    modelMatrix = mat4.identity(modelMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -5.0]);
    pushMatrix();
    
    //SUN
    gl.uniform4fv(colorUniform, sunColor);
    mat4.rotateX(modelMatrix, modelMatrix, degToRad(90.0));
 
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    sun.draw();

    popMatrix();

    pushMatrix();

    //unbind
    //Mesh.js
    //----------------------------------------------------------------//

    //----------------------------------------------------------------//
    
    //SPHERE
    //EARTH
    gl.uniform4fv(colorUniform, earthColor);              
    mat4.rotateY(modelMatrix, modelMatrix, degToRad(year));
    mat4.translate(modelMatrix, modelMatrix, [1.5, 0.0, 0.0]);
    mat4.rotateX(modelMatrix, modelMatrix, degToRad(90.0));
    mat4.rotateZ(modelMatrix, modelMatrix, degToRad(day));
    
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    earth.draw();

    pushMatrix();
        
    //----------------------------------------------------------------//

        //----------------------------------------------------------------//
    
    //SPHERE
    //MOON
    gl.uniform4fv(colorUniform, moonColor);
    mat4.rotateX(modelMatrix, modelMatrix, degToRad(180.0));
    mat4.rotateZ(modelMatrix, modelMatrix, degToRad(moonDay));

    mat4.translate(modelMatrix, modelMatrix, [0.4, 0.0, 0.0]);
    
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    moon.draw();

    popMatrix();

    popMatrix();
        
    //----------------------------------------------------------------//

    //unuse 
    gl.useProgram(null);


    ////////////////////////////////////////////////////////////////
    //
    //                      ANIMATION LOGIC
    //
    ////////////////////////////////////////////////////////////////    
    day = (day + 6) % 360;
    year = (year + 1) % 360;
	moonDay = (moonDay + 3) % 360;

    //double buffer animation loop
    requestAnimationFrame(display, canvas);
}

function pushMatrix()
{
    //code
    var tempMatrix = modelMatrix.slice(0);
    stackMatrix.push(tempMatrix);
    //stackMatrix.push(modelMatrix);
}

function popMatrix()
{
    //code
    modelMatrix = stackMatrix.pop();
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
    shoulder.deallocate();
    elbow.deallocate();

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
        
        case 83: //Shoulder (S)
            angleShoulder = (angleShoulder + 3) % 360;
            break;

        case 69: //Shoulder (E)
            angleElbow = (angleElbow + 3) % 360;
            break;

    }
}

function mouseDown()
{
    //code
}
