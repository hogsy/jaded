// ------------------------------------------------------------------------------------------------
// File   : XeLightShaftManager.h
// Date   : 2005-03-08
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XELIGHTSHAFTMANAGER_H
#define GUARD_XELIGHTSHAFTMANAGER_H

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------
#define XE_LIGHTSHAFT_REGISTER_MAX 128
#define XE_LIGHTSHAFT_MAX          8

// ------------------------------------------------------------------------------------------------
// DEFINITIONS
// ------------------------------------------------------------------------------------------------
class XeRenderObject;

// ------------------------------------------------------------------------------------------------
// CLASSES
// ------------------------------------------------------------------------------------------------
class XeLightShaftManager
{
public:

    enum
    {
        // Plane method contants
        Plane_VertexFormat = XEVC_POSITION | XEVC_COLOR0,
        MaxPlanes          = 100,

        // Filter (Blur)
        Filter_VertexFormat = XEVC_POSITION | XEVC_COLOR0 | XEVC_TEXCOORD0,

        // Plane method textures
        Texture_Cookie = 0,
        Texture_Noise  = 1,
        Texture_Shadow = 2,
        Texture_Depth  = 3,

        MaxClipPlanes = 6,

        NbBlurPasses = 3,

        MaxFastLightShafts = 3,
        MaxWorkBuffers     = MaxFastLightShafts + 1,

        // Custom feature types
        Custom_LightShaftMode  = 1,
        Custom_NbNoiseTextures = 2,
        Custom_EnableShadow    = 3,
        Custom_EnableColor     = 4,
        Custom_CombineChannels = 5,
        Custom_SaturateColors  = 6,
        Custom_EnableFog       = 7,

        // Custom feature values
        LightShaftMode_Render   = 0,
        LightShaftMode_Apply    = 1,
        LightShaftMode_Blur     = 2,
        LightShaftMode_Resample = 3,
    };

private:

    struct PlaneVertex
    {
        D3DXVECTOR3 position;                       // XEVC_POSITION
        ULONG       color;                          // XEVC_COLOR0
    };

    struct FilterVertex
    {
        D3DXVECTOR3 position;                       // XEVC_POSITION
        ULONG       color;                          // XEVC_COLOR0
        D3DXVECTOR2 texCoord;                       // XEVC_TEXCOORD0
    };

    struct LightShaftInfo
    {
        XeMaterial* pMaterial;                      // Material used for rendering

        OBJ_tdst_GameObject* pGO;                   // Game object
        LIGHT_tdst_Light*    pLight;                // Attached light

        ULONG ulWorldMatrixIndex;                   // World matrix index
        ULONG ulWorldViewMatrixIndex;               // World view matrix index
        ULONG ulProjectionMatrixIndex;              // Projection matrix index

        D3DXMATRIX  mInvWorldViewMatrix;            // Inverse world view matrix
        D3DXMATRIX  mLightShaftPosMatrix;           // Light shaft's position matrix (view -> light shaft local)
        D3DXMATRIX  mLightShaftMatrix;              // Light shaft's projective matrix
                                                    // From view space to light shaft's projective space
        D3DXMATRIX  mLightShaftShadowMatrix;        // Light shaft's shadow projection matrix

        D3DXVECTOR3 avFrustum[8];                   // View space frustum

        D3DXVECTOR3 vLightPosition;                 // Position (world space)

        FLOAT fStartZ;                              // Volume starting Z (view space)
        FLOAT fEndZ;                                // Volume ending Z (view space)

        FLOAT fCamHFOV;                             // Camera horizontal field of view
        FLOAT fCamVFOV;                             // Camera vertical field of view

        D3DXPLANE vClipPlanes[MaxClipPlanes];       // Clipping planes

        ULONG ulNbPlanes;                           // Number of planes used for rendering
        ULONG ulActiveClipPlanes;                   // Active clipping planes
    };

    struct XeLightShaftState
    {
        LightShaftInfo     m_aLightShafts[XE_LIGHTSHAFT_MAX];
        ULONG              m_ulNbLightShafts;
        ULONG              m_ulCurrentLightShaft;
    };

public:

