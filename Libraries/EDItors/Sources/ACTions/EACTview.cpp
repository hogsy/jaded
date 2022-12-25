/*$T EACTview.cpp GC! 1.081 06/01/01 18:05:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#ifdef ACTIVE_EDITORS
#include "EACTframe.h"
#include "EACTView.h"
#include "Res/Res.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDImainframe.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "ENGine/Sources/ACTions/ACTinit.h"
#include "ENGine/Sources/ACTions/ACTsave.h"
#include "ENGine/Sources/ACTions/ACTcompute.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/WORld/WORvars.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "LINKs/LINKtoed.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "EDIpaths.h"
#include "BROwser/BROframe.h"
#include "EDIeditors_infos.h"
#include "EDImsg.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNCREATE(EACT_cl_View, CFormView)
BEGIN_MESSAGE_MAP(EACT_cl_View, CFormView)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_ZONE0, IDC_CHECK_ZONE15, OnZoneCheckClick)	
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_CBIT0, IDC_CHECK_CBIT7, OnCBitCheckClick)
	ON_COMMAND(IDC_CHECK_ARAM, OnARAMClick)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ACTIONS, OnActionKitSelect)
	ON_LBN_SELCHANGE(IDC_LIST_ACTIONITEMS, OnActionItemSelect)
	ON_CBN_SELCHANGE(IDC_COMBO_MODE, OnModeChange)
	ON_CBN_SELCHANGE(IDC_COMBO_LOOP, OnLoopChange)
	ON_CBN_SELCHANGE(IDC_COMBO_COPYFROMACT, OnCopyFromActChange)
	ON_EN_KILLFOCUS(IDC_EDIT_FREQ, OnFreqChange)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOPNUM, OnLoopNumChange)
	ON_BN_CLICKED(IDC_CHECK_IGNOREGRAVITY, OnIgnoreGravity)
	ON_BN_CLICKED(IDC_CHECK_DEVELOPPED, OnDevelopped)
	ON_BN_CLICKED(IDC_CHECK_IGNORETRACTION, OnIgnoreTraction)
	ON_BN_CLICKED(IDC_CHECK_IGNORESTREAM, OnIgnoreStream)
	ON_BN_CLICKED(IDC_CHECK_IGNOREVTRANS, OnIgnoreVTrans)
	ON_COMMAND(IDC_ACTIONITEM_UP, OnUpActionItem)
	ON_COMMAND(IDC_ACTIONITEM_DOWN, OnDownActionItem)
	ON_COMMAND(IDC_ACTION_UP, OnUpAction)
	ON_COMMAND(IDC_ACTION_DOWN, OnDownAction)
	ON_COMMAND(IDC_ACTION_HOLE, OnHoleAction)
	ON_COMMAND(IDC_BUTTON_BROWSEANIM, OnBrowseAnim)
	ON_COMMAND(IDC_BUTTON_BROWSESHAPE, OnBrowseShape)
	ON_COMMAND(IDC_BUTTON_TRANSMEME, OnTransMe)
	ON_COMMAND(IDC_BUTTON_COPYFROM, OnCopyFrom)
	ON_COMMAND(IDC_BUTTON_COPYFROMOVER, OnCopyFromOver)
	ON_COMMAND(IDC_BUTTON_COPYSAMEAS, OnCopySameAs)
	ON_COMMAND(IDC_BUTTON_KILLALLTRANS, DeleteAllTrans)
	ON_COMMAND(IDC_BUTTON_KILLTO, OnKillTo)
	ON_COMMAND(IDC_BUTTON_VIEWTO, OnViewTo)
	ON_COMMAND(IDC_BUTTON_USETRANS, OnUseTrans)
	ON_BN_CLICKED(IDC_BUTTON_LOCK, OnLockTrans)
	ON_BN_CLICKED(IDC_BUTTON_TRANSNONE, OnTransActNone)
	ON_CBN_SELCHANGE(IDC_COMBO_TRANSACT, OnTransActChange)
	ON_BN_CLICKED(IDC_CHECK_BLENDPROGRESSIVE, OnTransBlendProgressive)
	ON_BN_CLICKED(IDC_CHECK_BLENDPROGRESSIVEINV, OnTransBlendProgressiveInv)
	ON_BN_CLICKED(IDC_CHECK_BLENDFREEZE, OnTransActChange)
	ON_BN_CLICKED(IDC_CHECK_BLENDFREEZEMB, OnTransActChange)
	ON_BN_CLICKED(IDC_CHECK_BLENDSTOCK, OnTransActChange)
	ON_BN_CLICKED(IDC_CHECK_BLENDOFF, OnTransActChange)
	ON_BN_CLICKED(IDC_CHECK_DEFAULT, OnTransActChange)
	ON_BN_CLICKED(IDC_CHECK_NEXTDEFAULT, OnTransActChange)
	ON_EN_KILLFOCUS(IDC_EDIT_BLENDTIME, OnBlendTimeChange)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$off*/
int ai_ButZones[16] =
	{
		IDC_CHECK_ZONE0, IDC_CHECK_ZONE1, IDC_CHECK_ZONE2, IDC_CHECK_ZONE3, IDC_CHECK_ZONE4, IDC_CHECK_ZONE5, IDC_CHECK_ZONE6,
		IDC_CHECK_ZONE7,
		IDC_CHECK_ZONE8, IDC_CHECK_ZONE9, IDC_CHECK_ZONE10, IDC_CHECK_ZONE11, IDC_CHECK_ZONE12, IDC_CHECK_ZONE13, IDC_CHECK_ZONE14,
		IDC_CHECK_ZONE15,
	};
int ai_ButCBits[8] =
	{
		IDC_CHECK_CBIT0, IDC_CHECK_CBIT1, IDC_CHECK_CBIT2, IDC_CHECK_CBIT3, IDC_CHECK_CBIT4, IDC_CHECK_CBIT5, IDC_CHECK_CBIT6,
		IDC_CHECK_CBIT7
	};
