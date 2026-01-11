
#ifndef WATER_MANAGER_HEADER
#define WATER_MANAGER_HEADER

#include "XeRenderer.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_WATER3D.h"

#define WM_MAX_LIGHTS_PER_PASS     4
#define XE_WATERPATCH_REGISTER_MAX 1024
#define XE_MAX_PATCHSET_STACK_SIZE 128
#define XE_MAX_PATCH_ID_LIST       1024

class XeMaterial;

class XeWaterManager
{
public:
    struct XeWaterPatchInfo {
        
        // values set a creation time
        BOOL         bUpdated;
        BOOL         bReflectionEnabled;
        BOOL         bRefractionEnabled;
        FLOAT        fRefractionFactor;
        FLOAT        fReflectionFactor;
        XeMesh       *poMesh;
        FLOAT        fWaterDensity;
        FLOAT        fBaseMapOpacity;
        FLOAT        fMossMapOpacity;
        FLOAT        fFogIntensity;
        
        // values computed per-frame
        VECTOR4FLOAT oPlaneEquation;
        VECTOR4FLOAT oPlaneOrigin;
        D3DXVECTOR4  oViewSpacePlaneEquation;
        D3DXVECTOR4  oViewSpacePlaneOrigin;
        D3DXVECTOR4  oClipSpacePlaneEquation;
        FLOAT        fRefractionIntensity;
        FLOAT        fReflectionIntensity;
    };

    struct XeWaterPatchSet {
        ULONG ulFirstPatchIndex;
        ULONG ulPatchCount;

        void    Clear( ) { ulFirstPatchIndex = ulPatchCount = 0; }
    };

    struct XeWaterState
    {
        XeWaterPatchInfo    m_aWaterPatches[XE_WATERPATCH_REGISTER_MAX];
        XeWaterPatchSet     m_aoPatchSets[XE_MAX_PATCHSET_STACK_SIZE];
        ULONG               m_ulCurrentPatchSet;
        ULONG               m_ulNbPatchSets;
        ULONG               m_ulCurrentPatchInfoIdIndex;
        ULONG               m_aulPatchInfoIdList[XE_MAX_PATCH_ID_LIST];
    };

    XeWaterManager();
    ~XeWaterManager();

    void        ReInit                  ( );
    void        Shutdown                ( );
    void        OnDeviceLost            ( );
    void        OnDeviceReset           ( );

    ULONG               RegisterWaterPatch(WATER3D_tdst_Modifier* _pst_Water, WATER3D_tdst_Patches* p_Patch);
    void                UnregisterWaterPatch(WATER3D_tdst_Patches* p_Patch);
    void                InvalidatePatchInfo(ULONG ulId) { m_oStateEngine.m_aWaterPatches[ulId].bUpdated = FALSE; }
    inline XeWaterPatchInfo*   GetWaterPatchInfoByIndex(ULONG ulIndex);
    XeWaterPatchInfo*   UpdateWaterPatchInfo(XeRenderObject *_pWaterObject, ULONG _ulPatchInfoId);
    void                UpdateModifierParams(ULONG ulPatchId, WATER3D_tdst_Modifier* _pst_Water);
    BOOL                BeginReflection( int _nbElem );
    ULONG               RenderReflection(ULONG ulNbWaterElements, XeRenderObject **_aoWaterList, ULONG ulNbReflectedElements, XeRenderObject **_aoReflectedList);
    void                EndReflection();
    void                RenderWater(ULONG ulNbWaterElements, XeRenderObject **_aoWaterList, int ulNbReflectedElements, XeRenderObject **_aoReflectedList);
    VECTOR4FLOAT*       GetTexOffset(ULONG _ul_Index) { return &m_avTexOffsets[_ul_Index]; }

    // Patch Set
    inline void         ClearPatchSetStack();
    inline ULONG        GetCurrentWaterPatchSetIndex() { return m_oStateEngine.m_ulCurrentPatchSet; }
    inline void         BeginAddPatch();
    inline void         AddPatch(ULONG _ulPatchInfoId, XeMesh* _poMesh);

