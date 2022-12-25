// ------------------------------------------------------------------------------------------------
// File   : XeLightShaftManager.cpp
// Date   : 2005-03-08
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include "Precomp.h"

#if defined(_XENON)
#include <xgraphics.h>
#endif

#include "XeMesh.h"
#include "XeMaterial.h"
#include "XeLightShaftManager.h"
#include "XeRenderer.h"
#include "XeShader.h"
#include "XeRenderStateManager.h"
#include "XeTextureManager.h"
#include "XeShadowManager.h"
#include "XeRenderTargetManager.h"

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------
const D3DXVECTOR3 XE_LSM_ZERO(0.0f, 0.0f, 0.0f);
const D3DXVECTOR3 XE_LSM_X_AXIS( -1.0f,  0.0f,  0.0f );
const D3DXVECTOR3 XE_LSM_Y_AXIS(  0.0f,  0.0f, -1.0f );
const D3DXVECTOR3 XE_LSM_Z_AXIS(  0.0f, -1.0f,  0.0f );

const FLOAT XE_LSM_BASE_STEP = 1.0f / (FLOAT)XeLightShaftManager::MaxPlanes;

const ULONG XE_LSM_MODE_MASK  = 0xf0000000;
const ULONG XE_LSM_MODE_SHIFT = 28;

const D3DFORMAT XE_LSM_WORK_BUFFER_FORMAT  = D3DFMT_A8R8G8B8;
#if defined(_XENON)
const D3DFORMAT XE_LSM_DEPTH_BUFFER_FORMAT = D3DFMT_D24S8;
#else
const D3DFORMAT XE_LSM_DEPTH_BUFFER_FORMAT = D3DFMT_R32F;
#endif

#if defined(_XENON)
#define XE_LSM_USE_BACKBUFFER TRUE
#else
#define XE_LSM_USE_BACKBUFFER FALSE
#endif

// ------------------------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------------------------
XeLightShaftManager g_oXeLightShaftManager;

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - Utilities
// ------------------------------------------------------------------------------------------------

inline void XeMATHMatrixToD3DMatrix(D3DXMATRIX* _pst_D3DMatrix, MATH_tdst_Matrix* _pst_Matrix)
{
    MATH_tdst_Matrix stTempMatrix;
    MATH_MakeOGLMatrix(&stTempMatrix, _pst_Matrix);
    *_pst_D3DMatrix = *(D3DXMATRIX*)&stTempMatrix;
}

inline void XeComputePlaneSize(FLOAT _f_Distance, FLOAT _f_FOVX, FLOAT _f_FOVY, FLOAT* _pf_XSize, FLOAT* _pf_YSize)
{
    *_pf_XSize = _f_Distance * fTan(_f_FOVX * 0.5f);
    *_pf_YSize = _f_Distance * fTan(_f_FOVY * 0.5f);
}

inline void XePlaneFromTriangle(D3DXVECTOR4* _pst_Plane, const D3DXVECTOR3& _vP1,
                                                         const D3DXVECTOR3& _vP2,
                                                         const D3DXVECTOR3& _vP3)
{
    D3DXVECTOR3 vU;
    D3DXVECTOR3 vV;

    D3DXVec3Subtract(&vU, &_vP3, &_vP1);
    D3DXVec3Subtract(&vV, &_vP2, &_vP1);
    D3DXVec3Cross((D3DXVECTOR3*)_pst_Plane, &vU, &vV);
    D3DXVec3Normalize((D3DXVECTOR3*)_pst_Plane, (D3DXVECTOR3*)_pst_Plane);

    _pst_Plane->w = -D3DXVec3Dot(&_vP1, (D3DXVECTOR3*)_pst_Plane);
}

inline void XeD3DXVec3AddScale(D3DXVECTOR3* _vRes, const D3DXVECTOR3* _vA, const D3DXVECTOR3* _vB, FLOAT _fScale)
{
    D3DXVec3Scale(_vRes, _vB, _fScale);
    D3DXVec3Add(_vRes, _vRes, _vA);
}

static void WINAPI XeLSMFillNoiseTexture(D3DXVECTOR4* _pOut, const D3DXVECTOR2*, const D3DXVECTOR2*, LPVOID)
{
    _pOut->x = 0.0f;
    _pOut->y = 0.0f;
    _pOut->z = 0.0f;
    _pOut->w = 0.5f + 0.1f * fRand(-1.0f, 1.0f);
}

// ------------------------------------------------------------------------------------------------
// IMPLEMENTATION - XeLightShaftManager
// ------------------------------------------------------------------------------------------------

XeLightShaftManager::XeLightShaftManager(void)
{
    m_pPlaneMesh        = NULL;
    m_pFilterMesh       = NULL;
    m_pFilterRenderable = NULL;
    m_pBlurMaterial     = NULL;
    m_pResampleMaterial = NULL;
    m_pRenderable       = NULL;

    m_oLightShaftStateEngine.m_ulNbLightShafts     = 0;
    m_oLightShaftStateEngine.m_ulCurrentLightShaft = 0;

    m_bRenderToWorkBuffer = FALSE;

    for (ULONG i = 0; i < MaxFastLightShafts; ++i)
    {
        m_pApplyMaterial[i] = NULL;
    }

    for (ULONG i = 0; i < MaxWorkBuffers; ++i)
    {
        m_pWorkBuffers[i]    = NULL;
        m_uiWorkBufferIds[i] = (UINT)MAT_Xe_InvalidTextureId;
    }

#if defined(_XENON)

    m_pDepthBufferD24S8    = NULL;
    m_pDepthBufferA8R8G8B8 = NULL;
    m_pDepthBufferMemory   = NULL;

    m_uiDepthBufferA8R8G8B8Id = (UINT)MAT_Xe_InvalidTextureId;

#else

    m_pDepthBuffer = NULL;

#endif
    m_uiDepthBufferId = (UINT)MAT_Xe_InvalidTextureId;

    L_memset(&m_oLightShaftStateEngine.m_aLightShafts, 0, XE_LIGHTSHAFT_MAX * sizeof(LightShaftInfo));

    L_memset(&m_aRegisteredLightShafts[0], 0, XE_LIGHTSHAFT_REGISTER_MAX * sizeof(LIGHT_tdst_Light*));
}

XeLightShaftManager::~XeLightShaftManager(void)
{
}

void XeLightShaftManager::InitializePlaneMesh(void)
{
    XeIndexBuffer* pstIndices  = NULL;
    USHORT*        pwIndices   = NULL;
    PlaneVertex*   pstVertices = NULL;
    ULONG ulNbVertices = 4 * MaxPlanes;
    ULONG ulNbIndices  = 6 * MaxPlanes;
    ULONG ulIndex      = 0;
    ULONG ulBaseVertex = 0;
    ULONG ulColor;
    ULONG i;

    // Create the mesh
    m_pPlaneMesh = new XeMesh();

    // Indices
    pstIndices   = g_XeBufferMgr.CreateIndexBuffer(ulNbIndices);
    pwIndices    = (USHORT*)pstIndices->Lock(ulNbIndices);
    ulIndex      = 0;
    ulBaseVertex = 0;
    for (i = 0; i < MaxPlanes; ++i)
    {
        pwIndices[ulIndex++] = (USHORT)(ulBaseVertex + 0);
        pwIndices[ulIndex++] = (USHORT)(ulBaseVertex + 1);
        pwIndices[ulIndex++] = (USHORT)(ulBaseVertex + 2);

        pwIndices[ulIndex++] = (USHORT)(ulBaseVertex + 0);
        pwIndices[ulIndex++] = (USHORT)(ulBaseVertex + 2);
        pwIndices[ulIndex++] = (USHORT)(ulBaseVertex + 3);

        ulBaseVertex += 4;
    }
    pstIndices->Unlock();
    m_pPlaneMesh->SetIndices(pstIndices);

    // Vertices
    m_pPlaneMesh->AddStream(Plane_VertexFormat, FALSE, NULL, ulNbVertices);
    pstVertices = (PlaneVertex*)m_pPlaneMesh->GetStream(0)->pBuffer->Lock(ulNbVertices, sizeof(PlaneVertex));
    ulIndex     = 0;
    for (i = 0; i < MaxPlanes; ++i)
    {
        switch (i & 0x3)
        {
            case 0: ulColor = 0x00ff0000; break;
            case 1: ulColor = 0x0000ff00; break;
            case 2: ulColor = 0x000000ff; break;
            case 3: ulColor = 0xff000000; break;
        }

        pstVertices[ulIndex].position.x = -1.0f;
        pstVertices[ulIndex].position.y =  1.0f;
        pstVertices[ulIndex].position.z = (FLOAT)i;
        pstVertices[ulIndex].color      = ulColor;
        ++ulIndex;

        pstVertices[ulIndex].position.x =  1.0f;
        pstVertices[ulIndex].position.y =  1.0f;
        pstVertices[ulIndex].position.z = (FLOAT)i;
        pstVertices[ulIndex].color      = ulColor;
        ++ulIndex;

        pstVertices[ulIndex].position.x =  1.0f;
        pstVertices[ulIndex].position.y = -1.0f;
        pstVertices[ulIndex].position.z = (FLOAT)i;
        pstVertices[ulIndex].color      = ulColor;
        ++ulIndex;

        pstVertices[ulIndex].position.x = -1.0f;
        pstVertices[ulIndex].position.y = -1.0f;
        pstVertices[ulIndex].position.z = (FLOAT)i;
        pstVertices[ulIndex].color      = ulColor;
        ++ulIndex;
    }
    m_pPlaneMesh->GetStream(0)->pBuffer->Unlock();
}

