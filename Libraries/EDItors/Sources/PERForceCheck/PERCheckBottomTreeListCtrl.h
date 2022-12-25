#if !defined(AFX_PERCHECKBOTTOMTREELISTCTRL_H__B2E827F7_4D4B_11D1_980A_004095E0DEFA__INCLUDED_)
#define AFX_PERCHECKBOTTOMTREELISTCTRL_H__B2E827F7_4D4B_11D1_980A_004095E0DEFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "PERCheckTreeListHeaderCtrl.h"
#include "EDItors/Sources/PERForce/PERCDataCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// EPERCheckBottom_cl_TreeListCtrl window

#define UWM_TV_CHECKBOX    WM_APP   

class EPERCheckBottom_cl_TreeListCtrl : public CTreeCtrl, public PER_CDataCtrl
{
// Construction
public:
	EPERCheckBottom_cl_TreeListCtrl();

// Attributes
private:	
	int m_nColumns;
	int m_nColumnsWidth;
	int m_nItems;
	CDWordArray	m_dwaItems;
	CStringArray TreeLayout;
	UINT	m_nHoverTimerID;
	CArray<CTLItem*,CTLItem*> MemItemArray;
	COLORREF m_wndColor;
	CEdit *pEdit;
	CComboBox *pCombo;

public:
	EPERCheck_cl_TreeListHeaderCtrl m_wndHeader;
	CFont m_headerFont;
	CImageList m_cImageList;
	CImageList m_StatusImageList;
	
	int m_nOffset;

	enum CheckState{ NOSTATE = 0, UNCHECKED, CHECKED, CHILD_CHECKED, 
		SELF_AND_CHILD_CHECKED };
	enum CheckType{ CHECK, UNCHECK, TOGGLE, REFRESH };

// ----------------------------------------------------------------
// STRUCTURE AND METHODS TO HANDLE DATA

public:
	virtual ULONG GetItemReference(HDATACTRLITEM hItem) ;
	virtual HDATACTRLITEM GetFirstSelectedItem() const ;
	virtual HDATACTRLITEM GetNextSelectedItem(HDATACTRLITEM _hItem) const ;
	virtual BOOL ItemIsDirectory(HDATACTRLITEM _hItem) const ;

	virtual ePER_ItemReferenceType ItemReferenceType() { return eItemReferenceBIG_KEY ; } ;

// ----------------------------------------------------------------
	
// Operations
public:
	int GetColumnsNum() { return m_nColumns; };
	int GetColumnsWidth() { return m_nColumnsWidth; };
	int GetItemCount() { return m_nItems; };
	void RecalcColumnsWidth();
	
	void ResetVertScrollBar();

	HTREEITEM GetTreeItem(int nItem);
	int GetListItem(HTREEITEM hItem);

	int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	int GetColumnWidth(int nCol);
	int GetColumnAlign(int nCol);

	BOOL SetItemData(HTREEITEM hItem, DWORD dwData);
	DWORD GetItemData(HTREEITEM hItem) const;

	CString GetItemText( HTREEITEM hItem, int nSubItem = 0 );
	CString GetItemText( int nItem, int nSubItem );

	HTREEITEM InsertItem( LPCTSTR lpszItem, int nImage, int nSelectedImage, BOOL bIsEditable = FALSE, BOOL bHasCombo = FALSE, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertItem(LPCTSTR lpszItem, BOOL bIsEditable = FALSE, BOOL bHasCombo = FALSE, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST, BOOL bResetScrollBar = TRUE);
	HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter, BOOL bIsEditable = FALSE, BOOL bHasCombo = FALSE);

	HTREEITEM CopyItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);
	HTREEITEM MoveItem(HTREEITEM hItem, HTREEITEM hParent=TVI_ROOT, HTREEITEM hInsertAfter=TVI_LAST);

	BOOL DeleteSubItems( HTREEITEM hItem );
	BOOL DeleteItem( HTREEITEM hItem );
	BOOL DeleteItem( int nItem );
	void DeleteSubItemsData(HTREEITEM hParent);

	void MemDeleteAllItems(HTREEITEM hParent);
	BOOL DeleteAllItems();

	BOOL SetItemText( HTREEITEM hItem, int nCol ,LPCTSTR lpszItem );

	BOOL SetItemChildren( HTREEITEM hItem, BOOL _bChildren );

	BOOL SetItemFolder ( HTREEITEM hItem, BOOL m_bIsFolder );
	BOOL ItemIsFolder( HTREEITEM hItem );

	BOOL SetItemColor( HTREEITEM hItem, COLORREF m_newColor, BOOL m_bInvalidate = TRUE);
	BOOL SetItemBold( HTREEITEM hItem, BOOL m_Bold = TRUE, BOOL m_bInvalidate = TRUE );

	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	BOOL SortItems( int nCol, BOOL bAscending, HTREEITEM low);

	void DrawItemText (CDC* pDC, CString text, CRect rect, int nWidth, int nFormat);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EPERCheckBottom_cl_TreeListCtrl)	
	//}}AFX_VIRTUAL

// Implementation
public:	
	void SetGridStyle(UINT style);
	COLORREF GetGridColor();
	void SetGridColor(COLORREF color);
	BOOL HasGridLines();
	void SetGridLines(BOOL hasLines);
	void SetBackgroundColor(COLORREF color);
	int WhichCheck(HTREEITEM hItem);
	void SetChecked(HTREEITEM hItem,int Checked);
	void UpdateDisplay();
	BOOL LocatePickLists(CString CustNum,BOOL HighlightLast,int ReturnToPickID);
	CString GetNextLine(CString &message);
	CString Return_SelectedCustNum();
	int Return_SelectedPick();	
	BOOL IsInPickingList(CString csItem,CString OrdNum);
	virtual ~EPERCheckBottom_cl_TreeListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(EPERCheckBottom_cl_TreeListCtrl)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();			
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);		
	afx_msg void OnTreelist2refresh();		
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnTvCheckbox(WPARAM wp, LPARAM lp);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:		
	BOOL m_bPressedTick;
	BOOL m_bHasGridLines;
	COLORREF m_GridColor;
	UINT m_GridStyle;
	HTREEITEM m_selectedItem;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERCHECKBOTTOMTREELISTCTRL_H__B2E827F7_4D4B_11D1_980A_004095E0DEFA__INCLUDED_)
