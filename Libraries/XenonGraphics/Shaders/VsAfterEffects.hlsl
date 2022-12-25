//-----------------------------------------------------------------------------
// After Effects vertex shader
//-----------------------------------------------------------------------------

struct VSIN
{
    float4 Position         : POSITION;
    float4 Color			: COLOR0;
    float2 TexCoord0        : TEXCOORD0;
    float2 TexCoord1        : TEXCOORD1;
};

struct VSOUT
{
    float4 Position         : POSITION;
    float4 Color			: COLOR0;
    float3 TexCoord0        : TEXCOORD0;
    float3 TexCoord1        : TEXCOORD1;
    float3 TexCoord2        : TEXCOORD2;
    float3 TexCoord3        : TEXCOORD3;
    float3 TexCoord4        : TEXCOORD4;
    float3 TexCoord5        : TEXCOORD5;
};

uniform float4      g_vViewportUV;
uniform float4      g_vHalfViewportTexel;

uniform float4      g_vZoomBlurScale[3];
uniform float4      g_vZoomBlurTranslate[3];
uniform float       g_fZoomBlurAlpha[3];

uniform float4      g_vZoomBlurRatios;
#define g_fZoomBlurInputRatio   g_vZoomBlurRatios.x
#define g_fZoomBlurOutputRatio  g_vZoomBlurRatios.y

uniform int         g_iAEGlowInputRegion;
uniform int         g_iAEGlowOutputRegion;
uniform float4      g_vAEGlowBlurDirection;

uniform float4      g_vFogProjection; // width, height, near plane
uniform float4x3    g_mCamera;

uniform float       g_fBigBlurFactor;
uniform float4      g_vGodRayAdjust;

uniform float4      g_vBorderBrightness;
uniform float4      g_vResolution;

uniform float4x4    g_mSpinSmoothTransform;

//
// Scales and offsets for the color diffusion after effect
//
float GetGlowRegionScale( in int iRegion )
{
    return 1 / pow(2,iRegion);
}

float GetGlowRegionOffset( in int iRegion )
{
    float n = max( iRegion-1, 0 );
    return (1 - GetGlowRegionScale( n ));
}

float3 GetGlowRegionTexCoords( in int iRegion, in float2 texCoords )
{
    float2 vViewportPos = g_vViewportUV.xy;
    float2 vViewportSize = g_vViewportUV.zw - vViewportPos;
    
    float fScale  = GetGlowRegionScale( iRegion );
    float fOffset = GetGlowRegionOffset( iRegion );
    
    // Compute uv taking into account the viewport (in case we are in the editor) and the half viewport texel
    // to sample center of pixel
    float2 uv = ( texCoords * vViewportSize * fScale ) +  
                ( vViewportPos + (fOffset * vViewportSize) + g_vHalfViewportTexel.xy );
                
    return float3( uv, 0);
}

float4 GetGlowRegionPosition( in int iRegion, in float4 position )
{
    float fScale  = GetGlowRegionScale( iRegion );
    float fOffset = GetGlowRegionOffset( iRegion );

    // Compute position of the region
    // Note: Result = ([Flip Y & normalize -1..1]*[Translate]*[Scale]*[Flip Y & normalize 0..1]) * [xy]
    float2 pos;
    pos.x = position.x * fScale + ( fScale + (2.0f * fOffset) - 1);
    pos.y = position.y * fScale + (-fScale - (2.0f * fOffset) + 1);

    return float4( pos, 0, 1 );
}

void GodRaySetParams( inout VSOUT Output, int index, in float2 texCoords, in float fAlpha )
{
    float2 tex = (texCoords - g_vViewportUV.xy) * g_fZoomBlurInputRatio + g_vViewportUV.xy;
    if      ( index == 0 ) Output.TexCoord0 = float3( tex, fAlpha );
    else if ( index == 1 ) Output.TexCoord1 = float3( tex, fAlpha );
    else if ( index == 2 ) Output.TexCoord2 = float3( tex, fAlpha );
    else if ( index == 3 ) Output.TexCoord3 = float3( tex, fAlpha );
}

#define M_GodRayPass( step, prev )                                                                  \
void GodRayPass_##step( inout VSOUT Output, float2 texCoords, int index1, int index2, float mul )   \
{                                                                                                   \
    float2 PassUV = (texCoords * g_vZoomBlurScale[step]) + g_vZoomBlurTranslate[step];              \
    float a = g_fZoomBlurAlpha[step];                                                               \
    if( step == 0 )                                                                                 \
    {                                                                                               \
        GodRaySetParams( Output, index1, texCoords, (1-a) * mul );                                  \
        GodRaySetParams( Output, index2, PassUV,    a * mul );                                      \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
        GodRayPass_##prev( Output, texCoords, index1, index1 + step, (1-a)*mul  );                  \
        GodRayPass_##prev( Output, PassUV, index2, index2 + step, a*mul );                          \
    }                                                                                               \
 }

