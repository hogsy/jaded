// Common vertex shader definitions
#include "..\XeSharedDefines.h"


// must be kept in sync with the engine XeLight structure
struct XeLight
{
	float4 Position;
	float4 Direction;
	float4 Color;
	float4 Params;
};

struct TangentSpaceBasis
{
	float3 Tangent;
	float3 BiNormal;
	float3 Normal;
};

struct PPLightingOut
{
	float3 LightDir;
	float3 HalfWay;
	float  Attenuation;
};

uniform float4x4 g_mWorldViewProj;
uniform float4x3 g_mWorldView;
uniform float4x3 g_mInvTransWorldView;
uniform float4x3 g_mInvTransWorld;
uniform float4x4 g_mProjection;
uniform float4x3 g_mWorld;
uniform float4x3 g_mCamera;
uniform float4x3 g_amSkinning[VS_MAX_USED_BONES];
uniform int      g_iMaxWeight;
uniform float4x3 g_mTangentTransform;
uniform float4x2 g_mTextureTransform0;
uniform float4x2 g_mTextureTransform1;
uniform float4x2 g_mTextureTransform2;
uniform float4x2 g_mTextureTransform3;
uniform float4   g_vFogParams;
uniform float4	 g_vAmbientColor;
uniform float4   g_vMatDiffuseColor;
uniform float4   g_vMatConstantColor;
uniform float4   g_vMatSpecularColor;
uniform XeLight  g_aoLights[VS_MAX_USED_LIGHTS];
uniform int      g_iLightCounts[4];
uniform int      g_iLightBase;
uniform int	     g_aiPerPixelLight[VS_MAX_LIGHTS_PER_PASS];
uniform float4   g_vViewerPosition;
uniform float4	 g_vScalingXForm;
uniform float4   g_vTime;
uniform float4   g_vViewportUV;
uniform float4   g_vReflectionPlane;
uniform float4   g_vReflectionPlaneOrigin;
uniform float4   g_fRimLightHeightAttenuation;
uniform float4   g_vFogPitchAttenuation;
uniform float4   g_vFURParams;
uniform float4	 g_vMixed2;

// Definitions
#define g_fTime                             g_vTime.x
#define g_fTime_0_2Pi                       g_vTime.y
#define g_fViewportStartU                   g_vViewportUV.x
#define g_fViewportStartV                   g_vViewportUV.y
#define g_fViewportEndU                     g_vViewportUV.z
#define g_fViewportEndV                     g_vViewportUV.w

#define g_fFogPitchAttenuationIntensity     g_vFogPitchAttenuation.z
#define g_fFogNear                          g_vFogParams.x
#define g_fFogDensity                       g_vFogParams.z
#define g_fFogDistance                      g_vFogParams.w

#define g_iDirLightCount                    g_iLightCounts[0]
#define g_iOmniLightCount                   g_iLightCounts[1]
#define g_iSpotLightCount                   g_iLightCounts[2]
#define g_iCylSpotLightCount                g_iLightCounts[3]

#define g_fRimLightHeightWorldMin           g_fRimLightHeightAttenuation.x 
#define g_fRimLightHeightWorldMax           g_fRimLightHeightAttenuation.y 
#define g_fRimLightHeightAttenuationMin     g_fRimLightHeightAttenuation.z 
#define g_fRimLightHeightAttenuationMax     g_fRimLightHeightAttenuation.w 

#define g_fGlobalRLIScale                   g_vMixed2.x     
#define g_fGlobalRLIOffset                  g_vMixed2.y 
#define g_fShininess                        g_vMixed2.z 

#define g_fFurOffsetScale					g_vFURParams.x
#define g_fFurNormalOffset 					g_vFURParams.y
#define g_vFurTextureOffset					g_vFURParams.zw
