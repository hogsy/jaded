// ------------------------------------------------------------------------------------------------
// File   : XMP_XenonMeshUtils.h
// Date   : 2005-06-01
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XMP_XENONMESHUTILS_H
#define GUARD_XMP_XENONMESHUTILS_H

#if defined(ACTIVE_EDITORS)

#define _XMP_TRACK_MEMORY

namespace XenonMeshPack
{
  // Note: I am not using Jade's memory allocators since we are going to use
  //       quit a huge amount of memory and I don't wan't Jade to run out of
  //       memory while processing meshes...

// ----------------------------------------------------------------------------
// UTILITIES
// ----------------------------------------------------------------------------

#define XMP_PROFILE_FUNCTION XEProfileFunction
#define XMP_PROFILE_SCOPE    XEProfileScope

#define XMP_ASSERT     ERR_X_Assert

#if defined(_XMP_TRACK_MEMORY)
#define XMP_ALLOC(_size)         XenonMeshPack::XMP_Alloc(_size, __FILE__, __LINE__)
#define XMP_REALLOC(_ptr, _size) XenonMeshPack::XMP_Realloc(_ptr, _size, __FILE__, __LINE__)
#define XMP_FREE(_ptr)           XenonMeshPack::XMP_Free(_ptr)
#else
#define XMP_ALLOC(_size)         malloc(_size)
#define XMP_REALLOC(_ptr, _size) realloc(_ptr, _size)
#define XMP_FREE(_ptr)           free(_ptr)
#endif

#define XMP_DECLARE_NEW_AND_DELETE()                                 \
  void* operator new(unsigned int size)                              \
  {                                                                  \
    return XMP_ALLOC(size);                                          \
  }                                                                  \
                                                                     \
  void* operator new(unsigned int, void* ptr)                        \
  {                                                                  \
    return ptr;                                                      \
  }                                                                  \
                                                                     \
  void* operator new[](unsigned int size)                            \
  {                                                                  \
    return XMP_ALLOC(size);                                          \
  }                                                                  \
                                                                     \
  void* operator new[](unsigned int, void* ptr)                      \
  {                                                                  \
    return ptr;                                                      \
  }                                                                  \
                                                                     \
  void operator delete(void* ptr)                                    \
  {                                                                  \
    XMP_FREE(ptr);                                                   \
  }                                                                  \
                                                                     \
  void operator delete(void*, void*)                                 \
  {                                                                  \
  }                                                                  \
                                                                     \
  void operator delete[](void* ptr)                                  \
  {                                                                  \
    XMP_FREE(ptr);                                                   \
  }                                                                  \
                                                                     \
  void operator delete[](void*, void*)                               \
  {                                                                  \
  }

#if defined(_XMP_TRACK_MEMORY)
  void* XMP_Alloc(ULONG _size, const CHAR* _fileName, INT _line);
  void* XMP_Realloc(void* _ptr, ULONG _size, const CHAR* _fileName, INT _line);
  void  XMP_Free(void* _ptr);
  ULONG XMP_DumpMemory(void);
#endif

// ----------------------------------------------------------------------------
// FUNCTIONS
// ----------------------------------------------------------------------------

  template<class T>
  inline T Lerp(const T& _a, const T& _b, const T& _t)
  {
    return _a + (_t * (_b - _a));
  }

  template<class T>
  inline T Min(const T& _a, const T& _b)
  {
    return (_a > _b) ? _a : _b;
  }

  template<class T>
  inline T Max(const T& _a, const T& _b)
  {
    return (_a > _b) ? _a : _b;
  }

  template<class T>
  inline T Clamp(const T& _val, const T& _min, const T& _max)
  {
    if (_val < _min)
      return _min;
    else if (_val > _max)
      return _max;
    return _val;
  }

  inline ULONG MakeColor(BYTE _r, BYTE _g, BYTE _b, BYTE _a)
  {
    return ((_a << 24) | (_r << 16) | (_g << 8) | _b);
  }

  inline LONG MaxLog2(LONG _val)
  {
    if (_val < 1)
      return -1;

    LONG i = 0;

    while (_val != 1)
    {
      _val >>= 1;
      ++i;
    }

    return i;
  }

// ----------------------------------------------------------------------------
// CLASSES
// ----------------------------------------------------------------------------

  // Pool allocator
  template<class T>
  class PoolAllocator
  {
    struct Pool
    {
      T*    buffer;
      ULONG nbElements;
    };
    typedef std::list<Pool*> PoolList;

