//------------------------------------------------------------------------------
// Filename   :SELectionHelper_CellToData.cpp
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: Implementation of SELectionHelper_CellToData
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#include "SELectionHelper.h"
#include "SELectionGridCtrl.h"

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// externals and SELectionHelper::global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// statics and SELectionHelper::constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   void SELectionHelper::CellToData_MaterialName(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_MaterialName(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_LayerNo(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_LayerNo(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_STD_TextureName(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_STD_TextureName(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	CString strText = _poGridCtrl->GetItemText(_iRow, eCOL_STD_TextureName);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_STD_TextureName);
	if ( pst_TexLine )
		strcpy(pst_TexLine->TextureName, strText.GetBuffer());
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_STD_TextureOnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_STD_TextureOnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_STD_TextureOnOff);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_STD_TextureOnOff);	
	if ( pst_TexLine )
		pst_TexLine->bInactive = ! pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_STD_Color(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_STD_Color(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCombo* pCell = (SELectionGridCellCombo*)_poGridCtrl->GetCell(_iRow, eCOL_STD_Color);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_STD_Color);
	if ( pst_TexLine )
	{
		LONG index = 0 ;
		while ( MUTEX_BlendingTypes[index][0] )
		{
			if ( strcmp(MUTEX_BlendingTypes[index], pCell->GetText()) == 0)
			{
				pst_TexLine->TextureBlending = index;
				break;
			}
			index++;
		}
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_STD_Transparency(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_STD_Transparency(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCombo* pCell = (SELectionGridCellCombo*)_poGridCtrl->GetCell(_iRow, eCOL_STD_Transparency);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_STD_Transparency);	
	if ( pst_TexLine )
	{
		LONG index = 0 ;
		while ( MUTEX_TransparencyTypes[index][0] )
		{
			if ( strcmp(MUTEX_TransparencyTypes[index], pCell->GetText()) == 0)
			{
				pst_TexLine->TextureTransparency = index;
				break;
			}
			index++;
		}
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_STD_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_STD_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_STD_ScaleU);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_STD_ScaleU);	
	if ( pst_TexLine )
		pst_TexLine->UScale = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_STD_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_STD_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_STD_ScaleV);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_STD_ScaleV);	
	if ( pst_TexLine )
		pst_TexLine->VScale = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_STD_UseLocalAlpha(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_STD_UseLocalAlpha(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_STD_UseLocalAlpha);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_STD_UseLocalAlpha);	
	if ( pst_TexLine )
	{
		if ( pCell->GetCheck() )
			pst_TexLine->TextureFlags |= 1024;
		else
			pst_TexLine->TextureFlags &= ~1024;
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_STD_LocalAlphaValue(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_STD_LocalAlphaValue(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_STD_LocalAlphaValue);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_STD_LocalAlphaValue);	
	if ( pst_TexLine )
		pst_TexLine->LocalAlpha = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_STD_AlphaTest(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_STD_AlphaTest(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_STD_AlphaTest);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_STD_AlphaTest);	
	if ( pst_TexLine )
	{
		if ( pCell->GetCheck() )
			pst_TexLine->TextureFlags |= 16;
		else
			pst_TexLine->TextureFlags &= ~16;
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_STD_ATTreshold(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_STD_ATTreshold(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_STD_ATTreshold);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_STD_ATTreshold);	
	if ( pst_TexLine )
		pst_TexLine->AlphaTestTreshold = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_XenonOnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_XenonOnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_XenonOnOff);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_XenonOnOff);	
	if ( pst_TexLine )
		pst_TexLine->b_XeUseExtendedProperties = pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_DiffuseR(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_DiffuseR(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_DiffuseR);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_DiffuseR);	
	if ( pst_TexLine )
	{
		// Get NormalBoost
		int iNormalBoost = pst_TexLine->st_XeInfo.ul_DiffuseColor>>24;

		pst_TexLine->st_XeInfo.ul_DiffuseColor = RGB(pCell->GetInt(), GetGValue(pst_TexLine->st_XeInfo.ul_DiffuseColor), GetBValue(pst_TexLine->st_XeInfo.ul_DiffuseColor));

		// Reset NormalBoost
		pst_TexLine->st_XeInfo.ul_DiffuseColor = (pst_TexLine->st_XeInfo.ul_DiffuseColor & 0x00FFFFFF) | ((iNormalBoost)<<24);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_DiffuseG(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_DiffuseG(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_DiffuseG);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_DiffuseG);	
	if ( pst_TexLine )
	{
		// Get NormalBoost
		int iNormalBoost = pst_TexLine->st_XeInfo.ul_DiffuseColor>>24;

		pst_TexLine->st_XeInfo.ul_DiffuseColor = RGB(GetRValue(pst_TexLine->st_XeInfo.ul_DiffuseColor), pCell->GetInt(),GetBValue(pst_TexLine->st_XeInfo.ul_DiffuseColor));

		// Reset NormalBoost
		pst_TexLine->st_XeInfo.ul_DiffuseColor = (pst_TexLine->st_XeInfo.ul_DiffuseColor & 0x00FFFFFF) | ((iNormalBoost)<<24);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_DiffuseB(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_DiffuseB(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_DiffuseB);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_DiffuseB);	
	if ( pst_TexLine )
	{
		// Get NormalBoost
		int iNormalBoost = pst_TexLine->st_XeInfo.ul_DiffuseColor>>24;
		
		pst_TexLine->st_XeInfo.ul_DiffuseColor = RGB(GetRValue(pst_TexLine->st_XeInfo.ul_DiffuseColor), GetGValue(pst_TexLine->st_XeInfo.ul_DiffuseColor), pCell->GetInt());	
		
		// Reset NormalBoost
		pst_TexLine->st_XeInfo.ul_DiffuseColor = (pst_TexLine->st_XeInfo.ul_DiffuseColor & 0x00FFFFFF) | ((iNormalBoost)<<24);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_SpecR(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_SpecR(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_SpecR);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_SpecR);	
	if ( pst_TexLine )
	{
		// Get Detail Boost
		int iDetailBoost = pst_TexLine->st_XeInfo.ul_SpecularColor>>24;

		pst_TexLine->st_XeInfo.ul_SpecularColor = RGB(pCell->GetInt(), GetGValue(pst_TexLine->st_XeInfo.ul_SpecularColor), GetBValue(pst_TexLine->st_XeInfo.ul_SpecularColor));	

		// Reset Detail Boost
		pst_TexLine->st_XeInfo.ul_SpecularColor = (pst_TexLine->st_XeInfo.ul_SpecularColor & 0x00FFFFFF) | (iDetailBoost<<24);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_SpecG(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_SpecG(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_SpecG);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_SpecG);	
	if ( pst_TexLine )
	{
		// Get Detail Boost
		int iDetailBoost = pst_TexLine->st_XeInfo.ul_SpecularColor>>24;

		pst_TexLine->st_XeInfo.ul_SpecularColor = RGB(GetRValue(pst_TexLine->st_XeInfo.ul_SpecularColor), pCell->GetInt(), GetBValue(pst_TexLine->st_XeInfo.ul_SpecularColor));	

		// Reset Detail Boost
		pst_TexLine->st_XeInfo.ul_SpecularColor = (pst_TexLine->st_XeInfo.ul_SpecularColor & 0x00FFFFFF) | (iDetailBoost<<24);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_SpecB(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_SpecB(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_SpecB);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_SpecB);	
	if ( pst_TexLine )
	{
		// Get Detail Boost
		int iDetailBoost = pst_TexLine->st_XeInfo.ul_SpecularColor>>24;

		pst_TexLine->st_XeInfo.ul_SpecularColor = RGB(GetRValue(pst_TexLine->st_XeInfo.ul_SpecularColor), GetGValue(pst_TexLine->st_XeInfo.ul_SpecularColor), pCell->GetInt());	

		// Reset Detail Boost
		pst_TexLine->st_XeInfo.ul_SpecularColor = (pst_TexLine->st_XeInfo.ul_SpecularColor & 0x00FFFFFF) | (iDetailBoost<<24);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_SpecShininess(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_SpecShininess(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_SpecShininess);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_SpecShininess);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.f_SpecularExp = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_SpecStrength(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_SpecStrength(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_SpecStrength);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_SpecStrength);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.f_SpecularBias = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_AlphaRLow(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_AlphaRLow(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_AlphaRLow);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_AlphaRLow);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.f_AlphaStart = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_AlphaRHigh(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_AlphaRHigh(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_AlphaRHigh);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_AlphaRHigh);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.f_AlphaEnd = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_DiffuseMipMap(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_DiffuseMipMap(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_DiffuseMipMap);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_DiffuseMipMap);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.f_BaseMipMapLODBias = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_NormalMipMap(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_NormalMipMap(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_NormalMipMap);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_NormalMipMap);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.f_NormalMipMapLODBias = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_TwoSided(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_TwoSided(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_TwoSided);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_TwoSided);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_TwoSided = pCell->GetCheck();
}

//------------------------------------------------------------
//   void SELectionHelper::CellToData_SHP_Glow(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SHP_Glow(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_Glow);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SHP_Glow);
    if ( pst_TexLine )
        pst_TexLine->st_XeInfo.b_GlowEnable = pCell->GetCheck();
}

//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	CString strText = _poGridCtrl->GetItemText(_iRow, eCOL_NMP_Name);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_Name);	
	if ( pst_TexLine )
		strcpy(pst_TexLine->st_XeInfo.sz_NMapName, strText.GetBuffer());
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_OnOff);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_OnOff);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_NMapDisabled = ! pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_NormalBoost(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_NormalBoost(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_NormalBoost);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_NormalBoost);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.ul_DiffuseColor = (pst_TexLine->st_XeInfo.ul_DiffuseColor & 0x00FFFFFF) | ((pCell->GetInt())<<24);
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_Transform);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_Transform);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_NMapTransform = pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_Absolute(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_Absolute(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_Absolute);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_Absolute);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_NMapAbsolute = pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_ScaleU);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_ScaleU);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_NMapTransform.f_ScaleU = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_ScaleV);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_ScaleV);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_NMapTransform.f_ScaleV = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_ScaleV);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_ScaleV);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_NMapTransform.f_StartU = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_StartV);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_StartV);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_NMapTransform.f_StartV = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_Angle(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_Angle(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_Angle);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_Angle);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_NMapTransform.f_Angle = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_NMP_Roll(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_NMP_Roll(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_Roll);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_NMP_Roll);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_NMapTransform.f_RollSpeed = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	CString strText = _poGridCtrl->GetItemText(_iRow, eCOL_DNP_Name);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_Name);	
	if ( pst_TexLine )
		strcpy(pst_TexLine->st_XeInfo.sz_DNMapName, strText.GetBuffer());
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_OnOff);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_OnOff);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_DNMapDisabled = ! pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_DetailBoost(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_DetailBoost(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_DetailBoost);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_DetailBoost);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.ul_SpecularColor = (pst_TexLine->st_XeInfo.ul_SpecularColor & 0x00FFFFFF) | ((pCell->GetInt())<<24);
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_Transform);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_Transform);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_DNMapTransform = pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_ScaleU);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_ScaleU);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_DNMapTransform.f_ScaleU = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_ScaleV);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_ScaleV);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_DNMapTransform.f_ScaleV = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_StartU);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_StartU);	
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_DNMapTransform.f_StartU = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_StartV);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_StartV);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_DNMapTransform.f_StartV = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_Strength(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_Strength(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_Strength);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_Strength);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.f_DNMapStrength = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_LODStart(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_LODStart(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_LODStart);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_LODStart);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.ul_DNMapLODStart = pCell->GetInt();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_DNP_LODFull(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_DNP_LODFull(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_LODFull);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_DNP_LODFull);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.ul_DNMapLODFull = pCell->GetInt();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	CString strText = _poGridCtrl->GetItemText(_iRow, eCOL_SMP_Name);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_Name);
	if ( pst_TexLine )
		strcpy(pst_TexLine->st_XeInfo.sz_SpecularMapName, strText.GetBuffer());
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_OnOff);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_OnOff);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_SMapDisabled = ! pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_ARGB(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_ARGB(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCombo* pCell = (SELectionGridCellCombo*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_ARGB);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_ARGB);
	if ( pst_TexLine )
	{
		switch ( pCell->GetText()[0] )
		{
			case 'A' :
				pst_TexLine->st_XeInfo.ul_SpecularMapChannel = 0;
				break;
			case 'R' :
				pst_TexLine->st_XeInfo.ul_SpecularMapChannel = 1;
				break;
			case 'G' :
				pst_TexLine->st_XeInfo.ul_SpecularMapChannel = 2;
				break;
			case 'B' :
				pst_TexLine->st_XeInfo.ul_SpecularMapChannel = 3;
				break;
		}
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_Transform);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_Transform);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_SMapTransform = pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_Absolute(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_Absolute(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_Absolute);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_Absolute);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_NMapAbsolute = pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_ScaleU);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_ScaleU);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_SMapTransform.f_ScaleU = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_ScaleV);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_ScaleV);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_SMapTransform.f_ScaleV = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_StartU);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_StartU);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_SMapTransform.f_StartU = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_StartV);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_StartV);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_SMapTransform.f_StartV = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_Angle(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_Angle(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_Angle);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_Angle);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_SMapTransform.f_Angle = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_SMP_Roll(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_SMP_Roll(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_Roll);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_SMP_Roll);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.st_SMapTransform.f_RollSpeed = pCell->GetFloat();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_EMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_EMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	CString strText = _poGridCtrl->GetItemText(_iRow, eCOL_EMP_Name);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_EMP_Name);
	if ( pst_TexLine )
		strcpy(pst_TexLine->st_XeInfo.sz_EnvMapName, strText.GetBuffer());
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_EMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_EMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_EMP_OnOff);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_EMP_OnOff);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_EMapDisabled = ! pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_EMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_EMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_EMP_ColorR(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_EMP_ColorR(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_EMP_ColorR);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_EMP_ColorR);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.ul_EnvMapColor = RGB(pCell->GetInt(), GetGValue(pst_TexLine->st_XeInfo.ul_EnvMapColor), GetBValue(pst_TexLine->st_XeInfo.ul_EnvMapColor));	
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_EMP_ColorG(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_EMP_ColorG(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_EMP_ColorG);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_EMP_ColorG);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.ul_EnvMapColor = RGB(GetRValue(pst_TexLine->st_XeInfo.ul_EnvMapColor), pCell->GetInt(), GetBValue(pst_TexLine->st_XeInfo.ul_EnvMapColor));		
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_EMP_ColorB(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_EMP_ColorB(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_EMP_ColorB);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_EMP_ColorG);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.ul_EnvMapColor = RGB(GetRValue(pst_TexLine->st_XeInfo.ul_EnvMapColor), GetGValue(pst_TexLine->st_XeInfo.ul_EnvMapColor), pCell->GetInt());			
}

