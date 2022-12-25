//-----------------------------------------------------------------------------
// Common vertex shader
//-----------------------------------------------------------------------------
#include "..\XeSharedDefines.h"
#include "ShaderCommon.hlsl"
#include "VsCommon.hlsl"
#include "VsLighting.hlsl"

#define NORMAL_OFFSET_FACTOR	10

#ifndef NO_VSIN

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
    float2 TexCoord1        : TEXCOORD1;
    float4 Tangent          : TANGENT;
#endif
#ifdef VS_EXTRA_TEX_COORD
    float4 TexCoord2        : TEXCOORD2;
#endif
};

#endif

#ifndef VS_NO_TEX_COORD
#ifndef VS_NO_NORMAL

TangentSpaceBasis ComputeTangentSpaceBasis(in VSIN Input, in bool bTexTransform)
{
	TangentSpaceBasis Basis;
	
	Basis.Normal   = Input.Normal.xyz;

	if( bTexTransform )
	{
		// texture transform with matrix
		Basis.Tangent = mul( Input.Tangent.xyz, (float3x3) g_mTangentTransform );
	}
	else
	{
		Basis.Tangent  = Input.Tangent.xyz;	
	}
	
	Basis.BiNormal = cross(Basis.Tangent, Basis.Normal);
	
	// invert binormal if ncessary
	Basis.BiNormal *= Input.Tangent.w;

	return Basis;
}

TangentSpaceBasis ComputeSkinnedTangentSpaceBasis(in VSIN Input, in float4x3 mSkinning, in bool bTexTransform)
{
	TangentSpaceBasis Basis;
	
	// skin normal
	float3 vNormal = Input.Normal.xyz;
	Basis.Normal = mul(vNormal, (float3x3) mSkinning );
	
	// skin tangent
	float3 vTangent = Input.Tangent.xyz;
	Basis.Tangent = mul(vTangent, (float3x3) mSkinning );
	if( bTexTransform )
	{
		// texture transform with matrix
		Basis.Tangent = mul( Basis.Tangent, (float3x3) g_mTangentTransform );
	}	
	 
	// evaluate binormal vector
	Basis.BiNormal = cross(Basis.Tangent, Basis.Normal);

	// invert binormal if necessary
	Basis.BiNormal *= Input.Tangent.w;
		
	return Basis;
}

#endif // VS_NO_TEX_COORD
#endif // VS_NO_NORMAL

VSOUT ProcessVertex( VSIN Input, 
                     int  iTransform	        = TRANSFORM_NORMAL, 
                     int  iLighting		        = LIGHTING_USE_COLOR,
                     int  iDirLightCount		= 0,
                     int  iOmniLightCount		= 0,
                     int  iSpotLightCount		= 0,
                     int  iCylSpotLightCount	= 0,
                     int  iColorSource          = COLORSOURCE_VERTEX_COLOR,
                     int  iBTexTransform        = TEXTRANSFORM_NONE,
                     int  iUVSource		        = UVSOURCE_OBJECT,
                     bool bFog			        = FOG_OFF,
                     int  iNTexTransform        = TEXTRANSFORM_NO_TEX_COORD,
                     int  iSTexTransform        = TEXTRANSFORM_NO_TEX_COORD,
                     int  iDNTexTransform       = TEXTRANSFORM_NO_TEX_COORD,
                     bool bAddAmbient           = ADD_AMBIENT_ON,
                     bool bAddRLI               = ADD_RLI_ON,
					 bool bRLIScaleAndOffset    = RLI_SCALE_OFFSET_OFF,
                     int  iReflectionType       = REFLECTION_TYPE_NONE,
                     bool bRimLightEnabled      = RIMLIGHT_OFF,
                     bool bRimLightHeightAttenuation = RIMLIGHT_OFF,
                     bool bDepthToColorEnable   = DEPTH_TO_COLOR_OFF,
                     bool bInvertMoss			= INVERTMOSS_OFF,
                     bool bOffsetMap			= USE_OFFSET_OFF,
                     int  iSPG2Mode				= SPG2_MODE_NONE,
                     bool bShadows				= SHADOW_OFF,
                     bool bVertexSpecular		= VERTEX_SPECULAR_OFF)

