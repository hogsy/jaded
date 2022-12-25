// ------------------------------------------------------------------------------------------------
// File   : XeBlackWhiteEffect.cpp
// Date   : 2005-04-08
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeBlackWhiteEffect.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"

// ------------------------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : XeBlackWhiteEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeBlackWhiteEffect::XeBlackWhiteEffect()
: XeAfterEffect()
{
    m_poRenderObject = NULL;
    m_poMesh         = NULL;
    m_poMaterial     = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeBlackWhiteEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeBlackWhiteEffect::~XeBlackWhiteEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBlackWhiteEffect::ReInit()
{
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ++ulContext)
    {
        m_afFactor[ulContext] = 0.0f;
    }

    // Allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMesh         = new XeMesh();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh(m_poMesh);
    m_poRenderObject->SetMaterial(m_poMaterial);

    // Build mesh
    m_poMesh->ClearAllStreams();
    m_poMesh->AddStream(XEVC_POSITIONT | XEVC_TEXCOORD0, 0, NULL, 4);

    XeBuffer*            pBufferObject = m_poMesh->GetStream(0)->pBuffer;
    SimpleVertexNoColor* pVertexBuffer = (SimpleVertexNoColor*) pBufferObject->Lock(4, sizeof(SimpleVertexNoColor));

    // Fullscreen quad
    pVertexBuffer->x = fLeft;
    pVertexBuffer->y = fBottom;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer->u1 = 0.0f;
    pVertexBuffer->v1 = 0.0f;
    g_oXeRenderer.AddHalfViewportPixelOffset( pVertexBuffer->u1,  pVertexBuffer->v1 ); 
    pVertexBuffer++;

    pVertexBuffer->x = fLeft;
    pVertexBuffer->y = fTop;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer->u1 = 0.0f;
    pVertexBuffer->v1 = 1.0f;
    g_oXeRenderer.AddHalfViewportPixelOffset( pVertexBuffer->u1,  pVertexBuffer->v1 ); 
    pVertexBuffer++;

    pVertexBuffer->x = fRight;
    pVertexBuffer->y = fBottom;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer->u1 = 1.0f;
    pVertexBuffer->v1 = 0.0f;
    g_oXeRenderer.AddHalfViewportPixelOffset( pVertexBuffer->u1,  pVertexBuffer->v1 ); 
    pVertexBuffer++;

    pVertexBuffer->x = fRight;
    pVertexBuffer->y = fTop;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer->u1 = 1.0f;
    pVertexBuffer->v1 = 1.0f;
    g_oXeRenderer.AddHalfViewportPixelOffset( pVertexBuffer->u1,  pVertexBuffer->v1 ); 
    pVertexBuffer++;

    pBufferObject->Unlock(TRUE);

    // Build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    m_poMaterial->SetColorSource(MAT_Cc_ColorOp_ConstantColor);
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomPSFeature(4, BLACK_WHITE_SHADER);
    m_poMaterial->AddTextureStage();

    // Render flags
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor | GDI_Cul_DM_UseTexture);
    m_poRenderObject->SetPrimType(XeRenderObject::TriangleStrip);

    // Mesh
    m_poMesh->SetIndices(NULL);
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBlackWhiteEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBlackWhiteEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBlackWhiteEffect::Shutdown()
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
void XeBlackWhiteEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    XeBuffer* pBufferObject = m_poMesh->GetStream(0)->pBuffer;
    ULONG     ulColor;

    // Only apply if it is worth it
    if (m_afFactor[ulContext] <= 0.01f)
    {
        lTextureOut = lTextureIn;
        return;
    }

    m_afFactor[ulContext] = MATH_f_FloatLimit(m_afFactor[ulContext], 0.0f, 1.0f);

    // If the backbuffer is not already resolved, resolve it now
    if (lTextureIn == XEBACKBUFFER_ID)
    {
        lTextureIn = XESCRATCHBUFFER0_ID;
        g_oXeTextureMgr.ResolveToScratchBuffer( lTextureIn );
    }

#ifdef ACTIVE_EDITORS
    SimpleVertexNoColor* pVertexBuffer = (SimpleVertexNoColor*)pBufferObject->Lock(4, sizeof(SimpleVertexNoColor));

    M_AdjustUVToViewport(pVertexBuffer, 0.0f, 0.0f); pVertexBuffer++;
    M_AdjustUVToViewport(pVertexBuffer, 0.0f, 1.0f); pVertexBuffer++;
    M_AdjustUVToViewport(pVertexBuffer, 1.0f, 0.0f); pVertexBuffer++;
    M_AdjustUVToViewport(pVertexBuffer, 1.0f, 1.0f); pVertexBuffer++;

    pBufferObject->Unlock(TRUE);
#endif

    // Apply a black and white back buffer
    ulColor = ((ULONG)(m_afFactor[ulContext] * 255.0f) << 24) | 0x00ffffff;
    m_poMaterial->SetConstantColor(ulColor);
    m_poMaterial->SetTextureId(0, lTextureIn);

    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    // Result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}
