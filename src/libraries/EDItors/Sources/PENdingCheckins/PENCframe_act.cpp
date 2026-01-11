/*$T PENCframe_act.cpp GC 1.138 03/11/05 09:02:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "Res/Res.h"
#include "EDImainframe.h"
#include "PENcframe.h"
#include "LINKs/LINKtoed.h"
#include "BIGfiles/BIGfat.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDItors/Sources/BROwser/BROstrings.h"
#include "EDItors/Sources/BROwser/BROerrid.h"
#include "EDImsg.h"
#include "EDIeditors_infos.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern BOOL EDI_gb_NoUpdateVSS;
extern BOOL EDI_gb_CheckInDel;
extern BOOL EDI_can_lock;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::OnAction(ULONG _ul_Action)
{
	switch(_ul_Action)
	{
	case EPENC_ACTION_REFRESH:					Refresh(); break;
	case EPENC_ACTION_DATACONTROL:				M_MF()->OnLinkControl(); break;
	case EPENC_ACTION_SHOWINBROWSER:			ShowInBrowerSel(); break;
	case EPENC_ACTION_UNDOCHECKOUT_SELECTED:	UndoCheckOutSel(); break;
	case EPENC_ACTION_CHECKIN_SELECTED:			CheckInSel(); break;
	case EPENC_ACTION_CHECKINOUT_SELECTED:		CheckInOutSel(); break;
	case EPENC_ACTION_UNDOCHECKOUT_ALL:			UndoCheckOutAll(); break;
	case EPENC_ACTION_CHECKIN_ALL:				CheckInAll(); break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPEN_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	switch(_ul_Action)
	{
	case EPENC_ACTION_DATACONTROL:
		return TRUE;

	case EPENC_ACTION_REFRESH:
		if(!M_MF()->mst_Ini.b_LinkControlON) return FALSE;
		return TRUE;

	case EPENC_ACTION_UNDOCHECKOUT_SELECTED:
	case EPENC_ACTION_CHECKIN_SELECTED:
	case EPENC_ACTION_CHECKINOUT_SELECTED:
	case EPENC_ACTION_SHOWINBROWSER:
		if(!M_MF()->mst_Ini.b_LinkControlON) return FALSE;
		if(mpo_TreeView->GetSelectedItem()) return TRUE;
		return FALSE;

	case EPENC_ACTION_UNDOCHECKOUT_ALL:
	case EPENC_ACTION_CHECKIN_ALL:
		if(!M_MF()->mst_Ini.b_LinkControlON) return FALSE;
		if(mpo_TreeView->GetCount()) return TRUE;
		return FALSE;

	default:
		if(!M_MF()->mst_Ini.b_LinkControlON) return FALSE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EPEN_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~*/
	UINT	ui_State;
	/*~~~~~~~~~~~~~*/

	ui_State = (UINT) - 1;

	return ui_State;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::OnToolBarCommand(UINT nID)
{
	switch(nID)
	{
	case EPEN_ID_DC:		OnAction(EPENC_ACTION_DATACONTROL); break;
	case EPEN_ID_REFRESH:	OnAction(EPENC_ACTION_REFRESH); break;
	case EPEN_ID_UNDOCO:	OnAction(EPENC_ACTION_UNDOCHECKOUT_SELECTED); break;
	case EPEN_ID_CI:		OnAction(EPENC_ACTION_CHECKIN_SELECTED); break;
	case EPEN_ID_UNDOCOALL: OnAction(EPENC_ACTION_UNDOCHECKOUT_ALL); break;
	case EPEN_ID_CIALL:		OnAction(EPENC_ACTION_CHECKIN_ALL); break;
	case EPEN_ID_SHOW:		OnAction(EPENC_ACTION_SHOWINBROWSER); break;
	default:				break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::OnToolBarCommandUI(UINT nID, CCmdUI *pUI)
{
	switch(nID)
	{
	case EPEN_ID_DC:
		pUI->Enable(TRUE);
		if(M_MF()->mst_Ini.b_LinkControlON)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EPEN_ID_REFRESH:
		if(M_MF()->mst_Ini.b_LinkControlON)
			pUI->Enable(TRUE);
		else
			pUI->Enable(FALSE);
		break;

	case EPEN_ID_UNDOCO:
	case EPEN_ID_CI:
	case EPEN_ID_SHOW:
		if(M_MF()->mst_Ini.b_LinkControlON && mpo_TreeView->GetSelectedItem())
			pUI->Enable(TRUE);
		else
			pUI->Enable(FALSE);
		break;

	case EPEN_ID_UNDOCOALL:
	case EPEN_ID_CIALL:
		if(M_MF()->mst_Ini.b_LinkControlON && mpo_TreeView->GetCount())
			pUI->Enable(TRUE);
		else
			pUI->Enable(FALSE);
		break;

	default:
		pUI->SetCheck(0);
		pUI->Enable(FALSE);
		break;
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#endif
