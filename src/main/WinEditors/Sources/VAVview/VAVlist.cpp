/*$T VAVlist.cpp GC! 1.100 04/03/01 15:32:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h" 
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "LINKs/LINKstruct.h"
#include "VAVview/VAVlist.h"
#include "EDImainframe.h"
#include "Res/Res.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "DIAlogs/DIAchecklist_dlg.h"
#include "DIAlogs/DIAgamematerial_dlg.h"
#include "DIAlogs/DIAgamematerial_inside.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "AIinterp/Sources/AIengine.h"
#include "EDIeditors_infos.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "EDItors/Sources/SOuNd/SONutil.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDstruct.h"
#include "EDItors/Sources/SOuNd/SONframe.h"
#include "EDItors/Sources/SOuNd/SONmsg.h"
#include "EDImsg.h"
#include "SELection/SELection.h"
#include "DIAlogs/DIAselection_dlg.h"
#include "SOFT/SOFTpickingbuffer.h"


#define INDENT_C_STEP	9
extern void EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);
extern void BRO_OrderGroupFile(ULONG _ul_Group);

#ifdef JADEFUSION
extern void AI_AddWatch(void *p, int s);
extern void AI_DelAllWatch(void);
extern int	AI_gi_WatchNum;
#else
extern "C" void AI_AddWatch(void *p, int s);
extern "C" void AI_DelAllWatch(void);
extern "C" int	AI_gi_WatchNum;
#endif

#define M_InsertItem(a, b, c, d) \
	pos = mpo_ListItems->InsertAfter \
		( \
			pos, \
			po_NewItem = new EVAV_cl_ViewItem(a, b, c, EVAV_None, 0, 0, 0, d, po_Item->mx_Color, 1) \
		); \
	po_NewItem->mul_Offset = (char *) po_NewItem->mp_Data - (char *) po_Item->mp_Data;


/*
 =======================================================================================================================
    Array for drawing values. Index must be the same of the enum type.
 =======================================================================================================================
 */
#define EVAV_DEFINE_TYPE(a, b, c, d)	c,
static tdpfnv_DrawValue sgapfnv_DrawArray[] =
{
#include "VAVtypes.h"
};
#undef EVAV_DEFINE_TYPE

/*
 =======================================================================================================================
    Array for drawing values. Index must be the same of the enum type.
 =======================================================================================================================
 */
#define EVAV_DEFINE_TYPE(a, b, c, d)	d,
static tdpfnv_SelectValue	sgapfnv_SelectArray[] =
{
#include "VAVtypes.h"
};
#undef EVAV_DEFINE_TYPE

/*
 =======================================================================================================================
    Array of edit types.
 =======================================================================================================================
 */
#define EVAV_DEFINE_TYPE(a, b, c, d)	b,
static EVAV_tde_VarsViewItemEditType	sgae_EditTypesArray[] =
{
#include "VAVtypes.h"
};
#undef EVAV_DEFINE_TYPE

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EVAV_cl_ListBox, CListBox)
	ON_WM_KILLFOCUS()
	ON_CBN_SELCHANGE(3, OnComboChangeSelect)
	ON_EN_KILLFOCUS(4, OnEditChangeSelect)
	ON_BN_CLICKED(5, OnButtonChangeSelect)
	ON_BN_CLICKED(6, OnCheckChangeSelect)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_CHAR()
END_MESSAGE_MAP()
#define DEC 5

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

