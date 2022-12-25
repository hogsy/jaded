#ifndef XERENDEROBJECT_HEADER
#define XERENDEROBJECT_HEADER

#include "XeMaterial.h"
#include "XeMesh.h"
#include "XeVertexShaderManager.h"
#include "XePixelShaderManager.h"
#include "XeFurManager.h"
#include "XeContextManager.h"

class XeRenderObject : public XeCustomShader
{
    enum
    {
        INSERT_ORDER_MASK = 0x00ffffff,
        LAYER_INDEX_MASK  = 0xff000000,
        LAYER_INDEX_SHIFT = 24,
    };

public:

    M_DeclareOperatorNewAndDelete();

    XeRenderObject();
    virtual ~XeRenderObject() { };
    void ReInit();

    enum XEPRIMITIVETYPE
    {
#if defined(ACTIVE_EDITORS)
        LineList,
#endif
        LineStrip,
        TriangleList,
        PointList,
        QuadList,
		TriangleStrip,
    };

	enum XEOBJECTTYPE
	{
		Common,
		SPG2,
		LightShaft,
		Fur,
        Sprites,
        Interface,
        Water,
        Rain,
        Ocean,
        Particles,
		NumberOfObjectTypes
	};

    enum XEMATERIALLODTYPE
    {
        MatLODFull = 0,
        MatLODBlend,
        MatLODOptimize
    };

    inline void SetMaterial(XeMaterial* _po_Material) { m_poMaterial = _po_Material; }
    inline void CopyMaterial();
	inline void SetBlendingMode(ULONG _BM) { m_ulBM = _BM; }
    inline void SetMesh(XeMesh* _poMesh) { m_poMesh = _poMesh; }
    inline void SetDrawMask(ULONG _ulDrawMask) { m_ulDrawMask = _ulDrawMask; }
    void SetPrimType(XEPRIMITIVETYPE _ePrimType);
    inline void SetUserData(LPVOID _pUserData) { m_pUserData = _pUserData; }
    void SetFaceCount(unsigned long ulNewFaceCount);

    inline XeMaterial*     GetMaterial() { return m_poMaterial; }
	inline ULONG           GetBlendingMode() { return m_ulBM; }
    inline XeMesh*         GetMesh() { return m_poMesh; }
    inline ULONG           GetDrawMask() { return m_ulDrawMask; }
    inline D3DPRIMITIVETYPE GetPrimType() { return m_ePrimitiveType; }
    inline LPVOID          GetUserData() { return m_pUserData; }
    UINT                   GetPrimitiveCount();

    inline ULONG           GetAmbientColorSlot();

    inline D3DXMATRIX*     GetWorldMatrix();
    inline void            SetWorldMatrixIndex(ULONG _ulIndex) { m_ulWorldMatrixIndex = _ulIndex; }
    inline ULONG           GetWorldMatrixIndex() { return m_ulWorldMatrixIndex; }

    inline void            SetWorldViewMatrixIndex(ULONG _ulIndex) { m_ulWorldViewMatrixIndex = _ulIndex; }
    inline ULONG           GetWorldViewMatrixIndex() { return m_ulWorldViewMatrixIndex; }

    inline void            SetShadowWorldViewMatrixIndex(ULONG _ulIndex) { m_ulShadowWorldViewMatrixIndex = _ulIndex; }
    inline ULONG           GetShadowWorldViewMatrixIndex() { return m_ulShadowWorldViewMatrixIndex; }

    inline void            SetProjMatrixIndex(ULONG _ulIndex) { m_ulProjMatrixIndex = _ulIndex; }
    inline ULONG           GetProjMatrixIndex() { return m_ulProjMatrixIndex; }

    inline void            SetShadowProjMatrixIndex(ULONG _ulIndex) { m_ulShadowProjMatrixIndex = _ulIndex; }
    inline ULONG           GetShadowProjMatrixIndex() { return m_ulShadowProjMatrixIndex; }

    inline void            SetSkinningSet(XeSkinningSet *_poSkinningSet) { m_oSkinningSet = *_poSkinningSet; }
    inline XeSkinningSet*  GetSkinningSet() { return &m_oSkinningSet; }

    inline void            SetLightSetIndex(ULONG _ulIndex) { m_ulLightSetIndex = _ulIndex; }
    inline ULONG           GetLightSetIndex() { return m_ulLightSetIndex; }

    inline void            SetLOD(UCHAR _ucLOD) { m_ucLOD = _ucLOD; }
    inline UCHAR           GetLOD(void) { return m_ucLOD; }

	inline void            SetOrder(CHAR _cOrder) { m_cOrder = _cOrder; }
	inline CHAR            GetOrder(void) { return m_cOrder; }

	inline void            SetObjectType(XEOBJECTTYPE _eType) { m_eObjectType = _eType; }
	inline XEOBJECTTYPE    GetObjectType(void) { return m_eObjectType; }

