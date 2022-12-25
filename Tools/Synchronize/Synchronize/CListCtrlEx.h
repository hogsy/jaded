/*$T CListCtrlEx.h GC! 1.081 12/01/03 19:58:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __CListCtrlEx_h__
#define __CListCtrlEx_h__

class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNCREATE(CListCtrlEx)

/*
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCTOR/DESTRUCTOR
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	CListCtrlEx(void);
	virtual~CListCtrlEx();

/*
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTE
 -----------------------------------------------------------------------------------------------------------------------
 */
protected:
	BOOL			m_bFullRowSel;
	int				m_cxClient;
	int				m_cxStateImageOffset;
	COLORREF		m_clrText;
	COLORREF		m_clrTextBk;
	COLORREF		m_clrBkgnd;
	
    COLORREF		m_SelclrText;
	COLORREF		m_SelclrBkgnd;

/*
 -----------------------------------------------------------------------------------------------------------------------
    MEMBER
 -----------------------------------------------------------------------------------------------------------------------
 */
protected:
	static LPCTSTR	MakeShortString(CDC *, LPCTSTR lpszLong, int nColumnLen, int nOffset);
	void			RepaintSelectedItems(void);
	BOOL			SetFullRowSel(BOOL bFillRowSel);

/*
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE
 -----------------------------------------------------------------------------------------------------------------------
 */
protected:
	afx_msg LRESULT OnSetImageList(WPARAM wParam, LPARAM lParam);
	virtual BOOL	PreCreateWindow(CREATESTRUCT &);
	virtual void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

/*
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP
 -----------------------------------------------------------------------------------------------------------------------
 */
protected:
	afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint(void);
	afx_msg void	OnSetFocus(CWnd *);
	afx_msg void	OnKillFocus(CWnd *);
	DECLARE_MESSAGE_MAP()
};

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif 
 