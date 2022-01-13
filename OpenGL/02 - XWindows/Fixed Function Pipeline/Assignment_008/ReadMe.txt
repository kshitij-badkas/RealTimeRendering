/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Tweaked_Smiley
------------------------------------------------------------------------------------------------------
Assignment Number 		: 008
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_008_Tweaked_Smiley
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Tweaked_Smiley.cpp, Smiley.bmp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW DIFFERENT TYPES OF SMILEY IMAGES AS TEXTURE ON USER-INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows : 
			  			  
			  1. unsigned int pressed_key;

2. main()  : Changes are as follows :
			1. --> In GAME LOOP :
					.
					.
					case 49: 
					case XK_KP_1:	//'1 is pressed
						pressed_key = 1;
						printf("\n pressed_key = %d \n", pressed_key);
						glEnable(GL_TEXTURE_2D);
						break;

					case 50: 
					case XK_KP_2:	//'2' is pressed
						pressed_key = 2;
						printf("\n pressed_key = %d \n", pressed_key);
						glEnable(GL_TEXTURE_2D);
						break;

					case 51:
					case XK_KP_3:	//'3' is pressed
						pressed_key = 3;
						printf("\n pressed_key = %d \n", pressed_key);
						glEnable(GL_TEXTURE_2D);
						break;

					case 52:
					case XK_KP_4:	//'4' is pressed
						pressed_key = 4;
						printf("\n pressed_key = %d \n", pressed_key);
						glEnable(GL_TEXTURE_2D);
						break;

					default:
						pressed_key = keysym;
						printf("\n pressed_key = %d \n", pressed_key);
						glDisable(GL_TEXTURE_2D);
						break;
					.
					.

4. Initialize() : Changes are as follows :
					1. --> Removing 'glEnable(GL_TEXTURE_2D);'
						
5. display() : Changes are as follows :
				1. --> void display(void)
						{
							.
							.
							if (pressed_key == 1)
							{
							
							  	//FULL SMILEY
							  	
							}
							else if (pressed_key == 2)
							{
								//LEFT BOTTOM SECTOR - WRAPPING AND CLAMPLING
							
							}
							else if (pressed_key == 3)
							{
								//TILED SMILEYS - TILING OR REPEAT
							
							}
							else if (pressed_key == 4)
							{
								//ONE PIXEL OF IMAGE
							
							}
							else
							{
								//WHITE RECTANGLE
							
							}
							.
							.						
						}
						
					
						
						
