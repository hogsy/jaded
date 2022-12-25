//-----------------------------------------------------------------------------
// Apply shadow pixel shader
//-----------------------------------------------------------------------------
//#include "ShaderCommon.hlsl"
#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

struct PSIN
{
    float4 TC_ScreenPos     : TEXCOORD0;
};

uniform float4   g_vCameraParams; // Near, Far, FOV_x/2, FOV_y/2
uniform float4x4 g_mCamToLightMatrix[3];
uniform float4   g_fShadowColor;
uniform float4   g_vShadowParams[3];

float4 ProcessPixel(PSIN Input,
                    int iNumLights,
                    int iNumIter,           // use g_vShadowParams.x
                    int iUseCookieTexture,
                    int bLocalAlpha,
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
    // Get Z for current pixel from the Z prepass texture
    
    // Calculate UV for Z prepass texture
    float2 ScreenPos;
    ScreenPos.x =  Input.TC_ScreenPos.x;
    ScreenPos.y = -Input.TC_ScreenPos.y;
	ScreenPos.xy *= Input.TC_ScreenPos.w;
	ScreenPos.xy += 0.5f;

    // Fetch Z from prepass texture
    float4 ViewPos;
    ViewPos.z = tex2D(g_TextureSampler[0], ScreenPos).r;
    ViewPos.xy = Input.TC_ScreenPos.xy * ViewPos.z;
    ViewPos.w = 1.0f;

    float3 shadowcolor = 0;
    float4 LightPos;
    float3 Z = 0, Z2 = 0;

    if (iNumLights > 0)
    {
		LightPos = mul(ViewPos, g_mCamToLightMatrix[0]);
		Z2.x = LightPos.z/LightPos.w - g_vShadowParams[0].z;// Add bias
		
		// Scale + Bias to get it in texture space
		LightPos.xy = 0.5f * (LightPos.xy + LightPos.w);
		LightPos.xy /= LightPos.w;
		LightPos.y = 1 - LightPos.y;
		
		Z.x = tex2D(g_TextureSampler[0 + 2], LightPos.xy).r;		
	}

    if (iNumLights > 1)
    {
		LightPos = mul(ViewPos, g_mCamToLightMatrix[1]);
		Z2.y = LightPos.z/LightPos.w - g_vShadowParams[1].z;// Add bias

		// Scale + Bias to get it in texture space
		LightPos.xy = 0.5f * (LightPos.xy + LightPos.w);
		LightPos.xy /= LightPos.w;
		LightPos.y = 1 - LightPos.y;
		
		Z.y = tex2D(g_TextureSampler[1 + 2], LightPos.xy).r;		
	}

    if (iNumLights > 2)
    {
		LightPos = mul(ViewPos, g_mCamToLightMatrix[2]);
		Z2.z = LightPos.z/LightPos.w - g_vShadowParams[2].z;// Add bias
		
		// Scale + Bias to get it in texture space
		LightPos.xy = 0.5f * (LightPos.xy + LightPos.w);
		LightPos.xy /= LightPos.w;
		LightPos.y = 1 - LightPos.y;
		
		Z.z = tex2D(g_TextureSampler[2 + 2], LightPos.xy).r;		
	}
            
    Z2 = min(Z2, 1.0f);
    
    shadowcolor = (Z < Z2) ? g_fShadowColor.xyz : float3(1,1,1);    
    
    return float4(shadowcolor, 0.0f);
}