/*$T EVEevent_TimeKey.cpp GC! 1.080 03/29/00 16:06:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEnt_timekey.h"
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

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::FillTimeKey(EVE_tdst_Event *_pst_Evt)
{
	/*~~~~~~~~~~~~~*/
	short	w_Type;
	float	*f_Params;
	/*~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_TimeKey_GetType(_pst_Evt);

	if(w_Type == -1) return;

	f_Params = ((float *) _pst_Evt->p_Data) + 1;

    switch (w_Type)
    {
    case EVE_KeyType_Speed:
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Speed", EVAV_EVVIT_Float, f_Params);
        break;
    case EVE_KeyType_SpeedPos:
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Speed", EVAV_EVVIT_Float, f_Params);
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Position", EVAV_EVVIT_Float, f_Params + 1);
        break;
    case EVE_KeyType_Acceleration:
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Acceleration", EVAV_EVVIT_Float, f_Params);
        break;
    case EVE_KeyType_AccelerationSpeed:
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Acceleration", EVAV_EVVIT_Float, f_Params);
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Speed", EVAV_EVVIT_Float, f_Params + 1);
        break;
    case EVE_KeyType_AccelerationPos:
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Acceleration", EVAV_EVVIT_Float, f_Params);
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Position", EVAV_EVVIT_Float, f_Params + 1);
        break;
    case EVE_KeyType_AccelerationSpeedPos:
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Acceleration", EVAV_EVVIT_Float, f_Params);
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Speed", EVAV_EVVIT_Float, f_Params + 1);
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Position", EVAV_EVVIT_Float, f_Params + 2);
        break;
    case EVE_KeyType_TimeFactorStart:
    case EVE_KeyType_TimeFactor:
        mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Factor", EVAV_EVVIT_Float, f_Params);
        break;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::SetTimeKeyName(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Event	*pst_Evt;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_Evt = pst_FindEngineEvent();
	if(!pst_Evt->p_Data)
		mo_Name = "";
	else
		mo_Name = "Timekey";
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::ComputeTimeKey(EVE_tdst_Event *_pst_Evt, int _w_Type)
{
	if(_w_Type == -1) return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::SetTimeKey(EVE_tdst_Event *_pst_Evt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	EMEN_cl_SubMenu o_Menu(FALSE);
	CPoint			pt;
	short			w_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_TimeKey_GetType(_pst_Evt);

	if(w_Type == -1) return;

	GetCursorPos(&pt);
	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, 0, TRUE, "Translation Key", -1);
	M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_KeyType_Linear + 1, TRUE, "Linear", -1);
    M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_KeyType_Square + 1, TRUE, "Square", -1);
    M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_KeyType_Speed + 1, TRUE, "Speed", -1);
    M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_KeyType_SpeedPos + 1, TRUE, "Speed + pos", -1);
    M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_KeyType_Acceleration + 1, TRUE, "Acc", -1);
    M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_KeyType_AccelerationSpeed + 1, TRUE, "Acc + speed", -1);
    M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_KeyType_AccelerationPos + 1, TRUE, "Acc + pos", -1);
    M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_KeyType_AccelerationSpeedPos + 1, TRUE, "Acc + speed + pos", -1);
    M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_KeyType_TimeFactorStart + 1, TRUE, "Factor start", -1);
    M_MF()->AddPopupMenuAction(mpo_Track->mpo_Frame, &o_Menu, EVE_KeyType_TimeFactor + 1, TRUE, "Factor", -1);
	i = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);

    if ( i <= 0) return;
    w_Type = (short) i - 1;

	EVE_Event_TimeKey_AllocData(_pst_Evt, w_Type);
    EVE_Event_TimeKey_ComputeSP( mpo_Track->mpst_Track );
	SetTimeKeyName();
	mpo_Track->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::DrawTimeKey(EVE_tdst_Event *_pst_Evt, CDC *pdc, CRect *_po_Rect)
{
	/*~~~~~~~~~~~~~~~~*/
	short		w_Type;
	CRect		o_Rect;
	COLORREF	col;
	/*~~~~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_TimeKey_GetType(_pst_Evt);
	if(w_Type == -1) return;

	col = 0x009F9F9F;
	o_Rect = *_po_Rect;
	o_Rect.left += 4;
	o_Rect.top += 4;
	o_Rect.bottom = o_Rect.top + 5;
	o_Rect.right = o_Rect.left + 5;
	SpeedSolidRect(pdc, &o_Rect, col);
}

#endif /* ACTIVE_EDITORS */