{
    VSOUT Output = (VSOUT)0;
    
    float4x3	mSkinning = (float4x3)0;
    float4		vModelSpacePosition;
	float3		vModelSpaceNormal;
    //float3		vEye;
        
    // transform position
    if( iTransform == TRANSFORM_NORMAL )
    {
		vModelSpacePosition = Input.Position;
        Output.Position = mul( vModelSpacePosition, g_mWorldViewProj );
    }
    else if( iTransform == TRANSFORM_SKINNING )
    {
		for (int i=0; i<g_iMaxWeight; i++)
		{
			mSkinning += mul(g_amSkinning[Input.BlendIndices[i]], Input.BlendWeights[i]);
		}
		
		// skin vertex
		vModelSpacePosition.xyz = mul(Input.Position, mSkinning);
		vModelSpacePosition.w = 1.0;
	
		// transform to screen space
		Output.Position = mul(vModelSpacePosition, g_mWorldViewProj);
    }
    else
    {
		vModelSpacePosition = Input.Position;
        Output.Position = vModelSpacePosition;
    }
    
/*#ifndef VS_NO_NORMAL
   	{
   		TangentSpaceBasis Basis;
		Basis.Normal   = Input.Normal.xyz;
		Basis.Tangent	= Input.Tangent.xyz;
	
		Basis.BiNormal = cross(Basis.Tangent, Basis.Normal);
		// invert binormal if ncessary
		Basis.BiNormal *= Input.Tangent.w;

		vEye = -g_vViewerPosition + vModelSpacePosition;
   		Output.EyeVector[0].x = dot( vEye, Basis.Tangent );
		Output.EyeVector[1].x = dot( vEye, Basis.BiNormal );
		Output.EyeVector[2].x = dot( vEye, Basis.Normal );
	}
#endif*/
	
    float4 vRLI;
    
#ifndef VS_NO_COLOR
    if (bAddRLI)
    {
		if (bRLIScaleAndOffset)
		{
			vRLI.rgb= (Input.RLI.rgb * g_fGlobalRLIScale) + g_fGlobalRLIOffset;
			vRLI.a	= Input.RLI.a;
		}
		else
		{
			vRLI	= Input.RLI;
		}
    }
#endif
    
    // calculate diffuse
    if ((iColorSource == COLORSOURCE_DIFFUSE)   || 
        (iColorSource == COLORSOURCE_DIFFUSE2X) || 
        (iColorSource == COLORSOURCE_INVERSE_DIFFUSE))
    {
		if (iLighting == LIGHTING_NO_COLOR)
		{
			Output.Color0 = float4(1.0, 1.0, 1.0, 1.0);	
		}
		
#ifndef VS_NO_COLOR
		else if (iLighting == LIGHTING_USE_COLOR)
		{
		    if (iColorSource == COLORSOURCE_INVERSE_DIFFUSE)
		    {
		        if (bAddRLI) Output.Color0.rgb = 1.0f - vRLI;
		    }
		    else
		    {
			    if (bAddRLI) Output.Color0.rgb = vRLI;						// use vertex paint
			}

			if (bAddAmbient) Output.Color0.rgb += g_vAmbientColor;				// add ambient contribution 			

			// alpha comes from RLI
			
			if (bAddRLI)
			{
				if(bInvertMoss)
				{
					Output.Color0.a = 1.0 - vRLI.a;
				}
				else
				{
					Output.Color0.a = vRLI.a;
				}
			}
        }
#endif // VS_NO_COLOR

#ifndef VS_NO_NORMAL
		else if (iLighting == LIGHTING_VERTEX)
		{		
			if ( iTransform == TRANSFORM_SKINNING )
			{
	            // skin normal
				vModelSpaceNormal = mul(Input.Normal.xyz, mSkinning);
			}
			else
			{			
				// transform normal to view space
				vModelSpaceNormal = Input.Normal.xyz;
			} 
		
			int iCurLight = 0, j = 0;

			// add directional lights contribution
			for (j=0; j<g_iDirLightCount; j++)
			{
				Output.Color0 += ProcessDirLight(vModelSpaceNormal, vModelSpacePosition, iCurLight, bVertexSpecular, iSPG2Mode);
				iCurLight++;
			}

			// add omni lights contribution
			for (j=0; j<g_iOmniLightCount; j++)
			{
				Output.Color0 += ProcessOmniLight(vModelSpaceNormal, vModelSpacePosition, iCurLight, bVertexSpecular, iSPG2Mode);
				iCurLight++;
			}
		
			// add spot lights contribution
			for (j=0; j<g_iSpotLightCount; j++)
			{
				Output.Color0 += ProcessSpotLight(vModelSpaceNormal, vModelSpacePosition, iCurLight, bVertexSpecular, iSPG2Mode);
				iCurLight++; 
			}

			// add cylindrical spot lights contribution
			for (j=0; j<g_iCylSpotLightCount; j++)
			{
				Output.Color0 += ProcessCylSpotLight(vModelSpaceNormal, vModelSpacePosition, iCurLight, bVertexSpecular, iSPG2Mode);
				iCurLight++; 
			}

			if (bAddRLI)
			{
#ifndef VS_NO_COLOR
				if (bShadows)
				{
					Output.Color1.rgb = vRLI.rgb;
				}
				else
				{
					Output.Color0.rgb += vRLI.rgb;
				}

				// alpha comes from RLI
				
				if(bInvertMoss)
				{
					Output.Color0.a = 1.0 - vRLI.a;
				}
				else
				{
					Output.Color0.a = vRLI.a;
				}
#endif
			}
			else
			{
				// use default alpha
				Output.Color0.a = 1.0f;
			}
			
			if (bAddAmbient && !bShadows) Output.Color0.rgb += g_vAmbientColor;				// add ambient contribution
		}
#ifndef VS_NO_TEX_COORD
		else if (iLighting == LIGHTING_PIXEL)
		{	
			TangentSpaceBasis Basis;
			
			if ( iTransform == TRANSFORM_SKINNING )
				Basis = ComputeSkinnedTangentSpaceBasis(Input, mSkinning, iBTexTransform == TEXTRANSFORM_NORMAL);
			else
		        Basis = ComputeTangentSpaceBasis(Input, iBTexTransform == TEXTRANSFORM_NORMAL);
		
		    float3 vModelSpaceViewer = normalize(g_vViewerPosition.xyz-vModelSpacePosition.xyz);
		    float3 vTangentSpaceViewer;
            vTangentSpaceViewer.x = dot(Basis.Tangent,  vModelSpaceViewer);
		    vTangentSpaceViewer.y = dot(Basis.BiNormal, vModelSpaceViewer);
		    vTangentSpaceViewer.z = dot(Basis.Normal,   vModelSpaceViewer);
		    
		    if (iDirLightCount > 0)
				Output.Color0[0] = ComputePPLFromDirLight(0, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output);
		    if (iDirLightCount > 1)
				Output.Color0[1] = ComputePPLFromDirLight(1, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output);
		    if (iDirLightCount > 2)
				Output.Color0[2] = ComputePPLFromDirLight(2, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output);
		    if (iDirLightCount > 3)
				Output.Color0[3] = ComputePPLFromDirLight(3, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output);
			
			if (iOmniLightCount > 0)
				Output.Color0[iDirLightCount + 0] = ComputePPLFromOmniLight(iDirLightCount + 0, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output);
			if (iOmniLightCount > 1)
				Output.Color0[iDirLightCount + 1] = ComputePPLFromOmniLight(iDirLightCount + 1, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output);
			if (iOmniLightCount > 2)
				Output.Color0[iDirLightCount + 2] = ComputePPLFromOmniLight(iDirLightCount + 2, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output);
			if (iOmniLightCount > 3)
				Output.Color0[iDirLightCount + 3] = ComputePPLFromOmniLight(iDirLightCount + 3, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output);
				
			if (iSpotLightCount > 0)
				Output.Color0[iDirLightCount + iOmniLightCount + 0] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + 0, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output, false);
			if (iSpotLightCount > 1)
				Output.Color0[iDirLightCount + iOmniLightCount + 1] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + 1, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output, false);
			if (iSpotLightCount > 2)
				Output.Color0[iDirLightCount + iOmniLightCount + 2] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + 2, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output, false);
			if (iSpotLightCount > 3)
				Output.Color0[iDirLightCount + iOmniLightCount + 3] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + 3, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output, false);
				
			if (iCylSpotLightCount > 0)
				Output.Color0[iDirLightCount + iOmniLightCount + iSpotLightCount + 0] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + iSpotLightCount + 0, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output, true);
			if (iCylSpotLightCount > 1)
				Output.Color0[iDirLightCount + iOmniLightCount + iSpotLightCount + 1] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + iSpotLightCount + 1, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output, true);
			if (iCylSpotLightCount > 2)
				Output.Color0[iDirLightCount + iOmniLightCount + iSpotLightCount + 2] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + iSpotLightCount + 2, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output, true);
			if (iCylSpotLightCount > 3)
				Output.Color0[iDirLightCount + iOmniLightCount + iSpotLightCount + 3] = ComputePPLFromSpotLight(iDirLightCount + iOmniLightCount + iSpotLightCount + 3, Basis, vModelSpacePosition, vTangentSpaceViewer, iSPG2Mode, Output, true);
		    
