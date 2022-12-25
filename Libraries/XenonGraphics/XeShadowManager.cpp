//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright 2005 Ubisoft
//
// Author       Eric Le
// Date         11 Feb 2005
//
// $Archive: $
//
// Description
//
//////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "XeShadowManager.h"
#include "XeTextureManager.h"
#include "XeRenderer.h"
#include "XeRenderTargetManager.h"
#include "XeShader.h"
#include "XePixelShaderManager.h"
#include "XeRenderStateManager.h"

#include <algorithm>

#ifdef _XENON
#include "Xgraphics.h"
#endif

// unique instance of the XeRenderer
XeShadowManager g_oXeShadowManager;

XeShadowManager::XeShadowManager()
{
    m_iNumShadows = 0;
    m_iNumSoftMips = XE_NUMSOFTMIPS;
    m_uiShadowBufferSize = XE_SHADOWBUFFERSIZE;
    m_iCurrentShadowIndex = XE_INVALIDSHADOWID;
    m_vecCookieTextureHandles.reserve(128);

    D3DXMatrixIdentity(&m_OffsetScale);

    m_OffsetScale._11 = 0.5f;
    m_OffsetScale._22 = 0.5f;  // flip Y axis
    m_OffsetScale._33 = 1.0f;   // max Z

    // offset
    m_OffsetScale._41 = 0.5f + 0.5f * XE_SHADOWBUFFERSIZE;
    m_OffsetScale._42 = 0.5f + 0.5f * XE_SHADOWBUFFERSIZE;

    m_fGaussianStrength = 0.005f;

    m_ulNbShadowLights = 0;
    m_ulNbUsedLights   = 0;

#if 0
    m_iJitterTextureSize = 32;
#endif

#if !defined(_XENON)
    m_ZSurface = NULL;
#endif
}

XeShadowManager::~XeShadowManager()
{
}

void XeShadowManager::Init(void)
{
    CreateAllShadowTextures();

    HRESULT hr;

    m_ulNbShadowLights = 0;
    m_ulNbUsedLights   = 0;

    // Custom material that will override the object's material during shadow pass
    m_pMaterial = new XeMaterial();
    m_pMaterial->AddTextureStage();
    m_pMaterial->AddTextureStage();
    m_pMaterial->AddTextureStage();
    m_pMaterial->AddTextureStage();
    m_pMaterial->AddTextureStage();
    m_pMaterial->SetCustomPS(CUSTOM_PS_RENDERSHADOW);

    hr = g_oXeRenderer.GetD3D()->CheckDepthStencilMatch(0,
                                                        D3DDEVTYPE_HAL,
                                                        D3DFMT_X8R8G8B8,
                                                        XE_SHADOWBUFFERFORMAT,
                                                        XE_DEPTHFORMAT);

    ERR_X_Assert((hr == 0) && "Incompatible Rendertarget/DepthStencil formats");

    // Fullscreen shadow quad
    m_poClearObject = new XeRenderObject();
    m_poClearMaterial = new XeMaterial();
    m_poClearMaterial->AddTextureStage();
    m_poClearMaterial->AddTextureStage();

    m_poClearObject->SetMesh(g_oXeRenderer.Get640x480QuadMesh());
    m_poClearObject->SetMaterial(m_poClearMaterial);

}

#if !defined(_XENON)
IDirect3DSurface9* XeShadowManager::CreateShadowZSurface(void)
{
    IDirect3DDevice9*   pD3DDevice = g_oXeRenderer.GetDevice();
    IDirect3DSurface9*  pSurface   = NULL;
    HRESULT hr;

#ifdef _XENON
    UINT renderTargetSize = XGSurfaceSize( XE_SHADOWBUFFERSIZE, XE_SHADOWBUFFERSIZE, XE_SHADOWBUFFERFORMAT, D3DMULTISAMPLE_NONE );
    D3DSURFACE_PARAMETERS depthStencilParameters = { 0 };
#endif
    // Create shadowbuffer texture

    hr = pD3DDevice->CreateDepthStencilSurface(XE_SHADOWBUFFERSIZE,
                                               XE_SHADOWBUFFERSIZE,
                                               XE_DEPTHFORMAT,
                                               D3DMULTISAMPLE_NONE,//g_oXeRenderer.GetMultiSampleType(),
                                               0,
                                               FALSE,
                                               &pSurface,
#ifdef _XENON
                                               &depthStencilParameters
#else
                                               NULL
#endif
                                               );

    return pSurface;
}
#endif

IDirect3DTexture9* XeShadowManager::CreateShadowTexture(Xe2DTexture* _pTempTexture)
{
    IDirect3DDevice9*   pD3DDevice = g_oXeRenderer.GetDevice();
    IDirect3DTexture9*  pTexture   = NULL;

#ifdef _XENON
    _pTempTexture->pTexture = &_pTempTexture->stD3DHeader;

    UINT uiTexSize = XGSetTextureHeader(XE_SHADOWBUFFERSIZE,
                                        XE_SHADOWBUFFERSIZE,
                                        1,
                                        0,
                                        XE_SHADOWBUFFERFORMAT,
                                        0,
                                        0,
                                        XGHEADER_CONTIGUOUS_MIP_OFFSET,
                                        0,
                                        &_pTempTexture->stD3DHeader,
                                        NULL,
                                        NULL);

    _pTempTexture->pD3DBuffer = XPhysicalAlloc(uiTexSize,
                                               MAXULONG_PTR,
                                               4096,
                                               PAGE_READWRITE | PAGE_WRITECOMBINE);

    ERR_X_Assert(_pTempTexture->pD3DBuffer != NULL);

    XGOffsetResourceAddress(&_pTempTexture->stD3DHeader, _pTempTexture->pD3DBuffer);

    pTexture = &_pTempTexture->stD3DHeader;

#else
    HRESULT hr;

    hr = pD3DDevice->CreateTexture(XE_SHADOWBUFFERSIZE,
                                   XE_SHADOWBUFFERSIZE,
                                   1,
                                   D3DUSAGE_RENDERTARGET,
                                   XE_SHADOWBUFFERFORMAT,
                                   D3DPOOL_DEFAULT,
                                   &pTexture,
                                   NULL);
#endif
    return pTexture;
}

