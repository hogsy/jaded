// hangeNumbersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hangeNumbersDlg.h"
#include ".\hangenumbersdlg.h"


// ChangeNumbersDlg dialog

IMPLEMENT_DYNAMIC(ChangeNumbersDlg, CDialog)
ChangeNumbersDlg::ChangeNumbersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ChangeNumbersDlg::IDD, pParent)
{
    m_iVersion = 0;
    m_iPatch   = 0;
}

ChangeNumbersDlg::~ChangeNumbersDlg()
{
}

BOOL ChangeNumbersDlg::OnInitDialog()
{
    BOOL bResult = CDialog::OnInitDialog();
    UpdateDisplay();
    return bResult;
}


void ChangeNumbersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void ChangeNumbersDlg::UpdateDisplay()
{
    char sz_Tmp[16];
    sprintf(sz_Tmp, "%d", m_iVersion);
    GetDlgItem(IDC_STATIC_VersionNumberValue)->SetWindowText(sz_Tmp);
    sprintf(sz_Tmp, "%d", m_iPatch);
    GetDlgItem(IDC_STATIC_PatchNumberValue2)->SetWindowText(sz_Tmp);
}

BEGIN_MESSAGE_MAP(ChangeNumbersDlg, CDialog)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VersionNumber, OnDeltaposSpinVersionnumber)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PatchNumber, OnDeltaposSpinPatchnumber)
END_MESSAGE_MAP()


// ChangeNumbersDlg message handlers

void ChangeNumbersDlg::OnDeltaposSpinVersionnumber(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    m_iVersion -= pNMUpDown->iDelta;
    if ( m_iVersion < 0 )
        m_iVersion =0;
    UpdateDisplay();

    *pResult = 0;
}

void ChangeNumbersDlg::OnDeltaposSpinPatchnumber(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    m_iPatch -= pNMUpDown->iDelta;
    if ( m_iPatch < 0 )
        m_iPatch =0;
    UpdateDisplay();

    *pResult = 0;
}

