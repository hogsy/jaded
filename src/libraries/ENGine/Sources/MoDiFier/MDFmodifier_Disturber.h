/*$T MDFmodifier_Disturber.h GC! 1.081 11/14/00 12:12:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_DISTURBER_H__
#define __MDFMODIFIER_DISTURBER_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Modifier const
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */

typedef struct	Disturber_tdst_Modifier_
{
	BOOL					bActive;
	FLOAT					fDisturbBoost;
	INT						nbDisturber;
	MATH_tdst_Vector		*ar_vDisturberWorldPos; 
	FLOAT					*ar_fDisturbances;

	BOOL					bStaticDisturber;
	FLOAT					fStaticDisturberForce;
	FLOAT					fStaticDisturberDelay;
	FLOAT					fCurDelay;
    FLOAT                   fZOffset;

    // WATER 3D
    BOOL                    bReflectOnWater;
	BOOL					bFloatOnWater;
	MATH_tdst_Vector		vFloatOnWaterInitialVelocity;
	FLOAT					fFloatOnWaterZOffset;
	FLOAT					fFloatOnWaterInitialZRotationSpeed;
	FLOAT					fFloatOnWaterBankingDamping;
	FLOAT					fFloatOnWaterZStability;
	FLOAT					fFloatOnWaterWaveStrength;
	FLOAT					fFloatOnWaterWaveInfluence;
	FLOAT					fFloatOnWaterVelocityBackToInitialStrength;
	FLOAT					vFloatOnWaterVelocityDamping;
	FLOAT					fFloatOnWaterEvaluationPlaneDelta;

	MATH_tdst_Vector		vFloatOnWaterVelocity;
	MATH_tdst_Matrix		stFloatOnWaterInitialPosition;
	MATH_tdst_Vector		vFloatOnWaterBanking;
	FLOAT					fFloatOnWaterYRotation;
	BOOL					bUseWaterBoundingBoxForCollision;

    // PAG
    BOOL                    bPAGDispersionActive;
    FLOAT                   fRadius;
    FLOAT                   fFrontForceTrans;
    FLOAT                   fFrontForceRot;
    FLOAT                   fRearForceTrans;
    FLOAT                   fRearForceRot;
    FLOAT                   fDraftNear;
    FLOAT                   fDraftFar;
    INT                     iIntensity;

/*	FLOAT					fCurParticleDelay;
	BOOL					bGenParticles;
	OBJ_tdst_GameObject*	pParticuleGenerator1;
	ULONG					nbParticules1;
	BOOL					bPonderateParticulesWithForce1;
	OBJ_tdst_GameObject*	pParticuleGenerator2;
	ULONG					nbParticules2;
	BOOL					bPonderateParticulesWithForce2;
	OBJ_tdst_GameObject*	pParticuleGenerator3;
	ULONG					nbParticules3;
	BOOL					bPonderateParticulesWithForce3;
*/
} Disturber_tdst_Modifier;

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void		Disturber_Modifier_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *,	void *);
extern void		Disturber_Modifier_Destroy(MDF_tdst_Modifier *);
extern void		Disturber_Modifier_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void		Disturber_Modifier_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG	Disturber_Modifier_Load(MDF_tdst_Modifier *, char *);
extern void		Disturber_Modifier_Save(MDF_tdst_Modifier *);
extern void		Disturber_Modifier_Reinit(MDF_tdst_Modifier *);

#ifdef __cplusplus
}
#endif
#endif 
