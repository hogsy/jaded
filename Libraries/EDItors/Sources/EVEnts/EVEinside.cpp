/*$T EVEinside.cpp GC 1.139 03/17/04 11:55:23 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EVEframe.h"
#include "EVEinside.h"
#include "EVEscroll.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "EDImainframe.h"
#include "Res/Res.h"
#include "OUTput/OUTframe.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/OUTput/OUTmsg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "LINks/LINKstruct.h"
#include "SELection/SELection.h"
#include "Engine/Sources/MODifier/MDFmodifier_GEO.h"
#include "AIinterp/Sources/AIengine.h"
#include "ENGine/Sources/ANImation/ANImain.h"

#ifdef JADEFUSION
extern BOOL EVE_gb_NeedToRecomputeSND;
#else
extern "C" BOOL EVE_gb_NeedToRecomputeSND;
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

IMPLEMENT_DYNCREATE(EEVE_cl_Inside, CScrollView)
BEGIN_MESSAGE_MAP(EEVE_cl_Inside, CScrollView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
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
EEVE_cl_Inside::EEVE_cl_Inside(void)
{
	mpo_Parent = NULL;
	mpo_Scroll = new EEVE_cl_Scroll;
	mb_SelectMode = FALSE;
	mb_SelTrackMode = FALSE;
	mpo_LastSelTrack = NULL;
	mb_MoveLeft = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Inside::~EEVE_cl_Inside(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EEVE_cl_Inside::OnCreate(LPCREATESTRUCT lpcs)
{
	if(CScrollView::OnCreate(lpcs) == -1) return -1;
	mpo_Scroll->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 0);
	mpo_Scroll->mpo_Parent = mpo_Parent;
	mpo_Scroll->mpo_Inside = this;
	mpo_Scroll->SetScrollSizes(MM_TEXT, CSize(50, 50));
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::OnSize(UINT nType, int cx, int cy)
{
	SetScrollSizes(MM_TEXT, CSize(0, 0));
	if(mpo_Parent) mpo_Scroll->MoveWindow(CRect(mpo_Parent->mst_Ini.i_XLeft, EVE_C_YRegle, cx, cy));
	CScrollView::OnSize(nType, cx, cy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_cl_Inside::OnEraseBkgnd(CDC *)
{
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *EEVE_cl_Inside::pt_GetGaoFromTrack(EVE_tdst_Track *pst_Track, CString &o_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				asz_Tmp[BIG_C_MaxLenName];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_Track->pst_GO)
		pst_GO = pst_Track->pst_GO;
	else
		pst_GO = mpo_Parent->mpst_GAO;

	if(pst_GO && (int) pst_GO != -1)
	{
		if
		(
			(pst_Track->uw_Gizmo != 0xFFFF)
		&&	(pst_GO->pst_Base)
		&&	(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
		&&	(pst_GO->pst_Base->pst_GameObjectAnim)
		&&	(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
		&&	(pst_GO->pst_Base->pst_AddMatrix)
		&&	(pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo)
		&&	(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer))
		)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~*/
			TAB_tdst_PFelem *pst_Bone;
			/*~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Bone = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);

			if(pst_Track->uw_Gizmo < pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects->ul_NbElems)
			{
				pst_Bone += pst_Track->uw_Gizmo;
				pst_GO = ((OBJ_tdst_GameObject *) pst_Bone->p_Pointer);

				L_strcpy(asz_Tmp, ((OBJ_tdst_GameObject *) pst_Bone->p_Pointer)->sz_Name);
				if(L_strrchr(asz_Tmp, '.')) *(L_strrchr(asz_Tmp, '.')) = 0;
				if(L_strrchr(asz_Tmp, '@')) *(L_strrchr(asz_Tmp, '@')) = 0;

				o_Name = asz_Tmp;
			}
			else
			{
				o_Name = "UNKNOWN";
			}
		}
		else
		{
			L_strcpy(asz_Tmp, pst_GO->sz_Name);
			if(L_strrchr(asz_Tmp, '.')) *(L_strrchr(asz_Tmp, '.')) = 0;
			o_Name = asz_Tmp;
		}
	}
	else
		o_Name = "UNKNOWN";

	return pst_GO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EEVE_cl_Inside::PreTranslateMessage(MSG *pMsg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint			pt;
	EEVE_cl_Track	*po_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		po_Track = po_GetTrackWithY(pt.y);
		if(po_Track) 
		{
			ClientToScreen(&pt);
			po_Track->ScreenToClient(&pt);
			if(po_Track->ToggleExpand(pt)) return TRUE;
		}
	}

	return CScrollView::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::OnDraw(CDC *pdc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect				o_Rect;
	CRect				o_Rect1;
	CRect				o_Rect2;
	POSITION			pos;
	EEVE_cl_Track		*po_Track;
	EVE_tdst_Track		*pst_Track;
	CString				o_Name;
	HFONT				hfnt;
	HICON				hicon;
	HICON				hicon1;
	int					iCount;
	OBJ_tdst_GameObject *pst_GO;
	char				col;
	COLORREF			colcol;
	OBJ_tdst_GameObject *pt_SelGao;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Parent->mpst_Data)
	{
		GetClientRect(&o_Rect);
		pdc->FillSolidRect(&o_Rect, GetSysColor(COLOR_3DSHADOW));
		return;
	}

	hicon = AfxGetApp()->LoadIcon(EVE_IDI_SHOWTRACK);
	hicon1 = AfxGetApp()->LoadIcon(IDI_ICON_CURVE);

	/* Draw intersection top/left */
	GetClientRect(&o_Rect);
	o_Rect.right = mpo_Parent->mst_Ini.i_XLeft;
	o_Rect1 = o_Rect;
	o_Rect1.bottom = EVE_C_YRegleSel;
	pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
	o_Rect1.InflateRect(-1, -1);
	pdc->FillSolidRect(&o_Rect1, GetSysColor(COLOR_BTNFACE));
	o_Rect1.left = o_Rect1.right - 10;
	pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));

	/* Track name */
	hfnt = 0;
	if(mpo_Parent->mul_ListTracks)
	{
		hfnt = (HFONT) pdc->SelectObject(&M_MF()->mo_Fnt);
		pdc->SetBkMode(TRANSPARENT);
		o_Rect1 = CRect(2, 2, o_Rect1.right - 12, o_Rect1.bottom);
		pdc->ExtTextOut(2, 2, ETO_CLIPPED, &o_Rect1, BIG_NameFile(mpo_Parent->mul_ListTracks), NULL);
	}

	o_Rect1 = o_Rect;
	o_Rect1.top = EVE_C_YRegleSel;
	o_Rect1.bottom = EVE_C_YRegle;
	pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
	o_Rect1.InflateRect(-1, -1);
	pdc->FillSolidRect(&o_Rect1, GetSysColor(COLOR_BTNFACE));

	/* Exclude intersection top/left */
	GetClientRect(&o_Rect);
	o_Rect.bottom = EVE_C_YRegle;
	o_Rect.right = mpo_Parent->mst_Ini.i_XLeft;
	pdc->ExcludeClipRect(&o_Rect);

	/* Draw tracks description */
	GetClientRect(&o_Rect);
	o_Rect.top -= mpo_Scroll->GetScrollPosition().y;
	o_Rect.right = mpo_Parent->mst_Ini.i_XLeft;
	o_Rect1 = o_Rect;
	o_Rect1.top += EVE_C_YRegle;

	/* Selected */
	pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
	if(mpo_Parent->mpst_ListTracks) pst_Track = mpo_Parent->mpst_ListTracks->pst_AllTracks;
	iCount = 0;
	pt_SelGao = NULL;
	while(pos)
	{
		po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && (!mpo_Parent->mst_Ini.i_DisplayHidden)) continue;
		if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
		{
			pt_SelGao = pt_GetGaoFromTrack(po_Track->mpst_Track, o_Name);
			break;
		}
	}

	pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
	if(mpo_Parent->mpst_ListTracks) pst_Track = mpo_Parent->mpst_ListTracks->pst_AllTracks;
	iCount = 0;
	while(pos)
	{
		po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && (!mpo_Parent->mst_Ini.i_DisplayHidden))
		{
			pst_Track++;
			continue;
		}

		po_Track->GetWindowRect(&o_Rect1);
		ScreenToClient(&o_Rect1);
		o_Rect1.left = 0;
		o_Rect1.right = mpo_Parent->mst_Ini.i_XLeft - 1;
		pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
		o_Rect1.InflateRect(-1, -1);

		/* Selected track ? */
		if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
			pdc->FillSolidRect(&o_Rect1, 0x000000FF);
		else if(pt_SelGao && pt_SelGao == pt_GetGaoFromTrack(po_Track->mpst_Track, o_Name))
			pdc->FillSolidRect(&o_Rect1, GetSysColor(COLOR_BTNFACE) | 0xFF);
		else
			pdc->FillSolidRect(&o_Rect1, GetSysColor(COLOR_BTNFACE));

		/* Color */
		col = po_Track->mpst_Track->c_Color;
		if(col > 4) col = 4;
		o_Rect2 = o_Rect1;
		o_Rect2.left++;
		o_Rect2.right = o_Rect2.left + 2;
		switch(col)
		{
		case 0: colcol = 0x00FF0000; break;
		case 1: colcol = 0x0000FF00; break;
		case 2: colcol = 0x000000FF; break;
		case 3: colcol = 0x0000FFFF; break;
		case 4: colcol = 0x00FFFF00; break;
		}

		pdc->Draw3dRect(&o_Rect2, colcol, colcol);

		/* Hidden mark */
		pdc->IntersectClipRect(&o_Rect);
		o_Rect2 = o_Rect1;
		o_Rect2.left += 5;
		o_Rect2.right = o_Rect2.left + 18;
		pdc->Draw3dRect(&o_Rect2, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
		if(!(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden))
			DrawIconEx(pdc->GetSafeHdc(), o_Rect2.left + 1, o_Rect2.top + 1, hicon, 16, 16, 0, 0, DI_NORMAL);

		/* Display curve */
		o_Rect2.left = o_Rect2.right + 2;
		o_Rect2.right = o_Rect2.left + 18;
		pdc->Draw3dRect(&o_Rect2, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
		if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Curve)
			DrawIconEx(pdc->GetSafeHdc(), o_Rect2.left + 1, o_Rect2.top + 1, hicon1, 16, 16, 0, 0, DI_NORMAL);

		/* Name of track */
		o_Rect1.left = o_Rect2.right + 2;
		if(mpo_Parent->mst_Ini.i_DisplayTrackName)
		{
			o_Name = pst_Track->asz_Name;
		}
		else
		{
			pst_GO = pt_GetGaoFromTrack(pst_Track, o_Name);
		}

		pdc->SetBkMode(TRANSPARENT);
		pdc->ExtTextOut(o_Rect1.left + 2, o_Rect1.top, ETO_CLIPPED, &o_Rect1, o_Name, NULL);

		o_Rect1.left = mpo_Parent->mst_Ini.i_XLeft - 1;
		o_Rect1.right = mpo_Parent->mst_Ini.i_XLeft;
		pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DDKSHADOW));
		pst_Track++;
		iCount++;
	}

	/* The rest */
	if(iCount) o_Rect1.top = o_Rect1.bottom;
	o_Rect1.bottom = o_Rect.bottom;
	if(o_Rect1.top < o_Rect1.bottom)
	{
		o_Rect1.left = 0;
		o_Rect1.top++;
		o_Rect1.right = mpo_Parent->mst_Ini.i_XLeft;
		pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
		o_Rect1.InflateRect(-1, -1);
		pdc->FillSolidRect(&o_Rect1, GetSysColor(COLOR_3DFACE));
	}

	/* Regle */
	if(hfnt) pdc->SelectObject(hfnt);
	pdc->SelectClipRgn(NULL);
	OnDrawRegle(pdc);
	OnDrawMarks();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::OnDrawRegle(CDC *pdc)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	CRect	o_Rect1;
	CRect	o_Rect2;
	CRect	o_Rect3;
	CRect	o_Rect4;
	int		i, j;
	CFont	*pofnt;
	char	asz_Name[128];
	CSize	o_Size;
	CPoint	o_Mouse;
	int		i_Cpt;
	int		i_MaxX;
	int		i_MaxInter;
	/*~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Parent->mpst_Data) return;

	i_Cpt = 1;
	GetCursorPos(&o_Mouse);
	ScreenToClient(&o_Mouse);

	GetClientRect(&o_Rect);
	o_Rect.right += mpo_Scroll->GetScrollPosition().x;
	i_MaxX = o_Rect.right;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	GetClientRect(&o_Rect);
	o_Rect.left = mpo_Parent->mst_Ini.i_XLeft;;
	o_Rect.top = EVE_C_YRegleSel;
	o_Rect.bottom = o_Rect.top + (EVE_C_YRegle - EVE_C_YRegleSel);
	pdc->Draw3dRect(&o_Rect, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
	o_Rect.InflateRect(-1, -1);
	pdc->FillSolidRect(&o_Rect, GetSysColor(COLOR_BTNFACE));

	/* Clip */
	GetClientRect(&o_Rect);
	o_Rect.left = mpo_Parent->mst_Ini.i_XLeft;
	pdc->IntersectClipRect(&o_Rect);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	GetClientRect(&o_Rect);
	o_Rect.left = mpo_Parent->mst_Ini.i_XLeft - mpo_Scroll->GetScrollPosition().x;
	o_Rect.top = 0;
	o_Rect.right += mpo_Scroll->GetScrollPosition().x;
	o_Rect.bottom = o_Rect.top + EVE_C_YRegleSel;

	/* Origin unit */
	o_Rect1 = o_Rect;
	o_Rect1.top += 12;
	o_Rect1.right = o_Rect1.left + EEVE_C_CXBorder + 1;
	o_Rect4 = o_Rect1;
	pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DSHADOW));
	o_Rect1.left++;
	pdc->FillSolidRect(&o_Rect1, GetSysColor(COLOR_ACTIVECAPTION));
	pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
	pdc->ExcludeClipRect(&o_Rect4);
	o_Rect.left += EEVE_C_CXBorder;

	pofnt = pdc->SelectObject(&mpo_Parent->mo_Fnt);

	o_Rect1 = o_Rect;
	o_Rect1.top += 12;

	if(mpo_Parent->mst_Ini.i_Inter > 15)
		i_MaxInter = 1;
	else
		i_MaxInter = 2;

	for(i = 0; !mpo_Parent->mst_Ini.i_AllEventsVis && i < o_Rect.Width() / mpo_Parent->mst_Ini.f_Factor; i++)
	{
		o_Rect3 = o_Rect1;
		o_Rect3.left += (int) (mpo_Parent->mst_Ini.f_Factor + 0.5f);

		/* Interval */
		o_Rect2 = o_Rect1;
		for(j = 1; j < mpo_Parent->mst_Ini.f_DivFactor; j++)
		{
			o_Rect2.top = o_Rect1.top + 2;
			o_Rect2.bottom = o_Rect1.bottom - 3;
			o_Rect2.left += (int) ((mpo_Parent->mst_Ini.f_Factor / mpo_Parent->mst_Ini.f_DivFactor) + 0.5f);
			o_Rect2.left -= (EEVE_C_CXBorder + 1);
			o_Rect2.right = o_Rect2.left + EEVE_C_CXBorder + 1;
			o_Rect4 = o_Rect2;
			pdc->Draw3dRect(&o_Rect2, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DSHADOW));
			o_Rect2.left++;
			o_Rect2.top++;
			o_Rect2.bottom--;
			pdc->FillSolidRect(&o_Rect2, GetSysColor(COLOR_3DSHADOW));
			pdc->Draw3dRect(&o_Rect2, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
			o_Rect2.left = o_Rect2.right;
			pdc->ExcludeClipRect(&o_Rect4);

			/* Label */
			if(!mpo_Parent->mst_Ini.i_AllEventsVis)
			{
				if((i_Cpt % i_MaxInter) == 0)
				{
					if(mpo_Parent->mst_Ini.i_TimeDisplay == EEVE_C_OptionTimeFrame)
					{
						sprintf
						(
							asz_Name,
							"%d",
							(int)
								(
									(float) i_Cpt *
									60.0f *
									mpo_Parent->mst_Ini.f_ValFactor /
									mpo_Parent->mst_Ini.f_DivFactor
								)
						);
					}
					else
					{
						sprintf
						(
							asz_Name,
							"%.2f",
							((float) (i) * mpo_Parent->mst_Ini.f_ValFactor) +
								(j * mpo_Parent->mst_Ini.f_ValFactor / mpo_Parent->mst_Ini.f_DivFactor)
						);
					}

					o_Size = pdc->GetTextExtent(asz_Name);
					o_Rect4.left = o_Rect4.right;
					o_Rect4.left = o_Rect4.left - (EEVE_C_CXBorder / 2) - (o_Size.cx / 2) - 2;
					o_Rect4.top -= o_Size.cy;
					o_Rect4.right = o_Rect4.left + o_Size.cx;
					o_Rect4.bottom = o_Rect4.top + o_Size.cy;
					pdc->SetBkColor(GetSysColor(COLOR_3DSHADOW));
					pdc->ExtTextOut(o_Rect4.left, o_Rect4.top, ETO_OPAQUE, &o_Rect4, asz_Name, NULL);
					pdc->ExcludeClipRect(&o_Rect4);
				}
			}

			i_Cpt++;
		}

		/* Unit */
		o_Rect1.left += (int) (mpo_Parent->mst_Ini.f_Factor + 0.5f) - EEVE_C_CXBorder - 1;
		o_Rect1.right = o_Rect1.left + EEVE_C_CXBorder + 1;
		o_Rect4 = o_Rect1;
		pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DSHADOW));
		o_Rect1.left++;
		pdc->FillSolidRect(&o_Rect1, GetSysColor(COLOR_ACTIVECAPTION));
		pdc->Draw3dRect(&o_Rect1, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
		o_Rect1.left = o_Rect1.right;
		pdc->ExcludeClipRect(&o_Rect4);

		/* Unit label */
		if(!mpo_Parent->mst_Ini.i_AllEventsVis)
		{
			if(mpo_Parent->mst_Ini.i_TimeDisplay == EEVE_C_OptionTimeFrame)
			{
				sprintf(asz_Name, "%d", i_Cpt);
				i_Cpt++;
			}
			else
			{
				sprintf(asz_Name, "%.2f", (float) (i + 1) * mpo_Parent->mst_Ini.f_ValFactor);
				i_Cpt = 0;
			}

			o_Size = pdc->GetTextExtent(asz_Name);
			o_Rect4 = o_Rect1;
			o_Rect4.left = o_Rect4.left - (EEVE_C_CXBorder / 2) - (o_Size.cx / 2) - 2;
			o_Rect4.top -= o_Size.cy;
			o_Rect4.right = o_Rect4.left + o_Size.cx;
			o_Rect4.bottom = o_Rect4.top + o_Size.cy;
			pdc->SetBkColor(GetSysColor(COLOR_3DSHADOW));
			pdc->ExtTextOut(o_Rect4.left, o_Rect4.top, ETO_OPAQUE, &o_Rect4, asz_Name, NULL);
			pdc->ExcludeClipRect(&o_Rect4);
		}

		if(o_Rect4.left > i_MaxX) break;
	}

	GetClientRect(&o_Rect);
	o_Rect.left = mpo_Parent->mst_Ini.i_XLeft;
	o_Rect.top = 0;
	o_Rect.right += mpo_Scroll->GetScrollPosition().x;
	o_Rect.bottom = EVE_C_YRegleSel;
	pdc->FillSolidRect(o_Rect, GetSysColor(COLOR_3DSHADOW));
	pdc->Draw3dRect(o_Rect, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));

	/* First realtime pos of first track ? */
	DrawRealTime();

	pdc->SelectObject(pofnt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::DrawRealTime(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CDC				*pdc;
	EEVE_cl_Track	*po_Track;
	CRect			o_Rect;
	CRect			o_Rect1;
	POSITION		pos;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpo_Parent->mo_ListTracks.IsEmpty()) return;

	pdc = GetDC();

	/* Real time */
	if(mpo_Parent->mst_Ini.b_RunSel)
	{
		pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
		while(pos)
		{
			po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
			if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected) break;
		}
	}
	else
	{
		pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
		while(pos)
		{
			po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
			if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && (!mpo_Parent->mst_Ini.i_DisplayHidden))
				continue;
			break;
		}
	}

	if(po_Track)
	{
		GetClientRect(&o_Rect);
		o_Rect.top = EVE_C_YRegleSel;
		o_Rect.bottom = EVE_C_YRegle;
		o_Rect.left = po_Track->mo_TrackTime.left - mpo_Scroll->GetScrollPosition().x;
		o_Rect.right = po_Track->mo_TrackTime.right - mpo_Scroll->GetScrollPosition().x;
		o_Rect.left += mpo_Parent->mst_Ini.i_XLeft;
		o_Rect.right += mpo_Parent->mst_Ini.i_XLeft;
		if(o_Rect.left >= mpo_Parent->mst_Ini.i_XLeft)
		{
			o_Rect.left--;
			o_Rect1 = o_Rect;
			pdc->Draw3dRect(o_Rect, 0, 0);
			o_Rect.left++;
			o_Rect.right--;
			pdc->FillSolidRect(o_Rect, 0x00FF50FF);
			pdc->ExcludeClipRect(&o_Rect1);
		}
	}

	GetClientRect(&o_Rect);
	o_Rect.top = EVE_C_YRegleSel;
	o_Rect.bottom = EVE_C_YRegle;
	o_Rect.left = mo_Marker.left - mpo_Scroll->GetScrollPosition().x;
	o_Rect.right = mo_Marker.right - mpo_Scroll->GetScrollPosition().x;
	o_Rect.left += mpo_Parent->mst_Ini.i_XLeft;
	o_Rect.right += mpo_Parent->mst_Ini.i_XLeft;
	if(o_Rect.left >= mpo_Parent->mst_Ini.i_XLeft)
	{
		o_Rect.left--;
		o_Rect1 = o_Rect;
		pdc->Draw3dRect(o_Rect, 0, 0);
		o_Rect.left++;
		o_Rect.right--;
		pdc->FillSolidRect(o_Rect, 0x0050FFFF);
		pdc->ExcludeClipRect(&o_Rect1);
	}

	/* Selection box */
	GetClientRect(&o_Rect);
	o_Rect.left = mpo_Parent->mst_Ini.i_XLeft;
	o_Rect.top = EVE_C_YRegleSel;
	o_Rect.bottom = EVE_C_YRegle;
	pdc->Draw3dRect(o_Rect, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
	o_Rect.InflateRect(-1, -1);
	pdc->FillSolidRect(o_Rect, GetSysColor(COLOR_BTNFACE));

	ReleaseDC(pdc);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::OnMouseMove(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint			o_Scroll;
	CDC				*pdc;
	CRect			o_Rect;
	CRect			o_Rect1;
	CPoint			o_Cursor;
	CPoint			o_Cursor1;
	EEVE_cl_Track	*po_Track;
	CPoint			pt1;
	POSITION		pos;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* On mark ? */
	if(PtToMark(pt) != -1)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		return;
	}

	/* Split left */
	if(mb_MoveLeft)
	{
		if(pt.x - mo_PanePt.x == 0) return;
		mpo_Parent->mst_Ini.i_XLeft += pt.x - mo_PanePt.x;
		if(mpo_Parent->mst_Ini.i_XLeft < 15) mpo_Parent->mst_Ini.i_XLeft = 15;
		mo_PanePt = pt;
		mpo_Parent->ForceRefresh();
		return;
	}

	/* Select */
	if(mb_SelectMode)
	{
		pdc = GetWindowDC();
		o_Rect.left = mo_PanePt.x;
		o_Rect.top = EVE_C_YRegleSel;
		o_Rect.right = pt.x;
		o_Rect.bottom = 1000000;
		o_Rect.NormalizeRect();
		if(o_Rect.left < mpo_Parent->mst_Ini.i_XLeft) o_Rect.left = mpo_Parent->mst_Ini.i_XLeft;

		ClientToScreen(&o_Rect);
		mpo_Parent->SelectInRect(&o_Rect);
		ScreenToClient(&o_Rect);

		o_Rect1 = o_Rect;
		o_Rect1.top = EVE_C_YRegleSel;
		o_Rect1.bottom = EVE_C_YRegle;
		pdc->FillSolidRect(&o_Rect1, 0x000000FF);

		if(mo_ZoomRect.right > o_Rect.right)
		{
			o_Rect1 = mo_ZoomRect;
			o_Rect1.left = o_Rect.right;
			o_Rect1.right = mo_ZoomRect.right;
			InvalidateRect(&o_Rect1);
		}

		mo_ZoomRect = o_Rect;
		ReleaseDC(pdc);
		return;
	}

	/* Select track mode */
	if(mb_SelTrackMode)
	{
		po_Track = po_GetTrackWithY(pt.y);
		if(po_Track && po_Track != mpo_LastSelTrack)
		{
			mpo_LastSelTrack = po_Track;
			SelectTrackMode(pt);
		}

		return;
	}

	/* First realtime pos of first track ? */
	if(mpo_Parent->mo_ListTracks.GetCount())
	{
		if(mpo_Parent->mst_Ini.b_RunSel)
		{
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected) break;
			}
		}
		else
		{
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && (!mpo_Parent->mst_Ini.i_DisplayHidden))
					continue;
				break;
			}
		}

		if(po_Track)
		{
			pt1 = pt;
			pt1.x -= mpo_Parent->mst_Ini.i_XLeft;
			pt1.x += mpo_Scroll->GetScrollPosition().x;
			o_Rect = po_Track->mo_TrackTime;
			o_Rect.top = pt1.y - 1;
			o_Rect.bottom = pt1.y + 1;
			if(o_Rect.PtInRect(pt1))
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				return;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::OnLButtonUp(UINT, CPoint pt)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(mb_SelectMode)
	{
		GetClientRect(&o_Rect);
		o_Rect.top = EVE_C_YRegleSel;
		o_Rect.bottom = EVE_C_YRegle;
		InvalidateRect(&o_Rect);
		mb_SelectMode = FALSE;
		ReleaseCapture();
		return;
	}

	if(mb_SelTrackMode)
	{
		mb_SelTrackMode = FALSE;
		ReleaseCapture();
		return;
	}

	if(mb_MoveLeft)
	{
		mb_MoveLeft = FALSE;
		ReleaseCapture();
		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EEVE_cl_Track *EEVE_cl_Inside::po_GetTrackWithY(int y)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	CRect			o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
		if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && (!mpo_Parent->mst_Ini.i_DisplayHidden)) continue;
		po_Track->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);
		if(o_Rect.PtInRect(CPoint(o_Rect.left + 1, y))) return po_Track;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::SelectTrackMode(CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Track	*po_Track, *po_Track1, *po_Track2;
	POSITION		pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Track = po_GetTrackWithY(pt.y);
	if(!po_Track) return;

	/* Color */
	if(!mb_SelTrackMode)
	{
		if(pt.x < 5)
		{
			po_Track->mpst_Track->c_Color++;
			if(po_Track->mpst_Track->c_Color > 4) po_Track->mpst_Track->c_Color = 0;
			Invalidate();
			return;
		}

		/* Display or hide tracks */
		if((pt.x < 23) && (pt.x > 5))
		{
			DisplayHideTracks(po_Track);
			return;
		}

		/* Display or hide curves */
		if(pt.x < 45)
		{
			DisplayHideCurves(po_Track);
			return;
		}
	}

	if(GetAsyncKeyState(VK_CONTROL) < 0)
	{
		if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
			po_Track->Unselect();
		else
			po_Track->Select();
	}
	else if((GetAsyncKeyState(VK_SHIFT) < 0) || (mb_SelTrackMode))
	{
		pos = mpo_Parent->mo_ListTracks.Find(po_Track);
		mpo_Parent->mo_ListTracks.GetPrev(pos);
		po_Track2 = NULL;
		while(pos)
		{
			po_Track1 = mpo_Parent->mo_ListTracks.GetAt(pos);
			mpo_Parent->mo_ListTracks.GetPrev(pos);
			if(po_Track1->mpst_Track->uw_Flags & EVE_C_Track_Selected)
				po_Track2 = po_Track1;
			else if(po_Track2)
				break;
		}

		if(GetAsyncKeyState(VK_CONTROL) >= 0) po_Track->UnselectTracks();
		if(po_Track2)
			pos = mpo_Parent->mo_ListTracks.Find(po_Track2);
		else
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
		while(po_Track2 != po_Track)
		{
			po_Track2 = mpo_Parent->mo_ListTracks.GetNext(pos);
			po_Track2->Select();
		}

		po_Track2->Select();
	}
	else
	{
		if(!mb_SelTrackMode) po_Track->UnselectTracks();
		po_Track->Select();
	}

	/* Change selected track in view */
	M_MF()->LockDisplay(mpo_Parent->mpo_VarsView);
	if(mpo_Parent->mpo_LastSelected)
		mpo_Parent->mpo_LastSelected->OnLButtonDown();
	else
		mpo_Parent->mpo_VarsView->ResetList();
	po_Track->FillBar();
	M_MF()->UnlockDisplay(mpo_Parent->mpo_VarsView);
	UpdateWindow();

	/* Mode sel */
	mb_SelTrackMode = TRUE;
	SetCapture();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EEVE_cl_Inside::PtToMark(CPoint pt)
{
	/*~~~~~~~~~~*/
	UINT	frame;
	UINT	i;
	/*~~~~~~~~~~*/

	frame = PtToFrame(pt);
	for(i = 0; i < mpo_Parent->mt_Tre.ui_NumMark; i++)
	{
		if(abs((int)mpo_Parent->mt_Tre.ui_FrameMark[i] - (int)frame) < 10)
		{
			return i;
		}
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EEVE_cl_Inside::PtToFrame(CPoint pt)
{
	/*~~~~~~~~~*/
	float	time;
	/*~~~~~~~~~*/

	pt.x += mpo_Scroll->GetScrollPosition().x;
	pt.x -= mpo_Parent->mst_Ini.i_XLeft;
	time = pt.x / mpo_Parent->mst_Ini.f_Factor;
	time *= mpo_Parent->mst_Ini.f_ValFactor;
	return(UINT) (time * 60.0f);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::FrameToPt(UINT frame, CPoint &pt)
{
	/*~~~~~~~~~*/
	float	time;
	/*~~~~~~~~~*/

	time = frame / 60.0f;
	time *= mpo_Parent->mst_Ini.f_Factor;
	time /= mpo_Parent->mst_Ini.f_ValFactor;
	pt.x = (UINT) time;
	pt.x -= mpo_Scroll->GetScrollPosition().x;
	pt.x += mpo_Parent->mst_Ini.i_XLeft;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::OnDrawMarks(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	UINT	i;
	CPoint	pt, pt1;
	CDC		*pdc;
	CRect	rect, rect1;
	HFONT	hfnt;
	int		lasthide, maxhide;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	GetClientRect(&rect);
	pdc = GetDC();
	hfnt = (HFONT) pdc->SelectObject(&M_MF()->mo_Fnt);

	lasthide = -1;
	maxhide = -10000000;
	for(i = 0; i < mpo_Parent->mt_Tre.ui_NumMark; i++)
	{
		FrameToPt(mpo_Parent->mt_Tre.ui_FrameMark[i], pt);
		pt.x -= GetScrollPosition().x;
		if(pt.x < mpo_Parent->mst_Ini.i_XLeft)
		{
			if(pt.x > maxhide)
			{
				lasthide = i;
				maxhide = pt.x;
			}
		}
	}

	for(i = 0; i < mpo_Parent->mt_Tre.ui_NumMark; i++)
	{
		FrameToPt(mpo_Parent->mt_Tre.ui_FrameMark[i], pt);
		if(i == lasthide)
		{
			pdc->SetBkMode(OPAQUE);
			pdc->SetBkColor(0x00FF0000);
			pdc->SetTextColor(0x00FFFFFF);
			rect1 = CRect(mpo_Parent->mst_Ini.i_XLeft + 2, 21, 10, 10);
			pdc->ExtTextOut(rect1.left, rect1.top, 0, &rect1, mpo_Parent->mt_Tre.az_Marks[i], NULL);
		}
		else
		{
			pt1 = pt;
			pt1.x -= GetScrollPosition().x;
			if(pt1.x > mpo_Parent->mst_Ini.i_XLeft)
			{
				pdc->FillSolidRect(pt.x, 21, 2, rect.bottom - 45, 0x00FF0000);

				pdc->SetBkMode(OPAQUE);
				pdc->SetBkColor(0x00FF0000);
				pdc->SetTextColor(0x00FFFFFF);
				rect1 = CRect(pt.x, 21, 10, 10);
				pdc->ExtTextOut(rect1.left, rect1.top, 0, &rect1, mpo_Parent->mt_Tre.az_Marks[i], NULL);
			}
		}
	}

	pdc->SelectObject(hfnt);
	ReleaseDC(pdc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::OnRButtonDown(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu				o_Menu(FALSE);
	int							i_Res, Rank;
	BOOL						b_Selected;
	POSITION					pos;
	EEVE_cl_Track				*po_Track;
	CPoint						pt1;
	ULONG						ul_NumSelectedTracks;
	EDIA_cl_NameDialog			o_Dialog("Enter Track Name", 15);
	EDIA_cl_NameDialog			o_Dialog1("Enter Name", 64);
	OBJ_tdst_GameObject			*pst_GO;
	int							mark, index;
	struct MDF_tdst_Modifier_	*pst_Modifier;
	EEVE_cl_Event				*po_Event;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&pt);
	pt1 = pt;
	ScreenToClient(&pt1);

	/* Barre de selection */
	if(pt1.x > mpo_Parent->mst_Ini.i_XLeft)
	{
		mark = PtToMark(pt1);
		M_MF()->InitPopupMenuAction(NULL, &o_Menu);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Add Mark", -1);
		if(mark != -1)
		{
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Rename Mark", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Delete Mark", -1);
		}

		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Delete All Marks", -1);

		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		if(i_Res <= 0) return;
		switch(i_Res)
		{
		case 1:
			if(o_Dialog1.DoModal() != IDCANCEL)
			{
				mpo_Parent->mt_Tre.ui_NumMark++;
				L_strcpy
				(
					mpo_Parent->mt_Tre.az_Marks[mpo_Parent->mt_Tre.ui_NumMark - 1],
					(char *) (LPCSTR) o_Dialog1.mo_Name
				);

				mpo_Parent->mt_Tre.ui_FrameMark[mpo_Parent->mt_Tre.ui_NumMark - 1] = PtToFrame(pt1);
				Invalidate();
			}
			break;

		case 2:
			o_Dialog1.mo_Name = mpo_Parent->mt_Tre.az_Marks[mark];
			if(o_Dialog1.DoModal() == IDOK)
			{
				L_strcpy(mpo_Parent->mt_Tre.az_Marks[mark], (char *) (LPCSTR) o_Dialog1.mo_Name);
				Invalidate();
			}
			break;

		case 3:
			mpo_Parent->mt_Tre.ui_NumMark--;
			L_strcpy(mpo_Parent->mt_Tre.az_Marks[mark], mpo_Parent->mt_Tre.az_Marks[mpo_Parent->mt_Tre.ui_NumMark]);
			mpo_Parent->mt_Tre.ui_FrameMark[mark] = mpo_Parent->mt_Tre.ui_FrameMark[mpo_Parent->mt_Tre.ui_NumMark];
			Invalidate();
			break;

		case 4:
			mpo_Parent->mt_Tre.ui_NumMark = 0;
			Invalidate();
			break;
		}

		return;
	}

	/* Sur les pistes */
	if(pt1.x < mpo_Parent->mst_Ini.i_XLeft)
	{
		po_Track = po_GetTrackWithY(pt1.y);
		Rank = 1;
		b_Selected = FALSE;
		ul_NumSelectedTracks = 0;
		pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
		while(pos)
		{
			po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
			if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
			{
				b_Selected = TRUE;
				ul_NumSelectedTracks++;
			}

			if(!b_Selected) Rank++;
		}

		M_MF()->InitPopupMenuAction(NULL, &o_Menu);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Select All", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Invert Selection", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "", -1);

		if(ul_NumSelectedTracks == 1)
		{
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Delete Track", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 6, TRUE, "Copy Track", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 7, TRUE, "Insert copied Track(s)", -1);
		}
		else
		{
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Delete Tracks", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 6, TRUE, "Copy Tracks", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 7, TRUE, "Insert copied Track(s)", -1);
		}

		if(po_Track && (ul_NumSelectedTracks == 1))
		{
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Set Track Name", -1);

			// Open/Close morph editor for animators ?
			if (mpo_Parent->mo_MorphEditor != NULL)
			{
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "", -1);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 16, TRUE, "Close Morph Editor", -1);
			}
			else
			{
				// Find selected track
				pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
				while(pos)
				{
					po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
					if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected) break;
				}
				// Is there a "OBJ_ModuleMorphDataSet" AI function event in the track ? If there is,
				// Find the GAO with the morph datas
				for(index = 0; index < po_Track->mo_ListEvents.GetSize(); index++)
				{
					po_Event = po_Track->mo_ListEvents.GetAt(index);
					if (po_Event->b_TestAIFunctionType(FUNCTION_OBJ_MODULEMORPHSETCHDATA)) break;
				}
				if (index < po_Track->mo_ListEvents.GetSize())
				{
					// Find the GAO with the morph datas
					pst_GO = po_Track->mpst_Track->pst_GO;
					pst_GO = ANI_pst_GetObjectByAICanal( pst_GO, (UCHAR)po_Event->i_GetIntParameter(0));

					// Does it really have morph modifier ?
					pst_Modifier = MDF_pst_GetByType(pst_GO, MDF_C_Modifier_Morphing);
					if (pst_Modifier != NULL) {
						M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "", -1);
						M_MF()->AddPopupMenuAction(NULL, &o_Menu, 15, TRUE, "Open Morph Editor", -1);
					}
				}
			}
		}

		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 10, TRUE, "Select Object", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 5, TRUE, "New Track", -1);

		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		if(i_Res <= 0) return;
		switch(i_Res)
		{
		case 1:
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				po_Track->Select();
			}
			break;

		case 2:
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
					po_Track->Unselect();
				else
					po_Track->Select();
			}
			break;

		case 3:
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected) break;
			}

			o_Dialog.mo_Name = po_Track->mpst_Track->asz_Name;
			if(o_Dialog.DoModal() == IDOK)
			{
				L_strcpy(po_Track->mpst_Track->asz_Name, (char *) (LPCSTR) o_Dialog.mo_Name);
				Invalidate();
			}
			break;

		case 4:
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
				{
					mpo_Parent->SaveForUndo();
					mpo_Parent->OnDeleteTrack(po_Track);
				}
			}
			break;

		case 5:
			mpo_Parent->SaveForUndo();
			mpo_Parent->OnNewTrack();
			break;

		case 6:
			mpo_Parent->CopySelectedTracks();
			break;

		case 7:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				extern BOOL EVE_gb_CanCloseUndo;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				mpo_Parent->SaveForUndo();
				if(b_Selected)
					mpo_Parent->PasteCopiedTracks(Rank);
				else
					mpo_Parent->PasteCopiedTracks(mpo_Parent->mpst_ListTracks->uw_NumTracks);

				EVE_gb_CanCloseUndo = FALSE;
				mpo_Parent->SetGAO(mpo_Parent->mpst_GAO, FALSE, TRUE);
				break;
			}

		case 10:
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if (!po_Track || po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected) break;
			}
            if (!po_Track)
                break;

			pst_GO = po_Track->mpst_Track->pst_GO;
			if(!pst_GO) pst_GO = mpo_Parent->mpst_GAO;
			if(pst_GO && (int) pst_GO != -1)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				ANI_st_GameObjectAnim	*pst_GOAnim;
				TAB_tdst_PFelem			*pst_CurrentBone;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if
				(
					(po_Track->mpst_Track->uw_Gizmo != 0xFFFF)
				&&	(pst_GO->pst_Base)
				&&	(pst_GO->pst_Base->pst_AddMatrix)
				&&	(pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo)
				&&	(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer))
				)
				{
					pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;
					pst_CurrentBone = pst_GOAnim->pst_Skeleton->pst_AllObjects->p_Table + po_Track->mpst_Track->uw_Gizmo;
					pst_GO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
				}
			}

			for(int i = 0; i < mpo_Parent->mi_NumberOfLinkedOutputEditors; i++)
			{
				mpo_Parent->mapo_LinkedOutputEditor[i]->mpo_EngineFrame->mpo_DisplayView->Selection_b_Treat
					(
						pst_GO,
						(long) pst_GO,
						SEL_C_SIF_Object,
						FALSE
					);
			}

			LINK_Refresh();
			break;

		case 15:
			// Find selected track
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected) break;
			}
			// Find the "OBJ_ModuleMorphDataSet" AI function event, then find
			// the GAO with the morph datas
			for(index = 0; index < po_Track->mo_ListEvents.GetSize(); index++)
			{
				po_Event = po_Track->mo_ListEvents.GetAt(index);
				if (po_Event->b_TestAIFunctionType(FUNCTION_OBJ_MODULEMORPHSETCHDATA)) break;
			}
			if (index < po_Track->mo_ListEvents.GetSize())
			{
				// Find the GAO with the morph datas
				pst_GO = po_Track->mpst_Track->pst_GO;
				pst_GO = ANI_pst_GetObjectByAICanal( pst_GO, (UCHAR)po_Event->i_GetIntParameter(0));
				// Does it really have morph modifier
				// (useless : it must have or the case 15 wouldn't be possible) ?
				pst_Modifier = MDF_pst_GetByType(pst_GO, MDF_C_Modifier_Morphing);
				if (pst_Modifier != NULL) {
					mpo_Parent->OpenMorphEditor(pst_Modifier, pst_GO);
				}
			}
			break;

		case 16:
			mpo_Parent->CloseMorphEditor();
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::OnLButtonDblClk(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EEVE_cl_Track	*po_Track, *po_Sel;
	POSITION		pos;
	CString			o_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Cache/Montre les pistes qui ont le meme gao */
	if((pt.y > EVE_C_YRegle) && (pt.x < mpo_Parent->mst_Ini.i_XLeft))
	{
		po_Sel = NULL;
		pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
		while(pos)
		{
			po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
			if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
			{
				po_Sel = po_Track;
				break;
			}
		}

		if(po_Sel)
		{
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if
				(
					po_Track != po_Sel
				&&	pt_GetGaoFromTrack(po_Track->mpst_Track, o_Name) == pt_GetGaoFromTrack
						(
							po_Sel->mpst_Track,
							o_Name
						)
				)
				{
					if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden)
						po_Track->mpst_Track->uw_Flags &= ~EVE_C_Track_Hidden;
					else
						po_Track->mpst_Track->uw_Flags |= EVE_C_Track_Hidden;
				}
			}

			mpo_Parent->ForceRefresh();
			return;
		}
	}
	else
	{
		if(mpo_Parent->mst_Ini.b_RunSel)
		{
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected) break;
			}
		}
		else
		{
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && (!mpo_Parent->mst_Ini.i_DisplayHidden))
					continue;
				break;
			}
		}

		if(po_Track) mo_Marker = po_Track->mo_TrackTime;
	}

	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::OnLButtonDown(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	EVE_tdst_Params *pst_Param;
	float			f_Total;
	int				i, index;
	CRect			o_Rect;
	CPoint			pt1;
	CPoint			pt2;
	UINT			frame, bef;
	MSG				msg;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Marks */
	i = PtToMark(pt);
	if(i != -1)
	{
		bef = mpo_Parent->mt_Tre.ui_FrameMark[i];
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		SetCapture();
		while(1)
		{
			::GetMessage(&msg, NULL, 0, 0);
			if(msg.message == WM_LBUTTONUP) break;
			if(msg.message != WM_MOUSEMOVE) continue;
			if(msg.message == WM_RBUTTONDOWN)
			{
				mpo_Parent->mt_Tre.ui_FrameMark[i] = bef;
				break;
			}

			GetCursorPos(&pt);
			ScreenToClient(&pt);
			frame = PtToFrame(pt);
			mpo_Parent->mt_Tre.ui_FrameMark[i] = frame;
			Invalidate();
			RedrawWindow();
		}

		ReleaseCapture();
		return;
	}

	if(!mpo_Parent->mpst_Data) return;
	pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
	if(!pos) return;
	po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
	po_Track->mi_DecSize = 0;
	pt2 = pt;
	po_Track->SnapOtherTracks(&pt);

	/* On regle */
	if((pt.y < EVE_C_YRegleSel) && (pt.x > mpo_Parent->mst_Ini.i_XLeft))
	{
		for(index = 0; index < mpo_Parent->mpst_Data->pst_ListTracks->uw_NumTracks; index++)
		{
			pst_Param = &mpo_Parent->mpst_Data->pst_ListParam[index];
			po_Track = mpo_Parent->mo_ListTracks.GetAt(mpo_Parent->mo_ListTracks.FindIndex(index));
			if(!pst_Param) continue;
			if(!(pst_Param->uw_Flags & EVE_C_Track_Running)) continue;
			if(pst_Param->uw_Flags & EVE_C_Track_Pause) continue;
			if(po_Track->mpst_Track->pst_AllEvents)
			{
				f_Total = 0.0f;
				for(i = 0; i < pst_Param->uw_CurrentEvent; i++)
					f_Total += EVE_FrameToTime(po_Track->mpst_Track->pst_AllEvents[i].uw_NumFrames);
				f_Total += pst_Param->f_Time;
				f_Total *= mpo_Parent->mst_Ini.f_Factor;
				f_Total /= mpo_Parent->mst_Ini.f_ValFactor;
				f_Total = (float) (pt.x - mpo_Parent->mst_Ini.i_XLeft + mpo_Scroll->GetScrollPosition().x) - f_Total;
				f_Total /= mpo_Parent->mst_Ini.f_Factor;
				f_Total *= mpo_Parent->mst_Ini.f_ValFactor;
				EVE_AddTime
				(
					mpo_Parent->mpst_GAO,
					mpo_Parent->mpst_ListTracks,
					pst_Param,
					po_Track->mpst_Track,
					f_Total
				);
				po_Track->OnPaint();
				EVE_gb_NeedToRecomputeSND = TRUE;
			}
		}

		if(mpo_Parent->mpst_GAO) OBJ_HierarchyMainCall(mpo_Parent->mpst_GAO->pst_World);
		LINK_Refresh();
	}

	/* Sélection */
	else if((pt.y < EVE_C_YRegle) && (pt.x > mpo_Parent->mst_Ini.i_XLeft))
	{
		/* First realtime pos of first track ? */
		if(mpo_Parent->mst_Ini.b_RunSel)
		{
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected) break;
			}
		}
		else
		{
			pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
			while(pos)
			{
				po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
				if((po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden) && (!mpo_Parent->mst_Ini.i_DisplayHidden))
					continue;
				break;
			}
		}

		if(po_Track)
		{
			pt1 = pt2;
			pt1.x -= mpo_Parent->mst_Ini.i_XLeft;
			pt1.x += mpo_Scroll->GetScrollPosition().x;
			o_Rect = po_Track->mo_TrackTime;
			o_Rect.top = pt1.y - 1;
			o_Rect.bottom = pt1.y + 1;
			if(o_Rect.PtInRect(pt1))
			{
				po_Track->OnMouseMove(0, pt1);
				po_Track->OnLButtonDown(0, pt1);
				return;
			}
		}

