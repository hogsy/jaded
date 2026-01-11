//-----------------------------------------------------------------------------
// After Effects pixel shader
//-----------------------------------------------------------------------------
#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

struct PSIN
{
    float4 Position         : POSITION;
    float4 Color 			: COLOR0_centroid;
    float3 TexCoord0        : TEXCOORD0_centroid;
    float3 TexCoord1        : TEXCOORD1_centroid;
    float3 TexCoord2        : TEXCOORD2_centroid;
    float3 TexCoord3        : TEXCOORD3_centroid;
    float3 TexCoord4        : TEXCOORD4_centroid;
    float3 TexCoord5        : TEXCOORD5_centroid;
};

uniform float4  g_vFogColor;
uniform float4  g_vFogParams;
#define g_fFogNearOverDistance  g_vFogParams.x
#define g_fFogDensity           g_vFogParams.z
#define g_fFogInvDistance       g_vFogParams.w
uniform float4  g_vFogNormalizing;
uniform float4  g_vFogPitchAttenuation;
#define g_fFogPitchAttenuationIntensity g_vFogPitchAttenuation.z

uniform float4 g_vGlowParams1;
#define  g_fGlowZNear            g_vGlowParams1.x
#define  g_fGlowZDistance        g_vGlowParams1.y
#define  g_fGlowIntensity        g_vGlowParams1.z

uniform float4 g_vGlowParams2;
#define  g_fGlowLuminosityMin    g_vGlowParams2.x
#define  g_fGlowLuminosityMax    g_vGlowParams2.y
uniform float4 g_vGlowColor;
uniform float4 g_vGlowBlurKernel;

uniform float g_fMotionBlurAlpha;
uniform float g_fRemananceFactor;
uniform float4 g_vColorBalance;
uniform float4 g_vBrightness;

#define g_fColorCorrectionSaturation        g_vColorBalance.x
#define g_fColorCorrectionBrightness        g_vColorBalance.y
#define g_fColorCorrectionContrasteScale    g_vColorBalance.z
#define g_fColorCorrectionContrasteTrans    g_vColorBalance.w

