// ----------------------------------------------------------------------------
// Author : Sebastien Comte
// File   : XMP_XenonMeshBuilder.inl
// Date   : 2005-05-28
// Descr. : 
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// IMPLEMENTATION
// ----------------------------------------------------------------------------

namespace XenonMeshPack
{

// ----------------------------------------------------------------------------
// IMPLEMENTATION - Quadric
// ----------------------------------------------------------------------------
  inline Quadric::Quadric(void)
  {
    Clear();
  }

  inline Quadric::Quadric(const Quadric& _q)
  {
    *this = _q;
  }

  inline Quadric::~Quadric(void)
  {
  }

  inline Quadric& Quadric::operator= (const Quadric& _q)
  {
    m_A = _q.m_A;
    m_B = _q.m_B;
    m_C = _q.m_C;

    return *this;
  }

  inline Quadric& Quadric::operator+= (const Quadric& _q)
  {
    m_A += _q.m_A;
    m_B += _q.m_B;
    m_C += _q.m_C;

    return *this;
  }

  inline void Quadric::Clear(void)
  {
    m_A.Reset();
    m_B.Reset();
    m_C = 0.0f;
  }

// ----------------------------------------------------------------------------
// IMPLEMENTATION - PositionInfo
// ----------------------------------------------------------------------------
  inline void PositionInfo::AddVertex(VertexPtr* _vertex)
  {
    VertexPtrNode* node = MeshBuilder::GetAllocators()->positionLinkAllocator.Allocate();

    node->element = _vertex;
    node->next    = vertices;
    vertices      = node;
  }

// ----------------------------------------------------------------------------
// IMPLEMENTATION - Vertex
// ----------------------------------------------------------------------------

  inline void Vertex::Reset(void)
  {
    data.pos  = NULL;
    neighbors = NULL;
    faces     = NULL;
    edges     = NULL;
  }

  inline const D3DXVECTOR3& Vertex::GetPosition(void) const
  {
    return data.pos->position;
  }

  inline D3DXVECTOR3& Vertex::GetPosition(void)
  {
    return data.pos->position;
  }

  inline bool Vertex::IsNeighbor(const VertexPtr* _vertex) const
  {
    XMP_ASSERT(_vertex != NULL);

    VertexPtrNode* node = neighbors;
    while (node != NULL)
    {
      if (node->element == _vertex)
        return true;

      node = node->next;
    }

    return false;
  }

  inline void Vertex::AddNeighbor(VertexPtr* _vertex)
  {
    if (!IsNeighbor(_vertex))
    {
      VertexPtrNode* prevNode = neighbors;
      neighbors               = MeshBuilder::GetAllocators()->vertexPtrNodeAllocator.Allocate();
      neighbors->element      = _vertex;
      neighbors->next         = prevNode;
    }
  }

  inline void Vertex::ReplaceNeighbor(VertexPtr* _oldVertex, VertexPtr* _newVertex)
  {
    VertexPtrNode* node = neighbors;
    while (node != NULL)
    {
      if (node->element == _oldVertex)
      {
        node->element = _newVertex;
        break;
      }

      node = node->next;
    }
  }

  inline void Vertex::SetValid(bool _valid)
  {
    if (_valid)
      vertexIndex |=  0x80000000;
    else
      vertexIndex &= ~0x80000000;
  }

  inline bool Vertex::IsValid(void) const
  {
    return ((vertexIndex & 0x80000000) != 0);
  }

  inline void Vertex::SetBoundary(bool _boundary)
  {
    if (_boundary)
      vertexIndex |=  0x40000000;
    else
      vertexIndex &= ~0x40000000;
  }

  inline bool Vertex::IsBoundary(void) const
  {
    return ((vertexIndex & 0x40000000) != 0);
  }

  inline void Vertex::SetIndex(ULONG _index)
  {
    vertexIndex &= 0xc0000000;
    vertexIndex |= (_index & 0x3fffffff);
  }

