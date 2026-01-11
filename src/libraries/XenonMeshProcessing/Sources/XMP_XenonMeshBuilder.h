// ----------------------------------------------------------------------------
// Author : Sebastien Comte
// File   : XMP_XenonMeshBuilder.h
// Date   : 2005-05-28
// Descr. : 
// ----------------------------------------------------------------------------

#ifndef GUARD_XMP_XENONMESHBUILDER_HPP
#define GUARD_XMP_XENONMESHBUILDER_HPP

#if defined(ACTIVE_EDITORS)

#include "XMP_XenonMeshUtils.h"
#include "XMP_XenonMeshMath.h"
#include "XMP_XenonMeshKDTree.h"

#if defined(_DEBUG) || defined(RELEASE)
#define MESH_BUILDER_VERBOSE
#endif

// ----------------------------------------------------------------------------
// DEFINITIONS
// ----------------------------------------------------------------------------
const FLOAT MESH_BUILDER_ONE_BY_255        = 1.0f / 255.0f;
const FLOAT MESH_BUILDER_EPSILON           = 0.000005f;
const FLOAT MESH_BUILDER_UV_EPSILON        = 0.01f;
const FLOAT MESH_BUILDER_MIN_TRIANGLE_AREA = 0.00001f;

const ULONG MESH_BUILDER_MAX_ELEMENTS = 256;

const FLOAT MESH_BUILDER_MIN_KDTREE_DISTANCE     = 0.0001f;
const FLOAT MESH_BUILDER_DEFAULT_KDTREE_DISTANCE = 0.1f;

const FLOAT MESH_BUILDER_PI = 3.141592654f;

const FLOAT MESH_BUILDER_EDGE_LENGTH_THRESHOLD = 0.4f;

namespace XenonMeshPack
{

// ----------------------------------------------------------------------------
// FORWARD DECLARATIONS
// ----------------------------------------------------------------------------
  class  MeshBuilder;
  class  Quadric;
  class  ChamferDatabase;

  struct Vertex;
  struct SimpleVertex;
  struct VertexPair;
  struct Face;
  struct SimpleFace;
  struct Edge;
  struct PositionInfo;
  struct PackVertex;
  struct PackUV;
  struct PackTriangle;

  typedef DeferredPointer<Vertex> VertexPtr;
  typedef DeferredPointer<Face>   FacePtr;
  typedef DeferredPointer<Edge>   EdgePtr;

  typedef std::vector<Vertex*>    VertexArray;
  typedef std::vector<Face*>      FaceArray;
  typedef std::vector<Edge*>      EdgeArray;

  typedef std::vector<VertexPtr*> VertexPtrArray;
  typedef std::vector<FacePtr*>   FacePtrArray;
  typedef std::vector<EdgePtr*>   EdgePtrArray;

  typedef std::vector<SimpleVertex> SimpleVertexArray;
  typedef std::vector<SimpleFace>   SimpleFaceArray;

  typedef std::vector<PositionInfo*> PositionInfoArray;

  typedef std::vector<VertexPair> VertexPairArray;

  typedef std::vector<PackVertex>   PackVertexArray;
  typedef std::vector<PackUV>       PackUVArray;
  typedef std::vector<PackTriangle> PackTriangleArray;

  enum LogLevel
  {
    LL_DEBUG   = 0,
    LL_INFO    = 1,
    LL_WARNING = 2,
    LL_ERROR   = 3,

    LL_COUNT
  };

  typedef void (*OutputFunction)(LogLevel _level, CHAR* _msg);
  typedef void (*ProgressTickFunction)(void);

  enum ProcessingFlags
  {
    PF_CHAMFER     = 0x00000001,
    PF_TESSELLATE  = 0x00000002,
    PF_DISPLACE    = 0x00000004,

    PF_FORCE_DWORD = 0xffffffff
  };

// ----------------------------------------------------------------------------
// STRUCTURES
// ----------------------------------------------------------------------------

  // Basic linked list node
  template<class T>
  struct Node
  {
    XMP_DECLARE_NEW_AND_DELETE();

    T*       element;
    Node<T>* next;
  };

  typedef Node<Vertex>    VertexNode;
  typedef Node<VertexPtr> VertexPtrNode;
  typedef Node<Face>      FaceNode;
  typedef Node<FacePtr>   FacePtrNode;
  typedef Node<Edge>      EdgeNode;
  typedef Node<EdgePtr>   EdgePtrNode;

