/*$T PFBframe.cpp GC 1.139 04/15/04 12:53:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDIpaths.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"
#include "EDIstrings.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAmsglink_dlg.h"
#include "DIAlogs/CCheckList.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "SELection/SELection.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "PFBframe.h"
#include "PFBview.h"
#include "PFBtreeview.h"
#define ACTION_GLOBAL
#include "PFBframe_act.h"
#include "Res/Res.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "ENGine/Sources/GRP/GRPsave.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    EXTERNAL.
 -----------------------------------------------------------------------------------------------------------------------
 */

extern "C" {
ULONG					WOR_ul_World_LoadCallback(ULONG);
}

//extern EDIA_cl_MsgLinkDialog	EDI_go_MsgTruncateFiles;
extern EDIA_cl_MsgLinkDialog	EDI_go_MsgGetLatest;
extern EDIA_cl_MsgLinkDialog	EDI_go_MsgUndoCheck;
extern EDIA_cl_MsgLinkDialog	EDI_go_MsgCheckInNo;
extern BOOL						EDI_gb_CheckInDel;
extern BOOL						EDI_gb_NoUpdateVSS;
extern BOOL						EDI_gb_GetLatestDel;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EPFB_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EPFB_cl_Frame, EDI_cl_BaseFrame)
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
EPFB_cl_Frame::EPFB_cl_Frame(void)
{
	mul_DirIndex = BIG_C_InvalidIndex;
	mul_FileIndex = BIG_C_InvalidIndex;

	mpo_Splitter = new CSplitterWnd;
	mpo_DataView = new EPFB_cl_View(this);
	mpo_TreeView = new EPFB_cl_TreeView(this);

	mpst_Prefab = NULL;
	mi_PrefabModif = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EPFB_cl_Frame::~EPFB_cl_Frame(void)
{
	mpo_Splitter->DestroyWindow();
	delete mpo_Splitter;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EPFB_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	/*~~~~~~~~~~~*/
	ULONG	ul_Dir;
	/*~~~~~~~~~~~*/

	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1)
	{
		return -1;
	}

	/* create splitter view */
	mpo_Splitter->CreateStatic(this, 2, 1, WS_CHILD | WS_VISIBLE);

	mpo_TreeView->Create("PFBTree", "PFBTree", 0, CRect(0, 0, 0, 0), mpo_Splitter, mpo_Splitter->IdFromRowCol(0, 0));
	mpo_TreeView->mpo_Editor = this;

	mpo_DataView->Create("PFBData", "PFBData", 0, CRect(0, 0, 0, 0), mpo_Splitter, mpo_Splitter->IdFromRowCol(1, 0));
	mpo_DataView->mpo_Editor = this;

	/* Redraw window */
	RecalcLayout();

	/* create eventually unsorted directory */
	ul_Dir = BIG_ul_SearchDir(EDI_Csz_Path_ObjModels);
	if(ul_Dir == BIG_C_InvalidIndex) BIG_ul_CreateDir(EDI_Csz_Path_ObjModels);
	ul_Dir = BIG_ul_SearchDir(EDI_Csz_Path_PrefabUnsorted);
	if(ul_Dir == BIG_C_InvalidIndex) ul_Dir = BIG_ul_CreateDir(EDI_Csz_Path_PrefabUnsorted);

	mpo_TreeView->Fill(BIG_ul_SearchDir(EDI_Csz_Path_ObjModels));
	mpo_TreeView->SelDir(ul_Dir);
	return 0;

	/* select current dir or data */
	if((mb_SelIsDir) && (mul_DirIndex != BIG_C_InvalidIndex))
	{
		if(mpo_TreeView->SelDir(mul_DirIndex)) return 0;
	}

	if(mul_FileIndex != BIG_C_InvalidIndex)
	{
		if(mpo_TreeView->SelFile(mul_FileIndex))
		{
			mb_SelIsDir = FALSE;
			return 0;
		}
	}

	EDIA_cl_BaseDialog::SetTheme(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPFB_cl_Frame::PreTranslateMessage(MSG *pMsg)
{
	return EDI_cl_BaseFrame::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	/* Call parent function */
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);
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
void EPFB_cl_Frame::Refresh(void)
{
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
void EPFB_cl_Frame::OnClose(void)
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
BOOL EPFB_cl_Frame::b_CanActivate(void)
{
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::CloseProject(void)
{
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
void EPFB_cl_Frame::Tree_SelChange(void)
{
	if(mpst_Prefab)
	{
		if(mpst_Prefab->ul_Index == mul_FileIndex) return;
		if(mi_PrefabModif)
		{
			if(mi_PrefabSaveAuto)
				Prefab_ul_Save(mpst_Prefab);
			else
			{
				/*~~~~~~~~~~~~~~~~~*/
				char	sz_Text[260];
				int		i_Res;
				/*~~~~~~~~~~~~~~~~~*/

				sprintf(sz_Text, "Save Prefab (%s)", BIG_NameFile(mpst_Prefab->ul_Index));
				i_Res = M_MF()->MessageBox(sz_Text, TEXT("Prefab Editor"), MB_YESNOCANCEL | MB_ICONQUESTION);
				if(i_Res == IDCANCEL)
				{
					mpo_TreeView->SelFile(mpst_Prefab->ul_Index);
					return;
				}
				else if(i_Res == IDYES)
					Prefab_ul_Save(mpst_Prefab);
			}
		}

		Prefab_Destroy(&mpst_Prefab);
	}

	if(mul_FileIndex != BIG_C_InvalidIndex) mpst_Prefab = Prefab_pst_Load(mul_FileIndex, NULL);

	mi_PrefabModif = 0;
	mpo_DataView->Display(mpst_Prefab);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::AddPrefab(WOR_tdst_World *_pst_World, SEL_tdst_Selection *_pst_Sel, BOOL _b_Unsorted)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_Prefab		*pst_Prefab, *pst_MergeTo;
	EDIA_cl_NameDialog	o_DlgName("Enter name of new prefab", BIG_C_MaxLenName);
	char				sz_Name[260], *sz_Ext;
	ULONG				ul_Index, ul_Dir;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_Unsorted)
	{
		ul_Dir = BIG_ul_SearchDir(EDI_Csz_Path_Prefab);
		if(ul_Dir == BIG_C_InvalidIndex) BIG_ul_CreateDir(EDI_Csz_Path_Prefab);
		ul_Dir = BIG_ul_SearchDir(EDI_Csz_Path_PrefabUnsorted);
		if(ul_Dir == BIG_C_InvalidIndex) ul_Dir = BIG_ul_CreateDir(EDI_Csz_Path_PrefabUnsorted);
	}
	else
		ul_Dir = mul_DirIndex;

	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(_pst_Sel, SEL_C_SIF_Object);
	o_DlgName.mo_Name = pst_GO->sz_Name;

	while(1)
	{
		if(o_DlgName.DoModal() != IDOK) return;

		L_strcpy(sz_Name, (LPCTSTR) o_DlgName.mo_Name);
		sz_Ext = L_strchr(sz_Name, '.');
		if(!sz_Ext) sz_Ext = sz_Name + strlen(sz_Name);
		L_strcpy(sz_Ext, EDI_Csz_ExtPrefab);

		ul_Index = BIG_ul_SearchFile(ul_Dir, sz_Name);
		if(ul_Index == BIG_C_InvalidIndex)
		{
			/* create new */
			pst_Prefab = Prefab_pst_BuildFromSel(_pst_Sel);
			if ( mst_Ini.i_MoveToPrefabMap )
				Prefab_MoveToMap( _pst_World, pst_Prefab );
			Prefab_CreateNew(ul_Dir, sz_Name, pst_Prefab);
			mpo_TreeView->AddPrefab(ul_Dir, pst_Prefab->ul_Index, TRUE);
			Prefab_Destroy(&pst_Prefab);
			break;
		}
		else
		{
			if
			(
				M_MF()->MessageBox
					(
						"This prefab file exist already.\nDo you want to Merge data ?\n(select yes to merge, no to choose another name)",
						"!?! Warning !?!",
						MB_ICONEXCLAMATION | MB_YESNO
					) == IDYES
			)
			{
				pst_Prefab = Prefab_pst_BuildFromSel(_pst_Sel);
				if ( mst_Ini.i_MoveToPrefabMap )
					Prefab_MoveToMap( _pst_World, pst_Prefab );
				pst_MergeTo = Prefab_pst_Load(ul_Index, NULL);
				Prefab_Merge(pst_MergeTo, pst_Prefab);
				mi_PrefabModif = 1;
				mpo_DataView->Display(pst_MergeTo);
				mpo_TreeView->SelFile(pst_MergeTo->ul_Index);
				Prefab_Destroy(&pst_Prefab);
				Prefab_Destroy(&pst_MergeTo);

				/* merge */
				break;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::MergePrefab(WOR_tdst_World *_pst_World, SEL_tdst_Selection *_pst_Sel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Prefab *pst_Prefab;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mul_FileIndex == BIG_C_InvalidIndex) return;
	if(!mpst_Prefab) mpst_Prefab = Prefab_pst_Load(mul_FileIndex, NULL);
	if(!mpst_Prefab) return;
	pst_Prefab = Prefab_pst_BuildFromSel(_pst_Sel);
	if ( mst_Ini.i_MoveToPrefabMap )
		Prefab_MoveToMap( _pst_World, pst_Prefab );
	Prefab_Merge(mpst_Prefab, pst_Prefab);
	mi_PrefabModif = 1;
	mpo_DataView->Display(mpst_Prefab);
	Prefab_Destroy(&pst_Prefab);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::PrefabSel(F3D_cl_View *po_View, WOR_tdst_World *_pst_World, SEL_tdst_Selection *_pst_Sel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						l_Count;
	SEL_tdst_SelectedItem	*pst_Item;
	OBJ_tdst_GameObject		*pst_GO;
	CList<BIG_KEY, BIG_KEY> o_Lst;
	TAB_tdst_PFelem			*pst_PFElem, *pst_PFLastElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Count = SEL_l_CountItem(_pst_Sel, SEL_C_SIF_Object);
	if(!l_Count) return;
	pst_Item = SEL_pst_GetFirst(_pst_Sel, SEL_C_SIF_Object);
	while(pst_Item)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_Item->p_Content;
		if(pst_GO->ul_PrefabKey && pst_GO->ul_PrefabKey != BIG_C_InvalidKey)
		{
			if(!o_Lst.Find(pst_GO->ul_PrefabKey)) o_Lst.AddTail(pst_GO->ul_PrefabKey);
		}

		pst_Item = SEL_pst_GetNext(_pst_Sel, pst_Item, SEL_C_SIF_Object);
	}

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(!pst_GO->ul_PrefabKey || pst_GO->ul_PrefabKey == BIG_C_InvalidKey) continue;
		if(o_Lst.Find(pst_GO->ul_PrefabKey)) po_View->Selection_Object(pst_GO, SEL_C_SIF_Object);
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::PrefabUpdate(WOR_tdst_World *_pst_World, SEL_tdst_Selection *_pst_Sel, CRect *_po_Rect )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						l_Count;
	SEL_tdst_SelectedItem	*pst_Item;
	OBJ_tdst_GameObject		*pst_GO;
	char					az[1024];
	CCheckList				o_Check;
	BOOL					ab_UpdateFlags[ 32 ];
	ULONG					ul_UpdateFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	l_Count = SEL_l_CountItem(_pst_Sel, SEL_C_SIF_Object);
	if(!l_Count) return;

	if(M_MF()->MessageBox("This will update all selected prefabs.\nDo you want to continue ?", "Warning", MB_YESNO) != IDYES) return;
	
	o_Check.AddString( "material" );
	o_Check.AddString( "geometry" );
	o_Check.AddString( "sound" );
	o_Check.AddString( "modifiers" );
	o_Check.AddString( "colmap" );
	o_Check.AddString( "colset" );
	o_Check.AddString( "design struct" );
	o_Check.AddString( "control flags" );
	o_Check.AddString( "custom bits" );
	o_Check.AddString( "shape and skeleton" );
	
	for (l_Count = 0; l_Count < 32; l_Count++ ) ab_UpdateFlags[ l_Count ] = 1;

	_po_Rect->left += (_po_Rect->Width() / 2) - 100;
	_po_Rect->top += (_po_Rect->Height() / 2) - 100;	
	o_Check.SetArrayBool( ab_UpdateFlags );

	// Si on a appuyé sur ESC, on sort
	if (o_Check.Do( _po_Rect, 200, this, 20, "update flags") == FALSE)
		return;

	// Sinon update
	else
	{
		ul_UpdateFlags = 0; 
		for (l_Count = 0; l_Count < 32; l_Count++ ) ul_UpdateFlags |= ab_UpdateFlags[ l_Count ] << l_Count;
		
		pst_Item = SEL_pst_GetFirst(_pst_Sel, SEL_C_SIF_Object);
		while(pst_Item)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_Item->p_Content;
			if(pst_GO->ul_PrefabObjKey && pst_GO->ul_PrefabObjKey != BIG_C_InvalidKey)
			{
				sprintf(az, "Update %s", pst_GO->sz_Name);
				LINK_PrintStatusMsg(az);
				OBJ_UpdateGaoGao(_pst_World, pst_GO, pst_GO->ul_PrefabObjKey, ul_UpdateFlags);
			}

			pst_Item = SEL_pst_GetNext(_pst_Sel, pst_Item, SEL_C_SIF_Object);
		}
		
		LINK_Refresh();
	}	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EPFB_cl_Frame::UpdateSelType(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CWnd		*po_Wnd;
	CListCtrl	*po_LC;
	int			i_Item;
	/*~~~~~~~~~~~~~~~~*/

	mi_SelType = EPFB_SelType_Nothing;

	po_Wnd = mpo_TreeView->GetDlgItem(IDC_TREE_PREFAB);
	if(mpo_Splitter->GetActivePane() == mpo_TreeView)
	{
		if(mb_SelIsDir && (mul_DirIndex != BIG_C_InvalidIndex)) mi_SelType = EPFB_SelType_TreeDir;
		if(!mb_SelIsDir && (mul_FileIndex != BIG_C_InvalidIndex)) mi_SelType = EPFB_SelType_TreePrefab;
	}
	else if(mpst_Prefab)
	{
		po_LC = (CListCtrl *) mpo_DataView->GetDlgItem(IDC_LISTCTRL_REF);
		i_Item = po_LC->GetNextItem(-1, LVNI_SELECTED);
		if(i_Item != -1)
		{
			if(mpst_Prefab->dst_Ref[i_Item].uc_Type == OBJPREFAB_C_RefIsGao)
				mi_SelType = EPFB_SelType_RefGao;
			else if(mpst_Prefab->dst_Ref[i_Item].uc_Type == OBJPREFAB_C_RefIsPrefab)
				mi_SelType = EPFB_SelType_RefPrefab;
		}
	}

	return mi_SelType;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::CommonVss(int _i_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_Item;
	char			asz_Path[BIG_C_MaxLenPath + BIG_C_MaxLenName];
	char			*psz_Name;
	BOOL			b_File;
	ULONG			ul_File;
	CListCtrl		*po_LC;
	EBRO_cl_Frame	*po_Browser;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
	if(!po_Browser) return;

	UpdateSelType();
	if(mi_SelType == EPFB_SelType_Nothing) return;

	if(mi_SelType == EPFB_SelType_TreeDir)
	{
		b_File = FALSE;
		BIG_ComputeFullName(mul_DirIndex, asz_Path);
	}
	else
	{
		if(mi_SelType == EPFB_SelType_TreePrefab)
			ul_File = mul_FileIndex;
		else
		{
			po_LC = (CListCtrl *) mpo_DataView->GetDlgItem(IDC_LISTCTRL_REF);
			i_Item = po_LC->GetNextItem(-1, LVNI_SELECTED);
			if(i_Item != -1)
				ul_File = mpst_Prefab->dst_Ref[i_Item].ul_Index;
			else
				return;
		}

		b_File = TRUE;
		BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Path);
	}

	//EDI_go_MsgTruncateFiles.mb_ApplyToAll = FALSE;
	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;

	/* For history */
	if(_i_Type == 1)
	{
		if
		(
			M_MF()->MessageBox
				(
					"You will update linked bigfile",
					EDI_STR_Csz_TitleConfirm,
					MB_ICONQUESTION | MB_OKCANCEL
				) != IDOK
		)
		{
			return;
		}
	}

	AfxGetApp()->DoWaitCursor(1);

	_Try_

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Request for a file.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* mb_NoUpdateVss = TRUE; */
	if(b_File)
	{
		psz_Name = BIG_NameFile(ul_File);

		switch(_i_Type)
		{
		case 1:
			EDI_gb_CheckInDel = FALSE;
			M_MF()->CheckInFile(asz_Path, psz_Name);
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
			break;

		case 2:
			M_MF()->CheckOutFile(asz_Path, psz_Name);
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
			break;

		case 3:
			M_MF()->UndoCheckOutFile(asz_Path, psz_Name);
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
			break;

		case 4:
			/* mb_NoUpdateVss = TRUE; */
			M_MF()->GetLatestVersionFile(asz_Path, psz_Name);
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
			break;

		case 5:
			/* mb_NoUpdateVss = TRUE; */
			EDI_gb_CheckInDel = FALSE;
			M_MF()->CheckInFileNoOut(asz_Path, psz_Name);
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
			break;
		}

		EDI_gb_NoUpdateVSS = FALSE;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Request for a directory.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else
	{
		switch(_i_Type)
		{
		case 1:
			EDI_gb_RecurseVss = po_Browser->mst_Ini.mst_Options.b_RecIn;
			M_MF()->CheckInDir(asz_Path);
			EDI_gb_NoUpdateVSS = TRUE;
			EDI_gb_CheckInDel = FALSE;
			LINK_gb_RefreshOnlyFiles = TRUE;
			break;

		case 2:
			EDI_gb_RecurseVss = po_Browser->mst_Ini.mst_Options.b_RecOut;
			EDI_gb_GetLatestDel = po_Browser->mst_Ini.mst_Options.b_MirrorGetLatest;
			M_MF()->CheckOutDir(asz_Path);
			EDI_gb_NoUpdateVSS = TRUE;
			EDI_gb_GetLatestDel = FALSE;
			if(!po_Browser->mst_Ini.mst_Options.b_MirrorGetLatest) LINK_gb_RefreshOnlyFiles = TRUE;
			break;

		case 3:
			EDI_gb_RecurseVss = po_Browser->mst_Ini.mst_Options.b_RecUndoOut;
			M_MF()->UndoCheckOutDir(asz_Path);
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
			break;

		case 4:
			EDI_gb_RecurseVss = po_Browser->mst_Ini.mst_Options.b_RecGet;
			EDI_gb_GetLatestDel = po_Browser->mst_Ini.mst_Options.b_MirrorGetLatest;
			M_MF()->GetLatestVersionDir(asz_Path);
			EDI_gb_NoUpdateVSS = TRUE;
			EDI_gb_GetLatestDel = FALSE;
			break;

		case 5:
			EDI_gb_RecurseVss = po_Browser->mst_Ini.mst_Options.b_RecIn;
			M_MF()->CheckInDirNoOut(asz_Path);
			EDI_gb_CheckInDel = FALSE;
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
			break;

		case 6:
			EDI_gb_RecurseVss = po_Browser->mst_Ini.mst_Options.b_RecOut;
			M_MF()->LocalCheckOutDir(asz_Path);
			EDI_gb_NoUpdateVSS = TRUE;
			LINK_gb_RefreshOnlyFiles = TRUE;
			break;
		}

		EDI_gb_NoUpdateVSS = FALSE;
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	_Catch_ _End_	LINK_PrintStatusMsg("OK");
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AfxGetApp()->DoWaitCursor(-1);
	M_MF()->FatHasChanged();
	LINK_gb_EscapeDetected = FALSE;
	EDI_gb_NoUpdateVSS = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::Prefab_MoveToMap( WOR_tdst_World *_pst_World, OBJ_tdst_Prefab *_pst_Prefab )
{
	EDIA_cl_FileDialog	o_DialogFile("Choose world File", 0, 0, 1, NULL, "*"EDI_Csz_ExtWorld );
	ULONG				ul_Dir, ul_File;
	CString				o_Temp;
	
	ul_File = mst_Ini.ul_PrefabMapIndex;
	
	if ( ul_File != BIG_C_InvalidIndex )
	{
		ul_Dir = BIG_ParentFile( ul_File );
		BIG_ComputeFullName( ul_Dir, o_DialogFile.masz_FullPath );
	}
		
	
	if ( o_DialogFile.DoModal() != IDOK )
		return;
		
	o_DialogFile.GetItem(o_DialogFile.mo_File, 0, o_Temp);
	ul_File = BIG_ul_SearchFileExt(o_DialogFile.masz_FullPath, (char *) (LPCSTR) o_Temp);
	if (ul_File == BIG_C_InvalidIndex) return;
	mst_Ini.ul_PrefabMapIndex = ul_File;
	ul_Dir = BIG_ParentFile( ul_File );
	
	if ( !BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtWorld ) )
		return;
	
		
	Prefab_MoveToMapExt( _pst_World, _pst_Prefab, TRUE, ul_Dir, ul_File );
	Prefab_MoveToMapExt( _pst_World, _pst_Prefab, FALSE, ul_Dir, ul_File );

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::Prefab_MoveToMap_MoveFile( ULONG _ul_Index, char *_sz_NewPath, char *_sz_WorldPath )
{
	char	sz_PathSrc[ BIG_C_MaxLenPath ];
	char	sz_PathDst[ BIG_C_MaxLenPath ];
	
	if ((_ul_Index == 0) || (_ul_Index == BIG_C_InvalidIndex ) )
		return;
	
	BIG_ComputeFullName( BIG_ParentFile(_ul_Index), sz_PathSrc );
	
    if ( !L_strnicmp( _sz_WorldPath, sz_PathSrc, strlen( _sz_WorldPath ) ) )
    {
		sprintf( sz_PathDst, "%s%s", _sz_NewPath, sz_PathSrc + strlen( _sz_WorldPath ) );
		BIG_ul_CreateDir( sz_PathDst );
    	BIG_MoveFile( sz_PathDst, sz_PathSrc, BIG_NameFile( _ul_Index ) );
    }
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::Prefab_MoveToMap_MoveObject( OBJ_tdst_GameObject *_pst_GO, char *_sz_NewPath, char *_sz_WorldPath )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct		    *pst_Gro;
	COL_tdst_ColMap		    *pst_ColMap;
	COL_tdst_Instance	    *pst_Zdm;
	BIG_INDEX			    ul_Index;
	BIG_INDEX				ul_Bank;
	int					    i;
    AI_tdst_Instance        *pst_AI;
    ULONG					*pul_Data;
    char					sz_PathSrc[ 260 ];
    char					sz_PathDst[ 260 ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* init */
	ul_Bank = BIG_ul_SearchDir( EDI_Csz_Path_Objects );
	
	if(!_pst_GO) return;
	ul_Index = LOA_ul_SearchIndexWithAddress( (ULONG) _pst_GO );
	if (ul_Index == BIG_C_InvalidIndex) return;
	BIG_ComputeFullName( BIG_ParentFile(ul_Index), sz_PathSrc );
	
	sprintf( sz_PathDst, "%s/"EDI_Csz_Path_GameObject, _sz_NewPath );
	BIG_MoveFile( sz_PathDst, sz_PathSrc, BIG_NameFile( ul_Index ) );

	if(_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Visu)
	{
        if ( !(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims))
        {
    		pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
		    if(pst_Gro && ((ul_Index = LOA_ul_SearchIndexWithAddress( (ULONG) pst_Gro )) != BIG_C_InvalidIndex) )
    			Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );

		    pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Material;
		    if(pst_Gro && ((ul_Index = LOA_ul_SearchIndexWithAddress( (ULONG) pst_Gro )) != BIG_C_InvalidIndex) )
				Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
		    
		    pul_Data = _pst_GO->pst_Base->pst_Visu->dul_VertexColors;
		    if (pul_Data && ((ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pul_Data )) != BIG_C_InvalidIndex) )
			    Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
		}
	}

	if ( _pst_GO->pst_Extended )
	{
		if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Events) )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_GO->pst_Extended->pst_Events->pst_ListTracks );
			Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
			
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_GO->pst_Extended->pst_Events->pst_ListParam );
			Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
		}
		if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights) )
		{
			pst_Gro = _pst_GO->pst_Extended->pst_Light;
		    if(pst_Gro && ((ul_Index = LOA_ul_SearchIndexWithAddress( (ULONG) pst_Gro )) != BIG_C_InvalidIndex) )
				Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
		}

		if( (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Group) )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_GO->pst_Extended->pst_Group );
			Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
		}

		if ( (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap) && (_pst_GO->pst_Extended->pst_Col) )
		{
			pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;
			if(pst_ColMap && ((ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap)) != BIG_C_InvalidIndex) )
			{
				Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
				
				if(pst_ColMap->uc_NbOfCob)
				{
					for(i = 0; i < pst_ColMap->uc_NbOfCob; i++)
					{
						if(!pst_ColMap->dpst_Cob[i]) continue;
						ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap->dpst_Cob[i]);
						if(ul_Index == BIG_C_InvalidIndex) continue;
						Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );

						if(pst_ColMap->dpst_Cob[i]->pst_GMatList )
						{
							ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap->dpst_Cob[i]);
							Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
						}
					}
				}
			}
		}

		if ( (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ZDM) && (_pst_GO->pst_Extended->pst_Col) )
		{
			pst_Zdm = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
			if(pst_Zdm)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Zdm);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
				
					if(pst_Zdm->pst_ColSet)
					{
						ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Zdm->pst_ColSet);
						Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
					}
				}
			}
		}

		if ( (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) && (_pst_GO->pst_Extended->pst_Ai) )
		{
			pst_AI = (AI_tdst_Instance *) _pst_GO->pst_Extended->pst_Ai;
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI );
			Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
			
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_VarDes );
			Prefab_MoveToMap_MoveFile( ul_Index, _sz_NewPath, _sz_WorldPath );
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPFB_cl_Frame::Prefab_MoveToMapExt(WOR_tdst_World *_pst_World, OBJ_tdst_Prefab *_pst_Prefab, BOOL _b_BuildGrp, ULONG _ul_Dir, ULONG _ul_File )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX				ul_Index;
	int						i;
	EDI_tdst_DragDrop		st_DD;
	OBJ_tdst_Prefab			*P;
	static OBJ_tdst_Group	*pst_Group;
	static OBJ_tdst_Group	*pst_NewGroup;
	//OBJ_tdst_Group			st_Gol;
	ULONG					ul_GolIndex;
	static int				i_RefCount;
	OBJ_tdst_GameObject		*pst_GO;
	OBJ_tdst_GameObject		*pst_DupGO;
	int						i_First;
	TAB_tdst_PFelem			*pst_Cur, *pst_End;
	char					*pc_Buffer, *sz_Ext, sz_GroupName[ BIG_C_MaxLenName ];
	char					sz_PathPrefab[ BIG_C_MaxLenPath ];
	char					sz_WorldPath[ BIG_C_MaxLenPath ];
	ULONG					ul_Key, ul_Length, ul_WorldIndex;
	//WOR_tdst_World			*pst_PrefabWorld;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	P = _pst_Prefab;
	if(!P || !P->l_NbRef) return;

	/* Groups to update references */
	i_First = -1;
	i_RefCount = 0;
	if( _b_BuildGrp )
	{
		pst_Group = GRP_pst_CreateNewGroup();
		pst_NewGroup = GRP_pst_CreateNewGroup();
	}
	else
	{
		GRP_AfterLoaded( _pst_World, pst_Group);
	}

	/* Scan all group */
	for(i = 0; i < P->l_NbRef; i++)
	{
		ul_Index = P->dst_Ref[i].ul_Index;
		if(ul_Index == BIG_C_InvalidIndex) continue;

		if(P->dst_Ref[i].uc_Type != OBJPREFAB_C_RefIsGao) continue;
		pst_GO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress( BIG_PosFile( ul_Index ) );
		if (!pst_GO) return;

		if(_b_BuildGrp)
		{
			PrefabRef_b_UpdateBV(&P->dst_Ref[i]);
			OBJ_AddInGroup(pst_Group, pst_GO);
			i_RefCount++;
		}
		else
		{
			/* Create object */
			pst_DupGO = OBJ_GameObject_Duplicate(_pst_World, pst_GO, TRUE, TRUE, NULL, 0, NULL);
			pst_DupGO->ul_EditorFlags &= ~(OBJ_C_EditFlags_Hidden | OBJ_C_EditFlags_Selected);
			pst_DupGO->ul_PrefabKey = BIG_FileKey(_pst_Prefab->ul_Index);
			pst_DupGO->ul_PrefabObjKey = BIG_FileKey(ul_Index);
			OBJ_AddInGroup(pst_NewGroup, pst_GO);
			i_RefCount++;
			//P->dst_Ref[ i ].ul_Index = LOA_ul_SearchIndexWithAddress( (ULONG) pst_DupGO );
		}
	}

	if( _b_BuildGrp ) 
	{
		Prefab_b_ComputeBV(P, FALSE);
		Prefab_ul_Save(P);
		return;
	}

	/* Update all references */
	GRP_RepercuteHierarchy(pst_Group, pst_NewGroup);
	GRP_RepercuteAIRefs(pst_Group, pst_NewGroup);
	GRP_RepercuteEventsRefs(pst_Group, pst_NewGroup);

	/* move all gao of group into prefab world */
	pst_Cur = TAB_pst_PFtable_GetFirstElem( pst_NewGroup->pst_AllObjects );
	pst_End = TAB_pst_PFtable_GetLastElem( pst_NewGroup->pst_AllObjects );
	
	ul_WorldIndex = LOA_ul_SearchIndexWithAddress( (ULONG) _pst_World );
	BIG_ComputeFullName( BIG_ParentFile( ul_WorldIndex), sz_WorldPath );
	
	BIG_ComputeFullName( _ul_Dir, sz_PathPrefab );
	for(; pst_Cur <= pst_End; pst_Cur++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_Cur->p_Pointer;
		Prefab_MoveToMap_MoveObject( pst_GO, sz_PathPrefab, sz_WorldPath );
	}
	
	/* add object to prefab world */
	strcpy( sz_GroupName, BIG_NameFile( _ul_File ) );
	if ( sz_Ext = strrchr( sz_GroupName, '.' ) )
	{
		strcpy( sz_Ext, EDI_Csz_ExtGameObjects );
		ul_GolIndex = BIG_ul_SearchFile( _ul_Dir, sz_GroupName);
		if ( ul_GolIndex && (ul_GolIndex != BIG_C_InvalidIndex) )
		{
			pc_Buffer = BIG_pc_ReadFileTmp( BIG_PosFile( ul_GolIndex), &ul_Length );
			SAV_Begin( sz_PathPrefab, sz_GroupName );
			SAV_Buffer( pc_Buffer, ul_Length );
			
			pst_Cur = TAB_pst_PFtable_GetFirstElem( pst_NewGroup->pst_AllObjects );
			pst_End = TAB_pst_PFtable_GetLastElem( pst_NewGroup->pst_AllObjects );
			for(; pst_Cur <= pst_End; pst_Cur++)
			{
				ul_Key = LOA_ul_SearchKeyWithAddress( (ULONG) pst_Cur->p_Pointer );
				SAV_Buffer( &ul_Key, 4 );
				ul_Key = *(ULONG *) EDI_Csz_ExtGameObject; 
				SAV_Buffer( &ul_Key, 4 );
			}
			SAV_ul_End();
		}
	}
	
	GRP_DetachAndDestroyGAOs(_pst_World, pst_NewGroup, FALSE);
	
	/* free groups */
	OBJ_FreeGroup(pst_Group);
	OBJ_FreeGroup(pst_NewGroup);

	/* Parcequ'on a recalculé les matrices/BV */
	Prefab_b_ComputeBV(P, FALSE);
	Prefab_ul_Save(P);
	WOR_l_World_Save( _pst_World );
}


#endif /* ACTIVE_EDITORS */
