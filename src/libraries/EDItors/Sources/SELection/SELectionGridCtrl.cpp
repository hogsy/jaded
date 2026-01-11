//------------------------------------------------------------------------------
// Filename   :SELectionGrid.cpp
/// \author    NBeaufils
/// \date      2005-04-29
/// \par       Description: Implementation of SELectionGrid
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#include "SELectionGridCtrl.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "GraphicDK/Sources/TEXture/TEXfile.h"
#include "BIGfiles\LOAding\LOAdefs.h"
#include "LINks\LINKtoed.h"
#include "BIGfiles/BIGfat.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDImsg.h"
#include "EDIeditors_infos.h"
#include "MATerial/MATframe.h"
#include <assert.h>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------
#define TEXTURE_PATH "ROOT/EngineDatas/01 Texture Bank"
#define AUTO_TEXTURE_MOSSMAP_NAME "mousse03_1024_XE_C.tex"
#define AUTO_TEXTURE_DETAILMAP_NAME "mousse03_1024_XE_N.tex"

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------
extern ULONG    EDI_OUT_gl_ForceSetMode;

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   SELectionGridCtrl::SELectionGridCtrl():
/// \author    NBeaufils
/// \date      2005-05-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
SELectionGridCtrl::SELectionGridCtrl():
mpst_Multi_Sample(NULL)
{
	m_pSELectionHelper = new SELectionHelper();
}

//------------------------------------------------------------
//   SELectionGridCtrl::~SELectionGridCtrl()
/// \author    NBeaufils
/// \date      2005-05-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
SELectionGridCtrl::~SELectionGridCtrl()
{
	if ( m_pSELectionHelper )
		delete m_pSELectionHelper;

	if(mpst_Multi_Sample) 
		mpst_Multi_Sample->st_Id.i->pfn_Destroy(mpst_Multi_Sample);
}

//------------------------------------------------------------
//   void SELectionGridCtrl::InitializeGrid()
/// \author    NBeaufils
/// \date      2005-05-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::InitializeGrid()
{
	SetColumnCount(eCOL_TOTAL);
	SetRowCount(1);
	SetFixedRowCount(1);
	SetFixedRowSelection(FALSE);
	SetFixedColumnSelection(TRUE);
	SetSingleColSelection(TRUE);
	SetSingleRowSelection(FALSE);
	SetRowResize(FALSE);
	SetHeaderSort(TRUE);
	EnableHiddenColUnhide(FALSE);
	EnableHiddenRowUnhide(FALSE);
	EnableColumnHide(FALSE);
	EnableRowHide(FALSE);

	m_BtnDataBase.SetGrid(this);

	for (int i = 0; i < eCOL_TOTAL; i++)
	{
		SetItemText(0,i, GetSELectionGridColumnName(i));

		if ( i < eSEC_GENERAL )
			GetCell(0,i)->SetBackClr(GetSELectionGridSectionColor(eSEC_GENERAL));
		else if ( i < eSEC_STANDARD_PARAM )
			GetCell(0,i)->SetBackClr(GetSELectionGridSectionColor(eSEC_STANDARD_PARAM));
		else if ( i < eSEC_SHADING_PARAM )
			GetCell(0,i)->SetBackClr(GetSELectionGridSectionColor(eSEC_SHADING_PARAM));
		else if ( i < eSEC_NORMALMAP_PARAM )
			GetCell(0,i)->SetBackClr(GetSELectionGridSectionColor(eSEC_NORMALMAP_PARAM));
		else if ( i < eSEC_DETAILNORMALMAP_PARAM )
			GetCell(0,i)->SetBackClr(GetSELectionGridSectionColor(eSEC_DETAILNORMALMAP_PARAM));
		else if ( i < eSEC_SPECULARMAP_PARAM )
			GetCell(0,i)->SetBackClr(GetSELectionGridSectionColor(eSEC_SPECULARMAP_PARAM));
		else if ( i < eSEC_ENVIRONMENTMAP_PARAM )
			GetCell(0,i)->SetBackClr(GetSELectionGridSectionColor(eSEC_ENVIRONMENTMAP_PARAM));
		else if ( i < eSEC_MOSS_PARAM )
			GetCell(0,i)->SetBackClr(GetSELectionGridSectionColor(eSEC_MOSS_PARAM));
        else if ( i < eSEC_RIMLIGHT_PARAM )
            GetCell(0,i)->SetBackClr(GetSELectionGridSectionColor(eSEC_RIMLIGHT_PARAM));
		else if ( i < eSEC_MESHPROCESSING_PARAM )
			GetCell(0,i)->SetBackClr(GetSELectionGridSectionColor(eSEC_MESHPROCESSING_PARAM));

	}

}