//------------------------------------------------------------
//   void SELectionHelper::CellToData_MMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_MMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    CString strText = _poGridCtrl->GetItemText(_iRow, eCOL_MMP_Name);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_MMP_Name);
    if ( pst_TexLine )
		strcpy(pst_TexLine->st_XeInfo.sz_MossMapName, strText.GetBuffer());
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_MMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_MMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_OnOff);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_MMP_OnOff);
    if ( pst_TexLine )
		pst_TexLine->st_XeInfo.b_MossMapDisabled = ! pCell->GetCheck();
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_MMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_MMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_MMP_ColorR(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_MMP_ColorR(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_ColorR);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_MMP_ColorR);
	if ( pst_TexLine )
	{
		// Get Alpha
		int iAlpha = pst_TexLine->st_XeInfo.ul_MossMapColor>>24;

		pst_TexLine->st_XeInfo.ul_MossMapColor = RGB(pCell->GetInt(), GetGValue(pst_TexLine->st_XeInfo.ul_MossMapColor), GetBValue(pst_TexLine->st_XeInfo.ul_MossMapColor));		

		// Reset Alpha
		pst_TexLine->st_XeInfo.ul_MossMapColor = (pst_TexLine->st_XeInfo.ul_MossMapColor & 0x00FFFFFF) | (iAlpha<<24);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_MMP_ColorG(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_MMP_ColorG(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_ColorG);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_MMP_ColorG);
	if ( pst_TexLine )
	{
		// Get Alpha
		int iAlpha = pst_TexLine->st_XeInfo.ul_MossMapColor>>24;

		pst_TexLine->st_XeInfo.ul_MossMapColor = RGB(GetRValue(pst_TexLine->st_XeInfo.ul_MossMapColor), pCell->GetInt(), GetBValue(pst_TexLine->st_XeInfo.ul_MossMapColor));		

		// Reset Alpha
		pst_TexLine->st_XeInfo.ul_MossMapColor = (pst_TexLine->st_XeInfo.ul_MossMapColor & 0x00FFFFFF) | (iAlpha<<24);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::CellToData_MMP_ColorB(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_MMP_ColorB(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_ColorB);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_MMP_ColorB);
	if ( pst_TexLine )
	{
		// Get Alpha
		int iAlpha = pst_TexLine->st_XeInfo.ul_MossMapColor>>24;

		pst_TexLine->st_XeInfo.ul_MossMapColor = RGB(GetRValue(pst_TexLine->st_XeInfo.ul_MossMapColor), GetGValue(pst_TexLine->st_XeInfo.ul_MossMapColor), pCell->GetInt());		

		// Reset Alpha
		pst_TexLine->st_XeInfo.ul_MossMapColor = (pst_TexLine->st_XeInfo.ul_MossMapColor & 0x00FFFFFF) | (iAlpha<<24);
	}
}

//------------------------------------------------------------
//   void SELectionHelper::CellToData_MMP_Alpha(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_MMP_Alpha(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_Alpha);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_MMP_Alpha);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.ul_MossMapColor = (pst_TexLine->st_XeInfo.ul_MossMapColor & 0x00FFFFFF) | ((pCell->GetInt())<<24);
}

