/*$T DIArli_dlg.cpp GC 1.129 09/14/01 17:23:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIArliadjust_dlg.h"
#include "DIAlogs/DIACOLOR_dlg.h"
#include "DIAlogs/DIAtoolbox_groview.h"
#include "DIAlogs/DIApalette_dlg.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/ENGVars.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "LINKs/LINKtoed.h"
#include "EDItors/Sources/AIscript/AIframe.h"
#include "EDItors/Sources/AIscript/EditView/AIview.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDImsg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "EDIpaths.h"
#include "EDIapp.h"
#include "SELection/SELection.h"
#include "GEOmetric/GEOsubobject.h"
#include "SOFT/SOFTcolor.h"

#ifdef JADEFUSION
extern void	(*GEO_gpfnv_AdjustRLI) (ULONG *);
#else
extern "C" void	(*GEO_gpfnv_AdjustRLI) (ULONG *);
#endif
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_RLIAdjustDialog, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED( IDC_CHECK_PREVIEWONSLIDE, OnBt_PreviewOnSlide )
	ON_BN_CLICKED( IDC_CHECK_HIDESUBOBJECT, OnBt_HideSubObject )
	ON_BN_CLICKED( IDC_CHECK_COLORIZE, OnBt_Colorize )
	ON_BN_CLICKED( IDC_RADIO_MODE1, OnRadio_Mode1 )
    ON_BN_CLICKED( IDC_RADIO_MODE2, OnRadio_Mode2 )
	ON_BN_CLICKED( IDC_RADIO_MODE3, OnRadio_Mode3 )
    ON_BN_CLICKED( IDC_RADIO_MODE4, OnRadio_Mode4 )
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_RLIAdjustDialog::EDIA_cl_RLIAdjustDialog(F3D_cl_View *_po_View, EDIA_cl_ToolBox_GROView * _po_ToolBox, BOOL _b_SubObject ) :	EDIA_cl_BaseDialog(DIALOGS_IDD_RLIADJUST)
{
    mpo_3DView = _po_View;
    mpo_ToolBox = _po_ToolBox;
    mpo_Palette = NULL;
    mpul_PaletteColor = NULL;
    mpc_PaletteSel = NULL;
    mi_PaletteColorNb = 0;
    mi_Capture = 0;
    mi_CurMode = 0;
    mi_PreviewOnSlide = 1;
    mi_HideSubObject = 1;
    mb_SubObject = _b_SubObject;
    mi_Cafe = 0;
    if (mpo_ToolBox)
    {
    	mi_HideSubObject = mpo_ToolBox->mpst_EditOption->i_RLIAdjust_HideSub;
		mi_PreviewOnSlide = mpo_ToolBox->mpst_EditOption->i_RLIAdjust_Preview ;
		mi_CurMode = mpo_ToolBox->mpst_EditOption->i_RLIAdjust_Mode;
		mi_Colorize = mpo_ToolBox->mpst_EditOption->i_RLIAdjust_Colorize;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_RLIAdjustDialog::EDIA_cl_RLIAdjustDialog( EDIA_cl_Palette *_po_Palette, ULONG *_pul_Color, char *_pc_Sel, int _i_NbColor ) : EDIA_cl_BaseDialog(DIALOGS_IDD_RLIADJUST)
{
    mpo_3DView = NULL;
    mpo_ToolBox = NULL;
    mpo_Palette = _po_Palette;
    mpul_PaletteColor = _pul_Color;
    mi_PaletteColorNb = _i_NbColor;
    mpc_PaletteSel = _pc_Sel;
    mi_Capture = 0;
    mi_CurMode = 0;
    mi_PreviewOnSlide = 1;
    mi_HideSubObject = 1;
    mb_SubObject = 0;
    mi_Cafe = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_RLIAdjustDialog::OnInitDialog(void)
{
	int i;
	
	EDIA_cl_BaseDialog::OnInitDialog();
	
	if (mb_SubObject || mpo_Palette)
		RLI_Save();
	
	GetDlgItem( IDC_STATIC_TITLE0 )->SetWindowText( "Hue:" );
	GetDlgItem( IDC_STATIC_TITLE1 )->SetWindowText( "Saturation:" );
	GetDlgItem( IDC_STATIC_TITLE2 )->SetWindowText( "Lightness:" );
	GetDlgItem( IDC_STATIC_TITLE3 )->SetWindowText( "Alpha:" );
	
	mi_Value[ 0 ] = 0;
	mi_Min[ 0 ] = -180;
	mi_Max[ 0 ] = 180;
	GetDlgItem( IDC_STATIC_SLIDE0 )->GetClientRect( mo_Rect[ 0 ] );
	GetDlgItem( IDC_STATIC_SLIDE0 )->ClientToScreen( mo_Rect[ 0 ] );
	ScreenToClient( mo_Rect[ 0 ] );
	
	
	mi_Value[ 1 ] = 0;
	mi_Min[ 1 ] = -100;
	mi_Max[ 1 ] = 100;
	GetDlgItem( IDC_STATIC_SLIDE1 )->GetClientRect( mo_Rect[ 1 ] );
	GetDlgItem( IDC_STATIC_SLIDE1 )->ClientToScreen( mo_Rect[ 1 ] );
	ScreenToClient( mo_Rect[ 1 ] );
	
	mi_Value[ 2 ] = 0;
	mi_Min[ 2 ] = -100;
	mi_Max[ 2 ] = 100;
	GetDlgItem( IDC_STATIC_SLIDE2 )->GetClientRect( mo_Rect[ 2 ] );
	GetDlgItem( IDC_STATIC_SLIDE2 )->ClientToScreen( mo_Rect[ 2 ] );
	ScreenToClient( mo_Rect[ 2 ] );

	mi_Value[ 3 ] = 0;
	mi_Min[ 3 ] = -255;
	mi_Max[ 3 ] = 255;
	GetDlgItem( IDC_STATIC_SLIDE3 )->GetClientRect( mo_Rect[ 3 ] );
	GetDlgItem( IDC_STATIC_SLIDE3 )->ClientToScreen( mo_Rect[ 3 ] );
	ScreenToClient( mo_Rect[ 3 ] );
	
	GetDlgItem( IDC_CHECK_VIEWOLD )->GetClientRect( mo_Rect[ 4 ] );
	GetDlgItem( IDC_CHECK_VIEWOLD )->ClientToScreen( mo_Rect[ 4 ] );
	ScreenToClient( mo_Rect[ 4 ] );
	
	((CButton *) GetDlgItem( IDC_CHECK_PREVIEWONSLIDE))->SetCheck( mi_PreviewOnSlide );
	((CButton *) GetDlgItem( IDC_CHECK_HIDESUBOBJECT))->SetCheck( mi_HideSubObject );
	((CButton *) GetDlgItem( IDC_CHECK_COLORIZE))->SetCheck( mi_Colorize );
	GetDlgItem( IDC_CHECK_COLORIZE)->ShowWindow( mi_CurMode == 0 ? SW_SHOW : SW_HIDE );
	
	
	if ( !mb_SubObject )
		GetDlgItem( IDC_CHECK_HIDESUBOBJECT)->ShowWindow( SW_HIDE );
	else
		if(mi_HideSubObject)
			Refresh();
		
	
	((CButton *) GetDlgItem( IDC_RADIO_MODE1 ))->SetCheck( mi_CurMode == 0 );
	((CButton *) GetDlgItem( IDC_RADIO_MODE2 ))->SetCheck( mi_CurMode == 1 );
	((CButton *) GetDlgItem( IDC_RADIO_MODE3 ))->SetCheck( mi_CurMode == 2 );
	((CButton *) GetDlgItem( IDC_RADIO_MODE4 ))->SetCheck( mi_CurMode == 3 );
	
	DisplayValues();
	
	if ( mpo_ToolBox && mpo_ToolBox->mpst_EditOption->i_RLIAdjust_X != -10000)
		SetWindowPos( NULL, mpo_ToolBox->mpst_EditOption->i_RLIAdjust_X, mpo_ToolBox->mpst_EditOption->i_RLIAdjust_Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
	else
		CenterWindow();
		
	// Table
	for (i = 0; i < 256; i++ )
	{
		maf_Rlum[ i ] = ((float) i) * RGB_INTENSITY_RED / 255.0;
		maf_Glum[ i ] = ((float) i) * RGB_INTENSITY_GREEN / 255.0;
		maf_Blum[ i ] = ((float) i) * RGB_INTENSITY_BLUE / 255.0;
	}
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::OnDestroy(void)
{

	if (mpo_ToolBox)
	{
		RECT	st_Rect;
		GetWindowRect( &st_Rect );
		mpo_ToolBox->mpst_EditOption->i_RLIAdjust_X = st_Rect.left;
		mpo_ToolBox->mpst_EditOption->i_RLIAdjust_Y = st_Rect.top;
		mpo_ToolBox->mpst_EditOption->i_RLIAdjust_HideSub = ((CButton *) GetDlgItem( IDC_CHECK_HIDESUBOBJECT))->GetCheck() ? 1 : 0;
		mpo_ToolBox->mpst_EditOption->i_RLIAdjust_Preview = ((CButton *) GetDlgItem( IDC_CHECK_PREVIEWONSLIDE))->GetCheck() ? 1 : 0;
		mpo_ToolBox->mpst_EditOption->i_RLIAdjust_Colorize = ((CButton *) GetDlgItem( IDC_CHECK_COLORIZE))->GetCheck() ? 1 : 0;
		mpo_ToolBox->mpst_EditOption->i_RLIAdjust_Mode = 0;
		if ( ((CButton *) GetDlgItem( IDC_RADIO_MODE2 ))->GetCheck() )
			mpo_ToolBox->mpst_EditOption->i_RLIAdjust_Mode = 1;
		mpo_ToolBox->mpo_AdjustRLI = NULL;
		((CButton *) mpo_ToolBox->GetDlgItem( IDC_CHECK_VERTEXADJUSTRLI ))->SetCheck( 0 );
	}
	
	EDI_go_TheApp.RemoveModeless( GetSafeHwnd() );
	if (mi_HideSubObject)
		LINK_Refresh();
	RLI_Free();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::OnCancel(void)
{
	RLI_Restore();
	if (mpo_Palette)
		mpo_Palette->InvalidateRect(&mpo_Palette->mo_DrawRect);
	else
		LINK_Refresh();
	EDIA_cl_BaseDialog::OnCancel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::Refresh( void )
{
	if (mpo_Palette)
	{
		mpo_Palette->InvalidateRect(&mpo_Palette->mo_DrawRect);
		return;
	}
	if (mi_HideSubObject)	GDI_gpst_CurDD->pst_EditOptions->ul_Flags |= GRO_Cul_EOF_HideSubObject;
	if (ENG_gb_EngineRunning)
	{
		ENG_gb_EngineRunning = 0;
		LINK_Refresh();
		ENG_gb_EngineRunning = 1;
	}
	else
		LINK_Refresh();
	if (mi_HideSubObject)	GDI_gpst_CurDD->pst_EditOptions->ul_Flags &= ~GRO_Cul_EOF_HideSubObject;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_RLIAdjustDialog::Value2Coord( int _i_ValIndex, int _i_Val )
{
	float	f_Factor;
	int		x;
	
	if (_i_Val < mi_Min[ _i_ValIndex ] ) _i_Val = mi_Min[ _i_ValIndex ];
	if (_i_Val > mi_Max[ _i_ValIndex ] ) _i_Val = mi_Max[ _i_ValIndex ];
	
	f_Factor = (float)(_i_Val - mi_Min[ _i_ValIndex ]);
	f_Factor /=(float)(mi_Max[ _i_ValIndex ] - mi_Min[ _i_ValIndex ]);
	
	x = (int) ( (float)(mo_Rect[ _i_ValIndex ].Width() - 8) * f_Factor);
	x += mo_Rect[ _i_ValIndex ].left + 4;
	
	return x;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_RLIAdjustDialog::Coord2Value( int _i_ValIndex, int _i_Val )
{
	float	f_Factor;
	int		x;
	
	mo_Rect[ _i_ValIndex ].DeflateRect( 4, 0);
	
	if (_i_Val < mo_Rect[ _i_ValIndex ].left ) _i_Val = mo_Rect[ _i_ValIndex ].left;
	if (_i_Val > mo_Rect[ _i_ValIndex ].right ) _i_Val = mo_Rect[ _i_ValIndex ].right;
	
	f_Factor = (float)( _i_Val - mo_Rect[ _i_ValIndex ].left);
	f_Factor /=(float)( mo_Rect[_i_ValIndex].Width());
	
	x = (int) ( (float)(mi_Max[_i_ValIndex] - mi_Min[_i_ValIndex]) * f_Factor);
	x += mi_Min[_i_ValIndex];
	
	mo_Rect[ _i_ValIndex ].InflateRect( 4, 0);
	
	return x;
}
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::DisplayValues( void )
{
	char	str[ 64 ];
	GetDlgItem( IDC_EDIT_SLIDE0 )->SetWindowText( itoa( mi_Value[ 0 ], str, 10 ) );
	GetDlgItem( IDC_EDIT_SLIDE1 )->SetWindowText( itoa( mi_Value[ 1 ], str, 10 ) );
	GetDlgItem( IDC_EDIT_SLIDE2 )->SetWindowText( itoa( mi_Value[ 2 ], str, 10 ) );
	GetDlgItem( IDC_EDIT_SLIDE3 )->SetWindowText( itoa( mi_Value[ 3 ], str, 10 ) );
	
	/* AVIRER */
	if ( (mi_Value[ 0 ] == 3) && (mi_Value[ 1 ] == 1) && (mi_Value[ 2 ] == 6) )
	{
		if ( mi_Cafe == 0 )
		{
			GetDlgItem( IDC_CHECK_COLORIZE)->SetWindowText( "!! CAFE !!" );
			mi_Cafe = 1;
		}
	}
	else if (mi_Cafe)
	{
		mi_Cafe = 0;
		GetDlgItem( IDC_CHECK_COLORIZE)->SetWindowText( "Colorize" );
	}
	/* AVIRER END */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::ChangeMode( int _i_Mode )
{
	if (_i_Mode == mi_CurMode ) return;
	mi_CurMode = _i_Mode;
	
	if (mi_CurMode == 0)
	{
		mi_Min[ 0 ] = -180;		mi_Max[ 0 ] = 180;
		mi_Min[ 1 ] = -100;		mi_Max[ 1 ] = 100;
		mi_Min[ 2 ] = -100;		mi_Max[ 2 ] = 100;
		GetDlgItem( IDC_STATIC_TITLE0 )->SetWindowText( "Hue:" );
		GetDlgItem( IDC_STATIC_TITLE1 )->SetWindowText( "Saturation:" );
		GetDlgItem( IDC_STATIC_TITLE2 )->SetWindowText( "Lightness:" );
	}
	else if (mi_CurMode == 1)
	{
		mi_Min[ 0 ] = -100;		mi_Max[ 0 ] = 100;
		mi_Min[ 1 ] = -100;		mi_Max[ 1 ] = 100;
		mi_Min[ 2 ] = -100;		mi_Max[ 2 ] = 100;
		GetDlgItem( IDC_STATIC_TITLE0 )->SetWindowText( "Contrast" );
		GetDlgItem( IDC_STATIC_TITLE1 )->SetWindowText( "Brightness" );
	}
	else if (mi_CurMode == 2) // Add RGB
	{
		mi_Min[ 0 ] = -255;		mi_Max[ 0 ] = 255;
		mi_Min[ 1 ] = -255;		mi_Max[ 1 ] = 255;
		mi_Min[ 2 ] = -255;		mi_Max[ 2 ] = 255;
		GetDlgItem( IDC_STATIC_TITLE0 )->SetWindowText( "Red:" );
		GetDlgItem( IDC_STATIC_TITLE1 )->SetWindowText( "Green:" );
		GetDlgItem( IDC_STATIC_TITLE2 )->SetWindowText( "Blue:" );
	}
	else if (mi_CurMode == 3) // Mul RGB
	{
		mi_Min[ 0 ] = -100;		mi_Max[ 0 ] = 100;
		mi_Min[ 1 ] = -100;		mi_Max[ 1 ] = 100;
		mi_Min[ 2 ] = -100;		mi_Max[ 2 ] = 100;
		GetDlgItem( IDC_STATIC_TITLE0 )->SetWindowText( "Red:" );
		GetDlgItem( IDC_STATIC_TITLE1 )->SetWindowText( "Green:" );
		GetDlgItem( IDC_STATIC_TITLE2 )->SetWindowText( "Blue:" );
	}
	
	GetDlgItem( IDC_STATIC_TITLE2 )->ShowWindow( mi_CurMode != 1 );
	GetDlgItem( IDC_EDIT_SLIDE2)->ShowWindow( mi_CurMode != 1 );
	GetDlgItem( IDC_CHECK_COLORIZE)->ShowWindow( mi_CurMode == 0 );
	
	mi_Value[ 0 ] = mi_Value[ 1 ] = mi_Value[ 2 ] = 0;
	DisplayValues();
	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CDC		*pDC;
	int		i, x, y, nb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	EDIA_cl_BaseDialog::OnPaint();
	
	pDC = GetDC();
	
	switch( mi_CurMode )
	{
	case 0: nb = 3; break;
	case 1: nb = 2; break;
	case 2: nb = 3; break;
	case 3: nb = 3; break;
	}
	
	for (i = 0; i < nb; i++)
	{
		pDC->FillSolidRect( mo_Rect[ i ], GetSysColor(COLOR_BTNFACE) );
		pDC->MoveTo( mo_Rect[ i ].left + 4, mo_Rect[ i ].top + 4);
		pDC->LineTo( mo_Rect[ i ].left + 4, mo_Rect[ i ].top + 8 );
		pDC->LineTo( mo_Rect[ i ].right - 4, mo_Rect[ i ].top + 8 );
		pDC->LineTo( mo_Rect[ i ].right - 4, mo_Rect[ i ].top );
		
		pDC->MoveTo( (mo_Rect[ i ].left + mo_Rect[ i ].right) / 2, mo_Rect[ i ].top + 5);
		pDC->LineTo( (mo_Rect[ i ].left + mo_Rect[ i ].right) / 2, mo_Rect[ i ].top + 8);
		
		x = Value2Coord( i, mi_Value[i] );
		y = mo_Rect[i].top + 9;
		pDC->MoveTo( x, y );
		pDC->LineTo( x+4, y+6);
		pDC->LineTo( x-4, y+6);
		pDC->LineTo( x, y);
	}
	pDC->FillSolidRect( mo_Rect[ 3 ], GetSysColor(COLOR_BTNFACE) );
	pDC->MoveTo( mo_Rect[ 3 ].left + 4, mo_Rect[ 3 ].top + 4);
	pDC->LineTo( mo_Rect[ 3 ].left + 4, mo_Rect[ 3 ].top + 8 );
	pDC->LineTo( mo_Rect[ 3 ].right - 4, mo_Rect[ 3 ].top + 8 );
	pDC->LineTo( mo_Rect[ 3 ].right - 4, mo_Rect[ 3 ].top );
	
	pDC->MoveTo( (mo_Rect[ 3 ].left + mo_Rect[ 3 ].right) / 2, mo_Rect[ 3 ].top + 5);
	pDC->LineTo( (mo_Rect[ 3 ].left + mo_Rect[ 3 ].right) / 2, mo_Rect[ 3 ].top + 8);
	
	x = Value2Coord( 3, mi_Value[3] );
	y = mo_Rect[3].top + 9;
	pDC->MoveTo( x, y );
	pDC->LineTo( x+4, y+6);
	pDC->LineTo( x-4, y+6);
	pDC->LineTo( x, y);
	
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_RLIAdjustDialog::PreTranslateMessage(MSG *pMsg)
{
	if ( pMsg->message == WM_LBUTTONDOWN )
	{
		if (pMsg->hwnd == GetDlgItem( IDC_CHECK_VIEWOLD)->GetSafeHwnd() )
		{
			((CButton *) GetDlgItem( IDC_CHECK_VIEWOLD))->SetCheck( 1 );
			mi_Capture = 5;
			SetCapture();
			RLI_Restore();
			Refresh();
			return TRUE;
		}
	}
	return EDIA_cl_BaseDialog::PreTranslateMessage( pMsg );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::OnLButtonDown(UINT, CPoint pt)
{
	int i;
	
	for (i = 0; i < 4; i++)
	{
		if( mo_Rect[i].PtInRect( pt ) )
		{
			mi_SaveValue[ 0 ] = mi_Value[ 0 ];
			mi_SaveValue[ 1 ] = mi_Value[ 1 ];
			mi_SaveValue[ 2 ] = mi_Value[ 2 ];
			mi_SaveValue[ 3 ] = mi_Value[ 3 ];
			
			mi_Value[ i ] = Coord2Value( i, pt.x );
			DisplayValues();
			Invalidate();
			mi_Capture = i+1;
			SetCapture();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::OnMouseMove(UINT, CPoint pt)
{
	int		i, newval;
	CPoint	savept;
	
	if (mi_Capture)
	{
		i = mi_Capture - 1;
		savept = pt;
		if(pt.x > mo_Rect[i].right) pt.x = mo_Rect[i].right;
		if(pt.x < mo_Rect[i].left) pt.x = mo_Rect[i].left;
		if(pt.y < mo_Rect[i].top) pt.y = mo_Rect[i].top;
		if(pt.y > mo_Rect[i].bottom) pt.y = mo_Rect[i].bottom;
		if(savept != pt)
		{
			ClientToScreen((LPPOINT) & pt);
			SetCursorPos(pt.x, pt.y);
			return;
		}
		
		if (i < 4)
		{
			newval = Coord2Value( i, pt.x );
			if (newval != mi_Value[ i ] )
			{
				mi_Value[ i ] = Coord2Value( i, pt.x );
				DisplayValues();
				Invalidate();
				if (mi_PreviewOnSlide)
					RLI_Compute();
			}
		}
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::OnLButtonUp(UINT, CPoint pt)
{
	if (mi_Capture)
	{
		if (mi_Capture == 5)
			((CButton *) GetDlgItem( IDC_CHECK_VIEWOLD))->SetCheck( 0 );

		ReleaseCapture();
		mi_Capture = 0;
		RLI_Compute();
	}
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::OnRButtonDown(UINT, CPoint pt)
{
	int i;
	
	if (mi_Capture == 5)
	{
		((CButton *) GetDlgItem( IDC_CHECK_VIEWOLD))->SetCheck( 0 );
		RLI_Compute();
		return;
	}
	
	if (mi_Capture)
	{
		ReleaseCapture();
		mi_Capture = 0;
		mi_Value[ 0 ] = mi_SaveValue[ 0 ];
		mi_Value[ 1 ] = mi_SaveValue[ 1 ];
		mi_Value[ 2 ] = mi_SaveValue[ 2 ];
		mi_Value[ 3 ] = mi_SaveValue[ 3 ];
		DisplayValues();
		Invalidate();
		RLI_Compute();
		return;
	}
	
	for (i = 0; i < 4; i++)
	{
		if( mo_Rect[i].PtInRect( pt ) )
		{
			mi_Value[ i ] = (mi_Max[i] + mi_Min[i]) >> 1;
			DisplayValues();
			Invalidate();
			RLI_Compute();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_RLIAdjustDialog::OnMouseWheel(UINT nFlags, short z, CPoint pt)
{
	int i, oldval, dval;
	
	ScreenToClient( &pt );
	
	if (mi_Capture)
		return 0;
	if (nFlags & MK_LBUTTON) return 0;
	if (nFlags & MK_RBUTTON) return 0;
	if (nFlags & MK_MBUTTON) return 0;
	
	dval = (nFlags & MK_SHIFT) ? 1: 5;
	
	for (i = 0; i < 4; i++)
	{
		if( mo_Rect[i].PtInRect( pt ) )
		{
			oldval = mi_Value[ i ];
			mi_Value[ i ] += (z < 0) ? -dval : dval;
			if (mi_Value[i] < mi_Min[i]) mi_Value[ i ] = mi_Min[ i ];
			if (mi_Value[i] > mi_Max[i]) mi_Value[ i ] = mi_Max[ i ];
			if (oldval != mi_Value[ i ])
			{
				DisplayValues();
				Invalidate();
				RLI_Compute();
			}
		}
	}
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::OnBt_PreviewOnSlide( void )
{
	mi_PreviewOnSlide = ((CButton *) GetDlgItem( IDC_CHECK_PREVIEWONSLIDE))->GetCheck();
}

/*
 =======================================================================================================================
 =======================================================================================================================
*/
void EDIA_cl_RLIAdjustDialog::OnBt_HideSubObject( void )
{
	mi_HideSubObject = ((CButton *) GetDlgItem( IDC_CHECK_HIDESUBOBJECT))->GetCheck();
	Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
*/
void EDIA_cl_RLIAdjustDialog::OnBt_Colorize( void )
{
	mi_Colorize = ((CButton *) GetDlgItem( IDC_CHECK_COLORIZE))->GetCheck();
	
	// AVIRER
	if (mi_Colorize )
	{
		char sz_Name[ 4096 ], *pz;
		FILE *f;
		
		if ( mi_Cafe )
		{
			strcpy(sz_Name, M_MF()->mst_Ini.asz_CurrentRefFile);
			pz = L_strrchr(sz_Name, '\\');
			if(pz) *pz = 0;
			strcat(sz_Name, "/vlhullier.msg");

			f = fopen( sz_Name, "wt");
			if(f)
			{
				fprintf( f, "FROM %s : CAFE !!", M_MF()->mst_Ini.asz_CurrentUserName );
				fclose(f);
			}
			mi_Value[ 0 ] = 0;
			mi_Value[ 1 ] = 0;
			mi_Value[ 2 ] = 0;
			mi_Value[ 3 ] = 0;
			DisplayValues();
		}
	}
	// AVIRER END
	RLI_Compute();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 void EDIA_cl_RLIAdjustDialog::OnRadio_Mode1( void )
 {
	ChangeMode( 0 );
 }
 
 /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void EDIA_cl_RLIAdjustDialog::OnRadio_Mode2( void )
 {
	ChangeMode( 1 );
 }

  /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void EDIA_cl_RLIAdjustDialog::OnRadio_Mode3( void )
 {
	ChangeMode( 2 );
 }

  /*
 =======================================================================================================================
 =======================================================================================================================
 */
 void EDIA_cl_RLIAdjustDialog::OnRadio_Mode4( void )
 {
	ChangeMode( 3 );
 }

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_b_RLISave(SEL_tdst_SelectedItem *_pst_Sel, ULONG l1, ULONG l2)
{
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	EDIA_tdst_RLIAdjust	*pst_RLI;
	
	EDIA_cl_RLIAdjustDialog *po_Dlg = (EDIA_cl_RLIAdjustDialog *) l1;
	
	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	
	pst_RLI = &po_Dlg->mpst_RLIAdjust[ po_Dlg->mi_RLICur++ ];
	L_memset( pst_RLI, 0, sizeof(EDIA_tdst_RLIAdjust) );
	
	if ( !pst_Obj ) return TRUE;
	if ( !pst_Obj->pst_SubObject ) return TRUE;
	if (!OBJ_i_RLIlocation(pst_GO)) return TRUE;
	
	pst_RLI->pc_Sel = pst_Obj->pst_SubObject->dc_VSel;
	if (po_Dlg->mpo_3DView->mb_WorkWithGRORLI)
		pst_RLI->pul_RLI = pst_Obj->dul_PointColors;
	else
		pst_RLI->pul_RLI = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
	pst_RLI->dul_RLISave = (ULONG *) L_malloc( sizeof(ULONG) * (pst_RLI->pul_RLI[0] + 1) );
	L_memcpy( pst_RLI->dul_RLISave, pst_RLI->pul_RLI, sizeof(ULONG) * (pst_RLI->pul_RLI[0] + 1) );
	return TRUE;
}
/**/
void EDIA_cl_RLIAdjustDialog::RLI_Save()
{
	int		i;
	char	*pc_Sel, *pc_Last;
	SEL_tdst_Selection *pst_Sel;
	
	if (mpo_Palette)
	{
		L_memcpy( mul_PaletteSave, mpul_PaletteColor, mi_PaletteColorNb * sizeof( ULONG ) );
		return;
	}
	
	if ( !mb_SubObject )
		return;
	
	pst_Sel = mpo_3DView->mst_WinHandles.pst_World->pst_Selection;
	mi_RLINb = SEL_l_CountItem(pst_Sel, SEL_C_SIF_Object );
	if (!mi_RLINb)
	{
		mpst_RLIAdjust = NULL;
		return;
	}
	
	mpst_RLIAdjust = (EDIA_tdst_RLIAdjust *) L_malloc( sizeof( EDIA_tdst_RLIAdjust ) * mi_RLINb );
	mi_RLICur = 0;
	SEL_EnumItem( pst_Sel, SEL_C_SIF_Object, F3D_b_RLISave, (ULONG) this, 0 );
	
	for (i = 0; i < mi_RLINb; i++)
	{
		if (mpst_RLIAdjust[i].pul_RLI )
		{
			pc_Sel = mpst_RLIAdjust[i].pc_Sel;
			pc_Last = pc_Sel + mpst_RLIAdjust[ i ].pul_RLI[ 0 ];
			while ( pc_Sel < pc_Last )
			{
				if ( *pc_Sel++ & 1 )
					goto RLI_Save_endofloop;
			}
		}
	}
RLI_Save_endofloop:
	if (i == mi_RLINb )
	{
		for (i = 0; i < mi_RLINb; i++)
			mpst_RLIAdjust[i].pc_Sel = NULL;
	}
#ifdef JADEFUSION
	XenonRefresh();
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::RLI_Free( void )
{
	int i;
	
	if ( !mb_SubObject )
	{
		COLOR_RLIAdjust_Free();
		return;
	}
		
	
	if ( !mpst_RLIAdjust )
		return;
		
	for (i = 0; i < mi_RLINb; i++)
	{
		if ( !mpst_RLIAdjust[ i ].dul_RLISave ) continue;
		L_free( mpst_RLIAdjust[ i ].dul_RLISave );
	}
	L_free( mpst_RLIAdjust );
	mpst_RLIAdjust = NULL;

#ifdef JADEFUSION
    XenonRefresh();
#endif
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::RLI_Restore()
{
	int		i;
	char	*pc, *pclast;
	ULONG	*pul_Src, *pul_Tgt;
	
	if (mpo_Palette)
	{
		L_memcpy( mpul_PaletteColor, mul_PaletteSave, mi_PaletteColorNb * sizeof( ULONG ) );
		return;
	}
	
	if ( !mb_SubObject )
	{
		COLOR_RLIAdjust_Restore();
#ifdef JADEFUSION
		XenonRefresh();
#endif
		return;
	}
	
	if ( !mpst_RLIAdjust )
		return;
		
	for (i = 0; i < mi_RLINb; i++)
	{
		if ( !mpst_RLIAdjust[ i ].pul_RLI  ) continue;
		pc = mpst_RLIAdjust[ i ].pc_Sel;
		if (!pc )
			L_memcpy( mpst_RLIAdjust[ i ].pul_RLI, mpst_RLIAdjust[ i ].dul_RLISave, (mpst_RLIAdjust[ i ].pul_RLI[0] + 1) * sizeof(ULONG) );
		else
		{
			pclast = pc + mpst_RLIAdjust[ i ].pul_RLI[0];
			pul_Src = mpst_RLIAdjust[ i ].dul_RLISave + 1;
			pul_Tgt = mpst_RLIAdjust[ i ].pul_RLI + 1;
			while (pc < pclast )
			{
				if (*pc++)
					*pul_Tgt = *pul_Src;
				pul_Tgt++;
				pul_Src++;
			}
		}
	}
#ifdef JADEFUSION
    XenonRefresh();
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::RLI_Compute_HLS( void )
{
	char	*pc_Sel;
	ULONG	*pul_RLI;
	int		i, j, nb;
	
	// get params
	mdh = (double) mi_Value[0];
	mds = -((double) mi_Value[1] / (double) mi_Max[ 1 ]);
	mdl = (double) mi_Value[2] / (double) mi_Max[ 2 ];
	
	RLI_Restore();
	
	if( mpo_Palette )
	{
		if ( !mpc_PaletteSel )
		{
			for (i = 0; i < mi_PaletteColorNb; i++ )
				mpul_PaletteColor[i] = RLI_AdjustHLS( mpul_PaletteColor[i] );
		}
		else
		{
			pc_Sel = mpc_PaletteSel;
			pul_RLI = mpul_PaletteColor;
			for (i = 0; i < mi_PaletteColorNb; i++, pc_Sel++, pul_RLI++)
			{
				if (!(*pc_Sel & 1)) continue;
				*pul_RLI = RLI_AdjustHLS( *pul_RLI );
			}
		}
	}
	else
	{
		for ( i = 0; i < mi_RLINb; i++)
		{
			if ( !mpst_RLIAdjust[ i ].pul_RLI  ) continue;
			pul_RLI = mpst_RLIAdjust[ i ].pul_RLI;
			nb = pul_RLI[0];
			pul_RLI++;
			
			pc_Sel = mpst_RLIAdjust[ i ].pc_Sel;
			if ( !pc_Sel )
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
					*pul_RLI = RLI_AdjustHLS( *pul_RLI );
			}
			else
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
				{
					if (!(*pc_Sel & 1)) continue;
					*pul_RLI = RLI_AdjustHLS( *pul_RLI );
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::RLI_Compute_HLS_Colorize( void )
{
	char	*pc_Sel;
	ULONG	*pul_RLI;
	int		i, j, nb;
	double	h, l, s,r, g, b;
	
	// get params
	mdh = (double) mi_Value[0];
	mds = -((double) mi_Value[1] / (double) mi_Max[ 1 ]);
	mdl = (double) mi_Value[2] / (double) mi_Max[ 2 ];
	
	// compute final lookup table
	h = mdh / 360.0;
	s = mds / 100.0;
	for (i = 0; i < 256; i ++)
	{
		l = i / 255.0;
		HLS_to_RGB( h, l, s, &r, &g, &b );
		maf_Rfinal[i] = i * r;
		maf_Gfinal[i] = i * g;
		maf_Bfinal[i] = i * b;
	}
	
	RLI_Restore();
	
	if( mpo_Palette )
	{
		if ( !mpc_PaletteSel )
		{
			for (i = 0; i < mi_PaletteColorNb; i++ )
				mpul_PaletteColor[i] = RLI_AdjustHLSColorize( mpul_PaletteColor[i] );
		}
		else
		{
			pc_Sel = mpc_PaletteSel;
			pul_RLI = mpul_PaletteColor;
			for (i = 0; i < mi_PaletteColorNb; i++, pc_Sel++, pul_RLI++)
			{
				if (!(*pc_Sel & 1)) continue;
				*pul_RLI = RLI_AdjustHLSColorize( *pul_RLI );
			}
		}
	}
	else
	{
		for ( i = 0; i < mi_RLINb; i++)
		{
			if ( !mpst_RLIAdjust[ i ].pul_RLI  ) continue;
			pul_RLI = mpst_RLIAdjust[ i ].pul_RLI;
			nb = pul_RLI[0];
			pul_RLI++;
			
			pc_Sel = mpst_RLIAdjust[ i ].pc_Sel;
			if ( !pc_Sel )
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
					*pul_RLI = RLI_AdjustHLSColorize( *pul_RLI );
			}
			else
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
				{
					if (!(*pc_Sel & 1)) continue;
					*pul_RLI = RLI_AdjustHLSColorize( *pul_RLI );
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::RLI_Compute_BC( void )
{
	char	*pc_Sel;
	ULONG	*pul_RLI, ul_RLI;
	int		i, j, nb;
	double	brightness, contrast, value, nvalue, power;
	UCHAR	lut[ 256 ];
	
	// get params
	contrast = (double) mi_Value[0] / (double) mi_Max[ 0 ];
	brightness = (double) mi_Value[1] / (double) mi_Max[ 1 ];
	
	for (i = 0; i < 256; i++)
	{
		value = (double) i / 255.0;
		
		/* apply brightness */
		if ( brightness < 0.0 )
			value = value * (1.0 + brightness);
		else
			value = value + ((1.0 - value) * brightness);

		/* apply contrast */
		if ( contrast < 0.0 )
		{
			if (value > 0.5)
				nvalue = 1.0 - value;
			else
				nvalue = value;

			if (nvalue < 0.0)
				nvalue = 0.0;

			nvalue = 0.5 * pow (nvalue * 2.0 , (double) (1.0 + contrast));

			if (value > 0.5)
				value = 1.0 - nvalue;
			else
				value = nvalue;
		}
		else
		{
			if (value > 0.5)
				nvalue = 1.0 - value;
			else
				nvalue = value;

			if (nvalue < 0.0)
				nvalue = 0.0;

			power = (contrast == 1.0) ? 127 : 1.0 / (1.0 - contrast);
			nvalue = 0.5 * pow (2.0 * nvalue, power);

			if (value > 0.5)
				value = 1.0 - nvalue;
			else
				value = nvalue;
		}
		
		nb = (int) (value * 255.0);
		if (nb < 0) nb = 0;
		if (nb > 255) nb = 255;
		lut[ i ] = (UCHAR) (nb & 0xFF);
	
	}
	
	RLI_Restore();
	
	if( mpo_Palette )
	{
		if ( !mpc_PaletteSel )
		{
			pul_RLI = mpul_PaletteColor;
			for (i = 0; i < mi_PaletteColorNb; i++, pul_RLI++ )
			{
				ul_RLI = *pul_RLI & 0xFF000000;
				ul_RLI |= lut[ *pul_RLI & 0xFF ];
				ul_RLI |= lut[ (*pul_RLI >> 8) & 0xFF ] << 8;
				ul_RLI |= lut[ (*pul_RLI >> 16) & 0xFF ] << 16;
				*pul_RLI = ul_RLI;
			}
		}
		else
		{
			pc_Sel = mpc_PaletteSel;
			pul_RLI = mpul_PaletteColor;
			for (i = 0; i < mi_PaletteColorNb; i++, pc_Sel++, pul_RLI++)
			{
				if (!(*pc_Sel & 1)) continue;
				ul_RLI = *pul_RLI & 0xFF000000;
				ul_RLI |= lut[ *pul_RLI & 0xFF ];
				ul_RLI |= lut[ (*pul_RLI >> 8) & 0xFF ] << 8;
				ul_RLI |= lut[ (*pul_RLI >> 16) & 0xFF ] << 16;
				*pul_RLI = ul_RLI;
			}
		}
	}
	else
	{
		for ( i = 0; i < mi_RLINb; i++)
		{
			if ( !mpst_RLIAdjust[ i ].pul_RLI  ) continue;
			pul_RLI = mpst_RLIAdjust[ i ].pul_RLI;
			nb = pul_RLI[0];
			pul_RLI++;
			
			pc_Sel = mpst_RLIAdjust[ i ].pc_Sel;
			if ( !pc_Sel )
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
				{
					ul_RLI = *pul_RLI & 0xFF000000;
					ul_RLI |= lut[ *pul_RLI & 0xFF ];
					ul_RLI |= lut[ (*pul_RLI >> 8) & 0xFF ] << 8;
					ul_RLI |= lut[ (*pul_RLI >> 16) & 0xFF ] << 16;
					*pul_RLI = ul_RLI;
				}
			}
			else
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
				{
					if (!(*pc_Sel & 1)) continue;
					ul_RLI = *pul_RLI & 0xFF000000;
					ul_RLI |= lut[ *pul_RLI & 0xFF ];
					ul_RLI |= lut[ (*pul_RLI >> 8) & 0xFF ] << 8;
					ul_RLI |= lut[ (*pul_RLI >> 16) & 0xFF ] << 16;
					*pul_RLI = ul_RLI;
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::RLI_Compute_AddRGB( void )
{
	char	*pc_Sel;
	ULONG	*pul_RLI;
	int		i, j, nb;
	
	// get params
	mdr = (double) mi_Value[0] / (double) mi_Max[ 0 ];
	mdg = (double) mi_Value[1] / (double) mi_Max[ 1 ];
	mdb = (double) mi_Value[2] / (double) mi_Max[ 2 ];
	
	RLI_Restore();
	
	if( mpo_Palette )
	{
		if ( !mpc_PaletteSel )
		{
			for (i = 0; i < mi_PaletteColorNb; i++ )
				mpul_PaletteColor[i] = RLI_AdjustRGBAdd( mpul_PaletteColor[i] );
		}
		else
		{
			pc_Sel = mpc_PaletteSel;
			pul_RLI = mpul_PaletteColor;
			for (i = 0; i < mi_PaletteColorNb; i++, pc_Sel++, pul_RLI++)
			{
				if (!(*pc_Sel & 1)) continue;
				*pul_RLI = RLI_AdjustRGBAdd( *pul_RLI );
			}
		}
	}
	else
	{
		for ( i = 0; i < mi_RLINb; i++)
		{
			if ( !mpst_RLIAdjust[ i ].pul_RLI  ) continue;
			pul_RLI = mpst_RLIAdjust[ i ].pul_RLI;
			nb = pul_RLI[0];
			pul_RLI++;
			
			pc_Sel = mpst_RLIAdjust[ i ].pc_Sel;
			if ( !pc_Sel )
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
					*pul_RLI = RLI_AdjustRGBAdd( *pul_RLI );
			}
			else
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
				{
					if (!(*pc_Sel & 1)) continue;
					*pul_RLI = RLI_AdjustRGBAdd( *pul_RLI );
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::RLI_Compute_MulRGB( void )
{
	char	*pc_Sel;
	ULONG	*pul_RLI;
	int		i, j, nb;
	
	// get params
	mdr = 1.0 + (double) mi_Value[0] / (double) mi_Max[ 0 ];
	mdg = 1.0 + (double) mi_Value[1] / (double) mi_Max[ 1 ];
	mdb = 1.0 + (double) mi_Value[2] / (double) mi_Max[ 2 ];
	
	RLI_Restore();
	
	if( mpo_Palette )
	{
		if ( !mpc_PaletteSel )
		{
			for (i = 0; i < mi_PaletteColorNb; i++ )
				mpul_PaletteColor[i] = RLI_AdjustRGBMul( mpul_PaletteColor[i] );
		}
		else
		{
			pc_Sel = mpc_PaletteSel;
			pul_RLI = mpul_PaletteColor;
			for (i = 0; i < mi_PaletteColorNb; i++, pc_Sel++, pul_RLI++)
			{
				if (!(*pc_Sel & 1)) continue;
				*pul_RLI = RLI_AdjustRGBMul( *pul_RLI );
			}
		}
	}
	else
	{
		for ( i = 0; i < mi_RLINb; i++)
		{
			if ( !mpst_RLIAdjust[ i ].pul_RLI  ) continue;
			pul_RLI = mpst_RLIAdjust[ i ].pul_RLI;
			nb = pul_RLI[0];
			pul_RLI++;
			
			pc_Sel = mpst_RLIAdjust[ i ].pc_Sel;
			if ( !pc_Sel )
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
					*pul_RLI = RLI_AdjustRGBMul( *pul_RLI );
			}
			else
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
				{
					if (!(*pc_Sel & 1)) continue;
					*pul_RLI = RLI_AdjustRGBMul( *pul_RLI );
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::RLI_Compute_Alpha( void ) {

	char	*pc_Sel;
	ULONG	*pul_RLI;
	int		i, j, nb;
	
	// get params
	mdalpha = (double) mi_Value[3] / (double) mi_Max[ 3 ];
	
	//RLI_Restore();
	
	if( mpo_Palette )
	{
		if ( !mpc_PaletteSel )
		{
			for (i = 0; i < mi_PaletteColorNb; i++ )
				mpul_PaletteColor[i] = RLI_AdjustAlphaAdd( mpul_PaletteColor[i] );
		}
		else
		{
			pc_Sel = mpc_PaletteSel;
			pul_RLI = mpul_PaletteColor;
			for (i = 0; i < mi_PaletteColorNb; i++, pc_Sel++, pul_RLI++)
			{
				if (!(*pc_Sel & 1)) continue;
				*pul_RLI = RLI_AdjustAlphaAdd( *pul_RLI );
			}
		}
	}
	else
	{
		for ( i = 0; i < mi_RLINb; i++)
		{
			if ( !mpst_RLIAdjust[ i ].pul_RLI  ) continue;
			pul_RLI = mpst_RLIAdjust[ i ].pul_RLI;
			nb = pul_RLI[0];
			pul_RLI++;
			
			pc_Sel = mpst_RLIAdjust[ i ].pc_Sel;
			if ( !pc_Sel )
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
					*pul_RLI = RLI_AdjustAlphaAdd( *pul_RLI );
			}
			else
			{
				for (j = 0; j < nb; j++, pc_Sel++, pul_RLI++)
				{
					if (!(*pc_Sel & 1)) continue;
					*pul_RLI = RLI_AdjustAlphaAdd( *pul_RLI );
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIAdjustDialog::RLI_Compute( void )
{
	double	brightness, contrast;
	double	r, g, b;
	double	a;
	
	if ( mb_SubObject || mpo_Palette )
	{
		switch( mi_CurMode )
		{
		case 0: if (mi_Colorize)
					RLI_Compute_HLS_Colorize(); 
				else
					RLI_Compute_HLS();	
				break;
		case 1: RLI_Compute_BC();	break;
		case 2: RLI_Compute_AddRGB(); break;
		case 3: RLI_Compute_MulRGB(); break;
		}
		RLI_Compute_Alpha();
		Refresh();
	}
	else
	{
		switch( mi_CurMode )
		{
		case 0: 
			mdh = (double) mi_Value[0];
			mds = -((double) mi_Value[1] / (double) mi_Max[ 1 ]);
			mdl = (double) mi_Value[2] / (double) mi_Max[ 2 ];
			if (mi_Colorize)
			{
				COLOR_RLIAdjust_SetHLSColorizeParams( mdh, mdl, mds );
				GEO_gpfnv_AdjustRLI = COLOR_RLIAdjust_HLSColorize;	
			}
			else
			{
				COLOR_RLIAdjust_SetHLSParams( mdh, mdl, mds );
				GEO_gpfnv_AdjustRLI = COLOR_RLIAdjust_HLS;	
			}
			break;
		case 1: 
			contrast = (double) mi_Value[0] / (double) mi_Max[ 0 ];
			brightness = (double) mi_Value[1] / (double) mi_Max[ 1 ];
			COLOR_RLIAdjust_SetBCParams( contrast, brightness );
			GEO_gpfnv_AdjustRLI = COLOR_RLIAdjust_BC;	
			break;
		case 2:
			r = (double) mi_Value[0] / (double) mi_Max[ 0 ];
			g = (double) mi_Value[1] / (double) mi_Max[ 1 ];
			b = (double) mi_Value[2] / (double) mi_Max[ 2 ];
			COLOR_RLIAdjust_SetRGBParams( r, g, b );
			GEO_gpfnv_AdjustRLI = COLOR_RLIAdjust_RGBAdd;	
			break;
		case 3:
			r = 1.0 + (double) mi_Value[0] / (double) mi_Max[ 0 ];
			g = 1.0 + (double) mi_Value[1] / (double) mi_Max[ 1 ];
			b = 1.0 + (double) mi_Value[2] / (double) mi_Max[ 2 ];
			COLOR_RLIAdjust_SetRGBParams( r, g, b );
			GEO_gpfnv_AdjustRLI = COLOR_RLIAdjust_RGBMul;	
			break;
		}
		a = (double) mi_Value[3] / (double) mi_Max[ 3 ];
		COLOR_RLIAdjust_SetAlphaParam( a );
		LINK_Refresh();
		GEO_gpfnv_AdjustRLI = NULL;
	}
#ifdef JADEFUSION
    XenonRefresh();
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EDIA_cl_RLIAdjustDialog::RLI_AdjustHLS( ULONG RGB )
{
	double	r, g, b, h, l, s;
		
	// RGB in float
	r = (RGB & 0xff) * 1.0 / 255.0;
    g = ((RGB & 0xff00) >> 8) * 1.0 / 255.0;
    b = ((RGB & 0xff0000) >> 16) * 1.0 / 255.0;
	
	// convert r,g,b to h,l,s
	RGB_to_HLS( r, g, b, &h, &l, &s );
	h *= 360.0;
	
	// transform h,l,s
	l += mdl;
	if (l < 0) l = 0;
	if (l > 1) l = 1;
	
	s += mds;
	if (s < 0) s = 0;
	if (s > 1) s = 1;
	
	h += mdh;
	if ( h < 0 ) h += 360;
	
	// convert h, l, s to r, g, b
	h /= 360.0;
	HLS_to_RGB( h, l, s, &r, &g, &b );
	
	// r,g,b, 2 RGB
	RGB &= 0xFF000000;
    RGB |= ((ULONG) (b * 255.0f)) << 16;
    RGB |= ((ULONG) (g * 255.0f)) << 8;
    RGB |= ((ULONG) (r * 255.0f));
    
    return RGB;
}

#ifdef JADEFUSION
void EDIA_cl_RLIAdjustDialog::XenonRefresh(void)
{
#if defined(_XENON_RENDER)
    if (mpo_3DView)
    {
        mpo_3DView->Selection_XenonRefresh();

        LINK_Refresh();
    }
#endif
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EDIA_cl_RLIAdjustDialog::RLI_AdjustHLSColorize( ULONG RGB )
{
	int		R, G, B, i;
	double	r, g, b, l, lum;
		
	// RGB in float
    R = RGB & 0xFF;
    G = (RGB & 0xFF00) >> 8;
    B = (RGB & 0xFF0000) >> 16;
    
    lum = maf_Rlum[ R ] + maf_Glum[ G ] + maf_Blum[ B ];
    l = mdl * 100;
    
    if ( mdl > 0)
    {
		lum = (lum * 255.0) * (100.0 - l) / 100.0;
		lum += 255 - (100.0 - l) * 255.0 / 100.0;
    }
    else if (mdl < 0)
    {
		lum = (lum *255.0) * (l + 100.0) / 100.0;
	}
	
	i = ( int ) lum;
	r = maf_Rfinal[ i ];
	g = maf_Gfinal[ i ];
	b = maf_Bfinal[ i ];

	RGB &= 0xFF000000;
    RGB |= ((ULONG) b) << 16;
    RGB |= ((ULONG) g) << 8;
    RGB |= ((ULONG) r);
    
    return RGB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EDIA_cl_RLIAdjustDialog::RLI_AdjustRGBAdd( ULONG RGB )
{
	double	r, g, b;
		
	// RGB in float
	r = (RGB & 0xff) * 1.0 / 255.0;
    g = ((RGB & 0xff00) >> 8) * 1.0 / 255.0;
    b = ((RGB & 0xff0000) >> 16) * 1.0 / 255.0;

	r += mdr;
	if (r < 0) r = 0;
	else if (r > 1) r = 1;

	g += mdg;
	if (g < 0) g = 0;
	else if (g > 1) g = 1;

	b += mdb;
	if (b < 0) b = 0;
	else if (b > 1) b = 1;

	// r,g,b, 2 RGB
	RGB &= 0xFF000000;
    RGB |= ((ULONG) (b * 255.0f)) << 16;
    RGB |= ((ULONG) (g * 255.0f)) << 8;
    RGB |= ((ULONG) (r * 255.0f));
    
    return RGB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EDIA_cl_RLIAdjustDialog::RLI_AdjustAlphaAdd( ULONG RGB ) {
	double	a;
		
	// Alpha in float
	a = ((RGB & 0xff000000) >> 24) * 1.0 / 255.0;

	a += mdalpha;
	if (a < 0) a = 0;
	else if (a > 1) a = 1;

	RGB &= 0x00FFFFFF;
    RGB |= ((ULONG) (a * 255.0f)) << 24;
    
    return RGB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EDIA_cl_RLIAdjustDialog::RLI_AdjustRGBMul( ULONG RGB )
{
	double	r, g, b;
		
	// RGB in float
	r = (RGB & 0xff) * 1.0 / 255.0;
    g = ((RGB & 0xff00) >> 8) * 1.0 / 255.0;
    b = ((RGB & 0xff0000) >> 16) * 1.0 / 255.0;

	r *= mdr;
	if (r < 0) r = 0;
	else if (r > 1) r = 1;

	g *= mdg;
	if (g < 0) g = 0;
	else if (g > 1) g = 1;

	b *= mdb;
	if (b < 0) b = 0;
	else if (b > 1) b = 1;

	// r,g,b, 2 RGB
	RGB &= 0xFF000000;
    RGB |= ((ULONG) (b * 255.0f)) << 16;
    RGB |= ((ULONG) (g * 255.0f)) << 8;
    RGB |= ((ULONG) (r * 255.0f));
    
    return RGB;
}

#endif /* ACTIVE_EDITOR */
