/*$T EVEframe.cpp GC 1.139 03/12/04 18:36:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#define ACTION_GLOBAL
#include "EVEframe.h"
#include "EVEtrack.h"
#include "EVEevent.h"
#include "EVEscroll.h"
#include "EVEinside.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "AIinterp/Sources/Events/EVEnt_morphkey.h"
#include "AIinterp/Sources/Events/EVEnt_timekey.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIACOLOR_dlg.h"
#include "EDIpaths.h"
#include "EDIeditors_infos.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "EDIstrings.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/ENGcall.h"
#include "OUTput/OUTframe.h"
#include "OUTput/OUTmsg.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDload.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "AIinterp/Sources/AIengine.h"
#include "DIAlogs/DIAlist2_dlg.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "ENGine/Sources/TEXT/TEXT.h"

#ifdef JADEFUSION
extern BOOL					EVE_gb_NeedToReparse;
extern OBJ_tdst_GameObject	*EVE_gpst_OwnerGAO;
extern EVE_tdst_Data		*EVE_gpst_CurrentData;
extern EVE_tdst_ListTracks	*EVE_gpst_CurrentListTracks;
extern BOOL					EVE_gb_CanFlash;
extern BOOL					EVE_gb_NeedToRecomputeSND;
extern BOOL					EVE_gb_UseRunningPauseFlag;
#else
extern "C" BOOL					EVE_gb_NeedToReparse;
extern "C" OBJ_tdst_GameObject	*EVE_gpst_OwnerGAO;
extern "C" EVE_tdst_Data		*EVE_gpst_CurrentData;
extern "C" EVE_tdst_ListTracks	*EVE_gpst_CurrentListTracks;
extern "C" BOOL					EVE_gb_CanFlash;
extern "C" BOOL					EVE_gb_NeedToRecomputeSND;
extern "C" BOOL					EVE_gb_UseRunningPauseFlag;
#endif

BOOL							EVE_gb_CanCloseUndo = TRUE;

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/* For copy */
CList<EEVE_tdst_CopyEvent *, EEVE_tdst_CopyEvent *> EEVE_go_ListCopyEvents;
CList<EVE_tdst_Track *, EVE_tdst_Track *>			EEVE_go_ListCopyTracks;
int													EEVE_gi_CopyMode = EEVE_C_CopyAll;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EEVE_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EEVE_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
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
EEVE_cl_Frame::EEVE_cl_Frame(void)
{
	mpo_MainSplitter = new CSplitterWnd;
	mpo_VarsView = new EVAV_cl_View;
	mpst_ListTracks = NULL;
	mpst_Data = NULL;
	mpst_GAO = NULL;
	mul_GAO = BIG_C_InvalidIndex;
	mul_ListTracks = BIG_C_InvalidIndex;
	mpo_LastSelected = NULL;
	mb_Play = FALSE;
	mi_NumCycles = 1;
	mi_PlayContract = 0;
	mb_Persist = TRUE;
	mb_LockUpdate = FALSE;
	mb_AnimMode = FALSE;
	L_memset(&mt_Tre, 0, sizeof(mt_Tre));
	mo_MorphEditor = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Frame::~EEVE_cl_Frame(void)
{
	CloseMorphEditor();
	mpo_MainSplitter->DestroyWindow();
	delete mpo_MainSplitter;
	delete mpo_VarsView;
	mo_Fnt.DeleteObject();
	if(mpo_Original == NULL) CloseCopy();
	CloseUndo();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void RefreshTracks(void *, void *, void *, LONG)
{
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EEVE_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CCreateContext	o_Context;
	LOGFONT			*plf;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1) return -1;

	/* Splitter wnd */
	mpo_MainSplitter->CreateStatic(this, 1, 2);

	/* Create small font */
	plf = (LOGFONT *) LocalAlloc(LPTR, sizeof(LOGFONT));
	lstrcpy(plf->lfFaceName, EDI_STR_Csz_Font);
	plf->lfHeight = 11;
	plf->lfWeight = 500;
	plf->lfEscapement = 0;
	mo_Fnt.CreateFontIndirect(plf);
	LocalFree((LOCALHANDLE) plf);

	/* Create scroll view */
	mpo_VarsView->mb_CanDragDrop = FALSE;
	mst_VarsViewStruct.po_ListItems = &mo_ListItems;
	mst_VarsViewStruct.psz_NameCol1 = "Name";
	mst_VarsViewStruct.i_WidthCol1 = 100;
	mst_VarsViewStruct.psz_NameCol2 = "Value";
	mpo_VarsView->MyCreate(mpo_MainSplitter, &mst_VarsViewStruct, this, mpo_MainSplitter->IdFromRowCol(0, 0));
	mpo_VarsView->mpo_ListBox->mpfnv_Callback = RefreshTracks;

	mpo_MainSplitter->CreateView(0, 1, RUNTIME_CLASS(EEVE_cl_Inside), CSize(0, 0), NULL);
	((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1))->mpo_Parent = this;
	((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1))->SetScrollSizes(MM_TEXT, CSize(50, 50));
	((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1))->mpo_Scroll->mpo_Parent = this;

	mpo_MainSplitter->RecalcLayout();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::WhenChanged(void)
{
	if(!mpst_GAO) return;
	LINK_UpdatePointer(mpst_GAO);
	mb_LockUpdate = TRUE;
	LINK_UpdatePointers();
	mb_LockUpdate = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::OnNewTrack(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_GAO) return;
	if(!mpst_Data || mpst_Data->pst_ListTracks != mpst_ListTracks) SetGAO(mpst_GAO);
	pst_Track = EVE_pst_NewTrack(mpst_Data);
	pst_Track->uw_Flags &= ~EVE_C_Track_AutoLoop;
	pst_Track->uw_Flags |= EVE_C_Track_AutoStop;
	AddTrack(pst_Track);
	ForceRefresh();
	WhenChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::OnDeleteTrack(EEVE_cl_Track *_po_ToDel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	int				index;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpst_Data->pst_ListTracks != mpst_ListTracks) SetGAO(mpst_GAO);
	index = 0;
	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		if(po_Track == _po_ToDel)
		{
			EVE_DeleteTrack(mpst_Data, index);
			_po_ToDel->CloseTrack();
			_po_ToDel->DestroyWindow();
			delete _po_ToDel;
			pos = mo_ListTracks.Find(_po_ToDel);
			mo_ListTracks.RemoveAt(pos);
			break;
		}

		index++;
	}

	ForceRefresh();
	WhenChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::OnRunAll(BOOL _b_Run, BOOL _b_Init, BOOL _b_Sel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Params *pst_Param;
	int				index;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for(index = 0; index < mpst_Data->pst_ListTracks->uw_NumTracks; index++)
	{
		pst_Param = &mpst_Data->pst_ListParam[index];
		if(_b_Sel && !(mpst_Data->pst_ListTracks->pst_AllTracks[index].uw_Flags & EVE_C_Track_Selected)) continue;

		if(!_b_Init)
			EVE_SetRunningTrack(pst_Param, _b_Run);
		else
		{
			if(_b_Run)
				mpst_Data->pst_ListTracks->pst_AllTracks[index].uw_Flags |= EVE_C_Track_RunningInit;
			else
				mpst_Data->pst_ListTracks->pst_AllTracks[index].uw_Flags &= ~EVE_C_Track_RunningInit;
		}
	}

	ForceRefresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EEVE_cl_Frame::EventToIndex(EEVE_cl_Track *_po_Track, EEVE_cl_Event *_po_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Event	*po_Tst;
	int				i_Cnt;
	int				index;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(_po_Event == NULL) return -1;
	i_Cnt = 0;
	for(index = 0; index < _po_Track->mo_ListEvents.GetSize(); index++)
	{
		po_Tst = _po_Track->mo_ListEvents.GetAt(index);
		if(po_Tst == _po_Event) return i_Cnt;
		i_Cnt++;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SetInter(int _i_Inter)
{
	/*~~~~~~~~~~~~~*/
	CRect	o_Rect;
	int		i_Rap1;
	CPoint	o_Scroll;
	/*~~~~~~~~~~~~~*/

	GetClientRect(&o_Rect);

	i_Rap1 = ((CScrollView *) mpo_MainSplitter->GetPane(0, 1))->GetScrollPosition().x;
	i_Rap1 /= mst_Ini.i_Inter;
	mst_Ini.i_Inter = _i_Inter;

	o_Scroll = ((CScrollView *) mpo_MainSplitter->GetPane(0, 1))->GetScrollPosition();
	o_Scroll.x = (i_Rap1 + 1) * mst_Ini.i_Inter;

	SetFactor(TRUE, FALSE);
	((CScrollView *) mpo_MainSplitter->GetPane(0, 1))->ScrollToPosition(o_Scroll);
}

BOOL	EEVE_Refresh = TRUE;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SetFactor(BOOL _b_ComputeInter, BOOL _b_SetInter)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	int				index;
	float			f_InterPerSeconds;
	CRect			o_Rect;
	int				i_YCur, i_Temp, A, B;
	float			f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_InterPerSeconds = mst_Ini.f_DivFactor / mst_Ini.f_ValFactor;
	M_MF()->SendMessageToLinks(this, EOUT_MESSAGE_CURVE_CHANGEINTERPERSECONDS, *(int *) &f_InterPerSeconds, 0);

	if(_b_ComputeInter)
	{
		GetClientRect(&o_Rect);
		mpo_MainSplitter->GetColumnInfo(0, i_YCur, i_Temp);
		mst_Ini.i_SplitterWidth = i_YCur;
		o_Rect.left += mst_Ini.i_SplitterWidth;
		o_Rect.left += mst_Ini.i_XLeft;

		if(_b_SetInter) mst_Ini.i_Inter = o_Rect.Width() / (int) (mst_Ini.f_DivFactor + 0.5f);
	}

	if(mst_Ini.i_Inter < EEVE_C_CXBorder + 2) mst_Ini.i_Inter = EEVE_C_CXBorder + 2;
	if(mst_Ini.i_Inter > 400) mst_Ini.i_Inter = 400;

	f = mst_Ini.f_ValFactor / mst_Ini.f_DivFactor;
	f /= mst_Ini.i_Inter;
	f = MATH_f_FloatModulo(f, 1.0f / 60.0f);
	if(f == 0) f = 1.0f / 60.0f;
	A = B = (int) ((mst_Ini.f_ValFactor / mst_Ini.f_DivFactor) / f);
	if(A)
	{
		while(A + B <= mst_Ini.i_Inter) A += B;
		mst_Ini.i_Inter = A;
	}

	/* Recompute factor depending on div and val */
	mst_Ini.f_Factor = mst_Ini.f_DivFactor * mst_Ini.i_Inter;

	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		for(index = 0; index < po_Track->mo_ListEvents.GetSize(); index++)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(index);
			po_Event->RecomputeWidth();
		}

		po_Track->Invalidate();
	}

	mpo_MainSplitter->GetPane(0, 1)->Invalidate();
	ForceRefresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::DeleteOneEvent(EVE_tdst_Track *_pst_Track, int index, BOOL _b_Keep)
{
	/*~~~~~~~~~~~~~~*/
	USHORT	uw_Frames;
	/*~~~~~~~~~~~~~~*/

	if(!_pst_Track->uw_NumEvents) return;

	uw_Frames = _pst_Track->pst_AllEvents[index].uw_NumFrames;
	EVE_DeleteEvent(_pst_Track, index);
	if(_pst_Track->uw_NumEvents && _b_Keep)
	{
		if(index)
			_pst_Track->pst_AllEvents[index - 1].uw_NumFrames += uw_Frames;
		else if(index < _pst_Track->uw_NumEvents - 1)
			_pst_Track->pst_AllEvents[index].uw_NumFrames += uw_Frames;
	}

	if(!(_pst_Track->uw_NumEvents)) return;
	if((_pst_Track->pst_AllEvents->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) return;
	if(!OBJ_b_TestIdentityFlag(_pst_Track->pst_GO ? _pst_Track->pst_GO : mpst_GAO, OBJ_C_IdentityFlag_Anims)) return;

	EVE_Event_InterpolationKey_UpdateNextValue
	(
		_pst_Track->pst_GO ? _pst_Track->pst_GO : mpst_GAO,
		mpst_ListTracks,
		_pst_Track	/* ((index) ? index - 1 : 0) */
	);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SplitSelected(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	int				index;
	EVE_tdst_Params *pst_Param;
	int				track1;
	BOOL			b_Del;
	float			f_Width, f_Dif;
	CPoint			pt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SaveForUndo();
	track1 = 0;
	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && !mst_Ini.i_DisplayHidden)
		{
			track1++;
			continue;
		}

		b_Del = FALSE;
		pst_Param = &mpst_Data->pst_ListParam[track1];
		for(index = po_Track->mo_ListEvents.GetSize() - 1; index >= 0; index--)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(index);
			if(po_Event->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)
			{
				/* Compute delay */
				SplitComputeWidth
				(
					po_Track->mpst_Track,
					pst_Param,
					po_Event->mi_NumEvent,
					pt,
					po_Event,
					&f_Width,
					&f_Dif,
					TRUE
				);
				SplitOneEvent(po_Track->mpst_Track, pst_Param, po_Event->mi_NumEvent, f_Width, f_Dif);
				b_Del = TRUE;
			}
		}

		if(b_Del)
		{
			po_Track->SetTrack(po_Track->mpst_Track);
			po_Track->Invalidate();
		}

		track1++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::DeleteSelected(BOOL _b_Keep)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	int				index;
	BOOL			b_Del;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	Copy();
	SaveForUndo();

	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && !mst_Ini.i_DisplayHidden) continue;
		b_Del = FALSE;
		for(index = po_Track->mo_ListEvents.GetSize() - 1; index >= 0; index--)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(index);
			if(po_Event->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)
			{
				DeleteOneEvent(po_Track->mpst_Track, index, _b_Keep);
				b_Del = TRUE;
			}
		}

		if(b_Del)
		{
			po_Track->SetTrack(po_Track->mpst_Track);
			po_Track->Invalidate();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SetDelaySelected(float _f_Delay)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	int				index;
	BOOL			b_Del;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && !mst_Ini.i_DisplayHidden) continue;
		b_Del = FALSE;
		for(index = po_Track->mo_ListEvents.GetSize() - 1; index >= 0; index--)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(index);
			if(po_Event->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)
			{
				b_Del = TRUE;
				po_Event->pst_FindEngineEvent()->uw_NumFrames = EVE_TimeToFrame(_f_Delay);
				po_Event->RecomputeWidth();
			}
		}

		if(b_Del)
		{
			po_Track->SetTrack(po_Track->mpst_Track);
			po_Track->Invalidate();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::AlignEventLeft(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	EEVE_cl_Event	*po_First;
	int				index;
	int				framefirst;
	int				framecur;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get first selected event */
	po_First = NULL;
	pos = mo_ListTracks.GetHeadPosition();
	while(pos && !po_First)
	{
		framefirst = 0;
		po_Track = mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && !mst_Ini.i_DisplayHidden) continue;
		for(index = 0; index < po_Track->mo_ListEvents.GetSize(); index++)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(index);
			if(po_Event->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)
			{
				po_First = po_Event;
				break;
			}
			else
			{
				framefirst += po_Event->pst_FindEngineEvent()->uw_NumFrames;
			}
		}
	}

	if(!po_First) return;

	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		framecur = 0;
		po_Track = mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && !mst_Ini.i_DisplayHidden) continue;
		for(index = 0; index < po_Track->mo_ListEvents.GetSize(); index++)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(index);
			if(po_Event->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)
			{
				if(po_Event == po_First) break;
				if(!index) break;
				po_Event = po_Track->mo_ListEvents.GetAt(index - 1);
				framecur = framefirst - framecur;
				if(!framecur) break;
				if(po_Event->pst_FindEngineEvent()->uw_NumFrames + framecur <= 0) break;

				po_Event->pst_FindEngineEvent()->uw_NumFrames += framecur;
				po_Event->RecomputeWidth();

				po_Event = po_Track->mo_ListEvents.GetAt(index);
				po_Event->pst_FindEngineEvent()->uw_NumFrames -= framecur;
				po_Event->RecomputeWidth();

				po_Track->SetTrack(po_Track->mpst_Track);
				po_Track->Invalidate();
				break;
			}
			else
			{
				framecur += po_Event->pst_FindEngineEvent()->uw_NumFrames;
			}
		}
	}
}

