// ----------------------------------------------------------------------------
// Author : Sebastien Comte
// File   : XMP_XenonMeshKDTree.h
// Date   : 2005-06-05
// Descr. : 
//
// Inspired by J. Andreas Bærentzen's K-D Tree implementation
// http://www.imm.dtu.dk/~jab/publications.html
//
// ----------------------------------------------------------------------------

#ifndef GUARD_XMP_XENONMESHKDTREE_H
#define GUARD_XMP_XENONMESHKDTREE_H

#if defined(ACTIVE_EDITORS)

#include "XMP_XenonMeshUtils.h"

namespace XenonMeshPack
{

// ----------------------------------------------------------------------------
// CLASSES
// ----------------------------------------------------------------------------
  template<const ULONG K, class Key, class Val>
  class KDTree
  {
  private:

    struct Node
    {
      Key  key;
      Val  val;
      LONG axis;

      inline Node(void)
        : axis(0)
      {
      }

      inline FLOAT GetSquareDistance(const Key& _ref)
      {
        Key diff = key;
        diff    -= _ref;

        FLOAT squareDist = 0.0f;
        for (LONG i = 0; i < K; ++i)
        {
          squareDist += diff[i] * diff[i];
        }

        return squareDist;
      }
    };
    typedef std::vector<Node> NodeArray;

    class NodeCompare
    {
      // Stupid C4512 warning
      NodeCompare& operator= (const NodeCompare&);

    public:

      NodeCompare(LONG _axis)
        : baseAxis(_axis)
      {
      }

      bool operator() (const Key& _k1, const Key& _k2) const
      {
        for (LONG i = 0; i < K; ++i)
        {
          LONG j = (baseAxis + i) % K;

          if (_k1[j] < _k2[j])
            return true;
          if (_k1[j] > _k2[j])
            return false;
        }

        return false;
      }

      bool operator() (const Node& _n1, const Node& _n2) const
      {
        return (*this)(_n1.key, _n2.key);
      }

    private:

      const LONG baseAxis;
    };

  public:

    XMP_DECLARE_NEW_AND_DELETE();

    inline KDTree(void)
      : m_NbElements(0), m_MaxDepth(0)
    {
    }

    inline ~KDTree(void)
    {
    }

    inline void Clear(void)
    {
      m_InitialList.clear();
      m_Tree.clear();

      m_NbElements = 0;
      m_MaxDepth   = 0;
    }

    inline void AddElement(const Key& _key, const Val& _val)
    {
      Node node;

      node.key  = _key;
      node.val  = _val;
      node.axis = -1;

      m_InitialList.push_back(node);
    }

    inline void Build(void)
    {
      m_NbElements = m_InitialList.size();

      // The actual tree starts at element 1
      m_Tree.resize(m_NbElements + 1);

      if (m_NbElements > 0)
        Optimize(1, 0, m_NbElements, 0);

      // Free the initial node array
      NodeArray clearArray;
      m_InitialList.swap(clearArray);
    }

    inline bool FindClosestPoint(const Key& _ref, FLOAT& _dist, Key& _resKey, Val& _resVal)
    {
      FLOAT squareDist = _dist * _dist;
      LONG index       = FindClosestPoint(1, _ref, squareDist);
      if (index > 0)
      {
        _resKey = m_Tree[index].key;
        _resVal = m_Tree[index].val;
        _dist   = sqrtf(squareDist);
        return true;
      }

      return false;
    }

    inline LONG FindNeighbors(const Key& _ref, FLOAT _dist, std::vector<Key>& _resKeys, std::vector<Val>& _resVals, std::vector<float>* _resSqDist = NULL)
    {
      FLOAT squareDist = _dist * _dist;

      _resKeys.clear();
      _resVals.clear();

      if (_resSqDist != NULL)
      {
        _resSqDist->clear();
      }

      FindNeighbors(1, _ref, squareDist, _resKeys, _resVals, _resSqDist);

      return _resKeys.size();
    }

  private:

    inline LONG FindBestAxis(LONG _min, LONG _max)
    {
      Key minKey = m_InitialList[_min].key;
      Key maxKey = m_InitialList[_min].key;

      for (LONG i = _min + 1; i < _max; ++i)
      {
        for (LONG j = 0; j < K; ++j)
        {
          if (m_InitialList[i].key[j] < minKey[j])
            minKey[j] = m_InitialList[i].key[j];
          if (m_InitialList[i].key[j] > maxKey[j])
            maxKey[j] = m_InitialList[i].key[j];
        }
      }

      LONG axis   = 0;
      Key  extend = maxKey - minKey;

      for (LONG i = 1; i < K; ++i)
      {
        if (extend[i] > extend[axis])
          axis = i;
      }

      return axis;
    }

