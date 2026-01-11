//-----------------------------------------------------------------------------
// Sprite pixel shader
//-----------------------------------------------------------------------------

#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

struct PSIN
{
    float4 Color       : COLOR0_centroid;
    float3 TexCoordFog : TEXCOORD0_centroid;
};

float4 ProcessPixel( PSIN _input, 
                     int  _bLocalAlpha, 
                     int  _bUseBaseMap, 
                     int  _bColor2X, 
                     int  _bFog, 
                     int  _iUnused5, 
                     int  _iUnused6, 
                     int  _iUnused7, 
                     int  _iUnused8, 
                     int  _iUnused9, 
                     int  _iUnused10, 
                     int  _iUnused11, 
                     int  _iUnused12, 
                     int  _iUnused13, 
                     int  _iUnused14, 
                     int  _iUnused15 )
{
    float4 vOutputColor = 0.0f;

    if ( _bLocalAlpha )
    {
        vOutputColor.rgb = _input.Color;
        vOutputColor.a   = g_fLocalAlpha;
    }
    else
    {
        vOutputColor = _input.Color;
    }

    if ( _bUseBaseMap )
    {
        vOutputColor *= tex2D( g_TextureSampler[0], _input.TexCoordFog.xy );
    }

    if ( _bColor2X )
    {
        vOutputColor.rgb *= g_fGlobalMul2XFactor;
    }
    
    vOutputColor = saturate(vOutputColor);

    if ( _bFog )
    {
        vOutputColor.rgb = lerp( vOutputColor.rgb, g_fFogColor.rgb, _input.TexCoordFog.z );
    }

    return vOutputColor;
}
