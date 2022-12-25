
#ifndef PIXELSHADER_MANAGER
#define PIXELSHADER_MANAGER

#include <map>
#include "XeSharedDefines.h"
#include "XeRenderStateManager.h"
#include "XeProfiling.h"

class XeRenderObject;
class XePixelShader;

// Pixel Shader Constant Mapping
#define PS_PHY_USER_CONST_MAX       224

struct XePSCustomShaderInfo
{
    const CHAR* szFileName;
};

extern const XePSCustomShaderInfo XEPS_CUSTOM_SHADERS[];
extern const ULONG                XEPS_NB_CUSTOM_SHADERS;

typedef struct 
{
    VECTOR4FLOAT    m_vScale;
}tdstHeatShimmerPSConsts;

typedef struct 
{
    float   m_fWidthMin;
    float   m_fWidthMax;
    float   m_fLerp;
    float   m_fIntensity;
    float   m_fFalloff;
    ULONG   m_ulColor;
    BOOL    m_bSpecularMapAttenuationEnabled;
   
}tdstRimLightPSConsts;

typedef struct 
{
    float   m_fLuminosityMin;
    float   m_fLuminosityMax;
    float   m_fIntensity;
    ULONG   m_ulGlowColor;
    float   m_fZNear;
    float   m_fZFar;
}tdstAEGlowPSConsts;

// XePixelShaderManager class definition
class XePixelShaderManager
{
public:

    union XePSFeature
    {
        struct
        {
            // SC: Please increment XE_SHADER_DB_VERSION (XeShaderDatabase.cpp) for any feature change

            ULONG64 ulCustomShader              : 8;    // See CUSTOMSHADER_*
            ULONG64 bUseBaseTexture             : 1;    // Enable base map
            ULONG64 ulPPLightCount              : 3;    // number of per-pixel light
            ULONG64 bAddAmbient                 : 1;    // ambient lighting
            ULONG64 bAddGlobalRLI               : 1;    // Global RLI 
            ULONG64 bAddLocalRLI                : 1;    // Local RLI 
            ULONG64 bDiffusePPL                 : 1;    // Add the diffuse component (PPL)
            ULONG64 bSpecularPPL                : 1;    // Add the specular component (PPL)
            ULONG64 bUseSpecularMap             : 1;    // Mask specular using the specular map
            ULONG64 bUseDetailNormalMap         : 1;    // Use the detail normal map
            ULONG64 bOutputFog                  : 1;    // Only output color as FogColor [RGB] + FogFactor [A]
            ULONG64 bLocalAlpha                 : 1;    // Enable local alpha
            ULONG64 bFogEnable                  : 1;    // Enable fog
            ULONG64 ulSpecularMapChannel        : 2;    // See SPECULARMAPCHANNEL_*
            ULONG64 bColor2X                    : 1;    // Scale final color by 2
            ULONG64 bRemapAlphaRange            : 1;    // alpha range remapping
            ULONG64 bUseMossMap                 : 1;    // Use a moss map
			ULONG64 boUseOffset					: 1;    // Use a Offset
			ULONG64 bDepthToColorEnable         : 1;    // Depth to color
            ULONG64 bUseShadow0                 : 1;    // Use Shadow 0
            ULONG64 bUseShadow1                 : 1;    // Use Shadow 1
            ULONG64 bUseShadow2                 : 1;    // Use Shadow 2
            ULONG64 bUseShadow3                 : 1;    // Use Shadow 3
            ULONG64 bRimLight0                  : 1;    // Use Rim Light 0
            ULONG64 bRimLight1                  : 1;    // Use Rim Light 1
            ULONG64 bRimLight2                  : 1;    // Use Rim Light 2
            ULONG64 bRimLight3                  : 1;    // Use Rim Light 3
            ULONG64 bRimLightSMapAttenuation    : 1;    // Use Rim Light Specular Map Attenuation
            ULONG64 bNormalOffset               : 1;    // Normal Offset feature
            ULONG64 bIsStaticObject             : 1;    // Whether object is static or dynamic (skinned)
            ULONG64 bAlphaTest                  : 1;    // Enable alpha testing
            ULONG64 bSpecularInBaseMap          : 1;    // Specular map is in the base texture
            ULONG64 bMaterialLODDetailOptimize  : 1;    // Material LOD Detail optimize
            ULONG64 iMaterialLODState           : 2;    // Material LOD state
            ULONG64 bLuminanceInAlpha           : 1;    // Luminance in alpha
            ULONG64 bReflection                 : 1;    // Reflection
			ULONG64 ulReserved2                 : 20;
        };
        struct
        {
            ULONG64 ulCustomShaderDup     : 8;    // See CUSTOMSHADER_*
            ULONG64 ulFeature1            : 3;
            ULONG64 ulFeature2            : 8;    // TODO: put this back to 2 bits when we have PS 3.0
            ULONG64 ulFeature3            : 3;
            ULONG64 ulFeature4            : 3;
            ULONG64 ulFeature5            : 3;
            ULONG64 ulFeature6            : 3;
            ULONG64 ulFeature7            : 3;
            ULONG64 ulFeature8            : 3;
            ULONG64 ulFeature9            : 3;
            ULONG64 ulFeature10           : 3;
            ULONG64 ulFeature11           : 3;
            ULONG64 ulFeature12           : 3;
            ULONG64 ulFeature13           : 3;
            ULONG64 ulFeature14           : 3;
            ULONG64 ulFeature15           : 3;
            //ULONG64 ulFeature16           : 3;

