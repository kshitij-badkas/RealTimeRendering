//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Toggling of Shaders 
------------------------------------------------------------------------------------------------------
Assignment Number 		: 015
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_015_Per_Vertex_Fragment_Toggle
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Per_Vertex_Fragment_Toggle.cpp, Headers.h, 
						vmath.h, Sphere.h, Sphere.lib, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. 
				//----------------------------------------------//
				//PER-VERTEX LIGHTING 
				//MODEL-VIEW-PROJECTION UNIFORMS
				GLuint gModelMatrixUniform_PV;
				GLuint gViewMatrixUniform_PV;
				GLuint gProjectionMatrixUniform_PV;

				//LIGHT UNIFORMS - ALL ARRAYS
				GLuint LaUniform_PV; //ambient light component
				GLuint LdUniform_PV; //diffuse light component
				GLuint LsUniform_PV; //specular light component
				GLuint lightPositionUniform_PV; //light position

				//MATERIAL UNIFORMS - Ka, Kd, Ks ARRAYS, shininess is a single value
				GLuint KaUniform_PV; //ambient material component
				GLuint KdUniform_PV; //diffuse material component
				GLuint KsUniform_PV; //specular material component
				GLuint materialShininessUniform_PV; //shininess material

				//per-vertex shader objects
				GLuint gVertexShaderObject_PV;
				GLuint gFragmentShaderObject_PV;
				GLuint gShaderProgramObject_PV;

				GLuint LKeyPressedUniform_PV; //L key is pressed

				//----------------------------------------------//

				//----------------------------------------------//
				//PER-FRAGMENT LIGHTING
				//MODEL-VIEW-PROJECTION UNIFORMS
				GLuint gModelMatrixUniform_PF;
				GLuint gViewMatrixUniform_PF;
				GLuint gProjectionMatrixUniform_PF;

				//LIGHT UNIFORMS - ALL ARRAYS
				GLuint LaUniform_PF; //ambient light component
				GLuint LdUniform_PF; //diffuse light component
				GLuint LsUniform_PF; //specular light component
				GLuint lightPositionUniform_PF; //light position

				//MATERIAL UNIFORMS - Ka, Kd, Ks ARRAYS, shininess is a single value
				GLuint KaUniform_PF; //ambient material component
				GLuint KdUniform_PF; //diffuse material component
				GLuint KsUniform_PF; //specular material component
				GLuint materialShininessUniform_PF; //shininess material

				//per-fragemnt shader objects
				GLuint gVertexShaderObject_PF;
				GLuint gFragmentShaderObject_PF;
				GLuint gShaderProgramObject_PF;

				GLuint LKeyPressedUniform_PF; //L key is pressed

				//----------------------------------------------//

2. WinMain  : NO CHANGE

 
3. WndProc  : Changes are as follows:

				case WM_CHAR:
				switch (wParam)
				{
				case 'Q': //QUIT
				case 'q':
					DestroyWindow(hwnd);
					break;

				case 'F' : //PER-FRAGMENT SHADING TOGGLE
				case 'f': 
					if (FKeyIsPressed == 0)
					{
						FKeyIsPressed = 1;
						VKeyIsPressed = 0;
					}
					else if (FKeyIsPressed == 1)
					{
						FKeyIsPressed = 0;
					}
					break;

				case 'V': //PER-VERTEX SHADING TOGGLE
				case 'v':
					if (VKeyIsPressed == 0)
					{
						VKeyIsPressed = 1;
						FKeyIsPressed = 0;
						fprintf(gpFile, "\nPER_VERTEX -> V pressed key = %d", VKeyIsPressed);
					}
					else if (VKeyIsPressed == 1)
					{
						VKeyIsPressed = 0;
						fprintf(gpFile, "\nPER_VERTEX -> V pressed key = %d", VKeyIsPressed);
					}
					break;

				case 'L':
				case 'l':
					if (gbLights == false)
					{
						gbLights = true;
						VKeyIsPressed = 1; //PER-VERTEX SHADING WILL BE ENABLED BY DEFAULT
						FKeyIsPressed = 0;
						fprintf(gpFile, "\n -> gbLights was false - now - L pressed key = %d", gbLights);

					}
					else if (gbLights == true)
					{
						gbLights = false;
						fprintf(gpFile, "\n -> gbLights was true - now - L pressed key = %d", gbLights);
					}
					break;

				default:
					break;
				}
				break;

