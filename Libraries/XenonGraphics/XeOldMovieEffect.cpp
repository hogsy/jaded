#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeOldMovieEffect.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"
#include "XeShadowManager.h"

// ------------------------------------------------------------------------------------------------
// Name   : XeOldMovieEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeOldMovieEffect::XeOldMovieEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMaterial        = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeOldMovieEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeOldMovieEffect::~XeOldMovieEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeOldMovieEffect::ReInit()
{
    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh(g_oXeRenderer.GetQuadMesh());
    m_poRenderObject->SetMaterial(m_poMaterial);

    // build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    m_poMaterial->SetColorSource(MAT_Cc_ColorOp_ConstantColor);


    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomPSFeature(13, 1);

    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetCustomVSFeature(10, 1);

    m_poMaterial->AddTextureStage();
    m_poMaterial->SetFilterMode(0, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT);

    // render flags
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_UseTexture | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poRenderObject->SetPrimType(XeRenderObject::TriangleStrip);

    Enable(0, FALSE);
    Enable(1, FALSE);
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeOldMovieEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeOldMovieEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeOldMovieEffect::Shutdown()
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
void XeOldMovieEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    // if the backbuffer is not already resolved, resolve it now
    if (lTextureIn != XESCRATCHBUFFER0_ID)
    {
        g_oXeTextureMgr.ResolveToScratchBuffer(XESCRATCHBUFFER0_ID);
        lTextureIn = XESCRATCHBUFFER0_ID;
    }

    m_poMaterial->SetTextureId(0, XESCRATCHBUFFER0_ID);

    // blend current target with a fraction of the previous frame
    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    // result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}