            ULONG64 ulReserved2           : 6;
        };

        ULONG64 ulKey;

        void Clear(void) { ulKey = 0; };
    };

    struct XePixelShaderEntry
    {
        TCHAR               FileName[MAX_PATH];
        XePSFeature         Key;
    };
    typedef std::vector<XePixelShaderEntry> PixelShaderArray;

    union ConstantDirtyFlags {
        struct {
            DWORD uReserved         : 32;
        };
        DWORD dwRawFlags;

        void Clear() { dwRawFlags = 0; }
        void DirtyAll() { dwRawFlags = 0xFFFFFFFF; }
    };

    XePixelShaderManager();
    ~XePixelShaderManager();
    void         ReInit();
    BOOL OnDeviceLost(void);
    BOOL OnDeviceReset(void);
    void Shutdown(void);

    // Shader management
    void        InitShadersTable(void);
    void        UnloadAll(void);
    BOOL        LoadShader(XePixelShaderEntry *_poShaderEntry);
    BOOL        GenerateShader(XePSFeature _oFeatures);
    BOOL        ReloadShaders(void);

    void AddShader(ULONG64 _ulKey, XePixelShader* _pShader);
    void AddCreateShader(ULONG64 _ulKey, const void* _pCode, ID3DXConstantTable* _pConstantTable, ULONG64 _ulShaderSemantics );

    // Features
    inline XePSFeature GetCurrentFeatures()                     { return m_oFeatures; }
    inline void        SetCurrentFeatures(XePSFeature _oFeatures){ m_oFeatures = _oFeatures; }
    inline void ResetAllFeatures()                              { m_oFeatures.Clear(); }
    inline void SetFeatureCustomShader(ULONG _ulShader)         { m_oFeatures.ulCustomShader        = _ulShader; }
    inline void SetFeatureUseBaseMap(BOOL _bEnable)             { m_oFeatures.bUseBaseTexture       = _bEnable; }
    inline void SetFeatureUseMossMap(BOOL _bEnable)             { m_oFeatures.bUseMossMap           = _bEnable; }
	inline void SetFeatureUseOffset(BOOL _bEnable)	            { m_oFeatures.boUseOffset			= _bEnable; }
    inline void SetFeatureColor2X(BOOL _bEnable)                { m_oFeatures.bColor2X              = _bEnable; }
    inline void SetFeaturePPLightCount(ULONG _ulCount)          { m_oFeatures.ulPPLightCount        = _ulCount; }
    inline void SetFeatureAddAmbient(BOOL _bEnable)             { m_oFeatures.bAddAmbient           = _bEnable; }
    inline void SetFeatureAddGlobalRLI(BOOL _bEnable)           { m_oFeatures.bAddGlobalRLI         = _bEnable; }
    inline void SetFeatureAddLocalRLI(BOOL _bEnable)            { m_oFeatures.bAddLocalRLI          = _bEnable; }
    inline void SetFeatureDiffusePPL(BOOL _bEnable)             { m_oFeatures.bDiffusePPL           = _bEnable; }
    inline void SetFeatureSpecularPPL(BOOL _bEnable)            { m_oFeatures.bSpecularPPL          = _bEnable; }
    inline void SetFeatureUseSpecularMap(BOOL _bEnable)         { m_oFeatures.bUseSpecularMap       = _bEnable; }
    inline void SetFeatureUseDetailNormalMap(BOOL _bEnable)     { m_oFeatures.bUseDetailNormalMap   = _bEnable; }
    inline void SetFeatureOutputFog(BOOL _bEnable)              { m_oFeatures.bOutputFog            = _bEnable; }
    inline void SetFeatureLocalAlpha(BOOL _bEnable)             { m_oFeatures.bLocalAlpha           = _bEnable; }
    inline void SetFeatureFogEnable(BOOL _bEnable)              { m_oFeatures.bFogEnable            = _bEnable; }
    inline void SetFeatureSpecularMapChannel(ULONG _ulChannel)  { m_oFeatures.ulSpecularMapChannel  = _ulChannel; }
    inline void SetFeatureRemapAlphaRange(BOOL _bEnable)        { m_oFeatures.bRemapAlphaRange      = _bEnable; }
    inline void SetFeatureNormalOffset(BOOL _bEnable)           { m_oFeatures.bNormalOffset         = _bEnable; }
    inline void SetFeatureCustom(ULONG _ulFeature, ULONG _ulVal);
    inline void SetFeatureDepthToColorEnable(BOOL _bEnable)     { m_oFeatures.bDepthToColorEnable   = _bEnable; }
    void        SetFeatureUseShadow(ULONG _ulShadow, BOOL _bEnable );
    void        SetFeatureRimLight(ULONG _ulLightIndex, ULONG _ulRimLightEnable );
    void        SetFeatureRimLightEnableSMapAttenuation( BOOL _bEnable )    { m_oFeatures.bRimLightSMapAttenuation    = _bEnable; }
    inline void SetFeatureIsStatic(BOOL _bStatic)               { m_oFeatures.bIsStaticObject       = _bStatic; }
    inline void SetFeatureAlphaTestEnable(BOOL _bEnable)        { m_oFeatures.bAlphaTest            = _bEnable; }
    inline void SetFeatureSpecularInBaseMap(BOOL _bEnable)      { m_oFeatures.bSpecularInBaseMap    = _bEnable; }
    inline void SetFeatureMaterialLODState(ULONG _iState)       { m_oFeatures.iMaterialLODState     = _iState; }
    inline void SetFeatureMaterialLODDetailOptimize(BOOL _bOptimize) { m_oFeatures.bMaterialLODDetailOptimize = _bOptimize; }
    inline void SetFeatureLuminanceInAlpha(BOOL _bEnable )      { m_oFeatures.bLuminanceInAlpha = _bEnable; }
    inline void SetFeatureReflection(BOOL _bEnable )            { m_oFeatures.bReflection = _bEnable; }

