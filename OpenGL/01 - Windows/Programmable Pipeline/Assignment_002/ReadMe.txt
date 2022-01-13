/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Perspective Triangle
------------------------------------------------------------------------------------------------------
Assignment Number 		: 002
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_002_Perspective_Triangle
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Perspective_Triangle.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. Header file  -
				#include "vmath.h"

			2. Global variables -
				GLuint gVao;
				GLuint gVbo_Position;
				GLuint gMVPMatrixUniform;

				mat4 gPerspectiveProjectionMatrix;

2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE


4. Initialize  : Changes are as follows:
			
			1. 	//VERTEX SHADER
				const GLchar* vertexShaderSourceCode =
					"#version 450 core" \
					"\n" \
					"in vec4 vPosition;" \
					"uniform mat4 u_mvpMatrix;" \
					"void main(void)" \
					"{" \
					"gl_Position = u_mvpMatrix * vPosition;" \
					"}";


			2.	//FRAGMENT SHADER
				const GLchar* fragmentShaderSourceCode =
					"#version 450 core" \
					"\n" \
					"out vec4 FragColor;" \
					"void main(void)" \
					"{" \
					"FragColor = vec4(1.0,1.0,1.0,1.0);" \
					"}";

			3.	//ERROR-CHECKING OF ALL SHADER OBJECTS -
				//Shader Compilation Error Checking
				GLint iInfoLogLength = 0;
				GLint iShaderCompiledStatus = 0;
				char* szInfoLog = NULL;
				glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
				if (iShaderCompiledStatus == GL_FALSE)	//if Shader Compiled Status Failed
				{
					glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
					if (iInfoLogLength > 0)
					{
						szInfoLog = (char*)malloc(iInfoLogLength);
						if (szInfoLog != NULL)
						{
							GLsizei written;
							glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
							fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
							free(szInfoLog);
							DestroyWindow(ghwnd);
						}
					}
				}

				
			4.	//get MVP Uniform Location
				gMVPMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");

				//vertices, colours, shader attribs, vbo, vao initializations
				const GLfloat triangleVertices[] =
				{
					0.0f, 1.0f, 0.0f, //apex
					-1.0f, -1.0f, 0.0f, //left-bottom
					1.0f, -1.0f, 0.0f //right-bottom
				};

				glGenVertexArrays(1, &gVao);
				glBindVertexArray(gVao);

				glGenBuffers(1, &gVbo_Position);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
				glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

				glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

				glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);


				//3D & Depth Code
				glShadeModel(GL_SMOOTH);	//anti-aliasing
				glClearDepth(1.0f);		//Default value passed to the Function
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
				//we will always cull back faces for beter performance
				glEnable(GL_CULL_FACE);

				//SetglColor
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Black

				//set identity matrix to identity matrix
				gPerspectiveProjectionMatrix = mat4::identity();

				Resize(WIN_WIDTH, WIN_HEIGHT);
				//WARM-UP CALL TO Resize();			

4. Display(): Changes as follows:

				//code
	
				//OpenGL Drawing
				mat4 translationMatrix;
				mat4 modelViewMatrix = mat4::identity();
				mat4 modelViewProjectionMatrix = mat4::identity();

				translationMatrix = translate(0.0f, 0.0f, -3.0f);
				modelViewMatrix = translationMatrix;
				modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

				glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

				//bind vao - vertex array object
				glBindVertexArray(gVao);

				//draw
				glDrawArrays(GL_TRIANGLES, 0, 3);

				//unbind vao
				glBindVertexArray(0);
				

--------------------------------------------------------------------------------------------------------
Following are the contents of 'Header.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101


--------------------------------------------------------------------------------------------------------
Following are the contents of 'Resources.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "Headers.h"
MYICON ICON KAB.ico
