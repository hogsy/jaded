/*$T DIAselection_dlg.cpp GC 1.134 09/21/04 13:12:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "CCheckList.h"
#include "DIAlogs/DIAselection_dlg.h"
#include "DIAlogs/DIApostit_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "LINKs/LINKtoed.h"
#include "BASe/CLIbrary/CLImem.h"
#include "Res/Res.h" 
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/GRId/GRI_struct.h"

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"


#include "SELection/SELection.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "SELection/SELection.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "F3Dframe/F3Dframe.h"
#include "ENGine/Sources/WORld/WORvars.h"
#include "Editors/Sources/OUTput/OUTframe.h"
#include "EDImsg.h"
#include "DIAlogs/DIAcompletion_dlg.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
extern char EDI_gaz_Message[4096];

#define PIT_READ	0x00000001
#define PIT_NREAD	0x00000002
#define PIT_ME		0x00000010
#define PIT_NME		0x00000020
#define PIT_ALL		0xFFFFFFFF

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

/*$off*/
static int sai_SortRadioId[3] = { IDC_RADIO_SORTALPHA, IDC_RADIO_SORTIF, IDC_RADIO_SORTGRO };
static int sai_IFFilterId[32] =
  {
	IDC_CHECK_IF0,	IDC_CHECK_IF1,	IDC_CHECK_IF2,	0,	IDC_CHECK_IF4,
	IDC_CHECK_IF5,	IDC_CHECK_IF6,	IDC_CHECK_IF7,	IDC_CHECK_IF8,	IDC_CHECK_IF9,
	IDC_CHECK_IF10, 0, 0, 0, IDC_CHECK_IF14,
	IDC_CHECK_IF15, IDC_CHECK_IF16, IDC_CHECK_IF17, IDC_CHECK_IF18, IDC_CHECK_IF19,
	IDC_CHECK_IF20, 0, IDC_CHECK_IF22, IDC_CHECK_IF23, 0, IDC_CHECK_IF25, 0, IDC_CHECK_IF27, 0, 0, 0, 0
};
static int sai_GroFilterId[7] =
  {
	IDC_CHECK_GRO0, IDC_CHECK_GRO1, IDC_CHECK_GRO2, IDC_CHECK_GRO3, IDC_CHECK_GRO4, IDC_CHECK_GRO5, IDC_CHECK_GRO6
};

static int sai_InvisibleFilterId[16] =
{
	IDC_CHECK_INVISIBLE0, IDC_CHECK_INVISIBLE1, IDC_CHECK_INVISIBLE2, IDC_CHECK_INVISIBLE3,
	IDC_CHECK_INVISIBLE4, IDC_CHECK_INVISIBLE5, IDC_CHECK_INVISIBLE6, IDC_CHECK_INVISIBLE7,
	IDC_CHECK_INVISIBLE8, IDC_CHECK_INVISIBLE9, IDC_CHECK_INVISIBLE10, IDC_CHECK_INVISIBLE11,
	IDC_CHECK_INVISIBLE12, IDC_CHECK_INVISIBLE13, IDC_CHECK_INVISIBLE14, IDC_CHECK_INVISIBLE15

};

static int sai_GroFilterMask[ GRO_Cl_NumberOfInterfaces ] ={ 1, 2, 1, 1, 1, 1, 4, 1, 8, 16, 32, 64 };
/*$on*/

