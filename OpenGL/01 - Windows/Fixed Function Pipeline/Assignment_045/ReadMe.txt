/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Textured_Pyramid
------------------------------------------------------------------------------------------------------
Assignment Number 		: 045
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_045_Textured_Pyramid
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Textured_Pyramid.cpp, MyIcon.h, MyIcon.rc, KAB.ico, Stone.bmp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW AN ANIMTATED TEXTURED 3-D PYRAMID.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				1. GLuint stone_texture;

2. WinMain  : Changes are as follows:
			1. In Initialize -
				LoadGLTexture(&stone_texture, MAKEINTRESOURCE(STONE_BITMAP));

				Enable Textures - glEnable(GL_TEXTURE_2D);	
			
			2. New Function - 
				bool LoadGLTexture(GLuint * texture, TCHAR resourceID[])

			3. In Display -
				1. Bind Texture using glBindTexture()
				2. Set Texture coordinates using glTexCoord2f()

3. WndProc  : No Changes. 

					
--------------------------------------------------------------------------------------------------------
Following are the contents of 'MyIcon.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101
#define STONE_BITMAP 102


--------------------------------------------------------------------------------------------------------
Following are the contents of 'MyIcon.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "MyIcon.h"
MYICON ICON KAB.ico
STONE_BITMAP BITMAP Stone.bmp
