/*$T DIAgaoinfo_dlg.cpp GC! 1.081 08/01/02 17:20:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAgaoinfo_dlg.h"
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
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "Sound/Sources/SNDstruct.h"
#include "EDItors/Sources/PERForce/PERmsg.h"
#include "DATaControl/DATCPerforce.h"

#define GAOINFO_ACTION_PERFORCE_EDITCHILD		1
#define GAOINFO_ACTION_PERFORCE_REVERTCHILD		2

#define GAOINFO_ACTION_PERFORCE_EDIT			3
#define GAOINFO_ACTION_PERFORCE_REVERT			4

#ifdef JADEFUSION
#define GAOINFO_ACTION_PERFORCE_SHOWHISTORY		5

#define GAOINFO_ACTION_ADD_REJECTION_LIST		6
#endif

#define GAOINFO_TEXT_PERFORCE_EDITCHILD			"Perforce - Check Out (+ children)"
#define GAOINFO_TEXT_PERFORCE_REVERTCHILD		"Perforce - Undo Check Out (+ children)"

#define GAOINFO_TEXT_PERFORCE_EDIT				"Perforce - Check Out "
#define GAOINFO_TEXT_PERFORCE_REVERT			"Perforce - Undo Check Out"
#ifdef JADEFUSION
#define GAOINFO_TEXT_PERFORCE_SHOWHISTORY		"Perforce - Show History"
#define GAOINFO_TEXT_ADD_REJECTION_LIST			"Add to rejection list"
#endif
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_GaoInfo, EDIA_cl_BaseDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_GO, OnBtGo)
#ifdef JADEFUSION
	ON_BN_CLICKED(IDC_BUTTON_3DVIEW, OnBt3DVIew)
	ON_BN_CLICKED(IDC_RADIO_TYPE1, OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_TYPE2, OnBnClickedRadio)
#endif
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelect)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
EDIA_cl_GaoInfo::EDIA_cl_GaoInfo(OBJ_tdst_GameObject *O, int _i_InfoType) :
#else
EDIA_cl_GaoInfo::EDIA_cl_GaoInfo(OBJ_tdst_GameObject *O) :
#endif
	EDIA_cl_BaseDialog(DIALOGS_IDD_CHECKGAMEOBJECT)
{
	mpst_Gao = O;
	mi_TreeContent = 0;
#ifdef JADEFUSION
	mi_InfoType = _i_InfoType;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GaoInfo::~EDIA_cl_GaoInfo()
{
   	if(((F3D_cl_View *) mpo_View)->mpo_GaoInfoDialog == this) 
        ((F3D_cl_View *) mpo_View)->mpo_GaoInfoDialog = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GaoInfo::ChangeGao(struct OBJ_tdst_GameObject_ *_pst_Gao)
{
	/*~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	/*~~~~~~~~~~~~~~~~~*/

	mpst_Gao = _pst_Gao;
	FillTree();
	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	po_Tree->SelectItem(po_Tree->GetRootItem());
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GaoInfo::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	/*~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnInitDialog();

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	po_Tree->SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);
	po_Tree->SetFont(&M_MF()->mo_Fnt);

	mi_IconGro = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicObject, 0);
	mi_IconGrm = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicMaterial, 0);
	mi_IconTex = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtTexture1, 0);
	mi_IconGao = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGameObject, 0);
	mi_IconGrl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicLight, 0);
	mi_IconMap = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLMap, 0);
	mi_IconCob = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLObject, 0);
	mi_IconGam = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLGMAT, 0);
	mi_IconCin = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLInstance, 0);
	mi_IconCmd = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLSetModel, 0);
    mi_IconSkl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtSkeleton, 0);
    mi_IconShp = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtShape, 0);
    mi_IconAck = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtActionKit, 0);
    mi_IconTrl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtEventAllsTracks, 0);
    mi_IconOin = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtAIEngineInstance, 0);
	mi_IconOva = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtAIEngineVars, 0 );
    mi_IconMdl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtAIEditorModel, 0);
    mi_IconSnk = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtSoundBank, 0);
    mi_IconMsk = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtSoundMetaBank, 0);
	mi_IconGrp = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtObjGroups, 0);
	mi_IconRli = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGameObjectRLI, 0);
	mi_IconPfb = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtPrefab, 0);

	mul_Index = BIG_C_InvalidIndex;

#ifdef JADEFUSION
	//set radio button
	CheckRadioButton(IDC_RADIO_TYPE1, IDC_RADIO_TYPE2, IDC_RADIO_TYPE1 + mi_InfoType);
#endif

	FillTree();

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
void EDIA_cl_GaoInfo::OnDestroy(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GaoInfo::PreTranslateMessage(MSG *p_Msg)
{
#ifdef JADEFUSION
	OBJ_tdst_GameObject * pst_Light;
#endif

	if((p_Msg->message == WM_KEYDOWN) && (p_Msg->wParam == VK_ESCAPE))
	{
		OnCancel();
		return 1;
	}

	if ((p_Msg->message == WM_RBUTTONDOWN ) && (GetDlgItem(IDC_TREE)->GetSafeHwnd() == p_Msg->hwnd )) 
	{
		EMEN_cl_SubMenu o_Menu(FALSE);

		CTreeCtrl* po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);

		UINT uFlags;
		CPoint pt( p_Msg->lParam );
		HTREEITEM hItem = po_Tree->HitTest(pt, &uFlags);

		m_DataCtrlEmul.Clear();

		if(hItem)
		{
			po_Tree->SelectItem(hItem);

			M_MF()->InitPopupMenuAction(NULL, &o_Menu);

			if(mul_Index != BIG_C_InvalidIndex)
			{
#if 0 // remove p4 ~hogsy
				if ( po_Tree->ItemHasChildren(hItem) )
				{
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, GAOINFO_ACTION_PERFORCE_EDITCHILD, TRUE, GAOINFO_TEXT_PERFORCE_EDITCHILD, -1 );
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, GAOINFO_ACTION_PERFORCE_REVERTCHILD, TRUE, GAOINFO_TEXT_PERFORCE_REVERTCHILD, -1 );

					M_MF()->AddPopupMenuAction( NULL, &o_Menu, NULL, TRUE, "");
				}

				M_MF()->AddPopupMenuAction( NULL, &o_Menu, GAOINFO_ACTION_PERFORCE_EDIT, TRUE, GAOINFO_TEXT_PERFORCE_EDIT, -1 );
				M_MF()->AddPopupMenuAction( NULL, &o_Menu, GAOINFO_ACTION_PERFORCE_REVERT, TRUE, GAOINFO_TEXT_PERFORCE_REVERT, -1 );
#endif

#ifdef JADEFUSION
				M_MF()->AddPopupMenuAction( NULL, &o_Menu, GAOINFO_ACTION_PERFORCE_SHOWHISTORY, TRUE, GAOINFO_TEXT_PERFORCE_SHOWHISTORY, -1 );

				if(mi_InfoType == EDIAC_InfoType_IsLightedBy)
				{
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, NULL, TRUE, "");
					M_MF()->AddPopupMenuAction( NULL, &o_Menu, GAOINFO_ACTION_ADD_REJECTION_LIST, TRUE, GAOINFO_TEXT_ADD_REJECTION_LIST, -1 );
				}
#endif
				GetCursorPos(&pt);

				switch( M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu) )
				{
					case GAOINFO_ACTION_PERFORCE_EDIT: 
						m_DataCtrlEmul.AddIndex(mul_Index);
						LINK_SendMessageToEditors(EPER_MESSAGE_EDIT_SELECTED, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(&m_DataCtrlEmul))), 0);
						break;
					case GAOINFO_ACTION_PERFORCE_REVERT: 
						m_DataCtrlEmul.AddIndex(mul_Index);
						LINK_SendMessageToEditors(EPER_MESSAGE_REVERT_SELECTED, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(&m_DataCtrlEmul))), 0);
						break;
#ifdef JADEFUSION
					case GAOINFO_ACTION_PERFORCE_SHOWHISTORY:
						m_DataCtrlEmul.AddIndex(mul_Index);
						LINK_SendMessageToEditors(EPER_MESSAGE_SHOWHISTORY_SELECTED, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(&m_DataCtrlEmul))),0);
						break;
#endif
					case GAOINFO_ACTION_PERFORCE_EDITCHILD:
					{
						BIG_INDEX ulIndex;
						int Image, SelImage;
						HTREEITEM hLastItem;
#ifdef JADEFUSION
						HTREEITEM hRootItem = hItem;
#endif
						// Traverse the tree to get index of each file
						do 
						{
							do 
							{
								hLastItem = hItem;
								if ( hLastItem != TVI_ROOT )
								{
									po_Tree->GetItemImage(hLastItem, Image, SelImage);
									if(Image == mi_IconTex)
									{
										int iTexture = po_Tree->GetItemData(hLastItem);
										BIG_KEY ul_Key = TEX_gst_GlobalList.dst_Texture[iTexture].ul_Key;
										ulIndex = BIG_ul_SearchKeyToFat(ul_Key);
									}
									else if (Image == mi_IconPfb )
									{
										ulIndex = po_Tree->GetItemData(hLastItem);
									}
									else
									{
										ULONG pObject = po_Tree->GetItemData(hLastItem);
										ulIndex = LOA_ul_SearchIndexWithAddress(pObject);
									}
									m_DataCtrlEmul.AddIndex(ulIndex);
								}
							} while( ( hItem = po_Tree->GetChildItem(hLastItem) ) );

#ifdef JADEFUSION
							// Look for siblings or go up a level if not found
							while ( (! ( hItem = po_Tree->GetNextSiblingItem(hLastItem ))) && (hLastItem = hItem = po_Tree->GetParentItem(hLastItem)) ) 
							{
								// We've got back up to our starting point (hRootItem
								if (hItem == hRootItem)
								{
									hItem = NULL;
									break;
								}
							}
#else
							while ( (! ( hItem = po_Tree->GetNextSiblingItem(hLastItem ))) && (hLastItem = hItem = po_Tree->GetParentItem(hLastItem)) ) {}
#endif
						} while ( hItem );

						LINK_SendMessageToEditors(EPER_MESSAGE_EDIT_SELECTED, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(&m_DataCtrlEmul))), 0);

						break;
					}
					case GAOINFO_ACTION_PERFORCE_REVERTCHILD:
					{						
						BIG_INDEX ulIndex;
						int Image, SelImage;
						HTREEITEM hLastItem;
#ifdef JADEFUSION
						HTREEITEM hRootItem = hItem;
#endif
						// Traverse the tree to get index of each file
						do 
						{
							do 
							{
								hLastItem = hItem;
								if ( hLastItem != TVI_ROOT )
								{
									po_Tree->GetItemImage(hLastItem, Image, SelImage);
									if(Image == mi_IconTex)
									{
										int iTexture = po_Tree->GetItemData(hLastItem);
										BIG_KEY ul_Key = TEX_gst_GlobalList.dst_Texture[iTexture].ul_Key;
										ulIndex = BIG_ul_SearchKeyToFat(ul_Key);
									}
									else if (Image == mi_IconPfb )
									{
										ulIndex = po_Tree->GetItemData(hLastItem);
									}
									else
									{
										ULONG pObject = po_Tree->GetItemData(hLastItem);
										ulIndex = LOA_ul_SearchIndexWithAddress(pObject);
									}
									m_DataCtrlEmul.AddIndex(ulIndex);
								}
							} while( ( hItem = po_Tree->GetChildItem(hLastItem) ) );

#ifdef JADEFUSION
							// Look for siblings or go up a level if not found
							while ( (! ( hItem = po_Tree->GetNextSiblingItem(hLastItem ))) && (hLastItem = hItem = po_Tree->GetParentItem(hLastItem)) ) 
							{
								// We've got back up to our starting point (hRootItem
								if (hItem == hRootItem)
								{
									hItem = NULL;
									break;
								}
							}
#else
							while ( (! ( hItem = po_Tree->GetNextSiblingItem(hLastItem ))) && (hLastItem = hItem = po_Tree->GetParentItem(hLastItem)) ) {}
#endif
						} while ( hItem );

						LINK_SendMessageToEditors(EPER_MESSAGE_REVERT_SELECTED, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(&m_DataCtrlEmul))), 0);
						break;
					}
#ifdef JADEFUSION
					case GAOINFO_ACTION_ADD_REJECTION_LIST:
					{
						if(!mpst_Gao)
							break;

						if(mul_Index != BIG_C_InvalidIndex)
						{
							pst_Light = (OBJ_tdst_GameObject *)(LOA_ul_SearchAddress(BIG_PosFile(mul_Index)));
							if(!OBJ_IsValidGAO(pst_Light))
								break;
						}

						LRL_Add(pst_Light, mpst_Gao);
						
						//refresh 3dview
						LINK_Refresh();
						//update tree
						FillTree();
						break;	
					}
#endif
				}
			}
		}
		return 1;
	}
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GaoInfo::OnSize(UINT a, int cx, int cy)
{
	/*~~~~~~~~~~~*/
