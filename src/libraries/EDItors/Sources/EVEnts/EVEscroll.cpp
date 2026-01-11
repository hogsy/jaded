/*$T EVEscroll.cpp GC! 1.081 05/02/00 10:36:50 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EVEframe.h"
#include "EVEtrack.h"
#include "EVEevent.h"
#include "EVEscroll.h"
#include "EVEinside.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDImainframe.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJmain.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EEVE_cl_Scroll, CScrollView)
BEGIN_MESSAGE_MAP(EEVE_cl_Scroll, CScrollView)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Scroll::EEVE_cl_Scroll(void)
{
	mpo_Parent = NULL;
	mb_PaneMode = FALSE;
	mpo_Inside = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Scroll::~EEVE_cl_Scroll(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Scroll::PlaceTracks(BOOL _b_S)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	int				x, y;
	float			f;
	int				i_ScrollH, i_ScrollV;
	CRect			rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_ScrollH = GetScrollPos(SB_HORZ);
	i_ScrollV = GetScrollPos(SB_VERT);
	x = y = 0;
	if(mpo_Parent)
	{
		pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
		y = 0;
		while(pos)
		{
			po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
			if((!(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden)) || (mpo_Parent->mst_Ini.i_DisplayHidden))
			{
				po_Track->ShowWindow(SW_SHOW);
				po_Track->mo_TrackTime.SetRectEmpty();
				po_Track->MoveWindow(-i_ScrollH, y - i_ScrollV, 10000000, mpo_Parent->mst_Ini.i_YBar);
				if(po_Track->mo_ListEvents.GetSize())
				{
					if(mpo_Parent->mst_Ini.i_AllEventsVis)
					{
						f = mpo_Parent->mst_Ini.f_WidthFixed;
						if(po_Track->mo_ListEvents.GetSize() * (int) (f + 0.5) > x)
							x = po_Track->mo_ListEvents.GetSize() * (int) (f + 0.5);
					}
					else
					{
						f = 0;
						for(int i = 0; i < po_Track->mo_ListEvents.GetSize(); i++)
							f += po_Track->mo_ListEvents.GetAt(i)->mf_Width;
						if((int) (f + 0.5f) > x) x = (int) (f + 0.5f);
					}
				}

				y += mpo_Parent->mst_Ini.i_YBar;
			}
			else
			{
				po_Track->ShowWindow(SW_HIDE);
			}
		}
	}

	if(_b_S)
	{
		GetClientRect(&rect);
		SetScrollSizes(MM_TEXT, CSize(x * 2, y), CSize(rect.Width(), rect.Height()), CSize(rect.Width(), 20));
		SetScrollPos(SB_VERT, i_ScrollV);
		SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (i_ScrollV << 16), 0);
		SetScrollPos(SB_HORZ, i_ScrollH);
		SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (i_ScrollH << 16), 0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Scroll::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	if(mpo_Parent) mpo_Parent->ForceRefreshReal();
	PlaceTracks();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Scroll::OnLButtonDown(UINT, CPoint pt)
{
	/* Pane ? */
	if(GetAsyncKeyState(VK_SPACE) < 0)
	{
		SetCapture();
		mb_PaneMode = TRUE;
		mo_PanePt = pt;
		return;
	}

	mpo_Parent->UnselectAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Scroll::OnMouseMove(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~*/
	CPoint	o_Scroll;
	CPoint	o_Scroll1;
	CRect	o_Rect;
	CRect	o_Rect1;
	CPoint	o_Cursor;
	CPoint	o_Cursor1;
	MSG		msg;
	/*~~~~~~~~~~~~~~*/

	/* Pane */
	if(mb_PaneMode)
	{
		o_Scroll = o_Scroll1 = GetScrollPosition();

		o_Scroll.x += -(pt.x - mo_PanePt.x);
		if(GetStyle() & WS_VSCROLL) o_Scroll.y += -(pt.y - mo_PanePt.y);

		if(o_Scroll.x < 0) o_Scroll.x = 0;
		if(o_Scroll.y < 0) o_Scroll.y = 0;

		ScrollToPosition(o_Scroll);
		mo_PanePt = pt;

		/* Clip mouse */
		GetWindowRect(&o_Rect1);
		GetCursorPos(&o_Cursor);
		GetCursorPos(&o_Cursor1);
		if(o_Cursor.x <= o_Rect1.left) o_Cursor.x = o_Rect1.right - 1;
		if(o_Cursor.x >= o_Rect1.right) o_Cursor.x = o_Rect1.left + 1;
		if(o_Cursor.y <= o_Rect1.top) o_Cursor.y = o_Rect1.bottom - 1;
		if(o_Cursor.y >= o_Rect1.bottom) o_Cursor.y = o_Rect1.top + 1;
		if(o_Cursor != o_Cursor1)
		{
			SetCursorPos(o_Cursor.x, o_Cursor.y);
			ScreenToClient(&o_Cursor);
			mo_PanePt = o_Cursor;
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message != WM_MOUSEMOVE) break;
			};
		}

		/* Parent */
		mpo_Inside->WhenScroll(o_Scroll1.x - o_Scroll.x, o_Scroll1.y - o_Scroll.y);

		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Scroll::OnLButtonUp(UINT, CPoint pt)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(mb_PaneMode)
	{
		mb_PaneMode = FALSE;
		ReleaseCapture();
		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Scroll::OnRButtonDown(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	int				i_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Parent->mpst_Data) return;
	GetCursorPos(&pt);
	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	M_MF()->AddPopupMenuAction(mpo_Parent, &o_Menu, 1, TRUE, "New Track", -1);

	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	if(i_Res <= 0) return;
	switch(i_Res)
	{
	/* New track */
	case 1:
		mpo_Parent->SaveForUndo();
		mpo_Parent->OnNewTrack();
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Scroll::OnDraw(CDC *pdc)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_cl_Scroll::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	BOOL	b_Res;
	CPoint	o_Scroll, o_Scroll1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_Scroll1 = GetScrollPosition();
	b_Res = CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
	GetClientRect(&o_Rect);
	o_Rect.bottom = o_Rect.top;
	InvalidateRect(&o_Rect, FALSE);
	o_Scroll = GetScrollPosition();

	/* Parent */
	if(mpo_Inside) mpo_Inside->WhenScroll(o_Scroll1.x - o_Scroll.x, o_Scroll1.y - o_Scroll.y);

	return b_Res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_cl_Scroll::OnEraseBkgnd(CDC *pdc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Track	*po_Track;
	CRect			o_Rect;
	CRect			o_Rect1;
	POSITION		pos;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	GetClientRect(&o_Rect);

	if(!mpo_Parent->mpst_ListTracks)
	{
		pdc->FillSolidRect(&o_Rect, GetSysColor(COLOR_3DSHADOW));
		return TRUE;
	}

	o_Rect.top += GetScrollPosition().y;
	pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && (!(mpo_Parent->mst_Ini.i_DisplayHidden)))
			continue;
		po_Track->GetWindowRect(&o_Rect1);
		ScreenToClient(&o_Rect1);
		o_Rect.top = o_Rect1.bottom;
	}

	if(o_Rect.bottom < o_Rect.top + GetScrollPosition().y) o_Rect.bottom = o_Rect.top + GetScrollPosition().y;
	pdc->FillSolidRect(&o_Rect, GetSysColor(COLOR_3DSHADOW));

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_cl_Scroll::OnMouseWheel(UINT, short z, CPoint pt)
{
	if(!mpo_Parent->mpst_ListTracks) return TRUE;
	if(GetAsyncKeyState(VK_SPACE) < 0)
	{
		mpo_Parent->OnAction(z < 0 ? EEVE_ACTION_BIGZOOMOUT : EEVE_ACTION_BIGZOOMIN);
		return TRUE;
	}

	return FALSE;
}

#endif /* ACTIVE_EDITORS */
