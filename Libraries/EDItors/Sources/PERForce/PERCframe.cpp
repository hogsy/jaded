//------------------------------------------------------------------------------
// Filename   :PERCframe.cpp
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: Implementation of PERCframe
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "EditLog.h"

#include "BROwser/BROframe.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "EDIpaths.h"
#include "EDIicons.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAname_dlg.h"
#include "SELection/SELection.h"
#include "PERCframe.h"
#include "EDImsg.h"
#include "EDIapp.h"
#include "MENu/MENmenu.h"
#include "MENu/MENsubmenu.h"
#include "DIAlogs/DIAfile_dlg.h"

#include "OUTput/OUTframe.h"
#include "AIscript/AIframe.h"

#ifdef JADEFUSION
#include "DIAlogs/DATCP4FileFilter.h"
#endif

#define ACTION_GLOBAL
#include "PERCframe_act.h"
#include "PERTreeCtrl.h"
#include <windowsx.h>

#include "DATaControl/DATCPerforce.h"
#include "DATaControl/DATCP4ClientUserChanges.h"
#include "DATaControl/DATCP4ClientUserOpened.h"
#include "BIGfiles\STReams\STReamaccess.h"
#include "DATaControl/DATCP4ClientInfo.h"

#include "DIAlogs\DIAmsglink_dlg.h"
#include "DIAlogs/DATCP4SettingsDlg.h"

#include "EDImainframe_act.h"

extern CWnd* GetSplashWindow( );
void DeleteLocalFiles(BIG_INDEX ul_DirIndex);

extern BOOL EDI_gb_BatchModeWithoutPerforce ;
#ifdef JADEFUSION
extern char	EDI_gaz_SlashL[1024];
#else
extern "C" char	EDI_gaz_SlashL[1024];
#endif
extern ULONG EDI_gul_EdiLock;


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------
#pragma comment(lib,"Rpcrt4.lib") // GUID convertions support
//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------
extern BOOL ERR_gb_Log;
extern VSSCB gpfn_VSSR;
extern void s_VssRecurse(char *, BIG_INDEX, int);
extern ULONG EDI_gul_EdiBigfileSyncChangelist;
extern ULONG EDI_gul_EdiBigfileSyncChangelistClean;
extern ULONG EDI_gul_EdiBigfileSyncForce;
extern ULONG EDI_gul_EdiBigfileFlushChangelist;
extern char  EDI_az_LogFileName[L_MAX_PATH];
#ifdef JADEFUSION
extern BOOL EDI_gb_EdiBigfileRefreshServerInfo;
#endif

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------
// message handing
static EPER_cl_Frame* s_pPerforceFrame = NULL;

void EPER_OutputMessage(const char* _pszMessage)
{
	DAT_CP4ClientApi::Log(_pszMessage);

    if (s_pPerforceFrame)
    	s_pPerforceFrame->OutputMessage(_pszMessage,0);
}


//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------


IMPLEMENT_DYNCREATE(EPER_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EPER_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(IDC_P4REFRESH, OnRefresh)
	ON_COMMAND(IDC_P4DEFAULT, OnDefault)
#ifdef JADEFUSION
	ON_COMMAND(IDC_P4CHECK_FILTERSETTINGS, OnCheckFilterSettings)
	ON_COMMAND(IDC_P4BUTTON_FILTERSETTINGS, OnClickFilterSettings)
#endif
END_MESSAGE_MAP()


//------------------------------------------------------------
//   EPER_cl_Frame::EPER_cl_Frame(void)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
EPER_cl_Frame::EPER_cl_Frame(void) : 
EDI_cl_BaseFrame(),
m_pEditLog(NULL)
{
	mpo_TreeView = new EPER_cl_TreeCtrl(this);
	mpo_InsideSplitter = new CSplitterWnd;
	mpo_Edit = new CEdit;
	s_pPerforceFrame = this;
	mb_Locked = FALSE;
}


//------------------------------------------------------------
//   EPER_cl_Frame::~EPER_cl_Frame(void)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
EPER_cl_Frame::~EPER_cl_Frame(void)
{
	ClearChangelist(-1);

	mpo_InsideSplitter->DestroyWindow();
	delete mpo_InsideSplitter;

	mpo_TreeView->DestroyWindow();
	delete mpo_TreeView;

	mpo_Edit->DestroyWindow();
	delete mpo_Edit;

	delete m_pEditLog;
	s_pPerforceFrame = NULL;

}

//------------------------------------------------------------
//   int EPER_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int EPER_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1)
	{
		return -1;
	}

	/* Create inside splitter. */
	mpo_InsideSplitter->CreateStatic(this, 2, 1, WS_CHILD | WS_VISIBLE);


	/* Tree ctrl */
	mpo_TreeView->Create
		(
		WS_VISIBLE |  WS_TABSTOP | WS_CHILD |TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
		CRect(0, 50, 100, 100),
		mpo_InsideSplitter,
		mpo_InsideSplitter->IdFromRowCol(0, 0)
		);
	
	/* Size of pane */
	mpo_InsideSplitter->SetRowInfo(0, 200, 60);

	/* Create edit box */
	mpo_Edit->Create
		(
		WS_HSCROLL | WS_VSCROLL | WS_VISIBLE | WS_BORDER | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		CRect(100, 100, 200, 150),
		mpo_InsideSplitter,
		mpo_InsideSplitter->IdFromRowCol(1, 0)
		);
	mpo_Edit->SetFont(&M_MF()->mo_Fnt);

	m_pEditLog = new CEditLog(mpo_Edit->m_hWnd,100);

 	CBitmap o_Bmp;
	o_Bmp.LoadBitmap(MAKEINTRESOURCE(IDB_PERFORCE));
	m_ImageList.Create(24, 16, ILC_COLOR16| ILC_MASK, 0, 10);
	m_ImageList.Add(&o_Bmp, RGB(0, 0, 0));

    mpo_TreeView->SetImageList(&m_ImageList, TVSIL_NORMAL);
	
	
	CComboBox* pComboBox;
	pComboBox = (CComboBox *) (mpo_DialogBar->GetDlgItem( IDC_P4CHANGELIST_STATUS ));
	pComboBox->ResetContent();

	CButton* pButton = (CButton *) (mpo_DialogBar->GetDlgItem( IDC_P4REFRESH ));
	pButton->SetIcon(m_ImageList.ExtractIcon(eICON_REFRESH));

	pButton = (CButton *) (mpo_DialogBar->GetDlgItem( IDC_P4DEFAULT ));
	pButton->SetIcon(m_ImageList.ExtractIcon(eICON_RESET));

	DWORD dwIndex = pComboBox->AddString((LPCSTR)P4_STATUS_PENDING);
	pComboBox->SetItemDataPtr(dwIndex,(void*)(LPCSTR)P4_STATUS_PENDING);
	
	dwIndex = pComboBox->AddString((LPCSTR)P4_STATUS_SUBMITTED);
	pComboBox->SetItemDataPtr(dwIndex,(void*)(LPCSTR)P4_STATUS_SUBMITTED);
	pComboBox->SetCurSel(0);

	pComboBox = (CComboBox *) (mpo_DialogBar->GetDlgItem( IDC_P4MAXCHANGELIST ));
	pComboBox->ResetContent();
	dwIndex = pComboBox->AddString((LPCSTR)"10");
	pComboBox->SetCurSel(dwIndex);
	pComboBox->SetItemDataPtr(dwIndex,(void*)(LPCSTR)"10");
	dwIndex = pComboBox->AddString((LPCSTR)"15");
	pComboBox->SetItemDataPtr(dwIndex,(void*)(LPCSTR)"15");
	dwIndex = pComboBox->AddString((LPCSTR)"25");
	pComboBox->SetItemDataPtr(dwIndex,(void*)(LPCSTR)"25");
	dwIndex = pComboBox->AddString((LPCSTR)"100");
	pComboBox->SetItemDataPtr(dwIndex,(void*)(LPCSTR)"100");

	mpo_InsideSplitter->RecalcLayout();
	RecalcLayout();
	return 0;
}


//------------------------------------------------------------
//   void EPER_cl_Frame::OutputMessage(const char* _pszMessage,DWORD _dwColor)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OutputMessage(const char* _pszMessage,DWORD _dwColor)
{
	if(ERR_gb_Log)
	{
		FILE *f;
		f = fopen(EDI_az_LogFileName, "at");
		if(!f) f = fopen(EDI_az_LogFileName, "wt");
		fwrite(_pszMessage, 1, strlen(_pszMessage), f);
		fclose(f);
	}

	m_pEditLog->AddText(_pszMessage,true);
}


