#ifndef __DESDAC_H__
#define __DESDAC_H__

//=========================================
// DESDAC means :
// DEdicated Server for DAta Coherency
//=========================================

#include <winbase.h>
#include <io.h>

#include "BigFileUtils.h"


//////////////////////////////////////////////////////////
const int ki_DESDAC_TimerValue = 2000;


#define k_MaxClientPc       256
#define k_MaxMapToTest      256
#define k_MaxMailToSend     256

#define kaz_True            "True"
#define kaz_False           "False"

#define k_Option_GetFromPerforce    0x00000001
#define k_Option_CheckPerforce      0x00000002
#define k_Option_RepairPerforce     0x00000004
#define k_Option_CompileAi          0x00000008
#define k_Option_LoadMap            0x00000010
#define k_Option_LaunchXenon        0x00000020
#define k_Option_CompileAiToC       0x00000040

#define k_ClientStatus_Waiting      0
#define k_ClientStatus_Busy         1
#define k_ClientStatus_Down         2
#define k_ClientStatus_NotLaunched  3
#define k_ClientStatus_Suspended    4
#define k_ClientStatus_Updating     5

// Update type flags for auto-update requests
const DWORD kuiEnginePCUpdate    = 0x01;
const DWORD kuiEngineXenonUpdate = 0x02;
const DWORD kuiNewBigfileUpdate  = 0x04;
const DWORD kuiDesdacSelfUpdate  = 0x08;
const DWORD kuiDesdacAutoQuit    = 0x10;

#define kaz_MainWindowName              "Automatic Data Check"


// Status
#define kaz_ClientStatus_Waiting        "waiting"
#define kaz_ClientStatus_Busy           "running"
#define kaz_ClientStatus_Down           "down"
#define kaz_ClientStatus_NotLaunched    "not launched"
#define kaz_ClientStatus_Suspended      "suspended"
#define kaz_ClientStatus_Updating       "performing update"
#define kaz_ClientStatus_Unknown        "gone with the wind"

#ifdef MATHIEU
#define kaz_FolderSystem                "SYS"
#define kaz_FolderResults               "Results"
#define kaz_FolderVersion               "Version"
#define kaz_FolderVersion_backup        "Version_BAK"
#define kaz_FolderVersionOverrides      "VersionOverrides"
#else
#define kaz_FolderSystem                "SYS"
#define kaz_FolderResults               "Results"
#define kaz_FolderVersion               "Version"
#define kaz_FolderVersion_backup        "Version_BAK"
#define kaz_FolderVersionOverrides      "VersionOverrides"
#endif


//////////////////////////////////////////////////////
// INI FILE
#define kaz_DESDAC_IniFile                  ".\\SYS\\desdac.ini"     // use relative path to avoid windows folder

// INI Sections
#define kaz_DESDAC_Section                  "desdac"
#define kaz_DESDACXenon_Section             "desdacXenon"

// INI Entries
#define kaz_DESDAC_PcServer                 "pcserver"
#define kaz_DESDAC_PcClient                 "pcclient%d"
#define kaz_DESDAC_NbClients                "nbclients"
#define kaz_DESDAC_ServerOrClient           "serverorclient"
#define kaz_DESDAC_ProjectName              "projectname"
#define kaz_DESDAC_PerforceServer           "perforceserver"
#define kaz_DESDAC_PerforceUser             "perforceuser"
#define kaz_DESDAC_DepotBranch              "depotbranch"
#define kaz_DESDAC_Password                 "password"
#define kaz_DESDAC_ExeName                  "exename"
#define kaz_DESDAC_JadeSourcesPath			"JadeSources"
#define kaz_DESDAC_BigFileName              "bigfilename"
#define kaz_DESDAC_EnginePCUpdatePath       "engineupdatepath"
#define kaz_DESDAC_TempFileName             "tempfilename"
#define kaz_DESDAC_OutputFolder             "outputfolder"
#define kaz_DESDAC_DefaultMail              "defaultmail"
#define kaz_DESDAC_SyncDepth                "syncdepth"
#define kaz_DESDAC_Version                  "version"
#define kaz_DESDAC_Patch                    "patch"
#define kaz_DESDAC_CompilNb                 "compilnb"
#define kaz_DESDAC_LastUpdate               "lastupdate"
#define kaz_DESDAC_MapName_NB               "mapName%d"
#define kaz_DESDAC_MapKey_NB                "mapKey%d"
#define kaz_DESDAC_MapTestingEnable         "maptesting"
#define kaz_DESDAC_NbMapToTest              "nbmaptotest"
#define kaz_DESDAC_MailToSend               "mailtosend"
#define kaz_DESDAC_NbMailToSend             "nbmailtosend"
#define kaz_DESDAC_SendMessages             "sendmessages"
#define kaz_DESDAC_SendMessagesUser         "sendmessagesuser"
#define kaz_DESDAC_StackThreshold           "stackthreshold"
#define kaz_DESDAC_TestLastCLonlyEnable     "TestLastCLonlyEnable"


// INI Entries Xenon
#define kaz_DESDAC_MapTestingXenonEnable    "XenonMapTesting"
#define kaz_DESDAC_XenonKitName             "XenonKitName"
#define kaz_DESDAC_XenonBigFileName         "XenonBigFileName"
#define kaz_DESDAC_XenonExeName             "XenonExeName"
#define kaz_DESDAC_XenonLauncherName        "XenonLauncherName"
#define kaz_DESDAC_XenonRebootTime          "XenonRebootTime"
#define kaz_DESDAC_XenonMapTestTime         "XenonMapTestTime"
#define kaz_DESDAC_EngineXenonUpdatePath    "XenonEngineupdatepath"


