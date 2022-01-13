//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Plain Sphere 
------------------------------------------------------------------------------------------------------
Assignment Number 		: 012
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_012_White_Sphere
------------------------------------------------------------------------------------------------------
Created New File(s) 	: White_Sphere.cpp, Headers.h, vmath.h, Sphere.h, Sphere.lib, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			1. Header - #include "Sphere.h"

			2. pragma -#pragma comment(lib,"Sphere.lib")

			3. Global variables -

				GLuint gVao_sphere;
				GLuint gVbo_sphere_position;
				GLuint gVbo_sphere_normal;
				GLuint gVbo_sphere_element;

				float sphere_vertices[1146];
				float sphere_normals[1146];
				float sphere_textures[764];
				unsigned short sphere_elements[2280];
				GLuint gNumVertices;
				GLuint gNumElements; //Elements a.k.a Indices

2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE

4. Initialize  : Changes are as follows:
			
			1. 	//VERTEX SHADER
				

			2. //FRAGMENT SHADER
			

			3. // element vbo of sphere
				glGenBuffers(1, &gVbo_sphere_element);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

4. Display(): 	

			glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

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