EVAV_cl_ViewItem	*EVAV_gpo_Copy = NULL;
tdListItems			*EVAV_gpo_ListItems = NULL;
int					EVAV_gi_ScrollM = 0;
int					EVAV_gpt_ScrollPt = 0;
BOOL				EVAV_gpt_ScrollCan = FALSE;
EVAV_cl_ListBox		*gpo_CurVavListBox = NULL;
MDF_tdst_Modifier * EVAV_gpo_CopyMdF=NULL;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ViewItem::EVAV_cl_ViewItem(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ViewItem::EVAV_cl_ViewItem
(
	char						*_psz_Name,
	EVAV_tde_VarsViewItemType	_e_Type,
	void						*_p_Data,
	ULONG						_ul_Flags,
	int							_i_Param1,
	int							_i_Param2,
	int							_i_Param3,
	EVAV_tdpfnv_Change			_pfn_CB,
	COLORREF					_x_Col,
	int							_i_Param4,
	int							_i_Param5,
	int							_i_Param6,
	int							_i_Param7,
	int							_i_DisplayGroup,
	int							_i_Param8
)
{
	if(L_strlen(_psz_Name) >= EVAV_MaxLenName)
	{
		L_strcpy(masz_Name, _psz_Name + L_strlen(_psz_Name) - EVAV_MaxLenName);
	}
	else
	{
		L_strcpy(masz_Name, _psz_Name);
	}

	me_Type = _e_Type;
	mp_Data = _p_Data;
	mi_NumGroup = 0;
	mi_Display = 1;
	mul_Flags = _ul_Flags;
	mi_Param1 = _i_Param1;
	mi_Param2 = _i_Param2;
	mi_Param3 = _i_Param3;
	mi_Param4 = _i_Param4;
	mi_Param5 = _i_Param5;
	mi_Param6 = _i_Param6;
	mi_Param7 = _i_Param7;
	mi_Param8 = _i_Param8;
	mi_DisplayGroup = _i_DisplayGroup;
	mpfn_CB = _pfn_CB;
	psz_Help = NULL;

	/* Separator */
	mb_Expand = FALSE;
	if(me_Type == EVAV_EVVIT_Separator) mb_Expand = TRUE;
	mb_ExpandOnce = FALSE;
	mi_NumFields = 0;
	mi_ID = 0;
	mi_SubID = 0;
	mx_Color = _x_Col;
	mb_Locked = FALSE;

	/* Default color */
	if(mi_DisplayGroup & EVAV_Filter_Private)
	{
		mx_Color = 0x00BEBEE4;
		if(mi_DisplayGroup & EVAV_Filter_Init) mx_Color = 0x008E8EC4;
	}
	else if(mi_DisplayGroup & EVAV_Filter_Init)
		mx_Color = 0x008EB48B;
	psz_NameKit = NULL;

	size_array_father = 0;
	size_elem_father = 0;
	index_me_father = 0;
	p_data_father = NULL;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ViewItem::~EVAV_cl_ViewItem(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ViewItem::CopyFrom(EVAV_cl_ViewItem *_po_Src)
{
	/* Copy basic parameters */
	L_strcpy(masz_Name, _po_Src->masz_Name); 
	me_Type = _po_Src->me_Type; 
	mp_Data = _po_Src->mp_Data; 
	mul_Flags = _po_Src->mul_Flags; 
	mi_Param1 = _po_Src->mi_Param1; 
	mi_Param2 = _po_Src->mi_Param2; 
	mi_Param3 = _po_Src->mi_Param3; 
	mpfn_CB = _po_Src->mpfn_CB; 
	mx_Color = _po_Src->mx_Color;
	mi_Param4 = _po_Src->mi_Param4;
	mi_Param5 = _po_Src->mi_Param5;
	mi_Param6 = _po_Src->mi_Param6;
	mi_Param7 = _po_Src->mi_Param7;
	psz_Help = _po_Src->psz_Help;
	mul_Offset = _po_Src->mul_Offset;

	/* Copy some infos */
	mi_Display = _po_Src->mi_Display;
	mi_DisplayGroup = _po_Src->mi_DisplayGroup;
	mi_SubID = _po_Src->mi_ID;
	mi_NumFields = _po_Src->mi_NumFields;
	mb_Expand = _po_Src->mb_Expand;
	mb_ExpandOnce = _po_Src->mb_ExpandOnce;
	mi_Indent = _po_Src->mi_Indent;
	mi_NumGroup = _po_Src->mi_NumGroup;
	L_strcpy(masz_Group, _po_Src->masz_Group);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVAV_cl_ListBox::b_DispGrp(EVAV_cl_ViewItem *_po_Item)
{
	/*~~~~~~~~~~~~~~~*/
	int i;
	int i_Mask, i_Test;
	/*~~~~~~~~~~~~~~~*/

	if(_po_Item->me_Type == EVAV_EVVIT_Separator) return TRUE;
	if(!_po_Item->mi_DisplayGroup && (!(mi_DisplayGroup & EVAV_Filter_NoFil))) return FALSE;
	i_Mask = mi_DisplayGroup;
	i_Test = _po_Item->mi_DisplayGroup;
	for(i = 0; i < 32; i++)
	{
		if((i_Test & 1) && (!(i_Mask & 1))) return FALSE;
		i_Test >>= 1;
		i_Mask >>= 1;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ViewItem *EVAV_cl_ListBox::po_IsVolatileSep(EVAV_cl_ViewItem *_po_Item)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos;
	EVAV_cl_ViewItem	*po_Item, *po_First;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mpo_ListItems->GetHeadPosition();
	po_First = NULL;
	while(pos)
	{
		po_Item = mpo_ListItems->GetNext(pos);
		if((po_Item->me_Type == EVAV_EVVIT_Separator) && (!po_Item->mb_Locked))
		{
			po_First = po_Item;
			break;
		}

		if(po_Item == _po_Item) return NULL;
	}

	if(!po_First) return NULL;
	po_Item = po_SearchSeparatorBefore(_po_Item);
	if(po_Item == po_First) return po_Item;
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ViewItem *EVAV_cl_ListBox::po_SearchSeparatorBefore(EVAV_cl_ViewItem *_po_Item)
{
	/*~~~~~~~~~~~~*/
	POSITION	pos;
	/*~~~~~~~~~~~~*/

	pos = mpo_ListItems->Find(_po_Item);
	do
	{
		if(!pos) return NULL;
		_po_Item = mpo_ListItems->GetPrev(pos);
	} while(_po_Item->me_Type != EVAV_EVVIT_Separator);
	return _po_Item;
}

/*
 =======================================================================================================================
    Aim:    When the list box loose the focus, we must refresh it (cause selected item must be redrawn), and we must
            delete actual edit window (such as button or edit box).
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnKillFocus(CWnd *_po_Wnd)
{
	/*
	 * Check if current edit window is here. Check if it's not here that gains the
	 * focus
	 */
	if(mpo_CurrentWnd && mpo_CurrentWnd != _po_Wnd)
	{
		mpo_CurrentWnd->ShowWindow(SW_HIDE);
		mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
		mpo_CurrentWnd = NULL;
	}

	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::EnableScroll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect				o_Rect;
	int					i;
	RECT				st_Rect;
	int					cy;
	CRect				o_Client;
	EVAV_cl_ViewItem	*po_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetClientRect(&o_Rect);
	cy = o_Rect.bottom;

	/* Retreive the last visible item */
	i = GetCount() - 1;
	st_Rect.top = st_Rect.bottom = 0;
	if(i != -1)
	{
		po_Item = (EVAV_cl_ViewItem *) GetItemData(i);
		while(i && ((po_Item->mi_Display <= 0) || (!b_DispGrp(po_Item))))
		{
			i--;
			po_Item = (EVAV_cl_ViewItem *) GetItemData(i);
		}

		GetItemRect(i, &st_Rect);
	}

	if((st_Rect.bottom > cy) || GetTopIndex())
		ShowScrollBar(SB_VERT, TRUE);
	else
		ShowScrollBar(SB_VERT, FALSE);

	/* Cause Windows draw its own background if list box is empty !!! */
	if(GetCount() == 0) MoveWindow(0, 0, 0, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnSize(UINT nID, int cx, int cy)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CWnd	*po_Wnd, *po_Capt;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	po_Wnd = GetFocus();
	po_Capt = GetCapture();
	CListBox::OnSize(nID, cx, cy);
	if(GetCapture()) ReleaseCapture();
	if(po_Capt) po_Capt->SetCapture();
	if(po_Wnd) po_Wnd->SetFocus();
	if(GetCount() == 0) MoveWindow(0, 0, 0, 0);
}

/*
 =======================================================================================================================
    Aim:    Determin if the current focus window is in list of parent.
 =======================================================================================================================
 */
#define CorrectFocus()	(GetFocus() && (GetFocus() == this || GetFocus()->GetParent() == this))
/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ListBox::EVAV_cl_ListBox(void)
{
	mo_BrushActive.CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
	mo_BrushBack.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	mo_PenGray.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
	mo_PenBlack.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	mb_CanDragDrop = FALSE;
	mi_DisplayGroup = 0xFFFFFFFF;
	L_memset(mab_AutoExpandStruct, FALSE, sizeof(mab_AutoExpandStruct));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ListBox::~EVAV_cl_ListBox(void)
{
	DeleteObject(&mo_BrushActive);
	DeleteObject(&mo_BrushBack);
	DeleteObject(&mo_PenGray);
	DeleteObject(&mo_PenBlack);
}

/*
 =======================================================================================================================
    Aim:    Draw a separator in the view list.
 =======================================================================================================================
 */
void EVAV_cl_ListBox::DrawSeparator(DRAWITEMSTRUCT *lpDrawItemStruct, CDC *pDC)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*p_Data;
	CRect				o_Rect, o_Rect1;
	CRect				o_Rect2;
	LINK_tdst_Pointer	*pst_Pointer;
	CString				o_Name;
	COLORREF			xCol, xCol1;
	UINT				ui_Res;
	int					iOff, iDec, iLeft;
	CSize				o_Size;
	BOOL				b_MulSel;
	char				asz_Tmp[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Data = (EVAV_cl_ViewItem *) lpDrawItemStruct->itemData;
	GetClientRect(o_Rect);
	o_Rect.top = lpDrawItemStruct->rcItem.top;
	o_Rect.bottom = lpDrawItemStruct->rcItem.bottom;
	o_Rect.left--;
	o_Rect.right++;
	o_Rect.top--;
	b_MulSel = FALSE;

	/* Draw first rect (separator) */
	pDC->Draw3dRect(o_Rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW));
	if(p_Data->me_Type == EVAV_EVVIT_Separator)
	{
		o_Rect1 = o_Rect;
		o_Rect1.InflateRect(-1, -1);
		pDC->Draw3dRect(o_Rect1, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
	}
	else if(p_Data->me_Type == EVAV_EVVIT_CustSep)
	{
	}

	/* Draw first rect (pointer) */
	else
	{
		if((p_Data->mi_Indent == 1) || (p_Data->me_Type == EVAV_EVVIT_CustSep))
		{
			o_Rect1 = o_Rect;
			o_Rect1.InflateRect(-1, -1);
			pDC->Draw3dRect(o_Rect1, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
		}
		else
		{
			pDC->Draw3dRect(o_Rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW));
		}
	}

	/* Hide current select box if separator has focus */
	if(lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		if(mpo_CurrentWnd)
		{
			/*~~~~~~~~*/
			CWnd	*po;
			/*~~~~~~~~*/

			po = mpo_CurrentWnd;
			mpo_CurrentWnd->ShowWindow(SW_HIDE);
			po->MoveWindow(0, 0, 0, 0);
			mpo_CurrentWnd = NULL;
		}
	}

	/* Color of text */
	pDC->SetTextColor(0);
	if((lpDrawItemStruct->itemState & ODS_SELECTED) && CorrectFocus())
	{
		xCol1 = GetSysColor(COLOR_ACTIVECAPTION);
		pDC->SetBkColor(xCol1);
		pDC->SetTextColor(GetSysColor(COLOR_3DHILIGHT));
	}
	else if(p_Data->me_Type == EVAV_EVVIT_Separator)
	{
		xCol1 = GetSysColor(COLOR_3DSHADOW);
		pDC->SetBkColor(xCol1);
		pDC->SetTextColor(0x00FFFFFF);
	}
	else if(p_Data->me_Type == EVAV_EVVIT_CustSep)
	{
		pDC->SetTextColor(GetSysColor(COLOR_3DHILIGHT));
	}

	/* Pointer */
	else
	{
		if(!p_Data->mp_Data) pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		if((p_Data->mi_Indent > 1) && (p_Data->me_Type != EVAV_EVVIT_CustSep))
		{
			xCol1 = p_Data->mx_Color;
			pDC->SetBkColor(xCol1);
		}
		else
		{
			/*
			 * Multiselection if separator is the first that is not locked, if they are things
			 * in mo_MulSel, and if it's a pointer at the first level
			 */
			if(mo_MulSel.GetCount() && (p_Data->me_Type != EVAV_EVVIT_CustSep) && po_IsVolatileSep(p_Data))
			{
				xCol1 = 0x00B08000;
				pDC->SetBkColor(xCol1);
				b_MulSel = TRUE;
			}
			else
			{
				xCol1 = GetSysColor(COLOR_MENU);
				pDC->SetBkColor(xCol1);
			}
		}
	}

	/* Compute name for a pointer */
	if(b_MulSel)
	{
		sprintf(asz_Tmp, "Multiple Selection (%d)", mo_MulSel.GetCount() + 1);
		o_Name = asz_Tmp;
	}
	else
	{
		o_Name = p_Data->masz_Name;
		if(p_Data->me_Type == EVAV_EVVIT_Pointer)
		{
			pst_Pointer = LINK_p_SearchPointer(p_Data->mp_Data);
			if(pst_Pointer) o_Name += "   (" + CString(pst_Pointer->asz_Name) + CString(")");
		}
	}

	if((p_Data->me_Type == EVAV_EVVIT_Pointer) && (!p_Data->mp_Data))
		o_Name += ("  (NULL)");

	/* Draw text of separator */
	o_Rect.left++;
	o_Rect.right--;
	o_Rect.top++;
	o_Rect.bottom--;
	o_Rect1 = o_Rect;
	iOff = 0;
	if
	(
		(p_Data->me_Type == EVAV_EVVIT_Separator)
	||	((p_Data->me_Type == EVAV_EVVIT_Pointer) && (p_Data->mi_Indent == 1))
	||	((p_Data->me_Type == EVAV_EVVIT_SubStruct) && (p_Data->mi_Indent == 1))
	||	((p_Data->mul_Flags & EVAV_Array) && (p_Data->mi_Indent == 1))
	)
	{
		o_Rect.left++;
		o_Rect.right--;
		o_Rect.top++;
		o_Rect.bottom--;
		if((p_Data->me_Type == EVAV_EVVIT_Pointer) && (p_Data->mi_Indent == 1)) iOff = -1;
	}

	iDec = DEC + 18;
	if(p_Data->me_Type == EVAV_EVVIT_CustSep)
	{
		iDec = 0;
		iOff = -1;
	}

	/* Draw text */
	iLeft = o_Rect.left + (p_Data->mi_Indent * INDENT_C_STEP) + iDec;
	o_Rect2 = o_Rect;
	if(p_Data->me_Type == EVAV_EVVIT_Separator) o_Rect2.right -= 16;
	if(p_Data->me_Type != EVAV_EVVIT_CustSep)
	{
		if((p_Data->me_Type != EVAV_EVVIT_Pointer) || (p_Data->mp_Data))
		{
			o_Rect2.right -= 20;
		}
	}
	else
	{
		iOff++;
	}

	pDC->ExtTextOut
		(
			iLeft,
			o_Rect.top - 1 + iOff,
			ETO_CLIPPED | ETO_OPAQUE,
			&o_Rect2,
			(char *) (LPCSTR) o_Name,
			o_Name.GetLength(),
			0
		);

	/* To the right of text */
	o_Rect2.left = o_Rect2.right;
	o_Rect2.right = o_Rect.right;
	pDC->FillSolidRect(&o_Rect2, pDC->GetBkColor());

	/* Draw rect for expansion */
	o_Rect1.left = (p_Data->mi_Indent * INDENT_C_STEP) + DEC;
	o_Rect1.right = o_Rect1.left + 10;
	o_Rect1.top = o_Rect.top + iOff + 2;
	if(p_Data->me_Type == EVAV_EVVIT_Separator) o_Rect1.top--;
	o_Rect1.bottom = o_Rect1.top + 9;
	if(p_Data->me_Type == EVAV_EVVIT_Separator)
		xCol = GetSysColor(COLOR_WINDOW);
	else
		xCol = GetSysColor(COLOR_3DDKSHADOW);
	if(p_Data->me_Type != EVAV_EVVIT_CustSep)
	{
		if((lpDrawItemStruct->itemState & ODS_SELECTED) && CorrectFocus()) xCol = GetSysColor(COLOR_WINDOW);
		pDC->Draw3dRect(o_Rect1, xCol, xCol);
		pDC->Draw3dRect(o_Rect1.left + 2, o_Rect1.top + 4, 6, 1, xCol, xCol);
		if(!p_Data->mb_Expand) pDC->Draw3dRect(o_Rect1.left + 5, o_Rect1.top + 2, 1, 6, xCol, xCol);
	}

	/* Draw locked/unlocked button */
	iOff = 0;
	if(p_Data->me_Type == EVAV_EVVIT_Separator)
	{
		if(p_Data->mb_Locked)
			ui_Res = BMP_LOCKED_MENU;
		else
			ui_Res = BMP_UNLOCKED_MENU;
		M_MF()->DrawTransparentBmp(ui_Res, pDC, o_Rect.right - 20, o_Rect.top - 2, 16, 15, 16, 15, RGB(192, 192, 192));
		iOff += 20;
	}

	pDC->DeleteTempMap();
}

/*
 =======================================================================================================================
    Aim:    To draw an item in the list box.
 =======================================================================================================================
 */
void EVAV_cl_ListBox::DrawItem(DRAWITEMSTRUCT *lpDrawItemStruct)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC					dc, *pDC;
	EVAV_cl_ViewItem	*p_Data;
	CRect				o_Rect, o_Rect1, o_Rect2, o_Rect3, o_Rect4, o_Rect5;
	CString				o_Value;
	CWnd				*po_Wnd;
	CPen				*po_OldPen;
	CBrush				*po_OldB;
	BOOL				b_Over;
	BOOL				b_Focus;
	COLORREF			x_Col;
	int					i;
	BOOL				bChange;
	static int			rec = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(rec) return;
	rec = 1;
	p_Data = (EVAV_cl_ViewItem *) lpDrawItemStruct->itemData;
	if((p_Data == NULL) || ((int) p_Data == -1))
	{
		rec = 0;
		return;
	}

	pDC = dc.FromHandle(lpDrawItemStruct->hDC);
	pDC->SetBkMode(TRANSPARENT);

	/* Separator */
	if
	(
		(p_Data->me_Type == EVAV_EVVIT_Separator)
	||	(p_Data->me_Type == EVAV_EVVIT_Pointer)
	||	(p_Data->me_Type == EVAV_EVVIT_SubStruct)
	||	(p_Data->me_Type == EVAV_EVVIT_CustSep)
	||	(p_Data->mul_Flags & EVAV_Array)
	)
	{
		DrawSeparator(lpDrawItemStruct, pDC);
		rec = 0;
		return;
	}

	/* The item must not be displayed. Hide select wnd if it has the focus */
	if((p_Data->mi_Display <= 0) || (!(b_DispGrp(p_Data))))
	{
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			if(mpo_CurrentWnd)
			{
				mpo_CurrentWnd->ShowWindow(SW_HIDE);
				mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
				mpo_CurrentWnd = NULL;
			}
		}

		rec = 0;
		return;
	}

	/* Is it correct focus */
	b_Focus = CorrectFocus();

	/* Rectangle for name */
	o_Rect1 = lpDrawItemStruct->rcItem;
	o_Rect1.right = mi_WidthCol1;
	o_Rect1.top--;
	o_Rect1.left--;

	/* Draw the focus rect */
	if((lpDrawItemStruct->itemState & ODS_SELECTED) && b_Focus)
	{
		o_Rect3 = o_Rect1;
		o_Rect3.InflateRect(0, 1);
		o_Rect3.top += 2;
		o_Rect3.bottom -= 2;
		pDC->Draw3dRect(o_Rect3, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW));
		o_Rect3 = o_Rect1;
		o_Rect3.InflateRect(0, -1);
		pDC->Draw3dRect(o_Rect3, RGB(0, 0, 0), RGB(0, 0, 0));
	}

	/* Draw first rect */
	else
	{
		pDC->Draw3dRect(o_Rect1, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW));
	}

	/* Select back color */
	if((p_Data->mul_Flags & EVAV_ReadOnly) || (!(lpDrawItemStruct->itemState & ODS_SELECTED)))
	{
		x_Col = p_Data->mx_Color;
	}
	else
	{
		if(b_Focus)
		{
			x_Col = GetSysColor(COLOR_ACTIVECAPTION);
		}
		else
		{
			if(mpo_CurrentWnd)
			{
				mpo_CurrentWnd->ShowWindow(SW_HIDE);
				mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
			}

			mpo_CurrentWnd = po_Wnd = NULL;
			x_Col = p_Data->mx_Color;
		}
	}

	pDC->SetBkColor(M_MF()->u4_Interpol2PackedColor(0, x_Col, 0.9f) & 0x00FFFFFF);

	/* Select text color */
	if(p_Data->mul_Flags & EVAV_ReadOnly)
	{
		pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
	}
	else if((lpDrawItemStruct->itemState & ODS_SELECTED) && b_Focus)
	{
		pDC->SetTextColor(GetSysColor(COLOR_WINDOW));
	}
	else
	{
		pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
	}

	/* Name */
	o_Rect4 = o_Rect1;
	o_Rect4.left++;
	o_Rect4.right--;
	o_Rect4.top++;
	o_Rect4.bottom--;
	pDC->ExtTextOut
		(
			o_Rect4.left + (p_Data->mi_Indent * INDENT_C_STEP) + DEC,
			o_Rect4.top - 1,
			ETO_CLIPPED | ETO_OPAQUE,
			&o_Rect4,
			p_Data->masz_Name,
			L_strlen(p_Data->masz_Name),
			0
		);
	o_Value = sgapfnv_DrawArray[p_Data->me_Type](p_Data, p_Data->mp_Data);
	bChange = FALSE;
	if(o_Value != p_Data->o_LastValue && !p_Data->o_LastValue.IsEmpty()) bChange = TRUE;
	p_Data->o_LastValue = o_Value;

	/* Rect for the second column */
	pDC->SetBkColor(x_Col);
	GetClientRect(o_Rect2);
	o_Rect2.top = o_Rect1.top;
	o_Rect2.bottom = o_Rect1.bottom;
	o_Rect2.left = o_Rect1.right - 1;
	o_Rect2.right++;

	/*
	 * Move a special change cwnd at the good position for the selected item, else
	 * just write it.
	 */
	b_Over = FALSE;
	if((p_Data->mul_Flags & EVAV_ReadOnly) && (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		if(mpo_CurrentWnd)
		{
			mpo_CurrentWnd->ShowWindow(SW_HIDE);
			mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
		}

		mpo_CurrentWnd = po_Wnd = NULL;
	}
	else if((lpDrawItemStruct->itemState & ODS_SELECTED) && b_Focus)
	{
		switch(sgae_EditTypesArray[p_Data->me_Type])
		{
		case EVAV_EVVIET_Combo:
			po_Wnd = &mo_ComboSelect;

			/* Move ctrl only if current position is invalid */
			po_Wnd->GetWindowRect(&o_Rect);
			ScreenToClient(&o_Rect);
			o_Rect4 = o_Rect2;
			o_Rect4.top--;
			o_Rect4.right--;
			o_Rect.bottom = o_Rect2.bottom;
			if(o_Rect.top != o_Rect4.top)
			{
				OnComboSelect();
				po_Wnd->MoveWindow(o_Rect4);
				po_Wnd->Invalidate();
				((CComboBox *) po_Wnd)->SetItemHeight(0, 15);
				((CComboBox *) po_Wnd)->SetItemHeight(-1, 14);
			}

			b_Over = TRUE;
			break;
		case EVAV_EVVIET_Edit:
			po_Wnd = &mo_EditSelect;
			switch(p_Data->me_Type)
			{
			case EVAV_EVVIT_String: mo_EditSelect.i_Type = 0; break;
			case EVAV_EVVIT_Int:	mo_EditSelect.i_Type = 1; break;
			case EVAV_EVVIT_Float:	mo_EditSelect.i_Type = 2; break;
			case EVAV_EVVIT_Hexa:	mo_EditSelect.i_Type = 3; break;
			}

			/* Move ctrl only if current position is invalid */
			po_Wnd->GetWindowRect(&o_Rect);
			ScreenToClient(&o_Rect);
			if(o_Rect != o_Rect2)
			{
				OnEditSelect();
				po_Wnd->MoveWindow(o_Rect2);
				po_Wnd->Invalidate();
			}

			b_Over = TRUE;
			break;
		case EVAV_EVVIET_Button:
			po_Wnd = &mo_ButtonSelect;

			/* Move ctrl only if current position is invalid */
			po_Wnd->GetWindowRect(&o_Rect);
			ScreenToClient(&o_Rect);
			o_Rect4 = o_Rect2;
			o_Rect4.left = o_Rect4.right - 20;
			o_Rect4.right = o_Rect4.left + 20;
			if(o_Rect != o_Rect4)
			{
				po_Wnd->MoveWindow(o_Rect4);
				po_Wnd->Invalidate();
			}

			b_Over = FALSE;
			break;
		case EVAV_EVVIET_Check:
			po_Wnd = &mo_CheckSelect;

			/* Move ctrl only if current position is invalid */
			po_Wnd->GetWindowRect(&o_Rect);
			ScreenToClient(&o_Rect);
			o_Rect4 = o_Rect2;
			o_Rect4.left = o_Rect4.right - 20;
			o_Rect4.top += 1;
			o_Rect4.right = o_Rect4.left + 12;
			o_Rect4.bottom = o_Rect4.top + 12;
			if(o_Rect != o_Rect4)
			{
				OnCheckSelect();
				po_Wnd->MoveWindow(o_Rect4);
				po_Wnd->Invalidate();
			}

			b_Over = FALSE;
			break;
		default:
			ERR_X_ForceAssert();
		}

		if(mpo_CurrentWnd != po_Wnd)
		{
			if(mpo_CurrentWnd)
			{
				mpo_CurrentWnd->ShowWindow(SW_HIDE);
				mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
			}

			if(po_Wnd) po_Wnd->ShowWindow(SW_SHOW);
			mpo_CurrentWnd = po_Wnd;
		}
	}

	/* Display value */
	if(!b_Over)
	{
		/* Draw second rectangle */
		pDC->Draw3dRect(o_Rect2, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW));

		/* Draw focus rect */
		if((lpDrawItemStruct->itemState & ODS_SELECTED) && b_Focus)
		{
			o_Rect3 = o_Rect2;
			o_Rect3.InflateRect(0, -1);
			pDC->Draw3dRect(o_Rect2, RGB(0, 0, 0), RGB(0, 0, 0));
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Special drawing: Colors
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(p_Data->me_Type == EVAV_EVVIT_Color)
		{
			/*~~~~~~~~~~~~~~~~~~~~*/
			CBrush		o_Fill;
			COLORREF	col, colmem;
			CPen		o_PenWhite;
			char		asz_R[100];
			UINT		almem;
			/*~~~~~~~~~~~~~~~~~~~~*/

			col = L_atol(o_Value);
			o_Fill.CreateSolidBrush(col & 0x00FFFFFF);
			po_OldB = pDC->SelectObject(&o_Fill);
			o_PenWhite.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
			po_OldPen = pDC->SelectObject(&o_PenWhite);
			o_Rect2.top++;
			o_Rect2.bottom--;
			o_Rect2.left++;
			o_Rect2.right--;
			if((lpDrawItemStruct->itemState & ODS_SELECTED) && b_Focus) o_Rect2.right -= 19;
			pDC->Rectangle(o_Rect2);
			o_Rect5 = o_Rect2;
			o_Rect5.DeflateRect(1, 1);
			o_Rect5.left += (o_Rect2.Width() * 3) / 4;
			colmem = (col & 0xFF000000) >> 8;
			colmem |= (colmem >> 8) | (colmem >> 16);
			pDC->FillSolidRect(&o_Rect5, colmem);
			pDC->SelectObject(po_OldB);
			pDC->SelectObject(po_OldPen);
			DeleteObject(&o_Fill);
			DeleteObject(&o_PenWhite);
			colmem = pDC->GetTextColor();
			pDC->SetTextColor((col & 0x00FFFFFF) ^ 0x00FFFFFF);
			almem = pDC->GetTextAlign();
			pDC->SetTextAlign(TA_CENTER);
			for(i = 0; i < 4; i++)
			{
				if(M_MF()->mst_Ini.b_IntAsHex)
					sprintf(asz_R, "0x%x", col & 0x000000FF);
				else
					sprintf(asz_R, "%5d", col & 0x000000FF);
				if(i == 3) pDC->SetTextColor(col < 128 ? 0xFFFFFF : 0);
				pDC->ExtTextOut
					(
						o_Rect2.left + (i * (o_Rect2.Width() / 4)) + o_Rect2.Width() / 8,
						o_Rect2.top - 1,
						ETO_CLIPPED,
						&o_Rect2,
						asz_R,
						L_strlen(asz_R),
						0
					);
				col >>= 8;
			}

			pDC->SetTextColor(colmem);
			pDC->SetTextAlign(almem);
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Draw normal text
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else
		{
			pDC->SetBkMode(OPAQUE);
			pDC->SetBkColor(p_Data->mx_Color);
			if(p_Data->mul_Flags & EVAV_ReadOnly)
				pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
			else if(bChange)
				pDC->SetTextColor(0x000000C0);
			else
				pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
			o_Rect2.left++;
			o_Rect2.right--;
			o_Rect2.top++;
			o_Rect2.bottom--;
			pDC->SelectObject(&mo_PenBlack);
			pDC->ExtTextOut
				(
					o_Rect2.left + DEC,
					o_Rect2.top - 1,
					ETO_CLIPPED | ETO_OPAQUE,
					&o_Rect2,
					o_Value,
					o_Value.GetLength(),
					0
				);
		}
	}

	/* End */
	pDC->DeleteTempMap();
	rec = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::MeasureItem(MEASUREITEMSTRUCT *lp)
{
	lp->itemHeight = 14;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVAV_cl_ListBox::OnEraseBkgnd(CDC *pDC)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DWORD				x_Col;
	CRect				o_Rect;
	RECT				st_Rect;
	int					i;
	EVAV_cl_ViewItem	*po_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	x_Col = GetSysColor(COLOR_BTNFACE);

	/* Retreive the last visible item */
	i = GetCount() - 1;
	st_Rect.top = st_Rect.bottom = 0;
	if(i != -1)
	{
		po_Item = (EVAV_cl_ViewItem *) GetItemData(i);
		while(i && ((po_Item->mi_Display <= 0) || (!(b_DispGrp(po_Item)))))
		{
			i--;
			po_Item = (EVAV_cl_ViewItem *) GetItemData(i);
		}

		GetItemRect(i, &st_Rect);
	}

	/* Erase the rest of the view */
	GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	o_Rect.top = st_Rect.bottom;
	pDC->FillSolidRect(o_Rect, x_Col);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::DeleteString(int i)
{
	if(i == GetCurSel())
	{
		if(i != GetCount() - 1)
			SetCurSel(i + 1);
		else if(i)
			SetCurSel(i - 1);
		else
			SetCurSel(0);
		CListBox::DeleteString(i);
		if(mpo_CurrentWnd)
		{
			mpo_CurrentWnd->ShowWindow(SW_HIDE);
			if(mpo_CurrentWnd) mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
			mpo_CurrentWnd = NULL;
		}

		return;
	}

	CListBox::DeleteString(i);
}

/*
 =======================================================================================================================
    Aim:    Call when user click on expand button +/-
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnExpand(UINT _n_ID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos, pos1, pos2, ppos2 = NULL;
	EVAV_cl_ViewItem	*po_Item, *po_NewItem, *po_Group, *po_First;
	EVAV_cl_ViewItem	*po_NewNew;
	int					i_Indent, i_Num, i_Count;
	void				*p_P;
	int					i_Sel, i_ScrollV, i_ScrollH;
	CRect				o_Rect;
	BOOL				bCanAutoExpand;
	CString				o_Str;
	int					i_DisplayGroup;
	OBJ_tdst_GameObject *pst_GO;
	char				*psz_FieldName;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->LockDisplay(this);
	_n_ID -= WM_USER;
	i_Sel = GetCurSel();
	i_ScrollV = GetScrollPos(SB_VERT);
	i_ScrollH = GetScrollPos(SB_HORZ);

	/* Search the item with the right id */
	po_First = NULL;
	i_Num = 0;
	pos = mpo_ListItems->GetHeadPosition();
	while(pos)
	{
		po_Item = mpo_ListItems->GetAt(pos);
		if(!po_First && (po_Item->me_Type == EVAV_EVVIT_Separator))
		{
			if(!po_Item->mb_Locked) po_First = po_Item;
		}

		if(po_Item->mi_ID == (int) _n_ID) break;
		if((po_Item->mi_Display > 0) && (b_DispGrp(po_Item))) i_Num++;
		mpo_ListItems->GetNext(pos);
	}

	/* Test if it is the first non locked separator */
	bCanAutoExpand = FALSE;
	po_Group = po_SearchSeparatorBefore(po_Item);
	if(po_Group && !po_Group->mb_Locked && po_Group == po_First)
		bCanAutoExpand = TRUE;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Display/Hide a group of items.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(po_Item->me_Type == EVAV_EVVIT_Separator)
	{
		/* Get group of separator */
		_n_ID = po_Item->mi_NumGroup;
		po_Item->mb_Expand = po_Item->mb_Expand ? FALSE : TRUE;
		if(po_Item->mb_Expand) po_Item->mb_ExpandOnce = TRUE;
		po_Group = po_Item;

		/* Hide all items of the group just above the separator */
		i_Num++;
		mpo_ListItems->GetNext(pos);
		while(pos)
		{
			po_Item = mpo_ListItems->GetNext(pos);
			if(po_Item->mi_NumGroup == (int) _n_ID + 1)
			{
				/* Display count */
				if(po_Group->mb_Expand)
					po_Item->mi_Display++;
				else
					po_Item->mi_Display--;
				if((po_Item->mi_Display == 1) && (b_DispGrp(po_Item)))
				{
					InsertString(i_Num, po_Item->masz_Name);
					SetItemData(i_Num++, (LONG) po_Item);
				}
				else if((po_Item->mi_Display == 0) && (!po_Group->mb_Expand) && (b_DispGrp(po_Item)))
				{
					DeleteString(i_Num);
					continue;
				}
			}

			/* To optimize : No need to treat groups after current one */
			else if(po_Item->mi_NumGroup > (int) _n_ID + 1)
				break;
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Expand/Hide a pointer structure.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else if
		(
			(po_Item->me_Type == EVAV_EVVIT_Pointer)
		||	(po_Item->me_Type == EVAV_EVVIT_SubStruct)
		||	(po_Item->mul_Flags & EVAV_Array)
		)
	{
		if((po_Item->me_Type == EVAV_EVVIT_Pointer) && !(po_Item->mp_Data)) goto zap;
		po_Item->mb_Expand = po_Item->mb_Expand ? FALSE : TRUE;

		/* Save autoexpand flag for struct */
		if(bCanAutoExpand)
		{
			if((po_Item->me_Type == EVAV_EVVIT_Pointer) || (po_Item->me_Type == EVAV_EVVIT_SubStruct))
			{
				if(po_Item->mb_Expand)
				{
					mab_AutoExpandStruct[po_Item->mi_Param1] = TRUE;
					pos2 = mas_AutoExpandStruct[po_Item->mi_Param1].GetHeadPosition();
					while(pos2)
					{
						ppos2 = pos2;
						o_Str = mas_AutoExpandStruct[po_Item->mi_Param1].GetNext(pos2);
						if(o_Str == po_Item->masz_Name)
						{
							ppos2 = NULL;
							break;
						}
					}

					if(ppos2) mas_AutoExpandStruct[po_Item->mi_Param1].AddTail(po_Item->masz_Name);
				}
				else
				{
					pos2 = mas_AutoExpandStruct[po_Item->mi_Param1].GetHeadPosition();
					while(pos2)
					{
						ppos2 = pos2;
						o_Str = mas_AutoExpandStruct[po_Item->mi_Param1].GetNext(pos2);
						if(o_Str == po_Item->masz_Name)
						{
							mas_AutoExpandStruct[po_Item->mi_Param1].RemoveAt(ppos2);
							if(mas_AutoExpandStruct[po_Item->mi_Param1].GetCount() == 0)
								mab_AutoExpandStruct[po_Item->mi_Param1] = FALSE;
							break;
						}
					}
				}
			}
		}

		if(po_Item->mb_Expand)
		{
			/* We must add fields in the list that correspond to the pointer. */
			if(!po_Item->mb_ExpandOnce)
			{

				/*$1
				 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				    Special pointer process
				 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				 */

				if(po_Item->mul_Flags & EVAV_Array)
					TreatArray(po_Item, pos);
				else if((po_Item->mi_Param1 == LINK_C_AI_Instance) && (po_Item->mp_Data))
					AIAddInstance(pos, (AI_tdst_Instance *) po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_COL_Instance) && (po_Item->mp_Data))
					COLAddInstance(pos, (struct COL_tdst_Instance_ *) po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_MSG_LiFo) && (po_Item->mp_Data))
					MsgAddList(pos, (AI_tdst_MessageLiFo *) po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_ENG_BoundingVolume) && (po_Item->mp_Data))
					OBJAddBoundingVolume(pos, po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_ENG_Zone) && (po_Item->mp_Data))
					OBJAddZone(pos, po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_ENG_Cob) && (po_Item->mp_Data))
					OBJAddCob(pos, po_Item->mp_Data);
#ifdef ODE_INSIDE
				else if((po_Item->mi_Param1 == LINK_C_ENG_ODE) && (po_Item->mp_Data))
					OBJAddODE(pos, po_Item->mp_Data);
#endif
				else if((po_Item->mi_Param1 == LINK_C_GDK_StructId) && (po_Item->mp_Data))
					OBJAddGraphicObject(pos, po_Item->mp_Data);
				else if(po_Item->mi_Param1 == LINK_C_MDF_Modifier)
					OBJAddModifier(pos, po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_ENG_GO_BaseAdditionalMatrix) && (po_Item->mp_Data))
					OBJAddBaseAdditionalMatrix(pos, po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_ENG_GameObjectAnim) && (po_Item->mp_Data))
					ANIAddGameObjectAnim(pos, po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_ENG_Anim) && (po_Item->mp_Data))
					ANIAddAnim(pos, po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_ENG_BlendAnim) && (po_Item->mp_Data))
					ANIAddBlendAnim(pos, po_Item->mp_Data);

				/*
				 * else if((po_Item->mi_Param1 == LINK_C_ENG_ActionKit) && (po_Item->mp_Data)) £
				 * ACTAddActionKit(pos, po_Item->mp_Data);
				 */
				else if((po_Item->mi_Param1 == LINK_C_ENG_Action) && (po_Item->mp_Data))
					ACTAddAction(pos, po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_TEX_Procedural) && (po_Item->mp_Data))
					TEXAddProcedural(pos, po_Item->mp_Data);
                else if((po_Item->mi_Param1 == LINK_C_TEX_Animated) && (po_Item->mp_Data))
					TEXAddAnimated(pos, po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_ENG_SpriteGen) && (po_Item->mp_Data))
					TEXAddSpriteGen(pos, po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_SND_Bank) && (po_Item->mp_Data))
					SNDAddBank(pos, (union SND_tdun_Main_ *) po_Item->mp_Data);
				else if((po_Item->mi_Param1 == LINK_C_SND_Sound) && (po_Item->mp_Data))
					SNDAddSound(pos, (struct SND_tdst_OneSound_ *) po_Item->mp_Data);
