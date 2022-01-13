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
var sphere = new Array(24);

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

//light and material properties
var lightAmbient = new Float32Array ([0.0, 0.0, 0.0]);
var lightDiffuse = new Float32Array ([1.0, 1.0, 1.0]);
var lightSpecular = new Float32Array ([1.0, 1.0, 1.0]);
var lightPosition = new Float32Array ([0.0, 0.0, 0.0, 1.0]);

//MATERIAL UNIFORMS
var KaUniform1; //ambient material component
var KdUniform1; //diffuse material component
var KsUniform1; //specular material component
var materialShininessUniform1; //shininess material

var KaUniform2; //ambient material component
var KdUniform2; //diffuse material component
var KsUniform2; //specular material component
var materialShininessUniform2; //shininess material

var KaUniform3; //ambient material component
var KdUniform3; //diffuse material component
var KsUniform3; //specular material component
var materialShininessUniform3; //shininess material

var KaUniform4; //ambient material component
var KdUniform4; //diffuse material component
var KsUniform4; //specular material component
var materialShininessUniform4; //shininess material

var KaUniform5; //ambient material component
var KdUniform5; //diffuse material component
var KsUniform5; //specular material component
var materialShininessUniform5; //shininess material

var KaUniform6; //ambient material component
var KdUniform6; //diffuse material component
var KsUniform6; //specular material component
var materialShininessUniform6; //shininess material

var KaUniform7; //ambient material component
var KdUniform7; //diffuse material component
var KsUniform7; //specular material component
var materialShininessUniform7; //shininess material

var KaUniform8; //ambient material component
var KdUniform8; //diffuse material component
var KsUniform8; //specular material component
var materialShininessUniform8; //shininess material

var KaUniform9; //ambient material component
var KdUniform9; //diffuse material component
var KsUniform9; //specular material component
var materialShininessUniform9; //shininess material

var KaUniform10; //ambient material component
var KdUniform10; //diffuse material component
var KsUniform10; //specular material component
var materialShininessUniform10; //shininess material

var KaUniform11; //ambient material component
var KdUniform11; //diffuse material component
var KsUniform11; //specular material component
var materialShininessUniform11; //shininess material

var KaUniform12; //ambient material component
var KdUniform12; //diffuse material component
var KsUniform12; //specular material component
var materialShininessUniform12; //shininess material

var KaUniform13; //ambient material component
var KdUniform13; //diffuse material component
var KsUniform13; //specular material component
var materialShininessUniform13; //shininess material

var KaUniform14; //ambient material component
var KdUniform14; //diffuse material component
var KsUniform14; //specular material component
var materialShininessUniform14; //shininess material

var KaUniform15; //ambient material component
var KdUniform15; //diffuse material component
var KsUniform15; //specular material component
var materialShininessUniform15; //shininess material

var KaUniform16; //ambient material component
var KdUniform16; //diffuse material component
var KsUniform16; //specular material component
var materialShininessUniform16; //shininess material

var KaUniform17; //ambient material component
var KdUniform17; //diffuse material component
var KsUniform17; //specular material component
var materialShininessUniform17; //shininess material

var KaUniform18; //ambient material component
var KdUniform18; //diffuse material component
var KsUniform18; //specular material component
var materialShininessUniform18; //shininess material

var KaUniform19; //ambient material component
var KdUniform19; //diffuse material component
var KsUniform19; //specular material component
var materialShininessUniform19; //shininess material

var KaUniform20; //ambient material component
var KdUniform20; //diffuse material component
var KsUniform20; //specular material component
var materialShininessUniform20; //shininess material

var KaUniform21; //ambient material component
var KdUniform21; //diffuse material component
var KsUniform21; //specular material component
var materialShininessUniform21; //shininess material

var KaUniform22; //ambient material component
var KdUniform22; //diffuse material component
var KsUniform22; //specular material component
var materialShininessUniform22; //shininess material

var KaUniform23; //ambient material component
var KdUniform23; //diffuse material component
var KsUniform23; //specular material component
var materialShininessUniform23; //shininess material

var KaUniform24; //ambient material component
var KdUniform24; //diffuse material component
var KsUniform24; //specular material component
var materialShininessUniform24; //shininess material

// material will have 24 different values
var materialAmbient1 = new Float32Array([ 0.0215, 0.1745, 0.0215 ]);
var materialDiffuse1 = new Float32Array([ 0.07568, 0.61424, 0.07568 ]);
var materialSpecular1 = new Float32Array([ 0.633, 0.727811, 0.633 ]);
var materialShininess1 = 0.6 * 128.0;

