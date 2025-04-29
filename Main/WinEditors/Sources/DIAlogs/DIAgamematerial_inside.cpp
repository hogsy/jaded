/*$T DIAcreategeometry_inside.cpp GC! 1.081 03/26/01 11:41:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BASe/BAStypes.h"
#include "EDIapp.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "DIAlogs/DIAgamematerial_dlg.h"
#include "DIAlogs/DIAgamematerial_inside.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGdefs.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/COLlision/COLload.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/COLlision/COLinit.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "GDInterface/GDInterface.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dview_undo.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#include "Res/Res.h"

#ifdef JADEFUSION
extern void COL_SortGameMaterial(COL_tdst_GameMatList *);
extern void GEO_CreateGeoFromCob(OBJ_tdst_GameObject *, GEO_tdst_Object *, COL_tdst_Cob *);
extern BOOL	COL_gb_OnMaterialPaint;
extern BOOL	COL_gb_SoundColorDisplay;
#else
extern "C" void COL_SortGameMaterial(COL_tdst_GameMatList *);
extern "C" void GEO_CreateGeoFromCob(OBJ_tdst_GameObject *, GEO_tdst_Object *, COL_tdst_Cob *);
extern BOOL	COL_gb_OnMaterialPaint;
extern BOOL	COL_gb_SoundColorDisplay;
#endif

IMPLEMENT_DYNCREATE(EDIA_cl_GameMaterialView, CFormView)

BEGIN_MESSAGE_MAP(EDIA_cl_GameMaterialView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEPATH, OnButtonBrowsePath)
	ON_BN_CLICKED(IDC_BUTTON_GOTO, OnGoto)
	ON_BN_CLICKED(IDC_CHECK_DISPLAY, OnChangeDisplay)
	ON_BN_CLICKED(IDC_CHECK_LINK, OnLink)
	ON_BN_CLICKED(IDC_CHECK_PAINT, OnPaint)
	ON_BN_CLICKED(IDC_CHECK_PAINT_SOUND, OnPaintSound)
	ON_BN_CLICKED(IDC_CHECK_SOUND, OnDisplaySound)
	ON_BN_CLICKED(IDC_BUTTON_ELEMID, OnElemID)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GAMEMATERIAL, OnIDDoubleClick)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GAMEMATERIAL, OnChangeGameMat)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GameMaterialView::EDIA_cl_GameMaterialView(void) : CFormView(DIALOGS_IDD_GMATIN1)
{
	mul_Index = BIG_C_InvalidIndex;
	mpst_GameMatList = NULL;
	Link3D_On = TRUE;
	Show_Sel = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GameMaterialView::~EDIA_cl_GameMaterialView(void)
{
	if(mpst_GameMatList)
	{
		mpst_GameMatList->ul_NbOfInstances--;
		if(!mpst_GameMatList->ul_NbOfInstances)
		{
			MEM_Free(mpst_GameMatList->pal_Id);
			MEM_Free(mpst_GameMatList->pst_GMat);
			MEM_Free(mpst_GameMatList);
			LOA_DeleteAddress(mpst_GameMatList);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GameMaterialView::Create
(
	LPCTSTR		lpszClassName,
	LPCTSTR		lpszWindowName,
	DWORD		dwStyle,
	const RECT	&rect,
	CWnd		*pParentWnd
)
{
	CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, 0, NULL);
	return TRUE;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GameMaterialView::PreTranslateMessage(MSG *pMsg)
{
	HWND	wind;

	wind = GetDlgItem(IDC_LIST_GAMEMATERIAL)->GetSafeHwnd();

	if((pMsg->message == WM_KEYDOWN) && (pMsg->hwnd == GetDlgItem(IDC_LIST_GAMEMATERIAL)->GetSafeHwnd()))
	{
		OnKeyDown(pMsg->wParam);
	}

	if((pMsg->message == WM_RBUTTONDOWN) && (pMsg->hwnd == GetDlgItem(IDC_LIST_GAMEMATERIAL)->GetSafeHwnd()))
	{
		OnRightClick();
	}


	return CFormView::PreTranslateMessage(pMsg);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::OnButtonBrowsePath(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Open/Create GameMaterial File", 3, TRUE, TRUE, NULL, "*.gam");
    CString				o_Temp;
	ULONG				ul_Index;
    char                *sz_Ext;
	CListCtrl			*pList;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	if(o_File.DoModal() == IDOK)
	{
		L_strcpy(msz_Path, o_File.masz_FullPath);

		o_File.GetItem(o_File.mo_File, 0, o_Temp);

		sz_Ext = strrchr( (char *) (LPCSTR) o_Temp, '.' );
		if ((sz_Ext == NULL) || (L_stricmp( sz_Ext, ".gam" ) != 0))
		{
			M_MF()->MessageBox( "Bad file extension\n(.gam)", "Error", MB_OK | MB_ICONSTOP );
			GetDlgItem(IDC_EDIT_PATH)->SetFocus();
			return;
		}

			/* Check/Create directory */
		if((ul_Index = BIG_ul_CreateDir(msz_Path)) == BIG_C_InvalidIndex)
		{
			M_MF()->MessageBox("Error while creating directory", "Error", MB_OK | MB_ICONERROR);
			GetDlgItem(IDC_EDIT_PATH)->SetFocus();
			return;
		}

		L_strcpy(msz_Name, o_Temp);

		/* Check/Create file */
		ul_Index = BIG_ul_SearchFile(ul_Index, msz_Name);
		if(ul_Index == BIG_C_InvalidIndex)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~*/
			int					temp;
			/*~~~~~~~~~~~~~~~~~~~~~~*/

			ul_Index = BIG_ul_CreateFile(msz_Path, msz_Name);


			SAV_Begin(msz_Path, msz_Name);
			temp = 0;
			SAV_Buffer(&temp, 4);
			SAV_ul_End();

	
			pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);
			pList->DeleteAllItems();

		}

		mul_Index = ul_Index;

		mpo_Dialog->mpo_DataView->ResetList();

		UpdateName();
		UpdateList();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::UpdateName()
{
	GetDlgItem(IDC_EDIT_PATH)->SetWindowText(msz_Path);
	GetDlgItem(IDC_EDIT_NAME)->SetWindowText(msz_Name);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::UpdateList()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl					*pList;
	COL_tdst_GameMat			*pst_GMat;
	int							i;
	char						token[256];
	char						asz_Comment[64];
	LV_ITEM						st_Item;
	int							index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);

	/* Previous Game Material */
	if(mpst_GameMatList && ((mul_Index == BIG_C_InvalidIndex)  || (LOA_ul_SearchKeyWithAddress((ULONG)mpst_GameMatList) != BIG_FileKey(mul_Index))))
	{
		mpst_GameMatList->ul_NbOfInstances--;
		if(!mpst_GameMatList->ul_NbOfInstances)
		{
			MEM_Free(mpst_GameMatList->pal_Id);
			MEM_Free(mpst_GameMatList->pst_GMat);
			MEM_Free(mpst_GameMatList);
			LOA_DeleteAddress(mpst_GameMatList);
		}

		mpst_GameMatList = NULL;
	}


	if(mul_Index == BIG_C_InvalidIndex) 
	{
		sprintf(msz_Name, "");
		sprintf(msz_Path, "");			
		UpdateName();

		pList->DeleteAllItems();
		mpo_Dialog->mpo_DataView->ResetList();
		return;
	}


	/* New Game Material */
	if(!mpst_GameMatList)
	{
		LOA_MakeFileRef
		(
			BIG_FileKey(mul_Index),
			(ULONG *) &mpst_GameMatList,
			COL_ul_CallBackLoadGameMaterial,
			LOA_C_MustExists
		);
		LOA_Resolve();

		mpst_GameMatList->ul_NbOfInstances ++;
	}

	pList->DeleteAllItems();
	mpo_Dialog->mpo_DataView->ResetList();

	st_Item.mask = LVIF_TEXT;
	st_Item.pszText = asz_Comment;
	st_Item.iImage = 0;
	st_Item.cchTextMax = 64;


	for(i = 0; i < (int) mpst_GameMatList->ul_GMat; i++)
	{
		pst_GMat = &mpst_GameMatList->pst_GMat[i];
		sprintf(token, "(%04d)", mpst_GameMatList->pal_Id[i]);
		index = pList->InsertItem(i, token);

		st_Item.iItem = index;
		strncpy(asz_Comment, pst_GMat->asz_Comment, 64);
		st_Item.iSubItem = 1;
		pList->SetItem(&st_Item);
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */


void EDIA_cl_GameMaterialView::OnIDDoubleClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl					*pList;
	int							item;
	COL_tdst_GameMat			*pst_GMat;
	EDIA_cl_NameDialog			o_Dialog("Enter ID Comment");
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);
	item = pList->GetNextItem(-1, LVNI_SELECTED);

    if((item != -1) && mpst_GameMatList)
	{
		pst_GMat = &mpst_GameMatList->pst_GMat[item];
		o_Dialog.mo_Name = pst_GMat->asz_Comment;

		if(pst_GMat && (o_Dialog.DoModal() == IDOK))
		{
			L_strcpy(pst_GMat->asz_Comment, (char *) (LPCSTR) o_Dialog.mo_Name);

			UpdateList();

			pList->SetItemState(item, 0xFFFF, LVIS_FOCUSED | LVIS_SELECTED);
			pList->EnsureVisible(item, TRUE);
		}
	}

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::OnChangeGameMat(NMHDR * pNotifyStruct, LRESULT * result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl					*pList;
	int							item;
	COL_tdst_GameMat			*pst_GMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);
	item = pList->GetNextItem(-1, LVNI_SELECTED);

    if(mpst_GameMatList)
	{
		GetDlgItem(IDC_CHECK_PAINT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_PAINT_SOUND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DISPLAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ELEMID)->EnableWindow(TRUE);			
		GetDlgItem(IDC_BUTTON_GOTO)->EnableWindow(TRUE);

		if(item != -1)
			pst_GMat = &mpst_GameMatList->pst_GMat[item];
		else
			pst_GMat = NULL;

		if(pst_GMat)
		{
			mpo_Dialog->mpo_DataView->ResetList();
			mpo_Dialog->mpo_DataView->AddItem("Display", EVAV_EVVIT_Bool, &pst_GMat->b_Display);
			mpo_Dialog->mpo_DataView->AddItem("Transparent", EVAV_EVVIT_Bool, &pst_GMat->b_Transparent);
			mpo_Dialog->mpo_DataView->AddItem("ID Color", EVAV_EVVIT_Color, &pst_GMat->ul_Color);
			mpo_Dialog->mpo_DataView->AddItem("Sound Color", EVAV_EVVIT_Color, &pst_GMat->ul_SoundColor);
			mpo_Dialog->mpo_DataView->AddItem("Attributes", EVAV_EVVIT_Separator, NULL);
			mpo_Dialog->mpo_DataView->AddItem("Slide", EVAV_EVVIT_Float, &pst_GMat->f_Slide);
			mpo_Dialog->mpo_DataView->AddItem("Rebound", EVAV_EVVIT_Float, &pst_GMat->f_Rebound);
			mpo_Dialog->mpo_DataView->AddItem("Sound", EVAV_EVVIT_Int, &pst_GMat->uc_Sound, 0, 0, 0, 1);
			mpo_Dialog->mpo_DataView->AddItem("Custom", EVAV_EVVIT_SubStruct,  &pst_GMat->ul_CustomBits , EVAV_AutoExpand, LINK_C_GameMaterialFlags, 0, 4);

			mpo_Dialog->mpo_DataView->SetItemList(&mpo_Dialog->mo_ListItems);

			if(Link3D_On)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				int							i;
				COL_tdst_Cob				*pst_Cob;
				EOUT_cl_Frame				*po_Output;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				po_Output = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);

				if(po_Output->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World)
				{
					pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(po_Output->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);
					if(!pst_Cob || (pst_Cob->pst_GMatList != mpst_GameMatList)) return;
	                   
					if(pst_Cob->uc_Type == COL_C_Zone_Triangles)
					{
						for(i = 0; i < (int) pst_Cob->pst_TriangleCob->l_NbElements; i++)
						{
							if(pst_Cob->pst_TriangleCob->dst_Element[i].l_MaterialId == mpst_GameMatList->pal_Id[item])
							{
								pst_Cob->ul_EditedElement = i;
								LINK_Refresh();
								return;
							}
						}

						pst_Cob->ul_EditedElement = (ULONG) -1;
						LINK_Refresh();
					}
				}
			}
		}
	}


}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_GameMaterialView::AddID(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl					*pList;
	EDIA_cl_NameDialog			o_Dialog("ID Number");
	int							ID, i, j, k;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_GameMatList) return;

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);

	o_Dialog.mo_Name = "";
	if(o_Dialog.DoModal() == IDOK)
	{
		ID = L_atoi((char *) (LPCSTR) o_Dialog.mo_Name);

		/* This ID may already exist */
		for(i = 0; i < (int) mpst_GameMatList->ul_GMat; i++)
		{
			if(mpst_GameMatList->pal_Id[i] == ID)
			{
				MessageBox("GameMaterial ID already in use", "Warning", MB_OK);
				return;
			}
		}

		if(mpst_GameMatList->ul_GMat)
		{
			mpst_GameMatList->pst_GMat = (COL_tdst_GameMat *) MEM_p_Realloc
			(
				mpst_GameMatList->pst_GMat,
				(mpst_GameMatList->ul_GMat + 1) * sizeof(COL_tdst_GameMat)
			);

			mpst_GameMatList->pal_Id = (ULONG *)  MEM_p_Realloc
			(
				mpst_GameMatList->pal_Id,
				(mpst_GameMatList->ul_GMat + 1) * sizeof(ULONG)
			);

		}
		else
		{
			mpst_GameMatList->pst_GMat = (COL_tdst_GameMat *) MEM_p_Alloc
			(
				sizeof(COL_tdst_GameMat)
			);

			mpst_GameMatList->pal_Id = (ULONG *)  MEM_p_Alloc
			(
				sizeof(ULONG)
			);
		}

		L_memset(&mpst_GameMatList->pst_GMat[mpst_GameMatList->ul_GMat], 0, sizeof(COL_tdst_GameMat));
		mpst_GameMatList->pst_GMat[mpst_GameMatList->ul_GMat].ul_Color = 0xFF00FF00; /* Default Green */
		mpst_GameMatList->pst_GMat[mpst_GameMatList->ul_GMat].ul_SoundColor = 0x00dcad21; /* Default Blue */
		mpst_GameMatList->pst_GMat[mpst_GameMatList->ul_GMat].b_Display = TRUE;
        mpst_GameMatList->pst_GMat[mpst_GameMatList->ul_GMat].b_Transparent = FALSE;

		mpst_GameMatList->pal_Id[mpst_GameMatList->ul_GMat] = ID;
		mpst_GameMatList->ul_GMat ++;
		COL_SortGameMaterial(mpst_GameMatList);
		UpdateList();
		pList->SetFocus();

		for(i = 0; i < (int) mpst_GameMatList->ul_GMat; i++)
		{
			if(mpst_GameMatList->pal_Id[i] == ID)
			{
				k = -1;
				while((j = pList->GetNextItem(k, LVNI_SELECTED)) != -1)
				{
					pList->SetItemState(j, 0, LVIS_FOCUSED | LVIS_SELECTED);
					k = j;
				}

				pList->SetItemState(i, 0xFFFF, LVIS_FOCUSED | LVIS_SELECTED);
				pList->EnsureVisible(i, TRUE);
				OnChangeGameMat();
				return;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::DeleteID(int ID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_GameMat			*pst_GMat;
	int							i_Res;
	char						asz_Text[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if((ID != -1) && mpst_GameMatList)
	{
		pst_GMat = &mpst_GameMatList->pst_GMat[ID];
		if(pst_GMat)
		{
			sprintf(asz_Text, "Delete ID (%04d)?", mpst_GameMatList->pal_Id[ID]);
			i_Res = MessageBox(asz_Text, "Confirmation", MB_YESNO | MB_ICONQUESTION);
			if(i_Res == IDYES)
			{
				L_memcpy(&mpst_GameMatList->pst_GMat[ID], &mpst_GameMatList->pst_GMat[ID+1], (mpst_GameMatList->ul_GMat - ID - 1) * sizeof(COL_tdst_GameMat));
				L_memcpy(&mpst_GameMatList->pal_Id[ID], &mpst_GameMatList->pal_Id[ID+1], (mpst_GameMatList->ul_GMat - ID - 1) * 4);
				mpst_GameMatList->ul_GMat --;
				COL_SortGameMaterial(mpst_GameMatList);
				UpdateList();
			}
		}
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::OnKeyDown(UINT nChar)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl					*pList;
	int							item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);
	item = pList->GetNextItem(-1, LVNI_SELECTED);

    if((item != -1) && mpst_GameMatList)
	{
		if(nChar == VK_DELETE) DeleteID(item);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::OnRightClick(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl					*pList;
	EMEN_cl_SubMenu				o_SubMenu(FALSE);
	CPoint						o_Point;
	int							item, i_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!mpst_GameMatList)
		return;

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);

	item = pList->GetNextItem(-1, LVNI_SELECTED);

	GetCursorPos(&o_Point);
	M_MF()->InitPopupMenuAction(NULL, &o_SubMenu);
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 1, TRUE, "New ID");
	if(item != -1)
		M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 2, TRUE, "Delete ID");
	M_MF()->AddPopupMenuAction(NULL, &o_SubMenu, 3, TRUE, "Paint");

	i_Res = M_MF()->TrackPopupMenuAction(NULL, o_Point, &o_SubMenu);

	switch(i_Res)
	{
		case 1:
			AddID();
			break;

		case 2:
			DeleteID(item);
			break;

		case 3:
			COL_gb_OnMaterialPaint = TRUE;
			((CButton *)GetDlgItem(IDC_CHECK_PAINT))->SetCheck(BST_CHECKED);		
			PaintColMap(mpst_GameMatList->pal_Id[item], FALSE);
			((CButton *)GetDlgItem(IDC_CHECK_PAINT))->SetCheck(BST_UNCHECKED);
			
			COL_gb_OnMaterialPaint = FALSE;
			break;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ChangeFaceID(COL_tdst_Cob *_pst_Cob, int elem, int triangle, int newID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ElementIndexedTriangles	*pst_Elem1, *pst_Elem2;
	int									i;
	int									r1, r2, r3;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Cob->uc_Type != COL_C_Zone_Triangles)
		return;

	if(elem == -1)
		return;

	pst_Elem1 = &_pst_Cob->pst_TriangleCob->dst_Element[elem];
	if(pst_Elem1->l_MaterialId == newID) return;

	pst_Elem2 = NULL;
	for(i=0; i < (int)_pst_Cob->pst_TriangleCob->l_NbElements; i++)
	{
		if(_pst_Cob->pst_TriangleCob->dst_Element[i].l_MaterialId == newID)
		{
			pst_Elem2 = &_pst_Cob->pst_TriangleCob->dst_Element[i];
			break;
		}
	}

	r1 = pst_Elem1->dst_Triangle[triangle].auw_Index[0];
	r2 = pst_Elem1->dst_Triangle[triangle].auw_Index[1];
	r3 = pst_Elem1->dst_Triangle[triangle].auw_Index[2];


	if(!pst_Elem2)
	{
		_pst_Cob->pst_TriangleCob->dst_Element = (COL_tdst_ElementIndexedTriangles *) MEM_p_Realloc(_pst_Cob->pst_TriangleCob->dst_Element, (_pst_Cob->pst_TriangleCob->l_NbElements + 1) * sizeof(COL_tdst_ElementIndexedTriangles));
		_pst_Cob->pst_TriangleCob->l_NbElements ++;

		pst_Elem1 = &_pst_Cob->pst_TriangleCob->dst_Element[elem];
		pst_Elem2 = &_pst_Cob->pst_TriangleCob->dst_Element[_pst_Cob->pst_TriangleCob->l_NbElements - 1];

		pst_Elem2->dst_Triangle = (COL_tdst_IndexedTriangle *) MEM_p_Alloc(sizeof(COL_tdst_IndexedTriangle));
		L_memset(pst_Elem2->dst_Triangle, 0, sizeof(COL_tdst_IndexedTriangle));

		pst_Elem2->dst_Triangle[0].auw_Index[0] = r1;
		pst_Elem2->dst_Triangle[0].auw_Index[1] = r2;
		pst_Elem2->dst_Triangle[0].auw_Index[2] = r3;

		pst_Elem2->dst_Triangle[0].auw_Prox[0] = -1;
		pst_Elem2->dst_Triangle[0].auw_Prox[1] = -1;
		pst_Elem2->dst_Triangle[0].auw_Prox[2] = -1;


		pst_Elem2->l_MaterialId = newID;
		pst_Elem2->pst_Cob = _pst_Cob;
		pst_Elem2->uw_NbTriangles = 1;
		pst_Elem2->uc_Flag = pst_Elem1->uc_Flag;
		pst_Elem2->uc_Design = pst_Elem1->uc_Design;

		_pst_Cob->ul_EditedElement = _pst_Cob->pst_TriangleCob->l_NbElements - 1;
	}
	else
	{
		_pst_Cob->ul_EditedElement = i;

		if(pst_Elem2->uw_NbTriangles)
		{
			pst_Elem2->dst_Triangle = (COL_tdst_IndexedTriangle *) MEM_p_Realloc(pst_Elem2->dst_Triangle, (pst_Elem2->uw_NbTriangles + 1) * sizeof(COL_tdst_IndexedTriangle));
		}
		else
		{
			pst_Elem2->dst_Triangle = (COL_tdst_IndexedTriangle *) MEM_p_Alloc(sizeof(COL_tdst_IndexedTriangle));
		}

		pst_Elem2->uw_NbTriangles ++;

		pst_Elem2->dst_Triangle[pst_Elem2->uw_NbTriangles - 1].auw_Index[0] = r1;
		pst_Elem2->dst_Triangle[pst_Elem2->uw_NbTriangles - 1].auw_Index[1] = r2;
		pst_Elem2->dst_Triangle[pst_Elem2->uw_NbTriangles - 1].auw_Index[2] = r3;

		pst_Elem2->dst_Triangle[pst_Elem2->uw_NbTriangles - 1].auw_Prox[0] = -1;
		pst_Elem2->dst_Triangle[pst_Elem2->uw_NbTriangles - 1].auw_Prox[1] = -1;
		pst_Elem2->dst_Triangle[pst_Elem2->uw_NbTriangles - 1].auw_Prox[2] = -1;
	}

	if(pst_Elem1->uw_NbTriangles > 1)
	{
		L_memcpy(&pst_Elem1->dst_Triangle[triangle], &pst_Elem1->dst_Triangle[triangle + 1], (pst_Elem1->uw_NbTriangles - triangle - 1) * sizeof(COL_tdst_IndexedTriangle));
		pst_Elem1->uw_NbTriangles --;
	}
	else
	{
		MEM_Free(pst_Elem1->dst_Triangle);
		
		L_memcpy(&_pst_Cob->pst_TriangleCob->dst_Element[elem], &_pst_Cob->pst_TriangleCob->dst_Element[elem + 1], (_pst_Cob->pst_TriangleCob->l_NbElements - elem - 1) * sizeof(COL_tdst_ElementIndexedTriangles));
		_pst_Cob->pst_TriangleCob->l_NbElements --;

		if((int) _pst_Cob->ul_EditedElement > elem)
			_pst_Cob->ul_EditedElement --;

	}


	/* Free the Geometric Cob */
	if(_pst_Cob->p_GeoCob)
	{             
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_ElementIndexedTriangles	*pst_GeoElement, *pst_LastElement;
		GEO_tdst_Object						*pst_GeoCob;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_GeoCob = (GEO_tdst_Object *) _pst_Cob->p_GeoCob;
		if(pst_GeoCob->dst_Point) MEM_Free(pst_GeoCob->dst_Point);
		if(pst_GeoCob->dst_PointNormal) MEM_Free(pst_GeoCob->dst_PointNormal);

		pst_GeoElement = pst_GeoCob->dst_Element;
		pst_LastElement = pst_GeoElement + pst_GeoCob->l_NbElements;
		for(; pst_GeoElement < pst_LastElement; pst_GeoElement++)
		{
			if(pst_GeoElement->l_NbTriangles && pst_GeoElement->dst_Triangle) MEM_Free(pst_GeoElement->dst_Triangle);
		}

		if(pst_GeoCob->dst_Element) MEM_Free(pst_GeoCob->dst_Element);

		if(pst_GeoCob->pst_SubObject) GEO_SubObject_Free(pst_GeoCob);

		MEM_Free(pst_GeoCob);


		_pst_Cob->p_GeoCob = MEM_p_Alloc(sizeof(GEO_tdst_Object));

		pst_GeoCob = (GEO_tdst_Object *) _pst_Cob->p_GeoCob;

		/* Fill the st_GeoCob structure with info needed to see the Cob. */
		L_memset(pst_GeoCob, 0, sizeof(GEO_tdst_Object));

		pst_GeoCob->st_Id.i = &GRO_gast_Interface[GRO_Geometric];
		GEO_CreateGeoFromCob(_pst_Cob->pst_GO, pst_GeoCob, _pst_Cob);
	}

	_pst_Cob->uc_Flag |= COL_C_Cob_Updated;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::PaintColMap(int ID, BOOL _b_Mask)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MSG								msg;
	CPoint							pt;
	EOUT_cl_Frame					*po_Output;
	SOFT_tdst_PickingBuffer_Pixel	*pst_Pick;
	COL_tdst_Cob					*pst_Cob;
	GEO_tdst_GraphicZone			*pst_GZ;
	char							asz_Text[256];
	CRect							rect;
	int								i_Res, e, t;
	int								nosound_id;
	BOOL							b_LButtonDown;
	F3D_cl_Undo_GeoModif			*po_Modif;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SetCapture();

	b_LButtonDown = FALSE;

	if(_b_Mask)
	{
		COL_gb_SoundColorDisplay = TRUE;
		SetDlgItemText(IDC_CHECK_SOUND, "Sound Color");
	}
	else
	{
		COL_gb_SoundColorDisplay = FALSE;
		SetDlgItemText(IDC_CHECK_SOUND, "ID Color");
	}


	po_Output = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	po_Output->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData->ul_DisplayFlags |= GDI_Cul_DF_ShowCOB;

	LINK_Refresh();

	while(1)
	{
		::GetMessage(&msg, NULL, 0, 0);

		if((GetAsyncKeyState(VK_SPACE) < 0) || (GetAsyncKeyState(VK_MENU) < 0))
		{
			if(EDI_go_TheApp.b_ProcessMouseWheel(&(msg))) continue;
			if(GetCapture() == this) ReleaseCapture();
			DispatchMessage(&msg);
			continue;
		}

		if(GetCapture() != this) 
		{
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PIPE));
			SetCapture();
		}

		if(GetAsyncKeyState(VK_CONTROL))
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PICK));
		else
			::SetCursor(AfxGetApp()->LoadCursor(MAIN_IDC_PIPE));  


		if(msg.message == WM_RBUTTONDOWN)
		{
			ReleaseCapture();
			po_Output->mpo_EngineFrame->mpo_DisplayView->Selection_EndSubObjectMode();
			return;
		}

		if(msg.message == WM_LBUTTONUP)
		{
			b_LButtonDown = FALSE;
		}


		if((msg.message == WM_LBUTTONDOWN) || (msg.message == WM_MOUSEMOVE))
		{
			if(msg.message == WM_MOUSEMOVE)
			{
				if(!b_LButtonDown) continue;
			}

			b_LButtonDown = TRUE;			/* Pick to know pos */
			GetCursorPos(&pt);
			po_Output->mpo_EngineFrame->mpo_DisplayView->GetClientRect(&rect);
			po_Output->mpo_EngineFrame->mpo_DisplayView->ScreenToClient(&pt);
			if(!rect.PtInRect(pt)) break;

			if(!po_Output->mpo_EngineFrame->mpo_DisplayView->Pick_l_UnderPoint(&pt, SOFT_Cuc_PBQF_Zone, 0)) continue;

			pst_Pick = po_Output->mpo_EngineFrame->mpo_DisplayView->Pick_pst_GetFirst(SOFT_Cuc_PBQF_Zone, -1);

			e = (pst_Pick->ul_ValueExt & SOFT_Cul_PBQF_ElementMask) >> SOFT_Cul_PBQF_ElementShift;
			t = (pst_Pick->ul_ValueExt & SOFT_Cul_PBQF_TriangleMask) >> SOFT_Cul_PBQF_TriangleShift;


			/* Gets the GraphicZone from the SoftPickingBuffer. */
			pst_GZ = po_Output->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData->st_DisplayedZones.dpst_GraphicZone + (ULONG) pst_Pick->ul_Value;

			if((pst_GZ->uc_Type == GEO_Cul_GraphicCob) && (pst_GZ->pv_Data))
			{
				pst_Cob = (COL_tdst_Cob *) pst_GZ->pv_Data;

				if(pst_Cob) COL_SynchronizeCob(pst_Cob, FALSE,FALSE);

				if(!SEL_RetrieveItem(po_Output->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_Selection, pst_Cob))
				{
					SEL_Close(po_Output->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_Selection);

					po_Output->mpo_EngineFrame->mpo_DisplayView->Selection_Zone(pst_Cob, (ULONG) pst_Cob->pst_GO, SEL_C_SIF_Cob);
					LINK_Refresh();
					if(!GetAsyncKeyState(VK_CONTROL))
						continue;
				}


				if(GetAsyncKeyState(VK_CONTROL))
				{
					pst_Cob->ul_EditedElement = (pst_Pick->ul_ValueExt & SOFT_Cul_PBQF_ElementMask) >> SOFT_Cul_PBQF_ElementShift;

					if(pst_Cob->uc_Type == COL_C_Zone_Triangles)
						ID = pst_Cob->pst_TriangleCob->dst_Element[pst_Cob->ul_EditedElement].l_MaterialId;
					else
						ID = pst_Cob->pst_MathCob->l_MaterialId;

					UpdateGameMaterial(pst_Cob->pst_GMatList, ID);
 
					LINK_Refresh();
					continue;
				}


				if(pst_Cob->pst_GMatList != mpst_GameMatList)
				{
					sprintf(asz_Text, "GameObject [%s] has a different GameMaterial File. Overwrite?", pst_Cob->pst_GO->sz_Name);
					i_Res = po_Output->mpo_EngineFrame->mpo_DisplayView->MessageBox(asz_Text, "Confirmation", MB_YESNO | MB_ICONQUESTION);
					b_LButtonDown = FALSE;
					if(i_Res == IDNO)
					{
						SEL_Close(po_Output->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_Selection);
					}
					else
					{
						if(pst_Cob->pst_GMatList)
							COL_FreeGameMaterial(pst_Cob);

						pst_Cob->pst_GMatList = mpst_GameMatList;
						pst_Cob->uc_Flag |= COL_C_Cob_GameMat;

						pst_Cob->ul_EditedElement = -1;

						if(pst_Cob->sz_GMatName)
							MEM_Free(pst_Cob->sz_GMatName);
						pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(L_strlen(BIG_NameFile(mul_Index)) + 1);
						L_strcpy(pst_Cob->sz_GMatName, BIG_NameFile(mul_Index));

						mpst_GameMatList->ul_NbOfInstances++;
					}

					LINK_Refresh();
					continue;
				}			

				nosound_id = pst_Cob->pst_TriangleCob->dst_Element[e].l_MaterialId;
				while(nosound_id > 99)
					nosound_id -= 100;

				if
				(
					(pst_Cob->uc_Type == COL_C_Zone_Triangles) 
				&&	(
						(	_b_Mask
						&&	(
								(pst_Cob->pst_TriangleCob->dst_Element[e].l_MaterialId - (ID * 100) < 0)
							||	(pst_Cob->pst_TriangleCob->dst_Element[e].l_MaterialId - (ID * 100) > 99)
							)
						)
					||	(	!_b_Mask
						&&	(pst_Cob->pst_TriangleCob->dst_Element[e].l_MaterialId != ID)
						)
					)

				)	
				{
					po_Modif = new F3D_cl_Undo_GeoModif(po_Output->mpo_EngineFrame->mpo_DisplayView, (GEO_tdst_Object *) pst_GZ->pst_GO, pst_GZ->pst_GO);
					po_Modif->SetDesc("ColMap Paint");
					po_Output->mpo_EngineFrame->mpo_DisplayView->mo_UndoManager.b_AskFor(po_Modif, FALSE);

					COL_ChangeFaceID(pst_Cob, e, t, (_b_Mask) ? nosound_id + (100 * ID) : ID);
				}
			}

			LINK_Refresh();
		}
	}

	ReleaseCapture();

}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::OnLink(void)
{
	Link3D_On = (Link3D_On) ? FALSE : TRUE;

	if(Link3D_On)
	{
		((CButton *)GetDlgItem(IDC_CHECK_LINK))->SetCheck(BST_CHECKED);		
	}
	else
	{
		((CButton *)GetDlgItem(IDC_CHECK_LINK))->SetCheck(BST_UNCHECKED);		
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::OnPaint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl					*pList;
	int							item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!mpst_GameMatList)
		return;

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);
	item = pList->GetNextItem(-1, LVNI_SELECTED);

	COL_gb_OnMaterialPaint = TRUE;
	((CButton *)GetDlgItem(IDC_CHECK_PAINT))->SetCheck(BST_CHECKED);
	PaintColMap(mpst_GameMatList->pal_Id[item], FALSE);
	((CButton *)GetDlgItem(IDC_CHECK_PAINT))->SetCheck(BST_UNCHECKED);
	COL_gb_OnMaterialPaint = FALSE;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::OnDisplaySound(void)
{
	if(COL_gb_SoundColorDisplay)
	{
		SetDlgItemText(IDC_CHECK_SOUND, "ID Color");
		((CButton *)GetDlgItem(IDC_CHECK_SOUND))->SetCheck(BST_UNCHECKED);		
		COL_gb_SoundColorDisplay = FALSE;
	}
	else
	{
		SetDlgItemText(IDC_CHECK_SOUND, "Sound Color");
		((CButton *)GetDlgItem(IDC_CHECK_SOUND))->SetCheck(BST_UNCHECKED);		
		COL_gb_SoundColorDisplay = TRUE;
	}
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GameMaterialView::OnPaintSound(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl					*pList;
	int							item;
	int							soundid;
	EDIA_cl_NameDialog			o_Dialog("Sound ID to paint");
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!mpst_GameMatList)
		return;

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);
	item = pList->GetNextItem(-1, LVNI_SELECTED);

