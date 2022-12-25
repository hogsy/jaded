// Common pixel shader definitions

#ifndef XE_VS_PSCOMMON
#define XE_VS_PSCOMMON

#include "..\XeSharedDefines.h"

#define DIFFUSE_CUTOFF 0.0f

#ifndef XE_VS_VSOUT
#define XE_VS_VSOUT

struct VSOUT
{
    float4 Color0           : COLOR0_centroid;
    float4 Color1           : COLOR1_centroid;
    float4 TexCoord0        : TEXCOORD0_centroid;
    float4 TexCoord1        : TEXCOORD1_centroid;
    float4 LightDir[3]      : TEXCOORD2_centroid;
    float4 HalfWay[3]       : TEXCOORD5_centroid;
    float2 ScreenPos        : VPOS;
};

#endif

uniform sampler g_TextureSampler[8] : register( s0 );

uniform float4 g_vAmbientColor;
uniform float4 g_vEnvMapColor;
uniform float3 g_fFogColor;
uniform float4 g_avMatDiffuseColor[VS_MAX_LIGHTS_PER_PASS];
uniform float4 g_vConstantColor;
uniform float4 g_avMatSpecularColor[VS_MAX_LIGHTS_PER_PASS];
uniform float4 g_avMossColor;
uniform float4 g_avMossMinusMatDiffuseColor[VS_MAX_LIGHTS_PER_PASS];
uniform float4 g_afRLIBlendingScale;
uniform float4 g_afRLIBlendingOffset;
uniform float4 g_vRimLightColor;
uniform float4 g_vRimLightParams;
uniform float4 g_vResolution;
uniform float4 g_vShadowChannels[4];
uniform float  g_fMossSpecularFactor;
uniform float4 g_vAlphaTestParams;

uniform float4 g_vMixed1;
#define g_fGlobalMul2XFactor g_vMixed1.r
#define g_fSpecularBias		 g_vMixed1.g
#define g_fLocalAlpha        g_vMixed1.b
#define g_fShininess		 g_vMixed1.a

uniform float4 g_vMixed2;
#define g_fDetailNormalMapFactor	g_vMixed2.r
#define g_fOneOverTotalLightCount	g_vMixed2.g
#define g_fAlphaScale				g_vMixed2.b
#define g_fAlphaOffset				g_vMixed2.a

uniform float4 g_vMixed3;
#define g_fGlobalRLIScale			g_vMixed3.r
#define g_fGlobalRLIOffset			g_vMixed3.g
#define g_fShadowCount				g_vMixed3.b
#define g_fMatLODBlend              g_vMixed3.a   

#define g_fRimLightWidthMin         g_vRimLightParams.x
#define g_fRimLightWidthMax         g_vRimLightParams.y
#define g_fRimLightNormalMapRatio   g_vRimLightParams.z
#define g_fRimLightIntensity        g_vRimLightParams.w

float3 SampleNormalMap( sampler2D _sampler, float2 _texCoord )
{
    float3 vNormal = ( tex2D( _sampler, _texCoord ) - 0.5f ) * 2.0f;

    vNormal.z = sqrt( 1.0f - dot( vNormal.xy, vNormal.xy ) );

    return vNormal;
}

#endif
