// ------------------------------------------------------------------------------------------------
// File   : XeBKQuadAE.cpp
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
#include "XeBKQuadAE.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"

// ------------------------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : XeBKQuadAE
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeBKQuadAE::XeBKQuadAE()
: XeAfterEffect()
{
    m_poRenderObject = NULL;
    m_poMesh         = NULL;
    m_poMaterial     = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeBKQuadAE
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeBKQuadAE::~XeBKQuadAE()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBKQuadAE::ReInit()
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
    m_poMesh->AddStream(XEVC_POSITIONT, 0, NULL, 4);

    XeBuffer*         pBufferObject = m_poMesh->GetStream(0)->pBuffer;
    SimpleVertexOnly* pVertexBuffer = (SimpleVertexOnly*) pBufferObject->Lock(4, sizeof(SimpleVertexOnly));

    // Fullscreen quad
    pVertexBuffer->x = fLeft;
    pVertexBuffer->y = fBottom;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x = fLeft;
    pVertexBuffer->y = fTop;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x = fRight;
    pVertexBuffer->y = fBottom;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x = fRight;
    pVertexBuffer->y = fTop;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer++;

    pBufferObject->Unlock(TRUE);

    // Build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    m_poMaterial->SetColorSource(MAT_Cc_ColorOp_ConstantColor);
    m_poMaterial->SetCustomPS(CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomPSFeature(11, 1);
    m_poMaterial->AddTextureStage();

    // Render flags
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
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
void XeBKQuadAE::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBKQuadAE::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBKQuadAE::Shutdown()
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
void XeBKQuadAE::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
#ifdef VIDEOCONSOLE_ENABLE
    if(NoBlackQuad)
        return;
#endif

    XeBuffer* pBufferObject = m_poMesh->GetStream(0)->pBuffer;
    ULONG     ulColor;

    // Only apply if it is worth it
    if (m_afFactor[ulContext] <= 0.01f)
    {
        lTextureOut = lTextureIn;
        return;
    }

    m_afFactor[ulContext] = MATH_f_FloatLimit(m_afFactor[ulContext], 0.0f, 1.0f);

    // Apply an alpha blended quad onto the back buffer
    ulColor = ((ULONG)(m_afFactor[ulContext] * 255.0f) << 24);
    m_poMaterial->SetConstantColor(ulColor);

    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    // Result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}
