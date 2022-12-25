//-----------------------------------------------------------------------------
// Common pixel shader
//-----------------------------------------------------------------------------
//#include "ShaderCommon.hlsl"
#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

uniform float4  g_vHeatShimmerScale;

struct PSIN
{ 
    float4 Color0           : COLOR0_centroid;
    float4 TexCoord0        : TEXCOORD0_centroid;    // UV in shadowbuffer
    float4 TexCoord1        : TEXCOORD1_centroid;
};

struct PS_OUT
{
    float4  Color : COLOR;
    float   Depth : DEPTH;
};

PS_OUT ProcessPixel_AlphaMask( PSIN Input )
{
    PS_OUT Output = (PS_OUT) 0;
    Output.Color = float4( 1,1,1, Input.Color0.a );
    Output.Depth = Input.Color0.a;
    return Output;
}


float4 ProcessPixel (	PSIN Input, 
						int iPassType = HEAT_SHIMMER_MASK_PASS,
						int iAlphaType  = HEAT_SHIMMER_ALPHA_FROM_TEXTURE, 
						int iDebug  = 0, 
						int iUnused4  = 0, 
						int iUnused5  = 0, 
						int iUnused6  = 0, 
						int iUnused7  = 0,
						int iUnused8  = 0,						
						int iUnused9  = 0,
						int iUnused10 = 0,
						int iUnused11 = 0,
						int iUnused12 = 0,
						int iUnused13 = 0,
						int iUnused14 = 0,
						int iUnused15 = 0) : COLOR
{
	float4 vOutputColor;
	float4 vOffset;
	float4 vMaskOrig;
	float4 vMaskOffset;
	
	if( iPassType == HEAT_SHIMMER_MASK_PASS )
	{
	    vOutputColor = Input.Color0;
	}
	else if( iPassType == HEAT_SHIMMER_OFFSET_PASS )
	{
	    if( iDebug == 1 )
	    {
       	    vMaskOrig = tex2D( g_TextureSampler[2], Input.TexCoord1.xy );
	        return float4(1,1,1, vMaskOrig.a );
	    }
	
	    // Kill if we are over a masked out region (alpha needs to be almost 1.0 to be considered)
	    vMaskOrig = tex2D( g_TextureSampler[2], Input.TexCoord1.xy );
	    clip( vMaskOrig.a - 0.99f );
	    
	    // Sample dudv in noise texture
	    vOffset = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
    
        // Scale the offset
        vOffset = g_vHeatShimmerScale * vOffset;
    
        // Kill if the offset brings us over a masked out region
	    vMaskOffset = tex2D( g_TextureSampler[2], Input.TexCoord1.xy + vOffset );
	    clip( vMaskOffset.a - 0.99f );
         
	    // Now sample with offset
	    vOutputColor.rgb = tex2D( g_TextureSampler[1], Input.TexCoord1.xy + vOffset );
	    
        if( iAlphaType == HEAT_SHIMMER_ALPHA_FROM_TEXTURE )
        {
	        float4 vAlphaMask = tex2D( g_TextureSampler[1], Input.TexCoord1.xy );
	        float4 vSPG2Alpha = tex2D( g_TextureSampler[4], Input.TexCoord1.xy );
	        
	        vOutputColor.a = vAlphaMask.a;
	        float fSPG2Alpha = smoothstep(0.75f, 1.0f, 1.0f-vSPG2Alpha.a);
	        vOutputColor.a *= fSPG2Alpha;
	    }
	    else
	    {
	        vOutputColor.a = Input.Color0.a;
	    }
	    
	    if( iDebug == 2 )
	    {
	        vOutputColor.rgb = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
	    }
    }
    else if( iPassType == 3 )
    {
        vOutputColor = Input.Color0.a;        
    }
    else
    {
        float4 vBase = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
        //float4 vSPG2Alpha = tex2D( g_TextureSampler[1], Input.TexCoord0.xy );
        
        vOutputColor = vBase; // - (vSPG2Alpha.a*float4(1,1,1,1));
    }
    
    return vOutputColor;
}


// Shader entry points for various configurations of shaders
float4 TestPS( PSIN Input ) : COLOR
{
	return ProcessPixel(Input, 1 );
}