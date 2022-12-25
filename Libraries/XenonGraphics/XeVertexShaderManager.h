
#ifndef XEVERTEXSHADERMGR_HEADER
#define XEVERTEXSHADERMGR_HEADER

#include <map>
#include "XeSharedDefines.h"
#include "XeUtils.h"

class XeRenderObject;
class XeVertexShader;

//#define VS_FORCE_SW_LIGHTING
//#define VS_FORCE_SW_SKINNING

inline BOOL Xe_IsForcingSWSkinning(void);

// Vertex Shader Constant Mapping

#define VS_INITIAL_MATRIX_STACK_SIZE 32
#define VS_INITIAL_SKINNING_MATRIX_STACK_SIZE   128
#define VS_INITIAL_LIGHT_STACK_SIZE  128
#define VS_PHY_USER_CONST_MAX        256
#define VS_MAX_STREAM                8
#define VS_MAX_AMBIENT_COLORS        2
#define VS_MAX_LIGHTSET_STACK_SIZE   4096

#define VS_MAX_WYB_STACK_SIZE        64

#define VS_MAX_SYMMETRY_STACK_SIZE 128

// Bones Config
#define VS_BONES_CONFIG_1       0
#define VS_BONES_CONFIG_12      1
#define VS_BONES_CONFIG_24      2
#define VS_BONES_CONFIG_36      3
#define VS_BONES_CONFIG_48      4
#define VS_BONES_MAX_CONFIG     8

// Lights Config
#define VS_LIGHTS_CONFIG_55     0
#define VS_LIGHTS_CONFIG_44     1
#define VS_LIGHTS_CONFIG_32     2
#define VS_LIGHTS_CONFIG_20     3
#define VS_LIGHTS_CONFIG_8      4
#define VS_LIGHTS_MAX_CONFIG    8

struct XeVSCustomShaderInfo
{
    const CHAR* szFileName;
};

struct VertexShaderKey
{
    VertexShaderKey( const ULONG128Holder & _ulFeatureKey, ULONG64 _ulVSOuputSemantics ) : ulFeatureKey(_ulFeatureKey), ulVSOuputSemantics(_ulVSOuputSemantics){}

    ULONG128Holder  ulFeatureKey;
    ULONG64         ulVSOuputSemantics;

    bool operator<  (const VertexShaderKey & rhs) const 
    {
        if( ulFeatureKey == rhs.ulFeatureKey)
        {
            return ulVSOuputSemantics < rhs.ulVSOuputSemantics;
        }
        return ulFeatureKey < rhs.ulFeatureKey;
    }

    bool operator==  (const VertexShaderKey & rhs) const 
    {
        return ( (ulFeatureKey == rhs.ulFeatureKey) && (ulVSOuputSemantics == rhs.ulVSOuputSemantics) );
    }
};

extern const XeVSCustomShaderInfo XEVS_CUSTOM_SHADERS[];
extern const ULONG                XEVS_NB_CUSTOM_SHADERS;

union XeCustomShaderFeatures
{
	struct
	{
		ULONG64 ulCustomShaderDup           : 8;
		ULONG64 ulFeature1                  : 3;
		ULONG64 ulFeature2                  : 3;
		ULONG64 ulFeature3                  : 3;
		ULONG64 ulFeature4                  : 3;
		ULONG64 ulFeature5                  : 3;
		ULONG64 ulFeature6                  : 3;
		ULONG64 ulFeature7                  : 3;
		ULONG64 ulFeature8                  : 3;
		ULONG64 ulFeature9                  : 3;
		ULONG64 ulFeature10                 : 3;
		ULONG64 ulFeature11                 : 3;
		ULONG64 ulFeature12                 : 3;
		ULONG64 ulFeature13                 : 3;
		ULONG64 ulFeature14                 : 3;
		ULONG64 ulFeature15                 : 3;
		//ULONG64 ulFeature16                 : 3;

#if defined(_XENON_RENDER_PC)
		ULONG64 ulReserved                  : 8;

		// Making sure some shaders will not be compiled with invalid components
		// since the PC implementation of DirectX 9 is more critical about those
		// WARNING: Those components are duplicates from the  previous structure
		ULONG64 bVertexHasColorDup          : 1;
		ULONG64 bVertexHasNormalDup         : 1;
		ULONG64 bVertexHasBaseTexCoordDup   : 1;
#else
		ULONG64 ulReserved                  : 11;
#endif
	};

	ULONG64 ul_RawFeatures;

	void Clear(void) { ul_RawFeatures = 0; }
};

class XeCustomShader
{

public:

    M_DeclareOperatorNewAndDelete();

    XeCustomShader(void) { m_stVSFeatures.Clear(); m_stPSFeatures.Clear(); }
    virtual ~XeCustomShader(void) { }

	inline ULONG GetCustomVS(void) const
	{
		return m_stVSFeatures.ulCustomShaderDup;
	}

	inline ULONG64 GetCustomVSFeatures() const
	{
		return m_stVSFeatures.ul_RawFeatures;
	}

