// ------------------------------------------------------------------------------------------------
// File   : XeBufferMgr.cpp
// Date   : 
// Author : Sebastien Comte
// Descr. : 
//
// Ubi Soft Entertainment Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"
 
#include "XeBufferMgr.h"
#include "XeIndexBuffer.h"
#include "XeVertexBuffer.h"
#include "XeDynVertexBuffer.h"
#include "XeBufferMgr.h"
#include "XeRenderer.h"

XeBufferMgr g_XeBufferMgr;

// ------------------------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------------------------

#define DYNBUFFER_SIZE  6*1024*1024

// ------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
// ------------------------------------------------------------------------------------------------

#if defined(USE_DEFERRED_FREE_AND_RELEASE)
ThreadSafeFree       g_ThreadSafeFree;
ThreadSafeMeshDelete g_ThreadSafeMeshDelete;
ThreadSafeVBRelease  g_ThreadSafeVBRelease;
ThreadSafeIBRelease  g_ThreadSafeIBRelease;
#endif

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::XeBufferMgr
// Params   None
// RetVal : None
// Descr. : Constructor
// ------------------------------------------------------------------------------------------------
XeBufferMgr::XeBufferMgr(void)
: m_Initialized(false), m_bFullLockMode(FALSE), m_ulCurrentIntDynVB(0), m_iVertexBufferContext(0)
{
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::~XeBufferMgr
// Params   None
// RetVal : None
// Descr. : Destructor
// ------------------------------------------------------------------------------------------------
XeBufferMgr::~XeBufferMgr(void)
{
    // Shutdown the manager to be sure
    Shutdown();
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::Init
// Params   device : Direct3D device
// RetVal : None
// Descr. : Initialize the buffer manager
// ------------------------------------------------------------------------------------------------
void XeBufferMgr::Init()
{
    // Already initialized?
    if (m_Initialized)
        return;

    m_Initialized = true;

#if defined(_XENON_RENDER_PC)
    ULONG i;

    m_SpriteIB = CreateIndexBuffer(XEBUFFER_SPRITE_MAX * 6);
    XeValidateErr(m_SpriteIB != NULL, , "Unable to create the sprite index buffer");

    USHORT* pIndices = (USHORT*)m_SpriteIB->Lock(XEBUFFER_SPRITE_MAX * 6);
    for (i = 0; i < XEBUFFER_SPRITE_MAX; ++i)
    {
        *pIndices++ = (USHORT)((i * 4) + 0);
        *pIndices++ = (USHORT)((i * 4) + 1);
        *pIndices++ = (USHORT)((i * 4) + 2);

        *pIndices++ = (USHORT)((i * 4) + 0);
        *pIndices++ = (USHORT)((i * 4) + 2);
        *pIndices++ = (USHORT)((i * 4) + 3);
    }
    m_SpriteIB->Unlock();
#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::Shutdown
// Params   None
// RetVal : None
// Descr. : Shutdown the buffer manager
// ------------------------------------------------------------------------------------------------
void XeBufferMgr::Shutdown(void)
{
    // Not initialized?
    if (!m_Initialized)
        return;

    IndexBufferList::iterator    ib;
    VertexBufferList::iterator   vb;
    DynVertexBufferList::iterator dynVB;

#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.AcquireThreadOwnership();
#endif

#if defined(_XENON_RENDER_PC)
    SAFE_RELEASE(m_SpriteIB);
#endif

    // Do garbage collection
    GarbageCollect(true);

    // Destroy all index buffers
    ib = m_IndexBuffers.begin();
    while (ib != m_IndexBuffers.end())
    {
        SAFE_DELETE((*ib));

        ++ib;
    }
    m_IndexBuffers.clear();

    // Destroy all vertex buffers
    vb = m_VertexBuffers.begin();
    while (vb != m_VertexBuffers.end())
    {
        SAFE_DELETE((*vb));

        ++vb;
    }
    m_VertexBuffers.clear();

    // Destroy all dynamic vertex buffers
    dynVB = m_DynVertexBuffers.begin();
    while (dynVB != m_DynVertexBuffers.end())
    {
        SAFE_DELETE((*dynVB));

        ++dynVB;
    }
    m_DynVertexBuffers.clear();

    // Destroy all internal dynamic vertex buffers
    for(UINT uiDVBIdx = 0; uiDVBIdx < XEBUFFER_ROUNDROBINSIZE; uiDVBIdx++)
    {
        dynVB = m_IntDynVertexBuffers[uiDVBIdx].begin();
        while (dynVB != m_IntDynVertexBuffers[uiDVBIdx].end())
        {
            SAFE_DELETE((*dynVB));

            ++dynVB;
        }
        m_IntDynVertexBuffers[uiDVBIdx].clear();
    }

    // Destroy all the shared static vertex buffers
    SharedStaticVBList::iterator itSharedStaticVB;
    itSharedStaticVB = m_SharedStaticVB.begin();
    while (itSharedStaticVB != m_SharedStaticVB.end())
    {
        SAFE_RELEASE((*itSharedStaticVB)->pVB);
        SAFE_DELETE((*itSharedStaticVB));
        ++itSharedStaticVB;
    }
    m_SharedStaticVB.clear();

    m_Initialized = false;
#if defined(_XENON_RENDERER_USETHREAD)
	g_oXeRenderer.ReleaseThreadOwnership();
#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::OnDeviceLost
// Params   None
// RetVal : Success
// Descr. : Release all resources for device reset
// ------------------------------------------------------------------------------------------------
BOOL XeBufferMgr::OnDeviceLost(void)
{
    DynVertexBufferList::iterator dynVB;

    if (!m_Initialized)
        return TRUE;

    // Do garbage collection
    GarbageCollect(true);

    // Free all dynamic vertex buffers
    dynVB = m_DynVertexBuffers.begin();
    while (dynVB != m_DynVertexBuffers.end())
    {
        (*dynVB)->Free();

        ++dynVB;
    }

    // Discard all internal dynamic vertex buffers
    for(UINT uiDVBIdx = 0; uiDVBIdx < XEBUFFER_ROUNDROBINSIZE; uiDVBIdx++)
    {
        dynVB = m_IntDynVertexBuffers[uiDVBIdx].begin();
        while (dynVB != m_IntDynVertexBuffers[uiDVBIdx].end())
        {
            SAFE_DELETE((*dynVB));

            ++dynVB;
        }
        m_IntDynVertexBuffers[uiDVBIdx].resize(0);
    }

#if defined(_PC_FORCE_DYNAMIC_VB)
    VertexBufferList::iterator vb;
    vb = m_VertexBuffers.begin();
    while (vb != m_VertexBuffers.end())
    {
        (*vb)->Free();

        ++vb;
    }
#endif

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::OnDeviceReset
// Params   None
// RetVal : Success
// Descr. : Restore all resources after device reset
// ------------------------------------------------------------------------------------------------
BOOL XeBufferMgr::OnDeviceReset(void)
{
    DynVertexBufferList::iterator dynVB;

    if (!m_Initialized)
        return TRUE;

#if defined(_PC_FORCE_DYNAMIC_VB)
    VertexBufferList::iterator vb;
    vb = m_VertexBuffers.begin();
    while (vb != m_VertexBuffers.end())
    {
        (*vb)->Init(TRUE);

        ++vb;
    }
#endif

    // Restore all dynamic vertex buffers
    dynVB = m_DynVertexBuffers.begin();
    while (dynVB != m_DynVertexBuffers.end())
    {
        (*dynVB)->Init(FALSE);

        ++dynVB;
    }

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::CreateIndexBuffer
// Params : indexCount : Number of indices
// RetVal : Index buffer
// Descr. : Creates a static index buffer
// ------------------------------------------------------------------------------------------------
XeIndexBuffer* XeBufferMgr::CreateIndexBuffer(ULONG indexCount)
{
    XeIndexBuffer* indexBuffer;

    ERR_X_Assert(indexCount > 0);

    // TODO: Validate index count

    // Create the index buffer
    indexBuffer = new XeIndexBuffer(indexCount);
    ERR_X_Assert(indexBuffer != NULL);

    // Initialize the index buffer
    indexBuffer->Init(false);

    // Add the index buffer to the list
    m_IndexBuffers.push_back(indexBuffer);

    return indexBuffer;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::CreateVertexBuffer
// Params : vertexCount  : Number of vertices
//          vertexStride : Vertex stride
//          vertexFormat : FVF
// RetVal : Vertex buffer
// Descr. : Creates a static vertex buffer
// ------------------------------------------------------------------------------------------------
XeVertexBuffer* XeBufferMgr::CreateVertexBuffer(ULONG vertexCount, ULONG vertexStride, BOOL _bShare)
{
    // SC: TODO: Allocate shared vertex buffers of 2 MB
    const ULONG C_ul_MaxStaticSharedVBSize = 2 * 1024 * 1024;

    XeVertexBuffer* vertexBuffer;

    ERR_X_Assert(vertexCount > 0);
    ERR_X_Assert(vertexStride != 0);

    // SC: TODO: Enable when we have more time to test
#if 0
    if (_bShare)
    {
        SharedStaticVB* pSharedVB       = NULL;
        ULONG           ulRequestedSize = vertexCount * vertexStride;

        ERR_X_Assert(ulRequestedSize <= C_ul_MaxStaticSharedVBSize);

        SharedStaticVBList::iterator it;

        it = m_SharedStaticVB.begin();
        while (it != m_SharedStaticVB.end())
        {
            SharedStaticVB* pTempVB = *it;

            if (pTempVB->ulUsedSize + ulRequestedSize <= C_ul_MaxStaticSharedVBSize)
            {
                pSharedVB = pTempVB;
                break;
            }

            ++it;
        }
        if (pSharedVB == NULL)
        {
            HRESULT hr;

            DWORD   dwUsage = D3DUSAGE_WRITEONLY;
            D3DPOOL ePool   = (D3DPOOL)0;

#if defined(_XENON_RENDER_PC)
            ePool = D3DPOOL_MANAGED;
#endif

            pSharedVB = new SharedStaticVB();
            pSharedVB->ulNbRef    = 0;
            pSharedVB->ulUsedSize = 0;

            hr = g_oXeRenderer.GetDevice()->CreateVertexBuffer(C_ul_MaxStaticSharedVBSize, dwUsage, 0, 
                                                            ePool, &pSharedVB->pVB, NULL);
            XeValidate(SUCCEEDED(hr) && (pSharedVB->pVB != NULL), "Failed to create a shared static vertex buffer");

            m_SharedStaticVB.push_back(pSharedVB);
        }

        vertexBuffer = new XeVertexBuffer(vertexCount, vertexStride, pSharedVB->ulUsedSize);
        ERR_X_Assert(vertexBuffer != NULL);

        vertexBuffer->Init(pSharedVB->pVB);
        ++pSharedVB->ulNbRef;
        pSharedVB->ulUsedSize += ulRequestedSize;

        m_VertexBuffers.push_back(vertexBuffer);
    }
    else
#endif
    {
        // Create the vertex buffer
        vertexBuffer = new XeVertexBuffer(vertexCount, vertexStride, 0);
        ERR_X_Assert(vertexBuffer != NULL);

        // Initialize the vertex buffer
        vertexBuffer->Init(false);

        // Add the vertex buffer to the list
        m_VertexBuffers.push_back(vertexBuffer);
    }

    return vertexBuffer;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::CreateDynVertexBuffer
// Params : vertexCount  : Number of vertices
//          vertexStride : Vertex stride
//          vertexFormat : FVF
// RetVal : Vertex buffer
// Descr. : Creates a dynamic vertex buffer
// ------------------------------------------------------------------------------------------------
XeDynVertexBuffer* XeBufferMgr::CreateDynVertexBuffer(BOOL bInternalBuffer)
{
    XeDynVertexBuffer* vertexBuffer;

    // Create the dynamic vertex buffer
    vertexBuffer = new XeDynVertexBuffer();
    ERR_X_Assert(vertexBuffer != NULL);

    // Initialize the vertex buffer
    vertexBuffer->Init(bInternalBuffer);

    if (bInternalBuffer)
    {
        // Add the vertex buffer to the list
        m_IntDynVertexBuffers[m_ulCurrentIntDynVB].push_back(vertexBuffer);
    }
    else
    {
        // Add the vertex buffer to the list
        m_DynVertexBuffers.push_back(vertexBuffer);
    }

    return vertexBuffer;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::Update
// Params : None
// RetVal : None
// Descr. : Update the buffer manager
// ------------------------------------------------------------------------------------------------
void XeBufferMgr::Update(void)
{
    DynVertexBufferList::iterator dynVB;

    // Reset the start vertex for all dynamic vertex buffers
    dynVB = m_IntDynVertexBuffers[m_ulCurrentIntDynVB].begin();
    while (dynVB != m_IntDynVertexBuffers[m_ulCurrentIntDynVB].end())
    {
        // We don't care about the VB's content
        (*dynVB)->ResetStartVertex();

        ++dynVB;
    }

    // Do garbage collection (only for static buffers)
    GarbageCollect();

    m_ulCurrentIntDynVB = (m_ulCurrentIntDynVB + 1) % XEBUFFER_ROUNDROBINSIZE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::ReleaseVB
// Params : vb : Vertex buffer to free
// RetVal : None
// Descr. : Release a vertex buffer
// ------------------------------------------------------------------------------------------------
void XeBufferMgr::ReleaseVB(XeVertexBuffer* vb)
{
    ERR_X_Assert(vb != NULL);

    // Remove the vertex buffer from the allocated list
    m_VertexBuffers.remove(vb);

    // Add the vertex buffer to the freed list
    m_FreedVBs.push_back(vb);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::ReleaseIB
// Params : ib : Index buffer to free
// RetVal : None
// Descr. : Release a index buffer
// ------------------------------------------------------------------------------------------------
void XeBufferMgr::ReleaseIB(XeIndexBuffer* ib)
{
    ERR_X_Assert(ib != NULL);

    // Remove the index buffer from the allocated list
    m_IndexBuffers.remove(ib);

    // Add the index buffer to the freed list
    m_FreedIBs.push_back(ib);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::ReleaseDynVB
// Params : dynVB : Dynamic vertex buffer to free
// RetVal : None
// Descr. : Release a dynamic vertex buffer
// ------------------------------------------------------------------------------------------------
void XeBufferMgr::ReleaseDynVB(XeDynVertexBuffer* dynVB)
{
    ERR_X_Assert(dynVB != NULL);

    // Remove the vertex buffer from the allocated list
    m_DynVertexBuffers.remove(dynVB);

    // Add the vertex buffer to the freed list
    m_DynFreedVBs.push_back(dynVB);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::GarbageCollect
// Params : freeDynamicBuffers : Also free the dynamic buffers
// RetVal : None
// Descr. : Actually free the buffers that needs to be freed
// ------------------------------------------------------------------------------------------------
void XeBufferMgr::GarbageCollect(bool freeDynamicBuffers)
{
#if defined(USE_DEFERRED_FREE_AND_RELEASE)
	//----------------------------------------------------------
	// commit mesh deletion and vb releases

    EnterCriticalSection(&g_ThreadSafeLock);

	XeBuffer *pBuffer;
	XeIndexBuffer *pIndexBuffer;
	XeMesh	 *pMesh;
	int i;

    for(i=0; i<(int)g_ThreadSafeFree.size(); i++)
    {
        if(g_ThreadSafeFree[i])
            MEM_Free(g_ThreadSafeFree[i]);
    }
    g_ThreadSafeFree.clear();

	for(i=0; i<(int)g_ThreadSafeMeshDelete.size(); i++)
	{
		pMesh = g_ThreadSafeMeshDelete[i];

        // Do not delete the dynamic meshes
        if ((pMesh != NULL) && (pMesh->IsDynamic()))
            continue;

		SAFE_DELETE(pMesh);
	}
	g_ThreadSafeMeshDelete.clear();

	for(i=0; i<(int)g_ThreadSafeVBRelease.size(); i++)
	{
		pBuffer = g_ThreadSafeVBRelease[i];
		SAFE_RELEASE(pBuffer);
	}
	g_ThreadSafeVBRelease.clear();

	for(i=0; i<(int)g_ThreadSafeIBRelease.size(); i++)
	{
		pIndexBuffer = g_ThreadSafeIBRelease[i];
		SAFE_RELEASE(pIndexBuffer);
	}
	g_ThreadSafeIBRelease.clear();

    LeaveCriticalSection(&g_ThreadSafeLock);

	//------------------------------------------------------------
#endif

    DynVertexBufferList::iterator DynVb;
    VertexBufferList::iterator vb;
    IndexBufferList::iterator  ib;
    DynVertexBufferList::iterator dynVB;

    // Release the vertex buffers
    vb = m_FreedVBs.begin();
    while (vb != m_FreedVBs.end())
    {
        SAFE_DELETE((*vb));
        ++vb;
    }
    m_FreedVBs.resize(0);

    // Release the vertex buffers
    DynVb = m_DynFreedVBs.begin();
    while (DynVb != m_DynFreedVBs.end())
    {
        SAFE_DELETE((*DynVb));
        ++DynVb;
    }
    m_DynFreedVBs.resize(0);

    // Release the index buffers
    ib = m_FreedIBs.begin();
    while (ib != m_FreedIBs.end())
    {
        SAFE_DELETE(*ib);
        ++ib;
    }
    m_FreedIBs.resize(0);
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::LockDynVertexBuffer
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
LPVOID XeBufferMgr::LockDynVertexBuffer(ULONG _ulSize, IDirect3DVertexBuffer9 **_ppVB, ULONG *_pOffset, BOOL _bForceLock)
{
    XeDynVertexBuffer *pDynVB = NULL;
    DynVertexBufferList::iterator it;
    BOOL bFound = FALSE, bFullLock = FALSE;
    LPVOID pLock;

    if (_bForceLock)
    {
        if (_ulSize == 0)
        {
            // full lock mode, the whole buffer is locked for everyone to use it
            m_bFullLockMode = TRUE;
            _ulSize = DYNBUFFER_SIZE;
        }
        else
        {
            // partial lock mode, only a section of the buffer is locked
            m_bFullLockMode = FALSE;
        }
    }

    // try to find a buffer big enough
    it = m_IntDynVertexBuffers[m_ulCurrentIntDynVB].begin();
    while (it != m_IntDynVertexBuffers[m_ulCurrentIntDynVB].end())
    {
        pDynVB = (*it);

        if ((pDynVB->m_CurrentOffset + _ulSize) <= DYNBUFFER_SIZE)
        {
            if (pDynVB->GetLastLockAddress() == NULL)
            {
                // Vertex buffer was not locked, force it
                _bForceLock = TRUE;

                if (_ulSize == DYNBUFFER_SIZE)
                {
                    // Full lock mode, the whole buffer is locked for everyone to use it
                    m_bFullLockMode = TRUE;
                }
                else
                {
                    // Partial lock mode, only a section of the buffer is locked
                    m_bFullLockMode = FALSE;
                }
            }

            (*_pOffset) = pDynVB->m_CurrentOffset;
            bFound = TRUE;
            break;
        }

        ++it;
    }

    if (!bFound)
    {
        // must create a new buffer
        pDynVB = CreateDynVertexBuffer(TRUE);

        HRESULT hr = g_oXeRenderer.GetDevice()->CreateVertexBuffer(DYNBUFFER_SIZE,
                                                                   D3DUSAGE_WRITEONLY ,//| D3DUSAGE_DYNAMIC
                                                                   0,
                                                                   D3DPOOL_DEFAULT,
                                                                   &pDynVB->m_ContextualVB[0],
                                                                   NULL);
        if (FAILED(hr) || (pDynVB->m_ContextualVB[0] == NULL))
        {
            SAFE_RELEASE(pDynVB);
            return NULL;
        }

        pDynVB->AddRef();

        (*_pOffset) = 0;
        _bForceLock = TRUE;
    }

    // update references
    (*_ppVB) = pDynVB->GetVB();

    // Mark the buffer as locked
    pDynVB->m_LockCount = 1;

    if (_bForceLock)
    {
        if (m_bFullLockMode)
        {
            // lock the whole buffer
            pDynVB->SetFullLock(m_bFullLockMode);
            _ulSize = DYNBUFFER_SIZE;
            (*_pOffset) = 0;
        }
        else
        {
            // update current offset
            pDynVB->m_CurrentOffset += _ulSize;
        }

        // actually lock the VB
        // THIS WILL CRASH IN MULTITHREAD MODE.
        // Band-aid solution: increase DYNBUFFER_SIZE to avoid the problem.
        (*_ppVB)->Lock((*_pOffset), _ulSize, &pLock, D3DLOCK_NOSYSLOCK);
        pDynVB->SetLastLockAddress(pLock);
    }
    else
    {
        // simply calculate the address of the already locked buffer
        pLock = (LPBYTE) pDynVB->GetLastLockAddress() + (*_pOffset);

        // update current offset
        pDynVB->m_CurrentOffset += _ulSize;
    }

    return pLock;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::UnLockDynVertexBuffers
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBufferMgr::UnLockDynVertexBuffers(BOOL _bForceUnlock)
{
    // Unlock all dynamic vertex buffers that have been completely locked
    DynVertexBufferList::iterator it = m_IntDynVertexBuffers[m_ulCurrentIntDynVB].begin();
    while (it != m_IntDynVertexBuffers[m_ulCurrentIntDynVB].end())
    {
        if ((*it)->IsCompletelyLocked() || _bForceUnlock)
        {
            (*it)->Unlock(TRUE);
            (*it)->SetFullLock(FALSE);
            (*it)->ResetStartVertex();
        }
        ++it;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeBufferMgr::NotifyRelease
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeBufferMgr::NotifyRelease(IDirect3DVertexBuffer9* _pVB)
{
    SharedStaticVBList::iterator it;
    SharedStaticVB* pSharedVB;

    it = m_SharedStaticVB.begin();
    while (it != m_SharedStaticVB.end())
    {
        pSharedVB = (*it);

        if (_pVB == pSharedVB->pVB)
        {
            --pSharedVB->ulNbRef;
            if (pSharedVB->ulNbRef == 0)
            {
                SAFE_RELEASE(pSharedVB->pVB);
                SAFE_DELETE(pSharedVB);

                m_SharedStaticVB.erase(it);
            }

            break;
        }

        ++it;
    }
}
