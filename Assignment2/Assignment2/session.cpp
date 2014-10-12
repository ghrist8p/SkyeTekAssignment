/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* SOURCE FILE: session.c                                                                                        *
*                                                                                                               *
* FUNCTIONS:                                                                                                    *
* Console::Console( void )                                                                                      *
* void Console::paintConsole( HWND hWnd )                                                                       *
* void Console::putChar( char c )                                                                               *
* void Console::incLine( void )                                                                                 *
* void Console::incCol( void )                                                                                  *
*                                                                                                               *
* DATE:        March 28, 2014                                                                                   *
*                                                                                                               *
* DESIGNER:    Georgi Hristov A00795026                                                                         *
*                                                                                                               *
* PROGRAMMER:  Georgi Hristov A00795026                                                                         *
*                                                                                                               *
* NOTES:       This class holds the data and methods nessecary to display text on a window.                     *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "session.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* FUNCTION:   constructor                                                                                       *
*                                                                                                               *
* DATE:       March 28, 2014                                                                                    *
*                                                                                                               *
* DESIGNER:   Georgi Hristov A00795026                                                                          *
*                                                                                                               *
* PROGRAMMER: Georgi Hristov A00795026                                                                          *
*                                                                                                               *
* INTERFACE:  Console::Console( void )                                                                          *
*                                                                                                               *
* NOTES:      This constructor initializes all the nessecary variables.                                         *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Console::Console(void)
{
	curLine = 0;
	curCol = 0;

	for (int i = 0; i < BUFFER; ++i)
		lines[i][0] = '\0';

	font = CreateFont(TEXT_HEIGHT         //nHeight
		, 0                   //nWidth
		, 0                   //nEscapement
		, 0                   //nOrientation
		, FW_DONTCARE         //fnWeight
		, FALSE               //fdwItalic
		, FALSE               //fdwUnderline
		, FALSE               //fdwStrikeOut
		, ANSI_CHARSET        //fdwCharSet
		, OUT_DEFAULT_PRECIS  //fdwOutputPrecision
		, CLIP_DEFAULT_PRECIS //fdwClipPrecision
		, DEFAULT_QUALITY     //fdwQuality
		, DEFAULT_PITCH       //fdwPitchAndFamily
		, "Consolas");       //lpszFace
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* FUNCTION:   paintConsole                                                                                      *
*                                                                                                               *
* DATE:       March 28, 2014                                                                                    *
*                                                                                                               *
* DESIGNER:   Georgi Hristov A00795026                                                                          *
*                                                                                                               *
* PROGRAMMER: Georgi Hristov A00795026                                                                          *
*                                                                                                               *
* INTERFACE:  void Console::paintConsole( HWND hWnd )                                                           *
*                    HWND hWnd: The window to paint to.                                                         *
*                                                                                                               *
* RETURNS:    void.                                                                                             *
*                                                                                                               *
* NOTES:      Paints all the stored text to the console.                                                        *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Console::paintConsole(HWND hWnd)
{
	PAINTSTRUCT paintstruct;
	HDC hdc = BeginPaint(hWnd, &paintstruct); // Acquire DC
	RECT clientRect;

	GetClientRect(hWnd, &clientRect);

	int height = clientRect.bottom - clientRect.top;

	SelectObject(hdc, font);
	if ((curLine + 1) * TEXT_HEIGHT > height)
	{
		for (int i = 0; i * TEXT_HEIGHT < height; ++i)
		{
			TextOut(hdc
				, 0
				, height - ((i + 1) * TEXT_HEIGHT)
				, lines[curLine - i]
				, strlen(lines[curLine - i])); // output character
		}
	}
	else
	{
		for (int i = 0; i <= curLine; ++i)
		{
			TextOut(hdc
				, 0
				, i * TEXT_HEIGHT
				, lines[i]
				, strlen(lines[i])); // output character
		}
	}

	EndPaint(hWnd, &paintstruct); // Release DC
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* FUNCTION:   putChar                                                                                           *
*                                                                                                               *
* DATE:       March 28, 2014                                                                                    *
*                                                                                                               *
* DESIGNER:   Georgi Hristov A00795026                                                                          *
*                                                                                                               *
* PROGRAMMER: Georgi Hristov A00795026                                                                          *
*                                                                                                               *
* INTERFACE:  void Console::putChar( char c )                                                                   *
*                    char c: The char to be stored.                                                             *
*                                                                                                               *
* RETURNS:    void.                                                                                             *
*                                                                                                               *
* NOTES:      Stores a single char.                                                                             *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Console::putChar(char c)
{
	if (c == '\n' || c == '\r')
	{
		incLine();
	}
	else
	{
		lines[curLine][curCol] = c;
		incCol();
		lines[curLine][curCol] = '\0';
	}
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* FUNCTION:   incLine                                                                                           *
*                                                                                                               *
* DATE:       March 28, 2014                                                                                    *
*                                                                                                               *
* DESIGNER:   Georgi Hristov A00795026                                                                          *
*                                                                                                               *
* PROGRAMMER: Georgi Hristov A00795026                                                                          *
*                                                                                                               *
* INTERFACE:  void Console::incLine( void )                                                                     *
*                                                                                                               *
* RETURNS:    void.                                                                                             *
*                                                                                                               *
* NOTES:      Increments the line counter properly.                                                             *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Console::incLine(void)
{
	++curLine;
	curCol = 0;
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* FUNCTION:   incCol                                                                                            *
*                                                                                                               *
* DATE:       March 28, 2014                                                                                    *
*                                                                                                               *
* DESIGNER:   Georgi Hristov A00795026                                                                          *
*                                                                                                               *
* PROGRAMMER: Georgi Hristov A00795026                                                                          *
*                                                                                                               *
* INTERFACE:  void Console::incCol( void )                                                                      *
*                                                                                                               *
* RETURNS:    void.                                                                                             *
*                                                                                                               *
* NOTES:      Increments the column counter properly.                                                           *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Console::incCol(void)
{
	++curCol;
	if (curCol >= BUFFER - 1)
	{
		lines[curLine][BUFFER - 1] = '\0';
		incLine();
	}
	return;
}