void XeLightShaftManager::InitializeFilterMesh(void)
{
    FilterVertex* pstVertices = NULL;
    ULONG ulNbVertices = 4;
    ULONG ulIndex      = 0;

    // Create the mesh
    m_pFilterMesh = new XeMesh();

    // Vertices
    m_pFilterMesh->AddStream(Filter_VertexFormat, FALSE, NULL, ulNbVertices);
    pstVertices = (FilterVertex*)m_pFilterMesh->GetStream(0)->pBuffer->Lock(ulNbVertices, sizeof(FilterVertex));

    ulIndex     = 0;
    pstVertices[ulIndex].position.x = -1.0f;
    pstVertices[ulIndex].position.y =  1.0f;
    pstVertices[ulIndex].position.z =  0.0f;
    pstVertices[ulIndex].texCoord.x =  0.0f;
    pstVertices[ulIndex].texCoord.y =  0.0f;
    pstVertices[ulIndex].color      = 0xffffffff;
    ++ulIndex; 

    pstVertices[ulIndex].position.x =  1.0f;
    pstVertices[ulIndex].position.y =  1.0f;
    pstVertices[ulIndex].position.z =  0.0f;
    pstVertices[ulIndex].texCoord.x =  1.0f;
    pstVertices[ulIndex].texCoord.y =  0.0f;
    pstVertices[ulIndex].color      = 0xffffffff;
    ++ulIndex;

    pstVertices[ulIndex].position.x = -1.0f;
    pstVertices[ulIndex].position.y = -1.0f;
    pstVertices[ulIndex].position.z =  0.0f;
    pstVertices[ulIndex].texCoord.x =  0.0f;
    pstVertices[ulIndex].texCoord.y =  1.0f;
    pstVertices[ulIndex].color      = 0xffffffff;
    ++ulIndex;

    pstVertices[ulIndex].position.x =  1.0f;
    pstVertices[ulIndex].position.y = -1.0f;
    pstVertices[ulIndex].position.z =  0.0f;
    pstVertices[ulIndex].texCoord.x =  1.0f;
    pstVertices[ulIndex].texCoord.y =  1.0f;
    pstVertices[ulIndex].color      = 0xffffffff;
    ++ulIndex;

    m_pFilterMesh->GetStream(0)->pBuffer->Unlock();
}

void XeLightShaftManager::InitializeWorkBuffers(void)
{
    HRESULT hr;
    UINT    uiBackBufferWidth;
    UINT    uiBackBufferHeight;
    ULONG   i;

    g_oXeRenderer.GetBackbufferResolution(&uiBackBufferWidth, &uiBackBufferHeight);

    if (uiBackBufferHeight < 512)
    {
        // Work buffer is always 1/4th of the screen size
        m_dwWorkBufferWidth  = (uiBackBufferWidth  >> 1);
        m_dwWorkBufferHeight = (uiBackBufferHeight >> 1);
    }
    else
    {
        // Work buffer is always 1/16th of the screen size
        m_dwWorkBufferWidth  = (uiBackBufferWidth  >> 2);
        m_dwWorkBufferHeight = (uiBackBufferHeight >> 2);
    }

    // Minimum of 8x8
    if (m_dwWorkBufferWidth < 8)
        m_dwWorkBufferWidth = 8;
    if (m_dwWorkBufferHeight < 8)
        m_dwWorkBufferHeight = 8;

    for (i = 0; i < MaxWorkBuffers; ++i)
    {
        hr = g_oXeRenderer.GetDevice()->CreateTexture(m_dwWorkBufferWidth, m_dwWorkBufferHeight, 
                                                      1, D3DUSAGE_RENDERTARGET, XE_LSM_WORK_BUFFER_FORMAT, 
                                                      D3DPOOL_DEFAULT, &m_pWorkBuffers[i], NULL);
        XeValidateRet(SUCCEEDED(hr) && (m_pWorkBuffers[i] != NULL), , "Failed to create the light shaft work buffer");

        if (m_uiWorkBufferIds[i] == (UINT)MAT_Xe_InvalidTextureId)
        {
            m_uiWorkBufferIds[i] = g_oXeTextureMgr.RegisterUserTexture(m_pWorkBuffers[i]);
        }
        else
        {
            g_oXeTextureMgr.UpdateUserTexture(m_uiWorkBufferIds[i], m_pWorkBuffers[i]);
        }
    }

    // Resample buffer
#if defined(_XENON)

    UINT uiTextureSizeD24S8    = 0;
    UINT uiTextureSizeA8R8G8B8 = 0;

    uiTextureSizeD24S8    = XGSetTextureHeader(m_dwWorkBufferWidth, m_dwWorkBufferHeight, 1, 0, D3DFMT_D24S8,    0, 0, XGHEADER_CONTIGUOUS_MIP_OFFSET, 0, &m_stDepthBufferD24S8,    NULL, NULL);
    uiTextureSizeA8R8G8B8 = XGSetTextureHeader(m_dwWorkBufferWidth, m_dwWorkBufferHeight, 1, 0, D3DFMT_A8R8G8B8, 0, 0, XGHEADER_CONTIGUOUS_MIP_OFFSET, 0, &m_stDepthBufferA8R8G8B8, NULL, NULL);
    ERR_X_Assert(uiTextureSizeD24S8 == uiTextureSizeA8R8G8B8);

    m_pDepthBufferMemory = XPhysicalAlloc(uiTextureSizeD24S8, MAXULONG_PTR, 4096, PAGE_READWRITE | PAGE_WRITECOMBINE);
    ERR_X_Assert(m_pDepthBufferMemory != NULL);

    XGOffsetResourceAddress(&m_stDepthBufferD24S8,    m_pDepthBufferMemory);
    XGOffsetResourceAddress(&m_stDepthBufferA8R8G8B8, m_pDepthBufferMemory);

    m_pDepthBufferD24S8    = &m_stDepthBufferD24S8;
    m_pDepthBufferA8R8G8B8 = &m_stDepthBufferA8R8G8B8;

    if (m_uiDepthBufferId == (UINT)MAT_Xe_InvalidTextureId)
    {
        m_uiDepthBufferId = g_oXeTextureMgr.RegisterUserTexture(m_pDepthBufferD24S8);
    }
    else
    {
        g_oXeTextureMgr.UpdateUserTexture(m_uiDepthBufferId, m_pDepthBufferD24S8);
    }

    // Original depth buffer as A8R8G8B8
    XGSetTextureHeader(uiBackBufferWidth, uiBackBufferHeight, 1, 0, D3DFMT_A8R8G8B8, 0, 0, XGHEADER_CONTIGUOUS_MIP_OFFSET, 0, &m_stDepthBufferFullScaleA8R8G8B8, NULL, NULL);
    m_stDepthBufferFullScaleA8R8G8B8.Format.BaseAddress = g_oXeTextureMgr.GetTextureFromID(g_oXeTextureMgr.GetDepthBufferID())->Format.BaseAddress;

    if (m_uiDepthBufferA8R8G8B8Id == (UINT)MAT_Xe_InvalidTextureId)
    {
        m_uiDepthBufferA8R8G8B8Id = g_oXeTextureMgr.RegisterUserTexture(&m_stDepthBufferFullScaleA8R8G8B8);
    }
    else
    {
        g_oXeTextureMgr.UpdateUserTexture(m_uiDepthBufferA8R8G8B8Id, &m_stDepthBufferFullScaleA8R8G8B8);
    }

#else

    hr = g_oXeRenderer.GetDevice()->CreateTexture(m_dwWorkBufferWidth, m_dwWorkBufferHeight,
                                                  1, D3DUSAGE_RENDERTARGET, XE_LSM_DEPTH_BUFFER_FORMAT,
                                                  D3DPOOL_DEFAULT, &m_pDepthBuffer, NULL);
    XeValidateRet(SUCCEEDED(hr) && (m_pDepthBuffer != NULL), , "Failed to create the resampled depth buffer");

    if (m_uiDepthBufferId == (UINT)MAT_Xe_InvalidTextureId)
    {
        m_uiDepthBufferId = g_oXeTextureMgr.RegisterUserTexture(m_pDepthBuffer);
    }
    else
    {
        g_oXeTextureMgr.UpdateUserTexture(m_uiDepthBufferId, m_pDepthBuffer);
    }

#endif
}

