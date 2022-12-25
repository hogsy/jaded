/*$T DIAsaveaddmatrix_dlg.cpp GC! 1.081 12/17/01 16:52:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "DIAlogs/DIAstockofcolor_dlg.h"
#include "Res/Res.h"


static int gai_Color_Control[ 10 ] = 
	{
		IDC_STATIC_COLOR0,
		IDC_STATIC_COLOR1,
		IDC_STATIC_COLOR2,
		IDC_STATIC_COLOR3,
		IDC_STATIC_COLOR4,
		IDC_STATIC_COLOR5,
		IDC_STATIC_COLOR6,
		IDC_STATIC_COLOR7,
		IDC_STATIC_COLOR8,
		IDC_STATIC_COLOR9
	};

BEGIN_MESSAGE_MAP(EDIA_cl_StockOfColorDialog, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_StockOfColorDialog::EDIA_cl_StockOfColorDialog( ULONG *_pul_Stock, ULONG _ul_Current, CRect _o_Rect) : EDIA_cl_BaseDialog(DIALOGS_IDD_STOCKOFCOLOR )
{
	mul_Res_Color = 0;
	
	mpul_Stock = _pul_Stock;
	mul_Current = _ul_Current;
	
	mo_ParentColorRect = _o_Rect;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_StockOfColorDialog::~EDIA_cl_StockOfColorDialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_StockOfColorDialog::OnInitDialog()
{
	int		max, x, y;
	CRect	o_Rect;
	
	GetClientRect( &o_Rect );
	
	max = GetSystemMetrics( SM_CXSCREEN );
	if (mo_ParentColorRect.right + o_Rect.Width() > max )
		x = mo_ParentColorRect.left - o_Rect.Width();
	else
		x = mo_ParentColorRect.right;
		
	max = GetSystemMetrics( SM_CYSCREEN );
	if (mo_ParentColorRect.top + o_Rect.Height() > max )
		y = mo_ParentColorRect.bottom - o_Rect.Height();
	else
		y = mo_ParentColorRect.top;
	
	SetWindowPos( NULL, x, y, 0, 0, SWP_NOREDRAW | SWP_NOSIZE | SWP_NOZORDER );
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_StockOfColorDialog::OnPaint(void)
{
	ULONG	ul_Color;
	RECT	st_Rect;
	CDC		*pDC;
	int		i, right;
	ULONG	alpha;
	
	EDIA_cl_BaseDialog::OnPaint();
	
	pDC = GetDC();
	
	for (i = 0; i < 10; i++)
	{
		ul_Color = mpul_Stock ? mpul_Stock[ i ] : 0;
		GetDlgItem( gai_Color_Control[ i ] )->GetWindowRect( &st_Rect );
		ScreenToClient( &st_Rect );
		st_Rect.bottom += 1;
		right = st_Rect.right;
		st_Rect.right = st_Rect.left + (int) ((float) (right - st_Rect.left) * 0.75);
		pDC->FillSolidRect( &st_Rect, ul_Color & 0xFFFFFF );
		st_Rect.left = st_Rect.right + 1;
		st_Rect.right = right;
		alpha = (ul_Color & 0xFF000000) >> 24;
		ul_Color = alpha | (alpha << 8) | (alpha << 16 );
		pDC->FillSolidRect( &st_Rect, ul_Color & 0xFFFFFF );
	}
	
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_StockOfColorDialog::PreTranslateMessage(MSG *pMsg)
{
	int	i_ColorIndex;
	if ( pMsg->message == WM_LBUTTONDOWN )
	{
		i_ColorIndex = GetColorIndex();
		if ( i_ColorIndex != -1 )
		{
			mul_Res_Color = mpul_Stock ? mpul_Stock[ i_ColorIndex ] : 0;
			OnOK();
		}
	}
	if ( pMsg->message == WM_RBUTTONDOWN )
	{
		i_ColorIndex = GetColorIndex();
		if ( ( i_ColorIndex != -1 ) && ( mpul_Stock ) )
		{
			mpul_Stock[ i_ColorIndex ] = mul_Current;
			Invalidate();
		}
	}
	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_StockOfColorDialog::GetColorIndex(void)
{
	POINT	st_Pt;
	CRect	o_Rect;
	int		i;
	
	GetCursorPos( &st_Pt );
	for (i = 0; i < 10; i++ )
	{
		GetDlgItem( gai_Color_Control[ i ] )->GetWindowRect( &o_Rect );
		if (o_Rect.PtInRect( st_Pt ) )
			return i;
	}
	return -1;
}

#endif /* ACTIVE_EDITORS */

