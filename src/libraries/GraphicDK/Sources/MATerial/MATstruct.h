/*$T MATstruct.h GC! 1.081 03/06/01 09:58:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#include "GRObject/GROstruct.h"
#include "BASe/BAStypes.h"

#ifndef __MATSTRUCT_H__
#define __MATSTRUCT_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constant contained in ul_Flags
 ***********************************************************************************************************************
 */

/* $2 Flags */
#define MAT_Cul_Flag_MASK				0x00000FFF
#define MAT_Cul_Flag_SHIFT				0
#define MAT_Cul_Flag_TileU				0x00000001
#define MAT_Cul_Flag_TileV				0x00000002
#define MAT_Cul_Flag_Bilinear			0x00000004
#define MAT_Cul_Flag_Trilinear			0x00000008
#define MAT_Cul_Flag_AlphaTest			0x00000010
#define MAT_Cul_Flag_HideAlpha			0x00000020
#define MAT_Cul_Flag_HideColor			0x00000040
#define MAT_Cul_Flag_InvertAlpha		0x00000080
#define MAT_Cul_Flag_ZEqual				0x00000100
#define MAT_Cul_Flag_NoZWrite			0x00000200
#define MAT_Cul_Flag_UseLocalAlpha		0x00000400
#define MAT_Cul_Flag_InActive			0x00000800
#define MAT_Cul_Flag_DontUsePrevious	0x00001000

/* $2 ColorOp */
#define MAT_Cc_ColorOp_MASK				0x0000F000
#define MAT_Cc_ColorOp_SHIFT			12
#define MAT_Cc_ColorOp_Diffuse			0
#define MAT_Cc_ColorOp_Specular			1
#define MAT_Cc_ColorOp_Disable			2
#define MAT_Cc_ColorOp_RLI				3
#define MAT_Cc_ColorOp_FullLight		4
#define MAT_Cc_ColorOp_InvertDiffuse	5
#define MAT_Cc_ColorOp_Diffuse2X		6	
#define MAT_Cc_ColorOp_SpecularColor	7
#define MAT_Cc_ColorOp_DiffuseColor		8
#define MAT_Cc_ColorOp_ConstantColor	9
#ifdef JADEFUSION
#define MAT_Cc_Op_XeAlphaAdd            10
#define MAT_Cc_Op_XeModulateColor       11
#endif

/* $2 Blending */
#define MAT_Cc_Blending_MASK			0x000F0000
#define MAT_Cc_Blending_SHIFT			16
#define MAT_Cc_Op_Copy					0
#define MAT_Cc_Op_Alpha					1
#define MAT_Cc_Op_AlphaPremult			2
#define MAT_Cc_Op_AlphaDest				3
#define MAT_Cc_Op_AlphaDestPremult		4
#define MAT_Cc_Op_Add					5
#define MAT_Cc_Op_Sub					6
#define MAT_Cc_Op_Glow					7
#define MAT_Cc_Op_PSX2ShadowSpecific	8
#define MAT_Cc_Op_SpecialContrast		9

/* $2 Uv source */
#define MAT_Cc_UVSource_MASK	0x00F00000
#define MAT_Cc_UVSource_SHIFT	20
#define MAT_Cc_UV_Object1		0
#define MAT_Cc_UV_Object2		1
#define MAT_Cc_UV_Chrome		2
#define MAT_Cc_UV_DF_GZMO		3
#define MAT_Cc_UV_Phong_GZMO	4
#define MAT_Cc_UV_Previous		5
#define MAT_Cc_UV_Planar_GZMO	6
#define MAT_Cc_UV_FaceMap		7
#define MAT_Cc_UV_FogZZ			8
#define MAT_Cc_UV_WaterHole		9

/* $2 Uv source */
#define MAT_Cc_AlphaTresh_MASK	0x3F000000
#define MAT_Cc_AlphaTresh_SHIFT 24

/* Other flags */
#define MAT_Cc_Flag_UDynamicTransEnable 0x40000000
#define MAT_Cc_Flag_VDynamicTransEnable 0x80000000

