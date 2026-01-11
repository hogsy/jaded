#include "precomp.h"
#include "XeFXManager.h"
#include "XeRenderer.h"
#include "XeMaterial.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"
#include "XeShadowManager.h"
#include "XeTextureManager.h"
#include "DevHelpers/XeBench.h"
#include "DevHelpers/XePerf.h"

XeFXManager g_oFXManager;
extern TBool NoSPG2_PixelLighting;

// ------------------------------------------------------------------------------------------------
// Private declarations
// ------------------------------------------------------------------------------------------------
inline static ULONG ConvertColorSource(ULONG _ul_JadeColorSource);
inline static ULONG ConvertUVSource(ULONG _ul_JadeUVSource);

// ------------------------------------------------------------------------------------------------
// Private functions
// ------------------------------------------------------------------------------------------------

inline static void ConvertAlphaBlendToMultipass(BOOL _bEnabled, ULONG _ulSrcBlend, ULONG _ulDestBlend,
                                                ULONG* _pulNewSrcBlend, ULONG* _pulNewDestBlend)
{
    ERR_X_Assert(_pulNewSrcBlend != NULL);
    ERR_X_Assert(_pulNewDestBlend != NULL);

    // TODO: Needs to be reviewed

    if (_bEnabled)
    {
        if (_ulSrcBlend == D3DBLEND_ONE)
        {
            if (_ulDestBlend == D3DBLEND_ZERO)
            {
                *_pulNewSrcBlend  = D3DBLEND_ONE;
                *_pulNewDestBlend = D3DBLEND_ONE;
            }
            else if (_ulDestBlend == D3DBLEND_ONE)
            {
                *_pulNewSrcBlend  = D3DBLEND_ONE;
                *_pulNewDestBlend = D3DBLEND_ONE;
            }
            else if (_ulDestBlend == D3DBLEND_INVSRCALPHA)
            {
                *_pulNewSrcBlend  = D3DBLEND_SRCALPHA;
                *_pulNewDestBlend = D3DBLEND_ONE;
            }
            else
            {
                *_pulNewSrcBlend  = D3DBLEND_ONE;
                *_pulNewDestBlend = D3DBLEND_ONE;
            }
        }
        else if (_ulSrcBlend == D3DBLEND_SRCALPHA)
        {
            if (_ulDestBlend == D3DBLEND_ZERO)
            {
                *_pulNewSrcBlend  = D3DBLEND_SRCALPHA;
                *_pulNewDestBlend = D3DBLEND_ONE;
            }
            else if (_ulDestBlend == D3DBLEND_ONE)
            {
                *_pulNewSrcBlend  = D3DBLEND_SRCALPHA;
                *_pulNewDestBlend = D3DBLEND_ONE;
            }
            else if (_ulDestBlend == D3DBLEND_INVSRCALPHA)
            {
                *_pulNewSrcBlend  = D3DBLEND_SRCALPHA;
                *_pulNewDestBlend = D3DBLEND_ONE;
            }
            else
            {
                *_pulNewSrcBlend  = D3DBLEND_ONE;
                *_pulNewDestBlend = D3DBLEND_ONE;
            }
        }
        else
        {
            *_pulNewSrcBlend  = D3DBLEND_ONE;
            *_pulNewDestBlend = D3DBLEND_ONE;
        }
    }
    else
    {
        *_pulNewSrcBlend  = D3DBLEND_ONE;
        *_pulNewDestBlend = D3DBLEND_ONE;
    }
}

