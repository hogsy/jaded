#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeFogEffect.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"
#include "XeShadowManager.h"

// ------------------------------------------------------------------------------------------------
// Name   : XeFogEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeFogEffect::XeFogEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMaterial        = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeFogEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeFogEffect::~XeFogEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFogEffect::ReInit()
{
    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh(g_oXeRenderer.GetQuadMesh());
    m_poRenderObject->SetMaterial(m_poMaterial);

    // build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomPSFeature(5, FOG_SHADER_1);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetCustomVSFeature(2, FOG_SHADER_1);

    m_poMaterial->AddTextureStage();
    m_poMaterial->SetFilterMode(0, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT);

    // render flags
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_UseTexture | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poRenderObject->SetPrimType(XeRenderObject::TriangleStrip);
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFogEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFogEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFogEffect::Shutdown()
{
    SAFE_DELETE(m_poRenderObject);
    SAFE_DELETE(m_poMaterial);
}

// ------------------------------------------------------------------------------------------------
// Name   : Apply
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFogEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    m_poMaterial->SetTextureId( 0, g_oXeTextureMgr.GetDepthBufferID());

    // blend current target with a fraction of the previous frame
    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    float fPitchAttenuationDensity = g_pXeContextManagerRender->GetPixelFogPitchAttenuation()->z;
    if( fPitchAttenuationDensity <= 0.01f )
    {
        m_poMaterial->SetCustomPSFeature(5, 1);
    }
    else
    {
        m_poMaterial->SetCustomPSFeature(5, 2);
    }

    // result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}