#if 0
static const int JITTER_SAMPLES = 8;

void XeShadowManager::InitJitterTexture(void)
{
    // Build the jitter texture
    D3DLOCKED_BOX lb;
    m_pJitterTexture->LockBox(0, &lb, NULL, 0);

    unsigned char *data = (unsigned char *)lb.pBits;

    for (int i = 0; i<m_iJitterTextureSize; i++)
    {
        for (int j = 0; j<m_iJitterTextureSize; j++)
        {
            float rot_offset = ((float)rand() / RAND_MAX - 1) * 2 * 3.1415926f;

            for (int k = 0; k<JITTER_SAMPLES*JITTER_SAMPLES/2; k++)
            {
                int x, y;
                float v[4];

                x = k % (JITTER_SAMPLES / 2);
                y = (JITTER_SAMPLES - 1) - k / (JITTER_SAMPLES / 2);

                v[0] = (float)(x * 2 + 0.5f) / JITTER_SAMPLES;
                v[1] = (float)(y + 0.5f) / JITTER_SAMPLES;
                v[2] = (float)(x * 2 + 1 + 0.5f) / JITTER_SAMPLES;
                v[3] = v[1];

                // jitter
                v[0] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
                v[1] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
                v[2] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
                v[3] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;

                // warp to disk
                float d[4];
                d[0] = sqrtf(v[1]) * cosf(2 * 3.1415926f * v[0] + rot_offset);
                d[1] = sqrtf(v[1]) * sinf(2 * 3.1415926f * v[0] + rot_offset);
                d[2] = sqrtf(v[3]) * cosf(2 * 3.1415926f * v[2] + rot_offset);
                d[3] = sqrtf(v[3]) * sinf(2 * 3.1415926f * v[2] + rot_offset);

                d[0] = (d[0] + 1.0f) / 2.0f;
                data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 0] = (unsigned char)(d[0] * 255);
                d[1] = (d[1] + 1.0f) / 2.0f;
                data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 1] = (unsigned char)(d[1] * 255);
                d[2] = (d[2] + 1.0f) / 2.0f;
                data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 2] = (unsigned char)(d[2] * 255);
                d[3] = (d[3] + 1.0f) / 2.0f;
                data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 3] = (unsigned char)(d[3] * 255);
            }
        }
    }
    m_pJitterTexture->UnlockBox(0);
}
#endif

