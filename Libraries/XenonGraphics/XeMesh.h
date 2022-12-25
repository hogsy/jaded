#ifndef XEMESH_HEADER
#define XEMESH_HEADER

#include "XeUtils.h"
#include "XeIndexBuffer.h"

// Forward declaration, see XeMaterial.*
class XeMaterial;

class XeMesh
{
public:

    M_DeclareOperatorNewAndDelete();

    XeMesh();
    ~XeMesh();
    inline void ReInit();

    // Utility
    void                    SetVertexFormatHandle(ULONG _ulHandle) { m_ulVertexFormatHandle = _ulHandle; }
    ULONG                   GetVertexFormatHandle(void) { return m_ulVertexFormatHandle; }
    void                    SetIndices(WORD *_pIndices, ULONG _ulIndexCount);
    inline void             SetIndices(XeIndexBuffer *_pIndexBuffer);
    XeIndexBuffer*          GetIndices() { return m_pIndexStream; }
    
    void                    SetDynamic(BOOL _bDynamic) { m_bDynamic = _bDynamic; }
    BOOL                    IsDynamic() { return m_bDynamic; }
    void                    ClearAllStreams();
    UINT                    AddStream(ULONG _ulComponents, BOOL _bDynamic = FALSE, LPVOID _pVertexBuffer = NULL, ULONG ulVertexCount = 0);
    UINT                    AddStream(ULONG _ulComponents, XeBuffer *_poBuffer);
    inline XeVertexStream*  GetStream(UINT _uiId);
    inline XeVertexStream*  GetStreamByComponents(ULONG _ulComponents);
    UINT                    GetStreamCount() { return m_vecVertexStream.size(); }
    inline void             SetStreamComponents(UINT _uiId, ULONG _ulComponents);

    // skinning
    inline void             SetMaxWeights(INT iMaxWeight) {m_iMaxWeight = iMaxWeight;}
    inline INT              GetMaxWeights() { return m_iMaxWeight; }
    inline void             SetMaxBones(INT iMaxBones) {m_iMaxBones = iMaxBones;}
    inline INT              GetMaxBones() { return m_iMaxBones; }

    BOOL                    HasBoundingVolume() { return m_bHasBoundingVolume; }
    void                    SetAxisAlignedBoundingVolume( const MATH_tdst_Vector & _vAABVMin, const MATH_tdst_Vector & _vAABVMax );
    void                    GetAxisAlignedBoundingVolume( MATH_tdst_Vector & _vAABVMin, MATH_tdst_Vector & _vAABVMax );
    const MATH_tdst_Vector & GetBoundingVolumeCenter( );
    float                   GetBoundingVolumeRadius( );
    inline MATH_tdst_Vector* GetBoundingVolumeMin(void) { return &m_vAABVMin; }
    inline MATH_tdst_Vector* GetBoundingVolumeMax(void) { return &m_vAABVMax; }

    BOOL                    UseLightSet( )                      { return m_bUseLightSet; }
    void                    UseLightSet( bool _bUseLightSet );
    void                    ClearLightSet( )                    { m_ulLightSetIndex = 0; }
    ULONG                   GetLightSetIndex( )                 { return m_ulLightSetIndex; }
    bool                    AddLight( ULONG _ulLighType, ULONG _ulLighIndex );

    //user data
    void                    SetUserDataPointer(void* pData) { m_pUserData = pData; }
    void*                   GetUserDataPointer() { return m_pUserData; }
 
private:

    // Members
    BOOL                        m_bDynamic;
    ULONG                       m_ulVertexFormatHandle;
    XeIndexBuffer               *m_pIndexStream;
    std::vector<XeVertexStream> m_vecVertexStream;

    // skinning
    INT                         m_iMaxWeight;
    INT                         m_iMaxBones;

    // Axis Aligned Bounding Volume
    BOOL                        m_bHasBoundingVolume;
    MATH_tdst_Vector            m_vAABVMin;
    MATH_tdst_Vector            m_vAABVMax;
    MATH_tdst_Vector            m_vAABVCenter;
    float                       m_fBoundingVolumeRadius;

    // lighting
    BOOL                        m_bUseLightSet;
    ULONG                       m_ulLightSetIndex;

    // user data
    void*                       m_pUserData;
};

// ------------------------------------------------------------------------------------------------
// Name   : ReInit()
// Params : 
// RetVal : 
// Descr. : Init
// ------------------------------------------------------------------------------------------------
void XeMesh::ReInit()
{
    m_bDynamic = FALSE;
    m_iMaxWeight = 0;
    m_iMaxBones = 0;
    SAFE_RELEASE(m_pIndexStream);
    ClearAllStreams();

    if (m_pUserData)
    {
        MEM_Free(m_pUserData);
        m_pUserData = NULL;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : GetStreamByComponents()
// Params : _ulComponents -> Mask of all components that we look for
// RetVal : pointer to stream if any was found
// Descr. : Get a stream pointer according to component mask
// ------------------------------------------------------------------------------------------------
XeVertexStream* XeMesh::GetStreamByComponents(ULONG _ulComponents)
{
    for (ULONG ulCurStream = 0; ulCurStream < m_vecVertexStream.size(); ulCurStream++)
    {
        if ((m_vecVertexStream[ulCurStream].ulComponents & _ulComponents) == _ulComponents)
        {
            return &m_vecVertexStream[ulCurStream];
        }
    }

    return NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : SetIndices
// Params : _pIndexBuffer -> new buffer pointer
// RetVal : 
// Descr. : Set a new index stream releasing any existing index stream
// ------------------------------------------------------------------------------------------------
void XeMesh::SetIndices(XeIndexBuffer *_pIndexBuffer)
{    
    SAFE_RELEASE(m_pIndexStream);
    m_pIndexStream = _pIndexBuffer;
    
    if (_pIndexBuffer != NULL) 
        _pIndexBuffer->AddRef();
}

XeVertexStream* XeMesh::GetStream(UINT _uiId) 
{ 
    ERR_X_Assert(m_vecVertexStream.size() > _uiId);

    if (m_vecVertexStream.size() > _uiId)
        return &m_vecVertexStream[_uiId]; 
    else
        return NULL;
}

void XeMesh::SetStreamComponents(UINT _uiId, ULONG _ulComponents)
{
    ERR_X_Assert(m_vecVertexStream.size() > _uiId);

    if(IsDynamic())
    {
        if (m_vecVertexStream.size() > _uiId)
        {
            m_vecVertexStream[_uiId].ulComponents = _ulComponents;
            m_ulVertexFormatHandle = VS_INVALID_HANDLE;
        }
    }
}

#endif // XEMESH_HEADER
