//--------------------------------------------------------------------------------------
// CD3DXBlt.cpp
//
// Contains general purpose blitting routines
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <float.h>
#include <math.h>
#include "d3d9.h"      // Note: make sure this is the Xbox 360 flavor of D3D9.h!
#include "xgraphics.h"
#include "d3dx9math.h"
#include "CD3DXBlt.h"


#define DPF(x,y)
#define D3DXASSERT(x)


//----------------------------------------------------------------------------
// Name: F2IBegin(), F2I(), F2IEnd()
// Desc: Fast FLOAT->INT conversion.  F2IBegin sets and F2IEnd restores the
//       FPU rounding mode.  F2I converts a float to an int.  You need to be
//       careful of what other floating point code resides between F2IBegin
//       and F2IEnd:  If something messes with the rounding mode, you could
//       get unpredicted results.
//----------------------------------------------------------------------------
static UINT32 g_ulFPU;

static VOID F2IBegin() 
{
    UINT32 ulFPU, ul;

    __asm 
    {
        fnstcw WORD PTR [ulFPU]        // Get FPU control word
        mov    eax, DWORD PTR [ulFPU]
        or     eax, 0x0C00             // Rounding mode = CLAMP
        mov    DWORD PTR [ul], eax
        fldcw  WORD PTR [ul]           // Set FPU control word
    }

    // Save old FPU control word in thread-local storage
    g_ulFPU = ulFPU;
}


static inline INT F2I(FLOAT f)
{
    volatile INT n;

    __asm 
    {
        fld   f   // Load fload
        fistp n   // Store integer (and pop)
    }

    return n;
}

static VOID F2IEnd() 
{
    // Get old FPU control word from thread-local storage
    UINT32 ulFPU = g_ulFPU;

    __asm
    {
        fldcw WORD PTR [ulFPU]    // Set FPU control word
    }
}




//-----------------------------------------------------------------------------
// Specific codecs
//-----------------------------------------------------------------------------

void CXD3DXBlt::Encode( UINT uRow, D3DXCOLOR* pColors )
{
    D3DXCOLOR* p = (D3DXCOLOR*)( (BYTE*)m_pbDstData + uRow*m_uDstPitch );

    for( UINT i = 0; i < m_uDstWidth; i++ )
    {
        *p++ = *pColors++;
    }
}


void CXD3DXBlt::Decode( UINT uRow, D3DXCOLOR* pColors )
{
    D3DXCOLOR* p    = (D3DXCOLOR*)( (BYTE*)m_pbSrcData + uRow*m_uSrcPitch );
    D3DXCOLOR* pLim = p + m_uSrcWidth;

    for( UINT i = 0; i < m_uSrcWidth; i++ )
    {
        if( ( p->r == m_ColorKey.r ) &&
            ( p->g == m_ColorKey.g ) &&
            ( p->b == m_ColorKey.b ) &&
            ( p->a == m_ColorKey.a ) )
        {
            pColors->r = 0.0f;
            pColors->g = 0.0f;
            pColors->b = 0.0f;
            pColors->a = 0.0f;

            pColors++, p++;
        }
        else
        {
            *pColors++ = *p++;
        }
    }
}




//-----------------------------------------------------------------------------
// CXD3DXBlt
//-----------------------------------------------------------------------------
HRESULT CXD3DXBlt_Blt( VOID* pDstData, DWORD dwDstWidth, DWORD dwDstHeight, 
                       VOID* pSrcData, DWORD dwSrcWidth, DWORD dwSrcHeight,
                       DWORD dwColorKey, DWORD dwFilter )
{
    CXD3DXBlt Blt;
    return Blt.Blt( pDstData, dwDstWidth, dwDstHeight, 
                    pSrcData, dwSrcWidth, dwSrcHeight,
                    dwColorKey, dwFilter );
}


