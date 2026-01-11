 /*$T EVEevent.cpp GC! 1.081 10/04/00 14:21:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "AIinterp/Sources/AIengine.h"
#include "EVEevent.h"
#include "EVEtrack.h"
#include "EVEframe.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDItors/Sources/AIscript/AIframe.h"
#include "EVEmsg.h"

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Event::EEVE_cl_Event(void)
{
	mf_Width = 50.0f;
	mo_Name = "";
	mi_NumEvent = 0;
	mb_Empty = FALSE;
	mi_IndexAI = -1;
	mb_Expand = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Event::~EEVE_cl_Event(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::Draw(CDC *pdc, CRect *_po_Rect, BOOL _b_SpecialCol, BOOL _b_SpecialCol1)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	COLORREF		col;
	CRect			o_Rect;
	CFont			*pold;
	char			asz_Name[128];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_po_Rect->left == _po_Rect->right) return;
	pst_Evt = pst_FindEngineEvent();
	if(!pst_Evt) return;
	if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_Empty)
	{
		if(pst_Evt->w_Flags & EVE_C_EventFlag_Selected)
			SpeedSolidRect(pdc, _po_Rect, 0x000000FF);
		else if(_b_SpecialCol)
			SpeedSolidRect(pdc, _po_Rect, 0x00FF6060);
		else if(_b_SpecialCol1)
			SpeedSolidRect(pdc, _po_Rect, 0x000050FF);
		else
			SpeedSolidRect(pdc, _po_Rect, GetSysColor(COLOR_BTNSHADOW));
	}
	else
	{
		if(pst_Evt->w_Flags & EVE_C_EventFlag_Selected)
			col = 0x000000FF;
		else if(_b_SpecialCol)
			col = 0x00FF6060;
		else if(_b_SpecialCol1)
			col = 0x000050FF;
		else if(mi_IndexAI != -1)
			col = mpo_Track->mpo_Frame->mst_Ini.ac_ColIA[mi_IndexAI];
		else
			col = M_MF()->u4_Interpol2PackedColor(0x00FF8080, GetSysColor(COLOR_BTNFACE), 0.8f);
		SpeedSolidRect(pdc, _po_Rect, col);
	}

	/* Specific */
	pdc->SetBkMode(TRANSPARENT);
	pold = pdc->SelectObject(&M_MF()->mo_Fnt);
	o_Rect = _po_Rect;
	switch(pst_Evt->w_Flags & EVE_C_EventFlag_Type)
	{
	case EVE_C_EventFlag_AIFunction:
		DrawAIFunction(pdc, _po_Rect);
		break;
	case EVE_C_EventFlag_InterpolationKey:
		DrawInterpolationKey(pst_Evt, pdc, _po_Rect);
		break;
	case EVE_C_EventFlag_GotoLabel:
		sprintf(asz_Name, "%s %d", "Goto Label ", (pst_Evt->w_Flags & EVE_C_EventFlag_LabelNum) >> 8);
		mo_Name = asz_Name;
		pdc->ExtTextOut(_po_Rect->left + 4, _po_Rect->top + 2, ETO_CLIPPED, _po_Rect, asz_Name, NULL);
		break;
	case EVE_C_EventFlag_SetTimeToLabel:
		sprintf(asz_Name, "%s %d", "Set Time To Label ", (pst_Evt->w_Flags & EVE_C_EventFlag_LabelNum) >> 8);
		mo_Name = asz_Name;
		pdc->ExtTextOut(_po_Rect->left + 4, _po_Rect->top + 2, ETO_CLIPPED, _po_Rect, asz_Name, NULL);
		break;
	case EVE_C_EventFlag_MorphKey:
		DrawMorphKey(pst_Evt, pdc, _po_Rect);
		break;
	}

	/* 3D rect */
	pdc->SelectObject(pold);
	pdc->Draw3dRect(o_Rect, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::RecomputeDelay(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	float			f_OldDelay;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = pst_FindEngineEvent();

	f_OldDelay = EVE_FrameToTime(pst_Evt->uw_NumFrames);

	if(mf_Width <= 0.0f)
		pst_Evt->uw_NumFrames = 0;
	else
	{
		/*~~~~~~~~~~~~*/
		float	f_Delay;
		/*~~~~~~~~~~~~*/

		f_Delay = mf_Width / mpo_Track->mpo_Frame->mst_Ini.f_Factor;
		f_Delay *= mpo_Track->mpo_Frame->mst_Ini.f_ValFactor;

		pst_Evt->uw_NumFrames = EVE_TimeToFrame(f_Delay);
	}

	if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
	{
		EVE_Event_InterpolationKey_ZoomTimeInterpolation(pst_Evt, EVE_FrameToTime(pst_Evt->uw_NumFrames) / f_OldDelay);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::RecomputeTimeKey( unsigned short uw_OldNumFrames )
{
    EVE_tdst_Event	*pst_Evt;

    pst_Evt = pst_FindEngineEvent();
    if((pst_Evt->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_InterpolationKey) return;
    if (pst_Evt->uw_NumFrames <= 0) return;

    EVE_Event_InterpolationKey_ZoomTimeInterpolation(pst_Evt, EVE_FrameToTime(pst_Evt->uw_NumFrames) / EVE_FrameToTime( uw_OldNumFrames ) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::RecomputeWidth(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = pst_FindEngineEvent();
	//Anti-Bug.
	if(!pst_Evt) return;

	mf_Width = EVE_FrameToTime(pst_Evt->uw_NumFrames) * mpo_Track->mpo_Frame->mst_Ini.f_Factor;
	mf_Width /= mpo_Track->mpo_Frame->mst_Ini.f_ValFactor;
	if(mpo_Track->mpo_Frame->mst_Ini.i_AllEventsVis) mf_Width = 40.0f;
	mf_CurWidth = mf_Width;
	if(mf_CurWidth == 0.0f) mb_Empty = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVE_tdst_Event *EEVE_cl_Event::pst_FindEngineEvent(void)
{
	return &mpo_Track->mpst_Track->pst_AllEvents[mi_NumEvent];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Event *EEVE_cl_Event::po_FindEditorEvent(EVE_tdst_Event *_pst_Event)
{
	return mpo_Track->mo_ListEvents.GetAt(_pst_Event - mpo_Track->mpst_Track->pst_AllEvents);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = pst_FindEngineEvent();
	switch(pst_Evt->w_Flags & EVE_C_EventFlag_Type)
	{
	case EVE_C_EventFlag_AIFunction:
		SetAIFunctionName();
		break;
	case EVE_C_EventFlag_InterpolationKey:
		SetInterpolationKeyName();
		break;
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
void RefreshEvent(void *, void *_po_Item, void *, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*psel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	psel = (EVAV_cl_ViewItem *) _po_Item;
	((EEVE_cl_Track *) psel->mi_Param4)->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::OnLButtonDown(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event		*pst_Evt;
	EVAV_cl_ViewItem	*po_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->LockDisplay(mpo_Track->mpo_Frame->mpo_VarsView);

	/* Save selected event */
	mpo_Track->mpo_Frame->mpo_LastSelected = this;

	/* Specific properties */
	mpo_Track->mpo_Frame->mpo_VarsView->ResetList();
	mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Specific Event Properties", EVAV_EVVIT_Separator, NULL);
	pst_Evt = pst_FindEngineEvent();
	switch(pst_Evt->w_Flags & EVE_C_EventFlag_Type)
	{
	case EVE_C_EventFlag_AIFunction:
		FillAIFunction(pst_Evt);
		break;
	case EVE_C_EventFlag_InterpolationKey:
		FillInterpolationKey(pst_Evt);
		break;
	case EVE_C_EventFlag_MorphKey:
		FillMorphKey(pst_Evt);
		break;
	}

	/* Common properties */
	mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Common Event Properties", EVAV_EVVIT_Separator, NULL);
	mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Execute Once", EVAV_EVVIT_Flags, &pst_Evt->w_Flags, 0, 0, 2);
	mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Flash Matrix", EVAV_EVVIT_Flags, &pst_Evt->w_Flags, 0, 2, 2);
	po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem
		(
			"Label Num (b0)",
			EVAV_EVVIT_Flags,
			&pst_Evt->w_Flags,
			0,
			12,
			2,
			0,
			0,
			(int) mpo_Track
		);
	po_Item->mpfn_CB = RefreshEvent;

	po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem
		(
			"Label Num (b1)",
			EVAV_EVVIT_Flags,
			&pst_Evt->w_Flags,
			0,
			13,
			2,
			0,
			0,
			(int) mpo_Track
		);
	po_Item->mpfn_CB = RefreshEvent;

	po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem
		(
			"Label Num (b2)",
			EVAV_EVVIT_Flags,
			&pst_Evt->w_Flags,
			0,
			14,
			2,
			0,
			0,
			(int) mpo_Track
		);
	po_Item->mpfn_CB = RefreshEvent;

	po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem
		(
			"Label Num (b3)",
			EVAV_EVVIT_Flags,
			&pst_Evt->w_Flags,
			0,
			15,
			2,
			0,
			0,
			(int) mpo_Track
		);
	po_Item->mpfn_CB = RefreshEvent;

	po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem
		(
			"Wait False",
			EVAV_EVVIT_Flags,
			&pst_Evt->w_Flags,
			0,
			3,
			2,
			0,
			0,
			(int) mpo_Track
		);
	po_Item->mpfn_CB = RefreshEvent;
	po_Item = mpo_Track->mpo_Frame->mpo_VarsView->AddItem
		(
			"Wait True",
			EVAV_EVVIT_Flags,
			&pst_Evt->w_Flags,
			0,
			4,
			2,
			0,
			0,
			(int) mpo_Track
		);
	po_Item->mpfn_CB = RefreshEvent;

	M_MF()->UnlockDisplay(mpo_Track->mpo_Frame->mpo_VarsView);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::OnLButtonDblClk(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = pst_FindEngineEvent();
	switch(pst_Evt->w_Flags & EVE_C_EventFlag_Type)
	{
	case EVE_C_EventFlag_AIFunction:
		SetAIFunction(pst_Evt);
		OnLButtonDown();
		mpo_Track->FillBar();
		break;
	case EVE_C_EventFlag_InterpolationKey:
		SetInterpolationKey(pst_Evt, FALSE);
		OnLButtonDown();
		mpo_Track->FillBar();
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::OnMButtonDown(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = pst_FindEngineEvent();
	switch(pst_Evt->w_Flags & EVE_C_EventFlag_Type)
	{
	case EVE_C_EventFlag_AIFunction:
		break;

	case EVE_C_EventFlag_InterpolationKey:
		ComputeInterpolationKey(pst_Evt, EVE_w_Event_InterpolationKey_GetType(pst_Evt), FALSE);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::RoundTo(float _f_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = pst_FindEngineEvent();
	pst_Evt->uw_NumFrames = EVE_TimeToFrame(MATH_f_FloatModulo(EVE_FrameToTime(pst_Evt->uw_NumFrames), _f_Val));
	RecomputeWidth();
}

#endif /* ACTIVE_EDITORS */
