/*$T DIAmessage_dlg.cpp GC!1.71 01/25/00 15:17:29 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAmessage_dlg.h"
#include "Res/Res.h"
#include "EDImainframe.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_MessageDialog, EDIA_cl_BaseDialog)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MessageDialog::EDIA_cl_MessageDialog(char *_pmo_Text, char *_pmo_Title, UINT _ui_Type) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_MESSAGE)
{
    mui_Type = _ui_Type;
    mo_Text = _pmo_Text;
    if(_pmo_Title == NULL)
        mo_Title = "Message";
    else
        mo_Title = _pmo_Title;
}

/*
 =======================================================================================================================
    Aim:    A funny big function to display a message in a dialog box. The dialog must resize to
            fit the message...
 =======================================================================================================================
 */
BOOL EDIA_cl_MessageDialog::OnInitDialog(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_RectDialog, o_RectButton, o_Temp;
    CButton *po_Button;
    char    *psz_Text, *psz_LastText;
    int     i_NumCR;
    CDC     *pDC;
    CRect   o_InitRect, o_InitStaticRect;
    CStatic *po_Static;
    CSize   mo_Size;
    int     i_Div, i_MinCx;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    EDIA_cl_BaseDialog::OnInitDialog();

    po_Static = (CStatic *) GetDlgItem(IDC_STATIC_MESSAGE);
    po_Static->GetWindowRect(&o_InitStaticRect);
    pDC = po_Static->GetDC();

    /*
     * Count the number of '\n' in string. Determin the minimal width of the static
     * that contains text.
     */
    i_NumCR = 1;
    i_MinCx = (o_InitStaticRect.right - o_InitStaticRect.left);
    psz_Text = psz_LastText = (char *) (LPCSTR) mo_Text;
    while(psz_LastText)
    {
        psz_Text = L_strchr(psz_Text, '\n');
        if(psz_Text) *psz_Text = 0;

        mo_Size = pDC->GetOutputTextExtent(psz_LastText);
        if(mo_Size.cx > i_MinCx)
        {
            i_MinCx = mo_Size.cx;
        }

        if(psz_Text)
        {
            *psz_Text = '\n';
            psz_Text++;
        }

        psz_LastText = psz_Text;
        if(psz_Text && *psz_Text) i_NumCR++;
    }

    if(mo_Text.GetLength())
    {
        if(mo_Text[mo_Text.GetLength() - 1] == '\n')
            i_NumCR--;
    }

    if(i_NumCR == 0) i_NumCR++;

    i_MinCx = (i_MinCx - (o_InitStaticRect.right - o_InitStaticRect.left));
    i_MinCx /= 2;

    /* Get height of a line of text */
    mo_Size = pDC->GetOutputTextExtent(mo_Text);
    i_NumCR *= mo_Size.cy;
    po_Static->ReleaseDC(pDC);

    /* Move window depending */
    i_Div = (o_InitStaticRect.bottom - o_InitStaticRect.top) - i_NumCR;

    /* Resize dialog */
    GetWindowRect(&o_InitRect);
    o_InitRect.bottom -= i_Div;
    o_InitRect.right += i_MinCx;
    MoveWindow(&o_InitRect);
    CenterWindow(AfxGetMainWnd());

    /* Resize statics */
    GetDlgItem(IDC_STATIC_TURN)->GetWindowRect(&o_InitRect);
    o_InitRect.bottom -= i_Div;
    o_InitRect.right += i_MinCx;
    ScreenToClient(&o_InitRect);
    GetDlgItem(IDC_STATIC_TURN)->MoveWindow(&o_InitRect);

    GetDlgItem(IDC_STATIC_MESSAGE)->GetWindowRect(&o_InitRect);
    o_InitRect.bottom -= i_Div;
    o_InitRect.right += i_MinCx;
    ScreenToClient(&o_InitRect);
    GetDlgItem(IDC_STATIC_MESSAGE)->MoveWindow(&o_InitRect);

    /* Move buttons up */
    GetDlgItem(IDYES)->GetWindowRect(&o_InitRect);
    o_InitRect.top -= i_Div;
    o_InitRect.bottom -= i_Div;
    ScreenToClient(&o_InitRect);
    GetDlgItem(IDYES)->MoveWindow(&o_InitRect);

    GetDlgItem(IDNO)->GetWindowRect(&o_InitRect);
    o_InitRect.top -= i_Div;
    o_InitRect.bottom -= i_Div;
    ScreenToClient(&o_InitRect);
    GetDlgItem(IDNO)->MoveWindow(&o_InitRect);

    GetDlgItem(IDOK)->GetWindowRect(&o_InitRect);
    o_InitRect.top -= i_Div;
    o_InitRect.bottom -= i_Div;
    ScreenToClient(&o_InitRect);
    GetDlgItem(IDOK)->MoveWindow(&o_InitRect);

    GetDlgItem(IDCANCEL)->GetWindowRect(&o_InitRect);
    o_InitRect.top -= i_Div;
    o_InitRect.bottom -= i_Div;
    ScreenToClient(&o_InitRect);
    GetDlgItem(IDCANCEL)->MoveWindow(&o_InitRect);

    /* Texts. */
    ((CStatic *) GetDlgItem(IDC_STATIC_MESSAGE))->SetFont(&M_MF()->mo_Fnt);
    ((CStatic *) GetDlgItem(IDC_STATIC_MESSAGE))->SetWindowText(mo_Text);
    SetWindowText(mo_Title);

    /* Icon. */
    if(mui_Type & MB_ICONHAND)
    {
        ((CStatic *) GetDlgItem(IDC_STATIC_ICON))->SetIcon(AfxGetApp()->LoadOEMIcon(OIC_ERROR));
    }
    else if(mui_Type & MB_ICONQUESTION)
    {
        ((CStatic *) GetDlgItem(IDC_STATIC_ICON))->SetIcon(AfxGetApp()->LoadOEMIcon(OIC_QUES));
    }
    else if(mui_Type & MB_ICONEXCLAMATION)
    {
        ((CStatic *) GetDlgItem(IDC_STATIC_ICON))->SetIcon(AfxGetApp()->LoadOEMIcon(OIC_WARNING));
    }
    else
    {
        ((CStatic *) GetDlgItem(IDC_STATIC_ICON))->SetIcon(AfxGetApp()->LoadOEMIcon(OIC_INFORMATION));
    }

    /* Buttons. */
    GetDlgItem(IDC_STATIC_MESSAGE)->GetWindowRect(&o_Temp);
    ScreenToClient(&o_Temp);
    GetClientRect(&o_RectDialog);
    o_RectDialog.right += (o_Temp.left - o_RectDialog.left);
    po_Button = (CButton *) GetDlgItem(IDYES);
    po_Button->GetClientRect(&o_RectButton);
    po_Button->GetClientRect(&o_Temp);
    po_Button->ClientToScreen(&o_Temp);
    ScreenToClient(&o_Temp);
    if((mui_Type & 15) == MB_OKCANCEL)
    {
        po_Button = (CButton *) GetDlgItem(IDOK);
        po_Button->ShowWindow(SW_SHOW);
        po_Button->SetFocus();
        po_Button->MoveWindow
            (
                (o_RectDialog.right / 2) - o_RectButton.right - 2,
                o_Temp.top,
                o_RectButton.right,
                o_RectButton.bottom
            );

        po_Button = (CButton *) GetDlgItem(IDCANCEL);
        po_Button->ShowWindow(SW_SHOW);
        po_Button->MoveWindow((o_RectDialog.right / 2) + 2, o_Temp.top, o_RectButton.right, o_RectButton.bottom);
        ((CButton *) GetDlgItem(IDYES))->ShowWindow(SW_HIDE);
        ((CButton *) GetDlgItem(IDNO))->ShowWindow(SW_HIDE);
    }
    else if((mui_Type & 15) == MB_YESNOCANCEL)
    {
        ((CButton *) GetDlgItem(IDOK))->ShowWindow(SW_HIDE);

        po_Button = (CButton *) GetDlgItem(IDYES);
        po_Button->ShowWindow(SW_SHOW);
        po_Button->SetFocus();
        po_Button->MoveWindow
            (
                (o_RectDialog.right / 3) - o_RectButton.right - 2,
                o_Temp.top,
                o_RectButton.right,
                o_RectButton.bottom
            );

        po_Button = (CButton *) GetDlgItem(IDNO);
        po_Button->ShowWindow(SW_SHOW);
        po_Button->MoveWindow((o_RectDialog.right / 3) + 2, o_Temp.top, o_RectButton.right, o_RectButton.bottom);

        po_Button = (CButton *) GetDlgItem(IDCANCEL);
        po_Button->ShowWindow(SW_SHOW);
        po_Button->MoveWindow
            (
                (o_RectDialog.right / 3) + o_RectButton.right + 6,
                o_Temp.top,
                o_RectButton.right,
                o_RectButton.bottom
            );
    }
    else if((mui_Type & 15) == MB_YESNO)
    {
        ((CButton *) GetDlgItem(IDOK))->ShowWindow(SW_HIDE);
        ((CButton *) GetDlgItem(IDCANCEL))->ShowWindow(SW_HIDE);

        po_Button = (CButton *) GetDlgItem(IDYES);
        po_Button->ShowWindow(SW_SHOW);
        po_Button->SetFocus();
        po_Button->MoveWindow
            (
                (o_RectDialog.right / 2) - o_RectButton.right - 2,
                o_Temp.top,
                o_RectButton.right,
                o_RectButton.bottom
            );

        po_Button = (CButton *) GetDlgItem(IDNO);
        po_Button->ShowWindow(SW_SHOW);
        po_Button->MoveWindow((o_RectDialog.right / 2) + 2, o_Temp.top, o_RectButton.right, o_RectButton.bottom);
    }
    else
    {
        po_Button = (CButton *) GetDlgItem(IDOK);
        po_Button->ShowWindow(SW_SHOW);
        po_Button->SetFocus();
        po_Button->MoveWindow
            (
                (o_RectDialog.right / 2) - (o_RectButton.right / 2),
                o_Temp.top,
                o_RectButton.right,
                o_RectButton.bottom
            );
        ((CButton *) GetDlgItem(IDCANCEL))->ShowWindow(SW_HIDE);
        ((CButton *) GetDlgItem(IDYES))->ShowWindow(SW_HIDE);
        ((CButton *) GetDlgItem(IDNO))->ShowWindow(SW_HIDE);
    }

    return FALSE;
}

#endif /* ACTIVE_EDITORS */
