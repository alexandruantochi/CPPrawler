#include "stdafx.h"

void crawl(LPCSTR currentUrl, UINT depth, UINT maxDepth);


DWORD WINAPI startCrawlerThread(LPVOID lParam)
{
	
		printf("Crawler thread started.\n");

		INT i = 0;
		LPSTR* links = (LPSTR*)LocalAlloc(LPTR, sizeof(CHAR*) * 256);
		readAdressesFromRegistry("SOFTWARE\\WOW6432Node\\WebCrawler", links);

		while (links[i] != NULL)
		{
			crawl(links[i], 0, 4);
			i++;
		}

		LocalFree(links);

		printf("Crawler thread finished.\n");
		continueListening = 1;
		return 0;
	
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
	LPSTR filePath = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * MAX_URLS);

	if (!downloadPage(currentUrl, filePath))
	{
		LocalFree(size);
		LocalFree(filePath);
		return;
	}

	if (!processPage(filePath, size, currentUrl, pageUrls))
	{
		LocalFree(size);
		LocalFree(filePath);
		return;
	}

	if (WAIT_TIMEOUT == WaitForSingleObject(threadSync, INFINITE))
	{
		_error("Mutex timed out @crawler.\n");
	}

	threadComm[currentUrl] = *size;


	if (!ReleaseMutex(threadSync))
	{
		_error("Error releasing mutex.");
	}

	LocalFree(size);
	INT i = 0;

	while (pageUrls[i] != NULL && isLinkWebsite(pageUrls[i]))
	{
		if (visitedLinks.find(pageUrls[i]) == visitedLinks.end())
		{
			visitedLinks.insert(pageUrls[i]);
			crawl(*(pageUrls + i), depth + 1, maxDepth);
		}

		i++;

		if (i > (MAX_URLS - 2))
		{
			break;
		}
	}


}