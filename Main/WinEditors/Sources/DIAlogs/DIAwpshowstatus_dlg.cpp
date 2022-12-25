/*$T DIApalette_dlg.cpp GC! 1.081 04/19/01 10:17:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "DIAlogs/DIAwpshowstatus_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
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
#include "LINKs/LINKtoed.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXeditorfct.h"
#include "DIAlogs/DIACOLOR_dlg.h"

/*
 =======================================================================================================================
    MESSAGE MAP
 =======================================================================================================================
 */
BEGIN_MESSAGE_MAP(EDIA_cl_WPShowStatus, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_ALL, OnAll )
	ON_BN_CLICKED(IDC_BUTTON_NONE, OnNone )
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_WPShowStatus::EDIA_cl_WPShowStatus( ULONG *pul_Flags, ULONG *pul_Flags2 ) :	EDIA_cl_BaseDialog(DIALOGS_IDD_WPSHOWSTATUS )
{
	mul_TextureIndex = BIG_ul_SearchFileExt(EDI_Csz_Path_DisplayData, "DebugObjectTexture.tga" );
  
    mh_BitmapRGB = 0;
    mpo_DC = NULL;
    mul_RawIndex = mul_TgaIndex = BIG_C_InvalidIndex;
    
    mpul_Flags = pul_Flags;
    mpul_Flags2 = pul_Flags2;
    UpdateTableFromFlags();
    
	L_memcpy( c_SaveValue, c_Value, sizeof( c_Value ) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_WPShowStatus::~EDIA_cl_WPShowStatus(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_WPShowStatus::OnInitDialog(void)
{
    UpdateTexture();
	ComputeDrawRect();
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_WPShowStatus::UpdateTableFromFlags()
{
	int i;
	
	for (i = 0; i < 32; i++)
	{
		c_Value[ 1 + i ] = (*mpul_Flags & (1 << i) ) ? 1 : 0;
		c_Value[ 33 + i ] = (*mpul_Flags2 & (1 << i) ) ? 1 : 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_WPShowStatus::UpdateFlagsFromTable()
{
	int i;
	
	*mpul_Flags = 0;
	*mpul_Flags2 = 0;
	for (i = 0; i < 32; i++)
	{
		*mpul_Flags |= c_Value[ i + 1 ] << i;
		*mpul_Flags2 |= c_Value[ 33 + i ] << i;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_WPShowStatus::UpdateTexture(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC		                            *pDC;
	HBITMAP                             h_RGB, h_A, h_PalRGB, h_PalA;
	int		                            bpp, W, H, result;
    TEX_tdst_4Edit_CreateBitmapResult   st_Res;
    BITMAPINFO			                st_BitmapInfo;
    UINT				                ui_Usage;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    st_Res.ppc_Raw = (char **) &mpc_Raw;
    st_Res.ph_Alpha = &h_A;
    st_Res.ph_Colors = &h_RGB;
    st_Res.pi_Height = &H;
    st_Res.pi_Width = &W;
    st_Res.pst_Pal = &mst_Palette;
    st_Res.i_PalHeight = 16;
    st_Res.i_PalWidth = 16;
    st_Res.ph_PaletteColors = &h_PalRGB;
    st_Res.ph_PaletteAlpha = &h_PalA;
    st_Res.b_RawPalPrio = 1;

	pDC = GetDC();
	result = TEX_i_4Edit_CreateBitmap(mul_TextureIndex, mi_Slot, pDC->GetSafeHdc(), &st_Res );
	ReleaseDC(pDC);

	if (!result) return;

	DeleteBitmap();
	mh_BitmapRGB = h_RGB;
	mi_BmpWidth = W;
	mi_BmpHeight = H;

    mpo_DC = new CDC;
    mpo_DC->CreateCompatibleDC( GetDC() );
    mpo_DC->SelectObject(mh_BitmapRGB);

    mul_TgaIndex = mul_TextureIndex;
    mb_TexIndex = FALSE;
    mb_TexAlpha = (h_A) ? TRUE : FALSE;
    bpp = (h_A) ? 32 : 24;
    if (bpp == 24)
    {
        TEX_M_File_Realloc( mpc_Raw, W * H * 4, unsigned char );
        TEX_Convert_24To32( (ULONG *) mpc_Raw, W * H );
    }

    /* Create bitmap for color */
    L_memset( &st_BitmapInfo, 0, sizeof( BITMAPINFOHEADER ) );
    st_BitmapInfo.bmiHeader.biWidth = 128;
	st_BitmapInfo.bmiHeader.biHeight = 1;
	st_BitmapInfo.bmiHeader.biPlanes = 1;
	st_BitmapInfo.bmiHeader.biBitCount = 24;
	st_BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	st_BitmapInfo.bmiHeader.biCompression = BI_RGB;
    ui_Usage = DIB_RGB_COLORS;

    if ( st_Res.ul_RawKey != BIG_C_InvalidKey )
        mul_RawIndex = BIG_ul_SearchKeyToFat( st_Res.ul_RawKey );

	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_WPShowStatus::DeleteBitmap(void)
{
	if(mh_BitmapRGB)
	{
        delete mpo_DC;
		DeleteObject(mh_BitmapRGB);
		mh_BitmapRGB = NULL;
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_WPShowStatus::PreTranslateMessage(MSG *pMsg)
{
	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_WPShowStatus::OnOK(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_WPShowStatus::OnCancel(void)
{
	L_memcpy( c_Value, c_SaveValue, sizeof( c_Value ) );
	UpdateFlagsFromTable();
	LINK_Refresh();
	EDIA_cl_BaseDialog::OnCancel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_WPShowStatus::OnAll()
{
	*mpul_Flags = 0xFFFFFFFF;
	*mpul_Flags2 = 0xFFFFFFFF;
	UpdateTableFromFlags();
	LINK_Refresh();
	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_WPShowStatus::OnNone()
{
	*mpul_Flags = 0;
	*mpul_Flags2 = 0;
	UpdateTableFromFlags();
	LINK_Refresh();
	Invalidate();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_WPShowStatus::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC		    *pDC, o_PalDC;
    CPen        *pOldPen, o_WhitePen, o_BlackPen;
    int			dx, dy;
    int			x0, y0;
    CRect	    oRect;
    PAINTSTRUCT st_PS;
    int			i;
    int			X, Y;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pDC = BeginPaint( &st_PS );

    EDIA_cl_BaseDialog::OnPaint();

    o_WhitePen.CreatePen( PS_SOLID, 1, COLORREF( 0xFFFFFF ) );
    o_BlackPen.CreatePen( PS_SOLID, 1, COLORREF( 0 ) );
    pOldPen = pDC->SelectObject( &o_BlackPen );

	/* draw texture */
	dx = mo_DrawRect.Width();
	dy = mo_DrawRect.Height();
	x0 = mo_DrawRect.left;
	y0 = mo_DrawRect.top;
	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(x0, y0, dx, dy, mpo_DC, 0, 0, mi_BmpWidth, mi_BmpHeight, SRCCOPY);
	
	/* draw eventually truc */
	dx = mo_DrawRect.Width() / 8;
	dy = mo_DrawRect.Height() / 8;
	for (i = 0; i < 64; i++)
	{
		if ( c_Value[ i ] ) continue;
		
		X = i & 0x7;
		Y = i >> 3;
		
		x0 = mo_DrawRect.left + X * dx;
		y0 = mo_DrawRect.top + Y * dy;
		
		pDC->SelectObject( &o_WhitePen );
		pDC->MoveTo( x0+5, y0+7); pDC->LineTo( x0+dx-7, y0+dy-5 );
		pDC->MoveTo( x0+5, y0+6); pDC->LineTo( x0+dx-6, y0+dy-5 );
		pDC->MoveTo( x0+5, y0+5); pDC->LineTo( x0+dx-5, y0+dy-5 );
		pDC->MoveTo( x0+6, y0+5); pDC->LineTo( x0+dx-5, y0+dy-6 );
		pDC->MoveTo( x0+7, y0+5); pDC->LineTo( x0+dx-5, y0+dy-7 );
		
		pDC->MoveTo( x0+dx-6, y0+5); pDC->LineTo( x0+5, y0+dy-7 );
		pDC->MoveTo( x0+dx-6, y0+5); pDC->LineTo( x0+5, y0+dy-6 );
		pDC->MoveTo( x0+dx-5, y0+5); pDC->LineTo( x0+5, y0+dy-5 );
		pDC->MoveTo( x0+dx-5, y0+6); pDC->LineTo( x0+6, y0+dy-5 );
		pDC->MoveTo( x0+dx-5, y0+7); pDC->LineTo( x0+7, y0+dy-5 );
		
		pDC->SelectObject( &o_BlackPen );
		pDC->MoveTo( x0+6, y0+7); pDC->LineTo( x0+dx-7, y0+dy-6 );
		pDC->MoveTo( x0+6, y0+6); pDC->LineTo( x0+dx-6, y0+dy-6 );
		pDC->MoveTo( x0+7, y0+6); pDC->LineTo( x0+dx-6, y0+dy-7 );

		pDC->MoveTo( x0+dx-7, y0+6); pDC->LineTo( x0+6, y0+dy-7 );
		pDC->MoveTo( x0+dx-6, y0+6); pDC->LineTo( x0+6, y0+dy-6 );
		pDC->MoveTo( x0+dx-6, y0+7); pDC->LineTo( x0+7, y0+dy-6 );
	}

    pDC->SelectObject( pOldPen );
    o_WhitePen.DeleteObject();
    o_BlackPen.DeleteObject();

    EndPaint( &st_PS );
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_WPShowStatus::OnLButtonDown(UINT ui, CPoint o_Pt)
{
	int i, x, y;
	
	if ( mo_DrawRect.PtInRect(o_Pt ) )
	{
		x = ((o_Pt.x - 5) * 8) / mo_DrawRect.Width();
		y = ((o_Pt.y - 5) * 8) / mo_DrawRect.Height();
		
		i = y * 8 + x;
		if (i == 0)
			return;
		
		
		c_Value[ i ] = c_Value[ i ] ? 0 : 1;
		
		Invalidate();
		UpdateFlagsFromTable();
		LINK_Refresh();
	}
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
void EDIA_cl_WPShowStatus::ComputeDrawRect(void)
{
	GetClientRect(&mo_DrawRect[0]);

	mo_DrawRect[0].left = 5;
	mo_DrawRect[0].right -= 5;
	mo_DrawRect[0].top = 5;
	mo_DrawRect[0].bottom = mo_DrawRect[0].top + (mo_DrawRect[0].right - mo_DrawRect[0].left );
}


#endif /* ACTIVE_EDITORS */
