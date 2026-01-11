//-----------------------------------------------------------------------------
// Apply shadow pixel shader
//-----------------------------------------------------------------------------
//#include "ShaderCommon.hlsl"
#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

struct PSIN
{ 
    float4 Color0           : COLOR0_centroid;
    float4 TC_Base          : TEXCOORD0_centroid;    // UV in shadowbuffer
    float4 TC_LSPos         : TEXCOORD1_centroid;    // Interpolated Z wrt light
    float4 TC_Cookie        : TEXCOORD2_centroid;    // Cookie UVs
    float4 TC_TEST          : TEXCOORD3_centroid;
    float2 ScreenPos        : VPOS;
};

uniform float4 g_vShadowColor;
uniform float4 g_vShadowParams;

struct PS_OUT
{
    float4 Color[2] : COLOR0;
}; 

PS_OUT ProcessPixel(PSIN Input, 
                    int iShaderId, 
                    int iNumIter, 
                    int iUseCookieTexture, 
                    int bLocalAlpha, 
                    int iUnused5, 
                    int iUnused6, 
                    int iUnused7, 
                    int iUnused8)
{
    PS_OUT Out;

    if(g_vAlphaTestParams.x)       
    {
		// Do our own alpha test because rendering to R32F disables alpha operations
		// Alpha testing
		float fAlpha = tex2D(g_TextureSampler[3], Input.TC_TEST).a;

        if (bLocalAlpha)
        {
            fAlpha *= g_fLocalAlpha;
        }
        else
        {
            fAlpha *= Input.Color0.a;
        }

        // Alpha test is enabled        
        float fDelta = fAlpha - g_vAlphaTestParams.y;
        fDelta *= g_vAlphaTestParams.z;	// Apply alpha inversion
        clip(fDelta);
    }

    float Z = 0;
    float Z2 = Input.TC_LSPos.z / Input.TC_LSPos.w;
    Z2 -= g_vShadowParams.z;

    float  fsize = Input.TC_Base.w * g_vShadowParams.y;
    float4 smcoord = {0, 0, Input.TC_Base.zw};
    float4 ScreenPos;
    ScreenPos = float4(Input.ScreenPos, 0, 0);

    float4 jcoord = float4(ScreenPos.xy * g_vShadowParams.w, 0, 0);
    float4 jitter;
    float shadowcolor = 0;
    float shadowsample;

    // Perform 4 test samples (2 iterations)
    for(int i = 0; i < 2; i++)
    {
        jitter = (2 * tex3D(g_TextureSampler[1], jcoord) - 1.0);
        jcoord.z += g_vShadowParams.w;
        
        smcoord.xy = jitter.xy * fsize + Input.TC_Base.xy;
        Z = tex2Dproj(g_TextureSampler[0], smcoord);
        shadowcolor += (Z < Z2) ? 0 : 1;
        
        smcoord.xy = jitter.zw * fsize + Input.TC_Base.xy;
        Z = tex2Dproj(g_TextureSampler[0], smcoord);
        shadowcolor += (Z < Z2) ? 0 : 1;
    }

    if( (shadowcolor > 0) && (shadowcolor < 4) )
    {
        // Not sure if pixel is in shadow or not, do remaining iterations
        for(int i = 0; i < (iNumIter-2); i++)
        {
            jitter = (2 * tex3D(g_TextureSampler[1], jcoord) - 1.0);
            jcoord.z += g_vShadowParams.w;
            
            smcoord.xy = jitter.xy * fsize + Input.TC_Base.xy;
            Z = tex2Dproj(g_TextureSampler[0  ], smcoord);
            shadowcolor += (Z < Z2) ? 0 : 1;
            
            smcoord.xy = jitter.zw * fsize + Input.TC_Base.xy;
            Z = tex2Dproj(g_TextureSampler[0], smcoord);
            shadowcolor += (Z < Z2) ? 0 : 1;
        }

        shadowcolor /= float(2 * iNumIter);
    }
    else
    {
        shadowcolor /= 4.0f;
    }
    
    if(iUseCookieTexture)
    {
        // Sample cookie texture
        float3 cookie = tex2Dproj(g_TextureSampler[2], Input.TC_Cookie).rgb;
        
        // modulate with cookie color intensity
        shadowcolor *= dot(cookie, float3(0.3, 0.11, 0.59));
    }

    shadowcolor += g_vShadowColor.w;
    Out.Color[0] = float4(shadowcolor, shadowcolor, shadowcolor, shadowcolor);
    Out.Color[1] = float4(Z, Z, Z, 1.0f);

    return Out;
}