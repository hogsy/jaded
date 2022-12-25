#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeWideScreenEffect.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"
#include "XeShadowManager.h"

// ------------------------------------------------------------------------------------------------
// Name   : XeWideScreenEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeWideScreenEffect::XeWideScreenEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMaterial        = NULL;
    m_poMesh            = NULL;
    m_poIB              = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeWideScreenEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeWideScreenEffect::~XeWideScreenEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWideScreenEffect::ReInit()
{
    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();
    m_poMesh         = new XeMesh();
   
    m_poRenderObject->SetMesh(m_poMesh);
    m_poRenderObject->SetMaterial(m_poMaterial);

    // build mesh
    m_poMesh->ClearAllStreams();
    m_poMesh->AddStream(XEVC_POSITION, 0, NULL, 8 );

    XeBuffer *pBufferObject = m_poMesh->GetStream(0)->pBuffer;
    GEO_Vertex *pVertexBuffer = (GEO_Vertex*) pBufferObject->Lock( 8, 3*sizeof(FLOAT));

    // fullscreen quad
    pVertexBuffer->x = -1.0f;
    pVertexBuffer->y = 1.0f;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer++;

    pVertexBuffer->x = 1.0f;
    pVertexBuffer->y = 1.0f;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer++;

    pVertexBuffer->x = -1.0f;
    pVertexBuffer->y = 0.75;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer++;

    pVertexBuffer->x = 1.0f;
    pVertexBuffer->y = 0.75;
    pVertexBuffer->z = 0.999f;
    pVertexBuffer++;

    pVertexBuffer->x = -1.0f;
    pVertexBuffer->y = -0.75;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer++;

    pVertexBuffer->x = 1.0f;
    pVertexBuffer->y = -0.75;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer++;

    pVertexBuffer->x = -1.0f;
    pVertexBuffer->y = -1.0f;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer++;

    pVertexBuffer->x = 1.0f;
    pVertexBuffer->y = -1.0f;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer++;

    pBufferObject->Unlock(TRUE);

    // Index buffer
    m_poIB = g_XeBufferMgr.CreateIndexBuffer( 12 );

    unsigned short* pIndices = ( unsigned short* )m_poIB->Lock( 12 );

    pIndices[0]  = 0; 
    pIndices[1]  = 2;
    pIndices[2]  = 3; 

    pIndices[3]  = 0;
    pIndices[4]  = 3; 
    pIndices[5]  = 1;

    pIndices[6]  = 4;
    pIndices[7]  = 6;
    pIndices[8]  = 7;

    pIndices[9]  = 4;
    pIndices[10] = 7;
    pIndices[11] = 5;

    m_poIB->Unlock();

    m_poMesh->SetIndices( m_poIB );

    // build material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(FALSE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    m_poMaterial->SetCustomVS( CUSTOM_VS_AFTEREFFECTS);
    m_poMaterial->SetCustomPS( CUSTOM_PS_AFTEREFFECTS);
    m_poMaterial->SetCustomVSFeature( 8,1 );

    // render flags
    m_poRenderObject->SetDrawMask( GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poRenderObject->SetPrimType(XeRenderObject::TriangleList);
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWideScreenEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWideScreenEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWideScreenEffect::Shutdown()
{
    SAFE_RELEASE(m_poIB);
    SAFE_DELETE(m_poRenderObject);
    SAFE_DELETE(m_poMaterial);
    SAFE_DELETE(m_poMesh);
}

// ------------------------------------------------------------------------------------------------
// Name   : Apply
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWideScreenEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
    // blend current target with a fraction of the previous frame
    g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

    // result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}