/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Robotic_Arm
------------------------------------------------------------------------------------------------------
Assignment Number 		: 023
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_023_Robotic_Arm
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Robotic_Arm.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW ROBOTIC-ARM
///		2. ANIMATE ON USER INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:

				1. -->  Global Variables
						
						int shoulder = 0;
						int elbow = 0;
						GLUquadric* quadric = NULL;

2. main()  : Changes are as follows :
				1. --> 
					case XK_S: 
						shoulder = (shoulder + 3) % 360;
						break;
					case XK_s: 
						shoulder = (shoulder - 3) % 360;
						break;
					case XK_E: 
						elbow = (elbow + 3) % 360;
						break;
					case XK_e: 
						elbow = (elbow - 3) % 360;
						break;
						

3. display() : Changes are as follows :
				1. -->  
					{
						.
						.
						glTranslatef(0.0f, 0.0f, -12.0f);
						glPushMatrix();

						glRotatef((GLfloat)shoulder, 0.0f, 0.0f, 1.0f);
						glTranslatef(1.0f, 0.0f, 0.0f);
						glPushMatrix();

						glScalef(2.0f, 0.5f, 1.0f);
						glColor3f(0.5f, 0.35f, 0.05f);
						quadric = gluNewQuadric();
						gluSphere(quadric, 0.5f, 10, 10);
						glPopMatrix();

						glTranslatef(1.0f, 0.0f, 0.0f);
						glRotatef((GLfloat)elbow, 0.0f, 0.0f, 1.0f);
						glTranslatef(1.0f, 0.0f, 0.0f);
						glPushMatrix();
						
						glScalef(2.15f, 0.5f, 1.0f);
						glColor3f(0.5f, 0.35f, 0.05f);
						quadric = gluNewQuadric();
						gluSphere(quadric, 0.5f, 10, 10);
						glPopMatrix();

						glPopMatrix();
						.
						.
					}
						
