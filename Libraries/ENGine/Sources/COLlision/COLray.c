/*$T COLray.c GC! 1.081 10/14/02 15:01:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/BIGio.h"

#include "COLstruct.h"
#include "COLaccess.h"
#include "COLcob.h"
#include "WORld/WORstruct.h"
#include "WORld/WORaccess.h"
#include "INTersection/INTmain.h"
#include "GEOmetric/GEOobject.h"

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GAO.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

typedef struct	COL_tdst_Ray
{
	WOR_tdst_World		*pst_World;
	OBJ_tdst_GameObject *pst_ClosestGO;
	OBJ_tdst_GameObject *pst_GMat_GO;
	MATH_tdst_Vector	*pst_GCS_Origin;
	MATH_tdst_Vector	*pst_GCS_Dir;
	float				f_GCS_MaxDist;
	float				f_GCS_CurrentDist;
	BOOL				b_UseRaySkipFlag;
	BOOL				b_SkipCrossable;
	BOOL				b_SkipCamera;
	BOOL				b_SkipFlagX;
	BOOL				b_UserCrossable;
	BOOL				b_ReturnLocalData;
	BOOL				b_UseBoneOBBox;
	BOOL				*pb_Visible;
	int					SoundIDFilter;
};

int	NumEmptyBox = 0;
USHORT				COL_guw_UserCrossable;
ULONG				COL_gul_SoundFilter = -1;
float				COL_gf_MinDistance = 0.0f;
BOOL				COL_gb_UseMinDistance = FALSE;

struct COL_tdst_Ray COL_gst_Ray;

#define RAY_ACCESS(a)	COL_gst_Ray.a

#ifdef ACTIVE_EDITORS
BOOL						COL_b_LogPhoto = FALSE;
#endif
extern MDF_tdst_Modifier	*GAO_ModifierPhoto_Get(OBJ_tdst_GameObject *, BOOL);
extern BOOL					GAO_ModifierPhoto_LODAndFrameareOK(GAO_tdst_ModifierPhoto *);
extern OBJ_tdst_GameObject	*GAO_ModifierPhoto_SnapGOGet(OBJ_tdst_GameObject *, GAO_tdst_ModifierPhoto *, int);

/*$F		
					------------------	
					RAY CAST FUNCTIONS
					------------------	

BOOL				COL_RaySphereBV(...)
BOOL				COL_RayAABBox(...)
GameObject			COL_RayBV(...)


void				COL_ColMap_OK3_RayCast(...)
void				COL_ColMap_Triangles_RayCast(...)
void				COL_ColMap_Sphere_RayCast(...)
void				COL_ColMap_Box_RayCast(...)
void				COL_ColMap_Cylinder_RayCast(...)
void				COL_ColMap_RayCast(...)

void				COL_Visual_OK3_RayCast(...)
void				COL_Visual_Triangles_RayCast(...)
void				COL_Visual_RayCast(...)

					--------------
				    PHOTO ANALYSER
					--------------

ULONG				IMG_ObjectAnalyser(...)
ULONG				IMG_Analyser(...)
					
*/

void COL_OK3_RayCast(OBJ_tdst_GameObject *, void *, BOOL);


BOOL COL_RaySphereBV
(
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	OBJ_tdst_GameObject *_pst_GO,
	float				*_pf_Dist,
	float				_f_MinDist
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_Sphere		st_Sphere;
	MATH_tdst_Vector	st_SpherePoint, st_Vect;
	OBJ_tdst_SingleBV	*pst_SingleBV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SingleBV = (OBJ_tdst_SingleBV *) _pst_GO->pst_BV;

	/*
	 * The Sphere is in the local coordinate system of the object. We have to compute
	 * it in the Global coordinate system before entering the function
	 */
	MATH_AddVector(&st_Sphere.st_Center, OBJ_pst_BV_GetCenter(pst_SingleBV), OBJ_pst_GetAbsolutePosition(_pst_GO));

	st_Sphere.f_Radius = OBJ_f_BV_GetRadius(pst_SingleBV);

	/* We want to find if the Sphere is behind the ray */

/*$off*/
	st_SpherePoint.x = fAdd(st_Sphere.st_Center.x, fSupZero(_pst_Direction->x) ? st_Sphere.f_Radius : -st_Sphere.f_Radius);
	st_SpherePoint.y = fAdd(st_Sphere.st_Center.y, fSupZero(_pst_Direction->x) ? st_Sphere.f_Radius : -st_Sphere.f_Radius);
	st_SpherePoint.z = fAdd(st_Sphere.st_Center.z, fSupZero(_pst_Direction->x) ? st_Sphere.f_Radius : -st_Sphere.f_Radius);
/*$on*/

	/*
	 * St_Vect is the vector from the origin of the ray to the Sphere point that is
	 * more likely to be ahead of the origin
	 */
	MATH_SubVector(&st_Vect, &st_SpherePoint, _pst_Origin);

	/* The Sphere is behind, we continue */
	if(fInfZero(MATH_f_DotProduct(&st_Vect, _pst_Direction))) return FALSE;

	return(INT_FullRaySphere(_pst_Origin, _pst_Direction, &st_Sphere, NULL, _f_MinDist, _pf_Dist));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL COL_RayAABBox
(
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	OBJ_tdst_GameObject *_pst_GO,
	float				*_pf_Dist,
	float				_f_MinDist
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	INT_tdst_Box		st_Box;
	MATH_tdst_Vector	st_End;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/


	MATH_AddVector(&st_Box.st_Max, OBJ_pst_BV_GetGMax(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
	MATH_AddVector(&st_Box.st_Min, OBJ_pst_BV_GetGMin(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));

	MATH_MulVector(&st_End, _pst_Direction, _f_MinDist);
	MATH_AddEqualVector(&st_End, _pst_Origin);

	/* X Rejection Test. */
	if(_pst_Direction->x > 0.0f)
	{
		if(_pst_Origin->x > st_Box.st_Max.x)  return FALSE;
		if(st_Box.st_Min.x > st_End.x)  return FALSE;
	}
	else
	{
		if(st_Box.st_Min.x > _pst_Origin->x)  return FALSE;
		if(st_End.x > st_Box.st_Max.x) return FALSE;
	}

	/* Y Rejection Test. */
	if(_pst_Direction->y > 0.0f)
	{
		if(_pst_Origin->y > st_Box.st_Max.y) return FALSE;
		if(st_Box.st_Min.y > st_End.y) return FALSE;
	}
	else
	{
		if(st_Box.st_Min.y > _pst_Origin->y) return FALSE;
		if(st_End.y > st_Box.st_Max.y) return FALSE;
	}

	/* Z Rejection Test. */
	if(_pst_Direction->z > 0.0f)
	{
		if(_pst_Origin->z > st_Box.st_Max.z) return FALSE;
		if(st_Box.st_Min.z > st_End.z) return FALSE;
	}
	else
	{
		if(st_Box.st_Min.z > _pst_Origin->z) return FALSE;
		if(st_End.z > st_Box.st_Max.z) return FALSE;
	}

	if
	(
		((_pst_Origin->x >= st_Box.st_Min.x) && (_pst_Origin->x <= st_Box.st_Max.x))
	&&	((_pst_Origin->y >= st_Box.st_Min.y) && (_pst_Origin->y <= st_Box.st_Max.y))
	&&	((_pst_Origin->z >= st_Box.st_Min.z) && (_pst_Origin->z <= st_Box.st_Max.z))
	)
	{
		if(_pf_Dist)
			*_pf_Dist = -1.0f;

		return TRUE;
	}

	/* The box must be tested */
	return(INT_FullRayAABBox(_pst_Origin, _pst_Direction, &st_Box, NULL, _f_MinDist, _pf_Dist));
}

/*
 =======================================================================================================================
    Aim:    Find the GameObject that is closest from the Origin of the Ray by looking at all the BV.

    Note:   !!! WARNING !!!The direction vector must be normalized.
 =======================================================================================================================
 */
OBJ_tdst_GameObject *COL_RayBV
(
	TAB_tdst_PFtable	*_pst_Table,
	MATH_tdst_Vector	*_pst_Origin,
	MATH_tdst_Vector	*_pst_Direction,
	float				f_MaxDist,
	ULONG				_ul_Filter,
	ULONG				_ul_NoFilter,
	ULONG				_ul_ID,
	BOOL				_b_UseRaySkipFlag
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO, *pst_ClosestGO;
	float				f_ObjDist, f_MinDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Init. */
	pst_ClosestGO = NULL;
	f_MinDist = f_MaxDist;

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if
		(
			(TAB_b_IsAHole(pst_GO))
		||	(_b_UseRaySkipFlag && (OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_RayInsensitive)))
		||	!(OBJ_b_TestFlag(pst_GO, _ul_Filter, _ul_NoFilter, _ul_ID))
		) continue;

		/* If the BV of the object is a sphere. */
		if(OBJ_BV_IsSphere(pst_GO->pst_BV))
		{
			if((COL_RaySphereBV(_pst_Origin, _pst_Direction, pst_GO, &f_ObjDist, f_MinDist)) && (f_ObjDist != -1.0f))
			{
				f_MinDist = f_ObjDist;
				pst_ClosestGO = pst_GO;
			}
		}

		/* Then, it is a AABB */
		else
		{
			if((COL_RayAABBox(_pst_Origin, _pst_Direction, pst_GO, &f_ObjDist, f_MinDist)) && (f_ObjDist != -1.0f))
			{
				f_MinDist = f_ObjDist;
				pst_ClosestGO = pst_GO;
			}
		}
	}

	return pst_ClosestGO;
}

/*$F
 =======================================================================================================================


									COLMAP OK3 RAY CAST


 =======================================================================================================================
 */

void COL_ColMap_OK3_RayCast(OBJ_tdst_GameObject *_pst_GO, COL_tdst_ColMap *_pst_ColMap)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_OK3						*pst_OK3;
	COL_tdst_OK3_Box					*pst_Box;
	INT_tdst_Box						st_AABBox;
	COL_tdst_ElementIndexedTriangles	*pst_Element;
	MATH_tdst_Matrix					st_InvertMatrix, st_Unscaled;
	MATH_tdst_Vector					st_OCS_Dir, st_OCS_Orig, st_OCS_End;
	MATH_tdst_Vector					*pst_OCS_Dir, *pst_OCS_Orig;
	MATH_tdst_Vector					*pst_T1, *pst_T2, *pst_T3;
	MATH_tdst_Vector					st_Norm, st_CollidedPoint;
	COL_tdst_OK3_Element				*pst_OK3_Element;
	COL_tdst_GameMat					*pst_GMat;
	COL_tdst_IndexedTriangles			*pst_CobObj;
	ULONG								ul_Triangles;
	int									i, j, k, l;
	float								f_ObjDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CobObj = _pst_ColMap->dpst_Cob[0]->pst_TriangleCob;
	pst_OK3 = pst_CobObj->pst_OK3;

	/*
	 * Compute Inverse Matrix of GameObject in order to compute Origin and Direction
	 * of the Ray in its coordinate system.
	 */

	/*
	 * --- COLMAP TRIANGLES --- : Dont forget that the scale of GameObject is already
	 * inside the Colmap vertices and so, we need to unscale the matrix first.
	 */
	if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
	{
		MATH_CopyMatrix(&st_Unscaled, _pst_GO->pst_GlobalMatrix);
		MATH_ClearScale(&st_Unscaled, 1);
		MATH_InvertMatrix(&st_InvertMatrix, &st_Unscaled);
	}
	else
		MATH_InvertMatrix(&st_InvertMatrix, _pst_GO->pst_GlobalMatrix);

	/* Compute Origin and Direction of the Ray in GameObject Coordinate System */
	if(MATH_b_TestIdentityType(_pst_GO->pst_GlobalMatrix))
	{
		pst_OCS_Dir = RAY_ACCESS(pst_GCS_Dir);
		MATH_SubVector(&st_OCS_Orig, RAY_ACCESS(pst_GCS_Origin), &_pst_GO->pst_GlobalMatrix->T);
		pst_OCS_Orig = &st_OCS_Orig;
	}
	else
	{
		MATH_TransformVertex(&st_OCS_Orig, &st_InvertMatrix, RAY_ACCESS(pst_GCS_Origin));
		MATH_TransformVector(&st_OCS_Dir, &st_InvertMatrix, RAY_ACCESS(pst_GCS_Dir));

#ifdef JADEFUSION
		if( MATH_b_NulVector(&st_OCS_Dir) )
        {
            return;
        }
#endif
		MATH_NormalizeEqualVector(&st_OCS_Dir);
		pst_OCS_Dir = &st_OCS_Dir;
		pst_OCS_Orig = &st_OCS_Orig;
	}

	for(i = 0; i < (int) pst_OK3->ul_NumBox; i++)
	{
		pst_Box = &pst_OK3->pst_OK3_Boxes[i];

		MATH_CopyVector(&st_AABBox.st_Min, &pst_Box->st_Min);
		MATH_CopyVector(&st_AABBox.st_Max, &pst_Box->st_Max);

		/* Quick Rejection X */
		if((RAY_ACCESS(pst_GCS_Dir)->x > 0.0f) && (RAY_ACCESS(pst_GCS_Origin)->x > pst_Box->st_Max.x))
			continue;

		if((RAY_ACCESS(pst_GCS_Dir)->x < 0.0f) && (RAY_ACCESS(pst_GCS_Origin)->x < pst_Box->st_Min.x))
			continue;

		/* Quick Rejection Y */
		if((RAY_ACCESS(pst_GCS_Dir)->y > 0.0f) && (RAY_ACCESS(pst_GCS_Origin)->y > pst_Box->st_Max.y))
			continue;

		if((RAY_ACCESS(pst_GCS_Dir)->y < 0.0f) && (RAY_ACCESS(pst_GCS_Origin)->y < pst_Box->st_Min.y))
			continue;

		/* Quick Rejection Z */
		if((RAY_ACCESS(pst_GCS_Dir)->z > 0.0f) && (RAY_ACCESS(pst_GCS_Origin)->z > pst_Box->st_Max.z))
			continue;

		if((RAY_ACCESS(pst_GCS_Dir)->z < 0.0f) && (RAY_ACCESS(pst_GCS_Origin)->z < pst_Box->st_Min.z))
			continue;


		if(!INT_FullRayAABBox(pst_OCS_Orig, pst_OCS_Dir, &st_AABBox, NULL, RAY_ACCESS(f_GCS_CurrentDist), NULL))
			continue;

		for(j = 0; j < (int) pst_Box->ul_NumElement; j++)
		{
			pst_OK3_Element = &pst_Box->pst_OK3_Element[j];

			ul_Triangles = 0;
			for(l = 0; l < (int) pst_OK3_Element->uw_Element; l++)
			{
				ul_Triangles += pst_CobObj->dst_Element[l].uw_NbTriangles;
			}

			pst_Element = &pst_CobObj->dst_Element[pst_OK3_Element->uw_Element];
			pst_GMat = COL_pst_GMat_Get(_pst_ColMap->dpst_Cob[0], pst_Element);;

			/* We may want to skip Crossable Elements. */
			if(pst_GMat && (RAY_ACCESS(b_SkipFlagX) || RAY_ACCESS(b_SkipCrossable) || RAY_ACCESS(b_SkipCamera)))
			{
				/*~~~~~~~~~~~~~~~~~*/
				USHORT	uw_Crossable;
				/*~~~~~~~~~~~~~~~~~*/

				if(RAY_ACCESS(b_UserCrossable))
					uw_Crossable = COL_guw_UserCrossable;
				else
				{
					if((RAY_ACCESS(pst_GMat_GO)) &&	OBJ_b_TestIdentityFlag(RAY_ACCESS(pst_GMat_GO), OBJ_C_IdentityFlag_ZDM))
						uw_Crossable = ((COL_tdst_Base *) (RAY_ACCESS(pst_GMat_GO))->pst_Extended->pst_Col)->pst_Instance->uw_Crossable;
					else
						uw_Crossable = COL_Cul_DefaultCross;
				}

				if(RAY_ACCESS(b_SkipCrossable) && (((USHORT) pst_GMat->ul_CustomBits) & uw_Crossable))
					continue;

				if(RAY_ACCESS(b_SkipCamera) && ((pst_GMat->ul_CustomBits) & COL_Cul_GMat_Camera))
					continue;

				if(RAY_ACCESS(b_SkipFlagX) && ((pst_GMat->ul_CustomBits) & COL_Cul_GMat_FlagX)) 
					continue;
			}

			/* Sound Filter ?? */
			if(pst_GMat && (COL_gul_SoundFilter != -1) && (COL_gul_SoundFilter != (ULONG) pst_GMat->uc_Sound))
				continue;


			for(k = 0; k < pst_OK3_Element->uw_NumTriangle; k++)
			{
				/*$off*/
				pst_T1 = &(pst_CobObj->dst_Point[pst_CobObj->dst_Element[pst_OK3_Element->uw_Element].dst_Triangle[pst_OK3_Element->puw_OK3_Triangle[k]].auw_Index[0]]);
				pst_T2 = &(pst_CobObj->dst_Point[pst_CobObj->dst_Element[pst_OK3_Element->uw_Element].dst_Triangle[pst_OK3_Element->puw_OK3_Triangle[k]].auw_Index[1]]);
				pst_T3 = &(pst_CobObj->dst_Point[pst_CobObj->dst_Element[pst_OK3_Element->uw_Element].dst_Triangle[pst_OK3_Element->puw_OK3_Triangle[k]].auw_Index[2]]);
				/*$on*/
				MATH_CopyVector
					(
						&st_Norm,
						pst_CobObj->dst_FaceNormal + ul_Triangles + pst_OK3_Element->puw_OK3_Triangle[k]
					);

				/* BackFace culling. */
				if(fSupZero(MATH_f_DotProduct(&st_Norm, pst_OCS_Dir))) continue;

				/* Point at End of the Ray. */
				MATH_MulVector(&st_OCS_End, pst_OCS_Dir, RAY_ACCESS(f_GCS_CurrentDist));
				MATH_AddEqualVector(&st_OCS_End, pst_OCS_Orig);

				/* X Rejection Test. */
				if(pst_OCS_Dir->x > 0.0f)
				{
					if(pst_OCS_Orig->x > fMax3(pst_T1->x, pst_T2->x, pst_T3->x)) continue;
					if(fMin3(pst_T1->x, pst_T2->x, pst_T3->x) > st_OCS_End.x) continue;
				}
				else
				{
					if(fMin3(pst_T1->x, pst_T2->x, pst_T3->x) > pst_OCS_Orig->x) continue;
					if(st_OCS_End.x > fMax3(pst_T1->x, pst_T2->x, pst_T3->x)) continue;
				}

				/* Y Rejection Test. */
				if(pst_OCS_Dir->y > 0.0f)
				{
					if(st_OCS_Orig.y > fMax3(pst_T1->y, pst_T2->y, pst_T3->y)) continue;
					if(fMin3(pst_T1->y, pst_T2->y, pst_T3->y) > st_OCS_End.y) continue;
				}
				else
				{
					if(fMin3(pst_T1->y, pst_T2->y, pst_T3->y) > st_OCS_Orig.y) continue;
					if(st_OCS_End.y > fMax3(pst_T1->y, pst_T2->y, pst_T3->y)) continue;
				}

				/* Z Rejection Test. */
				if(pst_OCS_Dir->z > 0.0f)
				{
					if(pst_OCS_Orig->z > fMax3(pst_T1->z, pst_T2->z, pst_T3->z)) continue;
					if(fMin3(pst_T1->z, pst_T2->z, pst_T3->z) > st_OCS_End.z) continue;
				}
				else
				{
					if(fMin3(pst_T1->z, pst_T2->z, pst_T3->z) > pst_OCS_Orig->z) continue;
					if(st_OCS_End.z > fMax3(pst_T1->z, pst_T2->z, pst_T3->z)) continue;
				}

				if
				(
					INT_FullRayTriangle
					(
						pst_OCS_Orig,
						pst_OCS_Dir,
						pst_T1,
						pst_T2,
						pst_T3,
						&f_ObjDist,
						&st_CollidedPoint,
						FALSE
					)
				)
				{
					if(f_ObjDist < RAY_ACCESS(f_GCS_CurrentDist))
					{
						if(!COL_gb_UseMinDistance || (COL_gb_UseMinDistance && (f_ObjDist > COL_gf_MinDistance)))
						{
							if(RAY_ACCESS(pb_Visible))
							{
								*(RAY_ACCESS(pb_Visible)) = FALSE;
								return;
							}

							/* Fills the World's RayInfo structure to be able to use it later if needed. */
#ifdef ACTIVE_EDITORS
							MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Origin, RAY_ACCESS(pst_GCS_Origin));
#endif
							MATH_TransformVertexNoScale
							(
								&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint,
								_pst_GO->pst_GlobalMatrix,
								&st_CollidedPoint
							);
							MATH_TransformVectorNoScale
							(
								&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal,
								_pst_GO->pst_GlobalMatrix,
								&st_Norm
							);

							MATH_NormalizeEqualVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal);
							(RAY_ACCESS(pst_World))->st_RayInfo.uw_Element = pst_OK3_Element->uw_Element;
							(RAY_ACCESS(pst_World))->st_RayInfo.ul_Triangle = pst_OK3_Element->puw_OK3_Triangle[k];
							(RAY_ACCESS(pst_World))->st_RayInfo.ul_TriangleIndex = pst_OK3_Element->puw_OK3_Triangle[k] + ul_Triangles;
							(RAY_ACCESS(pst_World))->st_RayInfo.pst_GMat = pst_GMat;
							(RAY_ACCESS(pst_World))->st_RayInfo.uc_Design = pst_CobObj->dst_Element[pst_OK3_Element->uw_Element].uc_Design;
							(RAY_ACCESS(pst_World))->st_RayInfo.pst_CollidedGO = _pst_GO;

							if(pst_GMat)
								(RAY_ACCESS(pst_World))->st_RayInfo.ul_Sound = pst_GMat->uc_Sound;


							/* Update min Distance. */
							RAY_ACCESS(f_GCS_CurrentDist) = f_ObjDist;
							RAY_ACCESS(pst_ClosestGO) = _pst_GO;
						}
					}
				}
			}
		}
	}
}

