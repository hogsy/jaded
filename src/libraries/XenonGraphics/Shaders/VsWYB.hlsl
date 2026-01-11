//-----------------------------------------------------------------------------
// Common vertex shader
//-----------------------------------------------------------------------------
#include "VsFur.hlsl"

uniform float4 g_vWYB_Axes[2];
uniform float4 g_vWYB_Conditions[2];
uniform float4 g_vWYB_Values[2];

VSOUT ProcessCustomVertexWYB( VSIN Input,
                              int  iNumWYB               = 1,
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
                              bool bVertexSpecular	     = VERTEX_SPECULAR_OFF)
{
    // Do WYB manipulation on input vertex
    float4 Pos = Input.Position;
    float Alpha;

    for(int i = 0; i < iNumWYB; i++)
    {
        if(g_vWYB_Conditions[i].y)
        {    
            // Use alpha of RLI
            if(g_vWYB_Conditions[i].z)
            {
                // Invert alpha
                Alpha = 1 - Input.RLI.a;
            }
            else
            {
                Alpha = Input.RLI.a;
            }
        }
        else
        {
            Alpha = 1;
        }
        
        float f = g_vWYB_Values[i].y * Alpha;
        
        if(g_vWYB_Axes[i].x)
        {
            if(g_vWYB_Conditions[i].x)
            {
                // Planar
                Pos.x += f * sin((Pos.y * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
			    Pos.x += f * sin((Pos.z * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
                
            }
            else
            {
                // Not planar
                Pos.y += f * sin((Pos.x * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
			    Pos.z += f * cos((Pos.x * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
            }
        }
        else if(g_vWYB_Axes[i].y)
        {
            if(g_vWYB_Conditions[i].x)
            {
                // Planar
                Pos.y += f * sin((Pos.x * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
			    Pos.y += f * sin((Pos.z * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
                
            }
            else
            {
                // Not planar
                Pos.x += f * sin((Pos.y * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
			    Pos.z += f * cos((Pos.y * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
		    }
        }
        else if(g_vWYB_Axes[i].z)
        {
            if(g_vWYB_Conditions[i].x)
            {
                // Planar
                Pos.z += f * sin((Pos.x * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
			    Pos.z += f * sin((Pos.y * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
                
            }
            else
            {
                // Not planar
                Pos.x += f * sin((Pos.z * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
			    Pos.y += f * cos((Pos.z * g_vWYB_Values[i].z) + g_vWYB_Values[i].x);
		    }
        }
    }
    
    Input.Position = Pos;

    // Check for fur modifier
    if(g_vWYB_Values[0].w == 0)
    {
        return ProcessVertex(Input, iTransform, iLighting, iDirLightCount, iOmniLightCount, iSpotLightCount, iCylSpotLightCount, iColorSource, iBTexTransform, iUVSource, bFog, iNTexTransform, iSTexTransform, iDNTexTransform, bAddAmbient, bAddRLI, bRLIScaleAndOffset, iReflectionType, bRimLightEnabled, bRimLightHeightAttenuation, bDepthToColorEnable, bInvertMoss, iSPG2Mode, bShadows, bVertexSpecular);
    }
    else
    {
        // Fur modifier was detected after WYB
        // Call the fur shader
        return ProcessCustomVertexMAT(Input, iTransform, iLighting, iDirLightCount, iOmniLightCount, iSpotLightCount, iCylSpotLightCount, iColorSource, iBTexTransform, iUVSource, bFog, iNTexTransform, iSTexTransform, iDNTexTransform, bAddAmbient, bAddRLI, bRLIScaleAndOffset, iReflectionType, bRimLightEnabled, bRimLightHeightAttenuation, bDepthToColorEnable, bInvertMoss, iSPG2Mode, bShadows, bVertexSpecular);
    }
}