// Default Ini Values
#define kaz_DESDAC_DefaultPcServer          "pc-ojourdan-twk"
#define kaz_DESDAC_DefaultPcClient          "invalid"
#define kaz_DESDAC_DefaultMailToSend        "DonaldDuck@Picsou-Magazine.com"
#define kaz_DESDAC_DefaultProjectName       "GRAW"
#define kaz_DESDAC_DefaultPerforceServer    "10.0.0.2:1668" //"10.0.0.2:1667"
#define kaz_DESDAC_DefaultPerforceUser      "mhidrio"
#define kaz_DESDAC_DefaultDepotBranch       "//rayman4-bf/version/Production/..." //"//gr3_gamedata/Data/..."
#define kaz_DESDAC_DefaultPassword          "password"
#define kaz_DESDAC_DefaultExeName           "\"jade_edd.exe\"" //"\"Yeti Game Platform.exe\""
#define kaz_DESDAC_DefaultSelfUpdaterName   "SelfUpdater.exe"
#define kaz_DESDAC_DefaultSelfUpdatePath    "\\\\Pc-robot-mrc\\desdac\\_ForInstall\\SelfUpdate"
#define kaz_DESDAC_DefaultBigFileName       "D:\\DESDAC\\Bigfile\\gr3_database.big"
#define kaz_DESDAC_DefaultTempFileName      "desdac.txt"
#define kaz_DESDAC_DefaultOutputFolder      "Output"
#define kaz_DESDAC_DefaultDefaultMail       "ojourdan@tiwak.com"
const int ki_DESDAC_DefaultVersion          = 0;
const int ki_DESDAC_DefaultPatch            = 0;
const int ki_DESDAC_DefaultCompilNb         = 0;
#define kaz_DESDAC_DefaultLastUpdate        "00/00/0000 00:00:00"
#define kaz_DESDAC_DefaultMapName           "invalidmap"
#define kaz_DESDAC_DefaultMapTesting        kaz_False
const int ki_DESDAC_DefaultNbMapToTest      = 0;

#define kaz_DESDAC_DefaultEnginePCUpdatePath        "\\\\Srvdata1-mrc\\GhostRecon3\\TOOLS\\EDITOR\\STABLE"
#define kaz_DESDAC_DefaultEngineXenonUpdatePath     "\\\\Srvdata1-mrc\\GhostRecon3\\TOOLS\\EDITOR\\STABLE\\Xenon"

#define kaz_DESDAC_DefaultXenonKitName              "kipouti"
#define kaz_DESDAC_DefaultXenonBigFileName          "xE:\\YetiDESDAC\\yeti.big"
#define kaz_DESDAC_DefaultXenonExeName              "xE:\\YetiDESDAC\\Yeti_Release_Xenon.exe"
#define kaz_DESDAC_DefaultXenonLauncherName         "D:\\DESDAC\\XMW\\XMW.exe"
#define kaz_DESDAC_DefaultXenonDVDEmuName           "D:\\DESDAC\\Bigfile\\VIDE.exe"
const DWORD kul_DESDAC_DefaultXenonRebootTime       = 90;       // in seconds
const float kf_DESDAC_DefaultXenonMapTestTime       = 30.0f;    // in seconds
const int   ki_DefaultStackThresholdWarning         = 50;       // warn if nb of Change Lists waiting in stack reach this number



//////////////////////////////////////////////////////
// Client local files
#ifdef MATHIEU		
#define kaz_DESDAC_JadeSolFile						"\\Main\\Jade.sln"
#define kaz_DESDAC_AIinterpFolder					"\\Libraries\\AIinterp\\Sources\\"

#define kaz_DESDAC_RequestUpdateFile                "SYS\\update.txt"
#define kaz_DESDAC_RequestFile                      "SYS\\request.txt"
#define kaz_DESDAC_ResultsFile                      "Results\\result.txt"
#define kaz_DESDAC_ResultsTempFile                  "Results\\result2.txt"
#define kaz_DESDAC_CrashDumpFile                    "Results\\CrashDump.log"    // file generated in case of crash
#define kaz_DESDAC_CrashDumpFile_MapNB              "Results\\CrashDump%d.log"  
#define kaz_DESDAC_AIBugsFile                       "Results\\AIBugs.txt"   // file generated in case of ai bugs during launch map PC
#define kaz_DESDAC_TagFile                          "SYS\\tag.txt"          // File used to say that DESDAC in launched
#define kaz_DESDAC_SuspendedFile                    "SYS\\suspended.txt"    // File used to say that client is suspended
#define kaz_DESDAC_XenonResultDumpFile              "Results\\xmw.dmp"
#define kaz_DESDAC_XenonResultDumpFile_MapNB        "Results\\xmw%d.dmp"
#define kaz_DESDAC_XenonResultScreenShotFile        "Results\\xmw.jpg"
#define kaz_DESDAC_XenonResultScreenShotFile_MapNB  "Results\\xmw%d.jpg"
#define kaz_DESDAC_XenonResultLogFile               "Results\\xmw.log"
#define kaz_DESDAC_XenonResultLogFile_MapNB         "Results\\xmw%d.log"
#else
#define kaz_DESDAC_RequestUpdateFile                "SYS\\update.txt"
#define kaz_DESDAC_RequestFile                      "SYS\\request.txt"
#define kaz_DESDAC_ResultsFile                      "Results\\result.txt"
#define kaz_DESDAC_ResultsTempFile                  "Results\\result2.txt"
#define kaz_DESDAC_CrashDumpFile                    "Results\\CrashDump.log"    // file generated in case of crash
#define kaz_DESDAC_CrashDumpFile_MapNB              "Results\\CrashDump%d.log"  
#define kaz_DESDAC_AIBugsFile                       "Results\\AIBugs.txt"   // file generated in case of ai bugs during launch map PC
#define kaz_DESDAC_TagFile                          "SYS\\tag.txt"          // File used to say that DESDAC in launched
#define kaz_DESDAC_SuspendedFile                    "SYS\\suspended.txt"    // File used to say that client is suspended
#define kaz_DESDAC_XenonResultDumpFile              "Results\\xmw.dmp"
#define kaz_DESDAC_XenonResultDumpFile_MapNB        "Results\\xmw%d.dmp"
#define kaz_DESDAC_XenonResultScreenShotFile        "Results\\xmw.jpg"
#define kaz_DESDAC_XenonResultScreenShotFile_MapNB  "Results\\xmw%d.jpg"
#define kaz_DESDAC_XenonResultLogFile               "Results\\xmw.log"
#define kaz_DESDAC_XenonResultLogFile_MapNB         "Results\\xmw%d.log"
#endif