/*$F
 =======================================================================================================================


									COLMAP TRIANGLES RAY CAST


 =======================================================================================================================
 */

void COL_ColMap_Triangles_RayCast(OBJ_tdst_GameObject *_pst_GO, COL_tdst_ColMap *_pst_ColMap)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	COL_tdst_IndexedTriangle			*pst_Triangle, *pst_LastTriangle;
	MATH_tdst_Matrix					st_InvertMatrix, st_Unscaled;
	MATH_tdst_Vector					st_OCS_Dir, st_OCS_Orig, st_OCS_End;
	MATH_tdst_Vector					*pst_OCS_Dir, *pst_OCS_Orig;
	MATH_tdst_Vector					*pst_T1, *pst_T2, *pst_T3;
	MATH_tdst_Vector					st_Norm, st_CollidedPoint;
	COL_tdst_IndexedTriangles			*pst_CobObj;
	ULONG								ul_Element, ul_Triangle, ul_TotalTriangle;
	float								f_ObjDist;
	COL_tdst_GameMat					*pst_GMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CobObj = _pst_ColMap->dpst_Cob[0]->pst_TriangleCob;

	/*
	 * Compute Inverse Matrix of GameObject in order to compute Origin and Direction
	 * of the Ray in its coordinate system.
	 */

	/*
	 * --- COLMAP TRIANGLES --- : Dont forget that the scale of GameObject is already
	 * inside the Colmap vertices and so, we need to unscale the matrix first.
	 */
	if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
	{
		MATH_CopyMatrix(&st_Unscaled, _pst_GO->pst_GlobalMatrix);
		MATH_ClearScale(&st_Unscaled, 1);
		MATH_InvertMatrix(&st_InvertMatrix, &st_Unscaled);
	}
	else
		MATH_InvertMatrix(&st_InvertMatrix, _pst_GO->pst_GlobalMatrix);

	pst_Element = pst_CobObj->dst_Element;
	pst_LastElement = pst_Element + pst_CobObj->l_NbElements;

	/* Compute Origin and Direction of the Ray in GameObject Coordinate System */
	if(MATH_b_TestIdentityType(_pst_GO->pst_GlobalMatrix))
	{
		pst_OCS_Dir = RAY_ACCESS(pst_GCS_Dir);
		MATH_SubVector(&st_OCS_Orig, RAY_ACCESS(pst_GCS_Origin), &_pst_GO->pst_GlobalMatrix->T);
		pst_OCS_Orig = &st_OCS_Orig;
	}
	else
	{
		MATH_TransformVertex(&st_OCS_Orig, &st_InvertMatrix, RAY_ACCESS(pst_GCS_Origin));
		MATH_TransformVector(&st_OCS_Dir, &st_InvertMatrix, RAY_ACCESS(pst_GCS_Dir));

		MATH_NormalizeEqualVector(&st_OCS_Dir);
		pst_OCS_Dir = &st_OCS_Dir;
		pst_OCS_Orig = &st_OCS_Orig;
	}

	for(ul_TotalTriangle = 0, ul_Element = 0; pst_Element < pst_LastElement; pst_Element++, ul_Element++)
	{
		pst_GMat = COL_pst_GMat_Get(_pst_ColMap->dpst_Cob[0], pst_Element);

		/* We may want to skip Crossable Elements. */
		if(pst_GMat && (RAY_ACCESS(b_SkipFlagX) || RAY_ACCESS(b_SkipCrossable) || RAY_ACCESS(b_SkipCamera)))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			USHORT				uw_Crossable;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(RAY_ACCESS(b_UserCrossable))
				uw_Crossable = COL_guw_UserCrossable;
			else
			{
				if((RAY_ACCESS(pst_GMat_GO)) && OBJ_b_TestIdentityFlag(RAY_ACCESS(pst_GMat_GO), OBJ_C_IdentityFlag_ZDM))
					uw_Crossable = ((COL_tdst_Base *) (RAY_ACCESS(pst_GMat_GO))->pst_Extended->pst_Col)->pst_Instance->uw_Crossable;
				else
					uw_Crossable = COL_Cul_DefaultCross;
			}

			if(RAY_ACCESS(b_SkipCrossable) && (((USHORT) pst_GMat->ul_CustomBits) & uw_Crossable))
			{
				ul_TotalTriangle += pst_Element->uw_NbTriangles;
				continue;
			}

			if(RAY_ACCESS(b_SkipCamera) && ((pst_GMat->ul_CustomBits) & COL_Cul_GMat_Camera))
			{
				ul_TotalTriangle += pst_Element->uw_NbTriangles;
				continue;
			}

			if(RAY_ACCESS(b_SkipFlagX) && ((pst_GMat->ul_CustomBits) & COL_Cul_GMat_FlagX))
			{
				ul_TotalTriangle += pst_Element->uw_NbTriangles;
				continue;
			}
		}

		if(pst_GMat && (COL_gul_SoundFilter != -1) && (COL_gul_SoundFilter != (ULONG) pst_GMat->uc_Sound))
		{
			ul_TotalTriangle += pst_Element->uw_NbTriangles;
			continue;
		}

		pst_Triangle = pst_Element->dst_Triangle;
		pst_LastTriangle = pst_Triangle + pst_Element->uw_NbTriangles;
		for(ul_Triangle = 0; pst_Triangle < pst_LastTriangle; pst_Triangle++, ul_TotalTriangle++, ul_Triangle++)
		{
			MATH_CopyVector(&st_Norm, pst_CobObj->dst_FaceNormal + ul_TotalTriangle);

			/* We get the triangles points. */
			pst_T1 = &pst_CobObj->dst_Point[pst_Triangle->auw_Index[0]];
			pst_T2 = &pst_CobObj->dst_Point[pst_Triangle->auw_Index[1]];
			pst_T3 = &pst_CobObj->dst_Point[pst_Triangle->auw_Index[2]];

			/* BackFace culling. */
			if(fSupZero(MATH_f_DotProduct(&st_Norm, pst_OCS_Dir))) continue;

			/* Point at End of the Ray. */
			MATH_MulVector(&st_OCS_End, pst_OCS_Dir, RAY_ACCESS(f_GCS_CurrentDist));
			MATH_AddEqualVector(&st_OCS_End, pst_OCS_Orig);

			/* X Rejection Test. */
			if(pst_OCS_Dir->x > 0.0f)
			{
				if(pst_OCS_Orig->x > fMax3(pst_T1->x, pst_T2->x, pst_T3->x)) continue;
				if(fMin3(pst_T1->x, pst_T2->x, pst_T3->x) > st_OCS_End.x) continue;
			}
			else
			{
				if(fMin3(pst_T1->x, pst_T2->x, pst_T3->x) > pst_OCS_Orig->x) continue;
				if(st_OCS_End.x > fMax3(pst_T1->x, pst_T2->x, pst_T3->x)) continue;
			}

			/* Y Rejection Test. */
			if(pst_OCS_Dir->y > 0.0f)
			{
				if(st_OCS_Orig.y > fMax3(pst_T1->y, pst_T2->y, pst_T3->y)) continue;
				if(fMin3(pst_T1->y, pst_T2->y, pst_T3->y) > st_OCS_End.y) continue;
			}
			else
			{
				if(fMin3(pst_T1->y, pst_T2->y, pst_T3->y) > st_OCS_Orig.y) continue;
				if(st_OCS_End.y > fMax3(pst_T1->y, pst_T2->y, pst_T3->y)) continue;
			}

			/* Z Rejection Test. */
			if(pst_OCS_Dir->z > 0.0f)
			{
				if(pst_OCS_Orig->z > fMax3(pst_T1->z, pst_T2->z, pst_T3->z)) continue;
				if(fMin3(pst_T1->z, pst_T2->z, pst_T3->z) > st_OCS_End.z) continue;
			}
			else
			{
				if(fMin3(pst_T1->z, pst_T2->z, pst_T3->z) > pst_OCS_Orig->z) continue;
				if(st_OCS_End.z > fMax3(pst_T1->z, pst_T2->z, pst_T3->z)) continue;
			}

			if
			(
				INT_FullRayTriangle
				(
					pst_OCS_Orig,
					pst_OCS_Dir,
					pst_T1,
					pst_T2,
					pst_T3,
					&f_ObjDist,
					&st_CollidedPoint,
					FALSE
				)
			)
			{
				if(f_ObjDist < RAY_ACCESS(f_GCS_CurrentDist))
				{
					if(!COL_gb_UseMinDistance || (COL_gb_UseMinDistance && (f_ObjDist > COL_gf_MinDistance)))
					{
						if(RAY_ACCESS(pb_Visible))
						{
							*(RAY_ACCESS(pb_Visible)) = FALSE;
							return;
						}

						/* Fills the World's RayInfo structure to be able to use it later if needed. */
#ifdef ACTIVE_EDITORS
						MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Origin, RAY_ACCESS(pst_GCS_Origin));
#endif
						MATH_TransformVertexNoScale
						(
							&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint,
							_pst_GO->pst_GlobalMatrix,
							&st_CollidedPoint
						);
						MATH_TransformVectorNoScale
						(
							&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal,
							_pst_GO->pst_GlobalMatrix,
							&st_Norm
						);

						MATH_NormalizeEqualVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal);
						(RAY_ACCESS(pst_World))->st_RayInfo.uw_Element = (USHORT) ul_Element;
						(RAY_ACCESS(pst_World))->st_RayInfo.ul_Triangle = ul_Triangle;
						(RAY_ACCESS(pst_World))->st_RayInfo.ul_TriangleIndex = ul_TotalTriangle;
						(RAY_ACCESS(pst_World))->st_RayInfo.pst_GMat = pst_GMat;

						(RAY_ACCESS(pst_World))->st_RayInfo.uc_Design = pst_Element->uc_Design;
						(RAY_ACCESS(pst_World))->st_RayInfo.pst_CollidedGO = _pst_GO;

						if(pst_GMat)
							(RAY_ACCESS(pst_World))->st_RayInfo.ul_Sound = pst_GMat->uc_Sound;

						/* Update Closest GameObject */
						RAY_ACCESS(pst_ClosestGO) = _pst_GO;
						RAY_ACCESS(f_GCS_CurrentDist) = f_ObjDist;
					}
				}
			}	
		}
	}
}

/*$F
 =======================================================================================================================


									COLMAP SPHERE RAY CAST


 =======================================================================================================================
 */

BOOL COL_ColMap_Sphere_RayCast(OBJ_tdst_GameObject *_pst_GO, COL_tdst_Mathematical *_pst_MathCob, BOOL _b_CallFromVisual)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_CollidedPoint;
	INT_tdst_Sphere		st_GCS_Sphere;
	float				f_OCS_Radius, f_GCS_Radius;
	float				f_ObjDist, f_MaxScale;
	MATH_tdst_Vector	st_Scale;
	MATH_tdst_Vector	st_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_TransformVertex
	(
		&st_GCS_Sphere.st_Center,
		OBJ_pst_GetAbsoluteMatrix(_pst_GO),
		COL_pst_Shape_GetCenter(_pst_MathCob->p_Shape)
	);

	/* If GO has scale, we have to recompute radius ray due to scale. */
	if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
	{
		MATH_GetScale(&st_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
		f_OCS_Radius = COL_f_Shape_GetRadius(_pst_MathCob->p_Shape);
		f_MaxScale = fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
		f_GCS_Radius = f_MaxScale * f_OCS_Radius;
	}
	else
		f_GCS_Radius = COL_f_Shape_GetRadius(_pst_MathCob->p_Shape);

	st_GCS_Sphere.f_Radius = f_GCS_Radius;

	if
	(
		!INT_FullRaySphere
		(
			RAY_ACCESS(pst_GCS_Origin),
			RAY_ACCESS(pst_GCS_Dir),
			&st_GCS_Sphere,
			&st_CollidedPoint,
			RAY_ACCESS(f_GCS_CurrentDist),
			&f_ObjDist
		)
	) 
		return FALSE;

	if(!COL_gb_UseMinDistance || (COL_gb_UseMinDistance && (f_ObjDist > COL_gf_MinDistance)))
	{
		if(RAY_ACCESS(pb_Visible))
		{
			*(RAY_ACCESS(pb_Visible)) = FALSE;
			return FALSE;
		}

		if(!_b_CallFromVisual || !(RAY_ACCESS(b_UseBoneOBBox)))
		{
			/* Fills the World's RayInfo structure to be able to use it later if needed. */
#ifdef ACTIVE_EDITORS
			MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Origin, RAY_ACCESS(pst_GCS_Origin));
#endif
			MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint, &st_CollidedPoint);
			MATH_SubVector(&st_Temp, &st_CollidedPoint, &st_GCS_Sphere.st_Center);
			MATH_NormalizeVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal, &st_Temp);
			(RAY_ACCESS(pst_World))->st_RayInfo.uw_Element = (USHORT) - 1;
			(RAY_ACCESS(pst_World))->st_RayInfo.ul_Triangle = (ULONG) - 1;
			(RAY_ACCESS(pst_World))->st_RayInfo.ul_TriangleIndex = (ULONG) - 1;
			(RAY_ACCESS(pst_World))->st_RayInfo.pst_GMat = NULL;
			(RAY_ACCESS(pst_World))->st_RayInfo.uc_Design = 0;
			(RAY_ACCESS(pst_World))->st_RayInfo.pst_CollidedGO = _pst_GO;

			/* Update Closest GameObject */
			RAY_ACCESS(pst_ClosestGO) = _pst_GO;
			RAY_ACCESS(f_GCS_CurrentDist) = f_ObjDist;
		}
		return TRUE;
	}

	return FALSE;
}

/*$F
 =======================================================================================================================


									COLMAP BOX RAY CAST


 =======================================================================================================================
 */

