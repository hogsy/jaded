/*$T COLzdx.c GC! 1.081 11/05/01 11:45:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$F GC Dependencies 11/22/99 */
#include "Precomp.h"

#include "BASe/BAStypes.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"

#include "MATHs/MATH.h"

#include "OBJects/OBJconst.h"
#include "OBJects/OBJaccess.h"
#include "OBJects/OBJorient.h"

#include "INTersection/INTmain.h"

#include "WORld/WORstruct.h"

#include "COLstruct.h"
#include "COLconst.h"
#include "COLvars.h"
#include "COLaccess.h"
#include "COLreport.h"
#include "COLcob.h"

#include "GraphicDK/Sources/GEOmetric/GEOobject.h"
#include "SDK/Sources/TIMer/TIMdefs.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
extern ULONG	COL_ul_ParsedFaces;
extern ULONG	COL_ul_ParsedFacesOK3;
extern ULONG	COL_ul_ComputedFaces;
#endif

/**
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_Box_Triangle
(
	COL_tdst_GlobalVars			*_pst_GlobalVars,
	COL_tdst_IndexedTriangle	*_pst_Triangle,
	USHORT						_uw_T1,
	USHORT						_uw_T2,
	USHORT						_uw_T3,
	MATH_tdst_Vector			*_pst_Normal,
	ULONG						_ul_Triangle,
	ULONG						_ul_Element,
	BOOL						_b_Dynamic
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_NumberOfHits, i;
	MATH_tdst_Matrix	st_InvAMatrix;
	MATH_tdst_Vector	*pst_T1, *pst_T2, *pst_T3;
	MATH_tdst_Vector	st_ACS_T1, st_ACS_T2, st_ACS_T3, st_ACS_Normal;
	MATH_tdst_Vector	st_GCS_T1, st_GCS_T2, st_GCS_T3, st_GCS_Normal;
	MATH_tdst_Vector	st_BCS_Static_Hit;
	MATH_tdst_Vector	ast_ACS_Static_Hit[8];
	ULONG				aul_CollisionType[8];
	MATH_tdst_Vector	st_Global_Norm, st_Global_Recal, st_Global_Hit;
	float				f_MaxRecal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_MaxRecal = -100000.0f;

	pst_T1 = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point[_uw_T1];
	pst_T2 = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point[_uw_T2];
	pst_T3 = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point[_uw_T3];

	MATH_TransformVertexNoScale(&st_GCS_T1, _pst_GlobalVars->pst_B_GlobalMatrix, pst_T1);
	MATH_TransformVertexNoScale(&st_GCS_T2, _pst_GlobalVars->pst_B_GlobalMatrix, pst_T2);
	MATH_TransformVertexNoScale(&st_GCS_T3, _pst_GlobalVars->pst_B_GlobalMatrix, pst_T3);
	MATH_TransformVectorNoScale(&st_GCS_Normal, _pst_GlobalVars->pst_B_GlobalMatrix, _pst_Normal);

	MATH_InvertMatrix(&st_InvAMatrix, _pst_GlobalVars->pst_A_GlobalMatrix);

	MATH_TransformVertexNoScale(&st_ACS_T1, &st_InvAMatrix, &st_GCS_T1);
	MATH_TransformVertexNoScale(&st_ACS_T2, &st_InvAMatrix, &st_GCS_T2);
	MATH_TransformVertexNoScale(&st_ACS_T3, &st_InvAMatrix, &st_GCS_T3);
	MATH_TransformVectorNoScale(&st_ACS_Normal, &st_InvAMatrix, &st_GCS_Normal);

	/* Initialisation of Box Hits to (0, 0, 0) */
	L_memset(ast_ACS_Static_Hit, 0, 8 * sizeof(MATH_tdst_Vector *));

	/*$F ----------------------- DYNAMIC COLLISION ----------------------- */
	/*								A FAIRE								   */
	/*$F ----------------------------------------------------------------- */


	/*$F ----------------------- STATIC COLLISION ----------------------- */
	if
	(
		INT_FullAABBoxTriangle
		(
			&st_ACS_T1,
			&st_ACS_T2,
			&st_ACS_T3,
			&st_ACS_Normal,
			COL_pst_Shape_GetMin(((COL_tdst_Box *)_pst_GlobalVars->pst_A_ZDx->p_Shape)),
			COL_pst_Shape_GetMax(((COL_tdst_Box *)_pst_GlobalVars->pst_A_ZDx->p_Shape)),
			&ast_ACS_Static_Hit[0],
			&ul_NumberOfHits,
			&aul_CollisionType[0]
		)
	)
	{
		for(i = 0; i < ul_NumberOfHits; i++)
		{
			MATH_TransformVertex(&st_Global_Hit, _pst_GlobalVars->pst_A_GlobalMatrix, &ast_ACS_Static_Hit[i]);
			MATH_TransformVertex(&st_BCS_Static_Hit, &_pst_GlobalVars->st_B_InvGlobalMatrix, &st_Global_Hit);

			/* We set the flag that indicates that there was a collision at the Final position. */
			aul_CollisionType[i] |= COL_Cul_Static;

			if
			(
				COL_ComputeReport
				(
					&st_BCS_Static_Hit,
					_pst_Normal,
					&st_Global_Hit,
					&st_Global_Norm,
					&st_Global_Recal,
					&aul_CollisionType[i],
					_ul_Triangle,
					_ul_Element,
					_pst_GlobalVars
				)
			)
			{

				COL_ReportCollision
				(
					&st_Global_Hit,
					&st_Global_Norm,
					&st_Global_Recal,
					aul_CollisionType[i] & ~COL_Cul_Inactive,
					_ul_Triangle,
					_ul_Element,
					_pst_GlobalVars
				);

#ifdef ACTIVE_EDITORS
				/* Trace Vectors sa-mère*/
				if(!(aul_CollisionType[i] & (COL_Cul_Extra_Minor | COL_Cul_Invalid | COL_Cul_Inactive)))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~*/
					WOR_tdst_World	*pst_World;
					/*~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_World = _pst_GlobalVars->pst_A->pst_World;

					MATH_CopyVector(&pst_World->st_Origin[pst_World->uc_Vector], &st_Global_Hit);
					MATH_NormalizeVector(&pst_World->st_Vector[pst_World->uc_Vector], &st_Global_Recal);
                    pst_World->ap_VectorGAO[pst_World->uc_Vector] = NULL;
					if(aul_CollisionType[i] & COL_Cul_Edge)
						pst_World->aul_Color[pst_World->uc_Vector] = 0x0000FF00; /* Blanc */
					else
						pst_World->aul_Color[pst_World->uc_Vector] = 0xFFFFFFFF; /* Blanc */

					pst_World->uc_Vector = (pst_World->uc_Vector == (WOR_Cte_DbgVectorRender-1)) ? 0 : pst_World->uc_Vector + 1;

					MATH_CopyVector(&pst_World->st_Origin[pst_World->uc_Vector], &st_Global_Hit);
					MATH_CopyVector(&pst_World->st_Vector[pst_World->uc_Vector], &st_Global_Recal);
                    pst_World->ap_VectorGAO[pst_World->uc_Vector] = NULL;
					pst_World->aul_Color[pst_World->uc_Vector] = 0xFFFF00FF; /* Rose */
					pst_World->uc_Vector = (pst_World->uc_Vector == (WOR_Cte_DbgVectorRender-1)) ? 0 : pst_World->uc_Vector + 1;
				}

				/*if(0 && (aul_CollisionType[i] & COL_Cul_Extra_Minor))
				{
					WOR_tdst_World	*pst_World;

					pst_World = _pst_GlobalVars->pst_A->pst_World;

					MATH_CopyVector(&pst_World->st_Origin[pst_World->uc_Vector], &st_Global_Hit);
					MATH_InitVector(&pst_World->st_Vector[pst_World->uc_Vector], 0.0f, 0.0f, 1.0f);
                    pst_World->ap_VectorGAO[pst_World->uc_Vector] = NULL;
					pst_World->aul_Color[pst_World->uc_Vector] = 0x000096FF; // Orange 
					pst_World->uc_Vector = (pst_World->uc_Vector == (WOR_Cte_DbgVectorRender-1)) ? 0 : pst_World->uc_Vector + 1;

				}*/
#endif
			}
		}

		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL COL_GMat_Skip(COL_tdst_GameMat *_pst_GMat, COL_tdst_GlobalVars *_pst_GlobalVars)
{
	if
	(
		(_pst_GMat && (_pst_GMat->ul_CustomBits & COL_Cul_GMat_FlagX))
	&&	!(_pst_GlobalVars->pst_A_ZDx->uc_Flag & COL_C_Zone_FlagX)
	)
	{
		return TRUE;
	}

	if
	(
		(_pst_GMat && (_pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera))
	&&	(_pst_GlobalVars->pst_B_Cob->uc_Flag & COL_C_Cob_ReadyForCamera)
	&&	!(_pst_GlobalVars->pst_A_ZDx->uc_Flag & COL_C_Zone_Camera)
	)
	{
		return TRUE;
	}


	/* Camera optimisation ? */
	if
	(
		(_pst_GlobalVars->pst_A_ZDx->uc_Flag & COL_C_Zone_Camera)
	&&	(_pst_GMat && !(_pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera))
	&&	(_pst_GlobalVars->pst_B_Cob->uc_Flag & COL_C_Cob_ReadyForCamera)
	)
	{
		return TRUE;
	}

	return FALSE;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_Sphere_Triangle
(
	COL_tdst_GlobalVars			*_pst_GlobalVars,
	COL_tdst_IndexedTriangle	*_pst_Triangle,
	USHORT						_uw_T1,
	USHORT						_uw_T2,
	USHORT						_uw_T3,
	MATH_tdst_Vector			*_pst_Normal,
	ULONG						_ul_Triangle,
	ULONG						_ul_Element,
	BOOL						_b_Dynamic
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_CollisionType;
	MATH_tdst_Vector	*pst_T1, *pst_T2, *pst_T3;
	MATH_tdst_Vector	st_BCS_Static_Hit, st_BCS_Dynamic_Hit;
	MATH_tdst_Vector	st_Global_Norm, st_Global_Recal, st_Global_Hit;
	MATH_tdst_Vector	*pst_BCS_Hit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_CollisionType = 0;

	pst_T1 = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point[_uw_T1];
	pst_T2 = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point[_uw_T2];
	pst_T3 = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Point[_uw_T3];

	/*$F ----------------------- DYNAMIC COLLISION ----------------------- */
	if(_b_Dynamic)
	{
		if
		(
			INT_DynamicHit
			(
				&_pst_GlobalVars->st_A_BCS_DynamicCenter,
				_pst_GlobalVars->f_A_GCS_DynamicRadius,
				&_pst_GlobalVars->st_A_BCS_ZDxCenter,
				_pst_GlobalVars->f_A_GCS_ZDxRadius,
				&_pst_GlobalVars->st_A_BCS_MoveUnit,
				pst_T1,
				pst_T2,
				pst_T3,
				_pst_Normal,
				&st_BCS_Dynamic_Hit,
				&ul_CollisionType
			)
		)
		{
			/* Was it a collision with the Final Sphere Position ? */
			if
			(
				INT_FullSphereTriangle
				(
					&_pst_GlobalVars->st_A_BCS_ZDxCenter,
					_pst_GlobalVars->f_A_GCS_ZDxRadius,
					pst_T1,
					pst_T2,
					pst_T3,
					_pst_Normal,
					&st_BCS_Static_Hit,
					&ul_CollisionType
				)
			)
			{
				ul_CollisionType |= COL_Cul_Static;
				pst_BCS_Hit = &st_BCS_Static_Hit;
			}
			else
			{
				ul_CollisionType |= COL_Cul_Dynamic;
				pst_BCS_Hit = &st_BCS_Dynamic_Hit;
			}

			if
			(
				COL_ComputeReport
				(
					pst_BCS_Hit,
					_pst_Normal,
					&st_Global_Hit,
					&st_Global_Norm,
					&st_Global_Recal,
					&ul_CollisionType,
					_ul_Triangle,
					_ul_Element,
					_pst_GlobalVars
				)
			)
			{
				/* We report the collision */
				COL_ReportCollision
				(
					&st_Global_Hit,
					&st_Global_Norm,
					&st_Global_Recal,
					ul_CollisionType,
					_ul_Triangle,
					_ul_Element,
					_pst_GlobalVars
				);
			}

			return TRUE;
		}
	}
	{
		/*$F ----------------------- STATIC COLLISION ----------------------- */
		if
		(
			INT_FullSphereTriangle
			(
				&_pst_GlobalVars->st_A_BCS_ZDxCenter,
				_pst_GlobalVars->f_A_GCS_ZDxRadius,
				pst_T1,
				pst_T2,
				pst_T3,
				_pst_Normal,
				&st_BCS_Static_Hit,
				&ul_CollisionType
			)
		)
		{
			/* We set the flag that indicates that there was a collision at the Final position. */
			ul_CollisionType |= COL_Cul_Static;

			if
			(
				COL_ComputeReport
				(
					&st_BCS_Static_Hit,
					_pst_Normal,
					&st_Global_Hit,
					&st_Global_Norm,
					&st_Global_Recal,
					&ul_CollisionType,
					_ul_Triangle,
					_ul_Element,
					_pst_GlobalVars
				)
			)
			{
				/* We report the collision */
				COL_ReportCollision
				(
					&st_Global_Hit,
					&st_Global_Norm,
					&st_Global_Recal,
					ul_CollisionType,
					_ul_Triangle,
					_ul_Element,
					_pst_GlobalVars
				);
			}

			return TRUE;
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_ZDM_Sphere(COL_tdst_GlobalVars *_pst_GlobalVars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ZDx		*pst_A_ZDx;
	COL_tdst_Cob		*pst_B_Cob;
	MATH_tdst_Vector	st_BCS_Normal, st_Global_Norm, st_Global_Hit, st_BCS_Hit;
	MATH_tdst_Vector	st_Global_Recal;
	ULONG				ul_CollisionType;
	float				f_B_BCS_Radius;
	MATH_tdst_Vector	st_B_BCS_Center;
	COL_tdst_GameMat	*pst_GMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Flag X and Camera */
	pst_GMat = COL_pst_GMat_Get(_pst_GlobalVars->pst_B_Cob, NULL);
	
	/* Flag X and Flag Camera Optimisation */
	if(COL_GMat_Skip(pst_GMat, _pst_GlobalVars)) return FALSE;

	ul_CollisionType = 0;

	/*
	 * To avoid any patatoid affect, we pre-compute the B center and radius to
	 * introduce the B Scale on it if needed (idem triangles).
	 */
	pst_B_Cob = _pst_GlobalVars->pst_B_Cob;
	pst_A_ZDx = _pst_GlobalVars->pst_A_ZDx;
	MATH_CopyVector(&st_B_BCS_Center, COL_pst_Shape_GetCenter(pst_B_Cob->pst_MathCob->p_Shape));
	if(MATH_b_TestScaleType(_pst_GlobalVars->pst_B_GlobalMatrix))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_B_Scale;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_GetScale(&st_B_Scale, _pst_GlobalVars->pst_B_GlobalMatrix);
		f_B_BCS_Radius = COL_f_Shape_GetRadius(pst_B_Cob->pst_MathCob->p_Shape) * fMax3(st_B_Scale.x, st_B_Scale.y, st_B_Scale.z);
		st_B_BCS_Center.x *= st_B_Scale.x;
		st_B_BCS_Center.y *= st_B_Scale.y;
		st_B_BCS_Center.z *= st_B_Scale.z;
	}
	else
		f_B_BCS_Radius = COL_f_Shape_GetRadius(pst_B_Cob->pst_MathCob->p_Shape);

	if
	(
		INT_FullSphereSphere
		(
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			_pst_GlobalVars->f_A_GCS_ZDxRadius,
			&st_B_BCS_Center,
			f_B_BCS_Radius,
			&st_BCS_Normal,
			&st_BCS_Hit
		)
	)
	{
		/*
		 * We set the flag that indicates that there was a collision at the Final position
		 * and with a Sphere.
		 */
		ul_CollisionType = COL_Cul_Static;
		ul_CollisionType |= COL_Cul_Sphere;

		if
		(
			COL_ComputeReport
			(
				&st_BCS_Hit,
				&st_BCS_Normal,
				&st_Global_Hit,
				&st_Global_Norm,
				&st_Global_Recal,
				&ul_CollisionType,
				(unsigned int) - 1,
				(unsigned int) - 1,
				_pst_GlobalVars
			)
		)
		{
			/* We report the collision */
			COL_ReportCollision
			(
				&st_Global_Hit,
				&st_Global_Norm,
				&st_Global_Recal,
				ul_CollisionType,
				(unsigned int) - 1,
				(unsigned int) - 1,
				_pst_GlobalVars
			);
		}

		return TRUE;
	}
	else
		return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_ZDM_Box(COL_tdst_GlobalVars *_pst_GlobalVars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ZDx		*pst_A_ZDx;
	COL_tdst_Cob		*pst_B_Cob;
	MATH_tdst_Vector	st_BCS_Normal, st_Global_Norm, st_Global_Hit, st_BCS_Hit;
	MATH_tdst_Vector	st_Global_Recal;
	ULONG				ul_CollisionType;
	MATH_tdst_Vector	st_B_BCS_Min, st_B_BCS_Max;
	COL_tdst_GameMat	*pst_GMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Flag X and Camera */
	pst_GMat = COL_pst_GMat_Get(_pst_GlobalVars->pst_B_Cob, NULL);
	
	/* Flag X and Flag Camera Optimisation */
	if(COL_GMat_Skip(pst_GMat, _pst_GlobalVars)) return FALSE;

	ul_CollisionType = 0;

	/*
	 * To avoid any patatoid affect, we pre-compute the B Min/Max points to introduce
	 * the B Scale on it if needed (idem triangles).
	 */
	pst_B_Cob = _pst_GlobalVars->pst_B_Cob;
	pst_A_ZDx = _pst_GlobalVars->pst_A_ZDx;
	MATH_CopyVector(&st_B_BCS_Min, COL_pst_Shape_GetMin(pst_B_Cob->pst_MathCob->p_Shape));
	MATH_CopyVector(&st_B_BCS_Max, COL_pst_Shape_GetMax(pst_B_Cob->pst_MathCob->p_Shape));
	if(MATH_b_TestScaleType(_pst_GlobalVars->pst_B_GlobalMatrix))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_B_Scale;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_GetScale(&st_B_Scale, _pst_GlobalVars->pst_B_GlobalMatrix);
		st_B_BCS_Min.x *= st_B_Scale.x;
		st_B_BCS_Min.y *= st_B_Scale.y;
		st_B_BCS_Min.z *= st_B_Scale.z;

		st_B_BCS_Max.x *= st_B_Scale.x;
		st_B_BCS_Max.y *= st_B_Scale.y;
		st_B_BCS_Max.z *= st_B_Scale.z;
	}

	if
	(
		INT_FullSphereOBBox
		(
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			_pst_GlobalVars->f_A_GCS_ZDxRadius,
			&st_B_BCS_Min,
			&st_B_BCS_Max,
			&_pst_GlobalVars->st_A_BCS_OldZDxCenter,
			&_pst_GlobalVars->st_A_BCS_MoveUnit,
			&st_BCS_Normal,
			&st_BCS_Hit,
			&ul_CollisionType
		)
	)
	{
		if
		(
			COL_ComputeReport
			(
				&st_BCS_Hit,
				&st_BCS_Normal,
				&st_Global_Hit,
				&st_Global_Norm,
				&st_Global_Recal,
				&ul_CollisionType,
				(unsigned int) - 1,
				(unsigned int) - 1,
				_pst_GlobalVars
			)
		)
		{
			/* We report the collision */
			COL_ReportCollision
			(
				&st_Global_Hit,
				&st_Global_Norm,	
				&st_Global_Recal,
				ul_CollisionType,
				(unsigned int) - 1,
				(unsigned int) - 1,
				_pst_GlobalVars
			);
		}

		return TRUE;
	}
	else
		return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_ZDM_Cylinder(COL_tdst_GlobalVars *_pst_GlobalVars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ZDx		*pst_A_ZDx;
	COL_tdst_Cob		*pst_B_Cob;
	MATH_tdst_Vector	st_BCS_Normal, st_Global_Norm, st_Global_Hit, st_BCS_Hit;
	MATH_tdst_Vector	st_Global_Recal;
	ULONG				ul_CollisionType;
	MATH_tdst_Vector	st_B_BCS_Center;
	float				f_B_BCS_Radius, f_B_BCS_Height;
	COL_tdst_GameMat	*pst_GMat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Flag X and Camera */
	pst_GMat = COL_pst_GMat_Get(_pst_GlobalVars->pst_B_Cob, NULL);
	
	/* Flag X and Flag Camera Optimisation */
	if(COL_GMat_Skip(pst_GMat, _pst_GlobalVars)) return FALSE;

	ul_CollisionType = 0;

	/*
	 * To avoid any patatoid affect, we pre-compute the B points to introduce the B
	 * Scale on it if needed (idem triangles).
	 */
	pst_B_Cob = _pst_GlobalVars->pst_B_Cob;
	pst_A_ZDx = _pst_GlobalVars->pst_A_ZDx;
	MATH_CopyVector(&st_B_BCS_Center, COL_pst_Shape_GetCenter(pst_B_Cob->pst_MathCob->p_Shape));

	if(MATH_b_TestScaleType(_pst_GlobalVars->pst_B_GlobalMatrix))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_B_Scale;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_GetScale(&st_B_Scale, _pst_GlobalVars->pst_B_GlobalMatrix);
		st_B_BCS_Center.x *= st_B_Scale.x;
		st_B_BCS_Center.y *= st_B_Scale.y;
		st_B_BCS_Center.z *= st_B_Scale.z;
		f_B_BCS_Radius = COL_f_Shape_GetRadius(pst_B_Cob->pst_MathCob->p_Shape) * fMax(st_B_Scale.x, st_B_Scale.y);
		f_B_BCS_Height = COL_f_Shape_GetHeight(pst_B_Cob->pst_MathCob->p_Shape) * st_B_Scale.z;
	}
	else
	{
		f_B_BCS_Radius = COL_f_Shape_GetRadius(pst_B_Cob->pst_MathCob->p_Shape);
		f_B_BCS_Height = COL_f_Shape_GetHeight(pst_B_Cob->pst_MathCob->p_Shape);
	}

	if
	(
		INT_FullSphereCylinder
		(
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			_pst_GlobalVars->f_A_GCS_ZDxRadius,
			&st_B_BCS_Center,
			f_B_BCS_Radius,
			f_B_BCS_Height,
			&_pst_GlobalVars->st_A_BCS_OldZDxCenter,
			&_pst_GlobalVars->st_A_BCS_MoveUnit,
			&st_BCS_Normal,
			&st_BCS_Hit,
			&ul_CollisionType
		)
	)
	{
		if
		(
			COL_ComputeReport
			(
				&st_BCS_Hit,
				&st_BCS_Normal,
				&st_Global_Hit,
				&st_Global_Norm,
				&st_Global_Recal,
				&ul_CollisionType,
				(unsigned int) - 1,
				(unsigned int) - 1,
				_pst_GlobalVars
			)
		)
		{
			/* We report the collision */
			COL_ReportCollision
			(
				&st_Global_Hit,
				&st_Global_Norm,
				&st_Global_Recal,
				ul_CollisionType,
				(unsigned int) - 1,
				(unsigned int) - 1,
				_pst_GlobalVars
			);
		}

		return TRUE;
	}
	else
		return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_TestAndCollideTriangleIfNeeded
(
	COL_tdst_GlobalVars			*_pst_GlobalVars,
	COL_tdst_IndexedTriangle	*_pst_Triangle,
	MATH_tdst_Vector			*_pst_Normal,
	USHORT						_uw_Element,
	USHORT						_uw_Triangle,
	MATH_tdst_Vector			*_pst_Min,
	MATH_tdst_Vector			*_pst_Max,
	BOOL						_b_Dynamic
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector			*pst_T1, *pst_T2, *pst_T3;
	MATH_tdst_Vector			st_Rejection_Min, st_Rejection_Max;
	USHORT						uw_T1, uw_T2, uw_T3;
	float						f_Max, f_Min;
	COL_tdst_IndexedTriangles	*pst_B_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



	if(!_pst_Min && !_pst_Max)
	{
		MATH_tdst_Vector	*pst_Center;
		float				R;

		if(_b_Dynamic)
		{
			pst_Center = &_pst_GlobalVars->st_A_BCS_DynamicCenter;
			R = _pst_GlobalVars->f_A_GCS_DynamicRadius;
		}
		else
		{
			pst_Center = &_pst_GlobalVars->st_A_BCS_ZDxCenter;
			R = _pst_GlobalVars->f_A_GCS_ZDxRadius;
		}
		
		st_Rejection_Max.x = (pst_Center->x + R);
		st_Rejection_Max.y = (pst_Center->y + R);
		st_Rejection_Max.z = (pst_Center->z + R);
		
		st_Rejection_Min.x = (pst_Center->x - R);
		st_Rejection_Min.y = (pst_Center->y - R);
		st_Rejection_Min.z = (pst_Center->z - R);

		_pst_Min = &st_Rejection_Min;
		_pst_Max = &st_Rejection_Max;
	}


	/* Anti-Bug */
	_pst_Normal->x = (fAbs(_pst_Normal->x) < 1E-3f) ? 0.0f : _pst_Normal->x;
	_pst_Normal->y = (fAbs(_pst_Normal->y) < 1E-3f) ? 0.0f : _pst_Normal->y;
	_pst_Normal->z = (fAbs(_pst_Normal->z) < 1E-3f) ? 0.0f : _pst_Normal->z;

	/* NULL Normal vector, we Skip the Face. */
	if(MATH_b_NulVector(_pst_Normal)) return;

	/*$F===================================================================================================

						BACKFACE WITH SPEED OR MOVE VECTOR IF RECOMPUTING

	======================================================================================================*/
	if(_pst_GlobalVars->pst_A_Instance->uc_UserBackFace)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_BCS_BackFace;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_TransformVectorNoScale(&st_BCS_BackFace, &_pst_GlobalVars->st_B_InvGlobalMatrix, &_pst_GlobalVars->pst_A_Instance->st_BackFace);

		if(MATH_f_DotProduct(&st_BCS_BackFace, _pst_Normal) > 0.0f) return;
	}
	else
	{
		
		if(OBJ_b_TestIdentityFlag(_pst_GlobalVars->pst_A, OBJ_C_IdentityFlag_Dyna) &&	!(_pst_GlobalVars->pst_B_Cob->uc_Flag & COL_C_Cob_Movable))
		{
			/* For Hierarchy Object and First COL Loop, we use the Speed for BackFace Tests. */
			if(OBJ_b_TestIdentityFlag(_pst_GlobalVars->pst_A, OBJ_C_IdentityFlag_Hierarchy) &&	!(_pst_GlobalVars->b_Recomputing))
			{
				if
				(
					fEqWithEpsilon(_pst_GlobalVars->pst_B_GlobalMatrix->Kz, 1.0f, 1E-3f)
				&&	(fAbs(MATH_f_DotProduct(_pst_GlobalVars->pst_InvUnitG, _pst_Normal)) < _pst_GlobalVars->f_WallCosAngle)
				)
				{
					if
					(
						(
							(
							_pst_GlobalVars->st_A_BCS_Speed.x *
							_pst_Normal->x +
							_pst_GlobalVars->st_A_BCS_Speed.y *
							_pst_Normal->y
							) > 0.0f
						)
					&&	(MATH_f_DotProduct(&_pst_GlobalVars->st_A_BCS_Speed, _pst_Normal) > 0.0f)
					) return;
				}
				else
				{
					if(MATH_f_DotProduct(&_pst_GlobalVars->st_A_BCS_Speed, _pst_Normal) > 0.0f) return;
				}
			}
			else
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	st_BCS_ZDxMove;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				
				/*
				* For Non-Hierarchy Objects and Loops, we use (Previous ZDM Pos)-(New ZDM Pos)
				* for BackFace Tests.
				*/
				MATH_SubVector
				(
					&st_BCS_ZDxMove,
					&_pst_GlobalVars->st_A_BCS_ZDxCenter,
					&_pst_GlobalVars->st_A_BCS_OldZDxCenter
				);
				
				if
				(
					fEqWithEpsilon(_pst_GlobalVars->pst_B_GlobalMatrix->Kz, 1.0f, 1E-3f)
				&&	(fAbs(MATH_f_DotProduct(_pst_GlobalVars->pst_InvUnitG, _pst_Normal)) < _pst_GlobalVars->f_WallCosAngle)
				)
				{
					if
					(
						(((st_BCS_ZDxMove.x * _pst_Normal->x) + (st_BCS_ZDxMove.y * _pst_Normal->y)) > 0.0f)
					&&	(MATH_f_DotProduct(&st_BCS_ZDxMove, _pst_Normal) > 0.0f)
					&&  (MATH_f_DotProduct(&_pst_GlobalVars->st_A_BCS_Speed, _pst_Normal) > 0.0f)
					) return;
				}
				else
				{
					if(
							(MATH_f_DotProduct(&st_BCS_ZDxMove, _pst_Normal) > 0.0f) 
						&&	(MATH_f_DotProduct(&_pst_GlobalVars->st_A_BCS_Speed, _pst_Normal) > 0.0f)
					   )
						return;
				}
			}
		}
	}

	uw_T1 = _pst_Triangle->auw_Index[0];
	uw_T2 = _pst_Triangle->auw_Index[1];
	uw_T3 = _pst_Triangle->auw_Index[2];

	pst_B_Geo = _pst_GlobalVars->pst_B_Cob->pst_TriangleCob;

	/* We get the triangles points. */
	pst_T1 = &pst_B_Geo->dst_Point[uw_T1];
	pst_T2 = &pst_B_Geo->dst_Point[uw_T2];
	pst_T3 = &pst_B_Geo->dst_Point[uw_T3];

	/*$F
 ===================================================================================================

							QUICK STATIC REJECTION TEST

 ===================================================================================================
 */

	/* --- X rejection */
	f_Max = fMax3(pst_T1->x, pst_T2->x, pst_T3->x);
	if(f_Max < _pst_Min->x) return;;

	f_Min = fMin3(pst_T1->x, pst_T2->x, pst_T3->x);
	if(_pst_Max->x < f_Min) return;

	/* --- Y rejection */
	f_Max = fMax3(pst_T1->y, pst_T2->y, pst_T3->y);
	if(f_Max < _pst_Min->y) return;

	f_Min = fMin3(pst_T1->y, pst_T2->y, pst_T3->y);
	if(_pst_Max->y < f_Min) return;

	/* --- Z rejection */
	f_Max = fMax3(pst_T1->z, pst_T2->z, pst_T3->z);
	if(f_Max < _pst_Min->z) return;

	f_Min = fMin3(pst_T1->z, pst_T2->z, pst_T3->z);
	if(_pst_Max->z < f_Min) return;

#ifdef ACTIVE_EDITORS
	COL_ul_ComputedFaces++;
#endif
	/*$F
 ===================================================================================================

							END QUICK STATIC REJECTION TEST

 ===================================================================================================
 */
	if(COL_b_Zone_TestType(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_Sphere))
	{
		COL_Sphere_Triangle
		(
			_pst_GlobalVars,
			_pst_Triangle,
			uw_T1,
			uw_T2,
			uw_T3,
			_pst_Normal,
			_uw_Triangle,
			_uw_Element,
			_b_Dynamic
		);
	}
	else
	{
		COL_Box_Triangle
		(
			_pst_GlobalVars,
			_pst_Triangle,
			uw_T1,
			uw_T2,
			uw_T3,
			_pst_Normal,
			_uw_Triangle,
			_uw_Element,
			_b_Dynamic
		);		
	}
}

extern float	TIM_gf_SpeedFactor;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_SetFaceTag(COL_tdst_OK3 *_pst_OK3, ULONG _ul_Face)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ul_Offset;
	/*~~~~~~~~~~~~~~*/

	if(!_pst_OK3 || !_pst_OK3->paul_Tag) return;

	ul_Offset = (_ul_Face >> 5);

	*(_pst_OK3->paul_Tag + ul_Offset) |=  (0x00000001 << (_ul_Face - (ul_Offset << 5)));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_b_TestFaceTag(COL_tdst_OK3 *_pst_OK3, ULONG _ul_Face)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ul_Offset;
	/*~~~~~~~~~~~~~~*/

	if(!_pst_OK3 || !_pst_OK3->paul_Tag) return FALSE;

	ul_Offset = (_ul_Face >> 5);

	return (*(_pst_OK3->paul_Tag + ul_Offset) & (0x00000001 << (_ul_Face - (ul_Offset << 5))));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void COL_ComputeRejection
(
	COL_tdst_GlobalVars		*_pst_GlobalVars, 
	MATH_tdst_Vector		*_pst_Rejection_Min, 
	MATH_tdst_Vector		*_pst_Rejection_Max,
	BOOL					_b_Dynamic
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Center;
	float				R;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	if(_b_Dynamic)
	{
		pst_Center = &_pst_GlobalVars->st_A_BCS_DynamicCenter;
		R = _pst_GlobalVars->f_A_GCS_DynamicRadius;
	}
	else
	{
		pst_Center = &_pst_GlobalVars->st_A_BCS_ZDxCenter;
		R = _pst_GlobalVars->f_A_GCS_ZDxRadius;
	}
	
	_pst_Rejection_Max->x = (pst_Center->x + R);
	_pst_Rejection_Max->y = (pst_Center->y + R);
	_pst_Rejection_Max->z = (pst_Center->z + R);
	
	_pst_Rejection_Min->x = (pst_Center->x - R);
	_pst_Rejection_Min->y = (pst_Center->y - R);
	_pst_Rejection_Min->z = (pst_Center->z - R);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_ZDM_OK3Node(COL_tdst_GlobalVars *_pst_GlobalVars, COL_tdst_OK3 *_pst_OK3, COL_tdst_OK3_Node *pst_Node, BOOL _b_Dynamic)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_OK3_Element				*pst_OK3_Element, *pst_LastElement;
	COL_tdst_ElementIndexedTriangles	*pst_Cob_Element;
	COL_tdst_IndexedTriangle			*pst_Triangle;
	COL_tdst_GameMat					*pst_GMat;
	MATH_tdst_Vector					*pst_Normal;
	ULONG								ul_Save;
	USHORT								uw_Triangle;
	int									i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	if(pst_Node->pst_OK3_Box->ul_NumElement)
		pst_Node->pst_OK3_Box->ul_OK3_Flag |= 1;
#endif
	pst_OK3_Element = pst_Node->pst_OK3_Box->pst_OK3_Element;
	pst_LastElement = pst_OK3_Element + pst_Node->pst_OK3_Box->ul_NumElement;
	for(; pst_OK3_Element < pst_LastElement; pst_OK3_Element++)
	{
		pst_Cob_Element = &_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Element[pst_OK3_Element->uw_Element];

		pst_GMat = COL_pst_GMat_Get(_pst_GlobalVars->pst_B_Cob, pst_Cob_Element);

		/* Flag X and Flag Camera Optimisation */
		if(COL_GMat_Skip(pst_GMat, _pst_GlobalVars)) continue;

#ifdef ACTIVE_EDITORS
		COL_ul_ParsedFaces += pst_OK3_Element->uw_NumTriangle;
		COL_ul_ParsedFacesOK3 += pst_OK3_Element->uw_NumTriangle;
#endif
		_pst_GlobalVars->pst_B_Element = pst_Cob_Element;

		for(ul_Save = 0, i = 0; i < pst_OK3_Element->uw_Element; i++)
		{
			ul_Save += _pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_Element[i].uw_NbTriangles;;
		}

		for(i = 0; i < pst_OK3_Element->uw_NumTriangle; i++)
		{						
			uw_Triangle = pst_OK3_Element->puw_OK3_Triangle[i];
			pst_Triangle = &pst_Cob_Element->dst_Triangle[uw_Triangle];

			/* Faces may have been already computed in another box */
			if(COL_b_TestFaceTag(_pst_OK3, ul_Save + uw_Triangle)) continue;

			pst_Normal = (_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_FaceNormal + ul_Save + uw_Triangle);							

			_pst_GlobalVars->pst_B_Triangle = pst_Triangle;

			COL_TestAndCollideTriangleIfNeeded
			(
				_pst_GlobalVars,
				pst_Triangle,
				pst_Normal,
				pst_OK3_Element->uw_Element,
				uw_Triangle,
				NULL,
				NULL,
				_b_Dynamic
			);

			/* Set Face has already computed for this GO */
			COL_SetFaceTag(_pst_OK3, ul_Save + uw_Triangle);

		}
	}

	return TRUE;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_ZDM_OK3Node_Recursive(COL_tdst_GlobalVars *_pst_GlobalVars, COL_tdst_OK3 *_pst_OK3, COL_tdst_OK3_Node *pst_Node, BOOL _b_Dynamic)
{
	if(!pst_Node->pst_OK3_Box) return TRUE;

#ifdef ACTIVE_EDITORS
	if(pst_Node->pst_OK3_Box->ul_NumElement)
		pst_Node->pst_OK3_Box->ul_OK3_Flag |= 2; /* Tested OK3 Box */
#endif

	if
	(
		INT_SphereAABBox
		(
			&_pst_GlobalVars->st_A_BCS_DynamicCenter,
			_pst_GlobalVars->f_A_GCS_DynamicRadius,
			&pst_Node->pst_OK3_Box->st_Min,
			&pst_Node->pst_OK3_Box->st_Max
		)
	)
	{
		COL_ZDM_OK3Node(_pst_GlobalVars, _pst_OK3, pst_Node, _b_Dynamic);
	}
	else
		goto COL_OK3_GotoNext;

	if(pst_Node->pst_Son)
		COL_ZDM_OK3Node_Recursive(_pst_GlobalVars, _pst_OK3, pst_Node->pst_Son, _b_Dynamic);


COL_OK3_GotoNext:
	if(pst_Node->pst_Next)
		COL_ZDM_OK3Node_Recursive(_pst_GlobalVars, _pst_OK3, pst_Node->pst_Next, _b_Dynamic);

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Computes the intersection (if there is one) between a Spherical ZDM and any ZDR.
 =======================================================================================================================
 */
BOOL COL_ZDM_ZDR(COL_tdst_GlobalVars *_pst_GlobalVars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob						*pst_B_Cob;
	COL_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	COL_tdst_IndexedTriangle			*pst_Triangle, *pst_LastTriangle;
	ULONG								ul_Triangle, ul_ElementTriangle, ul_Element;
	DYN_tdst_Dyna						*pst_Dyna;
	MATH_tdst_Vector					st_A_GCS_Speed;
	COL_tdst_OK3						*pst_OK3;
	BOOL								b_Dynamic;
	OBJ_tdst_GameObject					*pst_A;
	MATH_tdst_Vector					*pst_A_BCS_Speed;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_B_Cob = _pst_GlobalVars->pst_B_Cob;
	b_Dynamic = FALSE;
	pst_A = _pst_GlobalVars->pst_A;
	pst_A_BCS_Speed = &_pst_GlobalVars->st_A_BCS_Speed;

	/*
	 * Precalculate the inverse of unit gravity vector for the collision type
	 * calculation
	 */
	_pst_GlobalVars->pst_InvUnitG = &MATH_gst_BaseVectorK;
	 
	if(OBJ_b_TestIdentityFlag(pst_A, OBJ_C_IdentityFlag_Dyna))
	{
		pst_Dyna = OBJ_pst_GetDynaStruct(pst_A);
		DYN_GetSpeedVector(pst_Dyna, &st_A_GCS_Speed);
		if((DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces)) && !(MATH_b_NulVector(DYN_pst_GetGravity(pst_Dyna))))
		{
			if((OBJ_b_TestIdentityFlag(pst_A, OBJ_C_IdentityFlag_Hierarchy)) && pst_A->pst_Base && pst_A->pst_Base->pst_Hierarchy && pst_A->pst_Base->pst_Hierarchy->pst_Father)
			{
				MATH_MulMatrixMatrix
				(
					pst_A->pst_GlobalMatrix,
					&pst_A->pst_Base->pst_Hierarchy->st_LocalMatrix,
					pst_A->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
				);

				/* Force the dynamic Collision. */
//				b_Dynamic = TRUE;
			}

			if(!(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_IgnoreGravity)))
			{
				MATH_NormalizeVector(&_pst_GlobalVars->st_InvUnitG, DYN_pst_GetGravity(pst_Dyna));
				MATH_NegEqualVector(&_pst_GlobalVars->st_InvUnitG);
				_pst_GlobalVars->pst_InvUnitG = &_pst_GlobalVars->st_InvUnitG;
			}
		}
		
		MATH_TransformVector
		(
			pst_A_BCS_Speed,
			&_pst_GlobalVars->st_B_InvGlobalMatrix,
			&st_A_GCS_Speed
		);
	}
	else
	{
		MATH_InitVector(pst_A_BCS_Speed, 0.0f, 0.0f, 0.0f);
	}

	/* Anti-Bug */
	pst_A_BCS_Speed->x = (fAbs(pst_A_BCS_Speed->x) < 1E-3f) ? 0.0f : pst_A_BCS_Speed->x;
	pst_A_BCS_Speed->y = (fAbs(pst_A_BCS_Speed->y) < 1E-3f) ? 0.0f : pst_A_BCS_Speed->y;
	pst_A_BCS_Speed->z = (fAbs(pst_A_BCS_Speed->z) < 1E-3f) ? 0.0f : pst_A_BCS_Speed->z;

	switch(pst_B_Cob->uc_Type)
	{
	case COL_C_Zone_Sphere:
		if(COL_b_Zone_TestType(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_Sphere))
			COL_ZDM_Sphere(_pst_GlobalVars);
		break;

	case COL_C_Zone_Box:
		if(COL_b_Zone_TestType(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_Sphere))
			COL_ZDM_Box(_pst_GlobalVars);
		break;

	case COL_C_Zone_Cylinder:
		if(COL_b_Zone_TestType(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_Sphere))
			COL_ZDM_Cylinder(_pst_GlobalVars);
		break;

	case COL_C_Zone_Triangles:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			COL_tdst_IndexedTriangles	*pst_B_Geo;
			MATH_tdst_Vector			*pst_Normal;
			COL_tdst_GameMat			*pst_GMat;
			MATH_tdst_Vector			st_Rejection_Min, st_Rejection_Max;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
			if(pst_B_Cob->uc_Flag & COL_C_Cob_Updated) COL_SynchronizeCob(pst_B_Cob, FALSE,FALSE);
#endif
			/* If the Cob is moving, we dont want to allow the "ONLY-GRAVITY" Algo. */
			if(pst_B_Cob->uc_Flag & COL_C_Cob_Movable)
			{
				((COL_tdst_Base *) pst_A->pst_Extended->pst_Col)->pst_Instance->uw_Flags &= ~COL_Cul_OnlyGravity;
			}

			pst_B_Geo = pst_B_Cob->pst_TriangleCob;
			pst_Element = pst_B_Geo->dst_Element;
			pst_LastElement = (pst_Element + pst_B_Geo->l_NbElements);

			_pst_GlobalVars->ul_FirstCollision = ((WOR_tdst_World *) _pst_GlobalVars->pst_World)->ul_NbReports;

			/* We want to know which algorithm to use. STATIC or DYNAMIC */
			if(OBJ_b_TestIdentityFlag(pst_A, OBJ_C_IdentityFlag_Dyna))
			{
				/*~~~~~~~~~*/
				float	f_dt;
				/*~~~~~~~~~*/

				f_dt = TIM_gf_dt;

				if(((DYN_tdst_Dyna *) pst_A->pst_Base->pst_Dyna)->ul_DynFlags & DYN_C_NoSpeedFactor)
					f_dt /= TIM_gf_SpeedFactor;

				if((f_dt * MATH_f_NormVector(pst_A_BCS_Speed)) > fTwice(_pst_GlobalVars->f_A_GCS_ZDxRadius))
						b_Dynamic = TRUE;

				if(!b_Dynamic)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	st_Move;
					float				f_Move;
					MATH_tdst_Vector*p;
					/*~~~~~~~~~~~~~~~~~~~~~~~~*/

					if((pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && pst_A->pst_Base && pst_A->pst_Base->pst_Hierarchy && pst_A->pst_Base->pst_Hierarchy->pst_Father)
					{
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
						MATH_tdst_Matrix	st_Matrix;
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				
						MATH_MulMatrixMatrix(&st_Matrix, ((COL_tdst_Base *) pst_A->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix, pst_A->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix);
						p=&st_Matrix.T;
					}
					else
					{
					    p=&((COL_tdst_Base *) pst_A->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix->T;
					}
					
					MATH_SubVector
					(
						&st_Move,
						&pst_A->pst_GlobalMatrix->T,
						p
					);
					
					f_Move = MATH_f_NormVector(&st_Move);

					if(f_Move > fTwice(_pst_GlobalVars->f_A_GCS_ZDxRadius))
					{
						b_Dynamic = TRUE;
					}
				}
			}

			COL_ComputeRejection(_pst_GlobalVars, &st_Rejection_Min, &st_Rejection_Max, b_Dynamic);

			if(_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->pst_OK3)
			{
				/* Resets all OK3 Triangles Tag for this object */
				pst_OK3 = pst_B_Cob->pst_TriangleCob->pst_OK3;
				L_memset(pst_OK3->paul_Tag, 0, ((pst_B_Cob->pst_TriangleCob->l_NbFaces >> 5) + 1) << 2);

				if(1)
					COL_ZDM_OK3Node_Recursive(_pst_GlobalVars, pst_OK3, pst_OK3->pst_OK3_God, b_Dynamic);
				else
				{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				COL_tdst_OK3_Box					*pst_Box, *pst_LastBox;
				COL_tdst_OK3_Element				*pst_OK3_Element, *pst_LastElement;
				COL_tdst_ElementIndexedTriangles	*pst_Cob_Element;
				USHORT								uw_Triangle;
				ULONG								ul_Save;
				int									i;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_Box = pst_OK3->pst_OK3_Boxes;
				pst_LastBox = pst_Box + pst_OK3->ul_NumBox;
				for(; pst_Box < pst_LastBox; pst_Box++)
				{
					if
					(
						!INT_SphereAABBox
						(
							&_pst_GlobalVars->st_A_BCS_DynamicCenter,
							_pst_GlobalVars->f_A_GCS_DynamicRadius,
							&pst_Box->st_Min,
							&pst_Box->st_Max
						)
					)
					{
						continue;
					}

#ifdef ACTIVE_EDITORS
					/* For Display ONLY: Render Collided OK3 Box with a different Color. */
					pst_Box->ul_OK3_Flag |= 1;
#endif
					pst_OK3_Element = pst_Box->pst_OK3_Element;
					pst_LastElement = pst_OK3_Element +
					pst_Box->ul_NumElement;
					for(; pst_OK3_Element < pst_LastElement; pst_OK3_Element++)
					{
						pst_Cob_Element = &pst_B_Cob->pst_TriangleCob->dst_Element[pst_OK3_Element->uw_Element];

						pst_GMat = COL_pst_GMat_Get(pst_B_Cob, pst_Cob_Element);

						/* Flag X and Flag Camera Optimisation */
						if(COL_GMat_Skip(pst_GMat, _pst_GlobalVars)) continue;

#ifdef ACTIVE_EDITORS
						COL_ul_ParsedFaces += pst_OK3_Element->uw_NumTriangle;
						COL_ul_ParsedFacesOK3 += pst_OK3_Element->uw_NumTriangle;
#endif
						_pst_GlobalVars->pst_B_Element = pst_Cob_Element;

						for(ul_Save = 0, i = 0; i < pst_OK3_Element->uw_Element; i++)
						{
							ul_Save += pst_B_Cob->pst_TriangleCob->dst_Element[i].uw_NbTriangles;;
						}

						for(i = 0; i < pst_OK3_Element->uw_NumTriangle; i++)
						{						
							uw_Triangle = pst_OK3_Element->puw_OK3_Triangle[i];
							pst_Triangle = &pst_Cob_Element->dst_Triangle[uw_Triangle];

							/* Faces may have been already computed in another box */
							if(COL_b_TestFaceTag(pst_OK3, ul_Save + uw_Triangle)) continue;

							pst_Normal = (pst_B_Cob->pst_TriangleCob->dst_FaceNormal + ul_Save + uw_Triangle);							

							_pst_GlobalVars->pst_B_Triangle = pst_Triangle;

							COL_TestAndCollideTriangleIfNeeded
							(
								_pst_GlobalVars,
								pst_Triangle,
								pst_Normal,
								pst_OK3_Element->uw_Element,
								uw_Triangle,
								&st_Rejection_Min,
								&st_Rejection_Max,
								b_Dynamic
							);

							/* Set Face has already computed for this GO */
							COL_SetFaceTag(pst_OK3, ul_Save + uw_Triangle);

						}
					}
								

				}
			}
			}
			else
			{
				/* We go thru all the Elements of the Geometric object. */
				for(ul_Element = 0, ul_Triangle = 0; pst_Element < pst_LastElement; pst_Element++, ul_Element++)
				{
					pst_GMat = COL_pst_GMat_Get(pst_B_Cob, pst_Element);

					/* Flag X and Flag Camera Optimisation */
					if(COL_GMat_Skip(pst_GMat, _pst_GlobalVars))
					{
						ul_Triangle += pst_Element->uw_NbTriangles;
						continue;
					}

#ifdef ACTIVE_EDITORS
					COL_ul_ParsedFaces += pst_Element->uw_NbTriangles;
#endif
					_pst_GlobalVars->pst_B_Element = pst_Element;

					pst_Triangle = pst_Element->dst_Triangle;
					pst_LastTriangle = (pst_Triangle + pst_Element->uw_NbTriangles);
					for
					(
						ul_ElementTriangle = 0;
						pst_Triangle < pst_LastTriangle;
						pst_Triangle++, ul_Triangle++, ul_ElementTriangle++
					)
					{
						pst_Normal = (_pst_GlobalVars->pst_B_Cob->pst_TriangleCob->dst_FaceNormal + ul_Triangle);
						_pst_GlobalVars->pst_B_Triangle = pst_Triangle;

						COL_TestAndCollideTriangleIfNeeded
						(
							_pst_GlobalVars,
							pst_Triangle,
							pst_Normal,
							(USHORT) ul_Element,
							(USHORT) ul_ElementTriangle,
							&st_Rejection_Min,
							&st_Rejection_Max,
							b_Dynamic
						);
					}
				}
			}
		}
		break;
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Changes the Size of a specific ZDx.

    Note:   If the ZDx is a Sphere
 =======================================================================================================================
 */
void COL_ZDx_SpecificSizeSet(OBJ_tdst_GameObject *_pst_GO, UCHAR _uc_AI_Index, MATH_tdst_Vector *pst_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ZDx		*pst_ZDx;
	UCHAR				uc_ENG_Index, uc_ZDx_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) return;

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;

	/* We get the Real ENG index of the ZDx; */
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[_uc_AI_Index];

	/* If the concerning ZDx is not specific, we return; */
	if(!COL_b_Instance_IsSpecific(pst_Instance, uc_ENG_Index)) return;

	pst_ZDx = *(pst_Instance->dpst_ZDx + uc_ENG_Index);

	uc_ZDx_Type = (UCHAR) COL_Zone_GetType(pst_ZDx);

	switch(uc_ZDx_Type)
	{
	case COL_C_Zone_Box:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	*pst_Min, *pst_Max;
			MATH_tdst_Vector	st_Center;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Min = COL_pst_Shape_GetMin(pst_ZDx->p_Shape);
			pst_Max = COL_pst_Shape_GetMax(pst_ZDx->p_Shape);
			MATH_SubVector(&st_Center, pst_Max, pst_Min);
			MATH_MulEqualVector(&st_Center, 0.5f);
			MATH_AddEqualVector(&st_Center, pst_Min);

			pst_Min->x = st_Center.x -
			(pst_Size->x * 0.5f);
			pst_Min->y = st_Center.y -
			(pst_Size->y * 0.5f);
			pst_Min->z = st_Center.z -
			(pst_Size->z * 0.5f);
			pst_Max->x = st_Center.x +
			(pst_Size->x * 0.5f);
			pst_Max->y = st_Center.y +
			(pst_Size->y * 0.5f);
			pst_Max->z = st_Center.z +
			(pst_Size->z * 0.5f);
		}
		break;

	case COL_C_Zone_Sphere:
		COL_Shape_SetRadius(pst_ZDx->p_Shape, pst_Size->x);
		break;
	}
}

/*
 =======================================================================================================================
    Aim:    Returns the Size of a ZDx.

    Note:   Returns the Radius if the ZDx is a Sphere. Or Returns the Max half Length if the ZDx is a Box.
 =======================================================================================================================
 */
float COL_ZDx_SizeGet(OBJ_tdst_GameObject *_pst_GO, UCHAR _uc_AI_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ZDx		*pst_ZDx;
	UCHAR				uc_ENG_Index, uc_ZDx_Type;
	float				f_Radius;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Radius = 0.0f;

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) return f_Radius;

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;

	/* We get the Real ENG index of the ZDx; */
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[_uc_AI_Index];

	if(uc_ENG_Index == 0xFF)
	{
#ifdef ACTIVE_EDITORS
		LINK_PrintStatusMsg("Invalid ZDx given to COL_ZDx_SizeGet");
#endif
		return 0;
	}


	pst_ZDx = *(pst_Instance->dpst_ZDx + uc_ENG_Index);

	uc_ZDx_Type = (UCHAR) COL_Zone_GetType(pst_ZDx);

	switch(uc_ZDx_Type)
	{
	case COL_C_Zone_Box:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	*pst_Min, *pst_Max;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Min = COL_pst_Shape_GetMin(pst_ZDx->p_Shape);
			pst_Max = COL_pst_Shape_GetMax(pst_ZDx->p_Shape);

			f_Radius = fMax3(pst_Max->x - pst_Min->x, pst_Max->y - pst_Min->y, pst_Max->z - pst_Min->z);
			f_Radius /= 2.0f;

		}
		break;

	case COL_C_Zone_Sphere:
		f_Radius = COL_f_Shape_GetRadius(pst_ZDx->p_Shape);
		break;
	}

    // Consider scale
    {
        BOOL b_NoScaleMode = COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_NoScale);
        if(b_NoScaleMode)
        	return f_Radius;
        else
        {
            MATH_tdst_Vector st_Scale;
    		MATH_GetScale(&st_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
            return f_Radius * fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ZDx_PosGet(OBJ_tdst_GameObject *_pst_GO, UCHAR _uc_AI_Index, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ZDx		*pst_ZDx;
	UCHAR				uc_ENG_Index, uc_ZDx_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) return;

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;

	/* We get the Real ENG index of the ZDx; */
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[_uc_AI_Index];

	if(uc_ENG_Index == 0xFF)
	{
		MATH_InitVector(_pst_Pos, 0.0f, 0.0f, 0.0f);
#ifdef ACTIVE_EDITORS
		LINK_PrintStatusMsg("Invalid ZDx given to COL_ZDx_PosGet");
#endif
		return;
	}

	pst_ZDx = *(pst_Instance->dpst_ZDx + uc_ENG_Index);

	uc_ZDx_Type = (UCHAR) COL_Zone_GetType(pst_ZDx);

	switch(uc_ZDx_Type)
	{
	case COL_C_Zone_Box:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	*pst_Min, *pst_Max;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Min = COL_pst_Shape_GetMin(pst_ZDx->p_Shape);
			pst_Max = COL_pst_Shape_GetMax(pst_ZDx->p_Shape);

			MATH_AddVector(_pst_Pos, pst_Min, pst_Max);
			MATH_MulEqualVector(_pst_Pos, 0.5f);
			if(pst_ZDx->uc_Flag & COL_C_Zone_NoScale)
				MATH_TransformVertexNoScale(_pst_Pos, _pst_GO->pst_GlobalMatrix, _pst_Pos);
			else
				MATH_TransformVertex(_pst_Pos, _pst_GO->pst_GlobalMatrix, _pst_Pos);
		}
		break;

	case COL_C_Zone_Sphere:
		MATH_CopyVector(_pst_Pos, COL_pst_Shape_GetCenter(pst_ZDx->p_Shape));
		if(pst_ZDx->uc_Flag & COL_C_Zone_NoScale)
			MATH_TransformVertexNoScale(_pst_Pos, _pst_GO->pst_GlobalMatrix, _pst_Pos);
		else
			MATH_TransformVertex(_pst_Pos, _pst_GO->pst_GlobalMatrix, _pst_Pos);
		break;
	}
}

/*
 =======================================================================================================================
    Aim:    Changes the Pos of a specific ZDx.

    Note:   If the ZDx is a Sphere
 =======================================================================================================================
 */
void COL_ZDx_SpecificPosSet(OBJ_tdst_GameObject *_pst_GO, UCHAR _uc_AI_Index, MATH_tdst_Vector *pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColSet		*pst_ColSet;
	COL_tdst_Instance	*pst_Instance;
	COL_tdst_ZDx		*pst_ZDx;
	UCHAR				uc_ENG_Index, uc_ZDx_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)) return;

	pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_ColSet = pst_Instance->pst_ColSet;

	/* We get the Real ENG index of the ZDx; */
	uc_ENG_Index = pst_ColSet->pauc_AI_Indexes[_uc_AI_Index];

	/* If the concerning ZDx is not specific, we return; */
	if(!COL_b_Instance_IsSpecific(pst_Instance, uc_ENG_Index)) return;

	pst_ZDx = *(pst_Instance->dpst_ZDx + uc_ENG_Index);

	uc_ZDx_Type = (UCHAR) COL_Zone_GetType(pst_ZDx);

	switch(uc_ZDx_Type)
	{
	case COL_C_Zone_Box:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	*pst_Min, *pst_Max;
			MATH_tdst_Vector	st_Center, st_Trans;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Min = COL_pst_Shape_GetMin(pst_ZDx->p_Shape);
			pst_Max = COL_pst_Shape_GetMax(pst_ZDx->p_Shape);
			MATH_SubVector(&st_Center, pst_Max, pst_Min);
			MATH_MulEqualVector(&st_Center, 0.5f);
			MATH_AddEqualVector(&st_Center, pst_Min);
			MATH_SubVector(&st_Trans, pst_Pos, &st_Center);
			MATH_AddEqualVector(pst_Max, &st_Trans);
			MATH_AddEqualVector(pst_Min, &st_Trans);
		}
		break;

	case COL_C_Zone_Sphere:
		MATH_CopyVector(COL_pst_Shape_GetCenter(pst_ZDx->p_Shape), pst_Pos);
		break;
	}
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
