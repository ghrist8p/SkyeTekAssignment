#ifndef SESSION_H
#define SESSION_H

#include <windows.h>
#include <stdio.h>

class Console
{
private:
	static int const BUFFER = 512;

	static int const TEXT_HEIGHT = 22;
	static int const TEXT_WIDTH = 10;

	char lines[BUFFER][BUFFER];
	int curLine;
	int curCol;

	HFONT font;

	void incLine(void);
	void incCol(void);

public:
	Console(void);
	void putChar(char c);
	void paintConsole(HWND hwnd);
};
#endif