BOOL COL_ColMap_Box_RayCast(OBJ_tdst_GameObject *_pst_GO, COL_tdst_Mathematical *_pst_MathCob, BOOL _b_CallFromVisual)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_33_InvertMatrix, st_33_GlobalMatrix;
	MATH_tdst_Vector	st_CollidedPoint;
	MATH_tdst_Vector	st_OCS_Orig, st_OCS_Dir;
	MATH_tdst_Vector	st_GCS_Ray, st_OCS_Ray;
	MATH_tdst_Vector	st_Norm;
	INT_tdst_Box		st_Box;
	ULONG				ul_ClosestPlan;
	float				f_ObjDist, f_OCS_CurrentDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Creates a 3x3 matrix containing the possible scale of the object */
	MATH_MakeOGLMatrix(&st_33_GlobalMatrix, _pst_GO->pst_GlobalMatrix);
	MATH_ClearScale(&st_33_GlobalMatrix, 1);

	/* Inverts the 3x3 matrix */
	MATH_Invert33Matrix(&st_33_InvertMatrix, &st_33_GlobalMatrix);
	MATH_ClearScale(&st_33_InvertMatrix, 1);
	MATH_NegVector(MATH_pst_GetTranslation(&st_33_InvertMatrix), MATH_pst_GetTranslation(_pst_GO->pst_GlobalMatrix));
	MATH_TransformVector(&st_33_InvertMatrix.T, &st_33_InvertMatrix, &st_33_InvertMatrix.T);

	MATH_TransformVertex(&st_OCS_Orig, &st_33_InvertMatrix, RAY_ACCESS(pst_GCS_Origin));
	MATH_TransformVector(&st_OCS_Dir, &st_33_InvertMatrix, RAY_ACCESS(pst_GCS_Dir));

	MATH_NormalizeEqualVector(&st_OCS_Dir);

	MATH_CopyVector(&st_Box.st_Max, COL_pst_Shape_GetMax(_pst_MathCob->p_Shape));
	MATH_CopyVector(&st_Box.st_Min, COL_pst_Shape_GetMin(_pst_MathCob->p_Shape));

	/* If ColMap GO has scale, we have to recompute min value for ray due to scale. */
	if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
	{
		MATH_ScaleVector(&st_GCS_Ray, RAY_ACCESS(pst_GCS_Dir), RAY_ACCESS(f_GCS_CurrentDist));
		MATH_TransformVector(&st_OCS_Ray, &st_33_InvertMatrix, &st_GCS_Ray);
		f_OCS_CurrentDist = MATH_f_NormVector(&st_OCS_Ray);
	}
	else
		f_OCS_CurrentDist = RAY_ACCESS(f_GCS_CurrentDist);

	ul_ClosestPlan = INT_FullRayAABBox
		(
			&st_OCS_Orig,
			&st_OCS_Dir,
			&st_Box,
			&st_CollidedPoint,
			f_OCS_CurrentDist,
			&f_ObjDist
		);

	if(!ul_ClosestPlan) return FALSE;

	/* If ColMap GO has scale, we have to recompute min value for ray due to scale. */
	if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
	{
		MATH_ScaleVector(&st_OCS_Ray, RAY_ACCESS(pst_GCS_Dir), f_ObjDist);
		MATH_TransformVector(&st_GCS_Ray, &st_33_GlobalMatrix, &st_OCS_Ray);
		f_ObjDist = MATH_f_NormVector(&st_GCS_Ray);
	}

	if(!COL_gb_UseMinDistance || (COL_gb_UseMinDistance && (f_ObjDist > COL_gf_MinDistance)))
	{
		if(RAY_ACCESS(pb_Visible))
		{
			*(RAY_ACCESS(pb_Visible)) = FALSE;
			return FALSE;
		}

		if(!_b_CallFromVisual || !(RAY_ACCESS(b_UseBoneOBBox)))
		{
			/* Fills the World's RayInfo structure to be able to use it later if needed. */
#ifdef ACTIVE_EDITORS
			MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Origin, RAY_ACCESS(pst_GCS_Origin));
#endif
			MATH_TransformVertex(&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint, &st_33_GlobalMatrix, &st_CollidedPoint);

			switch(ul_ClosestPlan)
			{
			case 1:
				if(st_OCS_Dir.x > 0.0f)
					MATH_InitVector(&st_Norm, -1.0f, 0.0f, 0.0f);
				else
					MATH_InitVector(&st_Norm, 1.0f, 0.0f, 0.0f);
				break;

			case 2:
				if(st_OCS_Dir.y > 0.0f)
					MATH_InitVector(&st_Norm, 0.0f, -1.0f, 0.0f);
				else
					MATH_InitVector(&st_Norm, 0.0f, 1.0f, 0.0f);
				break;

			case 3:
				if(st_OCS_Dir.z > 0.0f)
					MATH_InitVector(&st_Norm, 0.0f, 0.0f, -1.0f);
				else
					MATH_InitVector(&st_Norm, 0.0f, 0.0f, 1.0f);
				break;
			}

			MATH_TransformVertex(&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint, &st_33_GlobalMatrix, &st_CollidedPoint);
			MATH_TransformVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal, &st_33_GlobalMatrix, &st_Norm);
			MATH_NormalizeEqualVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal);
			(RAY_ACCESS(pst_World))->st_RayInfo.uw_Element = (USHORT) - 1;
			(RAY_ACCESS(pst_World))->st_RayInfo.ul_Triangle = (ULONG) - 1;
			(RAY_ACCESS(pst_World))->st_RayInfo.ul_TriangleIndex = (ULONG) - 1;
			(RAY_ACCESS(pst_World))->st_RayInfo.pst_GMat = NULL;
			(RAY_ACCESS(pst_World))->st_RayInfo.uc_Design = 0;
			(RAY_ACCESS(pst_World))->st_RayInfo.pst_CollidedGO = _pst_GO;


			/* Update Closest GameObject */
			RAY_ACCESS(pst_ClosestGO) = _pst_GO;
			RAY_ACCESS(f_GCS_CurrentDist) = f_ObjDist;
		}
		return TRUE;
	}

	return FALSE;
}

/*$F
 =======================================================================================================================


									COLMAP CYLINDER RAY CAST


 =======================================================================================================================
 */

BOOL COL_ColMap_Cylinder_RayCast(OBJ_tdst_GameObject *_pst_GO, COL_tdst_Mathematical *_pst_MathCob, BOOL _b_CallFromVisual)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_CollidedPoint;
	MATH_tdst_Vector	st_OCS_Center, st_Scale;
	float				f_Radius, f_Height;
	float				f_ObjDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_TransformVertex
	(
		&st_OCS_Center,
		OBJ_pst_GetAbsoluteMatrix(_pst_GO),
		COL_pst_Shape_GetCenter(_pst_MathCob->p_Shape)
	);

	MATH_GetScale(&st_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_GO));

	f_Radius = COL_f_Shape_GetRadius(_pst_MathCob->p_Shape) * fMax(st_Scale.x, st_Scale.y);
	f_Height = COL_f_Shape_GetHeight(_pst_MathCob->p_Shape) * st_Scale.z;

	if
	(
		INT_b_FullRayCylinder
		(
			RAY_ACCESS(pst_GCS_Origin),
			RAY_ACCESS(pst_GCS_Dir),
			&st_OCS_Center,
			&MATH_gst_BaseVectorK,
			f_Radius,
			f_Height,
			RAY_ACCESS(f_GCS_CurrentDist),
			&f_ObjDist,
			&st_CollidedPoint
		)
	)
	{
		if(!COL_gb_UseMinDistance || (COL_gb_UseMinDistance && (f_ObjDist > COL_gf_MinDistance)))
		{
			if(RAY_ACCESS(pb_Visible))
			{
				*(RAY_ACCESS(pb_Visible)) = FALSE;
				return FALSE;
			}

			if(!_b_CallFromVisual || !(RAY_ACCESS(b_UseBoneOBBox)))
			{
				/* Fills the World's RayInfo structure to be able to use it later if needed. */
#ifdef ACTIVE_EDITORS
				MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Origin, RAY_ACCESS(pst_GCS_Origin));

#endif
				MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint, &st_CollidedPoint);

				MATH_NegVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal, RAY_ACCESS(pst_GCS_Dir));
				(RAY_ACCESS(pst_World))->st_RayInfo.uw_Element = (USHORT) - 1;
				(RAY_ACCESS(pst_World))->st_RayInfo.ul_Triangle = (ULONG) - 1;
				(RAY_ACCESS(pst_World))->st_RayInfo.ul_TriangleIndex = (ULONG) - 1;
				(RAY_ACCESS(pst_World))->st_RayInfo.pst_GMat = NULL;
				(RAY_ACCESS(pst_World))->st_RayInfo.uc_Design = 0;
				(RAY_ACCESS(pst_World))->st_RayInfo.pst_CollidedGO = _pst_GO;

				/* Update Closest GameObject */
				RAY_ACCESS(pst_ClosestGO) = _pst_GO;
				RAY_ACCESS(f_GCS_CurrentDist) = f_ObjDist;
			}
			return TRUE;
		}
	}

	return FALSE;
}

/*$F
 ===================================================================================================


									VISUAL RAY CAST ON ONE BONE



 ===================================================================================================
 */
BOOL COL_Visual_RayCast_OneBone(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_33_InvertMatrix, st_33_GlobalMatrix;
	MATH_tdst_Matrix	st_test;
	MATH_tdst_Vector	st_CollidedPoint;
	MATH_tdst_Vector	st_OCS_Orig, st_OCS_Dir;
	MATH_tdst_Vector	st_GCS_Ray, st_OCS_Ray;
	MATH_tdst_Vector	st_Norm;
	INT_tdst_Box		st_Box;
	ULONG				ul_ClosestPlan;
	float				f_ObjDist, f_OCS_CurrentDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Creates a 3x3 matrix containing the possible scale of the object */
	MATH_MakeOGLMatrix(&st_33_GlobalMatrix, _pst_GO->pst_GlobalMatrix);
	MATH_ClearScale(&st_33_GlobalMatrix, 1);

	/* Inverts the 3x3 matrix */
	MATH_Invert33Matrix(&st_33_InvertMatrix, &st_33_GlobalMatrix);
	/* test */
	MATH_InvertMatrix(&st_test, &st_33_GlobalMatrix);

	MATH_ClearScale(&st_33_InvertMatrix, 1);
	MATH_NegVector(MATH_pst_GetTranslation(&st_33_InvertMatrix), MATH_pst_GetTranslation(_pst_GO->pst_GlobalMatrix));
	MATH_TransformVector(&st_33_InvertMatrix.T, &st_33_InvertMatrix, &st_33_InvertMatrix.T);

	MATH_TransformVertex(&st_OCS_Orig, &st_33_InvertMatrix, RAY_ACCESS(pst_GCS_Origin));
	MATH_TransformVector(&st_OCS_Dir, &st_33_InvertMatrix, RAY_ACCESS(pst_GCS_Dir));

	MATH_NormalizeEqualVector(&st_OCS_Dir);

	MATH_CopyVector(&st_Box.st_Max, OBJ_pst_BV_GetLMax(_pst_GO->pst_BV));
	MATH_CopyVector(&st_Box.st_Min, OBJ_pst_BV_GetLMin(_pst_GO->pst_BV));

	/* If ColMap GO has scale, we have to recompute min value for ray due to scale. */
	if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
	{
		MATH_ScaleVector(&st_GCS_Ray, RAY_ACCESS(pst_GCS_Dir), RAY_ACCESS(f_GCS_CurrentDist));
		MATH_TransformVector(&st_OCS_Ray, &st_33_InvertMatrix, &st_GCS_Ray);
		f_OCS_CurrentDist = MATH_f_NormVector(&st_OCS_Ray);
	}
	else
		f_OCS_CurrentDist = RAY_ACCESS(f_GCS_CurrentDist);

	/* Quick Rejection X */
	if((st_OCS_Dir.x > 0.0f) && (st_OCS_Orig.x > st_Box.st_Max.x))
		return FALSE;

	if((st_OCS_Dir.x < 0.0f) && (st_OCS_Orig.x < st_Box.st_Min.x))
		return FALSE;

	/* Quick Rejection Y */
	if((st_OCS_Dir.y > 0.0f) && (st_OCS_Orig.y > st_Box.st_Max.y))
		return FALSE;

	if((st_OCS_Dir.y < 0.0f) && (st_OCS_Orig.y < st_Box.st_Min.y))
		return FALSE;

	/* Quick Rejection Z */
	if((st_OCS_Dir.z > 0.0f) && (st_OCS_Orig.z > st_Box.st_Max.z))
		return FALSE;

	if((st_OCS_Dir.z < 0.0f) && (st_OCS_Orig.z < st_Box.st_Min.z))
		return FALSE;


	ul_ClosestPlan = INT_FullRayAABBox
		(
			&st_OCS_Orig,
			&st_OCS_Dir,
			&st_Box,
			&st_CollidedPoint,
			f_OCS_CurrentDist,
			&f_ObjDist
		);

	if(!ul_ClosestPlan) return FALSE;

	/* If ColMap GO has scale, we have to recompute min value for ray due to scale. */
	if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
	{
		MATH_ScaleVector(&st_OCS_Ray, RAY_ACCESS(pst_GCS_Dir), f_ObjDist);
		MATH_TransformVector(&st_GCS_Ray, &st_33_GlobalMatrix, &st_OCS_Ray);
		f_ObjDist = MATH_f_NormVector(&st_GCS_Ray);
	}

	if(!COL_gb_UseMinDistance || (COL_gb_UseMinDistance && (f_ObjDist > COL_gf_MinDistance)))
	{
		if(RAY_ACCESS(pb_Visible))
		{
			*(RAY_ACCESS(pb_Visible)) = FALSE;
			return FALSE;
		}

		/* Fills the World's RayInfo structure to be able to use it later if needed. */
#ifdef ACTIVE_EDITORS
		MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Origin, RAY_ACCESS(pst_GCS_Origin));
#endif
		MATH_TransformVertex(&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint, &st_33_GlobalMatrix, &st_CollidedPoint);

		switch(ul_ClosestPlan)
		{
		case 1:
			if(st_OCS_Dir.x > 0.0f)
				MATH_InitVector(&st_Norm, -1.0f, 0.0f, 0.0f);
			else
				MATH_InitVector(&st_Norm, 1.0f, 0.0f, 0.0f);
			break;

		case 2:
			if(st_OCS_Dir.y > 0.0f)
				MATH_InitVector(&st_Norm, 0.0f, -1.0f, 0.0f);
			else
				MATH_InitVector(&st_Norm, 0.0f, 1.0f, 0.0f);
			break;

		case 3:
			if(st_OCS_Dir.z > 0.0f)
				MATH_InitVector(&st_Norm, 0.0f, 0.0f, -1.0f);
			else
				MATH_InitVector(&st_Norm, 0.0f, 0.0f, 1.0f);
			break;
		}

		MATH_TransformVertex(&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint, &st_33_GlobalMatrix, &st_CollidedPoint);
		(RAY_ACCESS(pst_World))->st_RayInfo.uw_Element = (USHORT) - 1;
		(RAY_ACCESS(pst_World))->st_RayInfo.ul_Triangle = (ULONG) - 1;
		(RAY_ACCESS(pst_World))->st_RayInfo.ul_TriangleIndex = (ULONG) - 1;
		(RAY_ACCESS(pst_World))->st_RayInfo.pst_GMat = NULL;
		(RAY_ACCESS(pst_World))->st_RayInfo.uc_Design = 0;
		(RAY_ACCESS(pst_World))->st_RayInfo.pst_CollidedGO = _pst_GO;

		(RAY_ACCESS(pst_World))->st_RayInfo.ul_Sound = 0;


		/* Update Closest GameObject */
		RAY_ACCESS(pst_ClosestGO) = _pst_GO;
		RAY_ACCESS(f_GCS_CurrentDist) = f_ObjDist;
		return TRUE;
	}

	return FALSE;
}

/*$F
 ===================================================================================================


									VISUAL RAY CAST ON ONE SKELETON



 ===================================================================================================
 */

void COL_Visual_RayCast_OneSkeleton(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_Group			*pst_Skeleton;
	OBJ_tdst_GameObject		*pst_BoneGO;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	pst_Skeleton = _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
	if (!pst_Skeleton) return;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
	for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));

		/* Skip Bones without OBBox */
		if(!(pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_OBBox))
			continue;

		COL_Visual_RayCast_OneBone(pst_BoneGO);
	}
}

/*$F
 =======================================================================================================================


									COLMAP RAY CAST ONE OBJECT



 =======================================================================================================================
 */
OBJ_tdst_GameObject *COL_ColMap_RayCast_OneObject
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*_pst_Orig,
	MATH_tdst_Vector	*_pst_Dir,
	float				_f_MaxDist,
	ULONG				_ul_Filter,
	ULONG				_ul_NoFilter,
	ULONG				_ul_ID,
	BOOL				*_pb_Visible,
	BOOL				_b_UseRaySkipFlag,
	BOOL				_b_SkipCrossable,
	BOOL				_b_SkipCamera,
	BOOL				_b_SkipFlagX,
	BOOL				_b_UserCrossable,
	BOOL				_b_UseBoneOBBox
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_ObjDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	RAY_ACCESS(pst_World) = WOR_World_GetWorldOfObject(pst_GO);
	RAY_ACCESS(b_UseRaySkipFlag) = _b_UseRaySkipFlag;
	RAY_ACCESS(b_SkipCrossable) = _b_SkipCrossable;
	RAY_ACCESS(b_SkipCamera) = _b_SkipCamera;
	RAY_ACCESS(b_SkipFlagX) = _b_SkipFlagX;
	RAY_ACCESS(b_UserCrossable) = _b_UserCrossable;
	RAY_ACCESS(pst_GCS_Origin) = _pst_Orig;
	RAY_ACCESS(pst_GCS_Dir) = _pst_Dir;
	RAY_ACCESS(f_GCS_CurrentDist) = _f_MaxDist;
	RAY_ACCESS(pb_Visible) = _pb_Visible;
	RAY_ACCESS(pst_ClosestGO) = NULL;
	RAY_ACCESS(b_ReturnLocalData) = FALSE;
	RAY_ACCESS(b_UseBoneOBBox) = _b_UseBoneOBBox;
	RAY_ACCESS(pst_GMat_GO) = pst_GO;

	if(_pb_Visible) *_pb_Visible = TRUE;


		/* If the object is filtered, we continue. */
		if
		(
			(TAB_b_IsAHole(pst_GO))
		||	!(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ColMap))
		||	!(OBJ_b_TestStatusFlag(pst_GO, OBJ_C_StatusFlag_Active))
		||	!(OBJ_b_TestFlag(pst_GO, _ul_Filter, _ul_NoFilter, _ul_ID))
		) return NULL;

		if
		(
			(
				OBJ_BV_IsAABBox(pst_GO->pst_BV)
			&&	(COL_RayAABBox(_pst_Orig, _pst_Dir, pst_GO, &f_ObjDist, RAY_ACCESS(f_GCS_CurrentDist)))
			)
		||	(
				OBJ_BV_IsSphere(pst_GO->pst_BV)
			&&	(COL_RaySphereBV(_pst_Orig, _pst_Dir, pst_GO, &f_ObjDist, RAY_ACCESS(f_GCS_CurrentDist)))
			)
		)
		{
			/* Crossed Objects management */
			if(RAY_ACCESS(b_UseRaySkipFlag) && (OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_RayInsensitive)))
			{
				if((RAY_ACCESS(pst_World))->ul_NbCrossed == COL_Cul_MaxCrossed)
					(RAY_ACCESS(pst_World))->ul_NbCrossed--;
				(RAY_ACCESS(pst_World))->apst_CrossObjects[(RAY_ACCESS(pst_World))->ul_NbCrossed++] = pst_GO;
				return NULL;
			}

			if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ColMap))
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				COL_tdst_ColMap				*pst_ColMap;
				COL_tdst_IndexedTriangles	*pst_CobObj;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_ColMap = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap;

				if(pst_ColMap->dpst_Cob[0]->uc_Type == COL_C_Zone_Triangles)
				{
					if(!COL_b_ColMap_IsActive(pst_ColMap, 0)) return NULL;

					pst_CobObj = pst_ColMap->dpst_Cob[0]->pst_TriangleCob;

					if(pst_CobObj->pst_OK3)
						COL_OK3_RayCast(pst_GO, (void *) pst_ColMap->dpst_Cob[0], TRUE);
					else
						COL_ColMap_Triangles_RayCast(pst_GO, pst_ColMap);

					if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) return NULL;
				}
				else
				{
					/*~~~~~~*/
					UCHAR	i;
					/*~~~~~~*/

					for(i = 0; i < pst_ColMap->uc_NbOfCob; i++)
					{
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
						COL_tdst_Mathematical	*pst_MathCob;
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

						if(!COL_b_ColMap_IsActive(pst_ColMap, i)) continue;
						pst_MathCob = pst_ColMap->dpst_Cob[i]->pst_MathCob;

						/* We may want to skip Crossable Cobs. */
						if(_b_SkipCrossable)
						{
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
							COL_tdst_GameMat	*pst_GMat;
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

							if(_b_UserCrossable)
							{
								pst_GMat = COL_pst_GMat_Get(pst_ColMap->dpst_Cob[i], NULL);

								if(pst_GMat && (((USHORT) pst_GMat->ul_CustomBits) & COL_guw_UserCrossable)) continue;
							}
							else
							{
								if(pst_ColMap->dpst_Cob[i]->pst_GMatList)
								{
									/*~~~~~~~~~~~~~~~~~*/
									USHORT	uw_Crossable;
									/*~~~~~~~~~~~~~~~~~*/

									uw_Crossable = COL_Cul_DefaultCross;

									pst_GMat = COL_pst_GMat_Get(pst_ColMap->dpst_Cob[i], NULL);

									if(pst_GMat && (((USHORT) pst_GMat->ul_CustomBits) & uw_Crossable)) continue;
								}
							}
						}

						switch(pst_ColMap->dpst_Cob[i]->uc_Type)
						{
						case COL_C_Zone_Sphere:
							if(RAY_ACCESS(b_UseBoneOBBox)) // && COL_ColMap_Sphere_RayCast(pst_GO, pst_MathCob, TRUE))
								COL_Visual_RayCast_OneSkeleton(pst_GO);
							else 
								COL_ColMap_Sphere_RayCast(pst_GO, pst_MathCob, FALSE);
							break;

						case COL_C_Zone_Box:
							if(RAY_ACCESS(b_UseBoneOBBox)) // && COL_ColMap_Sphere_RayCast(pst_GO, pst_MathCob, TRUE))
								COL_Visual_RayCast_OneSkeleton(pst_GO);
							else 
								COL_ColMap_Box_RayCast(pst_GO, pst_MathCob, FALSE);
							break;

						case COL_C_Zone_Cylinder:
							if(RAY_ACCESS(b_UseBoneOBBox)) // && COL_ColMap_Sphere_RayCast(pst_GO, pst_MathCob, TRUE))
								COL_Visual_RayCast_OneSkeleton(pst_GO);
							else 
								COL_ColMap_Cylinder_RayCast(pst_GO, pst_MathCob, FALSE);
							break;
						}

						if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) return NULL;

					}
				}
			}
		}

	COL_gb_UseMinDistance = FALSE;
	return(RAY_ACCESS(pst_ClosestGO));
}


