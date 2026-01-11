#include "Precomp.h"
#include "XeUtils.h"
#include "XeVertexShaderManager.h"
#include "XeMesh.h"
#include "XeBufferMgr.h"
#include "XeContextManager.h"
#include "XeRenderStateManager.h"

// ------------------------------------------------------------------------------------------------
// Name   : XeMesh()
// Params : 
// RetVal : 
// Descr. : Constructor
// ------------------------------------------------------------------------------------------------
XeMesh::XeMesh()
{
    m_bDynamic = FALSE;
    m_pIndexStream = NULL;
    m_ulVertexFormatHandle = VS_INVALID_HANDLE;
    m_iMaxWeight = 0;
    m_iMaxBones = 0;
    ClearAllStreams();

    m_bHasBoundingVolume = FALSE;
    m_bUseLightSet = FALSE;
    MATH_InitVector( &m_vAABVMin, 0.0f, 0.0f, 0.0f );
    MATH_InitVector( &m_vAABVMax, 0.0f, 0.0f, 0.0f );
    MATH_InitVector( &m_vAABVCenter, 0.0f, 0.0f, 0.0f );
    m_fBoundingVolumeRadius = 0.0f;

    m_pUserData = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : ~XeMesh()
// Params : 

// RetVal : 
// Descr. : Destructor
// ------------------------------------------------------------------------------------------------
XeMesh::~XeMesh()
{
    ReInit();
}

// ------------------------------------------------------------------------------------------------
// Name   : ClearAllStreams()
// Params : 
// RetVal : 
// Descr. : Release all streams
// ------------------------------------------------------------------------------------------------
void XeMesh::ClearAllStreams()
{
    if(!m_bDynamic)
    {
        for (ULONG ulCurStream = 0; ulCurStream < m_vecVertexStream.size(); ulCurStream++)
        {
            SAFE_RELEASE(m_vecVertexStream[ulCurStream].pBuffer);
        }

        m_vecVertexStream.resize(0);
        m_ulVertexFormatHandle = VS_INVALID_HANDLE;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : SetIndices
// Params : _pIndices -> system pointer to index buffer
//          _ulIndexCount -> Count of index to copy
// RetVal : 
// Descr. : Set a new index stream releasing any existing index stream
// ------------------------------------------------------------------------------------------------
void XeMesh::SetIndices(WORD *_pIndices, ULONG _ulIndexCount)
{
    // release existing buffer
    SAFE_RELEASE(m_pIndexStream);

    // create a new index buffer
    m_pIndexStream = g_XeBufferMgr.CreateIndexBuffer(_ulIndexCount);

    // copy index info
    LPVOID pIndices = m_pIndexStream->Lock(_ulIndexCount);
    L_memcpy(pIndices, _pIndices, _ulIndexCount * sizeof(WORD));
    m_pIndexStream->Unlock();
}

// ------------------------------------------------------------------------------------------------
// Name   : AddStream
// Params : _ulComponents -> Ored values of all the vertex component contained in the given stream
//          _bDynamic -> Is the stream dynamic or not
//          _pVertexBuffer -> system pointer to array of vertices to copy
//          _ulVertexCount -> count of vertices to copy
// RetVal : zero-based index of the new stream within the XeMesh object
// Descr. : Add and copy a new vertex stream to a XeMesh object.
// ------------------------------------------------------------------------------------------------
UINT XeMesh::AddStream(ULONG _ulComponents, BOOL _bDynamic, LPVOID _pVertexBuffer, ULONG _ulVertexCount)
{
    ERR_X_Assert(m_vecVertexStream.size() < XeRenderStateManager::RSMGR_MAX_STREAM);
    ERR_X_Assert(_ulComponents != 0);

    //make sure the components are not already present
    for (ULONG ulCurStream = 0; ulCurStream < m_vecVertexStream.size(); ulCurStream++)
    {
        if (((m_vecVertexStream[ulCurStream].ulComponents & _ulComponents) & ~XEVC_COMPESSION_MASK) != 0)
        {
            // component overlapping
            ERR_X_Assert(FALSE);
            return -1;
        }
    }

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
    m_ulVertexFormatHandle = VS_INVALID_HANDLE;

    return (m_vecVertexStream.size() - 1);
}

// ------------------------------------------------------------------------------------------------
// Name   : AddStream
// Params : _ulComponents -> Ored values of all the vertex component contained in the given stream
//          _poBuffer -> Existing buffer pointer to add as a new stream
// RetVal : zero-based index of the new stream within the XeMesh object
// Descr. : Add a new vertex stream to a XeMesh object.
// ------------------------------------------------------------------------------------------------
UINT XeMesh::AddStream(ULONG _ulComponents, XeBuffer *_poBuffer)
{
    ERR_X_Assert(m_vecVertexStream.size() < XeRenderStateManager::RSMGR_MAX_STREAM);
    ERR_X_Assert(_ulComponents != 0);

    //make sure the components are not already present
    for (ULONG ulCurStream = 0; ulCurStream < m_vecVertexStream.size(); ulCurStream++)
    {
        if (((m_vecVertexStream[ulCurStream].ulComponents & _ulComponents) & ~XEVC_COMPESSION_MASK) != 0)
        {
            // component overlapping
            ERR_X_Assert(FALSE);
            return -1;
        }
    }

    XeVertexStream oNewStream;
    ULONG          ulStride = GetVertexSize(_ulComponents);

    // save stream info and adjust ref count
    oNewStream.ulComponents = _ulComponents;
    oNewStream.pBuffer      = _poBuffer;
    _poBuffer->AddRef();

    // Add stream to stream vector
    m_vecVertexStream.push_back(oNewStream);
    m_ulVertexFormatHandle = VS_INVALID_HANDLE;

    return (m_vecVertexStream.size() - 1);
}

// ------------------------------------------------------------------------------------------------
// Name   : SetAxisAlignedBoundingVolume
// ------------------------------------------------------------------------------------------------
void XeMesh::SetAxisAlignedBoundingVolume( const MATH_tdst_Vector & _vAABVMin, const MATH_tdst_Vector & _vAABVMax )
{
    m_vAABVMin = _vAABVMin;
    m_vAABVMax = _vAABVMax; 

    MATH_AddVector( &m_vAABVCenter, &m_vAABVMin, &m_vAABVMax );
    MATH_MulEqualVector( &m_vAABVCenter, 0.5f );

    MATH_tdst_Vector vRadius;
    MATH_SubVector( &vRadius, &m_vAABVMax, &m_vAABVCenter );
    m_fBoundingVolumeRadius = MATH_f_NormVector( &vRadius );

    m_bHasBoundingVolume = TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : GetAxisAlignedBoundingVolume
// ------------------------------------------------------------------------------------------------
void XeMesh::GetAxisAlignedBoundingVolume( MATH_tdst_Vector & _vAABVMin, MATH_tdst_Vector & _vAABVMax )
{
    ERR_X_Assert( m_bHasBoundingVolume );
   _vAABVMin = m_vAABVMin;
   _vAABVMax = m_vAABVMax;
}

// ------------------------------------------------------------------------------------------------
// Name   : GetBoundingVolumeCenter
// ------------------------------------------------------------------------------------------------
const MATH_tdst_Vector & XeMesh::GetBoundingVolumeCenter( )
{
    ERR_X_Assert( m_bHasBoundingVolume ); 
    return m_vAABVCenter; 
}

// ------------------------------------------------------------------------------------------------
// Name   : GetBoundingVolumeRadius
// ------------------------------------------------------------------------------------------------
float XeMesh::GetBoundingVolumeRadius( ) 
{
    ERR_X_Assert( m_bHasBoundingVolume );
    return m_fBoundingVolumeRadius; 
}

// ------------------------------------------------------------------------------------------------
// Name   : AddLight
// ------------------------------------------------------------------------------------------------
bool XeMesh::AddLight( ULONG _ulLighType, ULONG _ulLighIndex )
{
    return g_pXeContextManagerEngine->PushLight(m_ulLightSetIndex, _ulLighType, _ulLighIndex);
}

void XeMesh::UseLightSet( bool _bUseLightSet )
{
    m_bUseLightSet = _bUseLightSet;
    if (m_bUseLightSet)
    {
        g_pXeContextManagerEngine->BeginAddLights();
        g_pXeContextManagerEngine->EndAddLights();
        m_ulLightSetIndex = g_pXeContextManagerEngine->GetCurrentLightSetIndex();
    }
}