    // context
    inline void         BackupState() { L_memcpy(&m_oStateRender, &m_oStateEngine, sizeof(XeWaterState)); }

private:
    IDirect3DDevice9    *m_pD3DDevice;
    BOOL                m_bUsingReflectionBuffer;
    ULONG               m_ulReflectedObjects;
    XeMaterial          *m_pWaterMaterial;
    WATER3D_tdst_Patches* m_aRegisteredWaterPatches[XE_WATERPATCH_REGISTER_MAX];
    
    // context sensitive data
    XeWaterState        m_oStateEngine;
    XeWaterState        m_oStateRender;

    // reflection
    XeMaterial         *m_pBlurMaterial;
    XeRenderObject     *m_pBlurObject;
    VECTOR4FLOAT       m_avTexOffsets[2];
    FLOAT              m_fBlurTexelWidth;
    FLOAT              m_fBlurTexelHeight;
    IDirect3DTexture9  *m_pReflectionBuffers[2];

};

extern XeWaterManager g_oWaterManager;
extern FLOAT          g_fWaterBlurFactor;

// ------------------------------------------------------------------------------------------------
// Name   : GetWaterPatchInfoByIndex
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
XeWaterManager::XeWaterPatchInfo* XeWaterManager::GetWaterPatchInfoByIndex(ULONG ulIndex)
{
	ERR_X_Assert((ulIndex < XE_WATERPATCH_REGISTER_MAX) && (m_aRegisteredWaterPatches[ulIndex] != NULL));

	return &m_oStateRender.m_aWaterPatches[ulIndex];
}

// ------------------------------------------------------------------------------------------------
// Name   : BeginAddPatch
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::BeginAddPatch()
{
#if defined(_DEBUG)
    if (m_oStateEngine.m_ulNbPatchSets == XE_MAX_PATCHSET_STACK_SIZE)
    {
        ERR_OutputDebugString("[Xenon] Patch set stack size is too small, patch information will be discarded\n");
        return;
    }
#endif

    m_oStateEngine.m_ulCurrentPatchSet = m_oStateEngine.m_ulNbPatchSets;
    ++m_oStateEngine.m_ulNbPatchSets;

    m_oStateEngine.m_aoPatchSets[m_oStateEngine.m_ulCurrentPatchSet].ulPatchCount = 0;
    m_oStateEngine.m_aoPatchSets[m_oStateEngine.m_ulCurrentPatchSet].ulFirstPatchIndex = m_oStateEngine.m_ulCurrentPatchInfoIdIndex;
}

// ------------------------------------------------------------------------------------------------
// Name   : AddPatch
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::AddPatch(ULONG _ulPatchInfoId, XeMesh* _poMesh)
{
    ERR_X_Assert(m_oStateEngine.m_ulCurrentPatchInfoIdIndex < XE_MAX_PATCH_ID_LIST);
    ERR_X_Assert(_ulPatchInfoId < XE_WATERPATCH_REGISTER_MAX);

    // update patch info with new mesh
    m_oStateEngine.m_aWaterPatches[_ulPatchInfoId].poMesh = _poMesh;

    // add patch id to patch id list
    m_oStateEngine.m_aulPatchInfoIdList[m_oStateEngine.m_ulCurrentPatchInfoIdIndex++] = _ulPatchInfoId;
    m_oStateEngine.m_aoPatchSets[m_oStateEngine.m_ulCurrentPatchSet].ulPatchCount++;
}

// ------------------------------------------------------------------------------------------------
// Name   : ClearPatchSetStack
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeWaterManager::ClearPatchSetStack()
{
    m_oStateEngine.m_ulCurrentPatchSet = 0;
    m_oStateEngine.m_ulNbPatchSets     = 1;
    m_oStateEngine.m_ulCurrentPatchInfoIdIndex = 0;
}

#endif // WATER_MANAGER_HEADER