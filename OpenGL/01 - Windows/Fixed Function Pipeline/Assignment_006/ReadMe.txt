-----------------------------------------------------------------------------------------------------
										PART A
-----------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Assignment Name   		: WM_PAINT
------------------------------------------------------------------------------------------------------
Assignment Number 		: 006
------------------------------------------------------------------------------------------------------
Base Assignment   		: Assignment_006
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_006_BeginPaint()_Used_In_WM_PAINT
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Assignment_006_BeginPaint()_Used_In_WM_PAINT.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Make These All Below Changes In Your New Application : Assignment_006_BeginPaint()_Used_In_WM_PAINT.cpp
------------------------------------------------------------------------------------------------------

1. Globally : No Changes

2. WinMain  : No Changes

3. WndProc  : Changes are as follows: 
				1. -->  Added code for using BeginPaint WM_PAINT. Declared 4 local variables.

						HDC hdc;
						PAINTSTRUCT ps;
						RECT rc;
						TCHAR str[] = TEXT("Hello World !!!");

				2. -->  Added code in the Switch case

					case WM_PAINT:
						GetClientRect(hwnd, &rc);
						hdc = BeginPaint(hwnd, &ps);
						SetBkColor(hdc, RGB(0, 0, 0));
						SetTextColor(hdc, RGB(0, 255, 0));
						DrawText(hdc, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						EndPaint(hwnd, &ps);
						break;


-----------------------------------------------------------------------------------------------------
										PART B
-----------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Assignment Name   		: WM_PAINT
------------------------------------------------------------------------------------------------------
Assignment Number 		: 006
------------------------------------------------------------------------------------------------------
Base Assignment   		: Assignment_006
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_006_GetDC()_Used_In_WM_LBUTTONDOWN
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Assignment_006_GetDC()_Used_In_WM_LBUTTONDOWN.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Make These All Below Changes In Your New Application : Assignment_006_GetDC()_Used_In_WM_LBUTTONDOWN.cpp
------------------------------------------------------------------------------------------------------

1. Globally : No Changes

2. WinMain  : No Changes

3. WndProc  : Changes are as follows: 
				1. -->  Added code for using GetDC() in WM_LBUTTONDOWN. Declared 3 local variables.

						HDC hdc;
						RECT rc;
						TCHAR str[] = TEXT("Hello World !!!");

				2. -->  Added code in the Switch case

					case WM_LBUTTONDOWN:
						GetClientRect(hwnd, &rc);
						hdc = GetDC(hwnd);
						SetBkColor(hdc, RGB(0, 0, 0));
						SetTextColor(hdc, RGB(0, 255, 0));
						DrawText(hdc, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						ReleaseDC(hwnd, hdc);
						break;


-----------------------------------------------------------------------------------------------------
										PART B
-----------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Assignment Name   		: WM_PAINT
------------------------------------------------------------------------------------------------------
Assignment Number 		: 006
------------------------------------------------------------------------------------------------------
Base Assignment   		: Assignment_006
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
Created New Folder(s) 	: Assignment_006_GetDC()_Used_In_WM_PAINT
------------------------------------------------------------------------------------------------------
Created New File(s) 	: Assignment_006_GetDC()_Used_In_WM_PAINT.cpp
------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------
Make These All Below Changes In Your New Application : Assignment_006_GetDC()_Used_In_WM_PAINT.cpp
------------------------------------------------------------------------------------------------------

1. Globally : No Changes

2. WinMain  : No Changes

3. WndProc  : Changes are as follows: 
				1. -->  Added code for using GetDC() in WM_LBUTTONDOWN. Declared 3 local variables.

						HDC hdc;
						RECT rc;
						TCHAR str[] = TEXT("Hello World !!!");

				2. -->  Added code in the Switch case

					case WM_PAINT:
						GetClientRect(hwnd, &rc);
						hdc = GetDC(hwnd);
						SetBkColor(hdc, RGB(0, 0, 0));
						SetTextColor(hdc, RGB(0, 255, 0));
						DrawText(hdc, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
						ReleaseDC(hwnd, hdc);
						break;