/*		mo_PanePt = pt;
		o_Rect.left = pt.x;
		o_Rect.top = EVE_C_YRegleSel;
		o_Rect.right = o_Rect.left + 1;
		o_Rect.bottom = 1000000;
		ClientToScreen(&o_Rect);
		mpo_Parent->SelectInRect(&o_Rect);
		ScreenToClient(&o_Rect);
		mo_ZoomRect = o_Rect;
		mb_SelectMode = TRUE;
		SetCapture();*/
	}

	/* Move left */
	else if(pt.x > mpo_Parent->mst_Ini.i_XLeft - 10)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		mo_PanePt = pt;
		mb_MoveLeft = TRUE;
		SetCapture();
	}

	/* Sélection track */
	else
	{
		po_Track = po_GetTrackWithY(pt.y);
		if(po_Track)
		{
//			mb_SelTrackMode = TRUE;
//			SetCapture();
			SelectTrackMode(pt);
			mpo_LastSelTrack = po_Track;
			Invalidate();
		}
		else
		{
			mpo_Parent->UnselectAll();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::DisplayHideTracks(EEVE_cl_Track *_po_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EEVE_cl_Track	*po_Track;
	BOOL			bHide;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* Display or hide ? */
	if(_po_Track->mpst_Track->uw_Flags & EVE_C_Track_Hidden)
		bHide = FALSE;
	else
		bHide = TRUE;

	pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
		if
		(
			(
				(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
			&&	(_po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
			)
		||	(po_Track == _po_Track)
		)
		{
			if(bHide)
				po_Track->mpst_Track->uw_Flags |= EVE_C_Track_Hidden;
			else
				po_Track->mpst_Track->uw_Flags &= ~EVE_C_Track_Hidden;
		}
	}

	mpo_Parent->ForceRefresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::DisplayHideCurves(EEVE_cl_Track *_po_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION				pos;
	EEVE_cl_Track			*po_Track;
	BOOL					bHide;
	EOUT_tdst_CurveParam	st_Curve;
	float					f_InterPerSeconds;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Display or hide ? */
	if(_po_Track->mpst_Track->uw_Flags & EVE_C_Track_Curve)
		bHide = FALSE;
	else
		bHide = TRUE;

	pos = mpo_Parent->mo_ListTracks.GetHeadPosition();
	while(pos)
	{
		po_Track = mpo_Parent->mo_ListTracks.GetNext(pos);
		if
		(
			(
				(po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
			&&	(_po_Track->mpst_Track->uw_Flags & EVE_C_Track_Selected)
			)
		||	(po_Track == _po_Track)
		)
		{
			st_Curve.pst_GAO = mpo_Parent->mpst_GAO;
			st_Curve.pst_Data = mpo_Parent->mpst_Data;
			st_Curve.pst_Track = po_Track->mpst_Track;
			if(bHide)
			{
				f_InterPerSeconds = mpo_Parent->mst_Ini.f_DivFactor / mpo_Parent->mst_Ini.f_ValFactor;
				M_MF()->SendMessageToEditors( EOUT_MESSAGE_CURVE_CHANGEINTERPERSECONDS, *(int *) &f_InterPerSeconds, 0);
				
				po_Track->mpst_Track->uw_Flags |= EVE_C_Track_Curve;
				M_MF()->SendMessageToEditors(EOUT_MESSAGE_ANIMADDCURVE, (ULONG) & st_Curve, 0);
			}
			else
			{
				po_Track->mpst_Track->uw_Flags &= ~EVE_C_Track_Curve;
				M_MF()->SendMessageToEditors(EOUT_MESSAGE_ANIMDELCURVE, (ULONG) & st_Curve, 0);
			}
		}
	}

	LINK_Refresh();
	mpo_Parent->ForceRefresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EEVE_cl_Inside::WhenScroll(int x, int y)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect, o_Rect1;
	/*~~~~~~~~~~~~~~~~~~~~*/

	GetClientRect(&o_Rect);
	if(x)
	{
		o_Rect1 = o_Rect;
		o_Rect1.left = mpo_Parent->mst_Ini.i_XLeft;
		o_Rect1.bottom = EVE_C_YRegle;
		InvalidateRect(&o_Rect1);
	}

	if(y)
	{
		o_Rect1 = o_Rect;
		o_Rect1.top = EVE_C_YRegle;
		o_Rect1.right = mpo_Parent->mst_Ini.i_XLeft;
		InvalidateRect(&o_Rect1);
	}

	mpo_Scroll->UpdateWindow();
	UpdateWindow();
}
#endif /* ACTIVE_EDITORS */
