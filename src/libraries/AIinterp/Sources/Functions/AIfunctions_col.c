/*$T AIfunctions_col.c GC! 1.081 03/18/02 18:00:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLmain.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "ENGine/Sources/COLlision/COLreport.h"
#include "ENGine/Sources/COLlision/COLzdx.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/COLlision/COLinit.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "BASe/BAStypes.h"
#include "ENGine/Sources/OBJects/OBJorient.h"

#ifdef RASTERS_ON
PRO_tdst_TrameRaster	ENG_gpst_RasterEng_RayCast;
PRO_tdst_TrameRaster	ENG_gpst_RasterEng_RayDist;
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern COL_tdst_GlobalVars	COL_gst_GlobalVars;
extern ULONG				COL_ul_SnP_RefreshType;
extern USHORT 				COL_guw_UserCrossable;
extern float				COL_gf_MinDistance;
extern BOOL					COL_gb_UseMinDistance;

/*$F
 ===================================================================================================
    Aim:    Returns an information concerning the Size of the BV.

    Note:	If AABBox, returns the Max Half Length of the 3 axis.
			If Sphere, returns the radius.
 ===================================================================================================
 */

float AI_EvalFunc_BVSizeGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Max, *pst_Min;
	float				f_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(OBJ_BV_IsAABBox(_pst_GO->pst_BV))
	{
		pst_Max = OBJ_pst_BV_GetGMax(_pst_GO->pst_BV);
		pst_Min = OBJ_pst_BV_GetGMin(_pst_GO->pst_BV);
		f_Max = fMax3(pst_Max->x - pst_Min->x, pst_Max->y - pst_Min->y, pst_Max->z - pst_Min->z);
		return(fHalf(f_Max));
	}
	else
		return(OBJ_f_BV_GetRadius(_pst_GO->pst_BV));
}
/**/
AI_tdst_Node *AI_EvalFunc_BVSizeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_BVSizeGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_BVSizeGetMin_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Max, *pst_Min;
	float				f_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(OBJ_BV_IsAABBox(_pst_GO->pst_BV))
	{
		pst_Max = OBJ_pst_BV_GetGMax(_pst_GO->pst_BV);
		pst_Min = OBJ_pst_BV_GetGMin(_pst_GO->pst_BV);
		f_Max = fMin3(pst_Max->x - pst_Min->x, pst_Max->y - pst_Min->y, pst_Max->z - pst_Min->z);
		return(fHalf(f_Max));
	}
	else
		return(OBJ_f_BV_GetRadius(_pst_GO->pst_BV));
}
/**/
AI_tdst_Node *AI_EvalFunc_BVSizeGetMin(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_BVSizeGetMin_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns a Random pos in the BV of the object. We can choose to find a Plan(x, y) random point or a Space
            random point in the BV.
 =======================================================================================================================
 */
void AI_EvalFunc_BVRandomPosGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_PlanZ, MATH_tdst_Vector *_pst_Random)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Random, st_Max, st_Min, st_Center;
	float				f_Radius;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(OBJ_BV_IsAABBox(_pst_GO->pst_BV))
	{
		MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
		MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));

		_pst_Random->x = fRand(st_Min.x, st_Max.x);
		_pst_Random->y = fRand(st_Min.y, st_Max.y);

		/* If ul_PlanZ = 0 -> Space random. else Plan (x,y) Random */
		_pst_Random->z = _ul_PlanZ ? st_Min.z + fHalf((st_Max.z - st_Min.z)) : fRand(st_Min.z, st_Max.z);

		return;
	}
	else
	{
		MATH_TransformVertexNoScale
		(
			&st_Center,
			OBJ_pst_GetAbsoluteMatrix(_pst_GO),
			OBJ_pst_BV_GetCenter(_pst_GO->pst_BV)
		);
		f_Radius = OBJ_f_BV_GetRadius(_pst_GO->pst_BV);

		st_Max.x = st_Center.x + f_Radius;
		st_Max.y = st_Center.y + f_Radius;
		st_Max.z = st_Center.z + f_Radius;

		st_Min.x = st_Center.x - f_Radius;
		st_Min.y = st_Center.y - f_Radius;
		st_Min.z = st_Center.z - f_Radius;

		_pst_Random->x = fRand(st_Min.x, st_Max.x);
		_pst_Random->y = fRand(st_Min.y, st_Max.y);

		/* If ul_PlanZ = 0 -> Space random. else Plan (x,y) Random */
		st_Random.z = _ul_PlanZ ? st_Center.z : fRand(st_Min.z, st_Max.z);

		return;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_BVRandomPosGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_PlanZ;
	MATH_tdst_Vector	st_Random;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_PlanZ = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_BVRandomPosGet_C(pst_GO, ul_PlanZ, &st_Random);

	AI_PushVector(&st_Random);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Get the Min point of the AABBox
 =======================================================================================================================
 */
void AI_EvalFunc_BVMinGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Point)
{
	if(OBJ_BV_IsAABBox(_pst_GO->pst_BV))
		MATH_CopyVector(_pst_Point, OBJ_pst_BV_GetGMinInit(_pst_GO->pst_BV));
	else
		MATH_CopyVector(_pst_Point, &MATH_gst_NulVector);
}
/**/
AI_tdst_Node *AI_EvalFunc_BVMinGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_BVMinGet_C(pst_GO, &st_Pos);

	AI_PushVector(&st_Pos);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Get the Min point of the OBBox
 =======================================================================================================================
 */
void AI_EvalFunc_BVOBBoxMinGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Point)
{
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_OBBox)
		MATH_CopyVector(_pst_Point, OBJ_pst_BV_GetLMin(_pst_GO->pst_BV));
	else
		MATH_CopyVector(_pst_Point, &MATH_gst_NulVector);
}
/**/
AI_tdst_Node *AI_EvalFunc_BVOBBoxMinGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_BVOBBoxMinGet_C(pst_GO, &st_Pos);

	AI_PushVector(&st_Pos);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Get the Min point of the OBBox
 =======================================================================================================================
 */
void AI_EvalFunc_BVOBBoxMaxGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Point)
{
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_OBBox)
		MATH_CopyVector(_pst_Point, OBJ_pst_BV_GetLMax(_pst_GO->pst_BV));
	else
		MATH_CopyVector(_pst_Point, &MATH_gst_NulVector);
}
/**/
AI_tdst_Node *AI_EvalFunc_BVOBBoxMaxGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_BVOBBoxMaxGet_C(pst_GO, &st_Pos);

	AI_PushVector(&st_Pos);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Set the Min point of the AABBox
 =======================================================================================================================
 */
void AI_EvalFunc_BVMinSet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Point)
{
	if(OBJ_BV_IsAABBox(_pst_GO->pst_BV))
	{
		MATH_CopyVector(OBJ_pst_BV_GetGMinInit(_pst_GO->pst_BV), _pst_Point);
		MATH_CopyVector(OBJ_pst_BV_GetGMin(_pst_GO->pst_BV), _pst_Point);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_BVMinSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Min;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_Min);
	AI_M_GetCurrentObject(pst_GO);
	pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EditableBV;
	AI_EvalFunc_BVMinSet_C(pst_GO, &st_Min);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Set the Min point of the OBBox
 =======================================================================================================================
 */
void AI_EvalFunc_BVOBBoxMinSet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Point)
{
	_pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EditableBV;

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_OBBox))
	{
		_pst_GO->pst_BV = MEM_p_Realloc(_pst_GO->pst_BV, sizeof(OBJ_tdst_ComplexBV));
		_pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_OBBox;
	}

	MATH_CopyVector(OBJ_pst_BV_GetLMin(_pst_GO->pst_BV), _pst_Point);
}
/**/
AI_tdst_Node *AI_EvalFunc_BVOBBoxMinSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Min;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_Min);
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_BVOBBoxMinSet_C(pst_GO, &st_Min);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Set the Max point of the OBBox
 =======================================================================================================================
 */
void AI_EvalFunc_BVOBBoxMaxSet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Point)
{
	_pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EditableBV;

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_OBBox))
	{
		_pst_GO->pst_BV = MEM_p_Realloc(_pst_GO->pst_BV, sizeof(OBJ_tdst_ComplexBV));
		_pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_OBBox;
	}

	MATH_CopyVector(OBJ_pst_BV_GetLMax(_pst_GO->pst_BV), _pst_Point);
}
/**/
AI_tdst_Node *AI_EvalFunc_BVOBBoxMaxSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_Max);
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_BVOBBoxMaxSet_C(pst_GO, &st_Max);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Get the Max point of the AABBox
 =======================================================================================================================
 */
void AI_EvalFunc_BVMaxGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Point)
{
	if(OBJ_BV_IsAABBox(_pst_GO->pst_BV))
		MATH_CopyVector(_pst_Point, OBJ_pst_BV_GetGMaxInit(_pst_GO->pst_BV));
	else
		MATH_CopyVector(_pst_Point, &MATH_gst_NulVector);
}
/**/
AI_tdst_Node *AI_EvalFunc_BVMaxGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_BVMaxGet_C(pst_GO, &st_Pos);

	AI_PushVector(&st_Pos);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Set the Max point of the AABBox
 =======================================================================================================================
 */
