#include "stdafx.h"

void getMappedFileHandle(PHANDLE hMap, LPVOID* mFileHandle)
{

	printf("Waiting for consumer to create mapped file...\n");


	if (NULL == (*hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, TEXT("consumer"))))
	{
		_error("Error opening mapped file on sender side.");
	}

	if (NULL == (*mFileHandle = MapViewOfFile(*hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0)))
	{
		_error("Erorr mapping view of file.");
	}

}

void getSyncEvents(PHANDLE producerEvent, PHANDLE consumerEvent)
{

	if (NULL == (*producerEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("producerEvent"))))
	{
		_error("Error creating producer event.");
	}



	if (NULL == (*consumerEvent = OpenEvent(SYNCHRONIZE, FALSE, TEXT("consumerEvent"))))
	{
		_error("Error opening consumer event on sender side.");
	}

}