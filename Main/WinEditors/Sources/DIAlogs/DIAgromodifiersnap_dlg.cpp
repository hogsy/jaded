/*$T DIAgromodifiersnap_dlg.cpp GC!1.55 01/17/00 17:38:43 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAgromodifiersnap_dlg.h"
#include "Res/Res.h"
#include "ENGine/sources/WORld/WORstruct.h"
#include "ENGine/sources/WORld/WORaccess.h"
#include "ENGine/sources/OBJects/OBJstruct.h"
#include "ENGine/sources/OBJects/OBJconst.h"
#include "GRObject/GROstruct.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "SELection/SELection.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_GroModifierSnapDialog, EDIA_cl_BaseDialog)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GroModifierSnapDialog::EDIA_cl_GroModifierSnapDialog(void) : EDIA_cl_BaseDialog(DIALOGS_IDD_GROMODIFIERSNAP)
{
    mpst_TgtGao = NULL;
    mpst_SrcGao = NULL;
    mf_Treshold = 1.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GroModifierSnapDialog::~EDIA_cl_GroModifierSnapDialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GroModifierSnapDialog::OnInitDialog(void)
{
    /*~~~~~~~~~~~~~~~*/
    BOOL    b_Init;
    /*~~~~~~~~~~~~~~~*/

    b_Init = 0;

    CDialog::OnInitDialog();
    b_Init = b_InitSrc();
    if(b_Init) b_Init &= b_InitList();
    if(!b_Init)
    {
        GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_SRCGAO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_TGTGAO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_TRESHOLD)->ShowWindow(SW_HIDE);
        GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
    }
    else
    {
        InitTresh();
        GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);
    }

    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroModifierSnapDialog::OnOK(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    CComboBox *po_CB;
    int         i;
    char        sz_Value[32];
    /*~~~~~~~~~~~~~~~~~~~~~*/

    po_CB = (CComboBox *) GetDlgItem(IDC_COMBO_TGTGAO);
    i = po_CB->GetCurSel();
    if(i != LB_ERR)
        mpst_TgtGao = (OBJ_tdst_GameObject *) po_CB->GetItemData( i );
    else
        mpst_TgtGao = NULL;

    GetDlgItem( IDC_EDIT_TRESHOLD)->GetWindowText( sz_Value, 32 );
    sscanf(sz_Value, "%f", &mf_Treshold);

    CDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GroModifierSnapDialog::b_InitSrc(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GRO_tdst_Struct *pst_Gro;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!mpst_SrcGao) return FALSE;
    if(!(mpst_SrcGao->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return FALSE;
    pst_Gro = mpst_SrcGao->pst_Base->pst_Visu->pst_Object;
    if(!pst_Gro || (pst_Gro->i->ul_Type != GRO_Geometric)) return FALSE;

    GetDlgItem(IDC_EDIT_SRCGAO)->SetWindowText(mpst_SrcGao->sz_Name);
    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDAI_Snap_AddInCombo(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Combo, ULONG _ul_NotThisOne)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject *pst_GO;
    CComboBox           *po_CB;
    GRO_tdst_Struct     *pst_Gro;
    int                 i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;

    if(!pst_GO) return TRUE;
    if((ULONG) pst_GO == _ul_NotThisOne) return TRUE;
    if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return TRUE;
    pst_Gro = pst_GO->pst_Base->pst_Visu->pst_Object;
    if(!pst_Gro || (pst_Gro->i->ul_Type != GRO_Geometric)) return TRUE;

    po_CB = (CComboBox *) _ul_Combo;
    i = po_CB->AddString(pst_GO->sz_Name);
    po_CB->SetItemData(i, (DWORD) pst_GO);
    return TRUE;
}

/**/
BOOL EDIA_cl_GroModifierSnapDialog::b_InitList(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CComboBox       *po_CB;
    WOR_tdst_World  *pst_World;
    int             i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_World = WOR_World_GetWorldOfObject(mpst_SrcGao);
    if(!pst_World) return FALSE;

    po_CB = (CComboBox *) GetDlgItem(IDC_COMBO_TGTGAO);
    po_CB->SetRedraw(FALSE);
    po_CB->ResetContent();

    if ( mpst_TgtGao )
    {
        i = po_CB->AddString(mpst_TgtGao->sz_Name);
        po_CB->SetItemData(i, (DWORD) mpst_TgtGao);
    }
    else
        SEL_EnumItem(pst_World->pst_Selection, SEL_C_SIF_Object, EDAI_Snap_AddInCombo, (ULONG) po_CB, (ULONG) mpst_SrcGao);

    po_CB->SetRedraw(TRUE);

    if (po_CB->GetCount())
    {
        po_CB->SetCurSel(0);
        return TRUE;
    }

    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GroModifierSnapDialog::InitTresh(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
    char    sz_String[32];
    /*~~~~~~~~~~~~~~~~~~~~~~*/

    sprintf(sz_String, "%g", mf_Treshold);
    GetDlgItem(IDC_EDIT_TRESHOLD)->SetWindowText(sz_String);
}

#endif /* ACTIVE_EDITORS */
