#include "application.h"

int WINAPI WinMain( HINSTANCE hInst          //_In_  HINSTANCE hInstance,
				  , HINSTANCE hprevInstance  //_In_  HINSTANCE hPrevInstance,
				  , LPSTR lspszCmdParam      //_In_  LPSTR lpCmdLine,
				  , int nCmdShow )           //_In_  int nCmdShow
{
	char const Name[] = "RFID Scanner";

	HWND hWnd;
    HWND hStatus;
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
					   , 800                  //_In_      int nWidth,
					   , 400                  //_In_      int nHeight,
					   , NULL                 //_In_opt_  HWND hWndParent,
					   , NULL                 //_In_opt_  HMENU hMenu,
					   , hInst                //_In_opt_  HINSTANCE hInstance,
					   , NULL);               //_In_opt_  LPVOID lpParam

    //status bar
    InitCommonControls();

    hStatus = CreateWindowEx( 0
                            , STATUSCLASSNAME
                            , NULL
                            , WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP
                            , 0
                            , 0
                            , 0
                            , 0
                            , hWnd
                            , (HMENU)IDC_MAIN_STATUS
                            , GetModuleHandle(NULL)
                            , NULL);

	int Statwidths[] = {150, -1};

	SendMessage( hStatus
               , SB_SETPARTS
               , (WPARAM)( sizeof(Statwidths)/sizeof(int) )
               , (LPARAM)Statwidths );
	SendMessage( hStatus
               , SB_SETTEXT
               , 0
               , (LPARAM)TEXT("Disconected") );
	SendMessage( hStatus
               , SB_SETTEXT
               , 1
               , (LPARAM)TEXT("No Tag") );

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
            if( p->open() )
            {
                SendMessage( GetDlgItem( hWnd, IDC_MAIN_STATUS )
                           , SB_SETTEXT
                           , 0
                           , (LPARAM)TEXT("Connected") );
            }
			break;
		case IDM_DISCONNECT:
            if( p->close() )
            {
                SendMessage( GetDlgItem( hWnd, IDC_MAIN_STATUS )
                           , SB_SETTEXT
                           , 0
                           , (LPARAM)TEXT("Disconected") );
            }
			break;
		case IDM_EXIT:
			PostQuitMessage(0);
			break;
        case IDM_HELP:
            MessageBox( NULL
                      , "This is a simple RFID tag Scanner.\n"
                        "To connect to a pluged-in SkyeTek device, press Connect.\n"
                        "Place RFID tags over RFID reader to read it.\n"
                        "When finished, press Disconnect, to disconnect and Exit.\n"
                        "About:\n"
                        "  Programmed and designed by:\n"
                        "    Sebastian Pelka\n"
                        "    Georgi Hristov"
                      , "Help"
                      , MB_OK );
            break;
		default:
			MessageBox( NULL, "Uh oh...\nThis app seems to be incomplete.", "WM_CHAR", MB_OK );
		}
		break;

	case WM_TAG_SCANNED: //when a char has been received
        
        SendMessage( GetDlgItem( hWnd, IDC_MAIN_STATUS )
                   , SB_SETTEXT
                   , 1
                   , lParam );

		for (char * i = (char*)lParam; *i != '\0'; ++i)
			(*c).putChar(*i);
		InvalidateRect(hWnd, NULL, TRUE);
		//MessageBox(NULL, (char*)lParam, "WM_TAG_SCANNED", MB_OK);
		break;

    case WM_TAG_REMOVED:
        
        SendMessage( GetDlgItem( hWnd, IDC_MAIN_STATUS )
                   , SB_SETTEXT
                   , 1
                   , (LPARAM)TEXT("No Tag") );
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

    case WM_SIZE:

        SendMessage( GetDlgItem( hWnd, IDC_MAIN_STATUS ), WM_SIZE, 0, 0 );
        break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc( hWnd, Message, wParam, lParam );
	}
	return 0;
}
