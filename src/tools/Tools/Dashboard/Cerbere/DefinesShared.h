#ifndef __DEFINESSHARED_H__
#define __DEFINESSHARED_H__


//////////////////////////////////////////////////////
// SHARED WITH DESDAC: must be coherent!!!!

// Watched files 
//It is strongly recommended for Watched Files to be located in the same, dedicated folder (lowers the number of messages to treat)
#define kaz_DESDAC_RemoteTagFile        "\\\\%s\\DESDAC\\SYS\\tag.txt"
#define kaz_DESDAC_RemoteIniFile        "\\\\%s\\DESDAC\\SYS\\desdac.ini"

// Other files
#define kaz_DESDAC_RemoteRequestFile    "\\\\%s\\DESDAC\\SYS\\request.txt"
#define kaz_DESDAC_RemoteUpdateFile     "\\\\%s\\DESDAC\\SYS\\update.txt"
#define kaz_DESDAC_RemoteSuspendedFile  "\\\\%s\\DESDAC\\SYS\\suspended.txt"

#define kaz_DESDAC_RemoteHtmlFile       "\\\\%s\\DESDAC\\Output\\desdac.htm"
#define kaz_DESDAC_RemoteHtmlMapFile    "\\\\%s\\DESDAC\\Output\\desdac_MapTest.htm"

// Status format
#define kaz_DESDAC_SystemStatusHeader   "SYS:"
const int ki_DESDAC_SystemStatusVersion = 1;

// Misc
const int ki_DESDAC_TimerValue          = 2000;     // timer used by desdac for tag file refresh. in MILLISEC!





//////////////////////////////////////////////////////
// INI FILE
#define kaz_True            "True"
#define kaz_False           "False"

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

#endif//__DEFINESSHARED_H__