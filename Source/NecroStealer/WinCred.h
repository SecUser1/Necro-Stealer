#pragma once
#include <windows.h>
#include <wincred.h>
#include <shlwapi.h>
#include "WinCred.h"
#include "Zip.h"

void doCredentials(HZIP hZip, DWORD* dwCountCred);