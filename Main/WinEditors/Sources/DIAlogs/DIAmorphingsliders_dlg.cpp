/*$T DIAmorphing_dlg.cpp GC! 1.081 06/14/00 10:03:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAmorphing_dlg.h"
#include "DIAlogs/DIAmorphingsliders_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "Engine/Sources/MODifier/MDFmodifier_GEO.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dframe.h"
#include "EDIapp.h"
#include "EDIeditors_infos.h"
#include "Editors/Sources/OUTput/OUTframe.h"
#include "GEOmetric/GEOsubobject.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "LINKs/LINKtoed.h"
#include "LINks/LINKmsg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "EDImsg.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"



BEGIN_MESSAGE_MAP(EDIA_cl_MorphingSlidersDialog, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MorphingSlidersDialog::EDIA_cl_MorphingSlidersDialog( EDIA_cl_MorphingDialog *_po_Owner, int _i_Index ) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_MORPHINGSLIDERS)
{
	int		i;
	LOGFONT lf;

	
    mpo_Owner = _po_Owner;
    
    mpst_Gao = mpo_Owner->mpst_Gao;
    mpst_Geo = mpo_Owner->mpst_Geo;
	mpst_Modifier = mpo_Owner->mpst_Modifier;
	mpst_Morph = mpo_Owner->mpst_Morph;
	
	mi_LineNb = (mpst_Morph->l_NbMorphData > 1) ? 1 : mpst_Morph->l_NbMorphData;
	L_memset(  mast_Line, 0, 8 * sizeof(EDIAMorph_tdst_Line) );

	for (i = 0; i < mi_LineNb; i++)
	{
		mast_Line[ i ].i_Visible = 1;
		mast_Line[ i ].i_Index = i;
	}
	mast_Line[ 0 ].i_Index = _i_Index;
	
	mi_Capture = 0;
	mf_FactorOffset = mf_ProgOffset = 0;
	mb_LockRefresh = FALSE;
	
	mi_ShowFactor = 1;
    mi_ShowProg = 1;
    mi_ShowSlider = 0;
    
    mi_UnitWidth = 100;
    
    /* Create small font */
    M_MF()->mo_Fnt.GetLogFont( &lf );
    lf.lfHeight -= 4;
	mo_SmallFont.CreateFontIndirect( &lf );

	maaul_Color[ 0 ][ COLOR_BCKGRND ] = GetSysColor(COLOR_3DFACE);
	maaul_Color[ 0 ][ COLOR_PROG0 ]	= 0xFF;
	maaul_Color[ 0 ][ COLOR_PROG1 ]	= 0xFF0000;	
	maaul_Color[ 0 ][ COLOR_FACTOR0 ] =	0xFF;
	maaul_Color[ 0 ][ COLOR_FACTOR1	] = 0xFF0000;	
	maaul_Color[ 0 ][ COLOR_CURSOR	] = 0;

	maaul_Color[ 1 ][ COLOR_BCKGRND ] = GetSysColor(COLOR_3DHILIGHT);
	maaul_Color[ 1 ][ COLOR_PROG0 ]	= 0xFF;
	maaul_Color[ 1 ][ COLOR_PROG1 ]	= 0xFF0000;	
	maaul_Color[ 1 ][ COLOR_FACTOR0 ] =	0xFF;
	maaul_Color[ 1 ][ COLOR_FACTOR1	] = 0xFF0000;	
	maaul_Color[ 1 ][ COLOR_CURSOR	] = 0xFF;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_MorphingSlidersDialog::~EDIA_cl_MorphingSlidersDialog(void)
{
	mo_SmallFont.DeleteObject();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MorphingSlidersDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect				o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetWindowRect(&o_Rect);

	mi_UnitWidth = 100;

	if ( !PlugTo3DView( 1 ) )
	{
		SetWindowPos( mpo_Owner, 0, 0, 560, 400, SWP_NOMOVE );
		CenterWindow();
	}
	Window_SetHeight();
	
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MorphingSlidersDialog::PlugTo3DView( int _i_Bottom )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect				o_Rect;
	EDI_cl_BaseFrame	*po_OutputFrame;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetWindowRect(&o_Rect);

	po_OutputFrame = M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	if ( !po_OutputFrame )
		return FALSE;

	po_OutputFrame->GetWindowRect( &o_Rect );
	if ( _i_Bottom )
	{
		o_Rect.top = o_Rect.bottom;
		if (o_Rect.bottom + 50 > GetSystemMetrics(SM_CYMAXIMIZED))
			o_Rect.top -= o_Rect.bottom;
	}
	else
	{
		o_Rect.bottom = o_Rect.top;
		if (o_Rect.top - 50 > 0)
			o_Rect.top -= 50;
	}
	SetWindowPos( mpo_Owner, o_Rect.left, o_Rect.top, o_Rect.Width(), 50, 0 );
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_MorphingSlidersDialog::i_OnMessage(ULONG msg, ULONG, ULONG) 
{ 
	if(msg == EDI_MESSAGE_REFRESHDLG)
	{
		RefreshFromExt();
	}
	return 1; 
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingSlidersDialog::RefreshFromExt(void)
{
	if(mb_LockRefresh) return;
	M_MF()->LockDisplay(this);
	InvalidateRect(&mo_FactorRect);
	InvalidateRect(&mo_ProgRect);
	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingSlidersDialog::OnPaint(void)
{
	/*~~~~~~~~~~~*/
	CDC		*pDC;
	CRect	o_Rect;
	CRect	o_LineRect;
	CRect	o_NameRect;
	CRect	o_SliderRect;
	int		i_LineHeight;
	int		i_Line;
	int		i_Color;
	/*~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnPaint();
	
	pDC = GetDC();
	
	pDC->SelectObject(&M_MF()->mo_Fnt);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(0);
	
	GetClientRect(&o_Rect);
	
	
	if ( mi_ShowFactor && mi_ShowProg )
		i_LineHeight = 40;
	else
		i_LineHeight = 20;
		
	if ( mi_ShowSlider )
		o_NameRect.left = 20;
	else
		o_NameRect.left = 0;
		
	o_NameRect.right = o_NameRect.left + 100;
	o_NameRect.bottom = o_Rect.top;
	o_SliderRect.left = o_NameRect.right;
	o_SliderRect.right = o_Rect.right;
	o_SliderRect.bottom = o_Rect.top;
	
	//mf_UnitSegs = (float) o_SliderRect.Width() / (float) mi_UnitWidth;
	o_LineRect = o_Rect;
	o_LineRect.bottom = o_Rect.top + 3;
	i_Line = 0;
	while ( 1 )
	{
		i_Color = i_Line & 1;
		o_LineRect.top = o_LineRect.bottom;
		o_LineRect.bottom += Line_GetHeight( i_Line );
		if (o_LineRect.bottom > o_Rect.bottom )
			break;
		Line_Display( pDC, i_Line, &o_LineRect, i_Color );
		i_Line++;
	}
		
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingSlidersDialog::Line_Display( CDC *pDC, int _i_Data, CRect *_po_Rect, int _i_Color )
{
	char								sz_Name[ 512 ];
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	int									i_Odd, i_Cursor;
	int									X, DX;
	float								x, dx;
	int									CursorX, CursorY;

	pst_Channel = mpst_Morph->dst_MorphChannel + mast_Line[ _i_Data ].i_Index;
	mast_Line[ _i_Data ].o_Rect = *_po_Rect;
	
	//pDC->SelectObject(&M_MF()->mo_Fnt);
	sprintf( sz_Name, "%02d - %s", _i_Data, pst_Channel->sz_Name );
	pDC->FillSolidRect( _po_Rect, maaul_Color[ _i_Color ][ COLOR_BCKGRND ] );

	_po_Rect->left += 5;
	_po_Rect->right -= 5;
	
	/* display name */
	pDC->TextOut( _po_Rect->left, _po_Rect->top, sz_Name, strlen(sz_Name) );

	/* display factor slider */
	_po_Rect->top += 15;

	mast_Line[ _i_Data ].o_Factor.top = _po_Rect->top - 3;
	mast_Line[ _i_Data ].o_Factor.bottom = _po_Rect->top + 8;
	mast_Line[ _i_Data ].o_Factor.left = _po_Rect->left - 10;
	mast_Line[ _i_Data ].o_Factor.right = _po_Rect->right + 10;

	X = _po_Rect->left;
	x = mast_Line[ _i_Data ].f_FactorOffset;
	i_Odd = ((int) floor( x )) & 1;
	i_Cursor = 0;

	/* show unit at start */
	if (ceil(x) == x )
	{
		itoa( ((int)x), sz_Name, 10);
		pDC->TextOut( X, _po_Rect->top, sz_Name, strlen( sz_Name ) );
	}
	
	while ( 1 )
	{
		dx = ceil( x ) - x;
		if (dx == 0 ) dx = 1;
		
		DX = (int) (dx * mi_UnitWidth);
		if (X + DX > _po_Rect->right )
		{
			DX = _po_Rect->right - X;
			dx = (float) DX / (float) mi_UnitWidth;
		}
		else
		{
			/* show unit at end */
			itoa( ((int) ceil(x+dx)), sz_Name, 10);
			pDC->TextOut( X + DX, _po_Rect->top, sz_Name, strlen( sz_Name ) );
		}

		/* draw bar */
		pDC->FillSolidRect( X, _po_Rect->top, DX, 2, maaul_Color[ _i_Color ][ i_Odd ? COLOR_FACTOR0 : COLOR_FACTOR1 ] );

		/* display cursor */
		if ( ( pst_Channel->f_Blend >= x ) && ( pst_Channel->f_Blend <= x + dx ) &&  !i_Cursor)
		{
			i_Cursor = 1;
			CursorX = X + (int) ((pst_Channel->f_Blend - x) * mi_UnitWidth) - 1;
			CursorY = _po_Rect->top - 3;
			pDC->FillSolidRect( CursorX, CursorY, 3, 8, maaul_Color[ _i_Color ][ COLOR_CURSOR ] );
		}

		/* next step */
		x += dx;
		X += DX;
		mast_Line[ _i_Data ].o_Factor.right = X + 10;
		mast_Line[ _i_Data ].f_FactorEnd = x;
		i_Odd = 1 - i_Odd;

		/* stop loop */
		if (X == _po_Rect->right)
			break;
	}
	
	/* display prog slider */
	_po_Rect->top += 15;
	
	mast_Line[ _i_Data ].o_Prog.top = _po_Rect->top - 3;
	mast_Line[ _i_Data ].o_Prog.bottom = _po_Rect->top + 8;
	mast_Line[ _i_Data ].o_Prog.left = _po_Rect->left - 10;
	mast_Line[ _i_Data ].o_Prog.right = _po_Rect->right + 10;

	X = _po_Rect->left;
	x = mast_Line[ _i_Data ].f_ProgOffset;
	i_Odd = ((int) floor( x )) & 1;
	i_Cursor = 0;

	/* show unit at start */
	if (ceil(x) == x )
	{
		itoa( ((int)x), sz_Name, 10);
		pDC->TextOut( X, _po_Rect->top, sz_Name, strlen( sz_Name ) );
	}

	while ( 1 )
	{
		dx = ceil( x ) - x;
		if (dx == 0 ) dx = 1;
		DX = (int) (dx * mi_UnitWidth);
		if (X + DX > _po_Rect->right )
		{
			DX = _po_Rect->right - X;
			dx = (float) DX / (float) mi_UnitWidth;
		}
		else
		{
			/* show unit at end */
			itoa( ((int)ceil(x+dx)), sz_Name, 10);
			pDC->TextOut( X + DX, _po_Rect->top, sz_Name, strlen( sz_Name ) );
		}

		/* display bar */
		pDC->FillSolidRect( X, _po_Rect->top, DX, 2, maaul_Color[ _i_Color ][ i_Odd ? COLOR_PROG0 : COLOR_PROG1 ] );

		/* display cursor */
		if ( ( pst_Channel->f_ChannelBlend >= x ) && ( pst_Channel->f_ChannelBlend <= x + dx ) && !i_Cursor )
		{
			i_Cursor = 1;
			CursorX = X + (int)((pst_Channel->f_ChannelBlend - x) * mi_UnitWidth) - 1;
			CursorY = _po_Rect->top - 3;
			pDC->FillSolidRect( CursorX, CursorY, 3, 8, maaul_Color[ _i_Color ][ COLOR_CURSOR ] );
		}

		/* next step */
		X += DX;
		x += dx;
		mast_Line[ _i_Data ].f_ProgEnd = x;
		mast_Line[ _i_Data ].o_Prog.right = X + 10;
		i_Odd = 1 - i_Odd;

		/* stop loop */
		if (X == _po_Rect->right)
			break;
		if ( x >= pst_Channel->l_NbData )
			break;
	}

	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_MorphingSlidersDialog::Line_GetHeight( int _i_Data )
{
	return 50;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingSlidersDialog::Window_SetHeight( void )
{
	int		i_Line;
	CRect	o_Rect;

	mi_WindowHeight = 0;
	for (i_Line = 0; i_Line < 8; i_Line++)
	{
		if (mast_Line[ i_Line ].i_Visible )
			mi_WindowHeight += Line_GetHeight( i_Line );
	}

	mi_WindowHeight += 10;

	GetClientRect( &o_Rect );
	SetWindowPos( mpo_Owner, 0, 0, o_Rect.Width(), mi_WindowHeight , SWP_NOREDRAW | SWP_NOMOVE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingSlidersDialog::Line_SetProgFromPoint( int _i_Line, CPoint _o_Pt )
{
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	int									X;
	float								f, x;

	pst_Channel = mpst_Morph->dst_MorphChannel + mast_Line[ _i_Line ].i_Index;

	X = (*(short *) &_o_Pt.x) - mast_Line[ _i_Line ].o_Prog.left;
	if ( X < 10 )
	{
		if (mast_Line[ _i_Line ].f_ProgOffset - 0.01f < 0 )
			mast_Line[ _i_Line ].f_ProgOffset = 0;
		else
			mast_Line[ _i_Line ].f_ProgOffset -= 0.01f;
		pst_Channel->f_ChannelBlend += mast_Line[ _i_Line ].f_ProgOffset;
		Invalidate();
		LINK_Refresh();
		return;
	}

	if (X > mast_Line[ _i_Line ].o_Prog.Width() - 10)
	{
		if (mast_Line[ _i_Line ].f_ProgEnd + 0.01f > (float) pst_Channel->l_NbData )
		{
			mast_Line[ _i_Line ].f_ProgOffset = 0;
			pst_Channel->f_ChannelBlend = (float) pst_Channel->l_NbData;
		}
		else
		{
			mast_Line[ _i_Line ].f_ProgOffset += 0.01f;
			pst_Channel->f_ChannelBlend += 0.01f;
		}
		Invalidate();
		LINK_Refresh();
		return;
	}
	
	X -= 10;
	f = ((float)X) /((float)  mast_Line[ _i_Line ].o_Prog.Width() - 20 );
	x = mast_Line[ _i_Line ].f_ProgOffset + (mast_Line[ _i_Line ].f_ProgEnd - mast_Line[ _i_Line ].f_ProgOffset ) * f;

	pst_Channel->f_ChannelBlend = x;
	Invalidate();
	LINK_Refresh();
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingSlidersDialog::Line_SetFactorFromPoint( int _i_Line, CPoint _o_Pt )
{
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	int									X;
	float								f, x;

	pst_Channel = mpst_Morph->dst_MorphChannel + mast_Line[ _i_Line ].i_Index;

	X = ( *(short *) &_o_Pt.x) - mast_Line[ _i_Line ].o_Factor.left;
	if ( X < 10 )
	{
		mast_Line[ _i_Line ].f_FactorOffset -= 0.01f;
		mast_Line[ _i_Line ].f_FactorEnd -= 0.01f;
		pst_Channel->f_Blend = mast_Line[ _i_Line ].f_FactorOffset;
		Invalidate();
		LINK_Refresh();
		return;
	}

	if (X > mast_Line[ _i_Line ].o_Factor.Width() - 10)
	{
		mast_Line[ _i_Line ].f_FactorOffset += 0.01f;
		mast_Line[ _i_Line ].f_FactorEnd += 0.01f;
		pst_Channel->f_Blend = mast_Line[ _i_Line ].f_FactorEnd;
		Invalidate();
		LINK_Refresh();
		return;
	}
	
	X -= 10;
	f = ((float)X) /((float)  mast_Line[ _i_Line ].o_Factor.Width() - 20 );
	x = mast_Line[ _i_Line ].f_FactorOffset + (mast_Line[ _i_Line ].f_FactorEnd - mast_Line[ _i_Line ].f_FactorOffset ) * f;

	pst_Channel->f_Blend = x;
	Invalidate();
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_MorphingSlidersDialog::PreTranslateMessage(MSG *pMsg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint				o_Pt, o_Pos;
	CRect				o_Rect;
	int					i_Index;
	EDI_cl_BaseFrame	*po_OutputFrame;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (pMsg->message == WM_MOUSEWHEEL)
	{
		short w_Wheel = (short) HIWORD( pMsg->wParam);
		if ( w_Wheel > 0)
			mi_UnitWidth += 10;
		else
			mi_UnitWidth -= 10;
		if (mi_UnitWidth >= 200 )
			mi_UnitWidth = 200;
		if (mi_UnitWidth < 10 )
			mi_UnitWidth = 10;
		Invalidate();
		return TRUE;
	}

	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}

	if(pMsg->message == WM_LBUTTONDOWN)
	{
		o_Pt.x = LOWORD(pMsg->lParam);
		o_Pt.y = HIWORD(pMsg->lParam);

		for ( i_Index = 0; i_Index < mi_LineNb; i_Index++)
		{
			if ( !mast_Line[ i_Index ].i_Visible) continue;

			o_Rect = mast_Line[ i_Index ].o_Factor;
			if ( o_Rect.PtInRect( o_Pt ) )
			{
				mi_CaptureLine = i_Index;
				mi_Capture = 1;
				SetCapture();
				Line_SetFactorFromPoint( i_Index, o_Pt );
				break;
			}
			o_Rect = mast_Line[ i_Index ].o_Prog;
			if ( o_Rect.PtInRect( o_Pt ) )
			{
				mi_CaptureLine = i_Index;
				mi_Capture = 2;
				SetCapture();
				Line_SetProgFromPoint( i_Index, o_Pt );
				break;
			}
		}

		if (!mi_Capture)
		{
			mi_Capture = 3;
			SetCapture();
			GetCursorPos( (LPPOINT) &mo_CapturePt );
		}
	}

	if(pMsg->message == WM_LBUTTONUP)
	{
		if (mi_Capture)
		{
			mi_Capture = 0;
			ReleaseCapture();
		}
	}
	
	if ((pMsg->message == WM_MOUSEMOVE) && mi_Capture )
	{
		o_Pt.x = LOWORD(pMsg->lParam);
		o_Pt.y = HIWORD(pMsg->lParam);

		if (mi_Capture == 1)
		{
			Line_SetFactorFromPoint( mi_CaptureLine, o_Pt );
			return TRUE;
		}
		else if (mi_Capture == 2 )
		{
			Line_SetProgFromPoint( mi_CaptureLine, o_Pt );
			return TRUE;
		}
		else if (mi_Capture == 3)
		{
			GetWindowRect( (LPRECT) &o_Rect );
			GetCursorPos( (LPPOINT) &o_Pt );
			o_Rect.left += o_Pt.x - mo_CapturePt.x;
			o_Rect.top += o_Pt.y - mo_CapturePt.y;
			SetWindowPos( mpo_Owner, o_Rect.left, o_Rect.top, 0, 0, SWP_NOSIZE );
			mo_CapturePt = o_Pt;
			return TRUE;
		}
	}

	if (pMsg->message == WM_RBUTTONDOWN)
	{
		o_Pt.x = LOWORD(pMsg->lParam);
		o_Pt.y = HIWORD(pMsg->lParam);

		for ( i_Index = 0; i_Index < mi_LineNb; i_Index++)
		{
			if (mast_Line[ i_Index ].o_Rect.PtInRect( o_Pt ) )
			{
				EMEN_cl_SubMenu o_Menu(FALSE);
				CPoint			pt;
				int				i, i_Res;

				M_MF()->InitPopupMenuAction(NULL, &o_Menu);

				po_OutputFrame = M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
				if ( po_OutputFrame )
				{
					M_MF()->AddPopupMenuAction(NULL, &o_Menu, 100, TRUE, "plug to 3DView (top)");
					M_MF()->AddPopupMenuAction(NULL, &o_Menu, 101, TRUE, "plug to 3DView (bottom)");
				}
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 102, TRUE, "synchronize with main morph editor", DFCS_BUTTONRADIO | (mi_Synchronise ? DFCS_CHECKED : 0) );

				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, "");
				for (i = 0; i < mpst_Morph->l_NbChannel; i++)
				{
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, i+1, TRUE, mpst_Morph->dst_MorphChannel[ i ].sz_Name, -1 );
				}
				GetCursorPos(&pt);
				i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
				if ((i_Res >= 1) && (i_Res <= mpst_Morph->l_NbChannel))
				{
					mast_Line[ i_Index ].i_Index = i_Res - 1;
					Invalidate();
				}
				else if (i_Res >= 100 )
				{
					switch ( i_Res )
					{
					case 100:	PlugTo3DView( 0 ); break;
					case 101:	PlugTo3DView( 1 ); break;
					case 102:	mi_Synchronise = !mi_Synchronise;
								if (mi_Synchronise && mpo_Owner->mpst_Morph && mpo_Owner->mpst_CurChannel )
								{
									mast_Line[ i_Index ].i_Index = mpo_Owner->mpst_CurChannel - mpo_Owner->mpst_Morph->dst_MorphChannel;
									Invalidate();
								}
								break;
					}
				}
			}
		}
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_MorphingSlidersDialog::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 200;

	if ( mi_WindowHeight )
	{
		lpMMI->ptMaxTrackSize.y = mi_WindowHeight;
		lpMMI->ptMinTrackSize.y = mi_WindowHeight;
	}
}


#endif /* ACTIVE_EDITORS */