void AI_EvalFunc_BVMaxSet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Point)
{
	if(OBJ_BV_IsAABBox(_pst_GO->pst_BV))
	{
		MATH_CopyVector(OBJ_pst_BV_GetGMaxInit(_pst_GO->pst_BV), _pst_Point);
		MATH_CopyVector(OBJ_pst_BV_GetGMax(_pst_GO->pst_BV), _pst_Point);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_BVMaxSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Min;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_Min);
	AI_M_GetCurrentObject(pst_GO);
	pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EditableBV;	
	AI_EvalFunc_BVMaxSet_C(pst_GO, &st_Min);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Recomput the BV
 =======================================================================================================================
 */
void AI_EvalFunc_BVRecompute_C( OBJ_tdst_GameObject *_pst_GO )
{
    OBJ_ComputeBV(_pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);
}
/**/
AI_tdst_Node *AI_EvalFunc_BVRecompute(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_BVRecompute_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Uses the SnP to determine the number of obejcts that overlaps the current one.
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLBVGetNbOverlaps_C(OBJ_tdst_GameObject *_pst_GO)
{
	/* Checking that the current GO is in the SnP */
	if(!(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_World = WOR_World_GetWorldOfObject(_pst_GO);
		if(pst_World)
		{
			_pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
			INT_SnP_AddObject(_pst_GO, pst_World->pst_SnP);
			INT_SnP_InsertionSort(pst_World->pst_SnP, 0);
		}

		ERR_X_Warning(0, "(COL_BVGetNbOverlaps) EnableSnP Control Flag set for GO. SAVE the map", _pst_GO->sz_Name);
	}

	return(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_List->ul_NbCollidedObjects);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBVGetNbOverlaps(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_Check(pst_GO->pst_Extended != NULL, "No Extended allocated");
	AI_Check(pst_GO->pst_Extended->pst_Col != NULL, "No Col allocated");
	AI_Check(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_List != NULL, "No SnP allocated");

	AI_PushInt(AI_EvalFunc_COLBVGetNbOverlaps_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Gets the first overlapping object that correponds to both filters (SnP)
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLBVGetFirstOverlap_C
(
	OBJ_tdst_GameObject *_pst_GO,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_DetectionList	*pst_DL;
	OBJ_tdst_GameObject		**dpst_Object, **dpst_LastObject;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Checking that the current GO is in the SnP */
	if(!(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_World = WOR_World_GetWorldOfObject(_pst_GO);
		if(pst_World)
		{
			_pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
			INT_SnP_AddObject(_pst_GO, pst_World->pst_SnP);
			INT_SnP_InsertionSort(pst_World->pst_SnP, 0);
		}

		ERR_X_Warning(0, "(COL_BVGetFirstOverlap) EnableSnP Control Flag set for GO. SAVE the map", _pst_GO->sz_Name);
	}

	pst_DL = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_List;

	dpst_Object = pst_DL->dpst_CollidedObject;
	dpst_LastObject = dpst_Object + pst_DL->ul_NbCollidedObjects;
	for(; dpst_Object < dpst_LastObject; dpst_Object++)
	{
		if(OBJ_b_TestFlag(*dpst_Object, _ul_OnFlags, _ul_OffFlags, _ul_FlagID))
		{
			return(*dpst_Object);
		}
	}

	return NULL;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBVGetFirstOverlap(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);

	AI_Check(pst_GO->pst_Extended != NULL, "No Extended allocated");
	AI_Check(pst_GO->pst_Extended->pst_Col != NULL, "No Col allocated");
	AI_Check(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_List != NULL, "No SnP allocated");

	AI_PushGameObject(AI_EvalFunc_COLBVGetFirstOverlap_C(pst_GO, ul_OnFlags, ul_OffFlags, ul_FlagID));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the list of the objects that collides the current one by using the SnP results.

    Note:   Filter on the Identity Flags
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLBVGetAllOverlaps_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_DetectionList	*pst_DL;
	ULONG					ul_NbFiltered;
	OBJ_tdst_GameObject		**dpst_Object, **dpst_LastObject;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Checking that the current GO is in the SnP */
	if(!(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_World = WOR_World_GetWorldOfObject(_pst_GO);
		if(pst_World)
		{
			_pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
			INT_SnP_AddObject(_pst_GO, pst_World->pst_SnP);
			INT_SnP_InsertionSort(pst_World->pst_SnP, 0);
		}

		ERR_X_Warning(0, "(COL_BVGetAllOverlaps) EnableSnP Control Flag set for GO. SAVE the map", _pst_GO->sz_Name);
	}

	pst_DL = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_List;

	dpst_Object = pst_DL->dpst_CollidedObject;
	dpst_LastObject = dpst_Object + pst_DL->ul_NbCollidedObjects;
	for(ul_NbFiltered = 0; dpst_Object < dpst_LastObject; dpst_Object++)
	{
		if(OBJ_b_TestFlag(*dpst_Object, _ul_OnFlags, _ul_OffFlags, _ul_FlagID))
		{
			L_memcpy(_ppst_Array + ul_NbFiltered, dpst_Object, sizeof(OBJ_tdst_GameObject *));
			ul_NbFiltered++;
		}
	}

#if defined(PSX2_TARGET) && defined(__CW__)
	* (_ppst_Array + ul_NbFiltered) = (LONG) NULL;
#else
	*(_ppst_Array + ul_NbFiltered) = NULL;
#endif
	return ul_NbFiltered;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBVGetAllOverlaps(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	AI_PopVar(&Val, &st_Var);

	AI_M_GetCurrentObject(pst_GO);

	AI_Check(pst_GO->pst_Extended != NULL, "No Extended allocated");
	AI_Check(pst_GO->pst_Extended->pst_Col != NULL, "No Col allocated");
	AI_Check(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_List != NULL, "No SnP allocated");

	AI_PushInt
	(
		AI_EvalFunc_COLBVGetAllOverlaps_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Boolean operation. Are the two objects overlapping ?
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLBVObjectObject_C(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	if(!_pst_B)
		return 0;
	else
	{
		return(COL_b_BVOverlap(_pst_A, _pst_B, FALSE, FALSE, TRUE));
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBVObjectObject(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Obj, *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Obj = AI_PopGameObject();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLBVObjectObject_C(pst_GO, pst_Obj));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Ray Tracing along the direction given and within the given distance.

    Note:   In that function, the direction vector MUST be normaized. If not, use the COLRAYBVVECTOR function.
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLRayBVDist_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Dir,
	float				_f_Dist,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_RayBV
			(
				&pst_World->st_ActivObjects,
				_pst_Origin,
				_pst_Dir,
				_f_Dist,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				TRUE
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRayBVDist(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Origin, st_Direction;
	float				f_Dist;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	f_Dist = AI_PopFloat();
	AI_PopVector(&st_Direction);
	AI_PopVector(&st_Origin);

	AI_M_GetCurrentObject(pst_GO);

	AI_PushGameObject(AI_EvalFunc_COLRayBVDist_C(pst_GO, &st_Origin, &st_Direction, f_Dist, ul_OnFlags, ul_OffFlags, ul_FlagID));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Idem the previous one but this time, we search aLONG the given vector.

    Note:   This function is not as fast as the previous one.
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLRayBVVector_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Dir,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Dist, f_InvDist;
	MATH_tdst_Vector	st_Direction;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	/* Get the norm of the vector */
	f_Dist = MATH_f_NormVector(_pst_Dir);

	/* Compute the unit vector of the ray. */
	f_InvDist = fInv(f_Dist);
	st_Direction.x = fMul(_pst_Dir->x, f_InvDist);
	st_Direction.y = fMul(_pst_Dir->y, f_InvDist);
	st_Direction.z = fMul(_pst_Dir->z, f_InvDist);

	return
		(
			COL_RayBV
			(
				&pst_World->st_ActivObjects,
				_pst_Origin,
				&st_Direction,
				f_Dist,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				TRUE
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRayBVVector(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Origin, st_Direction;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	AI_PopVector(&st_Direction);
	AI_PopVector(&st_Origin);

	AI_M_GetCurrentObject(pst_GO);

	AI_PushGameObject(AI_EvalFunc_COLRayBVVector_C(pst_GO, &st_Origin, &st_Direction, ul_OnFlags, ul_OffFlags, ul_FlagID));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Boolean operation. Is there a collision between the current Object and a particular type of obstacles
            (Groung, wall) ?
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLCollideType_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_Report_CollideType(_pst_GO, pst_World, _ul_OnFlags));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLCollideType(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLCollideType_C(pst_GO, ul_OnFlags));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLHasGMat_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_Report_HasGMat(_pst_GO, pst_World, _ul_OnFlags));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLHasGMat(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLHasGMat_C(pst_GO, ul_OnFlags));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    
 =======================================================================================================================
 */
int AI_EvalFunc_COLHasZDM_C(OBJ_tdst_GameObject *_pst_GO)
{
	if
	(
		!_pst_GO
	||	!_pst_GO->pst_Extended
	||	!_pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance
	) 
		return FALSE;
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Instance	*pst_Instance;
		COL_tdst_ZDx		*pst_ZDx;
		int					i;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
		for(i = 0; i < (int) pst_Instance->uc_NbOfZDx; i++)
		{
			pst_ZDx = COL_pst_Instance_GetZoneWithIndex(pst_Instance, (UCHAR) i);
			if(pst_ZDx && (pst_ZDx->uc_Flag & COL_C_Zone_ZDM))
				return TRUE;
		}
	}

	return FALSE;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLHasZDM(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLHasZDM_C(pst_GO));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLBestWallGaoGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Dir,
	int				*_pul_NumIndex
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_Report_GetBestWallGOInDirection(_pst_GO, pst_World, _pst_Dir, 0.0f, (ULONG*)_pul_NumIndex));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBestWallGaoGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Direction;
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVector(&st_Direction);
	AI_M_GetCurrentObject(pst_GO);

	AI_PushGameObject(AI_EvalFunc_COLBestWallGaoGet_C(pst_GO, &st_Direction, (int *) st_Var1.pv_Addr));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLSpecialBestWallGaoGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Dir,
	ULONG				*_pul_NumIndex
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_Report_GetBestWallGOInDirectionSpecial(_pst_GO, pst_World, _pst_Dir, 0.0f, _pul_NumIndex));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLSpecialBestWallGaoGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Direction;
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	AI_PopVector(&st_Direction);
	AI_M_GetCurrentObject(pst_GO);

	AI_PushGameObject(AI_EvalFunc_COLSpecialBestWallGaoGet_C(pst_GO, &st_Direction, (ULONG *) st_Var1.pv_Addr));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLBestWallAngleGaoGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Dir,
	float				_f_CosAngle,
	int				    *_pul_NumIndex
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_Report_GetBestWallGOInDirection(_pst_GO, pst_World, _pst_Dir, _f_CosAngle, (ULONG*)_pul_NumIndex));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBestWallAngleGaoGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Direction;
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	float				f_CosAngle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	f_CosAngle = AI_PopFloat();
	AI_PopVector(&st_Direction);
	AI_M_GetCurrentObject(pst_GO);

	AI_PushGameObject(AI_EvalFunc_COLBestWallAngleGaoGet_C(pst_GO, &st_Direction, f_CosAngle, (int*) st_Var1.pv_Addr));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the Normal of the first obstacle corresponding to the given type.
 =======================================================================================================================
 */
void AI_EvalFunc_COLNormalGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags, MATH_tdst_Vector *_pst_Norm)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector	st_Normal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	COL_Report_GetNormal(_pst_GO, pst_World, &st_Normal, _ul_OnFlags);

	if(!MATH_b_NulVector(&st_Normal))
		MATH_CopyVector(_pst_Norm, &st_Normal);
	else
		MATH_CopyVector(_pst_Norm, &MATH_gst_NulVector);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLNormalGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Normal;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLNormalGet_C(pst_GO, ul_OnFlags, &st_Normal);

	AI_PushVector(&st_Normal);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim: 
 =======================================================================================================================
 */
int AI_EvalFunc_COLReportFlagGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return (COL_Report_GetFlags(_pst_GO, pst_World, _ul_OnFlags));

}
/**/
AI_tdst_Node *AI_EvalFunc_COLReportFlagGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLReportFlagGet_C(pst_GO, ul_OnFlags));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
    Aim:    Force the SnP TOTAL Refresh in the current Frame.
 =======================================================================================================================
 */
void AI_EvalFunc_COLForceSnPRefresh_C(void)
{
	COL_ul_SnP_RefreshType = COL_SnP_RefreshXYZ;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLForceSnPRefresh(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_COLForceSnPRefresh_C();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the Normal of the first obstacle corresponding to the given type.
 =======================================================================================================================
 */
void AI_EvalFunc_COLPointGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags, MATH_tdst_Vector *_pst_P)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector	*pst_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	pst_Point = COL_Report_GetCollidedPoint(_pst_GO, pst_World, _ul_OnFlags);
	if(pst_Point)
		MATH_CopyVector(_pst_P, pst_Point);
	else
		MATH_CopyVector(_pst_P, &MATH_gst_NulVector);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLPointGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	MATH_tdst_Vector	st_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLPointGet_C(pst_GO, ul_OnFlags, &st_Point);

	AI_PushVector(&st_Point);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the Normal of the first obstacle corresponding to the given type.
 =======================================================================================================================
 */
void AI_EvalFunc_COLGMatPointGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	ULONG				_ul_GMatFlags,
	ULONG				*_pul_NumReport,
	MATH_tdst_Vector	*_pst_Pt
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector	*pst_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	pst_Point = COL_Report_GMat_GetCollidedPoint(_pst_GO, pst_World, _ul_GMatFlags, _pul_NumReport);
	if(pst_Point)
		MATH_CopyVector(_pst_Pt, pst_Point);
	else
		MATH_CopyVector(_pst_Pt, &MATH_gst_NulVector);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGMatPointGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_GMat;
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	MATH_tdst_Vector	st_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	ul_GMat = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLGMatPointGet_C(pst_GO, ul_GMat, (ULONG *) st_Var1.pv_Addr, &st_Point);

	AI_PushVector(&st_Point);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
    Aim:    Returns the Slide of the first GameMaterial corresponding to the given type.
 =======================================================================================================================
 */
float AI_EvalFunc_COLGMSlideGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_GameMaterial_GetSlide(_pst_GO, pst_World, _ul_OnFlags));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGMSlideGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushFloat(AI_EvalFunc_COLGMSlideGet_C(pst_GO, ul_OnFlags));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the Flags of the first GameMaterial corresponding to the given type.
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLGMFlagsGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_GameMaterial_GetFlags(_pst_GO, pst_World, _ul_OnFlags));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGMFlagsGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLGMFlagsGet_C(pst_GO, ul_OnFlags));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
    Aim:    Returns the Rebound of the first GameMaterial corresponding to the given type.
 =======================================================================================================================
 */
float AI_EvalFunc_COLGMReboundGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_GameMaterial_GetRebound(_pst_GO, pst_World, _ul_OnFlags));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGMReboundGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushFloat(AI_EvalFunc_COLGMReboundGet_C(pst_GO, ul_OnFlags));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the Sound of the first GameMaterial corresponding to the given type.
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLGMSoundGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_GameMaterial_GetSound(_pst_GO, pst_World, _ul_OnFlags));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGMSoundGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLGMSoundGet_C(pst_GO, ul_OnFlags));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
    Aim:    Returns the Sound of the first GameMaterial corresponding to the given type.
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLDesignGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return((ULONG) COL_GetDesign(_pst_GO, pst_World, _ul_OnFlags));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLDesignGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLDesignGet_C(pst_GO, ul_OnFlags));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the Sound of the first GameMaterial corresponding to the given type.
 =======================================================================================================================
 */
int AI_EvalFunc_COLGMIDGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	USHORT			uw_ID;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	uw_ID = COL_GameMaterial_GetID(_pst_GO, pst_World, _ul_OnFlags);
	if(uw_ID == 0xFFFF) return -1;	/* Short / long casting problem ... (long) (-1 short) != -1 long */
	else
		return((int) COL_GameMaterial_GetID(_pst_GO, pst_World, _ul_OnFlags));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGMIDGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLGMIDGet_C(pst_GO, ul_OnFlags));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the first Gao corresponding to the given type.
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLGaoGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_OnFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_Report_GetCOBGameObject(_pst_GO, pst_World, _ul_OnFlags));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGaoGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_OnFlags = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushGameObject(AI_EvalFunc_COLGaoGet_C(pst_GO, ul_OnFlags));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns TRUE or FALSE depending on the fact that the current GO has one particular type of zone or not
            (Fight for ex)
 =======================================================================================================================
 */
int AI_EvalFunc_COLHasZone_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_Instance;
	UCHAR				uc_ENG_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO || !_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Col) return 0;

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;

	if(!pst_Instance || !pst_Instance->pst_ColSet) return 0;

	uc_ENG_Index = pst_Instance->pst_ColSet->pauc_AI_Indexes[_ul_AI_Index];

	if(uc_ENG_Index >= pst_Instance->uc_NbOfZDx)
		return 0;
	else
		return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLHasZone(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_AI_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLHasZone_C(pst_GO, ul_AI_Index));

	return ++_pst_Node;
}

