// ------------------------------------------------------------------------------------------------
// File   : XMP_XenonMeshBuilder_Chamfer.cpp
// Date   : 2005-06-21
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------

// Prevent STL for complaining all the time
#pragma warning(disable:4702)

#include "Precomp.h"

#if defined(ACTIVE_EDITORS) && defined(_XENON_RENDER)

#include <list>
#include <vector>
#include <map>
#include <algorithm>

#include <d3dx9.h>

#if defined(_XENON_RENDER)
#include "XenonGraphics/DevHelpers/XeBench.h"
#include "XenonGraphics/DevHelpers/XePerf.h"
#endif

#include "XMP_XenonMeshBuilder.h"

namespace XenonMeshPack
{

// ------------------------------------------------------------------------------------------------
// PRIVATE CONSTANTS
// ------------------------------------------------------------------------------------------------
  const ULONG INVALID_ID = 0xffffffff;

  const FLOAT CHAMFER_POSITION_EPSILON      = 0.00005f;
  const FLOAT CHAMFER_POSITION_DB_EPSILON   = 0.0000001f;
  const FLOAT CHAMFER_UV_EPSILON            = 0.0005f;
  const FLOAT CHAMFER_TRIANGLE_AREA_EPSILON = 0.000000001f;
  const FLOAT CHAMFER_VISIBLE_THRESHOLD     = 0.1f;

  const ULONG SNAP_NONE     = 0xffffffff;
  const ULONG SNAP_AVERAGE  = 0xfffffffe;
  const ULONG SNAP_MIDPOINT = 0xfffffffd;

// ------------------------------------------------------------------------------------------------
// PRIVATE STRUCTURES
// ------------------------------------------------------------------------------------------------

  struct ChamferNode
  {
    XMP_DECLARE_NEW_AND_DELETE();

    ULONG        index;
    ChamferNode* next;
  };

  struct ChamferVertex
  {
    XMP_DECLARE_NEW_AND_DELETE();

    D3DXVECTOR3  position;
    ULONG        nbChamferedEdges;
    ULONG        vertexSnap;
    ChamferNode* faces;
    ChamferNode* edges;
    bool         isBoundary;
  };

  struct ChamferUV
  {
    XMP_DECLARE_NEW_AND_DELETE();

    D3DXVECTOR2 uv;
    bool        rewritten;
  };

  struct ChamferHalfEdge
  {
    XMP_DECLARE_NEW_AND_DELETE();

    ULONG vertexIndex;
    ULONG uvIndices[2];

    ULONG lineFlags;

    bool  cutVertex;
    FLOAT cutLength;
    ULONG cutVertexIndex;
    ULONG cutUVIndices[2];
  };

  struct ChamferEdge
  {
    XMP_DECLARE_NEW_AND_DELETE();

    bool  doChamfer;
    bool  uvsAreShared;
    bool  isVisible;
    bool  isBoundary;

    ULONG faceIndices[2];

    ChamferHalfEdge halfEdge[2];

    inline ULONG GetFaceId(ULONG _faceIndex)
    {
      if (_faceIndex == faceIndices[0])
        return 0;
      else
        return 1;
    }
  };

  struct ChamferTriangle
  {
    XMP_DECLARE_NEW_AND_DELETE();

    ULONG elementIndex;
    ULONG faceIndex;
    bool  locked;

    D3DXVECTOR3 normal;

    ULONG vertexIndices[3];
    ULONG uvIndices[3];

    ULONG newVertexIndices[3];
    ULONG newUVIndices[3];

    ChamferEdge* edges[3];

    D3DXVECTOR3 positionInterpolators[3];
    D3DXVECTOR2 uvInterpolators[3];

    inline ChamferHalfEdge& GetHalfEdge(ULONG _edgeIndex, ULONG _vertexIndex)
    {
      if (edges[_edgeIndex]->halfEdge[0].vertexIndex == vertexIndices[_vertexIndex])
        return edges[_edgeIndex]->halfEdge[0];
      else
      {
        if (edges[_edgeIndex]->halfEdge[1].vertexIndex == vertexIndices[_vertexIndex])
          return edges[_edgeIndex]->halfEdge[1];
        else
          return GetHalfEdge((_edgeIndex + 2) % 3, _vertexIndex);
      }
    }

    inline ULONG GetCutVertexIndex(ULONG _edgeIndex, ULONG _vertexIndex)
    {
      ChamferHalfEdge& halfEdge = GetHalfEdge(_edgeIndex, _vertexIndex);
      return halfEdge.cutVertexIndex;
    }

    inline void SetCutVertexIndex(ULONG _edgeIndex, ULONG _vertexIndex, ULONG _cutIndex)
    {
      ChamferHalfEdge& halfEdge = GetHalfEdge(_edgeIndex, _vertexIndex);
      halfEdge.cutVertexIndex = _cutIndex;
    }

    inline ULONG GetCutUVIndex(ULONG _edgeIndex, ULONG _vertexIndex)
    {
      ChamferHalfEdge& halfEdge = GetHalfEdge(_edgeIndex, _vertexIndex);
      return halfEdge.cutUVIndices[edges[_edgeIndex]->GetFaceId(faceIndex)];
    }

    inline void SetCutUVIndex(ULONG _edgeIndex, ULONG _vertexIndex, ULONG _cutUVIndex)
    {
      ChamferHalfEdge& halfEdge = GetHalfEdge(_edgeIndex, _vertexIndex);
      halfEdge.cutUVIndices[edges[_edgeIndex]->GetFaceId(faceIndex)] = _cutUVIndex;
    }
  };

  typedef std::vector<ChamferVertex*>      ChamferVertexArray;
  typedef std::vector<ChamferUV*>          ChamferUVArray;
  typedef std::vector<ChamferEdge*>        ChamferEdgeArray;
  typedef std::vector<ChamferTriangle*>    ChamferTriangleArray;

// ------------------------------------------------------------------------------------------------
// PRIVATE CLASSES
// ------------------------------------------------------------------------------------------------

  class ChamferDatabase;
  class TriangleDatabase;

  class ChamferDatabase
  {
  public:

    ChamferDatabase(void);
    ~ChamferDatabase(void);

    void SetChamfer(ULONG _elementIndex, FLOAT _chamferThreshold, FLOAT _chamferLength, FLOAT _weldThreshold);

    void AddTriangle(Face* _face, bool _locked);

    bool Process(TriangleDatabase* _triangleDB);

  private:

    ULONG AddPosition(const D3DXVECTOR3& _position, FLOAT _weldThreshold);
    ULONG AddUV(const D3DXVECTOR2& _texCoord);

    ULONG PushPosition(const D3DXVECTOR3& _position);
    ULONG PushUV(const D3DXVECTOR2& _texCoord);

    ULONG PushTriangle(void);
    ULONG PushEdge(void);

    void AddTriangleToVertex(ULONG _vertex, ULONG _face);

    bool BuildEdges(void);
    void ValidateChamfer(void);
    void ComputeEdgeCutLocations(void);
    void ComputeCornerPoints(void);
    void ComputeEdgeIndices(void);
    void ApplyQuadCorrection(void);
    void UpdateVertexIndices(void);
    void CollapseVertices(void);
    void GenerateTriangles(TriangleDatabase* _triangleDB);

  private:

    PoolAllocator<ChamferVertex>      m_VertexAllocator;
    PoolAllocator<ChamferUV>          m_UVAllocator;
    PoolAllocator<ChamferEdge>        m_EdgeAllocator;
    PoolAllocator<ChamferTriangle>    m_TriangleAllocator;
    PoolAllocator<ChamferNode>        m_NodeAllocator;

    ChamferVertexArray      m_Vertices;
    ChamferUVArray          m_TexCoords;
    ChamferEdgeArray        m_Edges;
    ChamferTriangleArray    m_Triangles;

    FLOAT m_ChamferLengths[MESH_BUILDER_MAX_ELEMENTS];
    FLOAT m_ChamferThresholds[MESH_BUILDER_MAX_ELEMENTS];
    FLOAT m_ChamferWeldThresholds[MESH_BUILDER_MAX_ELEMENTS];
  };


  class TriangleDatabase
  {
    struct ColorWeights
    {
      D3DXCOLOR color;
      D3DXCOLOR weights;
    };

