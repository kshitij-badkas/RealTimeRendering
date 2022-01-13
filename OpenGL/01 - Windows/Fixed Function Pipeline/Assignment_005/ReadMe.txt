------------------------------------------------------------------------------------------------------
Assignment Name   		: File IO
------------------------------------------------------------------------------------------------------
Assignment Number 		: 005
------------------------------------------------------------------------------------------------------
Base Assignment   		: Assignment_005
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Part A
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Assignment_005_Part_A_FileIO_Console.c
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Create New Application  : Assignment_005_Part_A_FileIO_Console.c
------------------------------------------------------------------------------------------------------

1. Globally : No Change.

2. Main 	: Changes are as follows :

				1. Declared variable of type FILE * - FILE *pFile = NULL

				2. Implemented code for file creation and writing text to the file -'KAB_Log.txt'

				3. This file is used as a Log file.


============================================================================================================================
============================================================================================================================
============================================================================================================================


------------------------------------------------------------------------------------------------------
Assignment Name   		: File IO
------------------------------------------------------------------------------------------------------
Assignment Number 		: 005
------------------------------------------------------------------------------------------------------
Base Assignment   		: Assignment_005
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Part B
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Assignment_005_Part_B_FileIO_GUI.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Make These All Below Changes In Your New Application : Assignment_005_Part_B_FileIO_GUI.cpp
------------------------------------------------------------------------------------------------------

1. Globally : Changes are as follows :
				1. --> Added variable of type FILE *
					1. FILE *gpFile = NULL;

2. WinMain  : Changes are as follows :
				1. --> Added code for File Handling (FILE I/O), displaying MessageBox after file creatiion 
				       and writing text to the file. The file 'KAB_Log.txt' will be used as a Log file.

					int iFileOpen;
					iFileOpen = fopen_s(&gpFile, "KAB_Log.txt", "w");

					MessageBox(NULL, TEXT("Cannot Create Desired File."), TEXT("Error"), MB_OK);
					

3. WndProc  : Changes are as follows: 
				1. --> Added code in the Switch case

					case WM_CREATE:
						fprintf(gpFile, "Bharat Is My Country.\n");
						MessageBox(hwnd, TEXT("WM_CREATE is received."), TEXT("My Message"), MB_OK);
						break;

					case WM_DESTROY:
						fprintf(gpFile, "Jai Hind.\n");
						PostQuitMessage(0);
						break;
