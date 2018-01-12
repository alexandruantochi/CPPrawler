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
#include "pugixml.hpp"


#define PARSEURL_PROTOCOL 0
#define PARSEURL_BASEURL 1
#define PARSEURL_REQUEST 2
#define PARSEURL_FILENAME 3

LPSTR* readAdressesFromRegistry(LPCSTR registryName);
void startCrawler();


// TODO: reference additional headers your program requires here
