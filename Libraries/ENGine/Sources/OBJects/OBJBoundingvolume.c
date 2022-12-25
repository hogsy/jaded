/*$T OBJBoundingvolume.c GC! 1.081 08/02/01 17:56:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "OBJBoundingVolume.h"
#include "GDInterface/GDInterface.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEO_MRM.h"
#include "LIGHT/LIGHTstruct.h"
#include "OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "TABles/TABles.h"
#include "OBJgrp.h"
#include "COLlision/COLstruct.h"
#include "COLlision/COLaccess.h"
#include "ANImation/ANIstruct.h"
#include "BASe/BAStypes.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_SymetriseMatrix(MATH_tdst_Matrix *pst_Matrix)
{
	*((ULONG *) &pst_Matrix->Ix) ^= 0x80000000;
	*((ULONG *) &pst_Matrix->Jx) ^= 0x80000000;
	*((ULONG *) &pst_Matrix->Kx) ^= 0x80000000;
	*((ULONG *) &pst_Matrix->T.x) ^= 0x80000000;
}

/*
 =======================================================================================================================
    Aim:    Returns wether the BV of the Object is sperical or not.
 =======================================================================================================================
 */
BOOL OBJ_BV_IsSphere(void *_pst_BV)
{
	return(*(LONG *) &((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax.z == -1);
}

/*
 =======================================================================================================================
    Aim:    Returns wether the BV of the Object is a AABBox or not.
 =======================================================================================================================
 */
BOOL OBJ_BV_IsAABBox(void *_pst_BV)
{
	return(*(LONG *) &((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax.z != -1);
}

/*
 =======================================================================================================================
    Aim:    Sets the BV internal Flag that indicates that the BV is a sphere.
 =======================================================================================================================
 */
void OBJ_BV_SetSphereFlag(void *_pst_BV)
{
	*(LONG *) &((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax.z = -1;
}

/*
 =======================================================================================================================
    Aim:    Translates an AABBox/Sphere. _pst_Move is the translation vector in the Global coordinate system.
 =======================================================================================================================
 */
void OBJ_BV_Move(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Move, BOOL _b_Init)
{
	/*~~~~~~~~~~~~*/
	void	*pst_BV;
	/*~~~~~~~~~~~~*/

	pst_BV = _pst_GO->pst_BV;

	if(OBJ_BV_IsSphere(pst_BV))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix st_Matrix	ONLY_PSX2_ALIGNED(16);
		MATH_tdst_Vector			st_Move;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_InvertMatrix(&st_Matrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO));

		/*
		 * After this, st_Move will be the translation vector in the Object coordinate
		 * system.
		 */
		MATH_TransformVector(&st_Move, &st_Matrix, _pst_Move);

		/* Finally, we move the center of the sphere. */
		MATH_AddEqualVector(OBJ_pst_BV_GetCenter(pst_BV), &st_Move);
	}
	else
	{
		if(_b_Init)
		{
			MATH_AddEqualVector(OBJ_pst_BV_GetGMinInit(pst_BV), _pst_Move);
			MATH_AddEqualVector(OBJ_pst_BV_GetGMaxInit(pst_BV), _pst_Move);
			MATH_CopyVector(OBJ_pst_BV_GetGMax(pst_BV), OBJ_pst_BV_GetGMaxInit(pst_BV));
			MATH_CopyVector(OBJ_pst_BV_GetGMin(pst_BV), OBJ_pst_BV_GetGMinInit(pst_BV));
		}
		else
		{
			/* Finally, we move the two points of the AABBox. */
			MATH_AddEqualVector(OBJ_pst_BV_GetGMin(pst_BV), _pst_Move);
			MATH_AddEqualVector(OBJ_pst_BV_GetGMax(pst_BV), _pst_Move);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Translates an OBBox. _pst_Move is the translation vector in the Object coordinate system.
 =======================================================================================================================
 */
void OBJ_BV_OBBoxMove(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Move)
{
	/*~~~~~~~~~~~~*/
	void	*pst_BV;
	/*~~~~~~~~~~~~*/

	pst_BV = _pst_GO->pst_BV;

#ifdef ACTIVE_EDITORS
	if(!_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV) return;
#endif
	/* Finally, we move the two points of the OBBox. */
	MATH_AddEqualVector(OBJ_pst_BV_GetLMin(pst_BV), _pst_Move);
	MATH_AddEqualVector(OBJ_pst_BV_GetLMax(pst_BV), _pst_Move);
}

/*
 =======================================================================================================================
    Aim:    Has the object a valid BV ?
 =======================================================================================================================
 */
BOOL OBJ_HasNoBV(OBJ_tdst_GameObject *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_GMax;
	void				*pst_BV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BV = _pst_Obj->pst_BV;
	if(OBJ_BV_IsAABBox(pst_BV))
	{
		pst_GMax = (OBJ_pst_BV_GetGMax(pst_BV));
		return(!(pst_GMax->x) && !(pst_GMax->y) && !(pst_GMax->z));
	}
	else
		return(!(OBJ_f_BV_GetRadius(pst_BV)));
}

/*
 =======================================================================================================================
    Aim:    Computes the center of a BV in the Global coordinate system.
 =======================================================================================================================
 */
void OBJ_BV_ComputeCenter(OBJ_tdst_GameObject *_pst_Obj, MATH_tdst_Vector *_pst_Center)
{
	if(OBJ_BV_IsAABBox(_pst_Obj->pst_BV))
	{
		MATH_AddVector(_pst_Center, OBJ_pst_BV_GetGMin(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMax(_pst_Obj->pst_BV));
		MATH_MulEqualVector(_pst_Center, 0.5f);
		MATH_AddEqualVector(_pst_Center, OBJ_pst_GetAbsolutePosition(_pst_Obj));
	}
	else
	{
		MATH_TransformVertexNoScale
		(
			_pst_Center,
			OBJ_pst_GetAbsoluteMatrix(_pst_Obj),
			OBJ_pst_BV_GetCenter(_pst_Obj->pst_BV)
		);
	}
}

/*
 =======================================================================================================================
    Aim:    Init the Sphere with one point of the object.
 =======================================================================================================================
 */
void OBJ_BV_InitSphereBV(void *_pst_BV, MATH_tdst_Vector *_pst_Point, float _f_Radius, MATH_tdst_Vector *_pst_Scale)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_ScaledPoint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Scale)
	{
		st_ScaledPoint.x = _pst_Point->x * _pst_Scale->x;
		st_ScaledPoint.y = _pst_Point->y * _pst_Scale->y;
		st_ScaledPoint.z = _pst_Point->z * _pst_Scale->z;

		MATH_CopyVector(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMin, &st_ScaledPoint);
	}
	else
		MATH_CopyVector(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMin, _pst_Point);
	((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax.x = _f_Radius;

	/* We set the flag that indicates that the BV is a Sphere. */
	OBJ_BV_SetSphereFlag(_pst_BV);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_BV_AddPointToSphereBV(void *_pst_BV, MATH_tdst_Vector *_pst_PointA, MATH_tdst_Vector *_pst_Scale)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_OldRadius, f_NormCA, f_Factor;
	MATH_tdst_Vector	st_CA;	/* Vector -Old Center of the Sphere- To -Point A- */
	MATH_tdst_Vector	st_ScaledA;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Scale)
	{
		st_ScaledA.x = _pst_PointA->x * _pst_Scale->x;
		st_ScaledA.y = _pst_PointA->y * _pst_Scale->y;
		st_ScaledA.z = _pst_PointA->z * _pst_Scale->z;

		MATH_SubVector(&st_CA, &st_ScaledA, OBJ_pst_BV_GetGMin(_pst_BV));
	}
	else
		MATH_SubVector(&st_CA, _pst_PointA, OBJ_pst_BV_GetGMin(_pst_BV));

	f_NormCA = MATH_f_NormVector(&st_CA);
	f_OldRadius = OBJ_f_BV_GetRadius(_pst_BV);

	if(f_NormCA <= f_OldRadius) return;

	f_Factor = fHalf(fSub(1.0f, fDiv(f_OldRadius, f_NormCA)));
	MATH_MulVector(&st_CA, &st_CA, f_Factor);

	/* New center. */
	MATH_AddEqualVector(OBJ_pst_BV_GetGMin(_pst_BV), &st_CA);

	/* New radius. */
	OBJ_v_BV_SetRadius(_pst_BV, fHalf(f_NormCA + f_OldRadius));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_BV_InitAABBox
(
	void				*_pst_BV,
	MATH_tdst_Matrix	*_pst_AbsoluteMatrix,
	MATH_tdst_Vector	*_pst_Point,
	BOOL				_b_NoScale
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_AbsolutePoint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_TransformVector(&st_AbsolutePoint, _pst_AbsoluteMatrix, _pst_Point);
	MATH_CopyVector(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMin, &st_AbsolutePoint);
	MATH_CopyVector(&((OBJ_tdst_SingleBV *) _pst_BV)->st_GMax, &st_AbsolutePoint);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_BV_AddPointToAABBox
(
	void				*_pst_BV,
	MATH_tdst_Matrix	*_pst_AbsoluteMatrix,
	MATH_tdst_Vector	*_pst_Point,
	BOOL				_bNoScale
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_AbsolutePoint;
	OBJ_tdst_SingleBV	*pst_SingleBV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_bNoScale)
		MATH_TransformVectorNoScale(&st_AbsolutePoint, _pst_AbsoluteMatrix, _pst_Point);
	else
		MATH_TransformVector(&st_AbsolutePoint, _pst_AbsoluteMatrix, _pst_Point);
	pst_SingleBV = (OBJ_tdst_SingleBV *) _pst_BV;

	if(st_AbsolutePoint.x < pst_SingleBV->st_GMin.x) pst_SingleBV->st_GMin.x = st_AbsolutePoint.x;
	if(st_AbsolutePoint.y < pst_SingleBV->st_GMin.y) pst_SingleBV->st_GMin.y = st_AbsolutePoint.y;
	if(st_AbsolutePoint.z < pst_SingleBV->st_GMin.z) pst_SingleBV->st_GMin.z = st_AbsolutePoint.z;

	if(st_AbsolutePoint.x > pst_SingleBV->st_GMax.x) pst_SingleBV->st_GMax.x = st_AbsolutePoint.x;
	if(st_AbsolutePoint.y > pst_SingleBV->st_GMax.y) pst_SingleBV->st_GMax.y = st_AbsolutePoint.y;
	if(st_AbsolutePoint.z > pst_SingleBV->st_GMax.z) pst_SingleBV->st_GMax.z = st_AbsolutePoint.z;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_BV_InitOBBox(void *_pst_BV, MATH_tdst_Vector *_pst_Point, MATH_tdst_Vector *_pst_Scale)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Scale)
	{
		st_Point.x = _pst_Scale->x * _pst_Point->x;
		st_Point.y = _pst_Scale->y * _pst_Point->y;
		st_Point.z = _pst_Scale->z * _pst_Point->z;

		MATH_CopyVector(&((OBJ_tdst_ComplexBV *) _pst_BV)->st_LMin, &st_Point);
		MATH_CopyVector(&((OBJ_tdst_ComplexBV *) _pst_BV)->st_LMax, &st_Point);
	}
	else
	{
		MATH_CopyVector(&((OBJ_tdst_ComplexBV *) _pst_BV)->st_LMin, _pst_Point);
		MATH_CopyVector(&((OBJ_tdst_ComplexBV *) _pst_BV)->st_LMax, _pst_Point);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_BV_AddPointToOBBox(void *_pst_BV, MATH_tdst_Vector *_pst_Point, MATH_tdst_Vector *_pst_Scale)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Point;
	MATH_tdst_Vector	*pst_LMin;
	MATH_tdst_Vector	*pst_LMax;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Scale)
	{
		st_Point.x = _pst_Scale->x * _pst_Point->x;
		st_Point.y = _pst_Scale->y * _pst_Point->y;
		st_Point.z = _pst_Scale->z * _pst_Point->z;
	}
	else
		MATH_CopyVector(&st_Point, _pst_Point);

	pst_LMin = OBJ_pst_BV_GetLMin(_pst_BV);
	pst_LMax = OBJ_pst_BV_GetLMax(_pst_BV);;

	if(st_Point.x < pst_LMin->x) pst_LMin->x = st_Point.x;
	if(st_Point.y < pst_LMin->y) pst_LMin->y = st_Point.y;
	if(st_Point.z < pst_LMin->z) pst_LMin->z = st_Point.z;

	if(st_Point.x > pst_LMax->x) pst_LMax->x = st_Point.x;
	if(st_Point.y > pst_LMax->y) pst_LMax->y = st_Point.y;
	if(st_Point.z > pst_LMax->z) pst_LMax->z = st_Point.z;
}

/*
 =======================================================================================================================
    Aim:    Init the Group BV with the First element object.
 =======================================================================================================================
 */
void OBJ_InitGroupBV(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_ACS_BMin, st_ACS_BMax;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_AddVector(&st_ACS_BMax, OBJ_pst_BV_GetGMax(_pst_B->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_B));
	MATH_AddVector(&st_ACS_BMin, OBJ_pst_BV_GetGMin(_pst_B->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_B));

	MATH_SubEqualVector(&st_ACS_BMax, OBJ_pst_GetAbsolutePosition(_pst_A));
	MATH_SubEqualVector(&st_ACS_BMin, OBJ_pst_GetAbsolutePosition(_pst_A));

	MATH_CopyVector(OBJ_pst_BV_GetGMax(_pst_A->pst_BV), &st_ACS_BMax);
	MATH_CopyVector(OBJ_pst_BV_GetGMin(_pst_A->pst_BV), &st_ACS_BMin);
}

/*
 =======================================================================================================================
    Aim:    Merges the 2 BVS into the first one.
 =======================================================================================================================
 */
void OBJ_MergeBVs(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_ACS_BMin, st_ACS_BMax;
	MATH_tdst_Vector	*pst_AMin, *pst_AMax;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Box + Box */
	if((OBJ_BV_IsAABBox(_pst_A->pst_BV)) && (OBJ_BV_IsAABBox(_pst_B->pst_BV)))
	{
		MATH_AddVector(&st_ACS_BMax, OBJ_pst_BV_GetGMax(_pst_B->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_B));
		MATH_AddVector(&st_ACS_BMin, OBJ_pst_BV_GetGMin(_pst_B->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_B));

		MATH_SubEqualVector(&st_ACS_BMax, OBJ_pst_GetAbsolutePosition(_pst_A));
		MATH_SubEqualVector(&st_ACS_BMin, OBJ_pst_GetAbsolutePosition(_pst_A));

		pst_AMin = OBJ_pst_BV_GetGMin(_pst_A->pst_BV);
		pst_AMax = OBJ_pst_BV_GetGMax(_pst_A->pst_BV);

		pst_AMin->x = fInf(st_ACS_BMin.x, pst_AMin->x) ? st_ACS_BMin.x : pst_AMin->x;
		pst_AMin->y = fInf(st_ACS_BMin.y, pst_AMin->y) ? st_ACS_BMin.y : pst_AMin->y;
		pst_AMin->z = fInf(st_ACS_BMin.z, pst_AMin->z) ? st_ACS_BMin.z : pst_AMin->z;

		pst_AMax->x = fSup(st_ACS_BMax.x, pst_AMax->x) ? st_ACS_BMax.x : pst_AMax->x;
		pst_AMax->y = fSup(st_ACS_BMax.y, pst_AMax->y) ? st_ACS_BMax.y : pst_AMax->y;
		pst_AMax->z = fSup(st_ACS_BMax.z, pst_AMax->z) ? st_ACS_BMax.z : pst_AMax->z;
		return;
	}

	/* Box + Sphere */
	if((OBJ_BV_IsAABBox(_pst_A->pst_BV)) && (OBJ_BV_IsSphere(_pst_B->pst_BV)))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_B_Min, st_B_Max, *pst_B_Center;
		float				f_B_Radius;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_B_Center = OBJ_pst_BV_GetCenter(_pst_B->pst_BV);
		f_B_Radius = OBJ_f_BV_GetRadius(_pst_B->pst_BV);

		st_B_Min.x = pst_B_Center->x - f_B_Radius;
		st_B_Min.y = pst_B_Center->y - f_B_Radius;
		st_B_Min.z = pst_B_Center->z - f_B_Radius;

		st_B_Max.x = pst_B_Center->x + f_B_Radius;
		st_B_Max.y = pst_B_Center->y + f_B_Radius;
		st_B_Max.z = pst_B_Center->z + f_B_Radius;
	}

	/* Sphere + Box */
	if((OBJ_BV_IsSphere(_pst_A->pst_BV)) && (OBJ_BV_IsAABBox(_pst_B->pst_BV)))
	{
	}

	/* Sphere + Sphere */
	if((OBJ_BV_IsSphere(_pst_A->pst_BV)) && (OBJ_BV_IsSphere(_pst_B->pst_BV)))
	{
	}
}

/*
 =======================================================================================================================
    Aim:    Comptutes the BV of an anim.
 =======================================================================================================================
 */
void OBJ_ComputeAnimBV(OBJ_tdst_GameObject *_pst_Obj, BOOL _b_AsSphere)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim	*pst_GOAnim;
	OBJ_tdst_Group			*pst_Skeleton;
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_GameObject		*pst_BoneGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Init of the BV. */
	MATH_InitVector(OBJ_pst_BV_GetGMin(_pst_Obj->pst_BV), 0.0f, 0.0f, 0.0f);
	MATH_InitVector(OBJ_pst_BV_GetGMax(_pst_Obj->pst_BV), 0.0f, 0.0f, 0.0f);

	if
	(
		(!OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_Anims))
	||	(!_pst_Obj->pst_Base)
	||	(!_pst_Obj->pst_Base->pst_GameObjectAnim)
	||	(!_pst_Obj->pst_Base->pst_GameObjectAnim->pst_Skeleton)
	) return;

	pst_GOAnim = _pst_Obj->pst_Base->pst_GameObjectAnim;
	pst_Skeleton = pst_GOAnim->pst_Skeleton;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);

	for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

		/*$F
		 * TO DEBUG:
		 * ---------
		 * If you stop at the following line, that means that there is one hole in the Skeleton Group.
		 * You have to remove it by processing the "Order Group" in the Editors.
		*/
		ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));
		if(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_OBBox) && !OBJ_b_TestControlFlag(pst_BoneGO, OBJ_C_ControlFlag_EditableBV)) OBJ_ComputeAABBoxFromOBBox(pst_BoneGO);
		OBJ_ComputeGlobalWhenHie(pst_BoneGO);
		OBJ_MergeBVs(_pst_Obj, pst_BoneGO);
	}

	MATH_CopyVector(OBJ_pst_BV_GetGMinInit(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMin(_pst_Obj->pst_BV));
	MATH_CopyVector(OBJ_pst_BV_GetGMaxInit(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMax(_pst_Obj->pst_BV));
}

/*
 =======================================================================================================================
    Aim:    Computes the AABBox of a Group.
 =======================================================================================================================
 */
void OBJ_ComputeGroupBV(OBJ_tdst_GameObject *_pst_Obj, BOOL _b_AsSphere)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group		*pst_Group;
	OBJ_tdst_GameObject *pst_GO;
	BOOL				b_First;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_First = TRUE;

	/* Gets the Group */
	pst_Group = (OBJ_tdst_Group *) _pst_Obj->pst_Extended->pst_Group;

	/* To avoid errors */
	if(!pst_Group) return;

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(OBJ_BV_IsSphere(pst_GO->pst_BV))
		{
#ifdef ACTIVE_EDITORS
			ERR_X_Warning(0, "AABBox needed for Group computation.", pst_GO->sz_Name);
#else
			ERR_X_Warning(0, "AABBox needed for Group computation.", NULL);
#endif
			return;
		}

		if(TAB_b_IsAHole(pst_GO)) continue;
		if(b_First)
		{
			OBJ_InitGroupBV(_pst_Obj, pst_GO);
			b_First = FALSE;
		}
		else
			OBJ_MergeBVs(_pst_Obj, pst_GO);
	}

	MATH_CopyVector(OBJ_pst_BV_GetGMinInit(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMin(_pst_Obj->pst_BV));
	MATH_CopyVector(OBJ_pst_BV_GetGMaxInit(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMax(_pst_Obj->pst_BV));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ComputeCameraBV(OBJ_tdst_GameObject *_pst_Obj, BOOL _b_AsSphere)
{
	/*~~~~~~~~~~~~*/
	void	*pst_BV;
	/*~~~~~~~~~~~~*/

	pst_BV = _pst_Obj->pst_BV;
	if(!_b_AsSphere)
	{
		MATH_InitVector(OBJ_pst_BV_GetGMin(pst_BV), -0.5f, -0.5f, -0.5f);
		MATH_InitVector(OBJ_pst_BV_GetGMax(pst_BV), 0.5f, 0.5f, 0.5f);
		MATH_CopyVector(OBJ_pst_BV_GetGMinInit(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMin(_pst_Obj->pst_BV));
		MATH_CopyVector(OBJ_pst_BV_GetGMaxInit(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMax(_pst_Obj->pst_BV));
	}
	else
	{
		MATH_InitVector(OBJ_pst_BV_GetCenter(pst_BV), 0.0f, 0.0f, 0.0f);
		OBJ_v_BV_SetRadius(pst_BV, 0.5f);
		OBJ_BV_SetSphereFlag(_pst_Obj->pst_BV);
	}

	if(OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_OBBox))
	{
		MATH_InitVector(OBJ_pst_BV_GetLMin(pst_BV), -0.5f, -0.5f, -0.5f);
		MATH_InitVector(OBJ_pst_BV_GetLMax(pst_BV), 0.5f, 0.5f, 0.5f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ComputeWaypointBV(OBJ_tdst_GameObject *_pst_Obj, BOOL _b_AsSphere)
{
	/*~~~~~~~~~~~~*/
	void	*pst_BV;
	/*~~~~~~~~~~~~*/

	pst_BV = _pst_Obj->pst_BV;
	if(OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_OBBox))
	{
		MATH_InitVector(OBJ_pst_BV_GetLMin(pst_BV), -1.0f, -1.0f, -1.0f);
		MATH_InitVector(OBJ_pst_BV_GetLMax(pst_BV), 1.0f, 1.0f, 1.0f);
	}
	else
	{
		if(!_b_AsSphere)
		{
			MATH_InitVector(OBJ_pst_BV_GetGMin(pst_BV), -1.0f, -1.0f, -1.0f);
			MATH_InitVector(OBJ_pst_BV_GetGMax(pst_BV), 1.0f, 1.0f, 1.0f);
			MATH_CopyVector(OBJ_pst_BV_GetGMinInit(pst_BV), OBJ_pst_BV_GetGMin(pst_BV));
			MATH_CopyVector(OBJ_pst_BV_GetGMaxInit(pst_BV), OBJ_pst_BV_GetGMax(pst_BV));
		}
		else
		{
			MATH_InitVector(OBJ_pst_BV_GetCenter(pst_BV), 0.0f, 0.0f, 0.0f);
			OBJ_v_BV_SetRadius(pst_BV, 1.0f);
			OBJ_BV_SetSphereFlag(_pst_Obj->pst_BV);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ComputeLightBV(OBJ_tdst_GameObject *_pst_Obj, BOOL _b_AsSphere)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LIGHT_tdst_Light	*pst_Light;
	void				*pst_BV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BV = _pst_Obj->pst_BV;
	pst_Light = (LIGHT_tdst_Light *) _pst_Obj->pst_Extended->pst_Light;

	switch(pst_Light->ul_Flags & LIGHT_Cul_LF_Type)
	{
	case LIGHT_Cul_LF_Omni:
		MATH_InitVector(OBJ_pst_BV_GetCenter(pst_BV), 0.0f, 0.0f, 0.0f);
		OBJ_v_BV_SetRadius(pst_BV, pst_Light->st_Omni.f_Far);
		OBJ_BV_SetSphereFlag(_pst_Obj->pst_BV);
		break;

	case LIGHT_Cul_LF_Direct:
	case LIGHT_Cul_LF_Spot:
	case LIGHT_Cul_LF_Fog:
		MATH_InitVector(OBJ_pst_BV_GetCenter(pst_BV), 0.0f, 0.0f, 0.0f);
		OBJ_v_BV_SetRadius(pst_BV, 0.0f);
		OBJ_BV_SetSphereFlag(_pst_Obj->pst_BV);
		break;

#ifdef JADEFUSION
    case LIGHT_Cul_LF_LightShaft:
        MATH_tdst_Vector avFrustum[8];
        MATH_tdst_Vector stMin;
        MATH_tdst_Vector stMax;

        LIGHT_GetLightShaftFrustum(&pst_Light->st_LightShaft, avFrustum);

        MATH_tdst_Vector* pstX = MATH_pst_GetXAxis(OBJ_pst_GetAbsoluteMatrix(_pst_Obj));
        MATH_tdst_Vector* pstY = MATH_pst_GetYAxis(OBJ_pst_GetAbsoluteMatrix(_pst_Obj));
        MATH_tdst_Vector* pstZ = MATH_pst_GetZAxis(OBJ_pst_GetAbsoluteMatrix(_pst_Obj));
        for (ULONG i = 0; i < 8; ++i)
        {
            MATH_tdst_Vector vTemp;
            MATH_tdst_Vector vTemp2;
            FLOAT fX = avFrustum[i].x;
            FLOAT fY = avFrustum[i].y;
            FLOAT fZ = avFrustum[i].z;

            MATH_ScaleVector(&vTemp, pstX, fX);
            MATH_AddScaleVector(&vTemp2, &vTemp, pstY, fY);
            MATH_AddScaleVector(&avFrustum[i], &vTemp2, pstZ, fZ);
        }

        MATH_CopyVector(&stMin, &avFrustum[0]);
        MATH_CopyVector(&stMax, &avFrustum[0]);
        for (ULONG i = 1; i < 8; ++i)
        {
            if (avFrustum[i].x < stMin.x) stMin.x = avFrustum[i].x;
            if (avFrustum[i].x > stMax.x) stMax.x = avFrustum[i].x;
            if (avFrustum[i].y < stMin.y) stMin.y = avFrustum[i].y;
            if (avFrustum[i].y > stMax.y) stMax.y = avFrustum[i].y;
            if (avFrustum[i].z < stMin.z) stMin.z = avFrustum[i].z;
            if (avFrustum[i].z > stMax.z) stMax.z = avFrustum[i].z;
        }

        MATH_CopyVector(OBJ_pst_BV_GetGMin(_pst_Obj->pst_BV), &stMin);
        MATH_CopyVector(OBJ_pst_BV_GetGMax(_pst_Obj->pst_BV), &stMax);
        MATH_CopyVector(OBJ_pst_BV_GetGMinInit(_pst_Obj->pst_BV), &stMin);
        MATH_CopyVector(OBJ_pst_BV_GetGMaxInit(_pst_Obj->pst_BV), &stMax);
        break;
#endif
	}
}

/*
 =======================================================================================================================
    Aim:    For RealTime BVs, compute the AABBox from the OBBox.

    Note:   We go thru all the points of the OBBox, transform them in the Global coordinate system (without the
            translation of the object ->TransformVector ) and find the AABBox.
 =======================================================================================================================
 */
void OBJ_ComputeAABBoxFromOBBox(OBJ_tdst_GameObject *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				l_Cpt;
	MATH_tdst_Vector	st_GlobalPoint, st_BoxPoint;
	MATH_tdst_Vector	*pst_GMin, *pst_GMax;
	OBJ_tdst_ComplexBV	*pst_BV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BV = (OBJ_tdst_ComplexBV *) _pst_Obj->pst_BV;

	pst_GMin = OBJ_pst_BV_GetGMin(pst_BV);
	pst_GMax = OBJ_pst_BV_GetGMax(pst_BV);

	MATH_InitVector(pst_GMin, Cf_Infinit, Cf_Infinit, Cf_Infinit);
	MATH_InitVector(pst_GMax, -Cf_Infinit, -Cf_Infinit, -Cf_Infinit);

	for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
	{
		st_BoxPoint.x = (l_Cpt & 1) ? pst_BV->st_LMin.x : pst_BV->st_LMax.x;
		st_BoxPoint.y = (l_Cpt & 2) ? pst_BV->st_LMin.y : pst_BV->st_LMax.y;
		st_BoxPoint.z = (l_Cpt & 4) ? pst_BV->st_LMin.z : pst_BV->st_LMax.z;

		/*
		 * The AABBox is in the Global coordinate system but WITHOUT the translation of
		 * the object.(--->TransformVector and not TransformVertex) .The scale is already
		 * in the OBBox structure, so in the transformation, we dont deal with it.
		 */
		MATH_TransformVectorNoScale(&st_GlobalPoint, OBJ_pst_GetAbsoluteMatrix(_pst_Obj), &st_BoxPoint);

		if(st_GlobalPoint.x < pst_GMin->x) pst_GMin->x = st_GlobalPoint.x;
		if(st_GlobalPoint.y < pst_GMin->y) pst_GMin->y = st_GlobalPoint.y;
		if(st_GlobalPoint.z < pst_GMin->z) pst_GMin->z = st_GlobalPoint.z;

		if(st_GlobalPoint.x > pst_GMax->x) pst_GMax->x = st_GlobalPoint.x;
		if(st_GlobalPoint.y > pst_GMax->y) pst_GMax->y = st_GlobalPoint.y;
		if(st_GlobalPoint.z > pst_GMax->z) pst_GMax->z = st_GlobalPoint.z;
	}
}

/*
 =======================================================================================================================
    Aim:    For RealTime BVs, compute the Sphere from the OBBox.
 =======================================================================================================================
 */
void OBJ_ComputeSphereFromOBBox(OBJ_tdst_GameObject *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Center;
	OBJ_tdst_ComplexBV	*pst_BV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BV = (OBJ_tdst_ComplexBV *) _pst_Obj->pst_BV;

	pst_Center = OBJ_pst_BV_GetCenter(pst_BV);

	MATH_SubVector(pst_Center, OBJ_pst_BV_GetLMax(pst_BV), OBJ_pst_BV_GetLMin(pst_BV));
	MATH_MulEqualVector(pst_Center, 0.5f);
	OBJ_v_BV_SetRadius(pst_BV, MATH_f_NormVector(pst_Center));
	MATH_AddEqualVector(pst_Center, OBJ_pst_BV_GetLMin(pst_BV));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ComputeGeometricObjectBV(OBJ_tdst_GameObject *_pst_Obj, BOOL _b_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex			*pst_Point, *pst_LastPoint;
	GEO_tdst_Object		*pst_Geo;
	MATH_tdst_Matrix	*pst_GlobalMatrix;
	BOOL				b_HasOBBox;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_HasOBBox = FALSE;
	pst_Geo = (GEO_tdst_Object *) _pst_Obj->pst_Base->pst_Visu->pst_Object;

	/* If the object has no point ... we return ... */
	if(!pst_Geo->dst_Point) return;

	pst_GlobalMatrix = OBJ_pst_GetAbsoluteMatrix(_pst_Obj);

	/*
	 * If the Geometric Object has MRM, we loop thru ALL the points and not only thru
	 * all the "MRM-filtered" points.
	 */
	pst_Point = pst_Geo->dst_Point + 1;
	pst_LastPoint = pst_Geo->dst_Point + GEO_MRM_GetRealNumberOfPoints(pst_Geo);;


	if(OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_OBBox))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Scale;
		MATH_tdst_Vector	*pst_LMin, *pst_LMax;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		b_HasOBBox = TRUE;
		if(MATH_b_TestScaleType(pst_GlobalMatrix))
		{
			MATH_GetScale(&st_Scale, pst_GlobalMatrix);
			OBJ_BV_InitOBBox(_pst_Obj->pst_BV, (MATH_tdst_Vector *) pst_Geo->dst_Point, &st_Scale);

			pst_Point = pst_Geo->dst_Point + 1;
			for(; pst_Point < pst_LastPoint; pst_Point++)
				OBJ_BV_AddPointToOBBox(_pst_Obj->pst_BV, (MATH_tdst_Vector *) pst_Point, &st_Scale);
		}
		else
		{
			OBJ_BV_InitOBBox(_pst_Obj->pst_BV, (MATH_tdst_Vector *) pst_Geo->dst_Point, NULL);

			pst_Point = pst_Geo->dst_Point + 1;
			for(; pst_Point < pst_LastPoint; pst_Point++)
				OBJ_BV_AddPointToOBBox(_pst_Obj->pst_BV, (MATH_tdst_Vector *) pst_Point, NULL);
		}

		if(OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_ColMap))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_ColMap *pst_ColMap;
			COL_tdst_Cob	*pst_Cob;
			int				i;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_ColMap = ((COL_tdst_Base *) _pst_Obj->pst_Extended->pst_Col)->pst_ColMap;

			if(pst_ColMap)
			{
				for(i = 0; i < pst_ColMap->uc_NbOfCob; i++)
				{
					pst_Cob = pst_ColMap->dpst_Cob[i];

					switch(pst_Cob->uc_Type)
					{
					case COL_C_Zone_Triangles:
						{
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
							COL_tdst_IndexedTriangles	*pst_CobObj;
							ULONG						j;
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

							pst_CobObj = (COL_tdst_IndexedTriangles *) pst_Cob->pst_TriangleCob;

							for(j = 0; j < pst_CobObj->l_NbPoints; j++)
							{
								OBJ_BV_AddPointToOBBox
								(
									_pst_Obj->pst_BV,
									(MATH_tdst_Vector *) &(pst_CobObj->dst_Point[j]),
									NULL
								);
							}
						}
						break;
					}
				}
			}
		}

		/* We dont want a "zero width" BV. */
		pst_LMin = OBJ_pst_BV_GetLMin(_pst_Obj->pst_BV);
		pst_LMax = OBJ_pst_BV_GetLMax(_pst_Obj->pst_BV);

		pst_LMin->x = (pst_LMax->x == pst_LMin->x) ? pst_LMin->x - 0.01f : pst_LMin->x;
		pst_LMin->y = (pst_LMax->y == pst_LMin->y) ? pst_LMin->y - 0.01f : pst_LMin->y;
		pst_LMin->z = (pst_LMax->z == pst_LMin->z) ? pst_LMin->z - 0.01f : pst_LMin->z;
	}


	if(_b_Type == OBJ_C_BV_Sphere)
	{
		if(b_HasOBBox)
		{
			OBJ_ComputeSphereFromOBBox(_pst_Obj);
			OBJ_BV_SetSphereFlag(_pst_Obj->pst_BV);
		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Scale;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(MATH_b_TestScaleType(pst_GlobalMatrix))
			{
				MATH_GetScale(&st_Scale, pst_GlobalMatrix);
				OBJ_BV_InitSphereBV(_pst_Obj->pst_BV, (MATH_tdst_Vector *) pst_Geo->dst_Point, 0.0f, &st_Scale);
				for(; pst_Point < pst_LastPoint; pst_Point++)
					OBJ_BV_AddPointToSphereBV(_pst_Obj->pst_BV, (MATH_tdst_Vector *) pst_Point, &st_Scale);
			}
			else
			{
				OBJ_BV_InitSphereBV(_pst_Obj->pst_BV, (MATH_tdst_Vector *) pst_Geo->dst_Point, 0.0f, NULL);
				for(; pst_Point < pst_LastPoint; pst_Point++)
					OBJ_BV_AddPointToSphereBV(_pst_Obj->pst_BV, (MATH_tdst_Vector *) pst_Point, NULL);
			}
		}
	}
	else
	{
		if(b_HasOBBox)
		{
			OBJ_ComputeAABBoxFromOBBox(_pst_Obj);
			MATH_CopyVector(OBJ_pst_BV_GetGMinInit(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMin(_pst_Obj->pst_BV));
			MATH_CopyVector(OBJ_pst_BV_GetGMaxInit(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMax(_pst_Obj->pst_BV));
		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	*pst_GMax, *pst_GMin;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			OBJ_BV_InitAABBox
			(
				_pst_Obj->pst_BV,
				OBJ_pst_GetAbsoluteMatrix(_pst_Obj),
				(MATH_tdst_Vector *) pst_Geo->dst_Point,
				FALSE
			);
			for(; pst_Point < pst_LastPoint; pst_Point++)
			{
				OBJ_BV_AddPointToAABBox
				(
					_pst_Obj->pst_BV,
					OBJ_pst_GetAbsoluteMatrix(_pst_Obj),
					(MATH_tdst_Vector *) pst_Point,
					FALSE
				);
			}

			if(OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_ColMap))
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~*/
				COL_tdst_ColMap *pst_ColMap;
				COL_tdst_Cob	*pst_Cob;
				int				i;
				/*~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_ColMap = ((COL_tdst_Base *) _pst_Obj->pst_Extended->pst_Col)->pst_ColMap;

				for(i = 0; i < pst_ColMap->uc_NbOfCob; i++)
				{
					pst_Cob = pst_ColMap->dpst_Cob[i];

					switch(pst_Cob->uc_Type)
					{
					case COL_C_Zone_Triangles:
						{
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
							COL_tdst_IndexedTriangles	*pst_CobObj;
							ULONG						j;
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

							pst_CobObj = (COL_tdst_IndexedTriangles *) pst_Cob->pst_TriangleCob;

							for(j = 0; pst_CobObj && j < pst_CobObj->l_NbPoints; j++)
							{
								OBJ_BV_AddPointToAABBox
								(
									_pst_Obj->pst_BV,
									OBJ_pst_GetAbsoluteMatrix(_pst_Obj),
									(MATH_tdst_Vector *) &(pst_CobObj->dst_Point[j]),
									TRUE
								);
							}
						}
						break;
					}
				}
			}

			/* We dont want a "zero width" BV. */
			pst_GMax = OBJ_pst_BV_GetGMax(_pst_Obj->pst_BV);
			pst_GMin = OBJ_pst_BV_GetGMin(_pst_Obj->pst_BV);

			pst_GMin->x = (pst_GMax->x == pst_GMin->x) ? pst_GMin->x - 0.01f : pst_GMin->x;
			pst_GMin->y = (pst_GMax->y == pst_GMin->y) ? pst_GMin->y - 0.01f : pst_GMin->y;
			pst_GMin->z = (pst_GMax->z == pst_GMin->z) ? pst_GMin->z - 0.01f : pst_GMin->z;

			MATH_CopyVector(OBJ_pst_BV_GetGMinInit(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMin(_pst_Obj->pst_BV));
			MATH_CopyVector(OBJ_pst_BV_GetGMaxInit(_pst_Obj->pst_BV), OBJ_pst_BV_GetGMax(_pst_Obj->pst_BV));
		}
	}

	/*
	 * The Bounding Volumes are in the OBJ_tdst_GameObject and in the GEO_tdst_Object
	 * ... The second one is a pointer to the first ..
	 */
	pst_Geo->pst_BV = _pst_Obj->pst_BV;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ComputeStaticLODObjectBV(OBJ_tdst_GameObject *_pst_Obj, BOOL _b_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_StaticLOD	*pst_LOD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_LOD = (GEO_tdst_StaticLOD *) _pst_Obj->pst_Base->pst_Visu->pst_Object;
	if(pst_LOD->dpst_Id[0])
	{
		_pst_Obj->pst_Base->pst_Visu->pst_Object = pst_LOD->dpst_Id[0];
		OBJ_ComputeGeometricObjectBV(_pst_Obj, _b_Type);
		_pst_Obj->pst_Base->pst_Visu->pst_Object = &pst_LOD->st_Id;
	}
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_CheckBV(OBJ_tdst_GameObject *_pst_GO)
{
	if(OBJ_BV_IsAABBox(_pst_GO->pst_BV))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	*pst_Max, *pst_Min;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Max = OBJ_pst_BV_GetGMax(_pst_GO->pst_BV);
		pst_Min = OBJ_pst_BV_GetGMin(_pst_GO->pst_BV);

		if((pst_Min->x > pst_Max->x) || (pst_Min->y > pst_Max->y) || (pst_Min->z > pst_Max->z))
		{
			ERR_X_Warning(0, "Flip detected in BV of GameObject", _pst_GO->sz_Name);
		}
	}
}

#endif

/*$F
 ===================================================================================================
 Note:	For now, the _b_Type is obsolete. We force it depending on the type of GameObject.

		- For Lights			Spheres
		- For all the rest		AABBox
 ===================================================================================================
 */

void OBJ_ComputeBV(OBJ_tdst_GameObject *_pst_Obj, BOOL _b_Force, BOOL _b_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object *pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	((OBJ_tdst_SingleBV *) _pst_Obj->pst_BV)->pst_GO = _pst_Obj;
#endif
	/* If the current object has the Editable Flag, we return. */
	if((_b_Force != OBJ_C_BV_ComputeIfNotLoaded) && OBJ_b_TestControlFlag(_pst_Obj, OBJ_C_ControlFlag_EditableBV))
	{
#ifdef ACTIVE_EDITORS
		/*
		 * The BV of this object has been edited. There can be a "double" flip that cannot
		 * be seen in 3D View. Checking ...
		 */
		OBJ_CheckBV(_pst_Obj);
#endif
		return;
	}

	/* Lights */
	if(_pst_Obj->pst_Extended && _pst_Obj->pst_Extended->pst_Light)
	{
		if(OBJ_HasNoBV(_pst_Obj) || (_b_Force == OBJ_C_BV_ForceComputation))
			OBJ_ComputeLightBV(_pst_Obj, OBJ_C_BV_Sphere);
		return;
	}

	/* Anims */
	if(OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_Anims))
	{
		if(OBJ_HasNoBV(_pst_Obj) || (_b_Force == OBJ_C_BV_ForceComputation))
			OBJ_ComputeAnimBV(_pst_Obj, OBJ_C_BV_AABBox);
		return;
	}

	/* Geometric Object Computation */
	if(OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_Visu))
	{
		pst_Geo = (GEO_tdst_Object *) _pst_Obj->pst_Base->pst_Visu->pst_Object;
		if(!pst_Geo) return;
		if(!pst_Geo->st_Id.i) return;

		switch(pst_Geo->st_Id.i->ul_Type)
		{
		case GRO_Geometric:
			if(OBJ_HasNoBV(_pst_Obj) || (_b_Force == OBJ_C_BV_ForceComputation))
				OBJ_ComputeGeometricObjectBV(_pst_Obj, OBJ_C_BV_AABBox);
			break;
		case GRO_GeoStaticLOD:
			if(OBJ_HasNoBV(_pst_Obj) || (_b_Force == OBJ_C_BV_ForceComputation))
				OBJ_ComputeStaticLODObjectBV(_pst_Obj, OBJ_C_BV_AABBox);
			break;

		case GRO_Camera:
			if(OBJ_HasNoBV(_pst_Obj) || (_b_Force == OBJ_C_BV_ForceComputation))
				OBJ_ComputeCameraBV(_pst_Obj, OBJ_C_BV_AABBox);
			break;
		case GRO_Unknown:
			break;
		}
	}
	else
	{
		
		/* Waypoints */
		if(OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_Waypoints))
		{
			if(OBJ_HasNoBV(_pst_Obj) || (_b_Force == OBJ_C_BV_ForceComputation))
				OBJ_ComputeWaypointBV(_pst_Obj, OBJ_C_BV_AABBox);
			return;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ReComputeRealTimeBVs(TAB_tdst_PFtable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO) || (OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_EditableBV))) continue;

		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_OBBox))
		{
			if(OBJ_BV_IsAABBox(pst_GO->pst_BV)) OBJ_ComputeAABBoxFromOBBox(pst_GO);
			continue;
		}

		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
		{
			OBJ_ComputeAnimBV(pst_GO, OBJ_C_BV_AABBox);
			continue;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_DuplicateBV(OBJ_tdst_GameObject *_pst_SrcGO, OBJ_tdst_GameObject *_pst_NewGO)
{
	if(OBJ_b_TestIdentityFlag(_pst_SrcGO, OBJ_C_IdentityFlag_OBBox))
		*(OBJ_tdst_ComplexBV *) (_pst_NewGO->pst_BV) = *(OBJ_tdst_ComplexBV *) (_pst_SrcGO->pst_BV);
	else
		*(OBJ_tdst_SingleBV *) (_pst_NewGO->pst_BV) = *(OBJ_tdst_SingleBV *) (_pst_SrcGO->pst_BV);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Checks if the BV of the Object englobes the object and the ZDM/Cob that it contains.
 =======================================================================================================================
 */
BOOL OBJ_CheckOneBV(OBJ_tdst_GameObject *_pst_Obj, ULONG _ul_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex			*pst_Point, *pst_LastPoint;
	MATH_tdst_Vector	st_GCS_Point;
	MATH_tdst_Vector	st_GCS_Min, st_GCS_Max;
	GEO_tdst_Object		*pst_Geo;
	MATH_tdst_Matrix	*pst_GlobalMatrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GlobalMatrix = OBJ_pst_GetAbsoluteMatrix(_pst_Obj);

	MATH_AddVector(&st_GCS_Max, OBJ_pst_BV_GetGMax(_pst_Obj->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_Obj));
	MATH_AddVector(&st_GCS_Min, OBJ_pst_BV_GetGMin(_pst_Obj->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_Obj));

	if((_ul_Flags & OBJ_C_IdentityFlag_Visu) && OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_Visu))
	{
		pst_Geo = (GEO_tdst_Object *) _pst_Obj->pst_Base->pst_Visu->pst_Object;
		if(!pst_Geo->dst_Point) return TRUE;

		if(pst_Geo->st_Id.i->ul_Type == GRO_Geometric)
		{
			/* First, we check if the points of the Object are in the AABBox. */
			pst_Point = pst_Geo->dst_Point;
			pst_LastPoint = pst_Geo->dst_Point + pst_Geo->l_NbPoints;
			for(; pst_Point < pst_LastPoint; pst_Point++)
			{
				MATH_TransformVertex(&st_GCS_Point, pst_GlobalMatrix, (MATH_tdst_Vector *) pst_Point);
				if
				(
					(st_GCS_Point.x + 0.01f < st_GCS_Min.x)
				||	(st_GCS_Point.x - 0.01f > st_GCS_Max.x)
				||	(st_GCS_Point.y + 0.01f < st_GCS_Min.y)
				||	(st_GCS_Point.y - 0.01f > st_GCS_Max.y)
				||	(st_GCS_Point.z + 0.01f < st_GCS_Min.z)
				||	(st_GCS_Point.z - 0.01f > st_GCS_Max.z)
				)
				{
					ERR_X_Warning(0, "Check BV Size [Geometric]", _pst_Obj->sz_Name);
					ERR_X_ForceError("Check BV Size [Geometric]", _pst_Obj->sz_Name);
					return FALSE;
				}
			}
		}
	}

	if((_ul_Flags & OBJ_C_IdentityFlag_ZDM) && OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_ZDM))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Instance	*pst_Instance;
		COL_tdst_ZDx		**dpst_ZDx, **dpst_LastZDx;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Instance = ((COL_tdst_Base *) _pst_Obj->pst_Extended->pst_Col)->pst_Instance;
		dpst_ZDx = pst_Instance->dpst_ZDx;
		dpst_LastZDx = dpst_ZDx + pst_Instance->uc_NbOfZDx;
		for(; dpst_ZDx < dpst_LastZDx; dpst_ZDx++)
		{
			if(COL_b_Zone_TestFlag(*dpst_ZDx, COL_C_Zone_ZDM))
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	st_GCS_Center;
				float				f_GCS_Radius;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_TransformVertex(&st_GCS_Center, pst_GlobalMatrix, COL_pst_Shape_GetCenter((*dpst_ZDx)->p_Shape));
				if(MATH_b_TestScaleType(pst_GlobalMatrix))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	st_Scale;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_GetScale(&st_Scale, pst_GlobalMatrix);
					f_GCS_Radius = COL_f_Shape_GetRadius((*dpst_ZDx)->p_Shape) * fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
				}
				else
					f_GCS_Radius = COL_f_Shape_GetRadius((*dpst_ZDx)->p_Shape);
				if
				(
					((st_GCS_Center.x - f_GCS_Radius) < st_GCS_Min.x)
				||	((st_GCS_Center.x + f_GCS_Radius) > st_GCS_Max.x)
				||	((st_GCS_Center.y - f_GCS_Radius) < st_GCS_Min.y)
				||	((st_GCS_Center.y + f_GCS_Radius) > st_GCS_Max.y)
				||	((st_GCS_Center.z - f_GCS_Radius) < st_GCS_Min.z)
				||	((st_GCS_Center.z + f_GCS_Radius) > st_GCS_Max.z)
				)
				{
					ERR_X_Warning(0, "Check BV Size [ZDM]", _pst_Obj->sz_Name);
					return FALSE;
				}
			}
		}
	}

	if((_ul_Flags & OBJ_C_IdentityFlag_ColMap) && OBJ_b_TestIdentityFlag(_pst_Obj, OBJ_C_IdentityFlag_ColMap))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_ColMap *pst_ColMap;
		COL_tdst_Cob	**dpst_Cob, **dpst_LastCob;
		ULONG			ul_Type;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_ColMap = ((COL_tdst_Base *) _pst_Obj->pst_Extended->pst_Col)->pst_ColMap;
		dpst_Cob = pst_ColMap->dpst_Cob;
		dpst_LastCob = dpst_Cob + pst_ColMap->uc_NbOfCob;
		for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
		{
			ul_Type = COL_Cob_GetType(*dpst_Cob);
			switch(ul_Type)
			{
			case COL_C_Zone_Sphere:
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	st_GCS_Center;
					float				f_GCS_Radius;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_TransformVertex
					(
						&st_GCS_Center,
						pst_GlobalMatrix,
						COL_pst_Shape_GetCenter((*dpst_Cob)->pst_MathCob->p_Shape)
					);
					if(MATH_b_TestScaleType(pst_GlobalMatrix))
					{
						/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
						MATH_tdst_Vector	st_Scale;
						/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

						MATH_GetScale(&st_Scale, pst_GlobalMatrix);
						f_GCS_Radius = COL_f_Shape_GetRadius((*dpst_Cob)->pst_MathCob->p_Shape) * fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
					}
					else
						f_GCS_Radius = COL_f_Shape_GetRadius((*dpst_Cob)->pst_MathCob->p_Shape);
					if
					(
						((st_GCS_Center.x - f_GCS_Radius) < st_GCS_Min.x)
					||	((st_GCS_Center.x + f_GCS_Radius) > st_GCS_Max.x)
					||	((st_GCS_Center.y - f_GCS_Radius) < st_GCS_Min.y)
					||	((st_GCS_Center.y + f_GCS_Radius) > st_GCS_Max.y)
					||	((st_GCS_Center.z - f_GCS_Radius) < st_GCS_Min.z)
					||	((st_GCS_Center.z + f_GCS_Radius) > st_GCS_Max.z)
					)
					{
						ERR_X_Warning(0, "Check BV Size [ColMap Sphere]", _pst_Obj->sz_Name);
						return FALSE;
					}
					break;
				}

			case COL_C_Zone_Box:
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	st_BoxPoint, st_GlobalPoint;
					MATH_tdst_Vector	*pst_CobMin, *pst_CobMax;
					ULONG				l_Cpt;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_CobMin = COL_pst_Shape_GetMin((*dpst_Cob)->pst_MathCob->p_Shape);

					pst_CobMax = COL_pst_Shape_GetMax((*dpst_Cob)->pst_MathCob->p_Shape);

					for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
					{
						st_BoxPoint.x = (l_Cpt & 1) ? pst_CobMin->x : pst_CobMax->x;
						st_BoxPoint.y = (l_Cpt & 2) ? pst_CobMin->y : pst_CobMax->y;
						st_BoxPoint.z = (l_Cpt & 4) ? pst_CobMin->z : pst_CobMax->z;

						MATH_TransformVertex(&st_GlobalPoint, OBJ_pst_GetAbsoluteMatrix(_pst_Obj), &st_BoxPoint);

						if
						(
							(st_GlobalPoint.x < st_GCS_Min.x)
						||	(st_GlobalPoint.x > st_GCS_Max.x)
						||	(st_GlobalPoint.y < st_GCS_Min.y)
						||	(st_GlobalPoint.y > st_GCS_Max.y)
						||	(st_GlobalPoint.z < st_GCS_Min.z)
						||	(st_GlobalPoint.z > st_GCS_Max.z)
						)
						{
							ERR_X_Warning(0, "Check BV Size [ColMap Box]", _pst_Obj->sz_Name);
							return FALSE;
						}
					}
					break;
				}

			case COL_C_Zone_Cylinder:
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	st_BoxPoint, st_GlobalPoint;
					MATH_tdst_Vector	st_CobMin, st_CobMax;
					MATH_tdst_Vector	*pst_Center;
					float				f_Radius, f_Height;
					ULONG				l_Cpt;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_Center = COL_pst_Shape_GetCenter((*dpst_Cob)->pst_MathCob->p_Shape);
					f_Radius = COL_f_Shape_GetRadius((*dpst_Cob)->pst_MathCob->p_Shape);
					f_Height = COL_f_Shape_GetHeight((*dpst_Cob)->pst_MathCob->p_Shape);

					MATH_CopyVector(&st_CobMin, pst_Center);
					MATH_CopyVector(&st_CobMax, pst_Center);

					st_CobMin.x -= f_Radius;
					st_CobMax.x += f_Radius;

					st_CobMin.y -= f_Radius;
					st_CobMax.y += f_Radius;

					st_CobMin.z -= f_Height;
					st_CobMax.z += f_Height;

					for(l_Cpt = 0; l_Cpt < 8; l_Cpt++)
					{
						st_BoxPoint.x = (l_Cpt & 1) ? st_CobMin.x : st_CobMax.x;
						st_BoxPoint.y = (l_Cpt & 2) ? st_CobMin.y : st_CobMax.y;
						st_BoxPoint.z = (l_Cpt & 4) ? st_CobMin.z : st_CobMax.z;

						MATH_TransformVertex(&st_GlobalPoint, OBJ_pst_GetAbsoluteMatrix(_pst_Obj), &st_BoxPoint);

						if
						(
							(st_GlobalPoint.x < st_GCS_Min.x)
						||	(st_GlobalPoint.x > st_GCS_Max.x)
						||	(st_GlobalPoint.y < st_GCS_Min.y)
						||	(st_GlobalPoint.y > st_GCS_Max.y)
						||	(st_GlobalPoint.z < st_GCS_Min.z)
						||	(st_GlobalPoint.z > st_GCS_Max.z)
						)
						{
							ERR_X_Warning(0, "Check BV Size [ColMap Cylinder]", _pst_Obj->sz_Name);
							return FALSE;
						}
					}
					break;

				case COL_C_Zone_Triangles:
					{
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
						COL_tdst_IndexedTriangles	*pst_CobObj;
						MATH_tdst_Vector			st_GlobalPoint;
						ULONG						i;
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

						pst_CobObj = (COL_tdst_IndexedTriangles *) (*dpst_Cob)->pst_TriangleCob;

						for(i = 0; i < pst_CobObj->l_NbPoints; i++)
						{
							MATH_TransformVertex
							(
								&st_GlobalPoint,
								_pst_Obj->pst_GlobalMatrix,
								&pst_CobObj->dst_Point[i]
							);

							if
							(
								(st_GlobalPoint.x < st_GCS_Min.x)
							||	(st_GlobalPoint.x > st_GCS_Max.x)
							||	(st_GlobalPoint.y < st_GCS_Min.y)
							||	(st_GlobalPoint.y > st_GCS_Max.y)
							||	(st_GlobalPoint.z < st_GCS_Min.z)
							||	(st_GlobalPoint.z > st_GCS_Max.z)
							)
							{
								ERR_X_Warning(0, "Check BV Size [Triangles-based ColMap]", _pst_Obj->sz_Name);
								return FALSE;
							}
						}
					}
					break;
				}
			}
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_CheckAllBVs(TAB_tdst_PFtable *_pst_Table, ULONG _ul_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	BOOL				b_Ok;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Ok = TRUE;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO) || !OBJ_b_TestIdentityFlag(pst_GO, _ul_Flags)) continue;

		if(!OBJ_CheckOneBV(pst_GO, _ul_Flags)) b_Ok = FALSE;
	}

	if(b_Ok)
	{
		ERR_X_ForceError("No BV Size Problem", NULL);
	}
	else
	{
		ERR_X_ForceError("BV Size Problems. Look in Log", NULL);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ReComputeAllBVs(TAB_tdst_PFtable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		OBJ_ComputeBV(pst_GO, OBJ_C_BV_ForceComputation, OBJ_C_BV_AABBox);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_HideAllBVs(TAB_tdst_PFtable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_ShowBV;
	}
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