// Remote Files
#ifdef MATHIEU
#define kaz_DESDAC_JadeServerExe							"\\\\srvdata4-mtp\\Sally_game\\Sally_Tools\\Jade\\jade_edd.exe"
#define kaz_DESDAC_RemoteDesdacFolder                       "\\\\%s\\AutoCompiler"
#define kaz_DESDAC_RemoteDesdacVersionFolder                "\\\\%s\\AutoCompiler\\Version"
#define kaz_DESDAC_RemoteRequestFile                        "\\\\%s\\AutoCompiler\\SYS\\request.txt"
#define kaz_DESDAC_RemoteRequestTempFile                    "\\\\%s\\AutoCompiler\\SYS\\request2.txt"
#define kaz_DESDAC_RemoteBuildPrevFile                      "\\\\%s\\AutoCompiler\\Results\\BuildPrevLog%d.txt"
#define kaz_DESDAC_RemoteResultsFile                        "\\\\%s\\AutoCompiler\\Results\\result.txt"
#define kaz_DESDAC_RemoteCrashDumpFile                      "\\\\%s\\AutoCompiler\\Results\\CrashDump.log"
#define kaz_DESDAC_RemoteCrashDumpFile_MapNB                "\\\\%s\\AutoCompiler\\Results\\CrashDump%d.log"
#define kaz_DESDAC_RemoteAIBugsFile                         "\\\\%s\\AutoCompiler\\Results\\AIBugs.txt"
#define kaz_DESDAC_RemoteLogFile                            "\\\\%s\\AutoCompiler\\Results\\log%d.txt"
#define kaz_DESDAC_RemoteBuildLogFile                       "\\\\%s\\AutoCompiler\\Results\\BuildLog%d.txt"
#define kaz_DESDAC_RemoteTagFile                            "\\\\%s\\AutoCompiler\\SYS\\tag.txt"
#define kaz_DESDAC_RemoteSuspendedFile                      "\\\\%s\\AutoCompiler\\SYS\\suspended.txt"
#define kaz_DESDAC_RemoteXenonResultDumpFile_MapNB          "\\\\%s\\AutoCompiler\\Results\\xmw%d.dmp"
#define kaz_DESDAC_RemoteXenonResultScreenShotFile_MapNB    "\\\\%s\\AutoCompiler\\Results\\xmw%d.jpg"
#define kaz_DESDAC_RemoteXenonResultLogFile_MapNB           "\\\\%s\\AutoCompiler\\Results\\xmw%d.log"
#define kaz_DESDAC_RemoteRequestUpdateFile                  "\\\\%s\\AutoCompiler\\SYS\\update.txt"
#define kaz_DESDAC_RemoteRequestUpdateTempFile              "\\\\%s\\AutoCompiler\\SYS\\update2.txt"
#else
#define kaz_DESDAC_RemoteDesdacFolder                       "\\\\%s\\DESDAC"
#define kaz_DESDAC_RemoteDesdacVersionFolder                "\\\\%s\\DESDAC\\Version"
#define kaz_DESDAC_RemoteRequestFile                        "\\\\%s\\DESDAC\\SYS\\request.txt"
#define kaz_DESDAC_RemoteRequestTempFile                    "\\\\%s\\DESDAC\\SYS\\request2.txt"
#define kaz_DESDAC_RemoteResultsFile                        "\\\\%s\\DESDAC\\Results\\result.txt"
#define kaz_DESDAC_RemoteCrashDumpFile                      "\\\\%s\\DESDAC\\Results\\CrashDump.log"
#define kaz_DESDAC_RemoteCrashDumpFile_MapNB                "\\\\%s\\DESDAC\\Results\\CrashDump%d.log"
#define kaz_DESDAC_RemoteAIBugsFile                         "\\\\%s\\DESDAC\\Results\\AIBugs.txt"
#define kaz_DESDAC_RemoteLogFile                            "\\\\%s\\DESDAC\\Results\\log%d.txt"
#define kaz_DESDAC_RemoteTagFile                            "\\\\%s\\DESDAC\\SYS\\tag.txt"
#define kaz_DESDAC_RemoteSuspendedFile                      "\\\\%s\\DESDAC\\SYS\\suspended.txt"
#define kaz_DESDAC_RemoteXenonResultDumpFile_MapNB          "\\\\%s\\DESDAC\\Results\\xmw%d.dmp"
#define kaz_DESDAC_RemoteXenonResultScreenShotFile_MapNB    "\\\\%s\\DESDAC\\Results\\xmw%d.jpg"
#define kaz_DESDAC_RemoteXenonResultLogFile_MapNB           "\\\\%s\\DESDAC\\Results\\xmw%d.log"
#define kaz_DESDAC_RemoteRequestUpdateFile                  "\\\\%s\\DESDAC\\SYS\\update.txt"
#define kaz_DESDAC_RemoteRequestUpdateTempFile              "\\\\%s\\DESDAC\\SYS\\update2.txt"
#endif

