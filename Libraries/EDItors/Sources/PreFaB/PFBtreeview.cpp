/*$T PFBtreeview.cpp GC! 1.081 03/09/04 10:44:48 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    EXTERNAL.
 -----------------------------------------------------------------------------------------------------------------------
 */

extern BOOL						EDI_gb_CheckInDel;

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- base -----------------------------------------------------------------------------------------------------------*/

#include "BASe/BAStypes.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/SAVing/SAVdefs.h"

/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#include "Res/Res.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDItors/Sources/BROwser/BROstrings.h"
#include "EDItors/Sources/BROwser/BROerrid.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDIicons.h"
#include "EDImsg.h"
#include "EDIstrings.h"
#include "SELection/SELection.h"
#include "DIAlogs/DIAname_dlg.h"

/*$2- Prefab editor --------------------------------------------------------------------------------------------------*/

#include "PFBframe.h"
#include "PFBtreeView.h"

#include "PERForce\PERmsg.h"
#include "LINks\LINKtoed.h"

#include "EDItors/Sources/PERForce/PERCDataTreeCtrl.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    message map
 ***********************************************************************************************************************
 */

IMPLEMENT_DYNCREATE(EPFB_cl_TreeView, CFormView)
BEGIN_MESSAGE_MAP(EPFB_cl_TreeView, CFormView)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PREFAB, OnPrefabTreeSelChanged)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    extern
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    menbers
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EPFB_cl_TreeView::EPFB_cl_TreeView(EPFB_cl_Frame *_po_Editor) :
	CFormView(EPFB_IDD_TREEVIEW)
{
	mpo_Editor = _po_Editor;
	mh_DDItem = NULL;
	mul_RootDir = BIG_C_InvalidIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EPFB_cl_TreeView::EPFB_cl_TreeView(void) :
	CFormView(EPFB_IDD_TREEVIEW)
{
	EPFB_cl_TreeView(NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EPFB_cl_TreeView::~EPFB_cl_TreeView(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPFB_cl_TreeView::Create
(
	LPCTSTR		lpszClassName,
	LPCTSTR		lpszWindowName,
	DWORD		dwStyle,
	const RECT	&rect,
	CWnd		*pParentWnd,
	UINT		nID
)
{
	/*~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	/*~~~~~~~~~~~~~~~~~*/

	CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, NULL);

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
	po_Tree->SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);
	po_Tree->SetFont(&M_MF()->mo_Fnt);

	return TRUE;
}

/*$4
 ***********************************************************************************************************************
    Message functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPFB_cl_TreeView::PreTranslateMessage(MSG *pMsg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl		*po_Tree;
	HTREEITEM		hItem;
	UINT			uFlags;
	CPoint			o_Pt;
	POINT			pt;
	int				i_Res, i_Img, i_SelImg;
	EBRO_cl_Frame	*po_Browser;
	ULONG			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pMsg->hwnd == GetDlgItem(IDC_TREE_PREFAB)->GetSafeHwnd())
	{
		po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
		o_Pt.x = LOWORD(pMsg->lParam);
		o_Pt.y = HIWORD(pMsg->lParam);

		if(pMsg->message == WM_LBUTTONDOWN)
		{
			/* Select the item that is at the point myPoint. */
			hItem = po_Tree->HitTest(o_Pt, &uFlags);
			if((hItem != NULL) && (TVHT_ONITEM & uFlags))
			{
				/* po_Tree->Select(hItem, TVGN_CARET); */
				mh_DDItem = hItem;
				mo_DDPt = o_Pt;
			}
		}

		if(pMsg->message == WM_LBUTTONUP)
		{
			/* Select the item that is at the point myPoint. */
			hItem = po_Tree->HitTest(o_Pt, &uFlags);
			if((hItem != NULL) && (TVHT_ONITEM & uFlags)) po_Tree->Select(hItem, TVGN_CARET);
		}
		else if(pMsg->message == WM_MOUSEMOVE)
		{
			if((pMsg->wParam & MK_LBUTTON) && (mh_DDItem))
			{
				if((abs(mo_DDPt.x - LOWORD(pMsg->lParam)) > 4) || (abs(mo_DDPt.y - HIWORD(pMsg->lParam)) > 4))
				{
					po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
					EDI_gst_DragDrop.ul_FatDir = BIG_C_InvalidIndex;
					EDI_gst_DragDrop.ul_FatFile = BIG_C_InvalidIndex;
					EDI_gst_DragDrop.i_Param1 = 0;
					EDI_gst_DragDrop.i_Param2 = (int) mh_DDItem;
					EDI_gst_DragDrop.i_Param3 = po_Tree->GetItemData(mh_DDItem);
					M_MF()->BeginDragDrop(o_Pt, this, mpo_Editor, EDI_DD_User);
				}
			}
		}
		else if(pMsg->message == WM_RBUTTONDOWN)
		{
			EMEN_cl_SubMenu o_Menu(FALSE);
			
			po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);

			hItem = po_Tree->HitTest(o_Pt, &uFlags);
			M_MF()->InitPopupMenuAction(NULL, &o_Menu);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 6, TRUE, EPFB_asz_String[EPFB_STRING_MenuShowInBrowser], -1);

			if((hItem != NULL) && (TVHT_ONITEM & uFlags))
			{
				po_Tree->Select(hItem, TVGN_CARET);
				po_Tree->GetItemImage(hItem, i_Img, i_SelImg);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, EPFB_asz_String[EPFB_STRING_TreeMenuRename], -1);
				M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, EPFB_asz_String[EPFB_STRING_TreeMenuRemove], -1);
				if(i_Img == EDI_IMAGE_FOLDER1)
				{
					/*$off*/
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, 3, TRUE, EPFB_asz_String[EPFB_STRING_TreeMenuCreateDir], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, 4, TRUE, EPFB_asz_String[EPFB_STRING_TreeMenuCreateEmptyPrefab], -1 );
					/*$on*/
				}
			}

			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 5, TRUE, EPFB_asz_String[EPFB_STRING_TreeMenuCreateRootDir], -1);
			
			if((hItem != NULL) && (TVHT_ONITEM & uFlags))
			{
				if( po_Browser && !po_Browser->mst_Ini.b_EngineMode && M_MF()->mst_Ini.b_LinkControlON )
				{
					/*$off*/
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, 0, TRUE, "Data Control" );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuCheckout, TRUE, EPFB_asz_String[EPFB_STRING_MenuCheckout], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuLocalCheckout, TRUE, EPFB_asz_String[EPFB_STRING_MenuLocalCheckout], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuUndoCheckout, TRUE, EPFB_asz_String[EPFB_STRING_MenuUndoCheckout], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuGetLVersion, TRUE, EPFB_asz_String[EPFB_STRING_MenuGetLVersion], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuCheckIn, TRUE, EPFB_asz_String[EPFB_STRING_MenuCheckIn], -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuCheckInOut, TRUE, EPFB_asz_String[EPFB_STRING_MenuCheckInOut], -1 );
					if(i_Img == EDI_IMAGE_FOLDER1)
						M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_MenuCheckInMirror, TRUE, EPFB_asz_String[EPFB_STRING_MenuCheckInMirror], -1 );
					/*$on*/
				}
				else
				{
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, 0, TRUE, "Data Control" );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_P4MenuSubmit				,TRUE,EPFB_asz_String[EPFB_STRING_P4MenuSubmit			],-1);
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_P4MenuCheckout			,TRUE,EPFB_asz_String[EPFB_STRING_P4MenuCheckout			],-1);
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_P4MenuSubmitEdit			,TRUE,EPFB_asz_String[EPFB_STRING_P4MenuSubmitEdit			],-1);
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_P4MenuUndoCheckout		,TRUE,EPFB_asz_String[EPFB_STRING_P4MenuUndoCheckout		],-1);
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_P4MenuGetLVersion		,TRUE,EPFB_asz_String[EPFB_STRING_P4MenuGetLVersion		],-1);
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_P4MenuForceGetLVersion	,TRUE,EPFB_asz_String[EPFB_STRING_P4MenuForceGetLVersion	],-1);
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_P4MenuAdd				,TRUE,EPFB_asz_String[EPFB_STRING_P4MenuAdd				],-1);
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_P4MenuDelete				,TRUE,EPFB_asz_String[EPFB_STRING_P4MenuDelete			],-1);
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_P4MenuDiff				,TRUE,EPFB_asz_String[EPFB_STRING_P4MenuDiff				],-1);
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, EPFB_STRING_P4MenuHistory			,TRUE,EPFB_asz_String[EPFB_STRING_P4MenuHistory			],-1);
				}
			}
			
			

			GetCursorPos(&pt);
			i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
			switch(i_Res)
			{
			case 1:
				Rename(hItem);
				break;
			case 2:
				Remove(hItem);
				break;
			case 3:
				CreateDir(hItem);
				break;
			case 4:
				CreateEmptyPrefab(hItem);
				break;
			case 5:
				CreateDir(TVI_ROOT);
				break;
			case 6:
				ul_Index = po_Tree->GetItemData(hItem);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					if(i_Img == EDI_IMAGE_FOLDER1)
						po_Browser->i_OnMessage(EDI_MESSAGE_SELDIR, ul_Index, 0);
					else
						po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ul_Index), ul_Index);
				}
				break;
			case EPFB_STRING_MenuCheckout:
				mpo_Editor->CommonVss(2);
				break;
			case EPFB_STRING_MenuLocalCheckout:
				mpo_Editor->CommonVss(6);
				break;
			case EPFB_STRING_MenuUndoCheckout:
				mpo_Editor->CommonVss(3);
				break;
			case EPFB_STRING_MenuGetLVersion:
				mpo_Editor->CommonVss(4);
				break;
			case EPFB_STRING_MenuCheckIn:
					EDI_gb_CheckInDel = FALSE;
				mpo_Editor->CommonVss(1);
				break;
			case EPFB_STRING_MenuCheckInOut:
				EDI_gb_CheckInDel = FALSE;
				mpo_Editor->CommonVss(5);
				break;
			case EPFB_STRING_MenuCheckInMirror:
				EDI_gb_CheckInDel = TRUE;
				mpo_Editor->CommonVss(1);
				break;

			case EPFB_STRING_P4MenuCheckout:
				{
					PER_CDataTreeCtrl PerforceDataTree ( po_Tree );
					LINK_SendMessageToEditors(EPER_MESSAGE_EDIT_SELECTED, ((ULONG)&PerforceDataTree),0);
				}
				break;
			case EPFB_STRING_P4MenuUndoCheckout:
				{
					PER_CDataTreeCtrl PerforceDataTree ( po_Tree );
					LINK_SendMessageToEditors(EPER_MESSAGE_REVERT_SELECTED, ((ULONG)&PerforceDataTree),0);
				}
				break;
			case EPFB_STRING_P4MenuGetLVersion		:
				{
					PER_CDataTreeCtrl PerforceDataTree ( po_Tree );
					LINK_SendMessageToEditors(EPER_MESSAGE_SYNC_SELECTED, ((ULONG)&PerforceDataTree),0);
				}
				break;
			case EPFB_STRING_P4MenuForceGetLVersion	:
				{
					PER_CDataTreeCtrl PerforceDataTree ( po_Tree );
					LINK_SendMessageToEditors(EPER_MESSAGE_FORCESYNC_SELECTED, ((ULONG)&PerforceDataTree),0);
				}
				break;
			case EPFB_STRING_P4MenuAdd		:
				{
					PER_CDataTreeCtrl PerforceDataTree ( po_Tree );
					LINK_SendMessageToEditors(EPER_MESSAGE_ADD_SELECTED, ((ULONG)&PerforceDataTree),0);
				}
				break;
			case EPFB_STRING_P4MenuDelete		:
				{
					PER_CDataTreeCtrl PerforceDataTree ( po_Tree );
					LINK_SendMessageToEditors(EPER_MESSAGE_DELETE_SELECTED, ((ULONG)&PerforceDataTree),0);
				}
				break;
			case EPFB_STRING_P4MenuSubmit		:
				{
					PER_CDataTreeCtrl PerforceDataTree ( po_Tree );
					LINK_SendMessageToEditors(EPER_MESSAGE_SUBMIT_SELECTED, ((ULONG)&PerforceDataTree),0);
				}
				break;
			case EPFB_STRING_P4MenuSubmitEdit	:
				{
					PER_CDataTreeCtrl PerforceDataTree ( po_Tree );
					LINK_SendMessageToEditors(EPER_MESSAGE_SUBMITEDIT_SELECTED, ((ULONG)&PerforceDataTree),0);
				}
				break;
			case EPFB_STRING_P4MenuDiff		:
				{
					PER_CDataTreeCtrl PerforceDataTree ( po_Tree );
					LINK_SendMessageToEditors(EPER_MESSAGE_DIFF_SELECTED, ((ULONG)&PerforceDataTree),0);
				}
				break;
			case EPFB_STRING_P4MenuHistory:
				{
					PER_CDataTreeCtrl PerforceDataTree ( po_Tree );
					LINK_SendMessageToEditors(EPER_MESSAGE_SHOWHISTORY_SELECTED, ((ULONG)&PerforceDataTree),0);
				}
				break;
			}

			return TRUE;
		}
		else if(pMsg->message == WM_KEYDOWN)
		{
			switch(pMsg->wParam)
			{
			case 'P':
				return TRUE;
			}
		}
	}
	else if(pMsg->message == WM_LBUTTONUP)
	{
		if(EDI_gst_DragDrop.b_BeginDragDrop)
		{
			po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
			po_Tree->SelectDropTarget(NULL);
			o_Pt.x = LOWORD(pMsg->lParam);
			o_Pt.y = HIWORD(pMsg->lParam);
			M_MF()->EndDragDrop(o_Pt);
			mh_DDItem = NULL;
		}
	}
	else if(pMsg->message == WM_MOUSEMOVE)
	{
		/* D & d mode */
		if(EDI_gst_DragDrop.b_BeginDragDrop)
		{
			o_Pt.x = LOWORD(pMsg->lParam);
			o_Pt.y = HIWORD(pMsg->lParam);
			if(pMsg->wParam & MK_LBUTTON) M_MF()->b_MoveDragDrop(o_Pt);
		}
	}
	else if(pMsg->wParam == WM_RBUTTONDOWN)
	{
		if(EDI_gst_DragDrop.b_BeginDragDrop)
		{
			po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
			po_Tree->SelectDropTarget(NULL);
			M_MF()->CancelDragDrop();
			mh_DDItem = NULL;
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
    resize view
 =======================================================================================================================
 */
void EPFB_cl_TreeView::OnSize(UINT nType, int cx, int cy)
{
	/*~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	/*~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
	if(po_Tree) po_Tree->MoveWindow(5, 5, cx - 10, cy - 10);
	CFormView::OnSize(nType, cx, cy);
}

/*
 =======================================================================================================================
    Cur prefab dir or file change
 =======================================================================================================================
 */
void EPFB_cl_TreeView::OnPrefabTreeSelChanged(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	NM_TREEVIEW *pNotifyStruct;
	int			i_Data, i_Image, i_Texture;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pNotifyStruct = (NM_TREEVIEW *) pNotifyStruct2;

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
	i_Data = po_Tree->GetItemData(pNotifyStruct->itemNew.hItem);
	if(i_Data == 0) return;

	po_Tree->GetItemImage(pNotifyStruct->itemNew.hItem, i_Image, i_Texture);

	if(i_Image == EDI_IMAGE_FOLDER1)
	{
		mpo_Editor->mb_SelIsDir = TRUE;
		mpo_Editor->mul_DirIndex = po_Tree->GetItemData(pNotifyStruct->itemNew.hItem);
		mpo_Editor->Tree_SelChange();
	}
	else if(i_Image == EDI_IMAGE_GRP)
	{
		mpo_Editor->mb_SelIsDir = FALSE;
		mpo_Editor->mul_FileIndex = po_Tree->GetItemData(pNotifyStruct->itemNew.hItem);
		mpo_Editor->Tree_SelChange();
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Fill Tree
 =======================================================================================================================
 */
void EPFB_cl_TreeView::Fill(ULONG _ul_Dir, HTREEITEM _h_Parent)
{
	/*~~~~~~~~~~~~~~~~~*/
	int			I1;
	HTREEITEM	h;
	CTreeCtrl	*po_Tree;
	/*~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);

	if(_h_Parent == TVI_ROOT)
	{
		mul_RootDir = _ul_Dir;
		po_Tree->SetRedraw(FALSE);
		po_Tree->DeleteAllItems();
	}

	/* get dir */
	I1 = BIG_SubDir(_ul_Dir);
	while(I1 != BIG_C_InvalidIndex)
	{
		h = po_Tree->InsertItem(BIG_NameDir(I1), EDI_IMAGE_FOLDER1, EDI_IMAGE_FOLDER1, _h_Parent);
		po_Tree->SetItemData(h, I1);
		Fill(I1, h);
		I1 = BIG_NextDir(I1);
	}

	/* get file */
	if(_h_Parent != TVI_ROOT)
	{
		I1 = BIG_FirstFile(_ul_Dir);
		if(I1 != BIG_C_InvalidIndex && BIG_PosFile(I1) == 0xcdcdcdcd)
		{
			I1 = BIG_C_InvalidIndex;
			BIG_FirstFile(_ul_Dir) = BIG_C_InvalidIndex;
		}

		while(I1 != BIG_C_InvalidIndex)
		{
			if(BIG_b_IsFileExtension(I1, EDI_Csz_ExtPrefab))
			{
				h = po_Tree->InsertItem(BIG_NameFile(I1), EDI_IMAGE_GRP, EDI_IMAGE_GRP, _h_Parent);
				po_Tree->SetItemData(h, I1);
			}

			I1 = BIG_NextFile(I1);
		}
	}

	po_Tree->SortChildren(_h_Parent);

	if(_h_Parent == TVI_ROOT)
	{
		po_Tree->SetRedraw(TRUE);
		po_Tree->UpdateWindow();
		po_Tree->Invalidate();
	}
}

/*
 =======================================================================================================================
    Refresh Tree
 =======================================================================================================================
 */
void EPFB_cl_TreeView::Refresh(void)
{
	if(mul_RootDir != BIG_C_InvalidIndex) Fill(mul_RootDir);
	mh_DDItem = NULL;
}

/*
 =======================================================================================================================
    Rename a tree item
 =======================================================================================================================
 */
void EPFB_cl_TreeView::Rename(HTREEITEM _h_Item)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EBRO_STR_Csz_TitleEnterNewName);
	CTreeCtrl			*po_Tree;
	BIG_INDEX			ul_Index;
	int					i_Img, i_SelImg;
	char				*psz_Ext, *psz_NewName, sz_Name[260], sz_NewName[260], sz_Path[260];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
	ul_Index = po_Tree->GetItemData(_h_Item);
	po_Tree->GetItemImage(_h_Item, i_Img, i_SelImg);

	/* dir */
	if(i_Img == EDI_IMAGE_FOLDER1) strcpy(sz_Name, BIG_NameDir(ul_Index));

	/* file */
	else
	{
		strcpy(sz_Name, BIG_NameFile(ul_Index));
		if(psz_Ext = L_strchr(sz_Name, '.')) *psz_Ext = 0;
	}

	/* changing name */
	o_Dialog.mo_Name = sz_Name;
	if(o_Dialog.DoModal() == IDOK)
	{
		psz_NewName = (char *) (LPCSTR) o_Dialog.mo_Name;
		if(psz_Ext = L_strchr(psz_NewName, '.')) *psz_Ext = 0;

		/* check name */
		if(BIG_b_CheckName(psz_NewName) == FALSE) return;

		/* same name as before : do nothing */
		if(L_strcmp(sz_Name, psz_NewName) == 0) return;

		if(i_Img == EDI_IMAGE_FOLDER1)
		{
			BIG_ComputeFullName(ul_Index, sz_Name);
			L_strcpy(sz_NewName, sz_Name);
			*(L_strrchr(sz_NewName, '/') + 1) = 0;
			L_strcat(sz_NewName, psz_NewName);

			if(BIG_ul_SearchDir(sz_NewName) != BIG_C_InvalidIndex)
			{
				ERR_X_Error(0, EBRO_ERR_Csz_NameAlreadyExists, NULL);
				return;
			}

			BIG_RenDir(sz_NewName, sz_Name);
			po_Tree->SetItemText(_h_Item, psz_NewName);
		}
		else
		{
			L_strcpy(sz_Name, BIG_NameFile(ul_Index));
			L_strcpy(sz_NewName, psz_NewName);
			L_strcat(sz_NewName, EDI_Csz_ExtPrefab);
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), sz_Path);

			if(BIG_ul_SearchFile(BIG_ParentFile(ul_Index), sz_NewName) != BIG_C_InvalidIndex)
			{
				ERR_X_Error(0, EBRO_ERR_Csz_NameAlreadyExists, NULL);
				return;
			}

			BIG_RenFile(sz_NewName, sz_Path, sz_Name);
			po_Tree->SetItemText(_h_Item, sz_NewName);
		}

		po_Tree->SortChildren(po_Tree->GetParentItem(_h_Item));
	}
}

/*
 =======================================================================================================================
    Rename a tree item
 =======================================================================================================================
 */
void EPFB_cl_TreeView::Remove(HTREEITEM _h_Item)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	BIG_INDEX	ul_Index;
	int			i_Img, i_SelImg;
	char		asz_Message[512], *psz_Name;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
	ul_Index = po_Tree->GetItemData(_h_Item);
	po_Tree->GetItemImage(_h_Item, i_Img, i_SelImg);

	AfxGetApp()->DoWaitCursor(1);

_Try_

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Request to delete a directory.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(i_Img == EDI_IMAGE_FOLDER1)
	{
		L_strcpy(asz_Message, EBRO_STR_Csz_ConfirmDeleteFolder);
		L_strcat(asz_Message, BIG_NameDir(ul_Index));
		L_strcat(asz_Message, "\" ?");
		if(M_MF()->MessageBox(asz_Message, EDI_STR_Csz_TitleConfirm, MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			BIG_ComputeFullName(ul_Index, asz_Message);
			BIG_DelDir(asz_Message);
			po_Tree->DeleteItem(_h_Item);
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Request to delete a shortcut in a group.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else if(i_Img == EDI_IMAGE_GRP)
	{
		L_strcpy(asz_Message, EBRO_STR_Csz_ConfirmDeleteFile);
		L_strcat(asz_Message, BIG_NameFile(ul_Index));
		L_strcat(asz_Message, "\" ?");

		if(M_MF()->MessageBox(asz_Message, EDI_STR_Csz_TitleConfirm, MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Message);
			psz_Name = BIG_NameFile(ul_Index);
			BIG_DelFile(asz_Message, psz_Name);
			po_Tree->DeleteItem(_h_Item);
		}
	}

_Catch_
_End_
	M_MF()->FatHasChanged();
	AfxGetApp()->DoWaitCursor(-1);
}

/*
 =======================================================================================================================
    Add a prefab in a directory
 =======================================================================================================================
 */
void EPFB_cl_TreeView::CreateDir(HTREEITEM _h_Item)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EBRO_STR_Csz_TitleEnterFileName);
	ULONG				ul_Index, ul_New;
	CTreeCtrl			*po_Tree;
	char				sz_Name[260];
	HTREEITEM			hNew;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_Dialog.DoModal() != IDOK) return;

	if(BIG_b_CheckName((char *) (LPCTSTR) o_Dialog.mo_Name) == FALSE) return;

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);

	if(_h_Item == TVI_ROOT)
		ul_Index = BIG_ul_SearchDir(EDI_Csz_Path_ObjModels);
	else
		ul_Index = po_Tree->GetItemData(_h_Item);

	BIG_ComputeFullName(ul_Index, sz_Name);
	L_strcat(sz_Name, "/");
	L_strcat(sz_Name, (char *) (LPCTSTR) o_Dialog.mo_Name);

	if( BIG_ul_SearchDir( sz_Name ) != BIG_C_InvalidIndex)
	{
		ERR_X_Error(0, EBRO_ERR_Csz_NameAlreadyExists, NULL);
		return;
	}

	ul_New = BIG_ul_CreateDir(sz_Name);

	hNew = po_Tree->InsertItem(BIG_NameDir(ul_New), EDI_IMAGE_FOLDER1, EDI_IMAGE_FOLDER1, _h_Item);
	po_Tree->SetItemData(hNew, ul_New);
	po_Tree->SortChildren(_h_Item);

	SelDir(ul_New);
}

