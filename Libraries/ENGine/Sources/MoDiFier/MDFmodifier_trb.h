/*$T MDFmodifier_XSPRING.h */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __MDFMODIFIER_XSPRING_H__
#define __MDFMODIFIER_XSPRING_H__

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
    Modifier desc
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	XSPRING flags
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#define GOMEC_NUMBER 50
typedef struct  GAO_tdst_ModifierXSPRING_
{
#ifdef ACTIVE_EDITORS
	ULONG							ulCodeKey;
#endif
	BIG_KEY							bk_GOSPRING;
	OBJ_tdst_GameObject				*pst_GOSPRING;
	MATH_tdst_Matrix				stSavedMatrix;
	MATH_tdst_Vector					stSpeed;
	OBJ_tdst_GameObject				*pst_GO;
	ULONG							ulFlags;
	float							fDistance;
	float							fGravity;
	float							fTension;
	float							fGround;
	float							fElasticity;
} GAO_tdst_ModifierXSPRING;

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
extern void     GAO_ModifierXSPRING_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void     GAO_ModifierXSPRING_Destroy(MDF_tdst_Modifier *);
extern void     GAO_ModifierXSPRING_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void     GAO_ModifierXSPRING_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void     GAO_ModifierXSPRING_Reinit(MDF_tdst_Modifier *);
extern ULONG    GAO_ModifierXSPRING_Load(MDF_tdst_Modifier *, char *);
extern void     GAO_ModifierXSPRING_Save(MDF_tdst_Modifier *);
extern void     GAO_ModifierXSPRING_DetectUsedTextures(char *pc_UsedIndex);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef __cplusplus
}
#endif
#endif 
