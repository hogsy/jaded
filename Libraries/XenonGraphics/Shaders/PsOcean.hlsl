//-----------------------------------------------------------------------------
// Ocean pixel shader
//-----------------------------------------------------------------------------
#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

uniform float4 g_vOceanParams;

struct PSIN
{
    float4 Position         : POSITION;
    float4 Color0           : COLOR0_centroid;
    float4 TC_Base          : TEXCOORD0_centroid;
    float4 TC_LSPos         : TEXCOORD1_centroid;
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
    float4 Base = tex2D(g_TextureSampler[0], Input.TC_Base);
    
	return lerp(Base, Input.Color0.bgra, Input.Color0.a);
	//return Input.Color0.a;
}
