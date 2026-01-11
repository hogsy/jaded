//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright 2004 Ubisoft
//
// Author       Eric Le
// Date         20 Dec 2004
//
// $Archive: $
//
// Description
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __XEVERTEXBUFFER_H__INCLUDED
#define __XEVERTEXBUFFER_H__INCLUDED

#include "XeBufferMgr.h"
#include "SDK/Sources/BASe/ERRors/ERRasser.h"

class XeBuffer
{
public:

    M_DeclareOperatorNewAndDelete();

    XeBuffer(unsigned long vertexCount, unsigned long vertexStride, unsigned long offset);
    virtual ~XeBuffer(void);

    virtual void Init(BOOL _bOwnsBuffer, bool bWriteOnly = true) = 0;
    virtual void Free(void) = 0;

    inline long AddRef(void)  { return ++m_RefCount; };
    inline long GetRef(void)  { return m_RefCount; };
    virtual long Release(void) = 0;

    inline void          SetVertexCount(unsigned long ulNewVertexCount)  { m_VertexCount = ulNewVertexCount; };
    inline unsigned long GetVertexCount(void)  { return m_VertexCount; };
    inline unsigned long GetVertexStride(void) { return m_VertexStride; };
    //inline void          SetOffset(unsigned long ulNewOffset) { m_Offset = ulNewOffset; };
    inline unsigned long GetOffset(void) 
    {
        if(m_bVBIsContextual)
            return m_ContextualOffset[(g_XeBufferMgr.GetVBContext())];
        else
            return m_ContextualOffset[0];
    }
    
    inline IDirect3DVertexBuffer9* GetVB(void)
    { 
        if(m_bVBIsContextual)
            return m_ContextualVB[(g_XeBufferMgr.GetVBContext())];
        else
            return m_ContextualVB[0];
    }

    virtual LPVOID Lock(ULONG _ulVertexCount, ULONG _ulVertexStride, BOOL _bForceLock = FALSE, BOOL _bContextual = FALSE) = 0;
    virtual void Unlock(BOOL _bForceUnLock = FALSE) = 0;

protected:
    BOOL                    m_bVBIsContextual;
    IDirect3DVertexBuffer9* m_ContextualVB[2];
    unsigned long           m_ContextualOffset[2];

    unsigned long  m_VertexCount;
    unsigned long  m_VertexStride;

    long m_RefCount;
};

class XeVertexBuffer : public XeBuffer
{
    friend XeBufferMgr;

public:

    M_DeclareOperatorNewAndDelete();

    struct SerializationHeader
    {
        DWORD dwNumVertices;
        DWORD dwVertexStride;
        // vertex data follows
    };

    long Release(void);
    inline unsigned char* GetBuffer(unsigned long vertexCount);

private:
    void Init(BOOL _bOwnsBuffer, bool bWriteOnly = true);
    void Init(IDirect3DVertexBuffer9* _pVB);
    void Free(void);
    LPVOID Lock(ULONG _ulVertexCount, ULONG _ulVertexStride, BOOL _bForceLock = FALSE, BOOL _bContextual = FALSE);
    void Unlock(BOOL _bForceUnLock = FALSE);

    XeVertexBuffer(unsigned long vertexCount, unsigned long vertexStride, unsigned long offset);
    ~XeVertexBuffer(void);
};

#endif // !defined(__XEVERTEXBUFFER_H__INCLUDED)
