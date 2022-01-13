/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Tweaked_Smiley
------------------------------------------------------------------------------------------------------
Assignment Number 		: 048
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_048_Tweaked_Smiley
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Tweaked_Smiley.cpp, Headers.h, Resources.rc, KAB.ico, Smiley.bmp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW DIFFERENT SMILEY IMAGES ON USER INPUT.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				1. GLuint smiley_texture;
				2. unsigned int pressed_key; 

2. WinMain  : Changes are as follows:
			1. In Initialize -
				LoadGLTexture(&smiley_texture, MAKEINTRESOURCE(SMILEY_BITMAP));

			3. In Display -
				1. Using if-else statments for 'pressed_key' values, draw Rectangle and apply texture.

3. WndProc  : Changes are as follows:
			1. Added case-breaks-

				case 49: //'1 is pressed
					pressed_key = 1;
					fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
					glEnable(GL_TEXTURE_2D);
					break;

				case 50: //'2' is pressed
					pressed_key = 2;
					fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
					glEnable(GL_TEXTURE_2D);
					break;

				case 51:
					//'3' is pressed
					pressed_key = 3;
					fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
					glEnable(GL_TEXTURE_2D);
					break;

				case 52:
					//'4' is pressed
					pressed_key = 4;
					fprintf(gpFile, "\n Pressed Digit = %d\n", pressed_key);
					glEnable(GL_TEXTURE_2D);
					break;

				default:
					pressed_key = wParam;
					fprintf(gpFile, "\n Pressed Digit = %d\n", wParam);
					glDisable(GL_TEXTURE_2D);
					break; 

					
--------------------------------------------------------------------------------------------------------
Following are the contents of 'Headers.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101
#define SMILEY_BITMAP 102


--------------------------------------------------------------------------------------------------------
Following are the contents of 'Resources.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "MyIcon.h"
MYICON ICON KAB.ico
SMILEY_BITMAP BITMAP Smiley.bmp
