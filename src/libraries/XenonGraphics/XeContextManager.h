// ------------------------------------------------------------------------------------------------
// File   : XeContextManager.h
// Date   : 2005-09-13
// Author : Dany Joannette
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XECONTEXTMANAGER_H
#define GUARD_XECONTEXTMANAGER_H

#include <map>
#include "XeSharedDefines.h"
#include "XeUtils.h"

#define XERENDER_MAXLISTSIZE            4096
#define XERENDER_RENDEROBJECTPOOLSIZE   8192

// pool to handle dynamically create mesh without having to do memory allocations
#define MAX_DYNAMIC_MESH                4000

#define XERENDER_MIN_UI_RECTANGLE       256
#define XERENDER_MAX_UI_RECTS           64

#define Xe_MaxSPG2Spheres 8

typedef struct
{
    VECTOR4FLOAT		m_vSPG2UVManip;
    float				m_fSPG2Ratio;
    float				m_fSPG2OoNumOfSeg;
    VECTOR4FLOAT		m_vSPG2GlobalPos;
    VECTOR4FLOAT		m_vSPG2GlobalZAdd;
    float				m_fSPG2GlobalScale;
    VECTOR4FLOAT		m_avSPG2Wind[8];
    float				m_fSPG2TrapezeDelta;
    float				m_fSPG2Trapeze;
    float				m_fSPG2EOHP;
    float				m_fSPG2SpriteRadius;
    VECTOR4FLOAT		m_vSPG2XCam;
    VECTOR4FLOAT		m_vSPG2YCam;

} tdstSPG2ShaderConsts;

// ------------------------------------------------------------------------------------------------
// CLASSES
// ------------------------------------------------------------------------------------------------
class XeContextManager
{
public:

    FLOAT f_XeMipMapLODBias;
    BOOL  b_XeMaterialLODDetailEnable;
    FLOAT f_XeMaterialLODDetailNear;
    FLOAT f_XeMaterialLODDetailFar;
    BOOL  b_XeMaterialLODEnable;
    FLOAT f_XeMaterialLODNear;
    FLOAT f_XeMaterialLODFar;
    BOOL  b_DrawWideScreenBands;
    ULONG ul_DisplayFlags;
    FLOAT f_FieldOfVision;
    FLOAT f_FactorX;
    FLOAT f_NearPlane;
    FLOAT f_FarPlane;
    BOOL  b_AntiAliasingBlur;
    ULONG ul_XeDiffuseColor;
    ULONG ul_XeSpecularColor;
    ULONG ul_AmbientColor;
    FLOAT f_XeSpecularShiny;
    FLOAT f_XeSpecularStrength;
    FLOAT f_XeSaturation;
    MATH_tdst_Vector v_XeBrightness;
    FLOAT f_XeContrast;
    BOOL  b_EnableColorCorrection;

    MATH_tdst_Matrix    st_InverseCameraMatrix;
    MATH_tdst_Matrix    st_CameraMatrix;

    union DirtyFlags {
        struct {
            DWORD bWorldViewProj    :  1;
            DWORD bWorldView        :  1;
            DWORD bProjection       :  1;
            DWORD bSkinning         :  1;
            DWORD bDirLights        :  1;
            DWORD bOmniLights       :  1;
            DWORD bSpotLights       :  1;
            DWORD bCylSpotLights    :  1;
            DWORD bPerPixelLightIndex: 1;
            DWORD uReserved         : 23;
        };
        DWORD dwRawFlags;

        void Clear() { dwRawFlags = 0; }
        void DirtyAll() { dwRawFlags = 0xFFFFFFFF; }
    };

public:

    XeContextManager(void);
    ~XeContextManager(void);

    // general
    void ReInit();
    void Shutdown();
    inline void ClearMatrixStacks();

    // world
    inline void SetWorldMatrix(ULONG _ulIndex);
    inline ULONG GetCurrentWorldMatrixIndex() { return m_vecWorldStack.GetCount()-1; }
    inline D3DXMATRIX* GetWorldMatrixByIndex(ULONG _ulIndex);
    inline ULONG PushWorldMatrix(D3DXMATRIX *_poWorldMatrix);
    inline D3DXMATRIX * GetCurrentWorldMatrix() { return m_vecWorldStack.At( GetCurrentWorldMatrixIndex() ); }

    // world view
    inline void SetWorldViewMatrix(ULONG _ulIndex);
    inline ULONG GetCurrentWorldViewMatrixIndex() { return m_vecWorldViewStack.GetCount()-1; }
    inline ULONG GetCurrentWorldViewMatrixActiveIndex() { return m_ulCurrentWorldViewIndex; }
    inline D3DXMATRIX* GetWorldViewMatrixByIndex(ULONG _ulIndex);
    inline ULONG PushWorldViewMatrix(D3DXMATRIX *_poWorldViewMatrix);
    inline D3DXMATRIX* GetCurrentWorldViewMatrix() { return m_vecWorldViewStack.At( GetCurrentWorldViewMatrixIndex() ); }