#define MAT_C_DefaultFlag (MAT_Cul_Flag_Bilinear | MAT_Cul_Flag_TileV | MAT_Cul_Flag_TileU | MAT_Cc_ColorOp_Diffuse | MAT_Cc_UV_Object1)

/* BEGIN Material flags definition */
#define MATERIAL_FLAG						ULONG
#define MAT_Cc_AlphaDiffuseFrom_OPACITY		0x00000000
#define MAT_Cc_AlphaDiffuseFrom_VertexColor 0x00000001

/* END Material flags definition */

/*$4
 ***********************************************************************************************************************
    Constant contained in s_AditionalFlags
 ***********************************************************************************************************************
 */

/* $2 Flags */
#define MAT_Cul_sFlag_MASK			0x000F
#define MAT_Cul_sFlag_SHIFT			0
#define MAT_Cul_sFlag_USeScale		0x0001
#define MAT_Cul_sFlag_USeSymetric	0x0002
#define MAT_Cul_sFlag_USeNegativ	0x0004
//#define MAT_Cul_sFlag_DeductAlpha	0x0008
#define MAT_XYZ_Flag_ShiftUsingNormal	0x0008

#define MAT_CC_XYZ_MASK				0x0030
#define MAT_CC_XYZ_SHIFT			4
#define MAT_CC_X					0x0000
#define MAT_CC_Y					0x0001
#define MAT_CC_Z					0x0002
#define MAT_CC_XYZ					0x0003

#define MAT_CC_MATRIX_FROM_MASK		0x00C0
#define MAT_CC_MATRIX_FROM_SHIFT	6
#define MAT_CC_OBJECT				0x0000
#define MAT_CC_WORLD				0x0001
#define MAT_CC_CAMERA				0x0002
#define MAT_CC_GIZMO				0x0003

/* 7 gizmo max */
#define MAT_CC_GZMONMBR_FROM_MASK	0x0700
#define MAT_CC_GZMONMBR_FROM_SHIFT	8

/* Local alpha 0..16 */
#define MAT_CC_LOCAL_ALPHA_FROM_MASK	0xF800
#define MAT_CC_LOCAL_ALPHA_FROM_SHIFT	11

#define MAT_GET_XX(Flag, XX)			((Flag & XX##MASK) >> XX##SHIFT)
#define MAT_SET_XX(Flag, Value, XX)		Flag = (Flag & (~XX##MASK)) | ((Value << XX##SHIFT) & XX##MASK)

/*
 =======================================================================================================================
    Ul flags
 =======================================================================================================================
 */
#define MAT_GET_FLAG(Flag)				MAT_GET_XX(Flag, MAT_Cul_Flag_)
#define MAT_SET_FLAG(Flag, Value)		MAT_SET_XX(Flag, Value, MAT_Cul_Flag_)

#define MAT_GET_ColorOp(Flag)			MAT_GET_XX(Flag, MAT_Cc_ColorOp_)
#define MAT_SET_ColorOp(Flag, Value)	MAT_SET_XX(Flag, Value, MAT_Cc_ColorOp_)

#define MAT_GET_Blending(Flag)			MAT_GET_XX(Flag, MAT_Cc_Blending_)
#define MAT_SET_Blending(Flag, Value)	MAT_SET_XX(Flag, Value, MAT_Cc_Blending_)

#define MAT_GET_UVSource(Flag)			MAT_GET_XX(Flag, MAT_Cc_UVSource_)
#define MAT_SET_UVSource(Flag, Value)	MAT_SET_XX(Flag, Value, MAT_Cc_UVSource_)

#define MAT_GET_AlphaTresh(Flag)		((MAT_GET_XX(Flag, MAT_Cc_AlphaTresh_)) << 2)
#define MAT_SET_AlphaTresh(Flag, Value) MAT_SET_XX(Flag, (Value >> 2), MAT_Cc_AlphaTresh_)

/*
 =======================================================================================================================
    S_flag
 =======================================================================================================================
 */