    struct Triangle
    {
      ULONG elementIndex;
      ULONG positionIndices[3];
      ULONG uvIndices[3];
      ULONG colorWeightsIndices[3];

      inline bool HasVertex(ULONG _posIndex) const
      {
        return ((positionIndices[0] == _posIndex) ||
                (positionIndices[1] == _posIndex) ||
                (positionIndices[2] == _posIndex));
      }
    };

    typedef std::vector<D3DXVECTOR2>  Vec2Array;
    typedef std::vector<D3DXVECTOR3>  Vec3Array;
    typedef std::vector<ColorWeights> ColorWeightsArray;
    typedef std::vector<Triangle>     TriangleArray;

  public:

    TriangleDatabase(void);
    ~TriangleDatabase(void);

    void SetKDTree(ULONG _elementIndex, StaticVertexDataKDTree* _tree, FLOAT _searchDistance);

    void AddTriangle(ULONG _elementIndex,
                     const D3DXVECTOR3& _p1, const D3DXVECTOR2& _uv1, 
                     const D3DXVECTOR3& _p2, const D3DXVECTOR2& _uv2, 
                     const D3DXVECTOR3& _p3, const D3DXVECTOR2& _uv3, 
                     const D3DXVECTOR3& _normal);

    void UpdateBuilder(MeshBuilder* _builder);

  private:

    ULONG AddPosition(const D3DXVECTOR3& _pos);
    ULONG AddUV(const D3DXVECTOR2& _uv);
    ULONG AddColorWeights(const D3DXVECTOR3& _position, ULONG _elementIndex);

    bool IsTriangleValid(const Triangle& _triangle, const D3DXVECTOR3& _normal);

  private:

    Vec3Array         m_Positions;
    Vec2Array         m_TexCoords;
    ColorWeightsArray m_ColorWeights;
    TriangleArray     m_Triangles;

    FLOAT m_SearchDistances[MESH_BUILDER_MAX_ELEMENTS];

    StaticVertexDataKDTree* m_Trees[MESH_BUILDER_MAX_ELEMENTS];
  };

// ------------------------------------------------------------------------------------------------
// EXTERNAL FUNCTIONS
// ------------------------------------------------------------------------------------------------
  extern void Log(LogLevel _level, const CHAR* _format, ...);

// ------------------------------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// ------------------------------------------------------------------------------------------------
  inline ULONG TriangleFindVertex(ChamferTriangle* _triangle, ULONG _index)
  {
    ULONG i;

    for (i = 0; i < 3; ++i)
    {
      if (_triangle->vertexIndices[i] == _index)
        return i;
    }

    return INVALID_ID;
  }

  inline ULONG TriangleFindNewVertex(ChamferTriangle* _triangle, ULONG _index)
  {
    ULONG i;

    for (i = 0; i < 3; ++i)
    {
      if (_triangle->newVertexIndices[i] == _index)
        return i;
    }

    return INVALID_ID;
  }

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

  void MeshBuilder::Chamfer(void)
  {
    XMP_PROFILE_FUNCTION();

    XMP_ASSERT(!m_CanAddVertices);

#if defined(MESH_BUILDER_VERBOSE)
    ULONG nbOriFaces    = m_NbFaces;
    ULONG nbOriVertices = m_NbVertices;
#endif

    ChamferDatabase   chamferDatabase;
    TriangleDatabase  triangleDB;

    if (m_CanAddVertices)
      return;

    ULONG nbFaces = m_Faces.size();
    ULONG i;
    bool  mustChamfer = false;

    // Set the trees for the triangle database
    for (i = 0; i < MESH_BUILDER_MAX_ELEMENTS; ++i)
    {
        // Set the KD tree and search distance for color and weight interpolation
      triangleDB.SetKDTree(i, m_MaterialInfo[i].GetDataTree(), Max(m_MaterialInfo[i].GetChamferLength(), 0.1f) * 4.0f);

      chamferDatabase.SetChamfer(i, m_MaterialInfo[i].GetChamferThreshold(),
                                    m_MaterialInfo[i].GetChamferLength(),
                                    m_MaterialInfo[i].GetChamferWeldThreshold());

      mustChamfer |= m_MaterialInfo[i].IsChamferEnabled();
    }

    if (!mustChamfer)
      return;

    // Build the chamfer database
    for (i = 0; i < nbFaces; ++i)
    {
      MaterialInfo* info;

      FacePtr* facePtr = m_Faces[i];
      Face*    face    = facePtr->ptr;

      // We're alive!
      if ((i % 500) == 0)
        Tick();

      if (face == NULL)
        continue;

      info = &m_MaterialInfo[face->elementIndex];

      chamferDatabase.AddTriangle(face, !info->IsChamferEnabled());
    }

    // Chamfer processing
    if (chamferDatabase.Process(&triangleDB))
    {
      // We're alive!
      Tick();

      // Wipe the current content of the mesh database
      Clear();
      Tick();

      // Rebuild internal structures using the result from the chamfer database
      triangleDB.UpdateBuilder(this);
      Tick();

#if defined(MESH_BUILDER_VERBOSE)
      Log(LL_INFO, "XMP - Chamfer -> %u faces (%u vertices) to %u faces (%u vertices)", 
          nbOriFaces, nbOriVertices, m_NbFaces, m_NbVertices);
#endif
    }
  }

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - ChamferDatabase
// ------------------------------------------------------------------------------------------------

  ChamferDatabase::ChamferDatabase(void)
  {
  }

  ChamferDatabase::~ChamferDatabase(void)
  {
  }

  void ChamferDatabase::AddTriangle(Face* _face, bool _locked)
  {
    ULONG indices[3];
    ULONG i;

    for (i = 0; i < 3; ++i)
    {
      indices[i] = AddPosition(_face->vertices[i]->ptr->GetPosition(), m_ChamferWeldThresholds[_face->elementIndex]);
    }

    // Make sure the triangle is valid..
    if ((indices[0] == indices[1]) || (indices[1] == indices[2]) || (indices[2] == indices[0]))
      return;

    // Check if the triangle is already present - Can happen due to the weld threshold
    ULONG nbTri = m_Triangles.size();
    for (i = 0; i < nbTri; ++i)
    {
      ChamferTriangle* tri = m_Triangles[i];

      if ((TriangleFindVertex(tri, indices[0]) != INVALID_ID) &&
          (TriangleFindVertex(tri, indices[1]) != INVALID_ID) &&
          (TriangleFindVertex(tri, indices[2]) != INVALID_ID))
        return;
    }

    ULONG            triangleId = PushTriangle();
    ChamferTriangle* triangle   = m_Triangles[triangleId];

    triangle->elementIndex = _face->elementIndex;
    triangle->locked       = _locked;

    for (i = 0; i < 3; ++i)
    {
      triangle->vertexIndices[i]       = indices[i];
      triangle->uvIndices[i]           = AddUV(_face->vertices[i]->ptr->data.texCoord);

      triangle->newVertexIndices[i] = triangle->vertexIndices[i];
      triangle->newUVIndices[i]     = triangle->uvIndices[i];

      AddTriangleToVertex(triangle->vertexIndices[i], triangleId);
    }

    triangle->normal.x = _face->plane.a;
    triangle->normal.y = _face->plane.b;
    triangle->normal.z = _face->plane.c;
  }

  void ChamferDatabase::SetChamfer(ULONG _elementIndex, FLOAT _threshold, FLOAT _length, FLOAT _weldThreshold)
  {
    m_ChamferThresholds[_elementIndex]     = _threshold;
    m_ChamferLengths[_elementIndex]        = _length;
    m_ChamferWeldThresholds[_elementIndex] = _weldThreshold;
  }

  ULONG ChamferDatabase::AddPosition(const D3DXVECTOR3& _position, FLOAT _weldThreshold)
  {
    ULONG nbVertices = m_Vertices.size();
    ULONG i;

    for (i = 0; i < nbVertices; ++i)
    {
      ChamferVertex* vtx = m_Vertices[i];

      if (IsPositionEqual(_position.x, _position.y, _position.z, vtx->position.x, vtx->position.y, vtx->position.z, _weldThreshold))
        return i;
    }

    return PushPosition(_position);
  }

