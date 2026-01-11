#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeMotionBlurEffect.h"
#include "XeRenderStateManager.h"

#define MOTIONBLUR_VERTEX_STRIDE    7*sizeof(FLOAT)
#define MOTIONBLUR_FRAMERATE_IMPACT 0.4f
#define MOTIONBLUR_REF_FRAMERATE    30.0f

// ------------------------------------------------------------------------------------------------
// Name   : XeMotionBlurEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeMotionBlurEffect::XeMotionBlurEffect()
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
XeMotionBlurEffect::~XeMotionBlurEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeMotionBlurEffect::ReInit()
{
    m_bFirstFrame   = TRUE;
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ulContext++)
    {
        m_fMotionBlurFactor[ulContext]  = 0.0f;
    }

    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh( g_oXeRenderer.GetQuadMesh() );
    m_poRenderObject->SetMaterial( m_poMaterial );

    // build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);

    m_poMaterial->SetCustomVSFeature( 3, BLIT_SHADER_1 );
    m_poMaterial->SetCustomPSFeature( 8, MOTION_BLUR_SHADER_1 );
    
    m_poMaterial->AddTextureStage();
    m_poMaterial->SetTextureId(0, XESCRATCHBUFFER2_ID);

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
void XeMotionBlurEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeMotionBlurEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeMotionBlurEffect::Shutdown()
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
void XeMotionBlurEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    FLOAT               fFactor;
  
    if (m_fMotionBlurFactor[ulContext] == 0.0f) 
    {
        m_bFirstFrame = TRUE;
        return;
    }

    if (!m_bFirstFrame)
    {
        // calculate the fraction of previous frame to blend with current frame
        fFactor = m_fMotionBlurFactor[ulContext] * g_oXeRenderer.GetFPS() / MOTIONBLUR_REF_FRAMERATE;
        fFactor = min(m_fMotionBlurFactor[ulContext] + (MOTIONBLUR_FRAMERATE_IMPACT * (fFactor - m_fMotionBlurFactor[ulContext])), 1.0f);
        
        g_oPixelShaderMgr.SetMotionBlurAlpha( fFactor );

        // blend current target with a fraction of the previous frame
        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

        // result for this pass is in the current render target
        lTextureOut = XEBACKBUFFER_ID;
    }
    else
    {
        lTextureOut = lTextureIn;
    }

    // save result in scratch buffer 2 for next frame
    g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER2_ID );

    m_bFirstFrame = FALSE;
}