#pragma once

#ifdef ACTIVE_EDITORS

#include <vector>
#include "DIAlogs/DIAbase.h"

// forward declarations
struct DAT_SHistoryInfo;

//////////////////////////////////////////////////////////////////////////
// EDIA_cl_HistoryDialog

class EDIA_cl_HistoryDialog : public EDIA_cl_BaseDialog
{
// construction
public:
	EDIA_cl_HistoryDialog(ULONG ul_Key);
	~EDIA_cl_HistoryDialog( );

// interface
public:

// implementation
private:
	void	UpdateText( const DAT_SHistoryInfo* pInfo );
	void	UpdateUI( );

// overrides
public:
	void    DoDataExchange(CDataExchange *);
	BOOL    OnInitDialog(void);
	BOOL	OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
	void	OnCancel( );

// fields
private:
	std::vector< DAT_SHistoryInfo* > m_lstEntries;

	ULONG		m_ulKey;
	CListCtrl	m_ctlEntries;
	CStatic		m_ctlActionsText;
	CStatic		m_ctlDescriptionText;
	CStatic		m_ctlFileTypeText;
	CEdit		m_ctlActionsEdit;
	CEdit		m_ctlDescriptionEdit;
	CButton		m_btnView;
	CButton		m_btnSync;
	CButton		m_btnDiffRevs;
	CButton		m_btnDiffVsClient;
	CButton		m_btnDescribe;
	CButton		m_btnRecover;

	CString		m_strActionsText;
	CString		m_strDescriptionText;
	CString		m_strFileTypeText;

// message map
protected:
	afx_msg void    OnHistoryClose( void );
	afx_msg void	OnDiffRevs( void );
	afx_msg void	OnDiffVsClient( void );
	afx_msg void    OnView( void );
	afx_msg void    OnSync( void );
	afx_msg void    OnDescribe( void );
	afx_msg void    OnRecover( void );
	afx_msg void	OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

extern bool g_bSynchedInHistory;

#endif /* ACTIVE_EDITORS */
