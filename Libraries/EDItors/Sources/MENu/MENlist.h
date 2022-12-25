/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EMEN_cl_FrameIn;

class EMEN_cl_FrameList : public CListCtrl
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EMEN_cl_FrameList(void);
    ~EMEN_cl_FrameList(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_ActionList   *mpo_Actions;
    BOOL                mb_Up;
    EMEN_cl_FrameIn     *mpo_Parent;
    BOOL                mb_BeginDragDrop;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    DrawAction(DRAWITEMSTRUCT *, CRect, BOOL _b_TextFromList = TRUE, BOOL _b_SubMenu = FALSE);
    void    DrawItem(DRAWITEMSTRUCT *);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
	afx_msg BOOL	OnEraseBkgnd(CDC *);
    afx_msg void    OnMouseMove(UINT, CPoint);
    afx_msg void    OnBeginDrag(NMHDR *, LRESULT *);
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
