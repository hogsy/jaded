// Dialog_ChangeString.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompiler.h"
#include "Dialog_ChangeString.h"


// CDialog_ChangeString dialog

IMPLEMENT_DYNCREATE(CDialog_ChangeString, CDialog)

CDialog_ChangeString::CDialog_ChangeString(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_ChangeString::IDD)
{
}

CDialog_ChangeString::~CDialog_ChangeString()
{
    m_TitleString = "Edit Text";
}

void CDialog_ChangeString::SetString(const char* _pz_String)
{
    if ( _pz_String )
        m_NewString.SetString(_pz_String);
}

void CDialog_ChangeString::SetTitle(const char* _pz_String)
{
    if ( _pz_String )
        m_TitleString.SetString(_pz_String);
}


void CDialog_ChangeString::DoDataExchange(CDataExchange* pDX)
{
    DDX_Text(pDX, IDC_EditChangeString, m_NewString);
    DDV_MaxChars(pDX, m_NewString, BIG_k_NameLength);
	CDialog::DoDataExchange(pDX);
}

BOOL CDialog_ChangeString::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetWindowText(m_TitleString);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CDialog_ChangeString, CDialog)
END_MESSAGE_MAP()