void XeShadowManager::CreateAllShadowTextures(bool _bForReset)
{
    HRESULT hr;

#if !defined(_XENON)
    m_ZSurface = CreateShadowZSurface();
#endif

    for(UINT uiMipIdx = 0; uiMipIdx < 2; uiMipIdx++)
    {
        for(UINT uiShadowIdx = 0; uiShadowIdx < XE_MAXNUMSHADOWS; uiShadowIdx++)
        {
#ifdef _XENON
            m_ShadowBufferTextures[uiShadowIdx][uiMipIdx] = CreateShadowTexture(&m_SB_D3DTextures[uiShadowIdx][uiMipIdx]);
#else
            m_ShadowBufferTextures[uiShadowIdx][uiMipIdx] = CreateShadowTexture(NULL);
#endif
            if (_bForReset)
            {
                g_oXeTextureMgr.UpdateUserTexture(m_adwShadowBufferHandles[uiShadowIdx][uiMipIdx], m_ShadowBufferTextures[uiShadowIdx][uiMipIdx]);
            }
            else
            {
                m_adwShadowBufferHandles[uiShadowIdx][uiMipIdx] = g_oXeTextureMgr.RegisterUserTexture(m_ShadowBufferTextures[uiShadowIdx][uiMipIdx]);
            }
        }
    }

    // Create "soft" shadow textures
    IDirect3DDevice9* pD3DDevice = g_oXeRenderer.GetDevice();
    UINT uiWidth, uiHeight;
    g_oXeRenderer.GetBackbufferResolution(&uiWidth, &uiHeight);

    uiWidth = 640;
    uiHeight = 480;

    for(UINT uiMipIdx = 0; uiMipIdx < 2; uiMipIdx++)
    {
        for(UINT uiSoftIdx = 0; uiSoftIdx < XE_NUMSOFTMIPS; uiSoftIdx++)
        {
            hr = pD3DDevice->CreateTexture(uiWidth,
                                           uiHeight,
                                           1,
                                           D3DUSAGE_RENDERTARGET,
                                           D3DFMT_A8R8G8B8,
                                           D3DPOOL_DEFAULT,
                                           &m_SoftRTTexture[uiSoftIdx][uiMipIdx],
                                           NULL);

            hr = m_SoftRTTexture[uiSoftIdx][uiMipIdx]->GetSurfaceLevel(0, &m_SoftRTSurface[uiSoftIdx][uiMipIdx]);
            m_SoftRTTexture[uiSoftIdx][uiMipIdx]->Release();

            if (_bForReset)
            {
                g_oXeTextureMgr.UpdateUserTexture(m_dwSoftTextureHandle[uiSoftIdx][uiMipIdx], m_SoftRTTexture[uiSoftIdx][uiMipIdx]);
            }
            else
            {
                m_dwSoftTextureHandle[uiSoftIdx][uiMipIdx] = g_oXeTextureMgr.RegisterUserTexture(m_SoftRTTexture[uiSoftIdx][uiMipIdx]);
            }
        }
    }

    hr = pD3DDevice->CreateTexture(uiWidth,
                                   uiHeight,
                                   1,
                                   D3DUSAGE_RENDERTARGET,
                                   D3DFMT_A8R8G8B8,
                                   D3DPOOL_DEFAULT,
                                   &m_StaticShadowResultTexture,
                                   NULL);

    hr = m_StaticShadowResultTexture->GetSurfaceLevel(0, &m_StaticShadowResultSurface);
    m_StaticShadowResultTexture->Release();

    if (_bForReset)
    {
        g_oXeTextureMgr.UpdateUserTexture(m_StaticShadowResultTextureHandle, m_StaticShadowResultTexture);
    }
    else
    {
        m_StaticShadowResultTextureHandle = g_oXeTextureMgr.RegisterUserTexture(m_StaticShadowResultTexture);
    }


    hr = pD3DDevice->CreateTexture(uiWidth,
                                   uiHeight,
                                   1,
                                   D3DUSAGE_RENDERTARGET,
                                   D3DFMT_A8R8G8B8,
                                   D3DPOOL_DEFAULT,
                                   &m_DynamicShadowResultTexture,
                                   NULL);

    hr = m_DynamicShadowResultTexture->GetSurfaceLevel(0, &m_DynamicShadowResultSurface);
    m_DynamicShadowResultTexture->Release();

    if (_bForReset)
    {
        g_oXeTextureMgr.UpdateUserTexture(m_DynamicShadowResultTextureHandle, m_DynamicShadowResultTexture);
    }
    else
    {
        m_DynamicShadowResultTextureHandle = g_oXeTextureMgr.RegisterUserTexture(m_DynamicShadowResultTexture);
    }

#if 0
    // Create a Z texture that will hold a copy of the backbuffer's Z-buffer
    hr = pD3DDevice->CreateTexture(uiWidth,
                                   uiHeight,
                                   1,
                                   D3DUSAGE_RENDERTARGET,
                                   D3DFMT_R32F,
                                   D3DPOOL_DEFAULT,
                                   &m_BackBufferZ,
                                   NULL);


    // Create jitter volume texture
    hr = pD3DDevice->CreateVolumeTexture(m_iJitterTextureSize,
                                         m_iJitterTextureSize,
                                         JITTER_SAMPLES*JITTER_SAMPLES/2,
                                         1, 
                                         D3DUSAGE_DYNAMIC,
                                         D3DFMT_A8R8G8B8,
                                         D3DPOOL_DEFAULT,
                                         &m_pJitterTexture,
                                         NULL);
    //InitJitterTexture();

    if (_bForReset)
    {
        g_oXeTextureMgr.UpdateUserTexture(m_dwBackBufferZHandle, m_BackBufferZ);
        g_oXeTextureMgr.UpdateUserTexture(m_dwJitterTextureHandle, m_pJitterTexture);
    }
    else
    {
        m_dwBackBufferZHandle = g_oXeTextureMgr.RegisterUserTexture(m_BackBufferZ);
        m_dwJitterTextureHandle = g_oXeTextureMgr.RegisterUserTexture(m_pJitterTexture);
    }
#endif
}

void XeShadowManager::DestroyAllShadowTextures(void)
{
    for(UINT uiMipIdx = 0; uiMipIdx < 2; uiMipIdx++)
    {
        for(UINT uiShadowIdx = 0; uiShadowIdx < XE_MAXNUMSHADOWS; uiShadowIdx++)
        {
#ifdef _XENON_RENDER_PC
            SAFE_RELEASE(m_ShadowBufferTextures[uiShadowIdx][uiMipIdx]);
            g_oXeTextureMgr.UpdateUserTexture(m_adwShadowBufferHandles[uiShadowIdx][uiMipIdx], NULL);
#endif
        }
    }

#if !defined(_XENON)
    SAFE_RELEASE(m_ZSurface);
#endif

    for(UINT uiMipIdx = 0; uiMipIdx < 2; uiMipIdx++)
    {
        for(UINT uiSoftIdx = 0; uiSoftIdx < XE_NUMSOFTMIPS; uiSoftIdx++)
        {
            SAFE_RELEASE(m_SoftRTTexture[uiSoftIdx][uiMipIdx]);
            g_oXeTextureMgr.UpdateUserTexture(m_dwSoftTextureHandle[uiSoftIdx][uiMipIdx], NULL);
        }
    }

    SAFE_RELEASE(m_StaticShadowResultTexture);
    g_oXeTextureMgr.UpdateUserTexture(m_StaticShadowResultTextureHandle, NULL);

    SAFE_RELEASE(m_DynamicShadowResultTexture);
    g_oXeTextureMgr.UpdateUserTexture(m_DynamicShadowResultTextureHandle, NULL);

#if 0
    SAFE_RELEASE(m_BackBufferZ);
    g_oXeTextureMgr.UpdateUserTexture(m_dwBackBufferZHandle, NULL);

    SAFE_RELEASE(m_pJitterTexture);
    g_oXeTextureMgr.UpdateUserTexture(m_dwJitterTextureHandle, NULL);
#endif
}

BOOL XeShadowManager::OnDeviceLost(void)
{
    DestroyAllShadowTextures();
    return TRUE;
}

BOOL XeShadowManager::OnDeviceReset(void)
{
    CreateAllShadowTextures(true);
    return TRUE;
}

void XeShadowManager::Shutdown(void)
{
    DestroyAllShadowTextures();
    SAFE_DELETE(m_pMaterial);
    SAFE_DELETE(m_poClearObject);
    SAFE_DELETE(m_poClearMaterial);
}

