// ----------------------------------------------------------------------------
// Author : Sebastien Comte
// File   : XMP_XenonMeshBuilder.cpp
// Date   : 2005-05-28
// Descr. : 
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// HEADERS
// ----------------------------------------------------------------------------

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

// Define to have memory usage information in various processing stages
//#define _XMP_MEMORY_LOG

#include "XMP_XenonMeshBuilder.h"

// ----------------------------------------------------------------------------
// IMPLEMENTATION
// ----------------------------------------------------------------------------
namespace XenonMeshPack
{

// ----------------------------------------------------------------------------
// PRIVATE VARIABLES
// ----------------------------------------------------------------------------
  OutputFunction       s_OutputFunction       = NULL;
  ProgressTickFunction s_ProgressTickFunction = NULL;

// ----------------------------------------------------------------------------
// PRIVATE STRUCTURES
// ----------------------------------------------------------------------------

#if defined(_XMP_TRACK_MEMORY)
  struct MemoryInfo
  {
    const CHAR* file;
    INT         line;
    INT         size;
  };
  typedef std::map<ULONG, MemoryInfo> MemoryInfoMap;
#endif

  struct DisplacementCost
  {
    FLOAT displacement;
    FLOAT costMultiplier;
  };

  const DisplacementCost MESH_BUILDER_DISPLACEMENT_COSTS[] = 
  {
    { 0.0125f, 50.0f },
    { 0.0250f, 12.0f },
    { 0.0500f,  3.0f },
    { 0.0750f,  2.0f },
    { 0.1000f,  1.0f }
  };
  const ULONG MESH_BUILDER_DISPLACEMENT_COSTS_COUNT = sizeof(MESH_BUILDER_DISPLACEMENT_COSTS) / sizeof(DisplacementCost);

// ----------------------------------------------------------------------------
// PRIVATE FUNCTIONS
// ----------------------------------------------------------------------------

  struct EdgeCostCompare
  {
    bool operator() (Edge* _e1, Edge* _e2)
    {
      return _e1->cost > _e2->cost;
    }

    bool operator() (EdgePtr* _e1, EdgePtr* _e2)
    {
      if (_e1->ptr == NULL)      return false;
      else if (_e2->ptr == NULL) return true;
      else                       return _e1->ptr->cost > _e2->ptr->cost;
    }
  };

  void Log(LogLevel _level, const CHAR* _format, ...)
  {
    CHAR    message[2048];
    va_list argList;

    if (s_OutputFunction == NULL)
        return;

    // Format the string
    va_start(argList, _format);
    vsprintf(message, _format, argList);
    va_end(argList);

    s_OutputFunction(_level, message);
  }

// ----------------------------------------------------------------------------
// MEMORY MANAGEMENT
// ----------------------------------------------------------------------------

#if defined(_XMP_TRACK_MEMORY)

  static MemoryInfoMap s_MemoryTrace;
  static INT           s_AllocatedMemory = 0;
  static INT           s_PeakMemory      = 0;

  void* XMP_Alloc(ULONG _size, const CHAR* _fileName, INT _line)
  {
    MemoryInfo memoryBlock;

    void* ptr = malloc(_size);
    ERR_X_Assert((ptr != NULL) && "Out of memory while building a Xenon mesh");

    memoryBlock.file = _fileName;
    memoryBlock.line = _line;
    memoryBlock.size = _size;
    s_MemoryTrace[(ULONG)ptr] = memoryBlock;

    s_AllocatedMemory += _size;
    if (s_AllocatedMemory > s_PeakMemory)
      s_PeakMemory = s_AllocatedMemory;

    return ptr;
  }

  void* XMP_Realloc(void* _ptr, ULONG _size, const CHAR* _fileName, INT _line)
  {
    MemoryInfoMap::iterator it;
    MemoryInfo memoryBlock;

    it = s_MemoryTrace.find((ULONG)_ptr);
    if (it != s_MemoryTrace.end())
    {
      s_AllocatedMemory -= it->second.size;

      s_MemoryTrace.erase(it);
    }

    void* ptr = realloc(_ptr, _size);
    ERR_X_Assert((ptr != NULL) && "Out of memory while building a Xenon mesh");

    memoryBlock.file = _fileName;
    memoryBlock.line = _line;
    memoryBlock.size = _size;
    s_MemoryTrace[(ULONG)ptr] = memoryBlock;

    s_AllocatedMemory += _size;
    if (s_AllocatedMemory > s_PeakMemory)
      s_PeakMemory = s_AllocatedMemory;

    return ptr;
  }

  void XMP_Free(void* _ptr)
  {
    MemoryInfoMap::iterator it;

    it = s_MemoryTrace.find((ULONG)_ptr);
    if (it != s_MemoryTrace.end())
    {
      s_AllocatedMemory -= it->second.size;

      s_MemoryTrace.erase(it);
    }

    free(_ptr);
  }

  ULONG XMP_DumpMemory(void)
  {
    MemoryInfoMap::iterator it;

    ULONG peakUsage = s_PeakMemory;

    Log(LL_DEBUG, "]> XMP_DumpMemory() - Allocated memory %d - Peak memory usage %d", s_AllocatedMemory, s_PeakMemory);

    it = s_MemoryTrace.begin();
    if (it != s_MemoryTrace.end())
    {
      Log(LL_DEBUG, "]> XMP_DumpMemory() - %u allocations", (ULONG)s_MemoryTrace.size());

      while (it != s_MemoryTrace.end())
      {
        // SC: Dump the actual allocations
        //Log(LL_DEBUG, "%s(%d) - %d bytes @ 0x%08x", it->second.file, it->second.line, it->second.size, (ULONG)it->first);

        free((void*)it->first);
        ++it;
      }
    }
    Log(LL_DEBUG, "]> End of XMP_DumpMemory()");

    s_MemoryTrace.clear();
    s_AllocatedMemory = 0;
    s_PeakMemory      = 0;

    return peakUsage;
  }

  void XMP_ShowMemoryInfo(const CHAR* _context)
  {
    Log(LL_DEBUG, "]> XMP_ShowMemoryInfo() - %s - Allocated memory %d - Peak memory usage %d\n", _context, s_AllocatedMemory, s_PeakMemory);
  }

#endif

// ----------------------------------------------------------------------------
// IMPLEMENTATION - Quadric
// ----------------------------------------------------------------------------

  void Quadric::Set(const D3DXVECTOR3& _pos1, const D3DXVECTOR2& _uv1, 
                    const D3DXVECTOR3& _pos2, const D3DXVECTOR2& _uv2, 
                    const D3DXVECTOR3& _pos3, const D3DXVECTOR2& _uv3)
  {
    QVector p1;
    QVector e1, e2;

    p1(0) = _pos1.x; p1(1) = _pos1.y; p1(2) = _pos1.z; p1(3) = _uv1.x; p1(4) = _uv1.y;

    // Build a 2-vector basis
    e1(0) = _pos2.x - _pos1.x;
    e1(1) = _pos2.y - _pos1.y;
    e1(2) = _pos2.z - _pos1.z;
    e1(3) = _uv2.x - _uv1.x;
    e1(4) = _uv2.y - _uv1.y;
    XMP_NormalizeVector(&e1);

    e2(0) = _pos3.x - _pos1.x;
    e2(1) = _pos3.y - _pos1.y;
    e2(2) = _pos3.z - _pos1.z;
    e2(3) = _uv3.x - _uv1.x;
    e2(4) = _uv3.y - _uv1.y;

    QVector t(e1);
    t  *= e1.DotProduct(e2);
    e2 -= t;
    XMP_NormalizeVector(&e2);

    FLOAT p1e1 = p1.DotProduct(e1);
    FLOAT p1e2 = p1.DotProduct(e2);

    m_A.SetIdentity();
    m_A.SymmetricSubtract(e1, e1);
    m_A.SymmetricSubtract(e2, e2);

    m_B  = e1;
    m_B *= p1e1;
    t    = e2;
    t   *= p1e2;
    m_B += t;
    m_B -= p1;

    m_C = p1.DotProduct(p1) - (p1e1 * p1e1) - (p1e2 * p1e2);
  }

  FLOAT Quadric::Evaluate(const D3DXVECTOR3* _pos, const D3DXVECTOR2* _texCoord) const
  {
    QMatrix A;
    QVector v;
    QVector t1;
    FLOAT   cost;

    v(0) = _pos->x;
    v(1) = _pos->y;
    v(2) = _pos->z;
    v(3) = _texCoord->x;
    v(4) = _texCoord->y;

    m_A.MakeMatrix(A);
    XMP_TransformVector(&t1, A, v);
    cost  = v.DotProduct(t1);
    cost += 2.0f * m_B.DotProduct(v);
    cost += m_C;

    if (cost < 0.0f)
      cost = 0.0f;

    return cost;
  }

  bool Quadric::Optimize(D3DXVECTOR3* _newPos) const
  {
    QMatrix A;
    QMatrix invA;
    QVector v;
    FLOAT   det;

    m_A.MakeMatrix(A);
    det = XMP_MatrixInverse<5>(&invA, A);
    if (fabsf(det) < MESH_BUILDER_EPSILON)
      return false;

    XMP_TransformVector<5>(&v, invA, m_B);

    _newPos->x = -v(0);
    _newPos->y = -v(1);
    _newPos->z = -v(2);

    return true;
  }

// ----------------------------------------------------------------------------
// IMPLEMENTATION - MaterialInfo
// ----------------------------------------------------------------------------

  MaterialInfo::MaterialInfo(void)
    : m_MaterialId(0), m_ProcessingFlags(0), 
      m_HeightMap(NULL), m_HeightMapWidth(0), m_HeightMapHeight(0), 
      m_DisplacementOffset(0.0f), m_DisplacementMax(0.0f), m_MaxTriangleArea(1.0f),
      m_MaxSearchDistance(MESH_BUILDER_DEFAULT_KDTREE_DISTANCE),
      m_SmoothThreshold(MESH_BUILDER_PI), m_CostMultiplier(1.0f),
      m_ChamferThreshold(-1.0f), m_ChamferLength(0.05f)
  {
  }

  MaterialInfo::~MaterialInfo(void)
  {
    if (m_HeightMap != NULL)
    {
      XMP_FREE(m_HeightMap);
    }
  }

  FLOAT MaterialInfo::GetBaseDisplacement(FLOAT _u, FLOAT _v)
  {
    if (!IsDisplacementEnabled() || (m_HeightMap == NULL))
      return 0.0f;

    FLOAT transformedU = (_u * m_TexCoordTransform._11) + (_v * m_TexCoordTransform._21) + m_TexCoordTransform._41;
    FLOAT transformedV = (_u * m_TexCoordTransform._12) + (_v * m_TexCoordTransform._22) + m_TexCoordTransform._42;

    return SampleHeightMap(transformedU, transformedV);
  }