void XeLightShaftManager::Initialize(void)
{
    ULONG i;

    Shutdown();

    InitializePlaneMesh();
    InitializeFilterMesh();

    // Materials
    for (i = 0; i < XE_LIGHTSHAFT_MAX; ++i)
    {
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial = new XeMaterial();
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetCustomVS(CUSTOM_VS_LIGHTSHAFT);
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetCustomVSFeature(Custom_LightShaftMode, LightShaftMode_Render);
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetCustomPS(CUSTOM_PS_LIGHTSHAFT);
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetCustomPSFeature(Custom_LightShaftMode, LightShaftMode_Render);
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetZState(TRUE, FALSE);

        // Cookie texture
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->AddTextureStage();
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetTextureId(Texture_Cookie, MAT_Xe_InvalidTextureId);
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetAddressMode(Texture_Cookie, D3DTADDRESS_CLAMP, 
                                                                    D3DTADDRESS_CLAMP, 
                                                                    D3DTADDRESS_CLAMP);

        // Noise texture
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->AddTextureStage();
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetTextureId(Texture_Noise, MAT_Xe_InvalidTextureId);
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetAddressMode(Texture_Noise, D3DTADDRESS_WRAP, 
                                                                   D3DTADDRESS_WRAP, 
                                                                   D3DTADDRESS_WRAP);

        // Shadow texture
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->AddTextureStage();
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetTextureId(Texture_Shadow, MAT_Xe_InvalidTextureId);
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetAddressMode(Texture_Shadow, D3DTADDRESS_CLAMP, 
                                                                    D3DTADDRESS_CLAMP, 
                                                                    D3DTADDRESS_CLAMP);

        // Depth texture
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->AddTextureStage();
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetFilterMode(Texture_Shadow, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT);
        m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial->SetAddressMode(Texture_Shadow, D3DTADDRESS_CLAMP, 
                                                                    D3DTADDRESS_CLAMP, 
                                                                    D3DTADDRESS_CLAMP);
    }

    // Blur material
    m_pBlurMaterial = new XeMaterial();
    m_pBlurMaterial->SetCustomVS(CUSTOM_VS_LIGHTSHAFT);
    m_pBlurMaterial->SetCustomVSFeature(Custom_LightShaftMode, LightShaftMode_Blur);
    m_pBlurMaterial->SetCustomPS(CUSTOM_PS_LIGHTSHAFT);
    m_pBlurMaterial->SetCustomPSFeature(Custom_LightShaftMode, LightShaftMode_Blur);
    m_pBlurMaterial->SetZState(FALSE, FALSE);
    m_pBlurMaterial->SetAlphaBlend(FALSE);
    m_pBlurMaterial->AddTextureStage();
    m_pBlurMaterial->SetFilterMode(0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR);
    m_pBlurMaterial->SetAddressMode(0, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);

    // Resample material
    m_pResampleMaterial = new XeMaterial();
    m_pResampleMaterial->SetCustomVS(CUSTOM_VS_LIGHTSHAFT);
    m_pResampleMaterial->SetCustomVSFeature(Custom_LightShaftMode, LightShaftMode_Resample);
    m_pResampleMaterial->SetCustomPS(CUSTOM_PS_LIGHTSHAFT);
    m_pResampleMaterial->SetCustomPSFeature(Custom_LightShaftMode, LightShaftMode_Resample);
    m_pResampleMaterial->SetZState(FALSE, FALSE);
    m_pResampleMaterial->SetAlphaBlend(FALSE);
    m_pResampleMaterial->AddTextureStage();
    m_pResampleMaterial->SetFilterMode(0, D3DTEXF_POINT, D3DTEXF_POINT);
    m_pResampleMaterial->SetAddressMode(0, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);

    // Filter renderable
    m_pFilterRenderable = new XeRenderObject();
    m_pFilterRenderable->SetDrawMask(0xffffffff & ~(GDI_Cul_DM_Lighted | GDI_Cul_DM_Fogged | GDI_Cul_DM_TestBackFace));
    m_pFilterRenderable->SetUserData((LPVOID)ROS_LIGHT_SHAFT);
    m_pFilterRenderable->SetObjectType(XeRenderObject::LightShaft);
    m_pFilterRenderable->SetMesh(m_pFilterMesh);
    m_pFilterRenderable->SetPrimType(XeRenderObject::TriangleStrip);

    // Apply materials
    for (ULONG i = 0; i < MaxFastLightShafts; ++i)
    {
        m_pApplyMaterial[i] = new XeMaterial();
        m_pApplyMaterial[i]->SetCustomVS(CUSTOM_VS_LIGHTSHAFT);
        m_pApplyMaterial[i]->SetCustomVSFeature(Custom_LightShaftMode, LightShaftMode_Apply);
        m_pApplyMaterial[i]->SetCustomPS(CUSTOM_PS_LIGHTSHAFT);
        m_pApplyMaterial[i]->SetCustomPSFeature(Custom_LightShaftMode, LightShaftMode_Apply);
        m_pApplyMaterial[i]->SetZState(FALSE, FALSE);
        m_pApplyMaterial[i]->SetAlphaBlend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
        m_pApplyMaterial[i]->AddTextureStage();
        m_pApplyMaterial[i]->SetFilterMode(0, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR);
        m_pApplyMaterial[i]->SetAddressMode(0, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
    }

    // Renderable for rendering the light shaft in the work buffer
    m_pRenderable = new XeRenderObject();
    m_pRenderable->SetDrawMask(0xffffffff & ~(GDI_Cul_DM_Lighted | GDI_Cul_DM_Fogged | GDI_Cul_DM_TestBackFace));
    m_pRenderable->SetUserData((LPVOID)ROS_LIGHT_SHAFT);
    m_pRenderable->SetObjectType(XeRenderObject::LightShaft);
    m_pRenderable->SetMesh(m_pPlaneMesh);
    m_pRenderable->SetPrimType(XeRenderObject::TriangleList);

    // Work buffers
    InitializeWorkBuffers();

    Update();
}

void XeLightShaftManager::OnDeviceLost(void)
{
    for (ULONG i = 0; i < MaxWorkBuffers; ++i)
    {
        SAFE_RELEASE(m_pWorkBuffers[i]);
    }

#if defined(_XENON)

    m_pDepthBufferD24S8    = NULL;
    m_pDepthBufferA8R8G8B8 = NULL;

    if (m_pDepthBufferMemory)
    {
        XPhysicalFree(m_pDepthBufferMemory);
        m_pDepthBufferMemory = NULL;
    }

#else

    SAFE_RELEASE(m_pDepthBuffer);

#endif
}

void XeLightShaftManager::OnDeviceReset(void)
{
    // Restore the work buffers
    InitializeWorkBuffers();

    Update();
}

void XeLightShaftManager::Shutdown(void)
{
    ULONG i;

    for (i = 0; i < MaxWorkBuffers; ++i)
    {
        SAFE_RELEASE(m_pWorkBuffers[i]);
        m_uiWorkBufferIds[i] = (UINT)MAT_Xe_InvalidTextureId;
    }

#if defined(_XENON)

    m_pDepthBufferD24S8    = NULL;
    m_pDepthBufferA8R8G8B8 = NULL;

    if (m_pDepthBufferMemory != NULL)
    {
        XPhysicalFree(m_pDepthBufferMemory);
        m_pDepthBufferMemory = NULL;
    }

    m_uiDepthBufferId         = (UINT)MAT_Xe_InvalidTextureId;
    m_uiDepthBufferA8R8G8B8Id = (UINT)MAT_Xe_InvalidTextureId;

#else

    SAFE_RELEASE(m_pDepthBuffer);
    m_uiDepthBufferId = (UINT)MAT_Xe_InvalidTextureId;

#endif

    SAFE_DELETE(m_pPlaneMesh);

    for (i = 0; i < XE_LIGHTSHAFT_MAX; ++i)
    {
        SAFE_DELETE(m_oLightShaftStateEngine.m_aLightShafts[i].pMaterial);
    }

    SAFE_DELETE(m_pFilterRenderable);
    SAFE_DELETE(m_pFilterMesh);

    SAFE_DELETE(m_pBlurMaterial);
    SAFE_DELETE(m_pResampleMaterial);

    for (i = 0; i < MaxFastLightShafts; ++i)
    {
        SAFE_DELETE(m_pApplyMaterial[i]);
    }

    SAFE_DELETE(m_pRenderable);

    m_oLightShaftStateEngine.m_ulNbLightShafts     = 0;
    m_oLightShaftStateEngine.m_ulCurrentLightShaft = 0;
}

void XeLightShaftManager::GetUsedTextures(char* _pc_UsedIndex)
{
    ULONG i;

    // Check the lights for light shafts with textures
    for (i = 0; i < XE_LIGHTSHAFT_REGISTER_MAX; ++i)
    {
        LIGHT_tdst_Light* pst_Light = m_aRegisteredLightShafts[i];

        if (pst_Light == NULL)
            continue;

        if ((pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_LightShaft)
        {
            // Cookie texture
            if (pst_Light->st_LightShaft.us_CookieTexture != -1)
            {
                _pc_UsedIndex[pst_Light->st_LightShaft.us_CookieTexture] = 1;
            }

            // Noise texture
            if (pst_Light->st_LightShaft.us_NoiseTexture != -1)
            {
                _pc_UsedIndex[pst_Light->st_LightShaft.us_NoiseTexture] = 1;
            }
        }
    }
}

void XeLightShaftManager::RegisterLightShaft(LIGHT_tdst_Light* _pst_Light)
{
    ULONG i;

    for (i = 0; i < XE_LIGHTSHAFT_REGISTER_MAX; ++i)
    {
        if ((m_aRegisteredLightShafts[i] == NULL) ||
            (m_aRegisteredLightShafts[i] == _pst_Light))
        {
            m_aRegisteredLightShafts[i] = _pst_Light;
            break;
        }
    }
}

void XeLightShaftManager::UnregisterLightShaft(LIGHT_tdst_Light* _pst_Light)
{
    ULONG i;

    for (i = 0; i < XE_LIGHTSHAFT_REGISTER_MAX; ++i)
    {
        if (m_aRegisteredLightShafts[i] == _pst_Light)
        {
            m_aRegisteredLightShafts[i] = NULL;
            break;
        }
    }
}

void XeLightShaftManager::ClearRequests(void)
{
    m_oLightShaftStateEngine.m_ulNbLightShafts     = 0;
    m_oLightShaftStateEngine.m_ulCurrentLightShaft = 0;

    m_oLightShaftStateRender.m_ulNbLightShafts     = 0;
    m_oLightShaftStateRender.m_ulCurrentLightShaft = 0;
}

void XeLightShaftManager::Update(void)
{
    // Reset
    m_oLightShaftStateEngine.m_ulNbLightShafts     = 0;
    m_oLightShaftStateEngine.m_ulCurrentLightShaft = 0;
}

void XeLightShaftManager::QueueLightShaftForRender(OBJ_tdst_GameObject* _pst_GO)
{
    LIGHT_tdst_Light* pst_Light;

    if (!_pst_GO || !OBJ_b_TestIdentityFlag(_pst_GO,  OBJ_C_IdentityFlag_Lights))
    {
        XeValidateRet(FALSE, , "XeLightShaftManager::RenderLightShaft() - Object is invalid or is not a light");
    }

    pst_Light = (LIGHT_tdst_Light*)_pst_GO->pst_Extended->pst_Light;
    if (!pst_Light)
    {
        XeValidateRet(FALSE, , "XeLightShaftManager::RenderLightShaft() - Object does not have a valid light");
    }

    if ((pst_Light->ul_Flags & LIGHT_Cul_LF_Type) != LIGHT_Cul_LF_LightShaft)
    {
        XeValidateRet(FALSE, , "XeLightShaftManager::RenderLightShaft() - Light is not a light shaft");
    }

    if (!(pst_Light->ul_Flags & LIGHT_Cul_LF_Active))
        return;

    if (m_oLightShaftStateEngine.m_ulNbLightShafts == XE_LIGHTSHAFT_MAX)
        return;

#if defined(ACTIVE_EDITORS)
    // We don't need to render light shafts when in forced colors or in wireframe
    if ((GDI_gpst_CurDD->ul_DrawMask & GDI_Cul_DM_DontForceColor) == 0)
        return;
    if ((GDI_gpst_CurDD->ul_WiredMode & 3) != 0)
        return;
#endif

    // Register the light shaft
    m_oLightShaftStateEngine.m_aLightShafts[m_oLightShaftStateEngine.m_ulNbLightShafts].pGO    = _pst_GO;
    m_oLightShaftStateEngine.m_aLightShafts[m_oLightShaftStateEngine.m_ulNbLightShafts].pLight = pst_Light;

    // Update the variables in the information structure
    UpdateLightInformation(&m_oLightShaftStateEngine.m_aLightShafts[m_oLightShaftStateEngine.m_ulNbLightShafts]);

    ++m_oLightShaftStateEngine.m_ulNbLightShafts;
}

bool XeLightShaftManager::BeginRendering(ULONG _ulIndex)
{
    m_ulMode = ((_ulIndex & XE_LSM_MODE_MASK) >> XE_LSM_MODE_SHIFT);

    _ulIndex &= ~XE_LSM_MODE_MASK;
    XeValidate(_ulIndex < m_oLightShaftStateRender.m_ulNbLightShafts, "Invalid light shaft index");

    ULONG ulPlane;

    m_oLightShaftStateRender.m_ulCurrentLightShaft = _ulIndex;

    switch (m_ulMode)
    {
        case LightShaftMode_Render:
            {
                // User clip using the light's frustum
                for (ulPlane = 0; ulPlane < MaxClipPlanes; ++ulPlane)
                {
                    if ((m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].ulActiveClipPlanes & (1 << ulPlane)) != 0)
                    {
                        g_oRenderStateMgr.SetClipPlane(ulPlane, (const float*)m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].vClipPlanes[ulPlane]);
                    }
                }

                // Activate the necessary user clipping planes
                g_oRenderStateMgr.SetRenderState(D3DRS_CLIPPLANEENABLE, m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].ulActiveClipPlanes);

                // Set the plane count in the mesh
                m_pPlaneMesh->GetIndices()->SetFaceCount(2 * m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].ulNbPlanes);
            }
            break;

        case LightShaftMode_Apply:
            break;

        case LightShaftMode_Blur:
            break;

        case LightShaftMode_Resample:
            break;

        default:
            ERR_OutputDebugString("[Xe3D] XeLightShaftManager - Invalid rendering mode for a light shaft\n");
            return false;
            break;
    }

    return true;
}

