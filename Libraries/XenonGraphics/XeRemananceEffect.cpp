#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeRemananceEffect.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"

#define REMANANCE_BLUR_FACTOR       0.02f
#define REMANANCE_STEP_COUNT        4

// ------------------------------------------------------------------------------------------------
// Name   : XeRemananceEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeRemananceEffect::XeRemananceEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMaterial        = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeRemananceEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeRemananceEffect::~XeRemananceEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeRemananceEffect::ReInit()
{
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ulContext++)
    {
        m_afFactor[ulContext] = 0.0f;
    }

    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh( g_oXeRenderer.GetQuadMesh() );
    m_poRenderObject->SetMaterial( m_poMaterial );

    // build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->AddTextureStage();
    m_poMaterial->AddTextureStage();
 
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
void XeRemananceEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeRemananceEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeRemananceEffect::Shutdown()
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
void XeRemananceEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    if (m_afFactor[ulContext] <= 0.01f) 
    {
        lTextureOut = lTextureIn;
        return;
    }

    CXBBeginEventObject("XeRemananceEffect::Apply");

    if( lTextureIn != XESCRATCHBUFFER0_ID )
    {
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID );
        lTextureIn = XESCRATCHBUFFER0_ID;
    }

    ERR_X_Assert( lTextureIn == XESCRATCHBUFFER0_ID );

    //
    // Big blur of back buffer
    //
    m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO);

    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetCustomVSFeature( 5, 1 );
    m_poMaterial->SetCustomPSFeature( 9, 1 );

    m_poMaterial->SetTextureId(0, XESCRATCHBUFFER0_ID );
    m_poMaterial->SetTextureId(1, MAT_Xe_InvalidTextureId );

    float fFactor = REMANANCE_BLUR_FACTOR;
  
    for( ULONG pass=0; pass<REMANANCE_STEP_COUNT; ++pass )
    {
        CXBBeginEventObject("Blur pass");

        // Do a blur pass
        g_oVertexShaderMgr.SetBigBlurFactor( fFactor );

        if( (pass == (REMANANCE_STEP_COUNT-1)) )
        {
            // Last pass must also combine with original
            m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
            m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
            m_poMaterial->SetCustomVSFeature( 6, 1 );
            m_poMaterial->SetCustomPSFeature( 2, 1 );
            m_poMaterial->SetTextureId(1, XESCRATCHBUFFER0_ID );
            g_oPixelShaderMgr.SetRemananceFactor( m_afFactor[ulContext] );
        }

        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
        fFactor *= 0.5f;

        if( pass < (REMANANCE_STEP_COUNT-1) )
        {
            g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER1_ID );
            m_poMaterial->SetTextureId(0, XESCRATCHBUFFER1_ID );
        }
    }

    //m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    //m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    //m_poMaterial->SetCustomVSFeature( 6, 1 );
    //m_poMaterial->SetCustomPSFeature( 2, 2 );
    //g_oPixelShaderMgr.SetRemananceFactor( m_afFactor[ulContext] );
    //m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_ONE);
    //g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    //m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    //m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    //m_poMaterial->SetCustomVSFeature( 6, 1 );
    //m_poMaterial->SetCustomPSFeature( 2, 3 );
    //g_oPixelShaderMgr.SetRemananceFactor( m_afFactor[ulContext] );
    // m_poMaterial->SetTextureId(0, XESCRATCHBUFFER0_ID );
    //m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
    //g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    lTextureOut = XEBACKBUFFER_ID;
}