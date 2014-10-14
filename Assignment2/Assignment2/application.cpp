#include "application.h"

int WINAPI WinMain( HINSTANCE hInst          //_In_  HINSTANCE hInstance,
				  , HINSTANCE hprevInstance  //_In_  HINSTANCE hPrevInstance,
				  , LPSTR lspszCmdParam      //_In_  LPSTR lpCmdLine,
				  , int nCmdShow )           //_In_  int nCmdShow
{
	char const Name[] = "RFID Scanner";

	HWND hWnd;
	MSG Msg;
	WNDCLASSEX windowClass;

	//Set up window class
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hIconSm = NULL;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	windowClass.lpfnWndProc = WndProc;
	windowClass.hInstance = hInst;
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszClassName = Name;

	windowClass.lpszMenuName = TEXT("MYMENU");
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = sizeof(Console*)+sizeof(Port*);

	if (!RegisterClassEx(&windowClass))
		return 0;

	hWnd = CreateWindow( Name                 //_In_opt_  LPCTSTR lpClassName,
					   , Name                 //_In_opt_  LPCTSTR lpWindowName,
					   , WS_OVERLAPPEDWINDOW  //_In_      DWORD dwStyle,
					   , 10                   //_In_      int x,
					   , 10                   //_In_      int y,
					   , 600                  //_In_      int nWidth,
					   , 400                  //_In_      int nHeight,
					   , NULL                 //_In_opt_  HWND hWndParent,
					   , NULL                 //_In_opt_  HMENU hMenu,
					   , hInst                //_In_opt_  HINSTANCE hInstance,
					   , NULL);               //_In_opt_  LPVOID lpParam

	//set up main objects
	Console c;
	Port p(hWnd);
	//and store them in the window
	SetWindowLongPtr( hWnd            //_In_  HWND hWnd,
					, 0               //_In_  int nIndex,
					, (LONG_PTR)&c ); //_In_  LONG_PTR dwNewLong
		
	SetWindowLongPtr( hWnd              //_In_  HWND hWnd,
					, sizeof(Console*)  //_In_  int nIndex,
					, (LONG_PTR)&p );   //_In_  LONG_PTR dwNewLong
		

	//display window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//handle message
	while( GetMessage( &Msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &Msg );
		DispatchMessage( &Msg );
	}

	return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd
	, UINT Message
	, WPARAM wParam
	, LPARAM lParam)
{
	//retrieve main objects from window
	Console* c = (Console*)GetWindowLongPtr(hWnd, 0);
	Port*    p = (Port*)GetWindowLongPtr(hWnd, sizeof(Console*));

	switch (Message)
	{
	case WM_COMMAND: //when a menu item is clicked
		switch( LOWORD(wParam) )
		{
		case IDM_CONNECT:
			p->open();
			break;
		case IDM_DISCONNECT:
			p->close();
			break;
		case IDM_EXIT:
			PostQuitMessage(0);
			break;
		default:
			MessageBox( NULL, "WM_COMMAND", "DEBUG", MB_OK );
		}
		break;

	case WM_TAG_SCANNED: //when a char has been received
		for (char * i = (char*)lParam; *i != '\0'; ++i)
			(*c).putChar(*i);
		InvalidateRect(hWnd, NULL, TRUE);
		//MessageBox(NULL, (char*)lParam, "WM_TAG_SCANNED", MB_OK);
		break;

	case WM_PAINT: //when a window need to repaint
		(*c).paintConsole(hWnd);
		break;

	case WM_CHAR: //when a char has been typed
		switch( wParam )
		{
		default:
			MessageBox( NULL, "WM_CHAR", "DEBUG", MB_OK );
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc( hWnd, Message, wParam, lParam );
	}
	return 0;
}
