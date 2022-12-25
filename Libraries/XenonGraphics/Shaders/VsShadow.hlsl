//-----------------------------------------------------------------------------
// Shadow vertex shader
//-----------------------------------------------------------------------------
#include "..\XeSharedDefines.h"

struct VSOUT
{
    float4 Position         : POSITION;
    float4 Color0           : COLOR0;
    float4 TC_Base          : TEXCOORD0;
    float4 TC_LSPos         : TEXCOORD1;
    float4 TC_Cookie        : TEXCOORD2;
    float4 TC_TEST          : TEXCOORD3;
};

#include "VsCommon.hlsl"

uniform float4x4    g_vShadowNearFar;
uniform float4      g_vResolution;

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
    float4 Tangent          : TANGENT;
#endif
};

VSOUT ProcessVertex(VSIN Input,
                    int iTransform, 
                    int iShadowMode,        // "Render to shadow buffer" (0) or "Apply" (1)
                    int iUnused3, 
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
    VSOUT Output = (VSOUT)0;

    float4x3    mSkinning;
    float4      vModelSpacePosition;
    float3      vModelSpaceNormal;

    // transform position
    if( iTransform == TRANSFORM_NORMAL )
    {
        vModelSpacePosition = Input.Position;
        Output.Position = mul( vModelSpacePosition, g_mWorldViewProj );
    }
    else if( iTransform == TRANSFORM_SKINNING )
    {
        if ( g_iMaxWeight > 0)
        {
            mSkinning = mul(g_amSkinning[Input.BlendIndices[0]], Input.BlendWeights[0]);

            for (int i=1; i < g_iMaxWeight; i++)
            {
                mSkinning += mul(g_amSkinning[Input.BlendIndices[i]], Input.BlendWeights[i]);
            }

            // skin vertex
            vModelSpacePosition.xyz = mul(Input.Position, mSkinning );
            vModelSpacePosition.w = 1.0;

            // transform to screen space
            Output.Position = mul(vModelSpacePosition, g_mWorldViewProj);
        }
        else
        { 
            // transform to screen space
            vModelSpacePosition = Input.Position;
            Output.Position = mul(vModelSpacePosition, g_mWorldViewProj);       
        }
    }
    else
    {
        // No transform!  Use model space position.
        vModelSpacePosition = Input.Position;
        Output.Position = vModelSpacePosition;
    }

    if(iShadowMode == 0)
    {
        // Render to shadow buffer
#ifndef VS_NO_TEX_COORD        
        Output.TC_Base = float4(Input.TexCoord0, 0, 0);         // Base texture UV
#else
		Output.TC_Base = float4(0, 0, 0, 0);
#endif        
        Output.TC_LSPos = Output.Position;                      // Position in light space (Output Z to shadowbuffer)
    }
    else
    {
        // Apply shadow buffer mode
        // Apply texture transform matrix
        //Output.TC_Base   = mul(vModelSpacePosition, g_mTextureTransform0);  // Shadow buffer UV
        //Output.TC_LSPos  = mul(vModelSpacePosition, g_mTextureTransform1);  // Position in light space 
        //Output.TC_Cookie = mul(vModelSpacePosition, g_mTextureTransform2);  // Cookie texture UV
        
#ifndef VS_NO_TEX_COORD                
        Output.TC_TEST = float4(Input.TexCoord0, 0, 0);
#else
		Output.TC_TEST = float4(0, 0, 0, 0);
#endif
            }

  #ifndef VS_NO_COLOR
    Output.Color0 = Input.RLI;
  #else
    Output.Color0 = float4(0.0, 0.0, 0.0, 1.0f);
  #endif
  
    return Output;
}