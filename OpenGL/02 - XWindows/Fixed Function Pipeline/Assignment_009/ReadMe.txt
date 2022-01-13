/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Checkered_Board
------------------------------------------------------------------------------------------------------
Assignment Number 		: 009
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_009_Checkered_Board
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Checkered_Board.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW CHECKERED BOARD TEXTURE
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows : 
			  
			  1. 			  
				#define CHECK_IMAGE_WIDTH 64
				#define CHECK_IMAGE_HEIGHT 64
			
			  2. //Textures
				GLuint tex_image; //texture
				GLubyte check_image[CHECK_IMAGE_WIDTH][CHECK_IMAGE_HEIGHT][4];

2. main()  : NO CHANGE

3. Initialize() : Changes are as follows :
					1. --> 
						//texture
						LoadBitmapAsTexture();
						glEnable(GL_TEXTURE_2D);

4. Resize () : Changes are as follows : 

				gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 0.1f, 30.0f); 										
5. void MakeCheckImage(void)
{
	//code
	int i, j, c;
	for (i = 0; i < CHECK_IMAGE_HEIGHT; i++)
	{
		for (j = 0; j < CHECK_IMAGE_WIDTH; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			check_image[i][j][0] = (GLubyte)c;
			check_image[i][j][1] = (GLubyte)c;
			check_image[i][j][2] = (GLubyte)c;
			check_image[i][j][3] = 255;
		}
	}
}