//------------------------------------------------------------
//   void SELectionGridCtrl::InitializeData(GRO_tdst_Struct* _pst_Gmt)
/// \author    NBeaufils
/// \date      2005-05-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::InitializeData(GRO_tdst_Struct* _pst_Gmt)
{
	if(_pst_Gmt->i->ul_Type == GRO_MaterialMulti)
		SetMaterialMultiToGrid(_pst_Gmt);
	else if (_pst_Gmt->i->ul_Type == GRO_MaterialMultiTexture)
		SetMaterialSingleToGrid(_pst_Gmt);
	else
	{
		assert(NULL && "SELectionGridCtrl::SetData - Don't know how to handle material type.");
	}
}

//------------------------------------------------------------
//   BOOL SELectionGridCtrl::CheckMaterialDuplicate(GRO_tdst_Struct* _pst_Gmt)
/// \author    NBeaufils
/// \date      2005-05-25
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL SELectionGridCtrl::CheckMaterialDuplicate(MAT_tdst_Material* _pst_Gmt, ULONG _indexTextures)
{
	BOOL bFound = FALSE;
	for ( int iRow=1 ; iRow < GetRowCount() ; iRow++ )
	{
		// Get the layer number
		ULONG ulLayerNo = atoi(GetItemText(iRow, eCOL_LayerNo).GetBuffer());
		
		// Get the material
		SELectionGridCtrl_Textures* pst_SELTextures = (SELectionGridCtrl_Textures*)GetItemData(iRow, eCOL_MaterialName);
		
		// Make sure there isn't no duplicate (NOTE: ulLayerNo-1 because _indexTextures is zero based)
		if ( _pst_Gmt == pst_SELTextures->pst_ACTIVE_GRO && (ulLayerNo-1) == _indexTextures)
			return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------
//   void SELectionGridCtrl::SetMaterialSingleToGrid(GRO_tdst_Struct* _pst_Gmt)
/// \author    NBeaufils
/// \date      2005-04-21
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::SetMaterialSingleToGrid(GRO_tdst_Struct* _pst_Gmt)
{
	SELectionGridCtrl_Textures* pst_SELTextures = new SELectionGridCtrl_Textures;
	pst_SELTextures->pst_GRO = (MAT_tdst_Material*)_pst_Gmt;
	pst_SELTextures->pst_ACTIVE_GRO = (MAT_tdst_Material*)_pst_Gmt;

	EMAT_cl_Frame::GRM_To_MUTEX(pst_SELTextures, mpst_Multi_Sample);
	SetTextureToGrid(&pst_SELTextures->Dest.AllLine[0], pst_SELTextures, 0);
}

//------------------------------------------------------------
//   void SELectionGridCtrl::SetMaterialMultiToGrid(GRO_tdst_Struct* _pst_Gmt)
/// \author    NBeaufils
/// \date      2005-04-21
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::SetMaterialMultiToGrid(GRO_tdst_Struct* _pst_Gmt)
{
	MAT_tdst_Multi* pst_Mtt = (MAT_tdst_Multi *) _pst_Gmt;
	for(int indexMTT = 0; indexMTT < pst_Mtt->l_NumberOfSubMaterials; indexMTT++)
	{
		if(pst_Mtt->dpst_SubMaterial[indexMTT] == NULL)
		{
			char szMessage[MAX_PATH];
			sprintf(szMessage, "[WARNING] Invalid reference detected in multi-material %s", _pst_Gmt->sz_Name);
			LINK_PrintStatusMsg(szMessage);
		}
		else
		{
			SELectionGridCtrl_Textures* pst_SELTextures = new SELectionGridCtrl_Textures;
			pst_SELTextures->pst_GRO = (MAT_tdst_Material*)_pst_Gmt;
			pst_SELTextures->pst_ACTIVE_GRO = pst_Mtt->dpst_SubMaterial[indexMTT];

			EMAT_cl_Frame::GRM_To_MUTEX(pst_SELTextures, mpst_Multi_Sample);

			for ( ULONG indexTex=0 ; indexTex < pst_SELTextures->Dest.NumberOfSubTextures ; indexTex++ )
				SetTextureToGrid(&pst_SELTextures->Dest.AllLine[indexTex], pst_SELTextures, indexTex);
		}
	}
}

//------------------------------------------------------------
//   void SELectionGridCtrl::SetTextureToGrid(MUTEX_TextureLine* _pst_TexLine, SELectionGridCtrl_Textures* _pst_SELTextures, ULONG _indexTextures)
/// \author    NBeaufils
/// \date      2005-04-26
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::SetTextureToGrid(MUTEX_TextureLine* _pst_TexLine, SELectionGridCtrl_Textures* _pst_SELTextures, ULONG _indexTextures)
{
	if ( CheckMaterialDuplicate(_pst_SELTextures->pst_ACTIVE_GRO, _indexTextures) )
		return;
	
	int iRow = InsertRow("NOT SET YET");

	for ( int iCol=0 ; iCol < eCOL_TOTAL ; iCol++ )
		m_pSELectionHelper->DataToCell(this, iRow, iCol, _pst_TexLine, _pst_SELTextures, _indexTextures);
}

//------------------------------------------------------------
//   BOOL SELectionGridCtrl::ClickSelectionDialog(BIG_KEY _ulKeyTextureSelected)
/// \author    NBeaufils
/// \date      2005-05-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL SELectionGridCtrl::ClickSelectionDialog(BIG_KEY& _ulKeyTextureSelected, int nCol)
{
	BOOL bCubeMap = FALSE;
	
	if ( nCol == eCOL_EMP_Name )
		bCubeMap = TRUE;
		
	EDIA_cl_FileDialog o_FileDlg(bCubeMap ? "Choose a Cube Map" : "Choose a Texture", 1, TRUE, TRUE, TEXTURE_PATH, bCubeMap ? "*.cbm" : "*.*");

	if ( o_FileDlg.DoModal() == IDOK )
	{
		CString o_Temp;
		o_FileDlg.GetItem(o_FileDlg.mo_File, 1, o_Temp);
		CHAR* psz_Temp = (CHAR*)(LPCSTR)o_Temp;

		if ((o_FileDlg.mo_File.IsEmpty()) || !BIG_b_CheckName(psz_Temp))
			return FALSE;

		// For common textures
		if(TEX_l_File_IsFormatSupported(psz_Temp, -1))
		{
			BIG_INDEX ul_Index = BIG_ul_SearchFileExt(o_FileDlg.masz_FullPath, psz_Temp);

			if(ul_Index == BIG_C_InvalidIndex)
				return FALSE;

			_ulKeyTextureSelected = BIG_FileKey(ul_Index);
		}
		// For cubemap textures
		else if ( bCubeMap )
		{
			BIG_INDEX ul_Index = BIG_ul_SearchFileExt(o_FileDlg.masz_FullPath, psz_Temp);

			if(ul_Index == BIG_C_InvalidIndex)
				return FALSE;

			_ulKeyTextureSelected = BIG_FileKey(ul_Index);
		}
		return TRUE;
	}
	return FALSE;	
}

//------------------------------------------------------------
//   BOOL SELectionGridCtrl::ClickDeleteDialog(BIG_KEY _ulKeyTextureDeleted)
/// \author    NBeaufils
/// \date      2005-05-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL SELectionGridCtrl::ClickDeleteDialog()
{
	return FALSE;	
}

//------------------------------------------------------------
//   void SELectionGridCtrl::EnableCell(int nRow, int nCol, BOOL bEnable)
/// \author    NBeaufils
/// \date      2005-05-10
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::EnableCell(int nRow, int nCol, BOOL bEnable)
{
	CGridCellBase* pCell = GetCell(nRow, nCol);
	if ( ! pCell )
		return ;

	if ( bEnable )
		pCell->SetState(0);
	else
		pCell->SetState(GVNI_READONLY);
}

//------------------------------------------------------------
//   void SELectionGridCtrl::EnableColumns(int nRow, int nCol, BOOL bEnable)
/// \author    NBeaufils
/// \date      2005-05-10
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::EnableColumns(int nRow, int nCol, BOOL bEnable)
{
	int iCol;
	switch ( nCol )
	{
		case eCOL_STD_TextureOnOff :
			for ( iCol=eCOL_STD_TextureOnOff+1 ; iCol < eCOL_TOTAL ; iCol++ )
				EnableCell(nRow, iCol, bEnable);
			
			// Since we re-enable the texture, we must refresh all the texture data
			if ( bEnable )
			{
				MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_STD_TextureOnOff);
				for ( iCol = eCOL_STD_TextureOnOff+1 ; iCol < eCOL_TOTAL ; iCol++ )
					m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			}
			break; 
		case eCOL_STD_UseLocalAlpha :
			EnableCell(nRow, eCOL_STD_LocalAlphaValue, bEnable);
			if ( bEnable )
			{
				MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_STD_LocalAlphaValue);
				m_pSELectionHelper->DataToCell(this, nRow, eCOL_STD_LocalAlphaValue, pst_TexLine);
			}
			break;
		case eCOL_STD_AlphaTest :
			EnableCell(nRow, eCOL_STD_ATTreshold, bEnable);		
			if ( bEnable )
			{
				MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_STD_ATTreshold);
				m_pSELectionHelper->DataToCell(this, nRow, eCOL_STD_ATTreshold, pst_TexLine);
			}
			break;
		case eCOL_XenonOnOff :
			for ( iCol=eCOL_XenonOnOff+1; iCol < eCOL_TOTAL ; iCol++ )
				EnableCell(nRow, iCol, bEnable);

			// Since we re-enable the xenon, we must refresh all the xenon data
			if ( bEnable )
			{
				MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_XenonOnOff);
				for ( iCol = eCOL_XenonOnOff+1 ; iCol < eCOL_TOTAL ; iCol++ )
					m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			}
			break;
		case eCOL_NMP_OnOff :
			for ( iCol=eCOL_NMP_OnOff+1; iCol < eCOL_DNP_OnOff ; iCol++ )
				EnableCell(nRow, iCol, bEnable);

			// Since we re-enable the normal map, we must refresh all the normal map data
			if ( bEnable )
			{
				MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_NMP_OnOff);
				for ( iCol=eCOL_NMP_OnOff+1; iCol < eCOL_DNP_OnOff ; iCol++ )
					m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			}
			break;
		case eCOL_DNP_OnOff :
			for ( iCol=eCOL_DNP_OnOff+1; iCol < eCOL_SMP_OnOff ; iCol++ )
				EnableCell(nRow, iCol, bEnable);

			// Since we re-enable the detail normal map, we must refresh all the detail normal map data
			if ( bEnable )
			{
				MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_DNP_OnOff);
				for ( iCol=eCOL_DNP_OnOff+1; iCol < eCOL_SMP_OnOff ; iCol++ )
					m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			}
			break;
		case eCOL_SMP_OnOff :
			for ( iCol=eCOL_SMP_OnOff+1; iCol < eCOL_EMP_OnOff ; iCol++ )
				EnableCell(nRow, iCol, bEnable);

			// Since we re-enable specular map, we must refresh all the specular map data
			if ( bEnable )
			{
				MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_SMP_OnOff);
				for ( iCol=eCOL_SMP_OnOff+1; iCol < eCOL_EMP_OnOff ; iCol++ )
					m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			}
			break;
		case eCOL_EMP_OnOff :
			for ( iCol=eCOL_EMP_OnOff+1; iCol < eCOL_MMP_OnOff ; iCol++ )
				EnableCell(nRow, iCol, bEnable);

			// Since we re-enable environment map, we must refresh all the environment map data
			if ( bEnable )
			{
				MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_EMP_OnOff);
				for ( iCol=eCOL_EMP_OnOff+1; iCol < eCOL_MMP_OnOff ; iCol++ )
					m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			}
			break;
		case eCOL_MMP_OnOff :
			for ( iCol=eCOL_MMP_OnOff+1; iCol < eCOL_RIM_OnOff ; iCol++ )
				EnableCell(nRow, iCol, bEnable);

			// Since we re-enable map, we must refresh all the  map data
			if ( bEnable )
			{
				MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_MMP_OnOff);
				for ( iCol=eCOL_MMP_OnOff+1; iCol < eCOL_RIM_OnOff ; iCol++ )
					m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			}
			break;
        case eCOL_RIM_OnOff :
            for ( iCol=eCOL_RIM_OnOff+1; iCol < eCOL_MSP_Smooth ; iCol++ )
                EnableCell(nRow, iCol, bEnable);

            // Since we re-enable map, we must refresh all the map data
            if ( bEnable )
            {
                MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_RIM_OnOff);
                for ( iCol=eCOL_RIM_OnOff+1; iCol < eCOL_MSP_Smooth ; iCol++ )
                    m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
            }
            break;
	}
}

