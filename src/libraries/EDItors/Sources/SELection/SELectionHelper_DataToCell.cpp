//------------------------------------------------------------------------------
// Filename   :SELectionHelper_DataToCell.cpp
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: Implementation of SELectionHelper_DataToCell
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
// externals and global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   void SELectionHelper::DataToCell_MaterialName(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_MaterialName(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_LayerNo(SELectionGridCtrl* _poGridCtrl,_iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_LayerNo(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_STD_TextureOnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_STD_TextureOnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
{
	SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_STD_TextureOnOff);
	pCell->SetCheck(!_pst_TexLine->bInactive);
	pCell->SetData((LPARAM)_pst_TexLine);

	_poGridCtrl->EnableColumns(_iRow, eCOL_STD_TextureOnOff, pCell->GetCheck());
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_STD_TextureName(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_STD_TextureName(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && !_pst_TexLine->bInactive)
	{
		SELectionGridCellButtonName* pCell = (SELectionGridCellButtonName*) _poGridCtrl->GetCell(_iRow, eCOL_STD_TextureName);
		pCell->SetText(_pst_TexLine->TextureName);
		pCell->SetDrawCtlBtnText(0, _pst_TexLine->TextureName);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_STD_Color(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)   
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_STD_Color(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)   
{
	if ( _pst_TexLine && !_pst_TexLine->bInactive)
	{
		SELectionGridCellCombo* pCell = (SELectionGridCellCombo*)_poGridCtrl->GetCell(_iRow, eCOL_STD_Color);
		if ( (sizeof(MUTEX_BlendingTypes)/sizeof(MUTEX_BlendingTypes[0]) > _pst_TexLine->TextureBlending) )
			pCell->SetText(MUTEX_BlendingTypes[_pst_TexLine->TextureBlending]);
		else
		{
			pCell->SetText("");
			pCell->SetBackClr(RGB(255,0,0));
		}
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_STD_Transparency(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_STD_Transparency(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && !_pst_TexLine->bInactive)
	{
		SELectionGridCellCombo* pCell = (SELectionGridCellCombo*)_poGridCtrl->GetCell(_iRow, eCOL_STD_Transparency);
		if ( (sizeof(MUTEX_TransparencyTypes)/sizeof(MUTEX_TransparencyTypes[0]) > _pst_TexLine->TextureTransparency) )	
			pCell->SetText(MUTEX_TransparencyTypes[_pst_TexLine->TextureTransparency]);
		else
		{
			pCell->SetText("");
			pCell->SetBackClr(RGB(255,0,0));
		}
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_STD_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_STD_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && !_pst_TexLine->bInactive)
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_STD_ScaleU);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_STD_ScaleU, "%.4f", _pst_TexLine->UScale);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_STD_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_STD_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && !_pst_TexLine->bInactive)
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_STD_ScaleV);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_STD_ScaleV, "%.4f", _pst_TexLine->VScale);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_STD_UseLocalAlpha(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_STD_UseLocalAlpha(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && !_pst_TexLine->bInactive)
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_STD_UseLocalAlpha);
		pCell->SetCheck(_pst_TexLine->TextureFlags & 1024);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_STD_UseLocalAlpha, !_pst_TexLine->bInactive && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_STD_LocalAlphaValue(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_STD_LocalAlphaValue(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
{
	if ( _pst_TexLine && !_pst_TexLine->bInactive)
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_STD_LocalAlphaValue);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_STD_LocalAlphaValue, "%.4f", _pst_TexLine->LocalAlpha);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_STD_AlphaTest(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_STD_AlphaTest(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && !_pst_TexLine->bInactive) 
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_STD_AlphaTest);
		pCell->SetCheck(_pst_TexLine->TextureFlags & 16);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_STD_AlphaTest, !_pst_TexLine->bInactive && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_STD_ATTreshold(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_STD_ATTreshold(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && !_pst_TexLine->bInactive)
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_STD_ATTreshold);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_STD_ATTreshold, "%.4f", _pst_TexLine->AlphaTestTreshold);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_XenonOnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_XenonOnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
{
	if ( _pst_TexLine && !_pst_TexLine->bInactive)
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_XenonOnOff);
		pCell->SetCheck(_pst_TexLine->b_XeUseExtendedProperties);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_XenonOnOff, !_pst_TexLine->bInactive && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_DiffuseR(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_DiffuseR(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_DiffuseR);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_DiffuseR, "%d", GetRValue(_pst_TexLine->st_XeInfo.ul_DiffuseColor));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_DiffuseG(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_DiffuseG(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_DiffuseG);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_DiffuseG, "%d", GetGValue(_pst_TexLine->st_XeInfo.ul_DiffuseColor));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_DiffuseB(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_DiffuseB(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_DiffuseB);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_DiffuseB, "%d", GetBValue(_pst_TexLine->st_XeInfo.ul_DiffuseColor));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_SpecR(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_SpecR(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_SpecR);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_SpecR, "%d", GetRValue(_pst_TexLine->st_XeInfo.ul_SpecularColor));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_SpecG(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_SpecG(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_SpecG);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_SpecG, "%d", GetGValue(_pst_TexLine->st_XeInfo.ul_SpecularColor));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_SpecB(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_SpecB(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_SpecB);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_SpecB, "%d", GetBValue(_pst_TexLine->st_XeInfo.ul_SpecularColor));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_SpecShininess(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_SpecShininess(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_SpecShininess);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_SpecShininess, "%.4f", _pst_TexLine->st_XeInfo.f_SpecularExp);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_SpecStrength(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_SpecStrength(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_SpecStrength);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_SpecStrength, "%.4f", _pst_TexLine->st_XeInfo.f_SpecularBias);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_AlphaRLow(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_AlphaRLow(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_AlphaRLow);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_AlphaRLow, "%.4f", _pst_TexLine->st_XeInfo.f_AlphaStart);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_AlphaRHigh(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_AlphaRHigh(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_AlphaRHigh);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_AlphaRHigh, "%.4f", _pst_TexLine->st_XeInfo.f_AlphaEnd);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_DiffuseMipMap((SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_DiffuseMipMap(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_DiffuseMipMap);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_DiffuseMipMap, "%.4f", _pst_TexLine->st_XeInfo.f_BaseMipMapLODBias);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_NormalMipMap(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_NormalMipMap(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_NormalMipMap);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SHP_NormalMipMap, "%.4f", _pst_TexLine->st_XeInfo.f_NormalMipMapLODBias);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_TwoSided(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_TwoSided(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_TwoSided);
		pCell->SetCheck(_pst_TexLine->st_XeInfo.b_TwoSided);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_SHP_TwoSided, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
	}
}