  inline FLOAT MaterialInfo::FetchTexel(LONG _u, LONG _v)
  {
    while (_u < 0)                  _u += m_HeightMapWidth;
    while (_u >= m_HeightMapWidth)  _u -= m_HeightMapWidth;
    while (_v < 0)                  _v += m_HeightMapHeight;
    while (_v >= m_HeightMapHeight) _v -= m_HeightMapHeight;

    return ((FLOAT)m_HeightMap[(_v * m_HeightMapWidth) + _u]) * MESH_BUILDER_ONE_BY_255;
  }

  inline FLOAT MaterialInfo::SampleHeightMap(FLOAT _u, FLOAT _v)
  {
    FLOAT samples[2][2];
    FLOAT fU = _u * (FLOAT)m_HeightMapWidth;
    FLOAT fV = _v * (FLOAT)m_HeightMapHeight;

    samples[0][0] = FetchTexel((LONG)floorf(fU),     (LONG)floorf(fV));
    samples[0][1] = FetchTexel((LONG)floorf(fU) + 1, (LONG)floorf(fV));
    samples[1][0] = FetchTexel((LONG)floorf(fU),     (LONG)floorf(fV) + 1);
    samples[1][1] = FetchTexel((LONG)floorf(fU) + 1, (LONG)floorf(fV) + 1);

    FLOAT val1 = Lerp(samples[0][0], samples[0][1], fmodf(fU, 1.0f));
    FLOAT val2 = Lerp(samples[1][0], samples[1][1], fmodf(fU, 1.0f));

    return Lerp(val1, val2, fmodf(fV, 1.0f));
  }

// ----------------------------------------------------------------------------
// IMPLEMENTATION - EdgeDatabase
// ----------------------------------------------------------------------------

  bool EdgeDatabase::IsEdgeTessellated(SplitEdge* _edge)
  {
    if (_edge->splitVertex != NULL)
      return true;

    VertexPtrNode* vertexNode = _edge->vertices[0]->ptr->data.pos->vertices;
    while (vertexNode != NULL)
    {
      if (vertexNode->element->ptr != NULL)
      {
        Vertex* vertex = vertexNode->element->ptr;

        FacePtrNode* faceNode = vertex->faces;
        while (faceNode != NULL)
        {
          if (faceNode->element->ptr != NULL)
          {
            Face* face = faceNode->element->ptr;

            VertexPtrNode* vertexNode2 = _edge->vertices[1]->ptr->data.pos->vertices;
            while (vertexNode2 != NULL)
            {
              if (vertexNode2->element->ptr != NULL)
              {
                Vertex* vertex2 = vertexNode2->element->ptr;

                if (face->HasVertex(vertex2))
                {
                  SplitEdge* edge = FindEdge(vertexNode->element, vertexNode2->element);
                  if ((edge != NULL) && (edge->splitVertex != NULL))
                     return true;
                }
              }

              vertexNode2 = vertexNode2->next;
            }
          }

          faceNode = faceNode->next;
        }
      }

      vertexNode = vertexNode->next;
    }

    return false;
  }

// ----------------------------------------------------------------------------
// IMPLEMENTATION - MeshBuilder
// ----------------------------------------------------------------------------

  AllAllocators* MeshBuilder::s_AllAllocators = NULL;

  MeshBuilder::MeshBuilder(void)
  : m_NbLastVertices(0), m_CanAddVertices(false), m_NbVertices(0), m_NbFaces(0),
    m_MaxVertexCount(0xffffffff), m_MaxFaceCount(0xffffffff), m_SafeMode(false)
  {
    m_Allocators.vertexAllocator.Initialize(4096);
    m_Allocators.vertexPtrNodeAllocator.Initialize(4096);

    m_Allocators.positionInfoAllocator.Initialize(4096);
    m_Allocators.positionLinkAllocator.Initialize(4096);

    m_Allocators.faceAllocator.Initialize(4096);
    m_Allocators.facePtrNodeAllocator.Initialize(4096);
    m_Allocators.splitFaceAllocator.Initialize(4096);

    m_Allocators.edgeAllocator.Initialize(4096);
    m_Allocators.edgePtrAllocator.Initialize(4096);
    m_Allocators.edgePtrNodeAllocator.Initialize(4096);

    s_AllAllocators = &m_Allocators;

    m_MaterialInfo = new MaterialInfo [MESH_BUILDER_MAX_ELEMENTS];
  }

  MeshBuilder::~MeshBuilder(void)
  {
    DumpAllocatorUsages();

    Clear();

    // Clear the material information structure
    delete [] m_MaterialInfo;

    s_AllAllocators = NULL;
  }

  void MeshBuilder::DumpAllocatorUsages(void)
  {
#if defined(_XMP_TRACK_MEMORY)
    Log(LL_DEBUG, "]> MeshBuilder - Memory usage of internal allocators");

    Log(LL_DEBUG, "   Vertex Allocator        : %10u bytes - Pool allocating batches of %u elements, 1 element is %u bytes",
        m_Allocators.vertexAllocator.GetPoolMemoryUsage(),
        m_Allocators.vertexAllocator.GetMaxElementsPerPool(),
        m_Allocators.vertexAllocator.GetElementSize());
    Log(LL_DEBUG, "   VertexPtrNode Allocator : %10u bytes - Pool allocating batches of %u elements, 1 element is %u bytes",
        m_Allocators.vertexPtrNodeAllocator.GetPoolMemoryUsage(),
        m_Allocators.vertexPtrNodeAllocator.GetMaxElementsPerPool(),
        m_Allocators.vertexPtrNodeAllocator.GetElementSize());

    Log(LL_DEBUG, "   PositionInfo Allocator  : %10u bytes - Pool allocating batches of %u elements, 1 element is %u bytes",
        m_Allocators.positionInfoAllocator.GetPoolMemoryUsage(),
        m_Allocators.positionInfoAllocator.GetMaxElementsPerPool(),
        m_Allocators.positionInfoAllocator.GetElementSize());
    Log(LL_DEBUG, "   VertexPtrNode Allocator : %10u bytes - Pool allocating batches of %u elements, 1 element is %u bytes",
        m_Allocators.positionLinkAllocator.GetPoolMemoryUsage(),
        m_Allocators.positionLinkAllocator.GetMaxElementsPerPool(),
        m_Allocators.positionLinkAllocator.GetElementSize());

    Log(LL_DEBUG, "   Face Allocator          : %10u bytes - Pool allocating batches of %u elements, 1 element is %u bytes",
        m_Allocators.faceAllocator.GetPoolMemoryUsage(),
        m_Allocators.faceAllocator.GetMaxElementsPerPool(),
        m_Allocators.faceAllocator.GetElementSize());
    Log(LL_DEBUG, "   FacePtrNode Allocator   : %10u bytes - Pool allocating batches of %u elements, 1 element is %u bytes",
        m_Allocators.facePtrNodeAllocator.GetPoolMemoryUsage(),
        m_Allocators.facePtrNodeAllocator.GetMaxElementsPerPool(),
        m_Allocators.facePtrNodeAllocator.GetElementSize());

    Log(LL_DEBUG, "   Edge Allocator          : %10u bytes - Pool allocating batches of %u elements, 1 element is %u bytes",
        m_Allocators.edgeAllocator.GetPoolMemoryUsage(),
        m_Allocators.edgeAllocator.GetMaxElementsPerPool(),
        m_Allocators.edgeAllocator.GetElementSize());
    Log(LL_DEBUG, "   EdgePtr Allocator       : %10u bytes - Pool allocating batches of %u elements, 1 element is %u bytes",
        m_Allocators.edgePtrAllocator.GetPoolMemoryUsage(),
        m_Allocators.edgePtrAllocator.GetMaxElementsPerPool(),
        m_Allocators.edgePtrAllocator.GetElementSize());
    Log(LL_DEBUG, "   EdgePtrNode Allocator   : %10u bytes - Pool allocating batches of %u elements, 1 element is %u bytes",
        m_Allocators.edgePtrNodeAllocator.GetPoolMemoryUsage(),
        m_Allocators.edgePtrNodeAllocator.GetMaxElementsPerPool(),
        m_Allocators.edgePtrNodeAllocator.GetElementSize());
#endif
  }

  void MeshBuilder::SetSafeMode(bool _enable)
  {
      m_SafeMode = _enable;
  }

  void MeshBuilder::BeginVertices(void)
  {
    Clear();

    m_CanAddVertices = true;
  }

  void MeshBuilder::EndVertices(void)
  {
    m_CanAddVertices = false;

    // Build the static data KD-Tree
    for (ULONG i = 0; i < MESH_BUILDER_MAX_ELEMENTS; ++i)
    {
      m_MaterialInfo[i].GetDataTree()->Build();
    }
  }

  void MeshBuilder::AddVertex(FLOAT _x, FLOAT _y, FLOAT _z, 
                              FLOAT _w1, FLOAT _w2, FLOAT _w3, FLOAT _w4, 
                              ULONG _color, 
                              FLOAT _u, FLOAT _v, 
                              BYTE _elementIndex)
  {
    XMP_ASSERT(m_CanAddVertices);

    if (!m_CanAddVertices)
      return;

    PositionInfo* positionInfo;

    // Using a D3DXCOLOR allows for a vector of 4 floats within the range 0..1 to be compressed using 1 DWORD
    D3DXCOLOR weights(_w1, _w2, _w3, _w4);

    // Add the vertex
    m_LastVertices[m_NbLastVertices] = FindVertex(_x, _y, _z, (DWORD)weights, _color, _u, _v, _elementIndex, &positionInfo);
    if (m_LastVertices[m_NbLastVertices] == NULL)
    {
      // Insert the vertex weights and color into the KD-Tree
      D3DXVECTOR3 position(_x, _y, _z);
      StaticVertexData vertexData;
      vertexData.weights = weights;
      vertexData.color   = _color;
      m_MaterialInfo[_elementIndex].GetDataTree()->AddElement(position, vertexData);

      m_LastVertices[m_NbLastVertices] = PushVertex(_x, _y, _z, (DWORD)weights, _color, _u, _v, _elementIndex, positionInfo);
    }
    ++m_NbLastVertices;

    // 3 vertices added -> create a new face
    if (m_NbLastVertices == 3)
    {
      // Add the new face
      PushFace(m_LastVertices[0], m_LastVertices[1], m_LastVertices[2]);

      // Reset the vertex counter
      m_NbLastVertices = 0;
    }
  }

