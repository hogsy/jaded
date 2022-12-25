//-----------------------------------------------------------------------------
// Shadow pixel shader
//-----------------------------------------------------------------------------
//#include "ShaderCommon.hlsl"
#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

uniform float4 g_vCastOnStatic;

sampler2D S_Test : register(s1);

struct PSIN
{
    float4 Position         : POSITION;
    float2 ScreenPos        : VPOS;
};


float4 ProcessPixel (PSIN Input, 
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
    float2 UV = (Input.ScreenPos.xy + 1.125f) / g_vResolution.xy;


    float4 Z1 = tex2D(g_TextureSampler[0], UV);
    float4 Z2 = tex2D(S_Test , UV);
    
	return float4(min(Z1, Z2));

/*
    float S = 1.0f - tex2D(g_TextureSampler[0], UV).r;
    float D = 1.0f - tex2D(S_Test, UV).r;

    float A = max(S, D) * 0.5f;

    if(D.r > 0.0f)
    {
        A += 0.4995f;
    }
    

    else
    {
        if(S.r > 0.0f)
        {
            A += g_vCastOnStatic.r;
        }
    }
*/
/*
    float4 S = 1.0f - tex2D(g_TextureSampler[0], UV);
    float4 D = 1.0f - tex2D(S_Test, UV);

    float4 A = max(S, D) * 0.5f;

    float4 temp1 = (D > 0);
    float4 temp2 = (S > 0);
    A += lerp(lerp(0, g_vCastOnStatic, temp2), 0.4995f, temp1);

    return A;
*/
}