#ifndef VS_NO_NORMAL
            if( bRimLightEnabled )
	        {
	            // Rim light contribution will be computed in pixel shader
	            // We need to pass on the view vector and vertex normal
	            float fHeightAttenuation = 1.0f;    
	            if( bRimLightHeightAttenuation )
	            {	
	                float3 vWorldSpacePosition = mul(vModelSpacePosition, g_mWorld);
	                float fHeightRatio = saturate( (vWorldSpacePosition.z - g_fRimLightHeightWorldMin) / (g_fRimLightHeightWorldMax - g_fRimLightHeightWorldMin) );
	                fHeightAttenuation = saturate( saturate( fHeightRatio - g_fRimLightHeightAttenuationMin ) / (g_fRimLightHeightAttenuationMax - g_fRimLightHeightAttenuationMin));
	            }
		        
   	            if( (iDirLightCount + iOmniLightCount + iSpotLightCount + iCylSpotLightCount) < 4 )
   	            {
   	                 Output.LightDir[0].w = fHeightAttenuation;
   	      		}
   	      		else
   	      		{
   	      		    // Encode height attenuation [0.5, 1.0] in vector length
   	                vTangentSpaceViewer *= (fHeightAttenuation*0.5) + 0.5f;
	       		}
   	      	}
   	      	if ( bOffsetMap || bRimLightEnabled)
   	      	{	
   	      		// NOTE: When there are 4 lights, this overwrites the halfway vector 3
   	            // which will need to be recomputed in the pixel shader
   	            Output.HalfWay[0].w = vTangentSpaceViewer.x;
				Output.HalfWay[1].w = vTangentSpaceViewer.y;
				Output.HalfWay[2].w = vTangentSpaceViewer.z;
            }
