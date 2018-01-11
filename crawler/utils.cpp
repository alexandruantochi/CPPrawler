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

LPSTR* readAdressesFromRegistry(LPCSTR registryName)
{
	HKEY hkResult;
	DWORD valueCount;
	DWORD maxKeySize;
	DWORD maxKeyValueSize;

	LPSTR* siteLinks;
	int errCode;

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

	if (NULL == (siteLinks = (LPSTR*)LocalAlloc(LPTR, sizeof(CHAR*)*valueCount)))
	{
		_error("Error allocating memory for the site links read from the registry.");
	}

	LPTSTR lpvalueName = (LPTSTR)LocalAlloc(LPTR, 256);
	DWORD cchValueName = 256;
	LPDWORD lpType = (LPDWORD)LocalAlloc(LPTR, sizeof(DWORD));
	LPBYTE lpData = (LPBYTE)LocalAlloc(LPTR, sizeof(maxKeyValueSize + 1));
	DWORD cbData = maxKeyValueSize + 1;
	unsigned char nullChar = '\0';

	for (DWORD i = 0; i < valueCount; i++)
	{
		if (ERROR_SUCCESS != (RegEnumValue(hkResult, i, lpvalueName, &cchValueName, NULL, lpType, lpData, &cbData)))
		{
			_error("Error reading key value index:", reinterpret_cast<char const *>(i));
		}

		lpData[maxKeyValueSize] = nullChar;



	}

	CloseHandle(hkResult);

	return siteLinks;
}