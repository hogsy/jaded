#ifndef __BIGFILEUTILS_H__
#define __BIGFILEUTILS_H__

#define k_UserLength    64



class ChangeListToLaunch
{
public:
    int     mi_ChangeList;
    DWORD   mul_Options;

    int     mi_NbClSkipped;
    int     mi_FirstCLSkipped;

    char    msz_UserName[k_UserLength];
    char    msz_ClientName[k_UserLength];

    void Reset()
    {
        mi_ChangeList       = -1;
        mul_Options         = 0;
        mi_NbClSkipped      = 0;
        mi_FirstCLSkipped   = 0;
        msz_UserName[0]     = 0;
        msz_ClientName[0]   = 0;
    }
};

class ChangeListResult
{
public:
    ChangeListToLaunch  mo_LaunchedCL;

    bool    mb_AiFailed;
	bool    mb_AiToCFailed;
    bool    mb_DbFailed;
    bool    mb_MapTestFailed;
	bool    mb_BuildPrevFailed;
	bool    mb_BuildFailed;
	bool	mb_StateOK;
    int     mi_NbDbErrors;
    int     mi_NbAiErrors;
    int     mi_NbMapErrors;
	int     mi_NbBuildPrevErrors;
	int     mi_NbBuildErrors;

    bool    mb_ResultValid;
    bool    mb_Crashed;
    bool    mb_LogExists;

    int     mi_ClientIndex;

    CTime   mo_StartOfTestTime;
    CTime   mo_EndOfTestTime;
    CTime   mo_SubmitTime;
    bool    mb_SubmitTimeIsValid;

    int     mi_EnginePCVersion;
    int     mi_EnginePCPatch;
    int     mi_EnginePCCompilNb;

	char *	msz_AiLog;
	char *	msz_AiToCLog;

	bool	mb_FullTest;
	

    void Reset()
    {
        mo_LaunchedCL.Reset();
        
        mb_AiFailed         = false;
		mb_AiToCFailed		= false;
        mb_DbFailed         = false;
        mb_MapTestFailed    = false;
		mb_BuildPrevFailed	= false;
		mb_BuildFailed		= false;
		mb_StateOK			= false;
        mi_NbDbErrors       = -1;
        mi_NbAiErrors       = -1;
        mi_NbMapErrors      = 0;
		mi_NbBuildErrors	= -1;
		mi_NbBuildPrevErrors= -1;

        mb_ResultValid      = false;
        mb_Crashed          = false;
        mb_LogExists        = false;

        mi_ClientIndex      = -1;
        mb_SubmitTimeIsValid= false;

        mi_EnginePCVersion  = 0;
        mi_EnginePCPatch    = 0;
        mi_EnginePCCompilNb = 0;
		msz_AiLog			= "No_AI_Error";
		msz_AiToCLog		= "No_AI_Error";
		mb_FullTest			= false;
    }
};


#define BIG_k_NameLength        256
#define BIG_k_LogMessageLength  512
#define BIG_k_CommandLength     1024
#define BIG_k_MailMessageLength 16384



void    LaunchSystemCommand         (char * _pz_Command,bool _bWaitForIt);
bool    bLaunchSystemCommand        (char * _pz_Command);
void    CopyFolder                  (const char* _szSource, const char* _szTarget);

int     BIG_iGetLastClTested        ();

bool    BIG_bGetAiResultStatus      ();
bool    BIG_bGetDatabaseCheckStatus ();
bool    BIG_bGetGlobalResultStatus  ();


void    BIG_SetPerforceServerPort   (char * _pz_ServerPort);
void    BIG_SetPerforceDepotBranch  (char * _pz_DepotBranch);
void    BIG_SetPerforceTempFile     (char * _pz_TempFile);
void    BIG_SetPerforceCurrentUser  (char * _pz_CurrentUser);
void    BIG_SetBigFileName          (char * _pz_BigFileName);

char *  BIG_pz_GetPerforceServerPort    ();
char *  BIG_pz_GetPerforceDepotBranch   ();
char *  BIG_pz_GetPerforceTempFile      ();
char *  BIG_pz_GetPerforcePasswordFile  ();
char *  BIG_pz_GetPerforceCurrentUser   ();
char *  BIG_pz_GetExeArguments          ();
char *  BIG_pz_GetExeName               ();
char *  BIG_pz_GetBigFileName           ();
char *  BIG_pz_GetLastUser              ();



////////////////////////////////////////////
// utility funcs
void        ExtractPathFromFullName (char* _pzPath, const char* _pzFullName);
const char* pzGetFileNameFromFullPath(const char* _pzFullName);
const char* pzSimpleMapName         (const char* _pzMapName);
bool        bAppendTxtFileToMessage (const char* _sz_File, char* sz_Message);
bool        bGetExeVersion          (DWORD& _ulV1, DWORD& _ulV2, DWORD& _ulV3, DWORD& _ulV4);
bool        bGetExeVersion          (char* _szVersion);
DWORD       ulGetFileSize           (const char* _pzFile);
CString     szFormatDuration        (const CTimeSpan& _roT);
bool        bGetFileCreationTime    (const char* _szFile, CTime& _rFileTime);
bool        bGetFileWriteTime       (const char* _szFile, CTime& _rFileTime);

#endif//__BIGFILEUTILS_H__