  ULONG ChamferDatabase::AddUV(const D3DXVECTOR2& _texCoord)
  {
    ULONG nbUVs = m_TexCoords.size();
    ULONG i;

    for (i = 0; i < nbUVs; ++i)
    {
      ChamferUV* vtx = m_TexCoords[i];

      if (IsTexCoordEqual(_texCoord.x, _texCoord.y, vtx->uv.x, vtx->uv.y, CHAMFER_UV_EPSILON))
        return i;
    }

    return PushUV(_texCoord);
  }

  ULONG ChamferDatabase::PushPosition(const D3DXVECTOR3& _position)
  {
    ChamferVertex* newVertex = m_VertexAllocator.Allocate();

    newVertex->position         = _position;
    newVertex->nbChamferedEdges = 0;
    newVertex->vertexSnap       = SNAP_NONE;
    newVertex->faces            = NULL;
    newVertex->edges            = NULL;
    newVertex->isBoundary       = false;

    m_Vertices.push_back(newVertex);

    return m_Vertices.size() - 1;
  }

  ULONG ChamferDatabase::PushUV(const D3DXVECTOR2& _texCoord)
  {
    ChamferUV* newUV = m_UVAllocator.Allocate();

    newUV->uv        = _texCoord;
    newUV->rewritten = false;

    m_TexCoords.push_back(newUV);

    return m_TexCoords.size() - 1;
  }

  ULONG ChamferDatabase::PushTriangle(void)
  {
    ChamferTriangle* triangle = m_TriangleAllocator.Allocate();

    memset(triangle, 0, sizeof(ChamferTriangle));
    m_Triangles.push_back(triangle);

    triangle->faceIndex = m_Triangles.size() - 1;

    return m_Triangles.size() - 1;
  }

  ULONG ChamferDatabase::PushEdge(void)
  {
    ChamferEdge* edge = m_EdgeAllocator.Allocate();

    memset(edge, 0, sizeof(ChamferEdge));
    m_Edges.push_back(edge);

    edge->isVisible             = true;
    edge->halfEdge[0].lineFlags = 0;
    edge->halfEdge[1].lineFlags = 0;
    edge->isBoundary            = false;

    return m_Edges.size() - 1;
  }

  void ChamferDatabase::AddTriangleToVertex(ULONG _vertex, ULONG _face)
  {
    ChamferNode*   faceNode = m_NodeAllocator.Allocate();
    ChamferVertex* vertex   = m_Vertices[_vertex];

    faceNode->index = _face;
    faceNode->next  = vertex->faces;
    vertex->faces   = faceNode;
  }

  bool ChamferDatabase::Process(TriangleDatabase* _triangleDB)
  {
    // Associate edges with the triangles
    if (!BuildEdges())
      return false;

    // Edge clean up to remove problematic cases
    ValidateChamfer();

    // Find the edge cut locations
    ComputeEdgeCutLocations();

    // Detect corner vertices (junction of multiple chamfered edges) and cut locations for them
    ComputeCornerPoints();

    // Recomute the new indices for the edges
    ComputeEdgeIndices();

    // Quad correction
    ApplyQuadCorrection();

    // Compute the new number of points and faces
    UpdateVertexIndices();

    // Collapse some vertices (will eventually discard all vertices that belong to chamfered edges)
    CollapseVertices();

    // Build the final list of triangles
    GenerateTriangles(_triangleDB);

    return true;
  }

  bool ChamferDatabase::BuildEdges(void)
  {
    ULONG nbTriangles = m_Triangles.size();
    ULONG i;
    ULONG j;
    bool  foundOne = false;

    for (i = 0; i < nbTriangles; ++i)
    {
      ChamferTriangle* triangle = m_Triangles[i];

      for (j = 0; j < 3; ++j)
      {
        ChamferVertex* vertex = m_Vertices[triangle->vertexIndices[j]];
        ULONG indexA1 = j;
        ULONG indexA2 = (j + 1) % 3;

        if (triangle->edges[j] != NULL)
          continue;

        ChamferNode* faceNode = vertex->faces;
        while (faceNode != NULL)
        {
          if (faceNode->index != i)
          {
            ChamferTriangle* adjTriangle = m_Triangles[faceNode->index];
            ULONG indexB1 = TriangleFindVertex(adjTriangle, triangle->vertexIndices[indexA1]);
            ULONG indexB2 = TriangleFindVertex(adjTriangle, triangle->vertexIndices[indexA2]);

            if ((indexB1 != INVALID_ID) && (indexB2 != INVALID_ID))
            {
              ULONG edgeId = PushEdge();

              triangle->edges[j] = m_Edges[edgeId];

              triangle->edges[j]->faceIndices[0] = i;
              triangle->edges[j]->faceIndices[1] = faceNode->index;

              triangle->edges[j]->halfEdge[0].vertexIndex = triangle->edges[j]->halfEdge[0].cutVertexIndex = triangle->vertexIndices[indexA1];
              triangle->edges[j]->halfEdge[1].vertexIndex = triangle->edges[j]->halfEdge[1].cutVertexIndex = triangle->vertexIndices[indexA2];

              triangle->edges[j]->halfEdge[0].uvIndices[0] = triangle->edges[j]->halfEdge[0].cutUVIndices[0] = triangle->uvIndices[indexA1];
              triangle->edges[j]->halfEdge[1].uvIndices[0] = triangle->edges[j]->halfEdge[1].cutUVIndices[0] = triangle->uvIndices[indexA2];

              triangle->edges[j]->halfEdge[0].uvIndices[1] = triangle->edges[j]->halfEdge[0].cutUVIndices[1] = adjTriangle->uvIndices[indexB1];
              triangle->edges[j]->halfEdge[1].uvIndices[1] = triangle->edges[j]->halfEdge[1].cutUVIndices[1] = adjTriangle->uvIndices[indexB2];

              if ((triangle->uvIndices[indexA1] == adjTriangle->uvIndices[indexB1]) &&
                  (triangle->uvIndices[indexA2] == adjTriangle->uvIndices[indexB2]))
              {
                triangle->edges[j]->uvsAreShared = true;

                // Check for mirror mapping
                {
                  D3DXVECTOR2 deltaUV;
                  D3DXVECTOR2 deltaUV1;
                  D3DXVECTOR2 deltaUV2;
                  ULONG indexB3 = 0;

                  if (((indexB1 == 0) && (indexB2 == 1)) ||
                      ((indexB1 == 1) && (indexB2 == 0)))
                    indexB3 = 2;
                  else if (((indexB1 == 0) && (indexB2 == 2)) ||
                            ((indexB1 == 2) && (indexB2 == 0)))
                    indexB3 = 1;

                  deltaUV = m_TexCoords[triangle->uvIndices[(j + 1) % 3]]->uv - m_TexCoords[triangle->uvIndices[j]]->uv;
                  D3DXVec2Normalize(&deltaUV, &deltaUV);

                  deltaUV1  = m_TexCoords[triangle->uvIndices[(j + 2) % 3]]->uv - m_TexCoords[triangle->uvIndices[j]]->uv;
                  deltaUV1 -= D3DXVec2Dot(&deltaUV1, &deltaUV) * deltaUV;

                  deltaUV2 = m_TexCoords[adjTriangle->uvIndices[indexB3]]->uv - m_TexCoords[triangle->uvIndices[j]]->uv;
                  deltaUV2 -= D3DXVec2Dot(&deltaUV2, &deltaUV) * deltaUV;

                  FLOAT dotUV = D3DXVec2Dot(&deltaUV1, &deltaUV2);
                  if (dotUV >= 0.0f)
                  {
                    // Mirror mapping, ugh, UVs cannot be considered as shared for chamfering
                    triangle->edges[j]->uvsAreShared = false;
                  }
                }
              }
              else
              {
                triangle->edges[j]->uvsAreShared = false;
              }

              // SC: Note: In MAX, edge visibility can be determined by the user, but since we don't have
              //           that information, we'll assume that all the edges that are shared by two 
              //           coplanar faces are invisible, all the others are visible.
              FLOAT dot = D3DXVec3Dot(&triangle->normal, &adjTriangle->normal);
              if (dot >= CHAMFER_VISIBLE_THRESHOLD)
              {
                triangle->edges[j]->isVisible = false;
              }

              // Are we applying chamfer to that edge?
              if ((dot < m_ChamferThresholds[triangle->elementIndex]) &&
                  (dot < m_ChamferThresholds[adjTriangle->elementIndex]))
              {
                triangle->edges[j]->doChamfer = true;

                foundOne = true;
              }

              // Add the edge to the adjacent triangle
              if (((indexB1 == 0) && (indexB2 == 1)) || ((indexB1 == 1) && (indexB2 == 0)))
                adjTriangle->edges[0] = triangle->edges[j];
              else if (((indexB1 == 1) && (indexB2 == 2)) || ((indexB1 == 2) && (indexB2 == 1)))
                adjTriangle->edges[1] = triangle->edges[j];
              else
                adjTriangle->edges[2] = triangle->edges[j];

              // Add the edge to both vertices
              ChamferNode*   node;
              ChamferVertex* vertex1 = m_Vertices[triangle->edges[j]->halfEdge[0].vertexIndex];
              node           = m_NodeAllocator.Allocate();
              node->index    = edgeId;
              node->next     = vertex1->edges;
              vertex1->edges = node;

              ChamferVertex* vertex2 = m_Vertices[triangle->edges[j]->halfEdge[1].vertexIndex];
              node           = m_NodeAllocator.Allocate();
              node->index    = edgeId;
              node->next     = vertex2->edges;
              vertex2->edges = node;

              // Do not allow the edge to be chamfered if any of the triangles is locked
              if (triangle->locked || adjTriangle->locked)
              {
                triangle->edges[j]->doChamfer = false;
              }

              break;
            }
          }

          faceNode = faceNode->next;
        }

        if (triangle->edges[j] == NULL)
        {
          ULONG edgeId = PushEdge();

          triangle->edges[j] = m_Edges[edgeId];

          triangle->edges[j]->faceIndices[0] = i;
          triangle->edges[j]->faceIndices[1] = INVALID_ID;

          triangle->edges[j]->halfEdge[0].vertexIndex = triangle->edges[j]->halfEdge[0].cutVertexIndex = triangle->vertexIndices[indexA1];
          triangle->edges[j]->halfEdge[1].vertexIndex = triangle->edges[j]->halfEdge[1].cutVertexIndex = triangle->vertexIndices[indexA2];

          triangle->edges[j]->halfEdge[0].uvIndices[0] = triangle->edges[j]->halfEdge[0].cutUVIndices[0] = triangle->uvIndices[indexA1];
          triangle->edges[j]->halfEdge[1].uvIndices[0] = triangle->edges[j]->halfEdge[1].cutUVIndices[0] = triangle->uvIndices[indexA2];

          // Should be set to invalid since there is no B face, but giving them the same values as A to
          // avoid having to do validity check on faces in all functions
          triangle->edges[j]->halfEdge[0].uvIndices[1] = triangle->edges[j]->halfEdge[0].cutUVIndices[1] = triangle->uvIndices[indexA1];
          triangle->edges[j]->halfEdge[1].uvIndices[1] = triangle->edges[j]->halfEdge[1].cutUVIndices[1] = triangle->uvIndices[indexA2];

          triangle->edges[j]->uvsAreShared = true;

          triangle->edges[j]->doChamfer = false;

          // Boundary edge...
          triangle->edges[j]->isBoundary = true;
          m_Vertices[triangle->vertexIndices[indexA1]]->isBoundary = true;
          m_Vertices[triangle->vertexIndices[indexA2]]->isBoundary = true;

          // Add the edge to both vertices
          ChamferNode*   node;
          ChamferVertex* vertex1 = m_Vertices[triangle->edges[j]->halfEdge[0].vertexIndex];
          node           = m_NodeAllocator.Allocate();
          node->index    = edgeId;
          node->next     = vertex1->edges;
          vertex1->edges = node;

          ChamferVertex* vertex2 = m_Vertices[triangle->edges[j]->halfEdge[1].vertexIndex];
          node           = m_NodeAllocator.Allocate();
          node->index    = edgeId;
          node->next     = vertex2->edges;
          vertex2->edges = node;
        }
      }
    }

    return foundOne;
  }

