#include "stdafx.h"
#include <stdexcept>

#define FORMAT "%s::%ul"


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

	LPSTR protocol = StrStr(link, "http");

	return protocol == link;
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


LPSTR parseUrl(LPCSTR pageUrl, unsigned int type)
{
	INT offset = 0;
	INT i = 0;
	LPSTR result = (LPSTR)LocalAlloc(LPTR, sizeof(CHAR) * 256);
	StrCpy(result, pageUrl);


	switch (type)
	{
	case 0:
		while (*(result + i) != ':')
		{
			i++;
			if (i > 5)
			{
				return NULL;
			}
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
		while (*(result + i) != '\0')
		{
			if (*(result + i) == '/' || *(result + i) == '.' || *(result + i) == ':' || *(result + i) == '?' || *(result + i) == '*')
			{
				*(result + i) = '_';
			}
			i++;
		}
		break;
	case 4:
		result = parseUrl(pageUrl, PARSEURL_PROTOCOL);
		StrCat(result, "://");
		StrCat(result, parseUrl(pageUrl, PARSEURL_BASEURL));
		StrCat(result, "/");
		break;
	case 5:

		while (*(result + i) != '\0')
		{
			if (*(result + i) == '/')
			{
				offset = i;
			}
			i++;
		}
		*(result + offset + 1) = '\0';
		break;
	default:
		_error("Invalid argument for parseUrl function.");
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

void sendData(LPVOID mappedView, LPCSTR siteLink, DWORD siteSize)
{
	if (!sprintf((char*)mappedView, FORMAT, siteLink, siteSize))
	{
		printf("Error printing to mapped file.\n");
	}
}