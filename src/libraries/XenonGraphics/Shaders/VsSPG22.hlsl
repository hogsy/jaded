//-----------------------------------------------------------------------------
// SPG2 vertex shader
//-----------------------------------------------------------------------------

#define VS_EXTRA_TEX_COORD
#include "VsGeneric.hlsl"

uniform float4   g_vSPG2UVManip;
uniform float4   g_vSPG2GlobalPos;
uniform float4   g_vSPG2GlobalZAdd;
uniform float4   g_avSPG2Wind[8];
uniform float4   g_vSPG2XCam;
uniform float4   g_vSPG2YCam;
uniform float4   g_vSPG2GlobalColorWind;
//uniform float4   g_vSPG2Mixed1;
//uniform float4   g_vSPG2Mixed2;

uniform float    g_fSPG2Ratio; // dynamicaly updated in grid, doesn't work well when packed in a mixed variable
uniform float    g_fSPG2GlobalScale; // dynamicaly updated in grid, doesn't work well when packed in a mixed variable

uniform float    g_fSPG2OoNumOfSeg;
uniform float    g_fSPG2Trapeze;
uniform float    g_fSPG2TrapezeDelta;
uniform float    g_fSPG2EOHP;
uniform float    g_fSPG2SpriteRadius;

/*
#define g_fSPG2OoNumOfSeg		g_vSPG2Mixed1.y
#define g_fSPG2Trapeze			g_vSPG2Mixed1.w

#define g_fSPG2TrapezeDelta		g_vSPG2Mixed2.x
#define g_fSPG2EOHP				g_vSPG2Mixed2.y
#define g_fSPG2SpriteRadius		g_vSPG2Mixed2.z
*/

VSOUT ProcessVertexSprites( VSIN Input )
{
    VSOUT Output = (VSOUT)0;    

    float4		vLocalX;
    float4		vLocalY;
    float4		vCamX = lerp(float4(Input.TexCoord1.x, Input.TexCoord1.y, Input.TexCoord2.x, 1.0), g_vSPG2XCam, g_fSPG2Trapeze);
    float4		vCamY = lerp(float4(Input.TexCoord2.y, Input.TexCoord2.z, Input.TexCoord2.w, 1.0), g_vSPG2YCam, g_fSPG2Trapeze);
    float4		vPos0 = float4(Input.Position.xyz, 1.0);
    float4		vPos1 = float4(Input.Tangent.xyz,1.0);
	float4		vPos2 = Input.BlendWeights;
	float3		vNrm  = Input.Normal;
	
	float		vPos2W = Input.Position.w;
		
    float		fComplexRadius_IM = Input.TexCoord0.y;
    
    float       fYMod = frac(Input.TexCoord0.x);
    int		    iSegmentIdx = (int)Input.TexCoord0.x;
    
    vPos0.xyz += g_vSPG2GlobalPos.xyz;
    
	vNrm.xyz += g_vSPG2GlobalZAdd.xyz;
	vNrm.xyz *= g_fSPG2GlobalScale;
	vNrm.xyz *= (vPos2W*g_fSPG2OoNumOfSeg);
	
	vPos1.xyz *= (vPos2W * g_fSPG2SpriteRadius * g_fSPG2Ratio);
	vPos2.xyz *= (vPos2W * g_fSPG2SpriteRadius * g_fSPG2Ratio);
	
	vLocalX.xyz = vNrm.xyz * g_fSPG2EOHP;
	
	vPos0.xyz += vLocalX.xyz / g_fSPG2OoNumOfSeg;

	vNrm.xyz *= (1.0 - g_fSPG2EOHP);
	
	float3 vDerivative = iSegmentIdx*vNrm;

	// add derivative factor			
	vPos0.xyz += vDerivative.xyz;
	
	vPos0.xyz = vPos0 + (vPos1 * fComplexRadius_IM);
	vPos0.xyz -= (vCamX * 0.5);
	vPos0.xyz -= (vCamY * 0.5);
	
	vPos0.xyz += vPos2.w*vCamX;
	vPos0.xyz += fYMod*vCamY;
		
	Output.Position = vPos0;
	
	Output.TexCoord0.x = 1.0 - vPos2.w;
	Output.TexCoord0.y = 1.0 - fYMod;

	float3 vTangent = normalize(vCamX); 
	float3 vBinormal = normalize(vCamY);
	float3 vNormal = cross(vTangent, vBinormal);
	
	Output.HalfWay[1].xyz = vTangent;
	Output.HalfWay[0].xyz = vNormal;
	
    return Output;
}

