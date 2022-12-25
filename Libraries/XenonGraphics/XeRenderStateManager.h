#ifndef XERENDERSTATEMANAGER_HEADER
#define XERENDERSTATEMANAGER_HEADER

#include <map>
#include "XeShader.h"
#include "XeIndexBuffer.h"

class XeRenderObject;

class XeRenderStateManager
{
public:

    enum {
        RSMGR_MAX_STREAM  = 2,
        RSMGR_MAX_TEXTURE = 8,
        RSMGR_MAX_SAMPLER = 16,
    };

    union DirtyFlags {
        struct {
            DWORD bPixelShader  : 1;
            DWORD bVertexShader : 1;
            DWORD bVertexDeclaration : 1;
            DWORD bIndexBuffer  : 1;
            DWORD uStream       : 8;
            DWORD uTexture      : 8;
            DWORD uReserved     : 13;
        };
        DWORD dwRawFlags;

        void Clear() { dwRawFlags = 0; }
        void DirtyAll() { dwRawFlags = 0xFFFFFFFF; }
    };

    // Initialization
    XeRenderStateManager();
    ~XeRenderStateManager();
    void ReInit();
    BOOL OnDeviceLost(void);
    BOOL OnDeviceReset(void);
    void Shutdown(void);
    void SetDefaultStates();

    void UnbindAll(void);

    // Set functions for all D3D states 
    inline void SetRenderState(D3DRENDERSTATETYPE _eState, DWORD _dwValue);
    inline void SetBlendState(BOOL _bEnable, DWORD _ulSrcBlend = D3DBLEND_ONE, DWORD _ulBlendOp = D3DBLENDOP_ADD, DWORD _ulDestBlend = D3DBLEND_ZERO);
    inline void SetSamplerState(DWORD _dwSampler, D3DSAMPLERSTATETYPE _eState, DWORD _dwValue);
    inline void SetPixelShader(XePixelShader *_pShader);
    inline void SetVertexShader(XeVertexShader *_pShader);
    inline void SetVertexDeclaration(LPDIRECT3DVERTEXDECLARATION9 _pDeclaration);
    inline void SetStreamSource(UINT _uiStream, XeBuffer *_pBuffer);
    inline void SetStreamSource(UINT _uiStream, XeVertexStream _oStream);
    inline void UnbindAllStreams();
    inline void UnbindStream(ULONG _ulStreamId);
    inline void SetIndices(XeIndexBuffer *_pBuffer);
    inline void SetTexture(DWORD _dwSampler, DWORD _dwTextureNum);
    inline void SetClipPlane(DWORD _dwIndex, const float* _pPlane);

    // Update functions
    BOOL Update(XeRenderObject *_pObject = NULL, BOOL _bUpdateVSConstants = TRUE, BOOL _bUpdatePSConstants = TRUE);

    XeVertexShader *GetCurrentVertexShader() { return m_pCurrentVertexShader; }
    XePixelShader *GetCurrentPixelShader() { return m_pCurrentPixelShader; }

    // Utility functions
    XeRenderStateManager& operator=(const XeRenderStateManager& roMgr);

private:

    void Clear(void);

    // utility functions
    BOOL UpdateTextures();

    // device
    LPDIRECT3DDEVICE9               m_pD3DDevice;

    // member variables
    DWORD                           m_adwRenderStates[D3DRS_MAX];
    DWORD                           m_adwSamplerStates[RSMGR_MAX_SAMPLER][D3DSAMP_MAX];
    XePixelShader                   *m_pCurrentPixelShader;
    XeVertexShader                  *m_pCurrentVertexShader;
    LPDIRECT3DVERTEXDECLARATION9    m_pCurrentVertexDeclaration;
    XeIndexBuffer                   *m_pCurrentIndexBuffer;
    ULONG                           m_aulCurrentStreamOffset[RSMGR_MAX_STREAM];
    ULONG                           m_aulCurrentStreamStride[RSMGR_MAX_STREAM];
    LPDIRECT3DVERTEXBUFFER9         m_apoCurrentVB[RSMGR_MAX_STREAM];
    DWORD                           m_adwCurrentTexture[RSMGR_MAX_TEXTURE];
#if defined(_XENON)
    D3DBLENDSTATE                   m_oCurrentBlendState;
#endif

    // Dirty flags
    DirtyFlags                           m_oDirtyFlags;
    std::map<D3DRENDERSTATETYPE, DWORD>  m_mapDirtyRenderStates;
    std::map<D3DSAMPLERSTATETYPE, DWORD> m_mapDirtySamplerStates[RSMGR_MAX_SAMPLER];
};