  bool MeshBuilder::IsSplitNecessary(void)
  {
    ULONG nbFaces = m_Faces.size();
    ULONG i;

    for (i = 0; i < nbFaces; ++i)
    {
      Face* face = m_Faces[i]->ptr;

      if (face == NULL)
        continue;

      if (!m_MaterialInfo[face->vertices[0]->ptr->data.elementIndex].IsTessellationEnabled())
        continue;

      if (face->area > m_MaterialInfo[face->vertices[0]->ptr->data.elementIndex].GetMaxTriangleArea())
      {
        return true;
      }

      if (!m_MaterialInfo[face->vertices[0]->ptr->data.elementIndex].IsSimplificationEnabled())
      {
        // Too many vertices or faces already, don't split anymore
        if (((m_NbVertices + 3) >= m_MaxVertexCount) || ((m_NbFaces + 3) >= m_MaxFaceCount))
          return false;
      }
    }

    return false;
  }

  bool MeshBuilder::SplitTriangles(void)
  {
    XMP_PROFILE_FUNCTION();

    EdgeDatabase   edges;
    SplitFaceArray newFaces;
    D3DXVECTOR3    tempVector;

    ULONG nbFaces = m_Faces.size();
    ULONG i;
    ULONG j;
    bool  canSplit;
    FLOAT maxTriangleArea;
    FLOAT edgeLength[3];
    FLOAT edgeLengthThreshold;

    m_Allocators.splitFaceAllocator.Reset();

    // Backup all the faces and create the edges
    for (i = 0; i < nbFaces; ++i)
    {
      FacePtr* facePtr = m_Faces[i];
      Face*    face    = facePtr->ptr;

      if (face == NULL)
        continue;

      // We're alive!
      if ((i % 500) == 0)
        Tick();

      // Add the face to the new list
      SplitFace* newFace   = m_Allocators.splitFaceAllocator.Allocate();
      newFace->vertices[0] = face->vertices[0];
      newFace->vertices[1] = face->vertices[1];
      newFace->vertices[2] = face->vertices[2];
      newFace->edges[0]    = edges.AddEdge(newFace->vertices[0], newFace->vertices[1]);
      newFace->edges[1]    = edges.AddEdge(newFace->vertices[1], newFace->vertices[2]);
      newFace->edges[2]    = edges.AddEdge(newFace->vertices[2], newFace->vertices[0]);
      newFaces.push_back(newFace);

      // Make sure the triangle is modifiable before splitting it
      canSplit        = m_MaterialInfo[face->vertices[0]->ptr->data.elementIndex].IsTessellationEnabled();
      maxTriangleArea = m_MaterialInfo[face->vertices[0]->ptr->data.elementIndex].GetMaxTriangleArea();

      // Do not split this triangle if we are not going to simplify the mesh afterward
      if (!m_MaterialInfo[face->vertices[0]->ptr->data.elementIndex].IsSimplificationEnabled())
      {
        if ((m_NbVertices + 3) >= m_MaxVertexCount)
        {
          canSplit = false;
        }
      }

      // Use the length of the edges to tessellate for more uniform triangle distribution
      D3DXVECTOR3 temp;
      edgeLength[0] = D3DXVec3Length(D3DXVec3Subtract(&temp, &face->vertices[0]->ptr->GetPosition(), 
                                                             &face->vertices[1]->ptr->GetPosition()));
      edgeLength[1] = D3DXVec3Length(D3DXVec3Subtract(&temp, &face->vertices[1]->ptr->GetPosition(), 
                                                             &face->vertices[2]->ptr->GetPosition()));
      edgeLength[2] = D3DXVec3Length(D3DXVec3Subtract(&temp, &face->vertices[2]->ptr->GetPosition(), 
                                                             &face->vertices[0]->ptr->GetPosition()));
      edgeLengthThreshold  = Max(edgeLength[0], Max(edgeLength[1], edgeLength[2]));
      edgeLengthThreshold *= MESH_BUILDER_EDGE_LENGTH_THRESHOLD;

      // Mark the edges as split if necessary
      if ( canSplit                                && 
          (face->area > maxTriangleArea)           && 
          (newFace->edges[0]->splitVertex == NULL) && 
          (edgeLength[0] > edgeLengthThreshold)    && 
          !edges.IsEdgeTessellated(newFace->edges[0]))
      {
        D3DXVec3Lerp(&temp, &face->vertices[0]->ptr->GetPosition(), &face->vertices[1]->ptr->GetPosition(), 0.5f);

        VertexPtr* splitVertexPtr = PushVertex(0.0f, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0, NULL);
        Vertex*    splitVertex    = splitVertexPtr->ptr;

        InterpolateVertex(&splitVertex->data, face->vertices[0]->ptr, face->vertices[1]->ptr, 0.5f);
        newFace->edges[0]->splitVertex = splitVertexPtr;

        // Add to KD-Tree
        StaticVertexData vertexData;
        vertexData.weights = splitVertex->data.weights;
        vertexData.color   = splitVertex->data.color;
        m_MaterialInfo[splitVertex->data.elementIndex].GetDataTree()->AddElement(splitVertex->GetPosition(), vertexData);
      }

      if ( canSplit                                && 
          (face->area > maxTriangleArea)           && 
          (newFace->edges[1]->splitVertex == NULL) && 
          (edgeLength[1] > edgeLengthThreshold)    && 
          !edges.IsEdgeTessellated(newFace->edges[1]))
      {
        D3DXVec3Lerp(&temp, &face->vertices[1]->ptr->GetPosition(), &face->vertices[2]->ptr->GetPosition(), 0.5f);

        VertexPtr* splitVertexPtr = PushVertex(0.0f, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0, NULL);
        Vertex*    splitVertex    = splitVertexPtr->ptr;

        InterpolateVertex(&splitVertex->data, face->vertices[1]->ptr, face->vertices[2]->ptr, 0.5f);
        newFace->edges[1]->splitVertex = splitVertexPtr;

        // Add to KD-Tree
        StaticVertexData vertexData;
        vertexData.weights = splitVertex->data.weights;
        vertexData.color   = splitVertex->data.color;
        m_MaterialInfo[splitVertex->data.elementIndex].GetDataTree()->AddElement(splitVertex->GetPosition(), vertexData);
      }

      if ( canSplit                                && 
          (face->area > maxTriangleArea)           && 
          (newFace->edges[2]->splitVertex == NULL) && 
          (edgeLength[2] > edgeLengthThreshold)    && 
          !edges.IsEdgeTessellated(newFace->edges[2]))
      {
        D3DXVec3Lerp(&temp, &face->vertices[2]->ptr->GetPosition(), &face->vertices[0]->ptr->GetPosition(), 0.5f);

        VertexPtr* splitVertexPtr = PushVertex(0.0f, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0, NULL);
        Vertex*    splitVertex    = splitVertexPtr->ptr;

        InterpolateVertex(&splitVertex->data, face->vertices[2]->ptr, face->vertices[0]->ptr, 0.5f);
        newFace->edges[2]->splitVertex = splitVertexPtr;

        // Add to KD-Tree
        StaticVertexData vertexData;
        vertexData.weights = splitVertex->data.weights;
        vertexData.color   = splitVertex->data.color;
        m_MaterialInfo[splitVertex->data.elementIndex].GetDataTree()->AddElement(splitVertex->GetPosition(), vertexData);
      }
    }

    // Let's not create T-junctions
    nbFaces = newFaces.size();
    for (i = 0; i < nbFaces; ++i)
    {
      SplitFace* face = newFaces[i];

      for (j = 0; j < 3; ++j)
      {
        if ((face->edges[j]->splitVertex == NULL) &&
          edges.IsEdgeTessellated(face->edges[j]))
        {
          D3DXVec3Lerp(&tempVector, &face->vertices[j]->ptr->GetPosition(), 
            &face->vertices[(j + 1) % 3]->ptr->GetPosition(), 0.5f);

          PositionInfo* info = FindPosition(tempVector.x, tempVector.y, tempVector.z);

          VertexPtr* splitVertexPtr = PushVertex(0.0f, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0, info);
          Vertex*    splitVertex    = splitVertexPtr->ptr;

          InterpolateVertex(&splitVertex->data, face->vertices[j]->ptr, face->vertices[(j + 1) % 3]->ptr, 0.5f);
          face->edges[j]->splitVertex = splitVertexPtr;

          // Add to KD-Tree
          StaticVertexData vertexData;
          vertexData.weights = splitVertex->data.weights;
          vertexData.color   = splitVertex->data.color;
          m_MaterialInfo[splitVertex->data.elementIndex].GetDataTree()->AddElement(splitVertex->GetPosition(), vertexData);
        }
      }
    }

    // Clear all the faces since we'll rebuild them
    ClearFaces();

    // Rebuild the faces
    nbFaces = newFaces.size();
    for (i = 0; i < nbFaces; ++i)
    {
      SplitFace* face = newFaces[i];

      // We're alive!
      if ((i % 500) == 0)
        Tick();

      // Let's not create T-junctions
      for (j = 0; j < 3; ++j)
      {
        if ((face->edges[j]->splitVertex == NULL) &&
             edges.IsEdgeTessellated(face->edges[j]))
        {
          D3DXVec3Lerp(&tempVector, &face->vertices[j]->ptr->GetPosition(), 
                                    &face->vertices[(j + 1) % 3]->ptr->GetPosition(), 0.5f);

          PositionInfo* info = FindPosition(tempVector.x, tempVector.y, tempVector.z);

          VertexPtr* splitVertexPtr = PushVertex(0.0f, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0, info);
          Vertex*    splitVertex    = splitVertexPtr->ptr;

          InterpolateVertex(&splitVertex->data, face->vertices[j]->ptr, face->vertices[(j + 1) % 3]->ptr, 0.5f);
          face->edges[j]->splitVertex = splitVertexPtr;

          // Add to KD-Tree
          StaticVertexData vertexData;
          vertexData.weights = splitVertex->data.weights;
          vertexData.color   = splitVertex->data.color;
          m_MaterialInfo[splitVertex->data.elementIndex].GetDataTree()->AddElement(splitVertex->GetPosition(), vertexData);
        }
      }

      if ((face->edges[0]->splitVertex != NULL) &&
          (face->edges[1]->splitVertex != NULL) &&
          (face->edges[2]->splitVertex != NULL))
      {
        // All 3 edges are splitted
        PushFace(face->vertices[0], face->edges[0]->splitVertex, face->edges[2]->splitVertex);
        PushFace(face->vertices[1], face->edges[1]->splitVertex, face->edges[0]->splitVertex);
        PushFace(face->vertices[2], face->edges[2]->splitVertex, face->edges[1]->splitVertex);
        PushFace(face->edges[0]->splitVertex, face->edges[1]->splitVertex, face->edges[2]->splitVertex);
      }
      else if ((face->edges[0]->splitVertex != NULL) &&
               (face->edges[1]->splitVertex != NULL) &&
               (face->edges[2]->splitVertex == NULL))
      {
        // First 2 edges are splitted
        PushFace(face->vertices[0], face->edges[0]->splitVertex, face->edges[1]->splitVertex);
        PushFace(face->vertices[1], face->edges[1]->splitVertex, face->edges[0]->splitVertex);
        PushFace(face->vertices[2], face->vertices[0], face->edges[1]->splitVertex);
      }
      else if ((face->edges[0]->splitVertex != NULL) &&
               (face->edges[1]->splitVertex == NULL) &&
               (face->edges[2]->splitVertex != NULL))
      {
        // Edges 1 & 3 are splitted
        PushFace(face->vertices[0], face->edges[0]->splitVertex, face->edges[2]->splitVertex);
        PushFace(face->vertices[2], face->edges[2]->splitVertex, face->edges[0]->splitVertex);
        PushFace(face->vertices[1], face->vertices[2], face->edges[0]->splitVertex);
      }
      else if ((face->edges[0]->splitVertex == NULL) &&
               (face->edges[1]->splitVertex != NULL) &&
               (face->edges[2]->splitVertex != NULL))
      {
        // Edges 2 & 3 are splitted
        PushFace(face->vertices[2], face->edges[2]->splitVertex, face->edges[1]->splitVertex);
        PushFace(face->vertices[1], face->edges[1]->splitVertex, face->edges[2]->splitVertex);
        PushFace(face->vertices[0], face->vertices[1], face->edges[2]->splitVertex);
      }
      else if ((face->edges[0]->splitVertex != NULL) &&
               (face->edges[1]->splitVertex == NULL) &&
               (face->edges[2]->splitVertex == NULL))
      {
        // Edge 1 is splitted
        PushFace(face->vertices[0], face->edges[0]->splitVertex, face->vertices[2]);
        PushFace(face->vertices[1], face->vertices[2], face->edges[0]->splitVertex);
      }
      else if ((face->edges[0]->splitVertex == NULL) &&
               (face->edges[1]->splitVertex != NULL) &&
               (face->edges[2]->splitVertex == NULL))
      {
        // Edge 2 is splitted
        PushFace(face->vertices[1], face->edges[1]->splitVertex, face->vertices[0]);
        PushFace(face->vertices[2], face->vertices[0], face->edges[1]->splitVertex);
      }
      else if ((face->edges[0]->splitVertex == NULL) &&
               (face->edges[1]->splitVertex == NULL) &&
               (face->edges[2]->splitVertex != NULL))
      {
        // Edge 3 is splitted
        PushFace(face->vertices[2], face->edges[2]->splitVertex, face->vertices[1]);
        PushFace(face->vertices[0], face->vertices[1], face->edges[2]->splitVertex);
      }
      else
      {
        // None of the edges are splitted
        PushFace(face->vertices[0], face->vertices[1], face->vertices[2]);
      }
    }

    // If there is no simplification at all and we exceeded the maximum number of
    // faces, we will revert to the faces we had when we started splitting and cancel
    // further tessellation.
    if (m_NbFaces > m_MaxFaceCount)
    {
      bool bSimplificationEnabled = false;

      for (i = 0; i < MESH_BUILDER_MAX_ELEMENTS; ++i)
      {
        if (m_MaterialInfo[i].IsSimplificationEnabled())
        {
          bSimplificationEnabled = true;
          break;
        }
      }

      // Revert :(
      if (!bSimplificationEnabled)
      {
        ClearFaces();

        nbFaces = newFaces.size();
        for (i = 0; i < nbFaces; ++i)
        {
          SplitFace* face = newFaces[i];
          PushFace(face->vertices[0], face->vertices[1], face->vertices[2]);
        }

        return false;
      }
    }

    return true;
  }

