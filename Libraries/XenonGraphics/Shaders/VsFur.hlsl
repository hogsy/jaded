//-----------------------------------------------------------------------------
// Common vertex shader
//-----------------------------------------------------------------------------
#include "VsGeneric.hlsl"

VSOUT ProcessCustomVertexMAT( VSIN Input, 
                     int  iTransform	        = TRANSFORM_NORMAL, 
                     int  iLighting		        = LIGHTING_USE_COLOR,
                     int  iDirLightCount		= 0,
                     int  iOmniLightCount		= 0,
                     int  iSpotLightCount		= 0,
                     int  iCylSpotLightCount	= 0,
                     int  iColorSource          = COLORSOURCE_VERTEX_COLOR,
                     int  iBTexTransform        = TEXTRANSFORM_NONE,
                     int  iUVSource		        = UVSOURCE_OBJECT,
                     bool bFog			        = FOG_OFF,
                     int  iNTexTransform        = TEXTRANSFORM_NO_TEX_COORD,
                     int  iSTexTransform        = TEXTRANSFORM_NO_TEX_COORD,
                     int  iDNTexTransform       = TEXTRANSFORM_NO_TEX_COORD,
                     bool bAddAmbient           = ADD_AMBIENT_ON,
                     bool bAddRLI               = ADD_RLI_ON,
					 bool bRLIScaleAndOffset    = RLI_SCALE_OFFSET_OFF,
                     int  iReflectionType       = REFLECTION_TYPE_NONE,
                     bool bRimLightEnabled      = RIMLIGHT_OFF,
                     bool bRimLightHeightAttenuation = RIMLIGHT_OFF,
                     bool bDepthToColorEnable   = DEPTH_TO_COLOR_OFF,
                     bool bInvertMoss			= INVERTMOSS_OFF,
                     int  iSPG2Mode				= SPG2_MODE_NONE,
                     bool bShadows				= SHADOW_OFF,
                     bool bVertexSpecular		= VERTEX_SPECULAR_OFF)
{
	Input.RLI.a = 1.0 - Input.RLI.a;
	
	float fScale = g_fFurNormalOffset*Input.RLI.a;
	
#ifndef VS_NO_NORMAL	
	Input.Position.xyz += Input.Normal.xyz * fScale;
#endif
	
	Input.TexCoord0.xy += g_vFurTextureOffset;

	return ProcessVertex( Input, iTransform, iLighting, iDirLightCount, iOmniLightCount, iSpotLightCount, iCylSpotLightCount, iColorSource, iBTexTransform, iUVSource, bFog, iNTexTransform, iSTexTransform, iDNTexTransform, bAddAmbient, bAddRLI, bRLIScaleAndOffset, iReflectionType, bRimLightEnabled, bRimLightHeightAttenuation, bDepthToColorEnable, bInvertMoss, iSPG2Mode, bShadows, bVertexSpecular);
}