#define MAT_GET_SXX(Flag, XX)				((Flag & XX##MASK) >> XX##SHIFT)
#define MAT_SET_SXX(Flag, Value, XX)		Flag = (unsigned short) (Flag & (~XX##MASK)) | ((Value << XX##SHIFT) & XX##MASK)

#define MAT_GET_XYZ(Flag)					MAT_GET_SXX(Flag, MAT_CC_XYZ_)
#define MAT_SET_XYZ(Flag, Value)			MAT_SET_SXX(Flag, Value, MAT_CC_XYZ_)

#define MAT_GET_MatrixFrom(Flag)			MAT_GET_SXX(Flag, MAT_CC_MATRIX_FROM_)
#define MAT_SET_MatrixFrom(Flag, Value)		MAT_SET_SXX(Flag, Value, MAT_CC_MATRIX_FROM_)

#define MAT_GET_GizmoNumber(Flag)			MAT_GET_SXX(Flag, MAT_CC_GZMONMBR_FROM_)
#define MAT_SET_GizmoNumber(Flag, Value)	MAT_SET_SXX(Flag, Value, MAT_CC_GZMONMBR_FROM_)

#define MAT_GET_LocalAlpha(Flag)			MAT_GET_SXX(Flag, MAT_CC_LOCAL_ALPHA_FROM_)
#define MAT_SET_LocalAlpha(Flag, Value)		MAT_SET_SXX(Flag, Value, MAT_CC_LOCAL_ALPHA_FROM_)

#define MAT_GET_s_Flags(Flag)				MAT_GET_SXX(Flag, MAT_Cul_sFlag_)
#define MAT_SET_s_Flags(Flag, Value)		MAT_SET_SXX(Flag, Value, MAT_Cul_sFlag_)

/*$4
 ***********************************************************************************************************************
    Structure
 ***********************************************************************************************************************
 */

typedef struct MAT_tdst_Decompressed_UVMatrix_
{
	float UVMatrix[4];
	float AddU, AddV;
}
MAT_tdst_Decompressed_UVMatrix;

/* Uv matrix definitions */
#define MAT_Cc_ScaleMask	0x7ffE0000
#define MAT_Cc_ScaleSign	0x80000000
#define MAT_Cc_ScaleShift	16
#define MAT_Cc_SpeedMask	0x00007ffE
#define MAT_Cc_SpeedSign	0x00008000
#define MAT_Cc_SpeedShift	0
#define MAT_Cc_PosMask		0x00007ffE
#define MAT_Cc_PosSign		0x00008000
#define MAT_Cc_PosShift		0

#define MAT_Cc_Identity		0x3f800000

typedef struct	MAT_tdst_MTLevel_
{
	short						s_TextureId;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Flags informations: This will contain all information about transparency, uvcalculatiosn & color operation.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	unsigned short				s_AditionalFlags;
	ULONG						ul_Flags;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Compressed UV matrix Information: -> SCALE is between -16..+16 it is coede in the 9 first bit of ScaleSPeedPos with
	    1 bit of sign, 4 bit integer, 4 bits float. -> SPEED is between -512..+512 (in percentage per SECONDE (%/s)) it is
	    coede in the 15 first bit of ScaleSPeedPos with 1 bit of sign, 10 bit integer, 4 bits float. this is aliased on
	    POS. -> POS is between 0,0..1,0 (in percentage) it is coded between 13 to 24 bits (8 bits)of ScaleSPeedPos this is
	    aliased on SPEED. -> Rtoation is coded ScaleSPeedPosU & ScaleSPeedPosV. Not simple!. Do not acces Yourseft to that
	    fields, USE: MAT_VUDecompress(MAT_tdst_MTLevel *CompressedUV , MAT_tdst_Decompressed_UVMatrix *DecompressedUV);
	    float GetScale (ULONG ScaleSPeedPosU); void SetScale (ULONG &ScaleSPeedPosU, float Scale); float GetSpeed (ULONG
	    ScaleSPeedPosU); void SetSpeed (ULONG &ScaleSPeedPosU, float Speed); float GetPos (ULONG ScaleSPeedPosU); void
	    SetPos (ULONG &ScaleSPeedPosU, float Pos);
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ULONG						ScaleSPeedPosU;
	ULONG						ScaleSPeedPosV;

	struct MAT_tdst_MTLevel_	*pst_NextLevel;

#if defined(_XENON_RENDER)
    // Extended level information for Xenon (Normal, Gloss, Env maps)
    struct MAT_tdst_XeMTLevel_* pst_XeLevel;

    class XeMaterial* pst_XeMaterial;
#endif // defined(_XENON_RENDER)

#if defined(_XENON)
	LONG   al_TextureIDCache[8]; // Used on Xenon to avoid branching in GetTextureID()
#endif

} MAT_tdst_MTLevel;