  void ChamferDatabase::ValidateChamfer(void)
  {
    ULONG nbEdges    = m_Edges.size();
    ULONG nbVertices = m_Vertices.size();
    ULONG i;
    ULONG j;

     // Do not allow chamfer to push edges that are too small
    for (i = 0; i < nbEdges; ++i)
    {
      ChamferEdge* edge = m_Edges[i];

      if (edge->doChamfer)
      {
        for (j = 0; j < 2; ++j)
        {
          ChamferVertex* vertex   = m_Vertices[edge->halfEdge[j].vertexIndex];
          ChamferNode*   edgeNode = vertex->edges;

          while (edgeNode != NULL)
          {
            if (edgeNode->index != i)
            {
              ChamferEdge* edge2 = m_Edges[edgeNode->index];

              FLOAT maxLen;

              if (edge2->faceIndices[0] == INVALID_ID)
              {
                maxLen = m_ChamferLengths[m_Triangles[edge2->faceIndices[1]]->elementIndex];
              }
              else if (edge2->faceIndices[1] == INVALID_ID)
              {
                maxLen = m_ChamferLengths[m_Triangles[edge2->faceIndices[0]]->elementIndex];
              }
              else
              {
                maxLen = Max(m_ChamferLengths[m_Triangles[edge2->faceIndices[0]]->elementIndex], 
                             m_ChamferLengths[m_Triangles[edge2->faceIndices[1]]->elementIndex]);
              }

              D3DXVECTOR3 delta = m_Vertices[edge2->halfEdge[0].vertexIndex]->position - 
                                  m_Vertices[edge2->halfEdge[1].vertexIndex]->position;
              FLOAT       len   = D3DXVec3Length(&delta);

              if (len < (1.5f * maxLen))
              {
                edge->doChamfer = false;
                break;
              }
            }

            edgeNode = edgeNode->next;
          }

          if (!edge->doChamfer)
            break;
        }
      }
    }
  }

