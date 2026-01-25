#include "stdafx.h"
#include <winbase.h>
#include <io.h>
#include <afxsock.h>
#include <xbdm.h>
#include <process.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "DesDac.h"
#include "MAIN_DesdacEditorTags.h"
#include "MAIN_DesdacEngineTags.h"

#include "smtp.h"


int cl_Desdac::mi_StatMaxNbClReachedInStack = 0;
int cl_Desdac::mi_NbClTreatedOnClient = 0;
DWORD cl_Desdac::mul_BigfileSizeOnCLient = 0;


/////////////////////////////////////// 
bool cl_Desdac::bSendLoggedMail(ChangeListResult* _pst_CL, char * _pz_User, char * _pz_Mail, char * _pz_Object, char * _pz_Body, char * _pz_FileToAttach)
{
    // get current time
    char OsTime[16];
    char OsDate[16];
    char szDateAndTime[33];
    _strtime(OsTime);
    _strdate(OsDate);

    // send the mail
    bool bRes = bSendMail( _pz_User, _pz_Mail, _pz_Object, _pz_Body, _pz_FileToAttach);
    
    // log result
    sprintf( szDateAndTime, "%s %s", OsDate, OsTime );
    LogMail( bRes, _pst_CL, _pz_User, _pz_Object, szDateAndTime );

#ifdef DEBUG
    // log content
    LogMailContent( _pz_User, _pz_Mail, _pz_Object, _pz_Body, _pz_FileToAttach, szDateAndTime );
#endif

    return bRes;
}


bool cl_Desdac::bSendMail(char * _pz_User, char * _pz_Mail, char * _pz_Object, char * _pz_Body, char * _pz_FileToAttach)
{
    // Initialize sockets
    if (!AfxSocketInit())
    {
        TRACE(_T("Failed to initialise the Winsock stack\n"));
        return FALSE;
    }

    // Create the SMTP connection
    CSMTPConnection smtp;

    // Connect to the server
    if (!smtp.Connect(_T("10.255.100.5")))
    {
        CString sResponse = smtp.GetLastCommandResponse();
        TRACE(_T("Failed to connect to SMTP server\n"));
        return FALSE;
    }

    // Create the message
    CSMTPMessage m;
    CSMTPAddress From(_T("DESDAC_MTP"), _T("DESDAC_MTP@ubisoft.fr"));
	//CSMTPAddress From(_T("Mathieu Hidrio"), _T("mathieu.hidrio@ubisoft.com"));
    m.m_From = From;
    char sz_Recipient[1024];
    if ( _pz_User && _pz_Mail )
    {
        sprintf(sz_Recipient, "%s <%s>", _pz_User, _pz_Mail);
        
//#ifdef MATHIEU				
		//m.AddMultipleRecipients("mathieu.hidrio@ubisoft.com", CSMTPMessage::TO);		
		//m.AddMultipleRecipients("DESDAC_MTP@ubisoft.fr", CSMTPMessage::TO);
//#else
		if((strcmp(_pz_Object, "[DESDAC] Build Error.") == 0))	
		{
			m.AddMultipleRecipients("Programmation_RM4@ubisoft.fr", CSMTPMessage::TO);	
			m.AddMultipleRecipients(_T("DESDAC_MTP@ubisoft.fr"), CSMTPMessage::CC);
		}
		else
		{
			m.AddMultipleRecipients(_T(sz_Recipient), CSMTPMessage::TO);
			m.AddMultipleRecipients(_T("DESDAC_MTP@ubisoft.fr"), CSMTPMessage::CC);
		}
//#endif MATHIEU
		//
    }
	
#ifndef MATHIEU	
    for ( int i = 0; i < mi_NbMailsToSend; ++i )
    {
        char sz_User[1024];
        sprintf(sz_User, "%s", pz_GetMailToSend(i));
        char * pTmp = strchr(sz_User, '@');
        if ( pTmp )
            *pTmp = 0x00;
        sprintf(sz_Recipient, "%s <%s>", sz_User, pz_GetMailToSend(i));
        m.AddMultipleRecipients(_T(sz_Recipient), CSMTPMessage::CC);
    }
#endif	

    m.m_sSubject = _pz_Object;
    CString oBuffer = CString(_pz_Body);
    m.AddBody(oBuffer);

    CSMTPAttachment oAttach;
    if ( _pz_FileToAttach )
    {
        // try to attach a file
        oAttach.Attach(CString(_pz_FileToAttach));
        oAttach.SetTitle(CString(_pz_FileToAttach));
        m.AddAttachment(&oAttach);
    }

    // Send the message
    if (!smtp.SendMessage(m))
    {
        CString sResponse = smtp.GetLastCommandResponse();
        TRACE(_T("Failed to send the SMTP message\n"));
        return FALSE;
    }

    // Disconnect from the server
    smtp.Disconnect();

    return TRUE;
}


// logs sent mails into a file
void cl_Desdac::LogMail( bool _bSendOK, ChangeListResult* _pst_CL, char * _pz_User, char * _pz_Object, char * _pzDate )
{
    char sz_LogFile[BIG_k_NameLength];
    sprintf(sz_LogFile, "%s\\%s", msz_OutputFolder, kaz_DESDAC_MailLog);

    FILE * pLog = fopen(sz_LogFile, "a+");
    if ( pLog )
    {
        if(_pst_CL)
        {
            fprintf(pLog,       "  CL : %6d",  _pst_CL->mo_LaunchedCL.mi_ChangeList );
            fprintf(pLog,       "  user:%15s", _pst_CL->mo_LaunchedCL.msz_UserName );

            if( _pst_CL->mb_AiFailed )
            {
                fprintf(pLog,   "  AI Error (%d)",  _pst_CL->mi_NbAiErrors );
            }

            if( _pst_CL->mb_DbFailed )
            {
                fprintf(pLog,   "  DB Error (%d)",  _pst_CL->mi_NbDbErrors );
            }

            if( _pst_CL->mb_MapTestFailed )
            {
                fprintf(pLog,   "  MAP Error(%d)",  _pst_CL->mi_NbMapErrors );
            }

            if( _pst_CL->mb_Crashed )
            {
                fprintf(pLog,   "  CRASH       " );
            }
        }
        else
        {
            fprintf(pLog, _pz_Object);
        }

        fprintf(pLog,  "  (%s - ", _pzDate );
        fprintf(pLog, _bSendOK ? "  Send   OK   )" : "  Send Failed )" );
        //fprintf(pLog, "   object: ");
        //fprintf(pLog, _pz_Object);

        // end line
        fprintf(pLog,  "\n" );

        // close file
        fclose(pLog);
    }
}


// log content of mails sent in a file.
void cl_Desdac::LogMailContent(char * _pz_User, char * _pz_Mail, char * _pz_Object, char * _pz_Body, char * _pz_FileToAttach, char * _pzDate)
{
    // append to log file
    char sz_LogFile[BIG_k_NameLength];
    sprintf(sz_LogFile, "%s\\%s", msz_OutputFolder, kaz_DESDAC_MailLogContent);

    FILE * pLog = fopen(sz_LogFile, "a+");
    if ( pLog )
    {
        fprintf( pLog, "\n\n\n------------------------------------------------------------------------------\n" );
        fprintf( pLog,       "----      MAIL      ----------------------------------------------------------\n" );

        if (_pzDate)                fprintf( pLog, "- Date   : %s\n", _pzDate );
        if (_pz_User && _pz_Mail)   fprintf( pLog, "- Mail To: %s (%s)\n", _pz_User, _pz_Mail );
        if (_pz_FileToAttach)       fprintf( pLog, "- Attach : %s\n", _pz_FileToAttach );
        if (_pz_Object)             fprintf( pLog, "- Object : %s\n", _pz_Object );

        if (_pz_Body)   
        {
            fprintf( pLog, "\n");
            fprintf( pLog, _pz_Body);
        }

        // close file
        fclose(pLog);
        flushall();
    }
}


//////////////////////////////////////////////////////////
cl_Desdac::cl_Desdac()
{
    Init();
}

cl_Desdac::~cl_Desdac()
{
    CleanFiles();
    SaveInfoInIniFile();
}

int cl_Desdac::iGetTimerDuration()
{
    return mi_TimerDuration;
}

void cl_Desdac::SetTimerDuration(int _iTime)
{
    mi_TimerDuration = _iTime;
}

void cl_Desdac::Init()
{
    // Default init values
    mb_FirstLaunch = true;

    mb_IsClientSuspended    = false;
    mb_IsServerSuspended    = false;
    mul_NbClients           = 0;
    mi_NbClInStack          = 0;
    mi_NbAvailableClients   = 0;
    mb_YetiPCisRunning      = false;
	mb_VisualIsRunning      = false;
	mb_VssIsRunning		    = false;
    mb_YetiXENONisRunning   = false;
    mb_XENONisRebooting     = false;
    mb_CopyToXENONisRunning = false;
    mb_YetiPCHasCrashed     = false;
    mi_TimerDuration        = 2;
    mi_SyncDepth            = 3;
    mi_NbMailsToSend        = 0;

    mi_NbClResultInStack = 0;
    mst_LastClResult.Reset();
    mst_LastClResult.mb_ResultValid = true;
    mi_StackThresholdWarning = ki_DefaultStackThresholdWarning;

    mb_TestLastCLonlyEnable = false;
    mb_CheckPerforceEnable  = true;
    mb_CompileAiEnable      = true;

    mi_NbMapToTest = ki_DESDAC_DefaultNbMapToTest;
    mb_MapTestingEnable         = false;
    mb_MapTestingXenonEnable    = false;
    mb_XenonDVDEmuMode          = false;
	mb_AiToCTestingEnable		= false;
    mul_RebootTimeXenon = kul_DESDAC_DefaultXenonRebootTime;
    mf_MapTestTimeXenon = kf_DESDAC_DefaultXenonMapTestTime;

    mb_SendMessages = true;


    // Read settings values in ini file
    ReadInfoFromIniFile();


    // Get the name of the current computer
    DWORD dwNameLength = BIG_k_NameLength;
    GetComputerName(msz_ComputerName, &dwNameLength);

    // ensure that the needed folders exist
    char sz_BigFilePath[BIG_k_NameLength];
    ExtractPathFromFullName( sz_BigFilePath, msz_BigFileName );
    CreateDirectory(sz_BigFilePath,     NULL);
    CreateDirectory(msz_OutputFolder,   NULL);
    CreateDirectory(kaz_FolderSystem,   NULL);
    CreateDirectory(kaz_FolderResults,  NULL);
    CreateDirectory(kaz_FolderVersion,  NULL);

    // reset tag files
    CleanFilesAtStart();
    
    // update status
    UpdateListOfAwareClients();
    UpdateListOfAvailableClients();
}


void cl_Desdac::CleanFiles()
{
    DeleteFile(msz_DesdacTempFile);
    DeleteFile(kaz_DESDAC_RequestFile);
    DeleteFile(kaz_DESDAC_RequestUpdateFile);
    DeleteFile(kaz_DESDAC_ResultsFile);
    DeleteFile(kaz_DESDAC_SuspendedFile);
    DeleteFile(kaz_DESDAC_CrashDumpFile);
    DeleteFile(kaz_DESDAC_AIBugsFile);

    if( ! DeleteFile(kaz_DESDAC_TagFile) )
    {   
        DWORD uwErr = GetLastError();
        char szErr[1024];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, uwErr ,LANG_SYSTEM_DEFAULT ,szErr, 1024, NULL);
        TRACE(szErr);
    }
}

void cl_Desdac::CleanFilesAtStart()
{
    CleanFiles();

    // Create TAG file if necessary
    FILE * pTag = fopen(kaz_DESDAC_TagFile, "rt");
    if ( !pTag )
    {
        pTag = fopen(kaz_DESDAC_TagFile, "wt");
        fprintf(pTag, "\n");
        fclose(pTag);
        flushall();
    }
}

void cl_Desdac::ReadInfoFromIniFile()
{
    // Client / Server
    mb_Server = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_ServerOrClient, 0, kaz_DESDAC_IniFile) ? true : false;
    int iNbClients = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_NbClients, 0, kaz_DESDAC_IniFile);

    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_PcServer, kaz_DESDAC_DefaultPcServer, msz_PcServerName, BIG_k_NameLength, kaz_DESDAC_IniFile);

    // list of clients
    mul_NbClients = 0;  // reset list. It will be incremented by "add..." method
    for ( int i = 0; i < iNbClients; ++i )
    {
        char sz_Tmp[BIG_k_NameLength];
        char sz_Client[BIG_k_NameLength];
        sprintf(sz_Client, kaz_DESDAC_PcClient, i);
        GetPrivateProfileString(kaz_DESDAC_Section, sz_Client, kaz_DESDAC_DefaultPcClient, sz_Tmp, BIG_k_NameLength, kaz_DESDAC_IniFile);
        AddClientPC(sz_Tmp);
    }

    // Mails to send
    mi_NbMailsToSend = 0;  // reset list. It will be incremented by "add..." method
    int iNbMailToSend = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_NbMailToSend, 0, kaz_DESDAC_IniFile);
    for ( int i = 0; i < iNbMailToSend; ++i )
    {
        char sz_Tmp[BIG_k_NameLength];
        char sz_Mail[BIG_k_NameLength];
        sprintf(sz_Mail, kaz_DESDAC_MailToSend"%d", i);
        GetPrivateProfileString(kaz_DESDAC_Section, sz_Mail, kaz_DESDAC_DefaultMailToSend, sz_Tmp, BIG_k_NameLength, kaz_DESDAC_IniFile);
        AddMailToSend(sz_Tmp);
    }
    mb_SendMessages    = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_SendMessages    , 1, kaz_DESDAC_IniFile) ? true : false;
    mb_SendMessageUser = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_SendMessagesUser, 1, kaz_DESDAC_IniFile) ? true : false;

    // Perforce
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_ProjectName, kaz_DESDAC_DefaultProjectName, msz_ProjectName, BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_PerforceServer, kaz_DESDAC_DefaultPerforceServer, msz_PerforceServer, BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_PerforceUser, kaz_DESDAC_DefaultPerforceUser, msz_PerforceUser, BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_Password, kaz_DESDAC_DefaultPassword, msz_ClientPassword, BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_DepotBranch, kaz_DESDAC_DefaultDepotBranch, msz_DepotBranch, BIG_k_NameLength, kaz_DESDAC_IniFile);
    mi_SyncDepth  = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_SyncDepth, 2, kaz_DESDAC_IniFile);

    // Jade
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_BigFileName, kaz_DESDAC_DefaultBigFileName, msz_BigFileName, BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_ExeName, kaz_DESDAC_DefaultExeName, msz_ExeName, BIG_k_NameLength, kaz_DESDAC_IniFile);
	GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_JadeSourcesPath, kaz_DESDAC_JadeSourcesPath, msz_JadeSourcesPath, BIG_k_NameLength, kaz_DESDAC_IniFile);

    // Internal
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_TempFileName, kaz_DESDAC_DefaultTempFileName, msz_DesdacTempFile, BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_OutputFolder, kaz_DESDAC_DefaultOutputFolder, msz_OutputFolder, BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_DefaultMail, kaz_DESDAC_DefaultDefaultMail, msz_DefaultMail, BIG_k_NameLength, kaz_DESDAC_IniFile);
    mi_StackThresholdWarning  = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_StackThreshold, ki_DefaultStackThresholdWarning, kaz_DESDAC_IniFile);

    // Engine Update: Remote path, Version, and last update time
    mi_EnginePCVersion = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_Version, ki_DESDAC_DefaultVersion,     kaz_DESDAC_IniFile);
    mi_EnginePCPatch   = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_Patch,   ki_DESDAC_DefaultPatch,       kaz_DESDAC_IniFile);
    mi_EnginePCCompilNb= GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_CompilNb,ki_DESDAC_DefaultCompilNb,    kaz_DESDAC_IniFile);

    char sz_LastUpdate[64];
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_LastUpdate, kaz_DESDAC_DefaultLastUpdate, sz_LastUpdate, 64, kaz_DESDAC_IniFile);
    int iYear, iMonth, iDay, iHour, iMinute, iSecond;
    sscanf(sz_LastUpdate, "%d/%d/%d %d:%d:%d", &iDay, &iMonth, &iYear, &iHour, &iMinute, &iSecond);
    if(  (1970<=iYear) && (iYear<=3000) && (1<=iMonth) && (iMonth<=12) && (1<=iDay) && (iDay<=31)  )
    {   mo_EngineLastUpdateTime = CTime(iYear, iMonth, iDay, iHour, iMinute, iSecond);
    }

    // Engine Update: Remote paths to use to get last versions
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_EnginePCUpdatePath, kaz_DESDAC_DefaultEnginePCUpdatePath, msz_EnginePCUpdatePath, BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_EngineXenonUpdatePath, kaz_DESDAC_DefaultEngineXenonUpdatePath, msz_EngineXenonUpdatePath, BIG_k_NameLength, kaz_DESDAC_IniFile);


    //////////////
    // Map testing
    char sz_TmpTest[64];    
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_MapTestingEnable, kaz_DESDAC_DefaultMapTesting, sz_TmpTest, 64, kaz_DESDAC_IniFile);
    mb_MapTestingEnable = ( 0 == stricmp(sz_TmpTest, kaz_True) );

    mi_NbMapToTest = 0;  // reset list. It will be incremented by "add..." method
    int iNbMapToTest = GetPrivateProfileInt(kaz_DESDAC_Section, kaz_DESDAC_NbMapToTest, ki_DESDAC_DefaultNbMapToTest, kaz_DESDAC_IniFile);
    for ( int i = 0; i < iNbMapToTest; ++i )
    {
        char sz_MapName[BIG_k_NameLength];
        char sz_MapNameEntry[BIG_k_NameLength];
        sprintf(sz_MapNameEntry, kaz_DESDAC_MapName_NB, i);
        GetPrivateProfileString(kaz_DESDAC_Section, sz_MapNameEntry, kaz_DESDAC_DefaultMapName, sz_MapName, BIG_k_NameLength, kaz_DESDAC_IniFile);

        char sz_MapKey[BIG_k_NameLength];
        char sz_MapKeyEntry[BIG_k_NameLength];
        sprintf(sz_MapKeyEntry, kaz_DESDAC_MapKey_NB, i);
        GetPrivateProfileString(kaz_DESDAC_Section, sz_MapKeyEntry, kaz_DESDAC_DefaultMapName, sz_MapKey, BIG_k_NameLength, kaz_DESDAC_IniFile);

        AddMapToTest(sz_MapKey, sz_MapName);
    }


    //////////////
    // Map testing Xenon
    GetPrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_MapTestingXenonEnable,   kaz_DESDAC_DefaultMapTesting,       sz_TmpTest,             64,               kaz_DESDAC_IniFile);
    mb_MapTestingXenonEnable = ( !strcmp(sz_TmpTest, kaz_True) );

    GetPrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonKitName,            kaz_DESDAC_DefaultXenonKitName,     msz_XenonKitName,       BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonBigFileName,        kaz_DESDAC_DefaultXenonBigFileName, msz_XenonBigfileName,   BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonExeName,            kaz_DESDAC_DefaultXenonExeName,     msz_XenonExeName,       BIG_k_NameLength, kaz_DESDAC_IniFile);
    GetPrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonLauncherName,       kaz_DESDAC_DefaultXenonLauncherName,msz_XenonLauncherName,  BIG_k_NameLength, kaz_DESDAC_IniFile);

    mul_RebootTimeXenon = (DWORD)GetPrivateProfileInt(kaz_DESDACXenon_Section, kaz_DESDAC_XenonRebootTime, kul_DESDAC_DefaultXenonRebootTime, kaz_DESDAC_IniFile);

    char sz_Tmp[BIG_k_NameLength];
    GetPrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonMapTestTime, "", sz_Tmp,  BIG_k_NameLength, kaz_DESDAC_IniFile);
    if( sz_Tmp[0] != 0 )
    {   mf_MapTestTimeXenon = (float)atof(sz_Tmp);
    }


    // misc
    GetPrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_TestLastCLonlyEnable, ( (mb_MapTestingEnable || mb_MapTestingXenonEnable) ? kaz_True : kaz_False ), sz_TmpTest, 64, kaz_DESDAC_IniFile);
    mb_TestLastCLonlyEnable = ( 0 == stricmp(sz_TmpTest, kaz_True) );


    ///////////////
    // Init endings
    ExtractPathFromFullName(msz_XenonLauncherPath, msz_XenonLauncherName);
    ExtractPathFromFullName(msz_ExeAndLogPath, msz_ExeName);
    if ( strcmp(msz_ClientPassword, kaz_DESDAC_DefaultPassword) == 0 )
        ComputePasswordFromUser();
}


void cl_Desdac::SaveInfoInIniFile()
{
    char sz_Tmp[BIG_k_NameLength];

    sprintf(sz_Tmp, "%d", bIsServer() ? 1 : 0);
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_ServerOrClient, sz_Tmp, kaz_DESDAC_IniFile);
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_PcServer, msz_PcServerName, kaz_DESDAC_IniFile);

    if( bIsServer() )
    {
        sprintf(sz_Tmp, "%d", mul_NbClients);
        WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_NbClients, sz_Tmp, kaz_DESDAC_IniFile);
        for ( unsigned long i = 0; i < mul_NbClients; ++i )
        {
            char sz_Tmp[BIG_k_NameLength];
            char sz_Client[BIG_k_NameLength];
            sprintf(sz_Client, kaz_DESDAC_PcClient, i);
            sprintf(sz_Tmp, "%s", mast_ClientPCs[i].msz_ClientName);
            WritePrivateProfileString(kaz_DESDAC_Section, sz_Client, sz_Tmp, kaz_DESDAC_IniFile);
        }
    }

    ///////////
    // Perforce 
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_ProjectName,       msz_ProjectName,    kaz_DESDAC_IniFile);
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_PerforceServer,    msz_PerforceServer, kaz_DESDAC_IniFile);
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_PerforceUser,      msz_PerforceUser,   kaz_DESDAC_IniFile);
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_DepotBranch,       msz_DepotBranch,    kaz_DESDAC_IniFile);
    sprintf(sz_Tmp, "%d", mi_SyncDepth);
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_SyncDepth, sz_Tmp, kaz_DESDAC_IniFile);

    ///////
    // Jade
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_BigFileName, msz_BigFileName, kaz_DESDAC_IniFile);
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_ExeName, msz_ExeName, kaz_DESDAC_IniFile);
	WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_JadeSourcesPath, msz_JadeSourcesPath, kaz_DESDAC_IniFile);

    ///////////
    // Internal
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_TempFileName, msz_DesdacTempFile, kaz_DESDAC_IniFile);

    if( bIsServer() )
    {
        WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_OutputFolder, msz_OutputFolder, kaz_DESDAC_IniFile);
        WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_DefaultMail, msz_DefaultMail, kaz_DESDAC_IniFile);
        sprintf(sz_Tmp, "%d", mi_StackThresholdWarning);
        WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_StackThreshold, sz_Tmp, kaz_DESDAC_IniFile);

        sprintf(sz_Tmp, "%d", mi_NbMailsToSend);
        WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_NbMailToSend, sz_Tmp, kaz_DESDAC_IniFile);
        for ( int i = 0; i < mi_NbMailsToSend; ++i )
        {
            char sz_Tmp[BIG_k_NameLength];
            char sz_Mail[BIG_k_NameLength];
            sprintf(sz_Mail, kaz_DESDAC_MailToSend"%d", i);
            sprintf(sz_Tmp, "%s", masz_MailsToSend[i]);
            WritePrivateProfileString(kaz_DESDAC_Section, sz_Mail, sz_Tmp, kaz_DESDAC_IniFile);
        }
        sprintf(sz_Tmp, "%d", (int)mb_SendMessages);
        WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_SendMessages, sz_Tmp, kaz_DESDAC_IniFile);
        sprintf(sz_Tmp, "%d", (int)mb_SendMessageUser);
        WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_SendMessagesUser, sz_Tmp, kaz_DESDAC_IniFile);
    }

    /////////////////////////////////
    // Version, patch and last update
    sprintf(sz_Tmp, "%d", mi_EnginePCVersion);
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_Version,   sz_Tmp, kaz_DESDAC_IniFile);
    sprintf(sz_Tmp, "%d", mi_EnginePCPatch);
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_Patch,     sz_Tmp, kaz_DESDAC_IniFile);
    sprintf(sz_Tmp, "%d", mi_EnginePCCompilNb);
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_CompilNb,  sz_Tmp, kaz_DESDAC_IniFile);
    
    char sz_LastUpdate[64];
    CTime& roT = mo_EngineLastUpdateTime;
    sprintf(sz_LastUpdate, "%02d/%02d/%04d %02d:%02d:%02d", roT.GetDay(),  roT.GetMonth(),  roT.GetYear(),
                                                            roT.GetHour(), roT.GetMinute(), roT.GetSecond()   );

    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_LastUpdate, sz_LastUpdate, kaz_DESDAC_IniFile);

    if( bIsServer() )
    {   
        WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_EnginePCUpdatePath, msz_EnginePCUpdatePath, kaz_DESDAC_IniFile);
        WritePrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_EngineXenonUpdatePath, msz_EngineXenonUpdatePath, kaz_DESDAC_IniFile);
    }


    //////////////
    // Map testing
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_MapTestingEnable, mb_MapTestingEnable ? kaz_True : kaz_False, kaz_DESDAC_IniFile);
    
    if( mb_MapTestingEnable )
    {
        sprintf(sz_Tmp, "%d", mi_NbMapToTest);
        WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_NbMapToTest, sz_Tmp, kaz_DESDAC_IniFile);

        for ( int i = 0; i < mi_NbMapToTest; ++i )
        {
            char sz_Entry[BIG_k_NameLength];
            char sz_Tmp[BIG_k_NameLength];
            sprintf(sz_Entry, kaz_DESDAC_MapName_NB, i);
            sprintf(sz_Tmp, "%s", mast_MapToTest[i].msz_MapName);
            WritePrivateProfileString(kaz_DESDAC_Section, sz_Entry, sz_Tmp, kaz_DESDAC_IniFile);
            sprintf(sz_Entry, kaz_DESDAC_MapKey_NB, i);
            sprintf(sz_Tmp, "%s", mast_MapToTest[i].msz_MapKey);
            WritePrivateProfileString(kaz_DESDAC_Section, sz_Entry, sz_Tmp, kaz_DESDAC_IniFile);
        }
    }


    //////////////
    // Map testing Xenon
    WritePrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_MapTestingXenonEnable,    mb_MapTestingXenonEnable ? kaz_True : kaz_False, kaz_DESDAC_IniFile);

    if( mb_MapTestingXenonEnable )
    {
        WritePrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonKitName,             msz_XenonKitName,       kaz_DESDAC_IniFile);
        WritePrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonBigFileName,         msz_XenonBigfileName,   kaz_DESDAC_IniFile);
        WritePrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonExeName,             msz_XenonExeName,       kaz_DESDAC_IniFile);
        WritePrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonLauncherName,        msz_XenonLauncherName,  kaz_DESDAC_IniFile);

        sprintf(sz_Tmp, "%d", mul_RebootTimeXenon);
        WritePrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonRebootTime,  sz_Tmp, kaz_DESDAC_IniFile);

        sprintf(sz_Tmp, "%.1f", mf_MapTestTimeXenon);
        WritePrivateProfileString(kaz_DESDACXenon_Section, kaz_DESDAC_XenonMapTestTime, sz_Tmp, kaz_DESDAC_IniFile);
    }


    // Misc
    WritePrivateProfileString(kaz_DESDAC_Section, kaz_DESDAC_TestLastCLonlyEnable, mb_TestLastCLonlyEnable ? kaz_True : kaz_False, kaz_DESDAC_IniFile);
}