/*
 =======================================================================================================================
    Add a prefab in a directory
 =======================================================================================================================
 */
void EPFB_cl_TreeView::CreateEmptyPrefab(HTREEITEM _h_Item)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EBRO_STR_Csz_TitleEnterFolderName);
	ULONG				ul_Index, ul_New;
	CTreeCtrl			*po_Tree;
	char				*psz_Ext, sz_Name[260];
	OBJ_tdst_Prefab		st_Prefab;
	HTREEITEM			hNew;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_Dialog.DoModal() != IDOK) return;

	if(BIG_b_CheckName((char *) (LPCTSTR) o_Dialog.mo_Name) == FALSE) return;

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
	ul_Index = po_Tree->GetItemData(_h_Item);

	L_strcpy(sz_Name, (char *) (LPCTSTR) o_Dialog.mo_Name);
	if(psz_Ext = L_strchr(sz_Name, '.')) *psz_Ext = 0;
	L_strcat(sz_Name, EDI_Csz_ExtPrefab);

	if(BIG_ul_SearchFile(ul_Index, sz_Name) != BIG_C_InvalidIndex)
	{
		ERR_X_Error(0, EBRO_ERR_Csz_NameAlreadyExists, NULL);
		return;
	}

	L_memset(&st_Prefab, 0, sizeof(OBJ_tdst_Prefab));
	ul_New = Prefab_ul_SaveExt(ul_Index, sz_Name, &st_Prefab);

	if(ul_New == BIG_C_InvalidIndex) return;

	hNew = po_Tree->InsertItem(BIG_NameFile(ul_New), EDI_IMAGE_GRP, EDI_IMAGE_GRP, _h_Item);
	po_Tree->SetItemData(hNew, ul_New);
	po_Tree->SortChildren(_h_Item);

	SelFile(ul_New);
}

