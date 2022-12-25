// ------------------------------------------------------------------------------------------------
// File   : XeLightningAE.cpp
// Date   : 2005-08-30
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
#include "XeRenderStateManager.h"
#include "XeShader.h"
#include "XeLightningAE.h"

// ------------------------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------------------------
const ULONG AE_LIGHTNING_XE_VERTEX_STRIDE = 4 * sizeof(FLOAT);

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

XeLightningAE::XeLightningAE()
: XeAfterEffect(), m_poRenderObject(NULL), m_poMesh(NULL), m_poMaterial(NULL)
{
}

XeLightningAE::~XeLightningAE()
{
    Shutdown();
}

void XeLightningAE::ReInit()
{
    for (ULONG ulContext = 0; ulContext < AE_CONTEXT_COUNT; ulContext++)
    {
        m_afBrightness[ulContext] = 0.5f;
        m_afContrast[ulContext]   = 0.5f;
    }

    m_poRenderObject = new XeRenderObject();
    m_poMesh         = new XeMesh();
    m_poMaterial     = new XeMaterial();

    m_poRenderObject->SetMesh(m_poMesh);
    m_poRenderObject->SetMaterial(m_poMaterial);

    // Create the mesh
    m_poMesh->ClearAllStreams();
    m_poMesh->AddStream(XEVC_POSITIONT, 0, NULL, 4);

    XeBuffer*         pBufferObject = m_poMesh->GetStream(0)->pBuffer;
    SimpleVertexOnly* pVertexBuffer = (SimpleVertexOnly*)pBufferObject->Lock(4, AE_LIGHTNING_XE_VERTEX_STRIDE);

    // Fullscreen quad
    pVertexBuffer->x   = fLeft;
    pVertexBuffer->y   = fBottom;
    pVertexBuffer->z   = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x   = fLeft;
    pVertexBuffer->y   = fTop;
    pVertexBuffer->z   = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x   = fRight;
    pVertexBuffer->y   = fBottom;
    pVertexBuffer->z   = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer++;

    pVertexBuffer->x   = fRight;
    pVertexBuffer->y   = fTop;
    pVertexBuffer->z   = 0.0f;
    pVertexBuffer->rhw = 1.0f;
    pVertexBuffer++;

    pBufferObject->Unlock(TRUE);

    // Material
    m_poMaterial->SetZState(FALSE, FALSE);
    m_poMaterial->SetColorSource(MAT_Cc_ColorOp_ConstantColor);

    // Render object settings
    m_poRenderObject->SetDrawMask(GDI_Cul_DM_UseRLI | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poRenderObject->SetPrimType(XeRenderObject::TriangleStrip);

    // No indices
    m_poMesh->SetIndices(NULL);
}

void XeLightningAE::OnDeviceLost()
{
    Shutdown();
}

void XeLightningAE::OnDeviceReset()
{
    ReInit();
}

void XeLightningAE::Shutdown()
{
    SAFE_DELETE(m_poRenderObject);
    SAFE_DELETE(m_poMesh);
    SAFE_DELETE(m_poMaterial);
}

void XeLightningAE::Apply(ULONG _ulContext, LONG _lTextureIn, LONG& _rlTextureOut)
{
    FLOAT fValue;
    UCHAR ucColor;
    ULONG ulColor;

    _rlTextureOut = _lTextureIn;

    if (m_afBrightness[_ulContext] != 0.5f)
    {
        fValue = fAbs((m_afBrightness[_ulContext] - 0.5f) * 2.0f);
        fValue = fMin(fValue, 1.0f);

        ucColor = (UCHAR)(fValue * 255.0f);
        ulColor = ucColor | (ucColor << 8) | (ucColor << 16) | 0x80000000;

        m_poMaterial->SetConstantColor(ulColor);
        m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE);

        if (m_afBrightness[_ulContext] < 0.5f)
        {
            m_poMaterial->SetBlendOp(D3DBLENDOP_REVSUBTRACT);
        }
        else
        {
            m_poMaterial->SetBlendOp(D3DBLENDOP_ADD);
        }

        g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

        _rlTextureOut = XEBACKBUFFER_ID;
    }

    if (m_afContrast[_ulContext] != 0.5f)
    {
        FLOAT fContrast = fAbs((m_afContrast[_ulContext] - 0.5f) * 2.0f);
        BOOL  bContinue = TRUE;

        m_poMaterial->SetAlphaBlend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_ONE);

        if (m_afContrast[_ulContext] < 0.5f)
        {
            m_poMaterial->SetBlendOp(D3DBLENDOP_REVSUBTRACT);
        }
        else
        {
            m_poMaterial->SetBlendOp(D3DBLENDOP_ADD);
        }

        while (bContinue)
        {
            fValue  = fMin(fContrast, 1.0f);
            ucColor = (UCHAR)(fValue * 255.0f);
            ulColor = ucColor | (ucColor << 8) | (ucColor << 16) | (ucColor << 24);

            m_poMaterial->SetConstantColor(ulColor);

            g_oXeRenderer.RenderObject(m_poRenderObject, XeFXManager::RP_DEFAULT);

            if ((m_afContrast[_ulContext] < 0.5f) || (fContrast <= 1.0f))
                bContinue = FALSE;
            else
                fContrast -= 1.0f;

            if (fContrast > 2.0f)
                fContrast = 2.0f;
        }

        _rlTextureOut = XEBACKBUFFER_ID;
    }
}