	inline ULONG GetCustomVSFeature(ULONG _ulFeature) const
	{
		switch (_ulFeature)
		{
		case 1:  return m_stVSFeatures.ulFeature1;
		case 2:  return m_stVSFeatures.ulFeature2;
		case 3:  return m_stVSFeatures.ulFeature3;
		case 4:  return m_stVSFeatures.ulFeature4;
		case 5:  return m_stVSFeatures.ulFeature5;
		case 6:  return m_stVSFeatures.ulFeature6;
		case 7:  return m_stVSFeatures.ulFeature7;
		case 8:  return m_stVSFeatures.ulFeature8;
        case 9:  return m_stVSFeatures.ulFeature9;
        case 10:  return m_stVSFeatures.ulFeature10;
        case 11:  return m_stVSFeatures.ulFeature11;
        case 12:  return m_stVSFeatures.ulFeature12;
        case 13:  return m_stVSFeatures.ulFeature13;
        case 14:  return m_stVSFeatures.ulFeature14;
        case 15:  return m_stVSFeatures.ulFeature15;
		//case 16:  return m_stVSFeatures.ulFeature16;

        default: ERR_X_Assert(FALSE && "Invalid vertex shader feature index");
		}

		return 0;
	}

	inline void SetCustomVS(ULONG64 _ulCustomVS)
	{
		m_stVSFeatures.Clear();
		m_stVSFeatures.ulCustomShaderDup = _ulCustomVS;
	}

	inline void SetCustomVSFeature(ULONG _ulFeature, ULONG _ulVal)
	{
		switch (_ulFeature)
		{
		case 1:  m_stVSFeatures.ulFeature1 = _ulVal; break;
		case 2:  m_stVSFeatures.ulFeature2 = _ulVal; break;
		case 3:  m_stVSFeatures.ulFeature3 = _ulVal; break;
		case 4:  m_stVSFeatures.ulFeature4 = _ulVal; break;
		case 5:  m_stVSFeatures.ulFeature5 = _ulVal; break;
		case 6:  m_stVSFeatures.ulFeature6 = _ulVal; break;
		case 7:  m_stVSFeatures.ulFeature7 = _ulVal; break;
		case 8:  m_stVSFeatures.ulFeature8 = _ulVal; break;
        case 9:  m_stVSFeatures.ulFeature9 = _ulVal; break;
        case 10:  m_stVSFeatures.ulFeature10 = _ulVal; break;
        case 11:  m_stVSFeatures.ulFeature11 = _ulVal; break;
        case 12:  m_stVSFeatures.ulFeature12 = _ulVal; break;
        case 13:  m_stVSFeatures.ulFeature13 = _ulVal; break;
        case 14:  m_stVSFeatures.ulFeature14 = _ulVal; break;
        case 15:  m_stVSFeatures.ulFeature15 = _ulVal; break;
		//case 16:  m_stVSFeatures.ulFeature16 = _ulVal; break;
        default: ERR_X_Assert(FALSE && "Invalid vertex shader feature index");
		}
	}

	inline void SetCustomPS(ULONG _ulCustomPS)
	{
		m_stPSFeatures.Clear();
		m_stPSFeatures.ulCustomShaderDup = _ulCustomPS;
	}

	inline void SetCustomPSFeature(ULONG _ulFeature, ULONG _ulVal)
	{
		switch (_ulFeature)
		{
		case 1:  m_stPSFeatures.ulFeature1 = _ulVal; break;
		case 2:  m_stPSFeatures.ulFeature2 = _ulVal; break;
		case 3:  m_stPSFeatures.ulFeature3 = _ulVal; break;
		case 4:  m_stPSFeatures.ulFeature4 = _ulVal; break;
		case 5:  m_stPSFeatures.ulFeature5 = _ulVal; break;
		case 6:  m_stPSFeatures.ulFeature6 = _ulVal; break;
		case 7:  m_stPSFeatures.ulFeature7 = _ulVal; break;
		case 8:  m_stPSFeatures.ulFeature8 = _ulVal; break;
		case 9:  m_stPSFeatures.ulFeature9 = _ulVal; break;
		case 10:  m_stPSFeatures.ulFeature10 = _ulVal; break;
		case 11:  m_stPSFeatures.ulFeature11 = _ulVal; break;
		case 12:  m_stPSFeatures.ulFeature12 = _ulVal; break;
		case 13:  m_stPSFeatures.ulFeature13 = _ulVal; break;
		case 14:  m_stPSFeatures.ulFeature14 = _ulVal; break;
		case 15:  m_stPSFeatures.ulFeature15 = _ulVal; break;
		//case 16:  m_stPSFeatures.ulFeature16 = _ulVal; break;
        default: ERR_X_Assert(FALSE && "Invalid pixel shader feature index");
		}
	}

	inline ULONG GetCustomPS(void) const
	{
		return m_stPSFeatures.ulCustomShaderDup;
	}

