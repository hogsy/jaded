/*$T PROframe.cpp GC! 1.081 04/11/00 09:47:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "EDImainframe.h"
#include "PROframe.h"
#include "PROstrings.h"

#define ACTION_GLOBAL

#include "PROframe_act.h"
#include "ENGine/Sources/ENGinit.h"
#include "LINKs/LINKtoed.h"

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

IMPLEMENT_DYNCREATE(EPRO_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EPRO_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
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
EPRO_cl_Frame::EPRO_cl_Frame(void)
{
	mpo_VarsView = new EVAV_cl_View;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EPRO_cl_Frame::~EPRO_cl_Frame(void)
{
	mpo_VarsView->ResetList();
	delete mpo_VarsView;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EPRO_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1) return -1;

	/* Create variable view. */
	mst_VarsViewStruct.po_ListItems = &mo_ListItems;

	mst_VarsViewStruct.psz_NameCol1 = EPRO_STR_Csz_Name;
	mst_VarsViewStruct.i_WidthCol1 = 50;
	mst_VarsViewStruct.psz_NameCol2 = EPRO_STR_Csz_Value;
	mpo_VarsView->mb_CanDragDrop = TRUE;
	mpo_VarsView->MyCreate(this, &mst_VarsViewStruct);

	return 0;
}

extern BAS_tdst_barray	LINK_gst_Pointers;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPRO_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	/*~~~~~~~~~~*/
	BOOL	b_Vis;
	/*~~~~~~~~~~*/

	b_Vis = FALSE;

	/* Call parent function */
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);

	/* Resize vars view */
	if(::IsWindow(mpo_VarsView->m_hWnd))
	{
		if(IsWindowVisible())
		{
			M_MF()->LockDisplay(this);
			b_Vis = TRUE;
		}

		mpo_VarsView->MoveWindow(0, 0, cx, cy);

		if(b_Vis)
		{
			M_MF()->UnlockDisplay(this);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPRO_cl_Frame::OneTrameEnding(void)
{
	if
	(
		M_MF()->mpo_MaxView
	&&	M_MF()->mpo_MaxView != mpo_MyView
	&&	!mpo_MyView->mb_Floating
	&&	M_MF()->mst_Desktop.b_VeryMaximized
	&&	mb_IsActivate
	) return;

	if(LINK_gi_SpeedDraw != 0) return;
	mpo_VarsView->mpo_ListBox->BeforeRefresh();
	mpo_VarsView->mpo_ListBox->RedrawWindow(NULL, NULL, RDW_UPDATENOW);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPRO_cl_Frame::AfterEngine(void)
{
	OneTrameEnding();
}

#endif /* ACTIVE_EDITORS */