#endif // VS_NO_NORMAL
				
#ifndef VS_NO_COLOR
			if (bAddRLI)
			{
				// send RLI for interpolation
				// Global RLI scale and offset will be applied per-pixel
				Output.Color1.rgb = Input.RLI.rgb;
				
				if(bInvertMoss)
				{
					Output.Color1.a = 1.0 - Input.RLI.a;
				}
				else
				{
					Output.Color1.a = Input.RLI.a;
				}
			}
			else
#endif
			{
  				// no RLI contribution
			    Output.Color1 = float4(0.0, 0.0, 0.0, 1.0);
			}
		}
#endif // VS_NO_TEX_COORD
#endif // VS_NO_NORMAL

		else // no color on vertex, use ambient if wanted
		{
			Output.Color0.a = 1.0;
			if (bAddAmbient) 
				Output.Color0.rgb += g_vAmbientColor;				// add ambient contribution 			
		}
	}
	else if (iColorSource == COLORSOURCE_CONSTANT)
	{
		Output.Color0 = g_vMatConstantColor;			// Diffuse coming from material constant color
	}
	else if (iColorSource == COLORSOURCE_VERTEX_COLOR)
	{
		if (bAddRLI) 
		{
#ifndef VS_NO_COLOR
			Output.Color0 = vRLI;
#endif
	    }
		else
			Output.Color0 = float4(1.0, 1.0, 1.0, 1.0);
	}
	else if (iColorSource == COLORSOURCE_FULL_LIGHT)
	{
		Output.Color0 = float4(1.0, 1.0, 1.0, 1.0);		// Force diffuse to white
	}
	
	if( bDepthToColorEnable )
	{
	    // Output position in tex coord 1
	    Output.TexCoord1 = Output.Position;
	}