  inline ULONG Vertex::GetIndex(void) const
  {
    return (vertexIndex & 0x3fffffff);
  }

// ----------------------------------------------------------------------------
// IMPLEMENTATION - Face
// ----------------------------------------------------------------------------
  inline void Face::Compute(void)
  {
    D3DXVECTOR3 v1 = vertices[2]->ptr->GetPosition() - vertices[0]->ptr->GetPosition();
    D3DXVECTOR3 v2 = vertices[1]->ptr->GetPosition() - vertices[0]->ptr->GetPosition();
    D3DXVECTOR3 v;

    // Area
    D3DXVec3Cross(&v, &v1, &v2);
    area = D3DXVec3Length(&v) * 0.5f;

    if (area > 0.0f)
    {
      // Plane
      D3DXPlaneFromPoints(&plane, &vertices[0]->ptr->GetPosition(),
                                  &vertices[2]->ptr->GetPosition(),
                                  &vertices[1]->ptr->GetPosition());
    }
    else
    {
      // I dont think this case is ever happening anymore, but let's play it safe
      // Vertices are colinear, face is invalid...
      plane.a = plane.b = plane.c = plane.d = 0.0f;
    }
  }

  inline void Face::GetQuadric(Quadric* _quadric) const
  {
    if (area > 0.0f)
    {
      _quadric->Set(vertices[0]->ptr->GetPosition(), vertices[0]->ptr->data.texCoord, 
                    vertices[1]->ptr->GetPosition(), vertices[1]->ptr->data.texCoord, 
                    vertices[2]->ptr->GetPosition(), vertices[2]->ptr->data.texCoord);
    }
    else
    {
      _quadric->Clear();
    }
  }

  inline bool Face::HasVertex(const Vertex* _vertex) const
  {
    XMP_ASSERT(_vertex != NULL);

    return (vertices[0]->ptr == _vertex) ||
           (vertices[1]->ptr == _vertex) ||
           (vertices[2]->ptr == _vertex);
  }

  inline bool Face::HasVertex(const VertexPtr* _vertex) const
  {
    return HasVertex(_vertex->ptr);
  }

  inline void Face::ReplaceVertex(const VertexPtr* _ori, VertexPtr* _new)
  {
    if (vertices[0] == _ori)
    {
      vertices[0] = _new;
    }
    else if (vertices[1] == _ori)
    {
      vertices[1] = _new;
    }
    else if (vertices[2] == _ori)
    {
      vertices[2] = _new;
    }
  }

// ----------------------------------------------------------------------------
// IMPLEMENTATION - MeshBuilder
// ----------------------------------------------------------------------------
  inline void MeshBuilder::RemoveVertex(VertexPtr* _vertex)
  {
    if (_vertex->ptr != NULL)
    {
      // Recycle the neighbor nodes
      VertexPtrNode* neighborNode = _vertex->ptr->neighbors;
      VertexPtrNode* nextNeighborNode;
      while (neighborNode != NULL)
      {
        nextNeighborNode = neighborNode->next;

        m_Allocators.vertexPtrNodeAllocator.Free(neighborNode);

        neighborNode = nextNeighborNode;
      }

      // Recycle the face nodes
      FacePtrNode* faceNodePtr = _vertex->ptr->faces;
      FacePtrNode* nextFaceNodePtr;
      while (faceNodePtr != NULL)
      {
        nextFaceNodePtr = faceNodePtr->next;

        m_Allocators.facePtrNodeAllocator.Free(faceNodePtr);

        faceNodePtr = nextFaceNodePtr;
      }

      // Recycle the vertex
      m_Allocators.vertexAllocator.Free(_vertex);

      --m_NbVertices;
    }
  }

  inline void MeshBuilder::RemoveFace(FacePtr* _face)
  {
    if (_face->ptr != NULL)
    {
      // Recycle the face
      m_Allocators.faceAllocator.Free(_face);

      --m_NbFaces;
    }
  }

  inline AllAllocators* MeshBuilder::GetAllocators(void)
  {
    return s_AllAllocators;
  }

  inline void MeshBuilder::SetMaximumVertexCount(ULONG _maxVertices)
  {
    if (_maxVertices > 0)
      m_MaxVertexCount = _maxVertices;
  }

  inline void MeshBuilder::SetMaximumFaceCount(ULONG _maxFaceCount)
  {
    if (_maxFaceCount > 0)
      m_MaxFaceCount = _maxFaceCount;
  }

