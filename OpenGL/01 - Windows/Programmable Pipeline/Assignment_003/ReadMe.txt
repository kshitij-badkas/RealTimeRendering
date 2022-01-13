//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Coloured Triangle
------------------------------------------------------------------------------------------------------
Assignment Number 		: 003
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_003_Coloured_Triangle
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Coloured_Triangle.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
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
				"in vec4 vColor;" \
				"uniform mat4 u_mvpMatrix;" \
				"out vec4 out_color;" \
				"void main(void)" \
				"{" \
				"gl_Position = u_mvpMatrix * vPosition;" \
				"out_color = vColor;" \
				"}";


			2.	//FRAGMENT SHADER
				const GLchar* fragmentShaderSourceCode =
				"#version 450 core" \
				"\n" \
				"in vec4 out_color;" \
				"out vec4 FragColor;" \
				"void main(void)" \
				"{" \
				"FragColor = out_color;" \
				"}";

			3. //pre-link binding of shader program object with fragment shader attribute
				glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_COLOR, "vColor");

			4.	const GLfloat triangleColors[] =
				{
					1.0f, 0.0f, 0.0f, //R
					0.0f, 1.0f, 0.0f, //G
					0.0f, 0.0f, 1.0f  //B
				};

				glGenBuffers(1, &gVbo_Color);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color);
				glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_STATIC_DRAW);
				glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
				glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				glBindVertexArray(0);

				
4. Display(): NO CHANGE
				

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