/*$F
 =======================================================================================================================


									COLMAP RAY CAST



 =======================================================================================================================
 */

OBJ_tdst_GameObject *COL_ColMap_RayCast
(
	WOR_tdst_World		*_pst_World,
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Orig,
	MATH_tdst_Vector	*_pst_Dir,
	float				_f_MaxDist,
	ULONG				_ul_Filter,
	ULONG				_ul_NoFilter,
	ULONG				_ul_ID,
	BOOL				*_pb_Visible,
	BOOL				_b_UseRaySkipFlag,
	BOOL				_b_SkipCrossable,
	BOOL				_b_SkipCamera,
	BOOL				_b_SkipFlagX,
	BOOL				_b_UserCrossable,
	BOOL				_b_UseBoneOBBox
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFtable	*pst_Table;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
	float				f_ObjDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//	PRO_StartTrameRaster(&ENG_gpst_RasterEng_Events);

	RAY_ACCESS(b_UseRaySkipFlag) = _b_UseRaySkipFlag;
	RAY_ACCESS(b_SkipCrossable) = _b_SkipCrossable;
	RAY_ACCESS(b_SkipCamera) = _b_SkipCamera;
	RAY_ACCESS(b_SkipFlagX) = _b_SkipFlagX;
	RAY_ACCESS(b_UserCrossable) = _b_UserCrossable;
	RAY_ACCESS(pst_World) = _pst_World;
	RAY_ACCESS(pst_GCS_Origin) = _pst_Orig;
	RAY_ACCESS(pst_GCS_Dir) = _pst_Dir;
	RAY_ACCESS(f_GCS_CurrentDist) = _f_MaxDist;
	RAY_ACCESS(pb_Visible) = _pb_Visible;
	RAY_ACCESS(pst_ClosestGO) = NULL;
	RAY_ACCESS(pst_GMat_GO) = _pst_GO;
	RAY_ACCESS(b_ReturnLocalData) = FALSE;
	RAY_ACCESS(b_UseBoneOBBox) = _b_UseBoneOBBox;

	if(_pb_Visible) *_pb_Visible = TRUE;
	_pst_World->ul_NbCrossed = 0;

	pst_Table = &_pst_World->st_EOT.st_ColMap;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Table);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		/* We get the current Game Object. */
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		/* If the object is filtered, we continue. */
		if
		(
			(TAB_b_IsAHole(pst_CurrentGO))
		||	!(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_ColMap))
		||	!(OBJ_b_TestStatusFlag(pst_CurrentGO, OBJ_C_StatusFlag_Active))
		||	!(OBJ_b_TestFlag(pst_CurrentGO, _ul_Filter, _ul_NoFilter, _ul_ID))
		||	(pst_CurrentGO == _pst_GO)
		) 
			continue;

		if
		(
			(
				OBJ_BV_IsAABBox(pst_CurrentGO->pst_BV)
			&&	(COL_RayAABBox(_pst_Orig, _pst_Dir, pst_CurrentGO, &f_ObjDist, RAY_ACCESS(f_GCS_CurrentDist)))
			)
		||	(
				OBJ_BV_IsSphere(pst_CurrentGO->pst_BV)
			&&	(COL_RaySphereBV(_pst_Orig, _pst_Dir, pst_CurrentGO, &f_ObjDist, RAY_ACCESS(f_GCS_CurrentDist)))
			)
		)
		{
			/* Crossed Objects management */
			if(RAY_ACCESS(b_UseRaySkipFlag) && (OBJ_b_TestControlFlag(pst_CurrentGO, OBJ_C_ControlFlag_RayInsensitive)))
			{
				if((RAY_ACCESS(pst_World))->ul_NbCrossed == COL_Cul_MaxCrossed)
					(RAY_ACCESS(pst_World))->ul_NbCrossed--;
				(RAY_ACCESS(pst_World))->apst_CrossObjects[(RAY_ACCESS(pst_World))->ul_NbCrossed++] = pst_CurrentGO;
				continue;
			}

			if(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_ColMap))
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				COL_tdst_ColMap				*pst_ColMap;
				COL_tdst_IndexedTriangles	*pst_CobObj;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_ColMap = ((COL_tdst_Base *) pst_CurrentGO->pst_Extended->pst_Col)->pst_ColMap;

				if(pst_ColMap->dpst_Cob[0]->uc_Type == COL_C_Zone_Triangles)
				{
					if(!COL_b_ColMap_IsActive(pst_ColMap, 0)) continue;

					pst_CobObj = pst_ColMap->dpst_Cob[0]->pst_TriangleCob;

					if(pst_CobObj->pst_OK3)
						COL_OK3_RayCast(pst_CurrentGO, (void *) pst_ColMap->dpst_Cob[0], TRUE);
					else
						COL_ColMap_Triangles_RayCast(pst_CurrentGO, pst_ColMap);

					if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) 
					{
//						PRO_StopTrameRaster(&ENG_gpst_RasterEng_Events);
						return FALSE;
					}
				}
				else
				{
					/*~~~~~~*/
					UCHAR	i;
					/*~~~~~~*/

					for(i = 0; i < pst_ColMap->uc_NbOfCob; i++)
					{
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
						COL_tdst_Mathematical	*pst_MathCob;
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

						if(!COL_b_ColMap_IsActive(pst_ColMap, i)) continue;

						if(COL_gul_SoundFilter != -1) 
							continue;

						pst_MathCob = pst_ColMap->dpst_Cob[i]->pst_MathCob;

						/* We may want to skip Crossable Cobs. */
						if(_b_SkipCrossable)
						{
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
							COL_tdst_GameMat	*pst_GMat;
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

							if(_b_UserCrossable)
							{
								pst_GMat = COL_pst_GMat_Get(pst_ColMap->dpst_Cob[i], NULL);

								if(pst_GMat && (((USHORT) pst_GMat->ul_CustomBits) & COL_guw_UserCrossable)) continue;
							}
							else
							{
								if(pst_ColMap->dpst_Cob[i]->pst_GMatList)
								{
									/*~~~~~~~~~~~~~~~~~*/
									USHORT	uw_Crossable;
									/*~~~~~~~~~~~~~~~~~*/

									if(_pst_GO && OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM))
										uw_Crossable =
											((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)
												->pst_Instance->uw_Crossable;
									else
										uw_Crossable = COL_Cul_DefaultCross;

									pst_GMat = COL_pst_GMat_Get(pst_ColMap->dpst_Cob[i], NULL);

									if(pst_GMat && (((USHORT) pst_GMat->ul_CustomBits) & uw_Crossable)) continue;
								}
							}
						}

						switch(pst_ColMap->dpst_Cob[i]->uc_Type)
						{
						case COL_C_Zone_Sphere:
							if(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_Anims) && RAY_ACCESS(b_UseBoneOBBox)) // && COL_ColMap_Sphere_RayCast(pst_CurrentGO, pst_MathCob, TRUE))
								COL_Visual_RayCast_OneSkeleton(pst_CurrentGO);
							else 
								COL_ColMap_Sphere_RayCast(pst_CurrentGO, pst_MathCob, FALSE);
							break;

						case COL_C_Zone_Box:
							if(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_Anims) && RAY_ACCESS(b_UseBoneOBBox)) // && COL_ColMap_Sphere_RayCast(pst_CurrentGO, pst_MathCob, TRUE))
								COL_Visual_RayCast_OneSkeleton(pst_CurrentGO);
							else 
								COL_ColMap_Box_RayCast(pst_CurrentGO, pst_MathCob, FALSE);
							break;

						case COL_C_Zone_Cylinder:
							if(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_Anims) && RAY_ACCESS(b_UseBoneOBBox)) // && COL_ColMap_Sphere_RayCast(pst_CurrentGO, pst_MathCob, TRUE))
								COL_Visual_RayCast_OneSkeleton(pst_CurrentGO);
							else 
								COL_ColMap_Cylinder_RayCast(pst_CurrentGO, pst_MathCob, FALSE);
							break;
						}

						if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) 
						{
//							PRO_StopTrameRaster(&ENG_gpst_RasterEng_Events);
							return FALSE;
						}
					}
				}
			}
		}
	}

	COL_gb_UseMinDistance = FALSE;
//	PRO_StopTrameRaster(&ENG_gpst_RasterEng_Events);
	return(RAY_ACCESS(pst_ClosestGO));
}

int	NumBoxTested = 0;
int	NumBoxTouched = 0;
int	NumTriTested = 0;
int	NumTriTouched = 0;


/*$F
=======================================================================================================================


				VISUAL OK3 RAY CAST


=======================================================================================================================
*/

BOOL COL_OK3Box_RayTouch
(
		OBJ_tdst_GameObject *_pst_GO, 
		COL_tdst_OK3_Node	*_pst_Node,
		void				*_p_Data,
		MATH_tdst_Vector	*_pst_OCS_Orig,
		MATH_tdst_Vector	*_pst_OCS_End,
		MATH_tdst_Vector	*_pst_OCS_Dir,
		float				*_pf_OCS_CurrentDist,
		MATH_tdst_Matrix	*_pst_Matrix,
		BOOL				*_pb_BVTouched,
		BOOL				_b_ColMap
 )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_OK3_Box					*pst_Box;
	COL_tdst_OK3_Element				*pst_OK3_Element;
	COL_tdst_IndexedTriangles			*pst_CobObj;
	INT_tdst_Box						st_AABBox;
	GEO_tdst_Object						*pst_Geo;
	COL_tdst_Cob						*pst_Cob;
	COL_tdst_GameMat					*pst_GMat;
	MATH_tdst_Vector					*pst_T1, *pst_T2, *pst_T3;
	MATH_tdst_Vector					st_Norm, st_CollidedPoint;
	MATH_tdst_Vector					st_VectT1T2, st_VectT1T3;
	MATH_tdst_Vector					st_GCS_Ray, st_OCS_Ray;
	ULONG								ul_Triangles;
	int									j, k, l;
	float								f_ObjDist;
	ULONG								ul_SoundID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Box = _pst_Node->pst_OK3_Box;

#ifdef ACTIVE_EDITORS
	pst_Box->ul_OK3_Flag |= 2; /* Tested OK3 Box */
#endif

	pst_Cob = NULL;
	pst_CobObj = NULL;
	pst_Geo = NULL;

	if(_b_ColMap)
	{
		pst_Cob = (COL_tdst_Cob *) _p_Data;
		pst_CobObj = pst_Cob->pst_TriangleCob;
	}
	else
		pst_Geo = (GEO_tdst_Object *) _p_Data;

	MATH_CopyVector(&st_AABBox.st_Min, &pst_Box->st_Min);
	MATH_CopyVector(&st_AABBox.st_Max, &pst_Box->st_Max);


//	PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
	if(!INT_FullRayAABBox(_pst_OCS_Orig, _pst_OCS_Dir, &st_AABBox, NULL, *_pf_OCS_CurrentDist, NULL)) 
	{
//		PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
		return TRUE;
	}

	*_pb_BVTouched = TRUE;

//	PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);

#ifdef ACTIVE_EDITORS
//	pst_Box->ul_OK3_Flag |= 1;	/* Touched OK3 Box */
#endif
	NumBoxTouched ++;

	
	if(!pst_Box->ul_NumElement)
	{
		NumEmptyBox ++;
		return TRUE;
	}

	f_ObjDist = *_pf_OCS_CurrentDist;
	for(j = 0; j < (int) pst_Box->ul_NumElement; j++)
	{
		NumTriTested ++;
		pst_OK3_Element = &pst_Box->pst_OK3_Element[j];

		ul_SoundID = -1;
		if(!_b_ColMap && _pst_GO->pst_Base && _pst_GO->pst_Base->pst_Visu && _pst_GO->pst_Base->pst_Visu->pst_Material)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MAT_tdst_Material		*pst_Material;
			long					l_SubMat;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Material = (MAT_tdst_Material *)_pst_GO->pst_Base->pst_Visu->pst_Material;
			if( pst_Material ) 
			{
				if ( pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti )
				{
					l_SubMat = pst_Geo->dst_Element[pst_OK3_Element->uw_Element].l_MaterialId;
					l_SubMat = lMin(l_SubMat , ((MAT_tdst_Multi *)pst_Material)->l_NumberOfSubMaterials -1 );
					pst_Material = (MAT_tdst_Material*) ((MAT_tdst_Multi *)pst_Material)->dpst_SubMaterial[ l_SubMat ];
				}
				if (pst_Material && pst_Material->st_Id.i->ul_Type == GRO_MaterialMultiTexture )
				{
					ul_SoundID = ((MAT_tdst_MultiTexture *) pst_Material)->uc_Sound;
				}
			}
		}

		/* Do we want to know if we collided a specified Sound ID (like do i collide a water face ?) */
		if((COL_gul_SoundFilter != -1) && (ul_SoundID != -1) && (COL_gul_SoundFilter != ul_SoundID))
			continue;

		pst_GMat = NULL;

		/* We may want to skip Crossable Elements. */
		if
		(
			_b_ColMap
		&&	(RAY_ACCESS(b_SkipFlagX) || RAY_ACCESS(b_SkipCrossable) || RAY_ACCESS(b_SkipCamera))
		&&	(pst_Cob->pst_GMatList)
		)
		{
			/*~~~~~~~~~~~~~~~~~*/
			USHORT	uw_Crossable;
			/*~~~~~~~~~~~~~~~~~*/

			if(RAY_ACCESS(b_UserCrossable))
				uw_Crossable = COL_guw_UserCrossable;
			else
			{
				if((RAY_ACCESS(pst_GMat_GO)) && OBJ_b_TestIdentityFlag(RAY_ACCESS(pst_GMat_GO), OBJ_C_IdentityFlag_ZDM))
					uw_Crossable = ((COL_tdst_Base *) (RAY_ACCESS(pst_GMat_GO))->pst_Extended->pst_Col)->pst_Instance->uw_Crossable;
				else
					uw_Crossable = COL_Cul_DefaultCross;
			}

			pst_GMat = COL_pst_GMat_Get(pst_Cob, &pst_CobObj->dst_Element[pst_OK3_Element->uw_Element]);

			if(pst_GMat && RAY_ACCESS(b_SkipCrossable) && (((USHORT) pst_GMat->ul_CustomBits) & uw_Crossable))
				continue;

			if(pst_GMat && RAY_ACCESS(b_SkipCamera) && ((pst_GMat->ul_CustomBits) & COL_Cul_GMat_Camera))
				continue;

			if(pst_GMat && RAY_ACCESS(b_SkipFlagX) && ((pst_GMat->ul_CustomBits) & COL_Cul_GMat_FlagX)) 
				continue;
		}

		if(pst_GMat && (COL_gul_SoundFilter != -1) && (COL_gul_SoundFilter != (ULONG) pst_GMat->uc_Sound))
			continue;

		ul_Triangles = 0;
		for(l = 0; l < (int) pst_OK3_Element->uw_Element; l++)
		{
			if(_b_ColMap)
				ul_Triangles += (ULONG) pst_CobObj->dst_Element[l].uw_NbTriangles;
			else
				ul_Triangles += pst_Geo->dst_Element[l].l_NbTriangles;
		}

		for(k = 0; k < pst_OK3_Element->uw_NumTriangle; k++)
		{
			/*$off*/
			if(_b_ColMap)
			{
				pst_T1 = &(pst_CobObj->dst_Point[pst_CobObj->dst_Element[pst_OK3_Element->uw_Element].dst_Triangle[pst_OK3_Element->puw_OK3_Triangle[k]].auw_Index[0]]);
				pst_T2 = &(pst_CobObj->dst_Point[pst_CobObj->dst_Element[pst_OK3_Element->uw_Element].dst_Triangle[pst_OK3_Element->puw_OK3_Triangle[k]].auw_Index[1]]);
				pst_T3 = &(pst_CobObj->dst_Point[pst_CobObj->dst_Element[pst_OK3_Element->uw_Element].dst_Triangle[pst_OK3_Element->puw_OK3_Triangle[k]].auw_Index[2]]);
			}
			else
			{
				pst_T1 = &(pst_Geo->dst_Point[pst_Geo->dst_Element[pst_OK3_Element->uw_Element].dst_Triangle[pst_OK3_Element->puw_OK3_Triangle[k]].auw_Index[0]]);
				pst_T2 = &(pst_Geo->dst_Point[pst_Geo->dst_Element[pst_OK3_Element->uw_Element].dst_Triangle[pst_OK3_Element->puw_OK3_Triangle[k]].auw_Index[1]]);
				pst_T3 = &(pst_Geo->dst_Point[pst_Geo->dst_Element[pst_OK3_Element->uw_Element].dst_Triangle[pst_OK3_Element->puw_OK3_Triangle[k]].auw_Index[2]]);
			}
			/*$on*/
			MATH_SubVector(&st_VectT1T2, pst_T2, pst_T1);
			MATH_SubVector(&st_VectT1T3, pst_T3, pst_T1);

			MATH_CrossProduct(&st_Norm, &st_VectT1T2, &st_VectT1T3);

			/* BackFace culling. */
			if(fSupZero(MATH_f_DotProduct(&st_Norm, _pst_OCS_Dir))) continue;

			/* X Rejection Test. */
			if(_pst_OCS_Dir->x > 0.0f)
			{
				if(_pst_OCS_Orig->x > fMax3(pst_T1->x, pst_T2->x, pst_T3->x)) continue;
				if(fMin3(pst_T1->x, pst_T2->x, pst_T3->x) > _pst_OCS_End->x) continue;
			}
			else
			{
				if(fMin3(pst_T1->x, pst_T2->x, pst_T3->x) > _pst_OCS_Orig->x) continue;
				if(_pst_OCS_End->x > fMax3(pst_T1->x, pst_T2->x, pst_T3->x)) continue;
			}

			/* Y Rejection Test. */
			if(_pst_OCS_Dir->y > 0.0f)
			{
				if(_pst_OCS_Orig->y > fMax3(pst_T1->y, pst_T2->y, pst_T3->y)) continue;
				if(fMin3(pst_T1->y, pst_T2->y, pst_T3->y) > _pst_OCS_End->y) continue;
			}
			else
			{
				if(fMin3(pst_T1->y, pst_T2->y, pst_T3->y) > _pst_OCS_Orig->y) continue;
				if(_pst_OCS_End->y > fMax3(pst_T1->y, pst_T2->y, pst_T3->y)) continue;
			}

			/* Z Rejection Test. */
			if(_pst_OCS_Dir->z > 0.0f)
			{
				if(_pst_OCS_Orig->z > fMax3(pst_T1->z, pst_T2->z, pst_T3->z)) continue;
				if(fMin3(pst_T1->z, pst_T2->z, pst_T3->z) > _pst_OCS_End->z) continue;
			}
			else
			{
				if(fMin3(pst_T1->z, pst_T2->z, pst_T3->z) > _pst_OCS_Orig->z) continue;
				if(_pst_OCS_End->z > fMax3(pst_T1->z, pst_T2->z, pst_T3->z)) continue;
			}

//			PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);

			if
			(
				INT_FullRayTriangle
				(
					_pst_OCS_Orig,
					_pst_OCS_Dir,
					pst_T1,
					pst_T2,
					pst_T3,
					&f_ObjDist,
					&st_CollidedPoint,
					FALSE
				)
			)
			{
				NumTriTouched ++;

//				PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
				if(f_ObjDist < *_pf_OCS_CurrentDist)
				{
					if(!COL_gb_UseMinDistance || (COL_gb_UseMinDistance && (f_ObjDist > COL_gf_MinDistance)))
					{
						if(RAY_ACCESS(pb_Visible))
						{
							*(RAY_ACCESS(pb_Visible)) = FALSE;
							return FALSE;
						}

						/* Visual Element Crossable ? */
						if(ul_SoundID == 8) /* Fake Sound ID to indicate ray insensitive elements */
							continue;


						/* Fills the World's RayInfo structure to be able to use it later if needed. */
#ifdef ACTIVE_EDITORS
						MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Origin, RAY_ACCESS(pst_GCS_Origin));
#endif
						if(RAY_ACCESS(b_ReturnLocalData))
						{
							MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint, &st_CollidedPoint);

						}
						else
						{
							if(_b_ColMap)
								MATH_TransformVertexNoScale
								(
									&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint,
									_pst_Matrix,
									&st_CollidedPoint
								);
							else
								MATH_TransformVertex
								(
									&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint,
									_pst_Matrix,
									&st_CollidedPoint
								);
						}

						if(RAY_ACCESS(b_ReturnLocalData))
						{
							MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal, &st_Norm);
						}
						else
						{
							if(_b_ColMap)
								MATH_TransformVectorNoScale
								(
									&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal,
									_pst_Matrix,
									&st_Norm
								);
							else
								MATH_TransformVector
								(
									&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal,
									_pst_Matrix,
									&st_Norm
								);

						}

						MATH_NormalizeEqualVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal);
						(RAY_ACCESS(pst_World))->st_RayInfo.uw_Element = pst_OK3_Element->uw_Element;
						(RAY_ACCESS(pst_World))->st_RayInfo.ul_Triangle = pst_OK3_Element->puw_OK3_Triangle[k];
						(RAY_ACCESS(pst_World))->st_RayInfo.ul_TriangleIndex = pst_OK3_Element->puw_OK3_Triangle[k] + ul_Triangles;
						(RAY_ACCESS(pst_World))->st_RayInfo.pst_GMat = pst_GMat;

						if(pst_GMat && pst_GMat->ul_CustomBits)
							pst_GMat->ul_CustomBits = pst_GMat->ul_CustomBits;

						(RAY_ACCESS(pst_World))->st_RayInfo.uc_Design = 0;
						(RAY_ACCESS(pst_World))->st_RayInfo.pst_CollidedGO = _pst_GO;

						(RAY_ACCESS(pst_World))->st_RayInfo.ul_Sound = 0;

						if(ul_SoundID != -1)
							(RAY_ACCESS(pst_World))->st_RayInfo.ul_Sound = ul_SoundID;

						/* Update Touched Object */
						RAY_ACCESS(pst_ClosestGO) = _pst_GO;

						/* Ray Length when scaled Object Issue */
						if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
						{
							MATH_ScaleVector(&st_OCS_Ray, _pst_OCS_Dir, f_ObjDist);
							MATH_TransformVector(&st_GCS_Ray, _pst_Matrix, &st_OCS_Ray);
							*_pf_OCS_CurrentDist = f_ObjDist;
							RAY_ACCESS(f_GCS_CurrentDist) =  MATH_f_NormVector(&st_GCS_Ray);
						}
						else
						{
							*_pf_OCS_CurrentDist = f_ObjDist;
							RAY_ACCESS(f_GCS_CurrentDist) = f_ObjDist;
						}
					}
				}
			}
