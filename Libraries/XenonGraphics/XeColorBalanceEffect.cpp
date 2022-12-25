#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeColorBalanceEffect.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"

#define COLORBALANCE_VERTEX_STRIDE    4*sizeof(FLOAT)

// ------------------------------------------------------------------------------------------------
// Name   : XeColorBalanceEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeColorBalanceEffect::XeColorBalanceEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMaterial        = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeColorBalanceEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeColorBalanceEffect::~XeColorBalanceEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorBalanceEffect::ReInit()
{
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ulContext++)
    {
        m_afIntensity[ulContext]            = 0.0f;
        m_afSpectre[ulContext]              = 0.0f;
    }

    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh( g_oXeRenderer.GetQuadMesh() );
    m_poRenderObject->SetMaterial(m_poMaterial);

    // build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ZERO, D3DBLEND_SRCCOLOR);
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);

    m_poMaterial->SetCustomVSFeature( 3, 1 );
    m_poMaterial->SetCustomPSFeature( 10, 1 );

    // render flags
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poRenderObject->SetPrimType(XeRenderObject::TriangleStrip);
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorBalanceEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorBalanceEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorBalanceEffect::Shutdown()
{
    SAFE_DELETE(m_poRenderObject);
    SAFE_DELETE(m_poMaterial);
}

// ------------------------------------------------------------------------------------------------
// Name   : GetPhase
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
FLOAT XeColorBalanceEffect::GetPhase(FLOAT fColorSpctr)
{
    if (fColorSpctr > 1.0f) fColorSpctr-= 1.0f;
    if (fColorSpctr > 0.5f) fColorSpctr = 1.0f - fColorSpctr;
    if (fColorSpctr < 0.1666666f) return 1.0f;
    if (fColorSpctr > 0.3333333f) return 0.0f;
    fColorSpctr = fColorSpctr - 0.166666666f;
    fColorSpctr *= 6.0f;
    return 1.0f - fColorSpctr;
}

// ------------------------------------------------------------------------------------------------
// Name   : Apply
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorBalanceEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    if (m_afIntensity[ulContext] == 0.0f) 
    {
        lTextureOut = lTextureIn;
        return;
    }

    VECTOR4FLOAT vColor;

    vColor.x    = m_afIntensity[ulContext] * (1.0f - GetPhase(m_afSpectre[ulContext])) + (1.0f - m_afIntensity[ulContext]);
    vColor.y    = m_afIntensity[ulContext] * (1.0f - GetPhase(m_afSpectre[ulContext] - 0.3333333f + 1.0f )) + (1.0f - m_afIntensity[ulContext]);
    vColor.z    = m_afIntensity[ulContext] * (1.0f - GetPhase(m_afSpectre[ulContext] - 0.6666666f + 1.0f)) + (1.0f - m_afIntensity[ulContext]);
    vColor.w    = 1.0f;

    g_oPixelShaderMgr.SetColorBalance( vColor );

    // blend current target with a fraction of the previous frame
    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    // result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}