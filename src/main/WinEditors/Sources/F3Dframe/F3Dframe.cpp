/*$T F3Dframe.cpp GC!1.71 01/20/00 17:07:07 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "EDIapp.h"
#include "EDIeditors_infos.h"
#include "EDImainframe.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dstrings.h"
#include "Res/Res.h"
#include "EDItors/Sources/PROperties/PROframe.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGloop.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "DIAlogs/DIAselection_dlg.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAanim_dlg.h"
#ifdef JADEFUSION
#include "DIAlogs/DIAlightrej_dlg.h"
#endif
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(F3D_cl_Frame, CFrameWnd)
    ON_WM_ERASEBKGND()
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
F3D_cl_Frame::F3D_cl_Frame(void)
{
    mpo_DisplayView = NULL;
    mb_LeftSel = FALSE;
	mb_SelMinimize = FALSE;
	mpo_Parent = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
F3D_cl_Frame::~F3D_cl_Frame(void)
{
    mpo_DisplayView->DestroyWindow();
    delete mpo_DisplayView;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_Frame::MyCreate(EDI_cl_BaseFrame *_po_Editor, CWnd *_po_Parent)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect, o_Rect1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Create display view */
    mpo_AssociatedEditor = _po_Editor;
	mpo_Parent = _po_Parent;
    mpo_DisplayView = new F3D_cl_View;
    mpo_DisplayView->mpo_AssociatedEditor = _po_Editor;
	mpo_DisplayView->mpo_Frame = this;

    /* Create frame */
    mo_PrevRect = CRect(0, 0, 100, 100);
    Create(NULL, "", WS_BORDER | WS_VISIBLE | WS_CHILD, mo_PrevRect, _po_Parent, NULL, WS_EX_TOOLWINDOW);

    /* Create display view */
    GetClientRect(&o_Rect);
	LINK_gb_AllRefreshEnable = FALSE;
    mpo_DisplayView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, o_Rect, this, 0);
	LINK_gb_AllRefreshEnable = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LRESULT F3D_cl_Frame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    /*~~~~~~~~~~~~~~~~~~~~*/
    LRESULT     result;
    CRect       o_Rect;
    CRect       o_Rect1;
    /*~~~~~~~~~~~~~~~~~~~~*/

    /* Special case to size selection dialog */
	if (
	        ( ( message == WM_SIZE ) || ( message == WM_MOVING ) ) &&
	        ( ( ( mpo_DisplayView->mpo_SelectionDialog ) && ( mpo_DisplayView->mb_SelectOn ) ) || ( ( mpo_DisplayView->mpo_ToolBoxDialog ) && ( mpo_DisplayView->mb_ToolBoxOn ) )
#	ifdef JADEFUSION
	          || ( ( mpo_DisplayView->mpo_LightRejectDialog ) && ( mpo_DisplayView->mb_LightRejectOn ) )
#	endif
	          || ( mpo_DisplayView->mpo_AnimDialog && mpo_DisplayView->mb_AnimOn ) ) )
    {
		GetClientRect( &o_Rect );

#ifdef JADEFUSION
		// NB: Added Light Rejection dialog - from POP5 Jade Editor
		// NOTE: Light rejection dialog always goes to the opposite side of selection dialog
		if(mpo_DisplayView->mb_LightRejectOn)
		{
            if( !mb_LeftSel )
			{
				o_Rect1 = o_Rect;
				o_Rect.right = o_Rect.left + WIDTH_LIGHTREJECT;
				if(mpo_DisplayView->mpo_LightRejectDialog) 
					mpo_DisplayView->mpo_LightRejectDialog->MoveWindow(&o_Rect);
				o_Rect.left = o_Rect.right;
				o_Rect.right = o_Rect1.right;
				mpo_DisplayView->MoveWindow(&o_Rect);
			}
			else
			{
				o_Rect1 = o_Rect;
				o_Rect.left = o_Rect.right - WIDTH_LIGHTREJECT;
				if(mpo_DisplayView->mpo_LightRejectDialog) 
					mpo_DisplayView->mpo_LightRejectDialog->MoveWindow(&o_Rect);
				
				o_Rect.right = o_Rect.left;
				o_Rect.left = o_Rect1.left;
				mpo_DisplayView->MoveWindow(&o_Rect);
			}
		}
#endif		

		if(mb_LeftSel && (mpo_DisplayView->mb_ToolBoxOn || mpo_DisplayView->mb_SelectOn))
        {
			o_Rect1 = o_Rect;
            o_Rect.right = o_Rect.left + WIDTH_SEL;
			if(mb_SelMinimize) o_Rect.right -= WIDTH_MINSEL;
			if(mpo_DisplayView->mpo_SelectionDialog) mpo_DisplayView->mpo_SelectionDialog->MoveWindow(&o_Rect);
			if(mpo_DisplayView->mpo_ToolBoxDialog) mpo_DisplayView->mpo_ToolBoxDialog->MoveWindow(&o_Rect);
            o_Rect.left = o_Rect.right;
			o_Rect.right = o_Rect1.right;
            mpo_DisplayView->MoveWindow(&o_Rect);
        }
        else if(mpo_DisplayView->mb_ToolBoxOn || mpo_DisplayView->mb_SelectOn)
        {
			o_Rect1 = o_Rect;
            o_Rect.left = o_Rect.right - WIDTH_SEL;
			if(mb_SelMinimize) o_Rect.left += WIDTH_MINSEL;
            if(mpo_DisplayView->mpo_SelectionDialog) mpo_DisplayView->mpo_SelectionDialog->MoveWindow(&o_Rect);
			if(mpo_DisplayView->mpo_ToolBoxDialog) mpo_DisplayView->mpo_ToolBoxDialog->MoveWindow(&o_Rect);
            o_Rect.right = o_Rect.left;
			o_Rect.left = o_Rect1.left;
            mpo_DisplayView->MoveWindow(&o_Rect);
        }
		else
		{
			mpo_DisplayView->MoveWindow( &o_Rect );
		}

		UpdatePosSize();
    }
	else if(MAI_b_TreatOwnerWndMessages(m_hWnd, &mpo_DisplayView->mst_WinHandles, message, wParam, lParam, &result))
    {
        return result;
    }

    return CFrameWnd::DefWindowProc(message, wParam, lParam);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_Frame::MinMaxSel(void)
{
	M_MF()->LockDisplay(this);
	mb_SelMinimize = mb_SelMinimize ? FALSE : TRUE;
	if(mb_SelMinimize)
	{
		if(mpo_DisplayView->mpo_SelectionDialog)
		{
			mpo_DisplayView->mpo_SelectionDialog->GetDlgItem(IDC_MINIMIZE)->SetWindowText("+");
			mpo_DisplayView->mpo_SelectionDialog->ScrollWindow(-WIDTH_MINSCRSEL, 0);
		}
		if(mpo_DisplayView->mpo_ToolBoxDialog)
		{
			mpo_DisplayView->mpo_ToolBoxDialog->GetDlgItem(IDC_MINIMIZE)->SetWindowText("+");
			mpo_DisplayView->mpo_ToolBoxDialog->ScrollWindow(-WIDTH_MINSCRSEL, 0);
		}
	}
	else
	{
		if(mpo_DisplayView->mpo_SelectionDialog)
		{
			mpo_DisplayView->mpo_SelectionDialog->GetDlgItem(IDC_MINIMIZE)->SetWindowText("-");
			mpo_DisplayView->mpo_SelectionDialog->ScrollWindow(WIDTH_MINSCRSEL, 0);
		}
		if(mpo_DisplayView->mpo_ToolBoxDialog)
		{
			mpo_DisplayView->mpo_ToolBoxDialog->GetDlgItem(IDC_MINIMIZE)->SetWindowText("-");
			mpo_DisplayView->mpo_ToolBoxDialog->ScrollWindow(WIDTH_MINSCRSEL, 0);
		}
	}
	SendMessage(WM_SIZE, 0, 0);
	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_Frame::UpdatePosSize(void)
{
    /*~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~*/

    /* Force the frame to fill the parent area */
    if(mpo_Parent)
        mpo_Parent->GetClientRect(o_Rect);
    else
        GetParent()->GetClientRect(o_Rect);

    MoveWindow(o_Rect);

	if ( mpo_DisplayView->mpo_AnimDialog && mpo_DisplayView->mb_AnimOn )
	{
#	ifdef JADEFUSION
		if ( !mpo_DisplayView->mb_ToolBoxOn && !mpo_DisplayView->mb_SelectOn && !mpo_DisplayView->mb_LightRejectOn )
#	else
		if ( !mpo_DisplayView->mb_ToolBoxOn && !mpo_DisplayView->mb_SelectOn )
#	endif
		{
			GetClientRect( &o_Rect );
		}
		mpo_DisplayView->GetWindowRect( &o_Rect );
		mpo_DisplayView->mpo_AnimDialog->MoveWindow( o_Rect.left + 5, o_Rect.bottom - 50, o_Rect.Width() - 10, 40 );
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL F3D_cl_Frame::OnEraseBkgnd(CDC*)
{
    /*~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~*/

    /*
     * Force readapt surface if we constat that screen coordinates of window has
     * changed (cause if parent of 3D frame has moving, the 3D frame itselve will not
     * receive a WM_MOVE.
     */
    GetWindowRect(&o_Rect);
    if(o_Rect != mo_PrevRect)
    {
        mo_PrevRect = o_Rect;
        GetParent()->ScreenToClient(&o_Rect);
        SendMessage(WM_MOVE, o_Rect.left + (o_Rect.top << 16));
    }

    return TRUE;
}

#endif /* ACTIVE_EDITORS */
