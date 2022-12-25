/*$T DIAshape_dlg.cpp GC! 1.081 01/17/02 10:38:01 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAshape_dlg.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "GraphicDK/Sources/SELection/SELection.h"
#include "EDIpaths.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "DIAlogs/DIAname_dlg.h"
#include "BASe/MEMory/MEM.h"
#include "LINKs/LINKtoed.h"
#include "Res/Res.h"

IMPLEMENT_DYNCREATE(EDIA_cl_ShapeDialog, CFormView)

/* Message Map */
BEGIN_MESSAGE_MAP(EDIA_cl_ShapeDialog, CFormView)
	ON_LBN_SELCHANGE(IDC_LISTBONES, OnSelChange)
	ON_EN_UPDATE(IDC_CANAL, OnUpdateCanal)
	ON_CBN_SELCHANGE(IDC_COMBOVISU, OnUpdateVisu)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_INVISIBLE, OnInvisible)
	ON_BN_CLICKED(IDC_X, OnClear)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ShapeDialog::EDIA_cl_ShapeDialog(EDIA_cl_ToolBoxDialog *_po_TB) :
	CFormView(DIALOGS_IDD_SHAPE)
{
	mpo_ToolBox = _po_TB;
	mpo_View = _po_TB->mpo_View;
	mdc_Visual = NULL;
	mdc_Canal = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ShapeDialog::EDIA_cl_ShapeDialog(void) :
	CFormView(DIALOGS_IDD_SHAPE)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ShapeDialog::~EDIA_cl_ShapeDialog(void)
{
	if(mdc_Visual) MEM_Free(mdc_Visual);
	if(mdc_Canal) MEM_Free(mdc_Canal);

	mdc_Visual = NULL;
	mdc_Canal = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_ShapeDialog::Create
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
BOOL EDIA_cl_ShapeDialog::OnInitDialog(void)
{
	((CEdit *) GetDlgItem(IDC_GAO))->SetWindowText("No Selection");
	((CEdit *) GetDlgItem(IDC_SHAPE))->SetWindowText("-No Shape-");
	UpdateList(TRUE);
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ShapeDialog::OnSize(UINT, int cx, int cy)
{
	if(!GetDlgItem(IDC_GAO)) return;

	GetDlgItem(IDC_GAO)->SetWindowPos(NULL, 5, 5, cx - 10, 20, SWP_NOZORDER);
	GetDlgItem(IDC_SHAPE)->SetWindowPos(NULL, 5, 27, cx - 10, 16, SWP_NOZORDER);
	GetDlgItem(IDC_LISTBONES)->SetWindowPos(NULL, 5, 45, cx - 10, cy - 110, SWP_NOZORDER);
	GetDlgItem(IDC_COMBOVISU)->SetWindowPos(NULL, 5, cy - 55, cx - 10, 20, SWP_NOZORDER);
	GetDlgItem(IDC_RESET)->SetWindowPos(NULL, 5, cy - 25, 20, 20, SWP_NOZORDER);
	GetDlgItem(IDC_INVISIBLE)->SetWindowPos(NULL, 25, cy - 25, 20, 20, SWP_NOZORDER);
	GetDlgItem(IDC_X)->SetWindowPos(NULL, cx - 25, cy - 25, 20, 20, SWP_NOZORDER);
	GetDlgItem(IDC_CANALTXT)->SetWindowPos(NULL, 50, cy - 23, 40, 20, SWP_NOZORDER);
	GetDlgItem(IDC_CANAL)->SetWindowPos(NULL, 90, cy - 25, 40, 20, SWP_NOZORDER);
	GetDlgItem(IDC_SAVE)->SetWindowPos(NULL, 140, cy - 25, 40, 20, SWP_NOZORDER);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ShapeDialog::UpdateList(BOOL _b_All)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	CComboBox			*CB;
	CEdit				*oEdCanal;
	CEdit				*o_Gao;
	CEdit				*o_Shape;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	ANI_tdst_Shape		*pst_Shape;
	int					index;
	int					i_ScrollH, i_ScrollV;
	int					i_TopIndex;
	CRect				o_Rect;
	CString				o_Str;
	char				asz_Name[100];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_View->mst_WinHandles.pst_World) return;

	if
	(
		(!_b_All)
	&&	(
			mpst_Object == (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem
				(
					mpo_View->mst_WinHandles.pst_World->pst_Selection,
					SEL_C_SIF_Object
				)
		)
	) return;

	mpst_Object = (OBJ_tdst_GameObject *) SEL_pst_GetFirstItem
		(
			mpo_View->mst_WinHandles.pst_World->pst_Selection,
			SEL_C_SIF_Object
		);

	o_Gao = (CEdit *) GetDlgItem(IDC_GAO);
	o_Shape = (CEdit *) GetDlgItem(IDC_SHAPE);
	LB = (CListBox *) GetDlgItem(IDC_LISTBONES);
	CB = (CComboBox *) GetDlgItem(IDC_COMBOVISU);
	oEdCanal = (CEdit *) GetDlgItem(IDC_CANAL);

	if
	(
		(!mpst_Object)
	||	!(mpst_Object->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	||	!(mpst_Object->pst_Base)
	||	!(mpst_Object->pst_Base->pst_GameObjectAnim)
	||	!(mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel)
	)
	{
		/*~~~~~~~~~~~*/
		CString o_Name;
		/*~~~~~~~~~~~*/

		LB->ResetContent();
		CB->ResetContent();
		oEdCanal->SetWindowText("");;
		oEdCanal->EnableWindow(FALSE);
		if(mpst_Object)
		{
			o_Name = mpst_Object->sz_Name;
			o_Gao->SetWindowText(o_Name);

			o_Name = " (- No Shape-)";
			o_Shape->SetWindowText(o_Name);
		}

		return;
	}

	oEdCanal->EnableWindow(TRUE);

	pst_Shape = mpst_Object->pst_Base->pst_GameObjectAnim->pst_Shape;

	if(mpst_Object)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~*/
		CString		o_Name;
		char		asz_Name[100];
		BIG_INDEX	ul_Index;
		/*~~~~~~~~~~~~~~~~~~~~~~*/

		o_Name = mpst_Object->sz_Name;
		o_Gao->SetWindowText(o_Name);

		if(pst_Shape)
		{
			o_Name = " (";
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Shape);
			if(ul_Index != BIG_C_InvalidIndex) L_strcpy(asz_Name, BIG_NameFile(ul_Index));
			o_Name += asz_Name;
			o_Name += ")";
		}
		else
		{
			o_Name = " (- No Shape -)";
		}

		o_Shape->SetWindowText(o_Name);
	}

	if(mdc_Visual) MEM_Free(mdc_Visual);
	if(mdc_Canal) MEM_Free(mdc_Canal);

	mdc_Visual = (UCHAR *) MEM_p_Alloc(TAB_ul_PFtable_GetNbElems(mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel->pst_AllObjects));
	mdc_Canal = (UCHAR *) MEM_p_Alloc(TAB_ul_PFtable_GetNbElems(mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel->pst_AllObjects));

	i_ScrollH = LB->GetScrollPos(SB_HORZ);
	i_ScrollV = LB->GetScrollPos(SB_VERT);
	i_TopIndex = LB->GetTopIndex();

	M_MF()->LockDisplay(LB);
	LB->ResetContent();

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel->pst_AllObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel->pst_AllObjects);

	/* Add all Skeleton bones */
	for(index = 0; pst_PFElem <= pst_PFLastElem; index++, pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(pst_Shape)
			mdc_Canal[index] = pst_Shape->auc_AI_Canal[index];
		else
			mdc_Canal[index] = -1;

		L_strcpy(asz_Name, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_GO)));
		o_Str = asz_Name;
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Group)) o_Str += "*";
		LB->InsertString(index, o_Str);
		LB->SetItemData(index, (DWORD) pst_GO);
	}

	/* Update scroll and top index */
	if(i_ScrollH != LB->GetScrollPos(SB_HORZ))
	{
		LB->SetScrollPos(SB_HORZ, i_ScrollH);
		LB->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (i_ScrollH << 16), 0);
	}

	if(i_ScrollV != LB->GetScrollPos(SB_VERT))
	{
		LB->SetScrollPos(SB_VERT, i_ScrollV);
		LB->SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (i_ScrollV << 16), 0);
	}

	Initialize();

	LB->SetCurSel(0);
	OnSelChange();

	LINK_RefreshMenu();
	M_MF()->UnlockDisplay(LB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ShapeDialog::OnSelChange(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	CComboBox			*CB;
	CEdit				*oEdCanal;
	OBJ_tdst_GameObject *pst_BoneGO, *pst_VisuGO;
	OBJ_tdst_Group		*pst_Group;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	CString				o_Str;
	char				sz_Value[10];
	int					index, canal;
	char				asz_Name[100];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LB = (CListBox *) GetDlgItem(IDC_LISTBONES);
	CB = (CComboBox *) GetDlgItem(IDC_COMBOVISU);
	oEdCanal = (CEdit *) GetDlgItem(IDC_CANAL);

	if(LB->GetCurSel() == LB_ERR) return;

	/* Resets content. */
	CB->ResetContent();

	pst_BoneGO = (OBJ_tdst_GameObject *) LB->GetItemData(LB->GetCurSel());

	if(pst_BoneGO && OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Group))
	{
		o_Str = "Keep current";
		index = CB->InsertString(0, o_Str);

		o_Str = "Invisible";
		index = CB->InsertString(1, o_Str);

		pst_Group = pst_BoneGO->pst_Extended->pst_Group;
		pst_PFElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
		pst_PFLastElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);

		/* Add all visuals */
		for(index = 2; pst_PFElem <= pst_PFLastElem; index++, pst_PFElem++)
		{
			pst_VisuGO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
			if(TAB_b_IsAHole(pst_VisuGO)) continue;
			L_strcpy(asz_Name, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_VisuGO)));
			o_Str = asz_Name;
			CB->InsertString(index, o_Str);
			CB->SetItemData(index, (DWORD) pst_VisuGO);
		}
	}
	else
	{
		o_Str = "Keep current";
		index = CB->InsertString(0, o_Str);

		o_Str = "Invisible";
		index = CB->InsertString(1, o_Str);

		L_strcpy(asz_Name, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) pst_BoneGO)));
		o_Str = asz_Name;
		index = CB->InsertString(2, o_Str);
	}

	index = TAB_ul_PFtable_GetElemIndexWithPointer
		(
			mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel->pst_AllObjects,
			pst_BoneGO
		);
	canal = mdc_Canal[index];

	oEdCanal->SetWindowText(_itoa(canal, sz_Value, 10));

	CB->SetCurSel(mdc_Visual[index]);
}