#ifdef JADEFUSION
// Editor values
#define MAT_Xe_Specular_Exponent_Min 4.0f
#define MAT_Xe_Specular_Exponent_Max 256.0f
#define MAT_Xe_Specular_Bias_Min     -1.0f
#define MAT_Xe_Specular_Bias_Max     1.0f
#define MAT_Xe_DNMap_Strength_Min    0.0f
#define MAT_Xe_DNMap_Strength_Max    1.0f

#if defined(_XENON_RENDER)

// Constants
#define MAT_Xe_InvalidTextureId -1

#define MAT_Xe_SpecularMapChannel_A 0
#define MAT_Xe_SpecularMapChannel_R 1
#define MAT_Xe_SpecularMapChannel_G 2
#define MAT_Xe_SpecularMapChannel_B 3

// Xenon MT level flags
#define MAT_Xe_Flag_NMapTransform               0x00000001
#define MAT_Xe_Flag_NMapAbsolute                0x00000002
#define MAT_Xe_Flag_NMapScrollU                 0x00000004
#define MAT_Xe_Flag_NMapScrollV                 0x00000008
#define MAT_Xe_Flag_SMapTransform               0x00000010
#define MAT_Xe_Flag_SMapAbsolute                0x00000020
#define MAT_Xe_Flag_SMapScrollU                 0x00000040
#define MAT_Xe_Flag_SMapScrollV                 0x00000080
#define MAT_Xe_Flag_DNMapTransform              0x00000100
//#define MAT_Xe_Flag_XXXXXXXXXXXXXXXXXXX         0x00000200
#define MAT_Xe_Flag_OffsetMap					0x00000200
#define MAT_Xe_Flag_DNMapScrollU                0x00000400
#define MAT_Xe_Flag_DNMapScrollV                0x00000800
#define MAT_Xe_Flag_NMapDisable                 0x00001000
#define MAT_Xe_Flag_SMapDisable                 0x00002000
#define MAT_Xe_Flag_DNMapDisable                0x00004000
#define MAT_Xe_Flag_EMapDisable                 0x00008000
//popowarning
//#define MAT_Xe_Flag_OffsetMap					31
#define MAT_Xe_Flag_SpecMapChannel_Mask         0x000f0000
#define MAT_Xe_Flag_SpecMapChannel_Shift        16
#define MAT_Xe_Flag_TwoSided                    0x00100000
#define MAT_Xe_Flag_MossMapDisable              0x00200000
#define MAT_Xe_Flag_RimLightEnable              0x00400000
#define MAT_Xe_Flag_RimLightSMapAttenuation     0x00800000
#define MAT_Xe_Flag_InvertMoss                  0x01000000
#define MAT_Xe_Flag_GlowEnable                  0x02000000
#define MAT_Xe_Flag_ReflectOnWater              0x04000000
#define MAT_Xe_Flag_AmbientSel_Mask             0x18000000
#define MAT_Xe_Flag_AmbientSel_Shift            27
#define MAT_Xe_Flag_DiffuseSel_Mask             0x20000000
#define MAT_Xe_Flag_DiffuseSel_Shift            29
#define MAT_Xe_Flag_SpecularSel_Mask            0x40000000
#define MAT_Xe_Flag_SpecularSel_Shift           30