    inline void            SetInsertOrder(UINT _iOrder, UINT _iLayerIndex) { m_iInsertOrder = (_iOrder & INSERT_ORDER_MASK) | ((_iLayerIndex << LAYER_INDEX_SHIFT) & LAYER_INDEX_MASK); }
    inline UINT	           GetInsertOrder(void) { return (m_iInsertOrder & INSERT_ORDER_MASK); }
    inline UINT            GetLayerIndex(void) { return ((m_iInsertOrder & LAYER_INDEX_MASK) >> LAYER_INDEX_SHIFT); }

	inline void					  SetSPG2ShadersConsts(tdstSPG2ShaderConsts _stConsts)	{ m_stSPG2ShaderConsts = _stConsts; }
	inline tdstSPG2ShaderConsts	  GetSPG2ShadersConsts()									{ return m_stSPG2ShaderConsts;		}
    inline tdstSPG2ShaderConsts	  *GetSPG2ShadersConstsPointer()							{ return &m_stSPG2ShaderConsts;		}

	inline void					  SetFURShaderConsts(XeFurInfo *_pvParams)	{ m_stFurShaderConsts = *_pvParams; }
	inline XeFurInfo*   GetFURShaderConsts()								{ return &m_stFurShaderConsts;					 }

    inline void            SetExtraDataIndex(ULONG _ulIndex) { m_ulExtraDataIndex = _ulIndex; }
    inline ULONG           GetExtraDataIndex(void) { return m_ulExtraDataIndex; }

    inline void            SetExtraFlags(ULONG _ulFlags) { m_ulExtraFlags = _ulFlags; }
    inline ULONG           GetExtraFlags(void) { return m_ulExtraFlags; }

    // rim light
    inline BOOL            IsRimLightEnabled( ) { return m_isRimLightEnabled; }
    inline void            EnableRimLight( BOOL _bEnable ) { m_isRimLightEnabled = _bEnable; }
    inline void            SetRimLightVSConsts( const tdstRimLightVSConsts & _rimLightVsConsts ) { m_stRimLightVSConsts = _rimLightVsConsts; }
    inline void            SetRimLightPSConsts( const tdstRimLightPSConsts & _rimLightPsConsts ) { m_stRimLightPSConsts = _rimLightPsConsts; }
    const tdstRimLightVSConsts & GetRimLightVSConsts( ) { return m_stRimLightVSConsts; }
    const tdstRimLightPSConsts & GetRimLightPSConsts( ) { return m_stRimLightPSConsts; }

    // Heat shimmering
    void                                SetHeatShimmerVSConsts( const tdstHeatShimmerVSConsts & _stHeatShimmerVSConsts ) { m_stHeatShimmerVSConsts = _stHeatShimmerVSConsts; }  
    const tdstHeatShimmerVSConsts &     GetHeatShimmerVSConsts( ) { return m_stHeatShimmerVSConsts; } 

    // Lightmaps
    void                    SetLMTexture(ULONG _LMTexID) { m_ulLMTexID = _LMTexID; }
    ULONG                   GetLMTexture()  { return m_ulLMTexID; } 

    // Fur
    void                    UpdateFurInfo() { g_oFurManager.UpdateObjectFurInfo(this); }

    // Visibility query
    inline void             SetVisQueryIndex(INT _iIdx);
    inline INT              GetVisQueryIndex(void);

    inline void             SetWYBIndices(INT _iIdx1, INT _iIdx2) { m_iWYBParamsIndex1 = _iIdx1; m_iWYBParamsIndex2 = _iIdx2; }
    inline void             GetWYBIndices(INT& _iIdx1, INT& _iIdx2) { _iIdx1 = m_iWYBParamsIndex1; _iIdx2 = m_iWYBParamsIndex2; }

    inline void             SetSymmetryIndex(ULONG _ulIndex) { m_ulSymmetryIndex = _ulIndex; }
    inline ULONG            GetSymmetryIndex(void) { return m_ulSymmetryIndex; }

    inline void             SetPlanarGizmoMatrixIndex( ULONG _ulIndex ) { m_ulPlanarGizmoMatrixIndex = _ulIndex; }
    inline ULONG            GetPlanarGizmoMatrixIndex(void) { return m_ulPlanarGizmoMatrixIndex; }

    // Water
    inline void             SetWaterPatchSetIndex( ULONG _ulIndex ) { m_ulWaterPatchSetIndex = _ulIndex; }
    inline ULONG            GetWaterPatchSetIndex() { return m_ulWaterPatchSetIndex; }