    // Constant access
    inline void DirtyAllConstants() { m_oDirtyFlags.DirtyAll(); }

    // update functions
    inline void Update(XeRenderObject *_pObject);
    void        UpdateShaderConstants(XePixelShader *_pShader, XeRenderObject* _pObject);

    typedef std::map<ULONG64, XePixelShader*> MapPixelShader;

    inline void  EnableGlobalMul2X(BOOL bEnabled) { m_bForce2X = bEnabled; }
    inline BOOL  IsGlobalMul2XEnabled() { return m_bForce2X; }
    inline void  SetGlobalMul2XFactor(FLOAT fFactor) { m_fGlobalMul2XFactor = fFactor; }
    inline FLOAT GetGlobalMul2XFactor() { return m_fGlobalMul2XFactor; }

    inline void  SetHeatShimmerConsts( tdstHeatShimmerPSConsts _stConsts ) { m_stHeatShimmerConsts = _stConsts; }

    inline void  SetPPLightCount(INT iCount) { m_iPPLLightCount = iCount; };
    inline INT   GetPPLightCount() { return m_iPPLLightCount; };

    void         SetRimLightShaderConsts( const tdstRimLightPSConsts & _rimLightShaderConsts ) { m_stRimLightShaderConsts = _rimLightShaderConsts; }
    const tdstRimLightPSConsts & GetRimLightShaderConsts( ) { return m_stRimLightShaderConsts; }

    void         SetAEGlowShaderConsts( const tdstAEGlowPSConsts & _stAEGlowConsts ) { m_stAEGlowConsts = _stAEGlowConsts; }
    const tdstAEGlowPSConsts & GetAEGlowShaderConsts( ) { return m_stAEGlowConsts; }

    inline void  SetShadowColor(VECTOR4FLOAT vColor) { m_vShadowColor = vColor; }
    void         SetShadowChannels( ULONG ulLightIndex, INT iChannel );
    inline BOOL  IsShadowEnabled(ULONG _ulShadow) { return m_abEnableShadow[_ulShadow]; }
    BOOL         IsRimLightEnabled(ULONG _ulLightIndex );

    void         SetMotionBlurAlpha ( float _fMotionBlurAlpha ) { m_fMotionBlurAlpha = _fMotionBlurAlpha; }
    float        GetMotionBlurAlpha ( )                         { return m_fMotionBlurAlpha; }

    void         SetRemananceFactor ( float _fFactor )          { m_fRemananceFactor = _fFactor; }
    float        GetRemananceFactor ( )                         { return m_fRemananceFactor; }

    void         SetColorBalance    ( const VECTOR4FLOAT & _vColor )          { m_vColorBalance = _vColor; }
    const VECTOR4FLOAT & GetColorBalance ( )                    { return m_vColorBalance; }

    void         SetBrightness    ( const VECTOR4FLOAT & _vBrightness )          { m_vBrightness = _vBrightness; }
    const VECTOR4FLOAT & GetBrightness ( )                    { return m_vBrightness; }