    XeLightShaftManager(void);
    ~XeLightShaftManager(void);

    void Initialize(void);
    void OnDeviceLost(void);
    void OnDeviceReset(void);
    void Shutdown(void);

    void GetUsedTextures(char* _pc_UsedIndex);
    void RegisterLightShaft(LIGHT_tdst_Light* _pst_Light);
    void UnregisterLightShaft(LIGHT_tdst_Light* _pst_Light);

    void ClearRequests(void);

    void Update(void);

    bool BeginRendering(ULONG _ulIndex);
    void EndRendering(void);

    void PreRender(void);

    void QueueLightShaftForRender(OBJ_tdst_GameObject* _pst_GO);

    void GetLightShaftPlaneParams(VECTOR4FLOAT* _pst_Params);
    void GetLightShaftVolume(VECTOR4FLOAT* _pst_Volume);
    void GetLightShaftMatrix(D3DXMATRIX* _pst_Matrix);
    void GetLightShaftNoiseMatrix(D3DXMATRIX* _pst_Matrix, ULONG _ul_NoiseIndex);
    void GetLightShaftInvWorldViewMatrix(D3DXMATRIX* _pst_Matrix);
    void GetLightShaftShadowMatrix(D3DXMATRIX* _pst_Matrix);
    void GetLightShaftTexOffset(VECTOR4FLOAT* _pst_Offset, ULONG _ul_Index);

    void GetLightShaftColor(VECTOR4FLOAT* _pst_Color);
    void GetLightShaftAttenuation(VECTOR4FLOAT* _pst_Attenuation);

    inline void BackupState() { L_memcpy(&m_oLightShaftStateRender, &m_oLightShaftStateEngine, sizeof(XeLightShaftState)); }

private:

    void InitializePlaneMesh(void);
    void InitializeFilterMesh(void);
    void InitializeWorkBuffers(void);

    void UpdateLightInformation(LightShaftInfo* _pst_Info);
    void ComputeLightShaftMatrix(LightShaftInfo* _pst_Info);

private:

    LIGHT_tdst_Light*  m_aRegisteredLightShafts[XE_LIGHTSHAFT_REGISTER_MAX];

    // context sensitive data
    XeLightShaftState  m_oLightShaftStateEngine;
    XeLightShaftState  m_oLightShaftStateRender;

    ULONG              m_ulMode;

    XeMesh*            m_pPlaneMesh;

    XeRenderObject*    m_pFilterRenderable;
    XeMesh*            m_pFilterMesh;

    XeMaterial*        m_pBlurMaterial;

    XeMaterial*        m_pResampleMaterial;

    XeMaterial*        m_pApplyMaterial[MaxFastLightShafts];

    XeRenderObject*    m_pRenderable;

    VECTOR4FLOAT       m_avTexOffsets[2];

    BOOL               m_bRenderToWorkBuffer;
    IDirect3DTexture9* m_pWorkBuffers[MaxWorkBuffers];
    UINT               m_uiWorkBufferIds[MaxWorkBuffers];

    DWORD              m_dwWorkBufferWidth;
    DWORD              m_dwWorkBufferHeight;

#if defined(_XENON)
    D3DTexture         m_stDepthBufferA8R8G8B8;
    D3DTexture         m_stDepthBufferD24S8;
    D3DTexture         m_stDepthBufferFullScaleA8R8G8B8;
    void*              m_pDepthBufferMemory;
    IDirect3DTexture9* m_pDepthBufferA8R8G8B8;
    IDirect3DTexture9* m_pDepthBufferD24S8;
    IDirect3DTexture9* m_pDepthBufferFullScaleA8R8G8B8;
    UINT               m_uiDepthBufferA8R8G8B8Id;
#else
    IDirect3DTexture9* m_pDepthBuffer;
#endif
    UINT               m_uiDepthBufferId;
};

// ------------------------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------------------------
extern XeLightShaftManager g_oXeLightShaftManager;

#endif // #ifdef GUARD_XELIGHTSHAFTMANAGER_H
