/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Spotlight
------------------------------------------------------------------------------------------------------
Assignment Number 		: 022
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_022_SpotLight
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Spotlight.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW A SPHERE WITH SPOT-LIGHT
///		2. LIGHT TOGGLE ON USER INTERACTION
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:

				1. -->  Global Variables
						
						//Lights
						bool gbLight = false;
						
						//Spot-Light
						GLfloat light2_Ambient[] = { 0.1f,0.1f,0.1f,1.0f };
						GLfloat light2_Diffused[] = { 1.0f,1.0f,1.0f,1.0f };
						GLfloat light2_Specular[] = { 1.0f,1.0f,1.0f,1.0f };
						GLfloat light2_Position[] = { 0.0f,0.0f,1.0f,1.0f };
						GLfloat spotDirection[] = { 0.0f,0.0f,-1.0f };

						//Material
						GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
						GLfloat materialDiffused[] = { 0.5f,0.2f,0.7f,1.0f };
						GLfloat materialSpecular[] = { 0.7f,0.7f,0.7f,1.0f };
						GLfloat materialShininess = 128.0f;

						GLUquadric* quadric = NULL;

2. main()  : NO CHANGES
						
3. Initialize() : Changes are as follows :
				1. --> 	//Initialization of light
						glLightfv(GL_LIGHT2, GL_AMBIENT, light2_Ambient);
						glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_Diffused);
						glLightfv(GL_LIGHT2, GL_SPECULAR, light2_Specular);
						glLightfv(GL_LIGHT2, GL_POSITION, light2_Position);
						
						glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.5f);
						glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.5f);
						glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.2f);
						glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 15.0f);
						glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDirection);
						glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 2.0f);
						
						
						//Material
						glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffused);
						glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
						glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
						
						glEnable(GL_LIGHT2);

4. display() : Changes are as follows :
				1. -->  
					{
						.
						.
						glLoadIdentity();
						glTranslatef(0.0f, 0.0f, -0.55f);

						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

						quadric = gluNewQuadric();
						gluSphere(quadric, 0.2f, 30, 30);
						.
						.
					}
						