//#ifdef JADEFUSION
	static	int h = 120;
/*#else
	static	h = 80;
#endif*/
	/*~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnSize(a, cx, cy);
	if(!GetDlgItem(IDC_TREE)) return;
//#ifdef JADEFUSION
	GetDlgItem(IDC_EDIT_NAME)->MoveWindow(156, 3, cx - 164, 20);
	GetDlgItem(IDC_EDIT_PATH)->MoveWindow(45, 28, cx - 75, 20);
	GetDlgItem(IDC_BUTTON_GO)->MoveWindow(cx - 30, 28, 23, 20);
	GetDlgItem(IDC_EDIT_STATUS)->MoveWindow(45, 52, cx - 100, 20);
	GetDlgItem(IDC_BUTTON_3DVIEW)->MoveWindow(cx - 54, 52, 47, 20);
	GetDlgItem(IDC_TREE)->MoveWindow(8, h, cx - 14, cy - (h + 6));
	GetDlgItem(IDC_STATIC_INFO_TYPE)->MoveWindow(8, 72, cx - 14, 44);
/*#else
	GetDlgItem(IDC_EDIT_NAME)->MoveWindow(156, 6, cx - 166, 20);
	GetDlgItem(IDC_EDIT_PATH)->MoveWindow(45, 32, cx - 75, 20);
	GetDlgItem(IDC_BUTTON_GO)->MoveWindow(cx - 30, 32, 20, 20);
	GetDlgItem(IDC_EDIT_STATUS)->MoveWindow(45, 56, cx - 55, 20);
	GetDlgItem(IDC_TREE)->MoveWindow(5, h, cx - 10, cy - (h + 5));
#endif*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GaoInfo::OnSelect(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_Tree;
	int			i_Texture, I, SI;
	char		sz_Path[BIG_C_MaxLenPath];
	ULONG		ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NM_TREEVIEW *pNotifyStruct ;
	pNotifyStruct = (NM_TREEVIEW *)pNotifyStruct2;

#ifdef JADEFUSION
	//alway disable 3DView button
	GetDlgItem(IDC_BUTTON_3DVIEW)->EnableWindow(FALSE);
#endif

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);

	po_Tree->GetItemImage(pNotifyStruct->itemNew.hItem, I, SI);
	if(I == mi_IconTex)
	{
		i_Texture = po_Tree->GetItemData(pNotifyStruct->itemNew.hItem);
		ul_Key = TEX_gst_GlobalList.dst_Texture[i_Texture].ul_Key;
		mul_Index = BIG_ul_SearchKeyToFat(ul_Key);
	}
	else if (I == mi_IconPfb )
	{
		mul_Index = po_Tree->GetItemData(pNotifyStruct->itemNew.hItem);
	}
	else
	{
		ul_Key = po_Tree->GetItemData(pNotifyStruct->itemNew.hItem);
		mul_Index = LOA_ul_SearchIndexWithAddress(ul_Key);
#ifdef JADEFUSION
		if(mi_InfoType)
		{
			//enable only if it's a GAO
			GetDlgItem(IDC_BUTTON_3DVIEW)->EnableWindow(TRUE);
		}
#endif
	}

	if(mul_Index == BIG_C_InvalidIndex)
	{
		GetDlgItem(IDC_EDIT_KEY)->SetWindowText("");
		GetDlgItem(IDC_EDIT_NAME)->SetWindowText("");
		GetDlgItem(IDC_EDIT_PATH)->SetWindowText("");
		GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("");
#ifdef JADEFUSION
		GetDlgItem(IDC_BUTTON_3DVIEW)->EnableWindow(FALSE);
#endif
	}
	else
	{
		sprintf(sz_Path, "%08X", BIG_FileKey(mul_Index));
		GetDlgItem(IDC_EDIT_KEY)->SetWindowText(sz_Path);
		GetDlgItem(IDC_EDIT_NAME)->SetWindowText(BIG_NameFile(mul_Index));
		BIG_ComputeFullName(BIG_ParentFile(mul_Index), sz_Path);
		GetDlgItem(IDC_EDIT_PATH)->SetWindowText(sz_Path);

		if(*BIG_P4OwnerName(mul_Index) == 0 || !L_strnicmp(BIG_P4OwnerName(mul_Index), "_nobody_", 8))
			GetDlgItem(IDC_EDIT_STATUS)->SetWindowText("Not checked out");
		else
		{
			sprintf(sz_Path, "Checked out by %s", BIG_P4OwnerName(mul_Index));
			GetDlgItem(IDC_EDIT_STATUS)->SetWindowText(sz_Path);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GaoInfo::FillTree(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl			    *po_Tree;
	HTREEITEM			    h_Parent, hItem, hItemSub, hItemSubSub;
	GRO_tdst_Struct		    *pst_Gro;
	COL_tdst_ColMap		    *pst_ColMap;
	COL_tdst_Instance	    *pst_Zdm;
	BIG_INDEX			    ul_Index;
	int					    i;
    ANI_st_GameObjectAnim   *pst_Anim;
    AI_tdst_Instance        *pst_AI;
    SND_tdun_Main           *pst_Sound;
    ULONG					*pul_Data;
    char					sz_String[ 1024 ];
#ifdef JADEFUSION
	BOOL					b_Sep_Added;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	po_Tree->DeleteAllItems();

	if(!mpst_Gao) return;

	std::vector<BIG_KEY> vFstatKey;
	vFstatKey.push_back(mpst_Gao->ul_MyKey);	

	hItem = po_Tree->InsertItem(mpst_Gao->sz_Name ? mpst_Gao->sz_Name : "Unknow", mi_IconGao, mi_IconGao);
	po_Tree->SetItemData(hItem, (DWORD) mpst_Gao);

	

	h_Parent = hItem;

#ifdef JADEFUSION
	switch(mi_InfoType)
	{
	case EDIAC_InfoType_Normal:
#endif
	if(mpst_Gao->pst_Base && mpst_Gao->pst_Base->pst_Visu)
	{
        if (mpst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
        {
            pst_Anim = mpst_Gao->pst_Base->pst_GameObjectAnim;
            if (pst_Anim->pst_SkeletonModel)
            {
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_SkeletonModel );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					 vFstatKey.push_back(BIG_FileKey(ul_Index));	
				}

                hItem = po_Tree->InsertItem(pst_Anim->pst_SkeletonModel->sz_Name, mi_IconSkl, mi_IconSkl, h_Parent);
				

		    	po_Tree->SetItemData(hItem, (DWORD) pst_Anim->pst_SkeletonModel);
            }
            if (pst_Anim->pst_Shape)
            {
                ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_Shape);
                if(ul_Index != BIG_C_InvalidIndex)
                {
					vFstatKey.push_back(BIG_FileKey(ul_Index));	
                    hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconShp, mi_IconShp, h_Parent);
		    	    po_Tree->SetItemData(hItem, (DWORD) pst_Anim->pst_Shape);
                }
            }
            if (pst_Anim->apst_Anim[0] && pst_Anim->apst_Anim[0]->pst_Data && pst_Anim->apst_Anim[0]->pst_Data->pst_ListTracks)
            {
                ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->apst_Anim[0]->pst_Data->pst_ListTracks );
                if(ul_Index != BIG_C_InvalidIndex)
                {
					vFstatKey.push_back(BIG_FileKey(ul_Index));	
                    hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconTrl, mi_IconTrl, h_Parent);
		    	    po_Tree->SetItemData(hItem, (DWORD) pst_Anim->apst_Anim[0]->pst_Data->pst_ListTracks);
                }
            }
            if (pst_Anim->apst_Anim[1] && pst_Anim->apst_Anim[1]->pst_Data && pst_Anim->apst_Anim[1]->pst_Data->pst_ListTracks)
            {
                ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->apst_Anim[1]->pst_Data->pst_ListTracks );
                if(ul_Index != BIG_C_InvalidIndex)
                {
					vFstatKey.push_back(BIG_FileKey(ul_Index));	
                    hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconTrl, mi_IconTrl, h_Parent);
		    	    po_Tree->SetItemData(hItem, (DWORD) pst_Anim->apst_Anim[1]->pst_Data->pst_ListTracks);
                }
            }
            if (pst_Anim->pst_ActionKit)
            {
                ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_ActionKit );
                if(ul_Index != BIG_C_InvalidIndex)
                {
					vFstatKey.push_back(BIG_FileKey(ul_Index));	
                    hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconAck, mi_IconAck, h_Parent);
		    	    po_Tree->SetItemData(hItem, (DWORD) pst_Anim->pst_ActionKit);
                }
            }
        }
        else
        {
    		pst_Gro = mpst_Gao->pst_Base->pst_Visu->pst_Object;
		    if(pst_Gro)
    		{   
				BIG_KEY ulKey = LOA_ul_SearchKeyWithAddress((ULONG) pst_Gro);
				if ( ulKey != BIG_C_InvalidKey ) 
				{
					vFstatKey.push_back(ulKey);	
				}			
	    		hItem = po_Tree->InsertItem(GRO_sz_Struct_GetName(pst_Gro), mi_IconGro, mi_IconGro, h_Parent);
		    	po_Tree->SetItemData(hItem, (DWORD) pst_Gro);
#ifdef JADEFUSION
                    if (pst_Gro->i->ul_Type == GRO_GeoStaticLOD)
                    {
                        GEO_tdst_StaticLOD* pst_LOD    = (GEO_tdst_StaticLOD*)pst_Gro;
                        HTREEITEM           hParentLOD = hItem;

                        for (ULONG i = 0; i < pst_LOD->uc_NbLOD; ++i)
                        {
                            if (pst_LOD->dpst_Id[i] != NULL)
                            {
                                hItem = po_Tree->InsertItem(GRO_sz_Struct_GetName(pst_LOD->dpst_Id[i]), mi_IconGro, mi_IconGro, hParentLOD);
                                po_Tree->SetItemData(hItem, (DWORD)pst_LOD->dpst_Id[i]);
                            }
                        }

                        po_Tree->Expand(hParentLOD, TVE_EXPAND);
                    }
#endif
			}

		    pst_Gro = mpst_Gao->pst_Base->pst_Visu->pst_Material;
		    if(pst_Gro)
		    {
				BIG_KEY ulKey = LOA_ul_SearchKeyWithAddress((ULONG) pst_Gro);
				if ( ulKey != BIG_C_InvalidKey ) 
				{
					vFstatKey.push_back(ulKey);	
				}

			    hItem = po_Tree->InsertItem(GRO_sz_Struct_GetName(pst_Gro), mi_IconGrm, mi_IconGrm, h_Parent);
			    po_Tree->SetItemData(hItem, (DWORD) pst_Gro);

#ifdef JADEFUSION
				if(pst_Gro->i->ul_Type == GRO_MaterialMulti)
				{
					FillTreeMaterialMulti(pst_Gro, hItem);
				}
#endif			
			}
		    
		    pul_Data = mpst_Gao->pst_Base->pst_Visu->dul_VertexColors;
		    if (pul_Data)
		    {
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pul_Data );
                if(ul_Index != BIG_C_InvalidIndex)
                {
					vFstatKey.push_back(BIG_FileKey(ul_Index));	
                    hItem = po_Tree->InsertItem( BIG_NameFile(ul_Index), mi_IconRli, mi_IconRli, h_Parent);
		    	    po_Tree->SetItemData(hItem, (DWORD) pul_Data);
                }
		    }
		}
	}

	if ( mpst_Gao->pst_Extended )
	{
		if((mpst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Events) )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpst_Gao->pst_Extended->pst_Events->pst_ListTracks );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				vFstatKey.push_back(BIG_FileKey(ul_Index));	
				hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconTrl, mi_IconTrl, h_Parent);
		    	po_Tree->SetItemData(hItem, (DWORD) mpst_Gao->pst_Extended->pst_Events->pst_ListTracks);
			}

			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpst_Gao->pst_Extended->pst_Events->pst_ListParam );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				vFstatKey.push_back(BIG_FileKey(ul_Index));	
				hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconTrl, mi_IconTrl, h_Parent);
		    	po_Tree->SetItemData(hItem, (DWORD) mpst_Gao->pst_Extended->pst_Events->pst_ListParam);
			}
		}
		if((mpst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights) )
		{
			pst_Gro = mpst_Gao->pst_Extended->pst_Light;
			if(pst_Gro)
			{
				BIG_KEY ulKey = LOA_ul_SearchKeyWithAddress((ULONG) pst_Gro);
				if ( ulKey != BIG_C_InvalidKey ) 
				{
					vFstatKey.push_back(ulKey);	
				}

				hItem = po_Tree->InsertItem(GRO_sz_Struct_GetName(pst_Gro), mi_IconGrl, mi_IconGrl, h_Parent);
				po_Tree->SetItemData(hItem, (DWORD) pst_Gro);
			}
		}

		if( (mpst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Group) )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpst_Gao->pst_Extended->pst_Group );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				vFstatKey.push_back(BIG_FileKey(ul_Index));	
				hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconGrp, mi_IconGrp, h_Parent);
				po_Tree->SetItemData(hItem, (DWORD) mpst_Gao->pst_Extended->pst_Group );
			}
		}

		if ( (mpst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap) && (mpst_Gao->pst_Extended->pst_Col) )
		{
			pst_ColMap = ((COL_tdst_Base *) mpst_Gao->pst_Extended->pst_Col)->pst_ColMap;
			if(pst_ColMap)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					vFstatKey.push_back(BIG_FileKey(ul_Index));	
					hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconMap, mi_IconMap, h_Parent);
					po_Tree->SetItemData(hItem, (DWORD) pst_ColMap);

					if(pst_ColMap->uc_NbOfCob)
					{
						for(i = 0; i < pst_ColMap->uc_NbOfCob; i++)
						{
							if(!pst_ColMap->dpst_Cob[i]) continue;
							ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap->dpst_Cob[i]);
							if(ul_Index == BIG_C_InvalidIndex) continue;

							vFstatKey.push_back(BIG_FileKey(ul_Index));	

							hItemSub = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconCob, mi_IconCob, hItem);
							po_Tree->SetItemData(hItemSub, (DWORD) pst_ColMap->dpst_Cob[i]);

							if(pst_ColMap->dpst_Cob[i]->pst_GMatList)
							{
								hItemSubSub = po_Tree->InsertItem
									(
										pst_ColMap->dpst_Cob[i]->sz_GMatName,
										mi_IconGam,
										mi_IconGam,
										hItemSub
									);
								po_Tree->SetItemData(hItemSubSub, (DWORD) pst_ColMap->dpst_Cob[i]->pst_GMatList);
							}
						}
					}
				}
			}
		}

		if ( (mpst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ZDM) && (mpst_Gao->pst_Extended->pst_Col)	)
		{
			pst_Zdm = ((COL_tdst_Base *) mpst_Gao->pst_Extended->pst_Col)->pst_Instance;
			if(pst_Zdm)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Zdm);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					vFstatKey.push_back(BIG_FileKey(ul_Index));	
					hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconCin, mi_IconCin, h_Parent);
					po_Tree->SetItemData(hItem, (DWORD) pst_Zdm);

					if(pst_Zdm->pst_ColSet)
					{
						ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Zdm->pst_ColSet);
						if(ul_Index != BIG_C_InvalidIndex)
						{
							vFstatKey.push_back(BIG_FileKey(ul_Index));	
							hItemSub = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconCmd, mi_IconCmd, hItem);
							po_Tree->SetItemData(hItemSub, (DWORD) pst_Zdm->pst_ColSet);
						}
					}
				}
			}
		}

		if ( (mpst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) && (mpst_Gao->pst_Extended->pst_Ai) )
		{
			pst_AI = (AI_tdst_Instance *) mpst_Gao->pst_Extended->pst_Ai;
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				vFstatKey.push_back(BIG_FileKey(ul_Index));	
				hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconOin, mi_IconOin, h_Parent);
				po_Tree->SetItemData(hItem, (DWORD) pst_AI);
			}

			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_VarDes );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				vFstatKey.push_back(BIG_FileKey(ul_Index));	
				hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconOva, mi_IconOva, h_Parent);
				po_Tree->SetItemData(hItem, (DWORD) pst_AI->pst_VarDes);
			}

			if(pst_AI->pst_Model)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_Model );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					vFstatKey.push_back(BIG_FileKey(ul_Index));	
					hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconMdl, mi_IconMdl, h_Parent);
		    		po_Tree->SetItemData(hItem, (DWORD) pst_AI->pst_Model);
				}
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_Model->pst_VarDes );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					vFstatKey.push_back(BIG_FileKey(ul_Index));	
					hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconOva, mi_IconOva, h_Parent);
					po_Tree->SetItemData(hItem, (DWORD) pst_AI->pst_VarDes);
				}
			}
		}

		if ( (mpst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Sound) && (mpst_Gao->pst_Extended->pst_Sound))
		{
			pst_Sound = mpst_Gao->pst_Extended->pst_Sound;
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Sound);
			if(ul_Index != BIG_C_InvalidIndex)
			{
				vFstatKey.push_back(BIG_FileKey(ul_Index));	
				hItem = po_Tree->InsertItem(BIG_NameFile(ul_Index), mi_IconSnk, mi_IconSnk, h_Parent);
				po_Tree->SetItemData(hItem, (DWORD) pst_Sound);
			}
		}
	}
	
	if(mpst_Gao->ul_PrefabObjKey && mpst_Gao->ul_PrefabObjKey != BIG_C_InvalidKey)
	{
		ul_Index = BIG_ul_SearchKeyToFat( mpst_Gao->ul_PrefabObjKey );
		if ( ul_Index && (ul_Index != BIG_C_InvalidIndex) )
		{
			vFstatKey.push_back(BIG_FileKey(ul_Index));	
			sprintf( sz_String, "[Prefab] %s", BIG_NameFile( ul_Index ) );
			hItem = po_Tree->InsertItem( sz_String, mi_IconPfb, mi_IconPfb);
			po_Tree->SetItemData(hItem, (DWORD) ul_Index );
		}
	}
#ifdef JADEFUSION
		break;
	case EDIAC_InfoType_IsLightedBy:
		b_Sep_Added = FALSE;
		for(i = 0; i < VS_MAX_LIGHTS; i++)
		{
			if(mpst_Gao->apst_IsLightBy[i])
			{
				//insert separator
				if(!b_Sep_Added &&(((F3D_cl_View *) mpo_View)->mst_WinHandles.pst_DisplayData->ul_DisplayFlags & GDI_Cul_DF_ForceMaxDynLights) && (i >= VS_MAX_LIGHTS_IN_ENGINE))
				{
					hItem = po_Tree->InsertItem("------------------------------------------", -1, -1, h_Parent);
					b_Sep_Added = TRUE;
				}

				//test if it's in a rejection list
				//RejectedLights * pLights = g_LightRejectionList.GetRejectionList( mpst_Gao );
				//if(pLights)
				//{
				//	RejectedLights::iterator itLight = pLights->begin();
				//	for( ; itLight != pLights->end(); ++itLight )
				//	{
				//		if( *itLight == mpst_Gao->apst_IsLightBy[i] )
				//		{
				//			mi_IconCurrentGrl = mi_IconRejectedGrl;
				//		}
				//	}
				//}
				hItem = po_Tree->InsertItem(mpst_Gao->apst_IsLightBy[i]->sz_Name ? mpst_Gao->apst_IsLightBy[i]->sz_Name : "Unknow", mi_IconGrl, mi_IconGrl, h_Parent);
				po_Tree->SetItemData(hItem, (DWORD) mpst_Gao->apst_IsLightBy[i]);
			}
		}
		break;
	default:
		//set to normal info type and recall fct
		mi_InfoType = EDIAC_InfoType_Normal;
		FillTree();
		break;
	}
#endif

	po_Tree->Expand( h_Parent, TVE_EXPAND );
}
#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_GaoInfo::FillTreeMaterialMulti(GRO_tdst_Struct* _pst_Gro, HTREEITEM hParent)
{
	CTreeCtrl* po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	
	MAT_tdst_Multi* pst_Mtt = (MAT_tdst_Multi *) _pst_Gro;
	for(int indexMTT = 0; indexMTT < pst_Mtt->l_NumberOfSubMaterials; indexMTT++)
	{
		if(pst_Mtt->dpst_SubMaterial[indexMTT] == NULL)
		{
			char szMessage[MAX_PATH];
			sprintf(szMessage, "[WARNING] Invalid reference detected in multi-material %s", _pst_Gro->sz_Name);
			LINK_PrintStatusMsg(szMessage);
		}
		else
		{
			HTREEITEM hItem = po_Tree->InsertItem(GRO_sz_Struct_GetName(&pst_Mtt->dpst_SubMaterial[indexMTT]->st_Id), mi_IconGrm, mi_IconGrm, hParent);
			po_Tree->SetItemData(hItem, (DWORD) &pst_Mtt->dpst_SubMaterial[indexMTT]->st_Id);
		}
	}
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GaoInfo::OnBtGo(void)
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
void EDIA_cl_GaoInfo::OnBt3DVIew(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject	*pst_Gao;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/


	if(mul_Index != BIG_C_InvalidIndex)
	{
		pst_Gao = (OBJ_tdst_GameObject *)(LOA_ul_SearchAddress(BIG_PosFile(mul_Index)));
		if(!OBJ_IsValidGAO(pst_Gao))
			return;

		if(mpst_Gao->pst_World && mpst_Gao->pst_World->pst_Selection)
			SEL_DelItem(mpst_Gao->pst_World->pst_Selection, pst_Gao);

		((F3D_cl_View *) mpo_View)->ForceSelectObject(pst_Gao, TRUE);
		LINK_Refresh();
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_GaoInfo::OnBnClickedRadio(void)
{
	// Get the radio button that has been clicked
	//The defines for the differents type of info must be continous in the res.h file
	mi_InfoType = GetCheckedRadioButton(IDC_RADIO_TYPE1, IDC_RADIO_TYPE2) - IDC_RADIO_TYPE1;
	
	// Update the content of the tree
	FillTree();

}
#endif

#endif /* ACTIVE_EDITORS */
