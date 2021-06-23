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
var radius = 6.0;

//uniform
//mvp matrix
var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;
//light
var LaUniform_Red;
var LdUniform_Red;
var LsUniform_Red;
var lightPositionUniform_Red;

var LaUniform_Green;
var LdUniform_Green;
var LsUniform_Green;
var lightPositionUniform_Green;

var LaUniform_Blue;
var LdUniform_Blue;
var LsUniform_Blue;
var lightPositionUniform_Blue;

//material
var KaUniform;
var KdUniform;
var KsUniform;
var materialShininessUniform;

//key pressed
var LKeyPressedUniform;

//light toggle
var isLightEnabled = 0;

//animation
var lightAngle_Red = 0.0;
var lightAngle_Green = 0.0;
var lightAngle_Blue = 0.0;

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
    "uniform vec4 u_light_position_Red;"+
    "uniform vec4 u_light_position_Green;"+
    "uniform vec4 u_light_position_Blue;"+
    "out vec3 transformed_normal;"+
    "out vec3 light_direction_Red;"+
    "out vec3 light_direction_Green;"+
    "out vec3 light_direction_Blue;"+
    "out vec3 view_vector;"+
    "void main(void)"+
    "{"+
    "if(u_L_KEY_PRESSED == 1)"+
    "{"+
    "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
    "transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
    "light_direction_Red = vec3(u_light_position_Red - eye_coordinates);"+
    "light_direction_Green = vec3(u_light_position_Green - eye_coordinates);"+
    "light_direction_Blue = vec3(u_light_position_Blue - eye_coordinates);"+
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
    "in vec3 light_direction_Red;"+
    "in vec3 light_direction_Green;"+
    "in vec3 light_direction_Blue;"+
    "in vec3 view_vector;"+
    "uniform int u_L_KEY_PRESSED;"+
    "uniform vec3 u_LA_Red;"+
    "uniform vec3 u_LD_Red;"+
    "uniform vec3 u_LS_Red;"+
    "uniform vec3 u_LA_Green;"+
    "uniform vec3 u_LD_Green;"+
    "uniform vec3 u_LS_Green;"+
    "uniform vec3 u_LA_Blue;"+
    "uniform vec3 u_LD_Blue;"+
    "uniform vec3 u_LS_Blue;"+
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
    "vec3 normalized_light_direction_Red = normalize(light_direction_Red);"+
    "vec3 normalized_light_direction_Green = normalize(light_direction_Green);"+
    "vec3 normalized_light_direction_Blue = normalize(light_direction_Blue);"+
    "vec3 normalized_view_vector = normalize(view_vector);"+
    
    "vec3 ambient_Red = u_LA_Red * u_KA;"+
    "vec3 diffuse_Red = u_LD_Red * u_KD * max(dot(normalized_light_direction_Red, normalized_transformed_normal), 0.0f);"+
    "vec3 reflection_vector_Red = reflect(-normalized_light_direction_Red, normalized_transformed_normal);"+
    "vec3 specular_Red = u_LS_Red * u_KS * pow(max(dot(reflection_vector_Red, normalized_view_vector), 0.0f), u_material_shininess);"+

    "vec3 ambient_Green = u_LA_Green * u_KA;"+
    "vec3 diffuse_Green = u_LD_Green * u_KD * max(dot(normalized_light_direction_Green, normalized_transformed_normal), 0.0f);"+
    "vec3 reflection_vector_Green = reflect(-normalized_light_direction_Green, normalized_transformed_normal);"+
    "vec3 specular_Green = u_LS_Green * u_KS * pow(max(dot(reflection_vector_Green, normalized_view_vector), 0.0f), u_material_shininess);"+

    "vec3 ambient_Blue = u_LA_Blue * u_KA;"+
    "vec3 diffuse_Blue = u_LD_Blue * u_KD * max(dot(normalized_light_direction_Blue, normalized_transformed_normal), 0.0f);"+
    "vec3 reflection_vector_Blue = reflect(-normalized_light_direction_Blue, normalized_transformed_normal);"+
    "vec3 specular_Blue = u_LS_Blue * u_KS * pow(max(dot(reflection_vector_Blue, normalized_view_vector), 0.0f), u_material_shininess);"+

    "phong_ads_light = phong_ads_light + ambient_Red + diffuse_Red + specular_Red;"+
    "phong_ads_light = phong_ads_light + ambient_Green + diffuse_Green + specular_Green;"+
    "phong_ads_light = phong_ads_light + ambient_Blue + diffuse_Blue + specular_Blue;"+
    "}"+
    "else"+
    "{"+
    "phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"+
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
    LaUniform_Red = gl.getUniformLocation(shaderProgramObject, "u_LA_Red");
	LdUniform_Red = gl.getUniformLocation(shaderProgramObject, "u_LD_Red");
    LsUniform_Red = gl.getUniformLocation(shaderProgramObject, "u_LS_Red");
    lightPositionUniform_Red = gl.getUniformLocation(shaderProgramObject, "u_light_position_Red");
    LaUniform_Green = gl.getUniformLocation(shaderProgramObject, "u_LA_Green");
	LdUniform_Green = gl.getUniformLocation(shaderProgramObject, "u_LD_Green");
    LsUniform_Green = gl.getUniformLocation(shaderProgramObject, "u_LS_Green");
    lightPositionUniform_Green = gl.getUniformLocation(shaderProgramObject, "u_light_position_Green");
    LaUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_LA_Blue");
	LdUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_LD_Blue");
    LsUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_LS_Blue");
    lightPositionUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_light_position_Blue");
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
    var lightAmbient_Red = [0.0, 0.0, 0.0];
    var lightDiffuse_Red = [1.0, 0.0, 0.0];
    var lightSpecular_Red = [1.0, 0.0, 0.0];
    var lightPosition_Red = [0.0, 0.0, 0.0, 1.0];

    var lightAmbient_Green = [0.0, 0.0, 0.0];
    var lightDiffuse_Green = [0.0, 1.0, 0.0];
    var lightSpecular_Green = [0.0, 1.0, 0.0];
    var lightPosition_Green = [0.0, 0.0, 0.0, 1.0];

    var lightAmbient_Blue = [0.0, 0.0, 0.0];
    var lightDiffuse_Blue = [0.0, 0.0, 1.0];
    var lightSpecular_Blue = [0.0, 0.0, 1.0];
    var lightPosition_Blue = [0.0, 0.0, 0.0, 1.0];
    
    //material values
    var materialAmbient = [0.0, 0.0, 0.0];
    var materialDiffuse = [1.0, 1.0, 1.0];
    var materialSpecular = [1.0, 1.0, 1.0];
    var materialShininess = 120.0;

    //code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    //use
    gl.useProgram(shaderProgramObject);

    if(isLightEnabled == 1)
    {
        lightPosition_Red[0] = 0.0; //x
        lightPosition_Red[1] = radius * Math.sin(lightAngle_Red); //y
        lightPosition_Red[2] = radius * Math.cos(lightAngle_Red); //z
        lightPosition_Red[3] = 1.0; //w

        lightPosition_Green[0] = radius * Math.sin(lightAngle_Green); //x
        lightPosition_Green[1] = 0.0; //y
        lightPosition_Green[2] = radius * Math.cos(lightAngle_Green); //z
        lightPosition_Green[3] = 1.0; //w

        lightPosition_Blue[0] = radius * Math.sin(lightAngle_Blue); //x
        lightPosition_Blue[1] = radius * Math.cos(lightAngle_Blue); //y
        lightPosition_Blue[2] = 0.0; //z
        lightPosition_Blue[3] = 1.0; //w

        gl.uniform1i(LKeyPressedUniform, 1);
        gl.uniform3fv(LaUniform_Red, lightAmbient_Red);
        gl.uniform3fv(LdUniform_Red, lightDiffuse_Red);
        gl.uniform3fv(LsUniform_Red, lightSpecular_Red);
        gl.uniform4fv(lightPositionUniform_Red, lightPosition_Red);

        gl.uniform3fv(LaUniform_Green, lightAmbient_Green);
        gl.uniform3fv(LdUniform_Green, lightDiffuse_Green);
        gl.uniform3fv(LsUniform_Green, lightSpecular_Green);
        gl.uniform4fv(lightPositionUniform_Green, lightPosition_Green);

        gl.uniform3fv(LaUniform_Blue, lightAmbient_Blue);
        gl.uniform3fv(LdUniform_Blue, lightDiffuse_Blue);
        gl.uniform3fv(LsUniform_Blue, lightSpecular_Blue);
        gl.uniform4fv(lightPositionUniform_Blue, lightPosition_Blue);

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
    lightAngle_Red += 0.05;
	lightAngle_Green += 0.05;
	lightAngle_Blue += 0.05;
	if (lightAngle_Red >= 360.0)
	{
		lightAngle_Red = 0.0;
	}
	if (lightAngle_Blue >= 360.0)
	{
		lightAngle_Blue = 0.0;
	}
	if (lightAngle_Green >= 360.0)
	{
		lightAngle_Green = 0.0;
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