HRESULT CXD3DXBlt::Blt( VOID* pDstData, DWORD dwDstWidth, DWORD dwDstHeight, 
                        VOID* pSrcData, DWORD dwSrcWidth, DWORD dwSrcHeight,
                        DWORD dwColorKey, DWORD dwFilter )
{
    m_dwFilter = dwFilter;

    // Validate filter
    if( dwFilter & (0xffff0000 & ~(IMG_FILTER_MIRROR | IMG_FILTER_DITHER)) )
    {
        DPF(0, "Invalid image filter");
        return E_FAIL;
    }

    // Setup src and dst data
    m_pbSrcData     = (D3DXCOLOR*)pSrcData;
    m_uSrcWidth     = dwSrcWidth;
    m_uSrcHeight    = dwSrcHeight;
    m_uSrcPitch     = dwSrcWidth * sizeof(D3DXCOLOR);
    m_ColorKey      = (D3DXCOLOR)dwColorKey;

    m_pbDstData     = (D3DXCOLOR*)pDstData;
    m_uDstWidth     = dwDstWidth;
    m_uDstHeight    = dwDstHeight;
    m_uDstPitch     = dwDstWidth * sizeof(D3DXCOLOR);

    // Execute blitter
    if( (m_dwFilter & 0xff) == IMG_FILTER_NONE )
        return BltPoint();
    if( (m_dwFilter & 0xff) == IMG_FILTER_POINT )
        return BltPoint();
    if( (m_dwFilter & 0xff) == IMG_FILTER_BOX )
        return BltBox();
    if( (m_dwFilter & 0xff) == IMG_FILTER_LINEAR )
        return BltLinear();
    if( (m_dwFilter & 0xff) == IMG_FILTER_TRIANGLE )
        return BltTriangle();

    DPF(0, "No suitable image filter found");
    return E_FAIL;
}


HRESULT CXD3DXBlt::BltPoint()
{
    D3DXCOLOR *pSrc;
    D3DXCOLOR *pDest;

    if( ((m_dwFilter & 0xff) != IMG_FILTER_POINT) && ((m_dwFilter & 0xff) != IMG_FILTER_NONE) )
        return E_FAIL;

    if(!(pSrc = new D3DXCOLOR[m_uSrcWidth]))
        return E_OUTOFMEMORY;

    if(!(pDest = new D3DXCOLOR[m_uDstWidth]))
    {
        delete []pSrc;
        return E_OUTOFMEMORY;
    }

    UINT uSrcXInc = (m_uSrcWidth  << 16) / m_uDstWidth;
    UINT uSrcYInc = (m_uSrcHeight << 16) / m_uDstHeight;

    {
        UINT uSrcY  = 0;
        UINT uDestY = 0;
        UINT uSrcYLast = (UINT) -1;

        while(uDestY < m_uDstHeight)
        {
            UINT uSrcX  = 0;
            UINT uDestX = 0;

            if((uSrcYLast ^ uSrcY) >> 16)
            {
                Decode(uSrcY >> 16, pSrc);
                uSrcYLast = uSrcY;
            }

            while(uDestX < m_uDstWidth)
            {
                pDest[uDestX] = pSrc[uSrcX >> 16];

                uSrcX += uSrcXInc;
                uDestX++;
            }

            Encode(uDestY, pDest);

            uSrcY += uSrcYInc;
            uDestY++;
        }
    }

    delete [] pSrc;
    delete [] pDest;

    return S_OK;
}


