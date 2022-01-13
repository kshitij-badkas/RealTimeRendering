/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		                              ASSIGNMENT DETAILS	  									  
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

------------------------------------------------------------------------------------------------------
Assignment Name   		: Blue_Window
------------------------------------------------------------------------------------------------------
Assignment Number 		: 009	
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_009_Blue_Window
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Blue_Window.cpp, Blue_Window.h, Blue_Window.rc, KAB.ico
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Following are the changes made for the assignment :-
------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////////////
///
///		THIS APPLICATION WILL : 
///		1. SHOW BLUE SCREEN WINDOW 
///		2. TOGGLE THE WINDOW TO FULLSCREEN RESOLUTION ON USER INTERACTION 
		3. CREATE A LOG FILE 'KAB_Log.txt' 
///		4. SHOW AN ICON ON THE WINDOW AND ON THE TASKBAR 
///
/////////////////////////////////////////////////////////////////////////////////////////////////////

1. Globally : Changes are as follows:
				1. --> Added Header files
						#include "Blue_Window.h"
						#include <stdio.h>
						#include <gl/gl.h>
						#pragma comment(lib, "OpenGL32.LIB")

				2. --> Added MACROS
						#define WIN_WIDTH 800
						#define WIN_HEIGHT 600

				3. --> Added Global Variables
						FILE* gpFile = NULL;
						HDC ghdc = NULL;
						HGLRC ghrc = NULL;
						bool gbActiveWindow = false;

2. WinMain  : Changes are as follows:
				1. -->  Using CreateWindowEx() function and passing the following parameters :
						
						hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName,
							TEXT("FFP: Assignment_009_Blue_Window : By Kshitij Badkas")
							WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
							iCreateWindowPositionX, 
							iCreateWindowPositionY, 
							WIN_WIDTH, 
							WIN_HEIGHT,
							NULL,
							NULL,
							hInstance,
							NULL);

				2. -->  Added Function:
						void Initialize(void);

				3. -->	Defining 'Game Loop' by using PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)

3. WndProc  : Changes are as follows:
				1. -->  Added Functions:

						void Resize(int, int);
						void Uninitialize(void);
						void Display(void);

				2. -->	Added Cases in Switch :

						case WM_PAINT:
							Display();
							break;

						case WM_SETFOCUS:
							gbActiveWindow = true;
							break;

						case WM_KILLFOCUS:
							gbActiveWindow = false;
							break;

						case WM_ERASEBKGND:
							return(0);

						case WM_SIZE:
							Resize(LOWORD(lParam), HIWORD(wParam));
							break;

						case WM_CLOSE:
							DestroyWindow(hwnd);

						case WM_DESTROY:
							Uninitialize();
							PostQuitMessage(0);
							break;

						<Inside Switch case WM_KEYDOWN>:
						case VK_ESCAPE:
							DestroyWindow(hwnd);
							break;



--------------------------------------------------------------------------------------------------------
Following are the contents of 'Blue_Window.h' (Header File)
--------------------------------------------------------------------------------------------------------
#pragma once
#define MYICON 101


--------------------------------------------------------------------------------------------------------
Following are the contents of 'Blue_Window.rc' (Resource Script)
--------------------------------------------------------------------------------------------------------
#include "Blue_Window.h"
MYICON ICON KAB.ico