var materialAmbient2 = [ 0.135, 0.225, 0.1575];
var materialDiffuse2 = [ 0.54, 0.89, 0.63];
var materialSpecular2 = [ 0.316228, 0.316228, 0.316228];
var materialShininess2 = 0.1 * 128.0;

var materialAmbient3 = [ 0.05375, 0.05, 0.06625];
var materialDiffuse3 = [ 0.18275, 0.17, 0.22525];
var materialSpecular3 = [ 1.0, 1.0, 1.0];
var materialShininess3 = 0.3 * 128.0;

var materialAmbient4 = [ 0.25, 0.20725, 0.20725];
var materialDiffuse4 = [ 1.0, 0.829, 0.829];
var materialSpecular4 = [ 0.296648, 0.296648, 0.296648];
var materialShininess4 = 0.088 * 128.0;

var materialAmbient5 = [ 0.1745, 0.01175, 0.01175];
var materialDiffuse5 = [ 0.61424, 0.04136, 0.04136];
var materialSpecular5 = [ 0.727811, 0.626959, 0.626959];
var materialShininess5 = 0.6* 128.0;

var materialAmbient6 = [ 0.1, 0.18725, 0.1745];
var materialDiffuse6 = [ 0.396, 0.74151, 0.69102];
var materialSpecular6 = [ 0.297254, 0.30829, 0.306678];
var materialShininess6 = 0.1 * 128.0;

var materialAmbient7 = [ 0.329412, 0.223529, 0.027451];
var materialDiffuse7 = [ 0.780392, 0.568627, 0.113725];
var materialSpecular7 = [ 0.992157, 0.941176, 0.807843];
var materialShininess7 = 0.21794872 * 128.0;

var materialAmbient8 = [ 0.2125, 0.1275, 0.054];
var materialDiffuse8 = [ 0.714, 0.4284, 0.18144];
var materialSpecular8 = [ 0.393548, 0.271906, 0.166721];
var materialShininess8 = 0.2 * 128.0;

var materialAmbient9 = [ 0.25, 0.25, 0.25];
var materialDiffuse9 = [ 0.4, 0.4, 0.4];
var materialSpecular9 = [ 0.774597, 0.774597, 0.774597];
var materialShininess9 = 0.6 * 128.0;

var materialAmbient10 = [ 0.19125, 0.0735, 0.0225];
var materialDiffuse10 = [ 0.7038, 0.27048, 0.0828];
var materialSpecular10 = [ 0.256777, 0.137622, 0.086014];
var materialShininess10 = 0.1 * 128.0;

var materialAmbient11 = [ 0.24725, 0.1995, 0.0745];
var materialDiffuse11 = [ 0.75164, 0.60648, 0.22648];
var materialSpecular11 = [ 0.628281, 0.555802, 0.366065];
var materialShininess11 = 0.4 * 128.0;

var materialAmbient12 = [ 0.19225, 0.19225, 0.19225];
var materialDiffuse12 = [ 0.50754, 0.50754, 0.50754];
var materialSpecular12 = [ 0.508273, 0.508273, 0.508273];
var materialShininess12 = 0.4 * 128.0;

var materialAmbient13 = [ 0.0, 0.0, 0.0];
var materialDiffuse13 = [ 0.01, 0.01, 0.01];
var materialSpecular13 = [ 0.50, 0.50, 0.50];
var materialShininess13 = 0.25 * 128.0;

var materialAmbient14 = [ 0.0, 0.1, 0.06];
var materialDiffuse14 = [ 0.0, 0.50980392, 0.50980392];
var materialSpecular14 = [ 0.50196078, 0.50196078, 0.50196078];
var materialShininess14 = 0.25 * 128.0;

var materialAmbient15 = [ 0.0, 0.0, 0.0];
var materialDiffuse15 = [ 0.1, 0.35, 0.1];
var materialSpecular15 = [ 0.45, 0.55, 0.45];
var materialShininess15 = 0.25 * 128.0;

var materialAmbient16 = [ 0.0, 0.0, 0.0];
var materialDiffuse16 = [ 0.5, 0.0, 0.0];
var materialSpecular16 = [ 0.7, 0.6, 0.6];
var materialShininess16 = 0.25 * 128.0;

var materialAmbient17 = [ 0.0, 0.0, 0.0];
var materialDiffuse17 = [ 0.55, 0.55, 0.55];
var materialSpecular17 = [ 0.70, 0.70, 0.70];
var materialShininess17 = 0.25 * 128.0;

