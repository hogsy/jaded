//_ **********************************************************
//_ 
//_ Name: EnTabCtrl.h 
//_ Purpose: 
//_ Created: 15 September 1998 
//_ Author: D.R.Godson
//_ Modified By: 
//_ 
//_ Copyright (c) 1998 Brilliant Digital Entertainment Inc. 
//_ 
//_ **********************************************************

#if !defined(AFX_ENTABCTRL_H__38F5C380_E2DA_11D1_AB24_0000E8425C3E__INCLUDED_)
#define AFX_ENTABCTRL_H__38F5C380_E2DA_11D1_AB24_0000E8425C3E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EnTabCtrl.h : header file
//

extern COLORREF Darker(COLORREF crBase, float fFactor);
extern COLORREF Lighter(COLORREF crBase, float fFactor);

#include "basetabctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CEnTabCtrl window

// custom look
enum
{
	ETC_FLAT = 1, 
	ETC_COLOR = 2, // draw tabs with color
	ETC_SELECTION = 4, // highlight the selected tab
	ETC_GRADIENT = 8, // draw colors with a gradient
	ETC_BACKTABS = 16,
};

class CEnTabCtrl : public CBaseTabCtrl
{
// Construction
public:
	CEnTabCtrl();

	static void EnableCustomLook(BOOL bEnable = TRUE, DWORD dwStyle = ETC_FLAT | ETC_COLOR);

// Attributes
public:

protected:
	static DWORD s_dwCustomLook;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnTabCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEnTabCtrl();

	// Generated message map functions
protected:
	virtual void DrawMainBorder(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawItemBorder(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual COLORREF GetTabColor(BOOL bSelected = FALSE);
	virtual COLORREF GetTabTextColor(BOOL bSelected = FALSE);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTABCTRL_H__38F5C380_E2DA_11D1_AB24_0000E8425C3E__INCLUDED_)
