/*$T BROframe.cpp GC! 1.081 04/04/03 11:03:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "EDIapp.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/LOAding/LOAdefs.h"

#define ACTION_GLOBAL

#include "BROframe_act.h"
#include "BROframe.h"
#include "BROtreectrl.h"
#include "BROlistctrl.h"
#include "BROgrpctrl.h"
#include "BROmsg.h"
#include "BROstrings.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "ENGine/Sources/ENGinit.h"
#include "EDIpaths.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "AIinterp/Sources/AIdebug.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"

#include "BIGFiles/IMPort/IMPbase.h"
#include "DIAlogs/DIAname_dlg.h"

#ifdef JADEFUSION
extern IMP_tdst_ImportDialog	IMP_gst_ImportDialog_Interface;
extern int						ANI_gi_ImportMode;
#else
extern "C"	IMP_tdst_ImportDialog	IMP_gst_ImportDialog_Interface;
extern "C"	int						ANI_gi_ImportMode;
#endif
/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

CMapPtrToPtr	mo_FileIcons;

char			*EBRO_gasz_ListColNames[EBRO_C_ListCtrlMaxColumns] =
{
	EBRO_STR_Csz_ListCol1,
	EBRO_STR_Csz_ListCol2,
	EBRO_STR_Csz_ListCol3,
	EBRO_STR_Csz_ListCol4,
	EBRO_STR_Csz_ListCol5,
	EBRO_STR_Csz_ListCol6,
	EBRO_STR_Csz_ListCol7,
	EBRO_STR_Csz_ListCol8,
	EBRO_STR_Csz_ListCol9,
};

char			*EBRO_gasz_GrpColNames[EBRO_C_GrpCtrlMaxColumns] =
{
	EBRO_STR_Csz_ListCol1,
	EBRO_STR_Csz_ListCol2,
	EBRO_STR_Csz_GrpCol3,
	EBRO_STR_Csz_GrpCol4,
};

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EBRO_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EBRO_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND_RANGE(WM_USER + 101, WM_USER + 200, OnViewColumnListCtrl)
	ON_COMMAND_RANGE(WM_USER + 201, WM_USER + 300, OnViewColumnGrpCtrl)
	ON_COMMAND(IDC_GOPARENT, OnGoParent)
	ON_COMMAND(IDC_GOROOT, OnGoRoot)
	ON_UPDATE_COMMAND_UI(IDC_GOPARENT, OnGoParentUI)
	ON_COMMAND(BROWSER_IDC_P4REFRESHLISTCTRL, OnP4RefreshListCtrl)
	ON_CBN_SELENDOK(BROWSER_IDC_COMBOFAVORITES, OnFavoriteChange)
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
EBRO_cl_Frame::EBRO_cl_Frame(void)
{
	mpo_InsideSplitter = new CSplitterWnd;
	mpo_InsideLstSplitter = new CSplitterWnd;
	mpo_TreeCtrl = new EBRO_cl_TreeCtrl;
	mpo_ListCtrl = new EBRO_cl_ListCtrl;
	mpo_GrpCtrl = new EBRO_cl_GrpCtrl;
	mul_TreeItemNormal = BIG_C_InvalidIndex;
	mul_TreeItemEngine = BIG_C_InvalidIndex;
	mi_ListItemNormal = -1;
	mi_ScrollPosXNormal = -1;
	mi_ScrollPosYNormal = -1;
	mi_ScrollPosXEngine = -1;
	mi_ScrollPosYEngine = -1;
	mb_CreateSpecialFile = FALSE;
	mb_NoUpdateVss = FALSE;
	*maz_Filter = 0;

	mb_P4RefreshListCtrl = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EBRO_cl_Frame::~EBRO_cl_Frame(void)
{
	mpo_TreeCtrl->DestroyWindow();
	delete mpo_TreeCtrl;

	mpo_ListCtrl->DestroyWindow();
	delete mpo_ListCtrl;

	mpo_GrpCtrl->DestroyWindow();
	delete mpo_GrpCtrl;

	mpo_InsideSplitter->DestroyWindow();
	delete mpo_InsideSplitter;

	mpo_InsideLstSplitter->DestroyWindow();
	delete mpo_InsideLstSplitter;

	/* Delete the list of associated paths */
	DeleteLinkedPath();
}
#ifdef JADEFUSION 
extern char		IMP_AnimScale_Names[16][256];
extern int		IMP_AnimScale_Num;
extern float	IMP_AnimScale_Values[16];


extern char		IMP_AnimKeepT_Names[32][256];
extern int		IMP_AnimKeepT_Gizmo[32][64];
extern int		IMP_AnimKeepT_Num;

extern char		*BIG_pc_ReadFileTmp(ULONG, ULONG *);
#else
extern "C" char		IMP_AnimScale_Names[16][256];
extern "C" int		IMP_AnimScale_Num;
extern "C" float	IMP_AnimScale_Values[16];


extern "C" char		IMP_AnimKeepT_Names[32][256];
extern "C" int		IMP_AnimKeepT_Gizmo[32][64];
extern "C" int		IMP_AnimKeepT_Num;

