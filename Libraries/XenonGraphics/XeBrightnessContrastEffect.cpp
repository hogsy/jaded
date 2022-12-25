#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeBrightnessContrastEffect.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"

// ------------------------------------------------------------------------------------------------
// Name   : XeBrightnessContrastEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeBrightnessContrastEffect::XeBrightnessContrastEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMaterial        = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeBrightnessContrastEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeBrightnessContrastEffect::~XeBrightnessContrastEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeBrightnessContrastEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBrightnessContrastEffect::Enable(ULONG ulContext, ULONG ulParam, BOOL bEnable)
{
    if( ulParam == 0 )
    {
        m_bEnableBrightness[ulContext] = bEnable;
    }
    else if( ulParam == 1 )
    {
        m_bEnableContraste[ulContext] = bEnable;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBrightnessContrastEffect::ReInit()
{
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ulContext++)
    {
        m_afBrightness[ulContext]            = 0.5f;
        m_afContrast[ulContext]              = 0.5f;
    }

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
void XeBrightnessContrastEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBrightnessContrastEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBrightnessContrastEffect::Shutdown()
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
void XeBrightnessContrastEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    FLOAT fValue;
    VECTOR4FLOAT vColor;

    lTextureOut = lTextureIn;

    if (m_afBrightness[ulContext] != 0.5f)
    {
        fValue = fAbs((m_afBrightness[ulContext] - 0.5f) * 2.0f);
        fValue = min( fValue, 1.0f );

        vColor.x = vColor.y = vColor.z = fValue;
        vColor.w = 1.0f;
        g_oPixelShaderMgr.SetColorBalance( vColor );

        m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE);

        if (m_afBrightness[ulContext] < 0.5f)
        {
            // subtract brightness
            m_poMaterial->SetBlendOp(D3DBLENDOP_REVSUBTRACT);
        }
        else
        {
            // add brightness        
            m_poMaterial->SetBlendOp(D3DBLENDOP_ADD);
        }

        // blend current target with a fraction of the previous frame
        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

        // result for this pass is in the current render target
        lTextureOut = XEBACKBUFFER_ID;
    }

    float SavedBr = m_afBrightness[ulContext];

    if( m_afContrast[ulContext] != 0.5f )
    {
        FLOAT fContrast = ((m_afContrast[ulContext] - 0.5f) * 2.0f);
      
        if( fContrast > 0.0f )
        {
            while( fContrast > 0.0f )
            {
                FLOAT fContraste = min( fContrast, 1.0f );
          
                m_poMaterial->SetBlendOp(D3DBLENDOP_ADD);
                m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_ONE);
                vColor.x = vColor.y = vColor.z = fContraste;
                vColor.w = 1.0f;
                g_oPixelShaderMgr.SetColorBalance( vColor );

                g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

                fContrast -= 1.0f;
                fContrast = min(fContrast, 2.0f);
            }
        }
        else
        {
            fContrast = -fContrast;
            fContrast = min( fContrast, 1.0f );
           
            SavedBr = max( SavedBr, 0.0f );
            SavedBr = min( SavedBr, 1 );

            vColor.x = vColor.y = vColor.z = SavedBr;
            vColor.w = fContrast;

            m_poMaterial->SetBlendOp(D3DBLENDOP_ADD);
            m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
            g_oPixelShaderMgr.SetColorBalance( vColor );
            g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
        }

        // result for this pass is in the current render target
        lTextureOut = XEBACKBUFFER_ID;
    }
}