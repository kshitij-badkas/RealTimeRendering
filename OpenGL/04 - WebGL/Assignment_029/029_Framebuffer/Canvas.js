////////////////////////////////////////////////////////////////////////////////////////
//  NOTE : FOR FIREFORX - about:config -> security.fileuri.strict_origin_policy = false
//                      - about:config -> content.cors.disable = true
//                          
//         FOR CHROME - command line arguments
////////////////////////////////////////////////////////////////////////////////////////

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

var vertexShaderObject_2;
var fragmentShaderObject_2;
var shaderProgramObject_2;

var perspectiveProjectionMatrix;

//cube
var vao_cube;
var vbo_position_cube;
var vbo_texcoord_cube;

var vao_cube_2;
var vbo_position_cube_2;
var vbo_color_cube_2;

//uniforms
var mvpUniform;
var mvpUniform_2;

var textureSamplerUniform;

//animation
var angleCube = 0.0;

//textures
var kundali_texture;

//FRAMEBUFFER
var fbo;
var color_texture;
var depth_texture;

var width;
var height;

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
    "in vec2 vTexCoord;"+
    "uniform mat4 u_mvpMatrix;"+
    "out vec2 out_texcoord;"+
    "void main(void)"+
    "{"+
    "gl_Position = u_mvpMatrix * vPosition;"+
    "out_texcoord = vTexCoord;"+
    "}"

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
    "in vec2 out_texcoord;"+
    "uniform highp sampler2D u_texture_sampler;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "FragColor = texture(u_texture_sampler, out_texcoord);"+
    "}"

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
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.KAB_ATTRIBUTE_TEXCOORD, "vTexCoord");

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

    //get Texture Sampler Uniform 
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "u_texture_sampler");
    

    //2nd cube
    ////////////////////////////////////////////////////////////////
    //
    //                      VERTEX SHADER
    //
    ////////////////////////////////////////////////////////////////
    var vertexShaderSourceCode_2 = 
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

    vertexShaderObject_2 = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject_2, vertexShaderSourceCode_2);
    gl.compileShader(vertexShaderObject_2);
    if(gl.getShaderParameter(vertexShaderObject_2, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(vertexShaderObject_2);
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
    var fragmentShaderSourceCode_2 = 
    "#version 300 es"+
    "\n"+
    "precision highp float;"+
    "in vec4 out_color;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "FragColor = out_color;"+
    "}";

    fragmentShaderObject_2 = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject_2, fragmentShaderSourceCode_2);
    gl.compileShader(fragmentShaderObject_2);
    if(gl.getShaderParameter(fragmentShaderObject_2, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(fragmentShaderObject_2);
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
    shaderProgramObject_2 = gl.createProgram();
    
    //attach
    gl.attachShader(shaderProgramObject_2, vertexShaderObject_2);
    gl.attachShader(shaderProgramObject_2, fragmentShaderObject_2);

    //pre-link binding of shader program object with vertex shader attributes
    gl.bindAttribLocation(shaderProgramObject_2, WebGLMacros.KAB_ATTRIBUTE_POSITION, "vPosition");
    
    //pre-link binding of shader program object with fragment shader attributes
    gl.bindAttribLocation(shaderProgramObject_2, WebGLMacros.KAB_ATTRIBUTE_COLOR, "vColor");

    //linking
    gl.linkProgram(shaderProgramObject_2);
    if(!gl.getProgramParameter(shaderProgramObject_2, gl.LINK_STATUS))
    {
        var error = gl.getProgramInfoLog(shaderProgramObject_2);
        if(error.length > 0)
        {
            alert("SHADER PROGRAM ERROR LOG : "+error);
            uninitialize();
        }
    }

    //get MVP uniform
    mvpUniform_2 = gl.getUniformLocation(shaderProgramObject_2, "u_mvpMatrix");



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

    var cubeTexCoords = new Float32Array([
                                        //FRONT
                                        0.0, 0.0,
                                        1.0, 0.0,
                                        1.0, 1.0,
                                        0.0, 1.0,
                                        //RIGHT
                                        0.0, 0.0,
                                        1.0, 0.0,
                                        1.0, 1.0,
                                        0.0, 1.0,
                                        //BACK
                                        0.0, 0.0,
                                        1.0, 0.0,
                                        1.0, 1.0,
                                        0.0, 1.0,
                                        //LEFT
                                        0.0, 0.0,
                                        1.0, 0.0,
                                        1.0, 1.0,
                                        0.0, 1.0,
                                        //TOP
                                        0.0, 0.0,
                                        1.0, 0.0,
                                        1.0, 1.0,
                                        0.0, 1.0,
                                        //BOTTOM
                                        0.0, 0.0,
                                        1.0, 0.0,
                                        1.0, 1.0,
                                        0.0, 1.0
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
    //create vbo for texcoord
    vbo_texcoord_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texcoord_cube);
    gl.bufferData(gl.ARRAY_BUFFER, cubeTexCoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_TEXCOORD,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_TEXCOORD);
    
    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //unbind vao
    gl.bindVertexArray(null);
    
    //----------------------------------------------------------------//

    //create vao - 2nd cube
    vao_cube_2 = gl.createVertexArray();
    gl.bindVertexArray(vao_cube_2);
    
    //----------------------------------------------------------------//
    //create vbo for position
    vbo_position_cube_2 = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_cube_2);
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
    vbo_color_cube_2 = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_cube_2);
    gl.bufferData(gl.ARRAY_BUFFER, cubeColors, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    
    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //----------------------------------------------------------------//

    //3D & Depth Code
    gl.clearDepth(1.0);	//Default value passed to the Function
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	
    //load textures
    //----------------------------------------------------------------//
    //kundali - cube
    kundali_texture = gl.createTexture(); //glGenTexture
    kundali_texture.image = new Image();
    kundali_texture.image.src = "Vijay_Kundali.png";
    //Lambda / Closure / Block / Unnamed Function :
    kundali_texture.image.onload = function()
    {
        gl.bindTexture(gl.TEXTURE_2D, kundali_texture);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true); //2nd parameter can be '1'

        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 
            gl.RGBA, gl.UNSIGNED_BYTE, 
            kundali_texture.image);
        
        //unbind
        gl.bindTexture(gl.TEXTURE_2D, null);
    }
    //----------------------------------------------------------------//

    
    //----------------------------------------------------------------//
    //FRAMBUFFER
    fbo = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
    
    color_texture = gl.createTexture(); //glGenTexture
    gl.bindTexture(gl.TEXTURE_2D, color_texture);
    //gl.texStorage2D(gl.TEXTURE_2D, 9, gl.RGBA8, 512, 512);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 512, 512, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);

    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);

    //depth_texture = gl.createTexture(); //glGenTexture
    //gl.bindTexture(gl.TEXTURE_2D, depth_texture);
    //gl.texStorage2D(gl.TEXTURE_2D, 9, gl.DEPTH_COMPONENT32F, 512, 512);
    depth_texture = gl.createRenderbuffer();
    gl.bindRenderbuffer(gl.RENDERBUFFER, depth_texture);
    gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, 512, 512);

    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, color_texture, 0);
    //gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.TEXTURE_2D, depth_texture, 0);
    gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, depth_texture);

    //gl.drawBuffers([gl.COLOR_ATTACHMENT0]);

    //gl.bindFramebuffer(gl.FRAMEBUFFER, null);

    //error check
    var e = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
    if(e == gl.FRAMEBUFFER_COMPLETE)
    {
        console.log("FBO IS COMPLETE "+ e.toString());
    }
    else
    {
        console.log("FBO IS 'INCOMPLETE' "+ e.toString());
    }
    //----------------------------------------------------------------//
    

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
	var green = new Float32Array([ 0.0, 0.5, 0.0, 1.0]);
	var blue = new Float32Array([ 0.0, 0.0, 0.3, 1.0]);
	var one = 1.0;

    //code
	//gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

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
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -5.0]);
    scaleMatrix = mat4.scale(scaleMatrix, scaleMatrix, [0.85, 0.85, 0.85]);
    rotationMatrix = mat4.rotateX(rotationMatrix, rotationMatrix, degToRad(angleCube));
    rotationMatrix = mat4.rotateY(rotationMatrix, rotationMatrix, degToRad(angleCube));
    rotationMatrix = mat4.rotateZ(rotationMatrix, rotationMatrix, degToRad(angleCube));
    
    // MVP = T * S * R
    mat4.multiply(modelViewMatrix, translationMatrix, scaleMatrix);
    mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    //FRAMEBUFFER
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

    gl.viewport(0, 0, 512, 512);
    //gl.clearBufferfv(gl.COLOR, 0, blue);
    //gl.clearBufferfv(gl.DEPTH, 0, [ 0.0, 0.0, 0.3, 1.0]);
    gl.clearColor(0.0, 0.5, 0.0, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject_2);
    gl.uniformMatrix4fv(mvpUniform_2, false, modelViewProjectionMatrix);
    gl.bindVertexArray(vao_cube_2);
    
    //drawing
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

    gl.bindFramebuffer(gl.FRAMEBUFFER, null);


    /////////////////////
    gl.useProgram(null);

    gl.viewport(0, 0, canvas.width, canvas.height);
    //gl.clearBufferfv(gl.COLOR, 0, green);
    //gl.clearBufferfv(gl.DEPTH, 0, [ 0.0, 0.0, 0.3, 1.0]);
    gl.clearColor(0.0, 0.0, 0.4, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    //textures
    gl.activeTexture(gl.TEXTURE0);
    //gl.bindTexture(gl.TEXTURE_2D, kundali_texture);
    gl.bindTexture(gl.TEXTURE_2D, color_texture);
    //gl.uniform1i(textureSamplerUniform, 0);

    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
    gl.bindVertexArray(vao_cube);

    //drawing
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

    gl.bindTexture(gl.TEXTURE_2D, null);
    
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
    angleCube = angleCube + 1.0;
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
