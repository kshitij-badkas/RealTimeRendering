//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: 3D Shapes Textures 
------------------------------------------------------------------------------------------------------
Assignment Number 		: 020
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_010_3D_Shapes_Textures
------------------------------------------------------------------------------------------------------
Created New File(s) 	: 3D_Shapes_Textures.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. Global variables -
				GLuint gVao_Square;
				GLuint gVbo_Position_Square;
				GLuint gVbo_Texcoord_Square;
				GLuint gTextureSamplerUniform;
				GLuint tex_image; //texture
				GLubyte check_image[CHECK_IMAGE_WIDTH][CHECK_IMAGE_HEIGHT][4];


2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE


4. Initialize  : Changes are as follows:
			
			1. //VERTEX SHADER
				

			2. //FRAGMENT SHADER
				

			3. //pre-link binding
				
			4. 	//In Red-Book the first co-ordinates start from LEFT-BOTTOM (0,0) 
				const GLfloat squareTexCoords[] =
				{
					0.0f, 0.0f,
					0.0f, 1.0f,
					1.0f, 1.0f,
					1.0f, 0.0f
				};

				//GEN START
				glGenVertexArrays(1, &gVao_Square);
				//BIND START
				glBindVertexArray(gVao_Square);

				//FOR SQUARE POSITION
				glGenBuffers(1, &gVbo_Position_Square);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);
				glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
				glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
				glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//FOR SQUARE TEXCOORD
				glGenBuffers(1, &gVbo_Texcoord_Square);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texcoord_Square);
				glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), squareTexCoords, GL_STATIC_DRAW);
				glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
				glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//BIND END
				glBindVertexArray(0);


5. //Function LoadGLTexture
void LoadGLTexture(void)
{
	//prototype
	void MakeCheckImage(void);

	//code
	MakeCheckImage();

	glGenTextures(1, &tex_image);
	glBindTexture(GL_TEXTURE_2D, tex_image);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //make 2nd parameter '1' in PP instead of '4' for Faster Performance.

	//Setting texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECK_IMAGE_WIDTH, CHECK_IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, check_image);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

6. 
void MakeCheckImage(void)
{
	//code
	int i, j, c;
	for (i = 0; i < CHECK_IMAGE_HEIGHT; i++)
	{
		for (j = 0; j < CHECK_IMAGE_WIDTH; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			check_image[i][j][0] = (GLubyte)c;
			check_image[i][j][1] = (GLubyte)c;
			check_image[i][j][2] = (GLubyte)c;
			check_image[i][j][3] = 255;
		}
	}
}


6. Display(): Chnages are as follows: 

void Display(void)
{
	//variables
	GLfloat squareVertices[12];

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	//OpenGL Drawing
	mat4 translationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	//SQUARE
	translationMatrix = translate(0.0f, 0.0f, -3.6f);
	modelViewMatrix = translationMatrix;
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_image);
	glUniform1i(gTextureSamplerUniform, 0);

	//bind vao - vertex array object
	glBindVertexArray(gVao_Square);

	//------------------------------draw start-------------------------------------//

	//FRONT-FACING QUAD
	squareVertices[0] = -2.0f;
	squareVertices[1] = -1.0f;
	squareVertices[2] = 0.0f;

	squareVertices[3] = -2.0f;
	squareVertices[4] = 1.0f;
	squareVertices[5] = 0.0f;

	squareVertices[6] = 0.0f;
	squareVertices[7] = 1.0f;
	squareVertices[8] = 0.0f;

	squareVertices[9] = 0.0f;
	squareVertices[10] = -1.0f;
	squareVertices[11] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), squareVertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//-------------------------------------------------------------------//
	
	//ANGLUAR QUAD
	squareVertices[0] = 1.0f;
	squareVertices[1] = -1.0f;
	squareVertices[2] = 0.0f;

	squareVertices[3] = 1.0f;
	squareVertices[4] = 1.0f;
	squareVertices[5] = 0.0f;

	squareVertices[6] = 2.41421f;
	squareVertices[7] = 1.0f;
	squareVertices[8] = -1.41421f;

	squareVertices[9] = 2.41421f;
	squareVertices[10] = -1.0f;
	squareVertices[11] = -1.41421f;

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Square);
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), squareVertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(KAB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(KAB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//unbind vao
	glBindVertexArray(0);

	//-------------------------------draw end-------------------------------------//

	//stop using OpenGL program object
	glUseProgram(0);

	//Double Buffer
	SwapBuffers(ghdc);

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


