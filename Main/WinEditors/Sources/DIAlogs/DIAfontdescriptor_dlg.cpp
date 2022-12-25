/*$T DIAfontdescriptor_dlg.cpp GC! 1.081 11/13/01 16:52:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "Res/Res.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGdefs.h"
#include "LINKs/LINKtoed.h"
#include "LINks/LINKmsg.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "EDIpaths.h"

#include "SOFT/SOFTpickingbuffer.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXeditorfct.h"
#include "STRing/STRstruct.h"

#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAfontdescriptor_dlg.h"
#include "DIALOGS/DIAtoolbox_dlg.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview_undo.h"
#include "EDImainframe.h"
#include "EDIapp.h"
#include "EDIeditors_infos.h"
#include "Editors/Sources/OUTput/OUTframe.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

int     sgac_CharTable[256];

/* for picking type */
#define C_Pick_Center		0
#define C_Pick_Left			1
#define C_Pick_UpLeft		2
#define C_Pick_Up			3
#define C_Pick_UpRight		4
#define C_Pick_Right		5
#define C_Pick_DownRight	6
#define C_Pick_Down			7
#define C_Pick_DownLeft		8

/*$off*/

#define M_IfLetterInRect(R, l) \
	if \
	( \
		(l->x[0] >= R.left) \
	&&	(l->x[1] <= R.right) \
	&&	(l->y[1] >= R.top) \
	&&	(l->y[0] <= R.bottom) \
	)
/*$on*/

