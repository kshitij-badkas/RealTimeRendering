/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Light_Cube
------------------------------------------------------------------------------------------------------
Assignment Number 		: 053
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_053_Light_Cube
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Light_Cube.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW ALL AXES ROTATING CUBE WITH LIGHT
///		2. LIGHT TOGGLE ON USER-INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				
				bool gbLight = false;
				GLfloat lightAmbient[] = { 0.2f,0.5f,0.7f,1.0f }; //Grey Light
				GLfloat lightDiffused[] = { 1.0f,1.0f,1.0f,1.0f }; //White Light
				GLfloat lightPosition[] = { 0.0f,0.0f,2.0f,1.0f }; //Z-Axis
 

2. WinMain  : Changes are as follows:
			1. In Initialize -
				
				//Lighting Code
				glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
				glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffused);
				glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
				glEnable(GL_LIGHT1);

			
3. WndProc  : Chnages as follows:
			1.  case 'l':
				case 'L':
					if (gbLight == false)
					{
						gbLight = true;
						glEnable(GL_LIGHTING);
					}
					else
					{
						if (gbLight == true)
						{
							gbLight = false;
							glDisable(GL_LIGHTING);
						}
						
					}
					break;

4. Display(): Changes as follows:

				.
				.
				glRotatef(rAngle, 1.0f, 1.0f, 1.0f);
				glScalef(0.5f, 0.5f, 0.5f);

				glBegin(GL_QUADS);
				
				//FRONT
				glNormal3f(0.0f, 0.0f, 1.0f);
				glVertex3f(1.0f, 1.0f, 1.0f);
				glVertex3f(-1.0f, 1.0f, 1.0f);
				glVertex3f(-1.0f, -1.0f, 1.0f);
				glVertex3f(1.0f, -1.0f, 1.0f);

				//RIGHT
				glNormal3f(1.0f, 0.0f, 0.0f);
				glVertex3f(1.0f, 1.0f, -1.0f);
				glVertex3f(1.0f, 1.0f, 1.0f);
				glVertex3f(1.0f, -1.0f, 1.0f);
				glVertex3f(1.0f, -1.0f, -1.0f);

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
