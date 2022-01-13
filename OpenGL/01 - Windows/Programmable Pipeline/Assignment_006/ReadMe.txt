//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: 2D Shapes Animation
------------------------------------------------------------------------------------------------------
Assignment Number 		: 006
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_006_2D_Shapes_Animation
------------------------------------------------------------------------------------------------------
Created New File(s) 	: 2D_Shapes_Animation.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : NO CHANGE

2. WinMain  : NO CHANGE
 
3. WndProc  : NO CHANGE


4. Initialize  : Changes are as follows:
			
			1. 	//Disable culling of back faces (default)
				glDisable(GL_CULL_FACE);


				
5. Display(): Changes are as follows:
			1. //OpenGL Drawing
				mat4 translationMatrix;
				mat4 rotationMatrix; //NEW
				mat4 modelViewMatrix;
				mat4 modelViewProjectionMatrix;

				//TRIANLGE
				translationMatrix = mat4::identity();
				rotationMatrix = mat4::identity(); //NEW
				modelViewMatrix = mat4::identity();
				modelViewProjectionMatrix = mat4::identity();

				translationMatrix = translate(-1.5f, 0.0f, -6.0f);
				rotationMatrix = rotate(0.0f, angleTriangle, 0.0f); //NEW
				modelViewMatrix = translationMatrix * rotationMatrix; //NEW
				modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

				glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

				//bind TRIANGLE vao - vertex array object
				glBindVertexArray(gVao_Triangle);

				//draw
				glDrawArrays(GL_TRIANGLES, 0, 3);

				//unbind TRIANGLE vao
				glBindVertexArray(0);

				//SQUARE
				translationMatrix = mat4::identity();
				rotationMatrix = mat4::identity(); //NEW
				modelViewMatrix = mat4::identity();
				modelViewProjectionMatrix = mat4::identity();

				translationMatrix = translate(1.5f, 0.0f, -6.0f);
				rotationMatrix = rotate(angleSquare, 0.0f, 0.0f); //NEW
				modelViewMatrix = translationMatrix * rotationMatrix; //NEW
				modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

				glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

				//bind SQUARE vao - vertex array object
				glBindVertexArray(gVao_Square);

				//draw
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

				//unbind SQUARE vao
				glBindVertexArray(0);
				
			
			2.	//ANIMATION CODE
				angleTriangle += 1.0f;
				angleSquare += 1.0f;
				
				if (angleTriangle >= 360.0f)
				{
					angleTriangle = 0.0f;
				}
				if (angleSquare >= 360.0f)
				{
					angleSquare = 0.0f;
				}


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
