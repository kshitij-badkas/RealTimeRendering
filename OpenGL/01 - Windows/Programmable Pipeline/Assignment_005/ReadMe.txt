//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: 2D Shapes
------------------------------------------------------------------------------------------------------
Assignment Number 		: 005
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_005_2D_Shapes
------------------------------------------------------------------------------------------------------
Created New File(s) 	: 2D_Shapes.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. Global variables -
				GLuint gVao_Triangle, gVbo_Position_Triangle, gVbo_Color_Triangle; //TRIANGLE
				GLuint gVao_Square, gVbo_Position_Square, gVbo_Color_Square; //SQUARE

2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE


4. Initialize  : Changes are as follows:
			
			1. 	
				//TRIANGLE
				const GLfloat triangleVertices[] =
				{
					0.0f, 1.0f, 0.0f, //apex
					-1.0f, -1.0f, 0.0f, //left-bottom
					1.0f, -1.0f, 0.0f //right-bottom
				};

				const GLfloat triangleColors[] =
				{
					1.0f, 0.0f, 0.0f, //R
					0.0f, 1.0f, 0.0f, //G
					0.0f, 0.0f, 1.0f  //B
				};

				//NEW 
				//SQUARE
				const GLfloat squareVertices[] =
				{
					1.0f, 1.0f, 0.0f, //right-top
					-1.0f, 1.0f, 0.0f, //left-top
					-1.0f, -1.0f, 0.0f, //left-bottom
					1.0f, -1.0f, 0.0f //right-bottom
				};

				const GLfloat squareColors[] =
				{
					0.0f, 0.0f, 1.0f, //B
					0.0f, 0.0f, 1.0f, //B
					0.0f, 0.0f, 1.0f, //B
					0.0f, 0.0f, 1.0f //B
				};

			2. //FOR TRIANGLE 
				//GEN START
				glGenVertexArrays(1, &gVao_Triangle);
				//BIND START
				glBindVertexArray(gVao_Triangle);

				//FOR TRIANGLE POSITION
				glGenBuffers(1, &gVbo_Position_Triangle);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Triangle);
				glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
				glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
				glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//FOR TRIANGLE COLORS
				glGenBuffers(1, &gVbo_Color_Triangle);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Triangle);
				glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_STATIC_DRAW);
				glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
				glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				
				//BIND END
				glBindVertexArray(0);


				//FOR SQUARE
				//GEN START
				glGenVertexArrays(1, &gVao_Square);
				//BIND START
				glBindVertexArray(gVao_Square);

				//FOR SQUARE POSITION
				glGenBuffers(1, &gVbo_Position_Square);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);
				glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
				glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
				glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//FOR SQUARE COLOR
				glGenBuffers(1, &gVbo_Color_Square);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Square);
				glBufferData(GL_ARRAY_BUFFER, sizeof(squareColors), squareColors, GL_STATIC_DRAW);
				glVertexAttribPointer(KAB_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
				glEnableVertexAttribArray(KAB_ATTRIBUTE_COLOR);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//BIND END
				glBindVertexArray(0);

				
4. Display(): Changes are as follows:
			1. //TRIANLGE
				translationMatrix = translate(-1.5f, 0.0f, -6.0f);
				modelViewMatrix = translationMatrix;
				modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

				glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

				//bind vao - vertex array object
				glBindVertexArray(gVao_Triangle);

				//draw
				glDrawArrays(GL_TRIANGLES, 0, 3);

				//unbind vao
				glBindVertexArray(0);

				//SQUARE
				translationMatrix = translate(1.5f, 0.0f, -6.0f);
				modelViewMatrix = translationMatrix;
				modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

				glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

				//bind vao - vertex array object
				glBindVertexArray(gVao_Square);

				//draw
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				
5. Uninitialize() -
				//TRIANGLE
				//destroy vao
				if (gVao_Triangle)
				{
					glDeleteVertexArrays(1, &gVao_Triangle);
					gVao_Triangle = 0;
				}

				//destroy vbo of position
				if (gVbo_Position_Triangle)
				{
					glDeleteVertexArrays(1, &gVbo_Position_Triangle);
					gVbo_Position_Triangle = 0;
				}

				//destroy vbo of color
				if (gVbo_Color_Triangle)
				{
					glDeleteVertexArrays(1, &gVbo_Color_Triangle);
					gVbo_Color_Triangle = 0;
				}

				//SQUARE
				//destroy vao
				if (gVao_Square)
				{
					glDeleteVertexArrays(1, &gVao_Square);
					gVao_Square = 0;
				}

				//destroy vbo of position
				if (gVbo_Position_Square)
				{
					glDeleteVertexArrays(1, &gVbo_Position_Square);
					gVbo_Position_Square = 0;
				}

				//destroy vbo of color
				if (gVbo_Color_Square)
				{
					glDeleteVertexArrays(1, &gVbo_Color_Square);
					gVbo_Color_Square = 0;
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
