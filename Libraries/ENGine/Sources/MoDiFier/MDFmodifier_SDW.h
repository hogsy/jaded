/*$T MDFmodifier_SDW.h */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __MDFMODIFIER_SDW_H__
#define __MDFMODIFIER_SDW_H__

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
    Modifier desc
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Shadow flags
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define MDF_SDW_1PerLight					0x00000001
#define MDF_SDW_1PerDirectional				0x00000002
#define MDF_SDW_1ObjectX					0x00100000
#define MDF_SDW_1ObjectY					0x00200000
#define MDF_SDW_1ObjectZ					0x00400000
#define MDF_SDW_1Vertical					0x00000004
#define MDF_SDW_InvertDirection				0x00800000
#define MDF_SDW_1RealTime					0x00000008
#define MDF_SDW_Volumetric					0x00000010
#define MDF_SDW_ApplyOnMyself				0x00000020
#define MDF_SDW_IsUpdatedOnThisDisplayData	0x00000040
#define MDF_SDW_ApplyOnDynam				0x00000080
#define MDF_SDW_ApplyOnNotDynam				0x00000100
#define MDF_SDW_90Turn						0x00000400
#define MDF_SDW_180Turn						0x00000800
#define MDF_SDW_EliminateSameFather			0x00001000
#define MDF_SDW_UseNormales					0x00004000
#define MDF_SDW_IsCulled					0x00020000
#define MDF_SDW_IsActivate					0x00040000
#define MDF_SDW_IsAdditional				0x00080000
#define MDF_SDW_AlwaysUseShadowColor		0x01000000

#define MDF_SDW_Projection_PlaneXY			0
#define MDF_SDW_Projection_PlaneXZ			1
#define MDF_SDW_Projection_PlaneYZ			2
#define MDF_SDW_Projection_CylinderX		3
#define MDF_SDW_Projection_CylinderY		4
#define MDF_SDW_Projection_CylinderZ		5
#define MDF_SDW_Projection_Spherical		6

#define MDF_SDW_DefaulFlag		(MDF_SDW_1Vertical | MDF_SDW_ApplyOnDynam | MDF_SDW_ApplyOnNotDynam | MDF_SDW_IsActivate | MDF_SDW_UseNormales | MDF_SDW_EliminateSameFather )

typedef struct  GAO_tdst_ModifierSDW_
{
	ULONG					ulFlags;
	BIG_KEY					TextureUsed;
	float					XSizeFactor;
	float					YSizeFactor;
	LONG					TextureIndex;
	struct  GAO_tdst_ModifierSDW_   *p_NextSdwMdf;
	OBJ_tdst_GameObject		*pst_GO;
	float					ZAttenuationFactor;
	ULONG					ulShadowColor;
	float					ZStart;
	MATH_tdst_Vector		stCenter;
	float					ZSizeFactor;
	ULONG					ulProjectionMethod;
	char					TextureTiling;

	char					uc_Version;
	USHORT					uw_Dummy;

	OBJ_tdst_GameObject		*pst_LightGO;
} GAO_tdst_ModifierSDW;

typedef struct DD_tdst_ShadowStackNode_
{
	GAO_tdst_ModifierSDW	*p_FatherModifier;
	MATH_tdst_Vector	stWorldVectZ,stWorldCenter;
	MATH_tdst_Vector	stWorldVectShadow,stWorldVectUOrtho,stWorldVectVOrtho;
	MATH_tdst_Vector	stGMin,stGMax;
	float			stUCoef,stVCoef,stZCoef,fZDepth,fStartFactor;
	ULONG			ulColor;
	ULONG			RDR_WindowAffectation;
} DD_tdst_ShadowStackNode;

#define MDF_MemoryGranularity 32

typedef struct DD_tdst_ShadowStack_
{
	ULONG					ulNumberOfNodes;
	ULONG					ulLastNode;
	DD_tdst_ShadowStackNode	*pp_Nodes;
} DD_tdst_ShadowStack;


/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
extern void     GAO_ModifierShadow_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void     GAO_ModifierShadow_Destroy(MDF_tdst_Modifier *);
extern void     GAO_ModifierShadow_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void     GAO_ModifierShadow_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void     GAO_ModifierShadow_Reinit(MDF_tdst_Modifier *);
extern ULONG    GAO_ModifierShadow_Load(MDF_tdst_Modifier *, char *);
extern void     GAO_ModifierShadow_Save(MDF_tdst_Modifier *);
extern void     GAO_ModifierShadow_DetectUsedTextures(char *pc_UsedIndex);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif 