// ------------------------------------------------------------------------------------------------
// XeFXManager
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeFXManager::XeFXManager()
{

}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeFXManager::~XeFXManager()
{

}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::ReInit()
{
    m_pD3DDevice   = g_oXeRenderer.GetDevice();
    m_eCurrentPass = RP_DEFAULT;

    m_bRenderingOpaques = FALSE;
    m_bRenderingDepthToColorInZPass = FALSE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::OnDeviceLost
// Params : None
// RetVal : Success
// Descr. : Before IDirect3DDevice::Reset()
// ------------------------------------------------------------------------------------------------
BOOL XeFXManager::OnDeviceLost(void)
{
    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::OnDeviceReset
// Params : None
// RetVal : Success
// Descr. : After IDirect3DDevice::Reset()
// ------------------------------------------------------------------------------------------------
BOOL XeFXManager::OnDeviceReset(void)
{
    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::Shutdown
// Params : None
// RetVal : None
// Descr. : Shutdown the FX manager
// ------------------------------------------------------------------------------------------------
void XeFXManager::Shutdown(void)
{
    m_pD3DDevice = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::SetFogParams( BOOL        _bEnable,
                                D3DCOLOR    _dwColor,
                                float       _fStart, 
                                float       _fEnd, 
                                float       _fDensity,
                                float       _fPitchAttenuationMin,
                                float       _fPitchAttenuationMax,
                                float       _fPitchAttenuationIntensity )
{
    float fSinMin = sin(_fPitchAttenuationMin * M_PI / 180.0f);
    float fSinDiff = sin(_fPitchAttenuationMax * M_PI / 180.0f) - fSinMin;

 	if( fabs(_fEnd - _fStart) < 1e-3f )
    {
        _fEnd = _fStart + 1e-3f;
    }

    if( fabs(fSinDiff) < 1e-3f )
    {
        fSinDiff = 1e-3f;
    }

    // Vertex fog params
    VECTOR4FLOAT vVertexFogParams;
    vVertexFogParams.x = _fStart;
    vVertexFogParams.y = _fEnd;
    vVertexFogParams.z = _fDensity;
    vVertexFogParams.w = (_fEnd - _fStart);

    VECTOR4FLOAT vVertexFogPitchAttenuation;
    vVertexFogPitchAttenuation.x = fSinMin;
    vVertexFogPitchAttenuation.y = fSinDiff;
    vVertexFogPitchAttenuation.z = _fPitchAttenuationIntensity;
    vVertexFogPitchAttenuation.w = 0.0f;

    g_pXeContextManagerEngine->SetVertexFogParams( _bEnable, D3DXCOLOR(_dwColor), vVertexFogParams, vVertexFogPitchAttenuation );

    // Pixel fog params
    VECTOR4FLOAT vPixelFogParams;
    vPixelFogParams.x = _fStart / (_fEnd - _fStart);
    vPixelFogParams.y = _fEnd;
    vPixelFogParams.z = _fDensity;
    vPixelFogParams.w = 1.0f / (_fEnd - _fStart);

    VECTOR4FLOAT vPixelFogPitchAttenuation;

    vPixelFogPitchAttenuation.x = fSinMin / fSinDiff;
    vPixelFogPitchAttenuation.y = 1.0f / fSinDiff;
    vPixelFogPitchAttenuation.z = _fPitchAttenuationIntensity;
    vPixelFogPitchAttenuation.w = 0.0f;

    g_pXeContextManagerEngine->SetPixelFogParams( _bEnable, D3DXCOLOR(_dwColor), vPixelFogParams, vPixelFogPitchAttenuation );
}
   
// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::BeginUpdate(XeRenderObject *_pObject, RenderPass _ePassType)
{
    XEProfileFunction();

	m_eCurrentPass      = _ePassType;
	m_pCurrentObject    = _pObject;
	m_pCurrentMaterial  = _pObject->GetMaterial();
	m_pCurrentMesh      = _pObject->GetMesh();
	m_ulCurrentDrawMask = _pObject->GetDrawMask();
	m_pCurrentDeclaration = g_oVertexShaderMgr.GetVertexDeclaration(m_pCurrentMesh);

	if (m_pCurrentMaterial == NULL)
		return;

	// Reset all the features
	g_oVertexShaderMgr.ResetAllFeatures();
	g_oPixelShaderMgr.ResetAllFeatures();

    // Default light set until further notice
    g_pXeContextManagerRender->SetCurrentLightSet(0);

	// Pass-independent features and states
	SetPassIndependentInformation();

	// Pass-specific render states
	SetPassSpecificRenderStates();

	// Set the textures and sampler states
	SetTextureStages();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::EndUpdate()
{
    XEProfileFunction();

	m_pCurrentObject    = NULL;
	m_pCurrentMaterial  = NULL;
	m_pCurrentMesh      = NULL;
	m_ulCurrentDrawMask = 0;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateShaderFeaturesForSprites(void)
{
    // Vertex shader features
    // ----------------------
    const ULONG C_VS_AddRLI     = 1;
    const ULONG C_VS_AddAmbient = 2;
    const ULONG C_VS_Fog        = 3;

    g_oVertexShaderMgr.SetFeatureCustomShader(CUSTOM_VS_SPRITE);

    // Add RLI
    if (m_pCurrentDeclaration->eAttributes.bHasColor && (m_ulCurrentDrawMask & GDI_Cul_DM_UseRLI))
    {
        g_oVertexShaderMgr.SetFeatureCustom(C_VS_AddRLI, 1);
    }

    // Ambient
    if(m_ulCurrentDrawMask & GDI_Cul_DM_UseAmbient)
    {
        ULONG ulAmbientSlot = m_pCurrentObject->GetAmbientColorSlot();
        g_oVertexShaderMgr.SetActiveAmbientColor(ulAmbientSlot);
        g_oVertexShaderMgr.SetFeatureCustom(C_VS_AddAmbient, 1);
    }

    // Fog
    if ( g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) != 0 )
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()
#endif
        )
    {
        g_oVertexShaderMgr.SetFeatureCustom(C_VS_Fog, 1);
    }

    // Pixel shader features
    // ---------------------
    const ULONG C_PS_LocalAlpha = 1;
    const ULONG C_PS_UseBaseMap = 2;
    const ULONG C_PS_Color2X    = 3;
    const ULONG C_PS_Fog        = 4;

    g_oPixelShaderMgr.SetFeatureCustomShader(CUSTOM_PS_SPRITE);

    // Local alpha
    if (m_pCurrentMaterial->IsUsingLocalAlpha())
    {
        g_oPixelShaderMgr.SetFeatureCustom(C_PS_LocalAlpha, 1);
    }

    // Base map
    LONG l_TextureId = m_pCurrentMaterial->GetTextureId(0);
    if (((m_ulCurrentDrawMask & GDI_Cul_DM_UseTexture) != 0) && (l_TextureId != MAT_Xe_InvalidTextureId))
    {
        g_oPixelShaderMgr.SetFeatureCustom(C_PS_UseBaseMap, 1);
    }

    // Global Mul 2X
    ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
    ulColorSource = ConvertColorSource(ulColorSource);
    if (g_oPixelShaderMgr.IsGlobalMul2XEnabled() || (ulColorSource == COLORSOURCE_DIFFUSE2X))
    {
        g_oPixelShaderMgr.SetFeatureCustom(C_PS_Color2X, 1);
    }

    // Fog
    if ( g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) != 0 )
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()
#endif
        )
    {
        g_oPixelShaderMgr.SetFeatureCustom(C_PS_Fog, 1);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateVSFeatures()
{
    XEProfileFunction();

	ERR_X_Assert(m_pCurrentObject);
	ERR_X_Assert(m_pCurrentMaterial);

	switch (m_eCurrentPass)
	{
	case RP_DEFAULT:
    case RP_APPLY_REFLECTION:
		UpdateVSForDefaultPass();
		break;

	case RP_APPLY_DEPTH_ENV_RLI:
		UpdateVSForDepthEnvRLIPass();
		break;

	case RP_APPLY_VERTEXLIGHTING:
		UpdateVSForVertexLightingPass();
		break;

    case RP_SPG2ZOVERWRITE:
	case RP_SPG2:
        UpdateVSForSPG2PerPixelLightingPass();
		break;

	case RP_APPLY_PIXELLIGHTING_FIRSTPASS:
    case RP_APPLY_PIXELLIGHTING:
        UpdateVSForPixelLightingPass();
        break;

    case RP_APPLY_AMBIENT:
		UpdateVSForAmbientPass();
		break;

    case RP_APPLY_WATER:
        UpdateVSForWaterPass();
        break;


	case RP_COMPUTE_SHADOW:
		UpdateForShadowBufferPass(); // TODO : UpdateVSForShadowBufferPass();
		break;

    case RP_APPLY_SHADOW:
        break;

    case RP_ZPASS_Z_ONLY:
        UpdateVSForZPass( false );
        break;

    case RP_ZPASS_ALPHA_TEST:
        UpdateVSForZPass( true );
        break;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSFeatures()
{
    XEProfileFunction();

	ERR_X_Assert(m_pCurrentObject);
	ERR_X_Assert(m_pCurrentMaterial);

	// Local alpha
	if (m_pCurrentMaterial->IsUsingLocalAlpha())
	{
		g_oPixelShaderMgr.SetFeatureLocalAlpha(TRUE);
	}
	else
	{
		g_oPixelShaderMgr.SetFeatureLocalAlpha(FALSE);
	}

	switch (m_eCurrentPass)
	{
    case RP_SPG2ZOVERWRITE:
	case RP_SPG2:
		if(g_pXeContextManagerRender->GetLightSetLightCount(m_pCurrentObject->GetLightSetIndex())>0)
		{
            UpdatePSForSPG2LightingPass();
            break;
		}
		// else use default
	
	case RP_DEFAULT:
		UpdatePSForDefaultPass();
		break;

	case RP_APPLY_DEPTH_ENV_RLI:
		UpdatePSForDepthEnvRLIPass();
		break;

	case RP_APPLY_VERTEXLIGHTING:
		UpdatePSForVertexLightingPass();
		break;

	case RP_APPLY_PIXELLIGHTING_FIRSTPASS:
		UpdatePSForPixelLightingPass(TRUE);
		break;

    case RP_APPLY_PIXELLIGHTING:
        UpdatePSForPixelLightingPass(FALSE);
        break;

    case RP_APPLY_WATER:
		UpdatePSForWaterPass();
		break;

    case RP_APPLY_AMBIENT:
		UpdatePSForAmbientPass();
		break;

	case RP_COMPUTE_SHADOW:
		break;

    case RP_APPLY_SHADOW:
        UpdateForApplyShadowPass(); // TODO : UpdateVSForApplyShadowPass();
        break;

    case RP_ZPASS_Z_ONLY:
        UpdatePSForZPass( false );
        break;

    case RP_ZPASS_ALPHA_TEST:
        UpdatePSForZPass( true );
        break;

    case RP_APPLY_REFLECTION:
        UpdatePSForReflectionPass();
        break;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::SetPassIndependentInformation
// Params : None
// RetVal : None
// Descr. : Set the pass independent states
// ------------------------------------------------------------------------------------------------
void XeFXManager::SetPassIndependentInformation(void)
{
    XEProfileFunction();

#if defined(_XENON_RENDER_PC)
    // Wireframe
    if (m_ulCurrentDrawMask & GDI_Cul_DM_NotWired)
    {
        g_oRenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    }
    else
    {
        g_oRenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    }
#else
    if (g_oXeRenderer.GetWireframe())
    {
        g_oRenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    }
    else
    {
        g_oRenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    }
#endif

    // Culling
    if (!m_pCurrentMaterial->IsTwoSided() && (m_pCurrentObject->GetObjectType() != XeRenderObject::SPG2))
    {
        if (m_ulCurrentDrawMask & GDI_Cul_DM_NotInvertBF)
        {
            if ((m_eCurrentPass == RP_COMPUTE_SHADOW) || (m_eCurrentPass == RP_APPLY_REFLECTION))
                g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
            else
                g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        }
        else
        {
            if ((m_eCurrentPass == RP_COMPUTE_SHADOW) || (m_eCurrentPass == RP_APPLY_REFLECTION))
                g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
            else
                g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        }
    }
    else
    {
        g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    }

    // Channels
    ULONG ul_ActiveChannels = 0;

    BOOL bSetColorWriteMask = TRUE;

    if( m_eCurrentPass == RP_ZPASS_Z_ONLY || m_eCurrentPass == RP_ZPASS_ALPHA_TEST )
    {
        bSetColorWriteMask = m_bRenderingDepthToColorInZPass && !g_oXeRenderer.IsFastZEnabled();
    }

    if( bSetColorWriteMask )
    {
        if (m_pCurrentMaterial->IsColorWriteEnabled())
        {
            ul_ActiveChannels |= D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE;
        }

        if( m_eCurrentPass == RP_SPG2 || m_eCurrentPass == RP_SPG2ZOVERWRITE )
        {
            if( m_pCurrentObject->GetExtraFlags() & QMFR_SPG2_WRITE_ALPHA )
            {
                ul_ActiveChannels |= D3DCOLORWRITEENABLE_ALPHA;
            }
        }
        else if (m_pCurrentMaterial->IsAlphaWriteEnabled() )
        {
            ul_ActiveChannels |= D3DCOLORWRITEENABLE_ALPHA;
        }
    }

    g_oRenderStateMgr.SetRenderState(D3DRS_COLORWRITEENABLE, ul_ActiveChannels);

    // send vertex declaration
    g_oRenderStateMgr.SetVertexDeclaration(m_pCurrentDeclaration->pDeclaration);

    // Set the transform feature
    // SC: Do not attempt skinning if the skinning set is empty, happens a lot with meshes 
    //     having weights only for SPG2 generation
    if ((m_pCurrentDeclaration->eAttributes.ulTransformType == TRANSFORM_SKINNING) &&
        ((m_pCurrentObject->GetSkinningSet()->ulSkinningMatrixCount == 0) ||
         (m_pCurrentMesh->GetMaxWeights() == 0)))
    {
        g_oVertexShaderMgr.SetFeatureTransform(TRANSFORM_NORMAL);
    }
    else
    {
        g_oVertexShaderMgr.SetFeatureTransform(m_pCurrentDeclaration->eAttributes.ulTransformType);
    }

    // Set the world matrix index
    g_pXeContextManagerRender->SetWorldViewMatrix(m_pCurrentObject->GetWorldViewMatrixIndex());

    // Set the projection matrix index
    g_pXeContextManagerRender->SetProjectionMatrix(m_pCurrentObject->GetProjMatrixIndex());

    // Skinning
    g_pXeContextManagerRender->SetCurrentSkinningSet(m_pCurrentObject->GetSkinningSet());
    g_oVertexShaderMgr.SetMaxWeight(m_pCurrentMesh->GetMaxWeights());
    g_oVertexShaderMgr.SetFeatureBonesConfig(m_pCurrentMesh->GetMaxBones());
    g_oVertexShaderMgr.SetFeatureLightsConfig(m_pCurrentMesh->GetMaxBones());

#if defined(ACTIVE_EDITORS)
    if ((m_pCurrentObject->GetExtraFlags() & QMFR_ZBIAS) != 0)
    {
      FLOAT fSlopeBias = 2.0f;

      g_oRenderStateMgr.SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fSlopeBias);
    }
    else
    {
      g_oRenderStateMgr.SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
    }
#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::SetPassSpecificRenderStates
// Params : None
// RetVal : None
// Descr. : Set the render states that are pass-dependent
// ------------------------------------------------------------------------------------------------
void XeFXManager::SetPassSpecificRenderStates(void)
{
    XEProfileFunction();

    BOOL  bAlphaTest   = m_pCurrentMaterial->IsAlphaTestEnabled();
    BOOL  bBlendEnable = m_pCurrentMaterial->IsAlphaBlendEnabled();
    ULONG ulBlendOp    = m_pCurrentMaterial->GetBlendOp();
	ULONG ulSrcBlend   = m_pCurrentMaterial->GetSrcBlend();
	ULONG ulDestBlend  = m_pCurrentMaterial->GetDestBlend();

	// Alpha blending
	if ((m_eCurrentPass == RP_APPLY_PIXELLIGHTING) ||
		(m_eCurrentPass == RP_APPLY_AMBIENT)  )
	{
		bBlendEnable = TRUE;

		ConvertAlphaBlendToMultipass(m_pCurrentMaterial->IsAlphaBlendEnabled(),
									 m_pCurrentMaterial->GetSrcBlend(),
									 m_pCurrentMaterial->GetDestBlend(),
									 &ulSrcBlend, &ulDestBlend);
	}
	else if ((m_eCurrentPass == RP_COMPUTE_SHADOW)
             || (m_eCurrentPass == RP_ZPASS_Z_ONLY)
             || (m_eCurrentPass == RP_ZPASS_ALPHA_TEST)
             || (m_eCurrentPass == RP_APPLY_REFLECTION)
            )
	{
		bBlendEnable = FALSE;
	}
	else if (m_eCurrentPass == RP_APPLY_SHADOW)
	{
		bBlendEnable = FALSE;
		ulSrcBlend   = D3DBLEND_DESTCOLOR;
		ulDestBlend  = D3DBLEND_ZERO;
	}
    else if (m_eCurrentPass == RP_ZPASS_Z_ONLY )
    {
        bBlendEnable = FALSE;
        ulSrcBlend   = D3DBLEND_ONE;
        ulDestBlend  = D3DBLEND_ZERO;
    }
    else if (m_eCurrentPass == RP_ZPASS_ALPHA_TEST)
    {
        bBlendEnable = !m_bRenderingDepthToColorInZPass;
        ulSrcBlend   = D3DBLEND_ONE;
        ulDestBlend  = D3DBLEND_ZERO;
    }

    g_oRenderStateMgr.SetBlendState(bBlendEnable, ulSrcBlend, ulBlendOp, ulDestBlend);

    // Alpha testing
    if ((m_eCurrentPass == RP_COMPUTE_SHADOW) ||
        (m_eCurrentPass == RP_ZPASS_Z_ONLY) ||
        (m_eCurrentPass == RP_APPLY_REFLECTION))
    {
        bAlphaTest = FALSE;
    }

    if( m_eCurrentPass == RP_ZPASS_ALPHA_TEST )
    {
        // When rendering depth to color, alpha test is done in shader
        bAlphaTest = !m_bRenderingDepthToColorInZPass || g_oXeRenderer.IsFastZEnabled();
    }

    if (bAlphaTest)
    {
        g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAREF, m_pCurrentMaterial->GetAlphaRef());

        if (m_pCurrentMaterial->IsAlphaInverted())
        {
            g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
        }
        else
        {
            g_oRenderStateMgr.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
        }
    }
    else
    {
        g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    }

	// Depth
	BOOL  bDepthEnable  = ((m_ulCurrentDrawMask & GDI_Cul_DM_ZTest) != 0) && m_pCurrentMaterial->IsZEnabled();
	BOOL  bWriteEnable = m_pCurrentMaterial->IsZWriteEnabled();
	ULONG ulDepthFunc  = m_pCurrentMaterial->GetZFunc();

	if ((m_eCurrentPass == RP_APPLY_PIXELLIGHTING) ||
		(m_eCurrentPass == RP_APPLY_AMBIENT)       ||
		(m_eCurrentPass == RP_APPLY_SHADOW) 
        || (m_eCurrentPass == RP_APPLY_DEPTH_ENV_RLI)
        || (m_bRenderingOpaques && bWriteEnable)
       )
	{
		bDepthEnable  = TRUE;
		bWriteEnable = FALSE;
		ulDepthFunc  = D3DCMP_LESSEQUAL;
	}
	else if ((m_eCurrentPass == RP_COMPUTE_SHADOW)
             || (m_eCurrentPass == RP_ZPASS_Z_ONLY)
             || (m_eCurrentPass == RP_ZPASS_ALPHA_TEST)
            )
	{
		bDepthEnable  = TRUE;
		bWriteEnable = TRUE;

        if(m_bRenderingDoublePassZOverwrite)
            ulDepthFunc  = D3DCMP_ALWAYS;
        else
		    ulDepthFunc  = D3DCMP_LESSEQUAL;
	}

    if(m_bRenderingInterface || m_eCurrentPass==RP_SPG2ZOVERWRITE)
        ulDepthFunc  = D3DCMP_ALWAYS;

	if (bDepthEnable)
	{
		g_oRenderStateMgr.SetRenderState(D3DRS_ZENABLE, TRUE);
		g_oRenderStateMgr.SetRenderState(D3DRS_ZWRITEENABLE, bWriteEnable);
		g_oRenderStateMgr.SetRenderState(D3DRS_ZFUNC, ulDepthFunc);
	}
	else
	{
		g_oRenderStateMgr.SetRenderState(D3DRS_ZENABLE, FALSE);
	}


    //  Stencil
    BOOL b_StencilEnabled = m_pCurrentMaterial->IsStencilEnabled() || (g_oXeRenderer.IsRenderingGlowMask() && 
        (m_eCurrentPass == RP_ZPASS_Z_ONLY || m_eCurrentPass == RP_ZPASS_ALPHA_TEST));

    g_oRenderStateMgr.SetRenderState( D3DRS_STENCILENABLE, b_StencilEnabled );
    if( b_StencilEnabled && g_oXeRenderer.IsRenderingGlowMask() )
    {
        BYTE stencilRef = m_pCurrentMaterial->IsGlowEnabled() ? 255 : 0;

        if( m_pCurrentMaterial->IsGlowEnabled( ) )
        {
            g_oXeRenderer.IncGlowPrimitiveCount( );
        }

        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILENABLE,  TRUE );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILFUNC,    D3DCMP_ALWAYS );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILZFAIL,   D3DSTENCILOP_KEEP );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILPASS,    D3DSTENCILOP_REPLACE );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILREF,     stencilRef );
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::SetTextureStages
// Params : None
// RetVal : None
// Descr. : Set the texture and sampler states
// ------------------------------------------------------------------------------------------------
void XeFXManager::SetTextureStages(void)
{
    XEProfileFunction();

    ULONG ul_Stage;
    ULONG ul_MaxStage = XeMaterial::TEXTURE_COUNT;

    if (m_eCurrentPass != RP_ZPASS_Z_ONLY)
    {
        if (!m_pCurrentMaterial->IsJadeMaterial())
        {
            ul_MaxStage = m_pCurrentMaterial->GetStageCount();
        }

        // We only need the base texture when doing the alpha test z pass
        if (m_eCurrentPass == RP_ZPASS_ALPHA_TEST)
        {
            ul_MaxStage = min(1, ul_MaxStage);
        }

        // clear unused texture stages
        for (ul_Stage = ul_MaxStage; ul_Stage < XeMaterial::TEXTURE_COUNT; ++ul_Stage)
        {
            g_oRenderStateMgr.SetTexture(ul_Stage, -1);
        }

        for (ul_Stage = 0; ul_Stage < ul_MaxStage; ++ul_Stage)
        {
            ULONG ul_AddressU, ul_AddressV, ul_AddressW;
            ULONG ul_MagFilter, ul_MinFilter, ul_MipFilter;
            FLOAT f_MipmapLODBias;

            // Textures
            LONG l_TextureId = m_pCurrentMaterial->GetTextureId(ul_Stage);

#ifdef VIDEOCONSOLE_ENABLE
            if(NoTexture)
                g_oRenderStateMgr.SetTexture(ul_Stage, -1);
            else
#endif
            g_oRenderStateMgr.SetTexture(ul_Stage, l_TextureId);

            if (l_TextureId == MAT_Xe_InvalidTextureId)
                continue;

            if (((m_eCurrentPass == RP_APPLY_DEPTH_ENV_RLI) && (ul_Stage == XeMaterial::TEXTURE_ENVIRONMENT)) || 
                ((m_eCurrentPass == RP_APPLY_PIXELLIGHTING) && (ul_Stage > XeMaterial::TEXTURE_BASE)))
            {
                ul_AddressU  = D3DTADDRESS_WRAP;
                ul_AddressV  = D3DTADDRESS_WRAP;
                ul_AddressW  = D3DTADDRESS_WRAP;
                ul_MagFilter = D3DTEXF_LINEAR;
                ul_MinFilter = D3DTEXF_LINEAR;
                ul_MipFilter = D3DTEXF_LINEAR;
            }
            else
            {
                m_pCurrentMaterial->GetAddressMode(ul_Stage, &ul_AddressU, &ul_AddressV, &ul_AddressW);
                m_pCurrentMaterial->GetFilterMode(ul_Stage, &ul_MagFilter, &ul_MinFilter, &ul_MipFilter);
            }

            f_MipmapLODBias  = m_pCurrentMaterial->GetMipmapLODBias(ul_Stage);
            f_MipmapLODBias += m_fLODBias;

            g_oRenderStateMgr.SetSamplerState(ul_Stage, D3DSAMP_ADDRESSU, ul_AddressU);
            g_oRenderStateMgr.SetSamplerState(ul_Stage, D3DSAMP_ADDRESSV, ul_AddressV);
            g_oRenderStateMgr.SetSamplerState(ul_Stage, D3DSAMP_ADDRESSW, ul_AddressW);
            g_oRenderStateMgr.SetSamplerState(ul_Stage, D3DSAMP_BORDERCOLOR, m_pCurrentMaterial->GetBorderColor(ul_Stage));
            g_oRenderStateMgr.SetSamplerState(ul_Stage, D3DSAMP_MAGFILTER, ul_MagFilter);
            g_oRenderStateMgr.SetSamplerState(ul_Stage, D3DSAMP_MINFILTER, ul_MinFilter);
            g_oRenderStateMgr.SetSamplerState(ul_Stage, D3DSAMP_MIPFILTER, ul_MipFilter);
            g_oRenderStateMgr.SetSamplerState(ul_Stage, D3DSAMP_MIPMAPLODBIAS, *(DWORD*)&f_MipmapLODBias);
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateVSForDefaultPass
// Params : None
// RetVal : None
// Descr. : Set everything for a default|basic pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateVSForDefaultPass(void)
{
    XEProfileFunction();

    // Fog
    if ( g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()   // Debug toggle
#endif
        )
    {
        g_oVertexShaderMgr.SetFeatureFogEnable(TRUE);
    }

    // Color source
    ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
#if defined(ACTIVE_EDITORS)
    if ((m_ulCurrentDrawMask & GDI_Cul_DM_DontForceColor) == 0)
    {
        ulColorSource = MAT_Cc_ColorOp_FullLight;
    }
    else if (GDI_gpst_CurDD->b_ShowVertexColor)
    {
        ulColorSource = MAT_Cc_ColorOp_RLI;
    }
#endif
    ulColorSource = ConvertColorSource(ulColorSource);
    g_oVertexShaderMgr.SetFeatureColorSource(ulColorSource);
    
    // UV Source
    ULONG ulUVSource = m_pCurrentMaterial->GetUVSource();
    g_oVertexShaderMgr.SetFeatureUVSource(ConvertUVSource(ulUVSource));

    // Lighting - None
    g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_USE_COLOR);

    // make sure we don't use RLI if not present in vertex format
    g_oVertexShaderMgr.SetFeatureAddRLI(m_pCurrentDeclaration->eAttributes.bHasColor && (m_ulCurrentDrawMask & GDI_Cul_DM_UseRLI));
    g_oVertexShaderMgr.SetFeatureRLIScaleAndOffset((m_ulCurrentDrawMask & GDI_Cul_DM_Lighted) != 0);

    // Texture coordinate transform
    ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
    m_pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
    g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);

    // Env map type
    if (m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_ENVIRONMENT) != MAT_Xe_InvalidTextureId)
    {
        g_oVertexShaderMgr.EnableReflection(TRUE);
        g_oVertexShaderMgr.SetFeatureEnvMap(TRUE);
        g_oVertexShaderMgr.SetFeatureReflectionType(REFLECTION_TYPE_CUBEMAP);
        g_oVertexShaderMgr.EnableReflection(FALSE);
    }

    // ambient feature
    if(m_ulCurrentDrawMask & GDI_Cul_DM_UseAmbient)
    {
        ULONG ulAmbientSlot = m_pCurrentObject->GetAmbientColorSlot();
        g_oVertexShaderMgr.SetActiveAmbientColor(ulAmbientSlot);
        g_oVertexShaderMgr.SetFeatureAddAmbient(TRUE);
    }

    if (m_eCurrentPass == RP_APPLY_REFLECTION)
    {
        g_oVertexShaderMgr.SetFeatureReflectionType(REFLECTION_TYPE_PLANAR);
    }
}


// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdatePSForDefaultPass
// Params : None
// RetVal : None
// Descr. : Set everything for a default|basic pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSForDefaultPass(void)
{
    XEProfileFunction();

    // Fog
    if ( g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()   // Debug toggle
#endif
        )
	{
		g_oPixelShaderMgr.SetFeatureFogEnable(TRUE);
	}

	// Color source
	ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
#if defined(ACTIVE_EDITORS)
	if ((m_ulCurrentDrawMask & GDI_Cul_DM_DontForceColor) == 0)
	{
		ulColorSource = MAT_Cc_ColorOp_FullLight;
	}
    else if (GDI_gpst_CurDD->b_ShowVertexColor)
    {
        ulColorSource = MAT_Cc_ColorOp_RLI;
    }
#endif
	ulColorSource = ConvertColorSource(ulColorSource);

	// apply Mul2X when color source is COLORSOURCE_DIFFUSE2X or when we are rendering SPG2
	g_oPixelShaderMgr.SetFeatureColor2X(g_oPixelShaderMgr.IsGlobalMul2XEnabled() ? TRUE : ((ulColorSource == COLORSOURCE_DIFFUSE2X) || (m_pCurrentObject->GetCustomVS() == CUSTOM_VS_SPG2)));
    g_oPixelShaderMgr.SetFeatureRemapAlphaRange(m_pCurrentMaterial->IsAlphaRangeRemapped() ? 1 : 0);

	// Samplers
	// --------
	LONG l_TextureId = m_pCurrentMaterial->GetTextureId(0);
	if (((m_ulCurrentDrawMask & GDI_Cul_DM_UseTexture) != 0) && (l_TextureId != MAT_Xe_InvalidTextureId))
	{
		g_oPixelShaderMgr.SetFeatureUseBaseMap(TRUE);
	}
	
	//g_oPixelShaderMgr.SetFeatureUseOffset(TRUE);

#ifdef VIDEOCONSOLE_ENABLE
    if(NoMoss)
        g_oPixelShaderMgr.SetFeatureUseMossMap(FALSE);
    else
#endif
#if defined(ACTIVE_EDITORS)
    if (!GDI_gpst_CurDD->b_ShowVertexColor &&
        !GDI_gpst_CurDD->b_DisableMoss)
#endif
    {
        LONG l_MossMapId = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_MOSS);
        g_oPixelShaderMgr.SetFeatureUseMossMap(l_MossMapId != MAT_Xe_InvalidTextureId);
    }

#if defined(ACTIVE_EDITORS)
    if (GDI_gpst_CurDD->b_ShowVertexColor)
    {
        if (m_ulCurrentDrawMask & GDI_Cul_DM_UseAmbient)
        {
            g_oPixelShaderMgr.SetFeatureAddAmbient(TRUE);
        }
    }
#endif

    if( m_pCurrentMaterial->IsGlowEnabled() && 
        ( m_pCurrentMaterial->GetSrcBlend() == D3DBLEND_ONE &&
          m_pCurrentMaterial->GetDestBlend() == D3DBLEND_ONE ))
    {
        g_oPixelShaderMgr.SetFeatureLuminanceInAlpha( TRUE );
    }

    if (m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_ENVIRONMENT) != MAT_Xe_InvalidTextureId)
    {
        g_oPixelShaderMgr.SetFeatureReflection(TRUE);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateForDepthEnvRLIPass
// Params : None
// RetVal : None
// Descr. : Set the states for a Depth, Environment map, RLI|Vertex paint pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateVSForDepthEnvRLIPass(void)
{
    XEProfileFunction();

    ERR_X_Assert(m_pCurrentMaterial->IsJadeMaterial());

    LONG l_EnvMap  = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_ENVIRONMENT);

    // Fog
    if ( g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()   // Debug toggle
#endif
        )
    {
        g_oVertexShaderMgr.SetFeatureFogEnable(TRUE);
    }

    // Color source
    ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
#if defined(ACTIVE_EDITORS)
    if ((m_ulCurrentDrawMask & GDI_Cul_DM_DontForceColor) == 0)
    {
        ulColorSource = MAT_Cc_ColorOp_FullLight;
    }
#endif
    ulColorSource = ConvertColorSource(ulColorSource);
    g_oVertexShaderMgr.SetFeatureColorSource(ulColorSource);

    // UV Source
    ULONG ul_UVSource = m_pCurrentMaterial->GetUVSource();
    g_oVertexShaderMgr.SetFeatureUVSource(ConvertUVSource(ul_UVSource));

    // Lighting - Disabled
    g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_USE_COLOR);
        
    if ((m_ulCurrentDrawMask & GDI_Cul_DM_Lighted) && (m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL) != -1))
    {
        g_oVertexShaderMgr.SetFeatureAddRLI(FALSE);
    }
    else
    {
        // make sure we don't use RLI if not present in vertex format
        g_oVertexShaderMgr.SetFeatureAddRLI(m_pCurrentDeclaration->eAttributes.bHasColor && (m_ulCurrentDrawMask & GDI_Cul_DM_UseRLI));
    }
    g_oVertexShaderMgr.SetFeatureRLIScaleAndOffset((m_ulCurrentDrawMask & GDI_Cul_DM_Lighted) != 0);

    // Texture coordinate transforms
    ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
    m_pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
    g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);

    // Environment map
    g_oVertexShaderMgr.SetFeatureEnvMap(l_EnvMap != MAT_Xe_InvalidTextureId);

    // ambient feature
    if(m_ulCurrentDrawMask & GDI_Cul_DM_UseAmbient)
    {
        ULONG ulAmbientSlot = m_pCurrentObject->GetAmbientColorSlot();
        g_oVertexShaderMgr.SetActiveAmbientColor(ulAmbientSlot);
        g_oVertexShaderMgr.SetFeatureAddAmbient(TRUE);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateForDepthEnvRLIPass
// Params : None
// RetVal : None
// Descr. : Set the states for a Depth, Environment map, RLI|Vertex paint pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSForDepthEnvRLIPass(void)
{
    XEProfileFunction();

	ERR_X_Assert(m_pCurrentMaterial->IsJadeMaterial());

	LONG l_BaseMap = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_BASE);

	if (!(m_ulCurrentDrawMask & GDI_Cul_DM_UseTexture))
	{
		l_BaseMap = MAT_Xe_InvalidTextureId;
	}

	g_oPixelShaderMgr.SetFeatureUseBaseMap(l_BaseMap != MAT_Xe_InvalidTextureId);

	// Fog
	if (g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
#if !defined(_FINAL)
		&& g_oXeRenderer.GetFog()   // Debug toggle
#endif
		)
	{
		g_oPixelShaderMgr.SetFeatureFogEnable(TRUE);
	}

	// Color source
	ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
#if defined(ACTIVE_EDITORS)
	if ((m_ulCurrentDrawMask & GDI_Cul_DM_DontForceColor) == 0)
	{
		ulColorSource = MAT_Cc_ColorOp_FullLight;
	}
#endif
	ulColorSource = ConvertColorSource(ulColorSource);
	g_oPixelShaderMgr.SetFeatureColor2X(g_oPixelShaderMgr.IsGlobalMul2XEnabled() ? TRUE : (ulColorSource == COLORSOURCE_DIFFUSE2X));
    g_oPixelShaderMgr.SetFeatureRemapAlphaRange(m_pCurrentMaterial->IsAlphaRangeRemapped() ? 1 : 0);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateForVertexLightingPass
// Params : None
// RetVal : None
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateVSForVertexLightingPass(void)
{
    XEProfileFunction();

    ERR_X_Assert(m_pCurrentMaterial->IsJadeMaterial());

    // Fog
    if( g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()   // Debug toggle
#endif
       )
    {
        g_oVertexShaderMgr.SetFeatureFogEnable(TRUE);
    }

    // Color source
    ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
    ulColorSource = ConvertColorSource(ulColorSource);
    g_oVertexShaderMgr.SetFeatureColorSource(ulColorSource);

    // UV Source
    ULONG ul_UVSource = m_pCurrentMaterial->GetUVSource();
    g_oVertexShaderMgr.SetFeatureUVSource(ConvertUVSource(ul_UVSource));

    // Lighting - Enabled
    g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_VERTEX);

    // invert moss mode
    g_oVertexShaderMgr.SetFeatureInvertMossMode(m_pCurrentMaterial->IsMossInverted());

    // offset mode
    g_oVertexShaderMgr.SetFeatureOffsetMapping(m_pCurrentMaterial->IsOffsetMapping());
	g_oPixelShaderMgr.SetFeatureUseOffset(m_pCurrentMaterial->IsOffsetMapping());

    // Set Current light set
    g_pXeContextManagerRender->SetCurrentLightSet(m_pCurrentObject->GetLightSetIndex());

    // make sure we don't use RLI if not present in vertex format
    g_oVertexShaderMgr.SetFeatureAddRLI(m_pCurrentDeclaration->eAttributes.bHasColor && (m_ulCurrentDrawMask & GDI_Cul_DM_UseRLI));
    g_oVertexShaderMgr.SetFeatureRLIScaleAndOffset((m_ulCurrentDrawMask & GDI_Cul_DM_Lighted) != 0);

    // Texture coordinate transforms
    ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
    m_pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
    g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);

    // Environment map
    if (m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_ENVIRONMENT) != MAT_Xe_InvalidTextureId)
    {
        g_oVertexShaderMgr.EnableReflection(TRUE);
        g_oVertexShaderMgr.SetFeatureEnvMap(TRUE);
        g_oVertexShaderMgr.SetFeatureReflectionType(REFLECTION_TYPE_CUBEMAP);
        g_oVertexShaderMgr.EnableReflection(FALSE);
    }

    // ambient feature
    if(m_ulCurrentDrawMask & GDI_Cul_DM_UseAmbient)
    {
        ULONG ulAmbientSlot = m_pCurrentObject->GetAmbientColorSlot();
        g_oVertexShaderMgr.SetActiveAmbientColor(ulAmbientSlot);
        g_oVertexShaderMgr.SetFeatureAddAmbient(TRUE);
    }

    // Vertex Shadows
    g_oVertexShaderMgr.SetFeatureShadows(m_iCurrentShadowLightCount > 0);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateForVertexLightingPass
// Params : None
// RetVal : None
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSForVertexLightingPass(void)
{
    XEProfileFunction();

	ERR_X_Assert(m_pCurrentMaterial->IsJadeMaterial());

	LONG l_BaseMap = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_BASE);

	if (!(m_ulCurrentDrawMask & GDI_Cul_DM_UseTexture))
	{
		l_BaseMap = MAT_Xe_InvalidTextureId;
	}

    // Fog
    if( g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
#if !defined(_FINAL)
		&& g_oXeRenderer.GetFog()   // Debug toggle
#endif
		)
	{
		g_oPixelShaderMgr.SetFeatureFogEnable(TRUE);
	}

	// Color source
	ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
	ulColorSource = ConvertColorSource(ulColorSource);
	g_oPixelShaderMgr.SetFeatureColor2X(g_oPixelShaderMgr.IsGlobalMul2XEnabled() ? TRUE : (ulColorSource == COLORSOURCE_DIFFUSE2X));
    g_oPixelShaderMgr.SetFeatureRemapAlphaRange(m_pCurrentMaterial->IsAlphaRangeRemapped() ? 1 : 0);

	// Environment map
	g_oPixelShaderMgr.SetFeatureUseBaseMap(l_BaseMap != MAT_Xe_InvalidTextureId);
//g_oPixelShaderMgr.SetFeatureUseOffset(TRUE);
#ifdef VIDEOCONSOLE_ENABLE
    if(NoMoss)
        g_oPixelShaderMgr.SetFeatureUseMossMap(FALSE);
    else
