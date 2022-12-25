#include "..\XeSharedDefines.h"
#include "PsCommon.hlsl"

#define REFLECTION_INTENSITY	0.2f

struct PSIN
{
    float4 Color0      : COLOR0_centroid;
    float4 TexCoord0   : TEXCOORD0_centroid;
    float4 TexCoord1   : TEXCOORD1_centroid;
};

float4 ProcessReflectionPixel(   PSIN Input,
								 bool bUseBaseMap         = false,
								 bool bLocalAlpha         = false,
								 bool bRemapAlphaRange    = false,
								 bool bAlphaTest          = false,
								 bool bColor2X			  = false) : COLOR
{
	float4 vOutputColor;
	
	vOutputColor.rgb = float3(REFLECTION_INTENSITY, REFLECTION_INTENSITY, REFLECTION_INTENSITY);
	
	if (bLocalAlpha)
    {
        vOutputColor.a = g_fLocalAlpha;
    }
    else if (bRemapAlphaRange)
	{
		vOutputColor.a = saturate((Input.Color0.a * g_fAlphaScale) + g_fAlphaOffset);
	}
	else
	{
		vOutputColor.a = Input.Color0.a;	
	}
    
	if (bUseBaseMap)
	{    	
    	vOutputColor *= tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
    }
    
    if (bAlphaTest)
    { 
		// Alpha test is enabled        
		float fDelta = vOutputColor.a - g_vAlphaTestParams.y;
		fDelta *= g_vAlphaTestParams.z;	// Apply alpha inversion
		clip(fDelta);         		
    }
        
	// modulate with material diffuse
	vOutputColor.rgb *= g_avMatDiffuseColor[0].rgb;	

    if (bColor2X)
    {
        vOutputColor.rgb *= g_fGlobalMul2XFactor;
    }
    
    // saturate final color
    vOutputColor = saturate(vOutputColor);
    
    // force alpha to black to mark the reflection buffer
    vOutputColor.a = 0;
 	
    return vOutputColor;
}

float4 ProcessBlurPixel(PSIN Input)
{
    float4 vColor;

    vColor  = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
    vColor += tex2D( g_TextureSampler[0], Input.TexCoord0.zw );
    vColor += tex2D( g_TextureSampler[0], Input.TexCoord1.xy );
    vColor += tex2D( g_TextureSampler[0], Input.TexCoord1.zw );

    return vColor * 0.25f;
    
    //return float4(1,0,0,0);
}

float4 ProcessPixel (   PSIN Input,
						int  iShaderId			 = 0,
                        bool bUseBaseMap         = false,
                        bool bLocalAlpha         = false,
						bool bRemapAlphaRange    = false,
                        bool bAlphaTest          = false,
                        bool bColor2X			 = false,
                        int iUnused7			 = 0,
                        int iUnused8			 = 0,
                        int iUnused9			 = 0,
                        int iUnused10			 = 0,
                        int iUnused11			 = 0,
                        int iUnused12			 = 0,
                        int iUnused13			 = 0,
                        int iUnused14			 = 0,
                        int iUnused15			 = 0) : COLOR
{
	if (iShaderId == 0)
	{
		return ProcessReflectionPixel(Input, bUseBaseMap, bLocalAlpha, bRemapAlphaRange, bAlphaTest, bColor2X);
	}
	else if (iShaderId == 1)
	{
		return ProcessBlurPixel(Input);
	}
}