/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Light_Material_Sphere
------------------------------------------------------------------------------------------------------
Assignment Number 		: 054
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_054_Light_Material_Sphere
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Light_Material_Sphere.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW A SPHERE WITH LIGHT AND MATERIAL.
///		2. LIGHT TOGGLE ON USER-INTERACTION.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				
				bool gbLight = false;

				GLfloat lightAmbient[] = { 0.1f,0.1f,0.1f,1.0f }; //
				GLfloat lightDiffused[] = { 1.0f,1.0f,1.0f,1.0f }; //White Light
				GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
				GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f }; //Z-Axis

				GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
				GLfloat materialDiffused[] = { 0.5f,0.2f,0.7f,1.0f };
				GLfloat materialSpecular[] = { 0.7f,0.7f,0.7f,1.0f };
				GLfloat materialShininess = 128.0f;

				GLUquadric* quadric = NULL;
 

2. WinMain  : Changes are as follows:
			1. In Initialize -
				
				//Lighting Code
				glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
				glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffused);
				glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
				glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffused);
				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
				glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
				glEnable(GL_LIGHT1);

			
3. WndProc  : NO CHANGE

4. Display(): Changes as follows:

				.
				.
				glLoadIdentity();
				glTranslatef(0.0f, 0.0f, -0.55f);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				quadric = gluNewQuadric();
				gluSphere(quadric, 0.2f, 30, 30);


				.
				.

--------------------------------------------------------------------------------------------------------
Following are the contents of 'MyIcon.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101


--------------------------------------------------------------------------------------------------------
Following are the contents of 'MyIcon.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "Headers.h"
MYICON ICON KAB.ico
