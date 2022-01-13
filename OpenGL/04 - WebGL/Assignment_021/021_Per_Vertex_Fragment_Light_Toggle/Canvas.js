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

var perspectiveProjectionMatrix;

//sphere
var sphere = null;

//----------------------------------------------//
//PER-VERTEX LIGHTING 
//shader objects
var vertexShaderObject_PV;
var fragmentShaderObject_PV;
var shaderProgramObject_PV;
//uniform
//mvp matrix
var modelMatrixUniform_PV;
var viewMatrixUniform_PV;
var projectionMatrixUniform_PV;
//light
var LaUniform_PV;
var LdUniform_PV;
var LsUniform_PV;
var lightPositionUniform_PV;
//material
var KaUniform_PV;
var KdUniform_PV;
var KsUniform_PV;
var materialShininessUniform_PV;
//key pressed
var LKeyPressedUniform_PV;
//----------------------------------------------//

//----------------------------------------------//
//PER-FRAGMENT LIGHTING
//shader objects
var vertexShaderObject_PF;
var fragmentShaderObject_PF;
var shaderProgramObject_PF;
//uniform
//mvp matrix
var modelMatrixUniform_PF;
var viewMatrixUniform_PF;
var projectionMatrixUniform_PF;
//light
var LaUniform_PF;
var LdUniform_PF;
var LsUniform_PF;
var lightPositionUniform_PF;
//material
var KaUniform_PF;
var KdUniform_PF;
var KsUniform_PF;
var materialShininessUniform_PF;
//key pressed
var LKeyPressedUniform_PF;
//----------------------------------------------//

//light toggle
var isLightEnabled = 0;
var VKeyIsPressed = 0;
var FKeyIsPressed = 0;

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

    //per-vretex 
    getVertexShader_PV();
    getFragmentShader_PV();
    getShaderProgramObject_PV();
    getUniformLocation_PV();

    //per-fragment
    getVertexShader_PF();
    getFragmentShader_PF();
    getShaderProgramObject_PF();
    getUniformLocation_PF();

    
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

function getVertexShader_PV()
{
    //code
    ////////////////////////////////////////////////////////////////
    //
    //                PER-VERTEX VERTEX SHADER
    //
    ////////////////////////////////////////////////////////////////
    var vertexShaderSourceCode_PV = 
    "#version 300 es"+
    "\n"+
    "in vec4 vPosition;"+
    "in vec3 vNormal;"+
    "uniform mat4 u_model_matrix;"+
    "uniform mat4 u_view_matrix;"+
    "uniform mat4 u_projection_matrix;"+
    "uniform int u_L_KEY_PRESSED;"+
    "uniform vec3 u_LA_PV;"+
    "uniform vec3 u_LD_PV;"+
    "uniform vec3 u_LS_PV;"+
    "uniform vec4 u_light_position_PV;"+
    "uniform vec3 u_KA_PV;"+
    "uniform vec3 u_KD_PV;"+
    "uniform vec3 u_KS_PV;"+
    "uniform float u_material_shininess_PV;"+
    "out vec3 phong_ads_light_PV;"+
    "void main(void)"+
    "{"+
        "if(u_L_KEY_PRESSED == 1)"+
        "{"+
            "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
            "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
            "vec3 light_direction = normalize(vec3(u_light_position_PV - eye_coordinates));"+
            "vec3 reflection_vector = reflect(-light_direction, transformed_normal);"+
            //SWIZZLING
            "vec3 view_vector = normalize(-eye_coordinates.xyz);"+
            "vec3 ambient_PV = u_LA_PV * u_KA_PV;"+
            "vec3 diffuse_PV = u_LD_PV * u_KD_PV * max(dot(light_direction, transformed_normal), 0.0f);"+
            "vec3 specular_PV = u_LS_PV * u_KS_PV * pow(max(dot(reflection_vector, view_vector), 0.0f), u_material_shininess_PV);"+
            "phong_ads_light_PV = ambient_PV + diffuse_PV + specular_PV;"+
        "}"+
        "else"+
        "{"+
            "phong_ads_light_PV = vec3(1.0f, 1.0f, 1.0f);"+
        "}"+
        "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
    "}";

    vertexShaderObject_PV = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject_PV, vertexShaderSourceCode_PV);
    gl.compileShader(vertexShaderObject_PV);
    if(gl.getShaderParameter(vertexShaderObject_PV, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(vertexShaderObject_PV);
        if(error.length > 0)
        {
            alert("PER-VERTEX---> VERTEX SHADER ERROR LOG : "+error);
            uninitialize();
        }
    }
}

