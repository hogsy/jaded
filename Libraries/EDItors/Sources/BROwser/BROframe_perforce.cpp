/*$T BROframe_vss.cpp GC!1.71 02/08/00 13:59:29 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BROframe.h"
#include "BROtreectrl.h"
#include "BROlistctrl.h"
#include "BROstrings.h"
#include "BROerrid.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDImsg.h"
#include "EDIstrings.h"
#include "EDImainframe.h"
#include "EDIapp.h"
#include "ENGine/Sources/ENGinit.h"
#include "DIAlogs/DIAmsglink_dlg.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGkey.h"

#include "EDItors/Sources/PERForce/PERmsg.h"
#include "DATaControl/DATCPerforce.h"
#include "EDItors/Sources/perForce/PERCframe.h"

#include <assert.h>

//////////////////////////////////////////////////////////////////////////
//




//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceSync()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceSync(BOOL _bForceSync)
{
	ULONG ulMessage = EPER_MESSAGE_SYNC_SELECTED;
	if ( _bForceSync ) 
		ulMessage = EPER_MESSAGE_FORCESYNC_SELECTED ;

	if ( LIST_ACTIVATED() )
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_ListCtrl))), 0);
	}
	else 
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_TreeCtrl))), 0);
	}

    // Update current dir after sync.
    OnTreeCtrlSelChange();
}

//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceServerSync()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceServerSync()
{
	ULONG ulMessage = EPER_MESSAGE_SERVER_SYNC_SELECTED;
	if ( LIST_ACTIVATED() )
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_ListCtrl))), 0);
	}
	else 
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_TreeCtrl))), 0);
	}

}

//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceEdit()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceEdit()
{
	ULONG ulMessage = EPER_MESSAGE_EDIT_SELECTED;
	if ( LIST_ACTIVATED() )
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_ListCtrl))), 0);
	}
	else 
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_TreeCtrl))), 0);
	}
}

//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceDelete()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceDelete()
{
	ULONG ulMessage = EPER_MESSAGE_DELETE_SELECTED;
	if ( LIST_ACTIVATED() )
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_ListCtrl))), 0);
	}
	else 
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_TreeCtrl))), 0);
	}
}

//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceAdd()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceAdd()
{
	ULONG ulMessage = EPER_MESSAGE_ADD_SELECTED;
	if ( LIST_ACTIVATED() )
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_ListCtrl))), 0);
	}
	else 
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_TreeCtrl))), 0);
	}

}

//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceDiff()
/// \author    NBeaufils
/// \date      19-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceDiff()
{
	if ( LIST_ACTIVATED() )
	{
		ULONG ulMessage = EPER_MESSAGE_DIFF_SELECTED;
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_ListCtrl))), 0);
	}
	else
	{
		ULONG ulMessage = EPER_MESSAGE_DIFFDIR_SELECTED;
		HTREEITEM hItem = mpo_TreeCtrl->GetSelectedItem();
		if ( hItem != NULL )
			LINK_SendMessageToEditors(ulMessage, (ULONG)mpo_TreeCtrl->GetItemData(hItem), 0);
	}
}

//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceHistory()
/// \author    FFerland
/// \date      31-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceHistory()
{
	ULONG ulMessage = EPER_MESSAGE_SHOWHISTORY_SELECTED;
	if ( LIST_ACTIVATED() )
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_ListCtrl))), 0);
	}
	else 
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_TreeCtrl))), 0);
	}
}

//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceSubmit()
/// \author    FFerland
/// \date      03-Mar-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceSubmit()
{
	ULONG ulMessage = EPER_MESSAGE_SUBMIT_SELECTED;
	if ( LIST_ACTIVATED() )
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_ListCtrl))), 0);
	}
	else 
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_TreeCtrl))), 0);
	}
}

//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceSubmitEdit()
/// \author    FFerland
/// \date      23-Mar-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceSubmitEdit()
{
	ULONG ulMessage = EPER_MESSAGE_SUBMITEDIT_SELECTED;
	if ( LIST_ACTIVATED() )
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_ListCtrl))), 0);
	}
	else 
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_TreeCtrl))), 0);
	}
}

//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceRevert()
/// \author    FFerland
/// \date      15-Mar-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceRevert()
{
	ULONG ulMessage = EPER_MESSAGE_REVERT_SELECTED;
	if ( LIST_ACTIVATED() )
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_ListCtrl))), 0);
	}
	else 
	{
		LINK_SendMessageToEditors(ulMessage, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(mpo_TreeCtrl))), 0);
	}
}

//------------------------------------------------------------
//   void EBRO_cl_Frame::OnPerforceRefreshListCtrl()
/// \author    NBeaufils
/// \date      19-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EBRO_cl_Frame::OnPerforceRefreshListCtrl(BIG_INDEX _ul_DirIndex)
{
}
#endif /* ACTIVE_EDITORS */