#ifdef JADEFUSION
extern UCHAR ANI_uc_GetEngineCanalByAICanal(OBJ_tdst_GameObject *, UCHAR);
#else
extern "C" UCHAR ANI_uc_GetEngineCanalByAICanal(OBJ_tdst_GameObject *, UCHAR);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ShapeDialog::OnUpdateVisu(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					index, visu, realvisu;
	OBJ_tdst_GameObject *pst_BoneGO;
	CListBox			*LB;
	CComboBox			*CB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!mpst_Object)
	||	(!mpst_Object->pst_Base)
	||	(!mpst_Object->pst_Base->pst_GameObjectAnim)
	||	(!mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel)
	) return;

	if(!OBJ_b_TestIdentityFlag(mpst_Object, OBJ_C_IdentityFlag_Anims)) return;

	LB = (CListBox *) GetDlgItem(IDC_LISTBONES);
	CB = (CComboBox *) GetDlgItem(IDC_COMBOVISU);

	pst_BoneGO = (OBJ_tdst_GameObject *) LB->GetItemData(LB->GetCurSel());
	if(!pst_BoneGO) return;
	visu = CB->GetCurSel();

	index = TAB_ul_PFtable_GetElemIndexWithPointer
		(
			mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel->pst_AllObjects,
			pst_BoneGO
		);

	mdc_Visual[index] = visu;

	switch(visu)
	{
	case 0:
		return;

	case 1:
		realvisu = (char) 0xFD; /* Flag to Force bone invisible */
		break;

	default:
		realvisu = visu - 2;
		break;
	}

	ANI_ChangeBoneShape(mpst_Object, index, realvisu, FALSE);

	if((index != 255) && mpst_Object->pst_Base->pst_GameObjectAnim->pst_Shape)
	{
		mpst_Object->pst_Base->pst_GameObjectAnim->pst_Shape->auc_Visu[index] = realvisu;
		mpst_Object->pst_Base->pst_GameObjectAnim->auc_DefaultVisu[index] = realvisu;
	}

	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ShapeDialog::OnUpdateCanal(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					index;
	OBJ_tdst_GameObject *pst_BoneGO;
	CString				o_String;
	CListBox			*LB;
	CEdit				*oEdCanal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!mpst_Object)
	||	(!mpst_Object->pst_Base)
	||	(!mpst_Object->pst_Base->pst_GameObjectAnim)
	||	(!mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel)
	) return;
	if(!OBJ_b_TestIdentityFlag(mpst_Object, OBJ_C_IdentityFlag_Anims)) return;

	LB = (CListBox *) GetDlgItem(IDC_LISTBONES);
	oEdCanal = (CEdit *) GetDlgItem(IDC_CANAL);

	pst_BoneGO = (OBJ_tdst_GameObject *) LB->GetItemData(LB->GetCurSel());
	if(!pst_BoneGO) return;

	oEdCanal->GetWindowText(o_String);

	index = TAB_ul_PFtable_GetElemIndexWithPointer
		(
			mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel->pst_AllObjects,
			pst_BoneGO
		);
	mdc_Canal[index] = atoi((char *) (LPCSTR) o_String);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_ShapeDialog::CheckShape(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox	*LB;
	int			NbBones, i, j;
	BOOL		b_Modif;
	OBJ_tdst_GameObject *pst_BoneGO1, *pst_BoneGO2;
	char		asz_Text[200];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!mpst_Object)
	||	(!mpst_Object->pst_Base)
	||	(!mpst_Object->pst_Base->pst_GameObjectAnim)
	||	(!mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel)
	) return FALSE;

	if(!OBJ_b_TestIdentityFlag(mpst_Object, OBJ_C_IdentityFlag_Anims)) return FALSE;

	b_Modif = FALSE;
	LB = (CListBox *) GetDlgItem(IDC_LISTBONES);
	NbBones = LB->GetCount();

	for(i = 0; i < NbBones; i++)
	{
		for(j = i + 1; j < NbBones; j++)
		{
			if((mdc_Canal[i] != 255) && (mdc_Canal[i] == mdc_Canal[j]))
			{
				pst_BoneGO1 = (OBJ_tdst_GameObject *) LB->GetItemData(i);
				pst_BoneGO2 = (OBJ_tdst_GameObject *) LB->GetItemData(j);

				sprintf(asz_Text, "Bones %s and %s share the same Canal", pst_BoneGO1->sz_Name, pst_BoneGO2->sz_Name);
				M_MF()->MessageBox(asz_Text, "Error", MB_OK);
				return FALSE;
			}
		}

		if(mdc_Visual[i] != 0) b_Modif = TRUE;
	}

	return b_Modif;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ShapeDialog::OnSave(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog("Enter Shape Name (Without extension)");
	OBJ_tdst_GameObject *pst_BoneGO;
	ANI_tdst_Shape		*pst_Shape;
	BIG_INDEX			ul_Index;
	CListBox			*LB;
	char				NbBones, i, c_Save;
	char				NbModif;
	char				asz_Name[BIG_C_MaxLenPath], *psz_Temp;
	char				asz_Path[BIG_C_MaxLenPath];
	BOOL				b_OverWrite;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!mpst_Object)
	||	(!mpst_Object->pst_Base)
	||	(!mpst_Object->pst_Base->pst_GameObjectAnim)
	||	(!mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel)
	) return;

	if(!OBJ_b_TestIdentityFlag(mpst_Object, OBJ_C_IdentityFlag_Anims)) return;
	if(CheckShape())
	{
		pst_Shape = mpst_Object->pst_Base->pst_GameObjectAnim->pst_Shape;
		if(pst_Shape)
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Shape);
			if(ul_Index != BIG_C_InvalidIndex)
			{
				L_strcpy(asz_Name, BIG_NameFile(ul_Index));
				psz_Temp = L_strrchr(asz_Name, '.');
				if(*psz_Temp) *psz_Temp = 0;

				o_Dialog.mo_Name = asz_Name;
			}
		}

		if(o_Dialog.DoModal() == IDOK)
		{
			if(!L_strcmp(o_Dialog.mo_Name, asz_Name))
				b_OverWrite = TRUE;
			else
				b_OverWrite = FALSE;


			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel);
			BIG_ComputeFullName(BIG_ParentDir(BIG_ParentFile(ul_Index)), asz_Path);
			L_strcat(asz_Path, "/Shapes");
			L_strcpy(asz_Name, (char *) (LPCSTR) o_Dialog.mo_Name);
			L_strcat(asz_Name, EDI_Csz_ExtShape);

			LB = (CListBox *) GetDlgItem(IDC_LISTBONES);
			NbBones = (char) LB->GetCount();
			NbModif = NbBones;

			SAV_Begin(asz_Path, asz_Name);
			SAV_Buffer(&NbModif, 1);
			for(i = 0; i < NbBones; i++)
			{
				pst_BoneGO = (OBJ_tdst_GameObject *) LB->GetItemData(i);

				if(b_OverWrite)
					pst_Shape->auc_AI_Canal[i] = mdc_Canal[i];			

				SAV_Buffer(&i, 1);
				SAV_Buffer(mdc_Canal + i, 1);

				if(mdc_Visual[i] == 1)
				{
					c_Save = (char) 0xFD;	/* Flag to Force bone invisible */
					SAV_Buffer(&c_Save, 1);
					continue;
				}

				c_Save = mdc_Visual[i] - 2;
				SAV_Buffer(&c_Save, 1);
			}

			SAV_ul_End();