M_GodRayPass( 0, 0 )
M_GodRayPass( 1, 0 )
M_GodRayPass( 2, 1 )

VSOUT ProcessVertex(VSIN Input,
					int iGodRayShaderId, 
					int iFogShaderId, 
					int iBlitShaderId, 
					int iColorDiffusion, 
					int iBigBlur, 
					int iRemanance, 
					int iXInvert,
					int iWideScreen,
					int iBorderBrightness,
					int iOldMovie,
					int iSpinSmooth,
					int iUnused12,
					int iUnused13,
					int iUnused14,
					int iUnused15)
{
    VSOUT Output = (VSOUT)0;
    
    if (iGodRayShaderId > 0)
    {
        // Scaled output
        Output.Position.zw = float2(0.99999999999999f,1);
        Output.Position.x = Input.Position.x * g_fZoomBlurOutputRatio + (g_fZoomBlurOutputRatio - 1);
        Output.Position.y = Input.Position.y * g_fZoomBlurOutputRatio + (-g_fZoomBlurOutputRatio + 1);
        Output.Position.x -= 2*g_vHalfViewportTexel.x;
        Output.Position.y += 2*g_vHalfViewportTexel.y;
        
        float2 vViewportTexCoord = lerp( g_vViewportUV.xy, g_vViewportUV.zw, Input.TexCoord0.xy );
     	
    	if( iGodRayShaderId == 2 || iGodRayShaderId == 6 )
    	{
    	    GodRayPass_1( Output, vViewportTexCoord, 0, 2, 1 );
    	    
    	    Output.Color = 1;
    	    if( iGodRayShaderId == 6 )
    	    {
    	        Output.Color = float4(g_vGodRayAdjust.xyz * Input.Color.a, Input.Color.a);
    	    }
        }
	    else 
        {
            float2 vScaledViewportTexCoord = (vViewportTexCoord - g_vViewportUV.xy) * g_fZoomBlurInputRatio + g_vViewportUV.xy;
  
            if( iGodRayShaderId == 3 )
            {
                GodRayPass_0( Output, vViewportTexCoord, 0, 1, 1 );
                float factor = g_fZoomBlurAlpha[1] * (g_fZoomBlurAlpha[1] + 1.0f);
		        Output.TexCoord2  = float3( vScaledViewportTexCoord, factor );
            }
            else if( iGodRayShaderId == 4 )
            {
                // Scaled input
                Output.TexCoord0 = float3( vScaledViewportTexCoord, 0 );
            }
            else if( iGodRayShaderId == 5 )
            {
                GodRayPass_0( Output, vViewportTexCoord, 0, 1, 1 );
                float factor = g_fZoomBlurAlpha[1] * (g_fZoomBlurAlpha[1] + 1.0f);
		        Output.TexCoord2 = float3( vViewportTexCoord, factor );
            }
        }
 	}
	 
    else if (iFogShaderId > 0)
    {
        // Adjust for viewport
   	    Output.TexCoord0.xy = lerp( g_vViewportUV.xy, g_vViewportUV.zw, Input.TexCoord0.xy );
   	    
        Output.Position		= Input.Position;
        Output.Color		= Input.Color;
 			
 		float2 vPoint = Input.TexCoord0.xy;
 		
 		// Compute vector in camera space on the near plane
 		float4 vCameraSpaceVector = float4( ((vPoint.xy - 0.5f) * g_vFogProjection.xy), g_vFogProjection.z, 0 );
 		vCameraSpaceVector = normalize(vCameraSpaceVector);
 		
 		// Transform in world space
 		Output.TexCoord1.xyz = mul( vCameraSpaceVector, g_mCamera );
 	} 
	else if( iBlitShaderId > 0 )
	{
        Output.Position      = Input.Position;
        Output.TexCoord0.xy  = lerp( g_vViewportUV.xy, g_vViewportUV.zw, Input.TexCoord0.xy );
        if( iBlitShaderId == 1 )
        {
    	    Output.TexCoord0.xy += g_vHalfViewportTexel;
        }
	}
	else if( iColorDiffusion > 0 )
    {
        if( iColorDiffusion == 1 )
        {
            Output.TexCoord0    = GetGlowRegionTexCoords( g_iAEGlowInputRegion, Input.TexCoord0.xy );
            Output.Position	    = GetGlowRegionPosition( g_iAEGlowOutputRegion, Input.Position );
        }
        else if( iColorDiffusion == 2 )
        {
            // Blur
            Output.Position    = GetGlowRegionPosition( g_iAEGlowOutputRegion, Input.Position );
            float3 texCoords   = GetGlowRegionTexCoords( g_iAEGlowInputRegion, Input.TexCoord0.xy );
            float3 pixelOffset = float3( 2.0f * g_vHalfViewportTexel.xy * g_vAEGlowBlurDirection.xy, 0 );
            float3 extraOffset = float3( g_vHalfViewportTexel.xy * g_vAEGlowBlurDirection.xy, 0 );
            
            // Always tap between two texels to get extra blur from filtering
            Output.TexCoord0  = texCoords - pixelOffset - extraOffset;
            Output.TexCoord1  = texCoords + pixelOffset + extraOffset;
            
            Output.TexCoord2  = texCoords - (2 * pixelOffset) - extraOffset;
            Output.TexCoord3  = texCoords + (2 * pixelOffset) + extraOffset;
            
            Output.TexCoord4  = texCoords - (3 * pixelOffset) - extraOffset;
            Output.TexCoord5  = texCoords + (3 * pixelOffset) + extraOffset;
        }
        else if( iColorDiffusion == 3 )
        {
            Output.Position      = Input.Position;
            Output.TexCoord0.xy  = lerp( g_vViewportUV.xy, g_vViewportUV.zw, Input.TexCoord0.xy );
    	    Output.TexCoord0.xy += g_vHalfViewportTexel;
    	    
            // Combine all 4 region starting with input region
            Output.TexCoord1  = GetGlowRegionTexCoords( g_iAEGlowInputRegion + 0, Input.TexCoord0.xy );
            Output.TexCoord2  = GetGlowRegionTexCoords( g_iAEGlowInputRegion + 1, Input.TexCoord0.xy );
            Output.TexCoord3  = GetGlowRegionTexCoords( g_iAEGlowInputRegion + 2, Input.TexCoord0.xy );
            Output.TexCoord4  = GetGlowRegionTexCoords( g_iAEGlowInputRegion + 3, Input.TexCoord0.xy );
        }
    }
 	else if( iBigBlur > 0 || iRemanance > 0 )
 	{
        float2 vOffset = g_fBigBlurFactor;
        
        Output.Position      = Input.Position;
        float2 vOriginal     = lerp( g_vViewportUV.xy, g_vViewportUV.zw, Input.TexCoord0.xy );
    	
    	Output.TexCoord0.xyz = float3( vOriginal + float2( vOffset.x, 0 ), 0 );
    	Output.TexCoord1.xyz = float3( vOriginal + float2( 0, vOffset.y ), 0 );
    	Output.TexCoord2.xyz = float3( vOriginal + float2( -vOffset.x, 0 ), 0 );
    	Output.TexCoord3.xyz = float3( vOriginal + float2( 0, -vOffset.y ), 0 );
    	
    	if( iRemanance > 0 )
    	{
    	    Output.TexCoord4.xyz =  float3( vOriginal, 0 );
    	}
    }
    else if( iXInvert > 0 )
    {
        Output.Position = Input.Position;

        float2 vTexCoords = Input.TexCoord0.xy;
        vTexCoords.x = 1 - vTexCoords.x;
        Output.TexCoord0.xy = lerp( g_vViewportUV.xy, g_vViewportUV.zw, vTexCoords.xy );
    }
    else if( iWideScreen > 0 )
    {
        Output.Position.xy = Input.Position.xy;
        Output.Position.x -= 2*g_vHalfViewportTexel.x;
        Output.Position.y += 2*g_vHalfViewportTexel.y;
        Output.Position.zw = float2(0,1);
    }
    else if( iBorderBrightness )
    {
        Output.Position.xy = Input.Position.xy;
        Output.Position.x -= 2*g_vHalfViewportTexel.x;
        Output.Position.y += 2*g_vHalfViewportTexel.y;
        Output.Position.zw = float2(0,1);
        
        float x = Input.Position.x;
        float y = Input.Position.y * 1.33f;
        
        Output.Color.rgb = g_vBorderBrightness.rgb;
        Output.Color.a = saturate(1.0f - ( ((x*x)+(y*y)) * g_vBorderBrightness.a));
    }

    else if( iOldMovie > 0 )
    {
        // Adjust for viewport
        Output.Position		= Input.Position;
        Output.Color		= Input.Color;    
   	    Output.TexCoord0.xy = lerp( g_vViewportUV.xy, g_vViewportUV.zw, Input.TexCoord0.xy );
    }
    else if( iSpinSmooth > 0 )
    {
        // Adjust for viewport
        Output.Position		= mul( Input.Position, g_mSpinSmoothTransform );
   	    Output.TexCoord0.xy = lerp( g_vViewportUV.xy, g_vViewportUV.zw, Input.TexCoord0.xy );
        Output.TexCoord0.xy = (Output.TexCoord0.xy - g_vViewportUV.xy) * g_fZoomBlurInputRatio + g_vViewportUV.xy;
    }
	return Output;
}

// Test shader entry points for various configurations of shaders
VSOUT VSTest( VSIN Input )
{
    return ProcessVertex( Input, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
}
