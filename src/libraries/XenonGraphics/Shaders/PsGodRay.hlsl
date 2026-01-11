//-----------------------------------------------------------------------------
// GodRay pixel shader
//-----------------------------------------------------------------------------
#include "PsCommon.hlsl"

struct PSIN
{
    float4 Position         : POSITION;
    float4 Color 			: COLOR0;
    float2 TexCoord0        : TEXCOORD0;
};

float4 ProcessPixel (	PSIN Input, 
						int iShaderId, 
						int iUnused1, 
						int iUnused2, 
						int iUnused3, 
						int iUnused4, 
						int iUnused5, 
						int iUnused6, 
						int iUnused7)
{
	float4 FinalColor;
	
	if (iShaderId == 0)
	{
		FinalColor	=  float4(0.0, 0.0, 0.0, 0.0);
	}
	else if (iShaderId == 1)
	{
		FinalColor.rgb = tex2D( g_TextureSampler[0], Input.TexCoord0 );
		FinalColor.a = Input.Color.a;
	}
	
	return FinalColor;
}