var materialAmbient18 = [ 0.0, 0.0, 0.0];
var materialDiffuse18 = [ 0.5, 0.5, 0.0];
var materialSpecular18 = [ 0.60, 0.60, 0.50];
var materialShininess18 = 0.25 * 128.0;

var materialAmbient19 = [ 0.02, 0.02, 0.02];
var materialDiffuse19 = [ 0.01, 0.01, 0.01];
var materialSpecular19 = [ 0.4, 0.4, 0.4];
var materialShininess19 = 0.078125 * 128.0;

var materialAmbient20 = [ 0.0, 0.05, 0.05];
var materialDiffuse20 = [ 0.4, 0.5, 0.5];
var materialSpecular20 = [ 0.04, 0.7, 0.7];
var materialShininess20 = 0.078125 * 128.0;

var materialAmbient21 = [ 0.0, 0.05, 0.0];
var materialDiffuse21 = [ 0.4, 0.5, 0.4];
var materialSpecular21 = [ 0.04, 0.7, 0.04];
var materialShininess21 = 0.078125 * 128.0;

var materialAmbient22 = [ 0.05, 0.0, 0.0];
var materialDiffuse22 = [ 0.5, 0.4, 0.4];
var materialSpecular22 = [ 0.7, 0.04, 0.04];
var materialShininess22 = 0.078125 * 128.0;

var materialAmbient23 = [ 0.05, 0.05, 0.05];
var materialDiffuse23 = [ 0.5, 0.5, 0.5];
var materialSpecular23 = [ 0.7, 0.7, 0.7];
var materialShininess23 = 0.078125 * 128.0;

var materialAmbient24 = [ 0.5, 0.05, 0.0];
var materialDiffuse24 = [ 0.5, 0.5, 0.4];
var materialSpecular24 = [ 0.7, 0.7, 0.04];
var materialShininess24 = 0.078125 * 128.0;



//24 Vaos for 24 spheres
var gVao_sphere1;
var gVbo_sphere_position1;
var gVbo_sphere_normal1;
var gVbo_sphere_element1;

var gVao_sphere2;
var gVbo_sphere_position2;
var gVbo_sphere_normal2;
var gVbo_sphere_element2;

var gVao_sphere3;
var gVbo_sphere_position3;
var gVbo_sphere_normal3;
var gVbo_sphere_element3;

var gVao_sphere4;
var gVbo_sphere_position4;
var gVbo_sphere_normal4;
var gVbo_sphere_element4;

var gVao_sphere5;
var gVbo_sphere_position5;
var gVbo_sphere_normal5;
var gVbo_sphere_element5;

var gVao_sphere6;
var gVbo_sphere_position6;
var gVbo_sphere_normal6;
var gVbo_sphere_element6;

var gVao_sphere7;
var gVbo_sphere_position7;
var gVbo_sphere_normal7;
var gVbo_sphere_element7;

var gVao_sphere8;
var gVbo_sphere_position8;
var gVbo_sphere_normal8;
var gVbo_sphere_element8;

var gVao_sphere9;
var gVbo_sphere_position9;
var gVbo_sphere_normal9;
var gVbo_sphere_element9;

var gVao_sphere10;
var gVbo_sphere_position10;
var gVbo_sphere_normal10;
var gVbo_sphere_element10;

var gVao_sphere11;
var gVbo_sphere_position11;
var gVbo_sphere_normal11;
var gVbo_sphere_element11;

var gVao_sphere12;
var gVbo_sphere_position12;
var gVbo_sphere_normal12;
var gVbo_sphere_element12;

var gVao_sphere13;
var gVbo_sphere_position13;
var gVbo_sphere_normal13;
var gVbo_sphere_element13;

var gVao_sphere14;
var gVbo_sphere_position14;
var gVbo_sphere_normal14;
var gVbo_sphere_element14;

var gVao_sphere15;
var gVbo_sphere_position15;
var gVbo_sphere_normal15;
var gVbo_sphere_element15;

var gVao_sphere16;
var gVbo_sphere_position16;
var gVbo_sphere_normal16;
var gVbo_sphere_element16;

var gVao_sphere17;
var gVbo_sphere_position17;
var gVbo_sphere_normal17;
var gVbo_sphere_element17;

var gVao_sphere18;
var gVbo_sphere_position18;
var gVbo_sphere_normal18;
var gVbo_sphere_element18;