void EVE_SetFlashMatrix(EVE_tdst_Event *_pst_Event,OBJ_tdst_GameObject *_pst_GO)
{
    _pst_Event->w_Flags |= EVE_C_EventFlag_Flash;
    OBJ_SetFlashMatrix(_pst_GO,OBJ_pst_GetAbsoluteMatrix(_pst_GO));
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::ChooseDialogForSoundPlayer(EEVE_cl_Track *_po_Track, EEVE_cl_Event *_po_Event, EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_DialogFile("Choose dialog file", 0, 0, 1, NULL, "*" EDI_Csz_ExtSoundDialog);
	CString				o;
	int					i_CurSoundIndex = -1;
	unsigned long		ul_DialogFileKey, ul_DialogFileIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Let user choose dialog file associated to the event
	ul_DialogFileKey = BIG_C_InvalidKey;
	if(o_DialogFile.DoModal() == IDOK) {
		o_DialogFile.GetItem(o_DialogFile.mo_File, 0, o);
		ul_DialogFileIndex = BIG_ul_SearchFileExt(o_DialogFile.masz_FullPath, (char *) (LPCSTR) o);
		if(ul_DialogFileIndex != BIG_C_InvalidIndex) ul_DialogFileKey = BIG_FileKey(ul_DialogFileIndex);
	}

	// Set length of event to the duration of the sound
	if(SND_gst_Params.l_Available) {
		i_CurSoundIndex = SND_l_GetSoundIndex(ul_DialogFileKey);
		if (i_CurSoundIndex == -1) {           // Sound is not loaded
			i_CurSoundIndex = SND_l_AddSound(ul_DialogFileKey);
		}
	}
	_po_Event->SetIntParameter(0, i_CurSoundIndex);
	ResetSizeOfSoundPlayer(_po_Track, _po_Event);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::ChooseDialogForEvent(EEVE_cl_Track *_po_Track, EEVE_cl_Event *_po_Event, EVE_tdst_Event *_pst_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_List2Dialog		o_Dlg(LIST2_ModeText);
	TEXT_tdst_Eval			st_Dialog;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st_Dialog.i_FileKey = -1;
	st_Dialog.i_Id = -1;
    o_Dlg.mp_TextRef = &st_Dialog;
	o_Dlg.DoModal();

	_po_Event->SetIntParameter(0, st_Dialog.i_FileKey);
	_po_Event->SetIntParameter(1, st_Dialog.i_Id);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SetSizeOfSoundEvent(EEVE_cl_Track *_po_Track, EEVE_cl_Event *_po_Event, int _i_SndIdx)
{
	EVE_tdst_Event		*pst_Event;
	SND_tdst_OneSound	*pst_Sound;
	float				f_Duration = 1;

	pst_Event = _po_Event->pst_FindEngineEvent();

	// Set length of event to the duration of the sound
	if(SND_gst_Params.l_Available) {
		if((_i_SndIdx >= 0) && (_i_SndIdx < SND_gst_Params.l_SoundNumber)) {
			pst_Sound = SND_gst_Params.dst_Sound + _i_SndIdx;
			if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) && pst_Sound->pst_Wave) {
				f_Duration = (float) SND_ui_SizeToSample
				(
					pst_Sound->pst_Wave->wFormatTag,
					pst_Sound->pst_Wave->wChannels,
					pst_Sound->pst_Wave->ul_DataSize
				);
				f_Duration /= (float) pst_Sound->pst_Wave->dwSamplesPerSec;
			}
		}
	}
	pst_Event->uw_NumFrames = EVE_TimeToFrame(f_Duration);
	_po_Event->RecomputeWidth();
	_po_Track->SetTrack(_po_Track->mpst_Track);
	_po_Track->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::ResetSizeOfSoundPlayer(EEVE_cl_Track *_po_Track, EEVE_cl_Event *_po_Event)
{
	SetSizeOfSoundEvent ( _po_Track, _po_Event, _po_Event->i_GetIntParameter(0));
}

///*
// =======================================================================================================================
// =======================================================================================================================
// */
//void EEVE_cl_Frame::ResetSizeOfSoundPlayer(EEVE_cl_Track *_po_Track, EEVE_cl_Event *_po_Event, EVE_tdst_Event *_pst_Event)
//{
//	SND_tdst_OneSound	*pst_Sound;
//	float				f_Duration = 1;
//	int					i_CurSoundIndex = _po_Event->i_GetIntParameter(0);
//
//	// Set length of event to the duration of the sound
//	if(SND_gst_Params.l_Available) {
//		if((i_CurSoundIndex >= 0) && (i_CurSoundIndex < SND_gst_Params.l_SoundNumber)) {
//			pst_Sound = SND_gst_Params.dst_Sound + i_CurSoundIndex;
//			if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) && pst_Sound->pst_Wave) {
//				f_Duration = (float) SND_ui_SizeToSample
//				(
//					pst_Sound->pst_Wave->wFormatTag,
//					pst_Sound->pst_Wave->wChannels,
//					pst_Sound->pst_Wave->ul_DataSize
//				);
//				f_Duration /= (float) pst_Sound->pst_Wave->dwSamplesPerSec;
//			}
//		}
//	}
//	_pst_Event->uw_NumFrames = EVE_TimeToFrame(f_Duration);
//	_po_Event->RecomputeWidth();
//	_po_Track->SetTrack(_po_Track->mpst_Track);
//	_po_Track->Invalidate();
//}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::ResetSizeOfRequestPlayDialog(EEVE_cl_Track *_po_Track, EEVE_cl_Event *_po_Event)
{
	int				i_SndIdx;
	TEXT_tdst_Eval	st_Text;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_OneText	*pst_Txt;
    int id;
    extern LONG SND_l_GetSoundIndex(BIG_KEY _ul_WavKey);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st_Text.i_FileKey = _po_Event->i_GetIntParameter(0);
	st_Text.i_Id = _po_Event->i_GetIntParameter(1);

	if(st_Text.i_FileKey == BIG_C_InvalidIndex) return;
    
    id = TEXT_i_GetOneTextIndex(st_Text.i_FileKey);
	if(id < 0) return;

	pst_Txt = TEXT_gst_Global.pst_AllTexts[id];
	if(!pst_Txt) return;

    id = TEXT_i_GetEntryIndex(pst_Txt, st_Text.i_Id);
	if(id == -1) return;
		
    i_SndIdx =  SND_l_GetSoundIndex(pst_Txt->pst_Ids[id].ul_SoundKey);

	SetSizeOfSoundEvent ( _po_Track, _po_Event, i_SndIdx);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::PopupEvent(EEVE_cl_Track *_po_Track, EEVE_cl_Event *_po_Event)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu		o_Menu(FALSE);
	int					i_Res, index;
	CPoint				pt;
	EVE_tdst_Event		*pst_Event;
	EDIA_cl_NameDialog	o_Dialog("Enter Delay in ms");
	char				asz_Name[100];
	EEVE_cl_Event		*po_Prev;
	//int					indexf;
	CPoint				ptl;
	float				f_Delay;
	EDIA_cl_ColorDialog *po_Col;
    BOOL                bSetFlashMatrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&pt);
	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "New Event", -1);

	/* Empty event */
	M_MF()->AddPopupMenuAction(this, &o_Menu, 1, TRUE, "Empty", -1);
	//if(!_po_Event)
	//	M_MF()->AddPopupMenuAction(this, &o_Menu, 1, TRUE, "Empty", -1);
	//else if((_po_Event->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_Empty)
	//{
	//	for(indexf = 0; indexf < _po_Track->mo_ListEvents.GetSize(); indexf++)
	//		if(_po_Track->mo_ListEvents.GetAt(indexf) == _po_Event) break;
	//	po_Prev = NULL;
	//	if(indexf) po_Prev = _po_Track->mo_ListEvents.GetAt(indexf - 1);
	//	if(!po_Prev || ((po_Prev->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_Empty))
	//		M_MF()->AddPopupMenuAction(this, &o_Menu, 1, TRUE, "Empty", -1);
	//}

	/* Type of event */
	if(_po_Track->mpst_Track->uw_NumEvents == 0)
	{
		M_MF()->AddPopupMenuAction(this, &o_Menu, 2, TRUE, "AI Function", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 3, TRUE, "Translation Key", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 30, TRUE, "Translation Key (no Interpolation)", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 4, TRUE, "Translation Key (with flash)", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 31, TRUE, "Morph Key", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 5, TRUE, "Rotation Key", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 60, TRUE, "Rotation Key (no Interpolation)", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 6, TRUE, "Rotation Key (with flash)", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 7, TRUE, "Time key", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 9, TRUE, "Sound Track for Animators", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 500, TRUE, "Goto Label", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 501, TRUE, "Set Time To Label", -1);
	}
	else
	{
		pst_Event = _po_Track->mpst_Track->pst_AllEvents;
		while
		(
			((pst_Event->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_Empty)
		&&	(pst_Event < _po_Track->mpst_Track->pst_AllEvents + _po_Track->mpst_Track->uw_NumEvents)
		) pst_Event++;

		if((pst_Event->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_Empty)
		{
			M_MF()->AddPopupMenuAction(this, &o_Menu, 2, TRUE, "AI Function", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 3, TRUE, "Translation Key", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 30, TRUE, "Translation Key (no Interpolation)", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 4, TRUE, "Translation Key (with flash)", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 31, TRUE, "Morph Key", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 5, TRUE, "Rotation Key", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 60, TRUE, "Rotation Key (no Interpolation)", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 6, TRUE, "Rotation Key (with flash)", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 9, TRUE, "Sound Track for Animators", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 500, TRUE, "Goto Label", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 501, TRUE, "Set Time To Label", -1);
		}
		else
		{
			if((pst_Event->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
			{
				i_Res = EVE_w_Event_InterpolationKey_GetType(pst_Event);
				if(i_Res & EVE_InterKeyType_Translation_Mask)
				{
					M_MF()->AddPopupMenuAction(this, &o_Menu, 3, TRUE, "Translation Key", -1);
					M_MF()->AddPopupMenuAction(this, &o_Menu, 30, TRUE, "Translation Key (no Interpolation)", -1);
				}

				if(i_Res & EVE_InterKeyType_Rotation_Matrix)
				{
					M_MF()->AddPopupMenuAction(this, &o_Menu, 5, TRUE, "Rotation Key", -1);
					M_MF()->AddPopupMenuAction(this, &o_Menu, 60, TRUE, "Rotation Key (no Interpolation)", -1);
				}
			}
			else
			{
				if ((pst_Event->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_MorphKey)
					M_MF()->AddPopupMenuAction(this, &o_Menu, 31, TRUE, "Morph Key", -1);
				M_MF()->AddPopupMenuAction(this, &o_Menu, 2, TRUE, "AI Function", -1);
				M_MF()->AddPopupMenuAction(this, &o_Menu, 9, TRUE, "Sound Track for Animators", -1);
				M_MF()->AddPopupMenuAction(this, &o_Menu, 500, TRUE, "Goto Label", -1);
				M_MF()->AddPopupMenuAction(this, &o_Menu, 501, TRUE, "Set Time To Label", -1);
				M_MF()->AddPopupMenuAction(this, &o_Menu, 666, TRUE, "Set Color", -1);
			}
		}
	}

	/* Split */
	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "", -1);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 50, TRUE, "Split Key", -1);

	/* General */
	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "", -1);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 8, TRUE, "Delete Track", -1);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "", -1);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 103, TRUE, "Delete Events (No Shift)", -1);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 113, TRUE, "Delete Events (Shift Right)", -1);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "", -1);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 104, TRUE, "Copy", -1);
	if(_po_Event && (EEVE_go_ListCopyEvents.GetCount()))
		M_MF()->AddPopupMenuAction(this, &o_Menu, 105, TRUE, "Paste", -1);
	if(EEVE_go_ListCopyEvents.GetCount())
	{
		M_MF()->AddPopupMenuAction(this, &o_Menu, 107, TRUE, "Insert/Paste With Delay", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 108, TRUE, "Insert/Paste Overwrite", -1);
	}

	/* When event under mouse */
	if(_po_Event)
	{
		M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "", -1);
		pst_Event = _po_Event->pst_FindEngineEvent();
		M_MF()->AddPopupMenuAction(this, &o_Menu, 100, TRUE, "Set Delay", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 101, TRUE, "Set To Empty", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 667, TRUE, "Align Left", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "", -1);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 102, TRUE, "Edit", -1);
		if (_po_Event->b_TestAIFunctionType(FUNCTION_SND_SOUNDPLAYERFORANIMATORS)) {
			M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 150, TRUE, "Set dialog", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 151, TRUE, "Reset size", -1);
		}
		if (_po_Event->b_TestAIFunctionType(FUNCTION_SND_PREFETCHDIALOG)
			|| _po_Event->b_TestAIFunctionType(FUNCTION_SND_REQUESTPLAYDIALOG)) {
			M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 152, TRUE, "Set dialog", -1);
			M_MF()->AddPopupMenuAction(this, &o_Menu, 153, TRUE, "Reset size", -1);
		}
	}

	GetCursorPos(&ptl);
	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	if(i_Res <= 0) return;

    // If event is inserted at index 0 before an event with a flash matrix, it has a flash matrix. 
	switch(i_Res)
	{

	/*$2--------------------------------------------------------------------------------------------------------------*/

	/* New Empty event */
	case 1:
		SaveForUndo();
		index = EventToIndex(_po_Track, _po_Event);
        bSetFlashMatrix = 
            (index == 0 && 
            _po_Track->mpst_Track->uw_NumEvents > 0 && 
            _po_Track->mpst_Track->pst_AllEvents[0].w_Flags & EVE_C_EventFlag_Flash);

		pst_Event = EVE_pst_NewEvent(_po_Track->mpst_Track, index, EVE_C_EventFlag_Empty);
        if (bSetFlashMatrix)
            EVE_SetFlashMatrix(pst_Event,_po_Track->mpo_Frame->mpst_GAO);

		pst_Event->uw_NumFrames = EVE_TimeToFrame(mst_Ini.f_ValFactor / mst_Ini.f_DivFactor);
		break;

	/* New AI function event */
	case 2:
		SaveForUndo();
		index = EventToIndex(_po_Track, _po_Event);
		pst_Event = EVE_pst_NewEvent(_po_Track->mpst_Track, index, EVE_C_EventFlag_AIFunction);
		pst_Event->uw_NumFrames = EVE_TimeToFrame(mst_Ini.f_ValFactor / mst_Ini.f_DivFactor);
		pst_Event->w_Flags |= EVE_C_EventFlag_DoOnce;

		_po_Track->SetTrack(_po_Track->mpst_Track);
		_po_Track->Invalidate();
		RecomputeWidthFromCur();
		WhenChanged();
		if(index == -1) index = _po_Track->mpst_Track->uw_NumEvents - 1;
		_po_Event = _po_Track->mo_ListEvents[index];
		_po_Event->SetAIFunction(pst_Event);
		_po_Event->OnLButtonDown();
		_po_Track->FillBar();

		return;

	case 9:
		// Create a AI function event as before
		SaveForUndo();
		index = EventToIndex(_po_Track, _po_Event);
		pst_Event = EVE_pst_NewEvent(_po_Track->mpst_Track, index, EVE_C_EventFlag_AIFunction);
		pst_Event->uw_NumFrames = EVE_TimeToFrame(1); // Temporary
		// This event is not executed only once
		pst_Event->w_Flags &= ~EVE_C_EventFlag_DoOnce;
		_po_Track->SetTrack(_po_Track->mpst_Track);
		_po_Track->Invalidate();
		RecomputeWidthFromCur();
		WhenChanged();
		if(index == -1) index = _po_Track->mpst_Track->uw_NumEvents - 1;
		_po_Event = _po_Track->mo_ListEvents[index];

		_po_Event->SetSoundPlayerAIFunction(pst_Event);

		_po_Event->OnLButtonDown();
		_po_Track->FillBar();

		ChooseDialogForSoundPlayer(_po_Track, _po_Event, pst_Event);

		return;

	/* New Key Event */
	case 3:
	case 4:
	case 5:
	case 6:
	case 30:
	case 60:
		SaveForUndo();
		index = EventToIndex(_po_Track, _po_Event);
        bSetFlashMatrix = 
            (index == 0 && 
            _po_Track->mpst_Track->uw_NumEvents > 0 && 
            _po_Track->mpst_Track->pst_AllEvents[0].w_Flags & EVE_C_EventFlag_Flash);

		pst_Event = EVE_pst_NewEvent(_po_Track->mpst_Track, index, EVE_C_EventFlag_InterpolationKey);
		pst_Event->uw_NumFrames = EVE_TimeToFrame(mst_Ini.f_ValFactor / mst_Ini.f_DivFactor);

		if((i_Res == 3) || (i_Res == 4) || (i_Res == 30))
			EVE_Event_InterpolationKey_AllocData(pst_Event, EVE_InterKeyType_Translation_0, 0);
		else
			EVE_Event_InterpolationKey_AllocData(pst_Event, EVE_InterKeyType_Rotation_Matrix, 0);

		if((i_Res == 4) || (i_Res == 6) || bSetFlashMatrix)
            EVE_SetFlashMatrix(pst_Event,_po_Track->mpo_Frame->mpst_GAO);

		if((i_Res == 30) || (i_Res == 60)) pst_Event->w_Flags |= EVE_C_EventFlag_NoInterpolation;

		_po_Track->SetTrack(_po_Track->mpst_Track);
		po_Prev = _po_Track->mo_ListEvents.GetAt(0);
		po_Prev = po_Prev->po_FindEditorEvent(pst_Event);
		po_Prev->SetInterpolationKey(pst_Event);
		break;

	case 31:
		SaveForUndo();
		index = EventToIndex(_po_Track, _po_Event);
		pst_Event = EVE_pst_NewEvent(_po_Track->mpst_Track, index, EVE_C_EventFlag_MorphKey);
		// TODO : Choose a correct size
		pst_Event->uw_NumFrames = EVE_TimeToFrame(mst_Ini.f_ValFactor / mst_Ini.f_DivFactor);

		// Set params to default values
		EVE_Event_MorphKey_AllocData(pst_Event, 1, 0, 0, 0, 0.0f, 1.0f);

		_po_Track->SetTrack(_po_Track->mpst_Track);
		//po_Prev = _po_Track->mo_ListEvents.GetAt(0);
		//po_Prev = po_Prev->po_FindEditorEvent(pst_Event);
		//po_Prev->SetMorphKey(pst_Event);

		break;

	case 50:
		SaveForUndo();
		SplitKey(ptl);
		break;

	case 500:
		SaveForUndo();
		index = EventToIndex(_po_Track, _po_Event);
		pst_Event = EVE_pst_NewEvent(_po_Track->mpst_Track, index, EVE_C_EventFlag_GotoLabel);
		pst_Event->uw_NumFrames = EVE_TimeToFrame(mst_Ini.f_ValFactor / mst_Ini.f_DivFactor);
		break;

	case 501:
		SaveForUndo();
		index = EventToIndex(_po_Track, _po_Event);
		pst_Event = EVE_pst_NewEvent(_po_Track->mpst_Track, index, EVE_C_EventFlag_SetTimeToLabel);
		pst_Event->uw_NumFrames = EVE_TimeToFrame(mst_Ini.f_ValFactor / mst_Ini.f_DivFactor);
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	/* Delete track */
	case 8:
		SaveForUndo();
		OnDeleteTrack(_po_Track);
		return;

	/* Delete selected events */
	case 103:
		DeleteSelected(TRUE);
		return;

	case 113:
		DeleteSelected(FALSE);
		return;

	/* Copy */
	case 104:
		Copy();
		break;

	/* Paste */
	case 105:
		SaveForUndo();
		Paste(_po_Track, _po_Event, FALSE);
		break;

	/* Paste / Insert with delay conservation */
	case 107:
		SaveForUndo();
		Paste(_po_Track, _po_Event, TRUE, FALSE, FALSE);
		break;

	/* Paste / Insert with delay conservation : Overwrite */
	case 108:
		SaveForUndo();
		Paste(_po_Track, _po_Event, TRUE, FALSE, FALSE, TRUE);
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	/* Set Delay */
	case 100:
		SaveForUndo();
		pst_Event = _po_Event->pst_FindEngineEvent();
		sprintf(asz_Name, "%f", EVE_FrameToTime(pst_Event->uw_NumFrames));
		o_Dialog.mo_Name = asz_Name;
		if(o_Dialog.DoModal() == IDOK)
		{
			f_Delay = (float) L_atof((char *) (LPCSTR) o_Dialog.mo_Name);
			SetDelaySelected(f_Delay);
		}
		break;

	/* Set to empty */
	case 101:
		SaveForUndo();
		pst_Event = _po_Event->pst_FindEngineEvent();
		pst_Event->w_Flags &= ~EVE_C_EventFlag_Type;
		pst_Event->w_Flags |= EVE_C_EventFlag_Empty;
		if(pst_Event->p_Data)
		{
			MEM_Free(pst_Event->p_Data);
			pst_Event->p_Data = NULL;
		}
		break;

	/* Edit */
	case 102:
		SaveForUndo();
		_po_Track->OnLButtonDblClk(0, CPoint(0, 0));
		break;

	/* Set param of sound player : choose a dialog file */
	case 150:
		ChooseDialogForSoundPlayer(_po_Track, _po_Event, _po_Event->pst_FindEngineEvent());
		break;

	/* Update size of the track according to the length of the dialog (for sound player only) */
	case 151:
		ResetSizeOfSoundPlayer(_po_Track, _po_Event);
		break;

	/* Open a dialog where the user can choose a dialog file. Then set the 2 first parameters of the event
	   (which must be the file key and the dialog id, like in a TEXT_tdst_Eval structure)  */
	case 152:
		ChooseDialogForEvent(_po_Track, _po_Event, _po_Event->pst_FindEngineEvent());
		break;

	/* Update size of the track according to the length of the dialog (for AI events with file key
	   and dialog id parameters, like SND_RequestPlayDialog) */
	case 153:
		ResetSizeOfRequestPlayDialog(_po_Track, _po_Event);
		break;

	/* Set color */
	case 666:
		if(_po_Event->mi_IndexAI != -1)
		{
			po_Col = new EDIA_cl_ColorDialog("Choose color", &mst_Ini.ac_ColIA[_po_Event->mi_IndexAI], NULL, 0, 0);
			po_Col->DoModal();
			delete po_Col;
			ForceRefresh();
		}
		break;

	/* Align left */
	case 667:
		SaveForUndo();
		AlignEventLeft();
		break;
	}

	_po_Track->SetTrack(_po_Track->mpst_Track);
	_po_Track->Invalidate();
	RecomputeWidthFromCur();
	WhenChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Track *EEVE_cl_Frame::AddTrack(EVE_tdst_Track *_pst_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Track	*po_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	po_Track = new EEVE_cl_Track;
	mo_ListTracks.AddTail(po_Track);
	po_Track->Create((CWnd *) (((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1))->mpo_Scroll));
	po_Track->mpo_Frame = this;
	po_Track->SetTrack(_pst_Track);
	po_Track->OnPaint();
	return po_Track;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::CloseAll(BOOL _b_Refresh)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&mt_Tre, 0, sizeof(mt_Tre));
	mul_Tre = BIG_C_InvalidIndex;
	mpst_GAO = NULL;
	mpst_ListTracks = NULL;
	mpst_Data = NULL;
	mul_GAO = BIG_C_InvalidIndex;
	mul_ListTracks = BIG_C_InvalidIndex;
	mpo_LastSelected = NULL;
	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		po_Track->CloseTrack(_b_Refresh);
		delete po_Track;
	}

	mpo_VarsView->ResetList();
	mo_ListTracks.RemoveAll();
	ForceRefresh();
	if(EVE_gb_CanCloseUndo) CloseUndo();
	EVE_gb_CanCloseUndo = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::ForceRefreshReal(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Event	*po_Event;
	EEVE_cl_Track	*po_Track;
	int				index, i;
	EVE_tdst_Params *pst_Param;
	EEVE_cl_Inside	*po_Inside;
	BOOL			b_First;
	int				xs, ys;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Data) return;
	if(!mpst_Data->pst_ListTracks) return;

	/* Save scroll pos */
	po_Inside = ((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1));
	xs = po_Inside->mpo_Scroll->GetScrollPosition().x;
	ys = po_Inside->mpo_Scroll->GetScrollPosition().y;

	b_First = TRUE;
	for(index = 0; index < mpst_Data->pst_ListTracks->uw_NumTracks; index++)
	{
		pst_Param = &mpst_Data->pst_ListParam[index];
		if(index < mo_ListTracks.GetCount())
		{
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(index));
			if(po_Track->mo_ListEvents.GetSize())
			{
				for(i = 0; i < po_Track->mo_ListEvents.GetSize(); i++)
				{
					po_Event = po_Track->mo_ListEvents.GetAt(i);
					po_Event->RecomputeWidth();
				}
			}

			po_Track->OnPaint();

			if(b_First)
			{
				po_Inside = ((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1));
				po_Inside->DrawRealTime();
				b_First = FALSE;
			}
		}
	}

	/* Restore scroll */
	po_Inside->mpo_Scroll->SetScrollPos(SB_HORZ, xs);
	po_Inside->mpo_Scroll->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (xs << 16), 0);
	po_Inside->mpo_Scroll->SetScrollPos(SB_VERT, ys);
	po_Inside->mpo_Scroll->SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (ys << 16), 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::ForceRefresh(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect			o_Rect;
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	int				i, xs, ys;
	EEVE_cl_Inside	*po_Inside;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->LockDisplay(mpo_MainSplitter->GetPane(0, 1));

	/* Save scroll pos */
	po_Inside = ((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1));
	xs = po_Inside->mpo_Scroll->GetScrollPosition().x;
	ys = po_Inside->mpo_Scroll->GetScrollPosition().y;

	i = 0;
	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		po_Track->mpst_Track = &mpst_ListTracks->pst_AllTracks[i];
		po_Track->mpst_Param = &mpst_Data->pst_ListParam[i];
		i++;
		po_Track->Invalidate();
		po_Track->SendMessage(WM_PAINT);
	}

	mpo_MainSplitter->GetPane(0, 1)->GetWindowRect(&o_Rect);
	mpo_MainSplitter->ScreenToClient(&o_Rect);
	o_Rect.right--;
	mpo_MainSplitter->GetPane(0, 1)->MoveWindow(&o_Rect);
	o_Rect.right++;
	mpo_MainSplitter->GetPane(0, 1)->MoveWindow(&o_Rect);

	/* po_Inside->mpo_Scroll->PlaceTracks(); */
	mpo_MainSplitter->GetPane(0, 1)->Invalidate();
	mpo_MainSplitter->GetPane(0, 1)->UpdateWindow();

	if(EEVE_Refresh) ForceRefreshReal();
	RefreshMenu();

	mpo_VarsView->mpo_ListBox->Invalidate();
	mpo_VarsView->mpo_ListBox->RedrawWindow(NULL, NULL, RDW_UPDATENOW);

	/* Restore scroll */
	po_Inside->mpo_Scroll->SetScrollPos(SB_HORZ, xs);
	po_Inside->mpo_Scroll->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (xs << 16), 0);
	po_Inside->mpo_Scroll->SetScrollPos(SB_VERT, ys);
	po_Inside->mpo_Scroll->SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (ys << 16), 0);

	M_MF()->UnlockDisplay(mpo_MainSplitter->GetPane(0, 1));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::InvertTracks(int i1, int i2)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	st_Track;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	L_memcpy(&st_Track, mpst_ListTracks->pst_AllTracks + i1, sizeof(EVE_tdst_Track));
	L_memcpy(mpst_ListTracks->pst_AllTracks + i1, mpst_ListTracks->pst_AllTracks + i2, sizeof(EVE_tdst_Track));
	L_memcpy(mpst_ListTracks->pst_AllTracks + i2, &st_Track, sizeof(EVE_tdst_Track));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::LoadTre(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	az_Name[BIG_C_MaxLenName];
	char	az_Path[BIG_C_MaxLenPath];
	ULONG	ul_Len;
	char	*pz;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&mt_Tre, 0, sizeof(mt_Tre));

	/* TRE existant, ou le créer */
	L_strcpy(az_Name, BIG_NameFile(mul_ListTracks));
	*L_strrchr(az_Name, '.') = 0;
	L_strcat(az_Name, ".tre");
	mul_Tre = BIG_ul_SearchFile(BIG_ParentFile(mul_ListTracks), az_Name);
	if(mul_Tre == BIG_C_InvalidIndex)
	{
		BIG_ComputeFullName(BIG_ParentFile(mul_ListTracks), az_Path);
		SAV_Begin(az_Path, az_Name);
		SAV_Buffer(&mt_Tre, sizeof(mul_Tre));
		mul_Tre = SAV_ul_End();
	}

	if(mul_Tre == BIG_C_InvalidIndex) return;
	pz = BIG_pc_ReadFileTmp(BIG_PosFile(mul_Tre), &ul_Len);
	if(ul_Len >= sizeof(mt_Tre)) L_memcpy(&mt_Tre, pz, sizeof(mt_Tre));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SaveTre(BOOL force)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	az_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mul_Tre == BIG_C_InvalidIndex) 
	{
		if(!force) return;
		LoadTre();
	}

	if(mul_Tre == BIG_C_InvalidIndex) return;
	BIG_ComputeFullName(BIG_ParentFile(mul_Tre), az_Path);
	SAV_Begin(az_Path, BIG_NameFile(mul_Tre));
	SAV_Buffer(&mt_Tre, sizeof(mt_Tre));
	SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SetListTracks(EVE_tdst_ListTracks *_pst_List)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	mul_Tre = BIG_C_InvalidIndex;
	if(!_pst_List)
	{
		mpst_ListTracks = NULL;
		return;
	}

	M_MF()->LockDisplay(this);
	for(i = 0; i < _pst_List->uw_NumTracks; i++) AddTrack(_pst_List->pst_AllTracks + i);
	mpst_ListTracks = _pst_List;
	mul_ListTracks = LOA_ul_SearchIndexWithAddress((ULONG) _pst_List);
	RecomputeWidthFromCur();
	ForceRefresh();
	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SetGAO(OBJ_tdst_GameObject *_pst_GO, BOOL _b_Refresh, BOOL _b_Force)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Track	*po_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->LockDisplay(this);
	if
	(
		OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)
	&&	_pst_GO->pst_Base
	&&	_pst_GO->pst_Base->pst_GameObjectAnim
	&&	_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]
	&&	((ULONG) _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0] != (ULONG) - 1)
	&&	_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data
	)
	{
		if
		(
			(!_b_Force)
		&&	(mpst_GAO == _pst_GO)
		&&	(mpst_Data)
		&&	(mpst_ListTracks == _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks)
		)
		{
			M_MF()->UnlockDisplay(this);
			return;
		}

		CloseAll(_b_Refresh);
		mpst_GAO = _pst_GO;
		mul_GAO = LOA_ul_SearchIndexWithAddress((ULONG) _pst_GO);
		mpst_Data = _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data;
		SetListTracks(mpst_Data->pst_ListTracks);

		if(!_b_Force)
		{
			UnselectAll();
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(0));
			po_Track->Select();
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(1));
			po_Track->Select();
			AfterEngine();
			SelectCurrentEvents();
		}
	}
	else
	{
		if
		(
			(!_b_Force)
		&&	(mpst_GAO == _pst_GO)
		&&	(_pst_GO->pst_Extended)
		&&	(mpst_Data == _pst_GO->pst_Extended->pst_Events)
		)
		{
			M_MF()->UnlockDisplay(this);
			return;
		}

		CloseAll(_b_Refresh);
		mpst_GAO = _pst_GO;
		mul_GAO = LOA_ul_SearchIndexWithAddress((ULONG) _pst_GO);
		if(_pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Events)
		{
			mpst_Data = _pst_GO->pst_Extended->pst_Events;
			SetListTracks(mpst_Data->pst_ListTracks);
		}
	}

	M_MF()->UnlockDisplay(this);
	ForceRefresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::UnselectAll(BOOL _b_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	CRect			o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		po_Track->UnselectAll();
		po_Track->mi_NumSelected = 0;

		if(_b_Track) po_Track->Unselect();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SelectCurrentEvents(BOOL _b_Speed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	CRect			o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected) po_Track->SelectCurrentEvent(_b_Speed);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SelectInRect(CRect *_po_Rect, BOOL _b_Add, BOOL _b_First)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	EEVE_cl_Event	*po_Event1;
	int				index;
	CRect			o_Rect1, o_Rect2;
	BOOL			b_Add, b_Refresh;
	int				first, last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Add mode ? */
	b_Add = FALSE;
	if(GetAsyncKeyState(VK_CONTROL) < 0 || _b_Add) b_Add = TRUE;

	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		b_Refresh = FALSE;
		po_Track = mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && !mst_Ini.i_DisplayHidden) continue;
		for(index = 0; index < po_Track->mo_ListEvents.GetSize(); index++)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(index);
			if(po_Event->mo_Rect.IsRectEmpty()) continue;
			o_Rect1 = *_po_Rect;
			o_Rect2 = po_Event->mo_Rect;
			po_Track->ClientToScreen(&o_Rect2);
			if(o_Rect1.IntersectRect(o_Rect1, o_Rect2))
			{
				/* sel: */
				if(!(po_Event->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected))
				{
					po_Event->pst_FindEngineEvent()->w_Flags |= EVE_C_EventFlag_Selected;
					if(!po_Event->mo_Rect.IsRectEmpty()) po_Track->InvalidateRect(po_Event->mo_Rect);
					po_Track->mi_NumSelected++;
					b_Refresh = TRUE;
				}
				else if(_b_First && GetAsyncKeyState(VK_CONTROL) < 0)
				{
					po_Event->pst_FindEngineEvent()->w_Flags &= ~EVE_C_EventFlag_Selected;
					if(!po_Event->mo_Rect.IsRectEmpty()) po_Track->InvalidateRect(po_Event->mo_Rect);
					po_Track->mi_NumSelected--;
					b_Refresh = TRUE;
				}

				first = index + 1;
				last = index + 1;
				while(last < po_Track->mo_ListEvents.GetSize() - 1)
				{
					po_Event1 = po_Track->mo_ListEvents.GetAt(last);
					if(po_Event1->mo_Rect.IsRectEmpty())
						last++;
					else
						break;
				}

				if((last > first) && (last != po_Track->mo_ListEvents.GetSize() - 1))
				{
					po_Event1 = po_Track->mo_ListEvents.GetAt(last);
					o_Rect1 = *_po_Rect;
					o_Rect2 = po_Event1->mo_Rect;
					po_Track->ClientToScreen(&o_Rect2);
					if(o_Rect1.IntersectRect(o_Rect1, o_Rect2))
					{
						while(first != last)
						{
							po_Event1 = po_Track->mo_ListEvents.GetAt(first);
							po_Event1->pst_FindEngineEvent()->w_Flags |= EVE_C_EventFlag_Selected;
							if(!po_Event1->mo_Rect.IsRectEmpty()) po_Track->InvalidateRect(po_Event1->mo_Rect);
							po_Track->mi_NumSelected++;
							b_Refresh = TRUE;
							first++;
						}
					}
				}
			}
			else if(po_Event->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)
			{
				if(!b_Add)
				{
unsel:
					po_Event->pst_FindEngineEvent()->w_Flags &= ~EVE_C_EventFlag_Selected;
					po_Track->InvalidateRect(po_Event->mo_Rect);
					po_Track->mi_NumSelected--;
					b_Refresh = TRUE;
					while(index < po_Track->mo_ListEvents.GetSize() - 1)
					{
						po_Event1 = po_Track->mo_ListEvents.GetAt(index + 1);
						if(po_Event1->mo_Rect.IsRectEmpty())
						{
							po_Event = po_Event1;
							index++;
							goto unsel;
						}
						else
							break;
					}
				}
			}
		}

		if(b_Refresh) po_Track->UpdateWindow();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::OneTrameEnding(void)
{
	if
	(
		M_MF()->mpo_MaxView
	&&	M_MF()->mpo_MaxView != mpo_MyView
	&&	!mpo_MyView->mb_Floating
	&&	M_MF()->mst_Desktop.b_VeryMaximized
	&&	mb_IsActivate
	) return;
	if(LINK_gi_SpeedDraw != 0) return;

	ForceRefreshReal();

	/* The anim has changed in current GAO ? */
	if(mst_Ini.i_LinkAction && mpst_GAO && mpst_GAO->pst_Base && mpst_GAO->pst_Base->pst_GameObjectAnim && mpst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0])
	{
		if(mpst_ListTracks && !mpst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data)
			CloseAll(TRUE);
		else if(mpst_ListTracks != mpst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks)
		{
			if(!mpst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks)
			{
				CloseAll(TRUE);
			}
			else
			{
				SetGAO(mpst_GAO);
			}
		}
	}

	mpo_VarsView->mpo_ListBox->BeforeRefresh();
	mpo_VarsView->mpo_ListBox->RedrawWindow(NULL, NULL, RDW_UPDATENOW);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::MoveRunningBox(EEVE_cl_Track *_po_Track, float _f_Delay, BOOL _b_CanScroll, BOOL cansend)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Track		*po_Track;
	int					index;
	int					i;
	float				f_RemainingTrackTime;
	EEVE_cl_Inside		*po_Inside;
	CRect				o_Rect;
	CPoint				o_Scroll;
	CPoint				o_Scroll1;
	EVE_tdst_Params		*pst_Param;
	EVE_tdst_Track		*pst_Track;
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor;
	int					j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Data) return;

	for(i = 0; cansend && i < EDI_C_MaxViews; i++)
	{
		/* Test all views, and all editors */
		po_View = M_MF()->po_GetViewWithNum(i);
		for(j = 0; j < M_CurNumEd(po_View->mi_NumPane); j++)
		{
			po_Editor = M_CurEd(po_View->mi_NumPane, j).po_Editor;
			if(po_Editor && po_Editor->b_CanActivate())
			{
				if(mst_BaseIni.ab_LinkTo[po_Editor->mi_PosInGArray][po_Editor->mi_NumEdit])
				{
					if(po_Editor->mst_Def.i_Type == EDI_IDEDIT_EVENTS)
					{
						((EEVE_cl_Frame *) po_Editor)->MoveRunningBox(NULL, _f_Delay, FALSE, FALSE);
					}
				}
			}
		}
	}

	/* We want to be able to stop ON the last zero-delay event */
	if(_f_Delay != 0.0f)
	{
		pst_Track = &mpst_Data->pst_ListTracks->pst_AllTracks[0];
		pst_Param = &mpst_Data->pst_ListParam[0];
		f_RemainingTrackTime = 0.0f;
		for(i = pst_Param->uw_CurrentEvent; i < pst_Track->uw_NumEvents; i++)
			f_RemainingTrackTime += EVE_FrameToTime(pst_Track->pst_AllEvents[i].uw_NumFrames);

		f_RemainingTrackTime -= pst_Param->f_Time;

		if(fabs(f_RemainingTrackTime - _f_Delay) < 10E-5)
		{
			GotoEnd();
			return;
		}
	}

