//-----------------------------------------------------------------------------
// Heat shimmmer vertex shader
//-----------------------------------------------------------------------------

#include "..\XeSharedDefines.h"


uniform float4   g_vViewportUV;
uniform float4x4 g_mHeatShimmerProj;
uniform float4   g_vHeatShimmerCenter;
uniform float4   g_vHeatShimmerShape;
#define g_fHeatShimmerWidth     g_vHeatShimmerShape.x
#define g_fHeatShimmerHeight    g_vHeatShimmerShape.y
#define g_fHeatShimmerSizeU     g_vHeatShimmerShape.z
#define g_fHeatShimmerSizeV     g_vHeatShimmerShape.w

uniform float4   g_vHeatShimmerParams;
#define g_fHeatShimmerScrollV       g_vHeatShimmerParams.x
#define g_fHeatShimmerBottomScale   g_vHeatShimmerParams.y
#define g_fHeatShimmerIntensity     g_vHeatShimmerParams.z

struct VSIN
{
    float4 Position         : POSITION;
};

struct VSOUT
{
    float4 Position         : POSITION;
    float4 Color0           : COLOR0;
    float4 TexCoord0        : TEXCOORD0;
    float4 TexCoord1        : TEXCOORD1;
};

VSOUT ProcessVertex( VSIN Input,	
                               int iPassType = HEAT_SHIMMER_MASK_PASS, 
					           int iUnused2  = 0, 
					           int iUnused3  = 0, 
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
					           int iUnused15 = 0)
{
    VSOUT Output = (VSOUT)0;
 
    float4 vPosition = Input.Position;
    
    // Bottom scaling
    if( vPosition.y > 0.0f )
    {
        vPosition.y *= g_fHeatShimmerBottomScale;
    }
 
    float4 vCenter =  g_vHeatShimmerCenter;
    float2 vSize = g_vHeatShimmerShape.xy;
    
    float4 vCameraSpacePosition = vCenter;
    float2 vDistance = vSize * vPosition.xy;
    vCameraSpacePosition.xy += vDistance;
    
    float4 vProjectedPosition = mul( vCameraSpacePosition, g_mHeatShimmerProj );
    Output.Position = vProjectedPosition;
        
    if( iPassType == HEAT_SHIMMER_OFFSET_PASS )
    {
        // Compute stage 0 tex coords
        float2 vTexSize = g_vHeatShimmerShape.zw;
        Output.TexCoord0.xy = vTexSize * vPosition;
        Output.TexCoord0.y += g_fHeatShimmerScrollV;
        
        // Compute stage 1 tex coords (Projected position converted to [0,1])
        vProjectedPosition /= vProjectedPosition.w;
        Output.TexCoord1.x = (vProjectedPosition.x + 1.0f) * 0.5f;
        Output.TexCoord1.y = (-vProjectedPosition.y + 1.0f) * 0.5f;
        Output.TexCoord1.zw = float2(1,1);
        
        // Adjust for viewport
        float2 vViewportPos = g_vViewportUV.xy;
        float2 vViewportSize = g_vViewportUV.zw - vViewportPos;
        Output.TexCoord1.xy = (Output.TexCoord1.xy * vViewportSize) + vViewportPos;
        
        // Alpha is in z component of position
        float4 vColorOut = float4(1.0f, 1.0f, 1.0f, g_fHeatShimmerIntensity * Input.Position.z );
        Output.Color0 = vColorOut;
    }
    else if( iPassType == 2 )
    {
        // Alpha is in z component of position
        float4 vColorOut = float4(1.0f, 1.0f, 1.0f, g_fHeatShimmerIntensity * Input.Position.z );
        Output.Color0 = vColorOut;
        
        vProjectedPosition /= vProjectedPosition.w;
        vProjectedPosition.z = Output.Color0.a;
        Output.Position = vProjectedPosition;
    }
    else if( iPassType == 3 )
    {   
        Output.Color0 = float4(0.0f, 1.0f, 0.0f, 1.0f);
    }
   
    return Output;
}

// Test shader entry points for various configurations of shaders
VSOUT TestVS( VSIN Input )
{
    return ProcessVertex(Input, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}