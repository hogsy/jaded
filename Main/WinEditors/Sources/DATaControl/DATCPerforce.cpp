//------------------------------------------------------------------------------
// Filename   :DATCPerforce.cpp
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: Implementation of DATCPerforce
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCPerforce.h"

// Bigfile support
#include "BIGfiles\BIGfat.h"
#include "BIGfiles\BIGmdfy_file.h"
#include "BIGfiles\BIGmerge.h"

#include "DATCP4ClientUserLogin.h"


//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------
#pragma comment(lib,"libclient.lib")
#pragma comment(lib,"librpc.lib")
#pragma comment(lib,"libsupp.lib")
#pragma comment(lib,"libcmt.lib")
#pragma comment(lib,"oldnames.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"advapi32.lib")
//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------
#ifdef JADEFUSION
extern BIG_gb_IgnoreRecent;
extern BIG_gb_CheckSameName;
extern BOOL BIG_b_CheckUserAbortCommand(void);
#else
extern "C" BOOL BIG_gb_IgnoreRecent;
extern "C" BOOL BIG_gb_CheckSameName;
extern "C" BOOL BIG_b_CheckUserAbortCommand(void);
#endif
//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------
#define P4_APP "JadeEditor - KingKong" // this is used to trace usage by p4 monitor

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------
DAT_CPerforce DAT_CPerforce::ms_Perforce;


//------------------------------------------------------------
//   DAT_CPerforce::DAT_CPerforce()
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
DAT_CPerforce::DAT_CPerforce():
m_ulWorkingChangelist(-1),
m_bIsConnected(FALSE),
m_ulTotalFile(0),
m_ulTotalOperationFile(0),
m_ulOperationFile(0),
m_bPermanentDisable(0),
m_bEnable(TRUE),
m_bRelog(FALSE)
{
	m_Client.SetClient("none");
}

//------------------------------------------------------------
//   DAT_CPerforce::~DAT_CPerforce()
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
DAT_CPerforce::~DAT_CPerforce()
{
	if ( ! DAT_CPerforce::GetInstance()->m_bEnable ) return;

	if ( m_bIsConnected )
	{
		m_Error.Clear();
		m_Client.Final( &m_Error );
	}
	
	CleanP4FilesNotInBF();

	if ( ! m_vcP4Directories.empty() )
	{
		for ( UINT i=0; i < m_vcP4Directories.size() ; i++ )
			delete [] m_vcP4Directories[i];
		m_vcP4Directories.clear();
	}
}

