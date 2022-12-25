/*$T MDFmodifier_GEO.h GC! 1.081 11/14/00 12:12:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_SPG2_H__
#define __MDFMODIFIER_SPG2_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#ifdef _XENON_RENDER
#include "XenonGraphics/XeHeatManager.h"
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Modifier const
 ***********************************************************************************************************************
 */

/* SPG2 modifier flags */
#define SPG2_IsMaterialTransparent				0x00000001
#define SPG2_MustBeSorted						0x00000002
#define SPG2_SpecialFog							0x00000004
#define SPG2_FogCulling							0x00000008
#define SPG2_GrassMove							0x00000010
#define SPG2_NormalAxis							0x00000020
#define SPG2_VertexAlphaIsDensity				0x00000040
#define SPG2_InversePrevious					0x00000080
#define SPG2_AlphaMeanSize						0x00000100
#define SPG2_DebugShowLines						0x00000200
#define SPG2_DisapearWithAngle					0x00000400
#define SPG2_DrawHat							0x00000800
#define SPG2_DrawX								0x00001000
#define SPG2_DrawY								0x00002000
#define SPG2_PreferenceZ						0x00004000
#define SPG2_PreferenceXY						0x00008000
#define SPG2_OnlyPositiveZ						0x00010000
#define SPG2_DrawSprites						0x00020000
#define SPG2_OnePerPoint						0x00040000
#define SPG2_FillSurface						0x00080000
#define SPG2_RotationNoise						0x00100000
#define SPG2_DrawTruncs							0x00200000
#define SPG2_DrawinAlpha						0x00400000
#define SPG2_SpriteRotation						0x00800000
#define SPG2_DontUseCache						0x01000000
#define SPG2_GridGeneration						0x02000000
#define SPG2_ModeAdd							0x04000000
#define SPG2_BurnedMode							0x08000000
#define SPG2_XAxisIsInlookat					0x10000000
#define SPG2_UsedInTransparency					0x20000000
#define SPG2_IAGenerator						0x40000000

#define SPG2_Damier								0x00000001
#define SPG2_GenInBV							0x00000002
#define SPG2_DontGenerate						0x00000004
#define SPG2_DontDisplay						0x00000008
#define SPG2_Damier2							0x00000010
#define SPG2_UseRLIOnly							0x00000020
#define SPG2_UseLOD								0x00000040
#define SPG2_UseLOD2							0x00000080

#ifdef JADEFUSION
#define SPG2_HeatShimmerEnable					0x00010000			// during merge. This is not nice, but should work.
#endif
/* General for GEO modifiers */
#define SPG2_C_ModifierFlags_CanBeApplyInGeom	0x00010000
#define SPG2_C_ModifierFlags_ApplyInGeom		0x00020000

/* Global params for wind */
#define SPG2_PARAM_SPRINGS	0
#define SPG2_PARAM_SPEEDA	1
#define SPG2_PARAM_WINDS	2
extern float SPG2_GlobalParams[5][20];

/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */
#if defined(PSX2_TARGET) || defined(_GAMECUBE) || defined(_XBOX)
#define SPG2_PACKET_SIZE 32
#elif defined(_XENON_RENDER)
#define SPG2_PACKET_SIZE 4096
#else
#define SPG2_PACKET_SIZE 64
#endif
typedef struct SPG2_InstanceInforamtion_
{
	MATH_tdst_Vector	GlobalPos;
	MATH_tdst_Vector	GlobalZADD;
	float				GlobalSCale;
	ULONG				BaseWind;
	float				Culling;
	ULONG				GlobalColor;
	u32					BaseAnimUv;
#ifdef JADEFUSION 
	float               HeatShimmerIntensity;
#endif
}
SPG2_InstanceInforamtion ONLY_PSX2_ALIGNED(64);

typedef s16 tCmpVertex; // float
typedef f32 tCmpNormal; 