essayeencore:
	o_Dialog.mo_Name = "";
	if(o_Dialog.DoModal() == IDOK)
	{
		soundid = L_atoi((char *) (LPCSTR) o_Dialog.mo_Name);

		if((soundid < 0) || (soundid > 10))
			goto essayeencore;

		COL_gb_OnMaterialPaint = TRUE;              
		((CButton *)GetDlgItem(IDC_CHECK_PAINT_SOUND))->SetCheck(BST_CHECKED);
		PaintColMap(soundid, TRUE);
		((CButton *)GetDlgItem(IDC_CHECK_PAINT_SOUND))->SetCheck(BST_UNCHECKED);
		COL_gb_OnMaterialPaint = FALSE;
	}

	((CButton *)GetDlgItem(IDC_CHECK_PAINT_SOUND))->SetCheck(BST_UNCHECKED);
}

#ifdef JADEFUSION
extern char *BIG_FileName(BIG_INDEX);
#else
extern "C" char *BIG_FileName(BIG_INDEX);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_GameMaterialView::UpdateGameMaterial(COL_tdst_GameMatList *_pst_GMat, int _ID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl	*pList;
	int			i, j, k, bestdelta, best;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	bestdelta = 1000000;
	best = -1;

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);
	if(!_pst_GMat)
	{
		mul_Index = BIG_C_InvalidIndex;
		sprintf(msz_Name, "");
		sprintf(msz_Path, "");			
		UpdateName();
		UpdateList();
		mpst_GameMatList = NULL;

		GetDlgItem(IDC_CHECK_PAINT_SOUND)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_PAINT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DISPLAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ELEMID)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_GOTO)->EnableWindow(FALSE);
		return -1;
	}

	if(mpst_GameMatList != _pst_GMat)
	{
		mul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_GMat);

		if(mul_Index == BIG_C_InvalidIndex) 
		{
			mul_Index = BIG_C_InvalidIndex;
			sprintf(msz_Name, "");
			sprintf(msz_Path, "");			
			UpdateName();
			UpdateList();
			mpst_GameMatList = NULL;

			GetDlgItem(IDC_CHECK_PAINT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_SOUND)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_DISPLAY)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_ELEMID)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_GOTO)->EnableWindow(FALSE);
			return - 1;
		}

		sprintf(msz_Name, "%s", BIG_FileName(mul_Index));
		BIG_ComputeFullName(BIG_ParentFile(mul_Index), msz_Path);

		UpdateName();
		UpdateList();
	}

	k = -1;
	while((j = pList->GetNextItem(k, LVNI_SELECTED)) != -1)
	{
		pList->SetItemState(j, 0, LVIS_FOCUSED | LVIS_SELECTED);
		k = j;
	}


	for(i = 0; i < (int) mpst_GameMatList->ul_GMat; i++)
	{
		if(mpst_GameMatList->pal_Id[i] == _ID)
		{
			pList->SetItemState(i, 0xFFFF, LVIS_FOCUSED | LVIS_SELECTED);
			pList->EnsureVisible(i, TRUE);
			OnChangeGameMat();
			return _ID;
		}

		if(lAbs((long) mpst_GameMatList->pal_Id[i] - (long) _ID) < (long) bestdelta)
		{
			bestdelta = lAbs((long) mpst_GameMatList->pal_Id[i] - (long) _ID);
			best = i;
		}
	}


