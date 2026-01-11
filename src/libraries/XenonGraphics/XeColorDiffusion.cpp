// ------------------------------------------------------------------------------------------------
// File   : XeColorDiffusion.cpp
// Date   : 
// Author : 
// Descr. : 
//
// Ubi Soft Entertainment Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"

#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeColorDiffusion.h"
#include "XeShader.h"
#include "XeRenderObject.h"
#include "XeMesh.h"
#include "XeMaterial.h"

struct ColorDiffusionVertex
{
    ColorDiffusionVertex( float _pos_x, float _pos_y, float _pos_z, float _u, float _v )
    {
        pos.x = _pos_x;
        pos.y = _pos_y;
        pos.z = _pos_z;
        u = _u;
        v = _v;
    }

    GEO_Vertex  pos;
    float       u;
    float       v;
};

// ------------------------------------------------------------------------------------------------
// Name   : XeColorDiffusionEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeColorDiffusionEffect::XeColorDiffusionEffect( ) :
m_poRenderObject    ( NULL ),
m_poMesh            ( NULL ),
m_poMaterial        ( NULL )
{
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeColorDiffusionEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeColorDiffusionEffect::~XeColorDiffusionEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorDiffusionEffect::ReInit()
{
    // Allocate geometry
    m_poRenderObject = new XeRenderObject();
    m_poMesh         = new XeMesh();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh( m_poMesh );
    m_poRenderObject->SetMaterial( m_poMaterial );

    // Build mesh
    m_poMesh->ClearAllStreams();
    m_poMesh->AddStream( XEVC_POSITION | XEVC_TEXCOORD0, 0, NULL, 4 );

    XeBuffer*             pBufferObject = m_poMesh->GetStream(0)->pBuffer;
    ColorDiffusionVertex* pVertexBuffer = (ColorDiffusionVertex *) pBufferObject->Lock(4, sizeof(ColorDiffusionVertex));

    // Fullscreen quad
    *(pVertexBuffer++)  = ColorDiffusionVertex( fLeft,  fBottom, 0.0f, 0.0f, 0.0f );
    *(pVertexBuffer++)  = ColorDiffusionVertex( fLeft,  fTop,    0.0f, 0.0f, 1.0f );
    *(pVertexBuffer++)  = ColorDiffusionVertex( fRight, fBottom, 0.0f, 1.0f, 0.0f );
    *(pVertexBuffer++)  = ColorDiffusionVertex( fRight, fTop,    0.0f, 1.0f, 1.0f );

    pBufferObject->Unlock(TRUE);

    // Build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaTest( FALSE );
    m_poMaterial->SetColorWrite( TRUE );
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVS(CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->AddTextureStage();
    m_poMaterial->AddTextureStage();
    m_poMaterial->SetFilterMode(0, D3DTEXF_LINEAR, D3DTEXF_LINEAR);
    m_poMaterial->SetFilterMode(1, D3DTEXF_LINEAR, D3DTEXF_LINEAR);
   
    // Render flags
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor | GDI_Cul_DM_UseTexture);
    m_poRenderObject->SetPrimType(XeRenderObject::TriangleStrip);

    // Mesh
    m_poMesh->SetIndices( NULL );
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorDiffusionEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorDiffusionEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorDiffusionEffect::Shutdown()
{
    SAFE_DELETE(m_poRenderObject);
    SAFE_DELETE(m_poMesh);
    SAFE_DELETE(m_poMaterial);
}

// ------------------------------------------------------------------------------------------------
// Name   : Apply
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void   
XeColorDiffusionEffect::PrepareForBlur( ULONG ulContext )
{
    tdstAEGlowPSConsts psConsts;
    float fLumMin = m_fLuminosityMin[ulContext];
    float fLumMax = m_fLuminosityMax[ulContext];

    fLumMin = min( fLumMin, 1.0f );
    fLumMin = max( fLumMin, 0.0f );
    fLumMax = min( fLumMax, 1.0f );
    fLumMax = max( fLumMax, 0.0f );

    if( fLumMin > fLumMax )
    {
        fLumMin = fLumMax;
    }
    psConsts.m_fLuminosityMin  = fLumMin;
    psConsts.m_fLuminosityMax  = fLumMax;
    psConsts.m_fZNear          = m_fZNear[ulContext];
    psConsts.m_fZFar           = m_fZFar[ulContext];

    g_oPixelShaderMgr.SetAEGlowShaderConsts( psConsts );

    if( g_oXeRenderer.HasRenderedGlowingOpaque() && !g_oXeRenderer.HasRenderedGlowingTransparent() )
    {
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID, NULL, TRUE );

        m_poMaterial->SetStencil( TRUE );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILFUNC,   D3DCMP_LESS    );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILFAIL,   D3DSTENCILOP_KEEP );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILZFAIL,  D3DSTENCILOP_KEEP );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILPASS,   D3DSTENCILOP_KEEP );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILREF,    0 );

        // Apply alpha mask to back buffer 
        m_poMaterial->SetTextureId( 0, XESCRATCHBUFFER0_ID );
        m_poMaterial->SetTextureId( 1, g_oXeTextureMgr.GetDepthBufferID() );
        m_poMaterial->SetCustomPSFeature(7, COLOR_DIFFUSION_SHADER_2 );
        m_poMaterial->SetCustomVSFeature(3, BLIT_SHADER_1 );
        m_poMaterial->SetCustomVSFeature(4, 0 );
        m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO );

        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
    }
    else if( !g_oXeRenderer.HasRenderedGlowingOpaque() && g_oXeRenderer.HasRenderedGlowingTransparent() )
    {
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID, NULL, TRUE );

        // Apply alpha mask to back buffer 
        m_poMaterial->SetTextureId( 0, XESCRATCHBUFFER0_ID );
        m_poMaterial->SetTextureId( 1, g_oXeTextureMgr.GetDepthBufferID() );
        m_poMaterial->SetCustomPSFeature(7, 6 );
        m_poMaterial->SetCustomVSFeature(3, BLIT_SHADER_1 );
        m_poMaterial->SetCustomVSFeature(4, 0 );
        m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO );

        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
    }
    else if( g_oXeRenderer.HasRenderedGlowingOpaque() && g_oXeRenderer.HasRenderedGlowingTransparent() )
    {
        m_poMaterial->SetStencil( TRUE );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILFUNC,   D3DCMP_LESS    );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILFAIL,   D3DSTENCILOP_KEEP );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILZFAIL,  D3DSTENCILOP_KEEP );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILPASS,   D3DSTENCILOP_KEEP );
        g_oRenderStateMgr.SetRenderState( D3DRS_STENCILREF,    0 );

        // Combine stencil with alpha mask
        m_poMaterial->SetCustomPSFeature(7, COLOR_DIFFUSION_SHADER_1);
        m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
        m_poMaterial->SetColorWrite( FALSE );
        m_poMaterial->SetAlphaWrite( TRUE );

        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
        g_oXeTextureMgr.ResolveToScratchBuffer( XESCRATCHBUFFER0_ID, NULL, TRUE );

        // Apply alpha mask to back buffer 
        m_poMaterial->SetStencil( FALSE );
        m_poMaterial->SetTextureId( 0, XESCRATCHBUFFER0_ID );
        m_poMaterial->SetTextureId( 1, g_oXeTextureMgr.GetDepthBufferID() );
        m_poMaterial->SetCustomPSFeature(7, 6 );
        m_poMaterial->SetCustomVSFeature(3, BLIT_SHADER_1 );
        m_poMaterial->SetCustomVSFeature(4, 0 );
        m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO );
        m_poMaterial->SetColorWrite( TRUE );

        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : Apply
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeColorDiffusionEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    ERR_X_Assert( m_poRenderObject && m_poMesh && m_poMaterial );
    
    if( m_fIntensity[ulContext] <= 0.0f || (!g_oXeRenderer.HasRenderedGlowingOpaque() && !g_oXeRenderer.HasRenderedGlowingTransparent()))
    {
        return;
    }

    //
    // Do glow mask rejection + luminosity cutoff rejection
    //
    PrepareForBlur( ulContext );

    g_oXeTextureMgr.ResolveToScratchBuffer( XEGLOWMASKSCRATCH_ID, NULL, TRUE );

    //
    // Downscale and blur
    //
    m_poMaterial->SetStencil( FALSE );
    m_poMaterial->SetTextureId( 1, MAT_Xe_InvalidTextureId );
  
    // Setup vertex shader consts
    tdstAEGlowVSConsts vsConsts;
    vsConsts.m_iInputRegion  = 0;
    vsConsts.m_iOutputRegion = 0;
  
    m_poMaterial->SetTextureId( 0, XEGLOWMASKSCRATCH_ID );

    // Loop for reduction using the bilinear filter and blurring
    for( INT iTextureIndex = 1; iTextureIndex < ReductionFactor; ++iTextureIndex )
    {
        // Previous pass is this pass's input
        vsConsts.m_iOutputRegion += 1;
        g_oVertexShaderMgr.SetAEGlowShaderConsts( vsConsts );

        D3DRECT  oRect;
        RegionToRect( oRect, vsConsts.m_iOutputRegion );

        // 
        // Downscale (blit using filtering to region half the size of source region)
        //
        m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO );
        m_poMaterial->SetCustomPSFeature(7, COLOR_DIFFUSION_SHADER_3 );
        m_poMaterial->SetCustomVSFeature(3, 0 );
        m_poMaterial->SetCustomVSFeature(4, COLOR_DIFFUSION_SHADER_1 );
 
        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
        g_oXeTextureMgr.ResolveToScratchBuffer( XEGLOWMASKSCRATCH_ID, (iTextureIndex == 1) ? NULL : &oRect );
       
        // Overwrite current output region
        vsConsts.m_iInputRegion = vsConsts.m_iOutputRegion;

        //
        // Blurring
        //
        if( !(iTextureIndex == 1 && g_oXeRenderer.IsHiDef() ))
        {
            m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_SRCALPHA );
            m_poMaterial->SetCustomPSFeature(7, COLOR_DIFFUSION_SHADER_4 );
            m_poMaterial->SetCustomVSFeature(3, 0 );
            m_poMaterial->SetCustomVSFeature(4, COLOR_DIFFUSION_SHADER_2 );

            // Horizontal blur
            vsConsts.m_bBlurHorizontal = true;
            g_oVertexShaderMgr.SetAEGlowShaderConsts( vsConsts );

            g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
            g_oXeTextureMgr.ResolveToScratchBuffer( XEGLOWMASKSCRATCH_ID, &oRect  );

            // Vertical blur
            vsConsts.m_bBlurHorizontal = false;
            g_oVertexShaderMgr.SetAEGlowShaderConsts( vsConsts );

            g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);
            g_oXeTextureMgr.ResolveToScratchBuffer( XEGLOWMASKSCRATCH_ID, &oRect );
        }
    }

    //
    // Combine the last 4 reduce steps additively with original frame buffer
    //
    m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO );
    m_poMaterial->SetTextureId( 0, XESCRATCHBUFFER0_ID );
    m_poMaterial->SetTextureId( 1, XEGLOWMASKSCRATCH_ID );
    m_poMaterial->SetCustomPSFeature(7, COLOR_DIFFUSION_SHADER_5 );
    m_poMaterial->SetCustomVSFeature(4, COLOR_DIFFUSION_SHADER_3 );

    // Input region is the biggest to use
    vsConsts.m_iInputRegion  = ReductionFactor - 1 - 4;
    vsConsts.m_iOutputRegion = 0;
    g_oVertexShaderMgr.SetAEGlowShaderConsts( vsConsts );

    tdstAEGlowPSConsts psConsts;
    psConsts.m_fIntensity  = m_fIntensity[ulContext];
    psConsts.m_ulGlowColor = m_ulGlowColor[ulContext];
    g_oPixelShaderMgr.SetAEGlowShaderConsts( psConsts );

    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    lTextureOut = XEBACKBUFFER_ID;
}

