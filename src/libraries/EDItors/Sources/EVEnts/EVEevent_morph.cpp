 /*$T EVEevent_morph.cpp 1.00 03/07/06 17:00:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEnt_morphkey.h"
#include "EVEevent.h"
#include "EVEframe.h"
#include "EDImainframe.h"
#include "EVEmsg.h"


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
void EEVE_cl_Event::FillMorphKey(EVE_tdst_Event *_pst_Evt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_MorphKeyParam	*pst_MorphParam;
	EVAV_cl_ViewItem	*po;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_MorphParam = EVE_Event_MorphKey_GetParam(_pst_Evt);
	po = mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Morph Bone Index", EVAV_EVVIT_Int, &pst_MorphParam->i_MorphBone);
	po->mpfn_CB = ChangePos;
	po = mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Channel Index", EVAV_EVVIT_Int, &pst_MorphParam->i_Channel);
	po->mpfn_CB = ChangePos;
	po = mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Target 1 Index", EVAV_EVVIT_Int, &pst_MorphParam->i_Target1);
	po->mpfn_CB = ChangePos;
	po = mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Target 2 Index", EVAV_EVVIT_Int, &pst_MorphParam->i_Target2);
	po->mpfn_CB = ChangePos;
	po = mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Progression", EVAV_EVVIT_Float, &pst_MorphParam->f_Prog);
	po->mpfn_CB = ChangePos;
	po = mpo_Track->mpo_Frame->mpo_VarsView->AddItem("Factor", EVAV_EVVIT_Float, &pst_MorphParam->f_Factor);
	po->mpfn_CB = ChangePos;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Event::DrawMorphKey(EVE_tdst_Event *_pst_Evt, CDC *pdc, CRect *_po_Rect)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect		o_Rect;
	COLORREF	col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	col = 0x00000000;

	o_Rect = *_po_Rect;
	o_Rect.left += 2;
	o_Rect.bottom -= 4;
	o_Rect.top = o_Rect.bottom - 2;
	o_Rect.right = o_Rect.left + _po_Rect->Width() - 6;
	if(o_Rect.left < o_Rect.right) SpeedSolidRect(pdc, &o_Rect, col);

}

#endif