//------------------------------------------------------------
//   BOOL EPER_cl_Frame::PerforceCheckFileUpToDate(BIG_INDEX ulIndex)
/// \author    YCharbonneau
/// \date      2005-02-14
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPER_cl_Frame::PerforceCheckFileUpToDate(std::vector<BIG_INDEX>& _vFileIndex,BOOL _bDiffDetectedChanged /* = TRUE */  )
{
	std::vector<std::string> vFilesToSync;
	std::vector<std::string> vFilesToDiff;
	
	/* added */ 
	std::vector<BIG_INDEX> vFilesIndex;
	/* */

	char aszMessage[MAX_PATH] = {0};
	sprintf(aszMessage,P4_USRMSG_SYNC_BEFORE_OPERATION,"");
	EDIA_cl_MsgLinkDialog	SyncModifiedMessage(&aszMessage[0],"Confirm");
	SyncModifiedMessage.mb_ApplyToAll = FALSE;

	for ( UINT ui = 0 ; ui < _vFileIndex.size(); ui ++  ) 
	{
		BIG_INDEX ulIndex = _vFileIndex[ui];
		if ( ulIndex == BIG_C_InvalidIndex ) 
			continue;

		// trying to detect changes just by checking the infos we have

		if (	BIG_P4RevisionServer(ulIndex) != 0 
			&& strcmp(BIG_P4Action(ulIndex),"new") != 0 
			&& strcmp(BIG_P4OwnerName(ulIndex),DAT_CPerforce::GetInstance()->GetUser()) != 0 
			// revision or time is outdated, sync the file
			&&(	BIG_P4RevisionClient(ulIndex) != BIG_P4RevisionServer(ulIndex) 
				|| BIG_P4Time(ulIndex)  != BIG_TimeFile(ulIndex) )
			)
		{
			std::string strP4File;
			DAT_CUtils::GetP4FileFromKey(BIG_FileKey(ulIndex),strP4File,DAT_CPerforce::GetInstance()->GetP4Root());
			vFilesToDiff.push_back(strP4File);

			/* added */ 
			vFilesIndex.push_back( ulIndex ) ; // detecting a possible difference
			/* */
		}
	}

	if ( vFilesToDiff.size() > 0 ) 
	{
		// every simple test failed, we do a diff on the file to see if it has 
		// really changed

		std::vector<DAT_CP4ClientInfo*> vDiffFiles;

		// if we really want to make a diff of the content
		if ( _bDiffDetectedChanged ) 
		{
			DAT_CPerforce::GetInstance()->P4Diff(vFilesToDiff,&vDiffFiles);
			vFilesIndex.clear();

			for ( UINT ui = 0 ; ui < vDiffFiles.size(); ui ++ )
			{
				BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat(vDiffFiles[ui]->ulKey);

				if ( (((DAT_CP4ClientInfoDiff*)vDiffFiles[ui])->dwFileDiff & 
					(DAT_CP4ClientInfoDiff::eFileModified|DAT_CP4ClientInfoDiff::eFileRenamed|DAT_CP4ClientInfoDiff::eFileMoved)) ||
					BIG_P4RevisionClient(ulIndex) != BIG_P4RevisionServer(ulIndex))
				{
					vFilesIndex.push_back(ulIndex);
				}
				delete vDiffFiles[ui];
			}
		}
		vFilesToDiff.clear();

		for ( UINT ui = 0 ; ui < vFilesIndex.size(); ui ++ )
		{
			char aszMessage[MAX_PATH] = {0};
			sprintf(aszMessage,P4_USRMSG_SYNC_BEFORE_OPERATION,BIG_NameFile(vFilesIndex[ui]));
			SyncModifiedMessage.mo_Text = aszMessage;
			int iRet = SyncModifiedMessage.DoModal();

			if ( iRet == IDYES )
			{
				std::string strP4File;
				DAT_CUtils::GetP4FileFromKey(BIG_FileKey(vFilesIndex[ui]),strP4File,DAT_CPerforce::GetInstance()->GetP4Root());
				vFilesToSync.push_back(strP4File);
			}
			else if ( iRet == IDCANCEL ) 
			{
				return FALSE;
			}
		}

		if ( vFilesToSync.size() > 0)
		{
			if ( ! IsWorldCurrentlyOpened() )
				return FALSE;

			DAT_CPerforce::GetInstance()->P4Sync(vFilesToSync,"",TRUE);
		}
	}

	return TRUE;
}