void cl_Desdac::PushLaunchedChangeListToResult(const ChangeListToLaunch * _pst_ChangeList)
{
    // test if stack is full
    if ( mi_NbClResultInStack >= ki_MaxChangeListInStack )
        return; // Can't do nothing, CL result will be lost.

    // copy values to results stack
    ChangeListResult& rCLres = mast_ClResultStack[mi_NbClResultInStack];
    rCLres.mo_LaunchedCL  = *_pst_ChangeList;

    // init result values
    rCLres.mb_ResultValid = false;
    rCLres.mb_Crashed     = false;
    rCLres.mi_ClientIndex = -1;


    mi_NbClResultInStack++;
}

bool cl_Desdac::bCanPopChangeListResult()
{
    if ( mi_NbClResultInStack == 0 )
        return false;

    return mast_ClResultStack[0].mb_ResultValid;
}

void cl_Desdac::PopChangeListResult(ChangeListResult * _pst_ChangeList)
{
    // Under optimal, really...

    memcpy(_pst_ChangeList, &mast_ClResultStack[0], sizeof(ChangeListResult));

    for ( int i = 0; i < mi_NbClResultInStack-1; ++i )
    {
        memcpy(&mast_ClResultStack[i], &mast_ClResultStack[i+1], sizeof(ChangeListResult));
    }
    mi_NbClResultInStack--;
}

void cl_Desdac::SetchangeListResultValue(ChangeListResult& _rCLres)
{
    for ( int i = 0; i < mi_NbClResultInStack; ++i )
    {
        if ( mast_ClResultStack[i].mo_LaunchedCL.mi_ChangeList == _rCLres.mo_LaunchedCL.mi_ChangeList )
        {
            _rCLres.mb_ResultValid = true;
            mast_ClResultStack[i] = _rCLres;
        }
    }
}

void cl_Desdac::CreateSimplifiedLogFileForAiToC(char * _pz_LogFile)
{
	char* sol;
    char sep[] = "\n";
    char* tok;

    FILE * pLog = fopen(_pz_LogFile, "rt");
    if ( !pLog )
        return;
    FILE * pSimpleLog = fopen(kaz_DESDAC_SimplifiedLogFile, "w+");
    if ( !pSimpleLog )
    {
        fclose(pLog);
        return;
    }

    int iFile = _fileno(pLog);
    int iFileSize = _filelength(iFile);
    char * pBuffer = (char*)malloc(iFileSize);
    if ( !pBuffer )
    {
        fclose(pLog);
        fclose(pSimpleLog);
        return;
    }

    fread(pBuffer, 1, iFileSize, pLog);
    fclose(pLog);
    flushall();

    char * pStart = strstr(pBuffer, "[AI compilation start]");
    if ( pStart )
    {
        pStart += strlen("[AI compilation start]");
        char * pEnd = NULL;
        if ( pStart )
		{
            pStart = strstr(pStart, "[AI compilation start]");
			if ( pStart )
			{
				pStart += strlen("[AI compilation start]\n");
				tok = strtok(pStart, sep);
				while(tok != NULL)
				{
					sol = strstr(tok, "Error detected -- [WAR");
					if(!sol)
						break;
					tok = strtok(NULL, sep);
				}
			}
		}
    }

    fprintf(pSimpleLog, "=======================================================\n");
    fprintf(pSimpleLog, "* Erreur trouvée lors de la génération des IA en C\n");
    fprintf(pSimpleLog, "=======================================================\n");
	fprintf(pSimpleLog, tok);
    //fwrite(pStart, 1, strlen(tok), pSimpleLog);

    free(pBuffer);
    fclose(pSimpleLog);
    flushall();	
}


void cl_Desdac::CreateSimplifiedLogFileForAi(char * _pz_LogFile)
{
	char* sol;
    char sep[] = "\n";
    char* tok;

    FILE * pLog = fopen(_pz_LogFile, "rt");
    if ( !pLog )
        return;
    FILE * pSimpleLog = fopen(kaz_DESDAC_SimplifiedLogFile, "w+");
    if ( !pSimpleLog )
    {
        fclose(pLog);
        return;
    }

    int iFile = _fileno(pLog);
    int iFileSize = _filelength(iFile);
    char * pBuffer = (char*)malloc(iFileSize);
    if ( !pBuffer )
    {
        fclose(pLog);
        fclose(pSimpleLog);
        return;
    }

    fread(pBuffer, 1, iFileSize, pLog);
    fclose(pLog);
    flushall();

    char * pStart = strstr(pBuffer, "[AI compilation start]");
    if ( pStart )
    {
        pStart += strlen("[AI compilation start]");
        char * pEnd = NULL;
        if ( pStart )
		{
			tok = strtok(pStart, sep);
			while(tok != NULL)
			{
				sol = strstr(tok, "Error detected");
				if(sol)
				{
					sol = (char*)malloc(strlen(tok));
					strcpy(sol, tok);
					break;
				}
				tok = strtok(NULL, sep);
			}			
		}
    }

    fprintf(pSimpleLog, "=======================================================\n");
    fprintf(pSimpleLog, "* Erreur trouvée lors de la compilation des IA\n");
    fprintf(pSimpleLog, "=======================================================\n");
	fprintf(pSimpleLog, tok);
    //fwrite(pStart, 1, strlen(tok), pSimpleLog);

    free(pBuffer);
    fclose(pSimpleLog);
    flushall();	

/*
    FILE * pLog = fopen(_pz_LogFile, "rt");
    if ( !pLog )
        return;
    FILE * pSimpleLog = fopen(kaz_DESDAC_SimplifiedLogFile, "w+");
    if ( !pSimpleLog )
    {
        fclose(pLog);
        return;
    }

    int iFile = _fileno(pLog);
    int iFileSize = _filelength(iFile);
    char * pBuffer = (char*)malloc(iFileSize);
    if ( !pBuffer )
    {
        fclose(pLog);
        fclose(pSimpleLog);
        return;
    }

    fread(pBuffer, 1, iFileSize, pLog);
    fclose(pLog);
    flushall();

    char * pStart = strstr(pBuffer, "[AI compilation start]");
    if ( pStart )
    {
        pStart += strlen("[AI compilation start]");
        char * pEnd = NULL;
        if ( pStart )
            pEnd = strstr(pStart, "[AI compilation failure]");
        if ( pEnd )
        {
            fprintf(pSimpleLog, "===================================\n");
            fprintf(pSimpleLog, "* AI Errors found after compilation\n");
            fprintf(pSimpleLog, "===================================\n");
            fwrite(pStart, 1, pEnd-pStart, pSimpleLog);
        }
    }
    free(pBuffer);
    fclose(pSimpleLog);
    flushall();
	*/
}

void cl_Desdac::DeleteSimplifiedLogFileForAi()
{
    DeleteFile(kaz_DESDAC_SimplifiedLogFile);
}



void cl_Desdac::UpdateChangeListResultStack()
{
    while ( bCanPopChangeListResult() )
    {
        ChangeListResult st_Tmp;
        PopChangeListResult(&st_Tmp);

        // Test if user must be warned
        // AI
        bool bNewError = false;
        bool bWarnUserForAi = false;
        if ( st_Tmp.mb_AiFailed && (!mst_LastClResult.mb_AiFailed || (strcmp(st_Tmp.msz_AiLog, mst_LastClResult.msz_AiLog) != 0)) )
        {
            if ( mst_LastClResult.mb_AiFailed )
                bNewError = true;
            bWarnUserForAi = true;
        }

		// Build Prev        
        bool bWarnUserForBuildPrev = false;
        if ( st_Tmp.mb_BuildPrevFailed && (!mst_LastClResult.mb_BuildPrevFailed || (mst_LastClResult.mi_NbBuildPrevErrors < st_Tmp.mi_NbBuildPrevErrors)) )
        {
            if ( mst_LastClResult.mb_BuildPrevFailed )
                bNewError = true;
            bWarnUserForBuildPrev = true;
        }
		
		// AI to C generation
        bool bWarnUserForAiToC = false;
        if ( st_Tmp.mb_AiToCFailed && (!mst_LastClResult.mb_AiToCFailed || (strcmp(st_Tmp.msz_AiToCLog, mst_LastClResult.msz_AiToCLog) != 0)) )
        {
            if ( mst_LastClResult.mb_AiToCFailed )
                bNewError = true;
            bWarnUserForAiToC = true;
        }

		// Build       
        bool bWarnUserForBuild = false;
        if ( st_Tmp.mb_BuildFailed && (!mst_LastClResult.mb_BuildFailed || (mst_LastClResult.mi_NbBuildErrors < st_Tmp.mi_NbBuildErrors)) )
        {
            if ( mst_LastClResult.mb_BuildFailed )
                bNewError = true;
            bWarnUserForBuild = true;
        }

        // DataBase
        bool bWarnUserForDb = false;
        if ( st_Tmp.mb_DbFailed && (!mst_LastClResult.mb_DbFailed || (mst_LastClResult.mi_NbDbErrors < st_Tmp.mi_NbDbErrors)) )
        {   bWarnUserForDb = true;
        }

        // MapTest
        bool bWarnUserForTestMap = false;
        if ( st_Tmp.mb_MapTestFailed && (!mst_LastClResult.mb_MapTestFailed || (mst_LastClResult.mi_NbMapErrors < st_Tmp.mi_NbMapErrors)) )
        {   bWarnUserForTestMap = true;
        }


        // Warn the user if needed
        if ( mb_SendMessages && mst_LastClResult.mb_ResultValid )
        {
            char sz_HtmlFile[BIG_k_NameLength];
            sprintf(sz_HtmlFile, "%s\\%s", msz_OutputFolder, kaz_DESDAC_Html);

            if( bGetGlobalMapTesting() )
            {   // map testing mode : only warn of map problems
                if ( bWarnUserForTestMap )
                {
                    SendMapTestWarning(st_Tmp);
                    TagMailedCL(st_Tmp.mo_LaunchedCL.mi_ChangeList, sz_HtmlFile);

                    // also tag detailed report
                    sprintf(sz_HtmlFile, "%s\\%s", msz_OutputFolder, kaz_DESDAC_HtmlMapTest);
                    TagMailedCL(st_Tmp.mo_LaunchedCL.mi_ChangeList, sz_HtmlFile);
                }
            }
            else
            {
                // CRASH Warning 
                if ( st_Tmp.mb_Crashed )
                {
                    SendCrashWarning(st_Tmp);
                    TagMailedCL(st_Tmp.mo_LaunchedCL.mi_ChangeList, sz_HtmlFile);
                }

                // AI Warning 
                if ( bWarnUserForAi )
                {
                    SendAIWarning(st_Tmp, bNewError);
                    TagMailedCL(st_Tmp.mo_LaunchedCL.mi_ChangeList, sz_HtmlFile);
                }

				if(bWarnUserForBuildPrev)
                {
                    SendBuildPrevWarning(st_Tmp, bNewError);
                    TagMailedCL(st_Tmp.mo_LaunchedCL.mi_ChangeList, sz_HtmlFile);
                }

                // AI to C Warning 
                if ( bWarnUserForAiToC )
                {
                    SendAiToCWarning(st_Tmp, bNewError);
                    TagMailedCL(st_Tmp.mo_LaunchedCL.mi_ChangeList, sz_HtmlFile);
                }

				if(bWarnUserForBuild)
                {
                    SendBuildWarning(st_Tmp, bNewError);
                    TagMailedCL(st_Tmp.mo_LaunchedCL.mi_ChangeList, sz_HtmlFile);
                }

                // DB Warning 
                if ( bWarnUserForDb )
                {
                    SendDBWarning(st_Tmp);
                    TagMailedCL(st_Tmp.mo_LaunchedCL.mi_ChangeList, sz_HtmlFile);
                }
            }
        }

        // Update last CL values
        memcpy(&mst_LastClResult, &st_Tmp, sizeof(ChangeListResult));

        mb_FirstLaunch = false;
    }
}

void cl_Desdac::AppendBuildMessage( const ChangeListResult& st_CLres, char sz_Message[], bool _bNewError )
{
    int iCL = st_CLres.mo_LaunchedCL.mi_ChangeList;

    if( mb_FirstLaunch )
    {   sprintf(sz_Message, "DESDAC has been re-launched. First tested Change List (%d) isn't ok\n\n", iCL);
    }
    else if ( mb_SendMessageUser )
    {   sprintf(sz_Message, "Your Change List %d caused problems. \nVotre Change List %d a créé des problèmes. \n", iCL, iCL);
    }
    else
    {   sprintf(sz_Message, "Change List %d from user %s caused problems.\n", iCL, st_CLres.mo_LaunchedCL.msz_UserName );
    }

    //
    strcat(sz_Message,   "\n========================== \n");
    strcat(sz_Message,     "* BUILD ERROR =============== \n");
    if ( _bNewError )
    {
        strcat(sz_Message, "* Erreur lors de la compilation des IA en C dûe à cette CL.\n");
        strcat(sz_Message, "        Merci de corriger ça le plus rapidement possible.\n");
        strcat(sz_Message, "        Vous trouverez une description du problème dans le lien ci-dessous.\n\n");
    }
    else
    {
        strcat(sz_Message, "* Erreur lors de la compilation des IA en C dûe à cette CL.\n");
        strcat(sz_Message, "        Merci de corriger ça le plus rapidement possible.\n");
        strcat(sz_Message, "        Vous trouverez une description du problème dans le lien ci-dessous.\n\n");
    }
    char sz_HtmlLink[BIG_k_NameLength];
	strcat(sz_Message, "\n\n\Fichier log : ");
    sprintf(sz_HtmlLink, "%s\\" kaz_DESDAC_BuildLogFile_CLNB, pz_GetRemoteOutputPath(), iCL);
    strcat(sz_Message, sz_HtmlLink);
	strcat(sz_Message,   "\n\n");
	strcat(sz_Message,   "\n========================== \n");
	strcat(sz_Message,   "Historique Desdac: \\\\mtp-desdac1\\Output\\desdac.htm ");
}

void cl_Desdac::AppendBuildPrevMessage( const ChangeListResult& st_CLres, char sz_Message[], bool _bNewError )
{
    int iCL = st_CLres.mo_LaunchedCL.mi_ChangeList;

    if( mb_FirstLaunch )
    {   sprintf(sz_Message, "DESDAC has been re-launched. First tested Change List (%d) isn't ok\n\n", iCL);
    }

    //
    strcat(sz_Message,   "\n========================== \n");
    strcat(sz_Message,     "* BUILD ERROR =============== \n");
    if ( _bNewError )
    {
        strcat(sz_Message, "* Nouvelle erreur dans le code C/C++ détectée après un get de la base VSS (il y avait déjà des erreurs).\n");
        strcat(sz_Message, "        Merci de corriger le plus rapidement possible.\n");
        strcat(sz_Message, "        Vous trouverez le log dans le lien ci-dessous.\n\n");
    }
    else
    {
        strcat(sz_Message, "* Erreur dans le code C/C++ détectée après un get de la base VSS.\n");
        strcat(sz_Message, "        Merci de corriger le plus rapidement possible.\n");
        strcat(sz_Message, "        Vous trouverez le log dans le lien ci-dessous.\n\n");
    }

    char sz_HtmlLink[BIG_k_NameLength];
	strcat(sz_Message, "\n\n\Fichier log : ");
    sprintf(sz_HtmlLink, "%s\\" kaz_DESDAC_BuildPrevLogFile_CLNB, pz_GetRemoteOutputPath(), iCL);
    strcat(sz_Message, sz_HtmlLink);
	strcat(sz_Message,   "\n\n");
	strcat(sz_Message,   "\n========================== \n");
	strcat(sz_Message,   "Historique Desdac: \\\\mtp-desdac1\\Output\\desdac.htm ");
}

void cl_Desdac::AppendAiToCMessage( const ChangeListResult& st_CLres, char sz_Message[], bool _bNewError )
{
    int iCL = st_CLres.mo_LaunchedCL.mi_ChangeList;

    if( mb_FirstLaunch )
    {   sprintf(sz_Message, "DESDAC has been re-launched. First tested Change List (%d) isn't ok\n\n", iCL);
    }
    else if ( mb_SendMessageUser )
    {   sprintf(sz_Message, "Your Change List %d caused problems. \nVotre Change List %d a créé des problèmes. \n", iCL, iCL);
    }
    else
    {   sprintf(sz_Message, "Change List %d from user %s caused problems.\n", iCL, st_CLres.mo_LaunchedCL.msz_UserName );
    }

    //
    strcat(sz_Message,   "\n========================== \n");
    strcat(sz_Message,     "* C FILES GENERATION ERROR =============== \n");
    if ( _bNewError )
    {
        strcat(sz_Message, "* Il y a une nouvelle erreur lors de la génération des IA en C.\n");
        strcat(sz_Message, "        Merci de corriger ça le plus rapidement possible.\n");
        strcat(sz_Message, "        Vous trouverez une description du problème dans le fichier inclus (SimpleLog.txt).\n\n");
    }
    else
    {
        strcat(sz_Message, "* Echec lors de la génération des IA en C.\n");
        strcat(sz_Message, "        Merci de corriger ça le plus rapidement possible.\n");
        strcat(sz_Message, "        Vous pourrez trouver une description du problème dans le fichier inclus (SimpleLog.txt).\n\n");
    }
	strcat(sz_Message,   "\n\n");
	strcat(sz_Message,   "\n========================== \n");
	strcat(sz_Message,   "Historique Desdac: \\\\mtp-desdac1\\Output\\desdac.htm ");
}


void cl_Desdac::AppendAIMessage( const ChangeListResult& st_CLres, char sz_Message[], bool _bNewError )
{
    int iCL = st_CLres.mo_LaunchedCL.mi_ChangeList;

    if( mb_FirstLaunch )
    {   sprintf(sz_Message, "DESDAC has been re-launched. First tested Change List (%d) isn't ok\n\n", iCL);
    }
    else if ( mb_SendMessageUser )
    {   sprintf(sz_Message, "Votre Change List %d a créé des problèmes. \n", iCL, iCL);
    }
    else
    {   sprintf(sz_Message, "Change List %d from user %s caused problems.\n", iCL, st_CLres.mo_LaunchedCL.msz_UserName );
    }

    //
    strcat(sz_Message,   "\n========================== \n");
    strcat(sz_Message,     "* AI ERROR =============== \n");
    if ( _bNewError )
    {
        strcat(sz_Message, "* Erreur lors de la compilation des IA (il y avait déjà des erreurs).\n");
        strcat(sz_Message, "        Merci de corriger ça le plus rapidement possible.\n");
        strcat(sz_Message, "        Vous trouverez une description des problèmes dans le fichier inclus (SimpleLog.txt).\n\n");
    }
    else
    {
        strcat(sz_Message, "* Erreur lors de la compilation des IA.\n");
        strcat(sz_Message, "        Merci de corriger ça le plus rapidement possible.\n");
        strcat(sz_Message, "        Vous pourrez trouver une description des problèmes dans le fichier inclus (SimpleLog.txt).\n\n");
    }
	strcat(sz_Message,   "\n\n");
	strcat(sz_Message,   "\n========================== \n");
	strcat(sz_Message,   "Historique Desdac: \\\\mtp-desdac1\\Output\\desdac.htm ");
}

void cl_Desdac::AppendDBMessage( const ChangeListResult& st_CLres, char sz_Message[] )
{
    sprintf(sz_Message, "Change List %d from user %s caused problems.\n", st_CLres.mo_LaunchedCL.mi_ChangeList, st_CLres.mo_LaunchedCL.msz_UserName );

    char sz_HtmlLink[BIG_k_NameLength];
    sprintf(sz_HtmlLink, "See Log:  %s\\" kaz_DESDAC_LogFile_CLNB "  for errors.\n", pz_GetRemoteOutputPath(), st_CLres.mo_LaunchedCL.mi_ChangeList);
    
    strcat(sz_Message, sz_HtmlLink);

    //
    strcat(sz_Message, "\n================================ \n");
    strcat(sz_Message,   "* DATABASE ERROR =============== \n");
    strcat(sz_Message,   "* [ENGLISH]\n");
    strcat(sz_Message,   "* Database is having supplementary coherence problems.\n");
    strcat(sz_Message,   "        Please check with p4win that all the files you wanted to submit have really been submitted.\n");
    strcat(sz_Message,   "        This case will be checked by Yeti Support to fix the problems in Yeti Platform if necessary.\n");
    strcat(sz_Message,   "* [FRENCH]\n");
    strcat(sz_Message,   "* Il y a des problèmes supplémentaires de cohérence sur la database.\n");
    strcat(sz_Message,   "        Merci de vérifier avec P4Win que la Change List a bien été soumise.\n");
    strcat(sz_Message,   "        Ce cas sera vérifié par l'équipe support de Yeti.\n\n\n");

    strcat(sz_Message,   "History of submitted Change Lists and associated results:\n");
    sprintf(sz_HtmlLink, "%s\\%s\n", pz_GetRemoteOutputPath(), kaz_DESDAC_Html);
    strcat(sz_Message, sz_HtmlLink);
}

void cl_Desdac::AppendCrashMessage( const ChangeListResult& st_CLres, char sz_Message[] )
{
    sprintf(sz_Message, "Analyze of Change List %d from user %s crashed Yeti\n", st_CLres.mo_LaunchedCL.mi_ChangeList, st_CLres.mo_LaunchedCL.msz_UserName);
    if ( st_CLres.mi_ClientIndex!= -1 && st_CLres.mi_ClientIndex < int(mul_NbClients) )
    {
        strcat(sz_Message, "Crash occurred on DESDAC client ");
        strcat(sz_Message, mast_ClientPCs[st_CLres.mi_ClientIndex].msz_ClientName);
        strcat(sz_Message, "\n");

        char sz_HtmlLink[BIG_k_NameLength];
        sprintf(sz_HtmlLink, "See Crash Dump:  %s\\" kaz_DESDAC_CrashDumpFile_CLNB "\n", pz_GetRemoteOutputPath(), st_CLres.mo_LaunchedCL.mi_ChangeList);
        strcat(sz_Message, sz_HtmlLink);
    }

    //
    strcat(sz_Message, "\n================================ \n");
    strcat(sz_Message,   "* CRASH ERROR ================== \n");
    strcat(sz_Message,   "* Yeti crashed during test\n");
}


