//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright 2004 Ubisoft
//
// Author       Eric Le
// Date         20 Dec 2004
//
// $Archive: $
//
// Description: Manages vertex and index buffers.  Mostly copied from
//              Sebastien Comte's PoP code.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __XEBUFFERMGR_H__INCLUDED
#define __XEBUFFERMGR_H__INCLUDED

#include <list>
#include <map>

#define XEBUFFER_SPRITE_MAX         8192
#define XEBUFFER_ROUNDROBINSIZE     3
class XeIndexBuffer;
class XeVertexBuffer;
class XeDynVertexBuffer;

class XeBufferMgr
{
public:
    XeBufferMgr();
    ~XeBufferMgr();

    void Init(void);
    void Shutdown(void);

    // Update the buffer manager (must be called every frame)
    void Update(void);

    BOOL OnDeviceLost(void);
    BOOL OnDeviceReset(void);

    XeIndexBuffer*  CreateIndexBuffer(unsigned long indexCount);
    XeVertexBuffer* CreateVertexBuffer(unsigned long vertexCount,
                                       unsigned long vertexStride,
                                       BOOL          _bShare = FALSE);
    XeDynVertexBuffer* CreateDynVertexBuffer(BOOL bInternalBuffer = FALSE);
    LPVOID LockDynVertexBuffer(ULONG _ulSize, IDirect3DVertexBuffer9 **_ppVB, ULONG *_pOffset, BOOL _bForceLock = FALSE); 
    void UnLockDynVertexBuffers(BOOL _bForceUnlock = FALSE);

    // Release buffers
    void ReleaseVB(XeVertexBuffer* vb);
    void ReleaseIB(XeIndexBuffer* ib);
    void ReleaseDynVB(XeDynVertexBuffer* dynVB);

    inline int GetVBContext() { return m_iVertexBufferContext; }
    inline void SwapVBContext() { m_iVertexBufferContext = ((m_iVertexBufferContext+1) % 2); }

    void NotifyRelease(IDirect3DVertexBuffer9* _pVB);

    // Garbage collection
    void GarbageCollect(bool freeDynamicBuffers = false);

#if defined(_XENON_RENDER_PC)
    inline XeIndexBuffer* GetSpriteIB(void) { return m_SpriteIB; };
#endif

private:

    typedef std::list<XeIndexBuffer*>                   IndexBufferList;
    typedef std::list<XeVertexBuffer*>                  VertexBufferList;
    typedef std::list<XeDynVertexBuffer*>               DynVertexBufferList;

    BOOL m_Initialized;
    BOOL m_bFullLockMode; 

    // Index buffers
    IndexBufferList m_IndexBuffers;
    IndexBufferList m_FreedIBs;

#if defined(_XENON_RENDER_PC)
    XeIndexBuffer*  m_SpriteIB;
#endif

    // Vertex buffers
    VertexBufferList m_VertexBuffers;
    VertexBufferList m_FreedVBs;

    // Dyn Vertex buffers
    DynVertexBufferList m_DynVertexBuffers;
    DynVertexBufferList m_DynFreedVBs;

    // Internal Dyn Vertex buffers
    DynVertexBufferList m_IntDynVertexBuffers[XEBUFFER_ROUNDROBINSIZE];
    DynVertexBufferList m_IntDynFreedVBs[XEBUFFER_ROUNDROBINSIZE];
    ULONG               m_ulCurrentIntDynVB;

    struct SharedStaticVB
    {
        IDirect3DVertexBuffer9* pVB;
        ULONG                   ulUsedSize;
        ULONG                   ulNbRef;
    };
    typedef std::list<SharedStaticVB*> SharedStaticVBList;

    SharedStaticVBList  m_SharedStaticVB;

    int                 m_iVertexBufferContext;
};

extern __declspec(align(32)) XeBufferMgr g_XeBufferMgr;
#endif //__XEBUFFERMGR_H__INCLUDED