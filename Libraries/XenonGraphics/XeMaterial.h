// ------------------------------------------------------------------------------------------------
// File   : XeMaterial.h
// Date   : 2004-12-21
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XEMATERIAL_H
#define GUARD_XEMATERIAL_H

// ------------------------------------------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------------------------------------------
#include <vector>
#include "XeSharedDefines.h"
#include "XeVertexShaderManager.h"
#include "XeUtils.h"

// ------------------------------------------------------------------------------------------------
// FORWARD DECLARATIONS
// ------------------------------------------------------------------------------------------------
struct MAT_tdst_MultiTexture_;
struct MAT_tdst_MTLevel_;

struct XeMaterialProperties;
struct XeMaterialConstants;
struct XeMaterialStageProperties;

class XeMaterial;

// ------------------------------------------------------------------------------------------------
// STRUCTURES
// ------------------------------------------------------------------------------------------------

union XeMaterialRS
{
    struct
    {
        ULONG b_AlphaTestEnable  : 1;
        ULONG b_InvertAlpha      : 1;
        ULONG b_UsingLocalAlpha  : 1;
        ULONG b_AlphaBlendEnable : 1;
        ULONG b_AlphaWriteEnable : 1;
        ULONG b_ColorWriteEnable : 1;
        ULONG b_ZEnable          : 1;
        ULONG b_ZWriteEnable     : 1;
        ULONG b_TwoSided         : 1;
        ULONG b_StencilEnabled   : 1;
        ULONG b_ReflectOnWater   : 1;
        ULONG ul_Unused          : 21;
    };
    ULONG ul_RawFlags;

    inline void ClearAll(void) { ul_RawFlags = 0; };
};

union XeMaterialSS
{
    struct
    {
        ULONG ul_AddressU           : 3;
        ULONG ul_AddressV           : 3;
        ULONG ul_AddressW           : 3;
        ULONG ul_MagFilter          : 2;
        ULONG ul_MinFilter          : 2;
        ULONG ul_MipFilter          : 2;
        ULONG b_TexCoordTransform   : 1;
        ULONG ul_Unused             : 16;
    };
    ULONG ul_RawFlags;

    inline void ClearAll(void) { ul_RawFlags = 0; };
};

struct XeMaterialConstants
{
    M_DeclareOperatorNewAndDelete();

    XeMaterialConstants(void);
    ~XeMaterialConstants(void);

    // Specular map properties
    FLOAT f_SpecularBias;               // Specular bias (Additive)

    // Environment map properties
    ULONG ul_EnvMapColor;               // Color
};

struct XeMaterialStageProperties
{
    // Texture identifier
    LONG l_TextureId;

    // Sampler states
    XeMaterialSS e_States;
    ULONG        ul_BorderColor;
    FLOAT        f_MipmapLODBias;

    // Transform
    D3DXMATRIX st_Transform;
};

struct XeMaterialProperties
{
    M_DeclareOperatorNewAndDelete();

    XeMaterialProperties(void);

    // Render states
    // SC: Memory note: Most of the following can be packed on a UCHAR instead of ULONG
    XeMaterialRS e_RenderStates;
    ULONG ul_AlphaRef;
    ULONG ul_LocalAlpha;
    ULONG ul_SrcBlend;
    ULONG ul_DestBlend;
    ULONG ul_ZFunc;
    ULONG ul_ColorSource;
    ULONG ul_UVSource;
    ULONG ul_BlendOp;

    // Material properties
    ULONG ul_Constant;
    ULONG ul_Ambient;
    ULONG ul_Diffuse;
    ULONG ul_Specular;
    FLOAT f_Shininess;

    XeMaterialConstants st_Constants;
   
    // Textures & Texture stage states
	XeMaterialStageProperties ast_Stages[8];
	ULONG ul_StagesStackDepth;
};

class XeMaterial : public XeCustomShader
{
public:

    M_DeclareOperatorNewAndDelete();

    XeMaterial(MAT_tdst_MultiTexture_* _pst_JadeMaterial = NULL,
               MAT_tdst_MTLevel_*      _pst_JadeMTLevel  = NULL);
    void ReInit(MAT_tdst_MultiTexture_* _pst_JadeMaterial = NULL,
                MAT_tdst_MTLevel_*      _pst_JadeMTLevel  = NULL);
    virtual ~XeMaterial(void);

    enum TextureType
    {
        TEXTURE_BASE                = 0,
        TEXTURE_NORMAL              = 1,
        TEXTURE_SPECULAR            = 2,
        TEXTURE_DETAILNMAP          = 3,
        TEXTURE_ENVIRONMENT         = 4,
        TEXTURE_MOSS                = 5,
        TEXTURE_MOSSMAP             = 5,
        TEXTURE_SHADOWMAP           = 6,
        TEXTURE_LM                  = 7,
        TEXTURE_COUNT
    };


