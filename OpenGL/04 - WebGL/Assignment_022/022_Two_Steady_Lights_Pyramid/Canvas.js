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

//pyramid
var vao_pyramid;
var vbo_position_pyramid;
var vbo_normal_pyramid;

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
var anglePyramid = 0.0;

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
    "in vec3 vNormal;"+
    "uniform mat4 u_model_matrix;"+
    "uniform mat4 u_view_matrix;"+
    "uniform mat4 u_projection_matrix;"+
    "uniform int u_L_KEY_PRESSED;"+
    "uniform vec3 u_LA_Red;\n"+
    "uniform vec3 u_LD_Red;\n"+
    "uniform vec3 u_LS_Red;\n"+
    "uniform vec4 u_light_position_Red;\n"+
    "uniform vec3 u_LA_Blue;\n"+
    "uniform vec3 u_LD_Blue;\n"+
    "uniform vec3 u_LS_Blue;\n"+
    "uniform vec4 u_light_position_Blue;\n"+
    "uniform vec3 u_KA;\n"+
    "uniform vec3 u_KD;\n"+
    "uniform vec3 u_KS;\n"+
    "uniform float u_material_shininess;\n"+
    "out vec3 phong_ads_light;\n"+
    "void main(void)"+
    "{"+
    "if(u_L_KEY_PRESSED == 1)"+
    "{"+
    "vec3 ambient_Red;"+
    "vec3 diffuse_Red;"+
    "vec3 specular_Red;"+
    "vec3 light_direction_Red;"+
    "vec3 reflection_vector_Red;"+
    "vec3 ambient_Blue;"+
    "vec3 diffuse_Blue;"+
    "vec3 specular_Blue;"+
    "vec3 light_direction_Blue;"+
    "vec3 reflection_vector_Blue;"+
    "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
    "vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);"+
    "vec3 view_vector = normalize(-eye_coordinates.xyz);"+
    "light_direction_Red = normalize(vec3(u_light_position_Red - eye_coordinates));"+
    "reflection_vector_Red = reflect(-light_direction_Red, transformed_normal);"+
    "ambient_Red = u_LA_Red * u_KA;"+
    "diffuse_Red = u_LD_Red * u_KD * max(dot(light_direction_Red, transformed_normal), 0.0f);"+
    "specular_Red = u_LS_Red * u_KS * pow(max(dot(reflection_vector_Red, view_vector), 0.0f), u_material_shininess);"+
    "light_direction_Blue = normalize(vec3(u_light_position_Blue - eye_coordinates));"+
    "reflection_vector_Blue = reflect(-light_direction_Blue, transformed_normal);"+
    "ambient_Blue = u_LA_Blue * u_KA;"+
    "diffuse_Blue = u_LD_Blue * u_KD * max(dot(light_direction_Blue, transformed_normal), 0.0f);"+
    "specular_Blue = u_LS_Blue * u_KS * pow(max(dot(reflection_vector_Blue, view_vector), 0.0f), u_material_shininess);"+
    "phong_ads_light = phong_ads_light + ambient_Red + diffuse_Red + specular_Red + ambient_Blue + diffuse_Blue + specular_Blue;"+
    "}"+
    "else"+
    "{"+
    "phong_ads_light = vec3(1.0f, 1.0f, 1.0f);"+
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
    "in vec3 phong_ads_light;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
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
    LaUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_LA_Blue");
	LdUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_LD_Blue");
    LsUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_LS_Blue");
    lightPositionUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_light_position_Blue");
	KaUniform = gl.getUniformLocation(shaderProgramObject, "u_KA");
    KdUniform = gl.getUniformLocation(shaderProgramObject, "u_KD");
    KsUniform = gl.getUniformLocation(shaderProgramObject, "u_KS");
    materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

    
    //vertices, colors, shaders attribs, vao, vbo init
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

var pyramidNormals = new Float32Array([
                                        0.0, 0.447214, 0.894427,// front-top
                                        0.0, 0.447214, 0.894427,// front-left
                                        0.0, 0.447214, 0.894427,// front-right

                                        0.894427, 0.447214, 0.0, // right-top
                                        0.894427, 0.447214, 0.0, // right-left
                                        0.894427, 0.447214, 0.0, // right-right

                                        0.0, 0.447214, -0.894427, // back-top
                                        0.0, 0.447214, -0.894427, // back-left
                                        0.0, 0.447214, -0.894427, // back-right

                                        -0.894427, 0.447214, 0.0, // left-top
                                        -0.894427, 0.447214, 0.0, // left-left
                                        -0.894427, 0.447214, 0.0 // left-right
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
    //create vbo for normal
    vbo_normal_pyramid = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_normal_pyramid);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidNormals, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_NORMAL,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_NORMAL);

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
    //variable declaration
    //light values
    var lightAmbient_Red = [0.0, 0.0, 0.0];
    var lightDiffuse_Red = [1.0, 0.0, 0.0];
    var lightSpecular_Red = [1.0, 0.0, 0.0];
    var lightPosition_Red = [-2.0, 0.0, 0.0, 1.0];

    var lightAmbient_Blue = [0.0, 0.0, 0.0];
    var lightDiffuse_Blue = [0.0, 0.0, 1.0];
    var lightSpecular_Blue = [0.0, 0.0, 1.0];
    var lightPosition_Blue = [2.0, 0.0, 0.0, 1.0];
    
    //material values
    var materialAmbient = [0.0, 0.0, 0.0];
    var materialDiffuse = [1.0, 1.0, 1.0];
    var materialSpecular = [1.0, 1.0, 1.0];
    var materialShininess = 50.0;

    //code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    //use
    gl.useProgram(shaderProgramObject);

    if(isLightEnabled == 1)
    {
        gl.uniform1i(LKeyPressedUniform, 1);
        gl.uniform3fv(LaUniform_Red, lightAmbient_Red);
        gl.uniform3fv(LdUniform_Red, lightDiffuse_Red);
        gl.uniform3fv(LsUniform_Red, lightSpecular_Red);
        gl.uniform4fv(lightPositionUniform_Red, lightPosition_Red);
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
    var rotationMatrix = mat4.create();
     
    //----------------------------------------------------------------//
    //PYRAMID
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);
    projectionMatrix = mat4.identity(projectionMatrix);
    rotationMatrix = mat4.identity(rotationMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -6.0]);
    rotationMatrix = mat4.rotateY(rotationMatrix, rotationMatrix, degToRad(anglePyramid));

    mat4.multiply(translationMatrix, translationMatrix, rotationMatrix); //T = T * R

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix); // T * R
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    gl.bindVertexArray(vao_pyramid);

    //drawing
    gl.drawArrays(gl.TRIANGLES, 0, 12);

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
    if(anglePyramid >= 360.0)
    {
        anglePyramid = 0.0;
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