void XeShadowManager::AddLight(XeShadowLight* _pLight)
{
    _pLight->ShadowTextureID = (ULONG)MAT_Xe_InvalidTextureId;

    m_vecShadowLights[m_ulNbShadowLights] = *_pLight;
    ++m_ulNbShadowLights;
}

void XeShadowManager::ClearLights(void)
{
    m_ulNbShadowLights = 0;
}

void XeShadowManager::BeginShadowBuffer(UINT _uiShadowID, UINT _uiMip, BOOL _bNeedToClear)
{
    if(_uiShadowID >= m_ulNbUsedLights)
    {
        ERR_X_Assert(0 && "Invalid shadow index");
        return;
    }

    if(m_iCurrentShadowIndex != XE_INVALIDSHADOWID)
    {
        ERR_X_Assert(0 && "A shadow is already being calculated");
        return;
    }

    m_vecUsedLights[_uiShadowID].ShadowTextureID = m_adwShadowBufferHandles[_uiShadowID][_uiMip];

    m_iCurrentShadowIndex = _uiShadowID;
    m_iCurrentShadowMip = _uiMip;

#ifdef _XENON
    g_oXeRenderTargetMgr.BeginRenderTarget(0,
                                           m_ShadowBufferTextures[_uiShadowID][_uiMip],  // Dest texture
                                           NULL,                        // Dest Z
                                           TRUE,                        // Use current Z?
                                           _bNeedToClear,               // Clear color buffer?
                                           0xFFFFFFFF,                  // Color to clear with
                                           _bNeedToClear,               // Clear Z?
                                           1.0f,                        // Z value to clear with
                                           TRUE,                        // Use current back buffer?
                                           XE_SHADOWBUFFERFORMAT,       // Xenon col buffer format
                                           XE_DEPTHFORMAT,              // Xenon Z buffer format
                                           g_oXeRenderer.GetMultiSampleType());      
#else
    g_oXeRenderTargetMgr.BeginRenderTarget(0,
                                           m_ShadowBufferTextures[_uiShadowID][_uiMip],  // Dest texture
                                           m_ZSurface,                  // Dest Z
                                           FALSE,                       // Use current Z?
                                           TRUE,                        // Clear color buffer?
                                           0xFFFFFFFF,                  // Color to clear with
                                           TRUE,                        // Clear Z?
                                           1.0f,                        // Z value to clear with
                                           FALSE,                       // Use current back buffer?
                                           XE_SHADOWBUFFERFORMAT,       // Xenon col buffer format
                                           XE_DEPTHFORMAT,
                                           g_oXeRenderer.GetMultiSampleType());        // Xenon Z buffer format

#endif
}

void XeShadowManager::EndShadowBuffer(BOOL _bNeedToClear)
{
    m_iCurrentShadowIndex = XE_INVALIDSHADOWID;
    g_oXeRenderTargetMgr.EndRenderTarget(0, FALSE, _bNeedToClear, 0xFFFFFFFF, TRUE);
}

D3DXMATRIX* XeShadowManager::GetViewMatrix(INT _iShadowID)
{
    if(_iShadowID == -1)
    {
        _iShadowID = m_iCurrentShadowIndex;
    }

    ERR_X_Assert(_iShadowID != XE_INVALIDSHADOWID);

    if(_iShadowID < (INT)m_ulNbUsedLights)
        return &m_vecUsedLights[_iShadowID].View;

    return NULL;
}

D3DXMATRIX* XeShadowManager::GetProjMatrix(INT _iShadowID)
{
    if(_iShadowID == -1)
    {
        _iShadowID = m_iCurrentShadowIndex;
    }

    ERR_X_Assert(_iShadowID != XE_INVALIDSHADOWID);

    if(_iShadowID < (INT)m_ulNbUsedLights)
    {
        return &m_vecUsedLights[_iShadowID].Projection[m_iCurrentShadowMip];
    }

    return NULL;
}

D3DXMATRIX* XeShadowManager::GetInvCameraProjMatrix(INT _iShadowID)
{
    if(_iShadowID == -1)
    {
        _iShadowID = m_iCurrentShadowIndex;
    }

    ERR_X_Assert(_iShadowID != XE_INVALIDSHADOWID);

    if(_iShadowID < (INT)m_ulNbUsedLights)
        return &m_vecUsedLights[_iShadowID].InvCameraProj;

    return NULL;
}

DWORD XeShadowManager::GetShadowBufferHandle(INT _iShadowID, INT _iMip)
{
    if(_iShadowID == -1)
    {
        _iShadowID = m_iCurrentShadowIndex;
    }

    ERR_X_Assert(_iShadowID != XE_INVALIDSHADOWID);

    if(_iMip == -1)
    {
        _iMip = m_iCurrentShadowMip;
    }

    if(_iShadowID < (INT)m_ulNbUsedLights)
        return m_adwShadowBufferHandles[_iShadowID][_iMip];

    return NULL;
}

XeShadowLight* XeShadowManager::GetShadowLight(INT _iShadowID)
{
    if(_iShadowID == -1)
    {
        // return current 
        _iShadowID = m_iCurrentShadowIndex;
    }

    ERR_X_Assert(_iShadowID != XE_INVALIDSHADOWID);

    if(_iShadowID < (INT)m_ulNbUsedLights)
        return &m_vecUsedLights[_iShadowID];

    return NULL;
}

D3DXMATRIX* XeShadowManager::GetOffsetScaleMatrix(void)
{
    return &m_OffsetScale;
}

void XeShadowManager::BackupLights(void)
{
    L_memcpy(m_vecUsedLights, m_vecShadowLights, m_ulNbShadowLights * sizeof(XeShadowLight));
    m_ulNbUsedLights   = m_ulNbShadowLights;
}


