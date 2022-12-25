//#include "ShaderCommon.hlsl"
#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

struct PSIN
{ 
    float4 Position         : POSITION;
    float2 TexCoord0        : TEXCOORD0;    // UV in shadowbuffer
    float2 TexCoord1        : TEXCOORD1;
    float2 TexCoord2        : TEXCOORD2;
    float2 TexCoord3        : TEXCOORD3;
    float2 TexCoord4        : TEXCOORD4;
    float2 TexCoord5        : TEXCOORD5;
    float2 TexCoord6        : TEXCOORD6;
};

uniform float4 g_afShadowBlurOffsetsAndWeights[4];
uniform float4 g_vShadowParams;

float4 ProcessPixel(PSIN Input, 
                    int iUnused1, 
                    int iUnused2, 
                    int iUnused3, 
                    int iUnused4, 
                    int iUnused5, 
                    int iUnused6, 
                    int iUnused7, 
                    int iUnused8,
                    int iUnused9,
                    int iUnused10,
                    int iUnused11,
                    int iUnused12,
                    int iUnused13,
                    int iUnused14,
                    int iUnused15)
{
    float4 vAccum = 0;

    vAccum = tex2D(g_TextureSampler[0], Input.TexCoord0) * g_afShadowBlurOffsetsAndWeights[0].w;

    vAccum += tex2D(g_TextureSampler[0], Input.TexCoord0 + g_afShadowBlurOffsetsAndWeights[1].xy) * g_afShadowBlurOffsetsAndWeights[1].w;
    vAccum += tex2D(g_TextureSampler[0], Input.TexCoord0 + g_afShadowBlurOffsetsAndWeights[2].xy) * g_afShadowBlurOffsetsAndWeights[2].w;
    vAccum += tex2D(g_TextureSampler[0], Input.TexCoord0 + g_afShadowBlurOffsetsAndWeights[3].xy) * g_afShadowBlurOffsetsAndWeights[3].w;
    
    vAccum += tex2D(g_TextureSampler[0], Input.TexCoord0 - g_afShadowBlurOffsetsAndWeights[1].xy) * g_afShadowBlurOffsetsAndWeights[1].w;
    vAccum += tex2D(g_TextureSampler[0], Input.TexCoord0 - g_afShadowBlurOffsetsAndWeights[2].xy) * g_afShadowBlurOffsetsAndWeights[2].w;
    vAccum += tex2D(g_TextureSampler[0], Input.TexCoord0 - g_afShadowBlurOffsetsAndWeights[3].xy) * g_afShadowBlurOffsetsAndWeights[3].w;
    
    return vAccum;
}