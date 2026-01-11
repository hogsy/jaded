// ------------------------------------------------------------------------------------------------
// File   : XenonTexture.cpp
// Date   : 2005-07-29
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include <Windows.h>

// Include from the Xenon XDK path
#include "d3d9.h"
#include "d3dx9.h"
#include "xgraphics.h"

#include "XenonTexture.h"
#include "XeSurface.h"

// ------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// ------------------------------------------------------------------------------------------------
static void XeTexture_Initialize(void);
static void XeTexture_Shutdown(void);

static DWORD XeTexture_GetNumLevels(DWORD _dwWidth, DWORD _dwHeight, DWORD _dwNbLevels);
static DWORD XeTexture_GetMaxLevels(DWORD _dwWidth, DWORD _dwHeight);

static DWORD XeTexture_GetMipLevelDesc(const XeTexture_Descriptor* _pDescriptor, DWORD _dwLevel, XGTEXTURE_DESC* _pDesc);

static D3DFORMAT        XeTexture_GetD3DFormat(XeTexture_Format _eFormat);
static XeTexture_Format XeTexture_GetNativeFormat(D3DFORMAT _eFormat);

static void XeTexture_SwapDword(DWORD* _pDword);

// ------------------------------------------------------------------------------------------------
// PRIVATE CLASS
// ------------------------------------------------------------------------------------------------
class XeTextureReader
{
public:

    XeTextureReader(const LPVOID _pBuffer, DWORD _dwSize);
    ~XeTextureReader(void);

    bool ReadDword(DWORD* _pdwData);
    bool ReadData(LPVOID _pBuffer, DWORD _dwSize);

private:

    const BYTE* m_pbyBuffer;
    DWORD       m_dwOffset;
    DWORD       m_dwSize;
};

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

BOOL WINAPI DllMain(HINSTANCE _hInstance, DWORD _dwReason, LPVOID _pReserved)
{
    static DWORD s_dwNbProcesses = 0;

    switch (_dwReason)
    {
        case DLL_PROCESS_ATTACH:
            ++s_dwNbProcesses;

            // First process to attach, initialize some stuff
            if (s_dwNbProcesses == 1)
            {
                XeTexture_Initialize();
            }
            break;

        case DLL_PROCESS_DETACH:
            --s_dwNbProcesses;

            // Last process to detach, clean up
            if (s_dwNbProcesses == 0)
            {
                XeTexture_Shutdown();
            }

            break;
    }

    return TRUE;
}

XE_EXPORT DWORD XeTexture_GetTextureFormat(XeTexture_Format _eFormat)
{
    switch (_eFormat)
    {
        case XETF_A8R8G8B8: return D3DFMT_A8R8G8B8;
        case XETF_A8:       return D3DFMT_A8;
        case XETF_DXT1:     return D3DFMT_DXT1;
        case XETF_DXT5:     return D3DFMT_DXT5;
        case XETF_DXN:      return D3DFMT_DXN;
        case XETF_CTX1:     return D3DFMT_CTX1;
    }

    return D3DFMT_UNKNOWN;
}

