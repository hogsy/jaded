/*$T MuTex.h GC!1.60 12/23/99 11:19:54 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#include "BASe/BAStypes.h"
#ifdef JADEFUSION
#include "GraphicDK/Sources/MATerial/MATstruct.h"
#endif
#define MAX_LEVEL_MT    20
#define VSIZE           18L

/* Fist Name will be the lowest bit (1) */

/* 6 maximum */
static char     MUTEX_Flags[][32] =
{
    "Tile U",
    "Tile V",
    "Bilinear",
    "Trilinear",
    "Alpha test",
    "Hide Alpha",
    "Hide Color",
    "Invert Alpha",
    "Z Equal",
    "No Z Write",
    "Use Local Alpha",
    "Cull UV",
    "Fur"
    ""
};
static char     MUTEX_BlendingTypes[][32] =
{
    "Diffuse",
    "Specular",
    "Constant",
    "VertexColor",
    "Full light",
    "Invert Diffuse",
    "Diffuse 2X",
    ""
};
static char     MUTEX_TransparencyTypes[][32] =
{
    "Copy",
    "Alpha",
    "Alpha Premult",
    "Previous Alpha",
    "Previous Alpha Premult",
    "Add",
    "Sub (Mul in OPENGL)",
    "Define normal Map",
    ""
};
static char     MUTEX_UVSources[][32] =
{
    "Object 1",
    "Object 2",
    "Chrome",
    "Diffus-Falloff ($)",
    "Phong gizmo ($)",
    "Previous",
    "Planar gizmo",
    "Face Map",
    "Fog ZZ",
    "Water HOLE",
    ""
};
static char     MUTEX_GizmoFrom[][32] =
{
    "Object",
    "World",
    "Camera",
    "Gizmo n° 0",
    "Gizmo n° 1",
    "Gizmo n° 2",
    "Gizmo n° 3",
    "Gizmo n° 4",
    "Gizmo n° 5",
    "Gizmo n° 6",
    "Gizmo n° 7",
    ""
};

// hogsy: I've translated the below...
static char MUTEX_SoundID[][ 32 ] =
        {
                "No Sound",
                "Dirt",       //"Terre battue",
                "Vegetation", //"Végétation",
                "Stone",      //"Pierre",
                "Water/Marsh",//"Eau/Marécage",
                "Chair",
                "Wood",//"Bois",
                "Metal",
                "Crossable",
                "",
};
#define MUTEX_NbSoundID 9

/*
 * Static char MUTEX_AlphaSources [][32] = {"Color *
 * Texture","Texture Only","",""} ;
 */
static char     MUTEX_DiffuseAlphaFrom[][64] = { "Opacity Alpha ", "VertexColor alpha", "" };

#ifdef JADEFUSION
typedef struct MUTEX_XenonTextureLine_
{
    // Flags
    ULONG b_NMapTransform;
    ULONG b_NMapAbsolute;
    ULONG b_NMapScrollU;
    ULONG b_NMapScrollV;
    ULONG b_SMapTransform;
    ULONG b_SMapAbsolute;
    ULONG b_SMapScrollU;
    ULONG b_SMapScrollV;
    ULONG b_DNMapTransform;
    ULONG b_DNMapScrollU;
    ULONG b_DNMapScrollV;

    ULONG ul_AmbientSel;
    ULONG ul_DiffuseSourceSel;
    ULONG ul_SpecularSourceSel;
    ULONG ul_DiffuseColor;
    ULONG ul_SpecularColor;
    FLOAT f_SpecularExp;

    ULONG b_TwoSided;
    ULONG b_ReflectOnWater;
    ULONG b_InvertMoss;

    // Normal map
    CHAR  sz_NMapName[64];
    LONG  l_NMapId;
    ULONG b_NMapDisabled;

    // Specular map
    CHAR  sz_SpecularMapName[64];
    LONG  l_SpecularMapId;
    ULONG b_SMapDisabled;

    ULONG ul_SpecularMapChannel;
    FLOAT f_SpecularBias;

    // Environment map
    CHAR  sz_EnvMapName[64];
    LONG  l_EnvMapId;
    ULONG ul_EnvMapColor;
    ULONG b_EMapDisabled;

    // Moss map
    CHAR  sz_MossMapName[64];
    LONG  l_MossMapId;
    ULONG ul_MossMapColor;
    ULONG b_MossMapDisabled;
    FLOAT f_MossSpecularFactor;

    // Detail normal map
    CHAR  sz_DNMapName[64];
    LONG  l_DNMapId;
    ULONG b_DNMapDisabled;

	//OFFSET
	ULONG b_OffsetMap;

    ULONG ul_DNMapLODStart;
    ULONG ul_DNMapLODFull;
    FLOAT f_DNMapStrength;

    // Texture transforms
    MAT_tdst_XeTransform st_NMapTransform;
    MAT_tdst_XeTransform st_SMapTransform;
    MAT_tdst_XeTransform st_DNMapTransform;

    // Mipmap LOD bias
    FLOAT f_BaseMipMapLODBias;
    FLOAT f_NormalMipMapLODBias;

    FLOAT f_AlphaStart;
    FLOAT f_AlphaEnd;

    // Rim Light
    ULONG b_RimLightEnable;
    ULONG b_RimLightSMapAttenuationEnabled;
    FLOAT f_RimLightWidthMin; 
    FLOAT f_RimLightWidthMax;
    FLOAT f_RimLightIntensity;
    FLOAT f_RimLightNormalMapRatio;

    // Xenon mesh processing
    BOOL  b_AllowChamfer;
    BOOL  b_AllowTessellation;
    BOOL  b_AllowDisplacement;
    FLOAT f_TessellationArea;
    FLOAT f_DisplacementOffset;
    FLOAT f_DisplacementHeight;
    FLOAT f_SmoothThreshold;
    FLOAT f_ChamferLength;
    FLOAT f_ChamferThreshold;
    FLOAT f_ChamferWeldThreshold;

    // Glow
    ULONG b_GlowEnable;

} MUTEX_XenonTextureLine;
#endif
/*
 -----------------------------------------------------------------------------------------------------------------------
    This structure is only used for editing parameters This structure have to be translate in motor
    struct after editing
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  MUTEX_TextureLine_
{
    ULONG   bInactive;
    char            TextureName[64];
    LONG            TEXTURE_ID;
    ULONG   TextureFlags;           /* Defined in MUTEX_Flags */
    ULONG   TextureBlending;        /* Defined in MUTEX_BlendingTypes */
    ULONG   TextureTransparency;    /* Defined in MUTEX_TransparencyTypes */
    ULONG   AlphaSource;            /* Defined in MUTEX_AlphaSources */
    ULONG   UVSource;               /* Defined in MUTEX_UVSources */
    ULONG   bUIsASpeed;             /* MUST BE 0 OR 1 */
    float           UPos;                   /* In % */
    ULONG   bVIsASpeed;             /* MUST BE 0 OR 1 */
    float           VPos;                   /* In % */
    float           UScale;                 /* Multiplicator */
    float           VScale;                 /* Multiplicator */
    float           Rotation;               /* In degree */
    LONG            IsOpen;                 /* The editor window is open */
    float           AlphaTestTreshold;      /* Alpha test treshold */
    LONG            IsSclLinked;
    ULONG   GizmoFrom;
    ULONG   XYZ;                    /* Respectively 0,1 or 2 */
    ULONG   Negativ;
    ULONG   Symetric;
    ULONG   UseScale;
    ULONG   DeductAlpha;
    float   LocalAlpha;
    ULONG   CullUV;
	// shift draw
    ULONG   ShiftFace;			/* MUST BE 0 OR 1 */

    // SC: Xenon properties
