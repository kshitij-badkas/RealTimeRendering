/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Blue_Window
------------------------------------------------------------------------------------------------------
Assignment Number 		: 001	
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_001_Blue_Window
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Blue_Window.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW BLUE SCREEN WINDOW 
///		2. TOGGLE THE WINDOW TO FULLSCREEN RESOLUTION ON USER INTERACTION 
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
				1. --> Added Header files
						#include<iostream>
						#include<stdio.h>
						#include<stdlib.h>
						#include<memory.h>
						
						#include<X11/Xlib.h>
						#include<X11/Xutil.h>
						#include<X11/XKBlib.h>
						#include<X11/keysym.h>
						
						#include<GL/gl.h>
						#include<GL/glx.h>

				2. -->  Global Variables
						GLXContext gGlxContext;

2. main()  : Changes are as follows:
				1. -->  Using the following functions :
						void CreateWindow(void);
						void ToggleFullscreen(void);
						void Unitialize(void);

						void Initialize(void);
						void Resize(int, int);
						void display(void);
						

				2. -->	Defining 'Game Loop'

3. Initialize() : Changes are as follows :
				1. --> 	//prototypes
						void Resize(int, int);	//FOR WARM-UP CALL
						
						//code
						//NEW
						gGlxContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
						glXMakeCurrent(gpDisplay, gWindow, gGlxContext);
						glClearColor(0.0f,0.0f,1.0f,1.0f); //BLUE 
						
						//WARM-UP CALL
						Resize(giWindowWidth, giWindowHeight);

4. display() : Changes are as follows :
				1. --> //code
						glClear(GL_COLOR_BUFFER_BIT);
						glFlush();
						
						
