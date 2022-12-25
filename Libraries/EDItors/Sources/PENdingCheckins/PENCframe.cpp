/*$T PENCframe.cpp GC 1.138 03/11/05 09:02:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "EDItors/Sources/BROwser/BROframe.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "EDIpaths.h"
#include "EDIicons.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAname_dlg.h"
#include "SELection/SELection.h"
#include "PENCframe.h"
#include "EDImsg.h"
#include "EDItors/Sources/MENu/MENmenu.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#define ACTION_GLOBAL
#include "PENCframe_act.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include <windowsx.h>
#include "DIAlogs/DIAmsglink_dlg.h"
#include "LINKs/LINKtoed.h"

extern VSSCB					gpfn_VSSR;
extern void					s_VssRecurse(char *, BIG_INDEX, int);

extern EDIA_cl_MsgLinkDialog	EDI_go_MsgGetLatest;
extern EDIA_cl_MsgLinkDialog	EDI_go_MsgUndoCheck;
extern EDIA_cl_MsgLinkDialog	EDI_go_MsgCheckInNo;
extern BOOL						EDI_gb_NoUpdateVSS;
extern BOOL						EDI_gb_CheckInDel;
extern BOOL						EDI_can_lock;
extern int						EDI_gi_OnlyOneVssCheck;
extern int						EDI_gi_OnlyOneRefCheck;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EPEN_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EPEN_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PARENTNOTIFY()
	ON_NOTIFY(NM_DBLCLK, 1000, On_TVN_Notify)
	ON_NOTIFY(NM_CUSTOMDRAW, 1000, On_TVN_Notify)
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
EPEN_cl_Frame::EPEN_cl_Frame(void)
{
	mpo_TreeView = new CTreeCtrl;
	mb_Locked = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EPEN_cl_Frame::~EPEN_cl_Frame(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EPEN_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1)
	{
		return -1;
	}

	mpo_TreeView->Create
		(
			WS_VISIBLE |
				WS_TABSTOP |
				WS_CHILD |
				WS_BORDER |
				TVS_HASBUTTONS |
				TVS_HASLINES |
				TVS_LINESATROOT |
				TVS_DISABLEDRAGDROP |
				TVS_SHOWSELALWAYS,
			CRect(0, 0, 0, 0),
			this,
			1000
		);

	mpo_TreeView->SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPEN_cl_Frame::PreTranslateMessage(MSG *pMsg)
{
	return EDI_cl_BaseFrame::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	if(mpo_TreeView) mpo_TreeView->MoveWindow(5, 0, cx - 10, cy - 5);
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);
}

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

static EPEN_cl_Frame	*gpo_Ed;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void RefreshCB(int index)
{
	gpo_Ed->AddFile(index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::AddFile(BIG_INDEX index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM	tree;
	int			img;
	char		az[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!gpo_Ed->IndexDirToTree.Lookup((void *) BIG_ParentFile(index), (void * &) tree))
	{
		BIG_ComputeFullName(BIG_ParentFile(index), az);
		tree = gpo_Ed->mpo_TreeView->InsertItem(az, EDI_IMAGE_FOLDER1, EDI_IMAGE_FOLDER1, TVI_ROOT);
		gpo_Ed->mpo_TreeView->SetItemData(tree, BIG_ParentFile(index) | 0x80000000);
		gpo_Ed->IndexDirToTree.SetAt((void *) BIG_ParentFile(index), tree);
	}

	img = M_MF()->i_GetIconImage(NULL, BIG_NameFile(index), 0);
	tree = gpo_Ed->mpo_TreeView->InsertItem(BIG_NameFile(index), img, img, tree);
	gpo_Ed->mpo_TreeView->SetItemData(tree, index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::Refresh(void)
{
	/*~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Fat;
	/*~~~~~~~~~~~~~~~*/

	mb_Locked = FALSE;
	if(!M_MF()->mst_Ini.b_LinkControlON)
	{
		IndexDirToTree.RemoveAll();
		mpo_TreeView->DeleteAllItems();
		return;
	}

	AfxGetApp()->DoWaitCursor(1);

	ul_Fat = BIG_ul_SearchDir(BIG_Csz_Root);
	if(ul_Fat == BIG_C_InvalidIndex) return;
	if(!M_MF()->b_LockBigFile()) return;
	M_MF()->VssFileChanged(FALSE, TRUE);

	mpo_TreeView->SetRedraw(FALSE);

	IndexDirToTree.RemoveAll();
	mpo_TreeView->DeleteAllItems();

	gpo_Ed = this;
	gpfn_VSSR = RefreshCB;
	s_VssRecurse(BIG_Csz_Root, ul_Fat, 8);
	gpfn_VSSR = NULL;

	mpo_TreeView->SetRedraw(TRUE);

	M_MF()->UnLockBigFile();

	AfxGetApp()->DoWaitCursor(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HTREEITEM EPEN_cl_Frame::GetTreeItemFromlParam(LONG lParam)
{
	/*~~~~~~~~~~~~~~~*/
	UINT		uFlags;
	CPoint		pt;
	HTREEITEM	hItem;
	/*~~~~~~~~~~~~~~~*/

	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	hItem = mpo_TreeView->HitTest(pt, &uFlags);
	if((TVHT_ONITEM & uFlags) && (hItem != NULL)) return hItem;
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::OnParentNotify(UINT message, LONG lParam)
{
	switch(message)
	{
	case WM_RBUTTONDOWN:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			HTREEITEM	hItem = GetTreeItemFromlParam(lParam);
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(NULL != hItem)
			{
				/*~~~~~~~*/
				CPoint	pt;
				/*~~~~~~~*/

				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);

				mpo_TreeView->SelectItem(hItem);
				ClientToScreen(&pt);
				OnCtrlPopup(pt);
			}
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::OnCtrlPopup(CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	InitPopupMenuAction(&o_Menu);
	AddPopupMenuAction(&o_Menu, EPENC_ACTION_SHOWINBROWSER);
	AddPopupMenuAction(&o_Menu, 0);
	AddPopupMenuAction(&o_Menu, EPENC_ACTION_UNDOCHECKOUT_SELECTED);
	AddPopupMenuAction(&o_Menu, EPENC_ACTION_CHECKIN_SELECTED);
	AddPopupMenuAction(&o_Menu, EPENC_ACTION_CHECKINOUT_SELECTED);
	TrackPopupMenuAction(pt, &o_Menu);
}

/*$4
 ***********************************************************************************************************************
    COMMANDS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::OnClose(void)
{
}

/*$4
 ***********************************************************************************************************************
    INTERFACE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPEN_cl_Frame::b_CanActivate(void)
{
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::CloseProject(void)
{
}

/*$4
 ***********************************************************************************************************************
    INTERFACE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::On_TVN_Notify(NMHDR *pNotifyStruct, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	TVHITTESTINFO	ht = { 0 };
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pNotifyStruct->idFrom == 1000)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		DWORD	dwpos = GetMessagePos();
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ht.pt.x = GET_X_LPARAM(dwpos);
		ht.pt.y = GET_Y_LPARAM(dwpos);
		mpo_TreeView->ScreenToClient(&ht.pt);
		mpo_TreeView->HitTest(&ht);
	}

	switch(pNotifyStruct->code)
	{
	case NM_DBLCLK:
		/* on dbl click on a file, go to the file in the browser */
		if(TVHT_ONITEM & ht.flags)
		{
			/*~~~~~~~~~~~~~~~*/
			TVITEM	stItemData;
			/*~~~~~~~~~~~~~~~*/

			stItemData.mask = TVIF_HANDLE | TVIF_PARAM;
			stItemData.hItem = ht.hItem;
			mpo_TreeView->GetItem(&stItemData);

			if((stItemData.lParam != BIG_C_InvalidIndex) && !(stItemData.lParam & 0x80000000))
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				EBRO_cl_Frame	*po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(po_Browser)
				{
					po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(stItemData.lParam), stItemData.lParam);
				}
			}
		}

		*result = 0;
		break;

	default:
		*result = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::UndoCheckOutAll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	HTREEITEM	h_Item;
	BIG_INDEX	index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_TreeView->GetCount()) return;

	index = M_MF()->MessageBox("Undo Check-Out all files ?", "Confirm your action", MB_YESNO | MB_ICONQUESTION);

	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;
	EDI_gi_OnlyOneVssCheck = 1;
	EDI_gi_OnlyOneRefCheck = 1;

	AfxGetApp()->DoWaitCursor(1);

	if((index == IDYES) && M_MF()->b_LockBigFile())
	{
		EDI_can_lock = FALSE;

		h_Item = mpo_TreeView->GetNextItem(NULL, TVGN_CHILD);
		while(h_Item)
		{
			while(h_Item)
			{
				index = mpo_TreeView->GetItemData(h_Item);

				if(index & 0x80000000)	/* Dir */
				{
					BIG_ComputeFullName(index, asz_Path);
					M_MF()->UndoCheckOutDir(asz_Path);
				}
				else
				{
					BIG_ComputeFullName(BIG_ParentFile(index), asz_Path);
					M_MF()->UndoCheckOutFile(asz_Path, BIG_NameFile(index));
				}

				mpo_TreeView->DeleteItem(h_Item);

				h_Item = mpo_TreeView->GetNextItem(h_Item, TVGN_NEXT);
			}

			h_Item = mpo_TreeView->GetNextItem(NULL, TVGN_CHILD);
		}

		EDI_gb_NoUpdateVSS = TRUE;
		LINK_gb_RefreshOnlyFiles = TRUE;
		EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
	}

	AfxGetApp()->DoWaitCursor(-1);
	EDI_gb_NoUpdateVSS = FALSE;
	EDI_gi_OnlyOneVssCheck = 0;
	EDI_gi_OnlyOneRefCheck = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::CheckInAll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	HTREEITEM	h_Item;
	BIG_INDEX	index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_TreeView->GetCount()) return;

	index = M_MF()->MessageBox("Check-In all files ?", "Confirm your action", MB_YESNO | MB_ICONQUESTION);

	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;
	EDI_gi_OnlyOneVssCheck = 1;
	EDI_gi_OnlyOneRefCheck = 1;

	AfxGetApp()->DoWaitCursor(1);

	if((index == IDYES) && M_MF()->b_LockBigFile())
	{
		EDI_can_lock = FALSE;
		EDI_gb_CheckInDel = FALSE;

		h_Item = mpo_TreeView->GetNextItem(NULL, TVGN_CHILD);
		while(h_Item)
		{
			while(h_Item)
			{
				index = mpo_TreeView->GetItemData(h_Item);

				if(index & 0x80000000)	/* Dir */
				{
					BIG_ComputeFullName(index, asz_Path);
					M_MF()->CheckInDir(asz_Path);
				}
				else
				{
					BIG_ComputeFullName(BIG_ParentFile(index), asz_Path);
					M_MF()->CheckInFile(asz_Path, BIG_NameFile(index));
				}

				mpo_TreeView->DeleteItem(h_Item);

				h_Item = mpo_TreeView->GetNextItem(h_Item, TVGN_NEXT);
			}

			h_Item = mpo_TreeView->GetNextItem(NULL, TVGN_CHILD);
		}

        
        EDI_gi_OnlyOneVssCheck = 0;
        EDI_gi_OnlyOneRefCheck = 0;
        M_MF()->REFFileChanged(FALSE, TRUE);


		EDI_gb_NoUpdateVSS = TRUE;
		LINK_gb_RefreshOnlyFiles = TRUE;
		EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
	}

	AfxGetApp()->DoWaitCursor(-1);
	EDI_gb_NoUpdateVSS = FALSE;
	EDI_gi_OnlyOneVssCheck = 0;
	EDI_gi_OnlyOneRefCheck = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::UndoCheckOutSel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	HTREEITEM	h_Item;
	BIG_INDEX	index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_TreeView->GetCount()) return;

	h_Item = mpo_TreeView->GetSelectedItem();
	if(!h_Item) return;

	index = mpo_TreeView->GetItemData(h_Item);
	if(index == BIG_C_InvalidIndex) return;

	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;
	EDI_gi_OnlyOneVssCheck = 1;
	EDI_gi_OnlyOneRefCheck = 1;

	AfxGetApp()->DoWaitCursor(1);
	if(M_MF()->b_LockBigFile())
	{
		EDI_can_lock = FALSE;

		if(index & 0x80000000)	/* Dir */
		{
			BIG_ComputeFullName(index, asz_Path);
			M_MF()->UndoCheckOutDir(asz_Path);
		}
		else
		{
			BIG_ComputeFullName(BIG_ParentFile(index), asz_Path);
			M_MF()->UndoCheckOutFile(asz_Path, BIG_NameFile(index));
		}

		mpo_TreeView->DeleteItem(h_Item);

		EDI_gb_NoUpdateVSS = TRUE;
		LINK_gb_RefreshOnlyFiles = TRUE;
		EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
	}

	AfxGetApp()->DoWaitCursor(-1);

	EDI_gb_NoUpdateVSS = FALSE;
	EDI_gi_OnlyOneVssCheck = 0;
	EDI_gi_OnlyOneRefCheck = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::CheckInSel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	HTREEITEM	h_Item;
	BIG_INDEX	index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_TreeView->GetCount()) return;

	h_Item = mpo_TreeView->GetSelectedItem();
	if(!h_Item) return;

	index = mpo_TreeView->GetItemData(h_Item);
	if(index == BIG_C_InvalidIndex) return;

	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;
	EDI_gi_OnlyOneVssCheck = 1;
	EDI_gi_OnlyOneRefCheck = 1;

	AfxGetApp()->DoWaitCursor(1);

	if(M_MF()->b_LockBigFile())
	{
		EDI_can_lock = FALSE;
		EDI_gb_CheckInDel = FALSE;

		if(index & 0x80000000)	/* Dir */
		{
			BIG_ComputeFullName(index, asz_Path);
			M_MF()->CheckInDir(asz_Path);
		}
		else
		{
			BIG_ComputeFullName(BIG_ParentFile(index), asz_Path);
			M_MF()->CheckInFile(asz_Path, BIG_NameFile(index));
		}

		mpo_TreeView->DeleteItem(h_Item);

		EDI_gb_NoUpdateVSS = TRUE;
		LINK_gb_RefreshOnlyFiles = TRUE;
		EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
	}

	AfxGetApp()->DoWaitCursor(-1);
	EDI_gb_NoUpdateVSS = FALSE;
	EDI_gi_OnlyOneVssCheck = 0;
	EDI_gi_OnlyOneRefCheck = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::CheckInOutSel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	HTREEITEM	h_Item;
	BIG_INDEX	index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_TreeView->GetCount()) return;

	h_Item = mpo_TreeView->GetSelectedItem();
	if(!h_Item) return;

	index = mpo_TreeView->GetItemData(h_Item);
	if(index == BIG_C_InvalidIndex) return;

	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;

	LINK_gb_EscapeDetected = FALSE;
	EDI_gi_OnlyOneVssCheck = 1;
	EDI_gi_OnlyOneRefCheck = 1;
	AfxGetApp()->DoWaitCursor(1);

	if(M_MF()->b_LockBigFile())
	{
		EDI_can_lock = FALSE;
		EDI_gb_CheckInDel = FALSE;

		if(index & 0x80000000)	/* Dir */
		{
			BIG_ComputeFullName(index, asz_Path);
			M_MF()->CheckInDirNoOut(asz_Path);
		}
		else
		{
			BIG_ComputeFullName(BIG_ParentFile(index), asz_Path);
			M_MF()->CheckInFileNoOut(asz_Path, BIG_NameFile(index));
		}

		EDI_gb_NoUpdateVSS = TRUE;
		LINK_gb_RefreshOnlyFiles = TRUE;
		EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
	}

	AfxGetApp()->DoWaitCursor(-1);
	EDI_gb_NoUpdateVSS = FALSE;
	EDI_gi_OnlyOneVssCheck = 0;
	EDI_gi_OnlyOneRefCheck = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPEN_cl_Frame::ShowInBrowerSel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM		h_Item;
	BIG_INDEX		index;
	EBRO_cl_Frame	*po_Browser;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_TreeView->GetCount()) return;

	h_Item = mpo_TreeView->GetSelectedItem();
	if(!h_Item) return;

	index = mpo_TreeView->GetItemData(h_Item);
	if(index == BIG_C_InvalidIndex) return;

	po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
	if(index & 0x80000000)
		po_Browser->i_OnMessage(EDI_MESSAGE_SELDIR, index, 0);
	else
		po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(index), index);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
