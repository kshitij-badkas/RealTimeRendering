//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: 3D Shapes Animation 
------------------------------------------------------------------------------------------------------
Assignment Number 		: 009
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_009_3D_Shapes_Animation
------------------------------------------------------------------------------------------------------
Created New File(s) 	: 3D_Shapes_Animation.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. Global variables -
				GLuint gVao_Pyramid, gVbo_Position_Pyramid, gVbo_Color_Pyramid; //PYRAMID
				GLuint gVao_Cube, gVbo_Position_Cube, gVbo_Color_Cube; //CUBE


2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE


4. Initialize  : Changes are as follows:
			
			1. //VERTEX SHADER
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

			2. //FRAGMENT SHADER
				const GLchar* fragmentShaderSourceCode =
				"#version 450 core" \
				"\n" \
				"in vec4 out_color;" \
				"out vec4 FragColor;" \
				"void main(void)" \
				"{" \
				"FragColor = out_color;" \
				"}";

			3. 	//TRIANGLE
			const GLfloat pyramidVertices[] =
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

			const GLfloat pyramidColors[] =
			{
				//FRONT
				1.0f, 0.0f, 0.0f, //R
				0.0f, 1.0f, 0.0f, //G
				0.0f, 0.0f, 1.0f, //B
				//RIGHT
				1.0f, 0.0f, 0.0f, //R
				0.0f, 0.0f, 1.0f, //B
				0.0f, 1.0f, 0.0f, //G
				//BACK
				1.0f, 0.0f, 0.0f, //R
				0.0f, 1.0f, 0.0f, //G
				0.0f, 0.0f, 1.0f, //B
				//LEFT
				1.0f, 0.0f, 0.0f, //R
				0.0f, 0.0f, 1.0f, //B
				0.0f, 1.0f, 0.0f  //G
			};

			//SQUARE
			const GLfloat cubeVertices[] =
			{
				//FRONT
				1.0f, 1.0f, 1.0f, //right-top
				-1.0f, 1.0f, 1.0f, //left-top
				-1.0f, -1.0f, 1.0f, //left-bottom
				1.0f, -1.0f, 1.0f, //right-bottom
				//RIGHT
				1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, -1.0f,
				//BACK
				-1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				//LEFT
				-1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, 1.0f,
				//TOP
				1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				//BOTTOM
				1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, 1.0f
			};

			const GLfloat cubeColors[] =
			{
				//FRONT
				1.0f, 0.0f, 0.0f, //R
				1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				//RIGHT
				0.0f, 1.0f, 0.0f, //G
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				//BACK
				0.0f, 0.0f, 1.0f, //B
				0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f,
				//LEFT
				0.0f, 1.0f, 1.0f, //C
				0.0f, 1.0f, 1.0f,
				0.0f, 1.0f, 1.0f,
				0.0f, 1.0f, 1.0f,
				//TOP
				1.0f, 0.0f, 1.0f, //M
				1.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 1.0f,
				//BOTTOM
				1.0f, 1.0f, 0.0f, //Y
				1.0f, 1.0f, 0.0f,
				1.0f, 1.0f, 0.0f,
				1.0f, 1.0f, 0.0f
			};

			2. //Disable culling of back faces (default)
				glDisable(GL_CULL_FACE);


				
4. Display(): 	NO CHANGE

				

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
