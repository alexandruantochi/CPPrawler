#include "stdafx.h"
#include <stdexcept>




void _error(const char message[])
{
	char error[255];

	sprintf(error, "[%d] ERROR: %s \n", GetLastError(), message);
	MessageBox(NULL, error, NULL, NULL);
	throw std::runtime_error(error);
}

void pause(const char message[])
{
	printf("%s\n", message);
	while (!_kbhit()) {}
	_getch();
}

bool linkHasValidExt(LPCSTR link)
{
	if (!linkHasExtension(link))
	{
		return true;
	}

	LPSTR extension = StrStr(link, ".");

	if (!extension)
	{
		return true;
	}

	if (!StrCmp(extension, ".html") || !StrCmp(extension, ".xhtml"))
	{
		return true;
	}


	return false;

}

bool linkHasExtension(LPCSTR link)
{
	UINT i = 0;
	UINT marker = 0;
	UINT slash = 0;

	while (*(link + i) != '\0')
	{
		if (*(link + i) == '.')
		{
			marker = i;
		}
		i++;
	}


	return (strlen(link + marker) < 6);
}

bool isLinkWebsite(LPCSTR link)
{
	LPSTR protocol = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * 8);
	memcpy(protocol, link, 7);
	*(protocol + 7) = '\0';

	if (!StrCmp(protocol, "http://") || !StrCmp(protocol, "https:/"))
	{
		LocalFree(protocol);
		return true;
	}

	LocalFree(protocol);
	return false;

}

bool isLinkToRoot(LPCSTR link)
{
	return *(link) == '/';
}

bool joinRootLink(LPCSTR url, LPSTR link)
{
	LPSTR result = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * 256);

	StrCpy(result, url);

	LPSTR finder;
	finder = StrChr(result, '.');
	finder = StrChr(finder, '/');

	if (!finder)
	{
		return false;
	}

	*(finder) = '\0';

	StrCat(result, link);
	StrCpy(link, result);

	LocalFree(result);
	return true;
}

bool joinLink(LPCSTR url, LPSTR link)
{
	LPSTR finder;
	LPSTR result = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * 256);
	StrCpy(result, url);

	finder = strrchr(result, '/');
	*(finder + 1) = '\0';
	StrCat(result, link);
	StrCpy(link, result);

	LocalFree(result);
	return true;
}


LPSTR parseUrlToFile(LPCSTR pageUrl)
{
	INT offset = 0;
	INT i = 0;
	LPSTR result = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * 256);
	StrCpy(result, pageUrl);

		while (*(result + i) != '\0')
		{
			if (*(result + i) == '/' || *(result + i) == '.' || *(result + i) == ':' || *(result + i) == '?' || *(result + i) == '*')
			{
				*(result + i) = '_';
			}
			i++;
		}
	
	return result;
}


void createDataFile(PHANDLE fileMapping, LPVOID mapView)
{

	if (NULL == (*fileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, 1024, TEXT("crawler"))))
	{
		_error("Error creating producer file mapping");
	}

	if (NULL == (mapView = MapViewOfFile(fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0)))
	{
		_error("Error mapping view of file.");
	}

}

void sendData(LPVOID mFileHandle, LPSTR siteLink, DWORD siteSize)
{
	if (!sprintf((char*)mFileHandle, FORMAT, siteLink, siteSize))
	{
		printf("Error printing to mapped file.\n");
	}

}