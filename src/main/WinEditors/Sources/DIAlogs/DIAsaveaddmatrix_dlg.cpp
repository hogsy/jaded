/*$T DIAsaveaddmatrix_dlg.cpp GC! 1.081 12/17/01 16:52:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "DIAlogs/DIAsaveaddmatrix_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "Engine/Sources/MODifier/MDFmodifier_GAO.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dframe.h"
#include "EDIapp.h"
#include "EDIeditors_infos.h"
#include "Editors/Sources/OUTput/OUTframe.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "LINKs/LINKtoed.h"
#include "LINks/LINKmsg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJload.h"

BEGIN_MESSAGE_MAP(EDIA_cl_SaveAddMatrixDialog, EDIA_cl_BaseDialog)
	ON_LBN_SELCHANGE(IDC_LIST_SLOT, OnSelChange_SlotList)
	ON_BN_CLICKED(IDC_BUTTON_RENAMESLOT, OnButton_SlotRename)
	ON_BN_CLICKED(IDC_BUTTON_DELSLOT, OnButton_SlotDelete)
	ON_BN_CLICKED(IDC_BUTTON_NEWSLOT, OnButton_SlotNew)
	ON_BN_CLICKED(IDC_BUTTON_UPDATESLOT, OnButton_SlotUpdate)
	ON_BN_CLICKED(IDC_BUTTON_APPLYSLOT, OnButton_SlotApply)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SaveAddMatrixDialog::EDIA_cl_SaveAddMatrixDialog(MDF_tdst_Modifier *_pst_Modifier) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_SAVEADDMATRIX)
{
        int i;

	mpst_Modifier = _pst_Modifier;
	mpst_Data = (GAO_tdst_ModifierSaveAddMatrix *) mpst_Modifier->p_Data;
	mpst_CurSlot = NULL;
	mi_CurSlot = -1;

    if (mpst_Data->c_Init == 0)
    {
        for (i = 0; i < mpst_Data->l_NbGizmo; i++)
        {
            LOA_MakeFileRef(mpst_Data->dst_Gizmo[i].ul_Key,(ULONG *) &mpst_Data->dst_Gizmo[i].pst_GO, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
            LOA_Resolve();
            if (mpst_Data->dst_Gizmo[i].pst_GO)
                mpst_Data->dst_Gizmo[i].pst_Matrix = OBJ_pst_GetAbsoluteMatrix( mpst_Data->dst_Gizmo[i].pst_GO );
            else
                mpst_Data->dst_Gizmo[i].pst_Matrix = NULL;
        }
        mpst_Data->c_Init = 1;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SaveAddMatrixDialog::~EDIA_cl_SaveAddMatrixDialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SaveAddMatrixDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	Slot_FillList();
	Gizmo_FillList();
	CenterWindow();
    Invalidate();
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SaveAddMatrixDialog::PreTranslateMessage(MSG *pMsg)
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*$4
 ***********************************************************************************************************************
    data functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::Slot_GetCur(void)
{
	/*~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	int			i_Sel;
	/*~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_SLOT);
	i_Sel = po_LB->GetCurSel();
	if(i_Sel == LB_ERR)
	{
		mi_CurSlot = -1;
		mpst_CurSlot = NULL;
		return;
	}

	mi_CurSlot = po_LB->GetItemData(i_Sel);
	mpst_CurSlot = &mpst_Data->dst_Slot[mi_CurSlot];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::Slot_FillList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	int			i_Data, i_Index, i_Sel;
	char		sz_Name[70];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_EDIT_NBSLOT)->SetWindowText(_itoa(mpst_Data->l_NbSlot, sz_Name, 10));

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_SLOT);

	i_Sel = po_LB->GetCurSel();
	if(i_Sel != LB_ERR) i_Sel = po_LB->GetItemData(i_Sel);

	po_LB->ResetContent();

	for(i_Data = 0; i_Data < mpst_Data->l_NbSlot; i_Data++)
	{
		sprintf(sz_Name, "%02d - %s", i_Data, mpst_Data->dst_Slot[i_Data].sz_Name);
		i_Index = po_LB->AddString(sz_Name);
		po_LB->SetItemData(i_Index, i_Data);
	}

	if(i_Sel != LB_ERR) Slot_Select(i_Sel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::Slot_Select(int _i_Data)
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	int			i_Index;
	/*~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_SLOT);
	for(i_Index = 0; i_Index < po_LB->GetCount(); i_Index++)
	{
		if(po_LB->GetItemData(i_Index) == (ULONG) _i_Data) po_LB->SetCurSel(i_Index);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::Slot_Display(void)
{
	Slot_GetCur();
	if(mi_CurSlot == -1)
	{
		GetDlgItem(IDC_EDIT_NAMESLOT)->SetWindowText("");
		GetDlgItem(IDC_BUTTON_RENAMESLOT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_NEWSLOT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_UPDATESLOT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DELSLOT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_APPLYSLOT)->EnableWindow(FALSE);
		return;
	}

	GetDlgItem(IDC_EDIT_NAMESLOT)->SetWindowText(mpst_CurSlot->sz_Name);
	GetDlgItem(IDC_BUTTON_RENAMESLOT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_NEWSLOT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_UPDATESLOT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DELSLOT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_APPLYSLOT)->EnableWindow(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::Slot_Delete(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(mi_CurSlot == -1) return;

	if(mpst_CurSlot->dst_Mat) MEM_Free(mpst_CurSlot->dst_Mat);

	for(i = mi_CurSlot; i < mpst_Data->l_NbSlot; i++)
	{
		L_memcpy(mpst_Data->dst_Slot + i, mpst_Data->dst_Slot + (i + 1), sizeof(GAO_tdst_ModifierSaveAddMatrix_Slot));
	}

	mpst_Data->l_NbSlot--;

    mi_CurSlot = -1;
    mpst_CurSlot = NULL;

	Slot_FillList();
    Slot_Display();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::OnButton_SlotRename(void)
{
	GetDlgItem(IDC_EDIT_NAMESLOT)->GetWindowText(mpst_CurSlot->sz_Name, 64);
	Slot_FillList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::OnButton_SlotDelete(void)
{
	Slot_Delete();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::OnButton_SlotUpdate(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < mpst_Data->l_NbGizmo; i++)
	{
        if (mpst_Data->dst_Gizmo[i].pst_Matrix)
		    MATH_CopyMatrix(&mpst_CurSlot->dst_Mat[i], mpst_Data->dst_Gizmo[i].pst_Matrix);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::OnButton_SlotNew(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierSaveAddMatrix_Slot *S;
	int									i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ( mpst_Data->l_NbGizmo == 0 )
    {
        M_MF()->MessageBox( "No gizmo, destroy and recreate modifier\nwhen object will have gizmo", "Error", MB_ICONERROR | MB_OK );
        return;
    }

	i = (mpst_Data->l_NbSlot + 1) * sizeof(GAO_tdst_ModifierSaveAddMatrix_Slot);
	if(mpst_Data->l_NbSlot == 0)
		mpst_Data->dst_Slot = (GAO_tdst_ModifierSaveAddMatrix_Slot *) MEM_p_Alloc(i);
	else
		mpst_Data->dst_Slot = (GAO_tdst_ModifierSaveAddMatrix_Slot *) MEM_p_Realloc(mpst_Data->dst_Slot, i);

	S = mpst_Data->dst_Slot + mpst_Data->l_NbSlot++;
	L_strcpy(S->sz_Name, "Unnamed");
	S->dst_Mat = (MATH_tdst_Matrix *) MEM_p_Alloc(mpst_Data->l_NbGizmo * sizeof(MATH_tdst_Matrix));

	for(i = 0; i < mpst_Data->l_NbGizmo; i++)
	{
        if (mpst_Data->dst_Gizmo[i].pst_Matrix)
		    MATH_CopyMatrix(&S->dst_Mat[i], mpst_Data->dst_Gizmo[i].pst_Matrix);
	}

	Slot_FillList();
    Slot_Select( mpst_Data->l_NbSlot - 1 );
    Slot_Display();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::OnButton_SlotApply(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(mi_CurSlot == -1) return;

	for(i = 0; i < mpst_Data->l_NbGizmo; i++)
	{
        if ( (mpst_Data->dst_Gizmo[i].pst_Matrix) && (mpst_Data->dst_Gizmo[i].pst_GO) )
		    MATH_CopyMatrix(mpst_Data->dst_Gizmo[i].pst_Matrix, &mpst_CurSlot->dst_Mat[i]);
	}

    for(i = 0; i < mpst_Data->l_NbGizmo; i++)
	{
        if ( (mpst_Data->dst_Gizmo[i].pst_Matrix) && (mpst_Data->dst_Gizmo[i].pst_GO) )
		    OBJ_ComputeLocalWhenHie(mpst_Data->dst_Gizmo[i].pst_GO);
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::OnSelChange_SlotList(void)
{
	Slot_Display();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SaveAddMatrixDialog::Gizmo_FillList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	char		sz_Name[70];
	int			i;
	/*~~~~~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_GIZMO);
	po_LB->ResetContent();

	GetDlgItem(IDC_EDIT_NBGIZMO)->SetWindowText(_itoa(mpst_Data->l_NbGizmo, sz_Name, 10));

	if(mpst_Data->c_GizmoPtr)
	{
		for(i = 0; i < mpst_Data->l_NbGizmo; i++)
		{
            if (mpst_Data->dst_Gizmo[i].pst_GO)
			    po_LB->InsertString(i, mpst_Data->dst_Gizmo[i].pst_GO->sz_Name);
            else
                po_LB->InsertString( i, "Unknown" );
		}
	}
}

#endif /* ACTIVE_EDITORS */