#ifdef JADEFUSION
	ULONG                  b_XeUseExtendedProperties;
	MUTEX_XenonTextureLine st_XeInfo;
#endif

} MUTEX_TextureLine;

typedef struct  MUTEX_Material_
{
    ULONG       DColor;
    ULONG       SColor;
    ULONG       AColor;
    ULONG       IColor;
    float       Opacity;
    ULONG       NumberOfSubTextures;
    char        MaterialName[64];
	UCHAR		Sound;
	UCHAR		Dummy1;
	USHORT		Dummy2;
    MUTEX_TextureLine   AllLine[MAX_LEVEL_MT];
    ULONG       MaterialFlag;
} MUTEX_Material;

/* DLL functions */
#define MUTEX_EXPORT    extern

extern BOOL APIENTRY    DllMain(HANDLE, ULONG, LPVOID);
MUTEX_EXPORT int        MUTEX_GetSubByRect(void *, POINT);
MUTEX_EXPORT void       MUTEX_AdaptToRect(void *Host);
MUTEX_EXPORT void       MUTEX_SetXYoffset(void *Host, ULONG ulXOFFSET, ULONG ulYOFFSET);
MUTEX_EXPORT void       MUTEX_Create(void *Host, ULONG IsClipBoar);
MUTEX_EXPORT void       MUTEX_SetMat(void *Host, MUTEX_Material *New);
MUTEX_EXPORT void       MUTEX_SetRefrechCLBK
                        (
                            void    *Host,
                            int (__stdcall *RefreshCLBK) (void *, BOOL IsLight),
                            void *UserParam
                        );
#ifdef JADEFUSION
// SC: Xenon support functions
enum EMAT_XeBrowseType
{
    EMAT_BT_BrowseTexture,
    EMAT_BT_BrowseCubeMap,
    EMAT_BT_Clear,
    EMAT_BT_ClearCubeMap,
};

void __stdcall EMAT_BrowseForTextureCLBK(void* _p_UserParam, LONG* _pul_TexId, CHAR* _psz_TexName, CHAR* _psz_RefTexName,EMAT_XeBrowseType _e_Type, LONG _l_RefTex_id = BIG_C_InvalidIndex);

#define MUTEX_GSBR_LAYER_MASK    0x00ff
#define MUTEX_GSBR_XELAYER_MASK  0xff00
#define MUTEX_GSBR_XELAYER_SHIFT 8

#define MUTEX_GSBR_XE_NONE          0
#define MUTEX_GSBR_XE_NORMALMAP     1
#define MUTEX_GSBR_XE_SPECULARMAP   2
#define MUTEX_GSBR_XE_ENVMAP        3
#define MUTEX_GSBR_XE_DETAILNMAP    4
#define MUTEX_GSBR_XE_MOSSMAP       5
#define MUTEX_GSBR_XE_BASEMAP       0xff

int MUTEX_PackTextureInfo(ULONG _ul_LayerId, ULONG _ul_XeTextureId);
int MUTEX_ExtractLayer(int _i_TexInfo);
int MUTEX_ExtractXeTexture(int _i_TexInfo);
#endif