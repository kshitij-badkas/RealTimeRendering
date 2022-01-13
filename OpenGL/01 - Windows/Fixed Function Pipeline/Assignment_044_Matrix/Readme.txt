/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Matrix
------------------------------------------------------------------------------------------------------
Assignment Number 		: 044
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_044_Matrix
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Matrix.cpp, Headers.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW A CUBE WITH ALL AXES ROTATION USING glLoadMatrixf() and glMultMatrixf()
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				#define _USE_MATH_DEFINES // FOR USING <math.h>
				#include <math.h>

2. WinMain  : NO CHANGE. 

3. WndProc  : NO CHANGE.

4. Initialize() : Changes are as follows:
			
			//	IDENTITY MATRIX
			identityMatrix[0] = 1.0f;
			identityMatrix[1] = 0.0f;
			identityMatrix[2] = 0.0f;
			identityMatrix[3] = 0.0f;
			identityMatrix[4] = 0.0f;
			identityMatrix[5] = 1.0f;
			identityMatrix[6] = 0.0f;
			identityMatrix[7] = 0.0f;
			identityMatrix[8] = 0.0f;
			identityMatrix[9] = 0.0f;
			identityMatrix[10] = 1.0f;
			identityMatrix[11] = 0.0f;
			identityMatrix[12] = 0.0f;
			identityMatrix[13] = 0.0f;
			identityMatrix[14] = 0.0f;
			identityMatrix[15] = 1.0f;

			//	TRANSLATION MATRIX - 0.0, 0.0, -6.0
			translationMatrix[0] = 1.0f;
			translationMatrix[1] = 0.0f;
			translationMatrix[2] = 0.0f;
			translationMatrix[3] = 0.0f;
			translationMatrix[4] = 0.0f;
			translationMatrix[5] = 1.0f;
			translationMatrix[6] = 0.0f;
			translationMatrix[7] = 0.0f;
			translationMatrix[8] = 0.0f;
			translationMatrix[9] = 0.0f;
			translationMatrix[10] = 1.0f;
			translationMatrix[11] = 0.0f;
			translationMatrix[12] = 0.0f;
			translationMatrix[13] = 0.0f;
			translationMatrix[14] = -4.0f;
			translationMatrix[15] = 1.0f;

			// SCALE MATRIX - 0.7, 0.7, 0.7
			scaleMatrix[0] = 0.7f;
			scaleMatrix[1] = 0.0f;
			scaleMatrix[2] = 0.0f;
			scaleMatrix[3] = 0.0f;
			scaleMatrix[4] = 0.0f;
			scaleMatrix[5] = 0.7f;
			scaleMatrix[6] = 0.0f;
			scaleMatrix[7] = 0.0f;
			scaleMatrix[8] = 0.0f;
			scaleMatrix[9] = 0.0f;
			scaleMatrix[10] = 0.7f;
			scaleMatrix[11] = 0.0f;
			scaleMatrix[12] = 0.0f;
			scaleMatrix[13] = 0.0f;
			scaleMatrix[14] = 0.0f;
			scaleMatrix[15] = 1.0f;

5. Display() : Chnages are as follows :
			1. 	.
				.
				glLoadMatrixf(identityMatrix);

				glMultMatrixf(translationMatrix);

				glMultMatrixf(scaleMatrix);		
				.
				.

			2. Define Rotation Matrices manually

			3. 	Convert Degress to Radians -
				angleCube_rad = angleCube * (M_PI / 180.0f);
	 
	--------------------------------------------------------------------------------------------------------
Following are the contents of 'Headers.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101


--------------------------------------------------------------------------------------------------------
Following are the contents of 'Resources.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "MyIcon.h"
MYICON ICON KAB.ico
