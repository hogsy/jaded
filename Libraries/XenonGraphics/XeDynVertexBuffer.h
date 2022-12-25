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
#ifndef __XEDYNVERTEXBUFFER_H__INCLUDED
#define __XEDYNVERTEXBUFFER_H__INCLUDED

#include "XeVertexBuffer.h"

class XeDynVertexBuffer : public XeBuffer
{
    friend XeBufferMgr;

private:

    M_DeclareOperatorNewAndDelete();

    XeDynVertexBuffer();
    ~XeDynVertexBuffer(void);

    void Init(BOOL _bOwnsBuffer, bool bWriteOnly = true);
    void Free(void);

public:

    long Release(void);
    inline void ResetStartVertex(void);
    LPVOID Lock(ULONG _ulVertexCount, ULONG _ulVertexStride, BOOL _bForceLock = FALSE, BOOL _bContextual = FALSE);
    void Unlock(BOOL _bForceUnLock = FALSE);
    
    inline void SetFullLock(BOOL _bFullLock) { m_bFullLock = _bFullLock; }
    inline BOOL IsCompletelyLocked() { return m_bFullLock; }

    inline void   SetLastLockAddress(LPVOID pAddress);
    inline LPVOID GetLastLockAddress() { return m_pLastLockAddress; }

#if defined(ACTIVE_EDITORS)
    inline void SetVertexCount(ULONG _ulVertexCount);
#endif

private:

    unsigned long m_CurrentOffset;
    unsigned long m_LockCount;
    BOOL          m_bFullLock;
    LPVOID        m_pLastLockAddress;
    BOOL          m_bOwnsBuffer;
};

// ------------------------------------------------------------------------------------------------
// Name   : XeDynVertexBuffer::SetLastLockAddress
// Params : pAddress : Buffer pointer
// RetVal : None
// Descr. : Set the lock address
// ------------------------------------------------------------------------------------------------
inline void XeDynVertexBuffer::SetLastLockAddress(LPVOID pAddress)
{
    m_pLastLockAddress = pAddress;

    if (pAddress != NULL)
    {
        m_LockCount = 1;
    }
    else
    {
        m_LockCount = 0;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeDynVertexBuffer::ResetStartVertex
// Params : None
// RetVal : None
// Descr. : Reset the buffer
// ------------------------------------------------------------------------------------------------
inline void XeDynVertexBuffer::ResetStartVertex(void)
{
#if defined(_XENON_RENDER_PC)
    SetLastLockAddress(NULL);
#endif
    m_CurrentOffset = 0;
}

#if defined(ACTIVE_EDITORS)
inline void XeDynVertexBuffer::SetVertexCount(ULONG _ulVertexCount)
{
    ERR_X_Assert(_ulVertexCount <= m_VertexCount);

    m_VertexCount = _ulVertexCount;
}
#endif

#endif // !defined(__XEBUFFERMGR_H__INCLUDED)#endif // !defined(__XEDYNVERTEXBUFFER_H__INCLUDED)
