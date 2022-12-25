/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "BASe/CLIbrary/CLIstr.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "DIAfavorites_dlg.h"

/*$4
 ***************************************************************************************************
    GLOBAL VARS
 ***************************************************************************************************
 */

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_OrganizeFavorites, EDIA_cl_BaseDialog)
    ON_LBN_SELCHANGE(IDC_LIST1, OnSelChange)
    ON_EN_KILLFOCUS(IDC_EDIT1, OnEditChange)
    ON_COMMAND(IDDELETE, OnDelete)
    ON_COMMAND(IDDELETEALL, OnDeleteAll)
END_MESSAGE_MAP()

/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_OrganizeFavorites::EDIA_cl_OrganizeFavorites(void) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_ORGANIZEFAVORITES)
{
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_OrganizeFavorites::DoDataExchange(CDataExchange *pDX)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int         i, mi_Res;
    BOOL        b_Ok;
    CListBox    *po_List;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    CDialog::DoDataExchange(pDX);

    po_List = (CListBox *) GetDlgItem(IDC_LIST1);

    /* Fill list box with current favorites. */
    if(pDX->m_bSaveAndValidate == FALSE)
    {
        b_Ok = FALSE;
        for(i = 0; i < i_NumFavorites; i++)
        {
            if(ast_Favorites[i].asz_DisplayName[0])
            {
                mi_Res = po_List->AddString(ast_Favorites[i].asz_DisplayName);
                po_List->SetItemData(mi_Res, i);
                b_Ok = TRUE;
            }
        }

        /* Can't delete all anymore */
        if(b_Ok == FALSE)
        {
            GetDlgItem(IDDELETE)->EnableWindow(FALSE);
            GetDlgItem(IDDELETEALL)->EnableWindow(FALSE);
        }
        else
        {
            po_List->SetFocus();
            po_List->SetCurSel(0);
            OnSelChange();
        }

        /* Limit text length */
        ((CEdit *) GetDlgItem(IDC_EDIT1))->LimitText(BIG_C_MaxLenPath - 1);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_OrganizeFavorites::OnSelChange(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CListBox    *po_List;
    int         mi_Res;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_List = (CListBox *) GetDlgItem(IDC_LIST1);
    if((mi_Res = po_List->GetCurSel()) != LB_ERR)
    {
        mi_Res = po_List->GetItemData(mi_Res);

        /* Set display name in edit box */
        ((CEdit *) GetDlgItem(IDC_EDIT1))->SetWindowText(ast_Favorites[mi_Res].asz_DisplayName);

        /* Set path name in edit box */
        ((CStatic *) GetDlgItem(IDC_STATIC1))->SetWindowText(ast_Favorites[mi_Res].asz_PathName);

        /* Enable delete */
        GetDlgItem(IDDELETE)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDDELETE)->EnableWindow(FALSE);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_OrganizeFavorites::OnEditChange(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CEdit       *po_Edit;
    CListBox    *po_List;
    CString     o_String;
    int         mi_Res, i_Sel;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Get current selected item */
    po_List = (CListBox *) GetDlgItem(IDC_LIST1);
    if(po_List->GetCurSel() == LB_ERR)
    {
        return;
    }

    mi_Res = po_List->GetItemData(po_List->GetCurSel());

    po_Edit = (CEdit *) GetDlgItem(IDC_EDIT1);
    po_Edit->GetWindowText(o_String);
    if(!o_String.IsEmpty())
    {
        L_strcpy(ast_Favorites[mi_Res].asz_DisplayName, (char *) (LPCSTR) o_String);

        i_Sel = po_List->GetCurSel();
        po_List->DeleteString(i_Sel);
        i_Sel = po_List->AddString(o_String);
        po_List->SetItemData(i_Sel, mi_Res);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_OrganizeFavorites::OnDelete(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CListBox    *po_List;
    int         mi_Res, i_Sel;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Get current selected item */
    po_List = (CListBox *) GetDlgItem(IDC_LIST1);
    if((i_Sel = po_List->GetCurSel()) == LB_ERR)
    {
        return;
    }

    mi_Res = po_List->GetItemData(i_Sel);
    po_List->DeleteString(po_List->GetCurSel());
    ast_Favorites[mi_Res].asz_DisplayName[0] = '\0';

    /* Delete views of that item */
    ((CEdit *) GetDlgItem(IDC_EDIT1))->SetWindowText("");
    ((CEdit *) GetDlgItem(IDC_STATIC1))->SetWindowText("");

    /* Select next element */
    if(po_List->GetCount())
    {
        if(i_Sel >= po_List->GetCount())
        {
            if(i_Sel == 0)
            {
                return;
            }

            i_Sel--;
        }

        po_List->SetFocus();
        po_List->SetCurSel(i_Sel);
    }
    else
    {
        GetDlgItem(IDDELETE)->EnableWindow(FALSE);
        GetDlgItem(IDDELETEALL)->EnableWindow(FALSE);
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_OrganizeFavorites::OnDeleteAll(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CListBox    *po_List;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_List = (CListBox *) GetDlgItem(IDC_LIST1);
    po_List->ResetContent();

    /* Delete views of that item */
    ((CEdit *) GetDlgItem(IDC_EDIT1))->SetWindowText("");
    ((CEdit *) GetDlgItem(IDC_STATIC1))->SetWindowText("");

    /* Can't delete all anymore */
    GetDlgItem(IDDELETE)->EnableWindow(FALSE);
    GetDlgItem(IDDELETEALL)->EnableWindow(FALSE);

    i_NumFavorites = 0;
}
#endif  /* ACTIVE_EDITORS */