//------------------------------------------------------------
//   BOOL EPER_cl_Frame::IsWorldCurrentlyOpened()
/// \author    Quentin Gallet
/// \date      2005-02-14
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPER_cl_Frame::IsWorldCurrentlyOpened()
{
	// Loop through the four possible frame with AI
	for ( int i=0 ; i < 4 ; i++ )
	{
		// Get the AI frame
		EAI_cl_Frame* po_AI = (EAI_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_AI, i);

		// If AI frame has set option to not close world before an perforce action (checkout, etc...)
		if ( po_AI != NULL && ! po_AI->mb_P4CloseWorld )
			return TRUE;
	}

	// Check if a world is open in the 3DView
	BOOL bIsOpenWorld = FALSE;
	EOUT_cl_Frame *po_Out;

	// Loop through the four possible frame with 3D View
	for ( int i=0 ; i < 4 ; i++ )
	{
		// Get the 3D View frame
		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, i);

		// If 3D View frame is present, check if a world is open
		if ( po_Out != NULL && po_Out->mul_CurrentWorld != BIG_C_InvalidIndex )
		{
			bIsOpenWorld = TRUE ;
			break;
		}
	}
	
	if ( bIsOpenWorld )
	{
		char aszWorldName[BIG_C_MaxLenName];
		strcpy(aszWorldName, BIG_NameFile(po_Out->mul_CurrentWorld));
		*strchr(aszWorldName, '.') = 0;

		char aszMessage[MAX_PATH];
		sprintf(aszMessage, "'%s' is currently open. Before doing an action on Perforce you need to\nclose the world. Do you wish to close the world now ?", aszWorldName);
		if ( M_MF()->MessageBox(aszMessage,"Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{		
			po_Out->CloseWorld();
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OpenProject(void)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OpenProject(void)
{
	EDI_cl_BaseFrame::OpenProject();
	
	if ( !SetupPerforce() ) 
		return;


	char pszBFGUID [MAX_PATH] = {0};
	GetStreamByName(BIG_gst.asz_Name ,STREAM_BFGUID,pszBFGUID); 

	if ( pszBFGUID[0] == 0 ) 
	{
		GUID BFGuid;
		CoCreateGuid(&BFGuid);
		UCHAR* aszBFGUID;
		UuidToString(&BFGuid,&aszBFGUID);
		strcpy(pszBFGUID,(char*)aszBFGUID);
		RpcStringFree(&aszBFGUID);
		SetStreamByName(BIG_gst.asz_Name ,STREAM_BFGUID,pszBFGUID);
	}

	BOOL bRefreshClientView = FALSE;

	// We don't want user to be able to abort the P4 commands when Jade starts
	BOOL bDisableAbort = TRUE;
	if ( DAT_CPerforce::GetInstance()->P4Connect(bDisableAbort) ) 
	{
		std::string strDescription;

		std::list<std::string> lstVersion; 

		// check if the BF GUID has changed in the client spec
		DAT_CPerforce::GetInstance()->P4Client(DAT_CPerforce::GetInstance()->GetBFVersion(),lstVersion ,strDescription,FALSE ) ;

		// client view needs to be refresh, we have a different bf
		if ( strDescription != pszBFGUID )  
		{
			bRefreshClientView = TRUE;
		}
		DAT_CPerforce::GetInstance()->P4Disconnect();
	}


	CComboBox* pComboBox;
	pComboBox = (CComboBox *) (mpo_DialogBar->GetDlgItem( IDC_P4USER ));
	ComboBoxResetContentAndDeleteDataPtr( IDC_P4USER );

	DWORD dwIndex ;
	dwIndex = pComboBox->AddString((LPCSTR)"-- ALL --");
	pComboBox->SetItemDataPtr(dwIndex,NULL);

	// We don't want user to be able to abort the P4 commands when Jade starts
	bDisableAbort = TRUE;
	if ( DAT_CPerforce::GetInstance()->P4Connect(bDisableAbort) ) 
	{
		std::list<std::string> lUsers;
		std::list<std::string> lClients;
		m_lstVersion.clear();
		std::list<std::string> lstDirs;
		std::string strDir = "//" + DAT_CPerforce::GetInstance()->GetP4Depot() + "/" + std::string(P4_VERSIONROOT);
		strDir += "*";


		DAT_CPerforce::GetInstance()->P4Users(lUsers);
		DAT_CPerforce::GetInstance()->P4Dirs(strDir.c_str(),lstDirs);

		// update the client view with the current data.
		DAT_CPerforce::GetInstance()->P4Client(DAT_CPerforce::GetInstance()->GetBFVersion(), lstDirs,std::string(pszBFGUID),TRUE ) ;

		DAT_CPerforce::GetInstance()->P4Disconnect();

		for ( std::list<std::string>::iterator iter = lUsers.begin(); iter != lUsers.end(); iter++ ) 
		{
			char* pUser = new char[(*iter).size() + 1];
			strcpy(pUser,(*iter).c_str());
			dwIndex = pComboBox->AddString((LPCSTR)pUser);
			pComboBox->SetItemDataPtr(dwIndex,(void*)(LPCSTR)pUser);

			if ((*iter) == DAT_CPerforce::GetInstance()->GetUser() ) 
			{
				pComboBox->SetCurSel(dwIndex);
			}
		}

		pComboBox = (CComboBox *) (mpo_DialogBar->GetDlgItem( IDC_P4VERSION ));
		pComboBox->ResetContent();

		dwIndex = pComboBox->AddString((LPCSTR)"-- ALL --");
		pComboBox->SetItemDataPtr(dwIndex,NULL);

		std::string strHost = DAT_CPerforce::GetInstance()->GetHost();
		strHost += "-";
		std::string strClientVersion = DAT_CPerforce::GetInstance()->GetClient() + strHost.length();

		// getting the versions

		STL::list<STL::string>::iterator iterDir = lstDirs.begin();
		UINT ui = 0;

		for ( iterDir; iterDir != lstDirs.end(); iterDir++ ) 
		{
			char* pszVersion = const_cast<char*>((*iterDir).c_str()) ;
			pszVersion += strDir.length() - 1;
			m_lstVersion.push_back(pszVersion);
			ui++;
		}

		BOOL bFound = FALSE;
		for ( std::list<std::string>::iterator iter = m_lstVersion.begin(); iter != m_lstVersion.end(); iter++ ) 
		{
			std::string strVersion = iter->c_str();
			dwIndex = pComboBox->AddString((LPCSTR)(strVersion.c_str()));
			pComboBox->SetItemDataPtr(dwIndex,(void*)iter->c_str());

			if (stricmp(strClientVersion.c_str(),iter->c_str()) == 0)
			{
				pComboBox->SetCurSel(dwIndex);
				bFound	 = TRUE;
			}
		}

		if ( !bFound ) 
		{
			dwIndex = pComboBox->AddString((LPCSTR)strClientVersion.c_str());
			static char aszVersion[MAX_PATH] = {0};
			strcpy(aszVersion,strClientVersion.c_str());
			pComboBox->SetItemDataPtr(dwIndex,(void*)aszVersion);
			pComboBox->SetCurSel(dwIndex);
		}

		
	}


	// Connect to Perforce server and validate bigfile revision 
	// against client revision stored on Perforce server

	// We don't want user to be able to abort the P4 commands when Jade starts
	bDisableAbort = TRUE;
	if ( bRefreshClientView && DAT_CPerforce::GetInstance()->P4Connect(bDisableAbort) ) 
	{
		DAT_CPerforce::GetInstance()->P4FlushBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str());
		DAT_CPerforce::GetInstance()->P4Disconnect();
	}

	// we do not want to refresh changelist, it will take to much time
	if ( !EDI_gul_EdiLock ) 
	{
		Refresh();
	}
}

BOOL EPER_cl_Frame::SetupPerforce()
{
	if ( EDI_gb_BatchModeWithoutPerforce ) 
	{
		DAT_CPerforce::GetInstance()->PermanentDisable();
		return false;
	}

	// these two values can be overwrite if the stream value is set
	// this is usualy for debug purpose only and is not set into the code. You'll have to set it 
	// within the stream itself
	char aszStreamUserName	[MAX_PATH]	= {0};
	char aszStreamHostName	[MAX_PATH]	= {0};
	char aszStreamP4Port	[MAX_PATH]	= {0};
	char aszStreamBfVersion	[MAX_PATH]	= {0};
	char aszStreamP4Depot	[MAX_PATH]	= {0};
	char aszStreamP4Password[MAX_PATH]	= {0};

	GetStreamByName(BIG_gst.asz_Name ,STREAM_P4PORT		,aszStreamP4Port	); 

	if ( aszStreamP4Port[0]	!= 0 ) 
	{
		GetStreamByName(BIG_gst.asz_Name ,STREAM_P4USER		,aszStreamUserName	); 
		GetStreamByName(BIG_gst.asz_Name ,STREAM_P4HOST		,aszStreamHostName	); 
		GetStreamByName(BIG_gst.asz_Name ,STREAM_BFVERSION	,aszStreamBfVersion	); 
		GetStreamByName(BIG_gst.asz_Name ,STREAM_P4DEPOT	,aszStreamP4Depot	); 
		GetStreamByName(BIG_gst.asz_Name ,STREAM_P4PASSWORD	,aszStreamP4Password); 
	}

	// by default we use the login name to login to server 
	// change stream P4User.stream to the user you want to your needs. 
	char aszUserName [MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH;
	GetUserName(aszUserName,&dwSize);

	// by default client view is setup with computer name.
	char aszComputerName[MAX_PATH]= {0};
	dwSize = MAX_PATH;
	GetComputerNameEx(ComputerNameDnsHostname ,aszComputerName,&dwSize);

	if ( aszStreamUserName[0] != 0 )  
	{ 
		strcpy(aszUserName,aszStreamUserName); 
		OutputMessage("USER stream is set\n"); 
	}

	if ( aszStreamHostName[0] != 0 )  
	{ 
		strcpy(aszComputerName,aszStreamHostName); 
		OutputMessage("HOST stream is set\n");
	}

	if ( aszStreamP4Password[0] == 0 ) 
	{
		strcpy(aszStreamP4Password,aszUserName);
		strcat(aszStreamP4Password,P4_DEFAULT_PASSWORD);
	}

	unsigned short shKeyState = GetKeyState(VK_SHIFT);
	unsigned short ctrlKeyState = GetKeyState(VK_CONTROL);
	shKeyState >>= 15;
	ctrlKeyState >>= 15;
	if( shKeyState == 1 && ctrlKeyState == 1)
	{
		DAT_CP4SettingsDlg P4SettingsDlg(	aszStreamP4Port,
											aszStreamP4Depot,
											aszStreamBfVersion,
											aszComputerName,
											aszUserName,
											aszStreamP4Password,
											GetSplashWindow() );

		if ( P4SettingsDlg.DoModal() == IDOK ) 
		{
				P4SettingsDlg.GetP4Port			(aszStreamP4Port		);
				P4SettingsDlg.GetP4Depot		(aszStreamP4Depot		);
				P4SettingsDlg.GetBfVersion		(aszStreamBfVersion		);
				P4SettingsDlg.GetP4Username		(aszUserName			);
				P4SettingsDlg.GetP4Password		(aszStreamP4Password	);
				P4SettingsDlg.GetP4Host			(aszComputerName		);
		}
		else 
		{	
			DAT_CPerforce::GetInstance()->Disable() ;
			return FALSE;
		}
	}

	if	(	aszStreamP4Port[0]		== 0 || 
			aszStreamBfVersion[0]	== 0 || 
			aszStreamP4Depot[0]		== 0 || 
			aszUserName[0]			== 0 || 
			aszStreamP4Password[0]	== 0 ||
			aszComputerName[0]		== 0 
		) 
	{
		OutputMessage("Perforce is disable for this Bigfile. Contact your administrator if it is not expected. Restore perforce server IP stream to enable connect\n");
		DAT_CPerforce::GetInstance()->Disable();
		return FALSE;
	}

	L_strcpy(M_MF()->mst_Ini.asz_CurrentUserName, aszUserName);
	DAT_CPerforce::Initialize(aszStreamP4Port,aszComputerName,aszUserName,aszStreamP4Password);
	
	DAT_CPerforce::GetInstance()->SetBFVersion(std::string(aszStreamBfVersion));
	DAT_CPerforce::GetInstance()->SetP4Depot(std::string(aszStreamP4Depot));

	std::string strP4Root = "//" + std::string(aszStreamP4Depot) + "/" + std::string(P4_VERSIONROOT) + std::string(aszStreamBfVersion) + "/";
	DAT_CPerforce::GetInstance()->SetP4Root( strP4Root );

	return TRUE;
}

//------------------------------------------------------------
//   BOOL EPER_cl_Frame::SelectPerforceDepot(char* _pszPerforceDepot)
/// \author    YCharbonneau
/// \date      18/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
//BOOL EPER_cl_Frame::SelectPerforceDepot(char* _pszPerforceDepot)
//{
//	CWnd* pSplashWnd = GetSplashWindow( );
//	EDIA_cl_NameDialogCombo o_TypeDlg("Choose perforce depot...", pSplashWnd);
//
//
//	o_TypeDlg.AddItem(P4_PROD_RESOLVED_PORT,0);
//	o_TypeDlg.AddItem(P4_PROD_PORT ,1);
//
//	if(o_TypeDlg.DoModal() != IDOK) 
//	{
//		DAT_CPerforce::GetInstance()->Disable();
//		return FALSE;
//	}
//
//	AfxGetApp()->DoWaitCursor(1);
//	SetStreamByName(BIG_gst.asz_Name ,STREAM_P4DEPOT	,o_TypeDlg.mo_Name); 
//	strcpy(_pszPerforceServer,o_TypeDlg.mo_Name);
//	AfxGetApp()->DoWaitCursor(-1);
//	return TRUE;
//}
//------------------------------------------------------------
//   BOOL EPER_cl_Frame::SelectPerforceServer(char* _pszPerforceServer)
/// \author    YCharbonneau
/// \date      18/03/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPER_cl_Frame::SelectPerforceServer(char* _pszPerforceServer)
{
	/*CWnd* pSplashWnd = GetSplashWindow( );
	EDIA_cl_NameDialogCombo o_TypeDlg("Choose perforce server...", pSplashWnd);

	o_TypeDlg.AddItem(P4_PROD_RESOLVED_PORT,0);
	o_TypeDlg.AddItem(P4_PROD_PORT ,1);

	if(o_TypeDlg.DoModal() != IDOK) 
	{
		DAT_CPerforce::GetInstance()->Disable();
		return FALSE;
	}

	AfxGetApp()->DoWaitCursor(1);
	SetStreamByName(BIG_gst.asz_Name ,STREAM_P4PORT	,o_TypeDlg.mo_Name); 
	strcpy(_pszPerforceServer,o_TypeDlg.mo_Name);
	AfxGetApp()->DoWaitCursor(-1);*/
	return TRUE;
}

//------------------------------------------------------------
//   void EPER_cl_Frame::SelectBigFileVersion()
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPER_cl_Frame::SelectBigFileVersion(char* _pszBfVersion)
{
	//// client view has not been set wet.
	//if ( DAT_CPerforce::GetInstance()->P4Connect() )
	//{
	//	m_lstVersion.clear();
	//	std::list<std::string> lstDirs;

	//	std::string strDir = P4_VERSIONROOT;
	//	strDir += "*";
	//	DAT_CPerforce::GetInstance()->P4Dirs(strDir.c_str(),lstDirs);

	//	///*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//	///*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~«~~~~~~~~~~~~*/

	//	STL::list<STL::string>::iterator iter= lstDirs.begin();
	//	UINT ui = 0;

	//	for ( iter; iter != lstDirs.end(); iter++ ) 
	//	{
	//		char* pszVersion = const_cast<char*>((*iter).c_str()) ;
	//		pszVersion += strlen(P4_VERSIONROOT);
	//		m_lstVersion.push_back(pszVersion);
	//		ui++;
	//	}

	//	if ( _pszBfVersion[0] == 0 ) 
	//	{
	//		CWnd* pSplashWnd = GetSplashWindow( );
	//		EDIA_cl_NameDialogCombo o_TypeDlg("Choose perforce version...", pSplashWnd);
	//		STL::list<STL::string>::iterator iter = m_lstVersion.begin();
	//		UINT ui = 0;

	//		for ( iter; iter != m_lstVersion.end(); iter++ ) 
	//		{
	//			o_TypeDlg.AddItem(const_cast<char*>(iter->c_str()), ui);
	//			ui++;
	//		}

	//		if(o_TypeDlg.DoModal() != IDOK) 
	//		{
	//			DAT_CPerforce::GetInstance()->P4Disconnect();
	//			return FALSE;
	//		}

	//		AfxGetApp()->DoWaitCursor(1);
	//		
	//		SetStreamByName(BIG_gst.asz_Name ,STREAM_BFVERSION,o_TypeDlg.mo_Name); 
	//		SetStreamByName(BIG_gst.asz_Name ,STREAM_P4USER,""); 
	//		SetStreamByName(BIG_gst.asz_Name ,STREAM_P4HOST	,""); 
	//		strcpy(_pszBfVersion,o_TypeDlg.mo_Name);

	//		AfxGetApp()->DoWaitCursor(-1);
	//	}
	//	DAT_CPerforce::GetInstance()->P4Disconnect();
	//	return TRUE;
	//}
	return FALSE;
	
}



//------------------------------------------------------------
//   void EPER_cl_Frame::OnSize(UINT nType, int cx, int cy)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
    if(mpo_TreeView) mpo_TreeView->MoveWindow(5, 0, cx - 10, cy - 5);
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);

	CRect		o_Rect;
	CComboBox	*po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4CHANGELIST_STATUS);
	po_Combo->GetWindowRect(o_Rect);
	mpo_DialogBar->ScreenToClient(o_Rect);
	po_Combo->SetWindowPos(0, 0, 0, o_Rect.right - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4USER);
	po_Combo->GetWindowRect(o_Rect);
	mpo_DialogBar->ScreenToClient(o_Rect);
	po_Combo->SetWindowPos(0, 0, 0, o_Rect.right - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4VERSION);
	po_Combo->GetWindowRect(o_Rect);
	mpo_DialogBar->ScreenToClient(o_Rect);
	po_Combo->SetWindowPos(0, 0, 0, o_Rect.right - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4DESCRIPTION);
	po_Combo->GetWindowRect(o_Rect);
	mpo_DialogBar->ScreenToClient(o_Rect);
	po_Combo->SetWindowPos(0, 0, 0, o_Rect.right - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4KEY);
	po_Combo->GetWindowRect(o_Rect);
	mpo_DialogBar->ScreenToClient(o_Rect);
	po_Combo->SetWindowPos(0, 0, 0, o_Rect.right - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4CHANGELISTID);
	po_Combo->GetWindowRect(o_Rect);
	mpo_DialogBar->ScreenToClient(o_Rect);
	po_Combo->SetWindowPos(0, 0, 0, o_Rect.right - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);

	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4MAXCHANGELIST);
	po_Combo->GetWindowRect(o_Rect);
	mpo_DialogBar->ScreenToClient(o_Rect);
	po_Combo->SetWindowPos(0, 0, 0, o_Rect.right - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);


	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4FILENAME);
	po_Combo->GetWindowRect(o_Rect);
	mpo_DialogBar->ScreenToClient(o_Rect);
	po_Combo->SetWindowPos(0, 0, 0, o_Rect.right - o_Rect.left, 200, SWP_NOMOVE | SWP_NOZORDER);


}

