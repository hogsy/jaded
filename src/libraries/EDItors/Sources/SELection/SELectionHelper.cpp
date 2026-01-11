//------------------------------------------------------------------------------
// Filename   :SELectionHelper.cpp
/// \author    NBeaufils
/// \date      2005-04-21
/// \par       Description: Implementation of SELectionHelper
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include <assert.h>

#include "Material/MuTex.h"
#include "GraphicDK\Sources\TEXture\TEXstruct.h"
#include "TEXture\TEXcubemap.h"
#include "EDItors/Sources/MATerial/MATframe.h"
#include "GraphicDK/Sources/GDInterface/GDInterface.h"
#include "SELectionHelper.h"
#include "SELectionGridCtrl.h"

//------------------------------------------------------------------------------

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
//   SELectionHelper::SELectionHelper()
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
SELectionHelper::SELectionHelper()
{
    DataToCellFunc[eCOL_MaterialName       ] = &SELectionHelper::DataToCell_MaterialName       ;
    DataToCellFunc[eCOL_LayerNo            ] = &SELectionHelper::DataToCell_LayerNo            ;
	DataToCellFunc[eCOL_STD_TextureName    ] = &SELectionHelper::DataToCell_STD_TextureName    ;
    DataToCellFunc[eCOL_STD_TextureOnOff   ] = &SELectionHelper::DataToCell_STD_TextureOnOff   ;
	DataToCellFunc[eCOL_STD_Color		   ] = &SELectionHelper::DataToCell_STD_Color		   ;
    DataToCellFunc[eCOL_STD_Transparency   ] = &SELectionHelper::DataToCell_STD_Transparency   ;
    DataToCellFunc[eCOL_STD_ScaleU         ] = &SELectionHelper::DataToCell_STD_ScaleU         ;
    DataToCellFunc[eCOL_STD_ScaleV         ] = &SELectionHelper::DataToCell_STD_ScaleV         ;
    DataToCellFunc[eCOL_STD_UseLocalAlpha  ] = &SELectionHelper::DataToCell_STD_UseLocalAlpha  ;
    DataToCellFunc[eCOL_STD_LocalAlphaValue] = &SELectionHelper::DataToCell_STD_LocalAlphaValue;
    DataToCellFunc[eCOL_STD_AlphaTest      ] = &SELectionHelper::DataToCell_STD_AlphaTest      ;
    DataToCellFunc[eCOL_STD_ATTreshold     ] = &SELectionHelper::DataToCell_STD_ATTreshold     ;
	DataToCellFunc[eCOL_XenonOnOff		   ] = &SELectionHelper::DataToCell_XenonOnOff	       ;
	DataToCellFunc[eCOL_SHP_DiffuseR       ] = &SELectionHelper::DataToCell_SHP_DiffuseR       ;
    DataToCellFunc[eCOL_SHP_DiffuseG       ] = &SELectionHelper::DataToCell_SHP_DiffuseG       ;
    DataToCellFunc[eCOL_SHP_DiffuseB       ] = &SELectionHelper::DataToCell_SHP_DiffuseB       ;
    DataToCellFunc[eCOL_SHP_SpecR          ] = &SELectionHelper::DataToCell_SHP_SpecR          ;
    DataToCellFunc[eCOL_SHP_SpecG          ] = &SELectionHelper::DataToCell_SHP_SpecG          ;
    DataToCellFunc[eCOL_SHP_SpecB          ] = &SELectionHelper::DataToCell_SHP_SpecB          ;
    DataToCellFunc[eCOL_SHP_SpecShininess  ] = &SELectionHelper::DataToCell_SHP_SpecShininess  ;
    DataToCellFunc[eCOL_SHP_SpecStrength   ] = &SELectionHelper::DataToCell_SHP_SpecStrength   ;
	DataToCellFunc[eCOL_SHP_AlphaRLow	   ] = &SELectionHelper::DataToCell_SHP_AlphaRLow      ;
	DataToCellFunc[eCOL_SHP_AlphaRHigh     ] = &SELectionHelper::DataToCell_SHP_AlphaRHigh     ;
	DataToCellFunc[eCOL_SHP_DiffuseMipMap  ] = &SELectionHelper::DataToCell_SHP_DiffuseMipMap  ;
	DataToCellFunc[eCOL_SHP_NormalMipMap   ] = &SELectionHelper::DataToCell_SHP_NormalMipMap   ;
	DataToCellFunc[eCOL_SHP_TwoSided       ] = &SELectionHelper::DataToCell_SHP_TwoSided	   ;
    DataToCellFunc[eCOL_SHP_Glow           ] = &SELectionHelper::DataToCell_SHP_Glow	       ;
    DataToCellFunc[eCOL_NMP_Name           ] = &SELectionHelper::DataToCell_NMP_Name           ;
    DataToCellFunc[eCOL_NMP_OnOff          ] = &SELectionHelper::DataToCell_NMP_OnOff          ;
    DataToCellFunc[eCOL_NMP_Delete         ] = &SELectionHelper::DataToCell_NMP_Delete         ;
	DataToCellFunc[eCOL_NMP_NormalBoost    ] = &SELectionHelper::DataToCell_NMP_NormalBoost    ;
    DataToCellFunc[eCOL_NMP_Transform      ] = &SELectionHelper::DataToCell_NMP_Transform      ;
    DataToCellFunc[eCOL_NMP_Absolute       ] = &SELectionHelper::DataToCell_NMP_Absolute       ;
    DataToCellFunc[eCOL_NMP_ScaleU         ] = &SELectionHelper::DataToCell_NMP_ScaleU         ;
    DataToCellFunc[eCOL_NMP_ScaleV         ] = &SELectionHelper::DataToCell_NMP_ScaleV         ;
    DataToCellFunc[eCOL_NMP_StartU         ] = &SELectionHelper::DataToCell_NMP_StartU         ;
    DataToCellFunc[eCOL_NMP_StartV         ] = &SELectionHelper::DataToCell_NMP_StartV         ;
    DataToCellFunc[eCOL_NMP_Angle          ] = &SELectionHelper::DataToCell_NMP_Angle          ;
    DataToCellFunc[eCOL_NMP_Roll           ] = &SELectionHelper::DataToCell_NMP_Roll           ;
    DataToCellFunc[eCOL_DNP_Name           ] = &SELectionHelper::DataToCell_DNP_Name           ;
    DataToCellFunc[eCOL_DNP_OnOff          ] = &SELectionHelper::DataToCell_DNP_OnOff          ;
    DataToCellFunc[eCOL_DNP_Delete         ] = &SELectionHelper::DataToCell_DNP_Delete         ;
	DataToCellFunc[eCOL_DNP_DetailBoost    ] = &SELectionHelper::DataToCell_DNP_DetailBoost    ;
    DataToCellFunc[eCOL_DNP_Transform      ] = &SELectionHelper::DataToCell_DNP_Transform      ;
    DataToCellFunc[eCOL_DNP_ScaleU         ] = &SELectionHelper::DataToCell_DNP_ScaleU         ;
    DataToCellFunc[eCOL_DNP_ScaleV         ] = &SELectionHelper::DataToCell_DNP_ScaleV         ;
    DataToCellFunc[eCOL_DNP_StartU         ] = &SELectionHelper::DataToCell_DNP_StartU         ;
    DataToCellFunc[eCOL_DNP_StartV         ] = &SELectionHelper::DataToCell_DNP_StartV         ;
	DataToCellFunc[eCOL_DNP_Strength       ] = &SELectionHelper::DataToCell_DNP_Strength       ;
    DataToCellFunc[eCOL_DNP_LODStart       ] = &SELectionHelper::DataToCell_DNP_LODStart       ;
    DataToCellFunc[eCOL_DNP_LODFull        ] = &SELectionHelper::DataToCell_DNP_LODFull        ;
    DataToCellFunc[eCOL_SMP_Name           ] = &SELectionHelper::DataToCell_SMP_Name           ;
    DataToCellFunc[eCOL_SMP_OnOff          ] = &SELectionHelper::DataToCell_SMP_OnOff          ;
    DataToCellFunc[eCOL_SMP_Delete         ] = &SELectionHelper::DataToCell_SMP_Delete         ;
    DataToCellFunc[eCOL_SMP_ARGB           ] = &SELectionHelper::DataToCell_SMP_ARGB           ;
    DataToCellFunc[eCOL_SMP_Transform      ] = &SELectionHelper::DataToCell_SMP_Transform      ;
    DataToCellFunc[eCOL_SMP_Absolute       ] = &SELectionHelper::DataToCell_SMP_Absolute       ;
    DataToCellFunc[eCOL_SMP_ScaleU         ] = &SELectionHelper::DataToCell_SMP_ScaleU         ;
    DataToCellFunc[eCOL_SMP_ScaleV         ] = &SELectionHelper::DataToCell_SMP_ScaleV         ;
    DataToCellFunc[eCOL_SMP_StartU         ] = &SELectionHelper::DataToCell_SMP_StartU         ;
    DataToCellFunc[eCOL_SMP_StartV         ] = &SELectionHelper::DataToCell_SMP_StartV         ;
    DataToCellFunc[eCOL_SMP_Angle          ] = &SELectionHelper::DataToCell_SMP_Angle          ;
    DataToCellFunc[eCOL_SMP_Roll           ] = &SELectionHelper::DataToCell_SMP_Roll           ;
    DataToCellFunc[eCOL_EMP_Name           ] = &SELectionHelper::DataToCell_EMP_Name           ;
    DataToCellFunc[eCOL_EMP_OnOff          ] = &SELectionHelper::DataToCell_EMP_OnOff          ;
    DataToCellFunc[eCOL_EMP_Delete         ] = &SELectionHelper::DataToCell_EMP_Delete         ;
    DataToCellFunc[eCOL_EMP_ColorR         ] = &SELectionHelper::DataToCell_EMP_ColorR         ;
    DataToCellFunc[eCOL_EMP_ColorG         ] = &SELectionHelper::DataToCell_EMP_ColorG         ;
    DataToCellFunc[eCOL_EMP_ColorB         ] = &SELectionHelper::DataToCell_EMP_ColorB         ;
    DataToCellFunc[eCOL_MMP_Name           ] = &SELectionHelper::DataToCell_MMP_Name           ;
    DataToCellFunc[eCOL_MMP_OnOff          ] = &SELectionHelper::DataToCell_MMP_OnOff          ;
    DataToCellFunc[eCOL_MMP_Delete         ] = &SELectionHelper::DataToCell_MMP_Delete         ;
    DataToCellFunc[eCOL_MMP_ColorR         ] = &SELectionHelper::DataToCell_MMP_ColorR         ;
    DataToCellFunc[eCOL_MMP_ColorG         ] = &SELectionHelper::DataToCell_MMP_ColorG         ;
    DataToCellFunc[eCOL_MMP_ColorB         ] = &SELectionHelper::DataToCell_MMP_ColorB         ;
    DataToCellFunc[eCOL_MMP_Alpha	       ] = &SELectionHelper::DataToCell_MMP_Alpha          ;
	DataToCellFunc[eCOL_MMP_SpecularMulti  ] = &SELectionHelper::DataToCell_MMP_SpecularMulti  ;
    DataToCellFunc[eCOL_RIM_OnOff          ] = &SELectionHelper::DataToCell_RIM_OnOff          ;
    DataToCellFunc[eCOL_RIM_WidthMin	   ] = &SELectionHelper::DataToCell_RIM_WidthMin       ;
    DataToCellFunc[eCOL_RIM_WidthMax	   ] = &SELectionHelper::DataToCell_RIM_WidthMax       ;
    DataToCellFunc[eCOL_RIM_Intensity	   ] = &SELectionHelper::DataToCell_RIM_Intensity      ;
    DataToCellFunc[eCOL_RIM_NMapRatio      ] = &SELectionHelper::DataToCell_RIM_NMapRatio      ;	
    DataToCellFunc[eCOL_RIM_SMapAttenuation] = &SELectionHelper::DataToCell_RIM_SMapAttenuation;
	DataToCellFunc[eCOL_MSP_Smooth		   ] = &SELectionHelper::DataToCell_MSP_Smooth		   ;
    
    CellToDataFunc[eCOL_MaterialName       ] = &SELectionHelper::CellToData_MaterialName       ;
    CellToDataFunc[eCOL_LayerNo            ] = &SELectionHelper::CellToData_LayerNo            ;
    CellToDataFunc[eCOL_STD_TextureName    ] = &SELectionHelper::CellToData_STD_TextureName    ;
    CellToDataFunc[eCOL_STD_TextureOnOff   ] = &SELectionHelper::CellToData_STD_TextureOnOff   ;
    CellToDataFunc[eCOL_STD_Color          ] = &SELectionHelper::CellToData_STD_Color          ;
    CellToDataFunc[eCOL_STD_Transparency   ] = &SELectionHelper::CellToData_STD_Transparency   ;
    CellToDataFunc[eCOL_STD_ScaleU         ] = &SELectionHelper::CellToData_STD_ScaleU         ;
    CellToDataFunc[eCOL_STD_ScaleV         ] = &SELectionHelper::CellToData_STD_ScaleV         ;
    CellToDataFunc[eCOL_STD_UseLocalAlpha  ] = &SELectionHelper::CellToData_STD_UseLocalAlpha  ;
    CellToDataFunc[eCOL_STD_LocalAlphaValue] = &SELectionHelper::CellToData_STD_LocalAlphaValue;
    CellToDataFunc[eCOL_STD_AlphaTest      ] = &SELectionHelper::CellToData_STD_AlphaTest      ;
    CellToDataFunc[eCOL_STD_ATTreshold     ] = &SELectionHelper::CellToData_STD_ATTreshold     ;
	CellToDataFunc[eCOL_XenonOnOff		   ] = &SELectionHelper::CellToData_XenonOnOff		   ;
    CellToDataFunc[eCOL_SHP_DiffuseR       ] = &SELectionHelper::CellToData_SHP_DiffuseR       ;
    CellToDataFunc[eCOL_SHP_DiffuseG       ] = &SELectionHelper::CellToData_SHP_DiffuseG       ;
    CellToDataFunc[eCOL_SHP_DiffuseB       ] = &SELectionHelper::CellToData_SHP_DiffuseB       ;
    CellToDataFunc[eCOL_SHP_SpecR          ] = &SELectionHelper::CellToData_SHP_SpecR          ;
    CellToDataFunc[eCOL_SHP_SpecG          ] = &SELectionHelper::CellToData_SHP_SpecG          ;
    CellToDataFunc[eCOL_SHP_SpecB          ] = &SELectionHelper::CellToData_SHP_SpecB          ;
    CellToDataFunc[eCOL_SHP_SpecShininess  ] = &SELectionHelper::CellToData_SHP_SpecShininess  ;
    CellToDataFunc[eCOL_SHP_SpecStrength   ] = &SELectionHelper::CellToData_SHP_SpecStrength   ;
	CellToDataFunc[eCOL_SHP_AlphaRLow	   ] = &SELectionHelper::CellToData_SHP_AlphaRLow      ;
	CellToDataFunc[eCOL_SHP_AlphaRHigh     ] = &SELectionHelper::CellToData_SHP_AlphaRHigh     ;
	CellToDataFunc[eCOL_SHP_DiffuseMipMap  ] = &SELectionHelper::CellToData_SHP_DiffuseMipMap  ;
	CellToDataFunc[eCOL_SHP_NormalMipMap   ] = &SELectionHelper::CellToData_SHP_NormalMipMap   ;
	CellToDataFunc[eCOL_SHP_TwoSided       ] = &SELectionHelper::CellToData_SHP_TwoSided	   ;
    CellToDataFunc[eCOL_SHP_Glow           ] = &SELectionHelper::CellToData_SHP_Glow	       ;
    CellToDataFunc[eCOL_NMP_Name           ] = &SELectionHelper::CellToData_NMP_Name           ;
    CellToDataFunc[eCOL_NMP_OnOff          ] = &SELectionHelper::CellToData_NMP_OnOff          ;
    CellToDataFunc[eCOL_NMP_Delete         ] = &SELectionHelper::CellToData_NMP_Delete         ;
	CellToDataFunc[eCOL_NMP_NormalBoost    ] = &SELectionHelper::CellToData_NMP_NormalBoost    ;
    CellToDataFunc[eCOL_NMP_Transform      ] = &SELectionHelper::CellToData_NMP_Transform      ;
    CellToDataFunc[eCOL_NMP_Absolute       ] = &SELectionHelper::CellToData_NMP_Absolute       ;
    CellToDataFunc[eCOL_NMP_ScaleU         ] = &SELectionHelper::CellToData_NMP_ScaleU         ;
    CellToDataFunc[eCOL_NMP_ScaleV         ] = &SELectionHelper::CellToData_NMP_ScaleV         ;
    CellToDataFunc[eCOL_NMP_StartU         ] = &SELectionHelper::CellToData_NMP_StartU         ;
    CellToDataFunc[eCOL_NMP_StartV         ] = &SELectionHelper::CellToData_NMP_StartV         ;
    CellToDataFunc[eCOL_NMP_Angle          ] = &SELectionHelper::CellToData_NMP_Angle          ;
    CellToDataFunc[eCOL_NMP_Roll           ] = &SELectionHelper::CellToData_NMP_Roll           ;
    CellToDataFunc[eCOL_DNP_Name           ] = &SELectionHelper::CellToData_DNP_Name           ;
    CellToDataFunc[eCOL_DNP_OnOff          ] = &SELectionHelper::CellToData_DNP_OnOff          ;
    CellToDataFunc[eCOL_DNP_Delete         ] = &SELectionHelper::CellToData_DNP_Delete         ;
	CellToDataFunc[eCOL_DNP_DetailBoost    ] = &SELectionHelper::CellToData_DNP_DetailBoost    ;
    CellToDataFunc[eCOL_DNP_Transform      ] = &SELectionHelper::CellToData_DNP_Transform      ;
    CellToDataFunc[eCOL_DNP_ScaleU         ] = &SELectionHelper::CellToData_DNP_ScaleU         ;
    CellToDataFunc[eCOL_DNP_ScaleV         ] = &SELectionHelper::CellToData_DNP_ScaleV         ;
    CellToDataFunc[eCOL_DNP_StartU         ] = &SELectionHelper::CellToData_DNP_StartU         ;
    CellToDataFunc[eCOL_DNP_StartV         ] = &SELectionHelper::CellToData_DNP_StartV         ;
	CellToDataFunc[eCOL_DNP_Strength       ] = &SELectionHelper::CellToData_DNP_Strength       ;
    CellToDataFunc[eCOL_DNP_LODStart       ] = &SELectionHelper::CellToData_DNP_LODStart       ;
    CellToDataFunc[eCOL_DNP_LODFull        ] = &SELectionHelper::CellToData_DNP_LODFull        ;
    CellToDataFunc[eCOL_SMP_Name           ] = &SELectionHelper::CellToData_SMP_Name           ;
    CellToDataFunc[eCOL_SMP_OnOff          ] = &SELectionHelper::CellToData_SMP_OnOff          ;
    CellToDataFunc[eCOL_SMP_Delete         ] = &SELectionHelper::CellToData_SMP_Delete         ;
    CellToDataFunc[eCOL_SMP_ARGB           ] = &SELectionHelper::CellToData_SMP_ARGB           ;
    CellToDataFunc[eCOL_SMP_Transform      ] = &SELectionHelper::CellToData_SMP_Transform      ;
    CellToDataFunc[eCOL_SMP_Absolute       ] = &SELectionHelper::CellToData_SMP_Absolute       ;
    CellToDataFunc[eCOL_SMP_ScaleU         ] = &SELectionHelper::CellToData_SMP_ScaleU         ;
    CellToDataFunc[eCOL_SMP_ScaleV         ] = &SELectionHelper::CellToData_SMP_ScaleV         ;
    CellToDataFunc[eCOL_SMP_StartU         ] = &SELectionHelper::CellToData_SMP_StartU         ;
    CellToDataFunc[eCOL_SMP_StartV         ] = &SELectionHelper::CellToData_SMP_StartV         ;
    CellToDataFunc[eCOL_SMP_Angle          ] = &SELectionHelper::CellToData_SMP_Angle          ;
    CellToDataFunc[eCOL_SMP_Roll           ] = &SELectionHelper::CellToData_SMP_Roll           ;
    CellToDataFunc[eCOL_EMP_Name           ] = &SELectionHelper::CellToData_EMP_Name           ;
    CellToDataFunc[eCOL_EMP_OnOff          ] = &SELectionHelper::CellToData_EMP_OnOff          ;
    CellToDataFunc[eCOL_EMP_Delete         ] = &SELectionHelper::CellToData_EMP_Delete         ;
    CellToDataFunc[eCOL_EMP_ColorR         ] = &SELectionHelper::CellToData_EMP_ColorR         ;
    CellToDataFunc[eCOL_EMP_ColorG         ] = &SELectionHelper::CellToData_EMP_ColorG         ;
    CellToDataFunc[eCOL_EMP_ColorB         ] = &SELectionHelper::CellToData_EMP_ColorB         ;
    CellToDataFunc[eCOL_MMP_Name           ] = &SELectionHelper::CellToData_MMP_Name           ;
    CellToDataFunc[eCOL_MMP_OnOff          ] = &SELectionHelper::CellToData_MMP_OnOff          ;
    CellToDataFunc[eCOL_MMP_Delete         ] = &SELectionHelper::CellToData_MMP_Delete         ;
    CellToDataFunc[eCOL_MMP_ColorR         ] = &SELectionHelper::CellToData_MMP_ColorR         ;
    CellToDataFunc[eCOL_MMP_ColorG         ] = &SELectionHelper::CellToData_MMP_ColorG         ;
    CellToDataFunc[eCOL_MMP_ColorB         ] = &SELectionHelper::CellToData_MMP_ColorB         ;
	CellToDataFunc[eCOL_MMP_Alpha          ] = &SELectionHelper::CellToData_MMP_Alpha          ;
	CellToDataFunc[eCOL_MMP_SpecularMulti  ] = &SELectionHelper::CellToData_MMP_SpecularMulti  ;
    CellToDataFunc[eCOL_RIM_OnOff          ] = &SELectionHelper::CellToData_RIM_OnOff          ;
    CellToDataFunc[eCOL_RIM_WidthMin	   ] = &SELectionHelper::CellToData_RIM_WidthMin       ;
    CellToDataFunc[eCOL_RIM_WidthMax	   ] = &SELectionHelper::CellToData_RIM_WidthMax       ;
    CellToDataFunc[eCOL_RIM_Intensity	   ] = &SELectionHelper::CellToData_RIM_Intensity      ;
    CellToDataFunc[eCOL_RIM_NMapRatio      ] = &SELectionHelper::CellToData_RIM_NMapRatio      ;	
    CellToDataFunc[eCOL_RIM_SMapAttenuation] = &SELectionHelper::CellToData_RIM_SMapAttenuation;
	CellToDataFunc[eCOL_MSP_Smooth		   ] = &SELectionHelper::CellToData_MSP_Smooth		   ;

	ValidateDataFunc[eCOL_MaterialName       ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_LayerNo            ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_STD_TextureName    ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_STD_TextureOnOff   ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_STD_Color          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_STD_Transparency   ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_STD_ScaleU         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_STD_ScaleV         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_STD_UseLocalAlpha  ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_STD_LocalAlphaValue] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_STD_AlphaTest      ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_STD_ATTreshold     ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_DiffuseR       ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_DiffuseG       ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_DiffuseB       ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_SpecR          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_SpecG          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_SpecB          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_SpecShininess  ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_SpecStrength   ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_AlphaRLow	     ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_AlphaRHigh     ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_DiffuseMipMap  ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_NormalMipMap   ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SHP_TwoSided       ] = &SELectionHelper::ValidateGeneric;
    ValidateDataFunc[eCOL_SHP_Glow           ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_Name           ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_OnOff          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_Delete         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_NormalBoost    ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_Transform      ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_Absolute       ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_ScaleU         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_ScaleV         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_StartU         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_StartV         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_Angle          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_NMP_Roll           ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_Name           ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_OnOff          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_Delete         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_DetailBoost    ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_Transform      ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_ScaleU         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_ScaleV         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_StartU         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_StartV         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_Strength       ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_LODStart       ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_DNP_LODFull        ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_Name           ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_OnOff          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_Delete         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_ARGB           ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_Transform      ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_Absolute       ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_ScaleU         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_ScaleV         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_StartU         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_StartV         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_Angle          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_SMP_Roll           ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_EMP_Name           ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_EMP_OnOff          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_EMP_Delete         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_EMP_ColorR         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_EMP_ColorG         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_EMP_ColorB         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_MMP_Name           ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_MMP_OnOff          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_MMP_Delete         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_MMP_ColorR         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_MMP_ColorG         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_MMP_ColorB         ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_MMP_Alpha          ] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_MMP_SpecularMulti  ] = &SELectionHelper::ValidateGeneric;
    ValidateDataFunc[eCOL_RIM_OnOff          ] = &SELectionHelper::ValidateGeneric;
    ValidateDataFunc[eCOL_RIM_WidthMin	     ] = &SELectionHelper::ValidateGeneric;
    ValidateDataFunc[eCOL_RIM_WidthMax	     ] = &SELectionHelper::ValidateGeneric;
    ValidateDataFunc[eCOL_RIM_Intensity	     ] = &SELectionHelper::ValidateGeneric;
    ValidateDataFunc[eCOL_RIM_NMapRatio      ] = &SELectionHelper::ValidateGeneric;	
    ValidateDataFunc[eCOL_RIM_SMapAttenuation] = &SELectionHelper::ValidateGeneric;
	ValidateDataFunc[eCOL_MSP_Smooth		 ] = &SELectionHelper::ValidateGeneric;
}

