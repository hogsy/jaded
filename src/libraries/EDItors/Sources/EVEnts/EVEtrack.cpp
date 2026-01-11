/*$T EVEtrack.cpp GC 1.139 03/17/04 12:21:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "EVEtrack.h"
#include "EVEframe.h"
#include "EVEinside.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "EDImainframe.h"
#include "EVEscroll.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "EDIapp.h"
#include "OUTput/OUTframe.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"

extern void		EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);
#ifdef JADEFUSION
extern BOOL EVE_gb_CanFlash;
extern BOOL EVE_gb_EndNeedToRecomputeSND;
extern 		EVE_gb_UseRunningPauseFlag;
#else
extern "C" BOOL EVE_gb_CanFlash;
extern "C" BOOL EVE_gb_EndNeedToRecomputeSND;
extern "C" BOOL EVE_gb_UseRunningPauseFlag;
#endif
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

BEGIN_MESSAGE_MAP(EEVE_cl_Track, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
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
EEVE_cl_Track::EEVE_cl_Track(void)
{
	mpst_Track = NULL;
	mb_Visible = TRUE;
	mb_MoveSize = FALSE;
	mpo_HitTest = NULL;
	mo_TrackTime.SetRectEmpty();
	mi_HitTest = 0;
	mi_NumSelected = 0;
	mb_SelMode = TRUE;
	mo_ZoomRect.SetRectEmpty();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Track::~EEVE_cl_Track(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::Create(CWnd *_po_Parent)
{
	CWnd::Create(NULL, NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), _po_Parent, 0, NULL);
	SetFont(&M_MF()->mo_Fnt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EEVE_cl_Track::HitTest(CPoint pt, EEVE_cl_Event **_po_Ret, BOOL cannull)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Event	*po_Event;
	EEVE_cl_Event	*po_Event1;
	CRect			o_Rect;
	int				index;
	CPoint			o_Pt;
	CPoint			o_Pt1;
	CPoint			pt1;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Where in the track */
	GetClientRect(&o_Rect);
	o_Pt1 = pt;
	pt.y = o_Rect.top + 1;
	o_Pt = pt;

	for(index = 0; index < mo_ListEvents.GetSize(); index++)
	{
		po_Event = mo_ListEvents.GetAt(index);
		if(mo_TrackTime.PtInRect(pt))
		{
			GetCursorPos(&pt1);
			ScreenToClient(&pt1);
			pt1.y -= ((EEVE_cl_Scroll *) GetParent())->GetScrollPosition().y;
			if((pt1.y < 0) || (pt1.y > mo_TrackTime.bottom) || (GetAsyncKeyState(VK_SHIFT) < 0))
			{
				mi_DecSize = pt.x - mo_TrackTime.left;
				if(_po_Ret) *_po_Ret = mo_ListEvents[mpst_Param->uw_CurrentEvent];
				return 3;
			}
		}

		o_Rect = po_Event->mo_Rect;
		if(o_Rect.IsRectEmpty())
		{
			if(index == 0 || cannull) goto com;
			continue;
		}

		if(o_Rect.PtInRect(pt))
		{
			/* In running box ? */
			if(mo_TrackTime.PtInRect(pt))
			{
				GetCursorPos(&pt1);
				ScreenToClient(&pt1);
				if((pt1.y < 0) || (GetAsyncKeyState(VK_SHIFT) < 0))
				{
					mi_DecSize = pt.x - mo_TrackTime.left;
					if(_po_Ret) *_po_Ret = po_Event;
					return 3;
				}
			}

			if(_po_Ret) *_po_Ret = po_Event;
			return 1;
		}

		if(!mpo_Frame->mst_Ini.i_AllEventsVis)
		{
com:
			o_Rect = po_Event->mo_Rect;
			o_Rect.left = o_Rect.right;
			if(!cannull || po_Event->mb_Empty) o_Rect.right += EEVE_C_CXBorder;
			if(cannull) o_Rect.left -= EEVE_C_CXBorder;
			pt.y = o_Rect.top + 1;
			if(o_Rect.PtInRect(pt))
			{
				if(cannull)
				{
					if(_po_Ret) *_po_Ret = po_Event;
					return 2;
				}
				else if(po_Event->mb_Expand)
				{
					if(_po_Ret) *_po_Ret = NULL;
					return 0;
				}

				while(po_Event->mb_Empty && index)
				{
					index--;
					po_Event = mo_ListEvents.GetAt(index);
				}

				/* Concat tracks */
				if(GetAsyncKeyState(VK_CONTROL) < 0)
				{
					if((index < mo_ListEvents.GetSize() - 1))
					{
						po_Event1 = mo_ListEvents.GetAt(index + 1);
						while(po_Event1 && po_Event1->mf_Width <= EEVE_C_CXBorder2)
						{
							index++;
							po_Event = po_Event1;
							if(index == mo_ListEvents.GetSize() - 1) break;
							po_Event1 = mo_ListEvents.GetAt(index + 1);
						}
					}
				}

				mi_DecSize = pt.x - o_Rect.left;
				if(_po_Ret) *_po_Ret = po_Event;
				return 2;
			}
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_cl_Track::SnapOtherTracks(CPoint *pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	POINT			pt1;
	POINT			pt2;
	CRect			o_Rect;
	int				x, xx, xx1;
	int				index;
	float			f_Width;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	x = ((EEVE_cl_Inside *) mpo_Frame->mpo_MainSplitter->GetPane(0, 1))->GetScrollPosition().x;
	pt1 = *pt;
	pt1.x += x;

	if(mpo_Frame->mst_Ini.i_SnapMode == EEVE_C_SnapModeUser)
	{
		f_Width = mpo_Frame->mst_Ini.f_SnapUnit * mpo_Frame->mst_Ini.f_Factor;
		f_Width /= mpo_Frame->mst_Ini.f_ValFactor;
		xx = (int) (f_Width + 0.5f);
		xx1 = (pt1.x + mi_DecSize);
		if(!xx) xx = 1;
		xx1 /= xx;
		xx1 *= xx;
		if(abs((pt1.x + mi_DecSize) - xx1) < EEVE_C_CXBorder2)
		{
			pt->x = xx1 + mi_DecSize;
			return TRUE;
		}

		return FALSE;
	}

	if(mpo_Frame->mst_Ini.i_SnapMode == EEVE_C_SnapMode160)
	{
		f_Width = (1.0f / 60.0f) * mpo_Frame->mst_Ini.f_Factor;
		f_Width /= mpo_Frame->mst_Ini.f_ValFactor;
		xx = (int) (f_Width + 0.5f);
		xx1 = (pt1.x + mi_DecSize);
		if(!xx) xx = 1;
		xx1 /= xx;
		xx1 *= xx;
		if(abs((pt1.x + mi_DecSize) - xx1) < EEVE_C_CXBorder2)
		{
			pt->x = xx1 + mi_DecSize;
			return TRUE;
		}

		return FALSE;
	}

	if(mpo_Frame->mst_Ini.i_SnapMode & EEVE_C_SnapModeAuto)
	{
		/* Snap to regle */
		pt1.x /= (int) (mpo_Frame->mst_Ini.f_Factor + 0.5f);
		pt1.x *= (int) (mpo_Frame->mst_Ini.f_Factor + 0.5f);
		o_Rect.left = pt1.x;
		o_Rect.right = o_Rect.left + EEVE_C_CXBorder;
		o_Rect.top = 0;
		o_Rect.bottom = 1000000;
		pt2 = *pt;
		pt2.y = o_Rect.top + 1;
		if(o_Rect.PtInRect(pt2))
		{
			pt->x = o_Rect.left + mi_DecSize;
			return TRUE;
		}

		/* Snap inside regle */
		pt1.x += EEVE_C_CXBorder;
		pt2.x = pt->x - pt1.x;
		pt2.x /= (int) ((mpo_Frame->mst_Ini.f_Factor / mpo_Frame->mst_Ini.f_DivFactor) + 0.5f);
		pt2.x *= (int) ((mpo_Frame->mst_Ini.f_Factor / mpo_Frame->mst_Ini.f_DivFactor) + 0.5f);
		pt2.x += (int) ((mpo_Frame->mst_Ini.f_Factor / mpo_Frame->mst_Ini.f_DivFactor) + 0.5f);
		pt2.x += pt1.x;
		o_Rect.left = pt2.x - EEVE_C_CXBorder;
		o_Rect.right = o_Rect.left + EEVE_C_CXBorder;
		o_Rect.top = 0;
		o_Rect.bottom = 1000000;
		pt1 = *pt;
		pt1.y = o_Rect.top + 1;
		if(o_Rect.PtInRect(pt1))
		{
			pt->x = o_Rect.left + mi_DecSize;
			return TRUE;
		}
	}

	/* Snap to other bars */
	if(mpo_Frame->mst_Ini.i_SnapMode & EEVE_C_SnapModeTracks)
	{
		pos = mpo_Frame->mo_ListTracks.GetHeadPosition();
		while(pos)
		{
			po_Track = mpo_Frame->mo_ListTracks.GetNext(pos);
			if(po_Track == this) continue;
			if(po_Track->mpst_Track->pst_AllEvents == mpst_Track->pst_AllEvents) continue;

			/* Running box */
			pt1.x = pt->x;
			pt1.y = pt->y;
			pt1.y = po_Track->mo_TrackTime.top + 1;
			if(po_Track->mo_TrackTime.PtInRect(pt1))
			{
				pt->x = po_Track->mo_TrackTime.left + mi_DecSize;
				return TRUE;
			}

			/* All events */
			for(index = 0; index < po_Track->mo_ListEvents.GetSize(); index++)
			{
				po_Event = po_Track->mo_ListEvents.GetAt(index);
				pt1.x = pt->x;
				pt1.y = pt->y;
				o_Rect = po_Event->mo_Rect;
				o_Rect.left = o_Rect.right;
				o_Rect.right += EEVE_C_CXBorder;
				pt1.y = o_Rect.top + 1;
				if(o_Rect.PtInRect(pt1))
				{
					pt->x = o_Rect.left + mi_DecSize;
					return TRUE;
				}

				if(pt1.x < o_Rect.left) break;
			}
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::OnMouseMove(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Event		*po_New;
	EEVE_cl_Event		*po_Prev, *po_Next;
	EEVE_cl_Event		*po_Next1;
	char				asz_Msg1[128];
	char				asz_Msg2[128];
	char				asz_Msg3[128];
	char				asz_Msg4[256];
	CRect				o_Rect;
	CPoint				pt1;
	int					i_Move;
	float				f_Delay;
	BOOL				b_Snap;
	int					indexf;
	CDC					*pdc;
	POSITION			pos;
	EEVE_cl_Track		*po_Track;
	int					index, index1;
	float				f_Time;
	BOOL				b_One;
	int					i_Dec;
	/* int i_Dec1; */
	int					i_First;
	BOOL				b_Shift;
	float				f_Last;
	EVE_tdst_Event		*pst_Evt;
	EVE_tdst_Event		*pst_Evt1;
	CString				o_Name;
	short				w_Type;
	char				*pc_Data;
	char				az[128];
	MATH_tdst_Vector	*pt_Vec;
	float				__Delay, fdec1;
	unsigned short		uw_OldNumFrames;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Tooltip help */
	if(mpo_HitTest && (mi_HitTest == 1))
	{
		f_Time = 0;
		pst_Evt = mpo_HitTest->pst_FindEngineEvent();
		pst_Evt1 = mpst_Track->pst_AllEvents;
		while(pst_Evt != pst_Evt1)
		{
			f_Time += pst_Evt1->uw_NumFrames;
			pst_Evt1++;
		}

		f_Time = EVE_FrameToTime(f_Time);

		if(mpo_HitTest->mo_Name.IsEmpty())
		{
			sprintf
			(
				asz_Msg4,
				"Empty\nLabel %d\nDelay = %f (%d frames)\nTime = %f (%d frames)",
				(pst_Evt->w_Flags & 0xFF00) >> 8,
				EVE_FrameToTime(pst_Evt->uw_NumFrames),
				pst_Evt->uw_NumFrames,
				f_Time,
				EVE_TimeToFrame(f_Time)
			);
		}
		else
		{
			sprintf
			(
				asz_Msg4,
				"%s\nLabel %d\nDelay = %f (%d frames)\nTime = %f (%d frames)",
				(char *) (LPCSTR) mpo_HitTest->mo_Name,
				(pst_Evt->w_Flags & 0xFF00) >> 8,
				EVE_FrameToTime(pst_Evt->uw_NumFrames),
				pst_Evt->uw_NumFrames,
				f_Time,
				EVE_TimeToFrame(f_Time)
			);
		}

		if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
		{
			w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Evt);
			if(w_Type & EVE_InterKeyType_Translation_Mask)
			{
				pc_Data = (char *) pst_Evt->p_Data;
				pc_Data += 4;
				pt_Vec = (MATH_tdst_Vector *) pc_Data;
				sprintf(az, "\nx = %f y = %f z = %f", pt_Vec->x, pt_Vec->y, pt_Vec->z);
				L_strcat(asz_Msg4, az);
			}
		}

		EDI_Tooltip_DisplayMessage(asz_Msg4, 500);
	}

	/* Before and after... */
	po_Prev = NULL;
	po_Next = NULL;
	if(mpo_HitTest && mi_HitTest && mi_HitTest != 3)
	{
		for(indexf = 0; indexf < mo_ListEvents.GetSize(); indexf++)
			if(mo_ListEvents.GetAt(indexf) == mpo_HitTest) break;
		if(indexf) po_Prev = mo_ListEvents.GetAt(indexf - 1);
		if(indexf < mo_ListEvents.GetSize() - 1) po_Next = mo_ListEvents.GetAt(indexf + 1);
	}

	/* Resize mode */
	if(mb_MoveSize)
	{
		pt1 = pt;
		b_Snap = SnapOtherTracks(&pt);

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    On an event
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(mi_HitTest == 1)
		{

			/*$1- Move event ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(!mb_SelMode)
			{
				pos = mpo_Frame->mo_ListTracks.GetHeadPosition();
				while(pos)
				{
					po_Track = mpo_Frame->mo_ListTracks.GetNext(pos);
					for(index = 1; index < po_Track->mo_ListEvents.GetSize(); index++)
					{
						po_New = po_Track->mo_ListEvents.GetAt(index);
						if(po_New->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)
						{
							po_Prev = po_Track->mo_ListEvents.GetAt(index - 1);
							if((!(po_Prev->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)) || (index == 1))
							{
								po_Prev->mf_Width += (pt.x - mo_HitPt.x);
								po_Prev->RecomputeDelay();
							}

							if(index < po_Track->mo_ListEvents.GetSize() - 1)
							{
								po_Next = po_Track->mo_ListEvents.GetAt(index + 1);
								if(!(po_Next->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected))
								{
									po_Next->mf_Width -= (pt.x - mo_HitPt.x);
									po_Next->RecomputeDelay();
								}
							}

							po_Track->Invalidate();
						}
					}
				}
			}

			/*$1- Selection box ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			else
			{
				if((abs(pt.x - mo_PanePt.x) > 5) || (abs(pt.y - mo_PanePt.y) > 5))
				{
					pdc = GetParent()->GetWindowDC();
					o_Rect.left = mo_PanePt.x;
					o_Rect.top = mo_PanePt.y;
					o_Rect.right = pt.x;
					o_Rect.bottom = pt.y;
					o_Rect.NormalizeRect();
					ClientToScreen(&o_Rect);

					GetParent()->ScreenToClient(&o_Rect);
					if((o_Rect.right < mo_ZoomRect.right) || (o_Rect.bottom < mo_ZoomRect.bottom))
						pdc->DrawDragRect(&o_Rect, CSize(2, 2), &mo_ZoomRect, CSize(2, 2));
					GetParent()->ClientToScreen(&o_Rect);
					mpo_Frame->SelectInRect(&o_Rect);

					GetParent()->ScreenToClient(&o_Rect);
					if((o_Rect.right < mo_ZoomRect.right) || (o_Rect.bottom < mo_ZoomRect.bottom))
						pdc->DrawDragRect(&o_Rect, CSize(2, 2), &o_Rect, CSize(2, 2));
					else
						pdc->DrawDragRect(&o_Rect, CSize(2, 2), &mo_ZoomRect, CSize(2, 2));
					mo_ZoomRect = o_Rect;
					ReleaseDC(pdc);

					mo_HitPt.x = pt.x;
				}

				return;
			}

			mo_HitPt.x = pt.x;
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    On a border
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else if(mi_HitTest == 2)
		{
			if(b_Snap || !mpo_Frame->mst_Ini.i_ForceSnap)
			{
				/*
				 * Compute moving offset £
				 * i_Dec = (pt.x - mi_DecSize) - mpo_HitTest->mo_Rect.left - (int)
				 * (mpo_HitTest->mf_Width + 0.5f);
				 */
				i_Dec = (pt.x - mi_DecSize) - mpo_HitTest->mo_Rect.left - (mpo_HitTest->mo_Rect.Width() + EEVE_C_CXBorder);
				if(!i_Dec) return;
				i_Dec += EEVE_C_CXBorder;
				if(i_Dec < 0 && mpo_HitTest->mo_Rect.IsRectEmpty()) return;

				if(mpo_HitTest->mo_Rect.left == mpo_HitTest->mo_Rect.right)
					i_Dec += (int) (mpo_HitTest->mf_Width + 0.5f);

				if(!i_Dec) return;
				if((mpo_HitTest->mo_Rect.left == mpo_HitTest->mo_Rect.right) && (abs(i_Dec) <= EEVE_C_CXBorder))
				{
					return;
				}

				/* Retreive event to change just after. In case of an empty event, zap */
				po_Next = NULL;
				index = mpo_HitTest->mi_NumEvent;
				if(index < mo_ListEvents.GetSize() - 1)
				{
					po_Next = mo_ListEvents.GetAt(index + 1);
					if(po_Next->mb_Empty)
					{
						if(index + 1 == mo_ListEvents.GetSize() - 1)
							po_Next = NULL;
						else
						{
							index1 = 1;
							while(index + index1 <= mo_ListEvents.GetSize() - 1)
							{
								po_Next1 = mo_ListEvents.GetAt(index + index1);
								if(!po_Next1->mb_Empty)
								{
									po_Next = po_Next1;
									goto zap;
								}

								index1++;
							}

							po_Next = NULL;
						}
					}
				}

				/* Just move current track ? */
zap:
				b_One = TRUE;
				if(mpo_HitTest->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected) b_One = FALSE;
				if(po_Next && po_Next->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected) b_One = FALSE;

				b_Shift = FALSE;
				if(GetAsyncKeyState(VK_SHIFT) < 0) b_Shift = TRUE;

				if(b_One)
				{
					index = mpo_HitTest->mi_NumEvent;
					po_New = mpo_HitTest;
					po_Track = this;
					goto justone;
				}

				/* First, last or inside ? */
				i_First = 0;
				if(mpo_HitTest->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)
				{
					if(mpo_HitTest->mi_NumEvent == mo_ListEvents.GetSize() - 1)
						i_First = 2;
					else
					{
						if(po_Next && po_Next->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)
							i_First = 0;
						else
							i_First = 2;
					}
				}
				else
				{
					i_First = 1;
				}

				if(i_First != 1) b_Shift = FALSE;

				pos = mpo_Frame->mo_ListTracks.GetHeadPosition();
				while(pos)
				{
					po_Track = mpo_Frame->mo_ListTracks.GetNext(pos);
					f_Time = 0;
					for(index = 0; index < po_Track->mo_ListEvents.GetSize(); index++)
					{
						po_New = po_Track->mo_ListEvents.GetAt(index);
						f_Time += EVE_FrameToTime(po_New->pst_FindEngineEvent()->uw_NumFrames);

						/* Retreive event to change just after. In case of an empty event, zap */
						po_Next = NULL;
						if(index < po_Track->mo_ListEvents.GetSize() - 1)
						{
							po_Next = po_Track->mo_ListEvents.GetAt(index + 1);
							if(po_Next->mb_Empty && ((i_First != 2) || (GetAsyncKeyState(VK_CONTROL) >= 0)))
							{
								if(index + 1 == po_Track->mo_ListEvents.GetSize() - 1)
									po_Next = NULL;
								else
								{
									index1 = 1;
									while(index + index1 <= po_Track->mo_ListEvents.GetSize() - 1)
									{
										po_Next1 = po_Track->mo_ListEvents.GetAt(index + index1);
										if(!po_Next1->mb_Empty)
										{
											index += index1 - 1;
											po_Next = po_Next1;
											goto zap1;
										}

										index1++;
									}

									po_Next = NULL;
									index = po_Track->mo_ListEvents.GetSize() - 1;
								}
							}
						}

zap1:
						if(!(po_New->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected))
						{
							if(!po_Next) continue;
							if(po_Next && !(po_Next->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected))
								continue;
							if(i_First != 1) continue;
						}

						/* Moving first, and current event is selected : Not move */
						else if(i_First == 1)
						{
							if(!b_Shift || (po_New->mi_NumEvent != po_Track->mo_ListEvents.GetSize() - 1)) continue;
						}

						/* Moving inside */
						else
						{
							if(!po_Next)
							{
								if(i_First != 2) continue;
							}
							else
							{
								if(po_Next && !(po_Next->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected))
								{
									if(i_First != 2) continue;
								}
								else
								{
									if(i_First == 2) continue;
								}
							}
						}

justone:
						po_Track->Invalidate();

						__Delay = i_Dec / po_Track->mpo_Frame->mst_Ini.f_Factor;
						__Delay *= po_Track->mpo_Frame->mst_Ini.f_ValFactor;
						uw_OldNumFrames = po_New->pst_FindEngineEvent()->uw_NumFrames;
						if(__Delay < 0 && po_New->pst_FindEngineEvent()->uw_NumFrames < -__Delay)
							po_New->pst_FindEngineEvent()->uw_NumFrames = 0;
						else
							po_New->pst_FindEngineEvent()->uw_NumFrames += EVE_TimeToFrame(__Delay);
						po_New->RecomputeWidth();
						po_New->RecomputeTimeKey(uw_OldNumFrames);

						f_Last = po_New->mf_Width;
						if(!b_Shift && po_Next)
						{
							if(f_Last > 0.0f || po_New->mf_Width >= 0.0f)
							{
								fdec1 = __Delay;
								if((f_Last < 0) && (po_New->mf_Width > 0))
									fdec1 = po_Next->pst_FindEngineEvent()->uw_NumFrames / 60.0f;
								else if((f_Last > 0) && (po_New->mf_Width < 0))
									fdec1 = -po_New->pst_FindEngineEvent()->uw_NumFrames / 60.0f;
								else if((f_Last == 0) && (po_New->mf_Width >= 0.0f))
									fdec1 = po_New->pst_FindEngineEvent()->uw_NumFrames / 60.0f;
								else if((f_Last < 0) && (po_New->mf_Width == 0.0f))
									fdec1 = 0;
								uw_OldNumFrames = po_Next->pst_FindEngineEvent()->uw_NumFrames;
								if(fdec1 < 0 && po_Next->pst_FindEngineEvent()->uw_NumFrames < -fdec1)
									po_Next->pst_FindEngineEvent()->uw_NumFrames = 0;
								else
									po_Next->pst_FindEngineEvent()->uw_NumFrames -= EVE_TimeToFrame(fdec1);
								po_Next->RecomputeWidth();
								po_Next->RecomputeTimeKey(uw_OldNumFrames);
								if(po_Next->mf_CurWidth == 0) po_Next->mb_Empty = TRUE;
							}
						}

						if(b_One) goto justoneend;
						if(i_First == 0) continue;
						break;
					}
				}

justoneend:
				mo_HitPt.x = pt.x;
			}

			if(mpst_Param->f_Time > EVE_FrameToTime(mpst_Track->pst_AllEvents[mpst_Param->uw_CurrentEvent].uw_NumFrames))
			{
				mpst_Param->f_Time = EVE_FrameToTime(mpst_Track->pst_AllEvents[mpst_Param->uw_CurrentEvent].uw_NumFrames);
			}

			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    On running box
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else if(mi_HitTest == 3)
		{
			mpo_HitTest = mo_ListEvents.GetAt(mpst_Param->uw_CurrentEvent);
			i_Move = pt.x - mi_DecSize;
			i_Move -= mpo_HitTest->mo_Rect.left;
			i_Move += EEVE_C_CXBorder;
			f_Delay = (float) (i_Move);
			f_Delay /= mpo_Frame->mst_Ini.f_Factor;
			f_Delay *= mpo_Frame->mst_Ini.f_ValFactor;
			f_Delay = f_Delay - mpst_Param->f_Time;
			if(!f_Delay) return;

			EVE_gb_UseRunningPauseFlag = FALSE;
			mpo_Frame->MoveRunningBox(this, f_Delay);
			EVE_gb_UseRunningPauseFlag = TRUE;

			mo_HitPt.x = pt.x;
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    On key specific box
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else if(mi_HitTest >= 1000)
		{
			/*~~~~~~~~~~~~~~~~~~~~~*/
			EVE_tdst_Event	*pst_Evt;
			/*~~~~~~~~~~~~~~~~~~~~~*/

			pst_Evt = mpo_HitTest->pst_FindEngineEvent();

			if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
			{
				i_Move = pt.x - mpo_HitTest->mo_Rect.left;
				f_Delay = (float) (i_Move);
				f_Delay /= mpo_Frame->mst_Ini.f_Factor;
				f_Delay *= mpo_Frame->mst_Ini.f_ValFactor;
				mpo_HitTest->InterpolationKey_OnMouseMove(pst_Evt, mi_HitTest, f_Delay);
			}

			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		}

		Invalidate();
	}

	/* Hittest to determin where the mouse is */
	else
	{
		mpo_HitTest = NULL;
		mi_HitTest = HitTest(pt, &mpo_HitTest);
		if((mi_HitTest == 2) || (mi_HitTest == 3))
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    On key specific box
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else if(mi_HitTest >= 1000)
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Infos in status bar
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mpo_HitTest && mi_HitTest && mi_HitTest != 3)
	{
		*asz_Msg1 = 0;
		if(po_Prev) sprintf(asz_Msg1, "Prev %f  ", EVE_FrameToTime(po_Prev->pst_FindEngineEvent()->uw_NumFrames));
		*asz_Msg3 = 0;
		if(po_Next) sprintf(asz_Msg3, "  Next %f", EVE_FrameToTime(po_Next->pst_FindEngineEvent()->uw_NumFrames));
		sprintf
		(
			asz_Msg2,
			"%sCurrent %f%s",
			asz_Msg1,
			EVE_FrameToTime(mpo_HitTest->pst_FindEngineEvent()->uw_NumFrames),
			asz_Msg3
		);
		LINK_gb_UseSecond = TRUE;
		LINK_gb_CanLog = FALSE;
		LINK_PrintStatusMsg(asz_Msg2);
		LINK_gb_CanLog = TRUE;
		LINK_gb_UseSecond = FALSE;
	}
	else if(mi_HitTest == 3)
	{
		sprintf
		(
			asz_Msg1,
			"%f / %f",
			mpst_Param->f_Time,
			EVE_FrameToTime(mpst_Track->pst_AllEvents[mpst_Param->uw_CurrentEvent].uw_NumFrames)
		);
		LINK_gb_UseSecond = TRUE;
		LINK_gb_CanLog = FALSE;
		LINK_PrintStatusMsg(asz_Msg1);
		LINK_gb_CanLog = TRUE;
		LINK_gb_UseSecond = FALSE;
	}
	else
	{
		LINK_gb_UseSecond = TRUE;
		LINK_gb_CanLog = FALSE;
		LINK_PrintStatusMsg("");
		LINK_gb_CanLog = TRUE;
		LINK_gb_UseSecond = FALSE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::Select(void)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(mpst_Track->uw_Flags & EVE_C_Track_Selected) return;
	mpst_Track->uw_Flags |= EVE_C_Track_Selected;
	GetWindowRect(&o_Rect);
	GetParent()->GetParent()->ScreenToClient(&o_Rect);
	o_Rect.left = 0;
	o_Rect.right = mpo_Frame->mst_Ini.i_XLeft;
	GetParent()->GetParent()->InvalidateRect(&o_Rect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::Unselect(void)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(!(mpst_Track->uw_Flags & EVE_C_Track_Selected)) return;
	mpst_Track->uw_Flags &= ~EVE_C_Track_Selected;
	GetWindowRect(&o_Rect);
	GetParent()->GetParent()->ScreenToClient(&o_Rect);
	o_Rect.left = 0;
	o_Rect.right = mpo_Frame->mst_Ini.i_XLeft;
	GetParent()->GetParent()->InvalidateRect(&o_Rect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::UnselectTracks(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mpo_Frame->mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mpo_Frame->mo_ListTracks.GetNext(pos);
		po_Track->Unselect();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::OnLButtonDown(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	CPoint			pt1;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	mi_HitTest = HitTest(pt, &mpo_HitTest);

	/* Save initial point */
	mo_PanePt = pt;
	mo_ZoomRect.SetRectEmpty();

	if(GetAsyncKeyState(VK_SPACE) < 0)
	{
		ClientToScreen(&pt);
		GetParent()->ScreenToClient(&pt);
		GetParent()->SendMessage(WM_LBUTTONDOWN, 0, (pt.y << 16) + pt.x);
		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Inside
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mi_HitTest == 1)
	{
		mi_DecSize = 0;
		M_MF()->LockDisplay(mpo_Frame->mpo_VarsView);

		if(GetAsyncKeyState(VK_CONTROL) >= 0) mpo_Frame->UnselectAll();

		pst_Evt = mpo_HitTest->pst_FindEngineEvent();
		pt1 = pt;
		ClientToScreen(&pt1);
		mpo_Frame->SelectInRect(&CRect(pt1.x, pt1.y, pt1.x + 1, pt1.y + 1), FALSE, TRUE);

		mpo_HitTest->OnLButtonDown();

		FillBar();
		UnselectTracks();
		Select();

		M_MF()->UnlockDisplay(mpo_Frame->mpo_VarsView);
		mo_HitPt = pt;
		mb_MoveSize = TRUE;
		SetCapture();
		InvalidateRect(mpo_HitTest->mo_Rect);
		if(GetAsyncKeyState(VK_SHIFT) < 0)
			mb_SelMode = FALSE;
		else
			mb_SelMode = TRUE;

		LINK_Refresh();
		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Resize
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else if(mi_HitTest == 2)
	{
		mpo_Frame->SaveForUndo();
		mo_HitPt = pt;
		mb_MoveSize = TRUE;
		SetCapture();
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Running box
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else if(mi_HitTest == 3)
	{
		mo_HitPt = pt;
		mb_MoveSize = TRUE;
		SetCapture();
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    specific event hit test
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else if(mi_HitTest >= 1000)
	{
		mo_HitPt = pt;
		mb_MoveSize = TRUE;
		SetCapture();
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		return;
	}
	else
	{
		M_MF()->LockDisplay(mpo_Frame->mpo_VarsView);
		mpo_Frame->mpo_VarsView->ResetList();
		mpo_Frame->UnselectAll();
		mpo_Frame->mpo_LastSelected = NULL;
		Select();
		FillBar();
		M_MF()->UnlockDisplay(mpo_Frame->mpo_VarsView);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::OnLButtonDblClk(UINT, CPoint pt)
{
	/* Inside */
	if(mi_HitTest == 1)
	{
		mpo_HitTest->OnLButtonDblClk();
		if(mpo_Frame->mpst_GAO)
		{
			LINK_UpdatePointer(mpo_Frame->mpst_GAO);
			mpo_Frame->mb_LockUpdate = TRUE;
			LINK_UpdatePointers();
			mpo_Frame->mb_LockUpdate = FALSE;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::OnLButtonUp(UINT, CPoint pt)
{
	ClipCursor(NULL);
	if(mb_MoveSize)
	{
		if(mi_HitTest == 3)
		{
			EVE_gb_EndNeedToRecomputeSND = TRUE;
			mpo_Frame->MoveRunningBox(this, 0.0f);
			EVE_gb_EndNeedToRecomputeSND = FALSE;
		}

		mb_MoveSize = FALSE;
		ReleaseCapture();
		if(mb_SelMode) mpo_Frame->ForceRefresh();
	}

	mpo_Frame->RecomputeWidthFromCur();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::OnRButtonDown(UINT, CPoint pt)
{
	if(!mb_MoveSize) mpo_Frame->PopupEvent(this, mpo_HitTest);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::OnMButtonDown(UINT, CPoint pt)
{
	if(mi_HitTest == 1) mpo_HitTest->OnMButtonDown();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::CloseTrack(BOOL _b_Refresh)
{
	/*~~~~~~*/
	int index;
	/*~~~~~~*/

	if(_b_Refresh && mpst_Track) Unselect();

	for(index = 0; index < mo_ListEvents.GetSize(); index++)
	{
		if(mo_ListEvents.GetAt(index) == mpo_Frame->mpo_LastSelected) mpo_Frame->mpo_LastSelected = NULL;
		delete mo_ListEvents.GetAt(index);
	}

	mo_ListEvents.RemoveAll();
	mo_TrackTime.SetRectEmpty();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::SetTrack(EVE_tdst_Track *_pst_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			i;
	EEVE_cl_Event	*po_Event;
	int				j, iSel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	iSel = -1;
	for(j = 0; j < mo_ListEvents.GetSize(); j++)
	{
		if(mo_ListEvents.GetAt(j) == mpo_Frame->mpo_LastSelected)
		{
			iSel = j;
			break;
		}
	}

	CloseTrack(FALSE);
	mpst_Track = _pst_Track;

	/* Find the corresponding Event Param. */
	for(i = 0; i < mpo_Frame->mpst_Data->pst_ListTracks->uw_NumTracks; i++)
	{
		if(mpst_Track == &mpo_Frame->mpst_Data->pst_ListTracks->pst_AllTracks[i])
			mpst_Param = &mpo_Frame->mpst_Data->pst_ListParam[i];
	}

	if(_pst_Track && _pst_Track->pst_AllEvents)
	{
		for(i = 0; i < _pst_Track->uw_NumEvents; i++)
		{
			po_Event = new EEVE_cl_Event;
			mo_ListEvents.Add(po_Event);
			po_Event->mi_NumEvent = mo_ListEvents.GetSize() - 1;
			po_Event->mpo_Track = this;
			po_Event->RecomputeWidth();
			po_Event->Init();
		}
	}

	/* Change selection ? */
	if(iSel != -1)
	{
		if(iSel < mo_ListEvents.GetSize())
			mpo_Frame->mpo_LastSelected = mo_ListEvents.GetAt(iSel);
		else
			mpo_Frame->mpo_LastSelected = NULL;
	}

	mpo_HitTest = NULL;

	/* Keep selection status */
	if(mpst_Track->uw_Flags & EVE_C_Track_Selected) Select();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::FillJustBar(void)
{
	mpo_Frame->mpo_VarsView->AddItem("TrackBar", EVAV_EVVIT_Separator, NULL);
	mpo_Frame->mpo_VarsView->AddItem("Running", EVAV_EVVIT_Flags, &mpst_Param->uw_Flags, 0, 0, 2);
	mpo_Frame->mpo_VarsView->AddItem("Running Init", EVAV_EVVIT_Flags, &mpst_Track->uw_Flags, 0, 0, 2);
	mpo_Frame->mpo_VarsView->AddItem("Pause", EVAV_EVVIT_Flags, &mpst_Param->uw_Flags, 0, 1, 2);
	mpo_Frame->mpo_VarsView->AddItem("Event Done", EVAV_EVVIT_Flags, &mpst_Param->uw_Flags, 0, 2, 2);
	mpo_Frame->mpo_VarsView->AddItem("Auto Loop", EVAV_EVVIT_Flags, &mpst_Track->uw_Flags, 0, 1, 2);
	mpo_Frame->mpo_VarsView->AddItem("Auto Stop", EVAV_EVVIT_Flags, &mpst_Track->uw_Flags, 0, 2, 2);
	mpo_Frame->mpo_VarsView->AddItem("Time track", EVAV_EVVIT_Flags, &mpst_Track->uw_Flags, 0, 3, 2);
	mpo_Frame->mpo_VarsView->AddItem("Use time track", EVAV_EVVIT_Flags, &mpst_Track->uw_Flags, 0, 4, 2);
	mpo_Frame->mpo_VarsView->AddItem("Object", EVAV_EVVIT_GO, &mpst_Track->pst_GO, 0, (ULONG) mpo_Frame->mpst_GAO);
	mpo_Frame->mpo_VarsView->AddItem("Gizmo Number", EVAV_EVVIT_Int, &mpst_Track->uw_Gizmo, 0, 0, 0, 2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::FillBar(void)
{
	FillJustBar();
	mpo_Frame->mpo_VarsView->AddItem("List of tracks", EVAV_EVVIT_Separator, NULL);
	mpo_Frame->mpo_VarsView->AddItem("Ignore flash", EVAV_EVVIT_Flags, &mpo_Frame->mpst_ListTracks->uw_Flags, 0, 0, 2);
	mpo_Frame->mpo_VarsView->SetItemList(&mpo_Frame->mo_ListItems);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_cl_Track::ToggleExpand(CPoint &pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int				ht;
	EEVE_cl_Event	*po_Event;
	BOOL			expand;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	po_Event = NULL;
	ht = HitTest(pt, &po_Event, TRUE);
	if(po_Event && (po_Event->mb_Empty || po_Event->mb_Expand))
	{
		for(i = 0; i < mo_ListEvents.GetSize(); i++)
			if(mo_ListEvents[i] == po_Event) break;
		while(i && (mo_ListEvents[i - 1]->mb_Empty || mo_ListEvents[i - 1]->mb_Expand)) i--;

		po_Event = mo_ListEvents[i];
		expand = po_Event->mb_Expand ? FALSE : TRUE;
		while(i < mo_ListEvents.GetSize() && (mo_ListEvents[i]->mb_Empty || mo_ListEvents[i]->mb_Expand))
		{
			po_Event = mo_ListEvents[i];
			po_Event->mb_Expand = expand;
			i++;
		}

		Invalidate();
		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC				*pdc;
	CRect			o_Rect, o_Rect1;
	CRect			o_Rect2, o_Correct;
	PAINTSTRUCT		ps;
	EEVE_cl_Event	*po_Event;
	EEVE_cl_Event	*po_Event1;
	COLORREF		col, col1, col2;
	int				i_size;
	int				index;
	float			f_Width;
	float			f_TotalDelay;
	BOOL			b_SpecialCol;
	BOOL			b_SpecialCol1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mo_TrackTime.IsRectEmpty())
	{
		o_Rect = mo_TrackTime;
		o_Rect.left--;
		InvalidateRect(o_Rect);
	}

	GetUpdateRect(&o_Correct);

	pdc = BeginPaint(&ps);

	if(mo_ListEvents.GetSize() == 0)
	{
		GetClientRect(&o_Rect);
		pdc->FillSolidRect(&o_Rect, GetSysColor(COLOR_BTNFACE));
		pdc->Draw3dRect(&o_Rect, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHILIGHT));
	}
	else
	{
		if(mpo_Frame->mst_Ini.b_AnimMode)
		{
			GetClientRect(&o_Rect);
			pdc->FillSolidRect(&o_Rect, GetSysColor(COLOR_BTNFACE));
			pdc->Draw3dRect(&o_Rect, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHILIGHT));
		}

		GetClientRect(&o_Rect);
		o_Rect.right = o_Rect.left;
		o_Rect.right += EEVE_C_CXBorder;
		if(!mpo_Frame->mst_Ini.b_AnimMode) SpeedSolidRect(pdc, &o_Rect, GetSysColor(COLOR_3DSHADOW));
		o_Rect.left = o_Rect.right;
		f_TotalDelay = 0.0f;

		i_size = mo_ListEvents.GetSize();
		col1 = GetSysColor(COLOR_3DSHADOW);
		for(index = 0; index < i_size; index++)
		{
			po_Event = mo_ListEvents.GetAt(index);
			col = col1;
			po_Event1 = NULL;

			b_SpecialCol = FALSE;
			b_SpecialCol1 = FALSE;
			if(mpo_Frame->mst_Ini.i_AllEventsVis)
			{
				f_TotalDelay += mpo_Frame->mst_Ini.f_WidthFixed;
				col = 0x00FF0000;
				if(!mpst_Track->pst_AllEvents[index].uw_NumFrames) b_SpecialCol = TRUE;
			}
			else if(po_Event->mb_Expand)
			{
				f_TotalDelay += mpo_Frame->mst_Ini.f_ValFactor * 0.25f;
				b_SpecialCol1 = TRUE;
				col = 0x00FF0000;
			}
			else
			{
				f_TotalDelay += EVE_FrameToTime(mpst_Track->pst_AllEvents[index].uw_NumFrames);
			}

			if(mpo_Frame->mst_Ini.i_AllEventsVis || (po_Event->mf_CurWidth > EEVE_C_CXBorder2) || po_Event->mb_Expand)
			{
				f_Width = f_TotalDelay;
				if(!mpo_Frame->mst_Ini.i_AllEventsVis)
				{
					f_Width *= mpo_Frame->mst_Ini.f_Factor;
					f_Width /= mpo_Frame->mst_Ini.f_ValFactor;
				}

				o_Rect.right = (int) (f_Width + 0.5f);

				po_Event->mo_Rect = o_Rect;
				po_Event->mo_Rect.bottom = mpo_Frame->mst_Ini.i_YBar;
				if(o_Rect.right >= o_Correct.left)
				{
					if(!mpo_Frame->mst_Ini.b_AnimMode) po_Event->Draw(pdc, &o_Rect, b_SpecialCol, b_SpecialCol1);
				}

				/* Size bar */
				o_Rect.left = o_Rect.right;
				o_Rect.right += EEVE_C_CXBorder;
				if(o_Rect.right >= o_Correct.left)
				{
					if(!mpo_Frame->mst_Ini.b_AnimMode) SpeedSolidRect(pdc, &o_Rect, col);
				}

				o_Rect.left = o_Rect.right;
			}
			else
			{
				if(index == 0)
				{
					o_Rect.left -= EEVE_C_CXBorder;
					o_Rect.right -= EEVE_C_CXBorder;
				}

				po_Event->mo_Rect.SetRectEmpty();
				po_Event->mo_Rect.left = po_Event->mo_Rect.right = o_Rect.right;
				po_Event->mo_Rect.bottom = mpo_Frame->mst_Ini.i_YBar;

				if(index < i_size - 1)
				{
					if(!po_Event->mf_CurWidth)
					{
						o_Rect.right += EEVE_C_CXBorder;
						col = 0x000050FF;
						if(o_Rect.right >= o_Correct.left)
						{
							if(!mpo_Frame->mst_Ini.b_AnimMode) SpeedSolidRect(pdc, &o_Rect, col);
						}

						o_Rect.left = o_Rect.right;
						continue;
					}

					index++;
					po_Event = mo_ListEvents.GetAt(index);

					while((index < i_size - 1) && (po_Event->mf_CurWidth) && (po_Event->mf_CurWidth < EEVE_C_CXBorder2))
					{
						if(mpst_Track->pst_AllEvents[index].uw_NumFrames > 0)
							f_TotalDelay += EVE_FrameToTime(mpst_Track->pst_AllEvents[index].uw_NumFrames);
						po_Event->mo_Rect.SetRectEmpty();
						po_Event->mo_Rect.left = po_Event->mo_Rect.right = o_Rect.right;
						index++;
						if(index != i_size) po_Event = mo_ListEvents.GetAt(index);
					}

					/* Special draw */
					f_Width = f_TotalDelay;
					f_Width *= mpo_Frame->mst_Ini.f_Factor;
					f_Width /= mpo_Frame->mst_Ini.f_ValFactor;
					o_Rect.right = (int) (f_Width + 0.5f);
					if(o_Rect.right >= o_Correct.left)
					{
						col2 = 0x00209020;
						if(!mpo_Frame->mst_Ini.b_AnimMode)
						{
							SpeedSolidRect(pdc, &o_Rect, col2);
							pdc->Draw3dRect
								(
									&o_Rect,
									M_MF()->u4_Interpol2PackedColor(0x00FFFFFF, col2, 0.7f),
									M_MF()->u4_Interpol2PackedColor(0x00000000, col2, 0.7f)
								);
						}
					}

					o_Rect.left = o_Rect.right;
					o_Rect.right += EEVE_C_CXBorder;
					if(o_Rect.right >= o_Correct.left)
					{
						if(!mpo_Frame->mst_Ini.b_AnimMode) SpeedSolidRect(pdc, &o_Rect, col1);
					}

					o_Rect.left = o_Rect.right;

					index--;
				}
				else
				{
					/* Last Track Event is a NULL Delay one. */
					if(!po_Event->mf_CurWidth)
					{
						o_Rect.left -= EEVE_C_CXBorder;
						o_Rect.right -= EEVE_C_CXBorder;

						o_Rect.left = o_Rect.right;
						o_Rect.right += EEVE_C_CXBorder;
						col = 0x000050FF;
						if(o_Rect.right >= o_Correct.left)
						{
							if(!mpo_Frame->mst_Ini.b_AnimMode) SpeedSolidRect(pdc, &o_Rect, col);
						}
					}
				}
			}

			if(mpo_Frame->mst_Ini.b_AnimMode && o_Rect.left > o_Correct.right) break;
		}

		/* The rest... */
		GetClientRect(&o_Rect1);
		o_Rect1.left = o_Rect.right;
		if(!mpo_Frame->mst_Ini.b_AnimMode) pdc->FillSolidRect(&o_Rect1, GetSysColor(COLOR_BTNFACE));
		o_Rect2 = o_Rect1;
		o_Rect2.bottom = o_Rect2.top;
		if(!mpo_Frame->mst_Ini.b_AnimMode)
			pdc->Draw3dRect(&o_Rect2, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DDKSHADOW));
		o_Rect2 = o_Rect1;
		o_Rect2.top = o_Rect2.bottom;
		if(!mpo_Frame->mst_Ini.b_AnimMode)
			pdc->Draw3dRect(&o_Rect2, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DHILIGHT));
	}

	if(!mpo_Frame->mst_Ini.b_AnimMode) DrawBars();
	EndPaint(&ps);
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		EEVE_cl_Inside	*po_Inside;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		po_Inside = ((EEVE_cl_Inside *) mpo_Frame->mpo_MainSplitter->GetPane(0, 1));
		po_Inside->OnDrawMarks();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::DrawBars(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Event	*po_Event;
	int				index;
	float			f_Val;
	CDC				*pdc;
	CRect			o_Rect;
	CRect			o_Rect1;
	CRect			o_RectBar;
	int				index1;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Param || !mpst_Track) return;

	/* Find event */
	index = 0;
	f_Val = 0;
	GetClientRect(&o_RectBar);
	for(index1 = 0; index1 < mo_ListEvents.GetSize(); index1++)
	{
		po_Event = mo_ListEvents.GetAt(index1);
		if(index == mpst_Param->uw_CurrentEvent)
		{
			f_Val += mpst_Param->f_Time;
			f_Val *= mpo_Frame->mst_Ini.f_Factor;
			f_Val /= mpo_Frame->mst_Ini.f_ValFactor;
			mo_TrackTime = po_Event->mo_Rect;
			mo_TrackTime.bottom = mo_TrackTime.top + o_RectBar.Height();
			mo_TrackTime.left = (int) (f_Val + 0.5f);
			mo_TrackTime.right = mo_TrackTime.left + EEVE_C_CXBorder;
			pdc = GetDC();

			o_Rect = mo_TrackTime;
			o_Rect.left--;
			o_Rect1 = o_Rect;

			o_Rect.right = o_Rect.left + 1;
			pdc->Draw3dRect(&o_Rect, 0, 0);

			o_Rect = o_Rect1;
			o_Rect.left = o_Rect.right - 1;
			pdc->Draw3dRect(&o_Rect, 0, 0);

			ReleaseDC(pdc);
			break;
		}
		else
		{
			f_Val += EVE_FrameToTime(mpst_Track->pst_AllEvents[index1].uw_NumFrames);
		}

		index++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_cl_Track::UnselectAll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Event	*po_Event;
	EVE_tdst_Event	*pst_Evt;
	BOOL			b_Res;
	int				index;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	b_Res = FALSE;
	for(index = 0; index < mo_ListEvents.GetSize(); index++)
	{
		po_Event = mo_ListEvents.GetAt(index);
		pst_Evt = po_Event->pst_FindEngineEvent();
		if(pst_Evt && pst_Evt->w_Flags & EVE_C_EventFlag_Selected)
		{
			pst_Evt->w_Flags &= ~EVE_C_EventFlag_Selected;
			InvalidateRect(po_Event->mo_Rect);
			mi_NumSelected--;
			if(!mi_NumSelected) break;
			b_Res = TRUE;
		}
	}

	return b_Res;
}

/*$F
 =======================================================================================================================
    Note:   This function can only select CONSECUTIVE events.
							---------
			*b_Speed is set to indicate that the first selected event is the current one.
 =======================================================================================================================
 */
void EEVE_cl_Track::SelectCurrentEvent(BOOL _b_Speed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Event	*po_Event;
	EVE_tdst_Event	*pst_Evt;
	BOOL			b_SelectedEvent;
	int				index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mo_ListEvents.GetSize()) return;
	b_SelectedEvent = FALSE;

	/* Find first selected event. */
	if(_b_Speed)
	{
		index = mpst_Param->uw_CurrentEvent;
		b_SelectedEvent = TRUE;
	}
	else
	{
		for(index = 0; index < mo_ListEvents.GetSize(); index++)
		{
			po_Event = mo_ListEvents.GetAt(index);
			pst_Evt = po_Event->pst_FindEngineEvent();
			if(pst_Evt->w_Flags & EVE_C_EventFlag_Selected)
			{
				b_SelectedEvent = TRUE;
				break;
			}
		}
	}

	/* If no events have been selected yet. */
	if(!b_SelectedEvent)
	{
		po_Event = mo_ListEvents.GetAt(mpst_Param->uw_CurrentEvent);
		pst_Evt = po_Event->pst_FindEngineEvent();
		if(pst_Evt)
		{
			pst_Evt->w_Flags |= EVE_C_EventFlag_Selected;
			InvalidateRect(po_Event->mo_Rect);
			mi_NumSelected++;
		}
	}
	else
	{
		if(index <= mpst_Param->uw_CurrentEvent)
		{
			while(index <= mpst_Param->uw_CurrentEvent)
			{
				po_Event = mo_ListEvents.GetAt(index);
				pst_Evt = po_Event->pst_FindEngineEvent();
				if(pst_Evt && !(pst_Evt->w_Flags & EVE_C_EventFlag_Selected))
				{
					pst_Evt->w_Flags |= EVE_C_EventFlag_Selected;
					InvalidateRect(po_Event->mo_Rect);
					mi_NumSelected++;
				}

				index++;
			}

			while(index < mo_ListEvents.GetSize())
			{
				po_Event = mo_ListEvents.GetAt(index);
				pst_Evt = po_Event->pst_FindEngineEvent();
				if(pst_Evt && pst_Evt->w_Flags & EVE_C_EventFlag_Selected)
				{
					pst_Evt->w_Flags &= ~EVE_C_EventFlag_Selected;
					InvalidateRect(po_Event->mo_Rect);
					mi_NumSelected--;
				}

				index++;
			}
		}
		else
		{
			while(index >= mpst_Param->uw_CurrentEvent)
			{
				po_Event = mo_ListEvents.GetAt(index);
				pst_Evt = po_Event->pst_FindEngineEvent();
				if(pst_Evt && !(pst_Evt->w_Flags & EVE_C_EventFlag_Selected))
				{
					pst_Evt->w_Flags |= EVE_C_EventFlag_Selected;
					InvalidateRect(po_Event->mo_Rect);
					mi_NumSelected++;
				}

				index--;
			}

			if(!_b_Speed)
			{
				while(index >= 0)
				{
					po_Event = mo_ListEvents.GetAt(index);
					pst_Evt = po_Event->pst_FindEngineEvent();
					if(pst_Evt && pst_Evt->w_Flags & EVE_C_EventFlag_Selected)
					{
						pst_Evt->w_Flags &= ~EVE_C_EventFlag_Selected;
						InvalidateRect(po_Event->mo_Rect);
						mi_NumSelected--;
					}

					index--;
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::MoveRunningBox(BOOL _b_Refresh, float f_Delay)
{
	/* if(!f_Delay) return; */
	EVE_gb_CanFlash = FALSE;
	EVE_AddTime(mpo_Frame->mpst_GAO, mpo_Frame->mpst_ListTracks, mpst_Param, mpst_Track, f_Delay);
	EVE_gb_CanFlash = TRUE;
	OnPaint();
	if(_b_Refresh)
	{
		if(mpo_Frame->mpst_GAO) OBJ_HierarchyMainCall(mpo_Frame->mpst_GAO->pst_World);
		LINK_Refresh();
	}

	/* Recompute current event depending of engine */
	if(mpst_Param->uw_CurrentEvent < mo_ListEvents.GetSize())
		mpo_HitTest = mo_ListEvents.GetAt(mpst_Param->uw_CurrentEvent);
	else
		mpo_HitTest = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Track::RoundTo(float _f_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int				index;
	EEVE_cl_Event	*po_Event;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	for(index = 0; index < mo_ListEvents.GetSize(); index++)
	{
		po_Event = mo_ListEvents.GetAt(index);
		po_Event->RoundTo(_f_Val);
	}
}
#endif /* ACTIVE_EDITORS */
