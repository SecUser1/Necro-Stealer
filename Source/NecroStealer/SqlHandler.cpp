#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <shlwapi.h>
#include <stdint.h>
#include "Array.h"
#include "SqlHandler.h"

struct RecordHeaderField
{
	long Size;
	long Type;
};

struct SqliteMasterEntry
{
	CHAR* ItemName;
	long RootNum;
	CHAR* SqlStatement;
};

struct TableEntry
{
	Array<BYTE*> Content;
	Array<INT> SizeList;
};

unsigned char _sqlDataTypeSize[10] = { 0, 1, 2, 3, 4, 6, 8, 8, 0, 0 };
unsigned long _dbEncoding;
unsigned char* _fileBytes;
unsigned long _pageSize;
Array<LPCSTR> _fieldNames;

LONGLONG _fileBytesLength;

Array<SqliteMasterEntry> _masterTableEntries;
Array<TableEntry> _tableEntries;

char* strdup_impl(const char* str) {
	char* mem = (char*)malloc(lstrlenA(str) + 1);
	lstrcpyA(mem, str);
	return mem;
}

LPCSTR longToStr(long l) {
	char* mem = (char*)malloc(50);
	wnsprintfA(mem, 50, "%lu", l);
	return mem;
}

LPCSTR substrOffset(LPCSTR str, int off) {
	char* mem = (char*)malloc((lstrlenA(str) - off) + 1);
	lstrcpyA(mem, str + off);
	return mem;
}

int IndexOfChars(LPCSTR wh, LPCSTR what) {
	for (int i = 0; i < lstrlenA(wh) - lstrlenA(what); i++) {
		int cmp = 0;
		for (int j = 0; j < lstrlenA(what); j++) {
			if (wh[i + j] == what[j]) cmp++;
		}
		if (cmp == lstrlenA(what)) return i; else cmp = 0;
	}
	return 0;
}

int IndexOfChar(LPCSTR string, char c) {
	for (int i = 0; i < lstrlenA(string); i++) { if (string[i] == c) return i; } return 0;
	return 0;
}

unsigned char* FileReadAllBytes(LPCWSTR stringPath) {
	HANDLE hFile = CreateFileW(stringPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD dwRead;
		LARGE_INTEGER fSize;
		GetFileSizeEx(hFile, &fSize);

		unsigned char* fBytes = (unsigned char*)malloc((size_t)fSize.QuadPart);
		ReadFile(hFile, fBytes, (DWORD)fSize.QuadPart, &dwRead, 0);
		CloseHandle(hFile);

		_fileBytesLength = fSize.QuadPart;
		return fBytes;
	}
	else {
		char s[23];
		wnsprintfA(s, 23, "%ld", GetLastError());
		MessageBoxA(0, s, 0, 0);
	}

	return 0;
}

unsigned long ConvertToULong(int startIndex, int size)
{
	if (size > 8 || size == 0)
		return 0;

	unsigned num = 0;
	for (int index = 0; index <= size - 1; ++index)
		num = num << 8 | (unsigned)_fileBytes[startIndex + index];
	return num;
}

BYTE* GetValue(int rowNum, int field, DWORD* size)
{
	if (rowNum >= _tableEntries.Count())
		return 0;

	Array<BYTE*> array_data = _tableEntries[rowNum].Content;
	Array<INT> array_size = _tableEntries[rowNum].SizeList;
	if (size) *size = array_size[field];

	return field >= array_data.Count() ? 0 : array_data[field];
}

int GetRowCount()
{
	return _tableEntries.Count();
}

int Gvl(int startIdx)
{
	if (startIdx > _fileBytesLength)
		return 0;
	for (int index = startIdx; index <= startIdx + 8; ++index)
	{
		if (index > _fileBytesLength - 1)
			return 0;
		if (((int)_fileBytes[index] & 128) != 128)
			return index;
	}
	return startIdx + 8;
}

