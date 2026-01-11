//------------------------------------------------------------------------------
//   SELectionGridData.h
/// \author    NBeaufils
/// \date      2005-04-22
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __SELECTIONGRIDDATA_H__
#define __SELECTIONGRIDDATA_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include "SElectionGridCtrlCell.h"

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------

enum eSELectionGridCol
{
	eCOL_MaterialName			=0,		// COL #
	eCOL_LayerNo				,		// COL #
	// STANDARD PARAMETERS
	eCOL_STD_TextureOnOff		,		// COL #
	eCOL_STD_TextureName		,		// COL #
	eCOL_STD_Color				,		// COL #
	eCOL_STD_Transparency		,		// COL #
	eCOL_STD_ScaleU				,		// COL #
	eCOL_STD_ScaleV				,		// COL #
	eCOL_STD_UseLocalAlpha		,		// COL #
	eCOL_STD_LocalAlphaValue	,		// COL #
	eCOL_STD_AlphaTest			,		// COL #
	eCOL_STD_ATTreshold			,		// COL #
	// XENON PARAMETERS
	eCOL_XenonOnOff				,		// COL #
	// SHADING PARAMETERS (Xenon)
	eCOL_SHP_DiffuseR			,		// COL #
	eCOL_SHP_DiffuseG			,		// COL #
	eCOL_SHP_DiffuseB			,		// COL #
	eCOL_SHP_SpecR				,		// COL #
	eCOL_SHP_SpecG				,		// COL #
	eCOL_SHP_SpecB				,		// COL #
	eCOL_SHP_SpecShininess		,		// COL #
	eCOL_SHP_SpecStrength		,		// COL #
	eCOL_SHP_AlphaRLow			,		// COL #
	eCOL_SHP_AlphaRHigh			,		// COL #
	eCOL_SHP_DiffuseMipMap		,		// COL #
	eCOL_SHP_NormalMipMap		,		// COL #
	eCOL_SHP_TwoSided			,		// COL #
    eCOL_SHP_Glow			    ,		// COL #
	// NORMAL MAP PARAMETERS (Xenon)
	eCOL_NMP_OnOff				,		// COL #
	eCOL_NMP_Name				,		// COL #
	eCOL_NMP_Delete				,		// COL #
	eCOL_NMP_NormalBoost		,		// COL #
	eCOL_NMP_Transform			,		// COL #
	eCOL_NMP_Absolute			,		// COL #
	eCOL_NMP_ScaleU				,		// COL #
	eCOL_NMP_ScaleV				,		// COL #
	eCOL_NMP_StartU				,		// COL #
	eCOL_NMP_StartV				,		// COL #
	eCOL_NMP_Angle				,		// COL #
	eCOL_NMP_Roll				,		// COL #
	// DETAIL NORMAL MAP PAREMETERS (Xenon)
	eCOL_DNP_OnOff				,		// COL #
	eCOL_DNP_Name				,		// COL #
	eCOL_DNP_Delete				,		// COL #
	eCOL_DNP_DetailBoost		,		// COL #
	eCOL_DNP_Transform			,		// COL #
	eCOL_DNP_ScaleU				,		// COL #
	eCOL_DNP_ScaleV				,		// COL #
	eCOL_DNP_StartU				,		// COL #
	eCOL_DNP_StartV				,		// COL #
	eCOL_DNP_Strength			,		// COL #
	eCOL_DNP_LODStart			,		// COL #
	eCOL_DNP_LODFull			,		// COL #
	// SPECULAR MAP PARAMETERS (Xenon)
	eCOL_SMP_OnOff				,		// COL #
	eCOL_SMP_Name				,		// COL #
	eCOL_SMP_Delete				,		// COL #
	eCOL_SMP_ARGB				,		// COL #
	eCOL_SMP_Transform			,		// COL #
	eCOL_SMP_Absolute			,		// COL #
	eCOL_SMP_ScaleU				,		// COL #
	eCOL_SMP_ScaleV				,		// COL #
	eCOL_SMP_StartU				,		// COL #
	eCOL_SMP_StartV				,		// COL #
	eCOL_SMP_Angle				,		// COL #
	eCOL_SMP_Roll				,		// COL #
	// ENVIRONMENT MAP PARAMETERS (Xenon)
	eCOL_EMP_OnOff				,		// COL #
	eCOL_EMP_Name				,		// COL #
	eCOL_EMP_Delete				,		// COL #
	eCOL_EMP_ColorR				,		// COL #
	eCOL_EMP_ColorG				,		// COL #
	eCOL_EMP_ColorB				,		// COL #
    // MOSS MAP PARAMETERS (Xenon)
    eCOL_MMP_OnOff				,		// COL #
    eCOL_MMP_Name				,		// COL #
    eCOL_MMP_Delete				,		// COL #
    eCOL_MMP_ColorR				,		// COL #
    eCOL_MMP_ColorG				,		// COL #
    eCOL_MMP_ColorB				,		// COL #
	eCOL_MMP_Alpha				,		// COL #
	eCOL_MMP_SpecularMulti      ,		// COL #
    // RIM LIGHT PARAMETERS (Xenon)
    eCOL_RIM_OnOff				,		// COL #
    eCOL_RIM_WidthMin			,	    // COL #
    eCOL_RIM_WidthMax			,		// COL #
    eCOL_RIM_Intensity			,		// COL #
    eCOL_RIM_NMapRatio		    ,		// COL #
    eCOL_RIM_SMapAttenuation	,		// COL #
	// MESH PROCESSING (Xenon)
	eCOL_MSP_Smooth				,		// COL #