  void MeshBuilder::Tessellate(void)
  {
    XMP_PROFILE_FUNCTION();

    XMP_ASSERT(!m_CanAddVertices);

#if defined(MESH_BUILDER_VERBOSE)
    ULONG nbOriVertices = m_Vertices.size();
    ULONG nbOriFaces    = m_Faces.size();
#endif

    if (m_CanAddVertices)
      return;

    // Split until we reach requested size
    ULONG nbIterations = 0;
    while (IsSplitNecessary())
    {
      if (!SplitTriangles())
        break;

#if defined(_XMP_MEMORY_LOG) && defined(_XMP_TRACK_MEMORY)
      XMP_ShowMemoryInfo("Tessellation Step");
      DumpAllocatorUsages();
#endif

      ++nbIterations;
    }

    m_Allocators.splitFaceAllocator.Shutdown();

#if defined(MESH_BUILDER_VERBOSE)
    Log(LL_INFO, "XMP - Tessellate -> %u faces (%u vertices) to %u faces (%u vertices) using %u iterations", 
        nbOriFaces, nbOriVertices, m_NbFaces, m_NbVertices, nbIterations);
#endif
  }

  void MeshBuilder::Displace(void)
  {
    XMP_PROFILE_FUNCTION();

    XMP_ASSERT(!m_CanAddVertices);

    if (m_CanAddVertices)
      return;

    // Update the search distance since we are moving the vertices away from the originals
    FLOAT maxOffset       = 0.0f;
    FLOAT maxDisplacement = 0.0f;
    for (ULONG i = 0; i < MESH_BUILDER_MAX_ELEMENTS; ++i)
    {
      if (!m_MaterialInfo[i].IsDisplacementEnabled())
        continue;

      if (fabsf(m_MaterialInfo[i].GetDisplacementOffset()) > maxOffset)
        maxOffset = fabsf(m_MaterialInfo[i].GetDisplacementOffset());
      if (fabsf(m_MaterialInfo[i].GetDisplacementMax()) > maxDisplacement)
        maxDisplacement = fabsf(m_MaterialInfo[i].GetDisplacementMax());

      m_MaterialInfo[i].SetMaxSearchDistance(m_MaterialInfo[i].GetMaxSearchDistance() + maxOffset + maxDisplacement);
    }

    // Update the mesh boundaries
    FindMeshBoundaries();

    // Compute the normals since we use them to displace the mesh
    ComputeVertexNormals();

    for (ULONG i = 0; i < m_Vertices.size(); ++i)
    {
      Vertex* vertex = m_Vertices[i]->ptr;

      // We're alive!
      if ((i % 200) == 0)
        Tick();

      if (vertex == NULL)
        continue;

      MaterialInfo* info = &m_MaterialInfo[vertex->data.elementIndex];

      // Make sure we can modify the vertex before doing so
      if (!info->IsDisplacementEnabled())
        continue;

      FLOAT height = info->GetBaseDisplacement(vertex->data.texCoord.x, vertex->data.texCoord.y);

      // Use boundary and neighbor information to find the maximum allowed displacement
      FLOAT alpha = GetVertexDisplacementFactor(vertex);
      FLOAT disp  = Lerp(0.0f, (height * info->GetDisplacementMax()) + info->GetDisplacementOffset(), alpha);

      if (disp != 0.0f)
      {
        vertex->GetPosition() += vertex->normal * disp;
      }

      FacePtrNode* faceNodePtr = m_Vertices[i]->ptr->faces;
      while (faceNodePtr != NULL)
      {
        faceNodePtr->element->ptr->Compute();
        faceNodePtr = faceNodePtr->next;
      }
    }

#if defined(_XMP_MEMORY_LOG) && defined(_XMP_TRACK_MEMORY)
    XMP_ShowMemoryInfo("Displacement");
    DumpAllocatorUsages();
#endif
  }

  FLOAT MeshBuilder::GetVertexDisplacementFactor(const Vertex* _vertex) const
  {
    ULONG nbNeighbors  = 0;
    ULONG nbBoundaries = 0;

    // Boundaries are not allowed to move
    if (_vertex->IsBoundary())
      return 0.0f;

    VertexPtrNode* neighbor = _vertex->neighbors;
    while (neighbor != NULL)
    {
      if (neighbor->element->ptr != NULL)
      {
        ++nbNeighbors;

        if (neighbor->element->ptr->IsBoundary())
        {
          ++nbBoundaries;
        }
      }

      neighbor = neighbor->next;
    }

    // A vertex with no neighbors shall not move - Should never happen though..
    if (nbNeighbors == 0)
      return 0.0f;

    // Smooth displacement near the boundaries of the mesh
    return 1.0f - ((FLOAT)nbBoundaries / (FLOAT)nbNeighbors);
  }

  void MeshBuilder::Simplify(FLOAT _maxCost)
  {
    XMP_PROFILE_FUNCTION();

    VertexPtrArray newVertices;
    ULONG          loopCounter = 0;

    XMP_ASSERT(!m_CanAddVertices);

    if (m_CanAddVertices)
      return;

    // Update the cost multiplier as a function the displacement we allowed
    for (ULONG i = 0; i < MESH_BUILDER_MAX_ELEMENTS; ++i)
    {
      if (m_MaterialInfo[i].IsDisplacementEnabled())
      {
        FLOAT displacement = fabsf(m_MaterialInfo[i].GetDisplacementMax());

        for (ULONG j = 0; j < MESH_BUILDER_DISPLACEMENT_COSTS_COUNT; ++j)
        {
          if (displacement <= MESH_BUILDER_DISPLACEMENT_COSTS[j].displacement)
          {
            m_MaterialInfo[i].SetCostMultiplier(MESH_BUILDER_DISPLACEMENT_COSTS[j].costMultiplier);
            break;
          }
        }
      }
    }

#if defined(MESH_BUILDER_VERBOSE)
    ULONG nbOriFaces    = m_NbFaces;
    ULONG nbOriVertices = m_NbVertices;
    ULONG nbCollapse    = 0;
#endif

    // Update the mesh boundaries
    FindMeshBoundaries();
    Tick();

    // Compute vertex matrices
    ComputeAllQuadrics();
    Tick();

    // Find all valid edges and their collapsing costs + targets
    ComputeEdges();
    Tick();

    // Sort all potential targets
    std::make_heap(m_Edges.begin(), m_Edges.end(), EdgeCostCompare());
    Tick();

    // Proceed with simplification
    while ((m_Edges.size() > 0) && (m_NbFaces > 1))
    {
      if ((m_Edges.front()->ptr      != NULL)     &&
          (m_Edges.front()->ptr->cost > _maxCost) && 
          (m_NbVertices <= m_MaxVertexCount)      && 
          (m_NbFaces    <= m_MaxFaceCount))
        break;

      // We're alive!
      if ((loopCounter % 1000) == 0)
        Tick();
      ++loopCounter;

      std::pop_heap(m_Edges.begin(), m_Edges.end(), EdgeCostCompare());

      EdgePtr* edgePtr = m_Edges.back();
      Edge*    edge    = edgePtr->ptr;

      if ((edge != NULL) && (edge->vertices[0]->ptr != NULL) && (edge->vertices[1]->ptr != NULL))
      {
#if defined(MESH_BUILDER_VERBOSE)
        ++nbCollapse;
#endif

        // Collapse the edge
        newVertices.clear();
        CollapseEdge(edge, &newVertices);

        m_Edges.pop_back();

        for (ULONG j = 0; j < newVertices.size(); ++j)
        {
          VertexPtr* newVertexPtr = newVertices[j];

          if ((newVertexPtr != NULL) && (newVertexPtr->ptr != NULL))
          {
            Vertex* newVertex = newVertexPtr->ptr;

            // Add the edges of the new vertex
            VertexPtrNode* neighbor = newVertex->neighbors;
            while (neighbor != NULL)
            {
              // All edges were invalidated when collapsing the vertices, we must recompute them..
              if (neighbor->element->ptr != NULL)
              {
                VertexPtrNode* neighborNeighbor = neighbor->element->ptr->neighbors;
                while (neighborNeighbor != NULL)
                {
                  if (neighborNeighbor->element->ptr != NULL)
                  {
                    if (ComputeOneEdge(neighbor->element, neighborNeighbor->element, false, -1.0f, NULL))
                    {
                      std::push_heap(m_Edges.begin(), m_Edges.end(), EdgeCostCompare());
                    }
                  }
                  neighborNeighbor = neighborNeighbor->next;
                }
              }

              neighbor = neighbor->next;
            }
          }
        }
      }
      else
      {
        m_Edges.pop_back();
      }
    }

#if defined(MESH_BUILDER_VERBOSE)
    Log(LL_INFO, "XMP - Simplify -> %u faces (%u vertices) to %u faces (%u vertices) by collapsing %u edges", 
        nbOriFaces, nbOriVertices, m_NbFaces, m_NbVertices, nbCollapse);
#endif

#if defined(_XMP_MEMORY_LOG) && defined(_XMP_TRACK_MEMORY)
    XMP_ShowMemoryInfo("Simplification");
    DumpAllocatorUsages();
#endif

    // Remove any noticable artefacts
    CleanMesh();
  }