4. Initialize  : Changes are as follows:
			
			1. 	// PER VERTEX SHADER

				void getVertexShader_PV(void)
				{
					//code
					//code
					//VERTEX SHADER FOR PER-VERTEX LIGHTING
					//create shader
					gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

					//provide source code to shader
					//use 'core' profile i.e PROGRAMMABLE PIPELINE
					const GLchar* vertexShaderSourceCode_PV =
						"#version 450 core" \
						"\n" \
						"in vec4 vPosition;" \
						"in vec3 vNormal;" \
						"uniform mat4 u_model_matrix;" \
						"uniform mat4 u_view_matrix;" \
						"uniform mat4 u_projection_matrix;" \
						"uniform int u_L_KEY_PRESSED;" \
						"uniform vec3 u_LA_PV;" \
						"uniform vec3 u_LD_PV;" \
						"uniform vec3 u_LS_PV;" \
						"uniform vec4 u_light_position_PV;" \
						"uniform vec3 u_KA_PV;" \
						"uniform vec3 u_KD_PV;" \
						"uniform vec3 u_KS_PV;" \
						"uniform float u_material_shininess_PV;" \
						"out vec3 phong_ads_light_PV;" \
						"void main(void)" \
						"{" \
						"if(u_L_KEY_PRESSED == 1)" \
						"{" \
						"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
						"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
						"vec3 light_direction = normalize(vec3(u_light_position_PV - eye_coordinates));" \
						"vec3 reflection_vector = reflect(-light_direction, transformed_normal);" \
						//SWIZZLING
						"vec3 view_vector = normalize(-eye_coordinates.xyz);" \
						"vec3 ambient_PV = u_LA_PV * u_KA_PV;" \
						"vec3 diffuse_PV = u_LD_PV * u_KD_PV * max(dot(light_direction, transformed_normal), 0.0f);" \
						"vec3 specular_PV = u_LS_PV * u_KS_PV * pow(max(dot(reflection_vector, view_vector), 0.0f), u_material_shininess_PV);" \
						"phong_ads_light_PV = ambient_PV + diffuse_PV + specular_PV;" \
						"}" \
						"else" \
						"{" \
						"phong_ads_light_PV = vec3(1.0f, 1.0f, 1.0f);" \
						"}" \
						"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
						"}";
				}

				void getFragmentShader_PV(void)
				{
					//code
					//FRAGMENT SHADER FOR PER-VERTEX LIGHTING
					//create shader
					gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

					//provide source code to shader
					const GLchar* fragmentShaderSourceCode_PV =
						"#version 450 core" \
						"\n" \
						"in vec3 phong_ads_light_PV;" \
						"out vec4 FragColor;" \
						"void main(void)" \
						"{" \
						"FragColor = vec4(phong_ads_light_PV, 1.0f);" \
						"}";

				}


			2. //PER FRAGMENT SHADER
				
				void getVertexShader_PF(void)
				{
					//code
					//VERTEX SHADER
					//create shader
					gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

					//provide source code to shader
					//use 'core' profile i.e PROGRAMMABLE PIPELINE
					const GLchar* vertexShaderSourceCode_PF =
						"#version 450 core" \
						"\n" \
						"in vec4 vPosition;" \
						"in vec3 vNormal;" \
						"uniform mat4 u_model_matrix;" \
						"uniform mat4 u_view_matrix;" \
						"uniform mat4 u_projection_matrix;" \
						"uniform int u_L_KEY_PRESSED;" \
						"uniform vec4 u_light_position_PF;" \
						"out vec3 transformed_normal;" \
						"out vec3 light_direction;" \
						"out vec3 view_vector;" \
						"void main(void)" \
						"{" \
						"if(u_L_KEY_PRESSED == 1)" \
						"{" \
						"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
						"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
						"light_direction = vec3(u_light_position_PF - eye_coordinates);" \
						//SWIZZLING
						"view_vector = -eye_coordinates.xyz;" \
						"}" \
						"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
						"}";

					
				}

				void getFragmentShader_PF(void) 
				{
					//code
					//FRAGMENT SHADER
					//create shader
					gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

					//provide source code to shader
					const GLchar* fragmentShaderSourceCode_PF =
						"#version 450 core" \
						"\n" \
						"in vec3 transformed_normal;" \
						"in vec3 light_direction;" \
						"in vec3 view_vector;" \
						"uniform int u_L_KEY_PRESSED;" \
						"uniform vec3 u_LA_PF;" \
						"uniform vec3 u_LD_PF;" \
						"uniform vec3 u_LS_PF;" \
						"uniform vec3 u_KA_PF;" \
						"uniform vec3 u_KD_PF;" \
						"uniform vec3 u_KS_PF;" \
						"uniform float u_material_shininess_PF;" \
						"out vec4 FragColor;" \
						"void main(void)" \
						"{" \
						"vec3 phong_ads_light_PF;" \
						"if(u_L_KEY_PRESSED == 1)" \
						"{" \
						"vec3 normalized_transformed_normal = normalize(transformed_normal);" \
						"vec3 normalized_light_direction = normalize(light_direction);" \
						"vec3 normalized_view_vector = normalize(view_vector);" \
						"vec3 ambient_PF = u_LA_PF * u_KA_PF;" \
						"vec3 diffuse_PF = u_LD_PF * u_KD_PF * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);" \
						"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);" \
						"vec3 specular_PF = u_LS_PF * u_KS_PF * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_material_shininess_PF);" \
						"phong_ads_light_PF = ambient_PF + diffuse_PF + specular_PF;" \
						"}" \
						"else" \
						"{" \
						"phong_ads_light_PF = vec3(1.0f,1.0f,1.0f);" \
						"}" \
						"FragColor = vec4(phong_ads_light_PF, 1.0f);" \
						"}";

				}

				
			3. //get Uniform Location 
				gModelMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_model_matrix");
				gViewMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_view_matrix");
				gProjectionMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_projection_matrix");
				LKeyPressedUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_L_KEY_PRESSED");
				//for PER-VERTEX LIGHTING
				LaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LA_PV");
				LdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LD_PV");
				LsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_LS_PV");
				lightPositionUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_PV");
				KaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KA_PV");
				KdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KD_PV");
				KsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_KS_PV");
				materialShininessUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_material_shininess_PV");

				//---------------------------------------------------------------------------------------------//

				//get Uniform Location for PER-FRAGMENT LIGHTING
				gModelMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_model_matrix");
				gViewMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_view_matrix");
				gProjectionMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_projection_matrix");
				//for PER-FRAGMENT LIGHTING
				LKeyPressedUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_L_KEY_PRESSED");
				LaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LA_PF");
				LdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LD_PF");
				LsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_LS_PF");
				lightPositionUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF");
				KaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KA_PF");
				KdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KD_PF");
				KsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_KS_PF");
				materialShininessUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_material_shininess_PF");