/*$on*/

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EACT_cl_View::EACT_cl_View(void) :
	CFormView(DIALOGS_IDD_ACTIONS)
{
	mpo_Editor = NULL;
	mb_LockTrans = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EACT_cl_View::~EACT_cl_View(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EACT_cl_View::Create
(
	LPCTSTR		lpszClassName,
	LPCTSTR		lpszWindowName,
	DWORD		dwStyle,
	const RECT	&rect,
	CWnd		*pParentWnd,
	UINT		nID
)
{
	/*~~~~~~~~~~~~~~~~*/
	CListCtrl	*plist;
	CComboBox	*pcombo;
	/*~~~~~~~~~~~~~~~~*/

	CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, NULL);
	EDIA_cl_BaseDialog::SetTheme(this);

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	plist->InsertColumn(0, "Actions", LVCFMT_LEFT, 150);
	plist->InsertColumn(1, "Blend", LVCFMT_LEFT, 50);
	plist->InsertColumn(2, "Time", LVCFMT_LEFT, 50);
	plist->InsertColumn(3, "Actions", LVCFMT_LEFT, 150);

	/* Modes */
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_MODE);
	pcombo->InsertString(0, "(0) Speed From Anim");
	pcombo->InsertString(1, "(1) Anim Freq changed by Dyna");
	pcombo->InsertString(2, "(2) ?");
	pcombo->InsertString(3, "(3) Speed From Dyna");
	pcombo->InsertString(4, "(4) ?");
	pcombo->InsertString(5, "(5) ?");
	pcombo->InsertString(6, "(6) ?");
	pcombo->InsertString(7, "(7) ?");

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	MoveActionList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::MoveActionList(void)
{
	/*~~~~~~~~~~~~~~~*/
	CListCtrl	*plist;
	CButton		*pbut;
	CRect		o_Rect;
	int			cy;
	/*~~~~~~~~~~~~~~~*/

	GetClientRect(&o_Rect);
	cy = o_Rect.Height();
	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	if(plist)
	{
		plist->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);
		o_Rect.bottom = cy - 30;
		plist->MoveWindow(o_Rect);

		pbut = (CButton *) GetDlgItem(IDC_ACTION_UP);
		pbut->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);
		o_Rect.top = cy - 25;
		o_Rect.bottom = cy - 5;
		pbut->MoveWindow(&o_Rect);

		pbut = (CButton *) GetDlgItem(IDC_ACTION_DOWN);
		pbut->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);
		o_Rect.top = cy - 25;
		o_Rect.bottom = cy - 5;
		pbut->MoveWindow(&o_Rect);

		pbut = (CButton *) GetDlgItem(IDC_ACTION_HOLE);
		pbut->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);
		o_Rect.top = cy - 25;
		o_Rect.bottom = cy - 5;
		pbut->MoveWindow(&o_Rect);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EACT_cl_View::OnScroll(UINT n, UINT p, BOOL b)
{
	/*~~~~~~~~~*/
	BOOL	bRes;
	/*~~~~~~~~~*/

	M_MF()->LockDisplay(this);
	bRes = CFormView::OnScroll(n, p, b);
	MoveActionList();
	M_MF()->UnlockDisplay(this);
	return bRes;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EACT_cl_View::PreTranslateMessage(MSG *pmsg)
{
	if(pmsg->message == WM_RBUTTONDOWN)
	{
		if(pmsg->hwnd == GetDlgItem(IDC_LIST_ACTIONS)->m_hWnd)
		{
			if(!mb_LockTrans) OnNewAction();
			return TRUE;
		}

		if(pmsg->hwnd == GetDlgItem(IDC_LIST_ACTIONITEMS)->m_hWnd)
		{
			if(!mb_LockTrans) OnNewActionItem();
			return TRUE;
		}
	}

	if(pmsg->message == WM_KEYDOWN)
	{
		switch(pmsg->wParam)
		{
		case VK_RETURN:
			GetDlgItem(IDC_LIST_ACTIONS)->SetFocus();
			break;
		case VK_HOME:
			if(((CButton *) GetDlgItem(IDC_BUTTON_LOCK))->GetCheck())
				((CButton *) GetDlgItem(IDC_BUTTON_LOCK))->SetCheck(0);
			else
				((CButton *) GetDlgItem(IDC_BUTTON_LOCK))->SetCheck(1);
			OnLockTrans();
			return TRUE;
		}
	}

	return CFormView::PreTranslateMessage(pmsg);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::CloseAll(void)
{
	/*~~~~~~~~~~~~~~~*/
	CListCtrl	*plist;
	/*~~~~~~~~~~~~~~~*/

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);

	if(mb_LockTrans)
	{
		((CButton *) GetDlgItem(IDC_BUTTON_LOCK))->SetCheck(0);
		plist->SetBkColor(GetSysColor(COLOR_WINDOW));
		plist->SetTextBkColor(GetSysColor(COLOR_WINDOW));
		mb_LockTrans = FALSE;
	}

	mpo_Editor->mpst_CurrentActionKit = NULL;
	plist->DeleteAllItems();
	CloseAction();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::CloseAction(void)
{
	/*~~~~~~~~~~~~~~~*/
	CListBox	*plist;
	/*~~~~~~~~~~~~~~~*/

	if(mb_LockTrans) return;
	plist = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	plist->ResetContent();
	((CStatic *) GetDlgItem(IDC_STATIC_ACTION))->SetWindowText("Action Item");
	CloseActionItem();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::CloseActionItem(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CEdit		*pedit;
	CButton		*pbut;
	int			i;
	CComboBox	*pcombo;
	/*~~~~~~~~~~~~~~~~*/

	pedit = (CEdit *) GetDlgItem(IDC_EDIT_ANIM);
	pedit->SetWindowText("");
	pedit = (CEdit *) GetDlgItem(IDC_EDIT_SHAPE);
	pedit->SetWindowText("");
	pedit = (CEdit *) GetDlgItem(IDC_EDIT_FREQ);
	pedit->SetWindowText("");
	pedit = (CEdit *) GetDlgItem(IDC_EDIT_LENGTH);
	pedit->SetWindowText("");
	pedit = (CEdit *) GetDlgItem(IDC_EDIT_SECONDS);
	pedit->SetWindowText("");
	pedit = (CEdit *) GetDlgItem(IDC_EDIT_LOOPNUM);
	pedit->SetWindowText("");

	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_MODE);
	pcombo->SetCurSel(0);

	for(i = 0; i < 16; i++)
	{
		pbut = (CButton *) GetDlgItem(ai_ButZones[i]);
		pbut->SetCheck(0);
	}

	for(i = 0; i < 8; i++)
	{
		pbut = (CButton *) GetDlgItem(ai_ButCBits[i]);
		pbut->SetCheck(0);
	}

	pbut = (CButton *) GetDlgItem(IDC_CHECK_IGNOREGRAVITY);
	pbut->SetCheck(0);
	pbut = (CButton *) GetDlgItem(IDC_CHECK_IGNORETRACTION);
	pbut->SetCheck(0);
	pbut = (CButton *) GetDlgItem(IDC_CHECK_IGNORESTREAM);
	pbut->SetCheck(0);
	pbut = (CButton *) GetDlgItem(IDC_CHECK_IGNOREVTRANS);
	pbut->SetCheck(0);
	pbut = (CButton *) GetDlgItem(IDC_CHECK_DEVELOPPED);
	pbut->SetCheck(0);

}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::SetActionKit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT				i;
	int					index, index1;
	CListCtrl			*plist;
	CComboBox			*pcomp;
	CComboBox			*pcomp1;
	CComboBox			*pcomp2;
	BIG_INDEX			ul_Action;
	char				asz_Name[128];
	char				asz_Name1[128];
	LV_ITEM				st_Item;
	ACT_tdst_Transition *pst_Trans;
	int					i_ScrollV;
	CRect				rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->LockDisplay(this);
	CloseAction();
	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	i_ScrollV = plist->GetScrollPos(SB_VERT);
	pcomp = (CComboBox *) GetDlgItem(IDC_COMBO_TRANSACT);
	pcomp1 = (CComboBox *) GetDlgItem(IDC_COMBO_COPYFROMACT);
	pcomp2 = (CComboBox *) GetDlgItem(IDC_COMBO_COPYSAMEAS);

	/* Transition combo */
	pcomp->ResetContent();
	pcomp1->ResetContent();
	pcomp2->ResetContent();
	index = pcomp->AddString("None");
	pcomp->SetItemData(index, -1);

	st_Item.mask = LVIF_TEXT;
	st_Item.pszText = asz_Name1;
	st_Item.iImage = 0;
	st_Item.cchTextMax = 128;

	plist->DeleteAllItems();
	for(i = 0; i < mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction; i++)
	{
		if(mpo_Editor->mpst_CurrentActionKit->apst_Action[i])
		{
			ul_Action = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Editor->mpst_CurrentActionKit->apst_Action[i]);
			ERR_X_Assert(ul_Action != BIG_C_InvalidIndex);
			sprintf(asz_Name, "(%03d)  %s", i, BIG_NameFile(ul_Action));
			*L_strrchr(asz_Name, '.') = 0;
			if(mb_LockTrans && mi_SelWhenTrans == i) L_strcat(asz_Name, " *");
			index = plist->InsertItem(i, asz_Name, 0);
			plist->SetItemData(index, ul_Action);

			if(mb_LockTrans)
			{
				pst_Trans = GetTrans(i);
				if(pst_Trans)
				{
					st_Item.iItem = index;

					/* By action */
					if(pst_Trans->uw_Action != 0xFFFF)
					{
						ul_Action = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Editor->mpst_CurrentActionKit->apst_Action[pst_Trans->uw_Action]);
						if(ul_Action == BIG_C_InvalidIndex)
							L_strcpy(asz_Name1, "ERROR");
						else
						{
							sprintf(asz_Name1, "(%03d)  %s", pst_Trans->uw_Action, BIG_NameFile(ul_Action));
							*L_strrchr(asz_Name1, '.') = 0;
						}

						st_Item.iSubItem = 3;
						plist->SetItem(&st_Item);
					}

					st_Item.iSubItem = 2;
					_itoa(pst_Trans->uc_Blend, asz_Name1, 10);
					plist->SetItem(&st_Item);

					*asz_Name1 = 0;
					if(pst_Trans->uc_Flag & ACT_C_TF_BlendProgressive) L_strcat(asz_Name1, "P ");
					if(pst_Trans->uc_Flag & ACT_C_TF_BlendProgressiveInv) L_strcat(asz_Name1, "PI ");
					if(pst_Trans->uc_Flag & ACT_C_TF_BlendFreezeBones) L_strcat(asz_Name1, "F ");
					if(pst_Trans->uc_Flag & ACT_C_TF_BlendFreezeMagicBox) L_strcat(asz_Name1, "FMB ");
					if(pst_Trans->uc_Flag & ACT_C_TF_BlendStock) L_strcat(asz_Name1, "STO ");
					st_Item.iSubItem = 1;
					plist->SetItem(&st_Item);
				}
			}

			/* Combo */
			index = pcomp->AddString(asz_Name);
			pcomp->SetItemData(index, i);
			index1 = pcomp1->AddString(asz_Name);
			pcomp1->SetItemData(index1, i);
			index1 = pcomp2->AddString(asz_Name);
			pcomp2->SetItemData(index1, i);
		}
		else
		{
			sprintf(asz_Name, "(%03d)", i);
			index = plist->InsertItem(i, asz_Name, 0);
			plist->SetItemData(index, BIG_C_InvalidIndex);
		}
	}

	if(i_ScrollV)
	{
		if(plist->GetItemCount())
		{
			plist->GetItemRect(0, &rect, 0);
			plist->Scroll(CSize(0, i_ScrollV * rect.Height()));
		}
	}

	pcomp1->SetCurSel(0);
	pcomp2->SetCurSel(0);
	OnCopyFromActChange();

	M_MF()->UnlockDisplay(this);
	EnableTrans();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnActionKitSelect(NMHDR *pNotifyStruct, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~*/
	CListCtrl	*plist;
	int			item;
	CButton		*pcheck;
	BIG_INDEX	ul_Index;
	static int	rec = FALSE;
	/*~~~~~~~~~~~~~~~~~*/

	if(rec) return;
	rec = TRUE;

		/* ARAM Kit ? */
	pcheck = (CButton *) GetDlgItem(IDC_CHECK_ARAM);
	if(mpo_Editor->mpst_CurrentActionKit->ul_Flag & 1)
		pcheck->SetCheck(1);
	else
		pcheck->SetCheck(0);

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	item = plist->GetNextItem(-1, LVNI_SELECTED);
	if((item == -1) || ((ul_Index = plist->GetItemData(item)) == BIG_C_InvalidIndex))
	{
		mpo_Editor->mpst_CurrentAction = NULL;
		if(mb_LockTrans) 
		{
			rec = FALSE;
			return;
		}
		mpo_Editor->mpst_CurrentActionItem = NULL;
		CloseAction();
		rec = FALSE;
		return;
	}

	if(GetAsyncKeyState(VK_RIGHT) < 0)
	{
		SetActionByIndex(ul_Index, TRUE);
		while(GetAsyncKeyState(VK_RIGHT) < 0);
	}
	else
		SetActionByIndex(ul_Index);

	/* ARAM Kit ? */
	pcheck = (CButton *) GetDlgItem(IDC_CHECK_ARAM);
	if(mpo_Editor->mpst_CurrentActionKit->ul_Flag & 1)
		pcheck->SetCheck(1);
	else
		pcheck->SetCheck(0);

	rec = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::SetActionByIndex(BIG_INDEX ul_Index, BOOL _b_Trans)
{
	mpo_Editor->mpst_CurrentAction = (ACT_st_Action *) LOA_ul_SearchAddress(BIG_PosFile(ul_Index));
	ERR_X_Assert(mpo_Editor->mpst_CurrentAction && ((int) mpo_Editor->mpst_CurrentAction != -1));

	if(mb_LockTrans && _b_Trans)
	{
		mb_LockTrans = FALSE;
		SetAction();
		mb_LockTrans = TRUE;
		OnLockTrans();
		SetTrans();
	}
	else
	{
		SetAction();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::SetAction(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*plist;
	CListCtrl	*pctrl;
	CStatic		*pstat;
	int			i, index;
	BIG_INDEX	ul_TrackList;
	char		asz_Name[BIG_C_MaxLenName];
	char		*psz_Temp;
	CComboBox	*pcombo;
	CString		o_Str;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Set all action items */
	if(mb_LockTrans)
	{
		SetTrans();
		return;
	}

	CloseAction();

	/* Set action item title */
	pctrl = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	pstat = (CStatic *) GetDlgItem(IDC_STATIC_ACTION);
	o_Str = pctrl->GetItemText(pctrl->GetNextItem(-1, LVNI_SELECTED), 0);
	o_Str = "Action Item (" + o_Str + ")";
	pstat->SetWindowText(o_Str);

	plist = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	plist->ResetContent();
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_LOOP);
	pcombo->ResetContent();
	pcombo->AddString("No Loop");
	if((!mpo_Editor->mpst_CurrentAction) || ((int) mpo_Editor->mpst_CurrentAction == -1)) return;
	for(i = 0; i < mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem; i++)
	{
		if(mpo_Editor->mpst_CurrentAction->ast_ActionItem[i].pst_TrackList)
		{
			ul_TrackList = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Editor->mpst_CurrentAction->ast_ActionItem[i].pst_TrackList);
			if(ul_TrackList == BIG_C_InvalidIndex)
			{
				L_strcpy(asz_Name, "<No Track List>");
				mpo_Editor->mpst_CurrentAction->ast_ActionItem[i].pst_TrackList = NULL;
			}
			else
			{
				L_strcpy(asz_Name, BIG_NameFile(ul_TrackList));
				psz_Temp = L_strrchr(asz_Name, '.');
				if(psz_Temp) *psz_Temp = 0;
			}
		}
		else
		{
			L_strcpy(asz_Name, "<No Track List>");
		}

		index = plist->InsertString(i, asz_Name);
		plist->SetItemData(index, i);
		index = pcombo->AddString(asz_Name);
	}

	if(mpo_Editor->mpst_CurrentAction->uc_ActionItemNumberForLoop == 255)
		pcombo->SetCurSel(0);
	else
		pcombo->SetCurSel(mpo_Editor->mpst_CurrentAction->uc_ActionItemNumberForLoop + 1);

	/* Select first action item */
	if(mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem)
	{
		plist->SetCurSel(0);
		SetActionItemByIndex(0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::SetActionItemByIndex(int index)
{
	if(mb_LockTrans)
	{
		mpo_Editor->mpst_CurrentActionItem = &mpo_Editor->mpst_CurrentActionKit->apst_Action[mi_SelWhenTrans]->ast_ActionItem[index];
	}
	else
	{
		mpo_Editor->mpst_CurrentActionItem = &mpo_Editor->mpst_CurrentAction->ast_ActionItem[index];
	}

	SetActionItem();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::SetActionItem(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CEdit		*pedit;
	BIG_INDEX	ul_Index;
	char		asz_Name[BIG_C_MaxLenName];
	CButton		*pcheck;
	int			i;
	CButton		*pbut;
	CComboBox	*pcombo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pedit = (CEdit *) GetDlgItem(IDC_EDIT_ANIM);
	if(mpo_Editor->mpst_CurrentActionItem->pst_TrackList)
	{
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Editor->mpst_CurrentActionItem->pst_TrackList);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
		pedit->SetWindowText(BIG_NameFile(ul_Index));
	}
	else
	{
		pedit->SetWindowText("<No Anim>");
	}

	pedit = (CEdit *) GetDlgItem(IDC_EDIT_SHAPE);
	if(mpo_Editor->mpst_CurrentActionItem->pst_Shape)
	{
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Editor->mpst_CurrentActionItem->pst_Shape);
		if(ul_Index != BIG_C_InvalidIndex)
			pedit->SetWindowText(BIG_NameFile(ul_Index));
		else
			pedit->SetWindowText("<No Shape>");
	}
	else
	{
		pedit->SetWindowText("<No Shape>");
	}

	/* Rep */
	sprintf(asz_Name, "%d", mpo_Editor->mpst_CurrentActionItem->uc_Repetition);
	pedit = (CEdit *) GetDlgItem(IDC_EDIT_LOOPNUM);
	pedit->SetWindowText(asz_Name);

	/* Freq */
	sprintf(asz_Name, "%d", mpo_Editor->mpst_CurrentActionItem->uc_Frequency);
	pedit = (CEdit *) GetDlgItem(IDC_EDIT_FREQ);
	pedit->SetWindowText(asz_Name);

	/* Length */
	if(mpo_Editor->mpst_CurrentActionItem->pst_TrackList)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EVE_tdst_Event	*pst_Event, *pst_Last;
		ULONG			ul_NumFrames;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_NumFrames = 0;
		pst_Event = mpo_Editor->mpst_CurrentActionItem->pst_TrackList->pst_AllTracks[0].pst_AllEvents;
		pst_Last = pst_Event + mpo_Editor->mpst_CurrentActionItem->pst_TrackList->pst_AllTracks[0].uw_NumEvents;
		for(; pst_Event < pst_Last; pst_Event++) ul_NumFrames += pst_Event->uw_NumFrames;

		sprintf(asz_Name, "%i", ul_NumFrames);
		pedit = (CEdit *) GetDlgItem(IDC_EDIT_LENGTH);
		pedit->SetWindowText(asz_Name);

		sprintf(asz_Name, "%0.3f", (float) ul_NumFrames * 1.0f / ((float) (((int) mpo_Editor->mpst_CurrentActionItem->uc_Frequency) * 60 / ACT_C_DefaultAnimFrequency)));
		pedit = (CEdit *) GetDlgItem(IDC_EDIT_SECONDS);
		pedit->SetWindowText(asz_Name);

	}
	else
	{
		pedit = (CEdit *) GetDlgItem(IDC_EDIT_LENGTH);
		pedit->SetWindowText("");
		pedit = (CEdit *) GetDlgItem(IDC_EDIT_SECONDS);
		pedit->SetWindowText("");
	}

	/* Average Speed */
	if(mpo_Editor->mpst_CurrentActionItem->pst_TrackList)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EVE_tdst_Event	*pst_Event, *pst_Last;
		ULONG			ul_NumFrames;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_NumFrames = 0;
		pst_Event = mpo_Editor->mpst_CurrentActionItem->pst_TrackList->pst_AllTracks[1].pst_AllEvents;
		pst_Last = pst_Event + mpo_Editor->mpst_CurrentActionItem->pst_TrackList->pst_AllTracks[1].uw_NumEvents;
		
		if((pst_Event->w_Flags & EVE_C_EventFlag_InterpolationKey) && (EVE_w_Event_InterpolationKey_GetType(pst_Event) & EVE_InterKeyType_Translation_Mask))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	*pst_PosInit, *pst_PosFinal, st_Move;
			float				f_Speed;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_PosInit = EVE_pst_Event_InterpolationKey_GetPos(pst_Event);
			pst_PosFinal = EVE_pst_Event_InterpolationKey_GetPos(pst_Last - 1);

			for(; pst_Event < pst_Last; pst_Event++) ul_NumFrames += pst_Event->uw_NumFrames;

			MATH_SubVector(&st_Move, pst_PosFinal, pst_PosInit);
			f_Speed = MATH_f_NormVector(&st_Move) / ((float) ul_NumFrames / ((float) (((int) mpo_Editor->mpst_CurrentActionItem->uc_Frequency) * 60 / ACT_C_DefaultAnimFrequency)));
			

			sprintf(asz_Name, "%0.2f", f_Speed);
			pedit = (CEdit *) GetDlgItem(IDC_EDIT_SPEED);
			pedit->SetWindowText(asz_Name);
		}
		else
		{
			pedit = (CEdit *) GetDlgItem(IDC_EDIT_SPEED);
			pedit->SetWindowText("");
		}		
	}
	else
	{
		pedit = (CEdit *) GetDlgItem(IDC_EDIT_SPEED);
		pedit->SetWindowText("");
	}



	/* Zones */
	for(i = 0; i < 16; i++)
	{
		pcheck = (CButton *) GetDlgItem(ai_ButZones[i]);
		if(mpo_Editor->mpst_CurrentActionItem->uw_DesignFlags & (1 << i))
			pcheck->SetCheck(1);
		else
			pcheck->SetCheck(0);
	}

	/* CBits */
	for(i = 0; i < 8; i++)
	{
		pcheck = (CButton *) GetDlgItem(ai_ButCBits[i]);
		if(mpo_Editor->mpst_CurrentActionItem->uc_CustomBit & (1 << i))
			pcheck->SetCheck(1);
		else
			pcheck->SetCheck(0);
	}

	/* Mode */
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_MODE);
	pcombo->SetCurSel((mpo_Editor->mpst_CurrentActionItem->uc_Flag & 0x07) >> 1);

	/* Flags */
	pbut = (CButton *) GetDlgItem(IDC_CHECK_IGNOREGRAVITY);
	if(mpo_Editor->mpst_CurrentActionItem->uc_Flag & ACT_C_ActionItemFlag_IgnoreGravity)
		pbut->SetCheck(1);
	else
		pbut->SetCheck(0);

	pbut = (CButton *) GetDlgItem(IDC_CHECK_DEVELOPPED);
	if(mpo_Editor->mpst_CurrentActionItem->uc_Flag & ACT_C_ActionItemFlag_Developped)
		pbut->SetCheck(1);
	else
		pbut->SetCheck(0);


	pbut = (CButton *) GetDlgItem(IDC_CHECK_IGNORETRACTION);
	if(mpo_Editor->mpst_CurrentActionItem->uc_Flag & ACT_C_ActionItemFlag_IgnoreTraction)
		pbut->SetCheck(1);
	else
		pbut->SetCheck(0);

	pbut = (CButton *) GetDlgItem(IDC_CHECK_IGNORESTREAM);
	if(mpo_Editor->mpst_CurrentActionItem->uc_Flag & ACT_C_ActionItemFlag_IgnoreStream)
		pbut->SetCheck(1);
	else
		pbut->SetCheck(0);

	pbut = (CButton *) GetDlgItem(IDC_CHECK_IGNOREVTRANS);
	if(mpo_Editor->mpst_CurrentActionItem->uc_Flag & ACT_C_ActionItemFlag_IgnoreVerticalTranslation)
		pbut->SetCheck(1);
	else
		pbut->SetCheck(0);

	/* Set current transition */
	SetTrans();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnActionItemSelect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*LB;
	int			index, isel;
	CListCtrl	*plist;
	/*~~~~~~~~~~~~~~~~~~~~*/

	LB = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	index = LB->GetCurSel();
	if(index == -1)
	{
		mpo_Editor->mpst_CurrentActionItem = NULL;
		CloseActionItem();
		return;
	}

	if(mb_LockTrans)
	{
		plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
		isel = plist->GetNextItem(-1, LVNI_SELECTED);
		SetActionKit();
		plist->SetItemState(isel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}

	SetActionItemByIndex(index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::RenameAction(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX			ul_Index;
	CListCtrl			*plist;
	EDIA_cl_NameDialog	o_Dialog("Enter action name");
	int					iSel;
	char				asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	iSel = plist->GetNextItem(-1, LVNI_SELECTED);
	if(iSel == -1) return;
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel]);
	if(ul_Index == BIG_C_InvalidIndex) return;
	o_Dialog.mo_Name = BIG_NameFile(ul_Index);
	*L_strrchr((char *) (LPCSTR) o_Dialog.mo_Name, '.') = 0;
	if(o_Dialog.DoModal() == IDOK)
	{
		o_Dialog.mo_Name += EDI_Csz_ExtAction;
		if(BIG_b_CheckName((char *) (LPCSTR) o_Dialog.mo_Name) == FALSE) return;
		BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
		BIG_RenFile((char *) (LPCSTR) o_Dialog.mo_Name, asz_Path, BIG_NameFile(ul_Index));
		SetActionKit();
		plist->SetItemState(iSel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnNewAction(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Res, iSel, i;
	POINT				pt;
	EDIA_cl_NameDialog	o_Dialog("Enter action name");
	EMEN_cl_SubMenu		o_Menu(FALSE);
	CListCtrl			*plist;
	BIG_INDEX			ul_Index;
	CString				o_Temp;
	EDIA_cl_FileDialog	o_File("Choose Action", 0, 0, 1, NULL, "*"EDI_Csz_ExtAction);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_File.mb_UseFavorite = TRUE;

	/* Select item under mouse */
    plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	GetCursorPos(&pt);
	plist->ScreenToClient(&pt);
	plist->SendMessage(WM_LBUTTONDOWN, 0, (pt.y << 16) + pt.x);
	plist->SendMessage(WM_LBUTTONUP, 0, (pt.y << 16) + pt.x);
	iSel = plist->GetNextItem(-1, LVNI_SELECTED);

	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	if(iSel == -1)
	{
		M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 1, TRUE, "Insert New Action", -1);
		M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 3, TRUE, "Insert Loaded Action", -1);
	}
	else
	{
		M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 1, TRUE, "Replace By New Action", -1);
		M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 3, TRUE, "Replace By Loaded Action", -1);
		if(mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel])
		{
			M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 2, TRUE, "Set To Empty Action", -1);
			M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 5, TRUE, "Rename Action", -1);
		}

		M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 4, TRUE, "Delete Action", -1);
	}

	GetCursorPos(&pt);
	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
