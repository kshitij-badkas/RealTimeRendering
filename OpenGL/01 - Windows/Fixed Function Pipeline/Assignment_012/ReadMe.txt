/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Transformation
------------------------------------------------------------------------------------------------------
Assignment Number 		: 012	
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_012_Transformation
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Transformation.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW MULTICOLOURED-TRIANGLE, WHITE TRIANGLE AND WHITE RECTANGLE USING 
///			MATRIX TRANSFORMATION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : No Changes

2. WinMain  : Changes are as follows:
				1. in Display()
					1. Draw Triangle using - 
						glClear(GL_COLOR_BUFFER_BIT);
						glMatrixMode(GL_MODELVIEW);
						glLoadIdentity();
						glTranslatef();
						glBegin();
						glVertex3f();

					2. Similarly draw other shapes and position them using Translation

3. WndProc  : No Changes.

					
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
