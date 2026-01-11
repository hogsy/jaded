/*$T DIAsndvumeter.cpp GC! 1.081 10/22/02 16:16:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAsndvumeter.h"
#include "DIAlogs/DIAsndvumeterset.h"
#include "Res/Res.h"
#include "VAVview/VAVlist.h"
#include "LINks/LINKtoed.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"
#include "ENGine/Sources/ENGvars.h"

#include "SouND/sources/SND.h"
#include "SouND/sources/SNDwave.h"
#include "SouND/sources/SNDtrack.h"
#include "Editors/Sources/SOuNd/SONframe.h"
#include "Editors/Sources/SOuNd/SONvumeter.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_SndVumeterDialog, EDIA_cl_BaseDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndVumeterDialog::EDIA_cl_SndVumeterDialog(ESON_cl_VUmeter *pManager) :
	EDIA_cl_BaseDialog(ESON_IDD_VUMETER)
{
	mpo_VumeterManager = pManager;
	mh_BrushRed = CreateSolidBrush(RGB(250, 50, 50));
	mh_BrushGreen = CreateSolidBrush(RGB(20, 250, 20));
	mh_BrushBlue = CreateSolidBrush(RGB(20, 50, 150));
	mh_BrushYellow = CreateSolidBrush(RGB(250, 250, 50));
	mb_New = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_SndVumeterDialog::~EDIA_cl_SndVumeterDialog(void)
{
	mpo_VumeterManager = NULL;
	DeleteObject(mh_BrushRed);
	DeleteObject(mh_BrushGreen);
	DeleteObject(mh_BrushBlue);
	DeleteObject(mh_BrushYellow);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndVumeterDialog::OnDestroy(void)
{
	if(mpo_VumeterManager)
	{
		mpo_VumeterManager->mpo_SndVumeterDialog = NULL;
	}

	EDIA_cl_BaseDialog::OnDestroy();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndVumeterDialog::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	RECT		mrc;
	int			ysize;
	int			ypos;
	char		asz[512];
	RECT		r;
	int			x;
	int			xp;
	int			delta;
	int			val;
	float		ratio;
	CDC			*pDC;
	CPaintDC	dc(this);
	/*~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnPaint();
	if(!mb_New) return;
	mb_New = FALSE;

	/*$2- get device context -----------------------------------------------------------------------------------------*/

	GetClientRect(&mrc);
	pDC = &dc;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	delta = 3;
	ratio = (9.0f / 10.0f);
	ysize = (int) ((float) mrc.bottom / 2.5f);
	ypos = delta;

	/*$2- clear screen -----------------------------------------------------------------------------------------------*/

	/*
	 * pDC->SelectObject(GetStockObject(BLACK_BRUSH)); pDC->Rectangle(mrc.left,
	 * mrc.top, mrc.right, mrc.bottom);
	 */
	if(!mpo_VumeterManager) return;

	/*$2- draw current value -----------------------------------------------------------------------------------------*/

	val = 10000 + mpo_VumeterManager->mst_VumeterValues.st_Left.i_CurrentLevel; /* positive value */
	val -= 6000;																/* only 50 dB scale */
	if(val < 0) val = 0;

	if(val > 3500)
	{
		pDC->SelectObject(mh_BrushBlue);
		x = (int) ((((float) 1000) * ratio * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(0, ypos, x, ypos + ysize);
		xp = x;

		pDC->SelectObject(mh_BrushGreen);
		x = (int) (((float) 2500 * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, ypos, x, ypos + ysize);
		xp = x;

		pDC->SelectObject(mh_BrushYellow);
		x = (int) (((float) 3500 * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, ypos, x, ypos + ysize);
		xp = x;

		pDC->SelectObject(mh_BrushRed);
		x = (int) (((float) val * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, ypos, x, ypos + ysize);
	}
	else if(val > 2500)
	{
		pDC->SelectObject(mh_BrushBlue);
		x = (int) ((((float) 1000) * ratio * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(0, ypos, x, ypos + ysize);
		xp = x;

		pDC->SelectObject(mh_BrushGreen);
		x = (int) (((float) 2500 * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, ypos, x, ypos + ysize);
		xp = x;

		pDC->SelectObject(mh_BrushYellow);
		x = (int) (((float) val * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, ypos, x, ypos + ysize);
	}
	else if(val > 1000)
	{
		pDC->SelectObject(mh_BrushBlue);
		x = (int) (((float) 1000 * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(0, ypos, x, ypos + ysize);
		xp = x;

		pDC->SelectObject(mh_BrushGreen);
		x = (int) (((float) val * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, ypos, x, ypos + ysize);
	}
	else
	{
		pDC->SelectObject(mh_BrushBlue);
		x = (int) (((float) val * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(0, ypos, x, ypos + ysize);
	}

	val = 10000 + mpo_VumeterManager->mst_VumeterValues.st_Right.i_CurrentLevel;
	val -= 6000;
	if(val < 0) val = 0;

	if(val > 3500)
	{
		pDC->SelectObject(mh_BrushBlue);
		x = (int) (((float) 1000 * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(0, mrc.bottom - ysize - ypos, x, mrc.bottom - ypos);
		xp = x;

		pDC->SelectObject(mh_BrushGreen);
		x = (int) (((float) 2500 * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, mrc.bottom - ysize - ypos, x, mrc.bottom - ypos);
		xp = x;

		pDC->SelectObject(mh_BrushYellow);
		x = (int) (((float) 3500 * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, mrc.bottom - ysize - ypos, x, mrc.bottom - ypos);
		xp = x;

		pDC->SelectObject(mh_BrushRed);
		x = (int) (((float) val * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, mrc.bottom - ysize - ypos, x, mrc.bottom - ypos);
	}
	else if(val > 2500)
	{
		pDC->SelectObject(mh_BrushBlue);
		x = (int) (((float) 1000 * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(0, mrc.bottom - ysize - ypos, x, mrc.bottom - ypos);
		xp = x;

		pDC->SelectObject(mh_BrushGreen);
		x = (int) (((float) 2500 * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, mrc.bottom - ysize - ypos, x, mrc.bottom - ypos);
		xp = x;

		pDC->SelectObject(mh_BrushYellow);
		x = (int) (((float) val * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, mrc.bottom - ysize - ypos, x, mrc.bottom - ypos);
	}
	else if(val > 1000)
	{
		pDC->SelectObject(mh_BrushBlue);
		x = (int) (((float) 1000 * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(0, mrc.bottom - ysize - ypos, x, mrc.bottom - ypos);
		xp = x;

		pDC->SelectObject(mh_BrushGreen);
		x = (int) (((float) val * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(xp, mrc.bottom - ysize - ypos, x, mrc.bottom - ypos);
	}
	else
	{
		pDC->SelectObject(mh_BrushBlue);
		x = (int) (((float) val * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
		pDC->Rectangle(0, mrc.bottom - ysize - ypos, x, mrc.bottom - ypos);
	}

	/*$2- draw peak value --------------------------------------------------------------------------------------------*/

	pDC->SelectObject(mh_BrushRed);
	pDC->SetBkMode(OPAQUE);

	val = 10000 + mpo_VumeterManager->mst_VumeterValues.st_Left.i_PeakLevel;
	val -= 5000;
	x = (int) (((float) val * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
	pDC->Rectangle(x, ypos, x + 4, ypos + ysize);

	val = 10000 + mpo_VumeterManager->mst_VumeterValues.st_Right.i_PeakLevel;
	val -= 5000;
	x = (int) (((float) val * (ratio) * (float) (mrc.right - mrc.left)) / 5000.0f);
	pDC->Rectangle(x, mrc.bottom - ysize - ypos, x + 4, mrc.bottom - ypos);

	/*$2- peak value -------------------------------------------------------------------------------------------------*/

	pDC->SetBkColor(RGB(0, 0, 0));
	pDC->SetTextColor(RGB(250, 250, 250));
	r.left = (int) ((float) mrc.right - ((float) (mrc.right - mrc.left) * (1.0f - ratio)));
	r.top = ypos;
	r.right = mrc.right - delta;
	r.bottom = ypos + ysize;
	sprintf(asz, "% -3.1f", (float) mpo_VumeterManager->mst_VumeterValues.st_Left.i_PeakLevel / 100.0f);
	pDC->DrawText(asz, -1, &r, 0);

	r.left = (int) ((float) mrc.right - ((float) (mrc.right - mrc.left) * (1.0f - ratio)));
	r.top = mrc.bottom - ysize - ypos;
	r.right = mrc.right - delta;
	r.bottom = mrc.bottom - ypos;
	sprintf(asz, "% -3.1f", (float) mpo_VumeterManager->mst_VumeterValues.st_Right.i_PeakLevel / 100.0f);
	pDC->DrawText(asz, -1, &r, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_SndVumeterDialog::OnEraseBkgnd(CDC *pDC)
{
	/*~~~~~~~~*/
	RECT	mrc;
	/*~~~~~~~~*/

	GetClientRect(&mrc);
	pDC->SelectObject(GetStockObject(BLACK_BRUSH));
	pDC->Rectangle(mrc.left, mrc.top, mrc.right, mrc.bottom);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_SndVumeterDialog::OnRButtonDown(UINT ui, CPoint o_Pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_SndVumeterSetDialog *po_Dial;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_VumeterManager) return;

	if((GetAsyncKeyState(VK_CONTROL) < 0) || (GetAsyncKeyState(VK_MENU) < 0))
	{
		po_Dial = new EDIA_cl_SndVumeterSetDialog;
		po_Dial->mi_Falloff = mpo_VumeterManager->mi_Falloff;
		po_Dial->mi_MaxScale = mpo_VumeterManager->mi_MaxScale;
		po_Dial->mi_PeakTTL = mpo_VumeterManager->mi_PeakTTL;
		po_Dial->mi_Scale = mpo_VumeterManager->mi_Scale;

		if(po_Dial->DoModal())
		{
			mpo_VumeterManager->mi_Falloff = po_Dial->mi_Falloff;
			mpo_VumeterManager->mi_MaxScale = po_Dial->mi_MaxScale;
			mpo_VumeterManager->mi_PeakTTL = po_Dial->mi_PeakTTL;
			mpo_VumeterManager->mi_Scale = po_Dial->mi_Scale;
            mpo_VumeterManager->ValueResetAll();
		}
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */

