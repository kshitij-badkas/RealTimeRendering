/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Sphere_Lights
------------------------------------------------------------------------------------------------------
Assignment Number 		: 018
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_018_Sphere_Lights
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Sphere_Lights.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW SPHERE WITH MATERIAL AND LIGHT
///		2. LIGHT TOGGLE ON USER-INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:

				1. -->  Global Variables
						
						//Lights
						bool gbLight = false;

						GLfloat lightAmbient[] = { 0.1f,0.1f,0.1f,1.0f }; //
						GLfloat lightDiffused[] = { 1.0f,1.0f,1.0f,1.0f }; //White Light
						GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
						GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f }; //Z-Axis

						GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
						GLfloat materialDiffused[] = { 0.5f,0.2f,0.7f,1.0f };
						GLfloat materialSpecular[] = { 0.7f,0.7f,0.7f,1.0f };
						GLfloat materialShininess = 128.0f;
						
						GLUquadric* quadric = NULL;

2. main()  : NO CHANGES
						
3. Initialize() : Changes are as follows :
				1. --> 	//Lighting Code
						glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
						glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffused);
						glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
						glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
						glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffused);
						glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
						glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
						glEnable(GL_LIGHT1);

4. display() : Changes are as follows :
				1. -->  
					{
						//code
						//Gaurad's shading
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						glMatrixMode(GL_MODELVIEW);
						glLoadIdentity();
						glTranslatef(0.0f, 0.0f, -0.55f);

						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

						quadric = gluNewQuadric();
						gluSphere(quadric, 0.2f, 30, 30);

						//Normals are calculated using gluSphere itself so we do not need glBegin and glEnd
						
						//Double Buffer
						glXSwapBuffers(gpDisplay, gWindow);
					}
						