HRESULT CXD3DXBlt::BltBox()
{
    D3DXCOLOR *pxyz, *pxYz, *pXyz, *pXYz;
    D3DXCOLOR *pSrc, *pDest;

    if( !(m_uDstWidth == (m_uSrcWidth >> 1)) && !(1 == m_uDstWidth && 1 == m_uSrcWidth) )
        return E_FAIL;

    if( !(m_uDstHeight == (m_uSrcHeight >> 1)) && !(1 == m_uDstHeight && 1 == m_uSrcHeight) )
        return E_FAIL;

    // Optimized filters
    if( !(m_dwFilter & IMG_FILTER_DITHER) && (m_uSrcWidth >= 2) && (m_uSrcHeight >= 2) )
    {
        D3DXCOLOR* pDest   = m_pbDstData;
        D3DXCOLOR* pSrc    = m_pbSrcData;
        D3DXCOLOR* pSrcLim = (D3DXCOLOR*)( (BYTE*)pSrc + m_uSrcPitch * m_uSrcHeight);

        while( pSrc < pSrcLim )
        {
            D3DXCOLOR* p     = pDest;
            D3DXCOLOR* pA    = pSrc;
            D3DXCOLOR* pB    = (D3DXCOLOR*)( (BYTE*)pSrc + m_uSrcPitch );
            D3DXCOLOR* pALim = pA + m_uSrcWidth;

            while( pA < pALim )
            {
                D3DXCOLOR* p00 = pA++;
                D3DXCOLOR* p01 = pA++;
                D3DXCOLOR* p10 = pB++;
                D3DXCOLOR* p11 = pB++;

                p->r = ( p00->r + p01->r + p10->r + p11->r ) / 4.0f;
                p->g = ( p00->g + p01->g + p10->g + p11->g ) / 4.0f;
                p->b = ( p00->b + p01->b + p10->b + p11->b ) / 4.0f;
                p->a = ( p00->a + p01->a + p10->a + p11->a ) / 4.0f;
                p++;
            }

            pDest = (D3DXCOLOR*)( (BYTE*)pDest + m_uDstPitch );
            pSrc  = (D3DXCOLOR*)( (BYTE*)pSrc + m_uSrcPitch + m_uSrcPitch );
        }

        return S_OK;
    }

    // Generic filter
    if( !(pDest = new D3DXCOLOR[m_uDstWidth]) )
        return E_OUTOFMEMORY;

    if( 1 == m_uSrcHeight )
    {
        pSrc = new D3DXCOLOR[m_uSrcWidth];
        pxyz = pSrc;
        pxYz = pxyz;
    }
    else
    {
        pSrc = new D3DXCOLOR[m_uSrcWidth * 2];
        pxyz = pSrc + m_uSrcWidth * 0;
        pxYz = pSrc + m_uSrcWidth * 1;
    }

    if( !pSrc )
    {
        delete[] pDest;
        return E_OUTOFMEMORY;
    }

    if( 1 == m_uSrcWidth )
    {
        pXyz = pxyz;
        pXYz = pxYz;
    }
    else
    {
        pXyz = pxyz + 1;
        pXYz = pxYz + 1;
    }

    for( UINT uY = 0; uY < m_uDstHeight; uY++ )
    {
        UINT uY2 = uY << 1;

        Decode(uY2 + 0, pxyz);

        if( pxYz != pxyz )
            Decode(uY2 + 1, pxYz);

        for( UINT uX = 0; uX < m_uDstWidth; uX++ )
        {
            UINT uX2 = uX << 1;
            pDest[uX] = (pxyz[uX2] + pXyz[uX2] + pxYz[uX2] + pXYz[uX2]) * 0.25f;
        }

        Encode( uY, pDest );
    }

    delete[] pSrc;
    delete[] pDest;

    return S_OK;
}


//---------------------------------------------------------------------------
// Name: LinearFilter
// Desc: This section of the code defines a bunch of structures, macros, and
//       functions which are used for the linear filter.
//---------------------------------------------------------------------------
namespace LinearFilter
{
    struct FROM
    {
        UINT  uFrom;
        FLOAT fWeight;
    };

    struct TO
    {
        FROM pFrom[2];
    };

    static BYTE* Setup( UINT uSrcLim, UINT uDstLim, BOOL bRepeat )
    {
        TO*   pTo;
        BYTE* pbFilter;
        
        if(!(pTo = new TO[uDstLim]))
            return NULL;

        pbFilter = (BYTE*)pTo;
        float fScale = (float) uSrcLim / (float) uDstLim;

        F2IBegin();

        for(UINT u = 0; u < uDstLim; u++)
        {
            float fSrc = (float) u * fScale - 0.5f;
            float fSrcFloor = floorf(fSrc);

            int iSrcA = F2I(fSrcFloor);
            int iSrcB = iSrcA + 1;

            if(iSrcA < 0)
                iSrcA = bRepeat ? uSrcLim - 1 : 0;

            if((UINT) iSrcB >= uSrcLim)
                iSrcB = bRepeat ? 0 : uSrcLim - 1;

            pTo->pFrom[0].uFrom   = (UINT) iSrcA;
            pTo->pFrom[0].fWeight = 1.0f - (fSrc - fSrcFloor);

            pTo->pFrom[1].uFrom   = (UINT) iSrcB;
            pTo->pFrom[1].fWeight = 1.0f - pTo->pFrom[0].fWeight;

            pTo++;
        }

        F2IEnd();

        return pbFilter;
    }
};

