//////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Coloured Square
------------------------------------------------------------------------------------------------------
Assignment Number 		: 004
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_004_Coloured_Square
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Coloured_Square.cpp, Headers.h, vmath.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. Global variables -
				GLuint gVao_Square, gVbo_Position_Square, gVbo_Color_Square;


				mat4 gPerspectiveProjectionMatrix;

2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE


4. Initialize  : Changes are as follows:
			
			1. 	
				const GLfloat squareVertices[] =
				{
					1.0f, 1.0f, 0.0f, //right-top
					-1.0f, 1.0f, 0.0f, //left-top
					-1.0f, -1.0f, 0.0f, //left-bottom
					1.0f, -1.0f, 0.0f //right-bottom
				};

				const GLfloat squareColors[] =
				{
					0.0f, 0.0f, 1.0f, //B
					0.0f, 0.0f, 1.0f, //B
					0.0f, 0.0f, 1.0f, //B
					0.0f, 0.0f, 1.0f, //B
				};


				
4. Display(): Changes are as follows:
			1. glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				

--------------------------------------------------------------------------------------------------------
Following are the contents of 'Header.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101


--------------------------------------------------------------------------------------------------------
Following are the contents of 'Resources.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "Headers.h"
MYICON ICON KAB.ico
/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Blue Window With Double Buffers And Safe Shader Cleanup
------------------------------------------------------------------------------------------------------
Assignment Number 		: 001
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_001_Blue_Window
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Blue_Window.cpp, Headers.h, Resources.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows:
			
			1. Header file - above <gl\GL.h>
				#include <gl\glew.h>

			2. pragma - #pragma comment(lib, "glew32.lib")

			3. Global variables -
				enum
				{
					KAB_ATTRIBUTE_VERTEX = 0,
					KAB_ATTRIBUTE_COLOR,
					KAB_ATTRIBUTE_NORMAL,
					KAB_ATTRIBUTE_TEXTURE0
				};

				GLuint gVertexShaderObject;
				GLuint gFragmentShaderObject;
				GLuint gShaderProgramObject;

2. WinMain  : NO CHANGE

 
3. WndProc  : NO CHANGE


4. Initialize  : Changes are as follows:
			
			1. 	//OPENGL RELATED LOG
				fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
				fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
				fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
				fprintf(gpFile, "GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

				//OPENGL ENABLES EXTENSIONS
				GLint numExt;
				glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
				for (int i = 0; i < numExt; i++)
				{
					fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
				}

				//VERTEX SHADER
				//create shader
				gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

				//provide source code to shader
				const GLchar* vertexShaderSourceCode =
					"#version 450" \
					"\n" \
					"void main(void)" \
					"{" \
					"}";

				glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

				//compile shader
				glCompileShader(gVertexShaderObject);

				//FRAGMENT SHADER
				//create shader
				gFragmentShaderObject = glCreateShader(GL_VERTEX_SHADER);

				//provide source code to shader
				const GLchar* fragmentShaderSourceCode =
					"#version 450" \
					"\n" \
					"void main(void)" \
					"{" \
					"}";

				glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

				//compile shader
				glCompileShader(gFragmentShaderObject);


				//SHADER PROGRAM
				//create
				gShaderProgramObject = glCreateProgram();

				//attach vertex shader object and fragment shader object to program 
				glAttachShader(gShaderProgramObject, gVertexShaderObject);
				glAttachShader(gShaderProgramObject, gFragmentShaderObject);

				//link shader
				glLinkProgram(gShaderProgramObject);

				//we will always cull back faces for beter performance
				glEnable(GL_CULL_FACE);

							

4. Display(): Changes as follows:

				//code
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				//start using OpenGL program object
				glUseProgram(gShaderProgramObject);

				//OpenGL Drawing

				//stop using OpenGL program object
				glUseProgram(0);

				//Double Buffer
				SwapBuffers(ghdc);

5. Uninitialize(): Changes are as follows:
				
				//SAFE SHADER CLEANUP
				if (gShaderProgramObject)
				{
					glUseProgram(gShaderProgramObject);

					GLsizei shaderCount;
					glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

					GLuint* pShader;
					pShader = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

					if (pShader == NULL)
					{
						//error checking
					}

					glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShader);

					for (GLsizei i = 0; i < shaderCount; i++)
					{
						glDetachShader(gShaderProgramObject, pShader[i]);
						glDeleteShader(pShader[i]);
						pShader[i] = 0;
					}

					free(pShader);

					glDeleteProgram(gShaderProgramObject);
					gShaderProgramObject = 0;
					
					//unlink
					glUseProgram(0);
				}

--------------------------------------------------------------------------------------------------------
Following are the contents of 'Header.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101


--------------------------------------------------------------------------------------------------------
Following are the contents of 'Resources.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "Headers.h"
MYICON ICON KAB.ico