//	pList->SetItemState(best, 0xFFFF, LVIS_FOCUSED | LVIS_SELECTED);
//	pList->EnsureVisible(best, TRUE);
	OnChangeGameMat();

	return best;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_GameMaterialView::OnGoto()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl					*pList;
	int							ID, RealID, i;
	EDIA_cl_NameDialog			o_Dialog("Goto ID");
	COL_tdst_Cob				*pst_Cob;
	EOUT_cl_Frame				*po_Output;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);
    if(mpst_GameMatList)
	{
		if(o_Dialog.DoModal() == IDOK)
		{
			ID = L_atoi((char *) (LPCSTR) o_Dialog.mo_Name);
			RealID = UpdateGameMaterial(mpst_GameMatList,ID);

			pList->SetItemState(RealID, 0xFFFF, LVIS_FOCUSED | LVIS_SELECTED);
			pList->EnsureVisible(RealID, TRUE);


			po_Output = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);

			pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(po_Output->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);
			if(!pst_Cob || (pst_Cob->pst_GMatList != mpst_GameMatList)) return;

			if(pst_Cob->uc_Type == COL_C_Zone_Triangles)            
			{
				for(i = 0; i < (int) pst_Cob->pst_TriangleCob->l_NbElements; i++)
				{
					if(pst_Cob->pst_TriangleCob->dst_Element[i].l_MaterialId == RealID)
					{
						pst_Cob->ul_EditedElement = i;
						LINK_Refresh();
						return;
					}
				}

				pst_Cob->ul_EditedElement = (ULONG) -1;
				LINK_Refresh();
			}
		}
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_GameMaterialView::OnChangeDisplay()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl	*pList;
	int			item, i;
	CString		o_String;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_GameMatList) return;

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);
	item = pList->GetNextItem(-1, LVNI_SELECTED);

	Show_Sel = (Show_Sel) ? 0 : 1;

	if(Show_Sel)
	{
		((CButton *)GetDlgItem(IDC_CHECK_DISPLAY))->SetCheck(BST_CHECKED);		
		if(item != -1)
		{
			for(i = 0; i < (int) mpst_GameMatList->ul_GMat; i++)
			{
				if(i == item) continue;
				mpst_GameMatList->pst_GMat[i].b_Display = FALSE;
			}                
		}
	}
	else
	{
		((CButton *)GetDlgItem(IDC_CHECK_DISPLAY))->SetCheck(BST_UNCHECKED);		
		for(i = 0; i < (int) mpst_GameMatList->ul_GMat; i++)
		{
			mpst_GameMatList->pst_GMat[i].b_Display = TRUE;
		}

	}

	LINK_Refresh();
}