void cl_Desdac::AppendMapTestMessage( const ChangeListResult& st_CLres, char sz_Message[] )
{
    int iCL = st_CLres.mo_LaunchedCL.mi_ChangeList;

    if(mb_TestLastCLonlyEnable)
    {
        if( mb_FirstLaunch )
        {   sprintf(sz_Message, "DESDAC has been re-launched. First tested Change List (%d) isn't ok\n\n", iCL);
        }
        else
        {
            if( st_CLres.mo_LaunchedCL.mi_NbClSkipped > 0 )
            {
                sprintf(sz_Message, "Some problems have been detected when testing maps with Change List %d.\nThe problem is located between CLs %d and %d (included)\n", iCL, iCL, st_CLres.mo_LaunchedCL.mi_FirstCLSkipped );
                strcat( sz_Message, "You can check the History of tested Change Lists to see the last good CL.\n" );
            }
            else
            {   sprintf(sz_Message, "Change List %d from user %s caused problems.\n", iCL, st_CLres.mo_LaunchedCL.msz_UserName );
            }
        }
    }
    else
    {
        if( mb_FirstLaunch )
        {   sprintf(sz_Message, "DESDAC has been re-launched. First tested Change List (%d) isn't ok\n\n", iCL);
        }
        else if ( mb_SendMessageUser )
        {   sprintf(sz_Message, "Your Change List %d caused problems \nVotre Change List %d a créé des problèmes \n\n", iCL, iCL);
        }
        else
        {   sprintf(sz_Message, "Change List %d from user %s caused problems.\n", iCL, st_CLres.mo_LaunchedCL.msz_UserName );
        }
    }


    //
    strcat(sz_Message, "\n=================================== \n");
    strcat(sz_Message,   "* MAP TEST ERROR ================== \n");
    strcat(sz_Message,   "* \n");
    strcat(sz_Message,   "* Some problems have been detected on the following map(s):\n");
    strcat(sz_Message,   "* \n\n");

    char sz_Target[32];
    if( mb_MapTestingXenonEnable )
    {   sprintf(sz_Target, kaz_DESDAC_TARGET_XENON);
    }
    else
    {   sprintf(sz_Target, kaz_DESDAC_TARGET_PC);
    }

    for ( int i = 0; i < mi_NbMapToTest; ++i )
    {
        st_MapToTest& rMap = mast_MapToTest[i];

        char sz_Buf[BIG_k_NameLength];
        if( bIsFailedStatus(rMap.me_LoadStatus)  )
        {
            sprintf(sz_Buf, "  Map %s couldn't be loaded on %s (%s).\n", rMap.msz_MapName, sz_Target, szStatusFullName(rMap.me_LoadStatus) );
            strcat(sz_Message, sz_Buf);
        }
        else if( bIsFailedStatus(rMap.me_LaunchStatus) )
        {
            sprintf(sz_Buf, "  Map %s couldn't be launched on %s (%s).\n", rMap.msz_MapName, sz_Target, szStatusFullName(rMap.me_LaunchStatus) );
            strcat(sz_Message, sz_Buf);
        }
    }

    char sz_HtmlLink[BIG_k_NameLength];
    strcat(sz_Message, "\n\n\nHistory of tested Change Lists and associated results:\n");
    sprintf(sz_HtmlLink, "%s\\%s\n", pz_GetRemoteOutputPath(), kaz_DESDAC_HtmlMapTest);
    strcat(sz_Message, sz_HtmlLink);
}

void cl_Desdac::SendBuildWarning(ChangeListResult& st_Tmp, bool _bNewError )
{
    // Create Message
    char sz_Message[BIG_k_MailMessageLength] = {0};
    AppendBuildMessage(st_Tmp, sz_Message, _bNewError);

    // Create Mail Object string
    char sz_Object[1024];
    sprintf(sz_Object, "[DESDAC] Build Error: Change List %d report", st_Tmp.mo_LaunchedCL.mi_ChangeList);

    // Create Attached Log File
/*    char sz_LogFileName[BIG_k_NameLength];
    sprintf(sz_LogFileName, "%s\\" kaz_DESDAC_BuildLogFile_CLNB, msz_OutputFolder, st_Tmp.mo_LaunchedCL.mi_ChangeList);    
    char* pzAttachedFile = sz_LogFileName;
*/

     // Send mail to User and Desdac Mailing list
    char * pz_User = NULL;
    char * pz_Mail = NULL;
    char sz_Mail[BIG_k_NameLength];
    char sz_HostName[BIG_k_NameLength];

    if ( mb_SendMessageUser && !mb_FirstLaunch )
    {
        // Must describe client first, to obtain host
        GetP4HostName( st_Tmp, sz_HostName );

        // Now describe user, to obtain mail
        GetP4UserMailName( st_Tmp, sz_Mail );

        pz_User = st_Tmp.mo_LaunchedCL.msz_UserName;
        pz_Mail = sz_Mail;
    }

	if(!mb_FirstLaunch)
	{
		if ( !bSendLoggedMail(&st_Tmp, pz_User, pz_Mail, sz_Object, sz_Message, NULL) )
		{

		}
	}
}


void cl_Desdac::SendBuildPrevWarning(ChangeListResult& st_Tmp, bool _bNewError )
{
    // Create Message
    char sz_Message[BIG_k_MailMessageLength] = {0};
    AppendBuildPrevMessage(st_Tmp, sz_Message, _bNewError);

    // Create Mail Object string
    char sz_Object[1024];
    sprintf(sz_Object, "%s", "[DESDAC] Build Error.");

    // Create Attached Log File
/*    char sz_LogFileName[BIG_k_NameLength];
    sprintf(sz_LogFileName, "%s\\" kaz_DESDAC_BuildPrevLogFile_CLNB, msz_OutputFolder, st_Tmp.mo_LaunchedCL.mi_ChangeList);    
    char* pzAttachedFile = sz_LogFileName;
*/
     // Send mail to User and Desdac Mailing list
    char * pz_User = NULL;
    char * pz_Mail = NULL;
    char sz_Mail[BIG_k_NameLength];
    char sz_HostName[BIG_k_NameLength];

    if ( mb_SendMessageUser && !mb_FirstLaunch )
    {
        // Must describe client first, to obtain host
        GetP4HostName( st_Tmp, sz_HostName );

        // Now describe user, to obtain mail
        GetP4UserMailName( st_Tmp, sz_Mail );

        pz_User = st_Tmp.mo_LaunchedCL.msz_UserName;
        pz_Mail = sz_Mail;
    }

	if(!mb_FirstLaunch)
	{
		if ( !bSendLoggedMail(&st_Tmp, pz_User, pz_Mail, sz_Object, sz_Message, NULL) )
		{

		}
	}

    DeleteSimplifiedLogFileForAi();
}

void cl_Desdac::SendAiToCWarning(ChangeListResult& st_Tmp, bool _bNewError )
{
    // Create Message
    char sz_Message[BIG_k_MailMessageLength] = {0};
    AppendAiToCMessage(st_Tmp, sz_Message, _bNewError);

    // Create Mail Object string
    char sz_Object[1024];
    sprintf(sz_Object, "[DESDAC] C Generation Error: Change List %d report", st_Tmp.mo_LaunchedCL.mi_ChangeList);

    // Create Attached Log File
    char sz_LogFileName[BIG_k_NameLength];
    sprintf(sz_LogFileName, "%s\\" kaz_DESDAC_LogFile_CLNB, msz_OutputFolder, st_Tmp.mo_LaunchedCL.mi_ChangeList);
    CreateSimplifiedLogFileForAiToC(sz_LogFileName);
    char* pzAttachedFile = kaz_DESDAC_SimplifiedLogFile;


     // Send mail to User and Desdac Mailing list
    char * pz_User = NULL;
    char * pz_Mail = NULL;
    char sz_Mail[BIG_k_NameLength];
    char sz_HostName[BIG_k_NameLength];

    if ( mb_SendMessageUser && !mb_FirstLaunch )
    {
        // Must describe client first, to obtain host
        GetP4HostName( st_Tmp, sz_HostName );

        // Now describe user, to obtain mail
        GetP4UserMailName( st_Tmp, sz_Mail );

        pz_User = st_Tmp.mo_LaunchedCL.msz_UserName;
        pz_Mail = sz_Mail;
    }

	if(!mb_FirstLaunch)
	{
		if ( !bSendLoggedMail(&st_Tmp, pz_User, pz_Mail, sz_Object, sz_Message, pzAttachedFile) )
		{
			// SendMail failed: should warn Desdac admin...

	//        if ( mb_SendMessageUser )
	//        {
	//            char sz_Command[BIG_k_CommandLength];
	//            sprintf(sz_Command, "net send %s \"your CL %d caused problems\"", sz_HostName, st_Tmp.mi_ChangeList);
	//            system(sz_Command);
	//        }
		}
	}
    DeleteSimplifiedLogFileForAi();
}

void cl_Desdac::SendAIWarning(ChangeListResult& st_Tmp, bool _bNewError )
{
    // Create Message
    char sz_Message[BIG_k_MailMessageLength] = {0};
    AppendAIMessage(st_Tmp, sz_Message, _bNewError);

    // Create Mail Object string
    char sz_Object[1024];
    sprintf(sz_Object, "[DESDAC] AI Error: Change List %d report", st_Tmp.mo_LaunchedCL.mi_ChangeList);

    // Create Attached Log File
    char sz_LogFileName[BIG_k_NameLength];
    sprintf(sz_LogFileName, "%s\\" kaz_DESDAC_LogFile_CLNB, msz_OutputFolder, st_Tmp.mo_LaunchedCL.mi_ChangeList);
    CreateSimplifiedLogFileForAi(sz_LogFileName);
    char* pzAttachedFile = kaz_DESDAC_SimplifiedLogFile;


     // Send mail to User and Desdac Mailing list
    char * pz_User = NULL;
    char * pz_Mail = NULL;
    char sz_Mail[BIG_k_NameLength];
    char sz_HostName[BIG_k_NameLength];

    if ( mb_SendMessageUser && !mb_FirstLaunch )
    {
        // Must describe client first, to obtain host
        GetP4HostName( st_Tmp, sz_HostName );

        // Now describe user, to obtain mail
        GetP4UserMailName( st_Tmp, sz_Mail );

        pz_User = st_Tmp.mo_LaunchedCL.msz_UserName;
        pz_Mail = sz_Mail;
    }

	if(!mb_FirstLaunch)
	{
			if ( !bSendLoggedMail(&st_Tmp, pz_User, pz_Mail, sz_Object, sz_Message, pzAttachedFile) )
		{
			// SendMail failed: should warn Desdac admin...

	//        if ( mb_SendMessageUser )
	//        {
	//            char sz_Command[BIG_k_CommandLength];
	//            sprintf(sz_Command, "net send %s \"your CL %d caused problems\"", sz_HostName, st_Tmp.mi_ChangeList);
	//            system(sz_Command);
	//        }
		}
	}
    DeleteSimplifiedLogFileForAi();
}



void cl_Desdac::SendDBWarning(ChangeListResult& st_Tmp)
{
    // Create Message
    char sz_Message[BIG_k_MailMessageLength] = {0};
    AppendDBMessage( st_Tmp, sz_Message );


    // Create Mail Object string
    char sz_Object[1024];
    sprintf(sz_Object, "[DESDAC] DB Error: Change List %d report", st_Tmp.mo_LaunchedCL.mi_ChangeList);


    // Send mail to Desdac Mailing list
    if ( !bSendLoggedMail(&st_Tmp, NULL, NULL, sz_Object, sz_Message, NULL) )
    {
        // SendMail failed: should warn Desdac admin...
    }
}


void cl_Desdac::SendCrashWarning(ChangeListResult& st_Tmp)
{
    // Create Message
    char sz_Message[BIG_k_MailMessageLength] = {0};
    AppendCrashMessage(st_Tmp, sz_Message);


    // Create Mail Object string
    char sz_Object[1024];
    sprintf(sz_Object, "[DESDAC] CRASH Error: Change List %d report", st_Tmp.mo_LaunchedCL.mi_ChangeList);

    // Send mail to Desdac Mailing list
    if ( !bSendLoggedMail(&st_Tmp, NULL, NULL, sz_Object, sz_Message, NULL) )
    {
        // SendMail failed: should warn Desdac admin...
    }
}

void cl_Desdac::SendMapTestWarning(ChangeListResult& st_Tmp )
{
    // Create Message
    char sz_Message[BIG_k_MailMessageLength] = {0};
    AppendMapTestMessage(st_Tmp, sz_Message);

    char sz_Target[32];
    if( mb_MapTestingXenonEnable )
    {   sprintf(sz_Target, kaz_DESDAC_TARGET_XENON);
    }
    else
    {   sprintf(sz_Target, kaz_DESDAC_TARGET_PC);
    }

    // Create Mail Object string
    char sz_Object[1024];
    sprintf(sz_Object, "[DESDAC] MAPTEST %s Error: Change List %d report", sz_Target, st_Tmp.mo_LaunchedCL.mi_ChangeList);


    // If some AI bugs occurred, attach the AI bugs report
    char sz_AIbugsFileName[BIG_k_NameLength];
    for ( int i = 0; i < mi_NbMapToTest; ++i )
    {
        if( E_StatusAIBug == mast_MapToTest[i].me_LaunchStatus )
        {
            strcat(sz_Message, "\n=================================== \n");
            strcat(sz_Message,   "AI Bugs description :\n\n" );

            sprintf(sz_AIbugsFileName, "%s\\" kaz_DESDAC_AIBugsFile_CLNB, msz_OutputFolder, st_Tmp.mo_LaunchedCL.mi_ChangeList);
            bAppendTxtFileToMessage(sz_AIbugsFileName, sz_Message);
            break;
        }
    }


    // Send mail to User and Desdac Mailing list
    char * pz_User = NULL;
    char * pz_Mail = NULL;
    char sz_Mail[BIG_k_NameLength];

    if ( mb_SendMessageUser && !mb_FirstLaunch )
    {
        // Must describe client first, to obtain host
        char sz_HostName[BIG_k_NameLength];
        GetP4HostName( st_Tmp, sz_HostName );

        // Now describe user, to obtain mail
        GetP4UserMailName( st_Tmp, sz_Mail );

        pz_User = st_Tmp.mo_LaunchedCL.msz_UserName;
        pz_Mail = sz_Mail;
    }

    if ( !bSendLoggedMail(&st_Tmp, pz_User, pz_Mail, sz_Object, sz_Message, NULL) )
    {
        // SendMail failed: should warn Desdac admin...
    }
}


void cl_Desdac::SendAdminWarning( char* _szMessage )
{
    //AppendAdminMessage(_szMessage);

    // Create Mail Object string
    char sz_Object[1024];
    sprintf(sz_Object, "[DESDAC] ADMIN WARNING" );

    // Send mail to Desdac Mailing list
    bSendLoggedMail(NULL, NULL, NULL, sz_Object, _szMessage, NULL);
}

void cl_Desdac::CheckStackThreshold()
{
    if( mi_StackThresholdWarning )  //0 means no threshold
    {
        char sz_Message[BIG_k_CommandLength*8];

        static bool bClInStackReachedThreshold       = false;
        static bool bClResultInStackReachedThreshold = false;

        // Check CL stack
        if( bStackThresholdCrossUpDetection(mi_NbClInStack, bClInStackReachedThreshold) )
        {
            sprintf( sz_Message, "The Number of Change Lists pending in server %s stack reached %d.\n", pz_GetPcServer(), mi_StackThresholdWarning );
            strcat(  sz_Message, "This might be caused by blocked Yeti processes on clients.");
            SendAdminWarning(sz_Message);
        }

        // Check CL result stack
        if( bStackThresholdCrossUpDetection(mi_NbClResultInStack, bClResultInStackReachedThreshold) )
        {
            sprintf( sz_Message, "The Number of results waiting to be treated on server %s reached %d.\n", pz_GetPcServer(), mi_StackThresholdWarning );
            strcat(  sz_Message, "This might be caused by blocked Yeti processes on some clients.");
            SendAdminWarning(sz_Message);
        }
    }
}

bool cl_Desdac::bStackThresholdCrossUpDetection(int _iVal, bool &_rbAlreadyUp)
{   
    // manage the threshold cross-up detection
    // _rbAlreadyUp must be kept between calls, for each value watched 
    bool bCrossedUp = false;

    if( _rbAlreadyUp )
    {
        if( _iVal < mi_StackThresholdWarning  )
        {
            _rbAlreadyUp = false;   // return under
        }
    }
    else
    {
        if( _iVal >= mi_StackThresholdWarning )
        {
            _rbAlreadyUp = true;
            bCrossedUp = true;
        }
    }
    
    return bCrossedUp;
}



void cl_Desdac::UpdateStatusFile()
{
    // update stats
    mi_StatMaxNbClReachedInStack = max(mi_StatMaxNbClReachedInStack, mi_NbClInStack);

    // use tag file
    FILE * pTag = fopen(kaz_DESDAC_TagFile, "wt");
    if ( pTag )
    {
        //////////////////////
        // *** Server side ***
        if( bIsServer() )
        {
            fprintf(pTag, "SERVER:\n");

            // General status
            fprintf(pTag, "\n" );
            if( iGetLastClResult() == -1)
            {   fprintf(pTag, kTab "Status: N/A\n");
            }
            else
            {
                fprintf(pTag, kTab "Status: %s  (",  bGetLastClResultGlobalFailed()  ?   "FAILED" : "OK" );
                fprintf(pTag, kTab "AI:%s - ",       bGetLastClResultAiFailed()      ?   "FAILED" : "OK" );
                fprintf(pTag, kTab "DB:%s",          bGetLastClResultDbFailed()      ?   "FAILED" : "OK" );
                if( bGetGlobalMapTesting() )
                {   fprintf(pTag, kTab " - MAP:%s",  bGetLastClResultMapTestFailed() ?   "FAILED" : "OK" );
                }
                fprintf(pTag, ")\n");
            }


            // Stack
            int iNbCL = iGetNbClOnStack();
            fprintf(pTag, kTab "Stack:  %d", iNbCL );
            if( iNbCL )
            {   fprintf(pTag, "   (CL %d to %d)", iGetClInTopOfStack(), iGetLastClInStack() );
            }
            fprintf(pTag, "\n");
            fprintf(pTag, kTab "stack max = %d\n", mi_StatMaxNbClReachedInStack);


            // Clients status
            fprintf(pTag, "\n" );
            for ( int i = 0; i < iGetNbClients(); ++i )
            {
                char sz_ClientInfos[BIG_k_NameLength];
                GetClientStatInfos(i, sz_ClientInfos);

                fprintf(pTag, kTab "%02d : %s (%s) %s\n", i, pz_GetClient(i), pz_GetClientStatus(i), sz_ClientInfos);
            }

            fprintf(pTag, "\n\n");
        }


        //////////////////////
        // *** Client side ***
        fprintf(pTag, "CLIENT:\n");
        char szStatus[256];
        GetLocalClientStatus(szStatus);
        fprintf(pTag, kTab "%s\n", szStatus);


        fclose(pTag);
    }

}

// (Client)
// returns the current explicit status in _pszStatus
void cl_Desdac::GetLocalClientStatus(char* _pszStatus)
{
    _pszStatus[0] = 0;

    if( bIsCopyToXENONrunning() )
    {   sprintf(_pszStatus, "Copying bigfile to Xenon kit '%s': %2.1f%%\n", msz_XenonKitName, fCopyProgress() );
    }
    
    if( bIsYetiXENONrunning() )
    {   sprintf(_pszStatus, "Testing on Xenon: map %d/%d\n", mi_FirstRemainingMapToTestOnClient+1, mi_NbMapToTestOnClient );
    }
    
    if( bIsYetiPCrunning() )
    {   sprintf(_pszStatus, "Running Yeti on PC\n" );
    }
    
    if( bIsXENONrebooting() )
    {   sprintf(_pszStatus, "Xenon '%s' is rebooting...\n", msz_XenonKitName );
    }
}


void cl_Desdac::GetP4HostName(const ChangeListResult& st_Tmp, char sz_HostName[] )
{
    char sz_Command[BIG_k_CommandLength];
    sprintf(sz_Command, "p4 -p %s client -o %s > %s", msz_PerforceServer, st_Tmp.mo_LaunchedCL.msz_ClientName, msz_DesdacTempFile);
    LaunchSystemCommand(sz_Command, true);

    FILE * pTmp = fopen(msz_DesdacTempFile, "rt");
    if ( pTmp )
    {
        int iFile = _fileno(pTmp);

        int iFileSize = _filelength(iFile);
        char * pBuffer = (char*)malloc(iFileSize);
        fread(pBuffer, 1, iFileSize, pTmp);
        fclose(pTmp);
        flushall();

        char * p = strstr(pBuffer, "Host:");
        if ( p )
            p += strlen("Host:");
        p = strstr(p, "Host:");
        if ( p )
            p += strlen("Host:");
        if ( p )
            sscanf(p, "%s", sz_HostName);
        free(pBuffer);
    }

    // Delete temporary file
    DeleteFile(msz_DesdacTempFile);
}

void cl_Desdac::GetP4UserMailName(const ChangeListResult& st_Tmp, char sz_Mail[] )
{
    char sz_Command[BIG_k_CommandLength];
    sprintf(sz_Command, "p4 -p %s user -o %s > %s", msz_PerforceServer, st_Tmp.mo_LaunchedCL.msz_UserName, msz_DesdacTempFile);
    LaunchSystemCommand(sz_Command, true);

    FILE * pTmp = fopen(msz_DesdacTempFile, "rt");
    if ( pTmp )
    {
        int iFile = _fileno(pTmp);

        int iFileSize = _filelength(iFile);
        char * pBuffer = (char*)malloc(iFileSize);
        fread(pBuffer, 1, iFileSize, pTmp);
        fclose(pTmp);
        flushall();

        char * p = strstr(pBuffer, "Email:");
        if ( p )
            p += strlen("Email:");
        p = strstr(p, "Email:");
        if ( p )
            p += strlen("Email:");
        if ( p )
            sscanf(p, "%s", sz_Mail);
        free(pBuffer);
    }

    // Delete temporary file
    DeleteFile(msz_DesdacTempFile);
}


//===================================================================
// This function is used on server when the server is really in
// late regarding of number of change lists.
// It empties the change list stack
//===================================================================
void cl_Desdac::FlushChangeListStack()
{
    mi_NbClInStack = 0;
}

void cl_Desdac::PushChangeList(int _iCl, char * _pz_UserName, char * _pz_ClientName)
{
    // Really not optimal, but we don't care at all

    // If max of CL reached, we skip this CL
    if ( mi_NbClInStack >= ki_MaxChangeListInStack )
        return;

    // CL already in stack ?
    if ( mi_NbClInStack > 0 )
        if ( mast_ClStack[0].mi_ChangeList == _iCl )
            return;

    // Change list must not be older than last CL
    if ( _iCl <= mst_LastClResult.mo_LaunchedCL.mi_ChangeList )
        return;

    // Change list must NOT be in result stack, waiting to be treated
    for ( int i = 0; i < mi_NbClResultInStack; ++i )
    {
        if ( mast_ClResultStack[i].mo_LaunchedCL.mi_ChangeList == _iCl )
            return;
    }


    // move queue
    for ( int i = mi_NbClInStack; i > 0; --i )
    {
        mast_ClStack[i] = mast_ClStack[i-1];
    }
    mi_NbClInStack++;


    // put the new CL in 0
    mast_ClStack[0].mi_ChangeList = _iCl;
    strcpy(mast_ClStack[0].msz_UserName,   _pz_UserName  );
    strcpy(mast_ClStack[0].msz_ClientName, _pz_ClientName);

    // Options for the CL
    mast_ClStack[0].mul_Options = k_Option_GetFromPerforce;

    if ( mb_MapTestingEnable )
    {   
        mast_ClStack[0].mul_Options |= k_Option_LoadMap;
    }

    if ( mb_MapTestingXenonEnable )
    {   
        mast_ClStack[0].mul_Options |= k_Option_LaunchXenon;
    }

    if ( mb_CheckPerforceEnable )
    {   mast_ClStack[0].mul_Options |= k_Option_CheckPerforce;
    }

    if ( mb_CompileAiEnable )
    {   mast_ClStack[0].mul_Options |= k_Option_CompileAi;
    }

    if ( mb_AiToCTestingEnable )
    {   mast_ClStack[0].mul_Options |= k_Option_CompileAiToC;
    }
}


///////////////////////////////////////////
// Global main loop
void cl_Desdac::RunOneFrame()
{
    RunOneFrame_Client();

    // The server can be a client too
    if ( bIsServer() )
        RunOneFrame_Server();

    // Write status infos in the tag file (for remote watch)
    UpdateStatusFile();
}


// (Client)
// main loop
void cl_Desdac::RunOneFrame_Client()
{
    // check end of test
    TestYetiFinished();

    if ( bIsYetiRunning() || bIsVisualRunning() || bIsVssRunning())
        return;

    // check for engine update
    bCheckAndStartEngineUpdate();

    if ( mb_IsClientSuspended )
        return;

    // Check for test request
    bCheckAndStartRequest();
}


