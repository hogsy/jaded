// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "precomp.h"

#include "XeTextureManager.h"
#include "XeRenderer.h"
#include "XeRenderTargetManager.h"

#include "../Tools/XenonTexture/Sources/XenonTexture.h"

#if defined(_XENON)
#include <xgraphics.h>
#endif

// ------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// ------------------------------------------------------------------------------------------------
// -> Size of one scanline for texture flipping (large enough for one line of 2048x2048 DXT5)
#define FLIP_SCRATCH_BUFFER_SIZE (32 * 1024)

UCHAR g_aucFlipScratchBuffer[FLIP_SCRATCH_BUFFER_SIZE];

typedef void (*FLIPLINEPROC)(void*, DWORD);

#pragma pack(push,1)

struct LineDXT1
{
    USHORT usColor0;
    USHORT usColor1;
    UCHAR  ucLine0;
    UCHAR  ucLine1;
    UCHAR  ucLine2;
    UCHAR  ucLine3;
};

struct LineDXT23
{
    USHORT   usAlpha0;
    USHORT   usAlpha1;
    USHORT   usAlpha2;
    USHORT   usAlpha3;
    LineDXT1 stDXT1Block;
};

struct LineDXT45
{
    UCHAR    ucAlpha0;
    UCHAR    ucAlpha1;
    UCHAR    ucAlphaLine0;
    UCHAR    ucAlphaLine1;
    UCHAR    ucAlphaLine2;
    UCHAR    ucAlphaLine3;
    UCHAR    ucAlphaLine4;
    UCHAR    ucAlphaLine5;
    LineDXT1 stDXT1Block;
};

#pragma pack(pop)

static inline ULONG NormalizeColor(ULONG _ul_Color);
static void         NormalizeSurfaceInPlace(void* _pBuffer, DWORD _dwWidth, DWORD _dwHeight, DWORD _dwPitch);

static void FlipSurfaceInPlace(void* _pBuffer, DWORD _dwWidth, DWORD _dwHeight, DWORD _dwPitch, D3DFORMAT _eFormat);
static void FlipSurfaceInPlace_Generic(void* _pBuffer, DWORD _dwWidth, DWORD _dwHeight, DWORD _dwPitch, DWORD _dwBPP);
static void FlipSurfaceInPlace_DXTn(void* _pBuffer, DWORD _dwWidth, DWORD _dwHeight, DWORD _dwPitch, DWORD _dwBlockWidth, DWORD _dwBlockHeight, DWORD _dwBytesPerBlock, FLIPLINEPROC _pfnLineFlip);

static void FlipLineDXT1(void* _pBuffer, DWORD _dwNbBlocks);
static void FlipLineDXT23(void* _pBuffer, DWORD _dwNbBlocks);
static void FlipLineDXT45(void* _pBuffer, DWORD _dwNbBlocks);

static ULONG ComputeTextureSize(ULONG _ulWidth, ULONG _ulHeight, ULONG _ulNbLevels, D3DFORMAT _eFormat);

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

XeTextureManager g_oXeTextureMgr;

XeTextureManager::XeTextureManager()
{
    m_uiDisplayTextureSize = 64;
#if defined(_XENON_RENDER_PC)
    m_uiDisplayTextureSize = 128;
#endif
}

XeTextureManager::~XeTextureManager()
{
}