function getFragmentShader_PV()
{
    //code
    ////////////////////////////////////////////////////////////////
    //
    //                PER-VERTEX FRAGMENT SHADER
    //
    ////////////////////////////////////////////////////////////////
    var fragmentShaderSourceCode_PV = 
    "#version 300 es"+
    "\n"+
    "precision highp float;"+
    "in vec3 phong_ads_light_PV;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "FragColor = vec4(phong_ads_light_PV, 1.0f);"+
    "}";

    fragmentShaderObject_PV = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject_PV, fragmentShaderSourceCode_PV);
    gl.compileShader(fragmentShaderObject_PV);
    if(gl.getShaderParameter(fragmentShaderObject_PV, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(fragmentShaderObject_PV);
        if(error.length > 0)
        {
            alert("PER-VERTEX---> FRAGMENT SHADER ERROR LOG : "+error);
            uninitialize();
        }
    }
}

function getShaderProgramObject_PV()
{
    //code
    ////////////////////////////////////////////////////////////////
    //
    //                PER-VERTEX SHADER PROGRAM
    //
    ////////////////////////////////////////////////////////////////
    //create
    shaderProgramObject_PV = gl.createProgram();
    
    //attach
    gl.attachShader(shaderProgramObject_PV, vertexShaderObject_PV);
    gl.attachShader(shaderProgramObject_PV, fragmentShaderObject_PV);

    //pre-link binding of shader program object with vertex shader attributes
    gl.bindAttribLocation(shaderProgramObject_PV, WebGLMacros.KAB_ATTRIBUTE_POSITION, "vPosition");
    
    //pre-link binding of shader program object with fragment shader attributes
    gl.bindAttribLocation(shaderProgramObject_PV, WebGLMacros.KAB_ATTRIBUTE_NORMAL, "vNormal");

    //linking
    gl.linkProgram(shaderProgramObject_PV);
    if(!gl.getProgramParameter(shaderProgramObject_PV, gl.LINK_STATUS))
    {
        var error = gl.getProgramInfoLog(shaderProgramObject_PV);
        if(error.length > 0)
        {
            alert("PER-VERTEX---> SHADER PROGRAM ERROR LOG : "+error);
            uninitialize();
        }
    }
}

function getUniformLocation_PV()
{
    //code
    //get uniform location
    modelMatrixUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_model_matrix");
    viewMatrixUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_view_matrix");
    projectionMatrixUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_projection_matrix");

	LKeyPressedUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_L_KEY_PRESSED");
    LaUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_LA_PV");
	LdUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_LD_PV");
    LsUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_LS_PV");
    lightPositionUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_light_position_PV");

	KaUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_KA_PV");
    KdUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_KD_PV");
    KsUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_KS_PV");
    materialShininessUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_material_shininess_PV");
}

function getVertexShader_PF()
{
    //code
    ////////////////////////////////////////////////////////////////
    //
    //                PER-FRAGMENT - VERTEX SHADER
    //
    ////////////////////////////////////////////////////////////////
    var vertexShaderSourceCode_PF = 
    "#version 300 es"+
    "\n"+
    "precision mediump float;"+
    "in vec4 vPosition;"+
    "in vec3 vNormal;"+
    "uniform mat4 u_model_matrix;"+
    "uniform mat4 u_view_matrix;"+
    "uniform mat4 u_projection_matrix;"+
    "uniform mediump int u_L_KEY_PRESSED;"+
    "uniform vec4 u_light_position_PF;"+
    "out vec3 transformed_normal;"+
    "out vec3 light_direction;"+
    "out vec3 view_vector;"+
    "void main(void)"+
    "{"+
    "if(u_L_KEY_PRESSED == 1)"+
    "{"+
    "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
    "transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
    "light_direction = vec3(u_light_position_PF - eye_coordinates);"+
    //SWIZZLING
    "view_vector = -eye_coordinates.xyz;"+
    "}"+
    "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
    "}";

    vertexShaderObject_PF = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject_PF, vertexShaderSourceCode_PF);
    gl.compileShader(vertexShaderObject_PF);
    if(gl.getShaderParameter(vertexShaderObject_PF, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(vertexShaderObject_PF);
        if(error.length > 0)
        {
            alert("PER-FRAGMENT---> VERTEX SHADER ERROR LOG : "+error);
            uninitialize();
        }
    }
}