#endif
#if defined(ACTIVE_EDITORS)
    if (!GDI_gpst_CurDD->b_DisableMoss)
#endif
    {       
        LONG l_MossMapId = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_MOSS);
        g_oPixelShaderMgr.SetFeatureUseMossMap(l_MossMapId != MAT_Xe_InvalidTextureId);
    }

    // Shadows
    if ((m_ulCurrentDrawMask & GDI_Cul_DM_ReceiveShadowBuffer) && (m_iCurrentShadowLightCount > 0))
    {
        for (ULONG ulCurShadow = 0; ulCurShadow < VS_MAX_LIGHTS_PER_PASS; ulCurShadow++)
        {
            g_oPixelShaderMgr.SetFeatureUseShadow(ulCurShadow, g_oPixelShaderMgr.IsShadowEnabled(ulCurShadow));
        }

        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

        DWORD dwShadowTexHandle;
        dwShadowTexHandle = g_oXeShadowManager.GetResultTextureHandle();
        g_oRenderStateMgr.SetTexture(XeMaterial::TEXTURE_SHADOWMAP, dwShadowTexHandle);

        if (m_ulCurrentDrawMask & GDI_Cul_DM_UseAmbient)
        {
            ULONG ulAmbientSlot = m_pCurrentObject->GetAmbientColorSlot();
            g_oVertexShaderMgr.SetActiveAmbientColor(ulAmbientSlot);
            g_oPixelShaderMgr.SetFeatureAddAmbient(TRUE);
        }
    }

    if( m_pCurrentMaterial->IsGlowEnabled() && 
        ( m_pCurrentMaterial->GetSrcBlend() == D3DBLEND_ONE &&
        m_pCurrentMaterial->GetDestBlend() == D3DBLEND_ONE ))
    {
        g_oPixelShaderMgr.SetFeatureLuminanceInAlpha( TRUE );
    }

    if (m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_ENVIRONMENT) != MAT_Xe_InvalidTextureId)
    {
        g_oPixelShaderMgr.SetFeatureReflection(TRUE);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateVSForSPG2PerPixelLightingPass
// Params : None
// RetVal : None
// Descr. : Set the states for an additive dynamic lighting pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateVSForSPG2PerPixelLightingPass(void)
{
    XEProfileFunction();

    if((g_pXeContextManagerRender->GetLightSetLightCount(m_pCurrentObject->GetLightSetIndex())>0) && 
        (m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL)!=-1)
#ifdef VIDEOCONSOLE_ENABLE
        && !NoSPG2_PixelLighting
#endif
        )
    {
        // Fog
        if( g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
    #if !defined(_FINAL)
            && g_oXeRenderer.GetFog()   // Debug toggle
    #endif
            )
        {
            //g_oVertexShaderMgr.SetFeatureFogEnable(TRUE);
        }

	    // Set Current light set
	    g_pXeContextManagerRender->SetCurrentLightSet(m_pCurrentObject->GetLightSetIndex());

        LONG l_NMapId  = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL);
        LONG l_SMapId  = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_SPECULAR);
        LONG l_DNMapId = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_DETAILNMAP);

        // Color source
        ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
        ulColorSource = ConvertColorSource(ulColorSource);
        g_oVertexShaderMgr.SetFeatureColorSource(ulColorSource);

        // UV Source
        ULONG ul_UVSource = m_pCurrentMaterial->GetUVSource();
        g_oVertexShaderMgr.SetFeatureUVSource(ConvertUVSource(ul_UVSource));

        // invert moss mode
        g_oVertexShaderMgr.SetFeatureInvertMossMode(m_pCurrentMaterial->IsMossInverted());

        // Lighting - Enabled
        g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_PIXEL);

        // Set Current light set
        //g_oVertexShaderMgr.SetCurrentLightSet(m_pCurrentObject->GetLightSetIndex());
        g_oVertexShaderMgr.SetFeaturePPLightCountByType(g_oPixelShaderMgr.GetPPLightCount());

        // make sure we don't use RLI if not present in vertex format
        g_oVertexShaderMgr.SetFeatureAddRLI(m_pCurrentDeclaration->eAttributes.bHasColor && (m_ulCurrentDrawMask & GDI_Cul_DM_UseRLI));
        g_oVertexShaderMgr.SetFeatureRLIScaleAndOffset((m_ulCurrentDrawMask & GDI_Cul_DM_Lighted) != 0);

        // Texture coordinate transforms
        ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
        m_pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
        g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);
        g_oVertexShaderMgr.SetFeatureNormalMapTransform(ul_NormalTC);
        if ((ul_SpecularTC != TEXTRANSFORM_NO_TEX_COORD) && (l_SMapId == MAT_Xe_InvalidTextureId))
        {
            ul_SpecularTC = TEXTRANSFORM_NO_TEX_COORD;
        }
        g_oVertexShaderMgr.SetFeatureSpecularMapTransform(ul_SpecularTC);
        if ((l_DNMapId == MAT_Xe_InvalidTextureId) || (l_NMapId == MAT_Xe_InvalidTextureId))
        {
            ul_DetailNMapTC = TEXTRANSFORM_NO_TEX_COORD;
            l_DNMapId       = MAT_Xe_InvalidTextureId;
        }
        if (l_DNMapId != MAT_Xe_InvalidTextureId)
        {
            // Outside the LOD range or below the visible threshold, discard the detail normal map
            if (m_pCurrentMaterial->GetDetailNormalMapStrength(m_pCurrentObject->GetLOD()) <= 0.01f)
            {
                ul_DetailNMapTC = TEXTRANSFORM_NO_TEX_COORD;
                l_DNMapId       = MAT_Xe_InvalidTextureId;
            }
        }
        g_oVertexShaderMgr.SetFeatureDetailNMapTransform(ul_DetailNMapTC);

        // Rim Light
        /*if( g_oVertexShaderMgr.IsRimLightEnabled() )
        {
            g_oVertexShaderMgr.SetFeatureRimLightEnable( RIMLIGHT_ON );
            if( m_pCurrentObject->GetRimLightVSConsts().m_isHeightAttenuationEnabled )
            {
                g_oVertexShaderMgr.SetFeatureRimLightHeightAttenuation( 1 );
                g_oVertexShaderMgr.SetRimLightShaderConsts( m_pCurrentObject->GetRimLightVSConsts() );
            }
        }*/

        //g_oVertexShaderMgr.SetFeatureSPG2Mode(m_pCurrentObject->GetCustomVSFeature(1));

        // ambient feature
        if(m_ulCurrentDrawMask & GDI_Cul_DM_UseAmbient)
        {
            ULONG ulAmbientSlot = (m_ulCurrentDrawMask & GDI_Cul_DM_DontUseAmbient2) ? 0 : 1;
            g_oVertexShaderMgr.SetActiveAmbientColor(ulAmbientSlot);
        }
    }
    else
    {
         UpdateVSForVertexLightingPass();
         g_oVertexShaderMgr.SetFeatureFogEnable(FALSE); // fog is done per pixel
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateVSForPixelLightingPass
// Params : None
// RetVal : None
// Descr. : Set the states for an additive dynamic lighting pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateVSForPixelLightingPass(void)
{
    XEProfileFunction();

    ERR_X_Assert(m_pCurrentMaterial->IsJadeMaterial());

    LONG l_NMapId  = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL);
    LONG l_SMapId  = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_SPECULAR);
    LONG l_DNMapId = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_DETAILNMAP);

    // Fog
    if(g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()   // Debug toggle
#endif
        )
    {
        g_oVertexShaderMgr.SetFeatureFogEnable(TRUE);
    }

    // Color source
    ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
    ulColorSource = ConvertColorSource(ulColorSource);
    g_oVertexShaderMgr.SetFeatureColorSource(ulColorSource);

    // UV Source
    ULONG ul_UVSource = m_pCurrentMaterial->GetUVSource();
    g_oVertexShaderMgr.SetFeatureUVSource(ConvertUVSource(ul_UVSource));

    // invert moss mode
    g_oVertexShaderMgr.SetFeatureInvertMossMode(m_pCurrentMaterial->IsMossInverted());

    // Lighting - Enabled
    g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_PIXEL);

    // offset mode
    g_oVertexShaderMgr.SetFeatureOffsetMapping(m_pCurrentMaterial->IsOffsetMapping());
	g_oPixelShaderMgr.SetFeatureUseOffset(m_pCurrentMaterial->IsOffsetMapping());

    // Set Current light set
    g_pXeContextManagerRender->SetCurrentLightSet(m_pCurrentObject->GetLightSetIndex());
    g_oVertexShaderMgr.SetFeaturePPLightCountByType(g_oPixelShaderMgr.GetPPLightCount());

    // make sure we don't use RLI if not present in vertex format
    g_oVertexShaderMgr.SetFeatureAddRLI(m_pCurrentDeclaration->eAttributes.bHasColor && (m_ulCurrentDrawMask & GDI_Cul_DM_UseRLI));
    g_oVertexShaderMgr.SetFeatureRLIScaleAndOffset((m_ulCurrentDrawMask & GDI_Cul_DM_Lighted) != 0);

    // Texture coordinate transforms
    ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
    m_pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
    g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);
    g_oVertexShaderMgr.SetFeatureNormalMapTransform(ul_NormalTC);
    if ((ul_SpecularTC != TEXTRANSFORM_NO_TEX_COORD) && (l_SMapId == MAT_Xe_InvalidTextureId))
    {
        ul_SpecularTC = TEXTRANSFORM_NO_TEX_COORD;
    }
    g_oVertexShaderMgr.SetFeatureSpecularMapTransform(ul_SpecularTC);
    if ((l_DNMapId == MAT_Xe_InvalidTextureId) || (l_NMapId == MAT_Xe_InvalidTextureId))
    {
        ul_DetailNMapTC = TEXTRANSFORM_NO_TEX_COORD;
        l_DNMapId       = MAT_Xe_InvalidTextureId;
    }
    if (l_DNMapId != MAT_Xe_InvalidTextureId)
    {
        // Outside the LOD range or below the visible threshold, discard the detail normal map
        if (m_pCurrentMaterial->GetDetailNormalMapStrength(m_pCurrentObject->GetLOD()) <= 0.01f)
        {
            ul_DetailNMapTC = TEXTRANSFORM_NO_TEX_COORD;
            l_DNMapId       = MAT_Xe_InvalidTextureId;
        }
    }
    g_oVertexShaderMgr.SetFeatureDetailNMapTransform(ul_DetailNMapTC);

    // Rim Light
    if( g_oVertexShaderMgr.IsRimLightEnabled() )
    {
        g_oVertexShaderMgr.SetFeatureRimLightEnable( RIMLIGHT_ON );
        if( m_pCurrentObject->GetRimLightVSConsts().m_isHeightAttenuationEnabled )
        {
            g_oVertexShaderMgr.SetFeatureRimLightHeightAttenuation( 1 );
            g_oVertexShaderMgr.SetRimLightShaderConsts( m_pCurrentObject->GetRimLightVSConsts() );
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdatePSForSPG2LightingPass
// Params : None
// RetVal : None
// Descr. : Set the states for an additive dynamic lighting pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSForSPG2LightingPass(void)
{
    XEProfileFunction();

    ERR_X_Assert(m_pCurrentMaterial->IsJadeMaterial());

    if((m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL)!=-1)
#ifdef VIDEOCONSOLE_ENABLE
        && !NoSPG2_PixelLighting
#endif
        )
    {
        LONG l_BaseMap   = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_BASE);
        LONG l_NMapId    = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL);
        LONG l_SMapId    = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_SPECULAR);

        if (!(m_ulCurrentDrawMask & GDI_Cul_DM_UseTexture))
        {
            l_BaseMap = MAT_Xe_InvalidTextureId;
        }

        // Fog
        if(g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
    #if !defined(_FINAL)
            && g_oXeRenderer.GetFog()   // Debug toggle
    #endif
            )
        {
            //g_oPixelShaderMgr.SetFeatureFogEnable(TRUE);
        }

        // Color source
        ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
        ulColorSource = ConvertColorSource(ulColorSource);
        g_oPixelShaderMgr.SetFeatureColor2X(g_oPixelShaderMgr.IsGlobalMul2XEnabled() ? TRUE : (ulColorSource == COLORSOURCE_DIFFUSE2X));

        // Lighting
        g_oPixelShaderMgr.SetFeaturePPLightCount(g_oPixelShaderMgr.GetPPLightCount());

        // RLI
        g_oPixelShaderMgr.SetFeatureAddGlobalRLI(TRUE);
        g_oPixelShaderMgr.SetFeatureAddLocalRLI(m_ulCurrentDrawMask & GDI_Cul_DM_DynamicRLI ? TRUE : FALSE);

        // Texture coordinate transforms
        ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
        m_pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
        g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);
        g_oVertexShaderMgr.SetFeatureNormalMapTransform(ul_NormalTC);
        if ((ul_SpecularTC != TEXTRANSFORM_NO_TEX_COORD) && (l_SMapId == MAT_Xe_InvalidTextureId))
        {
            ul_SpecularTC = TEXTRANSFORM_NO_TEX_COORD;
        }
        g_oVertexShaderMgr.SetFeatureSpecularMapTransform(ul_SpecularTC);

        // Texture fetch optimization by using the base map as specular
        if ((l_BaseMap != MAT_Xe_InvalidTextureId) && (ul_SpecularTC == TEXTRANSFORM_NONE) && (l_SMapId == l_BaseMap))
        {
            g_oPixelShaderMgr.SetFeatureSpecularInBaseMap(TRUE);
            g_oRenderStateMgr.SetTexture(XeMaterial::TEXTURE_SPECULAR, (DWORD)MAT_Xe_InvalidTextureId);
        }

        g_oPixelShaderMgr.SetFeatureDiffusePPL(TRUE);
        ULONG ulColor = m_pCurrentMaterial->GetSpecularColor();
        g_oPixelShaderMgr.SetFeatureSpecularPPL((ulColor & 0xFFFFFF) != 0);
        g_oPixelShaderMgr.SetFeatureUseSpecularMap(l_SMapId != MAT_Xe_InvalidTextureId);
        g_oPixelShaderMgr.SetFeatureUseBaseMap(l_BaseMap != MAT_Xe_InvalidTextureId);

        if (l_SMapId != MAT_Xe_InvalidTextureId)
        {
            ULONG ulSpecMapChannel = m_pCurrentMaterial->GetSpecularMapChannel();
            g_oPixelShaderMgr.SetFeatureSpecularMapChannel(ulSpecMapChannel);
        }

        // Shadows
        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

        DWORD dwShadowTexHandle;
        dwShadowTexHandle = g_oXeShadowManager.GetResultTextureHandle();
        g_oRenderStateMgr.SetTexture(XeMaterial::TEXTURE_SHADOWMAP, dwShadowTexHandle);

        //if (m_ulCurrentDrawMask & GDI_Cul_DM_ReceiveShadowBuffer)
        {
            for (ULONG ulCurShadow = 0; ulCurShadow < VS_MAX_LIGHTS_PER_PASS; ulCurShadow++)
            {
                g_oPixelShaderMgr.SetFeatureUseShadow(ulCurShadow, g_oPixelShaderMgr.IsShadowEnabled(ulCurShadow));
            }
        }

        // ambient feature
        if(m_ulCurrentDrawMask & GDI_Cul_DM_UseAmbient)
        {
            ULONG ulAmbientSlot = m_pCurrentObject->GetAmbientColorSlot();
            g_oVertexShaderMgr.SetActiveAmbientColor(ulAmbientSlot);
            g_oPixelShaderMgr.SetFeatureAddAmbient(TRUE);
        }

        g_oPixelShaderMgr.SetFeatureIsStatic((m_pCurrentObject->GetExtraFlags() & QMFR_ANIMATED) != QMFR_ANIMATED);
    }
    else
    {
        UpdatePSForVertexLightingPass();
    }

    // Force no moss
    g_oPixelShaderMgr.SetFeatureUseMossMap(FALSE);

	// Fog is done per pixel
    g_oPixelShaderMgr.SetFeatureFogEnable(FALSE);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdatePSForPixelLightingPass
