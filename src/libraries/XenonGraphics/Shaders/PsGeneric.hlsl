//-----------------------------------------------------------------------------
// Common pixel shader
//-----------------------------------------------------------------------------
//#include "ShaderCommon.hlsl"
#include "..\XeSharedDefines.h"
#include "PsCommon.hlsl"

float SampleSpecularMap( in VSOUT Input, int iSpecularMapChannel, bool bSpecularInBaseMap, float4 vBaseMapColor )
{
    /*float3 vTangentSpaceViewo;
					
    vTangentSpaceViewo.x = Input.HalfWay[0].w;
  	vTangentSpaceViewo.y = Input.HalfWay[1].w;
   	vTangentSpaceViewo.z = Input.HalfWay[2].w;
   	vTangentSpaceViewo = normalize( vTangentSpaceViewo );

    float4 caca = tex2D( g_TextureSampler[2], Input.TexCoord0.zw);
	float2 Newslip = Input.TexCoord0.zw -
	vTangentSpaceViewo*caca.w*0.04f;*/
	  
    if ( bSpecularInBaseMap )
    {
        if ( iSpecularMapChannel == SPECULARMAPCHANNEL_A )
        {
            return vBaseMapColor.a;
        }
        else if ( iSpecularMapChannel == SPECULARMAPCHANNEL_R )
        {
            return vBaseMapColor.r;
        }
        else if ( iSpecularMapChannel == SPECULARMAPCHANNEL_G )
        {
            return vBaseMapColor.g;
        }
        else
        {
            return vBaseMapColor.b;
        }
    }
    else
    {
    float2 Newslip = Input.TexCoord0.zw;
    /*float3 vTangentSpaceViewo;
					
    vTangentSpaceViewo.x = Input.HalfWay[0].w;
  	vTangentSpaceViewo.y = Input.HalfWay[1].w;
   	vTangentSpaceViewo.z = Input.HalfWay[2].w;
   	vTangentSpaceViewo = normalize( vTangentSpaceViewo );*/


        if ( iSpecularMapChannel == SPECULARMAPCHANNEL_A )
        {
            return tex2D(g_TextureSampler[2], Newslip/**-vTangentSpaceViewo*/).a;
        }
        else if ( iSpecularMapChannel == SPECULARMAPCHANNEL_R )
        {
            return tex2D(g_TextureSampler[2], Newslip/**-vTangentSpaceViewo*/).r;
        }
        else if ( iSpecularMapChannel == SPECULARMAPCHANNEL_G )
        {
            return tex2D(g_TextureSampler[2], Newslip/**-vTangentSpaceViewo*/).g;
        }
        else
        {
            return tex2D(g_TextureSampler[2], Newslip/**-vTangentSpaceViewo*/).b;
        }
    }
}

float3 ComputePPL(  in VSOUT Input, 
                    in bool bDiffusePPL, 
                    in bool bSpecularPPL,
                    in float fAttenuatedDiffuseDot,
                    in float fAttenuatedSpecularCoefficient,
                    in float4 vDiffuseColor,
                    in float4 vSpecularColor,
                    in float4 vBaseMapColor )
{
    float3 vOutputColor = 0;
	
    if (bDiffusePPL)
    {           
        if (bSpecularPPL)
        {
            // Add diffuse and specular contribution
            vOutputColor = (fAttenuatedDiffuseDot * vDiffuseColor * vBaseMapColor) + (fAttenuatedSpecularCoefficient * vSpecularColor);
        }
        else
        {
            // Diffuse only
            vOutputColor = fAttenuatedDiffuseDot * vDiffuseColor * vBaseMapColor;
        }
    }
    else
    {
        vOutputColor = fAttenuatedSpecularCoefficient * vSpecularColor;
    }
    
    return vOutputColor;
}