//------------------------------------------------------------
//   void SELectionGridCtrl::RefreshColumns(int nRow, int nCol)
/// \author    NBeaufils
/// \date      2005-05-10
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::RefreshColumns(int nRow, int nCol)
{
	int iCol;
	MUTEX_TextureLine* pst_TexLine = NULL;
	switch ( nCol )
	{
		case eCOL_STD_TextureName:
			pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_STD_TextureName);
			for ( iCol=eCOL_STD_TextureName; iCol < eCOL_TOTAL; iCol++ )
				m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			break;
		case eCOL_NMP_Delete:
		case eCOL_NMP_Name:
			pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_NMP_Name);
			for ( iCol=eCOL_NMP_Name; iCol < eCOL_DNP_OnOff ; iCol++ )
				m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			break;
		case eCOL_DNP_Delete:
		case eCOL_DNP_Name:
			pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_DNP_Name);
			for ( iCol=eCOL_DNP_Name; iCol < eCOL_SMP_OnOff ; iCol++ )
				m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			break;
		case eCOL_SMP_Delete:
		case eCOL_SMP_Name:
			pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_SMP_Name);
			for ( iCol=eCOL_SMP_Name; iCol < eCOL_EMP_OnOff ; iCol++ )
				m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			break;
		case eCOL_EMP_Delete:
		case eCOL_EMP_Name:
			pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_EMP_Name);
			for ( iCol=eCOL_EMP_Name; iCol < eCOL_MMP_OnOff ; iCol++ )
				m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			break;
		case eCOL_MMP_Delete:
		case eCOL_MMP_Name:
			pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_MMP_Name);
			for ( iCol=eCOL_MMP_Name; iCol < eCOL_TOTAL ; iCol++ )
				m_pSELectionHelper->DataToCell(this, nRow, iCol, pst_TexLine);
			break;
	}
}

