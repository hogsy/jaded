/*$T DIAsndinsert_dlg.cpp GC 1.138 03/31/04 16:40:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAsndinsert_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"

#include "BASe/CLIbrary/CLIstr.h"

#include "EDIpaths.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDImainframe.h"
#include "EDIstrings.h"

#include "Res/Res.h"

#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"

#include "LINKs/LINKmsg.h"
#include "LINks/LINKtoed.h"

#include "Sound/Sources/SND.h"
#include "Sound/Sources/SNDinsert.h"

#include "BASE/MEMory/MEM.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_SndInsert, EDIA_cl_BaseDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SAVE2, OnButtonSaveAs)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_GAO, OnButtonGao)
	ON_BN_CLICKED(IDC_CK_CYCLE, OnSetModif)
	ON_BN_CLICKED(IDC_CK_ONPLAY, OnSetModif)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE_Y, OnChangeTypeY)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE_X, OnChangeTypeX)
	ON_EN_CHANGE(IDC_EDIT_MIN_X, OnChangeMinMax)
	ON_EN_CHANGE(IDC_EDIT_MAX_X, OnChangeMinMax)
	ON_EN_CHANGE(IDC_EDIT_MIN_Y, OnChangeMinMax)
	ON_EN_CHANGE(IDC_EDIT_MAX_Y, OnChangeMinMax)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndInsert::EDIA_cl_SndInsert(SND_tdst_Insert *pInsert) :
	EDIA_cl_BaseDialog(ESON_IDD_INSERT)
{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mb_WndInitDone = FALSE;
	mi_Pickable = -1;
	mi_Picked = -1;
	mb_TraceRect = FALSE;
	mb_PickedRect = FALSE;
	o_InsertPoint.x = -1;
	mb_SelRect = FALSE;
	me_PickedCoord = en_SelNone;

	ml_NbKey[M_CurrentCurv] = 0;
	ml_NbKey[M_ClipboardCurv] = 0;

	mpst_HistoryRoot = mpst_History = (EDIA_tdst_HistoryInsertList *) L_malloc(sizeof(EDIA_tdst_HistoryInsertList));
	L_memset(mpst_HistoryRoot, 0, sizeof(EDIA_tdst_HistoryInsertList));

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_TypeX = 0;
	mi_TypeY = 0;
	mul_GaoKey = BIG_C_InvalidKey;
	mf_MinX = 0.0f;
	mf_MaxX = 100.0f;
	mf_MinY = 0.0f;
	mf_MaxY = 100.0f;

	mp_Insert = pInsert;
	mb_IsModified = FALSE;
	mb_IsLoadedByMyself = FALSE;

	mb_ForceExtentionFade = FALSE;

	mb_Cycling = FALSE;
	mb_ActiveOnPlay = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndInsert::~EDIA_cl_SndInsert(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	EDIA_tdst_HistoryInsertList *pst_Old;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((mi_Picked != -1) || mb_TraceRect || mb_PickedRect)
	{
		ReleaseCapture();
		ClipCursor(&mst_OldClipCursor);
	}

	for(i = 0; i < M_NbCurv; i++)
	{
		if(ml_NbKey[i]) L_free(mpst_Curve[i]);
	}

	if(mpst_History)
	{
		if(mpst_History->pst_Next) mpst_History->pst_Next->pst_Key = NULL;
		while(mpst_History->pst_Next) mpst_History = mpst_History->pst_Next;

		while(mpst_History)
		{
			pst_Old = mpst_History;
			mpst_History = pst_Old->pst_Prev;
			if(pst_Old->pst_Key) L_free(pst_Old->pst_Key);
			L_free(pst_Old);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SndInsert::OnInitDialog(void)
{
	GetDlgItem(IDC_CURVE_INSERT)->GetWindowRect(&mo_DrawRect);
	GetDlgItem(IDC_CURVE_INSERT)->ShowWindow(SW_HIDE);
	ScreenToClient(&mo_DrawRect);

	mo_CurveRect = mo_DrawRect;
	mo_CurveRect.DeflateRect(16, 16);

	mo_XRect = mo_CurveRect;
	mo_XRect.top = mo_XRect.bottom;
	mo_XRect.bottom = mo_XRect.top + 4;

	mo_YRect = mo_CurveRect;
	mo_YRect.right = mo_YRect.left;
	mo_YRect.left = mo_YRect.right - 4;

	mf_Time2Width = (float) mo_CurveRect.Width();
	mf_Time2Height = (float) mo_CurveRect.Height();

	GetCursorPos(&mo_MousePos);

	LoadData();

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL	ret = EDIA_cl_BaseDialog::OnInitDialog();
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SetIsModified(FALSE);

	mb_WndInitDone = TRUE;
	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SndInsert::PreTranslateMessage(MSG *pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_SPACE)
		{
			return 1;
		}
		else if(pMsg->wParam == VK_RETURN)
		{
			if(pMsg->hwnd == GetDlgItem(IDC_EDIT_MIN_X)->GetSafeHwnd())
			{
				GetDlgItem(IDC_BUTTON_SAVE)->SetFocus();
				return 1;
			}
			else if(pMsg->hwnd == GetDlgItem(IDC_EDIT_MAX_X)->GetSafeHwnd())
			{
				GetDlgItem(IDC_BUTTON_SAVE)->SetFocus();
				return 1;
			}
			else if(pMsg->hwnd == GetDlgItem(IDC_EDIT_MIN_Y)->GetSafeHwnd())
			{
				GetDlgItem(IDC_BUTTON_SAVE)->SetFocus();
				return 1;
			}
			else if(pMsg->hwnd == GetDlgItem(IDC_EDIT_MAX_Y)->GetSafeHwnd())
			{
				GetDlgItem(IDC_BUTTON_SAVE)->SetFocus();
				return 1;
			}
			else if(pMsg->hwnd == GetDlgItem(IDC_COMBO_TYPE_X)->GetSafeHwnd())
			{
				GetDlgItem(IDC_BUTTON_SAVE)->SetFocus();
				return 1;
			}
			else if(pMsg->hwnd == GetDlgItem(IDC_COMBO_TYPE_Y)->GetSafeHwnd())
			{
				GetDlgItem(IDC_BUTTON_SAVE)->SetFocus();
				return 1;
			}
		}

		OnKeyDown(pMsg->wParam);
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_TYPE_X, mi_TypeX);
	DDX_CBIndex(pDX, IDC_COMBO_TYPE_Y, mi_TypeY);

	DDX_Text(pDX, IDC_EDIT_MIN_X, mf_MinX);
	DDV_MinMaxFloat(pDX, mf_MinX, 0.0f, 360000.0f);
	DDX_Text(pDX, IDC_EDIT_MAX_X, mf_MaxX);
	DDV_MinMaxFloat(pDX, mf_MaxX, 0.0f, 360000.0f);

	DDX_Text(pDX, IDC_EDIT_MIN_Y, mf_MinY);
	DDV_MinMaxFloat(pDX, mf_MinY, 0.0f, 10000.0f);
	DDX_Text(pDX, IDC_EDIT_MAX_Y, mf_MaxY);
	DDV_MinMaxFloat(pDX, mf_MaxY, 0.0f, 10000.0f);

	DDX_Check(pDX, IDC_CK_CYCLE, mb_Cycling);
	DDX_Check(pDX, IDC_CK_ONPLAY, mb_ActiveOnPlay);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnSetModif(void)
{
	SetIsModified(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnChangeTypeY(void)
{
	UpdateData();

	if(mp_Insert->ul_Flags & SND_Cte_InsFade)
	{
		if(mi_TypeY > 1)
		{
			mi_TypeY = 1;
			UpdateData(FALSE);
		}
	}

	SetIsModified(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnChangeTypeX(void)
{
	UpdateData();

	if((mi_TypeX>=0) && (mi_TypeX<=3))
    {
        EnableXaxisAndGao(TRUE, TRUE);
    }
    else if(mi_TypeX == 4)
    {
		EnableXaxisAndGao(TRUE, FALSE);
    }
    else if((mi_TypeX>=5) && (mi_TypeX<=36))
    {
		EnableXaxisAndGao(FALSE, FALSE);
    }

	SetIsModified(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::EnableXaxisAndGao(BOOL b_Enable, BOOL b_Gao)
{
	if(b_Enable)
	{
		GetDlgItem(IDC_EDIT_MIN_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_MAX_X)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_MIN_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_MAX_X)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_MIN_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_MAX_X)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_MIN_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_MAX_X)->ShowWindow(SW_SHOW);
		mf_MinX = 0.0f;
		mf_MaxX = 100.0f;
	}

	if(b_Gao)
	{
		GetDlgItem(IDC_BUTTON_GAO)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_GAO)->ShowWindow(SW_HIDE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect						o_Rect;
	CDC							*pDC;
	CPen						o_RedPen, o_BluePen, o_YellowPen;
	CPen						o_BlackPen, o_BlackDotPen;
	void						*p_OldPen;
	int							i;
	EDIA_tdst_EdSndInsertKey	*C;
	CBrush						*p_OldBrush;
    BOOL                        b_PosText;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnPaint();

	/*$1- pens ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDC = GetDC();
	o_RedPen.CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
	o_BluePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	o_YellowPen.CreatePen(PS_SOLID, 1, RGB(0xFF, 0xFF, 0));
	o_BlackPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	o_BlackDotPen.CreatePen(PS_DOT, 1, RGB(0, 0, 0));
	p_OldPen = pDC->SelectObject(o_BlackPen);
	p_OldBrush = pDC->GetCurrentBrush();

	/* draw curves */

	/*$1- axis ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDC->SelectObject(o_BlackPen);
	pDC->MoveTo(mo_CurveRect.left, mo_CurveRect.top);
	pDC->LineTo(mo_CurveRect.left, mo_CurveRect.bottom);
	pDC->LineTo(mo_CurveRect.right, mo_CurveRect.bottom);

	pDC->SelectObject(o_BlackDotPen);
	pDC->LineTo(mo_CurveRect.right, mo_CurveRect.top);
	pDC->LineTo(mo_CurveRect.left, mo_CurveRect.top);

	/*$1- draw selection rect ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    b_PosText = FALSE;
	if(mb_SelRect)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		CRect	o_Focus = mo_SelRect;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		o_Focus.NormalizeRect();
		pDC->DrawFocusRect(&o_Focus);
	}
    
    if(SelCount(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]) == 1)
    {
        UpdateData();
        b_PosText = TRUE;
    }
    

	/*$1- draw curve ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDC->SelectObject(o_BluePen);
	pDC->MoveTo(mo_CurveRect.left, mo_CurveRect.bottom);

	C = mpst_Curve[M_CurrentCurv];

	C[0].x = mo_CurveRect.left + (int) (C[0].tx * mf_Time2Width);
	C[0].y = mo_CurveRect.bottom - (int) (C[0].ty * mf_Time2Height);
	pDC->MoveTo(C[0].x, C[0].y);

	for(i = 1; i < ml_NbKey[M_CurrentCurv]; i++)
	{
		C[i].x = mo_CurveRect.left + (int) (C[i].tx * mf_Time2Width);
		C[i].y = mo_CurveRect.bottom - (int) (C[i].ty * mf_Time2Height);
		pDC->LineTo(C[i].x, C[i].y);

        if(b_PosText && C[i].sel)
        {
            CString oPos;
            CRect oRect;

            oPos.Format("[%.1f, %.1f]", MATH_f_FloatBlend(mf_MinX, mf_MaxX, C[i].tx),MATH_f_FloatBlend(mf_MinY, mf_MaxY, C[i].ty));
            pDC->SetTextColor(RGB(0xFF, 0, 0));
            pDC->SetBkColor(GetSysColor(COLOR_3DLIGHT));

            oRect.left = C[i].x+15;
            oRect.right = oRect.left+100;
            oRect.top = C[i].y;
            oRect.bottom = oRect.top+50;

            if(oRect.bottom > mo_CurveRect.bottom)
            {
                oRect.bottom -= 20;
                oRect.top -= 20;
            }

            if(oRect.right > mo_CurveRect.right)
            {
                oRect.right =  C[i].x - 5;
                oRect.left = oRect.right - 100;

                pDC->DrawText(oPos, &oRect, DT_RIGHT | DT_TOP);
            }
            else
            {
                pDC->DrawText(oPos, &oRect, DT_LEFT | DT_TOP);
            }
        }
	}

	/*$1- draw points ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < ml_NbKey[M_CurrentCurv]; i++)
	{
		pDC->SelectObject(C[i].sel ? o_RedPen : o_BluePen);

		pDC->MoveTo(C[i].x - 3, C[i].y - 3);
		pDC->LineTo(C[i].x + 3, C[i].y - 3);
		pDC->LineTo(C[i].x + 3, C[i].y + 3);
		pDC->LineTo(C[i].x - 3, C[i].y + 3);
		pDC->LineTo(C[i].x - 3, C[i].y - 3);

		pDC->MoveTo(C[i].x, mo_CurveRect.bottom + 1);
		pDC->LineTo(C[i].x, mo_CurveRect.bottom + 4);

		pDC->MoveTo(mo_CurveRect.left - 1, C[i].y);
		pDC->LineTo(mo_CurveRect.left - 4, C[i].y);
	}

	/*$1- restore pen ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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
void EDIA_cl_SndInsert::OnMouseMove(UINT ui_Flags, CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	CPoint						o_Pos, o_Delta;
	EDIA_tdst_EdSndInsertKey	*pst_Key;
	CSize						o_OldSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mo_CurveRect.PtInRect(pt))
		o_InsertPoint = pt;
	else
		o_InsertPoint.x = -1;

	if((mi_Picked != -1) || mb_PickedRect)
	{
		GetCursorPos(&o_Pos);
		o_Delta = o_Pos - mo_MousePos;
		mo_MousePos = o_Pos;

		if(me_PickedCoord & (en_SelXaxis | en_SelYaxis))
		{
			pst_Key = mpst_Curve[M_CurrentCurv];

			for(i = 0; i < ml_NbKey[M_CurrentCurv]; i++, pst_Key++)
			{
				if(!pst_Key->sel) continue;
				if(me_PickedCoord & en_SelYaxis) pst_Key->ty -= ((float) (o_Delta.y)) / mf_Time2Height;
				if(i == 0) continue;
				if(i == ml_NbKey[M_CurrentCurv] - 1) continue;
				if(me_PickedCoord & en_SelXaxis) pst_Key->tx += ((float) (o_Delta.x)) / mf_Time2Width;
			}

			if(ui_Flags & MK_LBUTTON) SetIsModified(TRUE);

			if(mb_SelRect)
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
				mo_SelRect.OffsetRect(o_Delta);
				mo_SelRect &= mo_CurveRect;
			}
		}
		else
		{
			o_OldSize = mo_SelRect.Size();
			switch(me_PickedCoord)
			{
			case en_SelTopLeftCorner:
				mo_SelRect.left += o_Delta.x;
				mo_SelRect.top += o_Delta.y;
				ScaleKey(o_OldSize, -1, -1);
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));
				break;

			case en_SelTopEdge:
				mo_SelRect.top += o_Delta.y;
				ScaleKey(o_OldSize, 0, -1);
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
				break;

			case en_SelTopRightCorner:
				mo_SelRect.right += o_Delta.x;
				mo_SelRect.top += o_Delta.y;
				ScaleKey(o_OldSize, 1, -1);
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW));
				break;

			case en_SelRightEdge:
				mo_SelRect.right += o_Delta.x;
				ScaleKey(o_OldSize, 1, 0);
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				break;

			case en_SelBottomRightCorner:
				mo_SelRect.right += o_Delta.x;
				mo_SelRect.bottom += o_Delta.y;
				ScaleKey(o_OldSize, 1, 1);
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));
				break;

			case en_SelBottomEdge:
				mo_SelRect.bottom += o_Delta.y;
				ScaleKey(o_OldSize, 0, 1);
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
				break;

			case en_SelBottomLeftCorner:
				mo_SelRect.left += o_Delta.x;
				mo_SelRect.bottom += o_Delta.y;
				ScaleKey(o_OldSize, -1, 1);
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW));
				break;

			case en_SelLeftEdge:
				mo_SelRect.left += o_Delta.x;
				ScaleKey(o_OldSize, -1, 0);
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				break;
			}
		}

		if(ui_Flags & MK_SHIFT)
			AdjustKey((o_Delta.x > 0) ? 1 : -1);
		else
			SortKey(M_CurrentCurv);

		InvalidateRect(mo_DrawRect);
	}
	else
	{
		if(!mb_TraceRect)
		{
			GetPickable(pt);
			if(mi_Pickable == -1)
			{
				if(mb_SelRect && mo_SelRect.PtInRect(pt))
				{
					mo_SelRect.DeflateRect(4, 4);
					if(!mo_SelRect.PtInRect(pt))
					{
						if(pt.y < mo_SelRect.top + 4)
						{
							if(pt.x < mo_SelRect.left + 4) ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));	/* top-left
																														 * corner
																														 * */
							else if(pt.x > mo_SelRect.right - 4)
								::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW)); /* top-right corner */
							else
								::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));	/* top edge */
						}
						else if(pt.y > mo_SelRect.bottom - 4)
						{
							if(pt.x < mo_SelRect.left + 4) ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW));	/* bottom-left
																														 * corner
																														 * */
							else if(pt.x > mo_SelRect.right - 4)
								::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE)); /* bottom-right corner */
							else
								::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));	/* bottom edge */
						}
						else
						{
							if(pt.x < mo_SelRect.left + 4) ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));	/* left
																														 * edge
																														 * */
							else
								::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));	/* right edge */
						}
					}
					else
						::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
					mo_SelRect.InflateRect(4, 4);
				}
				else
					::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			}
			else
			{
				if(me_PickedCoord == en_SelXaxis)
					::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				else if(me_PickedCoord == en_SelYaxis)
					::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
				else
					::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
			}
		}
		else
		{
			if(me_PickedCoord & en_SelXaxis) mo_SelRect.right = pt.x;
			if(me_PickedCoord & (en_SelXaxis | en_SelYaxis)) mo_SelRect.bottom = pt.y;
			InvalidateRect(mo_DrawRect);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnLButtonDown(UINT ui, CPoint pt)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(mi_Pickable == -1)
	{
		mb_TraceRect = FALSE;
		{
			if((mb_SelRect) && (mo_SelRect.PtInRect(pt)))
			{
				me_PickedCoord = en_SelCurvePoints;
				mb_PickedRect = TRUE;

				mo_SelRect.DeflateRect(4, 4);
				o_Rect = mo_SelRect;
				if(!mo_SelRect.PtInRect(pt))
				{
					if(pt.y < mo_SelRect.top + 4)
					{
						if(pt.x < mo_SelRect.left + 4)
						{
							me_PickedCoord = en_SelTopLeftCorner;		/* top-left corner */
							o_Rect.left = mo_CurveRect.left;
							o_Rect.top = mo_CurveRect.top;
						}
						else if(pt.x > mo_SelRect.right - 4)
						{
							me_PickedCoord = en_SelTopRightCorner;		/* top-right corner */
							o_Rect.right = mo_CurveRect.right;
							o_Rect.top = mo_CurveRect.top;
						}
						else
						{
							me_PickedCoord = en_SelTopEdge;				/* top edge */
							o_Rect.top = mo_CurveRect.top;
						}
					}
					else if(pt.y > mo_SelRect.bottom - 4)
					{
						if(pt.x < mo_SelRect.left + 4)
						{
							me_PickedCoord = en_SelBottomLeftCorner;	/* bottom-left corner */
							o_Rect.left = mo_CurveRect.left;
							o_Rect.bottom = mo_CurveRect.bottom;
						}
						else if(pt.x > mo_SelRect.right - 4)
						{
							me_PickedCoord = en_SelBottomRightCorner;	/* bottom-right corner */
							o_Rect.right = mo_CurveRect.right;
							o_Rect.bottom = mo_CurveRect.bottom;
						}
						else
						{
							me_PickedCoord = en_SelBottomEdge;			/* bottom edge */
							o_Rect.bottom = mo_CurveRect.bottom;
						}
					}
					else
					{
						if(pt.x < mo_SelRect.left + 4)
						{
							me_PickedCoord = en_SelLeftEdge;			/* left edge */
							o_Rect.left = mo_CurveRect.left;
						}
						else
						{
							me_PickedCoord = en_SelRightEdge;			/* right edge */
							o_Rect.right = mo_CurveRect.right;
						}
					}
				}

				mo_SelRect.InflateRect(4, 4);
			}
			else if(mo_XRect.PtInRect(pt))
			{
				mb_TraceRect = TRUE;
				me_PickedCoord = en_SelXaxis;		/* X axis sel */
				mb_SelRect = TRUE;

				mo_SelRect.left = mo_SelRect.right = pt.x;
				mo_SelRect.top = mo_CurveRect.top;
				mo_SelRect.bottom = mo_CurveRect.bottom;
			}
			else if(mo_YRect.PtInRect(pt))
			{
				mb_TraceRect = TRUE;
				me_PickedCoord = en_SelYaxis;		/* Y axis sel */
				mb_SelRect = TRUE;

				mo_SelRect.top = mo_SelRect.bottom = pt.y;
				mo_SelRect.left = mo_CurveRect.left;
				mo_SelRect.right = mo_CurveRect.right;
			}
			else if(mo_CurveRect.PtInRect(pt))
			{
				mb_TraceRect = TRUE;
				me_PickedCoord = en_SelCurvePoints; /* curve sel */
				mb_SelRect = TRUE;

				mo_SelRect.left = mo_SelRect.right = pt.x;
				mo_SelRect.top = mo_SelRect.bottom = pt.y;
			}
		}

		if(!mb_TraceRect && !mb_PickedRect) return;

		HistPush();

		if(mb_TraceRect)
		{
			if(!(ui & MK_CONTROL)) SelNone(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]);
		}
	}
	else
	{
		HistPush();
		if(!(ui & MK_CONTROL))
		{
			if(!mpst_Curve[M_CurrentCurv][mi_Pickable].sel)
			{
				SelNone(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]);
				mpst_Curve[M_CurrentCurv][mi_Pickable].sel = 1;
			}
		}
		else
		{
			mpst_Curve[M_CurrentCurv][mi_Pickable].sel ^= 1;
		}

		mi_Picked = mi_Pickable;
	}

	SetCapture();
	GetCursorPos(&mo_MousePos);

	if(!(me_PickedCoord > en_SelCurvePoints))
	{
		o_Rect = mo_CurveRect;
		if((me_PickedCoord & en_SelXaxis) == 0)
		{
			o_Rect.right = o_Rect.left;
			o_Rect.left -= 4;
		}
		else if((me_PickedCoord & en_SelYaxis) == 0)
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
void EDIA_cl_SndInsert::OnLButtonUp(UINT ui, CPoint pt)
{
	/*~~~~*/
	int tmp;
	/*~~~~*/

	if((mi_Picked == -1) && !mb_TraceRect && !mb_PickedRect) return;

	mi_Picked = -1;
	mb_PickedRect = FALSE;

	if(mb_TraceRect)
	{
		if(mo_SelRect.left > mo_SelRect.right)
		{
			tmp = mo_SelRect.left;
			mo_SelRect.left = mo_SelRect.right;
			mo_SelRect.right = tmp;
		}

		if(mo_SelRect.top > mo_SelRect.bottom)
		{
			tmp = mo_SelRect.top;
			mo_SelRect.top = mo_SelRect.bottom;
			mo_SelRect.bottom = tmp;
		}

		if((mo_SelRect.Width() < 4) || (mo_SelRect.Height() < 4))
			mb_SelRect = FALSE;
		else
			SelectPointInRect();

		mb_TraceRect = FALSE;
	}

	ReleaseCapture();
	ClipCursor(&mst_OldClipCursor);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnRButtonDown(UINT ui, CPoint pt)
{
	/*~~~~~~~~~~~*/
	CMenu	o_Menu;
	int		i_Res;
	/*~~~~~~~~~~~*/

	o_Menu.CreatePopupMenu();
	o_Menu.AppendMenu(MF_BYCOMMAND, 3, "&Select all\tCtrl+A");

	o_Menu.AppendMenu(MF_SEPARATOR, -1, "");
	if(SomethingSel(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]))
		o_Menu.AppendMenu(MF_BYCOMMAND, 6, "&Delete\tDel");
	else
		o_Menu.AppendMenu(MF_BYCOMMAND | MF_DISABLED | MF_GRAYED, 6, "&Delete\tDel");

	o_Menu.AppendMenu(MF_BYCOMMAND, 1, "&Copy\tCtrl+C");
	o_Menu.AppendMenu
		(
			ml_NbKey[M_ClipboardCurv] ? MF_BYCOMMAND : MF_BYCOMMAND | MF_DISABLED | MF_GRAYED,
			2,
			"&Paste\tCtrl+V"
		);

	o_Menu.AppendMenu(MF_SEPARATOR, -1, "");
	o_Menu.AppendMenu(MF_BYCOMMAND, 4, "&Undo\tCtrl+Z");
	if(mpst_History && mpst_History->pst_Next && mpst_History->pst_Next->pst_Next)
		o_Menu.AppendMenu(MF_BYCOMMAND, 5, "&Redo\tCtrl+Y");
	else
		o_Menu.AppendMenu(MF_BYCOMMAND | MF_DISABLED | MF_GRAYED, 5, "&Redo\tCtrl+Y");

	GetCursorPos(&pt);
	i_Res = o_Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this);
	switch(i_Res)
	{
	case 1:
		Copy();
		break;

	case 2:
		Paste();
		break;

	case 3:
		mb_SelRect = FALSE;
		SelAll(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]);
		InvalidateRect(mo_DrawRect);
		break;

	case 4:
		HistPop();
		break;

	case 5:
		mpst_History = mpst_History->pst_Next;
		ml_NbKey[M_CurrentCurv] = mpst_History->pst_Next->l_NbKey;
		mpst_Curve[M_CurrentCurv] = mpst_History->pst_Next->pst_Key;
		mb_SelRect = mpst_History->pst_Next->b_SelRect;
		mo_SelRect = mpst_History->pst_Next->o_SelRect;
		InvalidateRect(mo_DrawRect);
		break;

	case 6:
		HistPush();
		SetIsModified(TRUE);

		SelDelete();
		mb_SelRect = FALSE;
		InvalidateRect(mo_DrawRect);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnKeyDown(UINT nChar)
{
	/*~~~~~~*/
	int		i;
	float	x;
	/*~~~~~~*/

	if(mi_Picked != -1) return;

	if(nChar == VK_DELETE)
	{
		if(SomethingSel(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]))
		{
			HistPush();
			SetIsModified(TRUE);

			SelDelete();
			mb_SelRect = FALSE;
			InvalidateRect(mo_DrawRect);
		}
	}
	else if(nChar == VK_INSERT)
	{
		if(o_InsertPoint.x == -1) return;
		x = ((float) (o_InsertPoint.x - mo_CurveRect.left)) / mf_Time2Width;

		HistPush();
		SetIsModified(TRUE);

		for(i = 1; i < ml_NbKey[M_CurrentCurv]; i++)
			if(x < mpst_Curve[M_CurrentCurv][i].tx) break;

		if(i == ml_NbKey[M_CurrentCurv])
		{
			i = ml_NbKey[M_CurrentCurv] - 1;
			x = 1.0f;
		}

		/* realloc array */
		mpst_Curve[M_CurrentCurv] = (EDIA_tdst_EdSndInsertKey *) L_realloc
			(
				mpst_Curve[M_CurrentCurv],
				(ml_NbKey[M_CurrentCurv] + 1) * sizeof(EDIA_tdst_EdSndInsertKey)
			);

		/* decal other pos */
		L_memmove
		(
			mpst_Curve[M_CurrentCurv] + i + 1,
			mpst_Curve[M_CurrentCurv] + i,
			(ml_NbKey[M_CurrentCurv] - i) * sizeof(EDIA_tdst_EdSndInsertKey)
		);
		ml_NbKey[M_CurrentCurv]++;

		/* insert new pos */
		mpst_Curve[M_CurrentCurv][i].tx = x;
		mpst_Curve[M_CurrentCurv][i].ty = ((float) (mo_CurveRect.bottom - o_InsertPoint.y)) / mf_Time2Height;;
		InvalidateRect(mo_DrawRect);
	}
	else if(nChar == 'A')
	{
		if(GetAsyncKeyState(VK_CONTROL) < 0)
		{
			mb_SelRect = FALSE;
			SelAll(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]);
			InvalidateRect(mo_DrawRect);
		}
	}
	else if(nChar == 'C')
	{
		if(GetAsyncKeyState(VK_CONTROL) < 0) Copy();
	}
	else if(nChar == 'V')
	{
		if(GetAsyncKeyState(VK_CONTROL) < 0) Paste();
	}
	else if(nChar == 'Z')
	{
		if(GetAsyncKeyState(VK_CONTROL) < 0) HistPop();
	}
	else if(nChar == 'Y')
	{
		if(GetAsyncKeyState(VK_CONTROL) < 0)
		{
			if(mpst_History && mpst_History->pst_Next && mpst_History->pst_Next->pst_Next)
			{
				mpst_History = mpst_History->pst_Next;
				ml_NbKey[M_CurrentCurv] = mpst_History->pst_Next->l_NbKey;
				mpst_Curve[M_CurrentCurv] = mpst_History->pst_Next->pst_Key;
				mb_SelRect = mpst_History->pst_Next->b_SelRect;
				mo_SelRect = mpst_History->pst_Next->o_SelRect;

				InvalidateRect(mo_DrawRect);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnButtonSave(void)
{
	SaveData();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnButtonSaveAs(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	tx0, txN;
	float	ty0, tyN;
	ULONG	ulKey;
	int		i_SavTypeX, i_SavTypeY;
	ULONG	ul_SavGaoKey;
	float	f_SavMinX, f_SavMaxX;
	float	f_SavMinY, f_SavMaxY;
	BOOL	b_SavCycling, b_SavActiveOnPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* save settings */
    UpdateData();
	i_SavTypeX = mi_TypeX;
	i_SavTypeY = mi_TypeY;
	ul_SavGaoKey = mul_GaoKey;
	f_SavMinX = mf_MinX;
	f_SavMaxX = mf_MaxX;
	f_SavMinY = mf_MinY;
	f_SavMaxY = mf_MaxY;
	b_SavCycling = mb_Cycling;
	b_SavActiveOnPlay = mb_ActiveOnPlay;

	mb_SelRect = FALSE;
	SelNone(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]);

	tx0 = mpst_Curve[M_CurrentCurv][0].tx;
	ty0 = mpst_Curve[M_CurrentCurv][0].ty;
	txN = mpst_Curve[M_CurrentCurv][ml_NbKey[M_CurrentCurv] - 1].tx;
	tyN = mpst_Curve[M_CurrentCurv][ml_NbKey[M_CurrentCurv] - 1].ty;

	ulKey = BIG_ul_SearchKeyToFat(mp_Insert->ul_FileKey);
	mb_ForceExtentionFade = BIG_b_IsFileExtension(ulKey, EDI_Csz_ExtSoundFade);

	ulKey = mp_Insert->ul_FileKey;
	Copy();
	SetIsModified(FALSE);
	OnButtonNew();
	mb_ForceExtentionFade = FALSE;

	/* restore settings */
	mi_TypeX = i_SavTypeX;
	mi_TypeY = i_SavTypeY;
	mul_GaoKey = ul_SavGaoKey;
	mf_MinX = f_SavMinX;
	mf_MaxX = f_SavMaxX;
	mf_MinY = f_SavMinY;
	mf_MaxY = f_SavMaxY;
	mb_Cycling = b_SavCycling;
	mb_ActiveOnPlay = b_SavActiveOnPlay;

	UpdateData(FALSE);

	if(ulKey == mp_Insert->ul_FileKey) return;	/* file is the same -> cancel operation */

	Paste();
	mb_SelRect = FALSE;
	SelNone(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]);

	if((tx0 == mpst_Curve[M_CurrentCurv][0].tx) && (ty0 == mpst_Curve[M_CurrentCurv][0].ty))
		mpst_Curve[M_CurrentCurv][1].sel = 1;
	else
		mpst_Curve[M_CurrentCurv][0].sel = 1;

	if
	(
		(txN == mpst_Curve[M_CurrentCurv][ml_NbKey[M_CurrentCurv] - 1].tx)
	&&	(tyN == mpst_Curve[M_CurrentCurv][ml_NbKey[M_CurrentCurv] - 1].ty)
	) mpst_Curve[M_CurrentCurv][ml_NbKey[M_CurrentCurv] - 2].sel = 1;
	else
		mpst_Curve[M_CurrentCurv][ml_NbKey[M_CurrentCurv] - 1].sel = 1;

	SelDeleteSpecial();
	SortKey(M_CurrentCurv);

	OnButtonSave();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnButtonNew(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog("Enter a new insert file name");
	CString				o_Temp;
	int					ulFatIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mb_IsModified)
	{
		switch
		(
			M_MF()->MessageBox
				(
					"The file has been modified. Continue without saving ?",
					EDI_STR_Csz_TitleConfirm,
					MB_ICONWARNING | MB_YESNO
				)
		)
		{
		case IDYES: UpdateData(FALSE); break;
		default:	return;
		}
	}

	if(mb_IsLoadedByMyself) SND_InsertUnload(SND_M_GetInsertFromPtr(mp_Insert));
	mb_IsLoadedByMyself = FALSE;
	SetIsModified(FALSE);

	/*$1- get new file name ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_Dialog.DoModal() != IDOK) return;
	o_Temp = o_Dialog.mo_Name;

	if(mb_ForceExtentionFade)
		o_Temp += CString(EDI_Csz_ExtSoundFade);
	else
		o_Temp += CString(EDI_Csz_ExtSoundInsert);

	/*$1- get file index/key ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulFatIndex = BIG_ul_SearchFileExt(EDI_Csz_Path_Audio, (char *) (LPCSTR) o_Temp);
	if(ulFatIndex != BIG_C_InvalidIndex)
	{
		ERR_X_ForceError("Insert name already exists : ", (char *) (LPCSTR) o_Temp);
	}
	else
	{
		BIG_ul_CreateDir(EDI_Csz_Path_Audio);
		ulFatIndex = BIG_ul_CreateFile(EDI_Csz_Path_Audio, (char *) (LPCSTR) o_Temp);

		if(BIG_ul_GetLengthFile(BIG_PosFile(ulFatIndex)) == 0)
		{
			mp_Insert = SND_p_InsertCreate(BIG_FileKey(ulFatIndex), en_InsYaxisVol, en_InsXaxisTime, 2);
			SND_InsertSave(mp_Insert, BIG_FileKey(ulFatIndex));
			SND_InsertUnload(SND_M_GetInsertFromPtr(mp_Insert));
		}

		/*$1- get the pointer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		LOA_MakeFileRef
		(
			BIG_FileKey(ulFatIndex),
			(ULONG *) &mp_Insert,
			SND_ul_CallbackInsertLoad,
			LOA_C_MustExists | LOA_C_HasUserCounter
		);
		LOA_Resolve();
		LoadData();
		mb_IsLoadedByMyself = TRUE;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	InvalidateRect(mo_DrawRect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnButtonClose(void)
{
	if(mb_IsModified)
	{
		switch
		(
			M_MF()->MessageBox
				(
					"The file has been modified. Continue without saving ?",
					EDI_STR_Csz_TitleConfirm,
					MB_ICONWARNING | MB_YESNO
				)
		)
		{
		case IDYES:
			if(mb_IsLoadedByMyself) SND_InsertUnload(SND_M_GetInsertFromPtr(mp_Insert));
			EDIA_cl_BaseDialog::OnCancel();
			return;

		default:
			return;
		}
	}

	if(mb_IsLoadedByMyself) SND_InsertUnload(SND_M_GetInsertFromPtr(mp_Insert));
	EDIA_cl_BaseDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnButtonOpen(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString				o_Temp;
	EDIA_cl_FileDialog	o_File("Choose File", 0, 0, 1, NULL, "*" EDI_Csz_ExtSoundInsert ",*" EDI_Csz_ExtSoundFade);
	int					ulFatIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_File.mb_UseFavorite = TRUE;

	if(mb_IsModified)
	{
		switch
		(
			M_MF()->MessageBox
				(
					"The file has been modified. Continue without saving ?",
					EDI_STR_Csz_TitleConfirm,
					MB_ICONWARNING | MB_YESNO
				)
		)
		{
		case IDYES: UpdateData(FALSE); break;
		default:	return;
		}
	}

	if(mb_IsLoadedByMyself) SND_InsertUnload(SND_M_GetInsertFromPtr(mp_Insert));
	mb_IsLoadedByMyself = FALSE;
	SetIsModified(FALSE);

	/*$1- get file name ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() != IDOK) return;
	o_File.GetItem(o_File.mo_File, 0, o_Temp);

	/*$1- get file index/key ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulFatIndex = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
	if(BIG_ul_GetLengthFile(BIG_PosFile(ulFatIndex)) == 0)
	{
		mp_Insert = SND_p_InsertCreate(BIG_FileKey(ulFatIndex), en_InsYaxisVol, en_InsXaxisTime, 2);
		SND_InsertSave(mp_Insert, BIG_FileKey(ulFatIndex));
		SND_InsertUnload(SND_M_GetInsertFromPtr(mp_Insert));
	}

	/*$1- get the pointer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LOA_MakeFileRef
	(
		BIG_FileKey(ulFatIndex),
		(ULONG *) &mp_Insert,
		SND_ul_CallbackInsertLoad,
		LOA_C_MustExists | LOA_C_HasUserCounter
	);
	LOA_Resolve();
	LoadData();

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	InvalidateRect(mo_DrawRect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::Copy(void)
{
	/*~~~~~*/
	int i, n;
	/*~~~~~*/

	if(!mb_SelRect) SelAll(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]);

	n = SelCount(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]);
	AllocData(M_ClipboardCurv, n);

	for(i = 0, n = 0; n < ml_NbKey[M_CurrentCurv]; n++)
	{
		if(mpst_Curve[M_CurrentCurv][n].sel)
		{
			mpst_Curve[M_ClipboardCurv][i].tx = mpst_Curve[M_CurrentCurv][n].tx;
			mpst_Curve[M_ClipboardCurv][i].ty = mpst_Curve[M_CurrentCurv][n].ty;
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
void EDIA_cl_SndInsert::AllocData(int i, int _nb)
{
	if(ml_NbKey[i])
	{
		mpst_Curve[i] = (EDIA_tdst_EdSndInsertKey *) L_realloc(mpst_Curve[i], sizeof(EDIA_tdst_EdSndInsertKey) * _nb);
	}
	else
	{
		mpst_Curve[i] = (EDIA_tdst_EdSndInsertKey *) L_malloc(sizeof(EDIA_tdst_EdSndInsertKey) * _nb);
	}

	ml_NbKey[i] = _nb;
}

/*$F
 =======================================================================================================================
 [0] = (float)	x(0)
 [1] = (float)	dx/dy(0)
 [2] = (float)	x(1)
 [3] = (float)	dx/dy(1)
 ...
 [2*i]   = (float)	x(i)
 [2*i+1] = (float)	dx/dy(i)
 ...
 [2*(N-1)]   = (float)	x(N-1)
 [2*(N-1)+1] = (float)	dx/dy(N-1)
 =======================================================================================================================
 */

void EDIA_cl_SndInsert::LoadData(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	float			*pf_Data = (float *) mp_Insert->pst_KeyArray->dst_Key;
	unsigned int	uiNb = mp_Insert->pst_KeyArray->ui_Size;
	ULONG			ulFat;
	SND_tdst_InsKey *pKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulFat = BIG_ul_SearchKeyToFat(mp_Insert->ul_FileKey);
	if(BIG_b_IsFileExtension(ulFat, EDI_Csz_ExtSoundFade))
	{
		if((mp_Insert->ul_Flags & SND_Cte_InsFade) == 0)
		{
			mp_Insert->ul_Flags |= SND_Cte_InsFade;
			SetIsModified(TRUE);
		}

		if(mp_Insert->Xaxis.pst_InsVoid->e_ChunkId != en_InsXaxisTime)
		{
			SND_InsertChangeType(mp_Insert, en_InsXaxisTime);
			mp_Insert->Xaxis.pst_TimeDef->f_Min = mf_MinX = 0.0f;
			mp_Insert->Xaxis.pst_TimeDef->f_Max = mf_MaxX = 100.0f;
			SetIsModified(TRUE);
		}

		if(mp_Insert->Yaxis.pst_InsVoid->e_ChunkId != en_InsYaxisVol)
		{
			SND_InsertChangeType(mp_Insert, en_InsYaxisVol);
			mp_Insert->Yaxis.pst_VolDef->b_Wet = FALSE;
			mp_Insert->Yaxis.pst_VolDef->f_Min = mf_MinY = 0.0f;
			mp_Insert->Yaxis.pst_VolDef->f_Max = mf_MaxY = 100.0f;
			SetIsModified(TRUE);
		}

		GetDlgItem(IDC_COMBO_TYPE_X)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_TYPE_X)->ShowWindow(SW_SHOW);
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(mp_Insert->Xaxis.pst_InsVoid->e_ChunkId)
	{
	case en_InsXaxisDistance:
		EnableXaxisAndGao(TRUE, TRUE);
		if(mp_Insert->Xaxis.pst_DistDef->ui_Axis & SND_Cte_InsAxisX)
			mi_TypeX = 0;
		else if(mp_Insert->Xaxis.pst_DistDef->ui_Axis & SND_Cte_InsAxisY)
			mi_TypeX = 1;
		else if(mp_Insert->Xaxis.pst_DistDef->ui_Axis & SND_Cte_InsAxisZ)
			mi_TypeX = 2;
		else if(mp_Insert->Xaxis.pst_DistDef->ui_Axis & SND_Cte_InsAxisXYZ)
			mi_TypeX = 3;
		mul_GaoKey = mp_Insert->Xaxis.pst_DistDef->ui_GaoKey;
		mf_MaxX = mp_Insert->Xaxis.pst_DistDef->f_Max;
		mf_MinX = mp_Insert->Xaxis.pst_DistDef->f_Min;

		ulFat = BIG_ul_SearchKeyToFat(mul_GaoKey);
		if(ulFat == BIG_C_InvalidIndex)
			GetDlgItem(IDC_BUTTON_GAO)->SetWindowText("None");
		else
			GetDlgItem(IDC_BUTTON_GAO)->SetWindowText(BIG_NameFile(ulFat));
		break;

	case en_InsXaxisTime:
		EnableXaxisAndGao(TRUE, FALSE);
		mi_TypeX = 4;
		mf_MaxX = mp_Insert->Xaxis.pst_TimeDef->f_Max;
		mf_MinX = mp_Insert->Xaxis.pst_TimeDef->f_Min;
		break;

	case en_InsXaxisVar:
		EnableXaxisAndGao(FALSE, FALSE);
		mi_TypeX = 5 + mp_Insert->Xaxis.pst_VarDef->ui_VarId;
		break;

	default:
		break;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(mp_Insert->Yaxis.pst_InsVoid->e_ChunkId)
	{
	case en_InsYaxisVol:
		mi_TypeY = mp_Insert->Yaxis.pst_VolDef->b_Wet ? 1 : 0;
		mf_MinY = mp_Insert->Yaxis.pst_VolDef->f_Min;
		mf_MaxY = mp_Insert->Yaxis.pst_VolDef->f_Max;
		break;

	case en_InsYaxisPan:
		mi_TypeY = mp_Insert->Yaxis.pst_PanDef->b_Surround ? 4 : 3;
		mf_MinY = mp_Insert->Yaxis.pst_PanDef->f_Min;
		mf_MaxY = mp_Insert->Yaxis.pst_VolDef->f_Max;
		break;

	case en_InsYaxisFreq:
		mi_TypeY = 2;
		mf_MinY = mp_Insert->Yaxis.pst_FreqDef->f_Min;
		mf_MaxY = mp_Insert->Yaxis.pst_FreqDef->f_Max;
		break;

	default:
		break;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i = (pf_Data && uiNb) ? uiNb : 2;

	AllocData(M_CurrentCurv, i);

	pKey = mp_Insert->pst_KeyArray->dst_Key;
	pf_Data = (float *) mp_Insert->pst_KeyArray->dst_Key;

	mpst_Curve[M_CurrentCurv][0].tx = 0.0f;
	mpst_Curve[M_CurrentCurv][0].ty = pKey[0].tx;

	for(i = 1; i < ml_NbKey[M_CurrentCurv] - 1; i++, pf_Data += 2)
	{
		mpst_Curve[M_CurrentCurv][i].tx = pKey[i].tx;

		mpst_Curve[M_CurrentCurv][i].ty = mpst_Curve[M_CurrentCurv][i - 1].ty + (mpst_Curve[M_CurrentCurv][i].tx - mpst_Curve[M_CurrentCurv][i - 1].tx) * pKey[i - 1].ty;
	}

	mpst_Curve[M_CurrentCurv][ml_NbKey[M_CurrentCurv] - 1].tx = 1.0f;
	mpst_Curve[M_CurrentCurv][ml_NbKey[M_CurrentCurv] - 1].ty = pKey[ml_NbKey[M_CurrentCurv] - 1].tx;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mb_Cycling = (mp_Insert->ul_Flags & SND_Cte_InsCycling) ? TRUE : FALSE;
	mb_ActiveOnPlay = (mp_Insert->ul_Flags & SND_Cte_InsActiveOnPlay) ? TRUE : FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mb_WndInitDone) UpdateData(FALSE);

	mb_SelRect = FALSE;
	SelNone(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]);

    SetIsModified(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::SaveData(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	float			*pf_Cur;
	int				i;
	SND_tdst_InsKey *pKey;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(!mb_IsModified) return;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	UpdateData();

	
    /*$1- distance X ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_TypeX == 0)
    {
		SND_InsertChangeType(mp_Insert, en_InsXaxisDistance);
		mp_Insert->Xaxis.pst_DistDef->ui_Axis = SND_Cte_InsAxisX;
		mp_Insert->Xaxis.pst_DistDef->ui_GaoKey = mul_GaoKey;
		mp_Insert->Xaxis.pst_DistDef->f_Min = mf_MinX;
		mp_Insert->Xaxis.pst_DistDef->f_Max = mf_MaxX;
    }

	/*$1- distance Y ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	else if(mi_TypeX == 1)
    {
		SND_InsertChangeType(mp_Insert, en_InsXaxisDistance);
		mp_Insert->Xaxis.pst_DistDef->ui_Axis = SND_Cte_InsAxisY;
		mp_Insert->Xaxis.pst_DistDef->ui_GaoKey = mul_GaoKey;
		mp_Insert->Xaxis.pst_DistDef->f_Min = mf_MinX;
		mp_Insert->Xaxis.pst_DistDef->f_Max = mf_MaxX;
    }

	/*$1- distance Z ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	else if(mi_TypeX == 2)
    {
	
		SND_InsertChangeType(mp_Insert, en_InsXaxisDistance);
		mp_Insert->Xaxis.pst_DistDef->ui_Axis = SND_Cte_InsAxisZ;
		mp_Insert->Xaxis.pst_DistDef->ui_GaoKey = mul_GaoKey;
		mp_Insert->Xaxis.pst_DistDef->f_Min = mf_MinX;
		mp_Insert->Xaxis.pst_DistDef->f_Max = mf_MaxX;
    }

	/*$1- distance XYZ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	else if(mi_TypeX == 3)
    {
	    SND_InsertChangeType(mp_Insert, en_InsXaxisDistance);
		mp_Insert->Xaxis.pst_DistDef->ui_Axis = SND_Cte_InsAxisXYZ;
		mp_Insert->Xaxis.pst_DistDef->ui_GaoKey = mul_GaoKey;
		mp_Insert->Xaxis.pst_DistDef->f_Min = mf_MinX;
		mp_Insert->Xaxis.pst_DistDef->f_Max = mf_MaxX;
    }

	/*$1- time ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	else if(mi_TypeX == 4)
    {

		SND_InsertChangeType(mp_Insert, en_InsXaxisTime);
		mp_Insert->Xaxis.pst_TimeDef->f_Min = mf_MinX;
		mp_Insert->Xaxis.pst_TimeDef->f_Max = mf_MaxX;
    }

	/*$1- AI avr ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   	else if((mi_TypeX >=5) && (mi_TypeX <= 36))
    {
		SND_InsertChangeType(mp_Insert, en_InsXaxisVar);
		mp_Insert->Xaxis.pst_VarDef->ui_VarId = mi_TypeX - 5;
    }
    else
    {
		ERR_X_Assert(0);	
	}

	switch(mi_TypeY)
	{

	/*$1- dry volume ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case 0:
		SND_InsertChangeType(mp_Insert, en_InsYaxisVol);
		mp_Insert->Yaxis.pst_VolDef->b_Wet = FALSE;
		mp_Insert->Yaxis.pst_VolDef->f_Min = mf_MinY;
		mp_Insert->Yaxis.pst_VolDef->f_Max = mf_MaxY;
		break;

	/*$1- wet volume ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case 1:
		SND_InsertChangeType(mp_Insert, en_InsYaxisVol);
		mp_Insert->Yaxis.pst_VolDef->b_Wet = TRUE;
		mp_Insert->Yaxis.pst_VolDef->f_Min = mf_MinY;
		mp_Insert->Yaxis.pst_VolDef->f_Max = mf_MaxY;
		break;

	/*$1- freq ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case 2:
		SND_InsertChangeType(mp_Insert, en_InsYaxisFreq);
		mp_Insert->Yaxis.pst_FreqDef->f_Min = mf_MinY;
		mp_Insert->Yaxis.pst_FreqDef->f_Max = mf_MaxY;
		break;

	/*$1- pan ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case 3:
		SND_InsertChangeType(mp_Insert, en_InsYaxisPan);
		mp_Insert->Yaxis.pst_PanDef->b_Surround = FALSE;
		mp_Insert->Yaxis.pst_PanDef->f_Min = mf_MinY;
		mp_Insert->Yaxis.pst_PanDef->f_Max = mf_MaxY;
		break;

	/*$1- surround pan ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case 4:
		SND_InsertChangeType(mp_Insert, en_InsYaxisPan);
		mp_Insert->Yaxis.pst_PanDef->b_Surround = TRUE;
		mp_Insert->Yaxis.pst_PanDef->f_Min = mf_MinY;
		mp_Insert->Yaxis.pst_PanDef->f_Max = mf_MaxY;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	default:
		ERR_X_Assert(0);
		break;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ml_NbKey[M_CurrentCurv] > (long) mp_Insert->pst_KeyArray->ui_Size)
	{
		mp_Insert->pst_KeyArray->dst_Key = (SND_tdst_InsKey *) MEM_p_Realloc
			(
				mp_Insert->pst_KeyArray->dst_Key,
				ml_NbKey[M_CurrentCurv] * sizeof(SND_tdst_InsKey)
			);
	}

	pf_Cur = (float *) mp_Insert->pst_KeyArray->dst_Key;
	pKey = mp_Insert->pst_KeyArray->dst_Key;

	for(i = 1; i < ml_NbKey[M_CurrentCurv]; i++)
	{
		pKey[i].tx = mpst_Curve[M_CurrentCurv][i].tx;
		pKey[i - 1].ty = mpst_Curve[M_CurrentCurv][i].tx - mpst_Curve[M_CurrentCurv][i - 1].tx;

		if(!pKey[i - 1].ty)
		{
			pKey[i].tx += 0.00000001f;
			pKey[i - 1].ty = 0.00000001f;
		}

		pKey[i - 1].ty = (mpst_Curve[M_CurrentCurv][i].ty - mpst_Curve[M_CurrentCurv][i - 1].ty) / pKey[i - 1].ty;
	}

	pKey[0].tx = mpst_Curve[M_CurrentCurv][0].ty;
	pKey[ml_NbKey[M_CurrentCurv] - 1].tx = mpst_Curve[M_CurrentCurv][ml_NbKey[M_CurrentCurv] - 1].ty;

	mp_Insert->pst_KeyArray->ui_Size = (unsigned int) ml_NbKey[M_CurrentCurv];

	if(mb_Cycling)
		mp_Insert->ul_Flags |= SND_Cte_InsCycling;
	else
		mp_Insert->ul_Flags &= ~SND_Cte_InsCycling;

	if(mb_ActiveOnPlay)
		mp_Insert->ul_Flags |= SND_Cte_InsActiveOnPlay;
	else
		mp_Insert->ul_Flags &= ~SND_Cte_InsActiveOnPlay;

	SND_InsertSave(mp_Insert, mp_Insert->ul_FileKey);
	SetIsModified(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::GetPickable(CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	EDIA_tdst_EdSndInsertKey	*k;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_Pickable = -1;
	k = mpst_Curve[M_CurrentCurv];

	if((pt.x >= mo_CurveRect.left - 4) && (pt.x < mo_CurveRect.left))
	{
		for(i = 0; i < ml_NbKey[M_CurrentCurv]; i++, k++)
		{
			me_PickedCoord = en_SelYaxis;
			if((pt.y >= k->y - 3) && (pt.y <= k->y + 3))
			{
				mi_Pickable = i;
				return;
			}
		}
	}
	else if((pt.y > mo_CurveRect.bottom) && (pt.y <= mo_CurveRect.bottom + 4))
	{
		k++;
		for(i = 1; i < ml_NbKey[M_CurrentCurv] - 1; i++, k++)
		{
			me_PickedCoord = en_SelXaxis;
			if((pt.x >= k->x - 3) && (pt.x <= k->x + 3))
			{
				mi_Pickable = i;
				return;
			}
		}
	}
	else
	{
		for(i = 0; i < ml_NbKey[M_CurrentCurv]; i++, k++)
		{
			me_PickedCoord = en_SelCurvePoints;
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
void EDIA_cl_SndInsert::AdjustKey(int _i_Dir)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	EDIA_tdst_EdSndInsertKey	*k;
	BOOL						bModif = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Dir >= 0)
	{
		k = mpst_Curve[M_CurrentCurv];
		for(i = 0; i < ml_NbKey[M_CurrentCurv]; i++, k++)
		{
			if(i + 1 < ml_NbKey[M_CurrentCurv])
			{
				if(k[1].tx < k->tx)
				{
					k[1].tx = k->tx;
					bModif = TRUE;
				}
			}

			if(k->tx > 1)
			{
				k->tx = 1;
				bModif = TRUE;
			}

			if(k->ty <= 0)
			{
				k->ty = 0;
				bModif = TRUE;
			}

			if(k->ty >= 1)
			{
				k->ty = 1;
				bModif = TRUE;
			}
		}
	}
	else
	{
		k = mpst_Curve[M_CurrentCurv] + (ml_NbKey[M_CurrentCurv] - 1);
		for(i = ml_NbKey[M_CurrentCurv] - 1; i >= 0; i--, k--)
		{
			if(i)
			{
				if(k[-1].tx > k->tx)
				{
					k[-1].tx = k->tx;
					bModif = TRUE;
				}
			}

			if(k->tx < 0)
			{
				k->tx = 0;
				bModif = TRUE;
			}

			if(k->ty <= 0)
			{
				k->ty = 0;
				bModif = TRUE;
			}

			if(k->ty >= 1)
			{
				k->ty = 1;
				bModif = TRUE;
			}
		}
	}

	if(bModif) SetIsModified(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::SortKey(int curve)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, j;
	EDIA_tdst_EdSndInsertKey	k;
	BOOL						bModif = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 1; i < ml_NbKey[curve]; i++)
	{
		if(mpst_Curve[curve][i].tx > 1)
		{
			mpst_Curve[curve][i].tx = 1;
			bModif = TRUE;
		}
		else if(mpst_Curve[curve][i].tx < 0)
		{
			mpst_Curve[curve][i].tx = 0;
			bModif = TRUE;
		}

		if(mpst_Curve[curve][i].ty > 1)
		{
			mpst_Curve[curve][i].ty = 1;
			bModif = TRUE;
		}
		else if(mpst_Curve[curve][i].ty < 0)
		{
			mpst_Curve[curve][i].ty = 0;
			bModif = TRUE;
		}

		if(i == 1) continue;

		j = i - 1;
		while(mpst_Curve[curve][i].tx < mpst_Curve[curve][j].tx) j--;
		if(j != i - 1)
		{
			bModif = TRUE;
			L_memcpy(&k, mpst_Curve[curve] + i, sizeof(EDIA_tdst_EdSndInsertKey));
			L_memmove
			(
				mpst_Curve[curve] + j + 2,
				mpst_Curve[curve] + j + 1,
				(i - j - 1) * sizeof(EDIA_tdst_EdSndInsertKey)
			);
			L_memcpy(mpst_Curve[curve] + j + 1, &k, sizeof(EDIA_tdst_EdSndInsertKey));
		}
	}

	if(bModif) SetIsModified(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::ScaleKey(CSize &_PrevSize, int _hDir, int _vDir)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	EDIA_tdst_EdSndInsertKey	*k;
	float						fx, fy, x, y;
	BOOL						bModif = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_hDir == 1)
		x = ((float) (mo_SelRect.left - mo_CurveRect.left)) / mf_Time2Width;
	else if(_hDir == -1)
		x = ((float) (mo_SelRect.right - mo_CurveRect.left)) / mf_Time2Width;

	if(_vDir == 1)
		y = ((float) (mo_CurveRect.bottom - mo_SelRect.top)) / mf_Time2Height;
	else if(_vDir == -1)
		y = ((float) (mo_CurveRect.bottom - mo_SelRect.bottom)) / mf_Time2Height;

	fx = (float) mo_SelRect.Width() / (float) _PrevSize.cx;
	fy = (float) mo_SelRect.Height() / (float) _PrevSize.cy;

	k = mpst_Curve[M_CurrentCurv] + 1;
	for(i = 1; i < ml_NbKey[M_CurrentCurv]; i++, k++)
	{
		if(!k->sel) continue;
		if(_hDir != 0)
		{
			k->tx = (k->tx - x) * fx + x;
			bModif = TRUE;
		}

		if(_vDir != 0)
		{
			k->ty = (k->ty - y) * fy + y;
			bModif = TRUE;
		}
	}

	if(bModif) SetIsModified(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::SelectPointInRect(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							j;
	EDIA_tdst_EdSndInsertKey	*k;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	k = mpst_Curve[M_CurrentCurv];
	for(j = 0; j < ml_NbKey[M_CurrentCurv]; j++, k++)
	{
		if
		(
			(k->x >= mo_SelRect.left)
		&&	(k->x <= mo_SelRect.right)
		&&	(k->y >= mo_SelRect.top)
		&&	(k->y <= mo_SelRect.bottom)
		) k->sel = 1;
	}

	InvalidateRect(mo_DrawRect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SndInsert::SomethingSel(EDIA_tdst_EdSndInsertKey *pst_Key, int _i_Count)
{
	while(_i_Count--)
	{
		if(pst_Key++->sel) return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_SndInsert::SelCount(EDIA_tdst_EdSndInsertKey *pst_Key, int _i_Count)
{
	/*~~*/
	int n;
	/*~~*/

	n = 0;
	while(_i_Count--)
		if(pst_Key++->sel) n++;
	return n;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::SelNone(EDIA_tdst_EdSndInsertKey *pst_Key, int _i_Count)
{
	while(_i_Count--) pst_Key++->sel = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::SelAll(EDIA_tdst_EdSndInsertKey *pst_Key, int _i_Count)
{
	while(_i_Count--) pst_Key++->sel = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::SelDelete(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, j;
	EDIA_tdst_EdSndInsertKey	*k;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ml_NbKey[M_CurrentCurv] < 3) return;

	/* first point never deleted */
	i = j = 1;
	k = mpst_Curve[M_CurrentCurv];

	while(j < ml_NbKey[M_CurrentCurv] - 1)
	{
		if(!k[j].sel)
		{
			if(i != j)
			{
				k[i].sel = k[j].sel;
				k[i].tx = k[j].tx;
				k[i].ty = k[j].ty;
			}

			i++;
		}

		j++;
	}

	/* last point never deleted */
	if(i != j)
	{
		k[i].sel = k[j].sel;
		k[i].tx = k[j].tx;
		k[i].ty = k[j].ty;
	}

	i++;

	ml_NbKey[M_CurrentCurv] = i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::SelDeleteSpecial(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, j;
	EDIA_tdst_EdSndInsertKey	*k;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ml_NbKey[M_CurrentCurv] < 3) return;

	i = j = 0;
	k = mpst_Curve[M_CurrentCurv];

	while(j < ml_NbKey[M_CurrentCurv] - 1)
	{
		if(!k[j].sel)
		{
			if(i != j)
			{
				k[i].sel = k[j].sel;
				k[i].tx = k[j].tx;
				k[i].ty = k[j].ty;
			}

			i++;
		}

		j++;
	}

	ml_NbKey[M_CurrentCurv] = i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::Paste(void)
{
	/*~~~~*/
	int end;
	/*~~~~*/

	mb_SelRect = FALSE;
	SelNone(mpst_Curve[M_CurrentCurv], ml_NbKey[M_CurrentCurv]);

	if(ml_NbKey[M_ClipboardCurv] != 0)
	{
		end = ml_NbKey[M_CurrentCurv];
		AllocData(M_CurrentCurv, ml_NbKey[M_CurrentCurv] + ml_NbKey[M_ClipboardCurv]);

		L_memcpy
		(
			mpst_Curve[M_CurrentCurv] + (ml_NbKey[M_CurrentCurv] - 1),
			mpst_Curve[M_CurrentCurv] + (end - 1),
			sizeof(EDIA_tdst_EdSndInsertKey)
		);
		L_memcpy
		(
			mpst_Curve[M_CurrentCurv] + (end - 1),
			mpst_Curve[M_ClipboardCurv],
			ml_NbKey[M_ClipboardCurv] * sizeof(EDIA_tdst_EdSndInsertKey)
		);

		SortKey(M_CurrentCurv);

		SetIsModified(TRUE);
	}

	InvalidateRect(mo_DrawRect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::HistPush(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_tdst_HistoryInsertList *pst_New, *pst_Old;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Old = mpst_History->pst_Next;
	if(pst_Old) pst_Old->pst_Key = NULL;
	while(pst_Old)
	{
		pst_New = pst_Old->pst_Next;
		if(pst_Old->pst_Key) L_free(pst_Old->pst_Key);
		L_free(pst_Old);
		pst_Old = pst_New;
	}

	pst_New = (EDIA_tdst_HistoryInsertList *) L_malloc(sizeof(EDIA_tdst_HistoryInsertList));
	pst_New->l_NbKey = ml_NbKey[M_CurrentCurv];
	pst_New->pst_Key = (EDIA_tdst_EdSndInsertKey *) L_malloc(pst_New->l_NbKey * sizeof(EDIA_tdst_EdSndInsertKey));
	L_memcpy(pst_New->pst_Key, mpst_Curve[M_CurrentCurv], pst_New->l_NbKey * sizeof(EDIA_tdst_EdSndInsertKey));
	pst_New->b_SelRect = mb_SelRect;
	pst_New->o_SelRect = mo_SelRect;
	pst_New->pst_Prev = mpst_History;
	mpst_History->pst_Next = pst_New;
	pst_New->pst_Next = NULL;
	mpst_History = pst_New;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::HistPop(void)
{
	if(!mpst_History->pst_Prev) return;

	if(mpst_History->pst_Next == NULL)
	{
		HistPush();
		mpst_History = mpst_History->pst_Prev;
	}

	ml_NbKey[M_CurrentCurv] = mpst_History->l_NbKey;
	mpst_Curve[M_CurrentCurv] = mpst_History->pst_Key;
	mb_SelRect = mpst_History->b_SelRect;
	mo_SelRect = mpst_History->o_SelRect;
	mpst_History = mpst_History->pst_Prev;

	InvalidateRect(mo_DrawRect);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnLButtonDblClk(UINT ui, CPoint pt)
{
	OnKeyDown(VK_INSERT);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnButtonGao(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString				o_Temp;
	ULONG				ul_Index;
	EDIA_cl_FileDialog	o_File("Choose File", 0, 0, 1, NULL, "*" EDI_Csz_ExtGameObject);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_File.mb_UseFavorite = TRUE;
	if(o_File.DoModal() == IDOK)
	{
		o_File.GetItem(o_File.mo_File, 0, o_Temp);
		ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		if(ul_Index == BIG_C_InvalidIndex)
		{
			mul_GaoKey = BIG_C_InvalidKey;
			GetDlgItem(IDC_BUTTON_GAO)->SetWindowText("None");
		}
		else
		{
			mul_GaoKey = BIG_FileKey(ul_Index);
			o_File.mo_File.SetAt(o_File.mo_File.GetLength() - 1, '\0');
			o_Temp = CString(o_File.masz_FullPath) + "/" + o_File.mo_File;
			GetDlgItem(IDC_BUTTON_GAO)->SetWindowText(o_Temp);
		}

		SetIsModified(TRUE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::SetIsModified(BOOL b_Modified)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ulFat;
	CString o_file = CString("new");
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mb_IsModified = b_Modified;
	if(mp_Insert)
	{
		ulFat = BIG_ul_SearchKeyToFat(mp_Insert->ul_FileKey);
		if(ulFat != BIG_C_InvalidIndex) o_file = CString(BIG_NameFile(ulFat));
	}

	if(mb_IsModified)
	{
		o_file = CString("Insert [") + o_file + CString(" *]");
		this->SetWindowText(o_file);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	else
	{
		o_file = CString("Insert [") + o_file + CString("]");
		this->SetWindowText(o_file);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndInsert::OnChangeMinMax(void)
{
	SetIsModified(TRUE);
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
