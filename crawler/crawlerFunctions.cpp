#include "stdafx.h"

#include <cstring>


std::set<std::string> visitedLinks;
std::map<std::string, DWORD> threadComm;
HANDLE threadSync;
INT continueListening = 1;
LPVOID mapView;


#define MAX_URLS 256

void downloadPage(LPCSTR pageUrl, LPSTR filePath)
{

	DWORD trheadId = GetCurrentThreadId();

	StrCpy(filePath, "F:\\Scoala\\Anul III\\Sem I\\CSSO\\Lab\\Proiect\\downloads\\");
	StrCat(filePath, parseUrl(pageUrl, PARSEURL_FILENAME));

	snprintf(filePath + lstrlen(filePath), 255 - lstrlen(filePath), "_%u", GetCurrentThreadId());

	DWORD result;

	if (S_OK != (result = URLDownloadToFileA(NULL, pageUrl, filePath, 0, NULL)))
	{
		filePath = NULL;
	}

}


bool processPage(LPCSTR filePath, LPDWORD fileSize, LPCSTR pageUrl, LPSTR* pageUrls)
{

	if (filePath == NULL)
	{
		pageUrls[0] = NULL;
		return false;
	}

	HANDLE hFile;

	if (INVALID_HANDLE_VALUE == (hFile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
	{
		return false;
	}

	if (INVALID_FILE_SIZE == (*fileSize = GetFileSize(hFile, NULL)))
	{
		return false;
	}


	LPSTR currentUrl = NULL;
	LPSTR urlGenerator = NULL;
	LPSTR aTag = NULL;
	LPSTR href = NULL;
	DWORD bytesRead;

	INT j;
	INT k = 0;
	INT bufferOffset = 0;

	LPSTR buffer = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * 2048);

	while (ReadFile(hFile, buffer, 2048, &bytesRead, NULL) && bytesRead)
	{
		if (k == MAX_URLS - 1)
		{
			pageUrls[k] = NULL;
			LocalFree(buffer);
			CloseHandle(hFile);
			return true;
		}

		bufferOffset = 0;


		do {

			aTag = StrStr(buffer + bufferOffset, "<a ");

			if (!aTag)
			{
				break;
			}

			href = StrStr(aTag, "href=\"");

			if (!href)
			{
				break;
			}

			bufferOffset = href - buffer;

			j = 6;
			while (*(href + j) != '"')
			{
				j++;
				// corrupt href? abort at 120
				if (j > 120)
				{
					break;
				}
			}
			if (j > 120)
			{
				continue;
			}


			currentUrl = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * 256);



			memcpy(currentUrl, href + 6, j - 6);
			currentUrl[j - 6] = '\0';

			if (!linkHasValidExt(currentUrl))
			{
				continue;
			}

			if (isLinkWebsite(currentUrl))
			{
				if (currentUrl[j - 7] != '/')
				{
					StrCat(currentUrl, "/");
				}
				pageUrls[k++] = currentUrl;
				continue;
			}

			if (isLinkToRoot(currentUrl))
			{
				if (joinRootLink(pageUrl, currentUrl))
				{
					pageUrls[k++] = currentUrl;
					continue;
				}
			}

			if (joinLink(pageUrl, currentUrl))
			{
				pageUrls[k++] = currentUrl;
				continue;
			}

		} while (aTag != NULL);
	}

	if (k == 0)
	{
		pageUrls[0] = NULL;
		CloseHandle(hFile);
		LocalFree(buffer);
		return false;
	}

	if (k != 255)
	{
		pageUrls[k] = NULL;
	}

	CloseHandle(hFile);
	LocalFree(buffer);

	/* delete file after processing
	if (!DeleteFile(filePath))
	{
		_error("Error clearing memory.");
	}
	*/

	return true;
}