	inline ULONG GetCustomPSFeature(ULONG _ulFeature) const
	{
		switch (_ulFeature)
		{
		case 1:  return m_stPSFeatures.ulFeature1;
		case 2:  return m_stPSFeatures.ulFeature2;
		case 3:  return m_stPSFeatures.ulFeature3;
		case 4:  return m_stPSFeatures.ulFeature4;
		case 5:  return m_stPSFeatures.ulFeature5;
		case 6:  return m_stPSFeatures.ulFeature6;
		case 7:  return m_stPSFeatures.ulFeature7;
		case 8:  return m_stPSFeatures.ulFeature8;
		case 9:  return m_stPSFeatures.ulFeature9;
		case 10:  return m_stPSFeatures.ulFeature10;
		case 11:  return m_stPSFeatures.ulFeature11;
		case 12:  return m_stPSFeatures.ulFeature12;
		case 13:  return m_stPSFeatures.ulFeature13;
		case 14:  return m_stPSFeatures.ulFeature14;
		case 15:  return m_stPSFeatures.ulFeature15;
		//case 16:  return m_stPSFeatures.ulFeature16;
        default: ERR_X_Assert(FALSE && "Invalid pixel shader feature index");
		}

		return 0;
	}

private:

	XeCustomShaderFeatures	 m_stVSFeatures;
	XeCustomShaderFeatures   m_stPSFeatures;
};

typedef struct 
{
    union
    {
        struct 
        {
            float   m_fHeightWorldMin;
            float   m_fHeightWorldMax;
            float   m_fHeightAttenuationMin;
            float   m_fHeightAttenuationMax;
        };
        VECTOR4FLOAT m_vHeightAttenuation;
    };

    BOOL    m_isHeightAttenuationEnabled;
}tdstRimLightVSConsts;

typedef struct 
{
    VECTOR4FLOAT    m_vCenter;
    union 
    {
        struct
        {
            float           m_fWidth;
            float           m_fHeight;
            float           m_fSizeU;
            float           m_fSizeV;
        };
        VECTOR4FLOAT    m_vShape;
    };

    union 
    {
        struct
        {
            float           m_fScrollV;
            float           m_fBottomScale;
            float           m_fIntensity;
            float           dummy;
        };
        VECTOR4FLOAT    m_vParams;
    };
    ULONG m_ulProjectionMatrixIndex;
} tdstHeatShimmerVSConsts;

typedef struct 
{ 
    int             m_iInputRegion;
    int             m_iOutputRegion;
    bool            m_bBlurHorizontal;
} tdstAEGlowVSConsts;


// XeVertexShaderManager class definition
class XeVertexShaderManager
{
public:

    union XeVSFeature
    {
        struct
        {
            // SC: Please increment XE_SHADER_DB_VERSION (XeShaderDatabase.cpp) for any feature change

            ULONG64 ulCustomShader              : 8;    // See CUSTOMSHADER_*
            ULONG64 ulTransform                 : 2;    // See TRANSFORM_*
            ULONG64 ulColorSource               : 3;    // See COLORSOURCE_*
            ULONG64 ulLighting                  : 2;    // See LIGHTING_*
            ULONG64 ulDirLightCount             : 3;    // type of light0
            ULONG64 ulOmniLightCount            : 3;    // type of light1
            ULONG64 ulSpotLightCount            : 3;    // type of light2
            ULONG64 ulCylSpotLightCount         : 3;    // type of light3
            ULONG64 ulUVSource                  : 2;    // See UVSOURCE_*
            ULONG64 ulBaseMapTransform          : 3;    // See TEXTRANSFORM_*
            ULONG64 ulNormalMapTransform        : 2;    // See TEXTRANSFORM_*
            ULONG64 ulSpecularMapTransform      : 2;    // See TEXTRANSFORM_*
            ULONG64 ulDetailNMapTransform       : 3;    // See TEXTRANSFORM_*
            ULONG64 bFogEnable                  : 1;    // Enable fog
            ULONG64 bEnvMap                     : 1;    // Enable environment map
            ULONG64 bShadowTexCoord             : 1;    // Enable shadow texture coordinates output
            ULONG64 bAddAmbient                 : 1;    // add ambient lighting
            ULONG64 bAddRLI                     : 1;    // add RLI lighting
            ULONG64 ulReflectionType            : 2;    // reflection type
            ULONG64 ulBonesConfig               : 3;    // Bones configuration
            ULONG64 ulLightsConfig              : 3;    // Lights configuration
            ULONG64 bRimLightEnable             : 1;    // Rim Lights
            ULONG64 bRimLightHeightAttenuation  : 1;    // Rim Light Height Attenuation
            ULONG64 bDepthToColorEnable         : 1;    // Depth to color
            ULONG64 bRLIScaleAndOffset          : 1;    // RLI scale/offset
            ULONG64 bInvertMossMode             : 1;    // Invert the moss/no moss distribution
			ULONG64 bOffsetMap		           : 1;
            ULONG64 iSPG2Mode                   : 3;    // SPG2 (used by shader mat for preprocess)
            ULONG64 bShadows                    : 1;    // Vertex Lighting and shadows
            ULONG64 bWaveYourBody               : 1;    // Wave Your Body modifier
            ULONG64 ulSymmetryMode              : 2;    // Symmetry modifier
            ULONG64 bVertexSpecular             : 1;    // Vertex lighting specular
            ULONG64 ulNumWYB                    : 2;
#if defined(_XENON_RENDER_PC)
            ULONG64 ulReserved                  : 29;