//------------------------------------------------------------
//   void    EPER_cl_Frame::OnRealIdle(void)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------

void    EPER_cl_Frame::OnRealIdle(void)
{
#ifdef JADEFUSION
	if ( EDI_gb_EdiBigfileRefreshServerInfo )
	{
		if( DAT_CPerforce::GetInstance( )->P4Connect( ) )
		{
			DAT_CPerforce::GetInstance()->P4FlushBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str());

			// reset
			EDI_gb_EdiBigfileRefreshServerInfo = FALSE;

			DAT_CPerforce::GetInstance( )->P4Disconnect( );

			// If we must sync, then don't close yet the editor
			if ( ! (EDI_gul_EdiBigfileSyncChangelist > 0) )
			{
				M_MF()->OnAction(EDI_ACTION_CLOSEPROJECT);
				M_MF()->OnAction(EDI_ACTION_EXIT);
				ExitProcess(0);
			}
		} 
	}
#endif

	if( EDI_gul_EdiBigfileSyncChangelist > 0 )
	{
		if( DAT_CPerforce::GetInstance( )->P4Connect( ) )
		{
			std::vector<DAT_CP4ClientInfo> vOpenedFiles;
			DAT_CPerforce::GetInstance( )->P4Opened(&vOpenedFiles);
			if ( vOpenedFiles.size() > 0 ) 
			{
				M_MF()->MessageBox("You have files in checkout, those files will not be retreived. Please make sure you revert and sync those files before you use this BF.", "WARNING",MB_OK);
			}

			if ( EDI_gul_EdiBigfileSyncChangelistClean ) 
			{
				DAT_CPerforce::GetInstance()->P4FStatBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str());
				DeleteLocalFiles(BIG_Root());
				M_MF()->FatHasChanged();
				EDI_gul_EdiBigfileSyncChangelistClean = 0;
			}

			// do sync
			DAT_CPerforce::GetInstance( )->P4SyncBF
				(
				DAT_CPerforce::GetInstance()->GetP4Root().c_str(),
				NULL,
                EDI_gul_EdiBigfileSyncForce,
				NULL,
				EDI_gul_EdiBigfileSyncChangelist
				);

			// reset
			EDI_gul_EdiBigfileSyncChangelist = 0;

			DAT_CPerforce::GetInstance( )->P4Disconnect( );
			M_MF()->OnAction(EDI_ACTION_CLOSEPROJECT);
			M_MF()->OnAction(EDI_ACTION_EXIT);
#ifdef JADEFUSION
			ExitProcess(0);
#else
			exit(1);
#endif
		} 
	}
    else if (EDI_gul_EdiBigfileFlushChangelist)
    {
		if( DAT_CPerforce::GetInstance( )->P4Connect( ) )
		{
			DAT_CPerforce::GetInstance()->P4FlushBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str());
			DAT_CPerforce::GetInstance( )->P4Disconnect( );

			// reset
            EDI_gul_EdiBigfileFlushChangelist = 0;

			M_MF()->OnAction(EDI_ACTION_CLOSEPROJECT);
			M_MF()->OnAction(EDI_ACTION_EXIT);
			exit(1);
        }
    }
	else if ( EDI_gul_EdiLock > 0 ) 
	{

		BOOL bPath = (strlen(EDI_gaz_SlashL) != 0) ;
		char	aszFilename[MAX_PATH] = {0};

		// if no path has been specified, ask for it
		if(!bPath)
		{
			EDIA_cl_FileDialog	o_Dlg("Choose directory/file to lock/unlock", 2, FALSE,TRUE);
			char*	pc_Cur;
			char	aszPath[1024] = {0};

			if (  o_Dlg.DoModal() == IDOK)
			{
				L_strcpy(EDI_gaz_SlashL, (char *) (LPCSTR) o_Dlg.masz_FullPath);
				L_strcpy(aszFilename, (char *) (LPCSTR) o_Dlg.mo_File);
				
				pc_Cur = aszPath;
				while(*pc_Cur)
				{
					if(*pc_Cur == '\\') *pc_Cur = '/';
					pc_Cur++;
				}
				
				if ( strchr(EDI_gaz_SlashL,'.') ) 
				{
					strcpy(aszFilename,strchr(EDI_gaz_SlashL,'/') + 1);
				}
				
				if((pc_Cur > EDI_gaz_SlashL) && (pc_Cur[-1] == '/')) pc_Cur[-1] = 0;
								
				bPath = TRUE;
			}
			else 
			{
				EPER_OutputMessage("Operation has been canceled");
			}
		}

		BIG_INDEX ulDir		= BIG_ul_SearchDir(EDI_gaz_SlashL);
		
	

		if (	bPath && (ulDir== BIG_C_InvalidIndex) )
		{
			char UserMessage[MAX_PATH] = {0};
			sprintf(UserMessage,"ERROR: Invalid directory - %s \n", EDI_gaz_SlashL);
			EPER_OutputMessage(UserMessage);
			bPath = FALSE;
		}

		

		if ( bPath && DAT_CPerforce::GetInstance( )->P4Connect( ) )
		{
			BIG_INDEX ulFile = BIG_C_InvalidIndex;
			
			if (strlen(aszFilename) > 0 )
			{
				aszFilename[strlen(aszFilename)-1]=0;
				ulFile		= BIG_ul_SearchFile(ulDir,aszFilename);
			}
			//// do sync
			DAT_CPerforce::GetInstance( )->P4SyncBF
				(
				DAT_CPerforce::GetInstance()->GetP4Root().c_str(),
				NULL,
				FALSE,
				NULL,
				-1
				);
			
			DAT_CPerforce::GetInstance()->P4FStatBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str());
			

			Lock((EDI_gul_EdiLock == 1),ulDir,ulFile,EDI_gaz_SlashL);
			DAT_CPerforce::GetInstance( )->P4Disconnect( );
		}
		M_MF()->OnAction(EDI_ACTION_CLOSEPROJECT);
		M_MF()->OnAction(EDI_ACTION_EXIT);
		exit(1);
	}
}