  void MeshBuilder::CleanMesh(void)
  {
    ULONG nbFaces = m_Faces.size();

    Log(LL_DEBUG, "XMP - CleanMesh - Function not yet implemented");

    // SC: TODO: Mesh clean up -> Detect discontinuities in the mesh and turn the edges to repair

#if defined(_XMP_MEMORY_LOG) && defined(_XMP_TRACK_MEMORY)
    XMP_ShowMemoryInfo("Clean Mesh");
    DumpAllocatorUsages();
#endif
  }

  VertexPtr* MeshBuilder::FindVertex(FLOAT _x, FLOAT _y, FLOAT _z, 
                                     ULONG _weights, ULONG _color, 
                                     FLOAT _u, FLOAT _v, 
                                     BYTE _elementIndex, 
                                     PositionInfo** _positionInfo)
  {
    ULONG nbVertices = m_Vertices.size();
    ULONG i;

    *_positionInfo = FindPosition(_x, _y, _z);

    // Make sure we dont duplicate vertices
    for (i = 0; i < nbVertices; ++i)
    {
      VertexPtr* vertexPtr = m_Vertices[i];
      Vertex*    vertex    = vertexPtr->ptr;

      if (vertex == NULL)
        continue;

      if (*_positionInfo != vertex->data.pos)
        continue;

      if (_elementIndex != vertex->data.elementIndex)
        continue;

      if (!IsColorEqual(_color, vertex->data.color))
        continue;

      if (!IsWeightEqual(_weights, vertex->data.weights))
        continue;

      if (!IsTexCoordEqual(_u, _v, vertex->data.texCoord.x, vertex->data.texCoord.y, MESH_BUILDER_UV_EPSILON))
        continue;

      return vertexPtr;
    }

    return NULL;
  }

  VertexPtr* MeshBuilder::PushVertex(FLOAT _x, FLOAT _y, FLOAT _z, ULONG _weights, ULONG _color, FLOAT _u, FLOAT _v, BYTE _elementIndex, PositionInfo* _positionInfo)
  {
    VertexPtr* newVertexPtr = m_Allocators.vertexAllocator.Allocate();
    Vertex*    newVertex    = newVertexPtr->ptr;
    newVertex->Reset();

    if (_positionInfo)
      newVertex->data.pos = _positionInfo;
    else
      newVertex->data.pos = PushPosition(_x, _y, _z);

    newVertex->data.weights      = _weights;
    newVertex->data.color        = _color;
    newVertex->data.texCoord.x   = _u;
    newVertex->data.texCoord.y   = _v;
    newVertex->data.elementIndex = _elementIndex;
    newVertex->SetBoundary(false);
    newVertex->SetValid(true);
    newVertex->SetIndex(m_Vertices.size());

    m_Vertices.push_back(newVertexPtr);

    // Add the vertex pointer to the position information
    newVertex->data.pos->AddVertex(newVertexPtr);

    // Increment the vertex count
    ++m_NbVertices;

    return newVertexPtr;
  }

  void MeshBuilder::PushFace(VertexPtr* _v1, VertexPtr* _v2, VertexPtr* _v3)
  {
    ULONG i;
    ULONG j;

    FacePtr* facePtr           = m_Allocators.faceAllocator.Allocate();
    facePtr->ptr->vertices[0]  = _v1;
    facePtr->ptr->vertices[1]  = _v2;
    facePtr->ptr->vertices[2]  = _v3;
    facePtr->ptr->area         = 0.0f;
    facePtr->ptr->elementIndex = _v1->ptr->data.elementIndex;
    facePtr->ptr->Compute();
    m_Faces.push_back(facePtr);

    // Update the vertices
    for (i = 0; i < 3; ++i)
    {
      Vertex* curVertex = facePtr->ptr->vertices[i]->ptr;

      // Add the face to the list
      FacePtrNode* prevNodePtr  = curVertex->faces;
      curVertex->faces          = m_Allocators.facePtrNodeAllocator.Allocate();
      curVertex->faces->element = facePtr;
      curVertex->faces->next    = prevNodePtr;

      // Add the neighbors
      for (j = 0; j < 3; ++j)
      {
        if (i != j)
        {
          curVertex->AddNeighbor(facePtr->ptr->vertices[j]);
        }
      }
    }

    // Increment the number of faces
    ++m_NbFaces;
  }

  void MeshBuilder::ComputeBestPositions(Vertex* _vertex1, Vertex* _vertex2, const Quadric* _quadric, FLOAT* _values)
  {
    D3DXVECTOR3 midPos;
    D3DXVECTOR2 midUV;
    D3DXVec3Lerp(&midPos, &_vertex1->GetPosition(), &_vertex2->GetPosition(), 0.5f);
    D3DXVec2Lerp(&midUV,  &_vertex1->data.texCoord, &_vertex2->data.texCoord, 0.5f);

    FLOAT costV1  = _quadric->Evaluate(&_vertex1->GetPosition(), &_vertex1->data.texCoord);
    FLOAT costV2  = _quadric->Evaluate(&_vertex2->GetPosition(), &_vertex2->data.texCoord);
    FLOAT costMid = _quadric->Evaluate(&midPos, &midUV);

    if (costV1 < costV2)
    {
      if (costMid < costV1)
      {
        _values[0] = 0.5f;
        _values[1] = 0.0f;
        _values[2] = 1.0f;
      }
      else if (costMid < costV2)
      {
        _values[0] = 0.0f;
        _values[1] = 0.5f;
        _values[2] = 1.0f;
      }
      else
      {
        _values[0] = 0.0f;
        _values[1] = 1.0f;
        _values[2] = 0.5f;
      }
    }
    else
    {
      if (costMid < costV2)
      {
        _values[0] = 0.5f;
        _values[1] = 1.0f;
        _values[2] = 0.0f;
      }
      else if (costMid < costV1)
      {
        _values[0] = 1.0f;
        _values[1] = 0.5f;
        _values[2] = 0.0f;
      }
      else
      {
        _values[0] = 1.0f;
        _values[1] = 0.0f;
        _values[2] = 0.5f;
      }
    }
  }

  FLOAT MeshBuilder::ComputeMinCostPosition(Vertex* _vertex1, Vertex* _vertex2, D3DXVECTOR3* _computedPosition)
  {
    D3DXVECTOR3 disp  = _vertex2->GetPosition();
    disp             -= _vertex1->GetPosition();
    FLOAT       len   = D3DXVec3Length(&disp);
    disp             /= len;
    D3DXVECTOR3 disp2 = *_computedPosition;
    disp2            -= _vertex1->GetPosition();
    FLOAT       fDot  = D3DXVec3Dot(&disp, &disp2);
    fDot             /= len;

    if (fDot < 0.0f)
      fDot = 0.0f;
    else if (fDot > 1.0f)
      fDot = 1.0f;

    return fDot;
  }

  ULONG MeshBuilder::GetValidBoundaryCollapse(Vertex* _vertex1, Vertex* _vertex2)
  {
    Vertex*     vertices[2]  = { _vertex1, _vertex2 };
    D3DXVECTOR3 displacement = _vertex2->GetPosition() - _vertex1->GetPosition();
    D3DXVECTOR3 tempDisp;
    D3DXVECTOR3 cross;
    ULONG nbBoundaries = 0;
    ULONG result       = 0;
    ULONG i;
    bool  isInvalid;

    // SC: TOADD: This algorithm is very conservative and some more edges could be collapsed

    for (i = 0; i < 2; ++i)
    {
      nbBoundaries = 0;
      isInvalid    = false;

      VertexPtrNode* neighbor = vertices[i]->neighbors;
      while (neighbor != NULL)
      {
        if ((neighbor->element->ptr != _vertex1) && 
            (neighbor->element->ptr != _vertex2) && 
            (neighbor->element->ptr != NULL)     && 
             neighbor->element->ptr->IsBoundary())
        {
          tempDisp = neighbor->element->ptr->GetPosition() - vertices[i]->GetPosition();
          D3DXVec3Cross(&cross, &displacement, &tempDisp);

          ++nbBoundaries;

          // Can be collapsed if all the boundary vertices a vertex is touching are colinear
          if ((fabsf(cross.x) + fabsf(cross.y) + fabsf(cross.z)) >= MESH_BUILDER_EPSILON)
          {
            isInvalid = true;
            break;
          }
        }

        neighbor = neighbor->next;
      }

      if (!isInvalid && (nbBoundaries > 0))
      {
        result |= (1 << i);
      }
    }

    return result;
  }

