/*$T EACTframe_act.cpp GC! 1.100 02/05/01 17:04:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EACTframe.h"
#include "EACTview.h"
#include "EDImainframe.h"
#include "Res/Res.h"
#include "ENGine/Sources/ACTions/ACTsave.h"
#include "ENGine/Sources/ACTions/ACTload.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "DIAlogs/DIAname_dlg.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGdefs.h"
#include "LINKs/LINKtoed.h"


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_Frame::OnAction(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static EDIA_cl_NameDialog	o_Dlg("Enter action to find");
	CListCtrl					*plist;
	int							i, j;
	CString						str;
	char						strstr[1024];
	BIG_KEY						key;
	BIG_INDEX					act, ani;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_ul_Action)
	{
	case EACT_ACTION_CLOSEALL:
		OnClose();
		break;

	case EACT_ACTION_SAVEKIT:
		ACT_ul_SaveActionKit(mpst_CurrentActionKit);
		M_MF()->FlashJade();
		break;

	case EACT_ACTION_RENAMEACTION:
		mpo_View->RenameAction();
		break;

	case EACT_ACTION_RESETTRANSITION:
		ResetTransition();
		break;

	case EACT_ACTION_RESTOREINIT:
		ACT_ActionRestoreInit(mpst_CurrentActionKit);
		break;

	case EACT_ACTION_FIND:
		if(o_Dlg.DoModal() == IDOK)
		{
			plist = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_ACTIONS);
			for(i = 0; i < plist->GetItemCount(); i++)
			{
				if(plist->GetItemState(i, LVIS_FOCUSED | LVIS_SELECTED)) break;
			}
			if(i == plist->GetItemCount()) 
				i = 0;
			else
				i++;
			for(; i < plist->GetItemCount(); i++)
			{
				plist->GetItemText(i, 0, strstr, 300);
				str = strstr;
				if(str.Find(o_Dlg.mo_Name) != -1)
				{
					for(j = 0; j < plist->GetItemCount(); j++) plist->SetItemState(j, 0, LVIS_SELECTED);
					plist->SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
					plist->EnsureVisible(i, FALSE);
					break;
				}
			}
		}
		break;

	case EACT_ACTION_DUPLICATEKIT:
		DuplicateKit();
		break;

	case EACT_ACTION_LOGKIT:
		key = LOA_ul_SearchKeyWithAddress((ULONG) mpst_CurrentActionKit);
		act = BIG_ul_SearchKeyToFat(key);
		if(act == BIG_C_InvalidIndex) break;
		LINK_PrintStatusMsg(BIG_NameFile(act));
		LINK_PrintStatusMsg("---------");
		for(i = 0; i < mpst_CurrentActionKit->uw_NumberOfAction; i++)
		{
			if(!mpst_CurrentActionKit->apst_Action[i]) continue;
			key = LOA_ul_SearchKeyWithAddress((ULONG) mpst_CurrentActionKit->apst_Action[i]);
			act = BIG_ul_SearchKeyToFat(key);
			if(act == BIG_C_InvalidIndex) continue;
			LINK_PrintStatusMsgEOL(BIG_NameFile(act));
			LINK_PrintStatusMsgEOL("\t\t\t\t\t\t");
			for(j = 0; j < mpst_CurrentActionKit->apst_Action[i]->uc_NumberOfActionItem; j++)
			{
				key = LOA_ul_SearchKeyWithAddress((ULONG) mpst_CurrentActionKit->apst_Action[i]->ast_ActionItem[j].pst_TrackList);
				ani = BIG_ul_SearchKeyToFat(key);
				if(ani == BIG_C_InvalidIndex) continue;
				LINK_PrintStatusMsgEOL(BIG_NameFile(ani));
				LINK_PrintStatusMsgEOL(", ");
			}

			LINK_PrintStatusMsgEOL("\\n");
		}
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EACT_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	return (UINT) -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EACT_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	/*~~~~~~~~~~~~~~~*/
	CListCtrl	*plist;
	int			iSel;
	/*~~~~~~~~~~~~~~~*/

	switch(_ul_Action)
	{
	case EACT_ACTION_CLOSEALL:
	case EACT_ACTION_LOGKIT:
	case EACT_ACTION_SAVEKIT:
	case EACT_ACTION_RESETTRANSITION:
	case EACT_ACTION_DUPLICATEKIT:
		if(mpst_CurrentActionKit == NULL) return FALSE;
		break;

	case EACT_ACTION_RENAMEACTION:
		if(mpst_CurrentActionKit == NULL) return FALSE;
		plist = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_ACTIONS);
		iSel = plist->GetNextItem(-1, LVNI_SELECTED);
		if(iSel == -1) return FALSE;
		break;

	case EACT_ACTION_RESTOREINIT:
		if(mpst_CurrentActionKit == NULL) return FALSE;
		if(mpst_CurrentActionKit->ppst_ActionInits == NULL) return FALSE;
		break;
	}

	return TRUE;
}

#endif