            // Making sure some shaders will not be compiled with invalid components
            // since the PC implementation of DirectX 9 is more critical about those
            // WARNING: Those components are duplicated in the next structure
            ULONG64 bVertexHasColor             : 1;
            ULONG64 bVertexHasNormal            : 1;
            ULONG64 bVertexHasBaseTexCoord      : 1;
#else
            ULONG64 ulReserved                  : 30;
#endif
        };

        XeCustomShaderFeatures stCustomShaderFeatures;

        ULONG128Holder ulKey;

        void Clear(void) { ulKey.part1 = 0; ulKey.part2 = 0; };
    };

    struct XeVertexShaderEntry
    {
        TCHAR               FileName[MAX_PATH];
        XeVSFeature         Key;
    };

    union XeVertexAttribute
    {
        struct
        {
            ULONG ulTransformType   : 2;
            ULONG bHasColor         : 1;
            ULONG bHasNormal        : 1;
            ULONG bHasBaseTexCoord  : 1;
            ULONG ulReserved        : 26;
        };
        ULONG ulRaw;

        void Clear(void) { ulRaw = 0; };
    };

    struct XeVertexDeclaration
    {
        ULONG                        aStreamComponents[VS_MAX_STREAM];
        XeVertexAttribute            eAttributes;
        IDirect3DVertexDeclaration9* pDeclaration;
    };


    union VSConstantDirtyFlags {
        struct {
            DWORD bMaxWeight        :  1;
            DWORD bAmbientColor     :  1;
            DWORD uReserved         : 30;
        };
        DWORD dwRawFlags;

        void Clear() { dwRawFlags = 0; }
        void DirtyAll() { dwRawFlags = 0xFFFFFFFF; }
    };

public:

    XeVertexShaderManager();
    ~XeVertexShaderManager();
    void        ReInit();

    BOOL OnDeviceLost(void);
    BOOL OnDeviceReset(void);
    void Shutdown(void);

    // Shader management
    void        InitShadersTable(void);
    void        UnloadAll(void);
    void        UnloadAllShaders(void);
    BOOL        GenerateShader(XeRenderObject *_poObject, XeVSFeature _oFeatures, ULONG _ulVertexFormatHandle, ULONG64 _ulOutputSemantics);
 
    void AddShader(const VertexShaderKey& _ulKey, XeVertexShader* _pShader);
    void AddCreateShader(const VertexShaderKey& _ulKey, const void* _pCode, ID3DXConstantTable* _pConstantTable);

    // Features
    inline XeVSFeature GetCurrentFeatures()                     { return m_oFeatures; }
    inline void        SetCurrentFeatures(XeVSFeature _oFeatures){ m_oFeatures = _oFeatures; }
    inline void ResetAllFeatures()                              { m_oFeatures.Clear(); }
    inline void SetFeatureCustomShader(ULONG _ulShader)         { m_oFeatures.ulCustomShader         = _ulShader; }
    inline void SetFeatureTransform(ULONG _ulTransform)         { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulTransform            = _ulTransform; }
    inline void SetFeatureColorSource(ULONG _ulColorSource)     { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulColorSource          = _ulColorSource; }
    inline void SetFeatureLighting(ULONG _ulLighting)           { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulLighting             = _ulLighting; }
    inline void SetFeatureUVSource(ULONG _ulUVSource)           { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulUVSource             = _ulUVSource; }
    inline void SetFeatureFogEnable(BOOL _bEnable)              { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bFogEnable             = _bEnable; }
    inline void SetFeatureEnvMap(BOOL _bEnable)                 { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bEnvMap                = _bEnable; }
    inline void SetFeatureShadowTexCoord(BOOL _bEnable)         { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bShadowTexCoord        = _bEnable; }
    inline void SetFeatureBaseMapTransform(ULONG _ulType)       { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulBaseMapTransform     = _ulType; }
    inline void SetFeatureNormalMapTransform(ULONG _ulType)     { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulNormalMapTransform   = _ulType; }
    inline void SetFeatureSpecularMapTransform(ULONG _ulType)   { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulSpecularMapTransform = _ulType; }
    inline void SetFeatureDetailNMapTransform(ULONG _ulType)    { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulDetailNMapTransform  = _ulType; }
    inline void SetFeatureAddAmbient(BOOL _bEnable)             { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bAddAmbient            = _bEnable; }
    inline void SetFeatureAddRLI(BOOL _bEnable)                 { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bAddRLI                = _bEnable; }
    inline void SetFeatureReflectionType(ULONG _ulType)         { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulReflectionType       = m_bReflectionEnabled ? _ulType : REFLECTION_TYPE_NONE; }
    inline void SetFeatureBonesConfig(ULONG _ulMaxBones)        { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulBonesConfig          = GetMaxHWBonesConfig(_ulMaxBones); }
    inline void SetFeatureLightsConfig(ULONG _ulMaxBones)       { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.ulLightsConfig         = GetMaxHWLightsConfig(_ulMaxBones); }
    inline void SetFeatureRimLightEnable(ULONG _bEnable)        { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bRimLightEnable        = _bEnable; }
    inline void SetFeatureRimLightHeightAttenuation(BOOL _bEnable) { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bRimLightHeightAttenuation    = _bEnable; }
    inline void SetFeatureDepthToColorEnable(BOOL _bEnable)     { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bDepthToColorEnable    = _bEnable; }
    inline void SetFeatureRLIScaleAndOffset(BOOL _bEnable)      { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bRLIScaleAndOffset      = _bEnable; }
    inline void SetFeatureInvertMossMode(BOOL _bEnable)         { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bInvertMossMode        = _bEnable; }
	inline void SetFeatureOffsetMapping(BOOL _bEnable)          { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bOffsetMap		      = _bEnable; }
	inline void SetFeatureSPG2Mode(int _iSPG2Mode)              { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.iSPG2Mode              = _iSPG2Mode; }
    inline void SetFeatureShadows(BOOL _bEnable)                { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bShadows               = _bEnable; }
    inline void SetFeatureVertexSpecular(BOOL _bEnable)         { if (m_oFeatures.ulCustomShader == 0) m_oFeatures.bVertexSpecular        = _bEnable; }

