#ifndef __PCWATCH_H__
#define __PCWATCH_H__


// STLs
#include <vector>
#include <string>
using namespace std;

#include "Defines.h"



/////////////////////////////////////////

// status flags
const DWORD kul_FlagInitializing    = 0x01;
const DWORD kul_FlagPCProblem       = 0x02;
const DWORD kul_FlagDataProblem     = 0x04;


enum tde_Activity
{
    E_ActivityUnknown   = 0,
    E_ActivityOK,
    E_ActivityPCDown,
    E_ActivityNotLaunched,
    E_ActivityFrozen,
    E_ActivitySuspended,
};

enum tde_RunningStatus
{
    E_RunningStatusUnknown   = 0,
    E_RunningStatusIddle,
    E_RunningStatusTesting,
    E_RunningStatusTestingUpdatePending,
    E_RunningStatusUpdating,
};

enum tde_TestStatus
{
    E_TestStatusUnknown   = 0,
    E_TestStatusOK,
    E_TestStatusFailed,
};



enum tde_FileType
{
    E_FileTypeTag         = 0,
    E_FileTypeIni,
    E_FileTypeNumber   // number of types : keep it at end!
};



/////////////////////////////////////////
class cl_FileWatch
{
public:

    DWORD               m_hFileHandle;
    bool                m_bFileDeleted;
    char                m_szFileName[MAX_PATH];

    void Reset()    {   m_hFileHandle = NULL;  m_bFileDeleted = false;  m_szFileName[0] = 0;    }
};


////////////////
class cl_PCsettings
{
public:
    bool   mb_Server;

    char    msz_ProjectName[ki_MaxNameLength];
    bool    mb_MapTestingEnable;
    int     mi_NbMapToTest;
    bool    mb_MapTestingXenonEnable;

    int     mi_EnginePCVersion;
    int     mi_EnginePCPatch;
    int     mi_EnginePCCompilNb;


    void Read(const char* _pzFile);
    void Reset();
};


////////////////
class cl_PCwatch
{
public:
    cl_PCwatch()                            {   Reset();    }
    cl_PCwatch(const char* _pszPCname)      {   Reset(); SetPCName(_pszPCname);  }
    ~cl_PCwatch()                           {}

    void Reset();

    //
    void SetPCName(const char* _pszPCname)              {   strcpy(m_szPCname, _pszPCname);     SetPCFileNames(); }
    void ReadFile(tde_FileType _eFileType);
    bool bIsFileDeleted(tde_FileType _eFileType) const  {   return m_oFile[_eFileType].m_bFileDeleted;  }
    void MarkFileAsDeleted(tde_FileType _eFileType)     {   m_oFile[_eFileType].m_bFileDeleted = true;  }
    
    DWORD ulTest();

    // 
    const char*         szStatus()          const;
    const char*         szRunningStatus()   const;
    const char*         szActivity()        const;



    //////////////
    // MEMBERS

    // PC name
    char                m_szPCname[MAX_PATH];

    // Files dynamically watched on this PC
    cl_FileWatch        m_oFile[E_FileTypeNumber];


    // activity
    time_t              m_Timer;
    float               m_fInactivityTime;
    tde_Activity        m_eActivity;


    // Status
    tde_RunningStatus   m_eRunningStatus;
    bool                m_bUpdateStatus;
    tde_TestStatus      m_eTestStatus;          // current data status (for test servers only)


    // PC Settings (desdac client/server, etc)
    cl_PCsettings       m_oSettings;


    //
    static bool GetLastWriteTime(LPCTSTR _lpszFileName, FILETIME &_rftLastWriteTime);

protected:
    bool bTestFile(const char* _pzFile);
    bool bReadTag (const char* _pzFile);
    bool bReadIni (const char* _pzFile);
    
    void TestStatus();

    void SetPCFileNames();

    // Files statically tested (presence only)
    char m_szRequestFileName    [MAX_PATH];
    char m_szUpdateFileName     [MAX_PATH];
    char m_szSuspendedFileName  [MAX_PATH];

};

typedef vector<cl_PCwatch>              PCvector;
typedef vector<cl_PCwatch>::iterator    PCvectorIterator;

extern time_t g_CurrentTime;



#endif//__PCWATCH_H__