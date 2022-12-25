/*$T EDIsplitter.h GC!1.41 09/07/99 14:30:04 */

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

#define SPLIT_WIDTH 8
class EDI_cl_Splitter : public CSplitterWnd
{
    DECLARE_DYNCREATE(EDI_cl_Splitter)

/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDI_cl_Splitter (void);

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    int mi_SpecialSplit;

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    ToggleOrientation(BOOL _b_Refresh = TRUE);
    void    StartTracking1(int);
    void    StopTracking1(BOOL);
    int     HitTest1(CPoint);
    void    SetColumnInfo(int, int, int);
    void    SetRowInfo(int, int, int);
    void    OnDrawSplitter(CDC *, ESplitType, const CRect &);
    void    RecalcLayout(void);
    void    TrackRowSize(int, int);
    void    TrackColumnSize(int, int);

    void GetInsideRect(CRect &) const;

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnLButtonDown(UINT, CPoint);
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    afx_msg BOOL    OnMouseWheel(UINT, short, CPoint);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