#define kaz_DESDAC_CLsavedFile								"\\\\srvdata3-mtp\\Bigfile\\Rayman4\\_DO_NOT_DELETE_Bigfile_Ref\\DesdacOK_CL.txt"

#define kaz_DESDAC_Html                                     "desdac.htm"
#define kaz_DESDAC_NewHtml                                  "desdac2.htm"

#define kaz_DESDAC_HtmlMapTest                              "desdac_MapTest.htm"
#define kaz_DESDAC_NewHtmlMapTest                           "desdac_MapTest2.htm"

#define kaz_DESDAC_LogFile_CLNB                             "log%d.txt"
#define kaz_DESDAC_BuildLogFile_CLNB                        "BuildLog%d.txt"
#define kaz_DESDAC_BuildPrevLogFile_CLNB                    "BuildPrevLog%d.txt"
#define kaz_DESDAC_CrashDumpFile_CLNB                       "Log%dCrashDump.txt"
#define kaz_DESDAC_CrashDumpFile_CLNB_MapName               "Log%dCrashDump_%s.txt"
#define kaz_DESDAC_AIBugsFile_CLNB                          "Log%dAIBugs.txt"
#define kaz_DESDAC_XenonResultDumpFile_CLNB_MapName         "Log%dXmw_%s.dmp"
#define kaz_DESDAC_XenonResultScreenShotFile_CLNB_MapName   "Log%dXmw_%s.jpg"
#define kaz_DESDAC_XenonResultLogFile_CLNB_MapName          "Log%dXmw_%s.log"
#define kaz_DESDAC_SimplifiedLogFile                        "SimpleLog.txt"

#define kaz_DESDAC_MapTestingFile                           "maptesting.txt"
#define kaz_DESDAC_MailLog                                  "DesdacMails.Log"           // used to trace mails sent
#define kaz_DESDAC_MailLogContent                           "DesdacMailsContent.Log"    // (generated content of the mail. used for debug)

#define kaz_DESDAC_EngineVersion                            "%d.%d.%d"

#define CSV_SEPARATOR                       ","
#define kTab                                "\t"



//////////////////////////////////////////
// HTML defs

// !!! these two ones MUST have same same number of chars
#define kaz_DESDAC_HTML_CL_Black        "<font color=\"#000000\">CL : <b>%6d</b> </font>"
#define kaz_DESDAC_HTML_CL_Red          "<font color=\"#FF0000\">CL : <b>%6d</b> </font>"

// HTML aligned tags
#define kaz_DESDAC_HTML_NT_NB_ALIGN                           "<b>  NT     </b>"            // "Not Tested"
#define kaz_DESDAC_HTML_OK_NB_ALIGN     "<font color=\"green\"><b>  OK     </b></font>"
#define kaz_DESDAC_HTML_FAILED_NB         "<font color=\"red\"><b>FAILED(%d)</b></font>"

#define kaz_DESDAC_HTML_NT                                    "<b>  NT  </b>"               // "Not Tested"
#define kaz_DESDAC_HTML_OK              "<font color=\"green\"><b>  OK  </b></font>"
#define kaz_DESDAC_HTML_FAILED            "<font color=\"red\"><b>FAILED</b></font>"
#define kaz_DESDAC_HTML_AIBUG             "<font color=\"red\"><b>AI BUG</b></font>"
#define kaz_DESDAC_HTML_CRASHED       "<font color=\"#C11B17\"><b>CRASH </b></font>"

#define kaz_DESDAC_HTML_CL_INDENT       "                  "
                                        
#define kaz_DESDAC_HTML_CL_BOOKMARK     "<a name=\"%d\"></a>"



// Time Formats
#define kaz_DESDAC_DATETIMEFORMAT       "%d %b %y %H:%M:%S"
#define kaz_DESDAC_TIMEFORMAT           "%H:%M:%S"

// Targets
#define kaz_DESDAC_TARGET_PC            "PC"
#define kaz_DESDAC_TARGET_XENON         "XENON"

// Status names (cf enum tde_Status)
#define kaz_DESDAC_StatusNotTested          "Not Tested"
#define kaz_DESDAC_StatusOK                 "OK"
#define kaz_DESDAC_StatusAIFailed           "AI Compile Error"
#define kaz_DESDAC_StatusAIBug              "AI BUG"
#define kaz_DESDAC_StatusCrashed            "CRASH"


//////////////////////////////////////////
const int   ki_MaxChangeListInStack     = 500;
const DWORD k_ulXenonTransferBlockSize  = 256*1024;  // max: 256ko


enum tde_Status
{
    E_StatusNotTested       = 0,
    E_StatusOK,
    E_StatusAICompileError,
    E_StatusAIBug,
    E_StatusCrashed,
    E_StatusUnidentifiedError,
    E_StatusCount   // keep it last: used to count the number of status
};