5. Display() - Chnages are as follows: 

			.
			.
			.
			glUseProgram(gShaderProgramObject_PV); //using Per-vertex by default

			//USING PER-VERTEX LIGHITNG
			if (VKeyIsPressed == 1)
			{
				//glUseProgram(gShaderProgramObject_PV);

				glUniform4fv(lightPositionUniform_PV, 1, (GLfloat*)lightPosition);
				if (gbLights == true)
				{
					glUniform1i(LKeyPressedUniform_PV, 1);
					glUniform3fv(LaUniform_PV, 1, (GLfloat*)lightAmbient);
					glUniform3fv(LdUniform_PV, 1, (GLfloat*)lightDiffuse);
					glUniform3fv(LsUniform_PV, 1, (GLfloat*)lightSpecular);
					glUniform3fv(KaUniform_PV, 1, (GLfloat*)materialAmbient);
					glUniform3fv(KdUniform_PV, 1, (GLfloat*)materialDiffuse);
					glUniform3fv(KsUniform_PV, 1, (GLfloat*)materialSpecular);
					glUniform1f(materialShininessUniform_PV, materialShininess);
					
				}
				else
				{
					glUniform1i(LKeyPressedUniform_PV, 0);

				}

				//sending MODEL, VIEW, PROJECTION Uniforms separately.
				glUniformMatrix4fv(gModelMatrixUniform_PV, 1, GL_FALSE, translationMatrix); //translation and model are same
				glUniformMatrix4fv(gViewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
				glUniformMatrix4fv(gProjectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
			}

			//USING PER-FRAGMENT LIGHITNG
			else if (FKeyIsPressed == 1)
			{
				//start using OpenGL program object
				glUseProgram(gShaderProgramObject_PF);

				glUniform4fv(lightPositionUniform_PF, 1, (GLfloat*)lightPosition);
				if (gbLights == true)
				{
					glUniform1i(LKeyPressedUniform_PF, 1);
					glUniform3fv(LaUniform_PF, 1, (GLfloat*)lightAmbient);
					glUniform3fv(LdUniform_PF, 1, (GLfloat*)lightDiffuse);
					glUniform3fv(LsUniform_PF, 1, (GLfloat*)lightSpecular);
					glUniform3fv(KaUniform_PF, 1, (GLfloat*)materialAmbient);
					glUniform3fv(KdUniform_PF, 1, (GLfloat*)materialDiffuse);
					glUniform3fv(KsUniform_PF, 1, (GLfloat*)materialSpecular);
					glUniform1f(materialShininessUniform_PF, materialShininess);
				}
				else
				{
					glUniform1i(LKeyPressedUniform_PF, 0);
				}

				//sending MODEL, VIEW, PROJECTION Uniforms separately.
				glUniformMatrix4fv(gModelMatrixUniform_PF, 1, GL_FALSE, translationMatrix); //translation and model are same
				glUniformMatrix4fv(gViewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
				glUniformMatrix4fv(gProjectionMatrixUniform_PF, 1, GL_FALSE, projectionMatrix);

			}

			else
			{
				//PER-VERTEX SHOULD BE ENABLED BY DEFAULT - HENCE, WRTING OUT OF IF-ELSE BLOCK
				//sending MODEL, VIEW, PROJECTION Uniforms separately.
				glUniformMatrix4fv(gModelMatrixUniform_PV, 1, GL_FALSE, translationMatrix); //translation and model are same
				glUniformMatrix4fv(gViewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
				glUniformMatrix4fv(gProjectionMatrixUniform_PV, 1, GL_FALSE, projectionMatrix);
			}		

			.
			.
			.

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
