/*$T DIAanim_dlg.cpp GC! 1.100 06/13/01 13:00:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAanim_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDItors/Sources/EVEnts/EVEmsg.h"
#include "EDItors/Sources/EVEnts/EVEframe.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "SELection/SELection.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/OUTput/OUTframe_act.h"
#include "SOFT/SOFTbackgroundimage.h"
#include "ENGine/Sources/WORld/WORmain.h"

#define WIDTH_FRAME 8
#define DECX		160

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_AnimDialog, EDIA_cl_BaseDialog)
	ON_COMMAND(IDC_BUTTON_GENERAL, OnGeneral)
	ON_COMMAND(IDC_BUTTON_PLAY, OnPlay)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float f_GetShortestSelectedTrackTime(ANI_st_GameObjectAnim *_pst_GOAnim)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Track	*pst_Track;
	int				i, j;
	float			f_Min, f_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GOAnim) return NULL;

	f_Min = 10000000.0f;

	/* We loop thru all the selected Tracks to find the shortest one. */
	for(i = 0; i < _pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks->uw_NumTracks; i++)
	{
		pst_Track = &_pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks->pst_AllTracks[i];
		if(!(pst_Track->uw_Flags & EVE_C_Track_Selected)) continue;
		f_Track = 0;
		for(j = 0; j < pst_Track->uw_NumEvents; j++)
			f_Track += EVE_FrameToTime(pst_Track->pst_AllEvents[j].uw_NumFrames);

		if(f_Track < f_Min) f_Min = f_Track;
	}

	return f_Min;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_AnimDialog::EDIA_cl_AnimDialog(F3D_cl_View *_pview, OBJ_tdst_GameObject *_pst_GAO) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_ANIM)
{
	mpo_View = _pview;
	mpst_GAO = _pst_GAO;
	mi_FirstFrame = 0;
	mb_BegSel = FALSE;
	SOFT_BackGroundImage_SetAnimGao(_pview->mst_WinHandles.pst_DisplayData->pst_BackgroundImage, mpst_GAO);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_AnimDialog::~EDIA_cl_AnimDialog(void)
{
	mpo_View->mpo_AnimDialog = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_AnimDialog::OneTrameEnding(void)
{
	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_AnimDialog::Reset(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item;
	OBJ_tdst_GameObject		*pst_GAO, *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpst_GAO = NULL;
	SOFT_BackGroundImage_SetAnimGao(mpo_View->mst_WinHandles.pst_DisplayData->pst_BackgroundImage, mpst_GAO);
	if(!mpo_View->mst_WinHandles.pst_World) return;
	
	if(!mpo_View->mst_WinHandles.pst_World || !mpo_View->mst_WinHandles.pst_World->pst_Selection) return;
	pst_Item = mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
	while(pst_Item != NULL)
	{
		if(pst_Item->l_Flag & SEL_C_SIF_Object)
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_Item->p_Content;
			if(ANI_b_IsGizmoAnim(pst_GAO, &pst_Father))
			{
				mpst_GAO = pst_Father;
				SOFT_BackGroundImage_SetAnimGao
				(
					mpo_View->mst_WinHandles.pst_DisplayData->pst_BackgroundImage,
					mpst_GAO
				);
				return;
			}
		}

		pst_Item = pst_Item->pst_Next;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_AnimDialog::OnGeneral(void)
{
	GetParent()->SetFocus();
	mpo_View->ActionAnimation(EEVE_MESSAGE_TBGENERAL, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_AnimDialog::OnPlay(void)
{
	GetParent()->SetFocus();
	mpo_View->ActionAnimation(EEVE_MESSAGE_TBPLAY);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVE_tdst_Event *pst_EventForFrame(OBJ_tdst_GameObject *_pst_GAO, int _i_Track, int _i_Frame, float *_pf_Rest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	short					t;
	EVE_tdst_Track			*pst_Track;
	EVE_tdst_Event			*pst_Event;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GOAnim = _pst_GAO->pst_Base->pst_GameObjectAnim;
	if(_i_Track >= pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks->uw_NumTracks) return NULL;
	pst_Track = &pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks->pst_AllTracks[_i_Track];

	t = _i_Frame;
	if(_pf_Rest) *_pf_Rest = 0.0f;
	pst_Event = pst_Track->pst_AllEvents;
	while(t > 0)
	{
		if(t < pst_Event->uw_NumFrames)
		{
			if(_pf_Rest) *_pf_Rest = EVE_FrameToTime(pst_Event->uw_NumFrames - t);
			break;
		}
		else
			t -= pst_Event->uw_NumFrames;
		pst_Event++;
		if(pst_Event == pst_Track->pst_AllEvents + pst_Track->uw_NumEvents) return NULL;
	}

	return pst_Event;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_AnimDialog::OnMouseMove(UINT, CPoint pt)
{
	/*~~~~~~~~~~~*/
	int		icur;
	float	f;
	float	f_Min;
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(mb_BegSel && mpst_GAO)
	{
		GetClientRect(&o_Rect);
		if(pt.x > o_Rect.right - 4) pt.x = o_Rect.right - 4;

		pt.x -= DECX;
		pt.x /= WIDTH_FRAME;
		pt.x += mi_FirstFrame;

		f_Min = f_GetShortestSelectedTrackTime(mpst_GAO->pst_Base->pst_GameObjectAnim);

		if((float) pt.x * (1.0f / 60.0f) - f_Min > 10E-5) pt.x = ((int) (f_Min * 60.0f));

		icur = ANI_i_CurFrame(mpst_GAO);
		icur = pt.x - icur;
		f = (float) icur;
		f *= (1.0f / 60.0f);

		if(f) mpo_View->ActionAnimation(EEVE_MESSAGE_ADDSETFRAME, *(ULONG *) &f);
		Invalidate();
		UpdateWindow();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_AnimDialog::OnLButtonUp(UINT, CPoint pt)
{
	if(mb_BegSel) ReleaseCapture();
	mb_BegSel = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_AnimDialog::OnRButtonDown(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	CPoint			point;
	int				ires;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&point);
	M_MF()->InitPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu);
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 100, TRUE, "Add Key");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 101, TRUE, "Remove Key");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 102, TRUE, "Move Key Left");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 103, TRUE, "Move Key Right");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 104, TRUE, "Pivot Trajectory (Rot)");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 105, TRUE, "Pivot Trajectory (Trans)");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 106, TRUE, "Show Only This Trajectory (Rot)");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 107, TRUE, "Display/Hide Bones On Trajectory");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 0, TRUE, "");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 1, TRUE, "Copy Keys");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 2, TRUE, "Paste Keys");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 4, TRUE, "Paste Keys with Symetry");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 5, TRUE, "Paste Keys with Symetry (Reference)");
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 0, TRUE);
	M_MF()->AddPopupMenuAction(mpo_View->mpo_AssociatedEditor, &o_Menu, 7, TRUE, "Add/Remove IK");
	ires = M_MF()->TrackPopupMenuAction(mpo_View->mpo_AssociatedEditor, point, &o_Menu);
	switch(ires)
	{
	case 100:	mpo_View->ActionAnimation(EOUT_ACTION_ANIMADDKEY, 0); break;
	case 101:	mpo_View->ActionAnimation(EOUT_ACTION_ANIMDELKEY, 0); break;
	case 102:	mpo_View->ActionAnimation(EOUT_ACTION_ANIMMOVEKEYLEFT, 0); break;
	case 103:	mpo_View->ActionAnimation(EOUT_ACTION_ANIMMOVEKEYRIGHT, 0); break;
	case 104:	mpo_View->ActionAnimation(EOUT_ACTION_ANIMPIVOTTRAJROT, 0); break;
	case 105:	mpo_View->ActionAnimation(EOUT_ACTION_ANIMPIVOTTRAJTRA, 0); break;
	case 106:	mpo_View->ActionAnimation(EOUT_ACTION_ANIMTHISTRAJROT, 0); break;
	case 107:	mpo_View->ActionAnimation(EOUT_ACTION_DISPLAYCURVEBONES, 0); break;
	case 1:		mpo_View->ActionAnimation(EOUT_ACTION_ANIMCOPYKEYS, 0); break;
	case 2:		mpo_View->ActionAnimation(EOUT_ACTION_ANIMPASTEKEYS, 0); break;
	case 4:		mpo_View->ActionAnimation(EOUT_ACTION_ANIMPASTEKEYSSYMETRY, 0); break;
	case 5:		mpo_View->ActionAnimation(EOUT_ACTION_ANIMPASTEKEYSSYMETRYREF, 0); break;
	case 7:		mpo_View->ActionAnimation(EOUT_ACTION_ANIMIK, 0); break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_AnimDialog::OnLButtonDown(UINT, CPoint pt)
{
	/*~~~~~~~~~~~*/
	int		icur;
	float	f;
	CRect	o_Rect;
	float	f_Min;
	/*~~~~~~~~~~~*/

	if(!mpst_GAO) return;

	SetCapture();
	mb_BegSel = TRUE;

	GetClientRect(&o_Rect);
	if(pt.x > o_Rect.right - 4) pt.x = o_Rect.right - 4;

	pt.x -= DECX;
	pt.x /= WIDTH_FRAME;
	pt.x += mi_FirstFrame;

	f_Min = f_GetShortestSelectedTrackTime(mpst_GAO->pst_Base->pst_GameObjectAnim);

	if((float) pt.x * (1.0f / 60.0f) - f_Min > 10E-5)
	{
		pt.x = ((int) (f_Min * 60.0f));
		LINK_PrintStatusMsg("Frame overpass shortest selected Track time");
	}

	icur = ANI_i_CurFrame(mpst_GAO);
	icur = pt.x - icur;
	f = (float) icur;
	f *= (1.0f / 60.0f);

	if(f)
	{
		if(GetAsyncKeyState(VK_CONTROL) < 0)
			mpo_View->ActionAnimation(EEVE_MESSAGE_ADDSETFRAME2, *(ULONG *) &f);
		else
			mpo_View->ActionAnimation(EEVE_MESSAGE_SETFRAME, *(ULONG *) &f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_AnimDialog::OnEraseBkgnd(CDC *pdc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	int						i;
	CRect					o_Rect;
	CRect					o_Rect1;
	CRect					o_Rect2;
	CRect					o_Rect3;
	CRect					o_Rect4;
	EVE_tdst_Event			*pst_Evt;
	EVE_tdst_Event			*pst_Evt1;
	float					f_Rest;
	float					f_Rest1;
	int						inum, icur;
	char					*pz;
	BOOL					b_HasKey, b_HasNoKey;
	BOOL					b_HasKeyTra, b_HasNoKeyTra;
	SEL_tdst_SelectedItem	*pst_Item;
	OBJ_tdst_GameObject		*pst_GAO, *pst_Father;
	COLORREF				col;
	CString					o_Str;
	HFONT					hold;
	BOOL					b_ValidGizmo;
	USHORT					w_Type;
	int						countsel;
	char					az[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Reset();

	b_ValidGizmo = FALSE;
	if(!mpo_View->mst_WinHandles.pst_World) return TRUE;
	pst_Item = mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;

	/* We may select only non valid Gizmos. If it is the case, we dont want any render. */
	while(pst_Item != NULL)
	{
		if(pst_Item->l_Flag & SEL_C_SIF_Object)
		{
			pst_GAO = (OBJ_tdst_GameObject *) pst_Item->p_Content;
			if(ANI_b_IsGizmoAnim(pst_GAO, &pst_Father) && (pst_Father == mpst_GAO) && ANI_b_GizmoHasTrack(pst_GAO))
				b_ValidGizmo = TRUE;
		}

		pst_Item = pst_Item->pst_Next;
	}

	if(!mpst_GAO || !b_ValidGizmo)
	{
		GetClientRect(&o_Rect);
		pdc->FillSolidRect(&o_Rect, GetSysColor(COLOR_BTNFACE));
		return TRUE;
	}

	hold = (HFONT) pdc->SelectObject(&M_MF()->mo_Fnt);
	GetClientRect(&o_Rect);
	icur = ANI_i_CurFrame(mpst_GAO);
	if(icur < mi_FirstFrame) mi_FirstFrame = icur;
	if(icur - mi_FirstFrame >= (o_Rect.Width() - DECX) / WIDTH_FRAME)
		mi_FirstFrame = (icur + 1) - ((o_Rect.Width() - DECX) / WIDTH_FRAME);

	pst_GOAnim = mpst_GAO->pst_Base->pst_GameObjectAnim;
	for(i = 0; i < (o_Rect.Width() - DECX) / WIDTH_FRAME; i++)
	{
		/* Border */
		o_Rect1.left = (i * WIDTH_FRAME) + DECX;
		o_Rect1.right = o_Rect1.left + WIDTH_FRAME + 1;
		o_Rect1.top = o_Rect.top + 2;
		o_Rect1.bottom = o_Rect.bottom - 2;
		pdc->Draw3dRect(&o_Rect1, 0, 0);
		o_Rect2 = o_Rect1;
		o_Rect1.InflateRect(-1, -1);

		if(i + mi_FirstFrame == icur)
			pdc->FillSolidRect(&o_Rect1, 0x00FF0000);
		else
		{
			b_HasKey = FALSE;
			b_HasNoKey = FALSE;
			b_HasKeyTra = FALSE;
			b_HasNoKeyTra = FALSE;

			/* Scan selection */
			pst_Item = mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem;
			if(!mpo_View->mst_WinHandles.pst_World) goto endanim;
			while(pst_Item != NULL)
			{
				if(pst_Item->l_Flag & SEL_C_SIF_Object)
				{
					pst_GAO = (OBJ_tdst_GameObject *) pst_Item->p_Content;
					if(ANI_b_IsGizmoAnim(pst_GAO, &pst_Father) && (pst_Father == mpst_GAO))
					{
						if(mpst_GAO == pst_GAO)
						{
							inum = 0;
						}
						else
						{
							pz = pst_GAO->sz_Name + L_strlen(pst_GAO->sz_Name) - 1;
							if(*pz == ']')
							{
								while(*pz != '[') pz--;
								pz--;
								while(L_isspace(*pz)) pz--;
							}

							while(L_isdigit(*pz)) pz--;
							pz++;
							inum = 2 + (L_atoi(pz) * 2);
						}

						f_Rest = 0.0f;
						pst_Evt = pst_EventForFrame(mpst_GAO, inum, i + mi_FirstFrame, &f_Rest);
						if(!pst_Evt)
						{
							pst_Evt1 = NULL;
							pst_Item = pst_Item->pst_Next;
							continue;
						}

						if(f_Rest == 0) b_HasKey = TRUE;
						if(f_Rest != 0) b_HasNoKey = TRUE;

						f_Rest1 = 0.0f;
						pst_Evt1 = pst_EventForFrame(mpst_GAO, inum + 1, i + mi_FirstFrame, &f_Rest1);
						if(!pst_Evt1)
						{
							pst_Item = pst_Item->pst_Next;
							continue;
						}

						if(f_Rest1 == 0) b_HasKeyTra = TRUE;
						if(f_Rest1 != 0) b_HasNoKeyTra = TRUE;
					}
				}

				pst_Item = pst_Item->pst_Next;
			}

			/* Rot */
			if(b_HasKey && b_HasNoKey)
				col = 0x0000FF00;
			else if(b_HasKey && !b_HasNoKey)
			{
				if
				(
					pst_Evt
				&&	((pst_Evt->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
				&&	pst_Evt->p_Data
				&&	(pst_Evt->w_Flags & EVE_C_EventFlag_Symetric)
				) col = 0x00009F00;
				else
					col = 0x000000FF;
			}
			else
				col = GetSysColor(COLOR_BTNFACE);
			if(pst_Evt && pst_Evt->w_Flags & EVE_C_EventFlag_Selected)
			{
				if(col == GetSysColor(COLOR_BTNFACE))
					col = 0x00FFFFFF;
				else
					col += 0x00FF0000;
			}

			col &= 0x00FFFFFF;
			o_Rect3 = o_Rect1;
			o_Rect3.bottom = o_Rect3.bottom /= 2;
			pdc->FillSolidRect(&o_Rect3, col);

			/* Tra */
			if(b_HasKeyTra && b_HasNoKeyTra)
				col = 0x0000FF00;
			else if(b_HasKeyTra && !b_HasNoKeyTra)
			{
				if
				(
					pst_Evt
				&&	((pst_Evt->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
				&&	pst_Evt->p_Data
				&&	(pst_Evt->w_Flags & EVE_C_EventFlag_Symetric)
				) col = 0x00009F00;
				else
					col = 0x000000FF;
			}
			else
				col = GetSysColor(COLOR_BTNFACE);
			if(pst_Evt1 && pst_Evt1->w_Flags & EVE_C_EventFlag_Selected)
			{
				if(col == GetSysColor(COLOR_BTNFACE))
					col = 0x00FFFFFF;
				else
					col += 0x00FF0000;
			}

			o_Rect3.top = o_Rect3.bottom;
			o_Rect3.bottom = o_Rect1.bottom;

			/* IK */
			if(pst_Evt1 && ((pst_Evt1->w_Flags & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey))
			{
				w_Type = EVE_w_Event_InterpolationKey_GetType(pst_Evt1);
				if(w_Type & EVE_InterKeyType_BlockedForIK)
				{
					o_Rect4 = o_Rect3;
					o_Rect4.top = o_Rect4.bottom - 3;
					o_Rect3.bottom -= 3;
					pdc->FillSolidRect(&o_Rect4, 0x00FFFF00);
				}
			}

			col &= 0x00FFFFFF;
			pdc->FillSolidRect(&o_Rect3, col);
		}

		/* Exclude frame */
		pdc->ExcludeClipRect(&o_Rect2);
	}

	/* Back */
endanim:
	GetClientRect(&o_Rect);
	pdc->FillSolidRect(&o_Rect, GetSysColor(COLOR_BTNFACE));

	/* Frame numbers */
	pdc->SelectClipRgn(NULL);
	for(i = 0; i < (o_Rect.Width() - DECX) / WIDTH_FRAME; i++)
	{
		o_Rect1.left = (i * WIDTH_FRAME) + DECX;
		o_Rect1.right = o_Rect1.left + WIDTH_FRAME + 1;
		o_Rect1.top = o_Rect.top + 2;
		o_Rect1.bottom = o_Rect.bottom - 2;
		if((i + mi_FirstFrame + 1) % 10 == 0)
		{
			o_Str.Format("%d", i + mi_FirstFrame + 1);
			pdc->SetBkMode(TRANSPARENT);
			pdc->DrawText(o_Str, &o_Rect1, DT_CENTER | DT_NOCLIP);
		}
	}

	/* Name */
	GetClientRect(&o_Rect1);
	o_Rect1.left = 55;
	o_Rect1.right = o_Rect1.left + 100;
	o_Rect1.top += 3;
	o_Rect1.bottom -= 3;
	pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
	if(mpo_View->mst_WinHandles.pst_World->pst_Selection->pst_FirstItem)
	{
		countsel = SEL_l_CountItem(mpo_View->mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Object);
		if(countsel == 1)
		{
			pst_GAO = WOR_pst_GetBoneFromGizmo(pst_GAO);
			if(!pst_GAO) pst_GAO = mpst_GAO;
			strcpy(az, pst_GAO->sz_Name);
			if(strrchr(az, '@')) *strrchr(az, '@') = 0;
			if(strrchr(az, '.')) *strrchr(az, '.') = 0;
			pdc->DrawText(az, CRect(60, 10, 145, 30), DT_LEFT);
		}
		else
		{
			sprintf(az, "%d selected", countsel);
			pdc->DrawText(az, CRect(60, 10, 145, 30), DT_LEFT);
		}
	}

	pdc->SelectObject(hold);

	return TRUE;
}

#endif /* ACTIVE_EDITORS */
