//-----------------------------------------------------------------------------
// Shadow vertex shader
//-----------------------------------------------------------------------------
#include "..\XeSharedDefines.h"

struct VSOUT
{
    float4 Position         : POSITION;
    float2 TC_ScreenPos     : TEXCOORD0;
    float2 TexCoord1        : TEXCOORD1;
    float2 TexCoord2        : TEXCOORD2;
    float2 TexCoord3        : TEXCOORD3;
    float2 TexCoord4        : TEXCOORD4;
    float2 TexCoord5        : TEXCOORD5;
    float2 TexCoord6        : TEXCOORD6;
};

#include "VsCommon.hlsl"


struct VSIN
{
    float4 Position         : POSITION;
    float2 TexCoord0        : TEXCOORD0;
};

uniform float4 g_afShadowBlurOffsetsAndWeights[4];

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
    Output.TC_ScreenPos = Input.TexCoord0;

    Output.TexCoord1 = Input.TexCoord0 + g_afShadowBlurOffsetsAndWeights[1];
    Output.TexCoord2 = Input.TexCoord0 + g_afShadowBlurOffsetsAndWeights[2];
    Output.TexCoord3 = Input.TexCoord0 + g_afShadowBlurOffsetsAndWeights[3];
    
    Output.TexCoord4 = Input.TexCoord0 - g_afShadowBlurOffsetsAndWeights[1];
    Output.TexCoord5 = Input.TexCoord0 - g_afShadowBlurOffsetsAndWeights[2];
    Output.TexCoord6 = Input.TexCoord0 - g_afShadowBlurOffsetsAndWeights[3];
    
    return Output;
}