recom:
	EVE_gb_NeedToRecomputeSND = TRUE;
	EVE_gb_NeedToReparse = FALSE;
	if(mst_Ini.b_RunSel)
	{
		for(index = 0; index < mpst_Data->pst_ListTracks->uw_NumTracks; index++)
		{
			if(index >= mo_ListTracks.GetCount()) break;
			pst_Param = &mpst_Data->pst_ListParam[index];
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(index));
			if(!(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)) continue;
			po_Track->MoveRunningBox(FALSE, _f_Delay);
		}
	}
	else if(!mst_Ini.i_AutoAlign)
	{
		for(index = 0; index < mpst_Data->pst_ListTracks->uw_NumTracks; index++)
		{
			if(index >= mo_ListTracks.GetCount()) break;
			pst_Param = &mpst_Data->pst_ListParam[index];
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(index));
			po_Track->MoveRunningBox(FALSE, _f_Delay);
			if(index == 0) ForceRefreshReal();
			if(EVE_gb_NeedToReparse) goto recom;
		}
	}
	else
	{
		if(!_po_Track) _po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(0));
		_po_Track->MoveRunningBox(FALSE, _f_Delay);
		if(_po_Track)
		{
			for(index = 0; index < mpst_Data->pst_ListTracks->uw_NumTracks; index++)
			{
				po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(index));
				if(po_Track == _po_Track)
				{
					Align(index);
					break;
				}
			}
		}
		else
			Align(0);

		/* if(EVE_gb_NeedToReparse) goto recom; */
	}

	/* Center running box */
	if(_b_CanScroll)
	{
		po_Inside = ((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1));
		po_Inside->mpo_Scroll->GetClientRect(&o_Rect);
		o_Rect.left += po_Inside->mpo_Scroll->GetScrollPosition().x;
		o_Rect.right += po_Inside->mpo_Scroll->GetScrollPosition().x;
		po_Track = mo_ListTracks.GetHead();
		if((po_Track->mo_TrackTime.left > o_Rect.right) || (po_Track->mo_TrackTime.left < o_Rect.left))
		{
			o_Scroll1 = po_Inside->mpo_Scroll->GetScrollPosition();
			o_Scroll.x = po_Track->mo_TrackTime.left;
			o_Scroll.y = po_Inside->mpo_Scroll->GetScrollPosition().y;
			po_Inside->mpo_Scroll->GetClientRect(&o_Rect);
			o_Scroll.x -= o_Rect.Width() / 2;
			po_Inside->mpo_Scroll->ScrollToPosition(o_Scroll);
			po_Inside->WhenScroll(o_Scroll1.x - o_Scroll.x, o_Scroll1.y - o_Scroll.y);
		}
	}

	/* Hierarchy */
	if(mpst_GAO) OBJ_HierarchyMainCall(mpst_GAO->pst_World);
	EVE_gb_NeedToRecomputeSND = TRUE;
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
void EEVE_cl_Frame::CloseCopy(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos;
	EEVE_tdst_CopyEvent *pst_CopyEvent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pos = EEVE_go_ListCopyEvents.GetHeadPosition();
	while(pos)
	{
		pst_CopyEvent = EEVE_go_ListCopyEvents.GetNext(pos);
		if(pst_CopyEvent->pst_Event->p_Data) MEM_Free(pst_CopyEvent->pst_Event->p_Data);
		MEM_Free(pst_CopyEvent->pst_Event);
		MEM_Free(pst_CopyEvent);
	}

	EEVE_go_ListCopyEvents.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::CloseCopyTracks(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EVE_tdst_Track	*pst_CopyTrack;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pos = EEVE_go_ListCopyTracks.GetHeadPosition();
	while(pos)
	{
		pst_CopyTrack = EEVE_go_ListCopyTracks.GetNext(pos);
		for(i = 0; i < pst_CopyTrack->uw_NumEvents; i++)
		{
			if(pst_CopyTrack->pst_AllEvents[i].p_Data) MEM_Free(pst_CopyTrack->pst_AllEvents[i].p_Data);
		}

		if(pst_CopyTrack->pst_AllEvents) MEM_Free(pst_CopyTrack->pst_AllEvents);
		MEM_Free(pst_CopyTrack);
	}

	EEVE_go_ListCopyTracks.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::CopySelectedTracks(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	int				j;
	void			*p;
	EVE_tdst_Track	*pst_CopyTrack;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CloseCopyTracks();

	/* Get indexes */
	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && !mst_Ini.i_DisplayHidden)
		{
			continue;
		}

		if(!(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)) continue;

		pst_CopyTrack = (EVE_tdst_Track *) MEM_p_Alloc(sizeof(EVE_tdst_Track));
		L_memcpy(pst_CopyTrack, po_Track->mpst_Track, sizeof(EVE_tdst_Track));

		pst_CopyTrack->uw_Flags &= ~EVE_C_Track_Selected;

		if(po_Track->mpst_Track->uw_NumEvents)
		{
			pst_CopyTrack->pst_AllEvents = (EVE_tdst_Event *) MEM_p_Alloc(po_Track->mpst_Track->uw_NumEvents * sizeof(EVE_tdst_Event));

			L_memcpy
			(
				pst_CopyTrack->pst_AllEvents,
				po_Track->mpst_Track->pst_AllEvents,
				po_Track->mpst_Track->uw_NumEvents * sizeof(EVE_tdst_Event)
			);
		}
		else
			pst_CopyTrack->pst_AllEvents = NULL;

		pst_CopyTrack->pc_AllData = NULL;

		for(j = 0; j < pst_CopyTrack->uw_NumEvents; j++)
		{
			if(pst_CopyTrack->pst_AllEvents[j].p_Data)
			{
				p = MEM_p_Alloc(*(short *) pst_CopyTrack->pst_AllEvents[j].p_Data);
				L_memcpy(p, pst_CopyTrack->pst_AllEvents[j].p_Data, *(short *) pst_CopyTrack->pst_AllEvents[j].p_Data);
				pst_CopyTrack->pst_AllEvents[j].p_Data = p;
			}
		}

		EEVE_go_ListCopyTracks.AddTail(pst_CopyTrack);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MoveOneTrackFromIndexToIndex(EVE_tdst_ListTracks *_pst_TrackList, int Index1, int Index2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_TempTracks;
	int				Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_TrackList) return;
	if(Index1 == Index2) return;
	if(Index1 >= _pst_TrackList->uw_NumTracks) return;
	if(Index2 >= _pst_TrackList->uw_NumTracks) return;

	if(Index1 < Index2)
	{
		Temp = Index1;
		Index1 = Index2;
		Index2 = Temp;
	}

	pst_TempTracks = (EVE_tdst_Track *) MEM_p_Alloc((Index1 - Index2) * sizeof(EVE_tdst_Track));

	L_memcpy(pst_TempTracks, &_pst_TrackList->pst_AllTracks[Index2], (Index1 - Index2) * sizeof(EVE_tdst_Track));
	L_memcpy(&_pst_TrackList->pst_AllTracks[Index2], &_pst_TrackList->pst_AllTracks[Index1], sizeof(EVE_tdst_Track));
	L_memcpy(&_pst_TrackList->pst_AllTracks[Index2 + 1], pst_TempTracks, (Index1 - Index2) * sizeof(EVE_tdst_Track));

	MEM_Free(pst_TempTracks);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::PasteCopiedTracks(int Rank)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EVE_tdst_Track	*pst_CopyTrack;
	EVE_tdst_Track	*pst_NewTrack;
	int				j;
	void			*p;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Data) return;

	pos = EEVE_go_ListCopyTracks.GetHeadPosition();
	while(pos)
	{
		pst_CopyTrack = EEVE_go_ListCopyTracks.GetNext(pos);

		pst_NewTrack = EVE_pst_NewTrack(mpst_Data);

		L_memcpy(pst_NewTrack, pst_CopyTrack, sizeof(EVE_tdst_Track));

		if (pst_CopyTrack->uw_NumEvents)
		{
			pst_NewTrack->pst_AllEvents = (EVE_tdst_Event *) MEM_p_Alloc(pst_CopyTrack->uw_NumEvents * sizeof(EVE_tdst_Event));

			L_memcpy
			(
				pst_NewTrack->pst_AllEvents,
				pst_CopyTrack->pst_AllEvents,
				pst_CopyTrack->uw_NumEvents * sizeof(EVE_tdst_Event)
			);
		}

		pst_NewTrack->pc_AllData = NULL;

		for(j = 0; j < pst_CopyTrack->uw_NumEvents; j++)
		{
			if(pst_NewTrack->pst_AllEvents[j].p_Data)
			{
				p = MEM_p_Alloc(*(short *) pst_CopyTrack->pst_AllEvents[j].p_Data);
				L_memcpy(p, pst_NewTrack->pst_AllEvents[j].p_Data, *(short *) pst_NewTrack->pst_AllEvents[j].p_Data);
				pst_NewTrack->pst_AllEvents[j].p_Data = p;
			}
		}

		AddTrack(pst_NewTrack);

		MoveOneTrackFromIndexToIndex(mpst_Data->pst_ListTracks, mpst_Data->pst_ListTracks->uw_NumTracks - 1, Rank);

		Rank++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::Copy(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos;
	EEVE_cl_Track		*po_Track;
	int					index;
	int					indextrack;
	EVE_tdst_Event		*pst_New;
	void				*p;
	EEVE_tdst_CopyEvent *pst_Copy;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CloseCopy();

	/* Get indexes */
	pos = mo_ListTracks.GetHeadPosition();
	indextrack = 0;
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && !mst_Ini.i_DisplayHidden)
		{
			indextrack++;
			continue;
		}

		for(index = 0; index < po_Track->mpst_Track->uw_NumEvents; index++)
		{
			if(po_Track->mpst_Track->pst_AllEvents[index].w_Flags & EVE_C_EventFlag_Selected)
			{
				pst_Copy = (EEVE_tdst_CopyEvent *) MEM_p_Alloc(sizeof(EEVE_tdst_CopyEvent));
				pst_New = (EVE_tdst_Event *) MEM_p_Alloc(sizeof(EVE_tdst_Event));
				pst_Copy->pst_Event = pst_New;
				pst_Copy->i_NumTrack = indextrack;
				pst_Copy->i_NumEvent = index;
				L_memcpy(pst_New, &po_Track->mpst_Track->pst_AllEvents[index], sizeof(EVE_tdst_Event));
				pst_New->w_Flags &= ~EVE_C_EventFlag_Selected;
				if(pst_New->p_Data)
				{
					p = MEM_p_Alloc(*(short *) po_Track->mpst_Track->pst_AllEvents[index].p_Data);
					pst_New->p_Data = p;
					L_memcpy
					(
						p,
						po_Track->mpst_Track->pst_AllEvents[index].p_Data,
						*(short *) po_Track->mpst_Track->pst_AllEvents[index].p_Data
					);
				}

				EEVE_go_ListCopyEvents.AddTail(pst_Copy);
			}
		}

		indextrack++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_b_CheckOriginalEvent(EVE_tdst_Event *_pst_Original, EVE_tdst_Event *_pst_Copy)
{
	if(_pst_Copy->p_Data)
	{
		if(!L_memcmp(_pst_Original->p_Data, _pst_Copy->p_Data, sizeof(*((short *) _pst_Copy->p_Data + 1))))
			return TRUE;
		else
		{
			LINK_PrintStatusMsg("Bug OriginalEvent");
			return FALSE;
		}
	}
	else
	{
		if(_pst_Copy->uw_NumFrames == _pst_Original->uw_NumFrames)
			return TRUE;
		else
		{
			LINK_PrintStatusMsg("Bug OriginalEvent");
			return FALSE;
		}
	}
}

/*$F
 =======================================================================================================================
	Note:		
		  [_b_Add = TRUE], we are about to really INSERT a new event. We dont want the
		  following events to have their delay changed.
		 
		
		  [_b_Add = FALSE], we are about to OVERWRITE the current event
		  with a copied one. We must recompute the remaining events delay so as the entire track time CANNOT be less than before.

			if _b_Sym is set, the Role of _b_Add changes. 

							- If it is off, that means that we want to use sysmetry reference.
							- If it is on,  that means that we want to really add symetry keys.
		
 =======================================================================================================================
 */
void EEVE_cl_Frame::CopyOneEvent
(
	EVE_tdst_Track		*pst_Track,
	int					i_Event,
	EVE_tdst_Event		*pst_Event,
	BOOL				_b_Add,		/* We are adding new events */
	BOOL				_b_Sym,		/* Symetry case ? */
	BOOL				_b_LastKey, /* Are we on the Last Key ? */
	EEVE_tdst_CopyEvent *pst_CopyEvent
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_New, *pst_Original;
	int				i_NumInter;
	BOOL			b_AnimCase, b_LastKeyHasZeroDelay;
	void			*p_NewEventData;
	short			w_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(mpst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	&&	mpst_GAO->pst_Base
	&&	mpst_GAO->pst_Base->pst_GameObjectAnim
	&&	mpst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0]
	&&	((ULONG) mpst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0] != (ULONG) - 1)
	) b_AnimCase = TRUE;
	else
		b_AnimCase = FALSE;

	/*
	 * Check if the last Event of the Track has a Zero_Delay BEFORE our Copy. If it is
	 * the case, we will force the last Event to have a Zero-Delay after our copy
	 * whatever we have done with the Last Event.
	 */
	if(pst_Track->pst_AllEvents && (pst_Track->pst_AllEvents[pst_Track->uw_NumEvents - 1].uw_NumFrames == 0))
		b_LastKeyHasZeroDelay = TRUE;
	else
		b_LastKeyHasZeroDelay = FALSE;

	/*
	 * We have chosen a Copy-Mode before entering this function
	 * (OnlyRotation/OnlyTranslation/ALL). The Current event may not fit this Mode
	 */
	if((pst_Event->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
	{
		w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Event);
		if(w_Type && (w_Type != -1))
		{
			if
			(
				((w_Type & EVE_InterKeyType_Translation_Mask) && (EEVE_gi_CopyMode & EEVE_C_CopyOnlyRotation))
			||	((w_Type & EVE_InterKeyType_Rotation_Matrix) && (EEVE_gi_CopyMode & EEVE_C_CopyOnlyTranslation))
			||	((w_Type & EVE_InterKeyType_Rotation_Quaternion) && (EEVE_gi_CopyMode & EEVE_C_CopyOnlyTranslation))
			) return;
		}
	}

	p_NewEventData = NULL;
	i_NumInter = 0;

	/* Gets or creates the Data of the new Event we are about to create. */
	if(_b_Sym)
	{
		if(!pst_Track) return;
		if(!pst_CopyEvent) return;

		/* Symetries can only concern Interpolation Keys */
		if((pst_Event->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey)
		{
			LINK_PrintStatusMsg("Cannot use Symetry on Non-Interpolation keys tracks.");
			return;
		}

		if(pst_Event->p_Data)
		{
			if(_b_Add)
				p_NewEventData = EVE_p_SymetrizeData(pst_Event->p_Data);
			else
			{
				pst_Original = &mpst_ListTracks->pst_AllTracks[pst_CopyEvent->i_NumTrack].pst_AllEvents[pst_CopyEvent->i_NumEvent];
				if(pst_Original && EEVE_b_CheckOriginalEvent(pst_Original, pst_Event))
					p_NewEventData = pst_Original->p_Data;
				else
					return;
			}
		}
	}
	else
	{
		if(pst_Event->p_Data)
		{
			p_NewEventData = MEM_p_Alloc(*(short *) pst_Event->p_Data);
			L_memcpy(p_NewEventData, pst_Event->p_Data, *(short *) pst_Event->p_Data);
		}
	}

	/* Sets the delay of the new Event. */
	if(!_b_Add) pst_Event->uw_NumFrames = pst_Track->pst_AllEvents[i_Event].uw_NumFrames;

	if(_b_Add && !_b_Sym)
	{
		pst_New = EVE_pst_NewEvent(pst_Track, i_Event, EVE_C_EventFlag_Empty);
		pst_New->uw_NumFrames = pst_Event->uw_NumFrames;
		pst_New->w_Flags = pst_Event->w_Flags;
		pst_New->p_Data = p_NewEventData;

		i_NumInter = pst_Event->uw_NumFrames;
	}
	else
	{
		/* Erasing current Event */
		if(pst_Track->pst_AllEvents[i_Event].p_Data)
		{
			MEM_Free(pst_Track->pst_AllEvents[i_Event].p_Data);
			pst_Track->pst_AllEvents[i_Event].p_Data = NULL;
		}

		L_memcpy(&pst_Track->pst_AllEvents[i_Event], pst_Event, sizeof(EVE_tdst_Event));

		if(pst_Event->p_Data)
		{
			pst_Track->pst_AllEvents[i_Event].p_Data = p_NewEventData;
			if(_b_Sym && !_b_Add) pst_Track->pst_AllEvents[i_Event].w_Flags |= EVE_C_EventFlag_Symetric;
		}
	}

	if(b_AnimCase)
	{
		/* Sets the last event's delay to 0 if it was the case before our Copy */
		if(b_LastKeyHasZeroDelay) pst_Track->pst_AllEvents[pst_Track->uw_NumEvents - 1].uw_NumFrames = 0;

		/* if(i_NumInter && !_b_LastKey) { AddFrameAll(pst_Track, i_NumInter, TRUE); } */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::PasteFirstSelAbsolute(void)
{
}

/*
 =======================================================================================================================
    Aim: Find the first Track and the first selected event within this Track to call the Paste. Note: It is not a
    "Paste only the first Track than has been copied", it is a "Paste ALL at the current first selected Track".
 =======================================================================================================================
 */
void EEVE_cl_Frame::PasteFirstSel(BOOL _b_Add, BOOL _b_Sym)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get first selected track */
	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected) break;
	}

	if(!po_Track) return;

	/* Get first selected event */
	int i=0;
	for(i = 0; i < po_Track->mo_ListEvents.GetSize(); i++)
	{
		po_Event = po_Track->mo_ListEvents.GetAt(i);
		if(po_Event->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected) break;
	}

	if(i == po_Track->mo_ListEvents.GetSize()) return;

	/* Paste */
	Paste(po_Track, po_Event, _b_Add, TRUE, _b_Sym);

	/*
	 * We may have pasted IK Events on non IK Events or the opposite. Call an
	 * UpdateNextValues on both rotation and translation tracks.
	 */
	EVE_Event_InterpolationKey_UpdateNextValue(mpst_GAO, mpst_ListTracks, po_Track->mpst_Track);
	EVE_Event_InterpolationKey_UpdateNextValue(mpst_GAO, mpst_ListTracks, po_Track->mpst_Track + 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::Paste
(
	EEVE_cl_Track	*_po_Track,
	EEVE_cl_Event	*_po_Base,
	BOOL			_b_Add,
	BOOL			_b_Ani,
	BOOL			_b_Sym,
	BOOL			_b_Over
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos, pos1;
	int					numtrack;
	EEVE_tdst_CopyEvent *pst_Copy, *pst_Copy1;
	EVE_tdst_Event		*pst_New, *pst_Prev, *pst_Event;
	EVE_tdst_Track		*pst_Track;
	int					i_Track;
	int					i_Event;
	float				f_Delay, f_New, f_Tot;
	float				f_Max, f_Max1;
	BOOL				b_LastKey;
	int					xs, ys;
	EEVE_cl_Inside		*po_Inside;
	USHORT				frame, count, counttot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Save scroll pos */
	po_Inside = ((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1));
	xs = po_Inside->mpo_Scroll->GetScrollPosition().x;
	ys = po_Inside->mpo_Scroll->GetScrollPosition().y;

	/* Get index of first track */
	i_Track = _po_Track->mpst_Track - mpst_ListTracks->pst_AllTracks;
	pst_Track = _po_Track->mpst_Track;

	/* Get the last event to retreive the last time */
	pos = EEVE_go_ListCopyEvents.GetHeadPosition();
	if(!pos) return;
	pst_Copy = EEVE_go_ListCopyEvents.GetAt(pos);
	M_MF()->LockDisplay(this);

	/* Compute f_Max the max time in all copied tracks. */
	f_Max = 0;
	while(pos)
	{
		numtrack = pst_Copy->i_NumTrack;
		f_Max1 = 0.0f;
		while((pos) && (pst_Copy->i_NumTrack == numtrack))
		{
			f_Max1 += EVE_FrameToTime(pst_Copy->pst_Event->uw_NumFrames);
			if(pos)
			{
				EEVE_go_ListCopyEvents.GetNext(pos);
				if(pos) pst_Copy = EEVE_go_ListCopyEvents.GetAt(pos);
			}
		}

		if(!f_Max || f_Max1 > f_Max) f_Max = f_Max1;
	}

	/* Get time to current event from the beginning of the track */
	f_Delay = 0.0f;
	i_Event = 0;
	if(_po_Base)
	{
		pst_Prev = _po_Base->pst_FindEngineEvent();
		pst_New = _po_Track->mpst_Track->pst_AllEvents;
		while(pst_New != pst_Prev)
		{
			f_Delay += EVE_FrameToTime(pst_New->uw_NumFrames);
			pst_New++;
			i_Event++;
		}
	}
	else
	{
		f_Delay = 1000000000000.0f;
		i_Event = -1;
	}

	b_LastKey = (i_Event == pst_Track->uw_NumEvents - 1);

	/* Parse all copied events */
	pos = EEVE_go_ListCopyEvents.GetHeadPosition();
	if(!pos) return;
	pst_Copy = EEVE_go_ListCopyEvents.GetAt(pos);
	while(pos)
	{
		f_Tot = 0;
		frame = 0;
		numtrack = pst_Copy->i_NumTrack;
		count = 0;

		/* Count number of events for a given track */
		counttot = 0;
		pst_Copy1 = pst_Copy;
		pos1 = pos;
		while(pos1 && (pst_Copy1->i_NumTrack == numtrack))
		{
			counttot++;
			EEVE_go_ListCopyEvents.GetNext(pos1);
			if(pos1) pst_Copy1 = EEVE_go_ListCopyEvents.GetAt(pos1);
		}

		while((pos) && (pst_Copy->i_NumTrack == numtrack) && (i_Event < pst_Track->uw_NumEvents))
		{
			count++;
			f_Tot += EVE_FrameToTime(pst_Copy->pst_Event->uw_NumFrames);
			if(f_Tot > f_Max) pst_Copy->pst_Event->uw_NumFrames -= EVE_TimeToFrame(f_Tot - f_Max);

			if(_b_Ani && count == counttot)
				CopyOneEvent(pst_Track, i_Event, pst_Copy->pst_Event, FALSE, _b_Sym, b_LastKey, pst_Copy);
			else
			{
				CopyOneEvent(pst_Track, i_Event, pst_Copy->pst_Event, _b_Add, _b_Sym, b_LastKey, pst_Copy);
				frame += pst_Copy->pst_Event->uw_NumFrames;
			}

			if(pos)
			{
				EEVE_go_ListCopyEvents.GetNext(pos);
				if(pos) pst_Copy = EEVE_go_ListCopyEvents.GetAt(pos);
				if(i_Event != -1) i_Event++;
				pst_Prev++;
			}
		}

		/*
		 * On vire tous les events décalé pour refleter un paste par dessus et pas en
		 * insert
		 */
		if((_b_Ani && counttot > 1) || (_b_Over && counttot))
		{
			if(_b_Ani)
			{
				pst_Event = &pst_Track->pst_AllEvents[i_Event - 1];
				i_Event--;
			}
			else
			{
				pst_Event = &pst_Track->pst_AllEvents[i_Event];
			}

			while(frame)
			{
				if(_b_Ani && i_Event == pst_Track->uw_NumEvents - 1) break;
				if(pst_Event->uw_NumFrames <= frame)
				{
					frame -= pst_Event->uw_NumFrames;
					if(_b_Ani)
					{
						pst_Event->uw_NumFrames = (pst_Event + 1)->uw_NumFrames;
						EVE_DeleteEvent(pst_Track, i_Event + 1);
						pst_Event = pst_Track->pst_AllEvents + i_Event;
					}
					else
					{
						EVE_DeleteEvent(pst_Track, i_Event);
						pst_Event = pst_Track->pst_AllEvents + i_Event;
					}
				}
				else
				{
					pst_Event->uw_NumFrames -= frame;
					frame = 0;
					break;
				}

				if(i_Event == pst_Track->uw_NumEvents) break;
			}
		}

		/* Zap some copies ? */
		if(!pos) break;
		if(i_Event >= pst_Track->uw_NumEvents)
		{
recom1:
			if(!pos) break;
			pst_Copy = EEVE_go_ListCopyEvents.GetAt(pos);
			while(pos && pst_Copy->i_NumTrack == numtrack)
			{
				pst_Copy = EEVE_go_ListCopyEvents.GetNext(pos);
				LINK_PrintStatusMsg("Some copies have been zapped");
			}

			if(!pos) break;
		}

		/* Pass to next track */
		i_Track += pst_Copy->i_NumTrack - numtrack;
		if(i_Track > mpst_ListTracks->uw_NumTracks - 1) break;

		/* Search the best first event */
		f_New = 0.0f;
		pst_Track = &mpst_ListTracks->pst_AllTracks[i_Track];
		pst_Prev = pst_Track->pst_AllEvents;
		if(!pst_Prev)
		{
			if(!pos) break;
			EEVE_go_ListCopyEvents.GetNext(pos);
			goto recom1;
		}

		i_Event = 0;
		if(f_Delay)
		{
			while
			(
				(i_Event < pst_Track->uw_NumEvents)
			&&	(f_Delay - (f_New + EVE_FrameToTime(pst_Prev->uw_NumFrames)) > 0.0001f)
			)
			{
				f_New += EVE_FrameToTime(pst_Prev->uw_NumFrames);
				i_Event++;
				pst_Prev++;
			}

			i_Event++;
			pst_Prev++;
		}

		if(i_Event > pst_Track->uw_NumEvents)
		{
			if(!_b_Add)
			{
				if(!pos) break;
				EEVE_go_ListCopyEvents.GetNext(pos);
				goto recom1;
			}
			else
			{
				i_Event = -1;
			}
		}
	}

	/* Force all LAST events to be 0 delay for animation */
	if(_b_Ani)
	{
		for(int i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
		{
			mpst_ListTracks->pst_AllTracks[i].pst_AllEvents[mpst_ListTracks->pst_AllTracks[i].uw_NumEvents - 1].uw_NumFrames = 0;
		}
	}

	/* Refresh all tracks */
	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		_po_Track = mo_ListTracks.GetNext(pos);
		_po_Track->SetTrack(_po_Track->mpst_Track);
		_po_Track->Invalidate();
	}

	MoveRunningBox(NULL, 0.0f, TRUE);

	/* Restore scroll */
	po_Inside->mpo_Scroll->SetScrollPos(SB_HORZ, xs);
	po_Inside->mpo_Scroll->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (xs << 16), 0);
	po_Inside->mpo_Scroll->SetScrollPos(SB_VERT, ys);
	po_Inside->mpo_Scroll->SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (ys << 16), 0);
	ForceRefresh();
	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
    Vincent:: la variable f_Dif ne sert plus à déterminer le temps de la seconde clé. Ce temp est maintenant égal à
    l'ancien temps moins le nouveau temps de la clé scindée ainsi on est sur que la somme des temps des 2 clés est égal
    au temps de la clé avant split
 =======================================================================================================================
 */
void EEVE_cl_Frame::SplitOneEvent
(
	EVE_tdst_Track	*_pst_Track,
	EVE_tdst_Params *_pst_Params,
	int				index,
	float			f_Width,
	float			f_Dif
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt, *pst_New;
	void			*p;
	unsigned short	uw_FrameLeft;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = _pst_Track->pst_AllEvents + index;
	pst_Evt->w_Flags &= ~EVE_C_EventFlag_Selected;

	/* Recompute delay for previous event */
	uw_FrameLeft = pst_Evt->uw_NumFrames;
	pst_Evt->uw_NumFrames = EVE_TimeToFrame((f_Width / mst_Ini.f_Factor) * mst_Ini.f_ValFactor);
	uw_FrameLeft -= pst_Evt->uw_NumFrames;

	/* Insert new event */
	pst_New = EVE_pst_NewEvent(_pst_Track, index + 1, EVE_C_EventFlag_Empty);

	/* The NewEvent has performed a ReAlloc of pst-AllEvents. pst_Evt is no more valid. */
	pst_Evt = _pst_Track->pst_AllEvents + index;

	L_memcpy(pst_New, pst_Evt, sizeof(EVE_tdst_Event));
	if(pst_New->p_Data)
	{
		p = MEM_p_Alloc(*(short *) pst_Evt->p_Data);
		pst_New->p_Data = p;
		L_memcpy(p, pst_Evt->p_Data, *(short *) pst_Evt->p_Data);
	}

	/*
	 * pst_New->uw_NumFrames = EVE_TimeToFrame((f_Dif / mst_Ini.f_Factor)
	 * mst_Ini.f_ValFactor);
	 */
	pst_New->uw_NumFrames = uw_FrameLeft;
	pst_New->w_Flags &= ~EVE_C_EventFlag_Flash;
	pst_New->w_Flags &= ~EVE_C_EventFlag_Selected;

	/* Inform key */
	switch(pst_New->w_Flags & EVE_C_EventFlag_Type)
	{
	case EVE_C_EventFlag_AIFunction:
		break;

	case EVE_C_EventFlag_InterpolationKey:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			OBJ_tdst_GameObject *pst_GO;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_GO = _pst_Track->pst_GO;
			if(!pst_GO) pst_GO = mpst_GAO;

			if(pst_Evt->w_Flags & EVE_C_EventFlag_NoInterpolation)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector		*V;
				MATH_tdst_Matrix		*M;
				MATH_tdst_Quaternion	Q;
				MATH_tdst_CompressedQuaternion	CQ;
				char					*pc_Data;
				char					*pc_DataNew;
				int						w_Type;
				/*~~~~~~~~~~~~~~~~~~~~~~~*/


				V = EVE_pst_Event_InterpolationKey_GetPos(pst_Evt);
				if(V) MATH_CopyVector(EVE_pst_Event_InterpolationKey_GetPos(pst_New), V);

				M = EVE_pst_Event_InterpolationKey_GetRotation(pst_Evt);
				if(M) MATH_CopyMatrix(EVE_pst_Event_InterpolationKey_GetRotation(pst_New), M);

				pc_Data = (char *) pst_Evt->p_Data + 4;
				w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Evt);
				if(w_Type & EVE_InterKeyType_Rotation_Quaternion)
				{
					pc_DataNew = (char *) pst_New->p_Data + 4;

					if(w_Type & EVE_InterKey_CompressedQuaternion)
					{
						EVE_Event_InterpolationKey_GetCompressedQuaternion(pst_Evt, &CQ);
						((MATH_tdst_CompressedQuaternion *) pc_DataNew)->x = CQ.x;
						((MATH_tdst_CompressedQuaternion *) pc_DataNew)->y = CQ.y;
						((MATH_tdst_CompressedQuaternion *) pc_DataNew)->z = CQ.z;
					}
					{
						EVE_Event_InterpolationKey_GetQuaternion(pst_Evt, &Q);
						MATH_CopyQuaternion(((MATH_tdst_Quaternion *) pc_DataNew), &Q);
					}
				}
			}
			else
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	*V, *W, Pos;
				short				w_Type;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				EVE_Event_InterpolationKey_WhenInsert(pst_GO, mpst_ListTracks, _pst_Track, pst_New);

				w_Type = EVE_w_Event_InterpolationKey_GetType(pst_New);
				if((w_Type & EVE_InterKeyType_Translation_Mask) == EVE_InterKeyType_Translation_2)
				{
					V = EVE_pst_Event_InterpolationKey_GetPos(pst_Evt);
					W = EVE_pst_Event_InterpolationKey_GetPos(pst_New);
					MATH_CopyVector(W + 2, V + 2);
					MATH_AddVector(&Pos, V, W);
					MATH_ScaleVector(V + 2, &Pos, 0.5f);
				}
			}
		}
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SplitComputeWidth
(
	EVE_tdst_Track	*_pst_Track,
	EVE_tdst_Params *_pst_Params,
	int				index,
	CPoint			pt,
	EEVE_cl_Event	*po_Event,
	float			*pf_Width,
	float			*pf_Dif,
	BOOL			b_FirstCaseOnly
)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = _pst_Track->pst_AllEvents + index;
	if(!(po_Event) || ((_pst_Params->uw_CurrentEvent == index) && (_pst_Params->f_Time > (1.0f / 60.0f))))
	{
		*pf_Width = _pst_Params->f_Time * mst_Ini.f_Factor;
		*pf_Width /= mst_Ini.f_ValFactor;
		*pf_Dif = EVE_FrameToTime(pst_Evt->uw_NumFrames) * mst_Ini.f_Factor;
		*pf_Dif /= mst_Ini.f_ValFactor;
		*pf_Dif -= *pf_Width;
	}
	else
	{
		if(b_FirstCaseOnly) return;

		pt.x -= po_Event->mo_Rect.left;
		*pf_Width = (float) pt.x;
		*pf_Dif = po_Event->mo_Rect.Width() -*pf_Width;
		*pf_Dif += EEVE_C_CXBorder;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SplitKey(CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect			o_Rect;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	EVE_tdst_Params *pst_Param;
	float			f_Width;
	float			f_Dif;
	int				index;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for(index = 0; index < mpst_Data->pst_ListTracks->uw_NumTracks; index++)
	{
		pst_Param = &mpst_Data->pst_ListParam[index];
		po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(index));
		po_Track->GetWindowRect(&o_Rect);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && !mst_Ini.i_DisplayHidden) continue;
		if(o_Rect.PtInRect(pt))
		{
			po_Track->ScreenToClient(&pt);
			if(!po_Track->HitTest(pt, &po_Event)) return;
			if(po_Event->mo_Rect.IsRectEmpty()) return;

			/* Compute delay */
			SplitComputeWidth
			(
				po_Track->mpst_Track,
				pst_Param,
				po_Event->mi_NumEvent,
				pt,
				po_Event,
				&f_Width,
				&f_Dif,
				FALSE
			);

			/* Split */
			SplitOneEvent(po_Track->mpst_Track, pst_Param, po_Event->mi_NumEvent, f_Width, f_Dif);

			/* Redraw */
			po_Track->SetTrack(po_Track->mpst_Track);
			po_Track->Invalidate();
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::Align(int _i_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Track	*po_Align;
	EVE_tdst_Params *pst_Param;
	float			f_Delay;
	float			f_Time;
	int				i, index;
	BOOL			b_First;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Track >= mpst_Data->pst_ListTracks->uw_NumTracks) return;

	/* Search track to align */
	pst_Param = &mpst_Data->pst_ListParam[_i_Track];
	po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(_i_Track));
	po_Align = po_Track;

	/* Compute absolute time */
	f_Time = 0.0f;
	for(i = 0; i < pst_Param->uw_CurrentEvent; i++)
	{
		if(po_Align->mpst_Track->pst_AllEvents)
			f_Time += EVE_FrameToTime(po_Align->mpst_Track->pst_AllEvents[i].uw_NumFrames);
	}

	f_Time += pst_Param->f_Time;

	/* Align */
	b_First = TRUE;
	for(index = 0; index < mpst_Data->pst_ListTracks->uw_NumTracks; index++)
	{
		pst_Param = &mpst_Data->pst_ListParam[index];
		if(index == mo_ListTracks.GetCount()) break;
		po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(index));

		if(EVE_gb_UseRunningPauseFlag)
		{
			if(!(pst_Param->uw_Flags & EVE_C_Track_Running)) continue;
			if(pst_Param->uw_Flags & EVE_C_Track_Pause) continue;
		}

		if(po_Track == po_Align) continue;

		/* Compute delay to actual running */
		f_Delay = 0.0f;
		for(i = 0; i < pst_Param->uw_CurrentEvent; i++)
		{
			if(po_Track->mpst_Track->pst_AllEvents)
				f_Delay += EVE_FrameToTime(po_Track->mpst_Track->pst_AllEvents[i].uw_NumFrames);
		}

		f_Delay += pst_Param->f_Time;
		if(po_Align != po_Track) po_Track->MoveRunningBox(FALSE, f_Time - f_Delay);
		if(b_First)
		{
			ForceRefreshReal();
			b_First = FALSE;
		}
	}

	/* Hierarchy */
	if(mpst_GAO) OBJ_HierarchyMainCall(mpst_GAO->pst_World);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::RecomputeWidthFromCur(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		for(i = 0; i < po_Track->mo_ListEvents.GetSize(); i++)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(i);
			po_Event->mf_Width = po_Event->mf_CurWidth;
			if(po_Event->mf_Width == 0.0f)
				po_Event->mb_Empty = TRUE;
			else
				po_Event->mb_Empty = FALSE;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SetLinkedOutputEditors(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_NumberOfLinkedOutputEditors = 0;
	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		/* Test all views, and all editors */
		po_View = M_MF()->po_GetViewWithNum(i);
		for(j = 0; j < M_CurNumEd(po_View->mi_NumPane); j++)
		{
			po_Editor = M_CurEd(po_View->mi_NumPane, j).po_Editor;
			if(po_Editor && po_Editor->b_CanActivate())
			{
				if(mst_BaseIni.ab_LinkTo[po_Editor->mi_PosInGArray][po_Editor->mi_NumEdit])
				{
					if(po_Editor->mst_Def.i_Type == EDI_IDEDIT_OUTPUT)
						mapo_LinkedOutputEditor[mi_NumberOfLinkedOutputEditors++] = (EOUT_cl_Frame *) po_Editor;
				}
			}
		}
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::MoveReal(int _i_Sens)
{
	/*~~~~~~~~~~*/
	float	f_Val;
	/*~~~~~~~~~~*/

	if(!mpst_Data) return;

	if(GetAsyncKeyState(VK_SHIFT) < 0)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EVE_tdst_Params *pst_Param;
		EVE_tdst_Track	*pst_Track;
		EVE_tdst_Event	*pst_Event;
		int				index, index2;
		float			f_Min;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		f_Val = 0.0f;
		f_Min = 0.0f;
		index = 0;
		while(index < mpst_Data->pst_ListTracks->uw_NumTracks)
		{
			pst_Track = &mpst_Data->pst_ListTracks->pst_AllTracks[index];
			if(!(pst_Track->uw_Flags & EVE_C_Track_Selected))
			{
				index++;
				continue;
			}

			pst_Param = &mpst_Data->pst_ListParam[index];
			for(index2 = 0; index2 < pst_Track->uw_NumEvents; index2++)
			{
				pst_Event = &pst_Track->pst_AllEvents[index2];

				if((_i_Sens > 0) && (index2 == pst_Param->uw_CurrentEvent))
				{
					f_Min = EVE_FrameToTime(pst_Event->uw_NumFrames) - pst_Param->f_Time;
					if(!f_Val || (f_Min < f_Val)) f_Val = f_Min;
				}

				if((_i_Sens < 0) && (pst_Param->f_Time >= 0.0001f) && (index2 == pst_Param->uw_CurrentEvent))
				{
					f_Min = pst_Param->f_Time;
					if(!f_Val || (f_Min < f_Val)) f_Val = f_Min;
				}

				if((_i_Sens < 0) && (pst_Param->f_Time < 0.0001f) && (index2 + 1 == pst_Param->uw_CurrentEvent))
				{
					f_Min = EVE_FrameToTime(pst_Event->uw_NumFrames);
					if(!f_Val || (f_Min < f_Val)) f_Val = f_Min;
				}
			}

			index++;
		}
	}
	else
	{
		if(mst_Ini.i_UnitMode & EEVE_C_UnitMode160)
		{
			f_Val = 1.0f / 60.0f;
		}
		else if(mst_Ini.i_UnitMode & EEVE_C_UnitModeAuto)
		{
			f_Val = mst_Ini.f_ValFactor / mst_Ini.f_DivFactor;
		}
		else
		{
			f_Val = mst_Ini.f_RealUnit;
			if(f_Val > 1.0f) f_Val = 1.0f;
		}
	}

	f_Val *= (float) _i_Sens;
	MoveRunningBox(NULL, f_Val, TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::OnRealIdle(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	static int	i_LastTime = 0;
	static int	i_First = 1;
	static int	i_Delay = 300;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Auto play */
	if(mb_Play)
	{
		Play();
		return;
	}

	if(!mb_IsActivate) return;
	if((GetAsyncKeyState(VK_LEFT) < 0) && (GetAsyncKeyState(VK_CONTROL) >= 0))
	{
		if(i_First)
		{
			if(!mst_Ini.b_AnimMode) M_MF()->LockDisplay(this);
			UnselectAll(FALSE);
			MoveReal(-1);
			i_LastTime = timeGetTime();
			i_Delay = 250;
			i_First = 0;
			SelectCurrentEvents(TRUE);
			if(!mst_Ini.b_AnimMode) M_MF()->UnlockDisplay(this);
		}
		else if(timeGetTime() - i_LastTime > (ULONG) i_Delay)
		{
			if(!mst_Ini.b_AnimMode) M_MF()->LockDisplay(this);
			UnselectAll(FALSE);
			MoveReal(-1);
			i_LastTime = timeGetTime();
			i_Delay = 0;
			SelectCurrentEvents(TRUE);
			if(!mst_Ini.b_AnimMode) M_MF()->UnlockDisplay(this);
		}

		return;
	}

	if((GetAsyncKeyState(VK_RIGHT) < 0) && (GetAsyncKeyState(VK_CONTROL) >= 0))
	{
		if(i_First)
		{
			if(!mst_Ini.b_AnimMode) M_MF()->LockDisplay(this);
			UnselectAll(FALSE);
			MoveReal(1);
			i_LastTime = timeGetTime();
			i_Delay = 250;
			i_First = 0;
			SelectCurrentEvents(TRUE);
			if(!mst_Ini.b_AnimMode) M_MF()->UnlockDisplay(this);
		}
		else if(timeGetTime() - i_LastTime > (ULONG) i_Delay)
		{
			if(!mst_Ini.b_AnimMode) M_MF()->LockDisplay(this);
			UnselectAll(FALSE);
			MoveReal(1);
			i_LastTime = timeGetTime();
			i_Delay = 0;
			SelectCurrentEvents(TRUE);
			if(!mst_Ini.b_AnimMode) M_MF()->UnlockDisplay(this);
		}

		return;
	}

	i_First = 1;
	i_LastTime = timeGetTime();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::OptimizeAnim(void)
{
	EVE_MysticalOptimization(NULL, mpst_ListTracks, (float) 0.0f, (float) 0.0f);
	EVE_ReinitData(mpst_Data);
	SetGAO(mpst_GAO);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::RoundTo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	float			f_Round;
	EEVE_cl_Track	*po_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	switch(mst_Ini.i_SnapMode)
	{
	case EEVE_C_SnapMode160:
	default:
		f_Round = 1.0f / 60.0f;
		break;

	case EEVE_C_SnapModeAuto:
		f_Round = mst_Ini.f_ValFactor / mst_Ini.f_DivFactor;
		break;

	case EEVE_C_SnapModeUser:
		f_Round = mst_Ini.f_SnapUnit;
		break;
	}

	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		po_Track->RoundTo(f_Round);
	}

	ForceRefresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::Force0(BOOL _b_Dec)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EEVE_cl_Event	*po_Event;
	EEVE_cl_Event	*po_Event1;
	int				index, index1;
	BOOL			b_Del;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SaveForUndo();
	pos = mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mo_ListTracks.GetNext(pos);
		b_Del = FALSE;
		for(index = po_Track->mo_ListEvents.GetSize() - 1; index >= 0; index--)
		{
			po_Event = po_Track->mo_ListEvents.GetAt(index);
			if(po_Event->pst_FindEngineEvent()->w_Flags & EVE_C_EventFlag_Selected)
			{
				/* Shift right */
				if(!_b_Dec)
				{
					index1 = index + 1;
					while(index1 < po_Track->mo_ListEvents.GetSize())
					{
						po_Event1 = po_Track->mo_ListEvents.GetAt(index1);
						if(po_Event1->pst_FindEngineEvent()->uw_NumFrames)
						{
							po_Event1->pst_FindEngineEvent()->uw_NumFrames += po_Event->pst_FindEngineEvent()->uw_NumFrames;
							po_Event1->RecomputeWidth();
							break;
						}

						index1++;
					}
				}

				po_Event->pst_FindEngineEvent()->uw_NumFrames = 0;
				po_Event->RecomputeWidth();
				b_Del = TRUE;
			}
		}

		if(b_Del) po_Track->Invalidate();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::Force1(void)
{
	SaveForUndo();
	SetDelaySelected(mst_Ini.f_ValFactor / mst_Ini.f_DivFactor);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::AfterEngine(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	float			f_Time;
	int				index;
	EVE_tdst_Params *pst_Param;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_GAO) return;
	if(!mpst_ListTracks) return;
	if(!mpst_Data) return;
	if(mpst_Data->pst_ListTracks != mpst_ListTracks) SetGAO(mpst_GAO);
	if(!mpst_Data || !mpst_Data->pst_ListParam) return;

	for(index = 0; index < mpst_Data->pst_ListTracks->uw_NumTracks; index++)
	{
		if(index >= mo_ListTracks.GetCount()) break;
		pst_Param = &mpst_Data->pst_ListParam[index];
		if(pst_Param->uw_Flags & EVE_C_Track_Running) goto ok;
	}

	return;

ok:
	mb_Play = FALSE;
	if(mst_Ini.i_SnapMode == EEVE_C_SnapMode160)
	{
		f_Time = mpst_Data->pst_ListParam->f_Time;
		f_Time = MATH_f_FloatModulo(f_Time, 1.0f / 60.0f);
		if(f_Time - mpst_Data->pst_ListParam->f_Time)
			MoveRunningBox(NULL, f_Time - mpst_Data->pst_ListParam->f_Time, TRUE);
	}

	MoveRunningBox(NULL, 0.0f, TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::GotoOrigin(void)
{
	/*~~~~~~~~~~*/
	float	f_Tot;
	/*~~~~~~~~~~*/

	if(!mpst_GAO) return;
	if(!mpst_ListTracks) return;
	if(!mpst_Data) return;

	f_Tot = mpst_Data->pst_ListParam->f_Time;
	for(int i = mpst_Data->pst_ListParam->uw_CurrentEvent - 1; i >= 0; i--)
	{
		if(mpst_Data->pst_ListTracks->pst_AllTracks[0].pst_AllEvents)
			f_Tot += EVE_FrameToTime(mpst_Data->pst_ListTracks->pst_AllTracks[0].pst_AllEvents[i].uw_NumFrames);
	}

	LINK_gb_AllRefreshEnable = FALSE;
	MoveRunningBox(NULL, -f_Tot, TRUE);
	UnselectAll(FALSE);
	SelectCurrentEvents(TRUE);
	MoveRunningBox(NULL, 0.0f, TRUE);
	LINK_gb_AllRefreshEnable = TRUE;
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::GotoEnd(void)
{
	/*~~*/
	int i;
	/*~~*/

	if(!mpst_GAO) return;
	if(!mpst_ListTracks) return;
	if(!mpst_Data) return;

	for(i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
	{
		mpst_Data->pst_ListParam[i].uw_CurrentEvent = mpst_Data->pst_ListTracks->pst_AllTracks[i].uw_NumEvents - 1;
		mpst_Data->pst_ListParam[i].f_Time = 0.0f;
	}

	LINK_gb_AllRefreshEnable = FALSE;
	MoveRunningBox(NULL, 0.0f, TRUE);
	UnselectAll(FALSE);
	SelectCurrentEvents(TRUE);
	MoveRunningBox(NULL, 0.0f, TRUE);
	LINK_gb_AllRefreshEnable = TRUE;
	LINK_Refresh();
}

#ifdef JADEFUSION
extern void s_HandleWinMessages(void);
#else
extern "C" void s_HandleWinMessages(void);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::Play(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f;
	int					b;
	int					i_ActCycle;
	MATH_tdst_Matrix	st_Mat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b = LINK_gi_SpeedDraw;
	LINK_gi_SpeedDraw = 1;

	i_ActCycle = 0;
	L_memcpy(&st_Mat, OBJ_pst_GetAbsoluteMatrix(mpst_GAO), sizeof(MATH_tdst_Matrix));

	for(;;)
	{
		if(!mb_Play) break;
		if(ENG_gb_EngineRunning) break;

		s_HandleWinMessages();
		TIM_Clock_Update();

		EVE_gpst_OwnerGAO = mpst_GAO;
		EVE_gpst_CurrentGAO = mpst_GAO;
		EVE_gpst_CurrentData = mpst_Data;
		EVE_gpst_CurrentListTracks = mpst_ListTracks;
		EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(mpst_GAO);

		/* Contracted ? */
		mpst_Data->pst_ListParam[1].uw_Flags &= ~EVE_C_Track_Running;
		if(!mi_PlayContract)
		{
			if(mpst_Data->pst_ListTracks->pst_AllTracks[1].uw_Flags & EVE_C_Track_RunningInit)
				mpst_Data->pst_ListParam[1].uw_Flags |= EVE_C_Track_Running;
		}

		if(mi_NumCycles == 1) EVE_gb_CanFlash = FALSE;

		TIM_gf_dt *= ((float) mst_Ini.i_PlayHz / 60.0f);
		f = TIM_gf_dt;
recom:
		TIM_gf_dt = f;
		if(EVE_b_RunAllTracks(mpst_Data, &f))
		{
			if(mpst_Data->pst_ListParam->f_Time == 0.0f)
			{
				i_ActCycle++;
				if(i_ActCycle == mi_NumCycles)
				{
					L_memcpy(OBJ_pst_GetAbsoluteMatrix(mpst_GAO), &st_Mat, sizeof(MATH_tdst_Matrix));
					EVE_FlashMatrix();
					i_ActCycle = 0;
				}
			}

			goto recom;
		}

		OBJ_HierarchyMainCall(mpst_GAO->pst_World);

		LINK_Refresh();
	}

	EVE_gb_CanFlash = TRUE;
	LINK_gi_SpeedDraw = b;
	AfterEngine();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EEVE_cl_Frame::CenterSel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i, j, ok;
	int				ii;
	float			f, f1;
	EEVE_cl_Inside	*po_Inside;
	CRect			o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Inside = ((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1));

	/* Get first selected event */
	ok = 0;
	for(i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
	{
		for(j = 0; j < mpst_ListTracks->pst_AllTracks[i].uw_NumEvents; j++)
		{
			if(mpst_ListTracks->pst_AllTracks[i].pst_AllEvents)
			{
				if(mpst_ListTracks->pst_AllTracks[i].pst_AllEvents[j].w_Flags & EVE_C_EventFlag_Selected)
				{
					ok = 1;
					goto yes;
				}
			}
		}
	}

yes:
	if(ok)
	{
		M_MF()->LockDisplay(this);
		if(mst_Ini.i_AllEventsVis)
		{
			f1 = mst_Ini.f_ValFactor / mst_Ini.f_DivFactor;
			f1 *= mst_Ini.f_Factor;
			f1 /= mst_Ini.f_ValFactor;
			f = f1 * j;
			po_Inside->mpo_Scroll->GetClientRect(&o_Rect);
			f -= o_Rect.Width() / 2;
			f += f1 / 2;
			po_Inside->mpo_Scroll->SetScrollPos(SB_HORZ, (int) f);
			po_Inside->mpo_Scroll->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + ((int) f << 16), 0);
		}
		else
		{
			f = 0;
			for(ii = 0; ii < j; ii++)
				f += (float) mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i))->mo_ListEvents.GetAt(ii)->mf_Width;
			f += ((float) mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i))->mo_ListEvents.GetAt(j)->mf_Width / 2);
			po_Inside->mpo_Scroll->GetClientRect(&o_Rect);
			f -= o_Rect.Width() / 2;
			po_Inside->mpo_Scroll->SetScrollPos(SB_HORZ, (int) f);
			po_Inside->mpo_Scroll->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + ((int) f << 16), 0);
		}

		ForceRefresh();
		M_MF()->UnlockDisplay(this);
	}

	return ok;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SwapAllVis(void)
{
	M_MF()->LockDisplay(this);

	mst_Ini.i_AllEventsVis = mst_Ini.i_AllEventsVis ? 0 : 1;
	mpo_MainSplitter->GetPane(0, 1)->Invalidate();
	SetFactor(FALSE);
	ForceRefresh();
	CenterSel();

	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::AddTimeAll(BOOL _b_Inc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT				uw_Current, uw_OldNumFrame;
	EVE_tdst_Track		*pst_Track;
	EVE_tdst_Event		*pst_Event;
	EEVE_cl_Track		*po_Track;
	float				f_Delay;
	EDIA_cl_NameDialog	o_Dialog("Enter delay (in SECONDS)");
	EDIA_cl_NameDialog	o_Dialog1("Enter delay (in FRAMES)");
	float				f_Time, f_Comp, f_Zoom;
	int					i;
	int					frm, res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mst_Ini.i_TimeDisplay == EEVE_C_OptionTimeSec)
		res = o_Dialog.DoModal();
	else
		res = o_Dialog1.DoModal();

	if(res == IDOK)
	{
		/* Compute absolute time */
		po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(0));
		f_Time = 0.0f;
		for(i = 0; i < mpst_Data->pst_ListParam[0].uw_CurrentEvent; i++)
		{
			if(po_Track->mpst_Track->pst_AllEvents)
				f_Time += EVE_FrameToTime(po_Track->mpst_Track->pst_AllEvents[i].uw_NumFrames);
		}

		f_Time += mpst_Data->pst_ListParam[0].f_Time;

		if(mst_Ini.i_TimeDisplay == EEVE_C_OptionTimeSec)
			f_Delay = (float) L_atof((char *) (LPCSTR) o_Dialog.mo_Name);
		else
		{
			f_Delay = (float) L_atof((char *) (LPCSTR) o_Dialog1.mo_Name);
			f_Delay *= 1.0f / 60.0f;
		}

		for(i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
		{
			po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(i));
			if(!po_Track->mpst_Track) continue;
			if(!po_Track->mpst_Track->pst_AllEvents) continue;
			f_Comp = 0.0f;
			uw_Current = 0;
			while(f_Comp + EVE_FrameToTime(po_Track->mpst_Track->pst_AllEvents[uw_Current].uw_NumFrames) < f_Time)
			{
				if(uw_Current == po_Track->mpst_Track->uw_NumEvents) goto zap;
				f_Comp += EVE_FrameToTime(po_Track->mpst_Track->pst_AllEvents[uw_Current].uw_NumFrames);
				uw_Current++;
			}

			pst_Track = &mpst_ListTracks->pst_AllTracks[i];
			pst_Event = &pst_Track->pst_AllEvents[uw_Current];
			uw_OldNumFrame = pst_Event->uw_NumFrames;
			if(_b_Inc)
				pst_Event->uw_NumFrames += EVE_TimeToFrame(f_Delay);
			else
			{
				frm = EVE_TimeToFrame(f_Delay);
				if(frm > pst_Event->uw_NumFrames)
					pst_Event->uw_NumFrames = 0;
				else
					pst_Event->uw_NumFrames -= frm;
			}

			if(((pst_Event->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey) && (uw_OldNumFrame))
			{
				f_Zoom = (float) pst_Event->uw_NumFrames / (float) uw_OldNumFrame;
				EVE_Event_InterpolationKey_ZoomTimeInterpolation(pst_Event, f_Zoom);
			}

			po_Track->SetTrack(pst_Track);
			po_Track->Invalidate();
zap: ;
		}

		Align(0);
		LINK_Refresh();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::CenterToRealTime(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Track	*po_Track;
	int				x;
	EEVE_cl_Inside	*po_Inside;
	CRect			o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(0));
	x = po_Track->mo_TrackTime.left;

	po_Inside = ((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1));
	po_Inside->mpo_Scroll->GetClientRect(&o_Rect);
	x -= o_Rect.Width() / 2;
	if(x < 0) x = 0;
	po_Inside->mpo_Scroll->SetScrollPos(SB_HORZ, x);
	po_Inside->mpo_Scroll->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (x << 16), 0);
	ForceRefresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MorphEditDialog *EEVE_cl_Frame::OpenMorphEditor( struct MDF_tdst_Modifier_ *_pst_Modif, OBJ_tdst_GameObject *_pst_GO )
{
	if (!_pst_Modif || !_pst_GO) return NULL;
	if (mo_MorphEditor != NULL) CloseMorphEditor();
	mo_MorphEditor = new EDIA_cl_MorphEditDialog(this, _pst_Modif, _pst_GO, (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(_pst_GO));
	mo_MorphEditor->DoModeless();
	return mo_MorphEditor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::CloseMorphEditor()
{
	if (mo_MorphEditor == NULL) return;
	delete mo_MorphEditor;
	mo_MorphEditor = NULL;
}

#endif /* ACTIVE_EDITORS */
