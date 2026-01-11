#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeBigBlurEffect.h"
#include "XeRenderStateManager.h"

#define MINUSSULUS 0.0015f

// ------------------------------------------------------------------------------------------------
// Name   : XeBigBlurEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeBigBlurEffect::XeBigBlurEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMaterial        = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeBigBlurEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeBigBlurEffect::~XeBigBlurEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBigBlurEffect::ReInit()
{
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ulContext++)
    {
        m_fBlurFactor[ulContext]  = 0.0f;
    }

    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh( g_oXeRenderer.GetQuadMesh() );
    m_poRenderObject->SetMaterial( m_poMaterial );

    // build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO);
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);

    m_poMaterial->SetCustomVSFeature( 5, BIG_BLUR_SHADER_1 );
    m_poMaterial->SetCustomPSFeature( 9, BIG_BLUR_SHADER_1 );

    m_poMaterial->AddTextureStage();
    m_poMaterial->SetTextureId(0, XESCRATCHBUFFER0_ID);

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
void XeBigBlurEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBigBlurEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBigBlurEffect::Shutdown()
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
void XeBigBlurEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    if (m_fBlurFactor[ulContext] == 0.0f) 
    {
        lTextureOut = lTextureIn;
        return;
    }

    CXBBeginEventObject("XeBigBlurEffect::Apply");

    if( lTextureIn != XESCRATCHBUFFER0_ID )
    {
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID );
        lTextureIn = XESCRATCHBUFFER0_ID;
    }

    ERR_X_Assert( lTextureIn == XESCRATCHBUFFER0_ID );

    float fFactor = m_fBlurFactor[ulContext] * 0.02f;

    UINT uiWidth, uiHeight;
    g_oXeRenderer.GetBackbufferResolution( &uiWidth, &uiHeight );
    float fLimit = 1.0f / ((float) uiWidth);

    BOOL b_isDone = FALSE;
    while( !b_isDone )
    {
        CXBBeginEventObject("while(fFactor > MINUSSULUS)");

        // Do a blur pass
        g_oVertexShaderMgr.SetBigBlurFactor( fFactor );
        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
        fFactor *= 0.5f;

        b_isDone = fFactor <= fLimit;
        if( !b_isDone )
        {
            g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID );
        }
    }
}
