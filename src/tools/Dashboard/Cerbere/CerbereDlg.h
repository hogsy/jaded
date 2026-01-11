// CerbereDlg.h : header file
//

#pragma once

#include "Main.h"
#include "SystemTray.h"
#include "Cerbere.h"
#include "splash.h"




class CCerbereDlg;

/////////////////////////
class clList : public CListCtrl 
{
public:
    clList() : mpo_Parent(NULL) {}

    inline void     SetParent(CCerbereDlg *_po_Parent) {mpo_Parent = _po_Parent; }

protected:
    afx_msg void    OnLButtonDblClk(UINT nFlags, CPoint point);

    DECLARE_MESSAGE_MAP()

private:
    CCerbereDlg * mpo_Parent;
};

class clListCol  
{
public:
    // columns ids for the listctrl
    int mi_PCname;
    int mi_TestType;
    int mi_Status;
    int mi_Results;
    int mi_ProjectName;
    int mi_EngineVersion;
};

/////////////////////////
// CCerbereDlg dialog
class CCerbereDlg : public CDialog
{
// Construction
public:
	CCerbereDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
	enum { IDD = IDD_CERBERE_DIALOG };

    
    // public funcs
    void    UpdateDisplay();
    void    OpenReport(int _iPCIndex);


    // Implementation
protected:

    void    InitList();

    // Display updates
    void    UpdatePCStatus();
    void    UpdatePCSettings();
    void    UpdateGlobalStatus();
    void    UpdateTitle();

    //
    void    OpenSplashWnd();
    void    CloseSplashWnd();
    void    SetIconGreen()      {   m_TrayIcon.SetIconList(IDR_CerbereGreen1, IDR_CerbereGreen1);   }
    void    SetIconRed()        {   m_TrayIcon.SetIconList(IDR_CerbereRed1,   IDR_CerbereRed6);     }

	// Generated message map functions
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnFileWatchNotification(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFileWatchDelete(WPARAM wParam, LPARAM lParam);

    afx_msg void OnMaximize();
    afx_msg void OnMinimize();
    afx_msg void OnPopupQuit();
    afx_msg void OnAboutbox();

	DECLARE_MESSAGE_MAP()


private:
    // MEMBERS
    HICON               m_hIcon;
    CSystemTray         m_TrayIcon;
    CSplash             m_oSplash;
    CBitmap             m_oSplashBmp;

    char                m_szVer[32];        // this version
    cl_Cerbere          mo_Cerbere;
    DWORD               m_ulCurrentStatus;

    clList              m_oList;            //PC list
    clListCol           m_oColumns;         //PC listColumns

};
