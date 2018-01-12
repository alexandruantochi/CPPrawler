#include "stdafx.h"
#include <stdexcept>
#include <conio.h>


void _error(const char message[], const char optional[] = "\0")
{
	char error[255];

	sprintf(error, "[%d] ERROR: %s %s \n", GetLastError(), message, optional);
	MessageBox(NULL, error, NULL, NULL);
	throw std::runtime_error(error);
}

void pause(const char message[])
{
	printf("%s\n", message);
	while (!_kbhit()) {}
	_getch();
}

LPSTR parseUrl(LPCSTR pageUrl, unsigned int type)
{
	USES_CONVERSION;

	if (S_FALSE == IsValidURL(NULL, A2W(pageUrl), 0))
	{
		_error("Invalid url passed to paseUrl function.");
	}

	LPSTR result = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * 256);
	StrCpy(result, pageUrl);
	int i = 0;

	switch (type)
	{
	case 0:
		while (*(result + i) != ':')
		{
			i++;
		}
		*(result + i) = '\0';
		break;
	case 1:
		while (*(result + i) != '/')
		{
			i++;
		}
		result = &(*(result + (i + 2)));
		i = 0;
		while (*(result + i) != '/')
		{
			i++;
		}
		*(result + i) = '\0';
		break;
	case 2:
		while (*(result + i) != '.')
		{
			i++;
		}
		while (*(result + i) != '/')
		{
			i++;
		}
		result = &(*(result + i));
		break;
	case 3:
		StrCpy(result, parseUrl(pageUrl, PARSEURL_BASEURL));
		StrCpy((result + lstrlen(result)), parseUrl(pageUrl, PARSEURL_REQUEST));
		while (*(result + i) != '\0')
		{
			if (*(result + i) == '/' || *(result + i) == '.')
			{
				*(result + i) = '_';
			}
			i++;
		}
		break;
	default:
		_error("Invalid argument for parseUrl function.");
	}

	return result;
}

void downloadPage(LPCSTR pageUrl)
{

	int errCode;
	DWORD trheadId = GetCurrentThreadId();
	LPSTR filePath = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR)*MAX_PATH);
	StrCpy(filePath, "F:\\Scoala\\Anul III\\Sem I\\CSSO\\Lab\\Proiect\\downloads\\");
	StrCpy(filePath + lstrlen(filePath), parseUrl(pageUrl, PARSEURL_FILENAME));
	snprintf(filePath + lstrlen(filePath), 255 - lstrlen(filePath), "_%u", GetCurrentThreadId());

	errCode = URLDownloadToFile(NULL, pageUrl, filePath, 0, NULL);
	
}

LPSTR getPageURLs(HINTERNET connectionHandle, LPCSTR getUrl, INT port)
{

	return NULL;

}

LPSTR* readAdressesFromRegistry(LPCSTR registryName)
{
	HKEY hkResult;
	DWORD valueCount;
	DWORD maxKeySize;
	DWORD maxKeyValueSize;

	LPSTR siteLinks[256];
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

		siteLinks[i] = (LPSTR)lpData;


		if (i == valueCount)
		{
			LocalFree(lpData);
		}

	}

	CloseHandle(hkResult);
	LocalFree(lpvalueName);
	LocalFree(lpType);


	return siteLinks;

}

void startCrawler()
{
	//HINTERNET inetHandle = initInternet();
	//downloadPage("https://profs.info.uaic.ro/~rbenchea/csso.html");
	//parseUrl("https://profs.info.uaic.ro/some/get/request", PARSEURL_FILENAME);



}