// (Client)
// Check if an update request arrived, decode it and perform it
// return true if an update was made
bool cl_Desdac::bCheckAndStartEngineUpdate()
{
	// If creation date of jade_edd.exe on the server is different, we get this one
	struct __stat64 fileInfo1;
	struct __stat64 fileInfo2;
	int result1, result2;
	bool sol = false;

	char * c1 = NULL;
	char * c2 = NULL;

	result1 = _stat64( "X:\\jade_edd.exe", &fileInfo1 );
	result2 = _stat64( "\\\\srvdata4-mtp\\Sally_game\\Sally_Tools\\Jade\\Jade_edd.exe", &fileInfo2 );
	
	if((result1 != 0) || (result2 != 0))
		return false;

	c1 = _ctime64(&fileInfo1.st_mtime);
	c2 = (char*)malloc(strlen(c1)+1);
	strcpy(c2, c1);
	
	c1 = _ctime64(&fileInfo2.st_mtime);

	if(strcmp(c1, c2) == 0)		
		sol = false;
	else
	{
		CopyFile(kaz_DESDAC_JadeServerExe, "X:\\jade_edd.exe", FALSE );		        
		sol = true;
	}
		
//	if(c1) free(c1);
	if(c2) free(c2);

	return sol;	

/*   
	// Update file ?
    FILE * pUpdate = fopen(kaz_DESDAC_RequestUpdateFile, "rt");
    if ( pUpdate )
    {
        UpdateListOfAvailableClients();
        UpdateStatusFile();

        char sz_RemotePCVersionPath     [BIG_k_NameLength];
        char sz_RemoteXenonVersionPath  [BIG_k_NameLength];
        char sz_RemoteDesdacVersionPath [BIG_k_NameLength];

        // Read update-request File Version and Type
        unsigned long uiFileVer;
        unsigned long uiUpdateTypeFlags;
        fscanf(pUpdate, "UpdateRequest=%d %d\n", &uiFileVer, &uiUpdateTypeFlags);

        ////////////////////////////
        // **** Read Requests according to type ****
        // Update PC engine
        if( uiUpdateTypeFlags & kuiEnginePCUpdate)
        {
            // Read version Number
            fscanf(pUpdate, "YetiVer=" kaz_DESDAC_EngineVersion "\n", &mi_EnginePCVersion, &mi_EnginePCPatch, &mi_EnginePCCompilNb);

            // read path to use for update. (fgets is "space compliant")
            fgets(sz_RemotePCVersionPath, BIG_k_NameLength, pUpdate);
            sz_RemotePCVersionPath[ strlen(sz_RemotePCVersionPath)-1 ] = 0;     // remove carriage return
        }

        // Update Xenon engine
        if( uiUpdateTypeFlags & kuiEngineXenonUpdate)
        {
            // read path to use for update. (fgets is "space compliant")
            fgets(sz_RemoteXenonVersionPath, BIG_k_NameLength, pUpdate);
            sz_RemoteXenonVersionPath[ strlen(sz_RemoteXenonVersionPath)-1 ] = 0;     // remove carriage return
        }

        // Update Desdac itself
        if( uiUpdateTypeFlags & kuiDesdacSelfUpdate)
        {
            // read path to use for update. (fgets is "space compliant")
            fgets(sz_RemoteDesdacVersionPath, BIG_k_NameLength, pUpdate);
            sz_RemoteDesdacVersionPath[ strlen(sz_RemoteDesdacVersionPath)-1 ] = 0;     // remove carriage return
        }

        // End of Request File Analysis
        fclose(pUpdate);



        ////////////////////////////
        // **** Launch requests ****

        // Engine Update
        if( uiUpdateTypeFlags & kuiEnginePCUpdate)
        {   bUpdatePCEngine(sz_RemotePCVersionPath);
        }

        if( (uiUpdateTypeFlags & kuiEngineXenonUpdate) && mb_MapTestingXenonEnable )
        {   bUpdateXenonEngine(sz_RemoteXenonVersionPath);
        }

        if( uiUpdateTypeFlags & (kuiEnginePCUpdate | kuiEngineXenonUpdate) )
        {   
            // store engine update time
            mo_EngineLastUpdateTime = CTime::GetCurrentTime();
            SaveInfoInIniFile();
        }


        // Bigfile reset
        if( uiUpdateTypeFlags & kuiNewBigfileUpdate )    // be careful: xenon bigfile still needs some manual settings...
        {   
            // remove the current bigfile (to restart with a fresh one)
            DeleteFile(msz_BigFileName);
        }
        

        // Self Update of desdac.exe : spawn external updater and quit (the updater will replace desdac and relaunch it)     
        if( uiUpdateTypeFlags & kuiDesdacSelfUpdate)
        {   
            // get the desdac exe name
            char szModuleFileName[MAX_PATH];
            GetModuleFileName(NULL, szModuleFileName, MAX_PATH);

            // build command line
            char szArg1[MAX_PATH];
            sprintf( szArg1, "\"%s\"", szModuleFileName );

            char szArg2[MAX_PATH];
            sprintf( szArg2, "\"%s\"", sz_RemoteDesdacVersionPath );
            

            // command : SelfUpdater.exe ExeToUpdate PathOfNewVersion
            _spawnl	(_P_DETACH, kaz_DESDAC_DefaultSelfUpdaterName, kaz_DESDAC_DefaultSelfUpdaterName, szArg1, szArg2, NULL);
            
            // delete all desdac files before quitting
            CleanFiles();

            // quit desdac
            exit(0);
        }


        // auto exit
        if( uiUpdateTypeFlags & kuiDesdacAutoQuit )
        {   
            // delete all desdac files before quitting
            CleanFiles();

            // quit desdac
            exit(0);
        }
        
        // delete the update request file (indicates that the update is finished)
        DeleteFile(kaz_DESDAC_RequestUpdateFile);

        return true;
    }
    else
    {   return false;
    }
*/
}



// (Client)
bool cl_Desdac::bUpdatePCEngine(const char* _szRemoteVersionPath)
{
    // Delete previous backup version
    char sz_Command[BIG_k_CommandLength];
    sprintf(sz_Command, "rmdir /Q /S .\\" kaz_FolderVersion_backup );
    LaunchSystemCommand(sz_Command, true);

    // backup current version
    sprintf(sz_Command, "move .\\" kaz_FolderVersion " .\\" kaz_FolderVersion_backup );
    LaunchSystemCommand(sz_Command, true);

    // copy version from server
    CopyFolder( _szRemoteVersionPath, ".\\" kaz_FolderVersion );

    // copy overrides (files which need a specific configuration for desdac)
    CopyFolder( ".\\" kaz_FolderVersionOverrides, ".\\" kaz_FolderVersion );

    return true; //todo, test if update succeeded
}

// (Client)
bool cl_Desdac::bUpdateXenonEngine(const char* _szRemoteVersionPath)
{
    char sz_BigFilePath[BIG_k_NameLength];
    ExtractPathFromFullName( sz_BigFilePath, msz_BigFileName );

    // Delete previous backup version
    // TODO

    // backup current version
    // TODO

    // copy version from server to PC (dvd emu folder, i.e. same as bigfile)
    CopyFolder(_szRemoteVersionPath, sz_BigFilePath);

    // copy version from server to XENON
    char sz_XenonExeFilePath[BIG_k_NameLength];
    ExtractPathFromFullName( sz_XenonExeFilePath, msz_XenonExeName );
    bCopyFolderToXenon(_szRemoteVersionPath, sz_XenonExeFilePath);

    return true; //todo, test if update succeeded
}



// (Client)
// Check if a request arrived, and start it
// return true if started
bool cl_Desdac::bCheckAndStartRequest()
{
    // Scan the "request" folder
    char sz_Find[BIG_k_NameLength];
	
    //sprintf(sz_Find, ".\\sys\\request.txt"); //".\\"kaz_DESDAC_RequestFile);
	sprintf(sz_Find, ".\\"kaz_DESDAC_RequestFile);
	//TCHAR szPath[50];
    //GetCurrentDirectory(50, szPath);
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile( sz_Find, &wfd );
    if( INVALID_HANDLE_VALUE == hFind )
    {
        // No request found
        return false;
    }
    else
    {
        char sz_FileName[BIG_k_NameLength];
        strcpy(sz_FileName, wfd.cFileName);
        FindClose( hFind );

        // start found request
        AnalyzeAndLaunchRequest(sz_FileName);
        return true;
    }
}


void cl_Desdac::TestCrashPC()
{
    FILE* pCrashDump = fopen( kaz_DESDAC_CrashDumpFile, "r");
    if ( pCrashDump )
    {
        SetYetiPCHasCrashed(true);
        fclose(pCrashDump);
    }
}


// Returns results of xenon launch. MAP PER MAP TEST
// in: _pst_CL 
// out: _pst_Results
bool cl_Desdac::AnalyzeLogFileXenon_MapPerMapTest(ChangeListToLaunch * _pst_CL, ChangeListResult* _pst_Results)
{
    // Check if last map already reached (in case of global abort)
    if( mi_FirstRemainingMapToTestOnClient == mi_NbMapToTestOnClient )
    {   return true;
    }

    // check map results
    st_MapToTest& rMap  = mast_MapToTestOnClient[mi_FirstRemainingMapToTestOnClient];

    char az_LogFileName[BIG_k_CommandLength]; 
    sprintf(az_LogFileName, kaz_DESDAC_XenonResultLogFile );
    FILE * pLogFile = NULL;
    pLogFile = fopen(az_LogFileName, "rt");
    if ( pLogFile )
    {
        int iFile = _fileno(pLogFile);

        int iFileSize = _filelength(iFile);
        char * pBuffer = (char*)malloc(iFileSize);
        fread(pBuffer, 1, iFileSize, pLogFile);
        fclose(pLogFile);
        flushall();

        // Analyze results of Map Testing
        char sz_MapMessage[BIG_k_LogMessageLength];
        char* pzMapKey = rMap.msz_MapKey;
        char* pFind;


        // Check Load
        sprintf(sz_MapMessage, MAIN_kaz_DESDACNAMEDTAG_ENGINELOADMAP_OK, pzMapKey);
        pFind = strstr(pBuffer, sz_MapMessage);
        rMap.me_LoadStatus = (pFind ? E_StatusOK:E_StatusCrashed);


        // Check Launch (if load was ok)
        if( rMap.me_LoadStatus == E_StatusOK )
        {
            sprintf(sz_MapMessage, MAIN_kaz_DESDACNAMEDTAG_ENGINELAUNCHMAP_OK, pzMapKey);
            pFind = strstr(pBuffer, sz_MapMessage);
            rMap.me_LaunchStatus = (pFind ? E_StatusOK:E_StatusCrashed);
            rMap.me_CloseStatus  = (pFind ? E_StatusOK:E_StatusNotTested);
        }

        free(pBuffer);

        // rename possible crash dumps (so that it is kept, if another crash occurs in remaining maps)
        char sz_File2[BIG_k_NameLength];
        sprintf(sz_File2, kaz_DESDAC_XenonResultDumpFile_MapNB, mi_FirstRemainingMapToTestOnClient);
        MoveFileEx( kaz_DESDAC_XenonResultDumpFile, sz_File2, MOVEFILE_REPLACE_EXISTING );
        sprintf(sz_File2, kaz_DESDAC_XenonResultScreenShotFile_MapNB, mi_FirstRemainingMapToTestOnClient);
        MoveFileEx( kaz_DESDAC_XenonResultScreenShotFile, sz_File2, MOVEFILE_REPLACE_EXISTING );
        sprintf(sz_File2, kaz_DESDAC_XenonResultLogFile_MapNB, mi_FirstRemainingMapToTestOnClient);
        MoveFileEx( kaz_DESDAC_XenonResultLogFile, sz_File2, MOVEFILE_REPLACE_EXISTING );

        mb_XENONisRebooting = true;
        Temporize(mul_RebootTimeXenon); // Xenon crashed, and reboots automatically : wait for it
        mb_XENONisRebooting = false;
    }
    else
    {  
        // no log means everything was ok
        rMap.me_LoadStatus      = E_StatusOK;
        rMap.me_LaunchStatus    = E_StatusOK;
        rMap.me_CloseStatus     = E_StatusOK;
    }


    // test remaining maps
    mi_FirstRemainingMapToTestOnClient++;
    int iNbRemainingMaps = mi_NbMapToTestOnClient - mi_FirstRemainingMapToTestOnClient;
    if( iNbRemainingMaps > 0 )
    {
        // launch next map
        LaunchOneMapXenon( mast_MapToTestOnClient[mi_FirstRemainingMapToTestOnClient].msz_MapKey );

        // don't create result file yet
        return false;
    }

    return true;
}


// Returns results of PC launch
// in: _pst_CL 
// out: _pst_Results
// returns true if test is finished, false otherwise (yeti was relaunched to finish them)
bool cl_Desdac::AnalyzeLogFile(ChangeListToLaunch * _pst_CL, ChangeListResult* _pst_Results)
{
    FILE * pLogFile = NULL;

    assert(_pst_CL);
    assert(_pst_Results);

    _pst_Results->mi_NbDbErrors = 0;
    _pst_Results->mi_NbAiErrors = 0;

    char az_LogFileName[BIG_k_CommandLength];
    sprintf(az_LogFileName, "%s\\%s", msz_ExeAndLogPath, msz_LogFileName);
    pLogFile = fopen(az_LogFileName, "rt");
    if ( pLogFile )
    {
        int iFile = _fileno(pLogFile);

        int iFileSize = _filelength(iFile);
        char * pBuffer = (char*)malloc(iFileSize);
        fread(pBuffer, 1, iFileSize, pLogFile);
        fclose(pLogFile);
        flushall();


        // detect DB errors
        if ( strstr(pBuffer, MAIN_kaz_DESDACTAG_DBFAILED) )
        {
            _pst_Results->mb_DbFailed = true;

            char * p = strstr(pBuffer, "End of errors found");
            if ( p )
            {
                p += strlen("End of errors found");
                sscanf(p, "%d errors detected", &(_pst_Results->mi_NbDbErrors) );
            }
        }
        else if( NULL == strstr(pBuffer, MAIN_kaz_DESDACTAG_DBOK) )
        {
            // DB wasn't tested (probably crashed before or during test)
            _pst_Results->mi_NbDbErrors = -1;
        }


        // detect AI errors
        char * pAiFailed = strstr(pBuffer, MAIN_kaz_DESDACTAG_AICOMPILFAILED);
        if ( pAiFailed )
        {
            _pst_Results->mb_AiFailed = true;
			_pst_Results->msz_AiLog = GetAiLog(pBuffer);
			RemoveBlanks(_pst_Results->msz_AiLog);

/*            
            // Try to count AI errors
            char * p = strstr(pBuffer, "...AI Models compiled successfully");
            if ( p )
            {
                char * s = strstr(p, "\n");
                if ( s )
                {
                    s++;
                    s = strstr(s, "\n");
                }
                while ( s && s < pAiFailed )
                {
                    _pst_Results->mi_NbAiErrors++;
                    s = strstr(s, "\n"); if ( s ) s++;
                }
                _pst_Results->mi_NbAiErrors /= 2;
            }
*/
        }
        else if( NULL == strstr(pBuffer, MAIN_kaz_DESDACTAG_AICOMPILOK) )
        {   
            // AI wasn't compiled (probably crashed before or during compilation)
            _pst_Results->mi_NbAiErrors = -1;
        }

/********** Analyse LaunchMap ************/
		if(mb_MapTestingEnable && !bAiErrorDetected())
		{
			
		}


/********** Analyse BuildPrevLog ************/
		if(mb_AiToCTestingEnable && !bAiErrorDetected())		
		//if((_pst_CL->mul_Options & k_Option_CompileAiToC) && !bAiErrorDetected())
		{
			_pst_Results->mi_NbBuildPrevErrors = iBuildPrevErrorDetected();
			if(_pst_Results->mi_NbBuildPrevErrors > 0)
				_pst_Results->mb_BuildPrevFailed = true;
		}
/********************************************/

/********** Analyse C generation ************/
		//if((_pst_CL->mul_Options & k_Option_CompileAiToC) && !bAiErrorDetected() && (iBuildPrevErrorDetected() <= 0))
		if(mb_AiToCTestingEnable && !bAiErrorDetected() && (iBuildPrevErrorDetected() <= 0))
		{			
			if(bAiToCErrorDetected())
			{
				_pst_Results->mb_AiToCFailed = true;
				_pst_Results->msz_AiToCLog = GetAiToCLog(pBuffer);
				RemoveBlanks(_pst_Results->msz_AiToCLog);
			}
		}
/********************************************/

/********** Analyse BuildLog ************/
		//if((_pst_CL->mul_Options & k_Option_CompileAiToC) && !bAiErrorDetected() && (iBuildPrevErrorDetected() <= 0) && !bAiToCErrorDetected())
		if(mb_AiToCTestingEnable && !bAiErrorDetected() && (iBuildPrevErrorDetected() <= 0) && !bAiToCErrorDetected())
		{				
			_pst_Results->mi_NbBuildErrors = iBuildErrorDetected();
			if(_pst_Results->mi_NbBuildErrors > 0)
				_pst_Results->mb_BuildFailed = true;
			else
				_pst_Results->mb_StateOK = true;
		}
/********************************************/

        // Analyze results of Map Testing
        int i=0;  // loop exit value will be used later 
        if(_pst_CL->mul_Options & k_Option_LoadMap)
        {
            // Seek optimization. avoid parsing all the log for each map
            char* pzSeek = strstr(pBuffer, MAIN_kaz_DESDACTAG_MAPTESTING_BEGIN);
            if( !pzSeek )
            {   pzSeek = pBuffer;   // handle previous yeti version, without the tag
            }

            // loop
            for ( i = mi_FirstRemainingMapToTestOnClient; i < mi_NbMapToTestOnClient; ++i )
            {
                char sz_MapMessage[BIG_k_LogMessageLength];
                st_MapToTest& rMap = mast_MapToTestOnClient[i];
                char* pzMapKey = rMap.msz_MapKey;

                // Check Start of test
                sprintf(sz_MapMessage, MAIN_kaz_DESDACNAMEDTAG_LOADMAP_START, pzMapKey);
                char* pzStart = strstr(pzSeek, sz_MapMessage);
                if( !pzStart ) continue;  // map wasn't tested

                // Check Load
                sprintf(sz_MapMessage, MAIN_kaz_DESDACNAMEDTAG_LOADMAP_OK, pzMapKey);
                char* pzLoad = strstr(pzStart, sz_MapMessage);

                if( pzLoad )
                {   rMap.me_LoadStatus = E_StatusOK;
                }
                else
                {
                    rMap.me_LoadStatus = E_StatusCrashed;
                    break;        // remaining maps could not be tested.
                }


                // Check Launch
                sprintf(sz_MapMessage, MAIN_kaz_DESDACNAMEDTAG_LAUNCHMAP_OK, pzMapKey);
                char* pzLaunchOK = strstr(pzLoad, sz_MapMessage);

                if( pzLaunchOK )
                {   rMap.me_LaunchStatus = E_StatusOK;
                }
                else
                {
                    sprintf(sz_MapMessage, MAIN_kaz_DESDACNAMEDTAG_LAUNCHMAP_FAILED, pzMapKey);
                    char* pzLaunchFailed = strstr(pzLoad, sz_MapMessage);

                    if( pzLaunchFailed )
                    {
                        // try to detect AI bug between end-load tag andlaunch-failed Tag
                        sprintf(sz_MapMessage, MAIN_kaz_DESDACTAG_AIERROR );
                        char* pzAIBug = strstr(pzLoad, MAIN_kaz_DESDACTAG_AIERROR);

                        // failed, but no crash: do not break
                        if( pzAIBug && (pzAIBug < pzLaunchFailed) )
                        {   
                            rMap.me_LaunchStatus = E_StatusAIBug;
                            
                            char sz_AIBug[BIG_k_LogMessageLength];
                            size_t uiAIbugLen = pzLaunchFailed-pzAIBug-1;

                            assert( uiAIbugLen < BIG_k_LogMessageLength);
                            strncpy(sz_AIBug, pzAIBug, uiAIbugLen);
                            sz_AIBug[uiAIbugLen] = 0;

                            StoreAIBugToFile(pzMapKey, sz_AIBug);
                        }
                        else
                        {   rMap.me_LaunchStatus = E_StatusUnidentifiedError;
                        }
                    }
                    else
                    {
                        rMap.me_LaunchStatus = E_StatusCrashed;
                        break;        // crash: remaining maps could not be tested.
                    }
                }

                // Check Close
                sprintf(sz_MapMessage, MAIN_kaz_DESDACNAMEDTAG_CLOSEMAP_OK, pzMapKey);
                char* pzClose = strstr(pzLoad, sz_MapMessage);

                if( pzClose )
                {   rMap.me_CloseStatus = E_StatusOK;
                }
                else
                {
                    rMap.me_CloseStatus = E_StatusCrashed;
                    break;        // crash: remaining maps could not be tested.
                }
            }
        }
        free(pBuffer);


        // rename possible crash dump (so that it is kept, if another crash occurs in remaining maps)
        if ( bYetiPCHasCrashed() ) 
        {
            char sz_File2[BIG_k_NameLength];
            sprintf(sz_File2, kaz_DESDAC_CrashDumpFile_MapNB, i);
            MoveFileEx( kaz_DESDAC_CrashDumpFile, sz_File2, MOVEFILE_REPLACE_EXISTING );
        }


        // if map test didn't finish properly : test remaining maps
        if(_pst_CL->mul_Options & k_Option_LoadMap)
        {
            mi_FirstRemainingMapToTestOnClient = i + 1;
            int iNbRemainingMaps = mi_NbMapToTestOnClient - mi_FirstRemainingMapToTestOnClient;
            if( iNbRemainingMaps > 0 )
            {
                // re-Create map testing file
                char sz_MapTestingFIleName[BIG_k_NameLength];
                sprintf(sz_MapTestingFIleName, "%s\\%s", msz_ExeAndLogPath, kaz_DESDAC_MapTestingFile);
                DeleteFile(sz_MapTestingFIleName);

                FILE * pMapTesting = fopen(sz_MapTestingFIleName, "w+");
                if ( pMapTesting )
                {
                    fprintf(pMapTesting, "MapTesting %d\n", iNbRemainingMaps);
                    for( int j=mi_FirstRemainingMapToTestOnClient; j<mi_NbMapToTestOnClient; j++ )
                    {
                        fprintf(pMapTesting, "%s\n", mast_MapToTestOnClient[j].msz_MapKey);
                    }

                    fclose(pMapTesting);
                    flushall();

                    // Set launch options to "Loadmap" only (no need to get or check again the database)
                    _pst_CL->mul_Options = k_Option_LoadMap | k_Option_CompileAi ; // also re-compile AI, for the moment

                    // re-launch yeti. previous test map results are kept in mast_MapToTestOnClient.
                    LaunchVersionGenerationAsync(_pst_CL);

                    // don't create result file yet
                    return false;
                }
            }
        }
    }

    return true;
}

void cl_Desdac::RemoveBlanks(char* _pBuffer)
{
	char *c, *d;
	c = d = _pBuffer;
	while(*c) 
	{
		if((*c != 32) && (*c != 10) && (*c != 13)) 		
			*d++ = *c; // SI != ESPACE
		c++;
	}
	*d = 0; // TERMINE LA CHAINE

	//return _pBuffer;
}


char* cl_Desdac::GetAiToCLog(char* _pBuffer)
{	
	char* log = "";
    char sep[] = "\n";
    char* tok;
    char * pStart = strstr(_pBuffer, "[AI compilation start]");
    if ( pStart )
    {
        pStart += strlen("[AI compilation start]");
        char * pEnd = NULL;
        if ( pStart )
		{
            pStart = strstr(pStart, "[AI compilation start]");
			if ( pStart )
			{
				pStart += strlen("[AI compilation start]\n");
				tok = strtok(pStart, sep);
				while(tok != NULL)
				{
					log = strstr(tok, "Error detected -- [WAR");
					if(!log)
					{
						log = (char*)malloc(strlen(tok));
						strcpy(log, tok);
						break;
					}
					tok = strtok(NULL, sep);
				}
			}
		}
    }	
	return log;
}


char* cl_Desdac::GetAiLog(char* _pBuffer)
{
	char* log = "";
    char sep[] = "\n";
    char* tok;
    char * pStart = strstr(_pBuffer, "[AI compilation start]");
    if ( pStart )
    {
        pStart += strlen("[AI compilation start]");
        char * pEnd = NULL;
        if ( pStart )
		{
			tok = strtok(pStart, sep);
			while(tok != NULL)
			{
				log = strstr(tok, "Error detected");
				if(log)
				{
					log = (char*)malloc(strlen(tok));
					strcpy(log, tok);
					break;
				}
				tok = strtok(NULL, sep);
			}			
		}
    }	
	return log;
/*	char * log = "";
    char * pStart = strstr(_pBuffer, "[AI compilation start]");
    if ( pStart )
    {
        pStart += strlen("[AI compilation start]");
        char * pEnd = NULL;
        if ( pStart )
            pEnd = strstr(pStart, "[AI compilation failure]");
        if ( pEnd )
        {
			log = (char*)malloc(pEnd-pStart);				
			strncpy(log, pStart, pEnd-pStart);
			log[pEnd-pStart] = '\0';            
        }
    }
	return log;
*/
}

// stores the AI bug description to a file
void cl_Desdac::StoreAIBugToFile(const char* _pzMapKey, const char* sz_AIBug)
{
    FILE * pLog = fopen(kaz_DESDAC_AIBugsFile, "a");
    if ( pLog )
    {
        fprintf(pLog, "Error while running %s\n", pzSimpleMapName(_pzMapKey) );
        fprintf(pLog, "%s\n", sz_AIBug);

        fclose(pLog);
        flushall();
    }
}