void EPER_cl_Frame::Lock(BOOL _bLock, BIG_INDEX ulDir,BIG_INDEX ulFile,char* _pszPath )
{
	std::vector<std::string> vP4File;
	std::vector<BIG_INDEX> vFile;
	std::vector<BIG_INDEX> vFileToCheckout;
	std::vector<BIG_INDEX> vFileCheckedOutByCurrentUser;
	std::vector<std::string> vFileCheckoutByAnotherUser;

	if ( ulFile == BIG_C_InvalidIndex ) 
	{
		DAT_CUtils::GetBFIndexesFromDirIndex(ulDir,vFile,TRUE);
	}
	else 
	{
		vFile.push_back(ulFile);
	}

	if(EDI_gpo_EnterWnd) 
		EDI_gpo_EnterWnd->DisplayMessage("Trying to lock files, please wait...");

	for ( UINT ui = 0 ; ui < vFile.size(); ui ++ )
	{
		// file is checkout by someone
		if ( strlen(BIG_P4OwnerName(vFile[ui])) > 0 ) 
		{
			if ( strcmp(BIG_P4OwnerName(vFile[ui]), DAT_CPerforce::GetInstance()->GetUser()) != 0 ) 
			{
				BOOL bInserted = FALSE;
				for ( UINT uj = 0; uj < vFileCheckoutByAnotherUser.size() ; uj++ ) 
				{
					if ( vFileCheckoutByAnotherUser[uj] == BIG_P4OwnerName(vFile[ui]) ) 
					{
						bInserted = TRUE;
					}
				}
				if ( !bInserted ) 
				{
					vFileCheckoutByAnotherUser.push_back(BIG_P4OwnerName(vFile[ui]));
				}
			}
			else 
			{
				vFileCheckedOutByCurrentUser.push_back(vFile[ui]);
			}
		}
		else 
		{
			vFileToCheckout.push_back(vFile[ui]);
		}
	}

	
	
	if ( _bLock ) 
	{
		char aszMessage[2048] = {0};
		for ( UINT ui = 0 ; ui < vFileCheckoutByAnotherUser.size() ; ui ++ )
		{
			char UserMessage[MAX_PATH] = {0};
			sprintf(UserMessage,"User: %s has file in checkout\n", vFileCheckoutByAnotherUser[ui].c_str());
			EPER_OutputMessage(UserMessage);
		}

		if ( vFileToCheckout.size() > 0 ) 
		{
			DAT_CUtils::GetP4FilesFromVecIndex(vFileToCheckout,vP4File,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

			char UserMessage[MAX_PATH] = {0};
			sprintf(UserMessage,"Locking: %s \n", _pszPath);
			strcat(aszMessage,UserMessage);
			if(EDI_gpo_EnterWnd) 
				EDI_gpo_EnterWnd->DisplayMessage(aszMessage);

			EPER_OutputMessage(UserMessage);


			DAT_CPerforce::GetInstance()->P4Edit(vP4File);

		}
		else 
		{
			char UserMessage[MAX_PATH] = {0};
			sprintf(UserMessage,"No files to checkout: %s \n", _pszPath);
			strcat(aszMessage,UserMessage);
			EPER_OutputMessage(UserMessage);

			if(EDI_gpo_EnterWnd) 
				EDI_gpo_EnterWnd->DisplayMessage(aszMessage);
		}
	}
	else 
	{

		if(EDI_gpo_EnterWnd) 
			EDI_gpo_EnterWnd->DisplayMessage("Trying to unlock files\n");

		EPER_OutputMessage("Trying to unlock files\n");
		DAT_CUtils::GetP4FilesFromVecIndex(vFileCheckedOutByCurrentUser,vP4File,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
		DAT_CPerforce::GetInstance()->P4Revert(vP4File);
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnDefault(void)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnDefault(void)
{
	CComboBox	*pComboBox;
	int i ;
	pComboBox = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4CHANGELIST_STATUS);
	for (  i = 0 ; i < pComboBox->GetCount(); i++ ) 
	{
		if ( pComboBox->GetItemDataPtr(i) )
		if (  std::string(P4_STATUS_PENDING ) == ((char*)pComboBox->GetItemDataPtr(i)) ) 
		{
			pComboBox->SetCurSel(i);
		}
	}

	pComboBox = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4USER);
	for (  i = 0 ; i < pComboBox->GetCount(); i++ ) 
	{
		if ( pComboBox->GetItemDataPtr(i) )
		if (  std::string(DAT_CPerforce::GetInstance()->GetUser()) == ((char*)pComboBox->GetItemDataPtr(i)) ) 
		{
			pComboBox->SetCurSel(i);
		}
	}


	pComboBox = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4VERSION);
	for (  i = 0 ; i < pComboBox->GetCount(); i++ ) 
	{
		if ( pComboBox->GetItemDataPtr(i) )
		if (  strstr(DAT_CPerforce::GetInstance()->GetClient(),((char*)pComboBox->GetItemDataPtr(i)) ) != NULL )
		{
			pComboBox->SetCurSel(i);
		}
	}

	pComboBox = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4CHANGELISTID);
	pComboBox->SetCurSel(-1);

	pComboBox = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4DESCRIPTION);
	pComboBox->SetCurSel(-1);

	pComboBox = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4KEY);
	pComboBox->SetCurSel(-1);

	pComboBox = (CComboBox *) mpo_DialogBar->GetDlgItem(IDC_P4FILENAME);
	pComboBox->SetCurSel(-1);


}
//------------------------------------------------------------
//   void EPER_cl_Frame::Refresh(void)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::Refresh(ULONG _ul_Changelist)
{

	// trying to open a new connection to perforce server
	// this is required because refresh can be called within an executed perforce command
	// which is usually connected with the instance

	DAT_CPerforce Perforce;
	if (Perforce.P4Connect() ) 
	{
		// verifying if we just want to refresh one specific changelist
		ClearChangelist(_ul_Changelist);
		if ( _ul_Changelist == -1 )
		{
			ClearChangelistTree(_ul_Changelist);
		}

		// requesting filters
		std::string strStatusFilter;
		std::string strUserFilter;
		std::string strVersionFilter;
		std::string strChangelistIDFilter;
		std::string strDescriptionFilter;
		std::string strMaxChangelistFilter;

		GetFilter(IDC_P4CHANGELIST_STATUS,strStatusFilter);
		GetFilter(IDC_P4USER,strUserFilter);
		GetFilter(IDC_P4VERSION,strVersionFilter);
		GetFilter(IDC_P4CHANGELISTID,strChangelistIDFilter,TRUE);
		GetFilter(IDC_P4DESCRIPTION,strDescriptionFilter,TRUE);
		GetFilter(IDC_P4MAXCHANGELIST,strMaxChangelistFilter,TRUE);

// new progress bar for server load, if you want to use it, 
// you need to know the maximum process of your server.
#ifndef _MONTREAL_SPECIFIC 
		int iProcessCount = Perforce.P4MonitorShow();
		CProgressCtrl* pServerLoad = (CProgressCtrl *) mpo_DialogBar->GetDlgItem(IDC_P4SERVER_LOAD);
		pServerLoad->SetRange(0,16);
		pServerLoad->SetPos( (iProcessCount < 16) ? iProcessCount : 16 );
#endif


		if ( strChangelistIDFilter != "" ) 
		{
			// retrieving the changelist for the given filters
			std::map< ULONG, PerforceChangelistInfo* > mapTemp;
			Perforce.P4Changes( strUserFilter.c_str(), strStatusFilter.c_str(),strMaxChangelistFilter.c_str(), &mapTemp);

			DAT_TChangelistInfo::iterator i = mapTemp.begin( );
			DAT_TChangelistInfo::iterator e = mapTemp.end( );
			for( ; i != e; ++i )
			{
				PerforceChangelistInfo* pChangelistInfo = (*i).second;
				if( strstr( pChangelistInfo->strChangelist.c_str( ), strChangelistIDFilter.c_str( ) ) != NULL )
				{
					ULONG ulChangelist = (*i).first;
					m_mChangelist[ ulChangelist ] = pChangelistInfo;
				}
			}
		}
		else
		{
			Perforce.P4Changes(strUserFilter.c_str(),strStatusFilter.c_str(),strMaxChangelistFilter.c_str(),&m_mChangelist);
		}

		if ( _ul_Changelist != -1 ) 
		{
			DAT_TChangelistInfo::iterator iter = m_mChangelist.find(_ul_Changelist);

			PerforceChangelistInfo* pInfo = (iter->second);
			if ( pInfo && Perforce.P4Describe(iter->first,pInfo) )
			{
				FillChangelistTree(pInfo,TRUE);
			}
		}
		else 
		{
			
			
			// for each changelist stored
			DAT_TChangelistInfo::const_iterator iter =  m_mChangelist.end() ;
			for ( UINT ui = 0; ui < m_mChangelist.size()  ;ui++ )
			{
				iter --;
				PerforceChangelistInfo* pInfo = (iter->second);

				if ( strVersionFilter != "" ) 
				{
					// checking the version filter
					if (pInfo->strClient.find(strVersionFilter) == -1 ) 
					continue;
				}

				if ( !Perforce.P4Describe(iter->first,pInfo) )
					continue;

				if ( DAT_CPerforce::GetInstance()->GetWorkingChangelist(FALSE) == -1 && 
					pInfo->strUser == DAT_CPerforce::GetInstance()->GetUser() && 
					strstr(DAT_CPerforce::GetInstance()->GetClient(),pInfo->strClient.c_str()) 
					) 
				{
					if ( pInfo->vFileInfo.size() == 0 ) 
					{
						DAT_CPerforce::GetInstance()->SetWorkingChangelist(atol(pInfo->strChangelist.c_str()));
					}		
				}

			
				if ( strDescriptionFilter != "" )
				{
					// checking the description filter
					std::string strDescription = pInfo->strDescription;
					std::string strDescriptionFilterUpper = strDescriptionFilter;
					
					for( UINT ui = 0 ; ui < strDescription.size(); ui++ )
						strDescription[ui] = toupper(strDescription[ui]);

					for( UINT ui = 0 ; ui < strDescriptionFilterUpper.size(); ui++ )
						strDescriptionFilterUpper[ui] = toupper(strDescriptionFilterUpper[ui]);

					if ( strDescription.find(strDescriptionFilterUpper) == -1 ) 
					continue;
				}

				FillChangelistTree(pInfo,FALSE);
				
			}
		}
		Perforce.P4Disconnect();
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::FillChangelistTree(ULONG _ulChangelist,BOOL _bFull)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::FillChangelistTree(ULONG _ulChangelist,BOOL _bFull)
{
	DAT_TChangelistInfo::const_iterator iter = m_mChangelist.find(_ulChangelist);
	PerforceChangelistInfo* _pChangelistInfo = iter->second;
	FillChangelistTree(_pChangelistInfo,_bFull);
}

//------------------------------------------------------------
//   void EPER_cl_Frame::FillChangelistTree(const PerforceChangelistInfo* _pChangelistInfo,BOOL _bFull)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::FillChangelistTree(const PerforceChangelistInfo* _pChangelistInfo,BOOL _bFull)
{
	// updating list control
	mpo_TreeView->SetRedraw(FALSE);

	ULONG ulChangelist = atol(_pChangelistInfo->strChangelist.c_str());

	std::string strDescription = _pChangelistInfo->strDescription;
	static const regex::rpattern descriptionFormatPattern("\r|\n|\t", " ", regex::GLOBAL);

	regex::subst_results substResults;
	descriptionFormatPattern.substitute(strDescription, substResults);

	// building up changelist description
	std::string Item =	_pChangelistInfo->strChangelist + " - " + 
						_pChangelistInfo->strDate + " " + 
						_pChangelistInfo->strUser + " " ;
	
	Item += + "  '" + strDescription + "'";		

	// adding number of files to changelist description
	if ( _pChangelistInfo->vFileInfo.size() > 0 )
	{
		char szSize[MAX_PATH];
		sprintf(szSize,"[ %d File(s) ]", _pChangelistInfo->vFileInfo.size());
		Item += szSize;
	}
	else 
	{
		Item += "[empty]";
	}

	

	int iIcon = GetIconChangelist(ulChangelist,_pChangelistInfo->strStatus,_pChangelistInfo->strUser,_pChangelistInfo->strClient);
	
	// triying to locate the item in the changelist
	HTREEITEM	hItem = mpo_TreeView->GetRootItem();
	while ( hItem != NULL  ) 
	{
		if ( mpo_TreeView->GetItemData(hItem) == ulChangelist) 
		{
			break;
		}
		hItem = mpo_TreeView->GetNextItem(hItem,TVGN_NEXT);
	}

	// the changelist is already in the tree
	if ( hItem ) 
	{
		mpo_TreeView->SetItemText(hItem,Item.c_str());
		mpo_TreeView->SetItemImage(hItem,iIcon,iIcon);
	}
	else
	{
		hItem = mpo_TreeView->InsertItem(Item.c_str(), iIcon, iIcon, TVI_ROOT);
		mpo_TreeView->SetItemData(hItem,ulChangelist);
	}

	ClearChangelistFiles(hItem);
	if ( ! FillChangelistTreeFiles(_pChangelistInfo,hItem,_bFull) )
	{
		mpo_TreeView->DeleteItem(hItem);
	}

	// updating list control
	mpo_TreeView->SetRedraw(TRUE);
}

//------------------------------------------------------------
//   EPER_EIcon EPER_cl_Frame::GetIconChangelist(const std::string& _strStatus, const std::string& _strUser)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
EPER_EIcon EPER_cl_Frame::GetIconChangelist(ULONG _ulChangelist,
											const std::string& _strStatus, 
											const std::string& _strUser,
											const std::string& _strVersion
											)
{
	if ( _strStatus == P4_STATUS_SUBMITTED ) 
		return eICON_SUBMITTED_CHANGELIST;

	if (	_strUser == DAT_CPerforce::GetInstance()->GetUser() && 
			strstr(DAT_CPerforce::GetInstance()->GetClient(),_strVersion.c_str()) ) 
	{
		if ( DAT_CPerforce::GetInstance()->GetWorkingChangelist(FALSE) == _ulChangelist ) 
			return eICON_WORKING_CHANGELIST;

		return eICON_USER_CHANGELIST;
	}
	
	return eICON_OTHERUSER_CHANGELIST;
}

//------------------------------------------------------------
//   EPER_EIcon EPER_cl_Frame::GetIconFile(const std::string& _strStatus, const std::string& _strUser,const std::string& _strCommand)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
EPER_EIcon EPER_cl_Frame::GetIconFile(const std::string& _strStatus, 
									  const std::string& _strUser,
									  const std::string& _strCommand,
									  const std::string& _strVersion)
{
	BOOL bOwner = FALSE;
	if ( _strUser == DAT_CPerforce::GetInstance()->GetUser() && strstr(DAT_CPerforce::GetInstance()->GetClient(),_strVersion.c_str()) ) 
	bOwner = TRUE;

	if ( _strStatus != P4_STATUS_PENDING  ) 
	return eICON_FILE2;

	if ( _strCommand == P4_ADD && bOwner )
	return eICON_ADD;
	
	if ( _strCommand == P4_ADD && !bOwner )
	return eICON_ADD_OTHERUSER;
	
	if ( _strCommand == P4_DELETE && bOwner )
	return eICON_DELETE;

	if ( _strCommand == P4_DELETE && !bOwner )
	return eICON_DELETE_OTHERUSER;

	if ( bOwner && (_strCommand == P4_EDIT || _strCommand == P4_INTEGRATE) ) 
	return eICON_EDIT;

	if ( !bOwner && (_strCommand == P4_EDIT || _strCommand == P4_INTEGRATE) ) 
	return eICON_EDIT_OTHERUSER;

	if ( _strCommand == P4_STATUS_UNRESOLVED && bOwner) 
		return eICON_UNRESOLVED;

	return eICON_FILE;

}

//------------------------------------------------------------
//   void EPER_cl_Frame::FillChangelistTreeFiles(const PerforceChangelistInfo* _pChangelistInfo, 
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPER_cl_Frame::FillChangelistTreeFiles(const PerforceChangelistInfo* _pChangelistInfo, 
											HTREEITEM hChangelist,
											BOOL _bFull
											)
{

	const std::vector<PerforceFileInfo*>&	_vFile = _pChangelistInfo->vFileInfo;
	std::vector<PerforceFileInfo*>			vFilteredFile ;
	std::vector<DAT_CP4ClientInfoChanges>	vOutputFile; // this will be displayed
	BOOL bAdded = _vFile.size() == 0;

	// updating list control
	mpo_TreeView->SetRedraw(FALSE);
	
	std::string strKeyFilter;
	GetFilter(IDC_P4KEY,strKeyFilter,TRUE);
	
	// first pass filter by key 
	for (	UINT ui = 0; ui < _vFile.size(); ui ++ ) 
	{
		// we do not want to add all the files into the tree for performance 
		ULONG ulKey = DAT_CUtils::GetKeyFromString(_vFile[ui]->strFilename.c_str());
		
		if ( strKeyFilter != "" )
		{
			char aszKey[KEYLENGHT] = {0};
			sprintf(aszKey,"%08X",ulKey);
			if ( strstr(aszKey,strKeyFilter.c_str()) == NULL ) 
				continue;
		}

		vFilteredFile.push_back(_vFile[ui]);
		
		if ( !_bFull ) 
		{
			DAT_CP4ClientInfoChanges info;
			info.strCommand			= _vFile[ui]->strCommand;
			info.strFilename		= _vFile[ui]->strFilename;
			info.strFileRevision	= _vFile[ui]->strFileRevision;
			info.ulKey = DAT_CUtils::GetKeyFromString(_vFile[ui]->strFilename.c_str());
			vOutputFile.push_back(info);
			break; // if we just want to display the chnagelist we don't bother about all the files.
		}
	}

	if ( _bFull ) 
	{
		FillChangelistTreeExpand(vFilteredFile,vOutputFile);
	}

	for (	UINT ui = 0; ui < vOutputFile.size(); ui ++ ) 
	{
		int iIcon = GetIconFile(_pChangelistInfo->strStatus,_pChangelistInfo->strUser, vOutputFile[ui].strCommand,_pChangelistInfo->strClient) ;

		char aszBuffer [MAX_PATH] = {0};
		sprintf(aszBuffer,"%s#%s (%08X) - %s ",vOutputFile[ui].strFilename.c_str(),vOutputFile[ui].strFileRevision.c_str(),vOutputFile[ui].ulKey,vOutputFile[ui].strCommand.c_str());

		HTREEITEM hItem = mpo_TreeView->InsertItem(aszBuffer, iIcon, iIcon, hChangelist);
		mpo_TreeView->SetItemData(hItem,vOutputFile[ui].ulKey);
		bAdded = TRUE;
	}

	mpo_TreeView->SortChildren(hChangelist);

	// updating list control
	mpo_TreeView->SetRedraw(TRUE);

	return bAdded ;

}

BOOL EPER_cl_Frame::FillChangelistTreeExpand( const std::vector<PerforceFileInfo*>& _vFile, 
											  std::vector<DAT_CP4ClientInfoChanges>& out_vFile)
{

	// in the case that you open jade and you already have files in checkout, 
	// we need to fstat these files in order to be able to have de correct
	// checkout info.
	std::vector<PerforceFileInfo*> vP4InBF;
	std::vector<DAT_CP4ClientInfoChanges> vP4NotInBF;

	std::string strFilenameFilter;
	GetFilter(IDC_P4FILENAME,strFilenameFilter,TRUE);

	for ( UINT ui = 0; ui < _vFile.size(); ui++ ) 
	{
		DAT_CP4ClientInfoChanges info;
		info.strCommand			= _vFile[ui]->strCommand;
		info.strFilename		= _vFile[ui]->strFilename;
		info.strFileRevision	= _vFile[ui]->strFileRevision;

		info.ulKey = DAT_CUtils::GetKeyFromString(_vFile[ui]->strFilename.c_str());
        BIG_INDEX ulFat = BIG_ul_SearchKeyToFat(info.ulKey);

		// key was not found in the big file, requesting server for it.
		if (  ulFat == BIG_C_InvalidIndex ) 
		{
			if ( info.strCommand == P4_DELETE ) 
			{
				// this is not very beautiful, but when changelist are submitted,
				// we cannot retreive file buffer to get the filename, 
				// so we need to get the previous revision. 
				// On the contrary, if the file has not been checked in yet, 
				// the revision on the server is good to retreive the filename.

				std::string strStatusFilter;
				GetFilter(IDC_P4CHANGELIST_STATUS,strStatusFilter,FALSE);
				int iRevision = atoi(info.strFileRevision.c_str());

				if ( strStatusFilter != P4_STATUS_PENDING ) 
				{
					iRevision -= 1;
				}

				char aszRevision[10] = {0};
				sprintf(aszRevision,"#%d", iRevision );

				info.strFilename += aszRevision;
			}

			vP4NotInBF.push_back(info);
		}
		else
		{
			if	(	strFilenameFilter != "" ) // we have a file filter 
			{
				std::string strBFFileName;
				static char aszBFFilename [BIG_C_MaxLenPath + BIG_C_MaxLenName];
				BIG_ComputeFullName(BIG_ParentFile(ulFat), aszBFFilename);
				strcat(aszBFFilename,"/");
				strcat(aszBFFilename,BIG_NameFile(ulFat));
				strBFFileName = aszBFFilename;

				// checking the description filter
				std::string strFilenameFilterUpper = strFilenameFilter;

				for( UINT ui = 0 ; ui < strBFFileName.size(); ui++ )
					strBFFileName[ui] = toupper(strBFFileName[ui]);

				for( UINT ui = 0 ; ui < strFilenameFilterUpper.size(); ui++ )
					strFilenameFilterUpper[ui] = toupper(strFilenameFilterUpper[ui]);

				if ( strBFFileName.find(strFilenameFilterUpper) == -1 ) 
				{ continue; }
			}
			vP4InBF.push_back( _vFile[ui] );
		}
	}

	DAT_CPerforce PerforceConnection;
	if ( PerforceConnection.P4Connect() ) 
	{
		if ( vP4InBF.size() > 0 ) 
		{
			std::vector<std::string> vFstatFile;
			for ( UINT ui = 0; ui < vP4InBF.size(); ui ++ ) 
			{
				vFstatFile.push_back(vP4InBF[ui]->strFilename);
			}
			PerforceConnection.P4Fstat(vFstatFile);

			for ( UINT ui = 0; ui < vP4InBF.size(); ui ++ ) 
			{
				DAT_CP4ClientInfoChanges info;
				info.ulKey			= DAT_CUtils::GetKeyFromString(vP4InBF[ui]->strFilename.c_str());
				BIG_INDEX ulFat		= BIG_ul_SearchKeyToFat(info.ulKey);
				info.strCommand		= BIG_P4Action(ulFat);
				info.strFilename	= BIG_NameFile(ulFat);
				char aszRevision[16] = {0};
				sprintf(aszRevision,"%d",BIG_P4RevisionClient(ulFat));
				info.strFileRevision= aszRevision;
				out_vFile.push_back(info);
			}
		
		}

		// these files have been deleted from bigfile.
		// we need to request info from the server in order to receive their actual filename
		if ( vP4NotInBF.size() > 0 ) 
		{
			std::vector<DAT_CP4ClientInfoHeader*> vFileHeader;

			std::vector<std::string> vFileInfo;
			for ( UINT ui = 0; ui < vP4NotInBF.size(); ui ++ ) 
			{
				vFileInfo.push_back(vP4NotInBF[ui].strFilename);
			}

			// this is a little bit overkill but it usually does not happen 
			// on a lot of files. 
			PerforceConnection.P4FileInfo(vFileInfo, &vFileHeader);

			for ( UINT ui = 0; ui < vP4NotInBF.size() ;ui++ )
			{
				DAT_CP4ClientInfoChanges info;

				info.ulKey			= DAT_CUtils::GetKeyFromString(vP4NotInBF[ui].strFilename.c_str());
				info.strCommand		= vP4NotInBF[ui].strCommand;
				info.strFilename	= vP4NotInBF[ui].strFilename;
				info.strFileRevision= vP4NotInBF[ui].strFileRevision;

				for ( UINT uiHeader = 0; uiHeader < vFileHeader.size(); uiHeader ++ )
				{
					if ( vFileHeader[uiHeader]->ulKey == info.ulKey )
					{
						info.strFilename	= vFileHeader[uiHeader]->aszBFFilename;
					}
				}	
				
				if	(	strFilenameFilter != "" ) // we have a file filter 
				{
					// checking the description filter
					std::string strFilenameFilterUpper = strFilenameFilter;
					std::string strBFFileName = info.strFilename;

					for( UINT ui = 0 ; ui < strBFFileName.size(); ui++ )
						strBFFileName[ui] = toupper(strBFFileName[ui]);

					for( UINT ui = 0 ; ui < strFilenameFilterUpper.size(); ui++ )
						strFilenameFilterUpper[ui] = toupper(strFilenameFilterUpper[ui]);

					if ( strBFFileName.find(strFilenameFilterUpper) == -1 ) 
					{ continue; }
				}

				out_vFile.push_back(info);
			}

			for ( UINT uiHeader = 0; uiHeader < vFileHeader.size(); uiHeader ++ )
			{
				delete vFileHeader[uiHeader];
			}
		}
		PerforceConnection.P4Disconnect();
	}
	return TRUE;
}


//------------------------------------------------------------
//   BOOL EPER_cl_Frame::GetFilter(UINT _ulFilter,std::string& _strFilter,BOOL _AddIfNotThere /* = FALSE */)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPER_cl_Frame::GetFilter(UINT _ulFilter,std::string& _strFilter,BOOL _AddIfNotThere /* = FALSE */)
{
	CComboBox	*po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(_ulFilter);
	if ( po_Combo->GetCurSel() >  CB_ERR )
	{
		char* pFilter = (char*)(po_Combo->GetItemDataPtr(po_Combo->GetCurSel()));
		if (  pFilter )
		{
			_strFilter = pFilter;
		}
		return TRUE;
	}
	else 
	{
		CEdit* pComboEdit=(CEdit*)po_Combo->GetWindow(GW_CHILD);
		if ( pComboEdit ) 
		{
			CString strFilter;
			pComboEdit->GetWindowText(strFilter);
			_strFilter = strFilter;

			if ( _AddIfNotThere && _strFilter != "" ) 
			{
				int i;
				for (  i = 0 ; i < po_Combo->GetCount(); i++ ) 
				{
					if ( _strFilter == ((char*)po_Combo->GetItemDataPtr(i)) ) 
					{
						break;
					}
				}

				if ( po_Combo->GetCount() == 0 || i == po_Combo->GetCount() ) 
				{
					int index = po_Combo->AddString(_strFilter.c_str());
					char* pNewFilter = new char[_strFilter.length()+1];
					strcpy(pNewFilter,_strFilter.c_str());
					po_Combo->SetItemDataPtr(index,pNewFilter);
					po_Combo->UpdateWindow();

				}
			}

			return TRUE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------
//   void EPER_cl_Frame::ClearChangelist(ULONG _ulChangelist)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::ClearChangelist(ULONG _ulChangelist)
{
	if ( _ulChangelist == -1 ) 
	{
	
		DAT_TChangelistInfo::iterator iter ;
		for ( iter = m_mChangelist.begin() ;iter != m_mChangelist.end();iter++ ) 
		{
			for ( UINT ui = 0; ui < (iter->second)->vFileInfo.size() ; ui ++ )
			{
				delete ((iter->second)->vFileInfo[ui]);
				(iter->second)->vFileInfo[ui] = NULL;
			}
			(iter->second)->vFileInfo.clear();
			delete (iter->second);
		}
		m_mChangelist.clear();
	}
	else
	{
		DAT_TChangelistInfo::iterator iter = m_mChangelist.find(_ulChangelist);
		if ( iter != m_mChangelist.end() ) 
		{
			for ( UINT ui = 0; ui < (iter->second)->vFileInfo.size() ; ui ++ )
			{
				delete ((iter->second)->vFileInfo[ui]);
			}
			(iter->second)->vFileInfo.clear();
			delete (iter->second);
			m_mChangelist.erase(iter);
		}


	}
}


//------------------------------------------------------------
//   void EPER_cl_Frame::ClearChangelistFiles(HTREEITEM hChangelist)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::ClearChangelistFiles(HTREEITEM hChangelist)
{
	if ( hChangelist ) 
	{	
		HTREEITEM   hItem;
		hItem = mpo_TreeView->GetNextItem(hChangelist,TVGN_CHILD);
		while ( hItem != NULL ) 
		{
			HTREEITEM   hDeleteItem = hItem;
			hItem = mpo_TreeView->GetNextItem(hItem,TVGN_NEXT);
			mpo_TreeView->DeleteItem(hDeleteItem);
		}
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::ClearChangelistTree(ULONG _ulChangelist)
/// \author    YCharbonneau
/// \date      2005-02-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::ClearChangelistTree(ULONG _ulChangelist)
{
	if ( _ulChangelist != -1  ) 
	{	
		HTREEITEM   hItem;
		hItem = mpo_TreeView->GetRootItem();
		while ( hItem != NULL ) 
		{
			if ( mpo_TreeView->GetItemData(hItem) == _ulChangelist ) 
			{
				mpo_TreeView->DeleteItem(hItem);
			}
			hItem = mpo_TreeView->GetNextItem(hItem,TVGN_NEXT);
		}
	}
	else
	{
		mpo_TreeView->DeleteAllItems();
	}

}

//------------------------------------------------------------
//   void EPER_cl_Frame::MoveItems(std::vector<BIG_INDEX>& _vFileIndex,ULONG _ulChangelist )
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::MoveItems(std::vector<BIG_KEY>& _vFileIndex,ULONG _ulChangelist )
{
	if ( DAT_CPerforce::GetInstance()->P4Connect()  )
	{
		std::vector<std::string> vFiles;
		DAT_CUtils::GetP4FilesFromVecKey(_vFileIndex,vFiles,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
		DAT_CPerforce::GetInstance()->P4Reopen(vFiles,_ulChangelist);
		DAT_CPerforce::GetInstance()->P4Disconnect();
	}
}


//------------------------------------------------------------
//   HTREEITEM EPER_cl_Frame::GetTreeItemFromlParam(LONG lParam)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
HTREEITEM EPER_cl_Frame::GetTreeItemFromlParam(LONG lParam)
{
    UINT        uFlags;
    CPoint      pt;
    HTREEITEM   hItem;

    pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
    hItem = mpo_TreeView->HitTest(pt, &uFlags);
    if((TVHT_ONITEM & uFlags) && (hItem != NULL)) return hItem;
    return NULL;
}



//------------------------------------------------------------
//   void EPER_cl_Frame::OnCtrlPopup(CPoint pt)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnCtrlPopup(CPoint pt)
{
	EMEN_cl_SubMenu o_Menu(FALSE);

	InitPopupMenuAction(&o_Menu);
	HTREEITEM hLastItem =  mpo_TreeView->GetLastSelectedItem();
	std::string strVersionFilter;
	GetFilter(IDC_P4VERSION,strVersionFilter);


	if ( !hLastItem ) 
	{
		AddPopupMenuAction(&o_Menu, EPERC_ACTION_ADDCHANGELIST);
	}
	
	// selecting a changelist 
	if (  hLastItem && mpo_TreeView->GetParentItem(hLastItem) == NULL  )
	{

		ULONG ulChangelist = mpo_TreeView->GetItemData(hLastItem);

		if (	m_mChangelist[ulChangelist]->strStatus	== P4_STATUS_PENDING && 
				m_mChangelist[ulChangelist]->strUser	== DAT_CPerforce::GetInstance()->GetUser() && 
				m_mChangelist[ulChangelist]->strClient	== DAT_CPerforce::GetInstance()->GetClient()) 
		{
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_SUBMIT);
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_SUBMIT_REOPEN);
			AddPopupMenuAction(&o_Menu, 0);
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_SETWORKINGCHANGELIST);
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_ADDCHANGELIST);
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_EDITCHANGELIST);
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_DELETEEMPTY);
			AddPopupMenuAction(&o_Menu, 0);
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_REVERT_UNCHANGED);
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_REVERT);
		}

		if (	m_mChangelist[ulChangelist]->strStatus	== P4_STATUS_SUBMITTED ) 
		{
			if ( DAT_CPerforce::GetInstance()->GetBFVersion() == strVersionFilter ) 
			{
				AddPopupMenuAction(&o_Menu, EPERC_ACTION_SYNC_FILES_IN_SELECTEDCHANGELIST);
				AddPopupMenuAction(&o_Menu, EPERC_ACTION_SYNC_FILES_IN_SELECTEDCHANGELIST_TO_PREVIOUSREVISION);
				AddPopupMenuAction(&o_Menu, EPERC_ACTION_SYNC_BF_TO_SELECTEDCHANGELIST);
				AddPopupMenuAction(&o_Menu, EPERC_ACTION_OPEN_FILES_IN_SELECTEDCHANGELIST);
			}
			else 
			{
				AddPopupMenuAction(&o_Menu, EPERC_ACTION_INTEG);
			}
		}
	}
	else if ( hLastItem ) 
	{
		ULONG ulChangelist = mpo_TreeView->GetItemData(mpo_TreeView->GetParentItem(hLastItem));
		


		// if we are in the correct BF configuration
		if (	m_mChangelist[ulChangelist]->strUser	== DAT_CPerforce::GetInstance()->GetUser() && 
				m_mChangelist[ulChangelist]->strClient	== DAT_CPerforce::GetInstance()->GetClient() && 
				m_mChangelist[ulChangelist]->strStatus	== P4_STATUS_PENDING ) 
		{
				int iIcon = 0;
				// the file can be resolved
				if ( mpo_TreeView->GetItemImage(hLastItem,iIcon,iIcon) && iIcon == eICON_UNRESOLVED )
				{
					AddPopupMenuAction(&o_Menu, EPERC_ACTION_RESOLVE);
					AddPopupMenuAction(&o_Menu, 0);
				}

				AddPopupMenuAction(&o_Menu, EPERC_ACTION_REVERT_UNCHANGED);
				AddPopupMenuAction(&o_Menu, EPERC_ACTION_REVERT);
				AddPopupMenuAction(&o_Menu, 0);

		}
		 
		// same version 
		if ( DAT_CPerforce::GetInstance()->GetBFVersion() == strVersionFilter ) 
		{
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_SHOWHISTORY);
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_SHOWDIFF);

			if ( m_mChangelist[ulChangelist]->strStatus	== P4_STATUS_SUBMITTED ) 
			{
				AddPopupMenuAction(&o_Menu, EPERC_ACTION_SYNC);
			}
		}
		// not the same version into submitted
		else if (	m_mChangelist[ulChangelist]->strStatus	== P4_STATUS_SUBMITTED ) 
		{
			AddPopupMenuAction(&o_Menu, EPERC_ACTION_INTEG);
		}
	}

	AddPopupMenuAction(&o_Menu, 0);
	AddPopupMenuAction(&o_Menu, EPERC_ACTION_REFRESH);

	TrackPopupMenuAction(pt, &o_Menu);
}



