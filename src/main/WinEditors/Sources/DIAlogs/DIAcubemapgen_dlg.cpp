/*$T DIAreplace_dlg.cpp GC! 1.078 03/16/00 10:33:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAcubemapgen_dlg.h"
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
#include "DIAlogs/DIAfile_dlg.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXxenon.h"

#include "EDItors/Sources/OUTput/OUTframe.h"
#include "F3Dframe\F3Dframe.h"
#include "F3Dframe\F3Dview.h"
#include "GDInterface/GDInterface.h"
#include "EDImsg.h"
#include "LINks/LINKtoed.h"


#include <string>

BEGIN_MESSAGE_MAP(EDIA_cl_CubeMapGenDialog, EDIA_cl_BaseDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TEX_RES, OnSpinTexRes)
	ON_BN_CLICKED(IDOK, OnBtOK)
	ON_BN_CLICKED(IDC_CHECK_TEX_GENMIPMAP, OnCheckGenMipMap)
END_MESSAGE_MAP()

/*$4
***********************************************************************************************************************
Globals
***********************************************************************************************************************
*/

static char *TEX_XenonTextureFormat_Text[ TEX_XTF_COUNT ] =
        {
                "Auto",
                "DXT1    ARGB  (1 bit alpha)",
                "DXT5    ARGB  (Interpolated Alpha)",
                "A8      Alpha  (8 bit Alpha Only)",
                "8:8:8:8 ARGB  (32 bit)",
                "DXN",
                "CTX1",
};

/*
=======================================================================================================================
=======================================================================================================================
*/
EDIA_cl_CubeMapGenDialog::EDIA_cl_CubeMapGenDialog(struct OBJ_tdst_GameObject_ *O) :
EDIA_cl_BaseDialog(DIALOGS_IDD_CUBEMAPGENERATOR)
{
	mpst_Gao = O;
	mi_Size = CUBEMAP_START_RES;

	//set faces names 
	mstrFace[0] = "CubeMap_X_pos.tga";
	mstrFace[1] = "CubeMap_X_neg.tga";
	mstrFace[2] = "CubeMap_Y_pos.tga";
	mstrFace[3] = "CubeMap_Y_neg.tga";
	mstrFace[4] = "CubeMap_Z_pos.tga";
	mstrFace[5] = "CubeMap_Z_neg.tga";
}

/*
=======================================================================================================================
=======================================================================================================================
*/
EDIA_cl_CubeMapGenDialog::~EDIA_cl_CubeMapGenDialog()
{
	if(((F3D_cl_View *) mpo_View)->mpo_CubeMapGenDialog == this) 
		((F3D_cl_View *) mpo_View)->mpo_CubeMapGenDialog = NULL;
}