//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SHP_Glow(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-05-09
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SHP_Glow(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_SHP_Glow);
        pCell->SetCheck(_pst_TexLine->st_XeInfo.b_GlowEnable);
        pCell->SetData((LPARAM)_pst_TexLine);

        _poGridCtrl->EnableColumns(_iRow, eCOL_SHP_Glow, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
    }
}

//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellButtonName* pCell = (SELectionGridCellButtonName*) _poGridCtrl->GetCell(_iRow, eCOL_NMP_Name);
		pCell->SetText(_pst_TexLine->st_XeInfo.sz_NMapName);
		pCell->SetDrawCtlBtnText(0, _pst_TexLine->st_XeInfo.sz_NMapName);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_OnOff);
		pCell->SetCheck(!_pst_TexLine->st_XeInfo.b_NMapDisabled);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_NMP_OnOff, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		_poGridCtrl->SetItemData(_iRow, eCOL_NMP_Delete, (LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_NormalBoost(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_NormalBoost(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_NormalBoost);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_NMP_NormalBoost, "%d", ((_pst_TexLine->st_XeInfo.ul_DiffuseColor)>>24));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_Transform);
		pCell->SetCheck(_pst_TexLine->st_XeInfo.b_NMapTransform);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_NMP_Transform, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_Absolute(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_Absolute(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_Absolute);
		pCell->SetCheck(_pst_TexLine->st_XeInfo.b_NMapAbsolute);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_NMP_Absolute, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_ScaleU);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_NMP_ScaleU, "%.4f", _pst_TexLine->st_XeInfo.st_NMapTransform.f_ScaleU);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{	
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_ScaleV);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_NMP_ScaleV, "%.4f", _pst_TexLine->st_XeInfo.st_NMapTransform.f_ScaleV);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_StartU);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_NMP_StartU, "%.4f", _pst_TexLine->st_XeInfo.st_NMapTransform.f_StartU);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_StartV);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_NMP_StartV, "%.4f", _pst_TexLine->st_XeInfo.st_NMapTransform.f_StartV);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_Angle(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_Angle(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_Angle);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_NMP_Angle, "%.4f", _pst_TexLine->st_XeInfo.st_NMapTransform.f_Angle);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_NMP_Roll(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_NMP_Roll(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_NMP_Roll);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_NMP_Roll, "%.4f", _pst_TexLine->st_XeInfo.st_NMapTransform.f_RollSpeed);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellButtonName* pCell = (SELectionGridCellButtonName*) _poGridCtrl->GetCell(_iRow, eCOL_DNP_Name);
		pCell->SetText(_pst_TexLine->st_XeInfo.sz_DNMapName);
		pCell->SetDrawCtlBtnText(0, _pst_TexLine->st_XeInfo.sz_DNMapName);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_OnOff);
		pCell->SetCheck(!_pst_TexLine->st_XeInfo.b_DNMapDisabled);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_DNP_OnOff, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		_poGridCtrl->SetItemData(_iRow, eCOL_DNP_Delete, (LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_DetailBoost(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_DetailBoost(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_DetailBoost);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_DNP_DetailBoost, "%d", ((_pst_TexLine->st_XeInfo.ul_SpecularColor)>>24));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_Transform);
		pCell->SetCheck(_pst_TexLine->st_XeInfo.b_DNMapTransform);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_DNP_Transform, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_ScaleU);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_DNP_ScaleU, "%.4f", _pst_TexLine->st_XeInfo.st_DNMapTransform.f_ScaleU);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_ScaleV);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_DNP_ScaleV, "%.4f", _pst_TexLine->st_XeInfo.st_DNMapTransform.f_ScaleV);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)   
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)   
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_StartU);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_DNP_StartU, "%.4f", _pst_TexLine->st_XeInfo.st_DNMapTransform.f_StartU);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_StartV);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_DNP_StartV, "%.4f", _pst_TexLine->st_XeInfo.st_DNMapTransform.f_StartV);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_Strength(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-05-10
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_Strength(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_Strength);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_DNP_Strength, "%.4f", _pst_TexLine->st_XeInfo.f_DNMapStrength);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_LODStart(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_LODStart(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_LODStart);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_DNP_LODStart, "%d", _pst_TexLine->st_XeInfo.ul_DNMapLODStart);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_DNP_LODFull(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_DNP_LODFull(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_DNP_LODFull);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_DNP_LODFull, "%d", _pst_TexLine->st_XeInfo.ul_DNMapLODFull);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellButtonName* pCell = (SELectionGridCellButtonName*) _poGridCtrl->GetCell(_iRow, eCOL_SMP_Name);
		pCell->SetText(_pst_TexLine->st_XeInfo.sz_SpecularMapName);
		pCell->SetDrawCtlBtnText(0, _pst_TexLine->st_XeInfo.sz_SpecularMapName);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_OnOff);
		pCell->SetCheck(!_pst_TexLine->st_XeInfo.b_SMapDisabled);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_SMP_OnOff, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		_poGridCtrl->SetItemData(_iRow, eCOL_SMP_Delete, (LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_ARGB(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_ARGB(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCombo* pCell = (SELectionGridCellCombo*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_ARGB);
		
		switch ( _pst_TexLine->st_XeInfo.ul_SpecularMapChannel )
		{
			case 0 :
				pCell->SetText("A");
				break;
			case 1 :
				pCell->SetText("R");
				break;
			case 2 :
				pCell->SetText("G");
				break;
			case 3 :
				pCell->SetText("B");
				break;
			default:
				pCell->SetText("");
				pCell->SetBackClr(RGB(255,0,0));
				break;
		}
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_Transform(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_Transform);
		pCell->SetCheck(_pst_TexLine->st_XeInfo.b_SMapTransform);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_SMP_Transform, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_Absolute(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_Absolute(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_Absolute);
		pCell->SetCheck(_pst_TexLine->st_XeInfo.b_NMapAbsolute);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_SMP_Absolute, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_ScaleU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_ScaleU);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SMP_ScaleU, "%.4f", _pst_TexLine->st_XeInfo.st_SMapTransform.f_ScaleU);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)    
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_ScaleV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)    
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_ScaleV);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SMP_ScaleV, "%.4f", _pst_TexLine->st_XeInfo.st_SMapTransform.f_ScaleV);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_StartU(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_StartU);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SMP_StartU, "%.4f", _pst_TexLine->st_XeInfo.st_SMapTransform.f_StartU);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_StartV(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_StartV);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SMP_StartV, "%.4f", _pst_TexLine->st_XeInfo.st_SMapTransform.f_StartV);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_Angle(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)  
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_Angle(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)  
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_Angle);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SMP_Angle, "%.4f", _pst_TexLine->st_XeInfo.st_SMapTransform.f_Angle);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_SMP_Roll(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_SMP_Roll(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_SMP_Roll);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_SMP_Roll, "%.4f", _pst_TexLine->st_XeInfo.st_SMapTransform.f_RollSpeed);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_EMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_EMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellButtonName* pCell = (SELectionGridCellButtonName*) _poGridCtrl->GetCell(_iRow, eCOL_EMP_Name);
		pCell->SetText(_pst_TexLine->st_XeInfo.sz_EnvMapName);
		pCell->SetDrawCtlBtnText(0, _pst_TexLine->st_XeInfo.sz_EnvMapName);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_EMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_EMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_EMP_OnOff);
		pCell->SetCheck(!_pst_TexLine->st_XeInfo.b_EMapDisabled);
		pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_EMP_OnOff, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_EMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_EMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		_poGridCtrl->SetItemData(_iRow, eCOL_EMP_Delete, (LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_EMP_ColorR(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_EMP_ColorR(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_EMP_ColorR);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_EMP_ColorR, "%d", GetRValue(_pst_TexLine->st_XeInfo.ul_EnvMapColor));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_EMP_ColorG(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_EMP_ColorG(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_EMP_ColorG);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_EMP_ColorG, "%d", GetGValue(_pst_TexLine->st_XeInfo.ul_EnvMapColor));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_EMP_ColorB(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_EMP_ColorB(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_EMP_ColorB);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_EMP_ColorB, "%d", GetBValue(_pst_TexLine->st_XeInfo.ul_EnvMapColor));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_MMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_MMP_Name(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
		SELectionGridCellButtonName* pCell = (SELectionGridCellButtonName*) _poGridCtrl->GetCell(_iRow, eCOL_MMP_Name);
		pCell->SetText(_pst_TexLine->st_XeInfo.sz_MossMapName);
		pCell->SetDrawCtlBtnText(0, _pst_TexLine->st_XeInfo.sz_MossMapName);
        pCell->SetData((LPARAM)_pst_TexLine);
    }
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_MMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_MMP_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_OnOff);
        pCell->SetCheck(!_pst_TexLine->st_XeInfo.b_MossMapDisabled);
        pCell->SetData((LPARAM)_pst_TexLine);

		_poGridCtrl->EnableColumns(_iRow, eCOL_MMP_OnOff, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
    }
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_MMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_MMP_Delete(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
		_poGridCtrl->SetItemData(_iRow, eCOL_MMP_Delete, (LPARAM)_pst_TexLine);
    }
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_MMP_ColorR(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_MMP_ColorR(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_ColorR);
        _poGridCtrl->SetItemTextFmt(_iRow, eCOL_MMP_ColorR, "%d", GetRValue(_pst_TexLine->st_XeInfo.ul_MossMapColor));
        pCell->SetData((LPARAM)_pst_TexLine);
    }
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_MMP_ColorG(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_MMP_ColorG(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine) 
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_ColorG);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_MMP_ColorG, "%d", GetGValue(_pst_TexLine->st_XeInfo.ul_MossMapColor));
		pCell->SetData((LPARAM)_pst_TexLine);
    }
}
//------------------------------------------------------------
//   void SELectionHelper::DataToCell_MMP_ColorB(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_MMP_ColorB(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_ColorB);
        _poGridCtrl->SetItemTextFmt(_iRow, eCOL_MMP_ColorB, "%d", GetBValue(_pst_TexLine->st_XeInfo.ul_MossMapColor));
		pCell->SetData((LPARAM)_pst_TexLine);
    }
}