/*$F
 ===================================================================================================
    Aim:    Returns the (Filtered) list of the objects whose one ZDE collides a particular ZDE of
            the current object.
	In:		- First Element of an Array where we can store the list of objects.
			- Filter (AI, Groups ? ...)
			- Index of the particular ZDE to Test.
 ===================================================================================================
 */

ULONG AI_EvalFunc_COLZDEZDEListGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_IndexA,
	ULONG				_ul_IndexB,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreate
			(
				_pst_GO,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				(UCHAR) _ul_IndexA,
				pst_World,
				(ULONG *) _ppst_Array,
				(UCHAR) _ul_IndexB
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLZDEZDEListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_Index, ul_DestIndex;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	ul_DestIndex = AI_PopInt();
	ul_Index = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLZDEZDEListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_Index,
			ul_DestIndex,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);

	return ++_pst_Node;
}

/*$F
 ===================================================================================================
    Aim:    Returns the (Filtered) list of the objects whose BV collides a particular ZDE of
            the current object.
	In:		- First Element of an Array where we can store the list of objects.
			- Filter (AI, Groups ? ...)
			- Index of the particular ZDE to Test.
 ===================================================================================================
 */

ULONG AI_EvalFunc_COLZDEBVListGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_IndexA,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreate
			(
				_pst_GO,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				(UCHAR) _ul_IndexA,
				pst_World,
				(ULONG *) _ppst_Array,
				(UCHAR) 0xFF
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLZDEBVListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_Index;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	ul_Index = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLZDEBVListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_Index,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);

	return ++_pst_Node;
}

/*$F
 ===================================================================================================
    Aim:    Returns the (Filtered) list of the objects whose ZDE collides the BV of the current object.
	In:		- First Element of an Array where we can store the list of objects.
			- Filter (AI, Groups ? ...)
			- Index of the particular ZDE to Test.
 ===================================================================================================
 */

ULONG AI_EvalFunc_COLBVZDEListGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_IndexA,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreate
			(
				_pst_GO,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				(UCHAR) _ul_IndexA,
				pst_World,
				(ULONG *) _ppst_Array,
				(UCHAR) 0xFE
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBVZDEListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_Index;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	ul_Index = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLBVZDEListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_Index,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);

	return ++_pst_Node;
}

/*$F
 ===================================================================================================
    Aim:    Returns the (Filtered) list of the objects whose Pivot collides the BV of the current object.
	In:		- First Element of an Array where we can store the list of objects.
			- Filter (AI, Groups ? ...)
 ===================================================================================================
 */

ULONG AI_EvalFunc_COLBVPivotListGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreate
			(
				_pst_GO,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				0,
				pst_World,
				(ULONG *) _ppst_Array,
				(UCHAR) 0xFD
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBVPivotListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLBVPivotListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);

	return ++_pst_Node;
}

/*$F
 ===================================================================================================
 ===================================================================================================
 */
ULONG AI_EvalFunc_COLDistanceListGet_C
(
	OBJ_tdst_GameObject	*_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	MATH_tdst_Vector	*_pst_GCS_Center,
	float				_f_MaxDist,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID,
	ULONG				_ul_MaxNumber
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreateDistance
			(
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				pst_World,
				(ULONG *) _ppst_Array,
				_pst_GCS_Center,
				_f_MaxDist,
				_ul_MaxNumber
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLDistanceListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	ULONG				ul_MaxNumber;
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector	st_GCS_Center;
	float				f_MaxDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_MaxNumber = AI_PopInt();
	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	f_MaxDist = AI_PopFloat();
	AI_PopVector(&st_GCS_Center);
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLDistanceListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			&st_GCS_Center,
			f_MaxDist,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID,
			ul_MaxNumber
		)
	);

	return ++_pst_Node;
}

extern ULONG COL_ListCreateSight
(
	OBJ_tdst_GameObject	*_pst_GO,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID,
	WOR_tdst_World		*_pst_World,
	ULONG				*_pul_Array,
	MATH_tdst_Vector	*_pst_GCS_Center,
	MATH_tdst_Vector	*_pst_Sight,
	float				_f_MaxDist,
	float				_f_MaxAngle,
	ULONG				_ul_MaxDist
);

/*$F
 ===================================================================================================
 ===================================================================================================
 */
ULONG AI_EvalFunc_COLSightListGet_C
(
	OBJ_tdst_GameObject	*_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	MATH_tdst_Vector	*_pst_GCS_Center,
	MATH_tdst_Vector	*_pst_Sight,
	float				_f_MaxDist,
	float				_f_MaxAngle,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID,
	ULONG				_ul_MaxNumber
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreateSight
			(
				_pst_GO,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				pst_World,
				(ULONG *) _ppst_Array,
				_pst_GCS_Center,
				_pst_Sight,
				_f_MaxDist,
				_f_MaxAngle,
				_ul_MaxNumber
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLSightListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	ULONG				ul_MaxNumber;
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector	st_GCS_Center;
	MATH_tdst_Vector	st_Sight;
	float				f_MaxDist, f_MaxAngle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_MaxNumber = AI_PopInt();
	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	f_MaxAngle = AI_PopFloat();
	f_MaxDist = AI_PopFloat();
	AI_PopVector(&st_Sight);
	AI_PopVector(&st_GCS_Center);
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLSightListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			&st_GCS_Center,
			&st_Sight,
			f_MaxDist,
			f_MaxAngle,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID,
			ul_MaxNumber
		)
	);

	return ++_pst_Node;
}

/*$F
 ===================================================================================================
    Aim:    Returns the (Filtered) list of the objects whose BV collides the Pivot of the current object.
	In:		- First Element of an Array where we can store the list of objects.
			- Filter (AI, Groups ? ...)
 ===================================================================================================
 */

ULONG AI_EvalFunc_COLPivotBVListGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreate
			(
				_pst_GO,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				0,
				pst_World,
				(ULONG *) _ppst_Array,
				(UCHAR) 0xFC
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLPivotBVListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLPivotBVListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);

	return ++_pst_Node;
}

/*$F
 ===================================================================================================
    Aim:    Returns the (Filtered) list of the objects whose Pivot collides a particular ZDE of
            the current object.

	In:		- First Element of an Array where we can store the list of objects.
			- Filter (AI, Groups ? ...)
			- Index of the particular ZDE to Test.
 ===================================================================================================
 */

ULONG AI_EvalFunc_COLZDEPivotListGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_IndexA,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreate
			(
				_pst_GO,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				(UCHAR) _ul_IndexA,
				pst_World,
				(ULONG *) _ppst_Array,
				(UCHAR) 0xFB
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLZDEPivotListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_Index;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	ul_Index = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLZDEPivotListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_Index,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);

	return ++_pst_Node;
}

/*$F
 ===================================================================================================
    Aim:    Returns the (Filtered) list of the objects whose ZDE collides the Pivot of the current object.
	In:		- First Element of an Array where we can store the list of objects.
			- Filter (AI, Groups ? ...)
			- Index of the particular ZDE to Test.
 ===================================================================================================
 */

ULONG AI_EvalFunc_COLPivotZDEListGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_IndexB,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreate
			(
				_pst_GO,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				(UCHAR) _ul_IndexB,
				pst_World,
				(ULONG *) _ppst_Array,
				(UCHAR) 0xFA
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLPivotZDEListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_Index;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	ul_Index = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLPivotZDEListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_Index,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);

	return ++_pst_Node;
}

/*$F
 ===================================================================================================
    Aim:    Returns the (Filtered) list of the objects whose ZDE collides the BV Center of the current object.
	In:		- First Element of an Array where we can store the list of objects.
			- Filter (AI, Groups ? ...)
			- Index of the particular ZDE to Test.
 ===================================================================================================
 */

