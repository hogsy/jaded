//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright 2005 Ubisoft
//
// Author       Eric Le
// Date         1 Feb 2005
//
// $Archive: $
//
// Description
//
//////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#ifdef _XENON
#include "Xgraphics.h"
#endif

#include "XeRenderTargetManager.h"
#include "XeRenderer.h"

XeRenderTargetManager g_oXeRenderTargetMgr;

#define XE_RTMAXSIZE (10 * 1024 * 1024) // 10 MB of EDRAM

XeRenderTargetManager::XeRenderTargetManager()
{
    ZeroMemory(m_astTargets, sizeof(m_astTargets));
}

XeRenderTargetManager::~XeRenderTargetManager()
{
    Shutdown();
}

void XeRenderTargetManager::Init()
{
}

void XeRenderTargetManager::OnDeviceLost(void)
{
}

void XeRenderTargetManager::OnDeviceReset(void)
{
}

void XeRenderTargetManager::Shutdown(void)
{
}

//----------------------------------------------------------------------@FHB--
// XeRenderTargetManager::BeginRenderTarget
// 
// Role   : Associate the device with a rendertarget of required dimensions
//          and format.
//
// Notes  : Supports 2D textures only (for now)
//          
// Params : _uiWidth : 
//          _uiHeight : 
//          _eFormat : 
//          
// Return : BOOL
// 
// Author : Eric Le
// Date   : 2 Feb 2005
//----------------------------------------------------------------------@FHE--
BOOL XeRenderTargetManager::BeginRenderTarget(UINT _uiRTIndex,
                                              IDirect3DTexture9* _pDestTexture,
                                              IDirect3DSurface9* _pDestZ,
                                              BOOL _bUseCurrentZ,
                                              BOOL _bClearColor,
                                              DWORD _dwClearColorValue,
                                              BOOL _bClearZ,
                                              FLOAT _fClearZValue,
                                              BOOL _bUseCurrentBackbuffer, // = FALSE
                                              D3DFORMAT _ColorFormat,   // = D3DFMT_X8R8G8B8
                                              D3DFORMAT _DepthFormat,   // = D3DFMT_D16
                                              D3DMULTISAMPLE_TYPE _eMultiSampleType
                                              )
{
    ERR_X_Assert((_uiRTIndex < XE_MAXSIMULTANEOUSRT) && "Invalid rendertarget index");

    XeRenderTargetInfo* pRTInfo = &m_astTargets[_uiRTIndex];

    // Only allow one RT at a time
    if(pRTInfo->m_bBusy)
    {
        ERR_X_Assert(0 && "A rendertarget is already being rendered into");
        return FALSE;
    }

    if((_pDestTexture == NULL) && (_pDestZ == NULL))
    {
        // Nothing to do!
        return FALSE;
    }

    // Check if our surface has enough room for the request
    IDirect3DSurface9* pColorSurf = NULL;
    D3DSURFACE_DESC stColDesc, stZDesc;

    // Figure out the format of the color buffer
    pRTInfo->m_eColorFormat = _ColorFormat;

    if(_pDestTexture)
    {
        _pDestTexture->GetSurfaceLevel(0, &pColorSurf);
        pColorSurf->GetDesc(&stColDesc);
        pColorSurf->Release();
#if defined(_XENON_RENDER_PC)
        pRTInfo->m_eColorFormat = stColDesc.Format;
#endif
    }

    // Figure out the format of the Z buffer
    pRTInfo->m_eZFormat = _DepthFormat;
    if(_pDestZ && !_bUseCurrentZ)
    {
        _pDestZ->GetDesc(&stZDesc);
#if defined(_XENON_RENDER_PC)
        pRTInfo->m_eZFormat = stZDesc.Format;
#endif
    }

    IDirect3DDevice9* pD3DDevice = g_oXeRenderer.GetDevice();

    pRTInfo->m_bBusy = TRUE;
    pRTInfo->m_pDestColTexture = _pDestTexture;
    pRTInfo->m_pDestZ = _pDestZ;
#ifdef _XENON_RENDER_PC
    pRTInfo->m_pRenderTargetSurface = pColorSurf;
#endif

#ifdef _XENON
    // TODO:
    // Have a list of surface pointers and associated width, height, format
    // and return the right pointer.  Use D3DSURFACE_PARAMETERS to make sure
    // the surface is allocated at the same offset in EDRAM.  They will overlap
    // but this is fine if we allow only 1 to be used at a time.
    HRESULT hr;
    pRTInfo->m_pRenderTargetSurface = NULL;

    if(_pDestTexture)
    {
        D3DSURFACE_PARAMETERS renderTargetParameters = { 0 };

        hr = pD3DDevice->CreateRenderTarget(stColDesc.Width,
                                            stColDesc.Height,
                                            pRTInfo->m_eColorFormat,
                                            _eMultiSampleType,
                                            0,                           // UnusedMultisampleQuality
                                            FALSE,                       // UnusedLockable (FALSE)
                                            &pRTInfo->m_pRenderTargetSurface,
                                            _bUseCurrentBackbuffer ? &renderTargetParameters : NULL);                       // D3DSURFACE_PARAMETERS
    }
    if(_pDestZ && !_bUseCurrentZ)
    {
        hr = pD3DDevice->CreateDepthStencilSurface(stZDesc.Width,
                                                   stZDesc.Height,
                                                   pRTInfo->m_eZFormat,
                                                   _eMultiSampleType,
                                                   0,                   // UnusedLockable (use 0)
                                                   TRUE,                // UnusedDiscard (use TRUE)
                                                   &pRTInfo->m_pDestZ,
                                                   NULL);
    }
#endif

    // Backup the current RT
    pD3DDevice->GetRenderTarget(_uiRTIndex, &pRTInfo->m_pOldRT);
    pD3DDevice->GetDepthStencilSurface(&pRTInfo->m_pOldZ);

    if(_uiRTIndex == 0)
    {
        pD3DDevice->GetViewport(&m_OldViewport);
    }

//    pD3DDevice->GetViewport(&pRTInfo->m_OldViewport);

    pD3DDevice->EndScene();

    // Set the new RT
    if(!_bUseCurrentBackbuffer
#ifdef _XENON // must take changes to format into consideration
        || (pRTInfo->m_eColorFormat != D3DFMT_A8R8G8B8)
#endif
        )
        pD3DDevice->SetRenderTarget(_uiRTIndex, pRTInfo->m_pRenderTargetSurface);
    
    if(!_bUseCurrentZ)
        pD3DDevice->SetDepthStencilSurface(pRTInfo->m_pDestZ);

    // Set a matching viewport
    D3DVIEWPORT9 vp;
    if(_pDestTexture)
    {
        vp.X = 0;
        vp.Y = 0;
        vp.Width = stColDesc.Width;
        vp.Height = stColDesc.Height;
        vp.MinZ = 0.0f;
        vp.MaxZ = 1.0f;
    }
    else
    {
        vp.X = 0;
        vp.Y = 0;
        vp.Width = stZDesc.Width;
        vp.Height = stZDesc.Height;
        vp.MinZ = 0.0f;
        vp.MaxZ = 1.0f;
    }

    if(_uiRTIndex == 0)
    {
        pD3DDevice->SetViewport(&vp);
    }

    pD3DDevice->BeginScene();

    DWORD dwClearFlags = 0;
#if defined(_XENON_RENDER_PC)
    IDirect3DSurface9* apoOldTargets[XE_MAXSIMULTANEOUSRT];
#endif

    if(_bClearColor && (pRTInfo->m_pRenderTargetSurface || _bUseCurrentBackbuffer))
    {
#ifdef _XENON
        DWORD adwClearTargetFlags[4] = { D3DCLEAR_TARGET0, D3DCLEAR_TARGET1, D3DCLEAR_TARGET2, D3DCLEAR_TARGET3 };
        dwClearFlags |= adwClearTargetFlags[_uiRTIndex];

#elif defined(_XENON_RENDER_PC)
        // Dammit to hell.  On PC all targets are cleared. :(
        // Force only this target to be cleared by setting the other ones to NULL.

        dwClearFlags |= D3DCLEAR_TARGET;
        for(UINT uiTargetIdx = 0; uiTargetIdx < XE_MAXSIMULTANEOUSRT; uiTargetIdx++)
        {
            pD3DDevice->GetRenderTarget(uiTargetIdx, &apoOldTargets[uiTargetIdx]);
            if (uiTargetIdx != 0)
                pD3DDevice->SetRenderTarget(uiTargetIdx, NULL);
        }

        pD3DDevice->SetRenderTarget(0, apoOldTargets[_uiRTIndex]);
#endif
    }

    if(_bClearZ && (pRTInfo->m_pDestZ || _bUseCurrentZ))
        dwClearFlags |= D3DCLEAR_ZBUFFER;

    if(dwClearFlags != 0)
    {
        pD3DDevice->Clear(0,
                          NULL,
                          dwClearFlags,
                          _dwClearColorValue,
                          _fClearZValue,
                          0);
    }

#if defined(_XENON_RENDER_PC)

    // Restore rendertargets for Clear() workaround on PC
    if(_bClearColor && (pRTInfo->m_pRenderTargetSurface || _bUseCurrentBackbuffer))
    {
        for(UINT uiTargetIdx = 0; uiTargetIdx < XE_MAXSIMULTANEOUSRT; uiTargetIdx++)
        {
            pD3DDevice->SetRenderTarget(uiTargetIdx, apoOldTargets[uiTargetIdx]);
            if(apoOldTargets[uiTargetIdx] != NULL)
                SAFE_RELEASE(apoOldTargets[uiTargetIdx]);
        }
    }
#endif

    return TRUE;
}

