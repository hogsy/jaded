/*$T EVEframe_act.cpp GC 1.139 03/17/04 14:02:38 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EVEframe.h"
#include "EVEframe_act.h"
#include "EVEinside.h"
#include "EVEscroll.h"
#include "EDImainframe.h"
#include "Res/Res.h"
#include "AIinterp/Sources/Events/EVEsave.h"
#include "DIAlogs/DIAname_dlg.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ENGvars.h"
#include "LINKs/LINKtoed.h"
#include "OUTput/OUTframe.h"
#include "OUTput/OUTmsg.h"
#include "SouND/Sources/SNDload.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::ComputeZoom(int sens, BOOL forceleft)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				A, B;
	int				sx;
	float			f;
	CPoint			pt;
	float			time;
	EEVE_cl_Inside	*po_Inside;
	CRect			rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	f = mst_Ini.f_ValFactor / mst_Ini.f_DivFactor;
	f /= mst_Ini.i_Inter;
	f = MATH_f_FloatModulo(f, 1.0f / 60.0f);
	if(f == 0) f = 1.0f / 60.0f;
	A = B = (int) ((mst_Ini.f_ValFactor / mst_Ini.f_DivFactor) / f);
	if(A)
	{
		while(A + B <= mst_Ini.i_Inter) A += B;
	}

	if(sens == 1)
	{
		mst_Ini.i_Inter = A + B;
	}
	else
	{
		mst_Ini.i_Inter = A - B;
	}

	po_Inside = ((EEVE_cl_Inside *) mpo_MainSplitter->GetPane(0, 1));

	if(forceleft)
	{
		M_MF()->LockDisplay(this);
		sx = po_Inside->PtToFrame(CPoint(mst_Ini.i_XLeft, 0));
		SetFactor(FALSE);
		time = sx / 60.0f;
		time *= mst_Ini.f_Factor;
		time /= mst_Ini.f_ValFactor;
		sx = (UINT) time;
		if(sx < 0) sx = 0;
		po_Inside->mpo_Scroll->SetScrollPos(SB_HORZ, sx);
		po_Inside->mpo_Scroll->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (sx << 16), 0);
		ForceRefresh();
		M_MF()->UnlockDisplay(this);
	}
	else
	{
		M_MF()->LockDisplay(this);
		po_Inside->GetClientRect(&rect);
		sx = po_Inside->PtToFrame(CPoint(rect.right, 0));
		SetFactor(FALSE);
		time = sx / 60.0f;
		time *= mst_Ini.f_Factor;
		time /= mst_Ini.f_ValFactor;
		sx = (UINT) time;
		sx -= rect.Width();
		sx += mst_Ini.i_XLeft;
		if(sx < 0) sx = 0;
		po_Inside->mpo_Scroll->SetScrollPos(SB_HORZ, sx);
		po_Inside->mpo_Scroll->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (sx << 16), 0);
		ForceRefresh();
		M_MF()->UnlockDisplay(this);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::OnAction(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog("Enter unit in ms");
	char				asz_Unit[64];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_ul_Action)
	{

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_ACTION_LOCK:
		mst_Ini.b_Lock = mst_Ini.b_Lock ? FALSE : TRUE;
		break;

	case EEVE_ACTION_NEWTRACK:
		SaveForUndo();
		OnNewTrack();
		break;

	case EEVE_ACTION_RUNALL:
		OnRunAll(TRUE);
		break;

	case EEVE_ACTION_NORUNALL:
		OnRunAll(FALSE);
		break;

	case EEVE_ACTION_RUNALLI:
		OnRunAll(TRUE, TRUE);
		break;

	case EEVE_ACTION_GENTRE:
		SaveTre(TRUE);
		break;

	case EEVE_ACTION_NORUNALLI:
		OnRunAll(FALSE, TRUE);
		break;

	case EEVE_ACTION_RUNALLIS:
		OnRunAll(TRUE, TRUE, TRUE);
		break;

	case EEVE_ACTION_NORUNALLIS:
		OnRunAll(FALSE, TRUE, TRUE);
		break;

	case EEVE_ACTION_CLOSEALL:
		CloseAll();
		break;

	case EEVE_ACTION_SAVEALL:
		EVE_SaveListTracks(mpst_GAO, mpst_ListTracks);
		SaveTre();
		M_MF()->FlashJade();
		break;

	case EEVE_ACTION_SIMPLECHECK:
		EVE_CheckListTracks(mpst_GAO, mpst_ListTracks);
		break;

	case EEVE_ACTION_FORCESNAP:
		mst_Ini.i_ForceSnap = mst_Ini.i_ForceSnap ? 0 : 1;
		break;

	case EEVE_ACTION_BIGZOOMIN:
		if(mst_Ini.i_AllEventsVis)
		{
			mst_Ini.f_WidthFixed += mst_Ini.f_WidthFixed >= 300 ? 0 : 50;
			M_MF()->LockDisplay(this);
			SetFactor(FALSE);
			M_MF()->UnlockDisplay(this);
		}
		else
			ComputeZoom(1);
		break;

	case EEVE_ACTION_BIGZOOMINP:
		if(mst_Ini.i_AllEventsVis)
		{
			mst_Ini.f_WidthFixed += mst_Ini.f_WidthFixed >= 300 ? 0 : 50;
			M_MF()->LockDisplay(this);
			SetFactor(FALSE);
			M_MF()->UnlockDisplay(this);
		}
		else
			ComputeZoom(1, FALSE);
		break;

	case EEVE_ACTION_BIGZOOMOUT:
		if(mst_Ini.i_AllEventsVis)
		{
			mst_Ini.f_WidthFixed -= mst_Ini.f_WidthFixed <= 50 ? 0 : 50;
			M_MF()->LockDisplay(this);
			SetFactor(FALSE);
			M_MF()->UnlockDisplay(this);
		}
		else
			ComputeZoom(-1);
		break;

	case EEVE_ACTION_BIGZOOMOUTP:
		if(mst_Ini.i_AllEventsVis)
		{
			mst_Ini.f_WidthFixed -= mst_Ini.f_WidthFixed <= 50 ? 0 : 50;
			M_MF()->LockDisplay(this);
			SetFactor(FALSE);
			M_MF()->UnlockDisplay(this);
		}
		else
			ComputeZoom(-1, FALSE);
		break;

	case EEVE_ACTION_CENTERREAL:
		CenterToRealTime();
		break;

	case EEVE_ACTION_MOVETRACKUP:
		SaveForUndo();
		MoveTrack(TRUE);
		break;

	case EEVE_ACTION_MOVETRACKDOWN:
		SaveForUndo();
		MoveTrack(FALSE);
		break;

	case EEVE_ACTION_UNDO:
		Undo();
		break;

	case EEVE_ACTION_INSERTTIME:
		AddTimeAll(TRUE);
		break;

	case EEVE_ACTION_REMOVETIME:
		AddTimeAll(FALSE);
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_ACTION_DISPLSEC:
		mst_Ini.i_TimeDisplay = EEVE_C_OptionTimeSec;
		ForceRefresh();
		break;

	case EEVE_ACTION_DISPLCNT:
		mst_Ini.i_TimeDisplay = EEVE_C_OptionTimeFrame;
		ForceRefresh();
		break;

	case EEVE_ACTION_DISPLHIDDEN:
		mst_Ini.i_DisplayHidden = mst_Ini.i_DisplayHidden ? 0 : 1;
		ForceRefresh();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_ACTION_UNIT100MS:
		mst_Ini.f_ValFactor = 0.100f;
		goto com;

	case EEVE_ACTION_UNIT1S:
		mst_Ini.f_ValFactor = 1.0f;
		goto com;

	case EEVE_ACTION_UNIT2S:
		mst_Ini.f_ValFactor = 2.0f;
		goto com;

	case EEVE_ACTION_UNIT3S:
		mst_Ini.f_ValFactor = 3.0f;
		goto com;

	case EEVE_ACTION_UNIT5S:
		mst_Ini.f_ValFactor = 5.0f;
		goto com;

	case EEVE_ACTION_UNIT6S:
		mst_Ini.f_ValFactor = 6.0f;
		goto com;

	case EEVE_ACTION_UNIT10S:
		mst_Ini.f_ValFactor = 10.0f;
		goto com;

	case EEVE_ACTION_UNIT15S:
		mst_Ini.f_ValFactor = 15.0f;
		goto com;

	case EEVE_ACTION_UNIT20S:
		mst_Ini.f_ValFactor = 20.0f;
		goto com;

	case EEVE_ACTION_UNIT30S:
		mst_Ini.f_ValFactor = 30.0f;
		goto com;

	case EEVE_ACTION_UNIT60S:
		mst_Ini.f_ValFactor = 60.0f;
com:
		M_MF()->LockDisplay(this);
		SetFactor();
		if(!CenterSel()) CenterToRealTime();
		M_MF()->UnlockDisplay(this);
		break;

	case EEVE_ACTION_ALLEVENTS:
		SwapAllVis();
		break;

	case EEVE_ACTION_DISPNAMES:
		mst_Ini.i_DisplayTrackName = mst_Ini.i_DisplayTrackName ? 0 : 1;
		ForceRefresh();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_ACTION_ALIGN:
		Align(0);
		break;

	case EEVE_ACTION_AUTOALIGN:
		mst_Ini.i_AutoAlign = mst_Ini.i_AutoAlign ? 0 : 1;
		break;

	case EEVE_ACTION_LINKACTION:
		mst_Ini.i_LinkAction = mst_Ini.i_LinkAction ? 0 : 1;
		break;

	case EEVE_ACTION_RUNSEL:
		mst_Ini.b_RunSel = mst_Ini.b_RunSel ? FALSE : TRUE;
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_ACTION_SETREALUNIT:
		mst_Ini.f_RealUnit = (float) fabs(mst_Ini.f_RealUnit);
		if(mst_Ini.f_RealUnit > 1.0f) mst_Ini.f_RealUnit = 1.0f;
		sprintf(asz_Unit, "%f", mst_Ini.f_RealUnit);
		o_Dialog.mo_Name = asz_Unit;
		if(o_Dialog.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dialog.mo_Name, "%f", &mst_Ini.f_RealUnit);
			mst_Ini.f_RealUnit = (float) fabs(mst_Ini.f_RealUnit);
			if(mst_Ini.f_RealUnit > 1.0f) mst_Ini.f_RealUnit = 1.0f;
		}
		break;

	case EEVE_ACTION_REALUNIT1:
		if(mst_Ini.i_UnitMode & EEVE_C_UnitMode160)
			mst_Ini.i_UnitMode &= ~EEVE_C_UnitMode160;
		else
			mst_Ini.i_UnitMode = EEVE_C_UnitMode160;
		break;

	case EEVE_ACTION_REALUNITI:
		if(mst_Ini.i_UnitMode & EEVE_C_UnitModeAuto)
			mst_Ini.i_UnitMode &= ~EEVE_C_UnitModeAuto;
		else
			mst_Ini.i_UnitMode = EEVE_C_UnitModeAuto;
		break;

	case EEVE_ACTION_REALUNITU:
		if(mst_Ini.i_UnitMode & EEVE_C_UnitModeUser)
			mst_Ini.i_UnitMode &= ~EEVE_C_UnitModeUser;
		else
			mst_Ini.i_UnitMode = EEVE_C_UnitModeUser;
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_ACTION_SETSNAPUNIT:
		mst_Ini.f_SnapUnit = (float) fabs(mst_Ini.f_SnapUnit);
		if(mst_Ini.f_SnapUnit > 1.0f) mst_Ini.f_SnapUnit = 1.0f;
		sprintf(asz_Unit, "%f", mst_Ini.f_SnapUnit);
		o_Dialog.mo_Name = asz_Unit;
		if(o_Dialog.DoModal() == IDOK)
		{
			sscanf((char *) (LPCSTR) o_Dialog.mo_Name, "%f", &mst_Ini.f_SnapUnit);
			mst_Ini.f_SnapUnit = (float) fabs(mst_Ini.f_SnapUnit);
			if(mst_Ini.f_SnapUnit > 1.0f) mst_Ini.f_SnapUnit = 1.0f;
		}
		break;

	case EEVE_ACTION_SNAPUNIT1:
		if(mst_Ini.i_SnapMode & EEVE_C_SnapMode160)
			mst_Ini.i_SnapMode &= ~EEVE_C_SnapMode160;
		else
			mst_Ini.i_SnapMode = EEVE_C_SnapMode160;
		break;

	case EEVE_ACTION_SNAPUNITI:
		if(mst_Ini.i_SnapMode & EEVE_C_SnapModeAuto)
			mst_Ini.i_SnapMode &= ~EEVE_C_SnapModeAuto;
		else
			mst_Ini.i_SnapMode = EEVE_C_SnapModeAuto;
		break;

	case EEVE_ACTION_SNAPUNITU:
		if(mst_Ini.i_SnapMode & EEVE_C_SnapModeUser)
			mst_Ini.i_SnapMode &= ~EEVE_C_SnapModeUser;
		else
			mst_Ini.i_SnapMode = EEVE_C_SnapModeUser;
		break;

	case EEVE_ACTION_SNAPUNITT:
		if(mst_Ini.i_SnapMode & EEVE_C_SnapModeTracks)
			mst_Ini.i_SnapMode &= ~EEVE_C_SnapModeTracks;
		else
			mst_Ini.i_SnapMode = EEVE_C_SnapModeTracks;
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_ACTION_INTER5:
		mst_Ini.f_DivFactor = 5.0f;
		SetFactor();
		break;

	case EEVE_ACTION_INTER6:
		mst_Ini.f_DivFactor = 6.0f;
		SetFactor();
		break;

	case EEVE_ACTION_INTER10:
		mst_Ini.f_DivFactor = 10.0f;
		SetFactor();
		break;

	case EEVE_ACTION_INTER50:
		mst_Ini.f_DivFactor = 50.0f;
		SetFactor();
		break;

	case EEVE_ACTION_INTER60:
		mst_Ini.f_DivFactor = 60.0f;
		SetFactor();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_ACTION_DELSELECTED:
		DeleteSelected(TRUE);
		break;

	case EEVE_ACTION_DELSELECTED2:
		DeleteSelected(FALSE);
		break;

	case EEVE_ACTION_COPY:
		Copy();
		break;

	case EEVE_ACTION_FORCE01:
		Force0(FALSE);
		break;

	case EEVE_ACTION_SPLITSEL:
		SplitSelected();
		break;

	case EEVE_ACTION_FORCE0:
		Force0(TRUE);
		break;

	case EEVE_ACTION_FORCE1:
		Force1();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_ACTION_OPTANIM:
		SaveForUndo();
		OptimizeAnim();
		break;

	case EEVE_ACTION_ROUND:
		RoundTo();
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/


	case EEVE_ACTION_ANIMMODE:
		mst_Ini.b_AnimMode = mst_Ini.b_AnimMode ? FALSE : TRUE;
		if(mst_Ini.b_AnimMode)
		{
			mst_Ini.i_UnitMode = EEVE_C_UnitMode160;
			mst_Ini.f_ValFactor = 1.0f;
			mst_Ini.f_DivFactor = 60.0f;
			mst_Ini.i_SnapMode = EEVE_C_SnapMode160;
			SetFactor();
		}

		ForceRefresh();
		break;

	case EEVE_ACTION_ADDFRAMEALL:
		SaveForUndo();
		AddFrameAll(NULL, 1, FALSE);
		break;

	case EEVE_ACTION_ADDXFRAMEALL:
		SaveForUndo();
		AddFrameAll();
		break;

	case EEVE_ACTION_DUPLICATEFRAMEALL:
		SaveForUndo();
		DuplicateFrameAll();
		UnselectAll(FALSE);
		SelectCurrentEvents(TRUE);
		break;

	case EEVE_ACTION_DELKEYFRAME:
		SaveForUndo();
		DelKeyFrame();
		break;

	case EEVE_ACTION_ADDKEY:
		SaveForUndo();
		AddKey();
		break;

	case EEVE_ACTION_DELKEY:
		SaveForUndo();
		DelKey();
		break;

	case EEVE_ACTION_SHOWALLROTCURVE:
		ShowAllCurves(TRUE);
		break;

	case EEVE_ACTION_DELALLCURVE:
		DelAllCurves();
		break;

	case EEVE_ACTION_GOTOORIGIN:
		GotoOrigin();
		break;

	case EEVE_ACTION_GOTOEND:
		GotoEnd();
		break;

	case EEVE_ACTION_EXPAND2:
		SaveForUndo();
		Expand(2);
		break;

	case EEVE_ACTION_EXPAND3:
		SaveForUndo();
		Expand(3);
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_ACTION_PLAY:
		if(mb_Play)
			mb_Play = FALSE;
		else
			mb_Play = TRUE;
		break;

	case EEVE_ACTION_PLAY2CYCLES:
		if(mi_NumCycles == 2)
			mi_NumCycles = 1;
		else
		{
			mi_PlayContract = 0;
			mi_NumCycles = 2;
		}
		break;

	case EEVE_ACTION_PLAYCONTRACT:
		if(mi_PlayContract == 1)
		{
			mi_PlayContract = 0;
			if(mpst_Data->pst_ListTracks->pst_AllTracks[1].uw_Flags & EVE_C_Track_RunningInit)
				mpst_Data->pst_ListParam[1].uw_Flags |= EVE_C_Track_Running;
		}
		else
		{
			mi_PlayContract = 1;
			mi_NumCycles = 1;
			mpst_Data->pst_ListParam[1].uw_Flags &= ~EVE_C_Track_Running;
		}
		break;

	case EEVE_ACTION_PLAY60:
		mst_Ini.i_PlayHz = 60;
		break;

	case EEVE_ACTION_PLAY30:
		mst_Ini.i_PlayHz = 30;
		break;

	case EEVE_ACTION_PLAY25:
		mst_Ini.i_PlayHz = 25;
		break;

	case EEVE_ACTION_PLAY15:
		mst_Ini.i_PlayHz = 15;
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EEVE_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~*/
	UINT	ui_State;
	/*~~~~~~~~~~~~~*/

	ui_State = (UINT) - 1;
	switch(_ul_Action)
	{
	case EEVE_ACTION_LOCK:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.b_Lock) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_ANIMMODE:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.b_AnimMode) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_AUTOALIGN:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.i_AutoAlign) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_RUNSEL:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.b_RunSel) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_LINKACTION:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.i_LinkAction) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_FORCESNAP:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.i_ForceSnap) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT100MS:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 0.100f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT1S:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 1.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT2S:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 2.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT3S:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 3.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT5S:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 5.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT6S:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 6.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT10S:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 10.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT15S:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 15.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT20S:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 20.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT30S:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 30.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_UNIT60S:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_ValFactor == 60.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_INTER5:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_DivFactor == 5.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_INTER6:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_DivFactor == 6.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_INTER10:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_DivFactor == 10.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_INTER50:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_DivFactor == 50.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_INTER60:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.f_DivFactor == 60.0f) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_DISPLSEC:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_TimeDisplay == EEVE_C_OptionTimeSec) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_DISPLCNT:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_TimeDisplay == EEVE_C_OptionTimeFrame) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_DISPLHIDDEN:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.i_DisplayHidden) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_REALUNIT1:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_UnitMode & EEVE_C_UnitMode160) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_REALUNITI:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_UnitMode & EEVE_C_UnitModeAuto) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_REALUNITU:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_UnitMode & EEVE_C_UnitModeUser) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_SNAPUNIT1:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_SnapMode & EEVE_C_SnapMode160) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_SNAPUNITI:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_SnapMode & EEVE_C_SnapModeAuto) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_SNAPUNITU:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_SnapMode & EEVE_C_SnapModeUser) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_SNAPUNITT:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_SnapMode & EEVE_C_SnapModeTracks) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_ALLEVENTS:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.i_AllEventsVis) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_DISPNAMES:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.i_DisplayTrackName) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_PLAY2CYCLES:
		ui_State = DFCS_BUTTONRADIO;
		if(mi_NumCycles == 2) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_PLAYCONTRACT:
		ui_State = DFCS_BUTTONRADIO;
		if(mi_PlayContract == 1) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_PLAY60:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.i_PlayHz == 60) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_PLAY30:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.i_PlayHz == 30) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_PLAY25:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.i_PlayHz == 25) ui_State |= DFCS_CHECKED;
		break;

	case EEVE_ACTION_PLAY15:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.i_PlayHz == 15) ui_State |= DFCS_CHECKED;
		break;
	}

	return ui_State;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	switch(_ul_Action)
	{
	case EEVE_ACTION_UNDO:
		if(!mo_ListUndo.GetCount()) return FALSE;
		if(!mpst_ListTracks) return FALSE;
		if(!mpst_GAO) return FALSE;
		break;

	case EEVE_ACTION_UNIT100MS:
	case EEVE_ACTION_UNIT1S:
	case EEVE_ACTION_UNIT2S:
	case EEVE_ACTION_UNIT3S:
	case EEVE_ACTION_UNIT5S:
	case EEVE_ACTION_UNIT6S:
	case EEVE_ACTION_UNIT10S:
	case EEVE_ACTION_UNIT15S:
	case EEVE_ACTION_UNIT20S:
	case EEVE_ACTION_UNIT30S:
	case EEVE_ACTION_UNIT60S:
		if(mst_Ini.i_AllEventsVis) return FALSE;

	case EEVE_ACTION_MOVETRACKUP:
	case EEVE_ACTION_MOVETRACKDOWN:
	case EEVE_ACTION_BIGZOOMIN:
	case EEVE_ACTION_BIGZOOMOUT:
	case EEVE_ACTION_BIGZOOMINP:
	case EEVE_ACTION_BIGZOOMOUTP:
	case EEVE_ACTION_CENTERREAL:
	case EEVE_ACTION_FORCE01:
	case EEVE_ACTION_FORCE0:
	case EEVE_ACTION_FORCE1:
	case EEVE_ACTION_DISPNAMES:
	case EEVE_ACTION_SNAPUNITT:
	case EEVE_ACTION_ROUND:
	case EEVE_ACTION_ALLEVENTS:
	case EEVE_ACTION_AUTOALIGN:
	case EEVE_ACTION_LINKACTION:
	case EEVE_ACTION_NEWTRACK:
	case EEVE_ACTION_GENTRE:
	case EEVE_ACTION_RUNALL:
	case EEVE_ACTION_NORUNALL:
	case EEVE_ACTION_RUNALLI:
	case EEVE_ACTION_NORUNALLI:
	case EEVE_ACTION_RUNALLIS:
	case EEVE_ACTION_NORUNALLIS:
	case EEVE_ACTION_CLOSEALL:
	case EEVE_ACTION_SAVEALL:
	case EEVE_ACTION_SIMPLECHECK:
	case EEVE_ACTION_FORCESNAP:
	case EEVE_ACTION_DISPLSEC:
	case EEVE_ACTION_DISPLCNT:
	case EEVE_ACTION_DISPLHIDDEN:
	case EEVE_ACTION_ALIGN:
	case EEVE_ACTION_INTER5:
	case EEVE_ACTION_INTER6:
	case EEVE_ACTION_INTER10:
	case EEVE_ACTION_INTER50:
	case EEVE_ACTION_INTER60:
	case EEVE_ACTION_DELSELECTED:
	case EEVE_ACTION_DELSELECTED2:
	case EEVE_ACTION_OPTANIM:
	case EEVE_ACTION_SETREALUNIT:
	case EEVE_ACTION_REALUNIT1:
	case EEVE_ACTION_REALUNITI:
	case EEVE_ACTION_REALUNITU:
	case EEVE_ACTION_SETSNAPUNIT:
	case EEVE_ACTION_SNAPUNIT1:
	case EEVE_ACTION_SNAPUNITI:
	case EEVE_ACTION_SNAPUNITU:
	case EEVE_ACTION_COPY:
		if(!mpst_ListTracks) return FALSE;
		if(!mpst_GAO) return FALSE;
		break;

	case EEVE_ACTION_INSERTTIME:
	case EEVE_ACTION_REMOVETIME:
		if(!mpst_ListTracks) return FALSE;
		if(!mpst_GAO) return FALSE;
		if(mst_Ini.i_AllEventsVis) return FALSE;
		break;

	case EEVE_ACTION_ADDXFRAMEALL:
	case EEVE_ACTION_ADDFRAMEALL:
	case EEVE_ACTION_DUPLICATEFRAMEALL:
	case EEVE_ACTION_DELKEYFRAME:
	case EEVE_ACTION_ADDKEY:
	case EEVE_ACTION_DELKEY:
	case EEVE_ACTION_SHOWALLROTCURVE:
	case EEVE_ACTION_DELALLCURVE:
	case EEVE_ACTION_GOTOORIGIN:
	case EEVE_ACTION_GOTOEND:
	case EEVE_ACTION_EXPAND2:
	case EEVE_ACTION_EXPAND3:
	case EEVE_ACTION_PLAY:
	case EEVE_ACTION_PLAY60:
	case EEVE_ACTION_PLAY30:
	case EEVE_ACTION_PLAY25:
	case EEVE_ACTION_PLAY15:
	case EEVE_ACTION_PLAY2CYCLES:
	case EEVE_ACTION_PLAYCONTRACT:
		if(!mpst_ListTracks) return FALSE;
		if(!mpst_GAO) return FALSE;
		if(!(mst_Ini.i_UnitMode & EEVE_C_UnitMode160)) return FALSE;
		break;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	if(!mpst_ListTracks) return FALSE;
	if
	(
		(
			(GetFocus() == &mpo_VarsView->mpo_ListBox->mo_EditSelect)
		||	(GetFocus() == mpo_VarsView->mpo_ListBox)
		||	(GetFocus() == &mpo_VarsView->mpo_ListBox->mo_ComboSelect)
		)
	)
	{
		if(M_MF()->b_EditKey(_uw_Key)) return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::MoveTrack(BOOL _b_Up)
{
	if(_b_Up)
	{
		if(mpst_ListTracks->pst_AllTracks[0].uw_Flags & EVE_C_Track_Selected) return;
		for(int i = 1; i < mpst_ListTracks->uw_NumTracks; i++)
		{
			if(mpst_ListTracks->pst_AllTracks[i].uw_Flags & EVE_C_Track_Selected)
			{
				InvertTracks(i, i - 1);
				i--;
			}
		}
	}
	else
	{
		if(mpst_ListTracks->pst_AllTracks[mpst_ListTracks->uw_NumTracks - 1].uw_Flags & EVE_C_Track_Selected) return;
		for(int i = mpst_ListTracks->uw_NumTracks - 2; i >= 0; i--)
		{
			if(mpst_ListTracks->pst_AllTracks[i].uw_Flags & EVE_C_Track_Selected)
			{
				InvertTracks(i, i + 1);
				i++;
			}
		}
	}

	SetGAO(mpst_GAO, FALSE, TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::SaveForUndo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* Remove older one */
	if(mo_ListUndo.GetCount() > 20)
	{
		pst_Data = mo_ListUndo.GetHead();
		mo_ListUndo.RemoveHead();
		if(pst_Data->pst_ListParam) MEM_Free(pst_Data->pst_ListParam);
		if(pst_Data->pst_ListTracks)
		{
			pst_Data->pst_ListTracks->ul_NbOfInstances = 1;
			EVE_DeleteListTracks(pst_Data->pst_ListTracks);
		}

		MEM_Free(pst_Data);
	}

	pst_Data = EVE_pst_DuplicateData(mpst_Data);
	mo_ListUndo.AddTail(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::CloseUndo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EVE_tdst_Data	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mo_ListUndo.GetHeadPosition();
	while(pos)
	{
		pst_Data = mo_ListUndo.GetNext(pos);
		if(pst_Data->pst_ListParam) MEM_Free(pst_Data->pst_ListParam);
		if(pst_Data->pst_ListTracks)
		{
			pst_Data->pst_ListTracks->ul_NbOfInstances = 1;
			EVE_DeleteListTracks(pst_Data->pst_ListTracks);
		}

		MEM_Free(pst_Data);
	}

	mo_ListUndo.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Frame::Undo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data			*pst_Data;
	BIG_INDEX				ul_File;
	extern BOOL				EVE_gb_CanCloseUndo;
	EOUT_tdst_CurveParam	st_Curve;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mo_ListUndo.GetCount()) return;

	/* Get animation file */
	ul_File = LOA_ul_SearchIndexWithAddress((ULONG) mpst_Data->pst_ListTracks);

	/* Copy tail */
	pst_Data = mo_ListUndo.GetTail();
	mo_ListUndo.RemoveTail();

	/* replace old curve */
	st_Curve.pst_GAO = mpst_GAO;
	st_Curve.pst_Data = mpst_Data;
	st_Curve.pst_NewData = pst_Data;
	M_MF()->SendMessageToEditors(EOUT_MESSAGE_ANIMREPLACECURVE, (ULONG) & st_Curve, 0);

	/* Free current */
	if(mpst_Data->pst_ListParam) MEM_Free(mpst_Data->pst_ListParam);
	if(mpst_Data->pst_ListTracks)
	{
		mpst_Data->pst_ListTracks->ul_NbOfInstances = 1;
		EVE_DeleteListTracks(mpst_Data->pst_ListTracks);
	}

	LOA_DeleteAddress(mpst_Data->pst_ListTracks);

	if
	(
		mpst_GAO->pst_Base
	&&	mpst_GAO->pst_Base->pst_GameObjectAnim
	&&	mpst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0]
	&&	mpst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data
	)
	{
		L_memcpy(mpst_GAO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data, pst_Data, sizeof(EVE_tdst_Data));
	}
	else if(mpst_GAO->pst_Extended && mpst_GAO->pst_Extended->pst_Events)
		L_memcpy(mpst_GAO->pst_Extended->pst_Events, pst_Data, sizeof(EVE_tdst_Data));
	else
		return;
	LOA_AddAddress(ul_File, pst_Data->pst_ListTracks);
	MEM_Free(pst_Data);

	/* Refresh */
	M_MF()->LockDisplay(this);
	EVE_gb_CanCloseUndo = FALSE;
	SetGAO(mpst_GAO, TRUE, TRUE);
	M_MF()->UnlockDisplay(this);

	LINK_UpdatePointer(mpst_GAO);
	mb_LockUpdate = TRUE;
	LINK_UpdatePointers();
	mb_LockUpdate = FALSE;
}
#endif