//			else
//				PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
		}
	}

	return TRUE;
}


/*$F
=======================================================================================================================
=======================================================================================================================
*/

BOOL COL_OK3Box_RayCast_Recursive
(
		OBJ_tdst_GameObject *_pst_GO,
		COL_tdst_OK3_Node	*_pst_Node,
		void				*_p_Data,
		MATH_tdst_Vector	*_pst_OCS_Orig,
		MATH_tdst_Vector	*_pst_OCS_End,
		MATH_tdst_Vector	*_pst_OCS_Dir,
		float				*_pf_OCS_CurrentDist,
		MATH_tdst_Matrix	*_pst_Matrix,
		BOOL				_b_ColMap
 )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_OK3_Box		*pst_Box;
	BOOL					b_BV_Touched;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_BV_Touched = FALSE;
	NumBoxTested ++;
	pst_Box = _pst_Node->pst_OK3_Box;

//	if(_pst_Node->pst_OK3_Box->ul_NumElement)
	{
		/* Quick Rejection X */
		if(_pst_OCS_Dir->x > 0.0f)
		{
			if(_pst_OCS_Orig->x > pst_Box->st_Max.x) goto TestNextNode;
			if(_pst_OCS_End->x < pst_Box->st_Min.x) goto TestNextNode;
		}
		else
		{
			if(_pst_OCS_Orig->x < pst_Box->st_Min.x) goto TestNextNode;
			if(_pst_OCS_End->x > pst_Box->st_Max.x) goto TestNextNode;
		}

		/* Quick Rejection Y */
		if(_pst_OCS_Dir->y > 0.0f)
		{
			if(_pst_OCS_Orig->y > pst_Box->st_Max.y) goto TestNextNode;
			if(_pst_OCS_End->y < pst_Box->st_Min.y) goto TestNextNode;

		}
		else
		{
			if(_pst_OCS_Orig->y < pst_Box->st_Min.y) goto TestNextNode;
			if(_pst_OCS_End->y > pst_Box->st_Max.y) goto TestNextNode;

		}

		/* Quick Rejection Z */
		if(_pst_OCS_Dir->z > 0.0f)
		{
			if(_pst_OCS_Orig->z > pst_Box->st_Max.z) goto TestNextNode;
			if(_pst_OCS_End->z < pst_Box->st_Min.z) goto TestNextNode;

		}
		else
		{
			if(_pst_OCS_Orig->z < pst_Box->st_Min.z) goto TestNextNode;
			if(_pst_OCS_End->z > pst_Box->st_Max.z) goto TestNextNode;
		}

		b_BV_Touched = FALSE;
		if(!COL_OK3Box_RayTouch(_pst_GO, _pst_Node, _p_Data, _pst_OCS_Orig, _pst_OCS_End, _pst_OCS_Dir, _pf_OCS_CurrentDist, _pst_Matrix, &b_BV_Touched, _b_ColMap))
		{
			return FALSE;
		}

		if(!b_BV_Touched) goto TestNextNode;
	}

	if(_pst_Node->pst_Son)
	{
		if(!COL_OK3Box_RayCast_Recursive(_pst_GO, _pst_Node->pst_Son, _p_Data, _pst_OCS_Orig, _pst_OCS_End, _pst_OCS_Dir, _pf_OCS_CurrentDist, _pst_Matrix, _b_ColMap))
		{
			return FALSE;
		}
	}


TestNextNode:

	if(_pst_Node->pst_Next)
	{
		if(!COL_OK3Box_RayCast_Recursive(_pst_GO, _pst_Node->pst_Next, _p_Data, _pst_OCS_Orig, _pst_OCS_End, _pst_OCS_Dir, _pf_OCS_CurrentDist, _pst_Matrix,_b_ColMap))
			return FALSE;
	}

	return TRUE;
}


/*$F
 =======================================================================================================================


									OK3 RAY CAST


 =======================================================================================================================
 */

void COL_OK3_RayCast(OBJ_tdst_GameObject *_pst_GO, void *_p_Data, BOOL _b_ColMap)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_OK3			*pst_OK3;
	MATH_tdst_Matrix		st_33_InvertMatrix, st_33_GlobalMatrix;
	MATH_tdst_Vector		st_OCS_Dir, st_OCS_Orig, st_OCS_End;
	MATH_tdst_Vector		*pst_OCS_Dir, *pst_OCS_Orig;
	MATH_tdst_Vector		st_GCS_Ray, st_OCS_Ray;
	float					f_OCS_CurrentDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//	PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
	if(_b_ColMap)
		pst_OK3 = ((COL_tdst_Cob *) _p_Data)->pst_TriangleCob->pst_OK3;
	else
		pst_OK3 = ((GEO_tdst_Object *) _p_Data)->pst_OK3;

	if(_b_ColMap)
	{
		if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
		{
			MATH_Transp33MatrixWithoutBuffer(&st_33_InvertMatrix, _pst_GO->pst_GlobalMatrix);
			MATH_SetType(&st_33_InvertMatrix, MATH_l_GetType(_pst_GO->pst_GlobalMatrix));

			if(MATH_b_TestScaleType(&st_33_InvertMatrix))
			{
				st_33_InvertMatrix.Sx = fInv(_pst_GO->pst_GlobalMatrix->Sx);
				st_33_InvertMatrix.Sy = fInv(_pst_GO->pst_GlobalMatrix->Sy);
				st_33_InvertMatrix.Sz = fInv(_pst_GO->pst_GlobalMatrix->Sz);
			}

			MATH_NegVector(MATH_pst_GetTranslation(&st_33_InvertMatrix), MATH_pst_GetTranslation(_pst_GO->pst_GlobalMatrix));
			MATH_TransformVectorNoScale(&st_33_InvertMatrix.T, &st_33_InvertMatrix, &st_33_InvertMatrix.T);
		}
		else
			MATH_InvertMatrix(&st_33_InvertMatrix, _pst_GO->pst_GlobalMatrix);

		MATH_CopyMatrix(&st_33_GlobalMatrix, _pst_GO->pst_GlobalMatrix);
	}
	else
	{
		/* Creates a 3x3 matrix containing the possible scale of the object */
		MATH_MakeOGLMatrix(&st_33_GlobalMatrix, _pst_GO->pst_GlobalMatrix);
		MATH_ClearScale(&st_33_GlobalMatrix, 1);

		/* Inverts the 3x3 matrix */
		MATH_Invert33Matrix(&st_33_InvertMatrix, &st_33_GlobalMatrix);
		MATH_ClearScale(&st_33_InvertMatrix, 1);
		MATH_NegVector(MATH_pst_GetTranslation(&st_33_InvertMatrix), MATH_pst_GetTranslation(_pst_GO->pst_GlobalMatrix));
		MATH_TransformVector(&st_33_InvertMatrix.T, &st_33_InvertMatrix, &st_33_InvertMatrix.T);
	}


	/* Compute Origin and Direction of the Ray in GameObject Coordinate System */
	if(MATH_b_TestIdentityType(_pst_GO->pst_GlobalMatrix))
	{
		pst_OCS_Dir = RAY_ACCESS(pst_GCS_Dir);
		MATH_SubVector(&st_OCS_Orig, RAY_ACCESS(pst_GCS_Origin), &_pst_GO->pst_GlobalMatrix->T);
		pst_OCS_Orig = &st_OCS_Orig;
	}
	else
	{
		MATH_TransformVertexNoScale(&st_OCS_Orig, &st_33_InvertMatrix, RAY_ACCESS(pst_GCS_Origin));
		MATH_TransformVectorNoScale(&st_OCS_Dir, &st_33_InvertMatrix, RAY_ACCESS(pst_GCS_Dir));
#ifdef JADEFUSION
		if( MATH_b_NulVector(&st_OCS_Dir) )
        {
            return;
        }
#endif
		MATH_NormalizeEqualVector(&st_OCS_Dir);
		pst_OCS_Dir = &st_OCS_Dir;
		pst_OCS_Orig = &st_OCS_Orig;
	}

	/* Ray Length when scaled Object Issue */
	if(!_b_ColMap && MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
	{
		MATH_ScaleVector(&st_GCS_Ray, RAY_ACCESS(pst_GCS_Dir), RAY_ACCESS(f_GCS_CurrentDist));
		MATH_TransformVector(&st_OCS_Ray, &st_33_InvertMatrix, &st_GCS_Ray);
		f_OCS_CurrentDist = MATH_f_NormVector(&st_OCS_Ray);
	}
	else
		f_OCS_CurrentDist = RAY_ACCESS(f_GCS_CurrentDist);


	/* Point at End of the Ray. */
	MATH_MulVector(&st_OCS_End, pst_OCS_Dir, f_OCS_CurrentDist);
	MATH_AddEqualVector(&st_OCS_End, pst_OCS_Orig);


	COL_OK3Box_RayCast_Recursive(_pst_GO, pst_OK3->pst_OK3_God, _p_Data, pst_OCS_Orig, &st_OCS_End, pst_OCS_Dir, &f_OCS_CurrentDist, &st_33_GlobalMatrix, _b_ColMap);

//	PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
}

/*$F
 =======================================================================================================================


									VISUAL TRIANGLES RAY CAST


 =======================================================================================================================
 */
int	_NumTriTested = 0;
int	_NumTriTouched = 0;