//------------------------------------------------------------
//   BOOL EPER_cl_Frame::b_KnowsKey(USHORT _uw_Key)
/// \author    YCharbonneau
/// \date      23-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
BOOL EPER_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	// having the refresh key handled by the Jade 
	// the reste will be shipped directly to controls
	switch(_uw_Key)
	{
		case VK_F4:
			return FALSE;
 	}
	return TRUE;
}


//------------------------------------------------------------
//   void EPER_cl_Frame::OnClose(void)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnClose(void)
{
}


//------------------------------------------------------------
//   BOOL EPER_cl_Frame::b_CanActivate(void)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPER_cl_Frame::b_CanActivate(void)
{
	return TRUE;
}

//------------------------------------------------------------
//   void EPER_cl_Frame::CloseProject(void)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::CloseProject(void)
{
	ComboBoxResetContentAndDeleteDataPtr( IDC_P4USER );
}

//------------------------------------------------------------
//   void EPER_cl_Frame::ComboBoxResetContentAndDeleteDataPtr
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::ComboBoxResetContentAndDeleteDataPtr( int ctrlID )
{
	CComboBox* pComboBox;
	pComboBox = (CComboBox *) (mpo_DialogBar->GetDlgItem( ctrlID ));

	for(int i =0 ; i < pComboBox->GetCount(); i++)
	{
		delete pComboBox->GetItemDataPtr(i);
	}
	pComboBox->ResetContent();
}
#ifdef JADEFUSION
//------------------------------------------------------------
//   void EPER_cl_Frame::OnClickFilterSettings(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnClickFilterSettings()
{
	EDIA_cl_P4FileFilterDialog Dlg(&m_FileFilter);

	Dlg.DoModal();
}

//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnCheckFilterSettings(void)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnCheckFilterSettings()
{
	BOOL bIsChecked = mpo_DialogBar->IsDlgButtonChecked(IDC_P4CHECK_FILTERSETTINGS);

	m_FileFilter.Enable(bIsChecked);
}
#endif
#endif /* ACTIVE_EDITORS */

