/*$T AIleftframe.cpp GC!1.52 10/11/99 09:48:06 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "AIframe.h"
#include "EditView/AIleftview.h"
#include "EditView/AIview.h"
#include "AIleftframe.h"
#include "BIGfiles/BIGfat.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "BASe/MEMory/MEM.h"
#include "EDImainframe.h"

#include "DATaControl/DATCPerforce.h"

extern BOOL EDI_gb_NoUpdateVSS;

/*$4
 ***************************************************************************************************
    GLOBAL VARS
 ***************************************************************************************************
 */

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EAI_cl_InsideFrame, CWnd)
    ON_WM_SIZE()
    ON_WM_MOUSEWHEEL()
    ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
END_MESSAGE_MAP()

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_InsideFrame::OnSize(UINT n, int cx, int cy)
{
    CWnd::OnSize(n, cx, cy);
    if(IsWindow(mpo_Parent->mpo_LeftView->m_hWnd))
        mpo_Parent->mpo_LeftView->MoveWindow(0, -1, 18, cy + 2);
    if(mpo_Parent->mpo_Edit && IsWindow(mpo_Parent->mpo_Edit->m_hWnd))
        mpo_Parent->mpo_Edit->MoveWindow(18, -1, cx - 18, cy + 2);
};

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EAI_cl_InsideFrame::OnMouseWheel(UINT, short, CPoint)
{
    return FALSE;
};

/*
 ===================================================================================================
 ===================================================================================================
 */
int EAI_cl_InsideFrame::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
{
    CWnd::OnToolHitTest(point, pTI);
    return 1;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
BOOL EAI_cl_InsideFrame::OnToolTipText(UINT iID, NMHDR *pNMHDR, LRESULT *pResult)
{
    return TRUE;
}

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EAI_cl_LeftFrame, CListCtrl)
    ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_LeftFrame::SelectFromFile(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_File;
    LV_FINDINFO st_Info;
    int         i_SelItem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul_File = mpo_Parent->mul_CurrentEditFile;
    st_Info.flags = LVFI_PARAM;
    st_Info.lParam = ul_File;
    i_SelItem = FindItem(&st_Info);
    if(i_SelItem != -1)
        SetItemState(i_SelItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_LeftFrame::SelectItem(int i_Item)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_File;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    M_MF()->LockDisplay(this);
    ul_File = GetItemData(i_Item);
    if(ul_File != mpo_Parent->mul_CurrentEditFile)
    {
		// Get stats again to be sure the panes are displayed with the right color.
		if ( DAT_CPerforce::GetInstance()->P4Connect()  )
		{
			// Get P4 file path
			std::vector<std::string> vP4Files;
			std::string asz_P4FilePath;
			DAT_CUtils::GetP4FileFromKey(BIG_FileKey(ul_File), asz_P4FilePath, DAT_CPerforce::GetInstance()->GetP4Root());
			vP4Files.push_back(asz_P4FilePath);
			DAT_CPerforce::GetInstance()->P4Fstat(vP4Files);
			DAT_CPerforce::GetInstance()->P4Disconnect();

			// Sometimes, there is still an hourglass instead of an arrow. So force the cursor
			//::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}

        if(mpo_Parent->b_CanClose())
            mpo_Parent->Load(GetItemData(i_Item));
        else
            SelectFromFile();
    }

    mpo_Parent->mpo_Edit->SetFocus();
    M_MF()->UnlockDisplay(this);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_LeftFrame::OnLButtonDown(UINT n, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int i_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    i_Item = HitTest(pt);
    if(i_Item == -1) return;
    CListCtrl::OnLButtonDown(n, pt);
    SelectItem(i_Item);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_LeftFrame::OnRButtonDown(UINT n, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EMEN_cl_SubMenu o_Menu(FALSE);
    int             i_Item;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    i_Item = HitTest(pt);
    if(i_Item == -1)
	{
		ClientToScreen(&pt);
		mpo_Parent->InitPopupMenuAction(&o_Menu);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_NEWFUNCTION);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_NEWPROCLIST);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_NEWVARS);
		mpo_Parent->AddPopupMenuAction(&o_Menu, 0);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_ADDFUNCTION);
		mpo_Parent->TrackPopupMenuAction(pt, &o_Menu);
	}
	else
	{
		SelectItem(i_Item);
		ClientToScreen(&pt);
		mpo_Parent->InitPopupMenuAction(&o_Menu);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_NEWFUNCTION);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_NEWPROCLIST);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_NEWVARS);
		mpo_Parent->AddPopupMenuAction(&o_Menu, 0);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_ADDFUNCTION);
		mpo_Parent->AddPopupMenuAction(&o_Menu, 0);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_RENAMEFUNC);
		mpo_Parent->AddPopupMenuAction(&o_Menu, 0);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_DELETE);
		mpo_Parent->AddPopupMenuAction(&o_Menu, 0);
		mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_SHOWINBROWSER);
		mpo_Parent->AddPopupMenuAction(&o_Menu, 0);
		
		if( M_MF()->mst_Ini.b_LinkControlON ) 
		{
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_CHECKOUT);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_CHECKIN);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_CHECKINOUT);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_UNDOCHECKOUTFILE);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_GETLATESTFILE);
			mpo_Parent->AddPopupMenuAction(&o_Menu, 0);
		}
		else 
		{

			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_P4_SUBMIT);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_P4_EDIT);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_P4_SUBMITEDIT);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_P4_REVERT);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_P4_SYNC);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_P4_ADD);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_P4_DELETE);
			mpo_Parent->AddPopupMenuAction(&o_Menu, 0);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_P4_DIFF);
			mpo_Parent->AddPopupMenuAction(&o_Menu, EAI_ACTION_P4_SHOWHISTORY);
		}

		mpo_Parent->TrackPopupMenuAction(pt, &o_Menu);
	}
}

/*
 ===================================================================================================
 ===================================================================================================
 */
static int CALLBACK si_CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *p1, *p2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    p1 = BIG_NameFile(lParam1);
    p2 = BIG_NameFile(lParam2);
    switch(lParamSort)
    {
    case 0:
        if(!p1) return -1;
        if(!p2) return 1;
        return L_strcmpi(p1, p2);
    case 1:
        p1 = L_strrchr(p1, '.');
        if(!p1) return -1;
        p2 = L_strrchr(p2, '.');
        if(!p2) return 1;
        return L_strcmpi(p1, p2);
    }

    return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_LeftFrame::OnColumnClick(NMHDR *pNotifyStruct2, LRESULT *result)
{
	NM_LISTVIEW *pNotifyStruct;
	pNotifyStruct = (NM_LISTVIEW *)pNotifyStruct2;
    mpo_Parent->mst_Ini.i_SortedCol = pNotifyStruct->iSubItem;
    SortItems(si_CompareFunc, mpo_Parent->mst_Ini.i_SortedCol);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EAI_cl_LeftFrame::SortList(void)
{
    SortItems(si_CompareFunc, mpo_Parent->mst_Ini.i_SortedCol);
}

#endif /* ACTIVE_EDITORS */
