/*$T DIAtimeinterpolation_dlg.cpp GC! 1.081 10/27/00 11:08:49 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAtimeinterpolation_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
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
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"
#include "LINKs/LINKmsg.h"

BEGIN_MESSAGE_MAP(EDIA_cl_TimeInter, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDOWN()
    ON_BN_CLICKED( IDC_BUTTON_SAVE, OnButtonSave )
    ON_BN_CLICKED( IDC_BUTTON_LOAD, OnButtonLoad )
    ON_BN_CLICKED( IDC_BUTTON_LOADCLOSE, OnButtonLoadClose )
    ON_LBN_SELCHANGE( IDC_LIST_FILE, OnListFileChange )
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_TimeInter::EDIA_cl_TimeInter(EVE_tdst_Event *_pst_Evt) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_TIMEINTER)
{
	mpst_Evt = _pst_Evt;
	mi_Picked = -1;
    mi_TraceRect = -1;
    mi_PickedRect = -1;
	o_InsertPoint.x = -1;
    mi_SelRect[0] = 0;
    mi_SelRect[1] = 0;
    ml_NbKey[0] = 0;
    ml_NbKey[1] = 0;
    ml_NbKey[2] = 0;
    mpst_HistoryRoot = mpst_History = (EDIA_tdst_HistoryList *) L_malloc( sizeof( EDIA_tdst_HistoryList ) );
    L_memset( mpst_HistoryRoot, 0, sizeof( EDIA_tdst_HistoryList ) );
    
	GetEventData();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_TimeInter::~EDIA_cl_TimeInter(void)
{
    int i;
    EDIA_tdst_HistoryList *pst_Old;

    if ( (mi_Picked != -1) || (mi_TraceRect != -1) || (mi_PickedRect != -1) )
    {
	    ReleaseCapture();
	    ClipCursor(&mst_OldClipCursor);
    }

    for (i = 0; i < 3; i++)
    {
        if (ml_NbKey[i])
            L_free( mpst_Curve[i]);
    }

    if (mpst_History)
    {
        if (mpst_History->pst_Next)
            mpst_History->pst_Next->pst_Key = NULL;
            
        while (mpst_History->pst_Next)
            mpst_History = mpst_History->pst_Next;

        while (mpst_History)
        {
            pst_Old = mpst_History;
            mpst_History = pst_Old->pst_Prev;
            if ( pst_Old->pst_Key ) L_free( pst_Old->pst_Key );
            L_free( pst_Old );
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_TimeInter::OnInitDialog(void)
{
    CRect   o_Rect;
    int     i;

    GetWindowRect(&o_Rect);
    mi_WindowHeight = o_Rect.bottom - o_Rect.top;
    mi_WindowWidthBig = o_Rect.right - o_Rect.left;

    GetDlgItem(IDC_STATIC_CURVE )->GetWindowRect(&mo_DrawRect[0]);
    GetDlgItem(IDC_STATIC_CURVE)->ShowWindow(SW_HIDE);

    ScreenToClient(&mo_DrawRect[0]);
    mi_WindowWidthSmall = mo_DrawRect[0].right - mo_DrawRect[0].left + 16;
    
    GetDlgItem( IDC_STATIC_CURVE2 )->GetWindowRect( &mo_DrawRect[1] );
    GetDlgItem(IDC_STATIC_CURVE2)->ShowWindow(SW_HIDE);
    ScreenToClient(&mo_DrawRect[1]);

    for ( i = 0; i < 2; i++)
    {
        mo_CurveRect[i] = mo_DrawRect[i];
        mo_CurveRect[i].DeflateRect(16, 16);
        mo_XRect[i] = mo_CurveRect[i];
        mo_XRect[i].top = mo_XRect[i].bottom;
        mo_XRect[i].bottom = mo_XRect[i].top + 4;

        mo_YRect[i] = mo_CurveRect[i];
        mo_YRect[i].right = mo_YRect[i].left;
        mo_YRect[i].left = mo_YRect[i].right - 4;
	    mi_CurveWidth[i] = mo_CurveRect[i].Width();
	    mi_CurveHeight[i] = mo_CurveRect[i].Height();
	    mf_Time2Width[i] = (float) mi_CurveWidth[i];
	    mf_Time2Height[i] = (float) mi_CurveHeight[i];
    }

    OnButtonLoadClose();

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_TimeInter::PreTranslateMessage(MSG *pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
        if ( pMsg->wParam == VK_SPACE )
            return 1;

		OnKeyDown(pMsg->wParam);
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnOK(void)
{
	SetEventData();
	EDIA_cl_BaseDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnCancel(void)
{
	EDIA_cl_BaseDialog::OnCancel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	CDC		*pDC;
	int		x1, y1;
	CPen	o_RedPen, o_BluePen, o_YellowPen;
	CPen	o_BlackPen, o_BlackDotPen;
	void	*p_OldPen;
	int		i, curve;
    EDIA_tdst_EditKey *C;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnPaint();

	pDC = GetDC();
	o_RedPen.CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
	o_BluePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	o_YellowPen.CreatePen(PS_SOLID, 1, RGB(0xFF, 0xFF, 0));
	o_BlackPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	o_BlackDotPen.CreatePen(PS_DOT, 1, RGB(0, 0, 0));

    /* draw curves */
	p_OldPen = pDC->SelectObject(o_BlackPen);

    for (curve = 0; curve < 2; curve++)
    {
        C = mpst_Curve[curve];

        pDC->SelectObject(o_BlackPen);
	    pDC->MoveTo(mo_CurveRect[curve].left, mo_CurveRect[curve].top);
	    pDC->LineTo(mo_CurveRect[curve].left, mo_CurveRect[curve].bottom);
	    pDC->LineTo(mo_CurveRect[curve].right, mo_CurveRect[curve].bottom);

	    pDC->SelectObject(o_BlackDotPen);
	    pDC->LineTo(mo_CurveRect[curve].right, mo_CurveRect[curve].top);
	    pDC->LineTo(mo_CurveRect[curve].left, mo_CurveRect[curve].top);

	    pDC->SetTextColor(RGB(0, 0, 0));
	    pDC->SetBkMode(TRANSPARENT);

	    x1 = mo_CurveRect[curve].left - 16;
	    y1 = mo_CurveRect[curve].bottom;
	    o_Rect = CRect(x1, y1, x1 + 16, y1 + 16);
	    pDC->DrawText("0", -1, &o_Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	    y1 = mo_CurveRect[curve].top - 16;;
	    o_Rect = CRect(x1, y1, x1 + 16, y1 + 16);
	    pDC->DrawText("1", -1, &o_Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	    x1 = mo_CurveRect[curve].right;
	    y1 = mo_CurveRect[curve].bottom;
	    o_Rect = CRect(x1, y1, x1 + 16, y1 + 16);
	    pDC->DrawText("1", -1, &o_Rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        /* draw selection rect */
        if (mi_SelRect[curve])
            pDC->Rectangle( &mo_SelRect[curve] );

	    pDC->SelectObject(o_BluePen);
	    pDC->MoveTo(mo_CurveRect[curve].left, mo_CurveRect[curve].bottom);

        for(i = 1; i < ml_NbKey[curve]; i++)
        {
            C[i].x = mo_CurveRect[curve].left + (int) (C[i].tx * mf_Time2Width[curve]);
            C[i].y = mo_CurveRect[curve].bottom - (int) (C[i].ty * mf_Time2Height[curve]);
		    pDC->LineTo(C[i].x, C[i].y);
        }

        for(i = 1; i < ml_NbKey[curve]; i++)
	    {
		    pDC->SelectObject( C[i].sel ? o_YellowPen : o_RedPen);
		    pDC->MoveTo(C[i].x + 3, C[i].y);
		    pDC->LineTo(C[i].x - 3, C[i].y);
		    pDC->MoveTo(C[i].x, C[i].y + 3);
		    pDC->LineTo(C[i].x, C[i].y - 3);

		    pDC->MoveTo(C[i].x, mo_CurveRect[curve].bottom + 1);
		    pDC->LineTo(C[i].x, mo_CurveRect[curve].bottom + 4);

		    pDC->MoveTo(mo_CurveRect[curve].left - 1, C[i].y);
		    pDC->LineTo(mo_CurveRect[curve].left - 4, C[i].y);
        }
    }

	pDC->SelectObject(p_OldPen);
	DeleteObject(o_RedPen);
	DeleteObject(o_BluePen);
	DeleteObject(o_YellowPen);
	DeleteObject(o_BlackPen);
	DeleteObject(o_BlackDotPen);
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnMouseMove(UINT ui_Flags, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	int		            i;
	CPoint	            o_Pos, o_Delta;
    EDIA_tdst_EditKey   *pst_Key;
    CSize               o_OldSize;
	/*~~~~~~~~~~~~~~~~~~~*/

	if(mo_CurveRect[0].PtInRect(pt))
		o_InsertPoint = pt;
	else
		o_InsertPoint.x = -1;

	if ( (mi_Picked != -1) || ( mi_PickedRect != -1) )
	{
        if ( mi_PickedRect == 1 )
            return;
		
        GetCursorPos(&o_Pos);
		o_Delta = o_Pos - mo_MousePos;

        if (mi_PickedCoord & 3)
        {
            pst_Key = mpst_Curve[0] + 1;
		    for(i = 1; i < ml_NbKey[0] - 1; i++, pst_Key++)
		    {
			    if(!pst_Key->sel) continue;
			    if(mi_PickedCoord & 1) pst_Key->tx += ((float) (o_Delta.x)) / mf_Time2Width[0];
			    if(mi_PickedCoord & 2) pst_Key->ty -= ((float) (o_Delta.y)) / mf_Time2Height[0];
		    }

            if (mi_SelRect[0])
            {
                mo_SelRect[0].OffsetRect( o_Delta );
                mo_SelRect[0] &= mo_CurveRect[0];
            }
        }
        else
        {
            o_OldSize = mo_SelRect[0].Size();
            switch ( mi_PickedCoord >> 2)
            {
            case 1:
                mo_SelRect[0].left += o_Delta.x;
                mo_SelRect[0].top += o_Delta.y;
                ScaleKey( o_OldSize, -1, -1 );
                break;
            case 2:
                mo_SelRect[0].top += o_Delta.y;
                ScaleKey( o_OldSize, 0, -1 );
                break;
            case 3:
                mo_SelRect[0].right += o_Delta.x;
                mo_SelRect[0].top += o_Delta.y;
                ScaleKey( o_OldSize, 1, -1 );
                break;
            case 4:
                mo_SelRect[0].right += o_Delta.x;
                ScaleKey( o_OldSize, 1, 0 );
                break;
            case 5:
                mo_SelRect[0].right += o_Delta.x;
                mo_SelRect[0].bottom += o_Delta.y;
                ScaleKey( o_OldSize, 1, 1 );
                break;
            case 6:
                mo_SelRect[0].bottom += o_Delta.y;
                ScaleKey( o_OldSize, 0, 1 );
                break;
            case 7:
                mo_SelRect[0].left += o_Delta.x;
                mo_SelRect[0].bottom += o_Delta.y;
                ScaleKey( o_OldSize, -1, 1 );
                break;
            case 8:
                mo_SelRect[0].left += o_Delta.x;
                ScaleKey( o_OldSize, -1, 0 );
                break;
            }
            
        }

        mo_MousePos = o_Pos;

        if ( ui_Flags & MK_SHIFT)
            AdjustKey((o_Delta.x > 0) ? 1 : -1);
        else
            SortKey( 0 );

		InvalidateRect(mo_DrawRect[0]);
	}
    else
	{
        if ( mi_TraceRect == -1 )
        {
		    GetPickable(pt);
		    if(mi_Pickable == -1)
		    {
			    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		    }
		    else
		    {
			    if(mi_PickedCoord == 1)
				    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
			    else if(mi_PickedCoord == 2)
				    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
			    else
				    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
		    }
        }
        else
        {
            if(mi_PickedCoord & 1)
                mo_SelRect[mi_TraceRect].right = pt.x;
            if(mi_PickedCoord & 3)
                mo_SelRect[mi_TraceRect].bottom = pt.y;
            InvalidateRect(mo_DrawRect[mi_TraceRect]);
        }
	}
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnLButtonDown(UINT ui, CPoint pt)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
    int     i;
	/*~~~~~~~~~~~*/

    GetDlgItem( IDOK)->SetFocus();

	if(mi_Pickable == -1) 
    {
        mi_TraceRect = -1;

        for (i = 0; i < 2; i++)
        {
            if ( ( mi_SelRect[i] ) && (mo_SelRect[i].PtInRect( pt ) ) )
            {
                mi_PickedCoord = 3;
                mi_PickedRect = i;

                if (i == 0)
                {
                    mo_SelRect[0].DeflateRect(4, 4);
                    o_Rect = mo_SelRect[0];
                    if ( !mo_SelRect[0].PtInRect(pt) )
                    {
                        if ( pt.y < mo_SelRect[0].top + 4)
                        {
                            if (pt.x < mo_SelRect[0].left + 4) 
                            {
                                mi_PickedCoord = 1;
                                o_Rect.left = mo_CurveRect[0].left;
                                o_Rect.top = mo_CurveRect[0].top;
                            }
                            else if (pt.x > mo_SelRect[0].right - 4) 
                            {
                                mi_PickedCoord = 3;
                                o_Rect.right = mo_CurveRect[0].right;
                                o_Rect.top = mo_CurveRect[0].top;
                            }
                            else 
                            {
                                mi_PickedCoord = 2;
                                o_Rect.top = mo_CurveRect[0].top ;
                            }
                        }
                        else if (pt.y > mo_SelRect[0].bottom - 4)
                        {
                            if (pt.x < mo_SelRect[0].left + 4) 
                            {
                                mi_PickedCoord = 7;
                                o_Rect.left = mo_CurveRect[0].left;
                                o_Rect.bottom = mo_CurveRect[0].bottom;
                            }
                            else if (pt.x > mo_SelRect[0].right - 4) 
                            {
                                mi_PickedCoord = 5;
                                o_Rect.right = mo_CurveRect[0].right;
                                o_Rect.bottom = mo_CurveRect[0].bottom;
                            }
                            else 
                            {
                                mi_PickedCoord = 6;
                                o_Rect.bottom = mo_CurveRect[0].bottom;
                            }
                        }
                        else
                        {
                            if (pt.x < mo_SelRect[0].left + 4) 
                            {
                                mi_PickedCoord = 8;
                                o_Rect.left = mo_CurveRect[0].left;
                            }
                            else 
                            {
                                mi_PickedCoord = 4;
                                o_Rect.right = mo_CurveRect[0].right;
                            }
                        }
                        mi_PickedCoord <<= 2;
                    }
                    mo_SelRect[i].InflateRect(4, 4);
                }
            }
            else if (mo_XRect[i].PtInRect( pt ) )
            {
                mi_TraceRect = i;
                mi_PickedCoord = 1;
                mi_SelRect[i] = 1;
                mo_SelRect[i].left = mo_SelRect[i].right = pt.x;
                mo_SelRect[i].top = mo_CurveRect[i].top;
                mo_SelRect[i].bottom = mo_CurveRect[i].bottom;
            }
            else if (mo_YRect[i].PtInRect( pt ) )
            {
                mi_TraceRect = i;
                mi_PickedCoord = 2;
                mi_SelRect[i] = 1;
                mo_SelRect[i].top = mo_SelRect[i].bottom = pt.y;
                mo_SelRect[i].left = mo_CurveRect[i].left;
                mo_SelRect[i].right = mo_CurveRect[i].right;
            }
            else if (mo_CurveRect[i].PtInRect( pt ) )
            {
                mi_TraceRect = i;
                mi_PickedCoord = 3;
                mi_SelRect[i] = 1;
                mo_SelRect[i].left = mo_SelRect[i].right = pt.x;
                mo_SelRect[i].top = mo_SelRect[i].bottom = pt.y;
            }
        }
        if ( (mi_TraceRect == -1) && (mi_PickedRect == -1))
            return;

        HistPush();
        
        if (mi_TraceRect != -1)
        {
            if(!(ui & MK_CONTROL))
                SelNone( mpst_Curve[mi_TraceRect], ml_NbKey[mi_TraceRect] );
        }
    }
    else
    {
        HistPush();
	    if(!(ui & MK_CONTROL))
	    {
            if ( !mpst_Curve[0][mi_Pickable].sel )
            {
                SelNone( mpst_Curve[0], ml_NbKey[0] );
                mpst_Curve[0][mi_Pickable].sel = 1;
            }
	    }
	    else
	    {
		    mpst_Curve[0][mi_Pickable].sel ^= 1;
	    }
	    mi_Picked = mi_Pickable;
    }

	SetCapture();
	GetCursorPos(&mo_MousePos);

    if ( !(mi_PickedCoord > 3) )
    {
        i = (mi_Picked != -1) ? 0 : ((mi_TraceRect == -1) ? mi_PickedRect : mi_TraceRect);
	    o_Rect = mo_CurveRect[i];
	    if((mi_PickedCoord & 1) == 0)
	    {
		    o_Rect.right = o_Rect.left;
		    o_Rect.left -= 4;
	    }
	    else if((mi_PickedCoord & 2) == 0)
	    {
		    o_Rect.top = o_Rect.bottom;
		    o_Rect.bottom += 4;
	    }
    }

	ClientToScreen(o_Rect);
	GetClipCursor(&mst_OldClipCursor);
	ClipCursor((LPRECT) & o_Rect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnLButtonUp(UINT ui, CPoint pt)
{
    int tmp;

	if ( (mi_Picked == -1) && (mi_TraceRect == -1) && (mi_PickedRect == -1) )return;

	mi_Picked = -1;
    mi_PickedRect = -1;

    
    if (mi_TraceRect != -1)
    {
        if (mo_SelRect[mi_TraceRect].left > mo_SelRect[mi_TraceRect].right)
        {
            tmp = mo_SelRect[mi_TraceRect].left;
            mo_SelRect[mi_TraceRect].left = mo_SelRect[mi_TraceRect].right;
            mo_SelRect[mi_TraceRect].right = tmp;
        }
        
        if (mo_SelRect[mi_TraceRect].top > mo_SelRect[mi_TraceRect].bottom)
        {
            tmp = mo_SelRect[mi_TraceRect].top;
            mo_SelRect[mi_TraceRect].top = mo_SelRect[mi_TraceRect].bottom;
            mo_SelRect[mi_TraceRect].bottom = tmp;
        }

        if ( (mo_SelRect[mi_TraceRect].Width() < 4) || (mo_SelRect[mi_TraceRect].Height() < 4) )
            mi_SelRect[ mi_TraceRect ] = 0;
        else
            SelectPointInRect( mi_TraceRect );

        mi_TraceRect = -1;
    }
    
	ReleaseCapture();
	ClipCursor(&mst_OldClipCursor);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnLButtonDblClk( UINT ui, CPoint pt )
{
    char sz_Text[ BIG_C_MaxLenName ], *sz_Ext;
    int  i;

    if ( ( mi_SelRect[1] ) && (mo_SelRect[1].PtInRect( pt ) ) )
    {
        OnCopy( 1 );
        if (ml_NbKey[2] == 0) return;

        SelNone( mpst_Curve[0], ml_NbKey[0] );
        Copy();

        mo_SelRect[0] = mo_SelRect[1];
        mo_SelRect[0] -= mo_CurveRect[1].TopLeft();
        
        mo_SelRect[0].left = (int) ((float) (mo_SelRect[0].left * mo_CurveRect[0].Width()) / (float) mo_CurveRect[1].Width());
        mo_SelRect[0].right = (int) ((float) (mo_SelRect[0].right * mo_CurveRect[0].Width()) / (float) mo_CurveRect[1].Width());
        mo_SelRect[0].top = (int) ((float) (mo_SelRect[0].top * mo_CurveRect[0].Height()) / (float) mo_CurveRect[1].Height());
        mo_SelRect[0].bottom = (int) ((float) (mo_SelRect[0].bottom * mo_CurveRect[0].Height()) / (float) mo_CurveRect[1].Height());
        
        mo_SelRect[0] += mo_CurveRect[0].TopLeft();

        mi_SelRect[0] = 1;

        InvalidateRect(mo_DrawRect[0]);
    }
    else if ( mo_CurveRect[1].PtInRect( pt ) )
    {
        if ( !ml_NbKey[1] ) return;

        i = ((CListBox *) GetDlgItem( IDC_LIST_FILE ))->GetCurSel();
        if (i == LB_ERR) return;

        SelAll( mpst_Curve[0], ml_NbKey[0] );
        SelDelete();
        SelAll( mpst_Curve[1], ml_NbKey[1] );
        OnCopy( 1 );
        Copy();
        InvalidateRect(mo_DrawRect[0]);

        ((CListBox *) GetDlgItem( IDC_LIST_FILE ))->GetText( i, sz_Text );
        sz_Ext = L_strrchr( sz_Text, '.' );
        if (sz_Ext) *sz_Ext = 0;
        GetDlgItem( IDC_EDIT_NAME )->SetWindowText( sz_Text );


    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnRButtonDown( UINT ui, CPoint pt )
{
    EMEN_cl_SubMenu o_Menu(FALSE);
    int             i_Res;
    	
    GetCursorPos(&pt);
	M_MF()->InitPopupMenuAction(NULL, &o_Menu);

	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "HFlip", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "VFlip", -1);

	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	switch(i_Res)
	{
	case 0:
	case -1:
		break;
	case 1:
		OnHFlip();
		break;
	case 2:
		OnVFlip();
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnKeyDown(UINT nChar)
{
	/*~~~~~~*/
	int		i;
	float	x;
	/*~~~~~~*/

    if (GetFocus() == GetDlgItem( IDC_EDIT_NAME) ) return;
	if(mi_Picked != -1) return;

	if(nChar == VK_DELETE)
	{
        if ( SomethingSel( mpst_Curve[0], ml_NbKey[0] ) )
        {
            HistPush();
            SelDelete();
            mi_SelRect[0] = 0;
		    InvalidateRect(mo_DrawRect[0]);
        }
	}
	else if(nChar == VK_INSERT)
	{
		if(o_InsertPoint.x == -1) return;
		x = ((float) (o_InsertPoint.x - mo_CurveRect[0].left)) / mf_Time2Width[0];

        HistPush();

		for(i = 1; i < ml_NbKey[0]; i++)
			if(x < mpst_Curve[0][i].tx) break;

		if(i == ml_NbKey[0] )
		{
			i = ml_NbKey[0] - 1;
			x = 1.0f;
		}

		/* realloc array */
        mpst_Curve[0] = (EDIA_tdst_EditKey *) L_realloc( mpst_Curve[0], (ml_NbKey[0] + 1) * sizeof( EDIA_tdst_EditKey) );

		/* decal other pos */
        L_memmove( mpst_Curve[0] + i + 1, mpst_Curve[0] + i, (ml_NbKey[0] - i) * sizeof( EDIA_tdst_EditKey) );
		ml_NbKey[0]++;

		/* insert new pos */
        mpst_Curve[0][i].tx = x;
        mpst_Curve[0][i].ty = ((float) (mo_CurveRect[0].bottom - o_InsertPoint.y)) / mf_Time2Height[0];;
		InvalidateRect(mo_DrawRect[0]);
	}
    else if (nChar == 'C')
    {
        if (GetAsyncKeyState( VK_CONTROL ) < 0)
            OnCopy(0);
    }
    else if (nChar == 'V')
    {
        if (GetAsyncKeyState( VK_CONTROL ) < 0)
        {
            mi_SelRect[0] = 0;
            SelNone( mpst_Curve[0], ml_NbKey[0]);
            Copy();
            InvalidateRect(mo_DrawRect[0]);
        }
    }
    else if (nChar == 'Z')
    {
        if (GetAsyncKeyState( VK_CONTROL ) < 0)
            HistPop();
    }
    else if (nChar == 'Y')
    {
        if (GetAsyncKeyState( VK_CONTROL ) < 0)
        {
            if (mpst_History && mpst_History->pst_Next && mpst_History->pst_Next->pst_Next )
            {
                mpst_History = mpst_History->pst_Next;
                ml_NbKey[0] = mpst_History->pst_Next->l_NbKey;
                mpst_Curve[0] = mpst_History->pst_Next->pst_Key;
                mi_SelRect[0] = mpst_History->pst_Next->i_SelRect;
                mo_SelRect[0] = mpst_History->pst_Next->o_SelRect;
                InvalidateRect(mo_DrawRect[0]);
            }
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnButtonSave()
{
    char    sz_Path[ BIG_C_MaxLenPath ];
    char    sz_Name[ BIG_C_MaxLenName ];
    int     i;
        
    GetDlgItem( IDC_EDIT_NAME )->GetWindowText( sz_Name, BIG_C_MaxLenName - 1);
    sprintf( sz_Name + strlen(sz_Name), ".tik" );
    sprintf( sz_Path, "%s/TimeInterpolationKey", EDI_Csz_Path_EditData );

    SAV_Begin( sz_Path, sz_Name );

    SAV_Buffer( ".TIK", 4);
    for (i = 0; i < ml_NbKey[0]; i++)
    {
        SAV_Buffer( &mpst_Curve[0][i].tx, 4 );
        SAV_Buffer( &mpst_Curve[0][i].ty, 4 );
    }
    
    SAV_ul_End();

    if ( GetDlgItem( IDC_BUTTON_LOADCLOSE )->IsWindowVisible() )
        ListRefresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnButtonLoad()
{
    SetWindowPos( NULL, 0, 0, mi_WindowWidthBig, mi_WindowHeight, SWP_NOMOVE | SWP_NOZORDER );
    GetDlgItem( IDC_BUTTON_LOAD )->ShowWindow( SW_HIDE );
    GetDlgItem( IDC_BUTTON_LOADCLOSE )->ShowWindow( SW_SHOW );
    ListRefresh();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnButtonLoadClose()
{
    SetWindowPos( NULL, 0, 0, mi_WindowWidthSmall, mi_WindowHeight, SWP_NOMOVE | SWP_NOZORDER );
    GetDlgItem( IDC_BUTTON_LOAD )->ShowWindow( SW_SHOW );
    GetDlgItem( IDC_BUTTON_LOADCLOSE )->ShowWindow( SW_HIDE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnListFileChange()
{
    int     i;
    ULONG   ul_Index, ul_Size;
    float   *pf_Data;

    i = ((CListBox *) GetDlgItem( IDC_LIST_FILE ))->GetCurSel();
    if (i == LB_ERR) return;

    ul_Index = (ULONG ) ((CListBox *) GetDlgItem( IDC_LIST_FILE ))->GetItemData( i );

    pf_Data = (float *) BIG_pc_ReadFileTmp( BIG_PosFile(ul_Index), &ul_Size);
    if ( L_strncmp( (char *) pf_Data, ".TIK", 4 ) )
        return;
    pf_Data ++;

    if ( ml_NbKey[1] != 0)
        L_free( mpst_Curve[1] );

    ml_NbKey[1] = (ul_Size - 4) / 8;
    mpst_Curve[1] = (EDIA_tdst_EditKey *) L_malloc( ml_NbKey[1] * sizeof( EDIA_tdst_EditKey) );

    for (i = 0; i < ml_NbKey[1]; i++)
    {
        mpst_Curve[1][i].tx = *pf_Data++;
        mpst_Curve[1][i].ty = *pf_Data++;
        mpst_Curve[1][i].sel= 0;
    }

    InvalidateRect(mo_DrawRect[1]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnCopy( int curve )
{
    int i, n;

    if ( !mi_SelRect[curve] )
        SelAll( mpst_Curve[curve], ml_NbKey[curve]);

    n = SelCount( mpst_Curve[curve], ml_NbKey[curve] );
    AllocData( 2, n );

    for ( i = 0, n = 0; n < ml_NbKey[curve]; n++)
    {
        if (mpst_Curve[curve][n].sel)
        {
            mpst_Curve[2][i].tx = mpst_Curve[curve][n].tx;
            mpst_Curve[2][i].ty = mpst_Curve[curve][n].ty;
            i++;
        }
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
void EDIA_cl_TimeInter::AllocData( int i, int _nb )
{
    if ( ml_NbKey[i] )
    {
        mpst_Curve[i] = (EDIA_tdst_EditKey *) L_realloc( mpst_Curve[i], sizeof(EDIA_tdst_EditKey) * _nb);
    }
    else
    {
        mpst_Curve[i] = (EDIA_tdst_EditKey *) L_malloc( sizeof(EDIA_tdst_EditKey) * _nb);
    }

    ml_NbKey[i] = _nb;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::GetEventData(void)
{
	/*~~~~~~~~~~~~~*/
	float	*pf_Data;
	int		i;
	/*~~~~~~~~~~~~~*/

	pf_Data = EVE_pf_Event_InterpolationKey_GetTime(mpst_Evt);

    i = (pf_Data == NULL) ? 2 : ( (*(int *) pf_Data) + 1 );

    AllocData( 0, i );

	mpst_Curve[0][0].tx = mpst_Curve[0][0].ty = 0;
    mpst_Curve[0][0].sel = 0;
	mpst_Curve[0][i - 1].tx = mpst_Curve[0][i - 1].ty = 1;
    mpst_Curve[0][i - 1].sel = 0;

	if(pf_Data == NULL) return;

	for(i = 1; i < ml_NbKey[0] - 1; i++)
	{
		mpst_Curve[0][i].tx = pf_Data[2];
		mpst_Curve[0][i].ty = mpst_Curve[0][i - 1].ty + (pf_Data[2] - pf_Data[0]) * pf_Data[1];
		pf_Data += 2;
	}

	for(i = 1; i < ml_NbKey[0] - 1; i++)
	{
		mpst_Curve[0][i].tx /= EVE_FrameToTime(mpst_Evt->uw_NumFrames);
		mpst_Curve[0][i].ty /= EVE_FrameToTime(mpst_Evt->uw_NumFrames);
		mpst_Curve[0][i].sel = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::SetEventData(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	short	w_Type;
	float	*pf_Data, *pf_Cur;
	int		i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	w_Type = EVE_w_Event_InterpolationKey_GetType(mpst_Evt);
	if(ml_NbKey[0] == 2)
	{
		w_Type &= ~EVE_InterKeyType_Time;
		EVE_Event_InterpolationKey_AllocData(mpst_Evt, w_Type, 0);
		return;
	}

	w_Type |= EVE_InterKeyType_Time;
	EVE_Event_InterpolationKey_AllocData(mpst_Evt, w_Type, ml_NbKey[0]- 1);
	pf_Cur = pf_Data = EVE_pf_Event_InterpolationKey_GetTime(mpst_Evt);

    for(i = 0; i < ml_NbKey[0] - 1; i++, pf_Cur += 2)
	{
		pf_Cur[0] = mpst_Curve[0][i].tx;
		pf_Cur[1] = mpst_Curve[0][i + 1].tx - pf_Cur[0];
		if(pf_Cur[1]) pf_Cur[1] = (mpst_Curve[0][i + 1].ty - mpst_Curve[0][i].ty) / pf_Cur[1];
		pf_Cur[0] *= EVE_FrameToTime(mpst_Evt->uw_NumFrames);
	}

	*(int *) pf_Data = ml_NbKey[0] - 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::GetPickable(CPoint pt)
{
	/*~~*/
	int i;
    EDIA_tdst_EditKey *k;
	/*~~*/

	mi_Pickable = -1;
    k = mpst_Curve[0] + 1;

	if((pt.x >= mo_CurveRect[0].left - 4) && (pt.x < mo_CurveRect[0].left))
	{
		for(i = 1; i < ml_NbKey[0] - 1; i++, k++)
		{
			mi_PickedCoord = 2;
			if((pt.y >= k->y - 3) && (pt.y <= k->y + 3))
			{
				mi_Pickable = i;
				return;
			}
		}
	}
	else if((pt.y > mo_CurveRect[0].bottom) && (pt.y <= mo_CurveRect[0].bottom + 4))
	{
		for(i = 1; i < ml_NbKey[0] - 1; i++, k++)
		{
			mi_PickedCoord = 1;
			if((pt.x >= k->x - 3) && (pt.x <= k->x + 3))
			{
				mi_Pickable = i;
				return;
			}
		}
	}
	else
	{
		for(i = 1; i < ml_NbKey[0] - 1; i++, k++)
		{
			mi_PickedCoord = 3;
			if((pt.x >= k->x - 3) && (pt.x <= k->x + 3) && (pt.y >= k->y - 3) && (pt.y <= k->y + 3))
			{
				mi_Pickable = i;
				return;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::AdjustKey(int _i_Dir)
{
	/*~~*/
	int i;
    EDIA_tdst_EditKey *k;
	/*~~*/

	if(_i_Dir >= 0)
	{
        k = mpst_Curve[0];
		for(i = 0; i < ml_NbKey[0]; i++, k++)
		{
			if(i + 1 < ml_NbKey[0])
			{
				if(k[1].tx < k->tx) k[1].tx = k->tx;
			}

			if(k->tx > 1) k->tx = 1;
			if(k->ty <= 0) k->ty = 0;
			if(k->ty >= 1) k->ty = 1;
		}
	}
	else
	{
        k = mpst_Curve[0] + (ml_NbKey[0] - 1);
		for(i = ml_NbKey[0] - 1; i >= 0; i--, k--)
		{
			if(i)
			{
				if(k[-1].tx > k->tx) k[-1].tx = k->tx;
			}

			if(k->tx < 0) k->tx = 0;
			if(k->ty <= 0) k->ty = 0;
			if(k->ty >= 1) k->ty = 1;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::SortKey( int curve )
{
	/*~~*/
	int i, j;
    EDIA_tdst_EditKey k;
	/*~~*/

    for (i = 1; i < ml_NbKey[curve]; i++)
    {
        if (mpst_Curve[curve][i].tx > 1)
            mpst_Curve[curve][i].tx = 1;
        else if (mpst_Curve[curve][i].tx < 0)
            mpst_Curve[curve][i].tx = 0;

        if (mpst_Curve[curve][i].ty > 1)
            mpst_Curve[curve][i].ty = 1;
        else if (mpst_Curve[curve][i].ty < 0)
            mpst_Curve[curve][i].ty = 0;

        if ( i == 1)
            continue;

        j = i-1;
        while (mpst_Curve[curve][i].tx < mpst_Curve[curve][j].tx)
            j--;
        if ( j != i - 1)
        {
            L_memcpy( &k, mpst_Curve[curve] + i, sizeof( EDIA_tdst_EditKey) );
            L_memmove( mpst_Curve[curve] + j + 2, mpst_Curve[curve] + j + 1, (i - j - 1) * sizeof( EDIA_tdst_EditKey) );
            L_memcpy( mpst_Curve[curve] + j + 1, &k, sizeof( EDIA_tdst_EditKey) );
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::ScaleKey( CSize &_PrevSize, int _hDir, int _vDir )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int                 i;
    EDIA_tdst_EditKey   *k;
    float               fx, fy, x, y;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (_hDir == 1)
        x = ((float) (mo_SelRect[0].left - mo_CurveRect[0].left)) / mf_Time2Width[0];
    else if ( _hDir == -1)
        x = ((float) (mo_SelRect[0].right - mo_CurveRect[0].left)) / mf_Time2Width[0];

    if (_vDir == 1)
        y = ((float) (mo_CurveRect[0].bottom - mo_SelRect[0].top)) / mf_Time2Height[0];
    else if ( _vDir == -1)
        y = ((float) (mo_CurveRect[0].bottom - mo_SelRect[0].bottom)) / mf_Time2Height[0];

    fx = (float) mo_SelRect[0].Width() / (float) _PrevSize.cx;
    fy = (float) mo_SelRect[0].Height() / (float) _PrevSize.cy;

    k = mpst_Curve[0] + 1;
    for (i = 1; i < ml_NbKey[0]; i++, k++)
    {
        if ( !k->sel ) continue;

        if (_hDir != 0) k->tx = (k->tx - x) * fx + x;
        if (_vDir != 0) k->ty = (k->ty - y) * fy + y;
    }
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::SelectPointInRect( int i )
{
    int j;
    EDIA_tdst_EditKey *k;

    k = mpst_Curve[i] + 1;
    for(j = 1; j < ml_NbKey[i] - 1; j++, k++)
    {
	    if( (k->x >= mo_SelRect[i].left) && (k->x <= mo_SelRect[i].right) && (k->y >= mo_SelRect[i].top) && (k->y <= mo_SelRect[i].bottom))
		    k->sel = 1;
    }
    InvalidateRect(mo_DrawRect[i]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::ListRefresh( void )
{
    CListBox    *LB;
    char        sz_Name[ BIG_C_MaxLenPath ];
    ULONG       ul_Index, ul_SaveIndex;
    int         i;

    LB = (CListBox *) GetDlgItem( IDC_LIST_FILE );

    i = LB->GetCurSel();
    if (i != -1)
        ul_SaveIndex = LB->GetItemData( i );
    else
        ul_SaveIndex = BIG_C_InvalidIndex;
    
    LB->ResetContent();

    sprintf( sz_Name, "%s/TimeInterpolationKey", EDI_Csz_Path_EditData );
    ul_Index = BIG_ul_SearchDir( sz_Name );

    ul_Index = BIG_FirstFile(ul_Index);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		if(BIG_b_IsFileExtension(ul_Index, ".tik"))
		{
            i = LB->AddString( BIG_NameFile(ul_Index) );
            LB->SetItemData( i, ul_Index );
		}
		ul_Index = BIG_NextFile(ul_Index);
	}

    if ( ul_SaveIndex != BIG_C_InvalidIndex )
    {
        for (i = 0; i < LB->GetCount(); i++)
        {
            if (LB->GetItemData(i) == ul_SaveIndex )
            {
                LB->SetCurSel( i );
                return;
            }
        }

        if ( ml_NbKey[1] )
        {
            ml_NbKey[1] = 0;
            L_free( mpst_Curve[1] );
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_TimeInter::SomethingSel( EDIA_tdst_EditKey *pst_Key, int _i_Count )
{
    while (_i_Count--)
    {
        if( pst_Key++->sel ) return TRUE;
    }
    return FALSE;
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_TimeInter::SelCount( EDIA_tdst_EditKey *pst_Key, int _i_Count )
{
    int n;

    n = 0;
    while (_i_Count--)
        if( pst_Key++->sel) n++;
    return n;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::SelNone( EDIA_tdst_EditKey *pst_Key, int _i_Count )
{
    while (_i_Count--)
        pst_Key++->sel = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::SelAll( EDIA_tdst_EditKey *pst_Key, int _i_Count )
{
    _i_Count -= 2;
    pst_Key++;
    while (_i_Count--)
        pst_Key++->sel = 1;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::SelDelete()
{
    int i, j;
    EDIA_tdst_EditKey *k;

    i= j = 0;
    k = mpst_Curve[0];
    while ( j < ml_NbKey[0] )
    {
        if (!k[j].sel)
        {
            if (i != j)
            {
                k[i].sel = k[j].sel;
                k[i].tx = k[j].tx;
                k[i].ty = k[j].ty;
            }
            i++;
        }
        j++;
    }
    ml_NbKey[0] = i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnHFlip()
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::OnVFlip()
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::Copy( )
{
    int end;

    if (ml_NbKey[2] == 0) return;

    end = ml_NbKey[0];
    AllocData( 0 ,ml_NbKey[0] + ml_NbKey[2] );

    L_memcpy( mpst_Curve[0] + (ml_NbKey[0] - 1), mpst_Curve[0] + (end - 1), sizeof( EDIA_tdst_EditKey) );
    L_memcpy( mpst_Curve[0] + (end - 1), mpst_Curve[2], ml_NbKey[2] * sizeof( EDIA_tdst_EditKey) );

    SortKey( 0 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::HistPush()
{
    EDIA_tdst_HistoryList *pst_New, *pst_Old;

    pst_Old = mpst_History->pst_Next;
    if (pst_Old) pst_Old->pst_Key = NULL;
    while (pst_Old)
    {
        pst_New = pst_Old->pst_Next;
        if ( pst_Old->pst_Key ) L_free( pst_Old->pst_Key );
        L_free( pst_Old );
        pst_Old = pst_New;
    }

    pst_New = (EDIA_tdst_HistoryList *) L_malloc( sizeof(EDIA_tdst_HistoryList) );
    pst_New->l_NbKey = ml_NbKey[0];
    pst_New->pst_Key = (EDIA_tdst_EditKey *) L_malloc( pst_New->l_NbKey * sizeof(EDIA_tdst_EditKey) );
    L_memcpy( pst_New->pst_Key, mpst_Curve[0], pst_New->l_NbKey * sizeof(EDIA_tdst_EditKey) );
    pst_New->i_SelRect = mi_SelRect[0];
    pst_New->o_SelRect = mo_SelRect[0];
    pst_New->pst_Prev = mpst_History;
    mpst_History->pst_Next = pst_New;
    pst_New->pst_Next = NULL;
    mpst_History = pst_New;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_TimeInter::HistPop()
{
    if ( !mpst_History->pst_Prev ) return;

    if (mpst_History->pst_Next == NULL)
    {
        HistPush();
        mpst_History = mpst_History->pst_Prev;
    }

    ml_NbKey[0] = mpst_History->l_NbKey;
    mpst_Curve[0] = mpst_History->pst_Key;
    mi_SelRect[0] = mpst_History->i_SelRect;
    mo_SelRect[0] = mpst_History->o_SelRect;

    mpst_History = mpst_History->pst_Prev;

    InvalidateRect(mo_DrawRect[0]);
}



#endif /* ACTIVE_EDITORS */
