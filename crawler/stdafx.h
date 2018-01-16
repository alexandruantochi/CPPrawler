// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include <Shlwapi.h>
#include <vector>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <WinInet.h>
#include <Urlmon.h>
#include <atlbase.h>
#include <conio.h>
#include <set>
#include <map>




//crawlerFunctions

void initCrawler();


//utils
void _error(const char message[]);
void pause(const char message[]);
bool linkHasValidExt(LPCSTR link);
bool isLinkWebsite(LPCSTR link);
bool isLinkToRoot(LPCSTR link);
bool joinRootLink(LPCSTR url, LPSTR link);
bool joinLink(LPCSTR url, LPSTR link);
bool linkHasExtension(LPCSTR link);
void createDataFile(PHANDLE fileMapping, LPVOID mapView);
void sendData(LPVOID mappedView, LPCSTR siteLink, DWORD siteSize);

LPSTR parseUrl(LPCSTR pageUrl, unsigned int type);

#define PARSEURL_PROTOCOL 0
#define PARSEURL_BASEURL 1
#define PARSEURL_REQUEST 2
#define PARSEURL_FILENAME 3
#define PARSEURL_SERVER 4
#define PARSEURL_BACK 5


// TODO: reference additional headers your program requires here
