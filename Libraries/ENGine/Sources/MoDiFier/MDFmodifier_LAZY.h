/*$T MDFmodifier_XMEC.h GC! 1.100 08/29/01 14:29:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_LAZY_H__
#define __MDFMODIFIER_LAZY_H__

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
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    XMEC flags
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#define GAO_FLG_Lazy_Trans					0x00000001
#define GAO_FLG_Lazy_Rotation				0x00000002
#define GAO_FLG_Lazy_Scale					0x00000004
#define GAO_FLG_Lazy_Hierarchy				0x00000008
#define GAO_FLG_Lazy_Inherit_Trans			0x00000010
#define GAO_FLG_Lazy_Inherit_Rotation		0x00000020
#define GAO_FLG_Lazy_Inherit_Scale			0x00000040
#define GAO_FLG_Lazy_Inherit_Flags			0x00000080
#define GAO_FLG_Lazy_Inherit_Factor			0x00000100
#define GAO_FLG_Lazy_Inherit_MASK			(GAO_FLG_Lazy_Inherit_Trans|GAO_FLG_Lazy_Inherit_Rotation|GAO_FLG_Lazy_Inherit_Scale)
#ifdef ACTIVE_EDITORS
#define GAO_FLG_Lazy_DrawInfo				0x00000200
#endif
#define GAO_FLG_Lazy_UseBV					0x00000400
#define GAO_FLG_Lazy_Inherit_BV				0x00000800
#define GAO_FLG_Lazy_UseSemiMecanicalModel	0x00001000

#define GAO_FLG_Lazy_IsComputed				0x10000000

typedef struct	GAO_tdst_ModifierLazy_
{
	ULONG ulCodeKey; // used for retreive begining in editor mode
	ULONG ulFlags;   // 
	float LazyFactor;
	MATH_tdst_Matrix stSaveMatrix;

	ULONG SaveFlags;
	float SaveFactor;

	MATH_tdst_Matrix stCurrentMatrix;


	OBJ_tdst_GameObject *pst_GO;


	MATH_tdst_Vector	stMinBV;
	MATH_tdst_Vector	stMaxBV;

	MATH_tdst_Vector	SavedstMinBV;
	MATH_tdst_Vector	SavedstMaxBV;


	/* Semi-Mecanical model */
	float Gravity;
	float SaveGravity;
	MATH_tdst_Vector	stCurrentSpeed;
	float				Friction;
	float				SaveFriction;

	struct GAO_tdst_ModifierLazy_ *p_NextModifierLazy;

} GAO_tdst_ModifierLazy;

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */
extern void		GAO_ModifierLazy_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierLazy_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierLazy_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierLazy_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierLazy_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ul_ModifierLazy_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierLazy_Save(MDF_tdst_Modifier *);
extern void		GAO_ModifierLazy_DetectUsedTextures(char *pc_UsedIndex);
extern void		GAO_ModifierLazy_UnapplyAll();
#ifdef USE_DOUBLE_RENDERING	
void 			GAO_ModifierLazy_Interpolate
				(
					struct MDF_tdst_Modifier_ * , 
					u_int mode , 
					float fInterpolatedValue
				);
#endif



typedef struct	GAO_tdst_ModifierBoneRefine_
{
#ifdef ACTIVE_EDITORS
	ULONG				ulCodeKey;
#endif
	ULONG				ulMode;
	float				fInterpolValue;
} GAO_tdst_ModifierBoneRefine;

void GAO_ModifierBoneRefineInterpolate(struct MDF_tdst_Modifier_ * p_Mod, u32 mode , float fInterpolatedValue);
void GAO_ModifierBoneRefineCreate(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data);
void GAO_ModifierBoneRefineDestroy(MDF_tdst_Modifier *_pst_Mod);
void GAO_ModifierBoneRefineApply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
void GAO_ModifierBoneRefineUnapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj); 
void GAO_ModifierBoneRefineInit(GAO_tdst_ModifierBoneRefine *p_FOGDY);
void GAO_ModifierBoneRefineReinit(MDF_tdst_Modifier *_pst_Mod);
void BoneRefineModifier_Save(MDF_tdst_Modifier *_pst_Mod);
ULONG BoneRefineul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);
int BoneRefineModifier_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src );


/* DONT TOUCH THAT !!! */
#define MAX_BONEMECA_EXCLUDERS 8
/* DONT TOUCH THAT !!! */
#define MAX_BONEMECA_MOVERS 16

typedef struct	GAO_tdst_ModifierBoneMecaExcluder_
{
	uint32_t ulBoneRef;
	MATH_tdst_Vector SphereRadius;
	MATH_tdst_Vector DeltaPos;
} GAO_tdst_ModifierBoneMecaExcluder;

typedef struct	GAO_tdst_ModifierBoneMeca_
{
#ifdef ACTIVE_EDITORS
	uint32_t ulCodeKey;
	OBJ_tdst_GameObject					*_pst_GO;
	uint32_t							ulDrawBones;
#endif
	/* Saved Parrams */
	uint32_t ulNumberOfExcluders;
	GAO_tdst_ModifierBoneMecaExcluder	stBM[MAX_BONEMECA_EXCLUDERS];
	uint32_t ulNumberOfMovers;
	uint32_t stBR[ MAX_BONEMECA_MOVERS ];
	/* Bones speeds */
	MATH_tdst_Vector					stBS[MAX_BONEMECA_MOVERS];

	float								fGravity;
	float								fFriction;
	float								fRenormToFatherFactor;

	uint32_t LastRenderNum;

} GAO_tdst_ModifierBoneMeca;


void GAO_ModifierBoneMecaInterpolate(struct MDF_tdst_Modifier_ * p_Mod, u32 mode , float fInterpolatedValue);
void GAO_ModifierBoneMecaCreate(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data);
void GAO_ModifierBoneMecaDestroy(MDF_tdst_Modifier *_pst_Mod);
void GAO_ModifierBoneMecaApply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
void GAO_ModifierBoneMecaUnapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj); 
void GAO_ModifierBoneMecaInit(GAO_tdst_ModifierBoneMeca *p_FOGDY);
void GAO_ModifierBoneMecaReinit(MDF_tdst_Modifier *_pst_Mod);
void BoneMecaModifier_Save(MDF_tdst_Modifier *_pst_Mod);
ULONG BoneMecaul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);
int BoneMecaModifier_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src );


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
