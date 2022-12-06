#pragma once
#include <windows.h>

int GetRowCount();
void SqlHandleFree();
bool SqlHandler(LPCWSTR stringPath);
bool ReadTable(const char* tableName);
BYTE* GetValue(int rowNum, int field, DWORD* size);