void XeTextureManager::Init(bool _bForReset)
{
    m_pD3DDevice = g_oXeRenderer.GetDevice();
	ULONG ulCurBuffer;

    if (!_bForReset)
    {
        m_ul2DTextureRAMUsage   = 0;
        m_ulCubeTextureRAMUsage = 0;
    }
    m_ulScratchBuffersRAMUsage = 0;

    // create scratch buffers
    UINT uiWidth, uiHeight;
#if defined(ACTIVE_EDITORS)
    g_oXeRenderer.GetViewportResolution(&uiWidth, &uiHeight);
#else
    g_oXeRenderer.GetBackbufferResolution(&uiWidth, &uiHeight);
#endif

    for (ulCurBuffer = 0; ulCurBuffer < MAX_SCRATCH_BUFFERS; ulCurBuffer++)
    {
        CreateScratchBuffer( ulCurBuffer, uiWidth, uiHeight, D3DFMT_A8R8G8B8, _bForReset );
    }

    D3DFORMAT fmtAlphaMask = D3DFMT_A8;

#ifdef _XENON_RENDER_PC
    fmtAlphaMask = D3DFMT_A8R8G8B8;
#endif

    // Create heat shimmer alpha mask scratch
    CreateScratchBuffer( ulCurBuffer++, uiWidth, uiHeight, fmtAlphaMask, _bForReset );

    // Create spg2 alpha
    CreateScratchBuffer( ulCurBuffer++, uiWidth, uiHeight, fmtAlphaMask, _bForReset );

    // Create glow alpha mask scratch
    CreateScratchBuffer( ulCurBuffer++, uiWidth, uiHeight, D3DFMT_A8R8G8B8, _bForReset );

    GenerateHeatShimmerNoiseTexture( _bForReset );
    ulCurBuffer++;

    // Create the opaque depth buffer texture
#if !defined(_XENON)
    CreateScratchBuffer( ulCurBuffer++, uiWidth, uiHeight, D3DFMT_R32F, _bForReset );
#else
    // Reserve the user texture slot
    if (!_bForReset)
    {
        RegisterUserTexture(NULL);
    }
#endif

    // Create reflection buffers
    UINT uiReflectionWidth, uiReflectionHeight;
    if (g_oXeRenderer.IsHiDef())
    {
        // render reflection in quarter size
        uiReflectionWidth  = uiWidth  / 4; 
        uiReflectionHeight = uiHeight / 4;
    }
    else
    {
        // render reflection in half size
        uiReflectionWidth  = uiWidth  / 2;
        uiReflectionHeight = uiHeight / 2;
    }
    CreateScratchBuffer( ulCurBuffer++, uiReflectionWidth, uiReflectionHeight, D3DFMT_A8R8G8B8, _bForReset );
    CreateScratchBuffer( ulCurBuffer++, uiReflectionWidth, uiReflectionHeight, D3DFMT_A8R8G8B8, _bForReset );

#ifdef _XENON
    CreateScratchBuffer( ulCurBuffer++, uiWidth, uiHeight, D3DFMT_D24S8, _bForReset );
#endif

#if defined(_XENON)
    // Let's share the XEOPAQUEDEPTHBUFFERR32F_ID with the XEOPAQUEDEPTHBUFFERD24S8_ID texture since they
    // are mutually exclusive
    {
        XGSetTextureHeader(uiWidth, uiHeight, 1, 0, D3DFMT_R32F, 0, 0, 0, 0, &m_stDepthBufferR32F, NULL, NULL);
        m_stDepthBufferR32F.Format.BaseAddress = g_oXeTextureMgr.GetTextureFromID(XEOPAQUEDEPTHBUFFERD24S8_ID)->Format.BaseAddress;

        UpdateUserTexture(XEOPAQUEDEPTHBUFFERR32F_ID, &m_stDepthBufferR32F);
    }
#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::CreateScratchBuffer
// Params : None
// RetVal : Success
// Descr. :
// ------------------------------------------------------------------------------------------------
void 
XeTextureManager::CreateScratchBuffer( UINT         _uiScratchBufferIndex, 
                                       UINT         _uiWidth, 
                                       UINT         _uiHeight,
                                       D3DFORMAT    _fmt,
                                       bool         _bForReset )
{
    HRESULT hr = S_OK;

    IDirect3DTexture9 * pTexture = NULL;

    hr = D3DXCreateTexture( m_pD3DDevice,
                            _uiWidth,
                            _uiHeight,
                            1,
                            D3DUSAGE_RENDERTARGET,
                            _fmt,
                            D3DPOOL_DEFAULT,
                            &pTexture );

    ERR_X_Assert((hr == S_OK) && (pTexture != NULL));

    if (_bForReset)
    {
        UpdateUserTexture( _uiScratchBufferIndex, pTexture );
    }
    else
    {
        RegisterUserTexture( pTexture );
    }
}


// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::OnDeviceLost
// Params : None
// RetVal : Success
// Descr. : Before IDirect3DDevice::Reset()
// ------------------------------------------------------------------------------------------------
BOOL XeTextureManager::OnDeviceLost(void)
{
    // TODO: Destroy all the D3DPOOL_DYNAMIC stuff
 
    // unload scratch buffers
    for (ULONG ulCurBuffer = 0; ulCurBuffer < MAX_USER_TEXTURES; ulCurBuffer++)
    {
        if (m_UserTextures.size() > ulCurBuffer)
        {
            SAFE_RELEASE(m_UserTextures[ulCurBuffer]);
        }
    }

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::OnDeviceReset
// Params : None
// RetVal : Success
// Descr. : After IDirect3DDevice::Reset()
// ------------------------------------------------------------------------------------------------
BOOL XeTextureManager::OnDeviceReset(void)
{
    // TODO: Restore all the D3DPOOL_DYNAMIC stuff
    Init(true);

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::Shutdown
// Params : None
// RetVal : None
// Descr. : Shutdown the texure manager
// ------------------------------------------------------------------------------------------------
void XeTextureManager::Shutdown(void)
{
#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.AcquireThreadOwnership();
#endif
    UnloadAll();
#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.ReleaseThreadOwnership();
#endif
}

//----------------------------------------------------------------------@FHB--
// Load2DTextureRaw
// 
// Role   : Create D3D texture  from a buffer in memory.  Only supports raw
//          32-bit textures for now
//          
// Params : _uiWidth : Width of texture
//          _uiHeight : Height of texture
//          _eSrcFormat : Format with which texture is stored in memory
//          _pData : Pointer to texture data
//          _eDstFormat : D3D format to use for creation
//          
// Return : LPDIRECT3DTEXTURE9
// 
// Author : Eric Le
// Date   : 6 Jan 2005
//----------------------------------------------------------------------@FHE--
LPDIRECT3DTEXTURE9 Load2DTextureRaw(UINT      _uiWidth,
                                    UINT      _uiHeight,
                                    UINT      _uiLevels,
                                    D3DFORMAT _eSrcFormat,
                                    void*     _pData,
                                    D3DFORMAT _eDstFormat)
{
    LPDIRECT3DTEXTURE9 pD3DTexture = NULL;
    D3DPOOL            ePool       = D3DPOOL_DEFAULT;
    HRESULT hr;

#if defined(_XENON_RENDER_PC)

    ERR_X_Assert((_eSrcFormat == D3DFMT_A8R8G8B8) && (_eDstFormat == D3DFMT_A8R8G8B8));

    ePool = D3DPOOL_MANAGED;

    if (g_oXeRenderer.GetDevice() == NULL)
    {
        ERR_OutputDebugString("[Xenon] Trying to load a texture with an uninitialized device\n");
        return NULL;
    }

    hr = D3DXCreateTexture(g_oXeRenderer.GetDevice(),
                           _uiWidth,
                           _uiHeight,
                           1,
                           0,
                           D3DFMT_A8R8G8B8,
                           ePool,
                           &pD3DTexture);
    XeValidateErr(SUCCEEDED(hr) && (pD3DTexture != NULL), NULL, "Failed to create a texture");

    ULONG* pSrc  = (ULONG*)_pData;
    if(pSrc != NULL)
    {
        // Lock the texture
        D3DLOCKED_RECT rc;
        hr = pD3DTexture->LockRect(0, &rc, NULL, D3DLOCK_NOSYSLOCK);
        if (FAILED(hr))
        {
            SAFE_RELEASE(pD3DTexture);
            XeValidateRet(FALSE, NULL, "Unable to lock the texture");
        }

        // Fill the texture
        ULONG* pDest = (ULONG*)rc.pBits;
        for (UINT j = 0; j < _uiHeight; ++j)
        {
            for (UINT i = 0; i < _uiWidth; ++i)
            {
                *pDest = XeConvertColor((*pSrc));
                ++pSrc;
                ++pDest;
            }

            // Skip the texture padding and go to next scanline
            pDest += (rc.Pitch >> 2) - _uiWidth;
        }

        // Unlock the texture
        pD3DTexture->UnlockRect(0);
    }
#endif

#if defined(_XENON)

    hr = D3DXCreateTexture(g_oXeRenderer.GetDevice(),
                           _uiWidth,
                           _uiHeight,
                           _uiLevels,
                           0,
                           _eDstFormat,
                           ePool,
                           &pD3DTexture);
    XeValidateErr(SUCCEEDED(hr) && (pD3DTexture != NULL), NULL, "Failed to create a texture");

    DWORD dwSize;
    dwSize = _uiWidth * _uiHeight * 4;

    // Load data into texture surface
    // Never mind the mipmaps for now
    IDirect3DSurface9* pD3DSurface;

    hr = pD3DTexture->GetSurfaceLevel(0, &pD3DSurface);
    XeValidate(SUCCEEDED(hr), "Unable to access the texture's surface");

    RECT SrcRect = { 0, 0, _uiWidth, _uiHeight };

    hr = D3DXLoadSurfaceFromMemory(pD3DSurface,          // Dest surface
                                   NULL,                 // Dest palette
                                   NULL,                 // Dest rect
                                   _pData,               // Src memory
                                   _eSrcFormat,          // Src format
                                   _uiWidth*4,           // Src pitch
                                   NULL,                 // Src palette
                                   &SrcRect,             // Src rect
                                   FALSE,                // Xenon Src packed
                                   0,                    // Xenon Parent width
                                   0,                    // Xenon parent height
                                   D3DX_FILTER_NONE,     // Filter
                                   0);                   // Color key
    SAFE_RELEASE(pD3DSurface);
    XeValidate(SUCCEEDED(hr), "Unable to load the texture surface");

    // Generate the mip maps using a box filter
    hr = D3DXFilterTexture(pD3DTexture, NULL, 0, D3DX_FILTER_BOX);
    XeValidate(SUCCEEDED(hr), "Unable to generate the mip maps");

#endif

    return pD3DTexture;
}

IDirect3DTexture9* LoadXenonTexture(void* _pData, DWORD _dwSize, Xe2DTexture* _pTexture)
{
    ULONG* pul_Data       = (ULONG*)_pData;
    ULONG  ul_Magic       = *pul_Data++;
    ULONG  ul_Width       = *pul_Data++;
    ULONG  ul_Height      = *pul_Data++;
    ULONG  ul_Format      = *pul_Data++;
    ULONG  ul_NbLevels    = *pul_Data++;
    ULONG  ul_TailLevel   = *pul_Data++;
    ULONG  ul_TextureSize = *pul_Data++;
    ULONG  ul_Unused      = *pul_Data++;

#if defined(_XENON)

    _pTexture->pTexture = &_pTexture->stD3DHeader;

    UINT uiTexSize = XGSetTextureHeader(ul_Width, ul_Height, ul_NbLevels, 0, (D3DFORMAT)ul_Format,
                                        0, 0, XGHEADER_CONTIGUOUS_MIP_OFFSET, 0, &_pTexture->stD3DHeader, 
                                        NULL, NULL);
    ERR_X_Assert(uiTexSize == ul_TextureSize);

    _pTexture->pD3DBuffer = XPhysicalAlloc(ul_TextureSize, MAXULONG_PTR, 4096, PAGE_READWRITE | PAGE_WRITECOMBINE);
    //_pTexture->pD3DBuffer = DmAllocatePool(ul_TextureSize, MAXULONG_PTR, 4096, PAGE_READWRITE | PAGE_WRITECOMBINE);
 
	/*DMHRAPI DmSetMemory(LPVOID lpbAddr, DWORD cb, LPCVOID lpbBuf,
    LPDWORD pcbRet);*/

	ERR_X_Assert(_pTexture->pD3DBuffer != NULL);

    // Copy the content of the texture
    L_memcpy(_pTexture->pD3DBuffer, pul_Data, ul_TextureSize);

    XGOffsetResourceAddress(&_pTexture->stD3DHeader, _pTexture->pD3DBuffer);

    return _pTexture->pTexture;

#else

    IDirect3DTexture9* pTexture = NULL;

    TEX_tdst_File_Desc st_Desc;

    SwapDWord(&ul_Width);
    SwapDWord(&ul_Height);
    SwapDWord(&ul_NbLevels);

    st_Desc.uw_Width  = (USHORT)ul_Width;
    st_Desc.uw_Height = (USHORT)ul_Height;
    st_Desc.p_Bitmap  = MEM_p_Alloc(ul_Width * ul_Height * sizeof(ULONG));

    if (!TEX_XeConvertXenonDDSFileToARGB32(_pData, _dwSize, &st_Desc))
    {
        MEM_Free(st_Desc.p_Bitmap);
        return NULL;
    }

    HRESULT hr = D3DXCreateTexture(g_oXeRenderer.GetDevice(), ul_Width, ul_Height, ul_NbLevels, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture);
    XeValidate(SUCCEEDED(hr), "Failed to create a texture");

    D3DLOCKED_RECT st_Lock;

    hr = pTexture->LockRect(0, &st_Lock, NULL, 0);
    XeValidate(SUCCEEDED(hr), "Failed to lock a texture");

    ULONG* pul_Dest   = (ULONG*)st_Lock.pBits;
    ULONG* pul_Bitmap = (ULONG*)st_Desc.p_Bitmap;

    st_Lock.Pitch = (st_Lock.Pitch >> 2) - ul_Width;

    for (ULONG j = 0; j < ul_Height; ++j)
    {
        for (ULONG i = 0; i < ul_Width; ++i)
        {
            *pul_Dest = *pul_Bitmap;

            ++pul_Bitmap;
            ++pul_Dest;
        }

        pul_Dest += st_Lock.Pitch;
    }

    pTexture->UnlockRect(0);

    MEM_Free(st_Desc.p_Bitmap);

    hr = D3DXFilterTexture(pTexture, NULL, 0, D3DX_FILTER_BOX);
    XeValidate(SUCCEEDED(hr), "Failed to filter a texture");

    return pTexture;

#endif
}

IDirect3DTexture9* LoadDDS(void* _pData, DWORD _dwSize, bool _bFlip)
{
    IDirect3DTexture9* pTexture = NULL;
    D3DSURFACE_DESC stDesc;
    D3DLOCKED_RECT stLock;
    D3DXIMAGE_INFO stInfo;
    HRESULT hr;
    DWORD   dwLevel;
    DWORD   dwWidth;
    DWORD   dwHeight;

#if defined(_XENON_RENDER_PC)
    if (g_oXeRenderer.GetDevice() == NULL)
    {
        ERR_OutputDebugString("[Xenon] Trying to load a texture with an uninitialized device\n");
        return NULL;
    }
#endif

    if (_bFlip)
    {
        hr = D3DXGetImageInfoFromFileInMemory(_pData, _dwSize, &stInfo);
        XeValidateRet(SUCCEEDED(hr), NULL, "Failed to load image information");

        // Load the texture as-is
        hr = D3DXCreateTextureFromFileInMemoryEx(g_oXeRenderer.GetDevice(), _pData, _dwSize, 
                                                 D3DX_DEFAULT, D3DX_DEFAULT, stInfo.MipLevels, 0, D3DFMT_UNKNOWN, 
                                                 D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &stInfo, NULL, &pTexture);
        XeValidateRet(SUCCEEDED(hr) && (pTexture != NULL), NULL, "Failed to create a DDS texture");

        dwWidth  = stInfo.Width;
        dwHeight = stInfo.Height;

        stInfo.MipLevels = pTexture->GetLevelCount();

        // In-place flip all the levels
        for (dwLevel = 0; dwLevel < stInfo.MipLevels; ++dwLevel)
        {
            hr = pTexture->GetLevelDesc(dwLevel, &stDesc);
            XeValidate(SUCCEEDED(hr), "Failed to get a surface level description");

            hr = pTexture->LockRect(dwLevel, &stLock, NULL, 0);
            XeValidate(SUCCEEDED(hr) && (stLock.pBits != NULL), "Failed to lock a surface");

            if (SUCCEEDED(hr))
            {
                FlipSurfaceInPlace(stLock.pBits, stDesc.Width, stDesc.Height, stLock.Pitch, stDesc.Format);

                hr = pTexture->UnlockRect(dwLevel);
                XeValidate(SUCCEEDED(hr), "Failed to unlock a surface");
            }
        }
    }
    else
    {
        // Load the texture as-is
        hr = D3DXCreateTextureFromFileInMemory(g_oXeRenderer.GetDevice(), _pData, _dwSize, &pTexture);
        XeValidateRet(SUCCEEDED(hr) && (pTexture != NULL), NULL, "Failed to create a DDS texture");
    }

    return pTexture;
}

IDirect3DTexture9* LoadConvertTexture(void* _pData, DWORD _dwSize, DWORD _dwWidth, DWORD _dwHeight, DWORD _dwNbLevels, D3DFORMAT _eDstFormat, bool _bNormalize)
{
    IDirect3DTexture9* pScratchTexture = NULL;
    IDirect3DTexture9* pTexture = NULL;
    HRESULT hr;

#if defined(_XENON_RENDER_PC)
    if (g_oXeRenderer.GetDevice() == NULL)
    {
        ERR_OutputDebugString("[Xenon] Trying to load a texture with an uninitialized device\n");
        return NULL;
    }
#endif

    if (_dwWidth > 2048)
        _dwWidth = 2048;
    if (_dwHeight > 2048)
        _dwHeight = 2048;

    ULONG ulNbLevels = TEX_ul_XeGetMaxMipMapsCount(_dwWidth, _dwHeight);

    if (_dwNbLevels == 0)
        _dwNbLevels = ulNbLevels;

    hr = D3DXCreateTextureFromFileInMemoryEx(g_oXeRenderer.GetDevice(), _pData, _dwSize, _dwWidth, 
                                             _dwHeight, _dwNbLevels, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_SCRATCH, 
                                             D3DX_FILTER_LINEAR, D3DX_FILTER_BOX, 0, NULL, NULL, &pScratchTexture);
    XeValidateRet(SUCCEEDED(hr) && (pScratchTexture != NULL), NULL, "Failed to convert a texture");

    // Create the final texture
    hr = D3DXCreateTexture(g_oXeRenderer.GetDevice(), _dwWidth, _dwHeight, _dwNbLevels, 
                           0, _eDstFormat, D3DPOOL_MANAGED, &pTexture);
    if (FAILED(hr) || (pTexture == NULL))
    {
        SAFE_RELEASE(pScratchTexture);
        XeValidateRet(SUCCEEDED(hr) && (pTexture != NULL), NULL, "Failed to convert a texture");
    }

    // Flip & copy the texture
    static ULONG aulSwap[2048];
    ULONG ulWidth  = _dwWidth;
    ULONG ulHeight = _dwHeight;
    for (ULONG i = 0; i < _dwNbLevels; ++i)
    {
        IDirect3DSurface9* pSrcSurface = NULL;
        IDirect3DSurface9* pDstSurface = NULL;
        D3DLOCKED_RECT stLockedRect;

        hr = pScratchTexture->LockRect(i, &stLockedRect, NULL, 0);
        XeValidate(SUCCEEDED(hr), "Failed to lock the surface");

        // Flip the surface
        if (SUCCEEDED(hr))
        {
            FlipSurfaceInPlace(stLockedRect.pBits, ulWidth, ulHeight, stLockedRect.Pitch, D3DFMT_LIN_A8R8G8B8);

            if (_bNormalize)
            {
                NormalizeSurfaceInPlace(stLockedRect.pBits, ulWidth, ulHeight, stLockedRect.Pitch);
            }

            hr = pScratchTexture->UnlockRect(i);
            XeValidate(SUCCEEDED(hr), "Failed to unlock the surface");
        }

        hr = pScratchTexture->GetSurfaceLevel(i, &pSrcSurface);
        XeValidate(SUCCEEDED(hr), "Failed to access the source surface");

        hr = pTexture->GetSurfaceLevel(i, &pDstSurface);
        XeValidate(SUCCEEDED(hr), "Failed to access the destination surface");

        hr = D3DXLoadSurfaceFromSurface(pDstSurface, NULL, NULL, pSrcSurface, NULL, NULL, D3DX_DEFAULT, 0);
        XeValidate(SUCCEEDED(hr), "Failed to copy the surface");

        SAFE_RELEASE(pSrcSurface);
        SAFE_RELEASE(pDstSurface);

        ulWidth  >>= 1;
        ulHeight >>= 1;

        if (ulWidth == 0)
            ulWidth = 1;
        if (ulHeight == 0)
            ulHeight = 1;
    }

    SAFE_RELEASE(pScratchTexture);

    return pTexture;
}

// Note: We don't have a palette and don't want to support them so we'll just build a
//       fake color using the index and setting alpha to 255
IDirect3DTexture9* LoadTexturePALx(UINT _uiWidth, UINT _uiHeight, UCHAR* _pRawData, ULONG* _pPal)
{
#if defined(_XENON_RENDER_PC)

    IDirect3DTexture9* pTexture = NULL;
    HRESULT hr;

    if (g_oXeRenderer.GetDevice() == NULL)
    {
        ERR_OutputDebugString("[Xenon] Trying to load a texture with an uninitialized device\n");
        return NULL;
    }

    // Create an empty texture
    hr = D3DXCreateTexture(g_oXeRenderer.GetDevice(), _uiWidth, _uiHeight, 
                           1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture);
    XeValidateRet(SUCCEEDED(hr) && (pTexture != NULL), NULL, "Failed to create a texture");

    // Lock the texture
    D3DLOCKED_RECT rc;
    hr = pTexture->LockRect(0, &rc, NULL, D3DLOCK_NOSYSLOCK);
    if (FAILED(hr))
    {
        SAFE_RELEASE(pTexture);
        XeValidateRet(FALSE, NULL, "Unable to lock the texture");
    }

    // Fill the texture
    UCHAR* pSrc    = _pRawData;
    ULONG* pDest   = (ULONG*)rc.pBits;
    ULONG  ulColor;
    for (UINT j = 0; j < _uiHeight; ++j)
    { 
        for (UINT i = 0; i < _uiWidth; ++i)
        {
            ulColor = _pPal[*pSrc];
            *pDest  = XeConvertColor(ulColor);
            ++pSrc;
            ++pDest;
        }

        // Skip the texture padding and go to next scanline
        pDest += (rc.Pitch >> 2) - _uiWidth;
    }

    // Unlock the texture
    pTexture->UnlockRect(0);

    return pTexture;

#else

    ERR_OutputDebugString("[Xe3D] LoadTexturePALx() not implemented for current target\n");
    return NULL;

#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::Init2DTextures
// Params : _ul_TextureCount : Maximum number of 2D textures
// RetVal : None
// Descr. : Initialize the 2D texture list
// ------------------------------------------------------------------------------------------------
void XeTextureManager::Init2DTextures(ULONG _ul_TextureCount)
{
    ERR_X_Assert(m_Loaded2DTextures.GetTextureCount() == 0);

    if (_ul_TextureCount == 0)
        return;

    // Create an empty array of textures
    m_Loaded2DTextures.Initialize(_ul_TextureCount);

    m_ul2DTextureRAMUsage = 0;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::Resize2DTextures
// Params : _ul_NewTextureCount : New maximum number of 2D textures
// RetVal : None
// Descr. : Resize the 2D texture list
// ------------------------------------------------------------------------------------------------
void XeTextureManager::Resize2DTextures(ULONG _ul_NewTextureCount)
{
    m_Loaded2DTextures.Initialize(_ul_NewTextureCount);
}

//----------------------------------------------------------------------@FHB--
// XeTextureManager::Create2DTexture
// 
// Role   : Create a texture with dimensions and format specified.
//          Store it in the manager
//          
// Params : _uiWidth  : width
//          _uiHeight : height
//          _eFormat  : texture format
//          _pData    : pointer to buffer where texture data is loaded
//          
// Return : void
// 
// Author : Eric Le
// Date   : 4 Jan 2005
//----------------------------------------------------------------------@FHE--
void XeTextureManager::Create2DTexture(ULONG     _ulTexture, 
                                       UINT      _uiWidth,
                                       UINT      _uiHeight,
                                       UINT      _uiNbLevels,
                                       D3DFORMAT _eSrcFormat,
                                       void*     _pData,
                                       DWORD     _dwSize,
                                       D3DFORMAT _eDstFormat,
                                       bool      _bAllowMipMaps, 
                                       bool      _bIsNormalMap)
{
    ERR_X_Assert(_ulTexture < m_Loaded2DTextures.GetTextureCount());

    LPDIRECT3DTEXTURE9 pD3DTexture = NULL;

#if defined(_DEBUG)
    static ULONG s_ulDebugTexId = 0xffffffff;
    if ((s_ulDebugTexId != 0xffffffff) && (s_ulDebugTexId == _ulTexture))
    {
        _breakpoint_;
    }
#endif

#if defined(_XENON)
    // Make sure the file is not a .TGA mapped directly in the material instead of using a .TEX
    // Once preprocessed, the file is actually a Xenon .DDS
    if ((_eSrcFormat == D3DFMT_LIN_A8R8G8B8) && (_pData != NULL) && (*(ULONG*)_pData == XE_TEXTURE_2D))
    {
        if ((*((ULONG*)_pData + 1) == _uiWidth) &&
            (*((ULONG*)_pData + 2) == _uiHeight))
        {
            _eSrcFormat = D3DFMT_DDS;
        }
    }
#endif

    switch(_eSrcFormat)
    {
        case D3DFMT_LIN_X8R8G8B8:
        case D3DFMT_LIN_A8R8G8B8:
            pD3DTexture = Load2DTextureRaw(_uiWidth, _uiHeight, _bAllowMipMaps ? 0 : 1, _eSrcFormat, _pData, _eDstFormat);

            m_ul2DTextureRAMUsage += ComputeTextureSize(_uiWidth, _uiHeight, _bAllowMipMaps ? 0 : 1, _eDstFormat);
            break;

#if defined(_XENON_RENDER_PC)
        case D3DFMT_PAL4:
        case D3DFMT_PAL8:
            UCHAR* pRawData;
            pRawData = (UCHAR*)MEM_p_Alloc(_uiWidth * _uiHeight);
            if (_eSrcFormat == D3DFMT_PAL4)
            {
                TEX_Convert_4To8(pRawData, (UCHAR*)_pData, _uiWidth, _uiHeight);
            }
            else
            {
                L_memcpy(pRawData, _pData, _uiWidth * _uiHeight);
            }
            AddRawTexture(_ulTexture, pRawData, _uiWidth, _uiHeight);
            break;
#endif

        case D3DFMT_DDS:
            {
                ULONG ulMagic = *(ULONG*)_pData;
#if !defined(_XENON)
                SwapDWord(&ulMagic);
#endif

                if (ulMagic == XE_TEXTURE_2D)
                {
                    // Native Xenon Texture
                    pD3DTexture = LoadXenonTexture(_pData, _dwSize, m_Loaded2DTextures.GetTexture(_ulTexture));
                    m_ul2DTextureRAMUsage += _dwSize;
                }
                else
                {
                    // Load the .DDS file from memory as-is
                    pD3DTexture = LoadDDS(_pData, _dwSize, false);
                    m_ul2DTextureRAMUsage += _dwSize;
                }
            }
            break;

        default:
            // Load and convert a texture
            pD3DTexture = LoadConvertTexture(_pData, _dwSize, _uiWidth, _uiHeight, _uiNbLevels, _eDstFormat, _bIsNormalMap);
            m_ul2DTextureRAMUsage += ComputeTextureSize(_uiWidth, _uiHeight, _uiNbLevels, _eDstFormat);
            break;
    }

    m_Loaded2DTextures.GetTexture(_ulTexture)->pTexture = pD3DTexture;
}

void XeTextureManager::ShareTexture(ULONG _ulTexToShare, ULONG _ulDstTex)
{
	if (_ulTexToShare >= m_Loaded2DTextures.GetTextureCount())
		return;
	if (_ulDstTex >= m_Loaded2DTextures.GetTextureCount())
		return;

    if ((m_Loaded2DTextures.GetTexture(_ulTexToShare)->pTexture == NULL) || 
        (m_Loaded2DTextures.GetTexture(_ulDstTex)->pTexture     != NULL))
        return;

    m_Loaded2DTextures.GetTexture(_ulDstTex)->pTexture = m_Loaded2DTextures.GetTexture(_ulTexToShare)->pTexture;
    m_Loaded2DTextures.GetTexture(_ulDstTex)->pTexture->AddRef();
}

void XeTextureManager::SetAnimatedTexture(ULONG _ulAnimatedTexture, ULONG _ulCurrentFrame)
{
    if (_ulAnimatedTexture >= m_Loaded2DTextures.GetTextureCount())
        return;
    if (_ulCurrentFrame >= m_Loaded2DTextures.GetTextureCount())
        return;

    SAFE_RELEASE(m_Loaded2DTextures.GetTexture(_ulAnimatedTexture)->pTexture);

    m_Loaded2DTextures.GetTexture(_ulAnimatedTexture)->pTexture = m_Loaded2DTextures.GetTexture(_ulCurrentFrame)->pTexture;
    if (m_Loaded2DTextures.GetTexture(_ulAnimatedTexture)->pTexture)
    {
        m_Loaded2DTextures.GetTexture(_ulAnimatedTexture)->pTexture->AddRef();
    }
}

#if defined(_XENON_RENDER_PC)

void XeTextureManager::SetPalette(ULONG _ulTexture, ULONG _ulTexSrc, void* _pPalData)
{
    if (!_pPalData)
        return;

    XeValidateRet(_ulTexture < m_Loaded2DTextures.GetTextureCount(), ,           "Invalid texture index");
    XeValidateRet(m_Loaded2DTextures.GetTexture(_ulTexture)->pTexture == NULL, , "Texture already loaded");

    IDirect3DTexture9* pTexture = NULL;
    RawTextureMap::iterator itRaw;
    RawTexture* pRawTex = NULL;

    itRaw = m_RawTextures.find(_ulTexSrc);
    if (itRaw == m_RawTextures.end())
    {
        // The texture manager will sometimes give us textures that are not used anymore
        // so let's just skip them
        return;
    }
    pRawTex = itRaw->second;

    pTexture = LoadTexturePALx(pRawTex->ulWidth, pRawTex->ulHeight, 
                               pRawTex->pucRawData, (ULONG*)_pPalData);

    m_Loaded2DTextures.GetTexture(_ulTexture)->pTexture = pTexture;
}

void XeTextureManager::AddRawTexture(ULONG _ulTexture, void* _pTexData, ULONG _ulWidth, ULONG _ulHeight)
{
    XeValidateRet(m_RawTextures.find(_ulTexture) == m_RawTextures.end(), , "Texture already exists!");

    RawTexture* pRawTexture = (RawTexture*)MEM_p_Alloc(sizeof(RawTexture));
    pRawTexture->pucRawData = (UCHAR*)_pTexData;
    pRawTexture->ulWidth    = _ulWidth;
    pRawTexture->ulHeight   = _ulHeight;
    m_RawTextures[_ulTexture] = pRawTexture;
}

#endif

IDirect3DBaseTexture9 * XeTextureManager::GetTextureFromID(DWORD _dwID)
{
    if(_dwID == 0xFFFFFFFF)
    {
        // Invalid texture ID
        return NULL;
    }

    if(_dwID & XEUSERTEXTUREMASK)
    {
        UINT uiIndex = _dwID & ~XEUSERTEXTUREMASK;
        ERR_X_Assert( uiIndex < m_UserTextures.size() );
        return m_UserTextures[uiIndex];
    }

    if (_dwID & XE_CUBE_MAP_MASK)
    {
        UINT uiIndex = _dwID & ~XE_CUBE_MAP_MASK;
        if (uiIndex < m_LoadedCubeTextures.size())
            return m_LoadedCubeTextures[uiIndex];

        return NULL;
    }

    // Insert other special texture cases here

    // Regular texture request
    if(m_Loaded2DTextures.GetTextureCount() > _dwID)
    {
        return m_Loaded2DTextures.GetTexture(_dwID)->pTexture;
    }

    // Texture not found (probably not yet loaded)
    return NULL;
}


IDirect3DBaseTexture9 * XeTextureManager::GetScratchBuffer(DWORD _dwID)
{
    ULONG ulSize   = m_UserTextures.size();
    DWORD dwIndex = _dwID & ~XEUSERTEXTUREMASK;

    bool isValid = (((dwIndex < MAX_SCRATCH_BUFFERS)    || 
                     (_dwID == XEHEATSHIMMERSCRATCH_ID) ||
                     (_dwID == XEGLOWMASKSCRATCH_ID )   ||
                     (_dwID == XESPG2ALPHASCRATCH_ID )  ||
                     (_dwID == XEREFLECTIONSCRATCH0_ID ) ||
                     (_dwID == XEREFLECTIONSCRATCH1_ID )) && (dwIndex < ulSize));

    ERR_X_Assert(isValid);

    if ( isValid )
    {
        return m_UserTextures[dwIndex];
    }

    return NULL;
}

//----------------------------------------------------------------------@FHB--
// XeTextureManager::DisplayLoadedTextures
// 
// Role   : Draw a grid of all textures loaded.
//          
// Params : _iPage : Page number.  If page < 0, nothing is drawn
//          
// Return : void
// 
// Author : Eric Le
// Date   : 6 Jan 2005
//----------------------------------------------------------------------@FHE--
void XeTextureManager::DisplayLoadedTextures(int _iPage)
{
    BOOL bUserTextures = FALSE;

    if(_iPage < 0)
        return;

    UINT uiWidth, uiHeight;
#if defined(ACTIVE_EDITORS)
    g_oXeRenderer.GetViewportResolution(&uiWidth, &uiHeight);
#else
    g_oXeRenderer.GetBackbufferResolution(&uiWidth, &uiHeight);
#endif

    UINT uiNumColumns = uiWidth / m_uiDisplayTextureSize;
    UINT uiNumRows = uiHeight / m_uiDisplayTextureSize;
    UINT uiNumTexturesPerPage = uiNumColumns * uiNumRows;

    UINT uiTextureIdx = uiNumTexturesPerPage * _iPage;

    UINT uiNumTexturesLoaded     = m_Loaded2DTextures.GetTextureCount();
    UINT uiNumUserTexturesLoaded = m_UserTextures.size();

    float fY = 0;
    for(UINT uiRow = 0; uiRow < uiNumRows; uiRow++)
    {
        float fX = 0;
        for(UINT uiColumn = 0; uiColumn < uiNumColumns; uiColumn++)
        {
            DrawRectangleEx(fX, fY, fX + m_uiDisplayTextureSize, fY + m_uiDisplayTextureSize,
                            0.0f, 1.0f, 1.0f, 0.0f,
                            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                            0.0f,
                            MAT_Cc_Op_Copy,
                            uiTextureIdx);

            if (!bUserTextures)
            {
                if (uiTextureIdx >= uiNumTexturesLoaded)
                {
                    // render all user texture
                    bUserTextures = TRUE;
                    uiTextureIdx = XEUSERTEXTUREMASK;
                }
                else
                {
                    uiTextureIdx++;
                }
            }
            else
            {
                uiTextureIdx &= ~XEUSERTEXTUREMASK;
                if (uiTextureIdx >= uiNumUserTexturesLoaded - 1)
                    return;
                else
                    uiTextureIdx++;
                
                uiTextureIdx |= XEUSERTEXTUREMASK;
            }

            fX += m_uiDisplayTextureSize;
        }

        fY += m_uiDisplayTextureSize;
    }
}


//----------------------------------------------------------------------@FHB--
// XeTextureManager::UnloadAll
// 
// Role   : Release all D3D textures.
//          
//          
// Return : void
// 
// Author : Eric Le
// Date   : 6 Jan 2005
//----------------------------------------------------------------------@FHE--
void XeTextureManager::UnloadAll(bool _bPreserveUserTextures)
{
    Unload2DTexturesAndCubeMaps();

    // Unload scratch buffers
    for (ULONG ulCurBuffer = 0; ulCurBuffer < MAX_USER_TEXTURES; ulCurBuffer++)
    {
#if defined(_XENON)
        // Texture is shared
        if ((ulCurBuffer | XEUSERTEXTUREMASK) == XEOPAQUEDEPTHBUFFERR32F_ID)
            continue;
#endif

        if(ulCurBuffer < m_UserTextures.size())
            SAFE_RELEASE(m_UserTextures[ulCurBuffer]);
    }
    if (!_bPreserveUserTextures)
    {
        m_UserTextures.clear();
    }
}

//----------------------------------------------------------------------@FHB--
// XeTextureManager::Unload2DTexturesAndCubeMaps
// 
// Role   : Release all D3D textures.
//          
//          
// Return : void
// 
// Author : Eric Le
// Date   : 6 Jan 2005
//----------------------------------------------------------------------@FHE--
void XeTextureManager::Unload2DTexturesAndCubeMaps()
{
#ifdef _XENON

	if( g_oXeSimpleRenderer.IsActive( ) )
	{
		g_oXeSimpleRenderer.RequestReleaseDeviceOwnership( );

		while( g_oXeSimpleRenderer.OwnsDevice( ) )
		{
			// just wait
			Sleep( 0 );
		}
	}

#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.AcquireThreadOwnership( );
#endif // defined(_XENON_RENDERER_USETHREAD)

#endif // _XENON

    // SC: Make we don't keep a reference to D3D resource that will eventually be freed
    g_oRenderStateMgr.UnbindAll();

    for(ULONG i = 0; i < m_Loaded2DTextures.GetTextureCount(); ++i)
    {
        Xe2DTexture* pTex = m_Loaded2DTextures.GetTexture(i);

#if defined(_XENON)
        if (pTex->pD3DBuffer)
        {
            XPhysicalFree(pTex->pD3DBuffer);
        }
        else
#endif
        {
            SAFE_RELEASE(pTex->pTexture);
        }
    }
    m_Loaded2DTextures.Shutdown();

  #if defined(_XENON_RENDER_PC)
    RawTextureMap::iterator itRaw = m_RawTextures.begin();
    while (itRaw != m_RawTextures.end())
    {
        RawTexture* pRawTex = itRaw->second;
        MEM_Free(pRawTex->pucRawData);
        MEM_Free(pRawTex);
        ++itRaw;
    }
    m_RawTextures.clear();
  #endif

    // Unload the cube maps in the garbage collector
    for (ULONG i = 0; i < m_CubeMapsGarbageCollector.size(); ++i)
    {
        SAFE_RELEASE(m_CubeMapsGarbageCollector[i]);
    }
    m_CubeMapsGarbageCollector.clear();

    m_ul2DTextureRAMUsage = 0;

#ifdef _XENON

#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.ReleaseThreadOwnership( );
#endif // defined(_XENON_RENDERER_USETHREAD)

	if( g_oXeSimpleRenderer.IsActive( ) )
		g_oXeSimpleRenderer.SignalDeviceOwnershipAvailable();

#endif // _XENON
}


//----------------------------------------------------------------------@FHB--
// XeTextureManager::RegisterUserTexture
// 
// Role   : Inserts a manually-created texture into the texture manager's
//          internal list.
//          
// Params : _pTexture : Texture to be added to the manager
//          
// Return : UINT Texture ID.  This is the value to send to the texture manager
//          when drawing with this texture.
// 
// Author : Eric Le
// Date   : 4 Feb 2005
//----------------------------------------------------------------------@FHE--
UINT XeTextureManager::RegisterUserTexture(IDirect3DBaseTexture9* _pTexture)
{
    m_UserTextures.push_back(_pTexture);
    UINT uiHandle = m_UserTextures.size() - 1;

    uiHandle |= XEUSERTEXTUREMASK;

    m_ulScratchBuffersRAMUsage += ComputeAnyTextureSize(_pTexture);

    return uiHandle;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::UpdateUserTexture
// Params : _uiHandle : User texture handle
//          _pTexture : New texture pointer
// RetVal : None
// Descr. : Update the texture pointer of a user defined texture
// ------------------------------------------------------------------------------------------------
void XeTextureManager::UpdateUserTexture(UINT _uiHandle, IDirect3DBaseTexture9* _pTexture)
{
    UINT uiTextureIndex = _uiHandle & ~XEUSERTEXTUREMASK;

    if(uiTextureIndex < m_UserTextures.size())
    {
        m_UserTextures[uiTextureIndex] = _pTexture;

        m_ulScratchBuffersRAMUsage += ComputeAnyTextureSize(_pTexture);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::LoadCubeMap
// Params : ..
// RetVal : None
// Descr. : Load a cube map
// ------------------------------------------------------------------------------------------------
void XeTextureManager::LoadCubeMap(ULONG _ulIndex, void* _pData, ULONG _ulSize)
{
    IDirect3DCubeTexture9* pTexture;
    ULONG   ulOldSize = m_LoadedCubeTextures.size();
    HRESULT hr;

    if (_ulIndex >= ulOldSize)
    {
        m_LoadedCubeTextures.resize(_ulIndex + 1);
        for (ULONG i = ulOldSize; i < _ulIndex + 1; ++i)
        {
            m_LoadedCubeTextures[i] = NULL;
        }
    }

    hr = D3DXCreateCubeTextureFromFileInMemory(g_oXeRenderer.GetDevice(), _pData, _ulSize, &pTexture);
    XeValidateRet(SUCCEEDED(hr) && pTexture != NULL, , "Failed to create a cube map");

    m_LoadedCubeTextures[_ulIndex] = pTexture;

    m_ulCubeTextureRAMUsage += ComputeAnyTextureSize(pTexture);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::UnloadCubeMap
// Params : _ulIndex : Index of the cube map to free
// RetVal : None
// Descr. : Unload a cube map
// ------------------------------------------------------------------------------------------------
void XeTextureManager::UnloadCubeMap(ULONG _ulIndex)
{
    if (_ulIndex >= m_LoadedCubeTextures.size())
        return;

    m_ulCubeTextureRAMUsage -= ComputeAnyTextureSize(m_LoadedCubeTextures[_ulIndex]);

    // Flag the cube map to be unloaded when the time comes
    m_CubeMapsGarbageCollector.push_back(m_LoadedCubeTextures[_ulIndex]);

    // Clear the texture entry
    m_LoadedCubeTextures[_ulIndex] = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::ResolveToScratchBuffer
// Params : _dwID : Scratch buffer id
// RetVal : None
// Descr. : Copies current back buffer to scratch buffer texture
// ------------------------------------------------------------------------------------------------
void XeTextureManager::ResolveToScratchBuffer( DWORD _dwID, D3DRECT * pRect, BOOL bClearTarget, DWORD dwClearCol, BOOL bClearDepth, FLOAT fDepthClearValue )
{
    HRESULT hr;
    IDirect3DTexture9 * pScratchBuffer = (IDirect3DTexture9*)GetScratchBuffer( _dwID );
    ERR_X_Assert( pScratchBuffer );

    if (pScratchBuffer != NULL)
    {
#if !defined(_XENON_RENDER_PC)

        ULONG ulToResolve = D3DRESOLVE_RENDERTARGET0;

        // See if we need to resolve the depth texture
        if( (_dwID == XEOPAQUEDEPTHBUFFERD24S8_ID) || (_dwID == XEOPAQUEDEPTHBUFFERR32F_ID) )
        {
            ulToResolve = D3DRESOLVE_DEPTHSTENCIL;
        }

        if( bClearTarget )
        {
            ulToResolve |= D3DRESOLVE_CLEARRENDERTARGET;
        }

        if( bClearDepth )
        {
            ulToResolve |= D3DRESOLVE_CLEARDEPTHSTENCIL;
        }

        D3DVECTOR4 oClearCol;
        oClearCol.x = (float)((dwClearCol >> 24) & 0xFF) / 255.0f;
        oClearCol.y = (float)((dwClearCol >> 16) & 0xFF) / 255.0f;
        oClearCol.z = (float)((dwClearCol >>  8) & 0xFF) / 255.0f;
        oClearCol.w = (float)((dwClearCol >>  0) & 0xFF) / 255.0f;

        D3DPOINT point;
        D3DPOINT *pPoint = NULL;
        if( pRect )
        {
            pPoint = &point;
            pPoint->x = pRect->x1;
            pPoint->y = pRect->y1;

        }

        hr = m_pD3DDevice->Resolve( ulToResolve,
                                    pRect,
                                    pScratchBuffer,
                                    pPoint,
                                    0,
                                    0,
                                    NULL,
                                    fDepthClearValue,
                                    0,
                                    NULL);

        ERR_X_Assert( SUCCEEDED(hr) );

#else
        ERR_X_Assert( _dwID != XEOPAQUEDEPTHBUFFERR32F_ID );

        IDirect3DSurface9 *pBackBuffer, *pDestBuffer;

        m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
        pScratchBuffer->GetSurfaceLevel(0, &pDestBuffer);

        hr = m_pD3DDevice->StretchRect(pBackBuffer, NULL, pDestBuffer, NULL, D3DTEXF_NONE);
        ERR_X_Assert( SUCCEEDED(hr) );

        pBackBuffer->Release();
        pDestBuffer->Release();

        if( bClearTarget )
        {
            g_oXeRenderer.ClearTarget(0);
        }
        if( bClearDepth )
        {
            g_oXeRenderer.ClearDepthStencil( fDepthClearValue, 0 );
        }

#endif // defined(_XENON_RENDER_PC)
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeTextureManager::GenerateHeatShimmerNoiseTexture
// Params : None
// RetVal : None
// Descr. : Generates heat shimmering noise texture
// ------------------------------------------------------------------------------------------------
void XeTextureManager::GenerateHeatShimmerNoiseTexture(  bool _bForReset )
{
    HRESULT hr = S_OK;

    //
    // Create the texture
    //
    IDirect3DTexture9 * pHeatShimmerTexture = NULL;

    hr = D3DXCreateTexture( m_pD3DDevice,
                            HeatShimmerTextureSize,
                            HeatShimmerTextureSize,
                            1,
                            0,
                            D3DFMT_V8U8,
                            D3DPOOL_MANAGED,
                            &pHeatShimmerTexture );

    ERR_X_Assert( SUCCEEDED(hr) && (pHeatShimmerTexture != NULL));

    //
    // Fill the texture with noise
    //
    D3DLOCKED_RECT lock;
    hr = pHeatShimmerTexture->LockRect( 0, &lock, NULL, 0 );
    ERR_X_Assert(hr == S_OK);

    WORD* pwTexel = NULL;
    BYTE abyColor[2];
   
    BYTE* pwTextureBuffer = (BYTE*)lock.pBits;
    ERR_X_Assert(pwTextureBuffer != NULL);

    for (int y=0; y<HeatShimmerTextureSize; y++)
    {
        pwTexel = (WORD*) (pwTextureBuffer + (y * lock.Pitch));

        for (int x=0; x<HeatShimmerTextureSize; x++)
        {
            abyColor[0] = rand() % 256;
            abyColor[1] = rand() % 256;

            *pwTexel = *((WORD*)abyColor);

            pwTexel++;
        }
    }
    pHeatShimmerTexture->UnlockRect( 0 );

    if (_bForReset)
    {
        UpdateUserTexture( XEHEATSHIMMERNOISE_ID, pHeatShimmerTexture );
    }
    else
    {
        RegisterUserTexture( pHeatShimmerTexture );
    }
}



inline ULONG NormalizeColor(ULONG _ul_Color)
{
    FLOAT fR, fG, fB;

    fR = (((FLOAT)(UCHAR)(_ul_Color >> 16)) / 127.5f) - 1.0f;
    fG = (((FLOAT)(UCHAR)(_ul_Color >> 8) ) / 127.5f) - 1.0f;
    fB = (((FLOAT)(UCHAR)(_ul_Color)      ) / 127.5f) - 1.0f;

    FLOAT fInvLength = fSqrt((fR * fR) + (fG * fG) + (fB * fB));
    if (fInvLength <= 0.0005f)
        return _ul_Color;

    fInvLength = 1.0f / fInvLength;
    fR *= fInvLength;
    fG *= fInvLength;
    fB *= fInvLength;

    fR = MATH_f_FloatLimit(fR, -1.0f, 1.0f);
    fG = MATH_f_FloatLimit(fG, -1.0f, 1.0f);
    fB = MATH_f_FloatLimit(fB, -1.0f, 1.0f);

    return (_ul_Color & 0xff000000)                             |
           (((ULONG)((fR + 1.0f) * 127.5f) << 16) & 0x00ff0000) |
           (((ULONG)((fG + 1.0f) * 127.5f) <<  8) & 0x0000ff00) |
           (((ULONG)((fB + 1.0f) * 127.5f)      ) & 0x000000ff);
}

void NormalizeSurfaceInPlace(void* _pBuffer, DWORD _dwWidth, DWORD _dwHeight, DWORD _dwPitch)
{
    ULONG* pColors = (ULONG*)_pBuffer;

    _dwPitch >>= 2;

    for (DWORD j = 0; j < _dwHeight; ++j)
    {
        for (DWORD i = 0; i < _dwWidth; ++i)
        {
            pColors[i] = NormalizeColor(pColors[i]);
        }

        pColors += _dwPitch;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : FlipSurfaceInPlace
// Params : ..
// RetVal : ..
// Descr. : In-place flip a surface
// ------------------------------------------------------------------------------------------------
void FlipSurfaceInPlace(void* _pBuffer, DWORD _dwWidth, DWORD _dwHeight, DWORD _dwPitch, D3DFORMAT _eFormat)
{
    switch (_eFormat)
    {
        case D3DFMT_LIN_A8:
            FlipSurfaceInPlace_Generic(_pBuffer, _dwWidth, _dwHeight, _dwPitch, 1);
            break;

        case D3DFMT_LIN_A4R4G4B4:
        case D3DFMT_LIN_R5G6B5:
            FlipSurfaceInPlace_Generic(_pBuffer, _dwWidth, _dwHeight, _dwPitch, 2);
            break;

        case D3DFMT_LIN_A8R8G8B8:
        case D3DFMT_LIN_X8R8G8B8:
            FlipSurfaceInPlace_Generic(_pBuffer, _dwWidth, _dwHeight, _dwPitch, 4);
            break;

        case D3DFMT_LIN_DXT1:
            FlipSurfaceInPlace_DXTn(_pBuffer, _dwWidth, _dwHeight, _dwPitch, 4, 4, 8, FlipLineDXT1);
            break;

#if !defined(_XENON)
        case D3DFMT_LIN_DXT2:
#endif
        case D3DFMT_LIN_DXT3:
            FlipSurfaceInPlace_DXTn(_pBuffer, _dwWidth, _dwHeight, _dwPitch, 4, 4, 16, FlipLineDXT23);
            break;

#if !defined(_XENON)
        case D3DFMT_LIN_DXT4:
#endif
        case D3DFMT_LIN_DXT5:
            FlipSurfaceInPlace_DXTn(_pBuffer, _dwWidth, _dwHeight, _dwPitch, 4, 4, 16, FlipLineDXT45);
            break;

        default:
            XeValidate(FALSE, "Texture format is not supported");
            break;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : FlipSurfaceInPlace_Generic
// Params : ..
// RetVal : ..
// Descr. : In-place flip a generic surface
// ------------------------------------------------------------------------------------------------
void FlipSurfaceInPlace_Generic(void* _pBuffer, DWORD _dwWidth, DWORD _dwHeight, DWORD _dwPitch, DWORD _dwBPP)
{
    ERR_X_Assert(((_dwWidth * _dwBPP) <= FLIP_SCRATCH_BUFFER_SIZE) && "Flip scratch buffer is too small");

    UCHAR* pulPixels = (UCHAR*)_pBuffer;

    for (ULONG j = 0; j < _dwHeight / 2; ++j)
    {
        L_memcpy(g_aucFlipScratchBuffer,                     &pulPixels[j * _dwPitch],                   _dwWidth * _dwBPP);
        L_memcpy(&pulPixels[j * _dwPitch],                   &pulPixels[(_dwHeight - j - 1) * _dwPitch], _dwWidth * _dwBPP);
        L_memcpy(&pulPixels[(_dwHeight - j - 1) * _dwPitch], g_aucFlipScratchBuffer,                     _dwWidth * _dwBPP);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : FlipSurfaceInPlace_DXTn
// Params : ..
// RetVal : ..
// Descr. : In-place flip a DXTn surface
// ------------------------------------------------------------------------------------------------
void FlipSurfaceInPlace_DXTn(void* _pBuffer, DWORD _dwWidth, DWORD _dwHeight, DWORD _dwPitch, DWORD _dwBlockWidth, DWORD _dwBlockHeight, DWORD _dwBytesPerBlock, FLIPLINEPROC _pfnLineFlip)
{
    UCHAR* pucPixels = (UCHAR*)_pBuffer;
    DWORD  dwTotalSize;
    DWORD  dwNbLines;
    DWORD  dwLinePitch;

    if (_dwHeight < _dwBlockHeight)
        _dwHeight = _dwBlockHeight;

    if (_dwWidth < _dwBlockWidth)
        _dwWidth = _dwBlockWidth;

    dwTotalSize = ((_dwWidth * _dwHeight) * _dwBytesPerBlock) / (_dwBlockWidth * _dwBlockHeight);
    dwNbLines   = _dwHeight / _dwBlockHeight;
    dwLinePitch = (dwTotalSize / dwNbLines);

    ERR_X_Assert((dwLinePitch <= FLIP_SCRATCH_BUFFER_SIZE) && "Flip scratch buffer is too small");

    for (ULONG j = 0; j < dwNbLines / 2; ++j)
    {
        L_memcpy(g_aucFlipScratchBuffer,                        &pucPixels[j * dwLinePitch],                   dwLinePitch);
        L_memcpy(&pucPixels[j * dwLinePitch],                   &pucPixels[(dwNbLines - j - 1) * dwLinePitch], dwLinePitch);
        L_memcpy(&pucPixels[(dwNbLines - j - 1) * dwLinePitch], g_aucFlipScratchBuffer,                        dwLinePitch);

        _pfnLineFlip(&pucPixels[j * dwLinePitch],                   _dwWidth / _dwBlockWidth);
        _pfnLineFlip(&pucPixels[(dwNbLines - j - 1) * dwLinePitch], _dwWidth / _dwBlockWidth);
    }

    // Lower mip maps levels only have one 
    if (dwNbLines == 1)
    {
        _pfnLineFlip(pucPixels, _dwWidth / _dwBlockWidth);
    }
}

void FlipLineDXT1(void* _pBuffer, DWORD _dwNbBlocks)
{
#define SWAP_RGBA(_v1, _v2) ucTemp = pLine[i].ucLine##_v1; pLine[i].ucLine##_v1 = pLine[i].ucLine##_v2; pLine[i].ucLine##_v2 = ucTemp

    LineDXT1* pLine = (LineDXT1*)_pBuffer;
    UCHAR     ucTemp;

    for (ULONG i = 0; i < _dwNbBlocks; ++i)
    {
        SWAP_RGBA(0, 3);
        SWAP_RGBA(1, 2);
    }

#undef SWAP_RGBA
}

void FlipLineDXT23(void* _pBuffer, DWORD _dwNbBlocks)
{
#define SWAP_RGB(_v1, _v2) ucTemp = pLine[i].stDXT1Block.ucLine##_v1; pLine[i].stDXT1Block.ucLine##_v1 = pLine[i].stDXT1Block.ucLine##_v2; pLine[i].stDXT1Block.ucLine##_v2 = ucTemp
#define SWAP_A(_v1, _v2)   usTemp = pLine[i].usAlpha##_v1; pLine[i].usAlpha##_v1 = pLine[i].usAlpha##_v2; pLine[i].usAlpha##_v2 = usTemp

    LineDXT23* pLine = (LineDXT23*)_pBuffer;
    UCHAR  ucTemp;
    USHORT usTemp;

    for (ULONG i = 0; i < _dwNbBlocks; ++i)
    {
        SWAP_A(0, 3);
        SWAP_A(1, 2);
        SWAP_RGB(0, 3);
        SWAP_RGB(1, 2);
    }

#undef SWAP_RGB
#undef SWAP_A
}

void FlipLineDXT45(void* _pBuffer, DWORD _dwNbBlocks)
{
#define SWAP_RGB(_v1, _v2) ucTemp = pLine[i].stDXT1Block.ucLine##_v1; pLine[i].stDXT1Block.ucLine##_v1 = pLine[i].stDXT1Block.ucLine##_v2; pLine[i].stDXT1Block.ucLine##_v2 = ucTemp

    LineDXT45* pLine = (LineDXT45*)_pBuffer;
    ULONG64 ulAlpha;
    ULONG64 ulAlpha2;
    UCHAR ucTemp;

    for (ULONG i = 0; i < _dwNbBlocks; ++i)
    {
        SWAP_RGB(0, 3);
        SWAP_RGB(1, 2);

        ulAlpha =  (ULONG64)pLine[i].ucAlphaLine0
                | ((ULONG64)pLine[i].ucAlphaLine1 << 8)
                | ((ULONG64)pLine[i].ucAlphaLine2 << 16)
                | ((ULONG64)pLine[i].ucAlphaLine3 << 24)
                | ((ULONG64)pLine[i].ucAlphaLine4 << 32)
                | ((ULONG64)pLine[i].ucAlphaLine5 << 40);

        ulAlpha2 =  ((ulAlpha & 0x000000000fff) << 36)
                  | ((ulAlpha & 0xfff000000000) >> 36)
                  | ((ulAlpha & 0x000000fff000) << 12)
                  | ((ulAlpha & 0x000fff000000) >> 12);

        pLine[i].ucAlphaLine0 = (UCHAR)( ulAlpha2        & 0xff);
        pLine[i].ucAlphaLine1 = (UCHAR)((ulAlpha2 >> 8)  & 0xff);
        pLine[i].ucAlphaLine2 = (UCHAR)((ulAlpha2 >> 16) & 0xff);
        pLine[i].ucAlphaLine3 = (UCHAR)((ulAlpha2 >> 24) & 0xff);
        pLine[i].ucAlphaLine4 = (UCHAR)((ulAlpha2 >> 32) & 0xff);
        pLine[i].ucAlphaLine5 = (UCHAR)((ulAlpha2 >> 40) & 0xff);
    }

#undef SWAP_RGB
}

static ULONG ComputeTextureSize(ULONG _ulWidth, ULONG _ulHeight, ULONG _ulNbLevels, D3DFORMAT _eFormat)
{
    ULONG ulNbBitsPerPixels = 32;
    ULONG ulSize = 0;
    ULONG ulLevel;

    switch (_eFormat)
    {
#if defined(_XENON)
        case D3DFMT_DXT1:
        case D3DFMT_CTX1:
        case D3DFMT_LIN_CTX1:
#endif
        case D3DFMT_LIN_DXT1: ulNbBitsPerPixels = 4; break;

#if defined(_XENON)
        case D3DFMT_DXT3:
        case D3DFMT_DXT5:
        case D3DFMT_DXN:
        case D3DFMT_LIN_DXN:
#endif
        case D3DFMT_LIN_DXT3:
        case D3DFMT_LIN_DXT5: ulNbBitsPerPixels = 8; break;

        case D3DFMT_A8:
            ulNbBitsPerPixels = 8;
            break;

        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4:
        case D3DFMT_X4R4G4B4:
        case D3DFMT_D16:
        case D3DFMT_R16F:
            ulNbBitsPerPixels = 16;
            break;

        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_A2R10G10B10:
        case D3DFMT_R32F:
            ulNbBitsPerPixels = 32;
            break;

        case D3DFMT_A16B16G16R16F:
            ulNbBitsPerPixels = 64;
            break;

        case D3DFMT_A32B32G32R32F:
            ulNbBitsPerPixels = 128;
            break;
    }

    if (_ulNbLevels == 0)
        _ulNbLevels = TEX_ul_XeGetMaxMipMapsCount(_ulWidth, _ulHeight);

    for (ulLevel = 0; ulLevel < _ulNbLevels; ++ulLevel)
    {
        ulSize += (_ulWidth * _ulHeight * ulNbBitsPerPixels) / 8;

        _ulWidth  >>= 1;
        _ulHeight >>= 1;

        if (_ulWidth == 0)
            _ulWidth = 1;
        if (_ulHeight == 0)
            _ulHeight = 1;
    }

    return ulSize;
}

ULONG ComputeAnyTextureSize(IDirect3DBaseTexture9* _pTexture)
{
    D3DSURFACE_DESC stDesc;
    D3DVOLUME_DESC  stVolumeDesc;

    if (_pTexture == NULL)
        return 0;

    switch (_pTexture->GetType())
    {
        case D3DRTYPE_TEXTURE:
            ((IDirect3DTexture9*)_pTexture)->GetLevelDesc(0, &stDesc);
            return ComputeTextureSize(stDesc.Width, stDesc.Height, ((IDirect3DTexture9*)_pTexture)->GetLevelCount(), stDesc.Format);

        case D3DRTYPE_CUBETEXTURE:
            ((IDirect3DCubeTexture9*)_pTexture)->GetLevelDesc(0, &stDesc);
            return 6 * ComputeTextureSize(stDesc.Width, stDesc.Height, ((IDirect3DCubeTexture9*)_pTexture)->GetLevelCount(), stDesc.Format);

        case D3DRTYPE_VOLUMETEXTURE:
            ((IDirect3DVolumeTexture9*)_pTexture)->GetLevelDesc(0, &stVolumeDesc);
            return stVolumeDesc.Depth * ComputeTextureSize(stVolumeDesc.Width, stVolumeDesc.Height, ((IDirect3DVolumeTexture9*)_pTexture)->GetLevelCount(), stVolumeDesc.Format);
    }

    return 0;
}

D3DFORMAT XeTextureManager::GetTextureFormat(DWORD _dwID)
{
    IDirect3DBaseTexture9* pTexture = GetTextureFromID(_dwID);
    D3DSURFACE_DESC        stDesc;
    D3DVOLUME_DESC         stVolumeDesc;

    if (pTexture == NULL)
        return D3DFMT_UNKNOWN;

    switch (pTexture->GetType())
    {
        case D3DRTYPE_TEXTURE:
            ((IDirect3DTexture9*)pTexture)->GetLevelDesc(0, &stDesc);
            return stDesc.Format;

        case D3DRTYPE_CUBETEXTURE:
            ((IDirect3DCubeTexture9*)pTexture)->GetLevelDesc(0, &stDesc);
            return stDesc.Format;

        case D3DRTYPE_VOLUMETEXTURE:
            ((IDirect3DVolumeTexture9*)pTexture)->GetLevelDesc(0, &stVolumeDesc);
            return stVolumeDesc.Format;
    }

    return D3DFMT_UNKNOWN;
}

int XeTextureManager::GetDepthBufferID()
{
#ifdef _XENON_RENDER_PC
    return XEOPAQUEDEPTHBUFFERR32F_ID;
#else

    if(g_oXeRenderer.IsFastZEnabled())
    {
        return XEOPAQUEDEPTHBUFFERD24S8_ID;
    }

    return XEOPAQUEDEPTHBUFFERR32F_ID;
#endif
}