  // Position (3 floats) + Texture coordinates (2 floats)
  class Quadric
  {
  public:

    XMP_DECLARE_NEW_AND_DELETE();

    inline Quadric(void);
    inline Quadric(const Quadric& _q);
    inline ~Quadric(void);

    inline Quadric& operator= (const Quadric& _q);

    inline Quadric& operator+= (const Quadric& _q);

    inline void Clear(void);

    void Set(const D3DXVECTOR3& _pos1, const D3DXVECTOR2& _uv1, 
             const D3DXVECTOR3& _pos2, const D3DXVECTOR2& _uv2, 
             const D3DXVECTOR3& _pos3, const D3DXVECTOR2& _uv3);

    FLOAT Evaluate(const D3DXVECTOR3* _pos, const D3DXVECTOR2* _texCoord) const;
    bool  Optimize(D3DXVECTOR3* _newPos) const;

  private:

    typedef SymmetricMatrix<5> QSMatrix;
    typedef Matrix<5>          QMatrix;
    typedef Vector<5>          QVector;

    QSMatrix m_A;
    QVector  m_B;
    FLOAT    m_C;
  };

  struct PositionInfo
  {
    D3DXVECTOR3    position;
    VertexPtrNode* vertices;

    inline void AddVertex(VertexPtr* _vertex);
  };

  struct VertexPair
  {
    VertexPtr* vertices[2];
  };

  struct VertexData
  {
    XMP_DECLARE_NEW_AND_DELETE();

    PositionInfo* pos;
    ULONG         weights;
    ULONG         color;
    D3DXVECTOR2   texCoord;
    BYTE          elementIndex;
  };

  struct Vertex
  {
    XMP_DECLARE_NEW_AND_DELETE();

    inline void Reset(void);

    VertexData     data;
    D3DXVECTOR3    normal;
    VertexPtrNode* neighbors;
    FacePtrNode*   faces;
    EdgePtrNode*   edges;
    Quadric        quadric;
    ULONG          vertexIndex;

    inline const D3DXVECTOR3& GetPosition(void) const;
    inline D3DXVECTOR3& GetPosition(void);

    inline bool IsNeighbor(const VertexPtr* _vertex) const;
    inline void AddNeighbor(VertexPtr* _vertex);
    inline void ReplaceNeighbor(VertexPtr* _oldVertex, VertexPtr* _newVertex);

    inline void SetValid(bool _valid);
    inline bool IsValid(void) const;

    inline void SetBoundary(bool _boundary);
    inline bool IsBoundary(void) const;

    inline void  SetIndex(ULONG _index);
    inline ULONG GetIndex(void) const;
  };

  struct SimpleVertex
  {
    XMP_DECLARE_NEW_AND_DELETE();

    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    D3DXVECTOR4 weights;
    ULONG       color;
    D3DXVECTOR2 texCoord;
  };

  struct Face
  {
    XMP_DECLARE_NEW_AND_DELETE();

    VertexPtr* vertices[3];
    D3DXPLANE  plane;
    FLOAT      area;
    BYTE       elementIndex;

    inline void Compute(void);
    inline void GetQuadric(Quadric* _quadric) const;
    inline bool HasVertex(const Vertex* _vertex) const;
    inline bool HasVertex(const VertexPtr* _vertex) const;
    inline void ReplaceVertex(const VertexPtr* _ori, VertexPtr* _new);
  };

  struct SimpleFace
  {
    XMP_DECLARE_NEW_AND_DELETE();

    USHORT indices[3];
    BYTE   elementIndex;
  };

  struct Edge
  {
    XMP_DECLARE_NEW_AND_DELETE();

    VertexPtr* vertices[2];
    FLOAT      cost;
    FLOAT      lerpTarget;
  };

  struct SplitEdge
  {
    XMP_DECLARE_NEW_AND_DELETE();

    VertexPtr* vertices[2];
    VertexPtr* splitVertex;
  };
  typedef std::map<ULONG64, SplitEdge*> SplitEdgeMap;

  struct SplitFace
  {
    XMP_DECLARE_NEW_AND_DELETE();

    VertexPtr* vertices[3];
    SplitEdge* edges[3];
  };
  typedef std::vector<SplitFace*> SplitFaceArray;