//------------------------------------------------------------
//   SELectionHelper::~SELectionHelper()
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
SELectionHelper::~SELectionHelper()
{
}

//------------------------------------------------------------
//   void SELectionHelper::DataToCell(SELectionGridCtrl* _poGridCtrl, int _iRow, int _iCol, MUTEX_TextureLine* _pst_TexLine, SELectionGridCtrl_Textures* _pst_SELTextures, ULONG _indexTextures)
/// \author    NBeaufils
/// \date      2005-05-03
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::DataToCell(SELectionGridCtrl* _poGridCtrl, int _iRow, int _iCol, MUTEX_TextureLine* _pst_TexLine, SELectionGridCtrl_Textures* _pst_SELTextures/*=NULL*/, ULONG _indexTextures/*=NULL*/)
{
	switch ( _iCol )
	{
		case eCOL_MaterialName:
		{
			assert(_pst_SELTextures && "SELectionHelper::DataToCell - NULL pointer, cannot set in column Material Name.");
			_poGridCtrl->SetItemText(_iRow, eCOL_MaterialName, GRO_sz_Struct_GetName(&_pst_SELTextures->pst_ACTIVE_GRO->st_Id));
			_poGridCtrl->SetItemData(_iRow, eCOL_MaterialName, (LPARAM)_pst_SELTextures);
			break;
		}
		case eCOL_LayerNo:
		{
			assert(_pst_SELTextures && "SELectionHelper::DataToCell - NULL texture index, cannot set in column Layer #.");
			CGridCellNumeric* pCell = (CGridCellNumeric*)_poGridCtrl->GetCell(_iRow, eCOL_LayerNo);
			_poGridCtrl->SetItemTextFmt(_iRow, eCOL_LayerNo, "%d", _indexTextures+1);
			pCell->SetData((LPARAM)_indexTextures);
			break;
		}
		default:
		{
			(this->*DataToCellFunc[_iCol])(_poGridCtrl, _iRow, _pst_TexLine);
			break;
		}
	}
}