long Cvl(int startIdx, int endIdx)
{
	++endIdx;
	unsigned char numArray[8] = { 0 };
	int num1 = endIdx - startIdx;
	bool flag = false;
	if (num1 == 0 | num1 > 9)
		return 0;
	if (num1 == 1)
	{
		numArray[0] = (byte)((unsigned int)_fileBytes[startIdx] & (unsigned int)127);
		return (long)(uint64_t)(*(uint64_t*)&numArray[0]);
	}
	if (num1 == 9)
		flag = true;
	int num2 = 1;
	int num3 = 7;
	int index1 = 0;
	if (flag)
	{
		numArray[0] = _fileBytes[endIdx - 1];
		--endIdx;
		index1 = 1;
	}
	int index2 = endIdx - 1;
	while (index2 >= startIdx)
	{
		if (index2 - 1 >= startIdx)
		{
			numArray[index1] = (byte)((int)_fileBytes[index2] >> num2 - 1 & (int)255 >> num2 | (int)_fileBytes[index2 - 1] << num3);
			++num2;
			++index1;
			--num3;
		}
		else if (!flag)
			numArray[index1] = (byte)((int)_fileBytes[index2] >> num2 - 1 & (int)255 >> num2);
		index2 += -1;
	}
	return (long)(uint64_t)(*(uint64_t*)&numArray[0]);
}

bool IsOdd(long value)
{
	return (value & 1L) == 1L;
}

BYTE* defaultGetString(unsigned char* str, int index, int count) {
	BYTE* mem = (BYTE*)malloc(count + 1);
	memset(mem, 0, count + 1);

	for (int i = 0; i < count; i++) {
		mem[i] = str[index + i];
	}
	return mem;
}

BYTE* unicodeGetString(unsigned char* str, int index, int count) {
	BYTE* mem = (BYTE*)malloc(count + 1);
	memset(mem, 0, count + 1);

	for (int i = 0; i < count; i++) {
		mem[i] = str[(2 * index) + (i * 2)];
	}
	return mem;
}

void ReadMasterTable(long offset)
{
	unsigned int num1;
	switch (_fileBytes[offset])
	{
	case 5:
		//MessageBoxA(0, "case 5", "", 0x01);
		num1 = (unsigned int)(ConvertToULong((int)offset + 3, 2) - 1UL);
		for (int index = 0; index <= (int)num1; ++index)
		{
			unsigned int num2 = (unsigned int)ConvertToULong((int)offset + 12 + index * 2, 2);
			if (offset == 100L) {
				//MessageBoxA(0, "if1", 0, 0);
				ReadMasterTable(((long)ConvertToULong((int)num2, 4) - 1L) * (long)_pageSize);
			}
			else {
				//MessageBoxA(0, "if2", 0, 0);
				ReadMasterTable(((long)ConvertToULong((int)(offset + (long)num2), 4) - 1L) * (long)_pageSize);
			}
		}
		//MessageBoxA(0, "rm3", 0, 0);
		ReadMasterTable(((long)ConvertToULong((int)offset + 8, 4) - 1L) * (long)_pageSize);
		break;
	case 13:
		//MessageBoxA(0, "case 13", 0, 0);
		unsigned long num3 = ConvertToULong((int)offset + 3, 2) - 1UL;
		int num4 = 0;
		if (_masterTableEntries.Count() != 0)
		{
			num4 = _masterTableEntries.Count();
		}

		for (unsigned long index1 = 0; index1 <= num3; ++index1)
		{
			unsigned long num2 = ConvertToULong((int)offset + 8 + (int)index1 * 2, 2);
			if (offset != 100L)
				num2 += (unsigned long)offset;
			int endIdx1 = Gvl((int)num2);
			Cvl((int)num2, endIdx1);
			int endIdx2 = Gvl((int)((long)num2 + ((long)endIdx1 - (long)num2) + 1L));
			Cvl((int)((long)num2 + ((long)endIdx1 - (long)num2) + 1L), endIdx2);
			unsigned long num5 = num2 + (unsigned long)((long)endIdx2 - (long)num2 + 1L);
			int endIdx3 = Gvl((int)num5);
			int endIdx4 = endIdx3;
			long num6 = Cvl((int)num5, endIdx3);
			long numArray[5];
			for (int index2 = 0; index2 <= 4; ++index2)
			{
				int startIdx = endIdx4 + 1;
				endIdx4 = Gvl(startIdx);
				numArray[index2] = Cvl(startIdx, endIdx4);
				numArray[index2] = numArray[index2] <= 9L ? (long)_sqlDataTypeSize[numArray[index2]] : (!IsOdd(numArray[index2]) ? (numArray[index2] - 12L) / 2L : (numArray[index2] - 13L) / 2L);
			}

			SqliteMasterEntry sme;

			if ((long)_dbEncoding == 1L)
				sme.ItemName = (CHAR*)defaultGetString(_fileBytes, (int)((long)num5 + num6 + numArray[0]), (int)numArray[1]);
			else if ((long)_dbEncoding == 2L)
				sme.ItemName = (CHAR*)unicodeGetString(_fileBytes, (int)((long)num5 + num6 + numArray[0]), (int)numArray[1]);

			sme.RootNum = (long)ConvertToULong((int)((long)num5 + num6 + numArray[0] + numArray[1] + numArray[2]), (int)numArray[3]);

			if ((long)_dbEncoding == 1L)
				sme.SqlStatement = (CHAR*)defaultGetString(_fileBytes, (int)((long)num5 + num6 + numArray[0] + numArray[1] + numArray[2] + numArray[3]), (int)numArray[4]);
			else if ((long)_dbEncoding == 2L)
				sme.SqlStatement = (CHAR*)unicodeGetString(_fileBytes, (int)((long)num5 + num6 + numArray[0] + numArray[1] + numArray[2] + numArray[3]), (int)numArray[4]);
				

			_masterTableEntries.Add(sme);
		}
		break;
	}
}