  FLOAT MeshBuilder::FindCollapsePoint(D3DXVECTOR3* _res, Vertex* _vertex1, Vertex* _vertex2)
  {
    XMP_PROFILE_FUNCTION();

    FLOAT lerpTarget = 0.0f;

    if (_vertex1->IsBoundary() && _vertex2->IsBoundary())
    {
      ULONG collapseFlags = GetValidBoundaryCollapse(_vertex1, _vertex2);

      if (collapseFlags == 0)
        return -1.0f;

      if (collapseFlags == 1)
      {
        if (IsCollapseFoldingMesh(_vertex2->GetPosition(), _vertex1, _vertex2))
          return -1.0f;

        *_res      = _vertex2->data.pos->position;
        lerpTarget = 1.0f;
      }
      else if (collapseFlags == 2)
      {
        if (IsCollapseFoldingMesh(_vertex1->GetPosition(), _vertex1, _vertex2))
          return -1.0f;

        *_res      = _vertex1->data.pos->position;
        lerpTarget = 0.0f;
      }
      else
      {
        // Any point on the edge is fine so we might as well find the optimal solution...

        D3DXVECTOR3 newPos;
        Quadric quadric(_vertex1->quadric);
        quadric += _vertex2->quadric;

        if (quadric.Optimize(&newPos))
        {
          FLOAT t = ComputeMinCostPosition(_vertex1, _vertex2, &newPos);

          D3DXVec3Lerp(&newPos, &_vertex1->GetPosition(), &_vertex2->GetPosition(), t);

          if (IsCollapseFoldingMesh(newPos, _vertex1, _vertex2))
            return -1.0f;

          *_res      = newPos;
          lerpTarget = t;
        }
        else
        {
          FLOAT weights[3]    = { 0.0f, 0.5f, 1.0f };
          FLOAT choosenWeight =  -1.0f;

          ComputeBestPositions(_vertex1, _vertex2, &quadric, weights);

          for (ULONG i = 0; i < 3; ++i)
          {
            D3DXVec3Lerp(&newPos, &_vertex1->GetPosition(), &_vertex2->GetPosition(), weights[i]);

            if (!IsCollapseFoldingMesh(newPos, _vertex1, _vertex2))
            {
              choosenWeight = weights[i];
              break;
            }
          }

          // Impossible to find a valid point, we shall NOT consider the edge
          if (choosenWeight < 0.0f)
            return -1.0f;

          *_res      = newPos;
          lerpTarget = choosenWeight;
        }
      }
    }
    else if (_vertex1->IsBoundary())
    {
      if (IsCollapseFoldingMesh(_vertex1->GetPosition(), _vertex1, _vertex2))
        return -1.0f;

      *_res      = _vertex1->data.pos->position;
      lerpTarget = 0.0f;
    }
    else if (_vertex2->IsBoundary())
    {
      if (IsCollapseFoldingMesh(_vertex2->GetPosition(), _vertex1, _vertex2))
        return -1.0f;

      *_res      = _vertex2->data.pos->position;
      lerpTarget = 1.0f;
    }
    else
    {
      D3DXVECTOR3 newPos;
      Quadric quadric(_vertex1->quadric);
      quadric += _vertex2->quadric;

      if (quadric.Optimize(&newPos))
      {
        FLOAT t = ComputeMinCostPosition(_vertex1, _vertex2, &newPos);

        D3DXVec3Lerp(&newPos, &_vertex1->GetPosition(), &_vertex2->GetPosition(), t);

        if (IsCollapseFoldingMesh(newPos, _vertex1, _vertex2))
          return -1.0f;

        *_res      = newPos;
        lerpTarget = t;
      }
      else
      {
        FLOAT weights[3]    = { 0.0f, 0.5f, 1.0f };
        FLOAT choosenWeight =  -1.0f;

        ComputeBestPositions(_vertex1, _vertex2, &quadric, weights);

        for (ULONG i = 0; i < 3; ++i)
        {
          D3DXVec3Lerp(&newPos, &_vertex1->GetPosition(), &_vertex2->GetPosition(), weights[i]);

          if (!IsCollapseFoldingMesh(newPos, _vertex1, _vertex2))
          {
            choosenWeight = weights[i];
            break;
          }
        }

        // Impossible to find a valid point, we shall NOT consider the edge
        if (choosenWeight < 0.0f)
          return -1.0f;

        *_res      = newPos;
        lerpTarget = choosenWeight;
      }
    }

    return lerpTarget;
  }

  void MeshBuilder::InterpolateVertex(VertexData* _res, Vertex* _v1, Vertex* _v2, FLOAT _t)
  {
    D3DXVECTOR3 newPos;

    // Position
    D3DXVec3Lerp(&newPos, &_v1->GetPosition(), &_v2->GetPosition(), _t);

    if (_res->pos)
      _res->pos->position = newPos;
    else
      _res->pos = PushPosition(newPos.x, newPos.y, newPos.z);

    // Weights
    D3DXCOLOR w1(_v1->data.weights);
    D3DXCOLOR w2(_v2->data.weights);
    D3DXCOLOR wR;
    D3DXColorLerp(&wR, &w1, &w2, _t);
    FLOAT weightLength = sqrtf((wR.r * wR.r) + (wR.g * wR.g) + (wR.b * wR.b) + (wR.a * wR.a));
    if (weightLength > MESH_BUILDER_EPSILON)
      wR *= 1.0f / weightLength;
    _res->weights = (DWORD)wR;

    // Color
    D3DXCOLOR c1(_v1->data.color);
    D3DXCOLOR c2(_v2->data.color);
    D3DXCOLOR cR;
    D3DXColorLerp(&cR, &c1, &c2, _t);
    _res->color = (DWORD)cR;

    // Texture coordinates
    D3DXVec2Lerp(&_res->texCoord, &_v1->data.texCoord, &_v2->data.texCoord, _t);

    // Element index
    _res->elementIndex = _v1->data.elementIndex;
  }

  void MeshBuilder::UpdateForCollapse(VertexPtr* _vertex1, VertexPtr* _vertex2, VertexPtr* _newVertex, FLOAT _target, VertexPtrArray* _newVertices, VertexPtrArray* _deadVertices)
  {
    VertexPtr*   vertices[2]   = { _vertex1, _vertex2 };
    Vertex*      newVertex     = _newVertex->ptr;
    FacePtrNode* facesToRemove = NULL;
    ULONG        i;

    // Generate a new vertex using the position information from newVertex
    if (_target >= 0.0f)
    {
      PositionInfo* info = newVertex->data.pos;

      _newVertex = PushVertex(0.0f, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0, info);
      newVertex  = _newVertex->ptr;

      InterpolateVertex(&newVertex->data, _vertex1->ptr, _vertex2->ptr, _target);
      newVertex->SetBoundary(_vertex1->ptr->IsBoundary() | _vertex2->ptr->IsBoundary());
    }

    // Add the vertex to the list
    _newVertices->push_back(_newVertex);

    for (i = 0; i < 2; ++i)
    {
      // Update the neighbor list
      VertexPtrNode* neighbor = vertices[i]->ptr->neighbors;
      while (neighbor != NULL)
      {
        if (neighbor->element->ptr != NULL)
        {
          neighbor->element->ptr->ReplaceNeighbor(vertices[i], _newVertex);

          if (neighbor->element != _newVertex)
          {
            newVertex->AddNeighbor(neighbor->element);
          }
        }

        neighbor = neighbor->next;
      }

      // Update the face list
      FacePtrNode* facePtr = vertices[i]->ptr->faces;
      while (facePtr != NULL)
      {
        Face* face = facePtr->element->ptr;

        if (face != NULL)
        {
          if (face->HasVertex(vertices[0]) && face->HasVertex(vertices[1]))
          {
            // Now a degenerate triangle
            FacePtrNode* prevHeadPtr = facesToRemove;
            facesToRemove            = m_Allocators.facePtrNodeAllocator.Allocate();
            facesToRemove->element   = facePtr->element;
            facesToRemove->next      = prevHeadPtr;

            RemoveFace(facePtr->element);
          }
          else if (!face->HasVertex(_newVertex))
          {
            // Add the face, replace the vertex and re-compute the face
            FacePtrNode* prevHeadPtr  = newVertex->faces;
            newVertex->faces          = m_Allocators.facePtrNodeAllocator.Allocate();
            newVertex->faces->element = facePtr->element;
            newVertex->faces->next    = prevHeadPtr;

            face->ReplaceVertex(vertices[i], _newVertex);
            face->Compute();
          }
        }

        facePtr = facePtr->next;
      }

      // Invalidate all the edges using the old vertices
      EdgePtrNode* edgePtr = vertices[i]->ptr->edges;
      EdgePtrNode* nextEdgePtr;
      while (edgePtr != NULL)
      {
        nextEdgePtr = edgePtr->next;

        if (edgePtr->element->ptr != NULL)
        {
          // Recycle the edge
          m_Allocators.edgeAllocator.Free(edgePtr->element->ptr);
          edgePtr->element->ptr = NULL;
        }

        // Recycle the edge pointer node
        m_Allocators.edgePtrNodeAllocator.Free(edgePtr);

        edgePtr = nextEdgePtr;
      }
      vertices[i]->ptr->edges = NULL;
    }

    // Remove the faces from all the neighbors
    VertexPtrNode* neighbor = newVertex->neighbors;
    while (neighbor != NULL)
    {
      FacePtrNode* facePtr = facesToRemove;
      while (facePtr != NULL)
      {
        FacePtrNode* currentFacePtr = neighbor->element->ptr->faces;
        FacePtrNode* prevFacePtr    = NULL;
        while (currentFacePtr != NULL)
        {
          if (currentFacePtr->element == facePtr->element)
          {
            if (prevFacePtr == NULL)
            {
              neighbor->element->ptr->faces = currentFacePtr->next;
            }
            else
            {
              prevFacePtr->next = currentFacePtr->next;
            }

            // Recycle the face node
            m_Allocators.facePtrNodeAllocator.Free(currentFacePtr);

            break;
          }

          prevFacePtr    = currentFacePtr;
          currentFacePtr = currentFacePtr->next;
        }

        facePtr = facePtr->next;
      }

      ComputeVertexQuadric(neighbor->element->ptr);

      // The vertex's quadric changed, all edges are now invalid
      EdgePtrNode* edgePtr = neighbor->element->ptr->edges;
      EdgePtrNode* nextEdgePtr;
      while (edgePtr != NULL)
      {
        nextEdgePtr = edgePtr->next;

        if (edgePtr->element->ptr != NULL)
        {
          // Recycle the edge
          m_Allocators.edgeAllocator.Free(edgePtr->element->ptr);
          edgePtr->element->ptr = NULL;
        }

        // Recycle the edge pointer node
        m_Allocators.edgePtrNodeAllocator.Free(edgePtr);

        edgePtr = nextEdgePtr;
      }
      neighbor->element->ptr->edges = NULL;

      neighbor = neighbor->next;
    }

    ComputeVertexQuadric(newVertex);

    // Delete the node pointers allocated for facesToRemove
    while (facesToRemove != NULL)
    {
      FacePtrNode* nextNode = facesToRemove->next;

      m_Allocators.facePtrNodeAllocator.Free(facesToRemove);

      facesToRemove = nextNode;
    }

    _deadVertices->push_back(_vertex1);
    _deadVertices->push_back(_vertex2);
  }

