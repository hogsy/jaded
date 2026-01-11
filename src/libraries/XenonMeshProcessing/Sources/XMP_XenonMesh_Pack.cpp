// ------------------------------------------------------------------------------------------------
// File   : XMP_XenonMesh_Pack.cpp
// Date   : 2005-07-11
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

  const FLOAT MESH_PACK_POSITION_EPSILON = 0.000005f;
  const FLOAT MESH_PACK_WEIGHT_EPSILON   = 0.01f;
  const FLOAT MESH_PACK_COLOR_EPSILON    = 0.01f;
  const FLOAT MESH_PACK_UV_EPSILON       = 0.0005f;

// ------------------------------------------------------------------------------------------------
// PRIVATE CLASSES
// ------------------------------------------------------------------------------------------------

  class MeshPacker
  {
  public:

    MeshPacker(PackVertexArray* _vertices, PackUVArray* _texCoords, PackTriangleArray* _triangles);
    ~MeshPacker(void);

    void AddVertex(const D3DXVECTOR3& _position, 
                   const D3DXVECTOR4& _weights, 
                   const D3DXCOLOR&   _color, 
                   const D3DXVECTOR2& _texCoord,
                   ULONG              _elementIndex);

  private:

    ULONG AddPWC(const D3DXVECTOR3& _position, 
                 const D3DXVECTOR4& _weights, 
                 const D3DXCOLOR&   _color);

    ULONG AddUV(const D3DXVECTOR2& _texCoord);

  private:

    PackVertexArray*   m_Vertices;
    PackUVArray*       m_TexCoords;
    PackTriangleArray* m_Triangles;

    PackTriangle m_NewTriangle;
    ULONG        m_VertexIndex;
  };

