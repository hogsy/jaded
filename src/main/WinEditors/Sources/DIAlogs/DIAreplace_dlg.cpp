/*$T DIAreplace_dlg.cpp GC! 1.078 03/16/00 10:33:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAreplace_dlg.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "edipaths.h"
#include "bigfiles/bigfat.h"
#include "f3dframe/f3dview.h"
#include "engine/sources/objects/objinit.h"
#include "BIGfiles\LOAding\LOAdefs.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "SELection/SELection.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"

#include <string>

BEGIN_MESSAGE_MAP(EDIA_cl_ReplaceDialog, EDIA_cl_BaseDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_REPLACE, OnBtReplace)
	ON_BN_CLICKED(IDC_REPLACEALL, OnBtReplaceAll)
	ON_BN_CLICKED(IDC_FINDNEXT, OnBtFindNext)
	ON_BN_CLICKED(IDCANCEL, OnBtCancel)
	ON_CBN_EDITCHANGE(IDC_COMBO_FIND, OnChangeComboFind)
	ON_CBN_EDITCHANGE(IDC_COMBO_REPLACE, OnChangeComboReplace)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ACTUAL, OnSelectActual)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PREVIEW, OnSelectPreview)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

/*
=======================================================================================================================
=======================================================================================================================
*/
EDIA_cl_ReplaceDialog::EDIA_cl_ReplaceDialog(WOR_tdst_World *W) :
EDIA_cl_BaseDialog(DIALOGS_IDD_NAMEREPLACE)
{
	mpst_World = W;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
EDIA_cl_ReplaceDialog::~EDIA_cl_ReplaceDialog()
{
	if(((F3D_cl_View *) mpo_View)->mpo_ReplaceDialog == this) 
		((F3D_cl_View *) mpo_View)->mpo_ReplaceDialog = NULL;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_ReplaceDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl *po_TreeCtrlActual;
	CTreeCtrl *po_TreeCtrlPreview;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	EDIA_cl_BaseDialog::OnInitDialog();


	// -------------------------------------------------------------------------
	// Setup TreeCtrl members
	po_TreeCtrlActual = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);
	po_TreeCtrlActual->SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);
	po_TreeCtrlActual->SetFont(&M_MF()->mo_Fnt);


	po_TreeCtrlPreview = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREVIEW);
	po_TreeCtrlPreview->SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);
	po_TreeCtrlPreview->SetFont(&M_MF()->mo_Fnt);

	//Setup Icon list
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

	//initialize variables
	m_StrFind = "";
	m_StrReplace = "";
	m_SelectedItemData = NULL;
	mb_AutoSelect = FALSE;
	mb_Loop = FALSE;
	mb_ItemReplaced = FALSE;

	//fill Trees
	UpdateTrees();

	//Disable all buttons
	DisableButtons();

	return TRUE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnDestroy(void)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_ReplaceDialog::PreTranslateMessage(MSG *p_Msg)
{
	if((p_Msg->message == WM_KEYDOWN) && (p_Msg->wParam == VK_ESCAPE))
	{
		OnCancel();
		return 1;
	}

	return FALSE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::UpdateTrees(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_SelectedItem	*pst_Item;
	CTreeCtrl				*po_TreeCtrlActual;
	CTreeCtrl				*po_TreeCtrlPreview;
	OBJ_tdst_GameObject		*pst_GO;
	DWORD					dw_SelItem;
	HTREEITEM				hSelectItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//keep trace of selected item
	dw_SelItem = m_SelectedItemData;
	hSelectItem = NULL;

	//clear Actual and Preview Trees
	po_TreeCtrlActual = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);
	po_TreeCtrlActual->DeleteAllItems();
	po_TreeCtrlPreview = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREVIEW);
	po_TreeCtrlPreview->DeleteAllItems();

	pst_Item = mpst_World->pst_Selection->pst_FirstItem;
	if(!pst_Item) return;

	pst_GO = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem(mpst_World->pst_Selection, SEL_C_SIF_Object);
	if(!pst_GO) return;

	/* Add all selected objects of world */
	while(pst_Item)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_Item->p_Content;
		AddGaoToTree(pst_GO, ACTUAL);
		AddGaoToTree(pst_GO, PREVIEW);
		pst_Item = pst_Item->pst_Next;
	}

	//reselect Item
	if(dw_SelItem != NULL && b_FindItem(dw_SelItem, hSelectItem, ACTUAL))
		if(b_SetSelectedItemData(hSelectItem))
			po_TreeCtrlActual->SelectItem(hSelectItem);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::AddGaoToTree(OBJ_tdst_GameObject* pst_Gao, BOOL b_Tree)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString					strGaoName, strObjName, strNewName;
	int						i_Str;
	HTREEITEM			    h_Parent, hItem, hItemSub, hItemSubSub;
	CTreeCtrl				*po_TreeCtrl;
	GRO_tdst_Struct		    *pst_Gro;
	COL_tdst_ColMap		    *pst_ColMap;
	COL_tdst_Instance	    *pst_Zdm;
	ANI_st_GameObjectAnim   *pst_Anim;
	AI_tdst_Instance        *pst_AI;

	BIG_INDEX			    ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if( ! pst_Gao ) 
		return;

	if(b_Tree == ACTUAL)
		po_TreeCtrl = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);
	else
		po_TreeCtrl = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREVIEW);

	//get Gao Name
	strGaoName = BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_Gao));
	//remove extension
	strGaoName.Replace(_T(EDI_Csz_ExtGameObject), _T(""));
	
	//set NewName
	strNewName = strGaoName;
	if(b_Tree == PREVIEW)
		strNewName.Replace(_T(m_StrFind), _T(m_StrReplace));

	// Insert root of the tree
	h_Parent = po_TreeCtrl->InsertItem(strNewName, mi_IconGao, mi_IconGao);
	po_TreeCtrl->SetItemData(h_Parent, (DWORD) pst_Gao);
	
	// Insert leaves of the tree
	if(pst_Gao->pst_Base && pst_Gao->pst_Base->pst_Visu)
	{
		if (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
		{
			pst_Anim = pst_Gao->pst_Base->pst_GameObjectAnim;
			if (pst_Anim->pst_SkeletonModel)
			{
				hItem = po_TreeCtrl->InsertItem(pst_Anim->pst_SkeletonModel->sz_Name, mi_IconSkl, mi_IconSkl, h_Parent);
				po_TreeCtrl->SetItemData(hItem, (DWORD) pst_Anim->pst_SkeletonModel);
			}
			if (pst_Anim->pst_Shape)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_Shape);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = po_TreeCtrl->InsertItem(BIG_NameFile(ul_Index), mi_IconShp, mi_IconShp, h_Parent);
					po_TreeCtrl->SetItemData(hItem, (DWORD) pst_Anim->pst_Shape);
				}
			}
			if (pst_Anim->pst_ActionKit)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_ActionKit );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = po_TreeCtrl->InsertItem(BIG_NameFile(ul_Index), mi_IconAck, mi_IconAck, h_Parent);
					po_TreeCtrl->SetItemData(hItem, (DWORD) pst_Anim->pst_ActionKit);
				}
			}
		}
		else
		{
			pst_Gro = pst_Gao->pst_Base->pst_Visu->pst_Object;
			if(pst_Gro)
			{   
				//add test to modify displayed name
				hItem = po_TreeCtrl->InsertItem(strGaoName == GRO_sz_Struct_GetName(pst_Gro) && b_Tree == PREVIEW ? strNewName: GRO_sz_Struct_GetName(pst_Gro), mi_IconGro, mi_IconGro, h_Parent);
				po_TreeCtrl->SetItemData(hItem, (DWORD) pst_Gro);
			}

			pst_Gro = pst_Gao->pst_Base->pst_Visu->pst_Material;
			if(pst_Gro)
			{
				hItem = po_TreeCtrl->InsertItem(GRO_sz_Struct_GetName(pst_Gro), mi_IconGrm, mi_IconGrm, h_Parent);
				po_TreeCtrl->SetItemData(hItem, (DWORD) pst_Gro);
			}
		}
	}

	// Insert leaves of the tree
	if ( pst_Gao->pst_Extended )
	{
		if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Events) )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Gao->pst_Extended->pst_Events->pst_ListTracks );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = po_TreeCtrl->InsertItem(strNewName, mi_IconTrl, mi_IconTrl, h_Parent);
				po_TreeCtrl->SetItemData(hItem, (DWORD) pst_Gao->pst_Extended->pst_Events->pst_ListTracks);
			}

			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Gao->pst_Extended->pst_Events->pst_ListParam );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = po_TreeCtrl->InsertItem(BIG_NameFile(ul_Index), mi_IconTrl, mi_IconTrl, h_Parent);
				po_TreeCtrl->SetItemData(hItem, (DWORD) pst_Gao->pst_Extended->pst_Events->pst_ListParam);
			}
		}
		if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights) )
		{
			pst_Gro = pst_Gao->pst_Extended->pst_Light;
			if(pst_Gro)
			{
				//add test to modify displayed name
				hItem = po_TreeCtrl->InsertItem(strGaoName == GRO_sz_Struct_GetName(pst_Gro) && b_Tree == PREVIEW ? strNewName: GRO_sz_Struct_GetName(pst_Gro), mi_IconGrl, mi_IconGrl, h_Parent);
				po_TreeCtrl->SetItemData(hItem, (DWORD) pst_Gro);
			}
		}

		if( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Group) )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Gao->pst_Extended->pst_Group );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = po_TreeCtrl->InsertItem(BIG_NameFile(ul_Index), mi_IconGrp, mi_IconGrp, h_Parent);
				po_TreeCtrl->SetItemData(hItem, (DWORD) pst_Gao->pst_Extended->pst_Group );
			}
		}

		if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap) && (pst_Gao->pst_Extended->pst_Col) )
		{
			pst_ColMap = ((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_ColMap;
			if(pst_ColMap)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = po_TreeCtrl->InsertItem(strNewName, mi_IconMap, mi_IconMap, h_Parent);
					po_TreeCtrl->SetItemData(hItem, (DWORD) pst_ColMap);

					if(pst_ColMap->uc_NbOfCob)
					{
						for(int i = 0; i < pst_ColMap->uc_NbOfCob; i++)
						{
							if(!pst_ColMap->dpst_Cob[i]) continue;
							ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap->dpst_Cob[i]);
							if(ul_Index == BIG_C_InvalidIndex) continue;

							//add test to modify displayed name
							hItemSub = po_TreeCtrl->InsertItem(strGaoName == BIG_NameFile(ul_Index) && b_Tree == PREVIEW ? strNewName: BIG_NameFile(ul_Index), mi_IconCob, mi_IconCob, hItem);
							po_TreeCtrl->SetItemData(hItemSub, (DWORD) pst_ColMap->dpst_Cob[i]);

							if(pst_ColMap->dpst_Cob[i]->pst_GMatList)
							{
								hItemSubSub = po_TreeCtrl->InsertItem(pst_ColMap->dpst_Cob[i]->sz_GMatName, mi_IconGam, mi_IconGam, hItemSub);
								po_TreeCtrl->SetItemData(hItemSubSub, (DWORD) pst_ColMap->dpst_Cob[i]->pst_GMatList);
							}
						}
					}
				}
			}
		}

		if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ZDM) && (pst_Gao->pst_Extended->pst_Col)	)
		{
			pst_Zdm = ((COL_tdst_Base *) pst_Gao->pst_Extended->pst_Col)->pst_Instance;
			if(pst_Zdm)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Zdm);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = po_TreeCtrl->InsertItem(BIG_NameFile(ul_Index), mi_IconCin, mi_IconCin, h_Parent);
					po_TreeCtrl->SetItemData(hItem, (DWORD) pst_Zdm);

					if(pst_Zdm->pst_ColSet)
					{
						ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Zdm->pst_ColSet);
						if(ul_Index != BIG_C_InvalidIndex)
						{
							hItemSub = po_TreeCtrl->InsertItem(BIG_NameFile(ul_Index), mi_IconCmd, mi_IconCmd, hItem);
							po_TreeCtrl->SetItemData(hItemSub, (DWORD) pst_Zdm->pst_ColSet);
						}
					}
				}
			}
		}

		if ( (pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) && (pst_Gao->pst_Extended->pst_Ai) )
		{
			pst_AI = (AI_tdst_Instance *) pst_Gao->pst_Extended->pst_Ai;
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				strObjName = BIG_NameFile(ul_Index);
				i_Str = strObjName.Find('.');
				if(i_Str)
					strObjName = strObjName.Left(i_Str);
				//add test to modify displayed name
				hItem = po_TreeCtrl->InsertItem(strGaoName == strObjName && b_Tree == PREVIEW ? strNewName: strObjName, mi_IconOin, mi_IconOin, h_Parent);
				po_TreeCtrl->SetItemData(hItem, (DWORD) pst_AI);
			}

			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_VarDes );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				strObjName = BIG_NameFile(ul_Index);
				i_Str = strObjName.Find('.');
				if(i_Str)
					strObjName = strObjName.Left(i_Str);
				//add test to modify displayed name
				hItem = po_TreeCtrl->InsertItem(strGaoName == strObjName && b_Tree == PREVIEW ? strNewName: strObjName, mi_IconOva, mi_IconOva, h_Parent);
				po_TreeCtrl->SetItemData(hItem, (DWORD) pst_AI->pst_VarDes);
			}

			if(pst_AI->pst_Model)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_Model );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = po_TreeCtrl->InsertItem(BIG_NameFile(ul_Index), mi_IconMdl, mi_IconMdl, h_Parent);
					po_TreeCtrl->SetItemData(hItem, (DWORD) pst_AI->pst_Model);
				}
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_Model->pst_VarDes );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = po_TreeCtrl->InsertItem(BIG_NameFile(ul_Index), mi_IconOva, mi_IconOva, h_Parent);
					po_TreeCtrl->SetItemData(hItem, (DWORD) pst_AI->pst_VarDes);
				}
			}
		}
	}

	po_TreeCtrl->Expand( h_Parent, TVE_EXPAND );
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_ReplaceDialog::b_SelectNextItem(HTREEITEM hItem)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl				*po_TreeCtrlActual;
	HTREEITEM				hNextItem, hSelectItem, hTopParent;
	OBJ_tdst_GameObject		*pst_GO;
	CString					StrGaoName;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_TreeCtrlActual = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);

	//test if a Gao is Selected
	hNextItem = po_TreeCtrlActual->GetSelectedItem();
	if(hNextItem)
	{
		pst_GO = (OBJ_tdst_GameObject *) po_TreeCtrlActual->GetItemData(hNextItem);
		if(!OBJ_IsValidGAO(pst_GO))
		{
			hTopParent = h_GetTopParentItem(hNextItem, ACTUAL);
			pst_GO = (OBJ_tdst_GameObject *) po_TreeCtrlActual->GetItemData(hTopParent);
			if(OBJ_IsValidGAO(pst_GO))
			{
				po_TreeCtrlActual->SelectItem(hTopParent);
				return b_SelectNextItem(hTopParent);	//now try with the parent node
			}
		}
	}
	
	if(hItem == NULL)	//restart at the top
	{
		if(!mb_Loop)	//we dont have loop
		{
			hNextItem = po_TreeCtrlActual->GetRootItem();
			mb_Loop = TRUE;
		}
		else			//we have loop, stop search
		{
			if(b_FindItem(m_SelectedItemData, hSelectItem, ACTUAL))	//if find, select it
				po_TreeCtrlActual->SelectItem(hSelectItem);
			return FALSE;
		}
	}
	else
		hNextItem = po_TreeCtrlActual->GetNextSiblingItem(hItem);


	if(hNextItem && po_TreeCtrlActual->GetItemData(hNextItem) == m_SelectedItemData)	//we have loop once, stop search
	{
		if(b_FindItem(m_SelectedItemData, hSelectItem, ACTUAL))	//if find, select it
			po_TreeCtrlActual->SelectItem(hSelectItem);
		return FALSE;
	}
	
	if(hNextItem)
	{
		po_TreeCtrlActual->SelectItem(hNextItem);
		po_TreeCtrlActual->EnsureVisible(hNextItem); 
		//get the pointer
		pst_GO = (OBJ_tdst_GameObject *) po_TreeCtrlActual->GetItemData(hNextItem);
		StrGaoName = BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG)pst_GO));
		if(StrGaoName.Find(m_StrFind) != -1)
			return TRUE;	//find next item with the 'find' string
		else
			return b_SelectNextItem(hNextItem);	//try with sibling item

	}
	else
		return b_SelectNextItem(NULL);	//no sibling item, restart at top
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_ReplaceDialog::b_ReplaceSelectedItem(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject	*pst_GO;
	CTreeCtrl			*po_TreeCtrlActual;
	CString				strGaoName, strNewName;
	HTREEITEM			hSelectItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(m_SelectedItemData == NULL)
		return FALSE;
	
	//get Tree pointer
	po_TreeCtrlActual = (CTreeCtrl *)GetDlgItem(IDC_TREE_ACTUAL);

	pst_GO = (OBJ_tdst_GameObject *)m_SelectedItemData;
	if(OBJ_IsValidGAO(pst_GO))
	{
		//get GAO name
		strGaoName = BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_GO));

		strNewName = strGaoName;
		strNewName.Replace(_T(m_StrFind), _T(m_StrReplace));

		if(strNewName == strGaoName)	//nothing was changed, failed
			return FALSE;
		
		//rename the file
		OBJ_GameObject_Rename(pst_GO, (LPSTR)(LPCSTR)strNewName);
		return TRUE;
	}
	else
	{
		if(b_FindItem(m_SelectedItemData, hSelectItem, ACTUAL))
			return po_TreeCtrlActual->SelectItem(h_GetTopParentItem(hSelectItem, ACTUAL));
		else
			return FALSE;
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_ReplaceDialog::b_SelectItemInTree(DWORD dw_NewSel, BOOL b_Tree)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl				*po_TreeCtrl, *po_OtherTreeCtrl;
	HTREEITEM				hItem, hStartItem, hParentItem, hParentItem2, hSelItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(b_Tree == ACTUAL)	//to find selected item in other tree
	{
        po_TreeCtrl = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREVIEW);
		po_OtherTreeCtrl = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);
	}
	else
	{
		po_TreeCtrl = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);
		po_OtherTreeCtrl = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREVIEW);
	}

	//find the newly selected item in Tree
	hItem = hStartItem = NULL;
	for(uint i = 0; i < po_TreeCtrl->GetCount(); i++)
	{
		if(b_FindItem(dw_NewSel, hItem, b_Tree, hStartItem))
		{
			po_OtherTreeCtrl->SelectItem(hItem);

			//test if we have the same parent
			hParentItem2 = h_GetTopParentItem(hItem, b_Tree);
			if(hParentItem2 == NULL)
				return TRUE;

			hSelItem = po_TreeCtrl->GetSelectedItem();
			if(hSelItem != NULL)
				hParentItem = h_GetTopParentItem(hSelItem, !b_Tree);
			else
				return TRUE;

			if(hParentItem != NULL && hParentItem2 != NULL)	//we have a parents node
				if(po_TreeCtrl->GetItemData(hParentItem) == po_OtherTreeCtrl->GetItemData(hParentItem2))
					return TRUE;
			//restart at parent node
			hStartItem = po_OtherTreeCtrl->GetNextSiblingItem(hParentItem2);
			hItem = NULL;
		}
		else
			return FALSE;
	}

	return TRUE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