//----------------------------------------------------------------------@FHB--
// XeShadowManager::BeginShadowAccumulation
// 
// Role   : Sets up the rendertarget used for rendering the scene's shadow.
//          This rendertarget will then be blurred and used as a texture
//          for the final compositing of the shadow over the scene.
//          
// Return : void
// 
// Author : Eric Le
// Date   : Mar 2005
//----------------------------------------------------------------------@FHE--
void XeShadowManager::BeginShadowAccumulation(INT _iAccumulationBufferID) 
{
    m_iCurrentShadowMip = _iAccumulationBufferID;
#ifdef _XENON
/*
    // Backup the current backbuffer
    g_oXeTextureMgr.ResolveToScratchBuffer(XESCRATCHBUFFER0_ID);
*/
#endif

    g_oXeRenderTargetMgr.BeginRenderTarget(0,
//                                           m_SoftRTTexture[0][m_iCurrentShadowMip],
                                           _iAccumulationBufferID == 0 ? m_StaticShadowResultTexture : m_DynamicShadowResultTexture,
                                           NULL,
                                           TRUE,            // Use current Z buffer
                                           TRUE,            // Clear color buffer?
                                           0xFFFFFFFF,      // Color buffer clear value
                                           FALSE,           // Clear Z?        
                                           1.0f,            // Clear Z value
#ifdef _XENON
                                           TRUE,            // use current backbuffer
#else
                                           FALSE,
#endif
                                           D3DFMT_X8R8G8B8,
                                           D3DFMT_D16,
                                           g_oXeRenderer.GetMultiSampleType()
                                           );
}

void XeShadowManager::EndShadowAccumulation(void)
{
    g_oXeRenderTargetMgr.EndRenderTarget(0, FALSE, FALSE, 0, FALSE);
}


void XeShadowManager::BlurShadowBuffer(DWORD _dwSourceTextureHandle, IDirect3DTexture9* _pDestTexture)
{
    CXBBeginEventObject oEvent("BlurShadowBuffer");
    
    GetGaussianOffsets7x7(TRUE, GetGaussianStrength(), (D3DXVECTOR4*)m_vHorizontalOffsetsAndWeights);
    GetGaussianOffsets7x7(FALSE, GetGaussianStrength(), (D3DXVECTOR4*)m_vVerticalOffsetsAndWeights);

    g_oXeRenderer.SetRTResolution(640, 480);

    m_poClearMaterial->SetFilterMode(0, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE);
    m_poClearMaterial->SetAddressMode(0);

    m_poClearMaterial->SetColorWrite(TRUE);
    m_poClearMaterial->SetAlphaWrite(TRUE);
    m_poClearMaterial->SetConstantColor(0xFFFFFFFF);
    m_poClearMaterial->SetZState(FALSE, FALSE);
    m_poClearMaterial->SetColorSource(MAT_Cc_ColorOp_ConstantColor);
    
    m_poClearMaterial->SetCustomVS(CUSTOM_VS_BLURSHADOW);

#ifdef _XENON
    // Optimized version of the shader,
    // Avoids 1 texture fetch, alpha blend with EDRAM rendertarget
    m_poClearMaterial->SetCustomPS(CUSTOM_PS_BLURSHADOW);
    m_poClearMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_SRCALPHA);
#else
    m_poClearMaterial->SetCustomPS(CUSTOM_PS_BLURSHADOWPC);
    m_poClearMaterial->SetAlphaBlend(FALSE, D3DBLEND_ONE, D3DBLEND_SRCALPHA);
#endif
    m_poClearMaterial->SetCustomPSFeature(1, 0);

    // render flags
    m_poClearObject->SetDrawMask(GDI_Cul_DM_UseTexture | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poClearObject->SetPrimType(XeRenderObject::TriangleStrip);

    m_bHorizontalBlur = TRUE;

    // Blur shadow texture into destination textures
    for(UINT uiSoftIdx = 0; uiSoftIdx < 2; uiSoftIdx++)
    {
        DWORD dwSource;
        IDirect3DTexture9* pDest;

        if(uiSoftIdx & 1)
        {
            dwSource = m_dwSoftTextureHandle[0][0];
            pDest = _pDestTexture;
        }
        else
        {
            dwSource = _dwSourceTextureHandle;
            pDest = m_SoftRTTexture[0][0];
        }

        m_poClearMaterial->SetTextureId(0, dwSource);

        g_oXeRenderTargetMgr.BeginRenderTarget(0,                           // RT index
                                               pDest,                       // Dest texture 
                                               NULL,                        // Dest Z
#ifdef _XENON
                                               TRUE,                        // Use current Z buffer
#else
                                               FALSE,                       // Use current Z buffer
#endif
                                               FALSE,                       // Clear color buffer?
                                               0xff00ff00,                  // Color buffer clear val
                                               FALSE,                       // Clear Z?        
                                               1.0f,                        // Clear Z value
#ifdef _XENON
                                               TRUE,                        // use current backbuffer
#else
                                               FALSE,                       // use current backbuffer
#endif
                                               D3DFMT_X8R8G8B8,
                                               D3DFMT_D16,
                                               g_oXeRenderer.GetMultiSampleType()
                                               );

        g_oXeRenderer.RenderObject(m_poClearObject, XeFXManager::RP_DEFAULT);

        g_oXeRenderTargetMgr.EndRenderTarget(0,
                                             FALSE,
                                             (uiSoftIdx == 1) ? TRUE : FALSE,
                                             0,
                                             FALSE);

        m_bHorizontalBlur = !m_bHorizontalBlur;
    }

    g_oXeRenderer.ResetRTResolution();
}

