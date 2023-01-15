/*$T MATframe.cpp GC! 1.100 08/24/01 16:21:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#define ACTION_GLOBAL

#include "BASe/MEMory/MEM.h"
#include "EDImainframe.h"
#include "EDIbaseframe.h"
#include "GEOmetric/GEOload.h"
#include "MATframe.h"
#include "MATframe_Act.h"
#include "../../../Libraries/ENGine/Sources/WORld/WOR.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "res/res.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIACOLOR_dlg.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "GraphicDK/Sources/TEXture/TEXfile.h"
#ifdef JADEFUSION
#include "GraphicDK/Sources/TEXture/TEXcubemap.h"
#endif
#include "EDIpaths.h"
#include "EDImsg.h"
#ifdef JADEFUSION
#include "GraphicDK/Sources/GDInterface/GDInterface.h"
#endif

#ifndef JADEFUSION
extern "C"
{
ULONG	EDI_OUT_gl_ForceSetMode = 0;
};
#else
ULONG	EDI_OUT_gl_ForceSetMode = 0;
#endif

float	MAtClippoardProp = 0.7f;
BOOL	b_AutoSetNormalMap = FALSE;
BOOL	b_ShowCLPBRDMT = TRUE;

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EMAT_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EMAT_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EMAT_cl_Frame::EMAT_cl_Frame(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	static BOOL b = FALSE;
	/*~~~~~~~~~~~~~~~~~~*/

	st_CDO.pst_ACTIVE_GRO = st_CDO.pst_GRO = NULL;
	pst_Multi_Sample = NULL;
	ul_DoPos = 0;
	ul_ReDoNumber = 0;
	ul_IsSaved = 1;
	ShowMLTSBMT = 1;
	memset(&st_CDO, 0, sizeof(st_CDO));
	memset(&ast_UndoDrome, 0, sizeof(st_CDO) * MAT_C_MAX_UNDO);
	CurrentTextureDir.Empty();
	if(!b)
	{
		DllMain(AfxGetInstanceHandle(), DLL_PROCESS_ATTACH, 0);
		b = TRUE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EMAT_cl_Frame::~EMAT_cl_Frame(void)
{
	if(pst_Multi_Sample) pst_Multi_Sample->st_Id.i->pfn_Destroy(pst_Multi_Sample);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EMAT_cl_Frame::PreTranslateMessage(MSG *pmsg)
{
	if(pmsg->message == WM_RBUTTONDOWN)
	{
		if(pmsg->hwnd == ::GetDlgItem(stw_MultiSM, IDC_LIST_MSM))
		{
			List_OnRButtonDown();
			return TRUE;
		}
		else if(pmsg->hwnd == ::GetDlgItem(stw_MultiSM, IDC_MATNAME))
		{
			MatName_OnRButtonDown();
			return TRUE;
		}
	}

	return EDI_cl_BaseFrame::PreTranslateMessage(pmsg);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::MatName_OnRButtonDown( void )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMEN_cl_SubMenu o_Menu(FALSE);
	CPoint			pt, local;
	int				i_Res;
	EBRO_cl_Frame   *po_Browser;
	ULONG           ul_Key, ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!st_CDO.pst_GRO) return;

	GetCursorPos(&pt);
	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Show in browser", -1);
	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);

	switch( i_Res )
	{
	case 1:
		ul_Key = LOA_ul_SearchKeyWithAddress( (ULONG) st_CDO.pst_GRO );
        if(ul_Key != BIG_C_InvalidKey)
        {
			ul_Index = BIG_ul_SearchKeyToFat( ul_Key );
            if  (ul_Index != BIG_C_InvalidIndex)
            {
				po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
                po_Browser->i_OnMessage( EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_Index ), ul_Index );
			}
		}
        break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::List_OnRButtonDown(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	HWND			h_LB;
	EMEN_cl_SubMenu o_Menu(FALSE);
	CPoint			pt, local;
	int				i, i_Res, i_Sub;
	MAT_tdst_Multi	*pst_MM;
	GRO_tdst_Struct *pst_Mat;
	EBRO_cl_Frame   *po_Browser;
	ULONG           ul_Key, ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!st_CDO.pst_GRO) return;

	h_LB = ::GetDlgItem(stw_MultiSM, IDC_LIST_MSM);
	pst_MM = (MAT_tdst_Multi *) st_CDO.pst_GRO;

	if(pst_MM->st_Id.i->ul_Type != GRO_MaterialMulti) return;

	GetCursorPos(&pt);

	local = pt;
	::ScreenToClient(h_LB, &local);
	i_Sub = ::SendDlgItemMessage(stw_MultiSM, IDC_LIST_MSM, LB_ITEMFROMPOINT, 0, MAKELPARAM(local.x, local.y));
	if((i_Sub >= 0) && (i_Sub < pst_MM->l_NumberOfSubMaterials))
	{
		::SendDlgItemMessage(stw_MultiSM, IDC_LIST_MSM, LB_SETCURSEL, i_Sub, 0);
		OnSetMaterial(st_CDO.pst_GRO, i_Sub);
	}
	else
		i_Sub = -1;

	M_MF()->InitPopupMenuAction(NULL, &o_Menu);

	if ( (i_Sub != -1) && pst_MM->dpst_SubMaterial[i_Sub] )
	{
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 6, TRUE, "Show in browser", -1);
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, NULL, -1);
	}
	
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Insert Single Material", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 5, TRUE, "Create & Insert Single Material", -1);
	
	if(i_Sub != -1)
	{
		M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Change Single Material", -1);
		if(pst_MM->l_NumberOfSubMaterials > 1)
		{
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 0, TRUE, NULL, -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Delete", -1);
			M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Delete And Decal", -1);
		}
	}

	i_Res = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);

	switch(i_Res)
	{
	case 5:
		pst_Mat = CreateSingleMaterial();
		goto com;
	case 1:
		pst_Mat = pst_OpenSingleMat();
com:
		if(pst_Mat)
		{
			if(!pst_MM->dpst_SubMaterial)
				pst_MM->dpst_SubMaterial = (MAT_tdst_Material **) MEM_p_Alloc(4);
			else
			{
				pst_MM->dpst_SubMaterial = (MAT_tdst_Material **) MEM_p_Realloc
					(
						pst_MM->dpst_SubMaterial,
						(pst_MM->l_NumberOfSubMaterials + 1) * 4
					);
			}

			if(i_Sub != -1)
			{
				L_memmove
				(
					pst_MM->dpst_SubMaterial + i_Sub + 1,
					pst_MM->dpst_SubMaterial + i_Sub,
					(pst_MM->l_NumberOfSubMaterials - i_Sub) * 4
				);
			}
			else
				i_Sub = pst_MM->l_NumberOfSubMaterials;

			pst_MM->l_NumberOfSubMaterials++;
			pst_MM->dpst_SubMaterial[i_Sub] = (MAT_tdst_Material *) pst_Mat;
			pst_Mat->i->pfn_AddRef(pst_Mat, 1);
			st_CDO.pst_GRO = NULL;
			OnSetMaterial((MAT_tdst_Material *) pst_MM, i_Sub);
		}
		break;

	case 2:
		pst_Mat = pst_OpenSingleMat();
		if(pst_Mat)
		{
			if(pst_MM->dpst_SubMaterial[i_Sub])
				pst_MM->dpst_SubMaterial[i_Sub]->st_Id.i->pfn_AddRef(pst_MM->dpst_SubMaterial[i_Sub], -1);
			pst_MM->dpst_SubMaterial[i_Sub] = (MAT_tdst_Material *) pst_Mat;
			pst_Mat->i->pfn_AddRef(pst_Mat, 1);
			st_CDO.pst_GRO = NULL;
			OnSetMaterial((MAT_tdst_Material *) pst_MM, i_Sub);
		}
		break;

	/* delete sub mat under mouse and leave an empty slot */
	case 3:
		if(pst_MM->dpst_SubMaterial[i_Sub])
			pst_MM->dpst_SubMaterial[i_Sub]->st_Id.i->pfn_AddRef(pst_MM->dpst_SubMaterial[i_Sub], -1);
		pst_MM->dpst_SubMaterial[i_Sub] = NULL;
		st_CDO.pst_GRO = NULL;
		OnSetMaterial((MAT_tdst_Material *) pst_MM, i_Sub);
		break;

	/* delete sub mat and decal other mat */
	case 4:
		if(pst_MM->dpst_SubMaterial[i_Sub])
			pst_MM->dpst_SubMaterial[i_Sub]->st_Id.i->pfn_AddRef(pst_MM->dpst_SubMaterial[i_Sub], -1);
		for(i = i_Sub + 1; i < pst_MM->l_NumberOfSubMaterials; i++)
			pst_MM->dpst_SubMaterial[i - 1] = pst_MM->dpst_SubMaterial[i];
		pst_MM->l_NumberOfSubMaterials--;
		st_CDO.pst_GRO = NULL;
		if(i_Sub == pst_MM->l_NumberOfSubMaterials) i_Sub--;
		OnSetMaterial((MAT_tdst_Material *) pst_MM, i_Sub);
		break;

	/* show sub mat in browser */
	case 6:
		ul_Key = LOA_ul_SearchKeyWithAddress( (ULONG) pst_MM->dpst_SubMaterial[i_Sub] );
        if(ul_Key != BIG_C_InvalidKey)
        {
			ul_Index = BIG_ul_SearchKeyToFat( ul_Key );
            if  (ul_Index != BIG_C_InvalidIndex)
            {
				po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
                po_Browser->i_OnMessage( EDI_MESSAGE_SELFILE, BIG_ParentFile( ul_Index ), ul_Index );
			}
		}
        break;

	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GRO_tdst_Struct *EMAT_cl_Frame::pst_OpenSingleMat(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File("Choose grm file", 3, TRUE, TRUE, NULL);
	CString				o_Temp;
	char				*psz_Ext;
	ULONG				ul_Index;
	CRect				o_Rect;
	GRO_tdst_Struct		*pst_Mat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() != IDOK) return NULL;

	o_File.GetItem(o_File.mo_File, 1, o_Temp);
	if(BIG_b_CheckName((char *) (LPCTSTR) o_Temp) == FALSE) return NULL;
	psz_Ext = strrchr((char *) (LPCTSTR) o_Temp, '.');
	if(!psz_Ext) return NULL;
	if(L_stricmp(psz_Ext + 1, "grm")) return NULL;

	ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCTSTR) o_Temp);
	if(ul_Index == BIG_C_InvalidIndex) return NULL;

	pst_Mat = GEO_pst_Object_Load(ul_Index, NULL);
	if((pst_Mat->i->ul_Type == GRO_MaterialSingle) || (pst_Mat->i->ul_Type == GRO_MaterialMultiTexture))
		return pst_Mat;

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EMAT_cl_Frame::OnEraseBkgnd(CDC *pdc)
{
	/*~~~~~~~~~~~~~~~*/
	CRect		o_Rect;
	COLORREF	col;
	/*~~~~~~~~~~~~~~~*/

	GetClientRect(&o_Rect);
	col = GetSysColor(COLOR_BTNFACE);
	col = M_MF()->u4_Interpol2PackedColor(0, col, 0.85f);
	pdc->FillSolidRect(&o_Rect, col);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int __stdcall RefreshCLBK(void *UserParam, BOOL NO_Undo)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	EMAT_cl_Frame	*ClMat;
	/*~~~~~~~~~~~~~~~~~~~*/

	ClMat = (EMAT_cl_Frame *) UserParam;
	ClMat->RefreshStruct(NO_Undo);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EMAT_cl_Frame::RefreshStruct(BOOL NO_Undo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File1("Choose a Texture", 3, TRUE, TRUE, (char *) (LPCSTR) CurrentTextureDir);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(NO_Undo > 1 && NO_Undo != 0xFF)	/* User wants to change texture */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MUTEX_TextureLine	*TLine;
		TEX_tdst_Data		*pst_Texture;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		TLine = (MUTEX_TextureLine *) NO_Undo;
		if(o_File1.DoModal() == IDOK)
		{
			/*~~~~~~~~~~~~~~~~~~*/
			char		*psz_Temp;
			CString		o_Temp;
			BIG_KEY		ul_Key;
			BIG_INDEX	ul_Index;
			/*~~~~~~~~~~~~~~~~~~*/

			o_File1.GetItem(o_File1.mo_File, 1, o_Temp);
			psz_Temp = (char *) (LPCSTR) o_Temp;
			if(BIG_b_CheckName(psz_Temp) == FALSE) return 0;
			CurrentTextureDir = o_File1.masz_FullPath;
			if(TEX_l_File_IsFormatSupported(psz_Temp, -1))
			{
#ifdef JADEFUSION
				BOOL bNewTexture = FALSE;
#endif
				ul_Index = BIG_ul_SearchFileExt(o_File1.masz_FullPath, psz_Temp);
				if(ul_Index == BIG_C_InvalidIndex) return 0;
				ul_Key = BIG_FileKey(ul_Index);
				pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, ul_Key);
				if(pst_Texture)
					TLine->TEXTURE_ID = pst_Texture->w_Index;
				else
				{
					TLine->TEXTURE_ID = TEX_w_List_AddTexture(&TEX_gst_GlobalList, ul_Key, 1);
#ifdef JADEFUSION
					bNewTexture       = TRUE;
#endif
				}
				L_strcpy(TLine->TextureName, psz_Temp);
#ifdef JADEFUSION
				MUTEX_To_GRM(&st_CDO);
#else
				MUTEX_To_GRM();
#endif
				OnDo();
#if defined(_XENON_RENDER)
                if (GDI_b_IsXenonGraphics())
                {
                    if (bNewTexture)
                    {
                        GDI_Xe_ForceLoadLastTexture();
                    }
                }
                else
                {
                    EDI_OUT_gl_ForceSetMode = 1;
                }
#else
                EDI_OUT_gl_ForceSetMode = 1;
#endif				LINK_Refresh();
				EDI_OUT_gl_ForceSetMode = 0;
				RefreshMenu();
			}
		}

		return 0;
	}