XE_EXPORT BOOL XeTexture_CompressTexture(DWORD                 _dwSrcWidth, 
                                         DWORD                 _dwSrcHeight, 
                                         LPVOID                _pSrcImage, 
                                         XeTexture_Descriptor* _pDescriptor)
{
    XGTEXTURE_DESC stBaseLevelDesc;
    XeSurface      stSurface((BYTE*)_pSrcImage, _dwSrcWidth, _dwSrcHeight);
    HRESULT hr;
    DWORD dwTailLevel;
    DWORD dwGPUFormat;
    DWORD dwTextureSize;
    DWORD dwDataSize     = 0;
    DWORD dwCopyFlags    = 0;
    BYTE* pBitsLinear    = NULL;
    BYTE* pBitsTiled     = NULL;
    BYTE* pBitsConverted = NULL;
    BYTE* pBitsFinal     = NULL;

    // Validate source
    if ((_dwSrcWidth  == 0)    || 
        (_dwSrcHeight == 0)    || 
        (_pSrcImage   == NULL) || 
        (_pDescriptor == NULL))
        return FALSE;

    // Validate destination
    if ((_pDescriptor->dwWidth  == 0) || (_pDescriptor->dwWidth  > 4096) ||
        (_pDescriptor->dwHeight == 0) || (_pDescriptor->dwHeight > 4096) ||
        (_pDescriptor->dwFormat == D3DFMT_UNKNOWN))
        return FALSE;

    // Level count
    _pDescriptor->dwNbLevels = XeTexture_GetNumLevels(_pDescriptor->dwWidth,
                                                      _pDescriptor->dwHeight,
                                                      _pDescriptor->dwNbLevels);

    if (_pDescriptor->pRawContent != NULL)
    {
        _pDescriptor->pRawContent->Release();
        _pDescriptor->pRawContent = NULL;
    }

    // Flip the texture since Jade works with textures upside down
    stSurface.Flip();

    // Tail mip map level (invalidate if no mip maps)
    dwTailLevel = XGGetMipTailBaseLevel(_pDescriptor->dwWidth, _pDescriptor->dwHeight, FALSE);
    if (_pDescriptor->dwNbLevels == 1)
    {
        dwTailLevel = 0xffff;
    }

    dwTextureSize = XeTexture_GetMipLevelDesc(_pDescriptor, 0, &stBaseLevelDesc);
    dwGPUFormat   = (stBaseLevelDesc.Format & D3DFORMAT_TEXTUREFORMAT_MASK) >> D3DFORMAT_TEXTUREFORMAT_SHIFT;

    // Allocate the memory for the linear buffer
    pBitsLinear = new BYTE [stBaseLevelDesc.SlicePitch];
    XE_ASSERT(pBitsLinear != NULL);

    pBitsConverted = pBitsLinear;
    pBitsFinal     = pBitsLinear;

    // Allocate memory for the tiled buffer
    if (stBaseLevelDesc.Format & D3DFORMAT_TILED_MASK)
    {
        pBitsTiled = new BYTE [stBaseLevelDesc.SlicePitch];
        XE_ASSERT(pBitsTiled != NULL);

        pBitsFinal = pBitsTiled;
    }

    // Prepare the output buffer and write the texture header
    _pDescriptor->pRawContent = XeTexture_SharedBuffer::Create();
    _pDescriptor->pRawContent->WriteDword(XE_TEXTURE_2D);
    _pDescriptor->pRawContent->WriteDword(_pDescriptor->dwWidth);
    _pDescriptor->pRawContent->WriteDword(_pDescriptor->dwHeight);
    _pDescriptor->pRawContent->WriteDword(XeTexture_GetD3DFormat((XeTexture_Format)_pDescriptor->dwFormat));
    _pDescriptor->pRawContent->WriteDword(_pDescriptor->dwNbLevels);
    _pDescriptor->pRawContent->WriteDword(dwTailLevel);
    _pDescriptor->pRawContent->WriteDword(dwTextureSize);
    _pDescriptor->pRawContent->WriteDword(0);    // SC: Future use

    // Convert all levels
    for (DWORD dwLevel = 0; dwLevel < _pDescriptor->dwNbLevels; ++dwLevel)
    {
        XGTEXTURE_DESC stLevelDesc;

        XeTexture_GetMipLevelDesc(_pDescriptor, dwLevel, &stLevelDesc);

        // Resize the surface
        stSurface.Resize(stLevelDesc.Width, stLevelDesc.Height);

        // Re-normalize the level if we are dealing with a normal map
        if (_pDescriptor->dwFormat >= XETF_NORMALMAP)
        {
            stSurface.Normalize();
        }

        if (dwLevel <= dwTailLevel)
        {
            memset(pBitsLinear, 0, stLevelDesc.SlicePitch);
        }

        // Convert the source pixels to our destination format
        {
            FLOAT fAlphaThreshold = 0.5f;

            // Source format is D3DXCOLOR (float[4])
            D3DFORMAT eSrcFormat = (D3DFORMAT)MAKED3DFMT2(GPUTEXTUREFORMAT_32_32_32_32_FLOAT,
                                                          GPUENDIAN_NONE, FALSE,
                                                          GPUSIGN_SIGNED, GPUSIGN_SIGNED, GPUSIGN_SIGNED, GPUSIGN_SIGNED,
                                                          GPUNUMFORMAT_INTEGER,
                                                          GPUSWIZZLE_X, GPUSWIZZLE_Y, GPUSWIZZLE_Z, GPUSWIZZLE_W);

            // Convert to a non-tiled format first for XGCopySurface
            D3DFORMAT eDestFormat = (D3DFORMAT)(stLevelDesc.Format & ~D3DFORMAT_TILED_MASK);

            // SC: Patch since the XGCopySurface will destroy the alpha channel when copying to a D3DFMT_A8 texture...
            if (_pDescriptor->dwFormat == XETF_A8)
            {
                eSrcFormat  = (D3DFORMAT)MAKED3DFMT(GPUTEXTUREFORMAT_32_32_32_32_FLOAT, GPUENDIAN_NONE, FALSE,
                                                    GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION, 
                                                    (GPUSWIZZLE_X | (GPUSWIZZLE_X << 3) | (GPUSWIZZLE_X << 6) | (GPUSWIZZLE_W << 9)));

                eDestFormat = (D3DFORMAT)MAKED3DFMT(GPUTEXTUREFORMAT_8, GPUENDIAN_NONE, TRUE,
                                                    GPUSIGN_ALL_UNSIGNED, GPUNUMFORMAT_FRACTION,
                                                    (GPUSWIZZLE_W | (GPUSWIZZLE_Z << 3) | (GPUSWIZZLE_Y << 6) | (GPUSWIZZLE_X << 9)));

                fAlphaThreshold = 0.99f;
            }

            if (dwLevel >= dwTailLevel)
            {
                pBitsConverted = pBitsLinear + XGGetMipTailLevelOffset(stBaseLevelDesc.Width, stBaseLevelDesc.Height,
                                                                       stBaseLevelDesc.Depth, dwLevel, dwGPUFormat,
                                                                       (pBitsTiled != NULL), FALSE);
            }

            // Format conversion
            hr = XGCopySurface(pBitsConverted, stLevelDesc.RowPitch, stLevelDesc.Width, stLevelDesc.Height,
                               eDestFormat, NULL, stSurface.GetRawData(), 4 * sizeof(FLOAT) * stLevelDesc.Width,
                               eSrcFormat, NULL, dwCopyFlags, fAlphaThreshold);
            XE_ASSERT(SUCCEEDED(hr));
        }

        if (dwLevel < dwTailLevel)
        {
            // Tile the destination pixels if necessary
            if (pBitsTiled)
            {
                POINT stPoint = { 0, 0 };
                RECT  stRect;

                SetRect(&stRect, 0, 0, stLevelDesc.WidthInBlocks, stLevelDesc.HeightInBlocks);

                XGTileSurface(pBitsTiled, stLevelDesc.RowPitch / stLevelDesc.BytesPerBlock, 
                              stLevelDesc.HeightInBlocks, &stPoint, pBitsLinear, stLevelDesc.RowPitch,
                              &stRect, stLevelDesc.BytesPerBlock);
            }

            // Write the content of this level
            _pDescriptor->pRawContent->WriteData(pBitsFinal, stLevelDesc.SlicePitch);

            dwDataSize += stLevelDesc.SlicePitch;
        }
    }

    // Mip map tail
    if (_pDescriptor->dwNbLevels > dwTailLevel)
    {
        XGTEXTURE_DESC stLevelDesc;

        XeTexture_GetMipLevelDesc(_pDescriptor, dwTailLevel, &stLevelDesc);

        if (pBitsTiled)
        {
            UINT dwBlockWidth;
            UINT dwBlockHeight;
            XGGetBlockDimensions(dwGPUFormat, &dwBlockWidth, &dwBlockHeight);

            UINT dwTileWidth          = GPU_TEXTURE_TILE_DIMENSION * dwBlockWidth;
            UINT dwTileHeight         = GPU_TEXTURE_TILE_DIMENSION * dwBlockHeight;
            UINT dwTailWidthInBlocks  = ((stLevelDesc.Width  + dwTileWidth  - 1) & ~(dwTileWidth  - 1)) / dwBlockWidth;
            UINT dwTailHeightInBlocks = ((stLevelDesc.Height + dwTileHeight - 1) & ~(dwTileHeight - 1)) / dwBlockHeight;

            POINT stPoint = { 0, 0 };
            RECT  stRect;
            SetRect(&stRect, 0, 0, dwTailWidthInBlocks, dwTailHeightInBlocks);

            XGTileSurface(pBitsTiled, dwTailWidthInBlocks, dwTailHeightInBlocks, &stPoint,
                          pBitsLinear, stLevelDesc.RowPitch, &stRect, stLevelDesc.BytesPerBlock);
        }

        // Write the content of this level
        _pDescriptor->pRawContent->WriteData(pBitsFinal, stLevelDesc.SlicePitch);

        dwDataSize += stLevelDesc.SlicePitch;
    }


    if (pBitsTiled)
        delete [] pBitsTiled;

    delete [] pBitsLinear;

    return TRUE;
}

