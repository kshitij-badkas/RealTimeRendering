/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Static_Smiley
------------------------------------------------------------------------------------------------------
Assignment Number 		: 007
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_007_Static_Smiley
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Static_Smiley.cpp, Smiley.bmp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW A SMILEY IMAGE AS TEXTURE
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows : 
			  
			  1. #include<SOIL/SOIL.h>
			  
			  2. GLuint smiley_texture;

2. main()  : NO CHANGE.

4. Initialize() : Changes are as follows :
					1. --> Adding Texture Code
							
						//texture
						GLuint LoadBitmapAsTexture(const char *);
						.
						.
						//texture
						glEnable(GL_TEXTURE_2D);
						smiley_texture = LoadBitmapAsTexture("Smiley.bmp");
						
5. display() : Changes are as follows :
				1. --> void display(void)
						{
							.
							.
							glBegin(GL_QUADS);
							glTexCoord2f(1.0f, 0.0f);
							glVertex3f(1.0f, 1.0f, 0.0f);

							glTexCoord2f(0.0f, 0.0f);
							glVertex3f(-1.0f, 1.0f, 0.0f);

							glTexCoord2f(0.0f, 1.0f);
							glVertex3f(-1.0f, -1.0f, 0.0f);

							glTexCoord2f(1.0f, 1.0f);
							glVertex3f(1.0f, -1.0f, 0.0f);
							glEnd();
							
						}
						
					
						
						