#define MAT_Xe_GetSpecularMapChannel(_flags)            (((_flags) & MAT_Xe_Flag_SpecMapChannel_Mask) >> MAT_Xe_Flag_SpecMapChannel_Shift)
#define MAT_Xe_SetSpecularMapChannel(_flags, _channel)  (_flags) = (((_flags) & ~MAT_Xe_Flag_SpecMapChannel_Mask) | ((_channel) << MAT_Xe_Flag_SpecMapChannel_Shift))

#define MAT_Xe_AmbientSel_Object   0
#define MAT_Xe_AmbientSel_Ambient1 1
#define MAT_Xe_AmbientSel_Ambient2 2

#define MAT_Xe_DiffuseSel_Material   0
#define MAT_Xe_DiffuseSel_World      1

#define MAT_Xe_SpecularSel_Material   0
#define MAT_Xe_SpecularSel_World      1

#define MAT_Xe_GetAmbientSel(_flags)            (((_flags) & MAT_Xe_Flag_AmbientSel_Mask) >> MAT_Xe_Flag_AmbientSel_Shift)
#define MAT_Xe_SetAmbientSel(_flags, _channel)  (_flags) = (((_flags) & ~MAT_Xe_Flag_AmbientSel_Mask) | ((_channel) << MAT_Xe_Flag_AmbientSel_Shift))

#define MAT_Xe_GetDiffuseSel(_flags)            (((_flags) & MAT_Xe_Flag_DiffuseSel_Mask) >> MAT_Xe_Flag_DiffuseSel_Shift)
#define MAT_Xe_SetDiffuseSel(_flags, _channel)  (_flags) = (((_flags) & ~MAT_Xe_Flag_DiffuseSel_Mask) | ((_channel) << MAT_Xe_Flag_DiffuseSel_Shift))

#define MAT_Xe_GetSpecularSel(_flags)           (((_flags) & MAT_Xe_Flag_SpecularSel_Mask) >> MAT_Xe_Flag_SpecularSel_Shift)
#define MAT_Xe_SetSpecularSel(_flags, _channel) (_flags) = (((_flags) & ~MAT_Xe_Flag_SpecularSel_Mask) | ((_channel) << MAT_Xe_Flag_SpecularSel_Shift))

typedef struct MAT_tdst_XeTransform_
{
    FLOAT f_ScaleU;
    FLOAT f_ScaleV;
    FLOAT f_Angle;
    union
    {
        FLOAT f_SpeedU;
        FLOAT f_StartU;
    };
    union
    {
        FLOAT f_SpeedV;
        FLOAT f_StartV;
    };
    FLOAT f_RollSpeed;
} MAT_tdst_XeTransform;

typedef struct MAT_tdst_XeMTLevel_
{
    ULONG  ul_Flags;                                // Flags, see MAT_Xe_Flag_*

    ULONG  ul_DiffuseColor;                         // Diffuse color
    ULONG  ul_SpecularColor;                        // Specular color
    FLOAT  f_SpecularExp;                           // Specular exponent

    // Texture maps (DiffuseMap is the base Jade texture of the level)
    LONG   l_NormalMapId;                           // ID of the normal map (valid if >= 0)
    LONG   l_SpecularMapId;                         // ID of the specular map (valid if >= 0)
    LONG   l_EnvMapId;                              // ID of the environmnent map
    LONG   l_MossMapId;                              // ID of the environmnent map
    LONG   l_DetailNMapId;                          // ID of the detail normal map

    FLOAT  f_BaseMipMapLODBias;                     // Mipmap LOD bias for the base map
    FLOAT  f_NormalMipMapLODBias;                   // Mipmap LOD bias for the normal map

    // Transforms
    MAT_tdst_XeTransform st_NormalMapTransform;     // Normal map transform
    MAT_tdst_XeTransform st_SpecularMapTransform;   // Specular map transform
    MAT_tdst_XeTransform st_DetailNMapTransform;    // Detail normal map transform

    // Specular
    FLOAT  f_SpecularBias;                          // Specular bias [0..1]

    // Environment mapping parameters
    ULONG  ul_EnvMapColor;                          // Environment map color

    // Moss mapping parameters
    ULONG  ul_MossMapColor;                         // Moss map color
    FLOAT  f_MossSpecularFactor;                    // Moss specular factor % of main specular

    // Detail normal mapping parameters
    UCHAR  uc_DetailNMapStartLOD;                   // Activation LOD of the detail normal map
    UCHAR  uc_DetailNMapFullLOD;                    // Activation LOD of the detail normal map
    FLOAT  f_DetailNMapStrength;                    // Strength of the detail normal map

    // Alpha range remaping
    FLOAT  f_AlphaScale;
    FLOAT  f_AlphaOffset;

    // Rim Light
    FLOAT  f_RimLightWidthMin;
    FLOAT  f_RimLightWidthMax;
    FLOAT  f_RimLightIntensity;
    FLOAT  f_RimLightNormalMapRatio;

#if defined(ACTIVE_EDITORS)
    ULONG ul_MeshProcessingFlags;
    FLOAT f_DisplacementOffset;
    FLOAT f_DisplacementHeight;
    FLOAT f_TessellationArea;
    FLOAT f_ChamferThreshold;
    FLOAT f_ChamferLength;
    FLOAT f_ChamferWeldThreshold;
#endif
    FLOAT f_SmoothThreshold;

} MAT_tdst_XeMTLevel;
#endif // defined(_XENON_RENDER)
#endif//jade fusion

