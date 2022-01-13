/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Textured_Pyramid_Cube
------------------------------------------------------------------------------------------------------
Assignment Number 		: 047
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_047_Textured_Pyramid_Cube
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Textured_Pyramid_Cube.cpp, MyIcon.h, MyIcon.rc, KAB.ico, Stone.bmp, Vijay_kundali.bmp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW AN ANIMTATED TEXTURED 3-D PYRAMID.
///		2. SHOW AN ANIMTATED TEXTURED 3-D CUBE.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
			1. Global variables -
				1. GLuint stone_texture;
				2. GLuint kundali_texture;

2. WinMain  : Changes are as follows:
			1. In Initialize -
				LoadGLTexture(&stone_texture, MAKEINTRESOURCE(STONE_BITMAP));
				LoadGLTexture(&kundali_texture, MAKEINTRESOURCE(KUNDALI_BITMAP));

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
#define KUNDALI_BITMAP 103


--------------------------------------------------------------------------------------------------------
Following are the contents of 'MyIcon.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "MyIcon.h"
MYICON ICON KAB.ico
STONE_BITMAP BITMAP Stone.bmp
KUNDALI_BITMAP BITMAP Vijay_Kundali.bmp