const char* szStatusFullName( tde_Status _eStatus );
const char* szStatusHtmlTag(  tde_Status _eStatus );
bool        bIsFailedStatus ( tde_Status _eStatus );



typedef struct st_MapToTest_
{
    char            msz_MapName[BIG_k_NameLength];  // name used for displaying results
    char            msz_MapKey[BIG_k_NameLength];   // key transfered to the engine (can be the full path, or the hexa key...)
    tde_Status      me_LoadStatus;
    tde_Status      me_LaunchStatus;
    tde_Status      me_CloseStatus;

    void ResetTestResults()  {   me_LoadStatus = E_StatusNotTested; me_LaunchStatus = E_StatusNotTested; me_CloseStatus = E_StatusNotTested;  }
} st_MapToTest;




const int k_iNotDefined = -1;
class st_ClientPc
{
public:
    char            msz_ClientName[BIG_k_NameLength];
    unsigned long   mul_Options;
    bool            mb_ClientAvailableForRequest;
    bool            mb_ClientAware; // Hello J-C ;)
    bool            mb_EngineUpdateRequest;

    int             mi_ClientStatus;

    // just for info
    int             mi_RunningCL;
    CTime           mo_StartOfTestTime;

    st_ClientPc()   { mi_RunningCL = k_iNotDefined; };
};

typedef struct st_Request_
{
    ChangeListToLaunch  mst_CL;
    unsigned long       mul_Option;

} st_Request;



class cl_Desdac
{
protected:
    // **** MEMBERS **** //

    // Desdac Status
    bool            mb_FirstLaunch;
    bool            mb_IsClientSuspended;
    bool            mb_IsServerSuspended;
    bool            mb_YetiPCisRunning;
	bool            mb_VisualIsRunning;
	bool            mb_VssIsRunning;
    bool            mb_YetiXENONisRunning;
    bool            mb_XENONisRebooting;
    bool            mb_CopyToXENONisRunning;
    float           mf_CopyPercentProgress;
    st_Request      mst_CurrentRequest;


    // Desdac Settings
    bool            mb_Server;
    char            msz_PcServerName[BIG_k_NameLength];
    unsigned long   mul_NbClients;
    int             mi_NbAvailableClients;
    int             mi_NbDownClients;
    st_ClientPc     mast_ClientPCs[k_MaxClientPc];

    int             mi_NbMailsToSend;
    char            masz_MailsToSend[k_MaxMailToSend][BIG_k_NameLength];
    bool            mb_SendMessages;
    bool            mb_SendMessageUser;

    int             mi_TimerDuration;   // in seconds
    int             mi_SyncDepth;
    
    char            msz_ProjectName             [BIG_k_NameLength];
    char            msz_PerforceServer          [BIG_k_NameLength];
    char            msz_PerforceUser            [BIG_k_NameLength];
    char            msz_ClientPassword          [BIG_k_NameLength];
    char            msz_ExeName                 [BIG_k_NameLength];
    char            msz_DepotBranch             [BIG_k_NameLength];
    char            msz_ExeAndLogPath           [BIG_k_NameLength];
    char            msz_BigFileName             [BIG_k_NameLength];
	char            msz_JadeSourcesPath         [BIG_k_NameLength];
    char            msz_DesdacTempFile          [BIG_k_NameLength];
    char            msz_LogFileName             [BIG_k_NameLength];
	char            msz_BuildLogFileName        [BIG_k_NameLength];
	char            msz_BuildPrevLogFileName    [BIG_k_NameLength];
    char            msz_OutputFolder            [BIG_k_NameLength];
    char            msz_DefaultMail             [BIG_k_NameLength];
    char            msz_ComputerName            [BIG_k_NameLength];
    char            msz_EnginePCUpdatePath      [BIG_k_NameLength];
    char            msz_EngineXenonUpdatePath   [BIG_k_NameLength];
    


    //  XENON full names
    char msz_XenonKitName[BIG_k_NameLength];
    char msz_XenonBigfileName[BIG_k_NameLength];
    char msz_XenonExeName[BIG_k_NameLength];
    char msz_XenonLauncherName[BIG_k_NameLength];
    char msz_XenonLauncherPath[BIG_k_NameLength];
    

    //////////////
    // testing Settings
    bool            mb_TestLastCLonlyEnable;
    bool            mb_CheckPerforceEnable;
    bool            mb_CompileAiEnable;
	bool			mb_AiToCTestingEnable;

    //////////////
    // Map testing (PC)
    bool            mb_MapTestingEnable;
    st_MapToTest    mast_MapToTest[k_MaxMapToTest];
    int             mi_NbMapToTest;

    //////////////
    // Map testing (Xenon)
    bool            mb_MapTestingXenonEnable;
    bool            mb_XenonDVDEmuMode;
    DWORD           mul_RebootTimeXenon;    // time to wait when xenon reboots (in seconds)
    float           mf_MapTestTimeXenon;    // in seconds

    // maps to test for the given request
    st_MapToTest    mast_MapToTestOnClient[k_MaxMapToTest];
    int             mi_NbMapToTestOnClient;
    int             mi_FirstRemainingMapToTestOnClient;


    //////////////////////
    // Yeti version update
    int             mi_EnginePCVersion;
    int             mi_EnginePCPatch;
    int             mi_EnginePCCompilNb;

    CTime           mo_EngineLastUpdateTime;


    /////////////////////////////////////////
    // Stack for change list to get and check
    int                 mi_NbClInStack;
    ChangeListToLaunch  mast_ClStack[ki_MaxChangeListInStack];
    ChangeListToLaunch  mst_CurrentCl;


