/*$T MDFstruct.h GC! 1.081 06/12/00 14:31:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __MDFSTRUCT_H__
#define __MDFSTRUCT_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Modifier const
 ***********************************************************************************************************************
 */

/* Modifier type */
#define MDF_C_Modifier_Snap									0
#define MDF_C_Modifier_OnduleTonCorps						1
#define MDF_C_Modifier_Explode                          	2
#define MDF_C_Modifier_LegLink								3
#define MDF_C_Modifier_Morphing								4
#define MDF_C_Modifier_SemiLookAt							5
#define MDF_C_Modifier_Shadow								6
#define MDF_C_Modifier_SpecialLookAt                        7
#define MDF_C_Modifier_Sound                                8
#define MDF_C_Modifier_XMEN									9
#define MDF_C_Modifier_XMEC									10
#define MDF_C_Modifier_SPG									11
#define MDF_C_Modifier_Symetrie                             12
#define MDF_C_Modifier_ROTR									13
#define MDF_C_Modifier_SNAKE								14
#define MDF_C_Modifier_SoundFx                              15
#define MDF_C_Modifier_PROTEX								16
#define MDF_C_Modifier_SaveAddMatrix                        17
#define MDF_C_Modifier_PAG									18
#define MDF_C_Modifier_SoundLoading                         19
#define MDF_C_Modifier_InfoPhoto							20
#define MDF_C_Modifier_StoreTransformedPoints               21
#define MDF_C_Modifier_Crush								22
#define MDF_C_Modifier_RLICarte								23
#define MDF_C_Modifier_Lazy									24
#define MDF_C_Modifier_GPG									25
#define MDF_C_Modifier_FUR									26
#define MDF_C_Modifier_VertexPerturb						27
#define MDF_C_Modifier_SpriteMapper2						28
#define MDF_C_Modifier_ODE									29
#define MDF_C_Modifier_MatrixBore							30
#define MDF_C_Modifier_GRID									31
#define MDF_C_Modifier_SoundVolume							32

// modifier Xenon->

#define MDF_C_Modifier_WATER3D                              33
#define MDF_C_Modifier_Disturber                            34
#define MDF_C_Modifier_Sfx                                  35
#define MDF_C_Modifier_RotationPaste                        36
#define MDF_C_Modifier_TranslationPaste                     37
#define MDF_C_Modifier_AnimatedGAO                          38
#define MDF_C_Modifier_Weather                              39
#define MDF_C_Modifier_SoftBody                             40
#define MDF_C_Modifier_Wind                                 41
#define MDF_C_Modifier_DYNFUR                               42
#define MDF_C_Modifier_SPG2Holder                           43

#define MDF_C_Modifier_Vine                                 49
// modifier Montpellier->

#define MDF_C_Modifier_FogDyn                               50                    // etait 33 a MTP
#define MDF_C_Modifier_FogDyn_Emiter                        51                    // etait 33 a MTP
#define MDF_C_Modifier_Half_Angle							52                    // etait 33 a MTP
#define MDF_C_Modifier_BoneMeca								53                    // etait 33 a MTP

#ifdef JADEFUSION //POPOWARNING 
#define MDF_C_Modifier_Number                               56
#else
#define MDF_C_Modifier_FClone								54
#endif

#define MDF_C_Modifier_UVTexWave							55
#ifndef JADEFUSION
#define MDF_C_Modifier_Number                               56
#endif

/* general modifier flags (high byte only) */
#define MDF_C_Modifier_ApplyGao 0x01000000
#define MDF_C_Modifier_Inactive 0x02000000
#define MDF_C_Modifier_ApplyGen 0x04000000 /* generic MDF */
#define MDF_C_Modifier_NoApply  0x08000000 /* generic MDF */
#ifdef JADEFUSION
#define MDF_C_Modifier_ApplyRender 0x10000000 /* apply when rendering the object*/
#endif


