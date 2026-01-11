/*$T DIAmsglink_dlg.cpp GC!1.52 11/17/99 12:21:20 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAmsglink_dlg.h"
#include "Res/Res.h"
#include "EDImainframe.h"

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_MsgLinkDialog, EDIA_cl_BaseDialog)
    ON_COMMAND(IDYES, OnYes)
    ON_COMMAND(IDNO, OnNo)
END_MESSAGE_MAP()

/*
 ===================================================================================================
 ===================================================================================================
 */
EDIA_cl_MsgLinkDialog::EDIA_cl_MsgLinkDialog(char *_po_Text, char *_po_Title) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_LINKMESSAGE)
{
    mo_Text = _po_Text;
    mo_Title = _po_Title;
    mo_Name = "";
    mi_LastRes = -1;
}

/*
 ===================================================================================================
    Aim:    A funny big function to display a message in a dialog box. The dialog must resize to
            fit the message...
 ===================================================================================================
 */
BOOL EDIA_cl_MsgLinkDialog::OnInitDialog(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CStatic *po_Static;
    CString o_Str;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    EDIA_cl_BaseDialog::OnInitDialog();
    SetWindowText(mo_Title);
    po_Static = (CStatic *) GetDlgItem(IDC_STATIC_MESSAGE);
    o_Str = mo_Name + "\n";
    o_Str += mo_Text;
    po_Static->SetWindowText(o_Str);

    return FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_MsgLinkDialog::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK1, mb_ApplyToAll);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_MsgLinkDialog::OnYes(void)
{
    UpdateData();
    EndDialog(IDYES);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDIA_cl_MsgLinkDialog::OnNo(void)
{
    UpdateData();
    EndDialog(IDNO);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int EDIA_cl_MsgLinkDialog::DoModal(void)
{
    if(mb_ApplyToAll) return mi_LastRes;
    mi_LastRes = EDIA_cl_BaseDialog::DoModal();
    return mi_LastRes;
}

#endif /* ACTIVE_EDITORS */
