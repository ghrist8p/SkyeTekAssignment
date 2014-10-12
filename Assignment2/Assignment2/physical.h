#ifndef PHYSICAL_H
#define PHYSICAL_H

#include <iostream>
#include <sstream>

#include "session.h"

#include "SkyeTek_C_API_4.2/SkyeTekAPI.h"
#include "SkyeTek_C_API_4.2/SkyeTekProtocol.h"
#include "SkyeTek_C_API_4.2/Platform.h"

#define WM_TAG_SCANNED ( WM_USER + 1 )

class Port
{
private:
	unsigned int numOfDevices;
	unsigned int numOfReaders;

	LPSKYETEK_DEVICE *lpsd;
	LPSKYETEK_READER *lpsr;

	HANDLE     hThrd;
	static DWORD WINAPI threadCallback(LPVOID lpVoid);
	static unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void *user);

	Console con;

public:
	HWND   sendMsgTo;

	Port(HWND);

	bool open();
	bool close();

	bool connected;
	std::string prevTag;
};


#endif