//------------------------------------------------------------
//   void SELectionGridCtrl::InitializeVectorSelectedItem()
/// \author    NBeaufils
/// \date      2005-05-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::InitializeVectorSelectedItem()
{
	m_vcIndexInGrid.clear();

	CCellRange range = GetSelectedCellRange();
	for ( int iRow=1 ; iRow < GetRowCount() ; iRow++ )
	{
		if ( range.InRange(iRow, range.GetMinCol()) )
		{
			SELectionGridCtrl_Textures* pst_SELTextures = (SELectionGridCtrl_Textures*)GetItemData(iRow, eCOL_MaterialName);
			BIG_INDEX ulIndex = LOA_ul_SearchIndexWithAddress((ULONG)pst_SELTextures->pst_ACTIVE_GRO);
			if ( ulIndex != BIG_C_InvalidIndex )
				m_vcIndexInGrid.push_back(ulIndex);
		}
	}
}
//------------------------------------------------------------
//   ULONG SELectionGridCtrl::GetItemReference(HDATACTRLITEM hItem)
/// \author    NBeaufils
/// \date      2005-05-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG SELectionGridCtrl::GetItemReference(HDATACTRLITEM hItem)
{
	// return the data of the index in the vector m_vcIndexInGrid
	return m_vcIndexInGrid[(UINT)hItem]; 
}

