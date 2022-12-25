//-----------------------------------------------------------------------------
// Water Effect pixel shader
//-----------------------------------------------------------------------------
#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

struct PSIN
{
    float4 Color0 			: COLOR0_centroid;
    float4 TexCoord0        : TEXCOORD0_centroid;
    float4 TexCoord1        : TEXCOORD1_centroid;
    float4 Reflected		: TEXCOORD2_centroid;
    float4 LightDir[3]      : TEXCOORD3_centroid;
    float4 TangentSpaceViewer : TEXCOORD6_centroid;
    float4 Position 		: TEXCOORD7_centroid;
};

uniform float4 g_vWaterMixed1;
#define g_fReflectionIntensity g_vWaterMixed1.x
#define g_fRefractionIntensity g_vWaterMixed1.y
#define g_fWaterDensity        g_vWaterMixed1.z
#define g_fBaseMapOpacity      g_vWaterMixed1.w

uniform float4 g_vWaterMixed2;
#define g_fFogIntensity		   g_vWaterMixed2.x

float4 ProcessWaterPixel (	PSIN Input, 
						bool bLightingOnly,
						int  iPerPixelLightCount, 
						bool iEnvMapReflection, 
						bool iRefraction, 
						bool iDynReflection, 
						bool bUseNormalMap, 
						bool bUseDetailNormalMap, 
						bool bColor2X,
                        bool bUseBaseMap,
                        bool bFog,
                        int  iUnused11 = 0,
                        int  iUnused12 = 0,
                        int  iUnused13 = 0,
                        int  iUnused14 = 0,
                        int  iUnused15 = 0)
{
	float4 FinalColor = 0; 
	float4 vRefracted;
					
	// Calculate UV for Z prepass texture
	float2 ScreenPos;
	ScreenPos.x =  Input.Position.x;
	ScreenPos.y = -Input.Position.y;
	ScreenPos *= 0.5f / Input.Position.w;
	ScreenPos += 0.5f;

    // fetch perturbated backbuffer texel to find out if this is a water pixel or an above water pixel
    Input.TexCoord0.xy /= Input.TexCoord0.z;
	vRefracted = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
	Input.TexCoord0.xy = lerp(ScreenPos, Input.TexCoord0.xy, vRefracted.a);
	
	// Fetch Z from prepass texture
	float fDepth;
	fDepth = tex2D(g_TextureSampler[5], Input.TexCoord0.xy).r * Input.TexCoord0.z;
	fDepth -= Input.Position.z;
	fDepth = max(fDepth, 0.0f);
	fDepth *= g_fWaterDensity;
	fDepth = min(fDepth, 1.0f);			
				
	if (!bLightingOnly) 
	{		
		if (iRefraction > 0)
		{
			// Fetch refracted texel
			vRefracted = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
			vRefracted = lerp(g_vConstantColor, vRefracted, g_fRefractionIntensity);

			// interpolate between refraction and environment texel
			vRefracted = lerp(vRefracted, g_vConstantColor, fDepth);
		}
		else
		{
			vRefracted = g_vConstantColor;
		}
		 
		float4 vReflected;
		if ((iDynReflection > 0) || (iEnvMapReflection > 0))
		{
			if (iDynReflection > 0)
			{
				vReflected = tex2D( g_TextureSampler[1], Input.TexCoord0.xy );		
				if (iEnvMapReflection > 0)
				{
					// EnvMap + Dyn reflection
					float3 vEnvReflection;
					vEnvReflection	= texCUBE( g_TextureSampler[2], Input.Reflected );
					vReflected.rgb	= lerp(vReflected.rgb, vEnvReflection, vReflected.a);
					FinalColor.rgb  = lerp(vRefracted, vReflected, g_fReflectionIntensity * fDepth);					
				}
				else
				{
					// Dyn reflection only
					vReflected		= lerp(vReflected, vRefracted, vReflected.a);
					FinalColor.rgb	= lerp(vRefracted, vReflected, g_fReflectionIntensity * fDepth);				
				}					
			}
			else
			{
				// environment reflection			
				// apply reflection intensity
				vReflected = texCUBE( g_TextureSampler[2], Input.Reflected );
				FinalColor.rgb = lerp(vRefracted, vReflected, g_fReflectionIntensity * fDepth);
			}
		}
		else
		{
			FinalColor.rgb = vRefracted;
		}
	}
	
	float3 vNormal, vLightDir, vHalfWay, vViewer, vDetailNormal, vLightingColor = 0;
	float4 vDiffuseDots = 0, vSpecularDots = 0, vAttenuatedDiffuseDots, vAttenuatedSpecularDots;
	if (iPerPixelLightCount > 0)
	{		 
		if (bUseNormalMap)
		{
			// fetch and bias normal vector
			vNormal = SampleNormalMap( g_TextureSampler[3], Input.TexCoord1.zw );
			
			if (bUseDetailNormalMap)
			{
				float2 vTexCoord;

				// SDALLAIRE : fixes arbitrary swizzle 
				// Remove when PS 3.0 is available
				vTexCoord.x = Input.Reflected.w;
				vTexCoord.y = Input.TangentSpaceViewer.w;

				vDetailNormal = SampleNormalMap( g_TextureSampler[4], vTexCoord );
				vNormal       = normalize(vNormal + vDetailNormal);
			}				
		}
		else
		{
			vNormal = float3(0.0f, 0.0f, 1.0f);
		}
		
		vViewer = normalize(Input.TangentSpaceViewer);
		

		// 
		// Compute diffuse and specular dots for all lights
		//
		vLightDir		= normalize(Input.LightDir[0].xyz);
		vDiffuseDots.x	= dot(vLightDir, vNormal);
		vHalfWay		= vViewer + vLightDir;
		vHalfWay		= normalize(vHalfWay);
		vSpecularDots.x = dot(vHalfWay, vNormal);
		
		if (iPerPixelLightCount > 1)
		{
			vLightDir		= normalize(Input.LightDir[1].xyz);
			vDiffuseDots.y	= dot(vLightDir, vNormal);
			vHalfWay		= vViewer + vLightDir;
			vHalfWay		= normalize(vHalfWay);
			vSpecularDots.y = dot(vHalfWay, vNormal);		
		}

		if (iPerPixelLightCount > 2)
		{
			vLightDir		= normalize(Input.LightDir[2].xyz);
			vDiffuseDots.z	= dot(vLightDir, vNormal);
			vHalfWay		= vViewer + vLightDir;
			vHalfWay		= normalize(vHalfWay);
			vSpecularDots.z = dot(vHalfWay, vNormal);		
		}

		if (iPerPixelLightCount > 3)
		{
			vLightDir       = float3(Input.LightDir[0].w, Input.LightDir[1].w, Input.LightDir[2].w);
			vLightDir		= normalize(vLightDir);
			vDiffuseDots.w	= dot(vLightDir, vNormal); 
			vHalfWay		= vViewer + vLightDir;
			vHalfWay		= normalize(vHalfWay);
			vSpecularDots.w = dot(vHalfWay, vNormal);
		}		
		
		vAttenuatedDiffuseDots  = max(vDiffuseDots, 0.0f);
		vAttenuatedDiffuseDots  = Input.Color0 * vAttenuatedDiffuseDots;

        // calculate a 1 or 0 value used as a specular cutoff
        vDiffuseDots = min(vAttenuatedDiffuseDots*1000000.0f, 1.0f); 

		vAttenuatedSpecularDots = max(vSpecularDots, 0.0f);
		vAttenuatedSpecularDots = vDiffuseDots * Input.Color0 * pow(vAttenuatedSpecularDots, g_fShininess);
		
		// Add diffuse and specular contribution
		vLightingColor  = (vAttenuatedDiffuseDots.x * g_avMatDiffuseColor[0]) + (vAttenuatedSpecularDots.x * g_avMatSpecularColor[0]);
   	}

	if (iPerPixelLightCount > 1)
	{
		// Add diffuse and specular contribution
        vLightingColor += (vAttenuatedDiffuseDots.y * g_avMatDiffuseColor[1]) + (vAttenuatedSpecularDots.y * g_avMatSpecularColor[1]);
	}
	
	if (iPerPixelLightCount > 2)
	{
		// Add diffuse and specular contribution
        vLightingColor += (vAttenuatedDiffuseDots.z * g_avMatDiffuseColor[2]) + (vAttenuatedSpecularDots.z * g_avMatSpecularColor[2]);
	}

	if (iPerPixelLightCount > 3) 
	{
		// Add diffuse and specular contribution
        vLightingColor += (vAttenuatedDiffuseDots.w * g_avMatDiffuseColor[3]) + (vAttenuatedSpecularDots.w * g_avMatSpecularColor[3]);
	}

    // add lighting contribution
	if (iPerPixelLightCount > 0)
		FinalColor.rgb += vLightingColor * min(fDepth * 5.0f, 1.0f);
		
	if (bUseBaseMap)
	{	
		float4 vBaseMap = tex2D(g_TextureSampler[6], Input.TexCoord1.xy);
		FinalColor.rgb = lerp(FinalColor.rgb, vBaseMap.rgb, g_fBaseMapOpacity*fDepth*vBaseMap.a);
	}

	FinalColor = saturate(FinalColor);
	
    if( bFog )
    {
		if (bLightingOnly)		
		{
			FinalColor.rgb *= Input.TexCoord0.w;			
		}
		else
		{
			float fFogFactor;
			if (iRefraction > 0)
			{
				// compensate for refraction intensity
				fFogFactor		= lerp(Input.TexCoord0.w, 0.0f, g_fRefractionIntensity);
				fFogFactor		= min(fFogFactor * g_fFogIntensity, 1.0f);		
				FinalColor.rgb	= lerp( FinalColor.rgb, g_fFogColor.rgb, fFogFactor);		
			}
			else
			{
				fFogFactor		= min(Input.TexCoord0.w * g_fFogIntensity, 1.0f);
				FinalColor.rgb	= lerp( FinalColor.rgb, g_fFogColor.rgb, fFogFactor );			
			}
		}
    }
    
	return FinalColor;
}

float4 ProcessWaterMaskPixel(PSIN Input)
{
	return float4(0,0,0,1);
}

float4 ProcessPixel (	PSIN Input, 
						bool bLightingOnly,
						int  iPerPixelLightCount, 
						bool iEnvMapReflection, 
						bool iRefraction, 
						bool iDynReflection, 
						bool bUseNormalMap, 
						bool bUseDetailNormalMap, 
						bool bColor2X,
                        bool bUseBaseMap,
                        bool bFog,
                        int  iShaderId,
                        int  iUnused12,
                        int  iUnused13,
                        int  iUnused14,
                        int  iUnused15)
{
	if (iShaderId == 0)
	{
		return ProcessWaterPixel(Input, bLightingOnly, iPerPixelLightCount, iEnvMapReflection, iRefraction, iDynReflection, bUseNormalMap, bUseDetailNormalMap, bColor2X, bUseBaseMap, bFog);
	}
	
	return ProcessWaterMaskPixel(Input);
}

// Test shader entry points for various configurations of shaders
float4 MainTest( PSIN Input ) : COLOR
{
    return ProcessPixel(Input, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}