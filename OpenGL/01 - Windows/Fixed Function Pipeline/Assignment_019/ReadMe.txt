/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: 2D_Animation
------------------------------------------------------------------------------------------------------
Assignment Number 		: 019	
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_019_2D_Animation
------------------------------------------------------------------------------------------------------
Created New File(s) 	: 2D_Animation.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW ANIMATED MULTICOLOURED TRIANGLE AND BLUE RECTANGLE IN 2D 
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : No Changes

2. WinMain  : Changes are as follows:
			1. In Display -
				Local Variables for applying rotating logic 	
				1. static GLfloat tAngle = 0.0f;
				2. static GLfloat rAngle = 0.0f;

				Rotating logic
				3. glRotatef(tAngle, 0.0f, 1.0f, 0.0f); - Y Axis
				4. glRotatef(rAngle, 1.0f, 0.0f, 0.0f);	- X Axis

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