  public:

    XMP_DECLARE_NEW_AND_DELETE();

    inline PoolAllocator(void)
      : m_MaxElementsPerPool(1)
    {
    }

    virtual inline ~PoolAllocator(void)
    {
      Shutdown();
    }

    virtual inline void Initialize(ULONG _nbEntriesPerPool)
    {
      Shutdown();

      m_MaxElementsPerPool = _nbEntriesPerPool;
    }

    virtual inline void Shutdown(void)
    {
      PoolList::iterator it = m_Pools.begin();

      while (it != m_Pools.end())
      {
        XMP_FREE((*it)->buffer);
        XMP_FREE((*it));
        ++it;
      }
      m_Pools.clear();
    }

    virtual inline void Reset(void)
    {
      PoolList::iterator it = m_Pools.begin();

      while (it != m_Pools.end())
      {
        Pool* pool = (*it);

        pool->nbElements = 0;

        ++it;
      }
    }

    virtual inline T* Allocate(void)
    {
      PoolList::iterator it = m_Pools.begin();

      while (it != m_Pools.end())
      {
        Pool* pool = (*it);

        if (pool->nbElements < m_MaxElementsPerPool)
        {
          ++pool->nbElements;

          return (pool->buffer + (pool->nbElements - 1));
        }

        ++it;
      }

      Pool* newPool  = RequestPool();
      ++newPool->nbElements;

      return newPool->buffer;
    }

    inline ULONG GetPoolMemoryUsage(void) const
    {
      return m_Pools.size() * m_MaxElementsPerPool * sizeof(T);
    }

    inline ULONG GetMaxElementsPerPool(void) const
    {
      return m_MaxElementsPerPool;
    }

    inline ULONG GetElementSize(void) const
    {
      return sizeof(T);
    }

  private:

    inline Pool* RequestPool(void)
    {
      Pool* pool = (Pool*)XMP_ALLOC(sizeof(Pool));

      pool->buffer     = (T*)XMP_ALLOC(m_MaxElementsPerPool * sizeof(T));
      pool->nbElements = 0;

      m_Pools.push_front(pool);

      return pool;
    }

  private:

    PoolList m_Pools;
    ULONG    m_MaxElementsPerPool;
  };


  // PoolAllocator with a small cache to reuse freed entries.
  // Meant to be used in cases where more nodes are freed than allocated
  template<class T, const ULONG CacheSize>
  class CachedPoolAllocator : public PoolAllocator<T>
  {
  public:

    XMP_DECLARE_NEW_AND_DELETE();

    inline CachedPoolAllocator(void)
    {
      m_CacheSize = 0;
    }

    virtual inline ~CachedPoolAllocator(void)
    {
    }

    virtual inline void Initialize(ULONG _nbEntriesPerPool)
    {
      PoolAllocator<T>::Initialize(_nbEntriesPerPool);

      m_CacheSize = 0;
    }

    virtual inline void Shutdown(void)
    {
      PoolAllocator<T>::Shutdown();

      m_CacheSize = 0;
    }

    virtual inline void Reset(void)
    {
      PoolAllocator<T>::Reset();

      m_CacheSize = 0;
    }

    virtual inline T* Allocate(void)
    {
      if (m_CacheSize > 0)
      {
        --m_CacheSize;
        return m_Cache[m_CacheSize];
      }

      return PoolAllocator<T>::Allocate();
    }

    inline void Free(T* _element)
    {
      if (m_CacheSize < CacheSize)
      {
        m_Cache[m_CacheSize++] = _element;
      }
    }

  private:

    T*    m_Cache[CacheSize];
    ULONG m_CacheSize;
  };


  // Basic deferred pointer
  template<class T>
  struct DeferredPointer
  {
    XMP_DECLARE_NEW_AND_DELETE();

    T* ptr;
  };

  // Pool allocator that can defrag itself
  template<class T>
  class DefragPoolAllocator
  {

    struct PoolEntry
    {
      T                   data;
      DeferredPointer<T>* ptr;
    };

    struct Pool
    {
      PoolEntry* buffer;
      ULONG      nbElements;
    };
    typedef std::list<Pool*> PoolList;

  public:

    XMP_DECLARE_NEW_AND_DELETE();

    inline DefragPoolAllocator(void)
      : m_MaxElementsPerPool(1)
    {
    }

