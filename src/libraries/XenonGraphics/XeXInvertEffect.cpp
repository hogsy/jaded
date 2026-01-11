#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeXInvertEffect.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"
#include "XeShadowManager.h"

// ------------------------------------------------------------------------------------------------
// Name   : XeXInvertEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeXInvertEffect::XeXInvertEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMaterial        = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeXInvertEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeXInvertEffect::~XeXInvertEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeXInvertEffect::ReInit()
{
    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh(g_oXeRenderer.GetQuadMesh());
    m_poRenderObject->SetMaterial(m_poMaterial);

    // build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(FALSE);
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomPSFeature(6, 1);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetCustomVSFeature(7, 1);

    m_poMaterial->AddTextureStage();
    m_poMaterial->SetFilterMode(0, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT);
    m_poMaterial->SetTextureId( 0, XESCRATCHBUFFER0_ID );

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
void XeXInvertEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeXInvertEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeXInvertEffect::Shutdown()
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
void XeXInvertEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    if( lTextureIn != XESCRATCHBUFFER0_ID )
    {
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID );
        lTextureIn = XESCRATCHBUFFER0_ID;
    }

    // Blit inverted
    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    // result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}