XE_EXPORT BOOL XeTexture_UncompressTexture(const LPVOID          _pBuffer,
                                           DWORD                 _dwBufferSize,
                                           XeTexture_Descriptor* _pDescriptor)
{
    XE_ASSERT(_pBuffer != NULL);
    XE_ASSERT(_dwBufferSize > 0);
    XE_ASSERT(_pDescriptor != NULL);

    XeTexture_Descriptor oDesc;

    if ((_pDescriptor->dwWidth == 0) || (_pDescriptor->dwHeight == 0))
        return FALSE;

    if (_pDescriptor->pRawContent)
    {
        _pDescriptor->pRawContent->Release();
        _pDescriptor->pRawContent = NULL;
    }

    XeTextureReader oReader(_pBuffer, _dwBufferSize);

    DWORD dwMagic;
    if (!oReader.ReadDword(&dwMagic))
        return FALSE;
    if (dwMagic != XE_TEXTURE_2D)
        return FALSE;

    if (!oReader.ReadDword(&oDesc.dwWidth))
        return FALSE;

    if (!oReader.ReadDword(&oDesc.dwHeight))
        return FALSE;

    DWORD dwFormat;
    if (!oReader.ReadDword(&dwFormat))
        return FALSE;

    oDesc.dwFormat = XeTexture_GetNativeFormat((D3DFORMAT)dwFormat);

    DWORD dwGPUFormat = (dwFormat & D3DFORMAT_TEXTUREFORMAT_MASK) >> D3DFORMAT_TEXTUREFORMAT_SHIFT;

    if (!oReader.ReadDword(&oDesc.dwNbLevels))
        return FALSE;

    DWORD dwTailLevel;
    if (!oReader.ReadDword(&dwTailLevel))
        return FALSE;

    DWORD dwTextureSize;
    if (!oReader.ReadDword(&dwTextureSize))
        return FALSE;

    DWORD dwTemp;
    if (!oReader.ReadDword(&dwTemp))
        return FALSE;

    XGTEXTURE_DESC oXGDesc;
    XeTexture_GetMipLevelDesc(&oDesc, 0, &oXGDesc);

    BYTE* pSourceBuffer = new BYTE [oXGDesc.SlicePitch];
    XE_ASSERT(pSourceBuffer != NULL);

    // Read the compressed surface
    oReader.ReadData(pSourceBuffer, oXGDesc.SlicePitch);

    // Untile the surface
    if (dwFormat & D3DFORMAT_TILED_MASK)
    {
        POINT stPoint = { 0, 0 };
        RECT  stRect;

        BYTE* pLinearBuffer = new BYTE [oXGDesc.SlicePitch];
        XE_ASSERT(pLinearBuffer != NULL);

        if (dwTailLevel > 0)
        {
            SetRect(&stRect, 0, 0, oXGDesc.WidthInBlocks, oXGDesc.HeightInBlocks);

            XGUntileSurface(pLinearBuffer, oXGDesc.RowPitch, &stPoint, pSourceBuffer, oXGDesc.WidthInBlocks,
                            oXGDesc.HeightInBlocks, &stRect, oXGDesc.BytesPerBlock);
        }
        else
        {
            // Untile the ENTIRE level, XGCopySurface() will extract the first level afterward

            UINT dwBlockWidth;
            UINT dwBlockHeight;
            XGGetBlockDimensions(dwGPUFormat, &dwBlockWidth, &dwBlockHeight);

            UINT dwTileWidth          = GPU_TEXTURE_TILE_DIMENSION * dwBlockWidth;
            UINT dwTileHeight         = GPU_TEXTURE_TILE_DIMENSION * dwBlockHeight;
            UINT dwTailWidthInBlocks  = ((oXGDesc.Width  + dwTileWidth  - 1) & ~(dwTileWidth  - 1)) / dwBlockWidth;
            UINT dwTailHeightInBlocks = ((oXGDesc.Height + dwTileHeight - 1) & ~(dwTileHeight - 1)) / dwBlockHeight;

            POINT stPoint = { 0, 0 };
            RECT  stRect;
            SetRect(&stRect, 0, 0, dwTailWidthInBlocks, dwTailHeightInBlocks);

            XGUntileSurface(pLinearBuffer, oXGDesc.RowPitch, &stPoint, pSourceBuffer, dwTailWidthInBlocks,
                            dwTailHeightInBlocks, &stRect, oXGDesc.BytesPerBlock);
        }

        // Swap the buffers to proceed with copy
        delete [] pSourceBuffer;
        pSourceBuffer = pLinearBuffer;
    }

    DWORD* pConvertedBits = new DWORD [oDesc.dwWidth * oDesc.dwHeight];
    XE_ASSERT(pConvertedBits != NULL);

    D3DFORMAT eSrcFormat  = (D3DFORMAT)(dwFormat & ~D3DFORMAT_TILED_MASK);
    D3DFORMAT eDestFormat = D3DFMT_LIN_A8R8G8B8;

    eDestFormat = (D3DFORMAT)(((DWORD)eDestFormat & ~D3DFORMAT_ENDIAN_MASK) | (GPUENDIAN_NONE << D3DFORMAT_ENDIAN_SHIFT));

    // Create the buffer
    _pDescriptor->pRawContent = XeTexture_SharedBuffer::Create();

    if (dwTailLevel == 0)
    {
        DWORD dwOffset = XGGetMipTailLevelOffset(oDesc.dwWidth, oDesc.dwHeight, 1, 0, dwGPUFormat, FALSE, FALSE);

        XGCopySurface(pConvertedBits, oDesc.dwWidth * sizeof(DWORD), oDesc.dwWidth, oDesc.dwHeight,
                      eDestFormat, NULL, pSourceBuffer + dwOffset, oXGDesc.RowPitch, eSrcFormat, NULL, 0, 0.0f);

    }
    else
    {
        XGCopySurface(pConvertedBits, oDesc.dwWidth * sizeof(DWORD), oDesc.dwWidth, oDesc.dwHeight,
                      eDestFormat, NULL, pSourceBuffer, oXGDesc.RowPitch, eSrcFormat, NULL, 0, 0.0f);
    }

    // Adjust the blue component for 2 channels normal map formats
    if ((eSrcFormat == D3DFMT_LIN_DXN) || (eSrcFormat == D3DFMT_LIN_CTX1))
    {
        for (DWORD i = 0; i < oDesc.dwWidth * oDesc.dwHeight; ++i)
        {
            D3DXCOLOR oColor = pConvertedBits[i];

            oColor.r = (oColor.r * 2.0f) - 1.0f;
            oColor.g = (oColor.g * 2.0f) - 1.0f;
            oColor.b = sqrtf(1.0f - (oColor.r * oColor.r) - (oColor.g * oColor.g));

            oColor.r = (oColor.r * 0.5f) + 0.5f;
            oColor.g = (oColor.g * 0.5f) + 0.5f;
            oColor.b = (oColor.b * 0.5f) + 0.5f;
            oColor.a = 0.0f;

            FLOAT fSwap = oColor.r;
            oColor.r    = oColor.g;
            oColor.g    = fSwap;

            pConvertedBits[i] = (DWORD)oColor;
        }
    }

    _pDescriptor->pRawContent->WriteData(pConvertedBits, oDesc.dwWidth * oDesc.dwHeight * sizeof(DWORD));

    delete [] pConvertedBits;
    delete [] pSourceBuffer;

    return TRUE;
}