    virtual inline ~DefragPoolAllocator(void)
    {
      Shutdown();
    }

    virtual inline void Initialize(ULONG _nbEntriesPerPool)
    {
      Shutdown();

      m_MaxElementsPerPool = _nbEntriesPerPool;

      m_NodeAllocator.Initialize(_nbEntriesPerPool);
    }

    virtual inline void Shutdown(void)
    {
      PoolList::iterator it = m_Pools.begin();

      while (it != m_Pools.end())
      {
        XMP_FREE((*it)->buffer);
        XMP_FREE((*it));
        ++it;
      }
      m_Pools.clear();

      m_NodeAllocator.Shutdown();
    }

    virtual inline void Reset(void)
    {
      PoolList::iterator it = m_Pools.begin();

      while (it != m_Pools.end())
      {
        XMP_FREE((*it)->buffer);
        XMP_FREE((*it));

        ++it;
      }
      m_Pools.clear();

      m_NodeAllocator.Reset();
    }

    virtual inline DeferredPointer<T>* Allocate(void)
    {
      PoolList::iterator it = m_Pools.begin();
      PoolEntry* entry = NULL;

      if (m_Pools.size() > 0)
      {
        Pool* pool = m_Pools.front();

        if (pool->nbElements < m_MaxElementsPerPool)
        {
          entry = &pool->buffer[pool->nbElements];
          ++pool->nbElements;
        }
      }

      if (entry == NULL)
      {
        Pool* newPool = RequestPool();
        entry         = &newPool->buffer[0];
        ++newPool->nbElements;
      }

      entry->ptr      = m_NodeAllocator.Allocate();
      entry->ptr->ptr = &entry->data;

      return entry->ptr;
    }

    inline void Free(DeferredPointer<T>* _ptr)
    {
      PoolList::iterator it = m_Pools.begin();
      ULONG elementAddr = (ULONG)_ptr->ptr;
      ULONG allocIndex  = 0;
      Pool* allocPool   = NULL;

      // Find the pool in which we allocated the pointer
      while (it != m_Pools.end())
      {
        Pool* pool = (*it);

        if ((elementAddr >= (ULONG)pool->buffer) &&
            (elementAddr <  (ULONG)(pool->buffer + m_MaxElementsPerPool)))
        {
          allocPool  = pool;
          allocIndex = (elementAddr - (ULONG)pool->buffer) / sizeof(PoolEntry);
          break;
        }

        ++it;
      }

      Pool* firstPool = m_Pools.front();
      ULONG lastIndex = firstPool->nbElements - 1;

      // Redirect the allocation
      if ((allocPool != firstPool) || ((allocPool == firstPool) && (allocIndex != lastIndex)))
      {
        allocPool->buffer[allocIndex]           =  firstPool->buffer[lastIndex];
        allocPool->buffer[allocIndex].ptr->ptr  = &allocPool->buffer[allocIndex].data;
      }

      // Pool is now empty and can be freed
      --firstPool->nbElements;
      if (firstPool->nbElements == 0)
      {
        XMP_FREE(firstPool->buffer);
        XMP_FREE(firstPool);

        m_Pools.pop_front();
      }

      // Mark the deferred pointer as invalid
      _ptr->ptr = NULL;
    }

    inline ULONG GetPoolMemoryUsage(void) const
    {
      return (m_Pools.size() * m_MaxElementsPerPool * sizeof(PoolEntry)) + 
             m_NodeAllocator.GetPoolMemoryUsage();
    }

    inline ULONG GetMaxElementsPerPool(void) const
    {
      return m_MaxElementsPerPool;
    }

    inline ULONG GetElementSize(void) const
    {
      return sizeof(PoolEntry);
    }

  private:

    inline Pool* RequestPool(void)
    {
      Pool* pool = (Pool*)XMP_ALLOC(sizeof(Pool));

      pool->buffer     = (PoolEntry*)XMP_ALLOC(m_MaxElementsPerPool * sizeof(PoolEntry));
      pool->nbElements = 0;

      m_Pools.push_front(pool);

      return pool;
    }

  private:

    PoolAllocator< DeferredPointer<T> > m_NodeAllocator;

    PoolList m_Pools;
    ULONG    m_MaxElementsPerPool;
  };

};

#endif // ACTIVE_EDITORS

#endif // #ifdef GUARD_XMP_XENONMESHUTILS_H