bool ReadTableFromOffset(unsigned long offset)
{
	if (_fileBytes[offset] == 13)
	{
		unsigned int num1 = (unsigned int)(ConvertToULong((int)offset + 3, 2) - 1UL);
		int num2 = 0;
		if (_tableEntries.Count() != 0)
		{
			num2 = _tableEntries.Count();
		}

		for (unsigned int index1 = 0; (int)index1 <= (int)num1; ++index1)
		{
			unsigned long num3 = ConvertToULong((int)offset + 8 + (int)index1 * 2, 2);
			if ((long)offset != 100L)
				num3 += offset;
			int endIdx1 = Gvl((int)num3);
			Cvl((int)num3, endIdx1);
			int endIdx2 = Gvl((int)((long)num3 + (endIdx1 - (long)num3) + 1L));
			Cvl((int)((long)num3 + (endIdx1 - (long)num3) + 1L), endIdx2);
			unsigned long num4 = num3 + (unsigned long)(endIdx2 - (long)num3 + 1L);
			int endIdx3 = Gvl((int)num4);
			int endIdx4 = endIdx3;
			long num5 = Cvl((int)num4, endIdx3);
			Array<RecordHeaderField> arr;
			long num6 = (long)num4 - endIdx3 + 1L;
			int index2 = 0;
			while (num6 < num5)
			{
				int startIdx = endIdx4 + 1;
				endIdx4 = Gvl(startIdx);

				RecordHeaderField rc;

				rc.Type = Cvl(startIdx, endIdx4);
				rc.Size = rc.Type <= 9L ? _sqlDataTypeSize[rc.Type] : (!IsOdd(rc.Type) ? (rc.Type - 12L) / 2L : (rc.Type - 13L) / 2L);

				arr.Add(rc);

				num6 = num6 + (endIdx4 - startIdx) + 1L;
				++index2;
			}
			if (arr.Count() != 0)
			{
				TableEntry tb;

				int num7 = 0;
				for (int index3 = 0; index3 < arr.Count(); ++index3)
				{
					if (arr[index3].Type > 9L)
					{
						if (!IsOdd(arr[index3].Type))
						{
							if ((long)_dbEncoding == 1L)
								tb.Content.Add((BYTE*)defaultGetString(_fileBytes, (int)((long)num4 + num5 + num7), (int)arr[index3].Size));
							else if ((long)_dbEncoding == 2L)
							{
								tb.Content.Add((BYTE*)unicodeGetString(_fileBytes, (int)((long)num4 + num5 + num7), (int)arr[index3].Size));
							}
						}
						else
							tb.Content.Add((BYTE*)defaultGetString(_fileBytes, (int)((long)num4 + num5 + (long)num7), (int)arr[index3].Size));
					}
					else
						tb.Content.Add((BYTE*)longToStr(ConvertToULong((int)((long)num4 + num5 + (long)num7), (int)arr[index3].Size)));

					tb.SizeList.Add((int)arr[index3].Size);
					num7 += (int)arr[index3].Size;
				}

				_tableEntries.Add(tb);
			}
		}
	}
	else if ((int)_fileBytes[offset] == 5)
	{
		unsigned int num1 = (unsigned int)(ConvertToULong((int)((long)offset + 3L), 2) - 1UL);
		for (unsigned int index = 0; (int)index <= (int)num1; ++index)
		{
			unsigned int num2 = (unsigned int)ConvertToULong((int)offset + 12 + (int)index * 2, 2);
			ReadTableFromOffset((ConvertToULong((int)((long)offset + (long)num2), 4) - 1UL) * _pageSize);
		}
		ReadTableFromOffset((ConvertToULong((int)((long)offset + 8L), 4) - 1UL) * _pageSize);
	}
	return true;
}

