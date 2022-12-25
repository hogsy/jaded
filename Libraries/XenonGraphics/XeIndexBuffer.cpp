// ------------------------------------------------------------------------------------------------
// File   : XeIndexBuffer.cpp
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
#include "XeIndexBuffer.h"
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
// Name   : XeIndexBuffer::XeIndexBuffer
// Params : indexCount : Number of indices
// RetVal : None
// Descr. : Constructor
// ------------------------------------------------------------------------------------------------
XeIndexBuffer::XeIndexBuffer(unsigned long indexCount)
: m_IB(NULL), m_IndexCount(indexCount), m_FaceCount(indexCount/3), m_RefCount(1)
{
    ERR_X_Assert(m_IndexCount > 0);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeIndexBuffer::~XeIndexBuffer
// Params : None
// RetVal : None
// Descr. : Destructor
// ------------------------------------------------------------------------------------------------
XeIndexBuffer::~XeIndexBuffer(void)
{
    Free();
}

// ------------------------------------------------------------------------------------------------
// Name   : XeIndexBuffer::Init
// Params : copyContent : Copy the system memory buffer into the IB
// RetVal : None
// Descr. : Initialize the buffer
// ------------------------------------------------------------------------------------------------
void XeIndexBuffer::Init(bool copyContent, bool bWriteOnly /* = true */)
{
    HRESULT hr;

    if (m_IB != NULL)
        return;

    DWORD   dwUsage = bWriteOnly ? D3DUSAGE_WRITEONLY : 0;
    D3DPOOL ePool   = (D3DPOOL)0;

#if defined(_XENON_RENDER_PC)
    ePool = D3DPOOL_MANAGED;
#endif

    // Create the index buffer
    hr = g_oXeRenderer.GetDevice()->CreateIndexBuffer(m_IndexCount * sizeof(unsigned short),
                                                      dwUsage,
                                                      D3DFMT_INDEX16,
                                                      ePool,
                                                      &m_IB,
                                                      NULL);
    if (FAILED(hr))
    {
        ERR_X_Assert(0);
        return;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeIndexBuffer::Free
// Params : None
// RetVal : None
// Descr. : Free the current index buffer
// ------------------------------------------------------------------------------------------------
void XeIndexBuffer::Free(void)
{
    if (m_IB != NULL)
    {
        m_IB->Release();
        m_IB = NULL;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeIndexBuffer::Release
// Params : None
// RetVal : None
// Descr. : Decrement the reference count
// ------------------------------------------------------------------------------------------------
long XeIndexBuffer::Release(void)
{
    --m_RefCount;

    if (m_RefCount == 0)
    {
        g_XeBufferMgr.ReleaseIB(this);
        return 0;
    }

    return m_RefCount;
}
