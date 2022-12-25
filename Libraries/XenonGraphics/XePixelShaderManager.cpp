#include "precomp.h"

#include "XePixelShaderManager.h"
#include "XeRenderer.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"
#include "XeLightShaftManager.h"
#include "XeShadowManager.h"
#include "XeUtils.h"
#include "XeWaterManager.h"
#include "XeWeatherManager.h"
#include "XeRenderer.h"
#include "XeShaderDatabase.h"

#include "DevHelpers/XeBench.h"
#include "DevHelpers/XePerf.h"

#if defined(_DEBUG)
#define XE_PS_VERBOSE
#endif

// unique instance of the pixel shader manager
XePixelShaderManager g_oPixelShaderMgr;
LPDIRECT3DDEVICE9 XeShader<LPDIRECT3DPIXELSHADER9, PS_GLOBAL_CONSTANT_COUNT>::m_pD3DDevice;

const CHAR XEPS_SHADER_ENTRY[]  = "PSMain";
const CHAR XEPS_SHADER_TARGET_20[] = "ps_2_0";
const CHAR XEPS_SHADER_TARGET_2A[] = "ps_2_a";
const CHAR XEPS_SHADER_TARGET_30[] = "ps_3_0";

const CHAR XEPS_SHADER_FORMAT[] = 
"#include \"PsCommon.hlsl\"\n"                                          \
"#include \"PsGeneric.hlsl\"\n"                                         \
"float4 PSMain( VSOUT Input ) : COLOR\n"                                \
"{\n"                                                                   \
"   return ProcessPixel(Input, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d );\n"  \
"}";

const CHAR XEPS_CUSTOM_SHADER_FORMAT[] = 
"#include \"%s\"\n"                                                     \
"float4 PSMain( PSIN Input ) : COLOR\n"                                 \
"{\n"                                                                   \
"    return ProcessPixel( Input, %d, %d, %d, %d, %d, %d, %d, %d,%d, %d, %d, %d, %d, %d, %d );\n"   \
"}";

const CHAR XEPS_CUSTOM_SHADER_FORMAT_APPLYSHADOW[] = 
"#include \"%s\"\n"                                                     \
"PS_OUT PSMain( PSIN Input ) : COLOR\n"                                 \
"{\n"                                                                   \
"    return ProcessPixel( Input, %d, %d, %d, %d, %d, %d, %d, %d );\n"   \
"}";

///// IMPORTANT : After adding a new custom shader, update the custom shader is defines in XeShader.h
// first parameter is to force the material features. else it takes the specified custom features
const XePSCustomShaderInfo XEPS_CUSTOM_SHADERS[] = 
{
    { NULL },                   //  Entry 0, NOT used (Generic shader)
    { "PsAfterEffects.hlsl" },  //  1 After Effects
    { "PsShadow.hlsl" },        //  2 Render to shadow buffer
    { "PsApplyShadow.hlsl" },   //  3 Apply shadow
    { "PsLightShaft.hlsl" },    //  4 Light Shaft
    { "PsWater.hlsl" },         //  5 Water Effect
    { "PsSPG2.hlsl" },          //  6 spg2 vegetal lighting (features are not used, use material features instead)
    { "PsHeatShimmer.hlsl" },   //  7 Heat Shimmering
    { "PsBlurShadow.hlsl" },    //  8 Shadow blurring
    { "PsRain.hlsl" },          //  9 Rain
    { "PsShadowQuad.hlsl" },    // 10 Shadow result calculation
    { "PsCompositeShadow.hlsl"},// 11 Shadow results composition
    { "PsBlurShadowPC.hlsl" },  // 12 Shadow blurring (PC version)
    { "PsFur.hlsl"},            // 13 Fur
    { "PsSprite.hlsl" },        // 14 Sprites
    { "PsOcean.hlsl" },         // 15 Ocean
    { "PsReflection.hlsl" },    // 16 Reflection
};