extern XeRenderStateManager g_oRenderStateMgr;

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::SetStreamSource(UINT _uiStream, XeVertexStream _oStream)
{
    SetStreamSource(_uiStream, _oStream.pBuffer);
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::UnbindAllStreams()
{
    for (ULONG ulCurStream = 0; ulCurStream < RSMGR_MAX_STREAM; ulCurStream++)
    {
        // update render state manager state
        SetStreamSource(ulCurStream, NULL);

        // send directly to D3D
        m_pD3DDevice->SetStreamSource(ulCurStream, NULL, 0, 0);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::UnbindStream(ULONG _ulStreamId)
{
    // update render state manager state
    SetStreamSource(_ulStreamId, NULL);

    // send directly to D3D
    m_pD3DDevice->SetStreamSource(_ulStreamId, NULL, 0, 0);
}


// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::SetRenderState(D3DRENDERSTATETYPE _eState, DWORD _dwValue)
{
    if (m_adwRenderStates[_eState] != _dwValue)
    {/*
        std::pair <D3DRENDERSTATETYPE, DWORD> oNewPair;
        oNewPair.first  = _eState;
        oNewPair.second = _dwValue;
        m_mapDirtyRenderStates.insert(oNewPair);*/

        m_pD3DDevice->SetRenderState(_eState, _dwValue);
        m_adwRenderStates[_eState] = _dwValue;
    }
}

inline void XeRenderStateManager::SetBlendState(BOOL _bEnable, DWORD _ulSrcBlend, DWORD _ulBlendOp, DWORD _ulDestBlend)
{
#if defined(_XENON)

    D3DBLENDSTATE oBlendState;

    if (_bEnable)
    {
        oBlendState.SrcBlend  = oBlendState.SrcBlendAlpha  = _ulSrcBlend;
        oBlendState.BlendOp   = oBlendState.BlendOpAlpha   = _ulBlendOp;
        oBlendState.DestBlend = oBlendState.DestBlendAlpha = _ulDestBlend;
    }
    else
    {
        oBlendState.SrcBlend  = oBlendState.SrcBlendAlpha  = D3DBLEND_ONE;
        oBlendState.BlendOp   = oBlendState.BlendOpAlpha   = D3DBLENDOP_ADD;
        oBlendState.DestBlend = oBlendState.DestBlendAlpha = D3DBLEND_ZERO;
    }

    if (*(DWORD*)&oBlendState != *(DWORD*)&m_oCurrentBlendState)
    {
        m_pD3DDevice->SetBlendState(0, oBlendState);
        m_oCurrentBlendState = oBlendState;
    }

#else

    SetRenderState(D3DRS_ALPHABLENDENABLE, _bEnable);
    if (_bEnable)
    {
        SetRenderState(D3DRS_BLENDOP,   _ulBlendOp);
        SetRenderState(D3DRS_SRCBLEND,  _ulSrcBlend);
        SetRenderState(D3DRS_DESTBLEND, _ulDestBlend);
    }

#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::SetSamplerState(DWORD _dwSampler, D3DSAMPLERSTATETYPE _eState, DWORD _dwValue)
{

    if (m_adwSamplerStates[_dwSampler][_eState] != _dwValue)
    {
/*
        std::pair <D3DSAMPLERSTATETYPE, DWORD> oNewPair;
        oNewPair.first  = _eState;
        oNewPair.second = _dwValue;
        m_mapDirtySamplerStates[_dwSampler].insert(oNewPair);*/

        m_pD3DDevice->SetSamplerState(_dwSampler, _eState, _dwValue);
        m_adwSamplerStates[_dwSampler][_eState] = _dwValue;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::SetPixelShader(XePixelShader *_pShader)
{
    if (m_pCurrentPixelShader != _pShader)
    {
        m_pCurrentPixelShader = _pShader;
        m_oDirtyFlags.bPixelShader = TRUE;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::SetVertexShader(XeVertexShader *_pShader)
{
    if (m_pCurrentVertexShader != _pShader)
    {
        m_pCurrentVertexShader = _pShader;
        m_oDirtyFlags.bVertexShader = TRUE;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::SetVertexDeclaration(LPDIRECT3DVERTEXDECLARATION9 _pDeclaration)
{
    if (m_pCurrentVertexDeclaration != _pDeclaration)
    {
        m_pCurrentVertexDeclaration = _pDeclaration;
        m_oDirtyFlags.bVertexDeclaration = TRUE;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::SetStreamSource(UINT _uiStream, XeBuffer *_pBuffer)
{
    if (_pBuffer == NULL)
    {
        m_aulCurrentStreamOffset[_uiStream] = 0;
        m_aulCurrentStreamStride[_uiStream] = 0;
        m_apoCurrentVB[_uiStream]           = NULL;
        m_oDirtyFlags.uStream |= 1 << _uiStream;
    }
    else if ((m_aulCurrentStreamOffset[_uiStream] != _pBuffer->GetOffset()) || 
             (m_aulCurrentStreamStride[_uiStream] != _pBuffer->GetVertexStride()) ||
             (m_apoCurrentVB[_uiStream]           != _pBuffer->GetVB())
            )    
    {
        m_aulCurrentStreamOffset[_uiStream] = _pBuffer->GetOffset();
        m_aulCurrentStreamStride[_uiStream] = _pBuffer->GetVertexStride();
        m_apoCurrentVB[_uiStream]           = _pBuffer->GetVB();
        m_oDirtyFlags.uStream |= 1 << _uiStream;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::SetIndices(XeIndexBuffer *_pBuffer)
{
    if((m_pCurrentIndexBuffer != _pBuffer) ||
       ((_pBuffer != NULL) && (m_pCurrentIndexBuffer->GetIB() != _pBuffer->GetIB()) )
      )
    {
        m_pCurrentIndexBuffer = _pBuffer;
        m_oDirtyFlags.bIndexBuffer = TRUE;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::SetTexture(DWORD _dwSampler, DWORD _dwTextureNum)
{
    if (m_adwCurrentTexture[_dwSampler] != _dwTextureNum)
    {
        m_adwCurrentTexture[_dwSampler] = _dwTextureNum;
        m_oDirtyFlags.uTexture |= 1 << _dwSampler;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderStateManager::SetClipPlane
// Params : ..
// RetVal : ..
// Descr. : Set a user clipping plane
// ------------------------------------------------------------------------------------------------
inline void XeRenderStateManager::SetClipPlane(DWORD _dwIndex, const float* _pPlane)
{
    m_pD3DDevice->SetClipPlane(_dwIndex, _pPlane);
}

#endif //XERENDERSTATEMANAGER_HEADER