// Params : None
// RetVal : None
// Descr. : Set the states for an additive dynamic lighting pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSForPixelLightingPass(BOOL bFirstPass)
{
    XEProfileFunction();

	ERR_X_Assert(m_pCurrentMaterial->IsJadeMaterial());

	LONG l_BaseMap   = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_BASE);
	LONG l_NMapId    = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL);
	LONG l_SMapId    = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_SPECULAR);
	LONG l_DNMapId   = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_DETAILNMAP);
    LONG l_MossMapId = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_MOSS);

	if (!(m_ulCurrentDrawMask & GDI_Cul_DM_UseTexture))
	{
		l_BaseMap = MAT_Xe_InvalidTextureId;
	}

#if defined(ACTIVE_EDITORS)
    if (GDI_gpst_CurDD->b_DisableMoss)
    {
        l_MossMapId = MAT_Xe_InvalidTextureId;
    }
#endif

    // Fog
    if(g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()   // Debug toggle
#endif
        )
    {
        g_oPixelShaderMgr.SetFeatureFogEnable(TRUE);
    }

	// Color source
	ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
	ulColorSource = ConvertColorSource(ulColorSource);
	g_oPixelShaderMgr.SetFeatureColor2X(g_oPixelShaderMgr.IsGlobalMul2XEnabled() ? TRUE : (ulColorSource == COLORSOURCE_DIFFUSE2X));
    g_oPixelShaderMgr.SetFeatureRemapAlphaRange(m_pCurrentMaterial->IsAlphaRangeRemapped() ? 1 : 0);

    // Lighting
    g_oPixelShaderMgr.SetFeaturePPLightCount(g_oPixelShaderMgr.GetPPLightCount());

    // RLI
    g_oPixelShaderMgr.SetFeatureAddGlobalRLI(bFirstPass);
    g_oPixelShaderMgr.SetFeatureAddLocalRLI(m_ulCurrentDrawMask & GDI_Cul_DM_DynamicRLI ? TRUE : FALSE);

	// Texture coordinate transforms
	ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
	m_pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
	g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);
	g_oVertexShaderMgr.SetFeatureNormalMapTransform(ul_NormalTC);
	if ((ul_SpecularTC != TEXTRANSFORM_NO_TEX_COORD) && (l_SMapId == MAT_Xe_InvalidTextureId))
	{
		ul_SpecularTC = TEXTRANSFORM_NO_TEX_COORD;
	}

    // Texture fetch optimization by using the base map as specular
    BOOL b_SpecularInBaseMap = FALSE;
    if ((l_BaseMap != MAT_Xe_InvalidTextureId) && (ul_SpecularTC == TEXTRANSFORM_NONE) && (l_SMapId == l_BaseMap))
    {
        g_oPixelShaderMgr.SetFeatureSpecularInBaseMap(TRUE);
        g_oRenderStateMgr.SetTexture(XeMaterial::TEXTURE_SPECULAR, (DWORD)MAT_Xe_InvalidTextureId);
        b_SpecularInBaseMap = TRUE;
    }

	g_oVertexShaderMgr.SetFeatureSpecularMapTransform(ul_SpecularTC);
	if ((l_DNMapId == MAT_Xe_InvalidTextureId) || (l_NMapId == MAT_Xe_InvalidTextureId))
	{
		ul_DetailNMapTC = TEXTRANSFORM_NO_TEX_COORD;
		l_DNMapId       = MAT_Xe_InvalidTextureId;
	}
	if (l_DNMapId != MAT_Xe_InvalidTextureId)
	{
		// Outside the LOD range or below the visible threshold, discard the detail normal map
		if (m_pCurrentMaterial->GetDetailNormalMapStrength(m_pCurrentObject->GetLOD()) <= 0.01f)
		{
			ul_DetailNMapTC = TEXTRANSFORM_NO_TEX_COORD;
			l_DNMapId       = MAT_Xe_InvalidTextureId;
		}
	}

    g_oPixelShaderMgr.SetFeatureDiffusePPL(TRUE);
    ULONG ulColor = m_pCurrentMaterial->GetSpecularColor();
    BOOL b_DoSpecular = (ulColor & 0xFFFFFF) != 0;
    BOOL b_UseSpecularMap = (l_SMapId != MAT_Xe_InvalidTextureId);
    g_oPixelShaderMgr.SetFeatureSpecularPPL(b_DoSpecular);
	g_oPixelShaderMgr.SetFeatureUseSpecularMap(l_SMapId != MAT_Xe_InvalidTextureId);
	g_oPixelShaderMgr.SetFeatureUseBaseMap(l_BaseMap != MAT_Xe_InvalidTextureId);
	g_oPixelShaderMgr.SetFeatureUseDetailNormalMap(ul_DetailNMapTC != TEXTRANSFORM_NO_TEX_COORD);
