//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: 3D Shapes Textures 
------------------------------------------------------------------------------------------------------
Assignment Number 		: 010
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
				//TEXTURES
				GLuint stone_texture;
				GLuint kundali_texture;
				GLuint gTextureSamplerUniform;


2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE


4. Initialize  : Changes are as follows:
			
			1. //VERTEX SHADER
				const GLchar* vertexShaderSourceCode =
				"#version 450 core" \
				"\n" \
				"in vec4 vPosition;" \
				"in vec2 vTexCoord;" \
				"uniform mat4 u_mvpMatrix;" \
				"out vec2 out_texcoord;" \
				"void main(void)" \
				"{" \
				"gl_Position = u_mvpMatrix * vPosition;" \
				"out_texcoord = vTexCoord;" \
				"}";

			2. //FRAGMENT SHADER
				const GLchar* fragmentShaderSourceCode =
				"#version 450 core" \
				"\n" \
				"in vec2 out_texcoord;" \
				"uniform sampler2D u_texture_sampler;" \
				"out vec4 FragColor;" \
				"void main(void)" \
				"{" \
				"FragColor = texture(u_texture_sampler, out_texcoord);" \
				"}";

			3. //pre-link binding of shader program object with vertex shader attribute
				glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_POSITION, "vPosition");

				//pre-link binding of shader program object with fragment shader attribute
				glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_TEXCOORD, "vTexCoord");
			
				//get MVP Uniform Location
				gMVPMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");
				
				//get Texture Sampler Uniform Location
				gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture_sampler");
			
			4. 	//PYRAMID
				const GLfloat pyramidTexCoords[] =
				{
					//FRONT
					0.5f, 1.0f,
					0.0f, 0.0f,
					1.0f, 0.0f,
					//RIGHT
					0.5f, 1.0f,
					1.0f, 0.0f,
					0.0f, 0.0f,
					//BACK
					0.5f, 1.0f,
					1.0f, 0.0f,
					0.0f, 0.0f,
					//LEFT
					0.5f, 1.0f,
					0.0f, 0.0f,
					1.0f, 0.0f
				};

				//CUBE
				const GLfloat cubeTexCoords[] =
				{
					//FRONT
					0.0f, 0.0f,
					1.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 1.0f,
					//RIGHT
					0.0f, 0.0f,
					1.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 1.0f,
					//BACK
					0.0f, 0.0f,
					1.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 1.0f,
					//LEFT
					0.0f, 0.0f,
					1.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 1.0f,
					//TOP
					0.0f, 0.0f,
					1.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 1.0f,
					//BOTTOM
					0.0f, 0.0f,
					1.0f, 0.0f,
					1.0f, 1.0f,
					0.0f, 1.0f
				};

			2. //Disable culling of back faces (default)
				glDisable(GL_CULL_FACE);

			3.
				//FOR PYRAMID TEXTURES
				glGenBuffers(1, &gVbo_Texture_Pyramid);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture_Pyramid);
				glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexCoords), pyramidTexCoords, GL_STATIC_DRAW);
				glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
				glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
				glBindBuffer(GL_ARRAY_BUFFER, 0); 
				
				//FOR CUBE TEXTURE
				glGenBuffers(1, &gVbo_Texture_Cube);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Texture_Cube);
				glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoords), cubeTexCoords, GL_STATIC_DRAW);
				glVertexAttribPointer(KAB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
				glEnableVertexAttribArray(KAB_ATTRIBUTE_TEXCOORD);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

			4. 
				//Loading Textures
				LoadGLTexture(&stone_texture, MAKEINTRESOURCE(STONE_BITMAP));
				LoadGLTexture(&kundali_texture, MAKEINTRESOURCE(KUNDALI_BITMAP));

			5. //Enable Textures
				glEnable(GL_TEXTURE_2D);

5. //Function LoadGLTexture
bool LoadGLTexture(GLuint* texture, TCHAR resourceID[])
{
	//variable declaration
	bool bResult = false;
	HBITMAP hBitmap = NULL; //image loading
	BITMAP bmp; //image loading

	//code
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), resourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		bResult = true;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);

		//OPENGL TEXTURE CODE STARTS HERE
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //make 2nd parameter '1' in PP instead of '4' for Faster Performance.

		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		//Setting texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//Push the Data into Graphics Memory with the Help of Graphics Driver
		//gluBuild2DMipmaps = glTexImage2D + glGenerateMipmap
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);

		// image loading
		DeleteObject(hBitmap);
	}

	return bResult;
}

6. Display(): Chnages are as follows: 

			//PYRAMID
			translationMatrix = mat4::identity();
			rotationMatrix = mat4::identity();
			modelViewMatrix = mat4::identity();
			modelViewProjectionMatrix = mat4::identity();

			translationMatrix = translate(-1.5f, 0.0f, -6.0f);
			rotationMatrix = rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
			modelViewMatrix = translationMatrix * rotationMatrix;
			modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix; //ORDER IS IMPORTANT

			glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, stone_texture);
			glUniform1i(gTextureSamplerUniform, 0);

			//bind vao - vertex array object
			glBindVertexArray(gVao_Pyramid);

			//draw
			glDrawArrays(GL_TRIANGLES, 0, 12); //PYRAMID HAS 4 FACES, HENCE 12 VERTICES

			//unbind vao
			glBindVertexArray(0);
						

--------------------------------------------------------------------------------------------------------
Following are the contents of 'Header.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101
#define STONE_BITMAP 102
#define KUNDALI_BITMAP 103

--------------------------------------------------------------------------------------------------------
Following are the contents of 'Resources.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "Headers.h"
MYICON ICON KAB.ico
STONE_BITMAP BITMAP Stone.bmp
KUNDALI_BITMAP BITMAP Vijay_Kundali.bmp