_Try_
	switch(i_Res)
	{
	case 1:
		if(o_Dialog.DoModal() == IDOK)
		{
			ACT_NewAction(mpo_Editor->mpst_CurrentActionKit, (char *) (LPCSTR) o_Dialog.mo_Name, iSel);
			SetActionKit();
			plist->SetItemState(iSel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		}
		break;

	case 2:
		if(iSel == -1) break;
		if(mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel])
			ACT_FreeAction(&mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel]);
		mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel] = NULL;
		SetActionKit();
		LINK_UpdatePointers();
		plist->SetItemState(iSel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		break;

	case 3:
		if(o_File.DoModal() == IDOK)
		{
			o_File.GetItem(o_File.mo_File, 0, o_Temp);
			ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
			if(ul_Index != BIG_C_InvalidIndex)
			{
				if(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtAction))
				{
					ERR_X_ForceError("This is not an action file", NULL);
				}
				else
				{
					ACT_LoadSetAction(mpo_Editor->mpst_CurrentActionKit, ul_Index, iSel);
					SetActionKit();
					plist->SetItemState(iSel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				}
			}
		}
		break;

	case 4:
		if(iSel == -1) break;
		if(mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel])
			ACT_FreeAction(&mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel]);
		for(i = iSel + 1; i < plist->GetItemCount(); i++) HasBeenMove(i, -1);
		L_memmove
		(
			&mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel],
			&mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel + 1],
			(mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction - iSel - 1) * sizeof(void *)
		);
		mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction--;
		SetActionKit();
		plist->SetItemState(iSel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		break;

	case 5:
		RenameAction();
		break;
	}

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnNewActionItem(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_Res;
	POINT			pt;
	EMEN_cl_SubMenu o_Menu(FALSE);
	CListBox		*plist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((!mpo_Editor->mpst_CurrentAction) || ((int) mpo_Editor->mpst_CurrentAction == -1)) return;

	/* Select item under mouse */
	plist = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	GetCursorPos(&pt);
	plist->ScreenToClient(&pt);
	plist->SendMessage(WM_LBUTTONDOWN, 0, (pt.y << 16) + pt.x);
	plist->SendMessage(WM_LBUTTONUP, 0, (pt.y << 16) + pt.x);

	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 1, TRUE, "New Action Item", -1);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 2, TRUE, "Delete Action Item", -1);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 3, TRUE, "Show in browser", -1);

	GetCursorPos(&pt);
	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	switch(i_Res)
	{
	/* Add */
	case 1:
		if(mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem == ACT_C_SizeOfAction)
		{
			ERR_X_ForceError("Too many items defined", NULL);
		}
		else
		{
			L_memset
			(
				&mpo_Editor->mpst_CurrentAction->ast_ActionItem[mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem],
				0,
				sizeof(ACT_st_ActionItem)
			);
			mpo_Editor->mpst_CurrentAction->ast_ActionItem[mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem].uc_Frequency = 32;
			mpo_Editor->mpst_CurrentAction->ast_ActionItem[mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem].uc_Repetition = 1;
			if(!mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem) mpo_Editor->mpst_CurrentAction->uc_ActionItemNumberForLoop = ACT_C_NoActionItem;
			mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem++;
			SetAction();
			plist->SetCurSel(mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem - 1);
			SetActionItemByIndex(mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem - 1);
		}
		break;

	/* Delete */
	case 2:
		i_Res = plist->GetCurSel();
		if(i_Res == -1) break;
		if(mpo_Editor->mpst_CurrentAction->ast_ActionItem[i_Res].pst_TrackList)
			EVE_DeleteListTracks(mpo_Editor->mpst_CurrentAction->ast_ActionItem[i_Res].pst_TrackList);
		L_memmove
		(
			&mpo_Editor->mpst_CurrentAction->ast_ActionItem[i_Res],
			&mpo_Editor->mpst_CurrentAction->ast_ActionItem[i_Res + 1],
			(mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem - (i_Res + 1)) * sizeof(ACT_st_ActionItem)
		);
		mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem--;
		SetAction();
		if(i_Res >= plist->GetCount()) i_Res--;
		if(i_Res >= 0)
		{
			plist->SetCurSel(i_Res);
			SetActionItemByIndex(i_Res);
		}
		break;

    /* show in browser */
    case 3:
		i_Res = plist->GetCurSel();
		if(i_Res == -1) break;
		if(mpo_Editor->mpst_CurrentAction->ast_ActionItem[i_Res].pst_TrackList)
        {
            ULONG ulFat;
            ulFat = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Editor->mpst_CurrentAction->ast_ActionItem[i_Res].pst_TrackList);
			if(ulFat != BIG_C_InvalidIndex)
			{
	            EBRO_cl_Frame	*po_Browser;
	            po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
	            po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
	            po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ulFat), ulFat);
            }
        }
        break;

    /**/
    default:
        break;
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnModeChange(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CComboBox	*pcombo;
	int			isel;
	/*~~~~~~~~~~~~~~~~*/

	if(!mpo_Editor->mpst_CurrentActionItem) return;
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_MODE);
	isel = pcombo->GetCurSel();
	if(isel == -1) return;
	mpo_Editor->mpst_CurrentActionItem->uc_Flag &= 0xF1;
	mpo_Editor->mpst_CurrentActionItem->uc_Flag |= (isel << 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnFreqChange(void)
{
	/*~~~~~~~~~~~*/
	CEdit	*pedit;
	CString o_Text;
	int		freq;
	char	asz_Name[200];
	/*~~~~~~~~~~~*/

	if(!mpo_Editor->mpst_CurrentActionItem) return;
	pedit = (CEdit *) GetDlgItem(IDC_EDIT_FREQ);
	pedit->GetWindowText(o_Text);
	freq = L_atol((char *) (LPCSTR) o_Text);
	if(freq > 255) freq = 255;
	if(freq < 0) freq = 0;
	mpo_Editor->mpst_CurrentActionItem->uc_Frequency = freq;


	/* Length */
	if(mpo_Editor->mpst_CurrentActionItem->pst_TrackList)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EVE_tdst_Event	*pst_Event, *pst_Last;
		ULONG			ul_NumFrames;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_NumFrames = 0;
		pst_Event = mpo_Editor->mpst_CurrentActionItem->pst_TrackList->pst_AllTracks[0].pst_AllEvents;
		pst_Last = pst_Event + mpo_Editor->mpst_CurrentActionItem->pst_TrackList->pst_AllTracks[0].uw_NumEvents;
		for(; pst_Event < pst_Last; pst_Event++) ul_NumFrames += pst_Event->uw_NumFrames;
	
		sprintf(asz_Name, "%i", ul_NumFrames);
		pedit = (CEdit *) GetDlgItem(IDC_EDIT_LENGTH);
		pedit->SetWindowText(asz_Name);

		sprintf(asz_Name, "%0.3f", (float) ul_NumFrames * 1.0f / ((float) (((int) mpo_Editor->mpst_CurrentActionItem->uc_Frequency) * 60 / ACT_C_DefaultAnimFrequency)));
		pedit = (CEdit *) GetDlgItem(IDC_EDIT_SECONDS);
		pedit->SetWindowText(asz_Name);

	}
	else
	{
		pedit = (CEdit *) GetDlgItem(IDC_EDIT_LENGTH);
		pedit->SetWindowText("");
		pedit = (CEdit *) GetDlgItem(IDC_EDIT_SECONDS);
		pedit->SetWindowText("");
	}

	/* Average Speed */
	if(mpo_Editor->mpst_CurrentActionItem->pst_TrackList)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EVE_tdst_Event	*pst_Event, *pst_Last;
		ULONG			ul_NumFrames;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_NumFrames = 0;
		pst_Event = mpo_Editor->mpst_CurrentActionItem->pst_TrackList->pst_AllTracks[1].pst_AllEvents;
		pst_Last = pst_Event + mpo_Editor->mpst_CurrentActionItem->pst_TrackList->pst_AllTracks[1].uw_NumEvents;
		
		if((pst_Event->w_Flags & EVE_C_EventFlag_InterpolationKey) && (EVE_w_Event_InterpolationKey_GetType(pst_Event) & EVE_InterKeyType_Translation_Mask))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	*pst_PosInit, *pst_PosFinal, st_Move;
			float				f_Speed;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_PosInit = EVE_pst_Event_InterpolationKey_GetPos(pst_Event);
			pst_PosFinal = EVE_pst_Event_InterpolationKey_GetPos(pst_Last - 1);

			for(; pst_Event < pst_Last; pst_Event++) ul_NumFrames += pst_Event->uw_NumFrames;

			MATH_SubVector(&st_Move, pst_PosFinal, pst_PosInit);
			f_Speed = MATH_f_NormVector(&st_Move) / ((float) ul_NumFrames / ((float) (((int) mpo_Editor->mpst_CurrentActionItem->uc_Frequency) * 60 / ACT_C_DefaultAnimFrequency)));
			

			sprintf(asz_Name, "%0.2f", f_Speed);
			pedit = (CEdit *) GetDlgItem(IDC_EDIT_SPEED);
			pedit->SetWindowText(asz_Name);
		}
		else
		{
			pedit = (CEdit *) GetDlgItem(IDC_EDIT_SPEED);
			pedit->SetWindowText("");
		}		
	}
	else
	{
		pedit = (CEdit *) GetDlgItem(IDC_EDIT_SPEED);
		pedit->SetWindowText("");
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnLoopNumChange(void)
{
	/*~~~~~~~~~~~*/
	CEdit	*pedit;
	CString o_Text;
	int		freq;
	/*~~~~~~~~~~~*/

	if(!mpo_Editor->mpst_CurrentActionItem) return;
	pedit = (CEdit *) GetDlgItem(IDC_EDIT_LOOPNUM);
	pedit->GetWindowText(o_Text);
	freq = L_atol((char *) (LPCSTR) o_Text);
	if(freq > 255) freq = 255;
	if(freq < 0) freq = 0;
	mpo_Editor->mpst_CurrentActionItem->uc_Repetition = freq;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnZoneCheckClick(UINT _i_Num)
{
	if(!mpo_Editor->mpst_CurrentActionItem) return;
	if(((CButton *) GetDlgItem(_i_Num))->GetCheck())
		mpo_Editor->mpst_CurrentActionItem->uw_DesignFlags |= (1 << (_i_Num - IDC_CHECK_ZONE0));
	else
		mpo_Editor->mpst_CurrentActionItem->uw_DesignFlags &= ~(1 << (_i_Num - IDC_CHECK_ZONE0));
}

#ifdef JADEFUSION
extern BOOL	ACT_gb_ForceSaveListTracks;
#else
extern "C" BOOL	ACT_gb_ForceSaveListTracks;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnARAMClick()
{
	if(!mpo_Editor->mpst_CurrentActionKit) return;

	ACT_gb_ForceSaveListTracks = TRUE;

	MessageBox("You must SAVE THE ACTION KIT to confirm this modification. Saving the world is useless and inadequate", "Warning", MB_OK);
	if(((CButton *) GetDlgItem(IDC_CHECK_ARAM))->GetCheck())
		mpo_Editor->mpst_CurrentActionKit->ul_Flag |= 1;
	else
		mpo_Editor->mpst_CurrentActionKit->ul_Flag &= ~1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnCBitCheckClick(UINT _i_Num)
{
	if(!mpo_Editor->mpst_CurrentActionItem) return;
	if(((CButton *) GetDlgItem(_i_Num))->GetCheck())
		mpo_Editor->mpst_CurrentActionItem->uc_CustomBit |= (1 << (_i_Num - IDC_CHECK_CBIT0));
	else
		mpo_Editor->mpst_CurrentActionItem->uc_CustomBit &= ~(1 << (_i_Num - IDC_CHECK_CBIT0));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnIgnoreGravity(void)
{
	if(!mpo_Editor->mpst_CurrentActionItem) return;
	if(((CButton *) GetDlgItem(IDC_CHECK_IGNOREGRAVITY))->GetCheck())
		mpo_Editor->mpst_CurrentActionItem->uc_Flag |= ACT_C_ActionItemFlag_IgnoreGravity;
	else
		mpo_Editor->mpst_CurrentActionItem->uc_Flag &= ~ACT_C_ActionItemFlag_IgnoreGravity;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnDevelopped(void)
{
	if(!mpo_Editor->mpst_CurrentActionItem) return;
	if(((CButton *) GetDlgItem(IDC_CHECK_DEVELOPPED))->GetCheck())
		mpo_Editor->mpst_CurrentActionItem->uc_Flag |= ACT_C_ActionItemFlag_Developped;
	else
		mpo_Editor->mpst_CurrentActionItem->uc_Flag &= ~ACT_C_ActionItemFlag_Developped;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnIgnoreTraction(void)
{
	if(!mpo_Editor->mpst_CurrentActionItem) return;
	if(((CButton *) GetDlgItem(IDC_CHECK_IGNORETRACTION))->GetCheck())
		mpo_Editor->mpst_CurrentActionItem->uc_Flag |= ACT_C_ActionItemFlag_IgnoreTraction;
	else
		mpo_Editor->mpst_CurrentActionItem->uc_Flag &= ~ACT_C_ActionItemFlag_IgnoreTraction;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnIgnoreStream(void)
{
	if(!mpo_Editor->mpst_CurrentActionItem) return;
	if(((CButton *) GetDlgItem(IDC_CHECK_IGNORESTREAM))->GetCheck())
		mpo_Editor->mpst_CurrentActionItem->uc_Flag |= ACT_C_ActionItemFlag_IgnoreStream;
	else
		mpo_Editor->mpst_CurrentActionItem->uc_Flag &= ~ACT_C_ActionItemFlag_IgnoreStream;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnIgnoreVTrans(void)
{
	if(!mpo_Editor->mpst_CurrentActionItem) return;
	if(((CButton *) GetDlgItem(IDC_CHECK_IGNOREVTRANS))->GetCheck())
		mpo_Editor->mpst_CurrentActionItem->uc_Flag |= ACT_C_ActionItemFlag_IgnoreVerticalTranslation;
	else
		mpo_Editor->mpst_CurrentActionItem->uc_Flag &= ~ACT_C_ActionItemFlag_IgnoreVerticalTranslation;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnUpActionItem(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*plist;
	int					iSel;
	ACT_st_ActionItem	st_Swap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Editor->mpst_CurrentAction) return;
	plist = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	iSel = plist->GetCurSel();
	if(iSel == -1) return;
	if(iSel == 0) return;
	L_memcpy(&st_Swap, &mpo_Editor->mpst_CurrentAction->ast_ActionItem[iSel], sizeof(ACT_st_ActionItem));
	L_memcpy
	(
		&mpo_Editor->mpst_CurrentAction->ast_ActionItem[iSel],
		&mpo_Editor->mpst_CurrentAction->ast_ActionItem[iSel - 1],
		sizeof(ACT_st_ActionItem)
	);
	L_memcpy(&mpo_Editor->mpst_CurrentAction->ast_ActionItem[iSel - 1], &st_Swap, sizeof(ACT_st_ActionItem));

	SetAction();
	plist->SetCurSel(iSel - 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnDownActionItem(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*plist;
	int					iSel;
	ACT_st_ActionItem	st_Swap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Editor->mpst_CurrentAction) return;
	plist = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	iSel = plist->GetCurSel();
	if(iSel == -1) return;
	if(iSel == mpo_Editor->mpst_CurrentAction->uc_NumberOfActionItem - 1) return;
	L_memcpy(&st_Swap, &mpo_Editor->mpst_CurrentAction->ast_ActionItem[iSel], sizeof(ACT_st_ActionItem));
	L_memcpy
	(
		&mpo_Editor->mpst_CurrentAction->ast_ActionItem[iSel],
		&mpo_Editor->mpst_CurrentAction->ast_ActionItem[iSel + 1],
		sizeof(ACT_st_ActionItem)
	);
	L_memcpy(&mpo_Editor->mpst_CurrentAction->ast_ActionItem[iSel + 1], &st_Swap, sizeof(ACT_st_ActionItem));

	SetAction();
	plist->SetCurSel(iSel + 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnLoopChange(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CComboBox	*pcombo;
	int			isel;
	/*~~~~~~~~~~~~~~~~*/

	if(!mpo_Editor->mpst_CurrentAction) return;
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_LOOP);
	isel = pcombo->GetCurSel();
	if(isel == -1) return;
	mpo_Editor->mpst_CurrentAction->uc_ActionItemNumberForLoop = isel - 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnCopyFromActChange(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox	*pcombo;
	int			isel;
	BIG_INDEX	ul_TrackList;
	char		asz_Name[BIG_C_MaxLenName];
	char		*psz_Temp;
	int			index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_COPYFROMACT);
	isel = pcombo->GetCurSel();
	if(isel == -1) return;
	isel = pcombo->GetItemData(isel);
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_COPYFROMACTITEM);
	pcombo->ResetContent();

	if(mpo_Editor->mpst_CurrentActionKit->apst_Action[isel])
	{
		for(int i = 0; i < mpo_Editor->mpst_CurrentActionKit->apst_Action[isel]->uc_NumberOfActionItem; i++)
		{
			if(mpo_Editor->mpst_CurrentActionKit->apst_Action[isel]->ast_ActionItem[i].pst_TrackList && 
                (ul_TrackList = LOA_ul_SearchIndexWithAddress(
                (ULONG) mpo_Editor->mpst_CurrentActionKit->apst_Action[isel]->ast_ActionItem[i].pst_TrackList)) != BIG_C_InvalidIndex )
			{
				L_strcpy(asz_Name, BIG_NameFile(ul_TrackList));
				psz_Temp = L_strrchr(asz_Name, '.');
				if(psz_Temp) *psz_Temp = 0;
			}
			else
			{
				L_strcpy(asz_Name, "<No Track List>");
			}

			index = pcombo->InsertString(i, asz_Name);
			pcombo->SetItemData(index, i);
		}
	}

	pcombo->SetCurSel(0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::DeleteAllTrans(void)
{
	/*~~~~~~~~~~~~~~~*/
	CListBox	*pb;
	CListCtrl	*plist;
	/*~~~~~~~~~~~~~~~*/

	if
	(
		M_MF()->MessageBox
			(
				"This will delete all transitions of current action",
				"Are you sure ?",
				MB_ICONQUESTION | MB_YESNO
			) == IDNO
	) return;
	pb = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	int i=0;
	for(i = 0; i < mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction; i++)
	{
		ACT_RemoveTransition(mpo_Editor->mpst_CurrentActionKit, mpo_Editor->muw_CurrentAction, pb->GetCurSel(), i);
	}

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	i = plist->GetNextItem(-1, LVNI_SELECTED);
	SetActionKit();
	plist->SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnCopyFromOver(void)
{
	CommonCopyFrom(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnKillTo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl			*plist;
	int					iItem;
	int					i, j;
	CListBox			*pb;
	ACT_tdst_Transition *pst_Trans;
	char				msg[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mb_LockTrans) return;

	pb = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	iItem = pb->GetCurSel();
	if(iItem == -1) return;

	LINK_PrintStatusMsg("Start killing transition");
	for(i = 0; i < mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction; i++)
	{
		if(mpo_Editor->mpst_CurrentActionKit->apst_Action[i])
		{
			for(j = 0; j < mpo_Editor->mpst_CurrentActionKit->apst_Action[i]->uc_NumberOfActionItem; j++)
			{
				pst_Trans = ACT_pst_FindTransition
					(
						mpo_Editor->mpst_CurrentActionKit,
						i,
						j,
						mpo_Editor->muw_CurrentAction
					);
				if(pst_Trans)
				{
					sprintf(msg, "Action %d, Action Item %d", i, j);
					LINK_PrintStatusMsg(msg);
					ACT_RemoveTransition(mpo_Editor->mpst_CurrentActionKit, i, j, mpo_Editor->muw_CurrentAction);
				}
			}
		}
	}

	LINK_PrintStatusMsg("** End");

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	i = plist->GetNextItem(-1, LVNI_SELECTED);
	SetActionKit();
	plist->SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnViewTo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl			*plist;
	int					iItem;
	int					i, j, i_Act;
	CListBox			*pb;
	ACT_tdst_Transition *pst_Trans;
	char				msg[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	i_Act = plist->GetNextItem(-1, LVNI_SELECTED);
	if(i_Act == -1) return;

	pb = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	iItem = pb->GetCurSel();
	if(iItem == -1) return;

	LINK_PrintStatusMsg("Start finding transition");
	for(i = 0; i < mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction; i++)
	{
		if(mpo_Editor->mpst_CurrentActionKit->apst_Action[i])
		{
			for(j = 0; j < mpo_Editor->mpst_CurrentActionKit->apst_Action[i]->uc_NumberOfActionItem; j++)
			{
				pst_Trans = ACT_pst_FindTransition(mpo_Editor->mpst_CurrentActionKit, i, j, i_Act);
				if(pst_Trans)
				{
					sprintf
					(
						msg,
						"Action %d, Action Item %d (Transition is action %d), Blend %d",
						i,
						j,
						pst_Trans->uw_Action,
						pst_Trans->uc_Blend
					);
					LINK_PrintStatusMsg(msg);
				}
			}
		}
	}

	LINK_PrintStatusMsg("** End");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnUseTrans(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl			*plist;
	int					iItem;
	int					i, j, k, i_Act;
	CListBox			*pb;
	char				msg[1024];
	BAS_tdst_barray		*parray;
	ACT_tdst_Transition *ptrans;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	i_Act = plist->GetNextItem(-1, LVNI_SELECTED);
	if(i_Act == -1) return;

	pb = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	iItem = pb->GetCurSel();
	if(iItem == -1) return;

	LINK_PrintStatusMsg("Start finding transition");
	for(i = 0; i < mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction; i++)
	{
		if(mpo_Editor->mpst_CurrentActionKit->apst_Action[i])
		{
			for(j = 0; j < mpo_Editor->mpst_CurrentActionKit->apst_Action[i]->uc_NumberOfActionItem; j++)
			{
				parray = mpo_Editor->mpst_CurrentActionKit->apst_Action[i]->ast_ActionItem[j].pst_Transitions;
				if(parray)
				{
					for(k = 0; k < parray->num; k++)
					{
						ptrans = (ACT_tdst_Transition *) parray->base[k].ul_Val;
						if(ptrans->uw_Action == (ULONG) i_Act)
						{
							sprintf
							(
								msg,
								"Action %d, Action Item %d => Action %d, Blend %d",
								i,
								j,
								parray->base[k].ul_Key,
								ptrans->uc_Blend
							);
							LINK_PrintStatusMsg(msg);
						}
					}
				}
			}
		}
	}

	LINK_PrintStatusMsg("** End");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnCopySameAs(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl			*plist;
	CComboBox			*pcombo;
	CListBox			*pb;
	int					iSel, iAct;
	int					i, j;
	ACT_tdst_Transition *pst_Trans;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);

	/* Get action and item */
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_COPYSAMEAS);
	iSel = pcombo->GetCurSel();
	if(iSel == -1) return;
	iAct = pcombo->GetItemData(iSel);

	pb = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	if(pb->GetCurSel() == -1) return;

	for(i = 0; i < mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction; i++)
	{
		if(mpo_Editor->mpst_CurrentActionKit->apst_Action[i])
		{
			for(j = 0; j < mpo_Editor->mpst_CurrentActionKit->apst_Action[i]->uc_NumberOfActionItem; j++)
			{
				pst_Trans = ACT_pst_FindTransition(mpo_Editor->mpst_CurrentActionKit, i, j, iAct);
				if(pst_Trans)
				{
					ACT_SetTransition
					(
						mpo_Editor->mpst_CurrentActionKit,
						i,
						j,
						mpo_Editor->muw_CurrentAction,
						pst_Trans
					);
				}
			}
		}
	}

	i = plist->GetNextItem(-1, LVNI_SELECTED);
	SetActionKit();
	plist->SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::CommonCopyFrom(BOOL _b_Over)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox			*pcombo;
	CComboBox			*pcombo1;
	int					iSel, iAct, iItem;
	int					i;
	ACT_tdst_Transition *pst_Trans;
	ACT_tdst_Transition *pst_Trans1;
	CListBox			*pb;
	CListCtrl			*plist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);

	/* Get action and item */
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_COPYFROMACT);
	iSel = pcombo->GetCurSel();
	if(iSel == -1) return;
	iAct = pcombo->GetItemData(iSel);

	pcombo1 = (CComboBox *) GetDlgItem(IDC_COMBO_COPYFROMACTITEM);
	iItem = pcombo1->GetCurSel();
	if(iItem == -1) return;

	pb = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	if(pb->GetCurSel() == -1) return;

	for(i = 0; i < mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction; i++)
	{
		if(i == mpo_Editor->muw_CurrentAction) continue;
		pst_Trans = ACT_pst_FindTransition(mpo_Editor->mpst_CurrentActionKit, iAct, iItem, i);
		if(!pst_Trans) continue;
		pst_Trans1 = ACT_pst_FindTransition
			(
				mpo_Editor->mpst_CurrentActionKit,
				mpo_Editor->muw_CurrentAction,
				pb->GetCurSel(),
				i
			);
		if(pst_Trans1 && _b_Over)
		{
			ACT_RemoveTransition(mpo_Editor->mpst_CurrentActionKit, mpo_Editor->muw_CurrentAction, pb->GetCurSel(), i);
			pst_Trans1 = NULL;
		}

		if(!pst_Trans1)
		{
			ACT_SetTransition
			(
				mpo_Editor->mpst_CurrentActionKit,
				mpo_Editor->muw_CurrentAction,
				pb->GetCurSel(),
				i,
				pst_Trans
			);
		}
	}

	i = plist->GetNextItem(-1, LVNI_SELECTED);
	SetActionKit();
	pcombo->SetCurSel(iSel);
	OnCopyFromActChange();
	pcombo1->SetCurSel(iItem);
	plist->SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnCopyFrom(void)
{
	CommonCopyFrom(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::HasBeenMove(int _i_ActionUp, int _i_ActionDown)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j, k;
	BAS_tdst_barray		*parray;
	ACT_tdst_Transition *ptrans;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction; i++)
	{
		if(mpo_Editor->mpst_CurrentActionKit->apst_Action[i])
		{
			for(j = 0; j < mpo_Editor->mpst_CurrentActionKit->apst_Action[i]->uc_NumberOfActionItem; j++)
			{
				parray = mpo_Editor->mpst_CurrentActionKit->apst_Action[i]->ast_ActionItem[j].pst_Transitions;
				if(parray)
				{
					for(k = 0; k < parray->num; k++)
					{
						ptrans = (ACT_tdst_Transition *) parray->base[k].ul_Val;
						if(ptrans->uw_Action == (ULONG) _i_ActionUp)
							ptrans->uw_Action--;
						else if(ptrans->uw_Action == (ULONG) _i_ActionDown) ptrans->uw_Action++;

						if(parray->base[k].ul_Key == (ULONG) _i_ActionUp)
							parray->base[k].ul_Key--;
						else if(parray->base[k].ul_Key == (ULONG) _i_ActionDown) parray->base[k].ul_Key++;
					}

					BAS_bsort(parray);
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnUpAction(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl		*plist;
	int				iSel;
	ACT_st_Action	*pst_Swap;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Editor->mpst_CurrentActionKit) return;
	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	iSel = plist->GetNextItem(-1, LVNI_SELECTED);
	if(iSel == -1) return;
	if(iSel == 0) return;

	HasBeenMove(iSel, iSel - 1);

	pst_Swap = mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel];
	mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel] = mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel - 1];
	mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel - 1] = pst_Swap;
	mpo_Editor->muw_CurrentAction--;

	SetActionKit();
	plist->SetItemState(iSel - 1, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnDownAction(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl		*plist;
	int				iSel;
	ACT_st_Action	*pst_Swap;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Editor->mpst_CurrentActionKit) return;
	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	iSel = plist->GetNextItem(-1, LVNI_SELECTED);
	if(iSel == -1) return;
	if(iSel == mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction - 1) return;

	HasBeenMove(iSel + 1, iSel);

	pst_Swap = mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel];
	mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel] = mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel + 1];
	mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel + 1] = pst_Swap;
	mpo_Editor->muw_CurrentAction++;

	SetActionKit();
	plist->SetItemState(iSel + 1, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnHoleAction(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CListCtrl	*plist;
	int			iSel, i;
	/*~~~~~~~~~~~~~~~~*/

	ERR_X_Error
	(
		mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction != ACT_C_SizeOfActionKit,
		"Too many actions in action kit",
		NULL
	);

	if(!mpo_Editor->mpst_CurrentActionKit) return;
	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	iSel = plist->GetNextItem(-1, LVNI_SELECTED);
	if(iSel == -1) return;
	for(i = plist->GetItemCount() - 1; i >= iSel; i--) HasBeenMove(-1, i);

	L_memmove
	(
		&mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel + 1],
		&mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel],
		(mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction - iSel) * sizeof(ACT_st_Action *)
	);
	mpo_Editor->mpst_CurrentActionKit->apst_Action[iSel] = NULL;
	mpo_Editor->mpst_CurrentActionKit->uw_NumberOfAction++;
	mpo_Editor->muw_CurrentAction++;

	SetActionKit();
	plist->SetItemState(iSel, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnBrowseAnim(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Choose Anim", 0, 0, 1, NULL, "*"EDI_Csz_ExtEventAllsTracks);
	CString				o_Temp;
	BIG_INDEX			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_File.mb_UseFavorite = TRUE;
	if(o_File.DoModal() == IDOK)
	{
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			if
			(
				(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtAnimation))
			&&	(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtEventAllsTracks))
			)
			{
				ERR_X_ForceError("This is not an animation file", NULL);
			}
			else
			{
				SetAnim(BIG_FileKey(ul_Index));
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnBrowseShape(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Choose Shape", 0, 0, 1, NULL, "*"EDI_Csz_ExtShape);
	CString				o_Temp;
	BIG_INDEX			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_File.mb_UseFavorite = TRUE;

	if(o_File.DoModal() == IDOK)
	{
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			if(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtShape))
			{
				ERR_X_ForceError("This is not an Shape file", NULL);
			}
			else
			{
				SetShape(BIG_FileKey(ul_Index));
			}
		}
		else
		{
			if(mpo_Editor->mpst_CurrentActionItem->pst_Shape)
			{
				ANI_FreeShape(&mpo_Editor->mpst_CurrentActionItem->pst_Shape);
				mpo_Editor->mpst_CurrentActionItem->pst_Shape = NULL;
				SetAction();
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::SetAnim(BIG_INDEX _ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_ListTracks *pst_TrackList, *pst_PreviousTrackList;
	BIG_KEY				ul_TrackListKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Editor->mpst_CurrentActionItem) return;
	pst_PreviousTrackList = mpo_Editor->mpst_CurrentActionItem->pst_TrackList;

	ul_TrackListKey = ANI_DataToTrack(_ul_File);

	/* Load animation */
	LOA_MakeFileRef(ul_TrackListKey, (ULONG *) &pst_TrackList, EVE_ul_CallbackLoadListTracks, LOA_C_MustExists);
	LOA_Resolve();

	if(!pst_TrackList || (pst_TrackList == pst_PreviousTrackList)) return;

	/* Replace animation data */
	if(pst_PreviousTrackList) EVE_DeleteListTracks(pst_PreviousTrackList);
	pst_TrackList->ul_NbOfInstances++;
	mpo_Editor->mpst_CurrentActionItem->pst_TrackList = pst_TrackList;
	SetAction();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::SetShape(BIG_INDEX _ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_tdst_Shape	*pst_Shape, *pst_PreviousShape;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Editor->mpst_CurrentActionItem) return;
	pst_PreviousShape = mpo_Editor->mpst_CurrentActionItem->pst_Shape;

	/* Load animation */
	LOA_MakeFileRef(_ul_File, (ULONG *) &pst_Shape, ANI_ul_ShapeCallback, LOA_C_MustExists);
	LOA_Resolve();

	if(!pst_Shape || (pst_Shape == pst_PreviousShape)) return;

	/* Replace Shape */
	if(pst_PreviousShape) ANI_FreeShape(&mpo_Editor->mpst_CurrentActionItem->pst_Shape);
	pst_Shape->uw_Counter++;
	mpo_Editor->mpst_CurrentActionItem->pst_Shape = pst_Shape;
	SetAction();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::EnableTrans(void)
{
	GetDlgItem(IDC_COMBO_TRANSACT)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_BUTTON_TRANSMEME)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_EDIT_BLENDTIME)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_CHECK_BLENDPROGRESSIVE)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_CHECK_BLENDPROGRESSIVEINV)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_CHECK_BLENDFREEZE)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_CHECK_BLENDFREEZEMB)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_CHECK_BLENDSTOCK)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_CHECK_BLENDOFF)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_CHECK_DEFAULT)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_CHECK_NEXTDEFAULT)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_COMBO_COPYFROMACT)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_COMBO_COPYFROMACTITEM)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_BUTTON_COPYFROM)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_BUTTON_COPYFROMOVER)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_BUTTON_KILLTO)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_ACTION_HOLE)->EnableWindow(mb_LockTrans ? FALSE : TRUE);
	GetDlgItem(IDC_ACTION_UP)->EnableWindow(mb_LockTrans ? FALSE : TRUE);
	GetDlgItem(IDC_ACTION_DOWN)->EnableWindow(mb_LockTrans ? FALSE : TRUE);
	GetDlgItem(IDC_BUTTON_KILLALLTRANS)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_COMBO_COPYSAMEAS)->EnableWindow(mb_LockTrans);
	GetDlgItem(IDC_BUTTON_COPYSAMEAS)->EnableWindow(mb_LockTrans);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnLockTrans(void)
{
	/*~~~~~~~~~~~~~~~*/
	CListCtrl	*plist;
	int			item;
	/*~~~~~~~~~~~~~~~*/

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	item = plist->GetNextItem(-1, LVNI_SELECTED);
	if(item == -1) plist->SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

	if(((CButton *) GetDlgItem(IDC_BUTTON_LOCK))->GetCheck())
	{
		plist->SetBkColor(GetSysColor(COLOR_BTNFACE));
		plist->SetTextBkColor(GetSysColor(COLOR_BTNFACE));
		item = plist->GetNextItem(-1, LVNI_SELECTED);
		mi_SelWhenTrans = item;
		mpo_Editor->muw_CurrentAction = item;
		mb_LockTrans = TRUE;
		SetActionKit();
		plist->SetItemState(item, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}
	else
	{
		plist->SetBkColor(GetSysColor(COLOR_WINDOW));
		plist->SetTextBkColor(GetSysColor(COLOR_WINDOW));
		mb_LockTrans = FALSE;
		SetActionKit();
		plist->SetItemState(mi_SelWhenTrans, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}

	EnableTrans();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnTransMe(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox		*pcombo;
	int				item, action;
	CListCtrl		*plist;
	EMEN_cl_SubMenu o_Menu(FALSE);
	CPoint			pt;
	char			sz[100];
	int				i_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mb_LockTrans) return;
	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	action = plist->GetNextItem(-1, LVNI_SELECTED);
	if(action == -1) return;
	if(plist->GetItemData(action) == BIG_C_InvalidIndex) return;
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_TRANSACT);
	for(item = 0; item < pcombo->GetCount(); item++)
	{
		if(pcombo->GetItemData(item) == (ULONG) action)
		{
			pcombo->SetCurSel(item);
			break;
		}
	}

	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 1, TRUE, "Me - No Blend", -1);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 2, TRUE, "Me - Blend 4", -1);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 3, TRUE, "Me - Blend 6", -1);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 4, TRUE, "Me - Blend 10", -1);
	M_MF()->AddPopupMenuAction(mpo_Editor, &o_Menu, 5, TRUE, "Me - Blend 15", -1);
	GetCursorPos(&pt);
	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	switch(i_Res)
	{
	case 1:
		break;
	case 2:
		sprintf(sz, "%d", 4);
		GetDlgItem(IDC_EDIT_BLENDTIME)->SetWindowText(sz);
		break;
	case 3:
		sprintf(sz, "%d", 6);
		GetDlgItem(IDC_EDIT_BLENDTIME)->SetWindowText(sz);
		break;
	case 4:
		sprintf(sz, "%d", 10);
		GetDlgItem(IDC_EDIT_BLENDTIME)->SetWindowText(sz);
		break;
	case 5:
		sprintf(sz, "%d", 15);
		GetDlgItem(IDC_EDIT_BLENDTIME)->SetWindowText(sz);
		break;
	}

	OnTransActChange();
	plist->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnTransActNone(void)
{
	/*~~~~~~~~~~~~~~~*/
	CListCtrl	*plist;
	CListBox	*pb;
	int			item;
	/*~~~~~~~~~~~~~~~*/

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	item = plist->GetNextItem(-1, LVNI_SELECTED);
	if(item == -1) return;
	pb = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	if(pb->GetCurSel() == -1) return;
	ACT_RemoveTransition(mpo_Editor->mpst_CurrentActionKit, mpo_Editor->muw_CurrentAction, pb->GetCurSel(), item);
	item = plist->GetNextItem(-1, LVNI_SELECTED);
	SetActionKit();
	plist->SetItemState(item, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnTransActChange(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl			*plist;
	CComboBox			*pcombo;
	CListBox			*pb;
	int					item;
	ACT_tdst_Transition st_Trans;
	char				sz_Value[10];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mb_LockTrans) return;

	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_TRANSACT);
	if(pcombo->GetCurSel() == -1) return;
	st_Trans.uw_Action = (USHORT) pcombo->GetItemData(pcombo->GetCurSel());
	GetDlgItem(IDC_EDIT_BLENDTIME)->GetWindowText(sz_Value, 10);
	st_Trans.uc_Blend = (UCHAR) atoi(sz_Value);
	st_Trans.uc_Flag = ((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVE))->GetCheck() ? ACT_C_TF_BlendProgressive : 0;
	st_Trans.uc_Flag |= ((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVEINV))->GetCheck() ? ACT_C_TF_BlendProgressiveInv : 0;
	st_Trans.uc_Flag |= ((CButton *) GetDlgItem(IDC_CHECK_BLENDFREEZE))->GetCheck() ? ACT_C_TF_BlendFreezeBones : 0;
	st_Trans.uc_Flag |= ((CButton *) GetDlgItem(IDC_CHECK_BLENDFREEZEMB))->GetCheck() ? ACT_C_TF_BlendFreezeMagicBox : 0;
	st_Trans.uc_Flag |= ((CButton *) GetDlgItem(IDC_CHECK_BLENDSTOCK))->GetCheck() ? ACT_C_TF_BlendStock : 0;
	st_Trans.uc_Flag |= ((CButton *) GetDlgItem(IDC_CHECK_BLENDOFF))->GetCheck() ? ACT_C_TF_BlendBetweenActionItem : 0;
	st_Trans.uc_Flag |= ((CButton *) GetDlgItem(IDC_CHECK_DEFAULT))->GetCheck() ? ACT_C_TF_DefaultTransition : 0;
	st_Trans.uc_Flag |= ((CButton *) GetDlgItem(IDC_CHECK_NEXTDEFAULT))->GetCheck() ? ACT_C_TF_NextDefaultTransition : 0;

	item = plist->GetNextItem(-1, LVNI_SELECTED);
	if(item == -1) return;

	pb = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	if(pb->GetCurSel() == -1) return;

	/* Remove transition */
	if(st_Trans.uw_Action == 0xFFFF)
	{
		ACT_RemoveTransition(mpo_Editor->mpst_CurrentActionKit, mpo_Editor->muw_CurrentAction, pb->GetCurSel(), item);
	}

	/* Set transition */
	else
	{
		ACT_SetTransition
		(
			mpo_Editor->mpst_CurrentActionKit,
			mpo_Editor->muw_CurrentAction,
			pb->GetCurSel(),
			item,
			&st_Trans
		);
	}

	item = plist->GetNextItem(-1, LVNI_SELECTED);
	SetActionKit();
	plist->SetItemState(item, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnTransBlendProgressive(void)
{
	if(((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVE))->GetCheck())
	{
		((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVEINV))->SetCheck(0);
	}

	OnTransActChange();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnTransBlendProgressiveInv(void)
{
	if(((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVEINV))->GetCheck())
	{
		((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVE))->SetCheck(0);
	}

	OnTransActChange();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::OnBlendTimeChange(void)
{
	OnTransActChange();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ACT_tdst_Transition *EACT_cl_View::GetTrans(int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*pb;
	ACT_tdst_Transition *pst_Trans;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pb = (CListBox *) GetDlgItem(IDC_LIST_ACTIONITEMS);
	if(pb->GetCurSel() == -1) return NULL;
	pst_Trans = ACT_pst_FindTransition
		(
			mpo_Editor->mpst_CurrentActionKit,
			mpo_Editor->muw_CurrentAction,
			pb->GetCurSel(),
			i
		);
	return pst_Trans;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EACT_cl_View::SetTrans(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl			*plist;
	ACT_tdst_Transition *pst_Trans;
	CComboBox			*pcombo;
	int					item;
	char				sz_Value[10];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pcombo = (CComboBox *) GetDlgItem(IDC_COMBO_TRANSACT);
	plist = (CListCtrl *) GetDlgItem(IDC_LIST_ACTIONS);
	item = plist->GetNextItem(-1, LVNI_SELECTED);
	if(item == -1) return;

	if(!mb_LockTrans)
	{
		pcombo->SetCurSel(-1);
		GetDlgItem(IDC_EDIT_BLENDTIME)->SetWindowText("");
		((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVE))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVEINV))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDFREEZE))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDFREEZEMB))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDSTOCK))->SetCheck(1);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDOFF))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_DEFAULT))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_NEXTDEFAULT))->SetCheck(0);
		return;
	}

	pst_Trans = GetTrans(item);
	if(pst_Trans)
	{
		for(item = 0; item < pcombo->GetCount(); item++)
		{
			if(pcombo->GetItemData(item) == pst_Trans->uw_Action)
			{
				pcombo->SetCurSel(item);
				break;
			}
		}

		GetDlgItem(IDC_EDIT_BLENDTIME)->SetWindowText(_itoa(pst_Trans->uc_Blend, sz_Value, 10));
		((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVE))->SetCheck(pst_Trans->uc_Flag & ACT_C_TF_BlendProgressive);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVEINV))->SetCheck((pst_Trans->uc_Flag & ACT_C_TF_BlendProgressiveInv) ? 1 : 0);

		/* Delete Freeze Bones */