void COL_Visual_Triangles_RayCast(OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *_pst_Geo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	GEO_tdst_IndexedTriangle			*pst_Triangle, *pst_LastTriangle;
	MATH_tdst_Matrix					st_33_GlobalMatrix, st_33_InvertMatrix;
	MATH_tdst_Vector					st_OCS_Dir, st_OCS_Orig, st_OCS_End;
	MATH_tdst_Vector					*pst_OCS_Dir, *pst_OCS_Orig;
	MATH_tdst_Vector					*pst_T1, *pst_T2, *pst_T3;
	MATH_tdst_Vector					st_Norm, st_CollidedPoint;
	MATH_tdst_Vector					st_VectT1T2, st_VectT1T3;
	MATH_tdst_Vector					st_GCS_Ray, st_OCS_Ray;
	ULONG								ul_Element, ul_Triangle, ul_TotalTriangle;
	float								f_ObjDist, f_OCS_CurrentDist;
	BOOL								b_XPlus, b_YPlus, b_ZPlus;
	ULONG								ul_SoundID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//	PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
#ifdef JADEFUSION
	if( MATH_b_NulVector( RAY_ACCESS(pst_GCS_Dir) ) )
    {
        return;
    }
#endif
	/* Creates a 3x3 matrix containing the possible scale of the object */
	MATH_MakeOGLMatrix(&st_33_GlobalMatrix, _pst_GO->pst_GlobalMatrix);
	MATH_ClearScale(&st_33_GlobalMatrix, 1);

	/* Inverts the 3x3 matrix */
	MATH_Invert33Matrix(&st_33_InvertMatrix, &st_33_GlobalMatrix);
	MATH_ClearScale(&st_33_InvertMatrix, 1);
	MATH_NegVector(MATH_pst_GetTranslation(&st_33_InvertMatrix), MATH_pst_GetTranslation(_pst_GO->pst_GlobalMatrix));
	MATH_TransformVector(&st_33_InvertMatrix.T, &st_33_InvertMatrix, &st_33_InvertMatrix.T);

	/* Compute Origin and Direction of the Ray in GameObject Coordinate System */
	if(MATH_b_TestIdentityType(_pst_GO->pst_GlobalMatrix))
	{
		pst_OCS_Dir = RAY_ACCESS(pst_GCS_Dir);
		MATH_SubVector(&st_OCS_Orig, RAY_ACCESS(pst_GCS_Origin), &_pst_GO->pst_GlobalMatrix->T);
		pst_OCS_Orig = &st_OCS_Orig;
	}
	else
	{
		MATH_TransformVertex(&st_OCS_Orig, &st_33_InvertMatrix, RAY_ACCESS(pst_GCS_Origin));
		MATH_TransformVector(&st_OCS_Dir, &st_33_InvertMatrix, RAY_ACCESS(pst_GCS_Dir));

		MATH_NormalizeEqualVector(&st_OCS_Dir);
		pst_OCS_Dir = &st_OCS_Dir;
		pst_OCS_Orig = &st_OCS_Orig;
	}

	/* Ray Length when scaled Object Issue */
	if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
	{
		MATH_ScaleVector(&st_GCS_Ray, RAY_ACCESS(pst_GCS_Dir), RAY_ACCESS(f_GCS_CurrentDist));
		MATH_TransformVector(&st_OCS_Ray, &st_33_InvertMatrix, &st_GCS_Ray);
		f_OCS_CurrentDist = MATH_f_NormVector(&st_OCS_Ray);
	}
	else
		f_OCS_CurrentDist = RAY_ACCESS(f_GCS_CurrentDist);

	b_XPlus = (pst_OCS_Dir->x > 0.0f);
	b_YPlus = (pst_OCS_Dir->y > 0.0f);
	b_ZPlus = (pst_OCS_Dir->z > 0.0f);

	pst_Element = _pst_Geo->dst_Element;
	pst_LastElement = pst_Element + _pst_Geo->l_NbElements;
	for(ul_TotalTriangle = 0, ul_Element = 0; pst_Element < pst_LastElement; pst_Element++, ul_Element++)
	{
		/* Strip purpose - To avoid Bug if triangles not decompressed - */
		if(!pst_Element->dst_Triangle) continue;

		ul_SoundID = -1;
		if(_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Visu && _pst_GO->pst_Base->pst_Visu->pst_Material)
		{

			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MAT_tdst_Material			*pst_Material;
			long						l_SubMat;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Material = (MAT_tdst_Material *)_pst_GO->pst_Base->pst_Visu->pst_Material;
			if( pst_Material ) 
			{
				if ( pst_Material->st_Id.i->ul_Type == GRO_MaterialMulti )
				{
					l_SubMat = pst_Element->l_MaterialId;
					l_SubMat = lMin(l_SubMat , ((MAT_tdst_Multi *)pst_Material)->l_NumberOfSubMaterials -1 );
					pst_Material = (MAT_tdst_Material*) ((MAT_tdst_Multi *)pst_Material)->dpst_SubMaterial[ l_SubMat ];
				}
				if (pst_Material && pst_Material->st_Id.i->ul_Type == GRO_MaterialMultiTexture )
				{
					ul_SoundID = ((MAT_tdst_MultiTexture *) pst_Material)->uc_Sound;
				}
			}							
		}

		/* Do we want to know if we collided a specified Sound ID (like do i collide a water face ?) */
		if((COL_gul_SoundFilter != -1) && (ul_SoundID != -1) && (COL_gul_SoundFilter != ul_SoundID))
		//if((COL_gul_SoundFilter != -1) && (COL_gul_SoundFilter != ul_SoundID))
		{
			ul_TotalTriangle += (ULONG) pst_Element->l_NbTriangles;
			continue;
		}

		pst_Triangle = pst_Element->dst_Triangle;
		pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;
		for(ul_Triangle = 0; pst_Triangle < pst_LastTriangle; pst_Triangle++, ul_TotalTriangle++, ul_Triangle++)
		{
			_NumTriTested ++;

			/* We get the triangles points. */
			pst_T1 = VCast(&_pst_Geo->dst_Point[pst_Triangle->auw_Index[0]]);
			pst_T2 = VCast(&_pst_Geo->dst_Point[pst_Triangle->auw_Index[1]]);
			pst_T3 = VCast(&_pst_Geo->dst_Point[pst_Triangle->auw_Index[2]]);

			MATH_SubVector(&st_VectT1T2, pst_T2, pst_T1);
			MATH_SubVector(&st_VectT1T3, pst_T3, pst_T1);

			MATH_CrossProduct(&st_Norm, &st_VectT1T2, &st_VectT1T3);

			/* BackFace culling. */
			if(fSupZero(MATH_f_DotProduct(&st_Norm, pst_OCS_Dir))) continue;

			/* Point at End of the Ray. */
			MATH_MulVector(&st_OCS_End, pst_OCS_Dir, f_OCS_CurrentDist);
			MATH_AddEqualVector(&st_OCS_End, pst_OCS_Orig);

			/* X Rejection Test. */
			if(b_XPlus)
			{
				if(pst_OCS_Orig->x > fMax3(pst_T1->x, pst_T2->x, pst_T3->x)) continue;
				if(fMin3(pst_T1->x, pst_T2->x, pst_T3->x) > st_OCS_End.x) continue;
			}
			else
			{
				if(fMin3(pst_T1->x, pst_T2->x, pst_T3->x) > pst_OCS_Orig->x) continue;
				if(st_OCS_End.x > fMax3(pst_T1->x, pst_T2->x, pst_T3->x)) continue;
			}

			/* Y Rejection Test. */
			if(b_YPlus)
			{
				if(st_OCS_Orig.y > fMax3(pst_T1->y, pst_T2->y, pst_T3->y)) continue;
				if(fMin3(pst_T1->y, pst_T2->y, pst_T3->y) > st_OCS_End.y) continue;
			}
			else
			{
				if(fMin3(pst_T1->y, pst_T2->y, pst_T3->y) > st_OCS_Orig.y) continue;
				if(st_OCS_End.y > fMax3(pst_T1->y, pst_T2->y, pst_T3->y)) continue;
			}

			/* Z Rejection Test. */
			if(b_ZPlus > 0.0f)
			{
				if(pst_OCS_Orig->z > fMax3(pst_T1->z, pst_T2->z, pst_T3->z)) continue;
				if(fMin3(pst_T1->z, pst_T2->z, pst_T3->z) > st_OCS_End.z) continue;
			}
			else
			{
				if(fMin3(pst_T1->z, pst_T2->z, pst_T3->z) > pst_OCS_Orig->z) continue;
				if(st_OCS_End.z > fMax3(pst_T1->z, pst_T2->z, pst_T3->z)) continue;
			}

//			PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
			if
			(
				INT_FullRayTriangle
				(
					pst_OCS_Orig,
					pst_OCS_Dir,
					pst_T1,
					pst_T2,
					pst_T3,
					&f_ObjDist,
					&st_CollidedPoint,
					FALSE
				)
			)
			{
				_NumTriTouched ++;
//				PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
				if(f_ObjDist < f_OCS_CurrentDist)
				{
					if(!COL_gb_UseMinDistance || (COL_gb_UseMinDistance && (f_ObjDist > COL_gf_MinDistance)))
					{
						if(RAY_ACCESS(pb_Visible))
						{
							*(RAY_ACCESS(pb_Visible)) = FALSE;
							return;
						}

						/* Visual Element Crossable */
						if(ul_SoundID == 8)
							continue;

						
						/* Fills the World's RayInfo structure to be able to use it later if needed. */
#ifdef ACTIVE_EDITORS
						MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Origin, RAY_ACCESS(pst_GCS_Origin));
#endif
						if(RAY_ACCESS(b_ReturnLocalData))
						{
							MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint, &st_CollidedPoint);					
						}
						else
						{
							MATH_TransformVertex
								(
								&(RAY_ACCESS(pst_World))->st_RayInfo.st_CollidedPoint,
								&st_33_GlobalMatrix,
								&st_CollidedPoint
								);
						}
						
						
						if(RAY_ACCESS(b_ReturnLocalData))
						{
							MATH_CopyVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal, &st_Norm);
						}
						else
						{
							MATH_TransformVector
								(
								&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal,
								&st_33_GlobalMatrix,
								&st_Norm
								);
						}
						
						MATH_NormalizeEqualVector(&(RAY_ACCESS(pst_World))->st_RayInfo.st_Normal);
						(RAY_ACCESS(pst_World))->st_RayInfo.uw_Element = (USHORT) ul_Element;
						(RAY_ACCESS(pst_World))->st_RayInfo.ul_Triangle = ul_Triangle;
						(RAY_ACCESS(pst_World))->st_RayInfo.ul_TriangleIndex = ul_TotalTriangle;

						(RAY_ACCESS(pst_World))->st_RayInfo.ul_Sound = 0;

						if(ul_SoundID != -1)
							(RAY_ACCESS(pst_World))->st_RayInfo.ul_Sound = ul_SoundID;

						(RAY_ACCESS(pst_World))->st_RayInfo.pst_CollidedGO = _pst_GO;
						
						/* Update Touched Object */
						RAY_ACCESS(pst_ClosestGO) = _pst_GO;
						
						/* Ray Length when scaled Object Issue */
						if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
						{
							MATH_ScaleVector(&st_OCS_Ray, pst_OCS_Dir, f_ObjDist);
							MATH_TransformVector(&st_GCS_Ray, &st_33_GlobalMatrix, &st_OCS_Ray);
							f_ObjDist = MATH_f_NormVector(&st_GCS_Ray);
						}
						
						RAY_ACCESS(f_GCS_CurrentDist) = f_ObjDist;
						
						/* Ray Length when scaled Object Issue */
						if(MATH_b_TestScaleType(_pst_GO->pst_GlobalMatrix))
						{
							MATH_ScaleVector(&st_GCS_Ray, RAY_ACCESS(pst_GCS_Dir), RAY_ACCESS(f_GCS_CurrentDist));
							MATH_TransformVector(&st_OCS_Ray, &st_33_InvertMatrix, &st_GCS_Ray);
							f_OCS_CurrentDist = MATH_f_NormVector(&st_OCS_Ray);
						}
						else
							f_OCS_CurrentDist = RAY_ACCESS(f_GCS_CurrentDist);
					}
				}
			}
//			else
//				PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
		}
	}

//	PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
}


/*$F
 ===================================================================================================


									VISUAL RAY CAST ON ONE OBJECT



 ===================================================================================================
 */

OBJ_tdst_GameObject *COL_Visual_RayCast_OneObject
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Orig,
	MATH_tdst_Vector	*_pst_Dir,
	float				_f_MaxDist,
	ULONG				_ul_Filter,
	ULONG				_ul_NoFilter,
	ULONG				_ul_ID,
	BOOL				*_pb_Visible,
	BOOL				_b_UseRaySkipFlag,
	BOOL				_b_SkipBVTest,				/* For Vincent RayCast on Map */
	BOOL				_b_ReturnLocalData,			/* For Vincent RayCast on Map */
	BOOL				_b_UseBoneOBBox
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object *pst_Geo;
	float			f_ObjDist;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	RAY_ACCESS(pst_GCS_Origin) = _pst_Orig;
	RAY_ACCESS(pst_GCS_Dir) = _pst_Dir;
	RAY_ACCESS(pst_ClosestGO) = NULL;
	RAY_ACCESS(pb_Visible) = _pb_Visible;
	RAY_ACCESS(f_GCS_CurrentDist) = _f_MaxDist;
	RAY_ACCESS(b_UseRaySkipFlag) = _b_UseRaySkipFlag;
	RAY_ACCESS(b_ReturnLocalData) = _b_ReturnLocalData;
	RAY_ACCESS(b_UseBoneOBBox) = _b_UseBoneOBBox;

	if
	(
		(TAB_b_IsAHole(_pst_GO))
	||	!(OBJ_b_TestFlag(_pst_GO, _ul_Filter, _ul_NoFilter, _ul_ID))
	||	(!(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)) && !(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)))
#ifdef ACTIVE_EDITORS
	||	(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden)
#endif
	) return NULL;

	if
	(
		_b_SkipBVTest
	||	(
			OBJ_BV_IsSphere(_pst_GO->pst_BV)
		&&	(COL_RaySphereBV(_pst_Orig, _pst_Dir, _pst_GO, &f_ObjDist, RAY_ACCESS(f_GCS_CurrentDist)))
		)
	||	(
			OBJ_BV_IsAABBox(_pst_GO->pst_BV)
		&&	(COL_RayAABBox(_pst_Orig, _pst_Dir, _pst_GO, &f_ObjDist, RAY_ACCESS(f_GCS_CurrentDist)))
		)
	)
	{
		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
		{
			if(!(_pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Draw)) return NULL;

			pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(_pst_GO);

			if(!pst_Geo) return NULL;

			GEO_I_Need_The_Triangles_Begin(pst_Geo,0);

			if(pst_Geo->pst_OK3)
				COL_OK3_RayCast(_pst_GO, (void *)pst_Geo, FALSE);
			else
				COL_Visual_Triangles_RayCast(_pst_GO, pst_Geo);

			GEO_I_Need_The_Triangles_End(pst_Geo,0);
		}
		else
		{
			if((OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)))
			{
				if(RAY_ACCESS(b_UseBoneOBBox))
				{
					COL_Visual_RayCast_OneSkeleton(_pst_GO);
					if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) return NULL;
				}
				else
				{
					if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
					{
						/*~~~~~~~~~~~~~~~~~~~~~~~~*/
						COL_tdst_ColMap *pst_ColMap;
						/*~~~~~~~~~~~~~~~~~~~~~~~~*/

						pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;

						if(pst_ColMap->dpst_Cob[0]->uc_Type == COL_C_Zone_Triangles)
						{
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
							COL_tdst_IndexedTriangles	*pst_CobObj;
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

							if(!COL_b_ColMap_IsActive(pst_ColMap, 0)) return NULL;

							pst_CobObj = pst_ColMap->dpst_Cob[0]->pst_TriangleCob;

							if(pst_CobObj->pst_OK3)
								COL_ColMap_OK3_RayCast(_pst_GO, pst_ColMap);
							else
								COL_ColMap_Triangles_RayCast(_pst_GO, pst_ColMap);

							if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) return NULL;
						}
						else
						{
							/*~~~~~~*/
							UCHAR	i;
							/*~~~~~~*/

							for(i = 0; i < pst_ColMap->uc_NbOfCob; i++)
							{
								/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
								COL_tdst_Mathematical	*pst_MathCob;
								/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

								if(!COL_b_ColMap_IsActive(pst_ColMap, i)) continue;

								pst_MathCob = pst_ColMap->dpst_Cob[i]->pst_MathCob;

								switch(pst_ColMap->dpst_Cob[i]->uc_Type)
								{
								case COL_C_Zone_Sphere:
										COL_ColMap_Sphere_RayCast(_pst_GO, pst_MathCob, TRUE);
									break;

								case COL_C_Zone_Box:
										COL_ColMap_Box_RayCast(_pst_GO, pst_MathCob, TRUE);
									break;

								case COL_C_Zone_Cylinder:
										COL_ColMap_Cylinder_RayCast(_pst_GO, pst_MathCob, TRUE);
									break;
								}

								if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) return NULL;

							}
						}

					}

				}
			}
		}
	}

	COL_gb_UseMinDistance = FALSE;
	return(RAY_ACCESS(pst_ClosestGO));
}

int NumTriCast = 0;
int NumOK3Cast = 0;
int NumSkelCast = 0;
int NumColTriCast = 0;
int NumColMathCast = 0;


/*$F
 ===================================================================================================


									VISUAL RAY CAST ON WORLD



 ===================================================================================================
 */

OBJ_tdst_GameObject *COL_Visual_RayCast
(
	WOR_tdst_World		*_pst_World,
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Orig,
	MATH_tdst_Vector	*_pst_Dir,
	float				_f_MaxDist,
	ULONG				_ul_Filter,
	ULONG				_ul_NoFilter,
	ULONG				_ul_ID,
	BOOL				*_pb_Visible,
	BOOL				_b_UseRaySkipFlag,
#ifdef JADEFUSION
	BOOL				_b_UseBoneOBBox,
	BOOL                _bIgnoreDrawState
#else
	BOOL				_b_UseBoneOBBox
#endif
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFtable	*pst_Table;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
	GEO_tdst_Object		*pst_Geo;
	float				f_ObjDist;
	BOOL				b_CastOnAnims;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
#ifdef TEST_DETECT_LINE_OF_FIRE_SPG2
    void SPG2_AddALine(MATH_tdst_Vector *Pos, MATH_tdst_Vector *Dir);
    SPG2_AddALine(_pst_Orig, _pst_Dir);
#endif
*/

	NumEmptyBox = 0;
	NumBoxTested = 0;
	NumBoxTouched = 0;
	NumTriTested = 0;
	NumTriTouched = 0;

	b_CastOnAnims = FALSE;

//	PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_Events);
	RAY_ACCESS(pst_World) = _pst_World;
	RAY_ACCESS(pst_GCS_Origin) = _pst_Orig;
	RAY_ACCESS(pst_GCS_Dir) = _pst_Dir;
	RAY_ACCESS(pst_ClosestGO) = NULL;
	RAY_ACCESS(pb_Visible) = _pb_Visible;
	RAY_ACCESS(f_GCS_CurrentDist) = _f_MaxDist;
	RAY_ACCESS(b_UseRaySkipFlag) = _b_UseRaySkipFlag;
	RAY_ACCESS(b_ReturnLocalData) = FALSE;
	RAY_ACCESS(b_UseBoneOBBox) = _b_UseBoneOBBox;

	if(_pb_Visible) *_pb_Visible = TRUE;
	_pst_World->ul_NbCrossed = 0;

	pst_Table = &_pst_World->st_EOT.st_Visu;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Table);

OneMoreTime: 

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		/*$off*/
		/* If the object is filtered, we continue. */
		if
		(
			(TAB_b_IsAHole(pst_CurrentGO))
		||	!(OBJ_b_TestFlag(pst_CurrentGO, _ul_Filter, _ul_NoFilter, _ul_ID))
		||	(!(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_Visu)) && !(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_Anims)))
		||	(pst_CurrentGO == _pst_GO)
#ifdef ACTIVE_EDITORS
		||	(pst_CurrentGO->ul_EditorFlags & OBJ_C_EditFlags_Hidden)
#endif
		) continue;

//		PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
		if
		(
			(
				OBJ_BV_IsSphere(pst_CurrentGO->pst_BV)
			&&	(COL_RaySphereBV(_pst_Orig, _pst_Dir, pst_CurrentGO, &f_ObjDist, RAY_ACCESS(f_GCS_CurrentDist)))
			)
		||	(
				OBJ_BV_IsAABBox(pst_CurrentGO->pst_BV)
			&&	(COL_RayAABBox(_pst_Orig, _pst_Dir, pst_CurrentGO, &f_ObjDist, RAY_ACCESS(f_GCS_CurrentDist)))
			)
		)
		/*$on*/
		{
//			PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
			/* Crossed Objects management */
			if(RAY_ACCESS(b_UseRaySkipFlag) && (OBJ_b_TestControlFlag(pst_CurrentGO, OBJ_C_ControlFlag_RayInsensitive)))
			{
				if((RAY_ACCESS(pst_World))->ul_NbCrossed == COL_Cul_MaxCrossed)
					(RAY_ACCESS(pst_World))->ul_NbCrossed--;
				(RAY_ACCESS(pst_World))->apst_CrossObjects[(RAY_ACCESS(pst_World))->ul_NbCrossed++] = pst_CurrentGO;
				continue;
			}

			if(OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_Visu))
			{
#ifdef JADEFUSION
				if(!_bIgnoreDrawState && !(pst_CurrentGO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Draw)) continue;
#else
				if(!(pst_CurrentGO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_Draw)) continue;
#endif
				pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_CurrentGO);
				if(!pst_Geo) continue;

				GEO_I_Need_The_Triangles_Begin(pst_Geo,1);

				if(pst_Geo->pst_OK3)
				{
					NumOK3Cast ++;
//					PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
					COL_OK3_RayCast(pst_CurrentGO, (void *)pst_Geo, FALSE);
//					PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
				}
				else
				{
					NumTriCast ++;
//					PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
					COL_Visual_Triangles_RayCast(pst_CurrentGO, pst_Geo);
//					PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
				}

				GEO_I_Need_The_Triangles_End(pst_Geo,1);

				if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) 
				{
					PRO_StopTrameRaster(&ENG_gpst_RasterEng_Events);
					return NULL;
				}
			}
			else
			{
				if((OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_Anims)))
				{
					if(RAY_ACCESS(b_UseBoneOBBox))
					{	
						NumSkelCast ++;
//						PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
						COL_Visual_RayCast_OneSkeleton(pst_CurrentGO);
//						PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
						if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) 
						{
							PRO_StopTrameRaster(&ENG_gpst_RasterEng_Events);
//							PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
							return NULL;
						}
					}
					else
					{
						if((COL_gul_SoundFilter == -1) && OBJ_b_TestIdentityFlag(pst_CurrentGO, OBJ_C_IdentityFlag_ColMap))
						{
							/*~~~~~~~~~~~~~~~~~~~~~~~~*/
							COL_tdst_ColMap *pst_ColMap;
							/*~~~~~~~~~~~~~~~~~~~~~~~~*/

							pst_ColMap = ((COL_tdst_Base *) pst_CurrentGO->pst_Extended->pst_Col)->pst_ColMap;

							if(pst_ColMap->dpst_Cob[0]->uc_Type == COL_C_Zone_Triangles)
							{
								/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
								COL_tdst_IndexedTriangles	*pst_CobObj;
								/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

								if(!COL_b_ColMap_IsActive(pst_ColMap, 0)) return NULL;

								NumColTriCast ++;

								pst_CobObj = pst_ColMap->dpst_Cob[0]->pst_TriangleCob;

//								PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
								if(pst_CobObj->pst_OK3)
									COL_ColMap_OK3_RayCast(pst_CurrentGO, pst_ColMap);
								else
									COL_ColMap_Triangles_RayCast(pst_CurrentGO, pst_ColMap);

//								PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);

								if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) 
								{
									PRO_StopTrameRaster(&ENG_gpst_RasterEng_Events);
///									PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
									return NULL;
								}
							}
							else
							{
								/*~~~~~~*/
								UCHAR	i;
								/*~~~~~~*/

								for(i = 0; i < pst_ColMap->uc_NbOfCob; i++)
								{
									/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
									COL_tdst_Mathematical	*pst_MathCob;
									/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

									if(!COL_b_ColMap_IsActive(pst_ColMap, i)) continue;

									NumColMathCast ++;

									pst_MathCob = pst_ColMap->dpst_Cob[i]->pst_MathCob;

//									PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
									switch(pst_ColMap->dpst_Cob[i]->uc_Type)
									{
									case COL_C_Zone_Sphere:
											COL_ColMap_Sphere_RayCast(pst_CurrentGO, pst_MathCob, TRUE);
										break;

									case COL_C_Zone_Box:
											COL_ColMap_Box_RayCast(pst_CurrentGO, pst_MathCob, TRUE);
										break;

									case COL_C_Zone_Cylinder:
											COL_ColMap_Cylinder_RayCast(pst_CurrentGO, pst_MathCob, TRUE);
										break;
									}

//									PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);

									if(RAY_ACCESS(pb_Visible) && (*(RAY_ACCESS(pb_Visible)) == FALSE)) 
									{
										PRO_StopTrameRaster(&ENG_gpst_RasterEng_Events);
//										PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
										return NULL;
									}
								}
							}
						}
					}
				}
			}
		}