const ULONG XEPS_NB_CUSTOM_SHADERS = sizeof(XEPS_CUSTOM_SHADERS) / sizeof(XePSCustomShaderInfo);

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XePixelShaderManager::XePixelShaderManager()
{
    InitShadersTable();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XePixelShaderManager::~XePixelShaderManager()
{
    UnloadAll();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XePixelShaderManager::ReInit()
{
    m_pD3DDevice                = g_oXeRenderer.GetDevice();
    XePixelShader::m_pD3DDevice = m_pD3DDevice;
    m_bForce2X                  = FALSE;
    m_fGlobalMul2XFactor        = 1.0f;
    m_iPPLLightCount            = 0;
    m_fMotionBlurAlpha          = 0.0f;
    m_fRemananceFactor          = 0.0f;
    m_vColorBalance.x = m_vColorBalance.y = m_vColorBalance.z = m_vColorBalance.w = 0.0f;

    ResetAllFeatures();

    UnloadAll();
    if (ReloadShaders() == FALSE)
    {
        // Add debug string
        UnloadAll();
    }

    InvalidateFeatureKey();
}

// ------------------------------------------------------------------------------------------------
// Name   : XePixelShaderManager::OnDeviceLost
// Params : None
// RetVal : Success
// Descr. : Before IDirect3DDevice::Reset()
// ------------------------------------------------------------------------------------------------
BOOL XePixelShaderManager::OnDeviceLost(void)
{
    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XePixelShaderManager::OnDeviceReset
// Params : None
// RetVal : Success
// Descr. : After IDirect3DDevice::Reset()
// ------------------------------------------------------------------------------------------------
BOOL XePixelShaderManager::OnDeviceReset(void)
{
    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XePixelShaderManager::Shutdown
// Params : None
// RetVal : None
// Descr. : Shutdown the pixel shader manager
// ------------------------------------------------------------------------------------------------
void XePixelShaderManager::Shutdown(void)
{
#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.AcquireThreadOwnership();
#endif
    UnloadAll();
#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.ReleaseThreadOwnership();
#endif

    m_pD3DDevice = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XePixelShaderManager::UnloadAll(void)
{
    MapPixelShader::iterator it = m_mapLoadedShaders.begin();
    while (it != m_mapLoadedShaders.end())
    {
        SAFE_DELETE(it->second);
        it++;
    }

    m_mapLoadedShaders.clear();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
BOOL XePixelShaderManager::LoadShader(XePixelShaderEntry *_poShaderEntry)
{
    void* pCode = NULL;
    XePixelShader *pNewPS;

    // load binary shader file
    if (XeLoadFile(_poShaderEntry->FileName, &pCode) == FALSE)
        return FALSE;

    if (pCode == NULL)
        return FALSE;

    pNewPS = CreateShader(_poShaderEntry->Key.ulKey, pCode, NULL, _poShaderEntry->FileName);

    XeUnloadFile( pCode );

    return (pNewPS != NULL);
}


// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
BOOL XePixelShaderManager::ReloadShaders(void)
{
    ULONG ulShaderCount = m_aPixelShaderEntries.size();

    for (ULONG ulCurShader = 0; ulCurShader < ulShaderCount; ulCurShader++)
    {
        if (LoadShader(&m_aPixelShaderEntries[ulCurShader]) == FALSE) 
        {
            // shader could not be loaded
            ERR_X_Assert(FALSE);
            return FALSE;
        }
    }

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XePixelShaderManager::UpdateShaderConstants(XePixelShader *_pShader, XeRenderObject* _pObject)
{
    ERR_X_Assert(_pShader != NULL);
    ERR_X_Assert(_pObject != NULL);

    XeMaterial  *pMaterial = _pObject->GetMaterial();
    ULONG       aulType[VS_MAX_LIGHTS_PER_PASS];
    XeLight     *poPerPixelLight[VS_MAX_LIGHTS_PER_PASS];
    
    for (ULONG ulCurLight = 0; ulCurLight < VS_MAX_LIGHTS_PER_PASS; ulCurLight++)
    {
        poPerPixelLight[ulCurLight] = g_pXeContextManagerRender->GetCurrentPerPixelLight(ulCurLight, &aulType[ulCurLight]);
    }

    // update all constants
    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_ENVMAP_COLOR))
    {
        VECTOR4FLOAT oColor;

        XeJadeColorToV4F(&oColor, pMaterial->GetEnvMapColor());

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_ENVMAP_COLOR, &oColor);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_AMBIENT_COLOR))
    {
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_AMBIENT_COLOR, g_oVertexShaderMgr.GetCurrentAmbientColor());
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_FOGCOLOR))
    {
        VECTOR4FLOAT fogColor = *((VECTOR4FLOAT*)g_pXeContextManagerRender->GetFogColor());

        if (_pObject)
        {
            XeMaterial* pMat = _pObject->GetMaterial();

			if( pMat && pMat->IsAlphaBlendEnabled() )
			{
	            // Check if additive blending
				if( (pMat->GetDestBlend() == D3DBLEND_ONE && (pMat->GetSrcBlend() == D3DBLEND_ONE || pMat->GetSrcBlend() == D3DBLEND_SRCALPHA)) ||
                    (g_oFXManager.GetCurrentPass() == XeFXManager::RP_APPLY_PIXELLIGHTING ) )
				{
	                fogColor.x = fogColor.y = fogColor.z = fogColor.w = 0.0f; 
				}
			}
        }

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_FOGCOLOR, (VECTOR4FLOAT*)&fogColor);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_CONSTANTCOLOR))
    {
        VECTOR4FLOAT oColor;
        XeJadeColorToV4F(&oColor, pMaterial->GetConstantColor());
#if defined(ACTIVE_EDITORS)
		XeEdModifyColor(&oColor, _pObject->GetExtraFlags());
#endif
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_CONSTANTCOLOR, &oColor);
    }

    BOOL bHasDiffuseColors = FALSE;
    static VECTOR4FLOAT aoLightDiffuseColorArray[VS_MAX_LIGHTS_PER_PASS];
    VECTOR4FLOAT oMatDiffuseColor;

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_MAT_DIFFUSE))
    {
        BOOL bReflectionPass = g_oFXManager.GetCurrentPass() == XeFXManager::RP_APPLY_REFLECTION;

      #ifdef _XENON
        VECTOR4FLOAT * aoDiffuseColor = NULL;
        INT iVectorCount = bReflectionPass ? 1 : m_iPPLLightCount;
        _pShader->FastConstBegin( PS_GLOBAL_CONSTANT_MAT_DIFFUSE, &aoDiffuseColor, iVectorCount );
      #else
        static VECTOR4FLOAT aoDiffuseColorArray[VS_MAX_LIGHTS_PER_PASS];
        VECTOR4FLOAT * aoDiffuseColor = aoDiffuseColorArray;
      #endif

        if (bReflectionPass)
        {
            XeJadeColorToV4F(&aoDiffuseColor[0], pMaterial->GetDiffuseColor());
        }
        else
        {
            XeJadeColorToV4F(&oMatDiffuseColor, pMaterial->GetDiffuseColor());

            if(pMaterial->GetDiffuseSelection()==MAT_Xe_DiffuseSel_World)
            {
                VECTOR4FLOAT oMatDiffuseMult;
                XeJadeColorToV4F(&oMatDiffuseMult, g_pXeContextManagerRender->ul_XeDiffuseColor);
                
                oMatDiffuseColor.x *= oMatDiffuseMult.x * 2.0f;
                oMatDiffuseColor.y *= oMatDiffuseMult.y * 2.0f;
                oMatDiffuseColor.z *= oMatDiffuseMult.z * 2.0f;

                oMatDiffuseColor.x = fMin(oMatDiffuseColor.x, 1.0f);
                oMatDiffuseColor.y = fMin(oMatDiffuseColor.y, 1.0f);
                oMatDiffuseColor.z = fMin(oMatDiffuseColor.z, 1.0f);
            }

        #if defined(ACTIVE_EDITORS)
		    XeEdModifyColor(&oMatDiffuseColor, _pObject->GetExtraFlags());
        #endif

            for (ULONG ulCurLight = 0; ulCurLight < VS_MAX_LIGHTS_PER_PASS; ulCurLight++)
            {
                if(poPerPixelLight[ulCurLight])
                {
                    aoLightDiffuseColorArray[ulCurLight].x = poPerPixelLight[ulCurLight]->color.x * poPerPixelLight[ulCurLight]->DiffuseMultiplier;
                    aoLightDiffuseColorArray[ulCurLight].y = poPerPixelLight[ulCurLight]->color.y * poPerPixelLight[ulCurLight]->DiffuseMultiplier;
                    aoLightDiffuseColorArray[ulCurLight].z = poPerPixelLight[ulCurLight]->color.z * poPerPixelLight[ulCurLight]->DiffuseMultiplier;

                    aoDiffuseColor[ulCurLight].x = oMatDiffuseColor.x * aoLightDiffuseColorArray[ulCurLight].x; 
                    aoDiffuseColor[ulCurLight].y = oMatDiffuseColor.y * aoLightDiffuseColorArray[ulCurLight].y;
                    aoDiffuseColor[ulCurLight].z = oMatDiffuseColor.z * aoLightDiffuseColorArray[ulCurLight].z;

                    if( !(_pObject->GetDrawMask() & GDI_Cul_DM_UseMapLightSettings ))
                    {
                        aoDiffuseColor[ulCurLight].x *= poPerPixelLight[ulCurLight]->ActorDiffusePonderator;
                        aoDiffuseColor[ulCurLight].y *= poPerPixelLight[ulCurLight]->ActorDiffusePonderator;
                        aoDiffuseColor[ulCurLight].z *= poPerPixelLight[ulCurLight]->ActorDiffusePonderator;
                    }

                    aoDiffuseColor[ulCurLight].w = oMatDiffuseColor.w * 4.0f;   // used as a normal map booster
                }
            }
        }

      #ifdef _XENON
        _pShader->FastConstEnd();
      #else
        _pShader->SetGlobalVectorArray(PS_GLOBAL_CONSTANT_MAT_DIFFUSE, aoDiffuseColor, VS_MAX_LIGHTS_PER_PASS);
      #endif
        bHasDiffuseColors = TRUE;
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_MOSS_DIFFUSE))
    {
        VECTOR4FLOAT oMossMatDiffuseColor;
        XeJadeColorToV4F(&oMossMatDiffuseColor, pMaterial->GetMossMapColor());
        VECTOR4FLOAT oMossMatDiffuseConsts;

        oMossMatDiffuseConsts.x = 1.0f / (1.0f-oMossMatDiffuseColor.w);
        oMossMatDiffuseConsts.y = oMossMatDiffuseColor.w * oMossMatDiffuseConsts.x;
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_MOSS_DIFFUSE, &oMossMatDiffuseConsts );

        if( _pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_MOSS_MINUS_MAT_DIFFUSE) )
        {
            ERR_X_Assert( bHasDiffuseColors );

#if defined(ACTIVE_EDITORS)
            XeEdModifyColor(&oMossMatDiffuseColor, _pObject->GetExtraFlags());
#endif

            VECTOR4FLOAT oMossMatMinusDiffuseColor;
            oMossMatMinusDiffuseColor.x = oMossMatDiffuseColor.x - oMatDiffuseColor.x;
            oMossMatMinusDiffuseColor.y = oMossMatDiffuseColor.y - oMatDiffuseColor.y;
            oMossMatMinusDiffuseColor.z = oMossMatDiffuseColor.z - oMatDiffuseColor.z;

#ifdef _XENON
            VECTOR4FLOAT * aoMossMinusDiffuseColor = NULL;
            _pShader->FastConstBegin( PS_GLOBAL_CONSTANT_MOSS_MINUS_MAT_DIFFUSE, &aoMossMinusDiffuseColor, m_iPPLLightCount );
#else
            static VECTOR4FLOAT aoMossMinusDiffuseColorArray[VS_MAX_LIGHTS_PER_PASS];
            VECTOR4FLOAT * aoMossMinusDiffuseColor = aoMossMinusDiffuseColorArray;
#endif

            for (ULONG ulCurLight = 0; ulCurLight < VS_MAX_LIGHTS_PER_PASS; ulCurLight++)
            {
                if(poPerPixelLight[ulCurLight])
                {
                    aoMossMinusDiffuseColor[ulCurLight].x = oMossMatMinusDiffuseColor.x * aoLightDiffuseColorArray[ulCurLight].x;
                    aoMossMinusDiffuseColor[ulCurLight].y = oMossMatMinusDiffuseColor.y * aoLightDiffuseColorArray[ulCurLight].y;
                    aoMossMinusDiffuseColor[ulCurLight].z = oMossMatMinusDiffuseColor.z * aoLightDiffuseColorArray[ulCurLight].z;
                }
            }

#ifdef _XENON
            _pShader->FastConstEnd();
#else
            _pShader->SetGlobalVectorArray(PS_GLOBAL_CONSTANT_MOSS_MINUS_MAT_DIFFUSE, aoMossMinusDiffuseColor, VS_MAX_LIGHTS_PER_PASS);
#endif
        }
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_MOSS_SPECULAR_FACTOR))
    {
        _pShader->SetGlobalFloat(PS_GLOBAL_CONSTANT_MOSS_SPECULAR_FACTOR, pMaterial->GetMossSpecularFactor());
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_MAT_SPECULAR))
    {
        VECTOR4FLOAT oMatSpecularColor;
  
        XeJadeColorToV4F(&oMatSpecularColor, pMaterial->GetSpecularColor());

        if(pMaterial->GetSpecularSelection()==MAT_Xe_SpecularSel_World)
        {
            VECTOR4FLOAT oMatSpecularMult;
            XeJadeColorToV4F(&oMatSpecularMult, g_pXeContextManagerRender->ul_XeSpecularColor);

            oMatSpecularColor.x *= oMatSpecularMult.x * 2.0f;
            oMatSpecularColor.y *= oMatSpecularMult.y * 2.0f;
            oMatSpecularColor.z *= oMatSpecularMult.z * 2.0f;

            oMatSpecularColor.x = fMin(oMatSpecularColor.x, 1.0f);
            oMatSpecularColor.y = fMin(oMatSpecularColor.y, 1.0f);
            oMatSpecularColor.z = fMin(oMatSpecularColor.z, 1.0f);
        }

#if defined(ACTIVE_EDITORS)
		XeEdModifyColor(&oMatSpecularColor, _pObject->GetExtraFlags());
#endif
        static VECTOR4FLOAT aoSpecularColor[VS_MAX_LIGHTS_PER_PASS];
      
        for (ULONG ulCurLight = 0; ulCurLight < VS_MAX_LIGHTS_PER_PASS; ulCurLight++)
        {
            aoSpecularColor[ulCurLight].w = oMatSpecularColor.w * 4.0f;    // used as a normal map booster

            FLOAT fFactor;
            if( poPerPixelLight[ulCurLight] )
            {
                if(_pObject->GetDrawMask() & GDI_Cul_DM_UseMapLightSettings)
                    fFactor = poPerPixelLight[ulCurLight]->SpecularMultiplier;
                else 
                    fFactor = poPerPixelLight[ulCurLight]->SpecularMultiplier * poPerPixelLight[ulCurLight]->ActorSpecularPonderator;

                aoSpecularColor[ulCurLight].x = oMatSpecularColor.x * poPerPixelLight[ulCurLight]->color.x * fFactor;
                aoSpecularColor[ulCurLight].y = oMatSpecularColor.y * poPerPixelLight[ulCurLight]->color.y * fFactor;
                aoSpecularColor[ulCurLight].z = oMatSpecularColor.z * poPerPixelLight[ulCurLight]->color.z * fFactor;
            }
        }

        _pShader->SetGlobalVectorArray(PS_GLOBAL_CONSTANT_MAT_SPECULAR, aoSpecularColor, VS_MAX_LIGHTS_PER_PASS);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_SHADOWCOLOR))
    {
        VECTOR4FLOAT ShadowColor;

        INT iNumShadowLights = min(g_oXeShadowManager.GetNumLights(), XE_MAXNUMSHADOWS);

        for(INT i = 0; i < iNumShadowLights; i++)
        {
            ShadowColor.component[i] = g_oXeShadowManager.GetShadowLight(i)->Color.w;
        }

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_SHADOWCOLOR, &ShadowColor);
    }

    // Light shaft
    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_LIGHTSHAFT_COLOR))
    {
        VECTOR4FLOAT stColor;

        g_oXeLightShaftManager.GetLightShaftColor(&stColor);

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_LIGHTSHAFT_COLOR, &stColor);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_LIGHTSHAFT_ATTENUATION))
    {
        VECTOR4FLOAT stAttenuation;

        g_oXeLightShaftManager.GetLightShaftAttenuation(&stAttenuation);

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_LIGHTSHAFT_ATTENUATION, &stAttenuation);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_HEAT_SHIMMER_SCALE))
    {
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_HEAT_SHIMMER_SCALE, &m_stHeatShimmerConsts.m_vScale);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_SHADOWBLUROFFSETS))
    {
        // only need 4 entries since 7x7 gaussian is in mirror
        _pShader->SetGlobalVectorArray(PS_GLOBAL_CONSTANT_SHADOWBLUROFFSETS, g_oXeShadowManager.GetBlurOffsetsAndWeights(), 4);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_SHADOWPARAMS))
    {
        switch(g_oFXManager.GetCurrentPass())
        {
            case XeFXManager::RP_DEFAULT:
            case XeFXManager::RP_APPLY_REFLECTION:
            {
                VECTOR4FLOAT vParams;
                FLOAT fNear, fFar, fFOVX, fFOVY;
                g_pXeContextManagerRender->GetCameraParams(&fNear, &fFar, &fFOVX, &fFOVY);

                static const float fBlurThreshold = 0.25f;

                vParams.x = fBlurThreshold / (fFar - fNear);
                vParams.y = fFar;
                vParams.z = fFar - fNear;
                vParams.w = 1.0f;

                _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_SHADOWPARAMS, &vParams);
                break;
            }
            case XeFXManager::RP_APPLY_SHADOW:
            {
                VECTOR4FLOAT vParams[XE_MAXNUMSHADOWS];
                INT iNumShadowLights = min(g_oXeShadowManager.GetNumLights(), XE_MAXNUMSHADOWS);

                for(INT i = 0; i < iNumShadowLights; i++)
                {
                    XeShadowLight* pLight = g_oXeShadowManager.GetShadowLight(i);

                    vParams[i].x = pLight->NumIterations / 2.0f;
                    vParams[i].y = pLight->FilterSize / XE_SHADOWBUFFERSIZE;
                    vParams[i].z = pLight->ZOffset;
#if 0
                    vParams[i].w = 1.0f / g_oXeShadowManager.GetJitterTextureSize();
#else
                    vParams[i].w = 1.0f / 32.0f;
#endif
                }

                _pShader->SetGlobalVectorArray(PS_GLOBAL_CONSTANT_SHADOWPARAMS, vParams, iNumShadowLights);
                break;
            }
        }

    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_ALPHATEST_PARAMS))
    {
		VECTOR4FLOAT vParams = {0.0f};

        if( _pObject )
        {
            const XeMaterial* pMat = _pObject->GetMaterial();

			if (pMat)
			{
	            vParams.x = (FLOAT)pMat->IsAlphaTestEnabled();

                // Alpha ref needs to be offset with (1/255) to avoid white dots on spg2 bug 
				vParams.y = (pMat->GetAlphaRef() / 255.0f) + 0.004f; 
                vParams.z = pMat->IsAlphaInverted() ? -1.0f : 1.0f;
				vParams.w = 1.0f;
			}
        }

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_ALPHATEST_PARAMS, &vParams);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_WATERMIXED1))
    {
        XeWaterManager::XeWaterPatchInfo *pPatchInfo = g_oWaterManager.GetWaterPatchInfoByIndex(_pObject->GetExtraDataIndex());

		if (pPatchInfo != NULL)
        {
            VECTOR4FLOAT oWaterMixed;

#if defined(_XENON)
            oWaterMixed.x = pPatchInfo->fReflectionIntensity;
            oWaterMixed.y = pPatchInfo->fRefractionIntensity;
            oWaterMixed.z = pPatchInfo->fWaterDensity;
            oWaterMixed.w = pPatchInfo->fBaseMapOpacity;
#else
            // water reflection intensity
            oWaterMixed.x = fMin(pPatchInfo->fReflectionIntensity, 1.0f);
            oWaterMixed.x = fMax(oWaterMixed.x, 0.0f);

            // water refraction intensity
            oWaterMixed.y = fMin(pPatchInfo->fRefractionIntensity, 1.0f);
            oWaterMixed.y = fMax(oWaterMixed.y, 0.0f);

            // water density
            oWaterMixed.z = fMax(pPatchInfo->fWaterDensity, 0.0f);

            // basemap opacity
            oWaterMixed.w = fMin(pPatchInfo->fBaseMapOpacity, 1.0f);
            oWaterMixed.w = fMax(oWaterMixed.w, 0.0f);
#endif
            _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_WATERMIXED1, &oWaterMixed);

            if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_WATERMIXED2))
            {
#if defined(_XENON)
                oWaterMixed.x = pPatchInfo->fFogIntensity;
#else
                // water reflection intensity
                oWaterMixed.x = fMax(pPatchInfo->fFogIntensity, 0.0f);
#endif
                _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_WATERMIXED2, &oWaterMixed);
            }
        }
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_RLIBLENDINGSCALE)) 
    {
        VECTOR4FLOAT afScale = { 0.0f, 0.0f, 0.0f, 0.0f };

        if (poPerPixelLight[0] != NULL && 0 < VS_MAX_LIGHTS_PER_PASS )
        {
            afScale.x   = poPerPixelLight[0]->RLIBlendingScale;
        }

        if (poPerPixelLight[1] != NULL && 1 < VS_MAX_LIGHTS_PER_PASS )
        {
            afScale.y   = poPerPixelLight[1]->RLIBlendingScale;
        }

        if (poPerPixelLight[2] != NULL && 2 < VS_MAX_LIGHTS_PER_PASS )
        {
            afScale.z   = poPerPixelLight[2]->RLIBlendingScale;
        }

        if (poPerPixelLight[3] != NULL && 3 < VS_MAX_LIGHTS_PER_PASS )
        {
           afScale.w    = poPerPixelLight[3]->RLIBlendingScale;
        }

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_RLIBLENDINGSCALE, &afScale );
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_RLIBLENDINGOFFSET)) 
    {
        VECTOR4FLOAT afOffset = { 0.0f, 0.0f, 0.0f, 0.0f };

        if (poPerPixelLight[0] != NULL && 0 < VS_MAX_LIGHTS_PER_PASS )
        {
            afOffset.x  = poPerPixelLight[0]->RLIBlendingOffset;
        }

        if (poPerPixelLight[1] != NULL && 1 < VS_MAX_LIGHTS_PER_PASS )
        {
            afOffset.y  = poPerPixelLight[1]->RLIBlendingOffset;
        }

        if (poPerPixelLight[2] != NULL && 2 < VS_MAX_LIGHTS_PER_PASS )
        {
            afOffset.z  = poPerPixelLight[2]->RLIBlendingOffset;
        }

        if (poPerPixelLight[3] != NULL && 3 < VS_MAX_LIGHTS_PER_PASS )
        {
            afOffset.w   = poPerPixelLight[3]->RLIBlendingOffset;
        }

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_RLIBLENDINGOFFSET, &afOffset );
    }
    
    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_MIXED3))
    {
        VECTOR4FLOAT oMixed3;

        if (_pObject->GetDrawMask() & GDI_Cul_DM_DynamicRLI)
        {
            g_pXeContextManagerRender->GetGlobalRLIScaleAndOffset(&oMixed3.x, &oMixed3.y);
        }
        else
        {
            oMixed3.x = 1.0f;
            oMixed3.y = 0.0f;
        }

        
        oMixed3.z = 0.0f;
        for (ULONG ulCurShadow = 0; ulCurShadow < VS_MAX_LIGHTS_PER_PASS; ulCurShadow++)
        {
            if (m_abEnableShadow[ulCurShadow])
                oMixed3.z += 1.0f;
        }

        oMixed3.w = _pObject->GetMaterialLODBlend();

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_MIXED3, &oMixed3);
    }

    // Rain
    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_RAIN_ALPHA_BOOST))
    {
        _pShader->SetGlobalFloat(PS_GLOBAL_CONSTANT_RAIN_ALPHA_BOOST, g_oXeWeatherManager.GetRainAlphaBoost());
    }
    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_RAIN_ALPHA_INTENSITY))
    {
        _pShader->SetGlobalFloat(PS_GLOBAL_CONSTANT_RAIN_ALPHA_INTENSITY, g_oXeWeatherManager.GetRainAlphaIntensity());
    }
    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_RAIN_COLOR))
    {
        VECTOR4FLOAT vTemp;
        g_oXeWeatherManager.GetRainColor(&vTemp.component[0]);
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_RAIN_COLOR, &vTemp);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_FOG_COLOR))
    {
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_FOG_COLOR, (VECTOR4FLOAT*)(g_pXeContextManagerRender->GetFogColor()) );
    }

    if(_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_FOG_NORMALIZING))
    {
        // These ares used to inverse the z projection to get depth buffer value to camera space
        float fCameraNear = g_pXeContextManagerRender->f_NearPlane * 0.05f; // apply gigascale
        float fCameraFar  = g_pXeContextManagerRender->f_FarPlane;

        VECTOR4FLOAT vNormalizing;
        float fX = -fCameraNear;
        float fY = ((fCameraFar + fCameraNear) / (fCameraFar - fCameraNear));
        vNormalizing.x = 1.0f / fX;
        vNormalizing.y = fY / fX;
        vNormalizing.z = (fX / g_pXeContextManagerRender->GetPixelFogParams()->y) + fY;
        vNormalizing.w = 0.0f;
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_FOG_NORMALIZING, (VECTOR4FLOAT*)(&vNormalizing) );
    }

    if ( _pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_FOG) )
    {
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_FOG, g_pXeContextManagerRender->GetPixelFogParams());
    }

    if ( _pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_FOG_PITCH_ATTENUATION ) )
    {
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_FOG_PITCH_ATTENUATION, g_pXeContextManagerRender->GetPixelFogPitchAttenuation());
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_RESOLUTION))
    {
        VECTOR4FLOAT Resolution;
        UINT uiWidth, uiHeight;

        g_oXeRenderer.GetBackbufferResolution(&uiWidth, &uiHeight);

        Resolution.x = (FLOAT)uiWidth;
        Resolution.y = (FLOAT)uiHeight;
        Resolution.z = 1.0f / (FLOAT)uiWidth;
        Resolution.w = 1.0f / (FLOAT)uiHeight;

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_RESOLUTION, &Resolution);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_SHADOWCHANNELS))
    {
        _pShader->SetGlobalVectorArray(PS_GLOBAL_CONSTANT_SHADOWCHANNELS, &m_aoShadowChannels[0], 4);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_CAMERAPARAMS))
    {
        FLOAT* pCameraParams;
        pCameraParams = g_pXeContextManagerRender->GetCameraParams(); 

        VECTOR4FLOAT vCameraParams;
        vCameraParams.component[0] = pCameraParams[0];  // Near
        vCameraParams.component[1] = pCameraParams[1];  // Far
        vCameraParams.component[2] = tanf(pCameraParams[2] * 0.5f);  // FOV X
        vCameraParams.component[3] = tanf(pCameraParams[3] * 0.5f);  // FOV Y

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_CAMERAPARAMS, &vCameraParams );
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_CAMTOLIGHTMATRIX))
    {
        D3DXMATRIX CamToLight[XE_MAXNUMSHADOWS];

        INT iNumShadowLights = min(g_oXeShadowManager.GetNumLights(), XE_MAXNUMSHADOWS);

        D3DXMATRIX* CamWorld = g_pXeContextManagerRender->GetCameraMatrix();

        for(INT i = 0; i < iNumShadowLights; i++)
        {
            D3DXMATRIX* InvCamProj  = g_oXeShadowManager.GetInvCameraProjMatrix(i);
            D3DXMATRIX* InvLight    = g_oXeShadowManager.GetViewMatrix(i);
            D3DXMATRIX* LightProj   = g_oXeShadowManager.GetProjMatrix(i);

            D3DXMatrixMultiply(&CamToLight[i], InvCamProj, CamWorld);
            D3DXMatrixMultiply(&CamToLight[i], &CamToLight[i], InvLight);
            D3DXMatrixMultiply(&CamToLight[i], &CamToLight[i], LightProj);
        }

        _pShader->SetGlobalMatrixArray(PS_GLOBAL_CONSTANT_CAMTOLIGHTMATRIX, CamToLight, iNumShadowLights);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_CASTSHADOWONSTATIC))
    {

        VECTOR4FLOAT v;

        g_oXeShadowManager.IsForceReceiver(&v);

       _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_CASTSHADOWONSTATIC, &v);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_MIXED1))
    {
        VECTOR4FLOAT oMixedConstant;

        if(_pObject->GetObjectType()==XeRenderObject::Sprites || _pObject->GetObjectType()==XeRenderObject::Interface)
            oMixedConstant.x = IsGlobalMul2XEnabled() ? ((GetGlobalMul2XFactor()+1.0f)/2.0f) : 2.0f;
        else
            oMixedConstant.x = IsGlobalMul2XEnabled() ? (GetGlobalMul2XFactor()+1.0f) : 2.0f;

        oMixedConstant.y = pMaterial->GetSpecularBias();        
        oMixedConstant.z = pMaterial->GetLocalAlpha() / 255.0f;
        oMixedConstant.w = pMaterial->GetSpecularExponent();

        if(pMaterial->GetSpecularSelection()==MAT_Xe_SpecularSel_World)
        {
            oMixedConstant.y = g_pXeContextManagerRender->f_XeSpecularStrength*(oMixedConstant.y - MAT_Xe_Specular_Bias_Min)+MAT_Xe_Specular_Bias_Min;
            MATH_f_FloatLimit(oMixedConstant.y, MAT_Xe_Specular_Bias_Min, MAT_Xe_Specular_Bias_Max);
            oMixedConstant.w = g_pXeContextManagerRender->f_XeSpecularShiny*(oMixedConstant.w - MAT_Xe_Specular_Exponent_Min)+MAT_Xe_Specular_Exponent_Min;
            MATH_f_FloatLimit(oMixedConstant.w, MAT_Xe_Specular_Exponent_Min, MAT_Xe_Specular_Exponent_Max);
        }

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_MIXED1, &oMixedConstant);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_MIXED2))
    {
        VECTOR4FLOAT oMixedConstant;
        float fDetailNormalMapStrength = pMaterial->GetDetailNormalMapStrength(_pObject->GetLOD());
        if( _pObject->GetMaterialLODDetailState() == XeRenderObject::MatLODBlend )
        {
            fDetailNormalMapStrength *= _pObject->GetMaterialLODDetailBlend();
        }

        oMixedConstant.x = fDetailNormalMapStrength;
        oMixedConstant.y = 1.0f / (FLOAT) g_pXeContextManagerRender->GetLightSetLightCount(_pObject->GetLightSetIndex());
        oMixedConstant.z = pMaterial->GetAlphaScale();
        oMixedConstant.w = pMaterial->GetAlphaOffset();

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_MIXED2, &oMixedConstant);
    }
    
    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_RIMLIGHT_PARAMS))
    {
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_RIMLIGHT_PARAMS, (VECTOR4FLOAT*)&m_stRimLightShaderConsts);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_GLOWPARAMS1))
    {
        VECTOR4FLOAT oGlowParams1;

        oGlowParams1.x = m_stAEGlowConsts.m_fZNear;
        oGlowParams1.y = m_stAEGlowConsts.m_fZFar - m_stAEGlowConsts.m_fZNear;
        oGlowParams1.z = m_stAEGlowConsts.m_fIntensity;

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_GLOWPARAMS1, &oGlowParams1);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_GLOWPARAMS2))
    {
        VECTOR4FLOAT oGlowParams2;

        oGlowParams2.x = m_stAEGlowConsts.m_fLuminosityMin;
        oGlowParams2.y = m_stAEGlowConsts.m_fLuminosityMax;

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_GLOWPARAMS2, &oGlowParams2);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_GLOWCOLOR))
    {
        VECTOR4FLOAT oColor;

        XeJadeColorToV4F(&oColor, m_stAEGlowConsts.m_ulGlowColor );

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_GLOWCOLOR, &oColor);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_GLOW_BLUR_KERNEL))
    {
        // Gaussian filter
        static const float sigma = 2.31f;
        VECTOR4FLOAT vKernel;

        vKernel.y = GetGaussianDistribution(1.0f, 0.0f, sigma);
        vKernel.z = GetGaussianDistribution(2.0f, 0.0f, sigma);
        vKernel.w = GetGaussianDistribution(3.0f, 0.0f, sigma);
        vKernel.x = 1.0f - 2.0f*( vKernel.y + vKernel.z + vKernel.w );

        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_GLOW_BLUR_KERNEL, &vKernel);
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_MOTION_BLUR_ALPHA))
    {      
        _pShader->SetGlobalFloat(PS_GLOBAL_CONSTANT_MOTION_BLUR_ALPHA, m_fMotionBlurAlpha );
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_REMANANCE_FACTOR))
    {      
        _pShader->SetGlobalFloat(PS_GLOBAL_CONSTANT_REMANANCE_FACTOR, m_fRemananceFactor );
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_COLOR_BALANCE))
    {      
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_COLOR_BALANCE, &m_vColorBalance );
    }

    if (_pShader->IsGlobalConstPresent(PS_GLOBAL_CONSTANT_BRIGHTNESS))
    {      
        _pShader->SetGlobalVector(PS_GLOBAL_CONSTANT_BRIGHTNESS, &m_vBrightness );
    }

    m_oDirtyFlags.Clear();
}