#if defined(_DEBUG)

void XeTexture_Breakpoint(void)
{
    __asm int 03h;
}

#endif

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XeTexture_SharedBuffer
// ------------------------------------------------------------------------------------------------

#if defined(XENONTEXTURE_EXPORTS)
XeTexture_SharedBuffer* XeTexture_SharedBuffer::Create(void)
{
    return new XeTexture_SharedBuffer();
}
#endif

void XeTexture_SharedBuffer::AllocMem(DWORD _dwSize)
{
    FreeMem();
    ReallocMem(_dwSize);
}

void XeTexture_SharedBuffer::ReallocMem(DWORD _dwSize)
{
    if (_dwSize == 0)
        _dwSize = 1;

    if (_dwSize <= m_dwAllocSize)
        return;

    if (m_pbyBuffer != NULL)
    {
        BYTE* pTempBuffer = m_pbyBuffer;

        m_pbyBuffer = new BYTE [_dwSize];
        memset(m_pbyBuffer, 0, _dwSize);
        memcpy(m_pbyBuffer, pTempBuffer, m_dwAllocSize * sizeof(BYTE));

        delete [] pTempBuffer;

        m_dwAllocSize = _dwSize;
    }
    else
    {
        m_pbyBuffer   = new BYTE [_dwSize];
        m_dwAllocSize = _dwSize;
        m_dwSize      = 0;

        memset(m_pbyBuffer, 0, _dwSize);
    }
}

