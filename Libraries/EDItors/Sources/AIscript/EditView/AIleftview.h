/*$T AIleftview.h GC!1.39 07/02/99 14:48:15 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "BIGfiles/BIGdefs.h"

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class   EAI_cl_Frame;
class EAI_cl_LeftView : public CFrameWnd
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EAI_cl_LeftView (void);
    ~EAI_cl_LeftView(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EAI_cl_Frame                *mpo_Frame;
    int                         mi_LineError;
	int							mi_LineBreak;
    CList<int, int>             mo_ListBreak;
    CList<BIG_INDEX, BIG_INDEX> mo_ListBreakFile;
    CList<BIG_INDEX, BIG_INDEX> mo_ListBreakModel;
    CList<BIG_INDEX, BIG_INDEX> mo_ListBreakInstance;
	CList<void *, void *>		mo_ListBreakPtInst;

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    Init(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg BOOL    OnEraseBkgnd(CDC *);
    afx_msg void    OnPaint(void);
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
