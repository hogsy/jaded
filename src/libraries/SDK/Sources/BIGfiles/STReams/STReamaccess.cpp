#include "precomp.h"

#ifdef ACTIVE_EDITORS
#include "STReamaccess.h"

int GetStreamByName(const char* szFileName, const char* szStreamName,char* szStreamValue)
{
	HANDLE hFile, hMap;

	char* szStreamFileName = new char [strlen(szFileName) + strlen(szStreamName) + 2];
	wsprintf(szStreamFileName, ("%s:%s"), szFileName, szStreamName);

	// -------------------------
	// Set StreamValue to invalid 
	szStreamValue[0] = '\0';

	hFile = CreateFile(szStreamFileName, GENERIC_READ, FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_SEQUENTIAL_SCAN, (HANDLE)NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		hMap = CreateFileMapping(hFile,0,PAGE_READONLY,0,0,0);
		void* pFileName = (char*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

		if(pFileName != NULL)
		{
			static char szPath[BIG_C_MaxLenPath];
			sscanf( (char*)pFileName, "%255s", szPath);
			strcpy(szStreamValue, szPath) ;
		}
		else
		{
			CloseHandle(hFile);
			CloseHandle(hMap);
			delete [] szStreamFileName;
			return 0;
		}

		UnmapViewOfFile(pFileName);
		CloseHandle(hMap);
		CloseHandle(hFile);
		delete [] szStreamFileName;
		return 1; 
	}

	delete [] szStreamFileName;
	return 0;
}

int SetStreamByName(const char* szFileName, const char* szStreamName,const char* szStreamValue)
{
	char* szStreamFileName = new char [strlen(szFileName) + strlen(szStreamName) + 2];
	wsprintf(szStreamFileName, ("%s:%s"), szFileName, szStreamName);

	// If main stream doesn't exist don't create stream ADS_USER
	if ( L_access( szFileName, 0 ) != -1 )
	{	
		FILE* hFile = fopen(szStreamFileName, "wtc");
		if(hFile)
		{
			fprintf(hFile, "%s", szStreamValue);
			fclose(hFile);
			delete [] szStreamFileName;
			return 1;
		}
	}
	delete [] szStreamFileName;
	return 0;
}

#endif // ACTIVE_EDITORS
