#ifndef XESTATICMESH_HEADER
#define XESTATICMESH_HEADER

#include "XeUtils.h"
#include "XeIndexBuffer.h"

// Forward declaration, see XeMaterial.*
class XeMaterial;

class XeStaticMesh
{
public:

    XeStaticMesh();
    ~XeStaticMesh();

    // Utility
    void                    SetVertexFormatHandle(ULONG _ulHandle) { m_ulVertexFormatHandle = _ulHandle; }
    ULONG                   GetVertexFormatHandle(void) { return m_ulVertexFormatHandle; }
    void                    SetIndices(WORD *_pIndices, ULONG _ulIndexCount);
    XeIndexBuffer*          GetIndices() { return m_pIndexStream; }

    void                    ClearAllStreams();
    UINT                    AddStream(ULONG _ulComponents, BOOL _bDynamic = FALSE, LPVOID _pVertexBuffer = NULL, ULONG ulVertexCount = 0);
    XeVertexStream*         GetStream(UINT _uiId) { return &m_vecVertexStream[_uiId]; }
    UINT                    GetStreamCount() { return m_vecVertexStream.size(); }

private:

    // Members
    ULONG                       m_ulVertexFormatHandle;
    XeIndexBuffer               *m_pIndexStream;
    std::vector<XeVertexStream> m_vecVertexStream;
};

#endif // XESTATICMESH_HEADER