ONLY_XBOX_ALIGNED(16) typedef struct SPG2_CachedPrimitivs_
{
#if 0 //def _GAMECUBE
	// Don't change order of these attributes (we invalidate cache)
	union
	{
		SOFT_tdst_AVertex	a_PointLA2[SPG2_PACKET_SIZE<<2];
		// This space can also be used to store the geometry for cross and hat cases (12 vertices of 3 shorts each).
		tCmpVertex					a_VtxBuffer[SPG2_PACKET_SIZE*36];
	};
	ULONG				a_ColorLA2[SPG2_PACKET_SIZE]	ONLY_PSX2_ALIGNED(64);

	// Normal buffer for compressed data 
	//tCmpNormal					a_NormalBuffer[SPG2_PACKET_SIZE*3]; 
	
	// TextCoord buffer for compressed data : 12 points, 2 floats (u and v) per point,
	// or for sprites (we then use only the 8 first values, for 4 points).
	float 				a_TextureCoordBuffer[24]; 
	
#else // _GAMECUBE
	ONLY_XBOX_ALIGNED(16) SOFT_tdst_AVertex	a_PointLA2[SPG2_PACKET_SIZE<<2]	ONLY_PSX2_ALIGNED(64);
	ONLY_XBOX_ALIGNED(16) ULONG				a_ColorLA2[SPG2_PACKET_SIZE]	ONLY_PSX2_ALIGNED(64);
#endif // _GAMECUBE

	ULONG				a_PtrLA2;
	ULONG				ulFlags; /* 0 = Turned , 1 = Used */
	
#ifdef _GAMECUBE
	u8					uFrac;
	MATH_tdst_Vector	stCenter;	
#endif // _GAMECUBE
	
	struct SPG2_tdst_Modifier_	*p_Generator;
#ifndef _XENON_RENDER
	struct SPG2_CachedPrimitivs_ *p_NextCacheLine;
#endif

#ifdef _XENON_RENDER
	XeMesh				*pMesh2X;
	XeBuffer			*pVB2X;
    XeBuffer			*pVBHelicoidal;
	XeMesh				*pMeshSprites;
	XeBuffer			*pVBSprites;
    float               fMaxSpriteSize;
#endif

}
SPG2_CachedPrimitivs ONLY_PSX2_ALIGNED(64);

#ifdef _XENON_RENDER
typedef struct SPG2_Instance_
{
    SOFT_tdst_AVertex	*a_PointLA2;
    ULONG				*a_ColorLA2;
    ULONG				a_PtrLA2;

    XeMesh				*pMesh2X;
    XeBuffer			*pVB2X;
    XeBuffer			*pVBHelicoidal;
    XeMesh				*pMeshSprites;
    XeBuffer			*pVBSprites;
    float               fMaxSpriteSize;

} SPG2_Instance;
#endif

#define SPG2_CachedPrimitivsInitFlag  3 /* Turned */

enum SPG2_CachedPrimitivsFlag
{
	SPG2_CachedPrimitivsFlagRAND_ROT = 		0x00000001,
	SPG2_CachedPrimitivsFlagLOOK_AT =		0x00000002,
	SPG2_CachedPrimitivsFlagGRID = 			0x00000004,
	SPG2_CachedPrimitivsFlagCOMPRESSED =	0x00000008,
	SPG2_CachedPrimitivsFlagINDIRECT_DATA = 0x00000010, // Use indirect data that goes through DMA.
	SPG2_CachedPrimitivsFlagUNUSED2 = 		0x00000020,
	SPG2_CachedPrimitivsFlagUNUSED3 = 		0x00000040
};


typedef struct SPG2_SpriteMapper_
{
	/* Set to Zero main Full Texture */
	/* Global Position */
	unsigned char UShift;
	unsigned char Uadd;
	unsigned char VShift;
	unsigned char Vadd;
	/* Sub-image description */
	unsigned char USubFrameShift;
	unsigned char VSubFrameShift;
	unsigned char AnimationFrameDT;
	unsigned char AnimOffset;
} SPG2_SpriteMapper;

typedef struct SPG2_SpriteElementDescriptor_
{
	ULONG	ulSubElementMaterialNumber;
	float 	fSizeFactor;
	float 	fSizeNoiseFactor;
	float	fRatioFactor;
} SPG2_SpriteElementDescriptor;

#ifdef JADEFUSION
typedef struct FakeStr_
{
    MATH_tdst_Vector         FakeXYZ;
    ULONG					AlphhaT;
} FakeStr ONLY_PSX2_ALIGNED(16);

#define MAX_RND 4096
#define MaxSpheres 8
#endif