// Composite static and dynamic shadow buffers into final buffer (m_SoftRTTexture[2][0] for now)
void XeShadowManager::CalculateShadowResult()
{
    CXBBeginEventObject oEvent("XeShadowManager::CalculateShadowResult");

    g_oXeRenderer.SetRTResolution(640, 480);

    m_poClearObject->SetPrimType(XeRenderObject::TriangleStrip);
    m_poClearObject->SetDrawMask(GDI_Cul_DM_UseTexture | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);

    m_poClearMaterial->SetColorWrite(TRUE);
    m_poClearMaterial->SetAlphaWrite(TRUE);
    m_poClearMaterial->SetAlphaBlend(FALSE);
    m_poClearMaterial->SetConstantColor(0xFFFFFFFF);
    m_poClearMaterial->SetZState(FALSE, FALSE);
    m_poClearMaterial->SetColorSource(MAT_Cc_ColorOp_ConstantColor);
    m_poClearMaterial->SetCustomVS(0);
    m_poClearMaterial->SetCustomPS(0);

    // Shadow quad for static objects
    {
        CXBBeginEventObject oEvent("Static shadow quad");
        g_oXeShadowManager.BeginShadowAccumulation(0);
        g_oXeRenderer.RenderObject(m_poClearObject, XeFXManager::RP_APPLY_SHADOW);
        g_oXeShadowManager.EndShadowAccumulation();
    }

    // Shadow quad for dynamic objects
    {
        CXBBeginEventObject oEvent("Dynamic shadow quad");
        g_oXeShadowManager.BeginShadowAccumulation(1);
        g_oXeRenderer.RenderObject(m_poClearObject,  XeFXManager::RP_APPLY_SHADOW);
        g_oXeShadowManager.EndShadowAccumulation();
    }

    // Composite
    {
        CXBBeginEventObject oEvent("Composite static + dynamic");

        m_poClearMaterial->SetTextureId(0, m_StaticShadowResultTextureHandle);
        m_poClearMaterial->SetFilterMode(0, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE);
        m_poClearMaterial->SetAddressMode(0);

        m_poClearMaterial->SetTextureId(1, m_DynamicShadowResultTextureHandle);
        m_poClearMaterial->SetFilterMode(1, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE);
        m_poClearMaterial->SetAddressMode(1);

        m_poClearMaterial->SetCustomPS(CUSTOM_PS_COMPOSITESHADOW);
        m_poClearObject->SetCustomPS(CUSTOM_PS_COMPOSITESHADOW);

        g_oXeRenderTargetMgr.BeginRenderTarget(0,                           // RT index
                                               m_SoftRTTexture[2][0],       // Dest texture 
                                               NULL,                        // Dest Z
                                               TRUE,                        // Use current Z buffer
                                               FALSE,                        // Clear color buffer?
                                               0x00FF00FF,                  // Color buffer clear val
                                               FALSE,                       // Clear Z?        
                                               1.0f,                        // Clear Z value
#ifdef _XENON 
                                               TRUE,                        // use current backbuffer
#else
                                               FALSE,                       // use current backbuffer
#endif
                                               D3DFMT_X8R8G8B8,
                                               D3DFMT_D16,
                                               g_oXeRenderer.GetMultiSampleType()
                                               );

        g_oXeRenderer.RenderObject(m_poClearObject, XeFXManager::RP_DEFAULT);
        g_oXeRenderTargetMgr.EndRenderTarget(0, FALSE, FALSE, 0, FALSE);
    }

    BlurShadowBuffer(m_dwSoftTextureHandle[2][0], m_SoftRTTexture[2][0]);

    g_oXeRenderer.ResetRTResolution();
}

void XeShadowManager::DrawShadowQuad(INT _iBufferIndex)
{
    CXBBeginEventObject oEvent("XeShadowManager::DrawShadowQuad");

    // USED FOR DEBUGGING PURPOSES ONLY

    g_oXeRenderer.SetRTResolution(640, 480);

    switch(_iBufferIndex)
    {
        case 0:
            m_poClearMaterial->SetTextureId(0, m_StaticShadowResultTextureHandle);
            break;
        case 1:
            m_poClearMaterial->SetTextureId(0, m_DynamicShadowResultTextureHandle);
            break;
        case 2:
            m_poClearMaterial->SetTextureId(0, GetResultTextureHandle());
            break;
    }

    m_poClearMaterial->SetColorWrite(TRUE);
    m_poClearMaterial->SetAlphaWrite(TRUE);
    m_poClearMaterial->SetConstantColor(0xFFFFFFFF);
    m_poClearMaterial->SetZState(FALSE, FALSE);
    m_poClearMaterial->SetColorSource(MAT_Cc_ColorOp_ConstantColor);
    m_poClearMaterial->SetAlphaBlend(FALSE, D3DBLEND_ZERO, D3DBLEND_SRCCOLOR);
    m_poClearMaterial->SetCustomVS(0);
    m_poClearMaterial->SetCustomPS(0);
    // render flags
    m_poClearObject->SetDrawMask(GDI_Cul_DM_UseTexture | GDI_Cul_DM_NotWired | GDI_Cul_DM_DontForceColor);
    m_poClearObject->SetPrimType(XeRenderObject::TriangleStrip);

    // Disable global mul2x !!!
    BOOL bIsMul2X = g_oPixelShaderMgr.IsGlobalMul2XEnabled();
    g_oPixelShaderMgr.EnableGlobalMul2X(FALSE);

    g_oXeRenderer.RenderObject(m_poClearObject, XeFXManager::RP_DEFAULT);

    // Restore global mul2x
    g_oPixelShaderMgr.EnableGlobalMul2X(bIsMul2X);

    g_oXeRenderer.ResetRTResolution();
}

