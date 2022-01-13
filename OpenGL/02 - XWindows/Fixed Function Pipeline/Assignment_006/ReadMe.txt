/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: 3D_Animation
------------------------------------------------------------------------------------------------------
Assignment Number 		: 006
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_006_3D_Animation
------------------------------------------------------------------------------------------------------
Created New File(s) 	: 3D_Animation.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW SPINNING 3D TRIANGLE 
///		2. ALL AXES ROTATING 3D RECTANGLE  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : NO CHANGE.

2. main()  : NO CHANGE.

3. CreateWindow() : Changes are as follows :
					1. --> Adding Depth Parameters
					
					static int frameBufferAttribute[] = { GLX_DOUBLEBUFFER, True, 
										  GLX_RGBA,
										  GLX_RED_SIZE, 8,
										  GLX_GREEN_SIZE, 8,
										  GLX_BLUE_SIZE, 8,
										  GLX_ALPHA_SIZE, 8,
										  //Depth
										  GLX_DEPTH_SIZE, 24, //video for Linux (V4L) recommends 24 instead of 32 
										  None //Here we can say 0 as well										  
										}; //PixelFormatAttribute
						//static because it is a convention

4. Initialize() : Changes are as follows :
					1. --> Adding Depth Parameters
							
							//3D & Depth Code
							glShadeModel(GL_SMOOTH);  //anti-aliasing
							glClearDepth(1.0f);    //Default value passed to the Function
							glEnable(GL_DEPTH_TEST);
							glDepthFunc(GL_LEQUAL);
							glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 	
												

5. display() : Changes are as follows :
				1. --> void display(void)
						{
							.
							.
							Draw_Pyramid();
							Draw_Cube();
							.
							.
						}
						
					
						
						