/* 
	 -------------------------------------------------------------------------------------------------------------------
	 ValidateMask definition
	 -------------------------------------------------------------------------------------------------------------------
*/
#define MAT_ValidateMask_Diffuse			0x00000001
#define MAT_ValidateMask_Specular_Color		0x00000002
#define MAT_ValidateMask_Constant			0x00000004
#define MAT_ValidateMask_FullLight			0x00000008
#define MAT_ValidateMask_RLI				0x00000010
#define MAT_ValidateMask_Specular_UV		0x00000020
#define MAT_ValidateMask_Transparency_And	0x00000040
#define MAT_ValidateMask_Transparency_Or	0x00000080
#define MAT_ValidateMask_Transparency		(MAT_ValidateMask_Transparency_And | MAT_ValidateMask_Transparency_Or)
#if defined (_XBOX) || defined(_XENON_RENDER)
#define MAT_ValidateMask_RLIAlpha			0x00000100
#elif defined(PSX2_TARGET) || defined(_GAMECUBE) || defined (ACTIVE_EDITORS) || defined (PCWIN_TOOL) || defined (_PC_RETAIL)
#define MAT_ValidateMask_RLIAlpha			MAT_ValidateMask_RLI 
#else
#   error TODO: Unsupported platform 
#endif
#define MAT_ValidateMask_Fur				0x00000200
#define MAT_ValidateMask_KingKong			0x00000400
#define MAT_ValidateMask_EnalbleAll			0xffffffff

typedef struct	MAT_tdst_MultiTexture_
{
	/* BEGIN MUST BE THE SAME THAN MAT_tdst_Single */
	GRO_tdst_Struct		st_Id;
	ULONG				ul_Ambiant;
	ULONG				ul_Diffuse;
	ULONG				ul_Specular;
	float				f_SpecularExp;
	float				f_Opacity;
	MATERIAL_FLAG		ul_Flags;

	/* END MUST BE THE SAME THAN MAT_tdst_Single */
	MAT_tdst_MTLevel	*pst_FirstLevel;
	ULONG				ul_ValidateMask;

	UCHAR				uc_Version;
	UCHAR				uc_Sound;
	USHORT				uw_Dummy;
} MAT_tdst_MultiTexture;

typedef struct	MAT_tdst_Single_
{
	/* BEGIN MUST BE THE SAME THAN MAT_tdst_MultiTexture */
	GRO_tdst_Struct st_Id;
	ULONG			ul_Ambiant;
	ULONG			ul_Diffuse;
	ULONG			ul_Specular;
	float			f_SpecularExp;
	float			f_Opacity;
	MATERIAL_FLAG	ul_Flags;

	/* END MUST BE THE SAME THAN MAT_tdst_MultiTexture */
	LONG			l_TextureId;
	ULONG			ul_ValidateMask;
} MAT_tdst_Single;