/*$4
 ***********************************************************************************************************************
    fonctions of CLetterListBox:: subclass of CListBox
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(CLetterListBox, CListBox)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CLetterListBox::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CLetterListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	lpMIS->itemHeight = 12;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CLetterListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i;
	CDC			*pDC;
	char		sz_Text[64], *sz_Cur;
	static int	Tab[3] = { 30, 50, 100 };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDC = CDC::FromHandle(lpDIS->hDC);

	pDC->FillSolidRect(&lpDIS->rcItem, GetSysColor(COLOR_WINDOW));

	i = lpDIS->itemData;
	if (i < 256 )
		sz_Cur = sz_Text + sprintf(sz_Text, "%4d\t(%c)", i, sgac_CharTable[i] );
	else
		sz_Cur = sz_Text + sprintf(sz_Text, "%4d\t ", i );
	if(mpo_FontDlg->mpst_Letter[i].c_Valid)
	{
		sz_Cur += sprintf
			(
				sz_Cur,
				"\t(%3d, %3d)\t(%3d, %3d) %s %s %s %s",
				mpo_FontDlg->mpst_Letter[i].u0,
				mpo_FontDlg->mpst_Letter[i].v0,
				mpo_FontDlg->mpst_Letter[i].u1,
				mpo_FontDlg->mpst_Letter[i].v1, 
				mpo_FontDlg->mpst_Letter[i].c_InvertX ? "-x" : "",
				mpo_FontDlg->mpst_Letter[i].c_InvertY ? "-y" : "",
				mpo_FontDlg->mpst_Letter[i].c_RotateCW? "-cw" : "",
				mpo_FontDlg->mpst_Letter[i].c_RotateCCW? "-ccw" : ""
			);
		pDC->SetTextColor(0);
	}
	else
		pDC->SetTextColor(0xFF);

	pDC->TabbedTextOut(lpDIS->rcItem.left, lpDIS->rcItem.top, sz_Text, -1, 3, Tab, 0);

	if(lpDIS->itemState & ODS_SELECTED) pDC->InvertRect(&lpDIS->rcItem);
};

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

static char *sgasz_PickCursor[9] =
{
	IDC_SIZEALL,
	IDC_SIZEWE,
	IDC_SIZENWSE,
	IDC_SIZENS,
	IDC_SIZENESW,
	IDC_SIZEWE,
	IDC_SIZENWSE,
	IDC_SIZENS,
	IDC_SIZENESW
};
static char sgac_PickMove[9] = { 15, 1, 3, 2, 6, 4, 12, 8, 9 };

/*$4
 ***********************************************************************************************************************
    Fnuctions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::PointNormalize(EDIA_tdst_Letter *L)
{
	/*~~*/
	int i;
	/*~~*/

	if(L->x[0] > L->x[1])
	{
		i = L->x[0];
		L->x[0] = L->x[1];
		L->x[1] = i;
	}

	if(L->y[0] > L->y[1])
	{
		i = L->y[0];
		L->y[0] = L->y[1];
		L->y[1] = i;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::Point2UV(int x, int y, int *u, int *v)
{
	x -= mo_TexRect.left;
	y -= mo_TexRect.top;
	
    x /= mi_Zoom;
    y /= mi_Zoom;

	*u = x + mi_X;
	*v = y + mi_Y;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::UV2Point(int u, int v, int *x, int *y, char c_UpLeft)
{
	u -= mi_X;
	v -= mi_Y;

	u *= mi_Zoom;
    v *= mi_Zoom;

	*x = u + mo_TexRect.left;
	*y = v + mo_TexRect.top;

	if( c_UpLeft )
	{
		*x += mi_Zoom - 1;
		*y += mi_Zoom - 1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog_Update(ULONG ul_Dlg)
{
	((EDIA_cl_FontDescriptorDialog *) ul_Dlg)->UpdateUV();
}

/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    Message map / init
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_FontDescriptorDialog, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_CHECK_ALPHA, OnRedraw)
	ON_BN_CLICKED(IDC_BUTTON_SHOWALL, OnShowAll)
	ON_BN_CLICKED(IDC_BUTTON_ZOOMALL, OnZoomAll)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_CHECK_AUTOREFRESH, OnAutoRefresh)
    ON_BN_CLICKED(IDC_BUTTON_COPYUPPERTOLOWER, OnButton_CopyUpperToLower )
    ON_BN_CLICKED(IDC_BUTTON_COPYLOWERTOUPPER, OnButton_CopyLowerToUpper )
	ON_BN_CLICKED(IDC_BUTTON_GENDESC, OnButton_GenerateDesc )
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButton_ApplyCharRange )
	ON_EN_CHANGE(IDC_EDIT_CHARRANGE, OnEditChange_CharRange )
	ON_LBN_SELCHANGE(IDC_LIST_LETTER, UpdateShowStatus)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_FontDescriptorDialog::EDIA_cl_FontDescriptorDialog(ULONG ul_Texture) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_FONTE)
{
    int j;

	mul_TextureIndex = ul_Texture;

	mh_BitmapRGB = NULL;
	mh_BitmapA = NULL;

	mi_Capture = 0;
	mi_Pickable = -1;
	mi_CurrentLetter = -1;

	mi_ZoomAllWithOrigin = 0;
	mb_AutoRefresh = FALSE;

	mi_Zoom = 1;
	mi_X = mi_Y = 0;
	
	mi_LetterNb = 0;
	mpst_Letter = NULL;
	
	mb_Copy = FALSE;

    /* Convert ascii table. Thanks bill */
	for(j = 0; j < 256; j++)
		sgac_CharTable[j] = j;
	sgac_CharTable[168] = 161;
	sgac_CharTable[133] = 224;
	sgac_CharTable[160] = 225;
	sgac_CharTable[131] = 226;
	sgac_CharTable[132] = 228;
	sgac_CharTable[135] = 231;
	sgac_CharTable[138] = 232;
	sgac_CharTable[130] = 233;
	sgac_CharTable[136] = 234;
	sgac_CharTable[137] = 235;
	sgac_CharTable[141] = 236;
	sgac_CharTable[161] = 237;
	sgac_CharTable[140] = 238;
	sgac_CharTable[139] = 239;
	sgac_CharTable[164] = 241;
	sgac_CharTable[149] = 242;
	sgac_CharTable[162] = 243;
	sgac_CharTable[147] = 244;
	sgac_CharTable[167] = 245;
	sgac_CharTable[148] = 246;
	sgac_CharTable[151] = 249;
	sgac_CharTable[163] = 250;
	sgac_CharTable[150] = 251;
	sgac_CharTable[129] = 252;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_FontDescriptorDialog::~EDIA_cl_FontDescriptorDialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_FontDescriptorDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	//int		type;
	char	sz_Name[BIG_C_MaxLenName], *sz_Ext;
	ULONG	ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_STATIC_TEXTURE)->ShowWindow(SW_HIDE);
	UpdateTexture();
	GetDlgItem(IDC_CHECK_ALPHA)->ShowWindow(mh_BitmapA ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_STATIC_TEXTURE)->GetWindowRect(&mo_TexRect);
	ScreenToClient(&mo_TexRect);

	mo_LB.SubclassDlgItem(IDC_LIST_LETTER, this);
	mo_LB.mpo_FontDlg = this;


    LoadLetterDesc();
	UpdateLetterList();
	UpdateShowStatus();

	if(mpst_Font)
	{
		ul_Index = BIG_ul_SearchKeyToFat(mpst_Font->ul_Key);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			L_strcpy(sz_Name, BIG_NameFile(ul_Index));
			sz_Ext = L_strrchr(sz_Name, '.');
			if(sz_Ext)
			{
				L_strcpy(sz_Ext, ".fod");
				GetDlgItem(IDC_EDIT_NAME)->SetWindowText(sz_Name);
			}
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_FontDescriptorDialog::PreTranslateMessage(MSG *pMsg)
{
	/*~~~~~~~~~~~~~~~~*/
	int		i;
	static	int uvgizmo = 0;
	RECT	st_Rect;
	/*~~~~~~~~~~~~~~~~*/

	if(pMsg->message == WM_MOUSEWHEEL)
	{
		if(WheelZoom((short) HIWORD(pMsg->wParam))) return 1;
	}
	else if(pMsg->message == WM_KEYDOWN)
	{
		if((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_SPACE)) return 1;

		/* undo */
		if((pMsg->wParam == 'Z') && (GetAsyncKeyState(VK_SHIFT) < 0))
		{
			return 1;
		}

		/* redo */
		if((pMsg->wParam == 'Y') && (GetAsyncKeyState(VK_SHIFT) < 0))
		{
			return 1;
		}

		/* select all */
		if((pMsg->wParam == 'A') && (GetAsyncKeyState(VK_SHIFT) < 0))
		{
			for(i = 0; i < mi_LetterNb; i++)
				if(mpst_Letter[i].c_Show) mpst_Letter[i].c_Sel = 1;
			InvalidateRect(NULL, FALSE);
			return 1;
		}

		if(pMsg->wParam == VK_DELETE)
		{
			LetterDelete();
			return 1;
		}

		/* modif with mouse */
		if ( (pMsg->wParam == VK_UP) || (pMsg->wParam == VK_DOWN) || (pMsg->wParam == VK_LEFT) || (pMsg->wParam == VK_RIGHT) )
		{
			for(i = 0; i < mi_LetterNb; i++)
			{
				if(mpst_Letter[i].c_Sel || (GetAsyncKeyState(VK_CONTROL) && mpst_Letter[i].c_Show) )
				{
					if ( GetAsyncKeyState(VK_SHIFT) )
					{
						if ( pMsg->wParam == VK_UP )
						{
							mpst_Letter[i].v1 -= 1;
							if(mpst_Letter[i].v1 < mpst_Letter[i].v0) mpst_Letter[i].v1 = mpst_Letter[i].v0;
						}
						else if ( pMsg->wParam == VK_DOWN )
						{
							mpst_Letter[i].v1 += 1;
						}
						else if ( pMsg->wParam == VK_LEFT )
						{
							mpst_Letter[i].u1 -= 1;
							if(mpst_Letter[i].u1 < mpst_Letter[i].u0) mpst_Letter[i].u1 = mpst_Letter[i].u0;
						}
						else if ( pMsg->wParam == VK_RIGHT )
							mpst_Letter[i].u1 += 1;
					}
					else
					{
						if ( pMsg->wParam == VK_UP )
						{
							mpst_Letter[i].v0 -= 1;
							mpst_Letter[i].v1 -= 1;
						}
						else if ( pMsg->wParam == VK_DOWN )
						{
							mpst_Letter[i].v0 += 1;
							mpst_Letter[i].v1 += 1;
						}
						else if ( pMsg->wParam == VK_RIGHT)
						{
							mpst_Letter[i].u0 += 1;
							mpst_Letter[i].u1 += 1;
						}
						else if ( pMsg->wParam == VK_LEFT )
						{
							mpst_Letter[i].u0 -= 1;
							mpst_Letter[i].u1 -= 1;
						}
					}
					mo_LB.GetItemRect(i - 32, &st_Rect);
					mo_LB.InvalidateRect(&st_Rect, FALSE);
				}
			}

			InvalidateRect(NULL, FALSE);
			if(mb_AutoRefresh) OnRefresh();
			return 1;
		}

		return 0;
	}
	else if (pMsg->message == WM_LBUTTONDBLCLK )
	{
		if (pMsg->hwnd == GetDlgItem(IDC_LIST_LETTER )->GetSafeHwnd() )
		{
			CListBox		*po_LB;
			CPoint			o_Pt( pMsg->lParam );
			int				b_Outside;
			int				x, y;

			po_LB = (CListBox *) GetDlgItem( IDC_LIST_LETTER );
			i = po_LB->ItemFromPoint( o_Pt, b_Outside );
			if ( (i!= -1) && (!b_Outside) )
			{
				po_LB->SetSel( i );
				i = po_LB->GetItemData( i );

				x = (mpst_Letter[ i ].u0 + mpst_Letter[ i ].u1) / 2;
				y = (mpst_Letter[ i ].v0 + mpst_Letter[ i ].v1) / 2;

				mi_X = x - ((mo_TexRect.Width() / mi_Zoom) / 2);
				mi_Y = y - ((mo_TexRect.Height() / mi_Zoom) / 2);
				
				if (mi_X < 0) 
					mi_X = 0;
				else if (mi_X + (mo_TexRect.Width() / mi_Zoom) > mi_BmpWidth )
					mi_X = mi_BmpWidth - (mo_TexRect.Width() / mi_Zoom);
				if (mi_Y < 0) 
					mi_Y = 0;
				else if (mi_Y + (mo_TexRect.Height() / mi_Zoom) > mi_BmpHeight )
					mi_Y = mi_BmpHeight - (mo_TexRect.Height() / mi_Zoom);
			}
			InvalidateRect(NULL, FALSE);
			if(mb_AutoRefresh) OnRefresh();
			return 1;
		}
	}
	else if (pMsg->message == WM_RBUTTONDOWN )
	{
		if (pMsg->hwnd == GetDlgItem(IDC_LIST_LETTER )->GetSafeHwnd() )
		{
			EMEN_cl_SubMenu o_Menu(FALSE);
			CListBox		*po_LB;
			CPoint			o_Pt( pMsg->lParam );
			BOOL			b_Outside;
			int				res;

			po_LB = (CListBox *) GetDlgItem( IDC_LIST_LETTER );
			i = po_LB->ItemFromPoint( o_Pt, b_Outside );
			if ( (i!= -1) && (!b_Outside) )
			{
				i = po_LB->GetItemData( i );

		        M_MF()->InitPopupMenuAction(NULL, &o_Menu);
		        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Copy", -1);
				if (mb_Copy) M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Paste", -1);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Invert X", -1);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Invert Y", -1);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 5, TRUE, "Rotate CW", -1);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 6, TRUE, "Rotate CCW", -1);
				po_LB->ClientToScreen(&o_Pt);
				res = M_MF()->TrackPopupMenuAction(NULL, o_Pt, &o_Menu);
				switch( res )
				{
				case 1: // copy
					mb_Copy = TRUE;
					L_memcpy( &mst_CopyLetter, &mpst_Letter[i], sizeof( EDIA_tdst_Letter ) );
					break;
				case 2: // paste
					L_memcpy( &mpst_Letter[i], &mst_CopyLetter, sizeof( EDIA_tdst_Letter ) );
					mo_LB.GetItemRect(i - 32, &st_Rect);
					mo_LB.InvalidateRect(&st_Rect, FALSE);
					break;
				case 3: 
					mpst_Letter[ i ].c_InvertX = 1 - mpst_Letter[ i ].c_InvertX;
					UpdateLetterList();
					break;
				case 4:
					mpst_Letter[ i ].c_InvertY = 1 - mpst_Letter[ i ].c_InvertY;
					UpdateLetterList();
					break;
				case 5:
					mpst_Letter[ i ].c_RotateCCW = 0;
					mpst_Letter[ i ].c_RotateCW = 1 - mpst_Letter[ i ].c_RotateCW;
					UpdateLetterList();
					break;
				case 6:
					mpst_Letter[ i ].c_RotateCW = 0;
					mpst_Letter[ i ].c_RotateCCW = 1 - mpst_Letter[ i ].c_RotateCCW;
					UpdateLetterList();
					break;
				}
			}
		}
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    messages handles
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnSize(UINT n, int x, int y)
{
	/*~~~~~~~~~~~~~*/
	CWnd	*po_List;
	RECT	st_Rect;
	/*~~~~~~~~~~~~~*/

	po_List = GetDlgItem(IDC_LIST_LETTER);
	if(po_List)
	{
		po_List->GetWindowRect(&st_Rect);
		ScreenToClient(&st_Rect);
		st_Rect.bottom = (y - 5);
		po_List->MoveWindow(&st_Rect);
	}

	mo_TexRect.right = x - 5;
	mo_TexRect.bottom = y - 5;

	EDIA_cl_BaseDialog::OnSize(n, x, y);

	st_Rect = *(RECT *) &mo_TexRect;
	st_Rect.top = mo_TexRect.bottom;
	st_Rect.bottom += 5;
	st_Rect.right += 5;
	InvalidateRect(&st_Rect);

	st_Rect = *(RECT *) &mo_TexRect;
	st_Rect.left = st_Rect.right;
	st_Rect.right += 5;
	InvalidateRect(&st_Rect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 550;
	lpMMI->ptMinTrackSize.y = 300;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC					*pDC, oDC;
	CRgn				o_Region;
	CPen				o_Dot, *po_OldPen;
	HBITMAP				h_Bmp;
	int					w, h, dx, dy;
	int					i, x0, y0, x1, y1;
	int					xmin, ymin;
	EDIA_tdst_Letter	*l;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnPaint();

	pDC = GetDC();
	po_OldPen = NULL;

	o_Dot.CreatePen(PS_DOT, 1, (COLORREF) 0);
	po_OldPen = pDC->SelectObject(&o_Dot);

	o_Region.CreateRectRgn(mo_TexRect.left, mo_TexRect.top + 1, mo_TexRect.right, mo_TexRect.bottom + 1);
	pDC->SelectClipRgn(&o_Region);

	/* display texture */
	if((((CButton *) GetDlgItem(IDC_CHECK_ALPHA))->GetCheck()) && mh_BitmapA)
		h_Bmp = mh_BitmapA;
	else
		h_Bmp = mh_BitmapRGB;

	if(h_Bmp)
	{
		xmin = mi_X;
		ymin = mi_Y;
		w = mi_BmpWidth - xmin;
		h = mi_BmpHeight - ymin;

		dx = mo_TexRect.Width();
		dy = mo_TexRect.Height();

		w *= mi_Zoom;
		h *= mi_Zoom;
		if(w > dx) w = dx;
		if(h > dy) h = dy;
		w /= mi_Zoom;
		dx = w * mi_Zoom;
		h /= mi_Zoom;
		dy = h * mi_Zoom;
		
		x0 = mo_TexRect.left;
		y0 = mo_TexRect.top;

		oDC.CreateCompatibleDC(pDC);
		oDC.SelectObject(h_Bmp);
		pDC->SetStretchBltMode(COLORONCOLOR);
		pDC->StretchBlt(x0, y0, dx, dy, &oDC, xmin, ymin, w, h, SRCCOPY);

		x1 = x0 + dx;
		if(x1 < mo_TexRect.right) pDC->FillSolidRect(x1, mo_TexRect.top, mo_TexRect.right, mo_TexRect.bottom, 0);
		y1 = y0 + dy;
		if(y1 < mo_TexRect.bottom) pDC->FillSolidRect(mo_TexRect.left, y1, mo_TexRect.right, mo_TexRect.bottom, 0);
	}

	/* display rect point */
	pDC->SetBkColor(0xFFFFFF);
	pDC->SelectObject(&o_Dot);

	l = mpst_Letter + 32;
	for(i = 32; i < mi_LetterNb; i++, l++)
	{
		l->c_Visible = 0;
		if((l->c_Show) && (l->c_Valid))
		{
			UV2Point(l->u0, l->v0, &l->x[0], &l->y[0], 0);
			UV2Point(l->u1, l->v1, &l->x[1], &l->y[1], 1);

			pDC->SetBkColor(l->c_Sel ? 0xFF : 0xFFFFFF);

			if
			(
				(l->x[1] < mo_TexRect.left)
			||	(l->x[0] > mo_TexRect.right)
			||	(l->y[1] < mo_TexRect.top)
			||	(l->y[0] > mo_TexRect.bottom)
			) continue;

			l->c_Visible = 1;
			pDC->MoveTo(l->x[0], l->y[0]);
			pDC->LineTo(l->x[0], l->y[1]);
			pDC->LineTo(l->x[1], l->y[1]);
			pDC->LineTo(l->x[1], l->y[0]);
			pDC->LineTo(l->x[0], l->y[0]);
		}
	}

	pDC->SelectObject(po_OldPen);
	pDC->SelectClipRgn(NULL);
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnOK(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Name[BIG_C_MaxLenName];
	char	asz_Path[BIG_C_MaxLenPath];
	ULONG	ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	WriteLetterDesc();

	ul_Index = BIG_ul_SearchKeyToFat(mpst_Font->ul_Key);
	if(ul_Index == BIG_C_InvalidIndex) return;

	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	GetDlgItem(IDC_EDIT_NAME)->GetWindowText(asz_Name, BIG_C_MaxLenName);
	STR_ul_SaveFontDescriptor(mpst_Font, asz_Path, asz_Name );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnLButtonDown(UINT n_Flags, CPoint o_Pt)
{
	if(mi_Capture) return;
	if(!mo_TexRect.PtInRect(o_Pt)) return;

	if(GetAsyncKeyState(VK_SPACE) < 0)
	{
		mi_Capture = 1;
	}
	else
	{
		if(mi_Pickable != -1)
		{
			mi_Capture = 5;
		}
		else if((mi_CurrentLetter != -1) && (!mpst_Letter[mi_CurrentLetter].c_Valid))
		{
			mi_Capture = 4;
			mo_SelRect.TopLeft() = o_Pt;
			mo_SelRect.BottomRight() = o_Pt;
			SelRect_Draw();
		}
		else
			return;
	}

	mo_CapturePt = o_Pt;
	SetCapture();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnLButtonUp(UINT n_Flags, CPoint o_Pt)
{
	/*~~~~~~~~~~~~*/
	RECT	st_Rect;
	/*~~~~~~~~~~~~*/

	if((mi_Capture == 0) || (mi_Capture == 3)) return;

	if(mi_Capture == 5)
	{
		if(mi_Pickable != -1)
		{
			if(n_Flags & MK_CONTROL)
			{
				if(n_Flags & MK_SHIFT)
					mpst_Letter[mi_Pickable].c_Sel = 1 - mpst_Letter[mi_Pickable].c_Sel;
				else
					mpst_Letter[mi_Pickable].c_Sel = 1;
			}
			else if(n_Flags & MK_SHIFT)
			{
				mpst_Letter[mi_Pickable].c_Sel = 0;
			}
			else
			{
				LetterSelNone();
				mpst_Letter[mi_Pickable].c_Sel = 1;
			}

			LINK_Refresh();
		}
	}
	else if(mi_Capture == 4)
	{
		SelRect_Draw();

		mo_SelRect.NormalizeRect();

		mpst_Letter[mi_CurrentLetter].x[0] = mst_SelLetter.x[0];
		mpst_Letter[mi_CurrentLetter].x[1] = mst_SelLetter.x[1];
		mpst_Letter[mi_CurrentLetter].y[0] = mst_SelLetter.y[0];
		mpst_Letter[mi_CurrentLetter].y[1] = mst_SelLetter.y[1];
		mpst_Letter[mi_CurrentLetter].u0 = mst_SelLetter.u0;
		mpst_Letter[mi_CurrentLetter].u1 = mst_SelLetter.u1;
		mpst_Letter[mi_CurrentLetter].v0 = mst_SelLetter.v0;
		mpst_Letter[mi_CurrentLetter].v1 = mst_SelLetter.v1;
		mpst_Letter[mi_CurrentLetter].c_Valid = 1;
		mpst_Letter[mi_CurrentLetter].c_InvertX = 0;
		mpst_Letter[mi_CurrentLetter].c_InvertY = 0;
		mpst_Letter[mi_CurrentLetter].c_RotateCW = 0;
		mpst_Letter[mi_CurrentLetter].c_RotateCCW = 0;
		mo_LB.GetItemRect(mi_CurrentLetter - 32, &st_Rect);
		mo_LB.InvalidateRect(&st_Rect, FALSE);
	}

	ReleaseCapture();
	mi_Capture = 0;
	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnLButtonDblClk(UINT n_Flags, CPoint o_Pt)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnRButtonDown(UINT n_Flags, CPoint o_Pt)
{
	if(mi_Capture)
	{
		if((mi_Capture == 3) || (mi_Capture == 4)) SelRect_Draw();

		ReleaseCapture();
		mi_Capture = 0;

		InvalidateRect(NULL, FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnMButtonDown(UINT n_Flags, CPoint o_Pt)
{
	if(mi_Capture) return;
	if(!mo_TexRect.PtInRect(o_Pt)) return;

	mi_Capture = 3;
	mo_SelRect.TopLeft() = o_Pt;
	mo_SelRect.BottomRight() = o_Pt;
	SelRect_Draw();

	mo_CapturePt = o_Pt;
	SetCapture();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnMButtonUp(UINT n_Flags, CPoint o_Pt)
{
	if(mi_Capture == 3)
	{
		SelRect_Draw();

		if(!(n_Flags & MK_CONTROL) && !(n_Flags & MK_SHIFT)) LetterSelNone();
		SelRect_Sel();
		ReleaseCapture();
		mi_Capture = 0;
		InvalidateRect(NULL, FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnMouseMove(UINT n_Flags, CPoint o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, x, y, DX, DY;
	EDIA_tdst_Letter	*l;
	RECT				st_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Capture == 5)
	{
		x = (o_Pt.x - mo_CapturePt.x);
		y = (o_Pt.y - mo_CapturePt.y);
		if((x * x) + (y * y) > 9)
		{
			if(!mpst_Letter[mi_Pickable].c_Sel)
			{
				if(!(n_Flags & MK_CONTROL) && !(n_Flags & MK_SHIFT)) LetterSelNone();
				mpst_Letter[mi_Pickable].c_Sel = 1;
			}

			mo_CapturePt = o_Pt;
			mi_Capture = 2;
		}
		else
		{
			::SetCursor(AfxGetApp()->LoadStandardCursor((mi_Pickable == -1) ? IDC_ARROW : sgasz_PickCursor[mi_PickType]));
			return;
		}
	}

	mi_Pickable = -1;

	if(mi_Capture == 1)
	{
		DX = o_Pt.x - mo_CapturePt.x;
		if( (DX >= mi_Zoom) || (-DX >= mi_Zoom) )
		{
			DX /= mi_Zoom;
			x = (mi_BmpWidth - mi_X + DX) * mi_Zoom;
			if(x < mo_TexRect.Width())
			    mi_X = mi_BmpWidth - (mo_TexRect.Width() / mi_Zoom);
			else
			    mi_X -= DX;
			if(mi_X < 0) mi_X = 0;
			mo_CapturePt.x = o_Pt.x;
		}

		DY = o_Pt.y - mo_CapturePt.y;
		if( (DY >= mi_Zoom) || (-DY >= mi_Zoom) )
		{
			DY /= mi_Zoom;
			y = (mi_BmpHeight - mi_Y + DY) * mi_Zoom;
			if(y < mo_TexRect.Height())
			    mi_Y = mi_BmpHeight - (mo_TexRect.Height() / mi_Zoom);
			else
				mi_Y -= DY;
			if(mi_Y < 0) mi_Y = 0;
			mo_CapturePt.y = o_Pt.y;
		}

		InvalidateRect(NULL, FALSE);
	}
	else if(mi_Capture == 2)
	{
		x = o_Pt.x - mo_CapturePt.x;
		y = o_Pt.y - mo_CapturePt.y;
		
        x /= mi_Zoom;
		y /= mi_Zoom;
		mo_CapturePt.x += (x * mi_Zoom );
		mo_CapturePt.y += (y * mi_Zoom );
		

		for(i = 0; i < mi_LetterNb; i++)
		{
			if(mpst_Letter[i].c_Sel)
			{
				if(mi_PickType == C_Pick_Center)
				{
					mpst_Letter[i].u0 += x;
					mpst_Letter[i].u1 += x;
					mpst_Letter[i].v0 += y;
					mpst_Letter[i].v1 += y;
				}
				else
				{
					if(sgac_PickMove[mi_PickType] & 1)
					{
						mpst_Letter[i].u0 += x;
						if(mpst_Letter[i].u0 > mpst_Letter[i].u1) mpst_Letter[i].u0 = mpst_Letter[i].u1;
					}

					if(sgac_PickMove[mi_PickType] & 4)
					{
						mpst_Letter[i].u1 += x;
						if(mpst_Letter[i].u0 > mpst_Letter[i].u1) mpst_Letter[i].u1 = mpst_Letter[i].u0;
					}

					if(sgac_PickMove[mi_PickType] & 2)
					{
						mpst_Letter[i].v0 += y;
						if(mpst_Letter[i].v0 > mpst_Letter[i].v1) mpst_Letter[i].v0 = mpst_Letter[i].v1;
					}

					if(sgac_PickMove[mi_PickType] & 8)
					{
						mpst_Letter[i].v1 += y;
						if(mpst_Letter[i].v0 > mpst_Letter[i].v1) mpst_Letter[i].v1 = mpst_Letter[i].v0;
					}
				}

				mo_LB.GetItemRect(i - 32, &st_Rect);
				mo_LB.InvalidateRect(&st_Rect, FALSE);
			}
		}

		InvalidateRect(NULL, FALSE);
		if(mb_AutoRefresh) OnRefresh();
	}
	else if((mi_Capture == 3) || (mi_Capture == 4))
	{
		SelRect_Draw();
		mo_SelRect.BottomRight() = o_Pt;
		SelRect_Draw();
	}
	else
	{
		if(mo_TexRect.PtInRect(o_Pt))
		{
			l = mpst_Letter + 32;
			for(i = 32; i < mi_LetterNb; i++, l++)
			{
				if(l->c_Visible)
				{
					if((o_Pt.x >= l->x[0]) && (o_Pt.x <= l->x[1]) && (o_Pt.y >= l->y[0]) && (o_Pt.y <= l->y[1]))
					{
						if(o_Pt.x < l->x[0] + 3)
						{
							if(o_Pt.y < (l->y[0] + 3))
								mi_PickType = 2;
							else if(o_Pt.y > (l->y[1] - 3))
								mi_PickType = 8;
							else
								mi_PickType = 1;
						}
						else if(o_Pt.x > l->x[1] - 3)
						{
							if(o_Pt.y < (l->y[0] + 3))
								mi_PickType = 4;
							else if(o_Pt.y > (l->y[1] - 3))
								mi_PickType = 6;
							else
								mi_PickType = 5;
						}
						else
						{
							if(o_Pt.y < (l->y[0] + 3))
								mi_PickType = 3;
							else if(o_Pt.y > (l->y[1] - 3))
								mi_PickType = 7;
							else
								mi_PickType = 0;
						}

						mi_Pickable = i;
						break;
					}
				}
			}

			::SetCursor(AfxGetApp()->LoadStandardCursor((mi_Pickable == -1) ? IDC_ARROW : sgasz_PickCursor[mi_PickType]));
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnRedraw(void)
{
	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnShowAll(void)
{
    mi_X = 0;
    mi_Y = 0;

	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnZoomAll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL	b_Zoom;
	float	x, xmin, xmax, y, ymin, ymax;
	int		i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	xmin = ymin = 10000;
	xmax = ymax = -10000;

	b_Zoom = FALSE;

	for(i = 32; i < mi_LetterNb; i++)
	{
		if(mpst_Letter[i].c_Visible)
		{
			b_Zoom = TRUE;
		}
	}

	if(b_Zoom)
	{
		if(xmin == xmax)
		{
			xmin -= 0.5f;
			xmax += 0.5f;
		}

		if(ymin == ymax)
		{
			ymin -= 0.5f;
			ymax += 0.5f;
		}

		x = 0.05f * (xmax - xmin);
		xmin -= x;
		xmax += x;

		y = 0.05f * (ymax - ymin);
		ymin -= y;
		ymax += y;

		InvalidateRect(NULL, FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnRefresh(void)
{
	WriteLetterDesc();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnAutoRefresh(void)
{
	mb_AutoRefresh = ((CButton *) GetDlgItem(IDC_CHECK_AUTOREFRESH))->GetCheck() ? TRUE : FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnButton_ApplyCharRange( void )
{
	char	sz_Value[ 1024 ], *psz;
	int		i_Value, i_OK;

	GetDlgItem( IDC_EDIT_CHARRANGE )->GetWindowText( sz_Value, 10 );
	i_Value = atoi( sz_Value );

	if ( i_Value <= 40) return;
	if ( i_Value > 4000 ) return;
	
	if ( i_Value == mi_LetterNb ) return;

	i_OK = 1;
	if ( i_Value < mi_LetterNb )
	{
		psz = sz_Value + sprintf( sz_Value, "!WARNING! you want less character than actual number\n" );
		psz += sprintf( psz, "          if you've already define character above %d, you will lost them\n", i_Value );
		sprintf( psz, "          Change number of char anyway ?" );
		i_OK = MessageBox( sz_Value, "Font warning", MB_ICONQUESTION | MB_YESNO ) == IDYES ? 1 : 0;
	}

	if ( i_OK )
	{
		mpst_Letter = (EDIA_tdst_Letter *) L_realloc( mpst_Letter, i_Value * sizeof( EDIA_tdst_Letter ) );
		if (i_Value > mi_LetterNb )
			L_memset( mpst_Letter + mi_LetterNb, 0, (i_Value - mi_LetterNb) * sizeof( EDIA_tdst_Letter ) );
		mi_LetterNb = i_Value;
		LetterSelNone();
		UpdateLetterList();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnEditChange_CharRange( void )
{
	char	sz_Value[ 16 ];
	int		i_Value;
	
	GetDlgItem( IDC_EDIT_CHARRANGE )->GetWindowText( sz_Value, 10 );
	i_Value = atoi( sz_Value );
	if ( (i_Value > 40) && (i_Value <= 4000) && ( i_Value != mi_LetterNb ) )
		GetDlgItem( IDC_BUTTON_APPLY )->EnableWindow( TRUE );
	else
		GetDlgItem( IDC_BUTTON_APPLY )->EnableWindow( FALSE );
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnButton_CopyUpperToLower(void)
{
    int i, j;

    for (j = 'A', i = 'a'; i <= 'z'; i++, j++)
    {
        mpst_Letter[i].c_Valid = mpst_Letter[j].c_Valid;
        mpst_Letter[i].u0 = mpst_Letter[j].u0;
        mpst_Letter[i].u1 = mpst_Letter[j].u1;
        mpst_Letter[i].v0 = mpst_Letter[j].v0;
        mpst_Letter[i].v1 = mpst_Letter[j].v1;
    }
    mo_LB.RedrawWindow();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnButton_CopyLowerToUpper(void)
{
    int i, j;

    for (j = 'a', i = 'A'; i <= 'Z'; i++, j++)
    {
        mpst_Letter[i].c_Valid = mpst_Letter[j].c_Valid;
        mpst_Letter[i].u0 = mpst_Letter[j].u0;
        mpst_Letter[i].u1 = mpst_Letter[j].u1;
        mpst_Letter[i].v0 = mpst_Letter[j].v0;
        mpst_Letter[i].v1 = mpst_Letter[j].v1;
    }
    mo_LB.RedrawWindow();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::OnButton_GenerateDesc( void )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File( "Choose folder where desc will be created", 2, FALSE);
	int					i, w, h, walign, dw, dh, linepitch, x;
	char				*p_Raw, *p_Data, *p_Data2, *p_CurData, *p_Src, c_Save;
	BITMAPINFO			st_Bmp;
	BITMAPFILEHEADER    st_BmpHeader;
	char				sz_Path[ 260 ], sz_Name[ 260 ], sz_TexName[ 260 ];
	FILE				*hp_File;
	BOOL				b_Invert;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	if(o_File.DoModal() != IDOK) 
		return;
		
	b_Invert = (M_MF()->MessageBox( "Invert Color ?", "Invert color ?", MB_ICONQUESTION | MB_YESNO ) == IDYES);

	L_strcpy( sz_TexName, BIG_NameFile( mul_TextureIndex ) );
	if (strrchr( sz_TexName, '.' ) )
		*strrchr( sz_TexName, '.' ) = 0;
	
	L_strcpy( sz_Path, o_File.masz_FullPath );
	L_strcat( sz_Path, "/" );
	L_strcat( sz_Path, sz_TexName );

	if ( !SetCurrentDirectory( sz_Path ) )
	{
		if ( !CreateDirectory( sz_Path, NULL ) )
		{
			ERR_X_Warning( 0, "can't create export folder", NULL );
			return;
		}
	}

	/* création des bitmaps correspondants aux caractères */
	p_Raw = (char *) L_malloc( mi_BmpWidth * mi_BmpHeight * 4 );
	L_memset( &st_Bmp, 0, sizeof( BITMAPINFO ) );
	st_Bmp.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	st_Bmp.bmiHeader.biWidth = mi_BmpWidth;
	st_Bmp.bmiHeader.biHeight = mi_BmpHeight;
	st_Bmp.bmiHeader.biPlanes = 1;
	st_Bmp.bmiHeader.biBitCount = 24;
	st_Bmp.bmiHeader.biCompression = BI_RGB;
	i = GetDIBits( GetDC()->GetSafeHdc(), mh_BitmapRGB, 0, mi_BmpHeight, p_Raw, &st_Bmp, DIB_RGB_COLORS );

	for(i = 32; i < mi_LetterNb; i++)
	{
		if(mpst_Letter[i].c_Valid)
		{
			w = mpst_Letter[ i ].u1 - mpst_Letter[ i ].u0 + 1;
			h = mpst_Letter[ i ].v1 - mpst_Letter[ i ].v0 + 1;
			walign = (w * 3) + (((w * 3) & 3) ? (4 - ((w * 3) & 3)) : 0);
			p_Data = (char *) L_malloc( walign * h );
			p_CurData = p_Data; 
			p_Src = p_Raw + ( (((mi_BmpHeight - mpst_Letter[ i ].v1 - 1) * mi_BmpWidth) + mpst_Letter[ i ].u0) * 3 );
			linepitch = (mi_BmpWidth - w) * 3;
			if (b_Invert)
			{
				for (dh = 0; dh < h; dh++)
				{
					for (dw = 0; dw < w; dw++)
					{
						*p_CurData++ = ~*p_Src++;
						*p_CurData++ = ~*p_Src++;
						*p_CurData++ = ~*p_Src++;
					}
					dw *= 3;
					while ( dw < walign ) 
					{
						*p_CurData++ = (char) 255;
						dw++;
					}
					p_Src += linepitch;
				}
			}
			else
			{
				for (dh = 0; dh < h; dh++)
				{
					for (dw = 0; dw < w; dw++)
					{
						*p_CurData++ = *p_Src++;
						*p_CurData++ = *p_Src++;
						*p_CurData++ = *p_Src++;
					}
					dw *= 3;
					while ( dw < walign ) 
					{
						*p_CurData++ = 0;
						dw++;
					}
					p_Src += linepitch;
				}
			}

			if (mpst_Letter[i].c_InvertX)
			{
				for (dh = 0; dh < h; dh++)
				{
					p_CurData = p_Data + (dh * walign);
					p_Src = p_CurData + (w * 3) - 3;
					for (dw = 0; dw < (w>>1); dw++)
					{
						c_Save = *p_Src;
						*p_Src++ = *p_CurData;
						*p_CurData++ = c_Save;
						c_Save = *p_Src;
						*p_Src++ = *p_CurData;
						*p_CurData++ = c_Save;
						c_Save = *p_Src;
						*p_Src++ = *p_CurData;
						*p_CurData++ = c_Save;
						p_Src -= 6;
					}
				}
			}

			if (mpst_Letter[i].c_InvertY)
			{
				for (dh = 0; dh < (h>>1); dh++)
				{
					p_CurData = p_Data + (dh * walign);
					p_Src = p_Data + ((h - dh - 1) * walign);
					for (dw = 0; dw < w * 3; dw++)
					{
						c_Save = *p_Src;
						*p_Src++ = *p_CurData;
						*p_CurData++ = c_Save;
					}
				}
			}

			if (mpst_Letter[i].c_RotateCCW && (w == h) )
			{
				p_Data2 = p_Data;
				p_Data = (char *) L_malloc( walign * h );
				for( dh = 0; dh < h; dh ++)
				{
					p_Src = p_Data2 + (dh * walign);
					x = (h - 1 - dh) * 3;
					for (dw = 0; dw < w; dw ++)
					{
						p_CurData = p_Data + (dw * walign) + x;
						*p_CurData++ = *p_Src++;
						*p_CurData++ = *p_Src++;
						*p_CurData++ = *p_Src++;
					}
				}
				L_free( p_Data2 );
			}
			if (mpst_Letter[i].c_RotateCW && (w == h) )
			{
				p_Data2 = p_Data;
				p_Data = (char *) L_malloc( walign * h );
				for( dh = 0; dh < h; dh ++)
				{
					p_Src = p_Data2 + (dh * walign);
					for (dw = 0; dw < w; dw ++)
					{
						p_CurData = p_Data + ((w - 1 - dw) * walign) + (dh * 3);
						*p_CurData++ = *p_Src++;
						*p_CurData++ = *p_Src++;
						*p_CurData++ = *p_Src++;
					}
				}
				L_free( p_Data2 );
			}

			sprintf( sz_Name, "%s/%s_Letter_%d.bmp", sz_Path, sz_TexName, i );
			L_memset( &st_BmpHeader, 0, sizeof( BITMAPFILEHEADER ) );
			st_BmpHeader.bfType = ((WORD) ('M' << 8) | 'B');
			st_BmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof( BITMAPINFOHEADER );
			st_BmpHeader.bfSize = st_BmpHeader.bfOffBits + (walign * h); 

			st_Bmp.bmiHeader.biWidth = w;
			st_Bmp.bmiHeader.biHeight = h;

			hp_File = L_fopen( sz_Name, "wb" );
			if (hp_File )
			{
				L_fwrite( &st_BmpHeader, 1, sizeof( BITMAPFILEHEADER ), hp_File );
				L_fwrite( &st_Bmp.bmiHeader, 1, sizeof( BITMAPINFOHEADER ), hp_File );
				L_fwrite( p_Data, walign * h, 1, hp_File );
				L_fclose( hp_File );
			}
			L_free( p_Data );
		}
	}

	/* generate html file */
	sprintf( sz_Name, "%s/%s.htm", sz_Path, sz_TexName );
	hp_File = L_fopen( sz_Name, "wt" );
	if ( hp_File )
	{
		fprintf( hp_File, "<html><body lang=FR style='tab-interval:35.4pt'>\n" );
		fprintf( hp_File, "<p>%s&nbsp</p>\n", sz_TexName );
		fprintf( hp_File, "<table border=0 style='margin-left:24.8pt; border-collapse:collapse;border:none'>\n" );

		for (i = 32; i < mi_LetterNb; i++)
		{
			if ( !mpst_Letter[ i ].c_Valid ) continue;

			sprintf( sz_Name, "%s_Letter_%d", sz_TexName, i );
			w = mpst_Letter[ i ].u1 - mpst_Letter[ i ].u0 + 1;
			h = mpst_Letter[ i ].v1 - mpst_Letter[ i ].v0 + 1;

			fprintf( hp_File, "<tr>\n");
			fprintf( hp_File, "<td width=76  valign=center style='border:solid windowtext .5pt'> <p align=center>%d</p></td>\n", i );
			fprintf( hp_File, "<td width=76  valign=center style='border:solid windowtext .5pt'> <p align=center>&#%d;</p></td>\n", i,i );
			/*
			if ( i < 256 )
				fprintf( hp_File, "<td width=76  valign=center style='border:solid windowtext .5pt'> <p align=center>%c</p></td>\n", i );
			else
				fprintf( hp_File, "<td width=76  valign=center style='border:solid windowtext .5pt'> <p align=center>&nbsp</p></td>\n", i );
				*/
			
			fprintf( hp_File, "<td width=151 valign=center style='border:solid windowtext .5pt'> <p align=center>\n" );
			fprintf( hp_File, "(%d,%d) – (%d,%d)</p></td>\n", mpst_Letter[i].u0, mpst_Letter[i].v0, w, h );
			fprintf( hp_File, "<td width=76  valign=center style='border:solid windowtext .5pt'> <p align=center ><img width=%d height=%d src=\"%s.bmp\"></p> </td>\n", w, h, sz_Name );
			fprintf( hp_File, "</tr>\n" );
		}
		
		fprintf( hp_File, "</table>\n" );
		fprintf( hp_File, "<p class=MsoNormal><![if !supportEmptyParas]>&nbsp;<![endif]><o:p></o:p></p>\n" );
		fprintf( hp_File, "</body></html>\n" );
		fclose( hp_File );
	}

}

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::UpdateUV(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::LetterSelNone(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < mi_LetterNb; i++) mpst_Letter[i].c_Sel = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::UpdateTexture(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	CDC		*pDC;
	HBITMAP h_RGB, h_A;
	int		W, H, result;
    TEX_tdst_4Edit_CreateBitmapResult   st_Res;
	/*~~~~~~~~~~~~~~~~~*/

    st_Res.ppc_Raw = NULL;
    st_Res.ph_Alpha = &h_A;
    st_Res.ph_Colors = &h_RGB;
    st_Res.pi_Height = &H;
    st_Res.pi_Width = &W;
    st_Res.pst_Pal = NULL;
    st_Res.ph_PaletteAlpha = NULL;
    st_Res.ph_PaletteColors = NULL;
    st_Res.b_RawPalPrio = 0;

	pDC = GetDC();
    result = TEX_i_4Edit_CreateBitmap(mul_TextureIndex, -1, pDC->GetSafeHdc(), &st_Res );
	ReleaseDC(pDC);

	if(result)
	{
		DeleteBitmap();
		mh_BitmapRGB = h_RGB;
		mh_BitmapA = h_A;
		mi_BmpWidth = W;
		mi_BmpHeight = H;
		InvalidateRect(NULL, FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::UpdateLetterList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	int			i, j, top;
	char		sz_Text[100], *sz_Cur;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_LETTER);
	top = po_LB->GetTopIndex();
	po_LB->ResetContent();

	for(i = 32; i < mi_LetterNb; i++)
	{
		sz_Cur = sz_Text + sprintf(sz_Text, "%4d", i, i);
		j = po_LB->AddString(sz_Text);
		po_LB->SetItemData(j, i);
	}

	po_LB->SetTopIndex( top );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::UpdateCurrentLetter(int _i_Letter)
{
	mi_CurrentLetter = _i_Letter;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::UpdateShowStatus(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*po_LB;
	int			i, j;
	int			ai_Sel[5000];
	/*~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < mi_LetterNb; i++)
	{
		mpst_Letter[i].c_Show = 0;
	}

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_LETTER);
	j = po_LB->GetSelItems(5000, ai_Sel);
	for(i = 0; i < j; i++)
	{
		mpst_Letter[po_LB->GetItemData(ai_Sel[i])].c_Show = 1;
	}

	for(i = 0; i < mi_LetterNb; i++)
	{
		if(!mpst_Letter[i].c_Show) mpst_Letter[i].c_Sel = 0;
	}

	UpdateCurrentLetter((j != 0) ? po_LB->GetItemData(ai_Sel[0]) : -1);
	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_FontDescriptorDialog::WheelZoom(int i_Delta)
{
	/*~~~~~~~~~*/
	CPoint	o_Pt;
	int		X, Y;
	/*~~~~~~~~~*/

	GetCursorPos(&o_Pt);
	ScreenToClient(&o_Pt);

	if(!mo_TexRect.PtInRect(o_Pt))
		return 0;
	else if(GetAsyncKeyState(VK_SPACE) < 0)
	{
		X = ((o_Pt.x - mo_TexRect.left) / mi_Zoom ) + mi_X;
		Y = ((o_Pt.y - mo_TexRect.top) / mi_Zoom ) + mi_Y;
		

		mi_Zoom += (i_Delta < 0) ? -1 : 1;
		if(mi_Zoom < 1)
		{
			mi_Zoom = 1;
			return 1;
		}

		if(mi_Zoom > 16)
		{
			mi_Zoom = 16;
			return 1;
		}

		mi_X = X - (o_Pt.x - mo_TexRect.left) / mi_Zoom;
		mi_Y = Y - (o_Pt.y - mo_TexRect.top) / mi_Zoom;

		X = (mi_BmpWidth - mi_X) * mi_Zoom;
		if(X < mo_TexRect.Width()) mi_X = mi_BmpWidth - (mo_TexRect.Width() / mi_Zoom);
		Y = (mi_BmpWidth - mi_Y) * mi_Zoom;
		if(Y < mo_TexRect.Height()) mi_Y = mi_BmpHeight - (mo_TexRect.Height() / mi_Zoom );

		if(mi_X < 0) mi_X = 0;
		if(mi_Y < 0) mi_Y = 0;

		InvalidateRect(NULL, FALSE);
	}

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::SelRect_Draw(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CDC		*pDC;
	CPen	o_Pen, *poldpen;
	/*~~~~~~~~~~~~~~~~~~~~*/

	pDC = GetDC();
	pDC->SetROP2(R2_XORPEN);

	o_Pen.CreatePen(PS_SOLID, 1, 0xFFFFFF);
	poldpen = pDC->SelectObject(&o_Pen);

	if(mi_Capture == 4)
	{
		mst_SelLetter.x[0] = mo_SelRect.left;
		mst_SelLetter.x[1] = mo_SelRect.right;
		mst_SelLetter.y[0] = mo_SelRect.top;
		mst_SelLetter.y[1] = mo_SelRect.bottom;

		PointNormalize(&mst_SelLetter);

		Point2UV(mst_SelLetter.x[0], mst_SelLetter.y[0], &mst_SelLetter.u0, &mst_SelLetter.v0);
		Point2UV(mst_SelLetter.x[1], mst_SelLetter.y[1], &mst_SelLetter.u1, &mst_SelLetter.v1);

		UV2Point(mst_SelLetter.u0, mst_SelLetter.v0, &mst_SelLetter.x[0], &mst_SelLetter.y[0], 0);
		UV2Point(mst_SelLetter.u1, mst_SelLetter.v1, &mst_SelLetter.x[1], &mst_SelLetter.y[1], 1);

		pDC->MoveTo(mst_SelLetter.x[0], mst_SelLetter.y[0]);
		pDC->LineTo(mst_SelLetter.x[1], mst_SelLetter.y[0]);
		pDC->LineTo(mst_SelLetter.x[1], mst_SelLetter.y[1]);
		pDC->LineTo(mst_SelLetter.x[0], mst_SelLetter.y[1]);
		pDC->LineTo(mst_SelLetter.x[0], mst_SelLetter.y[0]);
	}
	else
	{
		pDC->MoveTo(mo_SelRect.TopLeft());
		pDC->LineTo(mo_SelRect.right, mo_SelRect.top);
		pDC->LineTo(mo_SelRect.BottomRight());
		pDC->LineTo(mo_SelRect.left, mo_SelRect.bottom);
		pDC->LineTo(mo_SelRect.TopLeft());
	}

	pDC->SelectObject(poldpen);
	DeleteObject(&o_Pen);
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::SelRect_Sel(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	int					i;
	EDIA_tdst_Letter	*l;
	/*~~~~~~~~~~~~~~~~~~~*/

	mo_SelRect.NormalizeRect();
	l = mpst_Letter + 32;

	if(GetAsyncKeyState(VK_SHIFT) < 0)
	{
		if(GetAsyncKeyState(VK_CONTROL) < 0)
		{
			for(i = 32; i < mi_LetterNb; i++, l++)
			{
				if(!l->c_Visible) continue;
				M_IfLetterInRect(mo_SelRect, l) l->c_Sel = 1 - l->c_Sel;
			}
		}
		else
		{
			for(i = 32; i < mi_LetterNb; i++, l++)
			{
				if(!l->c_Visible) continue;
				M_IfLetterInRect(mo_SelRect, l) l->c_Sel = 0;
			}
		}
	}
	else
	{
		for(i = 32; i < mi_LetterNb; i++, l++)
		{
			if(!l->c_Visible) continue;
			M_IfLetterInRect(mo_SelRect, l) l->c_Sel = 1;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::LetterDelete(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 32; i < mi_LetterNb; i++)
	{
		if(mpst_Letter[i].c_Show)
		{
			mpst_Letter[i].u0 = mpst_Letter[i].u1 = 0;
			mpst_Letter[i].c_Valid = 0;
			UpdateLetterList();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::DeleteBitmap(void)
{
	if(mh_BitmapRGB)
	{
		DeleteObject(mh_BitmapRGB);
		mh_BitmapRGB = NULL;
	}

	if(mh_BitmapA)
	{
		DeleteObject(mh_BitmapA);
		mh_BitmapA = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::LoadLetterDesc(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_FontLetterDesc *pst_Letter;
	int						i, saveu; 
	float					fu0, fu1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( mi_LetterNb && mpst_Letter )
	{
		mi_LetterNb = 0;
		L_free( mpst_Letter );
		mpst_Letter = NULL;
	}
	
	mpst_Font = STR_pst_GetFont(BIG_FileKey(mul_TextureIndex));
	if(mpst_Font == NULL) return; //-1;

	mi_LetterNb = mpst_Font->ul_NbChar + 32;
	mpst_Letter = (EDIA_tdst_Letter *) L_malloc( mi_LetterNb * sizeof( EDIA_tdst_Letter ) );
	L_memset( mpst_Letter, 0, mi_LetterNb * sizeof( EDIA_tdst_Letter ) );

	
	pst_Letter = mpst_Font->pst_Letter;
	for(i = 32; i < mi_LetterNb; i++, pst_Letter++)
	{
		fu0 = pst_Letter->f_U[0];
		fu1 = pst_Letter->f_U[1];
		if ( fu0 > 2.0f )
		{
			fu0 -= 2.0f;
			fu1 -= 2.0f;
			mpst_Letter[i].c_RotateCCW = 1;
		}
		else if ( fu0 > 1.0f )
		{
			fu0 -= 1.0f;
			fu1 -= 1.0f;
			mpst_Letter[i].c_RotateCW = 1;
		}

		mpst_Letter[i].u0 = (int) (fu0 * mi_BmpWidth);
		mpst_Letter[i].u1 = (int) (fu1 * mi_BmpWidth);
		mpst_Letter[i].v0 = (int) ((1.0f - pst_Letter->f_V[1]) * mi_BmpHeight);
		mpst_Letter[i].v1 = (int) ((1.0f - pst_Letter->f_V[0]) * mi_BmpHeight);

        if (mpst_Letter[i].u0 > mi_BmpWidth)
            mpst_Letter[i].u0 = mi_BmpWidth;
        if (mpst_Letter[i].u1 < mpst_Letter[i].u0 )
		{
			saveu = mpst_Letter[i].u1;
            mpst_Letter[i].u1 = mpst_Letter[i].u0;
			mpst_Letter[i].u0 = saveu;
			mpst_Letter[i].c_InvertX = 1;
		}

        
        if (mpst_Letter[i].v0 > mi_BmpHeight)
            mpst_Letter[i].v0 = mi_BmpHeight;
        if (mpst_Letter[i].v1 < mpst_Letter[i].v0 )
		{
			saveu = mpst_Letter[i].v1;
            mpst_Letter[i].v1 = mpst_Letter[i].v0;
			mpst_Letter[i].v0 = saveu;
			mpst_Letter[i].c_InvertY = 1;
		}

		mpst_Letter[i].u1--;
		mpst_Letter[i].v1--;

		if(mpst_Letter[i].u1 > 0)
            mpst_Letter[i].c_Valid = 1;
        else
            mpst_Letter[i].u1 = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FontDescriptorDialog::WriteLetterDesc(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	STR_tdst_FontLetterDesc *pst_Letter;
	int						i;
	float					oow, ooh;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpst_Font == NULL) return;

	oow = 1.0f / ((float) mi_BmpWidth);
	ooh = 1.0f / ((float) mi_BmpHeight);
	
	if ( (mi_LetterNb - 32 ) != (int) mpst_Font->ul_NbChar )
	{
		mpst_Font->pst_Letter = (STR_tdst_FontLetterDesc *) MEM_p_Realloc( mpst_Font->pst_Letter, (mi_LetterNb - 32 ) * sizeof( STR_tdst_FontLetterDesc ) );
		if ((mi_LetterNb - 32 ) > (int) mpst_Font->ul_NbChar )
			L_memset( mpst_Font->pst_Letter + mpst_Font->ul_NbChar, 0, ((mi_LetterNb - 32 ) - mpst_Font->ul_NbChar) * sizeof( STR_tdst_FontLetterDesc ) );
		mpst_Font->ul_NbChar = (mi_LetterNb - 32 );
	}

	pst_Letter = mpst_Font->pst_Letter;
	for(i = 32; i < mi_LetterNb; i++, pst_Letter++)
	{
		if(mpst_Letter[i].c_Valid)
		{
			if ( mpst_Letter[ i ].c_InvertX )
			{
				pst_Letter->f_U[0] = ((float) mpst_Letter[i].u1 + 1) * oow;
				pst_Letter->f_U[1] = ((float) mpst_Letter[i].u0) * oow;
			}
			else
			{
				pst_Letter->f_U[0] = ((float) mpst_Letter[i].u0) * oow;
				pst_Letter->f_U[1] = ((float) mpst_Letter[i].u1 + 1) * oow;
			}
			if (mpst_Letter[i].c_InvertY)
			{
				pst_Letter->f_V[0] = 1.0f - ((float) mpst_Letter[i].v0) * ooh;
				pst_Letter->f_V[1] = 1.0f - ((float) mpst_Letter[i].v1 + 1) * ooh;
			}
			else
			{
				pst_Letter->f_V[0] = 1.0f - ((float) mpst_Letter[i].v1 + 1) * ooh;
				pst_Letter->f_V[1] = 1.0f - ((float) mpst_Letter[i].v0) * ooh;
			}
			if (mpst_Letter[i].c_RotateCCW)
			{
				pst_Letter->f_U[0] += 2.0f;
				pst_Letter->f_U[1] += 2.0f;
			}
			else if (mpst_Letter[i].c_RotateCW)
			{
				pst_Letter->f_U[0] += 1.0f;
				pst_Letter->f_U[1] += 1.0f;
			}
		}
		else
		{
			pst_Letter->f_U[1] = 0;
		}
	}
}

#endif /* ACTIVE_EDITORS */
