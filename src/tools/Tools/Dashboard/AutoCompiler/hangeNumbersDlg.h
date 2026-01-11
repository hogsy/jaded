#pragma once

#include "desdac.h"
#include "resource.h"


// ChangeNumbersDlg dialog

class ChangeNumbersDlg : public CDialog
{
	DECLARE_DYNAMIC(ChangeNumbersDlg)

public:
	ChangeNumbersDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ChangeNumbersDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ChangePatchNumbers };

    int m_iVersion;
    int m_iPatch;

protected:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    void    UpdateDisplay();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDeltaposSpinVersionnumber(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposSpinPatchnumber(NMHDR *pNMHDR, LRESULT *pResult);
};
