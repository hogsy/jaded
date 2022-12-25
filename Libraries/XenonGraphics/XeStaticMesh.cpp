#include "Precomp.h"
#include "XeStaticMesh.h"
#include "XeBufferMgr.h"
#include "XeVertexShaderManager.h"

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeStaticMesh::XeStaticMesh()
{
    m_pIndexStream         = NULL;
    m_ulVertexFormatHandle = VS_INVALID_HANDLE;
    ClearAllStreams();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeStaticMesh::~XeStaticMesh()
{
    SAFE_RELEASE(m_pIndexStream);
    ClearAllStreams();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeStaticMesh::ClearAllStreams()
{
    for (ULONG ulCurStream = 0; ulCurStream < m_vecVertexStream.size(); ulCurStream++)
    {
        SAFE_RELEASE(m_vecVertexStream[ulCurStream].pBuffer);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeStaticMesh::SetIndices(WORD *_pIndices, ULONG _ulIndexCount)
{
    // release existing buffer
    SAFE_RELEASE(m_pIndexStream);

    // create a new index buffer
    m_pIndexStream = g_XeBufferMgr.CreateIndexBuffer(_ulIndexCount);

    // copy index info
    L_memcpy(m_pIndexStream->GetBuffer(), _pIndices, _ulIndexCount * sizeof(WORD));
    
    // load in video memory
    m_pIndexStream->GetIB();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
UINT XeStaticMesh::AddStream(ULONG _ulComponents, BOOL _bDynamic, LPVOID _pVertexBuffer, ULONG _ulVertexCount)
{
    ERR_X_Assert(m_vecVertexStream.size() < 8);
    ERR_X_Assert(_ulComponents != 0);

    XeVertexStream oNewStream;
    ULONG          ulStride = GetVertexSize(_ulComponents);
    ERR_X_Assert(ulStride != 0);

    oNewStream.ulComponents = _ulComponents;

    // Create a new vertex buffer
    if (_bDynamic)
    {
        oNewStream.pBuffer  = (XeBuffer*) g_XeBufferMgr.CreateDynVertexBuffer();
    }
    else
    {
        oNewStream.pBuffer  = (XeBuffer*) g_XeBufferMgr.CreateVertexBuffer(_ulVertexCount, ulStride);
    }

    if ((_pVertexBuffer != NULL) && (oNewStream.pBuffer != NULL))
    {
        // Copy vertex info
        L_memcpy(oNewStream.pBuffer->Lock(_ulVertexCount, ulStride) , _pVertexBuffer, _ulVertexCount * ulStride);
        oNewStream.pBuffer->Unlock();
    }

    // Add stream to stream vector
    m_vecVertexStream.push_back(oNewStream);

    return (m_vecVertexStream.size() - 1);
}
