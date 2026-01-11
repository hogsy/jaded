#ifndef XEFXMGR_HEADER
#define XEFXMGR_HEADER

#include "Precomp.h"
#include "XeRenderObject.h"
#include "DevHelpers/XePerf.h"

class XeFXManager
{
public:

    enum RenderPass
    {
        RP_DEFAULT,                     // Default pass, use the material and settings as-is (editor gizmos, rects, interface...)

        // For Jade default materials

        RP_ZPASS_Z_ONLY,                // Z only pass
        RP_ZPASS_ALPHA_TEST,            // Z only pass for alpha tested objects

        RP_APPLY_DEPTH_ENV_RLI,         // First pass - Depth, environment, RLI|Vertex paint
        RP_APPLY_PIXELLIGHTING_FIRSTPASS, // Light pass (additive)
        RP_APPLY_PIXELLIGHTING,         // Light pass (additive)
        RP_APPLY_VERTEXLIGHTING,        // Light pass
        RP_APPLY_SHADOW,                // Shadow pass (subtractive)
        RP_APPLY_AMBIENT,               // Ambient pass (additive)
        RP_APPLY_FOG,                   // Fog pass (alpha blend)
        RP_APPLY_WATER,                 // Water Pass
        RP_APPLY_REFLECTION,            // Reflection Pass
        RP_COMPUTE_SHADOW,              // Separate pass, render to shadowmap (Z only)

		RP_SPG2,
        RP_SPG2ZOVERWRITE,

        RP_NUM_PASSES
    };

    // Initialization
    XeFXManager();
    ~XeFXManager();
    void ReInit();

    BOOL OnDeviceLost(void);
    BOOL OnDeviceReset(void);
    void Shutdown(void);

    // update functions
	void BeginUpdate(XeRenderObject *_pObject, RenderPass _ePassType = RP_DEFAULT);
    void UpdateVSForSPG2PerPixelLightingPass(void);
	inline void UpdateCustomVSFeatures(ULONG _ulCustom, ULONG64 _ulFeatures);
	inline void UpdateCustomPSFeatures();
	void UpdateVSFeatures();
	void UpdatePSFeatures();
	void EndUpdate();

    void UpdateShaderFeaturesForSprites(void);

    void SetFogParams(BOOL _bEnable,
                      D3DCOLOR _dwColor = 0xffffffff,
                      float _fStart = 0.0f,
                      float _fEnd = 1000.0f,
                      float _fDensity = 1.0f,
                      float _fPitchAttenuationMin = 0.0f,
                      float _fPitchAttenuationMax = 90.0f,
                      float _fPitchAttenuationIntensity = 0.0f );

    inline void SetGlobalLODBias(FLOAT _f_Bias) { m_fLODBias = _f_Bias; }
    inline FLOAT GetGlobalLODBias() { return m_fLODBias; }

    // utility functions
    RenderPass  GetCurrentPass() { return m_eCurrentPass; }
    const XeMaterial* GetCurrentMaterial() { return m_pCurrentMaterial; }

    void SetRenderingDoublePassZOverwrite(BOOL _bRenderingDoublePassZOverwrite) { m_bRenderingDoublePassZOverwrite = _bRenderingDoublePassZOverwrite; }
    void SetRenderingInterface(BOOL _bRenderingInterface) { m_bRenderingInterface = _bRenderingInterface; }
    void SetRenderingOpaques(BOOL _bRenderingOpaques) { m_bRenderingOpaques = _bRenderingOpaques; }
    void SetRenderingDepthToColorInZPass( BOOL _bRenderToColor ) { m_bRenderingDepthToColorInZPass = _bRenderToColor; }

    void SetShadowCount(INT iCount) {m_iCurrentShadowLightCount = iCount;}

    void UpdatePSForFurPass(BOOL bFirstPass);

private:

    void SetPassIndependentInformation(void);
	void SetPassSpecificRenderStates(void);

	void SetTextureStages(void);

    void UpdateVSForDefaultPass(void);
	void UpdatePSForDefaultPass(void);
    void UpdateVSForDepthEnvRLIPass(void);
	void UpdatePSForDepthEnvRLIPass(void);
    void UpdateVSForVertexLightingPass(void);
	void UpdatePSForVertexLightingPass(void);
    void UpdateVSForPixelLightingPass(void);
	void UpdatePSForSPG2LightingPass(void);
	void UpdatePSForPixelLightingPass(BOOL bFirstPass);
    void UpdateVSForAmbientPass(void);
	void UpdatePSForAmbientPass(void);
    void UpdatePSForReflectionPass(void);
	
	void UpdateForShadowBufferPass(void);
    //void UpdateVSForShadowBufferPass(void);
	//void UpdatePSForShadowBufferPass(void);
    
	void UpdateForApplyShadowPass(void);
	//void UpdateVSForApplyShadowPass(void);
	//void UpdatePSForApplyShadowPass(void);

	void UpdateVSForWaterPass(void);
	void UpdatePSForWaterPass(void);

    void UpdateVSForZPass( bool _bAphaTest );
    void UpdatePSForZPass( bool _bAphaTest );

private:

    LPDIRECT3DDEVICE9   m_pD3DDevice;
    RenderPass          m_eCurrentPass;

    XeRenderObject*     m_pCurrentObject;
    XeMaterial*         m_pCurrentMaterial;
    XeMesh*             m_pCurrentMesh;
    ULONG               m_ulCurrentDrawMask;
    XeVertexShaderManager::XeVertexDeclaration *m_pCurrentDeclaration;
    INT                 m_iCurrentShadowLightCount;

    // LOD bias
    FLOAT               m_fLODBias;

    BOOL                m_bRenderingOpaques;
    BOOL                m_bRenderingDoublePassZOverwrite;
    BOOL                m_bRenderingDepthToColorInZPass;
    BOOL                m_bRenderingInterface;
};

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateCustomVSFeatures(ULONG _ulCustom, ULONG64 _ulFeatures)
{
    XEProfileFunction();

    // Set the custom vertex shader and features
    g_oVertexShaderMgr.SetFeatureCustomShader(_ulCustom);
    g_oVertexShaderMgr.SetFeaturesCustom(_ulFeatures);
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeFXManager::UpdateCustomPSFeatures()
{
    XEProfileFunction();

    ULONG i;

    ERR_X_Assert(m_pCurrentObject);
    ERR_X_Assert(m_pCurrentMaterial);

    // Set the custom pixel shader and features
    g_oPixelShaderMgr.SetFeatureCustomShader(m_pCurrentMaterial->GetCustomPS());

    for (i = 1; i <= XE_PS_CUSTOM_FEATURE_COUNT; ++i)
    {
        g_oPixelShaderMgr.SetFeatureCustom(i, m_pCurrentMaterial->GetCustomPSFeature(i));
    }
}


extern XeFXManager g_oFXManager;
#endif