void XeTexture_SharedBuffer::FreeMem(void)
{
    if (m_pbyBuffer != NULL)
    {
        delete [] m_pbyBuffer;
        m_pbyBuffer = NULL;
    }

    m_dwSize      = 0;
    m_dwAllocSize = 0;
}

void XeTexture_SharedBuffer::WriteDword(DWORD _dwData)
{
    ReallocMem(m_dwSize + sizeof(DWORD));

    XeTexture_SwapDword(&_dwData);

    memcpy(&m_pbyBuffer[m_dwSize], &_dwData, sizeof(DWORD));
    m_dwSize += sizeof(DWORD);
}

void XeTexture_SharedBuffer::WriteData(const void* _pData, DWORD _dwLength)
{
    ReallocMem(m_dwSize + _dwLength);

    memcpy(&m_pbyBuffer[m_dwSize], _pData, _dwLength);
    m_dwSize += _dwLength;
}

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - PRIVATE FUNCTIONS
// ------------------------------------------------------------------------------------------------

static void XeTexture_Initialize(void)
{
}

static void XeTexture_Shutdown(void)
{
}

static DWORD XeTexture_GetMaxLevels(DWORD _dwWidth, DWORD _dwHeight)
{
#define XE_MIP_CHECK(_size, _count) if (dwMax >= _size) return _count

    DWORD dwMax = (_dwWidth > _dwHeight) ? _dwWidth : _dwHeight;

    XE_MIP_CHECK(2048, 12);
    XE_MIP_CHECK(1024, 11);
    XE_MIP_CHECK(512,  10);
    XE_MIP_CHECK(256,  9);
    XE_MIP_CHECK(128,  8);
    XE_MIP_CHECK(64,   7);
    XE_MIP_CHECK(32,   6);
    XE_MIP_CHECK(16,   5);
    XE_MIP_CHECK(8,    4);
    XE_MIP_CHECK(4,    3);
    XE_MIP_CHECK(2,    2);

    return 1;

#undef XE_MIP_CHECK
}