  struct AllAllocators
  {
    XMP_DECLARE_NEW_AND_DELETE();

    DefragPoolAllocator<Vertex>             vertexAllocator;
    CachedPoolAllocator<VertexPtrNode, 256> vertexPtrNodeAllocator;

    PoolAllocator<PositionInfo>             positionInfoAllocator;
    PoolAllocator<VertexPtrNode>            positionLinkAllocator;

    DefragPoolAllocator<Face>               faceAllocator;
    CachedPoolAllocator<FacePtrNode, 256>   facePtrNodeAllocator;
    PoolAllocator<SplitFace>                splitFaceAllocator;

    CachedPoolAllocator<Edge, 256>          edgeAllocator;
    PoolAllocator<EdgePtr>                  edgePtrAllocator;
    CachedPoolAllocator<EdgePtrNode, 256>   edgePtrNodeAllocator;
  };

  struct StaticVertexData
  {
    XMP_DECLARE_NEW_AND_DELETE();

    D3DXCOLOR weights;
    D3DXCOLOR color;
  };

  typedef KDTree<3, D3DXVECTOR3, StaticVertexData> StaticVertexDataKDTree;

  struct PackVertex
  {
    XMP_DECLARE_NEW_AND_DELETE();

    D3DXVECTOR3 position;
    D3DXVECTOR4 weights;
    D3DXCOLOR   color;
  };

  struct PackUV
  {
    XMP_DECLARE_NEW_AND_DELETE();

    D3DXVECTOR2 texCoord;
  };

  struct PackTriangle
  {
    XMP_DECLARE_NEW_AND_DELETE();

    ULONG elementIndex;
    ULONG vertexIndices[3];
    ULONG uvIndices[3];
  };

// ----------------------------------------------------------------------------
// CLASSES
// ----------------------------------------------------------------------------

  class EdgeDatabase
  {
  public:

    inline EdgeDatabase(void);
    inline ~EdgeDatabase(void);

    inline ULONG64 BuildKey(VertexPtr* _v1, VertexPtr* _v2);

    inline SplitEdge* FindEdge(VertexPtr* _v1, VertexPtr* _v2);
    inline SplitEdge* AddEdge(VertexPtr* _v1, VertexPtr* _v2);

    bool IsEdgeTessellated(SplitEdge* _edge);

  private:

    SplitEdgeMap             m_Edges;
    PoolAllocator<SplitEdge> m_EdgePool;
  };

  class PointCloud
  {
    struct Vertex;

    typedef std::vector<Vertex*> VertexArray;

    struct Vertex
    {
      XMP_DECLARE_NEW_AND_DELETE();

      D3DXVECTOR3 position;
    };

  public:

    XMP_DECLARE_NEW_AND_DELETE();

    PointCloud(void);
    ~PointCloud(void);

    void AddVertex(FLOAT _x, FLOAT _y, FLOAT _z);

    void Optimize(void);

    FLOAT GetSplitFactor(const D3DXVECTOR3& _pos1,
                         const D3DXVECTOR3& _pos2,
                         D3DXVECTOR3*       _resPos);

    bool FindSnapTarget(const D3DXVECTOR3& _pos, D3DXVECTOR3* _resPos);

  private:

    bool FindMatchingVertices(const D3DXVECTOR3& _pos, FLOAT _sqRadius, VertexArray* _vertices);

  private:

    VertexArray m_Vertices;

    PoolAllocator<Vertex> m_VertexPool;
  };

  class MaterialInfo
  {
  public:

    XMP_DECLARE_NEW_AND_DELETE();

    MaterialInfo(void);
    ~MaterialInfo(void);

    inline void SetMaterialId(LONG _id);
    inline void SetFlags(ULONG _processingFlags);
    inline void SetTransform(const D3DXMATRIX& _xform);
    inline void SetHeightMap(UCHAR* _map, LONG _width, LONG _height);
    inline void SetDisplacement(FLOAT _offset, FLOAT _max);
    inline void SetMaxTriangleArea(FLOAT _maxArea);
    inline void SetMaxSearchDistance(FLOAT _maxSearchDistance);
    inline void SetSmoothThreshold(FLOAT _smoothThreshold);
    inline void SetCostMultiplier(FLOAT _costMultiplier);
    inline void SetChamfer(FLOAT _threshold, FLOAT _length, FLOAT _weldThreshold);

