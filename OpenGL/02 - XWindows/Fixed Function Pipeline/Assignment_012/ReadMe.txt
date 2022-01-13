/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Graph_Paper
------------------------------------------------------------------------------------------------------
Assignment Number 		: 012
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_012_Graph_Paper
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Graph_Paper.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW A GRAPH-PAPER
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : NO CHANGE.

2. main()  : NO CHANGE.

3. display() :  void display(void)
				{
					//code
					//variables
					float x_coord;
					float y_coord;
					float inc = 0.05f;

					glClear(GL_COLOR_BUFFER_BIT);
					glMatrixMode(GL_MODELVIEW);

					//	GRAPH PAPER
					glLoadIdentity();
					glTranslatef(0.0f, 0.0f, -3.0f);
					glBegin(GL_LINES);
						
					// 20 VERTICAL LINES : BLUE
					glColor3f(0.0f, 0.0f, 1.0f);
					//LOOP +ve X-AXIS
					for (x_coord = inc; x_coord <= 1.05f; x_coord = x_coord + inc)
					{
						glVertex3f(x_coord, 1.0f, 0.0f);
						glVertex3f(x_coord, -1.0f, 0.0f);
					}

					// VERTICAL GREEN
					glColor3f(0.0f, 1.0f, 0.0f);
					glVertex3f(0.0f, -1.0f, 0.0f);
					glVertex3f(0.0f, 1.0f, 0.0f);

					// 20 VERTICAL LINES : BLUE 
					glColor3f(0.0f, 0.0f, 1.0f);
					//LOOP -ve X-AXIS
					for (x_coord = inc; x_coord <= 1.05f; x_coord = x_coord + inc)
					{
						glVertex3f(-x_coord, -1.0f, 0.0f);
						glVertex3f(-x_coord, 1.0f, 0.0f);
					}

					// 20 HORIZONTAL LINES : BLUE
					glColor3f(0.0f, 0.0f, 1.0f);
					//LOOP +ve Y-AXIS
					for (y_coord = inc; y_coord <= 1.05f; y_coord = y_coord + inc)
					{
						glVertex3f(-1.0f, y_coord, 0.0f);
						glVertex3f(1.0f, y_coord, 0.0f);
					}

					// HORIZONTAL RED 
					glColor3f(1.0f, 0.0f, 0.0f);
					glVertex3f(-1.0f, 0.0f, 0.0f);
					glVertex3f(1.0f, 0.0f, 0.0f);

					// 20 HORIZONTAL LINES : BLUE
					glColor3f(0.0f, 0.0f, 1.0f);
					//LOOP -ve Y-AXIS
					for (y_coord = inc; y_coord <= 1.05f; y_coord = y_coord + inc)
					{
						glVertex3f(-1.0f, -y_coord, 0.0f);
						glVertex3f(1.0f, -y_coord, 0.0f);
					}
					glEnd();
					

					//Double Buffer
					glXSwapBuffers(gpDisplay, gWindow);
				}								