bool ReadTable(const char* tableName)
{
	int index1 = -1;
	for (int index2 = 0; index2 <= _masterTableEntries.Count(); ++index2)
	{
		if (lstrcmpA(CharLowerA((LPSTR)(_masterTableEntries[index2]).ItemName), tableName) == 0)
		{
			index1 = index2;
			break;
		}
	}
	if (index1 == -1)
		return false;

	char* mainStr = (char*)substrOffset(_masterTableEntries[index1].SqlStatement, IndexOfChar(_masterTableEntries[index1].SqlStatement, '(') + 1);
	char* pTempStr = strdup_impl(mainStr);
	char* pWord = strtok(pTempStr, ",");

	while (pWord != NULL)
	{
		int length = IndexOfChar(pWord, ' ');

		if (length > 0)
			pWord[0] = length;

		if (IndexOfChars(pWord, "UNIQUE") != 0)
		{
			char* mem = (char*)malloc(lstrlenA(pWord) + 1);
			lstrcpyA(mem, pWord);

			_fieldNames.Add(mem);
		}

		pWord = strtok(NULL, ",");
	}

	free(pTempStr);
	free(mainStr);

	return ReadTableFromOffset((unsigned long)(_masterTableEntries[index1].RootNum - 1L) * _pageSize);
}

bool SqlHandler(LPCWSTR stringPath) {
	//MessageBoxA(0, "FBYTES", 0, 0);
	_fileBytes = FileReadAllBytes(stringPath);
	if (_fileBytesLength < 1) if(0 != memcmp(_fileBytes, "SQLite format 3", 15)) return false;
	//MessageBoxA(0, "PAGE", 0, 0);
	_pageSize = ConvertToULong(16, 2);
	//MessageBoxA(0, "ENC", 0, 0);
	_dbEncoding = ConvertToULong(56, 4);
	//MessageBoxA(0, "RMTRMTRMT", 0, 0);

	ReadMasterTable(100L);
	//MessageBoxA(0, "READMASTER end", 0, 0);
	return true;
}

void SqlHandleFree() {
	for (int i = 0; i < _fieldNames.Count(); i++) free((void*)_fieldNames[i]);
	for (int i = 0; i < _tableEntries.Count(); i++) for (int j = 0; j < _tableEntries[i].Content.Count(); j++) free((void*)_tableEntries[i].Content[j]);
	for (int i = 0; i < _masterTableEntries.Count(); i++) { free((void*)_masterTableEntries[i].ItemName); free((void*)_masterTableEntries[i].SqlStatement); }

	_fieldNames.Clear();
	_tableEntries.Clear();
	_masterTableEntries.Clear();

	free(_fileBytes);
	_fileBytes = 0;
}