    // carefull.. just a copy and not a full replication
    inline void MakeACopy(XeMaterial &_oMaterial) { _oMaterial = *this; _oMaterial.m_stInfo.b_IsACopy = 1; }

    inline bool IsJadeMaterial(void);
    inline bool IsJadeExtendedMaterial(void);

    // Render state management
    inline BOOL IsTwoSided(void) const;
    inline BOOL IsReflected(void) const;
    inline BOOL IsAlphaTestEnabled(void) const;
    inline BOOL IsAlphaInverted(void) const;
    inline BOOL IsUsingLocalAlpha(void) const;
    inline BOOL IsAlphaBlendEnabled(void) const;
    inline BOOL IsAlphaWriteEnabled(void) const;
    inline BOOL IsColorWriteEnabled(void) const;
    inline BOOL IsZEnabled(void) const;
    inline BOOL IsZWriteEnabled(void) const;
    inline BOOL IsStencilEnabled(void) const;
    inline ULONG GetAlphaRef(void) const;
    inline ULONG GetLocalAlpha(void) const;
    inline ULONG GetSrcBlend(void) const;
    inline ULONG GetDestBlend(void) const;
    inline ULONG GetZFunc(void) const;
    inline ULONG GetColorSource(void) const;
    inline ULONG GetUVSource(void) const;
    inline BOOL  IsMossInverted(void) const;
    inline ULONG GetBlendOp(void) const;
    inline void SetBlendOp(ULONG _ul_BlendOp);
	
	inline BOOL IsOffsetMapping();
    inline void SetTwoSided(BOOL _b_Enable);
    inline void SetReflected(BOOL _b_Enable);
    inline void SetAlphaTest(BOOL _b_Enable, ULONG _ul_AlphaRef = 0);
    inline void SetInvertAlpha(BOOL _b_Invert);
    inline void SetLocalAlpha(BOOL _b_Enable, ULONG _ul_LocalAlpha = 0);
    inline void SetAlphaBlend(BOOL _b_Enable, 
                              ULONG _ul_SrcBlend  = D3DBLEND_ONE, 
                              ULONG _ul_DestBlend = D3DBLEND_ZERO);
    inline void SetColorWrite(BOOL _b_Enable);
    inline void SetAlphaWrite(BOOL _b_Enable);
    inline void SetZState(BOOL  _b_Enable, 
                          BOOL  _b_WriteEnable, 
                          ULONG _ul_ZFunc = D3DCMP_LESSEQUAL);
    inline void SetColorSource(ULONG _ul_ColorSource);
    inline void SetUVSource(ULONG _ul_UVSource);
    inline void SetStencil( BOOL _b_Enable );

    inline void GetTexCoordTransformStates(ULONG* _pul_BaseMapXForm, 
                                           ULONG* _pul_NMapXForm, 
                                           ULONG* _pul_SMapXForm, 
                                           ULONG* _pul_DNMapXForm);

    // Material properties
    inline ULONG GetAmbientSelection(void) const;
    inline ULONG GetDiffuseSelection(void) const;
    inline ULONG GetSpecularSelection(void) const;
    inline ULONG GetConstantColor(void) const;
    inline ULONG GetAmbientColor(void) const;
    inline ULONG GetDiffuseColor(void) const;
    inline ULONG GetSpecularColor(void) const;
    inline FLOAT GetSpecularExponent(void) const;

    inline void SetConstantColor(ULONG _ul_Color);
    inline void SetAmbientColor(ULONG _ul_Color);
    inline void SetDiffuseColor(ULONG _ul_Color);
    inline void SetSpecularColor(ULONG _ul_Color);
    inline void SetSpecularExponent(FLOAT _f_Shininess);

    // Texture stage management
    inline ULONG GetStageCount(void) const;
    ULONG AddTextureStage(void);

    // Texture and sampler stage states
    inline LONG GetTextureId(ULONG _ul_Stage) const;
    inline void GetAddressMode(ULONG  _ul_Stage, 
                               ULONG* _pul_AddressU, 
                               ULONG* _pul_AddressV, 
                               ULONG* _pul_AddressW) const;
    inline void GetFilterMode(ULONG  _ul_Stage, 
                              ULONG* _pul_MagFilter, 
                              ULONG* _pul_MinFilter, 
                              ULONG* _pul_MipFilter) const;
    inline ULONG GetBorderColor(ULONG _ul_Stage) const;
    inline void GetTransform(ULONG _ul_Stage, D3DXMATRIX* _pst_Matrix, ULONG _ulWorldIndex = 0xffffffff) const;

