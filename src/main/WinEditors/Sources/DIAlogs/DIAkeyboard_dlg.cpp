/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAkeyboard_dlg.h"
#include "EDImainframe.h"
#include "Res/Res.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_KeyboardDialog::EDIA_cl_KeyboardDialog(void) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_NAME)
{
    mo_Key.Empty();
    muw_Key = 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_KeyboardDialog::DoDataExchange(CDataExchange *pDX)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CEdit   *po_Edit;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    CDialog::DoDataExchange(pDX);

    SetWindowText("Press a key");
    po_Edit = (CEdit *) GetDlgItem(IDC_EDIT);
    po_Edit->SetReadOnly();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDIA_cl_KeyboardDialog::PreTranslateMessage(MSG *pMsg)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CEdit   *po_Edit;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(pMsg->message == WM_KEYDOWN)
    {
        mo_Key = M_MF()->mpo_Actions->o_KeyToString(pMsg->wParam, muw_Key);
        po_Edit = (CEdit *) GetDlgItem(IDC_EDIT);
        po_Edit->SetWindowText(mo_Key);
        return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}
#endif  /* ACTIVE_EDITORS */