ULONG AI_EvalFunc_COLZDEBVCenterListGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_IndexA,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreate
			(
				_pst_GO,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				(UCHAR) _ul_IndexA,
				pst_World,
				(ULONG *) _ppst_Array,
				(UCHAR) 0xEF
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLZDEBVCenterListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_Index;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	ul_Index = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLZDEBVCenterListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_Index,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLBVCenterZDEListGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_IndexB,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return
		(
			COL_ListCreate
			(
				_pst_GO,
				_ul_OnFlags,
				_ul_OffFlags,
				_ul_FlagID,
				(UCHAR) _ul_IndexB,
				pst_World,
				(ULONG *) _ppst_Array,
				(UCHAR) 0xEE
			)
		);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBVCenterZDEListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_Index;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	ul_Index = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLBVCenterZDEListGet_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_Index,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Determines if one particular ZDE of an object collides the other one's particular ZDE.
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLZDEZDECollide_C
(
	OBJ_tdst_GameObject *_pst_A,
	OBJ_tdst_GameObject *_pst_B,
	ULONG				_ul_IndexA,
	ULONG				_ul_IndexB
)
{
	return(COL_ZDE_ZDECollide(_pst_A, (UCHAR) _ul_IndexA, (UCHAR) _ul_IndexB, _pst_B));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLZDEZDECollide(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Obj;
	ULONG				ul_A_Index, ul_B_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_B_Index = AI_PopInt();
	ul_A_Index = AI_PopInt();
	pst_Obj = AI_PopGameObject();
	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_Obj != NULL, "Invalid Object as parameter");

	AI_PushInt(AI_EvalFunc_COLZDEZDECollide_C(pst_GO, pst_Obj, ul_A_Index, ul_B_Index));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Determines if one particular ZDE of an object collides another object's BV.
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLZDEBVCollide_C(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B, ULONG _ul_IndexA)
{
	return(COL_ZDE_BVCollide(_pst_A, (UCHAR) _ul_IndexA, _pst_B, 0));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLZDEBVCollide(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Obj;
	ULONG				ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = AI_PopInt();
	pst_Obj = AI_PopGameObject();
	AI_Check(pst_Obj != NULL, "Invalid Object as parameter");
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLZDEBVCollide_C(pst_GO, pst_Obj, ul_Index));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Determines if one particular ZDE of an object collides another object's Pivot.
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLZDEPivotCollide_C(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B, ULONG _ul_IndexA)
{
	return(COL_ZDE_PivotCollide(_pst_A, (UCHAR) _ul_IndexA, _pst_B, 0));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLZDEPivotCollide(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Obj;
	ULONG				ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = AI_PopInt();
	pst_Obj = AI_PopGameObject();
	AI_Check(pst_Obj != NULL, "Invalid Object as parameter");
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLZDEPivotCollide_C(pst_GO, pst_Obj, ul_Index));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLBVPivotCollide_C(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	return(COL_BV_PivotCollide(_pst_A, _pst_B, 0));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBVPivotCollide(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Obj = AI_PopGameObject();
	AI_Check(pst_Obj != NULL, "Invalid Object as parameter");
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLBVPivotCollide_C(pst_GO, pst_Obj));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLPivotBVCollide_C(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	return(COL_BV_PivotCollide(_pst_A, _pst_B, 1));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLPivotBVCollide(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Obj = AI_PopGameObject();
	AI_Check(pst_Obj != NULL, "Invalid Object as parameter");
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLPivotBVCollide_C(pst_GO, pst_Obj));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Determines if one particular ZDE of an object collides another object's BV Center.
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLZDEBVCenterCollide_C(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B, ULONG _ul_IndexA)
{
	return(COL_ZDE_BVCenterCollide(_pst_A, (UCHAR) _ul_IndexA, _pst_B, 0));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLZDEBVCenterCollide(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Obj;
	ULONG				ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = AI_PopInt();
	pst_Obj = AI_PopGameObject();
	AI_Check(pst_Obj != NULL, "Invalid Object as parameter");
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLZDEBVCenterCollide_C(pst_GO, pst_Obj, ul_Index));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    [RAY CASTING] Returns the first object met aLONG the ray that correponds to the filter. The Engine function
            of Ray Casting also fills a structure in the world with other info about this Ray Cast.

    Note:   In this function, you give a unit vector and a distance that corresponds to the length of the ray.
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLRayObjectDist_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Orig,
	MATH_tdst_Vector	*_pst_Dir,
	float				_f_MaxDist,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID,
	ULONG				_ul_TypeRay
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	BOOL			b_UserCrossable;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	b_UserCrossable = _ul_TypeRay & 0x80000000;

	_ul_TypeRay &= ~0x80000000;

	switch(_ul_TypeRay)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
		return
			(
				COL_ColMap_RayCast
				(
					pst_World,
					_pst_GO,
					_pst_Orig,
					_pst_Dir,
					_f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					NULL,
					TRUE,
					((_ul_TypeRay == 2) || (_ul_TypeRay == 5) || (_ul_TypeRay == 22) || (_ul_TypeRay == 25)) ? FALSE : TRUE,
					((_ul_TypeRay == 3) || (_ul_TypeRay == 23)) ? FALSE : TRUE,
					((_ul_TypeRay == 4) || (_ul_TypeRay == 5) || (_ul_TypeRay == 24) || (_ul_TypeRay == 25)) ? FALSE : TRUE,
					b_UserCrossable,
					(_ul_TypeRay > 20) ? TRUE : FALSE
				)
			);

	case 0:
		return
			(
				COL_Visual_RayCast
				(
					pst_World,
					_pst_GO,
					_pst_Orig,
					_pst_Dir,
					_f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					NULL,
					TRUE,
					FALSE
				)
			);
		break;

	case 11:
		return
			(
				COL_Visual_RayCast
				(
					pst_World,
					_pst_GO,
					_pst_Orig,
					_pst_Dir,
					_f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					NULL,
					TRUE,
					TRUE
				)
			);
		break;

	}

	return NULL;
}
/**/
static int			i_Raster = 0;
AI_tdst_Node *AI_EvalFunc_COLRayObjectDist(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Orig, st_Dir;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID, ul_TypeRay;
	float				f_MaxDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_TypeRay = AI_PopInt();
	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	f_MaxDist = AI_PopFloat();
	AI_PopVector(&st_Dir);
	AI_PopVector(&st_Orig);
	AI_M_GetCurrentObject(pst_GO);

	if(!i_Raster)
	{
		PRO_FirstInitTrameRaster
		(
			&ENG_gpst_RasterEng_RayDist,
			"Trame Loop",
			"Game Engine",
			"Ray Object Dist",
			PRO_E_Time,
			0
		);
		i_Raster = 1;
	}

	PRO_StartTrameRaster(&ENG_gpst_RasterEng_RayDist);

	AI_PushGameObject
	(
		AI_EvalFunc_COLRayObjectDist_C
		(
			pst_GO,
			&st_Orig,
			&st_Dir,
			f_MaxDist,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID,
			ul_TypeRay
		)
	);

	PRO_StopTrameRaster(&ENG_gpst_RasterEng_RayDist);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    [RAY CASTING] Returns the first object met aLONG the ray that correponds to the filter. The Engine function
            of Ray Casting also fills a structure in the world with other info about this Ray Cast.

    Note:   In this function, you give a unit vector and a distance that corresponds to the length of the ray.
 =======================================================================================================================
 */
int AI_EvalFunc_COLRayIsCutDist_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Orig,
	MATH_tdst_Vector	*_pst_Dir,
	float				_f_MaxDist,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID,
	ULONG				_ul_TypeRay
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	BOOL			b_UserCrossable;
	BOOL			b_IsNotCut;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	b_IsNotCut = TRUE;

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	b_UserCrossable = _ul_TypeRay & 0x80000000;

	_ul_TypeRay &= ~0x80000000;

	switch(_ul_TypeRay)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
				COL_ColMap_RayCast
				(
					pst_World,
					_pst_GO,
					_pst_Orig,
					_pst_Dir,
					_f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					&b_IsNotCut,
					TRUE,
					((_ul_TypeRay == 2) || (_ul_TypeRay == 5) || (_ul_TypeRay == 22) || (_ul_TypeRay == 25)) ? FALSE : TRUE,
					((_ul_TypeRay == 3) || (_ul_TypeRay == 23)) ? FALSE : TRUE,
					((_ul_TypeRay == 4) || (_ul_TypeRay == 5) || (_ul_TypeRay == 24) || (_ul_TypeRay == 25)) ? FALSE : TRUE,
					b_UserCrossable,
					(_ul_TypeRay > 20) ? TRUE : FALSE
				);

			return !b_IsNotCut;


	case 0:
				COL_Visual_RayCast
				(
					pst_World,
					_pst_GO,
					_pst_Orig,
					_pst_Dir,
					_f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					&b_IsNotCut,
					TRUE,
					FALSE
				);

			return !b_IsNotCut;

	case 11:
				COL_Visual_RayCast
				(
					pst_World,
					_pst_GO,
					_pst_Orig,
					_pst_Dir,
					_f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					&b_IsNotCut,
					TRUE,
					TRUE
				);

		return !b_IsNotCut;

	}

	return FALSE;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRayIsCutDist(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Orig, st_Dir;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID, ul_TypeRay;
	float				f_MaxDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_TypeRay = AI_PopInt();
	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	f_MaxDist = AI_PopFloat();
	AI_PopVector(&st_Dir);
	AI_PopVector(&st_Orig);
	AI_M_GetCurrentObject(pst_GO);

	if(!i_Raster)
	{
		PRO_FirstInitTrameRaster
		(
			&ENG_gpst_RasterEng_RayDist,
			"Trame Loop",
			"Game Engine",
			"Ray Object Dist",
			PRO_E_Time,
			0
		);
		i_Raster = 1;
	}

	PRO_StartTrameRaster(&ENG_gpst_RasterEng_RayDist);

	AI_PushInt
	(
		AI_EvalFunc_COLRayIsCutDist_C
		(
			pst_GO,
			&st_Orig,
			&st_Dir,
			f_MaxDist,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID,
			ul_TypeRay
		)
	);

	PRO_StopTrameRaster(&ENG_gpst_RasterEng_RayDist);
	return ++_pst_Node;
}



OBJ_tdst_GameObject *AI_EvalFunc_COLRaySingleObjectDist_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Orig,
	MATH_tdst_Vector	*_pst_Dir,
	float				_f_MaxDist,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID,
	ULONG				_ul_TypeRay
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL			b_UserCrossable;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	b_UserCrossable = _ul_TypeRay & 0x80000000;

	_ul_TypeRay &= ~0x80000000;

	switch(_ul_TypeRay)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
		return
			(
				COL_ColMap_RayCast_OneObject
				(
					_pst_GO,
					_pst_Orig,
					_pst_Dir,
					_f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					NULL,
					TRUE,
					((_ul_TypeRay == 2) || (_ul_TypeRay == 5) || (_ul_TypeRay == 22) || (_ul_TypeRay == 25)) ? FALSE : TRUE,
					((_ul_TypeRay == 3) || (_ul_TypeRay == 23)) ? FALSE : TRUE,
					((_ul_TypeRay == 4) || (_ul_TypeRay == 5) || (_ul_TypeRay == 24) || (_ul_TypeRay == 25)) ? FALSE : TRUE,
					b_UserCrossable,
					(_ul_TypeRay > 20) ? TRUE : FALSE
				)
			);

		return NULL;

	case 10:
		return
			(
				COL_Visual_RayCast_OneObject
				(
					_pst_GO,
					_pst_Orig,
					_pst_Dir,
					_f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					NULL,
					TRUE,
					TRUE,
					TRUE,
					TRUE
				)
			);

	case 0:
		return
			(
				COL_Visual_RayCast_OneObject
				(
					_pst_GO,
					_pst_Orig,
					_pst_Dir,
					_f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					NULL,
					TRUE,
					FALSE,
					FALSE,
					FALSE
				)
			);

	case 11:
		return
			(
				COL_Visual_RayCast_OneObject
				(
					_pst_GO,
					_pst_Orig,
					_pst_Dir,
					_f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					NULL,
					TRUE,
					FALSE,
					FALSE,
					TRUE
				)
			);

	}

	return NULL;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRaySingleObjectDist(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Orig, st_Dir;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID, ul_TypeRay;
	float				f_MaxDist;
//	static int			i_Raster = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_TypeRay = AI_PopInt();
	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	f_MaxDist = AI_PopFloat();
	AI_PopVector(&st_Dir);
	AI_PopVector(&st_Orig);
	pst_GO = AI_PopGameObject();

	if(!i_Raster)
	{
		PRO_FirstInitTrameRaster
		(
			&ENG_gpst_RasterEng_RayDist,
			"Trame Loop",
			"Game Engine",
			"Ray Object Dist",
			PRO_E_Time,
			0
		);
		i_Raster = 1;
	}

	PRO_StartTrameRaster(&ENG_gpst_RasterEng_RayDist);

	AI_PushGameObject
	(
		AI_EvalFunc_COLRaySingleObjectDist_C
		(
			pst_GO,
			&st_Orig,
			&st_Dir,
			f_MaxDist,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID,
			ul_TypeRay
		)
	);

	PRO_StopTrameRaster(&ENG_gpst_RasterEng_RayDist);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
    Aim:    [RAY CASTING] Returns the first object met aLONG the ray that correponds to the filter. The Engine function
            of Ray Casting also fills a structure in the world with other info about this Ray Cast.

    Note:   In this function, you give a vector that "is" tha ray. this vector can be not unit.
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLRayObjectVector_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Orig,
	MATH_tdst_Vector	*_pst_Vector,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID,
	ULONG				_ul_TypeRay
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector	st_Dir;
	BOOL				b_UserCrossable;
	float				f_MaxDist, f_InvDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	/* Compute the Norm of the vector. */
	f_MaxDist = MATH_f_NormVector(_pst_Vector);

	/* Compute the unit vector of the ray. */
	if(f_MaxDist != 0.0f)
	{
		f_InvDist = fInv(f_MaxDist);
		st_Dir.x = fMul(_pst_Vector->x, f_InvDist);
		st_Dir.y = fMul(_pst_Vector->y, f_InvDist);
		st_Dir.z = fMul(_pst_Vector->z, f_InvDist);
	}
	else
	{
		MATH_CopyVector(&st_Dir, _pst_Vector);
	}

	b_UserCrossable = _ul_TypeRay & 0x80000000;

	_ul_TypeRay &= ~0x80000000;

	switch(_ul_TypeRay)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
		return
			(
				COL_ColMap_RayCast
				(
					pst_World,
					_pst_GO,
					_pst_Orig,
					&st_Dir,
					f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					NULL,
					TRUE,
					((_ul_TypeRay == 2) || (_ul_TypeRay == 5) || (_ul_TypeRay == 22) || (_ul_TypeRay == 25)) ? FALSE : TRUE,
					((_ul_TypeRay == 3) || (_ul_TypeRay == 23)) ? FALSE : TRUE,
					((_ul_TypeRay == 4) || (_ul_TypeRay == 5) || (_ul_TypeRay == 24) || (_ul_TypeRay == 25)) ? FALSE : TRUE,
					b_UserCrossable,
					(_ul_TypeRay > 20) ? TRUE : FALSE
				)
			);


	case 0:
		return
			(
				COL_Visual_RayCast
				(
					pst_World,
					_pst_GO,
					_pst_Orig,
					&st_Dir,
					f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					NULL,
					TRUE,
					FALSE
				)
			);
		break;

	case 11:
		return
			(
				COL_Visual_RayCast
				(
					pst_World,
					_pst_GO,
					_pst_Orig,
					&st_Dir,
					f_MaxDist,
					_ul_OnFlags,
					_ul_OffFlags,
					_ul_FlagID,
					NULL,
					TRUE,
					TRUE
				)
			);
		break;

	}

	return NULL;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRayObjectVector(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Orig, st_Vector;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID, ul_TypeRay;
	static int			i_Raster = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_TypeRay = AI_PopInt();
	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	AI_PopVector(&st_Vector);
	AI_PopVector(&st_Orig);
	AI_M_GetCurrentObject(pst_GO);

	if(!i_Raster)
	{
		PRO_FirstInitTrameRaster
		(
			&ENG_gpst_RasterEng_RayCast,
			"Trame Loop",
			"Game Engine",
			"Ray Object Vector",
			PRO_E_Time,
			0
		);
		i_Raster = 1;
	}

	PRO_StartTrameRaster(&ENG_gpst_RasterEng_RayCast);

	AI_PushGameObject
	(
		AI_EvalFunc_COLRayObjectVector_C
		(
			pst_GO,
			&st_Orig,
			&st_Vector,
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID,
			ul_TypeRay
		)
	);
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_RayCast);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    After a ray Casting, returns the exact position of the colliding point.
 =======================================================================================================================
 */
void AI_EvalFunc_COLRayPosGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	MATH_CopyVector(_pst_Pos, &pst_World->st_RayInfo.st_CollidedPoint);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRayPosGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLRayPosGet_C(pst_GO, &st_Pos);

	AI_PushVector(&st_Pos);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    After a ray Casting, returns the Sound touched (Visual/ColMap)
 =======================================================================================================================
 */
int AI_EvalFunc_COLRaySoundGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return pst_World->st_RayInfo.ul_Sound;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRaySoundGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLRaySoundGet_C(pst_GO));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
    Aim:    After a ray Casting, returns the normal of the face that has been touched
 =======================================================================================================================
 */
void AI_EvalFunc_COLRayNormalGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Normal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	MATH_CopyVector(_pst_Normal, &pst_World->st_RayInfo.st_Normal);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRayNormalGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Normal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLRayNormalGet_C(pst_GO, &st_Normal);

	AI_PushVector(&st_Normal);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLRayEdgeNormalGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Normal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	MATH_CopyVector(_pst_Normal, &pst_World->st_RayInfo.st_EdgeNormal);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRayEdgeNormalGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Normal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLRayEdgeNormalGet_C(pst_GO, &st_Normal);

	AI_PushVector(&st_Normal);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLRayFlagsGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return (int) (pst_World->st_RayInfo.uc_Flags);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRayFlagsGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_COLRayFlagsGet_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Get the instance activation Flag of the object.
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLSetActivationGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
		return((ULONG) (((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Activation));
	else
		return(0);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLSetActivationGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_COLSetActivationGet_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Set Max LOD of an object
 =======================================================================================================================
 */
void AI_EvalFunc_COLLODSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_MinLOD, ULONG _ul_MaxLOD)
{
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->uc_MaxLOD = (UCHAR) _ul_MaxLOD;
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->uc_MinLOD = (UCHAR) _ul_MinLOD;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_COLLODSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_MaxLOD, ul_MinLOD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_MaxLOD = AI_PopInt();
	ul_MinLOD = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_COLLODSet_C(pst_GO, ul_MinLOD, ul_MaxLOD);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Set Max LOD of an object
 =======================================================================================================================
 */
void AI_EvalFunc_COLWallPush_C(OBJ_tdst_GameObject *_pst_GO, ULONG ul_Val)
{
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->b_InTheWall = ul_Val ? TRUE : FALSE;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_COLWallPush(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Val = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_COLWallPush_C(pst_GO, ul_Val);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLCrossableSet_C(OBJ_tdst_GameObject *pst_GO, USHORT uw_CrossOn, USHORT uw_CrossOff)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!pst_GO
	||	!pst_GO->pst_Extended
	||	!pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance
	) return;

	pst_Instance = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_Instance->uw_Crossable = (pst_Instance->uw_Crossable & (~uw_CrossOff)) | uw_CrossOn;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLCrossableSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	USHORT				uw_CrossOn, uw_CrossOff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_CrossOff = AI_PopInt();
	uw_CrossOn = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_COLCrossableSet_C(pst_GO, uw_CrossOn, uw_CrossOff);
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLCrossableGet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!pst_GO
	||	!pst_GO->pst_Extended
	||	!pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance
	) return -1;

	pst_Instance = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance;
	return ((int) pst_Instance->uw_Crossable);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLCrossableGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_COLCrossableGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the instance activation flag of the object.
 =======================================================================================================================
 */
void AI_EvalFunc_COLSetActivationSet_C(OBJ_tdst_GameObject *pst_GO, USHORT uw_On, USHORT uw_Off)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!pst_GO
	||	!pst_GO->pst_Extended
	||	!pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance
	) return;

	pst_Instance = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_Instance->uw_Activation = (pst_Instance->uw_Activation & (~uw_Off)) | uw_On;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLSetActivationSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	USHORT				uw_On, uw_Off;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uw_Off = AI_PopInt();
	uw_On = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_COLSetActivationSet_C(pst_GO, uw_On, uw_Off);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLStartMatrixSet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_NewOldPos)
{
	if
	(
		!pst_GO
	||	!pst_GO->pst_Extended
	||	!pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance
	) return;

	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	st_InverseMatrix;
		MATH_tdst_Vector	st_Temp;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_InvertMatrix(&st_InverseMatrix, pst_GO->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix);
		MATH_TransformVertex(&st_Temp, &st_InverseMatrix, pst_NewOldPos);

		MATH_CopyVector
		(
			&(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix->T),
			&st_Temp
		);
	}
	else
	{
		MATH_CopyVector
		(
			&(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix->T),
			pst_NewOldPos
		);
	}

	COL_ul_SnP_RefreshType = COL_SnP_RefreshXYZ;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLStartMatrixSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_NewOldPos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_NewOldPos);
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_COLStartMatrixSet_C(pst_GO, &st_NewOldPos);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLMapActivationGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
		return(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->uc_Activation);
	else
		return(0);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLMapActivationGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_COLMapActivationGet_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the ColMap activation flag of the object.
 =======================================================================================================================
 */
void AI_EvalFunc_COLMapActivationSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_On, ULONG _ul_Off)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap *pst_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!_pst_GO
	||	!_pst_GO->pst_Extended
	||	!_pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap
	) return;

	pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
		pst_ColMap->uc_Activation = (pst_ColMap->uc_Activation & (~((UCHAR) _ul_Off))) | ((UCHAR) _ul_On);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLMapActivationSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_On, ul_Off;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Off = AI_PopInt();
	ul_On = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLMapActivationSet_C(pst_GO, ul_On, ul_Off);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Changes the Size of a Specific ZDx.
 =======================================================================================================================
 */
