/*$T EVEevent_interpolationkey.cpp GC! 1.081 10/27/00 11:23:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "EVEevent.h"
#include "EVEtrack.h"
#include "EVEframe.h"
#include "EDImainframe.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDIeditors_infos.h"
#include "MATHs/MATH.h"
#include "Res/Res.h"
#include "LINks/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "DIAlogs/DIAtimeinterpolation_dlg.h"
#include "EVEmsg.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ChangePos(void *, void *, void *, LONG)
{
	M_MF()->SendMessageToEditors(EEVE_MESSAGE_FORCEPLAY0, 0, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::FillInterpolationKey(EVE_tdst_Event *_pst_Evt)
{
	/*~~~~~~~~~~~~~~~~*/
	short	w_Type;
	char	*pc_Data;
	int		i_Number, i;
	char	sz_Name[64];
	EVAV_cl_ViewItem *po;
	/*~~~~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Evt);

	if(w_Type == -1) return;

	pc_Data = (char *) _pst_Evt->p_Data;
	pc_Data += 4;

	if(w_Type & EVE_InterKeyType_Translation_Mask)
	{
		po = mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Position", EVAV_EVVIT_Vector, pc_Data);
		po->mpfn_CB = ChangePos;
		po = mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Position X", EVAV_EVVIT_Float, &((MATH_tdst_Vector *) pc_Data)->x);
		po->mpfn_CB = ChangePos;
		po = mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Position Y", EVAV_EVVIT_Float, &((MATH_tdst_Vector *) pc_Data)->y);
		po->mpfn_CB = ChangePos;
		po = mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Position Z", EVAV_EVVIT_Float, &((MATH_tdst_Vector *) pc_Data)->z);
		po->mpfn_CB = ChangePos;
		pc_Data += sizeof(MATH_tdst_Vector);
	}

	if((w_Type & EVE_InterKeyType_Translation_Mask) > 1)
	{
		mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Control Point 0", EVAV_EVVIT_Vector, pc_Data);
		pc_Data += sizeof(MATH_tdst_Vector);
	}

	if((w_Type & EVE_InterKeyType_Translation_Mask) > 2)
	{
		mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Control Point 1", EVAV_EVVIT_Vector, pc_Data);
		pc_Data += sizeof(MATH_tdst_Vector);
	}

	if(w_Type & EVE_InterKeyType_Rotation_Matrix)
	{
		mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Rotation", EVAV_EVVIT_Matrix, pc_Data);
		pc_Data += sizeof(MATH_tdst_Matrix);
	}

	if(w_Type & EVE_InterKeyType_Rotation_Quaternion)
	{
		mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Quaternion", EVAV_EVVIT_Vector, pc_Data);
		pc_Data += sizeof(MATH_tdst_Vector);
		mpo_Track->mpo_Frame->mpo_VarsView->AddItem("W", EVAV_EVVIT_Float, pc_Data);
		pc_Data += sizeof(float);
	}

	if(w_Type & EVE_InterKeyType_HasNextValue)
	{
		if(w_Type & EVE_InterKeyType_Translation_Mask)
		{
			mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Next position", EVAV_EVVIT_Vector, pc_Data);
			pc_Data += sizeof(MATH_tdst_Vector);
		}

		if(w_Type & EVE_InterKeyType_Rotation_Matrix)
		{
			mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Next rotation", EVAV_EVVIT_Matrix, pc_Data);
			pc_Data += sizeof(MATH_tdst_Matrix);
		}

		if(w_Type & EVE_InterKeyType_Rotation_Quaternion)
		{
			mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Next quaternion", EVAV_EVVIT_Vector, pc_Data);
			pc_Data += sizeof(MATH_tdst_Vector);
			mpo_Track->mpo_Frame->mpo_VarsView->AddItem("NExt W", EVAV_EVVIT_Float, pc_Data);
			pc_Data += sizeof(float);
		}
	}

	if(w_Type & EVE_InterKeyType_Time)
	{
		i_Number = *(int *) pc_Data;
		pc_Data += 4;
		mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Factor 0", EVAV_EVVIT_Float, pc_Data);
		pc_Data += 4;
		for(i = 1; i < i_Number; i++)
		{
			sprintf(sz_Name, "Time %d", i);
			mpo_Track->mpo_Frame->mpo_VarsView->AddItem(sz_Name, EVAV_EVVIT_Float, pc_Data);
			pc_Data += 4;
			sprintf(sz_Name, "Factor %d", i);
			mpo_Track->mpo_Frame->mpo_VarsView->AddItem(sz_Name, EVAV_EVVIT_Float, pc_Data);
			pc_Data += 4;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::SetInterpolationKeyName(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = pst_FindEngineEvent();
	if(!pst_Evt->p_Data)
		mo_Name = "";
	else
		mo_Name = "Key";
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::ComputeInterpolationKey(EVE_tdst_Event *_pst_Evt, int _w_Type, BOOL _b_New )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_w_Type == -1) return;

	pst_GO = mpo_Track->mpst_Track->pst_GO;
	if(!pst_GO) pst_GO = mpo_Track->mpo_Frame->mpst_GAO;
	if(!pst_GO) return;

	EVE_Event_InterpolationKey_Compute
	(
		pst_GO,
		mpo_Track->mpo_Frame->mpst_GAO,
		mpo_Track->mpo_Frame->mpst_ListTracks,
		mpo_Track->mpst_Track,
		_pst_Evt,
		_w_Type, 
        _b_New ? 1 : 0
	);

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::SetInterpolationKey(EVE_tdst_Event *_pst_Evt, BOOL _b_New)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	EMEN_cl_SubMenu o_Menu(FALSE);
	CPoint			pt, ptInEvent;
	short			w_Type;

	/* float f_Time; */
	BOOL			b_Menu;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Menu = FALSE;

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Evt);

	if(w_Type == -1) return;

	GetCursorPos(&pt);
	M_MF()->InitPopupMenuAction(NULL, &o_Menu);

	if(
		w_Type & EVE_InterKeyType_Translation_Mask
		&& !(_b_New && _pst_Evt->w_Flags & EVE_C_EventFlag_NoInterpolation))
	{
		M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, 0, TRUE, "Translation Key", -1);
		M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_InterKeyType_Translation_0, TRUE, "Linear", -1);
		M_MF()->AddPopupMenuAction
			(
				mpo_Track->mpo_Frame,
				&o_Menu,
				EVE_InterKeyType_Translation_1,
				TRUE,
				"1 Control Point",
				-1
			);
		M_MF()->AddPopupMenuAction
			(
				mpo_Track->mpo_Frame,
				&o_Menu,
				EVE_InterKeyType_Translation_2,
				TRUE,
				"2 Control Points",
				-1
			);
		b_Menu = TRUE;
	}

	if(!_b_New)
	{
		if(_pst_Evt->w_Flags & EVE_C_EventFlag_NoInterpolation)
			M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, 200, TRUE, "Interpole", -1);
		else
			M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, 200, TRUE, "No Interpole", -1);

		/*
		 * M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu,
		 * EVE_InterKeyType_Time, TRUE, "Add time key", -1);
		 */
		M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, 1001, TRUE, "Edit time key", -1);
		if(w_Type & EVE_InterKeyType_BlockedForIK)
		{
			M_MF()->AddPopupMenuAction
				(
					mpo_Track->mpo_Frame,
					&o_Menu,
					EVE_InterKeyType_BlockedForIK,
					TRUE,
					"Don't block for IK",
					-1
				);
		}
		else
		{
			M_MF()->AddPopupMenuAction
				(
					mpo_Track->mpo_Frame,
					&o_Menu,
					EVE_InterKeyType_BlockedForIK,
					TRUE,
					"Block for IK",
					-1
				);
		}

		M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, 1000, TRUE, "Recompute", -1);
		b_Menu = TRUE;
	}

	if(b_Menu)
		i = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	else
		i = w_Type;

	if(i > 0)
	{
		if(i == EVE_InterKeyType_BlockedForIK)
		{
			if(w_Type & EVE_InterKeyType_BlockedForIK)
				w_Type -= EVE_InterKeyType_BlockedForIK;
			else
				w_Type += EVE_InterKeyType_BlockedForIK;
			EVE_Event_InterpolationKey_ChangeTypeWithoutChangingData(_pst_Evt, w_Type);
		}

		/*$F
        else if(i == EVE_InterKeyType_Time)
		{
            ptInEvent = pt;
			mpo_Track->ScreenToClient(&ptInEvent);
			f_Time = ((float) ptInEvent.x - mo_Rect.left) / ((float) mo_Rect.right - mo_Rect.left);
			f_Time *= _pst_Evt->f_Delay;
			EVE_Event_InterpolationKey_AddTimeInterpolation(_pst_Evt, f_Time);
			mpo_Track->Invalidate();
		}
        */
		else if(i == 1001)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_TimeInter	o_Dlg(_pst_Evt);
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			o_Dlg.DoModal();
		}
		else if(i == 200)
		{
			if(_pst_Evt->w_Flags & EVE_C_EventFlag_NoInterpolation)
				_pst_Evt->w_Flags &= ~EVE_C_EventFlag_NoInterpolation;
			else
				_pst_Evt->w_Flags |= EVE_C_EventFlag_NoInterpolation;
		}
		else
		{
			if(i != 1000)
			{
				w_Type = (short) i;
				EVE_Event_InterpolationKey_AllocData(_pst_Evt, w_Type, 0);

				if (i == EVE_InterKeyType_Translation_1 || i == EVE_InterKeyType_Translation_2)
				{
					EVE_tdst_Event		*pst_NextEvt;
					int					i_EveIdx;
					MATH_tdst_Vector	*pst_VControl1, *pst_VControl2, *pst_V1, *pst_V2;

					/** Try to find next event on track to put the control point between the two events **/
					i_EveIdx = _pst_Evt - mpo_Track->mpst_Track->pst_AllEvents;
					pst_NextEvt = mpo_Track->mpst_Track->pst_AllEvents + i_EveIdx + 1;
					while (pst_NextEvt != _pst_Evt)
					{
						if (pst_NextEvt == mpo_Track->mpst_Track->pst_AllEvents + mpo_Track->mpst_Track->uw_NumEvents)
						{
							pst_NextEvt = mpo_Track->mpst_Track->pst_AllEvents;
							continue;
						}
						if (pst_NextEvt->w_Flags & EVE_C_EventFlag_InterpolationKey)
							break;
						pst_NextEvt++;
					}
					
					pst_VControl1 = (MATH_tdst_Vector *)( ((char *)_pst_Evt->p_Data) + 4 + sizeof(MATH_tdst_Vector) );
					if (pst_NextEvt == _pst_Evt
						|| pst_NextEvt < mpo_Track->mpst_Track->pst_AllEvents
						|| pst_NextEvt >= mpo_Track->mpst_Track->pst_AllEvents +mpo_Track->mpst_Track->uw_NumEvents
						)
					{
						// No next event
						// Move control points so that they are not exactly at the same position
						// as the event position
						pst_VControl1->x++;
						pst_VControl1->y++;
						pst_VControl1->z++;
						if (i == EVE_InterKeyType_Translation_2)
						{
							MATH_tdst_Vector	*pst_VControl2;
							pst_VControl2 = (MATH_tdst_Vector *)( ((char *)_pst_Evt->p_Data) + 4 + 2*sizeof(MATH_tdst_Vector) );
							pst_VControl2->x--;
							pst_VControl2->y--;
							pst_VControl2->z--;
						}
					}
					else
					{
						pst_V1 = (MATH_tdst_Vector *)( ((char *)_pst_Evt->p_Data) + 4 );
						pst_V2 = (MATH_tdst_Vector *)( ((char *)pst_NextEvt->p_Data) + 4 );
						if (i == EVE_InterKeyType_Translation_2)
						{
							pst_VControl2 = (MATH_tdst_Vector *)( ((char *)_pst_Evt->p_Data) + 4 + 2*sizeof(MATH_tdst_Vector) );
							pst_VControl1->x = 2.0f/3.0f * pst_V1->x + 1.0f/3.0f * pst_V2->x;
							pst_VControl1->y = 2.0f/3.0f * pst_V1->y + 1.0f/3.0f * pst_V2->y;
							pst_VControl1->z = 2.0f/3.0f * pst_V1->z + 1.0f/3.0f * pst_V2->z;
							pst_VControl2->x = 1.0f/3.0f * pst_V1->x + 2.0f/3.0f * pst_V2->x;
							pst_VControl2->y = 1.0f/3.0f * pst_V1->y + 2.0f/3.0f * pst_V2->y;
							pst_VControl2->z = 1.0f/3.0f * pst_V1->z + 2.0f/3.0f * pst_V2->z;
						}
						else
						{
							pst_VControl1->x = (pst_V1->x + pst_V2->x) / 2.0f;
							pst_VControl1->y = (pst_V1->y + pst_V2->y) / 2.0f;
							pst_VControl1->z = (pst_V1->z + pst_V2->z) / 2.0f;
						}
					}
				}
			}
			ComputeInterpolationKey(_pst_Evt, w_Type, _b_New );
			SetInterpolationKeyName();
			mpo_Track->Invalidate();
		}
	}

	mpo_Track->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::DrawInterpolationKey(EVE_tdst_Event *_pst_Evt, CDC *pdc, CRect *_po_Rect)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	short		w_Type;
	CRect		o_Rect, o_TimeRect;
	COLORREF	col;
	float		*pf_TimeKey, f_Delay, f_Time, f_Dist, t;
	int			i_NbTimeKey, i, x;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Evt);
	if(w_Type == -1) return;

	if(w_Type & EVE_InterKeyType_Rotation_Mask)
		col = 0x0000009F;
	else
		col = 0x009F0000;

	if(_pst_Evt->w_Flags & EVE_C_EventFlag_Symetric) col = 0x00009F00;

	if(_pst_Evt->w_Flags & EVE_C_EventFlag_NoInterpolation)
	{
		if(w_Type & EVE_InterKeyType_Rotation_Mask)
			col = 0x000050FF;
		else
			col = 0x00969100;
	}

	if(w_Type & EVE_InterKeyType_Time)
	{
		pf_TimeKey = EVE_pf_Event_InterpolationKey_GetTime(_pst_Evt);
		i_NbTimeKey = *(int *) pf_TimeKey;
	}
	else
		i_NbTimeKey = 0;

	if(i_NbTimeKey < 2)
	{
		o_Rect = *_po_Rect;
		o_Rect.left += 4;
		o_Rect.top += 4;
		o_Rect.bottom = o_Rect.top + 5;
		o_Rect.right = o_Rect.left + 5;
		if(o_Rect.left < o_Rect.right) SpeedSolidRect(pdc, &o_Rect, col);
		if(w_Type & EVE_InterKeyType_BlockedForIK)
		{
			o_Rect = *_po_Rect;
			o_Rect.left += 12;
			pdc->SetTextColor(0);
			pdc->SetBkMode(TRANSPARENT);
			pdc->DrawText("IK", &o_Rect, DT_VCENTER);
		}
	}
	else
	{
		o_Rect = mo_Rect;
		o_Rect.DeflateRect(0, 2);
		f_Dist = (float) (o_Rect.right - o_Rect.left);
		if(f_Dist <= 0) return;
		f_Dist /= EVE_TimeToFrame(_pst_Evt->uw_NumFrames);
		f_Delay = 0;
		f_Time = 0;
		for(i = 1; i < i_NbTimeKey; i++)
		{
			t = pf_TimeKey[2] - f_Delay;
			f_Delay += t;
			f_Time += t * pf_TimeKey[1];

			x = (int) (f_Delay * f_Dist) + o_Rect.left;
			o_TimeRect.left = x;
			o_TimeRect.right = x + 4;
			o_TimeRect.top = o_Rect.top;
			o_TimeRect.bottom = o_Rect.top + 4;
			if(o_TimeRect.left < o_TimeRect.right) SpeedSolidRect(pdc, &o_TimeRect, col);

			x = (int) (f_Time * f_Dist) + o_Rect.left;
			o_TimeRect.left = x;
			o_TimeRect.right = x + 4;
			o_TimeRect.top = o_Rect.bottom - 4;
			o_TimeRect.bottom = o_Rect.bottom;
			if(o_TimeRect.left < o_TimeRect.right) SpeedSolidRect(pdc, &o_TimeRect, col);
			pf_TimeKey += 2;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EEVE_cl_Event::InterpolationKey_i_HitTest(EVE_tdst_Event *_pst_Evt, CPoint _o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	short	w_Type;
	float	*pf_TimeKey, f_Dist, f_Time, f_Delay, t;
	int		i_NbTimeKey, i, row, x;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(_pst_Evt);
	if(w_Type == -1) return 0;

	if((_o_Pt.y - mo_Rect.top) < 6)
		row = 0;
	else if((mo_Rect.bottom - _o_Pt.y) < 6)
		row = 1;
	else
		return 0;

	if(!(w_Type & EVE_InterKeyType_Time)) return 0;

	pf_TimeKey = EVE_pf_Event_InterpolationKey_GetTime(_pst_Evt);
	i_NbTimeKey = *(int *) pf_TimeKey;
	if(i_NbTimeKey < 2) return 0;
	f_Delay = 0;
	f_Time = 0;
	f_Dist = (float) (mo_Rect.right - mo_Rect.left);
	if(f_Dist <= 0) return 0;
	f_Dist /= EVE_FrameToTime(_pst_Evt->uw_NumFrames);

	for(i = 1; i < i_NbTimeKey; i++)
	{
		t = pf_TimeKey[2] - f_Time;
		f_Time += t;
		if(row == 1) t *= pf_TimeKey[1];

		f_Delay += t;

		x = (int) (f_Delay * f_Dist) + mo_Rect.left;
		if(x > _o_Pt.x) return 0;
		if(x + 4 >= _o_Pt.x) return(1000 + i * 2 + row);

		pf_TimeKey += 2;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::InterpolationKey_OnMouseMove(EVE_tdst_Event *_pst_Evt, int _i_Code, float _f_Delta)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		row;
	float	*pf_TimeKey, t, f_Min, f_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_i_Code -= 1000;
	row = _i_Code & 1;
	_i_Code >>= 1;

	pf_TimeKey = EVE_pf_Event_InterpolationKey_GetTime(_pst_Evt);

	if(row == 0)
	{
		if(_i_Code >= 2)
			f_Min = pf_TimeKey[(_i_Code - 1) * 2];
		else
			f_Min = 0;

		if(_i_Code + 1 < *(int *) pf_TimeKey)
			f_Max = pf_TimeKey[(_i_Code + 1) * 2];
		else
			f_Max = EVE_FrameToTime(_pst_Evt->uw_NumFrames);

		if(_f_Delta <= f_Min) return;
		if(_f_Delta >= f_Max) return;

		pf_TimeKey[_i_Code * 2 - 1] = (pf_TimeKey[_i_Code * 2] - f_Min) /
			(_f_Delta - f_Min) *
			pf_TimeKey[_i_Code * 2 - 1];
		pf_TimeKey[_i_Code * 2 + 1] = (pf_TimeKey[_i_Code * 2] - f_Max) /
			(_f_Delta - f_Max) *
			pf_TimeKey[_i_Code * 2 + 1];
		pf_TimeKey[_i_Code * 2] = _f_Delta;
	}
	else
	{
		if(_i_Code == 1)
			f_Min = 0;
		else
			f_Min = EVE_f_Event_InterpolationKey_InterpolateTime(_pst_Evt, pf_TimeKey[(_i_Code - 1) * 2]);

		if(_i_Code + 1 < *(int *) pf_TimeKey)
			f_Max = EVE_f_Event_InterpolationKey_InterpolateTime(_pst_Evt, pf_TimeKey[(_i_Code + 1) * 2]);
		else
			f_Max = EVE_FrameToTime(_pst_Evt->uw_NumFrames);

		if(_f_Delta <= f_Min) return;
		if(_f_Delta >= f_Max) return;

		t = EVE_f_Event_InterpolationKey_InterpolateTime(_pst_Evt, pf_TimeKey[_i_Code * 2]);

		pf_TimeKey[_i_Code * 2 - 1] = (_f_Delta - f_Min) / (t - f_Min) * pf_TimeKey[_i_Code * 2 - 1];
		pf_TimeKey[_i_Code * 2 + 1] = (_f_Delta - f_Max) / (t - f_Max) * pf_TimeKey[_i_Code * 2 + 1];
	}

	LINK_Refresh();
}

#endif /* ACTIVE_EDITORS */