  void ChamferDatabase::ComputeEdgeCutLocations(void)
  {
    ULONG nbTriangles = m_Triangles.size();
    ULONG i;
    ULONG j;

    for (i = 0; i < nbTriangles; ++i)
    {
      ChamferTriangle* triangle = m_Triangles[i];

      // Basic edge chamfer
      for (j = 0; j < 3; ++j)
      {
        if (triangle->edges[j]->doChamfer)
        {
          FLOAT cutLocation;
          FLOAT dot;
          D3DXVECTOR3 expandDir;
          D3DXVECTOR3 workEdgeVec;
          D3DXVECTOR3 edgeDir = m_Vertices[triangle->edges[j]->halfEdge[0].vertexIndex]->position - 
                                m_Vertices[triangle->edges[j]->halfEdge[1].vertexIndex]->position;

          D3DXVec3Normalize(&edgeDir, &edgeDir);
          D3DXVec3Cross(&expandDir, &triangle->normal, &edgeDir);
          D3DXVec3Normalize(&expandDir, &expandDir);

          // Previous edge, vertex B
          workEdgeVec = m_Vertices[triangle->vertexIndices[j]]->position - 
                        m_Vertices[triangle->vertexIndices[(j + 2) % 3]]->position;
          dot         = fabsf(D3DXVec3Dot(&expandDir, &workEdgeVec));
          if (dot > MESH_BUILDER_EPSILON)
            cutLocation = m_ChamferLengths[triangle->elementIndex] / dot;
          else
            cutLocation = MESH_BUILDER_EPSILON;

          if (cutLocation > 0.0f)
          {
            ChamferHalfEdge& halfEdge1 = triangle->GetHalfEdge((j + 2) % 3, j);
            halfEdge1.cutVertex = true;
            if (halfEdge1.cutLength == 0.0f)
            {
              halfEdge1.cutLength = cutLocation;
            }
            else if (halfEdge1.cutLength > cutLocation)
            {
            halfEdge1.cutLength = cutLocation;
            }
          }

          // Next edge, vertex A
          workEdgeVec = m_Vertices[triangle->vertexIndices[(j + 2) % 3]]->position - 
                        m_Vertices[triangle->vertexIndices[(j + 1) % 3]]->position;
          dot         = fabsf(D3DXVec3Dot(&expandDir, &workEdgeVec));
          if (dot > MESH_BUILDER_EPSILON)
            cutLocation = m_ChamferLengths[triangle->elementIndex] / dot;
          else
            cutLocation = MESH_BUILDER_EPSILON;

          if (cutLocation > 0.0f)
          {
            ChamferHalfEdge& halfEdge2 = triangle->GetHalfEdge((j + 1) % 3, (j + 1) % 3);
            halfEdge2.cutVertex = true;
            if (halfEdge2.cutLength == 0.0f)
            {
              halfEdge2.cutLength = cutLocation;
            }
            else if (halfEdge2.cutLength > cutLocation)
            {
              halfEdge2.cutLength = cutLocation;
            }
          }
        }
      }

      // Corner chamfer
      for (j = 0; j < 3; ++j)
      {
        ChamferEdge* edge1 = triangle->edges[j];
        ChamferEdge* edge2 = triangle->edges[(j + 2) % 3];

        if (edge1->doChamfer && edge2->doChamfer)
        {
          // We must create a new vertex

          // Interpolate the position
          D3DXVECTOR3 v1 = m_Vertices[triangle->vertexIndices[(j + 1) % 3]]->position -
                           m_Vertices[triangle->vertexIndices[j]]->position;
          D3DXVECTOR3 v2 = m_Vertices[triangle->vertexIndices[(j + 2) % 3]]->position -
                           m_Vertices[triangle->vertexIndices[j]]->position;

          D3DXVECTOR3 v1p;
          D3DXVECTOR3 v2p;
          D3DXVec3Normalize(&v1p, &v1);
          D3DXVec3Normalize(&v2p, &v2);
          v2p += v1p;
          D3DXVec3Normalize(&v2p, &v2p);

          D3DXVECTOR3 vTemp;
          D3DXVec3Cross(&vTemp, &v1p, &v2p);

          FLOAT projDist     = m_ChamferLengths[triangle->elementIndex] / D3DXVec3Length(&vTemp);
          FLOAT projDistMax  = 2.0f / ( (1.0f / D3DXVec3Dot(&v1, &v2p)) + (1.0f / D3DXVec3Dot(&v2, &v2p)) );
          FLOAT uvPercent    = projDist / projDistMax;
          projDistMax       *= 0.95f;

          if (uvPercent > 0.95f)
            uvPercent = 1.0f;

          if (projDist > projDistMax)
            projDist = projDistMax;

          triangle->positionInterpolators[j] = m_Vertices[triangle->vertexIndices[j]]->position + (projDist * v2p);
          triangle->newVertexIndices[j]      = PushPosition(triangle->positionInterpolators[j]);

          // Interpolate the texture coordinates
          D3DXVECTOR2 uv1Delta = m_TexCoords[triangle->uvIndices[(j + 1) % 3]]->uv -
                                 m_TexCoords[triangle->uvIndices[j]]->uv;
          D3DXVECTOR2 uv2Delta = m_TexCoords[triangle->uvIndices[(j + 2) % 3]]->uv -
                                 m_TexCoords[triangle->uvIndices[j]]->uv;

          D3DXVECTOR2 uv1Dir;
          D3DXVECTOR2 uv2Dir;
          D3DXVec2Normalize(&uv1Dir, &uv1Delta);
          D3DXVec2Normalize(&uv2Dir, &uv2Delta);
          uv2Dir += uv1Dir;
          D3DXVec2Normalize(&uv2Dir, &uv2Dir);

          projDist = (2.0f * uvPercent) / ( (1.0f / D3DXVec2Dot(&uv1Delta, &uv2Dir)) + (1.0f / D3DXVec2Dot(&uv2Delta, &uv2Dir)) );

          triangle->uvInterpolators[j] = m_TexCoords[triangle->uvIndices[j]]->uv + (projDist * uv2Dir);
          triangle->newUVIndices[j]    = PushUV(triangle->uvInterpolators[j]);
        }
      }
    }
  }

  void ChamferDatabase::ComputeCornerPoints(void)
  {
    ULONG nbEdges = m_Edges.size();
    ULONG i;

    // Corner poitns detection
    for (i = 0; i < nbEdges; ++i)
    {
      ChamferEdge* edge = m_Edges[i];

      if (edge->doChamfer)
      {
        ++m_Vertices[edge->halfEdge[0].vertexIndex]->nbChamferedEdges;
        ++m_Vertices[edge->halfEdge[1].vertexIndex]->nbChamferedEdges;
      }
    }

    // Cut the edges from corner points
    for (i = 0; i < nbEdges; ++i)
    {
      ChamferEdge* edge = m_Edges[i];

      D3DXVECTOR3 delta = m_Vertices[edge->halfEdge[1].vertexIndex]->position -
                          m_Vertices[edge->halfEdge[0].vertexIndex]->position;
      FLOAT deltaLength = D3DXVec3Length(&delta);

      FLOAT chamferLength;
      if (edge->faceIndices[0] != 0xffffffff)
      {
          chamferLength = m_ChamferLengths[m_Triangles[edge->faceIndices[0]]->elementIndex];
      }
      if (edge->faceIndices[1] != 0xffffffff)
      {
          chamferLength = Max(chamferLength, m_ChamferLengths[m_Triangles[edge->faceIndices[1]]->elementIndex]);
      }

      if (edge->doChamfer && (chamferLength > 0.0f))
      {
        if (m_Vertices[edge->halfEdge[0].vertexIndex]->nbChamferedEdges > 2)
        {
          if (edge->halfEdge[0].cutLength == 0.0f)
          {
            edge->halfEdge[0].cutLength = chamferLength / deltaLength;
            edge->halfEdge[0].cutVertex = true;
          }
        }

        if (m_Vertices[edge->halfEdge[1].vertexIndex]->nbChamferedEdges > 2)
        {
          if (edge->halfEdge[1].cutLength == 0.0f)
          {
            edge->halfEdge[1].cutLength = chamferLength / deltaLength;
            edge->halfEdge[1].cutVertex = true;
          }
        }
      }
      else if (chamferLength > 0.0f)
      {
        if (m_Vertices[edge->halfEdge[0].vertexIndex]->nbChamferedEdges > 0)
        {
          if (edge->halfEdge[0].cutLength == 0.0f)
          {
            edge->halfEdge[0].cutLength = chamferLength / deltaLength;
            edge->halfEdge[0].cutVertex = true;
          }
        }

        if (m_Vertices[edge->halfEdge[1].vertexIndex]->nbChamferedEdges > 0)
        {
          if (edge->halfEdge[1].cutLength == 0.0f)
          {
            edge->halfEdge[1].cutLength = chamferLength / deltaLength;
            edge->halfEdge[1].cutVertex = true;
          }
        }
      }
    }
  }

