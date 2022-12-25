//-----------------------------------------------------------------------------
// Symmetry modifier vertex shader
//-----------------------------------------------------------------------------

#include "VsGeneric.hlsl"

uniform float g_fSymmetryOffset;

VSOUT ProcessCustomVertexSYM( VSIN Input, 
                              int  iTransform            = TRANSFORM_NORMAL, 
                              int  iLighting             = LIGHTING_USE_COLOR,
							  int  iDirLightCount		 = 0,
							  int  iOmniLightCount		 = 0,
						      int  iSpotLightCount		 = 0,
                              int  iCylSpotLightCount	 = 0,                              
                              int  iColorSource          = COLORSOURCE_VERTEX_COLOR,
                              int  iBTexTransform        = TEXTRANSFORM_NONE,
                              int  iUVSource             = UVSOURCE_OBJECT,
                              bool bFog                  = FOG_OFF,
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
                              bool bInvertMoss           = INVERTMOSS_OFF,
                              int  iSPG2Mode             = SPG2_MODE_NONE,
                              bool bShadows              = SHADOW_OFF,
                              bool bVertexSpecular       = VERTEX_SPECULAR_OFF,
                              int  iSymmetryMode         = SYMMETRY_MODE_X )
{
    if ( iSymmetryMode == SYMMETRY_MODE_X )
    {
        Input.Position.x = g_fSymmetryOffset - Input.Position.x;

#ifndef VS_NO_NORMAL
        Input.Normal.x = -Input.Normal.x;
#endif
    }
    else if ( iSymmetryMode == SYMMETRY_MODE_Y )
    {
        Input.Position.y = g_fSymmetryOffset - Input.Position.y;

#ifndef VS_NO_NORMAL
        Input.Normal.y = -Input.Normal.y;
#endif
    }
    else
    {
        Input.Position.z = g_fSymmetryOffset - Input.Position.z;

#ifndef VS_NO_NORMAL
        Input.Normal.z = -Input.Normal.z;
#endif
    }

    return ProcessVertex( Input, iTransform, iLighting,
						  iDirLightCount, iOmniLightCount, iSpotLightCount, iCylSpotLightCount,
                          iColorSource, iBTexTransform, iUVSource, bFog,
                          iNTexTransform, iSTexTransform, iDNTexTransform,
                          bAddAmbient, bAddRLI, bRLIScaleAndOffset, iReflectionType,
                          bRimLightEnabled, bRimLightHeightAttenuation, bDepthToColorEnable,
                          bInvertMoss, iSPG2Mode, bShadows, bVertexSpecular );
}