    ///////////////////
    // Stack of results
    int                 mi_NbClResultInStack;
    ChangeListResult    mast_ClResultStack[ki_MaxChangeListInStack];
    ChangeListResult    mst_LastClResult;


    STARTUPINFO				mst_Async_StartUpInfo;
    PROCESS_INFORMATION		mst_Async_ProcInfo;

    STARTUPINFO				mst_Async_StartUpInfo_Visual;
    PROCESS_INFORMATION		mst_Async_ProcInfo_Visual;

    STARTUPINFO				mst_Async_StartUpInfo_VSS;
    PROCESS_INFORMATION		mst_Async_ProcInfo_VSS;

    STARTUPINFO				mst_Async_StartUpInfo_DVDEmu;
    PROCESS_INFORMATION		mst_Async_ProcInfo_DVDEmu;

    bool                    mb_YetiPCHasCrashed;
    int                     mi_StackThresholdWarning;


    // internal statistics, for tests and watch
    static int      mi_StatMaxNbClReachedInStack;    // server
    static int      mi_NbClTreatedOnClient;
    static DWORD    mul_BigfileSizeOnCLient;


    // Functions
    void            RunOneFrame_Server();
    void            RunOneFrame_Client();
    bool            bCheckAndStartEngineUpdate();
    bool            bUpdatePCEngine(const char* _szRemoteVersionPath);
    bool            bUpdateXenonEngine(const char* _szRemoteVersionPath);
    bool            bCheckAndStartRequest();

    void            PollPerforceServer();

    void            PushChangeList  (int _iCl, char * _pz_UserName, char * _pz_ClientName);
    void            PopChangeList   (ChangeListToLaunch * _pst_ChangeList);

    void            PushLaunchedChangeListToResult  (const ChangeListToLaunch * _pst_ChangeList);
    void            PopChangeListResult             (ChangeListResult * _pst_ChangeList);
    bool            bCanPopChangeListResult         ();
    void            SetchangeListResultValue        ( ChangeListResult& _rCLres );
    void            UpdateChangeListResultStack     ();
    void            CheckStackThreshold             ();
    bool            bStackThresholdCrossUpDetection (int _iVal, bool &_rbAlreadyUp);
    void            UpdateStatusFile                ();
    void            GetLocalClientStatus            ( char* _pszStatus );
    void            UpdateHtmlMainFile              ( const ChangeListResult& _rCLres );
    void            UpdateHtmlTestMapFile           ( const ChangeListResult& _rCLres );
    void            TagMailedCL                     ( int _iChangeList, const char* pzHtmlFile );

    void            AppendAIMessage                 ( const ChangeListResult& st_CLres, char sz_Message[], bool _bNewError );
	void            AppendAiToCMessage              ( const ChangeListResult& st_CLres, char sz_Message[], bool _bNewError );
	void            AppendBuildPrevMessage          ( const ChangeListResult& st_CLres, char sz_Message[], bool _bNewError );
	void            AppendBuildMessage		        ( const ChangeListResult& st_CLres, char sz_Message[], bool _bNewError );
    void            AppendDBMessage                 ( const ChangeListResult& st_CLres, char sz_Message[] );
    void            AppendCrashMessage              ( const ChangeListResult& st_CLres, char sz_Message[] );
    void            AppendMapTestMessage            ( const ChangeListResult& st_CLres, char sz_Message[] );
    void            SendAIWarning                   ( ChangeListResult& st_Tmp, bool _bNewError );
	void            SendAiToCWarning                ( ChangeListResult& st_Tmp, bool _bNewError );	
	void            SendBuildPrevWarning            ( ChangeListResult& st_Tmp, bool _bNewError );
	void            SendBuildWarning	            ( ChangeListResult& st_Tmp, bool _bNewError );
    void            SendDBWarning                   ( ChangeListResult& st_Tmp );
    void            SendCrashWarning                ( ChangeListResult& st_Tmp );
    void            SendMapTestWarning              ( ChangeListResult& st_Tmp );
    void            SendAdminWarning                ( char* _szMessage );
    void            GetP4HostName                   ( const ChangeListResult& st_Tmp, char sz_HostName[] );
    void            GetP4UserMailName               ( const ChangeListResult& st_Tmp, char sz_Mail[] );

	bool			bAiErrorDetected				();
	bool			bAiToCErrorDetected				();
	int				iBuildPrevErrorDetected			();
	int				iBuildErrorDetected				();

	void			SaveNumCL						(int _iCL);
    void            AnalyzeAndLaunchRequest         (char * _pz_RequestFile);
    void            AnalyzeAndTreatResults          ();
    bool            bCopyDataToXenon                ();
    bool            bCopyFileToXenon                (const char* _pzSrcFile, const char* _pzTargetFile );
    bool            bCopyFolderToXenon              (const char* _pzSrcPath, const char* _pzTargetPathXenon );
    void            LaunchVersionGenerationAsync    (ChangeListToLaunch * _pst_ChangeList);
    void            LaunchXenon                     ();
    void            LaunchOneMapXenon               (const char* _pszMapKey);
    void            LaunchDVDEmuXenon               ();
    void            TestYetiFinished                ();
    void            FinishTest                      ();
    void            UpdateClientStatistics          ();
    static bool     bTestAsyncProcessIsFinished     (PROCESS_INFORMATION& _rProcInfo);
    bool            bTestPCisFinished               ();
    bool            bTestXENONisFinished            ();
    void            Temporize                       (DWORD _uiSeconds);



