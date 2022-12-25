/*$T MDFmodifier_GEO.h GC! 1.081 11/14/00 12:12:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_SPG_H__
#define __MDFMODIFIER_SPG_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Modifier const
 ***********************************************************************************************************************
 */

/* SPG modifier flags */
#define SPG_IsMaterialTransparent				0x00000001
#define SPG_MustBeSorted						0x00000002
#define SPG_SpecialFog							0x00000004
#define SPG_FogCulling							0x00000008
#define SPG_GrassMove							0x00000010
#define SPG_NormalAxis							0x00000020

/* General for GEO modifiers */
#define SPG_C_ModifierFlags_CanBeApplyInGeom	0x00010000
#define SPG_C_ModifierFlags_ApplyInGeom			0x00020000

/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */

typedef struct SPG_SpriteMapper_
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
} SPG_SpriteMapper;

typedef struct SPG_SpriteElementDescriptor_
{
	ULONG	ulSubElementMaterialNumber;
	float 	fSizeFactor;
	float 	fSizeNoiseFactor;
	float	fRatioFactor;
} SPG_SpriteElementDescriptor;

typedef struct	SPG_tdst_Modifier_
{
#ifdef ACTIVE_EDITORS
	ULONG				ulCodeKey;
	ULONG				ulNumberofgeneratedSprites;
	ULONG				ulNumberofUsedTriangles;
#endif
	BIG_KEY				bk_MaterialID;
	MAT_tdst_Material	*p_Material;
	float				f_GlobalSize;
	float				f_GlobalRatio;
	float				f_Extraction;
	float				f_ThresholdMin;
	float				f_Noise;
	ULONG				ulMaxDepth;
	ULONG				ulSubMaterialMask;
	float 				fLODCorrectionFactor;
	SPG_SpriteMapper			stSpriteMapper[4]; // Not used animore
	SPG_SpriteElementDescriptor stSED[4];
	ULONG				ulFlags;

	float				fSpecialFogNear;
	float				fSpecialFogFar;
	ULONG				ulSpecialFogColor;

#ifdef USE_DOUBLE_RENDERING	
	float				f_Noise_0;
	float				f_Noise_1;
	float				f_Noise_Out;
#endif

} SPG_tdst_Modifier;


/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void							SPG_Modifier_Create
									(
										struct OBJ_tdst_GameObject_ *,
										MDF_tdst_Modifier *,
										void *
									);
extern void							SPG_Modifier_Destroy(MDF_tdst_Modifier *);
extern void							SPG_Modifier_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void							SPG_Modifier_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG						SPG_ul_Modifier_Load(MDF_tdst_Modifier *, char *);
extern void							SPG_Modifier_Save(MDF_tdst_Modifier *);
void 								SPG_Modifier_Interpolate(struct MDF_tdst_Modifier_ * p_Mod, unsigned int Globalmode , float fInterpolatedValue);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEOMODIFIER_H */