    inline void                 SetMaterialLODDetailState( XEMATERIALLODTYPE _ulState ) { m_ulMaterialLODDetailState = _ulState; }
    inline void                 SetMaterialLODDetailBlend( FLOAT _fBlend )  { m_fMaterialLODDetailBlend = _fBlend; }
    inline XEMATERIALLODTYPE    GetMaterialLODDetailState( ) { return m_ulMaterialLODDetailState; }
    inline FLOAT                GetMaterialLODDetailBlend( ) { return m_fMaterialLODDetailBlend; }
    inline void                 SetMaterialLODState( XEMATERIALLODTYPE _ulState ) { m_ulMaterialLODState = _ulState; }
    inline void                 SetMaterialLODBlend( FLOAT _fBlend )  { m_fMaterialLODBlend = _fBlend; }
    inline XEMATERIALLODTYPE    GetMaterialLODState( ) { return m_ulMaterialLODState; }
    inline FLOAT                GetMaterialLODBlend( ) { return m_fMaterialLODBlend; }

private:                                      

	ULONG			        m_ulBM;
    XeMaterial             *m_poMaterial;
    XeMaterial              m_stMaterial;
    MAT_tdst_MTLevel		m_oMLTTXLVL;
    XeMesh*                 m_poMesh;
    ULONG                   m_ulDrawMask;
    D3DPRIMITIVETYPE        m_ePrimitiveType;
    ULONG                   m_ulWorldMatrixIndex;
    ULONG                   m_ulWorldViewMatrixIndex;
    ULONG                   m_ulProjMatrixIndex;
    ULONG                   m_ulShadowWorldViewMatrixIndex;
    ULONG                   m_ulShadowProjMatrixIndex;
    UCHAR                   m_ucLOD;
	CHAR                    m_cOrder;
	UINT	                m_iInsertOrder;
    ULONG                   m_ulExtraDataIndex;
    ULONG                   m_ulExtraFlags;
    XeSkinningSet           m_oSkinningSet;
    ULONG                   m_ulLightSetIndex;
    ULONG                   m_ulWaterPatchSetIndex;
    LPVOID                  m_pUserData;
	XEOBJECTTYPE	        m_eObjectType;
    ULONG                   m_ulLMTexID; // lightmap texture ID

	tdstSPG2ShaderConsts	m_stSPG2ShaderConsts;
	XeFurInfo       		m_stFurShaderConsts;

    BOOL                    m_isRimLightEnabled;
    tdstRimLightVSConsts    m_stRimLightVSConsts;
    tdstRimLightPSConsts    m_stRimLightPSConsts;

    tdstHeatShimmerVSConsts m_stHeatShimmerVSConsts;

    INT                     m_iVisQueryIndex;  
    INT                     m_iWYBParamsIndex1;
    INT                     m_iWYBParamsIndex2;

    ULONG                   m_ulSymmetryIndex;
    ULONG                   m_ulPlanarGizmoMatrixIndex;

    XEMATERIALLODTYPE       m_ulMaterialLODDetailState;
    FLOAT                   m_fMaterialLODDetailBlend;
    XEMATERIALLODTYPE       m_ulMaterialLODState;
    FLOAT                   m_fMaterialLODBlend;

public:
	MATH_tdst_Matrix        m_stModelViewMatrix;
	BOOL                    bDynamic;
};

inline void XeRenderObject::CopyMaterial() 
{ 
    m_poMaterial->MakeACopy(m_stMaterial); 
    if(m_stMaterial.IsJadeMaterial())
    {
        m_oMLTTXLVL = *m_stMaterial.GetJadeMatLevel();
        m_stMaterial.SetJadeMatLevel(&m_oMLTTXLVL);
    }
    m_poMaterial = &m_stMaterial;
}

D3DXMATRIX* XeRenderObject::GetWorldMatrix()
{
    return g_pXeContextManagerRender->GetWorldMatrixByIndex(m_ulWorldMatrixIndex);
}

ULONG XeRenderObject::GetAmbientColorSlot()
{
    if (m_poMaterial != NULL)
    {
        ULONG ulAmbientSel = m_poMaterial->GetAmbientSelection();
        if (ulAmbientSel == MAT_Xe_AmbientSel_Ambient1)
        {
            return 0;
        }
        else if (ulAmbientSel == MAT_Xe_AmbientSel_Ambient2)
        {
            return 1;
        }
    }
    
    return (m_ulDrawMask & GDI_Cul_DM_DontUseAmbient2) ? 0 : 1;
}

// Visibility query
#ifdef _XENON
inline void             XeRenderObject::SetVisQueryIndex(INT _iIdx) { m_iVisQueryIndex = _iIdx; }
inline INT              XeRenderObject::GetVisQueryIndex(void) { return m_iVisQueryIndex; }
#else
inline void             XeRenderObject::SetVisQueryIndex(INT _iIdx) {}
inline INT              XeRenderObject::GetVisQueryIndex(void) { return -1; }
#endif

#endif // XERENDEROBJECT_HEADER