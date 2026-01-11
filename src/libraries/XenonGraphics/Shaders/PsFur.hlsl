
#include "PsCommon.hlsl"

struct PSIN
{
    float4 Color0           : COLOR0_centroid;
    float4 Color1           : COLOR1_centroid;
    float4 TexCoord0        : TEXCOORD0_centroid;
    float4 TexCoord1        : TEXCOORD1_centroid;
    float4 LightDir[3]      : TEXCOORD2_centroid;
    float4 HalfWay[3]       : TEXCOORD5_centroid;
    float2 ScreenPos        : VPOS;
};

float4 ProcessPixel (	PSIN Input, 
						bool bNormalOffset,
						int iLightCount, 
						bool bLocalAlpha, 
						bool bRemapAlphaRange, 
						bool bUseBaseMap, 
						bool bUseNormalMap, 
						bool bAddAmbient, 
						bool bFog,
						int iUnused9 = 0,
						int iUnused10 = 0,
						int iUnused11 = 0,
						int iUnused12 = 0,
						int iUnused13 = 0,
						int iUnused14 = 0,
						int iUnused15 = 0)
{
	float4 vOutputColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (bLocalAlpha)
    {
        vOutputColor.a  = g_fLocalAlpha;
    }
    else
    {
		if (iLightCount > 0)
			vOutputColor.a = Input.Color1.a;
		else
			vOutputColor.a  = Input.Color0.a;
    }

	float4 vBaseMapColor;
	if (bUseBaseMap)
	{
    	vBaseMapColor = tex2D( g_TextureSampler[0], Input.TexCoord0.xy);
	}

	// Lighting
	float3 vNormal, vDiffuse = 0, vSpecular = 0;
	float4 vDiffuseDots = 0, vAttenuatedDiffuseDots = 0, vAttenuatedSpecularDots = 0;
	if (iLightCount > 0) 
	{
        if (bUseNormalMap)
        {
			// fetch and bias normal vector
			vNormal = SampleNormalMap( g_TextureSampler[1], Input.TexCoord1.xy );        
        } 
        {
			vNormal = float3(0.0, 0.0, 1.0);
        }

		if (bNormalOffset)
		{
			vNormal.xy += Input.TexCoord1.zw;
			vNormal = normalize(vNormal);
		}	
	
		// 
		// Compute diffuse dots for all lights
		//
		float3 vTemp3;
						      vDiffuseDots.x = dot(normalize(Input.LightDir[0].xyz), vNormal);
		if( iLightCount > 1 ) vDiffuseDots.y = dot(normalize(Input.LightDir[1].xyz), vNormal);
		if( iLightCount > 2 ) vDiffuseDots.z = dot(normalize(Input.LightDir[2].xyz), vNormal);
		if( iLightCount > 3 )
		{
			vTemp3 = float3(Input.LightDir[0].w, Input.LightDir[1].w, Input.LightDir[2].w);
			vDiffuseDots.w = dot(normalize(vTemp3.xyz), vNormal);
		}
		vAttenuatedDiffuseDots = Input.Color0 * vDiffuseDots;

		// 
		// Compute specular dots for all lights
		//
							  vAttenuatedSpecularDots.x = dot(normalize(Input.HalfWay[0].xyz), vNormal);
		if( iLightCount > 1 ) vAttenuatedSpecularDots.y = dot(normalize(Input.HalfWay[1].xyz), vNormal);
		if( iLightCount > 2 ) vAttenuatedSpecularDots.z = dot(normalize(Input.HalfWay[2].xyz), vNormal);
		if( iLightCount > 3 )
		{
			vTemp3 = float3(Input.HalfWay[0].w, Input.HalfWay[1].w, Input.HalfWay[2].w);
			vAttenuatedSpecularDots.w = dot(normalize(vTemp3.xyz), vNormal);
		}
	        
		vAttenuatedSpecularDots = max(vAttenuatedSpecularDots, 0.0f);
		vAttenuatedSpecularDots = Input.Color0 * pow(vAttenuatedSpecularDots, g_fShininess);

		if (bAddAmbient)
			vDiffuse += g_vAmbientColor;

		if (vDiffuseDots.x >= 0.0f)
		{
			vDiffuse  += (vAttenuatedDiffuseDots.x * g_avMatDiffuseColor[0].rgb);
			vSpecular += (vAttenuatedSpecularDots.x * g_avMatSpecularColor[0].rgb);
		}
	}
	else
	{
		vOutputColor.rgb = Input.Color0.rgb;
		
		if (bAddAmbient)
			vOutputColor.rgb += g_vAmbientColor;

		if (bUseBaseMap)
			vOutputColor *= vBaseMapColor;
	}
	
	if (iLightCount > 1)
	{   
		if (vDiffuseDots.y >= 0.0f)
		{
			vDiffuse  += (vAttenuatedDiffuseDots.y * g_avMatDiffuseColor[1].rgb);
			vSpecular += (vAttenuatedSpecularDots.y * g_avMatSpecularColor[1].rgb);
		}
	}

	if (iLightCount > 2)
	{   
		if (vDiffuseDots.z >= 0.0f)
		{
			vDiffuse  += (vAttenuatedDiffuseDots.z * g_avMatDiffuseColor[2].rgb);
			vSpecular += (vAttenuatedSpecularDots.z * g_avMatSpecularColor[2].rgb);
		}
	}

	if (iLightCount > 3)
	{   
		if (vDiffuseDots.w >= 0.0f)
		{
			vDiffuse  += (vAttenuatedDiffuseDots.w * g_avMatDiffuseColor[3].rgb);
			vSpecular += (vAttenuatedSpecularDots.w * g_avMatSpecularColor[3].rgb);
		}
	}
	
	if (iLightCount > 0)
	{
		if (bUseBaseMap)
			vOutputColor.rgb = (vDiffuse * vBaseMapColor) + vSpecular;
		else
			vOutputColor.rgb = vDiffuse + vSpecular;

		if (bUseBaseMap)
			vOutputColor.a *= vBaseMapColor.a;		
	}	

	vOutputColor = saturate(vOutputColor);
	
    if( bFog )
    {
        vOutputColor.rgb = lerp( vOutputColor.rgb, g_fFogColor.rgb, Input.TexCoord0.z );
    }

	return vOutputColor;
}

float4 TestPS(	PSIN Input ) : COLOR
{
	return ProcessPixel(Input, true, 1, false, false, true, true, true, false);
}