  void MeshBuilder::CollapseEdge(Edge* _edge, VertexPtrArray* _newVertices)
  {
    XMP_PROFILE_FUNCTION();

    VertexPairArray collapseTargets;
    VertexPtrArray  deadVertices;
    VertexPtr*      newVertexPtr;
    Vertex*         newVertex;
    PositionInfo*   positionInfo = NULL;
    ULONG           i;

    _newVertices->clear();

    // Re-use the position information if possible to keep the links between the vertices
    if (_edge->lerpTarget < MESH_BUILDER_EPSILON)
    {
      positionInfo = _edge->vertices[0]->ptr->data.pos;
    }
    else if (_edge->lerpTarget > 1.0f - MESH_BUILDER_EPSILON)
    {
      positionInfo = _edge->vertices[1]->ptr->data.pos;
    }

    newVertexPtr = PushVertex(0.0f, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0, positionInfo);
    newVertex    = newVertexPtr->ptr;

    // Create the new vertex
    InterpolateVertex(&newVertex->data, _edge->vertices[0]->ptr, _edge->vertices[1]->ptr, _edge->lerpTarget);
    newVertex->SetBoundary(_edge->vertices[0]->ptr->IsBoundary() | _edge->vertices[1]->ptr->IsBoundary());

    // Find all vertex combinations that must be collapsed at the same time
    VertexPtrNode* sharedV1Ptr = _edge->vertices[0]->ptr->data.pos->vertices;
    while (sharedV1Ptr != NULL)
    {
      Vertex* sharedV1 = sharedV1Ptr->element->ptr;

      if ((sharedV1 != NULL) && (sharedV1Ptr->element != _edge->vertices[0]))
      {
        VertexPtrNode* sharedV2Ptr = _edge->vertices[1]->ptr->data.pos->vertices;

        while (sharedV2Ptr != NULL)
        {
          Vertex* sharedV2 = sharedV2Ptr->element->ptr;

          if ((sharedV2 != NULL) && (sharedV2Ptr->element != _edge->vertices[1]))
          {
            FacePtrNode* faceV1Ptr = sharedV1->faces;

            while (faceV1Ptr != NULL)
            {
              Face* face = faceV1Ptr->element->ptr;

              if ((face != NULL) && (face->HasVertex(sharedV2)))
              {
                VertexPair vertexPair;
                vertexPair.vertices[0] = sharedV1Ptr->element;
                vertexPair.vertices[1] = sharedV2Ptr->element;
                collapseTargets.push_back(vertexPair);
              }

              faceV1Ptr = faceV1Ptr->next;
            }
          }

          sharedV2Ptr = sharedV2Ptr->next;
        }
      }

      sharedV1Ptr = sharedV1Ptr->next;
    }

    // Update the collapsed edge
    UpdateForCollapse(_edge->vertices[0], _edge->vertices[1], newVertexPtr, -1.0f, _newVertices, &deadVertices);

    // Update the shared edges
    for (i = 0; i < collapseTargets.size(); ++i)
    {
      UpdateForCollapse(collapseTargets[i].vertices[0], collapseTargets[i].vertices[1], 
                        newVertexPtr, _edge->lerpTarget, _newVertices, &deadVertices);
    }

    // Delete the old vertices
    for (i = 0; i < deadVertices.size(); ++i)
    {
      RemoveVertex(deadVertices[i]);
    }
  }

  void MeshBuilder::Clear(void)
  {
    ULONG i;

    for (i = 0; i < MESH_BUILDER_MAX_ELEMENTS; ++i)
    {
      m_MaterialInfo[i].GetDataTree()->Clear();
    }

    m_NbLastVertices = 0;

    m_Allocators.vertexAllocator.Reset();
    m_Allocators.vertexPtrNodeAllocator.Reset();
    m_Vertices.clear();

    m_Allocators.positionInfoAllocator.Reset();
    m_Allocators.positionLinkAllocator.Reset();
    m_Positions.clear();

    m_Allocators.edgeAllocator.Reset();
    m_Allocators.edgePtrAllocator.Reset();
    m_Allocators.edgePtrNodeAllocator.Reset();
    m_Edges.clear();

    m_Allocators.faceAllocator.Reset();
    m_Allocators.facePtrNodeAllocator.Reset();
    m_Faces.clear();

    m_NbVertices = 0;
    m_NbFaces    = 0;
  }

  void MeshBuilder::ClearEdges(void)
  {
    ULONG i;

    if (m_Edges.size() == 0)
      return;

    for (i = 0; i < m_Vertices.size(); ++i)
    {
      if (m_Vertices[i]->ptr != NULL)
      {
        m_Vertices[i]->ptr->edges = NULL;
      }
    }

    m_Allocators.edgeAllocator.Reset();
    m_Allocators.edgePtrAllocator.Reset();
    m_Allocators.edgePtrNodeAllocator.Reset();

    m_Edges.clear();
  }

  void MeshBuilder::ClearFaces(void)
  {
    ULONG i;

    if (m_Faces.size() == 0)
      return;

    ClearEdges();

    for (i = 0; i < m_Vertices.size(); ++i)
    {
      if (m_Vertices[i]->ptr != NULL)
      {
        m_Vertices[i]->ptr->neighbors = NULL;
        m_Vertices[i]->ptr->faces     = NULL;
      }
    }

    m_Allocators.vertexPtrNodeAllocator.Reset();
    m_Allocators.faceAllocator.Reset();

    m_Faces.clear();

    m_NbFaces = 0;
  }

  void MeshBuilder::FindMeshBoundaries(void)
  {
    VertexPtrNode* neighbor;
    FacePtrNode*   facePtr;
    ULONG nbVertices = m_Vertices.size();
    ULONG nbFaces;
    ULONG i;
    bool  isBoundary;

    // A vertex is a boundary if it shares at most one face with one of its neighbors

    for (i = 0; i < nbVertices; ++i)
    {
      VertexPtr* vertexPtr = m_Vertices[i];
      Vertex*    vertex    = vertexPtr->ptr;

      if (vertex == NULL)
        continue;

      isBoundary = false;

      neighbor = vertex->neighbors;
      while (neighbor != NULL)
      {
        nbFaces = 0;
        facePtr = vertex->faces;
        while (facePtr != NULL)
        {
          if (facePtr->element->ptr->HasVertex(neighbor->element))
            ++nbFaces;

          facePtr = facePtr->next;
        }

        if (nbFaces == 1)
        {
          // It is a boundary, early out
          neighbor->element->ptr->SetBoundary(true);
          isBoundary = true;
          break;
        }

        neighbor = neighbor->next;
      }

      vertex->SetBoundary(isBoundary);
    }
  }

  void MeshBuilder::ComputeAllQuadrics(void)
  {
    ULONG nbVertices = m_Vertices.size();
    ULONG i;

    for (i = 0; i < nbVertices; ++i)
    {
      if (m_Vertices[i]->ptr != NULL)
      {
        ComputeVertexQuadric(m_Vertices[i]->ptr);
      }
    }
  }

  void MeshBuilder::ComputeVertexQuadric(Vertex* _vertex)
  {
    Quadric tempQuadric;

    _vertex->quadric.Clear();

    FacePtrNode* facePtr = _vertex->faces;
    while (facePtr != NULL)
    {
      Face* face = facePtr->element->ptr;

      if (face != NULL)
      {
        face->GetQuadric(&tempQuadric);

        _vertex->quadric += tempQuadric;
      }

      facePtr = facePtr->next;
    }
  }

  void MeshBuilder::ComputeEdges()
  {
    XMP_PROFILE_FUNCTION();

    ULONG nbVertices = m_Vertices.size();
    ULONG i;

    ClearEdges();

    for (i = 0; i < nbVertices; ++i)
    {
      VertexPtr* vertexPtr = m_Vertices[i];
      Vertex*    vertex    = vertexPtr->ptr;

      if (vertex == NULL)
        continue;

      VertexPtrNode* neighbor = vertex->neighbors;
      while (neighbor != NULL)
      {
        ComputeOneEdge(vertexPtr, neighbor->element, false, -1.0f, NULL);

        neighbor = neighbor->next;
      }
    }
  }

  bool MeshBuilder::IsCollapseFoldingMesh(const D3DXVECTOR3& _collapsePoint, Vertex* _vertex1, Vertex* _vertex2)
  {
    XMP_PROFILE_FUNCTION();

    Vertex* vertices[2] = { _vertex1, _vertex2 };
    ULONG   i;

    for (i = 0; i < 2; ++i)
    {
      FacePtrNode* facePtr = vertices[i]->faces;
      while (facePtr != NULL)
      {
        Face* face = facePtr->element->ptr;

        if (face != NULL)
        {
          bool hasV1 = face->HasVertex(_vertex1);
          bool hasV2 = face->HasVertex(_vertex2);

          if (hasV1 ^ hasV2)
          {
            const D3DXVECTOR3* faceVertices[3];
            D3DXPLANE plane;
            ULONG     index;

            for (index = 0; index < 3; ++index)
            {
              if (face->vertices[index]->ptr == _vertex1)
                faceVertices[index] = &_collapsePoint;
              else if (face->vertices[index]->ptr == _vertex2)
                faceVertices[index] = &_collapsePoint;
              else
                faceVertices[index] = &face->vertices[index]->ptr->GetPosition();
            }

            D3DXVECTOR3 v1 = *faceVertices[2] - *faceVertices[0];
            D3DXVECTOR3 v2 = *faceVertices[1] - *faceVertices[0];
            D3DXVECTOR3 v;

            D3DXVec3Cross(&v, &v1, &v2);

            FLOAT length = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

            // Make sure we don't create degenerate triangles
            if (length < MESH_BUILDER_EPSILON)
              return true;

            v *= 1.0f / length;

            // Make sure we don't flip the face
            FLOAT fDot = (face->plane.a * v.x) + (face->plane.b * v.y) + (face->plane.c * v.z);
            if (fDot <= 0.0f)
              return true;
          }
        }

        facePtr = facePtr->next;
      }
    }

    return false;
  }

