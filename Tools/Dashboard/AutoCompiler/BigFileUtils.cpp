#include "stdafx.h"
#include "io.h"

#include <winbase.h>
#include <assert.h>

#include "BigFileUtils.h"
#include "ConsProc.h"

static char saz_ServerPort          [BIG_k_NameLength];
static char saz_ClientName          [BIG_k_NameLength];
static char saz_ClientPass          [BIG_k_NameLength];
static char saz_TempFile            [BIG_k_NameLength];
static char saz_DepotBranch         [BIG_k_NameLength];
static char saz_CurrentUser         [BIG_k_NameLength];
static char saz_PasswordFile        [BIG_k_NameLength];
static char saz_ExeName             [BIG_k_NameLength];
static char saz_Arguments           [BIG_k_NameLength];
static char saz_ExeAndLogPath       [BIG_k_NameLength];
static char saz_LogFileName         [BIG_k_NameLength];
static char saz_BigFileName         [BIG_k_NameLength];
static char saz_LastUser            [BIG_k_NameLength];

static bool sb_AiFailed         = false;
static bool sb_DbFailed         = false;
static bool sb_Suspended        = false;
static bool sb_YetiIsRunning    = false;
static int  si_LastClTested     = -1;



bool BIG_bCanLaunchYeti()
{
    return sb_YetiIsRunning == false;
}

int BIG_iGetLastClTested()
{
    return si_LastClTested;
}

 bool bLaunchSystemCommand(char * _pz_Command)
{
    P4::ConsProc oConsole;

    oConsole.Execute(0, _pz_Command, ".", 0);
    oConsole.Terminate();

    return true;
}

void LaunchSystemCommand(char * _pz_Command,bool _bWaitForIt)
{
    STARTUPINFO				stStartUpInfo;
    PROCESS_INFORMATION		stProcInfo;
    char szCommand[1024*4];

    //*** Launching process ***
    memset(&stStartUpInfo,0,sizeof(STARTUPINFO));
    stStartUpInfo.cb = sizeof(STARTUPINFO);
    sprintf(szCommand, "Console.exe %d \"cmd.exe /c %s\"", 1, _pz_Command); 
    if ( CreateProcess(NULL,
        szCommand,
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW|NORMAL_PRIORITY_CLASS,
        NULL,
        ".",
        &stStartUpInfo,
        &stProcInfo) )
    {
        if ( _bWaitForIt )
            while( WaitForSingleObject(stProcInfo.hProcess,100) );
        CloseHandle( stProcInfo.hProcess );
        CloseHandle( stProcInfo.hThread );
    }
    else
    {
        MessageBox(NULL,"Command error. Check console.exe","COMMAND ERROR",MB_OK|MB_ICONERROR);
    }
}

void CopyFolder(const char* _szSource, const char* _szTarget)
{
    char sz_Command[BIG_k_CommandLength];
    sprintf(sz_Command, "xcopy /Y /S \"%s\\*.*\" \"%s\\*.*" , _szSource, _szTarget);
    LaunchSystemCommand(sz_Command, true);
}

static void BIG_ComputePathFromExeName()
{
    strcpy(saz_ExeAndLogPath, saz_ExeName);
    char * c = strrchr(saz_ExeAndLogPath, '\\');
    if ( c )
        *c = 0;
}


void BIG_SetPerforceCurrentUser(char * _pz_CurrentUser)
{
    strcpy(saz_CurrentUser, _pz_CurrentUser);
}

void BIG_SetPerforceServerPort(char * _pz_ServerPort)
{
    strcpy(saz_ServerPort, _pz_ServerPort);
}

void BIG_SetPerforceDepotBranch(char * _pz_DepotBranch)
{
    strcpy(saz_DepotBranch, _pz_DepotBranch);
}

void BIG_SetPerforceTempFile(char * _pz_TempFile)
{
    strcpy(saz_TempFile, _pz_TempFile);
}

void BIG_SetBigFileName(char * _pz_BigFileName)
{
    strcpy(saz_BigFileName, _pz_BigFileName);
}

char * BIG_pz_GetPerforceServerPort()
{
    return saz_ServerPort;
}

char * BIG_pz_GetPerforceDepotBranch()
{
    return saz_DepotBranch;
}

char * BIG_pz_GetPerforceTempFile()
{
    return saz_TempFile;
}

char * BIG_pz_GetPerforcePasswordFile()
{
    return saz_PasswordFile;
}

char * BIG_pz_GetPerforceCurrentUser()
{
    return saz_CurrentUser;
}

char * BIG_pz_GetExeArguments()
{
    return saz_Arguments;
}

char * BIG_pz_GetExeName()
{
    return saz_ExeName;
}

char * BIG_pz_GetBigFileName()
{
    return saz_BigFileName;
}

char * BIG_pz_GetLastUser()
{
    return saz_LastUser;
}

bool BIG_bGetAiResultStatus()
{
    return sb_AiFailed;
}

bool BIG_bGetDatabaseCheckStatus()
{
    return sb_DbFailed;
}

bool BIG_bGetGlobalResultStatus()
{
    return (sb_AiFailed || sb_DbFailed);
}




