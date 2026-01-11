#if !defined(AFX_TREELISTHEADERCTRL_H__99EB0481_4FA1_11D1_980A_004095E0DEFA__INCLUDED_)
#define AFX_TREELISTHEADERCTRL_H__99EB0481_4FA1_11D1_980A_004095E0DEFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// PERCheckTreeListHeaderCtrl.h : header file

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
enum ePERCheckViewType
{
	ePERCheckNone,
	ePERCheckTopLeftView,
	ePERCheckTopRightView,
	ePERCheckBottomView
};

enum ePERCheckViewFilter
{
	ePERCheckViewFilterModify		= 0x00000001,
	ePERCheckViewFilterRename		= 0x00000002,
	ePERCheckViewFilterMove			= 0x00000004,
	ePERCheckViewFilterNew			= 0x00000008,
	ePERCheckViewFilterSame			= 0x00000010,
	ePERCheckViewFilterDelete		= 0x00000020,
	ePERCheckViewFilterNewP4		= 0x00000040
};

/////////////////////////////////////////////////////////////////////////////

struct SSortType
{
	int nCol;
	BOOL bAscending;
};

/////////////////////////////////////////////////////////////////////////////

class CTLItem
{
	//private:
public:
	CString m_itemString;

public:
	CTLItem();
	CTLItem(CTLItem &copyItem);
	DWORD itemData;

	char m_cEnding;

	// visual attributes
	BOOL m_Bold;
	COLORREF m_Color;
	COLORREF m_BkColor;

	// editable attributes
	BOOL m_bIsEditable;
	BOOL m_bHasCombo;

	BOOL m_bIsFolder;

	// m_nSub is zero-based
	CString GetItemString() { return m_itemString; };
	CString GetSubstring(int m_nSub);
	CString GetItemText() { return GetSubstring(0); };
	BOOL IsFolder() {return m_bIsFolder;}
	BOOL IsEditable() { return m_bIsEditable; }; 
	BOOL HasCombo() { return m_bHasCombo; };

	void SetSubstring(int m_nSub, CString m_sText);
	void InsertItem(CString m_sText) { SetSubstring(0, m_sText); };
	void SetEditable(BOOL bEditable);
	void SetCombo(BOOL bCombo);
	void SetFolder(BOOL _bFolder){ m_bIsFolder = _bFolder ;} 
};

/////////////////////////////////////////////////////////////////////////////
// EPERCheck_cl_TreeListHeaderCtrl window

class EPERCheck_cl_TreeListHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	EPERCheck_cl_TreeListHeaderCtrl();

// Attributes
protected:
	CImageList *m_pImageList;
	CMap< int, int, int, int> m_mapImageIndex;

private:
	BOOL m_bAutofit;
	void Autofit(int nOverrideItemData = -1, int nOverrideWidth = 0);

// Operations
public:
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	CImageList* SetImageList( CImageList* pImageList );

	int GetItemImage( int nItem );
	void SetItemImage( int nItem, int nImage );
	void SetAutofit(bool bAutofit = true) { m_bAutofit = bAutofit; Autofit(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EPERCheck_cl_TreeListHeaderCtrl)	
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~EPERCheck_cl_TreeListHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(EPERCheck_cl_TreeListHeaderCtrl)
	afx_msg void OnPaint();		
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTHEADERCTRL_H__99EB0481_4FA1_11D1_980A_004095E0DEFA__INCLUDED_)
