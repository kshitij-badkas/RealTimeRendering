/////////////////////////////////////////////////////////////////////////////////////////////////////
///																								  ///
///		                              ASSIGNMENT DETAILS	  									  ///
///																								  ///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Icon
------------------------------------------------------------------------------------------------------
Assignment Number 		: 008	
------------------------------------------------------------------------------------------------------
Base Assignment   		: Assignment_007
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_008_MyIcon
------------------------------------------------------------------------------------------------------
Created New File(s) 	: MyIcon.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///																								  ///
///		THIS APPLICATION WILL SHOW THE ICON (KAB.ico) ON THE WINDOW AND ON THE TASKBAR		      ///
///																								  ///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
				1. --> Added Header file
						#include "MyIcon.h"

2. WinMain  : Changes are as follows:
				1. --> Modified the Paramters of LoadIcon function in the initializtion of WNDCLASSEX members

						wndclass.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(MYICON));
						wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

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