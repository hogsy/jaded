/*$T EDIsplitter.cpp GC!1.52 11/12/99 18:21:58 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Overwrite of CSplitter MFC class. We want to resize all panes when clicking on the
            center of the splitter. £
            We want to resize to be clean (actually, it's not in MFC). £
            That splitter can have row and columns with a negative size (to hide a given pane or
            row when we maximised a pane). £
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIsplitter.h"
#include "EDImainframe.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDImainframe_act.h"
#include "EDIapp.h"
#include "ENGine/Sources/ENGmsg.h"

CWnd    *gpo_LastFocus = NULL;
#define SPEC_SIZE   20

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EDI_cl_Splitter, CSplitterWnd)
BEGIN_MESSAGE_MAP(EDI_cl_Splitter, CSplitterWnd)
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

/*
 ===================================================================================================
 ===================================================================================================
 */
EDI_cl_Splitter::EDI_cl_Splitter(void)
{
    mi_SpecialSplit = 0;
    m_cxSplitter = SPLIT_WIDTH;
    m_cySplitter = SPLIT_WIDTH;
    m_cxSplitterGap = SPLIT_WIDTH;
    m_cySplitterGap = SPLIT_WIDTH;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::ToggleOrientation(BOOL _b_Refresh)
{
	int	i;

    if(GetRowCount() == 2)
    {
        SetWindowLong(GetPane(1, 0)->m_hWnd, GWL_ID, AFX_IDW_PANE_FIRST + 1);
        m_nMaxRows = 1;
        m_nMaxCols = 2;
        m_nRows = 1;
        m_nCols = 2;

		i = m_pColInfo[0].nIdealSize;
		delete m_pColInfo;
		m_pColInfo = new CRowColInfo[m_nMaxCols];

		m_pColInfo[0].nIdealSize = i;
        if(i == 0)
            m_pColInfo[0].nIdealSize = m_pRowInfo[0].nIdealSize;
        m_pColInfo[0].nCurSize = m_pRowInfo[0].nCurSize;
        m_pColInfo[0].nMinSize = m_pRowInfo[0].nMinSize;
        m_pColInfo[1].nIdealSize = m_pRowInfo[0].nIdealSize;
        m_pColInfo[1].nCurSize = m_pRowInfo[0].nCurSize;
        m_pColInfo[1].nMinSize = m_pRowInfo[0].nMinSize;
    }
    else
    {
        SetWindowLong(GetPane(0, 1)->m_hWnd, GWL_ID, AFX_IDW_PANE_FIRST + 16);
        m_nMaxRows = 2;
        m_nMaxCols = 1;
        m_nRows = 2;
        m_nCols = 1;

		i = m_pRowInfo[0].nIdealSize;
		delete m_pRowInfo;
		m_pRowInfo = new CRowColInfo[m_nMaxRows];

		m_pRowInfo[0].nIdealSize = i;
        if(i == 0)
            m_pRowInfo[0].nIdealSize = m_pColInfo[0].nIdealSize;
        m_pRowInfo[0].nCurSize = m_pColInfo[0].nCurSize;
        m_pRowInfo[0].nMinSize = m_pColInfo[0].nMinSize;
        m_pRowInfo[1].nIdealSize = m_pColInfo[0].nIdealSize;
        m_pRowInfo[1].nCurSize = m_pColInfo[0].nCurSize;
        m_pRowInfo[1].nMinSize = m_pColInfo[0].nMinSize;
    }

	if(_b_Refresh)
		RecalcLayout();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
int EDI_cl_Splitter::HitTest1(CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int     i_Res, i_Res1, i_Res2;
    CPoint  pt1;
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ClientToScreen(&pt);

    /* Get hit test of left splitter */
    pt1 = pt;
    M_MF()->mo_Splitter1.ScreenToClient(&pt1);
    i_Res1 = M_MF()->mo_Splitter1.HitTest(pt1);

    /* To resize left splitter and main one */
    if(&(M_MF()->mo_Splitter1) == this)
    {
        pt1 = pt;
        M_MF()->mo_Splitter1.ScreenToClient(&pt1);
        GetClientRect(&o_Rect);
        if
        (
            (M_MF()->mo_Splitter1.GetRowCount() == 2) &&
            (i_Res1 == 101) &&
            (pt1.x > o_Rect.right - SPEC_SIZE)
        ) return 1000;
        if
        (
            (M_MF()->mo_Splitter1.GetRowCount() == 1) &&
            (i_Res1 == 201) &&
            (pt1.y > o_Rect.bottom - SPEC_SIZE)
        ) return 1000;
    }

    /* Get hit test of right splitter */
    pt1 = pt;
    M_MF()->mo_Splitter2.ScreenToClient(&pt1);
    i_Res2 = M_MF()->mo_Splitter2.HitTest(pt1);

    /* To resize right splitter and main one */
    if(&(M_MF()->mo_Splitter2) == this)
    {
        pt1 = pt;
        M_MF()->mo_Splitter2.ScreenToClient(&pt1);
        GetClientRect(&o_Rect);
        if((M_MF()->mo_Splitter2.GetRowCount() == 2) && (i_Res2 == 101) && (pt1.x < SPEC_SIZE))
            return 1000;
        if((M_MF()->mo_Splitter2.GetRowCount() == 1) && (i_Res2 == 201) && (pt1.y < SPEC_SIZE))
            return 1000;
    }

    /* Get hit test of main splitter */
    pt1 = pt;
    M_MF()->mo_Splitter.ScreenToClient(&pt1);
    i_Res = M_MF()->mo_Splitter.HitTest(pt1);

    /* Get hit test of left splitter */
    pt1 = pt;
    M_MF()->mo_Splitter1.ScreenToClient(&pt1);
    if(M_MF()->mo_Splitter1.GetRowCount() == 2)
        pt1.x = 10;
    else
        pt1.y = 10;
    i_Res1 = M_MF()->mo_Splitter1.HitTest(pt1);

    /* Get hit test of right splitter */
    pt1 = pt;
    M_MF()->mo_Splitter2.ScreenToClient(&pt1);
    if(M_MF()->mo_Splitter2.GetRowCount() == 2)
        pt1.x = 10;
    else
        pt1.y = 10;
    i_Res2 = M_MF()->mo_Splitter2.HitTest(pt1);

    /* To resize the four panes */
    ScreenToClient(&pt);
    if(&(M_MF()->mo_Splitter) != this) return HitTest(pt);
    if((i_Res1 == 0) && (i_Res2 == 0)) return i_Res;

    /* SplitterIntersection1 */
    return 301;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::StartTracking1(int ht)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* No special split yet */
    mi_SpecialSplit = 0;

    if(&(M_MF()->mo_Splitter) == this)
    {
        StartTracking(ht);
        return;
    }

    /* Resize left/right and main */
    if(ht == 1000)
    {
        if(GetRowCount() == 2)
        {
            M_MF()->mo_Splitter.StartTracking(201);
            StartTracking(101);
        }
        else
        {
            M_MF()->mo_Splitter.StartTracking(101);
            StartTracking(201);
        }

        mi_SpecialSplit = 1000;
        return;
    }

    /* Normal process */
    StartTracking(ht);
    return;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::StopTracking1(BOOL bAccept)
{
    if(!m_bTracking) return;

    if(m_htTrack != 301)
    {
        CSplitterWnd::StopTracking(bAccept);
        return;
    }

    ReleaseCapture();
    OnInvertTracker(m_rectTracker);
    if(m_bTracking2)
        OnInvertTracker(m_rectTracker2);

    m_bTracking = m_bTracking2 = FALSE;

    /* Vertical orientation */
    if(M_MF()->mo_Splitter1.GetRowCount() == 2)
    {
        M_MF()->mo_Splitter.SetColumnInfo(0, m_rectTracker2.left, 0);
        M_MF()->mo_Splitter.SetColumnInfo(1, 0, 0);
        M_MF()->mo_Splitter1.SetRowInfo(0, m_rectTracker.top, 0);
        M_MF()->mo_Splitter1.SetRowInfo(1, 0, 0);
        M_MF()->mo_Splitter2.SetRowInfo(0, m_rectTracker.top, 0);
        M_MF()->mo_Splitter2.SetRowInfo(1, 0, 0);
    }

    /* Horizontal orientation */
    else
    {
        M_MF()->mo_Splitter.SetRowInfo(0, m_rectTracker.top, 0);
        M_MF()->mo_Splitter.SetRowInfo(1, 0, 0);
        M_MF()->mo_Splitter1.SetColumnInfo(0, m_rectTracker2.left, 0);
        M_MF()->mo_Splitter1.SetColumnInfo(1, 0, 0);
        M_MF()->mo_Splitter2.SetColumnInfo(0, m_rectTracker2.left, 0);
        M_MF()->mo_Splitter2.SetColumnInfo(1, 0, 0);
    }

    M_MF()->mo_Splitter.RecalcLayout();
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::OnLButtonDown(UINT, CPoint pt)
{
    gpo_LastFocus = GetFocus();
    if(m_bTracking) return;

    StartTracking1(HitTest1(pt));
    OnMouseMove(0, pt);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::OnRButtonDown(UINT, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EMEN_cl_SubMenu o_Menu(FALSE);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(m_bTracking) return;
    ClientToScreen(&pt);
    M_MF()->InitPopupMenuAction(NULL, &o_Menu);
    M_MF()->AddPopupMenuAction(NULL, &o_Menu, EDI_ACTION_DESKEQUALIZE);
    M_MF()->AddPopupMenuAction(NULL, &o_Menu, EDI_ACTION_DESKFILL);
    M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::OnMouseMove(UINT, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CPoint  pt1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(m_bTracking && (mi_SpecialSplit == 1000))
    {
        pt1 = pt;
        ClientToScreen(&pt1);
        M_MF()->mo_Splitter.ScreenToClient(&pt1);
        M_MF()->mo_Splitter.SetCapture();
        M_MF()->mo_Splitter.SendMessage(WM_MOUSEMOVE, 0, pt1.x + (pt1.y << 16));

        SetCapture();
        CSplitterWnd::OnMouseMove(0, pt);
        return;
    }

    CSplitterWnd::OnMouseMove(0, pt);

    if(!m_bTracking)
    {
        SetSplitCursor(HitTest1(pt));
    }
    else if(gpo_LastFocus)
    {
        if(IsWindow(gpo_LastFocus->m_hWnd))
            gpo_LastFocus->SetFocus();
        else
            gpo_LastFocus = NULL;
    }
}

/*
 ===================================================================================================
    Aim:    To restore the focus to the previous window when finishing with a splitter.
 ===================================================================================================
 */
void EDI_cl_Splitter::OnLButtonUp(UINT, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CPoint  pt1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(mi_SpecialSplit == 1000)
    {
        pt1.x = M_MF()->mo_Splitter.m_rectTracker.left;
        pt1.y = M_MF()->mo_Splitter.m_rectTracker.top;
        M_MF()->mo_Splitter.SendMessage(WM_LBUTTONUP, 0, pt1.x + (pt1.y << 16));

        CSplitterWnd::OnLButtonUp(0, pt);
        mi_SpecialSplit = 0;
    }
    else
    {
        StopTracking1(TRUE);
    }

    /* Restore focus to previous window */
    if(gpo_LastFocus && IsWindow(gpo_LastFocus->m_hWnd))
        gpo_LastFocus->SetFocus();
    gpo_LastFocus = NULL;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::SetColumnInfo(int col, int cxIdeal, int cxMin)
{
    /* If negative value, force value to be splitter width (to hide it) */
    if(cxIdeal < 0)
        cxIdeal = cxMin = -SPLIT_WIDTH;

    m_pColInfo[col].nIdealSize = cxIdeal;
    m_pColInfo[col].nMinSize = cxMin;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::SetRowInfo(int row, int cxIdeal, int cxMin)
{
    /* If negative value, force value to be splitter width (to hide it) */
    if(cxIdeal < 0)
        cxIdeal = cxMin = -SPLIT_WIDTH;

    m_pRowInfo[row].nIdealSize = cxIdeal;
    m_pRowInfo[row].nMinSize = cxMin;
}

/*
 ===================================================================================================
    Aim:    We want to draw a special rect where we can drag the splitter for a special recize.
 ===================================================================================================
 */
void EDI_cl_Splitter::OnDrawSplitter(CDC *pDC, ESplitType nType, const CRect &rect)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect       o_Rect, o_Rect1;
    int         i;
    COLORREF    xCol, xCol1, xCol2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_Rect1 = rect;
    if(!pDC)
    {
        CSplitterWnd::OnDrawSplitter(pDC, nType, rect);
        return;
    }

    /* Normal color */
    xCol2 = GetSysColor(COLOR_ACTIVECAPTION);
    if(EDI_go_TheApp.mb_RunEngine)
        xCol2 = M_MF()->mst_Ini.mst_Options.x_ColRunEngine;

    /* Compute light and dark color */
    xCol = xCol1 = xCol2;
    xCol = M_MF()->u4_Interpol2PackedColor(0x00000000, xCol, 0.5f);
    xCol1 = M_MF()->u4_Interpol2PackedColor(0x00FFFFFF, xCol1, 0.8f);

    if(nType == splitBorder)
    {
        pDC->Draw3dRect(o_Rect1, xCol, xCol1);
        o_Rect1.InflateRect(-1, -1);
        pDC->Draw3dRect(o_Rect1, xCol, M_MF()->u4_Interpol2PackedColor(0x00FFFFFF, xCol1, 0.6f));
    }

    o_Rect1 = rect;
    if(nType == splitBar)
    {
        pDC->FillSolidRect(o_Rect1, xCol2);
    }

    if(nType == splitBar)
    {
        if(&(M_MF()->mo_Splitter1) == this)
        {
            /* Vertical horientation */
            if(GetRowCount() == 2)
            {
                o_Rect = o_Rect1;
                o_Rect.top += 2;
                for(i = 0; i < 2; i++)
                {
                    o_Rect.top += i * 2;
                    o_Rect.left = o_Rect1.right - SPEC_SIZE;
                    o_Rect.bottom = o_Rect.top + 2;
                    pDC->Draw3dRect(o_Rect, xCol, xCol1);
                }
            }

            /* Horizontal orientation */
            else
            {
                o_Rect = o_Rect1;
                o_Rect.left += 2;
                for(i = 0; i < 2; i++)
                {
                    o_Rect.left += i * 2;
                    o_Rect.top = o_Rect1.bottom - SPEC_SIZE;
                    o_Rect.right = o_Rect.left + 2;
                    pDC->Draw3dRect(o_Rect, xCol, xCol1);
                }
            }
        }

        if(&(M_MF()->mo_Splitter2) == this)
        {
            /* Vertical horientation */
            if(GetRowCount() == 2)
            {
                o_Rect = o_Rect1;
                o_Rect.top += 2;
                for(i = 0; i < 2; i++)
                {
                    o_Rect.top += i * 2;
                    o_Rect.right = o_Rect1.left + SPEC_SIZE;
                    o_Rect.bottom = o_Rect.top + 2;
                    pDC->Draw3dRect(o_Rect, xCol, xCol1);
                }
            }

            /* Horizontal horientation */
            else
            {
                o_Rect = o_Rect1;
                o_Rect.left += 2;
                for(i = 0; i < 2; i++)
                {
                    o_Rect.left += i * 2;
                    o_Rect.bottom = o_Rect1.top + SPEC_SIZE;
                    o_Rect.right = o_Rect.left + 2;
                    pDC->Draw3dRect(o_Rect, xCol, xCol1);
                }
            }
        }
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::RecalcLayout(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* To maximised splitter */
    if(this != &M_MF()->mo_BigSplitter)
    {
        if((m_nMaxCols > 1) && (m_pColInfo[1].nIdealSize < 0))
        {
            M_MF()->mo_BigSplitter.GetPane(0, 1)->GetClientRect(&o_Rect);
            m_pColInfo[0].nIdealSize = m_pColInfo[0].nMinSize = o_Rect.right;
        }

        if((m_nMaxRows > 1) && (m_pRowInfo[1].nIdealSize < 0))
        {
            M_MF()->mo_BigSplitter.GetPane(0, 1)->GetClientRect(&o_Rect);
            m_pRowInfo[0].nIdealSize = m_pRowInfo[0].nMinSize = o_Rect.bottom;
        }
    }

    /* To have a clean repaint */
    if(IsWindowVisible())
    {
        M_MF()->LockDisplay(this);
        CSplitterWnd::RecalcLayout();
        M_MF()->UnlockDisplay(this);
    }
    else
    {
        CSplitterWnd::RecalcLayout();
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::GetInsideRect(CRect &rect) const
{
    CSplitterWnd::GetInsideRect(rect);

    if(m_nMaxCols > 1)
    {
        if(m_pColInfo[m_nMaxCols - 1].nIdealSize < 0)
            rect.right += SPLIT_WIDTH;
    }

    if(m_nMaxRows > 1)
    {
        if(m_pRowInfo[m_nMaxRows - 1].nIdealSize < 0)
            rect.bottom += SPLIT_WIDTH;
    }
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EDI_cl_Splitter::OnMouseWheel(UINT a, short b, CPoint)
{
    return FALSE;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::TrackRowSize(int y, int row)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CPoint  pt(0, y);
    BOOL    bMemo;
    CRect   o_Rect;
    CWnd    *pview;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ClientToScreen(&pt);
    pview = GetPane(row, 0);
    bMemo = FALSE;
    if(pview->IsKindOf(RUNTIME_CLASS(EDI_cl_BaseView)))
        bMemo = ((EDI_cl_BaseView *) pview)->mb_TabFill;
    pview->ScreenToClient(&pt);
    if(bMemo)
    {
        ((EDI_cl_BaseView *) pview)->GetItemRect(0, &o_Rect);
        pt.y -= o_Rect.bottom + 4;
    }

    m_pRowInfo[row].nIdealSize = pt.y;
    if(pt.y < m_pRowInfo[row].nMinSize)
        m_pRowInfo[row].nIdealSize = 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EDI_cl_Splitter::TrackColumnSize(int x, int col)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CPoint  pt(x, 0);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ClientToScreen(&pt);
    GetPane(0, col)->ScreenToClient(&pt);
    m_pColInfo[col].nIdealSize = pt.x;
    if(pt.x < m_pColInfo[col].nMinSize)
        m_pColInfo[col].nIdealSize = 0;
}

#endif /* ACTIVE_EDITORS */
