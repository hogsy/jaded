

#include "stdafx.h"
#include <assert.h>

#include "Defines.h"
#include "PCwatch.h"

time_t g_CurrentTime;

///////////////////////////////////////
void cl_PCwatch::Reset()    
{
    m_szPCname[0]       = 0;

    for(int i=0; i<E_FileTypeNumber; i++)
    {   m_oFile[i].Reset();
    }
    
    m_Timer             = 0;
    m_fInactivityTime   = 0;
    m_eActivity         = E_ActivityUnknown;

    m_eRunningStatus    = E_RunningStatusUnknown;
    m_bUpdateStatus     = false;
    m_eTestStatus       = E_TestStatusUnknown;

    m_oSettings.Reset();
}

void cl_PCsettings::Reset()
{
    mb_Server                   = false;
    mb_MapTestingEnable         = false;
    mi_NbMapToTest              = 0;
    mb_MapTestingXenonEnable    = false;
}


void cl_PCwatch::SetPCFileNames()    
{
    // Files dynamically watched on this PC
    sprintf( m_oFile[E_FileTypeTag].m_szFileName,       kaz_DESDAC_RemoteTagFile,       m_szPCname );    
    sprintf( m_oFile[E_FileTypeIni].m_szFileName,       kaz_DESDAC_RemoteIniFile,       m_szPCname );

    // other files
    sprintf( m_szRequestFileName,       kaz_DESDAC_RemoteRequestFile,   m_szPCname );    
    sprintf( m_szUpdateFileName,        kaz_DESDAC_RemoteUpdateFile,    m_szPCname );  
    sprintf( m_szSuspendedFileName,     kaz_DESDAC_RemoteSuspendedFile, m_szPCname );  
}



void cl_PCwatch::ReadFile(tde_FileType _eFileType)
{
    bool bRead = false;
    char* pzFile = m_oFile[_eFileType].m_szFileName;

    switch(_eFileType)
    {
    case E_FileTypeTag:
        {
            bRead = bReadTag(pzFile);

            if(bRead)
            {   // set timer to current, for freeze detection: tag file is continuously updated when desdac runs
                time(&m_Timer);
            }
        }
        break;


    case E_FileTypeIni:
        {
            bRead = bReadIni(pzFile);
        }
        break;

    default:    assert(0);  // undefined type
    }

    m_oFile[_eFileType].m_bFileDeleted = !bRead;
}


// only test file presence
bool cl_PCwatch::bTestFile(const char* _pzFile)
{
    FILE * pFile = fopen(_pzFile, "rt");
    if(pFile)
    {   fclose(pFile);
    }

    return (NULL != pFile);
}


// read infos in tag file
bool cl_PCwatch::bReadTag(const char* _pzFile)
{
    FILE * pFile = fopen(_pzFile, "rt");
    if(pFile)
    {
        // read status in Tag file


        fclose(pFile);
    }

    return (NULL != pFile);
}


// read settings in ini file
bool cl_PCwatch::bReadIni(const char* _pzFile)
{
    // read settings in Ini file
    FILE * pFile = fopen(_pzFile, "rt");
    if(pFile)
    {
        // test presence only
        fclose(pFile);

        // now access settings
        m_oSettings.Read(_pzFile);

    }

    return (NULL != pFile);
}

void cl_PCsettings::Read(const char* _pzFile)
{   // read the settings in desdac ini file. must be coherent with desdac code
    
    // Client / Server
    mb_Server = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_ServerOrClient, 0, _pzFile) ? true : false;

    // Project name
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_ProjectName,     "", msz_ProjectName,    ki_MaxNameLength, _pzFile);
    
    // Engine version
    mi_EnginePCVersion = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_Version, 0, _pzFile);
    mi_EnginePCPatch   = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_Patch,   0, _pzFile);
    mi_EnginePCCompilNb= GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_CompilNb,0, _pzFile);

    // map testing
    char sz_TmpTest[64];    
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_MapTestingEnable, "", sz_TmpTest, 64, _pzFile);
    mb_MapTestingEnable = ( 0 == stricmp(sz_TmpTest, kaz_True) );
    mi_NbMapToTest = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_NbMapToTest, 0, _pzFile);

    // Map testing Xenon
    GetPrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_MapTestingXenonEnable, "", sz_TmpTest, 64, _pzFile);
    mb_MapTestingXenonEnable = ( !strcmp(sz_TmpTest, kaz_True) );
}