//		else
//			PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
	}

	if(!b_CastOnAnims)
	{
		b_CastOnAnims = TRUE;
		pst_Table = &_pst_World->st_EOT.st_Anims;
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Table);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Table);
		goto OneMoreTime;
	}

	COL_gb_UseMinDistance = FALSE;

	PRO_StopTrameRaster(&ENG_gpst_RasterEng_Events);
//	PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);

	return(RAY_ACCESS(pst_ClosestGO));
}

/*$F
 ===================================================================================================


									PHOTO ANALYSER



 ===================================================================================================
 */
extern void							OBJ_UpdateCullingVars(CAM_tdst_Camera *);
extern BOOL							OBJ_Frame_CullingAABBox
									(
										MATH_tdst_Vector *,
										MATH_tdst_Vector *,
										CAM_tdst_Camera *,
										LONG,
										LONG,
										float *,
										float *
									);
extern BOOL							OBJ_Frame_CullingPoint(MATH_tdst_Vector *, CAM_tdst_Camera *, LONG, LONG);
extern OBJ_tdst_GameObject			*ANI_pst_GetObjectByAICanal(OBJ_tdst_GameObject *, UCHAR);
extern struct OBJ_tdst_GameObject_	**AI_gpst_MainActors;

#define IMG_FotoScreenX 512
#define IMG_FotoScreenY 256

#define IMG_TINY		1
#define IMG_CENTER		2
#define IMG_RAY			4
#define IMG_HUGE		8

/*
 * Les infos photos sont stockés ans l'entier i3 de la designe struct Il y a 2
 * types d'objets à prendre en photo les objets de la mission (cumulable) : pour
 * ceux ci 2 octets de i3 servent seulement celui de poids fort indique le numéro
 * de mission (à partir de 1) celui de poids faible donne le numéro du bit à
 * mettre à 1 (entre 0 et 23) Dans ce cas la valeur de retour sera 1 décalé du
 * numéro de bit Des objets spécifiques (non cumulables et prioritaire par rapport
 * aux précédents:: pour l'instant photo) ils sont reconnaissables par un octet de
 * poids faible > 24 Dans ce cas la valeur retournée sera i3
 */
int IMG_Log;
#ifdef PSX2_TARGET
#define M_Photolog	"host0:X:/PhotoLog.txt"
#else
#define M_Photolog	"X:/PhotoLog.txt"
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void IMG_Log4Debugging(char *sz_Text)
{
	/*~~~~~~~~~~~*/
	L_FILE	x_File;
	/*~~~~~~~~~~~*/

	x_File = L_fopen(M_Photolog, L_fopen_AB);
	if(x_File)
	{
		L_fwrite(sz_Text, L_strlen(sz_Text), 1, x_File);
		L_fclose(x_File);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG IMG_ModifierObjectAnalyser
(
	OBJ_tdst_GameObject		*_pst_GO,
	MDF_tdst_Modifier		*pst_Modifier,
	WOR_tdst_World			*_pst_World,
	GDI_tdst_DisplayData	*_pst_DD,
	ULONG					_ul_MissionMask,
	BOOL					_b_RayCast,
	BOOL					_b_NoDecal,
	BOOL					_b_SkipMissionTest,
	ULONG					*_pul_Return
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierPhoto	*pst_ModPhoto;
	OBJ_tdst_GameObject		*pst_SnapGO, *pst_RayGO;
	MATH_tdst_Vector		st_Dir, st_Origin;
	MATH_tdst_Vector		st_GCS_Center;
	float					f_DistRay;
	int						i_Info;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ModPhoto = (GAO_tdst_ModifierPhoto *) pst_Modifier->p_Data;

	/*
	 * If _b_SkipMissionTest is set, we just want to know if the current GO is well
	 * centred in the screen, however it has photo value or not.
	 */
	if(!_b_SkipMissionTest)
	{
		i_Info = pst_ModPhoto->i_Info;

		if(pst_ModPhoto->i_Mission == 0) return 0;

		if((_ul_MissionMask != (ULONG) - 1) && (_ul_MissionMask != 0))
		{
			if(((pst_ModPhoto->i_Mission << 24) & 0xFF000000) == _ul_MissionMask)
			{
				if(_b_NoDecal)
					i_Info = i_Info;
				else
					i_Info = 1 << i_Info;
			}
			else
			{
				return 0;
			}
		}
	}

	if
	(
		(pst_ModPhoto->f_CurrentFrame < pst_ModPhoto->f_FrameMin)
	||	(pst_ModPhoto->f_CurrentFrame > pst_ModPhoto->f_FrameMax)
	||	(pst_ModPhoto->ul_Flags & MDF_InfoPhoto_ForceBadFrame)
	)
	{
#ifdef ACTIVE_EDITORS
		if(COL_b_LogPhoto)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf
			(
				asz_Log,
				"[MDF][MISSED] %s  %%BV[%0.1f%% < %0.1f%%] or [%0.1f%% > %0.1f%%]",
				pst_Modifier->pst_GO->sz_Name,
				pst_ModPhoto->f_CurrentFrame * 100.0f,
				pst_ModPhoto->f_FrameMin,
				pst_ModPhoto->f_CurrentFrame * 100.0f,
				pst_ModPhoto->f_FrameMax
			);
			LINK_PrintStatusMsg(asz_Log);
		}

#endif
		if(IMG_Log)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf
			(
				asz_Log,
				"[MDF][MISSED] %%BV[%0.1f%% < %0.1f%%] or [%0.1f%% > %0.1f%%]",
				pst_ModPhoto->f_CurrentFrame * 100.0f,
				pst_ModPhoto->f_FrameMin,
				pst_ModPhoto->f_CurrentFrame * 100.0f,
				pst_ModPhoto->f_FrameMax
			);
			IMG_Log4Debugging(asz_Log);
		}

		if(_pul_Return) *_pul_Return = IMG_CENTER;
		return 0;
	}

	if(
			(pst_ModPhoto->f_CurrentLOD < pst_ModPhoto->f_LODMin) 
		||	(pst_ModPhoto->f_CurrentLOD > pst_ModPhoto->f_LODMax)
		||	(pst_ModPhoto->ul_Flags & MDF_InfoPhoto_ForceBadLOD)
	)
	{
#ifdef ACTIVE_EDITORS
		if(COL_b_LogPhoto)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf
			(
				asz_Log,
				"[MDF][MISSED] %s %%Screen[%0.1f%% < %0.1f%%] or [%0.1f%% > %0.1f%%]",
				pst_Modifier->pst_GO->sz_Name,
				pst_ModPhoto->f_CurrentLOD * 100.0f,
				pst_ModPhoto->f_FrameMin * 100.0f,
				pst_ModPhoto->f_CurrentLOD * 100.0f,
				pst_ModPhoto->f_FrameMax * 100.0f
			);
			LINK_PrintStatusMsg(asz_Log);
		}

#endif
		if(IMG_Log)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf
			(
				asz_Log,
				"[MDF][MISSED] %%Screen[%0.1f%% < %0.1f%%] or [%0.1f%% > %0.1f%%]",
				pst_ModPhoto->f_CurrentLOD * 100.0f,
				pst_ModPhoto->f_FrameMin * 100.0f,
				pst_ModPhoto->f_CurrentLOD * 100.0f,
				pst_ModPhoto->f_FrameMax * 100.0f
			);
			IMG_Log4Debugging(asz_Log);
		}

		if(_pul_Return)
		{
			if((pst_ModPhoto->f_CurrentLOD < pst_ModPhoto->f_LODMin) || (pst_ModPhoto->ul_Flags & MDF_InfoPhoto_ForceBadLOD))
				*_pul_Return = IMG_TINY;
			else
				*_pul_Return = IMG_HUGE;
		}

		return 0;
	}

	pst_SnapGO = GAO_ModifierPhoto_SnapGOGet(pst_Modifier->pst_GO, pst_ModPhoto, 0);

	MATH_CopyVector(&st_Origin, &_pst_DD->st_Camera.st_Matrix.T);

	MATH_TransformVertex(&st_GCS_Center, pst_SnapGO->pst_GlobalMatrix, &((GAO_tdst_ModifierPhoto *)pst_Modifier->p_Data)->st_SphereOffset);

	MATH_SubVector(&st_Dir, &st_GCS_Center, &st_Origin);

	/* Adds a 2cm certical Offset to avoid Objects that are snaped perfectly on the ground. We want to collide them with the Ray, not with the Ground */
	st_Dir.z += 0.02f;

	f_DistRay = MATH_f_NormVector(&st_Dir);
	MATH_NormalizeEqualVector(&st_Dir);

	/* We may not want to cast a ray. */

	/* if(!_b_RayCast) return(1 << (_pst_GO->pst_Extended->pst_Design->i3 & 0xFF) ); */
	if(!_b_RayCast)
	{
		return (_b_SkipMissionTest) ? 1 : i_Info;
	}

	pst_RayGO = COL_Visual_RayCast
		(
			_pst_World,
			AI_gpst_MainActors[0],
			&st_Origin,
			&st_Dir,
			f_DistRay + 0.01f,
			-1,
			0,
			OBJ_C_OR_OR_IdentityFlags,
			NULL,
			TRUE,
			FALSE
#ifdef JADEFUSION			
			,FALSE
#endif
		);

	if(!pst_RayGO || (pst_RayGO == _pst_GO))
	{
#ifdef ACTIVE_EDITORS
		if(COL_b_LogPhoto)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf
			(
				asz_Log,
				"[MDF][Photo] %s %%BV[%0.1f%%], %%Screen[%0.1f%%]",
				_pst_GO->sz_Name,
				pst_ModPhoto->f_CurrentFrame * 100.0f,
				pst_ModPhoto->f_CurrentLOD * 100.0f
			);
			LINK_PrintStatusMsg(asz_Log);
		}

#endif
		if(IMG_Log)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf
			(
				asz_Log,
				"[MDF][Photo] %%BV[%0.1f%%], %%Screen[%0.1f%%]\n",
				pst_ModPhoto->f_CurrentFrame * 100.0f,
				pst_ModPhoto->f_CurrentLOD * 100.0f
			);
			IMG_Log4Debugging(asz_Log);
		}

		return (_b_SkipMissionTest) ? 1 : i_Info;
	}
	else
	{
#ifdef ACTIVE_EDITORS
		if(COL_b_LogPhoto)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf
			(
				asz_Log,
				"[MDF][MISSED] %s [RayCast = %s]",
				pst_Modifier->pst_GO,
				pst_RayGO ? pst_RayGO->sz_Name : "NULL"
			);
			LINK_PrintStatusMsg(asz_Log);
		}

#endif
		if(_pul_Return) *_pul_Return = IMG_RAY;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG IMG_ObjectAnalyser
(
	OBJ_tdst_GameObject		*_pst_GO,
	WOR_tdst_World			*_pst_World,
	GDI_tdst_DisplayData	*_pst_DD,
	ULONG					_ul_MissionMask,
	BOOL					_b_RayCast,
	BOOL					_b_NoDecal,
	BOOL					_b_SkipMissionTest,
	ULONG					*_pul_Return
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_RayGO, *pst_HeadGO;
	MDF_tdst_Modifier	*pst_Modifier;
	MATH_tdst_Vector	st_Dir, st_Origin, st_BV_Center;
	MATH_tdst_Vector	st_Max, st_Min;
	float				f_PercentBV, f_PercentScreen, f_DistRay;
	int					i_Info;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pul_Return) *_pul_Return = 0;

	/* Not visible object returns 0 */
	if(!(_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible)) return 0;

	/*
	 * First, look for Active Info-Photo Modifier. If any, update it and call another
	 * Algo.
	 */
	pst_Modifier = GAO_ModifierPhoto_Get(_pst_GO, TRUE);
	if(pst_Modifier)
	{
		return
			(
				IMG_ModifierObjectAnalyser
				(
					_pst_GO,
					pst_Modifier,
					_pst_World,
					_pst_DD,
					_ul_MissionMask,
					_b_RayCast,
					_b_NoDecal,
					_b_SkipMissionTest,
					_pul_Return
				)
			);
	}

	/* No Active Info-Photo Modifier, Old Bounding Volume Algo then ... */
	if
	(
		(!_pst_GO->pst_Extended)
	||	(!_b_SkipMissionTest && !_pst_GO->pst_Extended->pst_Design)
	||	(!_b_SkipMissionTest && !OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_DesignStruct))
	)
	{
		return 0;
	}

	/*
	 * If _b_SkipMissionTest is set, we just want to know if the current GO is well
	 * centred in the screen, however it has photo value or not.
	 */
	if(!_b_SkipMissionTest)
	{
		i_Info = _pst_GO->pst_Extended->pst_Design->i3;

		if((i_Info & 0xFF000000) == 0)
		{
			return 0;
		}

		if((_ul_MissionMask != (ULONG) - 1) && (_ul_MissionMask != 0))
		{
			if((i_Info & 0xFF000000) == _ul_MissionMask)
			{
				if(_b_NoDecal)
					i_Info = (i_Info & 0xFF);
				else
					i_Info = 1 << (i_Info & 0xFF);
			}
			else
			{
				return 0;
			}
		}
	}

	MATH_CopyMatrix(_pst_DD->st_Camera.pst_ObjectToCameraMatrix, &_pst_DD->st_Camera.st_InverseMatrix);

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		pst_HeadGO = ANI_pst_GetObjectByAICanal(_pst_GO, 0);

		if(pst_HeadGO && OBJ_BV_IsAABBox(pst_HeadGO->pst_BV))
		{
			MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_HeadGO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_HeadGO));
			MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_HeadGO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_HeadGO));
		}
		else
		{
			if(!OBJ_BV_IsAABBox(_pst_GO->pst_BV))
			{
				return 0;
			}

			MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
			MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
		}
	}
	else
	{
		if(!OBJ_BV_IsAABBox(_pst_GO->pst_BV))
		{
			return 0;
		}

		MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
		MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(_pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
	}

	OBJ_Frame_CullingAABBox
	(
		&st_Min,
		&st_Max,
		&_pst_DD->st_Camera,
		IMG_FotoScreenX,
		IMG_FotoScreenY,
		&f_PercentBV,
		&f_PercentScreen
	);

	/* Object Culled ... */
	if(f_PercentBV == 0.0f)
	{
		if(_pul_Return) *_pul_Return = 2;	
		return 0;
	}

	/* If less than 2.5% of the Frame-Screen is covered by this BV, we return 0 */
	if(f_PercentScreen < 0.025f)
	{
#ifdef ACTIVE_EDITORS
		if(COL_b_LogPhoto)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Log, "[MISSED] %s %%Screen[%0.1f%% < 2.5%%]", _pst_GO->sz_Name, f_PercentScreen * 100.0f);
			LINK_PrintStatusMsg(asz_Log);
		}

#endif
		if(IMG_Log)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Log, "[MISSED] %%Screen[%0.1f%% < 2.5%%]\n", f_PercentScreen * 100.0f);
			IMG_Log4Debugging(asz_Log);
		}

		if(_pul_Return) *_pul_Return = 1;
		return 0;
	}

	/* If less than 50% of the BV is inside the Frame, we return 0; */
	if(f_PercentBV < 0.50f)
	{
#ifdef ACTIVE_EDITORS
		if(COL_b_LogPhoto)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Log, "[MISSED] %s  %%BV[%0.1f%% < 50%%]", _pst_GO->sz_Name, f_PercentBV * 100.0f);
			LINK_PrintStatusMsg(asz_Log);
		}

#endif
		if(IMG_Log)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Log, "[MISSED] %%BV[%0.1f%% < 50%%]\n", f_PercentBV * 100.0f);
			IMG_Log4Debugging(asz_Log);
		}

		if(_pul_Return) *_pul_Return = 2;
		return 0;
	}

	MATH_CopyVector(&st_Origin, &_pst_DD->st_Camera.st_Matrix.T);

	if(pst_HeadGO && OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		OBJ_BV_ComputeCenter(pst_HeadGO, &st_BV_Center);
	}
	else
	{
		OBJ_BV_ComputeCenter(_pst_GO, &st_BV_Center);
	}

	MATH_SubVector(&st_Dir, &st_BV_Center, &st_Origin);

	/* Adds a 2cm certical Offset to avoid Objects that are snaped perfectly on the ground. We want to collide them with the Ray, not with the Ground */
	st_Dir.z += 0.02f;

	f_DistRay = MATH_f_NormVector(&st_Dir);
	MATH_NormalizeEqualVector(&st_Dir);

	/* We may not want to cast a ray. */
	if(!_b_RayCast)
	{
		return (_b_SkipMissionTest) ? 1 : i_Info;
	}

	pst_RayGO = COL_Visual_RayCast
		(
			_pst_World,
			AI_gpst_MainActors[0],
			&st_Origin,
			&st_Dir,
			f_DistRay + 0.01f,
			-1,
			0,
			OBJ_C_OR_OR_IdentityFlags,
			NULL,
			TRUE,
			FALSE
#ifdef JADEFUSION
			,FALSE
#endif
		);

	if(!pst_RayGO || (pst_RayGO == _pst_GO))
	{
#ifdef ACTIVE_EDITORS
		if(COL_b_LogPhoto)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf
			(
				asz_Log,
				"[Photo] %s %%BV[%0.1f%%], %%Screen[%0.1f%%]",
				_pst_GO->sz_Name,
				f_PercentBV * 100.0f,
				f_PercentScreen * 100.0f
			);
			LINK_PrintStatusMsg(asz_Log);
		}

#endif
		if(IMG_Log)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf
			(
				asz_Log,
				"[Photo] %%BV[%0.1f%%], %%Screen[%0.1f%%]\n",
				f_PercentBV * 100.0f,
				f_PercentScreen * 100.0f
			);
			IMG_Log4Debugging(asz_Log);
		}

		return (_b_SkipMissionTest) ? 1 : i_Info;
	}

	else
	{
#ifdef ACTIVE_EDITORS
		if(COL_b_LogPhoto)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Log, "[MISSED] %s [RayCast = %s]", _pst_GO->sz_Name, pst_RayGO ? pst_RayGO->sz_Name : "NULL");
			LINK_PrintStatusMsg(asz_Log);
		}
#endif

		if(_pul_Return) *_pul_Return = 4;
	}

	return 0;
}

ULONG	IMG_PhotoSummary;
ULONG	IMG_Gao;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG IMG_Analyser(WOR_tdst_World *_pst_World, int _i_Mission)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_DisplayData	*pst_DD;
	TAB_tdst_PFtable		*pst_Table;
	TAB_tdst_PFelem			*pst_CurrentElem;
	TAB_tdst_PFelem			*pst_EndElem;
	OBJ_tdst_GameObject		*pst_CurrentGO;
	ULONG					ul_Mask, ul_GOMask;
	ULONG					ul_MissionMask;
	UCHAR					uc_Tiny, uc_Center;
	ULONG					ul_Result;
	BOOL					b_NoDecal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* When the mission entered is -1, we return the Last photo summary. */
	if(_i_Mission == -1) return IMG_PhotoSummary;

	ul_Mask = 0;
	uc_Tiny = 0;
	uc_Center = 0;

	ul_MissionMask = (ULONG) (_i_Mission << 24);
	b_NoDecal = (_i_Mission & 0x100) ? TRUE : FALSE;

	if(!_pst_World) return 0;
	IMG_PhotoSummary = 0;

