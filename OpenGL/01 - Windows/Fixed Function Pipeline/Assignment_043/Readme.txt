/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Solar_System
------------------------------------------------------------------------------------------------------
Assignment Number 		: 043
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_043_Solar_System
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Solar_System.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW THE SUN AND ANIMATE THE EARTH AND MOON USING 3D-GEOMETRY..
///		2. SHOW PARENT-CHILD RELATIONSHIP AMONG THE SUN, EARTH AND MOON USING PUSH-POP MATRIX LOGIC.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				1. int day = 0;
				2. int year = 0;
				3. int moon = 0;
				4. GLUquadric* quadric = NULL;

2. WinMain  : Changes are as follows:
			1. 	In Display -
				1.	Draw Sphere by 
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
					quadric = gluNewQuadric(); 
					gluSphere(quadric, 0.75f, 30, 30);

				2. Animate the Earth and Moon using Rotation before Translation.

				3. Use glPushMatrix(); and glPopMatrix(); for parent-child relationship. 

3. WndProc  : Changes are as follows: 
			1. Added the following case-break statements -
				case WM_CHAR:
					switch (wParam)
					{
					case 'D': day = (day + 6) % 360;
						break;
					case 'd': day = (day - 6) % 360;
						break;
					case 'Y':
						year = (year + 1) % 360;
						moon = (moon + 3) % 360;
						break;
					case 'y':
						year = (year - 1) % 360;
						moon = (moon - 3) % 360;
						break;
					case 'M': moon = (moon + 3) % 360;
						break;
					case 'm': moon = (moon - 3) % 360;
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
