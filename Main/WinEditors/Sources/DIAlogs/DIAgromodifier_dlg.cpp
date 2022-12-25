/*$T DIAgromodifier_dlg.cpp GC! 1.075 03/06/00 14:56:30 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAgromodifier_dlg.h"
#include "Res/Res.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_GroModifierDialog, EDIA_cl_BaseDialog)
    ON_LBN_DBLCLK(IDC_LIST_MOD, OnOK)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GroModifierDialog::EDIA_cl_GroModifierDialog(void) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_GROMODIFIER)
{
    mi_Modifier = -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GroModifierDialog::~EDIA_cl_GroModifierDialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GroModifierDialog::OnInitDialog(void)
{
    EDIA_cl_BaseDialog::OnInitDialog();
    InitList();
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroModifierDialog::OnOK(void)
{
    /*~~~~~~~~~~~~~~~~~*/
    CListBox    *po_List;
    /*~~~~~~~~~~~~~~~~~*/

    po_List = (CListBox *) GetDlgItem(IDC_LIST_MOD);
    mi_Modifier = po_List->GetCurSel();
    if(mi_Modifier != LB_ERR)
        mi_Modifier = po_List->GetItemData(mi_Modifier);
    else
        mi_Modifier = -1;
    EDIA_cl_BaseDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroModifierDialog::InitList(void)
{
    /*~~~~~~~~~~~~~~~~~*/
    CListBox    *po_List;
    ULONG       ul_Res;
    int         i;
    /*~~~~~~~~~~~~~~~~~*/

    po_List = (CListBox *) GetDlgItem(IDC_LIST_MOD);
    po_List->SetRedraw(FALSE);
    po_List->ResetContent();

    for(i = 0; i < MDF_C_Modifier_Number; i++)
    {
        if ( L_strcmp(MDF_gasz_ModifierName[i], "FREE") == 0) continue;
        ul_Res = po_List->AddString(MDF_gasz_ModifierName[i]);
        po_List->SetItemData(ul_Res, i);
    }

    po_List->SetRedraw(TRUE);
}

#endif /* ACTIVE_EDITORS */