  void ChamferDatabase::ComputeEdgeIndices(void)
  {
    const D3DXVECTOR3 DEBUG_POSITION(1e10f, 1e10f, 1e10f);
    const D3DXVECTOR2 DEBUG_UV(-1.0f, -1.0f);

    ULONG nbEdges = m_Edges.size();
    ULONG i;

    for (i = 0; i < nbEdges; ++i)
    {
      ChamferEdge* edge = m_Edges[i];

      if (edge->halfEdge[0].cutVertex)
        edge->halfEdge[0].cutVertexIndex = PushPosition(DEBUG_POSITION);
      else
        edge->halfEdge[0].cutVertexIndex = edge->halfEdge[0].vertexIndex;

      if (edge->halfEdge[1].cutVertex)
        edge->halfEdge[1].cutVertexIndex = PushPosition(DEBUG_POSITION);
      else
        edge->halfEdge[1].cutVertexIndex = edge->halfEdge[1].vertexIndex;

      // Make sure the cuts are valid
      if (edge->halfEdge[0].cutLength + edge->halfEdge[1].cutLength >= 1.0f)
      {
        if ((edge->halfEdge[0].cutVertexIndex != edge->halfEdge[0].vertexIndex) &&
            (edge->halfEdge[1].cutVertexIndex != edge->halfEdge[1].vertexIndex))
        {
          edge->halfEdge[0].cutVertexIndex = edge->halfEdge[1].cutVertexIndex;
          edge->halfEdge[0].cutLength = edge->halfEdge[1].cutLength = 0.5f;
        }
        else if (edge->halfEdge[0].cutVertexIndex != edge->halfEdge[0].vertexIndex)
        {
          edge->halfEdge[0].cutVertexIndex = edge->halfEdge[1].vertexIndex;
          edge->halfEdge[0].cutLength      = 1.0f;
        }
        else if (edge->halfEdge[1].cutVertexIndex != edge->halfEdge[1].vertexIndex)
        {
          edge->halfEdge[1].cutVertexIndex = edge->halfEdge[0].vertexIndex;
          edge->halfEdge[1].cutLength      = 1.0f;
        }
      }

      // Texture coordinates

      if (edge->halfEdge[0].cutVertexIndex != edge->halfEdge[0].vertexIndex)
      {
        if (edge->uvsAreShared)
        {
          edge->halfEdge[0].cutUVIndices[0] = edge->halfEdge[0].cutUVIndices[1] = PushUV(DEBUG_UV);
        }
        else
        {
          edge->halfEdge[0].cutUVIndices[0] = PushUV(DEBUG_UV);
          edge->halfEdge[0].cutUVIndices[1] = PushUV(DEBUG_UV);
        }
      }

      if (edge->halfEdge[1].cutVertexIndex != edge->halfEdge[1].vertexIndex)
      {
        if (edge->uvsAreShared)
        {
          edge->halfEdge[1].cutUVIndices[0] = edge->halfEdge[1].cutUVIndices[1] = PushUV(DEBUG_UV);
        }
        else
        {
          edge->halfEdge[1].cutUVIndices[0] = PushUV(DEBUG_UV);
          edge->halfEdge[1].cutUVIndices[1] = PushUV(DEBUG_UV);
        }
      }
    }
  }

  void ChamferDatabase::ApplyQuadCorrection(void)
  {
    ULONG nbTriangles = m_Triangles.size();
    ULONG i;
    ULONG j;

    for (i = 0; i < nbTriangles; ++i)
    {
      ChamferTriangle* triangle = m_Triangles[i];

      for (j = 0; j < 3; ++j)
      {
        // Seems like a hack, but prevents vertices shared by multiple non-aligned triangles from
        // moving thus causing the mesh to fold over itself or creates unwanted concave regions
        if (m_Vertices[triangle->vertexIndices[j]]->nbChamferedEdges > 2)
          continue;

        if (!triangle->edges[j]->doChamfer && !triangle->edges[(j + 2) % 3]->doChamfer)
        {
          if (!triangle->edges[j]->isVisible           &&
               triangle->edges[(j + 2) % 3]->isVisible &&
               triangle->edges[j]->uvsAreShared)
          {
            ChamferHalfEdge& halfEdge1 = triangle->GetHalfEdge(j, j);
            ChamferHalfEdge& halfEdge2 = triangle->GetHalfEdge((j + 2) % 3, j);

            halfEdge1.cutVertexIndex  = halfEdge2.cutVertexIndex;
            halfEdge1.lineFlags       = 1;
            halfEdge1.cutUVIndices[0] = halfEdge2.cutUVIndices[0];
            halfEdge1.cutUVIndices[1] = halfEdge2.cutUVIndices[1];
          }

          if ( triangle->edges[j]->isVisible           &&
              !triangle->edges[(j + 2) % 3]->isVisible &&
               triangle->edges[(j + 2) % 3]->uvsAreShared)
          {
            ChamferHalfEdge& halfEdge1 = triangle->GetHalfEdge((j + 2) % 3, j);
            ChamferHalfEdge& halfEdge2 = triangle->GetHalfEdge(j, j);

            halfEdge1.cutVertexIndex  = halfEdge2.cutVertexIndex;
            halfEdge1.lineFlags       = 1;
            halfEdge1.cutUVIndices[0] = halfEdge2.cutUVIndices[0];
            halfEdge1.cutUVIndices[1] = halfEdge2.cutUVIndices[1];
          }

          if (!triangle->edges[j]->isVisible           &&
              !triangle->edges[(j + 2) % 3]->isVisible &&
               triangle->edges[j]->uvsAreShared        &&
               triangle->edges[(j + 2) % 3]->uvsAreShared)
          {
            ChamferHalfEdge& halfEdge1 = triangle->GetHalfEdge((j + 2) % 3, j);
            ChamferHalfEdge& halfEdge2 = triangle->GetHalfEdge(j, j);

            if ((halfEdge1.lineFlags & 0x2) == 0)
            {
              halfEdge1.cutVertexIndex  = halfEdge2.cutVertexIndex;
              halfEdge1.lineFlags      |= 0x2;
              halfEdge2.lineFlags      |= 0x2;
              halfEdge1.cutUVIndices[0] = halfEdge2.cutUVIndices[0];
              halfEdge1.cutUVIndices[1] = halfEdge2.cutUVIndices[1];
            }
          }
        }
      }
    }
  }

  void ChamferDatabase::UpdateVertexIndices(void)
  {
    ULONG nbTriangles = m_Triangles.size();
    ULONG nbEdges     = m_Edges.size();
    ULONG i;
    ULONG j;

    // Vertex index matching
    for (i = 0; i < nbTriangles; ++i)
    {
      ChamferTriangle* triangle = m_Triangles[i];

      for (j = 0; j < 3; ++j)
      {
        if (triangle->newVertexIndices[j] == triangle->vertexIndices[j])
        {
          if (triangle->edges[j]->doChamfer)
          {
            ChamferHalfEdge& halfEdge     = triangle->GetHalfEdge((j + 2) % 3, j);
            triangle->newVertexIndices[j] = halfEdge.cutVertexIndex;
            triangle->newUVIndices[j]     = triangle->GetCutUVIndex((j + 2) % 3, j);
          }
          else
          {
            ChamferHalfEdge& halfEdge     = triangle->GetHalfEdge(j, j);
            triangle->newVertexIndices[j] = halfEdge.cutVertexIndex;
            triangle->newUVIndices[j]     = triangle->GetCutUVIndex(j, j);
          }
        }

        if (triangle->newVertexIndices[j] == triangle->vertexIndices[j])
        {
          ChamferHalfEdge& halfEdge     = triangle->GetHalfEdge((j + 2) % 3, j);
          triangle->newVertexIndices[j] = halfEdge.cutVertexIndex;
          triangle->newUVIndices[j]     = triangle->GetCutUVIndex((j + 2) % 3, j);
        }
        if (triangle->newVertexIndices[j] == triangle->vertexIndices[j])
        {
          ChamferHalfEdge& halfEdge     = triangle->GetHalfEdge(j, j);
          triangle->newVertexIndices[j] = halfEdge.cutVertexIndex;
          triangle->newUVIndices[j]     = triangle->GetCutUVIndex(j, j);
        }
      }
    }

    // Compute the new points and texture coordinates
    for (i = 0; i < nbTriangles; ++i)
    {
      ChamferTriangle* triangle = m_Triangles[i];

      for (j = 0; j < 3; ++j)
      {
        if (triangle->edges[j]->doChamfer && triangle->edges[(j + 2) % 3]->doChamfer)
        {
          m_Vertices[triangle->newVertexIndices[j]]->position = triangle->positionInterpolators[j];
          m_TexCoords[triangle->newUVIndices[j]]->uv          = triangle->uvInterpolators[j];
        }
      }
    }

    // Edges
    for (i = 0; i < nbEdges; ++i)
    {
      ChamferEdge* edge = m_Edges[i];

      if (edge->halfEdge[0].cutVertexIndex != edge->halfEdge[0].vertexIndex)
      {
        if ((edge->halfEdge[0].lineFlags & 0x1) == 0)
        {
          m_Vertices[edge->halfEdge[0].cutVertexIndex]->position = m_Vertices[edge->halfEdge[0].vertexIndex]->position + 
            (m_Vertices[edge->halfEdge[1].vertexIndex]->position - m_Vertices[edge->halfEdge[0].vertexIndex]->position) * edge->halfEdge[0].cutLength;

          m_TexCoords[edge->halfEdge[0].cutUVIndices[0]]->uv = m_TexCoords[edge->halfEdge[0].uvIndices[0]]->uv + 
            (m_TexCoords[edge->halfEdge[1].uvIndices[0]]->uv - m_TexCoords[edge->halfEdge[0].uvIndices[0]]->uv) * edge->halfEdge[0].cutLength;
          m_TexCoords[edge->halfEdge[0].cutUVIndices[1]]->uv = m_TexCoords[edge->halfEdge[0].uvIndices[1]]->uv + 
            (m_TexCoords[edge->halfEdge[1].uvIndices[1]]->uv - m_TexCoords[edge->halfEdge[0].uvIndices[1]]->uv) * edge->halfEdge[0].cutLength;
        }
      }

      if (edge->halfEdge[1].cutVertexIndex != edge->halfEdge[1].vertexIndex)
      {
        if ((edge->halfEdge[1].lineFlags & 0x1) == 0)
        {
          m_Vertices[edge->halfEdge[1].cutVertexIndex]->position = m_Vertices[edge->halfEdge[1].vertexIndex]->position + 
            (m_Vertices[edge->halfEdge[0].vertexIndex]->position - m_Vertices[edge->halfEdge[1].vertexIndex]->position) * edge->halfEdge[1].cutLength;

          m_TexCoords[edge->halfEdge[1].cutUVIndices[0]]->uv = m_TexCoords[edge->halfEdge[1].uvIndices[0]]->uv + 
            (m_TexCoords[edge->halfEdge[0].uvIndices[0]]->uv - m_TexCoords[edge->halfEdge[1].uvIndices[0]]->uv) * edge->halfEdge[1].cutLength;
          m_TexCoords[edge->halfEdge[1].cutUVIndices[1]]->uv = m_TexCoords[edge->halfEdge[1].uvIndices[1]]->uv + 
            (m_TexCoords[edge->halfEdge[0].uvIndices[1]]->uv - m_TexCoords[edge->halfEdge[1].uvIndices[1]]->uv) * edge->halfEdge[1].cutLength;
        }
      }
    }
  }