function getFragmentShader_PF()
{
    //code
    ////////////////////////////////////////////////////////////////
    //
    //                 PER-FRAGMENT FRAGMENT SHADER
    //
    ////////////////////////////////////////////////////////////////
    var fragmentShaderSourceCode_PF = 
    "#version 300 es"+
    "\n"+
    "precision highp float;"+
    "in vec3 transformed_normal;"+
    "in vec3 light_direction;"+
    "in vec3 view_vector;"+
    "uniform int u_L_KEY_PRESSED;"+
    "uniform vec3 u_LA_PF;"+
    "uniform vec3 u_LD_PF;"+
    "uniform vec3 u_LS_PF;"+
    "uniform vec3 u_KA_PF;"+
    "uniform vec3 u_KD_PF;"+
    "uniform vec3 u_KS_PF;"+
    "uniform float u_material_shininess_PF;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "vec3 phong_ads_light_PF;"+
    "if(u_L_KEY_PRESSED == 1)"+
    "{"+
    "vec3 normalized_transformed_normal = normalize(transformed_normal);"+
    "vec3 normalized_light_direction = normalize(light_direction);"+
    "vec3 normalized_view_vector = normalize(view_vector);"+
    "vec3 ambient_PF = u_LA_PF * u_KA_PF;"+
    "vec3 diffuse_PF = u_LD_PF * u_KD_PF * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);"+
    "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);"+
    "vec3 specular_PF = u_LS_PF * u_KS_PF * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_material_shininess_PF);"+
    "phong_ads_light_PF = ambient_PF + diffuse_PF + specular_PF;"+
    "}"+
    "else"+
    "{"+
    "phong_ads_light_PF = vec3(1.0f,1.0f,1.0f);"+
    "}"+
    "FragColor = vec4(phong_ads_light_PF, 1.0f);"+ 
    "}";

    fragmentShaderObject_PF = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject_PF, fragmentShaderSourceCode_PF);
    gl.compileShader(fragmentShaderObject_PF);
    if(gl.getShaderParameter(fragmentShaderObject_PF, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(fragmentShaderObject_PF);
        if(error.length > 0)
        {
            alert("PER-FRAGMENT--> FRAGMENT SHADER ERROR LOG : "+error);
            uninitialize();
        }
    }
}

function getShaderProgramObject_PF()
{
    //code
    ////////////////////////////////////////////////////////////////
    //
    //                PER-FRAGMENT SHADER PROGRAM
    //
    ////////////////////////////////////////////////////////////////
    //create
    shaderProgramObject_PF = gl.createProgram();
    
    //attach
    gl.attachShader(shaderProgramObject_PF, vertexShaderObject_PF);
    gl.attachShader(shaderProgramObject_PF, fragmentShaderObject_PF);

    //pre-link binding of shader program object with vertex shader attributes
    gl.bindAttribLocation(shaderProgramObject_PF, WebGLMacros.KAB_ATTRIBUTE_POSITION, "vPosition");
    
    //pre-link binding of shader program object with fragment shader attributes
    gl.bindAttribLocation(shaderProgramObject_PF, WebGLMacros.KAB_ATTRIBUTE_NORMAL, "vNormal");

    //linking
    gl.linkProgram(shaderProgramObject_PF);
    if(!gl.getProgramParameter(shaderProgramObject_PF, gl.LINK_STATUS))
    {
        var error = gl.getProgramInfoLog(shaderProgramObject_PF);
        if(error.length > 0)
        {
            alert("PER-FRAGMENT---> SHADER PROGRAM ERROR LOG : "+error);
            uninitialize();
        }
    }
}

