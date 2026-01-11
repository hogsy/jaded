/*$T MDFmodifier_XMEN.h */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef __MDFMODIFIER_XMEN_H__
#define __MDFMODIFIER_XMEN_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define XMEN_NumberOfSegs_PO2 3 // = 8


/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	XMEN flags
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define MOD_XMEN_Smooth				1
#define MOD_XMEN_GlobalMatrix		2
#define MOD_XMEN_TransMat			4
#define MOD_XMEN_Validate			8
#define MOD_XMEN_Centered			16
#define MOD_XMEN_TurnText90			32

typedef struct  GAO_tdst_XMEN_Chhlaahhh_
{
	ULONG					ulBonesNum;
	float					fLenght;
} GAO_tdst_XMEN_Chhlaahhh;

typedef struct  GAO_tdst_XMEN_Chhlaahhh_Memory_
{
	GEO_Vertex		stOldPos1;
	GEO_Vertex		stOldPos2;
} GAO_tdst_XMEN_Chhlaahhh_Memory;

typedef struct  GAO_tdst_ModifierXMEN_
{
#ifdef ACTIVE_EDITORS
	ULONG							ulCodeKey;
	ULONG							ulOLD_Number_Of_Chhlaahhh;
	ULONG							bGlobalMatrix;
	ULONG							bSmoothed;
	ULONG							bMaterialIsTransparent;
	ULONG							bCentered;
	ULONG							bTurn90;
#endif
	ULONG							ulFlags;
	ULONG							ulNumber_Of_Chhlaahhh;
	ULONG							ulCurrentRINGentry;
	ULONG							ulNumberOfSmooths;
	ULONG							ulProjectionMethod;
	BIG_KEY							bk_MaterialUsed;
	MAT_tdst_Material				*p_MaterialUsed;
	GAO_tdst_XMEN_Chhlaahhh			*p_st_Chhlaahhh;
	OBJ_tdst_GameObject				*pst_GO;
	GAO_tdst_XMEN_Chhlaahhh_Memory	*p_stXMEN_MEM;
	float							*p_XmenLocalLenght;
	float							fDTMin;
	float							fDTMinLocal;
	ULONG							ulUserID;
	ULONG 							ulFrameCounter;
} GAO_tdst_ModifierXMEN;

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
extern void     GAO_ModifierXMEN_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *, void *);
extern void     GAO_ModifierXMEN_Destroy(MDF_tdst_Modifier *);
extern void     GAO_ModifierXMEN_Apply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void     GAO_ModifierXMEN_Unapply(MDF_tdst_Modifier *, GEO_tdst_Object *);
extern void     GAO_ModifierXMEN_Reinit(MDF_tdst_Modifier *);
extern ULONG    GAO_ModifierXMEN_Load(MDF_tdst_Modifier *, char *);
extern void     GAO_ModifierXMEN_Save(MDF_tdst_Modifier *);
extern void     GAO_ModifierXMEN_DetectUsedTextures(char *pc_UsedIndex);
#ifdef USE_DOUBLE_RENDERING	
void 			GAO_ModifierXMEN_Interpolate
				(
					struct MDF_tdst_Modifier_ * , 
					u_int mode , 
					float fInterpolatedValue
				);
#endif

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif 