float4 ProcessPixel (   VSOUT Input,
                        bool bLocalAlpha         = LOCAL_ALPHA_OFF,
                        bool bFog                = FOG_OFF,
                        bool bOutputFog          = FOG_OFF,
                        bool bUseBaseMap         = BASEMAP_OFF,
                        bool bColor2X            = COLOR2X_OFF,
                        int  iPPLightCount       = 0,
						bool bAddAmbient		 = DONT_ADD_AMBIENT,
						bool bAddGlobalRLI 		 = ADD_RLI_OFF,
						bool bAddLocalRLI 		 = ADD_RLI_OFF,
                        bool bDiffusePPL         = DIFFUSE_PPL_OFF,
                        bool bSpecularPPL        = SPECULAR_PPL_OFF,
                        bool bShadow0		     = SHADOW_OFF,
                        bool bShadow1		     = SHADOW_OFF,
                        bool bShadow2		     = SHADOW_OFF,
                        bool bShadow3		     = SHADOW_OFF,
                        bool bUseSpecularMap     = SPECULARMAP_OFF,
                        bool bUseDetailNormalMap = DETAILNMAP_OFF,
						int  iSpecularMapChannel = SPECULARMAPCHANNEL_A,
						bool bRemapAlphaRange    = DONT_REMAP_ALPHA_RANGE,
						bool bUseMossMap         = MOSS_MAP_OFF,
						bool boUseOffset			 = USE_OFFSET_OFF,
						bool bDepthToColor       = DEPTH_TO_COLOR_OFF,
                        bool bRimLight0		     = RIMLIGHT_OFF,
                        bool bRimLight1		     = RIMLIGHT_OFF,
                        bool bRimLight2		     = RIMLIGHT_OFF,
                        bool bRimLight3		     = RIMLIGHT_OFF,
                        bool bRimLightSMapAttenuation = false,
                        bool bNormalOffset       = NORMAL_OFFSET_OFF,
                        bool bIsStaticObject     = 1,
                        bool bAlphaTest          = 0,
                        bool bSpecularInBaseMap  = 0,
                        bool bMatLODDetailOptimize = 0,
                        int iMatLODState         = MAT_LOD_FULL,
                        bool bLuminanceInAlpha   = false,
                        bool bReflection		 = false) : COLOR
{
	float4 vOutputColor     = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 vTotalRimLight   = 0;
    float  fRimLightFresnel = 0;
	float4 vBaseMapColor    = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 vShadowSample;

    float3 vTangentSpaceViewo;
	float2 Newslip;

	if ( boUseOffset )			
    {
		vTangentSpaceViewo.x = -Input.HalfWay[0].w;
  		vTangentSpaceViewo.y = Input.HalfWay[1].w;
   		vTangentSpaceViewo.z = Input.HalfWay[2].w;
   		vTangentSpaceViewo = normalize( vTangentSpaceViewo );
	
		float4 caca = tex2D( g_TextureSampler[0], Input.TexCoord1.xy);
		Newslip = Input.TexCoord1.xy -vTangentSpaceViewo*(caca.w*0.04f-0.02f);
	}
	else
	{
		Newslip = Input.TexCoord1.xy;
	}

	// Sample shadow
    if (bShadow0 || bShadow1 || bShadow2 || bShadow3)
	{
		float2 ScreenPos = (Input.ScreenPos / g_vResolution.xy) + g_vResolution.zw;
		vShadowSample = tex2D(g_TextureSampler[6], ScreenPos);
	}
	else
	{
		vShadowSample = 0.0f;
	}

	if (bUseBaseMap)
	{
		if (boUseOffset)
		vBaseMapColor = tex2D( g_TextureSampler[0], Newslip);
		else
		vBaseMapColor = tex2D( g_TextureSampler[0], Input.TexCoord0.xy);
    }

	if (bLocalAlpha)
    {
        vOutputColor.a  = g_fLocalAlpha;
    }
    else
    {
		if (!bDiffusePPL && !bSpecularPPL)
			vOutputColor.a = Input.Color0.a;
		else
			vOutputColor.a  = Input.Color1.a;

        if (bRemapAlphaRange)
	    {
		    vOutputColor.a = saturate((vOutputColor.a * g_fAlphaScale) + g_fAlphaOffset);
	    }
    }
	
    float  fMossFactor;
	float4 vMossColor;
	if (bUseMossMap)
    {
		if (bUseDetailNormalMap || bMatLODDetailOptimize)
		{ 
			vMossColor = tex2D( g_TextureSampler[5], Input.TexCoord1.zw);
			//vMossColor = tex2D( g_TextureSampler[5], Newslip);
		}
		else
		{
			vMossColor = tex2D( g_TextureSampler[5], Input.TexCoord0.xy);
			//vMossColor = tex2D( g_TextureSampler[5], Newslip);
		}
		 
		// make it opaque no matter what the texture says when moss is at 100%
		vMossColor.a = lerp(vOutputColor.a*vMossColor.a*2, 1, vOutputColor.a);
		
		// alpha test of the moss blended out by moss texture alpha
		fMossFactor = vMossColor.a*saturate((vMossColor.a * g_avMossColor.x) - g_avMossColor.y);
    }
        
    if (!bDiffusePPL && !bSpecularPPL)
    {        
        if (bShadow0 || bShadow1 || bShadow2 || bShadow3)
        {
			float fShadowSum = 0;
			if (bShadow0) fShadowSum += dot(vShadowSample, g_vShadowChannels[0]);
			if (bShadow1) fShadowSum += dot(vShadowSample, g_vShadowChannels[1]);
			if (bShadow2) fShadowSum += dot(vShadowSample, g_vShadowChannels[2]);
			if (bShadow3) fShadowSum += dot(vShadowSample, g_vShadowChannels[3]);
			
			float fShadowAtt = (1.0f - (g_fOneOverTotalLightCount * (g_fShadowCount - fShadowSum )));
			fShadowAtt = saturate(fShadowAtt);
			vOutputColor.rgb = (fShadowAtt * Input.Color0.rgb) + Input.Color1.rgb;
        }
        else
        {
			vOutputColor.rgb = Input.Color0.rgb;
        }

 	    if (bAddAmbient)
 			vOutputColor.rgb += g_vAmbientColor; 			

        if (bUseMossMap)
        {
		    if (bUseBaseMap)
			    vOutputColor *= lerp(vBaseMapColor, vMossColor, fMossFactor);
		    else
                vOutputColor *= vMossColor;
        }
        else
        {
		    if (bUseBaseMap)
			    vOutputColor *= vBaseMapColor;
	    }
    }
    else
    {
        float3 vNormal;
		float4 vMatDiffuseColor;
        
        if( iMatLODState == MAT_LOD_OPTIMIZED )
        {
            vNormal = float3(0,0,1);
        }
        else
        {
            // fetch and bias normal vector
			vNormal = SampleNormalMap( g_TextureSampler[1], Newslip );

            if( iMatLODState == MAT_LOD_BLEND ) 
            {
                vNormal = normalize( lerp(float3(0,0,1), vNormal, g_fMatLODBlend) );
            }
        }
        
        // normal  bias
		vNormal.xy *= g_avMatDiffuseColor[0].a;
                    
        if (bUseDetailNormalMap)
        {
            float3 vDetailNormal = SampleNormalMap( g_TextureSampler[3], Input.TexCoord1.zw );
			vDetailNormal.xy	*= g_avMatSpecularColor[0].a;

			if(bUseMossMap) // detail normal map applies only on moss
			{
				vNormal = normalize(lerp(vNormal, vDetailNormal, fMossFactor * g_fDetailNormalMapFactor));
			}
			else
			{
	            vNormal = normalize(vNormal + (g_fDetailNormalMapFactor * vDetailNormal));
			}
        }
        else
        {
			if (bNormalOffset)
			{
				vNormal += float3(Input.TexCoord1.zw, 0.0f);
			}
			
			vNormal = normalize(vNormal);
        }
        
        float fSpecularIntensity = 1.0;
        float fSpecularSample;
        if( bSpecularPPL )
        {
            // calculate specular intensity
            if (bUseSpecularMap)
            {
                fSpecularSample = SampleSpecularMap(Input, iSpecularMapChannel, bSpecularInBaseMap, vBaseMapColor);
                fSpecularIntensity = fSpecularSample;
            }
            
            fSpecularIntensity = max(fSpecularIntensity + g_fSpecularBias, 0);

			if(bUseMossMap) // moss only uses g_fMossSpecularFactor% of specular intensity (1=100%)
			{
				fSpecularIntensity *= lerp(1.0, g_fMossSpecularFactor, fMossFactor);
			}
        }
        else if (bUseMossMap)
        {
			fSpecularIntensity = lerp(1.0, g_fMossSpecularFactor, fMossFactor);
        }
        
        if( bSpecularPPL && !bSpecularInBaseMap && iMatLODState == MAT_LOD_BLEND )
        {
            fSpecularIntensity = fSpecularIntensity * g_fMatLODBlend;
        }
              
        float3 vTangentSpaceView;
					
        if ( bRimLight0 || bRimLight1 || bRimLight2 || bRimLight3 )
	    {
	        //
	        // Compute rim intensity (constant for all lights)
	    	// Border appears when the normal and view vector are separated by almost a right angle
            //
 	        vTangentSpaceView.x = Input.HalfWay[0].w;
	    	vTangentSpaceView.y = Input.HalfWay[1].w;
	    	vTangentSpaceView.z = Input.HalfWay[2].w;
	    	
	    	float fHeightAttenuation;
	    	if( iPPLightCount == 4 )
	    	{
	    	    // Height attenuation is encoded in vector length [0.5, 1.0f]
	    	    float fLength = length(vTangentSpaceView);
	    	    fHeightAttenuation = saturate( (fLength-0.5f) * 2.0f);
	    	    vTangentSpaceView /= fLength;
	    	}
	    	else
	    	{
	    	    vTangentSpaceView = normalize( vTangentSpaceView );
	    	    fHeightAttenuation = Input.LightDir[0].w;
	    	}
	    		        
            // Compute rim light contribution using vertex normal
            float fBorderVertexNormal = vTangentSpaceView.z;
 	        float fRimLightVertexNormal = 1 - smoothstep( g_fRimLightWidthMin, g_fRimLightWidthMax, fBorderVertexNormal );
 	        
 	        if( iMatLODState == MAT_LOD_OPTIMIZED )
			{
			    fRimLightFresnel = fRimLightVertexNormal;
			}
			else
			{
   	            // Compute rim light contribution using normal from texture
   	            float fBorderPixelNormal = saturate( dot( vNormal, vTangentSpaceView ) );
                float fRimLightPixelNormal = 1 - smoothstep( g_fRimLightWidthMin, g_fRimLightWidthMax, fBorderPixelNormal );
                
                // Modulate both results. This way we combine the border detection of the vertex method, but we add the detail of normal map
                fRimLightFresnel = lerp( fRimLightVertexNormal, fRimLightPixelNormal * fRimLightVertexNormal, g_fRimLightNormalMapRatio );
            }
              
            // Intensity
            fRimLightFresnel *= g_fRimLightIntensity; 
            fRimLightFresnel *= fHeightAttenuation;
	        
            if( bRimLightSMapAttenuation )
            {
                if( iMatLODState == MAT_LOD_OPTIMIZED )
                {
                    fRimLightFresnel = 0;
                }
                else if( bSpecularPPL && bUseSpecularMap )
                {
                    if( iMatLODState == MAT_LOD_BLEND )
                    {
                        fRimLightFresnel *= (fSpecularSample * g_fMatLODBlend);
                    }
                    else
                    {
                        fRimLightFresnel *= fSpecularSample;
                    }
                }
            }
        }
 	    else
 	    {
 	        vTangentSpaceView = float3(0,0,1);
 	    }
 	    
        if (bUseMossMap)
        {
			if (bUseBaseMap)
			{
    			vBaseMapColor   = lerp(vBaseMapColor, vMossColor, fMossFactor);    				
    		}
		    else
		    {
			        vBaseMapColor = vMossColor;
		    }
		
			vOutputColor.a *= vBaseMapColor.a;
        }
        else
        {	
	    	if (bUseBaseMap)
	    	{
				vOutputColor.a *= vBaseMapColor.a;
			}
            else
                vBaseMapColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    	} 
    
        float fRLIScale;
        float3 vGlobalRLI, vLocalRLI;
        
		if (bAddGlobalRLI)
		{
			// initialize to global RLI
            vGlobalRLI = ((Input.Color1.rgb * g_fGlobalRLIScale) + g_fGlobalRLIOffset);
	    }
            
        //
		// Compute shadow terms for all lights
		//
		float4 fShadowTerms = 1.0;
	    if( iPPLightCount > 0 && bShadow0 ) fShadowTerms.x = dot(vShadowSample, g_vShadowChannels[0]); 
	    if( iPPLightCount > 1 && bShadow1 ) fShadowTerms.y = dot(vShadowSample, g_vShadowChannels[1]); 
	    if( iPPLightCount > 2 && bShadow2 ) fShadowTerms.z = dot(vShadowSample, g_vShadowChannels[2]); 
	    if( iPPLightCount > 3 && bShadow3 ) fShadowTerms.w = dot(vShadowSample, g_vShadowChannels[3]); 
		
	    float4 vAttenuations = fShadowTerms * Input.Color0;
    
		//			
		// Compute Local RLI
		//
		if (bAddLocalRLI)
		{
            float4 vFactor = vAttenuations * g_afRLIBlendingScale;
            float fFactor = dot( vFactor, 1 );
            vLocalRLI = Input.Color1.rgb * fFactor;
		}

        //
        // Compute RLI blends for all lights
        //
        float4 vRLIBlends = lerp(1, Input.Color1.g, g_afRLIBlendingOffset);
        
        float3 vNormLightDir0;
        float3 vNormLightDir1;
        float3 vNormLightDir2;
        float3 vNormLightDir3;
        
        // 
        // Compute diffuse dots for all lights
        //
        float4 vDiffuseDots=0;
        float3 vLightDir3;
        if( iPPLightCount > 0 ) { vNormLightDir0 = normalize(Input.LightDir[0].xyz); vDiffuseDots.x = dot(vNormLightDir0, vNormal); }
        if( iPPLightCount > 1 ) { vNormLightDir1 = normalize(Input.LightDir[1].xyz); vDiffuseDots.y = dot(vNormLightDir1, vNormal); }
        if( iPPLightCount > 2 ) { vNormLightDir2 = normalize(Input.LightDir[2].xyz); vDiffuseDots.z = dot(vNormLightDir2, vNormal); }
        if( iPPLightCount > 3 )
        {
            vLightDir3 = float3(Input.LightDir[0].w, Input.LightDir[1].w, Input.LightDir[2].w);
            vNormLightDir3 = normalize(vLightDir3.xyz);
            vDiffuseDots.w = dot(vNormLightDir3, vNormal);
        }
        float4 vAttenuatedDiffuseDots = max(vAttenuations * vDiffuseDots, 0.0f);
        
        // calculate a 1 or 0 value used as a specular cutoff
        vDiffuseDots = min(vAttenuatedDiffuseDots*1000000.0f, 1.0f);
        
        // 
        // Compute specular dots for all lights
        //
        float4 vAttenuatedSpecularCoefficients = 0;
        if( bSpecularPPL )
        {
            if( iPPLightCount > 0 ) vAttenuatedSpecularCoefficients.x = dot(normalize(Input.HalfWay[0].xyz), vNormal);
            if( iPPLightCount > 1 ) vAttenuatedSpecularCoefficients.y = dot(normalize(Input.HalfWay[1].xyz), vNormal);
            if( iPPLightCount > 2 ) vAttenuatedSpecularCoefficients.z = dot(normalize(Input.HalfWay[2].xyz), vNormal);
            if( iPPLightCount > 3 )
            {
                float3 vHalfWay3 = normalize( float3(Input.HalfWay[0].w, Input.HalfWay[1].w, Input.HalfWay[2].w) );
                if( bRimLight0 || bRimLight1 || bRimLight2 || bRimLight3 )
                {
                    vHalfWay3 = normalize( vNormLightDir3 + vHalfWay3 );
                }
		        vAttenuatedSpecularCoefficients.w = dot(vHalfWay3.xyz, vNormal);
            }
              
            vAttenuatedSpecularCoefficients = max(vAttenuatedSpecularCoefficients, 0.0f);
            vAttenuatedSpecularCoefficients = vDiffuseDots * vAttenuations * fSpecularIntensity * pow(vAttenuatedSpecularCoefficients, g_fShininess);
        }
        
        // 
        // Compute attenuated rim dots for all lights
        //
		float4 fRimLightDots;
		if( bRimLight0 || bRimLight1 ||  bRimLight2 || bRimLight3 )
		{
		    fRimLightDots = 0;
		    if( iPPLightCount > 0 && bRimLight0 ) fRimLightDots.x = dot( vNormLightDir0, vTangentSpaceView);
	        if( iPPLightCount > 1 && bRimLight1 ) fRimLightDots.y = dot( vNormLightDir1, vTangentSpaceView);
	        if( iPPLightCount > 2 && bRimLight2 ) fRimLightDots.z = dot( vNormLightDir2, vTangentSpaceView);
	        if( iPPLightCount > 3 && bRimLight3 ) fRimLightDots.w = dot( vNormLightDir3, vTangentSpaceView);
	        fRimLightDots = -fRimLightDots;
	        fRimLightDots = max(fRimLightDots,0);
	        fRimLightDots *= Input.Color0;
	    } 

        float fMossAlphaFactor;
        if( bUseMossMap )
        {
            fMossAlphaFactor = vOutputColor.a * fMossFactor;
        }
        
        if (iPPLightCount > 0)
        {
			if (bAddGlobalRLI && bShadow0 )
			{
				// attenuate global RLI contribution
				vGlobalRLI *= fShadowTerms.x;
            }
				
            // compute Per-Pixel Lighting
            if(bUseMossMap)
            { 
			    vMatDiffuseColor = g_avMatDiffuseColor[0] + fMossAlphaFactor * g_avMossMinusMatDiffuseColor[0];
            }
            else
            {
			    vMatDiffuseColor = g_avMatDiffuseColor[0];
		    }

			vOutputColor.rgb += vRLIBlends.x * ComputePPL( Input, bDiffusePPL, bSpecularPPL, vAttenuatedDiffuseDots.x,
			                                               vAttenuatedSpecularCoefficients.x, vMatDiffuseColor, g_avMatSpecularColor[0],
			                                               vBaseMapColor );

            if( bRimLight0 )
            {
                vTotalRimLight += fRimLightDots.x * vMatDiffuseColor;
            }
        }

        if (iPPLightCount > 1)
        {
			if (bAddGlobalRLI && bShadow1)
			{
				// attenuate global RLI contribution
			    vGlobalRLI *= fShadowTerms.y;
            }
            
            // compute Per-Pixel Lighting
            if(bUseMossMap)
            { 
			    vMatDiffuseColor = g_avMatDiffuseColor[1] + fMossAlphaFactor * g_avMossMinusMatDiffuseColor[1];
            }
            else
            {
				vMatDiffuseColor = g_avMatDiffuseColor[1];
			}

	        vOutputColor.rgb += vRLIBlends.y * ComputePPL( Input, bDiffusePPL, bSpecularPPL, vAttenuatedDiffuseDots.y,
			                                               vAttenuatedSpecularCoefficients.y, vMatDiffuseColor, g_avMatSpecularColor[1],
			                                               vBaseMapColor );

            if( bRimLight1 )
            {
                vTotalRimLight += fRimLightDots.y * vMatDiffuseColor;
            }
        }

        if (iPPLightCount > 2)
        {
			if (bAddGlobalRLI && bShadow2 )
			{
				// attenuate global RLI contribution
			    vGlobalRLI *= fShadowTerms.z;
            }
            
            // compute Per-Pixel Lighting
            if(bUseMossMap)
            { 
        	    vMatDiffuseColor = g_avMatDiffuseColor[2] + fMossAlphaFactor * g_avMossMinusMatDiffuseColor[2];
            }
            else
            {
				vMatDiffuseColor = g_avMatDiffuseColor[2];
			}

	        vOutputColor.rgb += vRLIBlends.z * ComputePPL( Input, bDiffusePPL, bSpecularPPL, vAttenuatedDiffuseDots.z,
			                                               vAttenuatedSpecularCoefficients.z, vMatDiffuseColor, g_avMatSpecularColor[2],
			                                               vBaseMapColor );

            if( bRimLight2 )
            {
                vTotalRimLight += fRimLightDots.z * vMatDiffuseColor;
            }
        }
            
        if (iPPLightCount > 3)
        {
            if (bAddGlobalRLI && bShadow3 )
		    {
				// attenuate global RLI contribution
			    vGlobalRLI *= fShadowTerms.w;
	        }
                
            // compute Per-Pixel Lighting
            if (bUseMossMap)
            { 
                vMatDiffuseColor = g_avMatDiffuseColor[3] + fMossAlphaFactor * g_avMossMinusMatDiffuseColor[3];
            }
            else
            {
		        vMatDiffuseColor = g_avMatDiffuseColor[3];
	        }
								
	        vOutputColor.rgb += vRLIBlends.w * ComputePPL( Input, bDiffusePPL, bSpecularPPL, vAttenuatedDiffuseDots.w,
			                                               vAttenuatedSpecularCoefficients.w, vMatDiffuseColor, g_avMatSpecularColor[3],
			                                               vBaseMapColor );

            if( bRimLight3 )
            {
                vTotalRimLight += fRimLightDots.w * vMatDiffuseColor;
            }
        }

        // add up global and local RLI contributions
        float3 vTotalRLI = float3(0,0,0);
        if ((iPPLightCount > 0) && bAddLocalRLI)
        {
			if (bAddGlobalRLI)
			{
				vTotalRLI = max((vGlobalRLI + vLocalRLI), float3(0.0f, 0.0f, 0.0f));
			}
			else
			{
				vTotalRLI = max(vLocalRLI, float3(0.0f, 0.0f, 0.0f));
			}
        }
        else if (bAddGlobalRLI)
        {
			vTotalRLI = max(vGlobalRLI, float3(0.0f, 0.0f, 0.0f));
        }
        
        
 	    if(1) // RLI is now also normal mapped
		{					
		    float3 vFactor = 0;
						
			if (bAddAmbient)
		    {
		        vFactor += g_vAmbientColor;
	        }

			if( bRimLight0 || bRimLight1 || bRimLight2 || bRimLight3 )
			{
				vTotalRimLight *= fRimLightFresnel;
			    vFactor += vTotalRimLight;
			}    
			
		    vOutputColor.rgb += (vFactor * vBaseMapColor);
		
			// if we want ambient normal mapped as well, uncomment this and comment the ambient further up
			//if (bAddAmbient)
 			//	vTotalRLI.rgb += g_vAmbientColor;
 				
			vTotalRLI *= vBaseMapColor;
			
			// add the rli using the lighting to "normal map" the RLI
			// luminescence of the scene was reduces by the RLI multiplier. boost it
			// might want to expose that boost factor...
			vOutputColor.rgb += vOutputColor.rgb*vTotalRLI.rgb*10;
        }
		else
		{				
			vOutputColor.rgb += vTotalRLI * vBaseMapColor;
			
			if (bAddAmbient)
 				vOutputColor.rgb += g_vAmbientColor * vBaseMapColor;

			if( bRimLight0 || bRimLight1 || bRimLight2 || bRimLight3 )
			{
				vTotalRimLight *= fRimLightFresnel;
				vOutputColor.rgb += vTotalRimLight * vBaseMapColor;  
			}    
		}
    }
    
    if (bReflection)
    {
		vOutputColor.rgb += texCUBE(g_TextureSampler[4], Input.TexCoord1.xyz);
    }

    if (bColor2X)
    {
        vOutputColor.rgb *= g_fGlobalMul2XFactor;
    }
    
    if( bDepthToColor )
    {
        // Do our own alpha test because rendering to R32F disables alpha operations
		if(bAlphaTest)
        {
			// Alpha test is enabled        
			float fDelta = vOutputColor.a - g_vAlphaTestParams.y;
			fDelta *= g_vAlphaTestParams.z;	// Apply alpha inversion
			clip(fDelta);            
        }

        float z = (Input.TexCoord1.z / Input.TexCoord1.w);
        vOutputColor = (z,z,z,z);
    }

    // saturate final color
    vOutputColor = saturate(vOutputColor);
    
    if( bFog )
    {
        if (iPPLightCount > 3)
			vOutputColor.rgb = lerp( vOutputColor.rgb, g_fFogColor.rgb, Input.TexCoord1.z );
        else
			vOutputColor.rgb = lerp( vOutputColor.rgb, g_fFogColor.rgb, Input.LightDir[0].w );
    }

    if( bLuminanceInAlpha )
    {
        vOutputColor.a = dot(vOutputColor.rgb, float3(0.11f, 0.59f, 0.3f) );
    }

    return vOutputColor;
}

// Shader entry points for various configurations of shaders
float4 TestPS( VSOUT Input ) : COLOR
{
    return ProcessPixel( Input,
                         LOCAL_ALPHA_OFF,
                         FOG_OFF,
						 FOG_OFF,
						 BASEMAP_ON,
						 COLOR2X_OFF,
						 4,
						 DONT_ADD_AMBIENT,
						 ADD_RLI_OFF,
						 ADD_RLI_OFF,
						 DIFFUSE_PPL_ON,
						 SPECULAR_PPL_ON,
						 SHADOW_ON,
						 SHADOW_ON,
						 SHADOW_ON,
						 SHADOW_OFF,
						 SPECULARMAP_ON,
						 DETAILNMAP_ON,
						 SPECULARMAPCHANNEL_A,
						 DONT_REMAP_ALPHA_RANGE, 
						 MOSS_MAP_OFF,
						 USE_OFFSET_OFF,
						 DEPTH_TO_COLOR_OFF,
						 RIMLIGHT_ON,
						 RIMLIGHT_OFF,
						 RIMLIGHT_OFF,
						 RIMLIGHT_OFF,
						 false,
						 NORMAL_OFFSET_OFF,
						 0,
						 MAT_LOD_FULL );
}
