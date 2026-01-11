//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright 2005 Ubisoft
//
// Author       Eric Le
// Date         3 Jan 2005
//
// $Archive: $
//
// Description
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __XETEXTUREMANAGER_H__INCLUDED
#define __XETEXTUREMANAGER_H__INCLUDED

#include <vector>

#if defined(_XENON_RENDER_PC)
#include <map>
#endif

#include "XeUtils.h"

#define MAX_SCRATCH_BUFFERS         3

#define XEUSERTEXTUREMASK           0x80000000
#define XEBACKBUFFER_ID             0xC0000000
#define XESCRATCHBUFFER0_ID         (0|XEUSERTEXTUREMASK)
#define XESCRATCHBUFFER1_ID         (1|XEUSERTEXTUREMASK)

// Used by Motion Blur, can be used by other AE only if Motion Blur is disabled
#define XESCRATCHBUFFER2_ID         (2|XEUSERTEXTUREMASK)

#define XEHEATSHIMMERSCRATCH_ID     (3|XEUSERTEXTUREMASK)
#define XESPG2ALPHASCRATCH_ID       (4|XEUSERTEXTUREMASK)
#define XEGLOWMASKSCRATCH_ID        (5|XEUSERTEXTUREMASK)
#define XEHEATSHIMMERNOISE_ID       (6|XEUSERTEXTUREMASK)
#define XEOPAQUEDEPTHBUFFERR32F_ID  (7|XEUSERTEXTUREMASK)
#define XEREFLECTIONSCRATCH0_ID     (8|XEUSERTEXTUREMASK)
#define XEREFLECTIONSCRATCH1_ID     (9|XEUSERTEXTUREMASK)

#ifdef _XENON
#define XEOPAQUEDEPTHBUFFERD24S8_ID (10|XEUSERTEXTUREMASK)
#define MAX_USER_TEXTURES           11
#else
#define MAX_USER_TEXTURES           10
#endif

struct Xe2DTexture
{
    IDirect3DTexture9* pTexture;
#if defined(_XENON)
    D3DTexture         stD3DHeader;
    LPVOID             pD3DBuffer;
#endif
};

struct Xe2DTextureArray
{
public:

    Xe2DTexture* pAllTextures;
    ULONG        ulNbTextures;

public:

    inline Xe2DTextureArray(void)
    : pAllTextures(NULL), ulNbTextures(0)
    {
    }

    inline ~Xe2DTextureArray(void)
    {
        Shutdown();
    }

    inline ULONG GetTextureCount(void) const
    {
        return ulNbTextures;
    }

    inline Xe2DTexture* GetTexture(ULONG _ulId) const
    {
        return &pAllTextures[_ulId];
    }

    inline void Initialize(ULONG _ulNbTextures)
    {
        if (ulNbTextures >= _ulNbTextures)
            return;

        if (pAllTextures != NULL)
        {
            pAllTextures = (Xe2DTexture*)MEM_p_Realloc(pAllTextures, _ulNbTextures * sizeof(Xe2DTexture));

            L_memset(pAllTextures + ulNbTextures, 0, (_ulNbTextures - ulNbTextures) * sizeof(Xe2DTexture));
        }
        else
        {
            pAllTextures = (Xe2DTexture*)MEM_p_Alloc(_ulNbTextures * sizeof(Xe2DTexture));

            L_memset(pAllTextures, 0, _ulNbTextures * sizeof(Xe2DTexture));
        }

        ulNbTextures = _ulNbTextures;
    }

    inline void Shutdown(void)
    {
        if (pAllTextures != NULL)
        {
            MEM_Free(pAllTextures);
            pAllTextures = NULL;
        }

        ulNbTextures = 0;
    }
};

class XeTextureManager
{
#if defined(_XENON_RENDER_PC)
    struct RawTexture
    {
        M_DeclareOperatorNewAndDelete();

        ULONG  ulWidth;
        ULONG  ulHeight;
        UCHAR* pucRawData;
    };
    typedef std::map<ULONG, RawTexture*> RawTextureMap;
#endif

public:
    enum
    {
        HeatShimmerTextureSize = 64
    };

public:
    XeTextureManager();
    ~XeTextureManager();

    void Init(bool _bForReset = false);
    BOOL OnDeviceLost(void);
    BOOL OnDeviceReset(void);
    void Shutdown(void);

    // Load a texture from a memory buffer, file, etc.
    void LoadTexture(void);
    void DeleteTexture(void);