HTREEITEM EDIA_cl_ReplaceDialog::h_GetTopParentItem(HTREEITEM hItem, BOOL b_Tree)
{	//must be a valid GAO!!
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl				*po_TreeCtrl;
	HTREEITEM				hParentItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	if(hItem == NULL)
		return NULL;

	//set Tree pointer
	if(b_Tree == ACTUAL)
		po_TreeCtrl = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);
	else
		po_TreeCtrl = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREVIEW);

	hParentItem = po_TreeCtrl->GetParentItem(hItem);
	if(hParentItem != NULL)	//we have a parent node
	{
		if(OBJ_IsValidGAO((OBJ_tdst_GameObject *)po_TreeCtrl->GetItemData(hParentItem)))
			return hParentItem;
		else
			return h_GetTopParentItem(hParentItem, b_Tree);
	}
	return NULL;
}

/*
=======================================================================================================================
=======================================================================================================================
*/

BOOL EDIA_cl_ReplaceDialog::b_FindItem(DWORD itemdata, HTREEITEM &foundat,  BOOL b_Tree, HTREEITEM startnode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl			*po_Tree;
	HTREEITEM			hchild;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(b_Tree == ACTUAL)
		po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);
	else
		po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREVIEW);

	// start at root is required
	if ( startnode == NULL )
		return b_FindItem(itemdata, foundat, b_Tree, po_Tree->GetRootItem());

	if ( itemdata == po_Tree->GetItemData(startnode) )
	{
		foundat = startnode;
		return TRUE;
	}

	// now recurse on all children of this node
	hchild = po_Tree->GetNextItem(startnode,TVGN_CHILD);
	while (hchild != NULL && foundat == NULL ) 
	{
		if(b_FindItem(itemdata, foundat, b_Tree, hchild))
			return TRUE;
		else
			hchild = po_Tree->GetNextSiblingItem(hchild);
	}
	//pass to the next sibling
	hchild = po_Tree->GetNextSiblingItem(startnode);
	if(hchild != NULL)
		return b_FindItem(itemdata, foundat, b_Tree, hchild);
	else
		return FALSE;

}
/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_ReplaceDialog::b_SetSelectedItemData(HTREEITEM hItem)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl			*po_Tree;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Tree = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);

	if(!hItem)
		return FALSE;

	m_SelectedItemData = po_Tree->GetItemData(hItem);
	return TRUE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::EnableButtons(void)
{
	(CButton *) GetDlgItem(IDC_REPLACE)->EnableWindow(TRUE);
	(CButton *) GetDlgItem(IDC_REPLACEALL)->EnableWindow(TRUE);
	(CButton *) GetDlgItem(IDC_FINDNEXT)->EnableWindow(TRUE);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::DisableButtons(void)
{
	(CButton *) GetDlgItem(IDC_REPLACE)->EnableWindow(FALSE);
	(CButton *) GetDlgItem(IDC_REPLACEALL)->EnableWindow(FALSE);
	(CButton *) GetDlgItem(IDC_FINDNEXT)->EnableWindow(FALSE);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnBtReplace(void)
{
	if(b_ReplaceSelectedItem())
	{
		//change Cancel label
		((CButton *) GetDlgItem(IDCANCEL))->SetWindowText("Close");

 		UpdateTrees();
		mb_ItemReplaced = TRUE;
	}
	else
		mb_ItemReplaced = FALSE;

	//goto next item
	OnBtFindNext();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnBtReplaceAll(void)
{
	do
	{
		OnBtReplace();
	}
	while(mb_ItemReplaced);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnBtFindNext(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl				*po_TreeCtrlActual;
	HTREEITEM				hSelectItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	//to prevent stop at end of a voluntary loop
	mb_Loop = FALSE;

	po_TreeCtrlActual = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);

	hSelectItem = po_TreeCtrlActual->GetSelectedItem();
	if(b_SetSelectedItemData(hSelectItem))
		b_SelectNextItem(hSelectItem);

	po_TreeCtrlActual->SetFocus();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnBtCancel(void)
{
	OnCancel();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnChangeComboFind(void)
{
	//get find String
	((CComboBox*) GetDlgItem(IDC_COMBO_FIND))->GetWindowText(m_StrFind);
	
	if(m_StrFind == "")
		DisableButtons();
	else
		EnableButtons();

	UpdateTrees();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnChangeComboReplace(void)
{
	//get replace String
	((CComboBox*) GetDlgItem(IDC_COMBO_REPLACE))->GetWindowText(m_StrReplace);
	UpdateTrees();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnSelectActual(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_TreeActual;
	NM_TREEVIEW *pNotifyStruct ;
	HTREEITEM	hSelectItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mb_AutoSelect)	//to auto select same item in other tree
	{
		mb_AutoSelect = TRUE;

		pNotifyStruct = (NM_TREEVIEW *)pNotifyStruct2;

		po_TreeActual = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);
		hSelectItem = h_GetTopParentItem(pNotifyStruct->itemNew.hItem, ACTUAL);
        if(hSelectItem)	//if a SubObject is selected, select Parent
		{
			b_SelectItemInTree(po_TreeActual->GetItemData(hSelectItem), PREVIEW);
			po_TreeActual->SelectItem(hSelectItem);
		}
		else
			b_SelectItemInTree(po_TreeActual->GetItemData(pNotifyStruct->itemNew.hItem), PREVIEW);
		
		hSelectItem = po_TreeActual->GetSelectedItem();
		b_SetSelectedItemData(hSelectItem);

		mb_AutoSelect = FALSE;
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnSelectPreview(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CTreeCtrl	*po_TreePreview;
	CTreeCtrl	*po_TreeActual;
	NM_TREEVIEW *pNotifyStruct ;
	HTREEITEM	hSelectItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mb_AutoSelect)	//to auto select same item in other tree
	{
		mb_AutoSelect = TRUE;

		pNotifyStruct = (NM_TREEVIEW *)pNotifyStruct2;

		po_TreeActual = (CTreeCtrl *) GetDlgItem(IDC_TREE_ACTUAL);
		po_TreePreview = (CTreeCtrl *) GetDlgItem(IDC_TREE_PREVIEW);
		hSelectItem = h_GetTopParentItem(pNotifyStruct->itemNew.hItem, PREVIEW);
		if(hSelectItem)	//if a SubObject is selected, select Parent
		{
			b_SelectItemInTree(po_TreeActual->GetItemData(hSelectItem), ACTUAL);
			po_TreePreview->SelectItem(hSelectItem);
		}
		else
			b_SelectItemInTree(po_TreePreview->GetItemData(pNotifyStruct->itemNew.hItem), ACTUAL);

		hSelectItem = po_TreeActual->GetSelectedItem();
		b_SetSelectedItemData(hSelectItem);

		mb_AutoSelect = FALSE;
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 503;
	lpMMI->ptMinTrackSize.y = 305;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_ReplaceDialog::OnSize(UINT a, int cx, int cy)
{
	/*~~~~~~~~~~~*/
	CRect	Size, Size2;
	int		i_Shift;
	/*~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnSize(a, cx, cy);
	if(!GetDlgItem(IDC_TREE_ACTUAL)) return;
	if(!GetDlgItem(IDC_TREE_PREVIEW)) return;

	//Replace Buttons
	GetDlgItem(IDC_FINDNEXT)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_FINDNEXT)->MoveWindow((cx - Size.Width() - 11), Size.top, Size.Width(), Size.Height());

	//get shifting
	GetDlgItem(IDC_FINDNEXT)->GetWindowRect(Size2);
	ScreenToClient(Size2);
	i_Shift = Size2.left - Size.left;
	//get shifting

	GetDlgItem(IDC_REPLACE)->GetWindowRect(Size2);
	ScreenToClient(Size2);
	GetDlgItem(IDC_REPLACE)->MoveWindow((cx - Size2.Width() - Size.Width() - (Size.left - Size2.right) - 11),
										Size2.top, Size2.Width(), Size2.Height());

	GetDlgItem(IDCANCEL)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDCANCEL)->MoveWindow((cx - Size.Width() - 11), Size.top, Size.Width(), Size.Height());

	GetDlgItem(IDC_REPLACEALL)->GetWindowRect(Size2);
	ScreenToClient(Size2);
	GetDlgItem(IDC_REPLACEALL)->MoveWindow((cx - Size2.Width() - Size.Width() - (Size.left - Size2.right) - 11),
										   Size2.top, Size2.Width(), Size2.Height());

	//replace ComboBox
	GetDlgItem(IDC_COMBO_FIND)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_COMBO_FIND)->MoveWindow(Size.left, Size.top, Size.Width() + i_Shift, Size.Height());

	GetDlgItem(IDC_COMBO_REPLACE)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_COMBO_REPLACE)->MoveWindow(Size.left, Size.top, Size.Width() + i_Shift, Size.Height());

	//replace Trees
	GetDlgItem(IDC_TREE_PREVIEW)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_TREE_PREVIEW)->MoveWindow((cx / 2) + 10, Size.top, cx - 11 - Size.left, cy - Size.top - 11);

	GetDlgItem(IDC_TREE_PREVIEW)->GetWindowRect(Size2);
	ScreenToClient(Size2);

	GetDlgItem(IDC_TREE_ACTUAL)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_TREE_ACTUAL)->MoveWindow(Size.left, Size.top, Size2.left - Size.left - 20, cy - Size.top - 11);

	//replace static text
	GetDlgItem(IDC_STATIC_PREVIEW)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_STATIC_PREVIEW)->MoveWindow((cx / 2) + 10, Size.top, Size.Width(),Size.Height());


	//GetDlgItem(IDC_TREE_ACTUAL)->MoveWindow(11, h, cx - 240, cy - (h + 11));
	//GetDlgItem(IDC_TREE_PREVIEW)->MoveWindow(5, h, cx - 10, cy - (h + 7));
	//GetDlgItem(IDC_EDIT_NAME)->MoveWindow(156, 6, cx - 166, 20);
	//GetDlgItem(IDC_EDIT_PATH)->MoveWindow(45, 32, cx - 75, 20);
	//GetDlgItem(IDC_BUTTON_GO)->MoveWindow(cx - 30, 32, 20, 20);
	//GetDlgItem(IDC_EDIT_STATUS)->MoveWindow(45, 56, cx - 55, 20);
	//GetDlgItem(IDC_TREE)->MoveWindow(5, h, cx - 10, cy - (h + 5));
}

#endif /* ACTIVE_EDITORS */
