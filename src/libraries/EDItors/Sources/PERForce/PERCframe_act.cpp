//------------------------------------------------------------------------------
// Filename   :PERCframe_act.cpp
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: Implementation of PERCframe_act
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include <assert.h>

#ifdef ACTIVE_EDITORS
#include "Res/Res.h"
#include "EDImainframe.h"
#include "PERcframe.h"
#include "PERtreectrl.h"
#include "LINKs/LINKtoed.h"
#include "BIGfiles/BIGfat.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDItors/Sources/BROwser/BROstrings.h"
#include "EDItors/Sources/BROwser/BROerrid.h"
#include "EDImsg.h"
#include "EDIeditors_infos.h"

#include "DATaControl/DATCPerforce.h"
#include "DATaControl/DATCUtils.h"
#include "PERCframe_act.h"

#include "DIAlogs\DIAname_dlg.h"
#include "DIAlogs\DATCP4ChangelistComment.h"
#include "DIAlogs/DATCP4SettingsDlg.h"
#include "DIAlogs\ListCtrlStyled.h"

#include "DIAlogs\DIAmsglink_dlg.h"

#include "BIGFiles\BIGmdfy_dir.h"
#include "BIGFiles\BIGmdfy_file.h"


void DeleteLocalFiles(BIG_INDEX ul_DirIndex);

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------
extern BOOL EDI_gb_NoUpdateVSS;
extern BOOL EDI_gb_CheckInDel;
extern BOOL EDI_can_lock;
extern bool g_bSynchedInHistory;

static std::vector< ULONG >* g_pListFiles = NULL;
static bool g_bAutoAccept = false;

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

namespace
{
	ULONG FindBaseChangelist( const std::vector< ULONG >& lstIndexes )
	{
		for( size_t n = 0; n < lstIndexes.size( ); ++n )
		{
			const ULONG ulChangelist = BIG_P4ChangeList( lstIndexes[ n ] );
			if( ulChangelist != -1 )
				return ulChangelist;
		}

		return -1;
	}

	bool NeedNewChangelist( const std::vector< ULONG >& lstIndexes, DAT_TChangelistInfo& lstChangelists )
	{
		if( lstIndexes.empty( ) )
			return false;

		// are all files to check in in the same changelist?
		const ULONG ulBaseChangelist = FindBaseChangelist( lstIndexes );
		if( ulBaseChangelist == BIG_C_InvalidIndex || ulBaseChangelist == 0 )
			return false;

		std::vector< ULONG >::const_iterator i = lstIndexes.begin( );
		std::vector< ULONG >::const_iterator e = lstIndexes.end( );

		for( ; i != e; ++i )
		{
			const ULONG ulChangelist = BIG_P4ChangeList( *i );

			if( ulChangelist == -1 )
				continue;

			if( ulChangelist != ulBaseChangelist )
				return true;
		}

		// all files are in the same changelist, so verify if changelist contains other files too
		DAT_TChangelistInfo::const_iterator f = lstChangelists.find( ulBaseChangelist );
		const PerforceChangelistInfo* pChangelist = (*f).second;

		if( pChangelist->vFileInfo.size( ) != lstIndexes.size( ) )
			return true;

		return false;
	}

	void ProcessFile( BIG_INDEX ulFile, std::vector< ULONG >& lstIndexes )
	{
		// bug fix: info appears to get out of sync at times, so force update
		//DAT_CPerforce::GetInstance()->P4FStat( ulFile );

		if( BIG_P4Action( ulFile )[0] != P4_INVALIDSTRING )
			lstIndexes.push_back( ulFile );
	}

	void ProcessFolder( BIG_INDEX ulDir, std::vector< ULONG >& lstIndexes )
	{
		// process files in this folder
		BIG_INDEX ulFile = BIG_FirstFile( ulDir );
		while( ulFile != BIG_C_InvalidIndex )
		{
			ProcessFile( ulFile, lstIndexes );
			ulFile = BIG_NextFile( ulFile );
		}

		// recursively process sub-folders
		BIG_INDEX ulSubDir = BIG_SubDir(ulDir);
		while(ulSubDir != BIG_C_InvalidIndex)
		{
			ProcessFolder( ulSubDir, lstIndexes );
			ulSubDir = BIG_NextDir(ulSubDir);
		}
	}

	void UpdateFilePerforceInfoImpl( PER_CDataCtrl* _pItemCtrl, HDATACTRLITEM hItem, std::vector< ULONG >& lstKeys )
	{
		while( hItem != -1 )
		{
			BIG_INDEX ulIndex = _pItemCtrl->GetItemReference(hItem);
			hItem =_pItemCtrl->GetNextSelectedItem(hItem);

			if( ulIndex == BIG_C_InvalidIndex )
				continue;

			const ULONG ulKey = BIG_FileKey( ulIndex );
			lstKeys.push_back( ulKey );
		} 
	}

	void UpdateDirPerforceInfoImpl( BIG_INDEX ulDir, std::vector< ULONG >& lstKeys )
	{
		// process files in this folder
		BIG_INDEX ulFile = BIG_FirstFile( ulDir );
		while( ulFile != BIG_C_InvalidIndex )
		{
			const ULONG ulKey = BIG_FileKey( ulFile );
			lstKeys.push_back( ulKey );

			ulFile = BIG_NextFile( ulFile );
		}

		// recursively process sub-folders
		BIG_INDEX ulSubDir = BIG_SubDir(ulDir);
		while(ulSubDir != BIG_C_InvalidIndex)
		{
			UpdateDirPerforceInfoImpl( ulSubDir, lstKeys );
			ulSubDir = BIG_NextDir(ulSubDir);
		}
	}