//////////////////////////////////////////////
void ExtractPathFromFullName( char* _pzPath, const char* _pzFullName)
{
    strcpy(_pzPath, _pzFullName);
    char * c = strrchr(_pzPath, '\\');
    if ( c )  *c = 0;
}

const char* pzGetFileNameFromFullPath(const char* _pzFullName)
{
    char* pName = strrchr(_pzFullName, '\\' );
    if(pName) 
        return pName+1;
    else 
        return _pzFullName;
}

const char* pzSimpleMapName( const char* _pzMapName )   
{   // Map name (without path)
    char* pName = strrchr(_pzMapName, '/' );
    if(pName) 
        return pName+1;
    else 
        return _pzMapName;
}


// Desdac Version.
// Reads the standard version number stored in the resources (Version/VS_VERSION_INFO block)
bool bGetExeVersion (DWORD& _ulV1, DWORD& _ulV2, DWORD& _ulV3, DWORD& _ulV4)
{
    // get the desdac exe name
    char szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH);

    // get the file version info
    DWORD lNul;

    DWORD lSizeStruct = GetFileVersionInfoSize( szFileName, &lNul );
    if(lSizeStruct!=0)
    {
        char *pcBuf;
        pcBuf = (char *)malloc(lSizeStruct+1);
        GetFileVersionInfo(szFileName, NULL,lSizeStruct,pcBuf);
        UINT uwLenght;
        char *pcBuffer;
        VerQueryValue(pcBuf,"\\",(void **)&pcBuffer,&uwLenght);
        VS_FIXEDFILEINFO *pstFFI=(VS_FIXEDFILEINFO *)pcBuffer;

        _ulV1 = pstFFI->dwFileVersionMS/0xffff;
        _ulV2 = pstFFI->dwFileVersionMS & 0x0000ffff;
        _ulV3 = pstFFI->dwFileVersionLS/0xffff;
        _ulV4 = pstFFI->dwFileVersionLS & 0x0000ffff;

        free(pcBuf);
        return true;
    }
    else
    {
        return false;
    }
}

bool bGetExeVersion (char* _szVersion)
{
    DWORD ulV1, ulV2, ulV3, ulV4;
    if( bGetExeVersion(ulV1, ulV2, ulV3, ulV4) )
    {
        sprintf(_szVersion, "v%ld.%ld.%ld.%ld", ulV1, ulV2, ulV3, ulV4);
        return true;
    }
    else
    {
        strcpy(_szVersion, "");
        return false;
    }
}


DWORD ulGetFileSize(const char* _pzFile)
{
    // Find the file
    WIN32_FIND_DATA stFileData;
    HANDLE hFile = FindFirstFile( _pzFile, &stFileData );
    if ( NULL == hFile )
        return 0;

    // Get its size
    DWORD ul_FileSize =  stFileData.nFileSizeLow;
    assert( 0 == stFileData.nFileSizeHigh );    // file shouldn't be bigger than 4Go...
    FindClose(hFile);

    return ul_FileSize;
}


CString szFormatDuration(const CTimeSpan& _roT)
{
    if( _roT.GetDays() > 0 )
        return _roT.Format("%Dd%Hh%M'%S''");

    if( _roT.GetTotalHours() > 0 )
        return _roT.Format("%Hh%M'%S''");

    if( _roT.GetTotalMinutes() > 0 )
        return _roT.Format("%M'%S''");

    if( _roT.GetTotalSeconds() > 0 )
        return _roT.Format("   %S''");

    return "   --  ";
}



bool bGetFileCreationTime(const char* _szFile, CTime& _rFileTime)
{
    WIN32_FIND_DATA  stFileInfo;
    HANDLE hFile = FindFirstFile( _szFile, &stFileInfo);

    bool bFileFound = ( hFile != INVALID_HANDLE_VALUE );

    if( bFileFound )
    {   _rFileTime = CTime(stFileInfo.ftCreationTime);
    }

    FindClose(hFile);

    return bFileFound;
}

bool bGetFileWriteTime(const char* _szFile, CTime& _rFileTime)
{
    WIN32_FIND_DATA  stFileInfo;
    HANDLE hFile = FindFirstFile( _szFile, &stFileInfo);

    bool bFileFound = ( hFile != INVALID_HANDLE_VALUE );

    if( bFileFound )
    {   _rFileTime = CTime(stFileInfo.ftLastWriteTime);
    }

    FindClose(hFile);

    return bFileFound;
}

bool bAppendTxtFileToMessage(const char* _sz_File, char* sz_Message)
{
    FILE * pFile = fopen(_sz_File, "rt");
    int iFileSize = 0;
    if ( pFile )
    {
        int iFile = _fileno(pFile);
        iFileSize = _filelength(iFile);
        char* pzFileBuffer = (char*)malloc(iFileSize+1);
        fread(pzFileBuffer, 1, iFileSize, pFile);
        fclose(pFile);
        flushall();
        pzFileBuffer[iFileSize] = 0;

        strcat(sz_Message, pzFileBuffer);
        free(pzFileBuffer);

        return true;
    }

    return false;
}
