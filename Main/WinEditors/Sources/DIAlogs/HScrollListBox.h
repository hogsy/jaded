/////////////////////////////////////////////////////////////////////////////
// HScrollListBox.h : header file
//
// Copyright (c) 2002, Nebula Technologies, Inc.
// www.nebutech.com
//
// Nebula Technologies, Inc. grants you a royalty free 
// license to use, modify and distribute this code 
// provided that this copyright notice appears on all 
// copies. This code is provided "AS IS," without a 
// warranty of any kind.
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_HSCROLLLISTBOX_H__E9948720_F12B_11D4_93CF_00105AC9B942__INCLUDED_)
#define AFX_HSCROLLLISTBOX_H__E9948720_F12B_11D4_93CF_00105AC9B942__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHScrollListBox window
/////////////////////////////////////////////////////////////////////////////
class CHScrollListBox : public CListBox
{
// Construction
public:
	CHScrollListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHScrollListBox)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHScrollListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHScrollListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	afx_msg LRESULT OnAddString(WPARAM Wparam, LPARAM LParam); // WParam - none, LParam - string, returns - int
	afx_msg LRESULT OnInsertString(WPARAM Wparam, LPARAM LParam); // WParam - index, LParam - string, returns - int 
	afx_msg LRESULT OnDeleteString(WPARAM Wparam, LPARAM LParam); // WParam - index, LParam - none, returns - int 
	afx_msg LRESULT OnResetContent(WPARAM Wparam, LPARAM LParam); // WParam - none, LParam - none, returns - int 
	afx_msg LRESULT OnDir(WPARAM Wparam, LPARAM LParam); // WParam - attr, LParam - wildcard, returns - int 

	DECLARE_MESSAGE_MAP()

private:
	void ResetHExtent();
	void SetNewHExtent(LPCTSTR lpszNewString);
	int GetTextLen(LPCTSTR lpszText);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HSCROLLLISTBOX_H__E9948720_F12B_11D4_93CF_00105AC9B942__INCLUDED_)
