
#include "stdafx.h"
#include "windows.h"
#include "malloc.h"
#include "stdio.h"


// VREAD_VersionReader
#ifndef VREAD_VersionReader_h
#include "VersionReader.h"
#endif


BOOL VREAD_VersionReader::bGetFileVersion (char* _szFileName, DWORD& _ulV1, DWORD& _ulV2, DWORD& _ulV3, DWORD& _ulV4)
{
	// get the file version info
    DWORD lNul;

    DWORD lSizeStruct = GetFileVersionInfoSize( _szFileName, &lNul );
    if(lSizeStruct!=0)
    {
        char *pcBuf;
        pcBuf = (char *)malloc(lSizeStruct+1);
        GetFileVersionInfo(_szFileName,NULL,lSizeStruct,pcBuf);
        UINT uwLenght;
        char *pcBuffer;
        VerQueryValue(pcBuf,"\\",(void **)&pcBuffer,&uwLenght);
        VS_FIXEDFILEINFO *pstFFI=(VS_FIXEDFILEINFO *)pcBuffer;
        
		_ulV1 = pstFFI->dwFileVersionMS/0xffff;
		_ulV2 = pstFFI->dwFileVersionMS & 0x0000ffff;
		_ulV3 = pstFFI->dwFileVersionLS/0xffff;
		_ulV4 = pstFFI->dwFileVersionLS & 0x0000ffff;

        free(pcBuf);
		return TRUE;
    }
	else
	{
		return FALSE;
	}
}

BOOL VREAD_VersionReader::bGetFileVersion (char* _szFileName, char* _szVersion)
{
	DWORD ulV1, ulV2, ulV3, ulV4;

	if( bGetFileVersion(_szFileName, ulV1, ulV2, ulV3, ulV4) )
	{
		sprintf(_szVersion, "%ld.%ld.%ld.%ld", ulV1, ulV2, ulV3, ulV4);
		return TRUE;
	}
	else
	{
		strcpy(_szVersion, "None");
		return FALSE;
	}
}

