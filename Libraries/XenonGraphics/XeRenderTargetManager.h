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
#ifndef __XERENDERTARGETMANAGER_H__INCLUDED
#define __XERENDERTARGETMANAGER_H__INCLUDED

#define XE_MAXSIMULTANEOUSRT    4


typedef struct _XeRenderTargetInfo
{
    BOOL                    m_bBusy;

    D3DFORMAT               m_eColorFormat;
    D3DFORMAT               m_eZFormat;

    IDirect3DSurface9*      m_pRenderTargetSurface;

    IDirect3DSurface9*      m_pOldRT;
    IDirect3DSurface9*      m_pOldZ;
//    D3DVIEWPORT9            m_OldViewport;

    IDirect3DTexture9*      m_pDestColTexture;
    IDirect3DSurface9*      m_pDestZ;
} XeRenderTargetInfo;


class XeRenderTargetManager
{
public:
    XeRenderTargetManager();
    ~XeRenderTargetManager();

    void    Init();
    void    OnDeviceLost(void);
    void    OnDeviceReset(void);
    void    Shutdown(void);

    BOOL    BeginRenderTarget(UINT _uiRTIndex,                          // 0 - 3
                              IDirect3DTexture9* _pDestTexture,
                              IDirect3DSurface9* _pDestZ,
                              BOOL _bUseCurrentZ,
                              BOOL _bClearColor,
                              DWORD _dwClearColorValue,
                              BOOL _bClearZ,
                              FLOAT _fClearZValue,
                              BOOL _bUseCurrentBackbuffer,
                              D3DFORMAT _ColorFormat = D3DFMT_X8R8G8B8, // (Xenon) Temp color buffer format in EDRAM
                              D3DFORMAT _DepthFormat = D3DFMT_D16,// (Xenon) Temp depth buffer format in EDRAM
                              D3DMULTISAMPLE_TYPE _eMultiSampleType = D3DMULTISAMPLE_NONE);     

    DWORD   EndRenderTarget(UINT _uiRTIndex, BOOL _bXenonResolveZ, BOOL _bClearTarget, DWORD _dwClearCol, BOOL _bClearZ);

private:

    XeRenderTargetInfo  m_astTargets[XE_MAXSIMULTANEOUSRT];

    D3DVIEWPORT9        m_OldViewport;
};

extern XeRenderTargetManager g_oXeRenderTargetMgr;

#endif // !defined(__XERENDERTARGETMANAGER_H__INCLUDED)
