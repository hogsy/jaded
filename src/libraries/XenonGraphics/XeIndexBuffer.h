#ifndef XEINDEXBUFFER_HEADER
#define XEINDEXBUFFER_HEADER

class XeBufferMgr;

class XeIndexBuffer
{
    friend XeBufferMgr;

private:

    M_DeclareOperatorNewAndDelete();

    XeIndexBuffer(unsigned long indexCount);
    ~XeIndexBuffer(void);

    void Init(bool copyContent, bool bWriteOnly = true);
    void Free(void);

    void Update(void);

public:

    inline long AddRef(void)  { return ++m_RefCount; }
    inline long GetRef(void)  { return m_RefCount; }
    long Release(void);

    inline unsigned long GetFaceCount(void) { return m_FaceCount; }
    inline unsigned long GetIndexCount(void) { return m_IndexCount; }

    inline void SetFaceCount(ULONG _ulFaceCount)
    {
        ERR_X_Assert(_ulFaceCount > 0);

        m_FaceCount = _ulFaceCount;
    }

    inline LPDIRECT3DINDEXBUFFER9 GetIB(void) { return m_IB; }
    inline LPVOID Lock(ULONG _ulIndexCount);
    inline void Unlock();

private:

    LPDIRECT3DINDEXBUFFER9 m_IB;

    unsigned long   m_IndexCount;
    unsigned long   m_FaceCount;

    long m_RefCount;
};

// ------------------------------------------------------------------------------------------------
// Name   : XeIndexBuffer::Lock
// Params : None
// RetVal : System memory buffer
// Descr. : Get the system memory buffer
// ------------------------------------------------------------------------------------------------
LPVOID XeIndexBuffer::Lock(ULONG _ulIndexCount)
{
    LPVOID pIndices = NULL;
    HRESULT hr = m_IB->Lock(0, _ulIndexCount * sizeof(WORD), (void**)&pIndices, D3DLOCK_NOSYSLOCK);
    ERR_X_Assert(SUCCEEDED(hr));

    return pIndices;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeIndexBuffer::Unlock
// Params : None
// RetVal : System memory buffer
// Descr. : Get the system memory buffer
// ------------------------------------------------------------------------------------------------
inline void XeIndexBuffer::Unlock()
{
    m_IB->Unlock();
}

#endif // XEINDEXBUFFER_HEADER