/*
 =======================================================================================================================
    Add a prefab in a directory
 =======================================================================================================================
 */
BOOL EPFB_cl_TreeView::AddPrefab(ULONG _ul_Dir, ULONG _ul_Prefab, BOOL _b_Sel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	HTREEITEM	h_Dir, h_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	h_Dir = FindDir(_ul_Dir);
	if(h_Dir == 0) return FALSE;

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);

	h_Item = po_Tree->InsertItem(BIG_NameFile(_ul_Prefab), EDI_IMAGE_GRP, EDI_IMAGE_GRP, h_Dir);
	po_Tree->SetItemData(h_Item, _ul_Prefab);
	po_Tree->SortChildren(h_Dir);

	if(_b_Sel)
	{
		po_Tree->Select(h_Item, TVGN_CARET);
		mpo_Editor->mb_SelIsDir = FALSE;
		mpo_Editor->mul_DirIndex = _ul_Dir;
		mpo_Editor->mul_FileIndex = _ul_Prefab;
		mpo_Editor->Tree_SelChange();
	}

	return TRUE;
}

/*
 =======================================================================================================================
    Select a directory
 =======================================================================================================================
 */
BOOL EPFB_cl_TreeView::SelDir(ULONG _ul_Dir)
{
	/*~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	HTREEITEM	h_Item;
	/*~~~~~~~~~~~~~~~~~*/

	h_Item = FindDir(_ul_Dir);
	if(h_Item == 0) return FALSE;

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
	po_Tree->Select(h_Item, TVGN_CARET);
	mpo_Editor->mb_SelIsDir = TRUE;
	mpo_Editor->mul_DirIndex = _ul_Dir;
	mpo_Editor->Tree_SelChange();
	return TRUE;
}