/*
=======================================================================================================================
=======================================================================================================================
*/void EDIA_cl_CubeMapGenDialog::DoDataExchange(CDataExchange* pDX)
{
	//CDialog::DoDataExchange(pDX);
	EDIA_cl_BaseDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_GAO_POS_X, GaoPosX);
	DDX_Control(pDX, IDC_EDIT_GAO_POS_Y, GaoPosY);
	DDX_Control(pDX, IDC_EDIT_GAO_POS_Z, GaoPosZ);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_CubeMapGenDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox	*poTexFormat;
	CComboBox	*poTexMipMap;
	CString		strTemp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	EDIA_cl_BaseDialog::OnInitDialog();

	FillGaoInfo();
	FillTexNameInfo();
	FillTexResInfo();

	//fill format combobox
	poTexFormat = (CComboBox*) GetDlgItem(IDC_TEX_COMBO_FORMAT);
	poTexFormat->ResetContent();
	for(int i = 1; i < TEX_XTF_COUNT; i++)
	{
		poTexFormat->AddString(TEX_XenonTextureFormat_Text[i]);
	}
	poTexFormat->SetCurSel(0);

	//fill mipmap combobox
	poTexMipMap = (CComboBox*) GetDlgItem(IDC_TEX_COMBO_MIPMAP);
	poTexMipMap->ResetContent();
	poTexMipMap->AddString("ALL");
	for(int i = 1; i <= 13; i++)
	{
		strTemp.Format("%d",i);
		poTexMipMap->AddString(strTemp);
	}
	poTexMipMap->SetCurSel(1);

	return TRUE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_CubeMapGenDialog::PreTranslateMessage(MSG *p_Msg)
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
void EDIA_cl_CubeMapGenDialog::OnSpinTexRes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;

	mi_Size *= pNMUpDown->iDelta < 0 ? 2 : 0.5;
	//borne value test
	if(mi_Size > CUBEMAP_MAX_RES)
		mi_Size = CUBEMAP_MAX_RES;
	if(mi_Size < CUBEMAP_MIN_RES)
		mi_Size = CUBEMAP_MIN_RES;
	
	FillTexResInfo();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_CubeMapGenDialog::OnBtOK(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_DisplayData_	*pst_DD;
	BOOL					b_Export;
	CString					strFullPath, strName;
	CString					strTemp;
	EOUT_cl_Frame			*po_3DV;
	char					sz_SaveSnapName[ 255 ];
	char					sz_SaveSnapPath[ 255 ];
	CAM_tdst_Camera			st_SaveCamera;
	LONG					l_SaveScreenRatioConst;
	LONG					l_SaveWidth, l_SaveHeight;
	SEL_tdst_Selection		*pst_Sel;
	float					f_SaveFOV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	

	//get Display Data
	po_3DV = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	if (!po_3DV) return;

	pst_DD = po_3DV->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData;

	//test the 3dView size
	if(pst_DD->st_Device.l_Width < mi_Size || pst_DD->st_Device.l_Height < mi_Size)
	{
		MessageBox( "Your 3DView must be bigger than the CubeMap Resolution!\nResize your 3DView before continue.", "Warning", MB_OK | MB_ICONSTOP);
		return;
	}
	
	b_Export = ((CButton *)GetDlgItem(IDC_CHECK_TEX_EXPORT))->GetCheck();
	EDIA_cl_FileDialog o_FileDlg("Save CubeMap in ...", 2, FALSE, !b_Export);

	if(o_FileDlg.DoModal() == IDOK)
	{
		//save camera
		L_memcpy(&st_SaveCamera, &pst_DD->st_Camera, sizeof(CAM_tdst_Camera));

		//save snapshot name and path
		L_strcpy(sz_SaveSnapName, pst_DD->sz_SnapshotName);
		L_strcpy(sz_SaveSnapPath, pst_DD->sz_SnapshotPath);

		//save params
		l_SaveWidth = pst_DD->st_Device.l_Width;
		l_SaveHeight = pst_DD->st_Device.l_Height;
		l_SaveScreenRatioConst = pst_DD->st_ScreenFormat.l_ScreenRatioConst;

		//save FOV
		f_SaveFOV = po_3DV->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_View[0].st_DisplayInfo.f_FieldOfVision;

		//set params for capture
		po_3DV->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_View[0].st_DisplayInfo.f_FieldOfVision = Cf_PiBy2 / 0.75f;
		pst_DD->st_Camera.f_ViewportWidth = (float) mi_Size;
		pst_DD->st_Camera.f_ViewportHeight = (float) mi_Size;

		pst_DD->st_Device.l_Width = mi_Size;
		pst_DD->st_Device.l_Height = mi_Size;
		pst_DD->st_ScreenFormat.l_ScreenRatioConst = 1; 

		//set snapshot path
		strcpy(pst_DD->sz_SnapshotPath, CUBEMAP_SNAPSHOT_PATH);

		//get file name and path
		strFullPath = o_FileDlg.masz_FullPath;
		GetDlgItem(IDC_EDIT_TEX_NAME)->GetWindowText(strName);
		if(strName.Find(EDI_Csz_ExtTextureCubeMap) == -1)
			strName += EDI_Csz_ExtTextureCubeMap;

		//set camera matrix position
		CAM_SetCameraPos(&pst_DD->st_Camera, &GetGenerationPos());
		
		//hide dialog
		this->ShowWindow(SW_HIDE);

		//clear selection list
		pst_Sel = po_3DV->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_Selection;
        while(!SEL_b_IsEmpty(pst_Sel))
		{
			((OBJ_tdst_GameObject *) pst_Sel->pst_FirstItem->p_Content)->ul_EditorFlags &= ~OBJ_C_EditFlags_Selected;
			SEL_DelItem(pst_Sel, pst_Sel->pst_FirstItem->p_Content);
		}

		//for each face
		for(int nFace = 0; nFace < CUBEMAP_FACE_NUM; ++nFace)
		{
			//set camera orientation
			SetCameraMatrixForFace(&pst_DD->st_Camera, (D3DCUBEMAP_FACES)nFace);
			
			//set snapshot name
			strcpy(pst_DD->sz_SnapshotName, (char *)(LPCSTR) mstrFace[nFace]);

			//Capture face
			pst_DD->uc_SnapshotFlag |= GDI_Cc_CaptureFlag_Running;
			pst_DD->uc_SnapshotFlag |= GDI_Cc_CaptureFlag_ForCBM;
			LINK_Refresh();
			pst_DD->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_Running;
			pst_DD->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_ForCBM;
		}

 		//create CubeMap
		b_CreateCubeMap(o_FileDlg.masz_FullPath, strName, b_Export);

		//restore snapshot name and path
		L_strcpy(pst_DD->sz_SnapshotName, sz_SaveSnapName);
		L_strcpy(pst_DD->sz_SnapshotPath, sz_SaveSnapPath);

		//restore viewport size
		pst_DD->st_Device.l_Width = l_SaveWidth;
		pst_DD->st_Device.l_Height = l_SaveHeight;

		//restore ScreenRatioConst
		pst_DD->st_ScreenFormat.l_ScreenRatioConst = l_SaveScreenRatioConst;

		//restore FOV
		po_3DV->mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_World->pst_View[0].st_DisplayInfo.f_FieldOfVision = f_SaveFOV;

		//restore camera
		L_memcpy(&pst_DD->st_Camera, &st_SaveCamera, sizeof(CAM_tdst_Camera));

		OnOK();
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_CubeMapGenDialog::OnCheckGenMipMap(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL b_GenMipMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_GenMipMap = ((CButton *)GetDlgItem(IDC_CHECK_TEX_GENMIPMAP))->GetCheck();

	//activate  or desactivate MipMap combobox
	((CComboBox *)GetDlgItem(IDC_TEX_COMBO_MIPMAP))->EnableWindow(b_GenMipMap);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_CubeMapGenDialog::ChangeGao(struct OBJ_tdst_GameObject_ *_pst_Gao)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EOUT_cl_Frame		*po_3DV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//get Display Data
	po_3DV = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);

	mpst_Gao = _pst_Gao;
	FillGaoInfo();
	FillTexNameInfo();
	FillTexResInfo();
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_CubeMapGenDialog::FillGaoInfo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Index;
	char	sz_Path[BIG_C_MaxLenPath];
	CString	strTemp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = mpst_Gao ? LOA_ul_SearchIndexWithAddress((ULONG) mpst_Gao) : BIG_C_InvalidIndex;
	if(ul_Index == BIG_C_InvalidIndex)
	{
		GetDlgItem(IDC_EDIT_GAO_NAME)->SetWindowText("");
		GetDlgItem(IDC_EDIT_GAO_KEY)->SetWindowText("");
		GetDlgItem(IDC_EDIT_GAO_PATH)->SetWindowText("");
		GetDlgItem(IDC_EDIT_GAO_POS_X)->SetWindowText("");
		GetDlgItem(IDC_EDIT_GAO_POS_Y)->SetWindowText("");
		GetDlgItem(IDC_EDIT_GAO_POS_Z)->SetWindowText("");
	}
	else
	{
		sprintf(sz_Path, "%08X", BIG_FileKey(ul_Index));
		GetDlgItem(IDC_EDIT_GAO_KEY)->SetWindowText(sz_Path);
		GetDlgItem(IDC_EDIT_GAO_NAME)->SetWindowText(BIG_NameFile(ul_Index));
		BIG_ComputeFullName(BIG_ParentFile(ul_Index), sz_Path);
		GetDlgItem(IDC_EDIT_GAO_PATH)->SetWindowText(sz_Path);

		strTemp.Format(_T("%.3f"), mpst_Gao->pst_GlobalMatrix->T.x);
		GetDlgItem(IDC_EDIT_GAO_POS_X)->SetWindowText(strTemp);
		strTemp.Format(_T("%.3f"), mpst_Gao->pst_GlobalMatrix->T.y);
		GetDlgItem(IDC_EDIT_GAO_POS_Y)->SetWindowText(strTemp);
		strTemp.Format(_T("%.3f"), mpst_Gao->pst_GlobalMatrix->T.z);
		GetDlgItem(IDC_EDIT_GAO_POS_Z)->SetWindowText(strTemp);
	}
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_CubeMapGenDialog::FillTexNameInfo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString strTemp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//fill texture name info
	strTemp = mpst_Gao ? mpst_Gao->sz_Name : "";
	strTemp.Replace(_T(EDI_Csz_ExtGameObject), _T(EDI_Csz_ExtTextureCubeMap));
	GetDlgItem(IDC_EDIT_TEX_NAME)->SetWindowText(strTemp);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_CubeMapGenDialog::FillTexResInfo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString strTemp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//fill texture resolution info
	strTemp.Format(_T("%d"), mi_Size);
	GetDlgItem(IDC_EDIT_TEX_RES_X)->SetWindowText(strTemp);
	GetDlgItem(IDC_EDIT_TEX_RES_Y)->SetWindowText(strTemp);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
MATH_tdst_Vector EDIA_cl_CubeMapGenDialog::GetGenerationPos(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector st_Pos;
	CString strTemp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetDlgItem(IDC_EDIT_GAO_POS_X)->GetWindowText(strTemp);
	st_Pos.x = (float) atof((char *) (LPCSTR) strTemp);
	GetDlgItem(IDC_EDIT_GAO_POS_Y)->GetWindowText(strTemp);
	st_Pos.y = (float) atof((char *) (LPCSTR) strTemp);
	GetDlgItem(IDC_EDIT_GAO_POS_Z)->GetWindowText(strTemp);
	st_Pos.z = (float) atof((char *) (LPCSTR) strTemp);

	return st_Pos;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_CubeMapGenDialog::SetCameraMatrixForFace(CAM_tdst_Camera *_pst_Cam, DWORD _dw_Face)
{	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector I, J, K;
	MATH_tdst_Matrix st_Matrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	//final orientation
	switch( _dw_Face )
	{
	case D3DCUBEMAP_FACE_POSITIVE_X:
		MATH_InitVector(&I, Cf_Zero, Cf_Zero, Cf_One);
		MATH_InitVector(&J, -Cf_One, Cf_Zero, Cf_Zero);
		MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
 		break; 
	case D3DCUBEMAP_FACE_NEGATIVE_X:
		MATH_InitVector(&I, Cf_Zero, Cf_Zero, -Cf_One);
		MATH_InitVector(&J, Cf_One, Cf_Zero, Cf_Zero);
		MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
		break;
	case D3DCUBEMAP_FACE_POSITIVE_Y:
		MATH_InitVector(&I, -Cf_One, Cf_Zero, Cf_Zero);
		MATH_InitVector(&J, Cf_Zero, -Cf_One, Cf_Zero);
		MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
		break;
	case D3DCUBEMAP_FACE_NEGATIVE_Y:
		MATH_InitVector(&I, -Cf_One, Cf_Zero, Cf_Zero);
		MATH_InitVector(&J, Cf_Zero, Cf_One, Cf_Zero);
		MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
		break;
	case D3DCUBEMAP_FACE_POSITIVE_Z:
		MATH_InitVector(&I, -Cf_One, Cf_Zero, Cf_Zero);
		MATH_InitVector(&J, Cf_Zero, Cf_Zero, -Cf_One);
		MATH_InitVector(&K, Cf_Zero, Cf_One, Cf_Zero);
		break;
	case D3DCUBEMAP_FACE_NEGATIVE_Z:
		MATH_InitVector(&I, Cf_One, Cf_Zero, Cf_Zero);
		MATH_InitVector(&J, Cf_Zero, Cf_Zero, Cf_One);
		MATH_InitVector(&K, Cf_Zero, Cf_One, Cf_Zero);
		break;
	default:
		break;
	} 
	
	//get Camera Matrix
	MATH_CopyMatrix(&st_Matrix, &_pst_Cam->st_Matrix);

	//set camera matrix
	MATH_Set33Matrix(&st_Matrix, &I, &J, &K);

	CAM_SetCameraMatrix(_pst_Cam, &st_Matrix);

	/*-----------------------------------------------*/
	//jade orientation
	//switch( _dw_Face )
	//{
	//case D3DCUBEMAP_FACE_POSITIVE_X:
	//	MATH_InitVector(&I, Cf_Zero, Cf_One, Cf_Zero);
	//	MATH_InitVector(&J, -Cf_One, Cf_Zero, Cf_Zero);
	//	MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
	//	break;
	//case D3DCUBEMAP_FACE_NEGATIVE_X:
	//	MATH_InitVector(&I, Cf_Zero, -Cf_One, Cf_Zero);
	//	MATH_InitVector(&J, Cf_One, Cf_Zero, Cf_Zero);
	//	MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
	//	break; 
	//case D3DCUBEMAP_FACE_POSITIVE_Y:
	//	MATH_InitVector(&I, -Cf_One, Cf_Zero, Cf_Zero);
	//	MATH_InitVector(&J, Cf_Zero, -Cf_One, Cf_Zero);
	//	MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
	//	break;
	//case D3DCUBEMAP_FACE_NEGATIVE_Y:
	//	MATH_InitVector(&I, Cf_One, Cf_Zero, Cf_Zero);
	//	MATH_InitVector(&J, Cf_Zero, Cf_One, Cf_Zero);
	//	MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
	//	break;
	//case D3DCUBEMAP_FACE_POSITIVE_Z:
	//	MATH_InitVector(&I, Cf_Zero, Cf_One, Cf_Zero);
	//	MATH_InitVector(&J, Cf_Zero, Cf_Zero, -Cf_One);
	//	MATH_InitVector(&K, -Cf_One, Cf_Zero, Cf_Zero);
	//	break;
	//case D3DCUBEMAP_FACE_NEGATIVE_Z:
	//	MATH_InitVector(&I, Cf_Zero, Cf_One, Cf_Zero);
	//	MATH_InitVector(&J, Cf_Zero, Cf_Zero, Cf_One);
	//	MATH_InitVector(&K, Cf_One, Cf_Zero, Cf_Zero);
	//	break;
	//default:
	//	break;
	//}
	/*-----------------------------------------------*/

	/*-----------------------------------------------*/
	//D3D orientation
	//switch( _dw_Face )
	//{
	//case D3DCUBEMAP_FACE_POSITIVE_X:
	//	MATH_InitVector(&I, Cf_Zero, Cf_One, Cf_Zero);
	//	MATH_InitVector(&J, -Cf_One, Cf_Zero, Cf_Zero);
	//	MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
	//	break;
	//case D3DCUBEMAP_FACE_NEGATIVE_X:
	//	MATH_InitVector(&I, Cf_Zero, -Cf_One, Cf_Zero);
	//	MATH_InitVector(&J, Cf_One, Cf_Zero, Cf_Zero);
	//	MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
	//	break; 
	//case D3DCUBEMAP_FACE_POSITIVE_Y:
	//	MATH_InitVector(&I, Cf_Zero, Cf_One, Cf_Zero);
	//	MATH_InitVector(&J, Cf_Zero, Cf_Zero, -Cf_One);
	//	MATH_InitVector(&K, -Cf_One, Cf_Zero, Cf_Zero);
	//	break;
	//case D3DCUBEMAP_FACE_NEGATIVE_Y:
	//	MATH_InitVector(&I, Cf_Zero, Cf_One, Cf_Zero);
	//	MATH_InitVector(&J, Cf_Zero, Cf_Zero, Cf_One);
	//	MATH_InitVector(&K, Cf_One, Cf_Zero, Cf_Zero);
	//	break;
	//case D3DCUBEMAP_FACE_POSITIVE_Z:
	//	MATH_InitVector(&I, -Cf_One, Cf_Zero, Cf_Zero);
	//	MATH_InitVector(&J, Cf_Zero, -Cf_One, Cf_Zero);
	//	MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
	//	break;
	//case D3DCUBEMAP_FACE_NEGATIVE_Z:
	//	MATH_InitVector(&I, Cf_One, Cf_Zero, Cf_Zero);
	//	MATH_InitVector(&J, Cf_Zero, Cf_One, Cf_Zero);
	//	MATH_InitVector(&K, Cf_Zero, Cf_Zero, Cf_One);
	//	break;
	//default:
	//	break;
	//}
	/*-----------------------------------------------*/
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_CubeMapGenDialog::b_CreateCubeMap(CString _strFullPath, CString _strName, BOOL _b_Export)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_XenonCubeMapFileTex	st_XeCBMProperties;
	BIG_INDEX						ul_Index;
	CString							strMsg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	//initialize CubeMap properties
	st_XeCBMProperties.ul_OutputFormat = (TEX_XenonTextureFormat) (((CComboBox*) GetDlgItem(IDC_TEX_COMBO_FORMAT))->GetCurSel() + 1);
	st_XeCBMProperties.ul_NbLevels = ((CButton *)GetDlgItem(IDC_CHECK_TEX_GENMIPMAP))->GetCheck() ? 
									 ((CComboBox*) GetDlgItem(IDC_TEX_COMBO_MIPMAP))->GetCurSel() : 1;
	st_XeCBMProperties.ul_Flags |= TEX_Xe_IsCubeMap;
	st_XeCBMProperties.ul_OutputSize = mi_Size;
	
	for(int nFace = 0; nFace < CUBEMAP_FACE_NUM; ++nFace)
	{
		//get all keys
		ul_Index = BIG_ul_SearchFileExt(CUBEMAP_SNAPSHOT_PATH, (char *)(LPCTSTR) mstrFace[nFace]);
		if(ul_Index == BIG_C_InvalidIndex) return FALSE;
		st_XeCBMProperties.ul_OriginalTextures[nFace] = BIG_FileKey(ul_Index);	
	}

	// Convert 
	if(TEX_XeGenerateCBM(&st_XeCBMProperties,(char *)(LPCTSTR) _strFullPath, (char *)(LPCTSTR) _strName, _b_Export))
	{
		strMsg = _strName + " was succesfully created in " + _strFullPath;
		LINK_PrintStatusMsg((char *)(LPCTSTR) strMsg);
		//TEX_ul_File_SaveTexWithIndex(st_XeCBMProperties.ul_NativeTexture, &st_Tex[nFace]);
	}
	else
	{
		strMsg = "An error occur during the creation of " + _strName;
		ERR_X_Warning(0,(char *)(LPCTSTR) strMsg, NULL);
	}

	return TRUE;
}

#endif /* ACTIVE_EDITORS */
