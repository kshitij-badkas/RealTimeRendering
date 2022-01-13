#include <GL/freeglut.h>

bool bFullscreen_KB = false;

int main(int argc, char** argv)
{
	//code
	//function declaration
	void initialize_KB(void);
	void resize_KB(int, int);
	void display_KB(void);
	void keyboard_KB(unsigned char, int, int);
	void mouse_KB(int, int, int, int);
	void uninitialize_KB(void);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("GLUT : Colored Triangle : By Kshitij_Badkas");

	initialize_KB();

	glutDisplayFunc(display_KB);
	glutReshapeFunc(resize_KB);
	glutKeyboardFunc(keyboard_KB);
	glutMouseFunc(mouse_KB);
	glutCloseFunc(uninitialize_KB);
	glutMainLoop();

	return(0); //This line is not neccessary.
}

void initialize_KB(void)
{
	//code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Black
}

void resize_KB(int width, int height)
{
	//code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void display_KB(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();
	glFlush();
}

void keyboard_KB(unsigned char key, int x, int y)
{
	//code
	switch (key)
	{
		case 27:
				glutLeaveMainLoop();
				break;

		case 'F':
		case 'f':
				if (bFullscreen_KB == false)
				{
					glutFullScreen();
					bFullscreen_KB = true;
				}
					
				else
				{
					glutLeaveFullScreen();
					bFullscreen_KB = false;
				}
				break;
		
		default:break;
	}
}

void mouse_KB(int button, int state, int x, int y)
{
	//code
	switch (button)
	{
		case GLUT_LEFT_BUTTON:	break;
		case GLUT_RIGHT_BUTTON:
								glutLeaveMainLoop();
								break;

		default:break;
	}
}

void uninitialize_KB(void)
{
	//code
}