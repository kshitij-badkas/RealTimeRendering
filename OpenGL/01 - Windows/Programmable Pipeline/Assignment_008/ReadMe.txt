//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: 3D White Cube 
------------------------------------------------------------------------------------------------------
Assignment Number 		: 008
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_008_White_Cube
------------------------------------------------------------------------------------------------------
Created New File(s) 	: White_Cube.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. Global variables -
				GLuint gVao_Cube, gVbo_Position_Cube; //CUBE

2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE


4. Initialize  : Changes are as follows:
			
			1. 	const GLfloat cubeVertices[] =
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

			2. //Disable culling of back faces (default)
				glDisable(GL_CULL_FACE);


				
4. Display(): 	//CUBE HAS 6 FACES, HENCE CALLING SIX TIMES - "2ND PARAMETER WILL CHANGE ACCORDINGLY"
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
				glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
				glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
				glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
				glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

				

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