    void            CleanFilesAtStart();
    void            CleanFiles();

    void            CreateSimplifiedLogFileForAi(char * _pz_LogFile);
	void            CreateSimplifiedLogFileForAiToC(char * _pz_LogFile);
    void            DeleteSimplifiedLogFileForAi();

public:
    cl_Desdac();
    ~cl_Desdac();
    void            Init();
    void            ReadInfoFromIniFile();
    void            SaveInfoInIniFile();
    void            RunOneFrame();
    void            UpdateListOfAwareClients();
    void            UpdateListOfAvailableClients();

    void            ComputePasswordFromUser();

    void            SendUpdateEngineVersionRequest      (unsigned long _uiUpdateTypeFlags, int _iEnginePCVersion, int _iEnginePCPatch, int _iEnginePCCompilNb);
    void            SendUpdateEngineVersionRequest      (unsigned long _uiUpdateTypeFlags) { SendUpdateEngineVersionRequest(_uiUpdateTypeFlags, mi_EnginePCVersion, mi_EnginePCPatch, mi_EnginePCCompilNb);   }
 
    void            AddClientPC                         (char * _pz_ClientName);
    void            RemoveClientPC                      (int _iIndex);
    void            SetIsServer                         (bool _bServer)    { mb_Server = _bServer; }
    bool            bIsServer                           ()    { return mb_Server; }
    bool            bIsClientAvalaibleForRequest        (int _iIndex);
    bool            bIsYetiRunning                      ()   { return (mb_YetiPCisRunning || mb_CopyToXENONisRunning || mb_YetiXENONisRunning );  }
    bool            bIsYetiPCrunning                    ()   { return mb_YetiPCisRunning;       }
	bool            bIsVisualRunning                    ()   { return mb_VisualIsRunning;       }
	bool            bIsVssRunning						()   { return mb_VssIsRunning;       }
    bool            bIsYetiXENONrunning                 ()   { return mb_YetiXENONisRunning;    }
    bool            bIsXENONrebooting                   ()   { return mb_XENONisRebooting;      }
    bool            bIsCopyToXENONrunning               ()   { return mb_CopyToXENONisRunning;  }
    float           fCopyProgress                       ()   { return mf_CopyPercentProgress;   }
    const char*     szGetXenonKitName                   ()   { return msz_XenonKitName;         }
    
    STARTUPINFO	*	      pst_GetStartupInfo            () { return &mst_Async_StartUpInfo; }
    PROCESS_INFORMATION * pst_GetProcessInformation     () { return &mst_Async_ProcInfo; }
    ChangeListToLaunch *  pst_GetCurrentCl              () { return &mst_CurrentCl; }

    bool            bYetiPCHasCrashed                     () { return mb_YetiPCHasCrashed; }
    void            SetYetiPCHasCrashed                 (bool _bCrashed) { mb_YetiPCHasCrashed = _bCrashed; }

    bool            bGetSuspendedStatus                 ()   { return mb_IsClientSuspended; }
    void            Suspend                             (bool _bSuspend);
    int             iFindMostAppropriateClientForRequest(unsigned long _ul_Option);
    int             iGetNbClients                       ()         { return (int)mul_NbClients; }
    int             iGetNbBusyClients                   ();
    int             iGetNbNotLaunchedClients            ();
    int             iGetNbDownClients                   ()         { return mi_NbDownClients; }
    int             iGetNbAvailableClients              ()         { return mi_NbAvailableClients; }
    int             iGetNbSuspendedClients              ();
    int             iGetNbUpdatingClients               ();
    int             iGetNbOfPendingUpdateRequests       ();
    char *          pz_GetClient                        (int _iIndex);
    const char *    pz_GetClientStatus                  (int _iIndex);
    void            GetClientStatInfos                  (int _iIndex, char * _szInfos);
    char *          pz_GetPcServer                      ();
    void            SetPcServer                         (char * _pz_PcServer);
    int             iGetClInTopOfStack                  ();
    int             iGetLastClInStack                   ();
    int             iGetNbClOnStack                     ();

    void            AddMailToSend                       (char * _pz_Mail);
    void            RemoveMailToSend                    (int _iIndex);
    char *          pz_GetMailToSend                    (int _iIndex);
    int             iGetNbMailToSend                    ();

    bool            bSendLoggedMail                     (ChangeListResult* _pst_CL, char * _pz_User, char * _pz_Mail, char * _pz_Object, char * _pz_Body, char * _pz_FileToAttach);
    bool            bSendMail                           (char * _pz_User, char * _pz_Mail, char * _pz_Object, char * _pz_Body, char * _pz_FileToAttach);
    void            LogMail                             (bool _bSendOK, ChangeListResult* _pst_CL, char * _pz_User, char * _pz_Object, char * _pzDate );
    void            LogMailContent                      (char * _pz_User, char * _pz_Mail, char * _pz_Object, char * _pz_Body, char * _pz_FileToAttach, char * _pzDate );
    
    bool            bGetSendMails                       ()                  { return mb_SendMessages; }
    void            SetSendMails                        (bool _bSendMails)  { mb_SendMessages = _bSendMails; }
    bool            bGetSendMailsUser                   ()                  { return mb_SendMessageUser; }
    void            SetSendMailsUser                    (bool _bSendMails)  { mb_SendMessageUser = _bSendMails; }


