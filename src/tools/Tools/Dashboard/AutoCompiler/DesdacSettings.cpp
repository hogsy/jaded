// DesdacSettings.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompiler.h"
#include "DesdacSettings.h"
#include ".\desdacsettings.h"
#include "Dialog_ChangeString.h"


// CDesdacSettings dialog

IMPLEMENT_DYNAMIC(CDesdacSettings, CDialog)
CDesdacSettings::CDesdacSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDesdacSettings::IDD, pParent)
{
}

CDesdacSettings::~CDesdacSettings()
{
}

BOOL CDesdacSettings::OnInitDialog()
{
    BOOL bResult = CDialog::OnInitDialog();
    UpdateDisplay();
    return bResult;
}

void CDesdacSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CDesdacSettings::SetDesdac(cl_Desdac * _po_Desdac)
{
    mpo_Desdac = _po_Desdac;
}

void CDesdacSettings::UpdateDisplay()
{
    GetDlgItem(IDC_BUTTON_PERFORCESERVER)->SetWindowText(mpo_Desdac->pz_GetPerforceServer());
    GetDlgItem(IDC_BUTTON_PERFORCEBRANCH)->SetWindowText(mpo_Desdac->pz_GetPerforceBranch());
    GetDlgItem(IDC_BUTTON_PERFORCEUSER  )->SetWindowText(mpo_Desdac->pz_GetPerforceUser()  );
    //GetDlgItem(IDC_BUTTON_CLIENTPASSWORD)->SetWindowText(mpo_Desdac->pz_GetPassword()      );
    GetDlgItem(IDC_BUTTON_YETIEXENAME   )->SetWindowText(mpo_Desdac->pz_GetExeName()       );
    GetDlgItem(IDC_BUTTON_BIGFILE       )->SetWindowText(mpo_Desdac->pz_GetBigFileName()   );
	GetDlgItem(IDC_BUTTON_JADESOL       )->SetWindowText(mpo_Desdac->pz_GetJadeSourcesPath()   );
    GetDlgItem(IDC_BUTTON_PCSERVER      )->SetWindowText(mpo_Desdac->pz_GetPcServer()      );

    char sz_Tmp[256];
    sprintf(sz_Tmp, "%d", mpo_Desdac->iGetNbClients());
    GetDlgItem(IDC_STATIC_NBCLIENTSVALUE)->SetWindowText(sz_Tmp);
    ((CButton*)GetDlgItem(IDC_CHECK_SERVER))->SetCheck(mpo_Desdac->bIsServer() ? 1 : 0);
    if ( mpo_Desdac->bIsServer() )
    {
        //////////////////////
        // List of all clients
        GetDlgItem(IDC_LIST_CLIENTS)->EnableWindow(TRUE);
        CListBox * pList = (CListBox*)GetDlgItem(IDC_LIST_CLIENTS);
        pList->ResetContent();
        for ( int i = 0; i < mpo_Desdac->iGetNbClients(); ++i )
        {
            pList->AddString(mpo_Desdac->pz_GetClient(i));
        }

        GetDlgItem(IDC_BUTTON_ADDCLIENT   )->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_REMOVECLIENT)->EnableWindow(TRUE);

        GetDlgItem(IDC_BUTTON_PCSERVER)->EnableWindow(FALSE);

        ////////////////////
        // List of all mails
        GetDlgItem(IDC_LIST_Mails)->EnableWindow(TRUE);
        pList = (CListBox*)GetDlgItem(IDC_LIST_Mails);
        pList->ResetContent();
        for ( int i = 0; i < mpo_Desdac->iGetNbMailToSend(); ++i )
        {
            pList->AddString(mpo_Desdac->pz_GetMailToSend(i));
        }
        GetDlgItem(IDC_BUTTON_AddMail     )->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_RemoveMail  )->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_FlushCLStack)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_LIST_CLIENTS       )->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_ADDCLIENT   )->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_REMOVECLIENT)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_PCSERVER    )->EnableWindow(TRUE);
        GetDlgItem(IDC_LIST_Mails         )->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_AddMail     )->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_RemoveMail  )->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_FlushCLStack)->EnableWindow(FALSE);
    }

    CSliderCtrl * pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SYNCDEPTH);
    pSlider->SetRange(0, 5, TRUE);
    pSlider->SetPos(mpo_Desdac->iGetSyncDepth());
}

BEGIN_MESSAGE_MAP(CDesdacSettings, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_PERFORCESERVER, OnBnClickedButtonPerforceserver)
    ON_BN_CLICKED(IDC_BUTTON_PERFORCEBRANCH, OnBnClickedButtonPerforcebranch)
    ON_BN_CLICKED(IDC_BUTTON_PERFORCEUSER, OnBnClickedButtonPerforceuser)
    ON_BN_CLICKED(IDC_BUTTON_PasswordFile, OnBnClickedButtonPasswordfile)
    ON_BN_CLICKED(IDC_BUTTON_YETIEXENAME, OnBnClickedButtonYetiexename)
    ON_BN_CLICKED(IDC_BUTTON_BIGFILE, OnBnClickedButtonBigfile)
    ON_BN_CLICKED(IDC_BUTTON_ADDCLIENT, OnBnClickedButtonAddclient)
    ON_BN_CLICKED(IDC_CHECK_SERVER, OnBnClickedCheckServer)
    ON_BN_CLICKED(IDC_BUTTON_PCSERVER, OnBnClickedButtonPcserver)
    ON_BN_CLICKED(IDC_BUTTON_REMOVECLIENT, OnBnClickedButtonRemoveclient)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SYNCDEPTH, OnNMCustomdrawSliderSyncdepth)
    ON_BN_CLICKED(IDC_BUTTON_CLIENTPASSWORD, OnBnClickedButtonClientpassword)
    ON_BN_CLICKED(IDC_BUTTON_AddMail, OnBnClickedButtonAddmail)
    ON_BN_CLICKED(IDC_BUTTON_RemoveMail, OnBnClickedButtonRemovemail)
    ON_BN_CLICKED(IDC_BUTTON_FlushCLStack, OnBnClickedButtonFlushclstack)
    ON_BN_CLICKED(IDC_BUTTON_RELOADINI, OnBnClickedButtonReloadini)
	ON_BN_CLICKED(IDC_BUTTON_JADESOL, OnBnClickedButtonJadesol)
