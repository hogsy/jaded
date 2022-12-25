/*$T MDFmodifier_XMEC.h GC! 1.100 08/29/01 14:29:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_XMEC_H__
#define __MDFMODIFIER_XMEC_H__

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

#define XMEC_C_TakeY		0x00000001
#define XMEC_C_ComputeDist	0x00000002

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

#define GOMEC_NUMBER	50

typedef struct	GAO_tdst_ModifierXMEC_
{
#ifdef ACTIVE_EDITORS
	ULONG				ulCodeKey;
#endif
	BIG_KEY				bk_GOMEC;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_TrtGO;
	ULONG				ulFlags;
	float				fDistance;
	float				fGravity;
	float				fTension;
	float				fGround;
	float				fGroundFriction;
	float				fElasticity;
	MATH_tdst_Vector	stSpeed[GOMEC_NUMBER];
	MATH_tdst_Vector	stForces[GOMEC_NUMBER];
	MATH_tdst_Matrix	stSavedMatrix;
	int					i_NumBones;
	int					i_RefBone;
	int					ai_Bones[GOMEC_NUMBER];
	OBJ_tdst_GameObject *ap_BonesObj[GOMEC_NUMBER];
	MATH_tdst_Matrix	*ap_Bones[GOMEC_NUMBER];
} GAO_tdst_ModifierXMEC;

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void		GAO_ModifierXMEC_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void		GAO_ModifierXMEC_Destroy(MDF_tdst_Modifier *);
extern void		GAO_ModifierXMEC_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierXMEC_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void		GAO_ModifierXMEC_Reinit(MDF_tdst_Modifier *);
extern ULONG	GAO_ModifierXMEC_Load(MDF_tdst_Modifier *, char *);
extern void		GAO_ModifierXMEC_Save(MDF_tdst_Modifier *);
extern void		GAO_ModifierXMEC_DetectUsedTextures(char *pc_UsedIndex);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