    void Init2DTextures(ULONG _ul_TextureCount);
    void Resize2DTextures(ULONG _ul_NewTextureCount);
    void Unload2DTexturesAndCubeMaps();

    void Create2DTexture(ULONG _ulTexture, UINT _uiWidth, UINT _uiHeight, UINT _uiNbLevels, 
                         D3DFORMAT _eSrcFormat, void* _pData, DWORD _dwSize, D3DFORMAT _eDstFormat, bool _bAllowMipMaps, bool _bIsNormalMap);
    void ShareTexture(ULONG _ulTexToShare, ULONG _ulDstTex);
    UINT RegisterUserTexture(IDirect3DBaseTexture9* _pTexture);
    void UpdateUserTexture(UINT _uiHandle, IDirect3DBaseTexture9* _pTexture);
    void SetAnimatedTexture(ULONG _ulAnimatedTexture, ULONG _ulCurrentFrame);

#if defined(_XENON_RENDER_PC)
    void SetPalette(ULONG _ulTexture, ULONG _ulTexSrc, void* _pPalData);
    void AddRawTexture(ULONG _ulTexture, void* _pTexData, ULONG _ulWidth, ULONG _ulHeight);
#endif

    void LoadCubeMap(ULONG _ulIndex, void* _pData, ULONG _ulSize);
    void UnloadCubeMap(ULONG _ulIndex);

    // Forces a reload of the textures
    void ReloadTextures(void);

    // Deallocates all currently loaded textures
    void UnloadAll(bool _bPreserveUserTextures = false);

    void ResolveToScratchBuffer( DWORD _dwID, D3DRECT * pRect=NULL, BOOL bClearTarget = FALSE, DWORD dwClearCol = 0, BOOL bClearDepth = FALSE, FLOAT fDepthClearValue = 1.0f );

    //
    int GetTotalUsedMemory(void)          { return m_ul2DTextureRAMUsage + m_ulCubeTextureRAMUsage + m_ulScratchBuffersRAMUsage; }
    int Get2DTextureMemoryUsage(void)     { return m_ul2DTextureRAMUsage; }
    int GetCubeTextureMemoryUsage(void)   { return m_ulCubeTextureRAMUsage; }
    int GetScratchBufferMemoryUsage(void) { return m_ulScratchBuffersRAMUsage; }

    int GetNum2DTextures() { return m_Loaded2DTextures.GetTextureCount(); }
    int GetNumTextures() { return m_Loaded2DTextures.GetTextureCount() + 
                                  m_LoadedCubeTextures.size() +
                                  m_LoadedVolumeTextures.size(); }

    D3DFORMAT GetTextureFormat(DWORD _dwID);

    int GetDepthBufferID(void);

    IDirect3DBaseTexture9* GetTextureFromID(DWORD _dwID);
    IDirect3DBaseTexture9* GetScratchBuffer(DWORD _dwID);

    // Display all loaded textures on the screen
    void DisplayLoadedTextures(int _iPage);

private:
    void GenerateHeatShimmerNoiseTexture( bool _bForReset );
  
    void CreateScratchBuffer( UINT         _uiScratchBufferIndex, 
                              UINT         _uiWidth, 
                              UINT         _uiHeight,
                              D3DFORMAT    _fmt,
                              bool         _bForReset );

private:
    LPDIRECT3DDEVICE9 m_pD3DDevice;

    UINT m_uiDisplayTextureSize;
    IDirect3DTexture9* DefaultTexture; // when texture isn't loaded, ready, etc.
    UINT m_uiTotalMemoryUsage;

    Xe2DTextureArray   m_Loaded2DTextures;
    XeTextureContainer m_LoadedCubeTextures;
    XeTextureContainer m_LoadedVolumeTextures;

    XeTextureContainer m_UserTextures;    // Manually-created textures (not by Jade GD interface)

    XeTextureContainer m_CubeMapsGarbageCollector;

#if defined(_XENON_RENDER_PC)
    RawTextureMap m_RawTextures;
#endif

    ULONG m_ul2DTextureRAMUsage;
    ULONG m_ulCubeTextureRAMUsage;
    ULONG m_ulScratchBuffersRAMUsage;

#if defined(_XENON)
    D3DTexture m_stDepthBufferR32F;
#endif
};

extern XeTextureManager g_oXeTextureMgr;

ULONG ComputeAnyTextureSize(IDirect3DBaseTexture9* _pTexture);

#endif // !defined(__XETEXTUREMANAGER_H__INCLUDED)
