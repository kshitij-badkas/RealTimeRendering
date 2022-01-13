------------------------------------------------------------------------------------------------------
Assignment Name   		: Centering of Window
------------------------------------------------------------------------------------------------------
Assignment Number 		: 004
------------------------------------------------------------------------------------------------------
Base Assignment   		: Assignment_004
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Part A
						: Part B
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Assignment_004_Part_A_Centering_of_Window.cpp
						: Assignment_004_Part_B_Centering_of_Window.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Make These All Below Changes In Your New Application : Assignment_004_Part_A_Centering_of_Window.cpp
------------------------------------------------------------------------------------------------------

1. Globally : No Change

2. WinMain  : Changes are as follows :
				1. --> Added 8 variables for storing and calculating Screen Resolution and New Windows's resolution -
					1. int iScreenWidth;
					2. int iScreenHeight;

					3. int iScreenWidthCentre;
					4. int iScreenHeightCentre;

					5. int iWindowWidth = 800;
					6. int iWindowHeight = 600;

					7. int iCreateWindowPositionX;
					8. int iCreateWindowPositionY;

				2. --> Calculate the position of New Window -
					1. iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
					2. iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

					3. iScreenWidthCentre = (iScreenWidth / 2);
					4. iScreenHeightCentre = (iScreenHeight / 2);

					5. iCreateWindowPositionX = iScreenWidthCentre - (iWindowWidth / 2);
					6. iCreateWindowPositionY = iScreenHeightCentre - (iWindowHeight / 2);

				3. --> Pass the parameters to CreateWindow()
						-> 
							iCreateWindowPositionX,
							iCreateWindowPositionY,
							iWindowWidth,
							iWindowHeight

							hwnd = CreateWindow(szAppName,
									TEXT("FFP: Assignment_004_Part_A_Centering_of_Window : By Kshitij Badkas"),
									WS_OVERLAPPEDWINDOW,
									iCreateWindowPositionX,
									iCreateWindowPositionY,
									iWindowWidth,
									iWindowHeight,
									NULL,
									NULL,
									hInstance,
									NULL);

3. WndProc  : No Change

============================================================================================================================
============================================================================================================================
============================================================================================================================

------------------------------------------------------------------------------------------------------
Make These All Below Changes In Your New Application : Assignment_004_Part_B_Centering_of_Window.cpp
------------------------------------------------------------------------------------------------------

1. Globally : No Change

2. WinMain  : Changes are as follows :
				1. --> Added 1 extra variable of structure type RECT for storing the Screen Resolution of the monitor's work area -
					1. RECT rdWorkArea; (all members of the RECT Structure are of type double)


				2. --> Get Work Area (Resolution excluding Taskbar area)
					1. SystemParametersInfo(SPI_GETWORKAREA, 0, &rdWorkArea, 0);

				3. --> Calculate the position of New Window -

					1. iScreenWidth = (int)rdWorkArea.right;
					2. iScreenHeight = (int)rdWorkArea.bottom;

					3. iScreenWidthCentre = ((int)iScreenWidth / 2);
					4. iScreenHeightCentre = ((int)iScreenHeight / 2);

					5. iCreateWindowPositionX = iScreenWidthCentre - (iWindowWidth / 2);
					6. iCreateWindowPositionY = iScreenHeightCentre - (iWindowHeight / 2);
					
				4. --> Pass the parameters to CreateWindow()
						->  	
							iCreateWindowPositionX,
							iCreateWindowPositionY,
							iWindowWidth,
							iWindowHeight,
							
							hwnd = CreateWindow(szAppName,
								TEXT("FFP: Assignment_004_Part_A_Centering_of_Window : By Kshitij Badkas"),
								WS_OVERLAPPEDWINDOW,
								iCreateWindowPositionX,
								iCreateWindowPositionY,
								iWindowWidth,
								iWindowHeight,
								NULL,
								NULL,
								hInstance,
								NULL);

3. WndProc  : No Change