    inline LONG              GetMaterialId(void) const;
    inline bool              IsChamferEnabled(void) const;
    inline bool              IsTessellationEnabled(void) const;
    inline bool              IsDisplacementEnabled(void) const;
    inline bool              IsSimplificationEnabled(void) const;
    inline const D3DXMATRIX* GetTransform(void) const;
    inline FLOAT             GetDisplacementOffset(void) const;
    inline FLOAT             GetDisplacementMax(void) const;
    inline FLOAT             GetMaxTriangleArea(void) const;
    inline FLOAT             GetMaxSearchDistance(void) const;
    inline FLOAT             GetSmoothThreshold(void) const;
    inline FLOAT             GetCostMultiplier(void) const;
    inline FLOAT             GetChamferThreshold(void) const;
    inline FLOAT             GetChamferLength(void) const;
    inline FLOAT             GetChamferWeldThreshold(void) const;

    inline StaticVertexDataKDTree* GetDataTree(void);

    FLOAT GetBaseDisplacement(FLOAT _u, FLOAT _v);

  private:

    inline FLOAT FetchTexel(LONG _u, LONG _v);
    inline FLOAT SampleHeightMap(FLOAT _u, FLOAT _v);

  private:

    LONG       m_MaterialId;
    ULONG      m_ProcessingFlags;
    D3DXMATRIX m_TexCoordTransform;
    UCHAR*     m_HeightMap;
    LONG       m_HeightMapWidth;
    LONG       m_HeightMapHeight;
    FLOAT      m_DisplacementOffset;
    FLOAT      m_DisplacementMax;
    FLOAT      m_MaxTriangleArea;
    FLOAT      m_MaxSearchDistance;
    FLOAT      m_SmoothThreshold;
    FLOAT      m_CostMultiplier;
    FLOAT      m_ChamferThreshold;
    FLOAT      m_ChamferLength;
    FLOAT      m_ChamferWeldThreshold;

    StaticVertexDataKDTree m_DataTree;
  };

  typedef std::map<ULONG, MaterialInfo*> MaterialInfoMap;

  class MeshBuilder
  {
    XMP_DECLARE_NEW_AND_DELETE();

  public:

    MeshBuilder(void);
    ~MeshBuilder(void);

    void SetSafeMode(bool _enable);

    void BeginVertices(void);
    void AddVertex(FLOAT _x, FLOAT _y, FLOAT _z, 
                   FLOAT _w1, FLOAT _w2, FLOAT _w3, FLOAT _w4, 
                   ULONG _color, 
                   FLOAT _u, FLOAT _v, 
                   BYTE _elementIndex);
    void EndVertices(void);

    void Chamfer(void);
    void Tessellate(void);
    void Displace(void);
    void Simplify(FLOAT _maxCost);

    void SnapVerticesOnPointCloud(PointCloud* _pointCloud);
    void RemoveTJunctions(PointCloud* _pointCloud);

    void GetPackedMesh(PackVertexArray& _rVertices, PackUVArray& _rTexCoords, PackTriangleArray& _rTriangles);
    void GetSimpleMesh(SimpleVertexArray& _rVertices, SimpleFaceArray& _rFaces);

    // SC: TODO: Allow the heightmap information to be shared since many elements in the same mesh
    //           might use the same normal map...
    void SetMaterialInfo(BYTE _index, LONG _materialId, ULONG _processingFlags, 
                         const D3DXMATRIX& _transform, UCHAR* _heightMap, LONG _width, LONG _height, 
                         FLOAT _displacementOffset, FLOAT _displacementMax, FLOAT _maxTriArea, 
                         FLOAT _smoothThreshold, 
                         FLOAT _chamferThreshold, FLOAT _chamferLength, FLOAT _chamferWeldThreshold);
    LONG GetMaterialId(BYTE _index);

    inline static AllAllocators* GetAllocators(void);

    inline void SetMaximumVertexCount(ULONG _maxVertices);
    inline void SetMaximumFaceCount(ULONG _maxFaceCount);

    static void SetOutputFunction(OutputFunction _outputFunction);
    static void SetProgressTickFunction(ProgressTickFunction _progressTickFunction);

  private:

    void DumpAllocatorUsages(void);

    void Tick(void);

