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

var vao_lines;
var vbo_position_lines;
var vbo_color_lines;

var mvpUniform;
var perspectiveProjectionMatrix;

//colour uniforms
var colorUniform;

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
    //"out vec4 out_color;"+
    "void main(void)"+
    "{"+
    "gl_Position = u_mvpMatrix * vPosition;"+
    //"out_color = vColor;"+
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
    //"in vec4 out_color;"+
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

    //get uniform
    mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvpMatrix");
    colorUniform = gl.getUniformLocation(shaderProgramObject, "u_Color");
    //greenColorUniform = gl.getUniformLocation(shaderProgramObject, "u_greenColor");
    //blueColorUniform = gl.getUniformLocation(shaderProgramObject, "u_blueColor");
    //whiteColorUniform = gl.getUniformLocation(shaderProgramObject, "u_whiteColor");


    //vertices, colors, shaders attribs, vao, vbo init
    var lineColors = new Float32Array([
                                        0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0
                                        ]);
                                        

	//create vao
    vao_lines = gl.createVertexArray();
    gl.bindVertexArray(vao_lines);

    
    //----------------------------------------------------------------//
    //create vbo for position
    vbo_position_lines = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
    gl.bufferData(gl.ARRAY_BUFFER, 1300 * 2 * 4, gl.DYNAMIC_DRAW);
    
    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //----------------------------------------------------------------//

    
    //create vbo for color
    vbo_color_lines = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, 3 * 2 * 4, gl.DYNAMIC_DRAW);

    //unbind vbo
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //----------------------------------------------------------------//
    //unbind vao
    gl.bindVertexArray(null);

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
    var lines = new Float32Array(2000);
    var horizontalCentreLine = new Float32Array([-1.0, 0.0, 1.0, 0.0]); //central horizontal line
	var verticalCentreLine = new Float32Array([0.0, 1.0, 0.0, -1.0]); //central vertical line

    var horizontalLines = new Float32Array(2000);
    var colours = new Float32Array(2000);

    //colours
    var redColor = new Float32Array([1.0, 0.0, 0.0, 1.0]);
    var greenColor = new Float32Array([0.0, 1.0, 0.0, 1.0]);
    var blueColor = new Float32Array([0.0, 0.0, 1.0, 1.0]);
    var whiteColor = new Float32Array([1.0, 1.0, 1.0, 1.0]);


    //code
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    var translationMatrix = mat4.create();
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    // = resultingMatrix , currentMatrix, values
    translationMatrix = mat4.translate(modelViewMatrix, translationMatrix, [0.0, 0.0, -3.0]);

    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vao_lines);

    //drawing
    //HORIZONTAL LINES TOP
    var i = 0;
	var inc = 0.05;
	for (i = 0; i < 80; i += 4, inc += 0.05)
	{
		lines[i] = -1.0; //x1
		lines[i + 1] = inc; //y1
		lines[i + 2] = 1.0; //x2
		lines[i + 3] = inc; //y2
	}
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
    gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
	gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //Color
    gl.uniform4fv(colorUniform, blueColor);
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
	gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //draw
    gl.drawArrays(gl.LINES, 0, 40); //40 indices to render for 20 lines


	//HORIZONTAL LINE CENTRE
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
	gl.bufferData(gl.ARRAY_BUFFER, horizontalCentreLine, gl.DYNAMIC_DRAW); // 2 vertices with (x, y)
	gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);
    
    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //Color
    gl.uniform4fv(colorUniform, redColor);
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, horizontalCentreLine, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
	gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.LINES, 0, 2); //2 indices to render
	
    //HORIZONTAL LINES BOTTOM
	i = 0;
	inc = 0.05;
	for (i = 0; i < 80; i += 4, inc += 0.05)
	{
		lines[i] = -1.0; //x1
		lines[i + 1] = -inc; //y1
		lines[i + 2] = 1.0; //x2
		lines[i + 3] = -inc; //y2

	}
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
	gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
	gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //Color
    gl.uniform4fv(colorUniform, blueColor);
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.LINES, 0, 40); //40 indices to render for 20 lines

	///////////////////////
	//VERTICAL LINES LEFT
	i = 0;
	inc = 0.05;
	for (i = 0; i < 80; i += 4, inc += 0.05)
	{
		lines[i] = -inc; //x1
		lines[i + 1] = 1.0; //y1
		lines[i + 2] = -inc; //x2
		lines[i + 3] = -1.0; //y2
	}
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
	gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
	gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //Color
    gl.uniform4fv(colorUniform, blueColor);
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.LINES, 0, 40); //40 indices to render for 20 lines

	//VERTICAL LINE CENTRE - green
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
	gl.bufferData(gl.ARRAY_BUFFER, verticalCentreLine, gl.DYNAMIC_DRAW); // 2 vertices with (x, y)
	gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);
    
    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //Color
    gl.uniform4fv(colorUniform, greenColor);
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, verticalCentreLine, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.LINES, 0, 2); //2 indices to render


	//VERTICAL LINES RIGHT
	i = 0;
	inc = 0.05;
	for (i = 0; i < 80; i += 4, inc += 0.05)
	{
		lines[i] = inc; //x1
		lines[i + 1] = 1.0; //y1
		lines[i + 2] = inc; //x2
		lines[i + 3] = -1.0; //y2

	}
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
	gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
	gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //Color
    gl.uniform4fv(colorUniform, blueColor);
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.LINES, 0, 40); //40 indices to render for 20 lines


	//TRAINGLE USING LINES
	lines[0] = 0.0;
	lines[1] = 0.7;
	lines[2] = 0.7;
	lines[3] = -0.7;

	lines[4] = 0.7;
	lines[5] = -0.7;
	lines[6] = -0.7;
	lines[7] = -0.7;
	
	lines[8] = -0.7;
	lines[9] = -0.7;
	lines[10] = 0.0;
	lines[11] = 0.7;

    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
	gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 2 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
    //Color
    gl.uniform4fv(colorUniform, whiteColor);
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.LINES, 0, 6); //40 indices to render for 20 lines

    
	//RECTANGLE USING LINES
	lines[0] = 0.7;
	lines[1] = 0.7;
	lines[2] = 0.7;
	lines[3] = -0.7;

	lines[4] = 0.7;
	lines[5] = 0.7;
	lines[6] = -0.7;
	lines[7] = 0.7;

	lines[8] = -0.7;
	lines[9] = 0.7;
	lines[10] = -0.7;
	lines[11] = -0.7;

	lines[12] = -0.7;
	lines[13] = -0.7;
	lines[14] = 0.7;
	lines[15] = -0.7;


    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
	gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 2 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //Color
    gl.uniform4fv(colorUniform, whiteColor);
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.LINES, 0, 8); //40 indices to render for 20 lines

	
	//CIRCUMCIRCLE USING POINTS
	var circumCentreRadius = 0.0;
	circumCentreRadius = Math.sqrt(Math.pow(0.7, 2) + Math.pow(0.7, 2));

	i = 0;
	var cAngle = 0.0;
	for (cAngle = 0.0; cAngle < (2.0 * 3.14); cAngle = cAngle + 0.01, i += 2)
	{
		lines[i] = Math.cos(cAngle) * circumCentreRadius; //xn
		lines[i + 1] = Math.sin(cAngle) * circumCentreRadius; //yn
	}


    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
	gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 2 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //Color
    gl.uniform4fv(colorUniform, whiteColor);
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.LINES, 0, 629); //40 indices to render for 20 lines


	//INCIRCLE USING POINTS
    var inCircleRadius = 0.0;
	var distance_A = 0.0;
	var distance_B = 0.0;
	var distance_C = 0.0;
	var perimeter = 0.0;
	var semi = 0.0;
	var tArea = 0.0;
	//centre of circle
	var iX = 0.0;
	var iY = 0.0;

	//	DISTANCE FORMULA
	distance_A = Math.sqrt(Math.pow((-0.7 - 0.7), 2) + Math.pow((-0.7 - -0.7), 2));
	distance_B = Math.sqrt(Math.pow((0.0 - -0.7), 2) + Math.pow((0.7 - -0.7), 2));
	distance_C = Math.sqrt(Math.pow((0.7 - 0.0), 2) + Math.pow((-0.7 - 0.7), 2));

	//	SEMIPERIMETER
	perimeter = distance_A + distance_B + distance_C;
	semi = perimeter / 2;

	//	AREA OF TRIANGLE
	tArea = Math.sqrt(semi * (semi - distance_A) * (semi - distance_B) * (semi - distance_C));
	
	//	INCIRCLE COORDINATES
	iX = (distance_A * (0.0) + distance_B * (0.7) + distance_C * (-0.7)) / perimeter;
	iY = (distance_A * (0.7) + distance_B * (-0.7) + distance_C * (-0.7)) / perimeter;

	inCircleRadius = (tArea / semi);
	
	//OUTPUT ->
	
	i = 0;
	cAngle = 0.0;
	for (cAngle = 0.0; cAngle < (2.0 * 3.14); cAngle = cAngle + 0.01, i += 2)
	{
		lines[i] = iX + Math.cos(cAngle) * inCircleRadius; //xn
		lines[i + 1] = iY + Math.sin(cAngle) * inCircleRadius; //yn
	}

    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_lines);
	gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 2 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_POSITION,
                            2, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    //Color
    gl.uniform4fv(colorUniform, whiteColor);
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_lines);
    gl.bufferData(gl.ARRAY_BUFFER, lines, gl.DYNAMIC_DRAW); // 40 vertices with (x, y)
    gl.vertexAttribPointer(WebGLMacros.KAB_ATTRIBUTE_COLOR,
                            3, 
                            gl.FLOAT,
                            false, 0, 0);

    gl.enableVertexAttribArray(WebGLMacros.KAB_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.drawArrays(gl.LINES, 0, 629); //40 indices to render for 20 lines
	
	//-------------------------------draw end-------------------------------------//

    //unbind
    gl.bindVertexArray(null);

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