  bool MeshBuilder::IsSharedCollapseValid(const D3DXVECTOR3& _collapsePoint, Vertex* _vertex1, Vertex* _vertex2)
  {
      // Only test this when running in safe mode
      if (!m_SafeMode)
          return true;

      if (_vertex1->IsBoundary() && _vertex2->IsBoundary())
      {
          ULONG collapseFlags = GetValidBoundaryCollapse(_vertex1, _vertex2);

          // Make sure we can actually collapse this edge
          if (collapseFlags == 0)
              return false;
      }

      return true;
  }

  bool MeshBuilder::ComputeOneEdge(VertexPtr* _vertex1, VertexPtr* _vertex2, bool _isShared, FLOAT _target, FLOAT* _cost)
  {
    XMP_PROFILE_FUNCTION();

    D3DXVECTOR3 bestFit;
    D3DXVECTOR2 bestFitUV;
    FLOAT lerpTarget;

    if (_vertex1 == _vertex2)
      return false;

    if (!m_MaterialInfo[_vertex1->ptr->data.elementIndex].IsSimplificationEnabled() && !_isShared)
      return false;

    if (IsEdgePresent(_vertex1, _vertex2, true))
      return false;

    if (_isShared)
    {
      D3DXVec3Lerp(&bestFit,   &_vertex1->ptr->data.pos->position, &_vertex2->ptr->data.pos->position, _target);
      D3DXVec2Lerp(&bestFitUV, &_vertex1->ptr->data.texCoord,      &_vertex2->ptr->data.texCoord,      _target);

      // Make sure we really can collapse the vertices
      if (!IsSharedCollapseValid(bestFit, _vertex1->ptr, _vertex2->ptr))
          return false;

      Quadric quadric(_vertex1->ptr->quadric);
      quadric += _vertex2->ptr->quadric;
      *_cost   = quadric.Evaluate(&bestFit, &bestFitUV);

      return true;
    }
    else
    {
      lerpTarget = FindCollapsePoint(&bestFit, _vertex1->ptr, _vertex2->ptr);
      if (lerpTarget < 0.0f)
        return false;
    }

    D3DXVec2Lerp(&bestFitUV, &_vertex1->ptr->data.texCoord, &_vertex2->ptr->data.texCoord, lerpTarget);

    Quadric quadric(_vertex1->ptr->quadric);
    quadric   += _vertex2->ptr->quadric;
    FLOAT cost = quadric.Evaluate(&bestFit, &bestFitUV);

    // Check the shared vertices and edges
    if (!ComputeSharedEdges(_vertex1, _vertex2, lerpTarget, &cost))
      return false;

    Edge* edge        = m_Allocators.edgeAllocator.Allocate();
    edge->vertices[0] = _vertex1;
    edge->vertices[1] = _vertex2;
    edge->cost        = cost * m_MaterialInfo[_vertex1->ptr->data.elementIndex].GetCostMultiplier();
    edge->lerpTarget  = lerpTarget;
    EdgePtr* edgePtr  = m_Allocators.edgePtrAllocator.Allocate();
    edgePtr->ptr      = edge;
    m_Edges.push_back(edgePtr);

    // Add the edge to both vertices
    EdgePtrNode* oldNode             = _vertex1->ptr->edges;
    _vertex1->ptr->edges          = m_Allocators.edgePtrNodeAllocator.Allocate();
    _vertex1->ptr->edges->element = edgePtr;
    _vertex1->ptr->edges->next    = oldNode;

    oldNode                          = _vertex2->ptr->edges;
    _vertex2->ptr->edges          = m_Allocators.edgePtrNodeAllocator.Allocate();
    _vertex2->ptr->edges->element = edgePtr;
    _vertex2->ptr->edges->next    = oldNode;

    return true;
  }

  bool MeshBuilder::ComputeSharedEdges(VertexPtr* _vertex1, VertexPtr* _vertex2, FLOAT _target, FLOAT* _cost)
  {
    VertexPtrNode* sharedV1Ptr = _vertex1->ptr->data.pos->vertices;

    while (sharedV1Ptr != NULL)
    {
      Vertex* sharedV1 = sharedV1Ptr->element->ptr;

      if ((sharedV1Ptr->element != _vertex1) && (sharedV1 != NULL))
      {
        VertexPtrNode* sharedV2Ptr = _vertex2->ptr->data.pos->vertices;

        while (sharedV2Ptr != NULL)
        {
          Vertex* sharedV2 = sharedV2Ptr->element->ptr;

          if ((sharedV2Ptr->element != _vertex2) && (sharedV2 != NULL))
          {
            FacePtrNode* faceV1Ptr = sharedV1->faces;

            while (faceV1Ptr != NULL)
            {
              Face* face = faceV1Ptr->element->ptr;

              if ((face != NULL) && (face->HasVertex(sharedV2)))
              {
                FLOAT cost;

                if (!ComputeOneEdge(sharedV1Ptr->element, sharedV2Ptr->element, true, _target, &cost))
                  return false;

                *_cost += cost;
              }

              faceV1Ptr = faceV1Ptr->next;
            }
          }

          sharedV2Ptr = sharedV2Ptr->next;
        }
      }

      sharedV1Ptr = sharedV1Ptr->next;
    }

    return true;
  }

  bool MeshBuilder::IsEdgePresent(VertexPtr* _vertex1, VertexPtr* _vertex2, bool _checkShared)
  {
    VertexPtr* vertices[2] = { _vertex1, _vertex2 };
    ULONG   i;

    for (i = 0; i < 2; ++i)
    {
      EdgePtrNode* edgePtr = vertices[i]->ptr->edges;
      while (edgePtr != NULL)
      {
        if (edgePtr->element->ptr != NULL)
        {
          Edge* edge = edgePtr->element->ptr;

          if (((edge->vertices[0] == vertices[0]) && (edge->vertices[1] == vertices[1])) ||
              ((edge->vertices[0] == vertices[1]) && (edge->vertices[1] == vertices[0])))
            return true;
        }

        edgePtr = edgePtr->next;
      }
    }

    if (!_checkShared)
      return false;

    // Check the shared edges
    VertexPtrNode* v1Node = _vertex1->ptr->data.pos->vertices;
    while (v1Node != NULL)
    {
      if ((v1Node->element->ptr != NULL) && (v1Node->element != _vertex1))
      {
        VertexPtrNode* v2Node = _vertex2->ptr->data.pos->vertices;
        while (v2Node != NULL)
        {
          if ((v2Node->element->ptr != NULL) && (v2Node->element != _vertex2))
          {
            if (IsEdgePresent(v1Node->element, v2Node->element, false))
              return true;
          }

          v2Node = v2Node->next;
        }
      }

      v1Node = v1Node->next;
    }

    return false;
  }

  void MeshBuilder::ComputeVertexNormals(bool _skipNormalize)
  {
    ULONG nbVertices = m_Vertices.size();
    ULONG i;

    for (i = 0; i < nbVertices; ++i)
    {
      VertexPtr* vertexPtr = m_Vertices[i];
      Vertex*    vertex    = vertexPtr->ptr;

      if (vertex == NULL)
        continue;

      vertex->normal.x = vertex->normal.y = vertex->normal.z = 0.0f;

      FacePtrNode* facePtr = vertex->faces;
      while (facePtr != NULL)
      {
        Face* face = facePtr->element->ptr;

        if (face != NULL)
        {
          vertex->normal.x -= face->plane.a;
          vertex->normal.y -= face->plane.b;
          vertex->normal.z -= face->plane.c;
        }

        facePtr = facePtr->next;
      }

      if (!_skipNormalize)
      {
        FLOAT vecLength = D3DXVec3Length(&vertex->normal);
        if (vecLength > 0.0f)
        {
          vertex->normal /= vecLength;
        }
      }
    }
  }

  void MeshBuilder::SetMaterialInfo(BYTE _index, LONG _materialId, ULONG _processingFlags, 
                                    const D3DXMATRIX& _transform, UCHAR* _heightMap, LONG _width, LONG _height, 
                                    FLOAT _displacementOffset, FLOAT _displacementMax, 
                                    FLOAT _maxTriArea, FLOAT _smoothThreshold, 
                                    FLOAT _chamferThreshold, FLOAT _chamferLength, FLOAT _chamferWeldThreshold)
  {
    MaterialInfo* info = &m_MaterialInfo[_index];

    if (_maxTriArea < MESH_BUILDER_MIN_TRIANGLE_AREA)
        _maxTriArea = MESH_BUILDER_MIN_TRIANGLE_AREA;

    info->SetMaterialId(_materialId);
    info->SetFlags(_processingFlags);
    info->SetTransform(_transform);
    info->SetHeightMap(_heightMap, _width, _height);
    info->SetDisplacement(_displacementOffset, _displacementMax);
    info->SetMaxTriangleArea(_maxTriArea);
    info->SetMaxSearchDistance(MESH_BUILDER_DEFAULT_KDTREE_DISTANCE);
    info->SetSmoothThreshold(_smoothThreshold);
    info->SetChamfer(_chamferThreshold, _chamferLength, _chamferWeldThreshold);
  }

  LONG MeshBuilder::GetMaterialId(BYTE _index)
  {
    if (_index >= MESH_BUILDER_MAX_ELEMENTS)
      return 0;

    return m_MaterialInfo[_index].GetMaterialId();
  }

  void MeshBuilder::SetOutputFunction(OutputFunction _outputFunction)
  {
    s_OutputFunction = _outputFunction;
  }

  void MeshBuilder::SetProgressTickFunction(ProgressTickFunction _progressTickFunction)
  {
    s_ProgressTickFunction = _progressTickFunction;
  }

  void MeshBuilder::Tick(void)
  {
    if (s_ProgressTickFunction == NULL)
      return;

    s_ProgressTickFunction();
  }

  PositionInfo* MeshBuilder::FindPosition(FLOAT _x, FLOAT _y, FLOAT _z)
  {
    ULONG nbPositions = m_Positions.size();
    ULONG i;

    for (i = 0; i < nbPositions; ++i)
    {
      PositionInfo* pos = m_Positions[i];

      if (IsPositionEqual(_x, _y, _z, pos->position.x, pos->position.y, pos->position.z, MESH_BUILDER_EPSILON))
        return pos;
    }

    return NULL;
  }

  PositionInfo* MeshBuilder::PushPosition(FLOAT _x, FLOAT _y, FLOAT _z)
  {
    PositionInfo* pos = GetAllocators()->positionInfoAllocator.Allocate();

    pos->position.x = _x;
    pos->position.y = _y;
    pos->position.z = _z;
    pos->vertices   = NULL;

    m_Positions.push_back(pos);

    return pos;
  }

};

#endif // ACTIVE_EDITORS && _XENON_RENDER