    eCOL_TOTAL
};

enum eSELectionGridSection
{
	eSEC_GENERAL				= eCOL_STD_TextureOnOff,
	eSEC_STANDARD_PARAM			= eCOL_SHP_DiffuseR,
	eSEC_SHADING_PARAM			= eCOL_NMP_OnOff,
	eSEC_NORMALMAP_PARAM		= eCOL_DNP_OnOff,
	eSEC_DETAILNORMALMAP_PARAM	= eCOL_SMP_OnOff,
	eSEC_SPECULARMAP_PARAM		= eCOL_EMP_OnOff,
	eSEC_ENVIRONMENTMAP_PARAM	= eCOL_MMP_OnOff,
    eSEC_MOSS_PARAM	            = eCOL_RIM_OnOff,
    eSEC_RIMLIGHT_PARAM	        = eCOL_MSP_Smooth,
	eSEC_MESHPROCESSING_PARAM	= eCOL_TOTAL,
	eSEC_TOTAL
};

#define ENUM_NAME(_ENUMVALUE, _ENUMNAME) case _ENUMVALUE: return #_ENUMNAME;

static char* GetSELectionGridColumnName(int _iCol)
{
	switch(_iCol)
	{
		ENUM_NAME(eCOL_MaterialName			,Material Name)			
		ENUM_NAME(eCOL_LayerNo				,Layer #)			
		ENUM_NAME(eCOL_STD_TextureName		,Texture Name)
		ENUM_NAME(eCOL_STD_TextureOnOff		,Texture On/Off)
		ENUM_NAME(eCOL_STD_Color			,Color)    	
		ENUM_NAME(eCOL_STD_Transparency		,Transparency)
		ENUM_NAME(eCOL_STD_ScaleU			,ScaleU)   	
		ENUM_NAME(eCOL_STD_ScaleV			,ScaleV)       	
		ENUM_NAME(eCOL_STD_UseLocalAlpha	,Use Local Alpha) 
		ENUM_NAME(eCOL_STD_LocalAlphaValue	,Local Alpha Value)
		ENUM_NAME(eCOL_STD_AlphaTest		,Alpha Test)
		ENUM_NAME(eCOL_STD_ATTreshold		,AT Treshold)
		ENUM_NAME(eCOL_XenonOnOff			,Xenon On/Off)
		ENUM_NAME(eCOL_SHP_DiffuseR			,Diffuse R)			
		ENUM_NAME(eCOL_SHP_DiffuseG			,Diffuse G)			
		ENUM_NAME(eCOL_SHP_DiffuseB			,Diffuse B)
		ENUM_NAME(eCOL_SHP_SpecR			,Spec. R)	        	
		ENUM_NAME(eCOL_SHP_SpecG			,Spec. G)	        	
		ENUM_NAME(eCOL_SHP_SpecB			,Spec. B) 	
		ENUM_NAME(eCOL_SHP_SpecShininess	,Spec. Shininess)  
		ENUM_NAME(eCOL_SHP_SpecStrength		,Spec. Strength)
		ENUM_NAME(eCOL_SHP_AlphaRLow		,AlphaR. Low)
		ENUM_NAME(eCOL_SHP_AlphaRHigh		,AlphaR. High)
		ENUM_NAME(eCOL_SHP_DiffuseMipMap	,Diffuse MipMap)
		ENUM_NAME(eCOL_SHP_NormalMipMap		,Normal MipMap)
		ENUM_NAME(eCOL_SHP_TwoSided			,Two Sided)
        ENUM_NAME(eCOL_SHP_Glow			    ,Glow)
		ENUM_NAME(eCOL_NMP_Name				,Map Name)			
		ENUM_NAME(eCOL_NMP_OnOff			,Normal On/Off)        	
		ENUM_NAME(eCOL_NMP_Delete			,Delete)     	
		ENUM_NAME(eCOL_NMP_NormalBoost		,Normal Boost)     	
		ENUM_NAME(eCOL_NMP_Transform		,Transform)	
		ENUM_NAME(eCOL_NMP_Absolute			,Absolute)
		ENUM_NAME(eCOL_NMP_ScaleU			,ScaleU)        	
		ENUM_NAME(eCOL_NMP_ScaleV			,ScaleV)        	
		ENUM_NAME(eCOL_NMP_StartU			,StartU)        	
		ENUM_NAME(eCOL_NMP_StartV			,StartV)	        	
		ENUM_NAME(eCOL_NMP_Angle			,Angle)	        	
		ENUM_NAME(eCOL_NMP_Roll				,Roll)			
		ENUM_NAME(eCOL_DNP_Name				,Detail Name)			
		ENUM_NAME(eCOL_DNP_OnOff			,Detail On/Off)        	
		ENUM_NAME(eCOL_DNP_Delete			,Delete)
		ENUM_NAME(eCOL_DNP_DetailBoost		,Detail Boost)
		ENUM_NAME(eCOL_DNP_Transform		,Transform)
		ENUM_NAME(eCOL_DNP_ScaleU			,ScaleU)        	
		ENUM_NAME(eCOL_DNP_ScaleV			,ScaleV)        	
		ENUM_NAME(eCOL_DNP_StartU			,StartU)        	
		ENUM_NAME(eCOL_DNP_StartV			,StartV)   
		ENUM_NAME(eCOL_DNP_Strength			,Strength)
		ENUM_NAME(eCOL_DNP_LODStart			,LOD Start)			
		ENUM_NAME(eCOL_DNP_LODFull			,LOD Full)
		ENUM_NAME(eCOL_SMP_Name				,Specular Name)			
		ENUM_NAME(eCOL_SMP_OnOff			,Specular On/Off)	        	
		ENUM_NAME(eCOL_SMP_Delete			,Delete)	        	
		ENUM_NAME(eCOL_SMP_ARGB				,A/R/G/B)		
		ENUM_NAME(eCOL_SMP_Transform		,Transform)	
		ENUM_NAME(eCOL_SMP_Absolute			,Absolute)			
		ENUM_NAME(eCOL_SMP_ScaleU			,ScaleU)        	
		ENUM_NAME(eCOL_SMP_ScaleV			,ScaleV)	        	
		ENUM_NAME(eCOL_SMP_StartU			,StartU)        	
		ENUM_NAME(eCOL_SMP_StartV			,StartV)	        	
		ENUM_NAME(eCOL_SMP_Angle			,Angle)	        	
		ENUM_NAME(eCOL_SMP_Roll				,Roll)			
		ENUM_NAME(eCOL_EMP_Name				,Env. Name)			
		ENUM_NAME(eCOL_EMP_OnOff			,Env. On/Off)        	
		ENUM_NAME(eCOL_EMP_Delete			,Delete)        	
		ENUM_NAME(eCOL_EMP_ColorR			,ColorR)	        	
		ENUM_NAME(eCOL_EMP_ColorG			,ColorG)        	
		ENUM_NAME(eCOL_EMP_ColorB			,ColorB)
        ENUM_NAME(eCOL_MMP_OnOff			,Moss On/Off)        	
		ENUM_NAME(eCOL_MMP_Name				,Moss Name)			
        ENUM_NAME(eCOL_MMP_Delete			,Delete)        	
        ENUM_NAME(eCOL_MMP_ColorR			,ColorR)	        	
        ENUM_NAME(eCOL_MMP_ColorG			,ColorG)        	
        ENUM_NAME(eCOL_MMP_ColorB			,ColorB)
		ENUM_NAME(eCOL_MMP_Alpha			,Alpha)
		ENUM_NAME(eCOL_MMP_SpecularMulti    ,Specular Multi.)
        ENUM_NAME(eCOL_RIM_OnOff            ,Rim Light On/Off)
        ENUM_NAME(eCOL_RIM_WidthMin			,Width Min )
        ENUM_NAME(eCOL_RIM_WidthMax			,Width Max )
        ENUM_NAME(eCOL_RIM_Intensity		,Intensity )
        ENUM_NAME(eCOL_RIM_NMapRatio        ,NMap Ratio)
        ENUM_NAME(eCOL_RIM_SMapAttenuation  ,SMap Attenuation)
		ENUM_NAME(eCOL_MSP_Smooth			,Smooth)
	}
	return "Invalid Column Name";
}

static char* GetSELectionGridSectionName(int _iSec)
{
	switch(_iSec)
	{
		ENUM_NAME(eSEC_GENERAL					,General)
		ENUM_NAME(eSEC_STANDARD_PARAM			,Standard)
		ENUM_NAME(eSEC_SHADING_PARAM			,Shading)
		ENUM_NAME(eSEC_NORMALMAP_PARAM			,Normal Map)
		ENUM_NAME(eSEC_DETAILNORMALMAP_PARAM	,Detail Normal Map)
		ENUM_NAME(eSEC_SPECULARMAP_PARAM		,Specular Map)
		ENUM_NAME(eSEC_ENVIRONMENTMAP_PARAM		,Environment Map)
        ENUM_NAME(eSEC_MOSS_PARAM		        ,Moss Map)
        ENUM_NAME(eSEC_RIMLIGHT_PARAM		    ,Rim Light)
		ENUM_NAME(eSEC_MESHPROCESSING_PARAM		,Mesh Processing)
	}
	return "Invalid Section Name";
}

#define ENUM_COLOR(_ENUMVALUE, _ENUMCOLOR) case _ENUMVALUE: return _ENUMCOLOR;

static COLORREF GetSELectionGridSectionColor(int _iSec)
{
	switch(_iSec)
	{
		ENUM_COLOR(eSEC_GENERAL					, RGB(212,208,200))
		ENUM_COLOR(eSEC_STANDARD_PARAM			, RGB(153,204,255))
		ENUM_COLOR(eSEC_SHADING_PARAM			, RGB(255,255,0))
		ENUM_COLOR(eSEC_NORMALMAP_PARAM			, RGB(255,204,0))
		ENUM_COLOR(eSEC_DETAILNORMALMAP_PARAM	, RGB(153,204,0))
		ENUM_COLOR(eSEC_SPECULARMAP_PARAM		, RGB(255,204,153))
		ENUM_COLOR(eSEC_ENVIRONMENTMAP_PARAM	, RGB(255,255,153))
        ENUM_COLOR(eSEC_MOSS_PARAM	            , RGB(141,193,102))
        ENUM_COLOR(eSEC_RIMLIGHT_PARAM	        , RGB(255,75,75))
		ENUM_COLOR(eSEC_MESHPROCESSING_PARAM	, RGB(75,51,117))
	}
	return RGB(0,0,0);
}

#define ENUM_CTRL(_ENUMVALUE, _ENUMCTRL) case _ENUMVALUE: return _ENUMCTRL;

static CRuntimeClass* GetSELectionGridColumnCtrl(int _iCol)
{
	switch(_iCol)
	{
		ENUM_CTRL(eCOL_MaterialName			,RUNTIME_CLASS(SELectionGridCellPerforce))			
		ENUM_CTRL(eCOL_LayerNo				,RUNTIME_CLASS(SELectionGridCellNoModify))
		ENUM_CTRL(eCOL_STD_TextureName		,RUNTIME_CLASS(SELectionGridCellButtonName))			
		ENUM_CTRL(eCOL_STD_TextureOnOff		,RUNTIME_CLASS(SELectionGridCellCheck))
		ENUM_CTRL(eCOL_STD_Color			,RUNTIME_CLASS(SELectionGridCellCombo))    	
		ENUM_CTRL(eCOL_STD_Transparency		,RUNTIME_CLASS(SELectionGridCellCombo))
		ENUM_CTRL(eCOL_STD_ScaleU			,RUNTIME_CLASS(SELectionGridCellFloat)) 	
		ENUM_CTRL(eCOL_STD_ScaleV			,RUNTIME_CLASS(SELectionGridCellFloat))
		ENUM_CTRL(eCOL_STD_UseLocalAlpha	,RUNTIME_CLASS(SELectionGridCellCheck))
		ENUM_CTRL(eCOL_STD_LocalAlphaValue	,RUNTIME_CLASS(SELectionGridCellFloat))
		ENUM_CTRL(eCOL_STD_AlphaTest		,RUNTIME_CLASS(SELectionGridCellCheck))
		ENUM_CTRL(eCOL_STD_ATTreshold		,RUNTIME_CLASS(SELectionGridCellFloat))
		ENUM_CTRL(eCOL_XenonOnOff			,RUNTIME_CLASS(SELectionGridCellCheck))
		ENUM_CTRL(eCOL_SHP_DiffuseR			,RUNTIME_CLASS(SELectionGridCellInteger))			
		ENUM_CTRL(eCOL_SHP_DiffuseG			,RUNTIME_CLASS(SELectionGridCellInteger))			
		ENUM_CTRL(eCOL_SHP_DiffuseB			,RUNTIME_CLASS(SELectionGridCellInteger))
		ENUM_CTRL(eCOL_SHP_SpecR			,RUNTIME_CLASS(SELectionGridCellInteger))	        	
		ENUM_CTRL(eCOL_SHP_SpecG			,RUNTIME_CLASS(SELectionGridCellInteger))	        	
		ENUM_CTRL(eCOL_SHP_SpecB			,RUNTIME_CLASS(SELectionGridCellInteger)) 	
		ENUM_CTRL(eCOL_SHP_SpecShininess	,RUNTIME_CLASS(SELectionGridCellFloat))  
		ENUM_CTRL(eCOL_SHP_SpecStrength		,RUNTIME_CLASS(SELectionGridCellFloat))
		ENUM_CTRL(eCOL_SHP_AlphaRLow		,RUNTIME_CLASS(SELectionGridCellFloat))
		ENUM_CTRL(eCOL_SHP_AlphaRHigh		,RUNTIME_CLASS(SELectionGridCellFloat))
		ENUM_CTRL(eCOL_SHP_DiffuseMipMap	,RUNTIME_CLASS(SELectionGridCellFloat))
		ENUM_CTRL(eCOL_SHP_NormalMipMap		,RUNTIME_CLASS(SELectionGridCellFloat))
		ENUM_CTRL(eCOL_SHP_TwoSided			,RUNTIME_CLASS(SELectionGridCellCheck))
        ENUM_CTRL(eCOL_SHP_Glow			    ,RUNTIME_CLASS(SELectionGridCellCheck))
		ENUM_CTRL(eCOL_NMP_Name				,RUNTIME_CLASS(SELectionGridCellButtonName))			
		ENUM_CTRL(eCOL_NMP_OnOff			,RUNTIME_CLASS(SELectionGridCellCheck))        	
		ENUM_CTRL(eCOL_NMP_Delete			,RUNTIME_CLASS(SELectionGridCellButtonDelete))     	
		ENUM_CTRL(eCOL_NMP_NormalBoost		,RUNTIME_CLASS(SELectionGridCellInteger))
		ENUM_CTRL(eCOL_NMP_Transform		,RUNTIME_CLASS(SELectionGridCellCheck))	
		ENUM_CTRL(eCOL_NMP_Absolute			,RUNTIME_CLASS(SELectionGridCellCheck))
		ENUM_CTRL(eCOL_NMP_ScaleU			,RUNTIME_CLASS(SELectionGridCellFloat))        	
		ENUM_CTRL(eCOL_NMP_ScaleV			,RUNTIME_CLASS(SELectionGridCellFloat))        	
		ENUM_CTRL(eCOL_NMP_StartU			,RUNTIME_CLASS(SELectionGridCellFloat))        	
		ENUM_CTRL(eCOL_NMP_StartV			,RUNTIME_CLASS(SELectionGridCellFloat))	        	
		ENUM_CTRL(eCOL_NMP_Angle			,RUNTIME_CLASS(SELectionGridCellFloat))	        	
		ENUM_CTRL(eCOL_NMP_Roll				,RUNTIME_CLASS(SELectionGridCellFloat))			
		ENUM_CTRL(eCOL_DNP_Name				,RUNTIME_CLASS(SELectionGridCellButtonName))			
		ENUM_CTRL(eCOL_DNP_OnOff			,RUNTIME_CLASS(SELectionGridCellCheck))        	
		ENUM_CTRL(eCOL_DNP_Delete			,RUNTIME_CLASS(SELectionGridCellButtonDelete))     	
		ENUM_CTRL(eCOL_DNP_DetailBoost		,RUNTIME_CLASS(SELectionGridCellInteger))
		ENUM_CTRL(eCOL_DNP_Transform		,RUNTIME_CLASS(SELectionGridCellCheck))
		ENUM_CTRL(eCOL_DNP_ScaleU			,RUNTIME_CLASS(SELectionGridCellFloat))        	
		ENUM_CTRL(eCOL_DNP_ScaleV			,RUNTIME_CLASS(SELectionGridCellFloat))        	
		ENUM_CTRL(eCOL_DNP_StartU			,RUNTIME_CLASS(SELectionGridCellFloat))        	
		ENUM_CTRL(eCOL_DNP_StartV			,RUNTIME_CLASS(SELectionGridCellFloat))      	
		ENUM_CTRL(eCOL_DNP_Strength			,RUNTIME_CLASS(SELectionGridCellFloat))			
		ENUM_CTRL(eCOL_DNP_LODStart			,RUNTIME_CLASS(SELectionGridCellInteger))			
		ENUM_CTRL(eCOL_DNP_LODFull			,RUNTIME_CLASS(SELectionGridCellInteger))
		ENUM_CTRL(eCOL_SMP_Name				,RUNTIME_CLASS(SELectionGridCellButtonName))			
		ENUM_CTRL(eCOL_SMP_OnOff			,RUNTIME_CLASS(SELectionGridCellCheck))	        	
		ENUM_CTRL(eCOL_SMP_Delete			,RUNTIME_CLASS(SELectionGridCellButtonDelete))	        	
		ENUM_CTRL(eCOL_SMP_ARGB				,RUNTIME_CLASS(SELectionGridCellCombo))		
		ENUM_CTRL(eCOL_SMP_Transform		,RUNTIME_CLASS(SELectionGridCellCheck))	
		ENUM_CTRL(eCOL_SMP_Absolute			,RUNTIME_CLASS(SELectionGridCellCheck))			
		ENUM_CTRL(eCOL_SMP_ScaleU			,RUNTIME_CLASS(SELectionGridCellFloat))        	
		ENUM_CTRL(eCOL_SMP_ScaleV			,RUNTIME_CLASS(SELectionGridCellFloat))	        	
		ENUM_CTRL(eCOL_SMP_StartU			,RUNTIME_CLASS(SELectionGridCellFloat))        	
		ENUM_CTRL(eCOL_SMP_StartV			,RUNTIME_CLASS(SELectionGridCellFloat))	        	
		ENUM_CTRL(eCOL_SMP_Angle			,RUNTIME_CLASS(SELectionGridCellFloat))	        	
		ENUM_CTRL(eCOL_SMP_Roll				,RUNTIME_CLASS(SELectionGridCellFloat))			
		ENUM_CTRL(eCOL_EMP_Name				,RUNTIME_CLASS(SELectionGridCellButtonName))			
		ENUM_CTRL(eCOL_EMP_OnOff			,RUNTIME_CLASS(SELectionGridCellCheck))        	
		ENUM_CTRL(eCOL_EMP_Delete			,RUNTIME_CLASS(SELectionGridCellButtonDelete))        	
		ENUM_CTRL(eCOL_EMP_ColorR			,RUNTIME_CLASS(SELectionGridCellInteger))	        	
		ENUM_CTRL(eCOL_EMP_ColorG			,RUNTIME_CLASS(SELectionGridCellInteger))        	
		ENUM_CTRL(eCOL_EMP_ColorB			,RUNTIME_CLASS(SELectionGridCellInteger))
        ENUM_CTRL(eCOL_MMP_Name				,RUNTIME_CLASS(SELectionGridCellButtonName))			
        ENUM_CTRL(eCOL_MMP_OnOff			,RUNTIME_CLASS(SELectionGridCellCheck))        	
        ENUM_CTRL(eCOL_MMP_Delete			,RUNTIME_CLASS(SELectionGridCellButtonDelete))        	
        ENUM_CTRL(eCOL_MMP_ColorR			,RUNTIME_CLASS(SELectionGridCellInteger))	        	
        ENUM_CTRL(eCOL_MMP_ColorG			,RUNTIME_CLASS(SELectionGridCellInteger))        	
        ENUM_CTRL(eCOL_MMP_ColorB			,RUNTIME_CLASS(SELectionGridCellInteger))
		ENUM_CTRL(eCOL_MMP_Alpha			,RUNTIME_CLASS(SELectionGridCellInteger))
		ENUM_CTRL(eCOL_MMP_SpecularMulti    ,RUNTIME_CLASS(SELectionGridCellFloat))
        ENUM_CTRL(eCOL_RIM_OnOff            ,RUNTIME_CLASS(SELectionGridCellCheck))   
        ENUM_CTRL(eCOL_RIM_WidthMin			,RUNTIME_CLASS(SELectionGridCellFloat))	 
        ENUM_CTRL(eCOL_RIM_WidthMax			,RUNTIME_CLASS(SELectionGridCellFloat))	 
        ENUM_CTRL(eCOL_RIM_Intensity		,RUNTIME_CLASS(SELectionGridCellFloat))	 
        ENUM_CTRL(eCOL_RIM_NMapRatio        ,RUNTIME_CLASS(SELectionGridCellFloat))	
        ENUM_CTRL(eCOL_RIM_SMapAttenuation  ,RUNTIME_CLASS(SELectionGridCellCheck))	
		ENUM_CTRL(eCOL_MSP_Smooth			,RUNTIME_CLASS(SELectionGridCellFloat))	
	}
	return NULL;
}


//------------------------------------------------------------------------------

#endif //#ifndef __SELECTIONDATA_H__

