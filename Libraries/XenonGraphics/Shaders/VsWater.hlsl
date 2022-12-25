//-----------------------------------------------------------------------------
// Water Effect vertex shader
//-----------------------------------------------------------------------------

#include "..\XeSharedDefines.h"

struct VSIN
{
    float4 Position         : POSITION;
    float4 Normal			: NORMAL;
    float4 Tangent			: TANGENT;
    float2 TexCoord0		: TEXCOORD0;
};

struct VSOUT
{
    float4 Position         : POSITION;
    float4 Color0			: COLOR0;
    float4 TexCoord0        : TEXCOORD0;
    float4 TexCoord1		: TEXCOORD1;
    float4 Reflected        : TEXCOORD2;
    float4 LightDir[3]		: TEXCOORD3;
    float4 TangentSpaceViewer : TEXCOORD6;
    float4 ScreenPosition	: TEXCOORD7;
};

#undef VS_NO_TEX_COORD
#undef VS_NO_NORMAL
#include "VsCommon.hlsl"

// refraction params
#define u1			0.6f
#define u2			1.0f
#define u			u1 / u2
#define uSrq		u*u
#define fMaxDu		48.0f / 640.0f
#define fMaxDv		8.5f / 480.0f
#define fZOffset	1.0f
#define fMaxZ		3.0f
#define fBias		0.0f;

// reflection params
#define REFLECTION_DAMPING 0.95f
uniform float3 g_vWorldSpacePlaneNormal;

TangentSpaceBasis ComputeWaterTangentSpaceBasis(in VSIN Input)
{
	TangentSpaceBasis Basis;
	
	Basis.Normal   = Input.Normal.xyz;	
	Basis.Tangent  = Input.Tangent.xyz;
	Basis.BiNormal = cross(Basis.Tangent, Basis.Normal);
	
	return Basis;
}

float ComputePPLFromDirLight(in int iCurLightIndex, in TangentSpaceBasis Basis, inout VSOUT Output)
{
	// transform direction to tangent space
	if (iCurLightIndex < 3)
	{
		Output.LightDir[iCurLightIndex].x = dot(Basis.Tangent,  g_aoLights[iCurLightIndex].Direction);
		Output.LightDir[iCurLightIndex].y = dot(Basis.BiNormal, g_aoLights[iCurLightIndex].Direction);
		Output.LightDir[iCurLightIndex].z = dot(Basis.Normal,   g_aoLights[iCurLightIndex].Direction);
	}
	else
	{
		Output.LightDir[0].w = dot(Basis.Tangent,  g_aoLights[iCurLightIndex].Direction);
		Output.LightDir[1].w = dot(Basis.BiNormal, g_aoLights[iCurLightIndex].Direction);
		Output.LightDir[2].w = dot(Basis.Normal,   g_aoLights[iCurLightIndex].Direction);	
	}

	return 1.0f;
}

float ComputePPLFromOmniLight(in int iCurLightIndex, in TangentSpaceBasis Basis, in float4 vModelSpacePosition, inout VSOUT Output)
{
	float2 vFactors;
	
	// find light direction
	float3 vDirection;
	
	vDirection = g_aoLights[iCurLightIndex].Position.xyz - vModelSpacePosition.xyz;

	// apply inversion if necessary
	vDirection *= g_aoLights[iCurLightIndex].Position.w;
	
	// normalize direction
	vFactors.x = length(vDirection);

	// patch for bad constant behaviour : to be investigated
	if(vFactors.x < 0.001)
		vFactors.x = 1.0;
		
	vDirection = vDirection / vFactors.x;
		
	// calculate attenuation ((far - dist) / (far - near))
	vFactors.y = (g_aoLights[iCurLightIndex].Params.x - vFactors.x) * g_aoLights[iCurLightIndex].Params.y;
	
	// saturate all factors
	vFactors = saturate(vFactors);
		
	if (iCurLightIndex < 3)
	{
		// transform direction to tangent space
		Output.LightDir[iCurLightIndex].x = dot(Basis.Tangent,  vDirection);
		Output.LightDir[iCurLightIndex].y = dot(Basis.BiNormal, vDirection);
		Output.LightDir[iCurLightIndex].z = dot(Basis.Normal,   vDirection); 
	}
	else
	{
		// transform direction to tangent space
		Output.LightDir[0].w = dot(Basis.Tangent,  vDirection);
		Output.LightDir[1].w = dot(Basis.BiNormal, vDirection);
		Output.LightDir[2].w = dot(Basis.Normal,   vDirection); 		
	}
	
	return vFactors.y;
}