bool readAdressesFromRegistry(LPCSTR registryName, LPSTR* links)
{
	HKEY hkResult;
	DWORD valueCount;
	DWORD maxKeySize;
	DWORD maxKeyValueSize;

	int errCode;

	printf("Reading from %s ... \n", registryName);

	if (ERROR_SUCCESS != (errCode = RegOpenKey(HKEY_LOCAL_MACHINE, registryName, &hkResult)))
	{
		SetLastError(errCode);
		_error("Failed to open key.");
	}

	if (ERROR_SUCCESS != (errCode = RegQueryInfoKey(hkResult, NULL, NULL, NULL, NULL, NULL, NULL, &valueCount, &maxKeySize, &maxKeyValueSize, NULL, NULL)))
	{
		SetLastError(errCode);
		_error("Failed to query key information.");
	}

	if (valueCount > 256)
	{
		_error("Only the first 256 value will be read from the registry.");
	}

	DWORD cbData;
	LPTSTR lpvalueName = (LPTSTR)LocalAlloc(LPTR, 256);
	LPDWORD lpType = (LPDWORD)LocalAlloc(LPTR, sizeof(DWORD));


	for (DWORD i = 0; i < valueCount; i++)
	{

		cbData = maxKeyValueSize;
		DWORD cchValueName = 256;
		LPBYTE lpData = (LPBYTE)LocalAlloc(LPTR, sizeof(BYTE)*maxKeyValueSize);

		if (ERROR_SUCCESS != (errCode = RegEnumValue(hkResult, i, lpvalueName, &cchValueName, NULL, lpType, lpData, &cbData)))
		{
			SetLastError(errCode);
			_error("Error reading key value index");
		}

		if (*lpType != REG_SZ)
		{
			continue;
		}

		links[i] = (LPSTR)lpData;
	}

	links[valueCount] = NULL;

	CloseHandle(hkResult);
	LocalFree(lpvalueName);
	LocalFree(lpType);

	return valueCount > 0;

}

void crawl(LPCSTR currentUrl, UINT depth, UINT maxDepth)
{

	if (depth > maxDepth)
	{
		printf("Reached max depth at %s.\n", currentUrl);
		return;
	}

	LPDWORD size = (LPDWORD)LocalAlloc(LPTR, sizeof(DWORD));

	LPSTR pageUrls[MAX_URLS];
	LPSTR filePath = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * 256);

	downloadPage(currentUrl, filePath);

	if (filePath == NULL)
	{
		return;
	}

	if (!processPage(filePath, size, currentUrl, pageUrls))
	{
		return;
	}

	if (WAIT_TIMEOUT == WaitForSingleObject(threadSync,INFINITE))
	{
		_error("Mutex timed out @crawler.\n");
	}

	threadComm[currentUrl] = *size;

	if (!ReleaseMutex(threadSync))
	{
		_error("Error releasing mutex.");
	}

	
	INT i = 0;

	LocalFree(size);

	while (pageUrls[i] != NULL)
	{
		if (visitedLinks.find(pageUrls[i]) == visitedLinks.end())
		{
			visitedLinks.insert(pageUrls[i]);
			crawl(*(pageUrls + i), depth + 1, maxDepth);
		}

		if (++i > MAX_URLS - 1)
		{
			break;
		}
	}


}

DWORD WINAPI startCrawlerThread(LPVOID lParam)
{
	printf("Crawler thread started.\n");

	INT i = 0;
	LPSTR* links = (LPSTR*)LocalAlloc(LPTR, sizeof(CHAR*) * 256);
	readAdressesFromRegistry("SOFTWARE\\WOW6432Node\\WebCrawler", links);

	while (links[i] != NULL)
	{
		crawl(links[i], 0, 1);
		i++;
	}

	

	LocalFree(links);

	printf("Crawler thread finished.\n");
	continueListening = 0;
	return 0;
}

DWORD WINAPI startSenderThread(LPVOID lParam)
{
	HANDLE consumerEvent;
	HANDLE producerEvent;

	bool dataWasRead;

	std::map<std::string, DWORD>::iterator itt;
	LPCSTR siteLink=NULL;
	DWORD siteSize;

	if (NULL == (consumerEvent = CreateEvent(NULL, FALSE, TRUE, TEXT("producerEvent"))))
	{
		_error("Error creating producer event.");
	}

	if (NULL == (producerEvent = OpenEvent(SYNCHRONIZE, FALSE, TEXT("consumerEvent"))))
	{
		_error("Error creating consumer event.");
	}

	
	printf("Transmitter thread started.\n");

	while (continueListening)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(threadSync, INFINITE))
		{
			_error("Mutex timed out @sender.\n");
		}

		if (dataWasRead=!threadComm.empty())
		{
			itt = threadComm.begin();

			siteLink = itt->first.c_str();

			siteSize = itt->second;

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

			sendData(mapView, siteLink,siteSize);

			if (!SetEvent(producerEvent))
			{
				_error("Error setting producer event.");
			}

		}

	}

	printf("Sender thread finished.\n");
	return 0;

}

void initCrawler()
{

	HANDLE crawlerThread;
	HANDLE senderThread;
	HANDLE mappedFile;
	
	createDataFile(&mappedFile, mapView);


	if (NULL == (senderThread = CreateThread(NULL, 0, startSenderThread, NULL, CREATE_SUSPENDED,NULL)))
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


	if (-1 == (ResumeThread(crawlerThread)))
	{
		_error("Error starting crawler thread.");
	
	}

	if (-1 == (ResumeThread(senderThread)))
	{
		_error("Error starting sender thread.");

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