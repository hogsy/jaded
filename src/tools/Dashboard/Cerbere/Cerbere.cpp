

#include "stdafx.h"
#include <winbase.h>
#include <io.h>
#include <afxsock.h>
#include <process.h>
#include <assert.h>

#include "Defines.h"
#include "Cerbere.h"
#include "FileWatch.h"


///////////////////////////////////////

cl_Cerbere::cl_Cerbere()
{
    m_hWnd = NULL;
    m_ulGlobalStatus = kul_FlagInitializing;
}

cl_Cerbere::~cl_Cerbere()
{
    DeInit();
}

void cl_Cerbere::Init(HWND _hWnd)
{
    m_hWnd = _hWnd;
    ReadInfoFromCommonIniFile();
    ReadInfoFromUserIniFile();
}

void cl_Cerbere::DeInit()
{
    SaveInfoToUserIniFile();
    CFileWatch::Stop();
    mo_PCWatched.clear();
}


void cl_Cerbere::ReadInfoFromCommonIniFile()
{
    // Nb of PC Watched
    int iNbPCWatched = GetPrivateProfileInt(kaz_CERBERE_SettingsSection, kaz_CERBERE_NbPCWatched, 0, kaz_CERBERE_IniFileCommon);

    // list of PC Watched
    mo_PCWatched.clear();   // reset list. Will be filled by "add..." method
    for ( int i = 0; i < iNbPCWatched; ++i )
    {
        char sz_Name[ki_MaxNameLength];
        char sz_PCWatched[ki_MaxNameLength];
        sprintf(sz_PCWatched, kaz_CERBERE_PCWatched, i);
        GetPrivateProfileString(kaz_CERBERE_SettingsSection, sz_PCWatched, kaz_CERBERE_Unknown, sz_Name, ki_MaxNameLength, kaz_CERBERE_IniFileCommon);
        AddPCtoWatch(sz_Name);
    }
}


void cl_Cerbere::SaveInfoToUserIniFile()
{
}

void cl_Cerbere::ReadInfoFromUserIniFile()
{
}


void cl_Cerbere::RunOneFrame()
{
    time(&g_CurrentTime);

    m_ulGlobalStatus = 0; // reset flags

    for ( UINT i = 0; i < mo_PCWatched.size(); i++ )
    {
        WatchPC(i);
        m_ulGlobalStatus |= mo_PCWatched[i].ulTest();
    }
}


void cl_Cerbere::AddPCtoWatch(const char* _pzPCName)
{
    // only add to file watch if it wasn't already in set
    //if()
    {
        // add the PC to the list
        mo_PCWatched.push_back( cl_PCwatch(_pzPCName) );
        UINT uiNewPCindex = (UINT)(mo_PCWatched.size()) -1;

        // start the watch
        WatchPC( uiNewPCindex );

        // init the status and settings
        for(int i=0; i<E_FileTypeNumber; i++)
        {   mo_PCWatched[uiNewPCindex].ReadFile( (tde_FileType)i );
        }
    }
}



// Register the files in CFileWatch
void cl_Cerbere::WatchPC(DWORD _ulPCIndex)
{
    assert( _ulPCIndex < mo_PCWatched.size() );

    for(int i=0; i<E_FileTypeNumber; i++)
    {   
        cl_FileWatch& roFile = mo_PCWatched[_ulPCIndex].m_oFile[i];

        if( 0 == roFile.m_hFileHandle  )   // only add files not watched yet
        {
            roFile.m_hFileHandle = CFileWatch::AddFileFolder( roFile.m_szFileName, m_hWnd );

            if( 0 != roFile.m_hFileHandle )
            {   CFileWatch::SetData( roFile.m_hFileHandle, ulEncodeHighLow(i, _ulPCIndex) );
            }
        }
    }
}



// Called By CFileWatch when a change is detected on one of the registered file
void cl_Cerbere::OnFileWatchNotification( LPCTSTR _pzFile, DWORD _uiParam )
{
    tde_FileType eFileType = (tde_FileType)( ulGetHigh(_uiParam) );
    DWORD ulPCIndex  = ulGetLow (_uiParam);

    assert( ulPCIndex < mo_PCWatched.size() );
    cl_PCwatch& rPC = mo_PCWatched[ulPCIndex];

    assert( 0 == strcmp(_pzFile, rPC.m_oFile[eFileType].m_szFileName) );
    rPC.ReadFile(eFileType);
}


// Called By CFileWatch when one of the registered file is deleted
void cl_Cerbere::OnFileWatchDelete( LPCTSTR _pzFile, DWORD _uiParam )
{
    tde_FileType eFileType = (tde_FileType)( ulGetHigh(_uiParam) );
    DWORD ulPCIndex  = ulGetLow (_uiParam);

    assert( ulPCIndex < mo_PCWatched.size() );
    cl_PCwatch& rPC = mo_PCWatched[ulPCIndex];

    assert( 0 == strcmp(_pzFile, rPC.m_oFile[eFileType].m_szFileName) );
    rPC.MarkFileAsDeleted(eFileType);
}


const char* cl_Cerbere::szGlobalStatus() const
{
    return ( 0 == m_ulGlobalStatus ) ? kaz_CERBERE_GlobalStatusOK : kaz_CERBERE_GlobalStatusNOTOK;
}