//		((CButton *) GetDlgItem(IDC_CHECK_BLENDFREEZE))->SetCheck((pst_Trans->uc_Flag & ACT_C_TF_BlendFreezeBones) ? 1 : 0);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDFREEZE))->SetCheck(0);

		((CButton *) GetDlgItem(IDC_CHECK_BLENDFREEZEMB))->SetCheck((pst_Trans->uc_Flag & ACT_C_TF_BlendFreezeMagicBox) ? 1 : 0);

		/* Force Stock Mode */
//		((CButton *) GetDlgItem(IDC_CHECK_BLENDSTOCK))->SetCheck((pst_Trans->uc_Flag & ACT_C_TF_BlendStock) ? 1 : 0);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDSTOCK))->SetCheck(1);

		((CButton *) GetDlgItem(IDC_CHECK_BLENDOFF))->SetCheck((pst_Trans->uc_Flag & ACT_C_TF_BlendBetweenActionItem) ? 1 : 0);
		((CButton *) GetDlgItem(IDC_CHECK_DEFAULT))->SetCheck((pst_Trans->uc_Flag & ACT_C_TF_DefaultTransition) ? 1 : 0);
		((CButton *) GetDlgItem(IDC_CHECK_NEXTDEFAULT))->SetCheck((pst_Trans->uc_Flag & ACT_C_TF_NextDefaultTransition) ? 1 : 0);
	}
	else
	{
		pcombo->SetCurSel(0);
		GetDlgItem(IDC_EDIT_BLENDTIME)->SetWindowText("");
		((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVE))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDPROGRESSIVEINV))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDFREEZE))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDFREEZEMB))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDSTOCK))->SetCheck(1);
		((CButton *) GetDlgItem(IDC_CHECK_BLENDOFF))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_DEFAULT))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_CHECK_NEXTDEFAULT))->SetCheck(0);
	}
}

#endif /* ACTIVE_EDITORS */

