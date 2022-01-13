//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Two Steady Lights on Moving Pyramid
------------------------------------------------------------------------------------------------------
Assignment Number 		: 016
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_016_Two_Steady_Lights_Pyramid
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Two_Steady_Lights_Pyramid.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. 
				//pyramid Vao and Vbo
				GLuint gVao_Pyramid;
				GLuint gVbo_Position_Pyramid;
				GLuint gVbo_Normal_Pyramid;

				//MATRIX UNIFORMS
				GLuint gModelMatrixUniform;
				GLuint gViewMatrixUniform;
				GLuint gProjectionMatrixUniform;

				//LIGHT UNIFORMS
				GLuint LaUniform_Red; //ambient light component
				GLuint LdUniform_Red; //diffuse light component
				GLuint LsUniform_Red; //specular light component
				GLuint lightPositionUniform_Red; //light position

				GLuint LaUniform_Blue; //ambient light component
				GLuint LdUniform_Blue; //diffuse light component
				GLuint LsUniform_Blue; //specular light component
				GLuint lightPositionUniform_Blue; //light position

				//MATERIAL UNIFORMS
				GLuint KaUniform; //ambient material component
				GLuint KdUniform; //diffuse material component
				GLuint KsUniform; //specular material component
				GLuint materialShininessUniform; //shininess material

				GLuint LKeyPressedUniform;

				GLfloat lightAmbient_Red[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				GLfloat lightDiffuse_Red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
				GLfloat lightSpecular_Red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
				GLfloat lightPosition_Red[] = { -2.0f, 0.0f, 0.0f, 1.0f };

				GLfloat lightAmbient_Blue[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				GLfloat lightDiffuse_Blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
				GLfloat lightSpecular_Blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
				GLfloat lightPosition_Blue[] = { 2.0f, 0.0f, 0.0f, 1.0f };

				GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				GLfloat materialShininess = 50.0f;
				bool gbLights = false;

2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE

4. Initialize  : Changes are as follows:
			
			1. 	//VERTEX SHADER
				const GLchar* vertexShaderSourceCode =
					"#version 450 core" \
					"\n" \
					"in vec4 vPosition;\n" \
					"in vec3 vNormal;\n" \
					"uniform mat4 u_model_matrix;\n" \
					"uniform mat4 u_view_matrix;\n" \
					"uniform mat4 u_projection_matrix;\n" \
					"uniform int u_L_KEY_PRESSED;\n" \
					"uniform vec3 u_LA_Red;\n" \
					"uniform vec3 u_LD_Red;\n" \
					"uniform vec3 u_LS_Red;\n" \
					"uniform vec4 u_light_position_Red;\n" \
					"uniform vec3 u_LA_Blue;\n" \
					"uniform vec3 u_LD_Blue;\n" \
					"uniform vec3 u_LS_Blue;\n" \
					"uniform vec4 u_light_position_Blue;\n" \
					"uniform vec3 u_KA;\n" \
					"uniform vec3 u_KD;\n" \
					"uniform vec3 u_KS;\n" \
					"uniform float u_material_shininess;\n" \
					"out vec3 phong_ads_light;\n" \
					"void main(void)" \
					"{" \
					"if(u_L_KEY_PRESSED == 1)" \
					"{" \
					"vec3 ambient_Red;" \
					"vec3 diffuse_Red;" \
					"vec3 specular_Red;" \
					"vec3 light_direction_Red;" \
					"vec3 reflection_vector_Red;" \
					"vec3 ambient_Blue;" \
					"vec3 diffuse_Blue;" \
					"vec3 specular_Blue;" \
					"vec3 light_direction_Blue;" \
					"vec3 reflection_vector_Blue;" \
					"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
					"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
					"vec3 view_vector = normalize(-eye_coordinates.xyz);" \
					"light_direction_Red = normalize(vec3(u_light_position_Red - eye_coordinates));" \
					"reflection_vector_Red = reflect(-light_direction_Red, transformed_normal);" \
					"ambient_Red = u_LA_Red * u_KA;" \
					"diffuse_Red = u_LD_Red * u_KD * max(dot(light_direction_Red, transformed_normal), 0.0f);" \
					"specular_Red = u_LS_Red * u_KS * pow(max(dot(reflection_vector_Red, view_vector), 0.0f), u_material_shininess);" \
					//"phong_ads_light = phong_ads_light + ambient_Red + diffuse_Red + specular_Red;" \
					
					"light_direction_Blue = normalize(vec3(u_light_position_Blue - eye_coordinates));" \
					"reflection_vector_Blue = reflect(-light_direction_Blue, transformed_normal);" \
					"ambient_Blue = u_LA_Blue * u_KA;" \
					"diffuse_Blue = u_LD_Blue * u_KD * max(dot(light_direction_Blue, transformed_normal), 0.0f);" \
					"specular_Blue = u_LS_Blue * u_KS * pow(max(dot(reflection_vector_Blue, view_vector), 0.0f), u_material_shininess);" \
					"phong_ads_light = phong_ads_light + ambient_Red + diffuse_Red + specular_Red + ambient_Blue + diffuse_Blue + specular_Blue;" \
					"}" \
					"else" \
					"{" \
					"phong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
					"}" \
					"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
					"}";

			2. //FRAGMENT SHADER
				const GLchar* fragmentShaderSourceCode =
					"#version 450 core" \
					"\n" \
					"in vec3 phong_ads_light;" \
					"out vec4 FragColor;" \
					"void main(void)" \
					"{" \
					"FragColor = vec4(phong_ads_light, 1.0f);" \
					"}";

			3.  //get Uniform Location
				gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
				gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
				gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
				LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_L_KEY_PRESSED");
				LaUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_LA_Red");
				LdUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_LD_Red");
				LsUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_LS_Red");
				lightPositionUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_light_position_Red");
				LaUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_LA_Blue");
				LdUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_LD_Blue");
				LsUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_LS_Blue");
				lightPositionUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_light_position_Blue");
				KaUniform = glGetUniformLocation(gShaderProgramObject, "u_KA");
				KdUniform = glGetUniformLocation(gShaderProgramObject, "u_KD");
				KsUniform = glGetUniformLocation(gShaderProgramObject, "u_KS");
				materialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

			4.
				//vertices, colours, shader attribs, vbo, vao initializations
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

				const GLfloat pyramidNormals[] =
				{
					0.0f, 0.447214f, 0.894427f,// front-top
					0.0f, 0.447214f, 0.894427f,// front-left
					0.0f, 0.447214f, 0.894427f,// front-right

					0.894427f, 0.447214f, 0.0f, // right-top
					0.894427f, 0.447214f, 0.0f, // right-left
					0.894427f, 0.447214f, 0.0f, // right-right

					0.0f, 0.447214f, -0.894427f, // back-top
					0.0f, 0.447214f, -0.894427f, // back-left
					0.0f, 0.447214f, -0.894427f, // back-right

					-0.894427f, 0.447214f, 0.0f, // left-top
					-0.894427f, 0.447214f, 0.0f, // left-left
					-0.894427f, 0.447214f, 0.0f // left-right

				};

4. Display(): 	

				if (gbLights == true)
				{
					glUniform1i(LKeyPressedUniform, 1);
					glUniform3fv(LaUniform_Red, 1, (GLfloat*)lightAmbient_Red);
					glUniform3fv(LdUniform_Red, 1, (GLfloat*)lightDiffuse_Red);
					glUniform3fv(LsUniform_Red, 1, (GLfloat*)lightSpecular_Red);
					glUniform4fv(lightPositionUniform_Red, 1, (GLfloat*)lightPosition_Red);
					glUniform3fv(LaUniform_Blue, 1, (GLfloat*)lightAmbient_Blue);
					glUniform3fv(LdUniform_Blue, 1, (GLfloat*)lightDiffuse_Blue);
					glUniform3fv(LsUniform_Blue, 1, (GLfloat*)lightSpecular_Blue);
					glUniform4fv(lightPositionUniform_Blue, 1, (GLfloat*)lightPosition_Blue);
					glUniform3fv(KaUniform, 1, (GLfloat*)materialAmbient);
					glUniform3fv(KdUniform, 1, (GLfloat*)materialDiffuse);
					glUniform3fv(KsUniform, 1, (GLfloat*)materialSpecular);
					glUniform1f(materialShininessUniform, materialShininess);

				}
				else
				{
					glUniform1i(LKeyPressedUniform, 0);
				}

				.
				.
				translationMatrix = translate(0.0f, 0.0f, -6.0f);
				rotationMatrix = rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
				translationMatrix = translationMatrix * rotationMatrix;
				projectionMatrix = gPerspectiveProjectionMatrix;

				glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, translationMatrix);
				glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
				glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);
				.
				.
				//draw
				glDrawArrays(GL_TRIANGLES, 0, 12); //PYRAMID HAS 4 FACES, HENCE 12 VERTICES
				
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