  inline bool IsPositionEqual(FLOAT _x1, FLOAT _y1, FLOAT _z1,
                              FLOAT _x2, FLOAT _y2, FLOAT _z2, FLOAT _epsilon)
  {
    return ((fabsf(_x1 - _x2) + fabsf(_y1 - _y2) + fabsf(_z1 - _z2)) < _epsilon);
  }

  inline bool IsPositionEqual(const D3DXVECTOR3& _p1,
                              const D3DXVECTOR3& _p2,
                              FLOAT              _epsilon)
  {
    return IsPositionEqual(_p1.x, _p1.y, _p1.z, _p2.x, _p2.y, _p2.z, _epsilon);
  }

  inline bool IsColorEqual(ULONG _color1, ULONG _color2)
  {
    return (_color1 == _color2);
  }

  inline bool IsVec4Equal(FLOAT _x1, FLOAT _y1, FLOAT _z1, FLOAT _w1,
                          FLOAT _x2, FLOAT _y2, FLOAT _z2, FLOAT _w2,
                          FLOAT _epsilon)
  {
    return ((fabsf(_x1 - _x2) + fabsf(_y1 - _y2) + fabsf(_z1 - _z2) + fabsf(_w1 - _w2)) < _epsilon);
  }

  inline bool IsColorEqual(const D3DXCOLOR& _color1, 
                           const D3DXCOLOR& _color2, 
                           FLOAT            _epsilon)
  {
    return IsVec4Equal(_color1.r, _color1.g, _color1.b, _color1.a,
                       _color2.r, _color2.g, _color2.b, _color2.a,
                       _epsilon);
  }

  inline bool IsWeightEqual(ULONG _w1, ULONG _w2)
  {
    return (_w1 == _w2);
  }

  inline bool IsWeightEqual(const D3DXVECTOR4& _w1,
                            const D3DXVECTOR4& _w2,
                            FLOAT              _epsilon)
  {
    return IsVec4Equal(_w1.x, _w1.y, _w1.z, _w1.w,
                       _w2.x, _w2.y, _w2.z, _w2.w,
                       _epsilon);
  }

  inline bool IsTexCoordEqual(FLOAT _u1, FLOAT _v1, FLOAT _u2, FLOAT _v2, FLOAT _epsilon)
  {
    return ((fabsf(_u1 - _u2) + fabsf(_v1 - _v2)) < _epsilon);
  }

  inline bool IsTexCoordEqual(const D3DXVECTOR2& _uv1,
                              const D3DXVECTOR2& _uv2,
                              FLOAT              _epsilon)
  {
    return IsTexCoordEqual(_uv1.x, _uv1.y, _uv2.x, _uv2.y, _epsilon);
  }

// ----------------------------------------------------------------------------
// IMPLEMENTATION - MaterialInfo
// ----------------------------------------------------------------------------

  inline void MaterialInfo::SetMaterialId(LONG _id)
  {
    m_MaterialId = _id;
  }

  inline void MaterialInfo::SetFlags(ULONG _processingFlags)
  {
    m_ProcessingFlags = _processingFlags;
  }

  inline void MaterialInfo::SetTransform(const D3DXMATRIX& _xform)
  {
    m_TexCoordTransform = _xform;
  }

  inline void MaterialInfo::SetHeightMap(UCHAR* _map, LONG _width, LONG _height)
  {
    m_HeightMap       = _map;
    m_HeightMapWidth  = _width;
    m_HeightMapHeight = _height;
  }

  inline void MaterialInfo::SetDisplacement(FLOAT _offset, FLOAT _max)
  {
    m_DisplacementOffset = _offset;
    m_DisplacementMax    = _max;
  }

  inline void MaterialInfo::SetMaxTriangleArea(FLOAT _maxArea)
  {
    m_MaxTriangleArea = _maxArea;
  }

  inline void MaterialInfo::SetMaxSearchDistance(FLOAT _maxSearchDistance)
  {
    m_MaxSearchDistance = _maxSearchDistance;
  }

  inline void MaterialInfo::SetSmoothThreshold(FLOAT _smoothThreshold)
  {
    m_SmoothThreshold = _smoothThreshold;
  }

  inline void MaterialInfo::SetCostMultiplier(FLOAT _costMultiplier)
  {
    m_CostMultiplier = _costMultiplier;
  }