    // projection
    inline void SetProjectionMatrix(ULONG _ulIndex);
    inline ULONG GetCurrentProjMatrixIndex() { return m_vecProjectionStack.GetCount()-1; } // index 0 means identity
    inline ULONG GetCurrentProjMatrixActiveIndex() { return m_ulCurrentProjIndex; }
    inline ULONG PushProjectionMatrix(const D3DXMATRIX *_poProjMatrix);
    inline D3DXMATRIX* GetProjectionMatrixByIndex(ULONG _ulIndex);
    inline D3DXMATRIX* GetCurrentProjectionMatrix() { return m_vecProjectionStack.At( GetCurrentProjMatrixIndex() ); }

    // gizmo
    inline void SetPlanarGizmoMatrix(ULONG _ulIndex);
    inline ULONG GetCurrentPlanarGizmoMatrixIndex() { return m_vecPlanarGizmoStack.GetCount()-1; } // index 0 means identity
    inline ULONG PushPlanarGizmoMatrix(const D3DXMATRIX *_poPlanarGizmoMatrix);
    inline D3DXMATRIX* GetPlanarGizmoMatrixByIndex(ULONG _ulIndex);
    inline D3DXMATRIX* GetCurrentPlanarGizmoMatrix() { return m_vecPlanarGizmoStack.At( GetCurrentPlanarGizmoMatrixIndex() ); }

    // skinning
    inline Float4x3 *   GetSkinningMatrixByIndex(ULONG _ulIndex) { return m_vecSkinningStack.At(_ulIndex); }
    inline ULONG        GetCurrentSkinningMatrixIndex() { return m_vecSkinningStack.GetCount()-1; };
    inline ULONG        PushSkinningMatrix(D3DXMATRIX *_poSkinningMatrix);
    inline void         BeginSkinning();
    inline void         EndSkinning();
    inline void         ClearSkinning();
    inline XeSkinningSet*  GetLastSkinningSet() { return &m_oLastSkinningSet; }
    inline XeSkinningSet*  GetCurrentSkinningSet() { return &m_oCurrentSkinningSet; }
    inline void         SetCurrentSkinningSet(XeSkinningSet *_pSkinningSet);
    inline ULONG        GetNextSkinningIndex() { return m_vecSkinningStack.GetCount(); }
    inline BOOL         GetSkinningMatrix(D3DXMATRIX & _matrix, XeSkinningSet *_poSkinningSet, INT _iIndex);

    // lighting
    inline void     ClearLightStacks();
    inline void     BeginAddLights();
    inline void     EndAddLights();
    inline XeLight* PushLight(ULONG _ulType, BOOL bAddToLightToSet = TRUE );
    inline bool     PushLight(ULONG _ulLightSet, ULONG _ulType, ULONG _ulLightIndex);
    inline ULONG    GetCurrentLightSetIndex(void) { return m_ulCurrentLightSet; }
    inline void     SetCurrentLightSet(ULONG _ulIndex);
    inline ULONG    GetLightSetLightCount(ULONG _ulIndex);
    inline ULONG    GetCurrentLightSetLightCount();
    inline INT      *GetCurrentLightSetLightCountArray();
    inline ULONG    GetCurrentLightSetDirLightCount();
    inline ULONG    GetCurrentLightSetOmniLightCount();
    inline ULONG    GetCurrentLightSetSpotLightCount();
    inline ULONG    GetCurrentLightSetCylSpotLightCount();
    inline ULONG    GetNextDirLightIndex() { return m_vecDirLightStack.GetCount(); }
    inline ULONG    GetNextOmniLightIndex() { return m_vecOmniLightStack.GetCount(); }
    inline ULONG    GetNextSpotLightIndex() { return m_vecSpotLightStack.GetCount(); }
    inline ULONG    GetNextCylSpotLightIndex() { return m_vecCylSpotLightStack.GetCount(); }
    XeLight*        GetLight(ULONG _ulLightSet, INT _iIndex, ULONG *_pulType);
    inline void     ResetLastLightSet(void) { m_ulCurrentLightSet = 0; }
    inline void     SetPerPixelLightIndex(INT _iLight, INT _iIndex);
    inline INT      GetCurrentPerPixelLightIndex(INT _iLight) { return m_aiPerPixelLightIndex[_iLight]; }
    inline INT      *GetCurrentPerPixelLightArray() { return m_aiPerPixelLightIndex; }
    inline XeLight* GetCurrentPerPixelLight(INT _iLight, ULONG *_pulType) { return GetLight(m_ulCurrentLightSet, m_aiPerPixelLightIndex[_iLight], _pulType); }
    inline XeLight* GetCurrentLightSetDirLight(INT _index) { return m_vecDirLightStack.At( m_aoLightSets[m_ulCurrentLightSet].ulDirLightIndices[_index] ); }
    inline XeLight* GetCurrentLightSetOmniLight(INT _index) { return m_vecOmniLightStack.At( m_aoLightSets[m_ulCurrentLightSet].ulOmniLightIndices[_index] ); }
    inline XeLight* GetCurrentLightSetSpotLight(INT _index) { return m_vecSpotLightStack.At( m_aoLightSets[m_ulCurrentLightSet].ulSpotLightIndices[_index] ); }
    inline XeLight* GetCurrentLightSetCylSpotLight(INT _index) { return m_vecCylSpotLightStack.At( m_aoLightSets[m_ulCurrentLightSet].ulCylSpotLightIndices[_index] ); }
    inline void     ResetLastActiveLightSet() { m_ulLastActiveLightSet = m_ulCurrentLightSet; } 
    inline XeLightSet * GetCurrentLightSet() { return &m_aoLightSets[m_ulCurrentLightSet]; }
    
