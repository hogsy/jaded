// ------------------------------------------------------------------------------------------------
// File   : XeVertexBuffer.cpp
// Date   : 
// Author : Sebastien Comte
// Descr. : 
//
// Ubi Soft Entertainment Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"

#include "XeRenderer.h"
#include "XeVertexBuffer.h"
#include "XeBufferMgr.h"
#include "XeUtils.h"


// ------------------------------------------------------------------------------------------------
// EXTERNAL VARIABLES
// ------------------------------------------------------------------------------------------------
extern XeBufferMgr g_XeBufferMgr;

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : PCBuffer::PCBuffer
// Params : vertexCount  : Number of vertices
//          vertexStride : Vertex stride
// RetVal : None
// Descr. : Constructor
// ------------------------------------------------------------------------------------------------
XeBuffer::XeBuffer( unsigned long vertexCount, unsigned long vertexStride, unsigned long offset)
                   : m_VertexCount(vertexCount), m_VertexStride(vertexStride),
                     m_RefCount(1), m_bVBIsContextual(FALSE)
{
    m_ContextualVB[0] = NULL;
    m_ContextualVB[1] = NULL;
    m_ContextualOffset[0] = 0;
    m_ContextualOffset[1] = 0;
}

// -----------------------------------------------------------------------------------------------
// Name   : PCBuffer::~PCBuffer
// Params : None
// RetVal : None
// Descr. : Destructor
// ------------------------------------------------------------------------------------------------
XeBuffer::~XeBuffer(void)
{

}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexBuffer::XeVertexBuffer
// Params : vertexCount  : Number of vertices
//          vertexStride : Vertex stride
// RetVal : None
// Descr. : Constructor
// ------------------------------------------------------------------------------------------------
XeVertexBuffer::XeVertexBuffer(unsigned long vertexCount, unsigned long vertexStride, unsigned long offset)
                               : XeBuffer(vertexCount, vertexStride, offset)
{

}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexBuffer::~XeVertexBuffer
// Params : None
// RetVal : None
// Descr. : Destructor
// ------------------------------------------------------------------------------------------------
XeVertexBuffer::~XeVertexBuffer(void)
{
    Free(); 
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexBuffer::Init
// Params : _bOwnsBuffer : Not used for static vertex buffers
// RetVal : None
// Descr. : Initialize the buffer
// ------------------------------------------------------------------------------------------------
void XeVertexBuffer::Init(BOOL, bool bWriteOnly)
{
    HRESULT hr;

    if (m_ContextualVB[0] != NULL)
        return;

    DWORD dwUsage = bWriteOnly ? D3DUSAGE_WRITEONLY : 0;
    D3DPOOL ePool = (D3DPOOL)0;

#if defined(_XENON_RENDER_PC)
    ePool = D3DPOOL_MANAGED;

#if defined(_PC_FORCE_DYNAMIC_VB)
    ePool    = D3DPOOL_DEFAULT;
    dwUsage |= D3DUSAGE_DYNAMIC;
#endif

#endif

    hr = g_oXeRenderer.GetDevice()->CreateVertexBuffer(m_VertexCount * m_VertexStride,
                                                       dwUsage,
                                                       0,
                                                       ePool,
                                                       &m_ContextualVB[0],
                                                       NULL);
    if (FAILED(hr))
    {
        ERR_X_Assert(0);
        return;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexBuffer::Init
// Params : _pVB : Shared vertex buffer to use
// RetVal : None
// Descr. : Initialize the buffer
// ------------------------------------------------------------------------------------------------
void XeVertexBuffer::Init(IDirect3DVertexBuffer9* _pVB)
{
    ERR_X_Assert(_pVB != NULL);

    Free();

    m_ContextualVB[0] = _pVB;
    m_ContextualVB[0]->AddRef();
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexBuffer::Free
// Params : None
// RetVal : None
// Descr. : Free the current vertex buffer
// ------------------------------------------------------------------------------------------------
void XeVertexBuffer::Free(void)
{
    // Release the vertex buffer
    if (m_ContextualVB[0] != NULL)
    {
        // Notify the buffer manager that we are not using the buffer anymore
        g_XeBufferMgr.NotifyRelease(m_ContextualVB[0]);

        m_ContextualVB[0]->Release();
        m_ContextualVB[0] = NULL;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexBuffer::Release
// Params : None
// RetVal : None
// Descr. : Decrement the reference count
// ------------------------------------------------------------------------------------------------
long XeVertexBuffer::Release(void)
{
    --m_RefCount;

    if (m_RefCount == 0)
    {
        g_XeBufferMgr.ReleaseVB(this);
        return 0;
    }

    return m_RefCount;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBuffer::Lock
// Params : None
// RetVal : System memory buffer
// Descr. : Get the system memory buffer
// ------------------------------------------------------------------------------------------------
LPVOID XeVertexBuffer::Lock(ULONG _ulVertexCount, ULONG _ulVertexStride, BOOL _bForceLock, BOOL _bContextual)
{
    LPVOID pVertices = NULL;
    HRESULT hr = m_ContextualVB[0]->Lock(m_ContextualOffset[0], _ulVertexCount * _ulVertexStride, (void**)&pVertices, 0);
    ERR_X_Assert(SUCCEEDED(hr) && (pVertices != NULL));

    return pVertices;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBuffer::Unlock
// Params : None
// RetVal : System memory buffer
// Descr. : Get the system memory buffer
// ------------------------------------------------------------------------------------------------
void XeVertexBuffer::Unlock(BOOL _bForceUnLock)
{
    m_ContextualVB[0]->Unlock();
}