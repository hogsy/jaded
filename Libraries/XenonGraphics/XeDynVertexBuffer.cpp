// ------------------------------------------------------------------------------------------------
// File   : XeDynVertexBuffer.cpp
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

#include "XeBufferMgr.h"
#include "XeDynVertexBuffer.h"
#include "XeRenderer.h"


// ------------------------------------------------------------------------------------------------
// EXTERNAL VARIABLES
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : XeDynVertexBuffer::XeDynVertexBuffer
// Params : vertexCount  : Number of vertices
//          vertexStride : Vertex stride
// RetVal : None
// Descr. : Constructor
// ------------------------------------------------------------------------------------------------
XeDynVertexBuffer::XeDynVertexBuffer()
: XeBuffer(0, 0, 0),
m_CurrentOffset(0), m_LockCount(0), m_bFullLock(FALSE), m_pLastLockAddress(NULL), m_bOwnsBuffer(FALSE)
{
}

// ------------------------------------------------------------------------------------------------
// Name   : XeDynVertexBuffer::~XeDynVertexBuffer
// Params : None
// RetVal : None
// Descr. : Destructor
// ------------------------------------------------------------------------------------------------
XeDynVertexBuffer::~XeDynVertexBuffer(void)
{
    Free();
}

// ------------------------------------------------------------------------------------------------
// Name   : XeDynVertexBuffer::Init
// Params : _bOwnsBuffer : Owns the vertex buffer
// RetVal : None
// Descr. : Initialize the buffer
// ------------------------------------------------------------------------------------------------
void XeDynVertexBuffer::Init(BOOL _bOwnsBuffer, bool)
{
    m_bOwnsBuffer   = _bOwnsBuffer;
    m_CurrentOffset = 0;
    m_LockCount     = 0;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeDynVertexBuffer::Free
// Params : None
// RetVal : None
// Descr. : Free the current vertex buffer
// ------------------------------------------------------------------------------------------------
void XeDynVertexBuffer::Free(void)
{
    Unlock();

    // Release the vertex buffer
    if (m_bOwnsBuffer)
    {
        SAFE_RELEASE(m_ContextualVB[0]);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeDynVertexBuffer::Release
// Params : None
// RetVal : None
// Descr. : Decrement the reference count
// ------------------------------------------------------------------------------------------------
long XeDynVertexBuffer::Release(void)
{
    --m_RefCount;

    if (m_RefCount == 0)
    {
        g_XeBufferMgr.ReleaseDynVB(this);
        return 0;
    }

    return m_RefCount;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeDynVertexBuffer::Lock
// Params : vertexCount : Number of vertices
// RetVal : Buffer
// Descr. : Get the system memory buffer
// ------------------------------------------------------------------------------------------------
LPVOID XeDynVertexBuffer::Lock(ULONG _ulVertexCount, ULONG _ulVertexStride, BOOL _bForceLock, BOOL _bContextual)
{
    ERR_X_Assert(m_LockCount == 0);
    ERR_X_Assert(!m_bOwnsBuffer);

    m_VertexCount   = _ulVertexCount;
    m_VertexStride  = _ulVertexStride;
    m_LockCount     = 1;

    m_bVBIsContextual = _bContextual;

    if(_bContextual)
        return g_XeBufferMgr.LockDynVertexBuffer(_ulVertexCount * _ulVertexStride, &m_ContextualVB[(g_XeBufferMgr.GetVBContext()+1)%2], &m_ContextualOffset[(g_XeBufferMgr.GetVBContext()+1)%2], _bForceLock);
    else
        return g_XeBufferMgr.LockDynVertexBuffer(_ulVertexCount * _ulVertexStride, &m_ContextualVB[0], &m_ContextualOffset[0], _bForceLock);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeDynVertexBuffer::Lock
// Params : vertexCount : Number of vertices
// RetVal : Buffer
// Descr. : Get the system memory buffer
// ------------------------------------------------------------------------------------------------
void XeDynVertexBuffer::Unlock(BOOL _bForceUnLock)
{
    IDirect3DVertexBuffer9* pVB = m_bVBIsContextual ? m_ContextualVB[(g_XeBufferMgr.GetVBContext()+1)%2] : m_ContextualVB[0];

    if ((pVB != NULL) && (m_LockCount > 0))
    {
        if (m_bOwnsBuffer || _bForceUnLock)
        {
            pVB->Unlock();
        }
        m_pLastLockAddress = NULL;
    }

    m_LockCount = 0;
}