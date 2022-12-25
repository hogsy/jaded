//-----------------------------------------------------------------------------
// Sprite vertex shader
//-----------------------------------------------------------------------------
#define NO_VSIN
#include "VsCommon.hlsl"

struct VSIN
{
    float4 Position  : POSITION;
    float4 RLI       : COLOR0;
    float2 TexCoord0 : TEXCOORD0;
};

struct VSOUT
{
    float4 Position    : POSITION;
    float4 Color       : COLOR0;
    float3 TexCoordFog : TEXCOORD0;
};

VSOUT ProcessVertex( VSIN _input, 
                     int  _bAddRLI, 
                     int  _bAddAmbient, 
                     int  _bFog, 
                     int  _iUnused4, 
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
    VSOUT Output = (VSOUT)0;

    // Position - Normal transform
    Output.Position = mul( _input.Position, g_mWorldViewProj );

    // Color - Lighting use color, source is diffuse
#ifndef VS_NO_COLOR
    if ( _bAddRLI )
    {
        Output.Color = _input.RLI;
    }

    if ( _bAddAmbient )
    {
        Output.Color.rgb += g_vAmbientColor.rgb;
    }
#endif

    // Texture coordinate
#ifndef VS_NO_TEXCOORD
    Output.TexCoordFog.xy = _input.TexCoord0.xy;
#endif

    // Fog
    if ( _bFog )
    {
        float3 vViewSpacePosition = mul( _input.Position, g_mWorldView );

        float fFog = saturate( (vViewSpacePosition.z - g_fFogNear) / g_fFogDistance );
  
        // Pitch attenuation
        float3 vNormWorldVec = normalize( mul( vViewSpacePosition.xyz, g_mCamera ) );
        float  fLerp     = 1.0f - saturate( ( vNormWorldVec.z - g_vFogPitchAttenuation.x ) / g_vFogPitchAttenuation.y );
        float  fPitchAtt = 1.0f - ( g_fFogPitchAttenuationIntensity * exp( 3.0f * (-fLerp) ) );

        // Compute fog
        Output.TexCoordFog.z = saturate( fFog * g_fFogDensity * fPitchAtt );
    }

    return Output;
};