//------------------------------------------------------------
//   HDATACTRLITEM SELectionGridCtrl::GetFirstSelectedItem() const
/// \author    NBeaufils
/// \date      2005-05-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
HDATACTRLITEM SELectionGridCtrl::GetFirstSelectedItem() const
{ 
	// return the first index in the vector m_vcIndexInGrid
	if ( m_vcIndexInGrid.size() == 0 )
		return (HDATACTRLITEM)-1; 

	return (HDATACTRLITEM)0; 
}

//------------------------------------------------------------
//   HDATACTRLITEM SELectionGridCtrl::GetNextSelectedItem(HDATACTRLITEM _hItem) const
/// \author    NBeaufils
/// \date      2005-05-11
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
HDATACTRLITEM SELectionGridCtrl::GetNextSelectedItem(HDATACTRLITEM _hItem) const
{
	// return the increment of the index in the vector m_vTreeFileItemsVisible
	_hItem++ ;

	if ( (UINT)_hItem >= m_vcIndexInGrid.size() )
		_hItem = -1 ;

	return (HDATACTRLITEM)_hItem; 
} 

//------------------------------------------------------------
//   void SELectionGridCtrl::AutoSetNormalMapName()
/// \author    NBeaufils
/// \date      2005-05-25
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::AutoSetNormalMapName(int iRow)
{
	MUTEX_TextureLine* pst_TexLine = NULL;

	// Is Normal map already set ?
	pst_TexLine = (MUTEX_TextureLine*)GetItemData(iRow, eCOL_XenonOnOff);	
	if ( pst_TexLine->st_XeInfo.l_NMapId == 0xFFFFFFFF )
	{
		CString strTextureName;
		char sz_TexturePath[BIG_C_MaxLenPath];
		
		strTextureName = GetItemText(iRow, eCOL_STD_TextureName);		
		
		int indexExt = strTextureName.Find(".tex");
		if ( indexExt == -1 )
			return;

		strTextureName.Insert(indexExt, MAT_C_XENON_NMAP_SUFFIXE);

		// Construct Xenon file path name and index
		strcpy(sz_TexturePath, MAT_C_XENON_START_DIR);
		BIG_INDEX ul_DirIndex = BIG_ul_SearchDir(sz_TexturePath);
		if(ul_DirIndex == BIG_C_InvalidIndex)
			ul_DirIndex = BIG_Root();

		// Find file in dir
		BIG_INDEX ul_XenonFileIndex = BIG_ul_SearchFileInDirRec(ul_DirIndex, (char *)(LPCSTR) strTextureName);
		if(ul_XenonFileIndex == BIG_C_InvalidIndex)
			return;
		
		m_pSELectionHelper->RefreshTexture(this, iRow, eCOL_NMP_Name, pst_TexLine, BIG_FileKey(ul_XenonFileIndex));
	}
}