void XeLightShaftManager::EndRendering(void)
{
    switch (m_ulMode)
    {
        case LightShaftMode_Render:
            {
                // Disable user clip planes
                 g_oRenderStateMgr.SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
            }
            break;

        case LightShaftMode_Apply:
            break;

        case LightShaftMode_Blur:
            break;

        case LightShaftMode_Resample:
            break;
    }
}

void XeLightShaftManager::PreRender(void)
{
    ULONG aulFastLightShafts[MaxFastLightShafts] = { 0xffffffff, 0xffffffff };
    UCHAR aucMaxLOD[MaxFastLightShafts]          = { 0, 0 };
    ULONG ulNbFastLightShafts                    = 0;
    BOOL  bEnableFastMode                        = FALSE;
    ULONG i;
    ULONG j;
    ULONG k;

    if (m_oLightShaftStateRender.m_ulNbLightShafts == 0)
        return;

    CXBBeginEventObject oEvent("XeLightShaftManager::PreRender");

    // Can we do some special processing on some of the light shafts?
    if (m_pWorkBuffers[0] != NULL)
    {
        bEnableFastMode = TRUE;
    }
 
    // Find out the most important light shaft on screen
    if (bEnableFastMode)
    {
        for (i = 0; i < m_oLightShaftStateRender.m_ulNbLightShafts; ++i)
        {
            for (j = 0; j < MaxFastLightShafts; ++j)
            {
                UCHAR ucLOD = m_oLightShaftStateRender.m_aLightShafts[i].pGO->uc_LOD_Vis;

                if (aulFastLightShafts[j] == 0xffffffff)
                {
                    aulFastLightShafts[j] = i;
                    aucMaxLOD[j]          = ucLOD;
                    ++ulNbFastLightShafts;
                    break;
                }
                else if (ucLOD > aucMaxLOD[j])
                {
                    if (j < MaxFastLightShafts - 1)
                    {
                        for (k = MaxFastLightShafts - 1; k > j; --k)
                        {
                            aulFastLightShafts[k] = aulFastLightShafts[k - 1];
                            aucMaxLOD[k]          = aucMaxLOD[k - 1];
                        }
                    }

                    aulFastLightShafts[j] = i;
                    aucMaxLOD[j]          = ucLOD;
                    if (ulNbFastLightShafts < MaxFastLightShafts)
                    {
                        ++ulNbFastLightShafts;
                    }
                    break;
                }
            }
        }

        if (bEnableFastMode)
        {
            for (i = 0; i < ulNbFastLightShafts; ++i)
            {
                // Update the textures
                if (NbBlurPasses & 1)
                {
                    m_pApplyMaterial[i]->SetTextureId(0, m_uiWorkBufferIds[MaxWorkBuffers - 1 - i]);
                }
                else
                {
                    m_pApplyMaterial[i]->SetTextureId(0, m_uiWorkBufferIds[i]);
                }

                // Queue the filtered light shaft
                g_oXeRenderer.QueueLightShaftForRender(m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].pGO->pst_GlobalMatrix,
                                                       m_pFilterMesh, m_pApplyMaterial[i],
                                                       0xffffffff & ~(GDI_Cul_DM_Lighted | GDI_Cul_DM_Fogged | GDI_Cul_DM_TestBackFace),
                                                       XeRenderObject::TriangleStrip,
                                                       0, 0, (LPVOID)ROS_LIGHT_SHAFT,
                                                       (LightShaftMode_Apply << XE_LSM_MODE_SHIFT) | aulFastLightShafts[i],
                                                       m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulWorldMatrixIndex,
                                                       m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulWorldViewMatrixIndex,
                                                       m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulProjectionMatrixIndex);
            }
        }
    }

    // Queue the light shafts that will be rendered normally 
    for (i = 0; i < m_oLightShaftStateRender.m_ulNbLightShafts; ++i)
    {
        BOOL bCancel = FALSE;

        for (j = 0; j < ulNbFastLightShafts; ++j)
        {
            if (aulFastLightShafts[j] == i)
            {
                bCancel = TRUE;
                break;
            }
        }

        if (!bCancel)
        {
            g_oXeRenderer.QueueLightShaftForRender(m_oLightShaftStateRender.m_aLightShafts[i].pGO->pst_GlobalMatrix,
                                                   m_pPlaneMesh,
                                                   m_oLightShaftStateRender.m_aLightShafts[i].pMaterial, 
                                                   0xffffffff & ~(GDI_Cul_DM_Lighted | GDI_Cul_DM_Fogged | GDI_Cul_DM_TestBackFace), 
                                                   XeRenderObject::TriangleList, 
                                                   0, 0, (LPVOID)ROS_LIGHT_SHAFT,
                                                   (LightShaftMode_Render << XE_LSM_MODE_SHIFT) | i,
                                                   m_oLightShaftStateRender.m_aLightShafts[i].ulWorldMatrixIndex,
                                                   m_oLightShaftStateRender.m_aLightShafts[i].ulWorldViewMatrixIndex,
                                                   m_oLightShaftStateRender.m_aLightShafts[i].ulProjectionMatrixIndex);
        }
    }

    // We don't need to go through the rest of the function if we are not doing any special
    // processing on one of the light shafts
    if (!bEnableFastMode)
        return;

    // Resample
    if (
#if defined(_XENON)
        g_oXeRenderTargetMgr.BeginRenderTarget(0, m_pDepthBufferA8R8G8B8, NULL, TRUE, TRUE, 0,
                                               FALSE, 0.0f, XE_LSM_USE_BACKBUFFER && TRUE, D3DFMT_A8R8G8B8, D3DFMT_D16, g_oXeRenderer.GetMultiSampleType())
#else
        g_oXeRenderTargetMgr.BeginRenderTarget(0, m_pDepthBuffer, NULL, TRUE, TRUE, 0,
                                               FALSE, 0.0f, XE_LSM_USE_BACKBUFFER && TRUE, XE_LSM_DEPTH_BUFFER_FORMAT, D3DFMT_D16, g_oXeRenderer.GetMultiSampleType())
#endif
       )
    {
#if defined(_XENON)
        m_pResampleMaterial->SetTextureId(0, m_uiDepthBufferA8R8G8B8Id);
#else
        m_pResampleMaterial->SetTextureId(0, g_oXeTextureMgr.GetDepthBufferID());
#endif

        m_pRenderable->SetMesh(m_pFilterMesh);
        m_pRenderable->SetMaterial(m_pResampleMaterial);
        m_pRenderable->SetExtraDataIndex((LightShaftMode_Resample << XE_LSM_MODE_SHIFT));
        m_pRenderable->SetPrimType(XeRenderObject::TriangleStrip);

        // Not using any of these matrices in the shader, but something seems to require them to be valid...
        m_pRenderable->SetWorldMatrixIndex(m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[0]].ulWorldMatrixIndex);
        m_pRenderable->SetWorldViewMatrixIndex(m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[0]].ulWorldViewMatrixIndex);
        m_pRenderable->SetProjMatrixIndex(m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[0]].ulProjectionMatrixIndex);

        g_oXeRenderer.RenderObject(m_pRenderable, XeFXManager::RP_DEFAULT);

        g_oXeRenderTargetMgr.EndRenderTarget(0, FALSE, FALSE, 0, FALSE);
    }

    FLOAT fTexelWidth  = 1.0f / (FLOAT)m_dwWorkBufferWidth;
    FLOAT fTexelHeight = 1.0f / (FLOAT)m_dwWorkBufferHeight;

    // Render the selected light shafts in the work buffer
    for (i = 0; i < ulNbFastLightShafts; ++i)
    {
        ULONG aulAlternateBuffers[2];

        if (NbBlurPasses & 1)
        {
            aulAlternateBuffers[0] = MaxWorkBuffers - 2 - i;
            aulAlternateBuffers[1] = MaxWorkBuffers - 1 - i;
        }
        else
        {
            aulAlternateBuffers[0] = i;
            aulAlternateBuffers[1] = i + 1;
        }

        // SC: Keep the current depth buffer even if the size don't match since we are not using
        //     it. This allows for the HiZ information to be preserved.
        if (g_oXeRenderTargetMgr.BeginRenderTarget(0, m_pWorkBuffers[aulAlternateBuffers[0]], NULL, TRUE, TRUE, 0, 
                                                   FALSE, 0.0f, XE_LSM_USE_BACKBUFFER && TRUE, XE_LSM_WORK_BUFFER_FORMAT, D3DFMT_D16, g_oXeRenderer.GetMultiSampleType()))
        {
            // Update the material
            m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].pMaterial->SetZState(FALSE, FALSE);
            m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].pMaterial->SetCustomPSFeature(Custom_EnableColor, 1);

            m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].pMaterial->SetTextureId(Texture_Depth, m_uiDepthBufferId);

            m_pRenderable->SetMesh(m_pPlaneMesh);
            m_pRenderable->SetMaterial(m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].pMaterial);
            m_pRenderable->SetExtraDataIndex((LightShaftMode_Render << XE_LSM_MODE_SHIFT) | aulFastLightShafts[i]);
            m_pRenderable->SetPrimType(XeRenderObject::TriangleList);

            m_pRenderable->SetWorldMatrixIndex(m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulWorldMatrixIndex);
            m_pRenderable->SetWorldViewMatrixIndex(m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulWorldViewMatrixIndex);
            m_pRenderable->SetProjMatrixIndex(m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulProjectionMatrixIndex);

            // Must use a multiple of 4 planes since we split the rendering in the 4 channels
            if ((m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulNbPlanes & 0x3) != 0)
            {
                m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulNbPlanes = (m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulNbPlanes & 0xfffffffc) + 4;
                if (m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulNbPlanes > MaxPlanes)
                    m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].ulNbPlanes = MaxPlanes;
            }

            m_bRenderToWorkBuffer = TRUE;

            // Render the priority light shaft
            g_oXeRenderer.RenderObject(m_pRenderable, XeFXManager::RP_DEFAULT);

            m_bRenderToWorkBuffer = FALSE;

            g_oXeRenderTargetMgr.EndRenderTarget(0, FALSE, FALSE, 0, FALSE);
        }

        // Use the LOD to control how far in we are fetching pixels for blurring so that when the
        // light shaft is far, we will use a very soft blur
        FLOAT fBlurFactor = 0.5f * (FLOAT)m_oLightShaftStateRender.m_aLightShafts[aulFastLightShafts[i]].pGO->uc_LOD_Vis / 255.0f;

        // Blur the work buffer
        for (j = 0; j < NbBlurPasses; ++j)
        {
            // SC: Keep the current depth buffer even if the size don't match since we are not using
            //     it. This allows for the HiZ information to be preserved.
            if (g_oXeRenderTargetMgr.BeginRenderTarget(0, m_pWorkBuffers[aulAlternateBuffers[(j + 1) & 1]], NULL, TRUE, TRUE, 0, 
                                                       FALSE, 0.0f, XE_LSM_USE_BACKBUFFER && TRUE, XE_LSM_WORK_BUFFER_FORMAT, D3DFMT_D16, g_oXeRenderer.GetMultiSampleType()))
            {
                // Update the material
                m_pBlurMaterial->SetTextureId(0, m_uiWorkBufferIds[aulAlternateBuffers[j & 1]]);
                m_pBlurMaterial->SetCustomPSFeature(Custom_CombineChannels, (j == 0) ? 1 : 0);

                m_avTexOffsets[0].x = fBlurFactor * ((FLOAT)j + 1.0f) * -fTexelWidth; m_avTexOffsets[0].y = fBlurFactor * ((FLOAT)j + 1.0f) * -fTexelHeight;
                m_avTexOffsets[0].z = fBlurFactor * ((FLOAT)j + 1.0f) *  fTexelWidth; m_avTexOffsets[0].w = fBlurFactor * ((FLOAT)j + 1.0f) * -fTexelHeight;
                m_avTexOffsets[1].x = fBlurFactor * ((FLOAT)j + 1.0f) *  fTexelWidth; m_avTexOffsets[1].y = fBlurFactor * ((FLOAT)j + 1.0f) *  fTexelHeight;
                m_avTexOffsets[1].z = fBlurFactor * ((FLOAT)j + 1.0f) * -fTexelWidth; m_avTexOffsets[1].w = fBlurFactor * ((FLOAT)j + 1.0f) *  fTexelHeight;

                m_pRenderable->SetMesh(m_pFilterMesh);
                m_pRenderable->SetMaterial(m_pBlurMaterial);
                m_pRenderable->SetExtraDataIndex((LightShaftMode_Blur << XE_LSM_MODE_SHIFT) | aulFastLightShafts[i]);
                m_pRenderable->SetPrimType(XeRenderObject::TriangleStrip);

                // Apply blur
                g_oXeRenderer.RenderObject(m_pRenderable, XeFXManager::RP_DEFAULT);

                g_oXeRenderTargetMgr.EndRenderTarget(0, FALSE, XE_LSM_USE_BACKBUFFER && (j == (NbBlurPasses - 1)) && (i == (ulNbFastLightShafts - 1)), 0, FALSE);
            }
        }
    }

