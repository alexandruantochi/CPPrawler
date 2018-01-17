#include "stdafx.h"



std::set<std::string> visitedLinks;
std::map<std::string, DWORD> threadComm;
INT continueListening = 1;
HANDLE threadSync;

void init()
{

	HANDLE crawlerThread;
	HANDLE senderThread;

	if (NULL == (senderThread = CreateThread(NULL, 0, startProducerThread, NULL, CREATE_SUSPENDED, NULL)))
	{
		_error("Error creating sender thread.");
	}

	if (NULL == (crawlerThread = CreateThread(NULL, 0, startCrawlerThread, NULL, CREATE_SUSPENDED, NULL)))
	{
		_error("Error creating crawler thread.");
	}

	if (NULL == (threadSync = CreateMutex(NULL, FALSE, TEXT("threadSync"))))
	{
		_error("Error creating threadsync mutex.");
	}

	pause("Press any key to start..\n");

	if (-1 == (ResumeThread(crawlerThread)))
	{
		_error("Error starting crawler thread.");

	}

	if (-1 == (ResumeThread(senderThread)))
	{
		_error("Error starting producer thread.");

	}

	Sleep(INFINITE);

	/*
	if (0 == TerminateThread(crawlerThread, 0))
	{
	_error("Could not terminate crawler thread");
	}

	if (0 == TerminateThread(senderThread, 0))
	{
	_error("Could not terminate sender thread");
	}
	*/


}