//------------------------------------------------------------
//   void SELectionHelper::CellToData(SELectionGridCtrl* _poGridCtrl, int _iRow, int _iCol)
/// \author    NBeaufils
/// \date      2005-05-03
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::CellToData(SELectionGridCtrl* _poGridCtrl, int _iRow, int _iCol)
{
	(this->*CellToDataFunc[_iCol])(_poGridCtrl, _iRow);		
}

//------------------------------------------------------------
//   BOOL SELectionHelper::ValidateData(int _iCol, CString _strText)
/// \author    NBeaufils
/// \date      2005-05-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL SELectionHelper::ValidateData(int _iCol, CString _strText)
{
	return (this->*ValidateDataFunc[_iCol])(_strText);		
}

//------------------------------------------------------------
//   void SELectionHelper::UpdateTexture(SELectionGridCtrl* _poGridCtrl, int _iRow, int _iCol, MUTEX_TextureLine* _pst_TexLine, ULONG _ulKeyTextureSelected)
/// \author    NBeaufils
/// \date      2005-05-04
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void SELectionHelper::RefreshTexture(SELectionGridCtrl* _poGridCtrl, int _iRow, int _iCol, MUTEX_TextureLine* _pst_TexLine, ULONG _ulKeyTextureSelected)
{
	LONG l_TextureId = BIG_C_InvalidKey ;
	if ( _ulKeyTextureSelected != BIG_C_InvalidKey )
	{
		// Texture CUBE MAP
		if ( _iCol == eCOL_EMP_Name )
		{
			SHORT s_PrevCubeMapIndex = (SHORT)_pst_TexLine->st_XeInfo.l_EnvMapId;

			// Load the cube map
			l_TextureId = TEX_CubeMap_Add(_ulKeyTextureSelected);

			// Decrement the reference count
			if (s_PrevCubeMapIndex>= 0)
			{
				TEX_CubeMap_Remove(s_PrevCubeMapIndex);
			}
		}
		// Other CUBE MAP
		else
		{
			BOOL bNewTexture = FALSE;
			TEX_tdst_Data* pst_Texture = TEX_pst_List_FindTexture(&TEX_gst_GlobalList, _ulKeyTextureSelected);

			if (pst_Texture)
				l_TextureId = (LONG)pst_Texture->w_Index;
			else
			{
				l_TextureId = (LONG)TEX_w_List_AddTexture(&TEX_gst_GlobalList, _ulKeyTextureSelected, 1);
				bNewTexture = TRUE;
			}

#if defined(_XENON_RENDER)
			if (GDI_b_IsXenonGraphics())
			{
				if (bNewTexture)
					GDI_Xe_ForceLoadLastTexture();
			}
		}
#endif // _XENON_RENDER
	}
	else
	{
		l_TextureId = MAT_Xe_InvalidTextureId;
	}

	switch ( _iCol )
	{
		case eCOL_STD_TextureName:
			_pst_TexLine->TEXTURE_ID = l_TextureId ;
			EMAT_cl_Frame::FetchTextureName(_pst_TexLine->TextureName, l_TextureId, false);
			break ;
		case eCOL_NMP_Delete:
		case eCOL_NMP_Name:
			_pst_TexLine->st_XeInfo.l_NMapId = l_TextureId ;
			EMAT_cl_Frame::FetchTextureName(_pst_TexLine->st_XeInfo.sz_NMapName, l_TextureId, false);
			break ;
		case eCOL_DNP_Delete:
		case eCOL_DNP_Name:
			_pst_TexLine->st_XeInfo.l_DNMapId = l_TextureId ;
			EMAT_cl_Frame::FetchTextureName(_pst_TexLine->st_XeInfo.sz_DNMapName, l_TextureId, false);
			break;
		case eCOL_SMP_Delete:
		case eCOL_SMP_Name:
			_pst_TexLine->st_XeInfo.l_SpecularMapId = l_TextureId ;
			EMAT_cl_Frame::FetchTextureName(_pst_TexLine->st_XeInfo.sz_SpecularMapName, l_TextureId, false);
			break;
		case eCOL_EMP_Delete:
		case eCOL_EMP_Name:
			_pst_TexLine->st_XeInfo.l_EnvMapId = l_TextureId ;
			EMAT_cl_Frame::FetchTextureName(_pst_TexLine->st_XeInfo.sz_EnvMapName, l_TextureId, true);
			break;
		case eCOL_MMP_Delete:
		case eCOL_MMP_Name:
			_pst_TexLine->st_XeInfo.l_MossMapId = l_TextureId ;
			EMAT_cl_Frame::FetchTextureName(_pst_TexLine->st_XeInfo.sz_MossMapName, l_TextureId, false);
			break;
	}
}


//------------------------------------------------------------------------------

