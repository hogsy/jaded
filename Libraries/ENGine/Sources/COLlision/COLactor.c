/*$T COLactor.c GC! 1.081 01/09/02 18:05:48 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "BASe/BAStypes.h"
#include "BASe/BASsys.h"
#include "BASe/ERRors/ERRasser.h"

#include "MATHs/MATH.h"

#include "OBJects/OBJstruct.h"

#include "COLlision/COLstruct.h"
#include "COLconst.h"
#include "COLlision/COLaccess.h"
#include "COLlision/COLvars.h"
#include "COLlision/COLzdx.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$F
 ===================================================================================================
 Aim:
	- Compute the position of the center of the A ZDM in the BCS
	- Compute the previous position of the center of the A ZDM in the BCS
	- Compute the movement vector to go from the old ZDM position to the current one in the BCS
 ===================================================================================================
 */

void COL_A_BCS_ComputeZDxInfo(COL_tdst_GlobalVars *_pst_GlobalVars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Global_Center;
	MATH_tdst_Vector	st_Scale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(_pst_GlobalVars->pst_A_ZDx);

	/* If the A ZDx is a Sphere */
	if(COL_b_Zone_TestType(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_Sphere))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Sphere		*p_Sphere;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* We get the sphere shape from the ZDx. */
		p_Sphere = (COL_tdst_Sphere *) _pst_GlobalVars->pst_A_ZDx->p_Shape;

		/*
		 * We compute the position of the center of the A ZDM in the B (object) Coordinate
		 * System (BCS).
		 */

		/*$F--------- If the ZDx is Scale sensitive ---------------- */
		if(!(COL_b_Zone_TestFlag(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_NoScale)))
		{
			/* Position of the ZDx */
			MATH_TransformVertex
			(
				&st_Global_Center,
				_pst_GlobalVars->pst_A_GlobalMatrix,
				COL_pst_Shape_GetCenter(p_Sphere)
			);

			/* If the A Matrix is scaled, we use it to compute the radius in GCS. */
			if(MATH_b_TestScaleType(_pst_GlobalVars->pst_A_GlobalMatrix))
			{
				MATH_GetScale(&st_Scale, _pst_GlobalVars->pst_A_GlobalMatrix);
				_pst_GlobalVars->f_A_GCS_ZDxRadius = COL_f_Shape_GetRadius(p_Sphere) * fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
			}
			else
				_pst_GlobalVars->f_A_GCS_ZDxRadius = COL_f_Shape_GetRadius(p_Sphere);
		}
		else

		/*$F--------- If the ZDx is NOT scale sensitive --------------*/
		{
			MATH_TransformVertexNoScale
			(
				&st_Global_Center,
				_pst_GlobalVars->pst_A_GlobalMatrix,
				COL_pst_Shape_GetCenter(p_Sphere)
			);

			_pst_GlobalVars->f_A_GCS_ZDxRadius = COL_f_Shape_GetRadius(p_Sphere);
		}

		MATH_TransformVertexNoScale
		(
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			&_pst_GlobalVars->st_B_InvGlobalMatrix,
			&st_Global_Center
		);

		MATH_TransformVertexNoScale
		(
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			&_pst_GlobalVars->st_B_InvGlobalMatrix,
			&st_Global_Center
		);

		/*
		 * We compute the previous position of the center of the A ZDM in the B (object)
		 * Coordinate System (BCS)
		 */

		/*$F--------- If the ZDx is Scale sensitive ---------------- */
		if(!(COL_b_Zone_TestFlag(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_NoScale)))
		{
			if((_pst_GlobalVars->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && _pst_GlobalVars->pst_A->pst_Base && _pst_GlobalVars->pst_A->pst_Base->pst_Hierarchy && _pst_GlobalVars->pst_A->pst_Base->pst_Hierarchy->pst_Father)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	st_Matrix;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_MulMatrixMatrix
				(
					&st_Matrix,
					((COL_tdst_Base *) _pst_GlobalVars->pst_A->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					_pst_GlobalVars->pst_A->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
				);
				MATH_TransformVertex(&st_Global_Center, &st_Matrix, COL_pst_Shape_GetCenter(p_Sphere));
			}
			else
			{
				MATH_TransformVertex
				(
					&st_Global_Center,
					_pst_GlobalVars->pst_A_OldGlobalMatrix,
					COL_pst_Shape_GetCenter(p_Sphere)
				);
			}
		}
		else

		/*$F--------- If the ZDx is NOT scale sensitive --------------*/
		{
			if(_pst_GlobalVars->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	st_Matrix;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_MulMatrixMatrix
				(
					&st_Matrix,
					((COL_tdst_Base *) _pst_GlobalVars->pst_A->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					_pst_GlobalVars->pst_A->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
				);
				MATH_TransformVertexNoScale(&st_Global_Center, &st_Matrix, COL_pst_Shape_GetCenter(p_Sphere));
			}
			else
			{
				MATH_TransformVertexNoScale
				(
					&st_Global_Center,
					_pst_GlobalVars->pst_A_OldGlobalMatrix,
					COL_pst_Shape_GetCenter(p_Sphere)
				);
			}
		}

		MATH_TransformVertexNoScale
		(
			&_pst_GlobalVars->st_A_BCS_OldZDxCenter,
			&_pst_GlobalVars->st_B_InvGlobalMatrix,
			&st_Global_Center
		);

		/*
		 * We compute the movement vector to go from the old ZDM position to the current
		 * one in the BCS.
		 */
		MATH_SubVector
		(
			&_pst_GlobalVars->st_A_BCS_Move,
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			&_pst_GlobalVars->st_A_BCS_OldZDxCenter
		);

		/* Find the Unit vector on the Move direction. */
		if(!MATH_b_NulVector(&_pst_GlobalVars->st_A_BCS_Move))
			MATH_NormalizeVector(&_pst_GlobalVars->st_A_BCS_MoveUnit, &_pst_GlobalVars->st_A_BCS_Move);
		else
			MATH_InitVector(&_pst_GlobalVars->st_A_BCS_MoveUnit, 0.0f, 0.0f, 0.0f);

		/* Computes the Middle point of the 2 Spheres positions */
		MATH_SubVector
		(
			&_pst_GlobalVars->st_A_BCS_DynamicCenter,
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			&_pst_GlobalVars->st_A_BCS_OldZDxCenter
		);

		MATH_MulEqualVector(&_pst_GlobalVars->st_A_BCS_DynamicCenter, 0.5f);

		MATH_AddEqualVector(&_pst_GlobalVars->st_A_BCS_DynamicCenter, &_pst_GlobalVars->st_A_BCS_OldZDxCenter);

		/* Computes the Radius of the Sphere that englobes the 2 positions in the BCS */
		_pst_GlobalVars->f_A_GCS_DynamicRadius = MATH_f_Distance
			(
				&_pst_GlobalVars->st_A_BCS_DynamicCenter,
				&_pst_GlobalVars->st_A_BCS_OldZDxCenter
			);
		_pst_GlobalVars->f_A_GCS_DynamicRadius += COL_f_Shape_GetRadius(p_Sphere);

		/* Computes the Radius od the Sphere that englobes the 2 positions in the GCS */
		if(MATH_b_TestScaleType(_pst_GlobalVars->pst_A_GlobalMatrix) && !(COL_b_Zone_TestFlag(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_NoScale)))
		{
			MATH_GetScale(&st_Scale, _pst_GlobalVars->pst_A_GlobalMatrix);
			_pst_GlobalVars->f_A_GCS_DynamicRadius += fMax3(st_Scale.x, st_Scale.y, st_Scale.z) * COL_f_Shape_GetRadius(p_Sphere);
		}
		else
		{
			_pst_GlobalVars->f_A_GCS_DynamicRadius += COL_f_Shape_GetRadius(p_Sphere);
		}
	}
	/* ZDM Box Case */
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Box		*p_Box;
		MATH_tdst_Vector	st_ACS_Center;
		MATH_tdst_Vector	st_HalfDiag;
		float				f_Radius;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* We get the Box shape from the ZDx. */
		p_Box = (COL_tdst_Box *) _pst_GlobalVars->pst_A_ZDx->p_Shape;

		/* Compute box Center */
		MATH_AddVector(&st_ACS_Center, COL_pst_Shape_GetMin(p_Box), COL_pst_Shape_GetMax(p_Box));
		MATH_ScaleEqualVector(&st_ACS_Center, 0.5f);

		MATH_SubVector(&st_HalfDiag, COL_pst_Shape_GetMax(p_Box), COL_pst_Shape_GetMin(p_Box));
		MATH_ScaleEqualVector(&st_HalfDiag, 0.5f);

		f_Radius = MATH_f_NormVector(&st_HalfDiag);

		/*
		 * We compute the position of the center of the A ZDM in the B (object) Coordinate
		 * System (BCS).
		 */

		/*$F--------- If the ZDx is Scale sensitive ---------------- */
		if(!(COL_b_Zone_TestFlag(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_NoScale)))
		{
			/* Position of the ZDx */
			MATH_TransformVertex
			(
				&st_Global_Center,
				_pst_GlobalVars->pst_A_GlobalMatrix,
				&st_ACS_Center
			);

			/* If the A Matrix is scaled, we use it to compute the radius in GCS. */
			if(MATH_b_TestScaleType(_pst_GlobalVars->pst_A_GlobalMatrix))
			{
				MATH_GetScale(&st_Scale, _pst_GlobalVars->pst_A_GlobalMatrix);
				_pst_GlobalVars->f_A_GCS_ZDxRadius = f_Radius * fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
			}
			else
				_pst_GlobalVars->f_A_GCS_ZDxRadius = f_Radius;
		}
		else

		/*$F--------- If the ZDx is NOT scale sensitive --------------*/
		{
			MATH_TransformVertexNoScale
			(
				&st_Global_Center,
				_pst_GlobalVars->pst_A_GlobalMatrix,
				&st_ACS_Center
			);

			_pst_GlobalVars->f_A_GCS_ZDxRadius = f_Radius;
		}

		MATH_TransformVertexNoScale
		(
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			&_pst_GlobalVars->st_B_InvGlobalMatrix,
			&st_Global_Center
		);

		MATH_TransformVertexNoScale
		(
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			&_pst_GlobalVars->st_B_InvGlobalMatrix,
			&st_Global_Center
		);

		/*
		 * We compute the previous position of the center of the A ZDM in the B (object)
		 * Coordinate System (BCS)
		 */

		/*$F--------- If the ZDx is Scale sensitive ---------------- */
		if(!(COL_b_Zone_TestFlag(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_NoScale)))
		{
			if(_pst_GlobalVars->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	st_Matrix;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_MulMatrixMatrix
				(
					&st_Matrix,
					((COL_tdst_Base *) _pst_GlobalVars->pst_A->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					_pst_GlobalVars->pst_A->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
				);
				MATH_TransformVertex(&st_Global_Center, &st_Matrix, &st_ACS_Center);
			}
			else
			{
				MATH_TransformVertex
				(
					&st_Global_Center,
					_pst_GlobalVars->pst_A_OldGlobalMatrix,
					&st_ACS_Center
				);
			}
		}
		else

		/*$F--------- If the ZDx is NOT scale sensitive --------------*/
		{
			if(_pst_GlobalVars->pst_A->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	st_Matrix;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_MulMatrixMatrix
				(
					&st_Matrix,
					((COL_tdst_Base *) _pst_GlobalVars->pst_A->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					_pst_GlobalVars->pst_A->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
				);
				MATH_TransformVertexNoScale(&st_Global_Center, &st_Matrix, &st_ACS_Center);
			}
			else
			{
				MATH_TransformVertexNoScale
				(
					&st_Global_Center,
					_pst_GlobalVars->pst_A_OldGlobalMatrix,
					&st_ACS_Center
				);
			}
		}

		MATH_TransformVertexNoScale
		(
			&_pst_GlobalVars->st_A_BCS_OldZDxCenter,
			&_pst_GlobalVars->st_B_InvGlobalMatrix,
			&st_Global_Center
		);

		/*
		 * We compute the movement vector to go from the old ZDM position to the current
		 * one in the BCS.
		 */
		MATH_SubVector
		(
			&_pst_GlobalVars->st_A_BCS_Move,
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			&_pst_GlobalVars->st_A_BCS_OldZDxCenter
		);

		/* Find the Unit vector on the Move direction. */
		if(!MATH_b_NulVector(&_pst_GlobalVars->st_A_BCS_Move))
			MATH_NormalizeVector(&_pst_GlobalVars->st_A_BCS_MoveUnit, &_pst_GlobalVars->st_A_BCS_Move);
		else
			MATH_InitVector(&_pst_GlobalVars->st_A_BCS_MoveUnit, 0.0f, 0.0f, 0.0f);

		/* Computes the Middle point of the 2 Spheres positions */
		MATH_SubVector
		(
			&_pst_GlobalVars->st_A_BCS_DynamicCenter,
			&_pst_GlobalVars->st_A_BCS_ZDxCenter,
			&_pst_GlobalVars->st_A_BCS_OldZDxCenter
		);

		MATH_MulEqualVector(&_pst_GlobalVars->st_A_BCS_DynamicCenter, 0.5f);

		MATH_AddEqualVector(&_pst_GlobalVars->st_A_BCS_DynamicCenter, &_pst_GlobalVars->st_A_BCS_OldZDxCenter);

		/* Computes the Radius of the Sphere that englobes the 2 positions in the BCS */
		_pst_GlobalVars->f_A_GCS_DynamicRadius = MATH_f_Distance
			(
				&_pst_GlobalVars->st_A_BCS_DynamicCenter,
				&_pst_GlobalVars->st_A_BCS_OldZDxCenter
			);
		_pst_GlobalVars->f_A_GCS_DynamicRadius += f_Radius;

		/* Computes the Radius od the Sphere that englobes the 2 positions in the GCS */
		if(MATH_b_TestScaleType(_pst_GlobalVars->pst_A_GlobalMatrix) && !(COL_b_Zone_TestFlag(_pst_GlobalVars->pst_A_ZDx, COL_C_Zone_NoScale)))
		{
			MATH_GetScale(&st_Scale, _pst_GlobalVars->pst_A_GlobalMatrix);
			_pst_GlobalVars->f_A_GCS_DynamicRadius += fMax3(st_Scale.x, st_Scale.y, st_Scale.z) * f_Radius;
		}
		else
		{
			_pst_GlobalVars->f_A_GCS_DynamicRadius += f_Radius;
		}

	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_Actor_GameObject(COL_tdst_GlobalVars *_pst_GlobalVars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Instance	*pst_A_Instance;
	COL_tdst_ColMap		*pst_B_ColMap;
	COL_tdst_Cob		**dpst_B_Cob, **dpst_B_LastCob;
	COL_tdst_ZDx		**dpst_A_ZDx, **dpst_A_LastZDx;
	UCHAR				uc_A_Index, uc_B_Index, uc_A_Type;
	BOOL				b_Skip;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Skip = FALSE;
	pst_A_Instance = ((COL_tdst_Base *) _pst_GlobalVars->pst_A->pst_Extended->pst_Col)->pst_Instance;
	pst_B_ColMap = ((COL_tdst_Base *) _pst_GlobalVars->pst_B->pst_Extended->pst_Col)->pst_ColMap;
	if(!pst_B_ColMap) return TRUE;

	/* Update Global Vars. */
	_pst_GlobalVars->pst_A_Instance = pst_A_Instance;
	_pst_GlobalVars->pst_B_ColMap = pst_B_ColMap;

	dpst_A_ZDx = pst_A_Instance->dpst_ZDx;
	dpst_A_LastZDx = dpst_A_ZDx + pst_A_Instance->uc_NbOfZDx;

	_pst_GlobalVars->dpst_A_ZDx = dpst_A_ZDx;

	/* Loop thru all the A's ZDMs */
	for(uc_A_Index = 0; dpst_A_ZDx < dpst_A_LastZDx; dpst_A_ZDx++, uc_A_Index++)
	{
		dpst_B_Cob = pst_B_ColMap->dpst_Cob;
		dpst_B_LastCob = dpst_B_Cob + pst_B_ColMap->uc_NbOfCob;

		if(COL_b_Instance_IsActive(pst_A_Instance, uc_A_Index) && COL_b_Zone_TestFlag(*dpst_A_ZDx, COL_C_Zone_ZDM))
		{
			if(!b_Skip)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~*/
				OBJ_tdst_GameObject *pst_B;
				/*~~~~~~~~~~~~~~~~~~~~~~~*/

				b_Skip = TRUE;
				pst_B = _pst_GlobalVars->pst_B;

				if(MATH_b_TestScaleType(pst_B->pst_GlobalMatrix))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Matrix	*pst_InvBmat, *pst_Bmat;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_Bmat = pst_B->pst_GlobalMatrix;
					pst_InvBmat = &_pst_GlobalVars->st_B_InvGlobalMatrix;
					MATH_Transp33MatrixWithoutBuffer(pst_InvBmat, pst_Bmat);
					MATH_SetType(pst_InvBmat, MATH_l_GetType(pst_Bmat));

					if(MATH_b_TestScaleType(pst_Bmat))
					{
						pst_InvBmat->Sx = fInv(pst_Bmat->Sx);
						pst_InvBmat->Sy = fInv(pst_Bmat->Sy);
						pst_InvBmat->Sz = fInv(pst_Bmat->Sz);
					}

					MATH_NegVector(MATH_pst_GetTranslation(pst_InvBmat), MATH_pst_GetTranslation(pst_Bmat));
					MATH_TransformVectorNoScale(&pst_InvBmat->T, pst_InvBmat, &pst_InvBmat->T);
				}
				else
					MATH_InvertMatrix(&_pst_GlobalVars->st_B_InvGlobalMatrix, pst_B->pst_GlobalMatrix);
			}

			uc_A_Type = (*dpst_A_ZDx)->uc_Type;

			/* Update Global Vars. */
			_pst_GlobalVars->pst_A_ZDx = *dpst_A_ZDx;

			/* Compute A Stuff in BCS */
			COL_A_BCS_ComputeZDxInfo(_pst_GlobalVars);

			switch(uc_A_Type)
			{
			case COL_C_Zone_Box:
				/* Loop thru all the B's ZDRs */
				for(uc_B_Index = 0; dpst_B_Cob < dpst_B_LastCob; dpst_B_Cob++, uc_B_Index++)
				{
					if(COL_b_ColMap_IsActive(pst_B_ColMap, uc_B_Index))
					{
						/* Update Global Vars. */
						_pst_GlobalVars->pst_B_Cob = *dpst_B_Cob;
						COL_ZDM_ZDR(_pst_GlobalVars);
					}
				}
				break;

			case COL_C_Zone_Sphere:

				/* Loop thru all the B's ZDRs */
				for(uc_B_Index = 0; dpst_B_Cob < dpst_B_LastCob; dpst_B_Cob++, uc_B_Index++)
				{
					if(COL_b_ColMap_IsActive(pst_B_ColMap, uc_B_Index))
					{
						/* Update Global Vars. */
						_pst_GlobalVars->pst_B_Cob = *dpst_B_Cob;
						COL_ZDM_ZDR(_pst_GlobalVars);
					}
				}
				break;
			}
		}
		else
			continue;
	}

	return TRUE;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