    // General
    void Clear(void);
    void ClearEdges(void);
    void ClearFaces(void);

    PositionInfo* FindPosition(FLOAT _x, FLOAT _y, FLOAT _z);
    PositionInfo* PushPosition(FLOAT _x, FLOAT _y, FLOAT _z);

    VertexPtr* FindVertex(FLOAT _x, FLOAT _y, FLOAT _z, ULONG _weights, ULONG _color, 
                          FLOAT _u, FLOAT _v, BYTE _elementIndex, PositionInfo** _positionInfo);
    VertexPtr* PushVertex(FLOAT _x, FLOAT _y, FLOAT _z, ULONG _weights, ULONG _color, 
                          FLOAT _u, FLOAT _v, BYTE _elementIndex, PositionInfo* _positionInfo);
    void       PushFace(VertexPtr* _v1, VertexPtr* _v2, VertexPtr* _v3);

    inline void RemoveVertex(VertexPtr* _vertex);
    inline void RemoveFace(FacePtr* _face);

    void GetVertexWeightColor(Vertex* _vertex, D3DXVECTOR4* _weights, D3DXCOLOR* _color);

    // Geometry
    void FindMeshBoundaries(void);
    void ComputeVertexNormals(bool _skipNormalize = false);

    // Tessellation
    bool IsSplitNecessary(void);
    bool SplitTriangles(void);

    // Displacement
    FLOAT GetVertexDisplacementFactor(const Vertex* _vertex) const;

    // Simplification
    void  ComputeAllQuadrics(void);
    void  ComputeVertexQuadric(Vertex* _vertex);
    void  ComputeEdges(void);
    bool  ComputeOneEdge(VertexPtr* _vertex1, VertexPtr* _vertex2, bool _isShared, FLOAT _target, FLOAT* _cost);
    bool  ComputeSharedEdges(VertexPtr* _vertex1, VertexPtr* _vertex2, FLOAT _target, FLOAT* _cost);
    void  CollapseEdge(Edge* _edge, VertexPtrArray* _newVertices);
    void  UpdateForCollapse(VertexPtr* _vertex1, VertexPtr* _vertex2, VertexPtr* _newVertex, FLOAT _target, VertexPtrArray* _newVertices, VertexPtrArray* _deadVertices);
    bool  IsEdgePresent(VertexPtr* _vertex1, VertexPtr* _vertex2, bool _checkShared);
    FLOAT FindCollapsePoint(D3DXVECTOR3* _res, Vertex* _vertex1, Vertex* _vertex2);
    void  InterpolateVertex(VertexData* _res, Vertex* _v1, Vertex* _v2, FLOAT _t);
    void  ComputeBestPositions(Vertex* _vertex1, Vertex* _vertex2, const Quadric* _quadric, FLOAT* _values);
    FLOAT ComputeMinCostPosition(Vertex* _vertex1, Vertex* _vertex2, D3DXVECTOR3* _computedPosition);
    ULONG GetValidBoundaryCollapse(Vertex* _vertex1, Vertex* _vertex2);
    bool  IsCollapseFoldingMesh(const D3DXVECTOR3& _collapsePoint, Vertex* _vertex1, Vertex* _vertex2);
    bool  IsSharedCollapseValid(const D3DXVECTOR3& _collapsePoint, Vertex* _vertex1, Vertex* _vertex2);

    // T-Junctions removal
    bool SplitAtTJunctions(PointCloud* _pointCloud);

    void CleanMesh(void);

    void AutoSmoothMesh(void);

  private:

    static AllAllocators* s_AllAllocators;

    AllAllocators m_Allocators;

    bool m_CanAddVertices;

    VertexPtrArray    m_Vertices;
    FacePtrArray      m_Faces;
    EdgePtrArray      m_Edges;
    PositionInfoArray m_Positions;

    VertexPtr* m_LastVertices[3];
    ULONG      m_NbLastVertices;

    MaterialInfo* m_MaterialInfo;

    ULONG m_MaxVertexCount;
    ULONG m_MaxFaceCount;
    ULONG m_NbVertices;
    ULONG m_NbFaces;

    bool m_SafeMode;
  };

};

// Inline implementation
#include "XMP_XenonMeshBuilder.inl"

#endif // ACTIVE_EDITORS

#endif // #ifdef GUARD_XMP_XENONMESHBUILDER_HPP