	void UpdateDirPerforceInfo( BIG_INDEX ulDir )
	{
		std::vector< ULONG > lstKeys;
		UpdateDirPerforceInfoImpl( ulDir, lstKeys );
		
		std::vector<std::string> vFiles;
		DAT_CUtils::GetP4FilesFromVecKey(lstKeys,vFiles,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

		DAT_CPerforce::GetInstance()->P4Fstat( vFiles );
	}

	void UpdateFilePerforceInfo( PER_CDataCtrl* _pItemCtrl, HDATACTRLITEM hItem )
	{
		std::vector< ULONG > lstKeys;
		UpdateFilePerforceInfoImpl( _pItemCtrl, hItem, lstKeys );
		
		std::vector<std::string> vFiles;
		DAT_CUtils::GetP4FilesFromVecKey(lstKeys,vFiles,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

		DAT_CPerforce::GetInstance()->P4Fstat( vFiles );
	}
}

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//extern EDIA_cl_MsgLinkDialog	EDI_go_MsgTruncateFiles;
//------------------------------------------------------------
//   void EPER_cl_Frame::OnAction(ULONG _ul_Action)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnAction(ULONG _ul_Action)
{

	//EDI_go_MsgTruncateFiles.mb_ApplyToAll = FALSE;

	switch ( _ul_Action )
	{
		case EPERC_ACTION_ADDCHANGELIST:
			M_MF()->BeginWaitCursor();
			OnAdd();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_SETWORKINGCHANGELIST:
			M_MF()->BeginWaitCursor();
			OnSetWorking();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_REVERT:
			M_MF()->BeginWaitCursor();
			OnRevert();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_REVERT_UNCHANGED:
			M_MF()->BeginWaitCursor();
			OnRevertUnchanged();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_SUBMIT:
			M_MF()->BeginWaitCursor();
			OnSubmit();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_SUBMIT_REOPEN:
			M_MF()->BeginWaitCursor();
			OnSubmitEdit();
			M_MF()->EndWaitCursor();
			break;

		case EPERC_ACTION_SHOWHISTORY:
			M_MF()->BeginWaitCursor();
			OnHistory();
			M_MF()->EndWaitCursor();
			break;

		case EPERC_ACTION_EDITCHANGELIST:
			M_MF()->BeginWaitCursor();
			OnEdit();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_INTEG:
			M_MF()->BeginWaitCursor();
			IntegrateSelected(mpo_TreeView);
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_SYNC:
			M_MF()->BeginWaitCursor();
				SyncSelected(mpo_TreeView,TRUE);
			M_MF()->EndWaitCursor();
			break;

		case EPERC_ACTION_REFRESH:
			M_MF()->BeginWaitCursor();
			Refresh();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_DELETEEMPTY:
			M_MF()->BeginWaitCursor();
			OnDeleteEmpty();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_RESOLVE:
			M_MF()->BeginWaitCursor();
			OnResolve();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_SHOWDIFF:
			M_MF()->BeginWaitCursor();
			DiffSelected(dynamic_cast<PER_CDataCtrl*>(mpo_TreeView));
			M_MF()->EndWaitCursor();
			break;

		case EPERC_ACTION_SYNC_FILES_IN_SELECTEDCHANGELIST:
			M_MF()->BeginWaitCursor();
			if ( IsWorldCurrentlyOpened() )
				OnSyncChangelistFiles(FALSE);
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_SYNC_FILES_IN_SELECTEDCHANGELIST_TO_PREVIOUSREVISION:
			M_MF()->BeginWaitCursor();
			if ( IsWorldCurrentlyOpened() )
				OnSyncChangelistFiles(TRUE);	
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_SYNC_BF_TO_SELECTEDCHANGELIST:
			M_MF()->BeginWaitCursor();
			OnSyncBfToChangelist();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_OPEN_FILES_IN_SELECTEDCHANGELIST:
			M_MF()->BeginWaitCursor();
			if ( IsWorldCurrentlyOpened() )
				OnOpenChangelistFiles();
			M_MF()->EndWaitCursor();
		break;

		case EPERC_ACTION_CLEARLOG:
			M_MF()->BeginWaitCursor();
			OnClearLogWindow();
			M_MF()->EndWaitCursor();
		break;

		case EPER_ACTION_REFRESH_CLIENTVIEW:
		{
			// We don't want user to be able to abort the P4 commands when Jade starts
			if ( M_MF()->MessageBox( "Refresh server info ? This cannot be aborted, Continue ?", "[P4 - Confirm]", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
				DAT_CPerforce::GetInstance()->P4Connect(FALSE) ) 
			{
				DAT_CPerforce::GetInstance()->P4FlushBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str());
				DAT_CPerforce::GetInstance()->P4Disconnect();
			}
		}
		break;
	}	
}

//------------------------------------------------------------
//   void EPER_cl_Frame::RemoveSelectedItem(HTREEITEM _hSelectedItem)
/// \author    YCharbonneau
/// \date      2005-01-14
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::RemoveSelectedItem(HTREEITEM _hSelectedItem)
{
	HTREEITEM hNewSelectedItem = mpo_TreeView->GetParentItem(_hSelectedItem);
	mpo_TreeView->DeleteItem(_hSelectedItem);
	mpo_TreeView->SelectItem(hNewSelectedItem,TVIS_SELECTED);
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnSyncChangelistFiles(BOOL _bPreviousRevision /* = FALSE */ )
/// \author    YCharbonneau
/// \date      2005-02-16
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnSyncChangelistFiles(BOOL _bPreviousRevision /* = FALSE */ )
{
	HTREEITEM hSelectedItem = mpo_TreeView->GetLastSelectedItem();
	if  (!hSelectedItem) return ;
	
			// its a changelist
	if	(	mpo_TreeView->GetParentItem(hSelectedItem) == NULL &&  mpo_TreeView->GetItemData(hSelectedItem) && 
			// its a submitted changelist
			m_mChangelist[mpo_TreeView->GetItemData(hSelectedItem)]->strStatus	== "submitted" /*&& 
			// its a user changelist
			-NOTE- Removed this condition so that any user can sync to any changelist
			m_mChangelist[mpo_TreeView->GetItemData(hSelectedItem)]->strUser	== DAT_CPerforce::GetInstance()->GetUser() &&
			// its the same version			
			-NOTE- Removed this condition so that any user can sync to any changelist
			m_mChangelist[mpo_TreeView->GetItemData(hSelectedItem)]->strClient	== DAT_CPerforce::GetInstance()->GetClient()*/
		)
	{

		ULONG ulChangelist = mpo_TreeView->GetItemData(hSelectedItem);
		if (	m_mChangelist[ulChangelist]->vFileInfo.size() > 0 && 
				DAT_CPerforce::GetInstance()->P4Connect() ) 
		{
			std::vector<std::string> vFiles;
			for ( UINT ui = 0 ; ui < m_mChangelist[ulChangelist]->vFileInfo.size(); ui ++ )
			{
				ULONG ulRevision = atol(m_mChangelist[ulChangelist]->vFileInfo[ui]->strFileRevision.c_str());
				if ( _bPreviousRevision ) ulRevision --;

				char aszFile[MAX_PATH] = {0};
				PerforceChangelistInfo* pChangelistInfo = m_mChangelist[ulChangelist];
				PerforceFileInfo* pFileInfo = pChangelistInfo->vFileInfo[ui];
				sprintf(aszFile,"%s#%d",pFileInfo->strFilename.c_str(),ulRevision);
				vFiles.push_back(aszFile);
			}
			DAT_CPerforce::GetInstance()->P4Sync(vFiles,"",TRUE);
			DAT_CPerforce::GetInstance()->P4Disconnect();
			M_MF()->FatHasChanged();
		}
	}

}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnSyncBfToChangelist()
/// \author    YCharbonneau
/// \date      2005-02-16
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnSyncBfToChangelist()
{
	HTREEITEM hSelectedItem = mpo_TreeView->GetLastSelectedItem();
	if  (!hSelectedItem) return ;

	// its a changelist
	if	(	mpo_TreeView->GetParentItem(hSelectedItem) == NULL &&  mpo_TreeView->GetItemData(hSelectedItem) && 
		// its a submitted changelist
		m_mChangelist[mpo_TreeView->GetItemData(hSelectedItem)]->strStatus	== "submitted" /*&& 
		// its a user changelist
		-NOTE- Removed this condition so that any user can sync to any changelist
		m_mChangelist[mpo_TreeView->GetItemData(hSelectedItem)]->strUser	== DAT_CPerforce::GetInstance()->GetUser() &&
		// its the same version
		-NOTE- Removed this condition so that any user can sync to any changelist
		m_mChangelist[mpo_TreeView->GetItemData(hSelectedItem)]->strClient	== DAT_CPerforce::GetInstance()->GetClient()*/
		)
	{
		ULONG ulChangelist = mpo_TreeView->GetItemData(hSelectedItem);
		if (	m_mChangelist[ulChangelist]->vFileInfo.size() > 0 && DAT_CPerforce::GetInstance()->P4Connect() ) 
		{
			DAT_CPerforce::GetInstance()->P4SyncBF(	DAT_CPerforce::GetInstance()->GetP4Root().c_str(),
													NULL,TRUE,NULL,ulChangelist);
			DAT_CPerforce::GetInstance()->P4Disconnect();
			M_MF()->FatHasChanged();
		}
	}

}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnOpenChangelistFiles()
/// \author    YCharbonneau
/// \date      2005-02-16
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnOpenChangelistFiles()
{

	HTREEITEM hSelectedItem = mpo_TreeView->GetLastSelectedItem();
	if  (!hSelectedItem) return ;

		// its a changelist
	if	(	mpo_TreeView->GetParentItem(hSelectedItem) == NULL &&  mpo_TreeView->GetItemData(hSelectedItem) && 
		// its a submitted changelist
		m_mChangelist[mpo_TreeView->GetItemData(hSelectedItem)]->strStatus	== "submitted" /*&& 
		// its a user changelist
		-NOTE- Removed this condition so that any user can sync to any changelist
		m_mChangelist[mpo_TreeView->GetItemData(hSelectedItem)]->strUser	== DAT_CPerforce::GetInstance()->GetUser() /*&&
		// its the same version
		-NOTE- Removed this condition so that any user can sync to any changelist
		m_mChangelist[mpo_TreeView->GetItemData(hSelectedItem)]->strClient	== DAT_CPerforce::GetInstance()->GetClient() */
		)
	{

		ULONG ulChangelist = mpo_TreeView->GetItemData(hSelectedItem);
		if (	m_mChangelist[ulChangelist]->vFileInfo.size() > 0 && 
			DAT_CPerforce::GetInstance()->P4Connect() ) 
		{
			std::vector<std::string> vFiles;
			for ( UINT ui = 0 ; ui < m_mChangelist[ulChangelist]->vFileInfo.size(); ui ++ )
			{
				vFiles.push_back(m_mChangelist[ulChangelist]->vFileInfo[ui]->strFilename);
			}
			DAT_CPerforce::GetInstance()->P4Edit(vFiles);
			DAT_CPerforce::GetInstance()->P4Disconnect();
			M_MF()->FatHasChanged();
			OnDefault();
			Refresh();
		}
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnEdit()
/// \author    YCharbonneau
/// \date      2005-01-14
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnEdit()
{
	HTREEITEM hSelectedItem = mpo_TreeView->GetLastSelectedItem();
	if  (!hSelectedItem) return ;

	if ( mpo_TreeView->GetParentItem(hSelectedItem) == NULL  )
	{
		if ( DAT_CPerforce::GetInstance()->P4Connect() ) 
		{
			ULONG ulChangelist = mpo_TreeView->GetItemData(hSelectedItem) ; 
			std::string strAdditionnalVersion;
			BOOL bRevertSelected;
			BOOL bVerifyLinks = FALSE;
			EditChangelist(ulChangelist,FALSE,strAdditionnalVersion,bVerifyLinks,bRevertSelected);
			DAT_CPerforce::GetInstance()->P4Disconnect();
			Refresh();
		}
	}
}



//------------------------------------------------------------
//   int EPER_cl_Frame::EditChangelist(ULONG _ulChangelist)
/// \author    YCharbonneau
/// \date      24/06/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPER_cl_Frame::EditChangelist(ULONG _ulChangelist,BOOL _bOnSubmit, 
								   std::string& _strAdditionnalVersion,BOOL& _bVerifyLinks, 
								   BOOL& _bRevertUnchanged)
{
	std::string strDescription;
	DAT_CPerforce::GetInstance()->P4Describe(_ulChangelist,strDescription);
	char aszType[MAX_PATH] = {0};
	
	if ( _bOnSubmit ) 
	{
		std::vector<DAT_CP4ClientInfo> vOpenedFiles;
		DAT_CPerforce::GetInstance()->P4Opened(_ulChangelist,&vOpenedFiles);
		BIG_INDEX ulIndex = BIG_C_InvalidIndex;

		// trying to find a map from the files in the changelist
		for ( UINT ui = 0 ; ui < vOpenedFiles.size() ; ui ++ ) 
		{
			if ( (ulIndex = BIG_ul_SearchKeyToFat(vOpenedFiles[ui].ulKey)) != BIG_C_InvalidIndex )
			{
				char asz_Path[BIG_C_MaxLenPath + BIG_C_MaxLenName] = {0};
				BIG_ComputeFullName(BIG_ParentFile(ulIndex), asz_Path);	

				char *pMap = NULL;
				if ( strstr(asz_Path,"01 Texture")) 
				{
					strcpy(aszType,"-Texture-");
				}
				else if ( strstr(asz_Path,"02 Modelisation")) 
				{
					strcpy(aszType,"-Modelisation-");
				}
				else if ( strstr(asz_Path,"03 Animation")) 
				{
					strcpy(aszType,"-Animations|Actions-");
				}
				else if ( strstr(asz_Path,"04 Technical")) 
				{
					strcpy(aszType,"-AI-");
				}
				else if ( strstr(asz_Path,"05 Audio")) 
				{
					strcpy(aszType,"-Audio-");
				}
				// its a map
				else if ( pMap = strstr(asz_Path,"06 Levels") ) 
				{
					pMap += strlen("06 Levels");
					if ( pMap ) pMap = strchr(pMap,'/') + 1 ;
					if ( pMap ) pMap = strchr(pMap,'/') + 1 ;
					if ( strchr(pMap,'/') ) strchr(pMap,'/')[0] = 0;
					sprintf(aszType,"-%s-",pMap);
					break;
				}
			}
		}
	}
	

	EDIA_cl_P4ChangelistCommentDialog ChangelistComment("Perforce changelist comment",
														aszType,
														strDescription.c_str(),
														_bOnSubmit,
														_bRevertUnchanged,
														_bVerifyLinks,
														TRUE);

	BOOL bOK = FALSE;
	for (UINT ui = 0; ui < 3 && !bOK; ui ++ )
	{
		if ( ChangelistComment.DoModal() == IDOK ) 
		{
			if ( ChangelistComment.mo_Comment.IsEmpty() || strstr(ChangelistComment.mo_Comment,"New changelist") != 0 ) 
			{
				M_MF()->MessageBox( "You must enter a proper comment", "Error", MB_OK );
			}
			else 
			{
				M_MF()->BeginWaitCursor();
				std::vector<DAT_CP4ClientInfo> vFiles;
				DAT_CPerforce::GetInstance()->P4Opened(_ulChangelist,&vFiles);
				char *pFiles;
				if ( vFiles.size() > 0 ) 
				{
					pFiles = new char[vFiles.size() * MAX_PATH];
					pFiles[0] = '\n';
					char* pFilesCurosor = pFiles;
					pFilesCurosor ++;
					for ( UINT ui = 0; ui < vFiles.size(); ui ++ )
					{	
						char p4Buffer[MAX_PATH];
						char p4Filename[MAX_PATH];
						DAT_CUtils::GetP4FileFromKey(vFiles[ui].ulKey,p4Filename,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
						sprintf(p4Buffer," \t%s  # %s\n",p4Filename,vFiles[ui].aszAction);
						strcpy(pFilesCurosor,p4Buffer);
						pFilesCurosor += strlen(pFilesCurosor);
					}
				}
				else 
				{
					pFiles = new char[MAX_PATH];
					memset(pFiles,0,MAX_PATH);
				}

				// change line-break format to accomodate Perforce
				std::string strPerforceComment;
				const char* szBegin = ChangelistComment.mo_Comment;
				while( const char* szFound = strstr( szBegin, "\r\n" ) )
				{
					strPerforceComment.append( szBegin, szFound );
					strPerforceComment.append( "\n\r" );
					szBegin = szFound + 2;
				}
				strPerforceComment.append( szBegin );
				std::string strFormatedComment;
				if ( ChangelistComment.mo_SubmitType != "" ) 
				{
					strFormatedComment = ChangelistComment.mo_SubmitType + "\n ";
				}

				if ( ChangelistComment.m_bTagForDemo ) 
				{
					strFormatedComment += "VERSION DEMO\n ";
				}

				if ( ChangelistComment.mo_BugFix != "" ) 
				{
					strFormatedComment += "Fix: " + ChangelistComment.mo_BugFix + "\n ";
				}


				strPerforceComment =  strFormatedComment + strPerforceComment;

				DAT_CPerforce::GetInstance()->P4Change(_ulChangelist, strPerforceComment.c_str( ), pFiles);

				delete[] pFiles;
				if ( ChangelistComment.GetAdditionnalVersion() )
					_strAdditionnalVersion = ChangelistComment.GetAdditionnalVersion();

				_bRevertUnchanged = ChangelistComment.GetRevertUnchanged();
				_bVerifyLinks = ChangelistComment.GetVerifyLinks();

				bOK = TRUE;
			}
		}
		else 
		{
			// canceling
			break;
		}
	}

	if (!bOK) 
	{
		M_MF()->MessageBox( "Operation canceled","Information",MB_OK);
	}

	return bOK;

}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnAdd()
/// \author    YCharbonneau
/// \date      2005-01-14
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnAdd()
{
	if ( DAT_CPerforce::GetInstance()->P4Connect() ) 
	{
		DAT_CPerforce::GetInstance()->SetWorkingChangelist(DAT_CPerforce::GetInstance()->P4Change(-1,"New changelist",""));
		Refresh();
		DAT_CPerforce::GetInstance()->P4Disconnect();
	}
}

void EPER_cl_Frame::OnResolve()
{
	const SEL_ITEM_LIST& vSelected =  mpo_TreeView->GetSelectedItems();
	if( vSelected.size() > 0 )
	{
		if ( DAT_CPerforce::GetInstance()->P4Connect() ) 
		{

			std::vector<BIG_KEY> vFstat;
			int iRes = 0 ;
			BOOL bApplyToAll = FALSE;
			for( UINT ui = 0; ui < vSelected.size() ; ui++ )
			{
				int iIcon = 0;
				
				if( vSelected[ui] != NULL && 
					mpo_TreeView->GetItemImage(vSelected[ui],iIcon,iIcon) && 
					iIcon == eICON_UNRESOLVED ) 
				{
					const ULONG ulKey = mpo_TreeView->GetItemData( vSelected[ ui ] );

					
					if ( !bApplyToAll) 
					{
						BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat(ulKey);
						char aszMessage[MAX_PATH] = {0};
						sprintf(aszMessage, "Resolving:  %s \n Keep your file ?\n [NO will sync content to head revision]",BIG_NameFile(ulIndex));
						iRes = M_MF()->MessageBox(aszMessage,"P4 - Resolve [ SHIFT (YES|NO|CANCEL) to apply to all ]",MB_YESNOCANCEL | MB_ICONQUESTION);
					}

					unsigned short shKeyState = GetKeyState(VK_SHIFT);
					shKeyState >>= 15;
					if( shKeyState == 1 )
					{
						bApplyToAll = TRUE;
					}
					
					if (  iRes == IDYES ) 
					{
						// -ay
						DAT_CPerforce::GetInstance()->P4Resolve(ulKey,TRUE);
					}
					else if ( iRes == IDNO ) 
					{
						// -at
						DAT_CPerforce::GetInstance()->P4Resolve(ulKey,FALSE);
					}

					vFstat.push_back(ulKey);
					// else do not resolve										
				}
			}
			std::vector<std::string> vFstatFile;
			DAT_CUtils::GetP4FilesFromVecKey(vFstat,vFstatFile,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
			DAT_CPerforce::GetInstance()->P4Fstat(vFstatFile);
			DAT_CPerforce::GetInstance()->P4Disconnect();
		}

		
		Refresh();
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnSetWorking()
/// \author    YCharbonneau
/// \date      2005-01-14
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnSetWorking()
{
	HTREEITEM hSelectedItem = mpo_TreeView->GetLastSelectedItem();
	if  (!hSelectedItem) return ;

	if ( mpo_TreeView->GetParentItem(hSelectedItem) == NULL  )
	{
		ULONG index = mpo_TreeView->GetItemData(hSelectedItem) ; 

		DAT_CPerforce::GetInstance()->SetWorkingChangelist(index);
		Refresh();
	}
}


//------------------------------------------------------------
//   void EPER_cl_Frame::OnDeleteEmpty()
/// \author    YCharbonneau
/// \date      2005-01-14
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnDeleteEmpty()
{
	const SEL_ITEM_LIST& vSelected =  mpo_TreeView->GetSelectedItems();
	if  (vSelected.size() > 0 )
	{
		char Question[MAX_PATH];
		sprintf(Question,"Delete selected changelist(s) ?");

		if ( M_MF()->MessageBox( Question, "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			if ( DAT_CPerforce::GetInstance()->P4Connect() ) 
			{
				for ( UINT ui = 0; ui < vSelected.size() ; ui++ ) 
				{
					if ( vSelected[ui] != NULL && mpo_TreeView->GetParentItem(vSelected[ui]) == NULL ) 
					{
						ULONG index = mpo_TreeView->GetItemData(vSelected[ui]) ; 
						if ( DAT_CPerforce::GetInstance()->P4ChangeDelete(index) ) 
						{			
							RemoveSelectedItem(vSelected[ui]);
						}
					}
				}
				mpo_TreeView->ClearSelection();
				DAT_CPerforce::GetInstance()->P4Disconnect();
			}

		}
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnSubmit()
/// \author    YCharbonneau
/// \date      2005-01-14
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnSubmit()
{
	const SEL_ITEM_LIST& vSelected =  mpo_TreeView->GetSelectedItems();
	if  (vSelected.size() > 0 )
	{
		if( !g_bAutoAccept )
		{
			if( M_MF()->MessageBox
				(
				"Submit selected changelist(s) ?",
				"Confirm",
				MB_YESNO | MB_ICONQUESTION
				) != IDYES )
			{
				return;
			}
		}

		bool bForcedRevertUnchanged = TRUE;
		// confirm actions
		unsigned short shKeyState = GetKeyState(VK_SHIFT);
		unsigned short ctrlKeyState = GetKeyState(VK_CONTROL);
		shKeyState >>= 15;
		ctrlKeyState >>= 15;
		if( shKeyState == 1 && ctrlKeyState == 1)
		{
			bForcedRevertUnchanged = M_MF()->MessageBox(
				"Revert unchanged file(s) before submitting ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES;
		}
	}
	Refresh();

}

//------------------------------------------------------------
//   void EPER_cl_Frame::ReplicateSubmitOnVersion(ULONG _ulChangelist, const std::string& _strAdditionnalVersion)
/// \author    YCharbonneau
/// \date      06/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::ReplicateSubmitOnVersion(ULONG _ulChangelist, const std::string& _strAdditionnalVersion)
{
	std::string strSelectedVersionPath = "//" + DAT_CPerforce::GetInstance()->GetP4Depot() + "/" + std::string(P4_VERSIONROOT) + _strAdditionnalVersion + "/";
	std::string strCurrentVersion = DAT_CPerforce::GetInstance()->GetBFVersion();
	// switching client view
	std::list<std::string> lstDirs;
	DAT_CPerforce::GetInstance()->P4Client(_strAdditionnalVersion.c_str(), lstDirs,std::string(""),FALSE ) ;

	char aszChangelist[20] = {0} ;
	sprintf(aszChangelist,"%d",_ulChangelist);
	std::string strComment = "Integrating changelist : ";
	strComment += aszChangelist ;
	strComment += " from version : " ;
	strComment +=  DAT_CPerforce::GetInstance()->GetBFVersion();
	std::string strDescription ;
	DAT_CPerforce::GetInstance()->P4Describe(_ulChangelist,strDescription);
	strComment += "\t" + strDescription;


	std::string strBranch = DAT_CPerforce::GetInstance()->GetBFVersion() + std::string("->") + _strAdditionnalVersion;

	DAT_CPerforce::GetInstance()->SetCreateChangelistDescription(strComment);
	DAT_CPerforce::GetInstance()->SetWorkingChangelist(-1);

	DAT_CPerforce::GetInstance()->P4Integrate(strBranch,aszChangelist,FALSE);
	std::vector<DAT_CP4ClientInfo> vFileInfo;
	DAT_CPerforce::GetInstance()->P4Describe(DAT_CPerforce::GetInstance()->GetWorkingChangelist(),&vFileInfo);

	std::vector<std::string> vstrFiles;
	
	for( UINT ui = 0 ; ui < vFileInfo.size(); ui ++ ) 
	{
		char aszP4File[MAX_PATH] = {0};
		DAT_CUtils::GetP4FileFromKey(vFileInfo[ui].ulKey,aszP4File,strSelectedVersionPath.c_str());
		vstrFiles.push_back(aszP4File);
	}
	
	DAT_CPerforce::GetInstance()->P4Resolve(vstrFiles,TRUE /* accept the bf revision */ );
	
	ULONG ulBranchChangelist = DAT_CPerforce::GetInstance()->GetWorkingChangelist();
	DAT_CPerforce::GetInstance()->P4Submit(ulBranchChangelist,FALSE);

	DAT_CPerforce::GetInstance()->SetCreateChangelistDescription("");
	DAT_CPerforce::GetInstance()->P4Client(strCurrentVersion.c_str(), lstDirs,std::string(""),FALSE ) ;
}
//------------------------------------------------------------
//   void EPER_cl_Frame::OnSubmitEdit()
/// \author    FFerland
/// \date      2005-04-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnSubmitEdit()
{
	if ( M_MF()->MessageBox( "Submit selected changelists and reopen them for edit ?","Confirm", MB_YESNO | MB_ICONQUESTION ) != IDYES )
		return;

	// set module variables 
	std::vector< ULONG > lstFilesToKeep;
	g_pListFiles = &lstFilesToKeep;
	g_bAutoAccept = true;

	// submit files
	OnSubmit( );

	// reopen files for edit
	PER_CDataCtrlEmulator emulator;

	for( size_t i = 0; i < lstFilesToKeep.size( ); ++i )
		emulator.AddIndex( lstFilesToKeep[ i ] );

	EditSelected( &emulator );

	// reset module variables
	g_pListFiles = NULL;
	g_bAutoAccept = false;
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnHistory()
/// \author    FFerland
/// \date      2005-02-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnHistory()
{
	const SEL_ITEM_LIST& vSelected =  mpo_TreeView->GetSelectedItems();
	if( vSelected.size() > 0 )
	{
		for( UINT ui = 0; ui < vSelected.size() ; ui++ )
		{
			if( vSelected[ui] != NULL ) 
			{
				const ULONG ulKey = mpo_TreeView->GetItemData( vSelected[ ui ] );
				DAT_CPerforce::GetInstance()->P4ShowHistory( ulKey );
			}
		}
		//mpo_TreeView->ClearSelection();
		//Refresh();
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnRevert()
/// \author    YCharbonneau
/// \date      2005-01-14
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnRevert()
{
	HTREEITEM hSelectedItem = mpo_TreeView->GetLastSelectedItem();
	if  (!hSelectedItem) return ;

	std::vector<std::string> vRevertedFiles;
	if ( mpo_TreeView->GetParentItem(hSelectedItem) == NULL )
	{
		const SEL_ITEM_LIST& vSelected =  mpo_TreeView->GetSelectedItems();
		if  (vSelected.size() > 0 )
		{
			if ( M_MF()->MessageBox( "Revert selected changelist(s) ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
			{
				if ( DAT_CPerforce::GetInstance()->P4Connect() ) 
				{
					for ( UINT ui = 0; ui < vSelected.size() ; ui++ ) 
					{
						assert( vSelected [ui] != NULL ) ;

						if ( mpo_TreeView->GetParentItem(vSelected[ui]) == NULL ) 
						{
							ULONG index = mpo_TreeView->GetItemData(vSelected[ui]) ; 
							DAT_CPerforce::GetInstance()->SetWorkingChangelist(index);
							DAT_CPerforce::GetInstance()->P4RevertChangelist(index,vRevertedFiles);
						}
					}
					mpo_TreeView->ClearSelection();
	
					if (	M_MF()->MessageBox( "Sync reverted file(s) ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES ) 
					{
						if ( IsWorldCurrentlyOpened() )
							DAT_CPerforce::GetInstance()->P4Sync(vRevertedFiles,"",TRUE);
					}
					else
					{
						DAT_CPerforce::GetInstance()->P4Fstat(vRevertedFiles);
					}

					DAT_CPerforce::GetInstance()->P4Disconnect();
					LINK_gb_RefreshOnlyFiles = TRUE;
					M_MF()->FatHasChanged();
					Refresh();
				}
			}
		}
	}
	else
	{
		// this is a file
			char Question[MAX_PATH];
			sprintf(Question,"Revert selected file(s) ?");

		if (	M_MF()->MessageBox( Question, "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
				DAT_CPerforce::GetInstance()->P4Connect() )
			{
				
				for ( unsigned int i = 0 ; i < mpo_TreeView->GetSelectedItems().size(); i++ ) 
				{
					assert( mpo_TreeView->GetSelectedItems()[i] != NULL ) ;

					if ( mpo_TreeView->GetParentItem(mpo_TreeView->GetSelectedItems()[i]) != NULL ) 
					{
						BIG_KEY ulKey =  mpo_TreeView->GetItemData(mpo_TreeView->GetSelectedItems()[i]);
						char aszFilename[MAX_PATH] = {0};
						DAT_CUtils::GetP4FileFromKey(ulKey,aszFilename,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
						vRevertedFiles.push_back(aszFilename);
						RemoveSelectedItem(mpo_TreeView->GetSelectedItems()[i]);
					}
				}

				if ( vRevertedFiles.size() > 0 )
				{				
					DAT_CPerforce::GetInstance()->P4Revert(vRevertedFiles,NULL);
					DAT_CPerforce::GetInstance()->P4Fstat(vRevertedFiles);
					if (M_MF()->MessageBox( "Sync reverted file(s) ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES ) 
					{
						if ( IsWorldCurrentlyOpened() )
							DAT_CPerforce::GetInstance()->P4Sync(vRevertedFiles,"",TRUE);
					}
				}
				
			DAT_CPerforce::GetInstance()->P4Disconnect();
			mpo_TreeView->ClearSelection();
			LINK_gb_RefreshOnlyFiles = TRUE;
			M_MF()->FatHasChanged();
		}
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnRevertUnchanged()
/// \author    YCharbonneau
/// \date      2005-01-14
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnRevertUnchanged()
{
	HTREEITEM hSelectedItem = mpo_TreeView->GetLastSelectedItem();
	if  (!hSelectedItem) return ;

	if ( mpo_TreeView->GetParentItem(hSelectedItem) == NULL )
	{
		const SEL_ITEM_LIST& vSelected =  mpo_TreeView->GetSelectedItems();
		if  (vSelected.size() > 0 )
		{
			if ( M_MF()->MessageBox( "Revert unchanged file(s) in selected changelist(s) ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
			{
				if ( DAT_CPerforce::GetInstance()->P4Connect() ) 
				{
					for ( UINT ui = 0; ui < vSelected.size() ; ui++ ) 
					{
						assert( vSelected [ui] != NULL ) ;
						OnRevertUnchanged((ULONG)mpo_TreeView->GetItemData(vSelected [ui]));
					}

					mpo_TreeView->ClearSelection();
					LINK_gb_RefreshOnlyFiles = TRUE;
					M_MF()->FatHasChanged();

					DAT_CPerforce::GetInstance()->P4Disconnect();
					Refresh();
				}
			}
		}
	}
	else
	{
		// this is a file
		if ( DAT_CPerforce::GetInstance()->P4Connect() ) 
		{
			char Question[MAX_PATH];
			sprintf(Question,"Revert unchanged selected file(s) ?");

			if ( M_MF()->MessageBox( Question, "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
			{
				for ( unsigned int i = 0 ; i < mpo_TreeView->GetSelectedItems().size(); i++ ) 
				{
					assert( mpo_TreeView->GetSelectedItems()[i] != NULL ) ;

					if ( mpo_TreeView->GetParentItem(mpo_TreeView->GetSelectedItems()[i]) != NULL ) 
					{
						ULONG ulKey = mpo_TreeView->GetItemData(mpo_TreeView->GetSelectedItems()[i]);
						if ( DAT_CPerforce::GetInstance()->P4Diff(ulKey) ) 
						{
							DAT_CPerforce::GetInstance()->P4Revert(ulKey,TRUE);
							RemoveSelectedItem(mpo_TreeView->GetSelectedItems()[i]);
						}
					}
				}
			}
			mpo_TreeView->ClearSelection();

			LINK_gb_RefreshOnlyFiles = TRUE;
			M_MF()->FatHasChanged();

			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnRevertUnchanged(ULONG _hChangelist)
/// \author    YCharbonneau
/// \date      20/02/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnRevertUnchanged(ULONG _ulChangelist)
{
	DAT_TChangelistInfo::const_iterator iter = m_mChangelist.find(_ulChangelist);
	if ( iter != m_mChangelist.end() ) 
	{
		std::vector<std::string> vFiles;
		std::vector<PerforceFileInfo*>& vFileInfo = iter->second->vFileInfo;
		for ( UINT ui = 0 ; ui < vFileInfo.size(); ui ++ )
		{
			vFiles.push_back(vFileInfo[ui]->strFilename);
		}

		
		DAT_CPerforce::GetInstance()->P4Fstat(vFiles);
		vFiles.clear();

		for ( UINT ui = 0 ; ui < vFileInfo.size(); ui ++ )
		{
			PerforceFileInfo* pInfo = vFileInfo[ui] ;
			ULONG ulKey = DAT_CUtils::GetKeyFromString(pInfo->strFilename.c_str()) ;
			ULONG ulFat = BIG_C_InvalidIndex  ;
			if ( ulKey != BIG_C_InvalidKey && 
				((ulFat = BIG_ul_SearchKeyToFat(ulKey)) != BIG_C_InvalidIndex) && 
				strcmp(BIG_P4Action(ulFat),P4_ADD) != 0 && 
				strcmp(BIG_P4Action(ulFat),P4_DELETE ) != 0 )
			{
				vFiles.push_back(pInfo->strFilename);
			}
		}

		if ( vFiles.size() > 0 )
		{
			std::vector<DAT_CP4ClientInfo*> vClientInfo;
			DAT_CPerforce::GetInstance()->P4Diff(vFiles,&vClientInfo);
			vFiles.clear();
			std::vector<std::string> vDifferentFile;
			for ( UINT ui = 0; ui < vClientInfo.size();ui++ ) 
			{
				if ( !(((DAT_CP4ClientInfoDiff*)vClientInfo[ui])->dwFileDiff & 
					(DAT_CP4ClientInfoDiff::eFileModified|DAT_CP4ClientInfoDiff::eFileMoved|DAT_CP4ClientInfoDiff::eFileRenamed)) ) 
				{
					char aszFilename[MAX_PATH] = {0};
					DAT_CUtils::GetP4FileFromKey(vClientInfo[ui]->ulKey,aszFilename,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
					vFiles.push_back(aszFilename);
				}

				delete vClientInfo[ui];
			}
			if ( vFiles.size() > 0 )
			{
				DAT_CPerforce::GetInstance()->P4Revert(vFiles,NULL,FALSE);
				DAT_CPerforce::GetInstance()->P4Fstat(vFiles);
			}
		}
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnAutoCheckout( BIG_INDEX _ulIndex,BOOL* _pIsCheckout )
/// \author    Ycharbonneau
/// \date      14-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnAutoCheckout( BIG_INDEX _ulIndex,BOOL* _pIsCheckout )
{
	if (  DAT_CPerforce::GetInstance()->P4Connect() ) 
	{
		std::vector<std::string> vFiles;
		std::string strP4File;
		DAT_CUtils::GetP4FileFromKey(BIG_FileKey(_ulIndex),strP4File,DAT_CPerforce::GetInstance()->GetP4Root());
		vFiles.push_back(strP4File);

		// forcing fstat on the file to retreive the latest info
		DAT_CPerforce::GetInstance()->P4Fstat(vFiles);
		BOOL bCanCheckout = TRUE;

		// its a new file
		if ( BIG_P4Time(_ulIndex) == 0 ) 
		{
			(*_pIsCheckout) = TRUE;
			bCanCheckout = FALSE;
			strcpy ( BIG_P4Action(_ulIndex), "new" ) ;
		}

		// already checkout
		if ( bCanCheckout && strlen(BIG_P4OwnerName(_ulIndex)) > 0) 
		{
			bCanCheckout = FALSE;
			// already checkout by another user
			if ( strcmp(BIG_P4OwnerName(_ulIndex),DAT_CPerforce::GetInstance()->GetUser()) == 0 ) 
			{
				(*_pIsCheckout) = TRUE;
			}
			else 
			{
				char aszMessage[MAX_PATH] = {0};
				sprintf(aszMessage,"[P4 ERROR] - Cannot checkout - file is already checkout by user:%s\nFile will stay read only.",BIG_P4OwnerName(_ulIndex));
				M_MF()->MessageBox( aszMessage, "Error", MB_OK  );
				(*_pIsCheckout) = FALSE;
			}
		}

		

		// checking if the client is sync with the server, 
		// if not we need to sync before any other operations
		if ( bCanCheckout && 
				(
				BIG_P4RevisionClient(_ulIndex) !=  BIG_P4RevisionServer(_ulIndex) || 
				!DAT_CPerforce::GetInstance()->P4Diff(_ulIndex)
				) 
			) 
		{
			if ( M_MF()->MessageBox( "[P4 WARNING] - File not sync with server, getlatest before checkout ? ", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
			{
				DAT_CPerforce::GetInstance()->P4Sync(_ulIndex,TRUE);
				bCanCheckout = TRUE;
			}
			// even local checkout cannot be done if we are not sync
			(*_pIsCheckout) = FALSE;
		}

		if ( bCanCheckout ) 
		{
			if ( M_MF()->MessageBox( "Do you want to checkout ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
			{
				std::string strP4File;
				std::vector<std::string> vFiles;

				DAT_CUtils::GetP4FileFromKey(BIG_FileKey(_ulIndex),strP4File,DAT_CPerforce::GetInstance()->GetP4Root());
				vFiles.push_back(strP4File);

				DAT_CPerforce::GetInstance()->P4Edit(vFiles);

				(*_pIsCheckout) = TRUE;
			}
			// else // local checkout
			//if (M_MF()->MessageBox( "Local checkout ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
			//{
			//	(*_pIsCheckout) = TRUE;
			//	strcpy(Bigmul_CurrentEditFile
			//}
			else 
			{
				(*_pIsCheckout) = FALSE;
			}
			Refresh();
		}
		
		DAT_CPerforce::GetInstance()->P4Disconnect();

	}
	else
	{
		DAT_CP4Message::OutputErrorMessage(" - You are not connected to perforce, file will be as local checkout.");
		(*_pIsCheckout) = TRUE;
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::OnClearLogWindow()
/// \author    NBeaufils
/// \date      02-Mar-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::OnClearLogWindow()
{
	mpo_Edit->SetSel(0, -1);
	mpo_Edit->Clear();
}

/*
 ===================================================================================================
 ===================================================================================================
 */

//------------------------------------------------------------
//   BOOL EPER_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPER_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	switch (_ul_Action)
	{
		
		case EPERC_ACTION_REVERT:
		case EPERC_ACTION_SUBMIT:														
		case EPERC_ACTION_SUBMIT_REOPEN:
		case EPERC_ACTION_DELETEEMPTY:
		case EPERC_ACTION_EDITCHANGELIST:
		case EPERC_ACTION_SETWORKINGCHANGELIST:
		case EPERC_ACTION_SHOWHISTORY:
		case EPERC_ACTION_REVERT_UNCHANGED:
		case EPERC_ACTION_RESOLVE:
		case EPERC_ACTION_SYNC_BF_TO_SELECTEDCHANGELIST:
		case EPERC_ACTION_SYNC_FILES_IN_SELECTEDCHANGELIST:
		case EPERC_ACTION_SYNC_FILES_IN_SELECTEDCHANGELIST_TO_PREVIOUSREVISION:
		case EPERC_ACTION_OPEN_FILES_IN_SELECTEDCHANGELIST:
		{
			return mpo_TreeView->GetLastSelectedItem() != NULL;
		}
		break;
		
		case EPERC_ACTION_REFRESH:
		case EPERC_ACTION_ADDCHANGELIST:
		case EPERC_ACTION_CLEARLOG:
		break;

	}
	return TRUE;
 
}


/*
 ===================================================================================================
 ===================================================================================================
 */

//------------------------------------------------------------
//   UINT EPER_cl_Frame::ui_OnActionState(ULONG _ul_Action)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
UINT EPER_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    UINT    ui_State;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ui_State = (UINT) - 1;
    
    return ui_State;
}


//------------------------------------------------------------
//   void EPER_cl_Frame::EditSelectedFiles()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::EditSelected(PER_CDataCtrl* _pItemCtrl)
{

	HDATACTRLITEM hItem = _pItemCtrl->GetFirstSelectedItem();

	// is dir 
	if ( _pItemCtrl->ItemIsDirectory( hItem ) )
	{
		if ( g_bAutoAccept || M_MF()->MessageBox( "Open directory for edit ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			BOOL _bAutoSubmit = FALSE;

			unsigned short shKeyState = GetKeyState(VK_SHIFT);
			unsigned short ctrlKeyState = GetKeyState(VK_CONTROL);
			shKeyState >>= 15;
			ctrlKeyState >>= 15;
			if( shKeyState == 1 && ctrlKeyState == 1)
			{
				if ( M_MF()->MessageBox( "Enable auto submit  ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
				{
					_bAutoSubmit = TRUE;
					EDIA_cl_P4ChangelistCommentDialog ChangelistComment("Perforce for auto-submitted changelists","","",FALSE,FALSE,FALSE,FALSE);
					if ( ChangelistComment.DoModal() == IDOK ) 
					{
						if ( ChangelistComment.mo_Comment.IsEmpty() || strstr(ChangelistComment.mo_Comment,"New changelist") != 0 ) 
						{
							M_MF()->MessageBox( "You must enter a proper comment", "Error", MB_OK );
							DAT_CPerforce::GetInstance()->P4Disconnect();
							return;
						}
						DAT_CPerforce::GetInstance()->SetCreateChangelistDescription(ChangelistComment.mo_Comment.GetBuffer());
						DAT_CPerforce::GetInstance()->SetWorkingChangelist(-1);
					}
				}
			}

			M_MF()->BeginWaitCursor();
			if ( DAT_CPerforce::GetInstance()->P4Connect() ) 
			{
				BIG_INDEX ulDir = _pItemCtrl->GetItemReference(hItem);
			
				std::vector<std::string> vFile;
				std::vector<BIG_INDEX> vFileIndex;
				DAT_CUtils::GetP4FilesFromDirIndex(ulDir,vFile,&vFileIndex,DAT_CPerforce::GetInstance()->GetP4Root().c_str(),TRUE);

				DAT_CPerforce::GetInstance()->P4Fstat(vFile);
				if (PerforceCheckFileUpToDate(vFileIndex))
				{

					std::vector<BIG_INDEX> vEditIndex;
					std::vector<BIG_INDEX> vAddIndex;

					EDIA_cl_MsgLinkDialog	SyncModifiedMessage("%s is not in Perforce. Do you want to add it ?","Confirm"); 
					SyncModifiedMessage.mb_ApplyToAll = FALSE;
					for ( UINT ui = 0 ; ui < vFileIndex.size() ; ui ++ ) 
					{
						BIG_INDEX index = vFileIndex[ui];

						if (BIG_P4RevisionServer(index) > 0)
						{
							vEditIndex.push_back(index);
						}
						else	// file is not on server yet
						{
							if (!strcmp(BIG_P4Action(index),P4_ADD))
							{			
								// file is already in add state.. do nothing
							}
							else
							{
								char aszMessage[MAX_PATH];
								sprintf(aszMessage, "%s is not in Perforce. Do you want to add it ?", BIG_NameFile(index));

								SyncModifiedMessage.mo_Text = aszMessage;
								int iRes = SyncModifiedMessage.DoModal();
								// Ask if user wants to add it
								if (iRes == IDYES)
								{
									vAddIndex.push_back(index);
								}
								else if (  iRes == IDCANCEL ) 
								{
									DAT_CPerforce::GetInstance()->P4Disconnect();
									M_MF()->EndWaitCursor();
									return;
								}
							}
						}
					}

					std::vector<std::string> vEditFiles;
					std::vector<std::string> vAddFiles;
					DAT_CUtils::GetP4FilesFromVecIndex(vEditIndex,vEditFiles,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
					DAT_CUtils::GetP4FilesFromVecIndex(vAddIndex,vAddFiles,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

					if ( vEditFiles.size() ) 
						DAT_CPerforce::GetInstance()->P4Edit(vEditFiles,_bAutoSubmit);

					if ( vAddFiles.size() )
						DAT_CPerforce::GetInstance()->P4Add(vAddFiles,_bAutoSubmit);
				}

				DAT_CPerforce::GetInstance()->SetCreateChangelistDescription("");
				DAT_CPerforce::GetInstance()->P4Disconnect();
			}
			M_MF()->EndWaitCursor();
		}
	}
	else if ( ( g_bAutoAccept || M_MF()->MessageBox( "Open selected files for edit ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES ) && 
				DAT_CPerforce::GetInstance()->P4Connect() )
	{
		M_MF()->BeginWaitCursor();
				
		std::vector<HDATACTRLITEM> vSelected;
		while ( hItem != -1 )
		{
			vSelected.push_back(hItem);
			hItem =_pItemCtrl->GetNextSelectedItem(hItem);
		} 

		std::vector<BIG_INDEX> vAllIndex;

		for ( UINT ui = 0 ; ui < vSelected.size() ; ui ++ ) 
		{
			vAllIndex.push_back(_pItemCtrl->GetItemReference(vSelected[ui]));
		}

		std::vector<std::string> vAllFiles;
		DAT_CUtils::GetP4FilesFromVecIndex(vAllIndex,vAllFiles,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
		DAT_CPerforce::GetInstance()->P4Fstat(vAllFiles);


		if (PerforceCheckFileUpToDate(vAllIndex))
		{

			std::vector<BIG_INDEX> vEditIndex;
			std::vector<BIG_INDEX> vAddIndex;

			EDIA_cl_MsgLinkDialog	SyncModifiedMessage("%s is not in Perforce. Do you want to add it ?","Confirm"); 
			SyncModifiedMessage.mb_ApplyToAll = FALSE;
			for ( UINT ui = 0 ; ui < vAllIndex.size() ; ui ++ ) 
			{
				BIG_INDEX index = vAllIndex[ui];

				if (BIG_P4RevisionServer(index) > 0)
				{
					vEditIndex.push_back(index);
				}
				else	// file is not on server yet
				{
					if (!strcmp(BIG_P4Action(index),P4_ADD))
					{			
						// file is already in add state.. do nothing
					}
					else
					{
						char aszMessage[MAX_PATH];
						sprintf(aszMessage, "%s is not in Perforce. Do you want to add it ?", BIG_NameFile(index));
						
						SyncModifiedMessage.mo_Text = aszMessage;
						int iRes = SyncModifiedMessage.DoModal();
						// Ask if user wants to add it
						if (iRes == IDYES)
						{
							vAddIndex.push_back(index);
						}
						else if (  iRes == IDCANCEL ) 
						{
							DAT_CPerforce::GetInstance()->P4Disconnect();
							M_MF()->EndWaitCursor();
							return;
						}
					}
				}
			}

			std::vector<std::string> vEditFiles;
			std::vector<std::string> vAddFiles;
			DAT_CUtils::GetP4FilesFromVecIndex(vEditIndex,vEditFiles,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
			DAT_CUtils::GetP4FilesFromVecIndex(vAddIndex,vAddFiles,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
			
			if ( vEditFiles.size() ) 
			DAT_CPerforce::GetInstance()->P4Edit(vEditFiles);
			
			if ( vAddFiles.size() )
			DAT_CPerforce::GetInstance()->P4Add(vAddFiles);
		}

		DAT_CPerforce::GetInstance()->P4Disconnect();
		M_MF()->EndWaitCursor();
	}



	LINK_gb_RefreshOnlyFiles = TRUE;
	M_MF()->FatHasChanged();
	Refresh();
}

//------------------------------------------------------------
//   void EPER_cl_Frame::SyncSelected()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::SyncSelected(PER_CDataCtrl* _pItemCtrl,BOOL _bForceSync)
{
	HDATACTRLITEM hItem = _pItemCtrl->GetFirstSelectedItem();

	// is dir 
	if ( _pItemCtrl->ItemIsDirectory( hItem ) )
	{
		char asz_Path[BIG_C_MaxLenPath + BIG_C_MaxLenName];
		BIG_ComputeFullName(_pItemCtrl->GetItemReference(hItem), asz_Path);
		M_MF()->BeginWaitCursor();
		if ( _pItemCtrl->GetItemReference(hItem) == BIG_Root() )
		{
			if ( M_MF()->MessageBox( "This will sync the entire bigfile, continue ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
				DAT_CPerforce::GetInstance()->P4Connect())
			{
				DAT_CPerforce::GetInstance()->P4FStatBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str());
				DAT_CPerforce::GetInstance()->P4SyncBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str(),NULL,_bForceSync);
				
				std::vector<BIG_INDEX> vIndex;
				DAT_CUtils::GetBFIndexesFromDirIndex(_pItemCtrl->GetItemReference(hItem),vIndex,TRUE);
				PerforceCheckFileUpToDate(vIndex,FALSE);
				DAT_CPerforce::GetInstance()->P4Disconnect();
			}
		}
		else 
		{

			if ( M_MF()->MessageBox( "This will sync the selected directory, continue ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
		  		 DAT_CPerforce::GetInstance()->P4Connect())
			{

				std::vector<std::string> vP4File;
				std::vector<BIG_INDEX>	vIndexFile;
				DAT_CUtils::GetP4FilesFromDirIndex(	_pItemCtrl->GetItemReference(hItem),
													vP4File,&vIndexFile,DAT_CPerforce::GetInstance()->GetP4Root().c_str(),TRUE);		

				if ( !_bForceSync ) 
				{
					DAT_CPerforce::GetInstance()->P4Fstat(vP4File);
					if ( PerforceCheckFileUpToDate(vIndexFile,FALSE) ) 
					{
                        //if (vP4File.size() > P4_FILE_SYNC_NB)
    						DAT_CPerforce::GetInstance()->P4SyncBF(	DAT_CPerforce::GetInstance()->GetP4Root().c_str(),asz_Path,FALSE);
                        //else
    					//	DAT_CPerforce::GetInstance()->P4SyncBFFiles(vP4File);

					}
				}
				else 
				{
					std::vector<std::string> vFiles;
					DAT_CUtils::GetP4FilesFromDirIndex(_pItemCtrl->GetItemReference(hItem),vFiles,NULL,DAT_CPerforce::GetInstance()->GetP4Root().c_str(),TRUE);		
					DAT_CPerforce::GetInstance()->P4Sync(vFiles,asz_Path,_bForceSync ) ;

					DAT_CPerforce::GetInstance()->P4SyncBF(	DAT_CPerforce::GetInstance()->GetP4Root().c_str(),
															asz_Path,FALSE);
				}




				// old code 
				//if ( !_bForceSync ) 
				//{
				//	DAT_CPerforce::GetInstance()->P4SyncBF(	DAT_CPerforce::GetInstance()->GetP4Root().c_str(),
				//											asz_Path,_bForceSync);
				//}
				//else 
				//{
				//	std::vector<std::string> vFiles;
				//	DAT_CUtils::GetP4FilesFromDirIndex(_pItemCtrl->GetItemReference(hItem),vFiles,NULL,DAT_CPerforce::GetInstance()->GetP4Root().c_str(),TRUE);		
				//	DAT_CPerforce::GetInstance()->P4Sync(vFiles,asz_Path,_bForceSync ) ;

				//	DAT_CPerforce::GetInstance()->P4SyncBF(	DAT_CPerforce::GetInstance()->GetP4Root().c_str(),
				//											asz_Path,FALSE);
				//}

				DAT_CPerforce::GetInstance()->P4Disconnect();
			}
		}
	}
	else if (	M_MF()->MessageBox( "Sync selected items ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
				DAT_CPerforce::GetInstance()->P4Connect() )
	{
		std::vector<HDATACTRLITEM> vSelected;
		while ( hItem != -1 )
		{
			vSelected.push_back(hItem);
			hItem =_pItemCtrl->GetNextSelectedItem(hItem);
		} 

		std::vector<std::string> vFiles;
		std::vector<BIG_INDEX> vIndexFile;

		for ( UINT ui = 0 ; ui < vSelected.size() ; ui ++ ) 
		{
			std::string strFile;
			// TreeCtrl returns BIG_INDEX for each of its HTREEITEM
			if ( _pItemCtrl->ItemReferenceType() == PER_CDataCtrl::eItemReferenceBIG_INDEX )
			{
				BIG_INDEX index = BIG_C_InvalidIndex;
				index = _pItemCtrl->GetItemReference(vSelected[ui]);

				if ( index == BIG_C_InvalidIndex ) continue;

				BIG_KEY ulKey = BIG_FileKey(index); 
				if ( ulKey == BIG_C_InvalidKey ) continue;
				vIndexFile.push_back(index);

				DAT_CUtils::GetP4FileFromKey(ulKey,strFile,DAT_CPerforce::GetInstance()->GetP4Root());
			}
			// TreeCtrl returns BIG_KEY for each of its HTREEITEM
			else 
			{
				BIG_KEY ulKey = _pItemCtrl->GetItemReference(vSelected[ui]);
				if ( ulKey == BIG_C_InvalidKey ) continue;
				BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat(ulKey);
				if ( ulIndex == BIG_C_InvalidIndex ) continue;

				DAT_CUtils::GetP4FileFromKey(ulKey,strFile,DAT_CPerforce::GetInstance()->GetP4Root());
				vIndexFile.push_back(ulIndex);
			}
			vFiles.push_back(strFile);
		}

		if ( vFiles.size() ) 
		{
			if ( _bForceSync ) 
			{
				DAT_CPerforce::GetInstance()->P4Sync(vFiles,"",TRUE);
			}
			else
			{
				DAT_CPerforce::GetInstance()->P4Fstat(vFiles);
				PerforceCheckFileUpToDate(vIndexFile,FALSE);
			}
		}

		DAT_CPerforce::GetInstance()->P4Disconnect();
		
	}

	LINK_gb_RefreshOnlyFiles = FALSE;
	M_MF()->EndWaitCursor();
	M_MF()->FatHasChanged();
	Refresh();
}

void EPER_cl_Frame::IntegrateSelected(PER_CDataCtrl* _pItemCtrl)
{
	
	HDATACTRLITEM hItem = _pItemCtrl->GetFirstSelectedItem();
	
	std::string strVersionFilter;
	GetFilter(IDC_P4VERSION,strVersionFilter);

	std::string strBranch = strVersionFilter + std::string("->") + DAT_CPerforce::GetInstance()->GetBFVersion() ;
	std::string strSelectedVersionPath = "//" + DAT_CPerforce::GetInstance()->GetP4Depot() + "/" + std::string(P4_VERSIONROOT) + strVersionFilter + "/";

	// is a changelist
	if ( _pItemCtrl->ItemIsDirectory( hItem ) )
	{
		if (	M_MF()->MessageBox( "Integrated selected changelist ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
			DAT_CPerforce::GetInstance()->P4Connect() )
		{
			ULONG ulChangelist = _pItemCtrl->GetItemReferenceData(hItem) ; 
			char aszChangelist[20] = {0} ;
			sprintf(aszChangelist,"%d",ulChangelist);
			std::string strComment = "Integrating changelist : ";
			strComment += aszChangelist ;
			strComment += " from version : " + strVersionFilter;

			DAT_CPerforce::GetInstance()->SetCreateChangelistDescription(strComment);
			DAT_CPerforce::GetInstance()->SetWorkingChangelist(-1);

			DAT_CPerforce::GetInstance()->P4Integrate(strBranch,aszChangelist,TRUE);
			std::vector<DAT_CP4ClientInfo> vFileInfo;
			DAT_CPerforce::GetInstance()->P4Describe(ulChangelist,&vFileInfo);

			std::vector<std::string> vstrFiles;
			for( UINT ui = 0 ; ui < vFileInfo.size(); ui ++ ) 
			{
				char aszP4File[MAX_PATH] = {0};
				DAT_CUtils::GetP4FileFromKey(vFileInfo[ui].ulKey,aszP4File,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
				vstrFiles.push_back(aszP4File);
			}
			DAT_CPerforce::GetInstance()->P4Resolve(vstrFiles,FALSE /* accept the server revision */ );
			DAT_CPerforce::GetInstance()->SetCreateChangelistDescription("");
			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
	}
	else if (	M_MF()->MessageBox( "Integrated selected items ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
		DAT_CPerforce::GetInstance()->P4Connect() )
	{
		std::vector<HDATACTRLITEM> vSelected;
		PerforceChangelistInfo* pChangelistInfo = NULL;
		if ( hItem != -1 ) 
		{
			HDATACTRLITEM hParent = _pItemCtrl->GetParentReference(hItem);
			ULONG ulChangelist = _pItemCtrl->GetItemReferenceData(hParent) ; 
			pChangelistInfo = m_mChangelist[ulChangelist];
		}

		while ( hItem != -1 )
		{
			vSelected.push_back(hItem);
			hItem =_pItemCtrl->GetNextSelectedItem(hItem);
		} 

		
		

		std::vector<std::string> vFiles;
		std::vector<BIG_INDEX> vIndexFile;
		std::vector<std::string> vResolvedFiles;


		for ( UINT ui = 0 ; ui < vSelected.size() ; ui ++ ) 
		{
			std::string strFile;
			// TreeCtrl returns BIG_INDEX for each of its HTREEITEM
			if ( _pItemCtrl->ItemReferenceType() == PER_CDataCtrl::eItemReferenceBIG_INDEX )
			{
				BIG_INDEX index = BIG_C_InvalidIndex;
				index = _pItemCtrl->GetItemReference(vSelected[ui]);

				BIG_KEY ulKey = BIG_C_InvalidKey;

				if ( index == BIG_C_InvalidIndex ) 
				{
					ulKey = _pItemCtrl->GetItemReferenceData(vSelected[ui]);
				}
				else 
				{
					ulKey = BIG_FileKey(index); 				
				}

				if ( ulKey == BIG_C_InvalidKey ) continue;

				DAT_CUtils::GetP4FileFromKey(ulKey,strFile,strSelectedVersionPath);
			}

			std::string strFileWithRevision = strFile; 
			for( UINT ui = 0 ; ui < pChangelistInfo->vFileInfo.size(); ui ++ ) 
			{
				if ( strFileWithRevision == pChangelistInfo->vFileInfo[ui]->strFilename ) 
				{
					strFileWithRevision += "#" + pChangelistInfo->vFileInfo[ui]->strFileRevision;
					break;
				}
			}

			vResolvedFiles.push_back(strFile);
			vFiles.push_back(strFileWithRevision);
		}

		if ( vFiles.size() ) 
		{
			std::string strComment = "Integrating file(s) from changelist:";
			strComment += pChangelistInfo->strChangelist;
			strComment += "from version : "  + strVersionFilter + "\n";
			strComment += pChangelistInfo->strDescription;

			DAT_CPerforce::GetInstance()->SetCreateChangelistDescription(strComment);
			DAT_CPerforce::GetInstance()->SetWorkingChangelist(-1);

			std::string strBranch = DAT_CPerforce::GetInstance()->GetBFVersion() + std::string("->") + strVersionFilter ;

			DAT_CPerforce::GetInstance()->P4Integrate(strBranch,vFiles,TRUE);
			DAT_CPerforce::GetInstance()->P4Resolve(vResolvedFiles,FALSE /* accept the server revision */ );
			DAT_CPerforce::GetInstance()->SetCreateChangelistDescription("");
		}

		DAT_CPerforce::GetInstance()->P4Disconnect();

	}

	M_MF()->EndWaitCursor();
	Refresh();
}
//------------------------------------------------------------
//   void EPER_cl_Frame::DeleteSelected()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::DeleteSelected(PER_CDataCtrl* _pItemCtrl)
{

	HDATACTRLITEM hItem = _pItemCtrl->GetFirstSelectedItem();

	// is dir 
	if ( _pItemCtrl->ItemIsDirectory( hItem ) )
	{
		if (	M_MF()->MessageBox( "Open directory for delete ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
				DAT_CPerforce::GetInstance()->P4Connect() )
		{
			M_MF()->BeginWaitCursor();
			BIG_INDEX ulDir = _pItemCtrl->GetItemReference(hItem);
			
			std::vector<std::string> vFile;
			std::vector<BIG_INDEX> vFileIndex;
			DAT_CUtils::GetP4FilesFromDirIndex(ulDir,vFile,&vFileIndex,DAT_CPerforce::GetInstance()->GetP4Root().c_str(),TRUE);

			DAT_CPerforce::GetInstance()->P4Fstat(vFile);
			if ( PerforceCheckFileUpToDate(vFileIndex) ) 
			{
				DAT_CPerforce::GetInstance()->P4Delete(vFile);
			}

			DAT_CPerforce::GetInstance()->SetCreateChangelistDescription("");
			
			M_MF()->EndWaitCursor();
			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
	}
	else if (	M_MF()->MessageBox( "Open selected files for delete ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
				DAT_CPerforce::GetInstance()->P4Connect() )
	{
		M_MF()->BeginWaitCursor();
		std::vector<HDATACTRLITEM> vSelected;
		while ( hItem != -1 )
		{
			vSelected.push_back(hItem);
			hItem =_pItemCtrl->GetNextSelectedItem(hItem);
		} 

		std::vector<std::string> vFiles;

		for ( UINT ui = 0 ; ui < vSelected.size() ; ui ++ ) 
		{
			std::string strFile;
			// TreeCtrl returns BIG_INDEX for each of its HTREEITEM
			if ( _pItemCtrl->ItemReferenceType() == PER_CDataCtrl::eItemReferenceBIG_INDEX )
			{
				BIG_INDEX index = BIG_C_InvalidIndex;
				index = _pItemCtrl->GetItemReference(vSelected[ui]);

				if ( index == BIG_C_InvalidIndex ) continue;

				BIG_KEY ulKey = BIG_FileKey(index); 
				if ( ulKey == BIG_C_InvalidKey ) continue;

				DAT_CUtils::GetP4FileFromKey(ulKey,strFile,DAT_CPerforce::GetInstance()->GetP4Root());
			}
			// TreeCtrl returns BIG_KEY for each of its HTREEITEM
			else 
			{
				BIG_KEY ulKey = _pItemCtrl->GetItemReference(vSelected[ui]);

				if ( ulKey == BIG_C_InvalidKey ) continue;

				DAT_CUtils::GetP4FileFromKey(ulKey,strFile,DAT_CPerforce::GetInstance()->GetP4Root());
			}
			vFiles.push_back(strFile);
		}

		DAT_CPerforce::GetInstance()->P4Delete(vFiles);
		DAT_CPerforce::GetInstance()->P4Disconnect();
		M_MF()->EndWaitCursor();
	}


	LINK_gb_RefreshOnlyFiles = TRUE;
	M_MF()->FatHasChanged();
	Refresh();
}

//------------------------------------------------------------
//   void EPER_cl_Frame::SyncDeleteSelected()
/// \author    Nbeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::SyncDeleteSelected(PER_CDataCtrl* _pItemCtrl)
{
	HDATACTRLITEM hItem = _pItemCtrl->GetFirstSelectedItem();

	// is dir 
	if ( _pItemCtrl->ItemIsDirectory( hItem ) )
	{
		if (	M_MF()->MessageBox( "Open directory for delete ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
			DAT_CPerforce::GetInstance()->P4Connect() )
		{
			M_MF()->BeginWaitCursor();
			BIG_INDEX ulDir = _pItemCtrl->GetItemReference(hItem);

			std::vector<std::string> vFile;
			std::vector<BIG_INDEX> vFileIndex;
			DAT_CUtils::GetP4FilesFromDirIndex(ulDir,vFile,&vFileIndex,DAT_CPerforce::GetInstance()->GetP4Root().c_str(),TRUE);

			DAT_CPerforce::GetInstance()->P4Fstat(vFile);
			if ( PerforceCheckFileUpToDate(vFileIndex) ) 
			{
				DAT_CPerforce::GetInstance()->P4Delete(vFile);
			}

			DAT_CPerforce::GetInstance()->SetCreateChangelistDescription("");

			M_MF()->EndWaitCursor();
			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
	}
	else if (	M_MF()->MessageBox( "Open selected files for delete ?\n\r(NOTE: files will be synced to your BF before being opened for delete)","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
		DAT_CPerforce::GetInstance()->P4Connect() )
	{
		M_MF()->BeginWaitCursor();
		std::vector<HDATACTRLITEM> vSelected;
		while ( hItem != -1 )
		{
			vSelected.push_back(hItem);
			hItem =_pItemCtrl->GetNextSelectedItem(hItem);
		} 

		std::vector<std::string> vFiles;

		for ( UINT ui = 0 ; ui < vSelected.size() ; ui ++ ) 
		{
			std::string strFile;
			// TreeCtrl returns BIG_INDEX for each of its HTREEITEM
			if ( _pItemCtrl->ItemReferenceType() == PER_CDataCtrl::eItemReferenceBIG_INDEX )
			{
				BIG_INDEX index = BIG_C_InvalidIndex;
				index = _pItemCtrl->GetItemReference(vSelected[ui]);

				if ( index == BIG_C_InvalidIndex ) continue;

				BIG_KEY ulKey = BIG_FileKey(index); 
				if ( ulKey == BIG_C_InvalidKey ) continue;

				DAT_CUtils::GetP4FileFromKey(ulKey,strFile,DAT_CPerforce::GetInstance()->GetP4Root());
			}
			// TreeCtrl returns BIG_KEY for each of its HTREEITEM
			else 
			{
				BIG_KEY ulKey = _pItemCtrl->GetItemReference(vSelected[ui]);

				if ( ulKey == BIG_C_InvalidKey ) continue;

				DAT_CUtils::GetP4FileFromKey(ulKey,strFile,DAT_CPerforce::GetInstance()->GetP4Root());
			}
			vFiles.push_back(strFile);
		}

		DAT_CPerforce::GetInstance()->P4Sync(vFiles);
		DAT_CPerforce::GetInstance()->P4Delete(vFiles);
		DAT_CPerforce::GetInstance()->P4Disconnect();
		M_MF()->EndWaitCursor();
	}


	LINK_gb_RefreshOnlyFiles = TRUE;
	M_MF()->FatHasChanged();
	Refresh();
}

//------------------------------------------------------------
//   void EPER_cl_Frame::ArrangeIntoChangelist()
/// \author    FFerland
/// \date      2005-03-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG EPER_cl_Frame::ArrangeIntoChangelist( const std::vector< ULONG >& lstIndexes )
{
	ULONG ulChangelist = 0;

	if( lstIndexes.empty( ) )
		return ulChangelist;

	bool bAlreadyConnected = DAT_CPerforce::GetInstance()->IsConnected() > 0;

	if( !bAlreadyConnected )
	{
		if( !DAT_CPerforce::GetInstance()->P4Connect() )
			return ulChangelist;
	}

	if( NeedNewChangelist( lstIndexes, m_mChangelist ) )
	{
		// create a new change list
		ulChangelist = DAT_CPerforce::GetInstance()->P4Change( -1, "New changelist", "" );
		DAT_CPerforce::GetInstance()->SetWorkingChangelist( ulChangelist );

		// move files to new changelist
		std::vector< ULONG >::const_iterator i = lstIndexes.begin( );
		std::vector< ULONG >::const_iterator e = lstIndexes.end( );

		std::vector<std::string> vFiles;
		DAT_CUtils::GetP4FilesFromVecIndex( lstIndexes, vFiles, DAT_CPerforce::GetInstance()->GetP4Root().c_str() );

		DAT_CPerforce::GetInstance()->P4Fstat( vFiles );

		std::vector< std::string > vAddFiles;
		std::vector< std::string > vReopenFiles;

		for( ; i != e; ++i )
		{
			const ULONG ulIndex = *i;
			const ULONG ulKey = BIG_FileKey( ulIndex );

			if( ulKey == BIG_C_InvalidKey )
				continue;

			std::string strFile;
			DAT_CUtils::GetP4FileFromKey( ulKey, strFile, DAT_CPerforce::GetInstance()->GetP4Root() );
			if( stricmp( BIG_P4Action( ulIndex ), "new" ) == 0 )
				vAddFiles.push_back( strFile );
			else
				vReopenFiles.push_back( strFile );
		}

		DAT_CPerforce::GetInstance()->P4Add( vAddFiles );
		DAT_CPerforce::GetInstance()->P4Reopen( vReopenFiles, ulChangelist );
	}
	else
	{
		// get changelist from first file in list since they are all in the same changelist
		const ULONG ulIndex = lstIndexes[ 0 ];
		ulChangelist = BIG_P4ChangeList( ulIndex );
	}

	// list files in changelist
	std::vector< DAT_CP4ClientInfo > vFiles;

	if( g_pListFiles != NULL )
		DAT_CPerforce::GetInstance()->P4Opened( ulChangelist, &vFiles );

	// save list of indices in g_pListFiles if it has been set for future usage
	if( g_pListFiles != NULL )
	{
		for( size_t n = 0; n < vFiles.size( ); ++n )
		{
			const BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat( vFiles[ n ].ulKey );
			g_pListFiles->push_back( ulIndex );
		}
	}

	if( !bAlreadyConnected )
		DAT_CPerforce::GetInstance()->P4Disconnect();

	return ulChangelist;
}

//------------------------------------------------------------
//   void EPER_cl_Frame::SubmitSelected()
/// \author    FFerland
/// \date      2005-03-03
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::SubmitSelected(PER_CDataCtrl* _pItemCtrl)
{
	const ULONG ulCurWorkingChangelist = DAT_CPerforce::GetInstance()->GetWorkingChangelist( FALSE );

	// this ensures we are in the correct changelist display ( current user, current version, pending ) 
	OnDefault();
	Refresh();

	HDATACTRLITEM hItem = _pItemCtrl->GetFirstSelectedItem();
	const BOOL bIsDir = _pItemCtrl->ItemIsDirectory( hItem );

	if( !g_bAutoAccept )
	{
		if( M_MF()->MessageBox
			(
			bIsDir ? "Check in selected directory ?" : "Check in selected files ?",
			"Confirm",
			MB_YESNO | MB_ICONQUESTION
			) != IDYES )
		{
			return;
		}
	}

	bool bForceRevertUnchanged = TRUE;
	// confirm actions
	unsigned short shKeyState = GetKeyState(VK_SHIFT);
	unsigned short ctrlKeyState = GetKeyState(VK_CONTROL);
	shKeyState >>= 15;
	ctrlKeyState >>= 15;
	if( shKeyState == 1 && ctrlKeyState == 1)
	{
		bForceRevertUnchanged = M_MF()->MessageBox(
			"Revert unchanged file(s) before submitting ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES;
	}

	// user has confirmed, proceed...
	M_MF()->BeginWaitCursor();
	std::vector< ULONG > lstIndexes;

	// get file keys to check in
	if( DAT_CPerforce::GetInstance()->P4Connect() )
	{
		if( bIsDir )
		{
			BIG_INDEX ulDir = _pItemCtrl->GetItemReference(hItem);

			UpdateDirPerforceInfo( ulDir );
			ProcessFolder( ulDir, lstIndexes );
		}
		else
		{
			UpdateFilePerforceInfo( _pItemCtrl, hItem );

			while( hItem != -1 )
			{
				BIG_INDEX ulIndex = _pItemCtrl->GetItemReference(hItem);
				hItem =_pItemCtrl->GetNextSelectedItem(hItem);

				if( ulIndex == BIG_C_InvalidIndex )
					continue;

				ProcessFile( ulIndex, lstIndexes );
			} 
		}
	}

	// if we have files to check in...
	bool bSubmitOk = false;
	ULONG ulChangelist = 0;

	if( !lstIndexes.empty( ) )
	{
		ulChangelist = ArrangeIntoChangelist( lstIndexes );

		// check if there are still files to be checked in
		std::vector< DAT_CP4ClientInfo> vFiles;
		if( DAT_CPerforce::GetInstance()->P4Connect( ) )
		{
			DAT_CPerforce::GetInstance()->P4Opened( ulChangelist, &vFiles );
			DAT_CPerforce::GetInstance()->P4Disconnect( );
		}

		if( vFiles.empty( ) )
			ulChangelist = 0; // invalidate changelist

		// submit new changelist
		if( ulChangelist != 0 && DAT_CPerforce::GetInstance()->P4Connect() )
		{

			std::string strAdditionnalVersion;
			BOOL bRevertUnchanged = TRUE;
			BOOL bVerifyLinks = TRUE;
			if ( EditChangelist(ulChangelist,TRUE,strAdditionnalVersion,bVerifyLinks,bRevertUnchanged)) 
			{
				// revert unchanged files if requested
				if( bRevertUnchanged && bForceRevertUnchanged)
				{
					
					std::vector< DAT_CP4ClientInfo >::iterator iter ;
					std::vector<std::string> vDiffFiles;

					for(iter = vFiles.begin( ) ; iter != vFiles.end( ); ++iter )
					{
						DAT_CP4ClientInfo* pInfo = &(*iter);
						BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat( pInfo->ulKey );

						if( stricmp( BIG_P4Action( ulIndex ), P4_ADD    ) != 0 &&
							stricmp( BIG_P4Action( ulIndex ), P4_DELETE ) != 0 )
						{
							std::string strFile;
							DAT_CUtils::GetP4FileFromKey( pInfo->ulKey, strFile, DAT_CPerforce::GetInstance()->GetP4Root() );
							vDiffFiles.push_back( strFile );
						}
					}

					vFiles.clear();

					DAT_CPerforce::GetInstance()->P4Fstat( vDiffFiles );

					std::vector< DAT_CP4ClientInfo* > vDiffFileInfo;
					DAT_CPerforce::GetInstance()->P4Diff( vDiffFiles, &vDiffFileInfo );
					vDiffFiles.clear();
					for ( UINT ui = 0 ; ui < vDiffFileInfo.size() ; ui ++ )
					{				
						if ( !(((DAT_CP4ClientInfoDiff*)vDiffFileInfo[ui])->dwFileDiff & 
							(DAT_CP4ClientInfoDiff::eFileModified|DAT_CP4ClientInfoDiff::eFileMoved|DAT_CP4ClientInfoDiff::eFileRenamed)) ) 
						{
							std::string strFile;
							DAT_CUtils::GetP4FileFromKey( vDiffFileInfo[ui]->ulKey, strFile, DAT_CPerforce::GetInstance()->GetP4Root() );
							vDiffFiles.push_back( strFile );
						}
						delete vDiffFileInfo[ui];
					}

					if ( vDiffFiles.size() > 0 )
					{
						DAT_CPerforce::GetInstance()->P4Revert(vDiffFiles);
						DAT_CPerforce::GetInstance()->P4Fstat( vDiffFiles );
					}
				}
				
				if ( DAT_CPerforce::GetInstance()->P4Submit(ulChangelist,bVerifyLinks) != 0 ) 
				{
					if ( strAdditionnalVersion != "" ) 
					{
						ReplicateSubmitOnVersion(ulChangelist,strAdditionnalVersion);
					}
				}
			}

			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
	}

	if( ulChangelist == 0 )
		M_MF()->MessageBox( "Nothing to check in! All files were either reverted or not checked out.", "Cancelled", MB_OK | MB_ICONEXCLAMATION );

	LINK_gb_RefreshOnlyFiles = TRUE;
	M_MF()->FatHasChanged();
	
	Refresh();
	

	M_MF()->EndWaitCursor();
}

//------------------------------------------------------------
//   void EPER_cl_Frame::SubmitEditSelected()
/// \author    FFerland
/// \date      2005-03-23
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::SubmitEditSelected(PER_CDataCtrl* _pItemCtrl)
{
	if ( M_MF()->MessageBox( "Submit selected items and reopen them for edit ?","Confirm", MB_YESNO | MB_ICONQUESTION ) != IDYES )
		return;

	// set module variables 
	std::vector< ULONG > lstFilesToKeep;
	g_pListFiles = &lstFilesToKeep;
	g_bAutoAccept = true;

	// submit files
	SubmitSelected( _pItemCtrl );

	// reopen files for edit
	PER_CDataCtrlEmulator emulator;

	for( size_t i = 0; i < lstFilesToKeep.size( ); ++i )
		emulator.AddIndex( lstFilesToKeep[ i ] );

	EditSelected( &emulator );

	// reset module variables
	g_pListFiles = NULL;
	g_bAutoAccept = false;
}

//------------------------------------------------------------
//   void EPER_cl_Frame::AddSelected()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::AddSelected(PER_CDataCtrl* _pItemCtrl)
{
	HDATACTRLITEM hItem = _pItemCtrl->GetFirstSelectedItem();

	// is dir 
	if ( _pItemCtrl->ItemIsDirectory( hItem ) )
	{
		if (	M_MF()->MessageBox( "Add files in selected directory ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
				DAT_CPerforce::GetInstance()->P4Connect())
		{
			BOOL _bAutoSubmit = FALSE;
			
			unsigned short shKeyState = GetKeyState(VK_SHIFT);
			unsigned short ctrlKeyState = GetKeyState(VK_CONTROL);
			shKeyState >>= 15;
			ctrlKeyState >>= 15;
			if( shKeyState == 1 && ctrlKeyState == 1)
			{
				if ( M_MF()->MessageBox( "Enable auto submit  ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES )
				{
					_bAutoSubmit = TRUE;
					EDIA_cl_P4ChangelistCommentDialog ChangelistComment("Perforce for auto-submitted changelists","","",FALSE,FALSE,FALSE,FALSE);
					if ( ChangelistComment.DoModal() == IDOK ) 
					{
						if ( ChangelistComment.mo_Comment.IsEmpty() || strstr(ChangelistComment.mo_Comment,"New changelist") != 0 ) 
						{
							M_MF()->MessageBox( "You must enter a proper comment", "Error", MB_OK );
							DAT_CPerforce::GetInstance()->P4Disconnect();
							return;
						}
						DAT_CPerforce::GetInstance()->SetCreateChangelistDescription(ChangelistComment.mo_Comment.GetBuffer());
						DAT_CPerforce::GetInstance()->SetWorkingChangelist(-1);
					}
				}
			}
			
			BIG_INDEX ulDir = _pItemCtrl->GetItemReference(hItem);
			
			std::vector<std::string> vFile;
			std::vector<BIG_INDEX> vFileIndex;
			DAT_CUtils::GetP4FilesFromDirIndex(ulDir,vFile,&vFileIndex,DAT_CPerforce::GetInstance()->GetP4Root().c_str(),TRUE);

			DAT_CPerforce::GetInstance()->P4Fstat(vFile);
			DAT_CPerforce::GetInstance()->P4Add(vFile,_bAutoSubmit);
			DAT_CPerforce::GetInstance()->SetCreateChangelistDescription("");
			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
	}
	else if (	M_MF()->MessageBox( "Add selected files ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
				DAT_CPerforce::GetInstance()->P4Connect() )
	{
		M_MF()->BeginWaitCursor();
		std::vector<HDATACTRLITEM> vSelected;
		while ( hItem != -1 )
		{
			vSelected.push_back(hItem);
			hItem =_pItemCtrl->GetNextSelectedItem(hItem);
		} 

		std::vector<std::string> vFiles;

		for ( UINT ui = 0 ; ui < vSelected.size() ; ui ++ ) 
		{
			std::string strFile;
			// TreeCtrl returns BIG_INDEX for each of its HTREEITEM
			if ( _pItemCtrl->ItemReferenceType() == PER_CDataCtrl::eItemReferenceBIG_INDEX )
			{
				BIG_INDEX index = BIG_C_InvalidIndex;
				index = _pItemCtrl->GetItemReference(vSelected[ui]);

				if ( index == BIG_C_InvalidIndex ) continue;

				BIG_KEY ulKey = BIG_FileKey(index); 
				if ( ulKey == BIG_C_InvalidKey ) continue;

				DAT_CUtils::GetP4FileFromKey(ulKey,strFile,DAT_CPerforce::GetInstance()->GetP4Root());
			}
			// TreeCtrl returns BIG_KEY for each of its HTREEITEM
			else 
			{
				BIG_KEY ulKey = _pItemCtrl->GetItemReference(vSelected[ui]);

				if ( ulKey == BIG_C_InvalidKey ) continue;

				DAT_CUtils::GetP4FileFromKey(ulKey,strFile,DAT_CPerforce::GetInstance()->GetP4Root());
			}
			vFiles.push_back(strFile);
		}

		DAT_CPerforce::GetInstance()->P4Add(vFiles);
		DAT_CPerforce::GetInstance()->P4Disconnect();
		M_MF()->EndWaitCursor();
	}
		
	
	LINK_gb_RefreshOnlyFiles = TRUE;
	M_MF()->FatHasChanged();
	Refresh();
}

//------------------------------------------------------------
//   void EPER_cl_Frame::RevertSelected()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::RevertSelected(PER_CDataCtrl* _pItemCtrl)
{
	HDATACTRLITEM hItem = _pItemCtrl->GetFirstSelectedItem();

	// is dir 
	if (	_pItemCtrl->ItemIsDirectory( hItem ) && 
			M_MF()->MessageBox( "Revert selected directory ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
			DAT_CPerforce::GetInstance()->P4Connect()
		)
	{
		M_MF()->BeginWaitCursor();
		BIG_INDEX ulDir = _pItemCtrl->GetItemReference(hItem);

		std::vector<std::string> vFile;
		std::vector<BIG_INDEX> vFileIndex;
		DAT_CUtils::GetP4FilesFromDirIndex(ulDir,vFile,&vFileIndex,DAT_CPerforce::GetInstance()->GetP4Root().c_str(),TRUE);

		DAT_CPerforce::GetInstance()->P4Revert(vFile);

		if (	M_MF()->MessageBox( "Sync reverted file(s) ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES ) 
		{
			if ( IsWorldCurrentlyOpened() )
				DAT_CPerforce::GetInstance()->P4Sync(vFile,"",TRUE);
		}

		DAT_CPerforce::GetInstance()->P4Fstat(vFile);
		DAT_CPerforce::GetInstance()->P4Disconnect();

		M_MF()->EndWaitCursor();
	}
	else if (	M_MF()->MessageBox( "Revert selected file(s) ?","Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES && 
				DAT_CPerforce::GetInstance()->P4Connect() )
	{
		M_MF()->BeginWaitCursor();
		std::vector<HDATACTRLITEM> vSelected;
		while ( hItem != -1 )
		{
			vSelected.push_back(hItem);
			hItem =_pItemCtrl->GetNextSelectedItem(hItem);
		} 

		std::vector<std::string> vRevertedFiles;
		for ( UINT ui = 0 ; ui < vSelected.size() ; ui ++ ) 
		{
			BIG_INDEX index = BIG_C_InvalidIndex;
			index = _pItemCtrl->GetItemReference(vSelected[ui]);

			if ( index == BIG_C_InvalidIndex ) continue;

			ULONG ulKey = BIG_FileKey(index); 
			if ( ulKey == BIG_C_InvalidKey ) continue;

			char aszFilename[MAX_PATH] = {0};
			DAT_CUtils::GetP4FileFromKey(ulKey,aszFilename,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
			vRevertedFiles.push_back(aszFilename);

		}
		DAT_CPerforce::GetInstance()->P4Revert(vRevertedFiles);
		
		if (	M_MF()->MessageBox( "Sync reverted file(s) ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDYES ) 
		{
			if ( IsWorldCurrentlyOpened() )
				DAT_CPerforce::GetInstance()->P4Sync(vRevertedFiles,"",TRUE);
		}

		DAT_CPerforce::GetInstance()->P4Fstat(vRevertedFiles);

		DAT_CPerforce::GetInstance()->P4Disconnect();
		M_MF()->EndWaitCursor();
	}

	LINK_gb_RefreshOnlyFiles = TRUE;
	M_MF()->FatHasChanged();
	Refresh();
}

//------------------------------------------------------------
//   void EPER_cl_Frame::ShowHistorySelected()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::ShowHistorySelected(PER_CDataCtrl* _pItemCtrl)
{
	HDATACTRLITEM hItem = _pItemCtrl->GetFirstSelectedItem();

	if( hItem == 0xFFFFFFFF )
	{
		M_MF( )->MessageBox( "No file selected! Please load an MDL file instead of an FCT file!", "Error", MB_OK | MB_ICONEXCLAMATION );
		return;
	}

	// is dir 
	if ( _pItemCtrl->ItemIsDirectory( hItem ) )
	{
	}
	else 
	{
		BIG_INDEX index = _pItemCtrl->GetItemReference(hItem);
		if ( index == BIG_C_InvalidIndex ) return;

		ULONG ulKey = BIG_FileKey(index); 
		if ( ulKey == BIG_C_InvalidKey ) return;

		DAT_CPerforce::GetInstance()->P4ShowHistory(ulKey);
	} 

	if( g_bSynchedInHistory )
		Refresh();
}

//------------------------------------------------------------
//   void EPER_cl_Frame::DiffSelected()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::DiffSelected(PER_CDataCtrl* _pItemCtrl)
{
	HDATACTRLITEM hItem = _pItemCtrl->GetFirstSelectedItem();

	if( hItem == 0xFFFFFFFF )
	{
		M_MF( )->MessageBox( "No file selected! Please load an MDL file instead of an FCT file!", "Error", MB_OK | MB_ICONEXCLAMATION );
		return;
	}

	// is dir 
	if ( _pItemCtrl->ItemIsDirectory( hItem ) )
	{
	}
	else
	{
		BIG_INDEX index = _pItemCtrl->GetItemReference(hItem);
		if ( index == BIG_C_InvalidIndex ) return;

		BIG_KEY ulKey = BIG_FileKey(index); 
		if ( ulKey == BIG_C_InvalidKey ) return;
	
		std::string strOutputFilename1;
		std::string strOutputFilename2;

		std::vector< DAT_CP4ClientInfoHeader* > vFileInfo;
		if ( DAT_CPerforce::GetInstance()->P4Connect() )
		{
			DAT_CPerforce::GetInstance()->P4FileInfo( ulKey, &vFileInfo );
			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
		else
			return;

		if( vFileInfo.empty( ) )
		{
			M_MF()->MessageBox( "Nothing to compare against!", "Unable to compare files", MB_OK | MB_ICONERROR );
			return;
		}

		strOutputFilename2 = DAT_CUtils::MakeTempFName( "ReadOnly-0-Head-Rev", *vFileInfo[ 0 ] );
		strOutputFilename1 = DAT_CUtils::MakeTempFName( "CurBF", *vFileInfo[ 0 ] );

		for ( UINT ui = 0; ui < vFileInfo.size(); ui++ ) 
			delete vFileInfo[ui];

		// get head revision from Perforce
		StrBuf buffer;

		if ( DAT_CPerforce::GetInstance()->P4Connect() )
		{
			DAT_CPerforce::GetInstance()->P4Print( ulKey, 0, buffer, true );
			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
		else
			return;

		CFile file( strOutputFilename2.c_str( ), CFile::modeCreate | CFile::modeWrite );
		file.Write( buffer.Value( ), buffer.Length( ) );
		file.Flush( );
		file.Close( );

		// extract current file from BF
		DWORD ulBufferSize = 0;
		char* pBfBuffer = DAT_CUtils::PrepareFileBuffer( index, ulBufferSize );

		if( pBfBuffer )
		{
			CFile file( strOutputFilename1.c_str( ), CFile::modeCreate | CFile::modeWrite );
			file.Write( pBfBuffer, ulBufferSize );
			file.Flush( );
			file.Close( );

			delete[] pBfBuffer;
		}

		// call comparison tool
		DAT_CUtils::RunDiff( strOutputFilename2.c_str( ), strOutputFilename1.c_str( ), ulKey, NULL );
	}

}

//------------------------------------------------------------
//   void EPER_cl_Frame::FlushFiles()
/// \author    STourangeau
/// \date      2005-01-24
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::FlushFiles(std::vector<ULONG >& _vFiles, ULONG rev)
{
	if ( DAT_CPerforce::GetInstance()->P4Connect() )
	{
		DAT_CPerforce::GetInstance()->P4Flush(_vFiles, rev);
		DAT_CPerforce::GetInstance()->P4Disconnect();
	}
}

//------------------------------------------------------------
//   void EPER_cl_Frame::FlushDir()
/// \author    STourangeau
/// \date      2005-01-24
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::FlushDir(BIG_INDEX _ulDirIndex, ULONG rev)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static std::vector<ULONG > v_Files;
	static ULONG ul_callLevel = 0;
	BIG_INDEX   ul_SubDir;
	BIG_INDEX	ul_FileIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// handle recursivity
	if (ul_callLevel == 0)
		v_Files.clear();
	ul_callLevel++;

	/* Parse all subdirs. */
	ul_SubDir = BIG_SubDir(_ulDirIndex);
	while(ul_SubDir != BIG_C_InvalidIndex)
	{
		FlushDir(ul_SubDir, rev);
		ul_SubDir = BIG_NextDir(ul_SubDir);
	}

	// check for user cancel 
//	if(LINK_gb_EscapeDetected) 
//	{
//		return 0 ;
//	}
	
	// executing requested operation on each file
	ul_FileIndex = BIG_FirstFile(_ulDirIndex);
	while(ul_FileIndex != BIG_C_InvalidIndex)
	{
		v_Files.push_back(BIG_FileKey(ul_FileIndex));

		/* Pass to brother */
		ul_FileIndex = BIG_NextFile(ul_FileIndex);
	}

	ul_callLevel--;

	// If we are done recursing, call the flush on all files we found
	if (ul_callLevel == 0)
		FlushFiles(v_Files, rev);
}

//------------------------------------------------------------
//   void EPER_cl_Frame::ServerSync()
/// \author    STourangeau
/// \date      2005-01-24
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPER_cl_Frame::ServerSync(PER_CDataCtrl* _pItemCtrl)
{
	if ( M_MF()->MessageBox( "Performing a Server Sync will cleanup your BF and make it a mirror of the server.\nALL LOCAL FILES WILL BE DELETED!", "Confirm", MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2 ) == IDYES )
	{
		LINK_PrintStatusMsg("Removing local files that are not in the BF");
		M_MF()->FatHasChanged();
		if (DAT_CPerforce::GetInstance()->P4Connect())
		{		
			DAT_CPerforce::GetInstance()->P4FStatBF(DAT_CPerforce::GetInstance()->GetP4Root().c_str());
			DAT_CPerforce::GetInstance()->P4Disconnect();
		}
		DeleteLocalFiles(BIG_Root());
		M_MF()->FatHasChanged();
		LINK_PrintStatusMsg("");

		SyncSelected(_pItemCtrl, false);

		M_MF()->MessageBox( "All local files have been deleted and files updated to latest revision. If you have files in local checkout\nand would want to get the server version, you should perform a Force GetLatest on ROOT.", "Confirm", MB_OK );
	}
}

//------------------------------------------------------------
//   void DeleteLocalFiles()
/// \author    STourangeau
/// \date      2005-01-24
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DeleteLocalFiles(BIG_INDEX ul_DirIndex)
{
	BIG_INDEX	ul_SubDirIndex;
	BIG_INDEX	ul_FileIndex;

	// Parse all subdirs
	while(ul_DirIndex != BIG_C_InvalidIndex)
	{
		// Check each file.. if server rev is 0.. it means it is not in P4.. delete it
		ul_FileIndex = BIG_FirstFile(ul_DirIndex);
		while(ul_FileIndex != BIG_C_InvalidIndex)
		{
			if (BIG_P4RevisionServer(ul_FileIndex) == 0)
			{
				BIG_INDEX ul_NextIndex = BIG_NextFile(ul_FileIndex);
				BIG_DeleteFile(ul_DirIndex, ul_FileIndex);
				ul_FileIndex = ul_NextIndex;
			}
			else
			{
				// Pass to brother
				ul_FileIndex = BIG_NextFile(ul_FileIndex);
			}
		}

		// Recurse for all subdirs
		ul_SubDirIndex = BIG_SubDir(ul_DirIndex);
		while(ul_SubDirIndex != BIG_C_InvalidIndex)
		{
			DeleteLocalFiles(ul_SubDirIndex);
			ul_SubDirIndex = BIG_NextDir(ul_SubDirIndex);
		}

		// Check if the directory is empty.. remove it
		if (BIG_FirstFile(ul_DirIndex) == BIG_C_InvalidIndex && 
			BIG_SubDir(ul_DirIndex) == BIG_C_InvalidIndex)
		{
			char asz_DirName[MAX_PATH];
			BIG_INDEX ul_NextDir = BIG_NextDir(ul_DirIndex);
			BIG_ComputeFullName(ul_DirIndex, asz_DirName);
			if (BIG_ul_SearchDir(asz_DirName) != BIG_C_InvalidIndex)
				BIG_DelDir(asz_DirName);
			ul_DirIndex = ul_NextDir;
		}
		else
		{		
			ul_DirIndex = BIG_NextDir(ul_DirIndex);
		}
	}
}

#endif

