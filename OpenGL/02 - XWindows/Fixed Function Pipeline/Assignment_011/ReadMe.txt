/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Multiple_Viewport
------------------------------------------------------------------------------------------------------
Assignment Number 		: 011
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assingmnet_011_Multiple_Viewport
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Multiple_Viewport.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW MULTI-COLOURED TRIANGLE FROM DIFFERENT VIEWPORTS ON USER-INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows : 
			  
			  1. int updated_width = giWindowWidth;
				 int updated_height = giWindowHeight;

2. main()  : Changes are as follows :
			  1. -->
			   
				case 48:
				case XK_KP_0:
					glViewport(0, 0, GLsizei(updated_width), GLsizei(updated_height));
					break;

				case 49:
				case XK_KP_1:
					glViewport(0, updated_height / 2, GLsizei(updated_width / 2), GLsizei(updated_height / 2));
					break;

				case 50:
				case XK_KP_2:
					glViewport(updated_width / 2, updated_height / 2, GLsizei(updated_width / 2), GLsizei(updated_height / 2));
					break;

				case 51:
				case XK_KP_3:
					glViewport(updated_width / 2, 0, GLsizei(updated_width / 2), GLsizei(updated_height / 2));
					break;

				case 52:
				case XK_KP_4:
					glViewport(0, 0, GLsizei(updated_width / 2), GLsizei(updated_height / 2));
					break;

				case 53:
				case XK_KP_5:
					glViewport(0, 0, GLsizei(updated_width / 2), GLsizei(updated_height));
					break;

				case 54:
				case XK_KP_6:
					glViewport(updated_width / 2, 0, GLsizei(updated_width / 2), GLsizei(updated_height));
					break;

				case 55:
				case XK_KP_7:
					glViewport(0, updated_height / 2, GLsizei(updated_width), GLsizei(updated_height / 2));
					break;

				case 56:
				case XK_KP_8:
					glViewport(0, 0, GLsizei(updated_width), GLsizei(updated_height / 2));
					break;

				case 57:
				case XK_KP_9:
					glViewport(updated_width / 4, updated_height / 4, GLsizei(updated_width / 2), GLsizei(updated_height / 2));
					break;


3. Resize () : Changes are as follows : 
				1. -->
				
					updated_width = width;
					updated_height = height; 										