static DWORD XeTexture_GetNumLevels(DWORD _dwWidth, DWORD _dwHeight, DWORD _dwNbLevels)
{
    DWORD dwMaxLevels = XeTexture_GetMaxLevels(_dwWidth, _dwHeight);

    if ((_dwNbLevels == 0) || (_dwNbLevels > dwMaxLevels))
        return dwMaxLevels;

    return _dwNbLevels;
}

static DWORD XeTexture_GetMipLevelDesc(const XeTexture_Descriptor* _pDescriptor, DWORD _dwLevel, XGTEXTURE_DESC* _pDesc)
{
    D3DTexture stD3DTex;

    DWORD dwTextureSize = XGSetTextureHeader(_pDescriptor->dwWidth, _pDescriptor->dwHeight, _pDescriptor->dwNbLevels, 0, 
                                             XeTexture_GetD3DFormat((XeTexture_Format)_pDescriptor->dwFormat), D3DPOOL_DEFAULT, 
                                             0, XGHEADER_CONTIGUOUS_MIP_OFFSET, 0, &stD3DTex, NULL, NULL);

    XGGetTextureDesc(&stD3DTex, _dwLevel, _pDesc);

    return dwTextureSize;
}

static D3DFORMAT XeTexture_GetD3DFormat(XeTexture_Format _eFormat)
{
    return (D3DFORMAT)XeTexture_GetTextureFormat(_eFormat);
}

