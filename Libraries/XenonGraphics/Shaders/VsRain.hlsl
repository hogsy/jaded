//-----------------------------------------------------------------------------
// Rain Vertex Shader
//-----------------------------------------------------------------------------
#define NO_VSIN
#include "VsCommon.hlsl"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
uniform float4   g_vRainTexCoordModifiers;
uniform float4   g_vRainTexCoordPosition;
uniform float4   g_vRainWindVector;
uniform float4   g_vRainBlurTexOffset0_1;
uniform float4   g_vRainBlurTexOffset2_3;
uniform float4   g_vRainBlurPosOffset;
uniform float4x4 g_mRainTexCoordMatrix;

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define g_vRainTextureScale     g_vRainTexCoordModifiers.xy
#define g_vRainTextureOffset    g_vRainTexCoordModifiers.zw

#define g_vRainBlurTexOffset0   g_vRainBlurTexOffset0_1.xy
#define g_vRainBlurTexOffset1   g_vRainBlurTexOffset0_1.zw
#define g_vRainBlurTexOffset2   g_vRainBlurTexOffset2_3.xy
#define g_vRainBlurTexOffset3   g_vRainBlurTexOffset2_3.zw

#define RAIN_VSMODE_NORMAL 0
#define RAIN_VSMODE_BLUR   1
#define RAIN_VSMODE_APPLY  2

//-----------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------
struct VSIN
{
    float4 Position       : POSITION;
    float4 Diffuse        : COLOR0;
#ifndef VS_NO_TEX_COORD
    float2 BaseTexCoord   : TEXCOORD0;
    float2 RainInfo       : TEXCOORD1;
#endif
};

struct VSOUT
{
    float4 Position  : POSITION;
    float4 TexCoord0 : TEXCOORD0;
    float2 TexCoord1 : TEXCOORD1;
    float2 TexCoord2 : TEXCOORD2;
    float2 TexCoord3 : TEXCOORD3;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

VSOUT ProcessRainVertex(VSIN _input, int _dynamic)
{
    VSOUT  Out = (VSOUT)0;
    float4 position;

    if ( _dynamic )
    {
        position = _input.Position + ( _input.RainInfo.x * g_vRainWindVector );

#ifndef VS_NO_TEX_COORD
        Out.TexCoord0.xy = g_vRainTexCoordPosition.xy + ( g_vRainTextureScale * ( _input.BaseTexCoord + g_vRainTextureOffset ) );
        Out.TexCoord0.w  = _input.RainInfo.y;
#endif
    }
    else
    {
        position = _input.Position;

        Out.TexCoord0.xy = mul( position, g_mRainTexCoordMatrix );
        Out.TexCoord0.w  = 1.0f;
    }

    Out.Position = mul( position, g_mWorldViewProj );

    return Out;
}

VSOUT ProcessBlurVertex(VSIN _input)
{
    VSOUT Out = (VSOUT)0;

    Out.Position = _input.Position;

#ifndef VS_NO_TEX_COORD
    Out.TexCoord0.xy = _input.BaseTexCoord + g_vRainBlurTexOffset0;
    Out.TexCoord1    = _input.BaseTexCoord + g_vRainBlurTexOffset1;
    Out.TexCoord2    = _input.BaseTexCoord + g_vRainBlurTexOffset2;
    Out.TexCoord3    = _input.BaseTexCoord + g_vRainBlurTexOffset3;
#endif

    return Out;
}

VSOUT ProcessApplyVertex(VSIN _input)
{
    VSOUT  Out = (VSOUT)0;

    Out.Position  = _input.Position + g_vRainBlurPosOffset;

#ifndef VS_NO_TEX_COORD
    Out.TexCoord0.x = lerp( g_fViewportStartU, g_fViewportEndU, _input.BaseTexCoord.x );
    Out.TexCoord0.y = lerp( g_fViewportStartV, g_fViewportEndV, _input.BaseTexCoord.y );
#endif

    return Out;
}

VSOUT ProcessVertex(VSIN _input,
                    int  _mode, 
                    int  _dynamic, 
                    int  iUnused3, 
                    int  iUnused4, 
                    int  iUnused5, 
                    int  iUnused6, 
                    int  iUnused7,
                    int  iUnused8,
                    int  iUnused9,
                    int  iUnused10,
                    int  iUnused11,
                    int  iUnused12,
                    int  iUnused13,
                    int  iUnused14,
                    int  iUnused15)
{
    if ( _mode == RAIN_VSMODE_APPLY )
    {
        return ProcessApplyVertex( _input );
    }
    else if ( _mode == RAIN_VSMODE_BLUR )
    {
        return ProcessBlurVertex( _input );
    }
    else
    {
        return ProcessRainVertex( _input, _dynamic );
    }
}
