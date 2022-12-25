#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeSpinSmoothEffect.h"
#include "XeRenderStateManager.h"
#include "XeRenderTargetManager.h"
#include "XeShader.h"
#include "XeSharedDefines.h"
#include "XeContextManager.h"

#define MINUSSULUS        0.0015f
#define SS_HD_RATIO       0.7f

// ------------------------------------------------------------------------------------------------
// Name   : XeSpinSmoothEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeSpinSmoothEffect::XeSpinSmoothEffect( ) :
XeAfterEffect   ( )
{
    m_poRenderObject = NULL;
    m_poMaterial     = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeSpinSmoothEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeSpinSmoothEffect::~XeSpinSmoothEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeSpinSmoothEffect::ReInit()
{ 
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ulContext++)
    {
        m_fFactor[ulContext] = 0.0f;
    }

    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh( g_oXeRenderer.GetQuadMesh() );
    m_poRenderObject->SetMaterial(m_poMaterial);

    // build material
    m_poMaterial->AddTextureStage();
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetTextureId(0, XESCRATCHBUFFER0_ID);

    m_poMaterial->SetCustomVSFeature(11, 1);
    m_poMaterial->SetCustomPSFeature(14, 1);

    // setup render object
    m_poRenderObject->SetPrimType(XeRenderObject::TriangleStrip);
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_UseTexture | GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeSpinSmoothEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeSpinSmoothEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeSpinSmoothEffect::Shutdown()
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
void XeSpinSmoothEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    if( m_fFactor[ulContext] == 0.0f )
    {
        lTextureOut = lTextureIn;
        return;
    }

    FLOAT fFactor = 0.1f * m_fFactor[ulContext];
    D3DXMATRIX mRot1;
    D3DXMATRIX mRot2;
    D3DXMATRIX mTransform;
    D3DRECT rect;

    BOOL b_ScaleToBlur = g_oXeRenderer.IsHiDef();

    float fInputRatio  = 1.0f;
    float fOutputRatio = b_ScaleToBlur ? SS_HD_RATIO : 1.0f;

    if( b_ScaleToBlur )
    {
        // Build the rect that represents the desired portion of backbuffer to resolve
        rect.x1 =  rect.y1 = 0;
        UINT uiWidth, uiHeight;
        g_oXeRenderer.GetBackbufferResolution( &uiWidth, &uiHeight );
        uiWidth = (UINT) (((float)uiWidth) * fOutputRatio);
        uiHeight= (UINT) (((float)uiHeight) * fOutputRatio);

        // Make multiple of 8 
        if( uiWidth & 7 )
        {
            uiWidth = (uiWidth & ~7) + 8;
        }
        if( uiHeight & 7 )
        {
            uiHeight = (uiHeight & ~7) + 8;
        }
        rect.x2 = uiWidth;
        rect.y2 = uiHeight;
    }

    BOOL b_FirstPass = TRUE;
    while( fFactor > MINUSSULUS )
    {
        BOOL isLastPass = (fFactor * 0.5f) <= MINUSSULUS;

        BOOL b_NeedToClear = b_FirstPass && b_ScaleToBlur;
        if( lTextureIn != XESCRATCHBUFFER0_ID )
        {
            g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID, (b_FirstPass || !b_ScaleToBlur) ? NULL : &rect, b_NeedToClear );
        }
        else if( b_NeedToClear )
        {
            g_oXeRenderer.ClearTarget( 0 );
        }
       
        D3DXMatrixRotationZ( &mRot1, -fFactor );
        D3DXMatrixRotationZ( &mRot2, fFactor );

        if( !isLastPass && b_ScaleToBlur )
        {
            // Scale down by modifying ouput position
            D3DXMatrixIdentity( &mTransform );
            mTransform._11 = fOutputRatio;
            mTransform._41 = fOutputRatio - 1.0f;
            mTransform._22 = fOutputRatio;
            mTransform._42 = -fOutputRatio + 1.0f;

            D3DXMatrixMultiply( &mRot1, &mRot1, &mTransform );
            D3DXMatrixMultiply( &mRot2, &mRot2, &mTransform );

            if( b_FirstPass )
            {
                // We have to blit a downscaled version to avoid artefacts at edges where the rotated quads don't cover
                g_oVertexShaderMgr.SetSpinSmoothTransform( mTransform );
                g_oVertexShaderMgr.SetZoomBlurRatios( fInputRatio, fOutputRatio );

                m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO);
                g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT); 
            }
        }

        g_oVertexShaderMgr.SetSpinSmoothTransform( mRot1 );
        g_oVertexShaderMgr.SetZoomBlurRatios( fInputRatio, fOutputRatio );

        m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO);
        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT); 

        g_oVertexShaderMgr.SetSpinSmoothTransform( mRot2 );
        m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT); 

        lTextureIn = XEBACKBUFFER_ID;
        fInputRatio = fOutputRatio;
        b_FirstPass = FALSE;

        fFactor *= 0.5f;
    }

    lTextureOut = XEBACKBUFFER_ID;
}