// ------------------------------------------------------------------------------------------------
// Name   : XePixelShaderManager::InitShadersTable
// Params : None
// RetVal : None
// Descr. : Initialize the shaders table
// ------------------------------------------------------------------------------------------------
void XePixelShaderManager::InitShadersTable(void)
{

}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
BOOL XePixelShaderManager::GenerateShader(XePSFeature _oFeatures)
{
    ULONG ulFlags   = 0;
    CHAR  szShaderEntry[512];

    // Flags
#if defined(_XENON)
  #if (_XDK_VER >= 1640)
    // SDALLAIRE : use old back-end since the new one sucks for now
    ulFlags |= D3DXSHADER_SKIPVALIDATION | D3DXSHADER_MICROCODE_BACKEND_OLD;
  #endif
#endif

    // Main function declaration
    if (m_oFeatures.ulCustomShader == 0 || _oFeatures.ulCustomShader==CUSTOM_PS_SPG2PLANT)
    {
        sprintf(szShaderEntry, XEPS_SHADER_FORMAT, 
				(ULONG)_oFeatures.bLocalAlpha,
                (ULONG)_oFeatures.bFogEnable,
                (ULONG)_oFeatures.bOutputFog,
                (ULONG)_oFeatures.bUseBaseTexture,
                (ULONG)_oFeatures.bColor2X,
                (ULONG)_oFeatures.ulPPLightCount,
                (ULONG)_oFeatures.bAddAmbient,
                (ULONG)_oFeatures.bAddGlobalRLI,
                (ULONG)_oFeatures.bAddLocalRLI,
                (ULONG)_oFeatures.bDiffusePPL,
                (ULONG)_oFeatures.bSpecularPPL,
                (ULONG)_oFeatures.bUseShadow0,
                (ULONG)_oFeatures.bUseShadow1,
                (ULONG)_oFeatures.bUseShadow2,
                (ULONG)_oFeatures.bUseShadow3,
                (ULONG)_oFeatures.bUseSpecularMap,
                (ULONG)_oFeatures.bUseDetailNormalMap,
                (ULONG)_oFeatures.ulSpecularMapChannel,
                (ULONG)_oFeatures.bRemapAlphaRange,
                (ULONG)_oFeatures.bUseMossMap,
                (ULONG)_oFeatures.boUseOffset,
                (ULONG)_oFeatures.bDepthToColorEnable,
                (ULONG)_oFeatures.bRimLight0,
                (ULONG)_oFeatures.bRimLight1,
                (ULONG)_oFeatures.bRimLight2,
                (ULONG)_oFeatures.bRimLight3,
                (ULONG)_oFeatures.bRimLightSMapAttenuation,
                (ULONG)_oFeatures.bNormalOffset,
                (ULONG)_oFeatures.bIsStaticObject,
                (ULONG)_oFeatures.bAlphaTest,
                (ULONG)_oFeatures.bSpecularInBaseMap,
                (ULONG)_oFeatures.bMaterialLODDetailOptimize,
                (ULONG)_oFeatures.iMaterialLODState,
                (ULONG)_oFeatures.bLuminanceInAlpha,
                (ULONG)_oFeatures.bReflection);
	}
    else
    {
        XeValidateRet((_oFeatures.ulCustomShader < XEPS_NB_CUSTOM_SHADERS), FALSE, "Invalid custom shader index");

        if (m_oFeatures.ulCustomShader == CUSTOM_PS_APPLYSHADOW)
        {
            // PsApplyShadow.hlsl
            sprintf(szShaderEntry, XEPS_CUSTOM_SHADER_FORMAT_APPLYSHADOW,
                    XEPS_CUSTOM_SHADERS[_oFeatures.ulCustomShader].szFileName,
                    (ULONG)_oFeatures.ulFeature1,
                    (ULONG)_oFeatures.ulFeature2,
                    (ULONG)_oFeatures.ulFeature3,
                    (ULONG)_oFeatures.ulFeature4,
                    (ULONG)_oFeatures.ulFeature5,
                    (ULONG)_oFeatures.ulFeature6,
                    (ULONG)_oFeatures.ulFeature7,
                    (ULONG)_oFeatures.ulFeature8);
        }
        else
        {
            sprintf(szShaderEntry, XEPS_CUSTOM_SHADER_FORMAT,
                    XEPS_CUSTOM_SHADERS[_oFeatures.ulCustomShader].szFileName,
                    (ULONG)_oFeatures.ulFeature1,
                    (ULONG)_oFeatures.ulFeature2,
                    (ULONG)_oFeatures.ulFeature3,
                    (ULONG)_oFeatures.ulFeature4,
                    (ULONG)_oFeatures.ulFeature5,
                    (ULONG)_oFeatures.ulFeature6,
                    (ULONG)_oFeatures.ulFeature7,
                    (ULONG)_oFeatures.ulFeature8,
                    (ULONG)_oFeatures.ulFeature9,
                    (ULONG)_oFeatures.ulFeature10,
                    (ULONG)_oFeatures.ulFeature11,
                    (ULONG)_oFeatures.ulFeature12,
                    (ULONG)_oFeatures.ulFeature13,
                    (ULONG)_oFeatures.ulFeature14,
                    (ULONG)_oFeatures.ulFeature15);//,
					//(ULONG)_oFeatures.ulFeature16);
        }
    }

    // Request compilation of that shader
    g_oXeShaderDatabase.RequestPixelShader(_oFeatures.ulKey, szShaderEntry, 
                                           XEPS_SHADER_ENTRY, ulFlags);

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XePixelShader* XePixelShaderManager::CreateShader(ULONG64 _ulKey, const void* _pShaderCode, ID3DXConstantTable* _pConstantTable, CHAR* _szName)
{
    ERR_X_Assert(_pShaderCode != NULL);

    XePixelShader* pShader = NULL;

    pShader = new XePixelShader();
    if (pShader == NULL)
        return NULL;

    if (_pConstantTable)
    {
        pShader->SetConstantTable(_pConstantTable);
    }

    if( (pShader == NULL) || (pShader->ReInit((DWORD *)_pShaderCode) == FALSE))
    {
        SAFE_DELETE(pShader);
        return NULL;
    }

#ifdef _DEBUG
    pShader->SetFilename(_szName);
#endif

    return pShader;
}

void XePixelShaderManager::AddShader(ULONG64 _ulKey, XePixelShader* _pShader)
{
    MapPixelShader::iterator it = m_mapLoadedShaders.find(_ulKey);

    if (it != m_mapLoadedShaders.end())
    {
        SAFE_DELETE(it->second);

        it->second = _pShader;
    }
    else
    {
        m_mapLoadedShaders[_ulKey] = _pShader;
    }
}

void XePixelShaderManager::AddCreateShader(ULONG64 _ulKey, const void* _pCode, ID3DXConstantTable* _pConstantTable, ULONG64 _ulShaderSemantics )
{
    XePixelShader* pPixelShader;
    CHAR           szShaderName[64];

    sprintf(szShaderName, "0x%016I64x.hlsl", _ulKey);

    pPixelShader = CreateShader(_ulKey, _pCode, _pConstantTable, szShaderName);
    pPixelShader->SetInputSemantics( _ulShaderSemantics );

    AddShader(_ulKey, pPixelShader);
}

void XePixelShaderManager::PreLoadShader(ULONG64 _ulKey, void* _pCode, ULONG64 _ulSemantics )
{
    AddCreateShader(_ulKey, _pCode, NULL, _ulSemantics);
}

void   
XePixelShaderManager::SetShadowChannels( ULONG ulLightIndex, INT iChannel )
{
    ERR_X_Assert( iChannel >= -1 && iChannel < 4 && ulLightIndex < VS_MAX_LIGHTS_PER_PASS );
    if ( (iChannel >= 0) && (iChannel < 4) && (ulLightIndex < VS_MAX_LIGHTS_PER_PASS) )
    {        
        if (iChannel == 0)
        {
            m_aoShadowChannels[ulLightIndex].x = 1.0f;
            m_aoShadowChannels[ulLightIndex].y = 0.0f;
            m_aoShadowChannels[ulLightIndex].z = 0.0f;
            m_aoShadowChannels[ulLightIndex].w = 0.0f;
        }
        else if (iChannel == 1)
        {
            m_aoShadowChannels[ulLightIndex].x = 0.0f;
            m_aoShadowChannels[ulLightIndex].y = 1.0f;
            m_aoShadowChannels[ulLightIndex].z = 0.0f;
            m_aoShadowChannels[ulLightIndex].w = 0.0f;
        }
        else if (iChannel == 2)
        {
            m_aoShadowChannels[ulLightIndex].x = 0.0f;
            m_aoShadowChannels[ulLightIndex].y = 0.0f;
            m_aoShadowChannels[ulLightIndex].z = 1.0f;
            m_aoShadowChannels[ulLightIndex].w = 0.0f;
        }
        else if (iChannel == 3)
        {
            m_aoShadowChannels[ulLightIndex].x = 0.0f;
            m_aoShadowChannels[ulLightIndex].y = 0.0f;
            m_aoShadowChannels[ulLightIndex].z = 0.0f;
            m_aoShadowChannels[ulLightIndex].w = 1.0f;
        }

        m_abEnableShadow[ulLightIndex] = TRUE;
    }
    else
    {
        m_abEnableShadow[ulLightIndex] = FALSE;
    }
}


void XePixelShaderManager::SetFeatureUseShadow(ULONG _ulShadow, BOOL _bEnable)
{
    switch(_ulShadow)
    {
    case 0:
        m_oFeatures.bUseShadow0 = _bEnable;
        break;
    case 1:
        m_oFeatures.bUseShadow1 = _bEnable;
        break;
    case 2:
        m_oFeatures.bUseShadow2 = _bEnable;
        break;
    case 3:
        m_oFeatures.bUseShadow3 = _bEnable;
        break;
    default:
        break;
    }

}

void XePixelShaderManager::SetFeatureRimLight(ULONG _ulLightIndex, ULONG _ulRimLightEnable )
{
    switch(_ulLightIndex)
    {
    case 0:
        m_oFeatures.bRimLight0 = _ulRimLightEnable;
        break;
    case 1:
        m_oFeatures.bRimLight1 = _ulRimLightEnable;
        break;
    case 2:
        m_oFeatures.bRimLight2 = _ulRimLightEnable;
        break;
    case 3:
        m_oFeatures.bRimLight3 = _ulRimLightEnable;
        break;
    }
}