  inline void MaterialInfo::SetChamfer(FLOAT _threshold, FLOAT _length, FLOAT _weldThreshold)
  {
    m_ChamferThreshold     = _threshold;
    m_ChamferLength        = _length;
    m_ChamferWeldThreshold = _weldThreshold;
  }

  inline LONG MaterialInfo::GetMaterialId(void) const
  {
    return m_MaterialId;
  }

  inline bool MaterialInfo::IsChamferEnabled(void) const
  {
    return ((m_ProcessingFlags & PF_CHAMFER) != 0);
  }

  inline bool MaterialInfo::IsTessellationEnabled(void) const
  {
    return ((m_ProcessingFlags & PF_TESSELLATE) != 0);
  }

  inline bool MaterialInfo::IsDisplacementEnabled(void) const
  {
    return ((m_ProcessingFlags & PF_DISPLACE) != 0) && (m_HeightMap != NULL);
  }

  inline bool MaterialInfo::IsSimplificationEnabled(void) const
  {
    return (IsTessellationEnabled() && IsDisplacementEnabled());
  }

  inline const D3DXMATRIX* MaterialInfo::GetTransform(void) const
  {
    return &m_TexCoordTransform;
  }

  inline FLOAT MaterialInfo::GetDisplacementOffset(void) const
  {
    return m_DisplacementOffset;
  }

  inline FLOAT MaterialInfo::GetDisplacementMax(void) const
  {
    return m_DisplacementMax;
  }

  inline FLOAT MaterialInfo::GetMaxTriangleArea(void) const
  {
    return m_MaxTriangleArea;
  }

  inline FLOAT MaterialInfo::GetMaxSearchDistance(void) const
  {
    return m_MaxSearchDistance;
  }

  inline FLOAT MaterialInfo::GetSmoothThreshold(void) const
  {
    return m_SmoothThreshold;
  }

  inline FLOAT MaterialInfo::GetCostMultiplier(void) const
  {
    return m_CostMultiplier;
  }

  inline FLOAT MaterialInfo::GetChamferThreshold(void) const
  {
    return m_ChamferThreshold;
  }

  inline FLOAT MaterialInfo::GetChamferLength(void) const
  {
    return m_ChamferLength;
  }

  inline FLOAT MaterialInfo::GetChamferWeldThreshold(void) const
  {
    return m_ChamferWeldThreshold;
  }

  inline StaticVertexDataKDTree* MaterialInfo::GetDataTree(void)
  {
    return &m_DataTree;
  }

// ----------------------------------------------------------------------------
// IMPLEMENTATION - EdgeDatabase
// ----------------------------------------------------------------------------

  inline EdgeDatabase::EdgeDatabase(void)
  {
    m_EdgePool.Initialize(4096);
  }

  inline EdgeDatabase::~EdgeDatabase(void)
  {
    m_Edges.clear();
  }

  inline ULONG64 EdgeDatabase::BuildKey(VertexPtr* _v1, VertexPtr* _v2)
  {
    ULONG64 key;

    if ((DWORD)_v1 < (DWORD)_v2)
    {
      key   = (DWORD)_v1;
      key <<= 32;
      key  |= (DWORD)_v2;
    }
    else
    {
      key   = (DWORD)_v2;
      key <<= 32;
      key  |= (DWORD)_v1;
    }

    return key;
  }

  inline SplitEdge* EdgeDatabase::FindEdge(VertexPtr* _v1, VertexPtr* _v2)
  {
    ULONG64 key = BuildKey(_v1, _v2);

    SplitEdgeMap::iterator itEdge = m_Edges.find(key);
    if (itEdge != m_Edges.end())
    {
      return itEdge->second;
    }

    return NULL;
  }

  inline SplitEdge* EdgeDatabase::AddEdge(VertexPtr* _v1, VertexPtr* _v2)
  {
    ULONG64 key = BuildKey(_v1, _v2);

    SplitEdge* edge = FindEdge(_v1, _v2);
    if (edge != NULL)
      return edge;

    edge              = m_EdgePool.Allocate();
    edge->vertices[0] = _v1;
    edge->vertices[1] = _v2;
    edge->splitVertex = NULL;

    m_Edges[key] = edge;

    return edge;
  }

};
