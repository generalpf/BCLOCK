#include <windows.h> 
#include <memory.h>
#include <time.h>
#include <commdlg.h>
#include "bclock.h"

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdParam, int nCmdShow)
{
	static char szAppName[] = "BClock";
	HWND	hwnd ;
	MSG		msg ;
	WNDCLASS	wndclass ;
	HMENU	hMenu;

	if (!hPrevInstance)
	{
		wndclass.style			= CS_HREDRAW | CS_VREDRAW ;
		wndclass.lpfnWndProc	= WndProc ;
		wndclass.cbClsExtra		= 0 ;
		wndclass.cbWndExtra		= 0 ;
		wndclass.hInstance		= hInstance ;
		wndclass.hIcon			= LoadIcon(NULL,IDI_APPLICATION) ;
		wndclass.hCursor		= LoadCursor(NULL,IDC_ARROW) ;
		wndclass.hbrBackground	= NULL; // I will supply my own background
		wndclass.lpszMenuName	= NULL ;
		wndclass.lpszClassName	= szAppName ;

		RegisterClass (&wndclass) ;
	}

	hwnd = CreateWindow(szAppName,			// window class name
						"BaseClock",		// window caption
						WS_OVERLAPPEDWINDOW,	// window style
						CW_USEDEFAULT,		// initial x position
						CW_USEDEFAULT,		// initial y position
						500,				// initial x size
						250,				// initial y size
						NULL,				// parent window handle
						NULL,				// window menu handle
						hInstance,			// program instance handle
						NULL);				// creation parameters

	hMenu = LoadMenu(hInstance,"BCLOCK");
	SetMenu(hwnd,hMenu);    
	
	CheckMenuItem(hMenu,IDM_DEC,MF_CHECKED);
	            
	ShowWindow (hwnd,nCmdShow);
	UpdateWindow(hwnd);                       
                                                                                     
	SetTimer(hwnd,BCLOCK_TIMER,1000,NULL);

	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

long FAR PASCAL _export WndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
	HDC hdc;   				// the device context for drawing and bkgnd fills
	PAINTSTRUCT ps;			// the paint structure for WM_PAINT
	RECT rect;				// rectangle for GetClientRect()
	HBRUSH hBackgroundBrush, hOldBrush;		// background brush, and temp. old brush
    static LOGBRUSH lb;
	
	HFONT hFont, hOldFont;		// font for drawing, and temp. old font
	static LOGFONT lf, tempLf;		// the logical font for CreateFontIndirect()
									// a temporary font is used so the clock doesn't
									// get messed up while in the Font dialog
	static CHOOSEFONT cf;		// structure for the ChooseFont() function
	static CHOOSECOLOR cc;		// structure for the ChooseColor() function
	static COLORREF custom[16];	// the custom colors for the Color dialog

    char time_string[128], dayhalf;	// junk for getting and formatting the time
	struct tm* datetime;
	time_t lTime;  
	
	static int Base = 10, MenuBase = IDM_DEC;	// Base for printing and menu base
	int i;										// for loop...  this compiler SUCKS
		
	HMENU hMenu;			// menu handle for SetCheck() etc.

	switch (message)
	{
		case WM_CREATE:
			lb.lbStyle = BS_SOLID;			// solid background
			lb.lbColor = RGB(255,255,255);	// default to a white background
			lb.lbHatch = NULL;				// no hatch

			memset(&lf,0,sizeof(LOGFONT));				// clear all fields
			lstrcpy(lf.lfFaceName,"Times New Roman");	// default Times New Roman
														// default pt. size?
		
			memset(&cf, 0, sizeof(CHOOSEFONT));			// clear all fields
			cf.lStructSize = sizeof(CHOOSEFONT);		
			cf.hwndOwner = hwnd;
			cf.lpLogFont = &tempLf;                     // work done goes in the temp
			cf.Flags = CF_SCREENFONTS | CF_EFFECTS;		// RTFM for this one
			cf.rgbColors = RGB(0,0,0);					// default to black text
			cf.nFontType = SCREEN_FONTTYPE;				// see cf.Flags

			for (i = 0; i < 16; i++)					// set all custom colors to white
				custom[i] = RGB(255,255,255);
			memset(&cc,0,sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hwnd;
			cc.rgbResult = RGB(255,255,255);			// Default to white
			cc.lpCustColors = custom;
			cc.Flags = CC_FULLOPEN;

			return 0;
		case WM_COMMAND:
			switch(wParam)
			{
				case IDM_BIN:    
				case IDM_OCT:	
				case IDM_DEC:
				case IDM_HEX:	hMenu = GetMenu(hwnd);      			
       							CheckMenuItem(hMenu,MenuBase,MF_UNCHECKED);	// uncheck the current one
       							MenuBase = wParam;							// the menu base is wParam
       							CheckMenuItem(hMenu,MenuBase,MF_CHECKED);	// check the new one
	       						switch(wParam)
       							{             							// set the mathematical base
       								case IDM_BIN:	Base = 2; break;
       								case IDM_OCT:	Base = 8; break;
       								case IDM_DEC:	Base = 10; break;
	       							case IDM_HEX:	Base = 16;
       							}
           						break;
           		case IDM_ARBITRARY:	hMenu = GetMenu(hwnd);
									CheckMenuItem(hMenu,MenuBase,MF_UNCHECKED);		// uncheck the current one
									MenuBase = IDM_ARBITRARY;						
									CheckMenuItem(hMenu,MenuBase,IDM_ARBITRARY);   	// check the arbitrary item
     	         					Base = 10;				// until dialog implemented, arbitrary == decimal
     	         					break;
         		case IDM_FONT:	if (ChooseFont(&cf))
									lf = tempLf;         		
								break;
				case IDM_BACKGROUND:	if (ChooseColor(&cc))
											lb.lbColor = cc.rgbResult;
										break;
         		case IDM_ABOUT:	MessageBox(hwnd,(LPCSTR) "BaseClock 0.91\nDeveloped by Ryan Walberg",(LPCSTR) "About BaseClock",MB_APPLMODAL | MB_ICONINFORMATION | MB_OK);
         	}

         	InvalidateRect(hwnd,NULL,TRUE);
         	return 0;

		case WM_PAINT:
	      		hdc = BeginPaint(hwnd,&ps);
				GetClientRect(hwnd,&rect);
			
				time(&lTime);
				datetime = localtime(&lTime);  
				dayhalf = 'a';
				if (datetime->tm_hour > 12)
				{
					dayhalf = 'p';
					datetime->tm_hour -= 12;
				}
				
				hFont = CreateFontIndirect(&lf);
         		hOldFont = SelectObject(hdc,hFont);
         		
         		wsprintf(time_string,"%s:%s:%s%c",(LPSTR) base_cvt(Base,datetime->tm_hour),(LPSTR) base_cvt(Base,datetime->tm_min),(LPSTR) base_cvt(Base,datetime->tm_sec),dayhalf);
  				SetTextColor(hdc,cf.rgbColors);
  				SetBkColor(hdc,cc.rgbResult);
  				DrawText(hdc,time_string, -1, &rect,DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      
      			SelectObject(hdc,hOldFont);
         		DeleteObject(hFont);
		      	EndPaint(hwnd,&ps);
				return 0;                     

		case WM_ERASEBKGND:
	        hdc = GetDC(hwnd);

			hBackgroundBrush = CreateBrushIndirect(&lb);
			hOldBrush = SelectObject(hdc,hBackgroundBrush);
			UnrealizeObject(hBackgroundBrush);
		
			GetClientRect(hwnd,&rect);
			Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);

			SelectObject(hdc,hOldBrush);
			DeleteObject(hBackgroundBrush);
			ReleaseDC(hwnd,hdc);
			return 1; 
						
		case WM_TIMER:
			InvalidateRect(hwnd,NULL,TRUE);
			return 0;
	
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hwnd,message,wParam,lParam);
}
                                   
               