//------------------------------------------------------------
//   void SELectionGridCtrl::AutoSetMossMapAndDetailMapName()
/// \author    NBeaufils
/// \date      2005-05-25
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::AutoSetMossMapAndDetailMapName(int iRow)
{
	MUTEX_TextureLine* pst_TexLine = NULL;
	char sz_TexturePath[BIG_C_MaxLenPath];

	// Is Moss and Detail map already set ?
	pst_TexLine = (MUTEX_TextureLine*)GetItemData(iRow, eCOL_MMP_OnOff);	
	
	if ( pst_TexLine->st_XeInfo.l_MossMapId == 0xFFFFFFFF )
	{
		// Construct Xenon file path name and index
		strcpy(sz_TexturePath, MAT_C_XENON_START_DIR);
		BIG_INDEX ul_DirIndex = BIG_ul_SearchDir(sz_TexturePath);
		if(ul_DirIndex == BIG_C_InvalidIndex)
			ul_DirIndex = BIG_Root();

		// Find file in dir
		BIG_INDEX ul_XenonFileIndex = BIG_ul_SearchFileInDirRec(ul_DirIndex, AUTO_TEXTURE_MOSSMAP_NAME);
		if(ul_XenonFileIndex != BIG_C_InvalidIndex)
			m_pSELectionHelper->RefreshTexture(this, iRow, eCOL_MMP_Name, pst_TexLine, BIG_FileKey(ul_XenonFileIndex));
	}

	if ( pst_TexLine->st_XeInfo.l_DNMapId == 0xFFFFFFFF )
	{
		// Construct Xenon file path name and index
		strcpy(sz_TexturePath, MAT_C_XENON_START_DIR);
		BIG_INDEX ul_DirIndex = BIG_ul_SearchDir(sz_TexturePath);
		if(ul_DirIndex == BIG_C_InvalidIndex)
			ul_DirIndex = BIG_Root();

		// Find file in dir
		BIG_INDEX ul_XenonFileIndex = BIG_ul_SearchFileInDirRec(ul_DirIndex, AUTO_TEXTURE_DETAILMAP_NAME);
		if(ul_XenonFileIndex != BIG_C_InvalidIndex)
			m_pSELectionHelper->RefreshTexture(this, iRow, eCOL_DNP_Name, pst_TexLine, BIG_FileKey(ul_XenonFileIndex));
	}
}
/////////////////////////////////////////////////////////////////////////////
// SELectionGrid message handlers

