/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Teapot_Rotation
------------------------------------------------------------------------------------------------------
Assignment Number 		: 025
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_025_Teapot_Rotation
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Teapot_Rotation.cpp, Teapot.h
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW SPINNING TEAPOT 
///		2. ANIMATE ON USER INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:

				1. -->  Global Variables

						GLfloat rotate = 0.0f;
						bool gbAnimate = false;

2. main()  : Changes are as follows :
				1. --> 
				
				case XK_a:
				case XK_A: //ANIMATION TOGGLE
					if (gbAnimate == false)
					{
						gbAnimate = true;
						
					}
					else
						if (gbAnimate == true)
						{
							gbAnimate = false;
						}
					break;	
						

3. display() : Changes are as follows :
				1. -->  
					{
						.
						.
						//code
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						glMatrixMode(GL_MODELVIEW);
						glLoadIdentity();
						glTranslatef(0.0f, 0.0f, -1.0f);
						glRotatef(rotate, 0.0f, 1.0f, 0.0f);


						glBegin(GL_TRIANGLES);
						for (int i = 0; i < sizeof(face_indicies) / sizeof(face_indicies[0]); i++)
						{
							for (int j = 0; j < 3; j++)
							{
								int vi = face_indicies[i][j]; //vertices
								int ni = face_indicies[i][j + 3]; //normals
								int ti = face_indicies[i][j + 6]; //textures

								glNormal3f(normals[ni][0], normals[ni][1], normals[ni][2]);
								glTexCoord2f(textures[ti][0], textures[ti][1]);
								glVertex3f(vertices[vi][0], vertices[vi][1], vertices[vi][2]);
							}
						}

						if (gbAnimate == true)
						{
							rotate += 1.0f;
							if (rotate >= 360.f)
							{
								rotate = 0.0f;
							}
						}
						
						glEnd();
						.
						.
					}
						
