/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Robotic_Arm
------------------------------------------------------------------------------------------------------
Assignment Number 		: 044
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_044_Robotic_Arm
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Robotic_Arm.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW A ROBOTIC ARM AND ANIMATE IT BY USER-DEFINED CONTROLS.
///		2. SHOW PARENT-CHILD RELATIONSHIP BETWEEN THE 'SHOULDER' AND 'ELBOW' USING PUSH-POP MATRIX LOGIC.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				1. int shoulder = 0;
				2. int elbow = 0;
				3. GLUquadric* quadric = NULL;

2. WinMain  : Changes are as follows:
			1. 	In Display -
				1.	Draw Shoulder and Elbow - 
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
					quadric = gluNewQuadric();
					gluSphere(quadric, 0.5f, 10, 10);

				2. Use glPushMatrix(); and glPopMatrix(); for parent-child relationship. 

3. WndProc  : Changes are as follows: 
			1. Added the following case-break statements -
				case WM_CHAR:
					switch (wParam)
					{
					case 'S': shoulder = (shoulder + 3) % 360;
						break;
					case 's': shoulder = (shoulder - 3) % 360;
						break;
					case 'E': elbow = (elbow + 3) % 360;
						break;
					case 'e': elbow = (elbow - 3) % 360;
						break;
					default:
						break;
					}
					break;

					
--------------------------------------------------------------------------------------------------------
Following are the contents of 'MyIcon.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101


--------------------------------------------------------------------------------------------------------
Following are the contents of 'MyIcon.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "MyIcon.h"
MYICON ICON KAB.ico