var gVao_sphere19;
var gVbo_sphere_position19;
var gVbo_sphere_normal19;
var gVbo_sphere_element19;

var gVao_sphere20;
var gVbo_sphere_position20;
var gVbo_sphere_normal20;
var gVbo_sphere_element20;

var gVao_sphere21;
var gVbo_sphere_position21;
var gVbo_sphere_normal21;
var gVbo_sphere_element21;

var gVao_sphere22;
var gVbo_sphere_position22;
var gVbo_sphere_normal22;
var gVbo_sphere_element22;

var gVao_sphere23;
var gVbo_sphere_position23;
var gVbo_sphere_normal23;
var gVbo_sphere_element23;

var gVao_sphere24;
var gVbo_sphere_position24;
var gVbo_sphere_normal24;
var gVbo_sphere_element24;

var updated_width = 0.0;
var updated_height = 0.0;

//new
//Light Rotation
var XRotation = 0.0;
var YRotation = 0.0;
var ZRotation = 0.0;

var key_pressed = 0; // = 1 for X Axis; = 2 for Y Axis; = 3 for Z Axis.
var radius = 10.0;

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
    "vec3 diffuse = u_LD * u_KD * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0);"+
    "vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);"+
    "vec3 specular = u_LS * u_KS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_material_shininess);"+
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

    KaUniform1 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform1 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform1 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform1 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform2 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform2 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform2 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform2 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform3 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform3 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform3 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform3 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform4 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform4 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform4 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform4 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform5 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform5 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform5 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform5 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform6 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform6 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform6 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform6 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform7 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform7 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform7 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform7 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform8 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform8 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform8 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform8 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform9 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform9 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform9 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform9 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform10 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform10 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform10 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform10 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform11 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform11 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform11 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform11 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform12 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform12 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform12 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform12 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform13 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform13 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform13 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform13 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform14 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform14 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform14 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform14 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform15 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform15 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform15 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform15 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform16 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform16 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform16 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform16 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform17 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform17 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform17 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform17 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform18 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform18 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform18 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform18 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform19 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform19 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform19 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform19 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform20 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform20 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform20 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform20 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform21 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform21 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform21 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform21 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform22 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform22 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform22 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform22 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform23 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform23 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform23 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform23 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

	KaUniform24 = gl.getUniformLocation(shaderProgramObject, "u_KA");
	KdUniform24 = gl.getUniformLocation(shaderProgramObject, "u_KD");
	KsUniform24 = gl.getUniformLocation(shaderProgramObject, "u_KS");
	materialShininessUniform24 = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");


    
    //vertices, colors, shaders attribs, vao, vbo init
    //----------------------------------------------------------------//
    //SPHERE
    for(var i = 0; i < 24; i++)
    {
        sphere[i] = new Mesh();
        makeSphere(sphere[i], 2.0, 30, 30);
    }
    
    //sphere = new Mesh();
    //makeSphere(sphere, 2.0, 30, 30);
    
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

    updated_width = canvas.width;
	updated_height = canvas.height;

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

    //code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    //use
    gl.useProgram(shaderProgramObject);

    if(isLightEnabled == 1)
    {
        if(key_pressed == 1)
        {
            lightPosition[0] = radius * Math.sin(YRotation); //x
			lightPosition[1] = 0.0; //y
			lightPosition[2] = radius * Math.cos(YRotation); //z
			lightPosition[3] = 1.0; //w
        }
        else if(key_pressed == 2)
        {
            lightPosition[0] = 0.0; //x
			lightPosition[1] = radius * Math.sin(YRotation);  //y
			lightPosition[2] = radius * Math.cos(YRotation); //z
			lightPosition[3] = 1.0; //w
        }
        else if(key_pressed == 3)
        {
            lightPosition[0] = radius * Math.sin(YRotation); //x
			lightPosition[1] = radius * Math.cos(YRotation); //y
			lightPosition[2] = 0.0;//z
			lightPosition[3] = 1.0; //w
        }

        gl.uniform1i(LKeyPressedUniform, 1);
        gl.uniform3fv(LaUniform, lightAmbient);
        gl.uniform3fv(LdUniform, lightDiffuse);
        gl.uniform3fv(LsUniform, lightSpecular);
        gl.uniform4fv(lightPositionUniform, lightPosition);

    }
    else
    {
        gl.uniform1i(LKeyPressedUniform, 0);
    }
    

    var translationMatrix = mat4.create();
    var viewMatrix = mat4.create();
     
    //----------------------------------------------------------------//
    //SPHERE 1
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(0, updated_height * 4.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform1, materialAmbient1);
    gl.uniform3fv(KdUniform1, materialDiffuse1);
    gl.uniform3fv(KsUniform1, materialSpecular1);
    gl.uniform1f(materialShininessUniform1, materialShininess1);
    sphere[0].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 2
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 4, updated_height * 4.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform2, materialAmbient2);
    gl.uniform3fv(KdUniform2, materialDiffuse2);
    gl.uniform3fv(KsUniform2, materialSpecular2);
    gl.uniform1f(materialShininessUniform2, materialShininess2);
    sphere[1].draw();
    //unbind
    //Mesh.js
    
    //----------------------------------------------------------------//
    //SPHERE 3
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 2, updated_height * 4.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform3, materialAmbient3);
    gl.uniform3fv(KdUniform3, materialDiffuse3);
    gl.uniform3fv(KsUniform3, materialSpecular3);
    gl.uniform1f(materialShininessUniform3, materialShininess3);
    sphere[2].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 4
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width * 3.0 / 4, updated_height * 4.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform4, materialAmbient4);
    gl.uniform3fv(KdUniform4, materialDiffuse4);
    gl.uniform3fv(KsUniform4, materialSpecular4);
    gl.uniform1f(materialShininessUniform4, materialShininess4);
    sphere[3].draw();

    //unbind
    //Mesh.js


    //----------------------------------------------------------------//
    //SPHERE 5
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(0, updated_height * 3.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform5, materialAmbient5);
    gl.uniform3fv(KdUniform5, materialDiffuse5);
    gl.uniform3fv(KsUniform5, materialSpecular5);
    gl.uniform1f(materialShininessUniform5, materialShininess5);
    sphere[4].draw();

    //unbind
    //Mesh.js


    //----------------------------------------------------------------//
    //SPHERE 6
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 4, updated_height * 3.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform6, materialAmbient6);
    gl.uniform3fv(KdUniform6, materialDiffuse6);
    gl.uniform3fv(KsUniform6, materialSpecular6);
    gl.uniform1f(materialShininessUniform6, materialShininess6);
    sphere[5].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 7
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 2, updated_height * 3.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform7, materialAmbient7);
    gl.uniform3fv(KdUniform7, materialDiffuse7);
    gl.uniform3fv(KsUniform7, materialSpecular7);
    gl.uniform1f(materialShininessUniform6, materialShininess6);
    sphere[6].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 8
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width * 3.0 / 4.0, updated_height * 3.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform8, materialAmbient8);
    gl.uniform3fv(KdUniform8, materialDiffuse8);
    gl.uniform3fv(KsUniform8, materialSpecular8);
    gl.uniform1f(materialShininessUniform8, materialShininess8);
    sphere[7].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 9
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(0, updated_height * 2.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform9, materialAmbient9);
    gl.uniform3fv(KdUniform9, materialDiffuse9);
    gl.uniform3fv(KsUniform9, materialSpecular9);
    gl.uniform1f(materialShininessUniform9, materialShininess9);
    sphere[8].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 10
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 4, updated_height * 2.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform10, materialAmbient10);
    gl.uniform3fv(KdUniform10, materialDiffuse10);
    gl.uniform3fv(KsUniform10, materialSpecular10);
    gl.uniform1f(materialShininessUniform10, materialShininess10);
    sphere[9].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 11
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 2, updated_height * 2.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform11, materialAmbient11);
    gl.uniform3fv(KdUniform11, materialDiffuse11);
    gl.uniform3fv(KsUniform11, materialSpecular11);
    gl.uniform1f(materialShininessUniform11, materialShininess11);
    sphere[10].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 12
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width * 3.0 / 4.0, updated_height * 2.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform12, materialAmbient12);
    gl.uniform3fv(KdUniform12, materialDiffuse12);
    gl.uniform3fv(KsUniform12, materialSpecular12);
    gl.uniform1f(materialShininessUniform12, materialShininess12);
    sphere[11].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 13
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(0, updated_height * 1.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform13, materialAmbient13);
    gl.uniform3fv(KdUniform13, materialDiffuse13);
    gl.uniform3fv(KsUniform13, materialSpecular13);
    gl.uniform1f(materialShininessUniform13, materialShininess13);
    sphere[12].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 14
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 4, updated_height * 1.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform14, materialAmbient14);
    gl.uniform3fv(KdUniform14, materialDiffuse14);
    gl.uniform3fv(KsUniform14, materialSpecular14);
    gl.uniform1f(materialShininessUniform14, materialShininess14);
    sphere[13].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 15
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 2, updated_height * 1.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform15, materialAmbient15);
    gl.uniform3fv(KdUniform15, materialDiffuse15);
    gl.uniform3fv(KsUniform15, materialSpecular15);
    gl.uniform1f(materialShininessUniform15, materialShininess15);
    sphere[14].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 16
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width * 3.0 / 4.0, updated_height * 1.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform16, materialAmbient16);
    gl.uniform3fv(KdUniform16, materialDiffuse16);
    gl.uniform3fv(KsUniform16, materialSpecular16);
    gl.uniform1f(materialShininessUniform16, materialShininess16);
    sphere[15].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 17
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(0, updated_height * 0.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform17, materialAmbient17);
    gl.uniform3fv(KdUniform17, materialDiffuse17);
    gl.uniform3fv(KsUniform17, materialSpecular17);
    gl.uniform1f(materialShininessUniform17, materialShininess17);
    sphere[16].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 18
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 4, updated_height * 0.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform18, materialAmbient18);
    gl.uniform3fv(KdUniform18, materialDiffuse18);
    gl.uniform3fv(KsUniform18, materialSpecular18);
    gl.uniform1f(materialShininessUniform18, materialShininess18);
    sphere[17].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 19
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 2, updated_height * 0.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform19, materialAmbient19);
    gl.uniform3fv(KdUniform19, materialDiffuse19);
    gl.uniform3fv(KsUniform19, materialSpecular19);
    gl.uniform1f(materialShininessUniform19, materialShininess19);
    sphere[18].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 20
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width * 3.0 / 4.0, updated_height * 0.65 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform20, materialAmbient20);
    gl.uniform3fv(KdUniform20, materialDiffuse20);
    gl.uniform3fv(KsUniform20, materialSpecular20);
    gl.uniform1f(materialShininessUniform20, materialShininess20);
    sphere[19].draw();

    //unbind
    //Mesh.js


    //----------------------------------------------------------------//
    //SPHERE 21
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(0, updated_height * -0.3 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform21, materialAmbient21);
    gl.uniform3fv(KdUniform21, materialDiffuse21);
    gl.uniform3fv(KsUniform21, materialSpecular21);
    gl.uniform1f(materialShininessUniform21, materialShininess21);
    sphere[20].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 22
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 4, updated_height * -0.3 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform22, materialAmbient22);
    gl.uniform3fv(KdUniform22, materialDiffuse22);
    gl.uniform3fv(KsUniform22, materialSpecular22);
    gl.uniform1f(materialShininessUniform22, materialShininess22);
    sphere[21].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 23
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width / 2, updated_height * -0.3 / 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform23, materialAmbient23);
    gl.uniform3fv(KdUniform23, materialDiffuse23);
    gl.uniform3fv(KsUniform23, materialSpecular23);
    gl.uniform1f(materialShininessUniform23, materialShininess23);
    sphere[22].draw();

    //unbind
    //Mesh.js

    //----------------------------------------------------------------//
    //SPHERE 24
    //identity
    translationMatrix = mat4.identity(translationMatrix);
    viewMatrix = mat4.identity(viewMatrix);

    //transformation
    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -8.0]);

    gl.uniformMatrix4fv(modelMatrixUniform, false, translationMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    //bind 
    //Mesh.js

    //drawing
    gl.viewport(updated_width * 3.0 / 4.0, updated_height * -0.3/ 6.0, updated_width / 4, updated_height / 4);
    gl.uniform3fv(KaUniform24, materialAmbient24);
    gl.uniform3fv(KdUniform24, materialDiffuse24);
    gl.uniform3fv(KsUniform24, materialSpecular24);
    gl.uniform1f(materialShininessUniform24, materialShininess24);
    sphere[23].draw();

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
    XRotation += 0.05;
	YRotation += 0.05;
	ZRotation += 0.05;
	if (XRotation >= 360.0)
	{
		XRotation = 0.0;
	}
	if (YRotation >= 360.0)
	{
		YRotation = 0.0;
	}
	if (ZRotation >= 360.0)
	{
		ZRotation = 0.0;
	}


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

        // 88,.89,90
        case 88:
			key_pressed = 1;
			XRotation = 0.0;
			break;

		case 89:
			key_pressed = 2;
			YRotation = 0.0;
			break;


		case 90:
			key_pressed = 3;
			ZRotation = 0.0;
			break;
    }
}

function mouseDown()
{
    //code
}
