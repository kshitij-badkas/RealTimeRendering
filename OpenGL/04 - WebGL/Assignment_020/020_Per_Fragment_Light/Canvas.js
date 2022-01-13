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

var perspectiveProjectionMatrix;

//sphere
var sphere = null;

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

//light toggle
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
    "uniform mat4 u_model_matrix;"+
    "uniform mat4 u_view_matrix;"+
    "uniform mat4 u_projection_matrix;"+
    "uniform mediump int u_L_KEY_PRESSED;"+
    "uniform vec4 u_light_position;"+
    "out vec3 transformed_normal;"+
    "out vec3 light_direction;"+
    "out vec3 view_vector;"+
    "void main(void)"+
    "{"+
    "if(u_L_KEY_PRESSED == 1)"+
    "{"+
    "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
    "transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
    "light_direction = vec3(u_light_position - eye_coordinates);"+
    //SWIZZLING
    "view_vector = -eye_coordinates.xyz;"+
    "}"+
    "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
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
    "in vec3 transformed_normal;"+
    "in vec3 light_direction;"+
    "in vec3 view_vector;"+
    "uniform int u_L_KEY_PRESSED;"+
    "uniform vec3 u_LA;"+
    "uniform vec3 u_LD;"+
    "uniform vec3 u_LS;"+
    "uniform vec3 u_KA;"+
    "uniform vec3 u_KD;"+
    "uniform vec3 u_KS;"+
    "uniform float u_material_shininess;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "vec3 phong_ads_light;"+
    "if(u_L_KEY_PRESSED == 1)"+
    "{"+
    "vec3 normalized_transformed_normal = normalize(transformed_normal);"+
    "vec3 normalized_light_direction = normalize(light_direction);"+
    "vec3 normalized_view_vector = normalize(view_vector);"+
    "vec3 ambient = u_LA * u_KA;"+
    "vec3 diffuse = u_LD * u_KD * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);"+
    "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);"+
    "vec3 specular = u_LS * u_KS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_material_shininess);"+
    "phong_ads_light = ambient + diffuse + specular;"+
    "}"+
    "else"+
    "{"+
    "phong_ads_light = vec3(1.0f,1.0f,1.0f);"+
    "}"+
    "FragColor = vec4(phong_ads_light, 1.0f);"+ 
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

    //get uniform location
    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");

	LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "u_L_KEY_PRESSED");
    LaUniform = gl.getUniformLocation(shaderProgramObject, "u_LA");
	LdUniform = gl.getUniformLocation(shaderProgramObject, "u_LD");
    LsUniform = gl.getUniformLocation(shaderProgramObject, "u_LS");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");

	KaUniform = gl.getUniformLocation(shaderProgramObject, "u_KA");
    KdUniform = gl.getUniformLocation(shaderProgramObject, "u_KD");
    KsUniform = gl.getUniformLocation(shaderProgramObject, "u_KS");
    materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

    
    //vertices, colors, shaders attribs, vao, vbo init
    //----------------------------------------------------------------//
    //SPHERE
    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);
    
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
    //variable declaration
    //light values
    var lightAmbient = [0.1, 0.1, 0.1];
    var lightDiffuse = [1.0, 1.0, 1.0];
    var lightSpecular = [1.0, 1.0, 1.0];
    var lightPosition = [100.0, 100.0, 100.0, 1.0];
    //material values
    var materialAmbient = [0.0, 0.0, 0.0];
    var materialDiffuse = [0.5, 0.2, 0.7];
    var materialSpecular = [1.0, 1.0, 1.0];
    var materialShininess = 128.0;

    //code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    //use
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
    

    var translationMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var projectionMatrix = mat4.create();
     
    //----------------------------------------------------------------//
    //SPHERE
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);
    //projectionMatrix = mat4.identity(projectionMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -6.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    sphere.draw();

    //unbind
    //Mesh.js
    
    //----------------------------------------------------------------//
    //unuse 
    gl.useProgram(null);


    ////////////////////////////////////////////////////////////////
    //
    //                      ANIMATION LOGIC
    //
    ////////////////////////////////////////////////////////////////    

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
    sphere.deallocate();

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