// ------------------------------------------------------------------------------------------------
// EXTERNAL FUNCTIONS
// ------------------------------------------------------------------------------------------------
  extern void Log(LogLevel _level, const CHAR* _format, ...);

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

  void MeshBuilder::GetVertexWeightColor(Vertex* _vertex, D3DXVECTOR4* _weights, D3DXCOLOR* _color)
  {
    std::vector<D3DXVECTOR3>      positions;
    std::vector<StaticVertexData> vertexData;
    std::vector<float>            squareDistances;

    MaterialInfo* info            = &m_MaterialInfo[_vertex->data.elementIndex];
    const FLOAT   maxSearchDist   = info->GetMaxSearchDistance();
    const FLOAT   maxSqSearchDist = maxSearchDist * maxSearchDist;

    ULONG i;

    // Recover the vertex weights and color using the KD-Tree
    // We are doing this because the weights and colors are interpolated when generating new
    // vertices, but they have absolutely no impact on mesh simplification so they may be way
    // off once we are ready to build the final mesh.
    D3DXCOLOR newColor(0.0f, 0.0f, 0.0f, 0.0f);
    D3DXCOLOR newWeights(0.0f, 0.0f, 0.0f, 0.0f);
    FLOAT weight = 0.0f;
    ULONG nbRes  = (ULONG)info->GetDataTree()->FindNeighbors(_vertex->data.pos->position, maxSearchDist, 
                                                             positions, vertexData, &squareDistances);
    for (i = 0; i < nbRes; ++i)
    {
      FLOAT vertexWeight     = maxSqSearchDist - squareDistances[i];
      vertexData[i].color   *= vertexWeight;
      vertexData[i].weights *= vertexWeight;
      newColor              += vertexData[i].color;
      newWeights            += vertexData[i].weights;
      weight                += vertexWeight;
    }

    // Use the KD-Tree only if displacement was enabled
    if ((weight > 0.0f) && info->IsDisplacementEnabled())
    {
      FLOAT weightLen = sqrtf((newWeights.r * newWeights.r) +
        (newWeights.g * newWeights.g) +
        (newWeights.b * newWeights.b) +
        (newWeights.a * newWeights.a));
      if (weightLen >= MESH_BUILDER_EPSILON)
      {
        newWeights *= 1.0f / weightLen;
      }

      newColor   *= 1.0f / weight;
      newWeights *= 1.0f / weight;
      _weights->x = newWeights.r;
      _weights->y = newWeights.g;
      _weights->z = newWeights.b;
      _weights->w = newWeights.a;
      *_color     = newColor;
    }
    else
    {
      D3DXCOLOR weights(_vertex->data.weights);
      FLOAT weightLen = sqrtf((weights.r * weights.r) +
                              (weights.g * weights.g) +
                              (weights.b * weights.b) +
                              (weights.a * weights.a));
      if (weightLen >= MESH_BUILDER_EPSILON)
      {
        weights *= 1.0f / weightLen;
      }

      _weights->x = weights.r;
      _weights->y = weights.g;
      _weights->z = weights.b;
      _weights->w = weights.a;
      *_color     = _vertex->data.color;
    }
  }

  void MeshBuilder::GetPackedMesh(PackVertexArray& _rVertices, PackUVArray& _rTexCoords, PackTriangleArray& _rTriangles)
  {
    XMP_PROFILE_FUNCTION();

    MeshPacker  meshPacker(&_rVertices, &_rTexCoords, &_rTriangles);
    ULONG       nbFaces = m_Faces.size();
    D3DXCOLOR   color;
    D3DXVECTOR4 weight;
    ULONG i;
    ULONG j;

    for (i = 0; i < nbFaces; ++i)
    {
      Face* face = m_Faces[i]->ptr;

      if (face != NULL)
      {
        if ((face->vertices[0]->ptr == NULL) ||
            (face->vertices[1]->ptr == NULL) ||
            (face->vertices[2]->ptr == NULL))
          continue;

        for (j = 0; j < 3; ++j)
        {
          GetVertexWeightColor(face->vertices[j]->ptr, &weight, &color);

          meshPacker.AddVertex(face->vertices[j]->ptr->data.pos->position, 
                               weight, color, face->vertices[j]->ptr->data.texCoord, 
                               face->vertices[j]->ptr->data.elementIndex);
        }
      }
    }

#if defined(MESH_BUILDER_VERBOSE)
    Log(LL_INFO, "XMP - GetPackedMesh -> %u vertices to %u vertices & %u texture coordinates", 
        m_NbVertices, (ULONG)_rVertices.size(), (ULONG)_rTexCoords.size());
#endif
  }

  void MeshBuilder::GetSimpleMesh(SimpleVertexArray& _rVertices, SimpleFaceArray& _rFaces)
  {
    XMP_PROFILE_FUNCTION();

    std::vector<D3DXVECTOR3>      positions;
    std::vector<StaticVertexData> vertexData;
    std::vector<float>            squareDistances;

    _rVertices.clear();
    _rFaces.clear();

    // Apply auto-smooth on the mesh
    AutoSmoothMesh();

    ComputeVertexNormals();

    ULONG maxVertices = m_Vertices.size();
    ULONG maxFaces    = m_Faces.size();
    ULONG nbVertices  = 0;
    ULONG i;
    ULONG j;

    for (i = 0; i < maxVertices; ++i)
    {
      VertexPtr* vertexPtr = m_Vertices[i];
      Vertex*    vertex    = vertexPtr->ptr;

      if (vertex != NULL)
      {
        MaterialInfo* info            = &m_MaterialInfo[vertex->data.elementIndex];
        const FLOAT   maxSearchDist   = info->GetMaxSearchDistance();
        const FLOAT   maxSqSearchDist = maxSearchDist * maxSearchDist;

        SimpleVertex newVertex;
        newVertex.position = vertex->GetPosition();
        newVertex.normal   = vertex->normal;
        newVertex.texCoord = vertex->data.texCoord;

        // Recover the vertex weights and color using the KD-Tree
        // We are doing this because the weights and colors are interpolated when generating new
        // vertices, but they have absolutely no impact on mesh simplification so they may be way
        // off once we are ready to build the final mesh.
        D3DXCOLOR newColor(0.0f, 0.0f, 0.0f, 0.0f);
        D3DXCOLOR newWeights(0.0f, 0.0f, 0.0f, 0.0f);
        FLOAT weight = 0.0f;
        ULONG nbRes  = (ULONG)info->GetDataTree()->FindNeighbors(newVertex.position, maxSearchDist, 
                                                                 positions, vertexData, &squareDistances);
        for (j = 0; j < nbRes; ++j)
        {
          FLOAT vertexWeight     = maxSqSearchDist - squareDistances[j];
          vertexData[j].color   *= vertexWeight;
          vertexData[j].weights *= vertexWeight;
          newColor              += vertexData[j].color;
          newWeights            += vertexData[j].weights;
          weight                += vertexWeight;
        }

        // Use the KD-Tree only if displacement was enabled
        if ((weight > 0.0f) && info->IsDisplacementEnabled())
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
          newVertex.weights.x = newWeights.r;
          newVertex.weights.y = newWeights.g;
          newVertex.weights.z = newWeights.b;
          newVertex.weights.w = newWeights.a;
          newVertex.color     = (DWORD)newColor;
        }
        else
        {
          D3DXCOLOR weights(vertex->data.weights);
          FLOAT weightLen = sqrtf((weights.r * weights.r) +
                                  (weights.g * weights.g) +
                                  (weights.b * weights.b) +
                                  (weights.a * weights.a));
          if (weightLen >= MESH_BUILDER_EPSILON)
          {
              weights *= 1.0f / weightLen;
          }

          newVertex.weights.x = weights.r;
          newVertex.weights.y = weights.g;
          newVertex.weights.z = weights.b;
          newVertex.weights.w = weights.a;
          newVertex.color     = vertex->data.color;
        }

        _rVertices.push_back(newVertex);

        vertex->SetIndex(nbVertices);
        ++nbVertices;
      }
    }

    for (i = 0; i < maxFaces; ++i)
    {
      FacePtr* facePtr = m_Faces[i];
      Face*    face    = facePtr->ptr;

      if (face != NULL)
      {
        SimpleFace newFace;
        newFace.indices[0]   = (USHORT)face->vertices[0]->ptr->GetIndex();
        newFace.indices[1]   = (USHORT)face->vertices[1]->ptr->GetIndex();
        newFace.indices[2]   = (USHORT)face->vertices[2]->ptr->GetIndex();
        newFace.elementIndex = face->elementIndex;

        _rFaces.push_back(newFace);
      }
    }
  }

  void MeshBuilder::AutoSmoothMesh(void)
  {
    FacePtrNode* faceNodePtr;
    ULONG nbVertices    = m_Vertices.size();
    ULONG nbFaces       = m_Faces.size();
    ULONG nbOriVertices = m_NbVertices;
    ULONG i;
    ULONG j;
    FLOAT dot;
    FLOAT angle;
    FLOAT smoothThreshold;
    bool  warnedOverflow = false;

    // Clear all the edges since we are going to use the edge nodes to duplicate vertices
    ClearEdges();

    // Reset all the vertex indices and faces
    for (i = 0; i < nbVertices; ++i)
    {
      if (m_Vertices[i]->ptr != NULL)
      {
        m_Vertices[i]->ptr->SetIndex(0);
        m_Vertices[i]->ptr->faces = NULL;
      }
    }

    // Reset the face node pool, we are going to rebuild the face hierarchy in the next step
    m_Allocators.facePtrNodeAllocator.Reset();

    // Parse all the faces and duplicate vertices when the normal difference is greater than the smooth threshold
    for (i = 0; i < nbFaces; ++i)
    {
      FacePtr* facePtr = m_Faces[i];
      Face*    face    = facePtr->ptr;

      // We're alive!
      if ((i % 500) == 0)
        Tick();

      if (face == NULL)
        continue;

      D3DXVECTOR3 faceNormal(face->plane.a, face->plane.b, face->plane.c);

      smoothThreshold = m_MaterialInfo[face->vertices[0]->ptr->data.elementIndex].GetSmoothThreshold();

      for (j = 0; j < 3; ++j)
      {
        VertexPtr* vertexPtr = face->vertices[j];
        Vertex*    vertex    = vertexPtr->ptr;

        if (vertex->GetIndex() == 0)
        {
          // Free vertex, mark as used
          vertex->SetIndex(1);
          vertex->normal = faceNormal;

          faceNodePtr          = m_Allocators.facePtrNodeAllocator.Allocate();
          faceNodePtr->element = facePtr;
          faceNodePtr->next    = vertex->faces;
          vertex->faces        = faceNodePtr;
        }
        else
        {
          dot   = D3DXVec3Dot(&vertex->normal, &faceNormal);
          angle = acosf(dot);

          // Stop creating new vertices if we reached the maximum count
          if (m_NbVertices >= m_MaxVertexCount)
          {
            angle = smoothThreshold;
            if (!warnedOverflow)
            {
              Log(LL_WARNING, "XMP - AutoSmoothMesh - Too many vertices in the mesh, auto smooth threshold will not be applied");
              warnedOverflow = true;
            }
          }

          // Angle too big, we must use another vertex
          if (angle > smoothThreshold)
          {
            EdgePtrNode* edgePtr = vertex->edges;
            while (edgePtr != NULL)
            {
              dot   = D3DXVec3Dot(&edgePtr->element->ptr->vertices[1]->ptr->normal, &faceNormal);
              angle = acosf(dot);

              if (angle <= smoothThreshold)
                break;

              edgePtr = edgePtr->next;
            }

            if (edgePtr == NULL)
            {
              VertexPtr* newVertexPtr = PushVertex(0.0f, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0, NULL);
              Vertex*    newVertex    = newVertexPtr->ptr;

              newVertex->data = vertex->data;

              edgePtr                            = m_Allocators.edgePtrNodeAllocator.Allocate();
              edgePtr->element                   = m_Allocators.edgePtrAllocator.Allocate();
              edgePtr->element->ptr              = m_Allocators.edgeAllocator.Allocate();
              edgePtr->element->ptr->vertices[0] = vertexPtr;
              edgePtr->element->ptr->vertices[1] = newVertexPtr;
              edgePtr->next                      = vertex->edges;
              vertex->edges                      = edgePtr;

              // Set the right normal and mark as used
              newVertex->normal = faceNormal;
              newVertex->SetIndex(1);

              // Branch on the selected vertex
              face->vertices[j]->ptr = newVertex;

              faceNodePtr          = m_Allocators.facePtrNodeAllocator.Allocate();
              faceNodePtr->element = facePtr;
              faceNodePtr->next    = newVertex->faces;
              newVertex->faces     = faceNodePtr;
            }
            else
            {
              // Branch on the selected vertex
              face->vertices[j] = edgePtr->element->ptr->vertices[1];

              faceNodePtr                                        = m_Allocators.facePtrNodeAllocator.Allocate();
              faceNodePtr->element                               = facePtr;
              faceNodePtr->next                                  = edgePtr->element->ptr->vertices[1]->ptr->faces;
              edgePtr->element->ptr->vertices[1]->ptr->faces = faceNodePtr;

              // Recompute the normal
              faceNormal.x = faceNormal.y = faceNormal.z = 0.0f;
              faceNodePtr  = edgePtr->element->ptr->vertices[1]->ptr->faces;
              while (faceNodePtr != NULL)
              {
                faceNormal.x += faceNodePtr->element->ptr->plane.a;
                faceNormal.y += faceNodePtr->element->ptr->plane.b;
                faceNormal.z += faceNodePtr->element->ptr->plane.c;
                faceNodePtr   = faceNodePtr->next;
              }
              D3DXVec3Normalize(&faceNormal, &faceNormal);
              edgePtr->element->ptr->vertices[1]->ptr->normal = faceNormal;
            }
          }
          else
          {
            faceNodePtr          = m_Allocators.facePtrNodeAllocator.Allocate();
            faceNodePtr->element = facePtr;
            faceNodePtr->next    = vertex->faces;
            vertex->faces        = faceNodePtr;

            // Recompute the normal
            faceNormal.x = faceNormal.y = faceNormal.z = 0.0f;
            faceNodePtr  = vertex->faces;
            while (faceNodePtr != NULL)
            {
                faceNormal.x += faceNodePtr->element->ptr->plane.a;
                faceNormal.y += faceNodePtr->element->ptr->plane.b;
                faceNormal.z += faceNodePtr->element->ptr->plane.c;
                faceNodePtr   = faceNodePtr->next;
            }
            D3DXVec3Normalize(&faceNormal, &faceNormal);
            vertex->normal = faceNormal;
          }
        }
      }
    }

#if defined(MESH_BUILDER_VERBOSE)
    Log(LL_INFO, "XMP - AutoSmoothMesh -> %u vertices to %u vertices (%u faces)", 
        nbOriVertices, m_NbVertices, m_NbFaces);
#endif

#if defined(_XMP_MEMORY_LOG) && defined(_XMP_TRACK_MEMORY)
    XMP_ShowMemoryInfo("Auto Smooth");
    DumpAllocatorUsages();
#endif
  }

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - MeshPacker
// ------------------------------------------------------------------------------------------------

  MeshPacker::MeshPacker(PackVertexArray* _vertices, PackUVArray* _texCoords, PackTriangleArray* _triangles)
    : m_VertexIndex(0), m_Vertices(_vertices), m_TexCoords(_texCoords), m_Triangles(_triangles)
  {
    m_Vertices->clear();
    m_TexCoords->clear();
    m_Triangles->clear();
  }

  MeshPacker::~MeshPacker(void)
  {
  }

  void MeshPacker::AddVertex(const D3DXVECTOR3& _position, 
                             const D3DXVECTOR4& _weights, 
                             const D3DXCOLOR&   _color, 
                             const D3DXVECTOR2& _texCoord,
                             ULONG              _elementIndex)
  {
    m_NewTriangle.vertexIndices[m_VertexIndex] = AddPWC(_position, _weights, _color);
    m_NewTriangle.uvIndices[m_VertexIndex]     = AddUV(_texCoord);
    ++m_VertexIndex;

    if (m_VertexIndex == 3)
    {
      m_NewTriangle.elementIndex = _elementIndex;

      m_Triangles->push_back(m_NewTriangle);

      m_VertexIndex = 0;
    }
  }

  ULONG MeshPacker::AddPWC(const D3DXVECTOR3& _position, 
                           const D3DXVECTOR4& _weights, 
                           const D3DXCOLOR&   _color)
  {
    ULONG nbVertices = m_Vertices->size();
    ULONG i;

    for (i = 0; i < nbVertices; ++i)
    {
      PackVertex& vertex = (*m_Vertices)[i];

      if (!IsPositionEqual(_position, vertex.position, MESH_PACK_POSITION_EPSILON))
        continue;

      if (!IsWeightEqual(_weights, vertex.weights, MESH_PACK_WEIGHT_EPSILON))
        continue;

      if (!IsColorEqual(_color, vertex.color, MESH_PACK_COLOR_EPSILON))
        continue;

      return i;
    }

    PackVertex newVertex;
    newVertex.position = _position;
    newVertex.weights  = _weights;
    newVertex.color    = _color;

    m_Vertices->push_back(newVertex);

    return nbVertices;
  }

  ULONG MeshPacker::AddUV(const D3DXVECTOR2& _texCoord)
  {
    ULONG nbTexCoords = m_TexCoords->size();
    ULONG i;

    for (i = 0; i < nbTexCoords; ++i)
    {
      PackUV& uv = (*m_TexCoords)[i];

      if (!IsTexCoordEqual(_texCoord, uv.texCoord, MESH_PACK_UV_EPSILON))
        continue;

      return i;
    }

    PackUV newUV;
    newUV.texCoord = _texCoord;

    m_TexCoords->push_back(newUV);

    return nbTexCoords;
  }

};

#endif // ACTIVE_EDITORS && _XENON_RENDER
