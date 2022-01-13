/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Pyramid_3D
------------------------------------------------------------------------------------------------------
Assignment Number 		: 039
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_039_Pyramid_3D
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Pyramid_3D.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW  3D-TRIANGLE (PYRAMID) WITH ROTATION. 
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : No Changes

2. WinMain  : Changes are as follows:
			1. 	In Initialise()- Enable 3D using the following functions -
				1.	pfd.cDepthBits = 32;
				2. 	glShadeModel(GL_SMOOTH);	//anti-aliasing
				4. 	glClearDepth(1.0f);		//Default value passed to the Function
				5. 	glEnable(GL_DEPTH_TEST);
				6. 	glDepthFunc(GL_LEQUAL);
				7.	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);



			2. 	In Display() -
				1. 	Draw a 3D Triangle using GL_TRIANGLES and Rotate it along Y-Axis -
				2. 	Using the Z-axis coordinates of the Triangle. eg. - glVertex3f(-1.0f, -1.0f, 1.0f);
					Define coordinates of Front, Right, Back,Left.
					Use Scaling to reduce its size.
					1. glRotatef(tAngle, 0.0f, 1.0f, 0.0f);
					2. glScalef(0.7f, 0.7f, 0.7f);
					

3. WndProc  : No Changes

					
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