float ComputePPLFromSpotLight(in int iCurLightIndex, in TangentSpaceBasis Basis, in float4 vModelSpacePosition, inout VSOUT Output, in bool bCylindricalAttenuation)
{
	float3 vFactors;
	float3 vDirection, vPosToLight;
	
	// find light direction
	vPosToLight = g_aoLights[iCurLightIndex].Position.xyz - vModelSpacePosition.xyz;
	
	// normalize direction
	vFactors.x = length(vPosToLight);
		
	// calculate attenuation ((far - dist) / (far - near))
	vFactors.y = (g_aoLights[iCurLightIndex].Params.x - vFactors.x) * g_aoLights[iCurLightIndex].Params.y;
	
	if (!bCylindricalAttenuation)
	{
		vDirection = vPosToLight / vFactors.x;
	
		// apply spotlight cone ((cos(alpha) - outer) / (inner - outer))
		vFactors.z = (dot(g_aoLights[iCurLightIndex].Direction, vDirection) - g_aoLights[iCurLightIndex].Params.z) * g_aoLights[iCurLightIndex].Params.w;	
	}
	else
	{
		vDirection = g_aoLights[iCurLightIndex].Direction;
		
		// apply cylinder (1 - ((d - inner) / (outer - inner)))
		float fDot = dot(g_aoLights[iCurLightIndex].Direction, -vPosToLight);
		vFactors.z = 0.0f;
		if (fDot < 0.0f)
		{
			float d = length(-vPosToLight - (fDot * vDirection));
			vFactors.z = 1.0f - saturate((d - g_aoLights[iCurLightIndex].Params.z) * g_aoLights[iCurLightIndex].Params.w);		
		}
	}

	// saturate all factors
	vFactors = saturate(vFactors);
	
	if (iCurLightIndex < 3)
	{
		// transform direction to tangent space
		Output.LightDir[iCurLightIndex].x = dot(Basis.Tangent,  vDirection);
		Output.LightDir[iCurLightIndex].y = dot(Basis.BiNormal, vDirection);
		Output.LightDir[iCurLightIndex].z = dot(Basis.Normal,   vDirection);
	}
	else
	{
		// transform direction to tangent space
		Output.LightDir[0].w = dot(Basis.Tangent,  vDirection);
		Output.LightDir[1].w = dot(Basis.BiNormal, vDirection);
		Output.LightDir[2].w = dot(Basis.Normal,   vDirection);	
	}
	
	return vFactors.y * vFactors.z;
}