HRESULT CXD3DXBlt::BltLinear()
{
    HRESULT hr;

    // Create linear filters
    BOOL bRepeatX = !(m_dwFilter & IMG_FILTER_MIRROR_U);
    BOOL bRepeatY = !(m_dwFilter & IMG_FILTER_MIRROR_V);

    BYTE* pbXFilter = LinearFilter::Setup( m_uSrcWidth,  m_uDstWidth,  bRepeatX );
    BYTE* pbYFilter = LinearFilter::Setup( m_uSrcHeight, m_uDstHeight, bRepeatY );

    if( !pbXFilter || !pbYFilter )
        goto LOutOfMemory;

    // Create temp color space
    D3DXCOLOR* pDest;
    if( !(pDest = new D3DXCOLOR[m_uDstWidth]) )
        goto LOutOfMemory;

    D3DXCOLOR* pSrc;
    if( !(pSrc = new D3DXCOLOR[m_uSrcWidth * 2]) )
        goto LOutOfMemory;

    D3DXCOLOR* pxyz = pSrc + m_uSrcWidth * 0;
    D3DXCOLOR* pxYz = pSrc + m_uSrcWidth * 1;

    UINT uY = 0;
    LinearFilter::TO* pToY = (LinearFilter::TO*) pbYFilter;

    UINT uFrom0 = (UINT) -1;
    UINT uFrom1 = (UINT) -1;

    while(uY < m_uDstHeight)
    {
        UINT uX = 0;
        LinearFilter::TO* pToX = (LinearFilter::TO*)pbXFilter;

        if( pToY->pFrom[0].uFrom != uFrom0 )
        {
            if( pToY->pFrom[0].uFrom != uFrom1 )
            {
                uFrom0 = pToY->pFrom[0].uFrom;
                Decode(uFrom0, pxyz);
            }
            else
            {
                uFrom0 = uFrom1;
                uFrom1 = (UINT) -1;

                D3DXCOLOR* pSrcT = pxyz; pxyz = pxYz; pxYz = pSrcT;
            }
        }

        if( pToY->pFrom[1].uFrom != uFrom1 )
        {
            uFrom1 = pToY->pFrom[1].uFrom;
            Decode(uFrom1, pxYz);
        }

        while( uX < m_uDstWidth )
        {
            pDest[uX] = ((pxyz[pToX->pFrom[0].uFrom] * pToX->pFrom[0].fWeight + 
                          pxyz[pToX->pFrom[1].uFrom] * pToX->pFrom[1].fWeight) * pToY->pFrom[0].fWeight +
                         (pxYz[pToX->pFrom[0].uFrom] * pToX->pFrom[0].fWeight + 
                          pxYz[pToX->pFrom[1].uFrom] * pToX->pFrom[1].fWeight) * pToY->pFrom[1].fWeight);

            pToX++; uX++;
        }

        Encode(uY, pDest);

        pToY++; uY++;
    }

    hr = S_OK;
    goto LDone;

LOutOfMemory:
    hr = E_OUTOFMEMORY;
    goto LDone;

LDone:
    delete [] pbXFilter;
    delete [] pbYFilter;

    delete [] pDest;
    delete [] pSrc;

    return hr;
}


//---------------------------------------------------------------------------
// Name: TriangleFilter
// Desc: This section of the code defines a bunch of structures, macros, and
//       functions which are used for the triangle filter.
//---------------------------------------------------------------------------
namespace TriangleFilter
{
    #define TF_EPSILON 0.00001f

    struct TO
    {
        UINT  uTo;         // mapping
        FLOAT fWeight;     // mapping weight
    };

    struct FROM
    {
        UINT uSize;        // size, in bytes
        TO   pTo[1];       // may continue past 1
    };

