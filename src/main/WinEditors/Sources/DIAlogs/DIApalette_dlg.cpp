/*$T DIApalette_dlg.cpp GC! 1.081 04/19/01 10:17:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "DIAlogs/DIApalette_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIArliadjust_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "EDIpaths.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "F3DFrame/F3Dview.h"
#include "SOFT/SOFTbackgroundImage.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "INOut/INOjoystick.h"
#include "LINKs/LINKmsg.h"
#include "TEXture/TEXfile.h"
#include "DIAlogs/DIACOLOR_dlg.h"

BEGIN_MESSAGE_MAP(EDIA_cl_Palette, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_CHECK_COLOR, OnChangeDisplayComponent)
	ON_BN_CLICKED(IDC_CHECK_RED, OnChangeDisplayComponent)
	ON_BN_CLICKED(IDC_CHECK_GREEN, OnChangeDisplayComponent)
	ON_BN_CLICKED(IDC_CHECK_BLUE, OnChangeDisplayComponent)
	ON_BN_CLICKED(IDC_CHECK_ALPHA, OnChangeDisplayComponent)
	ON_BN_CLICKED(IDC_BUTTON_OPALPHA, OnSwapAlphaChannel)
	ON_BN_CLICKED(IDC_BUTTON_COLORNUMBER, OnSwapColorNumber)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_Palette::EDIA_cl_Palette(ULONG _ul_Index) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_PALETTE)
{
	mul_Index = _ul_Index;
    L_memset( &mst_Palette, 0, sizeof( TEX_tdst_Palette ) );
	mst_Palette.ul_Key = BIG_FileKey(_ul_Index);
	TEX_File_LoadPalette(&mst_Palette, TRUE);

	if(mst_Palette.uc_Flags & TEX_uc_InvalidPalette)
	{
		mb_Alpha = FALSE;
		mi_NbColor = 256;
		L_memset(&mul_Color, 0, 1024);
	}
	else
	{
		mb_Alpha = mst_Palette.uc_Flags & TEX_uc_AlphaPalette;
		mi_NbColor = (mst_Palette.uc_Flags & TEX_uc_Palette16) ? 16 : 256;
		L_memcpy(&mul_Color, mst_Palette.pul_Color, mi_NbColor * 4);
		TEX_M_File_Free(mst_Palette.pul_Color);
	}

	L_memset(&mc_Sel, 0, 256);
	mi_Picked = -1;
	mi_Picked4Gradient = -1;
    mi_Picked4Invert = -1;
    mi_Picked4DeductAlpha = -1;

	mi_DisplayComponent = C_DisplayAll;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_Palette::~EDIA_cl_Palette(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_Palette::OnInitDialog(void)
{
	ComputeDrawRect();
	UpdateControl();
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_Palette::PreTranslateMessage(MSG *pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE) return 1;

		OnKeyDown(pMsg->wParam);
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnOK(void)
{
    ULONG *pul_Save;
	/* saving */
    pul_Save = mst_Palette.pul_Color;
	mst_Palette.pul_Color = mul_Color;
	TEX_ul_File_SavePaletteBF(&mst_Palette, TRUE);
    mst_Palette.pul_Color = pul_Save;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnCancel(void)
{
	EDIA_cl_BaseDialog::OnCancel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 410;
	lpMMI->ptMinTrackSize.y = 310;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnSize(UINT n, int x, int y)
{
	ComputeDrawRect();
	InvalidateRect(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_Palette::OnEraseBkgnd(CDC *pdc)
{
	/*~~~~~~~~~~*/
	DWORD	x_Col;
	CRect	oRect;
	/*~~~~~~~~~~*/

	x_Col = GetSysColor(COLOR_BTNFACE);
	GetClientRect(&oRect);

	pdc->FillSolidRect(0, 0, mo_DrawRect.left, oRect.bottom, x_Col);
	pdc->FillSolidRect(mo_DrawRect.right, 0, oRect.right, oRect.bottom, x_Col);
	pdc->FillSolidRect(mo_DrawRect.left, 0, mo_DrawRect.right, mo_DrawRect.top, x_Col);
	pdc->FillSolidRect(mo_DrawRect.left, mo_DrawRect.bottom, mo_DrawRect.right, oRect.bottom, x_Col);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC		*pDC;
	int		i, x, y, w1, w2;
	ULONG	*pul_Color, ul_Mask, ul_Alpha;
	CPen	o_WhitePen, o_RedPen;
	void	*p_OldPen;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnPaint();

	pDC = GetDC();

	o_WhitePen.CreatePen(PS_SOLID, 1, RGB(0xFF, 0xFF, 0xFF));
	//o_RedPen.CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
    o_RedPen.CreatePen(PS_DOT, 1, RGB(0xFF, 0, 0));

	p_OldPen = pDC->SelectObject(o_WhitePen);

	pul_Color = mul_Color;
	ul_Mask = 0;
	ul_Mask |= (mi_DisplayComponent & C_DisplayR) ? 0xFF : 0;
	ul_Mask |= (mi_DisplayComponent & C_DisplayG) ? 0xFF00 : 0;
	ul_Mask |= (mi_DisplayComponent & C_DisplayB) ? 0xFF0000 : 0;

	if((mi_DisplayComponent & C_DisplayA) && (mb_Alpha))
	{
		if(mi_DisplayComponent & C_DisplayColor)
		{
			w1 = (mo_Case.cx / 4) * 3;
			w2 = mo_Case.cx - w1;
			for(y = mo_DrawRect.top; y < mo_DrawRect.bottom; y += mo_Case.cy)
			{
				for(x = mo_DrawRect.left; x < mo_DrawRect.right; x += mo_Case.cx)
				{
					ul_Alpha = *pul_Color >> 24;
					ul_Alpha |= (ul_Alpha << 8) | (ul_Alpha << 16);
					pDC->FillSolidRect(x, y, w1, mo_Case.cy, (*pul_Color++ &ul_Mask));
					pDC->FillSolidRect(x + w1, y, w2, mo_Case.cy, ul_Alpha);
				}
            }
        } 
		else
		{
			for(y = mo_DrawRect.top; y < mo_DrawRect.bottom; y += mo_Case.cy)
			{
				for(x = mo_DrawRect.left; x < mo_DrawRect.right; x += mo_Case.cx)
				{
					ul_Alpha = *pul_Color++ >> 24;
					ul_Alpha |= (ul_Alpha << 8) | (ul_Alpha << 16);
					pDC->FillSolidRect(x, y, mo_Case.cx, mo_Case.cy, ul_Alpha);
				}
            }
		}
	}
	else
	{
		if(!(mi_DisplayComponent & C_DisplayColor)) ul_Mask = 0;
		for(y = mo_DrawRect.top; y < mo_DrawRect.bottom; y += mo_Case.cy)
		{
			for(x = mo_DrawRect.left; x < mo_DrawRect.right; x += mo_Case.cx)
				pDC->FillSolidRect(x, y, mo_Case.cx, mo_Case.cy, (*pul_Color++ &ul_Mask));
        } 
	}

	for(y = mo_DrawRect.top; y <= mo_DrawRect.bottom; y += mo_Case.cy)
	{
		pDC->MoveTo(mo_DrawRect.left, y);
		pDC->LineTo(mo_DrawRect.right, y);
	}

	for(x = mo_DrawRect.left; x <= mo_DrawRect.right; x += mo_Case.cx)
	{
		pDC->MoveTo(x, mo_DrawRect.top);
		pDC->LineTo(x, mo_DrawRect.bottom);
	}

	/*
	if(mi_Picked != -1)
	{
		pDC->SelectObject(o_RedPen);
		Index2LineCol(mi_Picked, &x, &y);
		x = mo_DrawRect.left + (x * mo_Case.cx) + 1;
		y = mo_DrawRect.top + (y * mo_Case.cy) + 1;

		pDC->MoveTo(x, y);
		pDC->LineTo(x + mo_Case.cx - 2, y);
		pDC->LineTo(x + mo_Case.cx - 2, y + mo_Case.cy - 2);
		pDC->LineTo(x, y + mo_Case.cy- 2);
		pDC->LineTo(x, y);
	}
	*/
	for (i = 0; i < mi_NbColor; i++)
	{
		if ( !mc_Sel[ i ] )
			continue;
		pDC->SelectObject(o_RedPen);
		Index2LineCol(i, &x, &y);
		x = mo_DrawRect.left + (x * mo_Case.cx) + 1;
		y = mo_DrawRect.top + (y * mo_Case.cy) + 1;

		pDC->MoveTo(x, y);
		pDC->LineTo(x + mo_Case.cx - 2, y);
		pDC->LineTo(x + mo_Case.cx - 2, y + mo_Case.cy - 2);
		pDC->LineTo(x, y + mo_Case.cy- 2);
		pDC->LineTo(x, y);
	}

	pDC->SelectObject(p_OldPen);
	DeleteObject(o_RedPen);
	DeleteObject(o_WhitePen);
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnMouseMove(UINT ui_Flags, CPoint pt)
{
	if(ui_Flags & MK_LBUTTON) OnLButtonDown(ui_Flags, pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnLButtonDown(UINT ui, CPoint pt)
{
	/*~~~~~~*/
	int index;
	/*~~~~~~*/

	index = Point2Color(pt, NULL, NULL);
    if (mi_Picked4Gradient != -1)
    {
        ComputeGradient( mi_Picked4Gradient, index );
        mi_Picked4Gradient = -1;
    }
    if (mi_Picked4Invert != -1)
    {
        ComputeInvert( mi_Picked4Invert, index );
        mi_Picked4Invert = -1;
    }
    if (mi_Picked4DeductAlpha != -1)
    {
        ComputeDeductAlpha( mi_Picked4DeductAlpha, index );
        mi_Picked4DeductAlpha = -1;
    }

	if((index != -1) && (index != mi_Picked)) 
		SetPickedColor(index, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnLButtonUp(UINT ui, CPoint pt)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnRButtonDown(UINT ui, CPoint pt)
{
	/*~~~~~~~~~~~~~*/
	int index, i_Res;
	/*~~~~~~~~~~~~~*/

	index = Point2Color(pt, NULL, NULL);
	if( index != -1 ) 
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EMEN_cl_SubMenu o_Menu(FALSE);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        if (index != mi_Picked)
		    SetPickedColor(index, 1);

		if(mi_Picked4Gradient != -1)
        {
            mi_Picked4Gradient = -1;
            return;
        }
        if (mi_Picked4Invert != -1)
        {
            mi_Picked4Invert = -1;
            return;
        }
        if (mi_Picked4DeductAlpha != -1)
        {
            mi_Picked4DeductAlpha = -1;
            return;
        }
            
        M_MF()->InitPopupMenuAction(NULL, &o_Menu);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Start Gradient", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Start Invert", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Invert", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Start Deduct Alpha from RGB", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 5, TRUE, "Deduct Alpha from RGB", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 6, TRUE, "Adjust HLS/CB", -1);
        M_MF()->AddPopupMenuAction(NULL, &o_Menu, 7, TRUE, "Adjust HLS/CB on all", -1);
        

        ClientToScreen(&pt);
		i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
		switch(i_Res)
		{
		case 1:
			mi_Picked4Gradient = mi_Picked;
			break;
        case 2:
            mi_Picked4Invert = mi_Picked;
            break;
        case 3:
            ComputeInvert( mi_Picked, mi_Picked );
            break;
        case 4:
            mi_Picked4DeductAlpha = mi_Picked;
            break;
        case 5:
            ComputeDeductAlpha( mi_Picked, mi_Picked );
            break;
        case 6:
			{
				EDIA_cl_RLIAdjustDialog dlg_RLIAdjust( this, mul_Color, mc_Sel, mi_NbColor );
				dlg_RLIAdjust.DoModal();
			}
			break;
		case 7:
			{
				char c_Sel[ 256 ];
				L_memset( c_Sel, 1, 256 );
				EDIA_cl_RLIAdjustDialog dlg_RLIAdjust( this, mul_Color, c_Sel, mi_NbColor );
				dlg_RLIAdjust.DoModal();
			}
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void HookProc(ULONG _o_Dlg, ULONG p_color )
{
    EDIA_cl_Palette * po_Dlg;

    po_Dlg = (EDIA_cl_Palette *) _o_Dlg;

    *(ULONG *) p_color = ( *(ULONG *) p_color & po_Dlg->mul_Mask) | (po_Dlg->mul_SaveColor & po_Dlg->mul_InvMask );

	((EDIA_cl_Palette *) _o_Dlg)->InvalidateRect(((EDIA_cl_Palette *) _o_Dlg)->mo_CaseRect);
	((EDIA_cl_Palette *) _o_Dlg)->UpdateRGBAValue();
}
/**/
void EDIA_cl_Palette::OnLButtonDblClk(UINT ui, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					x, y, index;
	EDIA_cl_ColorDialog *dlg_Color;
	char				sz_Title[64];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	index = Point2Color(pt, &x, &y);
	if(index != -1)
	{
		mo_CaseRect.left = mo_DrawRect.left + (x * mo_Case.cx);
		mo_CaseRect.right = mo_CaseRect.left + mo_Case.cx;
		mo_CaseRect.top = mo_DrawRect.top + (y * mo_Case.cy);
		mo_CaseRect.bottom = mo_CaseRect.top + mo_Case.cy;

        GetMask();
		sprintf(sz_Title, "Color %d", index);
        mul_SaveColor = mul_Color[ index ];
		dlg_Color = new EDIA_cl_ColorDialog(sz_Title, &mul_Color[index], HookProc, (ULONG) this, (ULONG) &mul_Color[index]);
		dlg_Color->DoModal();
		delete(dlg_Color);

		InvalidateRect(mo_DrawRect);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnKeyDown(UINT nChar)
{
}

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    calcul mo_DrawRect:: c'est le rectangle ou est déssinné la palette
 =======================================================================================================================
 */
void EDIA_cl_Palette::ComputeDrawRect(void)
{
	/*~~~~~*/
	int w, h;
	/*~~~~~*/

	GetClientRect(&mo_DrawRect);

	mo_DrawRect.left = 110;
	mo_DrawRect.right -= 5;

	w = ((int) floor((mo_DrawRect.right - mo_DrawRect.left) / 16.0)) * 16;
	mo_DrawRect.right = mo_DrawRect.left + w;

	h = ((int) floor((mo_DrawRect.bottom - 10) / 16.0)) * 16;
	mo_DrawRect.top = 5;
	mo_DrawRect.bottom = mo_DrawRect.top + h;

	mi_NbPerLine = (mi_NbColor == 256) ? 16 : 4;
	mo_Case = CSize(w / mi_NbPerLine, h / mi_NbPerLine);
}

/*
 =======================================================================================================================
    recherche l'index de la couleur sous un point (retourne -1 si en dehors de la palette) si les 2 pointeurs sur des
    entiers (x et y) sont non nuls et qu'une couleur est trouvé sous le point x et y contiendront la ligne et la
    colonne de la couleur
 =======================================================================================================================
 */
int EDIA_cl_Palette::Point2Color(CPoint pt, int *_x, int *_y)
{
	/*~~~~~*/
	int x, y;
	/*~~~~~*/

	if(!mo_DrawRect.PtInRect(pt)) return -1;

	x = (pt.x - mo_DrawRect.left) / mo_Case.cx;
	y = (pt.y - mo_DrawRect.top) / mo_Case.cy;

	if(_x) *_x = x;
	if(_y) *_y = y;

	return(x + (y * mi_NbPerLine));
}

/*
 =======================================================================================================================
    recherche l'index de la couleur sous un point (retourne -1 si en dehors de la palette) si les 2 pointeurs sur des
    entiers (x et y) sont non nuls et qu'une couleur est trouvé sous le point x et y contiendront la ligne et la
    colonne de la couleur
 =======================================================================================================================
 */
void EDIA_cl_Palette::Index2LineCol(int index, int *_x, int *_y)
{
	/*~~~~~~~~~~~*/
	div_t	result;
	/*~~~~~~~~~~~*/

	result = div(index, mi_NbPerLine);

	if(_x) *_x = result.rem;
	if(_y) *_y = result.quot;
}

/*
 =======================================================================================================================
    Selectionne une couleur
 =======================================================================================================================
 */
void EDIA_cl_Palette::SetPickedColor(int _i_Color, int _i_RButton)
{
	mi_Picked = _i_Color;
	UpdateRGBAValue();
	InvalidateRect(&mo_DrawRect);
	
	if ( GetAsyncKeyState( VK_CONTROL ) >= 0 )
	{
		L_memset( mc_Sel, 0, 256 );
		mc_Sel[ mi_Picked ] = 1;
		return;
	}
	
	if (_i_RButton)
	{
		mc_Sel[ mi_Picked ] = 1;
	}
	else
	{
		if (mc_Sel[ mi_Picked ] == 1)
			mc_Sel[ mi_Picked ] = 0;
		else
			mc_Sel[ mi_Picked ] = 1;
	}
}

/*
 =======================================================================================================================
    Met à jour les valeurs index, rouge, vert, bleu, alpha
 =======================================================================================================================
 */
void EDIA_cl_Palette::UpdateRGBAValue(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int		color;
	char	sz_Value[10];
	/*~~~~~~~~~~~~~~~~~*/

	color = mul_Color[mi_Picked];
	GetDlgItem(IDC_EDIT_INDEX)->SetWindowText(_itoa(mi_Picked, sz_Value, 10));
	GetDlgItem(IDC_EDIT_RED)->SetWindowText(_itoa((color & 0xFF), sz_Value, 10));
	GetDlgItem(IDC_EDIT_GREEN)->SetWindowText(_itoa(((color >> 8) & 0xFF), sz_Value, 10));
	GetDlgItem(IDC_EDIT_BLUE)->SetWindowText(_itoa(((color >> 16) & 0xFF), sz_Value, 10));
	GetDlgItem(IDC_EDIT_ALPHA)->SetWindowText(_itoa(((color >> 24) & 0xFF), sz_Value, 10));
}

/*
 =======================================================================================================================
    Met à jour les valeurs index, rouge, vert, bleu, alpha
 =======================================================================================================================
 */
void EDIA_cl_Palette::UpdateControl(void)
{
	GetDlgItem(IDC_CHECK_ALPHA)->ShowWindow(mb_Alpha ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_STATIC_ALPHA)->ShowWindow(mb_Alpha ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_EDIT_ALPHA)->ShowWindow(mb_Alpha ? SW_SHOW : SW_HIDE);
	((CButton *) GetDlgItem(IDC_CHECK_COLOR))->SetCheck((mi_DisplayComponent & C_DisplayColor) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_RED))->SetCheck((mi_DisplayComponent & C_DisplayR) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_GREEN))->SetCheck((mi_DisplayComponent & C_DisplayG) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_BLUE))->SetCheck((mi_DisplayComponent & C_DisplayB) ? 1 : 0);
	((CButton *) GetDlgItem(IDC_CHECK_ALPHA))->SetCheck((mi_DisplayComponent & C_DisplayA) ? 1 : 0);

	GetDlgItem(IDC_BUTTON_OPALPHA)->SetWindowText(mb_Alpha ? "Del alpha" : "Add alpha");
	GetDlgItem(IDC_BUTTON_COLORNUMBER)->SetWindowText((mi_NbColor == 16) ? "256 colors" : "16 colors");
}

/*
 =======================================================================================================================
    Met à jour les valeurs index, rouge, vert, bleu, alpha
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnChangeDisplayComponent(void)
{
	mi_DisplayComponent = ((CButton *) GetDlgItem(IDC_CHECK_COLOR))->GetCheck() ? C_DisplayColor : 0;
	mi_DisplayComponent |= ((CButton *) GetDlgItem(IDC_CHECK_RED))->GetCheck() ? C_DisplayR : 0;
	mi_DisplayComponent |= ((CButton *) GetDlgItem(IDC_CHECK_GREEN))->GetCheck() ? C_DisplayG : 0;
	mi_DisplayComponent |= ((CButton *) GetDlgItem(IDC_CHECK_BLUE))->GetCheck() ? C_DisplayB : 0;
	mi_DisplayComponent |= ((CButton *) GetDlgItem(IDC_CHECK_ALPHA))->GetCheck() ? C_DisplayA : 0;
	InvalidateRect(&mo_DrawRect);
}

/*
 =======================================================================================================================
    enlève le canal alpha si il y en a un, sinon rajoute le canal alpha
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnSwapAlphaChannel(void)
{
	mb_Alpha = !mb_Alpha;
	if(mb_Alpha)
		mst_Palette.uc_Flags |= TEX_uc_AlphaPalette;
	else
		mst_Palette.uc_Flags &= ~TEX_uc_AlphaPalette;
	UpdateControl();
	InvalidateRect(&mo_DrawRect);
}

/*
 =======================================================================================================================
    change le nombre de couleurs de la palette (16 -> 256, 256 -> 16 )
 =======================================================================================================================
 */
void EDIA_cl_Palette::OnSwapColorNumber(void)
{
	if(mi_NbColor == 16)
	{
		mi_NbColor = 256;
		mst_Palette.uc_Flags &= ~TEX_uc_Palette16;
	}
	else
	{
		mi_NbColor = 16;
		mst_Palette.uc_Flags |= TEX_uc_Palette16;
	}

	UpdateControl();
	ComputeDrawRect();
	InvalidateRect(&mo_DrawRect);
}

/*
 =======================================================================================================================
    recupère le mask
 =======================================================================================================================
 */
void EDIA_cl_Palette::GetMask()
{
    mul_Mask = 0;
    if ( mi_DisplayComponent & C_DisplayColor)
    {
	    mul_Mask |= (mi_DisplayComponent & C_DisplayR) ? 0xFF : 0;
	    mul_Mask |= (mi_DisplayComponent & C_DisplayG) ? 0xFF00 : 0;
	    mul_Mask |= (mi_DisplayComponent & C_DisplayB) ? 0xFF0000 : 0;
    }
	if((mi_DisplayComponent & C_DisplayA) && (mb_Alpha))
        mul_Mask |= 0xFF000000;

    mul_InvMask = ~mul_Mask;
}


/*
 =======================================================================================================================
    compute gradient color between two color indexes
 =======================================================================================================================
 */
void EDIA_cl_Palette::ComputeGradient( int i1, int i2 )
{
    int     i, n;
    float   f_Ratio, f_Interval;
    ULONG   C1, C2, C;
    float   r[3], g[3], b[3], a[3];

    if (i1 == i2) return;
    if (i1 > i2)
    {
        ComputeGradient( i2, i1);
        return;
    }
    
    n = i2 - i1;
    f_Interval = 1.0f / n;

    C1 = mul_Color[ i1 ];
    r[1] = (C1 & 0xFF) / 255.0f;
    g[1] = ((C1 & 0xFF00)>>8) / 255.0f;
    b[1] = ((C1 & 0xFF0000)>>16) / 255.0f;
    a[1] = ((C1 & 0xFF000000)>>24) / 255.0f;
    C2 = mul_Color[ i2 ];
    r[2] = (C2 & 0xFF) / 255.0f;
    g[2] = ((C2 & 0xFF00)>>8) / 255.0f;
    b[2] = ((C2 & 0xFF0000)>>16) / 255.0f;
    a[2] = ((C2 & 0xFF000000)>>24) / 255.0f;

    GetMask();

    for (i = i1 + 1, f_Ratio = f_Interval; i < i2; i++, f_Ratio += f_Interval )
    {
        r[0] = r[2] * f_Ratio + r[1] * (1.0f - f_Ratio);
        g[0] = g[2] * f_Ratio + g[1] * (1.0f - f_Ratio);
        b[0] = b[2] * f_Ratio + b[1] * (1.0f - f_Ratio);
        a[0] = a[2] * f_Ratio + a[1] * (1.0f - f_Ratio);

        C = (ULONG) (r[0] * 255.0f);
        C |= ((ULONG) (g[0] * 255.0f)) << 8;
        C |= ((ULONG) (b[0] * 255.0f)) << 16;
        C |= ((ULONG) (a[0] * 255.0f)) << 24;
        
        mul_Color[ i ] = (mul_Color[ i ] & mul_InvMask) | (C & mul_Mask);
    }
}

/*
 =======================================================================================================================
    compute gradient color between two color indexes
 =======================================================================================================================
 */
void EDIA_cl_Palette::ComputeInvert( int i1, int i2 )
{
    int     i;
    ULONG   C;

    if (i1 > i2)
    {
        ComputeInvert( i2, i1 );
        return;
    }

    GetMask();

    for (i = i1 ; i <= i2; i++ )
    {
        C = mul_Color[i];
        mul_Color[i] = (C & mul_InvMask) | ((~C) & mul_Mask);
    }
    
    if (i1==i2)
        InvalidateRect(&mo_DrawRect);
}

/*
 =======================================================================================================================
    compute deducted alpha from RGB value
 =======================================================================================================================
 */
void EDIA_cl_Palette::ComputeDeductAlpha( int i1, int i2 )
{
    int     i;
    ULONG   C, r, g, b, max, min;
    

    if (i1 > i2)
    {
        ComputeDeductAlpha( i2, i1 );
        return;
    }

    for (i = i1 ; i <= i2; i++ )
    {
        C = mul_Color[i];
        r = C & 0xFF;
        g = (C >> 8) & 0xFF;
        b = (C >> 16) & 0xFF;
        max = ( r > g ) ? ( (r > b) ? r : b) : (g > b) ? g : b;
        min = ( r < g ) ? ( (r < b) ? r : b) : (g < b) ? g : b;
        mul_Color[i] = (C & 0xFFFFFF) | ( ((max+min) >> 1) << 24);
    }
    
    if (i1==i2)
        InvalidateRect(&mo_DrawRect);
}




#endif /* ACTIVE_EDITORS */
