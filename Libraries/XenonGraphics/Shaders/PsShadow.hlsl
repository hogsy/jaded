//-----------------------------------------------------------------------------
// Shadow pixel shader
//-----------------------------------------------------------------------------
//#include "ShaderCommon.hlsl"
#include "PsCommon.hlsl"
#include "..\XeSharedDefines.h"

struct PSIN
{
    float4 Position         : POSITION;
    float4 Color0           : COLOR0_centroid;
    float4 TC_Base          : TEXCOORD0_centroid;
    float4 TC_LSPos         : TEXCOORD1_centroid;
};

float4 ProcessPixel (PSIN Input, 
					int iShaderId, 
					int bLocalAlpha, 
					int bAlphaTest, 
					int iUnused4, 
					int iUnused5, 
					int iUnused6, 
					int iUnused7,
					int iUnused8,
                    int iUnused9,
                    int iUnused10,
                    int iUnused11,
                    int iUnused12,
                    int iUnused13,
                    int iUnused14,
                    int iUnused15)
{
    // Do our own alpha test because rendering to R32F disables alpha operations
    float fAlpha = tex2D(g_TextureSampler[0], Input.TC_Base).a;
    if(bAlphaTest)
    {
        if (bLocalAlpha)
        {
            fAlpha *= g_fLocalAlpha;
        }
        else
        {
            fAlpha *= Input.Color0.a;
        }
      
		// Alpha test is enabled        
		float fDelta = fAlpha - g_vAlphaTestParams.y;
		fDelta *= g_vAlphaTestParams.z;	// Apply alpha inversion
		clip(fDelta);            		
    }
    
	float Z = Input.TC_LSPos.z / Input.TC_LSPos.w;  // z / w 
	
    float4 FinalColor = float4(Z, Z, Z, Z);         // Output depth in R32F texture

	return FinalColor;
}
/*
float4 PSMain( PSIN Input ) : COLOR
{
    return ProcessPixel( Input, 0, 1, 1, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0 );
}
*/