#ifdef JADEFUSION
	MUTEX_To_GRM(&st_CDO);
#else
	MUTEX_To_GRM();
#endif

	if(!NO_Undo)
	{
		OnDo();
	}

	LINK_Refresh();

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnSetMaterial(MAT_tdst_Material *pst_GRM, ULONG SubMaterialNum)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Material		*pst_GRMSub;
	MAT_tdst_Multi			*Mtt;
	MAT_tdst_MultiTexture	*Stt;
	LONG					ulMtlC;
	char					ac_String[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GRMSub = pst_GRM;
	M_MF()->LockDisplay(this);

	if((pst_GRM->st_Id.i->ul_Type == GRO_MaterialMulti))
	{
		ulCurrentMSMWidth = ulMSMWidth;
		Mtt = (MAT_tdst_Multi *) pst_GRM;
		if(Mtt->l_NumberOfSubMaterials)
		{
			//pst_GRMSub = Mtt->dpst_SubMaterial[ SubMaterialNum % Mtt->l_NumberOfSubMaterials];
			pst_GRMSub = Mtt->dpst_SubMaterial[ lMin( SubMaterialNum, Mtt->l_NumberOfSubMaterials - 1) ];
			if(pst_GRM != st_CDO.pst_GRO)
			{
				::SendDlgItemMessage(stw_MultiSM, IDC_LIST_MSM, LB_RESETCONTENT, 0, 0);
				for(ulMtlC = 0; ulMtlC < Mtt->l_NumberOfSubMaterials; ulMtlC++)
				{
					if(Mtt->dpst_SubMaterial[ulMtlC] == NULL)
					{
						::SendDlgItemMessage(stw_MultiSM, IDC_LIST_MSM, LB_ADDSTRING, 0, (LONG) "INVALID REFERENCE");
					}
					else
					{
						sprintf
						(
							ac_String,
							"%d %s",
							ulMtlC,
							GRO_sz_Struct_GetName(&Mtt->dpst_SubMaterial[ulMtlC]->st_Id)
						);
						::SendDlgItemMessage(stw_MultiSM, IDC_LIST_MSM, LB_ADDSTRING, 0, (LONG) ac_String);
					}
				}
			}
		}
		else
		{
			::SendDlgItemMessage(stw_MultiSM, IDC_LIST_MSM, LB_RESETCONTENT, 0, 0);
		}

		MAT_Validate_Multi(Mtt);
		if(Mtt->l_NumberOfSubMaterials)
		{
			::SendDlgItemMessage
			(
				stw_MultiSM,
				IDC_LIST_MSM,
				LB_SETCURSEL,
				(LONG) (lMin( SubMaterialNum, Mtt->l_NumberOfSubMaterials-1) ),
				0xC0DECACA
			);
		}

		sprintf(ac_String, "Multi Material\n%s", GRO_sz_Struct_GetName(&Mtt->st_Id));
		::SetDlgItemText(stw_MultiSM, IDC_MATNAME, ac_String);
		::EnableWindow(::GetDlgItem(stw_MultiSM, IDC_LIST_MSM), TRUE);
	}
	else
	{
		Stt = (MAT_tdst_MultiTexture *) pst_GRM;
		sprintf(ac_String, "Single Material\n%s", GRO_sz_Struct_GetName(&Stt->st_Id));
		::SetDlgItemText(stw_MultiSM, IDC_MATNAME, ac_String);
		ulCurrentMSMWidth = ulMSMWidth;
		::SendDlgItemMessage(stw_MultiSM, IDC_LIST_MSM, LB_RESETCONTENT, 0, 0);
		::EnableWindow(::GetDlgItem(stw_MultiSM, IDC_LIST_MSM), FALSE);
	}

	if((pst_GRMSub != st_CDO.pst_ACTIVE_GRO))
	{
		OnSave();
		st_CDO.pst_GRO = pst_GRM;
		st_CDO.pst_ACTIVE_GRO = pst_GRMSub;
#ifdef JADEFUSION
		GRM_To_MUTEX(&st_CDO, pst_Multi_Sample);
		OnDo();
#else
		GRM_To_MUTEX();
#endif
		MUTEX_SetMat(m_hWnd, &st_CDO.Dest);
		OnSize(0, CurrentCX, CurrentCY);
		RefreshMenu();
	}

	st_CDO.pst_GRO = pst_GRM;
	st_CDO.pst_ACTIVE_GRO = pst_GRMSub;
	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnDo(void)
{
	ul_ReDoNumber = 0;
	if(st_CDO.pst_ACTIVE_GRO != ast_UndoDrome[0].pst_ACTIVE_GRO)
	{
		ast_UndoDrome[0] = st_CDO;
		ul_DoPos = 1;
		return;
	}

	if((ul_DoPos != 0) && (memcmp(&ast_UndoDrome[ul_DoPos - 1], &st_CDO, sizeof(st_CDO)) == 0)) return;
	ast_UndoDrome[ul_DoPos++] = st_CDO;
	if(ul_DoPos == MAT_C_MAX_UNDO)
	{
		memmove(&ast_UndoDrome[0], &ast_UndoDrome[1], sizeof(st_CDO) * (MAT_C_MAX_UNDO - 1));
		ul_DoPos--;
	}

	ul_IsSaved = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnUndo(void)
{
	if(ul_DoPos <= 1) return;
	ul_DoPos--;
	st_CDO = ast_UndoDrome[ul_DoPos - 1];
	MUTEX_SetMat(m_hWnd, &st_CDO.Dest);
#ifdef JADEFUSION
	MUTEX_To_GRM(&st_CDO);
#else
	MUTEX_To_GRM();
#endif
	LINK_Refresh();
	ul_ReDoNumber++;
	ul_IsSaved = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnCopyAll(void)
{
	st_ClipBoardMat = st_CDO.Dest;
	MUTEX_SetMat(stw_ClipBoard, &st_ClipBoardMat);
	LINK_Refresh();
	ul_ReDoNumber++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnPasteAll(void)
{
	if(st_ClipBoardMat.MaterialName && *st_ClipBoardMat.MaterialName)
	{
		st_CDO.Dest = st_ClipBoardMat;
		MUTEX_SetMat(m_hWnd, &st_CDO.Dest);
#ifdef JADEFUSION
		MUTEX_To_GRM(&st_CDO);
#else
		MUTEX_To_GRM();
#endif
		LINK_Refresh();
		OnDo();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnRedo(void)
{
	if(!ul_ReDoNumber) return;
	st_CDO = ast_UndoDrome[ul_DoPos++];
	MUTEX_SetMat(m_hWnd, &st_CDO.Dest);
#ifdef JADEFUSION
	MUTEX_To_GRM(&st_CDO);
#else
	MUTEX_To_GRM();
#endif

	LINK_Refresh();
	ul_ReDoNumber--;
	ul_IsSaved = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnSave(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		sz_Path[BIG_C_MaxLenPath];
	BIG_KEY		ul_FileKey;
	BIG_INDEX	ul_FileIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ul_IsSaved) return;
	ul_IsSaved = 1;
	if(st_CDO.pst_ACTIVE_GRO)
	{
		sprintf(sz_Path, "Save material %s ?", st_CDO.Dest.MaterialName);
		if
		(
			(mst_Init.bAutoSave)
		||	(
				(ul_DoPos > 1)
			&&	M_MF()->MessageBox(sz_Path, TEXT("Material Editor"), MB_YESNO | MB_ICONQUESTION) == IDYES
			)
		)
		{
			/* Search file key associated with world pointer */
			ul_FileKey = LOA_ul_SearchKeyWithAddress((ULONG) st_CDO.pst_ACTIVE_GRO);
			if(ul_FileKey == BIG_C_InvalidKey) return;

			/* Search index associated with key */
			ul_FileIndex = BIG_ul_SearchKeyToFat(ul_FileKey);
			if(ul_FileIndex == BIG_C_InvalidIndex) return;

			BIG_ComputeFullName(BIG_ParentFile(ul_FileIndex), sz_Path);
#ifdef JADEFUSION
            // SC: Don't save multi or multi texture materials when there are no textures loaded
            if ((st_CDO.pst_ACTIVE_GRO->st_Id.i->ul_Type == GRO_MaterialSingle) || 
                (TEX_gst_GlobalList.l_NumberMaxOfTextures > 0))
            {
                SAV_Begin(sz_Path, BIG_NameFile(ul_FileIndex));
                st_CDO.pst_ACTIVE_GRO->st_Id.i->pfnl_SaveInBuffer(st_CDO.pst_ACTIVE_GRO, &TEX_gst_GlobalList);
                SAV_ul_End();
            }
            else
            {
                ::MessageBox(NULL, "The material will not be saved since the texture list is empty", 
                             "Material Save", MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
            }
#else
			SAV_Begin(sz_Path, BIG_NameFile(ul_FileIndex));
			st_CDO.pst_ACTIVE_GRO->st_Id.i->pfnl_SaveInBuffer(st_CDO.pst_ACTIVE_GRO, &TEX_gst_GlobalList);
			SAV_ul_End();
#endif
			M_MF()->FatHasChanged();
			M_MF()->DataHasChanged();
			LINK_PrintStatusMsg("Saving material OK");
		}
		else
		{
			st_CDO = ast_UndoDrome[0];
#ifdef JADEFUSION
			MUTEX_To_GRM(&st_CDO);
#else
			MUTEX_To_GRM();
#endif
			LINK_Refresh();
			ul_IsSaved = 1;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EMAT_cl_Frame::b_AcceptToCloseProject(void)
{
	OnSave();
	return TRUE;
}

/*
 =======================================================================================================================
    Callback multi submat (left pane)
 =======================================================================================================================
 */
BOOL CALLBACK MAT_MultiSubMatCLBK(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EMAT_cl_Frame		*pc_MF;
	ULONG				CurrentSel;
	HWND				hwnd;
	int					i_res;
	LPMINMAXINFO		lpmmi;
	CMenu				o_Menu;
	CPoint				pt;
	MAT_tdst_Multi		*pst_MM;
	MAT_tdst_Material	*pst_1;
	int					count;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_MF = (EMAT_cl_Frame *) GetWindowLong(hwndDlg, GWL_USERDATA);
	switch(uMsg)
	{
	case WM_INITDIALOG:
		pc_MF = (EMAT_cl_Frame *) lParam;
		SetWindowLong(hwndDlg, GWL_USERDATA, lParam);
		return TRUE;
		break;

	case WM_NCHITTEST:
		i_res = DefWindowProc(hwndDlg, uMsg, wParam, lParam);
		if(i_res == HTRIGHT) return 0;
		return 1;

	case WM_GETMINMAXINFO:
		lpmmi = (LPMINMAXINFO) lParam;
		lpmmi->ptMinTrackSize.x = 50;
		break;

	case WM_SIZE:
		M_MF()->LockDisplay(pc_MF);
		hwnd = GetDlgItem(hwndDlg, IDC_LIST_MSM);
		MoveWindow(hwnd, 0, 74, LOWORD(lParam), HIWORD(lParam) - 78, TRUE);
		pc_MF->ulMSMWidth = pc_MF->ulCurrentMSMWidth = LOWORD(lParam) + 6;
		MUTEX_SetXYoffset(pc_MF->m_hWnd, pc_MF->ulCurrentMSMWidth, 0);

		hwnd = GetDlgItem(hwndDlg, IDC_MATNAME);
		MoveWindow(hwnd, 0, 21, LOWORD(lParam), 29, TRUE);

		hwnd = GetDlgItem(hwndDlg, IDC_STATIC);
		MoveWindow(hwnd, 2, 7, LOWORD(lParam) - 21, 5, TRUE);

		hwnd = GetDlgItem(hwndDlg, IDC_BUTTONCLOSEMLTMT);
		MoveWindow(hwnd, LOWORD(lParam) - 16, 2, 14, 15, TRUE);

		M_MF()->UnlockDisplay(pc_MF);
		break;

	case WM_MOUSEMOVE:
		break;

	case WM_RBUTTONDOWN:
		hwnd = 0;
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_LIST_MSM:
			switch(HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				CurrentSel = ::SendDlgItemMessage(hwndDlg, IDC_LIST_MSM, LB_GETCURSEL, 0, 0);
				if(lParam != 0xC0DECACA)
					pc_MF->OnSetMaterial(pc_MF->st_CDO.pst_GRO, CurrentSel);
				else
					lParam = lParam;
				break;
			}
			break;

		case IDUP:
			if(!pc_MF->st_CDO.pst_GRO) break;
			pst_MM = (MAT_tdst_Multi *) pc_MF->st_CDO.pst_GRO;
			if(pst_MM->st_Id.i->ul_Type != GRO_MaterialMulti) break;
			CurrentSel = ::SendDlgItemMessage(hwndDlg, IDC_LIST_MSM, LB_GETCURSEL, 0, 0);
			if(CurrentSel == 0 || CurrentSel == -1) break;
			pst_1 = pst_MM->dpst_SubMaterial[CurrentSel - 1];
			pst_MM->dpst_SubMaterial[CurrentSel - 1] = pst_MM->dpst_SubMaterial[CurrentSel];
			pst_MM->dpst_SubMaterial[CurrentSel] = pst_1;
			pc_MF->st_CDO.pst_GRO = NULL;
			pc_MF->OnSetMaterial((MAT_tdst_Material *) pst_MM, CurrentSel - 1);
			LINK_Refresh();
			break;
		case IDDOWN:
			if(!pc_MF->st_CDO.pst_GRO) break;
			pst_MM = (MAT_tdst_Multi *) pc_MF->st_CDO.pst_GRO;
			if(pst_MM->st_Id.i->ul_Type != GRO_MaterialMulti) break;
			CurrentSel = ::SendDlgItemMessage(hwndDlg, IDC_LIST_MSM, LB_GETCURSEL, 0, 0);
			count = ::SendDlgItemMessage(hwndDlg, IDC_LIST_MSM, LB_GETCOUNT, 0, 0);
			if(CurrentSel == (ULONG) count - 1 || CurrentSel == -1) break;
			pst_1 = pst_MM->dpst_SubMaterial[CurrentSel + 1];
			pst_MM->dpst_SubMaterial[CurrentSel + 1] = pst_MM->dpst_SubMaterial[CurrentSel];
			pst_MM->dpst_SubMaterial[CurrentSel] = pst_1;
			pc_MF->st_CDO.pst_GRO = NULL;
			pc_MF->OnSetMaterial((MAT_tdst_Material *) pst_MM, CurrentSel + 1);
			LINK_Refresh();
			break;
		}

		if(HIWORD(wParam) == BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_BUTTONCLOSEMLTMT:
				pc_MF->SetFocus();
				pc_MF->ShowMLTSBMT = 0;
				b_ShowCLPBRDMT = FALSE;
				M_MF()->LockDisplay(pc_MF);
				pc_MF->OnSize(0, pc_MF->CurrentCX, pc_MF->CurrentCY);
				M_MF()->UnlockDisplay(pc_MF);
				break;
			}
		}
	}

	return FALSE;
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG FAR PASCAL WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EMAT_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	/*~~~~~~~~~~~~~~~~~~*/
	RECT		st_MSMr;
	WNDCLASS	wndClass =
	{
		(CS_HREDRAW | CS_VREDRAW),
		WndProc2,
		0,
		0,
		AfxGetInstanceHandle(),
		NULL,
		NULL,
		NULL,
		NULL,
		"EEERRRREEE"
	};
	/*~~~~~~~~~~~~~~~~~~*/

	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1) return -1;

	mst_Init.bAutoSave = FALSE;
	RegisterClass(&wndClass);
	stw_ClipBoard = ::CreateWindowEx
		(
			0,
			"EEERRRREEE",
			"",
			WS_CHILDWINDOW,
			0,
			0,
			1000,
			1000,
			m_hWnd,
			NULL,
			AfxGetInstanceHandle(),
			0L
		);
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		RECT	AdaptZone, AdaptZone2;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		::GetClientRect(m_hWnd, &AdaptZone);
		::GetClientRect(m_hWnd, &AdaptZone2);
		AdaptZone.top = AdaptZone.top + (int) ((float) (AdaptZone.bottom - AdaptZone.top) * MAtClippoardProp);
		::MoveWindow
		(
			stw_ClipBoard,
			0,
			AdaptZone.top - AdaptZone2.top,
			AdaptZone.right - AdaptZone.left,
			AdaptZone.bottom - AdaptZone.top,
			TRUE
		);
	}

	::ShowWindow(stw_ClipBoard, SW_SHOW);
	::UpdateWindow(stw_ClipBoard);
	memset(&st_ClipBoardMat, 0, sizeof(st_ClipBoardMat));
	MUTEX_Create(stw_ClipBoard, 1);
	MUTEX_SetMat(stw_ClipBoard, &st_ClipBoardMat);
	MUTEX_SetXYoffset(stw_ClipBoard, 0, 0);
	stw_MultiSM = CreateDialogParam
		(
			AfxGetInstanceHandle(),
			MAKEINTRESOURCE(DIALOG_MULTISUBMAT),
			m_hWnd,
			(int(__stdcall *) (struct HWND__ *, unsigned int, unsigned int, LONG)) & MAT_MultiSubMatCLBK,
			(LPARAM) this
		);
	::GetWindowRect(stw_MultiSM, &st_MSMr);
	ulMSMWidth = st_MSMr.right - st_MSMr.left;
	ulCurrentMSMWidth = ulMSMWidth;

	MUTEX_Create(m_hWnd, 0);
	MUTEX_SetRefrechCLBK(m_hWnd, RefreshCLBK, (void *) this);
	MUTEX_SetXYoffset(m_hWnd, ulCurrentMSMWidth, 0);
	::ShowWindow(stw_MultiSM, SW_HIDE);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EMAT_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	if(!IsWindow(m_hWnd)) return;

	/* Resize MUTEX */
	if((!ShowMLTSBMT) || (ulCurrentMSMWidth == 0))
	{
		::ShowWindow(stw_MultiSM, SW_HIDE);
		MUTEX_SetXYoffset(m_hWnd, 0, 0);
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		RECT	AdaptZone, AdaptZone2;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		::GetClientRect(m_hWnd, &AdaptZone);
		::GetClientRect(m_hWnd, &AdaptZone2);
		if(b_ShowCLPBRDMT)
		AdaptZone.bottom = AdaptZone.top + (int) ((float) (AdaptZone.bottom - AdaptZone.top) * MAtClippoardProp);
		::MoveWindow(stw_MultiSM, 0, 0, ulCurrentMSMWidth, AdaptZone.bottom - AdaptZone.top, TRUE);
		::ShowWindow(stw_MultiSM, SW_SHOW);
		MUTEX_SetXYoffset(m_hWnd, ulCurrentMSMWidth, 0);
	}

	if(b_ShowCLPBRDMT)
		::ShowWindow(stw_ClipBoard, SW_SHOW);
	else
		::ShowWindow(stw_ClipBoard, SW_HIDE);

	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		RECT	AdaptZone, AdaptZone2;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		::GetClientRect(m_hWnd, &AdaptZone);
		::GetClientRect(m_hWnd, &AdaptZone2);
		AdaptZone.top = AdaptZone.top + (int) ((float) (AdaptZone.bottom - AdaptZone.top) * MAtClippoardProp);
		::MoveWindow
		(
			stw_ClipBoard,
			0,
			AdaptZone.top - AdaptZone2.top,
			AdaptZone.right - AdaptZone.left,
			AdaptZone.bottom - AdaptZone.top,
			TRUE
		);

			if(b_ShowCLPBRDMT)
			MUTEX_AdaptToRect(stw_ClipBoard);

	}

	/* Call parent function */
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);
}
#ifdef JADEFUSION
// ------------------------------------------------------------------------------------------------
// Name   : EMAT_cl_Frame::BrowseForTexture
// Params : _pul_TexId   : Texture Id [out]
//          _psz_TexName : Texture name [out]
//          _b_CubeMap   : Browse for a cube map
// RetVal : None
// Descr. : Browse for a texture
// ------------------------------------------------------------------------------------------------
void EMAT_cl_Frame::BrowseForTexture(LONG* _pul_TexId, CHAR* _psz_TexName, CHAR* _psz_RefTexName, bool _b_CubeMap, LONG _l_RefTex_id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEX_tdst_Data*	pst_Texture;
	CString			strXenonPath, strXenonFileName, strMsg;
	BOOL			b_Res;
	int				i_Index;
	BIG_INDEX		ul_DirXenonIndex, ul_XenonFileIndex, ul_RefTexIndex;
	BIG_KEY			ul_RefTexKey;
	char			sz_RefTexPath[BIG_C_MaxLenPath];
	char			sz_XenonTexPath[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//set to invalid if we don't find the index of the reference texture
	ul_RefTexIndex = BIG_C_InvalidIndex;
	ul_RefTexKey = BIG_C_InvalidIndex;
	ul_XenonFileIndex = BIG_C_InvalidKey;
	*sz_RefTexPath = NULL;

	//construct Xenon file name
	strXenonFileName = _psz_RefTexName;
	i_Index = strXenonFileName.Find(".tex");
	if(i_Index != -1)
		strXenonFileName.Insert(i_Index, MAT_C_XENON_NMAP_SUFFIXE);
	else
		strXenonFileName = "";

	//construct Xenon file path and index
	strcpy(sz_XenonTexPath, MAT_C_XENON_START_DIR);
	ul_DirXenonIndex = BIG_ul_SearchDir(sz_XenonTexPath);
	if(ul_DirXenonIndex == BIG_C_InvalidIndex)
		ul_DirXenonIndex = BIG_Root();

	//get reference texture key
	if(_l_RefTex_id != BIG_C_InvalidIndex)
		if(_l_RefTex_id < TEX_gst_GlobalList.l_NumberOfTextures)
			ul_RefTexKey = TEX_gst_GlobalList.dst_Texture[_l_RefTex_id].ul_Key;

	//get reference texture path
	if(ul_RefTexKey != BIG_C_InvalidIndex)
	{
		ul_RefTexIndex = BIG_ul_SearchKeyToFat(ul_RefTexKey);
		if(ul_RefTexIndex != BIG_C_InvalidIndex)		
			BIG_ComputeFullName(BIG_ParentFile(ul_RefTexIndex), sz_RefTexPath);
	}

	if(sz_RefTexPath != NULL )
	{
		strXenonPath = sz_RefTexPath;
		strXenonPath.Replace(MAT_C_PS2_START_DIR, MAT_C_XENON_START_DIR);
		if(BIG_ul_SearchDir((char *)(LPCSTR) strXenonPath) == BIG_C_InvalidIndex && b_AutoSetNormalMap)
		{
			strMsg = strXenonPath + " does not exist !";
			ERR_X_Warning(0, (char *)(LPCSTR) strMsg, NULL);
		}
		else
		{
			//search for xenon file 
			ul_XenonFileIndex = BIG_ul_SearchFileExt((char *)(LPCSTR) strXenonPath, (char *)(LPCSTR) strXenonFileName);
			if(ul_XenonFileIndex != BIG_C_InvalidIndex)
			{
				strcpy(sz_XenonTexPath, (char *)(LPCSTR) strXenonPath);
			}
			else
			{
				//don't show msg for manual search
				if(b_AutoSetNormalMap)
				{
					strMsg = strXenonFileName + " not found in " + strXenonPath;
					ERR_X_Warning(0, (char *)(LPCSTR) strMsg, NULL);
				}
			}
		}
	}

	if(ul_XenonFileIndex == BIG_C_InvalidIndex && b_AutoSetNormalMap)
	{
		strMsg = strXenonFileName + " was not found in " + strXenonPath;
		ERR_X_Warning(0, (char *)(LPCSTR) strMsg, NULL);
		return;
	//	//make a full recursive search
	//	ul_XenonFileIndex = BIG_ul_SearchFileInDirRec(ul_DirXenonIndex, (char *)(LPCSTR) strXenonFileName);
	//	if(ul_XenonFileIndex != BIG_C_InvalidIndex)
	//		BIG_ComputeFullName(BIG_ParentFile(ul_XenonFileIndex), sz_XenonTexPath);
	//	else
	//		strcpy(sz_XenonTexPath, "");		
	//
	}
	

	EDIA_cl_FileDialog o_FileDlg(_b_CubeMap ? "Choose a Cube Map" : "Choose a Texture", 1, TRUE, TRUE,
								 sz_XenonTexPath, _b_CubeMap ? "*.cbm" : "*.*");

    ERR_X_Assert(_pul_TexId != NULL);
    ERR_X_Assert(_psz_TexName != NULL);

	if(b_AutoSetNormalMap)
	{
		b_Res = IDOK;
	
		//validity check
		if(ul_XenonFileIndex != BIG_C_InvalidIndex)
		{
			strMsg.FormatMessage(_T("The corresponding Xenon file(%1%) was found in %2%."), strXenonFileName, sz_XenonTexPath );
			LINK_PrintStatusMsg((char*)(LPCSTR) strMsg);
			o_FileDlg.mo_File = strXenonFileName;
			strcpy(o_FileDlg.masz_FullPath, sz_XenonTexPath);//(char *)(LPCSTR) strXenonPath);
		}
		else
		{
			strMsg.FormatMessage(_T("The corresponding Xenon file(%1%) wasn't found!"), strXenonFileName );
			MessageBox((char*)(LPCSTR) strMsg, "Warning", MB_OK | MB_ICONSTOP);
			return;
		}
	}
	else
		b_Res = o_FileDlg.DoModal();

    if(b_Res == IDOK)
    {
        CHAR*     psz_Temp;
        CString   o_Temp;
        BIG_KEY   ul_Key;
        BIG_INDEX ul_Index;

        o_FileDlg.GetItem(o_FileDlg.mo_File, 1, o_Temp);
        psz_Temp = (CHAR*)(LPCSTR)o_Temp;

        if ((o_FileDlg.mo_File == "") || !BIG_b_CheckName(psz_Temp))
            return;

        CurrentTextureDir = o_FileDlg.masz_FullPath;

        if (_b_CubeMap)
        {
            ul_Index = BIG_ul_SearchFileExt(o_FileDlg.masz_FullPath, psz_Temp);

            if(ul_Index == BIG_C_InvalidIndex)
                return;

            ul_Key = BIG_FileKey(ul_Index);

            SHORT s_PrevIndex = (SHORT)*_pul_TexId;

            // Load the cube map
            *_pul_TexId = TEX_CubeMap_Add(ul_Key);
            FetchTextureName(_psz_TexName, *_pul_TexId, true);

            // Decrement the reference count
            if (s_PrevIndex >= 0)
            {
                TEX_CubeMap_Remove(s_PrevIndex);
            }

            MUTEX_To_GRM(&st_CDO);

            OnDo();

            EDI_OUT_gl_ForceSetMode = 1;
            LINK_Refresh();
            EDI_OUT_gl_ForceSetMode = 0;

            RefreshMenu();
        }
        else
        {
            if(TEX_l_File_IsFormatSupported(psz_Temp, -1))
            {
                BOOL bNewTexture = FALSE;
                ul_Index = BIG_ul_SearchFileExt(o_FileDlg.masz_FullPath, psz_Temp);

                if(ul_Index == BIG_C_InvalidIndex)
                    return;

                ul_Key = BIG_FileKey(ul_Index);

                pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, ul_Key);
                if (pst_Texture)
                {
                    *_pul_TexId = (LONG)pst_Texture->w_Index;
                }
                else
                {
                    *_pul_TexId = (LONG)TEX_w_List_AddTexture(&TEX_gst_GlobalList, ul_Key, 1);
                    bNewTexture = TRUE;
                }
                FetchTextureName(_psz_TexName, *_pul_TexId, false);

                MUTEX_To_GRM(&st_CDO);

                OnDo();

#if defined(_XENON_RENDER)
                if (GDI_b_IsXenonGraphics())
                {
                    if (bNewTexture)
                    {
                        GDI_Xe_ForceLoadLastTexture();
                    }
                }
                else
                {
                    EDI_OUT_gl_ForceSetMode = 1;
                }
#else
                EDI_OUT_gl_ForceSetMode = 1;
#endif
                LINK_Refresh();
                EDI_OUT_gl_ForceSetMode = 0;

                RefreshMenu();
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : EMAT_BrowseForTextureCLBK
// Params : _p_UserParam : Pointer to the dialog
// Params : _pul_TexId   : Texture Id [out]
//          _psz_TexName : Texture name [out]
//          _e_Type      : Browse type
// RetVal : None
// Descr. : Browse for texture callback
// ------------------------------------------------------------------------------------------------
void __stdcall EMAT_BrowseForTextureCLBK(void* _p_UserParam, LONG* _pul_TexId, CHAR* _psz_TexName, CHAR* _psz_RefTexName, EMAT_XeBrowseType _e_Type, LONG _l_RefTex_id)
{
    ERR_X_Assert(_p_UserParam != NULL);

    EMAT_cl_Frame* pst_Frame = (EMAT_cl_Frame*)_p_UserParam;

    switch (_e_Type)
    {
        case EMAT_BT_BrowseTexture:
            pst_Frame->BrowseForTexture(_pul_TexId, _psz_TexName, _psz_RefTexName, false, _l_RefTex_id);
            break;

        case EMAT_BT_BrowseCubeMap:
            pst_Frame->BrowseForTexture(_pul_TexId, _psz_TexName, _psz_RefTexName, true, _l_RefTex_id);
            break;

        case EMAT_BT_ClearCubeMap:
            if (*_pul_TexId >= 0)
            {
                TEX_CubeMap_Remove((SHORT)*_pul_TexId);
            }
            // Pass through to normal clear
        case EMAT_BT_Clear:
            *_pul_TexId = MAT_Xe_InvalidTextureId;
            pst_Frame->FetchTextureName(_psz_TexName, *_pul_TexId, false);
            break;
    }
}
#endif
#endif /* ACTIVE_EDITORS */