void AI_EvalFunc_COLSpecificSizeSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index, MATH_tdst_Vector *_pst_Size)
{
	COL_ZDx_SpecificSizeSet(_pst_GO, (UCHAR) _ul_AI_Index, _pst_Size);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLSpecificSizeSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	MATH_tdst_Vector	st_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_Size);
	ul_AI_Index = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_COLSpecificSizeSet_C(pst_GO, ul_AI_Index, &st_Size);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLSpecificPosSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index, MATH_tdst_Vector *_pst_Pos)
{
	COL_ZDx_SpecificPosSet(_pst_GO, (UCHAR) _ul_AI_Index, _pst_Pos);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLSpecificPosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_Pos);
	ul_AI_Index = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_COLSpecificPosSet_C(pst_GO, ul_AI_Index, &st_Pos);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLGeoColMapPosSet_C(OBJ_tdst_GameObject *_pst_GO, UCHAR _uc_Index, MATH_tdst_Vector *_pst_Pos)
{
	COL_GeoColMap_PosSet(_pst_GO, (UCHAR) _uc_Index, _pst_Pos);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGeoColMapPosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	UCHAR				uc_Index;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_Pos);
	uc_Index = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_COLGeoColMapPosSet_C(pst_GO, uc_Index, &st_Pos);

	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLFlagGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ZDx		*pst_ZDx;
	UCHAR				uc_ENG_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) return 0;

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;

	/* We get the Real ENG index of the ZDx; */
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[(UCHAR) _ul_AI_Index];

	pst_ZDx = *(pst_Instance->dpst_ZDx + uc_ENG_Index);

	return pst_ZDx->uc_Flag;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLFlagGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_AI_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

    AI_PushInt(AI_EvalFunc_COLFlagGet_C(pst_GO, ul_AI_Index));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLFlagSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index, ULONG _ul_On, ULONG _ul_Off)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ZDx		*pst_ZDx;
	UCHAR				uc_ENG_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) return;

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;

	/* We get the Real ENG index of the ZDx; */
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[(UCHAR) _ul_AI_Index];

	pst_ZDx = *(pst_Instance->dpst_ZDx + uc_ENG_Index);

	pst_ZDx->uc_Flag |= (UCHAR)_ul_On;
	pst_ZDx->uc_Flag &= ~((UCHAR)_ul_Off);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLFlagSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	ULONG				ul_On;
	ULONG				ul_Off;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Off = AI_PopInt();
	ul_On = AI_PopInt();
	ul_AI_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLFlagSet_C(pst_GO, ul_AI_Index, ul_On, ul_Off);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLFlagXSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ZDx		*pst_ZDx;
	UCHAR				uc_ENG_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) return;

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;

	/* We get the Real ENG index of the ZDx; */
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[(UCHAR) _ul_AI_Index];

	pst_ZDx = *(pst_Instance->dpst_ZDx + uc_ENG_Index);

	pst_ZDx->uc_Flag |= COL_C_Zone_FlagX;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLFlagXSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_AI_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLFlagXSet_C(pst_GO, ul_AI_Index);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLFlagXReset_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ZDx		*pst_ZDx;
	UCHAR				uc_ENG_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) return;

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;

	/* We get the Real ENG index of the ZDx; */
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[(UCHAR) _ul_AI_Index];

	pst_ZDx = *(pst_Instance->dpst_ZDx + uc_ENG_Index);

	pst_ZDx->uc_Flag &= ~COL_C_Zone_FlagX;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLFlagXReset(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_AI_Index = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLFlagXReset_C(pst_GO, ul_AI_Index);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_COLSizeGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index)
{
	return(COL_ZDx_SizeGet(_pst_GO, (UCHAR) _ul_AI_Index));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLSizeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_AI_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushFloat(AI_EvalFunc_COLSizeGet_C(pst_GO, ul_AI_Index));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLPosGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index, MATH_tdst_Vector *_pst_Pos)
{
	MATH_InitVector(_pst_Pos, 0.0f, 0.0f, 0.0f);
	COL_ZDx_PosGet(_pst_GO, (UCHAR) _ul_AI_Index, _pst_Pos);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLPosGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_AI_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLPosGet_C(pst_GO, ul_AI_Index, &st_Pos);

	AI_PushVector(&st_Pos);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Return the type of a ZDx (Sphere or Box)
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLTypeGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	UCHAR				uc_ENG_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) || (_ul_AI_Index > 15))
	{
		return 0;
	}

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[(UCHAR) _ul_AI_Index];
	if(uc_ENG_Index > 15)
	{
		return 0;
	}

	return((ULONG) ((*(pst_Instance->dpst_ZDx + uc_ENG_Index))->uc_Type));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLTypeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_AI_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLTypeGet_C(pst_GO, ul_AI_Index));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLMapTypeGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap		*pst_ColMap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
		return 0;

	pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;

	if(!pst_ColMap || (_ul_Index >= (ULONG) pst_ColMap->uc_NbOfCob))
		return 0;

	return((ULONG) pst_ColMap->dpst_Cob[_ul_Index]->uc_Type);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLMapTypeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLMapTypeGet_C(pst_GO, ul_Index));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:
 =======================================================================================================================
 */