#if defined(SC_DEV)
    DrawRectangleEx(0.75f, 0.00f, 1.00f, 0.25f, 0.0f, 0.0f, 1.0f, 1.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Copy, m_uiWorkBufferIds[0]);
    DrawRectangleEx(0.75f, 0.25f, 1.00f, 0.50f, 0.0f, 0.0f, 1.0f, 1.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Copy, m_uiWorkBufferIds[1]);
    DrawRectangleEx(0.75f, 0.50f, 1.00f, 0.75f, 0.0f, 0.0f, 1.0f, 1.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Copy, m_uiWorkBufferIds[2]);
    DrawRectangleEx(0.75f, 0.75f, 1.00f, 1.00f, 0.0f, 0.0f, 1.0f, 1.0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0.0f, MAT_Cc_Op_Copy, m_uiDepthBufferId);
#endif
}

void XeLightShaftManager::UpdateLightInformation(LightShaftInfo* _pst_Info)
{
    LIGHT_tdst_Light* pLight           = _pst_Info->pLight;
    XeMaterial*       pMaterial        = _pst_Info->pMaterial;
    D3DXMATRIX*       mWorldViewMatrix = g_pXeContextManagerEngine->GetCurrentWorldViewMatrix();
    D3DXMATRIX*       mProjMatrix      = g_pXeContextManagerEngine->GetCurrentProjectionMatrix();
    D3DXVECTOR3       avLocalFrustum[8];
    ULONG ulNbNoiseTex;
    ULONG i;

    // Material
    pMaterial->SetZState(TRUE, FALSE);
    pMaterial->SetTextureId(Texture_Cookie, pLight->st_LightShaft.us_CookieTexture);
    pMaterial->SetTextureId(Texture_Noise,  pLight->st_LightShaft.us_NoiseTexture);

    // Custom features - Number of noise textures
    // SC: TOADD: LOD to disable noise - fade with a noise influence factor
    ulNbNoiseTex = 0;
    if ((pLight->st_LightShaft.ul_Flags & LIGHT_Cul_LightShaft_EnableNoise) != 0)
    {
        if ((pLight->st_LightShaft.ul_Flags & LIGHT_Cul_LightShaft_Enable2ndNoiseChannel) != 0)
        {
            ulNbNoiseTex = 2;
        } 
        else
        {
            ulNbNoiseTex = 1;
        }
    }
    pMaterial->SetCustomVSFeature(Custom_NbNoiseTextures, ulNbNoiseTex);
    pMaterial->SetCustomPSFeature(Custom_NbNoiseTextures, ulNbNoiseTex);

    // Fog
    ULONG bFogEnable = ((pLight->st_LightShaft.ul_Flags & LIGHT_Cul_LightShaft_DisableFog) == 0) ? 1 : 0;
    pMaterial->SetCustomVSFeature(Custom_EnableFog, bFogEnable);
    pMaterial->SetCustomPSFeature(Custom_EnableFog, bFogEnable);

    // Shadow (if a texture was generated)
    LONG lShadowTextureId = MAT_Xe_InvalidTextureId;
    if (_pst_Info->pLight->ul_Flags & LIGHT_Cul_LF_ExtendedShadowLight)
    {
        lShadowTextureId = g_oXeShadowManager.GetShadowLightTexture(_pst_Info->pLight);
    }
    pMaterial->SetCustomVSFeature(Custom_EnableShadow, (lShadowTextureId != MAT_Xe_InvalidTextureId) ? 1 : 0);
    pMaterial->SetCustomPSFeature(Custom_EnableShadow, (lShadowTextureId != MAT_Xe_InvalidTextureId) ? 1 : 0);
    pMaterial->SetTextureId(Texture_Shadow, lShadowTextureId);

    // Disable color
    pMaterial->SetCustomVSFeature(Custom_EnableColor, 0);
    pMaterial->SetCustomPSFeature(Custom_EnableColor, 0);

    // Inverse world view matrix for the texture projection matrix
    D3DXMatrixInverse(&_pst_Info->mInvWorldViewMatrix, NULL, mWorldViewMatrix);

    // World and view space position
    _pst_Info->vLightPosition = *(D3DXVECTOR3*)OBJ_pst_GetAbsolutePosition(_pst_Info->pGO);

    // Build the frustum
    {
        D3DXVECTOR3 vZ;
        D3DXVECTOR3 vTemp;
        FLOAT       fWidth;
        FLOAT       fHeight;

        // Top
        XeComputePlaneSize(_pst_Info->pLight->st_LightShaft.f_Start, 
                           Cf_PiBy180 * _pst_Info->pLight->st_LightShaft.f_FOVX, 
                           Cf_PiBy180 * _pst_Info->pLight->st_LightShaft.f_FOVY, 
                           &fWidth, &fHeight);
        vZ.x = 0.0f;
        vZ.y = 0.0f;
        vZ.z = 0.0f;

        XeD3DXVec3AddScale(&vTemp, &vZ, &XE_LSM_X_AXIS, -fWidth);
        XeD3DXVec3AddScale(&avLocalFrustum[0], &vTemp, &XE_LSM_Y_AXIS, fHeight);

        XeD3DXVec3AddScale(&vTemp, &vZ, &XE_LSM_X_AXIS, fWidth);
        XeD3DXVec3AddScale(&avLocalFrustum[1], &vTemp, &XE_LSM_Y_AXIS, fHeight);

        XeD3DXVec3AddScale(&vTemp, &vZ, &XE_LSM_X_AXIS, fWidth);
        XeD3DXVec3AddScale(&avLocalFrustum[2], &vTemp, &XE_LSM_Y_AXIS, -fHeight);

        XeD3DXVec3AddScale(&vTemp, &vZ, &XE_LSM_X_AXIS, -fWidth);
        XeD3DXVec3AddScale(&avLocalFrustum[3], &vTemp, &XE_LSM_Y_AXIS, -fHeight);

        // Bottom
        XeComputePlaneSize(_pst_Info->pLight->st_LightShaft.f_Start + _pst_Info->pLight->st_LightShaft.f_Length, 
                           Cf_PiBy180 * _pst_Info->pLight->st_LightShaft.f_FOVX, 
                           Cf_PiBy180 * _pst_Info->pLight->st_LightShaft.f_FOVY, 
                           &fWidth, &fHeight);

        D3DXVec3Scale(&vZ, &XE_LSM_Z_AXIS, _pst_Info->pLight->st_LightShaft.f_Length);

        XeD3DXVec3AddScale(&vTemp, &vZ, &XE_LSM_X_AXIS, -fWidth);
        XeD3DXVec3AddScale(&avLocalFrustum[4], &vTemp, &XE_LSM_Y_AXIS, fHeight);

        XeD3DXVec3AddScale(&vTemp, &vZ, &XE_LSM_X_AXIS, fWidth);
        XeD3DXVec3AddScale(&avLocalFrustum[5], &vTemp, &XE_LSM_Y_AXIS, fHeight);

        XeD3DXVec3AddScale(&vTemp, &vZ, &XE_LSM_X_AXIS, fWidth);
        XeD3DXVec3AddScale(&avLocalFrustum[6], &vTemp, &XE_LSM_Y_AXIS, -fHeight);

        XeD3DXVec3AddScale(&vTemp, &vZ, &XE_LSM_X_AXIS, -fWidth);
        XeD3DXVec3AddScale(&avLocalFrustum[7], &vTemp, &XE_LSM_Y_AXIS, -fHeight);

        // Transform in view space
        D3DXVec3TransformCoordArray(_pst_Info->avFrustum, sizeof(D3DXVECTOR3), avLocalFrustum, sizeof(D3DXVECTOR3), mWorldViewMatrix, 8);
    }

    // Compute the clipping planes (D3D wants the planes to be in clip space)
    {
        D3DXPLANE avClipPlanes[6];

        // Build the clip planes in view space

        // Near
        D3DXPlaneFromPoints(&avClipPlanes[0], &_pst_Info->avFrustum[0],
                                              &_pst_Info->avFrustum[3],
                                              &_pst_Info->avFrustum[1]);
        // Far
        D3DXPlaneFromPoints(&avClipPlanes[1], &_pst_Info->avFrustum[4],
                                              &_pst_Info->avFrustum[5],
                                              &_pst_Info->avFrustum[7]);
        // Right
        D3DXPlaneFromPoints(&avClipPlanes[2], &_pst_Info->avFrustum[1],
                                              &_pst_Info->avFrustum[2],
                                              &_pst_Info->avFrustum[5]);
        // Left
        D3DXPlaneFromPoints(&avClipPlanes[3], &_pst_Info->avFrustum[4],
                                              &_pst_Info->avFrustum[7],
                                              &_pst_Info->avFrustum[0]);
        // Top
        D3DXPlaneFromPoints(&avClipPlanes[4], &_pst_Info->avFrustum[0],
                                              &_pst_Info->avFrustum[1],
                                              &_pst_Info->avFrustum[4]);
        // Bottom
        D3DXPlaneFromPoints(&avClipPlanes[5], &_pst_Info->avFrustum[3],
                                              &_pst_Info->avFrustum[7],
                                              &_pst_Info->avFrustum[2]);

        // D3DXPlaneTransform uses the inverse transpose of the transformation matrix...
        D3DXMATRIX mInvTransposeProj;
        D3DXMatrixInverse(&mInvTransposeProj, NULL, mProjMatrix);
        D3DXMatrixTranspose(&mInvTransposeProj, &mInvTransposeProj);

        // Transform the planes in clip space
        D3DXPlaneTransformArray(_pst_Info->vClipPlanes, sizeof(D3DXPLANE), avClipPlanes, sizeof(D3DXPLANE), &mInvTransposeProj, 6);
    }

    // Find the range in view space
    _pst_Info->fStartZ =  Cf_Infinit;
    _pst_Info->fEndZ   = -Cf_Infinit;
    for (i = 0; i < 8; ++i)
    {
        if (_pst_Info->avFrustum[i].z < _pst_Info->fStartZ)
            _pst_Info->fStartZ = _pst_Info->avFrustum[i].z;
        if (_pst_Info->avFrustum[i].z > _pst_Info->fEndZ)
            _pst_Info->fEndZ = _pst_Info->avFrustum[i].z;
    }

    _pst_Info->fCamVFOV = GDI_gpst_CurDD->st_Camera.f_FieldOfVision;
    _pst_Info->fCamHFOV = GDI_gpst_CurDD->st_Camera.f_FieldOfVision;
#if defined(ACTIVE_EDITORS)
    _pst_Info->fCamHFOV *= g_oXeRenderer.GetViewportAspectRatio();
#else 
    _pst_Info->fCamHFOV *= g_oXeRenderer.GetBackbufferAspectRatio();
#endif

    {
        FLOAT fRange;

        // Activate all clipping planes
        _pst_Info->ulActiveClipPlanes = 0x3f;

        // No need to generate planes behind the screen
        if (_pst_Info->fStartZ < 0.0f)
        {
            _pst_Info->fStartZ = 0.0f;
        }

        fRange = _pst_Info->fEndZ - _pst_Info->fStartZ;

        // Find out the number of necessary planes using the length, density and LOD
        FLOAT fMaxPlanes = MATH_f_FloatLimit(fRange * _pst_Info->pLight->st_LightShaft.f_PlaneDensity, 4.0f, (FLOAT)MaxPlanes);
        fMaxPlanes      *= 0.5f + 0.5f * (FLOAT)_pst_Info->pGO->uc_LOD_Vis / 255.0f;
        _pst_Info->ulNbPlanes  = (ULONG)MATH_f_FloatLimit(fMaxPlanes, 4.0f, (FLOAT)MaxPlanes);
    }

    // Rendering indices
    {
        MATH_tdst_Matrix mOGLMatrix;

        MATH_MakeOGLMatrix(&mOGLMatrix, _pst_Info->pGO->pst_GlobalMatrix);
        g_pXeContextManagerEngine->PushWorldMatrix((D3DXMATRIX*)&mOGLMatrix);

        _pst_Info->ulWorldMatrixIndex      = g_pXeContextManagerEngine->GetCurrentWorldMatrixIndex();
        _pst_Info->ulWorldViewMatrixIndex  = g_pXeContextManagerEngine->GetCurrentWorldViewMatrixIndex();
        _pst_Info->ulProjectionMatrixIndex = g_pXeContextManagerEngine->GetCurrentProjMatrixIndex();
    }

    ComputeLightShaftMatrix(_pst_Info);
}

