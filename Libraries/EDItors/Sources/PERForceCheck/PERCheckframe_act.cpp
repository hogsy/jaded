//------------------------------------------------------------------------------
// Filename   :PERCheckframe_act.cpp
/// \author    Nbeaufils
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
#include "PERForce/PERmsg.h"
#include "EDImainframe.h"
#include "LINKs/LINKtoed.h"
#include "BIGfiles/BIGfat.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDItors/Sources/BROwser/BROstrings.h"
#include "EDItors/Sources/BROwser/BROerrid.h"
#include "EDImsg.h"
#include "EDIeditors_infos.h"

#include "PERCheckframe.h"
#include "PERCheckframe_act.h"

#include "PERCheck.h"
#include "PERCheckTopView.h"
#include "PERCheckBottomView.h"

#include "EDItors/Sources/PERForce/PERCDataTreeCtrl.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------


//------------------------------------------------------------
//   void EPERCheck_cl_Frame::OnAction(ULONG _ul_Action)
/// \author    Nbeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::OnAction(ULONG _ul_Action)
{
	HTREEITEM hItem = NULL;
	BIG_INDEX DirIndex = BIG_C_InvalidIndex;

	switch ( _ul_Action )
	{
		case EPERCHECK_ACTION_REFRESH_BRANCH:
			M_MF()->BeginWaitCursor();
			hItem = GetTopLeftWindow()->GetTree()->GetSelectedItem();
			DirIndex = GetTopLeftWindow()->GetTree()->GetItemData(hItem);
			RefreshDiff(DirIndex);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_EXPAND_BRANCH:
			M_MF()->BeginWaitCursor();
			hItem = GetTopLeftWindow()->GetTree()->GetSelectedItem();
			DirIndex = GetTopLeftWindow()->GetTree()->GetItemData(hItem);
			ExpandDiff(DirIndex, FALSE);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_REFRESH_NOTINP4:
		case EPERCHECK_ACTION_EXPAND_NOTINP4:		
			M_MF()->BeginWaitCursor();
			ExpandNotInP4();
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_REFRESH_NOTINBF:
		case EPERCHECK_ACTION_EXPAND_NOTINBF:
			M_MF()->BeginWaitCursor();
			ExpandNotInBF();
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_REFRESH_DELETED:
		case EPERCHECK_ACTION_EXPAND_DELETED:
			M_MF()->BeginWaitCursor();
			ExpandDeleted();
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_EDITTOP:
			M_MF()->BeginWaitCursor();
			GetTopLeftWindow()->GetTree()->InitializeFileItemVisisble();
			LINK_SendMessageToEditors(EPER_MESSAGE_EDIT_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetTopLeftWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_SYNCTOP:
			M_MF()->BeginWaitCursor();
			GetTopLeftWindow()->GetTree()->InitializeFileItemVisisble();
			LINK_SendMessageToEditors(EPER_MESSAGE_SYNC_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetTopLeftWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_FORCESYNCTOP:
			M_MF()->BeginWaitCursor();
			GetTopLeftWindow()->GetTree()->InitializeFileItemVisisble();
			LINK_SendMessageToEditors(EPER_MESSAGE_FORCESYNC_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetTopLeftWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_ADDTOP:
			M_MF()->BeginWaitCursor();
			GetTopLeftWindow()->GetTree()->InitializeFileItemVisisble();
			LINK_SendMessageToEditors(EPER_MESSAGE_ADD_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetTopLeftWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_DELETETOP:
			M_MF()->BeginWaitCursor();
			GetTopLeftWindow()->GetTree()->InitializeFileItemVisisble();
			LINK_SendMessageToEditors(EPER_MESSAGE_DELETE_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetTopLeftWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_DIFFTOP:
			M_MF()->BeginWaitCursor();
			GetTopLeftWindow()->GetTree()->InitializeFileItemVisisble();
			LINK_SendMessageToEditors(EPER_MESSAGE_DIFF_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetTopLeftWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_HISTORYTOP:
			M_MF()->BeginWaitCursor();
			GetTopLeftWindow()->GetTree()->InitializeFileItemVisisble();
			LINK_SendMessageToEditors(EPER_MESSAGE_SHOWHISTORY_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetTopLeftWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_SUBMITTOP:
			M_MF()->BeginWaitCursor();
			GetTopLeftWindow()->GetTree()->InitializeFileItemVisisble();
			LINK_SendMessageToEditors(EPER_MESSAGE_SUBMIT_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetTopLeftWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_SUBMITEDITTOP:
			M_MF()->BeginWaitCursor();
			GetTopLeftWindow()->GetTree()->InitializeFileItemVisisble();
			LINK_SendMessageToEditors(EPER_MESSAGE_SUBMITEDIT_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetTopLeftWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_REVERTTOP:
			M_MF()->BeginWaitCursor();
			GetTopLeftWindow()->GetTree()->InitializeFileItemVisisble();
			LINK_SendMessageToEditors(EPER_MESSAGE_REVERT_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetTopLeftWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_SYNCBOTTOM:
			M_MF()->BeginWaitCursor();
			LINK_SendMessageToEditors(EPER_MESSAGE_SYNC_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetBottomWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_ADDBOTTOM:
			M_MF()->BeginWaitCursor();
			LINK_SendMessageToEditors(EPER_MESSAGE_ADD_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetBottomWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
		case EPERCHECK_ACTION_PERFORCE_DELETEBOTTOM:
			M_MF()->BeginWaitCursor();
			LINK_SendMessageToEditors(EPER_MESSAGE_SYNCDELETE_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(GetBottomWindow()->GetTree()))),0);
			M_MF()->EndWaitCursor();
			break;
	}
}

/*
===================================================================================================
===================================================================================================
*/

//------------------------------------------------------------
//   BOOL EPERCHECK_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL EPERCheck_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	switch (_ul_Action)
	{
		case EPERCHECK_ACTION_REFRESH_BRANCH:
		case EPERCHECK_ACTION_EXPAND_BRANCH:
		case EPERCHECK_ACTION_REFRESH_NOTINP4:
		case EPERCHECK_ACTION_EXPAND_NOTINP4:
		case EPERCHECK_ACTION_REFRESH_NOTINBF:
		case EPERCHECK_ACTION_EXPAND_NOTINBF:
		case EPERCHECK_ACTION_REFRESH_DELETED:
		case EPERCHECK_ACTION_EXPAND_DELETED:
		case EPERCHECK_ACTION_PERFORCE_SUBMITTOP:
		case EPERCHECK_ACTION_PERFORCE_SUBMITEDITTOP:
		case EPERCHECK_ACTION_PERFORCE_REVERTTOP:
		case EPERCHECK_ACTION_PERFORCE_EDITTOP:
		case EPERCHECK_ACTION_PERFORCE_SYNCTOP:
		case EPERCHECK_ACTION_PERFORCE_FORCESYNCTOP:
		case EPERCHECK_ACTION_PERFORCE_ADDTOP:
		case EPERCHECK_ACTION_PERFORCE_DELETETOP:
		case EPERCHECK_ACTION_PERFORCE_DIFFTOP:
		case EPERCHECK_ACTION_PERFORCE_HISTORYTOP:
		case EPERCHECK_ACTION_PERFORCE_SYNCBOTTOM:
		case EPERCHECK_ACTION_PERFORCE_ADDBOTTOM:
		case EPERCHECK_ACTION_PERFORCE_DELETEBOTTOM:
				return TRUE;
			break;
	}
	return FALSE;
}

//------------------------------------------------------------
//   BOOL EPERCheck_cl_Frame::b_KnowsKey(USHORT _uw_Key)
/// \author    NBeaufils
/// \date      23-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
BOOL EPERCheck_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	switch(_uw_Key)
	{
	case VK_END:
	case VK_HOME:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
	case VK_NUMPAD1:
	case VK_NUMPAD2:
	case VK_NUMPAD3:
	case VK_NUMPAD4:
	case VK_NUMPAD6:
	case VK_NUMPAD7:
	case VK_NUMPAD8:
	case VK_NUMPAD9:
		return TRUE;
	}

	return FALSE;
}

/*
===================================================================================================
===================================================================================================
*/

//------------------------------------------------------------
//   UINT EPERCheck_cl_Frame::ui_OnActionState(ULONG _ul_Action)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
UINT EPERCheck_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UINT    ui_State;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ui_State = (UINT) - 1;

	return ui_State;
}

/*
===================================================================================================
===================================================================================================
*/

//------------------------------------------------------------
//   UINT EPERCheck_cl_Frame::RefreshDiff(BIG_INDEX DirIndex)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::RefreshDiff(BIG_INDEX DirIndex)
{
	GetTopLeftWindow()->ExpandTreeCtrl(DirIndex, TRUE);	
}

//------------------------------------------------------------
//   UINT EPERCheck_cl_Frame::ExpandDiff(BIG_INDEX DirIndex, BOOL bClear)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::ExpandDiff(BIG_INDEX DirIndex, BOOL bClear)
{
	if ( bClear )
	{
		EPERCheckTop_cl_View::m_mapFileToTreeReference.clear();
		EPERCheckTop_cl_View::m_mapFolderToTreeReference.clear();

		GetTopLeftWindow()->GetTree()->DeleteAllItems();
		GetTopRightWindow()->GetTree()->DeleteAllItems();

		EPERCheck_cl_Manager::GetInstance()->ClearDiffStruct();
		EPERCheck_cl_Manager::GetInstance()->FillDiffStruct(DirIndex, FALSE);
		EPERCheck_cl_Manager::GetInstance()->SortDiffStruct();

		// Did the user press escape ?
		if ( LINK_gb_EscapeDetected )
			return;

		GetTopLeftWindow()->ExpandTreeCtrl(DirIndex, FALSE);
	}
	else
	{
		// Remove structure holding tree data
		GetTopLeftWindow()->RemoveFolderTreeCtrl(DirIndex);

		HTREEITEM hFolderItemClient = EPERCheckTop_cl_View::m_mapFolderToTreeReference[DirIndex].m_TreeItemClient;
		HTREEITEM hFolderItemServer = EPERCheckTop_cl_View::m_mapFolderToTreeReference[DirIndex].m_TreeItemServer;

		// Remove structure holding diff data
		std::vector<BIG_INDEX> vDirIndex;
		GetTopLeftWindow()->GetTree()->GetDirIndexVisible(hFolderItemClient, vDirIndex);
		EPERCheck_cl_Manager::GetInstance()->RemoveDiffStruct(vDirIndex);

		// Collapse both trees
		GetTopLeftWindow()->GetTree()->DeleteSubItemsData(hFolderItemClient);
		GetTopLeftWindow()->GetTree()->Expand(hFolderItemClient, TVE_COLLAPSERESET|TVE_COLLAPSE);

		GetTopLeftWindow()->GetTwinTree()->DeleteSubItemsData(hFolderItemServer);
		GetTopLeftWindow()->GetTwinTree()->Expand(hFolderItemServer, TVE_COLLAPSERESET|TVE_COLLAPSE);

		EPERCheck_cl_Manager::GetInstance()->FillDiffStruct(DirIndex, TRUE);
		EPERCheck_cl_Manager::GetInstance()->SortDiffStruct();

		// Did the user press escape ?
		if ( LINK_gb_EscapeDetected )
			return;

		GetTopLeftWindow()->ExpandTreeCtrl(DirIndex, TRUE);
	}
}

//------------------------------------------------------------
//   UINT EPERCheck_cl_Frame::RefreshNotInBF()
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::RefreshNotInBF()
{
	std::vector<std::string> vP4Files;
	std::vector<DAT_CP4ClientInfoHeader*> vFileInfo;

	GetBottomWindow()->ClearNotInBFTreeCtrl();
	GetBottomWindow()->FillNotInBFTreeCtrl(vFileInfo);

	// Clean up
	for ( UINT index = 0 ; index < vFileInfo.size() ; index++ )
		delete vFileInfo[index];
}

//------------------------------------------------------------
//   UINT EPERCheck_cl_Frame::ExpandNotInBF()
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::ExpandNotInBF()
{
	GetBottomWindow()->ExpandNotInBFTreeCtrl();
}

//------------------------------------------------------------
//   UINT EPERCheck_cl_Frame::RefreshNotInP4()
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::RefreshNotInP4()
{
	GetBottomWindow()->ClearNotInP4TreeCtrl();
	GetBottomWindow()->FillNotInP4TreeCtrl();
}

//------------------------------------------------------------
//   UINT EPERCheck_cl_Frame::ExpandNotInP4()
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::ExpandNotInP4()
{
	RefreshNotInP4();
	GetBottomWindow()->ExpandNotInP4TreeCtrl();
}

//------------------------------------------------------------
//   UINT EPERCheck_cl_Frame::RefreshDeleted()
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::RefreshDeleted()
{
	std::vector<std::string> vP4Files;
	std::vector<DAT_CP4ClientInfoHeader*> vFileInfo;

	// - GET DELETED FILES IN BF ------------------
	BIG_INDEX	ulIndex, ulNextIndex;
	BAS_tdst_barray arrayDeletedFiles;

	BAS_binit(&arrayDeletedFiles, 200);

	ulIndex = BIG_gst.st_ToSave.ul_FirstFreeFile;
	while(ulIndex != BIG_C_InvalidIndex)
	{
		ulNextIndex = BAS_bsearch(ulIndex, &arrayDeletedFiles);
		if(ulNextIndex != -1)
		{
			assert(FALSE);
			break;
		}

		BAS_binsert(ulIndex, ulIndex, &arrayDeletedFiles);

		// Key is not present for the index meaning that it has been deleted locally in the BF
		// NOTE: To keep track of the key, we stored it in the P4RevisionClient
		std::string strP4File;
		DAT_CUtils::GetP4FileFromKey(BIG_P4RevisionClient(ulIndex), strP4File, DAT_CPerforce::GetInstance()->GetP4Root());
		vP4Files.push_back(strP4File);

		ulIndex = BIG_NextFile(ulIndex);
	}
	BAS_bfree(&arrayDeletedFiles);
	// --------------------------------------------

	GetBottomWindow()->ClearDeletedTreeCtrl();
	GetBottomWindow()->FillDeletedTreeCtrl(vFileInfo);

	// Clean up
	for ( UINT index = 0 ; index < vFileInfo.size() ; index++ )
		delete vFileInfo[index];
}

//------------------------------------------------------------
//   UINT EPERCheck_cl_Frame::ExpandDeleted()
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Frame::ExpandDeleted()
{
	RefreshDeleted();
	GetBottomWindow()->ExpandDeletedTreeCtrl();
}

#endif

