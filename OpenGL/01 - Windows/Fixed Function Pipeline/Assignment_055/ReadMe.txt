/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Two_Lights_Pyramid
------------------------------------------------------------------------------------------------------
Assignment Number 		: 055
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_055_Two_Lights_Pyramid
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Two_Lights_Pyramid.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW SPINNING PYRAMID WITH TWO LIGHTS
///		2. LIGHT TOGGLE ON USER-INTERACTION.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				
				bool gbLight = false;

				//LIGHT ->RED
				GLfloat lightAmbient_0[] = { 0.0f,0.0f,0.0f,0.0f };
				GLfloat lightDiffused_0[] = { 1.0f,0.0f,0.0f,1.0f };
				GLfloat lightSpecular_0[] = { 1.0f,0.0f,0.0f,1.0f };
				GLfloat lightPosition_0[] = { 2.0f,0.0f,0.0f,1.0f }; //positional-light

				//LIGHT ->BLUE
				GLfloat lightAmbient_1[] = { 0.0f,0.0f,0.0f,0.0f };
				GLfloat lightDiffused_1[] = { 0.0f,0.0f,1.0f,1.0f };
				GLfloat lightSpecular_1[] = { 0.0f,0.0f,1.0f,1.0f };
				GLfloat lightPosition_1[] = { -2.0f,0.0f,0.0f,1.0f }; //positional light

				//MATERIAL ->WHITE
				GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
				GLfloat materialDiffused[] = { 1.0f,1.0f,1.0f,1.0f };
				GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
				GLfloat materialShininess = 50.0f;
 

2. WinMain  : Changes are as follows:
			1. In Initialize -
				
				//Lighting Code
				glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient_0);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffused_0);
				glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular_0);
				glLightfv(GL_LIGHT0, GL_POSITION, lightPosition_0);

				glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient_1);
				glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffused_1);
				glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular_1);
				glLightfv(GL_LIGHT1, GL_POSITION, lightPosition_1);
				
				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffused);
				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
				glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);

				glEnable(GL_LIGHT0);
				glEnable(GL_LIGHT1);

			
3. WndProc  : NO CHANGE

4. Display(): Changes as follows:

				.
				.
				glLoadIdentity();
				.
				.
				//FRONT
				glNormal3f(0.0f, 0.447214f, 0.894427f); // front-top
				glVertex3f(0.0f, 1.0f, 0.0f);

				glNormal3f(0.0f, 0.447214f, 0.894427f); // front-left)
				glVertex3f(-1.0f, -1.0f, 1.0f);

				glNormal3f(0.0f, 0.447214f, 0.894427f); // front-right
				glVertex3f(1.0f, -1.0f, 1.0f);

				//RIGHT
				glNormal3f(0.894427f, 0.447214f, 0.0f); // right-top);
				glVertex3f(0.0f, 1.0f, 0.0f);

				glNormal3f(0.894427f, 0.447214f, 0.0f); // right-left);	
				glVertex3f(1.0f, -1.0f, 1.0f);

				glNormal3f(0.894427f, 0.447214f, 0.0f); // right-right);	
				glVertex3f(1.0f, -1.0f, -1.0f);

				//BACK
				glNormal3f(0.0f, 0.447214f, -0.894427f); // back-top	
				glVertex3f(0.0f, 1.0f, 0.0f);

				glNormal3f(0.0f, 0.447214f, -0.894427f); // back-left	
				glVertex3f(1.0f, -1.0f, -1.0f);

				glNormal3f(0.0f, 0.447214f, -0.894427f); // back-right	
				glVertex3f(-1.0f, -1.0f, -1.0f);

				//LEFT
				glNormal3f(-0.894427f, 0.447214f, 0.0f); // left-top)
				glVertex3f(0.0f, 1.0f, 0.0f);

				glNormal3f(-0.894427f, 0.447214f, 0.0f); // left-top)
				glVertex3f(-1.0f, -1.0f, -1.0f);

				glNormal3f(-0.894427f, 0.447214f, 0.0f); // left-top)
				glVertex3f(-1.0f, -1.0f, 1.0f);

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
