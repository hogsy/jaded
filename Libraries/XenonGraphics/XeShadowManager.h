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
#ifndef __XESHADOWMANAGER_H__INCLUDED
#define __XESHADOWMANAGER_H__INCLUDED

#include "XeMaterial.h"
#include "XeTextureManager.h"
#include <vector>

#ifdef ACTIVE_EDITORS
    #define XE_SHADOWBUFFERSIZE     1024
#else
    #define XE_SHADOWBUFFERSIZE     832
#endif

#define XE_SHADOWBUFFERFORMAT   D3DFMT_R32F
#define XE_DEPTHFORMAT          D3DFMT_D24S8
#define XE_MAXNUMSHADOWS        3
#define XE_INVALIDSHADOWID      -1234
#define XE_NUMSOFTMIPS          3

#define XE_MAX_SHADOW_LIGHTS 256

class XeShadowManager
{
public:

    XeShadowManager();
    ~XeShadowManager();

    void                        Init(void);
    BOOL                        OnDeviceLost(void);
    BOOL                        OnDeviceReset(void);
    void                        Shutdown(void);

    void                        GetUsedTextures(char* _pc_UsedIndex);
    void                        RegisterCookieTexture(LIGHT_tdst_Light* _pst_Light);
    void                        UnRegisterCookieTexture(LIGHT_tdst_Light* _pst_Light);

    // Indicates the beginning and end of a single shadow buffer in a rendertarget texture
    void                        BeginShadowBuffer(UINT _uiShadowID, UINT _uiMip, BOOL _bNeedToClear);
    void                        EndShadowBuffer(BOOL _bNeedToClear);

    // Indicates the beginning and end of the application of all shadow buffers on the scene
    // in a rendertarget texture
    void                        BeginShadowAccumulation(INT _iAccumulationBufferID);
    void                        EndShadowAccumulation(void);
    void                        BlurShadowBuffer(DWORD _dwSourceTextureHandle, IDirect3DTexture9* _pDestTexture);
    void                        CalculateShadowResult(void);

    void                        AddLight(XeShadowLight* _pLight);
    void                        ClearLights(void);
    void                        BackupLights(void);
    UINT                        GetNumLights(void);
    void                        SortLights(void);

    D3DXMATRIX*                 GetViewMatrix(INT _iShadowID);
    D3DXMATRIX*                 GetProjMatrix(INT _iShadowID);
    D3DXMATRIX*                 GetInvCameraProjMatrix(INT _iShadowID);
    D3DXMATRIX*                 GetOffsetScaleMatrix(void);

    DWORD                       GetShadowBufferHandle(INT _iShadowID, INT _iMip);

#if 0
    DWORD                       GetJitterTextureHandle(void) { return m_dwJitterTextureHandle; }
#endif

    XeMaterial*                 GetMaterial(XeMaterial* _pCopyFromMat = NULL);

    XeShadowLight*              GetShadowLight(INT _iShadowID);
    void                        SetCurrentShadowID(INT _iCurrentShadowIndex, INT _iCurrentShadowMip);
    INT                         GetCurrentShadowID(void) { return m_iCurrentShadowIndex; }
    INT                         GetCurrentShadowMip(void) { return m_iCurrentShadowMip; }

    DWORD                       GetSoftTextureHandle(UINT _uiIdx);

    void                        DrawShadowQuad(INT _iBufferIndex);
    BOOL                        BlurHorizontal(void) { return m_bHorizontalBlur; }

    ULONG                       GetShadowLightTexture(void* _pLight);

#if 0
    inline ULONG                GetBackBufferZHandle(void) { return m_dwBackBufferZHandle; }
    IDirect3DTexture9*          GetBackBufferZTexture(void) { return m_BackBufferZ; }
#endif

    BOOL                        IsLightUsedForShadows(void* _pLight, int& _iChannel);
    DWORD                       GetStaticResultTextureHandle(void) { return m_StaticShadowResultTextureHandle; }
    DWORD                       GetDynamicResultTextureHandle(void) { return m_DynamicShadowResultTextureHandle; }
    DWORD                       GetResultTextureHandle(void) { return m_dwSoftTextureHandle[2][0]; }

#if 0
    int                         GetJitterTextureSize(void) { return m_iJitterTextureSize; }
#endif