extern "C" char		*BIG_pc_ReadFileTmp(ULONG, ULONG *);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ImportDialog_Request(char *sz_Ext)
{
	if(!L_strcmp(sz_Ext, ".trl"))
	{
		/*
		 * if(ANI_gi_ImportMode == -1) { EDIA_cl_NameDialogCombo o_Dialog("Import
		 * animations"); o_Dialog.AddItem(IMP_AnimOptim_Names[0], 0);
		 * o_Dialog.AddItem(IMP_AnimOptim_Names[1], 1);
		 * o_Dialog.AddItem(IMP_AnimOptim_Names[2], 2);
		 * o_Dialog.AddItem(IMP_AnimOptim_Names[3], 3);
		 * o_Dialog.SetDefault(IMP_AnimOptim_Names[0]); if(o_Dialog.DoModal() == IDOK) {
		 * ANI_gi_ImportMode = o_Dialog.mi_CurSelData; return ANI_gi_ImportMode; } else
		 * return -1; }
		 */

		/* SCALE IMPORTATION*/
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			ULONG	ul_File, ul_Length;
			char	*pc_Buffer, *pc_Buf;
			char	asz_Name[256];
			float	f_Scale;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			IMP_AnimScale_Num = 0;

			ul_File = BIG_ul_SearchFileExt("Root/EngineDatas/03 Animation Bank", "AnimScale.ini");
			if(ul_File == BIG_C_InvalidIndex) return 0;

			pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(ul_File), &ul_Length);

			pc_Buf = pc_Buffer;
			while(pc_Buffer - pc_Buf < (int) ul_Length)
			{
				while((*pc_Buffer == '\n') || (*pc_Buffer == '\r')) pc_Buffer++;

				sscanf(pc_Buffer, "%s", asz_Name);

				L_strcpy(&IMP_AnimScale_Names[IMP_AnimScale_Num][0], asz_Name);

				while((*pc_Buffer != '\t') && (*pc_Buffer != ' ')) pc_Buffer++;
				while((*pc_Buffer == '\t') || (*pc_Buffer == ' ')) pc_Buffer++;

				sscanf(pc_Buffer, "%f", &f_Scale);
				IMP_AnimScale_Values[IMP_AnimScale_Num] = f_Scale;
				IMP_AnimScale_Num++;

				while((*pc_Buffer != '\n') && (*pc_Buffer != '\r'))
				{
					if(pc_Buffer - pc_Buf > (int) ul_Length) break;

					pc_Buffer++;
				}

				while((*pc_Buffer == '\n') || (*pc_Buffer == '\r'))
				{
					if(pc_Buffer - pc_Buf > (int) ul_Length) break;

					pc_Buffer++;
				}
			}
		}

		/* KEEP T IMPORTATION */
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			ULONG	ul_File, ul_Length;
			char	*pc_Buffer, *pc_Buf;
			char	asz_Name[256];
			int		i, j, i_Gizmo;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			/* Init */
			for(i=0; i < 32;i++)
			{
				for(j=0; j < 64;j++)
					IMP_AnimKeepT_Gizmo[i][j] = -1;
			}

			IMP_AnimKeepT_Num = 0;


			ul_File = BIG_ul_SearchFileExt("Root/EngineDatas/03 Animation Bank", "KeepTranslation.ini");
			if(ul_File == BIG_C_InvalidIndex) return 0;

			pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(ul_File), &ul_Length);

			pc_Buf = pc_Buffer;
			while(pc_Buffer - pc_Buf < (int) ul_Length)
			{
				while((*pc_Buffer == '\n') || (*pc_Buffer == '\r') || (*pc_Buffer == '\t') || (*pc_Buffer == ' ')) pc_Buffer++;

				if(pc_Buffer - pc_Buf >= (int) ul_Length) break;

				sscanf(pc_Buffer, "%s", asz_Name);

				L_strcpy(&IMP_AnimKeepT_Names[IMP_AnimKeepT_Num][0], asz_Name);
				i = 0;

				while((*pc_Buffer != '\t') && (*pc_Buffer != ' ') && (*pc_Buffer != '\n') && (*pc_Buffer != '\r')) pc_Buffer++;


				while((*pc_Buffer != '\n') && (*pc_Buffer != '\r'))
				{
					while((*pc_Buffer == '\t') || (*pc_Buffer == ' ')) pc_Buffer++;

					if((*pc_Buffer == '\n') || (*pc_Buffer == '\r')) break;						
					if(pc_Buffer - pc_Buf > (int) ul_Length) 
						return 0;


					sscanf(pc_Buffer, "%i", &i_Gizmo);
					IMP_AnimKeepT_Gizmo[IMP_AnimKeepT_Num][i++] = i_Gizmo;

					while((*pc_Buffer != '\t') && (*pc_Buffer != ' ') && (*pc_Buffer != '\n') && (*pc_Buffer != '\r')) 
					{
						
						if(pc_Buffer - pc_Buf > (int) ul_Length) 
							return 0;
						

						pc_Buffer++;
					}
				}

				IMP_AnimKeepT_Num++;
			}
		}
	}

	return 0;
}
/*$4
 ***********************************************************************************************************************
    MESSAGES
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EBRO_cl_Frame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(EDI_cl_BaseFrame::OnCreate(lpCreateStruct) != 0) return -1;

	/* Create inside splitter. */
	mpo_InsideSplitter->CreateStatic(this, 2, 1, WS_CHILD | WS_VISIBLE);

	/* Create inside splitter. */
	mpo_InsideLstSplitter->CreateStatic
		(
			mpo_InsideSplitter,
			2,
			1,
			WS_CHILD | WS_VISIBLE,
			mpo_InsideSplitter->IdFromRowCol(0, 0)
		);

	/* Tree ctrl */
	mpo_TreeCtrl->Create
		(
			TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
			CRect(0, 50, 100, 100),
			mpo_InsideLstSplitter,
			mpo_InsideLstSplitter->IdFromRowCol(0, 0)
		);

	mpo_TreeCtrl->SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);
	mpo_TreeCtrl->SetFont(&M_MF()->mo_Fnt);

	/* Create list */
	mpo_ListCtrl->Create
		(
			LVS_REPORT | LVS_SORTASCENDING | LVS_SHOWSELALWAYS,
			CRect(0, 0, 0, 0),
			mpo_InsideLstSplitter,
			mpo_InsideLstSplitter->IdFromRowCol(1, 0)
		);

	mpo_ListCtrl->SetImageList(&(M_MF()->mo_FileImageList), LVSIL_SMALL);
	mpo_ListCtrl->SetImageList(&(M_MF()->mo_FileImageList1), LVSIL_NORMAL);
	mpo_ListCtrl->SetFont(&M_MF()->mo_Fnt);

	/* Create list */
	mpo_GrpCtrl->Create
		(
			LVS_REPORT | LVS_SORTASCENDING | LVS_SHOWSELALWAYS,
			CRect(0, 0, 0, 0),
			mpo_InsideSplitter,
			mpo_InsideSplitter->IdFromRowCol(1, 0)
		);

	mpo_GrpCtrl->SetImageList(&(M_MF()->mo_FileImageList), LVSIL_SMALL);
	mpo_GrpCtrl->SetImageList(&(M_MF()->mo_FileImageList1), LVSIL_NORMAL);
	mpo_GrpCtrl->SetFont(&M_MF()->mo_Fnt);

	IMP_gst_ImportDialog_Interface.pfnb_Request = ImportDialog_Request;

	/* Init all default values. */
	ReinitIni();

	/* Redraw window */
	mpo_InsideSplitter->RecalcLayout();
	RecalcLayout();

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CRect		o_Rect;
	CComboBox	*po_Combo;
	/*~~~~~~~~~~~~~~~~~~*/

	/* Call parent function */
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);

	/* Move combo box */
	po_Combo = (CComboBox *) mpo_DialogBar->GetDlgItem(BROWSER_IDC_COMBOFAVORITES);
	po_Combo->GetWindowRect(&o_Rect);
	ScreenToClient(&o_Rect);
	po_Combo->SetWindowPos(0, 0, 0, cx - o_Rect.left-10, 250, SWP_NOMOVE | SWP_NOZORDER);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnMouseMove(UINT, CPoint pt)
{
	M_MF()->b_MoveDragDrop(pt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnRButtonDown(UINT, CPoint pt)
{
	if(EDI_gst_DragDrop.b_BeginDragDrop) M_MF()->CancelDragDrop();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnLButtonUp(UINT, CPoint pt)
{
	if(EDI_gst_DragDrop.b_BeginDragDrop) M_MF()->EndDragDrop(pt);
}

/*$4
 ***********************************************************************************************************************
    MESSAGES:: COMMANDS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnCtrlPopup(CPoint pt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	InitPopupMenuAction(&o_Menu);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_ADDFAV);

	if ( M_MF()->mst_Ini.b_LinkControlON ) 
	{
		AddPopupMenuAction(&o_Menu, 0);
		AddPopupMenuAction(&o_Menu, EBRO_ACTION_CHECKOUT);
		AddPopupMenuAction(&o_Menu, EBRO_ACTION_LOCALCHECKOUT);
		AddPopupMenuAction(&o_Menu, EBRO_ACTION_UNDOCHECKOUT);
		AddPopupMenuAction(&o_Menu, EBRO_ACTION_GETLVERSION);
		AddPopupMenuAction(&o_Menu, EBRO_ACTION_CHECKIN);
		AddPopupMenuAction(&o_Menu, EBRO_ACTION_CHECKINNOOUT);
		AddPopupMenuAction(&o_Menu, EBRO_ACTION_CHECKINMIRROR);
		AddPopupMenuAction(&o_Menu, 0);
		AddPopupMenuAction(&o_Menu, EBRO_ACTION_DELDIR);
	}

	AddPopupMenuAction(&o_Menu, 0);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_IGNORERECENT);
	AddPopupMenuAction(&o_Menu, 0);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_IMPORT);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_IMPFROMEXT);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_LINKTOEXT);
	AddPopupMenuAction(&o_Menu, 0);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_IMPORTONLYLOADED);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_IMPORTONLYTRUNCATED);
	AddPopupMenuAction(&o_Menu, 0);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_RENAME);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_RENAMEMULTI);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_DELETE);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_LOGSIZE);
	AddPopupMenuAction(&o_Menu, 0);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_CREATEDIR);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_CREATEFILE);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_CREATESPECIALFILE);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_ORDERGRP);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_CLEANGRP);
	AddPopupMenuAction(&o_Menu, 0);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_ZOOM3D1);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_FORCELOAD1);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_SELECTGROUP1);
	AddPopupMenuAction(&o_Menu, 0);
#ifdef JADEFUSION
	if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
	{
		AddPopupMenuAction(&o_Menu, EBRO_ACTION_FINDCHECKWORLD);
		AddPopupMenuAction(&o_Menu, 0);
	}
#endif
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_CREATECOB);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_DISPLAYINFO);
	AddPopupMenuAction(&o_Menu, 0);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_CREATEASSSMD);
	AddPopupMenuAction(&o_Menu, EBRO_ACTION_CREATEASSMTX);
	TrackPopupMenuAction(pt, &o_Menu);
}

/*
 =======================================================================================================================
    Aim: Call when the selection has changed in the tree ctrl. This function delete the list ctrl content and fill it
    with the files of the new selected directory.
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnTreeCtrlSelChange(void)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG		ul_Dir;
	HTREEITEM	h_Item;
	CRect		o_Rect;
	/*~~~~~~~~~~~~~~~*/

	/* Get selected directory */
	if((h_Item = mpo_TreeCtrl->GetSelectedItem()) == NULL) return;

	ul_Dir = mpo_TreeCtrl->GetItemData(h_Item);

	/* History */
	M_MF()->AddHistoryFile(this, ul_Dir);

	/* Update file list */
	mpo_ListCtrl->ShowWindow(SW_HIDE);
	mpo_ListCtrl->mb_LockSel = TRUE;
	mpo_ListCtrl->DeleteAllItems();
	mpo_ListCtrl->mb_LockSel = FALSE;
	ResetAllColumnsListCtrl(TRUE);

	/* Query Perforce server to get data for currently selected directory */
	OnPerforceRefreshListCtrl(ul_Dir);

	if(EBRO_M_EngineMode())
		mpo_ListCtrl->FillFileEngineDatas(mpo_MyView, ul_Dir);
	else
		mpo_ListCtrl->FillFile(mpo_MyView, ul_Dir, FALSE);
	mpo_ListCtrl->ShowWindow(SW_SHOW);

	/* Update group list */
	mpo_GrpCtrl->ShowWindow(SW_HIDE);
	mpo_GrpCtrl->DeleteAllItems();
	mpo_GrpCtrl->ShowWindow(SW_SHOW);

	/* Send selection to linked editors */
	if(EBRO_M_EngineMode())
		M_MF()->SendMessageToLinks(this, EDI_MESSAGE_SELDIRDATA, ul_Dir, 0);
	else
		M_MF()->SendMessageToLinks(this, EDI_MESSAGE_SELDIR, ul_Dir, 0);

	/* Refresh left menu */
	RefreshMenu();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::ResetExpanded(int _i_NumEdit, BIG_INDEX ul_Index)
{
	/*~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Item;
	/*~~~~~~~~~~~~~~~*/

	while(ul_Index != BIG_C_InvalidIndex)
	{
		h_Item = x_GetTreeItemByName(ul_Index);
		mpo_TreeCtrl->Expand(h_Item, TVE_COLLAPSE);
		BIG_gst.dst_DirTable[ul_Index].st_BRO.b_IsExpanded[_i_NumEdit] = FALSE;
		ul_Index = BIG_NextDir(ul_Index);
	}
}

/*
 =======================================================================================================================
    Aim: Save the info to remember if an item is currently expanded or not. The info is saved in the BigFile FAT in
    st_BRO structure. When we create a new tree based on a previous one, we can set the expanded state of the tree item
    as it was previously.
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnTreeCtrlExpanded(NM_TREEVIEW *pNotifyStruct)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG		ul_Dir;
	HTREEITEM	h_Item;
	/*~~~~~~~~~~~~~~~*/

	M_MF()->LockDisplay(mpo_TreeCtrl);

	/* Retreive corresponding item */
	h_Item = pNotifyStruct->itemNew.hItem;

	/* Retreive directory in FAT */
	ul_Dir = mpo_TreeCtrl->GetItemData(h_Item);

	/* Save expanded state */
	if(pNotifyStruct->itemNew.state & TVIS_EXPANDED)
		BIG_gst.dst_DirTable[ul_Dir].st_BRO.b_IsExpanded[mi_NumEdit] = TRUE;
	else
		BIG_gst.dst_DirTable[ul_Dir].st_BRO.b_IsExpanded[mi_NumEdit] = FALSE;

	/* Reset for each subdir */
	ul_Dir = BIG_SubDir(ul_Dir);
	ResetExpanded(mi_NumEdit, ul_Dir);

	M_MF()->UnlockDisplay(mpo_TreeCtrl);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::AppendMenu(CMenu &o_MenuH, CMenu &o_MenuB)
{
	/*~~~~~~~~~~~~~*/
	UINT	i;
	UINT	ui_State;
	CString mo_Text;
	/*~~~~~~~~~~~~~*/

	if(o_MenuH.GetMenuItemCount()) o_MenuH.AppendMenu(MF_SEPARATOR, 0, (LPCSTR) NULL);

	for(i = 0; i < o_MenuB.GetMenuItemCount(); i++)
	{
		ui_State = o_MenuB.GetMenuState(i, MF_BYPOSITION);
		o_MenuB.GetMenuString(i, mo_Text, MF_BYPOSITION);
		o_MenuH.AppendMenu(ui_State, o_MenuB.GetMenuItemID(i), mo_Text);
	}

	o_MenuB.DestroyMenu();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnTreeCtrlBeginDrag(CPoint pt)
{
	/*~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Item;
	/*~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	EDI_gst_DragDrop.ul_FatDir = mpo_TreeCtrl->GetItemData(h_Item);
	EDI_gst_DragDrop.ul_FatFile = BIG_C_InvalidIndex;
	EDI_gst_DragDrop.i_Param3 = 1;

	mpo_TreeCtrl->ClientToScreen(&pt);
	ScreenToClient(&pt);
	M_MF()->BeginDragDrop(pt, this, this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnListCtrlBeginDrag(CPoint pt, int _i_Item)
{
	/*~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Item;
	int			i_Type;
	/*~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();

	if(EBRO_M_EngineMode())
	{
		EDI_gst_DragDrop.ul_FatDir = BIG_C_InvalidIndex;
		EDI_gst_DragDrop.ul_FatFile = BIG_C_InvalidIndex;
		EDI_gst_DragDrop.i_Param1 = (int) mpo_TreeCtrl->GetItemData(h_Item);
		EDI_gst_DragDrop.i_Param2 = (int) mpo_ListCtrl->GetItemData(_i_Item);
		EDI_gst_DragDrop.i_Param3 = 2;
		i_Type = EDI_DD_Data;
	}
	else
	{
		EDI_gst_DragDrop.ul_FatDir = mpo_TreeCtrl->GetItemData(h_Item);
		EDI_gst_DragDrop.ul_FatFile = mpo_ListCtrl->GetItemData(_i_Item);
		EDI_gst_DragDrop.i_Param3 = 2;
		i_Type = EDI_DD_File;
	}

	mpo_ListCtrl->ClientToScreen(&pt);
	ScreenToClient(&pt);
	M_MF()->BeginDragDrop(pt, this, this, i_Type);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnGrpCtrlBeginDrag(CPoint pt, int _i_Item)
{
	/*~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Item;
	int			i_Type;
	/*~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();

	if(EBRO_M_EngineMode())
	{
		EDI_gst_DragDrop.ul_FatDir = BIG_C_InvalidIndex;
		EDI_gst_DragDrop.ul_FatFile = BIG_C_InvalidIndex;
		EDI_gst_DragDrop.i_Param1 = (int) mpo_TreeCtrl->GetItemData(h_Item);
		EDI_gst_DragDrop.i_Param2 = (int) mpo_GrpCtrl->GetItemData(_i_Item);
		EDI_gst_DragDrop.i_Param3 = 3;
		i_Type = EDI_DD_Data;
	}
	else
	{
		EDI_gst_DragDrop.ul_FatDir = mpo_TreeCtrl->GetItemData(h_Item);
		EDI_gst_DragDrop.ul_FatFile = mpo_GrpCtrl->GetItemData(_i_Item);
		EDI_gst_DragDrop.ul_FatFile = BIG_ul_SearchKeyToFat(EDI_gst_DragDrop.ul_FatFile);
		EDI_gst_DragDrop.i_Param3 = 3;
		i_Type = EDI_DD_File;
	}

	mpo_GrpCtrl->ClientToScreen(&pt);
	ScreenToClient(&pt);
	M_MF()->BeginDragDrop(pt, this, this, i_Type);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnListCtrlDblClk(int _i_Item)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	HTREEITEM			h_Item;
	EDI_cl_BaseFrame	*po_Ed;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < M_MF()->mi_TotalEditors; i++)
	{
		h_Item = mpo_TreeCtrl->GetSelectedItem();
		po_Ed = M_MF()->mast_ListOfEditors[i].po_Instance;
		if(po_Ed && (po_Ed->mst_Def.i_Type != EDI_IDEDIT_BROWSER))
		{
			if(EBRO_M_EngineMode())
			{
_Try_
				if(po_Ed->i_OnMessage(EDI_MESSAGE_CANSELDATA, NULL, mpo_ListCtrl->GetItemData(_i_Item)))
				{
					if(po_Ed->i_OnMessage(EDI_MESSAGE_SELDATA, NULL, mpo_ListCtrl->GetItemData(_i_Item)))
						po_Ed->mpo_MyView->IWantToBeActive(po_Ed);
				}

_Catch_
_End_
			}
			else
			{
_Try_
				if
				(
					po_Ed->i_OnMessage
						(
							EDI_MESSAGE_CANSELFILE,
							mpo_TreeCtrl->GetItemData(h_Item),
							mpo_ListCtrl->GetItemData(_i_Item)
						)
				)
				{
					if
					(
						po_Ed->i_OnMessage
							(
								EDI_MESSAGE_SELFILE,
								mpo_TreeCtrl->GetItemData(h_Item),
								mpo_ListCtrl->GetItemData(_i_Item)
							)
					) po_Ed->mpo_MyView->IWantToBeActive(po_Ed);
				}

_Catch_
_End_
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EBRO_cl_Frame::FillWithPointers(void *_p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	LINK_tdst_Pointer	*p2;
	/*~~~~~~~~~~~~~~~~~~~~*/

	p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer(_p_Data);
	if(p2)
	{
		if(p2->i_Type == LINK_C_ENG_GameObjectOriented)
		{
			mpo_GrpCtrl->FillGameObject(mpo_MyView, (OBJ_tdst_GameObject *) p2->pv_Data);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnListCtrlItemSel(int _i_Item)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM			h_Item;
	int					i_Item;
	BIG_INDEX			ul_Index;
	int					i;
	BOOL				b_First;
	LINK_tdst_Pointer	*p2;
	BOOL				b_Already;
	CRect				o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Already = FALSE;
	if(_i_Item != -1)
	{
		h_Item = mpo_TreeCtrl->GetSelectedItem();

		if(EBRO_M_EngineMode())
		{
			/* Send selection to linked editors */
_Try_
			i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
			for(i = 0; i < (int) mpo_ListCtrl->GetSelectedCount(); i++)
			{
				if(i == 0)
				{
					M_MF()->SendMessageToLinks(this, EDI_MESSAGE_SELDATA, NULL, mpo_ListCtrl->GetItemData(i_Item));
				}
				else
				{
					M_MF()->SendMessageToLinks(this, EDI_MESSAGE_ADDSELDATA, NULL, mpo_ListCtrl->GetItemData(i_Item));
				}

				i_Item = mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED);
			}

_Catch_
_End_
		}
		else
		{
_Try_
			i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
			b_First = TRUE;
			for(i = 0; i < (int) mpo_ListCtrl->GetSelectedCount(); i++)
			{
				ul_Index = LOA_ul_SearchAddress(BIG_PosFile((ULONG) mpo_ListCtrl->GetItemData(i_Item)));
				if(ul_Index != -1) p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) ul_Index);
				if((ul_Index == -1) || (!p2))
				{
					if(!b_Already)
					{
						M_MF()->SendMessageToLinks
							(
								this,
								EDI_MESSAGE_SELFILE,
								mpo_TreeCtrl->GetItemData(h_Item),
								mpo_ListCtrl->GetItemData(i_Item)
							);
					}

					b_Already = TRUE;
				}
				else if(b_First)
				{
					b_First = FALSE;
					M_MF()->SendMessageToLinks(this, EDI_MESSAGE_SELDATA, NULL, ul_Index);
				}
				else
				{
					M_MF()->SendMessageToLinks(this, EDI_MESSAGE_ADDSELDATA, NULL, ul_Index);
				}

				i_Item = mpo_ListCtrl->GetNextItem(i_Item, LVNI_SELECTED);
			}

_Catch_
_End_
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Update group list
		 ---------------------------------------------------------------------------------------------------------------
		 */

		mpo_GrpCtrl->ShowWindow(SW_HIDE);
		mpo_GrpCtrl->DeleteAllItems();
		_i_Item = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
		if(_i_Item != -1)
		{
			ul_Index = mpo_ListCtrl->GetItemData(_i_Item);

			/* Normal group file */
			if(EBRO_M_EngineMode())
			{
				FillWithPointers((void *) ul_Index);
			}
			else
			{
				if(BIG_b_IsGrpFile(ul_Index))
				{
					mpo_GrpCtrl->FillGrp(mpo_MyView, ul_Index);
				}
			}
		}

		/* Game object */
		mpo_GrpCtrl->ShowWindow(SW_SHOW);
	}

	/* Refresh left menu */
	RefreshMenu();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnGrpCtrlItemSel(int _i_Item)
{
	/*~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Item;
	BIG_INDEX	ul_Fat;
	/*~~~~~~~~~~~~~~~*/

	if(_i_Item != -1)
	{
		h_Item = mpo_TreeCtrl->GetSelectedItem();

		if(EBRO_M_EngineMode())
		{
			/* Send selection to linked editors */
			ul_Fat = mpo_GrpCtrl->GetItemData(_i_Item);
			if(LINK_p_SearchPointer((void *) ul_Fat))
				M_MF()->SendMessageToLinks(this, EDI_MESSAGE_SELDATA, NULL, ul_Fat);
		}
		else
		{
			/* Send selection to linked editors */
			ul_Fat = BIG_ul_SearchKeyToFat(mpo_GrpCtrl->GetItemData(_i_Item));
_Try_
			M_MF()->SendMessageToLinks(this, EDI_MESSAGE_SELFILE, mpo_TreeCtrl->GetItemData(h_Item), ul_Fat);
_Catch_
_End_
		}
	}

	/* Refresh left menu */
	RefreshMenu();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnGrpCtrlDblClk(int _i_Item)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	HTREEITEM			h_Item;
	EDI_cl_BaseFrame	*po_Ed;
	BIG_INDEX			ul_File;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_Item = mpo_TreeCtrl->GetSelectedItem();
	for(i = 0; i < M_MF()->mi_TotalEditors; i++)
	{
		po_Ed = M_MF()->mast_ListOfEditors[i].po_Instance;
		if(po_Ed && (po_Ed->mst_Def.i_Type != EDI_IDEDIT_BROWSER))
		{
			if(EBRO_M_EngineMode())
			{
_Try_
				if(po_Ed->i_OnMessage(EDI_MESSAGE_CANSELDATA, NULL, mpo_GrpCtrl->GetItemData(_i_Item)))
				{
					if(po_Ed->i_OnMessage(EDI_MESSAGE_SELDATA, NULL, mpo_GrpCtrl->GetItemData(_i_Item)))
						po_Ed->mpo_MyView->IWantToBeActive(po_Ed);
				}

_Catch_
_End_
			}
			else
			{
_Try_
				ul_File = BIG_ul_SearchKeyToFat(mpo_GrpCtrl->GetItemData(_i_Item));
				if(po_Ed->i_OnMessage(EDI_MESSAGE_CANSELFILE, mpo_TreeCtrl->GetItemData(h_Item), ul_File))
				{
					if(po_Ed->i_OnMessage(EDI_MESSAGE_SELFILE, mpo_TreeCtrl->GetItemData(h_Item), ul_File))
						po_Ed->mpo_MyView->IWantToBeActive(po_Ed);
				}

_Catch_
_End_
			}
		}
	}
}

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnP4RefreshListCtrl(void)
{
	if ( mpo_DialogBar->IsDlgButtonChecked(BROWSER_IDC_P4REFRESHLISTCTRL) == BST_CHECKED )
	{
		mb_P4RefreshListCtrl = TRUE;
	}
	else if ( mpo_DialogBar->IsDlgButtonChecked(BROWSER_IDC_P4REFRESHLISTCTRL) == BST_UNCHECKED )
	{
		mb_P4RefreshListCtrl = FALSE;
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void EBRO_cl_Frame::OnRefresh(void)
{
	RefreshAll(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnGoRoot(void)
{
	/* To hide carret and default style */
	(CButton *) mpo_DialogBar->GetDlgItem(IDC_GOROOT)->ModifyStyle(BS_DEFPUSHBUTTON, BS_PUSHBUTTON);
	mpo_TreeCtrl->SetFocus();

	/* Select root dir */
	SelectDir(BIG_Root());
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnGoParent(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	HTREEITEM	h;
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~*/

	/* To hide carret and default style */
	(CButton *) mpo_DialogBar->GetDlgItem(IDC_GOPARENT)->ModifyStyle(BS_DEFPUSHBUTTON, BS_PUSHBUTTON);
	mpo_TreeCtrl->SetFocus();

	/* Select parent dir */
	h = mpo_TreeCtrl->GetSelectedItem();
	if(h)
	{
		ul_Index = mpo_TreeCtrl->GetItemData(h);
		h = mpo_TreeCtrl->GetNextItem(h, TVGN_PARENT);

		if(h)
		{
			mpo_TreeCtrl->EnsureVisible(h);
			mpo_TreeCtrl->SelectItem(h);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnGoParentUI(CCmdUI *_po_UI)
{
	/*~~~~~~~~~~~~~~~~~*/
	HTREEITEM	h;
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~*/

	h = mpo_TreeCtrl->GetSelectedItem();
	if(!h)
		_po_UI->Enable(FALSE);
	else
	{
		ul_Index = mpo_TreeCtrl->GetItemData(h);
		if(BIG_ParentDir(ul_Index) == BIG_C_InvalidIndex)
			_po_UI->Enable(FALSE);
		else
			_po_UI->Enable(TRUE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
HTREEITEM EBRO_cl_Frame::x_GetTreeItemByName(BIG_INDEX _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Name[BIG_C_MaxLenPath];
	char		asz_Root[BIG_C_MaxLenPath];
	HTREEITEM	x_Tree;
	CString		o_Name;
	char		*psz_Temp, *psz_Temp1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Test if the dir has been deleted. If yes, return NULL */
	if((BIG_DirChanged(_ul_Index) == EDI_FHC_Delete) || (BIG_DirChanged(_ul_Index) == EDI_FHC_Deleted))
	{
		return NULL;
	}

	/* Else compute the full name of the dir */
	BIG_ComputeFullName(_ul_Index, asz_Name);

	/* Scan the tree control, root to desired dir */
	x_Tree = mpo_TreeCtrl->GetRootItem();
	if(x_Tree == NULL) return NULL;

	/* Compute the name of the root directory in the tree */
	BIG_ComputeFullName(mpo_TreeCtrl->GetItemData(x_Tree), asz_Root);
	if(L_strncmp(asz_Root, asz_Name, L_strlen(asz_Root))) return NULL;
	psz_Temp = L_strrchr(asz_Root, '/');
	if(!psz_Temp)
		psz_Temp = asz_Name;
	else
		psz_Temp = asz_Name + (psz_Temp - asz_Root) + 1;

	/* Search the parameter directory */
	do
	{
		o_Name = mpo_TreeCtrl->GetItemText(x_Tree);
		psz_Temp1 = L_strchr(psz_Temp, '/');
		if(psz_Temp1)
		{
			*psz_Temp1 = 0;
			psz_Temp1++;
		}

		while(L_strcmpi(psz_Temp, (char *) (LPCSTR) o_Name))
		{
			x_Tree = mpo_TreeCtrl->GetNextItem(x_Tree, TVGN_NEXT);
			if(x_Tree == NULL) return NULL;
			o_Name = mpo_TreeCtrl->GetItemText(x_Tree);
		}

		psz_Temp = psz_Temp1;
		if(psz_Temp) x_Tree = mpo_TreeCtrl->GetChildItem(x_Tree);
	} while(psz_Temp && x_Tree);

	return x_Tree;
}

/*
 =======================================================================================================================
    Aim: Force the refresh of all trees and lists. The trees and list are destroyed, and reconstruct. Try as possible
    to restore the same aspect as before. In: _b_VSS TRUE if we must request data control file to know if the listctrl
    file is currently checked out.
 =======================================================================================================================
 */
void EBRO_cl_Frame::RefreshAll(BOOL _b_VSS, BIG_INDEX _ul_TreeSel, int _i_LstSel, int _i_ScrollX, int _i_ScrollY)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Item, h_Item1;
	int			i_SelItem, i_SelGrp;
	BIG_INDEX	ul_DirNext, ul_DirPrev, ul_DirCur, ul_DirParent;
	int			i_TreeScrollH, i_TreeScrollV;
	CWnd		*po_Wnd;
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Lock display */

	/* if(GetAsyncKeyState(VK_CONTROL) < 0) return; */
	po_Wnd = GetFocus();

	if(mb_IsActivate) M_MF()->LockDisplay(this);
	mpo_TreeCtrl->ShowWindow(SW_HIDE);
	mpo_ListCtrl->ShowWindow(SW_HIDE);
	mpo_GrpCtrl->ShowWindow(SW_HIDE);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Refresh tree.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* Save scroll pos */
	i_TreeScrollH = _i_ScrollX;
	if(_i_ScrollX == -1) i_TreeScrollH = mpo_TreeCtrl->GetScrollPos(SB_HORZ);
	i_TreeScrollV = _i_ScrollY;
	if(_i_ScrollY == -1) i_TreeScrollV = mpo_TreeCtrl->GetScrollPos(SB_VERT);

	/* Remember current selection for tree */
	ul_DirParent = ul_DirCur = ul_DirPrev = ul_DirNext = BIG_C_InvalidIndex;
	ul_DirCur = _ul_TreeSel;
	if(ul_DirCur == BIG_C_InvalidIndex)
	{
		h_Item = mpo_TreeCtrl->GetSelectedItem();
		if(h_Item)
		{
			ul_DirCur = mpo_TreeCtrl->GetItemData(h_Item);
			h_Item1 = mpo_TreeCtrl->GetPrevSiblingItem(h_Item);
			if(h_Item1) ul_DirPrev = mpo_TreeCtrl->GetItemData(h_Item1);

			h_Item1 = mpo_TreeCtrl->GetNextSiblingItem(h_Item);
			if(h_Item1) ul_DirNext = mpo_TreeCtrl->GetItemData(h_Item1);

			h_Item1 = mpo_TreeCtrl->GetParentItem(h_Item);
			if(h_Item1) ul_DirParent = mpo_TreeCtrl->GetItemData(h_Item1);
		}
		else
		{
			h_Item = (HTREEITEM) - 1;
		}
	}

	/* Remember current selection for list */
	i_SelItem = _i_LstSel;
	if(i_SelItem == -1) i_SelItem = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
	i_SelGrp = mpo_GrpCtrl->GetNextItem(-1, LVNI_SELECTED);

	/* Fill */
	if(!LINK_gb_RefreshOnlyFiles)
	{
		if(EBRO_M_EngineMode())
		{
			mpo_TreeCtrl->DeleteAllItems();
			mpo_TreeCtrl->FillDir(mpo_MyView, mi_NumEdit, TVI_ROOT, BIG_C_InvalidIndex, BIG_Root());
		}
		else
		{
			mpo_TreeCtrl->DeleteAllItems();
			mpo_TreeCtrl->FillDir(mpo_MyView, mi_NumEdit, TVI_ROOT, BIG_C_InvalidIndex, BIG_Root());
		}

		/* Select tree item */
		if(ul_DirCur != BIG_C_InvalidIndex) h_Item = x_GetTreeItemByName(ul_DirCur);
		if((h_Item == NULL) && (ul_DirNext != BIG_C_InvalidIndex)) h_Item = x_GetTreeItemByName(ul_DirNext);
		if((h_Item == NULL) && (ul_DirPrev != BIG_C_InvalidIndex)) h_Item = x_GetTreeItemByName(ul_DirPrev);
		if((h_Item == NULL) && (ul_DirParent != BIG_C_InvalidIndex)) h_Item = x_GetTreeItemByName(ul_DirParent);

		if(h_Item && ((int) h_Item != -1))
		{
			mpo_TreeCtrl->mb_LockSel = TRUE;
			mpo_TreeCtrl->SelectItem(h_Item);
			mpo_TreeCtrl->mb_LockSel = FALSE;

			/* Restore scroll pos */
			if(i_TreeScrollH)
			{
				mpo_TreeCtrl->SetScrollPos(SB_HORZ, i_TreeScrollH);
				mpo_TreeCtrl->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (i_TreeScrollH << 16), 0);
			}

			if(i_TreeScrollV)
			{
				mpo_TreeCtrl->EnableScrollBar(SB_VERT);
				mpo_TreeCtrl->SetScrollPos(SB_VERT, i_TreeScrollV);
				mpo_TreeCtrl->SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (i_TreeScrollV << 16), 0);
			}

			mpo_TreeCtrl->EnsureVisible(h_Item);
		}
		else
			h_Item = NULL;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Update the file list.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* Fill */
    mpo_ListCtrl->mb_LockSel = TRUE;    
	mpo_ListCtrl->DeleteAllItems();
    mpo_ListCtrl->mb_LockSel = FALSE;

	if(h_Item && (int) h_Item != -1)
	{
		if(EBRO_M_EngineMode())
			mpo_ListCtrl->FillFileEngineDatas(mpo_MyView, mpo_TreeCtrl->GetItemData(h_Item));
		else
			mpo_ListCtrl->FillFile(mpo_MyView, mpo_TreeCtrl->GetItemData(h_Item), _b_VSS);
	}

	/* Restore focus */
	if(i_SelItem >= mpo_ListCtrl->GetItemCount()) i_SelItem = mpo_ListCtrl->GetItemCount() - 1;
	if(i_SelItem >= 0)
	{
		mpo_ListCtrl->mb_LockSel = TRUE;
		mpo_ListCtrl->SetItemState(i_SelItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

		mpo_ListCtrl->EnsureVisible(i_SelItem, FALSE);
		mpo_ListCtrl->mb_LockSel = FALSE;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Update the group control
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* Fill */
	mpo_GrpCtrl->DeleteAllItems();
	if(i_SelItem >= 0)
	{
		ul_Index = mpo_ListCtrl->GetItemData(i_SelItem);
		if(EBRO_M_EngineMode())
		{
			FillWithPointers((void *) ul_Index);
		}
		else
		{
			if(BIG_b_IsGrpFile(ul_Index))
				mpo_GrpCtrl->FillGrp(mpo_MyView, ul_Index);
			else
				i_SelItem = -1;
		}
	}

	if(i_SelItem != -1)
	{
		if(i_SelGrp >= mpo_GrpCtrl->GetItemCount()) i_SelGrp = mpo_GrpCtrl->GetItemCount() - 1;
		if(i_SelGrp >= 0)
		{
			mpo_GrpCtrl->mb_LockSel = TRUE;
			mpo_GrpCtrl->SetItemState(i_SelGrp, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

			mpo_GrpCtrl->EnsureVisible(i_SelGrp, FALSE);
			mpo_GrpCtrl->mb_LockSel = FALSE;
		}
	}

	/* Unlock display */
	mpo_TreeCtrl->ShowWindow(SW_SHOW);
	mpo_ListCtrl->ShowWindow(SW_SHOW);
	mpo_GrpCtrl->ShowWindow(SW_SHOW);
	if(mb_IsActivate) M_MF()->UnlockDisplay(this);

	/* Restore focus */
	if(po_Wnd) po_Wnd->SetFocus();

	/* Refresh left menu */
	RefreshMenu();
}

/*$4
 ***********************************************************************************************************************
    INTERFACE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim: To open a new project.
 =======================================================================================================================
 */
void EBRO_cl_Frame::OpenProject(void)
{
	EDI_cl_BaseFrame::OpenProject();
	RefreshAll();
}

/*
 =======================================================================================================================
    Aim: To close current project. Out: TRUE we can close project, FALSE else.
 =======================================================================================================================
 */
void EBRO_cl_Frame::CloseProject(void)
{
	/* Delete tree and list */
	mpo_TreeCtrl->DeleteAllItems();
	mpo_ListCtrl->DeleteAllItems();
	mpo_GrpCtrl->DeleteAllItems();

	/* Delete all favorites */
	((CComboBox *) mpo_DialogBar->GetDlgItem(BROWSER_IDC_COMBOFAVORITES))->ResetContent();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::BeforeEngine(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::AfterEngine(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OneTrameEnding(void)
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

	RefreshAll(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnEngineMode(void)
{
	/*~~~~~~~~~~~~~~~*/
	HTREEITEM	h_Tree;
	/*~~~~~~~~~~~~~~~*/

	SaveColSizeList();
	SaveColSizeGrp();

	mst_Ini.b_EngineMode = mst_Ini.b_EngineMode ? FALSE : TRUE;

	/* Save current selection */
	if(mst_Ini.b_EngineMode)
	{
		h_Tree = mpo_TreeCtrl->GetSelectedItem();
		mul_TreeItemNormal = BIG_C_InvalidIndex;
		if(h_Tree) mul_TreeItemNormal = mpo_TreeCtrl->GetItemData(h_Tree);
		mi_ListItemNormal = mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED);
		mi_ScrollPosXNormal = mpo_TreeCtrl->GetScrollPos(SB_HORZ);
		mi_ScrollPosYNormal = mpo_TreeCtrl->GetScrollPos(SB_VERT);
	}
	else
	{
		h_Tree = mpo_TreeCtrl->GetSelectedItem();
		mul_TreeItemEngine = BIG_C_InvalidIndex;
		if(h_Tree) mul_TreeItemEngine = mpo_TreeCtrl->GetItemData(h_Tree);
		mi_ScrollPosXEngine = mpo_TreeCtrl->GetScrollPos(SB_HORZ);
		mi_ScrollPosYEngine = mpo_TreeCtrl->GetScrollPos(SB_VERT);
	}

	if(mst_Ini.b_EngineMode && (mul_TreeItemEngine != BIG_C_InvalidIndex))
	{
		h_Tree = x_GetTreeItemByName(mul_TreeItemEngine);
		mpo_TreeCtrl->SelectItem(h_Tree);
	}

	ResetAllColumnsListCtrl();
	ResetAllColumnsGrpCtrl();

	if(mst_Ini.b_EngineMode)
		RefreshAll(TRUE, mul_TreeItemEngine, -1, mi_ScrollPosXEngine, mi_ScrollPosYEngine);
	else
		RefreshAll(TRUE, mul_TreeItemNormal, mi_ListItemNormal, mi_ScrollPosXNormal, mi_ScrollPosYNormal);
}

/*$4
 ***********************************************************************************************************************
    Columns
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnViewColumnListCtrl(UINT _i_ID)
{
	AddDeleteColumnListCtrl(_i_ID - WM_USER - 100);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnViewColumnGrpCtrl(UINT _i_ID)
{
	AddDeleteColumnGrpCtrl(_i_ID - WM_USER - 200);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::AddDeleteColumnListCtrl(int _i_SubItem)
{
	/*~~*/
	int i;
	/*~~*/

	if(mst_Ini.ai_NumColListCtrl[_i_SubItem] != -1)
	{
		/* Save width before deleting column */
		mst_Ini.ai_WidthListCol[_i_SubItem] = mpo_ListCtrl->GetColumnWidth(mst_Ini.ai_NumColListCtrl[_i_SubItem]);

		/* Delete column */
		mpo_ListCtrl->DeleteColumn(mst_Ini.ai_NumColListCtrl[_i_SubItem]);
		mpo_ListCtrl->Invalidate();

		/* Set current column index to -1 to tell that now its hide */
		mst_Ini.ai_NumColListCtrl[_i_SubItem] = -1;

		/* Decrease all next column index */
		for(i = _i_SubItem + 1; i < EBRO_C_ListCtrlMaxColumns; i++)
		{
			if(mst_Ini.ai_NumColListCtrl[i] != -1) mst_Ini.ai_NumColListCtrl[i]--;
		}
	}
	else
	{
		/* Set new column index as the last valid column index + 1 */
		for(i = _i_SubItem - 1; i >= 0; i--)
		{
			if(mst_Ini.ai_NumColListCtrl[i] != -1)
			{
				mst_Ini.ai_NumColListCtrl[_i_SubItem] = mst_Ini.ai_NumColListCtrl[i] + 1;
				break;
			}
		}

		/* Increase all column index of next columns */
		for(i = _i_SubItem + 1; i < EBRO_C_ListCtrlMaxColumns; i++)
		{
			if(mst_Ini.ai_NumColListCtrl[i] != -1) mst_Ini.ai_NumColListCtrl[i]++;
		}

		/* Add column */
		AddColumnToListCtrl(_i_SubItem);

		/* Refresh all */
		RefreshAll();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::AddDeleteColumnGrpCtrl(int _i_SubItem)
{
	/*~~*/
	int i;
	/*~~*/

	if(mst_Ini.ai_NumColGrpCtrl[_i_SubItem] != -1)
	{
		/* Save width before deleting column */
		mst_Ini.ai_WidthGrpCol[_i_SubItem] = mpo_GrpCtrl->GetColumnWidth(mst_Ini.ai_NumColGrpCtrl[_i_SubItem]);

		/* Delete column */
		mpo_GrpCtrl->DeleteColumn(mst_Ini.ai_NumColGrpCtrl[_i_SubItem]);
		mpo_GrpCtrl->Invalidate();

		/* Set current column index to -1 to tell that now its hide */
		mst_Ini.ai_NumColGrpCtrl[_i_SubItem] = -1;

		/* Decrease all next column index */
		for(i = _i_SubItem + 1; i < EBRO_C_GrpCtrlMaxColumns; i++)
		{
			if(mst_Ini.ai_NumColGrpCtrl[i] != -1) mst_Ini.ai_NumColGrpCtrl[i]--;
		}
	}
	else
	{
		/* Set new column index as the last valid column index + 1 */
		for(i = _i_SubItem - 1; i >= 0; i--)
		{
			if(mst_Ini.ai_NumColGrpCtrl[i] != -1)
			{
				mst_Ini.ai_NumColGrpCtrl[_i_SubItem] = mst_Ini.ai_NumColGrpCtrl[i] + 1;
				break;
			}
		}

		/* Increase all column index of next columns */
		for(i = _i_SubItem + 1; i < EBRO_C_GrpCtrlMaxColumns; i++)
		{
			if(mst_Ini.ai_NumColGrpCtrl[i] != -1) mst_Ini.ai_NumColGrpCtrl[i]++;
		}

		/* Add column */
		AddColumnToGrpCtrl(_i_SubItem);

		/* Refresh all */
		RefreshAll();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::ResetAllColumnsListCtrl(BOOL _b_Save)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i;
	HTREEITEM	h_Item;
	BIG_INDEX	ul_Index;
	char		asz_Path[BIG_C_MaxLenPath];
	char		*psz_Temp;
	int			i_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_Save) SaveColSizeList();

	/* Delete all columns */
	for(i = 0; i < EBRO_C_ListCtrlMaxColumns; i++) mpo_ListCtrl->DeleteColumn(0);
	if(mst_Ini.i_ListDispMode != EBRO_C_ListModeReport) return;

	/* Add all columns */
	i_Col = 2;
	for(i = 0; i < EBRO_C_ListCtrlMaxColumns; i++)
	{
		/* Special process if browser is in engine mode */
		if(EBRO_M_EngineMode())
		{
			if((i > 1) && (i < EBRO_C_ListCtrlSpecial)) continue;	/* Columns for files */

			/* Else special datas */
			h_Item = mpo_TreeCtrl->GetSelectedItem();
			if(i >= EBRO_C_ListCtrlSpecial)
			{
				mst_Ini.ai_NumColListCtrl[i] = -1;
				if(h_Item)
				{
					ul_Index = mpo_TreeCtrl->GetItemData(h_Item);
					BIG_ComputeFullName(ul_Index, asz_Path);
					if(!L_strncmp(asz_Path, EDI_Csz_Path_Levels, L_strlen(EDI_Csz_Path_Levels)))
					{
						psz_Temp = asz_Path + L_strlen(EDI_Csz_Path_Levels);
						if(*psz_Temp)
						{
							psz_Temp++;
							psz_Temp = L_strchr(psz_Temp, '/');
							if(psz_Temp)
							{
								psz_Temp++;
								if(!L_strcmpi(psz_Temp, EDI_Csz_Path_GameObject))
								{
									mst_Ini.ai_NumColListCtrl[i] = i_Col++;
									goto l_Ok;
								}
							}
						}
					}
				}
			}

			goto l_Ok;
		}
		else if(i < EBRO_C_ListCtrlSpecial) goto l_Ok;
		continue;
l_Ok:
		if(mst_Ini.ai_NumColListCtrl[i] != -1)
		{
			AddColumnToListCtrl(i);
			mpo_ListCtrl->SetColumnWidth(mst_Ini.ai_NumColListCtrl[i], mst_Ini.ai_WidthListCol[i]);
		}
	}

	/* Check sort column */
	if(mst_Ini.ai_NumColListCtrl[mst_Ini.i_SortColumnListCtrl] == -1)
	{
		mst_Ini.i_SortColumnListCtrl = 0;
		mpo_ListCtrl->mi_SortColumn = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::ResetAllColumnsGrpCtrl(BOOL _b_Save)
{
	/*~~*/
	int i;
	/*~~*/

	if(_b_Save) SaveColSizeGrp();

	/* Delete all columns */
	for(i = 0; i < EBRO_C_GrpCtrlMaxColumns; i++) mpo_GrpCtrl->DeleteColumn(0);

	/* Add all columns */
	for(i = 0; i < EBRO_C_GrpCtrlMaxColumns; i++)
	{
		/* Special process if browser is in engine mode */
		if(EBRO_M_EngineMode() && (i > 1)) continue;

		if(mst_Ini.ai_NumColGrpCtrl[i] != -1)
		{
			AddColumnToGrpCtrl(i);
			mpo_GrpCtrl->SetColumnWidth(mst_Ini.ai_NumColGrpCtrl[i], mst_Ini.ai_WidthGrpCol[i]);
		}
	}

	/* Check sort column */
	if(mst_Ini.ai_NumColGrpCtrl[mst_Ini.i_SortColumnGrpCtrl] == -1)
	{
		mst_Ini.i_SortColumnGrpCtrl = 0;
		mpo_GrpCtrl->mi_SortColumn = 0;
	}
}

/*
 =======================================================================================================================
    Aim: To add a column in list control.
 =======================================================================================================================
 */
void EBRO_cl_Frame::AddColumnToListCtrl(int _i_SubItem)
{
	/*~~~~~~~~~~~~~~~~~~*/
	LV_COLUMN	st_Column;
	/*~~~~~~~~~~~~~~~~~~*/

	st_Column.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	st_Column.iSubItem = _i_SubItem;
	st_Column.cx = mst_Ini.ai_WidthListCol[_i_SubItem];
	st_Column.pszText = EBRO_gasz_ListColNames[_i_SubItem];
	mpo_ListCtrl->InsertColumn(mst_Ini.ai_NumColListCtrl[_i_SubItem], &st_Column);
}

/*
 =======================================================================================================================
    Aim: To add a column in list control.
 =======================================================================================================================
 */
void EBRO_cl_Frame::AddColumnToGrpCtrl(int _i_SubItem)
{
	/*~~~~~~~~~~~~~~~~~~*/
	LV_COLUMN	st_Column;
	/*~~~~~~~~~~~~~~~~~~*/

	st_Column.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	st_Column.iSubItem = _i_SubItem;
	st_Column.cx = mst_Ini.ai_WidthGrpCol[_i_SubItem];
	st_Column.pszText = EBRO_gasz_GrpColNames[_i_SubItem];
	mpo_GrpCtrl->InsertColumn(mst_Ini.ai_NumColGrpCtrl[_i_SubItem], &st_Column);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EBRO_cl_Frame::OnActivate(void)
{
}

#endif /* ACTIVE_EDITORS */