    void            AddMapToTest                        (char * _pz_MapKey) { AddMapToTest(_pz_MapKey, kaz_DESDAC_DefaultMapName);  }
    void            AddMapToTest                        (char * _pz_MapToTest, char * _pz_MapKey);
    void            RemoveMapToTest                     (int _iIndex);
    bool            bGetMapTestingPC                    ()              {   return mb_MapTestingEnable;         }
    void            SetMapTesting                       (bool _bOnOff);
    bool            bGetTestLastClonly                  ()              {   return mb_TestLastCLonlyEnable;     }
    void            SetTestLastClonly                   (bool _bOnOff)  {   mb_TestLastCLonlyEnable = _bOnOff;  }
    int             iGetNbMapToTest                     ()              {   return mi_NbMapToTest;          }
    char *          pz_GetMapToTest                     (int _iIndex);
    void            ResetMapTestResults                 ();
    
	bool            bGetAiToCTesting                    ()              {   return mb_AiToCTestingEnable;         }
    void            SetAiToCTesting                     (bool _bOnOff);

    void            SetMapTestingXenon                  (bool _bOnOff);
    bool            bGetMapTestingXenon                 ()      {   return mb_MapTestingXenonEnable;     }
    bool            AnalyzeLogFileXenon_MapPerMapTest   (ChangeListToLaunch * _pst_CL, ChangeListResult* _pst_Results);

    bool            bGetGlobalMapTesting                ()      {   return ( bGetMapTestingPC() || bGetMapTestingXenon() );     }

    bool            AnalyzeLogFile                      (ChangeListToLaunch * _pst_CL, ChangeListResult* _pst_Results);
    void            TestCrashPC                         ();

    void            CreateResultFile                    (const ChangeListResult* _pst_Results);
    void            StoreAIBugToFile                    (const char* _pzMapKey, const char* sz_AIBug);

	char *			GetAiLog							(char* _pBuffer);
	char *			GetAiToCLog							(char* _pBuffer);
	void			RemoveBlanks						(char* _pBuffer);

    int             iGetLastClResult                    ()      {   return mst_LastClResult.mo_LaunchedCL.mi_ChangeList;      }
    bool            bGetLastClResultGlobalFailed        ();
    bool            bGetLastClResultAiFailed            ()      {   return mst_LastClResult.mb_AiFailed;        }
    bool            bGetLastClResultDbFailed            ()      {   return mst_LastClResult.mb_DbFailed;        }
    bool            bGetLastClResultMapTestFailed       ()      {   return mst_LastClResult.mb_MapTestFailed;   }
    char *          pz_GetLastClResultOwner             ()      {   return mst_LastClResult.mo_LaunchedCL.msz_UserName;       }

    int             iGetCurrentCl                       ();
    char *          pz_GetCurrentClOwner                ();

    void            FlushChangeListStack                ();

    int             iGetTimerDuration       ();
    void            SetTimerDuration        (int _iTime);

    int             iGetSyncDepth           () { return mi_SyncDepth; }
    void            SetSyncDepth            (int _iDepth) { mi_SyncDepth = _iDepth; }

    int             iGetYetiVersion         () { return mi_EnginePCVersion; }
    int             iGetYetiPatch           () { return mi_EnginePCPatch; }
    int             iGetYetiCompilNb        () { return mi_EnginePCCompilNb; }
    int             iGetYetiLastUpdateDay   () { return mo_EngineLastUpdateTime.GetDay();     }
    int             iGetYetiLastUpdateMonth () { return mo_EngineLastUpdateTime.GetMonth();   }
    int             iGetYetiLastUpdateYear  () { return mo_EngineLastUpdateTime.GetYear();    }
    int             iGetYetiLastUpdateHour  () { return mo_EngineLastUpdateTime.GetHour();    }
    int             iGetYetiLastUpdateMinute() { return mo_EngineLastUpdateTime.GetMinute();  }
    int             iGetYetiLastUpdateSecond() { return mo_EngineLastUpdateTime.GetSecond();  }

    void            SetYetiVersion          (int _iVersion) { mi_EnginePCVersion      = _iVersion;  }
    void            SetYetiPatch            (int _iPatch)   { mi_EnginePCPatch        = _iPatch;    }
    void            SetYetiCompilNb         (int _iCompilNb){ mi_EnginePCCompilNb     = _iCompilNb; }

    char *          pz_GetPerforceServer    ();
    char *          pz_GetPerforceUser      ();
    char *          pz_GetPassword          ();
    char *          pz_GetExeName           ();
    char *          pz_GetPerforceBranch    ();
    char *          pz_GetBigFileName       ();
	char *          pz_GetJadeSourcesPath   ();
    char *          pz_GetTempFileName      ();
    const char *    pz_GetOutputFolder      ();
    char *          pz_GetDefaultMail       ();
    char *          pz_GetSrvCompilerPath   ();
    const char*     pz_GetRemoteOutputPath  ();

    void            SetPerforceServer       (char * _pz_PerforceServer);
    void            SetPerforceUser         (char * _pz_PerforceUser);
    void            SetPassword             (char * _pz_Password);
    void            SetExeName              (char * _pz_ExeName);
    void            SetPerforceBranch       (char * _pz_PerforceBranch);
    void            SetBigFileName          (char * _pz_BigFileName);
	void			SetJadeSourcesPath		(char * _pz_JadeSolName);
    void            SetTempFileName         (char * _pz_TempFileName);
    void            SetOutputFolder         (char * _pz_OutputFolder);
    void            SetDefaultMail          (char * _pz_DefaultMail);
    void            SetSrvCompilerPath      (char * _pz_Path);
    bool            bGetChangeListSubmitTime(int _iCL, CTime& _rTime);

};



#endif//__DESDAC_H__