void XeShadowManager::SetCurrentShadowID(INT _iCurrentShadowIndex, INT _iCurrentShadowMip)
{
    if(_iCurrentShadowIndex == XE_INVALIDSHADOWID)
    {
        if(m_pMaterial != NULL)
            m_pMaterial->SetTextureId(0, NULL);
    }
    else
    {
        m_pMaterial->SetTextureId(0, m_adwShadowBufferHandles[_iCurrentShadowIndex][_iCurrentShadowMip]);
    }

    m_iCurrentShadowIndex = _iCurrentShadowIndex;
}
/*
DWORD XeShadowManager::GetSoftTextureHandle(UINT _uiIdx)
{
    ERR_X_Assert(_uiIdx < XE_NUMSOFTMIPS);

    if(_uiIdx < XE_NUMSOFTMIPS)
        return m_dwSoftTextureHandle[_uiIdx];

    return NULL;
}
*/
ULONG XeShadowManager::GetShadowLightTexture(void* _pLight)
{
    ULONG i;

    for (i = 0; i < m_ulNbUsedLights; ++i)
    {
        if (m_vecUsedLights[i].JadeLight == _pLight)
            return m_vecUsedLights[i].ShadowTextureID;
    }

    return (ULONG)MAT_Xe_InvalidTextureId;
}

XeMaterial* XeShadowManager::GetMaterial(XeMaterial* _pCopyFromMat)
{
    if(_pCopyFromMat != NULL)
    {
        m_pMaterial->SetTextureId(0, _pCopyFromMat->GetTextureId(0));
        m_pMaterial->SetAlphaTest(_pCopyFromMat->IsAlphaTestEnabled(), _pCopyFromMat->GetAlphaRef());
        m_pMaterial->SetLocalAlpha(_pCopyFromMat->IsUsingLocalAlpha(), _pCopyFromMat->GetLocalAlpha());
        m_pMaterial->SetInvertAlpha(_pCopyFromMat->IsAlphaInverted());
        m_pMaterial->SetTwoSided(_pCopyFromMat->IsTwoSided());
        m_pMaterial->SetZState(_pCopyFromMat->IsZEnabled(), _pCopyFromMat->IsZWriteEnabled(), _pCopyFromMat->GetZFunc());

        ULONG U, V, W;
        _pCopyFromMat->GetAddressMode(0, &U, &V, &W);
        m_pMaterial->SetAddressMode(0, U, V, W);

        ULONG Min, Mag, Mip;
        _pCopyFromMat->GetFilterMode(0, &Mag, &Min, &Mip);
        m_pMaterial->SetFilterMode(0, Mag, Min, Mip );

        m_pMaterial->SetCustomPSFeature(2, _pCopyFromMat->IsUsingLocalAlpha());
        m_pMaterial->SetCustomPSFeature(3, _pCopyFromMat->IsAlphaTestEnabled());
    }

    XeShadowLight* pCurrentLight = GetShadowLight(-1);
    
    if(pCurrentLight)
        m_pMaterial->SetTextureId(4, pCurrentLight->CookieTexureHandle);
    else
        m_pMaterial->SetTextureId(4, 0xFFFFFFFF);

    return m_pMaterial;
}

UINT XeShadowManager::GetNumLights()
{
    return m_ulNbUsedLights;
}

void XeShadowManager::GetUsedTextures(char* _pc_UsedIndex)
{
    std::vector<SHORT>::iterator it;
    for(it = m_vecCookieTextureHandles.begin(); it < m_vecCookieTextureHandles.end(); ++it)
    {
        SHORT TexIdx = *it;
        _pc_UsedIndex[TexIdx] = 1;
    }
}

void XeShadowManager::RegisterCookieTexture(LIGHT_tdst_Light* _pst_Light)
{
    if(_pst_Light->us_CookieTexture != -1)
    {
        m_vecCookieTextureHandles.push_back(_pst_Light->us_CookieTexture);
    }
}

void XeShadowManager::UnRegisterCookieTexture(LIGHT_tdst_Light* _pst_Light)
{
    if(_pst_Light->us_CookieTexture != -1)
    {
        int iNumCookieTextures = m_vecCookieTextureHandles.size();

        for(int i = 0; i < iNumCookieTextures; i++)
        {
            if(m_vecCookieTextureHandles[i] == _pst_Light->us_CookieTexture)
            {
                // Found it
                m_vecCookieTextureHandles[i] = m_vecCookieTextureHandles[iNumCookieTextures - 1];
                m_vecCookieTextureHandles.resize(iNumCookieTextures - 1);
            }
        }
    }
}
 
bool CompareDistance(XeShadowLight& oLight1, XeShadowLight& oLight2)
{
    return (oLight1.Priority > oLight2.Priority);
}

void XeShadowManager::SortLights()
{
    std::sort(m_vecUsedLights, m_vecUsedLights + m_ulNbUsedLights, CompareDistance);

    INT iMaxShadows = g_oXeRenderer.GetMaxShadows();
    int iNumLights = min(m_ulNbUsedLights, (UINT)iMaxShadows);
    for(int iLightIdx = 0; iLightIdx < iNumLights; iLightIdx++)
    {
        XeShadowLight* pLight = &m_vecUsedLights[iLightIdx];

        // Calculate clipping planes for light
        if(pLight->LightType == LIGHT_TYPE_SPOT)
        {
            float fAngle = pLight->Penumbra * 0.5f + 1.57079632679489661923f;
            float fSinAngle = sinf(fAngle);
            float fCosAngle = cosf(fAngle);

            // Top plane
            pLight->ClipPlanes[0].x = 0.0f;
            pLight->ClipPlanes[0].y = -fCosAngle;
            pLight->ClipPlanes[0].z = fSinAngle;

            // Bottom plane
            pLight->ClipPlanes[1].x = 0.0f;
            pLight->ClipPlanes[1].y = -fCosAngle;
            pLight->ClipPlanes[1].z = -fSinAngle;

            // Left plane
            pLight->ClipPlanes[2].x = fSinAngle;
            pLight->ClipPlanes[2].y = -fCosAngle;
            pLight->ClipPlanes[2].z = 0.0f;

            // Right plane
            pLight->ClipPlanes[3].x = -fSinAngle;
            pLight->ClipPlanes[3].y = -fCosAngle;
            pLight->ClipPlanes[3].z = 0.0f;
        }
    }
}