void XeLightShaftManager::ComputeLightShaftMatrix(LightShaftInfo* _pst_Info)
{
    D3DXMATRIX  mReorder;
    D3DXMATRIX  mDisplacement;
    D3DXMATRIX  mLightProjection;
    D3DXMATRIX  mShadowProjection;
    D3DXMATRIX  mScale;
    D3DXMATRIX  mOffset;
    D3DXMATRIX  mTemp;
    D3DXMATRIX  mTemp2;

    // Swap the Y and Z axis
    XeSetD3DXMatrix(&mReorder,  1.0f,  0.0f,  0.0f, 0.0f,
                                0.0f,  0.0f,  1.0f, 0.0f,
                                0.0f,  1.0f,  0.0f, 0.0f,
                                0.0f,  0.0f,  0.0f, 1.0f);

    // Compensate for the near plane distance
    D3DXMatrixTranslation(&mDisplacement, 0.0f, 0.0f, -_pst_Info->pLight->st_LightShaft.f_Start);

    // Light's projective matrix
    D3DXMatrixPerspectiveFovRH(&mLightProjection, Cf_PiBy180 * _pst_Info->pLight->st_LightShaft.f_FOVY, 
                               _pst_Info->pLight->st_LightShaft.f_FOVX / _pst_Info->pLight->st_LightShaft.f_FOVY,
                               _pst_Info->pLight->st_LightShaft.f_Start,
                               _pst_Info->pLight->st_LightShaft.f_Start + _pst_Info->pLight->st_LightShaft.f_Length);

    // Shadow projection matrix
    D3DXMatrixPerspectiveFovRH(&mShadowProjection, Cf_PiBy180 * _pst_Info->pLight->st_LightShaft.f_SpotOuterAngle, 
                               1.0f, _pst_Info->pLight->st_LightShaft.f_Start,
                               _pst_Info->pLight->st_LightShaft.f_Start + _pst_Info->pLight->st_LightShaft.f_Length);

    // Range remapping -1..1 to -0.5..0.5
    D3DXMatrixScaling(&mScale, 0.5f, 0.5f, 1.0f);

    // Range remapping -0.5..0.5 to 0..1 for texture lookup
    D3DXMatrixTranslation(&mOffset, 0.5f, 0.5f, 0.0f);

    // Matrix from view space to light's projective space
    D3DXMatrixMultiply(&mTemp,  &_pst_Info->mInvWorldViewMatrix, &mReorder);
    D3DXMatrixMultiply(&_pst_Info->mLightShaftPosMatrix, &mTemp, &mDisplacement);
    D3DXMatrixMultiply(&mTemp,  &_pst_Info->mLightShaftPosMatrix, &mLightProjection);
    D3DXMatrixMultiply(&mTemp2, &mTemp, &mScale);
    D3DXMatrixMultiply(&_pst_Info->mLightShaftMatrix, &mTemp2, &mOffset);

    // Shadow projection
    D3DXMatrixMultiply(&mTemp,  &_pst_Info->mLightShaftPosMatrix, &mShadowProjection);
    D3DXMatrixMultiply(&mTemp2, &mTemp, &mScale);
    D3DXMatrixMultiply(&_pst_Info->mLightShaftShadowMatrix, &mTemp2, &mOffset);
}