    // spg2
    inline tdstSPG2ShaderConsts GetSPG2ShadersConsts() { return m_stSPG2ShaderConsts; }
    inline void		SetSPG2ShadersConsts(tdstSPG2ShaderConsts _stConsts) { m_stSPG2ShaderConsts = _stConsts; }
    inline void     SetSPG2TexUVManip(VECTOR4FLOAT _vUVManip) { m_stSPG2ShaderConsts.m_vSPG2UVManip = _vUVManip; }
    inline void     SetSPG2Ratio(float _fRatio) { m_stSPG2ShaderConsts.m_fSPG2Ratio = _fRatio; }
    inline void     SetSPG2OoNumOfSeg(float _fOoNumOfSeg) { m_stSPG2ShaderConsts.m_fSPG2OoNumOfSeg = _fOoNumOfSeg; }
    inline void     SetSPG2GlobalPos(VECTOR4FLOAT _vGlobalPos) { m_stSPG2ShaderConsts.m_vSPG2GlobalPos = _vGlobalPos; }
    inline void     SetSPG2GlobalZAdd(VECTOR4FLOAT _vGlobalZAdd) { m_stSPG2ShaderConsts.m_vSPG2GlobalZAdd = _vGlobalZAdd; }
    inline void     SetSPG2GlobalScale(float _fGlobalScale) { m_stSPG2ShaderConsts.m_fSPG2GlobalScale = _fGlobalScale; }
    inline void     SetSPG2Wind(VECTOR4FLOAT *_avWind) { memcpy(&m_stSPG2ShaderConsts.m_avSPG2Wind[0], _avWind, 8*sizeof(VECTOR4FLOAT)); }
    inline void     SetSPG2TrapezeDelta(float _fTrapezeDelta) { m_stSPG2ShaderConsts.m_fSPG2TrapezeDelta = _fTrapezeDelta;}
    inline void     SetSPG2Trapeze(float _fTrapeze) { m_stSPG2ShaderConsts.m_fSPG2Trapeze = _fTrapeze;}
    inline void     SetSPG2EOHP(float _fEOHP) { m_stSPG2ShaderConsts.m_fSPG2EOHP = _fEOHP;}
    inline void     SetSPG2SpriteRadius(float _fSpriteRadius) { m_stSPG2ShaderConsts.m_fSPG2SpriteRadius = _fSpriteRadius;}
    inline void     SetSPG2XCam(VECTOR4FLOAT _vXCam) { m_stSPG2ShaderConsts.m_vSPG2XCam = _vXCam; }
    inline void     SetSPG2YCam(VECTOR4FLOAT _vYCam) { m_stSPG2ShaderConsts.m_vSPG2YCam = _vYCam; }

    // Wave Your Body
    inline void     ClearWYB(void) { m_vecWYBParamsStack.Clear(); }
    inline XeWYBParams* AddWYBParams(void);
    inline int      GetCurrentWYBParamsIndex(void) { return m_vecWYBParamsStack.GetCount(); }
    inline XeWYBParams* GetWYBParams(ULONG _iIdx);

    // Symmetry
    inline void              ClearSymmetry(void)                 { m_vecSymmetryParamsStack.Clear(); }
    inline XeSymmetryParams* AddSymmetryParams(void);
    inline ULONG             GetCurrentSymmetryParamsIndex(void) { return m_vecSymmetryParamsStack.GetCount(); }
    inline XeSymmetryParams* GetSymmetryParams(ULONG _ulIndex);

    // FUR
    inline VECTOR4FLOAT     *GetFURShaderConsts() { return &m_stFurShaderConsts; }
    inline void              SetFURShaderConsts(VECTOR4FLOAT *_pvParams) { m_stFurShaderConsts = *_pvParams; }

    // heat shimmer
    void   SetHeatShimmerConsts( const tdstHeatShimmerVSConsts & _stHeatShimmerConsts ) { m_stHeatShimmerConsts = _stHeatShimmerConsts; }  
    const  tdstHeatShimmerVSConsts & GetHeatShimmerConsts( ) { return m_stHeatShimmerConsts; } 
    inline tdstHeatShimmerVSConsts * GetHeatShimmerConstsPointer( ) { return &m_stHeatShimmerConsts; } 

    // render objects
    inline void              ClearRenderLists();
    inline UINT              GetTotalNumberOfRenderObjects() { return m_nbRenderObjects; }
    inline XeRenderObject   *GetCurrentRenderObject(void) { return m_pCurrentRenderObject; }
    inline void              SetCurrentRenderObject(XeRenderObject* _pRO) { m_pCurrentRenderObject = _pRO; }
    inline int               GetNbRenderObjectOfList(eXeRENDERLISTTYPE _list) { return m_nbRenderObjectsList[_list]; }
    inline XeRenderObject  **GetRenderObjectList(eXeRENDERLISTTYPE _list) { return &m_apRenderObjectLists[_list][0]; }
    inline XeRenderObject   *GetLastRenderObjectInList(eXeRENDERLISTTYPE _list) { return m_apRenderObjectLists[_list][m_nbRenderObjectsList[_list]-1]; }
    inline void              ResetRenderObjectOfList(eXeRENDERLISTTYPE _list) { m_nbRenderObjectsList[_list] = 0; }
    XeRenderObject*          PushRenderObject(eXeRENDERLISTTYPE _list);
    void                     AddRenderObjectToList(eXeRENDERLISTTYPE _list, XeRenderObject* _pRO);

