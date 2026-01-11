//------------------------------------------------------------------------------
//   SELectionHelper.h
/// \author    NBeaufils
/// \date      2005-04-21
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __SELECTIONUTILS_H__
#define __SELECTIONUTILS_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "SELectionGridData.h"

#include "GRObject/GROstruct.h"
#include "MATerial/MATstruct.h"
#include "EDitors/Sources/MATerial/MATframe.h"

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------
class SELectionGridCtrl;
typedef MAT_tdst_UndoStruct_ SELectionGridCtrl_Textures;

//------------------------------------------------------------------------------

// Class
//------------------------------------------------------------------------------
class SELectionHelper
{
	//--------------------------------------------------------------------------
	// public definitions 
	//--------------------------------------------------------------------------
public:
	SELectionHelper();
	virtual ~SELectionHelper();	

	void DataToCell(SELectionGridCtrl* _poGridCtrl, int _iRow, int _iCol, MUTEX_TextureLine* _pst_TexLine, SELectionGridCtrl_Textures* _pst_SELTextures = NULL, ULONG _indexTextures = NULL);
	void CellToData(SELectionGridCtrl* _poGridCtrl, int _iRow, int _iCol);

	BOOL ValidateData(int _iCol, CString strText);

	void RefreshTexture(SELectionGridCtrl* _poGridCtrl, int _iRow, int _iCol, MUTEX_TextureLine* _pst_TexLine, ULONG _ulKeyTextureSelected);

protected:
	// Data methods
	void DataToCell_MaterialName           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_LayerNo                (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_STD_TextureName        (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_STD_TextureOnOff       (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_STD_Color              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_STD_Transparency       (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_STD_ScaleU             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_STD_ScaleV             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_STD_UseLocalAlpha      (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_STD_LocalAlphaValue    (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_STD_AlphaTest          (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_STD_ATTreshold         (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_XenonOnOff			   (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_DiffuseR           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_DiffuseG           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_DiffuseB           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_SpecR              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_SpecG              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_SpecB              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_SpecShininess      (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_SpecStrength       (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_AlphaRLow		   (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);	
	void DataToCell_SHP_AlphaRHigh         (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_DiffuseMipMap      (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_NormalMipMap	   (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SHP_TwoSided		   (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);	
    void DataToCell_SHP_Glow		       (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);	
    void DataToCell_NMP_Name               (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_Delete             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_NormalBoost        (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_Transform          (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_Absolute           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_ScaleU             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_ScaleV             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_StartU             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_StartV             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_Angle              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_NMP_Roll               (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_Name               (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_Delete             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_DetailBoost        (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_Transform          (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_ScaleU             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_ScaleV             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_StartU             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_StartV             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_Strength           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_LODStart           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_DNP_LODFull            (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_Name               (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_Delete             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_ARGB               (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_Transform          (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_Absolute           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_ScaleU             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_ScaleV             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_StartU             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_StartV             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_Angle              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_SMP_Roll               (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_EMP_Name               (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_EMP_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_EMP_Delete             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_EMP_ColorR             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_EMP_ColorG             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_EMP_ColorB             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_MMP_Name               (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_MMP_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_MMP_Delete             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_MMP_ColorR             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_MMP_ColorG             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_MMP_ColorB             (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_MMP_Alpha	           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_MMP_SpecularMulti      (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_RIM_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_RIM_WidthMin           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_RIM_WidthMax           (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_RIM_Intensity          (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_RIM_NMapRatio          (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
    void DataToCell_RIM_SMapAttenuation    (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	void DataToCell_MSP_Smooth			   (SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
  
	void CellToData_MaterialName           (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_LayerNo                (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_STD_TextureName        (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_STD_TextureOnOff       (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_STD_Color              (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_STD_Transparency       (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_STD_ScaleU             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_STD_ScaleV             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_STD_UseLocalAlpha      (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_STD_LocalAlphaValue    (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_STD_AlphaTest          (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_STD_ATTreshold         (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_XenonOnOff			   (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_DiffuseR           (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_DiffuseG           (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_DiffuseB           (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_SpecR              (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_SpecG              (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_SpecB              (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_SpecShininess      (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_SpecStrength       (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_AlphaRLow		   (SELectionGridCtrl* _poGridCtrl, int _iRow);	
	void CellToData_SHP_AlphaRHigh         (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_DiffuseMipMap      (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_NormalMipMap       (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SHP_TwoSided		   (SELectionGridCtrl* _poGridCtrl, int _iRow);	
    void CellToData_SHP_Glow    		   (SELectionGridCtrl* _poGridCtrl, int _iRow);	
	void CellToData_NMP_Name               (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_Delete             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_NormalBoost        (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_Transform          (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_Absolute           (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_ScaleU             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_ScaleV             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_StartU             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_StartV             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_Angle              (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_NMP_Roll               (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_Name               (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_Delete             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_DetailBoost        (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_Transform          (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_ScaleU             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_ScaleV             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_StartU             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_StartV             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_Strength           (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_LODStart           (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_DNP_LODFull            (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_Name               (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_Delete             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_ARGB               (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_Transform          (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_Absolute           (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_ScaleU             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_ScaleV             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_StartU             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_StartV             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_Angle              (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_SMP_Roll               (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_EMP_Name               (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_EMP_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_EMP_Delete             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_EMP_ColorR             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_EMP_ColorG             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_EMP_ColorB             (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_MMP_Name               (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_MMP_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_MMP_Delete             (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_MMP_ColorR             (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_MMP_ColorG             (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_MMP_ColorB             (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_MMP_Alpha	           (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_MMP_SpecularMulti      (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_RIM_OnOff              (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_RIM_WidthMin           (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_RIM_WidthMax           (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_RIM_Intensity          (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_RIM_NMapRatio          (SELectionGridCtrl* _poGridCtrl, int _iRow);
    void CellToData_RIM_SMapAttenuation    (SELectionGridCtrl* _poGridCtrl, int _iRow);
	void CellToData_MSP_Smooth			   (SELectionGridCtrl* _poGridCtrl, int _iRow);

	typedef void (SELectionHelper::*_DataToCell)(SELectionGridCtrl* _poGridCtrl, int _iRow, MUTEX_TextureLine* _pst_TexLine);
	_DataToCell DataToCellFunc[eCOL_TOTAL];

	typedef void (SELectionHelper::*_CellToData)(SELectionGridCtrl* _poGridCtrl, int _iRow);
	_CellToData CellToDataFunc[eCOL_TOTAL];

	BOOL ValidateGeneric					(CString strText);
	BOOL ValidateGenericText				(CString strText);
	BOOL ValidateGenericFloat				(CString strText);
	BOOL ValidateGenericInteger				(CString strText);
	BOOL ValidateGenericCombo				(CString strText);
	
	typedef BOOL (SELectionHelper::*_ValidateData)(CString strText);
	_ValidateData ValidateDataFunc[eCOL_TOTAL];
};

//------------------------------------------------------------------------------

#endif //#ifndef __SELECTIONUTILS_H__