function getUniformLocation_PF()
{
    //code
    //get uniform location
    modelMatrixUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_model_matrix");
    viewMatrixUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_view_matrix");
    projectionMatrixUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_projection_matrix");

	LKeyPressedUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_L_KEY_PRESSED");
    LaUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_LA_PF");
	LdUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_LD_PF");
    LsUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_LS_PF");
    lightPositionUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_light_position_PF");

	KaUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_KA_PF");
    KdUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_KD_PF");
    KsUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_KS_PF");
    materialShininessUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_material_shininess_PF");
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

    //
    var translationMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var projectionMatrix = mat4.create();

    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);
    projectionMatrix = mat4.identity(projectionMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -6.0]);
    mat4.multiply(projectionMatrix, projectionMatrix, perspectiveProjectionMatrix);

    //use
    gl.useProgram(shaderProgramObject_PV); //PER-VERTEX BY DEFAULT
    if(VKeyIsPressed == 1)
    {
        gl.uniform4fv(lightPositionUniform_PV, lightPosition);
        if(isLightEnabled == 1)
        {
            gl.uniform1i(LKeyPressedUniform_PV, 1);
            gl.uniform3fv(LaUniform_PV, lightAmbient);
            gl.uniform3fv(LdUniform_PV, lightDiffuse);
            gl.uniform3fv(LsUniform_PV, lightSpecular);
            gl.uniform3fv(KaUniform_PV, materialAmbient);
            gl.uniform3fv(KdUniform_PV, materialDiffuse);
            gl.uniform3fv(KsUniform_PV, materialSpecular);
            gl.uniform1f(materialShininessUniform_PV, materialShininess);
        }
        else
        {
            gl.uniform1i(LKeyPressedUniform_PV, 0);
        }
    
        gl.uniformMatrix4fv(modelMatrixUniform_PV, false, translationMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform_PV, false, viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform_PV, false, projectionMatrix);
    }
    else if(FKeyIsPressed == 1)
    {
        gl.useProgram(shaderProgramObject_PF);
        gl.uniform4fv(lightPositionUniform_PF, lightPosition);
        if(isLightEnabled == 1)
        {
            gl.uniform1i(LKeyPressedUniform_PF, 1);
            gl.uniform3fv(LaUniform_PF, lightAmbient);
            gl.uniform3fv(LdUniform_PF, lightDiffuse);
            gl.uniform3fv(LsUniform_PF, lightSpecular);
            gl.uniform3fv(KaUniform_PF, materialAmbient);
            gl.uniform3fv(KdUniform_PF, materialDiffuse);
            gl.uniform3fv(KsUniform_PF, materialSpecular);
            gl.uniform1f(materialShininessUniform_PF, materialShininess);
        }
        else
        {
            gl.uniform1i(LKeyPressedUniform_PF, 0);
        }
    
        gl.uniformMatrix4fv(modelMatrixUniform_PF, false, translationMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform_PF, false, viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform_PF, false, projectionMatrix);
    }
    else
    {
        //PER-VERTEX SHOULD BE ENABLED BY DEFAULT
        gl.uniformMatrix4fv(modelMatrixUniform_PV, false, translationMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform_PV, false, viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform_PV, false, projectionMatrix);
    }

    //bind vao 
    //Mesh.js

    //drawing
    sphere.draw();

    //unbind vao
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

    //PER-VERTEX
    if(shaderProgramObject_PV)
    {
        if(fragmentShaderObject_PV)
        {
            gl.detachShader(shaderProgramObject_PV, fragmentShaderObject_PV);
            gl.deleteShader(fragmentShaderObject_PV);
            fragmentShaderObject_PV = null;
        }

        if(vertexShaderObject_PV)
        {
            gl.detachShader(shaderProgramObject_PV, vertexShaderObject_PV);
            gl.deleteShader(vertexShaderObject_PV);
            vertexShaderObject_PV = null;
        }

        gl.deleteProgram(shaderProgramObject_PV);
        shaderProgramObject_PV = null;
    }

    //PER-FRAGMENT
    if(shaderProgramObject_PF)
    {
        if(fragmentShaderObject_PF)
        {
            gl.detachShader(shaderProgramObject_PF, fragmentShaderObject_PF);
            gl.deleteShader(fragmentShaderObject_PF);
            fragmentShaderObject_PF = null;
        }

        if(vertexShaderObject_PF)
        {
            gl.detachShader(shaderProgramObject_PF, vertexShaderObject_PF);
            gl.deleteShader(vertexShaderObject_PF);
            vertexShaderObject_PF = null;
        }

        gl.deleteProgram(shaderProgramObject_PF);
        shaderProgramObject_PF = null;
    }
}

function keyDown(event)
{
    //code
    switch(event.keyCode)
    {
        case 27:
            //Escape
            toggleFullscreen();
            break;

        case 81:
            //'Q' or 'q'
            uninitialize();
            //close app's tab in browser
            window.close(); // may not work in Firefox but works in other
            break;
                
        case 76:
            // 'L' or 'l'
            if (isLightEnabled == 0)
            {
                isLightEnabled = 1;
                VKeyIsPressed = 1;
                FKeyIsPressed = 0;
            }
            else if(isLightEnabled == 1)
            {
                isLightEnabled = 0;
            }
            break;

        case 86:
            //'V' or 'v' - PER-VERTEX
            if (VKeyIsPressed == 0)
			{
				VKeyIsPressed = 1;
				FKeyIsPressed = 0;
			}
			else if (VKeyIsPressed == 1)
			{
				VKeyIsPressed = 0;
			}
			break;

        case 70:
            //'F' or 'f' - PER-FRAGMENT
            if(FKeyIsPressed == 0)
            {
                FKeyIsPressed = 1;
                VKeyIsPressed = 0;
            }
            else if (FKeyIsPressed == 1)
            {
                FKeyIsPressed = 0;
            }
            break;
    }
}

function mouseDown()
{
    //code
}