void DAT_CPerforce::Initialize(		const std::string& _strPort,
									const std::string& _strHostName,
									const std::string& _strUsername,
									const std::string& _strPassword)
{
	// setting default connection settings
	GetInstance()->GetClientApi()->SetPort(_strPort.c_str());
	GetInstance()->GetClientApi()->SetHost(_strHostName.c_str());
	GetInstance()->GetClientApi()->SetUser(_strUsername.c_str());
	GetInstance()->SetPassword(_strPassword.c_str());
	GetInstance()->SetWorkingChangelist(-1);
	GetInstance()->GetClientApi()->SetClient("NONE");
    if (!(GetInstance()->IsPermanentDisabled()))
    {
        GetInstance()->Enable();
    }
	GetInstance()->m_bRelog = TRUE;
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4Connect()
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Connect(BOOL bDisableAbort/*= FALSE*/)
{	
	if ( ! DAT_CPerforce::GetInstance()->m_bEnable ) return 0;

	// this should not happen but if we happen to have a jade crash, 
	// it will leave the connection open. So close the connection if we detect 
	// it is still open.
	if ( m_bIsConnected && !m_Client.Dropped() ) 
		P4Disconnect();

	m_Client.SetHost(GetInstance()->GetHost());
	m_Client.SetPort(GetInstance()->GetPort());
	m_Client.SetUser(GetInstance()->GetUser());
	m_Client.SetClient(GetInstance()->GetClient());

   	m_Error.Clear();

	// if we happend to have dropped the connection we need to 
	// reset the ClientApi by connecting and disconnecting. 
	// this will ensure that all FileSys object references have been properly cleaned up.
	if ( m_Client.Dropped() ) 
	{
		m_Client.Init( &m_Error );
		P4Disconnect();
	}


	m_Client.Init( &m_Error );
	m_Client.SetProg(P4_APP);

	// Set break must happen after the init. If Jade starting
	// we don't want user to be able to abort
	if ( ! bDisableAbort )
		m_Client.SetBreak(&m_UserBreak);
	else
		m_Client.SetBreak(NULL);

	int iReturn = CheckForErrors(m_Error) ;

	if ( iReturn ) 
	{
		m_bIsConnected = TRUE;
	}

	if ( m_bIsConnected && GetInstance()->m_bRelog ) 
	{
		DAT_CP4ClientUserLogin ClientUserLogin(GetInstance()->GetPassword());
		m_Client.Run(P4_LOGIN,&ClientUserLogin);
		if ( ClientUserLogin.IsError() ) 
		{
			DAT_CP4Message::OutputErrorMessage("Cannot login to server. Perforce has been disable. Please restart with good parameters.");
			GetInstance()->Disable();
		}
		m_bRelog = FALSE;
	}


	return iReturn;
}

//------------------------------------------------------------
//   int DAT_CPerforce::P4Disconnect()
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4Disconnect( )
{
	//// BEGIN - Test to verify recovery on p4 connect when jade crash when a connection is open
	// static BOOL bCrashRecovery = FALSE;
	//if ( bCrashRecovery ) 
	//{
	//	DAT_CPerforce* p = NULL ; 
	//	p->GetCount();
	//	return 0;
	//}	
	//// END 

	m_Client.Final( &m_Error );

	m_Client.SetBreak(NULL);

	if ( CheckForErrors(m_Error) == 1 )
	{
		// No errors occured during disconnection
		m_bIsConnected = FALSE;
		return 1;
	}
	return 0;
}

int DAT_CPerforce::P4MonitorShow()
{
	PerforceMonitorUI UserClient;

	// lists the opened file within the requested asz_Changelist
	{
		int argCount = 1;
		char *argValues[] = 
		{ 
			"show",
		};

		m_Client.SetArgv( argCount, argValues );
		m_Client.Run( P4_MONITOR, &UserClient );
	}

	return UserClient.GetProcessCount();
}

//------------------------------------------------------------
//   int DAT_CPerforce::GetWorkingChangeList()
/// \author    YCharbonneau
/// \date      2005-01-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG DAT_CPerforce::GetWorkingChangelist(BOOL _bCreate /* = TRUE */)
{
	if ( m_ulWorkingChangelist == -1 && _bCreate) 
	{
		m_ulWorkingChangelist = P4Change(-1,GetCreateChangelistDescription().c_str(),"");
	}
	return m_ulWorkingChangelist;
}

//------------------------------------------------------------
//   int DAT_CPerforce::CheckForErrors(char* )
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::CheckForErrors(Error& _Error)
{
	if( _Error.Test() )
	{
		StrBuf msg;
		_Error.Fmt( &msg );
		msg.Append("\n");
		DAT_CP4Message::OutputErrorMessage(msg.Value());
		return 0;
	}
	return 1;
}

//------------------------------------------------------------
//   int DAT_CPerforce::LocalCheckout()
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4LocalCheckout()
{
#pragma message( "DATCPerforce.h - Implement LocalCheckout")
	return 0;
}

//------------------------------------------------------------
//   int DAT_CPerforce::ExecuteOperationOnDir(BIG_INDEX _ulDirIndex, 
/// \author    YCharbonneau
/// \date      2005-01-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::ExecuteOperationOnDir(BIG_INDEX _ulDirIndex, 
										 int (DAT_CPerforce::*_fpOperation)(BIG_INDEX* ul_Indexes,ULONG _ul_Changelist),
										 BOOL _bRecursive , 
										 BOOL _bFirst,
										 BOOL _bAutoSubmit,
										 BOOL (*_fpUiFileCallback)(BIG_INDEX _ulIndex)
										)
{
	static BIG_INDEX s_ulIndex[P4_MAX_FILE]={BIG_C_InvalidIndex};
	static DWORD dwTime;

	// first call, setting up recursive variables
	if ( _bFirst ) 
	{
		dwTime = GetTickCount();
		m_ulTotalOperationFile = 0;
		m_ulOperationFile = 0;
		m_ulTotalFile = 0;
		for ( UINT ui = 0; ui < P4_MAX_FILE; ui ++ ) 
		{
			s_ulIndex[ui] = BIG_C_InvalidIndex;
		}
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX   ul_SubDir;
	BIG_INDEX	ul_FileIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Parse all subdirs. */
	if(_bRecursive)
	{
		ul_SubDir = BIG_SubDir(_ulDirIndex);
		while(ul_SubDir != BIG_C_InvalidIndex)
		{
			ExecuteOperationOnDir(ul_SubDir,_fpOperation,_bRecursive,FALSE,_bAutoSubmit,_fpUiFileCallback);
			ul_SubDir = BIG_NextDir(ul_SubDir);
		}
	}

	// check for user cancel 
	if(LINK_gb_EscapeDetected) 
	{
		return 0 ;
	}

	
	// executing requested operation on each file
	ul_FileIndex = BIG_FirstFile(_ulDirIndex);
	while(ul_FileIndex != BIG_C_InvalidIndex)
	{
		if ( !_fpUiFileCallback || (_fpUiFileCallback && _fpUiFileCallback(ul_FileIndex)) ) 
		{
			s_ulIndex[m_ulTotalFile] = ul_FileIndex; 
			m_ulTotalFile++;
			m_ulTotalOperationFile++;
		}

		if ( m_ulTotalFile >= P4_MAX_FILE ) 
		{
			m_ulTotalFile = 0;
			
			m_ulOperationFile += (this->*_fpOperation)(&s_ulIndex[0],GetWorkingChangelist());	
			for ( UINT ui = 0; ui < P4_MAX_FILE; ui ++ ) 
			{
				s_ulIndex[ui] = BIG_C_InvalidIndex;
			}
		}
		/* Pass to brother */
		ul_FileIndex = BIG_NextFile(ul_FileIndex);
	}

	if ( m_ulOperationFile > P4_MAX_CHANGELIST_FILE ) 
	{
		if ( _bAutoSubmit ) 
		{
			// on auto submit cases we do not want to verify the links
			ULONG ulChangelist = GetWorkingChangelist();
			P4Submit (ulChangelist,FALSE);
		}
		// divinding into multiple change list to keep high performance
		m_ulWorkingChangelist = -1;
		m_ulOperationFile = 0;
	}

	if (_bFirst) 
	{
		m_ulOperationFile += (this->*_fpOperation)(&s_ulIndex[0],GetWorkingChangelist());	
		m_ulTotalFile = 0 ;
		for ( UINT ui = 0; ui < P4_MAX_FILE; ui ++ ) 
		{
			s_ulIndex[m_ulTotalFile] = BIG_C_InvalidIndex;
		}

		if ( _bAutoSubmit ) 
		{
			// on auto submit cases we do not want to verify the links
			ULONG ulChangelist = GetWorkingChangelist();
			P4Submit (ulChangelist,FALSE);
		}

		dwTime = GetTickCount() - dwTime;

		char aszMessage[MAX_PATH] = {0};
		int iSecond = (dwTime / 1000) % 60;
		int iMinute = ((dwTime / 1000) / 60) % 60;
		int iHour   = ((dwTime / 1000) / 60) / 60 ;

		sprintf (aszMessage," - Time for operation: %dh:%dm:%ds",iHour,iMinute,iSecond);
		DAT_CP4Message::OutputInfoMessage(aszMessage,TRUE);
	}

	return 0;
}

BOOL DAT_CPerforce::ExecuteOperation(	const UINT _uiDefaultArgCount,
										ClientUser* _pClientUser,
										int (DAT_CPerforce::*_fpOperation)(char** _pszArgs,UINT _uiArgCount,ClientUser* _pClientUser),
										const std::vector<std::string>& _vFiles,
										BOOL _bAutoSubmit /* = FALSE */ ) 
{

	UINT	uiArgCount = _uiDefaultArgCount;	// number of argument sent to the command
	ULONG	ulChangelistSuccesfulFiles = 0 ;				// real affected file count, reseted when P4_MAX_CHANGELIST_FILE is reached
	ULONG	ulTotalSuccessfulFiles = 0 ;
	char**	ppszArgs = new char*[P4_MAX_FILE];	// P4_MAX_FILE per command

	DWORD dwTime = GetTickCount();

	for ( UINT uiFiles = 0 ; uiFiles < _vFiles.size(); uiFiles ++ ) 
	{
		if ( _vFiles[uiFiles] != "" ) 
		{
			ppszArgs[uiArgCount++] = const_cast<char*>(_vFiles[uiFiles].c_str());
		}

		if	(	uiArgCount  == P4_MAX_FILE || // we exceeded the number of argument per command
				uiFiles		==  _vFiles.size() -1 // we are at the end of the requested files
			) 
		{ 
			// this will fill up the default args and then send the command to Perforce.
			ULONG ulSuccessfulFiles = (this->*_fpOperation)(ppszArgs,uiArgCount,_pClientUser);	
			ulChangelistSuccesfulFiles += ulSuccessfulFiles;
			ulTotalSuccessfulFiles += ulSuccessfulFiles;
			uiArgCount = _uiDefaultArgCount;
		}

		if	(	_bAutoSubmit && ( ulChangelistSuccesfulFiles >= P4_MAX_CHANGELIST_FILE || uiFiles	==  (_vFiles.size() -1) ) ) 
		{
			// on auto submit cases we do not want to verify the links
			ULONG ulChangelist = GetWorkingChangelist();
			P4Submit (ulChangelist,FALSE);
		}

		// we exeeded the number of files permitted for that changelist
		// next files will be put into the next one.
		if	(	ulChangelistSuccesfulFiles >= P4_MAX_CHANGELIST_FILE	&& uiFiles	!=  (_vFiles.size() -1)) 
		{
			SetWorkingChangelist(-1);
			ulChangelistSuccesfulFiles = 0 ;
		}	
	}

	dwTime = GetTickCount() - dwTime;

	char aszMessage[MAX_PATH] = {0};
	int iSecond = (dwTime / 1000) % 60;
	int iMinute = ((dwTime / 1000) / 60) % 60;
	int iHour   = ((dwTime / 1000) / 60) / 60 ;

	sprintf (aszMessage,"Time for operation: %dh:%dm:%ds - %d file(s) requested / %d file(s) successfully executed",
			iHour,iMinute,iSecond,_vFiles.size(),ulTotalSuccessfulFiles);
	DAT_CP4Message::OutputInfoMessage(aszMessage,TRUE);

	delete []ppszArgs;
	return TRUE;
}
//------------------------------------------------------------
//   ULONG DAT_CPerforce::UpdateBFFileBuffer()
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG DAT_CPerforce::UpdateBFFileBuffer(	BIG_KEY _ulKey, 
											void* _pBuffer,
											ULONG _ulBufferSize,
											const char* _pszBFFilepath)
{
	BIG_INDEX ul_Index = BIG_ul_SearchKeyToFat(_ulKey);

	if (ul_Index != BIG_C_InvalidIndex && !DAT_CUtils::GetTruncateFile(ul_Index) )  
	{
		// the user did not wish to update the file ( sounds or video ) 
		return BIG_C_InvalidIndex;	
	}

	char asz_P4PathName[BIG_C_MaxLenPath];
	char asz_P4FileName[BIG_C_MaxLenName];

	DAT_CUtils::ExtractPathAndFileName(_pszBFFilepath, asz_P4PathName, asz_P4FileName);

	// Set global variables so the update is done properly
	BIG_gb_CheckSameName = FALSE;
	BIG_gpsz_RealName = NULL;
	BIG_gul_GlobalKey = _ulKey;
	BIG_gpsz_GlobalName = asz_P4FileName;
	BIG_gp_Buffer = _pBuffer;
	BIG_gul_Length = _ulBufferSize;
	BIG_gx_GlobalTime = NULL;
	BIG_gb_IgnoreRecent = TRUE;

	ULONG ulIndex = BIG_ul_UpdateCreateFileOptim(BIG_C_InvalidIndex, BIG_C_InvalidIndex, asz_P4PathName, asz_P4FileName, NULL, FALSE);

	assert(ulIndex != BIG_C_InvalidIndex && "DAT_CPerforce::UpdateBFFileBuffer() - Could not update file in BF during sync");	

	// Once the update is done, reset variables to make 
	// sure they are not used with the current values
	BIG_gb_IgnoreRecent = FALSE;
	BIG_gx_GlobalTime = NULL;
	BIG_gp_Buffer = NULL;
	BIG_gul_Length = NULL;
	BIG_gb_CheckSameName = TRUE;
	BIG_gul_GlobalKey = BIG_C_InvalidKey;

	return ulIndex;
}

//------------------------------------------------------------
//   void DAT_CPerforce::CleanP4FilesNotInBF() 
/// \author    NBeaufils
/// \date      28-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CPerforce::CleanP4FilesNotInBF() 
{
	DAT_TP4FilesNotInBF::iterator Iter;
	for( Iter = m_mapP4FilesNotInBF.begin(); Iter != m_mapP4FilesNotInBF.end(); Iter++)
		delete (Iter->second);
	
	m_mapP4FilesNotInBF.clear();
}

//------------------------------------------------------------
//   void DAT_CPerforce::AddP4FileNotInBF(BIG_KEY _ul_Key, DAT_CP4ClientInfoFStat* pFStatInfo)
/// \author    NBeaufils
/// \date      28-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CPerforce::AddP4FileNotInBF(BIG_KEY _ul_Key, DAT_CP4ClientInfoFStat* pFStatInfo)
{
	DAT_TP4FilesNotInBF::iterator Iter = m_mapP4FilesNotInBF.find(_ul_Key) ;
	if ( Iter != m_mapP4FilesNotInBF.end() )
	{
		delete (Iter->second);
		m_mapP4FilesNotInBF.erase(Iter) ; 
	}
	m_mapP4FilesNotInBF.insert(make_pair(_ul_Key, pFStatInfo));
}

//------------------------------------------------------------
//   void DAT_CPerforce::RemP4FileNotInBF(BIG_KEY _ul_Key) 
/// \author    NBeaufils
/// \date      28-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CPerforce::RemP4FileNotInBF(BIG_KEY _ul_Key) 
{
	DAT_TP4FilesNotInBF::iterator Iter = m_mapP4FilesNotInBF.find(_ul_Key) ;
	if ( Iter != m_mapP4FilesNotInBF.end() )
	{
		delete (Iter->second);
		m_mapP4FilesNotInBF.erase(Iter) ; 
	}
}

//------------------------------------------------------------
//   void DAT_CPerforce::UpdateBF(std::vector<DAT_CP4ClientInfo>* _pFiles)
/// \author    NBeaufils
/// \date      01-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CPerforce::UpdateBF(const std::vector<DAT_CP4ClientInfo>& _vFiles)
{
	// Query P4 for the status of all the files 
	P4Fstat(_vFiles);

	std::vector<DAT_CP4ClientInfo>::const_iterator Iter;
	for ( Iter = _vFiles.begin() ; Iter < _vFiles.end() ; Iter++ )
	{
		BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat((*Iter).ulKey);
		
		// File is not present in BF, no need to process
		if ( ulIndex == BIG_C_InvalidIndex )
			continue;

		// Has file been removed from Perforce, if yes then remove from BF
		

		/// not needed anymore since the file are deleted when opened for delete
		// if ( strcmp((*Iter).aszAction, P4_ACTION_DELETE) == 0 )
		//	BIG_DeleteFile(BIG_ParentFile(ulIndex), ulIndex);
		// else
		{
			// For each file, update the time given by Perforce in the fat
			if ( BIG_P4Time(ulIndex) != P4_INVALIDTIME)
				BIG_TimeFile(ulIndex) = BIG_P4Time(ulIndex);
            
			// updating temp revision 
			BIG_TmpRevisionClient(ulIndex) = BIG_P4RevisionClient(ulIndex);

			// Save to the fat
			BIG_UpdateOneFileInFat(ulIndex);
		}

		// Remove file from list of files not in BF
		RemP4FileNotInBF(BIG_FileKey(ulIndex));
	}
}

//------------------------------------------------------------
//   std::string& DAT_CPerforce::GetP4Directories(const char* _szP4Root, const char* _szValueToAppend = NULL)
/// \author    NBeaufils
/// \date      2005-04-28
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
std::vector<char*>& DAT_CPerforce::GetP4Directories(const char* _szP4Root, const char* _szValueToAppend/*= NULL*/)
{
	// Get all P4 root directories contained in server
	if ( ! m_vcP4Directories.empty() )
	{
		for ( UINT i=0; i < m_vcP4Directories.size() ; i++ )
			delete [] m_vcP4Directories[i];
		m_vcP4Directories.clear();
	}

	char szP4Dir[12];
	char* pszFullDir = NULL;
	for ( UINT uiDirName = 0 ; uiDirName < P4_DIR_TOTAL ; uiDirName++ )
	{
		if ( _szValueToAppend != NULL )
			pszFullDir = new char[strlen(_szP4Root)+13+strlen(_szValueToAppend)];
		else
			pszFullDir = new char[strlen(_szP4Root)+13];
		
		pszFullDir[0] = '\0';

		sprintf(szP4Dir, "0x%02X/...", uiDirName);
		strcat(pszFullDir, _szP4Root);
		strcat(pszFullDir, szP4Dir);
		if ( _szValueToAppend != NULL )
			strcat(pszFullDir, _szValueToAppend);

		m_vcP4Directories.push_back(pszFullDir);
	}

	return m_vcP4Directories;
}

//------------------------------------------------------------------------------