/*$4
 ***********************************************************************************************************************
    EDIA_cl_SelectionDialog Message map
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_SelectionDialog, EDIA_cl_BaseDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_IF0, IDC_CHECK_IF27, OnIFCheckClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_GRO0, IDC_CHECK_GRO6, OnGroCheckClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_INVISIBLE0, IDC_CHECK_INVISIBLE15, OnInvisibleFCheckClick)
	ON_LBN_SELCHANGE(IDC_LIST_OBJ, OnSelChange)
	ON_LBN_DBLCLK(IDC_LIST_OBJ, OnListDblClk)
	ON_BN_CLICKED(IDC_BUTTON1_NONE, OnBtNoneClicked)
	ON_BN_CLICKED(IDC_BUTTON1_ALL, OnBtAllClicked)
	ON_BN_CLICKED(IDC_BUTTON1_INVERT, OnBtInvertClicked)
	ON_BN_CLICKED(IDC_INVERS1, OnBtInvers1)
	ON_BN_CLICKED(IDC_INVERS2, OnBtInvers2)
	ON_BN_CLICKED(IDC_INVERS3, OnBtInvers3)
	ON_BN_CLICKED(IDC_CLEAR1, OnBtClear1)
	ON_BN_CLICKED(IDC_CLEAR2, OnBtClear2)
	ON_BN_CLICKED(IDC_CLEAR3, OnBtClear3)
	ON_BN_CLICKED(IDC_HIDE, OnHide)
	ON_BN_CLICKED(IDC_SHOW, OnShow)
	ON_BN_CLICKED(IDC_WIRED, OnWired)
	ON_BN_CLICKED(IDC_NOWIRED, OnNoWired)
	ON_BN_CLICKED(IDC_CHECK_HIDDEN, OnHidden)
	ON_BN_CLICKED(IDC_CHECK_NOTHIDDEN, OnNotHidden)
	ON_BN_CLICKED(IDC_CHECK_BONES, OnBones)
	ON_BN_CLICKED(IDC_CHECK_PREFAB, OnPrefab)
	ON_BN_CLICKED(IDC_CHECK_SPG2GRIDGEN, OnSpg2Grid)
	ON_BN_CLICKED(IDC_CHECK_PREFABSON, OnPrefabSon)
	ON_BN_CLICKED(IDC_CHECK_ACT, OnShowAct)
	ON_BN_CLICKED(IDC_CHECK_NOTACT, OnShowNotAct)
	ON_BN_CLICKED(IDC_CHECK_VIS, OnShowVis)
	ON_BN_CLICKED(IDC_CHECK_NOTVIS, OnShowNotVis)
	ON_BN_CLICKED(IDC_CHECK_MERGED, OnMerged)
	ON_BN_CLICKED(IDC_CHECK_NOTMERGED, OnNotMerged)
	ON_BN_CLICKED(IDC_CLOSESEL, OnCloseSel)
	ON_BN_CLICKED(IDC_MINIMIZE, OnMinimize)
	ON_BN_CLICKED(IDC_CHECK_SELECTIONTYPE, OnSelectionType)
	ON_BN_CLICKED(IDC_TOOLBOX, OnToolBox)
	ON_BN_CLICKED(IDC_BUTTON_UPDATEGRP, OnUpdateGroup)
	ON_BN_CLICKED(IDC_BUTTON_CLEARGOS, OnClearGos)
	ON_BN_CLICKED(IDC_BUTTON_ADD2GRP, OnAdd2Group)
	ON_BN_CLICKED(IDC_CHECKDM_DRAWIT, OnDrawIt)
	ON_BN_CLICKED(IDC_CHECKDM_NOTDRAWIT, OnNotDrawIt)
	ON_BN_CLICKED(IDC_SELECTION_NOTINSECTO, OnNotInSecto)
	ON_BN_CLICKED(IDC_MODELS, OnAIModels)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnGroupSel)
	ON_CBN_SELCHANGE(IDC_COMBO_GOS, OnGosSel)
	ON_EN_KILLFOCUS(IDC_EDIT_GAOF, OnGaoF)
	ON_EN_CHANGE(IDC_EDIT_GAOF, OnGaoF)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnChangePane)
	ON_LBN_SELCHANGE(IDC_LIST_GROUPS, OnSelChangeGroups)
	ON_LBN_DBLCLK(IDC_LIST_NETWORKS, OnNetDblClk)
	ON_LBN_DBLCLK(IDC_LIST_POSTIT, OnPostItDblClk)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    EDIA_cl_SelectionDialog implementation
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SelectionDialog::EDIA_cl_SelectionDialog(F3D_cl_View *_po_View) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_SELECTION)
{
	mpo_View = _po_View;
	mpst_World = _po_View->mst_WinHandles.pst_World;
	ml_SortType = EDIA_Selection_SortAlpha;
	mul_IFFilter = 0xFFFFFFFF;
	mb_Merge = TRUE;
	mb_NotMerge = TRUE;
	mul_GroFilter = 0xFFFFFFFF;
	mul_InvisibleFilter = 0xFFFF;
	mdc_Sel = NULL;
	mb_ShowHidden = TRUE;
	mb_ShowNotHidden = TRUE;
	mb_ShowBones = FALSE;
	mb_ShowPrefab = TRUE;
	mb_ShowSpg2GridGen = TRUE;
	mb_ShowPrefabSon = TRUE;
	mb_LockUpdate = FALSE;
	mb_ShowAct = TRUE;
	mb_ShowNotAct = TRUE;
	mb_ShowVis = TRUE;
	mb_ShowNotVis = TRUE;
	mb_DrawMask_DrawIt = TRUE;
	mb_DrawMask_NotDrawIt = TRUE;
	mb_NotInSecto = FALSE;
	mul_FilterPostIt = PIT_ALL;

	mao_SelectionButtonBmp[0].LoadBitmap(IDB_BITMAP_CROSSINGSELECTION);
	mao_SelectionButtonBmp[1].LoadBitmap(IDB_BITMAP_WINDOWSELECTION);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SelectionDialog::~EDIA_cl_SelectionDialog(void)
{
	if(mdc_Sel) L_free(mdc_Sel);
	mpo_View->mpo_SelectionDialog = NULL;

	mao_SelectionButtonBmp[0].DeleteObject();
	mao_SelectionButtonBmp[1].DeleteObject();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ControlKeyIsPressed = 0;
BOOL EDIA_cl_SelectionDialog::PreTranslateMessage(MSG *pmsg)
{
	if(GetFocus() && (GetFocus()->GetParent() == GetDlgItem(IDC_COMBO1)))
	{
		if((pmsg->message == WM_KEYDOWN) && (pmsg->wParam == VK_RETURN))
		{
			GetParent()->SetFocus();
			OnNewGroup(TRUE);
			return TRUE;
		}
	}

	if(GetFocus() && (GetFocus() == GetDlgItem(IDC_EDIT_GAOF)))
	{
		if((pmsg->message == WM_KEYDOWN) && (pmsg->wParam == VK_RETURN))
		{
			GetParent()->SetFocus();
			return TRUE;
		}
	}

	ControlKeyIsPressed = 0;
	if(GetAsyncKeyState(VK_CONTROL) < 0) ControlKeyIsPressed = 1;
	if(GetFocus() && (GetFocus()->GetParent() == GetDlgItem(IDC_COMBO_GOS)))
	{
		if((pmsg->message == WM_KEYDOWN) && (pmsg->wParam == VK_RETURN))
		{
			GetParent()->SetFocus();
			OnNewGos();
			return TRUE;
		}
	}


	if(pmsg->hwnd == GetDlgItem(IDC_LIST_NETWORKS)->GetSafeHwnd())
	{
		if(pmsg->message == WM_RBUTTONDOWN)
		{
			MenuNetwork();
			return TRUE;
		}
	}

	if(pmsg->hwnd == GetDlgItem(IDC_LIST_POSTIT)->GetSafeHwnd())
	{
		if(pmsg->message == WM_RBUTTONDOWN)
		{
			MenuPostIt();
			return TRUE;
		}
	}

	if(pmsg->hwnd == GetDlgItem(IDC_LIST_OBJ)->GetSafeHwnd())
	{
		if(pmsg->message == WM_RBUTTONDOWN)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EOUT_cl_Frame		*po_EOUT;
			OBJ_tdst_GameObject *pst_GO;
			CListBox			*LB;
			CPoint				point;
			BOOL				b_Multi;
			int					i;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			LB = &mo_ListBox;
			if(LB->GetSelCount() == 1)
				b_Multi = FALSE;
			else
				b_Multi = TRUE;

			for(i = 0; i < LB->GetCount(); i++)
			{
				if(mdc_Sel[i] == 1)
				{
					pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
					GetCursorPos(&point);
					po_EOUT = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
					po_EOUT->OnGaoSelect(pst_GO, b_Multi, point);
					return TRUE;
				}
			}
		}
	}

	if((pmsg->message == WM_KEYDOWN) && (GetAsyncKeyState(VK_SHIFT) < 0) && GetFocus() == GetDlgItem(IDC_LIST_OBJ))
		return TRUE;
	if((pmsg->message == WM_KEYDOWN) && (pmsg->wParam == VK_DELETE) && GetFocus() == GetDlgItem(IDC_LIST_OBJ))
		return TRUE;

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::DoDataExchange(CDataExchange *pDX)
{
	EDIA_cl_BaseDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_OBJ, mo_ListBox);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_SelectionDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTabCtrl	*ptab;
	CListBox	*LB, *LB1, *LB2, *LB3;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Base class call */
	EDIA_cl_BaseDialog::OnInitDialog();

	UpdateGroCheck();
	UpdateInvisibleCheck();
	UpdateIFCheck();
	UpdateList();

	ptab = (CTabCtrl *) GetDlgItem(IDC_TAB1);
	ptab->InsertItem(0, "Objects", 0);
	ptab->InsertItem(1, "Groups", 0);
	ptab->InsertItem(2, "Networks", 0);
	ptab->InsertItem(3, "PostIt", 0);

	if(mpo_View->mpo_Frame->mb_SelMinimize)
		GetDlgItem(IDC_MINIMIZE)->SetWindowText("+");
	else
		GetDlgItem(IDC_MINIMIZE)->SetWindowText("-");

	LB = &mo_ListBox;
	LB1 = (CListBox *) GetDlgItem(IDC_LIST_GROUPS);
	LB2 = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
	LB3 = (CListBox *) GetDlgItem(IDC_LIST_POSTIT);
	LB->BringWindowToTop();
	LB1->BringWindowToTop();
	LB2->BringWindowToTop();
	LB3->BringWindowToTop();

	UpdateSelectionButton();

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SelectionDialog::OnEraseBkgnd(CDC *pdc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*LB, *LB1, *LB2, *LB3;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LB = &mo_ListBox;
	LB1 = (CListBox *) GetDlgItem(IDC_LIST_GROUPS);
	LB2 = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
	LB3 = (CListBox *) GetDlgItem(IDC_LIST_POSTIT);
	LB->Invalidate();
	LB1->Invalidate();
	LB2->Invalidate();
	LB3->Invalidate();
	LB->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW | RDW_FRAME);
	LB1->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW | RDW_FRAME);
	LB2->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW | RDW_FRAME);
	LB3->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW | RDW_FRAME);
	return CDialog::OnEraseBkgnd(pdc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnSize(UINT, int, int cy)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTabCtrl	*tab;
	CListBox	*LB;
	CRect		o_Rect, o_Rect1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetParent()->GetClientRect(&o_Rect);

	tab = (CTabCtrl *) GetDlgItem(IDC_TAB1);
	if(!tab) return;
	tab->GetWindowRect(&o_Rect1);
	ScreenToClient(&o_Rect1);
	o_Rect1.bottom = o_Rect.bottom - 10;
	tab->MoveWindow(o_Rect1);

	/* Objects */
	LB = &mo_ListBox;
	if(!LB) return;
	o_Rect1.left += 5;
	o_Rect1.top += 5;
	o_Rect1.bottom -= 25;
	o_Rect1.right -= 5;
	LB->MoveWindow(o_Rect1);

	/* Groups */
	LB = (CListBox *) GetDlgItem(IDC_LIST_GROUPS);
	LB->MoveWindow(o_Rect1);

	/* Networks */
	LB = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
	LB->MoveWindow(o_Rect1);

	/* PostIt */
	LB = (CListBox *) GetDlgItem(IDC_LIST_POSTIT);
	LB->MoveWindow(o_Rect1);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::UpdateSelectionButton(void)
{
	/*~~~~~~~~~~~~~~~*/
	int		i_Bmp;
	CButton *po_Button;
	/*~~~~~~~~~~~~~~~*/

	i_Bmp = mpo_View->mst_WinHandles.pst_DisplayData->pst_PickingBuffer->ul_UserFlags & SOFT_Cul_PBF_WindowSelection;
	i_Bmp = i_Bmp ? 1 : 0;
	po_Button = (CButton *) GetDlgItem(IDC_CHECK_SELECTIONTYPE);
	po_Button->SetBitmap((HBITMAP) mao_SelectionButtonBmp[i_Bmp].GetSafeHandle());
	po_Button->SetCheck(i_Bmp);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::UpdateIFCheck(void)
{
	/*~~~~~~~*/
	ULONG	ul;
	int		i;
	/*~~~~~~~*/

	/* Special */
	if(mb_ShowNotHidden)
		((CButton *) GetDlgItem(IDC_CHECK_NOTHIDDEN))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_NOTHIDDEN))->SetCheck(0);
	if(mb_ShowHidden)
		((CButton *) GetDlgItem(IDC_CHECK_HIDDEN))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_HIDDEN))->SetCheck(0);
	if(mb_ShowBones)
		((CButton *) GetDlgItem(IDC_CHECK_BONES))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_BONES))->SetCheck(0);
	if(mb_ShowPrefab)
		((CButton *) GetDlgItem(IDC_CHECK_PREFAB))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_PREFAB))->SetCheck(0);
	if(mb_ShowSpg2GridGen)
		((CButton *) GetDlgItem(IDC_CHECK_SPG2GRIDGEN))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_SPG2GRIDGEN))->SetCheck(0);
	
	if(mb_ShowPrefabSon)
		((CButton *) GetDlgItem(IDC_CHECK_PREFABSON))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_PREFABSON))->SetCheck(0);
	if(mb_ShowAct)
		((CButton *) GetDlgItem(IDC_CHECK_ACT))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_ACT))->SetCheck(0);
	if(mb_ShowNotAct)
		((CButton *) GetDlgItem(IDC_CHECK_NOTACT))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_NOTACT))->SetCheck(0);
	if(mb_ShowVis)
		((CButton *) GetDlgItem(IDC_CHECK_VIS))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_VIS))->SetCheck(0);
	if(mb_ShowNotVis)
		((CButton *) GetDlgItem(IDC_CHECK_NOTVIS))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_NOTVIS))->SetCheck(0);

	if(mb_Merge)
		((CButton *) GetDlgItem(IDC_CHECK_MERGED))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_MERGED))->SetCheck(0);

	if(mb_NotMerge)
		((CButton *) GetDlgItem(IDC_CHECK_NOTMERGED))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_CHECK_NOTMERGED))->SetCheck(0);

	ul = mul_IFFilter;
	for(i = 0; i < 32; i++)
	{
		if(sai_IFFilterId[i])
		{
			((CButton *) GetDlgItem(sai_IFFilterId[i]))->SetCheck(ul & 1);
		}

		ul >>= 1;
	}
	((CButton *) GetDlgItem(IDC_CHECKDM_DRAWIT))->SetCheck(mb_DrawMask_DrawIt ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECKDM_NOTDRAWIT))->SetCheck(mb_DrawMask_NotDrawIt ? 1 : 0);

	((CButton *) GetDlgItem(IDC_SELECTION_NOTINSECTO))->SetCheck(mb_NotInSecto? 1 : 0);

	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnDrawIt(void)
{
	mb_DrawMask_DrawIt = mb_DrawMask_DrawIt ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnNotDrawIt(void)
{
	mb_DrawMask_NotDrawIt = mb_DrawMask_NotDrawIt ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnNotInSecto(void)
{
	mb_NotInSecto = mb_NotInSecto ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::UpdateIFFilter(void)
{
	/*~~~~~~~*/
	ULONG	ul;
	int		i;
	/*~~~~~~~*/

	/* Special */
	if(((CButton *) GetDlgItem(IDC_CHECK_NOTHIDDEN))->GetCheck())
		mb_ShowNotHidden = TRUE;
	else
		mb_ShowNotHidden = FALSE;
	if(((CButton *) GetDlgItem(IDC_CHECK_HIDDEN))->GetCheck())
		mb_ShowHidden = TRUE;
	else
		mb_ShowHidden = FALSE;
	if(((CButton *) GetDlgItem(IDC_CHECK_BONES))->GetCheck())
		mb_ShowBones = TRUE;
	else
		mb_ShowBones = FALSE;
	if(((CButton *) GetDlgItem(IDC_CHECK_PREFAB))->GetCheck())
		mb_ShowPrefab = TRUE;
	else
		mb_ShowPrefab = FALSE;


	if(((CButton *) GetDlgItem(IDC_CHECK_SPG2GRIDGEN))->GetCheck())
		mb_ShowSpg2GridGen = TRUE;
	else
		mb_ShowSpg2GridGen = FALSE;
	
	if(((CButton *) GetDlgItem(IDC_CHECK_PREFABSON))->GetCheck())
		mb_ShowPrefabSon = TRUE;
	else
		mb_ShowPrefabSon = FALSE;
	if(((CButton *) GetDlgItem(IDC_CHECK_ACT))->GetCheck())
		mb_ShowAct = TRUE;
	else
		mb_ShowAct = FALSE;
	if(((CButton *) GetDlgItem(IDC_CHECK_NOTACT))->GetCheck())
		mb_ShowNotAct = TRUE;
	else
		mb_ShowNotAct = FALSE;
	if(((CButton *) GetDlgItem(IDC_CHECK_VIS))->GetCheck())
		mb_ShowVis = TRUE;
	else
		mb_ShowVis = FALSE;
	if(((CButton *) GetDlgItem(IDC_CHECK_NOTVIS))->GetCheck())
		mb_ShowNotVis = TRUE;
	else
		mb_ShowNotVis = FALSE;
	if(((CButton *) GetDlgItem(IDC_CHECK_MERGED))->GetCheck())
		mb_Merge = TRUE;
	else
		mb_Merge = FALSE;
	if(((CButton *) GetDlgItem(IDC_CHECK_NOTMERGED))->GetCheck())
		mb_NotMerge = TRUE;
	else
		mb_NotMerge = FALSE;

	ul = 0;
	for(i = 31; i >= 0; i--)
	{
		ul <<= 1;
		if(sai_IFFilterId[i])
		{
			if(((CButton *) GetDlgItem(sai_IFFilterId[i]))->GetCheck()) ul |= 1;
		}
	}

	mul_IFFilter = ul;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::UpdateGroCheck(void)
{
	/*~~~~~~~*/
	ULONG	ul;
	int		i;
	/*~~~~~~~*/

	ul = mul_GroFilter;
	for(i = 0; i < 7; i++)
	{
		if(sai_GroFilterId[i])
		{
			((CButton *) GetDlgItem(sai_GroFilterId[i]))->SetCheck(ul & 1);
		}

		ul >>= 1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::UpdateGroFilter(void)
{
	/*~~~~~~~*/
	ULONG	ul;
	int		i;
	/*~~~~~~~*/

	ul = 0;
	for(i = 6; i >= 0; i--)
	{
		ul <<= 1;
		if(sai_GroFilterId[i])
		{
			if(((CButton *) GetDlgItem(sai_GroFilterId[i]))->GetCheck()) ul |= 1;
		}
	}

	mul_GroFilter = ul;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::UpdateInvisibleCheck(void)
{
	/*~~~~~~~*/
	ULONG	ul;
	int		i;
	/*~~~~~~~*/

	ul = mul_InvisibleFilter;
	for(i = 0; i < 16; i++)
	{
		if(sai_InvisibleFilterId[i])
		{
			((CButton *) GetDlgItem(sai_InvisibleFilterId[i]))->SetCheck(ul & 1);
		}

		ul >>= 1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::UpdateInvisibleFilter(void)
{
	/*~~~~~~~*/
	ULONG	ul;
	int		i;
	/*~~~~~~~*/

	ul = 0;
	for(i = 15; i >= 0; i--)
	{
		ul <<= 1;
		if(sai_InvisibleFilterId[i])
		{
			if(((CButton *) GetDlgItem(sai_InvisibleFilterId[i]))->GetCheck()) ul |= 1;
		}
	}

	mul_InvisibleFilter = ul;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_SelectionDialog::AddGAO(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox		*LB;
	GRO_tdst_Struct *pst_Gro;
	int				index;
	CString			o_Str;
	char			*pz;
	char			az[512];
	char			az1[512];
	AI_tdst_Model	*pst_Model;
	ULONG			key;
	CString			str;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO) return 0;
	LB = &mo_ListBox;

	/* Special */
	if(!mb_ShowNotHidden && (!(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden))) return 0;
	if(!mb_ShowHidden && (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden)) return 0;
	if(!mb_ShowBones && (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)) return 0;
	if(!mb_ShowBones && (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)) return 0;
	if(!mb_ShowAct && (pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active)) return 0;
	if(!mb_ShowNotAct && (!(pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active))) return 0;
	if(!mb_ShowVis && (pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible)) return 0;
	if(!mb_ShowNotVis && (!(pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible))) return 0;
	if(!mb_ShowPrefab && pst_GO->ul_PrefabKey && pst_GO->ul_PrefabKey != BIG_C_InvalidKey) return 0;

	if(!mb_ShowSpg2GridGen)
	{
		if (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject)
		{
			if (pst_GO->pst_Extended->pst_Modifiers)
			{
				MDF_tdst_Modifier  *pst_Modifier;
				pst_Modifier = pst_GO->pst_Extended->pst_Modifiers;
				u32 Value;
				Value = 0;
				while (pst_Modifier)
				{
					if (pst_Modifier->i->ul_Type == MDF_C_Modifier_SpriteMapper2) 
					{
						if (!(pst_Modifier->ul_Flags & (MDF_C_Modifier_Inactive)))
						{
							SPG2_tdst_Modifier *p_SPG2;
							p_SPG2 = (SPG2_tdst_Modifier *)pst_Modifier->p_Data;
							if (p_SPG2->ulFlags & SPG2_GridGeneration)
							Value = 1;
						}
					}
					pst_Modifier = pst_Modifier->pst_Next;
				}
				if (!Value) return 0;
			} else return 0;
		} else return 0;
	}


	if(!mb_ShowPrefabSon && pst_GO->ul_PrefabKey && pst_GO->ul_PrefabKey != BIG_C_InvalidKey)
	{
		CString strPFB(pst_GO->sz_Name);
		index = strPFB.Find('@');
		if(index)
		{
			if(mo_LstPrefab_Str.Find(strPFB.Left(index)))
				return 0;
			mo_LstPrefab_Str.AddTail(strPFB.Left(index));
		}		
		else
		{
		sscanf(pst_GO->sz_Name, "[%x]", &key);
		if(key)
		{
				if(mo_LstPrefab_Key.Find(key)) return 0;
				mo_LstPrefab_Key.AddTail(key);
			}
		}
	}

	/* Merged */
	if((pst_GO->c_FixFlags & OBJ_C_HasBeenMerge) && !mb_Merge) return 0;
	if(!(pst_GO->c_FixFlags & OBJ_C_HasBeenMerge) && !mb_NotMerge) return 0;

	/* Filtre par nom */
	str = pst_GO->sz_Name;
	if(GaoNameIsSkipped(str)) return 0;
	/*if(!mo_GaoF.IsEmpty())
	{
		str = pst_GO->sz_Name;
		str.MakeLower();
		if(str.Find(mo_GaoF) == -1) return 0;
	}*/

	/* Filter with identity flag */
	if(pst_GO->ul_IdentityFlags && !(pst_GO->ul_IdentityFlags && (pst_GO->ul_IdentityFlags & mul_IFFilter))) return 0;

	/* Filter with graphic object type */
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
	{
		pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
		if(pst_Gro == NULL)
		{
			if(!(mul_GroFilter & 1)) return 0;
		}
		else
		{
			if(!(mul_GroFilter & sai_GroFilterMask[pst_Gro->i->ul_Type])) return 0;
		}
	}

	/* Filter with invisible object type */
	if(!(pst_GO->ul_IdentityFlags & OBJ_C_VisibleObjectFlags))
	{
		if(!(mul_InvisibleFilter & (1L << pst_GO->ul_InvisibleObjectIndex))) return 0;
	}

	/* Drawmask */
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
	{
		if(!mb_DrawMask_DrawIt && (pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Draw)) return 0;
		if(!mb_DrawMask_NotDrawIt && !(pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Draw)) return 0;
	}

	/* Model AI */
	if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) && pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Ai)
	{
		pst_Model = ((AI_tdst_Instance *) pst_GO->pst_Extended->pst_Ai)->pst_Model;
		if(pst_Model && !pst_Model->c_DisplayFilter) return 0;
	}

	L_strcpy(az, pst_GO->sz_Name);
	if(az[L_strlen(az) - 1] != ']') /* Name of a gizmo */
	{
		pz = L_strrchr(az, '.');
		if(pz) *pz = 0;
	}

	if(pst_GO->ul_PrefabKey && pst_GO->ul_PrefabKey != BIG_C_InvalidKey && !mb_ShowPrefabSon)
	{
		pz = L_strrchr(az, ']');
		if(pz)
		{
			L_strcpy(az1, "PFB_");
			L_strcat(az1, pz + 2);
			L_strcpy(az, az1);
		}
		else
		{
			pz = L_strrchr(az, '@');
			if(pz) *pz = 0;
			L_strcpy(az1, "PFB_");
			L_strcat(az1, az);
			L_strcpy(az, az1);
		}		
	}

	if ( mb_NotInSecto  && pst_GO && pst_GO->pst_Extended ) 
	{
		for ( int i = 0 ; i < 4 ; i ++  ) 
		{
			if ( pst_GO->pst_Extended->auc_Sectos[i] != 0 ) 
			{
				return 0;
			}
		}
	}

	if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden) L_strcat(az, " (Hidden)");
	index = LB->AddString(CString(az));
	if(index == LB_ERR) return 0;
	
	LB->SetItemData(index, (DWORD) pst_GO);
	return 1;
}


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::ClearList(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	CComboBox	*CB;
	/*~~~~~~~~~~~~*/

	CB = (CComboBox *) GetDlgItem(IDC_COMBO1);
	CB->ResetContent();
	CB = (CComboBox *) GetDlgItem(IDC_COMBO_GOS);
	CB->ResetContent();
	LB = &mo_ListBox;
	LB->ResetContent();
	LB = (CListBox *) GetDlgItem(IDC_LIST_GROUPS);
	LB->ResetContent();
	LB = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
	LB->ResetContent();
	LB = (CListBox *) GetDlgItem(IDC_LIST_POSTIT);
	LB->ResetContent();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::UpdateList(BOOL _b_All)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	int					index, count;
	int					i_ScrollH, i_ScrollV;
	int					i_TopIndex, i_First;
	static BOOL			b_Rec = FALSE;
	CRect				o_Rect;
	LONG				l_EditZone;
	COL_tdst_Cob		*pst_Cob;
	TAB_tdst_PFelem		*pst_CurrentElemW;
	TAB_tdst_PFelem		*pst_EndElemW;
	WOR_tdst_World		*pst_World;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mb_LockUpdate) return;
	if(!mpst_World || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_World) == BIG_C_InvalidKey))
	{
		pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
		if(!pst_CurrentElemW) return;
		pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
		for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
		{
			pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
			if(TAB_b_IsAHole(pst_World)) continue;
			mpst_World = pst_World;
			break;
		}
	}

	if(!mpst_World || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_World) == BIG_C_InvalidKey)) return;

	LB = &mo_ListBox;

	/* To avoid flicking */
	if(!_b_All)
	{
		if(SEL_l_CountItem(mpst_World->pst_Selection, SEL_C_SIF_Object) == LB->GetSelCount())
		{
			/*~~~~~~~~~~~*/
			int		*pisel;
			BOOL	ok;
			/*~~~~~~~~~~~*/

			pisel = (int *) malloc(LB->GetSelCount() * sizeof(int));
			LB->GetSelItems(LB->GetSelCount(), pisel);
			ok = TRUE;
			for(i = 0; i < LB->GetSelCount(); i++)
			{
				pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(pisel[i]);
				if(pst_GO && !SEL_RetrieveItem(mpst_World->pst_Selection, pst_GO))
				{
					ok = FALSE;
					break;
				}
			}

			free(pisel);
			if(ok) return;
		}
#ifdef JADEFUSION 
		else if(SEL_l_CountItem(mpst_World->pst_Selection, SEL_C_SIF_Object) > LB->GetSelCount())
        {
            BOOL ok = TRUE;

            SEL_tdst_SelectedItem * pSel = SEL_pst_GetFirst( mpst_World->pst_Selection, SEL_C_SIF_Object );
            for( ;pSel && ok; pSel = SEL_pst_GetNext( mpst_World->pst_Selection, pSel, SEL_C_SIF_Object ) )
            {
                for( i=0; i < LB->GetCount() && ok; i++)
                {
                    pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
                    if( (((OBJ_tdst_GameObject *)pSel->p_Content) == pst_GO) && !LB->GetSel(i))
                    {
                        ok  = FALSE;
                    }
                }
            }
            if(ok) return;
        }
#endif
	}

	/* Stop recur */
	if(b_Rec) return;
	b_Rec = TRUE;

	/* Other lists */
	UpdateGroup();
	UpdateGos();
	UpdateNetworks();
	FillPostIt();

	i_ScrollH = LB->GetScrollPos(SB_HORZ);
	i_ScrollV = LB->GetScrollPos(SB_VERT);
	i_TopIndex = LB->GetTopIndex();

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mpst_World->st_AllWorldObjects);
	if(!pst_PFElem)
	{
		LB->ResetContent();
		b_Rec = FALSE;
		return;
	}

	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mpst_World->st_AllWorldObjects);

	M_MF()->LockDisplay(LB);
	LB->ResetContent();

	mo_LstPrefab_Key.RemoveAll();
	mo_LstPrefab_Str.RemoveAll();

	/* Add all objects of world */
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		AddGAO(pst_GO);
		/*
		if ( !AddGAO(pst_GO) && pst_GO)
		{
			if ( pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected )
				mpo_View->Selection_Unselect( pst_GO, SEL_C_SIF_Object, FALSE );
		}
		*/
	}

	/* Grid ? */
	if(mpst_World->pst_Grid)
	{
		pst_GO = (OBJ_tdst_GameObject *) mpst_World->pst_Grid->p_GameObject;
		AddGAO( pst_GO );
		/*if ( pst_GO && !AddGAO( pst_GO ) )
		{
			if ( pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected )
				mpo_View->Selection_Unselect( pst_GO, SEL_C_SIF_Object, FALSE );
		}*/
	}

	if(mdc_Sel) L_free(mdc_Sel);

	/* Update sel array */
	count = LB->GetCount();
	mdc_Sel = (char *) malloc(count * sizeof(char));
	i_First = -1;
	for(index = 0; index < count; index++)
	{
		pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(index);
		if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
		{
			LB->SetSel(index);
			if(i_First == -1) i_First = index;
		}
	}

	pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(mpst_World->pst_Selection, SEL_C_SIF_Cob);

	LINK_gb_AllRefreshEnable = FALSE;
	if(_b_All)
	{
		l_EditZone = mpst_World->pst_Selection->l_Flag & SEL_C_SF_Editable;
		mpo_View->Selection_b_Treat(NULL, 0, 0, 0);
		mpst_World->pst_Selection->l_Flag |= l_EditZone;
		/*
		for(index = 0; index < count; index++)
		{
			mdc_Sel[index] = LB->GetSel(index) ? 1 : 0;
			pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(index);

			if(mdc_Sel[index])
			{
				if ( !pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected )
					mpo_View->Selection_Object(pst_GO, SEL_C_SIF_Object, FALSE);
			}
			else
			{
				if ( pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected )
					mpo_View->Selection_Unselect( pst_GO, SEL_C_SIF_Object, FALSE );
			}
		}
		*/
	}
	
	for(index = 0; index < count; index++)
	{
		mdc_Sel[index] = LB->GetSel(index) ? 1 : 0;
		if(mdc_Sel[index])
		{
			pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(index);
			mpo_View->Selection_Object(pst_GO, SEL_C_SIF_Object, FALSE);
		}
	}

	if(pst_Cob) mpo_View->Selection_Zone(pst_Cob, (ULONG) pst_Cob->pst_GO, SEL_C_SIF_Cob);

	LINK_gb_AllRefreshEnable = TRUE;
	M_MF()->UnlockDisplay(LB);

	/* Update scroll and top index */
	LB->EnableScrollBar(SB_HORZ);
	if(i_ScrollH != LB->GetScrollPos(SB_HORZ))
	{
		LB->SetScrollPos(SB_HORZ, i_ScrollH);
		LB->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (i_ScrollH << 16), 0);
	}

	if(i_ScrollV != LB->GetScrollPos(SB_VERT))
	{
		LB->SetScrollPos(SB_VERT, i_ScrollV);
		LB->SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (i_ScrollV << 16), 0);
	}

	LB->GetWindowRect(&o_Rect);
	if((i_First < i_TopIndex) || (i_First > i_TopIndex + (o_Rect.Height() / LB->GetItemHeight(0))))
	{
		LB->SetTopIndex(i_First - ((o_Rect.Height() / LB->GetItemHeight(0)) / 2));
	}
	else
	{
		LB->SetTopIndex(i_TopIndex);
	}

	/* LINK_RefreshMenu(); */
	b_Rec = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::AddNameToList(OBJ_tdst_GameObject *pst_GO)
{
	CListBox			*LB;
	CString				objName;

	LB = &mo_ListBox;

	CString gaoName(pst_GO->sz_Name);
	for (int i=0 ; i<gaoName.ReverseFind('.') ; i++)
		objName.AppendChar(gaoName[i]);


	int index = LB->AddString(objName);
	if(index == LB_ERR) return;
	
	LB->SetItemData(index, (DWORD) pst_GO);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::RemoveNameFromList(OBJ_tdst_GameObject *pst_GO)
{
	CListBox			*LB;
	CString				objName;

	LB = &mo_ListBox;

	CString gaoName(pst_GO->sz_Name);
	for (int i=0 ; i<gaoName.ReverseFind('.') ; i++)
		objName.AppendChar(gaoName[i]);

	int index = LB->FindString(0, objName);
	if (index == LB_ERR)
		return;

	LB->DeleteString(index);
}
/*$4
 ***********************************************************************************************************************
    Message handler functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnIFCheckClick(UINT nId)
{
	UpdateIFFilter();
	UpdateList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnGroCheckClick(UINT nId)
{
	UpdateGroFilter();
	UpdateList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnInvisibleFCheckClick(UINT nId)
{
	UpdateInvisibleFilter();
	UpdateList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnSelChange(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i_Sel, i, j;
	int					*pi_Sel;
	char				refresh;
	OBJ_tdst_GameObject *pst_GO;
	int					i_Top;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	refresh = 0;

	if(!mdc_Sel) return;

	LB = &mo_ListBox;
	M_MF()->LockDisplay(LB);
	i_Top = LB->GetTopIndex();

	mb_LockUpdate = TRUE;

	/* Unselect first */
	i_Sel = LB->GetSelCount();
	if(i_Sel)
	{
		pi_Sel = (int *) L_malloc(sizeof(int) * i_Sel);
		LB->GetSelItems(i_Sel, pi_Sel);
		for(i = 0; i < i_Sel; i++) mdc_Sel[pi_Sel[i]] |= 2;
		j = pi_Sel[0];
		L_free(pi_Sel);
	}
	else
		j = 0;

	for(i = 0; i < LB->GetCount(); i++)
	{
		if(mdc_Sel[i] == 1)
		{
			pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
			mpo_View->Selection_Unselect(pst_GO, SEL_C_SIF_Object);
			refresh = 1;
			mdc_Sel[i] >>= 1;
		}
	}

	/* Then select */
	for(i = 0; i < LB->GetCount(); i++)
	{
		if(mdc_Sel[i] == 2)
		{
			pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
			mpo_View->Selection_Object(pst_GO, SEL_C_SIF_Object);
			refresh = 1;
		}

		mdc_Sel[i] >>= 1;
	}

	if(LB->GetSelCount() == 1)
	{
		pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(j);
		SEL_DelItem(mpst_World->pst_Selection, pst_GO);
		mpo_View->Selection_Object(pst_GO, SEL_C_SIF_Object);
		refresh = 1;
	}

	mb_LockUpdate = FALSE;
	if(refresh)
	{
		UpdateList();
		LINK_Refresh();

		mpo_View->mb_SelectionChange = TRUE;
		mpo_View->Selection_Change();
	}

	LB->SetTopIndex(i_Top);
	M_MF()->UnlockDisplay(LB);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnBtNoneClicked(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i;
	char				refresh;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	refresh = 0;

	if(((CTabCtrl *) GetDlgItem(IDC_TAB1))->GetCurSel() == 1)
	{
		LB = (CListBox *) GetDlgItem(IDC_LIST_GROUPS);
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(LB->GetSel(i))
			{
				LB->SetSel(i, FALSE);
				refresh = 1;
			}
		}

		if(refresh) OnSelChangeGroups();
	}
	else if(((CTabCtrl *) GetDlgItem(IDC_TAB1))->GetCurSel() == 2)
	{
		LB = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(LB->GetSel(i))
			{
				LB->SetSel(i, FALSE);
				refresh = 1;
			}
		}
	}
	else
	{
		LB = &mo_ListBox;
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(mdc_Sel[i] == 1)
			{
				pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
				mpo_View->Selection_Unselect(pst_GO, SEL_C_SIF_Object);
				refresh = 1;
			}
		}
	}

	if(refresh)
	{
		UpdateList();
		LINK_Refresh();
	}

	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnBtAllClicked(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i;
	char				refresh;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	refresh = 0;

	if(((CTabCtrl *) GetDlgItem(IDC_TAB1))->GetCurSel() == 1)
	{
		LB = (CListBox *) GetDlgItem(IDC_LIST_GROUPS);
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(!LB->GetSel(i))
			{
				LB->SetSel(i);
				refresh = 1;
			}
		}

		if(refresh) OnSelChangeGroups();
	}
	else if(((CTabCtrl *) GetDlgItem(IDC_TAB1))->GetCurSel() == 2)
	{
		LB = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(!LB->GetSel(i))
			{
				LB->SetSel(i);
				refresh = 1;
			}
		}
	}
	else
	{
		LB = &mo_ListBox;
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(mdc_Sel[i] == 0)
			{
				pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
				mpo_View->Selection_Object(pst_GO, SEL_C_SIF_Object);
				refresh = 1;
			}
		}
	}

	if(refresh)
	{
		UpdateList();
		LINK_Refresh();
	}

	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnBtInvertClicked(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i;
	char				refresh;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	refresh = 0;

	if(((CTabCtrl *) GetDlgItem(IDC_TAB1))->GetCurSel() == 1)
	{
		LB = (CListBox *) GetDlgItem(IDC_LIST_GROUPS);
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(LB->GetSel(i))
				LB->SetSel(i, FALSE);
			else
				LB->SetSel(i);
			refresh = 1;
		}

		if(refresh) OnSelChangeGroups();
	}
	else if(((CTabCtrl *) GetDlgItem(IDC_TAB1))->GetCurSel() == 2)
	{
		LB = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(LB->GetSel(i))
				LB->SetSel(i, FALSE);
			else
				LB->SetSel(i);
			refresh = 1;
		}
	}
	else
	{
		LB = &mo_ListBox;
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(mdc_Sel[i] == 0)
			{
				pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
				mpo_View->Selection_Object(pst_GO, SEL_C_SIF_Object);
				refresh = 1;
			}

			if(mdc_Sel[i] == 1)
			{
				pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
				mpo_View->Selection_Unselect(pst_GO, SEL_C_SIF_Object);
				refresh = 1;
			}
		}
	}

	if(refresh)
	{
		UpdateList();
		LINK_Refresh();
	}

	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnListDblClk(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i_Sel;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	LB = &mo_ListBox;
	i_Sel = LB->GetCurSel();
	if(i_Sel != -1)
	{
		pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i_Sel);
		mpo_View->ForceSelectObject(pst_GO);
		mpo_View->ZoomExtendSelected
			(
				&mpo_View->mst_WinHandles.pst_DisplayData->st_Camera,
				mpo_View->mst_WinHandles.pst_World
			);
		LINK_Refresh();
	}

	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnBtInvers1(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mul_IFFilter = ~mul_IFFilter;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnBtInvers2(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mul_GroFilter = ~mul_GroFilter;
	UpdateGroCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnBtInvers3(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mul_InvisibleFilter = ~mul_InvisibleFilter;
	UpdateInvisibleCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnBtClear1(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mul_IFFilter = 0;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnBtClear2(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mul_GroFilter = 0;
	UpdateGroCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnBtClear3(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mul_InvisibleFilter = 0;
	UpdateInvisibleCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnHide(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i;
	OBJ_tdst_GameObject *pst_GO;
	WAY_tdst_Network	*pst_Net;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Networks */
	if(((CTabCtrl *) GetDlgItem(IDC_TAB1))->GetCurSel() == 2)
	{
		LB = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(LB->GetSel(i))
			{
				pst_Net = (WAY_tdst_Network *) LB->GetItemData(i);
				pst_Net->ul_Flags &= ~WAY_C_DisplayNet;
			}
		}
	}
	else
	{
		LB = &mo_ListBox;
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(mdc_Sel[i] == 1)
			{
				pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
				pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_Hidden;
			}
		}
	}

	UpdateList();
	LINK_Refresh();
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnShow(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i;
	OBJ_tdst_GameObject *pst_GO;
	WAY_tdst_Network	*pst_Net;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Networks */
	if(((CTabCtrl *) GetDlgItem(IDC_TAB1))->GetCurSel() == 2)
	{
		LB = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(LB->GetSel(i))
			{
				pst_Net = (WAY_tdst_Network *) LB->GetItemData(i);
				pst_Net->ul_Flags |= WAY_C_DisplayNet;
			}
		}
	}
	else
	{
		LB = &mo_ListBox;
		for(i = 0; i < LB->GetCount(); i++)
		{
			if(mdc_Sel[i] == 1)
			{
				pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
				if (!(pst_GO->ul_AdditionalFlags & OBJ_C_EdAddFlags_HiddenByWowFilter))
					pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Hidden;
			}
		}
	}

	UpdateList();
	LINK_Refresh();
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnWired(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	LB = &mo_ListBox;
	for(i = 0; i < LB->GetCount(); i++)
	{
		if(mdc_Sel[i] == 1)
		{
			pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
			if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
				pst_GO->pst_Base->pst_Visu->ul_DrawMask &= ~GDI_Cul_DM_NotWired;
		}
	}

	/* UpdateList(); */
	LINK_Refresh();
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnNoWired(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	LB = &mo_ListBox;
	for(i = 0; i < LB->GetCount(); i++)
	{
		if(mdc_Sel[i] == 1)
		{
			pst_GO = (OBJ_tdst_GameObject *) LB->GetItemData(i);
			if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
				pst_GO->pst_Base->pst_Visu->ul_DrawMask |= GDI_Cul_DM_NotWired;
		}
	}

	/* UpdateList(); */
	LINK_Refresh();
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnHidden(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_ShowHidden = mb_ShowHidden ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnNotHidden(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_ShowNotHidden = mb_ShowNotHidden ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnBones(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_ShowBones = mb_ShowBones ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnPrefab(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_ShowPrefab = mb_ShowPrefab ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnSpg2Grid(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_ShowSpg2GridGen = mb_ShowSpg2GridGen ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnPrefabSon(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_ShowPrefabSon = mb_ShowPrefabSon ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnShowAct(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_ShowAct = mb_ShowAct ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnShowNotAct(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_ShowNotAct = mb_ShowNotAct ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnShowVis(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_ShowVis = mb_ShowVis ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnShowNotVis(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_ShowNotVis = mb_ShowNotVis ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnMerged(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_Merge = mb_Merge ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnNotMerged(void)
{
	/*~~~~~~~~~~~~*/
	CListBox	*LB;
	/*~~~~~~~~~~~~*/

	mb_NotMerge = mb_NotMerge ? FALSE : TRUE;
	UpdateIFCheck();
	UpdateList();
	LINK_Refresh();
	LB = &mo_ListBox;
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnCloseSel(void)
{
	mpo_View->Selection_WithDialog();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnMinimize(void)
{
	mpo_View->mpo_Frame->MinMaxSel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnToolBox(void)
{
	mpo_View->Selection_ToolBox();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnSelectionType(void)
{
	/*~~~~~~~~~~~~~~~*/
	CButton *po_Button;
	/*~~~~~~~~~~~~~~~*/

	po_Button = (CButton *) GetDlgItem(IDC_CHECK_SELECTIONTYPE);
	if(po_Button->GetCheck())
	{
		mpo_View->mst_WinHandles.pst_DisplayData->pst_PickingBuffer->ul_UserFlags |= SOFT_Cul_PBF_WindowSelection;
	}
	else
	{
		mpo_View->mst_WinHandles.pst_DisplayData->pst_PickingBuffer->ul_UserFlags &= ~SOFT_Cul_PBF_WindowSelection;
	}

	UpdateSelectionButton();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnChangePane(NMHDR *pNotifyStruct, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTabCtrl	*ptab;
	int			i_Pane;
	CListBox	*LB, *LB1, *LB2, *LB3;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LB = &mo_ListBox;
	LB1 = (CListBox *) GetDlgItem(IDC_LIST_GROUPS);
	LB2 = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
	LB3 = (CListBox *) GetDlgItem(IDC_LIST_POSTIT);

	ptab = (CTabCtrl *) GetDlgItem(IDC_TAB1);
	i_Pane = ptab->GetCurFocus();

	switch(i_Pane)
	{
	case 0:
		LB->BringWindowToTop();
		LB->ShowWindow(SW_SHOW);
		LB1->ShowWindow(SW_HIDE);
		LB2->ShowWindow(SW_HIDE);
		LB3->ShowWindow(SW_HIDE);
		break;

	case 1:
		LB->ShowWindow(SW_HIDE);
		LB1->BringWindowToTop();
		LB1->ShowWindow(SW_SHOW);
		LB2->ShowWindow(SW_HIDE);
		LB3->ShowWindow(SW_HIDE);
		break;

	case 2:
		LB->ShowWindow(SW_HIDE);
		LB1->ShowWindow(SW_HIDE);
		LB2->BringWindowToTop();
		LB2->ShowWindow(SW_SHOW);
		LB3->ShowWindow(SW_HIDE);
		break;

	case 3:
		LB->ShowWindow(SW_HIDE);
		LB1->ShowWindow(SW_HIDE);
		LB2->ShowWindow(SW_HIDE);
		LB3->BringWindowToTop();
		LB3->ShowWindow(SW_SHOW);
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
void EDIA_cl_SelectionDialog::UpdateGroup(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox	*PC;
	CListBox	*LB;
	int			i_Sel, i_Res, i_Top;
	BIG_INDEX	ul_Index, ul_Dir;
	char		asz_Path[BIG_C_MaxLenPath];
	int			*pi_Sel;
	ULONG		i;
	int			ii;
	CString		o_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_World || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_World) == BIG_C_InvalidKey)) return;
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpst_World);
	if(ul_Index == BIG_C_InvalidIndex) return;
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, EDI_Csz_Path_Groups);
	ul_Dir = BIG_ul_SearchDir(asz_Path);
	if(ul_Dir == BIG_C_InvalidIndex) return;

	PC = (CComboBox *) GetDlgItem(IDC_COMBO1);
	ii = PC->GetCurSel();
	PC->ResetContent();

	LB = (CListBox *) GetDlgItem(IDC_LIST_GROUPS);
	i_Sel = LB->GetSelCount();
	pi_Sel = (int *) L_malloc(i_Sel * sizeof(int));
	LB->GetSelItems(i_Sel, pi_Sel);
	i_Top = LB->GetTopIndex();
	M_MF()->LockDisplay(LB);
	LB->ResetContent();

	/* Scan dir */
	ul_Index = BIG_FirstFile(ul_Dir);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtObjGolGroups))
		{
			o_Name = BIG_NameFile(ul_Index);
			if(LOA_ul_SearchAddress(BIG_PosFile(ul_Index)) != -1) o_Name += " (Engine)";
			i_Res = PC->AddString(o_Name);
			PC->SetItemData(i_Res, ul_Index);
			i_Res = LB->AddString(o_Name);
			LB->SetItemData(i_Res, ul_Index);
		}

		ul_Index = BIG_NextFile(ul_Index);
	}

	for(i = 0; i < (ULONG) i_Sel; i++)
	{
		LB->SetSel(pi_Sel[i]);
	}

	L_free(pi_Sel);

	LB->SetTopIndex(i_Top);
	PC->SetCurSel(ii);
	M_MF()->UnlockDisplay(LB);
}


void EDIA_cl_SelectionDialog::OnClearGos(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_World || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_World) == BIG_C_InvalidKey)) return;
	mpst_World->pListOfGroups = NULL;
	mpst_World->ulNumberOfGroups = 0;
	UpdateGos();
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::UpdateGos(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox	*PC;
	ULONG		i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_World || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_World) == BIG_C_InvalidKey)) return;


	PC = (CComboBox *) GetDlgItem(IDC_COMBO_GOS);
	PC->ResetContent();

	for (i = 0 ; i < mpst_World->ulNumberOfGroups ; i ++)
	{
		PC->SetItemData(PC->AddString((char *)mpst_World->pListOfGroups[i].Name), i);
	}

	PC->SetCurSel(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnUpdateGroup(void)
{
	OnNewGroup(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnAdd2Group(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox	*PC;
	BIG_INDEX	ul_Index;
	int			i_Res, i_Res1;
	int			*pi_Sel;
	CListBox	*LB;
	ULONG		i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* Remember sel */
	LB = &mo_ListBox;

	i_Res = LB->GetSelCount();
	if(!i_Res) return;

	pi_Sel = (int *) L_malloc(i_Res * sizeof(int));
	LB->GetSelItems(i_Res, pi_Sel);

	PC = (CComboBox *) GetDlgItem(IDC_COMBO1);
	i_Res = PC->GetCurSel();
	if(i_Res == -1) return;

	/* Select group to have all objects of group */
	ul_Index = PC->GetItemData(i_Res);
	LINK_gb_RefreshEnable = FALSE;
	M_MF()->LockDisplay(LB);
	mpo_View->SelectGroup(ul_Index);
	UpdateList(TRUE);

	/* Add selected objects to current ones */
	i_Res1 = LB->GetSelCount();
	if(i_Res1)
	{
		if(i_Res)
			pi_Sel = (int *) L_realloc(pi_Sel, (i_Res1 + i_Res) * sizeof(int));
		else
			pi_Sel = (int *) L_malloc(i_Res1 * sizeof(int));
		LB->GetSelItems(i_Res1, pi_Sel + i_Res);
	}

	if(i_Res + i_Res1)
	{
		for(i = 0; i < (ULONG) i_Res + i_Res1; i++) LB->SetSel(pi_Sel[i]);
	}

	OnSelChange();
	if(i_Res) OnNewGroup(FALSE);
	for(i = i_Res; i < (ULONG) i_Res + i_Res1; i++) LB->SetSel(pi_Sel[i], FALSE);

	if(i_Res + i_Res1)
	{
		L_free(pi_Sel);
	}

	OnSelChange();
	M_MF()->UnlockDisplay(LB);
	LINK_gb_RefreshEnable = TRUE;
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnGroupSel(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	CComboBox	*PC;
	BIG_INDEX	ul_Index;
	int			i_Res;
	/*~~~~~~~~~~~~~~~~~*/

	PC = (CComboBox *) GetDlgItem(IDC_COMBO1);
	i_Res = PC->GetCurSel();
	if(i_Res == -1) return;
	ul_Index = PC->GetItemData(i_Res);
	LINK_gb_RefreshEnable = FALSE;
	mpo_View->SelectGroup(ul_Index);
	LINK_gb_RefreshEnable = TRUE;
	LINK_Refresh();
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
extern ULONG WORGos_IsInGroup(WOR_SelectionGroup *p_Group, ULONG ul_Key);
#else
extern "C" ULONG WORGos_IsInGroup(WOR_SelectionGroup *p_Group, ULONG ul_Key);
#endif
void EDIA_cl_SelectionDialog::OnGosSel(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	CComboBox	*PC;
	int			i_Res;
	/*~~~~~~~~~~~~~~~~~*/

	PC = (CComboBox *) GetDlgItem(IDC_COMBO_GOS);
	i_Res = PC->GetCurSel();
	if(!mpst_World || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_World) == BIG_C_InvalidKey)) return;
	if(i_Res == -1) return;
	{
		TAB_tdst_PFelem     *pst_Elem, *pst_LastElem;
		OBJ_tdst_GameObject *pst_GO;
		WOR_SelectionGroup *p_Group;
		ULONG ulNumberOfObject,AddMode;
		ULONG ulNumber;

		ulNumber = PC->GetItemData(i_Res);


		AddMode = 0;
		if(ControlKeyIsPressed) 
			AddMode = OBJ_C_EditFlags_Selected;

		if (mpst_World->ulNumberOfGroups > ulNumber) 
		{
			p_Group = &mpst_World->pListOfGroups[ulNumber];
			for(ulNumberOfObject = 0 , pst_Elem = mpst_World->st_AllWorldObjects.p_Table , pst_LastElem = mpst_World->st_AllWorldObjects.p_NextElem ; pst_Elem < pst_LastElem;pst_Elem++)
			{
				ULONG ul_Key;
				if(TAB_b_IsAHole(pst_Elem->p_Pointer)) continue;
				pst_GO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
				if (pst_GO->ul_EditorFlags & AddMode)
				{
					mpo_View->Selection_Object(pst_GO, SEL_C_SIF_Object, FALSE);
				} else
				{
					mpo_View->Selection_Unselect(pst_GO, SEL_C_SIF_Object, FALSE);
					ul_Key = LOA_ul_SearchKeyWithAddress((ULONG)pst_GO);
					if (ul_Key != BIG_C_InvalidIndex)
					{
						if (WORGos_IsInGroup(p_Group, ul_Key))
							mpo_View->Selection_Object(pst_GO, SEL_C_SIF_Object, FALSE);
					}
				}
			}
		}
	}
//	mpo_View->Selection_Object(pst_GO, SEL_C_SIF_Object);
	UpdateList();
	LINK_Refresh();

	
	PC->SetCurSel(i_Res);
	GetParent()->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnNewGroup(BOOL _b_ASK)
{
	/*~~~~~~~~~~~~~~~*/
	CComboBox	*PC;
	CString		o_Text;
	/*~~~~~~~~~~~~~~~*/

	PC = (CComboBox *) GetDlgItem(IDC_COMBO1);
	PC->GetWindowText(o_Text);
	if(!o_Text.IsEmpty())
	{
		mpo_View->Selection_GroupByName((char *) (LPCSTR) o_Text, _b_ASK);
		UpdateGroup();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnNewGos(BOOL _b_ASK)
{
	/*~~~~~~~~~~~~~~~*/
	CComboBox	*PC;
	CString		o_Text;
	/*~~~~~~~~~~~~~~~*/
/* Create a nex one*/
	if(!mpst_World || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_World) == BIG_C_InvalidKey)) return;

	PC = (CComboBox *) GetDlgItem(IDC_COMBO_GOS);
	PC->GetWindowText(o_Text);
	if(!o_Text.IsEmpty())
	{
		WORGos_DefineGroup(mpst_World, (char *) (LPCSTR) o_Text);
		UpdateGos();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnSelChangeGroups(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CListBox	*LB;
	int			i_Sel, ii;
	BIG_INDEX	ul_Index;
	BOOL		b_First;
	CComboBox	*po_Combo;
	CString		o_Str;
	/*~~~~~~~~~~~~~~~~~~*/

	LB = (CListBox *) GetDlgItem(IDC_LIST_GROUPS);

	b_First = TRUE;
	mb_LockUpdate = TRUE;
	LINK_gb_RefreshEnable = FALSE;

	mpo_View->Selection_b_Treat(NULL, 0, 0, 0);
	for(i_Sel = 0; i_Sel < LB->GetCount(); i_Sel++)
	{
		if(LB->GetSel(i_Sel))
		{
			ul_Index = LB->GetItemData(i_Sel);
			mpo_View->SelectGroup(ul_Index, b_First);
			b_First = FALSE;

			po_Combo = (CComboBox *) GetDlgItem(IDC_COMBO1);
			LB->GetText(i_Sel, o_Str);
			ii = po_Combo->FindString(-1, o_Str);
			if(ii != -1) po_Combo->SetCurSel(ii);
		}
	}

	LINK_gb_RefreshEnable = TRUE;
	mb_LockUpdate = FALSE;
	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::UpdateNetworks(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*LB;
	int			i_Sel, i_Top, i_Res;
	ULONG		i;
	BIG_INDEX	ul_Index;
	int			*pi_Sel;
	char		az[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LB = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
	if(!LB) return;

	M_MF()->LockDisplay(LB);
	i_Sel = LB->GetSelCount();
	if(i_Sel)
	{
		pi_Sel = (int *) L_malloc(i_Sel * sizeof(int));
		LB->GetSelItems(i_Sel, pi_Sel);
	}

	i_Top = LB->GetTopIndex();
	LB->ResetContent();

	if(!mpst_World->pst_AllNetworks)
	{
		if(i_Sel) L_free(pi_Sel);
		return;
	}

	for(i = 0; i < mpst_World->pst_AllNetworks->ul_Num; i++)
	{
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpst_World->pst_AllNetworks->ppst_AllNetworks[i]);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			if(mpst_World->pst_AllNetworks->ppst_AllNetworks[i]->ul_Flags & WAY_C_HasBeenMerge)
				sprintf(az, "%s (MERGE)", BIG_NameFile(ul_Index));
			else
				sprintf(az, "%s", BIG_NameFile(ul_Index));
			i_Res = LB->AddString(az);
			LB->SetItemData(i_Res, (ULONG) mpst_World->pst_AllNetworks->ppst_AllNetworks[i]);
		}
	}

	if(i_Sel)
	{
		for(i = 0; i < (ULONG) i_Sel; i++) LB->SetSel(pi_Sel[i]);
		L_free(pi_Sel);
	}

	LB->SetTopIndex(i_Top);
	M_MF()->UnlockDisplay(LB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnNetSel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i_Sel;
	OBJ_tdst_GameObject *pst_GO;
	BOOL				b_First;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	LB = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
	if(!LB) return;

	mb_LockUpdate = TRUE;
	mpo_View->Selection_b_Treat(NULL, 0, 0, 0);
	b_First = TRUE;
	for(i_Sel = 0; i_Sel < LB->GetCount(); i_Sel++)
	{
		if(LB->GetSel(i_Sel))
		{
			pst_GO = ((WAY_tdst_Network *) LB->GetItemData(i_Sel))->pst_Root;
			if(pst_GO)
			{
				mpo_View->ForceSelectObject(pst_GO, b_First);
				mpo_View->SelectNetGAO(pst_GO);
				b_First = FALSE;
			}
		}
	}

	LINK_Refresh();
	mb_LockUpdate = FALSE;
	UpdateList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnNetDblClk(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	int					i_Sel;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	LB = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
	if(!LB) return;

	i_Sel = LB->GetCurSel();
	pst_GO = ((WAY_tdst_Network *) LB->GetItemData(i_Sel))->pst_Root;
	if(!pst_GO) return;
	mpo_View->ForceSelectObject(pst_GO);
	mpo_View->ZoomExtendSelected
		(
			&mpo_View->mst_WinHandles.pst_DisplayData->st_Camera,
			mpo_View->mst_WinHandles.pst_World
		);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EDIA_cl_SelectionDialog::ul_GAOFromPt(POINT pt)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	BOOL	b;
	UINT	ui_Val;
	/*~~~~~~~~~~~*/

	GetCursorPos(&pt);
	GetDlgItem(IDC_LIST_OBJ)->GetWindowRect(&o_Rect);
	if(!o_Rect.PtInRect(pt)) return 0;
	GetDlgItem(IDC_LIST_OBJ)->ScreenToClient(&pt);
	ui_Val = mo_ListBox.ItemFromPoint(pt, b);
	if(ui_Val == -1) return 0;
	if(b) return 0;
	return mo_ListBox.GetItemData(ui_Val);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnAIModels(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	CCheckList			o_Check;
	BIG_INDEX			ul_Index;
	BOOL				*pb;
	CMapPtrToPtr		o_Ptr;
	POSITION			pos;
	CRect				o_Rect;
	int					i;
	char				az[128];
	AI_tdst_Model		*pt_Model;
	CStringArray		o_Lst;
	CString				o_Word;
	char				*pz;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Count */
	if(!mpst_World || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_World) == BIG_C_InvalidKey)) return;
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mpst_World->st_AllWorldObjects);
	if(!pst_PFElem) return;
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mpst_World->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AI)) continue;
		if(!pst_GO->pst_Extended) continue;
		if(!pst_GO->pst_Extended->pst_Ai) continue;
		pt_Model = ((AI_tdst_Instance *) pst_GO->pst_Extended->pst_Ai)->pst_Model;
		if(!pt_Model) continue;
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pt_Model);
		o_Ptr.SetAt((void *) ul_Index, (void *) ul_Index);
	}

	pb = (BOOL *) malloc(o_Ptr.GetCount() * sizeof(BOOL));

	/* Tri */
	pos = o_Ptr.GetStartPosition();
	while(pos)
	{
		o_Ptr.GetNextAssoc(pos, (void * &) ul_Index, (void * &) ul_Index);
		L_strcpy(az, BIG_NameFile(ul_Index));
		*L_strrchr(az, '.') = 0;
		o_Word.Format("%s@%d", az, ul_Index);
		o_Lst.Add(o_Word);
	}

	SortStringArray(o_Lst, FALSE);

	for(i = 0; i < o_Lst.GetCount(); i++)
	{
		o_Word = o_Lst.GetAt(i);
		L_strcpy(az, (char *) (LPCSTR) o_Word);
		pz = L_strrchr(az, '@');
		*pz = 0;
		o_Check.AddString(az);
		pz++;
		ul_Index = L_atoi(pz);
		pt_Model = (AI_tdst_Model *) LOA_ul_SearchAddress(BIG_PosFile(ul_Index));
		pb[i] = pt_Model->c_DisplayFilter;
	}

	mpo_View->GetWindowRect(&o_Rect);
	o_Rect.left += (o_Rect.Width() / 2) - 100;
	o_Rect.top += (o_Rect.Height() / 2) - 100;
	o_Check.SetArrayBool(pb);
	o_Check.Do(&o_Rect, 200, this, 20, "List Of Models");

	for(i = 0; i < o_Lst.GetCount(); i++)
	{
		o_Word = o_Lst.GetAt(i);
		pz = L_strrchr((char *) (LPCSTR) o_Word, '@');
		pz++;
		ul_Index = L_atoi(pz);
		pt_Model = (AI_tdst_Model *) LOA_ul_SearchAddress(BIG_PosFile(ul_Index));
		pt_Model->c_DisplayFilter = pb[i];
	}

	UpdateList();
	free(pb);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::FillPostIt(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox		*po_List;
	CString			o_Name;
	POSITION		ppos, pos;
	F3D_tdst_PostIt pit;
	int				index;
	int				sel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST_POSTIT);
	po_List->SetRedraw(FALSE);
	sel = po_List->GetCurSel();
	po_List->ResetContent();

	if(mpst_World)
	{
		pos = mpo_View->mo_ListPostIt.GetHeadPosition();
		while(pos)
		{
			ppos = pos;
			pit = mpo_View->mo_ListPostIt.GetNext(pos);
			pit.ul_Flags &= ~PIT_C_Draw;
			mpo_View->mo_ListPostIt.SetAt(ppos, pit);
			if(!(mul_FilterPostIt & PIT_ME) && (pit.ul_Flags & PIT_C_ForMe)) continue;
			if(!(mul_FilterPostIt & PIT_NME) && !(pit.ul_Flags & PIT_C_ForMe)) continue;
			if(!(mul_FilterPostIt & PIT_READ) && !(pit.ul_Flags & PIT_C_NoRead)) continue;
			if(!(mul_FilterPostIt & PIT_NREAD) && (pit.ul_Flags & PIT_C_NoRead)) continue;

			pit.ul_Flags |= PIT_C_Draw;
			mpo_View->mo_ListPostIt.SetAt(ppos, pit);

			o_Name = pit.az_Name;
			*L_strrchr((char *) (LPCSTR) o_Name, '.') = 0;
			if(pit.ul_Flags & PIT_C_NoRead) o_Name = "* " + o_Name;
			index = po_List->AddString(o_Name);
			po_List->SetItemData(index, (int) ppos);
		}
	}

	po_List->SetRedraw(TRUE);
	if(sel >= po_List->GetCount()) sel = po_List->GetCount() - 1;
	if(sel != -1) po_List->SetCurSel(sel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::MenuNetwork(void)
{
    enum EChoice {
        EChoiceNone = 0,
        EChoiceDisplay = 1,
        EChoiceDisplaySolo = 2,
        EChoiceHide = 10,
        EChoiceSelectWaypoints = 20,
        EChoiceRename = 30,
    };

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu		o_SubMenu(FALSE);
	CListBox			*po_List;
	CPoint				o_Point;
	int					i_Res;
	int					i_Sel;
	WAY_tdst_Network	*pst_Net;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_World || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_World) == BIG_C_InvalidKey)) return;
	po_List = (CListBox *) GetDlgItem(IDC_LIST_NETWORKS);
	GetCursorPos(&o_Point);
	M_MF()->InitPopupMenuAction(NULL, &o_SubMenu);
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, EChoiceDisplay, TRUE, "Display");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, EChoiceDisplaySolo, TRUE, "Display Solo");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, EChoiceNone, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, EChoiceHide, TRUE, "Hide");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, EChoiceNone, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, EChoiceSelectWaypoints, TRUE, "Select Waypoints");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, EChoiceRename, TRUE, "Rename");
	i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Point, &o_SubMenu);

	if(i_Res == EChoiceSelectWaypoints)
	{
		OnNetSel();
	}
    else if (i_Res == EChoiceRename)
    {
        int iSelected = -1;
		for (i_Sel = 0; i_Sel < po_List->GetCount(); i_Sel++)
        {
			if(po_List->GetSel(i_Sel))
            {
                // Can rename only one selection
                if (iSelected>=0)
                    return;
                else
                    iSelected =  i_Sel;
            }
        }

        // No selection -> no rename
        if (iSelected == -1)
            return;

        CString sCurrentName;
        po_List->GetText(iSelected,sCurrentName);
        sCurrentName = sCurrentName.Left(sCurrentName.GetLength()-4);

    	EDIA_cl_NameDialog	o_Dialog("Enter new name");
        o_Dialog.mo_Name = sCurrentName;
    	if(o_Dialog.DoModal() == IDOK)
	    {
    		o_Dialog.mo_Name += ".way";

    		/* Check validity of name */
	    	if(BIG_b_CheckName((char *) (LPCSTR) o_Dialog.mo_Name) == FALSE) 
                return;
            
            pst_Net = (WAY_tdst_Network *) po_List->GetItemData(iSelected);
            ULONG ulFat = LOA_ul_SearchIndexWithAddress((ULONG)pst_Net);
            if (ulFat == BIG_C_InvalidIndex)
                return;

            // Delete old string and create one with new name.
            po_List->DeleteString(iSelected);


            iSelected = po_List->AddString((LPCSTR) (o_Dialog.mo_Name));
            po_List->SetItemData(iSelected,(DWORD_PTR)pst_Net);

            // Change file name 
            char asz_Path[BIG_C_MaxLenPath];
            BIG_ComputeFullName(BIG_ParentFile(ulFat), asz_Path);
	    	BIG_RenFile((char *) (LPCSTR) o_Dialog.mo_Name, asz_Path, BIG_NameFile(ulFat));
        }
    }
	else if(i_Res == EChoiceDisplaySolo)
	{
		for(i_Sel = 0; i_Sel < po_List->GetCount(); i_Sel++)
		{
			pst_Net = (WAY_tdst_Network *) po_List->GetItemData(i_Sel);
			if(po_List->GetSel(i_Sel))
				pst_Net->ul_Flags |= WAY_C_DisplayNet;
			else
				pst_Net->ul_Flags &= ~WAY_C_DisplayNet;
		}
	}
	else
	{
		for(i_Sel = 0; i_Sel < po_List->GetCount(); i_Sel++)
		{
			if(po_List->GetSel(i_Sel))
			{
				pst_Net = (WAY_tdst_Network *) po_List->GetItemData(i_Sel);
				if(pst_Net)
				{
					if(i_Res == EChoiceDisplay)
						pst_Net->ul_Flags |= WAY_C_DisplayNet;
					else if(i_Res == EChoiceHide)
						pst_Net->ul_Flags &= ~WAY_C_DisplayNet;
				}
			}
		}
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::MenuPostIt(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu			o_SubMenu(FALSE);
	CPoint					o_Point;
	int						i_Res;
	EDIA_cl_PostItDialog	o_Dlg(mpst_World, mpo_View);
	CListBox				*po_List;
	char					az[1024];
	int						sel;
	POSITION				pos;
	F3D_tdst_PostIt			pit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_World || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_World) == BIG_C_InvalidKey)) return;
	po_List = (CListBox *) GetDlgItem(IDC_LIST_POSTIT);
	GetCursorPos(&o_Point);
	M_MF()->InitPopupMenuAction(NULL, &o_SubMenu);
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 1, TRUE, "New PostIt");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 2, TRUE, "Delete PostIt");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 5, TRUE, "Refresh");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction
		(
			NULL,
			&o_SubMenu,
			10,
			TRUE,
			"Display Read",
			DFCS_BUTTONCHECK | ((mul_FilterPostIt & PIT_READ) ? DFCS_CHECKED : 0)
		);
	M_MF()->AddPopupMenuAction
		(
			NULL,
			&o_SubMenu,
			11,
			TRUE,
			"Display Not Read",
			DFCS_BUTTONCHECK | ((mul_FilterPostIt & PIT_NREAD) ? DFCS_CHECKED : 0)
		);
	M_MF()->AddPopupMenuAction
		(
			NULL,
			&o_SubMenu,
			12,
			TRUE,
			"Display For Me",
			DFCS_BUTTONCHECK | ((mul_FilterPostIt & PIT_ME) ? DFCS_CHECKED : 0)
		);
	M_MF()->AddPopupMenuAction
		(
			NULL,
			&o_SubMenu,
			13,
			TRUE,
			"Display For Others",
			DFCS_BUTTONCHECK | ((mul_FilterPostIt & PIT_NME) ? DFCS_CHECKED : 0)
		);
	i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Point, &o_SubMenu);
	switch(i_Res)
	{

	/*$2- Filters ----------------------------------------------------------------------------------------------------*/

	case 10:
		if(mul_FilterPostIt & PIT_READ)
			mul_FilterPostIt &= ~PIT_READ;
		else
			mul_FilterPostIt |= PIT_READ;
		FillPostIt();
		break;
	case 11:
		if(mul_FilterPostIt & PIT_NREAD)
			mul_FilterPostIt &= ~PIT_NREAD;
		else
			mul_FilterPostIt |= PIT_NREAD;
		FillPostIt();
		break;
	case 12:
		if(mul_FilterPostIt & PIT_ME)
			mul_FilterPostIt &= ~PIT_ME;
		else
			mul_FilterPostIt |= PIT_ME;
		FillPostIt();
		break;
	case 13:
		if(mul_FilterPostIt & PIT_NME)
			mul_FilterPostIt &= ~PIT_NME;
		else
			mul_FilterPostIt |= PIT_NME;
		FillPostIt();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case 1:
		if(o_Dlg.DoModal() == IDOK)
		{
			mpo_View->mo_ListPostIt.AddTail(o_Dlg.mst_PostIt);
			FillPostIt();
		}
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case 2:
		if
		(
			M_MF()->MessageBox
				(
					"Are you you sure you want to destroy it ?",
					"Please Confirm",
					MB_YESNO | MB_ICONQUESTION
				) != IDYES
		) return;

		sel = po_List->GetCurSel();
		pos = (POSITION) po_List->GetItemData(sel);
		pit = mpo_View->mo_ListPostIt.GetAt(pos);
		if(L_strcmpi(pit.az_Owner, M_MF()->mst_Ini.asz_CurrentUserName) && !(pit.ul_Flags & PIT_C_NoReadOnly))
		{
			M_MF()->MessageBox("You are not the owner of the postit !", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		mpo_View->PostItPath(az);
		L_strcat(az, "/");
		L_strcat(az, pit.az_Name);
		DeleteFile(az);
		po_List->DeleteString(sel);
		mpo_View->mo_ListPostIt.RemoveAt(pos);
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case 5:
		mpo_View->KillPostIt();
		mpo_View->LoadPostIt();
		FillPostIt();
		break;
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnPostItDblClk(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_PostItDialog	o_Dlg(mpst_World, mpo_View);
	int						sel;
	POSITION				pos;
	CListBox				*po_List;
	CString					o_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListBox *) GetDlgItem(IDC_LIST_POSTIT);
	sel = po_List->GetCurSel();
	pos = (POSITION) po_List->GetItemData(sel);
	o_Dlg.mpt_Org = &mpo_View->mo_ListPostIt.GetAt(pos);
	mpo_View->PostItRefresh(o_Dlg.mpt_Org);
	L_memcpy(&o_Dlg.mst_PostIt, o_Dlg.mpt_Org, sizeof(o_Dlg.mst_PostIt));
	if(o_Dlg.DoModal() == IDOK)
	{
		o_Name = o_Dlg.mst_PostIt.az_Name;
		*L_strrchr((char *) (LPCSTR) o_Name, '.') = 0;
		o_Dlg.mst_PostIt.ul_Flags &= ~PIT_C_NoRead;
		mpo_View->mo_ListPostIt.SetAt(pos, o_Dlg.mst_PostIt);
		sel = po_List->InsertString(sel, o_Name);
		po_List->SetItemData(sel, (int) pos);
		po_List->DeleteString(sel + 1);
		LINK_Refresh();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SelectionDialog::OnGaoF(void)
{
	CString str;
	CString resToken;
	int curPos;
	int i ;

	GetDlgItem(IDC_EDIT_GAOF)->GetWindowText(mo_GaoF);
	mo_GaoF.MakeLower();

	for(i=0; i<10; i++) mo_GaoFregexp[i].Empty();
	str = mo_GaoF;
	
	i = 0;curPos= 0;
	resToken = str.Tokenize("*",curPos);
	while( (resToken != "") && (i<10) )
	{
		mo_GaoFregexp[i++] = resToken;
		resToken= str.Tokenize("*",curPos);
	};

	UpdateList();
}

BOOL EDIA_cl_SelectionDialog::GaoNameIsSkipped(CString & str)
{
	int i, j, k;
	str.MakeLower();

	if(mo_GaoFregexp[0].IsEmpty() && !mo_GaoF.IsEmpty())
	{
		if(str.Find(mo_GaoF) == -1) return TRUE;
	}
	else
	{
		for(i=j=0; (i<10) && !mo_GaoFregexp[i].IsEmpty(); i++)
		{
			k = str.Find(mo_GaoFregexp[i], j);
			if(k == -1) return TRUE;
			j=k+1;
		}
	}
	return FALSE;
}

#endif /* ACTIVE_EDITORS */




