typedef struct	SPG2_tdst_Modifier_
{
#if defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL)
	ULONG				ulCodeKey;
	ULONG				ulNumberofUsedTriangles;
#endif
	ULONG				ulNumberofgeneratedSprites;
	ULONG				ulFlags;
	ULONG				ulFlags1;
	float				f_GlobalSize;
	float				f_SpriteSize;
	float				f_SpriteGeneratorRadius;
	float				f_GlobalRatio;
	float				f_Density;
	float				f_Noise;
	float				f_Near;
	float				f_Far;
	float				f_Near2;
	float				f_Far2;
	ULONG				ulSubMaterialMask;
	ULONG				ulAlphaFromPondSelector;
	ULONG				AlphaThreshold;
	float				VShift;
	float				fTrapeze;
	ULONG				TileNumber;

	ULONG				GridFilter;

	float				fExtraction;
	ULONG				ulSubMaterialNum;
	float				fExtractionOfHorizontalPlane;

	ULONG				ulSubMaterial_LOOKAT;
	ULONG				ulSubMaterial_SPRITE;


	u32							AnimTextureTileUPo2;
	u32							AnimTextureTileVPo2;
	float						AnimTextureSpeed;

	float						SwitchDistance;
	ULONG						GameObjectKey;
	struct OBJ_tdst_GameObject_ *GameObjectToDuplicate;

	float Gravity;
	ULONG NumberOfSegments;
	ULONG NumberOfSprites;

	/* Mecanics */
#define SPG2_WindNumbers 4
	MATH_tdst_Vector		CurrentPos[SPG2_WindNumbers];
	MATH_tdst_Vector		CurrentSpeed[SPG2_WindNumbers];
	float					SringStrenght;
	float					SpeedAbsorbtion;
	float					Freedom;
	float					WindSensibility;
	ULONG					Preset;
	float					SphereCollideRadius;

	float					GridNoise;


	ULONG					ulMustbeRecompute;

#ifdef _XENON_RENDER
    SPG2_Instance	        stInstance;
#else
    SPG2_CachedPrimitivs	SPG2_FirstCachedPrimitivs ONLY_PSX2_ALIGNED(64);
#endif

	struct SPG2_tdst_Modifier_	*p_NextSPG2;

	LONG					ulUsedSince;
	u32						FrameCounter;

	MATH_tdst_Vector		MinBox;
	MATH_tdst_Vector		MaxBox;

	u32						UniqID;
	u32						BaseNoise;
#ifdef JADEFUSION
    float                   f_HeatShimmerNoisePixelSize;
    float                   f_HeatShimmerScrollSpeed;
    float                   f_HeatShimmerRange;
    float                   f_HeatShimmerHeight;
    float                   f_HeatShimmerScroll;
    float                   f_HeatShimmerOpacity;

    OBJ_tdst_GameObject	    *pSPG2Holder;
    MDF_tdst_Modifier       *pParentModifier;
#endif
} SPG2_tdst_Modifier;

#ifdef JADEFUSION
#define SPG2_RELATIVE_COMPUTING
#include "ENGine/Sources/Grid/GRI_Struct.h"

typedef struct SPG2_GridMazpperNode_
{
    MATH_tdst_Vector		stPos;
    u16						GridSrc;
    u16						Grid2Src;
    ULONG					Color;
    u8						GridValue;
    u8						GridFire;
    u8						GridFireContext[2];
    float                   MaxGlobalScale;
} SPG2_GridMazpperNode;

typedef struct SPG2_GridMazpper_
{
    ULONG 					ulNumbeOfNode;
    SPG2_GridMazpperNode 	*p_AllNodes;
    ULONG 					IsComputed;
    u32						SPG2_GRID_MAKS;
    float					GRID_NOISES[16];
    GRID_tdst_World			*_pst_GridUsed;
    GRID_tdst_World			*_pst_Grid2Used;
#ifdef SPG2_RELATIVE_COMPUTING
    MATH_tdst_Vector		OLDBVMAX,OLDBVMIN;
    u32						OLDGRISMASK;
#endif

} SPG2_GridMazpper;

#endif

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void							SPG2_LoadWorldHook( void );
extern void							SPG2_ReinitWorldHook( void );
extern void							SPG2_Modifier_Create(struct OBJ_tdst_GameObject_ *,MDF_tdst_Modifier *,void *);
extern void							SPG2_Modifier_Destroy(MDF_tdst_Modifier *);
extern void							SPG2_Modifier_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							SPG2_Modifier_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						SPG2_ul_Modifier_Load(MDF_tdst_Modifier *, char *);
extern void							SPG2_Modifier_Save(MDF_tdst_Modifier *);
extern int							SPG2_Modifier_Copy(MDF_tdst_Modifier *, MDF_tdst_Modifier *);
void 								SPG2_Modifier_Interpolate(struct MDF_tdst_Modifier_ * p_Mod, unsigned int Globalmode , float fInterpolatedValue);
extern void							SPG2_Modifier_FlushAll();
extern void							SPG2_Modifier_FlushAllModifiers();
void								SPG2_DestroySpecialGrid();
extern BOOL                         SPG2_bIsSameSPG2Modifier(struct	SPG2_tdst_Modifier_ *,struct SPG2_tdst_Modifier_ *);

extern ULONG						SPG2_AI_AddOne( MATH_tdst_Vector *, int );
extern void							SPG2_AI_SetState( ULONG , int );
extern void							SPG2_AI_SetPos( ULONG , MATH_tdst_Vector *);
extern void							SPG2_AI_SetFloat( ULONG , int , float );

#ifdef JADEFUSION
extern int g_iCurrentFireContext;
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif /* __MDFMODIFIER_SPG2_H__ */