BOOL XeShadowManager::IsLightUsedForShadows(void* _pLight, int& _iChannel)
{
    INT iMaxShadows = g_oXeRenderer.GetMaxShadows();

    _iChannel = -1; 

    if(iMaxShadows == 0)
    {
        return FALSE;
    }

    int iNumLights = min(m_ulNbUsedLights, (UINT)iMaxShadows);
    for(int iLightIdx = 0; iLightIdx < iNumLights; iLightIdx++)
    {
        void* pLight = m_vecUsedLights[iLightIdx].JadeLight;
        if(pLight == _pLight)
        {
            _iChannel = iLightIdx;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL XeShadowManager::IsVisibleFromLight(XeRenderObject* _pObj)
{
    if(!_pObj->GetMesh()->HasBoundingVolume())
        return TRUE;

    ERR_X_Assert(m_iCurrentShadowIndex != XE_INVALIDSHADOWID);
    ERR_X_Assert(m_iCurrentShadowIndex < (INT)m_ulNbUsedLights);

    XeShadowLight* pLight = &m_vecUsedLights[m_iCurrentShadowIndex];

    MATH_tdst_Vector temp = _pObj->GetMesh()->GetBoundingVolumeCenter();
    D3DXVECTOR3 Center(temp.x, temp.y, temp.z);
    float Radius = _pObj->GetMesh()->GetBoundingVolumeRadius();

    D3DXMATRIX LocalToLight;
    D3DXMatrixMultiply(&LocalToLight, _pObj->GetWorldMatrix(), &pLight->View);

    // Transform bounding sphere center to light space
    D3DXVECTOR3 temp2;
    D3DXVec3TransformCoord(&temp2, &Center, &LocalToLight);
    Center.x = temp2.x;
    Center.y = temp2.z;
    Center.z = -temp2.y;

    // Test bounding sphere against Near/Far of light
    if(-Center.y < (pLight->ShadowNear - Radius))
        return FALSE;

    if(-Center.y > (pLight->ShadowFar + Radius))
        return FALSE;

    if(pLight->LightType == LIGHT_TYPE_SPOT)
    {
        // Test bounding sphere against planes of light frustum
        for(int iPlaneIndex = 0; iPlaneIndex < 4; iPlaneIndex++)
        {
            float dot = D3DXVec3Dot(&Center, &pLight->ClipPlanes[iPlaneIndex]);
            if(dot > Radius)
                return FALSE;
        }
    }
    else if(pLight->LightType == LIGHT_TYPE_CYLINDRICAL_SPOT)
    {
        // Find distance between object center and cylinder axis
        D3DXVECTOR3 PointOnAxis(0.0f, Center.y, 0.0f);
        D3DXVECTOR3 Difference;

        D3DXVec3Subtract(&Difference, &PointOnAxis, &Center);
        float Distance = D3DXVec3Length(&Difference);

        float CylinderRadius = pLight->CylinderRadius;
        if(Distance > (CylinderRadius + Radius))
            return FALSE;
    }

    return TRUE;
}

void XeShadowManager::IsForceReceiver(VECTOR4FLOAT *_pFlags)
{
    // TODO: OPTIMIZE, only set 0 to unused light slots
    _pFlags->component[0] = 0;
    _pFlags->component[1] = 0;
    _pFlags->component[2] = 0;
    _pFlags->component[3] = 0;

    INT iMaxShadows = g_oXeRenderer.GetMaxShadows();

    int iNumLights = min(m_ulNbUsedLights, (UINT)iMaxShadows);
    for(int iLightIdx = 0; iLightIdx < iNumLights; iLightIdx++)
    {
        _pFlags->component[iLightIdx] = m_vecUsedLights[iLightIdx].ForceStaticReceiver ? 0.4995f : 0.0f;
    }
}

UINT XeShadowManager::SetShadowBufferSize(UINT _uiSize)
{
#ifdef _XENON
    if(_uiSize == m_uiShadowBufferSize)
        return m_uiShadowBufferSize;

    m_uiShadowBufferSize = min(_uiSize, XE_SHADOWBUFFERSIZE);

    for(UINT uiMipIdx = 0; uiMipIdx < 2; uiMipIdx++)
    {
        for(UINT uiShadowIdx = 0; uiShadowIdx < XE_MAXNUMSHADOWS; uiShadowIdx++)
        {
            XGSetTextureHeader(m_uiShadowBufferSize,    // Width
                               m_uiShadowBufferSize,    // Height
                               1,                       // Mips
                               0,                       // Levels, ignored
                               XE_SHADOWBUFFERFORMAT,   // D3DFORMAT
                               0,                       // Pool, ignored
                               0,                       // BaseOffset
                               0,                       // MipOffset, ignored when Mips = 1
                               0,                       // Pitch
                               //m_ShadowBufferTextures[uiShadowIdx][uiMipIdx],
                               &m_SB_D3DTextures[uiShadowIdx][uiMipIdx].stD3DHeader,
                               NULL,                    // pBaseSize
                               NULL                     // pMipSize
                               );

            XGOffsetResourceAddress(&m_SB_D3DTextures[uiShadowIdx][uiMipIdx].stD3DHeader,
                                    m_SB_D3DTextures[uiShadowIdx][uiMipIdx].pD3DBuffer);

        }
    }

    return m_uiShadowBufferSize;
#else
    return XE_SHADOWBUFFERSIZE;
#endif
}
