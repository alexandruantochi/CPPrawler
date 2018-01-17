#include "stdafx.h"


DWORD WINAPI startProducerThread(LPVOID lParam)
{

	

	HANDLE consumerEvent = NULL;
	HANDLE producerEvent = NULL;
	HANDLE hMap;
	LPVOID mFileHandle = NULL;


	getSyncEvents(&producerEvent, &consumerEvent);

	printf("Waiting for consumer to create mapped file...\n");

	if (WAIT_TIMEOUT == WaitForSingleObject(consumerEvent, INFINITE))
	{
		_error("Event timed out.");
	}


	getMappedFileHandle(&hMap, &mFileHandle);


	printf("[producerThread] Mapped file aquired..\n");

	if (!SetEvent(producerEvent))
	{
		_error("Error setting producer event.");
	}

	bool dataWasRead;

	std::map<std::string, DWORD>::iterator itt;
	LPSTR pageUrl = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * 256);
	DWORD pageSize;


	printf("Sender thread started.\n");

	while (continueListening)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(threadSync, INFINITE))
		{
			_error("Mutex timed out @sender.\n");
		}

		if (dataWasRead = !threadComm.empty())
		{
			itt = threadComm.begin();

			StrCpy(pageUrl,itt->first.c_str());

			pageSize = itt->second;

			threadComm.erase(itt->first);
		}

		if (!ReleaseMutex(threadSync))
		{
			_error("Error releasing mutex.");
		}

		if (dataWasRead)
		{
			dataWasRead = false;

			if (WAIT_TIMEOUT == WaitForSingleObject(consumerEvent, INFINITE))
			{
				_error("Event timed out.");
			}

			sendData(mFileHandle, pageUrl, pageSize);

			if (!SetEvent(producerEvent))
			{
				_error("Error setting producer event.");
			}

		}

	}

	printf("Sender thread finished.\n");
	return 0;

}