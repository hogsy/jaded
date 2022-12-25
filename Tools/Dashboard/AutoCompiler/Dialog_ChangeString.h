#pragma once

#include "BigFileUtils.h"

// CDialog_ChangeString dialog

class CDialog_ChangeString : public CDialog
{
	DECLARE_DYNCREATE(CDialog_ChangeString)

public:
	CDialog_ChangeString(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialog_ChangeString();

    void    SetString(const char* _pz_String);
    void    SetTitle (const char* _pz_String);

// Dialog Data
	enum { IDD = IDD_DIALOG_ChangeString };

    CString	m_NewString;
    CString	m_TitleString;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