//------------------------------------------------------------
//   void SELectionHelper::DataToCell_MMP_Alpha(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_MMP_Alpha(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellInteger* pCell = (SELectionGridCellInteger*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_Alpha);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_MMP_Alpha, "%d", ((_pst_TexLine->st_XeInfo.ul_MossMapColor)>>24));
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}

//------------------------------------------------------------
//   void SELectionHelper::DataToCell_MMP_SpecularMulti(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell_MMP_SpecularMulti(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_MMP_SpecularMulti);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_MMP_SpecularMulti, "%.4f", _pst_TexLine->st_XeInfo.f_MossSpecularFactor);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}

void SELectionHelper::DataToCell_RIM_OnOff(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_OnOff);
        pCell->SetCheck(_pst_TexLine->st_XeInfo.b_RimLightEnable);
        pCell->SetData((LPARAM)_pst_TexLine);

        _poGridCtrl->EnableColumns(_iRow, eCOL_RIM_OnOff, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
    }
}

void SELectionHelper::DataToCell_RIM_WidthMin(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_WidthMin);
        _poGridCtrl->SetItemTextFmt(_iRow, eCOL_RIM_WidthMin, "%.4f", _pst_TexLine->st_XeInfo.f_RimLightWidthMin);
        pCell->SetData((LPARAM)_pst_TexLine);
    }
}

