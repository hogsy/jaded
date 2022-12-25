/*$T EACTframe.cpp GC 1.134 10/18/04 11:18:13 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#define ACTION_GLOBAL
#include "LINKs/LINKmsg.h"
#include "EDImainframe.h"
#include "EACTframe.h"
#include "EACTframe_act.h"
#include "EACTview.h"
#include "EDImsg.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "Res/Res.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/LOading/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"

#ifdef JADEFUSION
extern int  EDI_gi_GenSpe;
#else
extern "C" int  EDI_gi_GenSpe;
#endif
extern int		EDI_gai_GenSpe[512];
extern BOOL		EDI_gb_SlashLLoaded;

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EACT_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EACT_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EACT_cl_Frame::EACT_cl_Frame(void)
{
	mpo_View = NULL;
	mul_CurrentActionKit = BIG_C_InvalidIndex;
	mpst_CurrentActionKit = NULL;
	mpst_CurrentAction = NULL;
	mpst_CurrentActionItem = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EACT_cl_Frame::~EACT_cl_Frame(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EACT_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	EDI_cl_BaseFrame::OnCreate(lpcs);

	/* View */
	mpo_View = new EACT_cl_View;
	mpo_View->Create("Edit", "Edit", 0, CRect(0, 0, 0, 0), this, 1);
	mpo_View->mpo_Editor = this;
	mpo_View->ShowWindow(SW_HIDE);

	/* Transitions */
	mpo_View->EnableTrans();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);

	/* Move view */
	if(mpo_View) mpo_View->MoveWindow(CRect(0, 0, cx, cy));
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_Frame::OnClose(void)
{
	mpst_CurrentActionKit = NULL;
	mpst_CurrentAction = NULL;
	mpst_CurrentActionItem = NULL;
	mul_CurrentActionKit = BIG_C_InvalidIndex;
	mpo_View->CloseAll();
	RefreshMenu();
	mpo_View->ShowWindow(SW_HIDE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_Frame::LoadActionKit(BIG_INDEX _ul_File)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	M_MF()->LockDisplay(this);
	OnClose();

	mul_CurrentActionKit = _ul_File;
	mpst_CurrentActionKit = (ACT_st_ActionKit *) LOA_ul_SearchAddress(BIG_PosFile(_ul_File));
	if(!mpst_CurrentActionKit || ((int) mpst_CurrentActionKit == -1))
	{
		mul_CurrentActionKit = BIG_C_InvalidIndex;
		mpst_CurrentActionKit = NULL;
		return;
	}

	GetClientRect(&o_Rect);
	o_Rect.left++;
	mpo_View->MoveWindow(&o_Rect);
	o_Rect.left--;
	mpo_View->MoveWindow(&o_Rect);

	mpo_View->SetActionKit();
	mpo_View->ShowWindow(SW_SHOW);
	RefreshMenu();

	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_Frame::LoadAnim(BIG_INDEX _ul_File)
{
	mpo_View->SetAnim(_ul_File);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EACT_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	if(!mpst_CurrentActionKit) return FALSE;
	if(M_MF()->b_EditKey(_uw_Key)) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_Frame::ResetTransition(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ACT_st_Action		*pst_Action;
	ACT_st_ActionItem	*pst_ActionItem;
	ACT_tdst_Transition *pst_Transition;
	int					i_Action, i_ActionItem, i_Transition;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_CurrentActionKit) return;

	for(i_Action = 0; i_Action < mpst_CurrentActionKit->uw_NumberOfAction; i_Action++)
	{
		pst_Action = mpst_CurrentActionKit->apst_Action[i_Action];
		if(!pst_Action) continue;

		for(i_ActionItem = 0; i_ActionItem < pst_Action->uc_NumberOfActionItem; i_ActionItem++)
		{
			pst_ActionItem = pst_Action->ast_ActionItem + i_ActionItem;
			if(!pst_ActionItem) continue;
			if(!pst_ActionItem->pst_Transitions) continue;
			for(i_Transition = 0; i_Transition < pst_ActionItem->pst_Transitions->num; i_Transition++)
			{
				pst_Transition = (ACT_tdst_Transition *) pst_ActionItem->pst_Transitions->base[i_Transition].ul_Val;
				if(!pst_Transition) continue;
				pst_Transition->uc_Blend = pst_Transition->uc_Flag = 0;
			}
		}
	}

	mpo_View->SetActionKit();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_Frame::OnRealIdle(void)
{
	/*~~~~~~~~~~~~~~~*/
	CListCtrl	*plist;
	/*~~~~~~~~~~~~~~~*/

	if(EDI_gi_GenSpe)
	{
		if(!EDI_gb_SlashLLoaded) return;
		LOA_SaveSpecialArray();
		ExitProcess(0);
		return;
	}

	plist = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_ACTIONS);
	if(!mpst_CurrentActionKit) return;
	if(mpst_CurrentActionKit->ppst_ActionInits)
	{
		if(plist->GetBkColor() != GetSysColor(COLOR_3DSHADOW))
		{
			plist->SetBkColor(GetSysColor(COLOR_3DSHADOW));
			plist->SetTextBkColor(GetSysColor(COLOR_3DSHADOW));
			plist->Invalidate();
		}
	}
	else if(mpo_View->mb_LockTrans)
	{
		if(plist->GetBkColor() != GetSysColor(COLOR_BTNFACE))
		{
			plist->SetBkColor(GetSysColor(COLOR_BTNFACE));
			plist->SetTextBkColor(GetSysColor(COLOR_BTNFACE));
			plist->Invalidate();
		}
	}
	else
	{
		if(plist->GetBkColor() != GetSysColor(COLOR_WINDOW))
		{
			plist->SetBkColor(GetSysColor(COLOR_WINDOW));
			plist->SetTextBkColor(GetSysColor(COLOR_WINDOW));
			plist->Invalidate();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_Frame::DuplicateKit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Action;
	ACT_st_Action		*pst_Action;
	BIG_KEY				ul_Key;
	BIG_INDEX			ul_File, ul_Kit;
	char				asz_SrcPath[BIG_C_MaxLenPath];
	char				asz_DestPath[BIG_C_MaxLenPath];
	BIG_KEY				ak[1000];
	BIG_tdst_GroupElem	st_Elem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(ak, 0, sizeof(ak));
	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) mpst_CurrentActionKit);
	ul_Kit = ul_File = BIG_ul_SearchKeyToFat(ul_Key);
	BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_SrcPath);
	L_strcpy(asz_DestPath, asz_SrcPath);
	L_strcat(asz_DestPath, "/Copy");
	BIG_ul_CreateDir(asz_DestPath);

	/* Toutes les actions */
	for(i_Action = 0; i_Action < mpst_CurrentActionKit->uw_NumberOfAction; i_Action++)
	{
		pst_Action = mpst_CurrentActionKit->apst_Action[i_Action];
		if(!pst_Action) continue;

		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Action);
		ul_File = BIG_ul_SearchKeyToFat(ul_Key);
		BIG_CopyFile(asz_DestPath, asz_SrcPath, BIG_NameFile(ul_File));
		ul_File = BIG_ul_SearchFileExt(asz_DestPath, BIG_NameFile(ul_File));
		ak[i_Action] = BIG_FileKey(ul_File);
	}

	/* Kit */
	SAV_Begin(asz_DestPath, BIG_NameFile(ul_Kit));
	for(i_Action = 0; i_Action < mpst_CurrentActionKit->uw_NumberOfAction; i_Action++)
	{
		if(ak[i_Action])
		{
			st_Elem.ul_Key = ak[i_Action];
			L_memcpy(&st_Elem.ul_Type, EDI_Csz_ExtAction, sizeof(st_Elem.ul_Type));
		}
		else
		{
			st_Elem.ul_Key = st_Elem.ul_Type = 0;
		}

		SAV_Buffer(&st_Elem, sizeof(st_Elem));
	}

	SAV_ul_End();
}
#endif /* ACTIVE_EDITORS */
