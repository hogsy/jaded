// MapTestingConfig.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompiler.h"
#include "MapTestingConfig.h"
#include ".\maptestingconfig.h"

#include "Dialog_ChangeString.h"

// CMapTestingConfig dialog

IMPLEMENT_DYNAMIC(CMapTestingConfig, CDialog)
CMapTestingConfig::CMapTestingConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CMapTestingConfig::IDD, pParent)
{
}

CMapTestingConfig::~CMapTestingConfig()
{
}

void CMapTestingConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CMapTestingConfig::OnInitDialog()
{
    BOOL bResult = CDialog::OnInitDialog();
    UpdateDisplay();
    return bResult;
}

void CMapTestingConfig::SetDesdac(cl_Desdac * _po_Desdac)
{
    mpo_Desdac = _po_Desdac;
}

void CMapTestingConfig::UpdateDisplay()
{
    int iNb = mpo_Desdac->iGetNbMapToTest();
    // List of all clients
    CListBox * pList = (CListBox*)GetDlgItem(IDC_LIST_MapTesting);
    pList->ResetContent();
    for ( int i = 0; i < iNb; ++i )
    {
        pList->AddString(mpo_Desdac->pz_GetMapToTest(i));
    }

}

BEGIN_MESSAGE_MAP(CMapTestingConfig, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_AddMapToTest, OnBnClickedButtonAddmaptotest)
    ON_BN_CLICKED(IDC_BUTTON_RemoveMapToTest, OnBnClickedButtonRemovemaptotest)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CMapTestingConfig message handlers

void CMapTestingConfig::OnBnClickedButtonAddmaptotest()
{
    CDialog_ChangeString ChangeString;

    ChangeString.SetString(kaz_DESDAC_DefaultMapName);
    if ( ChangeString.DoModal() == IDOK )
    {
        char az_NewString[BIG_k_NameLength];
        memcpy(az_NewString,ChangeString.m_NewString,BIG_k_NameLength);
        mpo_Desdac->AddMapToTest(az_NewString);
    }
    UpdateDisplay();
}

void CMapTestingConfig::OnBnClickedButtonRemovemaptotest()
{
    CListBox * pList = (CListBox*)GetDlgItem(IDC_LIST_MapTesting);
    int iSelected = pList->GetCurSel();
    if ( iSelected != -1 )
    {
        mpo_Desdac->RemoveMapToTest(iSelected);
        UpdateDisplay();
    }
}

void CMapTestingConfig::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
