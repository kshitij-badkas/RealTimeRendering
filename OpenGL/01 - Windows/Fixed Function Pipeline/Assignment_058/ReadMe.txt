/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Teapot
------------------------------------------------------------------------------------------------------
Assignment Number 		: 058
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_058_Teapot
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Teapot.cpp, Headers.h, Teapot.h, Resources.rc, marble.bmp, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW A SPINNING TEAPOT WITH TEXTURE AND LIGHT
///		2. ANIMATE, TEXTURE AND LIGHT TOGGLE ON USER-INTERACTION.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				
				bool gbTexture = false;
				bool gbLight = false;
				bool gbAnimate = false;

				GLfloat lightAmbient[] = { 1.0f,1.0f,1.0f,1.0f };
				GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
				GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
				GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

				GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
				GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
				GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
				GLfloat materialShininess = 50.0f;
 

2. WinMain  : Changes are as follows:
			1. In Initialize -
				
				//Loading Textures
				LoadGLTexture(&marble_texture, MAKEINTRESOURCE(MARBLE_BITMAP));
				
				//Lighting
				glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
				glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
				glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

				//Material
				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
				glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
				
				glEnable(GL_LIGHT0);

				//Material Initialization
				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
				glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
				glEnable(GL_LIGHT1);

			
3. WndProc  : Changes as follows:
			1. 
				case WM_CHAR:
				switch (wParam)
				{
				case 'l':
				case 'L': //LIGHT TOGGLE
					if (gbLight == false)
					{
						glEnable(GL_LIGHTING);
						gbLight = true;
					}
					else
						if (gbLight == true)
						{
							glDisable(GL_LIGHTING);
							gbLight = false;
						}
					
					break;
					
				case 't':
				case 'T': //TEXTURE TOGGLE
					if (gbTexture == false)
					{
						gbTexture = true;
						glEnable(GL_TEXTURE_2D);
					}
					else
						if (gbTexture == true)
						{
							gbTexture = false;
							glDisable(GL_TEXTURE_2D);
						}
					break;

				case 'a':
				case 'A': //ANIMATION TOGGLE
					if (gbAnimate == false)
					{
						gbAnimate = true;
						
					}
					else
						if (gbAnimate == true)
						{
							gbAnimate = false;
						}
					break;

4. Display(): Changes as follows:

				.
				.
				glRotatef(rotate, 0.0f, 1.0f, 0.0f);

				glBindTexture(GL_TEXTURE_2D, marble_texture);

				glBegin(GL_TRIANGLES);
				for (int i = 0; i < sizeof(face_indicies) / sizeof(face_indicies[0]); i++)
				{
					for (int j = 0; j < 3; j++)
					{
						int vi = face_indicies[i][j]; //vertices
						int ni = face_indicies[i][j + 3]; //normals
						int ti = face_indicies[i][j + 6]; //textures

						glNormal3f(normals[ni][0], normals[ni][1], normals[ni][2]);
						glTexCoord2f(textures[ti][0], textures[ti][1]);
						glVertex3f(vertices[vi][0], vertices[vi][1], vertices[vi][2]);
					}
				}

				if (gbAnimate == true)
				{
					rotate += 1.0f;
					if (rotate >= 360.f)
					{
						rotate = 0.0f;
					}
				}

				.
				.

--------------------------------------------------------------------------------------------------------
Following are the contents of 'Headers.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101


--------------------------------------------------------------------------------------------------------
Following are the contents of 'Resources.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "Headers.h"
MYICON ICON KAB.ico
