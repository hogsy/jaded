#pragma once

#include "desdac.h"

// CMapTestingConfig dialog

class CMapTestingConfig : public CDialog
{
	DECLARE_DYNAMIC(CMapTestingConfig)

public:
	CMapTestingConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMapTestingConfig();

// Dialog Data
	enum { IDD = IDD_DIALOG_MapTesting };

    void    SetDesdac(cl_Desdac * _po_Desdac);

protected:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    cl_Desdac *         mpo_Desdac;
    void    UpdateDisplay();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonAddmaptotest();
    afx_msg void OnBnClickedButtonRemovemaptotest();
	afx_msg void OnBnClickedOk();
};