CGridCellBase* SELectionGridCtrl::CreateCell(int nRow, int nCol)
{
	// If it is first row of grid
	if ( nRow == 0 )
		return CGridCtrl::CreateCell(nRow, nCol);
	
	// Else it is a normal row of grid
	ASSERT(!GetVirtualMode());

	CGridCellBase* pCell = (CGridCellBase*) GetSELectionGridColumnCtrl(nCol)->CreateObject();
	if (!pCell)
		return NULL;

	pCell->SetGrid(this);
	pCell->SetCoords(nRow, nCol); 

	if (nCol < m_nFixedCols)
		pCell->SetState(pCell->GetState() | GVIS_FIXED | GVIS_FIXEDCOL);
	if (nRow < m_nFixedRows)
		pCell->SetState(pCell->GetState() | GVIS_FIXED | GVIS_FIXEDROW);

	// -------------------------------------------------------------------------------------------
	// Setup specials settings for columns
	if ( pCell->IsKindOf(RUNTIME_CLASS(SELectionGridCellButtonName)) )
	{
		SELectionGridCellButtonName* pCellBtn = (SELectionGridCellButtonName*) pCell;
		pCellBtn->SetBtnDataBase( &m_BtnDataBase);
		pCellBtn->SetupBtns(0, DFC_BUTTON, DFCS_BUTTONPUSH, CGridBtnCellBase::CTL_ALIGN_CENTER, 2, FALSE, "");
		pCellBtn->SetFormat(pCell->GetDefaultCell()->GetFormat());
	}
	else if ( pCell->IsKindOf(RUNTIME_CLASS(SELectionGridCellButtonDelete)) )
	{
		SELectionGridCellButtonDelete* pCellBtn = (SELectionGridCellButtonDelete*) pCell;
		pCellBtn->SetBtnDataBase( &m_BtnDataBase);
		pCellBtn->SetupBtns(0, DFC_BUTTON, DFCS_BUTTONPUSH, CGridBtnCellBase::CTL_ALIGN_CENTER,2, FALSE, "Delete");
		pCellBtn->SetText("Delete");
		pCellBtn->SetFormat(pCell->GetDefaultCell()->GetFormat());
	}
	else if ( pCell->IsKindOf(RUNTIME_CLASS(CGridCellNumeric)) )
	{
		CGridCellNumeric* pCellNum = (CGridCellNumeric*) pCell;
		pCellNum->SetFormat(DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX | DT_END_ELLIPSIS);
	}
	else
		pCell->SetFormat(pCell->GetDefaultCell()->GetFormat());

	
	static LONG Add ;
	static CStringArray arrayOptions;

	Add=0;
	arrayOptions.RemoveAll();
	switch ( nCol )
	{
		case eCOL_STD_Color:
		{
			CGridCellCombo* pCellCombo = (CGridCellCombo*) pCell;

			while(MUTEX_BlendingTypes[Add][0] != 0)
				arrayOptions.Add(MUTEX_BlendingTypes[Add++]);

			pCellCombo->SetOptions(arrayOptions);
			pCellCombo->SetStyle(CBS_DROPDOWN); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE
			break;
		}
		case eCOL_STD_Transparency:
		{
			CGridCellCombo* pCellCombo = (CGridCellCombo*) pCell;

			while(MUTEX_TransparencyTypes[Add][0] != 0)
				arrayOptions.Add(MUTEX_TransparencyTypes[Add++]);

			pCellCombo->SetOptions(arrayOptions);
			pCellCombo->SetStyle(CBS_DROPDOWN); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE
			break;
		}
		case eCOL_SMP_ARGB:
		{
			CGridCellCombo* pCellCombo = (CGridCellCombo*) pCell;

			arrayOptions.Add("A");
			arrayOptions.Add("R");
			arrayOptions.Add("G");
			arrayOptions.Add("B");

			pCellCombo->SetOptions(arrayOptions);
			pCellCombo->SetStyle(CBS_DROPDOWN); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE
			break;
		}
	}
	// -------------------------------------------------------------------------------------------
	
	return pCell;
}

//------------------------------------------------------------
//   BOOL SELectionGridCtrl::ValidateEdit(int nRow, int nCol, LPCTSTR str)
/// \author    NBeaufils
/// \date      2005-05-10
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL SELectionGridCtrl::ValidateEdit(int nRow, int nCol, LPCTSTR str)
{
	return m_pSELectionHelper->ValidateData(nCol, str);
}

//------------------------------------------------------------
//   void SELectionGridCtrl::OnEndEditCell(int nRow, int nCol, CString str)
/// \author    NBeaufils
/// \date      2005-05-10
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::OnEndEditCell(int nRow, int nCol, CString str)
{
	bool bDataModified = false;

	M_MF()->BeginWaitCursor();

	for ( int iRowSelected=0 ; iRowSelected < GetRowCount() ; iRowSelected++ )
	{
		if ( IsCellEditable(iRowSelected, nCol) && IsCellSelected(iRowSelected, nCol) && GetRowHeight(iRowSelected) > 0 )
		{
			// Has the data in cell changed ?
			bDataModified |= ( GetItemText(iRowSelected, nCol) != str );

			CGridCtrl::OnEndEditCell(iRowSelected, nCol, str);
			m_pSELectionHelper->CellToData(this, iRowSelected, nCol);

			SELectionGridCtrl_Textures* pst_SELTextures = (SELectionGridCtrl_Textures*)GetItemData(iRowSelected, eCOL_MaterialName);
			EMAT_cl_Frame::MUTEX_To_GRM(pst_SELTextures);
		}
	}

	if ( bDataModified == true )
	{
		// FORCE a refresh MATerial Editor
		EMAT_cl_Frame* po_MaterialEditor = (EMAT_cl_Frame*) M_MF()->po_GetEditorByType(EDI_IDEDIT_MAT, 0);
		if ( po_MaterialEditor )
			po_MaterialEditor->i_OnMessage(EDI_MESSAGE_REFRESHDATA, NULL, NULL);

		// Removed because line above already does a refresh
		//EDI_OUT_gl_ForceSetMode = 1;
		//LINK_Refresh();
		//EDI_OUT_gl_ForceSetMode = 0;
	}
	Refresh();

	M_MF()->EndWaitCursor();

	// Force the cursor back
	HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW); 
	SetCursor(hCursor);
}