    BOOL                        IsVisibleFromLight(XeRenderObject* _pObj);
    void                        SetGaussianStrength(FLOAT _fStrength) { m_fGaussianStrength = _fStrength; }
    FLOAT                       GetGaussianStrength(void) { return m_fGaussianStrength; }
    void                        IsForceReceiver(VECTOR4FLOAT *_pFlags);
    VECTOR4FLOAT*               GetBlurOffsetsAndWeights(void) { return m_bHorizontalBlur ? m_vHorizontalOffsetsAndWeights : m_vVerticalOffsetsAndWeights; }
    UINT                        SetShadowBufferSize(UINT _uiSize);

private:

#if 0
    void                        InitJitterTexture(void);
#endif

#if !defined(_XENON)
    IDirect3DSurface9*          CreateShadowZSurface(void);
#endif

    IDirect3DTexture9*          CreateShadowTexture(Xe2DTexture* _pTempTexture);

    void                        CreateAllShadowTextures(bool _bForReset = false);
    void                        DestroyAllShadowTextures(void);

private:
    XeMaterial*                 m_pMaterial;
    INT                         m_iNumSoftMips;

    // Shadow buffers
#ifdef _XENON
    Xe2DTexture                 m_SB_D3DTextures[XE_MAXNUMSHADOWS][2];
#endif
    IDirect3DTexture9*          m_ShadowBufferTextures[XE_MAXNUMSHADOWS][2];
    DWORD                       m_adwShadowBufferHandles[XE_MAXNUMSHADOWS][2];

    // Depth buffer used with the above rendertargets
#if !defined(_XENON)
    IDirect3DSurface9*          m_ZSurface;
#endif

    INT                         m_iNumShadows;
    INT                         m_iCurrentShadowIndex;
    INT                         m_iCurrentShadowMip;

    IDirect3DTexture9*          m_StaticShadowResultTexture;
    IDirect3DSurface9*          m_StaticShadowResultSurface;
    DWORD                       m_StaticShadowResultTextureHandle;

    IDirect3DTexture9*          m_DynamicShadowResultTexture;
    IDirect3DSurface9*          m_DynamicShadowResultSurface;
    DWORD                       m_DynamicShadowResultTextureHandle;

    IDirect3DTexture9*          m_SoftRTTexture[XE_NUMSOFTMIPS][2];
    IDirect3DSurface9*          m_SoftRTSurface[XE_NUMSOFTMIPS][2];
    DWORD                       m_dwSoftTextureHandle[XE_NUMSOFTMIPS][2];

#if 0
    IDirect3DTexture9*          m_BackBufferZ;
    DWORD                       m_dwBackBufferZHandle;

    INT                         m_iJitterTextureSize;
    IDirect3DVolumeTexture9*    m_pJitterTexture;
    DWORD                       m_dwJitterTextureHandle;
#endif

    XeShadowLight               m_vecShadowLights[XE_MAX_SHADOW_LIGHTS];    // Engine adds lights here
    ULONG                       m_ulNbShadowLights;
    XeShadowLight               m_vecUsedLights[XE_MAX_SHADOW_LIGHTS];      // Renderer uses this list
    ULONG                       m_ulNbUsedLights;

    D3DXMATRIX                  m_OffsetScale;
    BOOL                        m_bHorizontalBlur;
    FLOAT                       m_fGaussianStrength;
    VECTOR4FLOAT                m_vHorizontalOffsetsAndWeights[15];
    VECTOR4FLOAT                m_vVerticalOffsetsAndWeights[15];

    std::vector<SHORT>          m_vecCookieTextureHandles;

    XeRenderObject*             m_poClearObject;
    XeMaterial*                 m_poClearMaterial;
    UINT                        m_uiShadowBufferSize;
};

extern XeShadowManager g_oXeShadowManager;

#endif // !defined(__XESHADOWMANAGER_H__INCLUDED)