static XeTexture_Format XeTexture_GetNativeFormat(D3DFORMAT _eFormat)
{
    switch (_eFormat)
    {
        case D3DFMT_A8R8G8B8: return XETF_A8R8G8B8;
        case D3DFMT_A8:       return XETF_A8;
        case D3DFMT_DXT1:     return XETF_DXT1;
        case D3DFMT_DXT5:     return XETF_DXT5;
        case D3DFMT_DXN:      return XETF_DXN;
        case D3DFMT_CTX1:     return XETF_CTX1;
    }

    XE_ASSERT(false);

    return XETF_A8R8G8B8;
}

static void XeTexture_SwapDword(DWORD* _pDword)
{
    *_pDword = ((*_pDword & 0x000000ff) << 24) | 
               ((*_pDword & 0x0000ff00) <<  8) | 
               ((*_pDword & 0x00ff0000) >>  8) | 
               ((*_pDword & 0xff000000) >> 24);
}

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XeTextureReader
// ------------------------------------------------------------------------------------------------

XeTextureReader::XeTextureReader(const LPVOID _pBuffer, DWORD _dwSize)
: m_pbyBuffer((BYTE*)_pBuffer), m_dwSize(_dwSize), m_dwOffset(0)
{
}

XeTextureReader::~XeTextureReader(void)
{
}

bool XeTextureReader::ReadDword(DWORD* _pdwData)
{
    if (m_dwOffset + sizeof(DWORD) > m_dwSize)
        return false;

    memcpy(_pdwData, &m_pbyBuffer[m_dwOffset], sizeof(DWORD));
    XeTexture_SwapDword(_pdwData);

    m_dwOffset += sizeof(DWORD);

    return true;
}

bool XeTextureReader::ReadData(LPVOID _pBuffer, DWORD _dwSize)
{
    if (m_dwOffset + _dwSize > m_dwSize)
        return false;

    memcpy(_pBuffer, &m_pbyBuffer[m_dwOffset], _dwSize);

    m_dwOffset += _dwSize;

    return true;
}
