/*$T DIAcreategeometry_inside.cpp GC! 1.081 03/26/01 11:41:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "BASe/BAStypes.h"
#include "DIAlogs/DIAcreategeometry_dlg.h"
#include "DIAlogs/DIAcreategeometry_inside.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "Res/Res.h"
#include "VAVview/VAVlist.h"
#include "LINKs/LINKtoed.h"
#include "SELection/SELection.h"
#include "F3Dframe/F3Dview.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "EDIpaths.h"

#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLinit.h"

#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"



/*$4
 ***********************************************************************************************************************
    Constant
 ***********************************************************************************************************************
 */

#define C_3DText	0
#define C_Plane		1
#define C_Box		2
#define C_Sphere	3
#define C_Cylindre	4
#define C_Geosphere 5

#define C_Number	6

/*$4
 ***********************************************************************************************************************
    globals
 ***********************************************************************************************************************
 */

char	*CreateGeometry_gasz_Name[C_Number] = { "3D text", "Plane", "Box", "Sphere", "Cylindre", "Geosphere" };

IMPLEMENT_DYNCREATE(EDIA_cl_CreateGeometryView, CFormView)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_CreateGeometryView::EDIA_cl_CreateGeometryView(void) :
	CFormView(DIALOGS_IDD_CREATEGEOM_INSIDE)
{
	mi_Create = -1;
	mpst_Object = NULL;
	mpst_Cob = NULL;
    mb_UserName = FALSE;

	mst_TextData.f_Espacement = 0.001f;
	mst_TextData.f_Height = 0.01f;
	mst_TextData.f_Width = 0.01f;
	mst_TextData.i_NbChar = 6;
    mst_TextData.f_Radius = 0;

	mst_PlaneData.x = 10.0f;
	mst_PlaneData.y = 10.0f;
	mst_PlaneData.i_NbX = 10;
	mst_PlaneData.i_NbY = 10;

    mst_BoxData.x = 10.0f;
	mst_BoxData.y = 10.0f;
	mst_BoxData.z = 10.0f;
	mst_BoxData.i_NbX = 10;
	mst_BoxData.i_NbY = 10;
	mst_BoxData.i_NbZ = 10;
    mst_BoxData.i_NbElem = 1;

    mst_CylindreData.f_Radius = 1.0f;
    mst_CylindreData.z = 1.0f;
    mst_CylindreData.i_NbCapSegs = 1;
    mst_CylindreData.i_NbSides = 8;
    mst_CylindreData.i_NbZSegs = 1;

    mst_SphereData.f_Radius = 10.0f;
    mst_SphereData.i_NbSeg = 8;

    mst_GeosphereData.f_Radius = 10.0f;
    mst_GeosphereData.i_NbSeg = 8;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_CreateGeometryView::~EDIA_cl_CreateGeometryView(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_CreateGeometryView::Create
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

/*$4
 ***********************************************************************************************************************
    Message map
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_CreateGeometryView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_TEXT, OnButtonText)
	ON_BN_CLICKED(IDC_BUTTON_PLANE, OnButtonPlane)
	ON_BN_CLICKED(IDC_BUTTON_BOX, OnButtonBox)
    ON_BN_CLICKED(IDC_BUTTON_SPHERE, OnButtonSphere)
    ON_BN_CLICKED(IDC_BUTTON_GEOSPHERE, OnButtonGeosphere)
    ON_BN_CLICKED(IDC_BUTTON_CYLINDRE, OnButtonCylindre)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEPATH, OnButtonBrowsePath)
    ON_EN_CHANGE(IDC_EDIT_NAME, OnEditName )
	ON_CBN_SELCHANGE(IDC_COMBO_ASSOCIATE, OnCbnSelchangeCombo)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::OnButtonText(void)
{
	mpo_Dialog->mpo_DataView->ResetList();

	mpo_Dialog->mpo_DataView->AddItem("Letter number", EVAV_EVVIT_Int, &mst_TextData.i_NbChar, 0, 1, 200);
	mpo_Dialog->mpo_DataView->AddItem("Letter width", EVAV_EVVIT_Float, &mst_TextData.f_Width);
	mpo_Dialog->mpo_DataView->AddItem("Letter height", EVAV_EVVIT_Float, &mst_TextData.f_Height);
	mpo_Dialog->mpo_DataView->AddItem("Letter espacement", EVAV_EVVIT_Float, &mst_TextData.f_Espacement);
    mpo_Dialog->mpo_DataView->AddItem("Radius (circular text)", EVAV_EVVIT_Float, &mst_TextData.f_Radius);

	mpo_Dialog->mpo_DataView->SetItemList(&mpo_Dialog->mo_ListItems);
	Update_Create(C_3DText);
    Generate_Name(C_3DText);
	mpst_Object = NULL;
	mpst_Cob = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::OnButtonPlane(void)
{
	mpo_Dialog->mpo_DataView->ResetList();

	mpo_Dialog->mpo_DataView->AddItem("Width (X)", EVAV_EVVIT_Float, &mst_PlaneData.x);
	mpo_Dialog->mpo_DataView->AddItem("Length (Y)", EVAV_EVVIT_Float, &mst_PlaneData.y);
	mpo_Dialog->mpo_DataView->AddItem("Width subdivision", EVAV_EVVIT_Int, &mst_PlaneData.i_NbX, 0, 1, 1000);
	mpo_Dialog->mpo_DataView->AddItem("Length subdivision", EVAV_EVVIT_Int, &mst_PlaneData.i_NbY, 0, 1, 1000);

	mpo_Dialog->mpo_DataView->SetItemList(&mpo_Dialog->mo_ListItems);
	Update_Create(C_Plane);
    Generate_Name(C_Plane);
	mpst_Object = NULL;
	mpst_Cob = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::OnButtonBox(void)
{
	mpo_Dialog->mpo_DataView->ResetList();

	mpo_Dialog->mpo_DataView->AddItem("Width (X)", EVAV_EVVIT_Float, &mst_BoxData.x);
	mpo_Dialog->mpo_DataView->AddItem("Length (Y)", EVAV_EVVIT_Float, &mst_BoxData.y);
	mpo_Dialog->mpo_DataView->AddItem("Height (Z)", EVAV_EVVIT_Float, &mst_BoxData.z);
	mpo_Dialog->mpo_DataView->AddItem("Width subdivision", EVAV_EVVIT_Int, &mst_BoxData.i_NbX, 0, 1, 1000);
	mpo_Dialog->mpo_DataView->AddItem("Length subdivision", EVAV_EVVIT_Int, &mst_BoxData.i_NbY, 0, 1, 1000);
	mpo_Dialog->mpo_DataView->AddItem("Height subdivision", EVAV_EVVIT_Int, &mst_BoxData.i_NbZ, 0, 1, 1000);
    mpo_Dialog->mpo_DataView->AddItem("Element number", EVAV_EVVIT_Int, &mst_BoxData.i_NbElem, 0, 1, 6);

	mpo_Dialog->mpo_DataView->SetItemList(&mpo_Dialog->mo_ListItems);

	Update_Create(C_Box);
    Generate_Name(C_Box);
	mpst_Object = NULL;
	mpst_Cob = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::OnButtonCylindre(void)
{
	mpo_Dialog->mpo_DataView->ResetList();

	mpo_Dialog->mpo_DataView->AddItem("Radius", EVAV_EVVIT_Float, &mst_CylindreData.f_Radius);
	mpo_Dialog->mpo_DataView->AddItem("Height (Z)", EVAV_EVVIT_Float, &mst_CylindreData.z);
	mpo_Dialog->mpo_DataView->AddItem("Height segments", EVAV_EVVIT_Int, &mst_CylindreData.i_NbZSegs, 0, 1, 100);
	mpo_Dialog->mpo_DataView->AddItem("Cap segments", EVAV_EVVIT_Int, &mst_CylindreData.i_NbCapSegs, 0, 1, 100);
	mpo_Dialog->mpo_DataView->AddItem("Sides", EVAV_EVVIT_Int, &mst_CylindreData.i_NbSides, 0, 3, 100);

	mpo_Dialog->mpo_DataView->SetItemList(&mpo_Dialog->mo_ListItems);

	Update_Create(C_Cylindre);
    Generate_Name(C_Cylindre);
	mpst_Object = NULL;
	mpst_Cob = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::OnButtonSphere(void)
{
	mpo_Dialog->mpo_DataView->ResetList();

	mpo_Dialog->mpo_DataView->AddItem("Radius", EVAV_EVVIT_Float, &mst_SphereData.f_Radius);
	mpo_Dialog->mpo_DataView->AddItem("Segments", EVAV_EVVIT_Int, &mst_SphereData.i_NbSeg, 0, 4, 200);

	mpo_Dialog->mpo_DataView->SetItemList(&mpo_Dialog->mo_ListItems);

	Update_Create(C_Sphere);
    Generate_Name(C_Sphere);
	mpst_Object = NULL;
	mpst_Cob = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::OnButtonGeosphere(void)
{
	mpo_Dialog->mpo_DataView->ResetList();

	mpo_Dialog->mpo_DataView->AddItem("Radius", EVAV_EVVIT_Float, &mst_GeosphereData.f_Radius);
	mpo_Dialog->mpo_DataView->AddItem("Segments", EVAV_EVVIT_Int, &mst_GeosphereData.i_NbSeg, 0, 1, 32);

	mpo_Dialog->mpo_DataView->SetItemList(&mpo_Dialog->mo_ListItems);

	Update_Create(C_Geosphere);
    Generate_Name(C_Geosphere);
	mpst_Object = NULL;
	mpst_Cob = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::OnButtonCreate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap		*pst_ColMap;
	ULONG				ul_Index, ul_Flags, ul_OldFlags;
	OBJ_tdst_GameObject *pst_Gao;
	CComboBox			*po_CB;
	char				*sz_Ext;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_EDIT_PATH)->GetWindowText(msz_Path, BIG_C_MaxLenPath);
	GetDlgItem(IDC_EDIT_NAME)->GetWindowText(msz_Name, BIG_C_MaxLenName);

	/* check name and path */
	if(!msz_Name[0])
	{
		M_MF()->MessageBox("Type a name", "Error", MB_OK | MB_ICONERROR);
		GetDlgItem(IDC_EDIT_NAME)->SetFocus();
		return;
	}

	if((ul_Index = BIG_ul_CreateDir(msz_Path)) == BIG_C_InvalidIndex)
	{
		M_MF()->MessageBox("Error while creating directory", "Error", MB_OK | MB_ICONERROR);
		GetDlgItem(IDC_EDIT_PATH)->SetFocus();
		return;
	}

	/* be sure of gro extension */
	if((sz_Ext = L_strrchr(msz_Name, '.')) == NULL) sz_Ext = msz_Name + L_strlen(msz_Name);
	if(mpo_Dialog->mi_ColMap)
		L_strcpy(sz_Ext, EDI_Csz_ExtCOLObject);
	else
		L_strcpy(sz_Ext, EDI_Csz_ExtGraphicObject);
	Update_Name();

	/* check for existence */
	ul_Index = BIG_ul_SearchFile(ul_Index, msz_Name);
	if(ul_Index != BIG_C_InvalidIndex)
	{
		if(M_MF()->MessageBox("File already exist\nOverwrite ?", "Warning", MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			GetDlgItem(IDC_EDIT_NAME)->SetFocus();
			return;
		}
	}

	switch(mi_Create)
	{
	case C_3DText:
		ul_Index = GEO_ul_CO_Text(&mpst_Object, msz_Path, msz_Name, &mst_TextData);
		break;
    case C_Plane:
		if(mpo_Dialog->mi_ColMap)
			ul_Index = COL_ul_CO_Plane(&mpst_Cob, msz_Path, msz_Name, &mst_PlaneData);
		else
			ul_Index = GEO_ul_CO_Plane(&mpst_Object, msz_Path, msz_Name, &mst_PlaneData);
		break;
    case C_Box:
		if(mpo_Dialog->mi_ColMap)
			ul_Index = COL_ul_CO_Box(&mpst_Cob, msz_Path, msz_Name, &mst_BoxData);
		else
			ul_Index = GEO_ul_CO_Box(&mpst_Object, msz_Path, msz_Name, &mst_BoxData);
		break;
    case C_Sphere:
		if(mpo_Dialog->mi_ColMap)
			ul_Index = COL_ul_CO_Sphere(&mpst_Cob, msz_Path, msz_Name, &mst_SphereData);
		else
			ul_Index = GEO_ul_CO_Sphere(&mpst_Object, msz_Path, msz_Name, &mst_SphereData);
		break;
    case C_Cylindre:
		if(mpo_Dialog->mi_ColMap)
			ul_Index = COL_ul_CO_Cylindre(&mpst_Cob, msz_Path, msz_Name, &mst_CylindreData);
		else
			ul_Index = GEO_ul_CO_Cylindre(&mpst_Object, msz_Path, msz_Name, &mst_CylindreData);
		break;
	case C_Geosphere:
		if(mpo_Dialog->mi_ColMap)
			ul_Index = COL_ul_CO_Geosphere(&mpst_Cob, msz_Path, msz_Name, &mst_GeosphereData);
		else
			ul_Index = GEO_ul_CO_Geosphere(&mpst_Object, msz_Path, msz_Name, &mst_GeosphereData);
		break;
	}

	if(ul_Index == BIG_C_InvalidKey) return;

	Update_ItemCallback();

	if(mpo_Dialog->mi_ColMap)
	{
		if(((CButton *) GetDlgItem(IDC_CHECK_ASSOCIATE))->GetCheck())
		{
			po_CB = (CComboBox *) GetDlgItem(IDC_COMBO_ASSOCIATE);
			pst_Gao = (OBJ_tdst_GameObject *) po_CB->GetItemData(po_CB->GetCurSel());
			if ((LONG) pst_Gao != -1)
			{
				/* Allocate ColMap if needed */
				if(!(pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap))
				{
					OBJ_GameObject_CreateExtendedIfNot(pst_Gao);
					if(!pst_Gao->pst_Extended->pst_Col)
					{
						pst_Gao->pst_Extended->pst_Col = (COL_tdst_Base *) MEM_p_Alloc(sizeof(COL_tdst_Base));
						L_memset(pst_Gao->pst_Extended->pst_Col, 0, sizeof(COL_tdst_Base));

						if((pst_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) && !(pst_Gao->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Detection))
						{
							OBJ_SetStatusFlag(pst_Gao, OBJ_C_StatusFlag_Detection);
							COL_AllocDetectionList(pst_Gao);
						}

					}

					if(!((COL_tdst_Base *)pst_Gao->pst_Extended->pst_Col)->pst_ColMap)
					{
						pst_Gao->ul_IdentityFlags |= OBJ_C_IdentityFlag_ColMap;
						((COL_tdst_Base *)pst_Gao->pst_Extended->pst_Col)->pst_ColMap = (COL_tdst_ColMap *) MEM_p_Alloc(sizeof(COL_tdst_ColMap));
						L_memset(((COL_tdst_Base *)pst_Gao->pst_Extended->pst_Col)->pst_ColMap, 0, sizeof(COL_tdst_ColMap));
					}
				}

				pst_ColMap = ((COL_tdst_Base *)pst_Gao->pst_Extended->pst_Col)->pst_ColMap;

				/* Delete previous Cobs */
				if(pst_ColMap->uc_NbOfCob)
				{
					COL_tdst_Cob	**dpst_Cob, **dpst_LastCob;

					dpst_Cob = pst_ColMap->dpst_Cob;
					dpst_LastCob = dpst_Cob + pst_ColMap->uc_NbOfCob;
					for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
					{
						/*~~~~~~~~~~~~~~~~*/
						BOOL	b_Triangles;
						/*~~~~~~~~~~~~~~~~*/

						b_Triangles = (COL_Cob_GetType(*dpst_Cob) == COL_C_Zone_Triangles);
						(*dpst_Cob)->uw_NbOfInstances--;

						if(!(*dpst_Cob)->uw_NbOfInstances)
						{
							COL_FreeGameMaterial(*dpst_Cob);

							LOA_DeleteAddress(*dpst_Cob);
							LINK_DelRegisterPointer(*dpst_Cob);
							if(b_Triangles)
								COL_FreeTrianglesIndexedCob(*dpst_Cob);
							else
								COL_FreeMathematicalCob(*dpst_Cob);
						}
					}

					MEM_Free(pst_ColMap->dpst_Cob);
					pst_ColMap->uc_NbOfCob = 0;
					pst_ColMap->uc_Activation = 0xFF;
				}


				pst_ColMap->dpst_Cob = (COL_tdst_Cob **) MEM_p_Alloc(sizeof(COL_tdst_Cob *));
				pst_ColMap->uc_NbOfCob = 1;
				pst_ColMap->uc_Activation = 0xFF;
				*(pst_ColMap->dpst_Cob) = mpst_Cob;
			}
		}

		LINK_Refresh();
	}
	else
	{
		if(((CButton *) GetDlgItem(IDC_CHECK_ASSOCIATE))->GetCheck())
		{
			po_CB = (CComboBox *) GetDlgItem(IDC_COMBO_ASSOCIATE);
			pst_Gao = (OBJ_tdst_GameObject *) po_CB->GetItemData(po_CB->GetCurSel());
			if ((LONG) pst_Gao != -1)
			{
				if(!pst_Gao->pst_Base)
				{
					ul_OldFlags = pst_Gao->ul_IdentityFlags;
					ul_Flags = ul_OldFlags | OBJ_C_IdentityFlag_BaseObject | OBJ_C_IdentityFlag_Visu;
					OBJ_ChangeIdentityFlags(pst_Gao, ul_Flags, ul_OldFlags);
				}

				if((pst_Gao->pst_Base) && (pst_Gao->pst_Base->pst_Visu))
				{
					// force draw it flag
					pst_Gao->pst_Base->pst_Visu->ul_DrawMask |= GDI_Cul_DM_Draw;

					// detach previous object
					if (pst_Gao->pst_Base->pst_Visu->pst_Object)
						pst_Gao->pst_Base->pst_Visu->pst_Object->l_Ref--;

					// attach new object
					pst_Gao->pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) mpst_Object;
					mpst_Object->st_Id.l_Ref++;

					// if object not in table add it
					if(TAB_ul_Ptable_GetElemIndexWithPointer(&pst_Gao->pst_World->st_GraphicObjectsTable, mpst_Object) == TAB_Cul_BadIndex)
					{
						TAB_Ptable_AddElemAndResize(&pst_Gao->pst_World->st_GraphicObjectsTable, mpst_Object);
						mpst_Object->st_Id.i->pfn_AddRef(mpst_Object, 1);
					}
				}

				/* force gameobject to have an OOBBox  */
				ul_OldFlags = pst_Gao->ul_IdentityFlags;
				ul_Flags = ul_OldFlags | OBJ_C_IdentityFlag_OBBox;
				OBJ_ChangeIdentityFlags(pst_Gao, ul_Flags, ul_OldFlags);
			}

			LINK_Refresh();
		}
	}

	Update_Create(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::OnEditName( void )
{
    GetDlgItem(IDC_EDIT_NAME)->GetWindowText(msz_Name, BIG_C_MaxLenName);
    mb_UserName = (*msz_Name == 0) ? FALSE : TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::OnCbnSelchangeCombo( void )
{
	Generate_Name(0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::OnButtonBrowsePath(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Choose folder", 2, TRUE, TRUE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		L_strcpy(msz_Path, o_File.masz_FullPath);
		Update_Name();
	}
}

/*$4
 ***********************************************************************************************************************
    Function
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::Init(void)
{
	/* Cannot select the "3D Text" for ColMap. */
	if(mpo_Dialog->mi_ColMap)
	{
			GetDlgItem(IDC_BUTTON_TEXT)->EnableWindow(FALSE);
	}

	Update_Name();
	Update_GameObject();
	Update_Create(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::Update_Name(void)
{
    BOOL    b_SaveUserName;

    b_SaveUserName = mb_UserName;
	GetDlgItem(IDC_EDIT_PATH)->SetWindowText(msz_Path);
	GetDlgItem(IDC_EDIT_NAME)->SetWindowText(msz_Name);
    mb_UserName = b_SaveUserName;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::Generate_Name(int _i_Type)
{
	/*~~~~~~~~~~~~~~~~~*/
    BIG_INDEX   ul_Dir;
	CComboBox	*po_CB;
	int			i_CBindex, i_CBcount;
	CString		strGaoName;
	/*~~~~~~~~~~~~~~~~~*/

	po_CB = (CComboBox *) GetDlgItem(IDC_COMBO_ASSOCIATE);
	i_CBindex = po_CB->GetCurSel();
	i_CBcount = po_CB->GetCount();
	po_CB->GetLBText(i_CBindex, strGaoName);
	if ((i_CBindex != LB_ERR) && (i_CBcount > 0))
	{
		// Remove file extension if present
		if (strGaoName.Find(EDI_Csz_ExtGameObject) != -1)
			strGaoName.Replace(EDI_Csz_ExtGameObject,EDI_Csz_ExtGraphicObject);
		else
			strGaoName += EDI_Csz_ExtGraphicObject;

		sprintf(msz_Name, "%s", (LPSTR)(LPCSTR) strGaoName);
	}
	


    if (mb_UserName) return;

    GetDlgItem(IDC_EDIT_PATH)->GetWindowText(msz_Path, BIG_C_MaxLenPath);
    if((ul_Dir = BIG_ul_CreateDir(msz_Path)) == BIG_C_InvalidIndex)
		return;

    Update_Name();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::Update_Create(int _i_Type)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Name[100];
	/*~~~~~~~~~~~~~~~~~*/

	mi_Create = _i_Type;
	if(mi_Create == -1)
	{
		GetDlgItem(IDC_BUTTON_CREATE)->SetWindowText("Choose type");
		GetDlgItem(IDC_BUTTON_CREATE)->EnableWindow(FALSE);
	}
	else
	{
		sprintf(sz_Name, "Create %s", CreateGeometry_gasz_Name[mi_Create]);
		GetDlgItem(IDC_BUTTON_CREATE)->SetWindowText(sz_Name);
		GetDlgItem(IDC_BUTTON_CREATE)->EnableWindow(TRUE);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView::Update_GameObject(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_Selection		*pst_Sel;
	SEL_tdst_SelectedItem	*pst_Item;
	CComboBox				*po_CB;
	int						i_Index;
	OBJ_tdst_GameObject		*pst_Gao;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_CB = (CComboBox *) GetDlgItem(IDC_COMBO_ASSOCIATE);
	po_CB->ResetContent();
	pst_Sel = mpo_Dialog->mpo_3DView->mst_WinHandles.pst_World->pst_Selection;
	pst_Item = pst_Sel->pst_FirstItem;

	while(pst_Item)
	{
		if(pst_Item->l_Flag & SEL_C_SIF_Object)
		{
			pst_Gao = (OBJ_tdst_GameObject *) pst_Item->p_Content;
			if(pst_Gao->sz_Name)
			{
				i_Index = po_CB->AddString(pst_Gao->sz_Name);
				po_CB->SetItemData(i_Index, (DWORD) pst_Gao);
			}
		}

		pst_Item = pst_Item->pst_Next;
	}

	if(po_CB->GetCount()) 
    {
        po_CB->SetCurSel(0);
        ((CButton *) GetDlgItem(IDC_CHECK_ASSOCIATE))->SetCheck(1);
        GetDlgItem(IDC_CHECK_ASSOCIATE)->EnableWindow( TRUE );
        GetDlgItem(IDC_COMBO_ASSOCIATE)->EnableWindow( TRUE );
    }
    else
    {
        ((CButton *) GetDlgItem(IDC_CHECK_ASSOCIATE))->SetCheck(0);
        GetDlgItem(IDC_CHECK_ASSOCIATE)->EnableWindow( FALSE );
        GetDlgItem(IDC_COMBO_ASSOCIATE)->EnableWindow( FALSE );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CreateGeometryView_ItemCallback(void *_p_Owner, void *, void *p_Data, LONG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CFrameWnd					*po_Frame;
	EDIA_cl_CreateGeometry		*po_Dlg;
	EDIA_cl_CreateGeometryView	*po_CG;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Frame = (CFrameWnd *) _p_Owner;
	po_Dlg = (EDIA_cl_CreateGeometry *) po_Frame->GetParent();
	po_CG = po_Dlg->mpo_View;

	
	if(!(po_CG->mpst_Object) || (po_CG->mi_Object == -1)) return;

	switch(po_CG->mi_Object)
	{
	case C_3DText:
		GEO_ul_CO_Text(&po_CG->mpst_Object, NULL, NULL, &po_CG->mst_TextData);
		break;
    case C_Plane:
		if(po_Dlg->mi_ColMap)
			COL_ul_CO_Plane(&po_CG->mpst_Cob, NULL, NULL, &po_CG->mst_PlaneData);
		else
			GEO_ul_CO_Plane(&po_CG->mpst_Object, NULL, NULL, &po_CG->mst_PlaneData);
		break;
    case C_Box:
		if(po_Dlg->mi_ColMap)
			COL_ul_CO_Box(&po_CG->mpst_Cob, NULL, NULL, &po_CG->mst_BoxData);
		else
			GEO_ul_CO_Box(&po_CG->mpst_Object, NULL, NULL, &po_CG->mst_BoxData);
		break;
    case C_Cylindre:
		if(po_Dlg->mi_ColMap)
			COL_ul_CO_Cylindre(&po_CG->mpst_Cob, NULL, NULL, &po_CG->mst_CylindreData);
		else
			GEO_ul_CO_Cylindre(&po_CG->mpst_Object, NULL, NULL, &po_CG->mst_CylindreData);
		break;
    case C_Sphere:
		if(po_Dlg->mi_ColMap)
			COL_ul_CO_Sphere(&po_CG->mpst_Cob, NULL, NULL, &po_CG->mst_SphereData);
		else
			GEO_ul_CO_Sphere(&po_CG->mpst_Object, NULL, NULL, &po_CG->mst_SphereData);
		break;
    case C_Geosphere:
		if(po_Dlg->mi_ColMap)
			COL_ul_CO_Geosphere(&po_CG->mpst_Cob, NULL, NULL, &po_CG->mst_GeosphereData);
		else
			GEO_ul_CO_Geosphere(&po_CG->mpst_Object, NULL, NULL, &po_CG->mst_GeosphereData);
		break;
	}

	LINK_Refresh();
}
/**/
void EDIA_cl_CreateGeometryView::Update_ItemCallback(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos;
	EVAV_cl_ViewItem	*po_Item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mi_Object = mi_Create;

	pos = mpo_Dialog->mst_DataView.po_ListItems->GetHeadPosition();
	while(pos)
	{
		po_Item = mpo_Dialog->mst_DataView.po_ListItems->GetNext(pos);
		po_Item->mpfn_CB = EDIA_cl_CreateGeometryView_ItemCallback;
	}
}

#endif /* ACTIVE_EDITORS */