VSOUT ProcessWaterVertex(VSIN Input,
					bool bLightingOnly, 
					int iUVSource,
					int iBTexTransform,
					int iNTexTransform, 
					int iDNTexTransform,
					int iDirLightCount, 
					int iOmniLightCount, 
					int iSpotLightCount,
					int iCylSpotLightCount,
					bool bFog,
					int iUnused11 = 0,
					int iUnused12 = 0,
					int iUnused13 = 0,
					int iUnused14 = 0,
					int iUnused15 = 0)
{
    VSOUT Output = (VSOUT)0;
    
	float4 vScreenspacePostion;
	float3 vModelSpaceViewerVector;
		
	// calculate screen space position
	float4 vModelSpacePosition = float4(Input.Position.xy, Input.Tangent.w, 1.0f);
	float3 vViewspacePosition = mul(vModelSpacePosition, g_mWorldView);
	vScreenspacePostion = mul( vModelSpacePosition, g_mWorldViewProj );
	Output.Position = vScreenspacePostion;
	Output.ScreenPosition = vScreenspacePostion;
			
	// normalize screenspace position
	vScreenspacePostion /= vScreenspacePostion.w;

	// Compute view space viewer vector 
	float3 vViewerVector = normalize(-vViewspacePosition);
	
	// Compute view space normal
	float3 vViewspaceNormal = mul(Input.Normal.xyz, g_mInvTransWorldView);
	vViewspaceNormal = normalize(vViewspaceNormal);	
	
	// compute refraction vector: R = uI -(cosO2 + ucos01)N where cos02 = sqrt(1 - u*u(1-cos01*cos01) )
	float fVdotN = dot(vViewerVector, vViewspaceNormal);
	float fCos02 = sqrt(1 - uSrq *(1 - (fVdotN * fVdotN)));
	float3 vRefractionVector = (u * -vViewerVector) - ((fCos02 + u * -fVdotN) * vViewspaceNormal);
	vRefractionVector = normalize(vRefractionVector);	 
	
	// calculate Du, Dv and mdulate by distance
	float4 fDelta;
	float4 fDeltaScale = { fMaxDu, fMaxDv, 0.0f, 0.0f };
	fDelta = float4(vRefractionVector + vViewerVector, 0.0f) * fDeltaScale;
	fDelta.xy *= min(vViewspacePosition.z, fMaxZ) + fZOffset;
	
	// use screen space position as texcoord
	Output.TexCoord0.x  = (vScreenspacePostion.x + 1.0f) * 0.5f;
	Output.TexCoord0.y  = (-vScreenspacePostion.y + 1.0f) * 0.5f;

	// take viewport into account
	// SDALLAIRE : Should be exectuted in editor only.
	Output.TexCoord0.xy = lerp( g_vViewportUV.xy, g_vViewportUV.zw, Output.TexCoord0.xy );

	Output.TexCoord0.xy *= Output.Position.w;
	Output.TexCoord0.xy += fDelta;
	Output.TexCoord0.z   = Output.Position.w;
			
	// calculate reflection vector
	vModelSpaceViewerVector = normalize(g_vViewerPosition.xyz - vModelSpacePosition.xyz);
	
	if (!bLightingOnly)
	{	
		vViewerVector = mul(vModelSpaceViewerVector, g_mInvTransWorld);
		float3 vReflection			= -reflect(vViewerVector, mul(Input.Normal.xyz, g_mInvTransWorld));
		float3 vPlanarReflection	= -reflect(vViewerVector, g_vWorldSpacePlaneNormal.xyz);
		Output.Reflected.xyz = lerp(vReflection, vPlanarReflection, REFLECTION_DAMPING);
	}
	
	if (iDirLightCount + iOmniLightCount + iSpotLightCount + iCylSpotLightCount > 0)
	{
		TangentSpaceBasis Basis = ComputeWaterTangentSpaceBasis(Input);
		
		Output.TangentSpaceViewer.x = dot(Basis.Tangent,  vModelSpaceViewerVector);
		Output.TangentSpaceViewer.y = dot(Basis.BiNormal, vModelSpaceViewerVector);
		Output.TangentSpaceViewer.z = dot(Basis.Normal,   vModelSpaceViewerVector); 

		// Output tangent space light direction and half way vectors from a directonal light
		if (iDirLightCount > 0)
			Output.Color0[0] = ComputePPLFromDirLight(0, Basis, Output);
		if (iDirLightCount > 1)
			Output.Color0[1] = ComputePPLFromDirLight(1, Basis, Output);
		if (iDirLightCount > 2)
			Output.Color0[2] = ComputePPLFromDirLight(2, Basis, Output);
		if (iDirLightCount > 3)
			Output.Color0[3] = ComputePPLFromDirLight(3, Basis, Output);

		// Output tangent space light direction and half way vectors from an omni light
		if (iOmniLightCount > 0)
			Output.Color0[iDirLightCount + 0] = ComputePPLFromOmniLight(iDirLightCount + 0, Basis, vModelSpacePosition, Output);
		if (iOmniLightCount > 1)
			Output.Color0[iDirLightCount + 1] = ComputePPLFromOmniLight(iDirLightCount + 1, Basis, vModelSpacePosition, Output);
		if (iOmniLightCount > 2)
			Output.Color0[iDirLightCount + 2] = ComputePPLFromOmniLight(iDirLightCount + 2, Basis, vModelSpacePosition, Output);
		if (iOmniLightCount > 3)
			Output.Color0[iDirLightCount + 3] = ComputePPLFromOmniLight(iDirLightCount + 3, Basis, vModelSpacePosition, Output);

		// Output tangent space light direction and half way vectors from an omni light
		if (iSpotLightCount > 0)
			Output.Color0[iDirLightCount + iOmniLightCount + 0] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + 0, Basis, vModelSpacePosition, Output, false);
		if (iSpotLightCount > 1)
			Output.Color0[iDirLightCount + iOmniLightCount + 1] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + 1, Basis, vModelSpacePosition, Output, false);
		if (iSpotLightCount > 2)
			Output.Color0[iDirLightCount + iOmniLightCount + 2] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + 2, Basis, vModelSpacePosition, Output, false);
		if (iSpotLightCount > 3)
			Output.Color0[iDirLightCount + iOmniLightCount + 3] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + 3, Basis, vModelSpacePosition, Output, false);

		// Output tangent space light direction and half way vectors from a spot light
		if (iCylSpotLightCount > 0)
			Output.Color0[iDirLightCount + iOmniLightCount + iSpotLightCount + 0] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + iSpotLightCount + 0, Basis, vModelSpacePosition, Output, true);			
		if (iCylSpotLightCount > 1)
			Output.Color0[iDirLightCount + iOmniLightCount + iSpotLightCount + 1] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + iSpotLightCount + 1, Basis, vModelSpacePosition, Output, true);
		if (iCylSpotLightCount > 2)
			Output.Color0[iDirLightCount + iOmniLightCount + iSpotLightCount + 2] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + iSpotLightCount + 2, Basis, vModelSpacePosition, Output, true);
		if (iCylSpotLightCount > 3)
			Output.Color0[iDirLightCount + iOmniLightCount + iSpotLightCount + 3] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + iSpotLightCount + 3, Basis, vModelSpacePosition, Output, true);

	}	
	
