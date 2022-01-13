/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Multi_Viewport
------------------------------------------------------------------------------------------------------
Assignment Number 		: 017	
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_017_Multi_Viewport
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Multi_Viewport.cpp, MyIcon.h, MyIcon.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW MULTICOLOURED TRIANGLE THORUGH DIFFERENT VIEWPORTS ON USER-INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. 	Global variables
				int updated_width = WIN_WIDTH;
				int updated_height = WIN_HEIGHT;

2. WinMain  : Changes are as follows:
			1. 	Draw MultiColoured-Triangle with perspective projection.
			2.	In Resize - 
				updated_width = width;
				updated_height = height;

3. WndProc  : Changes are as follows:
			1. Added the following case-breaks in switch{}	
				
				case 48:
				case VK_NUMPAD0:
					glViewport(0, 0, GLsizei(updated_width), GLsizei(updated_height));
					break;

				case 49:
				case VK_NUMPAD1:
					glViewport(0, updated_height / 2, GLsizei(updated_width / 2), GLsizei(updated_height / 2));
					break;

				case 50:
				case VK_NUMPAD2:
					glViewport(updated_width / 2, updated_height / 2, GLsizei(updated_width / 2), GLsizei(updated_height / 2));
					break;

				case 51:
				case VK_NUMPAD3:
					glViewport(updated_width / 2, 0, GLsizei(updated_width / 2), GLsizei(updated_height / 2));
					break;

				case 52:
				case VK_NUMPAD4:
					glViewport(0, 0, GLsizei(updated_width / 2), GLsizei(updated_height / 2));
					break;

				case 53:
				case VK_NUMPAD5:
					glViewport(0, 0, GLsizei(updated_width / 2), GLsizei(updated_height));
					break;

				case 54:
				case VK_NUMPAD6:
					glViewport(updated_width / 2, 0, GLsizei(updated_width / 2), GLsizei(updated_height));
					break;

				case 55:
				case VK_NUMPAD7:
					glViewport(0, updated_height / 2, GLsizei(updated_width), GLsizei(updated_height / 2));
					break;

				case 56:
				case VK_NUMPAD8:
					glViewport(0, 0, GLsizei(updated_width), GLsizei(updated_height / 2));
					break;

				case 57:
				case VK_NUMPAD9:
					glViewport(updated_width / 4, updated_height / 4, GLsizei(updated_width / 2), GLsizei(updated_height / 2));
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
