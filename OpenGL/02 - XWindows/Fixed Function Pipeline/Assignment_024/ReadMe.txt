/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Solar_System
------------------------------------------------------------------------------------------------------
Assignment Number 		: 024
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_024_Solar_System
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Solar_System.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW SUN, EARTH AND MOON OF THE SOLAR-SYSTEM
///		2. ANIMATE ON USER INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:

				1. -->  Global Variables

						int day = 0;
						int year = 0;
						int moon = 0;

						GLUquadric* quadric = NULL;

2. main()  : Changes are as follows :
				1. --> 
				
				case XK_D:
					day = (day + 6) % 360;
					break;
					
				case XK_d:
					day = (day - 6) % 360;
					break;
					
				case XK_Y:
					year = (year + 1) % 360;
					moon = (moon + 3) % 360;
					break;
					
				case XK_y:
					year = (year - 1) % 360;
					moon = (moon - 3) % 360;
					break;
						

3. display() : Changes are as follows :
				1. -->  
					{
						.
						.
						gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
						glPushMatrix();
						
						//Sun - Parent
						glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glColor3f(1.0f, 1.0f, 0.0f);

						quadric = gluNewQuadric();
						gluSphere(quadric, 0.75f, 30, 30);
						glPopMatrix();

						glPushMatrix();

						//Earth - Child of Sun -- AND -- Parent of Moon
						glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);
						glTranslatef(1.5f, 0.0f, 0.0f);
						glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
						glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glColor3f(0.4f, 0.9f, 1.0f);

						quadric = gluNewQuadric();
						gluSphere(quadric, 0.2f, 20, 20);
						glPushMatrix();

						//Moon - Child of Earth
						glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
						glRotatef((GLfloat)moon, 0.0f, 0.0f, 1.0f);
						glTranslatef(0.4f, 0.0f, 0.0f);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glColor3f(0.8f, 0.8f, 0.8f);

						quadric = gluNewQuadric();
						gluSphere(quadric, 0.05f, 10, 10);
						glPopMatrix();

						glPopMatrix();
						.
						.
					}
						