    // dynamic mesh
    inline void              ResetDynMesh() { m_ulCurrentDynMesh = 0; }
    inline UINT              GetNbDynMesh() { return m_ulCurrentDynMesh; }
    XeMesh*                  PushDynMesh();

    // UI Rects
    inline UINT              GetNbUIRects() { return m_avUIRectanglePool.size(); }
    inline void              ResetUIRects() { m_avUIRectanglePool.resize(0); }
    inline void              PushUIRects(XeUIRectangle &_oRect)  { m_avUIRectanglePool.push_back(_oRect); }
    inline XeUIRectangle*    GetUIRect(int _iIdx) { return &m_avUIRectanglePool[_iIdx]; }

    // RLI scale/offset
    void                     SetGlobalRLIScaleAndOffset(FLOAT _fScale, FLOAT _fOffset) {m_fGlobalRLIScale = _fScale; m_fGlobalRLIOffset = _fOffset; }
    void                     GetGlobalRLIScaleAndOffset(FLOAT* _pfScale, FLOAT* _pfOffset) {*_pfScale = m_fGlobalRLIScale; *_pfOffset = m_fGlobalRLIOffset; }

    // fog
    inline BOOL              IsFogEnabled() { return m_bFogEnabled; }
    inline void              SetVertexFogParams( BOOL _bEnable, D3DXCOLOR _stColor, const VECTOR4FLOAT & _vFogParams, const VECTOR4FLOAT & _vFogPitchAttenuation );
    inline void              SetPixelFogParams( BOOL _bEnable, D3DXCOLOR _stColor, const VECTOR4FLOAT & _vFogParams, const VECTOR4FLOAT & _vFogPitchAttenuation );

    inline VECTOR4FLOAT*     GetVertexFogParams() { return &m_oVertexFogParams; }
    inline VECTOR4FLOAT*     GetPixelFogParams() { return &m_oPixelFogParams; }

    inline VECTOR4FLOAT*     GetVertexFogPitchAttenuation() { return &m_oVertexFogPitchAttenuation; }
    inline VECTOR4FLOAT*     GetPixelFogPitchAttenuation() { return &m_oPixelFogPitchAttenuation; }

    inline D3DXCOLOR*        GetFogColor() { return &m_stFogColor; }
    inline void              ResetFogParams();

    // misc
    inline void              GetDirtyFlags(DirtyFlags &_oDirtyFlags) { _oDirtyFlags.dwRawFlags = m_oDirtyFlags.dwRawFlags; }
    inline void              ClearDirtyFlags() { m_oDirtyFlags.Clear(); }
    inline void              DirtyAllConstants() { m_oDirtyFlags.DirtyAll(); }

    // camera
    inline D3DXMATRIX*       GetCameraMatrix() { return &m_oCameraMatrix; }
    inline void              SetCameraParams(FLOAT _fNear, FLOAT _fFar, FLOAT _fFOVX, FLOAT _fFOVY);
    inline void              GetCameraParams(FLOAT* _pfNear, FLOAT* _pfFar, FLOAT* _fFOVX, FLOAT* _fFOVY);
    inline FLOAT*            GetCameraParams() { return m_afCameraParams; }

    // spg2
    float				     Xe_SPG2_SphereRadius[Xe_MaxSPG2Spheres];
    MATH_tdst_Vector	     Xe_SPG2_SphereCenters[Xe_MaxSPG2Spheres];
    ULONG                    Xe_SPG2_ulNumberOfSpheres;

    // draw at end
    void                     SetDoublePassZOverwrite(BOOL _bSet) { m_bDoublePassZOverwrite = _bSet; }
    BOOL                     GetDoublePassZOverwrite() { return m_bDoublePassZOverwrite; }

private:

    // structs
    typedef XeGrowableArray< D3DXMATRIX >       MatrixStack;
    typedef XeGrowableArray< Float4x3 >         SkinningMatrixStack;
    typedef XeGrowableArray< XeLight >          XeLightStack;
    typedef XeGrowableArray<XeWYBParams>        WYBParameterStack;
    typedef XeGrowableArray<XeSymmetryParams>   XeSymmetryStack;
    typedef std::vector<XeUIRectangle>          UIRectangleArray;

    // world
    MatrixStack             m_vecWorldStack;
    ULONG                   m_ulCurrentWorldIndex;

    // world view
    MatrixStack             m_vecWorldViewStack;
    ULONG                   m_ulCurrentWorldViewIndex;

    // projection
    MatrixStack             m_vecProjectionStack;
    ULONG                   m_ulCurrentProjIndex;

    // gizmo mapping
    MatrixStack             m_vecPlanarGizmoStack;
    ULONG                   m_ulCurrentPlanarGizmoIndex;

    // skinning
    SkinningMatrixStack     m_vecSkinningStack;
    ULONG                   m_ulCurrentSkinningIndex;
    XeSkinningSet           m_oCurrentSkinningSet;
    XeSkinningSet           m_oLastSkinningSet;

