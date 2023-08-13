/*$T LINKtoed.cpp GC! 1.081 06/19/00 10:38:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EDImainframe.h"
#include "LINKs/LINKtoed.h"
#include "EDImsg.h"
#include "EDIapp.h"
#include "ENGine/Sources/ENGvars.h"
#include "EDItors/Sources/RASters/RASframe.h"

#include "../../Shared/MainSharedSystem.h"

BOOL					LINK_gb_CanRefresh = TRUE;
BOOL					LINK_gb_RefreshEnable = TRUE;
BOOL					LINK_gb_AllRefreshEnable = TRUE;
BOOL					LINK_gb_EditRefresh = FALSE;
int						LINK_gi_SpeedDraw = 0;
BOOL					LINK_gb_RefreshOnlyFiles = FALSE;

extern ERAS_cl_Frame	*gpo_RasterEditor;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_SendMessageToEditors(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
	if ( !jaded::sys::launchOperations.editorMode )
		return;

	if(M_MF()) M_MF()->SendMessageToEditors(_ul_Msg, _ul_Param1, _ul_Param2);
}

/*
 =======================================================================================================================
    Aim:    Call when one engine frame is ending.
 =======================================================================================================================
 */
void LINK_OneTrameEnding(void)
{
	if ( !jaded::sys::launchOperations.editorMode )
		return;

	if(M_MF())
	{
		if(!LINK_gi_SpeedDraw) M_MF()->RefreshCheckDlg();
		M_MF()->OneTrameEnding();
	}
}

/*
 =======================================================================================================================
    Aim:    Call to refresh all engine 3D view.
 =======================================================================================================================
 */
void LINK_Refresh(void)
{
	if ( !jaded::sys::launchOperations.editorMode )
		return;

	if(!LINK_gb_AllRefreshEnable) return;
	if(M_MF() && LINK_gb_RefreshEnable)
	{
		M_MF()->RefreshCheckDlg();
		LINK_gb_EditRefresh = TRUE;
		M_MF()->SendMessageToEditors(EDI_MESSAGE_REFRESH, 0, 0);
		LINK_gb_EditRefresh = FALSE;
	}
}

/*
 =======================================================================================================================
    Aim:    Call to refresh all engine 3D view.
 =======================================================================================================================
 */
void LINK_RefreshMenu(void)
{
	if ( !jaded::sys::launchOperations.editorMode )
		return;

	if(!LINK_gb_AllRefreshEnable) return;
	if(M_MF()) M_MF()->SendMessageToEditors(EDI_MESSAGE_REFRESHMENU, 0, 0);
}

/*
 =======================================================================================================================
    Aim:    Call, when engine is running, if a windows message is received.

    Out:    TRUE to dispatch the message, FALSE else.
 =======================================================================================================================
 */
BOOL LINK_b_ProcessEngineWndMsg(MSG *_p_Msg)
{
	if ( !jaded::sys::launchOperations.editorMode )
		return FALSE;

	if(EDI_go_TheApp.b_ProcessMouseWheel(_p_Msg)) return FALSE;

	switch(_p_Msg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		EDI_go_TheApp.OnIdle(0);
		break;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_FatHasChanged(void)
{
	if ( !jaded::sys::launchOperations.editorMode )
		return;

	if(!LINK_gb_AllRefreshEnable) return;
	if(M_MF()) M_MF()->FatHasChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_UpdatePointers(void)
{
	if ( !jaded::sys::launchOperations.editorMode )
		return;

	if(!LINK_gb_AllRefreshEnable) return;
	if(!EDI_gb_DuringInit) M_MF()->DataHasChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LINK_DisplayRasters(void)
{
	if ( !jaded::sys::launchOperations.editorMode )
		return;

	if(gpo_RasterEditor) gpo_RasterEditor->OneTrameEnding();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL LINK_PreTranslateMessage(MSG *msg)
{
	return EDI_go_TheApp.PreTranslateMessage(msg);
}

#endif