#ifndef VS_NO_TEX_COORD
    // calculate texcoords
    
	float3 vInputTexCoord0;

	if( iBTexTransform != TEXTRANSFORM_NO_TEX_COORD )
	{
		if ( iUVSource == UVSOURCE_CHROME)
		{
	#ifndef VS_NO_NORMAL
			vInputTexCoord0 = mul( Input.Normal.xyz, g_mWorldView );
	#else
			vInputTexCoord0 = float3(0.0f, 0.0f, 0.0f);
	#endif 
		}
		else if ( iUVSource == UVSOURCE_PLANAR_GIZMO)
		{    
			vInputTexCoord0 = vModelSpacePosition;
		}
		else
		{
			// default case, use input tex coord
			vInputTexCoord0 = float3(Input.TexCoord0, 0.0f);
		}
		
		if( iBTexTransform == TEXTRANSFORM_NORMAL)
		{
			// texture transform with matrix
			Output.TexCoord1.xy = mul( float4(vInputTexCoord0, 1.0f), g_mTextureTransform0 );
		}
		else
		{
			Output.TexCoord1.xy = vInputTexCoord0.xy;
		}
	}
		    
    // Normal map texture coordinates
    if ( iNTexTransform != TEXTRANSFORM_NO_TEX_COORD )
    {
        if ( iNTexTransform == TEXTRANSFORM_NORMAL )
        {
            // Transform using the normal map texture coordinate matrix
            Output.TexCoord1.zw = mul( float4(Input.TexCoord0, 0.0f, 1.0f), g_mTextureTransform1 );
        }
        else
        {
            // Use the base texture coordinates as-is
            Output.TexCoord1.zw = vInputTexCoord0.xy;
        }
    }

    // Detail normal map texture coordinates
    if ( iDNTexTransform != TEXTRANSFORM_NO_TEX_COORD )
    {
        if ( iDNTexTransform == TEXTRANSFORM_NORMAL )
        {
            // Transform using the detail normal map texture coordinate matrix
            vInputTexCoord0.xy = mul( float4(Input.TexCoord0, 0.0f, 1.0f), g_mTextureTransform3 );
        }

		Output.Reflected.w			= vInputTexCoord0.x;
		Output.TangentSpaceViewer.w = vInputTexCoord0.y;
    }
#endif // VS_NO_TEX_COORD

	// calculate fog factor
	if( bFog )
	{
		Output.TexCoord0.w = saturate( (vViewspacePosition.z - g_fFogNear) / g_fFogDistance );
	        			
		// Pitch attenuation
		float3 vNormalizedWorldVector = normalize( mul( vViewspacePosition.xyz, g_mCamera ));
		float fLerp = 1.0f - saturate( (vNormalizedWorldVector.z - g_vFogPitchAttenuation.x) / g_vFogPitchAttenuation.y );
		float fPitchAttenuation = 1 - (g_fFogPitchAttenuationIntensity * exp( 3.0f * -fLerp ));

		// Compute fog
		Output.TexCoord0.w = saturate( Output.TexCoord0.w * g_fFogDensity * fPitchAttenuation );
	}

	return Output;
}

VSOUT ProcessWaterMaskVertex(VSIN Input)
{
    VSOUT Output = (VSOUT)0;

	// calculate screen space position
	float4 vModelSpacePosition = float4(Input.Position.xy, Input.Tangent.w, 1.0f);
	Output.Position = mul( vModelSpacePosition, g_mWorldViewProj );
	
	return Output;
}

VSOUT ProcessVertex(VSIN Input,
					bool bLightingOnly, 
					int iUVSource,
					int iBTexTransform,
					int iNTexTransform, 
					int iDNTexTransform,
					int iDirLightCount, 
					int iOmniLightCount, 
					int iSpotLightCount,
					int iCylSpotLightCount,
					bool bFog,
					int iShaderId,
					int iUnused12,
					int iUnused13,
					int iUnused14,
					int iUnused15)
{
	if (iShaderId == 0)
	{
		return ProcessWaterVertex(Input, bLightingOnly, iUVSource, iBTexTransform, iNTexTransform, iDNTexTransform, iDirLightCount, iOmniLightCount, iSpotLightCount, iCylSpotLightCount, bFog);
	}
	
	return ProcessWaterMaskVertex(Input);
}

// Test shader entry points for various configurations of shaders
VSOUT MainTest( VSIN Input )
{
    return ProcessVertex(Input, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}