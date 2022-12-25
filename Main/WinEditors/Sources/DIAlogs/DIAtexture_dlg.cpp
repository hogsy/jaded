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
#include "DIAlogs/DIAtexture_dlg.h"
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
#include "TEXture/TEXfile.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEXeditorfct.h"
#include "DIAlogs/DIACOLOR_dlg.h"

#define M_SwapRB( C )   (((C & 0xFF0000) >> 16) | (C & 0xFF00FF00) | ((C & 0xFF) << 16))

#define     EDIATEXTURE_ColorRect       2
#define     EDIATEXTURE_Move0           4
#define     EDIATEXTURE_MoveBis         6
#define     EDIATEXTURE_PaletteRect0    8
#define     EDIATEXTURE_PaletteRect1    9
#define     EDIATEXTURE_PickColor0      10
#define     EDIATEXTURE_PickColor1      11
#define     EDIATEXTURE_PickColor0Bis   12
#define     EDIATEXTURE_PickColor1Bis   13
#define     EDIATEXTURE_Paint0          14
#define     EDIATEXTURE_Paint1          15
#define     EDIATEXTURE_Paint0Bis       16
#define     EDIATEXTURE_Paint1Bis       17
#define     EDIATEXTURE_ColorCursorR    18
#define     EDIATEXTURE_ColorCursorG    20
#define     EDIATEXTURE_ColorCursorB    22
#define     EDIATEXTURE_ColorCursorA    24

/*
 =======================================================================================================================
    class CColorButton
 =======================================================================================================================
 */
class CColorButton : public CButton
{
public:
    void			DrawItem(LPDRAWITEMSTRUCT);
};

void CColorButton::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
    CDC     *pDC;
    CPoint  o_Pt;
    CFont   *po_Font;
    CString o_Str;

    pDC = GetDC();
    
    //pDC->Draw3dRect( &lpDIS->rcItem, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DDKSHADOW) );
    pDC->FillSolidRect( &lpDIS->rcItem, GetSysColor(COLOR_3DFACE) );
    pDC->Draw3dRect( &lpDIS->rcItem, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHILIGHT) );

    o_Pt.x = (lpDIS->rcItem.left + lpDIS->rcItem.right) >> 1;
    o_Pt.y = lpDIS->rcItem.top + 2;
    pDC->SetTextAlign( TA_CENTER );
    pDC->SetBkMode( TRANSPARENT );
    po_Font = pDC->SelectObject(&M_MF()->mo_Fnt2);
    GetWindowText( o_Str );
    pDC->ExtTextOut( o_Pt.x, o_Pt.y, ETO_CLIPPED, &lpDIS->rcItem, o_Str, NULL );

    pDC->SelectObject( po_Font );
    ReleaseDC( pDC );
}

/*
 =======================================================================================================================
    MESSAGE MAP
 =======================================================================================================================
 */