void cl_Desdac::CreateResultFile(const ChangeListResult* _pst_Results)
{
    // get times
    CTime oEndTime = CTime::GetCurrentTime();

    CTime oStartTime;
    bool bGet = bGetFileWriteTime( kaz_DESDAC_RequestFile ,oStartTime);
    if(!bGet)
    {   oStartTime = oEndTime;
    }

    // write file
    FILE * pResult = fopen(kaz_DESDAC_ResultsTempFile, "w+");
    if ( pResult )
    {
        // Write Engine version used for test
        fprintf(pResult, kaz_DESDAC_EngineVersion "\n", mi_EnginePCVersion, mi_EnginePCPatch, mi_EnginePCCompilNb);

        // Write start/end times
        fprintf(pResult, "%d\\%d\\%d-%d:%d:%d\n",   oStartTime.GetYear(), oStartTime.GetMonth(),  oStartTime.GetDay(),
                                                    oStartTime.GetHour(), oStartTime.GetMinute(), oStartTime.GetSecond()   );
        fprintf(pResult, "%d\\%d\\%d-%d:%d:%d\n",   oEndTime  .GetYear(), oEndTime  .GetMonth(),  oEndTime  .GetDay(),
                                                    oEndTime  .GetHour(), oEndTime  .GetMinute(), oEndTime  .GetSecond()   );

        // Write CL number
        fprintf(pResult, "%d\n",        _pst_Results->mo_LaunchedCL.mi_ChangeList   );

        // Write test Results
        fprintf(pResult, "AI = %d\n",			_pst_Results->mb_AiFailed  ? 0 : 1          );
        fprintf(pResult, "DB = %d\n",			_pst_Results->mb_DbFailed  ? 0 : 1          );
		fprintf(pResult, "Build Prev = %d\n",   _pst_Results->mb_BuildPrevFailed  ? 0 : 1   );
		fprintf(pResult, "C gen = %d\n",		_pst_Results->mb_AiToCFailed  ? 0 : 1		);
		fprintf(pResult, "Build = %d\n",		_pst_Results->mb_BuildFailed  ? 0 : 1		);
        fprintf(pResult, "%s\n",				_pst_Results->mo_LaunchedCL.msz_UserName    );
        fprintf(pResult, "%s\n",				_pst_Results->mo_LaunchedCL.msz_ClientName  );
        fprintf(pResult, "%d\n",				_pst_Results->mi_NbDbErrors                 );
        fprintf(pResult, "%d\n",				_pst_Results->mi_NbAiErrors                 );
		fprintf(pResult, "%d\n",				_pst_Results->mi_NbBuildPrevErrors          );
		fprintf(pResult, "%d\n",				_pst_Results->mi_NbBuildErrors		        );
		fprintf(pResult, "%s\n",				_pst_Results->msz_AiLog		                );
		fprintf(pResult, "%s\n",				_pst_Results->msz_AiToCLog	                );

		if(mb_AiToCTestingEnable)
			fprintf(pResult, "FullTest = %d\n",	1);
		else
			fprintf(pResult, "FullTest = %d\n",	0);

		fprintf(pResult, "State = %d\n",		_pst_Results->mb_StateOK ? 1 : 0            );

        // Write if a crash occurred
        fprintf(pResult, bYetiPCHasCrashed() ? "CRASHED\n" : "VALID\n");

        // result of map testing
        if(mi_NbMapToTestOnClient)
        {
            fprintf(pResult, "NbMaps = %d\n", mi_NbMapToTestOnClient);
            for ( int i = 0; i < mi_NbMapToTestOnClient; ++i )
            {
                st_MapToTest& rMap = mast_MapToTestOnClient[i];
                fprintf(pResult, "%s\n",    rMap.msz_MapKey);
                fprintf(pResult, "%d %d %d\n", rMap.me_LoadStatus, rMap.me_LaunchStatus, rMap.me_CloseStatus);
            }
        }

        fclose(pResult);
        flushall();
    }


    // Copy the log file to correct location (if exists)
    // Check if log file is existing
    char sz_Command[BIG_k_CommandLength];
    sprintf(sz_Command, "%s\\%s", msz_ExeAndLogPath, msz_LogFileName);
    FILE * pLog = fopen(sz_Command, "r");
    if ( pLog )
    {
        fclose(pLog);
        flushall();

        char sz_File1[BIG_k_NameLength];
        char sz_File2[BIG_k_NameLength];
        sprintf(sz_File1, "%s\\%s", msz_ExeAndLogPath, msz_LogFileName);
        sprintf(sz_File2, "%s\\%s", kaz_FolderResults, msz_LogFileName);
        MoveFileEx(sz_File1, sz_File2, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING /*| MOVEFILE_WRITE_THROUGH*/ );
    }

    char sz_CommandBuildPrevLog[BIG_k_CommandLength];
    sprintf(sz_CommandBuildPrevLog, "%s\\%s", msz_ExeAndLogPath, msz_BuildPrevLogFileName);
    FILE * pBuildPrevLog = fopen(sz_CommandBuildPrevLog, "r");
    if ( pBuildPrevLog )
    {
        fclose(pBuildPrevLog);
        flushall();

        char sz_File3[BIG_k_NameLength];
        char sz_File4[BIG_k_NameLength];
        sprintf(sz_File3, "%s\\%s", msz_ExeAndLogPath, msz_BuildPrevLogFileName);
        sprintf(sz_File4, "%s\\%s", kaz_FolderResults, msz_BuildPrevLogFileName);
        MoveFileEx(sz_File3, sz_File4, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING /*| MOVEFILE_WRITE_THROUGH*/ );
    }

    char sz_CommandBuild[BIG_k_CommandLength];
    sprintf(sz_CommandBuild, "%s\\%s", msz_ExeAndLogPath, msz_BuildLogFileName);
    FILE * pLogBuild = fopen(sz_CommandBuild, "r");
    if ( pLogBuild )
    {
        fclose(pLogBuild);
        flushall();

        char sz_File5[BIG_k_NameLength];
        char sz_File6[BIG_k_NameLength];
        sprintf(sz_File5, "%s\\%s", msz_ExeAndLogPath, msz_BuildLogFileName);
        sprintf(sz_File6, "%s\\%s", kaz_FolderResults, msz_BuildLogFileName);
        MoveFileEx(sz_File5, sz_File6, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING /*| MOVEFILE_WRITE_THROUGH*/ );
    }

    // copy temporary result file to real result file
    CopyFile(kaz_DESDAC_ResultsTempFile, kaz_DESDAC_ResultsFile, FALSE);

    // Delete temporary result file
    DeleteFile(kaz_DESDAC_ResultsTempFile);
}



// Check end of global tests on yeti
void cl_Desdac::TestYetiFinished()
{
    if( bTestPCisFinished() )
    {
        // test if a crash occurred
        TestCrashPC();

        // possible launch on xenon
        if( mb_MapTestingXenonEnable )
        {
            if( mb_XenonDVDEmuMode )
            {   
                LaunchDVDEmuXenon();
                Sleep(1000);
                LaunchOneMapXenon( mast_MapToTestOnClient[0].msz_MapKey );
            }
            else if( bCopyDataToXenon() )
            {
                Sleep(1000);
                LaunchOneMapXenon( mast_MapToTestOnClient[0].msz_MapKey );
                //LaunchXenon();
            }
            else
            {   
                // copy to xenon failed : abort remaining test
                mi_FirstRemainingMapToTestOnClient = mi_NbMapToTestOnClient;
                FinishTest();
            }
        }
        else
        {   FinishTest();
        }
    }


    if( bTestXENONisFinished() )
    {   FinishTest();
    }
}

// wait loop, including updates (so that desdac isn't completely freezed)
void cl_Desdac::Temporize(DWORD _uiSeconds )
{
    for( ; _uiSeconds!=0; _uiSeconds--)
    {   
        Sleep(1000);
        UpdateStatusFile();
        // RunOneFrame();   // update desdac
    }
}

void cl_Desdac::FinishTest()
{
    // init results object
    ChangeListResult st_Results;
    st_Results.Reset();
    st_Results.mo_LaunchedCL    = mst_CurrentCl;


    // Analyze log PC
    AnalyzeLogFile(&mst_CurrentCl, &st_Results);

    // Analyze log Xenon, if needed
    if( mst_CurrentCl.mul_Options & k_Option_LaunchXenon )
    {    AnalyzeLogFileXenon_MapPerMapTest(&mst_CurrentCl, &st_Results);
    }

    if( !bIsYetiRunning() )    // when test map is activated, yeti PC or Xenon might be relaunched in "AnalyzeLogFile" funcs
    {
        CreateResultFile(&st_Results);
        UpdateClientStatistics();

        // Delete the request file (and therefore indicate that this client is available for a new request)
        DeleteFile(kaz_DESDAC_RequestFile);
    }
}


void cl_Desdac::UpdateClientStatistics()
{
    mi_NbClTreatedOnClient++;
    mul_BigfileSizeOnCLient = ulGetFileSize(msz_BigFileName);
}


// Generic test for async process termination
bool cl_Desdac::bTestAsyncProcessIsFinished(PROCESS_INFORMATION& _rProcInfo)
{
    bool bJustFinished = false;

    if ( WaitForSingleObject(_rProcInfo.hProcess,1) != WAIT_TIMEOUT )
    {
        DWORD ulExitCode;
        GetExitCodeProcess(_rProcInfo.hProcess, &ulExitCode);

        CloseHandle( _rProcInfo.hProcess );
        CloseHandle( _rProcInfo.hThread );

        bJustFinished = true;
    }

    DEBUG_EVENT DebugEv;                   // debugging event information 
    DWORD dwContinueStatus = DBG_CONTINUE; // exception continuation 
    if ( WaitForDebugEvent(&DebugEv, INFINITE) )
    {
        switch ( DebugEv.dwDebugEventCode ) 
        { 
        case EXCEPTION_DEBUG_EVENT: 
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;
        case CREATE_THREAD_DEBUG_EVENT:
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;
        case CREATE_PROCESS_DEBUG_EVENT:
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;
        case EXIT_THREAD_DEBUG_EVENT:
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;
        case EXIT_PROCESS_DEBUG_EVENT:
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;
        case LOAD_DLL_DEBUG_EVENT:
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;
        case UNLOAD_DLL_DEBUG_EVENT:
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;
        case OUTPUT_DEBUG_STRING_EVENT:
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;
        case RIP_EVENT:
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;
        default:
            printf("DEFAULT\n");
            break;
        }
        ContinueDebugEvent(DebugEv.dwProcessId, DebugEv.dwThreadId, dwContinueStatus); 
    }

    return bJustFinished;
}


// Test finished - PC
bool cl_Desdac::bTestPCisFinished()
{
    bool bJustFinished = false;

    if ( mb_YetiPCisRunning )
    {
        if ( bTestAsyncProcessIsFinished(mst_Async_ProcInfo) )
        {
            mb_YetiPCisRunning = false;
            bJustFinished = true;
        }
    }

    if ( mb_VisualIsRunning )
    {
        if ( bTestAsyncProcessIsFinished(mst_Async_ProcInfo_Visual) )
        {
            mb_VisualIsRunning = false;
            bJustFinished = true;
        }
    }

    if ( mb_VssIsRunning )
    {
        if ( bTestAsyncProcessIsFinished(mst_Async_ProcInfo_VSS) )
        {
            mb_VssIsRunning = false;
            bJustFinished = true;
        }
    }

    return bJustFinished;
}



//
bool cl_Desdac::bCopyDataToXenon()
{
#ifdef DEBUG
    // !!!!! TTY TMP TEST !!!!!
    //return true;    // skip bigfile copy
#endif

    // start
    mb_CopyToXENONisRunning = true;


    // Connect to Xenon
    HRESULT hr = DmSetXboxName(msz_XenonKitName);
    bool bSucceed = ( XBDM_NOERR == hr );

    // copy the files 
    if( bSucceed )
    {
        bSucceed = bCopyFileToXenon(msz_BigFileName, msz_XenonBigfileName);

        if(!bSucceed)
        {
            // try to reboot
            mb_XENONisRebooting = true;
            DmReboot( DMBOOT_COLD );
            Temporize(mul_RebootTimeXenon); // tempo for Xenon reboot
            mb_XENONisRebooting = false;

            // try copying again
            bSucceed = bCopyFileToXenon(msz_BigFileName, msz_XenonBigfileName);
        }
    }

    // end
    mb_CopyToXENONisRunning = false;
    return bSucceed;
}

bool cl_Desdac::bCopyFileToXenon(const char* _pzSrcFile, const char* _pzTargetFile )
{
    bool bOk = true;

    // Get the source file size
    DWORD ul_FileSize = ulGetFileSize(_pzSrcFile);
    if ( 0 == ul_FileSize )
        return false;


    // Create the target file on Console
    HRESULT hr = DmSetFileSize(_pzTargetFile, 0, CREATE_ALWAYS);
    if ( XBDM_NOERR != hr )
        return false;


    // Open the source file to read
    FILE *pFile = fopen(_pzSrcFile, "rb");
    if ( NULL == pFile )
        return false;


    // Allocate the transfer buffer
    char *pc_Buffer  = (char *)malloc(k_ulXenonTransferBlockSize);
    float fIncrement = (float)(k_ulXenonTransferBlockSize*100)/(float)ul_FileSize;


    // Copy block by block
    DWORD ul_Offset = 0;
    mf_CopyPercentProgress = 0.0f;
    do {
        DWORD ul_DataSize = (ul_FileSize > k_ulXenonTransferBlockSize) ? k_ulXenonTransferBlockSize : ul_FileSize;

        size_t ui_ReadBytes = fread(pc_Buffer, ul_DataSize, 1, pFile);
        
        DWORD dw_WrittenBytes;
        hr = DmWriteFilePartial(_pzTargetFile, ul_Offset, (LPBYTE)pc_Buffer, ul_DataSize, &dw_WrittenBytes);
        assert(ul_DataSize == dw_WrittenBytes);

        if ( XBDM_NOERR != hr )
        {
            // TODO: treat error
            bOk = false;
            break;
        }

        mf_CopyPercentProgress += fIncrement;
        UpdateStatusFile();
        // RunOneFrame();

        ul_FileSize -= ul_DataSize;
        ul_Offset += ul_DataSize;
    }
    while( 0 != ul_FileSize );

    fclose(pFile);

    free(pc_Buffer);
    return bOk;
}

