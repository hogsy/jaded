//-----------------------------------------------------------------------------
// Common pixel shader
//-----------------------------------------------------------------------------

//#define XE_VS_VSOUT

//#include "ShaderCommon.hlsl"
//#include "PsCommon.hlsl"
//#include "..\XeSharedDefines.h"
#include "PsGeneric.hlsl"

/*struct VSOUT
{
    float4 Position         : POSITION;
    float4 Color0           : COLOR0_centroid;
    float4 Color1           : COLOR1_centroid;
    float2 TexCoord0        : TEXCOORD0_centroid;
    float2 TexCoord1        : TEXCOORD1_centroid;
    float4 TexCoord2        : TEXCOORD2_centroid;
    float3 Nrm		        : TEXCOORD3_centroid;
    float3 LightDir         : TEXCOORD4_centroid;
    float3 HalfWay          : TEXCOORD5_centroid;
};*/
/*
float SampleSpecularMap( in VSOUT Input, int iSpecularMapChannel )
{
    if ( iSpecularMapChannel == SPECULARMAPCHANNEL_A )
    {
        return tex2D(g_TextureSampler[2], Input.TexCoord0).a;
    }
    else if ( iSpecularMapChannel == SPECULARMAPCHANNEL_R )
    {
        return tex2D(g_TextureSampler[2], Input.TexCoord0).r;
    }
    else if ( iSpecularMapChannel == SPECULARMAPCHANNEL_G )
    {
        return tex2D(g_TextureSampler[2], Input.TexCoord0).g;
    }
    else
    {
        return tex2D(g_TextureSampler[2], Input.TexCoord0).b;
    }
}
*/
float4 ProcessCustomPixelMAT (	VSOUT Input, 
						bool bLocalAlpha	     = LOCAL_ALPHA_OFF,
						bool bFog			     = FOG_OFF,
						bool bOutputFog          = FOG_OFF,
						bool bUseBaseMap	     = BASEMAP_OFF,
						bool bColor2X			 = COLOR2X_OFF,
						bool bDiffusePPL         = DIFFUSE_PPL_OFF,
						bool bSpecularPPL        = SPECULAR_PPL_OFF,
						bool bUseSpecularMap     = SPECULARMAP_OFF,
						bool bUseDetailNormalMap = DETAILNMAP_OFF,
						int  iSpecularMapChannel = SPECULARMAPCHANNEL_A) : COLOR
{
	if(bDiffusePPL)
		return float4(1,1,1,1);
		
	return float4(1,0,0,1);
	/*
	float4 vOutputColor;
			
	if (!bOutputFog)
	{
		float3 vNormalTangent = Input.HalfWay0;
		
		float3 vNormal = (tex2D( g_TextureSampler[1], Input.TexCoord0 )-0.5)*2.0;
		
		// L dot N
		float fDiffuseDot  = abs(dot( Input.LightDir1, vNormal));
		vOutputColor = float4(fDiffuseDot, fDiffuseDot, fDiffuseDot, fDiffuseDot);

		// H dot N
		float fSpecularIntensity;
		float fSpecularDot  = abs(dot( Input.HalfWay1, vNormal));

		// calculate specular intensity
		if (bUseSpecularMap)
		{
			fSpecularIntensity = SampleSpecularMap(Input, iSpecularMapChannel);
		}
		else
		{
			fSpecularIntensity = 1.0;
		}
		fSpecularIntensity = saturate(fSpecularIntensity + g_fSpecularBias);
				
		float4 vSpecular = pow(fSpecularDot, g_fShininess);	
		
		float4 vBaseMapColor;
		if (bUseBaseMap)
			vBaseMapColor = tex2D( g_TextureSampler[0], Input.TexCoord0 );
		else
			vBaseMapColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

		if (bLocalAlpha)
			vOutputColor.a	= g_fLocalAlpha;
		else
			vOutputColor.a	= Input.Color0.a;	

        float3 vRLI;
        // RLI = GlobalRLI + LocalRLI
        vRLI  = (Input.Color0.rgb * g_fGlobalRLIScale) + g_fGlobalRLIOffset;
        vRLI += (Input.Color0.rgb * g_afRLIBlendingScale[0]) + g_afRLIBlendingOffset[0];
        
		// light influence ((ambient + GlobalRLI + LocalRLI + RLIdiffuse)*BaseMap + specular)
		vOutputColor.rgb = Input.Color1.rgb + vRLI + (vOutputColor.rgb*g_avMatDiffuseColor[0]);
		vOutputColor    *= vBaseMapColor;
		vOutputColor.rgb += (fSpecularIntensity*vSpecular*g_avMatSpecularColor[0]);
							
		if (bColor2X)
			vOutputColor.rgb *= g_fGlobalMul2XFactor;
		
		// saturate final color
		vOutputColor = saturate(vOutputColor);
		
		if (bFog)
		{
			// single pass which only outputs fog color and fog factor
			vOutputColor.rgb = lerp(vOutputColor.rgb, g_fFogColor, Input.LightDir0.w);
		}
	}
	else
	{
        // single pass which only outputs fog color and fog factor
        vOutputColor = float4(g_fFogColor, Input.Color1.w);
	}	
	
	return vOutputColor;*/
}
/*
float4 ProcessPixel (	VSOUT Input, 
						bool bLocalAlpha	     = LOCAL_ALPHA_OFF,
						bool bFog			     = FOG_OFF,
						bool bOutputFog          = FOG_OFF,
						bool bUseBaseMap	     = BASEMAP_OFF,
						bool bColor2X			 = COLOR2X_OFF,
						bool bDiffusePPL         = DIFFUSE_PPL_OFF,
						bool bSpecularPPL        = SPECULAR_PPL_OFF,
						bool bUseSpecularMap     = SPECULARMAP_OFF,
						bool bUseDetailNormalMap = DETAILNMAP_OFF,
						int  iSpecularMapChannel = SPECULARMAPCHANNEL_A) : COLOR
{
	ProcessCustomPixelMAT (	Input, 
						bLocalAlpha,
						bFog,
						bOutputFog,
						bUseBaseMap,
						bColor2X,
						bDiffusePPL,
						bSpecularPPL,
						bUseSpecularMap,
						bUseDetailNormalMap,
						iSpecularMapChannel);
}
*/

/* hogsy: declared under PsGeneric.hlsl, which is included...
// Shader entry points for various configurations of shaders
float4 TestPS( VSOUT Input ) : COLOR
{
	return ProcessPixel(Input,
						LOCAL_ALPHA_OFF,
						FOG_OFF,
						BASEMAP_ON);
}
*/