//-----------------------------------------------------------------------------
// Shadow vertex shader
//-----------------------------------------------------------------------------
#include "..\XeSharedDefines.h"

struct VSOUT
{
    float4 Position         : POSITION;
    float4 TC_ScreenPos     : TEXCOORD0;
};

#include "VsCommon.hlsl"

uniform float4x4    g_vShadowNearFar;
uniform float4      g_vResolution;

struct VSIN
{
    float4 Position         : POSITION;
#ifndef VS_NO_NORMAL
    float4 Normal			: NORMAL;
#endif
    float4 RLI				: COLOR0;
    float4 BlendWeights		: BLENDWEIGHT;
    int4   BlendIndices		: BLENDINDICES;
#ifndef VS_NO_TEX_COORD
    float2 TexCoord0        : TEXCOORD0;
    float4 Tangent          : TANGENT;
#endif
};

VSOUT ProcessVertex(VSIN Input,
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
    VSOUT Output = (VSOUT)0;

    Output.Position = Input.Position;
    Output.TC_ScreenPos.xyz = Input.Position.xyz;
    Output.TC_ScreenPos.w = 0.5 / Input.Position.w;
    
    return Output;
}