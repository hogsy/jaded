/*$T DIAtoolbox_curveview.cpp GC! 1.086 07/06/00 16:23:35 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAtoolbox_curveview.h"
#include "Res/Res.h"
#include "SOFT/SOFTHelper.h"
#include "LINKs/LINKtoed.h"

IMPLEMENT_DYNCREATE(EDIA_cl_ToolBox_CurveView, CFormView)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBox_CurveView::EDIA_cl_ToolBox_CurveView(EDIA_cl_ToolBoxDialog *_po_ToolBox) :
	CFormView(DIALOGS_IDD_TOOLBOX_CURVE)
{
	mpo_ToolBox = _po_ToolBox;
	mi_Capture = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBox_CurveView::EDIA_cl_ToolBox_CurveView(void) :
	CFormView(DIALOGS_IDD_TOOLBOX_CURVE)
{
	EDIA_cl_ToolBox_CurveView(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ToolBox_CurveView::~EDIA_cl_ToolBox_CurveView(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_ToolBox_CurveView::Create
(
	LPCTSTR		lpszClassName,
	LPCTSTR		lpszWindowName,
	DWORD		dwStyle,
	const RECT	&rect,
	CWnd		*pParentWnd
)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int		x;
	char	sz_Value[100];
	/*~~~~~~~~~~~~~~~~~~*/

	CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, 0, NULL);

	x = mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesBefore;
	GetDlgItem(IDC_STATIC_NBFRAMESBEFORE)->SetWindowText(_itoa(x, sz_Value, 10));
	x = mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesAfter;
	GetDlgItem(IDC_STATIC_NBFRAMESAFTER)->SetWindowText(_itoa(x, sz_Value, 10));

	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    Message map
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_ToolBox_CurveView, CFormView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_DG0, IDC_CHECK_DG31, OnDGCheckClick)
	ON_BN_CLICKED(IDC_CHECK_USEGIZMOFLAG, OnUseGFCheckClicked)
	ON_BN_CLICKED(IDC_BUTTON_NUMBERFRAMECENTER, OnButtonNbFrameCenterClicked)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    Message handles
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_CurveView::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	CDC		*pDC;
	int		x, ym;
	LONG	l_NbFrames;
	CPen	o_RedPen, o_BluePen;
	void	*p_OldPen;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	CFormView::OnPaint();

	pDC = GetDC();
	o_RedPen.CreatePen(PS_SOLID, 2, RGB(0xFF, 0, 0));
	o_BluePen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0xFF));

	/* before */
	l_NbFrames = mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesBefore;
	GetDlgItem(IDC_STATIC_NBFRAMESBEFOREEDIT)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	ym = (o_Rect.top + o_Rect.bottom) / 2;

	p_OldPen = pDC->SelectObject(o_BluePen);
	pDC->MoveTo(o_Rect.left + 4, ym);
	pDC->LineTo(o_Rect.right - 4, ym);
	x = o_Rect.right - o_Rect.left - 8;
	x = x * (l_NbFrames);
	x /= 64;
	x = o_Rect.left + 4 + x;
	pDC->MoveTo(x + 2, ym);
	pDC->LineTo(x, ym + 2);
	pDC->LineTo(x - 2, ym);
	pDC->LineTo(x, ym - 2);
	pDC->LineTo(x + 2, ym);
	pDC->MoveTo(x + 1, ym + 1);
	pDC->LineTo(x - 1, ym - 1);
	pDC->SetPixel(x, ym, RGB(0xFF, 0, 0));

	/* after */
	l_NbFrames = mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesAfter;
	GetDlgItem(IDC_STATIC_NBFRAMESAFTEREDIT)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	ym = (o_Rect.top + o_Rect.bottom) / 2;

	pDC->SelectObject(o_RedPen);
	pDC->MoveTo(o_Rect.left + 4, ym);
	pDC->LineTo(o_Rect.right - 4, ym);
	x = o_Rect.right - o_Rect.left - 8;
	x = x * (l_NbFrames);
	x /= 64;
	x = o_Rect.left + 4 + x;
	pDC->MoveTo(x + 2, ym);
	pDC->LineTo(x, ym + 2);
	pDC->LineTo(x - 2, ym);
	pDC->LineTo(x, ym - 2);
	pDC->LineTo(x + 2, ym);
	pDC->MoveTo(x + 1, ym + 1);
	pDC->LineTo(x - 1, ym - 1);
	pDC->SetPixel(x, ym, RGB(0, 0, 0xFF));

	pDC->SelectObject(p_OldPen);
	DeleteObject(o_RedPen);
	DeleteObject(o_BluePen);
	ReleaseDC(pDC);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_CurveView::OnLButtonDown(UINT nFlags, CPoint point)
{
	/*~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	int		x;
	char	sz_Value[10];
	/*~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_STATIC_NBFRAMESBEFOREEDIT)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	if(PtInRect(&o_Rect, point))
	{
		x = point.x - o_Rect.left - 4;
		if(x < 0)
			x = 0;
		else
		{
			x *= 64;
			x /= o_Rect.right - o_Rect.left - 8;
			if(x > 64) x = 64;
		}

		if(x != mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesBefore)
		{
			mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesBefore = x;
			InvalidateRect(&o_Rect);
			GetDlgItem(IDC_STATIC_NBFRAMESBEFORE)->SetWindowText(_itoa(x, sz_Value, 10));
		}

		mi_Capture = 1;
		SetCapture();
		return;
	}

	GetDlgItem(IDC_STATIC_NBFRAMESAFTEREDIT)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	if(PtInRect(&o_Rect, point))
	{
		x = point.x - o_Rect.left - 4;
		if(x < 0)
			x = 0;
		else
		{
			x *= 64;
			x /= o_Rect.right - o_Rect.left - 8;
			if(x > 64) x = 64;
		}

		if(x != mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesAfter)
		{
			mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesAfter = x;
			InvalidateRect(&o_Rect);
			GetDlgItem(IDC_STATIC_NBFRAMESAFTER)->SetWindowText(_itoa(x, sz_Value, 10));
		}

		mi_Capture = 2;
		SetCapture();
		return;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_CurveView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(mi_Capture)
	{
		mi_Capture = 0;
		ReleaseCapture();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_CurveView::OnMouseMove(UINT nFlags, CPoint point)
{
	/*~~~~~~~~~~~~~~~~~*/
	CRect	o_Rect;
	int		x;
	char	sz_Value[10];
	/*~~~~~~~~~~~~~~~~~*/

	if(mi_Capture == 1)
	{
		GetDlgItem(IDC_STATIC_NBFRAMESBEFOREEDIT)->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);

		x = point.x - o_Rect.left - 4;
		if(x < 0)
			x = 0;
		else
		{
			x *= 64;
			x /= o_Rect.right - o_Rect.left - 8;
			if(x > 64) x = 64;
		}

		if(x != mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesBefore)
		{
			mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesBefore = x;
			InvalidateRect(&o_Rect);
			GetDlgItem(IDC_STATIC_NBFRAMESBEFORE)->SetWindowText(_itoa(x, sz_Value, 10));
		}
		LINK_Refresh();
	}

	if(mi_Capture == 2)
	{
		GetDlgItem(IDC_STATIC_NBFRAMESAFTEREDIT)->GetWindowRect(&o_Rect);
		ScreenToClient(&o_Rect);

		x = point.x - o_Rect.left - 4;
		if(x < 0)
			x = 0;
		else
		{
			x *= 64;
			x /= o_Rect.right - o_Rect.left - 8;
			if(x > 64) x = 64;
		}

		if(x != mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesAfter)
		{
			mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesAfter = x;
			InvalidateRect(&o_Rect);
			GetDlgItem(IDC_STATIC_NBFRAMESAFTER)->SetWindowText(_itoa(x, sz_Value, 10));
		}
		LINK_Refresh();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_CurveView::OnDGCheckClick(UINT i)
{
	/*~~~~~~~*/
	ULONG	ul;
	/*~~~~~~~*/

	ul = 0;
	for(i = 31; i >= 0; i--)
	{
		ul <<= 1;
		ul |= ((CButton *) GetDlgItem(IDC_CHECK_DG0 + i))->GetCheck() ? 1 : 0;
	}

	mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->ul_GizmoDisplayFlag[0] = ul;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_CurveView::OnUseGFCheckClicked(void)
{
	if(((CButton *) GetDlgItem(IDC_CHECK_USEGIZMOFLAG))->GetCheck())
	{
		mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->ul_CurveFlag |= SOFT_Cl_UseGizmoFlag;
	}
	else
	{
		mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->ul_CurveFlag &= ~SOFT_Cl_UseGizmoFlag;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ToolBox_CurveView::OnButtonNbFrameCenterClicked(void)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesBefore = 0;
	GetDlgItem(IDC_STATIC_NBFRAMESBEFOREEDIT)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	InvalidateRect(&o_Rect);
	GetDlgItem(IDC_STATIC_NBFRAMESBEFORE)->SetWindowText("0");

	mpo_ToolBox->mpo_View->mst_WinHandles.pst_DisplayData->pst_Helpers->l_NbFramesAfter = 0;
	GetDlgItem(IDC_STATIC_NBFRAMESAFTEREDIT)->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	InvalidateRect(&o_Rect);
	GetDlgItem(IDC_STATIC_NBFRAMESAFTER)->SetWindowText("0");
}

#endif /* ACTIVE_EDITORS */