//			M_MF()->MessageBox("Shape saved", "Result", MB_OK);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ShapeDialog::OnReset(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	CListBox	*LB;
	CComboBox	*CB;
	char		NbBones, i;
	/*~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Object) return;
	if(!OBJ_b_TestIdentityFlag(mpst_Object, OBJ_C_IdentityFlag_Anims)) return;

	LB = (CListBox *) GetDlgItem(IDC_LISTBONES);
	CB = (CComboBox *) GetDlgItem(IDC_COMBOVISU);
	NbBones = (char) LB->GetCount();

	for(i = 0; i < NbBones; i++)
	{
		mdc_Visual[i] = 0;
		mdc_Canal[i] = -1;
	}

	OnSelChange();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ShapeDialog::OnInvisible(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	CListBox	*LB;
	CComboBox	*CB;
	char		NbBones, i;
	/*~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Object) return;
	if(!OBJ_b_TestIdentityFlag(mpst_Object, OBJ_C_IdentityFlag_Anims)) return;

	LB = (CListBox *) GetDlgItem(IDC_LISTBONES);
	CB = (CComboBox *) GetDlgItem(IDC_COMBOVISU);
	NbBones = (char) LB->GetCount();

	for(i = 0; i < NbBones; i++)
	{
		mdc_Visual[i] = 1;
		mdc_Canal[i] = -1;
	}

	OnSelChange();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ShapeDialog::OnClear(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*LB;
	CComboBox			*CB;
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_BoneGO;
	char				NbBones, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Object) return;
	if(!OBJ_b_TestIdentityFlag(mpst_Object, OBJ_C_IdentityFlag_Anims)) return;

	if(mpst_Object->pst_Base->pst_GameObjectAnim->pst_DefaultShape)
	{
		if
		(
			M_MF()->MessageBox
				(
					"Do you really want to remove the shape from this actor?",
					"Question",
					MB_ICONQUESTION | MB_YESNO
				) == IDNO
		) return;
	}
	else
	{
		if
		(
			M_MF()->MessageBox
				(
					"Do you really want to clear this new shape ?",
					"Question",
					MB_ICONQUESTION | MB_YESNO
				) == IDNO
		) return;
	}

	LB = (CListBox *) GetDlgItem(IDC_LISTBONES);
	CB = (CComboBox *) GetDlgItem(IDC_COMBOVISU);
	NbBones = (char) LB->GetCount();

	for(i = 0; i < NbBones; i++)
	{
		mdc_Visual[i] = 2;
	}

	pst_PFElem = TAB_pst_PFtable_GetFirstElem(mpst_Object->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(mpst_Object->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);

	for(i = 0; pst_PFElem <= pst_PFLastElem; i++, pst_PFElem++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO)) continue;

		if(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Group))
			ANI_ChangeBoneShape(mpst_Object, i, 0, FALSE);
		else
			ANI_ChangeBoneShape(mpst_Object, i, 0xFE, FALSE);
	}

	if(mpst_Object->pst_Base->pst_GameObjectAnim->pst_DefaultShape)
	{
		ANI_FreeShape(&mpst_Object->pst_Base->pst_GameObjectAnim->pst_DefaultShape);
		mpst_Object->pst_Base->pst_GameObjectAnim->pst_DefaultShape = NULL;
		mpst_Object->pst_Base->pst_GameObjectAnim->pst_Shape = NULL;
	}

	UpdateList(TRUE);
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ShapeDialog::Initialize(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	int					index;
	OBJ_tdst_GameObject *pst_BoneGO;
	ANI_tdst_Shape		*pst_Shape;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(!mpst_Object)
	||	(!mpst_Object->pst_Base)
	||	(!mpst_Object->pst_Base->pst_GameObjectAnim)
	||	(!mpst_Object->pst_Base->pst_GameObjectAnim->pst_SkeletonModel)
	) return;

	if(!OBJ_b_TestIdentityFlag(mpst_Object, OBJ_C_IdentityFlag_Anims)) return;

	pst_Shape = mpst_Object->pst_Base->pst_GameObjectAnim->pst_Shape;
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(mpst_Object->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(mpst_Object->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);

	for(index = 0; pst_PFElem <= pst_PFLastElem; index++, pst_PFElem++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO)) continue;

		if(pst_Shape)
		{
			if(pst_Shape->auc_Visu[index] == 0xFF)
			{
				mdc_Visual[index] = 0;
				continue;
			}

			if(pst_Shape->auc_Visu[index] == 0xFD)
			{
				mdc_Visual[index] = 1;
				continue;
			}
		}
		{
			if(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Group))
			{
				if(pst_Shape)
				{
					mdc_Visual[index] = pst_Shape->auc_Visu[index] + 2;
				}
				else
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					TAB_tdst_PFelem		*pst_PFElemGrp, *pst_PFLastElemGrp;
					int					indexgrp;
					OBJ_tdst_GameObject *pst_VisuGO;
					OBJ_tdst_Group		*pst_Group;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_Group = pst_BoneGO->pst_Extended->pst_Group;
					pst_PFElemGrp = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
					pst_PFLastElemGrp = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);

					for(indexgrp = 0; pst_PFElemGrp <= pst_PFLastElemGrp; indexgrp++, pst_PFElemGrp++)
					{
						pst_VisuGO = (OBJ_tdst_GameObject *) pst_PFElemGrp->p_Pointer;
						if(TAB_b_IsAHole(pst_VisuGO)) continue;
						if(!pst_BoneGO->pst_Base) continue;
						if(!pst_BoneGO->pst_Base->pst_Visu) continue;
						if(!pst_VisuGO->pst_Base) continue;
						if(!pst_VisuGO->pst_Base->pst_Visu) continue;

						if(pst_BoneGO->pst_Base->pst_Visu->pst_Object == pst_VisuGO->pst_Base->pst_Visu->pst_Object)
							break;
					}

					if(pst_PFElemGrp > pst_PFLastElemGrp)
						mdc_Visual[index] = 2;
					else
						mdc_Visual[index] = indexgrp + 2;
				}
			}
			else
			{
				mdc_Visual[index] = 2;
			}
		}
	}

	OnSelChange();
}

#endif /* ACTIVE_EDITORS */
