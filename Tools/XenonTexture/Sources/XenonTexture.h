// ------------------------------------------------------------------------------------------------
// File   : XenonTexture.h
// Date   : 2005-07-29
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XENONTEXTURE_H
#define GUARD_XENONTEXTURE_H

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------
#if defined(XENONTEXTURE_EXPORTS)

#define XE_EXPORT extern "C" __declspec(dllexport)

#   if defined(_DEBUG)
        void XeTexture_Breakpoint(void);
#       define XE_ASSERT(_x_) if (!(_x_)) { XeTexture_Breakpoint(); }
#   else
#       define XE_ASSERT(_x_)
#   endif

#define XE_EXPORT_FCT(_ret_, _fct_) XE_EXPORT _ret_ _fct_

#else

#define XE_EXPORT
#define XE_ASSERT(_x_)

#define XE_EXPORT_FCT(_ret_, _fct_) typedef _ret_ (*_fct_)

#endif

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------
#define XE_TEXTURE_2D   (DWORD)0x44324b4b
#define XE_TEXTURE_CUBE (DWORD)0x42434b4b

// ------------------------------------------------------------------------------------------------
// TYPES
// ------------------------------------------------------------------------------------------------
enum XeTexture_Format
{
    XETF_COLORMAP    = 0,
    XETF_A8R8G8B8    = XETF_COLORMAP,
    XETF_A8,
    XETF_DXT1,
    XETF_DXT5,

    XETF_NORMALMAP   = 100,
    XETF_DXN         = XETF_NORMALMAP,
    XETF_CTX1,

    XETF_FORCE_DWORD = 0x7fffffff
};

enum XeTexture_Type
{
    XETT_2D          = 0,
    XETT_CUBE        = 1,

    XETT_FORCE_DWORD = 0x7fffffff
};

// ------------------------------------------------------------------------------------------------
// STRUCTURES
// ------------------------------------------------------------------------------------------------

// SC: Must be a struct and the functions declared as virtual must remain virtual !
//     This makes it possible to use a C interface and to make sure the memory is freed by the
//     right process.

struct XeTexture_SharedBuffer
{
    XeTexture_SharedBuffer(void)
    : m_pbyBuffer(NULL), m_dwSize(0), m_dwAllocSize(0)
    {
    }

    virtual ~XeTexture_SharedBuffer(void)
    {
        FreeMem();
    }

public:

#if defined(XENONTEXTURE_EXPORTS)
    static XeTexture_SharedBuffer* Create(void);
#endif

    void Release(void)
    {
        delete this;
    }

#if defined(XENONTEXTURE_EXPORTS)
    void WriteDword(DWORD _dwData);
    void WriteData(const void* _pData, DWORD _dwLength);
#endif

    inline DWORD GetBufferSize(void) const { return m_dwSize; }
    inline const BYTE* GetBuffer(void) const { return m_pbyBuffer; }

private:

    virtual void AllocMem(DWORD _dwSize);
    virtual void ReallocMem(DWORD _dwSize);
    virtual void FreeMem(void);

private:

    BYTE* m_pbyBuffer;
    DWORD m_dwSize;
    DWORD m_dwAllocSize;
};

struct XeTexture_Descriptor
{
    XeTexture_Descriptor(void)
    : dwWidth(0), dwHeight(0), dwFormat(D3DFMT_UNKNOWN), dwNbLevels(0), pRawContent(NULL)
    {
    }

    ~XeTexture_Descriptor(void)
    {
        if (pRawContent != NULL)
        {
            pRawContent->Release();
            pRawContent = NULL;
        }
    }

    DWORD dwWidth;
    DWORD dwHeight;
    DWORD dwFormat;
    DWORD dwNbLevels;

    XeTexture_SharedBuffer* pRawContent;
};

// ------------------------------------------------------------------------------------------------
// EXPORTED FUNCTIONS
// ------------------------------------------------------------------------------------------------

#if defined(XENONTEXTURE_EXPORTS)

BOOL WINAPI DllMain(HINSTANCE _hInstance, DWORD _dwReason, LPVOID _pReserved);

#endif

XE_EXPORT_FCT(DWORD, XeTexture_GetTextureFormat)(XeTexture_Format _eFormat);

XE_EXPORT_FCT(BOOL, XeTexture_CompressTexture)(DWORD                 _dwSrcWidth, 
                                               DWORD                 _dwSrcHeight, 
                                               LPVOID                _pSrcImage, 
                                               XeTexture_Descriptor* _pDescriptor);

XE_EXPORT_FCT(BOOL, XeTexture_UncompressTexture)(const LPVOID          _pBuffer,
                                                 DWORD                 _dwBufferSize,
                                                 XeTexture_Descriptor* _pDescriptor);

#endif // #ifdef GUARD_XENONTEXTURE_H