    inline void Optimize(LONG _index, LONG _min, LONG _max, LONG _level)
    {
      XMP_ASSERT(_index < (LONG)m_Tree.size());

      // Only one element, insert as is
      if (_min + 1 == _max)
      {
        m_MaxDepth = Max(_level, m_MaxDepth);
        m_Tree[_index]      = m_InitialList[_min];
        m_Tree[_index].axis = -1;
        return;
      }

      // Find the best axis to split the remaining nodes
      LONG axis = FindBestAxis(_min, _max);

      // Compute the median element
      LONG N         = _max - _min;
      LONG M         = (1 << MaxLog2(N));
      LONG R         = N - (M - 1);
      LONG leftSize  = ((M - 2) >> 1);
      LONG rightSize = ((M - 2) >> 1);

      if (R < (M >> 1))
      {
        leftSize += R;
      }
      else
      {
        leftSize  += (M >> 1);
        rightSize += R - (M >> 1);
      }

      LONG median = _min + leftSize;

      // Sort the elements using nth_element so that all elements on the
      // left side of the median will be smaller or equal to the median and
      // all of those on the right side will be greater or equal
      NodeCompare comparator(axis);
      std::nth_element(&m_InitialList[_min], &m_InitialList[median], &m_InitialList[_max], comparator);

      // Insert the node
      m_Tree[_index]      = m_InitialList[median];
      m_Tree[_index].axis = axis;

      if (leftSize > 0)
        Optimize(2 * _index, _min, median, _level + 1);

      if (rightSize > 0)
        Optimize(2 * _index + 1, median + 1, _max, _level + 1);
    }

    inline LONG FindClosestPoint(LONG _index, const Key& _ref, FLOAT& _squareDist)
    {
      LONG  ret  = 0;
      FLOAT dist = m_Tree[_index].GetSquareDistance(_ref);

      if (dist < _squareDist)
      {
        _squareDist = dist;
        ret         = _index;
      }

      if (m_Tree[_index].axis != -1)
      {
        LONG  axis           = m_Tree[_index].axis;
        FLOAT axisSquareDist = m_Tree[_index].key[axis] - _ref[axis];
        bool  isLeft         = NodeCompare(axis)(_ref, m_Tree[_index].key);

        if (isLeft || (axisSquareDist < _squareDist))
        {
          LONG leftChildIndex = 2 * _index;

          if (leftChildIndex < (LONG)m_Tree.size())
          {
            LONG leftIndex = FindClosestPoint(leftChildIndex, _ref, _squareDist);
            if (leftIndex > 0)
            {
              ret = leftIndex;
            }
          }
        }

        if (!isLeft || (axisSquareDist < _squareDist))
        {
          LONG rightChildIndex = 2 * _index + 1;

          if (rightChildIndex < (LONG)m_Tree.size())
          {
            LONG rightIndex = FindClosestPoint(rightChildIndex, _ref, _squareDist);
            if (rightIndex > 0)
            {
              ret = rightIndex;
            }
          }
        }
      }

      return ret;
    }

    inline void FindNeighbors(LONG _index, const Key& _ref, FLOAT _squareDist, std::vector<Key>& _resKeys, std::vector<Val>& _resVals, std::vector<float>* _resSqDist)
    {
      FLOAT dist = m_Tree[_index].GetSquareDistance(_ref);

      if (dist < _squareDist)
      {
        _resKeys.push_back(m_Tree[_index].key);
        _resVals.push_back(m_Tree[_index].val);

        if (_resSqDist != NULL)
        {
          _resSqDist->push_back(dist);
        }
      }

      if (m_Tree[_index].axis != -1)
      {
        LONG  axis           = m_Tree[_index].axis;
        FLOAT axisSquareDist = m_Tree[_index].key[axis] - _ref[axis];
        bool  isLeft         = NodeCompare(axis)(_ref, m_Tree[_index].key);

        if (isLeft || (axisSquareDist < _squareDist))
        {
          LONG leftChildIndex = 2 * _index;

          if (leftChildIndex < (LONG)m_Tree.size())
          {
            FindNeighbors(leftChildIndex, _ref, _squareDist, _resKeys, _resVals, _resSqDist);
          }
        }

        if (!isLeft || (axisSquareDist < _squareDist))
        {
          LONG rightChildIndex = 2 * _index + 1;

          if (rightChildIndex < (LONG)m_Tree.size())
          {
            FindNeighbors(rightChildIndex, _ref, _squareDist, _resKeys, _resVals, _resSqDist);
          }
        }
      }
    }

  private:

    NodeArray m_InitialList;
    NodeArray m_Tree;

    LONG m_NbElements;
    LONG m_MaxDepth;
  };

};

#endif

#endif // #ifdef GUARD_XMP_XENONMESHKDTREE_H
