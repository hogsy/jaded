#include "precomp.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "XeRenderer.h"
#include "XeRenderStateManager.h"
#include "XeTextureManager.h"
#include "XeShader.h"

#include "BASe/BENch/BENch.h"

// Single instance of the XeRenderStateMgr
XeRenderStateManager g_oRenderStateMgr;

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeRenderStateManager::XeRenderStateManager()
{

}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeRenderStateManager::~XeRenderStateManager()
{

}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeRenderStateManager::ReInit()
{
    m_pD3DDevice = g_oXeRenderer.GetDevice();

    m_pCurrentPixelShader           = NULL;
    g_oPixelShaderMgr.InvalidateFeatureKey();

    m_pCurrentVertexShader          = NULL;
    g_oVertexShaderMgr.InvalidateFeatureKey();

    m_pCurrentVertexDeclaration     = NULL;
    m_pCurrentIndexBuffer           = NULL;
    ZeroMemory(m_aulCurrentStreamOffset,   RSMGR_MAX_STREAM  * sizeof(ULONG));
    ZeroMemory(m_aulCurrentStreamStride,   RSMGR_MAX_STREAM  * sizeof(ULONG));
    ZeroMemory(m_apoCurrentVB,             RSMGR_MAX_STREAM  * sizeof(LPDIRECT3DVERTEXBUFFER9));
    L_memset(m_adwCurrentTexture, 0xff, RSMGR_MAX_TEXTURE * sizeof(DWORD));
    
    for (ULONG ulCurSampler = 0; ulCurSampler < RSMGR_MAX_SAMPLER; ulCurSampler++)
    {
        ZeroMemory(&m_adwSamplerStates[ulCurSampler][0], D3DSAMP_MAX * sizeof(DWORD));
    }

    SetDefaultStates();
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderStateManager::OnDeviceLost
// Params : None
// RetVal : Success
// Descr. : Before IDirect3DDevice::Reset()
// ------------------------------------------------------------------------------------------------
BOOL XeRenderStateManager::OnDeviceLost(void)
{
    Clear();

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderStateManager::OnDeviceReset
// Params : None
// RetVal : Success
// Descr. : After IDirect3DDevice::Reset()
// ------------------------------------------------------------------------------------------------
BOOL XeRenderStateManager::OnDeviceReset(void)
{
    SetDefaultStates();

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderStateManager::Shutdown
// Params : None
// RetVal : None
// Descr. : Shutdown the render state manager
// ------------------------------------------------------------------------------------------------
void XeRenderStateManager::Shutdown(void)
{
    // Make sure all the internal Direct3D references are cleared
    Clear();

    m_pD3DDevice = NULL;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeRenderStateManager::Clear
// Params : None
// RetVal : None
// Descr. : Clear the states
// ------------------------------------------------------------------------------------------------
void XeRenderStateManager::Clear(void)
{
    ULONG i;

    m_pCurrentPixelShader           = NULL;
    m_pCurrentVertexShader          = NULL;
    m_pCurrentVertexDeclaration     = NULL;
    m_pCurrentIndexBuffer           = NULL;
    ZeroMemory(m_aulCurrentStreamOffset,   RSMGR_MAX_STREAM  * sizeof(ULONG));
    ZeroMemory(m_aulCurrentStreamStride,   RSMGR_MAX_STREAM  * sizeof(ULONG));
    ZeroMemory(m_apoCurrentVB,             RSMGR_MAX_STREAM  * sizeof(LPDIRECT3DVERTEXBUFFER9));
    L_memset(m_adwCurrentTexture, 0xff, RSMGR_MAX_TEXTURE * sizeof(DWORD));

    if(m_pD3DDevice)
    {
#if defined(_XENON_RENDERER_USETHREAD)
		g_oXeRenderer.AcquireThreadOwnership();
#endif
        m_pD3DDevice->SetPixelShader(NULL);
        g_oPixelShaderMgr.InvalidateFeatureKey();

        m_pD3DDevice->SetVertexShader(NULL);
        g_oVertexShaderMgr.InvalidateFeatureKey();

        m_pD3DDevice->SetVertexDeclaration(NULL);
        m_pD3DDevice->SetIndices(NULL);

        // Clear the streams
        for (i = 0; i < RSMGR_MAX_STREAM; ++i)
        {
            m_pD3DDevice->SetStreamSource(i, NULL, 0, 0);
        }

        // Clear the textures
        for (i = 0; i < RSMGR_MAX_TEXTURE; ++i)
        {
            m_pD3DDevice->SetTexture(i, NULL);
        }
#if defined(_XENON_RENDERER_USETHREAD)
		g_oXeRenderer.ReleaseThreadOwnership();
#endif
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeRenderStateManager::SetDefaultStates()
{
#define SET_RENDER_STATE(_state, _val)         m_adwRenderStates[_state] = _val;         m_pD3DDevice->SetRenderState(_state, _val)
#define SET_SAMPLER_STATE(_samp, _state, _val) m_adwSamplerStates[_samp][_state] = _val; m_pD3DDevice->SetSamplerState(_samp, _state, _val)

    const float fOne = 1.0f, fZero = 0.0f;

    SET_RENDER_STATE(D3DRS_ZENABLE,                    D3DZB_TRUE);
    SET_RENDER_STATE(D3DRS_FILLMODE,                   D3DFILL_SOLID);
    SET_RENDER_STATE(D3DRS_ZWRITEENABLE,               TRUE);
    SET_RENDER_STATE(D3DRS_ALPHATESTENABLE,            FALSE);
    SET_RENDER_STATE(D3DRS_SRCBLEND,                   D3DBLEND_ONE);
    SET_RENDER_STATE(D3DRS_DESTBLEND,                  D3DBLEND_ZERO);
    SET_RENDER_STATE(D3DRS_CULLMODE,                   D3DCULL_CCW);
    SET_RENDER_STATE(D3DRS_ZFUNC,                      D3DCMP_LESSEQUAL);
    SET_RENDER_STATE(D3DRS_ALPHAREF,                   0);
    SET_RENDER_STATE(D3DRS_ALPHAFUNC,                  D3DCMP_ALWAYS);
    SET_RENDER_STATE(D3DRS_DITHERENABLE,               FALSE);
    SET_RENDER_STATE(D3DRS_ALPHABLENDENABLE,           FALSE);
    SET_RENDER_STATE(D3DRS_STENCILENABLE,              FALSE);
    SET_RENDER_STATE(D3DRS_STENCILFAIL,                D3DSTENCILOP_KEEP);
    SET_RENDER_STATE(D3DRS_STENCILZFAIL,               D3DSTENCILOP_KEEP);
    SET_RENDER_STATE(D3DRS_STENCILPASS,                D3DSTENCILOP_KEEP);
    SET_RENDER_STATE(D3DRS_STENCILFUNC,                D3DCMP_ALWAYS);
    SET_RENDER_STATE(D3DRS_STENCILREF,                 0);
    SET_RENDER_STATE(D3DRS_STENCILMASK,                0xFFFFFFFF);
    SET_RENDER_STATE(D3DRS_STENCILWRITEMASK,           0xFFFFFFFF);
    SET_RENDER_STATE(D3DRS_WRAP0,                      0);
    SET_RENDER_STATE(D3DRS_WRAP1,                      0);
    SET_RENDER_STATE(D3DRS_WRAP2,                      0);
    SET_RENDER_STATE(D3DRS_WRAP3,                      0);
    SET_RENDER_STATE(D3DRS_WRAP4,                      0);
    SET_RENDER_STATE(D3DRS_WRAP5,                      0);
    SET_RENDER_STATE(D3DRS_WRAP6,                      0);
    SET_RENDER_STATE(D3DRS_WRAP7,                      0);
    SET_RENDER_STATE(D3DRS_CLIPPLANEENABLE,            *((DWORD*)&fZero));
    SET_RENDER_STATE(D3DRS_POINTSIZE,                  *((DWORD*)&fZero));
    SET_RENDER_STATE(D3DRS_POINTSIZE_MIN,              *((DWORD*)&fZero));
    SET_RENDER_STATE(D3DRS_POINTSPRITEENABLE,          FALSE);
    SET_RENDER_STATE(D3DRS_MULTISAMPLEANTIALIAS,       TRUE);
    SET_RENDER_STATE(D3DRS_MULTISAMPLEMASK,            0xFFFFFFFF);
    SET_RENDER_STATE(D3DRS_PATCHEDGESTYLE,             D3DPATCHEDGE_DISCRETE);
    SET_RENDER_STATE(D3DRS_POINTSIZE_MAX,              *((DWORD*)&fOne));
    SET_RENDER_STATE(D3DRS_COLORWRITEENABLE,           D3DCOLORWRITEENABLE_ALL);
    SET_RENDER_STATE(D3DRS_BLENDOP,                    D3DBLENDOP_ADD);
    SET_RENDER_STATE(D3DRS_POSITIONDEGREE,             D3DDEGREE_LINEAR);
    SET_RENDER_STATE(D3DRS_NORMALDEGREE,               D3DDEGREE_LINEAR);
    SET_RENDER_STATE(D3DRS_SCISSORTESTENABLE,          FALSE);
    SET_RENDER_STATE(D3DRS_SLOPESCALEDEPTHBIAS,        0);
    SET_RENDER_STATE(D3DRS_ANTIALIASEDLINEENABLE,      FALSE);
    SET_RENDER_STATE(D3DRS_MINTESSELLATIONLEVEL,       *((DWORD*)&fOne));
    SET_RENDER_STATE(D3DRS_MAXTESSELLATIONLEVEL,       *((DWORD*)&fOne));
    SET_RENDER_STATE(D3DRS_ADAPTIVETESS_X,             *((DWORD*)&fZero));
    SET_RENDER_STATE(D3DRS_ADAPTIVETESS_Y,             *((DWORD*)&fZero));
    SET_RENDER_STATE(D3DRS_ADAPTIVETESS_Z,             *((DWORD*)&fOne));
    SET_RENDER_STATE(D3DRS_ADAPTIVETESS_W,             *((DWORD*)&fZero));
    SET_RENDER_STATE(D3DRS_ENABLEADAPTIVETESSELLATION, FALSE);
    SET_RENDER_STATE(D3DRS_TWOSIDEDSTENCILMODE,        FALSE);
    SET_RENDER_STATE(D3DRS_CCW_STENCILFAIL,            D3DSTENCILOP_KEEP);
    SET_RENDER_STATE(D3DRS_CCW_STENCILZFAIL,           D3DSTENCILOP_KEEP);
    SET_RENDER_STATE(D3DRS_CCW_STENCILPASS,            D3DSTENCILOP_KEEP);
    SET_RENDER_STATE(D3DRS_CCW_STENCILFUNC,            D3DCMP_ALWAYS);
    SET_RENDER_STATE(D3DRS_COLORWRITEENABLE1,          0xf);
    SET_RENDER_STATE(D3DRS_COLORWRITEENABLE2,          0xf);
    SET_RENDER_STATE(D3DRS_COLORWRITEENABLE3,          0xf);
    SET_RENDER_STATE(D3DRS_BLENDFACTOR,                0xffffffff);
    SET_RENDER_STATE(D3DRS_SRGBWRITEENABLE,            0);
    SET_RENDER_STATE(D3DRS_DEPTHBIAS,                  0);
    SET_RENDER_STATE(D3DRS_WRAP8,                      0);
    SET_RENDER_STATE(D3DRS_WRAP9,                      0);
    SET_RENDER_STATE(D3DRS_WRAP10,                     0);
    SET_RENDER_STATE(D3DRS_WRAP11,                     0);
    SET_RENDER_STATE(D3DRS_WRAP12,                     0);
    SET_RENDER_STATE(D3DRS_WRAP13,                     0);
    SET_RENDER_STATE(D3DRS_WRAP14,                     0);
    SET_RENDER_STATE(D3DRS_WRAP15,                     0);
    SET_RENDER_STATE(D3DRS_SEPARATEALPHABLENDENABLE,   FALSE);
    SET_RENDER_STATE(D3DRS_SRCBLENDALPHA,              D3DBLEND_ONE);
    SET_RENDER_STATE(D3DRS_DESTBLENDALPHA,             D3DBLEND_ZERO);
    SET_RENDER_STATE(D3DRS_BLENDOPALPHA,               D3DBLENDOP_ADD);
#if defined(_XENON)
    SET_RENDER_STATE(D3DRS_VIEWPORTENABLE,             TRUE);
    SET_RENDER_STATE(D3DRS_HIGHPRECISIONBLENDENABLE,   FALSE);
    SET_RENDER_STATE(D3DRS_HIGHPRECISIONBLENDENABLE1,  FALSE);
    SET_RENDER_STATE(D3DRS_HIGHPRECISIONBLENDENABLE2,  FALSE);
    SET_RENDER_STATE(D3DRS_HIGHPRECISIONBLENDENABLE3,  FALSE);
    SET_RENDER_STATE(D3DRS_TESSELLATIONMODE,           D3DTM_DISCRETE);
#endif

    for (ULONG i = 0; i < RSMGR_MAX_SAMPLER; ++i)
    {
        SET_SAMPLER_STATE(i, D3DSAMP_ADDRESSU,      D3DTADDRESS_WRAP);
        SET_SAMPLER_STATE(i, D3DSAMP_ADDRESSV,      D3DTADDRESS_WRAP);
        SET_SAMPLER_STATE(i, D3DSAMP_ADDRESSW,      D3DTADDRESS_WRAP);
        SET_SAMPLER_STATE(i, D3DSAMP_BORDERCOLOR,   0);
        SET_SAMPLER_STATE(i, D3DSAMP_MAGFILTER,     D3DTEXF_LINEAR);
        SET_SAMPLER_STATE(i, D3DSAMP_MINFILTER,     D3DTEXF_LINEAR);
        SET_SAMPLER_STATE(i, D3DSAMP_MIPFILTER,     D3DTEXF_LINEAR);
        SET_SAMPLER_STATE(i, D3DSAMP_MIPMAPLODBIAS, 0);
    }

#if defined(_XENON)
    m_oCurrentBlendState.SrcBlend  = m_oCurrentBlendState.SrcBlendAlpha  = D3DBLEND_ONE;
    m_oCurrentBlendState.BlendOp   = m_oCurrentBlendState.BlendOpAlpha   = D3DBLENDOP_ADD;
    m_oCurrentBlendState.DestBlend = m_oCurrentBlendState.DestBlendAlpha = D3DBLEND_ZERO;
    m_pD3DDevice->SetBlendState(0, m_oCurrentBlendState);
#endif;

#undef SET_RENDER_STATE
#undef SET_SAMPLER_STATE

	m_oDirtyFlags.DirtyAll();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
BOOL XeRenderStateManager::Update(XeRenderObject *_pObject, BOOL _bUpdateVSConstants, BOOL _bUpdatePSConstants)
{
    CXBBeginEventObject oEvent("XeRenderStateManager::Update");

    HRESULT hr;

    // update index buffer
    if (m_oDirtyFlags.bIndexBuffer)
    {
        // send index buffer to D3D
        LPDIRECT3DINDEXBUFFER9 pIndexBuffer = (m_pCurrentIndexBuffer == NULL) ? NULL : m_pCurrentIndexBuffer->GetIB();
        hr = m_pD3DDevice->SetIndices(pIndexBuffer);
        ERR_X_Assert(SUCCEEDED(hr));
    }

    // update vertex declaration
    if (m_oDirtyFlags.bVertexDeclaration)
    {
        // send vertex declaration to D3D
        hr = m_pD3DDevice->SetVertexDeclaration(m_pCurrentVertexDeclaration);
        ERR_X_Assert(SUCCEEDED(hr));
    }

    // update stream
    for (ULONG ulCurStream = 0; ulCurStream < RSMGR_MAX_STREAM; ulCurStream++)
    {
        if ((m_oDirtyFlags.uStream & (1 << ulCurStream)) != 0)
        {
            // send stream state to D3D
            hr = m_pD3DDevice->SetStreamSource( ulCurStream, 
                                                m_apoCurrentVB[ulCurStream],
                                                m_aulCurrentStreamOffset[ulCurStream],
                                                m_aulCurrentStreamStride[ulCurStream]);
            ERR_X_Assert(SUCCEEDED(hr));
        }
    }

    // update vertex shader
    if (m_oDirtyFlags.bVertexShader)
    {
        // send vertex shader to D3D
        LPDIRECT3DVERTEXSHADER9 pShader = (m_pCurrentVertexShader == NULL) ? NULL : m_pCurrentVertexShader->GetShaderInterface();
        hr = m_pD3DDevice->SetVertexShader(pShader);
        ERR_X_Assert(SUCCEEDED(hr));
    }

    // update constants
	if ((_pObject != NULL) && _bUpdateVSConstants)
	{
        PIX_XeBeginEventSpecific(0, "Update Constant Vertex");
		g_oVertexShaderMgr.UpdateShaderConstants(m_pCurrentVertexShader, _pObject);
        PIX_XeEndEventSpecific(0);
	}
/*
    // update dirty render states
    std::map<D3DRENDERSTATETYPE, DWORD>::iterator itRS = m_mapDirtyRenderStates.begin();
    while (itRS != m_mapDirtyRenderStates.end())
    {
        // send render state to D3D
        hr = m_pD3DDevice->SetRenderState(itRS->first, itRS->second);
        ERR_X_Assert(SUCCEEDED(hr));

        // keep a local copy of the render state
        m_adwRenderStates[itRS->first] = itRS->second;

        itRS++;
    }
    m_mapDirtyRenderStates.resize(0);

    // update dirty sampler states
    for (ULONG ulCurSampler = 0; ulCurSampler < RSMGR_MAX_SAMPLER; ulCurSampler++)
    {
        std::map<D3DSAMPLERSTATETYPE, DWORD>::iterator itSS = m_mapDirtySamplerStates[ulCurSampler].begin();
        while (itSS != m_mapDirtySamplerStates[ulCurSampler].end())
        {
            // send sampler state to D3D
            hr = m_pD3DDevice->SetSamplerState(ulCurSampler, itSS->first, itSS->second);
            ERR_X_Assert(SUCCEEDED(hr));

            // keep a local copy of the sampler state
            m_adwSamplerStates[ulCurSampler][itSS->first] = itSS->second;

            itSS++;
        }

        m_mapDirtySamplerStates[ulCurSampler].resize(0);
    }
*/
    // update textures
    if (m_oDirtyFlags.uTexture != 0)
    {
        // send textures to D3D
        UpdateTextures();
    }

    // update pixel shader
    if (m_oDirtyFlags.bPixelShader)
    {
        // send pixel shader to D3D
        LPDIRECT3DPIXELSHADER9 pShader = (m_pCurrentPixelShader == NULL) ? NULL : m_pCurrentPixelShader->GetShaderInterface();
        hr = m_pD3DDevice->SetPixelShader(pShader);
        ERR_X_Assert(SUCCEEDED(hr));
    }

    // update constants
	if ((_pObject != NULL) && (m_pCurrentPixelShader != NULL) && _bUpdatePSConstants)
	{
        PIX_XeBeginEventSpecific(0, "Update Constant Pixel");
	    g_oPixelShaderMgr.UpdateShaderConstants(m_pCurrentPixelShader, _pObject);
        PIX_XeEndEventSpecific(0);
	}

    // clear all dirty flags.
    m_oDirtyFlags.Clear();

    return TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeRenderStateManager& XeRenderStateManager::operator=(const XeRenderStateManager& _roMgr)
{
    // copy all members
    m_pCurrentPixelShader           = _roMgr.m_pCurrentPixelShader;
    m_pCurrentVertexShader          = _roMgr.m_pCurrentVertexShader;
    m_pCurrentVertexDeclaration     = _roMgr.m_pCurrentVertexDeclaration;
    m_pCurrentIndexBuffer           = _roMgr.m_pCurrentIndexBuffer;
    L_memcpy(m_aulCurrentStreamOffset, _roMgr.m_aulCurrentStreamOffset, RSMGR_MAX_STREAM * sizeof(ULONG));
    L_memcpy(m_aulCurrentStreamStride, _roMgr.m_aulCurrentStreamStride, RSMGR_MAX_STREAM * sizeof(ULONG));
    L_memcpy(m_apoCurrentVB, _roMgr.m_apoCurrentVB, RSMGR_MAX_STREAM * sizeof(LPDIRECT3DVERTEXBUFFER9));
    L_memcpy(m_adwCurrentTexture, _roMgr.m_adwCurrentTexture, RSMGR_MAX_TEXTURE * sizeof(DWORD));
    L_memcpy(m_adwRenderStates, _roMgr.m_adwRenderStates, D3DRS_MAX * sizeof(DWORD));
    for (ULONG ulCurSampler = 0; ulCurSampler < RSMGR_MAX_SAMPLER; ulCurSampler++)
    {
        L_memcpy(&m_adwSamplerStates[ulCurSampler][0], &_roMgr.m_adwSamplerStates[ulCurSampler][0], D3DSAMP_MAX * sizeof(DWORD));
    }

    return *this;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
BOOL XeRenderStateManager::UpdateTextures()
{
    CXBBeginEventObject oEvent("XeRenderStateManager::UpdateTextures");

    for (ULONG ulCurTex = 0; ulCurTex < RSMGR_MAX_TEXTURE; ulCurTex++)
    {
        if (m_oDirtyFlags.uTexture & (1 << ulCurTex))
        {
            // texture is dirty, fetch the texture interface
            IDirect3DBaseTexture9* pD3DTexture = g_oXeTextureMgr.GetTextureFromID(m_adwCurrentTexture[ulCurTex]);

            // send texture to D3D
            m_pD3DDevice->SetTexture(ulCurTex, pD3DTexture);
        }
    }

    return TRUE;
}

void XeRenderStateManager::UnbindAll(void)
{
    ULONG i;

    if (m_pD3DDevice == NULL)
        return;

    // Textures
    for (i = 0; i < RSMGR_MAX_TEXTURE; ++i)
    {
        m_pD3DDevice->SetTexture(i, NULL);
    }

    // Streams
    for (i = 0; i < RSMGR_MAX_STREAM; ++i)
    {
        m_pD3DDevice->SetStreamSource(i, NULL, 0, 0);
    }

    // Index buffers
    m_pD3DDevice->SetIndices(NULL);

    // All dirty
    m_oDirtyFlags.DirtyAll();
}
