// ------------------------------------------------------------------------------------------------
// File   : XeSurface.cpp
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

#include "d3d9.h"
#include "d3dx9.h"

#include "XeSurface.h"
#include "XenonTexture.h"
#include "CD3DXBlt.h"

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

XeSurface::XeSurface(BYTE* _pbySurface, DWORD _dwWidth, DWORD _dwHeight)
{
    DWORD* pdwSurface = (DWORD*)_pbySurface;

    m_pRawSurface = new D3DXCOLOR [_dwWidth * _dwHeight];

    for (DWORD i = 0; i < _dwWidth * _dwHeight; ++i)
    {
        m_pRawSurface[i] = *pdwSurface;

        ++pdwSurface;
    }

    m_dwWidth  = _dwWidth;
    m_dwHeight = _dwHeight;
}

XeSurface::~XeSurface(void)
{
    delete [] m_pRawSurface;
}

void XeSurface::Flip(void)
{
    static D3DXCOLOR s_astColors[4096];

    DWORD i;
    DWORD dwFlipRow = m_dwHeight - 1;

    for (i = 0; i < (m_dwHeight >> 1); ++i, --dwFlipRow)
    {
        memcpy(s_astColors,                           &m_pRawSurface[i * m_dwWidth],         m_dwWidth * sizeof(D3DXCOLOR));
        memcpy(&m_pRawSurface[i * m_dwWidth],         &m_pRawSurface[dwFlipRow * m_dwWidth], m_dwWidth * sizeof(D3DXCOLOR));
        memcpy(&m_pRawSurface[dwFlipRow * m_dwWidth], s_astColors,                           m_dwWidth * sizeof(D3DXCOLOR));
    }
}

void XeSurface::Normalize(void)
{
    DWORD dwNbPixels = m_dwWidth * m_dwHeight;
    DWORD i;

    for (i = 0; i < dwNbPixels; ++i)
    {
        D3DXVECTOR3 vTempVec((m_pRawSurface[i].r * 2.0f) - 1.0f, 
                             (m_pRawSurface[i].g * 2.0f) - 1.0f, 
                             (m_pRawSurface[i].b * 2.0f) - 1.0f);

        D3DXVec3Normalize(&vTempVec, &vTempVec);

        m_pRawSurface[i].r = (vTempVec.x * 0.5f) + 0.5f;
        m_pRawSurface[i].g = (vTempVec.y * 0.5f) + 0.5f;
        m_pRawSurface[i].b = (vTempVec.z * 0.5f) + 0.5f;
    }
}

void XeSurface::Resize(DWORD _dwWidth, DWORD _dwHeight)
{
    if ((m_dwWidth == _dwWidth) && (m_dwHeight == _dwHeight))
        return;

    D3DXCOLOR* pNewData = new D3DXCOLOR [_dwWidth * _dwHeight];

    CXD3DXBlt stBlitter;
    HRESULT   hr;

    hr = stBlitter.Blt(pNewData, _dwWidth, _dwHeight, m_pRawSurface, m_dwWidth, m_dwHeight, 
                       0, IMG_FILTER_TRIANGLE | IMG_FILTER_DITHER);
    XE_ASSERT(SUCCEEDED(hr));

    delete [] m_pRawSurface;

    m_pRawSurface = pNewData;
    m_dwWidth     = _dwWidth;
    m_dwHeight    = _dwHeight;
}