//----------------------------------------------------------------------@FHB--
// XeRenderTargetManager::EndRenderTarget
// 
// Role   : Copies the content of rendertarget to the specified texture.
//          
// Params : _pTexture :     Texture to copy to
//          _bFromZBuffer : Specify to copy from depth buffer instead of color buffer
//          _bPostClear :   Specify whether to clear after the copy or not.
//          
// Return : BOOL
// 
// Author : Eric Le
// Date   : 2 Feb 2005
//----------------------------------------------------------------------@FHE--
DWORD XeRenderTargetManager::EndRenderTarget(UINT _uiRTIndex, BOOL _bXenonResolveZ, BOOL _bClearTarget, DWORD _dwClearCol, BOOL _bClearZ)
{
    ERR_X_Assert((_uiRTIndex < XE_MAXSIMULTANEOUSRT) && "Invalid rendertarget index");

    XeRenderTargetInfo* pRTInfo = &m_astTargets[_uiRTIndex];

    if(!pRTInfo->m_bBusy)
        return FALSE;

    pRTInfo->m_bBusy = FALSE;

    IDirect3DDevice9* pD3DDevice = g_oXeRenderer.GetDevice();

    pD3DDevice->EndScene();
#ifdef _XENON

    DWORD dwFlags;

    if(pRTInfo->m_pDestColTexture)
    {
        D3DVECTOR4 oClearCol;
        DWORD RTResolveFlag[4] = { D3DRESOLVE_RENDERTARGET0,
                                   D3DRESOLVE_RENDERTARGET1,
                                   D3DRESOLVE_RENDERTARGET2,
                                   D3DRESOLVE_RENDERTARGET3 };

        dwFlags = D3DRESOLVE_ALLFRAGMENTS | RTResolveFlag[_uiRTIndex];

        if(_bClearTarget)
        {
            dwFlags |= D3DRESOLVE_CLEARRENDERTARGET;
            oClearCol.x = (float)((_dwClearCol >>  0) & 0xFF) / 255.0f; // R
            oClearCol.y = (float)((_dwClearCol >> 16) & 0xFF) / 255.0f; // G
            oClearCol.z = (float)((_dwClearCol >>  8) & 0xFF) / 255.0f; // B
            oClearCol.w = (float)((_dwClearCol >> 24) & 0xFF) / 255.0f; // A
        }

        if(_bClearZ)
            dwFlags |= D3DRESOLVE_CLEARDEPTHSTENCIL;

        pD3DDevice->Resolve(dwFlags,
                            NULL,                                   // pSourceRect
                            pRTInfo->m_pDestColTexture,             // pDestTexture
                            NULL,                                   // pDestPoint
                            0,                                      // Dest mipmap level
                            0,                                      // DestSliceOrFace
                            &oClearCol,                             // pClearColor
                            1.0f,                                   // ClearZ
                            0,                                      // ClearStencil
                            NULL);                                  // D3DRESOLVE_PARAMETERS
    }
/*
    if(m_pDestZ && _bXenonResolveZ)
    {
        dwFlags = D3DRESOLVE_ALLFRAGMENTS | D3DRESOLVE_DEPTHSTENCIL;
        dwFlags |= D3DRESOLVE_CLEARRENDERTARGET | D3DRESOLVE_CLEARDEPTHSTENCIL;
        pD3DDevice->Resolve(dwFlags,
                            NULL,                                   // pSourceRect
                            m_pDestZ,                               // pDestTexture
                            NULL,                                   // pDestPoint
                            0,                                      // Dest mipmap level
                            0,                                      // DestSliceOrFace
                            NULL,                                   // pClearColor
                            1.0f,                                   // ClearZ
                            0,                                      // ClearStencil
                            NULL);                                  // D3DRESOLVE_PARAMETERS
    }
*/
    // TODO: don't release when we maintain a list of rendertargets
    //popo SAFE_RELEASE(pRTInfo->m_pRenderTargetSurface);
    SAFE_RELEASE(pRTInfo->m_pDestZ);
#else
    // PC

    // Clear Z if we need to
    if(_bClearZ)
    {
        g_oXeRenderer.ClearDepthStencil(1.0f, 0);
    }

#endif // _XENON


    // Restore rendertarget and Z-buffer
    pD3DDevice->SetRenderTarget(_uiRTIndex, pRTInfo->m_pOldRT);
    if(pRTInfo->m_pOldRT)
        pRTInfo->m_pOldRT->Release();

    pD3DDevice->SetDepthStencilSurface(pRTInfo->m_pOldZ);
    if(pRTInfo->m_pOldZ)
        pRTInfo->m_pOldZ->Release();

    // Restore viewport
    if(_uiRTIndex == 0)
    {
        pD3DDevice->SetViewport(&m_OldViewport);
    }

//    pD3DDevice->SetViewport(pRTInfo->m_OldViewport);

    pD3DDevice->BeginScene();

    return TRUE;
}
