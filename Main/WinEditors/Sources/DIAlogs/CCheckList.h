/*$T CCheckList.h GC! 1.114 04/21/01 12:39:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __CCHECKLIST_H__
#define __CCHECKLIST_H__

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#include "afxtempl.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
class CCheckList : CFrameWnd
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	CCheckList(void);
	BOOL					Do(CRect *, int, CWnd *, int = 15, char * = NULL);
	int						i_GetHeight(void);
	static LRESULT CALLBACK x_Hook(int, WPARAM, LPARAM);
	void					AddString(CString s, int i = -1, BOOL = TRUE);
	void					SetArrayBool(BOOL *p)				{ mpb_Checks = p; }
	void					SetImageList(CImageList *p)			{ mpo_ImageList = p; }
	void					DrawCheckMark(CDC *, int, int, COLORREF);
	LRESULT					DefWindowProc(UINT, WPARAM, LPARAM);

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	CFont					mo_Font;
	CList<CString, CString> mo_ListItems;
	CList<int, int>			mo_ListIcons;
	CList<BOOL, BOOL>		mo_ListEnabled;
	CList<CRect, CRect>		mo_ListRects;
	BOOL					*mpb_Checks;
	int						mi_MaxLines;
	int						mi_PosY;
	CRect					mo_RectAll;
	CRect					mo_RectNone;
	CRect					mo_RectInvert;
	CRect					mo_RectOK;
	CRect					mo_RectCancel;
	CImageList				*mpo_ImageList;
	int						mi_Selected;
	BOOL					mb_Caption;
	int						mb_Save;

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	afx_msg int				OnCreate(LPCREATESTRUCT);
	afx_msg void			OnDestroy(void);
	afx_msg BOOL			OnEraseBkgnd(CDC *);
	afx_msg void			OnPaint(void);
	afx_msg void			OnLButtonDown(UINT, CPoint);
	afx_msg void			OnVScroll(UINT, UINT, CScrollBar *);
	DECLARE_MESSAGE_MAP()
};
#endif /* !__CCHECKLIST_H__ */