/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    Interface
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	MDF_tdst_ModifierInterface_
{
	ULONG	ul_Type;
	void (*pfnv_Create) (struct OBJ_tdst_GameObject_ *, struct MDF_tdst_Modifier_ *, void *);
	void (*pfnv_Destroy) (struct MDF_tdst_Modifier_ *);
	void (*pfnv_Apply) (struct MDF_tdst_Modifier_ *, struct GEO_tdst_Object_ *);
	void (*pfnv_Unapply) (struct MDF_tdst_Modifier_ *, struct GEO_tdst_Object_ *);
	ULONG (*pfnul_Load) (struct MDF_tdst_Modifier_ *, char *);
	void (*pfnv_Reinit) (struct MDF_tdst_Modifier_ *);
#ifdef USE_DOUBLE_RENDERING	
	void (*pfnv_Interpolate)(struct MDF_tdst_Modifier_ * , u_int mode , float fInterpolatedValue);
#endif	

#ifdef ACTIVE_EDITORS
    void (*pfnv_ApplyInGeom) (struct MDF_tdst_Modifier_ *, struct GEO_tdst_Object_ *);
	BOOL (*pfnb_CanBeApply) (struct OBJ_tdst_GameObject_ *, char *);
	void (*pfnv_Save) (struct MDF_tdst_Modifier_ *);
	int	 (*pfni_Copy) (struct MDF_tdst_Modifier_ *, struct MDF_tdst_Modifier_ * );
	void (*pfnv_Desactivate) (struct MDF_tdst_Modifier_ *, struct GEO_tdst_Object_ *);
#endif
}
MDF_tdst_ModifierInterface;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Main struct
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	MDF_tdst_Modifier_
{
	MDF_tdst_ModifierInterface	*i;
	ULONG						ul_Flags;
	void						*p_Data;
	struct MDF_tdst_Modifier_	*pst_Next;
	struct MDF_tdst_Modifier_	*pst_Prev;
	struct OBJ_tdst_GameObject_ *pst_GO;
	struct OBJ_tdst_Group_		*pst_Group;
} MDF_tdst_Modifier;

/*$4
 ***********************************************************************************************************************
    Modifier globals
 ***********************************************************************************************************************
 */

extern MDF_tdst_ModifierInterface	MDF_gast_ModifierInterface[MDF_C_Modifier_Number];
extern MDF_tdst_Modifier			*MDF_gpst_GlobalCreate;
#ifdef ACTIVE_EDITORS
extern char							*MDF_gasz_ModifierName[MDF_C_Modifier_Number];
#endif

/*$3
 =======================================================================================================================
    General
 =======================================================================================================================
 */

void								MDF_Modifier_Init(void);
MDF_tdst_Modifier					*MDF_pst_Modifier_Create(struct OBJ_tdst_GameObject_ *, int, void *);
void								MDF_Modifier_Destroy(MDF_tdst_Modifier *);
ULONG								MDF_ul_Modifier_Load(MDF_tdst_Modifier **, char *, int, void *);

void								MDF_ApplyAll(struct OBJ_tdst_GameObject_ *, int);
void								MDF_UnApplyAll(struct OBJ_tdst_GameObject_ *, int);
void								MDF_ApplyAllGao(struct OBJ_tdst_GameObject_ * );
void								MDF_UnApplyAllGao(struct OBJ_tdst_GameObject_ *);
#ifdef JADEFUSION
void								MDF_ApplyAllRender(struct OBJ_tdst_GameObject_ * );
void								MDF_UnApplyAllRender(struct OBJ_tdst_GameObject_ *);
#endif
#ifdef USE_DOUBLE_RENDERING	
void 								MDF_InterpolateAll(struct OBJ_tdst_GameObject_ *_pst_GO,u_int Mode , float fInterpoler);
#endif
void								MDF_Reinit(struct OBJ_tdst_GameObject_ *);

MDF_tdst_Modifier					*MDF_pst_GetByType(struct OBJ_tdst_GameObject_ *, int);

#ifdef JADEFUSION
void								MDF_Modifier_DelInGameObject(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *);
#else
#ifdef ACTIVE_EDITORS
void								MDF_Modifier_DelInGameObject(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *);
#endif
#endif

#ifdef ACTIVE_EDITORS
void								MDF_Modifier_ReplaceInGameObject
									(
										struct OBJ_tdst_GameObject_ *,
										MDF_tdst_Modifier *,
										MDF_tdst_Modifier *
									);

void								MDF_Modifier_Save(MDF_tdst_Modifier *, int);
int									MDF_Modifier_Copy(MDF_tdst_Modifier *, MDF_tdst_Modifier * );
#endif
void								MDF_Modifier_AddToGameObject(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *);
void								MDF_Modifier_DuplicateList(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *);
struct OBJ_tdst_Group_				*MDF_Modifier_GetCurGroup(MDF_tdst_Modifier *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __MDFSTRUCT_H__ */
