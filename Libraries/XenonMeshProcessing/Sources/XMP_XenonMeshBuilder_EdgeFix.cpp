// ------------------------------------------------------------------------------------------------
// File   : XMP_XenonMeshBuilder_EdgeFix.cpp
// Date   : 2005-09-13
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
// EXTERNAL FUNCTIONS
// ------------------------------------------------------------------------------------------------
  extern void Log(LogLevel _level, const CHAR* _format, ...);

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - PointCloud
// ------------------------------------------------------------------------------------------------

  PointCloud::PointCloud(void)
  {
    m_VertexPool.Initialize(4096);
  }

  PointCloud::~PointCloud(void)
  {
  }

  void PointCloud::AddVertex(FLOAT _x, FLOAT _y, FLOAT _z)
  {
    Vertex* newVertex = m_VertexPool.Allocate();

    newVertex->position.x = _x;
    newVertex->position.y = _y;
    newVertex->position.z = _z;
    m_Vertices.push_back(newVertex);
  }

  FLOAT PointCloud::GetSplitFactor(const D3DXVECTOR3& _pos1,
                                   const D3DXVECTOR3& _pos2,
                                   D3DXVECTOR3*       _resPos)
  {
    VertexArray vertices;
    D3DXVECTOR3 center = _pos1;
    D3DXVECTOR3 delta  = _pos2;
    FLOAT       sqRadius;

    center += _pos2;
    center *= 0.5f;

    delta -= _pos1;

    sqRadius = D3DXVec3LengthSq(&delta) * 0.25f;

    if (!FindMatchingVertices(center, sqRadius, &vertices))
      return -1.0f;

    ULONG nbRes = vertices.size();
    ULONG i;

    FLOAT length = D3DXVec3Length(&delta);
    delta *= 1.0f / length;

    for (i = 0; i < nbRes; ++i)
    {
      D3DXVECTOR3 dir = vertices[i]->position;
      dir            -= _pos1;

      D3DXVECTOR3 nrmDir;
      D3DXVec3Normalize(&nrmDir, &dir);

      if (fabsf(D3DXVec3Dot(&nrmDir, &delta)) < (1.0f - MESH_BUILDER_EPSILON))
        continue;

      FLOAT dirLength = D3DXVec3Length(&dir);

      FLOAT ratio = dirLength / length;

      if ((ratio <= 0.0005f) || (ratio >= 0.9995f))
        continue;

      *_resPos = vertices[i]->position;

      return ratio;
    }

    return -1.0f;
  }

  bool PointCloud::FindMatchingVertices(const D3DXVECTOR3& _pos, FLOAT _sqRadius, VertexArray* _vertices)
  {
    ULONG nbVertices = m_Vertices.size();
    ULONG i;

    _vertices->clear();

    // SC: TODO: Use a KD-Tree instead

    for (i = 0; i < nbVertices; ++i)
    {
      Vertex* vertex = m_Vertices[i];

      D3DXVECTOR3 delta = _pos;
      delta            -= vertex->position;
      FLOAT sqDist      = D3DXVec3LengthSq(&delta);

      if (sqDist <= _sqRadius)
      {
        _vertices->push_back(vertex);
      }
    }

    return !_vertices->empty();
  }

  bool PointCloud::FindSnapTarget(const D3DXVECTOR3& _pos, D3DXVECTOR3* _resPos)
  {
    const FLOAT FST_EPSILON = 0.001f;

    VertexArray matches;

    if (!FindMatchingVertices(_pos, FST_EPSILON, &matches))
      return false;

    *_resPos = matches[0]->position;

    return true;
  }

  void PointCloud::Optimize(void)
  {
    // SC: TODO: Build the KD-Tree
  }

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

  void MeshBuilder::RemoveTJunctions(PointCloud* _pointCloud)
  {
    XMP_PROFILE_FUNCTION();

    XMP_ASSERT(!m_CanAddVertices);

#if defined(MESH_BUILDER_VERBOSE)
    ULONG nbOriVertices = m_Vertices.size();
    ULONG nbOriFaces    = m_Faces.size();
#endif

    ULONG nbIterations = 0;

    if (m_CanAddVertices)
      return;

    // Prepare the point cloud
    _pointCloud->Optimize();

    // Snap the edge vertices to the surrounding vertices if possible (will make it easier to match edges)
    SnapVerticesOnPointCloud(_pointCloud);

    // Split-Repeat until T-Junctions are gone
    while (SplitAtTJunctions(_pointCloud))
    {
      ++nbIterations;
    }

#if defined(MESH_BUILDER_VERBOSE)
    Log(LL_INFO, "XMP - RemoveTJunctions -> %u faces (%u vertices) to %u faces (%u vertices) using %u iterations", 
        nbOriFaces, nbOriVertices, m_NbFaces, m_NbVertices, nbIterations);
#endif
  }

  void MeshBuilder::SnapVerticesOnPointCloud(PointCloud* _pointCloud)
  {
    XMP_PROFILE_FUNCTION();

    D3DXVECTOR3 snapPos;

    // Mark the boundaries since we are only going to snap them
    FindMeshBoundaries();

    ULONG nbVertices = m_Vertices.size();
    ULONG i;

    for (i = 0; i < nbVertices; ++i)
    {
      VertexPtr* vertex = m_Vertices[i];

      if (vertex == NULL)
        continue;

      if (vertex->ptr == NULL)
        continue;

      if (!vertex->ptr->IsBoundary())
        continue;

      if (_pointCloud->FindSnapTarget(vertex->ptr->data.pos->position, &snapPos))
      {
        vertex->ptr->data.pos->position = snapPos;
      }
    }
  }

  bool MeshBuilder::SplitAtTJunctions(PointCloud* _pointCloud)
  {
    XMP_PROFILE_FUNCTION();

    EdgeDatabase   edges;
    SplitFaceArray newFaces;
    D3DXVECTOR3    tempVector;
    D3DXVECTOR3    resPos;

    bool  atLeastOneSplit = false;
    ULONG nbFaces         = m_Faces.size();
    ULONG i;
    ULONG j;

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

      for (j = 0; j < 3; ++j)
      {
        if (newFace->edges[j]->splitVertex != NULL)
          continue;

        FLOAT splitFactor = _pointCloud->GetSplitFactor(newFace->edges[j]->vertices[0]->ptr->GetPosition(),
                                                        newFace->edges[j]->vertices[1]->ptr->GetPosition(), &resPos);

        if ((splitFactor > 0.0f) && (splitFactor < 1.0f))
        {
          VertexPtr* splitVertexPtr = PushVertex(0.0f, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0, NULL);
          Vertex*    splitVertex    = splitVertexPtr->ptr;

          InterpolateVertex(&splitVertex->data, 
                            newFace->edges[j]->vertices[0]->ptr, 
                            newFace->edges[j]->vertices[1]->ptr, splitFactor);
          newFace->edges[j]->splitVertex = splitVertexPtr;

          // Snap the position to the one we found
          splitVertex->data.pos->position = resPos;

          atLeastOneSplit = true;
        }
        /*
        else
        {
          // Edges that are too long should be split since they may introduce discontinuities
          const FLOAT STJ_MAX_EDGE_LENGTH = 0.5f;

          D3DXVECTOR3 temp1;
          D3DXVECTOR3 temp2;
          if (_pointCloud->FindSnapTarget(newFace->edges[j]->vertices[0]->ptr->GetPosition(), &temp1) &&
              _pointCloud->FindSnapTarget(newFace->edges[j]->vertices[1]->ptr->GetPosition(), &temp2))
          {
            D3DXVECTOR3 edgeVec = newFace->edges[j]->vertices[0]->ptr->GetPosition();
            edgeVec            -= newFace->edges[j]->vertices[1]->ptr->GetPosition();

            FLOAT edgeLen = D3DXVec3Length(&edgeVec);
            if (edgeLen > STJ_MAX_EDGE_LENGTH)
            {
              VertexPtr* splitVertexPtr = PushVertex(0.0f, 0.0f, 0.0f, 0, 0, 0.0f, 0.0f, 0, NULL);
              Vertex*    splitVertex    = splitVertexPtr->ptr;

              InterpolateVertex(&splitVertex->data, 
                                newFace->edges[j]->vertices[0]->ptr, 
                                newFace->edges[j]->vertices[1]->ptr, 0.5f);
              newFace->edges[j]->splitVertex = splitVertexPtr;

              atLeastOneSplit = true;
            }
          }
        }
        */
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

    return atLeastOneSplit;
  }

};

#endif // ACTIVE_EDITORS && _XENON_RENDER
