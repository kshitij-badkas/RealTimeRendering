/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Teapot_with_Lights
------------------------------------------------------------------------------------------------------
Assignment Number 		: 026
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_026_Teapot_with_Lights
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Teapot_with_Lights.cpp, Teapot.h, marble.bmp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW SPINNING TEAPOT WITH TEXTURE AND LIGHTS
///		2. ANIMATE AND LIGHT TOGGLE ON USER INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:

				1. -->  Global Variables

						GLuint marble_texture;	
						GLfloat rotate = 0.0f;
						bool gbAnimate = false;
						bool gbTexture = false;
						bool gbLight = false;
						
						//Lights
						GLfloat lightAmbient[] = { 1.0f,1.0f,1.0f,1.0f };
						GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
						GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
						GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

						GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
						GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
						GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
						GLfloat materialShininess = 50.0f;

2. main()  : Changes are as follows :
				1. --> 
				
				case XK_t:
				case XK_T: //TEXTURE TOGGLE
					if (gbTexture == false)
					{
						gbTexture = true;
						glEnable(GL_TEXTURE_2D);
					}
					else
						if (gbTexture == true)
						{
							gbTexture = false;
							glDisable(GL_TEXTURE_2D);
						}
					break;

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
					
				case XK_l:
				case XK_L: //LIGHT TOGGLE
					if (gbLight == false)
					{
						glEnable(GL_LIGHTING);
						gbLight = true;
					}
					else
						if (gbLight == true)
						{
							glDisable(GL_LIGHTING);
							gbLight = false;
						}
					
					break;
						

3. Initialize() : Changes are as follows :
				  1. -- >
				  	.
				  	.
				  	//texture
					marble_texture = LoadBitmapAsTexture("marble.bmp");
					.
					.
					//Lighting
					glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
					glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
					glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
					glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

					//Material
					glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
					glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
					glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
					
					glEnable(GL_LIGHT0);
				
4. display() : NO CHANGE

						
