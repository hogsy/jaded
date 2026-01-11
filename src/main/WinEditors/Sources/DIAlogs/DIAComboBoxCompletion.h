#if !defined(AFX_COMBOBOXEX_H__115F422E_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_COMBOBOXEX_H__115F422E_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// DIAComboBoxCompletion.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComboBoxCompletion window

class CComboBoxCompletion : public CComboBox
{
// Construction
public:
	CComboBoxCompletion();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxCompletion)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboBoxCompletion();

	void FillCombo(ULONG);
	BOOL IsNamePresent(CString&);

	BOOL m_bAutoComplete;

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxCompletion)
	afx_msg void OnEditUpdate();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOBOXEX_H__115F422E_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_)
