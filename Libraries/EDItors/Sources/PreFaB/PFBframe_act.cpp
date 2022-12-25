/*$T PFBframe_act.cpp GC! 1.081 03/09/04 11:20:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "Res/Res.h"
#include "EDImainframe.h"
#include "PFBframe.h"
#include "PFBframe_act.h"
#include "PFBview.h"
#include "PFBtreeview.h"

#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDIeditors_infos.h"


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    EXTERNAL.
 -----------------------------------------------------------------------------------------------------------------------
 */

extern BOOL						EDI_gb_CheckInDel;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    interface Fonction
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::OnAction(ULONG _ul_Action)
{
	switch(_ul_Action)
	{

	/*$1- FILE menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EPFB_ACTION_REFRESHBROWSER:
		mpo_TreeView->Refresh();
		break;
	case EPFB_ACTION_SAVEPREFAB:
		Prefab_ul_Save(mpst_Prefab);
		break;
	case EPFB_ACTION_AUTOSAVE:
		mi_PrefabSaveAuto = (mi_PrefabSaveAuto) ? 0 : 1;
		break;
	case EPFB_ACTION_MOVEPREFABTOPREFABMAP:
		mst_Ini.i_MoveToPrefabMap = (mst_Ini.i_MoveToPrefabMap) ? 0 : 1;
		break;

	/*$1- DISPLAY menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EPFB_ACTION_DISPLAYPOINT:
		mst_Ini.i_DisplayPoint = mst_Ini.i_DisplayPoint ? 0 : 1;
		break;
	case EPFB_ACTION_DISPLAYBV:
		mst_Ini.i_DisplayBV = mst_Ini.i_DisplayBV ? 0 : 1;
		break;

	/*$1- DATA CONTROL menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EPFB_ACTION_CHECKOUT:
		CommonVss(2);
		break;
	case EPFB_ACTION_LOCALCHECKOUT:
		CommonVss(6);
		break;
	case EPFB_ACTION_UNDOCHECKOUT:
		CommonVss(3);
		break;
	case EPFB_ACTION_GETLVERSION:
		CommonVss(4);
		break;
	case EPFB_ACTION_CHECKIN:
		EDI_gb_CheckInDel = FALSE;
		CommonVss(1);
		break;
	case EPFB_ACTION_CHECKINNOOUT:
		EDI_gb_CheckInDel = FALSE;
		CommonVss(5);
		break;
	case EPFB_ACTION_CHECKINMIRROR:
		EDI_gb_CheckInDel = TRUE;
		CommonVss(1);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPFB_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	
	EBRO_cl_Frame	*po_Browser;
	switch(_ul_Action)
	{
	/*$1- FILE menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EPFB_ACTION_SAVEPREFAB:
		return(mpst_Prefab != NULL);
		break;

	/*$1- DATA CONTROL menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		
	case EPFB_ACTION_LOCALCHECKOUT:
		return (UpdateSelType() == EPFB_SelType_TreeDir);
	
	case EPFB_ACTION_CHECKIN:
	case EPFB_ACTION_CHECKINNOOUT:
	case EPFB_ACTION_UNDOCHECKOUT:
	case EPFB_ACTION_CHECKOUT:
	case EPFB_ACTION_GETLVERSION:
		po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
		if( !po_Browser || po_Browser->mst_Ini.b_EngineMode || !M_MF()->mst_Ini.b_LinkControlON ) return FALSE;
		return (UpdateSelType() != EPFB_SelType_Nothing);
	
	case EPFB_ACTION_CHECKINMIRROR:
				po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
		if( !po_Browser || po_Browser->mst_Ini.b_EngineMode || !M_MF()->mst_Ini.b_LinkControlON ) return FALSE;
		return (UpdateSelType() == EPFB_SelType_TreeDir);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EPFB_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~*/
	UINT	ui_State;
	/*~~~~~~~~~~~~~*/

	ui_State = (UINT) - 1;

	switch(_ul_Action)
	{
	case EPFB_ACTION_AUTOSAVE:
		ui_State = DFCS_BUTTONCHECK | (mi_PrefabSaveAuto ? DFCS_CHECKED : 0);
		break;
	case EPFB_ACTION_MOVEPREFABTOPREFABMAP:
		ui_State = DFCS_BUTTONCHECK | (mst_Ini.i_MoveToPrefabMap ? DFCS_CHECKED : 0);
		break;
	case EPFB_ACTION_DISPLAYPOINT:
		ui_State = DFCS_BUTTONCHECK | (mst_Ini.i_DisplayPoint ? DFCS_CHECKED : 0);
		break;
	case EPFB_ACTION_DISPLAYBV:
		ui_State = DFCS_BUTTONCHECK | (mst_Ini.i_DisplayBV ? DFCS_CHECKED : 0);
		break;
	}

	return ui_State;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPFB_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	if(mpo_DataView->GetFocus() != mpo_DataView->GetDlgItem(IDC_EDIT_COMMENT)) return FALSE;
	return M_MF()->b_EditKey(_uw_Key);
}

#endif
