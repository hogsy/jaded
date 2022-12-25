#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeColorCorrectionEffect.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"

// ------------------------------------------------------------------------------------------------
// Name   : XeColorCorrectionEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeColorCorrectionEffect::XeColorCorrectionEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMaterial        = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeColorCorrectionEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeColorCorrectionEffect::~XeColorCorrectionEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorCorrectionEffect::ReInit()
{
    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh(g_oXeRenderer.GetQuadMesh());
    m_poRenderObject->SetMaterial(m_poMaterial);

    // build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetBlendOp(D3DBLENDOP_ADD);
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetCustomVSFeature( 3, 2 );
    m_poMaterial->SetCustomPSFeature( 15, 1 );
    m_poMaterial->AddTextureStage();
    m_poMaterial->SetTextureId(0, XESCRATCHBUFFER0_ID);
    m_poMaterial->SetFilterMode(0, D3DTEXF_POINT, D3DTEXF_POINT );

    m_poMaterial->SetBlendOp(D3DBLENDOP_ADD);
    m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO);

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
void XeColorCorrectionEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorCorrectionEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorCorrectionEffect::Shutdown()
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
void XeColorCorrectionEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    VECTOR4FLOAT vColorCorrection = {1.0f, 0.0f, 1.0f, 0.0f};

    lTextureOut = lTextureIn;

    if( !g_pXeContextManagerRender->b_EnableColorCorrection )
    {
        return;
    }

    //
    // Saturation
    //
    float fSaturation = g_pXeContextManagerRender->f_XeSaturation;
    fSaturation = min( fSaturation, 1.0f );
    fSaturation = max( fSaturation, 0.0f );
    vColorCorrection.x = fSaturation;

    //
    // Brightness
    //
    VECTOR4FLOAT vBrightness;
    vBrightness.x = g_pXeContextManagerRender->v_XeBrightness.x/5.0f;
    vBrightness.y = g_pXeContextManagerRender->v_XeBrightness.y/5.0f;
    vBrightness.z = g_pXeContextManagerRender->v_XeBrightness.z/5.0f;
    vBrightness.w = 0;

    vBrightness.x = min(vBrightness.x, 1.0f );
    vBrightness.x = max(vBrightness.x, -1.0f );

    vBrightness.y = min(vBrightness.y, 1.0f );
    vBrightness.y = max(vBrightness.y, -1.0f );

    vBrightness.z = min(vBrightness.z, 1.0f );
    vBrightness.z = max(vBrightness.z, -1.0f );
 
    //
    // Contrast
    //
    float SavedBr = (vBrightness.x + vBrightness.y + vBrightness.z )/3.0f;
    if( g_pXeContextManagerRender->f_XeContrast != 0.0f )
    {
        FLOAT fContrast = g_pXeContextManagerRender->f_XeContrast/5.0f;
      
        if( fContrast > 0.0f )
        {
            vColorCorrection.z = 1.0f;

            while( fContrast > 0.0f )
            {
                FLOAT fContrastCurrent = min( fContrast, 1.0f );

                vColorCorrection.z *= (fContrastCurrent + 1.0f);
          
                fContrast -= 1.0f;
                fContrast = min(fContrast, 2.0f);
            }
        }
        else
        {
            fContrast = -fContrast;
            fContrast = min( fContrast, 1.0f );
           
            SavedBr = (SavedBr + 1.0f) / 2.0f;
            SavedBr = max( SavedBr, 0.0f );
            SavedBr = min( SavedBr, 1.0f );

            vColorCorrection.z = (1.0f - fContrast);
            vColorCorrection.w = SavedBr * fContrast;
        }
    }

    // Escape condition
    if( vColorCorrection.x >= 0.995f &&
        fabs(vBrightness.x) <= 0.005f &&
        fabs(vBrightness.y) <= 0.005f &&
        fabs(vBrightness.z) <= 0.005f &&
        fabs(1.0f - vColorCorrection.z) <= 0.005f &&
        fabs(vColorCorrection.w) <= 0.05f )
    {
        return;
    }

    if( lTextureIn != XESCRATCHBUFFER0_ID )
    {
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID );
    }

    g_oPixelShaderMgr.SetColorBalance( vColorCorrection );
    g_oPixelShaderMgr.SetBrightness( vBrightness );

    // Render the effect
    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    // result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}