/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Cube_Lights
------------------------------------------------------------------------------------------------------
Assignment Number 		: 017	
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_017_Cube_Lights
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Cube_Lights.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW CUBE WITH ALL AXES ROTATION
///		2. LIGHT TOGGLE ON USER-INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:

				1. -->  Global Variables
						
						bool gbLight = false;
						GLfloat lightAmbient[] = { 0.2f,0.5f,0.7f,1.0f }; //Grey Light
						GLfloat lightDiffused[] = { 1.0f,1.0f,1.0f,1.0f }; //White Light
						GLfloat lightPosition[] = { 0.0f,0.0f,2.0f,1.0f }; //Z-Axis

2. main()  : Changes are as follows:
				1. -->  case XK_L:
						case XK_l:
							if (gbLight == false)
							{
								gbLight = true;
								glEnable(GL_LIGHTING);
							}
							else
							{
								if (gbLight == true)
								{
									gbLight = false;
									glDisable(GL_LIGHTING);
								}
								
							}
							break;	 
						
3. Initialize() : Changes are as follows :
				1. --> 	//Lighting Code
						glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
						glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffused);
						glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
						glEnable(GL_LIGHT1);

4. display() : Changes are as follows :
				1. -->  
					{
						//variables
						static GLfloat rAngle = 0.0f;
						
						//code
						glLoadIdentity();
						glTranslatef(0.0f, 0.0f, -3.0f);
						glRotatef(rAngle, 1.0f, 1.0f, 1.0f);
						glScalef(0.5f, 0.5f, 0.5f);

						glBegin(GL_QUADS);
						
						//FRONT
						glNormal3f(0.0f, 0.0f, 1.0f);
						glVertex3f(1.0f, 1.0f, 1.0f);
						glVertex3f(-1.0f, 1.0f, 1.0f);
						glVertex3f(-1.0f, -1.0f, 1.0f);
						glVertex3f(1.0f, -1.0f, 1.0f);

						//RIGHT
						glNormal3f(1.0f, 0.0f, 0.0f);
						glVertex3f(1.0f, 1.0f, -1.0f);
						glVertex3f(1.0f, 1.0f, 1.0f);
						glVertex3f(1.0f, -1.0f, 1.0f);
						glVertex3f(1.0f, -1.0f, -1.0f);

						//BACK
						glNormal3f(0.0f, 0.0f, -1.0f);
						glVertex3f(-1.0f, 1.0f, -1.0f);
						glVertex3f(1.0f, 1.0f, -1.0f);
						glVertex3f(1.0f, -1.0f, -1.0f);
						glVertex3f(-1.0f, -1.0f, -1.0f);
						
						.
						.
						glEnd();

						rAngle = rAngle + 1.0f;
						if (rAngle >= 360.0f)
							rAngle = 0.0f;
					}
						
