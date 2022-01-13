/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Three_Rotating_Lights
------------------------------------------------------------------------------------------------------
Assignment Number 		: 056
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_056_Three_Rotating_Lights
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Three_Rotating_Lights.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW A SPHERE WITH THREE ROTATING LIGHTS
///		2. LIGHT TOGGLE ON USER-INTERACTION.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				
				bool gbLight = false;

				GLfloat lightAmbient_Zero[] = { 0.0f,0.0f,0.0f,1.0f };
				GLfloat lightDiffuse_Zero[] = { 1.0f,0.0f,0.0f,1.0f }; //Red
				GLfloat lightSpecular_Zero[] = { 1.0f,0.0f,0.0f,1.0f }; //Red
				GLfloat lightPosition_Zero[] = { 0.0f,0.0f,0.0f,1.0f }; //Positional Light

				GLfloat lightAmbient_One[] = { 0.0f,0.0f,0.0f,1.0f };
				GLfloat lightDiffuse_One[] = { 0.0f,1.0f,0.0f,1.0f }; //Green
				GLfloat lightSpecular_One[] = { 0.0f,1.0f,0.0f,1.0f }; //Green
				GLfloat lightPosition_One[] = { 0.0f,0.0f,0.0f,1.0f }; //Positional Light

				GLfloat lightAmbient_Two[] = { 0.0f,0.0f,0.0f,1.0f };
				GLfloat lightDiffuse_Two[] = { 0.0f,0.0f,1.0f,1.0f }; //Blue
				GLfloat lightSpecular_Two[] = { 0.0f,0.0f,1.0f,1.0f }; //Blue
				GLfloat lightPosition_Two[] = { 0.0f,0.0f,0.0f,1.0f }; //Positional Light

				GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
				GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f }; //White Material
				GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
				GLfloat materialShininess = 128.0f;

				GLfloat lightAngle0 = 0.0f;
				GLfloat lightAngle1 = 0.0f;
				GLfloat lightAngle2 = 0.0f;

				GLUquadric* quadric = NULL;
 

2. WinMain  : Changes are as follows:
			1. In Initialize -
				
				//Initialization of light
				glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient_Zero);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse_Zero);
				glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular_Zero);
				glEnable(GL_LIGHT0);

				glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient_One);
				glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse_One);
				glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular_One);
				glEnable(GL_LIGHT1);

				glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient_Two);
				glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse_Two);
				glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular_Two);
				glEnable(GL_LIGHT2);

				//Material Initialization
				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
				glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
				glEnable(GL_LIGHT1);

			
3. WndProc  : NO CHANGE

4. Display(): Changes as follows:

				.
				.
				glLoadIdentity();
				glPushMatrix();
				
				gluLookAt(0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
				glPushMatrix();

				glRotatef(lightAngle0, 1.0f, 0.0f, 0.0f);
				lightPosition_Zero[1] = lightAngle0;
				glLightfv(GL_LIGHT0, GL_POSITION, lightPosition_Zero);
				glPopMatrix();
				glPushMatrix();

				glRotatef(lightAngle1, 0.0f, 1.0f, 0.0f);
				lightPosition_One[0] = lightAngle1;
				glLightfv(GL_LIGHT1, GL_POSITION, lightPosition_One);
				glPopMatrix();
				glPushMatrix();

				glRotatef(lightAngle2, 0.0f, 0.0f, 1.0f);
				lightPosition_Two[0] = lightAngle2;
				glLightfv(GL_LIGHT2, GL_POSITION, lightPosition_Two);
				glPopMatrix();

				glTranslatef(0.0f, 0.0f, -1.0f);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				quadric = gluNewQuadric();
				gluSphere(quadric, 0.5f, 30, 30);
				glPopMatrix();
				//Normals are calculated using gluSphere itself so we do not need glBegin and glEnd

				lightAngle0 = lightAngle0 + 0.5f;
				if (lightAngle0 >= 360.0f)
				{
					lightAngle0 = 0.5f;
				}

				lightAngle1 = lightAngle1 + 0.5f;
				if (lightAngle1 >= 360.0f)
				{
					lightAngle1 = 0.5f;
				}

				lightAngle2 = lightAngle2 + 0.5f;
				if (lightAngle2 >= 360.0f)
				{
					lightAngle2 = 0.5f;
				}

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