#ifdef JADEFUSION
                else if((po_Item->mi_Param1 == LINK_C_MDF_SoftBodyListRods) && (po_Item->mp_Data))
                    OBJAddSoftBodyListRods(pos, po_Item->mp_Data);
                else if((po_Item->mi_Param1 == LINK_C_MDF_SoftBodyListVertices) && (po_Item->mp_Data))
                    OBJAddSoftBodyListVertices(pos, po_Item->mp_Data);
                else if((po_Item->mi_Param1 == LINK_C_MDF_SoftBodyListPlanes) && (po_Item->mp_Data))
                    OBJAddSoftBodyListPlanes(pos, po_Item->mp_Data);
#endif

				/*$1
				 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				    Normal process
				 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				 */

				else
				{
					po_Item->mi_NumFields = LINK_gast_StructTypes[po_Item->mi_Param1].i_NumFields;
					pos1 = LINK_gast_StructTypes[po_Item->mi_Param1].po_Fields->GetHeadPosition();
					i_Count = 1;
					while(pos1)
					{
						po_NewItem = LINK_gast_StructTypes[po_Item->mi_Param1].po_Fields->GetNext(pos1);
						psz_FieldName = po_NewItem->masz_Name;

						/* Is there a different name kit ? */
						if(LINK_gast_StructTypes[po_Item->mi_Param1].i_NameKit)
						{
							if
							(
								(LINK_gast_StructTypes[po_Item->mi_Param1].i_NameKit <= LINK_gast_StructTypes[po_Item->mi_Param1].i_MaxNameKit) &&
								(LINK_gast_StructTypes[po_Item->mi_Param1].i_NameKit > 0)
							)
							{
								if
								(
									LINK_gast_StructTypes[po_Item->mi_Param1].apsz_NameKits[LINK_gast_StructTypes[
										po_Item->mi_Param1].i_NameKit - 1][i_Count]
								)
								{
									psz_FieldName = LINK_gast_StructTypes[po_Item->mi_Param1].apsz_NameKits[LINK_gast_StructTypes[po_Item->mi_Param1].i_NameKit - 1][i_Count];
								}
							}
							else
								LINK_gast_StructTypes[po_Item->mi_Param1].i_NameKit = 0;
						}
						else if(po_Item->psz_NameKit)
						{
							char	az[512];
							char	*pz, *pz1;
							int		find;
							CString	ostr, str1, str2;

							// Format vrainom nomderemplacement vrainom nomderemplacement
							// Si le vrainom n'est pas trouvé, le champ n'est pas affiché
							ostr = po_Item->psz_NameKit;							

							str1 = ostr;
							str1.MakeUpper();
							str2 = po_NewItem->masz_Name;
							str2.MakeUpper();
							find = str1.Find(str2);
							if(find != -1)
							{
								find += L_strlen(po_NewItem->masz_Name);
								pz1 = (char *) (LPCSTR) ostr;
								pz1 += find;
								while(L_isspace(*pz1)) pz1++;
								pz = az;
								while(L_isalnum(*pz1)) *pz++ = *pz1++;
								*pz = 0;
								psz_FieldName = az;
							}
							else
							{
								continue;
							}
						}

						i_Count++;

						/* Special case for gameobject and visu/anim */
						if(po_Item->mi_Param1 == LINK_C_ENG_GameObjectOriented)
						{
							pst_GO = (OBJ_tdst_GameObject *) po_Item->mp_Data;
							if
							(
								pst_GO
							&&	(
									(po_NewItem->mi_Param1 == LINK_C_ENG_GO_Base)
								||	(po_NewItem->mi_Param1 == LINK_C_ENG_GO_BaseAnim)
								)
							)
							{
								if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
									po_NewItem->mi_Param1 = LINK_C_ENG_GO_BaseAnim;
								else
									po_NewItem->mi_Param1 = LINK_C_ENG_GO_Base;
							}
						}

						if(pos == NULL)
							pos = mpo_ListItems->GetTailPosition();

						/*
						 * For a pointer, get the pointer in the register field, and for a substruct, get
						 * the register field as the adress
						 */
						if((po_NewItem->me_Type == EVAV_EVVIT_Pointer) || (po_NewItem->me_Type == EVAV_EVVIT_String))
						{
							p_P = NULL;
							if(po_Item->mp_Data)
							{
								p_P = *((void **) ((char *) po_Item->mp_Data + (int) po_NewItem->mp_Data));
							}
						}
						else
							p_P = (void *) ((char *) po_Item->mp_Data + (int) po_NewItem->mp_Data);
						i_DisplayGroup = po_NewItem->mi_DisplayGroup;
						if(!i_DisplayGroup) i_DisplayGroup = po_Item->mi_DisplayGroup;
						pos = mpo_ListItems->InsertAfter
							(
								pos,
								po_NewNew = new EVAV_cl_ViewItem
									(
										psz_FieldName,
										po_NewItem->me_Type,
										p_P,
										po_NewItem->mul_Flags,
										po_NewItem->mi_Param1,
										po_NewItem->mi_Param2,
										po_NewItem->mi_Param3,
										LINK_gast_StructTypes[po_Item->mi_Param1].pfn_CB,
										po_NewItem->mx_Color,
										po_NewItem->mi_Param4,
										po_NewItem->mi_Param5,
										po_NewItem->mi_Param6,
										po_NewItem->mi_Param7,
										i_DisplayGroup
									)
							);
						po_NewNew->psz_Help = po_NewItem->psz_Help;
						po_NewNew->mul_Offset = (ULONG) po_NewItem->mp_Data;
					}
				}

				SetItemList(mpo_ListItems);
				Invalidate();
				po_Item->mb_ExpandOnce = TRUE;
			}
			else
			{
				/*
				 * The pointer has been already expanded once, so the fields are already in the
				 * list. We must just add those fields in the list box.
				 */
				if(pos)
				{
					i_Indent = po_Item->mi_Indent;
					i_Num++;
					mpo_ListItems->GetNext(pos);
					if(pos)
					{
						po_Item = mpo_ListItems->GetNext(pos);
						while(po_Item->mi_Indent > i_Indent)
						{
							po_Item->mi_Display++;
							if((po_Item->mi_Display > 0) && (b_DispGrp(po_Item)))
							{
								InsertString(i_Num, po_Item->masz_Name);
								SetItemData(i_Num++, (LONG) po_Item);
							}

							if(!pos) break;
							po_Item = mpo_ListItems->GetNext(pos);
						}
					}
				}
			}
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Hide contents of pointer structure.
		 ---------------------------------------------------------------------------------------------------------------
		 */

		else
		{
			i_Indent = po_Item->mi_Indent;
			i_Num++;
			mpo_ListItems->GetNext(pos);
			if(pos)
			{
				po_Item = mpo_ListItems->GetNext(pos);
				while(po_Item->mi_Indent > i_Indent)
				{
					po_Item->mi_Display--;
					if((po_Item->mi_Display == 0) && (b_DispGrp(po_Item))) DeleteString(i_Num);
					if(!pos) break;
					po_Item = mpo_ListItems->GetNext(pos);
				}
			}
		}
	}