#ifndef VS_NO_TEX_COORD
    // calculate texcoords
    if( iBTexTransform != TEXTRANSFORM_NO_TEX_COORD)
    {
		float3 vInputTexCoord0;

		if ( iUVSource == UVSOURCE_CHROME)
		{
#ifndef VS_NO_NORMAL
			vInputTexCoord0 = mul( Input.Normal.xyz, g_mWorldView );
#else
			vInputTexCoord0 = float3(0.0f, 0.0f, 0.0f);
#endif
		}
		else if ( iUVSource == UVSOURCE_DIFFUSEPHALOFF)
		{    
			float3 Local= normalize(g_vViewerPosition.xyz - vModelSpacePosition.xyz);
			float3 Color=Output.Color0;
#ifndef VS_NO_NORMAL
	float3 Norma=Input.Normal.xyz;
#else
	float3 Norma=float3 (0.0f, 0.0f, 0.0f);
#endif
			Norma=mul( Norma, g_mWorldViewProj );
			vInputTexCoord0.x = ( Color.r+Color.g+Color.b) /1.f;
			vInputTexCoord0.y =1.f-(dot( Norma, Local ));
			vInputTexCoord0.y =max(vInputTexCoord0.y,0.1f);
			vInputTexCoord0.y =min(vInputTexCoord0.y,0.9f);
			vInputTexCoord0.z =0;
		}
		else if ( iUVSource == UVSOURCE_PLANAR_GIZMO)
		{    
			vInputTexCoord0 = vModelSpacePosition;
		}
		else
		{
			vInputTexCoord0 = float3(Input.TexCoord0, 0.0f);
		}
    
		if( iBTexTransform == TEXTRANSFORM_NORMAL)
		{
			// texture transform with matrix
			Output.TexCoord0.xy = mul( float4(vInputTexCoord0, 1.0f), g_mTextureTransform0 );
		}
		else if ( iBTexTransform == TEXTRANSFORM_NONE_FUR_OFFSETS )
		{
			Output.TexCoord0.xy = vInputTexCoord0 + (g_fFurOffsetScale * Input.TexCoord1);
		}
		else if ( iBTexTransform == TEXTRANSFORM_NORMAL_FUR_OFFSETS )
		{
			Output.TexCoord0.xy = mul( float4(vInputTexCoord0, 1.0f), g_mTextureTransform0 );
			Output.TexCoord0.xy += g_fFurOffsetScale * Input.TexCoord1;
		}
		else
		{
			// default case, copy tex coords
			Output.TexCoord0.xy = vInputTexCoord0;
		}
    }

    // Normal map texture coordinates
    if ( iNTexTransform != TEXTRANSFORM_NO_TEX_COORD )
    {
        if ( iNTexTransform == TEXTRANSFORM_NORMAL )
        {
            // Transform using the normal map texture coordinate matrix
            Output.TexCoord1.xy = mul( float4(Input.TexCoord0, 0.0f, 1.0f), g_mTextureTransform1 );
        }
        else
        {
            // Use the base texture coordinates as-is
            Output.TexCoord1.xy = Output.TexCoord0.xy;
        }
    }

    // Specular map texture coordinates
    if ( iSTexTransform != TEXTRANSFORM_NO_TEX_COORD )
    {
        if ( iSTexTransform == TEXTRANSFORM_NORMAL )
        {
            // Transform using the specular map texture coordinate matrix
            Output.TexCoord0.zw = mul( float4(Input.TexCoord0, 0.0f, 1.0f), g_mTextureTransform2 );
        }
        else
        {
            // Use the base texture coordinates as-is
            Output.TexCoord0.zw = Output.TexCoord0.xy;
        }
    }

    // Detail normal map texture coordinates
    if ( iDNTexTransform != TEXTRANSFORM_NO_TEX_COORD )
    {
        if ( iDNTexTransform == TEXTRANSFORM_NORMAL )
        {
            // Transform using the detail normal map texture coordinate matrix
            Output.TexCoord1.zw = mul( float4(Input.TexCoord0, 0.0f, 1.0f), g_mTextureTransform3 );
        }
        else if (( iDNTexTransform == TEXTRANSFORM_NONE_FUR_OFFSETS ) || 
                 ( iDNTexTransform == TEXTRANSFORM_NORMAL_FUR_OFFSETS ))
        {
			// send texture coordinate offsets for fur rendering
			Output.TexCoord1.zw = Input.TexCoord1 * g_fFurOffsetScale * NORMAL_OFFSET_FACTOR;
        }
        else
        {
            // Use the base texture coordinates as-is
            Output.TexCoord1.zw = Output.TexCoord0.xy;
        }
    }
        
#endif // VS_NO_TEX_COORD

	// reflection
	if (iReflectionType == REFLECTION_TYPE_PLANAR)
	{
		
	    float3 vViewSpacePos = mul(vModelSpacePosition, g_mWorldView);
	
		// calculate an arbitrary vector from plane origin to viewspace position
		float3 vOriginToPos = vViewSpacePos - g_vReflectionPlaneOrigin.xyz;
		
		// reflect viewspace pos using the following formula P' = P - 2*Proj(v,n)*n
		vViewSpacePos.xyz = vViewSpacePos.xyz - (2 * dot(vOriginToPos.xyz, g_vReflectionPlane.xyz) * g_vReflectionPlane.xyz);
		
		// project back to clip space
		Output.Position = mul( float4(vViewSpacePos,1), g_mProjection);
	}