void SELectionHelper::DataToCell_RIM_WidthMax(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_WidthMax);
        _poGridCtrl->SetItemTextFmt(_iRow, eCOL_RIM_WidthMax, "%.4f", _pst_TexLine->st_XeInfo.f_RimLightWidthMax);
        pCell->SetData((LPARAM)_pst_TexLine);
    }
}

void SELectionHelper::DataToCell_RIM_Intensity(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_Intensity);
        _poGridCtrl->SetItemTextFmt(_iRow, eCOL_RIM_Intensity, "%.4f", _pst_TexLine->st_XeInfo.f_RimLightIntensity);
        pCell->SetData((LPARAM)_pst_TexLine);
    }
}

void SELectionHelper::DataToCell_RIM_NMapRatio(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_NMapRatio);
        _poGridCtrl->SetItemTextFmt(_iRow, eCOL_RIM_NMapRatio, "%.4f", _pst_TexLine->st_XeInfo.f_RimLightNormalMapRatio);
        pCell->SetData((LPARAM)_pst_TexLine);
    }
}

void SELectionHelper::DataToCell_RIM_SMapAttenuation(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
    if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
    {
        SELectionGridCellCheck* pCell = (SELectionGridCellCheck*)_poGridCtrl->GetCell(_iRow, eCOL_RIM_SMapAttenuation);
        pCell->SetCheck(_pst_TexLine->st_XeInfo.b_RimLightSMapAttenuationEnabled);
        pCell->SetData((LPARAM)_pst_TexLine);

        _poGridCtrl->EnableColumns(_iRow, eCOL_RIM_SMapAttenuation, !_pst_TexLine->bInactive && _pst_TexLine->b_XeUseExtendedProperties && pCell->GetCheck());
    }
}

void SELectionHelper::DataToCell_MSP_Smooth(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine)
{
	if ( _pst_TexLine && _pst_TexLine->b_XeUseExtendedProperties )
	{
		SELectionGridCellFloat* pCell = (SELectionGridCellFloat*)_poGridCtrl->GetCell(_iRow, eCOL_MSP_Smooth);
		_poGridCtrl->SetItemTextFmt(_iRow, eCOL_MSP_Smooth, "%.4f", _pst_TexLine->st_XeInfo.f_SmoothThreshold);
		pCell->SetData((LPARAM)_pst_TexLine);
	}
}

//------------------------------------------------------------------------------