// Copy the given folder to the Xenon. Not recursive (sub folders not copied)
bool cl_Desdac::bCopyFolderToXenon(const char* _pzSrcPath, const char* _pzTargetPathXenon )
{
    char sz_pathFilter[BIG_k_NameLength];
    sprintf(sz_pathFilter, "%s//*.*", _pzSrcPath );

    WIN32_FIND_DATA findFileData; 
    HANDLE hFind = FindFirstFile(sz_pathFilter, &findFileData);

    BOOL bNext = (hFind != INVALID_HANDLE_VALUE);
    bool bCopyOk = (bNext == TRUE);
    while( bNext )
    {
        if( ! (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
        {   
            char sz_source[BIG_k_NameLength];
            sprintf(sz_source, "%s\\%s", _pzSrcPath, findFileData.cFileName );

            char sz_target[BIG_k_NameLength];
            sprintf(sz_target, "%s\\%s", _pzTargetPathXenon, findFileData.cFileName );
            
            bCopyOk = bCopyOk && bCopyFileToXenon(sz_source, sz_target);
        }
        bNext = FindNextFile(hFind, &findFileData);
    }

    return bCopyOk;
}

//
void cl_Desdac::LaunchXenon()
{
    char az_Command[BIG_k_CommandLength];
    char az_YetiXenonArguments[BIG_k_CommandLength];

    // arguments for yeti
    //sprintf(az_YetiXenonArguments, "/key 0xcf01ec83" );   // just for temp test
    sprintf(az_YetiXenonArguments, "/AutoTestMaps /MapTestTime %.1f", mf_MapTestTimeXenon );

    // build command
    sprintf(az_Command, "%s %s %s", msz_XenonLauncherName, msz_XenonExeName, az_YetiXenonArguments );

    // *** create process ***
    memset(&mst_Async_StartUpInfo,0,sizeof(STARTUPINFO));
    mst_Async_StartUpInfo.cb = sizeof(STARTUPINFO);
    mb_YetiXENONisRunning = CreateProcess(NULL, az_Command, NULL, NULL, FALSE, 0/*DEBUG_PROCESS*/, NULL, msz_XenonLauncherPath,
        &mst_Async_StartUpInfo,
        &mst_Async_ProcInfo) ? true : false;
}

void cl_Desdac::LaunchOneMapXenon(const char* _pszMapKey)
{
    char az_Command[BIG_k_CommandLength];
    char az_YetiXenonArguments[BIG_k_CommandLength];

    // arguments for yeti
    sprintf(az_YetiXenonArguments, "/AutoTestMap %s /MapTestTime %.1f" , _pszMapKey, mf_MapTestTimeXenon );

    // build command
    if(mb_XenonDVDEmuMode)
    {   sprintf(az_Command, "%s %s %s", msz_XenonLauncherName, msz_XenonExeName, az_YetiXenonArguments );
    }
    else
    {   sprintf(az_Command, "%s %s %s", msz_XenonLauncherName, msz_XenonExeName, az_YetiXenonArguments );
    }

    // *** create process ***
    memset(&mst_Async_StartUpInfo,0,sizeof(STARTUPINFO));
    mst_Async_StartUpInfo.cb = sizeof(STARTUPINFO);
    mb_YetiXENONisRunning = CreateProcess(NULL, az_Command, NULL, NULL, FALSE, 0/*DEBUG_PROCESS*/, NULL, msz_XenonLauncherPath,
        &mst_Async_StartUpInfo,
        &mst_Async_ProcInfo) ? true : false;
}


void cl_Desdac::LaunchDVDEmuXenon()
{
    char az_Command[BIG_k_CommandLength];

    // build command
    sprintf(az_Command, "D:\\Boulot\\Projets\\GR3\\yti_sources\\main\\tools\\Perforce\\AutoCompiler\\AutoCompiler\\release\\Bigfile\\VIDE.exe /desdac"  );

    // *** create process ***
    memset(&mst_Async_StartUpInfo_DVDEmu,0,sizeof(STARTUPINFO));
    mst_Async_StartUpInfo_DVDEmu.cb = sizeof(STARTUPINFO);
    bool mb_DVDEmuXenonIsRunning = CreateProcess(NULL, az_Command, NULL, NULL, FALSE, 0/*DEBUG_PROCESS*/, NULL, msz_XenonLauncherPath,
        &mst_Async_StartUpInfo_DVDEmu,
        &mst_Async_ProcInfo_DVDEmu) ? true : false;
}



// Test finished - XENON
bool cl_Desdac::bTestXENONisFinished()
{
    bool bJustFinished = false;

    if( mb_YetiXENONisRunning )
    {
        if ( bTestAsyncProcessIsFinished(mst_Async_ProcInfo) )
        {
            mb_YetiXENONisRunning = false;
            bJustFinished = true;
        }
    }

    return bJustFinished;
}



// Launch yeti PC
void cl_Desdac::LaunchVersionGenerationAsync(ChangeListToLaunch * _pst_ChangeList)
{
    char az_Command[BIG_k_CommandLength];
	char az_CommandVisual[BIG_k_CommandLength];
	char az_CommandAItoC[BIG_k_CommandLength];
	char az_CommandLaunchMap[BIG_k_CommandLength];	
    char az_Arguments[BIG_k_CommandLength];
	char az_ArgumentsVisual[BIG_k_CommandLength];
	char az_ArgumentsAItoC[BIG_k_CommandLength];
	char az_LaunchMap[BIG_k_CommandLength];
    char sz_TmpBuf[BIG_k_NameLength];

/************* -Get CL ****************/
	sprintf(az_Arguments, "%s", msz_BigFileName );
    if( _pst_ChangeList->mul_Options & k_Option_GetFromPerforce )
    {
		sprintf(sz_TmpBuf, " /sync:%d", _pst_ChangeList->mi_ChangeList);
        strcat(az_Arguments, sz_TmpBuf);
    }	
    sprintf(az_Command, "%s %s", msz_ExeName, az_Arguments);
	
    // *** create jade process ***
    memset(&mst_Async_StartUpInfo,0,sizeof(STARTUPINFO));
    mst_Async_StartUpInfo.cb = sizeof(STARTUPINFO);
    mb_YetiPCisRunning = CreateProcess(NULL, az_Command, NULL, NULL, FALSE, 0, NULL, ".", &mst_Async_StartUpInfo, &mst_Async_ProcInfo) ? true : false;

	while(WaitForSingleObject(mst_Async_ProcInfo.hProcess,1) == WAIT_TIMEOUT);	

/**************************************/

/************* Compile AI ****************/	
	sprintf(az_Arguments, "%s", msz_BigFileName );
    if( _pst_ChangeList->mul_Options & k_Option_CompileAi )
    {
		sprintf(sz_TmpBuf, " %s", "/C_prefabs/_prefabs_sound/_prefabs_sound.wol");
        strcat(az_Arguments, sz_TmpBuf);
    }	

	// log
	strcat(az_Arguments, " /Z:X:\\");
    sprintf(msz_LogFileName, kaz_DESDAC_LogFile_CLNB, _pst_ChangeList->mi_ChangeList);
    strcat(az_Arguments, msz_LogFileName);

    sprintf(az_Command, "%s %s", msz_ExeName, az_Arguments);
	
    // *** create jade process ***
    memset(&mst_Async_StartUpInfo,0,sizeof(STARTUPINFO));
    mst_Async_StartUpInfo.cb = sizeof(STARTUPINFO);
    mb_YetiPCisRunning = CreateProcess(NULL, az_Command, NULL, NULL, FALSE, 0/*DEBUG_PROCESS*/, NULL, ".",
        &mst_Async_StartUpInfo,
        &mst_Async_ProcInfo) ? true : false;

	while(WaitForSingleObject(mst_Async_ProcInfo.hProcess,1) == WAIT_TIMEOUT);	
/**************************************/
	
	// launchmap PC
	if(mb_MapTestingEnable && !bAiErrorDetected())
	{
		FILE* pMaplist = fopen("R4_maplist.txt", "rt");
		if(pMaplist)
		{
			int iFile = _fileno(pMaplist);
			int iFileSize = _filelength(iFile);
			char * pBuffer = (char*)malloc(iFileSize);
			if ( !pBuffer )
			{
				fclose(pMaplist);				
				return;
			}

			fread(pBuffer, 1, iFileSize, pMaplist);
			fclose(pMaplist);
			flushall();
			
			char* tok;
			char sep[] = "\n";
			mi_NbMapToTestOnClient = 0;
			tok = strtok(pBuffer, sep);
			while(tok != NULL)
			{
				// Jade process with map name = tok
				sprintf(az_LaunchMap, "%s /L%s", msz_BigFileName, tok);

				// log
				strcat(az_LaunchMap, " /Z:X:\\");				
				strcat(az_LaunchMap, msz_LogFileName);				
				sprintf(az_CommandLaunchMap, "%s %s", msz_ExeName, az_LaunchMap);
				
				// *** create yeti process ***
				memset(&mst_Async_StartUpInfo,0,sizeof(STARTUPINFO));
				mst_Async_StartUpInfo.cb = sizeof(STARTUPINFO);
				mb_YetiPCisRunning = CreateProcess(NULL, az_CommandLaunchMap, NULL, NULL, FALSE, 0/*DEBUG_PROCESS*/, NULL, ".",
					&mst_Async_StartUpInfo,
					&mst_Async_ProcInfo) ? true : false;

				while(WaitForSingleObject(mst_Async_ProcInfo.hProcess,1) == WAIT_TIMEOUT);	

				tok = strtok(NULL, sep);
				mi_NbMapToTestOnClient++;
			}						
		}
		// all maps in maplist file
	}

	if(mb_AiToCTestingEnable)
	{
		//if( _pst_ChangeList->mul_Options & k_Option_CompileAiToC )
		//{
			if(!bAiErrorDetected())
			{
				// Get VSS
				memset(&mst_Async_StartUpInfo_VSS,0,sizeof(STARTUPINFO));
				mst_Async_StartUpInfo_VSS.cb = sizeof(STARTUPINFO);
				mb_VssIsRunning = CreateProcess("GetSourcesVSS_ALL.bat", NULL, NULL, NULL, FALSE, 0/*DEBUG_PROCESS*/, NULL, ".",
					&mst_Async_StartUpInfo_VSS,
					&mst_Async_ProcInfo_VSS) ? true : false;

				while(WaitForSingleObject(mst_Async_ProcInfo_VSS.hProcess,1) == WAIT_TIMEOUT);	


				// Build before C files generation

				// *** create Visual process ***
				sprintf(msz_BuildPrevLogFileName, kaz_DESDAC_BuildPrevLogFile_CLNB, _pst_ChangeList->mi_ChangeList);					
				sprintf(az_ArgumentsVisual, " %s%s %s %s", msz_JadeSourcesPath, kaz_DESDAC_JadeSolFile, "/build \"debug editors\" /project \"MainEdi\"", "/out X:\\");
				strcat(az_ArgumentsVisual, msz_BuildPrevLogFileName);

				sprintf(az_CommandVisual, "%s", "C:\\Program Files\\Microsoft Visual Studio .NET 2003\\Common7\\IDE\\devenv.exe");					
				strcat(az_CommandVisual, az_ArgumentsVisual);

				memset(&mst_Async_StartUpInfo_Visual,0,sizeof(STARTUPINFO));
				mst_Async_StartUpInfo_Visual.cb = sizeof(STARTUPINFO);
				mb_VisualIsRunning = CreateProcess(NULL, az_CommandVisual, NULL, NULL, FALSE, 0/*DEBUG_PROCESS*/, NULL, ".",
					&mst_Async_StartUpInfo_Visual,
					&mst_Async_ProcInfo_Visual) ? true : false;

				while(WaitForSingleObject(mst_Async_ProcInfo_Visual.hProcess,1) == WAIT_TIMEOUT);	

				// Si pas d'erreur de Build, continue, sinon stop
				if(iBuildPrevErrorDetected() <= 0)
				{
					/************* C files creation ****************/
					sprintf(az_ArgumentsAItoC, "%s /Y(%s%s) %s", msz_BigFileName, msz_JadeSourcesPath, kaz_DESDAC_AIinterpFolder, "/C_prefabs/_prefabs_sound/_prefabs_sound.wol");

					// log
					strcat(az_ArgumentsAItoC, " /Z:X:\\");				
					strcat(az_ArgumentsAItoC, msz_LogFileName);
					
					sprintf(az_CommandAItoC, "%s %s", msz_ExeName, az_ArgumentsAItoC);
					
					// *** create jade process ***
					memset(&mst_Async_StartUpInfo,0,sizeof(STARTUPINFO));
					mst_Async_StartUpInfo.cb = sizeof(STARTUPINFO);
					mb_YetiPCisRunning = CreateProcess(NULL, az_CommandAItoC, NULL, NULL, FALSE, 0/*DEBUG_PROCESS*/, NULL, ".",
						&mst_Async_StartUpInfo,
						&mst_Async_ProcInfo) ? true : false;

					while(WaitForSingleObject(mst_Async_ProcInfo.hProcess,1) == WAIT_TIMEOUT);				
					/**********************************************/
					
					if(!bAiToCErrorDetected())
					{
						// si pas d'erreur lors de la generation des IA en C... on build la solution à nouveau
						sprintf(msz_BuildLogFileName, kaz_DESDAC_BuildLogFile_CLNB, _pst_ChangeList->mi_ChangeList);					
						sprintf(az_ArgumentsVisual, " %s%s %s %s", msz_JadeSourcesPath, kaz_DESDAC_JadeSolFile, "/build \"debug editors\" /project \"MainEdi\"", "/out X:\\");
						strcat(az_ArgumentsVisual, msz_BuildLogFileName);

						sprintf(az_CommandVisual, "%s", "C:\\Program Files\\Microsoft Visual Studio .NET 2003\\Common7\\IDE\\devenv.exe");					
						strcat(az_CommandVisual, az_ArgumentsVisual);
						memset(&mst_Async_StartUpInfo_Visual,0,sizeof(STARTUPINFO));
						mst_Async_StartUpInfo_Visual.cb = sizeof(STARTUPINFO);
						
						mb_VisualIsRunning = CreateProcess(NULL, az_CommandVisual, NULL, NULL, FALSE, 0/*DEBUG_PROCESS*/, NULL, ".",
							&mst_Async_StartUpInfo_Visual,
							&mst_Async_ProcInfo_Visual) ? true : false;

						while(WaitForSingleObject(mst_Async_ProcInfo_Visual.hProcess,1) == WAIT_TIMEOUT);	
					}
				}
			}
		//}
	}
}

bool cl_Desdac::bAiToCErrorDetected()
{
	bool bSol = false;
	char logFile[256];
	sprintf(logFile, "X:\\");
	strcat(logFile, msz_LogFileName); 
	FILE * pLog = fopen(logFile, "rt");
    if ( !pLog )
        return true;

    int iFile = _fileno(pLog);
    int iFileSize = _filelength(iFile);
    char * pBuffer = (char*)malloc(iFileSize);
    if ( !pBuffer )
    {
        fclose(pLog);        
        return true;
    }

    fread(pBuffer, 1, iFileSize, pLog);
    fclose(pLog);
    flushall();

    char * pStart = strstr(pBuffer, "[AI compilation success]");
    if ( !pStart )
	{
		free(pBuffer);
		return true;
	}
	
	pStart += strlen("[AI compilation success]");
	pStart = strstr(pStart, "[AI compilation success]");
	if(!pStart)
		bSol = true;
    
	free(pBuffer);    
    flushall();	

	return bSol;
}


int cl_Desdac::iBuildErrorDetected()
{	
	char logFile[256];
	sprintf(logFile, "X:\\");
	strcat(logFile, msz_BuildLogFileName); 
	FILE * pLog = fopen(logFile, "rt");
    if ( !pLog )
    {		
        flushall();        
        return -1;
    }

    int iFile = _fileno(pLog);
    int iFileSize = _filelength(iFile);
    char * pBuffer = (char*)malloc(iFileSize+1);
	fread(pBuffer, 1, iFileSize, pLog);
	int i;
	for(i=0 ; i<iFileSize ; i++)
		if(pBuffer[i] == '\0')
			pBuffer[i] = '_';
	pBuffer[iFileSize] = '\0';

	if ( !pBuffer )   
        return -1;

    fclose(pLog);
    flushall();

    char * pStart = strstr(pBuffer, "---------------------- Done ----------------------");
	 
    if ( !pStart )
	{
		free(pBuffer);
		return -1;
	}

	pStart += strlen("---------------------- Done ----------------------\n\n");
	
    int iNbSuccess	= -1;
	int iNbFailed	= -1;
	int iNbSkipped	= -1;
    sscanf(pStart, "    Build: %d succeeded, %d failed, %d skipped", &iNbSuccess, &iNbFailed, &iNbSkipped);
     
	free(pBuffer);
    flushall();	

	return iNbFailed;	
}


int cl_Desdac::iBuildPrevErrorDetected()
{	
	char logFile[256];
	sprintf(logFile, "X:\\");
	strcat(logFile, msz_BuildPrevLogFileName); 
	FILE * pLog = fopen(logFile, "rt");
    if ( !pLog )
        return -1;

    int iFile = _fileno(pLog);
    int iFileSize = _filelength(iFile);
    char * pBuffer = (char*)malloc(iFileSize+1);
	fread(pBuffer, 1, iFileSize, pLog);
	int i;
	for(i=0 ; i<iFileSize ; i++)
		if(pBuffer[i] == '\0')
			pBuffer[i] = '_';
	pBuffer[iFileSize] = '\0';

	if ( !pBuffer )
    {
		free(pBuffer);
        fclose(pLog);        
        return -1;
    }

    fclose(pLog);
    flushall();

    char * pStart = strstr(pBuffer, "---------------------- Done ----------------------");
	 
    if ( !pStart )
	{
		free(pBuffer);
		return -1;
	}

	pStart += strlen("---------------------- Done ----------------------\n\n");
	
    int iNbSuccess	= -1;
	int iNbFailed	= -1;
	int iNbSkipped	= -1;
    sscanf(pStart, "    Build: %d succeeded, %d failed, %d skipped", &iNbSuccess, &iNbFailed, &iNbSkipped);
     
	free(pBuffer);
    flushall();	

	return iNbFailed;	
}

bool cl_Desdac::bAiErrorDetected()
{
	bool bSol = false;
	char logFile[256];
	sprintf(logFile, "X:\\");
	strcat(logFile, msz_LogFileName); 
	FILE * pLog = fopen(logFile, "rt");
    if ( !pLog )
        return false;

    int iFile = _fileno(pLog);
    int iFileSize = _filelength(iFile);
    char * pBuffer = (char*)malloc(iFileSize);
    if ( !pBuffer )
    {
        fclose(pLog);        
        return false;
    }

    fread(pBuffer, 1, iFileSize, pLog);
    fclose(pLog);
    flushall();

    char * pStart = strstr(pBuffer, "[AI compilation success]");
    if ( !pStart )
		bSol = true;

    free(pBuffer);    
    flushall();	

	return bSol;
}


//
void cl_Desdac::AnalyzeAndLaunchRequest(char * _pz_RequestFile)
{
    char sz_FileName[BIG_k_NameLength];
    //sprintf(sz_FileName, "\\\\mtp-mhidrio\\debug\\sys\\request.txt");//"%s\\%s", kaz_FolderSystem, _pz_RequestFile);
	sprintf(sz_FileName,"%s\\%s", kaz_FolderSystem, _pz_RequestFile);
    FILE * pRequest = fopen(sz_FileName, "r");
    int iChangeList = -1;
    if ( pRequest )
    {
        fscanf(pRequest, "%d\n", &mst_CurrentCl.mi_ChangeList);    
        fscanf(pRequest, "%s\n", mst_CurrentCl.msz_UserName);    
        fscanf(pRequest, "%s\n", mst_CurrentCl.msz_ClientName);    
        fscanf(pRequest, "%d\n", &mst_CurrentCl.mul_Options);            

        // Map test inits
        mi_NbMapToTestOnClient = 0;
        mi_FirstRemainingMapToTestOnClient = 0;

		// uniquement le serveur (finalement, pas besoin de creer ce fichier
        if ( (mst_CurrentCl.mul_Options & k_Option_LoadMap) || (mst_CurrentCl.mul_Options & k_Option_LaunchXenon) )
        {
            fscanf(pRequest, "MapTesting %d\n", &mi_NbMapToTestOnClient);    

            // Create map testing file
            char sz_MapTestingFIleName[BIG_k_NameLength];
            sprintf(sz_MapTestingFIleName, "%s\\%s", msz_ExeAndLogPath, kaz_DESDAC_MapTestingFile);
            DeleteFile(sz_MapTestingFIleName);
            FILE * pMapTesting = fopen(sz_MapTestingFIleName, "w+");
            if ( pMapTesting )
            {
                fprintf(pMapTesting, "MapTesting %d\n", mi_NbMapToTestOnClient);
                for ( int m = 0; m < mi_NbMapToTestOnClient; ++m )
                {
                    char sz_Tmp[BIG_k_NameLength];
                    fgets(sz_Tmp, BIG_k_NameLength, pRequest);
                    fprintf(pMapTesting, "%s", sz_Tmp);

                    // also store the maps to be tested
                    st_MapToTest& rMap = mast_MapToTestOnClient[m];
                    rMap.ResetTestResults();    //init to "not tested"

                    strcpy( rMap.msz_MapKey , sz_Tmp );
                    rMap.msz_MapKey[ strlen(sz_Tmp)-1 ] = 0;     // remove carriage return
                }

                fclose(pMapTesting);
                flushall();
            }
        }

        fclose(pRequest);
        flushall();


        // reset 
        SetYetiPCHasCrashed(false);

        // Launch Yeti
        LaunchVersionGenerationAsync(&mst_CurrentCl);
    }
}

void cl_Desdac::SaveNumCL(int _iCL)
{
	int prevCL;
    char sz_CLFile[BIG_k_NameLength];
    sprintf(sz_CLFile, "%s", kaz_DESDAC_CLsavedFile);

    FILE * pRemoteFile = fopen(sz_CLFile, "wt");
    if ( pRemoteFile )
    {
		fscanf(pRemoteFile, "%d\n", &prevCL);

		if(_iCL > prevCL)
			fprintf(pRemoteFile,"%d", _iCL);

		fclose(pRemoteFile);	
		flushall();
	}	
}


// (server)
void cl_Desdac::AnalyzeAndTreatResults()
{
    for ( int i = 0; i < iGetNbClients(); ++i )
    {
        st_ClientPc& roClientI = mast_ClientPCs[i];
        const char* pzDesdacClientName = roClientI.msz_ClientName;

        // Open result file
        char sz_ResultFile[BIG_k_NameLength];
        sprintf(sz_ResultFile, kaz_DESDAC_RemoteResultsFile, pzDesdacClientName);

        FILE * pRemoteFile = fopen(sz_ResultFile, "r");
        if ( pRemoteFile )
        {
            ChangeListResult stCLres;
            stCLres.Reset();
            stCLres.mi_ClientIndex = i;

            // *** Read result file ***
            // Read Engine version used for test
            fscanf(pRemoteFile, kaz_DESDAC_EngineVersion "\n", &stCLres.mi_EnginePCVersion, &stCLres.mi_EnginePCPatch, &stCLres.mi_EnginePCCompilNb);

            // Read start/end times
            int  iY, iM, iD, iH, iMin, iSec;
            fscanf(pRemoteFile, "%d\\%d\\%d-%d:%d:%d\n", &iY,&iM,&iD,&iH,&iMin,&iSec);
            stCLres.mo_StartOfTestTime    = CTime(iY, iM, iD, iH, iMin, iSec);
            fscanf(pRemoteFile, "%d\\%d\\%d-%d:%d:%d\n", &iY,&iM,&iD,&iH,&iMin,&iSec);
            stCLres.mo_EndOfTestTime      = CTime(iY, iM, iD, iH, iMin, iSec);

            // Read CL number
            int iCL;
            fscanf(pRemoteFile, "%d\n", &iCL);
            stCLres.mo_LaunchedCL.mi_ChangeList = iCL;

            // Read test Results
            int iAiIsOK = -1;
            fscanf(pRemoteFile, "AI = %d\n", &iAiIsOK);
            stCLres.mb_AiFailed = (iAiIsOK == 0);

            int iDbIsOK = -1;
            fscanf(pRemoteFile, "DB = %d\n", &iDbIsOK);
            stCLres.mb_DbFailed = (iDbIsOK == 0);

            int iBuildPrevIsOK = -1;
            fscanf(pRemoteFile, "Build Prev = %d\n", &iBuildPrevIsOK);
            stCLres.mb_BuildPrevFailed = (iBuildPrevIsOK == 0);

            int iAiToCIsOK = -1;
            fscanf(pRemoteFile, "C gen = %d\n", &iAiToCIsOK);
            stCLres.mb_AiToCFailed = (iAiToCIsOK == 0);

            int iBuildIsOK = -1;
            fscanf(pRemoteFile, "Build = %d\n", &iBuildIsOK);
            stCLres.mb_BuildFailed = (iBuildIsOK == 0);

            fscanf(pRemoteFile, "%s\n", stCLres.mo_LaunchedCL.msz_UserName);
            fscanf(pRemoteFile, "%s\n", stCLres.mo_LaunchedCL.msz_ClientName);
            fscanf(pRemoteFile, "%d", &stCLres.mi_NbDbErrors);
            fscanf(pRemoteFile, "%d", &stCLres.mi_NbAiErrors);
			fscanf(pRemoteFile, "%d", &stCLres.mi_NbBuildPrevErrors);
			fscanf(pRemoteFile, "%d", &stCLres.mi_NbBuildErrors);
			stCLres.msz_AiLog = (char*)malloc(512);
			fscanf(pRemoteFile, "%s\n", stCLres.msz_AiLog);
			stCLres.msz_AiToCLog = (char*)malloc(512);
			fscanf(pRemoteFile, "%s\n", stCLres.msz_AiToCLog);
			
            int iFullTest = -1;
            fscanf(pRemoteFile, "FullTest = %d\n", &iFullTest);
            stCLres.mb_FullTest = (iFullTest == 1);

            int iState = -1;
            fscanf(pRemoteFile, "State = %d\n", &iState);
            stCLres.mb_StateOK = (iState == 1);

			if(stCLres.mb_StateOK)
				SaveNumCL(iCL);

            // Read if a crash occurred
            char sz_CrashedOrNot[k_UserLength];
            fscanf(pRemoteFile, "%s\n", sz_CrashedOrNot);
            stCLres.mb_Crashed = ( strncmp(sz_CrashedOrNot, "CRASHED", strlen("CRASHED")) == 0 );


            // results of map test
            if( bGetGlobalMapTesting() )
            {
                // Transfer possible aibug file
                char sz_File1[BIG_k_NameLength];
                char sz_File2[BIG_k_NameLength];
                sprintf(sz_File1, kaz_DESDAC_RemoteAIBugsFile, pzDesdacClientName);
                sprintf(sz_File2, "%s\\" kaz_DESDAC_AIBugsFile_CLNB, msz_OutputFolder, stCLres.mo_LaunchedCL.mi_ChangeList);
                MoveFileEx(sz_File1, sz_File2, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING /*| MOVEFILE_WRITE_THROUGH*/ );

                // Analyse results
                ResetMapTestResults();
                int iNbMapTested = 0;

                fscanf(pRemoteFile, "NbMaps = %d\n", &iNbMapTested);
                for ( int i = 0; i < iNbMapTested; ++i )
                {
                    // read name
                    char szMapKey[BIG_k_NameLength];
                    fgets(szMapKey, BIG_k_NameLength, pRemoteFile);
                    szMapKey[ strlen(szMapKey)-1 ] = 0;     // remove carriage return

                    // read results
                    int iLoadStatus   = E_StatusNotTested;
                    int iLaunchStatus = E_StatusNotTested;
                    int iCloseStatus  = E_StatusNotTested;
                    fscanf( pRemoteFile, "%d %d %d\n", &iLoadStatus, &iLaunchStatus, &iCloseStatus);

                    // find tested map in list. not optimal, but safe in case of map list change while server is running
                    for ( int j = 0; j < mi_NbMapToTest; ++j )
                    {
                        if(  0 == strcmp( mast_MapToTest[j].msz_MapKey, szMapKey )  )
                        {
                            const char* szMapName = mast_MapToTest[j].msz_MapName;

                            mast_MapToTest[j].me_LoadStatus   = (tde_Status)iLoadStatus;
                            mast_MapToTest[j].me_LaunchStatus = (tde_Status)iLaunchStatus;
                            mast_MapToTest[j].me_CloseStatus  = (tde_Status)iCloseStatus;

                            if( bIsFailedStatus((tde_Status)iLoadStatus) || bIsFailedStatus((tde_Status)iLaunchStatus) )
                            {   stCLres.mi_NbMapErrors++;
                            }

                            // there can be an associated crash dump. Move it
                            // results of Xenon map test
                            if( mb_MapTestingXenonEnable )
                            {
                                char sz_File1[BIG_k_NameLength];
                                char sz_File2[BIG_k_NameLength];
                                sprintf(sz_File1, kaz_DESDAC_RemoteXenonResultDumpFile_MapNB, pzDesdacClientName, i);
                                sprintf(sz_File2, "%s\\" kaz_DESDAC_XenonResultDumpFile_CLNB_MapName, msz_OutputFolder, iCL, szMapName );
                                MoveFileEx(sz_File1, sz_File2, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING );

                                sprintf(sz_File1, kaz_DESDAC_RemoteXenonResultScreenShotFile_MapNB, pzDesdacClientName, i);
                                sprintf(sz_File2, "%s\\" kaz_DESDAC_XenonResultScreenShotFile_CLNB_MapName, msz_OutputFolder, iCL, szMapName );
                                MoveFileEx(sz_File1, sz_File2, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING );

                                sprintf(sz_File1, kaz_DESDAC_RemoteXenonResultLogFile_MapNB, pzDesdacClientName, i);
                                sprintf(sz_File2, "%s\\" kaz_DESDAC_XenonResultLogFile_CLNB_MapName, msz_OutputFolder, iCL, szMapName );
                                BOOL bCrashed = MoveFileEx(sz_File1, sz_File2, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING );
                            }
                            
                            if( stCLres.mb_Crashed )
                            {
                                char sz_File1[BIG_k_NameLength];
                                char sz_File2[BIG_k_NameLength];
                                sprintf(sz_File1, kaz_DESDAC_RemoteCrashDumpFile_MapNB, pzDesdacClientName, i);
                                sprintf(sz_File2, "%s\\" kaz_DESDAC_CrashDumpFile_CLNB_MapName, msz_OutputFolder, iCL, szMapName );
                                MoveFileEx(sz_File1, sz_File2, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING );
                            }
                        }
                    }

                }

                stCLres.mb_MapTestFailed = (stCLres.mi_NbMapErrors > 0);
            }
            else
            {   // no map test: single crash dump
                // if yeti crashed, there should be a crash dump file
                if( stCLres.mb_Crashed )
                {
                    char sz_File1[BIG_k_NameLength];
                    char sz_File2[BIG_k_NameLength];
                    sprintf(sz_File1, kaz_DESDAC_RemoteCrashDumpFile, pzDesdacClientName);
                    sprintf(sz_File2, "%s\\" kaz_DESDAC_CrashDumpFile_CLNB, msz_OutputFolder, iCL);
                    MoveFileEx(sz_File1, sz_File2, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING /*| MOVEFILE_WRITE_THROUGH*/ );
                }
            }

            fclose(pRemoteFile);
            flushall();

            SetchangeListResultValue(stCLres);


            // Delete result file (not needed anymore)
            char sz_FileName[BIG_k_CommandLength];
            sprintf(sz_FileName, kaz_DESDAC_RemoteResultsFile, pzDesdacClientName);
            DeleteFile(sz_FileName);

            // Copy the remote log file into output folder
            // Check if remote log is existing before copying it
            sprintf(sz_FileName, kaz_DESDAC_RemoteLogFile, pzDesdacClientName, iCL);
            FILE * pLog = fopen(sz_FileName, "r");
            if ( pLog )
            {
                fclose(pLog);
                flushall();
                char sz_File1[BIG_k_NameLength];
                char sz_File2[BIG_k_NameLength];
                sprintf(sz_File1, kaz_DESDAC_RemoteLogFile, pzDesdacClientName, iCL);
                sprintf(sz_File2, "%s\\" kaz_DESDAC_LogFile_CLNB, msz_OutputFolder, iCL);
                CopyFile(sz_File1, sz_File2, FALSE);
                stCLres.mb_LogExists = true;
            }

            // Delete result file (not needed anymore)
            char sz_FileNameBuildPrev[BIG_k_CommandLength];

            // Copy the remote log file into output folder
            // Check if remote log is existing before copying it
            sprintf(sz_FileNameBuildPrev, kaz_DESDAC_RemoteBuildPrevFile, pzDesdacClientName, iCL);
            FILE * pBuildPrevLog = fopen(sz_FileNameBuildPrev, "r");
            if ( pBuildPrevLog )
            {
                fclose(pBuildPrevLog);
                flushall();
                char sz_File3[BIG_k_NameLength];
                char sz_File4[BIG_k_NameLength];
                sprintf(sz_File3, kaz_DESDAC_RemoteBuildPrevFile, pzDesdacClientName, iCL);
                sprintf(sz_File4, "%s\\" kaz_DESDAC_BuildPrevLogFile_CLNB, msz_OutputFolder, iCL);
                CopyFile(sz_File3, sz_File4, FALSE);
                stCLres.mb_LogExists = true;
            }

            // Copy the remote log file into output folder
            // Check if remote log is existing before copying it
			char sz_FileNameBuild[BIG_k_CommandLength];
            sprintf(sz_FileNameBuild, kaz_DESDAC_RemoteBuildLogFile, pzDesdacClientName,iCL);
            FILE * pBuildLog = fopen(sz_FileNameBuild, "r");
            if ( pBuildLog )
            {
                fclose(pBuildLog);
                flushall();
                char sz_File5[BIG_k_NameLength];
                char sz_File6[BIG_k_NameLength];
                sprintf(sz_File5, kaz_DESDAC_RemoteBuildLogFile, pzDesdacClientName, iCL);
                sprintf(sz_File6, "%s\\" kaz_DESDAC_BuildLogFile_CLNB, msz_OutputFolder, iCL);
                CopyFile(sz_File5, sz_File6, FALSE);
                stCLres.mb_LogExists = true;
            }

            // Get submit time
            stCLres.mb_SubmitTimeIsValid  = bGetChangeListSubmitTime(iCL, stCLres.mo_SubmitTime);


            // Update Html result files
            UpdateHtmlMainFile(stCLres);

            if( bGetGlobalMapTesting() )
            {   UpdateHtmlTestMapFile(stCLres);
            }

        }
    } // end  for
}



void cl_Desdac::UpdateHtmlMainFile( const ChangeListResult& _rCLres )
{
    // HTML modification
    char * pOldFileBuffer = NULL;

    char sz_HtmlFile[BIG_k_NameLength];
    sprintf(sz_HtmlFile, "%s\\%s", msz_OutputFolder, kaz_DESDAC_Html);
    FILE * pHtml = fopen(sz_HtmlFile, "rt");
    int iOldBufSize = 0;
    int iFileSize = 0;
    if ( pHtml )
    {
        int iFile = _fileno(pHtml);
        iFileSize = _filelength(iFile);
        iOldBufSize = iFileSize;
        pOldFileBuffer = (char*)malloc(iFileSize);
        fread(pOldFileBuffer, 1, iFileSize, pHtml);
        fclose(pHtml);
        flushall();
    }

    char sz_NewHtmlFile[BIG_k_NameLength];
    sprintf(sz_NewHtmlFile, "%s\\%s", msz_OutputFolder, kaz_DESDAC_NewHtml);
    pHtml = fopen(sz_NewHtmlFile, "wt");
    if ( pHtml )
    {
        int iChangeList = _rCLres.mo_LaunchedCL.mi_ChangeList;
        const char* pzDesdacClientName = mast_ClientPCs[_rCLres.mi_ClientIndex].msz_ClientName;

        /////////
        // Header
        char sz_Ver[15];
        bGetExeVersion(sz_Ver);
        fprintf(pHtml, "<html>\n<head>\n");
		fprintf(pHtml, "<META HTTP-EQUIV=Refresh CONTENT=10; URL=\"desdac.htm\">\n");
		fprintf(pHtml,"<title>\n");
        fprintf(pHtml, "DESDAC %s: automated data-tests report\n", sz_Ver);
        fprintf(pHtml, "</title>\n</head>\n<pre>\n");

        fprintf(pHtml, "%s - ", msz_ProjectName);   // project Name (just for the record)

        if(mb_MapTestingEnable)
        {   fprintf(pHtml, "MAP TESTING " kaz_DESDAC_TARGET_PC);
        }
        else if(mb_MapTestingXenonEnable)
        {   fprintf(pHtml, "MAP TESTING " kaz_DESDAC_TARGET_XENON);
        }
        else
        {   fprintf(pHtml, "IA AND DATABASE CHECK");
        }

        fprintf(pHtml, " [%s %s]\n", msz_PerforceServer, msz_DepotBranch);  // depot & branch



        //////////////////////////////
        // Compute split of old buffer
        char * pOldFirstPart = NULL;
        char * pOldSecndPart = NULL;
        char * pEnd = NULL;
        if ( pOldFileBuffer )
        {
            // Look for the place to insert new CL
            int iCl = iChangeList+1;
            pOldSecndPart = pOldFileBuffer;
            while ( iCl > iChangeList )
            {
                pOldSecndPart = strstr(pOldSecndPart, "---");
                char * pTmp = strstr(pOldSecndPart, "CL : ");
                if ( pTmp )
                {
                    sscanf(pTmp, "CL : <b> %d", &iCl);
                }
                else 
                    iCl = iChangeList - 1;

                if ( iCl > iChangeList && !pOldFirstPart )
                    pOldFirstPart = strstr(pOldFileBuffer, "---");
                if ( iCl > iChangeList )
                    pOldSecndPart = pTmp;
            }

            pEnd = strstr(pOldFileBuffer, "</html>");
        }

        /////////////////////////////////
        // Write first part of old buffer
        if ( pOldFirstPart )
        {
            if ( pOldSecndPart )
            {
                fwrite(pOldFirstPart, 1, pOldSecndPart - pOldFirstPart, pHtml);
            }
            else if ( pEnd )
            {
                fwrite(pOldFirstPart, 1, pEnd - pOldFirstPart, pHtml);
            }
            else
            {
                fwrite(pOldFirstPart, 1, iFileSize - (pOldFirstPart - pOldFileBuffer), pHtml);
            }
        }

        ///////////////
        // Write new CL
        fprintf(pHtml, "\n------------------------------------------");
        fprintf(pHtml, kaz_DESDAC_HTML_CL_BOOKMARK "\n", iChangeList);
        fprintf(pHtml, kaz_DESDAC_HTML_CL_Black, iChangeList);


        // 
        // Global Status
/*        if(_rCLres.mb_Crashed)
        {   fprintf(pHtml, kaz_DESDAC_HTML_CRASHED);
        }
        else if ( _rCLres.mb_AiFailed || _rCLres.mb_DbFailed || ( (mb_MapTestingEnable||mb_MapTestingXenonEnable) && _rCLres.mb_MapTestFailed ) )
        {   fprintf(pHtml, kaz_DESDAC_HTML_FAILED);
        }
        else
        {   fprintf(pHtml, kaz_DESDAC_HTML_OK);
        }
*/
        // Name of the Submitter
        fprintf(pHtml, "(user:<b>%15s</b>) ", _rCLres.mo_LaunchedCL.msz_UserName);

        if ( !_rCLres.mb_Crashed )
        {
            // Database Status
/*            if ( _rCLres.mb_DbFailed )
            {    fprintf(pHtml, " DB: " kaz_DESDAC_HTML_FAILED_NB, _rCLres.mi_NbDbErrors);
            }
            else
            {    fprintf(pHtml, " DB: " kaz_DESDAC_HTML_OK_NB_ALIGN);
            }
*/
            // AI Status
            if ( _rCLres.mb_AiFailed )
			{
            	fprintf(pHtml, " AI: " kaz_DESDAC_HTML_FAILED_NB, _rCLres.mi_NbAiErrors);  
				fprintf(pHtml, " Build1: " kaz_DESDAC_HTML_NT);
				fprintf(pHtml, " Cgen: " kaz_DESDAC_HTML_NT);
				fprintf(pHtml, " Build2: " kaz_DESDAC_HTML_NT);
			}
            else
			{
                fprintf(pHtml, " AI: " kaz_DESDAC_HTML_OK_NB_ALIGN);
				
				if(_rCLres.mb_FullTest)
				{
					// Build prev Status  
					if(_rCLres.mb_BuildPrevFailed)
					{
						fprintf(pHtml, " Build1: " kaz_DESDAC_HTML_FAILED);
						fprintf(pHtml, " Cgen: " kaz_DESDAC_HTML_NT);
						fprintf(pHtml, " Build2: " kaz_DESDAC_HTML_NT);
					}
					else
					{
						fprintf(pHtml, " Build1: " kaz_DESDAC_HTML_OK_NB_ALIGN);
						if(_rCLres.mb_AiToCFailed)
						{
							fprintf(pHtml, " Cgen: " kaz_DESDAC_HTML_FAILED);
							fprintf(pHtml, " Build2: " kaz_DESDAC_HTML_NT);
						}
						else
						{
							fprintf(pHtml, " Cgen: " kaz_DESDAC_HTML_OK_NB_ALIGN);
							if(_rCLres.mb_BuildFailed)
								fprintf(pHtml, " Build2: " kaz_DESDAC_HTML_FAILED);
							else
								fprintf(pHtml, " Build2: " kaz_DESDAC_HTML_OK_NB_ALIGN);
						}
					}
				}
				else
				{
					fprintf(pHtml, " Build1: " kaz_DESDAC_HTML_NT);
					fprintf(pHtml, " Cgen: " kaz_DESDAC_HTML_NT);
					fprintf(pHtml, " Build2: " kaz_DESDAC_HTML_NT);
				}
			}
	

            // Map test status
 /*           if ( bGetGlobalMapTesting() )
            {
                if(_rCLres.mb_MapTestFailed)
                {    fprintf(pHtml, " MAPS: " kaz_DESDAC_HTML_FAILED_NB, _rCLres.mi_NbMapErrors);
                }
                else
                {    fprintf(pHtml, " MAPS: " kaz_DESDAC_HTML_OK_NB_ALIGN);
                }
            }
*/        }


        //
        // General infos
        fprintf(pHtml, " (");

        // Test info : engine version, client name, test end time, test duration
        fprintf(pHtml, "Test: v%d.%02d on %s. ", _rCLres.mi_EnginePCVersion, _rCLres.mi_EnginePCPatch, pzDesdacClientName);
        fprintf(pHtml, " End: <b>%s</b>", _rCLres.mo_EndOfTestTime.Format(kaz_DESDAC_DATETIMEFORMAT) );
        fprintf(pHtml, " [%s]", szFormatDuration(_rCLres.mo_EndOfTestTime - _rCLres.mo_StartOfTestTime) );

        // Submission time
        fprintf (   pHtml, "  Submitted:" );
        if ( _rCLres.mb_SubmitTimeIsValid )
        {   fprintf (   pHtml, " <b>%s</b>", _rCLres.mo_SubmitTime.Format(kaz_DESDAC_DATETIMEFORMAT) );
        }
        else
        {   fprintf(pHtml, " <b>undefined</b>" );
        }

        fprintf(pHtml, ")");
        
        // end of line 1
        fprintf(pHtml, "\n" );


        // start possible line 2
        bool bLine2 = false;

        // Link on Log
        if ( !_rCLres.mb_LogExists )  // security: there should be a log, but in case of early crash, there isn't
        {   
            fprintf(pHtml, "<font color=\"#C11B17\">(Warning: No log!) </font>");
            bLine2 = true;
        }
        else if ( _rCLres.mb_AiFailed || _rCLres.mb_DbFailed )
        {   
            
            fprintf(pHtml, "See log PC: <a href = \"" kaz_DESDAC_LogFile_CLNB "\" target = \"View Log\">" kaz_DESDAC_LogFile_CLNB "</a>. ", iChangeList, iChangeList);
            bLine2 = true;
		}
		else if(_rCLres.mb_FullTest)
		{
			// Build prev Status  
			if(_rCLres.mb_BuildPrevFailed)
			{
				fprintf(pHtml, "See log PC: <a href = \"" kaz_DESDAC_BuildPrevLogFile_CLNB "\" target = \"View Log\">" kaz_DESDAC_BuildPrevLogFile_CLNB "</a>. ", iChangeList, iChangeList);
				bLine2 = true;
			}
			else if(_rCLres.mb_BuildFailed)
			{
				fprintf(pHtml, "See log PC: <a href = \"" kaz_DESDAC_BuildLogFile_CLNB "\" target = \"View Log\">" kaz_DESDAC_BuildLogFile_CLNB "</a>. ", iChangeList, iChangeList);
				bLine2 = true;
			}
		}

        // Link on crash dump (except in case of map test: crash dump links are in detailed report, for each map)
        if ( _rCLres.mb_Crashed && (!mb_MapTestingEnable) )
        {   
            fprintf(pHtml, "<a href = \"" kaz_DESDAC_CrashDumpFile_CLNB "\" target = \"Crash Dump PC\">Crash Dump PC</a>. ", iChangeList);
            bLine2 = true;
        }


        // link on test map report
        if ( bGetGlobalMapTesting() && ( _rCLres.mb_MapTestFailed || _rCLres.mb_Crashed ) )
        {   
            fprintf(pHtml, "See <a href = \"" kaz_DESDAC_HtmlMapTest "#%d\" target = \"Map Test Page\">Map Test Page</a>", iChangeList );
            fprintf(pHtml, _rCLres.mb_Crashed ? " for crash dumps. " : ". " );
            bLine2 = true;
        }

        //end of line 2
        if(bLine2)
            fprintf(pHtml, "\n");




        /////////////////////////////////////////////////////////
        // Write remaining of the old previous file (if existing)
        if ( pOldFileBuffer )
        {
            if ( pOldSecndPart )
            {
                if ( pEnd )
                {
                    fwrite(pOldSecndPart, 1, pEnd - pOldSecndPart, pHtml);
                }
                else
                {
                    fwrite(pOldSecndPart, 1, iFileSize - (pOldSecndPart - pOldFileBuffer), pHtml);
                }
            }
            free(pOldFileBuffer);
        }
        else
        {
            // file creation : close block
            fprintf(pHtml, "</pre>\n");
        }

        /////////////////////////////////
        // Write footer for the html file
        fprintf(pHtml, "</html>\n");
        fclose(pHtml);
        flushall();

        // Delete old html file
        DeleteFile(sz_HtmlFile);
        // Rename new html file (forced to pass by a copy coz rename doesn't work
        // if you're not in the folder !!!)
        CopyFile(sz_NewHtmlFile, sz_HtmlFile, FALSE);
        DeleteFile(sz_NewHtmlFile);
    }
}

void cl_Desdac::UpdateHtmlTestMapFile( const ChangeListResult& _rCLres )
{
    // HTML modification
    char * pOldFileBuffer = NULL;

    char sz_HtmlFile[BIG_k_NameLength];
    sprintf(sz_HtmlFile, "%s\\%s", msz_OutputFolder, kaz_DESDAC_HtmlMapTest);
    FILE * pHtml = fopen(sz_HtmlFile, "rt");
    int iOldBufSize = 0;
    int iFileSize = 0;
    if ( pHtml )
    {
        int iFile = _fileno(pHtml);
        iFileSize = _filelength(iFile);
        iOldBufSize = iFileSize;
        pOldFileBuffer = (char*)malloc(iFileSize);
        fread(pOldFileBuffer, 1, iFileSize, pHtml);
        fclose(pHtml);
        flushall();
    }

    char sz_NewHtmlFile[BIG_k_NameLength];
    sprintf(sz_NewHtmlFile, "%s\\%s", msz_OutputFolder, kaz_DESDAC_NewHtmlMapTest);
    pHtml = fopen(sz_NewHtmlFile, "wt");
    if ( pHtml )
    {
        int iChangeList = _rCLres.mo_LaunchedCL.mi_ChangeList;
        const char* pzDesdacClientName = mast_ClientPCs[_rCLres.mi_ClientIndex].msz_ClientName;

        /////////
        // Header
        char sz_Ver[15];
        bGetExeVersion(sz_Ver);
        fprintf(pHtml, "<html>\n<head>\n<title>\n");
        fprintf(pHtml, "DESDAC %s: automated data-tests report\n", sz_Ver);
        fprintf(pHtml, "</title>\n</head>\n<pre>\n");

        fprintf(pHtml, "%s - ", msz_ProjectName);   // project Name (just for the record)

        if( mb_MapTestingXenonEnable ) 
        {   fprintf(pHtml, "MAP TESTING " kaz_DESDAC_TARGET_XENON);
        }
        else
        {   fprintf(pHtml, "MAP TESTING " kaz_DESDAC_TARGET_PC);
        }

        fprintf(pHtml, " [%s %s] (Load / Launch  -  NT = Not Tested)\n", msz_PerforceServer, msz_DepotBranch );  // depot & branch


        //////////////////////////////
        // Compute split of old buffer
        char * pOldFirstPart = NULL;
        char * pOldSecndPart = NULL;
        char * pEnd = NULL;
        if ( pOldFileBuffer )
        {
            // Look for the place to insert new CL
            int iCl = iChangeList+1;
            pOldSecndPart = pOldFileBuffer;
            while ( iCl > iChangeList )
            {
                pOldSecndPart = strstr(pOldSecndPart, "---");
                char * pTmp = strstr(pOldSecndPart, "CL : ");
                if ( pTmp )
                {
                    sscanf(pTmp, "CL : <b> %d", &iCl);
                }
                else 
                    iCl = iChangeList - 1;

                if ( iCl > iChangeList && !pOldFirstPart )
                    pOldFirstPart = strstr(pOldFileBuffer, "---");
                if ( iCl > iChangeList )
                    pOldSecndPart = pTmp;
            }

            pEnd = strstr(pOldFileBuffer, "</html>");
        }

        /////////////////////////////////
        // Write first part of old buffer
        if ( pOldFirstPart )
        {
            if ( pOldSecndPart )
            {
                fwrite(pOldFirstPart, 1, pOldSecndPart - pOldFirstPart, pHtml);
            }
            else if ( pEnd )
            {
                fwrite(pOldFirstPart, 1, pEnd - pOldFirstPart, pHtml);
            }
            else
            {
                fwrite(pOldFirstPart, 1, iFileSize - (pOldFirstPart - pOldFileBuffer), pHtml);
            }
        }

        ///////////////
        // Write new CL
        fprintf(pHtml, "\n------------------------------------------");
        fprintf(pHtml, kaz_DESDAC_HTML_CL_BOOKMARK "\n", iChangeList);
        fprintf(pHtml, kaz_DESDAC_HTML_CL_Black, iChangeList);

        // General Status
        if ( _rCLres.mb_MapTestFailed )
        {   fprintf(pHtml, kaz_DESDAC_HTML_FAILED);
        }
        else
        {   fprintf(pHtml, kaz_DESDAC_HTML_OK);
        }

        // result for each map
        const int kColumnNb = 5;
        for ( int i = 0; i < mi_NbMapToTest; ++i )
        {
            st_MapToTest& rMap = mast_MapToTest[i];

            // Map name (without path). In red if failed + link on possible crash dump
            if( bIsFailedStatus(rMap.me_LoadStatus) || bIsFailedStatus(rMap.me_LaunchStatus) )
            {   
                char sz_FileLink[BIG_k_NameLength];

                if( (rMap.me_LoadStatus == E_StatusCrashed) || (rMap.me_LaunchStatus == E_StatusCrashed) )
                {   
                    // link on crash dump
                    if(mb_MapTestingXenonEnable)
                    {   sprintf(sz_FileLink, "<a href = \"" kaz_DESDAC_XenonResultLogFile_CLNB_MapName "\" target = \"View Log\">", iChangeList, rMap.msz_MapName );
                    }
                    else
                    {   sprintf(sz_FileLink, "<a href = \"" kaz_DESDAC_CrashDumpFile_CLNB_MapName "\" target = \"View Log\">", iChangeList, rMap.msz_MapName );
                    }
                    fprintf( pHtml, "  %s<font color=\"red\">%s</font></a> ", sz_FileLink, rMap.msz_MapName );
                }
                else if( rMap.me_LaunchStatus == E_StatusAIBug )
                {
                    // link on IAbugs report
                    sprintf(sz_FileLink, "<a href = \"" kaz_DESDAC_AIBugsFile_CLNB "\" target = \"View Log\">", iChangeList );
                    fprintf( pHtml, "  %s<font color=\"red\">%s</font></a> ", sz_FileLink, rMap.msz_MapName );
                }
                else
                {   fprintf( pHtml, "  <font color=\"red\">%s</font> ", rMap.msz_MapName );
                }
            }
            else
            {   fprintf( pHtml, "  %s ", rMap.msz_MapName );
            }

            // Load status
            fprintf(pHtml, szStatusHtmlTag(rMap.me_LoadStatus)   );

            // Launch status
            fprintf(pHtml, "/");    // separator
            fprintf(pHtml, szStatusHtmlTag(rMap.me_LaunchStatus) );

            // display in columns 
            if( 0 == (i+1)%kColumnNb )
            {   fprintf(pHtml, "\n" kaz_DESDAC_HTML_CL_INDENT);
            }
        }

        // link on general report
        fprintf(pHtml, "  See <a href = \"" kaz_DESDAC_Html "#%d\" target = \"CL Test Page\">CL Test Page</a>", iChangeList );
        
        // Indicate log, if there is one
        if ( _rCLres.mb_LogExists )
        {   fprintf(pHtml, " (log PC: <a href = \"" kaz_DESDAC_LogFile_CLNB "\" target = \"View Log\">" kaz_DESDAC_LogFile_CLNB "</a>) ", iChangeList, iChangeList);
        }
        else
        {   fprintf(pHtml, " (No log) ");
        }

        //end of line
        fprintf(pHtml, "\n " kaz_DESDAC_HTML_CL_INDENT);

        // Test info : engine version, client name, test end time, test duration
        fprintf(pHtml, " Test: v%d.%02d on %s. ", _rCLres.mi_EnginePCVersion, _rCLres.mi_EnginePCPatch, pzDesdacClientName);
        fprintf(pHtml, " End: <b>%s</b>", _rCLres.mo_EndOfTestTime.Format(kaz_DESDAC_DATETIMEFORMAT) );
        fprintf(pHtml, " [%s]", szFormatDuration(_rCLres.mo_EndOfTestTime - _rCLres.mo_StartOfTestTime) );

        // Submission time
        fprintf (   pHtml, "  Submitted:" );
        if ( _rCLres.mb_SubmitTimeIsValid )
        {   fprintf (   pHtml, " <b>%s</b>", _rCLres.mo_SubmitTime.Format(kaz_DESDAC_DATETIMEFORMAT) );
        }
        else
        {   fprintf(pHtml, " <b>undefined</b>" );
        }

        // possible skipped CLs
        if( mb_TestLastCLonlyEnable )
        {   
            if(mb_FirstLaunch)
            {   fprintf(pHtml, "  Skipped CLs: ?");
            }
            else
            {   fprintf(pHtml, "  Skipped CLs: %d", _rCLres.mo_LaunchedCL.mi_NbClSkipped);
            }
        }

        //end of line
        fprintf(pHtml, "\n");



        /////////////////////////////////////////////////////////
        // Write remaining of the old previous file (if existing)
        if ( pOldFileBuffer )
        {
            if ( pOldSecndPart )
            {
                if ( pEnd )
                {
                    fwrite(pOldSecndPart, 1, pEnd - pOldSecndPart, pHtml);
                }
                else
                {
                    fwrite(pOldSecndPart, 1, iFileSize - (pOldSecndPart - pOldFileBuffer), pHtml);
                }
            }
            free(pOldFileBuffer);
        }
        else
        {
            // file creation : close block
            fprintf(pHtml, "</pre>\n");
        }


        /////////////////////////////////
        // Write footer for the html file
        fprintf(pHtml, "</html>\n");
        fclose(pHtml);
        flushall();

        // Delete old html file
        DeleteFile(sz_HtmlFile);
        // Rename new html file (forced to pass by a copy coz rename doesn't work
        // if you're not in the folder !!!)
        CopyFile(sz_NewHtmlFile, sz_HtmlFile, FALSE);
        DeleteFile(sz_NewHtmlFile);
    }
}

void cl_Desdac::PollPerforceServer()
{
    char az_Command[BIG_k_CommandLength];

    // Force password
    sprintf(az_Command, "p4 -p %s set P4USER=%s", msz_PerforceServer, msz_PerforceUser);
    LaunchSystemCommand(az_Command, true);
    FILE * pPassword = fopen(msz_DesdacTempFile, "w+");
    if ( pPassword )
    {
        fprintf(pPassword, "%s\n", msz_ClientPassword);
        fclose(pPassword);
        flushall();
    }
    sprintf(az_Command, "p4 -p %s login < %s", msz_PerforceServer, msz_DesdacTempFile);
    LaunchSystemCommand(az_Command, true);
    DeleteFile(msz_DesdacTempFile);

    int iLastCL = 0;
    ///////////////////////////////////
    // Get latest change list on server
    sprintf(az_Command, "p4 -p %s changes -m 1 -s submitted %s > %s", msz_PerforceServer, msz_DepotBranch, msz_DesdacTempFile);
    LaunchSystemCommand(az_Command, true);
    FILE * pFile = NULL;
    char az_LastUser[BIG_k_NameLength];
    char az_LastClient[BIG_k_NameLength];
    pFile = fopen(msz_DesdacTempFile, "rt");
    if ( pFile )
    {
        int iYear, iMonth, iDay;
        fscanf(pFile, "Change %d on %d/%d/%d by %s@%s", &iLastCL, &iYear, &iMonth, &iDay, az_LastUser);
        fclose(pFile);
        flushall();
        DeleteFile(msz_DesdacTempFile);

        char * c = az_LastUser;
        while ( *c )
        {
            if ( *c == '@' )
            {
                *c = 0x00;
                strcpy(az_LastClient, c+1);
            }
            c++;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    // Analyze CL to see if it's not a CL launched by DESDAC (AUTOMATED DATABASE FIX)
    bool bIsAutomatedCl = false;
    sprintf(az_Command, "p4 -p %s change -o %d > %s", msz_PerforceServer, iLastCL, msz_DesdacTempFile);
    LaunchSystemCommand(az_Command, true);
    FILE * pClDescription = fopen(msz_DesdacTempFile, "rt");
    if ( pClDescription )
    {
        int iFile = _fileno(pClDescription);
        int iFileSize = _filelength(iFile);
        char * pBuf = (char*)malloc(iFileSize);
        if ( pBuf )
            fread(pBuf, 1, iFileSize, pClDescription);
        fclose(pClDescription);
        flushall();
        // Delete temp file
        DeleteFile(msz_DesdacTempFile);
        if ( pBuf )
        {
            char * c = strstr(pBuf, "Description:");
            if ( c )
            {
                c += strlen("Description:");
                c = strstr(c, "Description:");
                if ( c )
                {
                    char sz_Description[1024];
                    sscanf(c, "Description: %s", sz_Description);
                    if ( strstr(sz_Description, "AUTOMATED_DATABASE_FIX") )
                        bIsAutomatedCl = true;
                }
            }
            free(pBuf);
        }
    }

    if ( bIsAutomatedCl == false )
        PushChangeList(iLastCL, az_LastUser, az_LastClient);
}

void cl_Desdac::Suspend(bool _bSuspend)
{
    mb_IsClientSuspended = _bSuspend;
    mb_IsServerSuspended = _bSuspend;

    if ( mb_IsClientSuspended )
    {
        FILE * pSuspended = fopen(kaz_DESDAC_SuspendedFile, "wt");
        if ( pSuspended )
        {
            fprintf(pSuspended, "suspended\n");
            fclose(pSuspended);
            flushall();
        }
    }
    else
    {
        DeleteFile(kaz_DESDAC_SuspendedFile);
    }
}

bool cl_Desdac::bIsClientAvalaibleForRequest(int _iIndex)
{
    st_ClientPc& rstClient = mast_ClientPCs[_iIndex];

    // Client PC down ?
    if ( !rstClient.mb_ClientAware )
        return false;

    char sz_File[BIG_k_NameLength];

    // Client launched ?
    sprintf(sz_File, kaz_DESDAC_RemoteTagFile, rstClient.msz_ClientName);
    FILE * pTagFile = fopen(sz_File, "r");
    if ( !pTagFile )
    {
        rstClient.mi_ClientStatus = k_ClientStatus_NotLaunched;
        rstClient.mi_RunningCL = k_iNotDefined;
        return false;
    }
    fclose(pTagFile);


    // Client Suspended ?
    sprintf(sz_File, kaz_DESDAC_RemoteSuspendedFile, rstClient.msz_ClientName);
    FILE * pSuspendedFile = fopen(sz_File, "r");
    if ( pSuspendedFile )
    {
        rstClient.mi_ClientStatus = k_ClientStatus_Suspended;
        fclose(pSuspendedFile);
        return false;
    }


    // Engine Update Request Pending ?
    sprintf(sz_File, kaz_DESDAC_RemoteRequestUpdateFile, rstClient.msz_ClientName);
    FILE * pUpdateFile = fopen(sz_File, "r");
    rstClient.mb_EngineUpdateRequest = (NULL != pUpdateFile);
    if(pUpdateFile)
    {   fclose(pUpdateFile);
    }


    // Client busy ?
    sprintf(sz_File, kaz_DESDAC_RemoteRequestFile, rstClient.msz_ClientName);
    FILE * pRequestFile = fopen(sz_File, "r");
    if ( pRequestFile )
    {
        rstClient.mi_ClientStatus = k_ClientStatus_Busy;
        fclose(pRequestFile);
        return false;
    }
    

    // Engine Update Running ?
    if(rstClient.mb_EngineUpdateRequest)
    {
        rstClient.mi_ClientStatus = k_ClientStatus_Updating;
        return false;
    }


    // Waiting for request...
    rstClient.mi_ClientStatus = k_ClientStatus_Waiting;
    rstClient.mi_RunningCL = k_iNotDefined;
    return true;
}

int cl_Desdac::iGetNbBusyClients()
{
    int iBusy = 0;
    for ( int i = 0; i < iGetNbClients(); ++i )
    {
        if ( mast_ClientPCs[i].mi_ClientStatus == k_ClientStatus_Busy )
            iBusy++;
    }
    return iBusy;
}

int cl_Desdac::iGetNbSuspendedClients()
{
    int iNbSuspended = 0;
    for ( int i = 0; i < iGetNbClients(); ++i )
    {
        if ( mast_ClientPCs[i].mi_ClientStatus == k_ClientStatus_Suspended )
            iNbSuspended++;
    }
    return iNbSuspended;
}

int cl_Desdac::iGetNbNotLaunchedClients()
{
    int iNotLaunched = 0;
    for ( int i = 0; i < iGetNbClients(); ++i )
    {
        if ( mast_ClientPCs[i].mi_ClientStatus == k_ClientStatus_NotLaunched )
            iNotLaunched++;
    }
    return iNotLaunched;
}

int cl_Desdac::iGetNbUpdatingClients()
{
    int iUpdating = 0;
    for ( int i = 0; i < iGetNbClients(); ++i )
    {
        if ( mast_ClientPCs[i].mi_ClientStatus == k_ClientStatus_Updating )
            iUpdating++;
    }
    return iUpdating;
}

int cl_Desdac::iGetNbOfPendingUpdateRequests()
{
    int iPendingUpdate = 0;
    for ( int i = 0; i < iGetNbClients(); ++i )
    {
        if ( mast_ClientPCs[i].mb_EngineUpdateRequest )
            iPendingUpdate++;
    }
    return iPendingUpdate;
}

void cl_Desdac::UpdateListOfAvailableClients()
{
    mi_NbAvailableClients = 0;
    for ( int i = 0; i < iGetNbClients(); ++i )
    {
        mast_ClientPCs[i].mb_ClientAvailableForRequest = bIsClientAvalaibleForRequest(i);
        if ( mast_ClientPCs[i].mb_ClientAvailableForRequest )
            mi_NbAvailableClients++;
    }
}



// sends an "update engine version" request to the clients
// the request will be treated by the client when it becomes available
// (server)
const unsigned long kuiUpdateRequestFileVersion = 1;
void cl_Desdac::SendUpdateEngineVersionRequest(unsigned long _uiUpdateTypeFlags, int _iEnginePCVersion, int _iEnginePCPatch, int _iEnginePCCompilNb)
{
    for ( int i = 0; i < iGetNbClients(); ++i )
    {
        const char* pzClientName = mast_ClientPCs[i].msz_ClientName;

        // Put a file to say that version needs to be updated
        char sz_UpdateTempFile[BIG_k_NameLength];
        sprintf(sz_UpdateTempFile, kaz_DESDAC_RemoteRequestUpdateTempFile, pzClientName);

        FILE * pRemoteFile = fopen(sz_UpdateTempFile, "w+");
        if ( pRemoteFile )
        {
            // create the request descriptor
            fprintf(pRemoteFile, "UpdateRequest=%d %d\n", kuiUpdateRequestFileVersion, _uiUpdateTypeFlags);

            // Update PC engine
            if( _uiUpdateTypeFlags & kuiEnginePCUpdate)
            {
                fprintf(pRemoteFile, "YetiVer=" kaz_DESDAC_EngineVersion "\n", _iEnginePCVersion, _iEnginePCPatch, _iEnginePCCompilNb);
                fprintf(pRemoteFile, "%s\n", msz_EnginePCUpdatePath);
            }

            // Update Xenon Engine
            if( _uiUpdateTypeFlags & kuiEngineXenonUpdate)
            {
                fprintf(pRemoteFile, "%s\n", msz_EngineXenonUpdatePath);
            }

            fclose(pRemoteFile);
            flushall();


            // Now copy the temp file to definitive file (this triggers the update on the client side)
            char sz_File2[BIG_k_NameLength];
            sprintf(sz_File2, kaz_DESDAC_RemoteRequestUpdateFile, pzClientName);
            CopyFile(sz_UpdateTempFile, sz_File2, FALSE);

            // Delete the request temp file
            DeleteFile( sz_UpdateTempFile );
        }
    }
}


void cl_Desdac::UpdateListOfAwareClients()
{
    mi_NbDownClients = 0;
    for ( int i = 0; i < iGetNbClients(); ++i )
    {
        char sz_Command[BIG_k_CommandLength];
        sprintf(sz_Command, "ping -n 1 %s > %s", mast_ClientPCs[i].msz_ClientName, msz_DesdacTempFile);
        LaunchSystemCommand(sz_Command, true);
        FILE * pPing = fopen(msz_DesdacTempFile, "rt");
        mast_ClientPCs[i].mb_ClientAware = false;
        mast_ClientPCs[i].mi_ClientStatus = k_ClientStatus_Down;
        if ( pPing ) 
        {
            int iFile = _fileno(pPing);

            int iFileSize = _filelength(iFile);
            char * pBuffer = (char*)malloc(iFileSize);
            fread(pBuffer, 1, iFileSize, pPing);
            // French and english only.
            if ( strstr(pBuffer, "us = 1") )
            {
                mast_ClientPCs[i].mb_ClientAware = true;
                mast_ClientPCs[i].mi_ClientStatus = k_ClientStatus_NotLaunched;
            }
            if ( strstr(pBuffer, "received = 1") )
            {
                mast_ClientPCs[i].mb_ClientAware = true;
                mast_ClientPCs[i].mi_ClientStatus = k_ClientStatus_NotLaunched;
            }
            free(pBuffer);
            fclose(pPing);
            flushall();
        }
        DeleteFile(msz_DesdacTempFile);
        if ( mast_ClientPCs[i].mb_ClientAware == false )
            mi_NbDownClients++;
    }
}

int cl_Desdac::iGetNbClOnStack()
{
    return mi_NbClInStack;
}

int cl_Desdac::iGetClInTopOfStack()
{
    if ( mi_NbClInStack == 0 )
        return -1;

    return mast_ClStack[mi_NbClInStack-1].mi_ChangeList;
}

int cl_Desdac::iGetLastClInStack()
{
    if ( mi_NbClInStack == 0 )
        return -1;

    return mast_ClStack[0].mi_ChangeList;
}

void cl_Desdac::PopChangeList(ChangeListToLaunch * _pst_ChangeList)
{
    if ( mi_NbClInStack == 0 )
    {
        _pst_ChangeList->mi_ChangeList = -1;
    }
    else if( mb_TestLastCLonlyEnable )
    {
        // get latest CL
        *_pst_ChangeList = mast_ClStack[0];

        // store skipped CLs
        _pst_ChangeList->mi_NbClSkipped     = mi_NbClInStack-1;
        _pst_ChangeList->mi_FirstCLSkipped  = mast_ClStack[mi_NbClInStack-1].mi_ChangeList;

        // reset stack
        mi_NbClInStack = 0;
    }
    else
    {
        *_pst_ChangeList = mast_ClStack[mi_NbClInStack-1];
        _pst_ChangeList->mi_NbClSkipped = 0;
        mi_NbClInStack--;
    }
}

bool cl_Desdac::bGetLastClResultGlobalFailed()
{
    bool bRes = (   bGetLastClResultDbFailed() 
                ||  bGetLastClResultAiFailed()
                || ( bGetGlobalMapTesting() && bGetLastClResultMapTestFailed() )
                );
    return bRes;
}

void cl_Desdac::RunOneFrame_Server()
{
    // Scan Perforce database
    PollPerforceServer();

    UpdateListOfAvailableClients();

    if ( !mb_IsServerSuspended )    // server: suspend only the dispatch of requests: allows to finish results treatment
    {
        int iNbRequest = 0;
        // try to launch all change lists in stack
        bool bClientAvailable = mi_NbAvailableClients >= 0 ? true : false;
        while ( bClientAvailable && mi_NbClInStack != 0 )
        {
            if ( iGetClInTopOfStack() != -1 )
            {
                bool bRequestLaunched = false;
                // Try to find an available client
                for ( int i = 0; i < iGetNbClients(); ++i )
                {
                    if ( mast_ClientPCs[i].mb_ClientAvailableForRequest )
                    {
                        // Create request
                        char sz_RequestFile[BIG_k_NameLength];
                        sprintf(sz_RequestFile, kaz_DESDAC_RemoteRequestTempFile, mast_ClientPCs[i].msz_ClientName);
                        FILE * pRemoteFile = fopen(sz_RequestFile, "w+");
                        if ( pRemoteFile )
                        {
                            bRequestLaunched = true;
                            mast_ClientPCs[i].mb_ClientAvailableForRequest = false;

                            // Fill request
                            ChangeListToLaunch st_Request;
                            PopChangeList(&st_Request);

                            fprintf(pRemoteFile, "%d\n", st_Request.mi_ChangeList);
                            fprintf(pRemoteFile, "%s\n", st_Request.msz_UserName);
                            fprintf(pRemoteFile, "%s\n", st_Request.msz_ClientName);
                            fprintf(pRemoteFile, "%d\n", st_Request.mul_Options);

							// launchmap faisable uniquement par le server
/*
                            if( (st_Request.mul_Options & k_Option_LoadMap) || (st_Request.mul_Options & k_Option_LaunchXenon) )
                            {
                                fprintf(pRemoteFile, "MapTesting %d\n", mi_NbMapToTest);
                                for ( int m = 0; m < mi_NbMapToTest; ++m )
                                {
                                    fprintf(pRemoteFile, "%s\n", mast_MapToTest[m].msz_MapKey);
                                }
                            }
*/
                            fclose(pRemoteFile);
                            flushall();

                            // Push the change list on results to be treated
                            PushLaunchedChangeListToResult(&st_Request);

                            // Now copy the request temp file to definitive file
                            char sz_Command[BIG_k_CommandLength];
                            char sz_File1[BIG_k_NameLength];
                            char sz_File2[BIG_k_NameLength];
                            sprintf(sz_File1, kaz_DESDAC_RemoteRequestTempFile, mast_ClientPCs[i].msz_ClientName);
                            sprintf(sz_File2, kaz_DESDAC_RemoteRequestFile, mast_ClientPCs[i].msz_ClientName);
                            CopyFile(sz_File1, sz_File2, FALSE);

                            // store Running infos
                            mast_ClientPCs[i].mo_StartOfTestTime        = CTime::GetCurrentTime();
                            mast_ClientPCs[i].mi_RunningCL              = st_Request.mi_ChangeList;

                            // Delete the request temp file
                            sprintf(sz_Command, kaz_DESDAC_RemoteRequestTempFile, mast_ClientPCs[i].msz_ClientName);
                            DeleteFile(sz_Command);
                            break;
                        }
                        else
                        {
                            // This client seems to be down
                            mast_ClientPCs[i].mb_ClientAware = false;
                        }
                    }
                }
                if ( !bRequestLaunched )
                    bClientAvailable = false;
            }
        }
    } //end "if not suspended"


    // Scan the "results" folder and
    // modify html log file accordingly
    AnalyzeAndTreatResults();

    // Look if Change list results stack, to warn user if necessary
    UpdateChangeListResultStack();

    // Check Stacks filling status (to detect blocking processes)
    CheckStackThreshold();
}


void cl_Desdac::AddClientPC(char * _pz_ClientName)
{
    // Invalid client ?
    if ( strcmp(_pz_ClientName, kaz_DESDAC_DefaultPcClient) == 0 )
        return;

    // Security test : check that client is not already in list
    for ( unsigned long i = 0; i < mul_NbClients; ++i )
    {
        if ( strcmp(mast_ClientPCs[i].msz_ClientName, _pz_ClientName) == 0 )
            return;
    }

    // Real add of that client
    strcpy( mast_ClientPCs[mul_NbClients].msz_ClientName, _pz_ClientName);
    mul_NbClients++;
}

void cl_Desdac::RemoveClientPC(int _iIndex)
{
    // This should not happen
    if ( _iIndex < 0 )
        return;

    // This should not happen too...
    if ( _iIndex >= (int)mul_NbClients )
        return;

    // Perhaps this should happen in a parallel dimension
    if ( mul_NbClients == 0 )
        return;

    for ( unsigned long i = _iIndex; i < mul_NbClients-1; ++i )
    {
        memcpy(&mast_ClientPCs[i], &mast_ClientPCs[i+1], sizeof(st_ClientPc));
    }
    mul_NbClients--;
}

//===========================================
// MAILS
void cl_Desdac::AddMailToSend(char * _pz_Mail)
{
    // To much mails ?
    if ( mi_NbMailsToSend >= k_MaxMailToSend )
        return;

    // Invalid mail ?
    if ( strcmp(_pz_Mail, kaz_DESDAC_DefaultMailToSend) == 0 )
        return;

    // Security test : check that mail is not already in list
    for ( int i = 0; i < mi_NbMailsToSend; ++i )
    {
        if ( strcmp(masz_MailsToSend[i], _pz_Mail) == 0 )
            return;
    }

    // Real add of that mail
    strcpy( masz_MailsToSend[mi_NbMailsToSend], _pz_Mail);
    mi_NbMailsToSend++;
}

void cl_Desdac::RemoveMailToSend(int _iIndex)
{
    // This should not happen
    if ( _iIndex < 0 )
        return;

    // This should not happen too...
    if ( _iIndex >= mi_NbMailsToSend )
        return;

    // Perhaps this should happen in a parallel dimension
    if ( mi_NbMailsToSend == 0 )
        return;

    for ( int i = _iIndex; i < mi_NbMailsToSend-1; ++i )
    {
        memcpy(masz_MailsToSend[i], masz_MailsToSend[i+1], BIG_k_NameLength);
    }
    mi_NbMailsToSend--;
}

char * cl_Desdac::pz_GetMailToSend(int _iIndex)
{
    // This should not happen
    if ( _iIndex < 0 )
        return NULL;

    // This should not happen too...
    if ( _iIndex >= mi_NbMailsToSend )
        return NULL;

    // Perhaps this should happen in a parallel dimension
    if ( mi_NbMailsToSend == 0 )
        return NULL;

    return masz_MailsToSend[_iIndex];
}

int cl_Desdac::iGetNbMailToSend()
{
    return mi_NbMailsToSend;
}
// MAILS
//===========================================

char * cl_Desdac::pz_GetPerforceServer()
{
    return msz_PerforceServer;
}

char * cl_Desdac::pz_GetPerforceUser()
{
    return msz_PerforceUser;
}

char * cl_Desdac::pz_GetPassword()
{
    return msz_ClientPassword;
}

char * cl_Desdac::pz_GetExeName()
{
    return msz_ExeName;
}

char * cl_Desdac::pz_GetPerforceBranch()
{
    return msz_DepotBranch;
}

char * cl_Desdac::pz_GetBigFileName()
{
    return msz_BigFileName;
}

char * cl_Desdac::pz_GetJadeSourcesPath()
{
    return msz_JadeSourcesPath;
}

char * cl_Desdac::pz_GetTempFileName()
{
    return msz_DesdacTempFile;
}

void cl_Desdac::SetPerforceServer(char * _pz_PerforceServer)
{
    strcpy(msz_PerforceServer, _pz_PerforceServer);
}

void cl_Desdac::SetPerforceUser(char * _pz_PerforceUser)
{
    strcpy(msz_PerforceUser, _pz_PerforceUser);
    ComputePasswordFromUser();
}

void cl_Desdac::SetPassword(char * _pz_Password)
{
    strcpy(msz_ClientPassword, _pz_Password);
}

void cl_Desdac::SetExeName(char * _pz_ExeName)
{
    strcpy(msz_ExeName, _pz_ExeName);
    ExtractPathFromFullName(msz_ExeAndLogPath, msz_ExeName);
}

void cl_Desdac::SetPerforceBranch(char * _pz_PerforceBranch)
{
    strcpy(msz_DepotBranch, _pz_PerforceBranch);
}

void cl_Desdac::SetBigFileName(char * _pz_BigFileName)
{
    strcpy(msz_BigFileName, _pz_BigFileName);
}

void cl_Desdac::SetJadeSourcesPath(char * _pz_JadeSourcesPath)
{
    strcpy(msz_JadeSourcesPath, _pz_JadeSourcesPath);
}

void cl_Desdac::SetTempFileName(char * _pz_TempFileName)
{
    strcpy(msz_DesdacTempFile, _pz_TempFileName);
}

char * cl_Desdac::pz_GetClient(int _iIndex)
{
    return mast_ClientPCs[_iIndex].msz_ClientName;
}

// (Server)
const char * cl_Desdac::pz_GetClientStatus(int _iIndex)
{
    switch ( mast_ClientPCs[_iIndex].mi_ClientStatus )
    {
    case k_ClientStatus_Waiting:
        return kaz_ClientStatus_Waiting;
    case k_ClientStatus_Busy:
        return kaz_ClientStatus_Busy;
    case k_ClientStatus_Down:
        return kaz_ClientStatus_Down;
    case k_ClientStatus_NotLaunched:
        return kaz_ClientStatus_NotLaunched;
    case k_ClientStatus_Suspended:
        return kaz_ClientStatus_Suspended;
    case k_ClientStatus_Updating:
        return kaz_ClientStatus_Updating;
        
    default:
        return kaz_ClientStatus_Unknown;
    }
}

// (Server)
void cl_Desdac::GetClientStatInfos(int _iIndex, char * _szInfos)
{
    st_ClientPc rstClient = mast_ClientPCs[_iIndex];

    if( (( rstClient.mi_ClientStatus == k_ClientStatus_Busy ) || ( rstClient.mi_ClientStatus == k_ClientStatus_Suspended ))
        && ( rstClient.mi_RunningCL != k_iNotDefined )
      )
    {
        char szUpdateOrNot [BIG_k_NameLength];
        sprintf( szUpdateOrNot, rstClient.mb_EngineUpdateRequest ? "[update pending]" : "" );

        sprintf( _szInfos, "  - CL%6d started at %s %s", rstClient.mi_RunningCL, rstClient.mo_StartOfTestTime.Format(kaz_DESDAC_TIMEFORMAT), szUpdateOrNot  );
    }
    else
    {   _szInfos[0] = 0;
    }
}

char * cl_Desdac::pz_GetPcServer()
{
    return msz_PcServerName;
}

void cl_Desdac::SetPcServer(char * _pz_PcServer)
{
    strcpy(msz_PcServerName, _pz_PcServer);
}

const char * cl_Desdac::pz_GetOutputFolder()
{
    return msz_OutputFolder;
}

void cl_Desdac::SetOutputFolder(char * _pz_OutputFolder)
{
    strcpy(msz_OutputFolder, _pz_OutputFolder);
    CreateDirectory(msz_OutputFolder, NULL);
}

static char sz_FullOutputPath [BIG_k_NameLength] = {0};
const char * cl_Desdac::pz_GetRemoteOutputPath()
{
    sprintf(sz_FullOutputPath, kaz_DESDAC_RemoteDesdacFolder "\\%s", msz_ComputerName, msz_OutputFolder);
    return sz_FullOutputPath;
}

char * cl_Desdac::pz_GetDefaultMail()
{
    return msz_DefaultMail;
}

void cl_Desdac::SetDefaultMail(char * _pz_DefaultMail)
{
    strcpy(msz_DefaultMail, _pz_DefaultMail);
}

int cl_Desdac::iGetCurrentCl()
{
    return mst_CurrentCl.mi_ChangeList;
}

char * cl_Desdac::pz_GetCurrentClOwner()
{
    return mst_CurrentCl.msz_UserName;
}


void cl_Desdac::ComputePasswordFromUser()
{
    strcpy(msz_ClientPassword, msz_PerforceUser);
    strcat(msz_ClientPassword, "_00");
    int i = (int)strlen(msz_ClientPassword);
    while ( i < 8 )
    {
        strcat(msz_ClientPassword, "0");
        i++;
    }
}



void cl_Desdac::AddMapToTest(char * _pz_MapKey, char * _pz_MapName)
{
    assert(_pz_MapName);
    assert(_pz_MapKey);

    // Invalid map ?
    if ( strcmp(_pz_MapKey, kaz_DESDAC_DefaultMapName) == 0 )
        return;

    // Security test : check that map is not already in list
    for ( int i = 0; i < mi_NbMapToTest; ++i )
    {
        if ( strcmp(mast_MapToTest[i].msz_MapKey, _pz_MapKey) == 0 )
            return;
    }

    // Add the map to the test list
    strcpy( mast_MapToTest[mi_NbMapToTest].msz_MapKey, _pz_MapKey);

    if ( strcmp(_pz_MapName, kaz_DESDAC_DefaultMapName) == 0 )
    {   // auto generate name
        strcpy( mast_MapToTest[mi_NbMapToTest].msz_MapName, pzSimpleMapName(_pz_MapKey) );
    }
    else
    {   strcpy( mast_MapToTest[mi_NbMapToTest].msz_MapName, _pz_MapName);
    }

    mi_NbMapToTest++;
}

void cl_Desdac::RemoveMapToTest(int _iIndex)
{
    // This should not happen
    if ( _iIndex < 0 )
        return;

    // This should not happen too...
    if ( _iIndex >= mi_NbMapToTest )
        return;

    // Perhaps this should happen in a parallel dimension
    if ( mi_NbMapToTest == 0 )
        return;

    for ( int i = _iIndex; i < mi_NbMapToTest-1; ++i )
    {
        memcpy(&mast_MapToTest[i], &mast_MapToTest[i+1], sizeof(st_MapToTest));
    }
    mi_NbMapToTest--;
}

void cl_Desdac::SetMapTesting(bool _bOnOff)
{
    mb_MapTestingEnable = _bOnOff;

    if(mb_MapTestingEnable && mb_MapTestingXenonEnable )
    {   mb_MapTestingXenonEnable = false;
    }
}

void cl_Desdac::SetAiToCTesting(bool _bOnOff)
{
    mb_AiToCTestingEnable = _bOnOff;
}


char * cl_Desdac::pz_GetMapToTest(int _iIndex)
{
    if ( _iIndex < 0 || _iIndex >= mi_NbMapToTest )
        return kaz_DESDAC_DefaultMapName;
    return mast_MapToTest[_iIndex].msz_MapName;
}

void cl_Desdac::ResetMapTestResults()
{
    for ( int i = 0; i < mi_NbMapToTest; ++i )
    {
        mast_MapToTest[i].ResetTestResults();
    }
}

void cl_Desdac::SetMapTestingXenon(bool _bOnOff)
{
    mb_MapTestingXenonEnable = _bOnOff;

    if(mb_MapTestingEnable && mb_MapTestingXenonEnable )
    {   mb_MapTestingEnable = false;
    }
}






// Tags the CL in red in the HTML file.
void cl_Desdac::TagMailedCL( int _iChangeList, const char* pzHtmlFile )
{
    char * pFileBuffer = NULL;
    int iBufSize = 0;
    int iFileSize = 0;

    // open HTML file
    FILE * pHtml = fopen(pzHtmlFile, "r+");

    if ( pHtml )
    {
        int iFile = _fileno(pHtml);
        iFileSize = _filelength(iFile);
        iBufSize = iFileSize;
        pFileBuffer = (char*)malloc(iFileSize);
        fread(pFileBuffer, 1, iFileSize, pHtml);

        // find the change list
        char szBlackCL[128];
        sprintf(szBlackCL, kaz_DESDAC_HTML_CL_Black, _iChangeList);
        char szRedCL[128];
        sprintf(szRedCL,   kaz_DESDAC_HTML_CL_Red,   _iChangeList);
        

        char* pzFind = strstr( pFileBuffer, szBlackCL);
        if( pzFind )
        {
            rewind(pHtml);
            // overwrite CL section in the file. Black CL and Red CL strings MUST have same length
            strncpy( pzFind, szRedCL, strlen(szRedCL) );
            fprintf( pHtml , pFileBuffer );
        }

        fclose(pHtml);
        flushall();
    }
}

bool cl_Desdac::bGetChangeListSubmitTime( int _iCL, CTime& _roTime )
{
    // Describe the change list to obtain date of submission
    char sz_Command[BIG_k_CommandLength];
    sprintf(sz_Command, "p4 -p %s change -o %d > %s", msz_PerforceServer, _iCL, msz_DesdacTempFile);
    LaunchSystemCommand(sz_Command, true);
    bool bSubmitTime = false;

    int iYear, iMonth, iDay, iHour, iMinute, iSecond;
    FILE * pClDescription = fopen(msz_DesdacTempFile, "rt");
    if ( pClDescription )
    {
        int iFile = _fileno(pClDescription);
        int iFileSize = _filelength(iFile);
        char * pBuf = (char*)malloc(iFileSize);
        if ( pBuf )
            fread(pBuf, 1, iFileSize, pClDescription);
        fclose(pClDescription);
        flushall();
        // Delete temp file
        DeleteFile(msz_DesdacTempFile);
        if ( pBuf )
        {
            char * c = strstr(pBuf, "Date:");
            if ( c )
            {
                c += strlen("Date:");
                c = strstr(c, "Date:");
                if ( c )
                {
                    sscanf(c, "Date: %d/%d/%d %d:%d:%d", &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);
                    _roTime = CTime(iYear, iMonth, iDay, iHour, iMinute, iSecond);
                    bSubmitTime = true;
                }
            }
            free(pBuf);
        }
    }

    return bSubmitTime;
}


bool bIsFailedStatus ( tde_Status _eStatus )
{   
    return (_eStatus != E_StatusNotTested) && (_eStatus != E_StatusOK); 
}

const char* szStatusFullName( tde_Status _eStatus )
{
    switch(_eStatus)
    {
        case E_StatusNotTested:         return  kaz_DESDAC_StatusNotTested;
        case E_StatusOK:                return  kaz_DESDAC_StatusOK;
        case E_StatusAICompileError:    return  kaz_DESDAC_StatusAIFailed;
        case E_StatusAIBug:             return  kaz_DESDAC_StatusAIBug;
        case E_StatusCrashed:           return  kaz_DESDAC_StatusCrashed;
        default:    assert(false);      return  "<unknown>";
    }
}

const char* szStatusHtmlTag( tde_Status _eStatus )
{
    switch(_eStatus)
    {
    case E_StatusNotTested:         return  kaz_DESDAC_HTML_NT;
    case E_StatusOK:                return  kaz_DESDAC_HTML_OK;
    case E_StatusAICompileError:    return  kaz_DESDAC_HTML_FAILED;
    case E_StatusAIBug:             return  kaz_DESDAC_HTML_AIBUG;
    case E_StatusCrashed:           return  kaz_DESDAC_HTML_CRASHED;
    default:    assert(false);      return  "<unknown>";
    }
}