END_MESSAGE_MAP()


// CDesdacSettings message handlers

void CDesdacSettings::OnBnClickedButtonPerforceserver()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(mpo_Desdac->pz_GetPerforceServer());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->SetPerforceServer(az_NewString);
        GetDlgItem(IDC_BUTTON_PERFORCESERVER)->SetWindowText(mpo_Desdac->pz_GetPerforceServer());
    }
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonPerforcebranch()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(mpo_Desdac->pz_GetPerforceBranch());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->SetPerforceBranch(az_NewString);
        GetDlgItem(IDC_BUTTON_PERFORCESERVER)->SetWindowText(mpo_Desdac->pz_GetPerforceBranch());
    }
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonPerforceuser()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(mpo_Desdac->pz_GetPerforceUser());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->SetPerforceUser(az_NewString);
        GetDlgItem(IDC_BUTTON_PERFORCESERVER)->SetWindowText(mpo_Desdac->pz_GetPerforceUser());
    }
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonPasswordfile()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(mpo_Desdac->pz_GetPassword());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->SetPassword(az_NewString);
        GetDlgItem(IDC_BUTTON_PERFORCESERVER)->SetWindowText(mpo_Desdac->pz_GetPassword());
    }
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonYetiexename()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(mpo_Desdac->pz_GetExeName());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->SetExeName(az_NewString);
        GetDlgItem(IDC_BUTTON_PERFORCESERVER)->SetWindowText(mpo_Desdac->pz_GetExeName());
    }
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonBigfile()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(mpo_Desdac->pz_GetBigFileName());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->SetBigFileName(az_NewString);
        GetDlgItem(IDC_BUTTON_PERFORCESERVER)->SetWindowText(mpo_Desdac->pz_GetBigFileName());
    }
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonAddclient()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(kaz_DESDAC_DefaultPcClient);
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->AddClientPC(az_NewString);
    }
    mpo_Desdac->UpdateListOfAwareClients();
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedCheckServer()
{
    CButton * pButton = (CButton*)GetDlgItem(IDC_CHECK_SERVER);
    mpo_Desdac->SetIsServer(pButton->GetCheck() ? true : false);
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonPcserver()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(mpo_Desdac->pz_GetPcServer());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->SetPcServer(az_NewString);
        GetDlgItem(IDC_BUTTON_PERFORCESERVER)->SetWindowText(mpo_Desdac->pz_GetPcServer());
    }
    UpdateDisplay();
}

void CDesdacSettings::OnEnChangeEditTimervalue()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void CDesdacSettings::OnBnClickedButtonOuputfolder()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(mpo_Desdac->pz_GetOutputFolder());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->SetOutputFolder(az_NewString);
        GetDlgItem(IDC_BUTTON_PERFORCESERVER)->SetWindowText(mpo_Desdac->pz_GetOutputFolder());
    }
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonRemoveclient()
{
    CListBox * pList = (CListBox*)GetDlgItem(IDC_LIST_CLIENTS);
    int iSelected = pList->GetCurSel();
    if ( iSelected != -1 )
    {
        mpo_Desdac->RemoveClientPC(iSelected);
        UpdateDisplay();
    }
}

void CDesdacSettings::OnBnClickedButtonDefaultmail()
{
}

void CDesdacSettings::OnNMCustomdrawSliderSyncdepth(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
    CSliderCtrl * pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SYNCDEPTH);
    mpo_Desdac->SetSyncDepth(pSlider->GetPos());
}

void CDesdacSettings::OnBnClickedButtonClientpassword()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(mpo_Desdac->pz_GetPassword());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->SetPassword(az_NewString);
        //GetDlgItem(IDC_BUTTON_PasswordFile)->SetWindowText(mpo_Desdac->pz_GetPassword());
    }
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonAddmail()
{
    CDialog_ChangeString ChangeString(this);

    ChangeString.SetString(kaz_DESDAC_DefaultMailToSend);
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->AddMailToSend(az_NewString);
    }
    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonRemovemail()
{
    CListBox * pList = (CListBox*)GetDlgItem(IDC_LIST_Mails);
    int iSelected = pList->GetCurSel();
    if ( iSelected != -1 )
    {
        mpo_Desdac->RemoveMailToSend(iSelected);
        UpdateDisplay();
    }
}

void CDesdacSettings::OnBnClickedButtonFlushclstack()
{
    mpo_Desdac->FlushChangeListStack();
}

void CDesdacSettings::OnBnClickedButtonReloadini()
{
    mpo_Desdac->ReadInfoFromIniFile();

    UpdateDisplay();
}

void CDesdacSettings::OnBnClickedButtonJadesol()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(mpo_Desdac->pz_GetJadeSourcesPath());
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->SetJadeSourcesPath(az_NewString);
        GetDlgItem(IDC_BUTTON_JADESOL)->SetWindowText(mpo_Desdac->pz_GetJadeSourcesPath());
    }
    UpdateDisplay();
}
