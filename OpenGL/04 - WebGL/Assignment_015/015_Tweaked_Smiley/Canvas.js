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

var perspectiveProjectionMatrix;

//square
var vao_square;
var vbo_position_square;
var vbo_color_square;
var vbo_texcoord_square;

//uniforms
var mvpUniform;
var textureSamplerUniform;
var otherKeyPressedUniform;

//animation
var angleSquare = 0.0;

//textures
var smiley_texture;
var pressed_key = 0;
console.log("on load -> pressed_key = "+pressed_key);

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
    "uniform int u_other_key_pressed;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
        "vec4 color;"+
        "if(u_other_key_pressed == 1)"+
        "{"+
        "color = vec4(1.0f, 1.0f, 1.0f, 1.0f);"+
        "}"+
        "else"+
        "{"+
        "color = texture(u_texture_sampler, out_texcoord);"+
        "}"+
        "FragColor = color;"+
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
    
    //if other key is pressed
    otherKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "u_other_key_pressed");

    //vertices, colors, shaders attribs, vao, vbo init
    //----------------------------------------------------------------//
    //SQUARE
    var squareVertices = new Float32Array([
                                            1.0, 1.0, 0.0, //right-top
                                            -1.0, 1.0, 0.0, //left-top
                                            -1.0, -1.0, 0.0, //left-bottom
                                            1.0, -1.0, 0.0 //right-bottom
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
    //create vbo for texcoord
    vbo_texcoord_square = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texcoord_square);
    gl.bufferData(gl.ARRAY_BUFFER, 32, gl.DYNAMIC_DRAW);
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

    //3D & Depth Code
    gl.clearDepth(1.0);	//Default value passed to the Function
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	
    //load textures
    //----------------------------------------------------------------//
    //smiley
    smiley_texture = gl.createTexture(); //glGenTexture
    smiley_texture.image = new Image();
    smiley_texture.image.src = "smiley.png";
    //Lambda / Closure / Block / Unnamed Function :
    smiley_texture.image.onload = function()
    {
        gl.bindTexture(gl.TEXTURE_2D, smiley_texture);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true); //2nd parameter can be '1'

        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 
            gl.RGBA, gl.UNSIGNED_BYTE, 
            smiley_texture.image);
        
        //unbind
        gl.bindTexture(gl.TEXTURE_2D, null);
    }
    //----------------------------------------------------------------//

    //enble Textures
    //gl.enable(gl.TEXTURE_2D);
    
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
    var texCoord = new Float32Array(8);

    //code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    var translationMatrix = mat4.create();
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();
    
    //----------------------------------------------------------------//
    //SQUARE
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    modelViewMatrix = mat4.identity(modelViewMatrix);
    modelViewProjectionMatrix = mat4.identity(modelViewProjectionMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(modelViewMatrix, translationMatrix, [0.0, 0.0, -3.0]);
    
    mat4.multiply(modelViewMatrix, modelViewMatrix, translationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
    
    //textures
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, smiley_texture);
    gl.uniform1i(textureSamplerUniform, 0);

    //bind vao
    gl.bindVertexArray(vao_square);

    //drawing
    //other key not pressed and 0 < pressed_key < 4
    gl.uniform1i(otherKeyPressedUniform, 0);
    if (pressed_key == 1)
	{
		texCoord[0] = 1.0;
		texCoord[1] = 1.0;
		texCoord[2] = 0.0;
		texCoord[3] = 1.0;
		texCoord[4] = 0.0;
		texCoord[5] = 0.0;
		texCoord[6] = 1.0;
		texCoord[7] = 0.0;
		
	}
	else if (pressed_key == 2)
	{
		texCoord[0] = 0.5;
		texCoord[1] = 0.5;
		texCoord[2] = 0.0;
		texCoord[3] = 0.5;
		texCoord[4] = 0.0;
		texCoord[5] = 0.0;
		texCoord[6] = 0.5;
		texCoord[7] = 0.0;
	}	
    else if (pressed_key == 3)
	{
		texCoord[0] = 2.0;
		texCoord[1] = 2.0;
		texCoord[2] = 0.0;
		texCoord[3] = 2.0;
		texCoord[4] = 0.0;
		texCoord[5] = 0.0;
		texCoord[6] = 2.0;
		texCoord[7] = 0.0;
	}
	else if (pressed_key == 4)
	{
		texCoord[0] = 0.5;
		texCoord[1] = 0.5;
		texCoord[2] = 0.5;
		texCoord[3] = 0.5;
		texCoord[4] = 0.5;
		texCoord[5] = 0.5;
		texCoord[6] = 0.5;
		texCoord[7] = 0.5;
	}
    else
    {
        //other key pressed and pressed_key = 0 or other than  1, 2, 3, 4
        gl.uniform1i(otherKeyPressedUniform, 1);
    }

    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texcoord_square);
    gl.bufferData(gl.ARRAY_BUFFER, texCoord, gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_TEXCOORD,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_TEXCOORD);
    
    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);    

    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

    //unbind vao
    gl.bindVertexArray(null);
    
    //----------------------------------------------------------------//
    //unuse 
    gl.useProgram(null);

    //double buffer animation loop
    requestAnimationFrame(display, canvas);
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
    if(vao_square)
    {
        gl.deleteVertexArray(vao_square);
        vao_square = null;
    }

    if(vbo_position_square)
    {
        gl.deleteBuffer(vbo_position_square);
        vbo_position_square = null;
    }

    if(vbo_texcoord_square)
    {
        gl.deleteBuffer(vbo_texcoord_square);
        vbo_texcoord_square = null;
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
        case 49:
            //1
            pressed_key = 1;
            console.log("pressed_key = \n"+pressed_key);
            break;
        case 50:
            //2
            pressed_key = 2;
            console.log("pressed_key = \n"+pressed_key);
            break;
        case 51:
            //3
            pressed_key = 3;
            console.log("pressed_key = \n"+pressed_key);
            break;
        case 52:
            //4
            pressed_key = 4;
            console.log("pressed_key = \n"+pressed_key);
            break;        
        case 70:
            //'F' or 'f'
            toggleFullscreen();
            break;
        default:
            pressed_key = event.keyCode;
            console.log("pressed_key = \n"+pressed_key);
            break;

    }
}

function mouseDown()
{
    //code
}