	inline void SetFeatureCustom(ULONG _ulFeature, ULONG _ulVal);
	inline void SetFeaturesCustom(ULONG64 _ulVal)				{ if (m_oFeatures.ulCustomShader != 0) m_oFeatures.stCustomShaderFeatures.ul_RawFeatures  = _ulVal;	}

    // Constant access
    inline void DirtyAllConstants() { m_oDirtyFlags.DirtyAll(); }

    // skinning
    inline UINT         GetMaxHWBonesConfig(UINT iMaxBones);
    inline void         SetMaxWeight(INT iMaxWeight);

    inline D3DXMATRIX* GetWorldViewProjMatrix() { return &m_oWorldViewProj; };

    // fog
    //inline void SetFogParams( const VECTOR4FLOAT & _vFogParams, const VECTOR4FLOAT & _vFogPitchAttenuation );

    inline BOOL HasColorInFormat(ULONG _ulVertexFormatHandle);
    inline BOOL HasNormalInFormat(ULONG _ulVertexFormatHandle);
    inline BOOL HasBaseTexCoordInFormat(ULONG _ulVertexFormatHandle);

    // lighting
    inline UINT     GetMaxHWLightsConfig(UINT iMaxBones);
    inline UINT     GetMaxHWLights(UINT iMaxBones);
    inline void     SetAmbientColor(ULONG _ulSlot, ULONG ulColor);
    inline void     SetActiveAmbientColor(ULONG _ulSlot);
    inline VECTOR4FLOAT* GetCurrentAmbientColor() { return &m_oAmbientColor[m_ulActiveAmbient]; }

    inline INT      GetCurrentPrimitiveInstance() { return m_iDrawPrimitiveInstance; }
    inline void     SetCurrentPrimitiveInstance(int _idx) { m_iDrawPrimitiveInstance=_idx; }

    inline void     SetBaseLightIndex(INT _iIndex) { m_iLightBase = _iIndex; }
    inline void     GetLightCountByType(XeLightSet *_pLightSet, 
                                        ULONG _ulBaseLightIndex,
                                        ULONG _ulRemainingLights, 
                                        ULONG &_ulDirLightCount, 
                                        ULONG &_ulOmniLightCount, 
                                        ULONG &_ulSpotLightCount, 
                                        ULONG &_ulCylSpotLightCount);
    
    // Reflection
    inline void     SetReflectionPlane(FLOAT _A, FLOAT _B, FLOAT _C, FLOAT _D);
    inline void     SetReflectionPlane(D3DXVECTOR3 *_poNormal, D3DXVECTOR3 *_poOrigin);
    inline VECTOR4FLOAT* GetReflectionPlane() { return &m_oReflectionPlane; }
    inline void     EnableReflection(BOOL _bEnable) { m_bReflectionEnabled = _bEnable; }
    inline BOOL     IsReflectionEnabled() { return m_bReflectionEnabled; }

    void            SetFeaturePPLightCountByType(ULONG _ulLightCount);

    // Rim Light
    void            SetRimLightShaderConsts( const tdstRimLightVSConsts & _rimLightShaderConsts ) { m_stRimLightShaderConsts = _rimLightShaderConsts; }
    const tdstRimLightVSConsts & GetRimLightShaderConsts( ) { return m_stRimLightShaderConsts; }

