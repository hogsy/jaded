#include "Precomp.h"
#include "XeRenderer.h"
#include "XeTextureManager.h"
#include "XeSafeFrameEffect.h"
#include "XeRenderStateManager.h"
#include "XeShader.h"

#define SAFE_FRAME_RATIO 0.9f

// ------------------------------------------------------------------------------------------------
// Name   : XeSafeFrameEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeSafeFrameEffect::XeSafeFrameEffect()
: XeAfterEffect()
{
    m_poRenderObject    = NULL;
    m_poMaterial        = NULL;
    m_poMesh            = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeFogEffect
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeSafeFrameEffect::~XeSafeFrameEffect()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : ReInit
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeSafeFrameEffect::ReInit()
{
    // allocate containers
    m_poRenderObject = new XeRenderObject();
    m_poMaterial     = new XeMaterial();
    m_poMesh         = new XeMesh();

    // build material
    m_poRenderObject->SetMaterial(m_poMaterial);
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetAlphaBlend(FALSE);
    m_poMaterial->SetColorSource(MAT_Cc_ColorOp_ConstantColor);
    m_poMaterial->SetConstantColor(0xFFFFFFFF);

    // render flags
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poRenderObject->SetPrimType(XeRenderObject::LineStrip);

    // build mesh
    m_poRenderObject->SetMesh(m_poMesh);
    m_poMesh->ClearAllStreams();

    FLOAT fBorder = (1.0f - SAFE_FRAME_RATIO);

    VECTOR4FLOAT aoBuffer[5];
    VECTOR4FLOAT *pVertexBuffer = &aoBuffer[0];

    pVertexBuffer->x = -1.0f + fBorder;
    pVertexBuffer->y =  1.0f - fBorder;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->w = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x = 1.0f - fBorder;
    pVertexBuffer->y = 1.0f - fBorder;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->w = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x =  1.0f - fBorder;
    pVertexBuffer->y = -1.0f + fBorder;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->w = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x = -1.0f + fBorder;
    pVertexBuffer->y = -1.0f + fBorder;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->w = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x = -1.0f + fBorder;
    pVertexBuffer->y =  1.0f - fBorder;
    pVertexBuffer->z = 0.0f;
    pVertexBuffer->w = 1.0f;

    m_poMesh->AddStream(XEVC_POSITIONT, 0, &aoBuffer[0], 5);
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceLost
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeSafeFrameEffect::OnDeviceLost()
{
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : OnDeviceReset
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeSafeFrameEffect::OnDeviceReset()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : Shutdown
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeSafeFrameEffect::Shutdown()
{
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
void XeSafeFrameEffect::Apply(ULONG ulContext, LONG lTextureIn, LONG &lTextureOut)
{
	g_oFXManager.BeginUpdate(m_poRenderObject, XeFXManager::RP_DEFAULT);
	g_oFXManager.UpdateVSFeatures();
	g_oFXManager.UpdatePSFeatures();
	g_oFXManager.EndUpdate();

    // Send object to VS Manager
    g_oVertexShaderMgr.Update(m_poRenderObject);

    // Update pixel shader
    g_oPixelShaderMgr.Update(m_poRenderObject);

    // Update RenderStateMgr
    g_oRenderStateMgr.Update(m_poRenderObject);

    HRESULT hr = m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 4);

    ERR_X_Assert(SUCCEEDED(hr));

    // result for this pass is in the current render target
    lTextureOut = XEBACKBUFFER_ID;
}