    struct FILTER
    {
        UINT uSize;        // size, in bytes
        FROM pFrom[1];     // may continue past 1
    };

    struct ROW
    {
        D3DXCOLOR*  pclr;    // row data
        FLOAT      fWeight;
        ROW*       pNext;

        ROW()
        {
            pclr  = NULL;
            pNext = NULL;
        }

        ~ROW() 
        {
            if( pclr )
                delete [] pclr;
            pclr = NULL;

            if( pNext )
                delete pNext;
            pNext = NULL;
        }
    };

    static const UINT FILTER_SIZE = sizeof(FILTER) - sizeof(FROM);
    static const UINT FROM_SIZE   = sizeof(FROM) - sizeof(TO);
    static const UINT TO_SIZE     = sizeof(TO);

    // Setup a triangle filter.  Each source pixel contriubtes equally to the
    // destination image.
    static BYTE* Setup( UINT uSrcLim, UINT uDstLim, BOOL bRepeat )
    {
        if( !uSrcLim || !uDstLim )
            return NULL;

        // Compute scale
        FLOAT fScale     = (FLOAT) uDstLim / (FLOAT) uSrcLim;
        FLOAT f2ScaleInv = 0.5f / fScale;

        // Allocate memory needed to describe the filter.
        UINT uSizeMax = FILTER_SIZE + FROM_SIZE + TO_SIZE;
        UINT uRepeat  = bRepeat ? 1 : 0;

        for( UINT uSrc = 0; uSrc < uSrcLim; uSrc++ )
        {
            FLOAT fSrc    = (FLOAT) uSrc - 0.5f;
            FLOAT fDstMin = fSrc * fScale;
            FLOAT fDstLim = fDstMin + fScale;

            uSizeMax += FROM_SIZE + TO_SIZE + (UINT)(fDstLim - fDstMin + uRepeat + 1) * TO_SIZE * 2;
        }

        BYTE* pbFilter = new UINT8[uSizeMax];
        if( !pbFilter )
            return NULL;

        // Initialize filter
        UINT uSize = FILTER_SIZE;

        // Initialize accumulators
        UINT  uAccumDst = 0;
        FLOAT fAccumWeight = 0.0f;

        for( uSrc = 0; uSrc < uSrcLim; uSrc++ )
        {
            // Initialize FROM block
            UINT uSizeFrom = uSize;
            FROM* pFrom = (FROM*)(pbFilter + uSize);
            uSize += FROM_SIZE;
            D3DXASSERT(uSize <= uSizeMax);

            // For each source pixel, we perform two passes.  During the first
            // pass, we record the influences due to this source pixel as we
            // approach it.  During the second pass, we record the influences
            // as we move away from it.
            for( UINT uPass = 0; uPass < 2; uPass++ )
            {
                FLOAT fSrc = ((FLOAT) uSrc + uPass) - 0.5f;

                // Start and end of source pixel, in destination coordinates
                FLOAT fDstMin = fSrc * fScale;
                FLOAT fDstLim = fDstMin + fScale;

                // Clamp source pixel to destination image.
                if( !bRepeat )
                {
                    if( fDstMin < 0.0f )
                        fDstMin = 0.0f;
                    if( fDstLim > (FLOAT)uDstLim )
                        fDstLim = (FLOAT)uDstLim;
                }

                // Start of first destination pixel
                INT nDst = (INT)floorf( fDstMin );

                while( (FLOAT)nDst < fDstLim )
                {
                    FLOAT fDst0 = (FLOAT)nDst;
                    FLOAT fDst1 = fDst0 + 1.0f;

                    UINT uDst;
                    if( nDst < 0 )
                        uDst = (UINT)nDst + uDstLim;
                    else if(nDst >= (INT) uDstLim)
                        uDst = (UINT)nDst - uDstLim;
                    else
                        uDst = (UINT)nDst;

                    // Save the accumulated weight from uAccumDst if we have moved
                    // on to accumulating for the next pixel.
                    if( uDst != uAccumDst )
                    {
                        if( fAccumWeight > TF_EPSILON )
                        {
                            TO* pTo  = (TO*)(pbFilter + uSize);
                            uSize += TO_SIZE;

                            D3DXASSERT(uSize <= uSizeMax);

                            pTo->uTo = uAccumDst;
                            pTo->fWeight = fAccumWeight;
                        }

                        fAccumWeight = 0.0f;
                        uAccumDst    = uDst;
                    }

                    // Clip destination pixel to source pixel.
                    if( fDst0 < fDstMin )
                        fDst0 = fDstMin;
                    if( fDst1 > fDstLim )
                        fDst1 = fDstLim;

                    // Calculate average weight over destination pixel.  Since the
                    // weight varies linearly, when the average weight is
                    // multiplied by the size of the destination pixel, the result
                    // is equal to the integral of the weight over this portion of
                    // the pixel.

                    FLOAT fWeight;
                    if( !bRepeat && fSrc < 0.0f )
                        fWeight = 1.0f;
                    else if( !bRepeat && fSrc + 1.0f >= (FLOAT)uSrcLim )
                        fWeight = 0.0f;
                    else
                        fWeight = (fDst0 + fDst1) * f2ScaleInv - fSrc;

                    fAccumWeight += (fDst1 - fDst0) * (uPass ? 1.0f - fWeight : fWeight);
                    nDst++;
                }
            }

            // If there is a valid value still in the accumulator, it needs to be
            // written out.

            if( fAccumWeight > TF_EPSILON )
            {
                TO* pTo = (TO*)( pbFilter + uSize );
                uSize += TO_SIZE;

                D3DXASSERT(uSize <= uSizeMax);

                pTo->uTo     = uAccumDst;
                pTo->fWeight = fAccumWeight;
            }

            fAccumWeight = 0.0f;

            // Record FROM's size
            pFrom->uSize = uSize - uSizeFrom;
        }

        // Record FILTER's size
        ((FILTER*)pbFilter)->uSize = uSize;

        return pbFilter;
    }
};

