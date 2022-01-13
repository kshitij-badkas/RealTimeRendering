//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Diffuse Light on Cube 
------------------------------------------------------------------------------------------------------
Assignment Number 		: 011
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_011_Diffuse_Cube
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Diffuse_Cube.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. Global variables -
				GLuint gVao_Cube, gVbo_Position_Cube; //CUBE

			2. 
				GLuint gVbo_Normal_Cube;
				GLuint modelViewMatrixUniform;
				GLuint projectionMatrixUniform;
				GLuint LKeyPressedUniform;
				GLuint LdUniform;
				GLuint KdUniform;
				GLuint lightPositionUniform;
				bool gbAnimate, gbLights;

2. WinMain  : NO CHANGE

 
3. WndProc  : Changes are as follows:
			
			case WM_CHAR:
			switch (wParam)
			{
			//Animation
			case 'A':
			case 'a':
				if (gbAnimate == true)
				{
					gbAnimate = false;
				}
				else if (gbAnimate == false)
				{
					gbAnimate = true;
				}
				break;
			
			//Light
			case 'L':
			case 'l':
				if (gbLights == true)
				{
					gbLights = false;
				}
				else if (gbLights == false)
				{
					gbLights = true;
				}
				break;

			default:
				break;
			}
			break;


4. Initialize  : Changes are as follows:
			
			1. 	//VERTEX SHADER
				const GLchar* vertexShaderSourceCode =
					"#version 450 core" \
					"\n" \
					"in vec4 vPosition;" \
					"in vec3 vNormal;" \
					"uniform mat4 u_model_view_matrix;" \
					"uniform mat4 u_projection_matrix;" \
					"uniform int u_L_KEY_PRESSED;" \
					"uniform vec3 u_LD;" \
					"uniform vec3 u_KD;" \
					"uniform vec4 u_light_position;" \
					"out vec3 diffuse_light;" \
					"void main(void)" \
					"{" \
						//LIGHT ENABLED
						"if(u_L_KEY_PRESSED == 1)" \
						"{" \
						//STEPS -
						//1. CALCUALTE EYE COORDINATES -> by multiplying position coordinates and model-view matrix
						//2. CALCUALTE NORMAL MATRIX -> by inverse of transpose of upper 3x3 of model-view matrix
						//3. CALCULATE TRANSFORMED NORMALS ->
						//4. CALCULATE 'S' SOURCE OF LIGHT ->
						//5. CALCULATE DIFFUSE LIGHT USING LIGHT EQUATION ->

						"vec4 eye_coordinates = u_model_view_matrix * vPosition;" \
						"mat3 normal_matrix = mat3(transpose(inverse(u_model_view_matrix)));" \
						"vec3 tnorm = normalize(normal_matrix * vNormal);" \
						"vec3 s = normalize(vec3(u_light_position - eye_coordinates));" \
						"diffuse_light = u_LD * u_KD * max(dot(s, tnorm), 0.0f);" \
						"}" \
						"gl_Position =  u_projection_matrix * u_model_view_matrix * vPosition;" \
					"}";

			2. //FRAGMENT SHADER
				const GLchar* fragmentShaderSourceCode =
					"#version 450 core" \
					"\n" \
					"in vec3 diffuse_light;" \
					"uniform int u_L_KEY_PRESSED;" \
					"out vec4 FragColor;" \
					"void main(void)" \
					"{" \
						"vec4 color;" \
						"if(u_L_KEY_PRESSED == 1)" \
						"{" \
						"color = vec4(diffuse_light, 1.0f);" \
						"}" \
						"else" \
						"{" \
						"color = vec4(1.0f, 1.0f, 1.0f, 1.0f);" \
						"}" \
						"FragColor = color;" \
					"}";

			3. //pre-link binding of shader program object with vertex shader attribute
				glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_POSITION, "vPosition");

				//NEW
				glBindAttribLocation(gShaderProgramObject, KAB_ATTRIBUTE_NORMAL, "vNormal");
			
			4.  modelViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_view_matrix");
				projectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
				LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_L_KEY_PRESSED");
				LdUniform = glGetUniformLocation(gShaderProgramObject, "u_LD");
				KdUniform = glGetUniformLocation(gShaderProgramObject, "u_KD");
				lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

			5.
				const GLfloat cubeNormals[] =
				{
					// top surface
					0.0f, 1.0f, 0.0f,  // top-right of top
					0.0f, 1.0f, 0.0f, // top-left of top
					0.0f, 1.0f, 0.0f, // bottom-left of top
					0.0f, 1.0f, 0.0f,  // bottom-right of top

					// bottom surface
					0.0f, -1.0f, 0.0f,  // top-right of bottom
					0.0f, -1.0f, 0.0f,  // top-left of bottom
					0.0f, -1.0f, 0.0f,  // bottom-left of bottom
					0.0f, -1.0f, 0.0f,   // bottom-right of bottom

					// front surface
					0.0f, 0.0f, 1.0f,  // top-right of front
					0.0f, 0.0f, 1.0f, // top-left of front
					0.0f, 0.0f, 1.0f, // bottom-left of front
					0.0f, 0.0f, 1.0f,  // bottom-right of front

					// back surface
					0.0f, 0.0f, -1.0f,  // top-right of back
					0.0f, 0.0f, -1.0f, // top-left of back
					0.0f, 0.0f, -1.0f, // bottom-left of back
					0.0f, 0.0f, -1.0f,  // bottom-right of back

					// left surface
					-1.0f, 0.0f, 0.0f, // top-right of left
					-1.0f, 0.0f, 0.0f, // top-left of left
					-1.0f, 0.0f, 0.0f, // bottom-left of left
					-1.0f, 0.0f, 0.0f, // bottom-right of left

					// right surface
					1.0f, 0.0f, 0.0f,  // top-right of right
					1.0f, 0.0f, 0.0f,  // top-left of right
					1.0f, 0.0f, 0.0f,  // bottom-left of right
					1.0f, 0.0f, 0.0f  // bottom-right of right
				};

		
				//FOR CUBE NORMALS
				glGenBuffers(1, &gVbo_Normal_Cube);
				glBindBuffer(GL_ARRAY_BUFFER, gVbo_Normal_Cube);
				glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
				glVertexAttribPointer(KAB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
				glEnableVertexAttribArray(KAB_ATTRIBUTE_NORMAL);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

4. Display(): 	

void Display(void)
{
	//variables
	static GLfloat angleCube = 0.0f;
	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	//NEW
	if (gbLights == true)
	{
		glUniform1i(LKeyPressedUniform, 1);
		glUniform3f(LdUniform, 1.0f, 1.0f, 1.0f); //white
		glUniform3f(KdUniform, 0.5f, 0.5f, 0.5f); //grey
		GLfloat lightPosition[] = { 0.0f,0.0f,2.0f,1.0f };
		glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	//OpenGL Drawing
	mat4 translationMatrix;
	mat4 scaleMatrix;
	mat4 rotationMatrix;
	mat4 modelViewMatrix;

	//CUBE
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -6.0f);
	rotationMatrix = rotate(angleCube, angleCube, angleCube);
	scaleMatrix = scale(0.75f, 0.75f, 0.75f);
	modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;

	//NEW
	glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	//bind vao - vertex array object
	glBindVertexArray(gVao_Cube);

	//draw
	//CUBE HAS 6 FACES, HENCE CALLING SIX TIMES - "2ND PARAMETER WILL CHANGE ACCORDINGLY"
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	//unbind vao
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);


	//ANIMATION CODE
	if (gbAnimate == true)
	{
		angleCube += 1.0f;
		if (angleCube >= 360.0f)
		{
			angleCube = 0.0f;
		}
	}
	
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