    inline void  InvalidateFeatureKey(void);

    void PreLoadShader(ULONG64 _ulKey, void* _pCode, ULONG64 _ulSemantics );

private:
    
    XePixelShader* CreateShader(ULONG64 _ulKey, const void* _pShaderCode, ID3DXConstantTable* _pConstantTable, CHAR* _szName);

private:

    PixelShaderArray    m_aPixelShaderEntries;

    LPDIRECT3DDEVICE9   m_pD3DDevice;
    MapPixelShader      m_mapLoadedShaders;

    ConstantDirtyFlags  m_oDirtyFlags;
    BOOL                m_bForce2X;
    FLOAT               m_fGlobalMul2XFactor;

    // Features parameters (0 means disabled)
    XePSFeature         m_oFeatures;
    XePSFeature         m_oCurrentFeatures;

    // Shadow info
    VECTOR4FLOAT        m_vShadowColor;

    // Per-Pixel Lighting
    INT                 m_iPPLLightCount;

    tdstHeatShimmerPSConsts     m_stHeatShimmerConsts;
    tdstRimLightPSConsts        m_stRimLightShaderConsts;
    tdstAEGlowPSConsts          m_stAEGlowConsts;
    float                       m_fMotionBlurAlpha;
    float                       m_fRemananceFactor;
    VECTOR4FLOAT                m_vColorBalance;
    VECTOR4FLOAT                m_vBrightness;

    BOOL                        m_abEnableShadow[VS_MAX_LIGHTS_PER_PASS];
    BOOL                        m_abEnableLM[VS_MAX_LIGHTS_PER_PASS];
    VECTOR4FLOAT                m_aoShadowChannels[VS_MAX_LIGHTS_PER_PASS];
};

inline void XePixelShaderManager::SetFeatureCustom(ULONG _ulFeature, ULONG _ulVal)
{
    if (m_oFeatures.ulCustomShader != 0)
    {
        switch (_ulFeature)
        {
            case 1: m_oFeatures.ulFeature1 = _ulVal; break;
            case 2: m_oFeatures.ulFeature2 = _ulVal; break;
            case 3: m_oFeatures.ulFeature3 = _ulVal; break;
            case 4: m_oFeatures.ulFeature4 = _ulVal; break;
            case 5: m_oFeatures.ulFeature5 = _ulVal; break;
            case 6: m_oFeatures.ulFeature6 = _ulVal; break;
            case 7: m_oFeatures.ulFeature7 = _ulVal; break;
            case 8: m_oFeatures.ulFeature8 = _ulVal; break;
            case 9: m_oFeatures.ulFeature9 = _ulVal; break;
            case 10: m_oFeatures.ulFeature10 = _ulVal; break;
            case 11: m_oFeatures.ulFeature11 = _ulVal; break;
            case 12: m_oFeatures.ulFeature12 = _ulVal; break;
            case 13: m_oFeatures.ulFeature13 = _ulVal; break;
            case 14: m_oFeatures.ulFeature14 = _ulVal; break;
            case 15: m_oFeatures.ulFeature15 = _ulVal; break;
			//case 16: m_oFeatures.ulFeature16 = _ulVal; break;
        }
    }
}

inline void XePixelShaderManager::InvalidateFeatureKey(void)
{
    m_oCurrentFeatures.ulKey = (ULONG)-1;
}

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XePixelShaderManager::Update(XeRenderObject *_pObject)
{
    if (m_oFeatures.ulKey == m_oCurrentFeatures.ulKey)
        return;

#if defined(_XENON)
    if (m_oFeatures.ulKey == 0)
    {
        g_oRenderStateMgr.SetPixelShader(NULL);
        m_oCurrentFeatures.ulKey = m_oFeatures.ulKey;
        return;
    }
#endif

    CXBBeginEventObject oEvent("XePixelShaderManager::Update");

    // select and send vertex shader    
    // look for cached vertex shader for current key
    MapPixelShader::iterator it = m_mapLoadedShaders.find( m_oFeatures.ulKey );

    if( it != m_mapLoadedShaders.end( ) )
    {
        // use cached vertex shader
        g_oRenderStateMgr.SetPixelShader(it->second);
        m_oCurrentFeatures.ulKey = m_oFeatures.ulKey;
    }
    else
    {
        if (GenerateShader(m_oFeatures))
        {
            it = m_mapLoadedShaders.find(m_oFeatures.ulKey);
            ERR_X_Assert( it != m_mapLoadedShaders.end() );
            g_oRenderStateMgr.SetPixelShader(it->second);
            m_oCurrentFeatures.ulKey = m_oFeatures.ulKey;
        }
        else
        {
            _breakpoint_;
        }
    }

    DirtyAllConstants();
}

extern __declspec(align(32)) XePixelShaderManager g_oPixelShaderMgr;

#endif // PIXELSHADER_MANAGER
