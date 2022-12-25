//------------------------------------------------------------------------------
// Filename   :PERCframe_msg.cpp
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: Implementation of PERCframe_msg
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "Res/Res.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "PERcframe.h"
#include "PERtreectrl.h"
#include "EDImsg.h"
#include "EDIicons.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDItors/Sources/BROwser/BROmsg.h"
#include "EDItors\Sources\BROwser\BROframe.h"
#include "EDItors\Sources\BROwser\BROtreectrl.h"
#include "EDItors\Sources\BROwser\BROlistctrl.h"
#include "EDItors\Sources\BROwser\BROgrpctrl.h"
#include "ENGine/Sources/ENGinit.h"

#include "EDItors/Sources/PERForce/PERmsg.h"
#include "PERCDataCtrl.h"
#include "DIAlogs/DIAmsglink_dlg.h"


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------
extern BOOL EDI_gb_OpVSS;
//extern EDIA_cl_MsgLinkDialog	EDI_go_MsgTruncateFiles;
//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   int EPER_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int EPER_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	//EDI_go_MsgTruncateFiles.mb_ApplyToAll = FALSE;
	BOOL bReturn = TRUE;

    switch(_ul_Msg)
    {

	case EDI_MESSAGE_CANDRAGDROP:
	{
		/* Only receive dirs and files */
		EDI_tdst_DragDrop* pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;

		if(pst_DragDrop->i_Type == EDI_DD_Long)
		{
			bReturn = FALSE;
			break;
		}
		
		if(	(pst_DragDrop->ul_FatDir == BIG_C_InvalidIndex) && 
			(pst_DragDrop->ul_FatFile == BIG_C_InvalidIndex))
		{
			bReturn = FALSE;
			break;
		}

		if(pst_DragDrop->i_Type != EDI_DD_File) 
		{
			bReturn = FALSE;
			break;
		}
		
		UINT uFlags;
		POINT pt = pst_DragDrop->o_Pt;

		mpo_TreeView->ScreenToClient(&pt);
		HTREEITEM hItem = mpo_TreeView->HitTest(pt,&uFlags);
		
		// if we have hit and its a changelist
		if ( (hItem != NULL) && (TVHT_ONITEM & uFlags) && 
			 mpo_TreeView->GetParentItem(hItem) == NULL ) 
		{
			bReturn = TRUE;
			break;
		}
		
		bReturn = FALSE;
		break;
	}
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELDATA: 
		bReturn = FALSE;
		break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    case EDI_MESSAGE_CANSELFILE:
		bReturn = FALSE;
		break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EDI_MESSAGE_FATHASCHANGED:
	case EDI_MESSAGE_REFRESH:
		bReturn = FALSE;
		break;

	case EPER_MESSAGE_AUTOCHECKOUT:
		//OnAutoCheckout(_ul_Param1,(BOOL*)_ul_Param2);
		// auto checkout has been deactivated for gold release.
		*((BOOL*)_ul_Param2) = TRUE;
		bReturn = FALSE;
		break;

	case EPER_MESSAGE_REFRESH:
		Refresh(_ul_Param1);
		bReturn = FALSE;
		break;

	case EPER_MESSAGE_EDIT_SELECTED:
		// Make sure frame is active and shown
		//mpo_MyView->SetCurSel(this->mi_NumPane);
		//mpo_MyView->OnChangePane(NULL, NULL);
		
		EditSelected((PER_CDataCtrl*)_ul_Param1);

		bReturn = FALSE;
		break;

	case EPER_MESSAGE_ADD_SELECTED:
		// Make sure frame is active and shown
		//mpo_MyView->SetCurSel(this->mi_NumPane);
		//mpo_MyView->OnChangePane(NULL, NULL);

		AddSelected((PER_CDataCtrl*)_ul_Param1);
		bReturn = FALSE;
		break;

	case EPER_MESSAGE_SHOWHISTORY_SELECTED:
		ShowHistorySelected((PER_CDataCtrl*)_ul_Param1);
		bReturn = FALSE;
		break;
	
	case EPER_MESSAGE_REVERT_SELECTED:
		// Make sure frame is active and shown
		//mpo_MyView->SetCurSel(this->mi_NumPane);
		//mpo_MyView->OnChangePane(NULL, NULL);

		RevertSelected((PER_CDataCtrl*)_ul_Param1);
		bReturn = FALSE;
		break;

	case EPER_MESSAGE_SYNC_SELECTED:
		if ( IsWorldCurrentlyOpened() )
		{
			// Make sure frame is active and shown
			//mpo_MyView->SetCurSel(this->mi_NumPane);
			//mpo_MyView->OnChangePane(NULL, NULL);
		
			SyncSelected((PER_CDataCtrl*)_ul_Param1,FALSE);
		}
		bReturn = FALSE;
		break;

	case EPER_MESSAGE_FORCESYNC_SELECTED:
		if ( IsWorldCurrentlyOpened() )
		{
			// Make sure frame is active and shown
			//mpo_MyView->SetCurSel(this->mi_NumPane);
			//mpo_MyView->OnChangePane(NULL, NULL);

			SyncSelected((PER_CDataCtrl*)_ul_Param1,TRUE);
		}
		bReturn = FALSE;
		break;
	
	case EPER_MESSAGE_DELETE_SELECTED:
		// Make sure frame is active and shown
		//mpo_MyView->SetCurSel(this->mi_NumPane);
		//mpo_MyView->OnChangePane(NULL, NULL);

		DeleteSelected((PER_CDataCtrl*)_ul_Param1);
		bReturn = FALSE;
		break;

	case EPER_MESSAGE_SYNCDELETE_SELECTED:
		SyncDeleteSelected((PER_CDataCtrl*)_ul_Param1);
		bReturn = FALSE;
		break;

	case EPER_MESSAGE_SUBMIT_SELECTED:
		// Make sure frame is active and shown
		//mpo_MyView->SetCurSel(this->mi_NumPane);
		//mpo_MyView->OnChangePane(NULL, NULL);

		SubmitSelected((PER_CDataCtrl*)_ul_Param1);
		bReturn = FALSE;
		break;

	case EPER_MESSAGE_SUBMITEDIT_SELECTED:
		SubmitEditSelected((PER_CDataCtrl*)_ul_Param1);
		bReturn = FALSE;
		break;

	case EPER_MESSAGE_DIFF_SELECTED:
		DiffSelected((PER_CDataCtrl*)_ul_Param1);
		bReturn = FALSE;
		break;

	 case EDI_MESSAGE_ENDDRAGDROP:
	 {
		EDI_tdst_DragDrop* pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;

		// this is only for the browser
		if	(	pst_DragDrop->i_Type == EDI_DD_File 	) 
		{
			UINT uFlags;
			POINT pt = pst_DragDrop->o_Pt;

			mpo_TreeView->ScreenToClient(&pt);
			HTREEITEM hItem = mpo_TreeView->HitTest(pt,&uFlags);

			// if we have hit and its a changelist
			if ( (hItem != NULL) && (TVHT_ONITEM & uFlags) && 
				mpo_TreeView->GetParentItem(hItem) == NULL ) 
			{
				ULONG ulSelectedChangelist = mpo_TreeView->GetItemData(hItem);
				DAT_CPerforce::GetInstance()->SetWorkingChangelist(ulSelectedChangelist);
			}

			EBRO_cl_Frame* pBrowser = (EBRO_cl_Frame*)pst_DragDrop->po_SourceEditor;
			switch (pst_DragDrop->i_Param3)
			{
				case 1: /* Folders */ 
				{
					EditSelected( dynamic_cast<PER_CDataCtrl*>(pBrowser->mpo_TreeCtrl) );
				}
				break;

				case 2: /* Files  */ 
				{
					EditSelected(dynamic_cast<PER_CDataCtrl*>(pBrowser->mpo_ListCtrl));
				}
				break;

				case 3: /* Files in files */ 
				{
					EditSelected(dynamic_cast<PER_CDataCtrl*>(pBrowser->mpo_GrpCtrl));
				}
				break;

			}
			bReturn = TRUE;
		}
	 }
	 break;

	 case EPER_MESSAGE_FLUSH_SELECTED:
		 FlushFiles(*(std::vector<ULONG >* )_ul_Param1, (ULONG )_ul_Param2);
		 bReturn = FALSE;
		 break;

	 case EPER_MESSAGE_FLUSH_DIR:
		 FlushDir((BIG_INDEX )_ul_Param1, (ULONG )_ul_Param2);
		 bReturn = FALSE;
		 break;

	 case EPER_MESSAGE_SERVER_SYNC_SELECTED:
		 if ( IsWorldCurrentlyOpened() )
		 {
			 // Make sure frame is active and shown
			 mpo_MyView->SetCurSel(this->mi_NumPane);
			 mpo_MyView->OnChangePane(NULL, NULL);

			ServerSync((PER_CDataCtrl*)_ul_Param1);
		 }
		 bReturn = FALSE;
		 break;
	}
	
    return bReturn;
}


#endif /* ACTIVE_EDITORS */