BEGIN_MESSAGE_MAP(EDIA_cl_Texture, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_SWAPCOL, OnSwapColor )
    ON_BN_CLICKED(IDC_CHECK_ACOL, OnCheck_PaletteAlpha )
    ON_BN_CLICKED(IDC_CHECK_RGBCOL, OnCheck_PaletteRGB )
    ON_BN_CLICKED(IDC_CHECK_RGB, OnCheck_RGB )
    ON_BN_CLICKED(IDC_CHECK_ALPHA, OnCheck_A )
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_Texture::EDIA_cl_Texture(ULONG _ul_Index, int _i_Slot) :	EDIA_cl_BaseDialog(DIALOGS_IDD_TEXTURE )
{
	mul_TextureIndex = _ul_Index;
	mi_Slot = _i_Slot;

    mh_BitmapRGB = 0;
    mh_BitmapA = 0;
    mpo_DC[0] = mpo_DC[1] = NULL;

	mi_Picked = -1;

    mi_X = 0;
    mi_Y = 0;
    mi_Zoom = 1;
    mi_Capture = 0;

    mi_CurColor = 0;
    mi_ColorIndex[ 0 ] = 1;
    mi_ColorIndex[ 1 ] = 0;
    mul_Color[ 0 ] = 0xFFFFFFFF;
    mul_ColorInv[ 0 ] = 0xFFFFFFFF;
    mul_Color[ 1 ] = 0;
    mul_ColorInv[ 1 ] = 0;
    mo_ColorRect[0] = CRect( 1, 11, 16, 67 );
    mo_ColorRect[1] = CRect( 1, 75, 16, 131 );

    mb_ShowRGB = TRUE;
    mb_ShowA = FALSE;

    mi_CurRect = mi_CurX = mi_CurY = -1;

    mi_StartEdit = 1;
    mi_ValueEdit = mi_CurValueEdit = 0;

    ul_RGBAColor = 0;
    
    mul_RawIndex = mul_TgaIndex = BIG_C_InvalidIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_Texture::~EDIA_cl_Texture(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_Texture::OnInitDialog(void)
{
    BOOL    b_Res;
    UpdateTexture();
    UpdateRGBAValue();
	ComputeDrawRect();

    mpo_ColorButton[ 0 ] = new CColorButton;
    mpo_ColorButton[ 1 ] = new CColorButton;
    mpo_ColorButton[ 2 ] = new CColorButton;
    mpo_ColorButton[ 3 ] = new CColorButton;
    mpo_ColorButton[ 4 ] = new CColorButton;

    b_Res = mpo_ColorButton[ 0 ]->SubclassDlgItem( IDC_CHECK_I, this );
    b_Res = mpo_ColorButton[ 1 ]->SubclassDlgItem( IDC_CHECK_R, this );
    b_Res = mpo_ColorButton[ 2 ]->SubclassDlgItem( IDC_CHECK_G, this );
    b_Res = mpo_ColorButton[ 3 ]->SubclassDlgItem( IDC_CHECK_B, this );
    b_Res = mpo_ColorButton[ 4 ]->SubclassDlgItem( IDC_CHECK_A, this );

    b_Res = mpo_ColorButton[0]->ModifyStyle( 0, BS_OWNERDRAW, 0 );
    b_Res = mpo_ColorButton[1]->ModifyStyle( 0, BS_OWNERDRAW, 0 );
    b_Res = mpo_ColorButton[2]->ModifyStyle( 0, BS_OWNERDRAW, 0 );
    b_Res = mpo_ColorButton[3]->ModifyStyle( 0, BS_OWNERDRAW, 0 );
    b_Res = mpo_ColorButton[4]->ModifyStyle( 0, BS_OWNERDRAW, 0 );

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::UpdateTexture(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC		                            *pDC;
	HBITMAP                             h_RGB, h_A, h_PalRGB, h_PalA;
	int		                            i, bpp, W, H, result;
    TEX_tdst_4Edit_CreateBitmapResult   st_Res;
    char                                sz_Title[ 1024 ];
    UCHAR                               *p_Bitmap;
    BITMAPINFO			                st_BitmapInfo;
    UINT				                ui_Usage;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset( &mst_Palette, 0, sizeof( TEX_tdst_Palette ) );

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
    mh_BitmapA = h_A;
    mh_PaletteRGB = h_PalRGB;
    mh_PaletteA = h_PalA;
	mi_BmpWidth = W;
	mi_BmpHeight = H;

    mpo_DC[0] = new CDC;
    mpo_DC[0]->CreateCompatibleDC( GetDC() );
    mpo_DC[0]->SelectObject(mh_BitmapRGB);

    if (mh_BitmapA)
    {
        mpo_DC[1] = new CDC;
        mpo_DC[1]->CreateCompatibleDC( GetDC() );
        mpo_DC[1]->SelectObject(mh_BitmapA);
    }
    else
        mpo_DC[1] = NULL;

    if (mst_Palette.ul_Key !=  BIG_C_InvalidKey)
    {
        mb_TexIndex = TRUE;
        mb_TexAlpha = (mst_Palette.uc_Flags & TEX_uc_AlphaPalette) ? TRUE : FALSE;
        bpp = (mst_Palette.uc_Flags & TEX_uc_Palette16) ? 4 : 8;
        mul_Color[ 0 ] = mst_Palette.pul_Color[ mi_ColorIndex [0] ];
        mul_ColorInv[ 0 ] = M_SwapRB( mul_Color[ 0 ] );
        mul_Color[ 1 ] = mst_Palette.pul_Color[ mi_ColorIndex [1] ];
        mul_ColorInv[ 1 ] = M_SwapRB( mul_Color[ 1 ] );

        if (mst_Palette.uc_Flags & TEX_uc_Palette16)
        {
            p_Bitmap = mpc_Raw;
            TEX_M_File_Alloc( mpc_Raw, W * H, unsigned char );
            TEX_Convert_4To8( mpc_Raw, p_Bitmap, W, H );
            TEX_M_File_Free( p_Bitmap );
            //mpc_Raw = p_Bitmap;
        }
    }
    else
    {
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

        mh_RGBABmp[0] = CreateDIBSection(pDC->GetSafeHdc(), &st_BitmapInfo, ui_Usage, (void **) &p_Bitmap, NULL, 0);
        for (i = 0; i < 128; i++ )
        {
            *p_Bitmap++ = 0;
            *p_Bitmap++ = 0;
            *p_Bitmap++ = (i << 1);
        }
        mpo_RGBADC[0] = new CDC;
        mpo_RGBADC[0]->CreateCompatibleDC( GetDC() );
        mpo_RGBADC[0]->SelectObject(mh_RGBABmp[0]);

        mh_RGBABmp[1] = CreateDIBSection(pDC->GetSafeHdc(), &st_BitmapInfo, ui_Usage, (void **) &p_Bitmap, NULL, 0);
        for (i = 0; i < 128; i++ )
        {
            *p_Bitmap++ = 0;
            *p_Bitmap++ = (i << 1);
            *p_Bitmap++ = 0;
        }
        mpo_RGBADC[1] = new CDC;
        mpo_RGBADC[1]->CreateCompatibleDC( GetDC() );
        mpo_RGBADC[1]->SelectObject(mh_RGBABmp[1]);

        mh_RGBABmp[2] = CreateDIBSection(pDC->GetSafeHdc(), &st_BitmapInfo, ui_Usage, (void **) &p_Bitmap, NULL, 0);
        for (i = 0; i < 128; i++ )
        {
            *p_Bitmap++ = (i << 1);
            *p_Bitmap++ = 0;
            *p_Bitmap++ = 0;
        }
        mpo_RGBADC[2] = new CDC;
        mpo_RGBADC[2]->CreateCompatibleDC( GetDC() );
        mpo_RGBADC[2]->SelectObject(mh_RGBABmp[2]);

        mh_RGBABmp[3] = CreateDIBSection(pDC->GetSafeHdc(), &st_BitmapInfo, ui_Usage, (void **) &p_Bitmap, NULL, 0);
        for (i = 0; i < 128; i++ )
        {
            *p_Bitmap++ = (i << 1);
            *p_Bitmap++ = (i << 1);
            *p_Bitmap++ = (i << 1);
        }
        mpo_RGBADC[3] = new CDC;
        mpo_RGBADC[3]->CreateCompatibleDC( GetDC() );
        mpo_RGBADC[3]->SelectObject(mh_RGBABmp[3]);
    }

    if ( st_Res.ul_RawKey != BIG_C_InvalidKey )
        mul_RawIndex = BIG_ul_SearchKeyToFat( st_Res.ul_RawKey );

    GetDlgItem( IDOK )->EnableWindow( ( ( mul_RawIndex == BIG_C_InvalidIndex ) && (mul_TgaIndex == BIG_C_InvalidIndex)) ? FALSE : TRUE );
    ((CButton *) GetDlgItem( IDC_CHECK_RGBCOL ))->SetCheck( 1 );
    ((CButton *) GetDlgItem( IDC_CHECK_ACOL ))->SetCheck( 0 );
    GetDlgItem( IDC_CHECK_ACOL )->EnableWindow( mb_TexAlpha );
    mb_PalShowAlpha = FALSE;

    ((CButton *) GetDlgItem( IDC_CHECK_RGB ))->SetCheck( 1 );
    GetDlgItem( IDC_CHECK_ALPHA )->EnableWindow( mb_TexAlpha );

    sprintf( sz_Title, "Texture %s, %d bpp, %d x %d", BIG_NameFile( mul_TextureIndex ), bpp, W, H );
    SetWindowText( sz_Title );

    UpdateRGBAValue();
	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::DeleteBitmap(void)
{
	if(mh_BitmapRGB)
	{
        delete mpo_DC[0];
		DeleteObject(mh_BitmapRGB);
		mh_BitmapRGB = NULL;
	}

	if(mh_BitmapA)
	{
        delete mpo_DC[1];
		DeleteObject(mh_BitmapA);
		mh_BitmapA = NULL;
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_Texture::PreTranslateMessage(MSG *pMsg)
{
    if(pMsg->message == WM_MOUSEWHEEL)
	{
		if(WheelZoom((short) HIWORD(pMsg->wParam))) return 1;
	}
	else if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE) return 1;
		OnKeyDown(pMsg->wParam, LOWORD( pMsg->lParam), HIWORD( pMsg->lParam) );
	}
    else if ( pMsg->message == WM_MOUSEMOVE )
    {
        if ( mi_Capture == 0)
        {
            mi_ValueEdit = 0;

            if (mb_TexIndex)
            {
                if (pMsg->hwnd == GetDlgItem( IDC_CHECK_I )->GetSafeHwnd() )
                    mi_ValueEdit = 1;
            }
            else
            {
                if (pMsg->hwnd == GetDlgItem( IDC_CHECK_R )->GetSafeHwnd() )
                    mi_ValueEdit = 2;
                else if (pMsg->hwnd == GetDlgItem( IDC_CHECK_G )->GetSafeHwnd() )
                    mi_ValueEdit = 3;
                else if (pMsg->hwnd == GetDlgItem( IDC_CHECK_B )->GetSafeHwnd() )
                    mi_ValueEdit = 4;
                else if (pMsg->hwnd == GetDlgItem( IDC_CHECK_A )->GetSafeHwnd() )
                    if (mb_TexAlpha) mi_ValueEdit = 5;
            }
            if (mi_ValueEdit == 0)
                mi_StartEdit = 1;
        }
    }

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnOK(void)
{
    char                sz_Path[ 260 ];
    TEX_tdst_File_Desc  st_Desc;
    UCHAR               *p_Bitmap;

    if ( mul_RawIndex != BIG_C_InvalidIndex )
    {
        st_Desc.p_Bitmap = mpc_Raw;
        st_Desc.uc_BPP = (mst_Palette.uc_Flags & TEX_uc_Palette16) ? 4 : 8;
        st_Desc.uw_Height = mi_BmpHeight;
        st_Desc.uw_Width = mi_BmpWidth;

        if ( st_Desc.uc_BPP == 4)
        {
            p_Bitmap = (UCHAR *) L_malloc( (mi_BmpHeight * mi_BmpWidth) >> 1 );
            st_Desc.p_Bitmap = p_Bitmap;
            TEX_Convert_8To4( p_Bitmap, mpc_Raw, mi_BmpWidth, mi_BmpHeight );
        }

        BIG_ComputeFullName( BIG_ParentFile( mul_RawIndex ), sz_Path );
        TEX_ul_File_SaveRawInBF( sz_Path, BIG_NameFile( mul_RawIndex ), &st_Desc, FALSE);

        if ( st_Desc.uc_BPP == 4)
            L_free( p_Bitmap );
    }
    else if (mul_TgaIndex != BIG_C_InvalidIndex )
    {
        st_Desc.p_Bitmap = mpc_Raw;
        st_Desc.uc_BPP = mb_TexAlpha ? 32 : 24;
        st_Desc.uw_Height = mi_BmpHeight;
        st_Desc.uw_Width = mi_BmpWidth;
        st_Desc.p_Palette = NULL;
        st_Desc.uw_PaletteLength = 0;
	    st_Desc.uc_PaletteBPC = 0;

        if ( st_Desc.uc_BPP == 24)
        {
            p_Bitmap = (UCHAR *) L_malloc( (mi_BmpHeight * mi_BmpWidth) << 2 );
            st_Desc.p_Bitmap = p_Bitmap;
            L_memcpy( p_Bitmap, mpc_Raw, (mi_BmpHeight * mi_BmpWidth) * 4 );
            TEX_Convert_32To24( (ULONG *)p_Bitmap, mi_BmpHeight * mi_BmpWidth );
        }

        BIG_ComputeFullName( BIG_ParentFile( mul_TgaIndex ), sz_Path );
        TEX_ul_File_SaveTgaInBF( sz_Path, BIG_NameFile( mul_TgaIndex ), &st_Desc  );

        if ( st_Desc.uc_BPP == 24)
            L_free( p_Bitmap );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnCancel(void)
{
	EDIA_cl_BaseDialog::OnCancel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 410;
	lpMMI->ptMinTrackSize.y = 310;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnSize(UINT n, int x, int y)
{
	ComputeDrawRect();
	InvalidateRect(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_Texture::OnEraseBkgnd(CDC *pdc)
{
	/*~~~~~~~~~~*/
	DWORD	x_Col;
	CRect	oRect;
	/*~~~~~~~~~~*/

	x_Col = GetSysColor(COLOR_BTNFACE);
	GetClientRect(&oRect);

	pdc->FillSolidRect(0, 0, mo_DrawRect[0].left, oRect.bottom, x_Col);
    pdc->FillSolidRect(mo_DrawRect[0].right, 0, oRect.right, oRect.bottom, x_Col);
	pdc->FillSolidRect(mo_DrawRect[0].left, 0, mo_DrawRect[0].right, mo_DrawRect[0].top, x_Col);
	pdc->FillSolidRect(mo_DrawRect[0].left, mo_DrawRect[0].bottom, mo_DrawRect[0].right, oRect.bottom, x_Col);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC		    *pDC, o_PalDC;
    CPen        *pOldPen, o_WhitePen, o_BlackPen;
    HBITMAP		h_Bmp;
    int			xmin, ymin;
    int			i, w, h, dx, dy;
    int			x0, y0, x1, y1;
    DWORD	    x_Col;
    CRect	    oRect;
    PAINTSTRUCT st_PS;
    BOOL        b_Res;
    ULONG       C;
    div_t       st_Div;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pDC = BeginPaint( &st_PS );

    EDIA_cl_BaseDialog::OnPaint();

    o_WhitePen.CreatePen( PS_SOLID, 1, COLORREF( 0xFFFFFF ) );
    o_BlackPen.CreatePen( PS_SOLID, 1, COLORREF( 0 ) );
    pOldPen = pDC->SelectObject( &o_BlackPen );

    if (mb_ShowA && mb_ShowRGB )
    {
        mpo_DrawRect[0] = &mo_DrawRect[1];
        mpo_DrawRect[1] = &mo_DrawRect[2];

        oRect = mo_DrawRect[ 0 ];
        oRect.top = mo_DrawRect[1].bottom;
        oRect.bottom = mo_DrawRect[2].top;
       	
	    x_Col = GetSysColor(COLOR_BTNFACE);
        pDC->FillSolidRect( &oRect, x_Col );
    }
    else if (mb_ShowA )
    {
        mpo_DrawRect[0] = NULL;
        mpo_DrawRect[1] = &mo_DrawRect[0];
    }
    else
    {
        mpo_DrawRect[0] = &mo_DrawRect[0];
        mpo_DrawRect[1] = NULL;
    }

    for ( i = 0; i < 2; i++)
    {
        if ( !mpo_DrawRect[ i ] ) continue;

        xmin = mi_X;
		ymin = mi_Y;
		w = mi_BmpWidth - xmin;
		h = mi_BmpHeight - ymin;

		dx = mpo_DrawRect[i]->Width();
		dy = mpo_DrawRect[i]->Height();

		w *= mi_Zoom;
		h *= mi_Zoom;
		if(w > dx) w = dx;
		if(h > dy) h = dy;
		w /= mi_Zoom;
		dx = w * mi_Zoom;
		h /= mi_Zoom;
		dy = h * mi_Zoom;
		
		x0 = mpo_DrawRect[i]->left + (mpo_DrawRect[i]->Width() - dx) / 2;
		y0 = mpo_DrawRect[i]->top + (mpo_DrawRect[i]->Height() - dy) / 2;

		pDC->SetStretchBltMode(COLORONCOLOR);
		pDC->StretchBlt(x0, y0, dx, dy, mpo_DC[i], xmin, ymin, w, h, SRCCOPY);

		x1 = x0 + dx;
        y1 = y0 + dy;
        if(x0 > mpo_DrawRect[i]->left) pDC->FillSolidRect(mpo_DrawRect[i]->left, mpo_DrawRect[i]->top, x0 - mpo_DrawRect[i]->left, mpo_DrawRect[i]->Height(), 0);
		if(x1 < mpo_DrawRect[i]->right) pDC->FillSolidRect(x1, mpo_DrawRect[i]->top, mpo_DrawRect[i]->right - x1, mpo_DrawRect[i]->Height(), 0);
        if(y0 > mpo_DrawRect[i]->top) pDC->FillSolidRect(x0, mpo_DrawRect[i]->top, x1 - x0, y0 - mpo_DrawRect[i]->top, 0);
		if(y1 < mpo_DrawRect[i]->bottom) pDC->FillSolidRect(x0, y1, x1 - x0, mpo_DrawRect[i]->bottom - y1, 0);

        mo_RealDrawRect[i] = CRect( x0, y0, x1, y1 );
	}

    if (mb_TexIndex)
    {
        h_Bmp = (mb_PalShowAlpha) ? mh_PaletteA : mh_PaletteRGB;
        if ( h_Bmp )
        {
            o_PalDC.CreateCompatibleDC(pDC);
		    o_PalDC.SelectObject(h_Bmp);
		    pDC->SetStretchBltMode(COLORONCOLOR);
            x0 = mo_PalDrawRect.left;
            y0 = mo_PalDrawRect.top;
            dx = mo_PalDrawRect.Width();
            dy = mo_PalDrawRect.Height();
		    pDC->StretchBlt(x0, y0, dx, dy, &o_PalDC, 0, 0, 16, 16, SRCCOPY);

            if (mst_Palette.uc_Flags & TEX_uc_Palette16)
            {
                st_Div = div(mi_ColorIndex[ mi_CurColor ], 4 );
                x0 = mo_PalDrawRect.left + (mo_PalDrawRect.Width() * st_Div.rem) / 4;
                x1 = (mo_PalDrawRect.left - 1) + (mo_PalDrawRect.Width() * (st_Div.rem + 1) ) / 4;
                y0 = mo_PalDrawRect.top + (mo_PalDrawRect.Height() * st_Div.quot) / 4;
                y1 = (mo_PalDrawRect.top - 1) + (mo_PalDrawRect.Height() * (st_Div.quot + 1) ) / 4;
            }
            else
            {
                st_Div = div(mi_ColorIndex[ mi_CurColor ], 16 );
                x0 = mo_PalDrawRect.left + (mo_PalDrawRect.Width() * st_Div.rem) / 16;
                x1 = (mo_PalDrawRect.left - 1) + (mo_PalDrawRect.Width() * (st_Div.rem + 1) ) / 16;
                y0 = mo_PalDrawRect.top + (mo_PalDrawRect.Height() * st_Div.quot) / 16;
                y1 = (mo_PalDrawRect.top - 1) + (mo_PalDrawRect.Height() * (st_Div.quot + 1) ) / 16;
            }
            
            dx = mul_Color[ mi_CurColor ];
            if ( (dx & 0x80) || (dx & 0x8000) || (dx & 0x800000) )
                pDC->SelectObject( &o_BlackPen );
            else
                pDC->SelectObject( &o_WhitePen );
            pDC->MoveTo( x0, y0 );
            pDC->LineTo( x1, y0 );
            pDC->LineTo( x1, y1 );
            pDC->LineTo( x0, y1 );
            pDC->LineTo( x0, y0 );
        }
    }
    else
    {
        if ( (mul_ColorInv[ mi_CurColor ] & 0xFFFFFF) != (ul_RGBAColor & 0xFFFFFF) )
        {
            BITMAP  st_Bmp;
            UCHAR   *p_Bitmap, R, G, B;

            ul_RGBAColor = mul_ColorInv[ mi_CurColor ];
            R = (UCHAR) (ul_RGBAColor & 0xFF);
            G = (UCHAR) ((ul_RGBAColor >> 8) & 0xFF);
            B = (UCHAR) ((ul_RGBAColor >> 16) & 0xFF);
            GetObject( mh_RGBABmp[0], sizeof( BITMAP ), &st_Bmp );
            p_Bitmap = (UCHAR *) st_Bmp.bmBits;
            for (x0 = 0; x0 < 128; x0++)
            {
                *p_Bitmap++ = B;
                *p_Bitmap++ = G;
                p_Bitmap++;
            }

            GetObject( mh_RGBABmp[1], sizeof( BITMAP ), &st_Bmp );
            p_Bitmap = (UCHAR *) st_Bmp.bmBits;
            for (x0 = 0; x0 < 128; x0++)
            {
                *p_Bitmap++ = B;
                p_Bitmap++;
                *p_Bitmap++ = R;
            }

            GetObject( mh_RGBABmp[2], sizeof( BITMAP ), &st_Bmp );
            p_Bitmap = (UCHAR *) st_Bmp.bmBits;
            for (x0 = 0; x0 < 128; x0++)
            {
                p_Bitmap++;
                *p_Bitmap++ = G;
                *p_Bitmap++ = R;
            }
        }
        ul_RGBAColor = mul_ColorInv[ mi_CurColor ];

        C = GetSysColor( COLOR_3DFACE );
        x0 = mo_PalDrawRect.left;
        dx = mo_PalDrawRect.Width();
        pDC->FillSolidRect( x0, mo_PalDrawRect.top, dx, mo_RGBARect[0].top - mo_PalDrawRect.top, C );
        pDC->FillSolidRect( x0, mo_RGBARect[0].bottom, dx, mo_RGBARect[1].top - mo_RGBARect[0].bottom, C );
        pDC->FillSolidRect( x0, mo_RGBARect[1].bottom, dx, mo_RGBARect[2].top - mo_RGBARect[1].bottom, C );
        if (mb_TexAlpha)
        {
            pDC->FillSolidRect( x0, mo_RGBARect[2].bottom, dx, mo_RGBARect[2].top - mo_RGBARect[3].bottom, C );
            pDC->FillSolidRect( x0, mo_RGBARect[3].bottom, dx, mo_PalDrawRect.bottom - mo_RGBARect[3].bottom, C );
        }
        else
        {
            pDC->FillSolidRect( x0, mo_RGBARect[2].bottom, dx, mo_PalDrawRect.bottom - mo_RGBARect[2].bottom, C );
        }

        pDC->SetStretchBltMode(COLORONCOLOR);
        y1 = ul_RGBAColor;
        for ( i = 0; i < 3 + (mb_TexAlpha ? 1 : 0) ; i++)
        {
            if ( (i == 3) && !mb_TexAlpha) break;
            x0 = mo_RGBARect[i].left;
            y0 = mo_RGBARect[i].top;
            dx = mo_RGBARect[i].Width();
            dy = mo_RGBARect[i].Height();
		    b_Res = pDC->StretchBlt(x0, y0, dx, dy, mpo_RGBADC[i], 0, 0, 128, 1, SRCCOPY);

            pDC->Draw3dRect( &mo_RGBARect[i], GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHILIGHT) );

            x0 = mo_RGBARect[i].left + (((y1 & 0xFF) * mo_RGBARect[i].Width() ) >> 8);
            y1 >>= 8;
            y0 = mo_RGBARect[i].bottom + 1;
            pDC->MoveTo( x0, y0 );
            pDC->LineTo( x0 + 3, y0 + 5 );
            pDC->LineTo( x0 - 3, y0 + 5 );
            pDC->LineTo( x0, y0 );
        }
    }

    for (x0 = 0; x0 <= 1; x0++ )
    {
        pDC->FillSolidRect( mo_ColorRect[ x0 ].left, mo_ColorRect[x0].top, 6, mo_ColorRect[x0].Height(),GetSysColor(COLOR_3DFACE)  );
        mo_ColorRect[x0].left += 6;
        pDC->FillSolidRect( &mo_ColorRect[ x0 ], mul_ColorInv[ x0 ] & 0xFFFFFF);
        pDC->Draw3dRect( &mo_ColorRect[ x0 ], GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DHILIGHT) );
        
        
        if (x0 == mi_CurColor)
        {
            x1 = mo_ColorRect[ x0 ].left - 1;
            y1 = mo_ColorRect[ x0 ].CenterPoint().y;
            pDC->MoveTo( x1, y1 );
            pDC->LineTo( x1 - 5, y1 + 3 );
            pDC->LineTo( x1 - 5, y1 -3 );
            pDC->LineTo( x1, y1 );
        }
        
        mo_ColorRect[x0].left -= 6;

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
/*
void EDIA_cl_Texture::PaintTexture( void )
{
}
*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_Texture::WheelZoom(int i_Delta)
{
	/*~~~~~~~~~*/
	CPoint	o_Pt;
	int		i, X, Y;
	/*~~~~~~~~~*/

	GetCursorPos(&o_Pt);
	ScreenToClient(&o_Pt);

    if(GetAsyncKeyState(VK_SPACE) < 0)
    {
        for (i = 0; i < 2; i++)
        {
            if ( !mpo_DrawRect[i]) continue;
            if ( !mo_RealDrawRect[i].PtInRect( o_Pt ) ) continue;

            X = ((o_Pt.x - mpo_DrawRect[i]->left) / mi_Zoom ) + mi_X;
		    Y = ((o_Pt.y - mpo_DrawRect[i]->top) / mi_Zoom ) + mi_Y;

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

		    mi_X = X - (o_Pt.x - mpo_DrawRect[i]->left) / mi_Zoom;
		    mi_Y = Y - (o_Pt.y - mpo_DrawRect[i]->top) / mi_Zoom;

		    X = (mi_BmpWidth - mi_X) * mi_Zoom;
		    if(X < mpo_DrawRect[i]->Width()) mi_X = mi_BmpWidth - (mpo_DrawRect[i]->Width() / mi_Zoom);
		    Y = (mi_BmpWidth - mi_Y) * mi_Zoom;
		    if(Y < mpo_DrawRect[i]->Height()) mi_Y = mi_BmpHeight - (mpo_DrawRect[i]->Height() / mi_Zoom );

		    if(mi_X < 0) mi_X = 0;
		    if(mi_Y < 0) mi_Y = 0;

		    InvalidateRect(NULL, FALSE);
            return 1;
        }
    }
	
    /*
    if(!mo_DrawRect[0].PtInRect(o_Pt))
		return 0;
	else if(GetAsyncKeyState(VK_SPACE) < 0)
	{
		X = ((o_Pt.x - mo_DrawRect.left) / mi_Zoom ) + mi_X;
		Y = ((o_Pt.y - mo_DrawRect.top) / mi_Zoom ) + mi_Y;

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

		mi_X = X - (o_Pt.x - mo_DrawRect.left) / mi_Zoom;
		mi_Y = Y - (o_Pt.y - mo_DrawRect.top) / mi_Zoom;

		X = (mi_BmpWidth - mi_X) * mi_Zoom;
		if(X < mo_DrawRect.Width()) mi_X = mi_BmpWidth - (mo_DrawRect.Width() / mi_Zoom);
		Y = (mi_BmpWidth - mi_Y) * mi_Zoom;
		if(Y < mo_DrawRect.Height()) mi_Y = mi_BmpHeight - (mo_DrawRect.Height() / mi_Zoom );

		if(mi_X < 0) mi_X = 0;
		if(mi_Y < 0) mi_Y = 0;

		InvalidateRect(NULL, FALSE);
	}
    */

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnMouseMove(UINT ui_Flags, CPoint o_Pt)
{
    int					i, x, y, DX, DY;

    if (mi_Capture == 0) 
    {
        if (!mi_ValueEdit)
            UpdateCoord( o_Pt );
        return;
    }

    if ( mi_Capture & 1 )
    {
        if ( !(ui_Flags & MK_RBUTTON) )
        {
            mi_Capture = 0;
            ReleaseCapture();
            return;
        }
    }
    else
    {
        if ( !(ui_Flags & MK_LBUTTON) )
        {
            mi_Capture = 0;
            ReleaseCapture();
            return;
        }
    }

    switch( mi_Capture )
    {
    case EDIATEXTURE_ColorRect:
        break;
    case EDIATEXTURE_Move0:
    case EDIATEXTURE_MoveBis:
        i = (mi_Capture - EDIATEXTURE_Move0) >> 1;

		DX = o_Pt.x - mo_CapturePt.x;
		if( (DX >= mi_Zoom) || (-DX >= mi_Zoom) )
		{
			DX /= mi_Zoom;
			x = (mi_BmpWidth - mi_X + DX) * mi_Zoom;
			if(x < mpo_DrawRect[i]->Width())
			    mi_X = mi_BmpWidth - (mpo_DrawRect[i]->Width() / mi_Zoom);
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
			if(y < mpo_DrawRect[i]->Height())
			    mi_Y = mi_BmpHeight - (mpo_DrawRect[i]->Height() / mi_Zoom);
			else
				mi_Y -= DY;
			if(mi_Y < 0) mi_Y = 0;
			mo_CapturePt.y = o_Pt.y;
		}

		InvalidateRect(NULL, FALSE);
        return;
    case EDIATEXTURE_PaletteRect0:
        PickPalColor( o_Pt, 0 );
        break;
    case EDIATEXTURE_PaletteRect1:
        PickPalColor( o_Pt, 1 );
        break;
    case EDIATEXTURE_PickColor0:
        PickColor( o_Pt, 0 );
        break;
    case EDIATEXTURE_PickColor1:
        PickColor( o_Pt, 1 );
        break;
    case EDIATEXTURE_PickColor0Bis:
        PickColor( o_Pt, 0 );
        break;
    case EDIATEXTURE_PickColor1Bis:
        PickColor( o_Pt, 1 );
        break;
    case EDIATEXTURE_Paint0:
        Paint( o_Pt, 0, 0, FALSE);
        break;
    case EDIATEXTURE_Paint1:
        Paint( o_Pt, 1, 0, FALSE);
        break;
    case EDIATEXTURE_Paint0Bis:
        Paint( o_Pt, 0, 1, FALSE);
        break;
    case EDIATEXTURE_Paint1Bis:
        Paint( o_Pt, 1, 1, FALSE);
        break;
    case EDIATEXTURE_ColorCursorR:
    case EDIATEXTURE_ColorCursorG:
    case EDIATEXTURE_ColorCursorB:
    case EDIATEXTURE_ColorCursorA:
        i = (mi_Capture - EDIATEXTURE_ColorCursorR) >> 1;
        x = ((o_Pt.x - mo_RGBARect[i].left) * 255) / mo_RGBARect[i].Width();
        if (x < 0) x = 0;
        else if (x > 255) x = 255;
        mul_ColorInv[ mi_CurColor ] &= ~( 0xFF << (8*i) );
        mul_ColorInv[ mi_CurColor ] |= (x << (i * 8));
        mul_Color[ mi_CurColor ] = M_SwapRB( mul_ColorInv[ mi_CurColor ] );
        UpdateRGBAValue();
        InvalidateRect( mo_ColorRect[0] );
        InvalidateRect( mo_ColorRect[1] );
        InvalidateRect( mo_PalDrawRect );
        break;
    }

    UpdateCoord( o_Pt );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnLButtonDown(UINT ui, CPoint o_Pt)
{
    int     i;
    BOOL    b_InRect;
    float   f;
    ULONG   ul_Color;

    if(mi_Capture) return;

    for (i = 0; i < 2; i++)
    {
        if (mo_ColorRect[i].PtInRect( o_Pt ) )
        {
            mi_CurColor = i;
            UpdateRGBAValue();
            InvalidateRect( NULL, FALSE );
            mi_Capture = EDIATEXTURE_ColorRect;
            SetCapture();
            return;
        }
    }

    if (mb_TexIndex)
    {
        if ( mo_PalDrawRect.PtInRect( o_Pt ) )
        {
            PickPalColor( o_Pt, 0 );
            mi_Capture = EDIATEXTURE_PaletteRect0;
            SetCapture();
            return;
        }
    }
    else
    {
        for ( i = 0; i < 3 + (mb_TexAlpha ? 1 : 0) ; i++)
        {
            mo_RGBARect[i].bottom += 7;
            b_InRect = mo_RGBARect[i].PtInRect( o_Pt );
            mo_RGBARect[i].bottom -= 7;
            if (b_InRect)
            {
                f = ((float) (o_Pt.x - mo_RGBARect[i].left))/((float) mo_RGBARect[i].Width());
                ul_Color = (f < 0) ? 0 : ((f >= 1) ? 0xFF : (int) (f * 255.0f) );
                ul_Color <<= (8 * i);
                mul_ColorInv[ mi_CurColor ] &= ~(0xFF << (8 * i));
                mul_ColorInv[ mi_CurColor ] |= ul_Color;
                mul_Color[ mi_CurColor ] = M_SwapRB( mul_ColorInv[ mi_CurColor ] );

                UpdateRGBAValue();
                InvalidateRect( mo_ColorRect[0] );
                InvalidateRect( mo_ColorRect[1] );
                InvalidateRect( mo_PalDrawRect );

                mi_Capture = EDIATEXTURE_ColorCursorR + (i << 1);
                SetCapture();
                return;
            }
        }
    }
        

    for (i = 0; i < 2; i++)
    {
        if (!mpo_DrawRect[i]) continue;
        if (!mo_RealDrawRect[i].PtInRect( o_Pt )) continue;
        if(GetAsyncKeyState(VK_SPACE) < 0)
        {
    		mi_Capture = 1;
            mo_CapturePt = o_Pt;
            mi_Capture = EDIATEXTURE_Move0 + (i*2);
        }
        else if ( ( GetAsyncKeyState( VK_CONTROL ) < 0 ) || ( GetAsyncKeyState( VK_MENU ) < 0 ) )
        {
            PickColor( o_Pt, 0 );
            mi_Capture = EDIATEXTURE_PickColor0 + (i * 2);
        }
        else
        {
            Paint( o_Pt, 0, i, TRUE );
            mi_Capture = EDIATEXTURE_Paint0 + (i * 2);
        }
        SetCapture();
        return;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnLButtonUp(UINT ui, CPoint pt)
{
    if (mi_Capture & 1) return;
    ReleaseCapture();
	mi_Capture = 0;
	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnRButtonDown(UINT ui, CPoint o_Pt )
{
    int i;

    if(mi_Capture) return;

    if ( mb_TexIndex && mo_PalDrawRect.PtInRect( o_Pt ) )
    {
        PickPalColor( o_Pt, 1 );
        mi_Capture = EDIATEXTURE_PaletteRect1;
        SetCapture();
        return;
    }

    for (i = 0; i < 2; i++)
    {
        if (!mpo_DrawRect[i]) continue;
        if (!mo_RealDrawRect[i].PtInRect( o_Pt )) continue;
    
        if ( ( GetAsyncKeyState( VK_CONTROL ) < 0 ) || ( GetAsyncKeyState( VK_MENU ) < 0 ) )
        {
            PickColor( o_Pt, 1 );
            mi_Capture = EDIATEXTURE_PickColor1 + (i * 2);
        }
        else
        {
            Paint( o_Pt, 1, i, TRUE );
            mi_Capture = EDIATEXTURE_Paint1 + (i * 2);
        }
        SetCapture();
        return;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnRButtonUp(UINT ui, CPoint pt)
{
    if (!(mi_Capture & 1)) return;
    ReleaseCapture();
	mi_Capture = 0;
	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if 0
static void HookProc(ULONG _o_Dlg, ULONG p_color )
{
    EDIA_cl_Texture * po_Dlg;

    po_Dlg = (EDIA_cl_Texture *) _o_Dlg;

    //*(ULONG *) p_color = ( *(ULONG *) p_color & po_Dlg->mul_Mask) | (po_Dlg->mul_SaveColor & po_Dlg->mul_InvMask );
	//((EDIA_cl_Texture *) _o_Dlg)->InvalidateRect(((EDIA_cl_Texture *) _o_Dlg)->mo_CaseRect);
	((EDIA_cl_Texture *) _o_Dlg)->UpdateRGBAValue();
}
/**/
void EDIA_cl_Texture::OnLButtonDblClk(UINT ui, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					x, y, index;
	//EDIA_cl_ColorDialog *dlg_Color;
	char				sz_Title[64];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	index = Point2Color(pt, &x, &y);
	if(index != -1)
	{
		sprintf(sz_Title, "Color %d", index);
		//dlg_Color = new EDIA_cl_ColorDialog(sz_Title, &mul_Color[index], HookProc, (ULONG) this, (ULONG) &mul_Color[index]);
		//dlg_Color->DoModal();
		//delete(dlg_Color);
		InvalidateRect(mo_DrawRect);
	}
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnKeyDown(UINT nChar, UINT nRep, UINT nFlags )
{
    int     i_OldValue, i_Value;
    ULONG   ul_Mask;

    if (mi_ValueEdit == 0) 
    {
        mi_StartEdit = 1;
        return;
    }
    if (mi_CurValueEdit != mi_ValueEdit )
    {
        mi_StartEdit = 1;
        mi_CurValueEdit = mi_ValueEdit;
    }

    if (mi_CurValueEdit != 0)
    {
        if (mi_CurValueEdit == 1)
            i_OldValue = mi_ColorIndex[ mi_CurColor ];
        else 
            i_OldValue = (mul_ColorInv[ mi_CurColor ] >> ( 8 * (mi_CurValueEdit - 2) ))& 0xFF;
        i_Value = i_OldValue;

        if ( (nChar == VK_RETURN) || (nChar == VK_SPACE) )
            mi_StartEdit = 1;
        else if ( ( nChar == VK_SPACE ) || (nChar == VK_DELETE ) )
            i_Value = 0;
        else if (nChar == VK_BACK )
        {
            i_Value = (i_Value - (i_Value % 10)) / 10;
            mi_StartEdit = 0;
        }
        else if ( isdigit( nChar ) || isdigit( nChar - VK_NUMPAD0 + '0' ) )
        {
            if (nChar >= VK_NUMPAD0) nChar -= VK_NUMPAD0 - '0';
            i_Value = ( mi_StartEdit ) ? (nChar - '0') : ((i_Value * 10) + nChar - '0');
            if (i_Value > 255) i_Value = i_OldValue;
            if ( (mi_CurValueEdit == 1) && (mst_Palette.uc_Flags & TEX_uc_Palette16) )
                if (i_Value > 15) i_Value = i_OldValue;
            mi_StartEdit = 0;
        }

        if ( i_OldValue != i_Value )
        {
            if (mi_CurValueEdit == 1)
            {
                mi_ColorIndex[ mi_CurColor ] = i_Value;
                mul_Color[ mi_CurColor ] = mst_Palette.pul_Color[ i_Value ];
                mul_ColorInv[ mi_CurColor ] = M_SwapRB( mul_Color[ mi_CurColor ] );
            }
            else
             {
                ul_Mask = 0xFF << (8 * (mi_CurValueEdit - 2) );
                mul_ColorInv[ mi_CurColor ] &= ~( 0xFF << (8 * (mi_CurValueEdit - 2) ) );
                mul_ColorInv[ mi_CurColor ] |= i_Value << (8 * (mi_CurValueEdit - 2) );
                mul_Color[ mi_CurColor ] = M_SwapRB( mul_ColorInv[ mi_CurColor ] );
            }

            UpdateRGBAValue();
            InvalidateRect( mo_ColorRect[0] );
            InvalidateRect( mo_ColorRect[1] );
            if (!mb_TexIndex)
                InvalidateRect( mo_PalDrawRect );
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnCheck_PaletteAlpha( void )
{
    mb_PalShowAlpha = ((CButton *) GetDlgItem( IDC_CHECK_ACOL ))->GetCheck() ? TRUE : FALSE;
    ((CButton *) GetDlgItem( IDC_CHECK_RGBCOL ))->SetCheck( mb_PalShowAlpha ? 0 : 1 );
    ((CButton *) GetDlgItem( IDC_CHECK_ACOL ))->SetCheck( mb_PalShowAlpha ? 1 : 0 );
    InvalidateRect( NULL );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnCheck_PaletteRGB( void )
{
    mb_PalShowAlpha = (!mb_TexAlpha) ? FALSE : ((CButton *) GetDlgItem( IDC_CHECK_RGBCOL ))->GetCheck() ? FALSE : TRUE;
    ((CButton *) GetDlgItem( IDC_CHECK_RGBCOL ))->SetCheck( mb_PalShowAlpha ? 0 : 1 );
    ((CButton *) GetDlgItem( IDC_CHECK_ACOL ))->SetCheck( mb_PalShowAlpha ? 1 : 0 );
    InvalidateRect( NULL );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnCheck_A( void )
{
    mb_ShowA = ((CButton *) GetDlgItem( IDC_CHECK_ALPHA ))->GetCheck() ? TRUE : FALSE;
    mb_ShowRGB = !mb_ShowA ? TRUE : ( ((CButton *) GetDlgItem( IDC_CHECK_RGB ))->GetCheck() ? TRUE : FALSE);
    ((CButton *) GetDlgItem( IDC_CHECK_RGB ))->SetCheck( mb_ShowRGB ? 1 : 0 );
    ((CButton *) GetDlgItem( IDC_CHECK_ALPHA ))->SetCheck( mb_ShowA ? 1 : 0 );
    InvalidateRect( NULL );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnCheck_RGB( void )
{
    mb_ShowRGB = (!mb_TexAlpha) ? TRUE : ((CButton *) GetDlgItem( IDC_CHECK_RGB ))->GetCheck() ? TRUE : FALSE;
    mb_ShowA= !mb_ShowRGB ? TRUE : ( ((CButton *) GetDlgItem( IDC_CHECK_ALPHA ))->GetCheck() ? TRUE : FALSE);
    ((CButton *) GetDlgItem( IDC_CHECK_RGB ))->SetCheck( mb_ShowRGB ? 1 : 0 );
    ((CButton *) GetDlgItem( IDC_CHECK_ALPHA ))->SetCheck( mb_ShowA ? 1 : 0 );
    InvalidateRect( NULL );
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
void EDIA_cl_Texture::ComputeDrawRect(void)
{
	/*~~~~~*/
	int w, h;
	/*~~~~~*/

	GetClientRect(&mo_DrawRect[0]);

	mo_DrawRect[0].left = 210;
	mo_DrawRect[0].right -= 5;

	w = ((int) floor((mo_DrawRect[0].right - mo_DrawRect[0].left) / 16.0)) * 16;
	mo_DrawRect[0].right = mo_DrawRect[0].left + w;

	h = ((int) floor((mo_DrawRect[0].bottom - 10) / 16.0)) * 16;
	mo_DrawRect[0].top = 5;
	mo_DrawRect[0].bottom = mo_DrawRect[0].top + h;

    mo_DrawRect[1] = mo_DrawRect[0];
    mo_DrawRect[1].bottom = mo_DrawRect[1].top + ((mo_DrawRect[0].Height() - 8) >> 1);
    mo_DrawRect[2] = mo_DrawRect[0];
    mo_DrawRect[2].top = mo_DrawRect[ 1 ].bottom + 8;

    mo_PalDrawRect.left = 68;
    mo_PalDrawRect.top = 16;
    mo_PalDrawRect.right = 176;
    mo_PalDrawRect.bottom = 124;

    mo_RGBARect[0] = mo_PalDrawRect;
    mo_RGBARect[0].left += 4;
    mo_RGBARect[0].right -= 4;
    mo_RGBARect[0].top = 42;
    mo_RGBARect[0].bottom = 50;

    mo_RGBARect[1] = mo_RGBARect[0];
    mo_RGBARect[1].top = 63;
    mo_RGBARect[1].bottom = 71;

    mo_RGBARect[2] = mo_RGBARect[0];
    mo_RGBARect[2].top = 84;
    mo_RGBARect[2].bottom = 92;

    mo_RGBARect[3] = mo_RGBARect[0];
    mo_RGBARect[3].top = 105;
    mo_RGBARect[3].bottom = 113;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void  EDIA_cl_Texture::PickPalColor( CPoint o_Pt, int i_Num )
{
    int i, j;

    if ( !mb_TexIndex ) return;
    if ( !mo_PalDrawRect.PtInRect( o_Pt ) ) return;

    o_Pt.x -= mo_PalDrawRect.left;
    o_Pt.y -= mo_PalDrawRect.top;

    i = (o_Pt.x * 16) / mo_PalDrawRect.Width();
    j = (o_Pt.y * 16) / mo_PalDrawRect.Height();

    if (mst_Palette.uc_Flags & TEX_uc_Palette16)
        i = (j>>2) * 4 + (i>>2);
    else
        i = j * 16 + i;
     
    mi_CurColor = i_Num;
    mi_ColorIndex[ i_Num ] = i;
    mul_Color[ i_Num ] = mst_Palette.pul_Color[ mi_ColorIndex[ i_Num ] ];
    mul_ColorInv[ i_Num ] = M_SwapRB( mul_Color[ i_Num ] );

    UpdateRGBAValue();
	InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void  EDIA_cl_Texture::PickColor( CPoint o_Pt, int i_Num )
{
    UpdateCoord( o_Pt );
    if (mi_CurX == -1) return;

    if (mb_TexIndex )
    {
        mi_ColorIndex[ i_Num ] = mpc_Raw[ mi_CurY * mi_BmpWidth + mi_CurX ];
        mul_Color[ i_Num ] = mst_Palette.pul_Color[ mi_ColorIndex[ i_Num ] ];
    }
    else
    {
        mul_Color[ i_Num ] = ((ULONG *) mpc_Raw) [ mi_CurY * mi_BmpWidth + mi_CurX ];
    }
    mul_ColorInv[ i_Num ] = M_SwapRB( mul_Color[ i_Num ] );

    mi_CurColor = i_Num;
    UpdateRGBAValue();
    InvalidateRect(NULL, FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void  EDIA_cl_Texture::Paint( CPoint o_Pt, int i_Num , int i_Pane, BOOL _b_Start )
{
    unsigned char   *puc_Pixel;
    unsigned long   *pul_Pixel;
    ULONG           ul_Color;
    CRect           st_Rect;
    int             i_DX, i_DY;
    int             i, i_Val, i_Inc;
    float           f_Val, f_Inc;
    BOOL            b_Paint;

    if ( !mo_RealDrawRect[i_Pane].PtInRect( o_Pt ) )
    {
        mi_PaintPreviousX = -1;
        return;
    }

    UpdateCoord( o_Pt );
    if (mi_CurX == -1) return;

    if (mi_PaintPreviousX == -1)
        _b_Start = TRUE;

    if (!_b_Start)
    {
        i_DX = mi_CurX - mi_PaintPreviousX;
        i_DY = mi_CurY - mi_PaintPreviousY;
        if ( ( lAbs( i_DX ) <= 1 ) &&( lAbs( i_DY ) <= 1 ) )
            _b_Start = TRUE;
    }

    if (_b_Start)
    {
        if ( mb_TexIndex )
        {
            puc_Pixel = mpc_Raw + (mi_CurY * mi_BmpWidth + mi_CurX );
            if ( *puc_Pixel == mi_ColorIndex[ i_Num ] ) goto EDIA_cl_Texture_Paint;
            *puc_Pixel = mi_ColorIndex[ i_Num ];
        }
        else
        {
            pul_Pixel = ((ULONG *) mpc_Raw) + (mi_CurY * mi_BmpWidth + mi_CurX );
            if ( *pul_Pixel == mul_Color[ i_Num ] ) goto EDIA_cl_Texture_Paint;
            *pul_Pixel = mul_Color[ i_Num ];
        }
        
        mpo_DC[0]->SetPixel( mi_CurX, mi_BmpHeight - mi_CurY - 1, mul_ColorInv[ i_Num ] & 0xFFFFFF );
        if ( mpo_DC[1] )
        {
            ul_Color = mul_ColorInv[ i_Num ] >> 24;
            ul_Color |= (ul_Color << 8) | (ul_Color << 16);
            mpo_DC[1]->SetPixel( mi_CurX, mi_BmpHeight - mi_CurY - 1, ul_Color );
        }

        mst_RectSel.left = o_Pt.x - mi_Zoom;
        mst_RectSel.right = o_Pt.x + mi_Zoom;
        mst_RectSel.top = o_Pt.y - mi_Zoom;
        mst_RectSel.bottom = o_Pt.y + mi_Zoom;
    }
    else
    {
        b_Paint = FALSE;
        if (lAbs( i_DX ) < lAbs( i_DY ) )
        {
            f_Val = (float) mi_PaintPreviousX;
            f_Inc = (float) i_DX / (float) lAbs( i_DY );
            i_Val = mi_PaintPreviousY;
            i_Inc = i_DY / lAbs( i_DY );
            for (i = 0; i < lAbs( i_DY ); i++ )
            {
                i_Val += i_Inc;
                f_Val += f_Inc;

                if ( mb_TexIndex )
                {
                    puc_Pixel = mpc_Raw + (i_Val * mi_BmpWidth + (int) f_Val );
                    if ( *puc_Pixel == mi_ColorIndex[ i_Num ] ) continue;
                    b_Paint = TRUE;
                    *puc_Pixel = mi_ColorIndex[ i_Num ];
                }
                else
                {
                    pul_Pixel = ((ULONG *) mpc_Raw) + (i_Val * mi_BmpWidth + (int) f_Val );
                    if ( *pul_Pixel == mul_Color[ i_Num ] ) continue;
                    b_Paint = TRUE;
                    *pul_Pixel = mul_Color[ i_Num ];
                }
                mpo_DC[0]->SetPixel( (int) f_Val, mi_BmpHeight - i_Val - 1, mul_ColorInv[ i_Num ] & 0xFFFFFF );
                if ( mpo_DC[1] )
                {
                    ul_Color = mul_ColorInv[ i_Num ] >> 24;
                    ul_Color |= (ul_Color << 8) | (ul_Color << 16);
                    mpo_DC[1]->SetPixel( (int) f_Val, mi_BmpHeight - i_Val - 1, ul_Color );
                }
            }
            if (!b_Paint) goto EDIA_cl_Texture_Paint;
        }
        else
        {
            f_Val = (float) mi_PaintPreviousY;
            f_Inc = (float) i_DY / (float) lAbs( i_DX );
            i_Val = mi_PaintPreviousX;
            i_Inc = i_DX / lAbs( i_DX );
            for (i = 0; i < lAbs( i_DX ); i++ )
            {
                i_Val += i_Inc;
                f_Val += f_Inc;

                if ( mb_TexIndex )
                {
                    puc_Pixel = mpc_Raw + ((int) f_Val * mi_BmpWidth + i_Val );
                    if ( *puc_Pixel == mi_ColorIndex[ i_Num ] ) continue;
                    b_Paint = TRUE;
                    *puc_Pixel = mi_ColorIndex[ i_Num ];
                }
                else
                {
                    pul_Pixel = ((ULONG *) mpc_Raw) + ((int) f_Val * mi_BmpWidth + i_Val );
                    if ( *pul_Pixel == mul_Color[ i_Num ] ) continue;
                    b_Paint = TRUE;
                    *pul_Pixel = mul_Color[ i_Num ];
                }
                mpo_DC[0]->SetPixel( i_Val, mi_BmpHeight - (int) f_Val - 1, mul_ColorInv[ i_Num ] & 0xFFFFFF );
                if ( mpo_DC[1] )
                {
                    ul_Color = mul_ColorInv[ i_Num ] >> 24;
                    ul_Color |= (ul_Color << 8) | (ul_Color << 16);
                    mpo_DC[1]->SetPixel( i_Val, mi_BmpHeight - (int) f_Val - 1, ul_Color );
                }
            }
            if (!b_Paint) goto EDIA_cl_Texture_Paint;
        }

        if (mo_PaintPreviousPos.x < o_Pt.x )
        {
            mst_RectSel.left = mo_PaintPreviousPos.x - 2 * mi_Zoom;
            mst_RectSel.right = o_Pt.x + 2 * mi_Zoom;
        }
        else
        {
            mst_RectSel.left = o_Pt.x - 2 * mi_Zoom;
            mst_RectSel.right = mo_PaintPreviousPos.x + 2 * mi_Zoom;
        }
        
        if (mo_PaintPreviousPos.y < o_Pt.y )
        {
            mst_RectSel.top = mo_PaintPreviousPos.y - 2 * mi_Zoom;
            mst_RectSel.bottom = o_Pt.y + 2 * mi_Zoom;
        }
        else
        {
            mst_RectSel.top = o_Pt.y - 2 * mi_Zoom;
            mst_RectSel.bottom = mo_PaintPreviousPos.y + 2 * mi_Zoom;
        }
    }

    InvalidateRect(&mst_RectSel, FALSE);

    if (mb_ShowRGB && mb_ShowA )
    {
        if (mi_CurRect == 0)
            mst_RectSel.OffsetRect( mo_RealDrawRect[1].TopLeft() - mo_RealDrawRect[0].TopLeft() );
        else
            mst_RectSel.OffsetRect( mo_RealDrawRect[0].TopLeft() - mo_RealDrawRect[1].TopLeft() );
        InvalidateRect(&mst_RectSel, FALSE);
    }

EDIA_cl_Texture_Paint:   
    mo_PaintPreviousPos = o_Pt;
    mi_PaintPreviousX = mi_CurX;
    mi_PaintPreviousY = mi_CurY;
}



/*
 =======================================================================================================================
    recherche l'index de la couleur sous un point (retourne -1 si en dehors de la palette) si les 2 pointeurs sur des
    entiers (x et y) sont non nuls et qu'une couleur est trouvé sous le point x et y contiendront la ligne et la
    colonne de la couleur
 =======================================================================================================================
 */
int EDIA_cl_Texture::Point2Color(CPoint pt, int *_x, int *_y)
{
    return 0;
}

/*
 =======================================================================================================================
    recherche l'index de la couleur sous un point (retourne -1 si en dehors de la palette) si les 2 pointeurs sur des
    entiers (x et y) sont non nuls et qu'une couleur est trouvé sous le point x et y contiendront la ligne et la
    colonne de la couleur
 =======================================================================================================================
 */
void EDIA_cl_Texture::Index2LineCol(int index, int *_x, int *_y)
{
}

/*
 =======================================================================================================================
    Met à jour les valeurs index, rouge, vert, bleu, alpha
 =======================================================================================================================
 */
void EDIA_cl_Texture::UpdateRGBAValue(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int		color;
	char	sz_Value[10];
	/*~~~~~~~~~~~~~~~~~*/

    color = mul_ColorInv[ mi_CurColor ];
    
    if (mb_TexIndex)
        GetDlgItem(IDC_CHECK_I)->SetWindowText(_itoa( mi_ColorIndex[ mi_CurColor ], sz_Value, 10));
    else
        GetDlgItem(IDC_CHECK_I)->SetWindowText( "---" );

    GetDlgItem(IDC_CHECK_R)->SetWindowText(_itoa((color & 0xFF), sz_Value, 10));
	GetDlgItem(IDC_CHECK_G)->SetWindowText(_itoa(((color >> 8) & 0xFF), sz_Value, 10));
	GetDlgItem(IDC_CHECK_B)->SetWindowText(_itoa(((color >> 16) & 0xFF), sz_Value, 10));
    
    if (mb_TexAlpha)
        GetDlgItem(IDC_CHECK_A)->SetWindowText(_itoa(((color >> 24) & 0xFF), sz_Value, 10));
    else
        GetDlgItem(IDC_CHECK_A)->SetWindowText( "---" );
}

/*
 =======================================================================================================================
    change le nombre de couleurs de la palette (16 -> 256, 256 -> 16 )
 =======================================================================================================================
 */
void EDIA_cl_Texture::OnSwapColor(void)
{
    int mi_Swap;

    if (mb_TexIndex)
    {
        mi_Swap = mi_ColorIndex[ 0 ];
        mi_ColorIndex[ 0 ] = mi_ColorIndex[ 1 ];
        mi_ColorIndex[ 1 ] = mi_Swap;

        mul_Color[ 0 ] = mst_Palette.pul_Color[ mi_ColorIndex[ 0 ] ];
        mul_Color[ 1 ] = mst_Palette.pul_Color[ mi_ColorIndex[ 1 ] ];
    }
    else
    {
        mi_Swap = (int) mul_Color[ 0 ];
        mul_Color[ 0 ] = mul_Color[ 1 ];
        mul_Color[ 1 ] = (ULONG) mi_Swap;
    }
    
    mul_ColorInv[ 0 ] = M_SwapRB( mul_Color[ 0 ] );
    mul_ColorInv[ 1 ] = M_SwapRB( mul_Color[ 1 ] );

    UpdateRGBAValue();
	InvalidateRect( mo_ColorRect[0] );
    InvalidateRect( mo_ColorRect[1] );
    InvalidateRect( mo_PalDrawRect );
}

/*
 =======================================================================================================================
    change le nombre de couleurs de la palette (16 -> 256, 256 -> 16 )
 =======================================================================================================================
 */
void EDIA_cl_Texture::UpdateCoord( CPoint o_Pt )
{
    char sz_Text[ 128 ];

    if (mpo_DrawRect[0] && mo_RealDrawRect[0].PtInRect( o_Pt ) )
    {
        mi_CurX = mi_X + ( (o_Pt.x - mo_RealDrawRect[0].left) / mi_Zoom );
        mi_CurY = mi_Y + ( (o_Pt.y - mo_RealDrawRect[0].top) / mi_Zoom );
        sprintf( sz_Text, "(x,y) = (%d,%d)", mi_CurX, mi_CurY );    
        mi_CurY = mi_BmpHeight - mi_CurY - 1;
        mi_CurRect = 0;
    }
    else if (mpo_DrawRect[1] && mo_RealDrawRect[1].PtInRect( o_Pt ) )
    {
        mi_CurX = mi_X + ( (o_Pt.x - mo_RealDrawRect[1].left) / mi_Zoom );
        mi_CurY = mi_Y + ( (o_Pt.y - mo_RealDrawRect[1].top) / mi_Zoom );
        sprintf( sz_Text, "(x,y) = (%d,%d)", mi_CurX, mi_CurY );    
        mi_CurY = mi_BmpHeight - mi_CurY - 1;
        mi_CurRect = 1;
    }
    else
    {
        mi_CurRect = -1;
        mi_CurX = -1;
        *sz_Text = 0;
    }

    GetDlgItem( IDC_STATIC_COORD )->SetWindowText( sz_Text );
}


#endif /* ACTIVE_EDITORS */