void AI_EvalFunc_COLMapMinGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Index, MATH_tdst_Vector *_pst_Min)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap		*pst_ColMap;
	COL_tdst_Cob		*pst_Cob;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
	{
		MATH_CopyVector(_pst_Min, &MATH_gst_NulVector);
		return;
	}

	pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;

	if(!pst_ColMap || (_ul_Index >= pst_ColMap->uc_NbOfCob))
	{
		MATH_CopyVector(_pst_Min, &MATH_gst_NulVector);
		return;
	}


	pst_Cob = pst_ColMap->dpst_Cob[_ul_Index];
	switch(pst_Cob->uc_Type)
	{
	case COL_C_Zone_Triangles:
		MATH_CopyVector(_pst_Min, &MATH_gst_NulVector);
		break;

	case COL_C_Zone_Box:
		MATH_CopyVector(_pst_Min, &((COL_tdst_Box *)pst_Cob->pst_MathCob->p_Shape)->st_Min);
		break;

	case COL_C_Zone_Sphere:
		MATH_CopyVector(_pst_Min, &((COL_tdst_Sphere *)pst_Cob->pst_MathCob->p_Shape)->st_Center);
		break;

	case COL_C_Zone_Cylinder:
		MATH_CopyVector(_pst_Min, &((COL_tdst_Cylinder *)pst_Cob->pst_MathCob->p_Shape)->st_Center);
		break;


	}
}
/**/
AI_tdst_Node *AI_EvalFunc_COLMapMinGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Index;
	MATH_tdst_Vector	st_Min;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLMapMinGet_C(pst_GO, ul_Index, &st_Min);

	AI_PushVector(&st_Min);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:
 =======================================================================================================================
 */
void AI_EvalFunc_COLMapMaxGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Index, MATH_tdst_Vector *_pst_Max)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap		*pst_ColMap;
	COL_tdst_Cob		*pst_Cob;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
	{
		MATH_CopyVector(_pst_Max, &MATH_gst_NulVector);
		return;
	}

	pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;

	if(!pst_ColMap || (_ul_Index >= pst_ColMap->uc_NbOfCob))
	{
		MATH_CopyVector(_pst_Max, &MATH_gst_NulVector);
		return;
	}

	pst_Cob = pst_ColMap->dpst_Cob[_ul_Index];

	switch(pst_Cob->uc_Type)
	{
	case COL_C_Zone_Triangles:
		MATH_CopyVector(_pst_Max, &MATH_gst_NulVector);
		break;

	case COL_C_Zone_Box:
		MATH_CopyVector(_pst_Max, &((COL_tdst_Box *)pst_Cob->pst_MathCob->p_Shape)->st_Max);
		break;

	case COL_C_Zone_Sphere:
		_pst_Max->x = ((COL_tdst_Sphere *)pst_Cob->pst_MathCob->p_Shape)->f_Radius;
		break;

	case COL_C_Zone_Cylinder:
		_pst_Max->x = ((COL_tdst_Cylinder *)pst_Cob->pst_MathCob->p_Shape)->f_Radius;

		_pst_Max->y = ((COL_tdst_Cylinder *)pst_Cob->pst_MathCob->p_Shape)->f_Height;
		break;
	}

}
/**/
AI_tdst_Node *AI_EvalFunc_COLMapMaxGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Index;
	MATH_tdst_Vector	st_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLMapMaxGet_C(pst_GO, ul_Index, &st_Max);

	AI_PushVector(&st_Max);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
    Aim:    Return the Min point of a Box ZDx
 =======================================================================================================================
 */
void AI_EvalFunc_COLBoxMinGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index, MATH_tdst_Vector *_pst_Min)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	UCHAR				uc_ENG_Index;
	COL_tdst_ZDx		*pst_ZDx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) || (_ul_AI_Index > 15))
	{
		MATH_CopyVector(_pst_Min, &MATH_gst_NulVector);
	}

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[(UCHAR) _ul_AI_Index];

	if(uc_ENG_Index > 15)
	{
		MATH_CopyVector(_pst_Min, &MATH_gst_NulVector);
	}

	pst_ZDx = *(pst_Instance->dpst_ZDx + uc_ENG_Index);
	if(!COL_b_Zone_TestType(pst_ZDx, COL_C_Zone_Box))
		MATH_CopyVector(_pst_Min, &MATH_gst_NulVector);
	else
		MATH_CopyVector(_pst_Min, COL_pst_Shape_GetMin(pst_ZDx->p_Shape));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBoxMinGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	MATH_tdst_Vector	st_Min;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_AI_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLBoxMinGet_C(pst_GO, ul_AI_Index, &st_Min);

	AI_PushVector(&st_Min);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Return the Max point of a Box ZDx
 =======================================================================================================================
 */
void AI_EvalFunc_COLBoxMaxGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index, MATH_tdst_Vector *_pst_Max)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	UCHAR				uc_ENG_Index;
	COL_tdst_ZDx		*pst_ZDx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) || (_ul_AI_Index > 15))
	{
		MATH_CopyVector(_pst_Max, &MATH_gst_NulVector);
	}

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[(UCHAR) _ul_AI_Index];

	if(uc_ENG_Index > 15)
	{
		MATH_CopyVector(_pst_Max, &MATH_gst_NulVector);
	}

	pst_ZDx = *(pst_Instance->dpst_ZDx + uc_ENG_Index);
	if(!COL_b_Zone_TestType(pst_ZDx, COL_C_Zone_Box))
		MATH_CopyVector(_pst_Max, &MATH_gst_NulVector);
	else
		MATH_CopyVector(_pst_Max, COL_pst_Shape_GetMax(pst_ZDx->p_Shape));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBoxMaxGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	MATH_tdst_Vector	st_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_AI_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLBoxMaxGet_C(pst_GO, ul_AI_Index, &st_Max);

	AI_PushVector(&st_Max);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Gets the ZDx Design Flag.
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLSetDesignGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_AI_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	UCHAR				uc_ENG_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) || (_ul_AI_Index > 15))
	{
		return 0;
	}

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;

	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[(UCHAR) _ul_AI_Index];

	return((*(pst_Instance->dpst_ZDx + uc_ENG_Index))->uc_Design);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLSetDesignGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_AI_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_AI_Index = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLSetDesignGet_C(pst_GO, ul_AI_Index));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Ex:     COL_GlobalWallCosAngleSet(0.5) // set the global wall angle of the engine to 60 °
 =======================================================================================================================
 */