/*
 =======================================================================================================================
    Select a directory
 =======================================================================================================================
 */
HTREEITEM EPFB_cl_TreeView::FindDir(ULONG _ul_Dir, HTREEITEM _h_Parent)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i_Img, i_SelImg;
	CTreeCtrl	*po_Tree;
	HTREEITEM	h_Item, h_Ret;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);

	h_Item = po_Tree->GetChildItem(_h_Parent);
	while(h_Item != NULL)
	{
		po_Tree->GetItemImage(h_Item, i_Img, i_SelImg);
		if(i_Img == EDI_IMAGE_FOLDER1)
		{
			if(po_Tree->GetItemData(h_Item) == _ul_Dir) return h_Item;
			if((h_Ret = FindDir(_ul_Dir, h_Item)) != 0) return h_Ret;
		}

		h_Item = po_Tree->GetNextItem(h_Item, TVGN_NEXT);
	}

	return 0;
}

/*
 =======================================================================================================================
    Select a directory
 =======================================================================================================================
 */
BOOL EPFB_cl_TreeView::SelFile(ULONG _ul_File)
{
	/*~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	HTREEITEM	h_Item;
	ULONG		ul_Dir;
	/*~~~~~~~~~~~~~~~~~*/

	h_Item = FindFile(_ul_File, &ul_Dir);
	if(h_Item == 0) return FALSE;

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);
	po_Tree->Select(h_Item, TVGN_CARET);
	mpo_Editor->mb_SelIsDir = FALSE;
	mpo_Editor->mul_DirIndex = ul_Dir;
	mpo_Editor->mul_FileIndex = _ul_File;
	mpo_Editor->Tree_SelChange();
	return TRUE;
}

/*
 =======================================================================================================================
    Select a directory
 =======================================================================================================================
 */
HTREEITEM EPFB_cl_TreeView::FindFile(ULONG _ul_File, ULONG *_pul_Dir, HTREEITEM _h_Parent)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i_Img, i_SelImg;
	CTreeCtrl	*po_Tree;
	HTREEITEM	h_Item, h_Ret;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREFAB);

	h_Item = po_Tree->GetChildItem(_h_Parent);
	while(h_Item != NULL)
	{
		po_Tree->GetItemImage(h_Item, i_Img, i_SelImg);
		if(i_Img == EDI_IMAGE_GRP)
		{
			if(po_Tree->GetItemData(h_Item) == _ul_File)
			{
				if(_pul_Dir) *_pul_Dir = po_Tree->GetItemData(_h_Parent);
				return h_Item;
			}
		}
		else if(i_Img == EDI_IMAGE_FOLDER1)
		{
			if((h_Ret = FindFile(_ul_File, _pul_Dir, h_Item)) != 0) return h_Ret;
		}

		h_Item = po_Tree->GetNextItem(h_Item, TVGN_NEXT);
	}

	return 0;
}

#endif /* ACTIVE_EDITORS */
