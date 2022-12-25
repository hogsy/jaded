//-----------------------------------------------------------------------------
// LightShaft Vertex Shader
//-----------------------------------------------------------------------------
#define NO_VSIN
#include "VsCommon.hlsl"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
uniform float4   g_vLightShaftPlaneParams;
uniform float4   g_vLightShaftVolume;
uniform float4x4 g_mLightShaftMatrix;
uniform float4x4 g_mLightShaftNoise1;
uniform float4x4 g_mLightShaftNoise2;
uniform float4x4 g_mLightShaftInvWorldView;
uniform float4x4 g_mLightShaftShadowMatrix;
uniform float4   g_vLightShaftTexOffset0_1;
uniform float4   g_vLightShaftTexOffset2_3;

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define g_fLightShaftRangeFactor        g_vLightShaftPlaneParams.x
#define g_fLightShaftDepthScale         g_vLightShaftPlaneParams.y
#define g_fLightShaftPlaneStartZ        g_vLightShaftPlaneParams.z
#define g_fLightShaftPlaneEndZ          g_vLightShaftPlaneParams.w

#define g_fLightShaftMinX               g_vLightShaftVolume.x
#define g_fLightShaftMinY               g_vLightShaftVolume.y
#define g_fLightShaftMaxX               g_vLightShaftVolume.z
#define g_fLightShaftMaxY               g_vLightShaftVolume.w

#define g_vLightShaftTexOffset0         g_vLightShaftTexOffset0_1.xy
#define g_vLightShaftTexOffset1         g_vLightShaftTexOffset0_1.zw
#define g_vLightShaftTexOffset2         g_vLightShaftTexOffset2_3.xy
#define g_vLightShaftTexOffset3         g_vLightShaftTexOffset2_3.zw

#define g_vLightShaftBlurPosOffset      g_vLightShaftPlaneParams

#define LightShaftMode_Render   0
#define LightShaftMode_Apply    1
#define LightShaftMode_Blur     2
#define LightShaftMode_Resample 3

//-----------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------
struct VSIN
{
    float4 Position       : POSITION;
#ifndef VS_NO_COLOR
    float4 Diffuse        : COLOR0;
#endif
#ifndef VS_NO_TEX_COORD
    float2 BaseTexCoord   : TEXCOORD0;
#endif
};

struct VSOUT
{
    float4 Position       : POSITION;
    float4 Diffuse        : COLOR0;
    float4 CookieTexCoord : TEXCOORD0;
    float4 NoiseTexCoord1 : TEXCOORD1;
    float4 NoiseTexCoord2 : TEXCOORD2;
    float4 ShadowTexCoord : TEXCOORD3;
    float4 DepthTexCoord  : TEXCOORD4;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

VSOUT ProcessApplyVertex(in VSIN _input)
{
    VSOUT  Out = (VSOUT)0;
    float2 vBaseTexCoord;

    Out.Position = _input.Position + g_vLightShaftBlurPosOffset;

#ifndef VS_NO_TEX_COORD
    Out.CookieTexCoord.xy = _input.BaseTexCoord.xy;
#endif

    return Out;
}

VSOUT ProcessRenderVertex(in VSIN _input, int _nbNoiseTex, int _enableShadow, int _enableFog)
{
    VSOUT Out = (VSOUT)0;

    float4 vViewSpacePosition;
    float  fFactor = _input.Position.z * g_fLightShaftRangeFactor;

    vViewSpacePosition.x = _input.Position.x * lerp( g_fLightShaftMinX, g_fLightShaftMaxX, fFactor );
    vViewSpacePosition.y = _input.Position.y * lerp( g_fLightShaftMinY, g_fLightShaftMaxY, fFactor );
    vViewSpacePosition.z = lerp( g_fLightShaftPlaneStartZ, g_fLightShaftPlaneEndZ, fFactor );
    vViewSpacePosition.w = 1.0f;

    Out.Position       = mul( vViewSpacePosition, g_mProjection );
    Out.CookieTexCoord = mul( vViewSpacePosition, g_mLightShaftMatrix );

    if ( _nbNoiseTex >= 1 )
    {
        Out.NoiseTexCoord1 = mul( vViewSpacePosition, g_mLightShaftNoise1 );
    }
    if ( _nbNoiseTex >= 2 )
    {
        Out.NoiseTexCoord2 = mul( vViewSpacePosition, g_mLightShaftNoise2 );
    }

    if ( _enableShadow != 0 )
    {
        Out.ShadowTexCoord = mul( vViewSpacePosition, g_mLightShaftShadowMatrix );
    }

    float4 vPosDepth     = mul( vViewSpacePosition, g_mLightShaftInvWorldView );
    Out.CookieTexCoord.z = vPosDepth.y * g_fLightShaftDepthScale;

    Out.DepthTexCoord = Out.Position;

#ifndef VS_NO_COLOR
    Out.Diffuse = _input.Diffuse;

    if ( _enableFog )
    {
        float fFog           = saturate( (vViewSpacePosition.z - g_vFogParams.x ) / g_vFogParams.w );
        fFog                 = saturate( fFog * g_vFogParams.z );
        Out.NoiseTexCoord1.z = fFog;
    }
#endif

    return Out;
}

VSOUT ProcessBlurVertex(VSIN _input)
{
    VSOUT  Out = (VSOUT)0;

    Out.Position = _input.Position + g_vLightShaftBlurPosOffset;

#ifndef VS_NO_TEX_COORD
    Out.CookieTexCoord.xy = _input.BaseTexCoord + g_vLightShaftTexOffset0;
    Out.NoiseTexCoord1.xy = _input.BaseTexCoord + g_vLightShaftTexOffset1;
    Out.NoiseTexCoord2.xy = _input.BaseTexCoord + g_vLightShaftTexOffset2;
    Out.ShadowTexCoord.xy = _input.BaseTexCoord + g_vLightShaftTexOffset3;
#endif

    return Out;
}

VSOUT ProcessResampleVertex(VSIN _input)
{
    VSOUT  Out = (VSOUT)0;
    float2 vBaseTexCoord;

    Out.Position = _input.Position;

#ifndef VS_NO_TEX_COORD
    Out.CookieTexCoord.xy = _input.BaseTexCoord.xy;
#endif

    return Out;
}

VSOUT ProcessVertex(VSIN _input,
                    int  _mode, 
                    int  _nbNoiseTex, 
                    int  _enableShadow, 
                    int  _enableColor, 
                    int  iUnused5, 
                    int  iUnused6, 
                    int  _enableFog, 
                    int  iUnused8,
					int  iUnused9,
					int  iUnused10,
					int  iUnused11,
					int  iUnused12,
					int  iUnused13,
					int  iUnused14,
					int  iUnused15)
{
    if (_mode == LightShaftMode_Apply)
    {
        return ProcessApplyVertex(_input);
    }
    else if (_mode == LightShaftMode_Blur)
    {
        return ProcessBlurVertex(_input);
    }
    else if (_mode == LightShaftMode_Resample)
    {
        return ProcessResampleVertex(_input);
    }
    else
    {
        return ProcessRenderVertex(_input, _nbNoiseTex, _enableShadow, _enableFog);
    }
}
