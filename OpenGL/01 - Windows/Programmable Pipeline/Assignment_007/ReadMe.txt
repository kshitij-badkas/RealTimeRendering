//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: 3D White Pyramid 
------------------------------------------------------------------------------------------------------
Assignment Number 		: 007
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_007_White_Pyramid
------------------------------------------------------------------------------------------------------
Created New File(s) 	: White_Pyramid.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. Global variables -
				GLuint gVao_Pyramid, gVbo_Position_Pyramid; //PYRAMID

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
				"FragColor = vec4(1.0f,1.0f,1.0f,1.0f);" \
				"}";

			3.	const GLfloat pyramidVertices[] =
				{
					//FRONT
					0.0f, 1.0f, 0.0f, //apex
					-1.0f, -1.0f, 1.0f, //left-bottom
					1.0f, -1.0f, 1.0f, //right-bottom
					//RIGHT
					0.0f, 1.0f, 0.0f,
					1.0f, -1.0f, 1.0f,
					1.0f, -1.0f, -1.0f,
					//BACK
					0.0f, 1.0f, 0.0f,
					1.0f, -1.0f, -1.0f,
					-1.0f, -1.0f, -1.0f,
					//LEFT
					0.0f, 1.0f, 0.0f,
					-1.0f, -1.0f, -1.0f,
					-1.0f, -1.0f, 1.0f
				};

				
4. Display(): 	glDrawArrays(GL_TRIANGLES, 0, 12); //PYRAMID HAS 4 FACES, HENCE 12 VERTICES

				

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