//g_oPixelShaderMgr.SetFeatureUseOffset(TRUE);
#ifdef VIDEOCONSOLE_ENABLE
    if(NoMoss)
        g_oPixelShaderMgr.SetFeatureUseMossMap(FALSE);
    else
#endif
        g_oPixelShaderMgr.SetFeatureUseMossMap(l_MossMapId != MAT_Xe_InvalidTextureId);

	if (l_SMapId != MAT_Xe_InvalidTextureId)
	{
		ULONG ulSpecMapChannel = m_pCurrentMaterial->GetSpecularMapChannel();
		g_oPixelShaderMgr.SetFeatureSpecularMapChannel(ulSpecMapChannel);
	}

    // Shadows
    g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(XeMaterial::TEXTURE_SHADOWMAP, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

    DWORD dwShadowTexHandle;
    dwShadowTexHandle = g_oXeShadowManager.GetResultTextureHandle();
    g_oRenderStateMgr.SetTexture(XeMaterial::TEXTURE_SHADOWMAP, dwShadowTexHandle);

    if (m_ulCurrentDrawMask & GDI_Cul_DM_ReceiveShadowBuffer)
    {
        for (ULONG ulCurShadow = 0; ulCurShadow < VS_MAX_LIGHTS_PER_PASS; ulCurShadow++)
        {
            g_oPixelShaderMgr.SetFeatureUseShadow(ulCurShadow, g_oPixelShaderMgr.IsShadowEnabled(ulCurShadow));
        }
    }

    // ambient feature
    if(bFirstPass && m_ulCurrentDrawMask & GDI_Cul_DM_UseAmbient)
    {
        ULONG ulAmbientSlot = m_pCurrentObject->GetAmbientColorSlot();
        g_oVertexShaderMgr.SetActiveAmbientColor(ulAmbientSlot);
        g_oPixelShaderMgr.SetFeatureAddAmbient(TRUE);
    }
  
    // Rim Light
    if( g_oVertexShaderMgr.IsRimLightEnabled() )
    {
        ERR_X_Assert( m_pCurrentObject->IsRimLightEnabled() );
        g_oPixelShaderMgr.SetRimLightShaderConsts( m_pCurrentObject->GetRimLightPSConsts() );
        for (ULONG ulCurRim = 0; ulCurRim < (ULONG) g_oPixelShaderMgr.GetPPLightCount(); ulCurRim++)
        {
            g_oPixelShaderMgr.SetFeatureRimLight( ulCurRim, g_oVertexShaderMgr.IsLightCreatingRim( ulCurRim ) );
        }

        if( m_pCurrentObject->GetRimLightPSConsts().m_bSpecularMapAttenuationEnabled )
        {
            g_oPixelShaderMgr.SetFeatureRimLightEnableSMapAttenuation( TRUE );
        }
    }

    g_oPixelShaderMgr.SetFeatureIsStatic((m_pCurrentObject->GetExtraFlags() & QMFR_ANIMATED) != QMFR_ANIMATED);

    // Material Detail LOD
    if( l_DNMapId != MAT_Xe_InvalidTextureId && 
        m_pCurrentObject->GetMaterialLODDetailState() == XeRenderObject::MatLODOptimize )
    {
        g_oPixelShaderMgr.SetFeatureUseDetailNormalMap(0);
        g_oPixelShaderMgr.SetFeatureMaterialLODDetailOptimize(TRUE);
    }

    // Material LOD
    g_oPixelShaderMgr.SetFeatureMaterialLODState( (ULONG) m_pCurrentObject->GetMaterialLODState() );
    if( m_pCurrentObject->GetMaterialLODState() == XeRenderObject::MatLODOptimize )
    {
        // Fully optimized material
        if( b_DoSpecular && b_UseSpecularMap && !b_SpecularInBaseMap )
        {   
            // If specular map is not already in base texture, do not add any specular
            g_oPixelShaderMgr.SetFeatureSpecularPPL(0);
            g_oPixelShaderMgr.SetFeatureUseSpecularMap(0);
        }
    }

    if( m_pCurrentMaterial->IsGlowEnabled() && 
        ( m_pCurrentMaterial->GetSrcBlend() == D3DBLEND_ONE &&
        m_pCurrentMaterial->GetDestBlend() == D3DBLEND_ONE ))
    {
        g_oPixelShaderMgr.SetFeatureLuminanceInAlpha( TRUE );
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateVSForAmbientPass
// Params : None
// RetVal : None
// Descr. : Set the states for an additive ambient pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateVSForAmbientPass(void)
{
    XEProfileFunction();

    ERR_X_Assert(m_pCurrentMaterial->IsJadeMaterial());

    // Fog
    if(g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 ) 
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()   // Debug toggle
#endif
        )
    {
        g_oVertexShaderMgr.SetFeatureFogEnable(TRUE);
    }


    // Color source - Ambient color
    g_oVertexShaderMgr.SetFeatureColorSource(COLORSOURCE_CONSTANT);

    // UV Source
    ULONG ul_UVSource = m_pCurrentMaterial->GetUVSource();
    g_oVertexShaderMgr.SetFeatureUVSource(ConvertUVSource(ul_UVSource));

    // Lighting - Disabled
    g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_USE_COLOR);

    // Texture coordinate transforms
    ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
    m_pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
    g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);
    g_oVertexShaderMgr.SetFeatureNormalMapTransform(TEXTRANSFORM_NO_TEX_COORD);
    g_oVertexShaderMgr.SetFeatureSpecularMapTransform(TEXTRANSFORM_NO_TEX_COORD);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdatePSForAmbientPass
// Params : None
// RetVal : None
// Descr. : Set the states for an additive ambient pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSForAmbientPass(void)
{
    XEProfileFunction();

	ERR_X_Assert(m_pCurrentMaterial->IsJadeMaterial());

    // Fog
    if(g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()   // Debug toggle
#endif
        )
    {
        g_oPixelShaderMgr.SetFeatureFogEnable(TRUE);
    }

	LONG l_BaseMap = m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_BASE);

	if (!(m_ulCurrentDrawMask & GDI_Cul_DM_UseTexture))
	{
		l_BaseMap = MAT_Xe_InvalidTextureId;
	}

	g_oPixelShaderMgr.SetFeatureUseBaseMap(l_BaseMap != MAT_Xe_InvalidTextureId);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdatePSForFurPass
// Params : None
// RetVal : None
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSForFurPass(BOOL bFirstPass)
{
    g_oPixelShaderMgr.SetFeatureCustomShader(CUSTOM_PS_FUR);

    // set normal offset feature
    g_oPixelShaderMgr.SetFeatureCustom(1, TRUE);

    // set light count
    g_oPixelShaderMgr.SetFeatureCustom(2, g_oPixelShaderMgr.GetPPLightCount());

    // set local alpha feature
    g_oPixelShaderMgr.SetFeatureCustom(3, m_pCurrentMaterial->IsUsingLocalAlpha());

    // set alpha remapping feature
    g_oPixelShaderMgr.SetFeatureCustom(4, m_pCurrentMaterial->IsAlphaRangeRemapped());

    // set basemap feature
    if ((m_ulCurrentDrawMask & GDI_Cul_DM_UseTexture) != 0)
    {
        g_oPixelShaderMgr.SetFeatureCustom(5, TRUE);
    }

    // set normal map feature
    g_oPixelShaderMgr.SetFeatureCustom(6, (g_oPixelShaderMgr.GetPPLightCount() > 0) && (m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL) != -1) ? TRUE : FALSE);

    // set ambient feature
    g_oPixelShaderMgr.SetFeatureCustom(7, bFirstPass);

    // set vertex fog feature
    if(g_pXeContextManagerRender->IsFogEnabled() && !m_bRenderingOpaques && ((m_ulCurrentDrawMask & GDI_Cul_DM_Fogged) !=0 )
#if !defined(_FINAL)
        && g_oXeRenderer.GetFog()   // Debug toggle
#endif
        )
    {
        g_oPixelShaderMgr.SetFeatureCustom(8, TRUE);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateVSForWaterPass
// Params : None
// RetVal : None
// Descr. : Set everything for a default|basic pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateVSForWaterPass(void)
{
    XEProfileFunction();


}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdatePSForWaterPass
// Params : None
// RetVal : None
// Descr. : Set everything for a default|basic pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSForWaterPass(void)
{
    XEProfileFunction();
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateVSForShadowBufferPass
// Params : None
// RetVal : None
// Descr. : Set the states for rendering into the shadow buffer
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateForShadowBufferPass(void)
{
    XEProfileFunction();

    // Custom shader features are set in the material
    g_oPixelShaderMgr.SetFeatureCustomShader(CUSTOM_PS_RENDERSHADOW);
    g_oPixelShaderMgr.SetFeatureCustom(2, m_pCurrentMaterial->IsUsingLocalAlpha());
    g_oPixelShaderMgr.SetFeatureCustom(3, m_pCurrentMaterial->IsAlphaTestEnabled());

    g_oVertexShaderMgr.SetFeatureCustomShader(CUSTOM_VS_SHADOW);
    if ((m_pCurrentDeclaration->eAttributes.ulTransformType == TRANSFORM_SKINNING) &&
        (m_pCurrentObject->GetSkinningSet()->ulSkinningMatrixCount == 0))
    {
        g_oVertexShaderMgr.SetFeatureCustom(1, TRANSFORM_NORMAL);
    }
    else
    {
        g_oVertexShaderMgr.SetFeatureCustom(1, m_pCurrentDeclaration->eAttributes.ulTransformType);
    }
    g_oVertexShaderMgr.SetFeatureCustom(2, 0);
    m_pCurrentMaterial->SetCustomVSFeature(2, 0);

    const D3DXMATRIX* Projection;
    D3DXMATRIX* pWorld;
    D3DXMATRIX WorldView;

    // Set the WorldView matrix index
    pWorld = m_pCurrentObject->GetWorldMatrix();

    D3DXMatrixMultiply(&WorldView, pWorld, g_oXeShadowManager.GetViewMatrix(-1));
    m_pCurrentObject->SetShadowWorldViewMatrixIndex(g_pXeContextManagerRender->PushWorldViewMatrix(&WorldView));

    // Set the projection matrix index
    Projection = g_oXeShadowManager.GetProjMatrix(-1);
    m_pCurrentObject->SetShadowProjMatrixIndex(g_pXeContextManagerRender->PushProjectionMatrix(Projection));

    g_pXeContextManagerRender->SetWorldViewMatrix(m_pCurrentObject->GetShadowWorldViewMatrixIndex());
    g_pXeContextManagerRender->SetProjectionMatrix(m_pCurrentObject->GetShadowProjMatrixIndex());

    g_oRenderStateMgr.SetBlendState(FALSE);
    g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    g_oRenderStateMgr.SetTexture(XeMaterial::TEXTURE_BASE, m_pCurrentMaterial->GetTextureId(0));
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateForApplyShadowPass
// Params : None
// RetVal : None
// Descr. : Set the states for shadow pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateForApplyShadowPass(void)
{
    XEProfileFunction();

    INT iNumShadowLights = min(g_oXeShadowManager.GetNumLights(), XE_MAXNUMSHADOWS);
    iNumShadowLights = min(iNumShadowLights, g_oXeRenderer.GetMaxShadows());

    g_oPixelShaderMgr.SetFeatureCustomShader(CUSTOM_PS_SHADOWQUAD);
    g_oPixelShaderMgr.SetFeatureCustom(1, iNumShadowLights);

    // TODO: remove this line once shaders support for() loops with variables
    g_oPixelShaderMgr.SetFeatureCustom(2, g_oXeShadowManager.GetShadowLight(0)->NumIterations);

    g_oVertexShaderMgr.SetFeatureCustomShader(CUSTOM_VS_SHADOWQUAD);

    // Sampler 0: Z buffer texture
    g_oRenderStateMgr.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    g_oRenderStateMgr.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    g_oRenderStateMgr.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    g_oRenderStateMgr.SetTexture(0, g_oXeTextureMgr.GetDepthBufferID());

#if 0
    // Sampler 1: Jitter texture
    g_oRenderStateMgr.SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    g_oRenderStateMgr.SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    g_oRenderStateMgr.SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    g_oRenderStateMgr.SetTexture(1, g_oXeShadowManager.GetJitterTextureHandle());
#endif

    INT iShadowMip = g_oXeShadowManager.GetCurrentShadowMip();

    // Sampler 2: Shadow buffer #0
    g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
    g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
    g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);
    g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    g_oRenderStateMgr.SetTexture(2, g_oXeShadowManager.GetShadowBufferHandle(0, iShadowMip));

    // Sampler 3: Shadow buffer #1
    g_oRenderStateMgr.SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
    g_oRenderStateMgr.SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
    g_oRenderStateMgr.SetSamplerState(3, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);
    g_oRenderStateMgr.SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    g_oRenderStateMgr.SetTexture(3, g_oXeShadowManager.GetShadowBufferHandle(1, iShadowMip));

    // Sampler 4: Shadow buffer #2
    g_oRenderStateMgr.SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
    g_oRenderStateMgr.SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
    g_oRenderStateMgr.SetSamplerState(4, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);
    g_oRenderStateMgr.SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    g_oRenderStateMgr.SetTexture(4, g_oXeShadowManager.GetShadowBufferHandle(2, iShadowMip));

/*
    // Sampler 5: Shadow buffer #3
    g_oRenderStateMgr.SetSamplerState(5, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
    g_oRenderStateMgr.SetSamplerState(5, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
    g_oRenderStateMgr.SetSamplerState(5, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);
    g_oRenderStateMgr.SetSamplerState(5, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(5, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    g_oRenderStateMgr.SetSamplerState(5, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    g_oRenderStateMgr.SetTexture(5, g_oXeShadowManager.GetShadowBufferHandle(3, iShadowMip));
*/

/*
    // Sampler 2: Cookie #1
    DWORD dwCookieHandle = g_oXeShadowManager.GetShadowLight(0)->CookieTexureHandle;
    if(dwCookieHandle != -1)
    {
        g_oRenderStateMgr.SetTexture(2, dwCookieHandle);
        g_oPixelShaderMgr.SetFeatureCustom(3, 1);

        g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
        g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
        g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);
        g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        g_oRenderStateMgr.SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    }
*/
    // Sampler : Shadow buffer #2 UV
    // Sampler : Cookie #2 UV

    // Sampler : Shadow buffer #3 UV
    // Sampler : Cookie #3 UV

    // Sampler : Shadow buffer #4 UV
    // Sampler : Cookie #4 UV

    g_oRenderStateMgr.SetRenderState(D3DRS_ZENABLE, FALSE);
    g_oRenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    g_oRenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    g_oRenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    g_oRenderStateMgr.SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    g_oRenderStateMgr.SetBlendState(FALSE);
    g_oRenderStateMgr.SetRenderState(D3DRS_STENCILENABLE, FALSE);
    g_oRenderStateMgr.SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
    g_oRenderStateMgr.SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED |
                                                             D3DCOLORWRITEENABLE_GREEN |
                                                             D3DCOLORWRITEENABLE_BLUE |
                                                             D3DCOLORWRITEENABLE_ALPHA);
}


// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdateVSForZPass
// Params : None
// RetVal : None
// Descr. : Update the vertex shader features for alpha tested z pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateVSForZPass( bool _bAphaTest )
{
    XEProfileFunction();

    g_oVertexShaderMgr.SetFeatureDepthToColorEnable( m_bRenderingDepthToColorInZPass );

    if( !_bAphaTest )
    {
        // SC: We need to use those features to prevent z fighting with the normal pass...
        //  -> Vertex or pixel based lighting will make sure the same compilation options are used 
        //     when generating the shader
        if (m_ulCurrentDrawMask & GDI_Cul_DM_Lighted)
        {
            if (m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL) != MAT_Xe_InvalidTextureId)
                g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_PIXEL);
            else
                g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_VERTEX);
        }
        else
        {
            g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_USE_COLOR);
        }

        // SC: Using a constant color source to make sure the lighting will not be computed
        g_oVertexShaderMgr.SetFeatureColorSource(COLORSOURCE_CONSTANT);
    }
    else
    {
        // Color source
        ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
#if defined(ACTIVE_EDITORS)
        if ((m_ulCurrentDrawMask & GDI_Cul_DM_DontForceColor) == 0)
        {
            ulColorSource = MAT_Cc_ColorOp_FullLight;
        }
#endif
        // SC: We need to use those features to prevent z fighting with the normal pass...
        //  -> Vertex or pixel based lighting will make sure the same compilation options are used 
        //     when generating the shader
        if (m_ulCurrentDrawMask & GDI_Cul_DM_Lighted)
        {
            if (m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL) != MAT_Xe_InvalidTextureId)
                g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_PIXEL);
            else
                g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_VERTEX);
        }
        else
        {
            g_oVertexShaderMgr.SetFeatureLighting(LIGHTING_USE_COLOR);
        }

        ulColorSource = ConvertColorSource(ulColorSource);
        g_oVertexShaderMgr.SetFeatureColorSource(ulColorSource);

        // UV Source
        ULONG ulUVSource = m_pCurrentMaterial->GetUVSource();
        g_oVertexShaderMgr.SetFeatureUVSource(ConvertUVSource(ulUVSource));

        // make sure we don't use RLI if not present in vertex format
        g_oVertexShaderMgr.SetFeatureAddRLI(m_pCurrentDeclaration->eAttributes.bHasColor && (m_ulCurrentDrawMask & GDI_Cul_DM_UseRLI));

        // Texture coordinate transform
        ULONG ul_BaseTC, ul_NormalTC, ul_SpecularTC, ul_DetailNMapTC;
        m_pCurrentMaterial->GetTexCoordTransformStates(&ul_BaseTC, &ul_NormalTC, &ul_SpecularTC, &ul_DetailNMapTC);
        g_oVertexShaderMgr.SetFeatureBaseMapTransform(ul_BaseTC);

        // ambient feature
        if(m_ulCurrentDrawMask & GDI_Cul_DM_UseAmbient)
        {
            ULONG ulAmbientSlot = m_pCurrentObject->GetAmbientColorSlot();
            g_oVertexShaderMgr.SetActiveAmbientColor(ulAmbientSlot);
            g_oVertexShaderMgr.SetFeatureAddAmbient(TRUE);
        }
    }
}


// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdatePSForZPass
// Params : None
// RetVal : None
// Descr. : Update the pixel shader features for alpha tested z pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSForZPass( bool _bAphaTest )
{
    XEProfileFunction();

    if(g_oXeRenderer.IsFastZEnabled())
    {
        // Pixel shader set to NULL (no features)
        // Z only, 2x rendering
        if(!_bAphaTest)
            return;
    }

    if (m_bRenderingDepthToColorInZPass && !g_oXeRenderer.IsFastZEnabled())
    {
        g_oPixelShaderMgr.SetFeatureDepthToColorEnable( TRUE );
        g_oPixelShaderMgr.SetFeatureAlphaTestEnable(_bAphaTest);
    }

    if( _bAphaTest )
    {
        g_oPixelShaderMgr.SetFeatureRemapAlphaRange(m_pCurrentMaterial->IsAlphaRangeRemapped() ? 1 : 0);

        // Samplers
        // --------
        LONG l_TextureId = m_pCurrentMaterial->GetTextureId(0);
        if (((m_ulCurrentDrawMask & GDI_Cul_DM_UseTexture) != 0) && (l_TextureId != MAT_Xe_InvalidTextureId))
        {
            g_oPixelShaderMgr.SetFeatureUseBaseMap(TRUE);
        }
//g_oPixelShaderMgr.SetFeatureUseOffset(TRUE);
        if (m_ulCurrentDrawMask & GDI_Cul_DM_Lighted)
        {
            if (m_pCurrentMaterial->GetTextureId(XeMaterial::TEXTURE_NORMAL) != MAT_Xe_InvalidTextureId)
                g_oPixelShaderMgr.SetFeatureDiffusePPL(TRUE);
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeFXManager::UpdatePSForReflectionPass
// Params : None
// RetVal : None
// Descr. : Update the pixel shader features for reflection pass
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdatePSForReflectionPass(void)
{

    g_oPixelShaderMgr.ResetAllFeatures();
    g_oPixelShaderMgr.SetFeatureCustomShader(CUSTOM_PS_REFLECTION);

    // shader id
    g_oPixelShaderMgr.SetFeatureCustom(1, 0);

    // basemap
    LONG l_TextureId = m_pCurrentMaterial->GetTextureId(0);
    if (((m_ulCurrentDrawMask & GDI_Cul_DM_UseTexture) != 0) && (l_TextureId != MAT_Xe_InvalidTextureId))
    {
        g_oPixelShaderMgr.SetFeatureCustom(2, TRUE);
    }

    // Local alpha
    if (m_pCurrentMaterial->IsUsingLocalAlpha())
    {
        g_oPixelShaderMgr.SetFeatureCustom(3, TRUE);
    }

    // remap alpha range
    if (m_pCurrentMaterial->IsAlphaRangeRemapped())
    {
        g_oPixelShaderMgr.SetFeatureCustom(4, TRUE);
    }

    // alpha test
    if (m_pCurrentMaterial->IsAlphaTestEnabled())
    {
        g_oPixelShaderMgr.SetFeatureCustom(5, TRUE);
    }

    // Color source
    ULONG ulColorSource = m_pCurrentMaterial->GetColorSource();
#if defined(ACTIVE_EDITORS)
    if ((m_ulCurrentDrawMask & GDI_Cul_DM_DontForceColor) == 0)
    {
        ulColorSource = MAT_Cc_ColorOp_FullLight;
    }
    else if (GDI_gpst_CurDD->b_ShowVertexColor)
    {
        ulColorSource = MAT_Cc_ColorOp_RLI;
    }
#endif
    ulColorSource = ConvertColorSource(ulColorSource);

    // apply Mul2X when color source is COLORSOURCE_DIFFUSE2X
    if (g_oPixelShaderMgr.IsGlobalMul2XEnabled() || (ulColorSource == COLORSOURCE_DIFFUSE2X))
    {
        g_oPixelShaderMgr.SetFeatureCustom(6, TRUE);
    }
}