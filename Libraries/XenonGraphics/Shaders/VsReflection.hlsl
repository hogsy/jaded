// Reflection vertex shader

#define NO_VSIN
#include "VsCommon.hlsl"

//-----------------------------------------------------------------------------
// Structures
//-----------------------------------------------------------------------------
struct VSIN
{
    float4 Position       : POSITION;
#ifndef VS_NO_COLOR
    float4 Diffuse        : COLOR0;
#endif
#ifndef VS_NO_TEX_COORD
    float2 BaseTexCoord   : TEXCOORD0;
#endif
};

struct VSOUT
{
    float4 Position       : POSITION;
    float4 TexCoordOffset1: TEXCOORD0;
    float4 TexCoordOffset2: TEXCOORD1;
};

uniform float4   g_vTexOffset0_1;
uniform float4   g_vTexOffset2_3;

#define g_vTexOffset0	g_vTexOffset0_1.xy
#define g_vTexOffset1   g_vTexOffset0_1.zw
#define g_vTexOffset2   g_vTexOffset2_3.xy
#define g_vTexOffset3   g_vTexOffset2_3.zw

VSOUT ProcessBlurVertex(VSIN Input)
{
    VSOUT  Out = (VSOUT)0;

    Out.Position = Input.Position;

#ifndef VS_NO_TEX_COORD
    Out.TexCoordOffset1.xy = Input.BaseTexCoord + g_vTexOffset0;
    Out.TexCoordOffset1.zw = Input.BaseTexCoord + g_vTexOffset1;
    Out.TexCoordOffset2.xy = Input.BaseTexCoord + g_vTexOffset2;
    Out.TexCoordOffset2.zw = Input.BaseTexCoord + g_vTexOffset3;
#endif

    return Out;
}

VSOUT ProcessVertex(VSIN _input,
                    int  _mode, 
                    int  iUnused2, 
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
	return ProcessBlurVertex(_input);
}