//------------------------------------------------------------
//   void SELectionGridCtrl::ClickCheck(int nRow, int nCol, BOOL bChecked)
/// \author    NBeaufils
/// \date      2005-05-10
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::ClickCheck(int nRow, int nCol, BOOL bChecked)
{
	bool bDataModified = false;

	M_MF()->BeginWaitCursor();

	for ( int iRowSelected=0 ; iRowSelected < GetRowCount() ; iRowSelected++ )
	{
		if ( IsCellEditable(iRowSelected, nCol) && IsCellSelected(iRowSelected, nCol) && GetRowHeight(iRowSelected) > 0 )
		{
			SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)GetCell(iRowSelected, nCol);
			
			// Has the data in cell changed ?
			bDataModified |= pCell->GetCheck() && bChecked ;
			pCell->SetCheck(bChecked);

			m_pSELectionHelper->CellToData(this, iRowSelected, nCol);

			SELectionGridCtrl_Textures* pst_SELTextures = (SELectionGridCtrl_Textures*)GetItemData(iRowSelected, eCOL_MaterialName);
			EMAT_cl_Frame::MUTEX_To_GRM(pst_SELTextures);

			// Have we clicked on XenonOnOff column and has it been activated ?
			if ( nCol == eCOL_XenonOnOff && bDataModified && bChecked )
			{	
				AutoSetNormalMapName(iRowSelected);
				EMAT_cl_Frame::MUTEX_To_GRM(pst_SELTextures);
			}
			// Have we clicked on MossOnOff column and has it been activated ?
			if ( nCol == eCOL_MMP_OnOff && bDataModified && bChecked )
			{
				AutoSetMossMapAndDetailMapName(iRowSelected);

				MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*) GetItemData(nRow, eCOL_DNP_OnOff);
				pst_TexLine->st_XeInfo.b_DNMapDisabled = FALSE;
				m_pSELectionHelper->DataToCell(this, nRow, eCOL_DNP_OnOff, pst_TexLine);
				EMAT_cl_Frame::MUTEX_To_GRM(pst_SELTextures);
			}
			EnableColumns(iRowSelected, nCol, bChecked);
		}
	}
	
	// FORCE a refresh MATerial Editor
	EMAT_cl_Frame* po_MaterialEditor = (EMAT_cl_Frame*) M_MF()->po_GetEditorByType(EDI_IDEDIT_MAT, 0);
	if ( po_MaterialEditor )
		po_MaterialEditor->i_OnMessage(EDI_MESSAGE_REFRESHDATA, NULL, NULL);

	// Removed because line above already does a refresh
	//EDI_OUT_gl_ForceSetMode = 1;
	//LINK_Refresh();
	//EDI_OUT_gl_ForceSetMode = 0;

	Refresh();

	M_MF()->EndWaitCursor();
	
	// Force the cursor back
	HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW); 
	SetCursor(hCursor);
}

//------------------------------------------------------------
//   void SELectionGridCtrl::ClickButton(int nRow, int nCol, ULONG ulKeyTexture)
/// \author    NBeaufils
/// \date      2005-05-10
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionGridCtrl::ClickButton(int nRow, int nCol, ULONG ulKeyTexture)
{
	M_MF()->BeginWaitCursor();
	
	for ( int iRowSelected=0 ; iRowSelected < GetRowCount() ; iRowSelected++ )
	{
		if ( IsCellEditable(iRowSelected, nCol) && IsCellSelected(iRowSelected, nCol) && GetRowHeight(iRowSelected) > 0 )
		{
			MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)GetItemData(iRowSelected, nCol);
			m_pSELectionHelper->RefreshTexture(this, iRowSelected, nCol, pst_TexLine, ulKeyTexture);

			SELectionGridCtrl_Textures* pst_SELTextures = (SELectionGridCtrl_Textures*)GetItemData(iRowSelected, eCOL_MaterialName);
			EMAT_cl_Frame::MUTEX_To_GRM(pst_SELTextures);

			RefreshColumns(iRowSelected, nCol);
		}
	}
	// FORCE a refresh MATerial Editor
	EMAT_cl_Frame* po_MaterialEditor = (EMAT_cl_Frame*) M_MF()->po_GetEditorByType(EDI_IDEDIT_MAT, 0);
	if ( po_MaterialEditor )
		po_MaterialEditor->i_OnMessage(EDI_MESSAGE_REFRESHDATA, NULL, NULL);

	// Removed because line above already does a refresh
	//EDI_OUT_gl_ForceSetMode = 1;
	//LINK_Refresh();
	//EDI_OUT_gl_ForceSetMode = 0;

	Refresh();

	M_MF()->EndWaitCursor();

	// Force the cursor back
	HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW); 
	SetCursor(hCursor);
}


//------------------------------------------------------------------------------