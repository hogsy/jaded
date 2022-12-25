/*$T EVEframe_anim.cpp GC! 1.081 01/25/02 11:24:47 */


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
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "AIinterp/Sources/Events/EVEnt_timekey.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDIpaths.h"
#include "EDIeditors_infos.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "EDIstrings.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ENGcall.h"
#include "OUTput/OUTframe.h"
#include "OUTput/OUTmsg.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "SELection/SELection.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::DisplayHideCurveForGAO(OBJ_tdst_GameObject *_pst_Main, OBJ_tdst_GameObject *_pst_Gizmo, BOOL _b_Rot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track			*pst_Track;
	int						i;
	EOUT_tdst_CurveParam	st_Curve;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpst_GAO != _pst_Main) return;
	pst_Track = mpst_ListTracks->pst_AllTracks;

	for(i = 0; i < mpst_ListTracks->uw_NumTracks; i++, pst_Track++)
	{
		if(_b_Rot && ((i % 2) != 0)) continue;
		if(!_b_Rot && ((i % 2) == 0)) continue;
		if(pst_Track->uw_Flags & EVE_C_Track_Selected)
		{
			st_Curve.pst_GAO = mpst_GAO;
			st_Curve.pst_Data = mpst_Data;
			st_Curve.pst_Track = pst_Track;
			if(pst_Track->uw_Flags & EVE_C_Track_Curve)
			{
				pst_Track->uw_Flags &= ~EVE_C_Track_Curve;
				M_MF()->SendMessageToEditors(EOUT_MESSAGE_ANIMDELCURVE, (ULONG) & st_Curve, 0);
			}
			else
			{
				pst_Track->uw_Flags |= EVE_C_Track_Curve;
				M_MF()->SendMessageToEditors(EOUT_MESSAGE_ANIMADDCURVE, (ULONG) & st_Curve, 0);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::ShowAllCurves(BOOL _b_Rot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track			*pst_Track;
	EOUT_tdst_CurveParam	st_Curve;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = mpst_ListTracks->pst_AllTracks;
	for(int j = 0; j < mpst_ListTracks->uw_NumTracks; j++)
	{
		if(!(pst_Track->uw_Flags & EVE_C_Track_Curve))
		{
			if((_b_Rot && (!(j % 2))) || (!_b_Rot && (j % 2)))
			{
				pst_Track->uw_Flags |= EVE_C_Track_Curve;
				st_Curve.pst_GAO = mpst_GAO;
				st_Curve.pst_Data = mpst_Data;
				st_Curve.pst_Track = pst_Track;
				M_MF()->SendMessageToEditors(EOUT_MESSAGE_ANIMADDCURVE, (ULONG) & st_Curve, 0);
			}
		}

		pst_Track++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::DelAllCurves(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track			*pst_Track;
	EOUT_tdst_CurveParam	st_Curve;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = mpst_ListTracks->pst_AllTracks;
	for(int j = 0; j < mpst_ListTracks->uw_NumTracks; j++)
	{
		if(pst_Track->uw_Flags & EVE_C_Track_Curve)
		{
			pst_Track->uw_Flags &= ~EVE_C_Track_Curve;
			st_Curve.pst_GAO = mpst_GAO;
			st_Curve.pst_Data = mpst_Data;
			st_Curve.pst_Track = pst_Track;
			M_MF()->SendMessageToEditors(EOUT_MESSAGE_ANIMDELCURVE, (ULONG) & st_Curve, 0);
		}

		pst_Track++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SplitEventForGAO
(
	OBJ_tdst_GameObject *_pst_Main,
	OBJ_tdst_GameObject *_pst_Gizmo,
	BOOL				_b_Rot,
	BOOL				_b_Refresh
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float			f_Width, f_Dif;
	EVE_tdst_Track	*pst_Track;
	EEVE_cl_Track	*po_Track;
	EVE_tdst_Params *pst_ListParam;
	int				align;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpst_GAO != _pst_Main) return;
	pst_Track = mpst_ListTracks->pst_AllTracks;
	pst_ListParam = mpst_Data->pst_ListParam;
	align = 0;
	for(int i = 0; i < mpst_ListTracks->uw_NumTracks; i++, pst_Track++, pst_ListParam++)
	{
		if(_b_Rot && ((i % 2) != 0)) continue;
		if(!_b_Rot && ((i % 2) == 0)) continue;
		if(pst_Track->uw_Flags & EVE_C_Track_Selected)
		{
			if(fabs(pst_ListParam->f_Time) > 10e-7)
			{
				SplitComputeWidth
				(
					pst_Track,
					pst_ListParam,
					pst_ListParam->uw_CurrentEvent,
					CPoint(0, 0),
					NULL,
					&f_Width,
					&f_Dif,
					FALSE
				);
				SplitOneEvent(pst_Track, pst_ListParam, pst_ListParam->uw_CurrentEvent, f_Width, f_Dif);
				po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i));
				po_Track->SetTrack(pst_Track);
				po_Track->Invalidate();
				if(i == align) align++;
			}
		}
	}

	Align(align);
	if(_b_Refresh) LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::DelEventForGAO(OBJ_tdst_GameObject *_pst_Main, OBJ_tdst_GameObject *_pst_Gizmo, BOOL _b_Rot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	EEVE_cl_Track	*po_Track;
	EVE_tdst_Params *pst_ListParam;
	int				align;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpst_GAO != _pst_Main) return;
	pst_Track = mpst_ListTracks->pst_AllTracks;
	pst_ListParam = mpst_Data->pst_ListParam;
	align = 0;
	for(int i = 0; i < mpst_ListTracks->uw_NumTracks; i++, pst_Track++, pst_ListParam++)
	{
		if(_b_Rot && ((i % 2) != 0)) continue;
		if(!_b_Rot && ((i % 2) == 0)) continue;
recom:
		for(int j = 1; j < pst_Track->uw_NumEvents - 1; j++)
		{
			if(pst_Track->pst_AllEvents[j].w_Flags & EVE_C_EventFlag_Selected)
			{
				DeleteOneEvent(pst_Track, j);
				po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i));
				po_Track->SetTrack(pst_Track);
				po_Track->Invalidate();
				if(i == align) align++;
				goto recom;
			}
		}
	}

	Align(align);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::MoveKeyLeftForGAO(OBJ_tdst_GameObject *_pst_Main, OBJ_tdst_GameObject *_pst_Gizmo, BOOL _b_Rot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	int				i;
	EVE_tdst_Params *pst_ListParam;
	EEVE_cl_Track	*po_Track;
	int				align;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpst_GAO != _pst_Main) return;
	pst_Track = mpst_ListTracks->pst_AllTracks;
	pst_ListParam = mpst_Data->pst_ListParam;
	align = 0;
	for(i = 0; i < mpst_ListTracks->uw_NumTracks; i++, pst_Track++, pst_ListParam++)
	{
		if(_b_Rot && ((i % 2) != 0)) continue;
		if(!_b_Rot && ((i % 2) == 0)) continue;
		if(pst_Track->uw_Flags & EVE_C_Track_Selected)
		{
			if((fabs(pst_ListParam->f_Time) < 10e-7) && (pst_ListParam->uw_CurrentEvent))
			{
				if((pst_ListParam->uw_CurrentEvent != 0) && (pst_ListParam->uw_CurrentEvent != pst_Track->uw_NumEvents - 1) && (pst_Track->pst_AllEvents[pst_ListParam->uw_CurrentEvent - 1].uw_NumFrames > 1))
				{
					pst_Track->pst_AllEvents[pst_ListParam->uw_CurrentEvent - 1].uw_NumFrames--;
					pst_Track->pst_AllEvents[pst_ListParam->uw_CurrentEvent].uw_NumFrames++;
					po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i));
					po_Track->SetTrack(pst_Track);
					po_Track->Invalidate();
					if(i == align) align++;
				}
			}
		}
	}

	Align(align);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::MoveKeyRightForGAO(OBJ_tdst_GameObject *_pst_Main, OBJ_tdst_GameObject *_pst_Gizmo, BOOL _b_Rot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	int				i;
	EVE_tdst_Params *pst_ListParam;
	EEVE_cl_Track	*po_Track;
	int				align;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpst_GAO != _pst_Main) return;
	pst_Track = mpst_ListTracks->pst_AllTracks;
	pst_ListParam = mpst_Data->pst_ListParam;
	align = 0;
	for(i = 0; i < mpst_ListTracks->uw_NumTracks; i++, pst_Track++, pst_ListParam++)
	{
		if(_b_Rot && ((i % 2) != 0)) continue;
		if(!_b_Rot && ((i % 2) == 0)) continue;
		if(pst_Track->uw_Flags & EVE_C_Track_Selected)
		{
			if((fabs(pst_ListParam->f_Time) < 10e-7) && (pst_ListParam->uw_CurrentEvent != pst_Track->uw_NumEvents - 1))
			{
				if((pst_ListParam->uw_CurrentEvent != 0) && (pst_ListParam->uw_CurrentEvent != pst_Track->uw_NumEvents - 1) && (pst_Track->pst_AllEvents[pst_ListParam->uw_CurrentEvent - 1].uw_NumFrames > 1))
				{
					pst_Track->pst_AllEvents[pst_ListParam->uw_CurrentEvent].uw_NumFrames--;
					pst_Track->pst_AllEvents[pst_ListParam->uw_CurrentEvent - 1].uw_NumFrames++;
					po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i));
					po_Track->SetTrack(pst_Track);
					po_Track->Invalidate();
					if(i == align) align++;
				}
			}
		}
	}

	Align(align);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::AddKey(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	EEVE_cl_Track	*po_Track;
	EVE_tdst_Params *pst_ListParam;
	float			f_Width, f_Dif;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(int i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
	{
		pst_Track = mpst_ListTracks->pst_AllTracks + i;
		pst_ListParam = mpst_Data->pst_ListParam + i;
		if(fabs(pst_ListParam->f_Time) > 10e-7)
		{
			SplitComputeWidth
			(
				pst_Track,
				pst_ListParam,
				pst_ListParam->uw_CurrentEvent,
				CPoint(0, 0),
				NULL,
				&f_Width,
				&f_Dif,
				FALSE
			);
			SplitOneEvent(pst_Track, pst_ListParam, pst_ListParam->uw_CurrentEvent, f_Width, f_Dif);
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i));
			po_Track->SetTrack(pst_Track);
			po_Track->Invalidate();
		}
	}

	Align(0);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::DelKey(BOOL _b_Ext)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	EEVE_cl_Track	*po_Track;
	EVE_tdst_Params *pst_ListParam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(int i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
	{
		pst_Track = mpst_ListTracks->pst_AllTracks + i;
		pst_ListParam = mpst_Data->pst_ListParam + i;
		if(fabs(pst_ListParam->f_Time) < 10e-7)
		{
			DeleteOneEvent(pst_Track, pst_ListParam->uw_CurrentEvent);
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i));
			po_Track->SetTrack(pst_Track);
			po_Track->Invalidate();
		}
	}

	MoveRunningBox(NULL, -(1.0f / 60.f), _b_Ext ? FALSE : TRUE);
	if(!_b_Ext) LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::DelKeyFrame(BOOL _b_AndKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	EEVE_cl_Track	*po_Track;
	EVE_tdst_Params *pst_ListParam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(int i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
	{
		pst_Track = mpst_ListTracks->pst_AllTracks + i;
		pst_ListParam = mpst_Data->pst_ListParam + i;
		if(!_b_AndKey && pst_ListParam->f_Time < 10e-7) continue;

		if(pst_Track->pst_AllEvents)
		{
			if(pst_ListParam->uw_CurrentEvent == pst_Track->uw_NumEvents - 1) break;
			pst_Track->pst_AllEvents[pst_ListParam->uw_CurrentEvent].uw_NumFrames--;
			if(pst_Track->pst_AllEvents[pst_ListParam->uw_CurrentEvent].uw_NumFrames == 0)
			{
				if(pst_ListParam->uw_CurrentEvent != pst_Track->uw_NumEvents - 1)
					DeleteOneEvent(pst_Track, pst_ListParam->uw_CurrentEvent);
			}
			else if
				(
					fabs
					(
						EVE_FrameToTime(pst_Track->pst_AllEvents[pst_ListParam->uw_CurrentEvent].uw_NumFrames) -
							pst_ListParam->f_Time
					) < 10e-7
				)
			{
				if(pst_ListParam->uw_CurrentEvent != pst_Track->uw_NumEvents - 1)
				{
					pst_ListParam->f_Time = 0.0f;
					pst_ListParam->uw_CurrentEvent++;
				}
			}
		}

		po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i));
		po_Track->SetTrack(pst_Track);
		po_Track->Invalidate();
	}

	MoveRunningBox(NULL, 0, TRUE);
	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EEVE_cl_Frame::AddFrameAll(EVE_tdst_Track *_pst_Ref, int _i_Num, BOOL _b_End)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT				uw_Current;
	EVE_tdst_Track		*pst_Track;
	EVE_tdst_Event		*pst_Event;
	EEVE_cl_Track		*po_Track;
	EDIA_cl_NameDialog	o_Dialog("Enter number of frames");
	char				asz_Name[200];
	float				f_Delay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Num)
		sprintf(asz_Name, "%d", _i_Num);
	else
		sprintf(asz_Name, "%d", 1);
	o_Dialog.mo_Name = asz_Name;
	if(_i_Num || o_Dialog.DoModal() == IDOK)
	{
		f_Delay = (float) L_atof((char *) (LPCSTR) o_Dialog.mo_Name);
		_i_Num = (int) f_Delay;
		f_Delay *= 1.0f / 60.0f;
		for(int i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
		{
			uw_Current = mpst_Data->pst_ListParam[i].uw_CurrentEvent;
			pst_Track = &mpst_ListTracks->pst_AllTracks[i];
			pst_Event = &pst_Track->pst_AllEvents[uw_Current];
			if(_pst_Ref && _pst_Ref == pst_Track) continue;
			if(fabs(mpst_Data->pst_ListParam[i].f_Time) < 10e-7)
			{
				if(uw_Current == 0)
					pst_Event = &pst_Track->pst_AllEvents[0];
				else
					pst_Event--;
			}

			/*
			 * If _b_End is Set, we want to add the extra frame at the End of the last
			 * non-zero delay event
			 */
			if((_b_End) && (pst_Track->uw_NumEvents >= 2))
			{
				pst_Event = &pst_Track->pst_AllEvents[pst_Track->uw_NumEvents - 2];
			}

			pst_Event->uw_NumFrames += EVE_TimeToFrame(f_Delay);
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i));
			po_Track->SetTrack(pst_Track);
			po_Track->Invalidate();
		}

		Align(0);
		LINK_Refresh();
	}

	return _i_Num;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::DuplicateFrameAll(EVE_tdst_Track *_pst_Ref)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT			uw_Current;
	EVE_tdst_Track	*pst_Track;
	EVE_tdst_Event	*pst_Event;
	EEVE_cl_Track	*po_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for(int i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
	{
		uw_Current = mpst_Data->pst_ListParam[i].uw_CurrentEvent;
		pst_Track = &mpst_ListTracks->pst_AllTracks[i];
		if(_pst_Ref && _pst_Ref != pst_Track) continue;
		pst_Event = &pst_Track->pst_AllEvents[uw_Current];

		if(fabs(mpst_Data->pst_ListParam[i].f_Time) < 10e-7)
		{
			CopyOneEvent(pst_Track, uw_Current, pst_Event, TRUE);
			pst_Event = &pst_Track->pst_AllEvents[uw_Current];
			pst_Event->uw_NumFrames = 1;
		}
		else
		{
			pst_Event->uw_NumFrames += 1;
		}

		po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i));
		po_Track->SetTrack(pst_Track);
		po_Track->Invalidate();
	}

	MoveRunningBox(NULL, 1.0f / 60.f, TRUE);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::RecomputeTrack(OBJ_tdst_GameObject *_pst_Main, EVE_tdst_Track *pst_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Params *pst_ListParam;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ListParam = mpst_Data->pst_ListParam + (pst_Track - mpst_Data->pst_ListTracks->pst_AllTracks);

	for(i = 0; i < pst_Track->uw_NumEvents; i++)
	{
		if((!(pst_Track->pst_AllEvents[i].w_Flags & EVE_C_EventFlag_Selected)) && (i != pst_ListParam->uw_CurrentEvent))
			continue;
		if
		(
				(fabs(pst_ListParam->f_Time) < 10e-7)
			||	(i != pst_ListParam->uw_CurrentEvent)
			||	(EVE_w_Event_InterpolationKey_GetType(&pst_Track->pst_AllEvents[i]) & EVE_InterKeyType_BlockedForIK)
		)
		{
			EVE_Event_InterpolationKey_Compute
			(
				pst_Track->pst_GO ? pst_Track->pst_GO : mpst_GAO,
				mpst_GAO,
				mpst_ListTracks,
				pst_Track,
				pst_Track->pst_AllEvents + i,
				EVE_w_Event_InterpolationKey_GetType(&pst_Track->pst_AllEvents[i]),
                0
			);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Recompute this Gizmo' 2 Tracks in the Main GO List of Tracks.
 =======================================================================================================================
 */
void EEVE_cl_Frame::Recompute(OBJ_tdst_GameObject *_pst_Main, OBJ_tdst_GameObject *_pst_Gizmo, BOOL _b_Magic)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *ago[100], *pst_Child;
	int					numago;
	EVE_tdst_Track		*pst_Track, *pst_TrackChild;
	EVE_tdst_Params		*pst_ListParamMagic;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_Magic)
	{
		/* LINK_gb_AllRefreshEnable = FALSE; */
		if(mpst_GAO != _pst_Main)
		{
			LINK_PrintStatusMsg("Code 467");
			return;
		}

		pst_Track = EVE_pst_GizmoToTrack(mpst_ListTracks, _pst_Gizmo);

		pst_ListParamMagic = mpst_Data->pst_ListParam + (pst_Track - mpst_Data->pst_ListTracks->pst_AllTracks);

		/* We must be ON A KEY and not on a single frame. */
		if(fabs(pst_ListParamMagic->f_Time) > 10e-7)
		{
			LINK_gb_AllRefreshEnable = TRUE;
			return;
		}

		RecomputeTrack(_pst_Main, pst_Track);
		if (pst_Track[1].uw_Gizmo == pst_Track[0].uw_Gizmo)
			RecomputeTrack(_pst_Main, pst_Track + 1);

		numago = WOR_i_GetAllChildsOf(WOR_World_GetWorldOfObject(_pst_Main), _pst_Gizmo, ago, TRUE);
		for(; numago; numago--)
		{
			pst_Child = ago[numago - 1];

			/* Wants only the Gizmo Objects. */
			if(!(pst_Child->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)) continue;

			if(pst_Child->pst_Base->pst_Hierarchy->pst_Father == pst_Child->pst_Base->pst_Hierarchy->pst_FatherInit)
			{
				/*
				 * We have updated the position of the Father of this Gizmo GO. Its local Matrix
				 * is not correct. We recompute it based on its Global Matrix.
				 */
				OBJ_ComputeLocalWhenHie(pst_Child);
				pst_TrackChild = EVE_pst_GizmoToTrack(mpst_ListTracks, pst_Child);

				/*
				 * RecomputeTrack(_pst_Main, pst_TrackChild); RecomputeTrack(_pst_Main,
				 * pst_TrackChild + 1);
				 */
			}
		}

		/* LINK_gb_AllRefreshEnable = TRUE; LINK_Refresh(); */
	}
	else
	{
		pst_Track = mpst_ListTracks->pst_AllTracks;
		for(int i = 0; i < mpst_ListTracks->uw_NumTracks; i++, pst_Track++)
		{
			RecomputeTrack(_pst_Main, pst_Track);
		}

		LINK_Refresh();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::Expand(int _i_Num)
{
	/*~~~~~*/
	int i, j;
	/*~~~~~*/

	for(i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
	{
		for(j = 0; j < mpst_ListTracks->pst_AllTracks[i].uw_NumEvents - 1; j++)
			mpst_ListTracks->pst_AllTracks[i].pst_AllEvents[j].uw_NumFrames *= _i_Num;
	}

	ForceRefresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SelectAllBones(OBJ_tdst_GameObject *_pst_Main, OBJ_tdst_GameObject *_pst_Gizmo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i, j;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	OBJ_tdst_GameObject		*ago[100], *pst_BoneGO;
	SEL_tdst_SelectedItem	*pst_Sel;
	int						numago;
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	EOUT_cl_Frame			*po_Out;
	EVE_tdst_Track			*pst_Track;
	EEVE_cl_Track			*po_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_GAO) return;
	if(!mpst_ListTracks) return;
	if(!mpst_Data) return;

	/* We unselect the current selection (double click on an invalid Gizmo) */
	for(j = 0; j < 4; j++)
	{
		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, j);
		if(po_Out)
		{
			pst_Sel = po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
			po_Out->mpo_EngineFrame->mpo_DisplayView->Selection_Unselect(pst_Sel->p_Content, SEL_C_SIF_Object);
		}
	}

	if(_pst_Main == _pst_Gizmo)
	{
		for(j = 0; j < 4; j++)
		{
			po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, j);
			if(po_Out) po_Out->mpo_EngineFrame->mpo_DisplayView->ForceSelectObject(_pst_Main, FALSE);
			pst_Track = EVE_pst_GizmoToTrack(mpst_ListTracks, _pst_Main);
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
			po_Track->Select();
			pst_Track++;
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
			po_Track->Select();
		}
	}

	/* For bassin */
	numago = WOR_i_GetAllChildsOf(WOR_World_GetWorldOfObject(_pst_Main), _pst_Main, ago, TRUE);
	if(numago)
	{
		for(i = 0; i < numago; i++)
		{
			if(!(ago[i]->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)) continue;
			if(!ANI_b_GizmoHasTrack(ago[i])) continue;
			for(j = 0; j < 4; j++)
			{
				po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, j);
				if(po_Out) po_Out->mpo_EngineFrame->mpo_DisplayView->ForceSelectObject(ago[i], FALSE);
				pst_Track = EVE_pst_GizmoToTrack(mpst_ListTracks, ago[i]);
				po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
				po_Track->Select();
				pst_Track++;
				po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
				po_Track->Select();
			}
		}
	}

	/* All bones */
	pst_GOAnim = _pst_Main->pst_Base->pst_GameObjectAnim;
	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_GOAnim->pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_GOAnim->pst_Skeleton->pst_AllObjects);
	for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO)) continue;
		numago = WOR_i_GetAllChildsOf(WOR_World_GetWorldOfObject(pst_BoneGO), pst_BoneGO, ago, TRUE);
		if(!numago) continue;
		for(i = 0; i < numago; i++)
		{
			if(!(ago[i]->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)) continue;
			if(!ANI_b_GizmoHasTrack(ago[i])) continue;
			for(j = 0; j < 4; j++)
			{
				/*~~~~~~~~~~~~~*/
				USHORT	uw_Gizmo;
				/*~~~~~~~~~~~~~*/

				po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, j);
				if(po_Out) po_Out->mpo_EngineFrame->mpo_DisplayView->ForceSelectObject(ago[i], FALSE);
				pst_Track = EVE_pst_GizmoToTrack(mpst_ListTracks, ago[i]);

				uw_Gizmo = pst_Track->uw_Gizmo;

				po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
				po_Track->Select();

				/*
				 * In classical Animation cases, the rotation track follows the Translation one.
				 * If we select one bone, both tracks must be selected. We check that the
				 * following track deals with the same gizmo.
				 */
				pst_Track++;

				if
				(
					(pst_Track - mpst_ListTracks->pst_AllTracks <= mpst_ListTracks->uw_NumTracks)
				&&	(pst_Track->uw_Gizmo == uw_Gizmo)
				)
				{
					po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
					po_Track->Select();
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::AddRemoveIK(OBJ_tdst_GameObject *_pst_Main, OBJ_tdst_GameObject *_pst_Gizmo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	EVE_tdst_Params *pst_ListParam;
	int				i;
	USHORT			w_Type;
	EEVE_cl_Track	*po_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpst_GAO != _pst_Main) return;
	pst_Track = EVE_pst_GizmoToTrack(mpst_ListTracks, _pst_Gizmo);
	if(!pst_Track) return;

	pst_ListParam = mpst_Data->pst_ListParam + (pst_Track - mpst_Data->pst_ListTracks->pst_AllTracks);

	/* Rotation */
	for(i = 0; i < pst_Track->uw_NumEvents; i++)
	{
		if((!(pst_Track->pst_AllEvents[i].w_Flags & EVE_C_EventFlag_Selected)) && (i != pst_ListParam->uw_CurrentEvent))
			continue;
		if(fabs(pst_ListParam->f_Time) < 10e-7 || (i != pst_ListParam->uw_CurrentEvent))
		{
			OBJ_ComputeGlobalWhenHie(_pst_Gizmo);

			w_Type = EVE_w_Event_InterpolationKey_GetType(&pst_Track->pst_AllEvents[i]);
			if(w_Type & EVE_InterKeyType_BlockedForIK)
			{
				_pst_Gizmo->pst_Base->pst_Hierarchy->pst_Father = _pst_Gizmo->pst_Base->pst_Hierarchy->pst_FatherInit;
				w_Type -= EVE_InterKeyType_BlockedForIK;
			}
			else
			{
				_pst_Gizmo->pst_Base->pst_Hierarchy->pst_Father = _pst_Main;
				w_Type += EVE_InterKeyType_BlockedForIK;
			}

			OBJ_ComputeLocalWhenHie(_pst_Gizmo);
			EVE_Event_InterpolationKey_ChangeTypeWithoutChangingData(&pst_Track->pst_AllEvents[i], w_Type);
		}
	}

	RecomputeTrack(_pst_Main, pst_Track);

	/* Translation */
	pst_Track++;
	pst_ListParam++;
	for(i = 0; i < pst_Track->uw_NumEvents; i++)
	{
		if((!(pst_Track->pst_AllEvents[i].w_Flags & EVE_C_EventFlag_Selected)) && (i != pst_ListParam->uw_CurrentEvent))
			continue;
		if(fabs(pst_ListParam->f_Time) < 10e-7 || (i != pst_ListParam->uw_CurrentEvent))
		{
			OBJ_ComputeGlobalWhenHie(_pst_Gizmo);

			w_Type = EVE_w_Event_InterpolationKey_GetType(&pst_Track->pst_AllEvents[i]);
			if(w_Type & EVE_InterKeyType_BlockedForIK)
			{
				_pst_Gizmo->pst_Base->pst_Hierarchy->pst_Father = _pst_Gizmo->pst_Base->pst_Hierarchy->pst_FatherInit;
				w_Type -= EVE_InterKeyType_BlockedForIK;
			}
			else
			{
				_pst_Gizmo->pst_Base->pst_Hierarchy->pst_Father = _pst_Main;
				w_Type += EVE_InterKeyType_BlockedForIK;
			}

			OBJ_ComputeLocalWhenHie(_pst_Gizmo);
			EVE_Event_InterpolationKey_ChangeTypeWithoutChangingData(&pst_Track->pst_AllEvents[i], w_Type);
		}
	}

	RecomputeTrack(_pst_Main, pst_Track);

	po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
	po_Track->Invalidate();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SwitchTrans(OBJ_tdst_GameObject *_pst_Main, OBJ_tdst_GameObject *_pst_Gizmo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	EVE_tdst_Params *pst_ListParam;
	int				i;
	USHORT			w_Type;
	EEVE_cl_Track	*po_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpst_GAO != _pst_Main) return;
	pst_Track = EVE_pst_GizmoToTrack(mpst_ListTracks, _pst_Gizmo);
	if(!pst_Track) return;

	pst_ListParam = mpst_Data->pst_ListParam + (pst_Track - mpst_Data->pst_ListTracks->pst_AllTracks);

	/* Translation */
	pst_Track++;
	pst_ListParam++;
	for(i = 0; i < pst_Track->uw_NumEvents; i++)
	{
		if((!(pst_Track->pst_AllEvents[i].w_Flags & EVE_C_EventFlag_Selected)) && (i != pst_ListParam->uw_CurrentEvent))
			continue;
		if(fabs(pst_ListParam->f_Time) < 10e-7 || (i != pst_ListParam->uw_CurrentEvent))
		{
			w_Type = EVE_w_Event_InterpolationKey_GetType(&pst_Track->pst_AllEvents[i]);
			if((w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_0)
			{
				w_Type &= ~EVE_InterKeyType_Translation_0;
				w_Type |= EVE_InterKeyType_Translation_1;
			}
			else if((w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_1)
			{
				w_Type &= ~EVE_InterKeyType_Translation_1;
				w_Type |= EVE_InterKeyType_Translation_2;
			}
			else
			{
				w_Type &= ~EVE_InterKeyType_Translation_2;
				w_Type |= EVE_InterKeyType_Translation_0;
			}

			EVE_Event_InterpolationKey_ChangeTypeWithoutChangingData(&pst_Track->pst_AllEvents[i], w_Type);
		}
	}

	po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
	po_Track->Invalidate();
	LINK_Refresh();
}

#endif /* ACTIVE_EDITORS */