void AI_EvalFunc_COLGlobalWallCosAngleSet_C(float f)
{
	COL_gst_GlobalVars.f_WallCosAngle = f;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGlobalWallCosAngleSet(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_COLGlobalWallCosAngleSet_C(AI_PopFloat());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLGlobalCornerCosAngleSet_C(float f)
{
	COL_gst_GlobalVars.f_CornerCosAngle = f;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGlobalCornerCosAngleSet(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_COLGlobalCornerCosAngleSet_C(AI_PopFloat());
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_COLGlobalWallCosAngleGet_C(void)
{
	return COL_gst_GlobalVars.f_WallCosAngle;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGlobalWallCosAngleGet(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_COLGlobalWallCosAngleGet_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_COLGlobalCornerCosAngleGet_C(void)
{
	return COL_gst_GlobalVars.f_CornerCosAngle;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGlobalCornerCosAngleGet(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_COLGlobalCornerCosAngleGet_C());
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLComputeJumpPoint_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Follow,
	float				_f_Alpha,
	int					_i_Down,
	int					_i_Ray,
	MATH_tdst_Vector	*_pst_Jump
)
{
	COL_ComputeJumpPoint(_pst_GO, _pst_Jump, _pst_Follow, _f_Alpha, _i_Down, _i_Ray, TRUE, FALSE, FALSE);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLComputeJumpPoint(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Jump;
	MATH_tdst_Vector	st_Dir;
	float				f_Alpha;
	BOOL				b_Down, b_Ray;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Ray = AI_PopInt();
	b_Down = AI_PopInt();
	f_Alpha = AI_PopFloat();

	AI_PopVector(&st_Dir);

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLComputeJumpPoint_C(pst_GO, &st_Dir, f_Alpha, (int) b_Down, (int) b_Ray, &st_Jump);

	AI_PushVector(&st_Jump);
	return ++_pst_Node;
}

extern void COL_TipTopPointGet (OBJ_tdst_GameObject *,MATH_tdst_Vector*,MATH_tdst_Vector*,float,BOOL,BOOL,BOOL,BOOL);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLComputeFullJumpPoint_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Dir,
	float				_f_Alpha,
	int					_i_Down,
	int					_i_Ray,
	int					_i_OnyElement,
	int					_i_StopOnXXX,
	MATH_tdst_Vector	*_pst_Jump
)
{
	if(0)
	{
	if((_pst_Dir->x == 0.0f) && (_pst_Dir->y == 0.0f))
		COL_ComputeJumpPoint(_pst_GO, _pst_Jump, _pst_Dir, _f_Alpha, _i_Down, _i_Ray, _i_OnyElement, TRUE, FALSE);
	else
		COL_ComputeJumpPoint(_pst_GO, _pst_Jump, _pst_Dir, _f_Alpha, _i_Down, _i_Ray, _i_OnyElement, FALSE, TRUE);
	}
	else
	{
	if((_pst_Dir->x == 0.0f) && (_pst_Dir->y == 0.0f))
		COL_TipTopPointGet(_pst_GO, _pst_Jump, _pst_Dir, _f_Alpha, _i_Down, _i_Ray, TRUE, FALSE);
	else
		COL_TipTopPointGet(_pst_GO, _pst_Jump, _pst_Dir, _f_Alpha, _i_Down, _i_Ray, FALSE, TRUE);
	}

}
/**/
AI_tdst_Node *AI_EvalFunc_COLComputeFullJumpPoint(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Jump;
	MATH_tdst_Vector	st_Dir;
	float				f_Alpha;
	BOOL				b_Down, b_Ray;
	BOOL				b_OnlyElement;
	BOOL				b_StopOnGround;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_StopOnGround = AI_PopInt();
	b_OnlyElement = AI_PopInt();
	b_Ray = AI_PopInt();
	b_Down = AI_PopInt();
	f_Alpha = AI_PopFloat();

	AI_PopVector(&st_Dir);

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLComputeFullJumpPoint_C(pst_GO, &st_Dir, f_Alpha, (int) b_Down, (int) b_Ray, (int) b_OnlyElement, (int) b_StopOnGround, &st_Jump);

	AI_PushVector(&st_Jump);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLZDMObjectListGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	MATH_tdst_Vector	*_pst_Dir,
	ULONG				_ul_Type
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_Report_GetZDMGameObjectList(_pst_GO, pst_World, (ULONG *) _ppst_Array, _pst_Dir, _ul_Type));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLZDMObjectListGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Dir;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Type = AI_PopInt();
	AI_PopVector(&st_Dir);
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLZDMObjectListGet_C(pst_GO, ((OBJ_tdst_GameObject **) st_Var.pv_Addr), &st_Dir, ul_Type));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLGetAllReportsNormal_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Normal, ULONG _ul_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_Report_GetAllReportsNormal(_pst_GO, pst_World, (ULONG *) _pst_Normal, _ul_Type));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGetAllReportsNormal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Type = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLGetAllReportsNormal_C(pst_GO, ((MATH_tdst_Vector *) st_Var.pv_Addr), (int) ul_Type));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLGetReportsNumber_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_Report_GetNumberOfReports(_pst_GO, pst_World, _ul_Type));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGetReportsNumber(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Type = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLGetReportsNumber_C(pst_GO, ul_Type));

	return ++_pst_Node;
}

int AI_EvalFunc_COLGetReportsNumberAndRanks_C(OBJ_tdst_GameObject *_pst_GO, int *_pul_Array, ULONG _ul_Max, ULONG _ul_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return((int) COL_Report_GetNumberOfReportsAndRanks(_pst_GO, pst_World, _ul_Type, _ul_Max, (ULONG*)_pul_Array));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGetReportsNumberAndRanks(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Type, ul_Max;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Type = AI_PopInt();
	ul_Max = AI_PopInt();
	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLGetReportsNumberAndRanks_C(pst_GO, (int *) (st_Var.pv_Addr), ul_Max, ul_Type));

	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLGetResultingNormal_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Type, MATH_tdst_Vector *_pst_Normal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	COL_Report_ComputeResultingNormal(_pst_GO, pst_World, _ul_Type, _pst_Normal);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGetResultingNormal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Type;
	MATH_tdst_Vector	st_Normal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Type = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLGetResultingNormal_C(pst_GO, ul_Type, &st_Normal);

	AI_PushVector(&st_Normal);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLGetAllVisibleObjects_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject **_ppst_Array,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_VisibleObjectListCreate(pst_World, _ul_OnFlags, _ul_OffFlags, _ul_FlagID, (ULONG *) _ppst_Array));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGetAllVisibleObjects(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_FlagID, ul_OffFlags, ul_OnFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();

	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLGetAllVisibleObjects_C
		(
			pst_GO,
			((OBJ_tdst_GameObject **) st_Var.pv_Addr),
			ul_OnFlags,
			ul_OffFlags,
			ul_FlagID
		)
	);

	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MATH_VecRotate(MATH_tdst_Vector *_pst_Dest, MATH_tdst_Vector *_pst_Axe, MATH_tdst_Vector *_pst_Vec, float f_Angle)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Vec, st_Vec2, W;
	float				f, f_Norm2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector(&st_Vec, _pst_Vec);
	f = MATH_f_DotProduct(_pst_Axe, &st_Vec);
	f_Norm2 = MATH_f_DotProduct(_pst_Axe, _pst_Axe);

	f /= f_Norm2;
	MATH_ScaleVector(&W, _pst_Axe, f);
	MATH_SubEqualVector(&st_Vec, &W);
	MATH_ScaleVector(_pst_Dest, &st_Vec, fCos(f_Angle));
	MATH_NormalizeEqualVector(_pst_Axe);
	MATH_CrossProduct(&st_Vec2, _pst_Axe, &st_Vec);
	MATH_AddScaleVector(_pst_Dest, _pst_Dest, &st_Vec2, fSin(f_Angle));
	MATH_AddEqualVector(_pst_Dest, &W);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_RayCompute
(
	MATH_tdst_Vector	*_pst_Dest,
	WOR_tdst_World		*_pst_World,
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	ULONG				_ul_Filter,
	ULONG				_ul_NoFilter,
	ULONG				_ul_ID,
	BOOL				_b_UseRaySkipFlag,
	UINT				*_pui_Mode
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	ast_ListVec[50];
	int					ai_CanSee[50];
	int					i_NumVec;
	MATH_tdst_Vector	st_Axis, st_AxisX;
	MATH_tdst_Vector	st_Tmp;
	int					i;
	float				f_MaxDist, f_Dist, f_InvDist;
	OBJ_tdst_GameObject *pst_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_MaxDist = MATH_f_NormVector(_pst_Direction);
	f_InvDist = fInv(f_MaxDist);
	_pst_Direction->x = fMul(_pst_Direction->x, f_InvDist);
	_pst_Direction->y = fMul(_pst_Direction->y, f_InvDist);
	_pst_Direction->z = fMul(_pst_Direction->z, f_InvDist);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Fill array of all tested directions
	 -------------------------------------------------------------------------------------------------------------------
	 */

	st_Axis.x = st_Axis.y = 0;
	st_Axis.z = 1.0f;
	st_AxisX.y = st_AxisX.z = 0;
	st_AxisX.x = 1.0f;
	i_NumVec = 0;

	/* Rotate actual dir */
	MATH_CopyVector(&ast_ListVec[i_NumVec], _pst_Direction);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, _pst_Direction, Cf_PiBy4);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, _pst_Direction, -Cf_PiBy4);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, _pst_Direction, Cf_PiBy2);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, _pst_Direction, -Cf_PiBy2);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, _pst_Direction, Cf_PiBy2 + Cf_PiBy4);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, _pst_Direction, -(Cf_PiBy2 + Cf_PiBy4));
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, _pst_Direction, Cf_Pi);

	/* Same dir on horizontal plan */
	MATH_CopyVector(&ast_ListVec[++i_NumVec], _pst_Direction);
	ast_ListVec[i_NumVec].z = 0;
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, &ast_ListVec[i_NumVec - 1], Cf_PiBy4);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, &ast_ListVec[i_NumVec - 2], -Cf_PiBy4);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, &ast_ListVec[i_NumVec - 3], Cf_PiBy2);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, &ast_ListVec[i_NumVec - 4], -Cf_PiBy2);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, &ast_ListVec[i_NumVec - 5], Cf_PiBy2 + Cf_PiBy4);
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, &ast_ListVec[i_NumVec - 6], -(Cf_PiBy2 + Cf_PiBy4));
	MATH_VecRotate(&ast_ListVec[++i_NumVec], &st_Axis, &ast_ListVec[i_NumVec - 7], Cf_Pi);

	/* Z up */
	MATH_CopyVector(&ast_ListVec[++i_NumVec], _pst_Direction);
	ast_ListVec[i_NumVec].z = 0;
	MATH_VecRotate(&ast_ListVec[i_NumVec], &ast_ListVec[0], &st_AxisX, Cf_PiBy2 - 0.1f);

	_pst_Origin->x = MATH_f_FloatRound(_pst_Origin->x, 0.1f);
	_pst_Origin->y = MATH_f_FloatRound(_pst_Origin->y, 0.1f);
	_pst_Origin->z = MATH_f_FloatRound(_pst_Origin->z, 0.1f);

	/* Test all vects */
	for(i = 0; i <= i_NumVec; i++)
	{
		/* Can I see */
		MATH_NormalizeVector(&ast_ListVec[i], &ast_ListVec[i]);
		MATH_CopyVector(_pst_Dest, &ast_ListVec[i]);
		_pst_Dest->x = fMul(_pst_Dest->x, f_MaxDist);
		_pst_Dest->y = fMul(_pst_Dest->y, f_MaxDist);
		_pst_Dest->z = fMul(_pst_Dest->z, f_MaxDist);

		ast_ListVec[i].x = MATH_f_FloatRound(ast_ListVec[i].x, 0.1f);
		ast_ListVec[i].y = MATH_f_FloatRound(ast_ListVec[i].y, 0.1f);
		ast_ListVec[i].z = MATH_f_FloatRound(ast_ListVec[i].z, 0.1f);

		pst_Obj = COL_Visual_RayCast
			(
				_pst_World,
				_pst_GO,
				_pst_Origin,
				&ast_ListVec[i],
				f_MaxDist,
				_ul_Filter,
				_ul_NoFilter,
				_ul_ID,
				NULL,
				_b_UseRaySkipFlag,
				FALSE
			);
		ai_CanSee[i] = pst_Obj ? 0 : 1;

		/* Can we go to position ? */
		if(!pst_Obj)
		{
			MATH_CopyVector(&st_Tmp, _pst_Origin);
			MATH_AddVector(&st_Tmp, &st_Tmp, _pst_Dest);
			MATH_SubVector(&st_Tmp, &st_Tmp, OBJ_pst_GetAbsolutePosition(_pst_GO));
			f_Dist = MATH_f_NormVector(&st_Tmp);
			MATH_NormalizeVector(&st_Tmp, &st_Tmp);

			pst_Obj = COL_Visual_RayCast
				(
					_pst_World,
					_pst_GO,
					OBJ_pst_GetAbsolutePosition(_pst_GO),
					&st_Tmp,
					f_Dist,
					_ul_Filter,
					_ul_NoFilter,
					_ul_ID,
					NULL,
					_b_UseRaySkipFlag,
					FALSE
				);

			if(!pst_Obj)
			{
				*_pui_Mode |= 16;
				*_pui_Mode &= ~32;
				return;
			}
		}
	}

	/* Failed */
	for(i = 0; i <= i_NumVec; i++)
	{
		if(ai_CanSee[i])
		{
			MATH_CopyVector(_pst_Dest, &ast_ListVec[i]);
			_pst_Dest->x = fMul(_pst_Dest->x, f_MaxDist);
			_pst_Dest->y = fMul(_pst_Dest->y, f_MaxDist);
			_pst_Dest->z = fMul(_pst_Dest->z, f_MaxDist);
			*_pui_Mode |= 16;
			*_pui_Mode |= 32;
			return;
		}
	}

	*_pui_Mode &= ~16;
	*_pui_Mode &= ~32;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_COLRayCompute(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector	st_Origin, st_Direction, st_Dir;
	ULONG				ul_OnFlags, ul_OffFlags, ul_FlagID;
	BOOL				b_ColMap;
	int					*pi_Res;
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val1, &st_Var1);
	pi_Res = (int *) st_Var1.pv_Addr;

	b_ColMap = AI_PopInt();
	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_PopVector(&st_Direction);
	MATH_CopyVector(&st_Dir, &st_Direction);
	AI_PopVector(&st_Origin);

	/* Get the norm of the vector */
	COL_RayCompute
	(
		&st_Dir,
		pst_World,
		pst_GO,
		&st_Origin,
		&st_Direction,
		ul_OnFlags,
		ul_OffFlags,
		ul_FlagID,
		b_ColMap,
		(UINT *) pi_Res
	);

	AI_PushVector(&st_Dir);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the instance Priority the object.
 =======================================================================================================================
 */
void AI_EvalFunc_COLPrioritySet_C(OBJ_tdst_GameObject *_pst_GO, UCHAR _l_Prio)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!_pst_GO
	||	!_pst_GO->pst_Extended
	||	!_pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance
	) return;

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_Instance->c_Priority = (char) _l_Prio;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLPrioritySet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	LONG        l_Prio;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_Prio = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLPrioritySet_C(pst_GO, l_Prio);

	return ++_pst_Node;
}

extern ULONG				COL_gul_SoundFilter;
/*
 =======================================================================================================================
    Aim:    Sets the instance Priority the object.
 =======================================================================================================================
 */