void XeLightShaftManager::GetLightShaftPlaneParams(VECTOR4FLOAT* _pst_Params)
{
    UINT uiWidth;
    UINT uiHeight;

    // Render:
    // x -> 1 / Number of planes used
    // y -> Depth scale
    // z -> First plane's Z in view space
    // w -> Last plane's Z in view space
    // Blur & Apply:
    // xy -> Position offset for proper texture sampling
    // zw -> 0

    switch (m_ulMode)
    {
        case LightShaftMode_Render:
            _pst_Params->x =  1.0f / (FLOAT)m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].ulNbPlanes;
            _pst_Params->y = -1.0f / m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].pLight->st_LightShaft.f_Length;
            _pst_Params->z = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].fStartZ;
            _pst_Params->w = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].fEndZ;
            break;

        case LightShaftMode_Blur:
            _pst_Params->x = -1.0f / (FLOAT)m_dwWorkBufferWidth;
            _pst_Params->y =  1.0f / (FLOAT)m_dwWorkBufferHeight;
            _pst_Params->z =  0.0f;
            _pst_Params->w =  0.0f;
            break;

        case LightShaftMode_Apply:
            g_oXeRenderer.GetBackbufferResolution(&uiWidth, &uiHeight);
            _pst_Params->x = -1.0f / (FLOAT)uiWidth;
            _pst_Params->y =  1.0f / (FLOAT)uiHeight;
            _pst_Params->z =  0.0f;
            _pst_Params->w =  0.0f;
            break;

        default:
            _pst_Params->z = 0.0f;
            _pst_Params->w = 0.0f;
            break;
    }
}

