/*$T AIleftview.cpp GC! 1.100 03/08/01 14:13:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "../AIframe.h"
#include "AIview.h"
#include "AIleftview.h"
#include "Res/Res.h"
#include "BIGfiles/BIGdefs.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/*$2 
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EAI_cl_LeftView, CFrameWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_LeftView::EAI_cl_LeftView(void)
{
	Init();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_LeftView::~EAI_cl_LeftView(void)
{
	mo_ListBreak.RemoveAll();
	mo_ListBreakFile.RemoveAll();
	mo_ListBreakModel.RemoveAll();
	mo_ListBreakInstance.RemoveAll();
	mo_ListBreakPtInst.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_LeftView::Init(void)
{
	mi_LineError = -1;
	mi_LineBreak = -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_LeftView::OnEraseBkgnd(CDC *pDC)
{
	/*~~~~~~~~~~~*/
	DWORD	x_Col;
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	x_Col = GetSysColor(COLOR_BTNFACE);

	GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	pDC->FillSolidRect(o_Rect, x_Col);

	o_Rect.left = o_Rect.right - 1;
	pDC->Draw3dRect(o_Rect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_LeftView::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				l_FirstLine, l_LastLine;
	int					ix;
	CRect				o_Rect;
	CDC					*pDC;
	HICON				h_Icon;
	POSITION			pos, pos1, pos2, pos3, pos4;
	int					i_LineBreak;
	BIG_INDEX			ul_BreakFile, ul_Model, ul_Instance;
	AI_tdst_Instance	*pst_Instance;
	CPoint				pt;
	int					iSel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CFrameWnd::OnPaint();

	l_FirstLine = mpo_Frame->mpo_Edit->l_FirstVisibleLine();
	l_LastLine = mpo_Frame->mpo_Edit->l_LastVisibleLine();

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Mark
	 -------------------------------------------------------------------------------------------------------------------
	 */
	pos = mpo_Frame->mo_History.Find(mpo_Frame->mul_CurrentEditFile);
	if(pos)
	{
		iSel = 0;
		pos1 = mpo_Frame->mo_History.GetHeadPosition();
		while(pos != pos1)
		{
			mpo_Frame->mo_History.GetNext(pos1);
			iSel++;
		}

		pos = mpo_Frame->mo_HistoryMarks[iSel].GetHeadPosition();
		while(pos)
		{
			iSel = mpo_Frame->mo_HistoryMarks[iSel].GetNext(pos);
			if((iSel >= l_FirstLine) && (iSel <= l_LastLine))
			{
				pt = mpo_Frame->mpo_Edit->GetCharPos(mpo_Frame->mpo_Edit->LineIndex(iSel));
				pt.y += 4;
				pDC = GetDC();
				h_Icon = AfxGetApp()->LoadIcon(EAI_IDI_MARK);
				pDC->DrawIcon(2, pt.y, h_Icon);
				ReleaseDC(pDC);
				DeleteObject(h_Icon);
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Draw error mark.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mi_LineError != -1)
	{
		/* Display error if line is visible */
		if((mi_LineError >= l_FirstLine) && (mi_LineError <= l_LastLine))
		{
			pt = mpo_Frame->mpo_Edit->GetCharPos(mpo_Frame->mpo_Edit->LineIndex(mi_LineError));
			pt.y += 4;
			pDC = GetDC();
			h_Icon = AfxGetApp()->LoadIcon(EAI_IDI_ERROR);
			pDC->DrawIcon(2, pt.y, h_Icon);
			ReleaseDC(pDC);
			DeleteObject(h_Icon);
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Draw stop break mark
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mi_LineBreak != -1)
	{
		/* Display error if line is visible */
		if((mi_LineBreak >= l_FirstLine) && (mi_LineBreak <= l_LastLine))
		{
			pt = mpo_Frame->mpo_Edit->GetCharPos(mpo_Frame->mpo_Edit->LineIndex(mi_LineBreak));
			pt.y += 4;
			pDC = GetDC();
			h_Icon = AfxGetApp()->LoadIcon(EAI_IDI_BREAKSTOP);
			pDC->DrawIcon(2, pt.y, h_Icon);
			ReleaseDC(pDC);
			DeleteObject(h_Icon);
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Draw breakpoint mark
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mpo_Frame->mul_CurrentEditFile != BIG_C_InvalidIndex)
	{
		if(!AI_gi_NumBreak)
		{
			mo_ListBreak.RemoveAll();
			mo_ListBreakFile.RemoveAll();
			mo_ListBreakModel.RemoveAll();
			mo_ListBreakInstance.RemoveAll();
			mo_ListBreakPtInst.RemoveAll();
		}

		pos = mo_ListBreak.GetHeadPosition();
		pos1 = mo_ListBreakFile.GetHeadPosition();
		pos2 = mo_ListBreakModel.GetHeadPosition();
		pos3 = mo_ListBreakInstance.GetHeadPosition();
		pos4 = mo_ListBreakPtInst.GetHeadPosition();
		while(pos)
		{
			i_LineBreak = mo_ListBreak.GetNext(pos);
			ul_BreakFile = BIG_ul_SearchKeyToFat(mo_ListBreakFile.GetNext(pos1));
			ul_Model = BIG_ul_SearchKeyToFat(mo_ListBreakModel.GetNext(pos2));
			ul_Instance = BIG_ul_SearchKeyToFat(mo_ListBreakInstance.GetNext(pos3));
			pst_Instance = (AI_tdst_Instance *) mo_ListBreakPtInst.GetNext(pos4);
			if
			(
				(i_LineBreak >= l_FirstLine)
			&&	(i_LineBreak <= l_LastLine)
			&&	(ul_BreakFile == mpo_Frame->mul_CurrentEditFile)
			)
			{
				if
				(
					(ul_Instance == BIG_C_InvalidIndex)
				||	(ul_Instance == mpo_Frame->mul_CurrentEditInstance)
				||	(pst_Instance == mpo_Frame->mpst_Instance)
				)
				{
					pt = mpo_Frame->mpo_Edit->GetCharPos(mpo_Frame->mpo_Edit->LineIndex(i_LineBreak));
					pt.y += 4;
					pDC = GetDC();
					if(AI_gb_CanBreak)
					{
						if
						(
							(ul_Instance == BIG_C_InvalidIndex)
						&&	((pst_Instance != mpo_Frame->mpst_Instance) || (pst_Instance == NULL))
						)
						{
							h_Icon = AfxGetApp()->LoadIcon(EAI_IDI_BREAK);	/* Model break */
							ix = 2;
						}
						else if(ul_Instance == mpo_Frame->mul_CurrentEditInstance)
						{
							h_Icon = AfxGetApp()->LoadIcon(EAI_IDI_BREAK1); /* Instance break */
							ix = 4;
						}
						else
							h_Icon = NULL;
					}
					else
					{
						h_Icon = AfxGetApp()->LoadIcon(EAI_IDI_BREAK2);
						ix = 2;
					}

					if(h_Icon)
					{
						pDC->DrawIcon(ix, pt.y, h_Icon);
						ReleaseDC(pDC);
						DeleteObject(h_Icon);
					}
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_LeftView::OnLButtonDown(UINT ui, CPoint pt)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if(mpo_Frame->mul_CurrentEditFile != BIG_C_InvalidIndex)
		mpo_Frame->mpo_Edit->PostMessage(WM_LBUTTONDOWN, ui, (pt.y << 16) + 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_LeftView::OnMouseMove(UINT ui, CPoint pt)
{
	CFrameWnd::OnMouseMove(ui, pt);
}

#endif /* ACTIVE_EDITORS */
