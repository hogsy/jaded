/*$T DIAcheckworld_dlg.cpp GC! 1.081 08/06/02 09:52:49 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAcheckworld_dlg.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORcheck.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/ENGVars.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "LINKs/LINKtoed.h"
#include "EDItors/Sources/AIscript/AIframe.h"
#include "EDItors/Sources/AIscript/EditView/AIview.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/TEXtures/TEXframe.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDImsg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIpaths.h"
#include "GEOmetric/GEOstaticlod.h"
#include "GRObject/GROmemstats.h"
#include "MATerial/MATSprite.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GAO.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_XMEN.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#ifdef JADEFUSION
#include "DIAlogs/DIAselection_dlg.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "SELection/SELection.h"
#endif

#ifndef JADEFUSION
extern "C"
{
    extern MDF_tdst_Modifier *GAO_ModifierPhoto_Get(OBJ_tdst_GameObject *, BOOL);
}
#else
    extern MDF_tdst_Modifier *GAO_ModifierPhoto_Get(OBJ_tdst_GameObject *, BOOL);
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_CheckWorld, EDIA_cl_BaseDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_WORLD, OnBtWorld)
	ON_BN_CLICKED(IDC_BUTTON_GO, OnBtGo)
#ifdef JADEFUSION
	ON_BN_CLICKED(IDC_BUTTON_3DVIEW, OnBt3DView)
#endif
	ON_BN_CLICKED(IDC_BUTTON_MEM, OnBtMem)
	ON_BN_CLICKED(IDC_BUTTON_LIST, OnBtList)
	ON_BN_CLICKED(IDC_RADIO_TEXTURES, OnRadio)
	ON_BN_CLICKED(IDC_RADIO_MATERIAUX, OnRadio)
	ON_BN_CLICKED(IDC_RADIO_GRO, OnRadio)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE, OnExpanded)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelect)
#ifdef JADEFUSION
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblClk)
#endif
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_CheckWorld::EDIA_cl_CheckWorld(WOR_tdst_World *W) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_CHECKWORLD)
{
	mpst_World = W;
	mi_TreeContent = 0;
	mi_TexFilter = EDIAC_FILTERALL;
    mi_TexFlags = 0;
}

/*
===================================================================================================
===================================================================================================
*/
#ifdef JADEFUSION
EDIA_cl_CheckWorld::~EDIA_cl_CheckWorld(void)
{
	if(mpo_View->mpo_CheckWorldDialog == this) 
		mpo_View->mpo_CheckWorldDialog = NULL;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_CheckWorld::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	/*~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnInitDialog();
	
	if ( mpst_World )
		SetWindowText( mpst_World->sz_Name );

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	po_Tree->SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);
	po_Tree->SetFont(&M_MF()->mo_Fnt);

	mi_IconGro = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicObject, 0);
	mi_IconGrm = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicMaterial, 0);
	mi_IconTex = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtTexture1, 0);
	mi_IconGao = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGameObject, 0);

	mul_Index = BIG_C_InvalidIndex;

#ifdef JADEFUSION
	//disable 3DView button
	GetDlgItem(IDC_BUTTON_3DVIEW)->EnableWindow(FALSE);
#endif
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnDestroy(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_CheckWorld::PreTranslateMessage(MSG *p_Msg)
{
	if((p_Msg->message == WM_RBUTTONUP) && (p_Msg->hwnd == GetDlgItem(IDC_RADIO_TEXTURES)->GetSafeHwnd()))
	{
		Menu_Texture();
	}
	if ((p_Msg->message == WM_RBUTTONDOWN ) && (GetDlgItem(IDC_TREE)->GetSafeHwnd() == p_Msg->hwnd )) 
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		CTreeCtrl		*po_Tree;
		HTREEITEM		h_Item;
		UINT			ui_Dum;
		CPoint			pt( p_Msg->lParam );
		EMEN_cl_SubMenu o_Menu(FALSE);
		EOUT_cl_Frame	*po_Out;
		int				i_Res, I, SI;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		
		po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
		h_Item = po_Tree->HitTest(pt, &ui_Dum);
	    if(h_Item)
		{
			po_Tree->SelectItem(h_Item);
			
			/*$off*/
			M_MF()->InitPopupMenuAction(NULL, &o_Menu);
			if(mul_Index != BIG_C_InvalidIndex)
				M_MF()->AddPopupMenuAction( NULL, &o_Menu, 1, TRUE, "Show in browser", -1 );
			if (po_Tree->ItemHasChildren( h_Item ) )
				M_MF()->AddPopupMenuAction( NULL, &o_Menu, 2, TRUE, "Expand all", -1 );
			po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
			if (po_Out)
			{
				po_Tree->GetItemImage(h_Item, I, SI);
				if(I == mi_IconGao)
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, 3, TRUE, "Select in 3DView", -1 );
				else
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, 4, TRUE, "Select all gao in 3DView", -1 );
			}
			/*$on*/
			
			
			GetCursorPos(&pt);
			i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
			switch(i_Res)
			{
				case 1: OnBtGo();	break;
				case 2: ExpandRecurse( po_Tree, h_Item ); break;
				case 3:
				{
					po_Out->mpo_EngineFrame->mpo_DisplayView->ForceSelectObject( (void *) po_Tree->GetItemData(h_Item) );
					break;
				}
				case 4:
				{
					ExpandRecurse( po_Tree, h_Item );
					po_Out->mpo_EngineFrame->mpo_DisplayView->ForceSelectObject( NULL );
					SelectGaoRecurse( po_Tree, h_Item, po_Out );
					break;
				}
			}
		}
	}
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::ExpandRecurse( CTreeCtrl *_po_Tree, HTREEITEM _h_Item )
{
	HTREEITEM h_Child;
	
	_po_Tree->Expand( _h_Item, TVE_EXPAND );
	h_Child = _po_Tree->GetChildItem( _h_Item );
	while (h_Child != NULL)
	{
		if ( _po_Tree->ItemHasChildren( h_Child ) )
			ExpandRecurse( _po_Tree, h_Child );
		h_Child = _po_Tree->GetNextItem(h_Child, TVGN_NEXT);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::SelectGaoRecurse( CTreeCtrl *_po_Tree, HTREEITEM _h_Item, EOUT_cl_Frame *_po_Out )
{
	HTREEITEM	h_Child;
	int			I, SI;
	
	_po_Tree->Expand( _h_Item, TVE_EXPAND );
	h_Child = _po_Tree->GetChildItem( _h_Item );
	while (h_Child != NULL)
	{
		_po_Tree->GetItemImage(h_Child, I, SI);

		if (I == mi_IconGao)
			_po_Out->mpo_EngineFrame->mpo_DisplayView->ForceSelectObject( (void *) _po_Tree->GetItemData( h_Child ), 0);

		if ( _po_Tree->ItemHasChildren( h_Child ) )
			SelectGaoRecurse( _po_Tree, h_Child, _po_Out );
		
		h_Child = _po_Tree->GetNextItem(h_Child, TVGN_NEXT);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 300;
	lpMMI->ptMinTrackSize.y = 200;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::Menu_Texture(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	int				i_Res, i_OldFilter, i_OldFlags;
	POINT			pt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_OldFilter = mi_TexFilter;
    i_OldFlags = mi_TexFlags;

    /*$off*/
	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	M_MF()->AddPopupMenuAction( NULL, &o_Menu, 1, TRUE, "raw", DFCS_BUTTONCHECK | ((mi_TexFilter & EDIAC_FILTERRAW) ? DFCS_CHECKED : 0) );
	M_MF()->AddPopupMenuAction( NULL, &o_Menu, 2, TRUE, "tex", DFCS_BUTTONCHECK | ((mi_TexFilter & EDIAC_FILTERTEX) ? DFCS_CHECKED : 0) );
	M_MF()->AddPopupMenuAction( NULL, &o_Menu, 3, TRUE, "tga", DFCS_BUTTONCHECK | ((mi_TexFilter & EDIAC_FILTERTGA) ? DFCS_CHECKED : 0) );
    M_MF()->AddPopupMenuAction( NULL, &o_Menu, 4, TRUE, "really used", DFCS_BUTTONCHECK | ((mi_TexFlags & EDIAC_FLAGSTEXREALLYUSED) ? DFCS_CHECKED : 0) );
    M_MF()->AddPopupMenuAction( NULL, &o_Menu, 5, TRUE, "show used in texture editor", DFCS_BUTTONCHECK | ((mi_TexFlags & EDIAC_FLAGSTEXSHOWINEDITOR) ? DFCS_CHECKED : 0) );
    M_MF()->AddPopupMenuAction( NULL, &o_Menu, 6, TRUE, "show unused in texture editor", DFCS_BUTTONCHECK | ((mi_TexFlags & EDIAC_FLAGSTEXSHOWUNUSEDINEDITOR) ? DFCS_CHECKED : 0) );
    /*$on*/
    

	GetCursorPos(&pt);
	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	switch(i_Res)
	{
	case 1:
		if(mi_TexFilter & EDIAC_FILTERRAW)
			mi_TexFilter &= ~EDIAC_FILTERRAW;
		else
			mi_TexFilter |= EDIAC_FILTERRAW;
		break;
	case 2:
		if(mi_TexFilter & EDIAC_FILTERTEX)
			mi_TexFilter &= ~EDIAC_FILTERTEX;
		else
			mi_TexFilter |= EDIAC_FILTERTEX;
		break;
	case 3:
		if(mi_TexFilter & EDIAC_FILTERTGA)
			mi_TexFilter &= ~EDIAC_FILTERTGA;
		else
			mi_TexFilter |= EDIAC_FILTERTGA;
		break;
    case 4:
		if(mi_TexFlags & EDIAC_FLAGSTEXREALLYUSED)
			mi_TexFlags &= ~EDIAC_FLAGSTEXREALLYUSED;
		else
			mi_TexFlags |= EDIAC_FLAGSTEXREALLYUSED;
		break;
    case 5:
		if(mi_TexFlags & EDIAC_FLAGSTEXSHOWINEDITOR)
			mi_TexFlags &= ~EDIAC_FLAGSTEXSHOWINEDITOR;
		else
			mi_TexFlags |= EDIAC_FLAGSTEXSHOWINEDITOR;
		break;
    case 6:
		if(mi_TexFlags & EDIAC_FLAGSTEXSHOWUNUSEDINEDITOR)
			mi_TexFlags &= ~EDIAC_FLAGSTEXSHOWUNUSEDINEDITOR;
		else
			mi_TexFlags |= EDIAC_FLAGSTEXSHOWUNUSEDINEDITOR;
		break;
	}

	if((mi_TreeContent == EDIACW_Textures) && ( (mi_TexFilter != i_OldFilter) || (mi_TexFlags != i_OldFlags) ) )
	{
		mi_TreeContent = 0;
		OnRadio();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnSize(UINT a, int cx, int cy)
{
	/*~~~~~~~~~~~*/
	static	int h = 80;
	RECT	st_Rect;
	/*~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnSize(a, cx, cy);
	if(GetDlgItem(IDC_TREE)) 
	{
		GetDlgItem(IDC_EDIT_NAME)->GetWindowRect( &st_Rect );
		ScreenToClient( &st_Rect );
		st_Rect.right = cx - 5;

		GetDlgItem(IDC_EDIT_NAME)->MoveWindow( &st_Rect );
		
		GetDlgItem(IDC_EDIT_PATH)->GetWindowRect( &st_Rect );
		ScreenToClient( &st_Rect );
#ifdef JADEFUSION
		st_Rect.right = cx - 84;
#else
		st_Rect.right = cx - 34;
#endif
		GetDlgItem(IDC_EDIT_PATH)->MoveWindow( &st_Rect );
		
		GetDlgItem(IDC_BUTTON_GO)->GetWindowRect( &st_Rect );
		ScreenToClient( &st_Rect );
#ifdef JADEFUSION
		st_Rect.right = cx - 55;
		st_Rect.left = cx - 82;
#else
		st_Rect.right = cx - 5;
		st_Rect.left = cx - 32;
#endif
		GetDlgItem(IDC_BUTTON_GO)->MoveWindow( &st_Rect );
	
#ifdef JADEFUSION
		GetDlgItem(IDC_BUTTON_3DVIEW)->GetWindowRect( &st_Rect );
		ScreenToClient( &st_Rect );
		st_Rect.right = cx - 5;
		st_Rect.left = cx - 53;
		GetDlgItem(IDC_BUTTON_3DVIEW)->MoveWindow( &st_Rect );
#endif

		GetDlgItem(IDC_TREE)->MoveWindow(5, h, cx - 10, cy - (h + 5));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnBtWorld(void)
{
	WOR_Check(mpst_World);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnBtGo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EBRO_cl_Frame	*po_Browser;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mul_Index != BIG_C_InvalidIndex)
	{
		po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
		po_Browser->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(mul_Index), mul_Index);
	}
}

#ifdef JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnBt3DView(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EOUT_cl_Frame		*po_Output;
	LONG				l_Param;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
		return;

	l_Param = NULL;

	pst_GO = (OBJ_tdst_GameObject *)(LOA_ul_SearchAddress(BIG_PosFile(mul_Index)));
	if(!OBJ_IsValidGAO(pst_GO))
		return;

	po_Output = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	po_Output->mpo_EngineFrame->mpo_DisplayView->Selection_b_Treat((void *)pst_GO, l_Param, SEL_C_SIF_Object, FALSE);

	//update list in de Selection Dialog

	//auto-zoom on the selected object
	mpo_View->ForceSelectObject(pst_GO);
	mpo_View->ZoomExtendSelected
		(
		&mpo_View->mst_WinHandles.pst_DisplayData->st_Camera,
		mpo_View->mst_WinHandles.pst_World
		);
	//refresh selection list
	LINK_Refresh();
	//if(mpo_View->mpo_SelectionDialog) mpo_View->mpo_SelectionDialog->UpdateList();

}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnBtMem(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	int				i_Res;
	POINT			pt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Geometry", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Light", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "All", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Materials", -1);

	if(mul_Index != BIG_C_InvalidIndex)
	{
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 5, TRUE, "Selection", -1);
	}

	GetCursorPos(&pt);
	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	switch(i_Res)
	{
	case 1:
		/*{
			UCHAR	*End, *Mem;
			ULONG	SIZE;

			SIZE = ((LONG) 60 * (LONG) 1024 * (LONG) 1024);
			Mem = (UCHAR *) MEM_gst_MemoryInfo.pv_DynamicBloc;
			End = Mem + SIZE;
			while ( Mem < End )
			{
				if (*((ULONG *) Mem) == 0x4510E8F0)
					SIZE = 0;
				if (*((ULONG *) Mem) == 0x3c680000)
					SIZE = 0;
				Mem++;
			}
		}
		*/
		GRO_MemStat_Geo(&mpst_World->st_GraphicObjectsTable, NULL);
		break;
	case 2:
		GRO_MemStat_Light(&mpst_World->st_GraphicObjectsTable, NULL);
		break;
	case 3:
		GRO_MemStat_AllGro(&mpst_World->st_GraphicObjectsTable);
		break;
	case 4:
		GRO_MemStat_AllMat(&mpst_World->st_GraphicMaterialsTable);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnBtList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu		    o_Menu(FALSE);
	int					    i_Mode, i_Res;
	LONG				    l_Index;
	POINT				    pt;
	TAB_tdst_PFtable	    *PFtable;
	TAB_tdst_PFelem		    *pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject     *pst_Gao;
	char				    *cur, sz_Text[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Object with design struct", -1);
    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Object with info photo (LOG)", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Object with info photo (FILE)", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "All info photo", -1);

	GetCursorPos(&pt);
	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	if ( (i_Res == 1) || (i_Res == 4 ) )
	{
		i_Mode = i_Res;
		PFtable = &mpst_World->st_AllWorldObjects;
		pst_PFElem = TAB_pst_PFtable_GetFirstElem(PFtable);
		pst_PFLastElem = TAB_pst_PFtable_GetLastElem(PFtable);
		
		for(l_Index = 0, i_Res = 0; pst_PFElem <= pst_PFLastElem; pst_PFElem++, l_Index++)
		{
			pst_Gao = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
			if(TAB_b_IsAHole(pst_Gao)) continue;
			if(!pst_Gao || !pst_Gao->pst_Extended) continue;
			if((pst_Gao->pst_Extended->pst_Design) || (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_DesignStruct))
			{
				if (i_Mode == 1)
					i_Res++;
				else if (pst_Gao->pst_Extended->pst_Design->flags & 64 )
					i_Res++;
			}
		}

		sprintf(sz_Text, "====< %d GAO with design struct found >=====", i_Res);
		LINK_PrintStatusMsg(sz_Text);

		if(i_Res == 0) return;

		pst_PFElem = TAB_pst_PFtable_GetFirstElem(PFtable);
		for(l_Index = 0, i_Res = 0; pst_PFElem <= pst_PFLastElem; pst_PFElem++, l_Index++)
		{
			pst_Gao = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
			if(TAB_b_IsAHole(pst_Gao)) continue;
			if(!pst_Gao || !pst_Gao->pst_Extended) continue;
			if((pst_Gao->pst_Extended->pst_Design) || (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_DesignStruct))
			{
				if ( (i_Mode != 1) && ((pst_Gao->pst_Extended->pst_Design->flags & 64 ) == 0 ) )
					continue;
				i_Res++;
				cur = sz_Text + sprintf(sz_Text, "-%02d- ", i_Res);
				if(pst_Gao->pst_Extended->pst_Design)
				{
					cur += sprintf
						(
							cur,
							"(%08x %08x %08X) ",
							pst_Gao->pst_Extended->pst_Design->i1,
							pst_Gao->pst_Extended->pst_Design->i2,
							pst_Gao->pst_Extended->pst_Design->i3
						);
				}

				cur += sprintf(cur, "%s ", pst_Gao->sz_Name ? pst_Gao->sz_Name : "Unknown");

				if(!pst_Gao->pst_Extended->pst_Design)
					cur += sprintf(cur, "[has design struct flag but not design struct data]");
				else if(!(pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_DesignStruct))
					cur += sprintf(cur, "[has design struct data but not design struct flag]");

				LINK_PrintStatusMsg(sz_Text);
				
				if ( pst_Gao->pst_Extended->pst_Design )
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
                    int                 i;
					TEXT_tdst_OneText	*pst_Txt;
					TEXT_tdst_Eval		*T;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

					T = &pst_Gao->pst_Extended->pst_Design->st_Text1;
					if(T->i_FileKey != (int) BIG_C_InvalidIndex)
					{
                        i = TEXT_i_GetOneTextIndex( T->i_FileKey );
                        if(i == -1) 
                            pst_Txt = NULL;
                        else
                            pst_Txt = TEXT_gst_Global.pst_AllTexts[i];
						
						if ( pst_Txt )
						{
							LINK_PrintStatusMsg(pst_Txt->psz_Text + pst_Txt->pst_Ids[T->i_Id].i_Offset);
						}
					}
				}
			}
		}
	}
    else if (i_Res == 2)
        WOR_Check_InfoPhoto( mpst_World, 0 );
	else if (i_Res == 3 )
		WOR_Check_InfoPhoto( mpst_World, 1 );

}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnRadio(void)
{
	/*~~~~~~~~~~*/
	int i_Content;
	/*~~~~~~~~~~*/

	if(((CButton *) GetDlgItem(IDC_RADIO_TEXTURES))->GetCheck())
		i_Content = EDIACW_Textures;
	else if(((CButton *) GetDlgItem(IDC_RADIO_MATERIAUX))->GetCheck())
		i_Content = EDIACW_Materiaux;
	else if(((CButton *) GetDlgItem(IDC_RADIO_GRO))->GetCheck())
		i_Content = EDIACW_Graphics;
	else
		i_Content = 0;

	if(i_Content == mi_TreeContent) return;
	mi_TreeContent = i_Content;
	((CTreeCtrl *) GetDlgItem(IDC_TREE))->DeleteAllItems();

	switch(mi_TreeContent)
	{
	case EDIACW_Textures:
		FillTree_Textures();
		break;
	case EDIACW_Materiaux:
		FillTree_Materiaux();
		break;
	case EDIACW_Graphics:
		FillTree_Graphics();
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnExpanded(NMHDR *pNMTV2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	HTREEITEM	hItem, hChild;
	int			I, SI;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	NM_TREEVIEW *pNMTV;
	pNMTV = (NM_TREEVIEW *)pNMTV2;

	if(pNMTV->action != 2) return;

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	hItem = pNMTV->itemNew.hItem;
	hChild = po_Tree->GetChildItem(hItem);
	if(po_Tree->GetItemData(hChild) == (DWORD) - 2)
	{
		po_Tree->DeleteItem(hChild);

		po_Tree->GetItemImage(hItem, I, SI);
		if(I == mi_IconTex)
		{
			Expand_Textures(hItem);
		}
		else
		{
			Expand_Graphics(hItem);
			Expand_MultiMat(hItem);
		}

		po_Tree->SortChildren(hItem);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnSelect(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	int			i_Texture, I, SI;
	char		sz_Path[BIG_C_MaxLenPath];
	ULONG		ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NM_TREEVIEW *pNotifyStruct;
	pNotifyStruct = (NM_TREEVIEW *)pNotifyStruct2;

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);

	po_Tree->GetItemImage(pNotifyStruct->itemNew.hItem, I, SI);
	if(I == mi_IconTex)
	{
		i_Texture = po_Tree->GetItemData(pNotifyStruct->itemNew.hItem);
		ul_Key = TEX_gst_GlobalList.dst_Texture[i_Texture].ul_Key;
		mul_Index = BIG_ul_SearchKeyToFat(ul_Key);
	}
	else
	{
		ul_Key = po_Tree->GetItemData(pNotifyStruct->itemNew.hItem);
		mul_Index = LOA_ul_SearchIndexWithAddress(ul_Key);
	}

	if(mul_Index == BIG_C_InvalidIndex)
	{
		GetDlgItem(IDC_EDIT_KEY)->SetWindowText("");
		GetDlgItem(IDC_EDIT_NAME)->SetWindowText("");
		GetDlgItem(IDC_EDIT_PATH)->SetWindowText("");
	}
	else
	{
#ifdef JADEFUSION
		if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
		{
			//we have a valid key, try to find it in the loaded world
			//enable the 3DView button ?
			
			GetDlgItem(IDC_BUTTON_3DVIEW)->EnableWindow(b_IsLoaded(mul_Index));
		}
		else
			GetDlgItem(IDC_BUTTON_3DVIEW)->EnableWindow(FALSE);
#endif
		sprintf(sz_Path, "%08X", BIG_FileKey(mul_Index));
		GetDlgItem(IDC_EDIT_KEY)->SetWindowText(sz_Path);
		GetDlgItem(IDC_EDIT_NAME)->SetWindowText(BIG_NameFile(mul_Index));
		BIG_ComputeFullName(BIG_ParentFile(mul_Index), sz_Path);
		GetDlgItem(IDC_EDIT_PATH)->SetWindowText(sz_Path);
	}
}
#ifdef JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::OnDblClk(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject	*pst_GO;
	OBJ_tdst_GameObject	*pst_Gao;
	TAB_tdst_PFtable	*PFtable;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
		return;

	pst_GO = (OBJ_tdst_GameObject *)(LOA_ul_SearchAddress(BIG_PosFile(mul_Index)));

	//if it's in table of all Objects of the world, auto-zoom
	PFtable = &mpst_World->st_AllWorldObjects;
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(PFtable);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(PFtable);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_Gao = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_Gao)) continue;
		
		if(pst_GO  == pst_Gao)
		{
			OnBt3DView();
			return;
		}
	}
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::FillTree_Graphics(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl		*po_Tree;
	HTREEITEM		hItem;
	TAB_tdst_Ptable *Ptable;
	GRO_tdst_Struct **ppst_Gro, **ppst_LastGro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);

	Ptable = &mpst_World->st_GraphicObjectsTable;

	ppst_Gro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(Ptable);
	ppst_LastGro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(Ptable);

	for(; ppst_Gro <= ppst_LastGro; ppst_Gro++)
	{
		if(TAB_b_IsAHole(*ppst_Gro)) continue;

		hItem = po_Tree->InsertItem(GRO_sz_Struct_GetName(*ppst_Gro), mi_IconGro, mi_IconGro);
		po_Tree->SetItemData(hItem, (DWORD) * ppst_Gro);

#ifdef JADEFUSION
		//Expand subItem
		if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
			Expand_Graphics(hItem);
		else
#endif
		{
		hItem = po_Tree->InsertItem("Wait", -1, -1, hItem);
		po_Tree->SetItemData(hItem, (DWORD) - 2);
		}
	}

	po_Tree->SortChildren(TVI_ROOT);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::FillTree_Materiaux(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl		*po_Tree;
	HTREEITEM		hItem;
	TAB_tdst_Ptable *Ptable;
	GRO_tdst_Struct **ppst_Gro, **ppst_LastGro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);

	Ptable = &mpst_World->st_GraphicMaterialsTable;

	ppst_Gro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(Ptable);
	ppst_LastGro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(Ptable);

	for(; ppst_Gro <= ppst_LastGro; ppst_Gro++)
	{
		if(TAB_b_IsAHole(*ppst_Gro)) continue;

		hItem = po_Tree->InsertItem(GRO_sz_Struct_GetName(*ppst_Gro), mi_IconGrm, mi_IconGrm);
		po_Tree->SetItemData(hItem, (DWORD) * ppst_Gro);

#ifdef JADEFUSION
		//Expand subItem
		if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
			Expand_MultiMat(hItem);
#endif

		hItem = po_Tree->InsertItem("Wait", -1, -1, hItem);
		po_Tree->SetItemData(hItem, (DWORD) - 2);
	}

	po_Tree->SortChildren(TVI_ROOT);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
typedef struct CheckWorld_tdst_TexInfo_
{
    short   w_NbGO;
    short   w_NbGOMax;
    void    **p_GO;
} CheckWorld_tdst_TexInfo;

int CheckWorld_TexNumber;

void CheckWorld_RUT_AddGO( CheckWorld_tdst_TexInfo *_pst_Info, short _w_TextureId, void *_p_GO )
{
    CheckWorld_tdst_TexInfo *Inf;
    int i;

    if (_w_TextureId == 2)
        _w_TextureId = 2;

    if ( (_w_TextureId < 0) || (_w_TextureId >= CheckWorld_TexNumber) ) return;

    Inf = _pst_Info + _w_TextureId;

    /* déja dans la liste ?*/
    for (i = 0; i < Inf->w_NbGO; i++)
        if (Inf->p_GO[ i ] == _p_GO ) return;

    /* liste pleine */
    if (Inf->w_NbGO == Inf->w_NbGOMax)
    {
        Inf->w_NbGOMax += 64;
        if ( !Inf->w_NbGO )
            Inf->p_GO = (void **) L_malloc( 4 * 64 );
        else
            Inf->p_GO = (void **) L_realloc( Inf->p_GO, 4 * Inf->w_NbGOMax);
    }
    Inf->p_GO[ Inf->w_NbGO++ ] = _p_GO;
}

void CheckWorld_ReallyUsedTexture( CheckWorld_tdst_TexInfo *_pst_Info, void *_p_GO, GRO_tdst_Struct *_pst_Gro, GRO_tdst_Struct *_pst_Mat )
{
    GEO_tdst_Object             *pst_Geo;
    int                         i, t, mat; 

    if ( (!_pst_Mat) || (!_pst_Gro ) || ((ULONG) _pst_Mat == 0xFFFFFFFF) ) 
        return;
    
    if ( ((ULONG) _pst_Gro == 1) || (_pst_Gro->i->ul_Type == GRO_Geometric) )
    {
        if (_pst_Mat->i->ul_Type == GRO_MaterialMulti )
        {
            if ( (ULONG) _pst_Gro == 1)
            {
                for (i = 0; i < ((MAT_tdst_Multi *) _pst_Mat)->l_NumberOfSubMaterials; i++)
                    CheckWorld_ReallyUsedTexture( _pst_Info, _p_GO, _pst_Gro, (GRO_tdst_Struct *) ((MAT_tdst_Multi *) _pst_Mat)->dpst_SubMaterial[ i ] );
            }
            else
            {
                pst_Geo = (GEO_tdst_Object *) _pst_Gro;
                for (i = 0; i < pst_Geo->l_NbElements; i++)
                {
                    if ( !pst_Geo->dst_Element[ i ].l_NbTriangles ) continue;
                    mat = pst_Geo->dst_Element[ i ].l_MaterialId;
                    if ( mat >= ((MAT_tdst_Multi *) _pst_Mat)->l_NumberOfSubMaterials ) 
                        mat = ((MAT_tdst_Multi *) _pst_Mat)->l_NumberOfSubMaterials - 1;
                    CheckWorld_ReallyUsedTexture( _pst_Info, _p_GO, _pst_Gro, (GRO_tdst_Struct *) ((MAT_tdst_Multi *) _pst_Mat)->dpst_SubMaterial[ mat ] );
                }
            }
        }
        else
        {
            if ((ULONG) _pst_Gro == 1 )
                t = 1;
            else
            {
                pst_Geo = (GEO_tdst_Object *) _pst_Gro;
                for ( t = 0, i = 0; i < pst_Geo->l_NbElements; i++)
                    t += pst_Geo->dst_Element[i].l_NbTriangles;
            }
            
            if (t)
            {
                if ( _pst_Mat->i->ul_Type == GRO_MaterialSingle )
                    CheckWorld_RUT_AddGO( _pst_Info, (short) ((MAT_tdst_Single *)_pst_Mat)->l_TextureId, _p_GO );
                else if (_pst_Mat->i->ul_Type == GRO_MaterialMultiTexture)
                {
                    MAT_tdst_MTLevel	*MTL;
                    MTL = ((MAT_tdst_MultiTexture *) _pst_Mat)->pst_FirstLevel;
                    while( MTL)
                    {
                        CheckWorld_RUT_AddGO( _pst_Info, MTL->s_TextureId, _p_GO );
                        MTL = MTL->pst_NextLevel;
                    }
                }
            }
        }
        
    }
    else if ( _pst_Gro->i->ul_Type == GRO_GeoStaticLOD )
    {
        for (i = 0; i < ((GEO_tdst_StaticLOD *) _pst_Gro)->uc_NbLOD; i++)
            CheckWorld_ReallyUsedTexture( _pst_Info, _p_GO, ((GEO_tdst_StaticLOD *) _pst_Gro)->dpst_Id[ i ], _pst_Mat );
    }
    else if (_pst_Gro->i->ul_Type == GRO_2DText )
        CheckWorld_ReallyUsedTexture( _pst_Info, _p_GO, (GRO_tdst_Struct *) 1, _pst_Mat );
    else if (_pst_Gro->i->ul_Type == GRO_ParticleGenerator )
        CheckWorld_ReallyUsedTexture( _pst_Info, _p_GO, (GRO_tdst_Struct *) 1, _pst_Mat );
}

void EDIA_cl_CheckWorld::FillTree_Textures(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	                *po_Tree;
	HTREEITEM	                hTex, hItem;
	int			                i, j;
	ULONG		                ul_Key, ul_Index;
	char		                *sz_Ext;
    OBJ_tdst_GameObject         *pst_GO;
    CheckWorld_tdst_TexInfo     *pst_Info;
    TAB_tdst_PFelem		        *pst_First, *pst_Last;
    ETEX_cl_Frame               *po_TexEditor;
    int                         addineditor;
    MDF_tdst_Modifier           *pst_Mod;
    GAO_tdst_ModifierXMEN       *p_XMEN;
    SPG_tdst_Modifier           *p_SPG;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
    if ( !TEX_gst_GlobalList.l_NumberOfTextures) return;

    if ( mi_TexFlags & EDIAC_FLAGSTEXREALLYUSED )
    {
        CheckWorld_TexNumber = TEX_gst_GlobalList.l_NumberOfTextures;
        pst_Info = (CheckWorld_tdst_TexInfo *) L_malloc( TEX_gst_GlobalList.l_NumberOfTextures * sizeof( CheckWorld_tdst_TexInfo ) );
        L_memset( pst_Info, 0, TEX_gst_GlobalList.l_NumberOfTextures * sizeof( CheckWorld_tdst_TexInfo ) );
        
        pst_First = TAB_pst_PFtable_GetFirstElem( &mpst_World->st_AllWorldObjects);
	    pst_Last = TAB_pst_PFtable_GetLastElem( &mpst_World->st_AllWorldObjects );
        for(; pst_First <= pst_Last; pst_First++)
	    {
		    pst_GO = (OBJ_tdst_GameObject *) pst_First->p_Pointer;
            if(TAB_b_IsAHole(pst_GO)) continue;
            if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu) && !OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
		    {
    			CheckWorld_ReallyUsedTexture( pst_Info, pst_GO, pst_GO->pst_Base->pst_Visu->pst_Object, pst_GO->pst_Base->pst_Visu->pst_Material );
            }
            
            if (pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Modifiers )
            {
                pst_Mod = pst_GO->pst_Extended->pst_Modifiers;
                while ( pst_Mod )
                {
                    if (pst_Mod->i->ul_Type == MDF_C_Modifier_XMEN)
                    {
                        p_XMEN = (GAO_tdst_ModifierXMEN *)pst_Mod->p_Data;
                        if (p_XMEN->p_MaterialUsed)
                            CheckWorld_ReallyUsedTexture( pst_Info, pst_GO, (GRO_tdst_Struct *) 1, (GRO_tdst_Struct *) p_XMEN->p_MaterialUsed );
                    }
                    else if (pst_Mod->i->ul_Type == MDF_C_Modifier_SPG)
                    {
                        p_SPG = (SPG_tdst_Modifier *) pst_Mod->p_Data;
                        if ( p_SPG->p_Material) 
                            CheckWorld_ReallyUsedTexture( pst_Info, pst_GO, (GRO_tdst_Struct *) 1, (GRO_tdst_Struct *) p_SPG->p_Material );
                    }
                    pst_Mod = pst_Mod->pst_Next;
                }
            }
        }
    }

    po_TexEditor = NULL;
    if ( mi_TexFlags & EDIAC_FLAGSTEXSHOW)
    {
        po_TexEditor = (ETEX_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_TEXTURE, 0);
        if (po_TexEditor)
            po_TexEditor->DeleteList();
    }

	for(i = 0; i < TEX_gst_GlobalList.l_NumberOfTextures; i++)
	{
		ul_Key = TEX_gst_GlobalList.dst_Texture[i].ul_Key;

		ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
		if(ul_Index == BIG_C_InvalidIndex) continue;

		if(mi_TexFilter != EDIAC_FILTERALL)
		{
			sz_Ext = strrchr(BIG_NameFile(ul_Index), '.');
			if(sz_Ext)
			{
				sz_Ext++;
				if(((mi_TexFilter & EDIAC_FILTERRAW) == 0) && (L_stricmp(sz_Ext, "raw") == 0)) continue;
				if(((mi_TexFilter & EDIAC_FILTERTEX) == 0) && (L_stricmp(sz_Ext, "tex") == 0)) continue;
				if(((mi_TexFilter & EDIAC_FILTERTGA) == 0) && (L_stricmp(sz_Ext, "tga") == 0)) continue;
			}
		}

		hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconTex, mi_IconTex);
		po_Tree->SetItemData(hItem, i);

#ifdef JADEFUSION
		//Expand subItem
		if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
			Expand_Textures(hItem);
#endif

        if ( mi_TexFlags & EDIAC_FLAGSTEXREALLYUSED )
        {
            if (pst_Info[i].w_NbGO)
            {
                hTex = hItem;
                for (j = 0; j < pst_Info[i].w_NbGO; j++)
                {
                    hItem = po_Tree->InsertItem( ((OBJ_tdst_GameObject *) pst_Info[i].p_GO[j])->sz_Name, mi_IconGao, mi_IconGao, hTex);
                    po_Tree->SetItemData(hItem, (DWORD) pst_Info[i].p_GO[j]);
                }
                L_free( pst_Info[i].p_GO );
                addineditor = ( mi_TexFlags & EDIAC_FLAGSTEXSHOWINEDITOR );
            }
            else 
                addineditor = ( mi_TexFlags & EDIAC_FLAGSTEXSHOWUNUSEDINEDITOR );
            
        }
        else
        {
#ifdef JADEFUSION
			if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
#endif			
			{ 
			hItem = po_Tree->InsertItem("Wait", -1, -1, hItem);
	    	po_Tree->SetItemData(hItem, (DWORD) - 2);
			}
			addineditor = 1;
        
		}

        if (po_TexEditor && addineditor)
        {
            po_TexEditor->mul_DirIndex = BIG_ParentFile( ul_Index );
		    po_TexEditor->mul_FileIndex = ul_Index;
		    po_TexEditor->Browse(FALSE);
        }
	}

    if ( mi_TexFlags & EDIAC_FLAGSTEXREALLYUSED )
        L_free( pst_Info );

	po_Tree->SortChildren(TVI_ROOT);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::Expand_Graphics(HTREEITEM hItem)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl			    *po_Tree;
	HTREEITEM			    hChild;
	TAB_tdst_PFtable	    *PFtable;
	GRO_tdst_Struct		    *pst_Gro, *pst_Gro2, *pst_Gro3;
	TAB_tdst_PFelem		    *pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject     *pst_Gao;
	int					    i;
    MDF_tdst_Modifier       *pst_Mod;
    GAO_tdst_ModifierXMEN   *p_XMEN;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	pst_Gro = (GRO_tdst_Struct *) po_Tree->GetItemData(hItem);

	PFtable = &mpst_World->st_AllWorldObjects;
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(PFtable);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(PFtable);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_Gao = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_Gao)) continue;

		/* Check existence of reference object and add ref */
		if(pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)
		{
			if(pst_Gao->pst_Extended->pst_Light == pst_Gro)
			{
				hChild = po_Tree->InsertItem(pst_Gao->sz_Name, mi_IconGao, mi_IconGao, hItem);
				po_Tree->SetItemData(hChild, (DWORD) pst_Gao);
				continue;
			}
		}

		if(pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
		{
			if(pst_Gro2 = pst_Gao->pst_Base->pst_Visu->pst_Object)
			{
				if(pst_Gro2 == pst_Gro)
				{
					hChild = po_Tree->InsertItem(pst_Gao->sz_Name, mi_IconGao, mi_IconGao, hItem);
					po_Tree->SetItemData(hChild, (DWORD) pst_Gao);
					continue;
				}

				/* LOD */
				if(pst_Gro2->i->ul_Type == GRO_GeoStaticLOD)
				{
					for(i = 0; i < ((GEO_tdst_StaticLOD *) pst_Gro)->uc_NbLOD; i++)
					{
						pst_Gro3 = ((GEO_tdst_StaticLOD *) pst_Gro)->dpst_Id[i];
						if(pst_Gro3 == pst_Gro)
						{
							hChild = po_Tree->InsertItem
								(
									GRO_sz_Struct_GetName(pst_Gro2),
									mi_IconGrm,
									mi_IconGrm,
									hItem
								);
							po_Tree->SetItemData(hChild, (DWORD) pst_Gro2);
							hChild = po_Tree->InsertItem("Wait", -1, -1, hChild);
							po_Tree->SetItemData(hChild, (DWORD) - 2);
						}
					}
				}
			}

			if(pst_Gao->pst_Base->pst_Visu->pst_Material == pst_Gro)
			{
				hChild = po_Tree->InsertItem(pst_Gao->sz_Name, mi_IconGao, mi_IconGao, hItem);
				po_Tree->SetItemData(hChild, (DWORD) pst_Gao);
				continue;
			}
		}

        if (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject)
        {
            pst_Mod = pst_Gao->pst_Extended->pst_Modifiers;
            while ( pst_Mod )
            {
                if (pst_Mod->i->ul_Type == MDF_C_Modifier_XMEN)
                {
                    p_XMEN = (GAO_tdst_ModifierXMEN *) pst_Mod->p_Data;
                    if ( ((GRO_tdst_Struct *) p_XMEN->p_MaterialUsed) == pst_Gro)
                    {
                        hChild = po_Tree->InsertItem(pst_Gao->sz_Name, mi_IconGao, mi_IconGao, hItem);
				        po_Tree->SetItemData(hChild, (DWORD) pst_Gao);
                    }
                }
                pst_Mod = pst_Mod->pst_Next;
            }

        }
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::Expand_MultiMat(HTREEITEM hItem)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl		*po_Tree;
	HTREEITEM		hChild;
	TAB_tdst_Ptable *Ptable;
	GRO_tdst_Struct *pst_Gro;
	int				i;
	GRO_tdst_Struct **ppst_Gro, **ppst_LastGro;
	MAT_tdst_Multi	*pst_MM;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	pst_Gro = (GRO_tdst_Struct *) po_Tree->GetItemData(hItem);

	Ptable = &mpst_World->st_GraphicMaterialsTable;

	ppst_Gro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(Ptable);
	ppst_LastGro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(Ptable);
	for(; ppst_Gro <= ppst_LastGro; ppst_Gro++)
	{
		if(TAB_b_IsAHole(*ppst_Gro)) continue;

		if((*ppst_Gro)->i->ul_Type == GRO_MaterialMulti)
		{
			pst_MM = (MAT_tdst_Multi *) (*ppst_Gro);
			for(i = 0; i < pst_MM->l_NumberOfSubMaterials; i++)
			{
				if(pst_Gro == (GRO_tdst_Struct *) pst_MM->dpst_SubMaterial[i])
				{
					hChild = po_Tree->InsertItem
						(
							GRO_sz_Struct_GetName((GRO_tdst_Struct *) pst_MM),
							mi_IconGrm,
							mi_IconGrm,
							hItem
						);
					po_Tree->SetItemData(hChild, (DWORD) pst_MM);
					hChild = po_Tree->InsertItem("Wait", -1, -1, hChild);
					po_Tree->SetItemData(hChild, (DWORD) - 2);
					break;
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CheckWorld::Expand_Textures(HTREEITEM hItem)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl			*po_Tree;
	HTREEITEM			hChild;
	TAB_tdst_Ptable		*Ptable;
	int					i, i_Tex, i_Layer;
	GRO_tdst_Struct		**ppst_Gro, **ppst_LastGro;
	MAT_tdst_MTLevel	*pst_MTL;
	char				sz_Name[256];
	MAT_tdst_SpriteGen	*pst_SG;
	ULONG				ul_Key, ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	i_Tex = (int) po_Tree->GetItemData(hItem);

	Ptable = &mpst_World->st_GraphicMaterialsTable;

	ppst_Gro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetFirstElem(Ptable);
	ppst_LastGro = (GRO_tdst_Struct **) TAB_ppv_Ptable_GetLastElem(Ptable);
	for(; ppst_Gro <= ppst_LastGro; ppst_Gro++)
	{
		if(TAB_b_IsAHole(*ppst_Gro)) continue;
		if((*ppst_Gro)->i->ul_Type == GRO_MaterialMulti) continue;

		if((*ppst_Gro)->i->ul_Type == GRO_MaterialSingle)
		{
			if(((MAT_tdst_Single *) * ppst_Gro)->l_TextureId == i_Tex)
			{
				hChild = po_Tree->InsertItem
					(
						GRO_sz_Struct_GetName((GRO_tdst_Struct *) * ppst_Gro),
						mi_IconGrm,
						mi_IconGrm,
						hItem
					);
				po_Tree->SetItemData(hChild, (DWORD) (*ppst_Gro));
				hChild = po_Tree->InsertItem("Wait", -1, -1, hChild);
				po_Tree->SetItemData(hChild, (DWORD) - 2);
			}
		}
		else if((*ppst_Gro)->i->ul_Type == GRO_MaterialMultiTexture)
		{
			pst_MTL = ((MAT_tdst_MultiTexture *) * ppst_Gro)->pst_FirstLevel;

			i_Layer = 0;
			while(pst_MTL)
			{
				if(i_Tex == pst_MTL->s_TextureId)
				{
					sprintf(sz_Name, "%s [Layer %d]", GRO_sz_Struct_GetName((GRO_tdst_Struct *) * ppst_Gro), i_Layer);
					hChild = po_Tree->InsertItem(sz_Name, mi_IconGrm, mi_IconGrm, hItem);
					po_Tree->SetItemData(hChild, (DWORD) (*ppst_Gro));
					hChild = po_Tree->InsertItem("Wait", -1, -1, hChild);
					po_Tree->SetItemData(hChild, (DWORD) - 2);
				}

				pst_MTL = pst_MTL->pst_NextLevel;
				i_Layer++;
			}
		}
	}

	for(i = 0; i < TEX_gst_GlobalList.l_NumberOfTextures; i++)
	{
		ul_Key = BIG_C_InvalidKey;

		if(TEX_gst_GlobalList.dst_Texture[i].uw_Flags & TEX_uw_IsSpriteGen)
		{
			pst_SG = *(MAT_tdst_SpriteGen **) &TEX_gst_GlobalList.dst_Texture[i].w_Width;
			if(i_Tex == pst_SG->s_TextureIndex)
			{
				ul_Key = TEX_gst_GlobalList.dst_Texture[i].ul_Key;
			}
		}
		else if(TEX_gst_GlobalList.dst_Texture[i].uw_Flags & TEX_uw_RawPal)
		{
			if(i_Tex == (int) TEX_gst_GlobalList.dst_Texture[i].w_Height)
				ul_Key = TEX_gst_GlobalList.dst_Texture[i].ul_Key;
		}

		if(ul_Key != BIG_C_InvalidKey)
		{
			ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
			if(ul_Index == BIG_C_InvalidIndex) continue;
			hChild = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconTex, mi_IconTex, hItem);
			po_Tree->SetItemData(hChild, i);
			hChild = po_Tree->InsertItem("Wait", -1, -1, hChild);
			po_Tree->SetItemData(hChild, (DWORD) - 2);
		}
	}
}
#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/

BOOL EDIA_cl_CheckWorld::b_IsLoaded(BIG_KEY ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	CString				strName;
	int					i_Str;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	BIG_KEY				ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(!EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
		return FALSE;	

	ul_Key = BIG_FileKey(ul_Index);
	strName = BIG_NameFile(ul_Index);
	i_Str = strName.Find(EDI_Csz_ExtGameObject);
	if(i_Str == -1)
		return FALSE;	//not a GAO
	

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mpst_World->st_AllWorldObjects);
	if(!pst_PFElem)
		return FALSE;

	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mpst_World->st_AllWorldObjects);

	/* Search in list of objects of world */
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(ul_Key == pst_GO->ul_MyKey)
			return TRUE;
	}
	return FALSE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/

BOOL EDIA_cl_CheckWorld::b_Change_Selection(ULONG _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl			*po_Tree;
	HTREEITEM			hItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	hItem = NULL;	

	if(FindItem((DWORD) _ul_Index, hItem))
	{
		po_Tree->SelectItem(hItem);
		po_Tree->EnsureVisible(hItem);
		po_Tree->SetRedraw(TRUE);
		return TRUE;
	}
	else
	{
		LINK_PrintStatusMsg( "File not found in the actual hierarchy.");
		return FALSE;
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/

BOOL EDIA_cl_CheckWorld::FindItem(DWORD itemdata, HTREEITEM &foundat, HTREEITEM startnode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl			*po_Tree;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	
	if(po_Tree->GetCount() == 0)
		return FALSE;

	// start at root is required
	if ( startnode == NULL )
		return FindItem(itemdata,foundat,po_Tree->GetRootItem());

	if ( itemdata == (ULONG)po_Tree->GetItemData(startnode))
	{
		foundat = startnode;
		return TRUE;
	}

	// now recurse on all children of this node
	HTREEITEM hchild = po_Tree->GetNextItem(startnode,TVGN_CHILD);
	while (hchild != NULL && foundat == NULL ) 
	{
		if(FindItem(itemdata,foundat,hchild))
			return TRUE;
		else
			hchild = po_Tree->GetNextSiblingItem(hchild);
	}
	//pass to the next sibling
	hchild = po_Tree->GetNextSiblingItem(startnode);
	if(hchild != NULL)
		return FindItem(itemdata,foundat, hchild);
	else
		return FALSE;

}
#endif

#endif /* ACTIVE_EDITORS */