  void ChamferDatabase::CollapseVertices(void)
  {
    ULONG nbTriangles = m_Triangles.size();
    ULONG nbVertices  = m_Vertices.size();
    ULONG i;
    ULONG j;

    // SC: TODO: Need to handle the case where the original vertices are boundaries and are collapsed on the 
    //           cut vertices thus causing stretches in the texture coordinates

    for (i = 0; i < nbTriangles; ++i)
    {
      ChamferTriangle* triangle = m_Triangles[i];

      for (j = 0; j < 3; ++j)
      {
        if (triangle->newVertexIndices[j] != triangle->vertexIndices[j])
        {
          ULONG cutVertexIndex;

          cutVertexIndex = triangle->GetCutVertexIndex(j, j);
          if ((cutVertexIndex != triangle->newVertexIndices[j]) &&
              (cutVertexIndex != triangle->vertexIndices[j]))
          {
            // Cut vertex = average of the new vertices

            ChamferVertex*   cutVertex = m_Vertices[cutVertexIndex];
            ChamferTriangle* triangles[2];
            ULONG            indices[2];

            if (cutVertex->vertexSnap == SNAP_NONE)
            {
              triangles[0] = m_Triangles[triangle->edges[j]->faceIndices[0]];
              triangles[1] = m_Triangles[triangle->edges[j]->faceIndices[1]];
              indices[0]   = TriangleFindVertex(triangles[0], triangle->vertexIndices[j]);
              indices[1]   = TriangleFindVertex(triangles[1], triangle->vertexIndices[j]);

              D3DXVECTOR3 newPosition = m_Vertices[triangles[0]->newVertexIndices[indices[0]]]->position;
              newPosition            += m_Vertices[triangles[1]->newVertexIndices[indices[1]]]->position;
              newPosition            *= 0.5f;

              cutVertex->position   = newPosition;
              cutVertex->vertexSnap = SNAP_MIDPOINT;
            }
          }
          else if (m_Vertices[triangle->vertexIndices[j]]->nbChamferedEdges > 1)
          {
            ChamferVertex* oldVertex = m_Vertices[triangle->vertexIndices[j]];

            // New position should be the average of all cut neighbors

            if (oldVertex->vertexSnap == SNAP_NONE)
            {
              oldVertex->vertexSnap = SNAP_AVERAGE;
            }
          }
          else if ((triangle->edges[j]->faceIndices[0] != 0xffffffff) &&
                   (triangle->edges[j]->faceIndices[1] != 0xffffffff))
          {
            // Must move the original vertex as average of the new vertices

            ChamferTriangle* triangles[2];
            ULONG            indices[2];

            triangles[0] = m_Triangles[triangle->edges[j]->faceIndices[0]];
            triangles[1] = m_Triangles[triangle->edges[j]->faceIndices[1]];
            indices[0]   = TriangleFindVertex(triangles[0], triangle->vertexIndices[j]);
            indices[1]   = TriangleFindVertex(triangles[1], triangle->vertexIndices[j]);

            if ((triangles[0]->newVertexIndices[indices[0]] != triangles[1]->newVertexIndices[indices[1]]) &&
                (triangles[0]->newVertexIndices[indices[0]] != triangles[0]->vertexIndices[indices[0]])    &&
                (triangles[1]->newVertexIndices[indices[1]] != triangles[1]->vertexIndices[indices[1]]))
            {
              ChamferVertex* oldVertex = m_Vertices[triangle->vertexIndices[j]];

              if (oldVertex->vertexSnap == SNAP_NONE)
              {
                D3DXVECTOR3 newPosition = m_Vertices[triangles[0]->newVertexIndices[indices[0]]]->position;
                newPosition            += m_Vertices[triangles[1]->newVertexIndices[indices[1]]]->position;
                newPosition            *= 0.5f;

                oldVertex->position   = newPosition;
                oldVertex->vertexSnap = SNAP_MIDPOINT;
              }
            }
          }
        }
      }
    }

    for (i = 0; i < nbVertices; ++i)
    {
      ChamferVertex* vertex = m_Vertices[i];

      if (vertex->vertexSnap != SNAP_AVERAGE)
        continue;

      ChamferNode* edgeNode = vertex->edges;

      D3DXVECTOR3 newPos(0.0f, 0.0f, 0.0f);
      ULONG       nbPos = 0;

      while (edgeNode != NULL)
      {
        ChamferEdge* edge = m_Edges[edgeNode->index];
        ULONG        heIndex;

        if (edge->halfEdge[0].vertexIndex == i)
          heIndex = 0;
        else
          heIndex = 1;

        if (edge->halfEdge[heIndex].cutVertexIndex != edge->halfEdge[heIndex].vertexIndex)
        {
          newPos += m_Vertices[edge->halfEdge[heIndex].cutVertexIndex]->position;
          ++nbPos;
        }

        edgeNode = edgeNode->next;
      }

      if (nbPos > 0)
      {
        vertex->position  = newPos;
        vertex->position *= 1.0f / (FLOAT)nbPos;
      }
    }
  }

  void ChamferDatabase::GenerateTriangles(TriangleDatabase* _triangleDB)
  {
#define M_AddTriangle(_i1, _uv1, _i2, _uv2, _i3, _uv3)                             \
        _triangleDB->AddTriangle(triangle->elementIndex,                           \
                                 m_Vertices[_i1]->position, m_TexCoords[_uv1]->uv, \
                                 m_Vertices[_i2]->position, m_TexCoords[_uv2]->uv, \
                                 m_Vertices[_i3]->position, m_TexCoords[_uv3]->uv, \
                                 triangle->normal)

    ULONG nbTriangles = m_Triangles.size();
    ULONG i;
    ULONG j;

    for (i = 0; i < nbTriangles; ++i)
    {
      ChamferTriangle* triangle = m_Triangles[i];

      // Main triangle
      M_AddTriangle(triangle->newVertexIndices[0], triangle->newUVIndices[0],
                    triangle->newVertexIndices[1], triangle->newUVIndices[1],
                    triangle->newVertexIndices[2], triangle->newUVIndices[2]);

      // Corners and inner edges
      for (j = 0; j < 3; ++j)
      {
        // Corners
        M_AddTriangle(triangle->vertexIndices[j],                  triangle->uvIndices[j],
                      triangle->GetCutVertexIndex(j, j),           triangle->GetCutUVIndex(j, j),
                      triangle->GetCutVertexIndex((j + 2) % 3, j), triangle->GetCutUVIndex((j + 2) % 3, j));
        M_AddTriangle(triangle->GetCutVertexIndex((j + 2) % 3, j), triangle->GetCutUVIndex((j + 2) % 3, j),
                      triangle->GetCutVertexIndex(j, j),           triangle->GetCutUVIndex(j, j),
                      triangle->newVertexIndices[j],               triangle->newUVIndices[j]);

        // Inner edges
        M_AddTriangle(triangle->GetCutVertexIndex(j, j),           triangle->GetCutUVIndex(j, j),
                      triangle->GetCutVertexIndex(j, (j + 1) % 3), triangle->GetCutUVIndex(j, (j + 1) % 3),
                      triangle->newVertexIndices[j],               triangle->newUVIndices[j]);
        M_AddTriangle(triangle->newVertexIndices[j],               triangle->newUVIndices[j],
                      triangle->GetCutVertexIndex(j, (j + 1) % 3), triangle->GetCutUVIndex(j, (j + 1) % 3),
                      triangle->newVertexIndices[(j + 1) % 3],     triangle->newUVIndices[(j + 1) % 3]);
      }
    }

#undef M_AddTriangle
  }

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - TriangleDatabase
// ------------------------------------------------------------------------------------------------