    void            SetAEGlowShaderConsts( const tdstAEGlowVSConsts & _stAEGlowConsts ) { m_stAEGlowConsts =_stAEGlowConsts; } 
    const tdstAEGlowVSConsts & GetAEGlowShaderConsts( ) { return m_stAEGlowConsts; }

    void            SetZoomBlurStepConsts( INT iStepNbr,
                                           float fAlpha, 
                                           float fUScale,
                                           float fUOffset,
                                           float fVScale,
                                           float fVOffset );
    void            SetZoomBlurRatios( float fInputRatio, float fOutputRatio );

    void            SetGodRayAdjust( const VECTOR4FLOAT & _vGodRayAdjust ) { m_vGodRayAdjust = _vGodRayAdjust; }

    void            SetBigBlurFactor( float _fFactor )  { m_fBigBlurFactor = _fFactor; }
    float           GetBigBlurFactor()                  { return  m_fBigBlurFactor; }

    void            IgnoreHalfViewportTexel( BOOL _bIgnore ) { m_ignoreHalfViewportTexel = _bIgnore; }

    BOOL            IsLightCreatingRim  ( ULONG ulCurRim );
    BOOL            IsRimLightEnabled   ( ) { return m_isRimLightEnabled; }
    void            RimLightEnable      ( BOOL _bEnable ) { m_isRimLightEnabled = _bEnable; }

    void            SetBorderBrightness ( const VECTOR4FLOAT & _vBorderBrightness) { m_vBorderBrightness = _vBorderBrightness; }
    void            SetSpinSmoothTransform( const D3DXMATRIX & _mSpinSmoothTransform ) { m_mSpinSmoothTransform = _mSpinSmoothTransform; }
    
    // Declaration management
    XeVertexDeclaration* GetVertexDeclaration(XeMesh* _pstMesh);
    
	// update functions
	void        Update(XeRenderObject *_pObject);
    void        UpdateShaderConstants(XeVertexShader *_pShader, XeRenderObject* _pObject);
    void        UpdateShaderConstantsSPG2(XeVertexShader *_pShader, XeRenderObject* _pObject, BOOL bSet);
    void        UpdateShaderConstantsFur(XeVertexShader *_pShader);
    void        UpdateLightConstantsForVertexLighting(XeVertexShader *_pShader, XeRenderObject *_pObject);
    void        UpdateLightConstantsForPixelLighting(XeVertexShader *_pShader, XeRenderObject *_pObject);
    
    inline void InvalidateFeatureKey(void);

    void PreLoadDeclaration(ULONG _ulIndex, void* _pContent);
    void PreLoadShader(const VertexShaderKey & _ulKey, void* _pCode);

    typedef std::map<VertexShaderKey, XeVertexShader*>              MapVertexShader;
    typedef std::vector<XeVertexDeclaration>                        VertexDeclarationArray;
    typedef std::vector<XeVertexShaderManager::XeVertexShaderEntry> VertexShaderArray;
    
private:

    ULONG           CreateDeclaration(ULONG _ulNbStreams, ULONG* _pulComponents, ULONG _ulIndex);
    XeVertexShader* CreateShader(const VertexShaderKey& _ulKey, const void* _pShaderCode, ID3DXConstantTable* _pConstantTable, CHAR* _szName);

private:
  
    LPDIRECT3DDEVICE9   m_pD3DDevice;
    MapVertexShader     m_mapLoadedShaders;

    // Members
    VSConstantDirtyFlags m_oDirtyFlags;

    // Currently selected features
    XeVSFeature         m_oFeatures;
    VertexShaderKey     m_oCurrentKey;

    // Streams
    VertexDeclarationArray m_aoVertexDeclarations;

    // Matrices
    D3DXMATRIX          m_oWorldViewProj;

    // Lighting    
    VECTOR4FLOAT        m_oAmbientColor[VS_MAX_AMBIENT_COLORS];
    ULONG               m_ulActiveAmbient;
    
    INT                 m_iLightBase;

    // Skinning
    INT                 m_iMaxWeight;

    // Reflection
    BOOL                m_bReflectionEnabled;
    VECTOR4FLOAT        m_oReflectionPlane;
    VECTOR4FLOAT        m_oReflectionPlaneOrigin;

	// misc
	INT					m_iDrawPrimitiveInstance;

	// custom shader
    tdstRimLightVSConsts        m_stRimLightShaderConsts;
    tdstAEGlowVSConsts          m_stAEGlowConsts;
    float                       m_fBigBlurFactor;

    // Zoom blur
    float               m_fZoomBlurPassAlpha[ZOOM_BLUR_MAX_STEPS_PER_PASS];
    VECTOR4FLOAT        m_vZoomBlurUVScale[ZOOM_BLUR_MAX_STEPS_PER_PASS];
    VECTOR4FLOAT        m_vZoomBlurUVTranslation[ZOOM_BLUR_MAX_STEPS_PER_PASS];
    VECTOR4FLOAT        m_vZoomBlurRatios;
    VECTOR4FLOAT        m_vGodRayAdjust;

