//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Per Vertex Light on Shpere 
------------------------------------------------------------------------------------------------------
Assignment Number 		: 013
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_013_Per_Vertex_Light_Shading
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Per_Vertex_Light_Shading.cpp, Headers.h, vmath.h, Sphere.h, Sphere.lib, Resources.rc, KAB.ico
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
					"uniform vec3 u_LA;" \
					"uniform vec3 u_LD;" \
					"uniform vec3 u_LS;" \
					"uniform vec4 u_light_position;" \
					"uniform vec3 u_KA;" \
					"uniform vec3 u_KD;" \
					"uniform vec3 u_KS;" \
					"uniform float u_material_shininess;" \
					"out vec3 phong_ads_light;" \
					"void main(void)" \
					"{" \
						"if(u_L_KEY_PRESSED == 1)" \
						"{" \
							"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" \
							"vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
							"vec3 light_direction = normalize(vec3(u_light_position - eye_coordinates));" \
							"vec3 reflection_vector = reflect(-light_direction, transformed_normal);" \
							//SWIZZLING
							"vec3 view_vector = normalize(-eye_coordinates.xyz);" \
							"vec3 ambient = u_LA * u_KA;" \
							"vec3 diffuse = u_LD * u_KD * max(dot(light_direction, transformed_normal), 0.0f);" \
							"vec3 specular = u_LS * u_KS * pow(max(dot(reflection_vector, view_vector), 0.0f), u_material_shininess);" \
							"phong_ads_light = ambient + diffuse + specular;" \
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

			3. // element vbo of sphere
				glGenBuffers(1, &gVbo_sphere_element);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			4.  //get Uniform Location
				gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
				gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
				gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

				LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_L_KEY_PRESSED");
				LaUniform = glGetUniformLocation(gShaderProgramObject, "u_LA");
				LdUniform = glGetUniformLocation(gShaderProgramObject, "u_LD");
				LsUniform = glGetUniformLocation(gShaderProgramObject, "u_LS");
				lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
				KaUniform = glGetUniformLocation(gShaderProgramObject, "u_KA");
				KdUniform = glGetUniformLocation(gShaderProgramObject, "u_KD");
				KsUniform = glGetUniformLocation(gShaderProgramObject, "u_KS");
				materialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

				//vertices, colours, shader attribs, vbo, vao initializations
				getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
				gNumVertices = getNumberOfSphereVertices();
				gNumElements = getNumberOfSphereElements();
4. Display(): 	

void Display(void)
{
	//variables
	//ALBEDO
	GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f ,1.0f };
	GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f ,1.0f };
	GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f ,1.0f };
	GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

	GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f ,1.0f };
	GLfloat materialDiffuse[] = { 0.5f, 0.2f, 0.7f ,1.0f };
	GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f ,1.0f };
	GLfloat materialShininess = 128.0f;
	

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);
	if (gbLights == true)
	{
		glUniform1i(LKeyPressedUniform, 1);
		glUniform3fv(LaUniform, 1, (GLfloat*)lightAmbient);
		glUniform3fv(LdUniform, 1, (GLfloat*)lightDiffuse);
		glUniform3fv(LsUniform, 1, (GLfloat*)lightSpecular);
		glUniform3fv(KaUniform, 1, (GLfloat*)materialAmbient);
		glUniform3fv(KdUniform, 1, (GLfloat*)materialDiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)materialSpecular);
		glUniform1f(materialShininessUniform, materialShininess);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	//OpenGL Drawing
	//new
	mat4 translationMatrix = mat4::identity(); //model
	mat4 viewMatrix = mat4::identity(); //view
	mat4 projectionMatrix = mat4::identity(); //projection

	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	//viewMatrix must be identity
	//viewMatrix is assigned to identity above 
	//hence, not writing again
	projectionMatrix = gPerspectiveProjectionMatrix;

	//sending MODEL, VIEW, PROJECTION Uniforms separately.
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, translationMatrix); //translation and model are same
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

	//bind vao - vertex array object
	glBindVertexArray(gVao_sphere);

	//draw
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//unbind vao
	glBindVertexArray(0);

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