DWORD cl_PCwatch::ulTest()
{
    // if ini isn't accessible, PC is down
    if( bIsFileDeleted(E_FileTypeIni) )
    {
        m_eActivity = E_ActivityPCDown;
        return kul_FlagPCProblem;
    }

    // if tag isn't accessible, PC isn't launched
    if( bIsFileDeleted(E_FileTypeTag) )
    {
        m_eActivity = E_ActivityNotLaunched;
        return kul_FlagPCProblem;
    }
    

    // Test activity (in order to detect desdac freeze/crash on this PC)
    m_fInactivityTime = (float)difftime( g_CurrentTime, m_Timer );
    if( m_fInactivityTime > kf_InactivityTimeThreshold )
    {   
        m_eActivity = E_ActivityFrozen;
        return kul_FlagPCProblem;
    }

    //
    if( bTestFile(m_szSuspendedFileName) )
    {
        m_eActivity = E_ActivitySuspended;
        return 0;
    }


    // everything is ok
    m_eActivity = E_ActivityOK;

    TestStatus();
    return 0;
}

void cl_PCwatch::TestStatus()
{
    bool m_bUpdateStatus = bTestFile(m_szUpdateFileName);

    if( bTestFile(m_szRequestFileName) )
    {
        m_eRunningStatus = m_bUpdateStatus ? E_RunningStatusTestingUpdatePending : E_RunningStatusTesting;
    }
    else
    {
        m_eRunningStatus = m_bUpdateStatus ? E_RunningStatusUpdating : E_RunningStatusIddle;
    }
}


const char* cl_PCwatch::szStatus() const
{
    switch(m_eActivity)
    {
        case E_ActivityUnknown:     return kaz_CERBERE_Unknown;
        case E_ActivityOK:          return szRunningStatus();
        case E_ActivityPCDown:      return kaz_CERBERE_ActivityPCDown;
        case E_ActivityNotLaunched: return kaz_CERBERE_ActivityNotLaunched;
        case E_ActivityFrozen:      return kaz_CERBERE_ActivityFrozen;
        case E_ActivitySuspended:   return kaz_CERBERE_ActivitySuspended;

        default:    assert(0);      return kaz_CERBERE_Unknown;
    }
}

const char* cl_PCwatch::szRunningStatus() const
{
    switch(m_eRunningStatus)
    {
        case E_RunningStatusUnknown:                return kaz_CERBERE_Unknown;
        case E_RunningStatusIddle:                  return kaz_CERBERE_StatusIddle;
        case E_RunningStatusTesting:                return kaz_CERBERE_StatusTesting;
        case E_RunningStatusTestingUpdatePending:   return kaz_CERBERE_StatusTestingUpdatePending;
        case E_RunningStatusUpdating:               return kaz_CERBERE_StatusUpdating;

        default:    assert(0);      return kaz_CERBERE_Unknown;
    }
}

const char* cl_PCwatch::szActivity() const
{
    if( E_ActivityOK == m_eActivity )
    {
        int iActivity = (int)(m_fInactivityTime)%4;

        switch(iActivity)
        {
            case 0: return ".";
            case 1: return "..";
            case 2: return "...";
            case 3: return "....";
        }
    }

    return "";
}



bool cl_PCwatch::GetLastWriteTime(LPCTSTR _lpszFileName, FILETIME &_rftLastWriteTime)
{
    CFile file;
    if (file.Open(_lpszFileName, CFile::shareDenyNone))
    {
        BY_HANDLE_FILE_INFORMATION info;
        if (GetFileInformationByHandle((HANDLE)file.m_hFile, &info))
        {
            _rftLastWriteTime = info.ftLastWriteTime;
            return true;
        }
    }

    return false;
}




