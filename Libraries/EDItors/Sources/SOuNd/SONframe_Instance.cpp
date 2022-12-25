/*$T SONframe_Instance.cpp GC! 1.081 02/20/03 13:56:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "DIAlogs/DIAsndinstance_dlg.h"
#include "Res/Res.h"
#include "BROwser/BROframe.h"
#include "EDImsg.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAdefs.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/sources/SNDconst.h"
#include "SouND/sources/SNDstruct.h"
#include "SouND/sources/SND.h"
#include "SouND/sources/SNDinsert.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDvolume.h"

/*$2- sound editor ---------------------------------------------------------------------------------------------------*/

#include "SONframe.h"
#include "SONview.h"
#include "SONutil.h"

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_Spy(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	mst_Ini.i_Option |= ESON_Cte_EnableDebugLog;
	ESON_SetSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_Set(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i_sel, i_instance;
	CListCtrl					*po_LC;
	SND_tdst_SoundInstance		*pst_SI;
	EDIA_cl_SndInstanceDialog	*po_Dialog;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	po_Dialog = new EDIA_cl_SndInstanceDialog(i_instance);
	po_Dialog->DoModeless();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_OnEditSound(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);
	Sound_Set(BIG_ul_SearchKeyToFat(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_OnFindSound(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	EBRO_cl_Frame			*po_Browser;
	ULONG					ul_Fat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	if(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey != -1)
	{
		ul_Fat = BIG_ul_SearchKeyToFat(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey);

		if(ul_Fat == BIG_C_InvalidIndex) return;

		po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
		po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
		po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ul_Fat), ul_Fat);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_OnEditSmd(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	if(pst_SI->ul_SmdKey != -1)
	{
		SModifier_Set(BIG_ul_SearchKeyToFat(pst_SI->ul_SmdKey));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_OnFindSmd(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	EBRO_cl_Frame			*po_Browser;
	ULONG					ul_Fat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	if(pst_SI->ul_SmdKey != -1)
	{
		ul_Fat = BIG_ul_SearchKeyToFat(pst_SI->ul_SmdKey);

		if(ul_Fat == BIG_C_InvalidIndex) return;

		po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
		po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
		po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ul_Fat), ul_Fat);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_OnFindGao(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	EBRO_cl_Frame			*po_Browser;
	ULONG					ul_Fat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	ul_Fat = LOA_ul_SearchIndexWithAddress((ULONG) pst_SI->p_GameObject);
	if(ul_Fat == BIG_C_InvalidIndex) return;

	po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
	po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
	po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ul_Fat), ul_Fat);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_OnSetSoloGao(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	ESON_SetSoloGao((OBJ_tdst_GameObject *) pst_SI->p_GameObject);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_OnSetMuteGao(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	ESON_SetMuteGao((OBJ_tdst_GameObject *) pst_SI->p_GameObject);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_OnSetSolo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	ESON_SetSoloInstance((ULONG) i_instance);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::Instance_OnSetMute(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	ESON_SetMuteInstance((ULONG) i_instance);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_Frame::Instance_b_GaoIsSolo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return FALSE;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return FALSE;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, FALSE);

	return ESON_b_GaoIsSolo((OBJ_tdst_GameObject *) pst_SI->p_GameObject);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_Frame::Instance_b_GaoIsMute(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return FALSE;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return FALSE;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, FALSE);

	return ESON_b_GaoIsMuted((OBJ_tdst_GameObject *) pst_SI->p_GameObject);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_Frame::Instance_b_IsSolo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return FALSE;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return FALSE;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, FALSE);

	return ESON_b_InstanceIsSolo((ULONG) i_instance);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_Frame::Instance_b_IsMute(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return FALSE;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return FALSE;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, FALSE);

	return ESON_b_InstanceIsMuted((ULONG) i_instance);
}

#ifdef JADEFUSION
extern void AI_AddWatch(void *p, int s);
#else
extern "C" void AI_AddWatch(void *p, int s);
#endif

void ESON_cl_Frame::Instance_AddWatch(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_sel, i_instance;
	CListCtrl				*po_LC;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View) return;
	i_sel = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
	if(i_sel == -1) return;

	po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
	i_instance = po_LC->GetItemData(i_sel);
	SND_M_GetInstanceOrReturn(i_instance, pst_SI, ;);

	AI_AddWatch(pst_SI, sizeof(SND_tdst_SoundInstance));
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
