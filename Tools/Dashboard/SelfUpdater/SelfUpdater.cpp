// SelfUpdater.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "psapi.h"
#include <process.h>
#include <direct.h>
#include <stdlib.h>


////////
const int ki_CommandLength = 1024;
const int ki_Timer         = 500;   // millisec


////////
void    CopyFolder(const char* _szSource, const char* _szTarget);
void    LaunchSystemCommand(char * _pz_Command, bool _bWaitForIt = true);
void    ExtractPathFromFullName(char* _pzPath, const char* _pzFullName);
const char* pzGetFileNameFromFullPath(const char* _pzFullName);
HANDLE  pFindProcess( const char* _processName);


/////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // argv[0] is always the exe name called

    if( argc != 3) 
    {   
        return 1; // bad param number
    }
    else
    {
        // get arguments
        LPCTSTR pzExeToRelaunch     = argv[1];
        LPCTSTR pzPathOfNewVersion  = argv[2];

        char szExePath[MAX_PATH];
        ExtractPathFromFullName(szExePath, pzExeToRelaunch);
        const char* pszExeName = pzGetFileNameFromFullPath(pzExeToRelaunch);

        // give time for caller to quit
        HANDLE hProcess = pFindProcess(pszExeName);
        while( hProcess )
        {
            CloseHandle( hProcess );
            Sleep(ki_Timer);
            hProcess = pFindProcess(pszExeName);
        }


        // copy new version
        CopyFolder( pzPathOfNewVersion, szExePath );

        // relaunch
        _spawnl(_P_DETACH, pzExeToRelaunch, pzExeToRelaunch, NULL);


        return 0;
    }
}



void CopyFolder(const char* _szSource, const char* _szTarget)
{
    char sz_Command[ki_CommandLength];
    sprintf(sz_Command, "xcopy /Y /S \"%s\\*.*\" \"%s\\*.*" , _szSource, _szTarget);
    LaunchSystemCommand(sz_Command);
}


void LaunchSystemCommand(char * _pz_Command, bool _bWaitForIt /*=true*/)
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
}


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


// if the indicated process is running, returns a handle on it (NULL otherwise)
// if handle returned is not null, caller is responsible for closing it.
HANDLE pFindProcess( const char* _processName)
{
    char szProcessName[MAX_PATH] = "unknown";

    DWORD aProcesses[1024];
    DWORD cbNeeded, dwNbProcesses;
    unsigned int i;
    HANDLE hProcess;

    // Get running processes list
    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return NULL;

    // Calculate how many process identifiers were returned.
    dwNbProcesses = cbNeeded / sizeof(DWORD);


    // loop on each process.
    for ( i = 0; i < dwNbProcesses; i++ )
    {
        // get the process
        hProcess = OpenProcess( PROCESS_ALL_ACCESS,
            FALSE, 
            aProcesses[i] );


        // Get the process name.
        if ( hProcess )
        {
            HMODULE hMod;
            DWORD cbNeeded;

            if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
                &cbNeeded) )
            {
                GetModuleBaseName( hProcess, hMod, szProcessName, 
                    sizeof(szProcessName) );
            }

            // Test the process name (CASE INDEPENDANT).
            if (!stricmp(szProcessName, _processName) )
            {
                return hProcess;	// found it! You should close it after use
            }
        }

        CloseHandle( hProcess );
    }

    return NULL;	// not found
}


