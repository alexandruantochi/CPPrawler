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
#include <string>

// senderThread
DWORD WINAPI startProducerThread(LPVOID lParam);

// senderThreadFunctions
void getMappedFileHandle(PHANDLE hMap, LPVOID* mFileHandle);
void getSyncEvents(PHANDLE producerEvent, PHANDLE consumerEvent);

// crawlerThread
DWORD WINAPI startCrawlerThread(LPVOID lParam);

//crawlerThreadFunctions
bool readAdressesFromRegistry(LPCSTR registryName, LPSTR* links);
bool downloadPage(LPCSTR pageUrl, LPSTR filePath);
bool processPage(LPCSTR filePath, LPDWORD fileSize, LPCSTR pageUrl, LPSTR* pageUrls);

//crawlerFunctions
extern std::set<std::string> visitedLinks;
extern std::map<std::string, DWORD> threadComm;
extern INT continueListening;
extern HANDLE threadSync;
void init();

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
void sendData(LPVOID mFileHandle, LPSTR siteLink, DWORD siteSize);
LPSTR parseUrlToFile(LPCSTR pageUrl);


#define FORMAT "%s %u"
#define MAX_URLS 256

// TODO: reference additional headers your program requires here