void XeLightShaftManager::GetLightShaftVolume(VECTOR4FLOAT* _pst_Volume)
{
    // Render:
    // xy -> Width and height of the first plane in view space
    // zw -> Width and height of the last plane in view space

    switch (m_ulMode)
    {
        case LightShaftMode_Render:
            XeComputePlaneSize(m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].fStartZ, 
                               m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].fCamHFOV,
                               m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].fCamVFOV,
                               &_pst_Volume->x, &_pst_Volume->y);
            XeComputePlaneSize(m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].fEndZ, 
                               m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].fCamHFOV,
                               m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].fCamVFOV,
                               &_pst_Volume->z, &_pst_Volume->w);
            break;

        default:
            _pst_Volume->x = _pst_Volume->y = _pst_Volume->z = _pst_Volume->w = 0.0f;
            break;
    }
}

void XeLightShaftManager::GetLightShaftMatrix(D3DXMATRIX* _pst_Matrix)
{
    *_pst_Matrix = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].mLightShaftMatrix;
}

void XeLightShaftManager::GetLightShaftNoiseMatrix(D3DXMATRIX* _pst_Matrix, ULONG _ul_NoiseIndex)
{
    D3DXMATRIX mOffset;
    FLOAT fNoiseU;
    FLOAT fNoiseV;

    if (_ul_NoiseIndex == 1)
    {
        fNoiseU = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].pLight->st_LightShaft.f_Noise2ScrollU;
        fNoiseV = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].pLight->st_LightShaft.f_Noise2ScrollV;
    }
    else
    {
        fNoiseU = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].pLight->st_LightShaft.f_Noise1ScrollU;
        fNoiseV = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].pLight->st_LightShaft.f_Noise1ScrollV;
    }

    D3DXMatrixTranslation(&mOffset, fmodf(fNoiseU * TIM_gf_MainClock, 1.0f), 
                                    fmodf(fNoiseV * TIM_gf_MainClock, 1.0f), 
                                    0.0f);

    D3DXMatrixMultiply(_pst_Matrix, &m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].mLightShaftMatrix, &mOffset);
}

void XeLightShaftManager::GetLightShaftInvWorldViewMatrix(D3DXMATRIX* _pst_Matrix)
{
    *_pst_Matrix = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].mInvWorldViewMatrix;
}

void XeLightShaftManager::GetLightShaftShadowMatrix(D3DXMATRIX* _pst_Matrix)
{
    *_pst_Matrix = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].mLightShaftShadowMatrix;
}

void XeLightShaftManager::GetLightShaftTexOffset(VECTOR4FLOAT* _pst_Offset, ULONG _ul_Index)
{
    _pst_Offset->x = m_avTexOffsets[_ul_Index].x;
    _pst_Offset->y = m_avTexOffsets[_ul_Index].y;
    _pst_Offset->z = m_avTexOffsets[_ul_Index].z;
    _pst_Offset->w = m_avTexOffsets[_ul_Index].w;
}

void XeLightShaftManager::GetLightShaftColor(VECTOR4FLOAT* _pst_Color)
{
    switch (m_ulMode)
    {
        case LightShaftMode_Blur:
            // xyz -> Light shaft color
            // w   -> 0
            XeJadeColorToV4F(_pst_Color, m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].pLight->st_LightShaft.ul_Color);
            _pst_Color->w = 0.0f;
            break;

        default:
            if (m_bRenderToWorkBuffer)
            {
                const FLOAT fColorBoost = 1.25f;

                // xyzw -> Intensity only
                _pst_Color->x = _pst_Color->y = _pst_Color->z = _pst_Color->w = fColorBoost * 4.0f / (FLOAT)m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].ulNbPlanes;
            }
            else
            {
                // xyz -> Color
                // w   -> Plane contribution (intensity)

                XeJadeColorToV4F(_pst_Color, m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].pLight->st_LightShaft.ul_Color);
                _pst_Color->w = 4.0f / (FLOAT)m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].ulNbPlanes;
            }
            break;
    }
}

void XeLightShaftManager::GetLightShaftAttenuation(VECTOR4FLOAT* _pst_Attenuation)
{
    // x -> Attenuation starting distance
    // y -> Attenuation factor (linear)
    // z -> FREE
    // w -> FREE

    _pst_Attenuation->x = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].pLight->st_LightShaft.f_AttenuationStart;
    _pst_Attenuation->y = m_oLightShaftStateRender.m_aLightShafts[m_oLightShaftStateRender.m_ulCurrentLightShaft].pLight->st_LightShaft.f_AttenuationFactor;
    _pst_Attenuation->z = 0.0f;
    _pst_Attenuation->w = 0.0f;
}
