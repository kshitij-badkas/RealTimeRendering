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

var perspectiveProjectionMatrix;

//cube
var vao;
var vbo;

//uniform
//mvp matrix
var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;
//light
var LaUniform;
var LdUniform;
var LsUniform;
var lightPositionUniform;
//material
var KaUniform;
var KdUniform;
var KsUniform;
var materialShininessUniform;
//key pressed
var LKeyPressedUniform;

//textures
var marble_texture;
var textureSamplerUniform;

//animation
var angleCube = 0.0;

//light toggle
var isLightEnabled;

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
    "in vec4 vColor;"+
    "in vec3 vNormal;"+
    "in vec2 vTexCoord;"+
    "uniform mat4 u_model_matrix;"+
    "uniform mat4 u_view_matrix;"+
    "uniform mat4 u_projection_matrix;"+
    "uniform int u_L_KEY_PRESSED;"+
    "uniform vec3 u_LA;"+
    "uniform vec3 u_LD;"+
    "uniform vec3 u_LS;"+
    "uniform vec4 u_light_position;"+
    "uniform vec3 u_KA;"+
    "uniform vec3 u_KD;"+
    "uniform vec3 u_KS;"+
    "uniform float u_material_shininess;"+
    "out vec4 out_color;"+
    "out vec3 phong_ads_light;"+
    "out vec2 out_texcoord;"+
    "void main(void)"+
    "{"+
    "if(u_L_KEY_PRESSED == 1)"+
    "{"+
    "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
    "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
    "vec3 light_direction = normalize(vec3(u_light_position - eye_coordinates));"+
    "vec3 reflection_vector = reflect(-light_direction, transformed_normal);"+
    //SWIZZLING
    "vec3 view_vector = normalize(-eye_coordinates.xyz);"+
    "vec3 ambient = u_LA * u_KA;"+
    "vec3 diffuse = u_LD * u_KD * max(dot(light_direction, transformed_normal), 0.0);"+
    "vec3 specular = u_LS * u_KS * pow(max(dot(reflection_vector, view_vector), 0.0), u_material_shininess);"+
    "phong_ads_light = ambient + diffuse + specular;"+
    "}"+
    "else"+
    "{"+
    "phong_ads_light = vec3(1.0, 1.0, 1.0);"+
    "}"+
    "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
    "out_color = vColor;"+
    "out_texcoord = vTexCoord;"+
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
    "in vec3 phong_ads_light;"+
    "in vec2 out_texcoord;"+
    "uniform highp sampler2D u_texture_sampler;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "vec3 tex = vec3(texture(u_texture_sampler, out_texcoord));"+
    "FragColor = vec4(vec3(out_color) * phong_ads_light * tex, 1.0);"+
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

    //pre-link binding of shader program object with fragment shader attribute
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.KAB_ATTRIBUTE_COLOR, "vColor");

	//pre-link binding of shader program object with fragment shader attribute
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.KAB_ATTRIBUTE_NORMAL, "vNormal");

	//pre-link binding of shader program object with fragment shader attribute
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

    //get Uniform Location
	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
	viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
	projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");
	LaUniform = gl.getUniformLocation(shaderProgramObject, "u_LA");
	LdUniform = gl.getUniformLocation(shaderProgramObject, "u_LD");
	LsUniform = gl.getUniformLocation(shaderProgramObject, "u_LS");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");
	KaUniform = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");
	LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "u_L_KEY_PRESSED");
    
    //vertices, colors, shaders attribs, vao, vbo init
    //----------------------------------------------------------------//
    
    // INITIALIZTION OF cubePCNT WITHHOUT TYPED ARRAY (new Float32Array) DID NOT WORK - 
    // TYPED ARRAY IS REQUIRED HERE.
    // WEBGL DOES NOT ACCEPT 2D ARRAYS OR ARRAY OF ARRAYS 
    /**
     * WebGL warning: drawArraysInstanced: Vertex fetch requires 4, but attribs only supply 0.
        WebGL warning: drawArraysInstanced: Vertex fetch requires 8, but attribs only supply 0.
        WebGL warning: drawArraysInstanced: Vertex fetch requires 12, but attribs only supply 0.
        WebGL warning: drawArraysInstanced: Vertex fetch requires 16, but attribs only supply 0.
        WebGL warning: drawArraysInstanced: Vertex fetch requires 20, but attribs only supply 0.
        WebGL warning: drawArraysInstanced: Vertex fetch requires 24, but attribs only supply 0
     
        THIS ERROR OCCURS IF 'new Float32Array()' IS NOT USED HERE.

        */
    
    //CUBE
    var cubePCNT = new Float32Array([		//x,y,z, r,g,b, x,y,z, s,t
		//FRONT
		1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
		-1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
		-1.0, -1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
		1.0, -1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0,

		//RIGHT
		1.0, 1.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
		1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
		1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0,
		1.0, -1.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,

		//BACK
		-1.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0,
		1.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 1.0, 0.0,
		1.0, -1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 1.0, 1.0,
		-1.0, -1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0,

		//LEFT
		-1.0, 1.0, 1.0, 0.0, 1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0,
		-1.0, 1.0, -1.0, 0.0, 1.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0,
		-1.0, -1.0, -1.0, 0.0, 1.0, 1.0, -1.0, 0.0, 0.0, 1.0, 1.0,
		-1.0, -1.0, 1.0, 0.0, 1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 1.0,

		//TOP
		1.0, 1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0,
		-1.0, 1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0,
		-1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0,

		//BOTTOM
		1.0, -1.0, -1.0, 1.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0,
		-1.0, -1.0, -1.0, 1.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0,
		-1.0, -1.0, 1.0, 1.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 1.0,
		1.0, -1.0, 1.0, 1.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0, 1.0
    ]);

    //create vao
    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);
    
    //----------------------------------------------------------------//
    //create vbo
    vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
    gl.bufferData(gl.ARRAY_BUFFER, cubePCNT, gl.STATIC_DRAW);

    // POSITION
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            3, 
                            gl.FLOAT,
                            false, 44, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    

    // COLOR
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 44, 12);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);

    // NORMAL
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_NORMAL,
                            3, 
                            gl.FLOAT,
                            false, 44, 24);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_NORMAL);

    // TEXTURE
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_TEXCOORD,
                            2, 
                            gl.FLOAT,
                            false, 44, 36);

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
	
    //stone - pyramid
    marble_texture = gl.createTexture(); //glGenTexture
    marble_texture.image = new Image();
    marble_texture.image.src = "marble.png";
    //Lambda / Closure / Block / Unnamed Function :
    marble_texture.image.onload = function()
    {
        gl.bindTexture(gl.TEXTURE_2D, marble_texture);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true); //2nd parameter can be '1'

        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 
                        gl.RGBA, gl.UNSIGNED_BYTE, 
                        marble_texture.image);

        //unbind
        gl.bindTexture(gl.TEXTURE_2D, null);
    }

    //set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0); //black

    isLightEnabled = 0;

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
    /*
    var lightAmbient = new Float32Array([0.1, 0.1, 0.1 ,1.0]);
	var lightDiffuse = new Float32Array([1.0, 1.0, 1.0 ,1.0]);
	var lightSpecular = new Float32Array([1.0, 1.0, 1.0 ,1.0]);
	var lightPosition = new Float32Array([0.0, 0.0, 2.0, 1.0]);
	var materialAmbient = new Float32Array([0.0, 0.0, 0.0 ,1.0]);
	var materialDiffuse = new Float32Array([1.0, 1.0, 1.0 ,1.0]);
	var materialSpecular = new Float32Array([0.7, 0.7, 0.7 ,1.0]);
	var materialShininess = 50.0;
    */
    
    // IN DISPLAY() initialization of light, material properties with new FLoat32Array did not work.
    /**
     * WebGL warning: uniform setter: `values` length (4) must be a positive integer multiple of size of <enum 0x8b51>
     * 
     * THIS ERROR OCCURS IF new FLoat32Array IS USED HERE
     */

    var lightAmbient = [0.1, 0.1, 0.1];
    var lightDiffuse = [1.0, 1.0, 1.0];
    var lightSpecular = [1.0, 1.0, 1.0];
    var lightPosition = [0.0, 0.0, 2.0, 1.0];
    //material values
    var materialAmbient = [0.0, 0.0, 0.0];
    var materialDiffuse = [0.5, 0.2, 0.7];
    var materialSpecular = [1.0, 1.0, 1.0];
    var materialShininess = 50.0;


	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    gl.uniform4fv(lightPositionUniform, lightPosition);
    if(isLightEnabled == 1)
    {
        gl.uniform1i(LKeyPressedUniform, 1);
        gl.uniform3fv(LaUniform, lightAmbient);
        gl.uniform3fv(LdUniform, lightDiffuse);
        gl.uniform3fv(LsUniform, lightSpecular);
        gl.uniform3fv(KaUniform, materialAmbient);
        gl.uniform3fv(KdUniform, materialDiffuse);
        gl.uniform3fv(KsUniform, materialSpecular);
        gl.uniform1f(materialShininessUniform, materialShininess);
    }
    else
    {
        gl.uniform1i(LKeyPressedUniform, 0);
    }

    var modelMatrix = mat4.create();
    var translationMatrix = mat4.create();
    var scaleMatrix = mat4.create();
    var rotationMatrix = mat4.create();
    var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();
     
    //----------------------------------------------------------------//
    //CUBE
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    scaleMatrix = mat4.identity(scaleMatrix);
    rotationMatrix = mat4.identity(rotationMatrix);
    modelMatrix = mat4.identity(modelMatrix);
    projectionMatrix = mat4.identity(projectionMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -6.0]);
    scaleMatrix = mat4.scale(scaleMatrix, scaleMatrix, [0.85, 0.85, 0.85]);
    rotationMatrix = mat4.rotateX(rotationMatrix, rotationMatrix, degToRad(angleCube));
    rotationMatrix = mat4.rotateY(rotationMatrix, rotationMatrix, degToRad(angleCube));
    rotationMatrix = mat4.rotateZ(rotationMatrix, rotationMatrix, degToRad(angleCube));
    
    // MVP = T * S * R
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    mat4.multiply(modelMatrix, modelMatrix, rotationMatrix);
    //mat4.multiply(projectionMatrix, perspectiveProjectionMatrix, modelMatrix);

    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //textures
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, marble_texture);
    gl.uniform1i(textureSamplerUniform, 0);

    gl.bindVertexArray(vao);

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
        
        case 76:
            //'L' or 'l'
            if(isLightEnabled == 0)
            {
                isLightEnabled = 1;
                console.log("LIGHT IS : "+ isLightEnabled);
            }
            else if(isLightEnabled == 1)
            {
                isLightEnabled = 0;
                console.log("LIGHT IS : "+ isLightEnabled);
            }
            break;
    }
}

function mouseDown()
{
    //code
}
