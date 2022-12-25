/*$T BROlistctrl.h GC!1.21 04/02/99 10:43:00 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#include "Dialogs/ListCtrlStyled.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
class EBRO_cl_ListCtrl : public CListCtrlStyled
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
	EBRO_cl_ListCtrl(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_BaseView *mpo_MyView;
    int             mi_SortColumn;
	BOOL			mb_LockSel;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    ComputeColName(BIG_INDEX, int, char *);
    void    FillFile(EDI_cl_BaseView *, ULONG, BOOL);
    void    FillFileEngineDatas(EDI_cl_BaseView *, ULONG);
    CString GetItemText(int, int);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

protected:
    afx_msg void    OnSetFocus(CWnd *);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonDblClk(UINT, CPoint);
    afx_msg void    OnColumnClick(NMHDR *, LRESULT *);
    afx_msg void    OnItemChanged(NMHDR *, LRESULT *);
    afx_msg void    OnBeginDrag(NMHDR *, LRESULT *);
    afx_msg void    OnParentNotify(UINT, LONG);
	afx_msg void	OnKeyDown(UINT, UINT, UINT);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