#ifndef VS_NO_NORMAL
	else if ((iReflectionType == REFLECTION_TYPE_CUBEMAP)   &&
			 (iNTexTransform  == TEXTRANSFORM_NO_TEX_COORD) &&
			 (iDNTexTransform == TEXTRANSFORM_NO_TEX_COORD))
	{
		float3 vViewerVector;
		vViewerVector = normalize(g_vViewerPosition.xyz - vModelSpacePosition.xyz);
		vViewerVector = mul(vViewerVector, g_mInvTransWorld);
		Output.TexCoord1.xyz = -reflect(vViewerVector, mul(Input.Normal.xyz, g_mInvTransWorld));
	}
#endif

    // Fog
    if( bFog )
    {
        float3 vViewSpacePosition = mul( vModelSpacePosition, g_mWorldView );
        
        float fFog = saturate( (vViewSpacePosition.z - g_fFogNear) / g_fFogDistance );
  
       /* if(iLighting == LIGHTING_VERTEX)
			fFog *= 0.05;*/
			
	    // Pitch attenuation
	    float3 vNormalizedWorldVector = normalize( mul( vViewSpacePosition.xyz, g_mCamera ));
	    float fLerp = 1.0f - saturate( (vNormalizedWorldVector.z - g_vFogPitchAttenuation.x) / g_vFogPitchAttenuation.y );
	    float fPitchAttenuation = 1 - (g_fFogPitchAttenuationIntensity * exp( 3.0f * -fLerp ));

        // Compute fog
	    fFog = saturate( fFog * g_fFogDensity * fPitchAttenuation );

	    // Might conflict with per-pixel lighting (4 lights) and a detail normal map
	    if ((iLighting == LIGHTING_PIXEL) && 
	        ((iDirLightCount + iOmniLightCount + iSpotLightCount + iCylSpotLightCount) > 3) && 
	        ( iDNTexTransform == TEXTRANSFORM_NO_TEX_COORD ))
	    {
			Output.TexCoord1.z = fFog;
		}
        else if (( iBTexTransform == TEXTRANSFORM_NONE_FUR_OFFSETS ) || 
			     ( iBTexTransform == TEXTRANSFORM_NORMAL_FUR_OFFSETS))
		{
		    Output.TexCoord0.z = fFog;
		}
		else
			Output.LightDir[0].w = fFog;
    }
/*#ifndef VS_NO_NORMAL
if (bOffsetMap == USE_OFFSET_ON)
{
	TangentSpaceBasis Basis;
	Basis.Normal   = Input.Normal.xyz;
	Basis.Tangent  = Input.Tangent.xyz;	
	Basis.BiNormal = cross(Basis.Tangent, Basis.Normal);
	
	// invert binormal if ncessary
	Basis.BiNormal *= Input.Tangent.w;

    float3 vModelSpaceViewero = -g_vViewerPosition.xyz+vModelSpacePosition.xyz;
    float3 vTangentSpaceViewero;
    vTangentSpaceViewero.x = dot(Basis.Tangent,  vModelSpaceViewero);
    vTangentSpaceViewero.y = dot(Basis.BiNormal, vModelSpaceViewero);
    vTangentSpaceViewero.z = dot(Basis.Normal,   vModelSpaceViewero);

    Output.HalfWay[0].w = vTangentSpaceViewero.x;
	Output.HalfWay[1].w = vTangentSpaceViewero.y;
	Output.HalfWay[2].w = vTangentSpaceViewero.z;
}
#endif*/
    return Output;
}

// Test shader entry points for various configurations of shaders
VSOUT TestVS( VSIN Input )
{
    return ProcessVertex(	Input, 
							TRANSFORM_NORMAL, 
							LIGHTING_PIXEL,
							1,
							1,
							1,
							1,
							COLORSOURCE_DIFFUSE,
							TEXTRANSFORM_NONE,
							UVSOURCE_OBJECT,
							FOG_OFF,
							TEXTRANSFORM_NO_TEX_COORD,
							TEXTRANSFORM_NO_TEX_COORD,
							TEXTRANSFORM_NO_TEX_COORD,
							ADD_AMBIENT_ON,
							ADD_RLI_ON,
							RLI_SCALE_OFFSET_OFF,
							REFLECTION_TYPE_NONE,
							RIMLIGHT_OFF,
							RIMLIGHT_OFF,
							DEPTH_TO_COLOR_OFF,
							INVERTMOSS_OFF,
							USE_OFFSET_OFF);
}