    inline void SetTextureId(ULONG _ul_Stage, LONG _l_TextureId);
    inline void SetAddressMode(ULONG _ul_Stage, 
                               ULONG _ul_AddressU = D3DTADDRESS_CLAMP, 
                               ULONG _ul_AddressV = D3DTADDRESS_CLAMP, 
                               ULONG _ul_AddressW = D3DTADDRESS_CLAMP);
    inline void SetFilterMode(ULONG _ul_Stage, 
                              ULONG _ul_MagFilter = D3DTEXF_LINEAR, 
                              ULONG _ul_MinFilter = D3DTEXF_LINEAR, 
                              ULONG _ul_MipFilter = D3DTEXF_LINEAR);
    inline void SetBorderColor(ULONG _ul_Stage, ULONG _ul_Color);
    inline void SetTransform(ULONG _ul_Stage, BOOL _b_Enable, D3DXMATRIX* _pst_Matrix);

    inline FLOAT GetMipmapLODBias(ULONG _ul_Stage);
    inline void  SetMipmapLODBias(ULONG _ul_Stage, FLOAT _f_Bias);

    // Alpha range remapping
    inline FLOAT GetAlphaScale(void) const;
    inline FLOAT GetAlphaOffset(void) const;
    inline BOOL  IsAlphaRangeRemapped() const { return ((GetAlphaScale() != 1.0f) || (GetAlphaOffset() != 0.0f)); };

    // Constant properties
    inline FLOAT GetSpecularBias(void) const;
    inline ULONG GetEnvMapColor(void) const;

    inline ULONG GetMossMapColor(void) const;
    inline FLOAT GetMossSpecularFactor(void) const;

    inline FLOAT GetDetailNormalMapStrength(UCHAR _uc_LOD) const;

    inline void GetTangentTransform(D3DXMATRIX* _pst_Matrix) const;

    inline ULONG GetSpecularMapChannel(void) const;

    inline void SetSpecularBias(FLOAT _f_Bias);
    inline void SetEnvMapColor(ULONG _ul_Color);

    inline ULONG GetJadeFlags(void) const;
    inline void  SetJadeFlags(ULONG _ulFlags);

    inline BOOL  IsRimLightEnabled() const;
    inline BOOL  IsRimLightSpecularMapAttenuation() const;
    inline FLOAT GetRimLightWidthMin() const;
    inline FLOAT GetRimLightWidthMax() const;
    inline FLOAT GetRimLightIntensity() const;
    inline FLOAT GetRimLightNormalMapRatio() const;

    inline BOOL  IsGlowEnabled() const;

    inline void  SetJadeMatLevel(MAT_tdst_MTLevel_* _level) { m_pstJadeMTLevel = _level; }
    inline MAT_tdst_MTLevel_* GetJadeMatLevel() { return m_pstJadeMTLevel; }

private:
    void ComputeJadeTexCoordMatrix(D3DXMATRIX* _pst_Matrix) const;
    void ComputeJadeChromeMatrix(D3DXMATRIX* _pst_Matrix) const;
    void ComputeJadePlanarGizmoMatrix(D3DXMATRIX* _pst_Matrix, ULONG _ulWorldIndex) const;
    void ComputeTransform(D3DXMATRIX* _pst_Matrix, const MAT_tdst_XeTransform& _stTransform, 
                          BOOL _bAbsolute, BOOL _bScrollU, BOOL _bScrollV, ULONG _ulWorldMatrixIndex = 0xFFFFFFFF, BOOL _bEnabled = TRUE) const;
    void ComputeTangentTransform(D3DXMATRIX* _pst_Matrix) const;

private:

    union JadeMaterialInfo
    {
        struct
        {
            ULONG b_IsJadeMaterial    : 1;       // Is a MAT_tdst_MultiTexture* ?
            ULONG b_IsACopy           : 1;       // was not allocated like a new material.. only a copy of one
            ULONG ul_Unused           : 30;
        };
        ULONG ul_RawFlags;
    };

    JadeMaterialInfo m_stInfo;

    union
    {
        struct
        {
            MAT_tdst_MultiTexture_* m_pstJadeMaterial;
            MAT_tdst_MTLevel_*      m_pstJadeMTLevel;
        };
        XeMaterialProperties*  m_pstProperties;
    };
};

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------

XeMaterial* XeMaterial_Create(void);
XeMaterial* XeMaterial_CreateFromJade(MAT_tdst_MultiTexture_* _pst_JadeMaterial, MAT_tdst_MTLevel_* _pst_JadeMTLevel);
void        XeMaterial_Destroy(XeMaterial* _pst_Mat);

// Inline functions
#include "XeMaterial.inl"

#endif // #ifdef GUARD_XEMATERIAL_H
