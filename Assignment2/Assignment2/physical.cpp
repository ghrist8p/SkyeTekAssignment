/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Physical.cpp - Outlines functions which are involved in the physical layer.
--
-- PROGRAM: RFID reader
--
-- FUNCTIONS:
-- 
-- DATE: 
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Sebastian Pelka A00870247 and Georgi Hristov A00795026  
--
-- PROGRAMMER: 
--
-- NOTES: These funcions are involved with activity using ports and communicating with other devices.
----------------------------------------------------------------------------------------------------------------------*/

#include "physical.h"


Port::Port(HWND hWnd)
{
	numOfDevices = 0;
	numOfReaders = 0;

	lpsd = NULL;
	lpsr = NULL;

	connected = false;

	sendMsgTo = hWnd;
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: open
--
-- DATE: October 11, 2014
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Sebastian Pelka and Georgi Hristov A00795026  
--
-- PROGRAMMER: Sebastian Pelka
--
-- NOTES: Checks to see if a Skyetek device is connected to the computer. If a device is detected, searches for a
		  reader. If a reader is detected, the function creates a thread, which calls the threadCallback function.
----------------------------------------------------------------------------------------------------------------------*/
bool Port::open()
{
	//find a device that is connected via USB to the host
	if ((numOfDevices = SkyeTek_DiscoverDevices(&lpsd)) > 0)
	{
		//if a device is found, determine if readers are found/present
		if (numOfReaders = SkyeTek_DiscoverReaders(lpsd, numOfDevices, &lpsr) > 0)
		{
			connected = true;
			//create thread
			if ((hThrd = CreateThread(NULL          //_In_opt_   LPSECURITY_ATTRIBUTES lpThreadAttributes,
				, 0             //_In_       SIZE_T dwStackSize,
				, threadCallback      //_In_       LPTHREAD_START_ROUTINE lpStartAddress,
				, this          //_In_opt_   LPVOID lpParameter,
				, 0             //_In_       DWORD dwCreationFlags,
				, NULL)   //_Out_opt_  LPDWORD lpThreadId
				) == NULL)
			{
				MessageBox( NULL
						  , TEXT("Error creating readLoop thread!")
						  , TEXT("")
						  , MB_OK );
				return false;
			}
			//MessageBox the number of readers connected
			MessageBox(NULL, "Device and Reader found!", "", MB_OK);
			return true;
		}
		else
		{
			MessageBox(NULL, "Reader not found!", "", MB_OK);
			return false;
		}
	}
	else
	{
		MessageBox(NULL, "Device not found!", "", MB_OK);
		return false;
	}
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: open
--
-- DATE: October 8, 2014
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Georgi Hristov and Sebastian Pelka
--
-- PROGRAMMER: Georgi Hristov
--
-- NOTES: Disconnects the host machine from the device and closes the connection, freeing the port
----------------------------------------------------------------------------------------------------------------------*/
bool Port::close()
{
	connected = false;
	Sleep(4000);
	SkyeTek_FreeReaders(lpsr, numOfReaders);
	SkyeTek_FreeDevices(lpsd, numOfDevices);
	return true;
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: threadCallback
--
-- DATE:
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER:
--
-- PROGRAMMER:
--
-- NOTES:
----------------------------------------------------------------------------------------------------------------------*/

DWORD WINAPI Port::threadCallback(LPVOID lpVoid)
{
	Port* thiz = (Port*)lpVoid;
	SKYETEK_STATUS st;
	st = SkyeTek_SelectTags( thiz->lpsr[0]    //lpReader Reader to execute this command on.
						   , AUTO_DETECT                   //tagType Select only a specific tag type. 
						   , SelectLoopCallback			   //callback Function to call when a tag is found.
						   								   // The return of this function determines when this
						   								   // call completes if in loop mode (0 to stop, 1 to continue)
    					   , 0							   //inv true(1) indicates the reader should run in inventory/anti-collision mode
						   , 1							   //loop Run reader in loop mode, reader will continually scan for tags in its field.	
						   , lpVoid);						   //user User data to pass to callback along with tag
	if (st != SKYETEK_SUCCESS)
		MessageBox(NULL, "Select loop failed", "", MB_OK);
	MessageBox(NULL, "Select loop done", "", MB_OK);

	return 0L;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SelectLoopCallback
--
-- DATE:
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER:
--
-- PROGRAMMER:
--
-- NOTES:
----------------------------------------------------------------------------------------------------------------------*/
unsigned char Port::SelectLoopCallback(LPSKYETEK_TAG lpTag, void *user)
{
	Port* thiz = (Port*)user;
	std::stringstream ss;
	//if lpTag->friendly == null
		
		//set the tag friendly to ""
	//if lpTag != null
		//if tag friendly == lpTag friendly
			//return and don't print
		//else
			//copy save lpTag for future comparason
			//post print event
	

	if (lpTag != NULL )
	{


		for (int i = 0; i < sizeof(lpTag->friendly); i++)
		{
			ss << (lpTag->friendly + i);
		}
		
		if (thiz->prevTag.compare( ss.str() ) == 0)
		{
			return true;
		}

		thiz->prevTag = ss.str();

		OutputDebugString(ss.str().c_str());
		ss.str("");//clear string stream

		ss << "Tag: ";
		for (int i = 0; i < sizeof(lpTag->friendly); i++)
		{
			ss << (lpTag->friendly + i);
		}
		ss << "  Type: ";
		for (int i = 0; i < (sizeof(SkyeTek_GetTagTypeNameFromType(lpTag->type)) * 16); i++)
		{
			ss << SkyeTek_GetTagTypeNameFromType(lpTag->type) + i;
		}
		ss << std::endl;
		SkyeTek_FreeTag(lpTag);

		SendMessage(thiz->sendMsgTo, WM_TAG_SCANNED, 0, (LPARAM)ss.str().c_str());

		OutputDebugString(ss.str().c_str());
		return thiz->connected;
	}
	else
	{
		thiz->prevTag = "";
		PostMessage(thiz->sendMsgTo, WM_TAG_REMOVED, 0, 0);
		return thiz->connected;
	}
}