// AutoCompilerDlg.h : header file
//

#pragma once

#include "desDac.h"

// CAutoCompilerDlg dialog
class CAutoCompilerDlg : public CDialog
{
// Construction
public:
	CAutoCompilerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AUTOCOMPILER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
    void    UpdateDisplay();
    void    UpdateClientDisplay();

private:
    void    Suspend();
// Implementation
protected:
	HICON m_hIcon;
    cl_Desdac   mo_Desdac;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedButtonServerport();
    afx_msg void OnBnClickedButtonServerbranch();
    afx_msg void OnBnClickedButtonUsername();
    afx_msg void OnBnClickedButtonExename();
    afx_msg void OnBnClickedButtonSuspend();
    afx_msg void OnBnClickedButtonChangesettings();
    afx_msg void OnBnClickedButtonForceupdateversion();
    afx_msg void OnBnClickedCheckMaptestingEnable();
    afx_msg void OnBnClickedCheckMaptestingXenonEnable();
    afx_msg void OnBnClickedCheckTestlastCLonly();
    afx_msg void OnBnClickedButtonMaptestingconfiguration();
    afx_msg void OnBnClickedCheckSendmails();
    afx_msg void OnBnClickedCheckSendmailsuser();	
	afx_msg void OnBnClickedCheckAitoc();
};