float4 ProcessPixel (	PSIN Input, 
						int iGodRayShaderId,
						int iRemananceShaderId, 
						int iZoomSmoothId, 
						int iBlackWhiteId, 
						int iFogShaderId, 
						int iBlitShaderId, 
						int iColorDiffusion, 
						int iMotionBlur,
						int iBigBlur,
						int iColorBalance,
						int iBKQuad,
						int iBorderBrightness,
						int iOldMovie,
						int iSpinSmooth,
						int iColorCorrection )
{
	float4 FinalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	if (iGodRayShaderId > 0)
	{
	  	if (iGodRayShaderId == GODRAY_SHADER_1)
		{
		    // Mask pass. Everything that passes z-test is black
			FinalColor	=  float4(0.0, 0.0, 0.0, 0.0f);
		}
		else if (iGodRayShaderId == GODRAY_SHADER_2 || iGodRayShaderId == 6 )
		{
		    FinalColor += Input.TexCoord1.z * tex2D( g_TextureSampler[0], Input.TexCoord1.xy );
		    FinalColor += Input.TexCoord2.z * tex2D( g_TextureSampler[0], Input.TexCoord2.xy );
		    FinalColor += Input.TexCoord3.z * tex2D( g_TextureSampler[0], Input.TexCoord3.xy );
		    FinalColor.a = Input.TexCoord0.z;
		    
		    if( iGodRayShaderId == 6 )
		    {
		        FinalColor *= Input.Color;
		    }
		}
        else if (iGodRayShaderId == GODRAY_SHADER_3)
		{
		    // Last blur and combine to output
		    float4 FinalBlur = Input.TexCoord1.z * tex2D( g_TextureSampler[0], Input.TexCoord1.xy );
		    float fFactor = Input.TexCoord2.z;
		    
		    FinalColor = (FinalBlur * fFactor ) + tex2D( g_TextureSampler[1], Input.TexCoord0.xy );
		    FinalColor.a = fFactor * Input.TexCoord0.z;
        }
        else if( iGodRayShaderId == GODRAY_SHADER_4 )
        {
            //Last blur stage
            float4 FinalBlur;
            FinalBlur  = Input.TexCoord0.z * tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
            FinalBlur += Input.TexCoord1.z * tex2D( g_TextureSampler[0], Input.TexCoord1.xy );
            
            float4 original = tex2D( g_TextureSampler[1], Input.TexCoord2.xy );
            
            float fFactor = Input.TexCoord2.z;
            FinalColor = original + (fFactor * FinalBlur);
        }
        else if (iGodRayShaderId == GODRAY_SHADER_5)
		{
		    FinalColor += Input.TexCoord0.z * tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
		    FinalColor += Input.TexCoord1.z * tex2D( g_TextureSampler[0], Input.TexCoord1.xy );
		    FinalColor += Input.TexCoord2.z * tex2D( g_TextureSampler[0], Input.TexCoord2.xy );
		    FinalColor += Input.TexCoord3.z * tex2D( g_TextureSampler[0], Input.TexCoord3.xy );
		    FinalColor.xyz *= Input.Color.xyz;
		}
    }
	else if( iZoomSmoothId > 0 )
	{
        if( iZoomSmoothId == ZOOM_SMOOTH_SHADER_1 )
		{
		    // Single blur pass
            FinalColor += Input.TexCoord1.z * tex2D( g_TextureSampler[0], Input.TexCoord1.xy );
		    FinalColor.a = Input.TexCoord0.z;
		}
        if( iZoomSmoothId == ZOOM_SMOOTH_SHADER_2 )
		{
		    // Single blur pass
            FinalColor += Input.TexCoord0.z * tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
            FinalColor += Input.TexCoord1.z * tex2D( g_TextureSampler[0], Input.TexCoord1.xy );
		}
	}
	else if (iRemananceShaderId > 0) 
	{
	    if( iRemananceShaderId >= 2 )
	    {
	        FinalColor = g_fRemananceFactor;
	        
	        if( iRemananceShaderId == 3 )
	        {
	            FinalColor.xyz = tex2D( g_TextureSampler[0], Input.TexCoord4.xy );
	        }
        }
	    else
	    {
	        float4 blurred;
            blurred    = 0.25f * tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
	        blurred   += 0.25f * tex2D( g_TextureSampler[0], Input.TexCoord1.xy );
            blurred   += 0.25f * tex2D( g_TextureSampler[0], Input.TexCoord2.xy );
            blurred   += 0.25f * tex2D( g_TextureSampler[0], Input.TexCoord3.xy );
            
            FinalColor = saturate( blurred * (g_fRemananceFactor+1) );
            FinalColor = (FinalColor * g_fRemananceFactor) + tex2D( g_TextureSampler[1], Input.TexCoord4.xy );
        }
	}
	else if (iBlackWhiteId > 0)
	{
	    FinalColor.rgb = dot( tex2D( g_TextureSampler[0], Input.TexCoord0.xy ).rgb, float3(0.31f, 0.59f, 0.1f) );
	    FinalColor.a   = g_vConstantColor.a;
	}
	else if (iFogShaderId > 0)
	{
	    float fDepth = tex2D( g_TextureSampler[0], Input.TexCoord0.xy ).r;
	    
	    fDepth = min( fDepth, g_vFogNormalizing.z );
	    
	    // Convert z buffer value back to z in camera space
	    float fInvDepthInCameraSpace = (g_vFogNormalizing.x * fDepth) - g_vFogNormalizing.y;
	    float fDepthInCameraSpace = 1.0f / fInvDepthInCameraSpace;
        float fFog = saturate( (fDepthInCameraSpace * g_fFogInvDistance) - g_fFogNearOverDistance );
	   
	    // Pitch attenuation
	    if( iFogShaderId == 2 )
	    {
	        float3 vNormalizedWorldVector = normalize( Input.TexCoord1.xyz );
	        float fLerp = 1.0f - saturate( (vNormalizedWorldVector.z * g_vFogPitchAttenuation.y ) - g_vFogPitchAttenuation.x);
	        float fPitchAttenuation = 1 - (g_fFogPitchAttenuationIntensity * exp( 3.0f * -fLerp ));
	        fFog *= fPitchAttenuation;
	    }
	    	    
	    fFog = saturate( fFog * g_fFogDensity );
	    
	 	FinalColor.rgb = g_vFogColor;
		FinalColor.a   = fFog;
	}
    else if (iBlitShaderId == BLIT_SHADER_1)
    {
	    // Simple blt
	    FinalColor = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
	}
	else if( iColorDiffusion > 0 )
	{
	    if( iColorDiffusion == 1 )
	    {
	        // Stencil to mask
            FinalColor = float4(1,1,1,1);
	    }
        else if( iColorDiffusion == 2 || iColorDiffusion == 6)
	    {
	        FinalColor = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
	        
            float fDepth = tex2D( g_TextureSampler[1], Input.TexCoord0.xy ).r;
	        		   
	        // Convert z buffer value back to z in camera space
            fDepth = min( fDepth, g_vFogNormalizing.z );
	    
	        float fInvDepthInCameraSpace = (g_vFogNormalizing.x * fDepth) - g_vFogNormalizing.y;
	        float fDepthInCameraSpace = 1.0f / fInvDepthInCameraSpace;

	        float fDepthAttenuation = 1.0f - saturate( (fDepthInCameraSpace - g_fGlowZNear) / g_fGlowZDistance );
	        
	        float fLuminosity = dot( FinalColor, float4( 0.299, 0.587, 0.114, 0 ) );
            float fLumAttenuation = smoothstep( g_fGlowLuminosityMin, g_fGlowLuminosityMax, fLuminosity );
            
            FinalColor.rgb *= (fDepthAttenuation * fLumAttenuation);
            if( iColorDiffusion == 6 )
            {
                FinalColor.rgb *= FinalColor.a;
            }
        }
        else if( iColorDiffusion == 3 )
	    {
	        // Blit
            FinalColor = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
	    }
  	    else if( iColorDiffusion == 4 )
	    {
	        // 7-tap blur
            FinalColor    = g_vGlowBlurKernel[1] * tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
	        FinalColor   += g_vGlowBlurKernel[1] * tex2D( g_TextureSampler[0], Input.TexCoord1.xy );
            FinalColor   += g_vGlowBlurKernel[2] * tex2D( g_TextureSampler[0], Input.TexCoord2.xy );
            FinalColor   += g_vGlowBlurKernel[2] * tex2D( g_TextureSampler[0], Input.TexCoord3.xy );
            FinalColor   += g_vGlowBlurKernel[3] * tex2D( g_TextureSampler[0], Input.TexCoord4.xy );
            FinalColor   += g_vGlowBlurKernel[3] * tex2D( g_TextureSampler[0], Input.TexCoord5.xy );

       	    FinalColor.a  = g_vGlowBlurKernel[0];
	    }
        else if( iColorDiffusion == 5 )
	    {
            FinalColor  =  tex2D( g_TextureSampler[1], Input.TexCoord1.xy );
	        FinalColor +=  tex2D( g_TextureSampler[1], Input.TexCoord2.xy );
	        FinalColor +=  tex2D( g_TextureSampler[1], Input.TexCoord3.xy );
	        FinalColor +=  tex2D( g_TextureSampler[1], Input.TexCoord4.xy );
            
	        FinalColor *= g_fGlowIntensity;
	        
	        FinalColor = saturate(FinalColor) * g_vGlowColor;
	     
   	        FinalColor += tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
	    }
	    if( iColorDiffusion == 7 )
        {
            float4 color = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
            FinalColor = color.a;
        }
	}
	else if( iMotionBlur > 0 )
	{
	    FinalColor.rgb = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
	    FinalColor.a = g_fMotionBlurAlpha;
	}
	else if( iBigBlur > 0 )
	{
        FinalColor    = 0.25f * tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
	    FinalColor   += 0.25f * tex2D( g_TextureSampler[0], Input.TexCoord1.xy );
        FinalColor   += 0.25f * tex2D( g_TextureSampler[0], Input.TexCoord2.xy );
        FinalColor   += 0.25f * tex2D( g_TextureSampler[0], Input.TexCoord3.xy );
 	}
 	else if( iColorBalance > 0 )
 	{
 	    FinalColor = g_vColorBalance;
 	}
 	else if( iBKQuad > 0 )
 	{
 	    FinalColor = g_vConstantColor;
 	}
 	else if( iBorderBrightness > 0 )
 	{
 	    FinalColor = Input.Color; 	    
 	}
 	else if( iOldMovie > 0 )
 	{
 	    float4 BWCoeff = float4(0.11f, 0.59f, 0.3f, 0.0f);
 	    FinalColor = dot(BWCoeff, tex2D( g_TextureSampler[0], Input.TexCoord0.xy ));
 	    FinalColor *= float4(1.0f, 0.92f, 0.8f, 1.0f);
 	}
 	else if( iSpinSmooth > 0 )
 	{
        FinalColor.rgb = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
        FinalColor.a = 0.5f;
   	}
   	else if( iColorCorrection > 0 )
   	{
   	    FinalColor = tex2D( g_TextureSampler[0], Input.TexCoord0.xy );
   	    
   	    // Saturation 
   	    FinalColor = lerp( dot(FinalColor, float4(0.11f, 0.59f, 0.3f, 0)), FinalColor, g_fColorCorrectionSaturation );
   	    
   	    // Brightness
   	    FinalColor = saturate(FinalColor + g_vBrightness);
   	    
   	    // Contraste
        FinalColor = saturate( FinalColor * g_fColorCorrectionContrasteScale + g_fColorCorrectionContrasteTrans );
   	}

	return FinalColor;
}

// Test shader entry points for various configurations of shaders
float4 PSTest( PSIN Input ) : COLOR
{
    return ProcessPixel(Input, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