    VECTOR4FLOAT        m_vBorderBrightness;
    D3DXMATRIX          m_mSpinSmoothTransform;
   
    BOOL                m_ignoreHalfViewportTexel;
    BOOL                m_isRimLightEnabled;
};

extern __declspec(align(32)) XeVertexShaderManager g_oVertexShaderMgr;

// ------------------------------------------------------------------------------------------------
// Name   : 
// Params : 
// RetVal :
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeVertexShaderManager::SetMaxWeight(INT iMaxWeight)
{
    if (m_iMaxWeight != iMaxWeight)
    {
        m_iMaxWeight = iMaxWeight;
        m_oDirtyFlags.bMaxWeight = TRUE;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::HasColorInFormat
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline BOOL XeVertexShaderManager::HasColorInFormat(ULONG _ulVertexFormatHandle)
{
    if (_ulVertexFormatHandle >= m_aoVertexDeclarations.size())
        return FALSE;

    return m_aoVertexDeclarations[_ulVertexFormatHandle].eAttributes.bHasColor;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::HasNormalInFormat
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline BOOL XeVertexShaderManager::HasNormalInFormat(ULONG _ulVertexFormatHandle)
{
    if (_ulVertexFormatHandle >= m_aoVertexDeclarations.size())
        return FALSE;

    return m_aoVertexDeclarations[_ulVertexFormatHandle].eAttributes.bHasNormal;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::HasBaseTexCoordInFormat
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline BOOL XeVertexShaderManager::HasBaseTexCoordInFormat(ULONG _ulVertexFormatHandle)
{
    if (_ulVertexFormatHandle >= m_aoVertexDeclarations.size())
        return FALSE;

    return m_aoVertexDeclarations[_ulVertexFormatHandle].eAttributes.bHasBaseTexCoord;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::SetAmbientColor
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::SetAmbientColor(ULONG _ulSlot, ULONG _ulColor)
{
    ERR_X_Assert(_ulSlot < VS_MAX_AMBIENT_COLORS);

    D3DXCOLOR oNewColor = _ulColor;
    m_oAmbientColor[_ulSlot].x = oNewColor.r;
    m_oAmbientColor[_ulSlot].y = oNewColor.g;
    m_oAmbientColor[_ulSlot].z = oNewColor.b;
    m_oAmbientColor[_ulSlot].w = oNewColor.a;

    m_oDirtyFlags.bAmbientColor = TRUE;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::SetActiveAmbientColor
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::SetActiveAmbientColor(ULONG _ulSlot)
{
    ERR_X_Assert(_ulSlot < VS_MAX_AMBIENT_COLORS);

    if (m_ulActiveAmbient != _ulSlot)
    {
        m_ulActiveAmbient = _ulSlot;
        m_oDirtyFlags.bAmbientColor = TRUE;
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::SetFeatureCustom
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline void XeVertexShaderManager::SetFeatureCustom(ULONG _ulFeature, ULONG _ulVal)
{
    if (m_oFeatures.ulCustomShader != 0)
    {
        switch (_ulFeature)
        {
            case 1: m_oFeatures.stCustomShaderFeatures.ulFeature1 = _ulVal; break;
            case 2: m_oFeatures.stCustomShaderFeatures.ulFeature2 = _ulVal; break;
            case 3: m_oFeatures.stCustomShaderFeatures.ulFeature3 = _ulVal; break;
            case 4: m_oFeatures.stCustomShaderFeatures.ulFeature4 = _ulVal; break;
            case 5: m_oFeatures.stCustomShaderFeatures.ulFeature5 = _ulVal; break;
            case 6: m_oFeatures.stCustomShaderFeatures.ulFeature6 = _ulVal; break;
            case 7: m_oFeatures.stCustomShaderFeatures.ulFeature7 = _ulVal; break;
            case 8: m_oFeatures.stCustomShaderFeatures.ulFeature8 = _ulVal; break;
            case 9: m_oFeatures.stCustomShaderFeatures.ulFeature9 = _ulVal; break;
            case 10: m_oFeatures.stCustomShaderFeatures.ulFeature10 = _ulVal; break;
            case 11: m_oFeatures.stCustomShaderFeatures.ulFeature11 = _ulVal; break;
            case 12: m_oFeatures.stCustomShaderFeatures.ulFeature12 = _ulVal; break;
            case 13: m_oFeatures.stCustomShaderFeatures.ulFeature13 = _ulVal; break;
            case 14: m_oFeatures.stCustomShaderFeatures.ulFeature14 = _ulVal; break;
            case 15: m_oFeatures.stCustomShaderFeatures.ulFeature15 = _ulVal; break;
			//case 16: m_oFeatures.stCustomShaderFeatures.ulFeature16 = _ulVal; break;
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::Xe_IsForcingSWSkinning
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
inline BOOL Xe_IsForcingSWSkinning(void)
{
#if defined(VS_FORCE_SW_SKINNING)
    return TRUE;
#else
    return FALSE;
#endif
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::GetMaxHWBonesConfig
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
UINT XeVertexShaderManager::GetMaxHWBonesConfig(UINT uiMaxBones)
{
    UINT uiMaxUsedBones;

    if ( uiMaxBones == 0)
        uiMaxUsedBones = VS_BONES_CONFIG_1;
    else if ( uiMaxBones <= 12 )
        uiMaxUsedBones = VS_BONES_CONFIG_12;
    else if ( uiMaxBones <= 24 )
        uiMaxUsedBones = VS_BONES_CONFIG_24;
    else if ( uiMaxBones <= 36 )
        uiMaxUsedBones = VS_BONES_CONFIG_36;
    else
        uiMaxUsedBones = VS_BONES_CONFIG_48;

    return uiMaxUsedBones;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::GetMaxHWLightsConfig
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
UINT XeVertexShaderManager::GetMaxHWLightsConfig(UINT uiMaxBones)
{
    UINT uiMaxUsedLights;

  #ifndef _XENON
    if ( uiMaxBones == 0)
        uiMaxUsedLights = VS_LIGHTS_CONFIG_55;
    else if ( uiMaxBones <= 12 )
        uiMaxUsedLights = VS_LIGHTS_CONFIG_44;
    else if ( uiMaxBones <= 24 )
        uiMaxUsedLights = VS_LIGHTS_CONFIG_32;
    else if ( uiMaxBones <= 36 )
        uiMaxUsedLights = VS_LIGHTS_CONFIG_20;
    else
  #endif
        uiMaxUsedLights = VS_LIGHTS_CONFIG_8;

    return uiMaxUsedLights;
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::GetMaxHWLights
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
UINT XeVertexShaderManager::GetMaxHWLights(UINT uiMaxBones)
{
    UINT ulConfig = GetMaxHWLightsConfig(uiMaxBones);

    static const UINT iMaxNbrConfig = 5;
    static UINT iConfig2Nbr[iMaxNbrConfig] = { 55, 44, 32, 20, 8 };

    ERR_X_Assert( ulConfig < iMaxNbrConfig );
    return iConfig2Nbr[ulConfig];
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::SetReflectionPlane
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::SetReflectionPlane(FLOAT _A, FLOAT _B, FLOAT _C, FLOAT _D)
{
    // normalize plane equation
    FLOAT fScale = 1.0f  / (FLOAT) L_sqrt(_A * _A + _B * _B + _C * _C);

    m_oReflectionPlane.x = _A * fScale;
    m_oReflectionPlane.y = _B * fScale;
    m_oReflectionPlane.z = _C * fScale;
    m_oReflectionPlane.w = _D * fScale;

    if (_A != 0.0f)
    {
        m_oReflectionPlaneOrigin.x = -_D / _A;
        m_oReflectionPlaneOrigin.y = 0.0f;
        m_oReflectionPlaneOrigin.z = 0.0f;
    }
    else if(_B != 0.0f)
    {
        m_oReflectionPlaneOrigin.x = 0.0f;
        m_oReflectionPlaneOrigin.y = -_D / _B;
        m_oReflectionPlaneOrigin.z = 0.0f;
    }
    else if (_C != 0.0f)
    {
        m_oReflectionPlaneOrigin.x = 0.0f;
        m_oReflectionPlaneOrigin.y = 0.0f;
        m_oReflectionPlaneOrigin.z = -_D / _C;
    }
    else
    {
        // Invalid plane equation
        ERR_X_Assert(FALSE);
    }
}

// ------------------------------------------------------------------------------------------------
// Name   : XeVertexShaderManager::SetReflectionPlane
// Params : 
// RetVal : 
// Descr. : 
// ------------------------------------------------------------------------------------------------
void XeVertexShaderManager::SetReflectionPlane(D3DXVECTOR3 *_poNormal, D3DXVECTOR3 *_poOrigin)
{
    // normalize plane equation
    FLOAT fScale = 1.0f  / (FLOAT) L_sqrt(_poNormal->x * _poNormal->x + _poNormal->y * _poNormal->y + _poNormal->z * _poNormal->z);

    m_oReflectionPlane.x = _poNormal->x * fScale;
    m_oReflectionPlane.y = _poNormal->y * fScale;
    m_oReflectionPlane.z = _poNormal->z * fScale;
    m_oReflectionPlane.w = -m_oReflectionPlane.x*_poOrigin->x -m_oReflectionPlane.y*_poOrigin->y -m_oReflectionPlane.z*_poOrigin->z;

    m_oReflectionPlaneOrigin.x = _poOrigin->x;
    m_oReflectionPlaneOrigin.y = _poOrigin->y;
    m_oReflectionPlaneOrigin.z = _poOrigin->z;
}

inline void XeVertexShaderManager::InvalidateFeatureKey(void)
{
    m_oCurrentKey.ulVSOuputSemantics = (ULONG32)-1;
    m_oCurrentKey.ulFeatureKey.part1 = (ULONG64)-1;
    m_oCurrentKey.ulFeatureKey.part2 = (ULONG64)-1;
}

#endif //XEVERTEXSHADERMGR_HEADER