    // lights
    XeLightStack            m_vecDirLightStack;
    XeLightStack            m_vecOmniLightStack;
    XeLightStack            m_vecSpotLightStack;
    XeLightStack            m_vecCylSpotLightStack;
    XeLightSet              m_aoLightSets[VS_MAX_LIGHTSET_STACK_SIZE];
    ULONG                   m_ulCurrentLightSet;
    ULONG                   m_ulLastActiveLightSet;
    ULONG                   m_ulNbLightSets;
    INT                     m_aiPerPixelLightIndex[VS_MAX_LIGHTS_PER_PASS];

    // spg2 consts
    tdstSPG2ShaderConsts    m_stSPG2ShaderConsts;

    // wave your body
    WYBParameterStack       m_vecWYBParamsStack;
    ULONG                   m_ulNumWYB;

    // symmetry
    XeSymmetryStack         m_vecSymmetryParamsStack;

    // fur consts
    VECTOR4FLOAT    		m_stFurShaderConsts;
    FLOAT                   m_fFurOffsetScale;

    // heat shimmer consts
    tdstHeatShimmerVSConsts m_stHeatShimmerConsts;

    // render object pool    
    int                     m_nbRenderObjects;
    XeRenderObject         *m_arRenderObjects;

    // The various lists where elements are stored according to their properties.
    // the list are now static for performance issue
    // most lists do not need that many object... TODO : optimize memory (if needed)
    XeRenderObject*   	    m_apRenderObjectLists[XeRT_NUMTYPES][XERENDER_MAXLISTSIZE];
    int                     m_nbRenderObjectsList[XeRT_NUMTYPES];
    XeRenderObject*         m_pCurrentRenderObject;

    // Dynamic mesh pool
    XeMesh                 *m_apoDynamicMeshPool;
    ULONG                   m_ulCurrentDynMesh;

    // UI Rects
    UIRectangleArray        m_avUIRectanglePool;

    // RLI scale/offset
    FLOAT                   m_fGlobalRLIScale;
    FLOAT                   m_fGlobalRLIOffset;

    // Fog
    BOOL                    m_bFogEnabled;
    D3DXCOLOR               m_stFogColor;
    VECTOR4FLOAT            m_oVertexFogParams;
    VECTOR4FLOAT            m_oVertexFogPitchAttenuation;
    VECTOR4FLOAT            m_oPixelFogParams;
    VECTOR4FLOAT            m_oPixelFogPitchAttenuation;

    // misc
    DirtyFlags              m_oDirtyFlags;

    // Camera
    D3DXMATRIX              m_oCameraMatrix;
    FLOAT                   m_afCameraParams[4];                    // Near, Far, FOV X, FOV Y

    // draw at end
    BOOL                    m_bDoublePassZOverwrite;
};

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::ClearMatrixStacks()
{
    m_vecWorldStack.SetCount(1);
    m_vecWorldViewStack.SetCount(1);
    m_vecProjectionStack.SetCount(1);
    m_vecPlanarGizmoStack.SetCount(1);

    m_vecSkinningStack.Clear();
}


// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::SetWorldMatrix(ULONG _ulIndex)
{
    m_ulCurrentWorldIndex = _ulIndex;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
ULONG XeContextManager::PushWorldMatrix(D3DXMATRIX *_poWorldMatrix)
{
    D3DXMATRIX * pNewMatrix = m_vecWorldStack.Push();
    ERR_X_Assert(pNewMatrix && _poWorldMatrix);
    *pNewMatrix = *_poWorldMatrix;
    return GetCurrentWorldMatrixIndex();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
D3DXMATRIX* XeContextManager::GetWorldMatrixByIndex(ULONG _ulIndex)
{
    return m_vecWorldStack.At(_ulIndex);
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::SetWorldViewMatrix(ULONG _ulIndex)
{
    m_ulCurrentWorldViewIndex = _ulIndex;
    m_oDirtyFlags.bWorldView = TRUE;
    m_oDirtyFlags.bWorldViewProj = TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
ULONG XeContextManager::PushWorldViewMatrix(D3DXMATRIX *_poWorldViewMatrix)
{
    D3DXMATRIX * pNewMatrix = m_vecWorldViewStack.Push();
    ERR_X_Assert(pNewMatrix && _poWorldViewMatrix);
    *pNewMatrix = *_poWorldViewMatrix;
    return GetCurrentWorldViewMatrixIndex();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
D3DXMATRIX* XeContextManager::GetWorldViewMatrixByIndex(ULONG _ulIndex)
{
    return m_vecWorldViewStack.At(_ulIndex);
}


// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::SetProjectionMatrix(ULONG _ulIndex)
{
    m_ulCurrentProjIndex = _ulIndex;
    m_oDirtyFlags.bProjection = TRUE;
    m_oDirtyFlags.bWorldViewProj = TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
ULONG XeContextManager::PushProjectionMatrix(const D3DXMATRIX *_poProjMatrix)
{
    D3DXMATRIX * pNewMatrix = m_vecProjectionStack.Push();
    ERR_X_Assert(pNewMatrix && _poProjMatrix);
    *pNewMatrix = *_poProjMatrix;
    return GetCurrentProjMatrixIndex();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
D3DXMATRIX* XeContextManager::GetProjectionMatrixByIndex(ULONG _ulIndex)
{
    return m_vecProjectionStack.At(_ulIndex);
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::SetPlanarGizmoMatrix(ULONG _ulIndex)
{
    m_ulCurrentPlanarGizmoIndex = _ulIndex;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
ULONG XeContextManager::PushPlanarGizmoMatrix(const D3DXMATRIX *_poPlanarGizmoMatrix)
{
    D3DXMATRIX * pNewMatrix = m_vecPlanarGizmoStack.Push();
    ERR_X_Assert(pNewMatrix && _poPlanarGizmoMatrix);
    *pNewMatrix = *_poPlanarGizmoMatrix;
    return GetCurrentPlanarGizmoMatrixIndex();

}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
D3DXMATRIX* XeContextManager::GetPlanarGizmoMatrixByIndex(ULONG _ulIndex)
{
    return m_vecPlanarGizmoStack.At(_ulIndex);
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::BeginSkinning()
{
    m_oLastSkinningSet.ulSkinningMatrixIndex  = GetNextSkinningIndex();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::EndSkinning()
{
    m_oLastSkinningSet.ulSkinningMatrixCount = GetNextSkinningIndex() - m_oLastSkinningSet.ulSkinningMatrixIndex;
    ERR_X_Assert(m_oLastSkinningSet.ulSkinningMatrixCount <= VS_MAX_BONES);
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::ClearSkinning()
{
    m_oLastSkinningSet.ulSkinningMatrixCount = 0;
    m_oLastSkinningSet.ulSkinningMatrixIndex = GetNextSkinningIndex();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
ULONG XeContextManager::PushSkinningMatrix(D3DXMATRIX *_poSkinningMatrix)
{
    Float4x3 * pNewMatrix = m_vecSkinningStack.Push();
    ERR_X_Assert(pNewMatrix && _poSkinningMatrix);
    XeConvertToFloat4x3( *pNewMatrix, *_poSkinningMatrix);
    return GetCurrentSkinningMatrixIndex();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::SetCurrentSkinningSet(XeSkinningSet *_pNewSkinningSet)
{
    ERR_X_Assert(_pNewSkinningSet != NULL);
    m_oCurrentSkinningSet = *_pNewSkinningSet;
    m_oDirtyFlags.bSkinning = TRUE;
}

BOOL XeContextManager::GetSkinningMatrix( D3DXMATRIX & _matrix, XeSkinningSet *_poSkinningSet, INT _iIndex)
{
    ERR_X_Assert((_poSkinningSet != NULL) && (_iIndex >= 0) && (_poSkinningSet->ulSkinningMatrixIndex + _iIndex < m_vecSkinningStack.GetCount()))
        if (_poSkinningSet != NULL)
        {
            Float4x3 * pMatrix = m_vecSkinningStack.At(_poSkinningSet->ulSkinningMatrixIndex + _iIndex);
            _matrix._11 = pMatrix->m_vColumns[0].x;
            _matrix._21 = pMatrix->m_vColumns[0].y;
            _matrix._31 = pMatrix->m_vColumns[0].z;
            _matrix._41 = pMatrix->m_vColumns[0].w;

            _matrix._12 = pMatrix->m_vColumns[1].x;
            _matrix._22 = pMatrix->m_vColumns[1].y;
            _matrix._32 = pMatrix->m_vColumns[1].z;
            _matrix._42 = pMatrix->m_vColumns[1].w;

            _matrix._13 = pMatrix->m_vColumns[2].x;
            _matrix._23 = pMatrix->m_vColumns[2].y;
            _matrix._33 = pMatrix->m_vColumns[2].z;
            _matrix._43 = pMatrix->m_vColumns[2].w;

            _matrix._14 = 0.0f;
            _matrix._24 = 0.0f;
            _matrix._34 = 0.0f;
            _matrix._44 = 1.0f;
            return TRUE;
        }
        return FALSE;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::ClearLightStacks()
{
    // Clear stacks
    m_vecDirLightStack.Clear();
    m_vecOmniLightStack.Clear();
    m_vecSpotLightStack.Clear();
    m_vecCylSpotLightStack.Clear();

    // Clear light sets
    m_ulCurrentLightSet = 0;
    m_ulNbLightSets     = 1;
    m_aoLightSets[m_ulCurrentLightSet].Clear();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::SetCurrentLightSet(ULONG _ulIndex )
{
    ERR_X_Assert(_ulIndex < m_ulNbLightSets);

    m_ulCurrentLightSet = _ulIndex;

    if((m_ulCurrentLightSet != m_ulLastActiveLightSet) && (m_ulCurrentLightSet != 0) )
    {
        // dirty all for now... to be optimized
        m_oDirtyFlags.bDirLights = TRUE;
        m_oDirtyFlags.bOmniLights = TRUE;
        m_oDirtyFlags.bSpotLights = TRUE;
        m_oDirtyFlags.bCylSpotLights = TRUE;
        m_oDirtyFlags.bPerPixelLightIndex = TRUE;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeLight* XeContextManager::PushLight(ULONG _ulType, BOOL bAddToLightToSet ) 
{ 
    ULONG ulCurrentIndex;
    XeLight * pLight = NULL;

    if (_ulType == LIGHT_TYPE_DIRECT)
    {
        ulCurrentIndex = m_vecDirLightStack.GetCount();
        pLight = m_vecDirLightStack.Push();

        if( bAddToLightToSet && m_aoLightSets[m_ulCurrentLightSet].ulDirLightCount < VS_MAX_LIGHTS )
        {
            m_aoLightSets[m_ulCurrentLightSet].ulDirLightIndices[m_aoLightSets[m_ulCurrentLightSet].ulDirLightCount++] = ulCurrentIndex;
        }
    }
    else if (_ulType == LIGHT_TYPE_OMNI)
    {
        ulCurrentIndex = m_vecOmniLightStack.GetCount();
        pLight = m_vecOmniLightStack.Push();

        if( bAddToLightToSet && m_aoLightSets[m_ulCurrentLightSet].ulOmniLightCount < VS_MAX_LIGHTS )
        {
            m_aoLightSets[m_ulCurrentLightSet].ulOmniLightIndices[m_aoLightSets[m_ulCurrentLightSet].ulOmniLightCount++] = ulCurrentIndex;
        }
    }
    else if (_ulType == LIGHT_TYPE_SPOT)
    {
        ulCurrentIndex = m_vecSpotLightStack.GetCount();
        pLight = m_vecSpotLightStack.Push();

        if( bAddToLightToSet && m_aoLightSets[m_ulCurrentLightSet].ulSpotLightCount < VS_MAX_LIGHTS )
        {
            m_aoLightSets[m_ulCurrentLightSet].ulSpotLightIndices[m_aoLightSets[m_ulCurrentLightSet].ulSpotLightCount++] = ulCurrentIndex;
        }
    }
    else if (_ulType == LIGHT_TYPE_CYLINDRICAL_SPOT)
    {
        ulCurrentIndex = m_vecCylSpotLightStack.GetCount();
        pLight = m_vecCylSpotLightStack.Push();

        if( bAddToLightToSet && m_aoLightSets[m_ulCurrentLightSet].ulCylSpotLightCount < VS_MAX_LIGHTS )
        {
            m_aoLightSets[m_ulCurrentLightSet].ulCylSpotLightIndices[m_aoLightSets[m_ulCurrentLightSet].ulCylSpotLightCount++] = ulCurrentIndex;
        }
    }    

    ERR_X_Assert( pLight != NULL );

    return pLight;
}

inline bool XeContextManager::PushLight(ULONG _ulLightSet, ULONG _ulType, ULONG _ulLightIndex)
{
    ERR_X_Assert(_ulLightSet < m_ulNbLightSets);

    // Do not allow writing to the first light set (always empty)
    if (_ulLightSet == 0)
        return false;

    if (_ulType == LIGHT_TYPE_DIRECT)
    {
        if (m_aoLightSets[_ulLightSet].ulDirLightCount < VS_MAX_LIGHTS)
        {
            m_aoLightSets[_ulLightSet].ulDirLightIndices[m_aoLightSets[_ulLightSet].ulDirLightCount++] = _ulLightIndex;
            return true;
        }
    }
    else if (_ulType == LIGHT_TYPE_OMNI)
    {
        if (m_aoLightSets[_ulLightSet].ulOmniLightCount < VS_MAX_LIGHTS)
        {
            m_aoLightSets[_ulLightSet].ulOmniLightIndices[m_aoLightSets[_ulLightSet].ulOmniLightCount++] = _ulLightIndex;
            return true;
        }
    }
    else if (_ulType == LIGHT_TYPE_SPOT)
    {
        if (m_aoLightSets[_ulLightSet].ulSpotLightCount < VS_MAX_LIGHTS)
        {
            m_aoLightSets[_ulLightSet].ulSpotLightIndices[m_aoLightSets[_ulLightSet].ulSpotLightCount++] = _ulLightIndex;
            return true;
        }
    }
    else if (_ulType == LIGHT_TYPE_CYLINDRICAL_SPOT)
    {
        if (m_aoLightSets[_ulLightSet].ulCylSpotLightCount < VS_MAX_LIGHTS)
        {
            m_aoLightSets[_ulLightSet].ulCylSpotLightIndices[m_aoLightSets[_ulLightSet].ulCylSpotLightCount++] = _ulLightIndex;
            return true;
        }
    }

    return false;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::BeginAddLights()
{
#if defined(_DEBUG)
    if (m_ulNbLightSets == VS_MAX_LIGHTSET_STACK_SIZE)
    {
        ERR_OutputDebugString("[Xenon] Light set stack size is too small, lighting information will be discarded\n");
        return;
    }
#endif

    m_ulCurrentLightSet = m_ulNbLightSets;
    ++m_ulNbLightSets;

    m_aoLightSets[m_ulCurrentLightSet].Clear();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::EndAddLights()
{
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline ULONG XeContextManager::GetLightSetLightCount(ULONG _ulIndex)
{
    ERR_X_Assert(_ulIndex < m_ulNbLightSets);

    return m_aoLightSets[_ulIndex].GetTotalLightCount();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline ULONG XeContextManager::GetCurrentLightSetLightCount()
{
    return m_aoLightSets[m_ulCurrentLightSet].GetTotalLightCount();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline INT *XeContextManager::GetCurrentLightSetLightCountArray()
{
    return m_aoLightSets[m_ulCurrentLightSet].GetLightCountArray();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline ULONG XeContextManager::GetCurrentLightSetDirLightCount()
{
    return m_aoLightSets[m_ulCurrentLightSet].GetDirLightCount();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline ULONG XeContextManager::GetCurrentLightSetOmniLightCount()
{
    return m_aoLightSets[m_ulCurrentLightSet].GetOmniLightCount();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline ULONG XeContextManager::GetCurrentLightSetSpotLightCount()
{
    return m_aoLightSets[m_ulCurrentLightSet].GetSpotLightCount();
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline ULONG XeContextManager::GetCurrentLightSetCylSpotLightCount()
{
    return m_aoLightSets[m_ulCurrentLightSet].GetCylSpotLightCount();
}

// ------------------------------------------------------------------------------------------------
// Name   : XeContextManager::SetPerPixelLightIndex
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeContextManager::SetPerPixelLightIndex(INT _iLight, INT _iIndex)
{
    if ((_iIndex < VS_MAX_LIGHTS) || (_iLight < VS_MAX_LIGHTS_PER_PASS))
    {
        if (m_aiPerPixelLightIndex[_iLight] != _iIndex)
        {
            m_aiPerPixelLightIndex[_iLight] = _iIndex;
            m_oDirtyFlags.bPerPixelLightIndex = TRUE;
        }
    }
    else
    {
        ERR_OutputDebugString("[Xenon] - invalid light index, light will be ignored ...\n");
    }
}

inline XeWYBParams* XeContextManager::AddWYBParams(void)
{
    ERR_X_Assert(m_ulNumWYB < VS_MAX_WYB_STACK_SIZE);

    XeWYBParams* pParams = NULL;

    if(m_vecWYBParamsStack.GetCount() < VS_MAX_WYB_STACK_SIZE)
    {
        pParams = m_vecWYBParamsStack.Push();
    }

    return pParams;

}
inline XeWYBParams* XeContextManager::GetWYBParams(ULONG _ulIdx)
{
    if(_ulIdx < m_vecWYBParamsStack.GetCount())
    {
        return m_vecWYBParamsStack.At(_ulIdx);
    }

    return NULL;
}

inline XeSymmetryParams* XeContextManager::AddSymmetryParams(void)
{
    XeSymmetryParams* pParams = NULL;

    if (m_vecSymmetryParamsStack.GetCount() < VS_MAX_SYMMETRY_STACK_SIZE)
    {
        pParams = m_vecSymmetryParamsStack.Push();
    }

    return pParams;
}

inline XeSymmetryParams* XeContextManager::GetSymmetryParams(ULONG _ulIndex)
{
    if (_ulIndex < m_vecSymmetryParamsStack.GetCount())
    {
        return m_vecSymmetryParamsStack.At(_ulIndex);
    }

    return NULL;
}

inline void XeContextManager::ClearRenderLists()
{
    // Clear the renderable object lists
    for(UINT uiListIdx = XeRT_FIRSTLIST; uiListIdx < XeRT_NUMTYPES; uiListIdx++)
    {
        m_nbRenderObjectsList[uiListIdx] = 0;
    }

    m_nbRenderObjects = 0;
}

void XeContextManager::SetVertexFogParams( BOOL _bEnable, D3DXCOLOR _stColor, const VECTOR4FLOAT & _vFogParams, const VECTOR4FLOAT & _vFogPitchAttenuation )
{
    m_stFogColor = _stColor;
    m_oVertexFogParams = _vFogParams;
    m_oVertexFogPitchAttenuation = _vFogPitchAttenuation;
    m_bFogEnabled = _bEnable;
}

void XeContextManager::SetPixelFogParams( BOOL _bEnable, D3DXCOLOR _stColor, const VECTOR4FLOAT & _vFogParams, const VECTOR4FLOAT & _vFogPitchAttenuation )
{
    m_stFogColor = _stColor;
    m_oPixelFogParams = _vFogParams;
    m_oPixelFogPitchAttenuation = _vFogPitchAttenuation;
    m_bFogEnabled = _bEnable;
}

inline void XeContextManager::SetCameraParams(FLOAT _fNear, FLOAT _fFar, FLOAT _fFOVX, FLOAT _fFOVY)
{
    m_afCameraParams[0] = _fNear;
    m_afCameraParams[1] = _fFar;
    m_afCameraParams[2] = _fFOVX;
    m_afCameraParams[3] = _fFOVY;
}

inline void XeContextManager::GetCameraParams(FLOAT* _pfNear, FLOAT* _pfFar, FLOAT* _fFOVX, FLOAT* _fFOVY)
{
    *_pfNear = m_afCameraParams[0];
    *_pfFar  = m_afCameraParams[1];
    *_fFOVX  = m_afCameraParams[2];
    *_fFOVY  = m_afCameraParams[3];
}
inline void XeContextManager::ResetFogParams() 
{
    memset( &m_oVertexFogParams, 0, sizeof( VECTOR4FLOAT) ); 
    memset( &m_oVertexFogPitchAttenuation, 0, sizeof( VECTOR4FLOAT) ); 
    memset( &m_oPixelFogParams, 0, sizeof( VECTOR4FLOAT) ); 
    memset( &m_oPixelFogPitchAttenuation, 0, sizeof( VECTOR4FLOAT) ); 
}

// ------------------------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------------------------
extern XeContextManager g_oXeContextManager1;
extern XeContextManager g_oXeContextManager2;
extern XeContextManager *g_pXeContextManagerEngine;
extern XeContextManager *g_pXeContextManagerRender;


#endif // #ifdef GUARD_XECONTEXTMANAGER_H