#ifdef ACTIVE_EDITORS
	COL_b_LogPhoto = TRUE;
#endif
#if (!defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_XENON))
	if(GetAsyncKeyState(VK_SHIFT) < 0)
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[500];
		/*~~~~~~~~~~~~~~~~~*/

		sprintf(asz_Log, "****** Analyse for mission %d *********\n", _i_Mission);
		IMG_Log4Debugging(asz_Log);
		IMG_Log = 1;
	}
	else
#endif
		IMG_Log = 0;

	pst_DD = (GDI_tdst_DisplayData*)_pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas;

	/* We want to loop thru all the Visible objects of this world. */
	pst_Table = &_pst_World->st_VisibleObjects;

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Table);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if(TAB_b_IsAHole(pst_CurrentGO)) continue;
		if((pst_CurrentGO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled)) continue;

		ul_GOMask = IMG_ObjectAnalyser
			(
				pst_CurrentGO,
				_pst_World,
				pst_DD,
				ul_MissionMask,
				TRUE,
				b_NoDecal,
				FALSE,
				&ul_Result
			);

		switch(ul_Result)
		{
		case IMG_TINY:
		case IMG_HUGE:
			IMG_PhotoSummary += (1 << 8);
			break;

		case IMG_CENTER:
			IMG_PhotoSummary++;
			break;
		}

		if((ul_GOMask & 0xFF000000) || (b_NoDecal && ul_GOMask))
		{
#ifdef ACTIVE_EDITORS
			COL_b_LogPhoto = FALSE;
#endif
			return ul_GOMask;
		}

		ul_Mask |= ul_GOMask;
	}

	if(IMG_Log)
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[500];
		/*~~~~~~~~~~~~~~~~~*/

		sprintf(asz_Log, "==> Result = %08X (%08X)\n", ul_Mask, IMG_PhotoSummary);
		IMG_Log4Debugging(asz_Log);
	}

#ifdef ACTIVE_EDITORS
	COL_b_LogPhoto = FALSE;
#endif
	return ul_Mask;
}


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    New image analyser
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#define IMG_MaskGetType(_Mask, _i)	((_Mask >> (_i * 8)) & 0x1F)

#define IMG_MaskSetType(_Mask, _i, _type) \
	{ \
		_Mask &= ~(0xFF << (_i * 8)); \
		_Mask |= ((1 << 5) + _type) << (_i * 8); \
	}

#define IMG_MaskIncNumber(_Mask, _i) \
	{ \
		int _temp; \
		_temp = (_Mask >> ((_i * 8) + 5)) & 7; \
		if(_temp < 7) \
		{ \
			_temp++; \
			_Mask &= ~(7 << ((_i * 8) + 5)); \
			_Mask |= _temp << ((_i * 8) + 5); \
		} \
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void IMG_LogOutput(char msg, OBJ_tdst_GameObject *_pst_GO, float f1, float f2, float f3, float f4)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	asz_Log[500];
	/*~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	if(!IMG_Log && !COL_b_LogPhoto) return;
#else
	if(!IMG_Log) return;
#endif
	switch(msg)
	{
	case 0:
		sprintf(asz_Log, "[MISSED] %%BV[%0.1f%% < 50%%]\n", f1 * 100.0f);
		break;
	case 1:
		sprintf(asz_Log, "[MISSED] %%Screen[%0.1f%% < 2.5%%]", f2 * 100.0f);
		break;
	case 2:
		sprintf(asz_Log, "[PHOTO] %%BV[%0.1f%%], %%Screen[%0.1f%%]\n", f1 * 100.0f, f2 * 100.0f);
		break;
	case 3:
		sprintf
		(
			asz_Log,
			"[MDF][MISSED] %%BV[%0.1f%% < %0.1f%%] or [%0.1f%% > %0.1f%%]",
			f1 * 100.0f,
			f2 * 100.0f,
			f3 * 100.0f,
			f4 * 100.0f
		);
		break;
	case 4:
		sprintf
		(
			asz_Log,
			"[MDF][MISSED] %%Screen[%0.1f%% < %0.1f%%] or [%0.1f%% > %0.1f%%]",
			f1 * 100.0f,
			f2 * 100.0f,
			f3 * 100.0f,
			f4 * 100.0f
		);
		break;
	case 5:
		sprintf(asz_Log, "[MDF][Photo] %%BV[%0.1f%%], %%Screen[%0.1f%%]", f1 * 100.0f, f2 * 100.0f);
		break;
	}

	if(IMG_Log) IMG_Log4Debugging(asz_Log);

#ifdef ACTIVE_EDITORS
	if(COL_b_LogPhoto)
	{
		if(_pst_GO->sz_Name) strcat(asz_Log, _pst_GO->sz_Name);
		LINK_PrintStatusMsg(asz_Log);
	}

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG IMG_ModifierObjectAnalyser_Bis
(
	OBJ_tdst_GameObject		*_pst_GO,
	MDF_tdst_Modifier		*pst_Modifier,
	WOR_tdst_World			*_pst_World,
	GDI_tdst_DisplayData	*_pst_DD,
	BOOL					_b_RayCast,
	BOOL					_b_SkipMissionTest,
	ULONG					*_pul_Return
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierPhoto	*pst_ModPhoto;
	OBJ_tdst_GameObject		*pst_SnapGO, *pst_RayGO;
	MATH_tdst_Vector		st_Dir, st_Origin;
	float					f_DistRay;
	int						i_Info;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ModPhoto = (GAO_tdst_ModifierPhoto *) pst_Modifier->p_Data;

	/*
	 * If _b_SkipMissionTest is set, we just want to know if the current GO is well
	 * centred in the screen, however it has photo value or not.
	 */
	if(_b_SkipMissionTest)
	{
		i_Info = 1;
	}
	else
	{
		if(pst_ModPhoto->i_Mission == 0) return 0;
		i_Info = (pst_ModPhoto->i_Mission << 24) | pst_ModPhoto->i_Info;
	}

	if
	(
		(pst_ModPhoto->f_CurrentFrame < pst_ModPhoto->f_FrameMin)
	||	(pst_ModPhoto->f_CurrentFrame > pst_ModPhoto->f_FrameMax)
	||	(pst_ModPhoto->ul_Flags & MDF_InfoPhoto_ForceBadFrame)
	)
	{
		IMG_LogOutput
		(
			3,
			pst_Modifier->pst_GO,
			pst_ModPhoto->f_CurrentFrame,
			pst_ModPhoto->f_FrameMin,
			pst_ModPhoto->f_CurrentFrame,
			pst_ModPhoto->f_FrameMax
		);
		if(_pul_Return) *_pul_Return = IMG_CENTER;
		i_Info = 0;
	}

	if
	(
		(pst_ModPhoto->f_CurrentLOD < pst_ModPhoto->f_LODMin) 
	||	(pst_ModPhoto->f_CurrentLOD > pst_ModPhoto->f_LODMax)
	||	(pst_ModPhoto->ul_Flags & MDF_InfoPhoto_ForceBadLOD)
	)

	{
		IMG_LogOutput
		(
			4,
			pst_Modifier->pst_GO,
			pst_ModPhoto->f_CurrentLOD,
			pst_ModPhoto->f_FrameMin,
			pst_ModPhoto->f_CurrentLOD,
			pst_ModPhoto->f_FrameMax
		);
		if(_pul_Return)
		{
			if((pst_ModPhoto->f_CurrentLOD < pst_ModPhoto->f_LODMin) || (pst_ModPhoto->ul_Flags & MDF_InfoPhoto_ForceBadLOD))
				*_pul_Return = IMG_TINY;
			else
				*_pul_Return = IMG_HUGE;
		}

		i_Info = 0;
	}

	/* no ray casting */
	if(!_b_RayCast) return i_Info;

	/* cast ray */
	pst_SnapGO = GAO_ModifierPhoto_SnapGOGet(pst_Modifier->pst_GO, pst_ModPhoto, 0);
	MATH_CopyVector(&st_Origin, &_pst_DD->st_Camera.st_Matrix.T);
	MATH_SubVector(&st_Dir, &pst_SnapGO->pst_GlobalMatrix->T, &st_Origin);

	/* Adds a 2cm certical Offset to avoid Objects that are snaped perfectly on the ground. We want to collide them with the Ray, not with the Ground */
	st_Dir.z += 0.02f;

	f_DistRay = MATH_f_NormVector(&st_Dir);
	MATH_NormalizeEqualVector(&st_Dir);

	pst_RayGO = COL_Visual_RayCast
		(
			_pst_World,
			AI_gpst_MainActors[0],
			&st_Origin,
			&st_Dir,
			f_DistRay + 0.01f,
			-1,
			0,
			OBJ_C_OR_OR_IdentityFlags,
			NULL,
			TRUE,
			FALSE
#ifdef JADEFUSION
			,FALSE
#endif
		);

	if(!pst_RayGO || (pst_RayGO == _pst_GO))
	{
		IMG_LogOutput(5, pst_Modifier->pst_GO, pst_ModPhoto->f_CurrentFrame, pst_ModPhoto->f_CurrentLOD, 0, 0);
		return i_Info;
	}
	else
	{
#ifdef ACTIVE_EDITORS
		if(COL_b_LogPhoto)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf
			(
				asz_Log,
				"[MDF][MISSED] %s [RayCast = %s]",
				pst_Modifier->pst_GO,
				pst_RayGO ? pst_RayGO->sz_Name : "NULL"
			);
			LINK_PrintStatusMsg(asz_Log);
		}

#endif
		if(_pul_Return) *_pul_Return = IMG_RAY;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG IMG_ObjectAnalyser_Bis
(
	OBJ_tdst_GameObject		*_pst_GO,
	WOR_tdst_World			*_pst_World,
	GDI_tdst_DisplayData	*_pst_DD,
	BOOL					_b_RayCast,
	BOOL					_b_SkipMissionTest,
	ULONG					*_pul_Return
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_RayGO, *pst_HeadGO, *pst_BVGO;
	MDF_tdst_Modifier	*pst_Modifier;
	MATH_tdst_Vector	st_Dir, st_Origin, st_BV_Center;
	MATH_tdst_Vector	st_Max, st_Min;
	float				f_PercentBV, f_PercentScreen, f_DistRay;
	int					i_Info;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pul_Return) *_pul_Return = 0;

	/*
	 * First, look for Active Info-Photo Modifier. If any, update it and call another
	 * Algo.
	 */
	pst_Modifier = GAO_ModifierPhoto_Get(_pst_GO, TRUE);
	if(pst_Modifier)
	{
		return
			(
				IMG_ModifierObjectAnalyser_Bis
				(
					_pst_GO,
					pst_Modifier,
					_pst_World,
					_pst_DD,
					_b_RayCast,
					_b_SkipMissionTest,
					_pul_Return
				)
			);
	}

	/* No Active Info-Photo Modifier, Old Bounding Volume Algo then ... */
	if(!_pst_GO->pst_Extended) return 0;
	if(!_b_SkipMissionTest && !_pst_GO->pst_Extended->pst_Design) return 0;
	if(!_b_SkipMissionTest && !OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_DesignStruct)) return 0;

	/*
	 * If _b_SkipMissionTest is set, we just want to know if the current GO is well
	 * centred in the screen, however it has photo value or not.
	 */
	if(_b_SkipMissionTest)
	{
		i_Info = 1;
	}
	else
	{
		i_Info = _pst_GO->pst_Extended->pst_Design->i3;
		if((i_Info & 0xFF000000) == 0) return 0;
	}

	MATH_CopyMatrix(_pst_DD->st_Camera.pst_ObjectToCameraMatrix, &_pst_DD->st_Camera.st_InverseMatrix);

	pst_BVGO = _pst_GO;
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		pst_HeadGO = ANI_pst_GetObjectByAICanal(_pst_GO, 0);
		if(pst_HeadGO && OBJ_BV_IsAABBox(pst_HeadGO->pst_BV)) pst_BVGO = pst_HeadGO;
	}

	if(!OBJ_BV_IsAABBox(pst_BVGO->pst_BV)) return 0;
	MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_BVGO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_BVGO));
	MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_BVGO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_BVGO));

	OBJ_Frame_CullingAABBox
	(
		&st_Min,
		&st_Max,
		&_pst_DD->st_Camera,
		IMG_FotoScreenX,
		IMG_FotoScreenY,
		&f_PercentBV,
		&f_PercentScreen
	);

	/* Object Culled ... */
	if(f_PercentBV == 0.0f) 
	{
		if(_pul_Return) *_pul_Return = 2;	
		return 0;
	}

	/* If less than 2.5% of the Frame-Screen is covered by this BV, we return 0 */
	if(f_PercentScreen < 0.025f)
	{
		IMG_LogOutput(1, _pst_GO, f_PercentBV, f_PercentScreen, 0, 0);
		if(_pul_Return)
		{
			if(*_pul_Return == 2)
			{
				if((f_PercentScreen / f_PercentBV) < 0.025f) *_pul_Return = 1;
			}
			else
				*_pul_Return = 1;
		}

		i_Info = 0;

		/* return 0; */
	}

	/* If less than 50% of the BV is inside the Frame, we return 0; */
	if(f_PercentBV < 0.50f)
	{
		IMG_LogOutput(0, _pst_GO, f_PercentBV, f_PercentScreen, 0, 0);
		if(_pul_Return) *_pul_Return = 2;
		i_Info = 0;

		/* return 0; */
	}

	/* We may not want to cast a ray. */
	if(!_b_RayCast) return i_Info;

	/* case ray */
	MATH_CopyVector(&st_Origin, &_pst_DD->st_Camera.st_Matrix.T);
	OBJ_BV_ComputeCenter(pst_BVGO, &st_BV_Center);
	MATH_SubVector(&st_Dir, &st_BV_Center, &st_Origin);

	/* Adds a 2cm certical Offset to avoid Objects that are snaped perfectly on the ground. We want to collide them with the Ray, not with the Ground */
	st_Dir.z += 0.02f;

	f_DistRay = MATH_f_NormVector(&st_Dir);
	MATH_NormalizeEqualVector(&st_Dir);

	pst_RayGO = COL_Visual_RayCast
		(
			_pst_World,
			AI_gpst_MainActors[0],
			&st_Origin,
			&st_Dir,
			f_DistRay + 0.01f,
			-1,
			0,
			OBJ_C_OR_OR_IdentityFlags,
			NULL,
			TRUE,
			FALSE
#ifdef JADEFUSION
			,FALSE
#endif
		);

	if(!pst_RayGO || (pst_RayGO == _pst_GO))
	{
		IMG_LogOutput(2, _pst_GO, f_PercentBV, f_PercentScreen, 0, 0);
		return i_Info;
	}
	else
	{
#ifdef ACTIVE_EDITORS
		if(COL_b_LogPhoto)
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	asz_Log[500];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(asz_Log, "[MISSED] %s [RayCast = %s]", _pst_GO->sz_Name, pst_RayGO ? pst_RayGO->sz_Name : "NULL");
			LINK_PrintStatusMsg(asz_Log);
		}

#endif
		if(_pul_Return) *_pul_Return = 4;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG IMG_Analyser_Bis(WOR_tdst_World *_pst_World, int _i_Request)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_DisplayData	*pst_DD;
	TAB_tdst_PFtable		*pst_Table;
	TAB_tdst_PFelem			*pst_CurrentElem;
	TAB_tdst_PFelem			*pst_EndElem;
	OBJ_tdst_GameObject		*pst_CurrentGO;
	ULONG					ul_Mask, ul_GOMask;
	UCHAR					uc_Tiny, uc_Center;
	CHAR					c_Report, ac_Type[3]; //, ac_Number[3];
	ULONG					ul_Result;
	int						i; //, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_World) return 0;

	/* When the request entered is -1, we return the Last photo summary. */
	if(_i_Request == -1) return IMG_PhotoSummary;
	if(_i_Request == -2) return IMG_Gao;

	IMG_Gao = 0;
	c_Report = 0;
	ul_Mask = 0;
	uc_Tiny = 0;
	uc_Center = 0;
	IMG_PhotoSummary = 0;
	pst_DD = (GDI_tdst_DisplayData*)_pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas;
	ac_Type[0] = ac_Type[1] = ac_Type[2] = 0;
	//ac_Number[0] = ac_Number[1] = ac_Number[2] = 0;

#ifdef ACTIVE_EDITORS
	COL_b_LogPhoto = TRUE;
#endif
	IMG_Log = 0;
#if (!defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_XENON))
	if(GetAsyncKeyState(VK_SHIFT) < 0)
	{
		IMG_Log4Debugging("****** photo analyser *********\n");
		IMG_Log = 1;
	}

#endif
	/* We want to loop thru all the Visible objects of this world. */
	pst_Table = &_pst_World->st_VisibleObjects;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Table);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if(TAB_b_IsAHole(pst_CurrentGO)) continue;
		if(pst_CurrentGO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled) continue;
		if(!(pst_CurrentGO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible)) continue;

		ul_GOMask = IMG_ObjectAnalyser_Bis(pst_CurrentGO, _pst_World, pst_DD, TRUE, FALSE, &ul_Result);

		switch(ul_Result)
		{
		case IMG_TINY:
		case IMG_HUGE:
			IMG_PhotoSummary += (1 << 8);
			break;

		case IMG_CENTER:
			IMG_PhotoSummary++;
			break;
		}

		if(ul_GOMask == 0) continue;

		/* objet spécial : carte ou code */
		if((ul_GOMask >> 24) >= 100)
		{
			IMG_Gao = (ULONG) pst_CurrentGO;
			ul_Mask = ul_GOMask;
			goto IMG_Analyser_Bis_end;
		}

		/* animaux */
		if((ul_GOMask >> 24) == 4)
		{
			if(c_Report) continue;
			ul_Mask = ul_GOMask;
			goto IMG_Analyser_Bis_end;
		}

		/* photo reportages */
		ul_GOMask &= 0xFF;
		for(i = 0; i < 3; i++)
		{
			if(ac_Type[i] == 0)
			{
				ac_Type[i] = (char) ul_GOMask;
				break;
			}

			if((char) ul_GOMask == ac_Type[i])
				break;
		}
	}

	for(i = 2; i >= 0; i--)
	{
		ul_Mask <<= 8;
		ul_Mask |= ac_Type[i];
	}

	if(ul_Mask == 0)
		ul_Mask = IMG_PhotoSummary ? (0xFF000000 + IMG_PhotoSummary) : 0;
	else
		ul_Mask |= (1 << 24);

	if(IMG_Log)
	{
		/*~~~~~~~~~~~~~~~~~*/
		char	asz_Log[500];
		/*~~~~~~~~~~~~~~~~~*/

		sprintf(asz_Log, "==> Result = %08X (%08X)\n", ul_Mask, IMG_PhotoSummary);
		IMG_Log4Debugging(asz_Log);
	}

IMG_Analyser_Bis_end:
#ifdef ACTIVE_EDITORS
	COL_b_LogPhoto = FALSE;
#endif
	return ul_Mask;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