VSOUT ProcessCustomVertexMAT( VSIN Input, 
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
                     int  iSPG2Mode				= SPG2_MODE_NONE,
                     bool bShadows				= SHADOW_OFF,
                     bool bVertexSpecular	    = VERTEX_SPECULAR_OFF)
{
	int iMode = iSPG2Mode;
	float3 vTangent;

	VSOUT Output = (VSOUT)0;
    VSIN  InputGeneric = (VSIN)0;
    	
	if(iMode==SPG2_MODE_SPRITES)
	{
		Output = ProcessVertexSprites( Input );
	}
	else
	{
		float4		vPos0 = float4(Input.Position.xyz, 1.0);
		float4		vPos1 = float4(0,0,0,1);
		
		float3		vNrm  = Input.Normal;
		float3		vDerivative;
		float4		vGravity;
		float4		vTrapeze;
		float4		vTrapezeHat;
		float3      vDelta;
		float3      vDelta2;
		
		float		vPos2W = Input.Position.w;
		
		float		fSegmentIdx = (float)floor(Input.TexCoord0.x/10.0);
		int			iVertexSelector = floor(Input.TexCoord0.x-((fSegmentIdx*10.0)));

		float		fTrapezeInc=0.0;
		float		fWindIdx = (float)floor(Input.TexCoord0.y/1000.0);	
		float		fPrimitiveIdx = (float)floor(Input.TexCoord0.y-(fWindIdx*1000.0));	
		
		float		fBaseU = 0.0;
		float		fBaseV = 0.0;
		
		if(iMode==SPG2_MODE_SPECIALX)
		{
			fBaseU = g_vSPG2UVManip.z*g_vSPG2UVManip.x;
			fBaseV = (1.0-(((int)(fBaseU+1.0001))*g_vSPG2UVManip.w));
		}
		
		fWindIdx = lerp(fWindIdx, g_vSPG2GlobalColorWind.w, saturate(g_vSPG2GlobalColorWind.w));

		float		fU = fBaseU + (iVertexSelector*g_vSPG2UVManip.z);
		float		fV = fBaseV;
		float		fDeltaU = g_vSPG2UVManip.y;
		
		vPos0.xyz += g_vSPG2GlobalPos.xyz;

		vNrm.xyz += g_vSPG2GlobalZAdd.xyz;
		vNrm.xyz *= g_fSPG2GlobalScale;
		vNrm.xyz *= (vPos2W*g_fSPG2OoNumOfSeg);

		vGravity = g_avSPG2Wind[fWindIdx];
		vGravity.xyz *= (vPos2W*g_fSPG2OoNumOfSeg);
		
		float fInterpolatorIntensity = vGravity.w * g_fSPG2OoNumOfSeg;

		if(iMode==SPG2_MODE_Y) // y mode
		{
			vPos0.xyz += Input.Tangent.xyz * (-0.5 * vPos2W * g_fSPG2Ratio);
			vPos1.xyz = vPos0.xyz + Input.Tangent.xyz * (vPos2W * g_fSPG2Ratio);
			fTrapezeInc = (g_fSPG2Trapeze + (g_fSPG2TrapezeDelta*fSegmentIdx));
		}
		else if(iMode==SPG2_MODE_X) // x mode
		{
			float3 vPos2;
			
			vPos2.xyz = Input.BlendWeights.xyz;
			vPos0.xyz += vPos2.xyz * (-0.5 * vPos2W * g_fSPG2Ratio);
			vPos1.xyz = vPos0.xyz + vPos2.xyz * (vPos2W * g_fSPG2Ratio);
			fTrapezeInc = (g_fSPG2Trapeze + (g_fSPG2TrapezeDelta*fSegmentIdx));
		}
		else if(iMode==SPG2_MODE_SPECIALX)
		{
			float3 vPos2;
			
			// g_vSPG2XCam is the precomputed camera direction (xcam x ycam)
			vPos2.xyz = normalize(cross(float4(g_vSPG2XCam.xyz, 1.0), float4(vNrm.xyz, 1.0))).xyz;
			vPos0.xyz += vPos2.xyz * (-0.5 * vPos2W * g_fSPG2Ratio);
			vPos1.xyz = vPos0.xyz + vPos2.xyz * (vPos2W * g_fSPG2Ratio);
			fTrapezeInc = (g_fSPG2Trapeze + (g_fSPG2TrapezeDelta*fSegmentIdx));
		}
		else if(iMode==SPG2_MODE_HAT) // hat mode
		{
			vTrapezeHat.xyz = vGravity.xyz - vNrm.xyz;
			vTrapezeHat.xyz *= (fInterpolatorIntensity*g_fSPG2EOHP*g_fSPG2EOHP) / (2.0 * g_fSPG2OoNumOfSeg);
			vTrapezeHat.xyz += (vNrm.xyz * g_fSPG2EOHP);
			vPos0.xyz += (vTrapezeHat.xyz/g_fSPG2OoNumOfSeg);
			vNrm.xyz = Input.BlendWeights.xyz * (vPos2W * g_fSPG2Ratio);
			vGravity.xyz = vNrm;
				
			vDelta = Input.BlendWeights.xyz*(0.5 * vPos2W * g_fSPG2Ratio);
			vDelta2 = float4(Input.Tangent.xyz,1.0)*(0.5 * vPos2W * g_fSPG2Ratio);
			
			vPos0.xyz = vPos0.xyz-vDelta2.xyz;	
			vPos0.xyz = vPos0.xyz-vDelta.xyz;	
			vPos1.xyz = vPos0.xyz+(2*vDelta2.xyz);
			fTrapezeInc = (g_fSPG2TrapezeDelta*fSegmentIdx);
		}
			
		fV += (fDeltaU * fSegmentIdx);
		
		float fInterpolatorPosSum = fInterpolatorIntensity*(fSegmentIdx*(fSegmentIdx-1))/(2.0);
		float3 vNormalSum = (fSegmentIdx - fInterpolatorPosSum) * vNrm;
		float3 vGravitySum = fInterpolatorPosSum * vGravity.xyz;
		
		vDerivative = vNormalSum+vGravitySum;
		
		float fInterpolatorPosSum2 = fInterpolatorIntensity*((fSegmentIdx-1)*max(0.0,(fSegmentIdx-2)))/(2.0);
		float3 vNormalSum2 = ((fSegmentIdx-1.0) - fInterpolatorPosSum2) * vNrm;
		float3 vGravitySum2 = fInterpolatorPosSum2 * vGravity.xyz;
		
		float3 vDerivative2 = vNormalSum2+vGravitySum2;

		// add derivative factor			
		vPos0.xyz += vDerivative.xyz;
		vPos1.xyz += vDerivative.xyz;
				
		vTrapeze = vPos0 - vPos1;
		float4 vT0 = vPos0 + (vTrapeze * fTrapezeInc);
		float4 vT1 = vPos1 + (vTrapeze * -fTrapezeInc);
		vTrapeze = (vT0 * iVertexSelector) + ((1.0-iVertexSelector)*vT1);
		vTrapeze.w = vPos0.w;
		
		// transform position
		Output.Position = vTrapeze;
	    
		float3 vBinormal = normalize(vDerivative.xyz-vDerivative2.xyz);
			
		vTangent = normalize(vT1-vT0); 
		float3 vNormal = cross(vTangent, vBinormal);
		Output.HalfWay[0].xyz = vNormal.xyz;
		Output.HalfWay[1].xyz = vTangent.xyz;
		
		Output.TexCoord1.x = fSegmentIdx / g_fSPG2OoNumOfSeg;
		Output.TexCoord1.y = 1.0-iVertexSelector;
		
		// UV
		Output.TexCoord0.xy = float2(fU, fV);
    }
    
    InputGeneric.Position = float4(Output.Position.xyz, 1);
    InputGeneric.Normal.xyz = Output.HalfWay[0];
    
	InputGeneric.RLI = lerp(Input.RLI, float4(g_vSPG2GlobalColorWind.xyz, 1.0), saturate(g_vSPG2GlobalColorWind.w));
    InputGeneric.TexCoord0 = Output.TexCoord0;
    InputGeneric.Tangent = float4(Output.HalfWay[1].xyz, 1);
    
    Output = ProcessVertex( InputGeneric, 
                     1,	        
                     iLighting,		    
					 iDirLightCount, 
					 iOmniLightCount, 
					 iSpotLightCount, 
					 iCylSpotLightCount,
                     iColorSource,       
                     iBTexTransform,     
                     iUVSource,		    
                     bFog,			    
                     iNTexTransform,		
                     iSTexTransform,		
                     iDNTexTransform,	
                     bAddAmbient,		
                     bAddRLI,			
					 bRLIScaleAndOffset,	
                     iReflectionType,	
                     bRimLightEnabled,	
                     bRimLightHeightAttenuation, 
                     bDepthToColorEnable,		
                     bInvertMoss,				
                     iSPG2Mode,
                     bShadows,
                     bVertexSpecular);
                   
    return Output;
}


VSOUT VSTest( VSIN Input )
{
    return ProcessCustomVertexMAT( Input, 1, 3, 0, 2, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 4, 0, 0);
}