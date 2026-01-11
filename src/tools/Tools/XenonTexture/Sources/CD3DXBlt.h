//--------------------------------------------------------------------------------------
// CD3DXBlt.h
//
// Contains general purpose blitting routines
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once


//--------------------------------------------------------------------------------------
// Filter flags for resizing images
//--------------------------------------------------------------------------------------
#define IMG_FILTER_DEFAULT         ((UINT) -1)
#define IMG_FILTER_NONE            (1 << 0)
#define IMG_FILTER_POINT           (2 << 0)
#define IMG_FILTER_LINEAR          (3 << 0)
#define IMG_FILTER_TRIANGLE        (4 << 0)
#define IMG_FILTER_BOX             (5 << 0)

#define IMG_FILTER_MIRROR_U        (1 << 16)
#define IMG_FILTER_MIRROR_V        (2 << 16)
#define IMG_FILTER_MIRROR_W        (4 << 16)
#define IMG_FILTER_MIRROR          (7 << 16)
#define IMG_FILTER_DITHER          (8 << 16)


//--------------------------------------------------------------------------------------
// Name: Class CXD3DXBlt
// Desc: A blitter used for resizing
//--------------------------------------------------------------------------------------
class CXD3DXBlt
{
    // Generic filters
    HRESULT BltPoint();
    HRESULT BltBox();
    HRESULT BltLinear();
    HRESULT BltTriangle();

    DWORD m_dwFilter;

    // Src and dst data
    D3DXCOLOR* m_pbSrcData;
    UINT       m_uSrcWidth;
    UINT       m_uSrcHeight;
    UINT       m_uSrcPitch;
    D3DXCOLOR  m_ColorKey;

    D3DXCOLOR* m_pbDstData;
    UINT       m_uDstWidth;
    UINT       m_uDstHeight;
    UINT       m_uDstPitch;

    void Decode( UINT uRow, D3DXCOLOR* pColors );
    void Encode( UINT uRow, D3DXCOLOR* pColors );

public:
    HRESULT Blt( VOID* pDstData, DWORD dwDstWidth, DWORD dwDstHeight, 
                 VOID* pSrcData, DWORD dwSrcWidth, DWORD dwSrcHeight,
                 DWORD dwColorKey, DWORD dwFilter );
};

