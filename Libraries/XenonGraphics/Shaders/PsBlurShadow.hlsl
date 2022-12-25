//#include "ShaderCommon.hlsl"
#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

struct PSIN
{ 
    float4 Position         : POSITION;
    float2 TexCoord0        : TEXCOORD0_centroid;    // UV in shadowbuffer
    float2 TexCoord1        : TEXCOORD1_centroid;
    float2 TexCoord2        : TEXCOORD2_centroid;
    float2 TexCoord3        : TEXCOORD3_centroid;
    float2 TexCoord4        : TEXCOORD4_centroid;
    float2 TexCoord5        : TEXCOORD5_centroid;
    float2 TexCoord6        : TEXCOORD6_centroid;
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

//    vAccum = tex2D(g_TextureSampler[0], Input.TexCoord0) * g_afShadowBlurOffsetsAndWeights[0].w;

    vAccum.rgb = tex2D(g_TextureSampler[0], Input.TexCoord1) * g_afShadowBlurOffsetsAndWeights[1].w;
    vAccum.rgb += tex2D(g_TextureSampler[0], Input.TexCoord2) * g_afShadowBlurOffsetsAndWeights[2].w;
    vAccum.rgb += tex2D(g_TextureSampler[0], Input.TexCoord3) * g_afShadowBlurOffsetsAndWeights[3].w;
    
    vAccum.rgb += tex2D(g_TextureSampler[0], Input.TexCoord4) * g_afShadowBlurOffsetsAndWeights[1].w;
    vAccum.rgb += tex2D(g_TextureSampler[0], Input.TexCoord5) * g_afShadowBlurOffsetsAndWeights[2].w;
    vAccum.rgb += tex2D(g_TextureSampler[0], Input.TexCoord6) * g_afShadowBlurOffsetsAndWeights[3].w;

    vAccum.a = g_afShadowBlurOffsetsAndWeights[0].w;
    
    return vAccum;
}