void AI_EvalFunc_COLRaySoundFilterSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG ul_SoundID)
{
	COL_gul_SoundFilter = ul_SoundID;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRaySoundFilterSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG        ul_SoundID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_SoundID = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLRaySoundFilterSet_C(pst_GO, ul_SoundID);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Get the Instance Priority of the object.
 =======================================================================================================================
 */
int AI_EvalFunc_COLPriorityGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	if
	(
		!_pst_GO
	||	!_pst_GO->pst_Extended
	||	!_pst_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance
	)
	{
		return 0;
	}

	return((int) (((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->c_Priority));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLPriorityGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_COLPriorityGet_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLCrossObjectNumber_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(pst_World->ul_NbCrossed);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLCrossObjectNumber(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLCrossObjectNumber_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLCrossObjectGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(pst_World->apst_CrossObjects[_ul_Index]);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLCrossObjectGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);
	AI_PushGameObject(AI_EvalFunc_COLCrossObjectGet_C(pst_GO, ul_Index));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLFirstObjectInRangeGet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Pos,
	float				_f_Distance,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_GetFirstObjectInRange(pst_World, _pst_Pos, _f_Distance, _ul_OnFlags, _ul_OffFlags, _ul_FlagID));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLFirstObjectInRangeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Pos;
	ULONG				ul_FlagID, ul_OffFlags, ul_OnFlags;
	float				f_Distance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();
	f_Distance = AI_PopFloat();
	AI_PopVector(&st_Pos);

	AI_M_GetCurrentObject(pst_GO);

	AI_PushGameObject(AI_EvalFunc_COLFirstObjectInRangeGet_C(pst_GO, &st_Pos, f_Distance, ul_OnFlags, ul_OffFlags, ul_FlagID));

	return ++_pst_Node;
}

extern int	COL_TriangleCollision
			(
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				MATH_tdst_Vector *,
				WOR_tdst_World	 *,
				ULONG,
				ULONG,
				ULONG
			);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_COLTriangleCollision(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_A, st_B, st_C;
	ULONG				ul_FlagID, ul_OffFlags, ul_OnFlags;
	WOR_tdst_World		*pst_World;
	MATH_tdst_Vector	*pst_Res;
	AI_tdst_UnionVar	Val1;
	AI_tdst_PushVar		st_Var1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	ul_FlagID = AI_PopInt();
	ul_OffFlags = AI_PopInt();
	ul_OnFlags = AI_PopInt();

	AI_PopVar(&Val1, &st_Var1);
	pst_Res = (MATH_tdst_Vector *) st_Var1.pv_Addr;

	AI_PopVector(&st_C);
	AI_PopVector(&st_B);
	AI_PopVector(&st_A);

	AI_PushInt(COL_TriangleCollision(&st_A, &st_B, &st_C, pst_Res, pst_World, ul_OnFlags, ul_OffFlags, ul_FlagID));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COL_SpecificCrossableSet_C(USHORT _uw_Crossable)
{
	COL_guw_UserCrossable = _uw_Crossable;
}
/**/
AI_tdst_Node *AI_EvalFunc_COL_SpecificCrossableSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~*/
	USHORT	uw_Crossable;
	/*~~~~~~~~~~~~~~~~*/

	uw_Crossable = AI_PopInt();
	AI_EvalFunc_COL_SpecificCrossableSet_C(uw_Crossable);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COL_RayMinDistSet_C(float _f_MinDist)
{
	COL_gb_UseMinDistance = TRUE;
	COL_gf_MinDistance = _f_MinDist;
}
/**/
AI_tdst_Node *AI_EvalFunc_COL_RayMinDistSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~*/
	float	f_MinDist;
	/*~~~~~~~~~~~~~~~~*/

	f_MinDist = AI_PopFloat();
	AI_EvalFunc_COL_RayMinDistSet_C(f_MinDist);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Are 2 Objects Colliding ?
 =======================================================================================================================
 */
BOOL AI_EvalFunc_COLObjectCollide_C(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_A)
		pst_World = WOR_World_GetWorldOfObject(_pst_A);
	else
		pst_World = WOR_World_GetWorldOfObject(_pst_B);

	return (COL_Report_b_ObjectsAreColliding(_pst_A, _pst_B, pst_World));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLObjectCollide(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A;
	OBJ_tdst_GameObject *pst_B;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/


	AI_M_GetCurrentObject(pst_A);
	pst_B = AI_PopGameObject();

	AI_PushInt(AI_EvalFunc_COLObjectCollide_C(pst_A, pst_B));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLSwapToSpecific_C(OBJ_tdst_GameObject *_pst_GO, int _i_AI_Index)
{
	COL_SwapToSpecific(_pst_GO, (UCHAR) _i_AI_Index);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLSwapToSpecific(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_AI_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	AI_M_GetCurrentObject(pst_GO);
	i_AI_Index = AI_PopInt();

	AI_EvalFunc_COLSwapToSpecific_C(pst_GO, i_AI_Index);
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLFilterSet_C(OBJ_tdst_GameObject *_pst_GO, int _i_On, int _i_Off, int _i_Type)
{
	if(_pst_GO && _pst_GO->pst_Extended && _pst_GO->pst_Extended->pst_Col && ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance)
	{
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->ul_Filter_Type = _i_Type;
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->ul_Filter_On = _i_On;
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->ul_Filter_Off = _i_Off;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_COLFilterSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int	i_Type, i_On, i_Off;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	i_Type = AI_PopInt();
	i_Off = AI_PopInt();
	i_On = AI_PopInt();

	AI_EvalFunc_COLFilterSet_C(pst_GO, i_On, i_Off, i_Type);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Are 2 Objects Colliding ?
 =======================================================================================================================
 */
int AI_EvalFunc_COL_IndexReportGet_C(OBJ_tdst_GameObject *_pst_GO, int _i_FlagsToTest, int _i_FlagsToSkip)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return ((int) (COL_Report_ul_BestReportGet(_pst_GO, pst_World, (ULONG) _i_FlagsToTest, (ULONG) _i_FlagsToSkip)));
}
/**/
AI_tdst_Node *AI_EvalFunc_COL_IndexReportGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int				i_FlagsToTest;
	int				i_FlagsToSkip;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_FlagsToSkip = AI_PopInt();
	i_FlagsToTest = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COL_IndexReportGet_C(pst_GO, i_FlagsToTest, i_FlagsToSkip));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
    Aim:    User BackFace test
 =======================================================================================================================
 */
void AI_EvalFunc_COLBackFaceSet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_BackFace)
{
	if(!_pst_GO || !_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Col || !((COL_tdst_Base *)_pst_GO->pst_Extended->pst_Col)->pst_Instance) return;


	((COL_tdst_Base *)_pst_GO->pst_Extended->pst_Col)->pst_Instance->uc_UserBackFace = 1;
	MATH_CopyVector(&((COL_tdst_Base *)_pst_GO->pst_Extended->pst_Col)->pst_Instance->st_BackFace, _pst_BackFace);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBackFaceSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_BackFace;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_BackFace);

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLBackFaceSet_C(pst_GO, &st_BackFace);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Get the first report that has a particular GMat. -1 if none.
 =======================================================================================================================
 */
int AI_EvalFunc_COLGMatReportGet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _i_GMat)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	return (COL_Report_ul_GMatReportGet(_pst_GO, pst_World, _i_GMat));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLGMatReportGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_GMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_GMat = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COLGMatReportGet_C(pst_GO, i_GMat));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLUnColAdd_C(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	COL_AddUnCollidableObject(_pst_A, _pst_B);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLUnColAdd(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A, *pst_B;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_A);
	pst_B = AI_PopGameObject();

	AI_EvalFunc_COLUnColAdd_C(pst_A, pst_B);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLUnColTest_C(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	return(COL_b_UnCollidable(_pst_A, _pst_B));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLUnColTest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A, *pst_B;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_A);
	pst_B = AI_PopGameObject();

	AI_PushInt(AI_EvalFunc_COLUnColTest_C(pst_A, pst_B));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLUnColDel_C(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{

	COL_RemoveUnCollidableObject(_pst_A, _pst_B);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLUnColDel(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A, *pst_B;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_A);
	pst_B = AI_PopGameObject();

	AI_EvalFunc_COLUnColDel_C(pst_A, pst_B);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_COLUnColReset_C(OBJ_tdst_GameObject *_pst_A)
{
	COL_FlushAndFreeUnCollidableList(_pst_A);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLUnColReset(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_A);

	AI_EvalFunc_COLUnColReset_C(pst_A);

	return ++_pst_Node;
}

extern OBJ_tdst_GameObject *ANI_pst_GetReference(OBJ_tdst_GameObject *);
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLRayCanalGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	OBJ_tdst_GameObject		*pst_A, *pst_Ref;
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_GameObject		*pst_BoneGO;
	OBJ_tdst_Group			*pst_Skeleton;
	ANI_tdst_Shape			*pst_Shape;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	pst_A = pst_World->st_RayInfo.pst_CollidedGO;

	if(!pst_A || !(pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)) return -1;

	pst_Ref = ANI_pst_GetReference(pst_A);

	if
	(
		!pst_Ref
	||	!(pst_Ref->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	||	!(pst_Ref->pst_Base)
	||	!(pst_Ref->pst_Base->pst_GameObjectAnim)
	||	!(pst_Ref->pst_Base->pst_GameObjectAnim->pst_Shape)
	||	!(pst_Ref->pst_Base->pst_GameObjectAnim->pst_Skeleton)
	) return -1;
		
	pst_Skeleton = pst_Ref->pst_Base->pst_GameObjectAnim->pst_Skeleton;
	pst_Shape = pst_Ref->pst_Base->pst_GameObjectAnim->pst_Shape;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
	for(i = 0; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++, i++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		if(TAB_b_IsAHole(pst_BoneGO)) continue;

		if(pst_BoneGO == pst_A)
			return (int) (pst_Shape->auc_AI_Canal[i]);
	}

	return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRayCanalGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_A);

	AI_PushInt(AI_EvalFunc_COLRayCanalGet_C(pst_A));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_COLRayActorGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	OBJ_tdst_GameObject		*pst_A;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	pst_A = pst_World->st_RayInfo.pst_CollidedGO;

	if(!pst_A) return NULL;

	if(!(pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)) return pst_A;

	return (ANI_pst_GetReference(pst_A));
}
/**/
AI_tdst_Node *AI_EvalFunc_COLRayActorGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_A);

	AI_PushGameObject(AI_EvalFunc_COLRayActorGet_C(pst_A));

	return ++_pst_Node;
}
	
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COL_EdgeBothNormalGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Normal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);

	return(COL_Report_ul_SlipperyEdgeNormalsGet(_pst_GO, pst_World, _pst_Normal));
}
/**/
AI_tdst_Node *AI_EvalFunc_COL_EdgeBothNormalGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_COL_EdgeBothNormalGet_C(pst_GO, ((MATH_tdst_Vector *) st_Var.pv_Addr)));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_COLBVPointCollide_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Point)
{
	if(OBJ_BV_IsSphere(_pst_GO->pst_BV))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_GCS_Center;
		float				fRadius;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
		MATH_AddVector(&st_GCS_Center, OBJ_pst_BV_GetCenter(_pst_GO->pst_BV), &_pst_GO->pst_GlobalMatrix->T);
		fRadius = OBJ_f_BV_GetRadius(_pst_GO->pst_BV);

		return
			(
				(fAbs(_pst_Point->x - st_GCS_Center.x) < fRadius)
			&&	(fAbs(_pst_Point->y - st_GCS_Center.y) < fRadius)
			&&	(fAbs(_pst_Point->z - st_GCS_Center.z) < fRadius)
			);
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_GCS_Min, st_GCS_Max;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_AddVector(&st_GCS_Min, OBJ_pst_BV_GetGMin(_pst_GO->pst_BV), &_pst_GO->pst_GlobalMatrix->T);
		MATH_AddVector(&st_GCS_Max, OBJ_pst_BV_GetGMax(_pst_GO->pst_BV), &_pst_GO->pst_GlobalMatrix->T);

		return
			(
				((_pst_Point->x >= st_GCS_Min.x) && (_pst_Point->x <= st_GCS_Max.x))
			&&	((_pst_Point->y >= st_GCS_Min.y) && (_pst_Point->y <= st_GCS_Max.y))
			&&	((_pst_Point->z >= st_GCS_Min.z) && (_pst_Point->z <= st_GCS_Max.z))
			);

	}
}
/**/
AI_tdst_Node *AI_EvalFunc_COLBVPointCollide(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Point);

	AI_PushInt(AI_EvalFunc_COLBVPointCollide_C(pst_GO, &st_Point));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
void AI_EvalFunc_COLODEReportsSkip_C(OBJ_tdst_GameObject *_pst_GO)
#else
void AI_EvalFunc_COLODEReportsSkip_C()
#endif
{
	COL_gb_SkipODEReports = FALSE;
}
/**/
AI_tdst_Node *AI_EvalFunc_COLODEReportsSkip(AI_tdst_Node *_pst_Node)
{
#ifdef JADEFUSION
	AI_EvalFunc_COLODEReportsSkip_C(NULL);
#else
	AI_EvalFunc_COLODEReportsSkip_C();
#endif
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_COLODESoundWithPrioGet_C(OBJ_tdst_GameObject *_pst_GO, int *_pi_Array, int i_NumPrio)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	return COL_GameMaterial_GetODESound(_pst_GO, pst_World, (ULONG*)_pi_Array, i_NumPrio);
}
/**/
AI_tdst_Node *AI_EvalFunc_COLODESoundWithPrioGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	ULONG				ul_NumPrio;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_NumPrio = AI_PopInt();
	AI_PopVar(&Val, &st_Var);

	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt
	(
		AI_EvalFunc_COLODESoundWithPrioGet_C
		(
			pst_GO,
			((int *) st_Var.pv_Addr),
			ul_NumPrio
		)
	);

	return ++_pst_Node;
}


/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_COLEnableSnP_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* If GO is not is SnP, add it */
	if(!(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP))
	{
		pst_World = WOR_World_GetWorldOfObject(_pst_GO);

		_pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
		INT_SnP_AddObject(_pst_GO, pst_World->pst_SnP);
		INT_SnP_InsertionSort(pst_World->pst_SnP, 0);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_COLEnableSnP(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLEnableSnP_C(pst_GO);

	return ++_pst_Node;
}
#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_COLDisableSnP_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* If GO is in SnP, remove it */
	if(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP)
	{
		pst_World = WOR_World_GetWorldOfObject(_pst_GO);

		INT_SnP_DetachObject(_pst_GO, pst_World);
		_pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_EnableSnP;
}
}
/**/
AI_tdst_Node *AI_EvalFunc_COLDisableSnP(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_COLDisableSnP_C(pst_GO);

	return ++_pst_Node;
}
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