// ------------------------------------------------------------------------------------------------
// Name   : GetGlowRegionScale
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float    
XeColorDiffusionEffect::GetGlowRegionScale( LONG iRegion )
{
    return 1.0f / pow( 2.0f, (float)iRegion );
}

// ------------------------------------------------------------------------------------------------
// Name   : GetGlowRegionOffset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
float   
XeColorDiffusionEffect::GetGlowRegionOffset( LONG iRegion )
{     
    LONG n = max( iRegion-1, 0 );
    return (1.0f - GetGlowRegionScale( n ));
}

// ------------------------------------------------------------------------------------------------
// Name   : GetGlowRegionOffset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void     
XeColorDiffusionEffect::RegionToRect( D3DRECT & _oRect, LONG iRegion )
{
    float fScale  = GetGlowRegionScale( iRegion );
    float fOffset = GetGlowRegionOffset( iRegion );

    // Compute uv taking into account the viewport (in case we are in the editor) and the half viewport texel
    // to sample center of pixel
    MATH_tdst_Vector vCorners[2] = {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}};
    
    for( LONG i=0; i<2; ++i )
    {
        vCorners[i].x = ( vCorners[i].x * fScale ) + fOffset;
        vCorners[i].y = ( vCorners[i].y * fScale ) + fOffset;
    }
    
    UINT uiWidth, uiHeight;
    g_oXeRenderer.GetBackbufferResolution( &uiWidth, &uiHeight );

    // Multiples of 8        
    _oRect.x1 = (UINT) (vCorners[0].x * uiWidth);
    _oRect.y1 = (UINT) (vCorners[0].y * uiHeight);
    _oRect.x2 = (UINT) (vCorners[1].x * uiWidth);
    _oRect.y2 = (UINT) (vCorners[1].y * uiHeight);

    _oRect.x1 = _oRect.x1 & ~7;
    _oRect.y1 = _oRect.y1 & ~7;
    _oRect.x2 = (_oRect.x2 & ~7) + 8;
    _oRect.y2 = (_oRect.y2 & ~7) + 8;
}