HRESULT CXD3DXBlt::BltTriangle()
{
    HRESULT hr;
    BYTE *pbXFilter, *pbYFilter;
    TriangleFilter::FROM *pXFrom, *pYFrom;
    TriangleFilter::TO *pXTo, *pYTo, *pXToLim, *pYToLim;
    TriangleFilter::ROW *pRow, **ppRowActive, *pRowFree;
    UINT uRowsActive;
    UINT uSrcRow;
    D3DXCOLOR *pclrSrc;
    D3DXCOLOR *pclrPal;

    pbXFilter   = NULL;
    pbYFilter   = NULL;
    ppRowActive = NULL;
    pRowFree    = NULL;
    pclrSrc     = NULL;
    pclrPal     = NULL;

    // Create filters for each axis
    BOOL bRepeatX = !(m_dwFilter & IMG_FILTER_MIRROR_U);
    BOOL bRepeatY = !(m_dwFilter & IMG_FILTER_MIRROR_V);

    if( !( pbXFilter = TriangleFilter::Setup( m_uSrcWidth,  m_uDstWidth,  bRepeatX ) ) ||
        !( pbYFilter = TriangleFilter::Setup( m_uSrcHeight, m_uDstHeight, bRepeatY ) ) )
    {
        hr = E_FAIL;
        goto LDone;
    }

    TriangleFilter::FILTER* pXFilter = (TriangleFilter::FILTER*)pbXFilter;
    TriangleFilter::FILTER* pYFilter = (TriangleFilter::FILTER*)pbYFilter;

    TriangleFilter::FROM*   pXFromLim = (TriangleFilter::FROM*)((BYTE*) pXFilter + pXFilter->uSize);
    TriangleFilter::FROM*   pYFromLim = (TriangleFilter::FROM*)((BYTE*) pYFilter + pYFilter->uSize);

    // Initialize accumulation rows
    if(!(ppRowActive = new TriangleFilter::ROW* [m_uDstHeight]))
        goto LOutOfMemory;

    memset(ppRowActive, 0, m_uDstHeight * sizeof(TriangleFilter::ROW *));
    uRowsActive = 0;

    if(!(pclrSrc = new D3DXCOLOR[m_uSrcWidth]))
        goto LOutOfMemory;

    // Filter image
    uSrcRow = 0;

    for(pYFrom = pYFilter->pFrom; pYFrom < pYFromLim; pYFrom = (TriangleFilter::FROM *) pYToLim)
    {
        D3DXCOLOR *pclrSrcX;

        pYToLim = (TriangleFilter::TO *) ((BYTE*) pYFrom + pYFrom->uSize);

        if(pYFrom->pTo < pYToLim)
        {
            // Create necessary accumulation rows
            for(pYTo = pYFrom->pTo; pYTo < pYToLim; pYTo++)
            {
                if(!(pRow = ppRowActive[pYTo->uTo]))
                {
                    if(pRowFree)
                    {
                        // Reuse a row
                        pRow = pRowFree;
                        pRowFree = pRow->pNext;
                    }
                    else
                    {
                        // Allocate a new row
                        if(!(pRow = new TriangleFilter::ROW))
                            goto LOutOfMemory;

                        if(!(pRow->pclr = new D3DXCOLOR[m_uDstWidth]))
                        {
                            delete pRow;
                            goto LOutOfMemory;
                        }
                    }

                    memset(pRow->pclr, 0, m_uDstWidth * sizeof(D3DXCOLOR));
                    pRow->fWeight = 0.0f;
                    pRow->pNext = NULL;

                    ppRowActive[pYTo->uTo] = pRow;
                    uRowsActive++;
                }
            }

            // Read source pixels
            Decode(uSrcRow, pclrSrc);

            // Process a row from the source image
            pclrSrcX = pclrSrc;
            for(pXFrom = pXFilter->pFrom; pXFrom < pXFromLim; pXFrom = (TriangleFilter::FROM *) pXToLim)
            {
                pXToLim = (TriangleFilter::TO *) ((BYTE*) pXFrom + pXFrom->uSize);

                for(pYTo = pYFrom->pTo; pYTo < pYToLim; pYTo++)
                {
                    pRow = ppRowActive[pYTo->uTo];

                    for(pXTo = pXFrom->pTo; pXTo < pXToLim; pXTo++)
                    {
                        FLOAT fWeight =  pYTo->fWeight * pXTo->fWeight;

                        pRow->pclr[pXTo->uTo].r += pclrSrcX->r * fWeight;
                        pRow->pclr[pXTo->uTo].g += pclrSrcX->g * fWeight;
                        pRow->pclr[pXTo->uTo].b += pclrSrcX->b * fWeight;
                        pRow->pclr[pXTo->uTo].a += pclrSrcX->a * fWeight;
                    }
                }

                pclrSrcX++;
            }

            // Write completed accumulation rows
            for(pYTo = pYFrom->pTo; pYTo < pYToLim; pYTo++)
            {
                pRow = ppRowActive[pYTo->uTo];
                pRow->fWeight += pYTo->fWeight;

                if(pRow->fWeight + TF_EPSILON >= 1.0f)
                {
                    // Write and free row
                    Encode(pYTo->uTo, pRow->pclr);

                    ppRowActive[pYTo->uTo] = NULL;
                    pRow->pNext = pRowFree;
                    pRowFree = pRow;

                    uRowsActive--;
                }
            }
        }

        uSrcRow++;
    }

    // Make sure that all accumulation rows have been written out.  This should
    // only happen if the filter was not constructed correctly.
    if(uRowsActive)
    {
        for(UINT uRow = 0; uRow < m_uDstHeight; uRow++)
        {
            if(ppRowActive[uRow])
            {
                // Write and delete row
                Encode(uRow, ppRowActive[uRow]->pclr);
                delete ppRowActive[uRow];

                if(--uRowsActive == 0)
                    break;
            }
        }
    }

    hr = S_OK;
    goto LDone;

LOutOfMemory:
    hr = E_OUTOFMEMORY;
    goto LDone;

LDone:
    // Clean up
    delete[] ppRowActive;
    delete pRowFree;
    delete [] pbYFilter;
    delete [] pbXFilter;
    delete [] pclrSrc;
    delete [] pclrPal;
    return hr;
}