//------------------------------------------------------------
//   void SELectionHelper::CellToData_MMP_SpecularMulti(SELectionGridCtrl* _poGridCtrl, int _iRow)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData_MMP_SpecularMulti(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_SpecularMulti);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_MMP_SpecularMulti);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.f_MossSpecularFactor = pCell->GetFloat();
}


void SELectionHelper::CellToData_RIM_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_OnOff);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_RIM_OnOff);
    if ( pst_TexLine )
        pst_TexLine->st_XeInfo.b_RimLightEnable = pCell->GetCheck();
}

void SELectionHelper::CellToData_RIM_WidthMin(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_WidthMin);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_RIM_WidthMin);
    if ( pst_TexLine )
        pst_TexLine->st_XeInfo.f_RimLightWidthMin = pCell->GetFloat();
}

void SELectionHelper::CellToData_RIM_WidthMax(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_WidthMax);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_RIM_WidthMax);
    if ( pst_TexLine )
        pst_TexLine->st_XeInfo.f_RimLightWidthMax = pCell->GetFloat();
}

void SELectionHelper::CellToData_RIM_Intensity(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_Intensity);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_RIM_Intensity);
    if ( pst_TexLine )
        pst_TexLine->st_XeInfo.f_RimLightIntensity = pCell->GetFloat();
}

void SELectionHelper::CellToData_RIM_NMapRatio(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_NMapRatio);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_RIM_NMapRatio);
    if ( pst_TexLine )
        pst_TexLine->st_XeInfo.f_RimLightNormalMapRatio = pCell->GetFloat();
}

void SELectionHelper::CellToData_RIM_SMapAttenuation(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
    SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_SMapAttenuation);
    MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_RIM_SMapAttenuation);
    if ( pst_TexLine )
        pst_TexLine->st_XeInfo.b_RimLightSMapAttenuationEnabled = pCell->GetCheck();
}

void SELectionHelper::CellToData_MSP_Smooth(SELectionGridCtrl* _poGridCtrl, int _iRow)
{
	SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_MSP_Smooth);
	MUTEX_TextureLine* pst_TexLine = (MUTEX_TextureLine*)_poGridCtrl->GetItemData(_iRow, eCOL_MSP_Smooth);
	if ( pst_TexLine )
		pst_TexLine->st_XeInfo.f_SmoothThreshold = pCell->GetFloat();
}

//------------------------------------------------------------------------------

