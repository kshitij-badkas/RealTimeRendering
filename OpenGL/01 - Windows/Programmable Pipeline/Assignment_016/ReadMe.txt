//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Per Fragment Light on Shpere 
------------------------------------------------------------------------------------------------------
Assignment Number 		: 014
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_014_Per_Fragment_Light_Shading
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Per_Fragment_Light_Shading.cpp, Headers.h, 
						vmath.h, Sphere.h, Sphere.lib, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. 
				GLuint LKeyPressedUniform; //L key is pressed
				//MODEL-VIEW-PROJECTION UNIFORMS
				GLuint gModelMatrixUniform;
				GLuint gViewMatrixUniform;
				GLuint gProjectionMatrixUniform;

				//LIGHT UNIFORMS - ALL ARRAYS
				GLuint LaUniform; //ambient light component
				GLuint LdUniform; //diffuse light component
				GLuint LsUniform; //specular light component
				GLuint lightPositionUniform; //light position

				//MATERIAL UNIFORMS - Ka, Kd, Ks ARRAYS, shininess is a single value
				GLuint KaUniform; //ambient material component
				GLuint KdUniform; //diffuse material component
				GLuint KsUniform; //specular material component
				GLuint materialShininessUniform; //shininess material

2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE

4. Initialize  : Changes are as follows:
			
			1. 	//VERTEX SHADER
				const GLchar* vertexShaderSourceCode =
					"#version 450 core" \
					"\n" \
					"in vec4 vPosition;" \
					"in vec3 vNormal;" \
					"uniform mat4 u_model_matrix;" \
					"uniform mat4 u_view_matrix;" \
					"uniform mat4 u_projection_matrix;" \
					"uniform int u_L_KEY_PRESSED;" \
					"uniform vec4 u_light_position;" \
					"out vec3 transformed_normal;" \
					"out vec3 light_direction;" \
					"out vec3 view_vector;" \
					"void main(void)" \
					"{" \
					"if(u_L_KEY_PRESSED == 1)" \
					"{" \
					"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
					"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
					"light_direction = vec3(u_light_position - eye_coordinates);" \
					//SWIZZLING
					"view_vector = -eye_coordinates.xyz;" \
					
					"}" \
					"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
					"}";

			2. //FRAGMENT SHADER
				const GLchar* fragmentShaderSourceCode =
					"#version 450 core" \
					"\n" \
					"in vec3 transformed_normal;" \
					"in vec3 light_direction;" \
					"in vec3 view_vector;" \
					"uniform int u_L_KEY_PRESSED;" \
					"uniform vec3 u_LA;" \
					"uniform vec3 u_LD;" \
					"uniform vec3 u_LS;" \
					"uniform vec3 u_KA;" \
					"uniform vec3 u_KD;" \
					"uniform vec3 u_KS;" \
					"uniform float u_material_shininess;" \
					"out vec4 FragColor;" \
					"void main(void)" \
					"{" \
					"vec3 phong_ads_light;" \
					"if(u_L_KEY_PRESSED == 1)" \
					"{" \
					"vec3 normalized_transformed_normal = normalize(transformed_normal);" \
					"vec3 normalized_light_direction = normalize(light_direction);" \
					"vec3 normalized_view_vector = normalize(view_vector);" \
					"vec3 ambient = u_LA * u_KA;" \
					"vec3 diffuse = u_LD * u_KD * max(dot(normalized_light_direction, normalized_transformed_normal), 0.0f);" \
					"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normal);" \
					"vec3 specular = u_LS * u_KS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0f), u_material_shininess);" \
					"phong_ads_light = ambient + diffuse + specular;" \
					"}" \
					"else" \
					"{" \
					"phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
					"}" \
					"FragColor = vec4(phong_ads_light, 1.0f);" \
					"}";

			


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