zap:
	SetCurSel(i_Sel);
	if(i_ScrollV)
	{
		SetScrollPos(SB_VERT, i_ScrollV);
		SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (i_ScrollV << 16), 0);
	}

	if(i_ScrollH)
	{
		SetScrollPos(SB_HORZ, i_ScrollH);
		SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (i_ScrollH << 16), 0);
	}

	EnableScroll();
	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ViewItem *EVAV_cl_ListBox::po_GetCurrentItem(void)
{
	/*~~~~~~*/
	int i_Sel;
	/*~~~~~~*/

	i_Sel = GetCurSel();
	if(i_Sel == LB_ERR) return NULL;
	if(!CorrectFocus()) return NULL;
	return (EVAV_cl_ViewItem *) GetItemData(i_Sel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ViewItem *EVAV_cl_ListBox::GetItemParent(EVAV_cl_ViewItem *_po_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos;
	EVAV_cl_ViewItem	*po_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_po_Data->mi_Indent == 0) return NULL;

	pos = mpo_ListItems->Find(_po_Data);
	if(!pos) return NULL;
	po_Item = mpo_ListItems->GetAt(pos);

	while(1)
	{
		po_Item = mpo_ListItems->GetPrev(pos);
		if(!pos) return NULL;
		if(_po_Data->mi_Indent == po_Item->mi_Indent + 1) return po_Item;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnComboSelect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*psel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	psel = po_GetCurrentItem();
	if(psel) sgapfnv_SelectArray[psel->me_Type]((CWnd *) &mo_ComboSelect, psel, psel->mp_Data, TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnEditSelect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*psel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	psel = po_GetCurrentItem();
	if(psel) sgapfnv_SelectArray[psel->me_Type]((CWnd *) &mo_EditSelect, psel, psel->mp_Data, TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnComboChangeSelect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*psel;
	long				l_Old;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	gpo_CurVavListBox = this;

	psel = po_GetCurrentItem();
	if(psel)
	{
		l_Old = *(long *) psel->mp_Data;
		LINK_gb_CanRefresh = FALSE;
		sgapfnv_SelectArray[psel->me_Type]((CWnd *) &mo_ComboSelect, psel, psel->mp_Data, FALSE);

		/* Inform user callback */
		if(mpfnv_Callback) mpfnv_Callback(mp_Owner, psel, psel->mp_Data, 0);
		if(psel->mpfn_CB) psel->mpfn_CB(mp_Owner, psel, psel->mp_Data, l_Old);
		psel = po_GetCurrentItem();
		if(psel) MulSel(psel);
		LINK_gb_CanRefresh = TRUE;
		LINK_Refresh();
	}

	/* Be sure that edit window is hidden if we clicked outside var view */
	if(!CorrectFocus() && mpo_CurrentWnd)
	{
		mpo_CurrentWnd->ShowWindow(SW_HIDE);
		mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
		mpo_CurrentWnd = NULL;
	}

	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnEditChangeSelect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*psel;
	long				l_Old;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	gpo_CurVavListBox = this;

	psel = po_GetCurrentItem();
	if(psel)
	{
		if(!psel->mp_Data) return;
		l_Old = *(long *) psel->mp_Data;
		LINK_gb_CanRefresh = FALSE;
		sgapfnv_SelectArray[psel->me_Type]((CWnd *) &mo_EditSelect, psel, psel->mp_Data, FALSE);
		sgapfnv_SelectArray[psel->me_Type]((CWnd *) &mo_EditSelect, psel, psel->mp_Data, TRUE);

		/* Inform user callback */
		if(mpfnv_Callback) mpfnv_Callback(mp_Owner, psel, psel->mp_Data, 0);
		if(psel->mpfn_CB) psel->mpfn_CB(mp_Owner, psel, psel->mp_Data, l_Old);
		psel = po_GetCurrentItem();
		if(psel) MulSel(psel);
		LINK_gb_CanRefresh = TRUE;
		LINK_Refresh();
	}

	/* Be sure that edit window is hidden if we clicked outside var view */
	if(!CorrectFocus() && mpo_CurrentWnd)
	{
		mpo_CurrentWnd->ShowWindow(SW_HIDE);
		mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
		mpo_CurrentWnd = NULL;
	}

	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnButtonChangeSelect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*psel;
	int					iSel;
	ULONG				ul_OldValue;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gpo_CurVavListBox = this;

	mo_ButtonSelect.ModifyStyle(BS_DEFPUSHBUTTON, BS_PUSHBUTTON);
	SetFocus();
	iSel = GetCurSel();
	psel = po_GetCurrentItem();
	if(psel)
	{
		LINK_gb_CanRefresh = FALSE;

		ul_OldValue = *(ULONG *) psel->mp_Data;
		sgapfnv_SelectArray[psel->me_Type]((CWnd *) &mo_ButtonSelect, psel, psel->mp_Data, FALSE);
		psel = po_GetCurrentItem();

		/* Inform user callback */
		if(mpfnv_Callback) mpfnv_Callback(mp_Owner, psel, psel ? psel->mp_Data : NULL, ul_OldValue);
		if(psel && psel->mpfn_CB) psel->mpfn_CB(mp_Owner, psel, psel ? psel->mp_Data : NULL, ul_OldValue);
		psel = po_GetCurrentItem();
		if(psel) MulSel(psel);
		LINK_gb_CanRefresh = TRUE;
		LINK_Refresh();
	}

	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnCheckSelect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*psel;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	psel = po_GetCurrentItem();
	if(psel) sgapfnv_SelectArray[psel->me_Type]((CWnd *) &mo_CheckSelect, psel, psel->mp_Data, TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnCheckChangeSelect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*psel;
	long				l_oldData;
	int					iSel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gpo_CurVavListBox = this;
	iSel = GetCurSel();
	psel = po_GetCurrentItem();
	if(psel)
	{
		LINK_gb_CanRefresh = FALSE;

		/* We save the old data */
		l_oldData = *(long *) (psel->mp_Data);
		sgapfnv_SelectArray[psel->me_Type]((CWnd *) &mo_CheckSelect, psel, psel->mp_Data, FALSE);
		psel = (EVAV_cl_ViewItem *) GetItemData(iSel);

		/* Inform user callback */
		if(mpfnv_Callback)
		{
			mpfnv_Callback(mp_Owner, psel, psel->mp_Data, l_oldData);
			psel = (EVAV_cl_ViewItem *) GetItemData(iSel);
		}

		if(psel->mpfn_CB)
		{
			psel->mpfn_CB(mp_Owner, psel, psel->mp_Data, l_oldData);
			psel = (EVAV_cl_ViewItem *) GetItemData(iSel);
		}

		MulSel(psel);
		LINK_gb_CanRefresh = TRUE;
		LINK_Refresh();
	}

	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnSelect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	int					i_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpo_CurrentWnd == &mo_ButtonSelect)
	{
		OnButtonChangeSelect();
		return;
	}

	if(mpo_CurrentWnd == &mo_CheckSelect)
	{
		if(((CButton *) mpo_CurrentWnd)->GetCheck())
			((CButton *) mpo_CurrentWnd)->SetCheck(0);
		else
			((CButton *) mpo_CurrentWnd)->SetCheck(1);
		OnCheckChangeSelect();
		return;
	}

	if(mpo_CurrentWnd == &mo_EditSelect)
	{
		OnEditChangeSelect();
		mo_EditSelect.SetFocus();
		return;
	}

	if(mpo_CurrentWnd == &mo_ComboSelect)
	{
		mo_ComboSelect.SetFocus();
		return;
	}

	/* Expand or not a category or a pointer */
	i_Sel = GetCurSel();
	if(i_Sel != LB_ERR)
	{
		po_Item = (EVAV_cl_ViewItem *) GetItemData(i_Sel);
		if
		(
			(po_Item->me_Type == EVAV_EVVIT_Separator)
		||	(po_Item->me_Type == EVAV_EVVIT_Pointer)
		||	(po_Item->me_Type == EVAV_EVVIT_SubStruct)
		||	(po_Item->mul_Flags & EVAV_Array)
		)
		{
			if((po_Item->me_Type != EVAV_EVVIT_Pointer) || (po_Item->mp_Data)) OnExpand(WM_USER + po_Item->mi_ID);
			return;
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Call to delete an entry in the view display.
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnDelete(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	POSITION			pos;
	int					i_Sel, i_Group, i_Indent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Sel = GetCurSel();
	if(i_Sel != LB_ERR)
	{
		SetRedraw(FALSE);
		po_Item = (EVAV_cl_ViewItem *) GetItemData(i_Sel);
		if(po_Item->mul_Flags & EVAV_CanDelete)
		{
			/* Is it a separator ? */
			pos = mpo_ListItems->Find(po_Item);
			if(po_Item->me_Type == EVAV_EVVIT_Separator)
			{
				i_Group = po_Item->mi_NumGroup;
				DeleteString(i_Sel);
				delete po_Item;
				mpo_ListItems->RemoveAt(pos);
				po_Item = (EVAV_cl_ViewItem *) GetItemData(i_Sel);
				while(((int) po_Item != LB_ERR) && (po_Item->mi_NumGroup == i_Group + 1))
				{
					pos = mpo_ListItems->Find(po_Item);
					DeleteString(i_Sel);
					delete po_Item;
					mpo_ListItems->RemoveAt(pos);
					po_Item = (EVAV_cl_ViewItem *) GetItemData(i_Sel);
				}
			}

			/* Or a pointer */
			else if
				(
					(po_Item->me_Type == EVAV_EVVIT_Pointer)
				||	(po_Item->me_Type == EVAV_EVVIT_SubStruct)
				||	(po_Item->mul_Flags & EVAV_Array)
				)
			{
				i_Indent = po_Item->mi_Indent;
				DeleteString(i_Sel);
				delete po_Item;
				mpo_ListItems->RemoveAt(pos);
				po_Item = (EVAV_cl_ViewItem *) GetItemData(i_Sel);
				while(((int) po_Item != LB_ERR) && (po_Item->mi_Indent > i_Indent))
				{
					pos = mpo_ListItems->Find(po_Item);
					DeleteString(i_Sel);
					delete po_Item;
					mpo_ListItems->RemoveAt(pos);
					po_Item = (EVAV_cl_ViewItem *) GetItemData(i_Sel);
				}
			}

			/* Or a single item */
			else
			{
				DeleteString(i_Sel);
				delete po_Item;
				mpo_ListItems->RemoveAt(pos);
			}

			/* New selection */
			if(i_Sel >= GetCount()) i_Sel--;
			SetCurSel(i_Sel);
		}

		if(GetCount() == 0) MoveWindow(0, 0, 0, 0);
		SetRedraw(TRUE);
		Invalidate();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVAV_cl_ListBox::DragALong(CPoint pt, void *p_Data, int i_Type, int i_Param1, EVAV_cl_ViewItem *p_Dst, BOOL _b_Cpy)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*_po_Dst;
	int					i_Sel;
	BOOL				b_Outside;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gpo_CurVavListBox = this;

	ScreenToClient(&pt);
	if(p_Dst == NULL)
	{
		i_Sel = ItemFromPoint(pt, b_Outside);
		if(i_Sel >= GetCount()) return FALSE;
		if(b_Outside) return FALSE;
		if((short) i_Sel == -1) return FALSE;
		_po_Dst = (EVAV_cl_ViewItem *) GetItemData(i_Sel);
	}
	else
		_po_Dst = p_Dst;
	if(!_po_Dst) return FALSE;
	if(_po_Dst->me_Type != i_Type) return FALSE;
	if(!_b_Cpy) return TRUE;
	switch(_po_Dst->me_Type)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EVAV_EVVIT_Bool:
		*(BOOL *) _po_Dst->mp_Data = *(BOOL *) p_Data;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EVAV_EVVIT_Vector:
		MATH_CopyVector((MATH_tdst_Vector *) _po_Dst->mp_Data, (MATH_tdst_Vector *) p_Data);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EVAV_EVVIT_Text:
		L_memcpy((TEXT_tdst_Eval *) _po_Dst->mp_Data, (TEXT_tdst_Eval *) p_Data, sizeof(TEXT_tdst_Eval));
		sgapfnv_SelectArray[_po_Dst->me_Type]((CWnd *) &mo_ComboSelect, _po_Dst, _po_Dst->mp_Data, TRUE);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EVAV_EVVIT_Matrix:
		MATH_Copy33Matrix((MATH_tdst_Matrix *) _po_Dst->mp_Data, (MATH_tdst_Matrix *) p_Data);
		break;

	case EVAV_EVVIT_Trigger:
		L_memcpy((SCR_tt_Trigger *) _po_Dst->mp_Data, (SCR_tt_Trigger *) p_Data, sizeof(SCR_tt_Trigger));
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EVAV_EVVIT_Int:
	case EVAV_EVVIT_ConstInt:
	case EVAV_EVVIT_Hexa:
		switch(abs(_po_Dst->mi_Param3))
		{
		case 1:
			*(char *) _po_Dst->mp_Data = (char) (*(long *) p_Data);
			break;
		case 2:
			*(short *) _po_Dst->mp_Data = (short) (*(long *) p_Data);
			break;
		case 4:
		default:
			*(long *) _po_Dst->mp_Data = *(long *) p_Data;
			break;
		}

		sgapfnv_SelectArray[_po_Dst->me_Type]((CWnd *) &mo_EditSelect, _po_Dst, p_Data, TRUE);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EVAV_EVVIT_Anim:
		break;
	case EVAV_EVVIT_Skeleton:
		if(*(long *) _po_Dst->mp_Data != *(long *) p_Data)
		{
			if(*(long *) _po_Dst->mp_Data) OBJ_FreeGroup(*((OBJ_tdst_Group **) _po_Dst->mp_Data));
			if(*(long *) p_Data) ANI_UseSkeleton(*(OBJ_tdst_Group **) p_Data);
		}

		*(long *) _po_Dst->mp_Data = *(long *) p_Data;
		break;
	case EVAV_EVVIT_Skin:
		if(*(long *) _po_Dst->mp_Data != *(long *) p_Data)
		{
			if(*(long *) _po_Dst->mp_Data) ANI_FreeShape((ANI_tdst_Shape **) _po_Dst->mp_Data);
			if(*(long *) p_Data) ANI_UseShape(*(ANI_tdst_Shape **) p_Data);
		}

		*(long *) _po_Dst->mp_Data = *(long *) p_Data;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EVAV_EVVIT_AdrLoaFile:
	case EVAV_EVVIT_GO:
	case EVAV_EVVIT_NetWork:
	case EVAV_EVVIT_AIFunction:
		*(long *) _po_Dst->mp_Data = *(long *) p_Data;
		sgapfnv_SelectArray[_po_Dst->me_Type]((CWnd *) &mo_ComboSelect, _po_Dst, _po_Dst->mp_Data, TRUE);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EVAV_EVVIT_Key:
	case EVAV_EVVIT_SndKey:
		*(BIG_KEY *) _po_Dst->mp_Data = *(BIG_KEY *) p_Data;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EVAV_EVVIT_Float:
		*(long *) _po_Dst->mp_Data = *(long *) p_Data;
		sgapfnv_SelectArray[_po_Dst->me_Type]((CWnd *) &mo_EditSelect, _po_Dst, _po_Dst->mp_Data, TRUE);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EVAV_EVVIT_Color:
		*(long *) _po_Dst->mp_Data = *(long *) p_Data;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	default:
		return FALSE;
	}

	Invalidate();
	if(mpfnv_Callback) mpfnv_Callback(mp_Owner, _po_Dst, _po_Dst->mp_Data, 0);
	if(_po_Dst->mpfn_CB) _po_Dst->mpfn_CB(mp_Owner, _po_Dst, _po_Dst->mp_Data, 0);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::Copy(EVAV_cl_ViewItem *_po_Dst, EVAV_cl_ViewItem *_po_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos, pos1;
	EVAV_cl_ViewItem	*po_Item, *po_Item1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_po_Dst->me_Type == _po_Src->me_Type)
	{
		/* Copy a struct */
		if((_po_Dst->me_Type == EVAV_EVVIT_Pointer) || (_po_Dst->me_Type == EVAV_EVVIT_SubStruct))
		{
			if(_po_Dst->mi_Param1 == _po_Dst->mi_Param1)
			{
				pos = mpo_ListItems->Find(_po_Dst);
				pos1 = EVAV_gpo_ListItems->Find(_po_Src);
				if(!pos1) return;
				mpo_ListItems->GetNext(pos);
				mpo_ListItems->GetNext(pos1);
				while(pos && pos1)
				{
					po_Item = mpo_ListItems->GetAt(pos);
					po_Item1 = EVAV_gpo_ListItems->GetAt(pos1);
					if(po_Item->mi_Indent <= _po_Dst->mi_Indent) break;
					if(po_Item1->mi_Indent <= _po_Src->mi_Indent) break;
					Copy(po_Item, po_Item1);
					mpo_ListItems->GetNext(pos);
					EVAV_gpo_ListItems->GetNext(pos1);
				}
			}
		}
        else if (_po_Src->me_Type == EVAV_EVVIT_CustSep)
        {
            if ( (_po_Src->mi_Param1 == LINK_C_MDF_Modifier) && (_po_Dst->mi_Param1 == LINK_C_MDF_Modifier) )
            {
				MDF_Modifier_Copy( (MDF_tdst_Modifier *) _po_Dst->mp_Data, (MDF_tdst_Modifier *) _po_Src->mp_Data );
			}
        }
		else
		{
            /* Copy a field */
			DragALong(CPoint(0, 0), _po_Src->mp_Data, _po_Src->me_Type, _po_Src->mi_Param3, _po_Dst);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	/*~~~~~~*/
	MSG msg;
	int i_Sel;
	/*~~~~~~*/

	if(L_isalnum(nChar) || ((nChar >= VK_NUMPAD0) && (nChar < VK_NUMPAD9)))
	{
		if(mpo_CurrentWnd && mpo_CurrentWnd == &mo_EditSelect)
		{
			mpo_CurrentWnd->SetFocus();
			mo_EditSelect.SetSel(0, -1);
			mo_EditSelect.Clear();
			msg.hwnd = mpo_CurrentWnd->m_hWnd;
			msg.message = WM_KEYDOWN;
			msg.wParam = nChar;
			msg.lParam = nRepCnt + (nFlags << 16);
			TranslateMessage(&msg);
			return;
		}
	}

	switch(nChar)
	{
	case VK_TAB:	i_Sel = GetCurSel(); SetCurSel(i_Sel + 1); break;
	case VK_RETURN: OnSelect(); break;
	case VK_DELETE: OnDelete(); break;
	default:		CListBox::OnKeyDown(nChar, nRepCnt, nFlags); break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(nSBCode != SB_ENDSCROLL) CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::AddMano(CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	int					iSel, iVal;
	char				cOffset;
	CRect				o_Rect;
	int					i_Div;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gpo_CurVavListBox = this;

	if(!mpo_CurrentWnd) return;
	if(mpo_CurrentWnd != &mo_ButtonSelect) return;
	iSel = GetCurSel();
	if(iSel == -1) return;
	po_Item = (EVAV_cl_ViewItem *) GetItemData(iSel);
	GetItemRect(iSel, &o_Rect);
	o_Rect.left = mi_WidthCol1;
	if(o_Rect.PtInRect(pt) || EVAV_gi_ScrollM)
	{
		pt.x -= o_Rect.left;
		o_Rect.right -= o_Rect.left;
		o_Rect.left = 0;

		/* Div item rect for each component */
		i_Div = 3;
		switch(po_Item->me_Type)
		{
		case EVAV_EVVIT_Color:	i_Div = 4; break;
		case EVAV_EVVIT_Vector: i_Div = 3; o_Rect.right = o_Rect.left + 185; break;
		}

		/* Which data we touched ? */
		if
		(
			(EVAV_gi_ScrollM == 1 || pt.x <= o_Rect.Width() / i_Div)
		&&	(EVAV_gi_ScrollM != 2)
		&&	(EVAV_gi_ScrollM != 3)
		&&	(EVAV_gi_ScrollM != 4)
		)
		{
			EVAV_gi_ScrollM = 1;
		}
		else if
			(
				(EVAV_gi_ScrollM == 2 || pt.x <= (2 * o_Rect.Width() / i_Div))
			&&	(EVAV_gi_ScrollM != 1)
			&&	(EVAV_gi_ScrollM != 3)
			&&	(EVAV_gi_ScrollM != 4)
			)
		{
			EVAV_gi_ScrollM = 2;
		}
		else if
			(
				(EVAV_gi_ScrollM == 3 || pt.x <= (3 * o_Rect.Width() / i_Div))
			&&	(EVAV_gi_ScrollM != 1)
			&&	(EVAV_gi_ScrollM != 2)
			&&	(EVAV_gi_ScrollM != 4)
			)
		{
			EVAV_gi_ScrollM = 3;
		}
		else
		{
			EVAV_gi_ScrollM = 4;
		}

		/* Process data depending on type */
		switch(po_Item->me_Type)
		{
		case EVAV_EVVIT_Color:
			cOffset = (char) (pt.y - EVAV_gpt_ScrollPt) % 256;
			switch(EVAV_gi_ScrollM)
			{
			case 1:
				iVal = ((char *) po_Item->mp_Data)[0];
				iVal &= 0xFF;
				iVal -= cOffset;
				if(iVal < 0) iVal = 0;
				if(iVal > 255) iVal = 255;
				((char *) po_Item->mp_Data)[0] = iVal;
				break;
			case 2:
				iVal = ((char *) po_Item->mp_Data)[1];
				iVal &= 0xFF;
				iVal -= cOffset;
				if(iVal < 0) iVal = 0;
				if(iVal > 255) iVal = 255;
				((char *) po_Item->mp_Data)[1] = iVal;
				break;
			case 3:
				iVal = ((char *) po_Item->mp_Data)[2];
				iVal &= 0xFF;
				iVal -= cOffset;
				if(iVal < 0) iVal = 0;
				if(iVal > 255) iVal = 255;
				((char *) po_Item->mp_Data)[2] = iVal;
				break;
			case 4:
				iVal = ((char *) po_Item->mp_Data)[3];
				iVal &= 0xFF;
				iVal -= cOffset;
				if(iVal < 0) iVal = 0;
				if(iVal > 255) iVal = 255;
				((char *) po_Item->mp_Data)[3] = iVal;
				break;
			}

			MulSel(po_Item);
			break;
		case EVAV_EVVIT_Vector:
			switch(EVAV_gi_ScrollM)
			{
			case 1: ((float *) po_Item->mp_Data)[0] -= (float) (pt.y - EVAV_gpt_ScrollPt) / 10; break;
			case 2: ((float *) po_Item->mp_Data)[1] -= (float) (pt.y - EVAV_gpt_ScrollPt) / 10; break;
			case 3: ((float *) po_Item->mp_Data)[2] -= (float) (pt.y - EVAV_gpt_ScrollPt) / 10; break;
			}

			MulSel(po_Item);
			break;
		}

		if(mpfnv_Callback) mpfnv_Callback(mp_Owner, po_Item, po_Item->mp_Data, 0);
		if(po_Item->mpfn_CB) po_Item->mpfn_CB(mp_Owner, po_Item, po_Item->mp_Data, 0);
		EVAV_gpt_ScrollPt = pt.y;
		Invalidate();
		RedrawWindow();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnMButtonDown(UINT uID, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Sel;
	BOOL				b;
	CRect				o_Rect;
	EVAV_cl_ViewItem	*po_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Sel = ItemFromPoint(pt, b);
	if((short) i_Sel != -1)
	{
		GetParent()->SetFocus();
		SetCurSel(i_Sel);
		SetFocus();
		if(mpo_CurrentWnd && (mpo_CurrentWnd != &mo_ButtonSelect))
		{
			ClientToScreen(&pt);
			mpo_CurrentWnd->ScreenToClient(&pt);
			mpo_CurrentWnd->SetFocus();
			mpo_CurrentWnd->SendMessage(WM_MBUTTONDOWN, uID, pt.x + (pt.y << 16));
		}
		else
		{
			i_Sel = GetCurSel();
			if(i_Sel != -1)
			{
				po_Item = (EVAV_cl_ViewItem *) GetItemData(i_Sel);
				GetItemRect(i_Sel, &o_Rect);
				o_Rect.left = mi_WidthCol1;
				if(o_Rect.PtInRect(pt))
				{
					EVAV_gpt_ScrollPt = pt.y;
					EVAV_gpt_ScrollCan = FALSE;
					EVAV_gi_ScrollM = 0;
					SetCapture();
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVAV_cl_ListBox::OnMouseWheel(UINT a, short zDelta, CPoint pt)
{
	return CListBox::OnMouseWheel(a, zDelta, pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnMButtonUp(UINT uID, CPoint pt)
{
	EVAV_gi_ScrollM = 0;
	ReleaseCapture();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnLButtonUp(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	CListBox::OnLButtonUp(n, pt);
	RedrawWindow();
	ReleaseCapture();
	EVAV_gi_ScrollM = 0;
	if(mpo_CurrentWnd)
	{
		mpo_CurrentWnd->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);
		if(o_Rect.PtInRect(pt)) mpo_CurrentWnd->SetFocus();
	}

	if(EDI_gst_DragDrop.b_BeginDragDrop && mb_CanDragDrop)
	{
		if(EDI_gst_DragDrop.b_CanDragDrop || !EDI_gst_DragDrop.o_OutRect.IsRectEmpty())
			M_MF()->EndDragDrop(pt);
		else
		{
			M_MF()->EndDragDrop(pt);
			OnDelete();
		}

		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnLButtonDown(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect				o_Rect;
	EVAV_cl_ViewItem	*po_Item;
	int					i_Sel;
	BOOL				b;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * To be sure that current list is still valid £
	 * LINK_UpdatePointers();
	 */
	CListBox::OnLButtonDown(n, pt);
	Invalidate();
	EVAV_gi_ScrollM = 0;
	EVAV_gpt_ScrollCan = FALSE;
	EVAV_gpt_ScrollPt = pt.y;

	/* Deselect */
	i_Sel = ItemFromPoint(pt, b);
	if(i_Sel == GetCount() - 1)
	{
		GetItemRect(i_Sel, &o_Rect);
		if(!o_Rect.PtInRect(pt)) i_Sel = -1;
	}

	if((short) i_Sel == -1)
	{
		if(mpo_CurrentWnd)
		{
			SetCurSel(-1);
			mpo_CurrentWnd->ShowWindow(SW_HIDE);
			mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
			mpo_CurrentWnd = NULL;
		}

		GetParent()->SetFocus();
		return;
	}

	/* Are we in the lock box ? */
	po_Item = (EVAV_cl_ViewItem *) GetItemData(GetCurSel());
	GetItemRect(GetCurSel(), &o_Rect);
	o_Rect.left = o_Rect.right - 20;
	if(o_Rect.PtInRect(pt))
	{
		if(po_Item->me_Type == EVAV_EVVIT_Separator)
		{
			po_Item->mb_Locked = po_Item->mb_Locked ? FALSE : TRUE;
			return;
		}
	}

	/* Drag & drop operation */
	if(mb_CanDragDrop)
	{
		po_Item = (EVAV_cl_ViewItem *) GetItemData(i_Sel);
        if
		(
            (po_Item->me_Type == EVAV_EVVIT_SndKey)
		||	(po_Item->me_Type == EVAV_EVVIT_Separator)
		||	(po_Item->me_Type == EVAV_EVVIT_Pointer)
		||	(po_Item->me_Type == EVAV_EVVIT_SubStruct)
		||	(po_Item->mul_Flags & EVAV_Array)        
		)
		{
			EDI_gst_DragDrop.ul_FatDir = BIG_C_InvalidIndex;
			EDI_gst_DragDrop.ul_FatFile = BIG_C_InvalidIndex;
			EDI_gst_DragDrop.i_Param1 = GetItemData(GetCurSel());
			if(EDI_gst_DragDrop.i_Param1 != -1)
			{
                if((po_Item->me_Type == EVAV_EVVIT_Pointer) && (po_Item->mi_Param1 == LINK_C_SND_Bank)) 
                {
                    EDI_gst_DragDrop.i_Param2 = (int) po_Item->mp_Data ;
                    M_MF()->BeginDragDrop(pt, this, (EDI_cl_BaseFrame *) GetParent()->GetParent(), EDI_DD_Data);                    
                }
                else if((po_Item->me_Type == EVAV_EVVIT_SubStruct) && (po_Item->mi_Param1 == LINK_C_SND_Sound)) 
                {
                    EDI_gst_DragDrop.i_Param2 = (int) po_Item->mp_Data ;
                    M_MF()->BeginDragDrop(pt, this, (EDI_cl_BaseFrame *) GetParent()->GetParent(), EDI_DD_Data);                    
                }
                else if(po_Item->me_Type == EVAV_EVVIT_SndKey) 
                {
                    EDI_gst_DragDrop.ul_FatFile = *(ULONG*)po_Item->mp_Data;
                    EDI_gst_DragDrop.ul_FatFile = BIG_ul_SearchKeyToFat(EDI_gst_DragDrop.ul_FatFile);
                    M_MF()->BeginDragDrop(pt, this, (EDI_cl_BaseFrame *) GetParent()->GetParent(), EDI_DD_File);
                }				
                else
                {
                    M_MF()->BeginDragDrop(pt, this, (EDI_cl_BaseFrame *) GetParent()->GetParent(), EDI_DD_User);
                }				
			}
		}
		else if(pt.x < mi_WidthCol1)
		{
			EDI_gst_DragDrop.i_Param1 = (int) po_Item->mp_Data;
			EDI_gst_DragDrop.i_Param2 = po_Item->me_Type;
			EDI_gst_DragDrop.i_Param3 = po_Item->mi_Param3;
			if(EDI_gst_DragDrop.i_Param1 != -1)
			{
				M_MF()->BeginDragDrop(pt, this, (EDI_cl_BaseFrame *) GetParent()->GetParent(), EDI_DD_Long);
			}
		}
	}

	/* Pointer/Struct/Categ ? */
	i_Sel = GetCurSel();
	if(i_Sel != LB_ERR)
	{
		po_Item = (EVAV_cl_ViewItem *) GetItemData(i_Sel);
		if
		(
			(po_Item->me_Type == EVAV_EVVIT_Separator)
		||	(po_Item->me_Type == EVAV_EVVIT_Pointer)
		||	(po_Item->me_Type == EVAV_EVVIT_SubStruct)
		||	(po_Item->mul_Flags & EVAV_Array)
		)
		{
			if((po_Item->me_Type != EVAV_EVVIT_Pointer) || (po_Item->mp_Data))
			{
				GetItemRect(i_Sel, o_Rect);
				o_Rect.left = (po_Item->mi_Indent * INDENT_C_STEP) + DEC;
				o_Rect.right = o_Rect.left + 11;
				o_Rect.top += 3;
				o_Rect.bottom = o_Rect.top + 11;
				if(o_Rect.PtInRect(pt)) OnExpand(WM_USER + po_Item->mi_ID);
			}

			return;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnLButtonDblClk(UINT n, CPoint pt)
{
	/*~~~~~~~~~~~*/
	int		i_Sel;
	BOOL	b;
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	i_Sel = ItemFromPoint(pt, b);
	if(i_Sel == GetCount() - 1)
	{
		GetItemRect(i_Sel, &o_Rect);
		if(!o_Rect.PtInRect(pt)) i_Sel = -1;
	}

	if((short) i_Sel == -1)
	{
		if(mpo_CurrentWnd)
		{
			SetCurSel(-1);
			mpo_CurrentWnd->ShowWindow(SW_HIDE);
			mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
			mpo_CurrentWnd = NULL;
		}
	}
	else
	{
		OnSelect();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL b_CanZoomObj(OBJ_tdst_GameObject *pst_GO, int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	EOUT_cl_Frame	*po_Out;
	void			*p_Adr;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if((int) pst_GO == AI_C_MainActor0) return FALSE;
	if((int) pst_GO == AI_C_MainActor1) return FALSE;
	if((int) pst_GO == BIG_C_InvalidKey) return FALSE;

	po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, _i_Num);
	if(po_Out == NULL) return FALSE;
	if(po_Out->mul_CurrentWorld == BIG_C_InvalidIndex) return FALSE;
	p_Adr = (void *) LOA_ul_SearchAddress(BIG_PosFile(po_Out->mul_CurrentWorld));
	if((int) p_Adr == -1) return FALSE;
	if(p_Adr != pst_GO->pst_World) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnRButtonDown(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu		o_Menu(FALSE);
	int					i_Sel, i_Count;
	EVAV_cl_ViewItem	*po_Item, *po_First;
	BOOL				b;
	LINK_tdst_Pointer	*p2;
	EOUT_cl_Frame		*po_Out;
	WOR_tdst_World		*pst_World;
	OBJ_tdst_GameObject *pst_GO;
	SOFT_tdst_PickingBuffer_Pixel	*pst_Pick;
	extern OBJ_tdst_GameObject *OUT_gpst_GORef;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mb_CanDragDrop && EDI_gst_DragDrop.b_BeginDragDrop)
		M_MF()->CancelDragDrop();
	else
	{
		i_Sel = ItemFromPoint(pt, b);
		if((short) i_Sel != -1)
		{
			if(i_Sel != GetCurSel())
			{
				GetParent()->SetFocus();
				SetCurSel(i_Sel);
				SetFocus();
			}

			po_Item = (EVAV_cl_ViewItem *) GetItemData(GetCurSel());
			M_MF()->InitPopupMenuAction(NULL, &o_Menu);

			/* Tableau */
			if(po_Item->p_data_father)
			{
                M_MF()->AddPopupMenuAction(NULL, &o_Menu, 40, TRUE, "Move Down");
                M_MF()->AddPopupMenuAction(NULL, &o_Menu, 41, TRUE, "Move Up");
                M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "");
			}

            /* special for sound */
            if(
                (po_Item->me_Type == EVAV_EVVIT_SndKey)//|| whats the fuck ?
                //(po_Item->me_Type == EVAV_EVVIT_String)
                )
            {
                M_MF()->AddPopupMenuAction(NULL, &o_Menu, 21, TRUE, "Show in Browser");                
                M_MF()->AddPopupMenuAction(NULL, &o_Menu, 24, TRUE, "Edit");                
            }
            else
            {
			    /* Copy / Paste */
			    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Copy ");
			    if(EVAV_gpo_Copy || OUT_gpst_GORef) M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Paste ");

			    if
			    (
				    (po_Item->me_Type == EVAV_EVVIT_Int)
			    ||	(po_Item->me_Type == EVAV_EVVIT_Float)
			    ||	(po_Item->me_Type == EVAV_EVVIT_GO)
			    ||	(po_Item->me_Type == EVAV_EVVIT_Color)
			    ||	(po_Item->me_Type == EVAV_EVVIT_Key)
			    ||	(po_Item->me_Type == EVAV_EVVIT_SndKey)
			    ||	(po_Item->me_Type == EVAV_EVVIT_Vector)
			    ||	(po_Item->me_Type == EVAV_EVVIT_NetWork)
			    ||	(po_Item->me_Type == EVAV_EVVIT_AIFunction)
			    ||	(po_Item->me_Type == EVAV_EVVIT_ConstInt)
			    ||	(po_Item->me_Type == EVAV_EVVIT_ConstFloat)
			    )
			    {
				    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 5, TRUE, "AI Add Watch");
			    }
			    if(AI_gi_WatchNum)
			    {
				    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 6, TRUE, "AI Del All Watch");
			    }

				/* Sound Color */
				if(po_Item->me_Type == EVAV_EVVIT_Color)
				{
                    if(!L_strncmp("Sound Color", po_Item->masz_Name, 11))
                    {
				        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 35, TRUE, "Assign Color to the entire sound id");

					}
				}
			    
			    if ( po_Item->me_Type == EVAV_EVVIT_Vector )
			    {
					po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
					if(po_Out && po_Out->mpo_EngineFrame && po_Out->mpo_EngineFrame->mpo_DisplayView && po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World )
					{
						pst_World = po_Out->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World;
						if (pst_World->uc_ShowVector < WOR_Cte_DbgShowVector )
						{
							pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem( pst_World->pst_Selection, SEL_C_SIF_Object);
							
							M_MF()->AddPopupMenuAction( NULL, &o_Menu, 30, TRUE, "Show position (global)" );
							if ( pst_GO )
							{
								M_MF()->AddPopupMenuAction( NULL, &o_Menu, 31, TRUE, "Show vector (global)" );
								M_MF()->AddPopupMenuAction( NULL, &o_Menu, 32, TRUE, "Show position (local)" );
								M_MF()->AddPopupMenuAction( NULL, &o_Menu, 33, TRUE, "Show vector (local)" );
							}
						}
						if (pst_World->uc_ShowVector)
						{
							M_MF()->AddPopupMenuAction( NULL, &o_Menu, 34, TRUE, "Del all Show vector/position" );
						}
					}
				}

			    /* Kit Name */
			    if((po_Item->me_Type == EVAV_EVVIT_Pointer) || (po_Item->me_Type == EVAV_EVVIT_SubStruct))
			    {
				    LINK_DeleteNameKits();
				    LINK_ReadNameKits();
				    if(LINK_gast_StructTypes[po_Item->mi_Param1].i_MaxNameKit)
				    {
					    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "Names Kits");
					    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Default");
					    for(i_Count = 0; i_Count < LINK_gast_StructTypes[po_Item->mi_Param1].i_MaxNameKit; i_Count++)
					    {
						    M_MF()->AddPopupMenuAction
							    (
								    NULL,
								    &o_Menu,
								    100 + i_Count,
								    TRUE,
								    LINK_gast_StructTypes[po_Item->mi_Param1].apsz_NameKits[i_Count][0]
							    );
					    }
				    }
			    }

			    /* Zoom for GAO */
			    if((po_Item->me_Type == EVAV_EVVIT_GO) && (*(int *) po_Item->mp_Data))// && !(po_Item->mul_Flags & EVAV_Array))
			    {
				    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "");
				    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 12, TRUE, "Set to none");
				    if(b_CanZoomObj(*(OBJ_tdst_GameObject **) po_Item->mp_Data, 0))
					{
						M_MF()->AddPopupMenuAction(NULL, &o_Menu, 11, TRUE, "Select");
					    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 10, TRUE, "Zoom");
					}
			    }

				if(po_Item->me_Type == EVAV_EVVIT_GO)
				{
				    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "");
				    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 13, TRUE, "Pick");
				}

                /* go to file */
                if((po_Item->me_Type == EVAV_EVVIT_Pointer) && (po_Item->mi_Param1 == LINK_C_SND_Bank))
                {
                    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 19, TRUE, "Show in Browser");
                    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 20, TRUE, "Modify");
                }
                
                if((po_Item->me_Type == EVAV_EVVIT_SubStruct) && (po_Item->mi_Param1 == LINK_C_SND_Sound))
                {
                    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 22, TRUE, "Show in Browser");                    
                    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 23, TRUE, "Edit");                    
                }
            }

            ClientToScreen(&pt);
			i_Sel = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
			if(i_Sel == -1) return;
			switch(i_Sel)
			{
			/* Copy */
			case 1:
				OUT_gpst_GORef=NULL;

				if(EVAV_gpo_Copy) delete EVAV_gpo_Copy;
				EVAV_gpo_Copy = new EVAV_cl_ViewItem;
				EVAV_gpo_Copy->CopyFrom(po_Item);
				EVAV_gpo_ListItems = mpo_ListItems;
				break;

			/* Paste */
			case 2:
				if(OUT_gpst_GORef) 
				{
					EVAV_gpo_ListItems = NULL;
					if(EVAV_gpo_Copy) delete EVAV_gpo_Copy;
					EVAV_gpo_Copy = new EVAV_cl_ViewItem;
					
					L_strcpy(EVAV_gpo_Copy->masz_Name, po_Item->masz_Name); 
					EVAV_gpo_Copy->me_Type=  EVAV_EVVIT_GO;
					EVAV_gpo_Copy->mp_Data = &OUT_gpst_GORef;
					EVAV_gpo_Copy->mul_Flags = 0; 
					EVAV_gpo_Copy->mi_Param1 = 0; 
					EVAV_gpo_Copy->mi_Param2 = -1; 
					EVAV_gpo_Copy->mi_Param3 = 0; 
					EVAV_gpo_Copy->mpfn_CB = NULL; 
					EVAV_gpo_Copy->mx_Color = 0;
					EVAV_gpo_Copy->mi_Param4 = 0;
					EVAV_gpo_Copy->mi_Param5 = 0;
					EVAV_gpo_Copy->mi_Param6 = 0;
					EVAV_gpo_Copy->mi_Param7 = 0;
					EVAV_gpo_Copy->psz_Help = NULL;
					EVAV_gpo_Copy->mul_Offset = 0;

					/* Copy some infos */
					EVAV_gpo_Copy->mi_Display = 0;
					EVAV_gpo_Copy->mi_DisplayGroup = 0;
					EVAV_gpo_Copy->mi_ID = 0;
					EVAV_gpo_Copy->mi_NumFields = 0;
					EVAV_gpo_Copy->mb_Expand = 0;
					EVAV_gpo_Copy->mb_ExpandOnce = 0;
					EVAV_gpo_Copy->mi_Indent =0 ;
					EVAV_gpo_Copy->mi_NumGroup = 0;
					L_strcpy(EVAV_gpo_Copy->masz_Group, po_Item->masz_Group);

					Copy(po_Item, EVAV_gpo_Copy);
				}
				else if(EVAV_gpo_Copy) 
				{
					Copy(po_Item, EVAV_gpo_Copy);
				}
				LINK_Refresh();
				break;

			case 5:
				if(po_Item->me_Type == EVAV_EVVIT_Vector)
				{
					AI_AddWatch(po_Item->mp_Data, 12);
				}
				else
				{
					AI_AddWatch(po_Item->mp_Data, 4);
				}
				break;

			case 6:
				AI_DelAllWatch();
				break;

			/* Zoom */
			case 10:
				po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
				po_Out->mpo_EngineFrame->mpo_DisplayView->ZoomOnObj(*(OBJ_tdst_GameObject **) po_Item->mp_Data);
				LINK_Refresh();
				break;
			case 11:
				po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
				po_Out->mpo_EngineFrame->mpo_DisplayView->ForceSelectObject(*(OBJ_tdst_GameObject **) po_Item->mp_Data);
				LINK_Refresh();
				break;
			case 12:
				*(int *) po_Item->mp_Data = 0;
				if ( mpo_CurrentWnd ) 
				{
					mpo_CurrentWnd->ShowWindow(SW_HIDE);
					mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
					mpo_CurrentWnd = NULL;
				}
				break;

			/* Pick */
			case 13:
				{
					MSG		msg;
					POINT	pt;
					ULONG	ul_Res;
					int		ok;

					ok = 0;
					while(GetMessage(&msg, NULL, NULL, NULL))
					{
						::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PIPE));
						if(msg.message == WM_SETCURSOR) continue;
						if(msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) break;
						if(msg.message == WM_RBUTTONDOWN) break;
						if(msg.message == WM_LBUTTONDOWN)
						{
							ok = 1;
							break;
						}
					}

					SendMessage(WM_SETCURSOR);

					if(ok)
					{
						/* Boite de sélection */
						GetCursorPos(&pt);
						po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
						if(po_Out->mpo_EngineFrame->mpo_DisplayView->mb_SelectOn && po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog)
						{
							ul_Res = po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog->ul_GAOFromPt(pt);
							if(ul_Res && ul_Res != 1) 
							{
								*(int *) po_Item->mp_Data = ul_Res;
								if(mpo_CurrentWnd)
								{
									mpo_CurrentWnd->ShowWindow(SW_HIDE);
									mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
									mpo_CurrentWnd = NULL;
								}
								break;
							}
						}

						po_Out->mpo_EngineFrame->mpo_DisplayView->ScreenToClient(&pt);
						if(po_Out->mpo_EngineFrame->mpo_DisplayView->Pick_l_UnderPoint(&pt, SOFT_Cuc_PBQF_GameObject, 0))
						{
							pst_Pick = po_Out->mpo_EngineFrame->mpo_DisplayView->Pick_pst_GetFirst(SOFT_Cuc_PBQF_GameObject, -1);
							if(pst_Pick)
							{								
								*(int *) po_Item->mp_Data = pst_Pick->ul_Value;
								if(mpo_CurrentWnd)
								{
									mpo_CurrentWnd->ShowWindow(SW_HIDE);
									mpo_CurrentWnd->MoveWindow(0, 0, 0, 0);
									mpo_CurrentWnd = NULL;
								}

								break;
							}
						}
					}
				}
				break;
           
            /* find snd bank file */
            case 19:
                if(po_Item->mp_Data)
                {
                    EBRO_cl_Frame * po_Browser ;
                    ULONG ul_fat;
                    
					ul_fat = LOA_ul_SearchIndexWithAddress((ULONG) po_Item->mp_Data);                    
                    if(ul_fat != BIG_C_InvalidIndex)
                    {
                        po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
						po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
                        po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_fat), ul_fat);
                    }
                }
                break;

            /* modify sound bank */
            case 20:
                if(po_Item->mp_Data)
                {
                    ULONG ul_fat;
                        
                    ul_fat = LOA_ul_SearchIndexWithAddress((ULONG) po_Item->mp_Data);                
                    if(ul_fat != BIG_C_InvalidIndex)
                    {
                        BRO_OrderGroupFile(ul_fat);
                    }
                }
                break;

            /* find sound file */
            case 21:
                if(po_Item->mp_Data)
                {
                    EBRO_cl_Frame * po_Browser ;
                    ULONG ul_fat;
                        
                    ul_fat =*(ULONG*) po_Item->mp_Data ;
                    if(ul_fat != BIG_C_InvalidKey)
                    {
                        ul_fat = BIG_ul_SearchKeyToFat(ul_fat);
                        if(ul_fat != BIG_C_InvalidIndex)
                        {
                            po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
							po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
                            po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_fat), ul_fat);
                        }
                    }
                }
                break;
            /* edit sndkey */
            case 24:
                if(po_Item->mp_Data)
                {
                    ESON_cl_Frame * po_SoundEdi;
                    ULONG ul_fat;
                        
                    ul_fat =*(ULONG*) po_Item->mp_Data;
                    if(ul_fat != BIG_C_InvalidKey)
                    {
                        ul_fat = BIG_ul_SearchKeyToFat(ul_fat);
                        if(ul_fat != BIG_C_InvalidIndex)
                        {
                            po_SoundEdi= (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
							po_SoundEdi->mpo_MyView->IWantToBeActive(po_SoundEdi);
                            po_SoundEdi->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_fat), ul_fat);
                        }
                    }
                }
                break;

            case 22:
                if(po_Item->mp_Data)
                {
                    EBRO_cl_Frame * po_Browser ;
                    ULONG ul_fat;
                        
                    ul_fat =((SND_tdst_OneSound*) po_Item->mp_Data)->ul_FileKey ;
                    if(ul_fat != BIG_C_InvalidKey)
                    {
                        ul_fat = BIG_ul_SearchKeyToFat(ul_fat);
                        if(ul_fat != BIG_C_InvalidIndex)
                        {
                            po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
							po_Browser->mpo_MyView->IWantToBeActive(po_Browser);
                            po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_fat), ul_fat);
                        }
                    }
                }
                break;

            case 23:
                if(po_Item->mp_Data)
                {
                    ESON_cl_Frame * po_SoundEdi;
                    ULONG ul_fat;
                        
                    ul_fat =((SND_tdst_OneSound*) po_Item->mp_Data)->ul_FileKey ;
                    if(ul_fat != BIG_C_InvalidKey)
                    {
                        ul_fat = BIG_ul_SearchKeyToFat(ul_fat);
                        if(ul_fat != BIG_C_InvalidIndex)
                        {
                            po_SoundEdi= (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
							po_SoundEdi->mpo_MyView->IWantToBeActive(po_SoundEdi);
                            po_SoundEdi->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_fat), ul_fat);
                        }
                    }
                }
                break;
                
            case 30:
            case 31:
            case 32:
            case 33:
				pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].c_Local = (i_Sel >= 32) ? 1 : 0;
				pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].c_Vector = i_Sel & 1;
				pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].pst_Gao = pst_GO;
				pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].pst_Vector = (MATH_tdst_Vector *) po_Item->mp_Data;
				pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].ul_Color = 0xFFFFFFFF;
				pst_World->ast_ShowVector[ pst_World->uc_ShowVector ].c_Remember = 0;
				pst_World->uc_ShowVector++;
				break;
			case 34:
				pst_World->uc_ShowVector = 0;
				break;

			case 35:
				{
					po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);

					if
					(
						po_Out 
					&&	po_Out->mpo_EngineFrame 
					&&	po_Out->mpo_EngineFrame->mpo_DisplayView 
					&&	po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_GameMaterialDialog 
					&&	po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_GameMaterialDialog->mpo_View 
					&&	po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_GameMaterialDialog->mpo_View->mpst_GameMatList
					)
					{
						COL_tdst_GameMatList	*pst_GMatList;
						COL_tdst_GameMat		*pst_GMat, *pst_CurGMat;
						COL_tdst_GameMat		st_GMat;
						USHORT					uw_Offset;
						int						i, id;

						uw_Offset = (char *) &st_GMat.ul_SoundColor - (char *) &st_GMat;       
						pst_GMat = (COL_tdst_GameMat *) ((char *) po_Item->mp_Data - uw_Offset);

						pst_GMatList = po_Out->mpo_EngineFrame->mpo_DisplayView->mpo_GameMaterialDialog->mpo_View->mpst_GameMatList;
						id = -1;
						for(i = 0; i < (int) pst_GMatList->ul_GMat; i++)
						{
							pst_CurGMat = &pst_GMatList->pst_GMat[i];
							if(pst_CurGMat == pst_GMat)
							{
								id = pst_GMatList->pal_Id[i];
								break;
							}									
						}

						if(id != -1)
						{
							id /= 100;
							for(i = 0; i < (int) pst_GMatList->ul_GMat; i++)
							{
								pst_CurGMat = &pst_GMatList->pst_GMat[i];
								if((pst_GMatList->pal_Id[i] / 100) == id)
								{
									pst_CurGMat->ul_SoundColor = pst_GMat->ul_SoundColor;
								}
							}

							LINK_Refresh();
						}
					}
                }
				break;

			/* Move array */
			case 40:	MoveArray(1, po_Item); break;
			case 41:	MoveArray(-1, po_Item); break;

            /* Kit names */
			default:
				LINK_gast_StructTypes[po_Item->mi_Param1].i_NameKit = i_Sel - 99;
				i_Count = GetCurSel();
				po_First = po_Item;
				while(i_Count >= 0)
				{
					po_Item = (EVAV_cl_ViewItem *) GetItemData(i_Count);
					if((po_Item->me_Type == EVAV_EVVIT_Pointer) || (po_Item->me_Type == EVAV_EVVIT_SubStruct))
					{
						p2 = LINK_p_SearchPointer(po_Item->mp_Data);
						if(p2 && (po_Item->mi_Indent < po_First->mi_Indent))
						{
							LINK_UpdatePointer(po_Item->mp_Data);
							break;
						}
					}

					i_Count--;
				}

				LINK_UpdatePointers();
				break;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::MoveArray(int sens, EVAV_cl_ViewItem *po_Item)
{
	char	*data;

	data = (char *) po_Item->p_data_father;
	if(sens == 1) /* Down */
	{
		if(po_Item->index_me_father != po_Item->size_array_father - 1)
		{
			L_memmove
			(
				data + (po_Item->index_me_father + 1) * po_Item->size_elem_father,
				data + (po_Item->index_me_father + 0) * po_Item->size_elem_father,
				(po_Item->size_array_father - po_Item->index_me_father - 1) * po_Item->size_elem_father
			);

			L_memset(data + po_Item->index_me_father * po_Item->size_elem_father, 0, po_Item->size_elem_father);
		}
	}
	else /* Up */
	{
		if(po_Item->index_me_father)
		{
			L_memmove
			(
				data + (po_Item->index_me_father - 1) * po_Item->size_elem_father,
				data + (po_Item->index_me_father + 0) * po_Item->size_elem_father,
				(po_Item->size_array_father - po_Item->index_me_father) * po_Item->size_elem_father
			);

			L_memset(data + (po_Item->size_array_father - 1) * po_Item->size_elem_father, 0, po_Item->size_elem_father);
		}
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::OnMouseMove(UINT, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UINT				ui_Item;
	BOOL				bOutside;
	EVAV_cl_ViewItem	*po_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Drag & drop */
	if(mb_CanDragDrop && EDI_gst_DragDrop.b_BeginDragDrop)
	{
		M_MF()->b_MoveDragDrop(pt);
		if(!EDI_gst_DragDrop.b_CanDragDrop && EDI_gst_DragDrop.o_OutRect.IsRectEmpty())
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_DRAGDEL));
	}
	else
	{
		if(GetCapture() == this)
		{
			if(abs(pt.y - EVAV_gpt_ScrollPt) < 5) EVAV_gpt_ScrollCan = TRUE;
			if(EVAV_gpt_ScrollCan) AddMano(pt);
		}
		else
		{
			/* Display an help string (if present) in status bar */
			ui_Item = ItemFromPoint(pt, bOutside);
			if((short) ui_Item != -1)
			{
				po_Item = (EVAV_cl_ViewItem *) GetItemData(ui_Item);
				if(po_Item->psz_Help) EDI_Tooltip_DisplayMessage(po_Item->psz_Help, 1000);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
POSITION sfnx_IndentLevelPointer
(
	EVAV_cl_ViewItem								*_po_Group,
	EVAV_cl_ViewItem								*_po_Base,
	CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>	*_po_List,
	POSITION	_Pos,
	int			_i_Num,
	int			_i_Type,
	int			&_i_ID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * Set ID to retreive command of expand button. The sub id can be used to know if
	 * an item is inside a pointer, and in wich pointer it is
	 */
	_po_Base->mi_ID = _i_ID;
	_po_Base->mi_SubID = _i_ID++;

	/* Treat all the struct */
	_po_List->GetNext(_Pos);
	if(!_Pos) return NULL;
	for(i = 0; _Pos && (i < _po_Base->mi_NumFields); i++)
	{
		po_Item = _po_List->GetAt(_Pos);
		po_Item->mi_Indent = _i_Num + 1;
		po_Item->mi_NumGroup = _po_Base->mi_NumGroup;
		po_Item->mi_SubID = _po_Base->mi_ID;
		if(_po_Group) L_strcpy(po_Item->masz_Group, _po_Group->masz_Name);
		if
		(
			(po_Item->me_Type == EVAV_EVVIT_Pointer)
		||	(po_Item->me_Type == EVAV_EVVIT_SubStruct)
		||	(po_Item->mul_Flags & EVAV_Array)
		)
		{
			_Pos = sfnx_IndentLevelPointer(_po_Group, po_Item, _po_List, _Pos, _i_Num + 1, po_Item->mi_Param1, _i_ID);
			if(!_Pos) return NULL;
			continue;
		}

		if(!_Pos) return NULL;
		_po_List->GetNext(_Pos);
	}

	return _Pos;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::SortList(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
POSITION EVAV_cl_ListBox::TreatArray(EVAV_cl_ViewItem *po_Item, POSITION pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i1, i2, i3;
	char				asz_Name[10];
	EVAV_cl_ViewItem	*po_First, *po_New, *po_New1, *po_New2;
	int					iSize;
	unsigned long		ul_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Special case for an array */
	if(po_Item->mul_Flags & EVAV_Array)
	{
		po_First = po_Item;
		po_First->mp_Data = (char *) po_First->mp_Data + (4 * po_First->mi_Param4);
		switch(po_Item->me_Type)
		{
		case EVAV_EVVIT_Vector:		iSize = sizeof(MATH_tdst_Vector); break;
		case EVAV_EVVIT_Text:		iSize = sizeof(TEXT_tdst_Eval); break;
		case EVAV_EVVIT_Trigger:	iSize = sizeof(SCR_tt_Trigger); break;
		case EVAV_EVVIT_SubStruct:	if(po_Item->mi_Param1 == LINK_C_MSG_Struct) { iSize = sizeof(AI_tdst_Message); break; }
		default:					iSize = 4; break;
		}

		if(po_Item->mi_Param4 == 1) po_Item->mi_Param6 = po_Item->mi_Param7 = 1;
		if(po_Item->mi_Param4 == 2) po_Item->mi_Param7 = 1;

		po_Item->mi_NumFields = po_Item->mi_Param5;
		for(i1 = 0; i1 < po_Item->mi_Param5; i1++)
		{
			sprintf(asz_Name, "%d", i1);
			ul_Offset = po_Item->mi_Param6 * po_Item->mi_Param7 * i1 * iSize;
			po_New = new EVAV_cl_ViewItem
				(
					asz_Name,
					po_Item->me_Type,
					(char *) po_First->mp_Data + ul_Offset,
					po_Item->mul_Flags &~EVAV_Array,
					po_Item->mi_Param1,
					po_Item->mi_Param2,
					po_Item->mi_Param3,
					NULL,
					po_Item->mx_Color,
					po_Item->mi_Param8
				);

			/* Infos sur le tableau IA maitre */
			if(po_Item->mi_Param4 <= 1)
			{
				po_New->size_array_father = po_Item->mi_Param5;
				po_New->size_elem_father = iSize;
				po_New->index_me_father = i1;
				po_New->p_data_father = po_First->mp_Data;
			}

			po_New->mul_Offset = ul_Offset;
			po_New->mi_Indent = po_Item->mi_Indent + 1;
			po_New->mi_DisplayGroup = po_Item->mi_DisplayGroup;
			if(po_Item->mi_Param4 > 1)
			{
				po_New->mul_Flags |= EVAV_Array;
				po_New->mi_NumFields = po_Item->mi_Param6;
				po_New->mb_ExpandOnce = TRUE;
			}

			pos = mpo_ListItems->InsertAfter(pos, po_New);

			/* Dim 2 */
			if(po_Item->mi_Param4 > 1)
			{
				for(i2 = 0; i2 < po_Item->mi_Param6; i2++)
				{
					sprintf(asz_Name, "%d", i2);
					ul_Offset = (po_Item->mi_Param6 * po_Item->mi_Param7 * i1 * iSize) + (po_Item->mi_Param7 * i2 * iSize);
					po_New1 = new EVAV_cl_ViewItem
						(
							asz_Name,
							po_Item->me_Type,
							(char *) po_First->mp_Data + ul_Offset,
							po_Item->mul_Flags &~EVAV_Array,
							po_Item->mi_Param1,
							po_Item->mi_Param2,
							po_Item->mi_Param3,
							NULL,
							po_Item->mx_Color,
							po_Item->mi_Param8
						);
					po_New1->mul_Offset = ul_Offset;
					po_New1->mi_DisplayGroup = po_Item->mi_DisplayGroup;
					po_New1->mi_Display = 0;
					po_New1->mi_Indent = po_New->mi_Indent + 1;
					if(po_Item->mi_Param4 > 2)
					{
						po_New1->mul_Flags |= EVAV_Array;
						po_New1->mi_NumFields = po_Item->mi_Param7;
						po_New1->mb_ExpandOnce = TRUE;
					}

					pos = mpo_ListItems->InsertAfter(pos, po_New1);

					/* Dim 3 */
					if(po_Item->mi_Param4 > 2)
					{
						for(i3 = 0; i3 < po_Item->mi_Param7; i3++)
						{
							sprintf(asz_Name, "%d", i3);
							ul_Offset = (po_Item->mi_Param6 * po_Item->mi_Param7 * i1 * iSize) + (po_Item->mi_Param7 * i2 * iSize) + (i3 * iSize);
							po_New2 = new EVAV_cl_ViewItem
								(
									asz_Name,
									po_Item->me_Type,
									(char *) po_First->mp_Data + ul_Offset,
									po_Item->mul_Flags &~EVAV_Array,
									po_Item->mi_Param1,
									po_Item->mi_Param2,
									po_Item->mi_Param3,
									NULL,
									po_Item->mx_Color,
									po_Item->mi_Param8
								);
							po_New2->mul_Offset = ul_Offset;
							po_New2->mi_DisplayGroup = po_Item->mi_DisplayGroup;
							po_New2->mi_Display = -1;
							po_New2->mi_Indent = po_New1->mi_Indent + 1;
							pos = mpo_ListItems->InsertAfter(pos, po_New2);
						}
					}
				}
			}
		}
	}

	return pos;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::SetItemList(CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *> *_po_ListItems)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item, *po_Group;
	POSITION			pos, pos1;
	int					mi_Res, i_Group, i_ID, i_Indent;
	int					i_StType;
	CRect				o_Rect;
	int					iCanAutoExpand;
	BOOL				bCanAutoExpand;
	BOOL				bAutoExpand;
	CString				o_Str;
	int					i_ScrollV, i_ScrollH;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_ScrollV = GetScrollPos(SB_VERT);
	i_ScrollH = GetScrollPos(SB_HORZ);
	iCanAutoExpand = 0;
	bCanAutoExpand = FALSE;
	mpo_ListItems = _po_ListItems;
	SortList();
	M_MF()->LockDisplay(this);
	ResetContent();
	if(mpo_ListItems)
	{
		/* Set groups. */
		pos = mpo_ListItems->GetHeadPosition();
		i_Group = i_Indent = 0;
		i_ID = 0;
		po_Group = NULL;
		while(pos)
		{
			po_Item = mpo_ListItems->GetAt(pos);
			if(po_Item->me_Type == EVAV_EVVIT_Separator)
			{
				i_Group++;
				po_Group = po_Item;
				po_Item->mi_NumGroup = i_Group;
				po_Item->mi_ID = i_ID++;
				po_Item->mi_Indent = i_Indent = 0;
				L_strcpy(po_Item->masz_Group, po_Item->masz_Name);
				i_Group++;
			}
			else
			{
				if(po_Group) L_strcpy(po_Item->masz_Group, po_Group->masz_Name);
				po_Item->mi_NumGroup = i_Group;
				po_Item->mi_Indent = 1;

				/*
				 * Special case for pointers. Recurse init the indentation level. Retreive the
				 * structure type and name depending on the address pointer.
				 */
				if
				(
					(po_Item->me_Type == EVAV_EVVIT_Pointer)
				||	(po_Item->me_Type == EVAV_EVVIT_SubStruct)
				||	(po_Item->mul_Flags & EVAV_Array)
				)
				{
					if((po_Item->me_Type == EVAV_EVVIT_Pointer) || (po_Item->me_Type == EVAV_EVVIT_SubStruct))
					{
						/* Search type of structure. It must be set in first param */
						i_StType = po_Item->mi_Param1;

						/* Save name */
						L_strcpy(po_Item->masz_Name, LINK_gast_StructTypes[i_StType].psz_Name);
					}

_Try_
					/* Set indent level */
					pos = sfnx_IndentLevelPointer(po_Group, po_Item, mpo_ListItems, pos, 1, po_Item->mi_Param1, i_ID);
_Catch_
_End_
					continue;
				}
			}

			if(pos) mpo_ListItems->GetNext(pos);
		}

		/* Content. */
		pos = mpo_ListItems->GetHeadPosition();
		while(pos)
		{
			po_Item = mpo_ListItems->GetNext(pos);
			if(po_Item->mi_Display <= 0) continue;
			if(!(b_DispGrp(po_Item))) continue;
			mi_Res = AddString(po_Item->masz_Name);
			SetItemData(mi_Res, (long) po_Item);
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Take care of autoexpand flag
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mpo_ListItems)
	{
		bAutoExpand = FALSE;
		pos = mpo_ListItems->GetHeadPosition();
		while(pos)
		{
			po_Item = mpo_ListItems->GetAt(pos);

			/* Auto expand only first non locked separator */
			if(po_Item->me_Type == EVAV_EVVIT_Separator)
			{
				bCanAutoExpand = FALSE;
				if(!po_Item->mb_Locked)
				{
					iCanAutoExpand++;
					if(iCanAutoExpand == 1) bCanAutoExpand = TRUE;
				}
			}

			if((!po_Item->mb_Expand) && (!po_Item->mb_ExpandOnce))
			{
				if(po_Item->mul_Flags & EVAV_AutoExpand)
				{
					OnExpand(po_Item->mi_ID);
					po_Item->mul_Flags &= ~EVAV_AutoExpand;
					bAutoExpand = TRUE;
				}
				else if((po_Item->me_Type == EVAV_EVVIT_Pointer) || (po_Item->me_Type == EVAV_EVVIT_SubStruct))
				{
					if(bCanAutoExpand && mab_AutoExpandStruct[po_Item->mi_Param1] && po_Item->mp_Data)
					{
						pos1 = mas_AutoExpandStruct[po_Item->mi_Param1].GetHeadPosition();
						while(pos1)
						{
							o_Str = mas_AutoExpandStruct[po_Item->mi_Param1].GetNext(pos1);
							if(o_Str == po_Item->masz_Name)
							{
								OnExpand(WM_USER + po_Item->mi_ID);
								bAutoExpand = TRUE;
								break;
							}
						}
					}
				}
			}

			mpo_ListItems->GetNext(pos);
		}

		if(bAutoExpand) SetItemList(_po_ListItems);
	}

	M_MF()->UnlockDisplay(this);

	if(i_ScrollV)
	{
		SetScrollPos(SB_VERT, i_ScrollV);
		SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (i_ScrollV << 16), 0);
	}

	if(i_ScrollH)
	{
		SetScrollPos(SB_HORZ, i_ScrollH);
		SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (i_ScrollH << 16), 0);
	}

	EnableScroll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::BeforeRefresh(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect				o_Rect;
	int					iSel;
	EVAV_cl_ViewItem	*po_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Invalidate column with values */
	GetClientRect(&o_Rect);
	InvalidateRect(CRect(mi_WidthCol1, 0, o_Rect.right, o_Rect.bottom), FALSE);

	/* Valid rect under current selection window */
	if(mpo_CurrentWnd)
	{
		mpo_CurrentWnd->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);
		ValidateRect(o_Rect);
	}

	/* Validate substruct, pointers and separators */
	for(iSel = GetTopIndex(); iSel < GetCount(); iSel++)
	{
		po_Item = (EVAV_cl_ViewItem *) GetItemData(iSel);
		if
		(
			(po_Item->me_Type == EVAV_EVVIT_Separator)
		||	((po_Item->me_Type == EVAV_EVVIT_Pointer) && (po_Item->mi_Indent == 1))
		||	((po_Item->me_Type == EVAV_EVVIT_SubStruct) && (po_Item->mi_Indent == 1))
		||	((po_Item->mul_Flags & EVAV_Array) && (po_Item->mi_Indent == 1))
		)
		{
			GetItemRect(iSel, o_Rect);
			ValidateRect(o_Rect);
		}

		iSel++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_cl_ListBox::MulSel(EVAV_cl_ViewItem *_po_SelItem)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Sep, *po_First;
	POSITION			pos;
	LINK_tdst_Pointer	*p1, *p2;
	int					i, i1, i2;
	char				*pc_Dest;
	int					ai_Offsets[100];
	BOOL				ab_Pointers[100];
	int					i_NumOf;
	BOOL				b_Call;
	long				l_Old;
	OBJ_tdst_SingleBV	*pst_BV;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject st_GO;
	USHORT				us_Offset;	/* Offset to get the pst_BV from the beginning of the GO */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gpo_CurVavListBox = this;

	if(mo_MulSel.GetCount() == 0) return;
	po_Sep = po_IsVolatileSep(_po_SelItem);
	if(!po_Sep) return;
	pos = mpo_ListItems->Find(_po_SelItem);

	/* Retreive all the pointers above item */
	po_First = _po_SelItem;
	pc_Dest = (char *) _po_SelItem->mp_Data;
	i_NumOf = 0;
	do
	{
		po_Sep = po_First;
		do
		{
			po_Sep = mpo_ListItems->GetPrev(pos);
			if((po_Sep->me_Type == EVAV_EVVIT_Pointer) || (po_Sep->me_Type == EVAV_EVVIT_SubStruct))
			{
				if(po_Sep->mi_Indent == po_First->mi_Indent - 1)
				{
					if(!i_NumOf)
					{
						ai_Offsets[0] = (char *) _po_SelItem->mp_Data - (char *) po_Sep->mp_Data;
						ab_Pointers[0] = FALSE;
						ai_Offsets[1] = po_Sep->mul_Offset;
						ab_Pointers[1] = po_Sep->me_Type == EVAV_EVVIT_Pointer ? TRUE : FALSE;
						i_NumOf = 2;
					}
					else
					{
						if(po_Sep->mi_Indent > 1)
						{
							ai_Offsets[i_NumOf] = po_Sep->mul_Offset;
							ab_Pointers[i_NumOf] = po_Sep->me_Type == EVAV_EVVIT_Pointer ? TRUE : FALSE;
							i_NumOf++;
						}
					}

					po_First = po_Sep;
					break;
				}
			}
		} while(pos);
	} while(po_First->mi_Indent > 1);
	p1 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) po_First->mp_Data);
	if(!p1) return;

	/* Scan all mulsel list for the same type */
	pos = mo_MulSel.GetHeadPosition();
	while(pos)
	{
		p2 = (LINK_tdst_Pointer *) mo_MulSel.GetNext(pos);
		if(p1->i_Type == p2->i_Type)
		{
			/* Retreive the correct field */
			pc_Dest = (char *) p2->pv_Data + ai_Offsets[i_NumOf - 1];
			if(ab_Pointers[i_NumOf - 1])
			{
				if(*(long *) pc_Dest == NULL) goto l_End;
				pc_Dest = (char *) * (long *) pc_Dest;
			}

			i = i_NumOf - 2;
			while(i >= 0)
			{
				if(ab_Pointers[i])
				{
					pc_Dest += ai_Offsets[i];
					if(*(long *) pc_Dest == NULL) goto l_End;
					pc_Dest = (char *) * (long *) pc_Dest;
				}
				else
				{
					pc_Dest += ai_Offsets[i];
				}

				i--;
			}

			b_Call = FALSE;
			l_Old = 0;
			switch(_po_SelItem->me_Type)
			{

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case EVAV_EVVIT_Flags:
				switch(_po_SelItem->mi_Param2)
				{
				case 1:
					l_Old = *(char *) pc_Dest;
					i1 = 1 << _po_SelItem->mi_Param1;
					i2 = *(char *) _po_SelItem->mp_Data & i1;
					if(i2)
						*(char *) pc_Dest |= i1;
					else
						*(char *) pc_Dest &= ~i1;
					break;
				case 2:
					l_Old = *(short *) pc_Dest;
					i1 = 1 << _po_SelItem->mi_Param1;
					i2 = *(short *) _po_SelItem->mp_Data & i1;
					if(i2)
						*(short *) pc_Dest |= i1;
					else
						*(short *) pc_Dest &= ~i1;
					break;
				case 4:
					l_Old = *(long *) pc_Dest;
					i1 = 1 << _po_SelItem->mi_Param1;
					i2 = *(long *) _po_SelItem->mp_Data & i1;
					if(i2)
						*(long *) pc_Dest |= i1;
					else
						*(long *) pc_Dest &= ~i1;
					break;
				}

				b_Call = TRUE;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case EVAV_EVVIT_Key:
			case EVAV_EVVIT_SndKey:
				*(BIG_KEY *) pc_Dest = *(BIG_KEY *) _po_SelItem->mp_Data;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			case EVAV_EVVIT_ConstInt:
			case EVAV_EVVIT_Int:
				switch(_po_SelItem->mi_Param3)
				{
				case 1:
				case -1:
					*(char *) pc_Dest = *(char *) _po_SelItem->mp_Data;
					break;
				case 2:
				case -2:
					*(short *) pc_Dest = *(short *) _po_SelItem->mp_Data;
					break;
				case 4:
				case -4:
				default:
					l_Old = *(long *) pc_Dest;
					*(long *) pc_Dest = *(long *) _po_SelItem->mp_Data;
					break;
				}

				b_Call = TRUE;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case EVAV_EVVIT_Bool:
				*(BOOL *) pc_Dest = *(BOOL *) _po_SelItem->mp_Data;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case EVAV_EVVIT_Vector:
				MATH_CopyVector((MATH_tdst_Vector *) pc_Dest, (MATH_tdst_Vector *) _po_SelItem->mp_Data);
				b_Call = TRUE;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case EVAV_EVVIT_Matrix:
				MATH_Copy33Matrix((MATH_tdst_Matrix *) pc_Dest, (MATH_tdst_Matrix *) _po_SelItem->mp_Data);
				b_Call = TRUE;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case EVAV_EVVIT_BV:
				pst_BV = (OBJ_tdst_SingleBV *) * (long *) _po_SelItem->mp_Data;
				us_Offset = (char *) &st_GO.pst_BV - (char *) &st_GO;
				pst_GO = (OBJ_tdst_GameObject *) ((char *) pc_Dest - us_Offset);
				if(OBJ_BV_IsAABBox(pst_BV))
					OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_AABBox);
				else
					OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_Sphere);
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case EVAV_EVVIT_Text:
				L_memcpy((TEXT_tdst_Eval *) pc_Dest, (TEXT_tdst_Eval *) _po_SelItem->mp_Data, sizeof(TEXT_tdst_Eval));
				b_Call = TRUE;
				break;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			case EVAV_EVVIT_Hexa:
			case EVAV_EVVIT_Float:
			case EVAV_EVVIT_Color:
			case EVAV_EVVIT_AdrLoaFile:
			case EVAV_EVVIT_GO:
			case EVAV_EVVIT_NetWork:
			case EVAV_EVVIT_AIFunction:
			case EVAV_EVVIT_GroLightType:
				*(long *) pc_Dest = *(long *) _po_SelItem->mp_Data;
				b_Call = TRUE;
				break;
			}

			/* Inform user callback */
			if(b_Call)
			{
				LINK_gb_AllRefreshEnable = FALSE;
				if(mpfnv_Callback) mpfnv_Callback(mp_Owner, _po_SelItem, pc_Dest, l_Old);
				if(_po_SelItem->mpfn_CB) _po_SelItem->mpfn_CB(mp_Owner, _po_SelItem, pc_Dest, l_Old);
				LINK_gb_AllRefreshEnable = TRUE;
			}

l_End: ;
		}
	}

	LINK_UpdatePointers();
	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
    Edit box. Overwritten to disable some keys when editing a number
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EVAV_CEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_CEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	/*~~~~~~*/
	int i_Sel;
	/*~~~~~~*/

	if(nChar == VK_LEFT)
	{
	}

	if(nChar == VK_RIGHT)
	{
	}

	if(nChar == VK_DOWN)
	{
		GetParent()->SetFocus();
		GetParent()->Invalidate();
		i_Sel = ((CListBox *) GetParent())->GetCurSel();
		((CListBox *) GetParent())->SetCurSel(i_Sel + 1);
		return;
	}

	if(nChar == VK_UP)
	{
		GetParent()->SetFocus();
		GetParent()->Invalidate();
		i_Sel = ((CListBox *) GetParent())->GetCurSel();
		if(i_Sel) ((CListBox *) GetParent())->SetCurSel(i_Sel - 1);
		return;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_CEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	/*~~~~~~*/
	int i_Sel;
	/*~~~~~~*/

	if(nChar == VK_RETURN)
	{
		GetParent()->SetFocus();
		GetParent()->Invalidate();
		return;
	}

	if(nChar == VK_BACK)
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}

	if(nChar == VK_TAB)
	{
		GetParent()->SetFocus();
		GetParent()->Invalidate();
		i_Sel = ((CListBox *) GetParent())->GetCurSel();
		((CListBox *) GetParent())->SetCurSel(i_Sel + 1);
		return;
	}

	switch(i_Type)
	{
	/* String */
	case 0:
		break;

	/* Int */
	case 1:
		if(!(M_MF()->mst_Ini.b_IntAsHex))
		{
			if(L_isdigit(nChar)) break;
			if((nChar == '-') || (nChar == '+')) break;
			return;
		}

	/* Hexa */
	case 3:
		if(L_isxdigit(nChar)) break;
		if((nChar == '-') || (nChar == '+')) break;
		return;

	/* Float */
	case 2:
		if(L_isdigit(nChar)) break;
		if(nChar == '.') break;
		if((nChar == '-') || (nChar == '+')) break;
		if((nChar == 'e') || (nChar == 'E')) break;
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_CEdit::OnMButtonDown(UINT uID, CPoint pt)
{
	SetFocus();
	if((i_Type == 1) || (i_Type == 2))
	{
		b_Inc = TRUE;
		SetCapture();
		o_Pt = pt;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_CEdit::OnMButtonUp(UINT, CPoint)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	ReleaseCapture();
	if(b_Inc)
	{
		GetParent()->SetFocus();
		GetParent()->Invalidate();
	}

	SetFocus();
	b_Inc = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EVAV_CEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	/*~~~~~~~~~~*/
	BOOL	bSave;
	/*~~~~~~~~~~*/

	bSave = b_Inc;
	b_Inc = TRUE;
	if(zDelta > 0)
		pt.y = o_Pt.y - (i_Type == 1 ? 1 : 10);
	else
		pt.y = o_Pt.y + (i_Type == 1 ? 1 : 10);
	OnMouseMove(nFlags, pt);
	b_Inc = bSave;

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_CEdit::OnMouseMove(UINT n, CPoint	pt)
{
	/*~~~~~~~~~~~~~~~~~*/
	CString	o_Str;
	int		iVal;
	float	fVal, fDelta;
	char	a[50];
	/*~~~~~~~~~~~~~~~~~*/

	#define	FACTOR	10
	if(b_Inc)
	{
		GetWindowText(o_Str);
		switch(i_Type)
		{
		case 1:	/* Int */
			iVal = L_atoi((char	*) (LPCSTR)	o_Str);
			iVal +=	(o_Pt.y	- pt.y);
			sprintf(a, "%d", iVal);
			o_Str =	a;
			SetWindowText(o_Str);
			((EVAV_cl_ListBox *) GetParent())->OnEditChangeSelect();
			((EVAV_cl_ListBox *) GetParent())->Invalidate();
			((EVAV_cl_ListBox *) GetParent())->UpdateWindow();
			break;
		case 2:	/* Float */
			fVal = (float) L_atof((char	*) (LPCSTR)	o_Str);
			fDelta = fVal /	(float)	FACTOR;
			if(fDelta <	.1f) fDelta	= .1f;
			if(o_Pt.y -	pt.y > 0)
				fVal +=	fDelta;
			else
				fVal -=	fDelta;
			sprintf(a, "%f", fVal);
			o_Str =	a;
			SetWindowText(o_Str);
			((EVAV_cl_ListBox *) GetParent())->OnEditChangeSelect();
			((EVAV_cl_ListBox *) GetParent())->Invalidate();
			((EVAV_cl_ListBox *) GetParent())->UpdateWindow();
			break;
		}

		o_Pt = pt;
	}
	else
	{
		CEdit::OnMouseMove(n, pt);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ViewItem *EVAV_cl_ListBox::po_GetParentGAO(EVAV_cl_ViewItem *_po_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Parent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Parent = _po_Data;
	while(1)
	{
		po_Parent = GetItemParent(po_Parent);
		if(po_Parent == NULL) return NULL;

		if((po_Parent->me_Type == EVAV_EVVIT_Pointer) && (po_Parent->mi_Param1 == LINK_C_ENG_GameObjectOriented))
			break;
	}

	return po_Parent;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVAV_cl_ViewItem *EVAV_cl_ListBox::po_GetTypedParent(EVAV_cl_ViewItem *_po_Data, int _i_StructType)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Parent;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Parent = _po_Data;
	while(1)
	{
		po_Parent = GetItemParent(po_Parent);
		if(po_Parent == NULL) return NULL;
		if((po_Parent->me_Type == EVAV_EVVIT_SubStruct || po_Parent->me_Type == EVAV_EVVIT_Pointer) && (po_Parent->mi_Param1 == _i_StructType)) break;
	}

	return po_Parent;
}

#endif /* ACTIVE_EDITORS */
