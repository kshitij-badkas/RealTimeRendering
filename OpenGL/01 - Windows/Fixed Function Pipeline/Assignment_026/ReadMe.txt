/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Graph_Paper
------------------------------------------------------------------------------------------------------
Assignment Number 		: 026
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_026_Graph_Paper
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Graph_Paper.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW 40 BLUE VERTICAL & HORIZONTAL LINES, ONE GREEN VERTICAL LINE 
///			AND ONE RED HORIZONTAL LINE AT THE CENTRE OF THE SCREEN
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : No Changes

2. WinMain  : Changes are as follows:
			1. 	In Display -
				1. 	float x_coord; 
					float y_coord; 
					float inc = 0.05f; 
				
				2. Draw 40 Blue lines parallel to Y Axis and One Green line Vertically -
					for (x_coord = inc; x_coord <= 1.05f; x_coord = x_coord + inc)

				3. Draw 40 Blue line parallel to X Asix and one Red line Horizonatlly -
					for (y_coord = inc; y_coord <= 1.05f; y_coord = y_coord + inc)

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
