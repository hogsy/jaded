#pragma once


#include "desdac.h"

// CDesdacSettings dialog

class CDesdacSettings : public CDialog
{
	DECLARE_DYNAMIC(CDesdacSettings)

public:
	CDesdacSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDesdacSettings();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTINGS };
    void    SetDesdac(cl_Desdac * _po_Desdac);
protected:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    cl_Desdac *         mpo_Desdac;
    void    UpdateDisplay();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonPerforceserver();
    afx_msg void OnBnClickedButtonPerforcebranch();
    afx_msg void OnBnClickedButtonPerforceuser();
    afx_msg void OnBnClickedButtonPasswordfile();
    afx_msg void OnBnClickedButtonYetiexename();
    afx_msg void OnBnClickedButtonBigfile();
    afx_msg void OnBnClickedButtonAddclient();
    afx_msg void OnBnClickedCheckServer();
    afx_msg void OnBnClickedButtonPcserver();
    afx_msg void OnEnChangeEditTimervalue();
    afx_msg void OnBnClickedButtonOuputfolder();
    afx_msg void OnBnClickedButtonRemoveclient();
    afx_msg void OnBnClickedButtonDefaultmail();
    afx_msg void OnNMCustomdrawSliderSyncdepth(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonClientpassword();
    afx_msg void OnBnClickedButtonAddmail();
    afx_msg void OnBnClickedButtonRemovemail();
    afx_msg void OnBnClickedButtonFlushclstack();
    afx_msg void OnBnClickedButtonReloadini();
	afx_msg void OnBnClickedButtonJadesol();
};