  TriangleDatabase::TriangleDatabase(void)
  {
  }

  TriangleDatabase::~TriangleDatabase(void)
  {
  }

  void TriangleDatabase::AddTriangle(ULONG _elementIndex,
                                     const D3DXVECTOR3& _p1, const D3DXVECTOR2& _uv1, 
                                     const D3DXVECTOR3& _p2, const D3DXVECTOR2& _uv2, 
                                     const D3DXVECTOR3& _p3, const D3DXVECTOR2& _uv3, 
                                     const D3DXVECTOR3& _normal)
  {
    Triangle triangle;

    triangle.elementIndex       = _elementIndex;
    triangle.positionIndices[0] = AddPosition(_p1);
    triangle.positionIndices[1] = AddPosition(_p2);
    triangle.positionIndices[2] = AddPosition(_p3);

    if (IsTriangleValid(triangle, _normal))
    {
      triangle.uvIndices[0]           = AddUV(_uv1);
      triangle.colorWeightsIndices[0] = AddColorWeights(_p1, _elementIndex);
      triangle.uvIndices[1]           = AddUV(_uv2);
      triangle.colorWeightsIndices[1] = AddColorWeights(_p2, _elementIndex);
      triangle.uvIndices[2]           = AddUV(_uv3);
      triangle.colorWeightsIndices[2] = AddColorWeights(_p3, _elementIndex);

      m_Triangles.push_back(triangle);
    }
  }

  ULONG TriangleDatabase::AddPosition(const D3DXVECTOR3& _pos)
  {
    ULONG nbPositions = m_Positions.size();
    ULONG i;

    for (i = 0; i < nbPositions; ++i)
    {
      if (IsPositionEqual(_pos.x, _pos.y, _pos.z, m_Positions[i].x, m_Positions[i].y, m_Positions[i].z, CHAMFER_POSITION_DB_EPSILON))
        return i;
    }

    m_Positions.push_back(_pos);

    return nbPositions;
  }

  ULONG TriangleDatabase::AddUV(const D3DXVECTOR2& _uv)
  {
    ULONG nbUVs = m_TexCoords.size();
    ULONG i;

    for (i = 0; i < nbUVs; ++i)
    {
      if (IsTexCoordEqual(_uv.x, _uv.y, m_TexCoords[i].x, m_TexCoords[i].y, CHAMFER_UV_EPSILON))
        return i;
    }

    m_TexCoords.push_back(_uv);

    return nbUVs;
  }

  ULONG TriangleDatabase::AddColorWeights(const D3DXVECTOR3& _position, ULONG _elementIndex)
  {
    std::vector<D3DXVECTOR3>      positions;
    std::vector<StaticVertexData> vertexData;
    std::vector<float>            squareDistances;

    D3DXCOLOR newColor(0.0f, 0.0f, 0.0f, 0.0f);
    D3DXCOLOR newWeights(0.0f, 0.0f, 0.0f, 0.0f);

    FLOAT maxSqSearchDist = m_SearchDistances[_elementIndex] * m_SearchDistances[_elementIndex];
    ULONG i;
    FLOAT weight = 0.0f;
    ULONG nbRes  = (ULONG)m_Trees[_elementIndex]->FindNeighbors(_position, m_SearchDistances[_elementIndex], positions, 
                                                                vertexData, &squareDistances);
    for (i = 0; i < nbRes; ++i)
    {
      FLOAT vertexWeight     = maxSqSearchDist - squareDistances[i];
      vertexData[i].color   *= vertexWeight;
      vertexData[i].weights *= vertexWeight;
      newColor              += vertexData[i].color;
      newWeights            += vertexData[i].weights;
      weight                += vertexWeight;
    }

    if (weight > 0.0f)
    {
      FLOAT weightLen = sqrtf((newWeights.r * newWeights.r) +
                              (newWeights.g * newWeights.g) +
                              (newWeights.b * newWeights.b) +
                              (newWeights.a * newWeights.a));
      if (weightLen >= MESH_BUILDER_EPSILON)
      {
        newWeights *= 1.0f / weightLen;
      }

      newColor           *= 1.0f / weight;
      newWeights         *= 1.0f / weight;
    }

    ColorWeights cw;
    cw.color   = newColor;
    cw.weights = newWeights;
    m_ColorWeights.push_back(cw);

    return m_ColorWeights.size() - 1;
  }

  bool TriangleDatabase::IsTriangleValid(const Triangle& _triangle, const D3DXVECTOR3& _normal)
  {
    ULONG nbTriangles = m_Triangles.size();
    ULONG i;

    // Degenerate?
    if ((_triangle.positionIndices[0] == _triangle.positionIndices[1]) ||
        (_triangle.positionIndices[1] == _triangle.positionIndices[2]) ||
        (_triangle.positionIndices[2] == _triangle.positionIndices[0]))
      return false;

    // Already exists?
    for (i = 0; i < nbTriangles; ++i)
    {
      if (m_Triangles[i].HasVertex(_triangle.positionIndices[0]) &&
          m_Triangles[i].HasVertex(_triangle.positionIndices[1]) &&
          m_Triangles[i].HasVertex(_triangle.positionIndices[2]))
        return false;
    }

    // Valid area?
    D3DXVECTOR3 v1 = m_Positions[_triangle.positionIndices[2]] - m_Positions[_triangle.positionIndices[0]];
    D3DXVECTOR3 v2 = m_Positions[_triangle.positionIndices[1]] - m_Positions[_triangle.positionIndices[0]];
    D3DXVECTOR3 cross;
    D3DXVec3Cross(&cross, &v1, &v2);

    FLOAT dot = D3DXVec3Dot(&cross, &cross);
    if (dot < CHAMFER_TRIANGLE_AREA_EPSILON)
      return false;

    return true;
  }

  void TriangleDatabase::SetKDTree(ULONG _elementIndex, StaticVertexDataKDTree* _tree, FLOAT _searchDistance)
  {
    m_Trees[_elementIndex]           = _tree;
    m_SearchDistances[_elementIndex] = _searchDistance;
  }

  void TriangleDatabase::UpdateBuilder(MeshBuilder* _builder)
  {
    ULONG nbTriangles = m_Triangles.size();
    ULONG i;
    ULONG j;

    _builder->BeginVertices();

    for (i = 0; i < nbTriangles; ++i)
    {
      const Triangle& triangle = m_Triangles[i];

      for (j = 0; j < 3; ++j)
      {
        const D3DXVECTOR3&  position = m_Positions[triangle.positionIndices[j]];
        const D3DXVECTOR2&  uv       = m_TexCoords[triangle.uvIndices[j]];
        const ColorWeights& cw       = m_ColorWeights[triangle.colorWeightsIndices[j]];

        _builder->AddVertex(position.x, position.y, position.z, 
                            cw.weights.r, cw.weights.g, cw.weights.b, cw.weights.a,
                            (ULONG)cw.color, uv.x, uv.y, (BYTE)triangle.elementIndex);
      }
    }

    _builder->EndVertices();
  }

};

#endif // ACTIVE_EDITORS && _XENON_RENDER
