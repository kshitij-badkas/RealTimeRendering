/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Orthographic_Triangle
------------------------------------------------------------------------------------------------------
Assignment Number 		: 002
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_002_Orthographic_Triangle
------------------------------------------------------------------------------------------------------
Created New File(s) 	: ortho.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW ORTHOGRAPHIC TRIANGLE 
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
				1. --> Added Header files
						#include<GL/glu.h>

				2. -->  Global Variables
						GLXContext gGlxContext;

2. main()  : NO CHANGE.
						

3. Resize() : Changes are as follows :
				1. --> 	void Resize(int width, int height)
						{
							if(height == 0)
							{
								height = 1;
							}
							glViewport(0,0,(GLsizei)width, (GLsizei)height);
							
						  glMatrixMode(GL_PROJECTION);
						  glLoadIdentity();

						  if (width <= height)
						  {
							glOrtho(-100.0f,
							  100.0f,
							  -100.0f * ((GLfloat)height / (GLfloat)width),
							  100.0f * ((GLfloat)height / (GLfloat)width),
							  -100.0f,
							  100.0f);
						  }
						  else
						  {
							glOrtho(-100.0f * ((GLfloat)width / (GLfloat)height),
							  100.0f * ((GLfloat)width / (GLfloat)height),
							  -100.0f,
							  100.0f,
							  -100.0f,
							  100.0f);
						  }
						}

4. display() : Changes are as follows :
				1. --> void display(void)
						{
							//code
						  glClear(GL_COLOR_BUFFER_BIT);
						  
						  glMatrixMode(GL_MODELVIEW);
						  glLoadIdentity();

						  glBegin(GL_TRIANGLES);
						  glColor3f(1.0f, 0.0f, 0.0f);
						  glVertex3f(0.0f, 50.0f, 0.0f);

						  glColor3f(0.0f, 1.0f, 0.0f);
						  glVertex3f(-50.0f, -50.0f, 0.0f);

						  glColor3f(0.0f, 0.0f, 1.0f);
						  glVertex3f(50.0f, -50.0f, 0.0f);

						  glEnd();
							
							glFlush();
						}

						
						