typedef struct	MAT_tdst_Multi_
{
	GRO_tdst_Struct				st_Id;
	LONG						l_NumberOfSubMaterials;
	struct MAT_tdst_Material_	**dpst_SubMaterial;
	ULONG						ul_ValidateMask;
} MAT_tdst_Multi;

typedef struct	MAT_tdst_Material_
{
	GRO_tdst_Struct st_Id;
} MAT_tdst_Material;

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

extern MAT_tdst_MultiTexture	MAT_gst_DefaultSingleMaterial;
extern MAT_tdst_MTLevel			MAT_gst_DefaultMatMTLevel;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

MAT_tdst_Material				*MAT_pst_CreateMaterial(LONG, char *);
void							MAT_DestroyMaterial(MAT_tdst_Material *);

void							MAT_SetRenderState(MAT_tdst_Material *, LONG);

#if defined(_XBOX)
__inline void Gx8_SetMultipleVBIndexForUpdate( int index )
{
	extern int g_MultipleVBIndex;
	// set Multiple VB Index to use for later VB Update
	g_MultipleVBIndex = index;
};
#endif // defined(_XBOX)

void							MAT_DrawIndexedTriangle
								(
									struct GEO_tdst_Object_ *,
									MAT_tdst_Material *,
									struct GEO_tdst_ElementIndexedTriangles_ *
								);
								
void							MAT_DrawIndexedSprites
								(
									struct GEO_tdst_Object_ *,
									MAT_tdst_Material *,
									struct GEO_tdst_ElementIndexedSprite_ *
								);
								
BOOL							MAT_IsMaterialTransparent(MAT_tdst_Material *, LONG, ULONG, struct GEO_tdst_Object_ *);
ULONG 							MAT_GetValidityMASK(MAT_tdst_Material *pst_GRO, LONG Submat, struct GEO_tdst_Object_ * );
char							*MAT_pc_GetAllUsedTextureIndex(struct GDI_tdst_DisplayData_ *, LONG);

void							MAT_Init(void);

/* This function wiil return 1 if compressedUV is != of identity */
BOOL							MAT_VUISIdentity(MAT_tdst_MTLevel *);
void							MAT_VUDecompress(MAT_tdst_MTLevel *, MAT_tdst_Decompressed_UVMatrix *);
void							MAT_VUCompress(MAT_tdst_Decompressed_UVMatrix *, MAT_tdst_MTLevel *);
float							MAT_GetScale(ULONG);
void							MAT_SetScale(ULONG *, float);
float							MAT_GetSpeed(ULONG);
void							MAT_SetSpeed(ULONG *, float);
float							MAT_GetPos(ULONG);
void							MAT_SetPos(ULONG *, float);

/* TypeOfColor could be : - 0 Diffuse, - 1 Constant, - 2 Ambient */
void							MAT_GetColorPtr(GRO_tdst_Struct *, ULONG **, ULONG SubMatNum, ULONG TypeOfColor);
void							MAT_GetMtxLvlPtr(GRO_tdst_Struct *, MAT_tdst_MTLevel **, ULONG SubMatNum, ULONG Lvl);

/* !!!! ROTATION IS 1 = 360° */
float							MAT_GetRotation(ULONG, ULONG);
void							MAT_SetRotation(ULONG *, ULONG *, float);

/* Ensalada de majorca */
void	MAT_Validate_Multi(MAT_tdst_Multi *_pst_Mat);
void	MAT_Validate_Multitexture(MAT_tdst_MultiTexture *Mat);
void	MAT_Validate_Single(MAT_tdst_Single *_pst_Mat);

#ifdef JADEFUSION
MAT_tdst_Material* MAT_GetElementMaterial(GRO_tdst_Visu* _pst_Visu, struct GEO_tdst_ElementIndexedTriangles_* _pst_Element);
#endif

#ifdef ACTIVE_EDITORS
void MAT_UVMatrix_Invert( MAT_tdst_Decompressed_UVMatrix *, MAT_tdst_Decompressed_UVMatrix *);
#endif /* ACTIVE_EDITORS */


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __MATSTRUCT_H__ */ 
 