void EDIA_cl_GameMaterialView::OnElemID()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl					*pList;
	int							New_ID, Old_ID, i, j, k;
	int							oldnumelem, item, edited;
	EDIA_cl_NameDialog			o_Dialog("Change Element ID");
	COL_tdst_Cob				*pst_Cob;
	EOUT_cl_Frame				*po_Output;
	char						asz_Text[256];
	F3D_cl_Undo_GeoModif		*po_Modif;
	BOOL						b_Ok;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pList = (CListCtrl *)GetDlgItem(IDC_LIST_GAMEMATERIAL);
	item = pList->GetNextItem(-1, LVNI_SELECTED);
	po_Output = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);

    if(mpst_GameMatList)
	{
		Old_ID = (item != -1) ? mpst_GameMatList->pal_Id[item] : -1;

		if(o_Dialog.DoModal() == IDOK)
		{
			New_ID = L_atoi((char *) (LPCSTR) o_Dialog.mo_Name);

			b_Ok = FALSE;
			for(i = 0; i < (int) mpst_GameMatList->ul_GMat; i++)
			{
				if(mpst_GameMatList->pal_Id[i] == New_ID)
				{
					b_Ok = TRUE;
					break;
				}
			}


			if(!b_Ok)
			{

				sprintf(asz_Text, "This GameMaterial ID (%i) does not exist.", New_ID);
				po_Output->mpo_EngineFrame->mpo_DisplayView->MessageBox(asz_Text, "Warning", MB_YESNO);
				return;
			}

			pst_Cob = (COL_tdst_Cob *) SEL_pst_GetFirstItem(po_Output->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_Selection, SEL_C_SIF_Cob);
			if(!pst_Cob || (pst_Cob->pst_GMatList != mpst_GameMatList)) return;

			if(pst_Cob->uc_Type == COL_C_Zone_Triangles)
			{
				edited = pst_Cob->ul_EditedElement;
				oldnumelem = pst_Cob->pst_TriangleCob->l_NbElements;

				if(edited == 0xFFFFFFFF)
					return;

				po_Modif = new F3D_cl_Undo_GeoModif(po_Output->mpo_EngineFrame->mpo_DisplayView, (GEO_tdst_Object *) pst_Cob->pst_GO, pst_Cob->pst_GO);
				po_Modif->SetDesc("ColMap Element ID");
				po_Output->mpo_EngineFrame->mpo_DisplayView->mo_UndoManager.b_AskFor(po_Modif, FALSE);

				while(pst_Cob->pst_TriangleCob->l_NbElements >= (ULONG) oldnumelem)
				{
					oldnumelem = pst_Cob->pst_TriangleCob->l_NbElements;
					COL_ChangeFaceID(pst_Cob, pst_Cob->ul_EditedElement, 0, New_ID);
					if(pst_Cob->pst_TriangleCob->l_NbElements >= (ULONG) oldnumelem)
						pst_Cob->ul_EditedElement = edited;
				}

				for(i = 0; i < (int) mpst_GameMatList->ul_GMat; i++)
				{
					if(mpst_GameMatList->pal_Id[i] == New_ID)
					{
						k = -1;
						while((j = pList->GetNextItem(k, LVNI_SELECTED)) != -1)
						{
							pList->SetItemState(j, 0, LVIS_FOCUSED | LVIS_SELECTED);
							k = j;
						}

						pList->SetItemState(i, 0xFFFF, LVIS_FOCUSED | LVIS_SELECTED);
						pList->EnsureVisible(i, TRUE);
						OnChangeGameMat();
						return;
					}
				}

			}



		}
	}
}


#endif /* ACTIVE_EDITORS */

