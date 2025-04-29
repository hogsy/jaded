/*$T COLMain.c GC! 1.081 05/30/02 15:30:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$F GC Dependencies 11/22/99 */
#include "Precomp.h"

#include "BASe/MEMory/MEM.h"

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "MATHs/MATH.h"

#include "TABles/TABles.h"

#include "OBJects/OBJstruct.h"
#include "OBJects/OBJconst.h"
#include "OBJects/OBJaccess.h"
#include "OBJects/OBJmain.h"
#include "OBJects/OBJorient.h"
#include "OBJects/OBJBoundingVolume.h"
#include "OBJects/OBJslowaccess.h"

#include "ANImation/ANImain.h"
#include "ANImation/ANIstruct.h"
#include "ANImation/ANIaccess.h"

#include "INTersection/INTSnP.h"
#include "INTersection/INTmain.h"

#include "WORld/WORaccess.h"

#include "COLstruct.h"
#include "COLconst.h"
#include "COLaccess.h"
#include "COLvars.h"
#include "COLactor.h"
#include "COLreport.h"
#include "COLcob.h"
#include "COLload.h"
#include "COLinit.h"

#include "GraphicDK/Sources/GEOmetric/GEOobject.h"
#include "WORld/WORstruct.h"

#include "MoDiFier/MDFstruct.h"

/* Temp */
#include "BASe/MEMory/MEM.h"
#include "COLsave.h"

/* End Temp */
#if defined(__cplusplus)
extern "C"
{
#endif

/* Global variables of the COL module. */
BOOL				COL_gb_SpecialCamera = FALSE;
COL_tdst_GlobalVars COL_gst_GlobalVars;
BOOL				COL_gb_LastLoop;
BOOL				COL_gb_AfterEngine = FALSE;
UCHAR				COL_uc_RecomputingLoop = 0;

ULONG				COL_ul_LOD_NumberOfLoops[4] = { { 0 }, { 2 }, { 4 }, { 6 } };

#ifdef ACTIVE_EDITORS
UCHAR		COL_s_GhostOptimisation = 0;
#endif

extern void MATH_CheckFloat(float *, char *);
extern void MATH_CheckVector(struct MATH_tdst_Vector_ *, const char *);
extern void MATH_CheckMatrix(struct MATH_tdst_Matrix_ *, char *);



/*
 =======================================================================================================================
    Aim:
 =======================================================================================================================
 */
BOOL COL_b_SingleBVOverlap(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_SingleBV	*pst_A_BV, *pst_B_BV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_A || !_pst_B) return FALSE;

	pst_A_BV = (OBJ_tdst_SingleBV *) _pst_A->pst_BV;
	pst_B_BV = (OBJ_tdst_SingleBV *) _pst_B->pst_BV;

	if(OBJ_BV_IsAABBox(pst_A_BV) && OBJ_BV_IsAABBox(pst_B_BV))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_A_Min, st_A_Max;
		MATH_tdst_Vector	st_B_Min, st_B_Max;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_AddVector(&st_A_Min, &pst_A_BV->st_GMin, &_pst_A->pst_GlobalMatrix->T);
		MATH_AddVector(&st_A_Max, &pst_A_BV->st_GMax, &_pst_A->pst_GlobalMatrix->T);

		MATH_AddVector(&st_B_Min, &pst_B_BV->st_GMin, &_pst_B->pst_GlobalMatrix->T);
		MATH_AddVector(&st_B_Max, &pst_B_BV->st_GMax, &_pst_B->pst_GlobalMatrix->T);

		return
			(
				fSup (st_A_Min.x, st_B_Max.x)
			||	fInf (st_A_Max.x, st_B_Min.x)
			||	fSup (st_A_Min.y, st_B_Max.y)
			||	fInf (st_A_Max.y, st_B_Min.y)
			||	fSup (st_A_Min.z, st_B_Max.z)
			||	fInf (st_A_Max.z, st_B_Min.z)
			) ? FALSE : TRUE;
	}

	if(OBJ_BV_IsAABBox(pst_A_BV) && OBJ_BV_IsSphere(pst_B_BV))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Max, st_Min;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_A_BV), OBJ_pst_GetAbsolutePosition(_pst_A));
		MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_A_BV), OBJ_pst_GetAbsolutePosition(_pst_A));

		return INT_SphereAABBox(OBJ_pst_BV_GetCenter(pst_B_BV), OBJ_f_BV_GetRadius(pst_B_BV), &st_Min, &st_Max);
	}

	if(OBJ_BV_IsSphere(pst_A_BV) && OBJ_BV_IsAABBox(pst_B_BV))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Max, st_Min;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_B_BV), OBJ_pst_GetAbsolutePosition(_pst_B));
		MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_B_BV), OBJ_pst_GetAbsolutePosition(_pst_B));

		return INT_SphereAABBox(OBJ_pst_BV_GetCenter(pst_A_BV), OBJ_f_BV_GetRadius(pst_A_BV), &st_Min, &st_Max);
	}

	if(OBJ_BV_IsSphere(pst_A_BV) && OBJ_BV_IsSphere(pst_B_BV))
	{
		return INT_SphereSphere
			(
				OBJ_pst_BV_GetCenter(pst_A_BV),
				OBJ_f_BV_GetRadius(pst_A_BV),
				OBJ_pst_BV_GetCenter(pst_B_BV),
				OBJ_f_BV_GetRadius(pst_B_BV)
			);
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Returns whether the BV of the 2 objects are overlapping.
 =======================================================================================================================
 */
BOOL COL_b_BVOverlap
(
	OBJ_tdst_GameObject *_pst_A,
	OBJ_tdst_GameObject *_pst_B,
	BOOL				_b_A_UseOBBox,
	BOOL				_b_B_UseOBBox,
	BOOL				_b_UseSnP
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Temp;
	BOOL				b_Temp, b_Overlap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_A || !_pst_B) return FALSE;
	
	
#if defined(_DEBUG) && defined(_GAMECUBE)
		if(
			((*(int *) &_pst_A->pst_GlobalMatrix->T.x) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_A->pst_GlobalMatrix->T.y) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_A->pst_GlobalMatrix->T.z) == (int) 0xFFC00000)
		||	((*(int *) &_pst_A->pst_GlobalMatrix->T.x) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_A->pst_GlobalMatrix->T.y) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_A->pst_GlobalMatrix->T.z) == (int) 0x7FC00000)
		
		)
		OSReport("BV error \n");
		
		if(
			((*(int *) &_pst_B->pst_GlobalMatrix->T.x) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_B->pst_GlobalMatrix->T.y) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_B->pst_GlobalMatrix->T.z) == (int) 0xFFC00000)
		||	((*(int *) &_pst_B->pst_GlobalMatrix->T.x) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_B->pst_GlobalMatrix->T.y) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_B->pst_GlobalMatrix->T.z) == (int) 0x7FC00000)
		
		)
		OSReport("BV error \n");
		
#endif			
	

	if
	(
		_b_UseSnP
	&&	(
			!(_pst_A->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP) || !
				(_pst_B->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP)
		)
	) _b_UseSnP = FALSE;

	if
	(
		_b_UseSnP
	&&	(!OBJ_b_TestStatusFlag(_pst_A, OBJ_C_StatusFlag_Detection))
	&&	(!OBJ_b_TestStatusFlag(_pst_B, OBJ_C_StatusFlag_Detection))
	) _b_UseSnP = FALSE;

	/* If A or B has no OBBox, we cannot use them !!! */
	if(!OBJ_b_TestIdentityFlag(_pst_A, OBJ_C_IdentityFlag_OBBox)) _b_A_UseOBBox = FALSE;

	if(!OBJ_b_TestIdentityFlag(_pst_B, OBJ_C_IdentityFlag_OBBox)) _b_B_UseOBBox = FALSE;

	if(!OBJ_b_TestStatusFlag(_pst_A, OBJ_C_StatusFlag_Detection))
	{
		b_Temp = _b_A_UseOBBox;
		pst_Temp = _pst_A;

		_b_A_UseOBBox = _b_B_UseOBBox;
		_pst_A = _pst_B;

		_b_B_UseOBBox = b_Temp;
		_pst_B = pst_Temp;
	}

	if(_b_UseSnP)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG			ul_RefA, ul_RefB;
		WOR_tdst_World	*pst_World;
		INT_tdst_SnP	*pst_SnP;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_World = WOR_World_GetWorldOfObject(_pst_A);
		pst_SnP = pst_World->pst_SnP;

		ul_RefA = (ULONG) _pst_A->us_SnP_Ref;
		ul_RefB = (ULONG) _pst_B->us_SnP_Ref;

		/*$off*/
		b_Overlap = pst_SnP->apst_AxisTable[ 0 ]->flags.Get( ul_RefA, ul_RefB )
			&& pst_SnP->apst_AxisTable[ 1 ]->flags.Get( ul_RefA, ul_RefB )
			&& pst_SnP->apst_AxisTable[ 2 ]->flags.Get( ul_RefA, ul_RefB );

		/*$on*/
	}
	else
	{
		b_Overlap = COL_b_SingleBVOverlap(_pst_A, _pst_B);
	}

	if(b_Overlap)
	{
		if(!_b_A_UseOBBox && !_b_B_UseOBBox)
			return TRUE;
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix				st_A_NoScaleMatrix, st_B_NoScaleMatrix;
			MATH_tdst_Matrix st_InvAMatrix	ONLY_PSX2_ALIGNED(16);
			MATH_tdst_Matrix				st_TransMatrix;
			MATH_tdst_Vector				*pst_A_Max, *pst_A_Min = NULL;
			MATH_tdst_Vector				*pst_B_Max, *pst_B_Min = NULL;
			MATH_tdst_Vector				st_A_Center, st_B_Center, st_Distance, st_T, st_A_Scale;
			MATH_tdst_Vector				st_A_GCS_Center, st_B_GCS_Center;
			MATH_tdst_Vector				st_A_Max, st_A_Min, st_B_Max, st_B_Min;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_A_Max = NULL;
			pst_B_Max = NULL;

			if(_b_A_UseOBBox && _b_B_UseOBBox)
			{
				if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_A)))
				{
					MATH_CopyMatrix(&st_A_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_A));
					MATH_ClearScale(&st_A_NoScaleMatrix, 1);
					MATH_InvertMatrix(&st_InvAMatrix, &st_A_NoScaleMatrix);
				}
				else
					MATH_InvertMatrix(&st_InvAMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_A));

				if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_B)))
				{
					MATH_CopyMatrix(&st_B_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_B));
					MATH_ClearScale(&st_B_NoScaleMatrix, 1);
				}
				else
				{
					MATH_CopyMatrix(&st_B_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_B));
				}

				MATH_MulMatrixMatrix(&st_TransMatrix, &st_B_NoScaleMatrix, &st_InvAMatrix);
				pst_A_Max = OBJ_pst_BV_GetLMax(_pst_A->pst_BV);
				pst_A_Min = OBJ_pst_BV_GetLMin(_pst_A->pst_BV);
				pst_B_Max = OBJ_pst_BV_GetLMax(_pst_B->pst_BV);
				pst_B_Min = OBJ_pst_BV_GetLMin(_pst_B->pst_BV);
			}

			if(_b_A_UseOBBox && !_b_B_UseOBBox)
			{
				if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_A)))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Matrix	st_A_NoScaleMatrix;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_CopyMatrix(&st_A_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_A));
					MATH_ClearScale(&st_A_NoScaleMatrix, 1);
					MATH_InvertMatrix(&st_InvAMatrix, &st_A_NoScaleMatrix);
				}
				else
					MATH_InvertMatrix(&st_InvAMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_A));
				MATH_CopyMatrix(&st_TransMatrix, &st_InvAMatrix);

				pst_A_Max = OBJ_pst_BV_GetLMax(_pst_A->pst_BV);
				pst_A_Min = OBJ_pst_BV_GetLMin(_pst_A->pst_BV);
				MATH_AddVector(&st_B_Max, OBJ_pst_BV_GetGMax(_pst_B->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_B));
				MATH_AddVector(&st_B_Min, OBJ_pst_BV_GetGMin(_pst_B->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_B));
				pst_B_Max = &st_B_Max;
				pst_B_Min = &st_B_Min;
			}

			if(!_b_A_UseOBBox && _b_B_UseOBBox)
			{
				if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_B)))
				{
					MATH_CopyMatrix(&st_B_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_B));
					MATH_ClearScale(&st_B_NoScaleMatrix, 1);
					MATH_CopyMatrix(&st_TransMatrix, &st_B_NoScaleMatrix);
				}
				else
				{
					MATH_CopyMatrix(&st_TransMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_B));
				}

				MATH_AddVector(&st_A_Max, OBJ_pst_BV_GetGMax(_pst_A->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_A));
				MATH_AddVector(&st_A_Min, OBJ_pst_BV_GetGMin(_pst_A->pst_BV), OBJ_pst_GetAbsolutePosition(_pst_A));
				pst_A_Max = &st_A_Max;
				pst_A_Min = &st_A_Min;
				pst_B_Max = OBJ_pst_BV_GetLMax(_pst_B->pst_BV);
				pst_B_Min = OBJ_pst_BV_GetLMin(_pst_B->pst_BV);
			}

			/*$F ---- A Center Computation --------- */
			MATH_SubVector(&st_A_Center, pst_A_Max, pst_A_Min);
			MATH_MulEqualVector(&st_A_Center, 0.5f);
			MATH_AddEqualVector(&st_A_Center, pst_A_Min);

			/*$F ---- A Center in GCS -------------- */
			if(_b_A_UseOBBox)
				MATH_TransformVertexNoScale(&st_A_GCS_Center, OBJ_pst_GetAbsoluteMatrix(_pst_A), &st_A_Center);
			else
				MATH_CopyVector(&st_A_GCS_Center, &st_A_Center);

			/*$F ---- B Center Computation --------- */
			MATH_SubVector(&st_B_Center, pst_B_Max, pst_B_Min);
			MATH_MulEqualVector(&st_B_Center, 0.5f);
			MATH_AddEqualVector(&st_B_Center, pst_B_Min);

			/*$F ---- B Center in GCS -------------- */
			if(_b_B_UseOBBox)
				MATH_TransformVertexNoScale(&st_B_GCS_Center, OBJ_pst_GetAbsoluteMatrix(_pst_B), &st_B_Center);
			else
				MATH_CopyVector(&st_B_GCS_Center, &st_B_Center);

			/*$F ---- Real Translation between A Center and B Center ----------- */
			MATH_SubVector(&st_Distance, &st_B_GCS_Center, &st_A_GCS_Center);

			/* The Translation vector that is in the GCS. We want it to be in the ACS. */
			if(_b_A_UseOBBox)
				MATH_TransformVectorNoScale(&st_T, &st_InvAMatrix, &st_Distance);
			else
				MATH_CopyVector(&st_T, &st_Distance);
			MATH_CopyVector(MATH_pst_GetTranslation(&st_TransMatrix), &st_T);

			/* The scale info is already in the OBBox structure. So, we dont deal with it. */
			MATH_InitVector(&st_A_Scale, 1.0f, 1.0f, 1.0f);

			return(INT_HighLevel_OBBoxOBBox(pst_A_Min, pst_A_Max, pst_B_Min, pst_B_Max, &st_TransMatrix, &st_A_Scale));
		}
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:
 =======================================================================================================================
 */
BOOL COL_b_SingleBVIn(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_SingleBV	*pst_A_BV, *pst_B_BV;
	MATH_tdst_Vector	V, min, Max;
	float				f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_A || !_pst_B) return FALSE;

	pst_A_BV = (OBJ_tdst_SingleBV *) _pst_A->pst_BV;
	pst_B_BV = (OBJ_tdst_SingleBV *) _pst_B->pst_BV;

	if(OBJ_BV_IsAABBox(pst_A_BV))
	{
		MATH_SubVector(&V, OBJ_pst_GetAbsolutePosition(_pst_B), OBJ_pst_GetAbsolutePosition(_pst_A));

		if(OBJ_BV_IsAABBox(pst_A_BV))
		{
			MATH_AddVector(&min, &pst_B_BV->st_GMin, &V);
			MATH_AddVector(&Max, &pst_B_BV->st_GMax, &V);
		}
		else if(OBJ_BV_IsSphere(pst_B_BV))
		{
			f = OBJ_f_BV_GetRadius(pst_B_BV);
			MATH_CopyVector(&min, OBJ_pst_BV_GetCenter(pst_B_BV));
			Max.x = V.x + min.x + f;
			Max.y = V.y + min.y + f;
			Max.z = V.z + min.z + f;
			min.x -= f - V.y;
			min.y -= f - V.y;
			min.z -= f - V.z;
		}
		else
			return FALSE;

		return
			(
				fSup (pst_A_BV->st_GMin.x, min.x)
			||	fInf (pst_A_BV->st_GMax.x, Max.x)
			||	fSup (pst_A_BV->st_GMin.y, min.y)
			||	fInf (pst_A_BV->st_GMax.y, Max.y)
			||	fSup (pst_A_BV->st_GMin.z, min.z)
			||	fInf (pst_A_BV->st_GMax.z, Max.z)
			) ? FALSE : TRUE;
	}

	if(OBJ_BV_IsSphere(pst_A_BV))
	{
		if(OBJ_BV_IsAABBox(pst_A_BV))
		{
			MATH_SubVector(&Max, &pst_A_BV->st_GMax, &pst_A_BV->st_GMin);
			MATH_ScaleEqualVector(&Max, 0.5f);
			MATH_AddVector(&min, OBJ_pst_GetAbsolutePosition(_pst_B), &Max);

			MATH_SubVector(&V, &min, OBJ_pst_BV_GetCenter(pst_A_BV));
			min.x += (min.x < 0) ? -Max.x : Max.x;
			min.y += (min.y < 0) ? -Max.y : Max.y;
			min.z += (min.z < 0) ? -Max.z : Max.z;

			f = OBJ_f_BV_GetRadius(pst_A_BV);
			return(MATH_f_SqrVector(&min) < (f * f));
		}
		else if(OBJ_BV_IsSphere(pst_B_BV))
		{
			if(OBJ_f_BV_GetRadius(pst_A_BV) < OBJ_f_BV_GetRadius(pst_B_BV)) return FALSE;

			f = MATH_f_Distance(OBJ_pst_BV_GetCenter(pst_A_BV), OBJ_pst_BV_GetCenter(pst_B_BV));

			return(f + OBJ_f_BV_GetRadius(pst_B_BV) < OBJ_f_BV_GetRadius(pst_A_BV));
		}
		else
			return FALSE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Returns whether the BV of the 2 objects are overlapping.
 =======================================================================================================================
 */
BOOL COL_b_BVIn(OBJ_tdst_GameObject *_pst_A, OBJ_tdst_GameObject *_pst_B)
{
	if(!_pst_A || !_pst_B) return FALSE;

	return COL_b_SingleBVIn(_pst_A, _pst_B);
}

/*$F
 ===================================================================================================
    Aim:    Update all the global variables concerning a particular collision case.

    Note:   When you enter this function, you are sure that:
				- A is  (at least) ZDM Collide. (So, it has a valid COL_tdst_Instance allocated)
				- B has (at least) a ColMap.	(So, it has a valid COL_tdst_Instance allocated)
 ===================================================================================================
 */

void COL_InitGlobalVars(OBJ_tdst_GameObject *pst_A, OBJ_tdst_GameObject *pst_B)
{
	COL_gst_GlobalVars.pst_A = pst_A;
	COL_gst_GlobalVars.pst_A_OldGlobalMatrix = ((COL_tdst_Base *) pst_A->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix;
	COL_gst_GlobalVars.pst_A_GlobalMatrix = pst_A->pst_GlobalMatrix;
	COL_gst_GlobalVars.pst_B = pst_B;
	COL_gst_GlobalVars.pst_B_GlobalMatrix = pst_B->pst_GlobalMatrix;
}

/*
 =======================================================================================================================
    Aim:    Updates all the GameObjects (that have an COL_tdst_Instance structure) Old Frame matrix.

    Note:   Also recals the GameObjects inside the Walls.
 =======================================================================================================================
 */
void COL_UpdateOldMatrix(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	COL_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Instance = NULL;

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ZDM);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ZDM);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ZDM))
		{
			pst_Instance = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance;

			/* Resets the User BackFace (must be set each frame IA wants it) */
			pst_Instance->uc_UserBackFace = 0;

			if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
				MATH_CopyMatrix(pst_Instance->pst_OldGlobalMatrix, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
			else
				MATH_CopyMatrix(pst_Instance->pst_OldGlobalMatrix, pst_GO->pst_GlobalMatrix);

		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_RecomputeAllVisualOK3(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				*pst_CurrentElem;
	ULONG				*pst_EndElem;
	GRO_tdst_Struct		*pst_Struct;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = (ULONG *)TAB_ppv_Ptable_GetFirstElem(&_pst_World->st_GraphicObjectsTable);
	pst_EndElem = (ULONG *)TAB_ppv_Ptable_GetLastElem(&_pst_World->st_GraphicObjectsTable);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
        if(TAB_b_IsAHole((void *) *pst_CurrentElem)) continue;

		pst_Struct = (GRO_tdst_Struct *) *pst_CurrentElem;
		if(pst_Struct->i->ul_Type != GRO_Geometric) continue;

		/* Force recomputation */
		COL_OK3_Build((GEO_tdst_Object *)pst_Struct, FALSE, TRUE);
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_UpdateBV(TAB_tdst_PFtable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Matrix	*pst_OldMatrix;
	MATH_tdst_Vector	*pst_Min, *pst_Max;
	MATH_tdst_Vector	st_Delta;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if((TAB_b_IsAHole(pst_GO)) || !(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ZDM))) continue;
		if(OBJ_BV_IsSphere(pst_GO->pst_BV)) continue;

		pst_OldMatrix = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix;

		if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && pst_GO->pst_Base && pst_GO->pst_Base->pst_Hierarchy && pst_GO->pst_Base->pst_Hierarchy->pst_Father)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix	st_Matrix;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_MulMatrixMatrix
			(
				&st_Matrix,
				pst_OldMatrix,
				pst_GO->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
			);
			MATH_SubVector(&st_Delta, OBJ_pst_GetAbsolutePosition(pst_GO), &st_Matrix.T);
		}
		else
			MATH_SubVector(&st_Delta, OBJ_pst_GetAbsolutePosition(pst_GO), &pst_OldMatrix->T);

		pst_Min = OBJ_pst_BV_GetGMin(pst_GO->pst_BV);
		pst_Max = OBJ_pst_BV_GetGMax(pst_GO->pst_BV);

		MATH_CopyVector(pst_Min, OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));
		MATH_CopyVector(pst_Max, OBJ_pst_BV_GetGMaxInit(pst_GO->pst_BV));

		if(st_Delta.x > 0.0f)
			pst_Min->x -= st_Delta.x;
		else
			pst_Max->x -= st_Delta.x;

		if(st_Delta.y > 0.0f)
			pst_Min->y -= st_Delta.y;
		else
			pst_Max->y -= st_Delta.y;

		if(st_Delta.z > 0.0f)
			pst_Min->z -= st_Delta.z;
		else
			pst_Max->z -= st_Delta.z;
	}
}

extern struct OBJ_tdst_GameObject_	**AI_gpst_MainActors;

#ifdef ACTIVE_EDITORS
ULONG								COL_ul_ParsedFaces;
ULONG								COL_ul_ParsedFacesOK3;
ULONG								COL_ul_ComputedFaces;
ULONG								COL_ul_SnP_Permutations;
#endif
#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_Check_Cobs(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	TAB_tdst_PFelem		*pst_CurrentElem2;
	TAB_tdst_PFelem		*pst_EndElem2;
	OBJ_tdst_GameObject *pst_GO, *pst_GO2;
	COL_tdst_Cob		*pst_Cob;
	char				asz_Log[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		if
		(
			!pst_GO
		||	!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
		||	!(pst_GO->pst_Extended)
		||	!(pst_GO->pst_Extended->pst_Col)
		||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap
		||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob
		||	(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles)
		||	(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uw_NbOfInstances == 1)
		) continue;

		pst_Cob = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0];

		pst_CurrentElem2 = pst_CurrentElem + 1;
		pst_EndElem2 = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
		for(; pst_CurrentElem2 <= pst_EndElem2; pst_CurrentElem2++)
		{
			pst_GO2 = (OBJ_tdst_GameObject *) pst_CurrentElem2->p_Pointer;
			if(TAB_b_IsAHole(pst_GO2)) continue;

			if
			(
				!pst_GO2
			||	!(pst_GO2->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
			||	(pst_GO2 == pst_GO)
			||	!(pst_GO2->pst_Extended)
			||	!(pst_GO2->pst_Extended->pst_Col)
			||	!((COL_tdst_Base *) pst_GO2->pst_Extended->pst_Col)->pst_ColMap
			||	!((COL_tdst_Base *) pst_GO2->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob
			||	(((COL_tdst_Base *) pst_GO2->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles)
			||	(((COL_tdst_Base *) pst_GO2->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uw_NbOfInstances == 1)
			||  (((COL_tdst_Base *) pst_GO2->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0] != pst_Cob)
			) continue;

			if(
				(pst_GO->pst_GlobalMatrix->Sx != pst_GO2->pst_GlobalMatrix->Sx)
			||	(pst_GO->pst_GlobalMatrix->Sy != pst_GO2->pst_GlobalMatrix->Sy)
			||	(pst_GO->pst_GlobalMatrix->Sz != pst_GO2->pst_GlobalMatrix->Sz)
				)
			{

				sprintf(asz_Log, "%s and %s share the same cob but have different scale. ==> THERE WILL BE COLLISION ERRORS", pst_GO->sz_Name, pst_GO2->sz_Name);
				ERR_X_Warning(0, asz_Log, NULL);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_OK3_Display_Refresh(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Geo;
	COL_tdst_OK3		*pst_OK3;
	COL_tdst_OK3_Box	*pst_OK3_Box;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ColMap);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ColMap);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		if
		(
			!pst_GO
		||	!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)
		||	!(pst_GO->pst_Extended)
		||	!(pst_GO->pst_Extended->pst_Col)
		||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap
		||	!((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob
		||	(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles)
		||	!(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->pst_TriangleCob->pst_OK3)
		) continue;

		pst_OK3 = (((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->pst_TriangleCob->pst_OK3);

		for(i = 0; i < (int) pst_OK3->ul_NumBox; i++)
		{
			pst_OK3_Box = &pst_OK3->pst_OK3_Boxes[i];
			pst_OK3_Box->ul_OK3_Flag = 0;
		}
	}

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_Visu);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_Visu);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
		if(pst_Geo && pst_Geo->pst_OK3)
		{
			pst_OK3 = pst_Geo->pst_OK3;

			for(i = 0; i < (int) pst_OK3->ul_NumBox; i++)
			{
				pst_OK3_Box = &pst_OK3->pst_OK3_Boxes[i];
				pst_OK3_Box->ul_OK3_Flag = 0;
			}
		}
	}
}

#endif

/*
 =======================================================================================================================
    Aim:    Call the collision module JUST for one object (AfterRec Camera case for example)
 =======================================================================================================================
 */
void COL_OneCall(OBJ_tdst_GameObject *pst_GO, ULONG _ul_FirstReport)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Base		*pst_ColBase;
	OBJ_tdst_GameObject **ppst_Object, **ppst_LastObject;
	WOR_tdst_World		*pst_World;
	BOOL				b_InactiveZDM;
	int					i_MaxLoop, i_Loop;
	UCHAR				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_GO == WOR_gpst_CurrentWorld->pst_View->pst_Father)
		COL_gb_SpecialCamera = TRUE;
	else
		COL_gb_SpecialCamera = FALSE;

	/* Update GameObject BV */
	if(OBJ_BV_IsAABBox(pst_GO->pst_BV))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	*pst_OldMatrix;
		MATH_tdst_Vector	*pst_Min, *pst_Max;
		MATH_tdst_Vector	st_Delta;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_OldMatrix = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix;

		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix	st_Matrix;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_MulMatrixMatrix
			(
				&st_Matrix,
				pst_OldMatrix,
				pst_GO->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
			);
			MATH_SubVector(&st_Delta, OBJ_pst_GetAbsolutePosition(pst_GO), &st_Matrix.T);
		}
		else
			MATH_SubVector(&st_Delta, OBJ_pst_GetAbsolutePosition(pst_GO), &pst_OldMatrix->T);

		pst_Min = OBJ_pst_BV_GetGMin(pst_GO->pst_BV);
		pst_Max = OBJ_pst_BV_GetGMax(pst_GO->pst_BV);

		MATH_CopyVector(pst_Min, OBJ_pst_BV_GetGMinInit(pst_GO->pst_BV));
		MATH_CopyVector(pst_Max, OBJ_pst_BV_GetGMaxInit(pst_GO->pst_BV));

		if(st_Delta.x > 0.0f)
			pst_Min->x -= st_Delta.x;
		else
			pst_Max->x -= st_Delta.x;

		if(st_Delta.y > 0.0f)
			pst_Min->y -= st_Delta.y;
		else
			pst_Max->y -= st_Delta.y;

		if(st_Delta.z > 0.0f)
			pst_Min->z -= st_Delta.z;
		else
			pst_Max->z -= st_Delta.z;
	}

	COL_gb_LastLoop = FALSE;
	COL_uc_RecomputingLoop = 0;
	COL_gst_GlobalVars.ul_FirstRecomputingReport = COL_gst_GlobalVars.ul_ReportIndex;
	COL_gst_GlobalVars.b_Recomputing = TRUE;

	if(!OBJ_b_TestStatusFlag(pst_GO, OBJ_C_StatusFlag_Active)) return;
	if(OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_ForceInactive)) return;

	COL_gb_AfterEngine = TRUE;

	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	/* SnP Refresh */
	INT_SnP_InsertionSort(pst_World->pst_SnP, COL_SnP_RefreshXYZ);

	pst_ColBase = (COL_tdst_Base *) pst_GO->pst_Extended->pst_Col;

	if(!OBJ_b_TestStatusFlag(pst_GO, OBJ_C_StatusFlag_Detection))
		return;
	else
		pst_ColBase = (COL_tdst_Base *) pst_GO->pst_Extended->pst_Col;

	/* If the Actor has no Active ZDM, we return; */
	b_InactiveZDM = TRUE;
	for(i = 0; i < pst_ColBase->pst_Instance->uc_NbOfZDx; i++)
	{
		if
		(
			COL_b_Zone_TestFlag(pst_ColBase->pst_Instance->dpst_ZDx[i], COL_C_Zone_ZDM)
		&&	COL_b_Instance_IsActive(pst_ColBase->pst_Instance, i)
		)
		{
			b_InactiveZDM = FALSE;

#ifdef _DEBUG
			if(!(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP))
			{
				if(pst_World)
				{
					pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
					INT_SnP_AddObject(pst_GO, pst_World->pst_SnP);
					INT_SnP_InsertionSort(pst_World->pst_SnP, 0);
				}

				ERR_X_Warning(0, "EnableSnP Control Flag set for GO. SAVE the map", pst_GO->sz_Name);
			}

#endif
			break;
		}
	}

	if(b_InactiveZDM) return;

	/* If there is no overlapping object given by the SnP, we return. */
	if(!pst_ColBase->pst_List->ul_NbCollidedObjects) return;

	/* Resets Instance Flag for this GO. */
	((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags = 0;

	if(pst_GO->pst_Base && pst_GO->pst_Base->pst_Dyna)
		COL_gst_GlobalVars.f_MaxStepSize = pst_GO->pst_Base->pst_Dyna->f_MaxStepSize;
	else
		COL_gst_GlobalVars.f_MaxStepSize = -1.0f;	/* Negative value to skip the test */

	/*$F
			 -------------------------------------------------------------------------------------

								--- CLASSICAL CASE ---

								  The Actor has moved.

			 -------------------------------------------------------------------------------------
		*/
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_A_GCS_Speed, st_Delta;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		DYN_GetSpeedVector(OBJ_pst_GetDynaStruct(pst_GO), &st_A_GCS_Speed);

		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix	st_Matrix;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_MulMatrixMatrix
			(
				&st_Matrix,
				pst_ColBase->pst_Instance->pst_OldGlobalMatrix,
				pst_GO->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
			);
			MATH_SubVector(&st_Delta, &st_Matrix.T, &pst_GO->pst_GlobalMatrix->T);
		}
		else
		{
			MATH_SubVector
			(
				&st_Delta,
				&(pst_ColBase->pst_Instance->pst_OldGlobalMatrix->T),
				&pst_GO->pst_GlobalMatrix->T
			);
		}

		if
		(
			(pst_GO->pst_Base->pst_Dyna->ul_DynFlags & DYN_C_OptimizeColEnable)
		&&	(fAbs(st_A_GCS_Speed.x) < 1E-3f)
		&&	(fAbs(st_A_GCS_Speed.y) < 1E-3f)
		&&	(fAbs(st_Delta.x) < 1E-3f)
		&&	(fAbs(st_Delta.y) < 1E-3f)
		)
		{
			((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags |= COL_Cul_OnlyGravity;
		}
		else
			((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags &= ~COL_Cul_OnlyGravity;

		pst_GO->pst_Base->pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColDisable;
		pst_GO->pst_Base->pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColEnable;
	}

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy)) OBJ_ComputeGlobalWhenHie(pst_GO);

	/* We go thru al the overlapping objects ... */
	ppst_Object = pst_ColBase->pst_List->dpst_CollidedObject;
	ppst_LastObject = ppst_Object + pst_ColBase->pst_List->ul_NbCollidedObjects - 1;

	for(; ppst_Object <= ppst_LastObject; ppst_Object++)
	{
		/* If the Overlapping object has no ColMap or is Inactive, we return; */
		if
		(
			(!OBJ_b_TestIdentityFlag(*ppst_Object, OBJ_C_IdentityFlag_ColMap))
		||	!(OBJ_b_TestStatusFlag(*ppst_Object, OBJ_C_StatusFlag_Active))
		||  (!OBJ_b_TestFlag(*ppst_Object, pst_ColBase->pst_Instance->ul_Filter_On, pst_ColBase->pst_Instance->ul_Filter_Off, pst_ColBase->pst_Instance->ul_Filter_Type))
		)
		{
			continue;
		}
		else
		{
			if
			(
				((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			&&	((*ppst_Object)->pst_Base->pst_Hierarchy->pst_Father == pst_GO)
			)
			{
				continue;
			}

			/* Uncollidable Objects */
			if(COL_b_UnCollidable(pst_GO, *ppst_Object))
				continue;


#ifdef ODE_INSIDE
				if
				(
					(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
				&&	((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
				&&	(pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
				&&	((*ppst_Object)->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
				&&  
					(
						!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
					||  !((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
					)

				)
				continue;
#endif

			COL_InitGlobalVars(pst_GO, *ppst_Object);
			COL_Actor_GameObject(&COL_gst_GlobalVars);
		}
	}

	COL_ComputeReports(&COL_gst_GlobalVars, _ul_FirstReport);

	/* Recompute Collision for some GOs ? */
	i_MaxLoop = 6;
	i_Loop = 0;
	while(COL_gst_GlobalVars.uc_RecomputeGO && (i_Loop < i_MaxLoop))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject		*pst_GO;
		COL_tdst_DetectionList	*pst_List;
		OBJ_tdst_GameObject		**ppst_Object, **ppst_LastObject;
		int						i, uc_Loop;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		i_Loop++;

		COL_uc_RecomputingLoop = i_Loop;

		COL_gb_LastLoop = (i_Loop == i_MaxLoop);

		COL_gst_GlobalVars.ul_FirstRecomputingReport = COL_gst_GlobalVars.ul_ReportIndex;

		uc_Loop = COL_gst_GlobalVars.uc_RecomputeGO;
		COL_gst_GlobalVars.uc_RecomputeGO = 0;

		for(i = 0; i < uc_Loop; i++)
		{
			pst_GO = COL_gst_GlobalVars.apst_RecomputeGO[i];

			/* Resets some Instance Flags for this GO. */
			((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags &= ~COL_Cul_StaticGround;
			((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags &= ~COL_Cul_Recompute_Mask;

			if(pst_GO->pst_Base && pst_GO->pst_Base->pst_Dyna)
				COL_gst_GlobalVars.f_MaxStepSize = pst_GO->pst_Base->pst_Dyna->f_MaxStepSize;
			else
				COL_gst_GlobalVars.f_MaxStepSize = -1.0f;	/* Negative value to skip the test */

			pst_List = COL_pst_GetDetectionList(pst_GO, FALSE);

			/* Else, we go thru al the overlapping objects ... */
			ppst_Object = pst_List->dpst_CollidedObject;
			ppst_LastObject = ppst_Object + pst_List->ul_NbCollidedObjects - 1;

			for(; ppst_Object <= ppst_LastObject; ppst_Object++)
			{
				/* If the Overlapping object has no ColMap, we continue; */
				if
				(
					(!OBJ_b_TestIdentityFlag(*ppst_Object, OBJ_C_IdentityFlag_ColMap))
				||	!(OBJ_b_TestStatusFlag(*ppst_Object, OBJ_C_StatusFlag_Active))
				||  (!OBJ_b_TestFlag(*ppst_Object, pst_ColBase->pst_Instance->ul_Filter_On, pst_ColBase->pst_Instance->ul_Filter_Off, pst_ColBase->pst_Instance->ul_Filter_Type))
				)
				{
					continue;
				}
				else
				{
					if
					(
						((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
					&&	((*ppst_Object)->pst_Base->pst_Hierarchy->pst_Father == pst_GO)
					)
					{
						continue;
					}

					/* Uncollidable Objects */
					if(COL_b_UnCollidable(pst_GO, *ppst_Object))
						continue;


#ifdef ODE_INSIDE
				if
				(
					(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
				&&	((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
				&&	(pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
				&&	((*ppst_Object)->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
				&&  
					(
						!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
					||  !((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
					)

				)
				continue;
#endif

					COL_InitGlobalVars(pst_GO, *ppst_Object);
					COL_Actor_GameObject(&COL_gst_GlobalVars);
				}
			}
		}

		COL_ComputeReports(&COL_gst_GlobalVars, _ul_FirstReport);
	}

	/*
	 * Aaarrgghhhhh ... didnt managed to find a safe position for GameObject. Replace
	 * it to where it was at the previous frame ... ARrghhh .. Horrible ...
	 */
	if(i_Loop == i_MaxLoop)
	{
		/*~~*/
		int i;
		/*~~*/

		for(i = 0; i < COL_gst_GlobalVars.uc_RecomputeGO; i++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Delta;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(COL_gst_GlobalVars.apst_RecomputeGO[i]->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	st_Matrix;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_MulMatrixMatrix
				(
					&st_Matrix,
					((COL_tdst_Base *) COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
				);
				MATH_SubVector(&st_Delta, &st_Matrix.T, &COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_GlobalMatrix->T);
			}
			else
			{
				MATH_SubVector
				(
					&st_Delta,
					&((COL_tdst_Base *) COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Extended->pst_Col)
							->pst_Instance->pst_OldGlobalMatrix->T,
					&COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_GlobalMatrix->T
				);
			}

			MATH_AddEqualVector(&COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_GlobalMatrix->T, &st_Delta);

			ANI_UpdateFlashAfterMagicBoxTranslation
			(
				COL_gst_GlobalVars.apst_RecomputeGO[i],
				&st_Delta,
				ANI_C_UpdateAllAnims
			);

			COL_Instance_SetFlag
			(
				((COL_tdst_Base *) COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Extended->pst_Col)->pst_Instance,
				COL_Cul_CornerFound
			);

#ifdef ACTIVE_EDITORS
			if(0)
			{
				/*~~~~~~~~~~~~~~~~~*/
				char	asz_Log[200];
				/*~~~~~~~~~~~~~~~~~*/

				sprintf
				(
					asz_Log,
					"[Collision Warning] No Valid Solution found for %s. [AI Engine CALL]",
					COL_gst_GlobalVars.apst_RecomputeGO[i]->sz_Name
				);
				LINK_PrintStatusMsg(asz_Log);
			}

#endif
		}
	}

	COL_UpdateOldMatrix(pst_World);


	COL_gb_SpecialCamera = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_DynamicHierarchisation(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_Father)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_InvMatrix;
	MATH_tdst_Matrix	st_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OBJ_ChangeIdentityFlags
	(
		_pst_GO,
		OBJ_ul_FlagsIdentityGet(_pst_GO) | OBJ_C_IdentityFlag_Hierarchy,
		OBJ_ul_FlagsIdentityGet(_pst_GO)
	);

	MATH_InvertMatrix(&st_InvMatrix, _pst_Father->pst_GlobalMatrix);

	/* Computes Local Matrix. */
	MATH_MulMatrixMatrix(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, _pst_GO->pst_GlobalMatrix, &st_InvMatrix);

	/*
	 * Old Global Matrix, which in in Global Coordinate System need to be recomputed
	 * in Local of the Father.
	 */
	MATH_MulMatrixMatrix
	(
		&st_Temp,
		((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
		&st_InvMatrix
	);

	MATH_CopyMatrix(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix, &st_Temp);

	_pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit = NULL;
	_pst_GO->pst_Base->pst_Hierarchy->pst_Father = _pst_Father;

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix)) ANI_ApplyHierarchyOnFlash(_pst_GO);
}

#if defined JADEFUSION
extern char		MEM_b_CheckBlockHasBeenFreed(void *);
#else
#if !defined (MEM_OPT)
extern char		MEM_b_CheckBlockHasBeenFreed(void *);
#endif // MEM_OPT
#endif //JADEFUSION
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL COL_b_MoveReports(OBJ_tdst_GameObject *_pst_GO, ULONG *_pul_Rank)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report *pst_CurrentReport, *pst_LastReport;
	COL_tdst_Report st_TempReport;
	ULONG			i;
	ULONG			ul_OldRank;
	BOOL			b_Return;
	BOOL			b_Hierarchy;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Return = FALSE;
	b_Hierarchy = _pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy;

	ul_OldRank = *_pul_Rank;

	pst_CurrentReport = COL_gst_GlobalVars.pst_World->ast_Reports + *_pul_Rank;
	pst_LastReport = COL_gst_GlobalVars.pst_World->ast_Reports + COL_gst_GlobalVars.ul_ReportIndex;
	for(i = *_pul_Rank; pst_CurrentReport < pst_LastReport; pst_CurrentReport++, i++)
	{
		if(pst_CurrentReport->pst_A == _pst_GO)
		{
			if(pst_CurrentReport->pst_B && MEM_b_CheckBlockHasBeenFreed(pst_CurrentReport->pst_B))
			{
#ifdef ACTIVE_EDITORS
				LINK_PrintStatusMsg("C	ollision Otpimisation with freed GO");
#endif
				continue;
			}

			if(!(pst_CurrentReport->ul_Flag & COL_Cul_Inactive) && !(pst_CurrentReport->ul_Flag & COL_Cul_Extra_ODE))
			{
				if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Corner)
				{
					COL_Instance_SetFlag
					(
						((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance,
						COL_Cul_CornerFound
					);
				}

				/*$F			---------------------------------------
								IF DISABLE COL OPTIM GROUND IS DETECTED
								---------------------------------------
				*/

				if
				(
					pst_CurrentReport->pst_B
				&&	OBJ_b_TestIdentityFlag(pst_CurrentReport->pst_B, OBJ_C_IdentityFlag_ColMap)
				&&	pst_CurrentReport->pst_B->pst_Extended
				&&	pst_CurrentReport->pst_B->pst_Extended->pst_Col
				&&	((COL_tdst_Base *) pst_CurrentReport->pst_B->pst_Extended->pst_Col)->pst_ColMap
//				&&	(((COL_tdst_Base *) pst_CurrentReport->pst_B->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Type == COL_C_Zone_Triangles)
				&&	(((COL_tdst_Base *) pst_CurrentReport->pst_B->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0]->uc_Flag & COL_C_Cob_DisableColOptim)
				)
				{
					*_pul_Rank = ul_OldRank;
					return FALSE;
				}

				/*$F			---------------------------------------
								IF WE FOUND SLIPPERY+GROUND, NO OPTIM
								---------------------------------------
				*/

				if
				(
					(pst_CurrentReport->ul_Flag & COL_Cul_SlipperyEdge)
				&&	(pst_CurrentReport->ul_Flag & COL_Cul_Ground)
				)
				{
					*_pul_Rank = ul_OldRank;
					return FALSE;
				}


				/*$F			---------------------
								SPECIAL AND RARE CASE
								---------------------
					The GameObject is on another one which suddently becomes Dynamic !! 
				*/
				if
				(
					!b_Hierarchy
				&&	(pst_CurrentReport->ul_Flag & COL_Cul_Ground)
				&&	(pst_CurrentReport->pst_B)
				&&	(pst_CurrentReport->pst_B->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
				&&	!(((DYN_tdst_Dyna *) pst_CurrentReport->pst_B->pst_Base->pst_Dyna)->ul_DynFlags & DYN_C_NeverDynamicFather)
				)
				{
					if
					(
						!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) 
					|| !(((DYN_tdst_Dyna *) _pst_GO->pst_Base->pst_Dyna)->ul_DynFlags & DYN_C_NeverDynamicHierarchy)
					)
					{
						COL_DynamicHierarchisation(_pst_GO, pst_CurrentReport->pst_B);
						b_Hierarchy = TRUE;
					}
				}

				/*$F			------------------
								DYNAMIC VS DYNAMIC
								------------------
					If a moving GameObject is in the report list of the current GO. We must be sure that it hasnt moved
					b4 inserting it into this frame reports.
				*/
				if					
				(
					pst_CurrentReport->pst_B
				&&  pst_CurrentReport->pst_B->pst_Extended
				&&  pst_CurrentReport->pst_B->pst_Extended->pst_Col
				&&  ((COL_tdst_Base *)pst_CurrentReport->pst_B->pst_Extended->pst_Col)->pst_Instance
				&&  (OBJ_pst_GetFather(pst_CurrentReport->pst_A) != pst_CurrentReport->pst_B)	/* If B is A's Father. We must not remove the report */
				)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	st_Move;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_SubVector(&st_Move, &(((COL_tdst_Base *)pst_CurrentReport->pst_B->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix->T), &pst_CurrentReport->pst_B->pst_GlobalMatrix->T);

					/* If the GO has moved, we dont insert it into the current GameObject list of report */
					if( !fEqWithEpsilon(st_Move.x, 0.0f, 1E-5f) || !fEqWithEpsilon(st_Move.y, 0.0f, 1E-5f))
					{
						continue;
					}
				}


				/*$F			----------------------
								COPY LAST FRAME REPORT
								----------------------
				*/

				if(i > *_pul_Rank)
				{
					L_memcpy
					(
						&st_TempReport,
						&COL_gst_GlobalVars.pst_World->ast_Reports[*_pul_Rank],
						sizeof(COL_tdst_Report)
					);
					L_memcpy
					(
						&COL_gst_GlobalVars.pst_World->ast_Reports[*_pul_Rank],
						pst_CurrentReport,
						sizeof(COL_tdst_Report)
					);
					L_memcpy(pst_CurrentReport, &st_TempReport, sizeof(COL_tdst_Report));
				}
				(*_pul_Rank)++;
				b_Return = TRUE;
			}
		}
	}

	return b_Return;
}

/*
 =======================================================================================================================
    Aim:    Compute Move between last frame and current one.
 =======================================================================================================================
 */
void COL_ComputeMove(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Delta)
{
	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && _pst_GO->pst_Base && _pst_GO->pst_Base->pst_Hierarchy && _pst_GO->pst_Base->pst_Hierarchy->pst_Father)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	st_Matrix;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_MulMatrixMatrix
		(
			&st_Matrix,
			((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
			_pst_GO->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
		);
		MATH_SubVector(_pst_Delta, &st_Matrix.T, &_pst_GO->pst_GlobalMatrix->T);
	}
	else
	{
		MATH_SubVector
		(
			_pst_Delta,
			&((((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col))->pst_Instance->pst_OldGlobalMatrix->T),
			&_pst_GO->pst_GlobalMatrix->T
		);
	}
}

#define COL_Cul_HasActiveZDM	0x1
#define COL_Cul_GravityOnly		0x2
#define COL_Cul_NoMove			0x4

static MATH_tdst_Matrix	st_SaveGlobal;

/*
 =======================================================================================================================
    Aim:    Compute all the collision using the intersection functions..
 =======================================================================================================================
 */
void COL_Compute(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	COL_tdst_Base		*pst_ColBase;
	OBJ_tdst_GameObject **ppst_Object, **ppst_LastObject;
	ULONG				ul_ReportIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* First, we set the number of collisions to 0. */
	COL_gst_GlobalVars.uc_RecomputeGO = 0;
	COL_gst_GlobalVars.ul_FirstRecomputingReport = 0;
	COL_gst_GlobalVars.b_Recomputing = FALSE;

	/* Set the World and resets Collision reports */
	COL_gst_GlobalVars.pst_World = _pst_World;

	ul_ReportIndex = 0;

	/*$F
		-----------------------------------------------------------------------------------------------
										[First Pass] 

								- Objects that didnt move. 
								- Objects that has only dropped with gravity.
		-----------------------------------------------------------------------------------------------
	 */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ZDM);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ZDM);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Delta;
		BOOL				b_ActiveZDM;
		UCHAR				i;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if
		(
			(TAB_b_IsAHole(pst_GO))
		||	(!OBJ_b_TestStatusFlag(pst_GO, OBJ_C_StatusFlag_Active))
		||	(OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_ForceInactive))
		||	(!OBJ_b_TestStatusFlag(pst_GO, OBJ_C_StatusFlag_Detection))
		) continue;

		/*
		 * If the GameObject has the DYN_C_AfterEngineCall flag. that means that we will
		 * compute its collisions later. (AfterRec Camera for Example)
		 */
		if
		(
			(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
		&&	(pst_GO->pst_Base->pst_Dyna->ul_DynFlags & DYN_C_AfterEngineCall)
		) continue;

#ifdef ACTIVE_EDITORS
		ERR_gpst_ContextGAO = pst_GO;
#endif


		pst_ColBase = (COL_tdst_Base *) pst_GO->pst_Extended->pst_Col;

		/* If There is no overlapping object given by the SnP, we continue. */
		if(!pst_ColBase->pst_List->ul_NbCollidedObjects) continue;

		/* Resets Instance Flag for this GO. */
		((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags = 0;


		/* If the Actor has no Active ZDM, we continue; */
		b_ActiveZDM = FALSE;
		for(i = 0; i < pst_ColBase->pst_Instance->uc_NbOfZDx; i++)
		{
			if
			(
				COL_b_Zone_TestFlag(pst_ColBase->pst_Instance->dpst_ZDx[i], COL_C_Zone_ZDM)
			&&	COL_b_Instance_IsActive(pst_ColBase->pst_Instance, i)
			)
			{
				b_ActiveZDM = TRUE;

				/* Flag to indicate this GO has Active ZDM (for next Loop ONLY) */
				((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags = COL_Cul_HasActiveZDM;
				break;
			}
		}

		if(!b_ActiveZDM) continue;

		COL_ComputeMove(pst_GO, &st_Delta);

		/* If the GO has not moved, we do nothing */
		if
		(
			(
				((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) && !(pst_GO->pst_Base->pst_Dyna->ul_DynFlags & DYN_C_OptimizeColDisable)) 
			|| !((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna))
			)
		&&	(fAbs(st_Delta.x) < (float)1E-5)
		&&	(fAbs(st_Delta.y) < (float)1E-5)
		&&	(fAbs(st_Delta.z) < (float)1E-5)
		)
		{
			((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags |= COL_Cul_NoMove;
			continue;
		}

		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_A_GCS_Speed, st_Delta, *pst_Pos;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			DYN_GetSpeedVector(OBJ_pst_GetDynaStruct(pst_GO), &st_A_GCS_Speed);
			pst_Pos = &pst_GO->pst_GlobalMatrix->T;

			COL_ComputeMove(pst_GO, &st_Delta);

			if
			(
				!(pst_GO->pst_Base->pst_Dyna->ul_DynFlags & DYN_C_OptimizeColDisable)
			&&	(pst_GO->pst_Base->pst_Dyna->ul_DynFlags & DYN_C_OptimizeColEnable)
			&&	(fAbs(st_A_GCS_Speed.x) < 1E-5f)
			&&	(fAbs(st_A_GCS_Speed.y) < 1E-5f)
			&&	(st_A_GCS_Speed.z < 0.0f)
			&&  (
					!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims) 
				||  !(pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]) 
				|| !(pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->uw_Flag & ANI_C_AnimFlag_Developped) 
				)
			)
			{
				if(COL_b_MoveReports(pst_GO, &ul_ReportIndex))
					((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags |= COL_Cul_OnlyGravity;
				else
					((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags &= ~COL_Cul_OnlyGravity;
			}
		}
	}

	COL_gst_GlobalVars.ul_ReportIndex = ul_ReportIndex;
	COL_gst_GlobalVars.pst_World->ul_NbReports = COL_gst_GlobalVars.ul_ReportIndex;

	/*$F
		-----------------------------------------------------------------------------------------------
										[Second Pass] 

								- Objects that have moved. 
		-----------------------------------------------------------------------------------------------
	 */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ZDM);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ZDM);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if
		(
			(TAB_b_IsAHole(pst_GO))
		||	(!OBJ_b_TestStatusFlag(pst_GO, OBJ_C_StatusFlag_Active))
		||	(OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_ForceInactive))
		||	(!OBJ_b_TestStatusFlag(pst_GO, OBJ_C_StatusFlag_Detection))
		) continue;

#ifdef ACTIVE_EDITORS
		ERR_gpst_ContextGAO = pst_GO;
#endif

		/*
		 * If the GameObject has the DYN_C_AfterEngineCall flag. that means that we will
		 * compute its collisions later. (AfterRec Camera for Example)
		 */
		if
		(
			(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
		&&	(pst_GO->pst_Base->pst_Dyna->ul_DynFlags & DYN_C_AfterEngineCall)
		) continue;

		pst_ColBase = (COL_tdst_Base *) pst_GO->pst_Extended->pst_Col;

		/* If There is no overlapping object given by the SnP, we continue. */
		if(!pst_ColBase->pst_List->ul_NbCollidedObjects) continue;

		/* If the Actor has no Active ZDM, we continue; */
		if(!(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags & COL_Cul_HasActiveZDM))
			continue;

#ifdef ACTIVE_EDITORS
		if
		(
			COL_s_GhostOptimisation
		&&	(
				(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags & (COL_Cul_OnlyGravity | COL_Cul_NoMove))
			)
#ifdef ODE_INSIDE
		&&  !(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GhostODE)
#endif
		) 
			pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_Ghost;
		else
			pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Ghost;

#endif

#ifdef ODE_INSIDE2
		if
		(
			(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) 
		&&	!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) 
		&&	pst_GO->pst_Base->pst_ODE->ode_id_body 
		&&  (
				!(pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_ENABLE)
			||	(!dBodyIsEnabled (pst_GO->pst_Base->pst_ODE->ode_id_body))
			)
		)
		continue;
#endif
		/* If the Actor dont move, we do nothing */
		if(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags & COL_Cul_NoMove) continue;

		/*
		 * If the Actor has only dropped with gravity, collision reports have already been
		 * created
		 */
		if(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags & COL_Cul_OnlyGravity) continue;

		/* Resets the Active ZDM Flag */
		((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags &= ~COL_Cul_HasActiveZDM;


		/* Max step value. Negative value to skip the test */
		if(pst_GO->pst_Base && pst_GO->pst_Base->pst_Dyna)
			COL_gst_GlobalVars.f_MaxStepSize = pst_GO->pst_Base->pst_Dyna->f_MaxStepSize;
		else
			COL_gst_GlobalVars.f_MaxStepSize = -1.0f;

		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy)) OBJ_ComputeGlobalWhenHie(pst_GO);

		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
		{
			pst_GO->pst_Base->pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColDisable;
			pst_GO->pst_Base->pst_Dyna->ul_DynFlags &= ~DYN_C_OptimizeColEnable;
		}

		/* We go thru al the overlapping objects ... */
		ppst_Object = pst_ColBase->pst_List->dpst_CollidedObject;
		ppst_LastObject = ppst_Object + pst_ColBase->pst_List->ul_NbCollidedObjects - 1;

		for(; ppst_Object <= ppst_LastObject; ppst_Object++)
		{
			/* If the Overlapping object has no ColMap or is Inactive, we continue; */
			if
			(
				(!OBJ_b_TestIdentityFlag(*ppst_Object, OBJ_C_IdentityFlag_ColMap))
			||	!(OBJ_b_TestStatusFlag(*ppst_Object, OBJ_C_StatusFlag_Active))
			||	(OBJ_b_TestControlFlag(*ppst_Object, OBJ_C_ControlFlag_ForceInactive))
			||  (!OBJ_b_TestFlag(*ppst_Object, pst_ColBase->pst_Instance->ul_Filter_On, pst_ColBase->pst_Instance->ul_Filter_Off, pst_ColBase->pst_Instance->ul_Filter_Type))
			) 
			{	
				continue;
			}
			else
			{
				if
				(
					((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
				&&	((*ppst_Object)->pst_Base->pst_Hierarchy->pst_Father == pst_GO)
				) continue;

				/* Uncollidable Objects */
				if(COL_b_UnCollidable(pst_GO, *ppst_Object))
					continue;
#ifdef ODE_INSIDE
				if
				(
					(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
				&&	((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
				&&	(pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
				&&	((*ppst_Object)->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
				&&  
					(
						!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
					||  !((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
					)
				)
				continue;
#endif

				COL_InitGlobalVars(pst_GO, *ppst_Object);
				COL_Actor_GameObject(&COL_gst_GlobalVars);
			}
		}
	}
}

ULONG						COL_ul_SnP_RefreshType = COL_SnP_RefreshXY;

/*$F
 ===================================================================================================
 Aim: Collision main function
 Note:	1) Update the SnP structure.
		2) Compute all the collisions.
		3) Deal with the reports and recal the object.
		4) Loop to 2 if recomputatin is needed.
		5) Update the Old frame matrix.
 ===================================================================================================
 */
#ifdef ACTIVE_EDITORS
extern L_jmp_buf			AI_gst_ContextOut;
extern GDI_tdst_DisplayData *GDI_gpst_CurDD;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_UpdateRealTimeColMaps(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	COL_tdst_ColMap		*pst_ColMap;
//	GEO_tdst_Object		*pst_Visual;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ColMap);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ColMap);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if
		(
			(TAB_b_IsAHole(pst_GO))
		||	(!OBJ_b_TestStatusFlag(pst_GO, OBJ_C_StatusFlag_Active))
		||	(OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_ForceInactive))
		) continue;


		pst_ColMap = ((COL_tdst_Base *)pst_GO->pst_Extended->pst_Col)->pst_ColMap;

		if(!pst_ColMap || !pst_ColMap->uc_NbOfCob || (pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles))
			continue;

		if(!(pst_ColMap->dpst_Cob[0]->uc_Flag & COL_C_RealTimeComputation))
			continue;

		if(pst_GO->pst_Base && pst_GO->pst_Base->pst_Visu && pst_GO->pst_Base->pst_Visu->pst_Object)
			COL_UpdateCobFromVisualRealTime(pst_GO, pst_ColMap->dpst_Cob[0], (GEO_tdst_Object *)pst_GO->pst_Base->pst_Visu->pst_Object);
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_MainCall(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int		i_MaxLoop, i_Loop;
#ifdef ACTIVE_EDITORS
	ULONG	ul_SnP_Ticks;
	ULONG	ul_Compute_Ticks;
	ULONG	ul_Reports_Ticks;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	COL_ul_ParsedFaces = 0;
	COL_ul_ParsedFacesOK3 = 0;
	COL_ul_ComputedFaces = 0;
	COL_ul_SnP_Permutations = 0;
	ERR_gpst_ContextGAO = NULL;
	ERR_gpsz_ContextString = "Collision";
#endif
	COL_gb_LastLoop = FALSE;
	COL_uc_RecomputingLoop = 0;
	COL_gb_AfterEngine = FALSE;

#ifdef ODE_INSIDE
	dJointGroupEmpty (_pst_World->ode_joint_col_jade);
	_pst_World->ode_contacts_num_jade = 0;
#endif

	COL_UpdateBV(&_pst_World->st_EOT.st_ZDM);

	/* SnP Refresh */
	if(ENG_gb_FirstFrame)
		INT_SnP_InsertionSort(_pst_World->pst_SnP, COL_SnP_RefreshXYZ);
	else
		INT_SnP_InsertionSort(_pst_World->pst_SnP, COL_SnP_RefreshXYZ);

	COL_ul_SnP_RefreshType = (COL_ul_SnP_RefreshType == COL_SnP_RefreshXY) ? COL_SnP_RefreshZ : COL_SnP_RefreshXY;

	COL_UpdateRealTimeColMaps(_pst_World);

#ifdef ACTIVE_EDITORS
	ul_SnP_Ticks = (TIM_ul_GetLowPartTimerInternalCounter() - ENG_gpst_RasterEng_COL.ul_StartValue);
	if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_cul_DF_DisplayOK3) COL_OK3_Display_Refresh(_pst_World);
#endif
	/* Real Collision computation */
	COL_Compute(_pst_World);



#ifdef ACTIVE_EDITORS
	ul_Compute_Ticks = (TIM_ul_GetLowPartTimerInternalCounter() - ENG_gpst_RasterEng_COL.ul_StartValue) - ul_SnP_Ticks;
#endif
	/* If there is at least one collision reported, we recal the concerning object(s) */
	if(COL_gst_GlobalVars.ul_ReportIndex) COL_ComputeReports(&COL_gst_GlobalVars, 0);

#ifdef ACTIVE_EDITORS
	ul_Reports_Ticks = (TIM_ul_GetLowPartTimerInternalCounter() - ENG_gpst_RasterEng_COL.ul_StartValue) -
		ul_SnP_Ticks -
		ul_Compute_Ticks;
#endif
	/* Recompute Collision for some GOs ? */
	i_MaxLoop = 6;
	i_Loop = 0;
	while(COL_gst_GlobalVars.uc_RecomputeGO && (i_Loop < i_MaxLoop))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject		*pst_GO;
		COL_tdst_DetectionList	*pst_List;
		OBJ_tdst_GameObject		**ppst_Object, **ppst_LastObject;
		int						i, uc_Loop;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		i_Loop++;

		COL_uc_RecomputingLoop = i_Loop;

		COL_gb_LastLoop = (i_Loop == i_MaxLoop);

		COL_gst_GlobalVars.b_Recomputing = TRUE;
		COL_gst_GlobalVars.ul_FirstRecomputingReport = COL_gst_GlobalVars.ul_ReportIndex;

		uc_Loop = COL_gst_GlobalVars.uc_RecomputeGO;
		COL_gst_GlobalVars.uc_RecomputeGO = 0;

		for(i = 0; i < uc_Loop; i++)
		{
			pst_GO = COL_gst_GlobalVars.apst_RecomputeGO[i];

#ifdef ACTIVE_EDITORS
			ERR_gpst_ContextGAO = pst_GO;
#endif

			/* Local Matrix has been updated by the previous Col loop ... Update the Global Matrix */
			if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
				OBJ_ComputeGlobalWhenHie(pst_GO);

			/* Resets some Instance Flags for this GO. */
			((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags &= ~COL_Cul_StaticGround;
			((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags &= ~COL_Cul_Recompute_Mask;

			/* Resets only gravity */
			((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->uw_Flags &= ~COL_Cul_OnlyGravity;

			if(pst_GO->pst_Base && pst_GO->pst_Base->pst_Dyna)
				COL_gst_GlobalVars.f_MaxStepSize = pst_GO->pst_Base->pst_Dyna->f_MaxStepSize;
			else
				COL_gst_GlobalVars.f_MaxStepSize = -1.0f;	/* Negative value to skip the test */

			pst_List = COL_pst_GetDetectionList(pst_GO, FALSE);

			/* Else, we go thru al the overlapping objects ... */
			ppst_Object = pst_List->dpst_CollidedObject;
			ppst_LastObject = ppst_Object + pst_List->ul_NbCollidedObjects - 1;

			for(; ppst_Object <= ppst_LastObject; ppst_Object++)
			{
				/* If the Overlapping object has no ColMap, we continue; */
				if
				(
					(!OBJ_b_TestIdentityFlag(*ppst_Object, OBJ_C_IdentityFlag_ColMap))
				||	!(OBJ_b_TestStatusFlag(*ppst_Object, OBJ_C_StatusFlag_Active))
				||  (!OBJ_b_TestFlag(*ppst_Object, ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->ul_Filter_On, ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->ul_Filter_Off, ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->ul_Filter_Type))
				) 
				{
					continue;
				}
				else
				{
					if
					(
						((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
					&&	((*ppst_Object)->pst_Base->pst_Hierarchy->pst_Father == pst_GO)
					) continue;

					/* Uncollidable Objects */
					if(COL_b_UnCollidable(pst_GO, *ppst_Object))
						continue;

#ifdef ODE_INSIDE
				if
				(
					(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
				&&	((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
				&&	(pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
				&&	((*ppst_Object)->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
				&&  
					(
						!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
					||  !((*ppst_Object)->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
					)

				)
				continue;
#endif

					COL_InitGlobalVars(pst_GO, *ppst_Object);
					COL_Actor_GameObject(&COL_gst_GlobalVars);
				}
			}
		}

		COL_ComputeReports(&COL_gst_GlobalVars, 0);
	}

	if(i_Loop == i_MaxLoop)
	{
		/*~~*/
		int i;
		/*~~*/

		for(i = 0; i < COL_gst_GlobalVars.uc_RecomputeGO; i++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Delta;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if((COL_gst_GlobalVars.apst_RecomputeGO[i]->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Base && COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Base->pst_Hierarchy && COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Base->pst_Hierarchy->pst_Father)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	st_Matrix;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_MulMatrixMatrix
				(
					&st_Matrix,
					((COL_tdst_Base *) COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
				);
				MATH_SubVector(&st_Delta, &st_Matrix.T, &COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_GlobalMatrix->T);
			}
			else
			{
				MATH_SubVector
				(
					&st_Delta,
					&((COL_tdst_Base *) COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Extended->pst_Col)
							->pst_Instance->pst_OldGlobalMatrix->T,
					&COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_GlobalMatrix->T
				);
			}

			MATH_AddEqualVector(&COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_GlobalMatrix->T, &st_Delta);

			if(COL_gst_GlobalVars.apst_RecomputeGO[i]->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				OBJ_ComputeLocalWhenHie(COL_gst_GlobalVars.apst_RecomputeGO[i]);
			}

			ANI_UpdateFlashAfterMagicBoxTranslation
			(
				COL_gst_GlobalVars.apst_RecomputeGO[i],
				&st_Delta,
				ANI_C_UpdateAllAnims
			);

			COL_Instance_SetFlag
			(
				((COL_tdst_Base *) COL_gst_GlobalVars.apst_RecomputeGO[i]->pst_Extended->pst_Col)->pst_Instance,
				COL_Cul_CornerFound
			);

#ifdef ACTIVE_EDITORS
			if(0)
			{
				/*~~~~~~~~~~~~~~~~~*/
				char	asz_Log[200];
				/*~~~~~~~~~~~~~~~~~*/

				sprintf
				(
					asz_Log,
					"[Collision Warning] No Valid Solution found for %s.",
					COL_gst_GlobalVars.apst_RecomputeGO[i]->sz_Name
				);
				LINK_PrintStatusMsg(asz_Log);

				/* Stop the Engine if case arrises. L_longjmp(AI_gst_ContextOut, 1); */
			}

#endif
		}
	}

	/* We update all the ZDM EOT objects old frame matrix. */
	COL_UpdateOldMatrix(_pst_World);


}

/*
 =======================================================================================================================
    Aim:    Transforms a SPHERICAL ZDM in another Coordinate System.

    Note:   The _ul_Data1 and _ul_Data2 are the center and the radius of a Sphere.
 =======================================================================================================================
 */
void COL_Zone_Transform(COL_tdst_ZDx *_pst_ZDx, OBJ_tdst_GameObject *_pst_GO, ULONG *_ul_Data1, ULONG *_ul_Data2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Scale, b_Oriented;
	MATH_tdst_Matrix	*pst_Matrix;
	MATH_tdst_Vector	*pst_Position;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Matrix = NULL;
	pst_Position = NULL;

	pst_Matrix = OBJ_pst_GetAbsoluteMatrix(_pst_GO);
	b_Scale = MATH_b_TestScaleType(pst_Matrix);
	b_Oriented = TRUE;

	if(b_Oriented)
	{
		switch(_pst_ZDx->uc_Type)
		{
		case COL_C_Zone_Sphere:
			if((b_Scale) && !(COL_b_Zone_TestFlag(_pst_ZDx, COL_C_Zone_NoScale)))
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	st_Scale;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_GetScale(&st_Scale, pst_Matrix);
				MATH_TransformVertex
				(
					(MATH_tdst_Vector *) _ul_Data1,
					pst_Matrix,
					COL_pst_Shape_GetCenter(_pst_ZDx->p_Shape)
				);
				*(float *) _ul_Data2 = COL_f_Shape_GetRadius(_pst_ZDx->p_Shape) * fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
			}
			else
			{
				MATH_TransformVertexNoScale
				(
					(MATH_tdst_Vector *) _ul_Data1,
					pst_Matrix,
					COL_pst_Shape_GetCenter(_pst_ZDx->p_Shape)
				);
				*(float *) _ul_Data2 = COL_f_Shape_GetRadius(_pst_ZDx->p_Shape);
			}
			break;

		case COL_C_Zone_Box:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	st_BoxCenter, st_HalfDiag, st_GCS_HalfDiag, st_GCS_BoxCenter;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				/* First, we compute the Center of the Box */
				MATH_AddVector
				(
					&st_BoxCenter,
					COL_pst_Shape_GetMin(_pst_ZDx->p_Shape),
					COL_pst_Shape_GetMax(_pst_ZDx->p_Shape)
				);
				MATH_MulEqualVector(&st_BoxCenter, 0.5f);

				/* Then, the Half Diagonal of the Box */
				MATH_SubVector(&st_HalfDiag, COL_pst_Shape_GetMax(_pst_ZDx->p_Shape), &st_BoxCenter);

				if((b_Scale) && !(COL_b_Zone_TestFlag(_pst_ZDx, COL_C_Zone_NoScale)))
				{
					MATH_TransformVertex(&st_GCS_BoxCenter, pst_Matrix, &st_BoxCenter);
					MATH_TransformVector(&st_GCS_HalfDiag, pst_Matrix, &st_HalfDiag);
				}
				else
				{
					MATH_TransformVertexNoScale(&st_GCS_BoxCenter, pst_Matrix, &st_BoxCenter);
					MATH_TransformVectorNoScale(&st_GCS_HalfDiag, pst_Matrix, &st_HalfDiag);
				}
				((MATH_tdst_Vector *) _ul_Data1)->x = st_GCS_BoxCenter.x - fAbs(st_GCS_HalfDiag.x);
				((MATH_tdst_Vector *) _ul_Data1)->y = st_GCS_BoxCenter.y - fAbs(st_GCS_HalfDiag.y);
				((MATH_tdst_Vector *) _ul_Data1)->z = st_GCS_BoxCenter.z - fAbs(st_GCS_HalfDiag.z);
				((MATH_tdst_Vector *) _ul_Data2)->x = st_GCS_BoxCenter.x + fAbs(st_GCS_HalfDiag.x);
				((MATH_tdst_Vector *) _ul_Data2)->y = st_GCS_BoxCenter.y + fAbs(st_GCS_HalfDiag.y);
				((MATH_tdst_Vector *) _ul_Data2)->z = st_GCS_BoxCenter.z + fAbs(st_GCS_HalfDiag.z);
			}
			break;
		}
	}
	else
	{
		switch(_pst_ZDx->uc_Type)
		{
		case COL_C_Zone_Sphere:
			MATH_AddVector((MATH_tdst_Vector *) _ul_Data1, COL_pst_Shape_GetCenter(_pst_ZDx->p_Shape), pst_Position);
			*(float *) _ul_Data2 = COL_f_Shape_GetRadius(_pst_ZDx->p_Shape);
			break;

		case COL_C_Zone_Box:
			MATH_AddVector((MATH_tdst_Vector *) _ul_Data1, COL_pst_Shape_GetMin(_pst_ZDx->p_Shape), pst_Position);
			MATH_AddVector((MATH_tdst_Vector *) _ul_Data2, COL_pst_Shape_GetMax(_pst_ZDx->p_Shape), pst_Position);
			break;
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Detects whether a ZDE collides with an Object BV Center.

    Note:   The _b_Inverse BOOL is set when we want to test the A BV Center with the B ZDE.
 =======================================================================================================================
 */
BOOL COL_ZDE_BVCenterCollide
(
	OBJ_tdst_GameObject *_pst_A_GO,
	UCHAR				_uc_AI_Index,
	OBJ_tdst_GameObject *_pst_B_GO,
	BOOL				_b_Inverse
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A_GO, *pst_B_GO;
	COL_tdst_ZDx		*pst_A_ZDx;
	MATH_tdst_Vector	st_B_GCS_Center, st_A_GCS_Center;
	float				f_A_GCS_Radius;
	COL_tdst_Instance	*pst_A_Instance;
	COL_tdst_ColSet		*pst_A_ColSet;
	UCHAR				uc_ENG_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_A_GO || !_pst_B_GO) return FALSE;

	/* If one of the 2 objects is inactive, return FALSE; */
	if
	(
		!OBJ_b_TestStatusFlag(_pst_A_GO, OBJ_C_StatusFlag_Active)
	||	!OBJ_b_TestStatusFlag(_pst_B_GO, OBJ_C_StatusFlag_Active)
	) return FALSE;

	if(_b_Inverse)
	{
		pst_A_GO = _pst_B_GO;
		pst_B_GO = _pst_A_GO;
	}
	else
	{
		pst_A_GO = _pst_A_GO;
		pst_B_GO = _pst_B_GO;
	}

	/* If A has no instance, we return FALSE; */
	if
	(
		(!OBJ_b_TestIdentityFlag(pst_A_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	||	!pst_A_GO->pst_Extended
	||	!pst_A_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance
	) return 0;

	pst_A_Instance = ((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_A_ColSet = pst_A_Instance->pst_ColSet;

	if(!pst_A_ColSet) return FALSE;

	uc_ENG_Index = pst_A_ColSet->pauc_AI_Indexes[_uc_AI_Index];

	if(uc_ENG_Index >= pst_A_Instance->uc_NbOfZDx) return FALSE;

	if(!COL_b_Instance_IsActive(pst_A_Instance, uc_ENG_Index)) return FALSE;

	pst_A_ZDx = *(((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance->dpst_ZDx + uc_ENG_Index);

	/* If the ZDx in not a ZDE, we return FALSE; */
	if(!COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_ZDE)) return FALSE;

	switch(pst_A_ZDx->uc_Type)
	{
	case COL_C_Zone_Sphere:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_PivotToCenter;
			float				f_Radius;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			f_Radius = COL_f_Shape_GetRadius(pst_A_ZDx->p_Shape);
			if(!f_Radius) return FALSE;
			OBJ_BV_ComputeCenter(pst_B_GO, &st_B_GCS_Center);

			/* We compute the Center and the Radius of the ZDx in the GCS. */
			if
			(
				(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(pst_A_GO)))
			&&	(!(COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_NoScale)))
			)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	st_A_Scale;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_GetScale(&st_A_Scale, OBJ_pst_GetAbsoluteMatrix(pst_A_GO));

				MATH_TransformVertex
				(
					&st_A_GCS_Center,
					OBJ_pst_GetAbsoluteMatrix(pst_A_GO),
					COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape)
				);
				f_A_GCS_Radius = COL_f_Shape_GetRadius(pst_A_ZDx->p_Shape) * fMax3(st_A_Scale.x, st_A_Scale.y, st_A_Scale.z);
			}
			else
			{
				MATH_TransformVertexNoScale
				(
					&st_A_GCS_Center,
					OBJ_pst_GetAbsoluteMatrix(pst_A_GO),
					COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape)
				);
				f_A_GCS_Radius = COL_f_Shape_GetRadius(pst_A_ZDx->p_Shape);
			}

			MATH_SubVector(&st_PivotToCenter, &st_B_GCS_Center, &st_A_GCS_Center);
			return(MATH_f_SqrNormVector(&st_PivotToCenter) < fSqr(f_A_GCS_Radius));
		}
		break;

	case COL_C_Zone_Box:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector				*pst_Min, *pst_Max;
			MATH_tdst_Vector				st_B_ACS_Center;
			MATH_tdst_Matrix st_InvA_Matrix ONLY_PSX2_ALIGNED(16);
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Min = COL_pst_Shape_GetMin(pst_A_ZDx->p_Shape);
			pst_Max = COL_pst_Shape_GetMax(pst_A_ZDx->p_Shape);

			/*
			 * The size of the box can be set by the JoyStick Move. If we dont move, the size
			 * is NULL. We can return a FALSE.
			 */
			if(((pst_Min->x) == (pst_Max->x)) || ((pst_Min->y) == (pst_Max->y)) || ((pst_Min->z) == (pst_Max->z)))
				return FALSE;

			OBJ_BV_ComputeCenter(pst_B_GO, &st_B_GCS_Center);

			if
			(
				(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(pst_A_GO)))
			&&	(!(COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_NoScale)))
			)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	st_A_NoScaleMatrix;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_CopyMatrix(&st_A_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(pst_A_GO));
				MATH_ClearScale(&st_A_NoScaleMatrix, 1);
				MATH_InvertMatrix(&st_InvA_Matrix, &st_A_NoScaleMatrix);
			}
			else
				MATH_InvertMatrix(&st_InvA_Matrix, OBJ_pst_GetAbsoluteMatrix(pst_A_GO));
			MATH_TransformVertex(&st_B_ACS_Center, &st_InvA_Matrix, &st_B_GCS_Center);

			return
				(
					(st_B_ACS_Center.x > pst_Min->x)
				&&	(st_B_ACS_Center.x < pst_Max->x)
				&&	(st_B_ACS_Center.y > pst_Min->y)
				&&	(st_B_ACS_Center.y < pst_Max->y)
				&&	(st_B_ACS_Center.z > pst_Min->z)
				&&	(st_B_ACS_Center.z < pst_Max->z)
				);
		}
		break;
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Detects whether a ZDE collides with an object's pivot.

    Note:   The _b_Inverse BOOL is set when we want to test the A Pivot with the B ZDE.
 =======================================================================================================================
 */
BOOL COL_ZDE_PivotCollide
(
	OBJ_tdst_GameObject *_pst_A_GO,
	UCHAR				_uc_AI_Index,
	OBJ_tdst_GameObject *_pst_B_GO,
	BOOL				_b_Inverse
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A_GO, *pst_B_GO;
	COL_tdst_ZDx		*pst_A_ZDx;
	MATH_tdst_Vector	st_B_GCS_Pivot, st_A_GCS_Center;
	float				f_A_GCS_Radius;
	MATH_tdst_Matrix	*pst_B_Matrix;
	COL_tdst_Instance	*pst_A_Instance;
	COL_tdst_ColSet		*pst_A_ColSet;
	UCHAR				uc_ENG_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_A_GO || !_pst_B_GO) return FALSE;

	/* If one of the 2 objects is inactive, return FALSE; */
	if
	(
		!OBJ_b_TestStatusFlag(_pst_A_GO, OBJ_C_StatusFlag_Active)
	||	!OBJ_b_TestStatusFlag(_pst_B_GO, OBJ_C_StatusFlag_Active)
	) return FALSE;

	if(_b_Inverse)
	{
		pst_A_GO = _pst_B_GO;
		pst_B_GO = _pst_A_GO;
	}
	else
	{
		pst_A_GO = _pst_A_GO;
		pst_B_GO = _pst_B_GO;
	}

	/* If A has no instance, we return FALSE; */
	if
	(
		(!OBJ_b_TestIdentityFlag(pst_A_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	||	!pst_A_GO->pst_Extended
	||	!pst_A_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance
	) return 0;

	pst_A_Instance = ((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_A_ColSet = pst_A_Instance->pst_ColSet;

	if(!pst_A_ColSet) return FALSE;

	uc_ENG_Index = pst_A_ColSet->pauc_AI_Indexes[_uc_AI_Index];

	if(uc_ENG_Index >= pst_A_Instance->uc_NbOfZDx) return FALSE;

	if(!COL_b_Instance_IsActive(pst_A_Instance, uc_ENG_Index)) return FALSE;

	pst_A_ZDx = *(((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance->dpst_ZDx + uc_ENG_Index);

	/* If the ZDx in not a ZDE, we return FALSE; */
	if(!COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_ZDE)) return FALSE;

	switch(pst_A_ZDx->uc_Type)
	{
	case COL_C_Zone_Sphere:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_PivotToCenter;
			float				f_Radius;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			f_Radius = COL_f_Shape_GetRadius(pst_A_ZDx->p_Shape);

			if(!f_Radius) return FALSE;
			pst_B_Matrix = OBJ_pst_GetAbsoluteMatrix(pst_B_GO);
			MATH_CopyVector(&st_B_GCS_Pivot, MATH_pst_GetTranslation(pst_B_Matrix));

			/* We compute the Center and the Radius of the ZDx in the GCS. */
			if
			(
				(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(pst_A_GO)))
			&&	(!(COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_NoScale)))
			)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	st_A_Scale;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_GetScale(&st_A_Scale, OBJ_pst_GetAbsoluteMatrix(pst_A_GO));

				MATH_TransformVertex
				(
					&st_A_GCS_Center,
					OBJ_pst_GetAbsoluteMatrix(pst_A_GO),
					COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape)
				);
				f_A_GCS_Radius = COL_f_Shape_GetRadius(pst_A_ZDx->p_Shape) * fMax3(st_A_Scale.x, st_A_Scale.y, st_A_Scale.z);
			}
			else
			{
				MATH_TransformVertexNoScale
				(
					&st_A_GCS_Center,
					OBJ_pst_GetAbsoluteMatrix(pst_A_GO),
					COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape)
				);
				f_A_GCS_Radius = COL_f_Shape_GetRadius(pst_A_ZDx->p_Shape);
			}

			MATH_SubVector(&st_PivotToCenter, &st_B_GCS_Pivot, &st_A_GCS_Center);
			return(MATH_f_SqrNormVector(&st_PivotToCenter) < fSqr(f_A_GCS_Radius));
		}
		break;

	case COL_C_Zone_Box:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector				*pst_Min, *pst_Max;
			MATH_tdst_Vector				st_B_ACS_Pivot;
			MATH_tdst_Matrix st_InvA_Matrix ONLY_PSX2_ALIGNED(16);
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Min = COL_pst_Shape_GetMin(pst_A_ZDx->p_Shape);
			pst_Max = COL_pst_Shape_GetMax(pst_A_ZDx->p_Shape);

			/*
			 * The size of the box can be set by the JoyStick Move. If we dont move, the size
			 * is NULL. We can return a FALSE.
			 */
			if(((pst_Min->x) == (pst_Max->x)) || ((pst_Min->y) == (pst_Max->y)) || ((pst_Min->z) == (pst_Max->z)))
				return FALSE;

			pst_B_Matrix = OBJ_pst_GetAbsoluteMatrix(pst_B_GO);
			MATH_CopyVector(&st_B_GCS_Pivot, MATH_pst_GetTranslation(pst_B_Matrix));

			if
			(
				(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(pst_A_GO)))
			&&	(COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_NoScale))
			)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	st_A_NoScaleMatrix;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				MATH_CopyMatrix(&st_A_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(pst_A_GO));
				MATH_ClearScale(&st_A_NoScaleMatrix, 1);
				MATH_InvertMatrix(&st_InvA_Matrix, &st_A_NoScaleMatrix);
			}
			else
				MATH_InvertMatrix(&st_InvA_Matrix, OBJ_pst_GetAbsoluteMatrix(pst_A_GO));
			MATH_TransformVertex(&st_B_ACS_Pivot, &st_InvA_Matrix, &st_B_GCS_Pivot);

			return
				(
					(st_B_ACS_Pivot.x > pst_Min->x)
				&&	(st_B_ACS_Pivot.x < pst_Max->x)
				&&	(st_B_ACS_Pivot.y > pst_Min->y)
				&&	(st_B_ACS_Pivot.y < pst_Max->y)
				&&	(st_B_ACS_Pivot.z > pst_Min->z)
				&&	(st_B_ACS_Pivot.z < pst_Max->z)
				);
		}
		break;
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Detects whether a ZDE collides with an object's pivot.

    Note:   The _b_Inverse BOOL is set when we want to test the A Pivot with the B ZDE.
 =======================================================================================================================
 */
BOOL COL_BV_PivotCollide(OBJ_tdst_GameObject *_pst_A_GO, OBJ_tdst_GameObject *_pst_B_GO, BOOL _b_Inverse)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A_GO, *pst_B_GO;
	MATH_tdst_Matrix	*pst_B_Matrix;
	MATH_tdst_Vector	st_B_GCS_Pivot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_A_GO || !_pst_B_GO) return FALSE;

	/* If one of the 2 objects is inactive, return FALSE; */
	if
	(
		!OBJ_b_TestStatusFlag(_pst_A_GO, OBJ_C_StatusFlag_Active)
	||	!OBJ_b_TestStatusFlag(_pst_B_GO, OBJ_C_StatusFlag_Active)
	) return FALSE;
	
#if defined(_DEBUG) && defined(_GAMECUBE)
		if(
			((*(int *) &_pst_A_GO->pst_GlobalMatrix->T.x) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_A_GO->pst_GlobalMatrix->T.y) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_A_GO->pst_GlobalMatrix->T.z) == (int) 0xFFC00000)
		||	((*(int *) &_pst_A_GO->pst_GlobalMatrix->T.x) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_A_GO->pst_GlobalMatrix->T.y) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_A_GO->pst_GlobalMatrix->T.z) == (int) 0x7FC00000)
		
		)
		OSReport("BV error \n");
		
		if(
			((*(int *) &_pst_B_GO->pst_GlobalMatrix->T.x) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_B_GO->pst_GlobalMatrix->T.y) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_B_GO->pst_GlobalMatrix->T.z) == (int) 0xFFC00000)
		||	((*(int *) &_pst_B_GO->pst_GlobalMatrix->T.x) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_B_GO->pst_GlobalMatrix->T.y) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_B_GO->pst_GlobalMatrix->T.z) == (int) 0x7FC00000)
		
		)
		OSReport("BV error \n");
		
#endif			
	

	if(_b_Inverse)
	{
		pst_A_GO = _pst_B_GO;
		pst_B_GO = _pst_A_GO;
	}
	else
	{
		pst_A_GO = _pst_A_GO;
		pst_B_GO = _pst_B_GO;
	}

	if(OBJ_BV_IsSphere(pst_A_GO->pst_BV))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_PivotToCenter;
		float				f_Radius;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		f_Radius = OBJ_f_BV_GetRadius(pst_A_GO->pst_BV);
		if(!f_Radius) return FALSE;

		pst_B_Matrix = OBJ_pst_GetAbsoluteMatrix(pst_B_GO);
		MATH_CopyVector(&st_B_GCS_Pivot, MATH_pst_GetTranslation(pst_B_Matrix));
		MATH_SubVector(&st_PivotToCenter, &st_B_GCS_Pivot, OBJ_pst_BV_GetCenter(pst_A_GO->pst_BV));
		return(MATH_f_SqrNormVector(&st_PivotToCenter) < fSqr(f_Radius));
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Min, st_Max;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_A_GO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_A_GO));
		MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_A_GO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_A_GO));

		if(((st_Min.x) == (st_Max.x)) || ((st_Min.y) == (st_Max.y)) || ((st_Min.z) == (st_Max.z))) return FALSE;

		pst_B_Matrix = OBJ_pst_GetAbsoluteMatrix(pst_B_GO);
		MATH_CopyVector(&st_B_GCS_Pivot, MATH_pst_GetTranslation(pst_B_Matrix));

		return
			(
				(st_B_GCS_Pivot.x > st_Min.x)
			&&	(st_B_GCS_Pivot.x < st_Max.x)
			&&	(st_B_GCS_Pivot.y > st_Min.y)
			&&	(st_B_GCS_Pivot.y < st_Max.y)
			&&	(st_B_GCS_Pivot.z > st_Min.z)
			&&	(st_B_GCS_Pivot.z < st_Max.z)
			);
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Detects whether a ZDE collides with an object's BV.
 =======================================================================================================================
 */
BOOL COL_ZDE_BVCollide
(
	OBJ_tdst_GameObject *_pst_A_GO,
	UCHAR				_uc_AI_Index,
	OBJ_tdst_GameObject *_pst_B_GO,
	BOOL				_b_Inverse
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_A_GO, *pst_B_GO;
	COL_tdst_ZDx		*pst_ZDx;
	MATH_tdst_Vector	st_A_GCS_Center, st_A_GCS_Min, st_A_GCS_Max;
	float				f_A_GCS_Radius;
	MATH_tdst_Matrix	*pst_B_GlobalMatrix;
	COL_tdst_Instance	*pst_A_Instance;
	COL_tdst_ColSet		*pst_A_ColSet;
	UCHAR				uc_ENG_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_A_GO || !_pst_B_GO) return FALSE;

	/* If one of the 2 objects is inactive, return FALSE; */
	if
	(
		!OBJ_b_TestStatusFlag(_pst_A_GO, OBJ_C_StatusFlag_Active)
	||	!OBJ_b_TestStatusFlag(_pst_B_GO, OBJ_C_StatusFlag_Active)
	) return FALSE;

	if(_b_Inverse)
	{
		pst_A_GO = _pst_B_GO;
		pst_B_GO = _pst_A_GO;
	}
	else
	{
		pst_A_GO = _pst_A_GO;
		pst_B_GO = _pst_B_GO;
	}

	/* If A has no instance, we return FALSE; */
	if
	(
		(!OBJ_b_TestIdentityFlag(pst_A_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	||	!pst_A_GO->pst_Extended
	||	!pst_A_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance
	) return 0;

	pst_A_Instance = ((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_A_ColSet = pst_A_Instance->pst_ColSet;

	if(!pst_A_ColSet) return 0;

	uc_ENG_Index = pst_A_ColSet->pauc_AI_Indexes[_uc_AI_Index];

	if(uc_ENG_Index >= pst_A_Instance->uc_NbOfZDx) return FALSE;

	if(!COL_b_Instance_IsActive(pst_A_Instance, uc_ENG_Index)) return FALSE;

	pst_ZDx = *(((COL_tdst_Base *) pst_A_GO->pst_Extended->pst_Col)->pst_Instance->dpst_ZDx + uc_ENG_Index);

	/* If the ZDx in not a ZDE, we return FALSE; */
	if(!COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_ZDE)) return FALSE;

	switch(pst_ZDx->uc_Type)
	{
	case COL_C_Zone_Sphere:
		COL_Zone_Transform(pst_ZDx, pst_A_GO, (ULONG *) &st_A_GCS_Center, (ULONG *) &f_A_GCS_Radius);
		break;

	case COL_C_Zone_Box:
		COL_Zone_Transform(pst_ZDx, pst_A_GO, (ULONG *) &st_A_GCS_Min, (ULONG *) &st_A_GCS_Max);
		break;
	}

	pst_B_GlobalMatrix = OBJ_pst_GetAbsoluteMatrix(pst_B_GO);
	if(OBJ_BV_IsSphere(pst_B_GO->pst_BV))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_B_GCS_Center;
		float				f_B_GCS_Radius;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		switch(pst_ZDx->uc_Type)
		{
		case COL_C_Zone_Sphere:
			MATH_TransformVertexNoScale(&st_B_GCS_Center, pst_B_GlobalMatrix, OBJ_pst_BV_GetCenter(pst_B_GO->pst_BV));
			f_B_GCS_Radius = OBJ_f_BV_GetRadius(pst_B_GO->pst_BV);
			return INT_SphereSphere(&st_A_GCS_Center, f_A_GCS_Radius, &st_B_GCS_Center, f_B_GCS_Radius);

		case COL_C_Zone_Box:
			break;
		}
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_B_GCS_Max, st_B_GCS_Min;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		switch(pst_ZDx->uc_Type)
		{
		case COL_C_Zone_Sphere:
			MATH_AddVector(&st_B_GCS_Max, OBJ_pst_BV_GetGMax(pst_B_GO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_B_GO));
			MATH_AddVector(&st_B_GCS_Min, OBJ_pst_BV_GetGMin(pst_B_GO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_B_GO));

			return INT_SphereAABBox(&st_A_GCS_Center, f_A_GCS_Radius, &st_B_GCS_Min, &st_B_GCS_Max);

		case COL_C_Zone_Box:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix st_InvAMatrix	ONLY_PSX2_ALIGNED(16);
				MATH_tdst_Vector				st_A_Center, st_B_Center, st_Distance;
				MATH_tdst_Vector				st_A_GCS_Center, st_B_GCS_Center;
				MATH_tdst_Vector				*pst_A_Max, *pst_A_Min, *pst_B_Max, *pst_B_Min;
				MATH_tdst_Vector				st_A_Scale;
				void							*p_A_Shape;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				/* First, we must compute the Matrix to go from ACS to GCS. */
				if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(pst_A_GO)))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Matrix	st_A_NoScaleMatrix;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_CopyMatrix(&st_A_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(pst_A_GO));
					MATH_ClearScale(&st_A_NoScaleMatrix, 1);
					MATH_InvertMatrix(&st_InvAMatrix, &st_A_NoScaleMatrix);
				}
				else
					MATH_InvertMatrix(&st_InvAMatrix, OBJ_pst_GetAbsoluteMatrix(pst_A_GO));

				p_A_Shape = pst_ZDx->p_Shape;

				pst_A_Max = COL_pst_Shape_GetMax(p_A_Shape);
				pst_A_Min = COL_pst_Shape_GetMin(p_A_Shape);

				pst_B_Max = OBJ_pst_BV_GetGMax(pst_B_GO->pst_BV);
				pst_B_Min = OBJ_pst_BV_GetGMin(pst_B_GO->pst_BV);

				/*$F ---- A Center Computation --------- */
				MATH_AddVector(&st_A_Center, pst_A_Max, pst_A_Min);
				MATH_MulEqualVector(&st_A_Center, 0.5f);

				/*$F ---- A Center in GCS -------------- */
				MATH_TransformVertex(&st_A_GCS_Center, OBJ_pst_GetAbsoluteMatrix(pst_A_GO), &st_A_Center);

				/*$F ---- B Center Computation --------- */
				MATH_AddVector(&st_B_Center, pst_B_Max, pst_B_Min);
				MATH_MulEqualVector(&st_B_Center, 0.5f);

				/*$F ---- B Center in GCS -------------- */
				MATH_TransformVertex(&st_B_GCS_Center, OBJ_pst_GetAbsoluteMatrix(pst_B_GO), &st_B_Center);

				/*$F ---- Real Translation between A Center and B Center ----------- */
				MATH_SubVector(&st_Distance, &st_B_GCS_Center, &st_A_GCS_Center);

				/* Sets the Translation vector of the Transform Matrix. */
				MATH_CopyVector(MATH_pst_GetTranslation(&st_InvAMatrix), &st_Distance);

				MATH_GetScale(&st_A_Scale, OBJ_pst_GetAbsoluteMatrix(pst_A_GO));

				return
					(
						INT_HighLevel_OBBoxOBBox
						(
							pst_A_Min,
							pst_A_Max,
							pst_B_Min,
							pst_B_Max,
							&st_InvAMatrix,
							&st_A_Scale
						)
					);
			}
		}
	}

	return FALSE;
}

#ifdef ODE_INSIDE
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ODE_ComputeInfo(OBJ_tdst_GameObject *_pst_GO, void *_pv_Data1, void *_pv_Data2, BOOL _b_Global)
{
	DYN_tdst_ODE		*pst_ODE;
	MATH_tdst_Vector	st_Temp;
	MATH_tdst_Vector	st_BoxPt;

	if(!_pst_GO || !OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ODE))
		return;

	pst_ODE = _pst_GO->pst_Base->pst_ODE;
	switch(pst_ODE->uc_Type)
	{
		case ODE_TYPE_SPHERE:
			MATH_CopyVector(&st_Temp, &pst_ODE->st_Offset);
			if(_b_Global)
				MATH_TransformVertex((MATH_tdst_Vector *) _pv_Data1, _pst_GO->pst_GlobalMatrix, &st_Temp);
			else
				MATH_CopyVector((MATH_tdst_Vector *) _pv_Data1, &st_Temp);
			*(float *)_pv_Data2 = pst_ODE->f_X;
			break;

		case ODE_TYPE_BOX:
			MATH_CopyVector(&st_Temp, &pst_ODE->st_Offset);
			st_BoxPt.x = st_Temp.x - (pst_ODE->f_X / 2.0f);
			st_BoxPt.y = st_Temp.y - (pst_ODE->f_Y / 2.0f);
			st_BoxPt.z = st_Temp.z - (pst_ODE->f_Z / 2.0f);
			if(_b_Global)
				MATH_TransformVertex((MATH_tdst_Vector *) _pv_Data1, _pst_GO->pst_GlobalMatrix, &st_BoxPt);
			else
				MATH_CopyVector((MATH_tdst_Vector *) _pv_Data1, &st_BoxPt);


			st_BoxPt.x = st_Temp.x + (pst_ODE->f_X / 2.0f);
			st_BoxPt.y = st_Temp.y + (pst_ODE->f_Y / 2.0f);
			st_BoxPt.z = st_Temp.z + (pst_ODE->f_Z / 2.0f);
			if(_b_Global)
				MATH_TransformVertex((MATH_tdst_Vector *) _pv_Data2, _pst_GO->pst_GlobalMatrix, &st_BoxPt);
			else
				MATH_CopyVector((MATH_tdst_Vector *) _pv_Data2, &st_BoxPt);

			break;
	}
}
#endif

/*
 =======================================================================================================================
    Aim:    Detects whether the given ZDE collides with the uc_B_Index(ieme) zone of _pst_B_GO.
 =======================================================================================================================
 */
BOOL COL_ZDE_ZDECollide
(
	OBJ_tdst_GameObject *pst_A_GO,
	UCHAR				uc_AI_Index,
	UCHAR				uc_B_Index,
	OBJ_tdst_GameObject *pst_B_GO
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *_pst_A_GO, *_pst_B_GO;
	COL_tdst_Instance	*pst_A_Instance, *pst_B_Instance;
	COL_tdst_ZDx		*pst_A_ZDx, *pst_B_ZDx;
	UCHAR				uc_B_ENG_Index;
	UCHAR				_uc_AI_Index, _uc_B_Index;
	UCHAR				uc_Combination;
	COL_tdst_ColSet		*pst_A_ColSet;
	UCHAR				uc_ENG_Index;
	BOOL				b_ODE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_uc_AI_Index = uc_AI_Index;
	_uc_B_Index = uc_B_Index;
	_pst_A_GO = pst_A_GO;
	_pst_B_GO = pst_B_GO;

#ifdef ODE_INSIDE
	if((uc_AI_Index == 50) && (uc_B_Index == 50))
	{
#ifdef ACTIVE_EDITORS
		LINK_PrintStatusMsg("COL_ZDEZDECollide error with ODE");
#endif
		return FALSE;
	}

	b_ODE = (uc_B_Index == 50) || (uc_AI_Index == 50);

	if(uc_AI_Index == 50)
	{
		_uc_AI_Index = uc_B_Index;
		_uc_B_Index = uc_AI_Index;

		_pst_A_GO = pst_B_GO;
		_pst_B_GO = pst_A_GO;
	}
#else

	b_ODE = FALSE;
#endif

	if(!_pst_A_GO || !_pst_B_GO) return FALSE;

	/* If one of the 2 objects is inactive, return FALSE; */
	if
	(
		!OBJ_b_TestStatusFlag(_pst_A_GO, OBJ_C_StatusFlag_Active)
	||	!OBJ_b_TestStatusFlag(_pst_B_GO, OBJ_C_StatusFlag_Active)
	) return FALSE;

	/* If A has no instance, we return FALSE; */
	if
	(
		(!OBJ_b_TestIdentityFlag(_pst_A_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	||	!_pst_A_GO->pst_Extended
	||	!_pst_A_GO->pst_Extended->pst_Col
	||	!((COL_tdst_Base *) _pst_A_GO->pst_Extended->pst_Col)->pst_Instance
	) return 0;

	/* If B has no instance, we return FALSE; */
	if
	(

		(
			!b_ODE
		&&	(
				(!OBJ_b_TestIdentityFlag(_pst_B_GO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
			||	!_pst_B_GO->pst_Extended
			||	!_pst_B_GO->pst_Extended->pst_Col
			||	!((COL_tdst_Base *) _pst_B_GO->pst_Extended->pst_Col)->pst_Instance
			)
		)
#ifdef ODE_INSIDE

	||	(
			b_ODE
		&&	(
				(!OBJ_b_TestIdentityFlag(_pst_B_GO, OBJ_C_IdentityFlag_ODE))
			||	!_pst_B_GO->pst_Base
			||	!_pst_B_GO->pst_Base->pst_ODE
			)	
		)
#endif
	
	) return 0;

	pst_A_Instance = ((COL_tdst_Base *) _pst_A_GO->pst_Extended->pst_Col)->pst_Instance;
	pst_A_ColSet = pst_A_Instance->pst_ColSet;

	if(!pst_A_ColSet) return 0;

	uc_ENG_Index = pst_A_ColSet->pauc_AI_Indexes[_uc_AI_Index];

	if(uc_ENG_Index >= pst_A_Instance->uc_NbOfZDx) return FALSE;

	if(!COL_b_Instance_IsActive(pst_A_Instance, uc_ENG_Index)) return FALSE;

	pst_A_ZDx = *(pst_A_Instance->dpst_ZDx + uc_ENG_Index);

	/* If the A ZDx in not a ZDE, we return FALSE; */
	if(!COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_ZDE)) return FALSE;

	uc_Combination = (pst_A_ZDx->uc_Type << 4);

#ifdef ODE_INSIDE
	if(b_ODE)
	{
		DYN_tdst_ODE	*pst_ODE;

		pst_ODE = _pst_B_GO->pst_Base->pst_ODE;
		switch(pst_ODE->uc_Type)
		{
		case ODE_TYPE_SPHERE:
			uc_Combination += COL_C_Zone_Sphere;
			break;

		case ODE_TYPE_BOX:
			uc_Combination += COL_C_Zone_Box;
			break;

		default:
			return FALSE;
		}
	}
	else
#endif
	{
		pst_B_Instance = ((COL_tdst_Base *) _pst_B_GO->pst_Extended->pst_Col)->pst_Instance;
		uc_B_ENG_Index = pst_B_Instance->pst_ColSet->pauc_AI_Indexes[_uc_B_Index];

		if(uc_B_ENG_Index > pst_B_Instance->uc_NbOfZDx - 1) 
			return FALSE;

		if(!COL_b_Instance_IsActive(pst_B_Instance, uc_B_ENG_Index)) 
			return FALSE;

		pst_B_ZDx = *(pst_B_Instance->dpst_ZDx + uc_B_ENG_Index);

		/* If the B ZDx is not a ZDE, we continue. */
		if(!COL_b_Zone_TestFlag(pst_B_ZDx, COL_C_Zone_ZDE)) 
			return FALSE;

		uc_Combination += pst_B_ZDx->uc_Type;
			
	}

	switch(uc_Combination)
	{
	case COL_C_SphereAndSphere:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_A_GCS_Center, st_B_GCS_Center;
			float				f_A_GCS_Radius, f_B_GCS_Radius;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			COL_Zone_Transform(pst_A_ZDx, _pst_A_GO, (ULONG *) &st_A_GCS_Center, (ULONG *) &f_A_GCS_Radius);
#ifdef ODE_INSIDE
			if(b_ODE)
				COL_ODE_ComputeInfo(_pst_B_GO, (ULONG *) &st_B_GCS_Center, (ULONG *) &f_B_GCS_Radius, 1);
			else
#endif
				COL_Zone_Transform(pst_B_ZDx, _pst_B_GO, (ULONG *) &st_B_GCS_Center, (ULONG *) &f_B_GCS_Radius);

			return INT_SphereSphere(&st_A_GCS_Center, f_A_GCS_Radius, &st_B_GCS_Center, f_B_GCS_Radius);
		}
		break;

	case COL_C_SphereAndBox:
		{
#ifdef ODE_INSIDE
			if(b_ODE)			
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector				st_A_Scale, st_GCS_Center;
				MATH_tdst_Vector				st_B_GCS_Min, st_B_GCS_Max;
				float							f_GCS_Radius, f_Radius;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				COL_ODE_ComputeInfo(_pst_B_GO, (ULONG *) &st_B_GCS_Min, (ULONG *) &st_B_GCS_Max, 1);

				f_Radius = COL_f_Shape_GetRadius(pst_A_ZDx->p_Shape);

				if(!(COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_NoScale)))
				{
					MATH_GetScale(&st_A_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));

					MATH_TransformVertex
					(
						&st_GCS_Center,
						OBJ_pst_GetAbsoluteMatrix(_pst_A_GO),
						COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape)
					);

					MATH_GetScale(&st_A_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));
					f_GCS_Radius = f_Radius * fMax3(st_A_Scale.x, st_A_Scale.y, st_A_Scale.z);

				}
				else
				{
					MATH_TransformVertexNoScale
					(
						&st_GCS_Center,
						OBJ_pst_GetAbsoluteMatrix(_pst_A_GO),
						COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape)
					);
					f_GCS_Radius = f_Radius;
				}

				return INT_SphereOBBox(&st_GCS_Center, f_GCS_Radius, &st_B_GCS_Min, &st_B_GCS_Max, NULL);

			}
			else

#endif
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix st_InvBMatrix	ONLY_PSX2_ALIGNED(16);
				MATH_tdst_Matrix				*pst_Bmat;
				MATH_tdst_Vector				st_BCS_Center, st_Temp;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_B_GO)))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	st_A_Scale, st_B_Scale, st_ScaledMin, st_ScaledMax;
					MATH_tdst_Vector	*pst_Min, *pst_Max;
					float				f_Radius, f_GCS_Radius;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					/*
					* There, the Box is a zone from a scaled object. We will have the "patatoid"
					* effect if we try to transform the sphere into the Box's coordinate system. So,
					* we compute the Min/Max of the box with the Scale.
					*/
					pst_Min = COL_pst_Shape_GetMin(pst_B_ZDx->p_Shape);
					pst_Max = COL_pst_Shape_GetMax(pst_B_ZDx->p_Shape);

					if(!(COL_b_Zone_TestFlag(pst_B_ZDx, COL_C_Zone_NoScale)))
					{
						MATH_GetScale(&st_B_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_B_GO));

						st_ScaledMin.x = pst_Min->x * st_B_Scale.x;
						st_ScaledMin.y = pst_Min->y * st_B_Scale.y;
						st_ScaledMin.z = pst_Min->z * st_B_Scale.z;

						st_ScaledMax.x = pst_Max->x * st_B_Scale.x;
						st_ScaledMax.y = pst_Max->y * st_B_Scale.y;
						st_ScaledMax.z = pst_Max->z * st_B_Scale.z;
					}
					else
					{
						MATH_CopyVector(&st_ScaledMin, pst_Min);
						MATH_CopyVector(&st_ScaledMax, pst_Max);
					}

					/*
					* There, our goal is to obtain a Matrix to go from the GCS to the "No Scaled"
					* BCS. So, we must no use the Scale componant of the B matrix when computing the
					* Translation vector of this "No Scaled" BCS.
					*/
					pst_Bmat = OBJ_pst_GetAbsoluteMatrix(_pst_B_GO);
					MATH_Transp33MatrixWithoutBuffer(&st_InvBMatrix, pst_Bmat);
					MATH_SetType(&st_InvBMatrix, MATH_l_GetType(pst_Bmat));

					st_InvBMatrix.Sx = fInv(pst_Bmat->Sx);
					st_InvBMatrix.Sy = fInv(pst_Bmat->Sy);
					st_InvBMatrix.Sz = fInv(pst_Bmat->Sz);

					MATH_NegVector(MATH_pst_GetTranslation(&st_InvBMatrix), MATH_pst_GetTranslation(pst_Bmat));
					MATH_TransformVectorNoScale(&st_InvBMatrix.T, &st_InvBMatrix, &st_InvBMatrix.T);

					if(!(COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_NoScale)))
					{
						MATH_TransformVertex
						(
							&st_Temp,
							OBJ_pst_GetAbsoluteMatrix(_pst_A_GO),
							COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape)
						);
					}
					else
					{
						MATH_TransformVertexNoScale
						(
							&st_Temp,
							OBJ_pst_GetAbsoluteMatrix(_pst_A_GO),
							COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape)
						);
					}

					/* Must NOT use the B Scale. */
					MATH_TransformVertexNoScale(&st_BCS_Center, &st_InvBMatrix, &st_Temp);

					f_Radius = COL_f_Shape_GetRadius(pst_A_ZDx->p_Shape);

					if(!(COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_NoScale)))
					{
						MATH_GetScale(&st_A_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));
						f_GCS_Radius = f_Radius * fMax3(st_A_Scale.x, st_A_Scale.y, st_A_Scale.z);
					}
					else
						f_GCS_Radius = f_Radius;

					return INT_SphereOBBox(&st_BCS_Center, f_GCS_Radius, &st_ScaledMin, &st_ScaledMax, NULL);
				}
				else
				{
					MATH_InvertMatrix(&st_InvBMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_B_GO));
					if(!(COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_NoScale)))
					{
						MATH_TransformVertex
						(
							&st_Temp,
							OBJ_pst_GetAbsoluteMatrix(_pst_A_GO),
							COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape)
						);
					}
					else
					{
						MATH_TransformVertexNoScale
						(
							&st_Temp,
							OBJ_pst_GetAbsoluteMatrix(_pst_A_GO),
							COL_pst_Shape_GetCenter(pst_A_ZDx->p_Shape)
						);
					}

					MATH_TransformVertex(&st_BCS_Center, &st_InvBMatrix, &st_Temp);

					return INT_SphereOBBox
						(
							&st_BCS_Center,
							COL_f_Shape_GetRadius(pst_A_ZDx->p_Shape),
							COL_pst_Shape_GetMin(pst_B_ZDx->p_Shape),
							COL_pst_Shape_GetMax(pst_B_ZDx->p_Shape),
							NULL
						);
				}
			}
		}
		break;

	case COL_C_BoxAndSphere:
		{
#ifdef ODE_INSIDE
			if(b_ODE)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	st_GCS_Center;
				float				f_GCS_Radius;
				MATH_tdst_Vector	st_GCS_Min, st_GCS_Max;
				MATH_tdst_Vector	*pst_Min, *pst_Max;
				MATH_tdst_Vector	st_A_Scale, st_ScaledMin, st_ScaledMax;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				COL_ODE_ComputeInfo(_pst_B_GO, (ULONG *) &st_GCS_Center, (ULONG *) &f_GCS_Radius, 0);

				pst_Min = COL_pst_Shape_GetMin(pst_A_ZDx->p_Shape);
				pst_Max = COL_pst_Shape_GetMax(pst_A_ZDx->p_Shape);


				if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_A_GO)))
				{
					if(!(COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_NoScale)))
					{
						MATH_GetScale(&st_A_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));

						st_ScaledMin.x = pst_Min->x * st_A_Scale.x;
						st_ScaledMin.y = pst_Min->y * st_A_Scale.y;
						st_ScaledMin.z = pst_Min->z * st_A_Scale.z;

						st_ScaledMax.x = pst_Max->x * st_A_Scale.x;
						st_ScaledMax.y = pst_Max->y * st_A_Scale.y;
						st_ScaledMax.z = pst_Max->z * st_A_Scale.z;
					}
					else
					{
						MATH_CopyVector(&st_ScaledMin, pst_Min);
						MATH_CopyVector(&st_ScaledMax, pst_Max);
					}
				}
				else
				{
					MATH_CopyVector(&st_ScaledMin, pst_Min);
					MATH_CopyVector(&st_ScaledMax, pst_Max);
				}

				MATH_TransformVertexNoScale(&st_GCS_Min, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO), &st_ScaledMin);
				MATH_TransformVertexNoScale(&st_GCS_Max, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO), &st_ScaledMax);

				return INT_SphereOBBox(&st_GCS_Center, f_GCS_Radius, &st_GCS_Min, &st_GCS_Max, NULL);
			}
			else
#endif
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix st_InvAMatrix	ONLY_PSX2_ALIGNED(16);
				MATH_tdst_Matrix				*pst_Amat;
				MATH_tdst_Vector				st_ACS_Center, st_Temp;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_A_GO)))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	st_A_Scale, st_B_Scale, st_ScaledMin, st_ScaledMax;
					MATH_tdst_Vector	*pst_Min, *pst_Max;
					float				f_Radius, f_GCS_Radius;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					/*
					* There, the Box is a zone from a scaled object. We will have the "patatoid"
					* effect if we try to transform the sphere into the Box's coordinate system. So,
					* we compute the Min/Max of the box with the Scale.
					*/
					pst_Min = COL_pst_Shape_GetMin(pst_A_ZDx->p_Shape);
					pst_Max = COL_pst_Shape_GetMax(pst_A_ZDx->p_Shape);

					if(!(COL_b_Zone_TestFlag(pst_A_ZDx, COL_C_Zone_NoScale)))
					{
						MATH_GetScale(&st_A_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));

						st_ScaledMin.x = pst_Min->x * st_A_Scale.x;
						st_ScaledMin.y = pst_Min->y * st_A_Scale.y;
						st_ScaledMin.z = pst_Min->z * st_A_Scale.z;

						st_ScaledMax.x = pst_Max->x * st_A_Scale.x;
						st_ScaledMax.y = pst_Max->y * st_A_Scale.y;
						st_ScaledMax.z = pst_Max->z * st_A_Scale.z;
					}
					else
					{
						MATH_CopyVector(&st_ScaledMin, pst_Min);
						MATH_CopyVector(&st_ScaledMax, pst_Max);
					}

					/*
					* There, our goal is to obtain a Matrix to go from the GCS to the "No Scaled"
					* ACS. So, we must no use the Scale componant of the A matrix when computing the
					* Translation vector of this "No Scaled" ACS.
					*/
					pst_Amat = OBJ_pst_GetAbsoluteMatrix(_pst_A_GO);
					MATH_Transp33MatrixWithoutBuffer(&st_InvAMatrix, pst_Amat);
					MATH_SetType(&st_InvAMatrix, MATH_l_GetType(pst_Amat));

					st_InvAMatrix.Sx = fInv(pst_Amat->Sx);
					st_InvAMatrix.Sy = fInv(pst_Amat->Sy);
					st_InvAMatrix.Sz = fInv(pst_Amat->Sz);

					MATH_NegVector(MATH_pst_GetTranslation(&st_InvAMatrix), MATH_pst_GetTranslation(pst_Amat));
					MATH_TransformVectorNoScale(&st_InvAMatrix.T, &st_InvAMatrix, &st_InvAMatrix.T);

					if(!(COL_b_Zone_TestFlag(pst_B_ZDx, COL_C_Zone_NoScale)))
					{
						MATH_TransformVertex
						(
							&st_Temp,
							OBJ_pst_GetAbsoluteMatrix(_pst_B_GO),
							COL_pst_Shape_GetCenter(pst_B_ZDx->p_Shape)
						);
					}
					else
					{
						MATH_TransformVertexNoScale
						(
							&st_Temp,
							OBJ_pst_GetAbsoluteMatrix(_pst_B_GO),
							COL_pst_Shape_GetCenter(pst_B_ZDx->p_Shape)
						);
					}

					/* Must NOT use the A Scale. */
					MATH_TransformVertexNoScale(&st_ACS_Center, &st_InvAMatrix, &st_Temp);

					f_Radius = COL_f_Shape_GetRadius(pst_B_ZDx->p_Shape);

					if(!(COL_b_Zone_TestFlag(pst_B_ZDx, COL_C_Zone_NoScale)))
					{
						MATH_GetScale(&st_B_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_B_GO));
						f_GCS_Radius = f_Radius * fMax3(st_B_Scale.x, st_B_Scale.y, st_B_Scale.z);
					}
					else
						f_GCS_Radius = f_Radius;

					return INT_SphereOBBox(&st_ACS_Center, f_GCS_Radius, &st_ScaledMin, &st_ScaledMax, NULL);
				}
				else
				{
					MATH_InvertMatrix(&st_InvAMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));
					MATH_TransformVertex
					(
						&st_Temp,
						OBJ_pst_GetAbsoluteMatrix(_pst_B_GO),
						COL_pst_Shape_GetCenter(pst_B_ZDx->p_Shape)
					);
					MATH_TransformVertex(&st_ACS_Center, &st_InvAMatrix, &st_Temp);

					return INT_SphereOBBox
						(
							&st_ACS_Center,
							COL_f_Shape_GetRadius(pst_B_ZDx->p_Shape),
							COL_pst_Shape_GetMin(pst_A_ZDx->p_Shape),
							COL_pst_Shape_GetMax(pst_A_ZDx->p_Shape),
							NULL
						);
				}
			}
		}
		break;

	case COL_C_BoxAndBox:
		{
#ifdef ODE_INSIDE
			if(b_ODE)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix st_InvAMatrix	ONLY_PSX2_ALIGNED(16);
				MATH_tdst_Matrix				st_TransMatrix;
				MATH_tdst_Vector				st_A_Center, st_B_Center, st_Distance, st_T;
				MATH_tdst_Vector				st_A_GCS_Center, st_B_GCS_Center;
				MATH_tdst_Vector				*pst_A_Max, *pst_A_Min;
				MATH_tdst_Vector				st_B_Max, st_B_Min;
				MATH_tdst_Vector				st_A_Scale;
				void							*p_A_Shape, *p_B_Shape;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				/* First, we must compute the Matrix to go from BCS to ACS. */
				if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_A_GO)))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Matrix	st_A_NoScaleMatrix;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_CopyMatrix(&st_A_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));
					MATH_ClearScale(&st_A_NoScaleMatrix, 1);
					MATH_InvertMatrix(&st_InvAMatrix, &st_A_NoScaleMatrix);
				}
				else
					MATH_InvertMatrix(&st_InvAMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));

				MATH_MulMatrixMatrix(&st_TransMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_B_GO), &st_InvAMatrix);
				p_A_Shape = pst_A_ZDx->p_Shape;
				p_B_Shape = pst_B_ZDx->p_Shape;

				pst_A_Max = COL_pst_Shape_GetMax(p_A_Shape);
				pst_A_Min = COL_pst_Shape_GetMin(p_A_Shape);

				COL_ODE_ComputeInfo(_pst_B_GO, &st_B_Min, &st_B_Max, 0);

				/*$F ---- A Center Computation --------- */
				MATH_AddVector(&st_A_Center, pst_A_Max, pst_A_Min);
				MATH_MulEqualVector(&st_A_Center, 0.5f);

				/*$F ---- A Center in GCS -------------- */
				MATH_TransformVertex(&st_A_GCS_Center, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO), &st_A_Center);

				/*$F ---- B Center Computation --------- */
				MATH_AddVector(&st_B_Center, &st_B_Max, &st_B_Min);
				MATH_MulEqualVector(&st_B_Center, 0.5f);

				/*$F ---- B Center in GCS -------------- */
				MATH_TransformVertex(&st_B_GCS_Center, OBJ_pst_GetAbsoluteMatrix(_pst_B_GO), &st_B_Center);

				/*$F ---- Real Translation between A Center and B Center ----------- */
				MATH_SubVector(&st_Distance, &st_B_GCS_Center, &st_A_GCS_Center);

				/* The Translation vector that is in the GCS. We want it to be in the ACS. */
				MATH_TransformVector(&st_T, &st_InvAMatrix, &st_Distance);
				MATH_CopyVector(MATH_pst_GetTranslation(&st_TransMatrix), &st_T);

				MATH_GetScale(&st_A_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));

				return(INT_HighLevel_OBBoxOBBox(pst_A_Min, pst_A_Max, &st_B_Min, &st_B_Max, &st_TransMatrix, &st_A_Scale));


			}
			else
#endif
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix st_InvAMatrix	ONLY_PSX2_ALIGNED(16);
				MATH_tdst_Matrix				st_TransMatrix;
				MATH_tdst_Vector				st_A_Center, st_B_Center, st_Distance, st_T;
				MATH_tdst_Vector				st_A_GCS_Center, st_B_GCS_Center;
				MATH_tdst_Vector				*pst_A_Max, *pst_A_Min, *pst_B_Max, *pst_B_Min;
				MATH_tdst_Vector				st_A_Scale;
				void							*p_A_Shape, *p_B_Shape;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				/* First, we must compute the Matrix to go from BCS to ACS. */
				if(MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(_pst_A_GO)))
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Matrix	st_A_NoScaleMatrix;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					MATH_CopyMatrix(&st_A_NoScaleMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));
					MATH_ClearScale(&st_A_NoScaleMatrix, 1);
					MATH_InvertMatrix(&st_InvAMatrix, &st_A_NoScaleMatrix);
				}
				else
					MATH_InvertMatrix(&st_InvAMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));

				MATH_MulMatrixMatrix(&st_TransMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_B_GO), &st_InvAMatrix);
				p_A_Shape = pst_A_ZDx->p_Shape;
				p_B_Shape = pst_B_ZDx->p_Shape;

				pst_A_Max = COL_pst_Shape_GetMax(p_A_Shape);
				pst_A_Min = COL_pst_Shape_GetMin(p_A_Shape);
				pst_B_Max = COL_pst_Shape_GetMax(p_B_Shape);
				pst_B_Min = COL_pst_Shape_GetMin(p_B_Shape);

				/*$F ---- A Center Computation --------- */
				MATH_AddVector(&st_A_Center, pst_A_Max, pst_A_Min);
				MATH_MulEqualVector(&st_A_Center, 0.5f);

				/*$F ---- A Center in GCS -------------- */
				MATH_TransformVertex(&st_A_GCS_Center, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO), &st_A_Center);

				/*$F ---- B Center Computation --------- */
				MATH_AddVector(&st_B_Center, pst_B_Max, pst_B_Min);
				MATH_MulEqualVector(&st_B_Center, 0.5f);

				/*$F ---- B Center in GCS -------------- */
				MATH_TransformVertex(&st_B_GCS_Center, OBJ_pst_GetAbsoluteMatrix(_pst_B_GO), &st_B_Center);

				/*$F ---- Real Translation between A Center and B Center ----------- */
				MATH_SubVector(&st_Distance, &st_B_GCS_Center, &st_A_GCS_Center);

				/* The Translation vector that is in the GCS. We want it to be in the ACS. */
				MATH_TransformVector(&st_T, &st_InvAMatrix, &st_Distance);
				MATH_CopyVector(MATH_pst_GetTranslation(&st_TransMatrix), &st_T);

				MATH_GetScale(&st_A_Scale, OBJ_pst_GetAbsoluteMatrix(_pst_A_GO));

				return(INT_HighLevel_OBBoxOBBox(pst_A_Min, pst_A_Max, pst_B_Min, pst_B_Max, &st_TransMatrix, &st_A_Scale));
			}
		}
		break;
	}

	return FALSE;
}

/*$F
 =======================================================================================================================
    Aim:    Returns a list of pointers to objects that effectively collide the given object's ZDE
            with a filter.
 =======================================================================================================================
 */

ULONG COL_ListCreate
(
	OBJ_tdst_GameObject *_pst_GO,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID,
	UCHAR				_uc_AI_Index,
	WOR_tdst_World		*_pst_World,
	ULONG				*_pul_Array,
	UCHAR				uc_Data
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
	OBJ_tdst_GameObject **dpst_Result;
	UCHAR				uc_NbInter;
	BOOL				b_ZDE_BV, b_BV_ZDE, b_BV_Pivot, b_Pivot_BV, b_ZDE_Pivot;
	BOOL				b_Pivot_ZDE, b_ZDE_BVCenter, b_BVCenter_ZDE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO) return 0;

	b_ZDE_BV = (uc_Data == 0xFF) ? TRUE : FALSE;
	b_BV_ZDE = (uc_Data == 0xFE) ? TRUE : FALSE;
	b_BV_Pivot = (uc_Data == 0xFD) ? TRUE : FALSE;
	b_Pivot_BV = (uc_Data == 0xFC) ? TRUE : FALSE;
	b_ZDE_Pivot = (uc_Data == 0xFB) ? TRUE : FALSE;
	b_Pivot_ZDE = (uc_Data == 0xFA) ? TRUE : FALSE;
	b_ZDE_BVCenter = (uc_Data == 0xEF) ? TRUE : FALSE;
	b_BVCenter_ZDE = (uc_Data == 0xEE) ? TRUE : FALSE;

	dpst_Result = ((OBJ_tdst_GameObject **) _pul_Array);
	uc_NbInter = 0;

	if
	(
		b_ZDE_BV
	||	b_BV_ZDE
	||	b_BV_Pivot
	||	b_Pivot_BV
	||	b_ZDE_Pivot
	||	b_Pivot_ZDE
	||	b_ZDE_BVCenter
	||	b_BVCenter_ZDE
	||	(uc_Data == 50)
	)
	{
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_ActivObjects);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_ActivObjects);
	}
	else
	{
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ZDE);
		pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ZDE);
	}

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		/* We get the current Game Object. */
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if(TAB_b_IsAHole(pst_CurrentGO)) continue;

		/* If the object is filtered, we return. */
		if(!OBJ_b_TestFlag(pst_CurrentGO, _ul_OnFlags, _ul_OffFlags, _ul_FlagID)) continue;

		/* If the object is the current one, we skip it. */
		if(pst_CurrentGO == _pst_GO) continue;

		if(b_ZDE_Pivot)
		{
			/* If there was an intersection */
			if(COL_ZDE_PivotCollide(_pst_GO, _uc_AI_Index, pst_CurrentGO, 0))
			{
				*(dpst_Result + uc_NbInter) = pst_CurrentGO;
				uc_NbInter++;
			}

			continue;
		}

		if(b_Pivot_ZDE)
		{
			/* If there was an intersection */
			if(COL_ZDE_PivotCollide(_pst_GO, _uc_AI_Index, pst_CurrentGO, 1))
			{
				*(dpst_Result + uc_NbInter) = pst_CurrentGO;
				uc_NbInter++;
			}

			continue;
		}

		if(b_ZDE_BV)
		{
			/* If there was an intersection */
			if(COL_ZDE_BVCollide(_pst_GO, _uc_AI_Index, pst_CurrentGO, 0))
			{
				*(dpst_Result + uc_NbInter) = pst_CurrentGO;
				uc_NbInter++;
			}

			continue;
		}

		if(b_BV_ZDE)
		{
			/* If there was an intersection */
			if(COL_ZDE_BVCollide(_pst_GO, _uc_AI_Index, pst_CurrentGO, 1))
			{
				*(dpst_Result + uc_NbInter) = pst_CurrentGO;
				uc_NbInter++;
			}

			continue;
		}

		if(b_BV_Pivot)
		{
			/* If there was an intersection */
			if(COL_BV_PivotCollide(_pst_GO, pst_CurrentGO, 0))
			{
				*(dpst_Result + uc_NbInter) = pst_CurrentGO;
				uc_NbInter++;
			}

			continue;
		}

		if(b_Pivot_BV)
		{
			/* If there was an intersection */
			if(COL_BV_PivotCollide(_pst_GO, pst_CurrentGO, 1))
			{
				*(dpst_Result + uc_NbInter) = pst_CurrentGO;
				uc_NbInter++;
			}

			continue;
		}

		if(b_ZDE_BVCenter)
		{
			/* If there was an intersection */
			if(COL_ZDE_BVCenterCollide(_pst_GO, _uc_AI_Index, pst_CurrentGO, 0))
			{
				*(dpst_Result + uc_NbInter) = pst_CurrentGO;
				uc_NbInter++;
			}

			continue;
		}

		if(b_BVCenter_ZDE)
		{
			/* If there was an intersection */
			if(COL_ZDE_BVCenterCollide(_pst_GO, _uc_AI_Index, pst_CurrentGO, 1))
			{
				*(dpst_Result + uc_NbInter) = pst_CurrentGO;
				uc_NbInter++;
			}

			continue;
		}

		/* If there was an intersection */
		if(COL_ZDE_ZDECollide(_pst_GO, _uc_AI_Index, uc_Data, pst_CurrentGO))
		{
			*(dpst_Result + uc_NbInter) = pst_CurrentGO;
			uc_NbInter++;
		}
	}

	return uc_NbInter;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_ListCreateDistance
(
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID,
	WOR_tdst_World		*_pst_World,
	ULONG				*_pul_Array,
	MATH_tdst_Vector	*_pst_GCS_Center,
	float				_f_MaxDist,
	ULONG				_ul_MaxDist
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
	OBJ_tdst_GameObject **dpst_Result;
	UCHAR				uc_NbInter;
	float				f_CurrentSqrDist;
	float				f_SqrMaxDist;
	MATH_tdst_Vector	st_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_SqrMaxDist = fSqr(_f_MaxDist);
		
	dpst_Result = ((OBJ_tdst_GameObject **) _pul_Array);
	uc_NbInter = 0;

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_ActivObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_ActivObjects);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		/* We get the current Game Object. */
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if(TAB_b_IsAHole(pst_CurrentGO)) continue;

		/* If the object is filtered, we return. */
		if(!OBJ_b_TestFlag(pst_CurrentGO, _ul_OnFlags, _ul_OffFlags, _ul_FlagID)) continue;

		MATH_SubVector(&st_Temp, &pst_CurrentGO->pst_GlobalMatrix->T, _pst_GCS_Center);

		f_CurrentSqrDist = MATH_f_SqrNormVector(&st_Temp);

		/* If there was an intersection */
		if(f_CurrentSqrDist < f_SqrMaxDist)
		{
			*(dpst_Result + uc_NbInter) = pst_CurrentGO;
			uc_NbInter++;

			if(uc_NbInter >= (UCHAR) _ul_MaxDist)
			{
#ifdef ACTIVE_EDITORS
				LINK_PrintStatusMsg("COL_CenterDistanceListGet: Array is FULL");
#endif
				uc_NbInter = ((UCHAR) _ul_MaxDist) - 1;
			}
		}
		continue;
	}

	return uc_NbInter;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_ListCreateSight
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
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
	OBJ_tdst_GameObject **dpst_Result;
	UCHAR				uc_NbInter;
	MATH_tdst_Vector	st_Temp;
	float				tf_dot;
	float				f_CurrentSqrDist;
	float				f_SqrMaxDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_SqrMaxDist = fSqr(_f_MaxDist);
	dpst_Result = ((OBJ_tdst_GameObject **) _pul_Array);
	uc_NbInter = 0;

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_ActivObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_ActivObjects);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		/* We get the current Game Object. */
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(pst_CurrentGO == _pst_GO) continue;

		if(TAB_b_IsAHole(pst_CurrentGO)) continue;

		if(pst_CurrentGO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInactive) continue;

		/* If the object is filtered, we return. */
		if(!OBJ_b_TestFlag(pst_CurrentGO, _ul_OnFlags, _ul_OffFlags, _ul_FlagID)) continue;

		/* the object has to have info photo modifier */
		if ( !MDF_pst_GetByType(pst_CurrentGO, MDF_C_Modifier_InfoPhoto) )
			continue;
	
		OBJ_BV_ComputeCenter(pst_CurrentGO, &st_Temp);
		MATH_SubEqualVector(&st_Temp, _pst_GCS_Center);
		f_CurrentSqrDist = MATH_f_SqrNormVector(&st_Temp);
		if(f_CurrentSqrDist < f_SqrMaxDist)
		{
			MATH_NormalizeVector(&st_Temp, &st_Temp);
			tf_dot = MATH_f_DotProduct(&st_Temp, _pst_Sight);

			/* If there was an intersection */
			if(tf_dot > _f_MaxAngle)
			{
				*(dpst_Result + uc_NbInter) = pst_CurrentGO;
				uc_NbInter++;

				if(uc_NbInter >= (UCHAR) _ul_MaxDist)
				{
#ifdef ACTIVE_EDITORS
					LINK_PrintStatusMsg("COL_ListCreateSight: Array is FULL");
#endif
					uc_NbInter = ((UCHAR) _ul_MaxDist) - 1;
				}
			}
		}
		continue;
	}

	return uc_NbInter;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *COL_GetFirstObjectInRange
(
	WOR_tdst_World		*_pst_World,
	MATH_tdst_Vector	*_pst_Pos,
	float				_f_Distance,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_CurrentGO;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	MATH_tdst_Vector	st_PosToObject;
	float				f_SqrDistance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_World) return NULL;

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_ActivObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_ActivObjects);

	f_SqrDistance = fSqr(_f_Distance);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		/* We get the current Game Object. */
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if(TAB_b_IsAHole(pst_CurrentGO)) continue;

		/* If the object is filtered, we return. */
		if(!OBJ_b_TestFlag(pst_CurrentGO, _ul_OnFlags, _ul_OffFlags, _ul_FlagID)) continue;

		MATH_SubVector(&st_PosToObject, _pst_Pos, &pst_CurrentGO->pst_GlobalMatrix->T);

		if(MATH_f_SqrNormVector(&st_PosToObject) < f_SqrDistance)
		{
			return pst_CurrentGO;
		}
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_VisibleObjectListCreate
(
	WOR_tdst_World	*_pst_World,
	ULONG			_ul_OnFlags,
	ULONG			_ul_OffFlags,
	ULONG			_ul_FlagID,
	ULONG			*_pul_Array
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
	OBJ_tdst_GameObject **dpst_Result;
	ULONG				ul_NbVisible;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dpst_Result = ((OBJ_tdst_GameObject **) _pul_Array);
	ul_NbVisible = 0;

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		/* We get the current Game Object. */
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		if(TAB_b_IsAHole(pst_CurrentGO)) continue;

		/* Object BV is not visible --> Culled. */
		if(pst_CurrentGO->uc_LOD_Vis == 0) continue;

		/* If the object is filtered, we return. */
		if(!OBJ_b_TestFlag(pst_CurrentGO, _ul_OnFlags, _ul_OffFlags, _ul_FlagID)) continue;

		*(dpst_Result + ul_NbVisible) = pst_CurrentGO;
		ul_NbVisible++;
	}

	return ul_NbVisible;
}

/*$F
 =======================================================================================================================
 -	Les points du Triangle a tester sont donnes dans le repere global.
 -	Pour le moment, les colmaps mathematiques ne sont pas traitees.
 -	Le Vecteur [AB] represente le segment prioritaire.
 =======================================================================================================================
 */

int COL_TriangleCollision
(
	MATH_tdst_Vector	*_pst_A,
	MATH_tdst_Vector	*_pst_B,
	MATH_tdst_Vector	*_pst_C,
	MATH_tdst_Vector	*_pst_CollidedPoint,
	WOR_tdst_World		*_pst_World,
	ULONG				_ul_OnFlags,
	ULONG				_ul_OffFlags,
	ULONG				_ul_FlagID
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem						*pst_CurrentElem;
	TAB_tdst_PFelem						*pst_EndElem;
	OBJ_tdst_GameObject					*pst_CurrentGO;
	MATH_tdst_Matrix					st_InvertMatrix;
	MATH_tdst_Vector					st_GMin, st_GMax;
	MATH_tdst_Vector					st_OCS_A, st_OCS_B, st_OCS_C;
	MATH_tdst_Vector					*pst_T1, *pst_T2, *pst_T3;
	MATH_tdst_Vector					st_OCS_CollidedPoint, st_OCS_UnitAB, st_Temp;
	MATH_tdst_Vector					st_GO_Min, st_GO_Max;
	COL_tdst_ColMap						*pst_ColMap;
	COL_tdst_Cob						**dpst_Cob, **dpst_LastCob;
	COL_tdst_IndexedTriangles			*pst_TriangleCob;
	COL_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	COL_tdst_IndexedTriangle			*pst_Triangle, *pst_LastTriangle;
	COL_tdst_GameMat					*pst_GMat;
	float								f_Max, f_Dot;
	BOOL								Collision;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_CollidedPoint) MATH_InitVector(_pst_CollidedPoint, 0.0f, 0.0f, 0.0f);

	f_Max = -100000.0;
	Collision = FALSE;

	st_GMin.x = fMin3(_pst_A->x, _pst_B->x, _pst_C->x);
	st_GMin.y = fMin3(_pst_A->y, _pst_B->y, _pst_C->y);
	st_GMin.z = fMin3(_pst_A->z, _pst_B->z, _pst_C->z);

	st_GMax.x = fMax3(_pst_A->x, _pst_B->x, _pst_C->x);
	st_GMax.y = fMax3(_pst_A->y, _pst_B->y, _pst_C->y);
	st_GMax.z = fMax3(_pst_A->z, _pst_B->z, _pst_C->z);

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ColMap);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ColMap);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;

		/* If the object is filtered, we return. */
		if(!OBJ_b_TestFlag(pst_CurrentGO, _ul_OnFlags, _ul_OffFlags, _ul_FlagID)) continue;

		if(!(pst_CurrentGO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active)) continue;

		/* BV Rejection */
		MATH_AddVector
		(
			&st_GO_Min,
			&((OBJ_tdst_SingleBV *) pst_CurrentGO->pst_BV)->st_GMin,
			&pst_CurrentGO->pst_GlobalMatrix->T
		);
		MATH_AddVector
		(
			&st_GO_Max,
			&((OBJ_tdst_SingleBV *) pst_CurrentGO->pst_BV)->st_GMax,
			&pst_CurrentGO->pst_GlobalMatrix->T
		);

		if
		(
			fSup(st_GO_Min.x, st_GMax.x)
		||	fInf(st_GO_Max.x, st_GMin.x)
		||	fSup(st_GO_Min.y, st_GMax.y)
		||	fInf(st_GO_Max.y, st_GMin.y)
		||	fSup(st_GO_Min.z, st_GMax.z)
		||	fInf(st_GO_Max.z, st_GMin.z)
		) continue;

		MATH_InvertMatrix(&st_InvertMatrix, pst_CurrentGO->pst_GlobalMatrix);

		/* Scale of ColMap is already inserted into vertices */
		MATH_TransformVertexNoScale(&st_OCS_A, &st_InvertMatrix, _pst_A);
		MATH_TransformVertexNoScale(&st_OCS_B, &st_InvertMatrix, _pst_B);
		MATH_TransformVertexNoScale(&st_OCS_C, &st_InvertMatrix, _pst_C);

		MATH_SubVector(&st_OCS_UnitAB, &st_OCS_B, &st_OCS_A);
		MATH_NormalizeEqualVector(&st_OCS_UnitAB);

		pst_ColMap = ((COL_tdst_Base *) pst_CurrentGO->pst_Extended->pst_Col)->pst_ColMap;

		dpst_Cob = pst_ColMap->dpst_Cob;
		dpst_LastCob = dpst_Cob + pst_ColMap->uc_NbOfCob;
		for(; dpst_Cob < dpst_LastCob; dpst_Cob++)
		{
			/* for now, only geometric Cob are treated */
			if((*dpst_Cob)->uc_Type != COL_C_Zone_Triangles) continue;

			pst_TriangleCob = (*dpst_Cob)->pst_TriangleCob;

			pst_Element = pst_TriangleCob->dst_Element;
			pst_LastElement = pst_Element + pst_TriangleCob->l_NbElements;
			for(; pst_Element < pst_LastElement; pst_Element++)
			{
				pst_GMat = COL_pst_GMat_Get(*dpst_Cob, pst_Element);

				if(pst_GMat && (pst_GMat->ul_CustomBits & (COL_Cul_GMat_FlagX | COL_Cul_GMat_Camera))) continue;

				pst_Triangle = pst_Element->dst_Triangle;
				pst_LastTriangle = pst_Triangle + pst_Element->uw_NbTriangles;

				for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
				{
					pst_T1 = &pst_TriangleCob->dst_Point[pst_Triangle->auw_Index[0]];
					pst_T2 = &pst_TriangleCob->dst_Point[pst_Triangle->auw_Index[1]];
					pst_T3 = &pst_TriangleCob->dst_Point[pst_Triangle->auw_Index[2]];

					if
					(
						INT_FullTriangleTriangle
						(
							&st_OCS_A,
							&st_OCS_B,
							&st_OCS_C,
							pst_T1,
							pst_T2,
							pst_T3,
							&st_OCS_CollidedPoint
						)
					)
					{
						MATH_SubVector(&st_Temp, &st_OCS_CollidedPoint, &st_OCS_A);
						MATH_NormalizeEqualVector(&st_Temp);

						f_Dot = MATH_f_DotProduct(&st_Temp, &st_OCS_UnitAB);

						if(f_Dot > f_Max)
						{
							f_Max = f_Dot;
							Collision = TRUE;
							if(_pst_CollidedPoint)
							{
								MATH_TransformVertexNoScale
								(
									_pst_CollidedPoint,
									pst_CurrentGO->pst_GlobalMatrix,
									&st_OCS_CollidedPoint
								);
							}
						}
					}
				}
			}
		}
	}

	return Collision;
}

#ifdef ODE_INSIDE
/*$F
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ODEPrimitive_Callback(void *data, dGeomID o1, dGeomID o2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct dContact			*pcontact;
	dJointID				contact_id;
	WOR_tdst_World			*pst_World;
	OBJ_tdst_GameObject		*pst_A, *pst_B;
	DYN_tdst_ODE			*pst_ODE_B;
	int						numcol, i; 
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	pst_World = (WOR_tdst_World *) data;

	pst_A = (OBJ_tdst_GameObject *) dGeomGetData(o1);
	pst_B = (OBJ_tdst_GameObject *) dGeomGetData(o2);

	if
	(
		!(pst_A->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_ENABLE)
	||	!(pst_B->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_ENABLE)
 	)
	return;


	/* If both A and B are disabled, return */
	if
	(
		pst_A->pst_Base->pst_ODE->ode_id_body
	&&	pst_B->pst_Base->pst_ODE->ode_id_body 
	&& !(dBodyIsEnabled(pst_A->pst_Base->pst_ODE->ode_id_body))
	&& !(dBodyIsEnabled(pst_B->pst_Base->pst_ODE->ode_id_body))
	)
	return;


	/* If both A and B are Immovable, return */
	if
	(
		!pst_A->pst_Base->pst_ODE->ode_id_body
	&&	!pst_B->pst_Base->pst_ODE->ode_id_body 
	)
	return;


	/* B Immovable et A en Pause */
	if
	(
		!pst_B->pst_Base->pst_ODE->ode_id_body 
	&&	pst_A->pst_Base->pst_ODE->ode_id_body
	&& !(dBodyIsEnabled(pst_A->pst_Base->pst_ODE->ode_id_body))
	)
	return;


	/* A Immovable et B en Pause */
	if
	(
		!pst_A->pst_Base->pst_ODE->ode_id_body 
	&&	pst_B->pst_Base->pst_ODE->ode_id_body
	&& !(dBodyIsEnabled(pst_B->pst_Base->pst_ODE->ode_id_body))
	)
	return;

	/* Uncollidable Objects */
	if(COL_b_UnCollidable(pst_A, pst_B))
	{	
		return;
	}
	
	
	/* Plan A */
	if
	(
		(pst_A->pst_Base->pst_ODE->uc_Type == ODE_TYPE_PLAN)
	&&  (pst_B->pst_Base->pst_ODE->ode_id_geom)
	)
	{
		dReal aabbox[6];
		MATH_tdst_Vector	*pst_BVMin,*pst_BVMax;
		MATH_tdst_Vector	st_GBVMin,st_GBVMax;				
		
		
		dGeomGetAABB(pst_B->pst_Base->pst_ODE->ode_id_geom, aabbox);
		
		pst_BVMin = OBJ_pst_BV_GetGMin(pst_A->pst_BV);		
		pst_BVMax = OBJ_pst_BV_GetGMax(pst_A->pst_BV);
		
		MATH_AddVector(&st_GBVMin, pst_BVMin, &pst_A->pst_GlobalMatrix->T);
		MATH_AddVector(&st_GBVMax, pst_BVMax, &pst_A->pst_GlobalMatrix->T);		
		
		if
		( 
			(st_GBVMin.x > aabbox[1]) || (st_GBVMax.x < aabbox[0])
		||	(st_GBVMin.y > aabbox[3]) || (st_GBVMax.y < aabbox[2])
		||	(st_GBVMin.z > aabbox[5]) || (st_GBVMax.z < aabbox[4])
		)						
		  	return;		
	}

	/* Plan B */
	if
	(
		(pst_B->pst_Base->pst_ODE->uc_Type == ODE_TYPE_PLAN)
	&&  (pst_A->pst_Base->pst_ODE->ode_id_geom)
	)
	{
		dReal aabbox[6];
		MATH_tdst_Vector	*pst_BVMin,*pst_BVMax;
		MATH_tdst_Vector	st_GBVMin,st_GBVMax;		
		
		
		dGeomGetAABB(pst_A->pst_Base->pst_ODE->ode_id_geom, aabbox);
		
		pst_BVMin = OBJ_pst_BV_GetGMin(pst_B->pst_BV);		
		pst_BVMax = OBJ_pst_BV_GetGMax(pst_B->pst_BV);
		
		MATH_AddVector(&st_GBVMin, pst_BVMin, &pst_B->pst_GlobalMatrix->T);
		MATH_AddVector(&st_GBVMax, pst_BVMax, &pst_B->pst_GlobalMatrix->T);		
		
		if
		( 
			(st_GBVMin.x > aabbox[1]) || (st_GBVMax.x < aabbox[0])
		||	(st_GBVMin.y > aabbox[3]) || (st_GBVMax.y < aabbox[2])
		||	(st_GBVMin.z > aabbox[5]) || (st_GBVMax.z < aabbox[4])
		)						
		  	return;		
	}



	/* A vs B - Which surface properties must we use, A or B ? */
	if(!(pst_A->pst_Base->pst_ODE->ode_id_body) && (pst_B->pst_Base->pst_ODE->ode_id_body))
		pst_ODE_B = pst_A->pst_Base->pst_ODE;
	else
		pst_ODE_B = pst_B->pst_Base->pst_ODE;

	numcol = dCollide(o1, o2, 5, &pst_World->ode_contacts_ode[pst_World->ode_contacts_num_ode].geom, sizeof(dContactGeom) + sizeof(dVector3) + sizeof(dSurfaceParameters));

	for(i = 0; i < numcol; i++)
	{
		pcontact = &pst_World->ode_contacts_ode[pst_World->ode_contacts_num_ode];

		pcontact->surface.mode = pst_ODE_B->SurfaceMode;
		pcontact->surface.mu =  pst_ODE_B->mu;
		pcontact->surface.mu2 = pst_ODE_B->mu2;
		pcontact->surface.bounce = pst_ODE_B->bounce;
		pcontact->surface.bounce_vel = pst_ODE_B->bounce_vel;
		pcontact->surface.soft_erp = pst_ODE_B->soft_erp;
		pcontact->surface.soft_cfm = pst_ODE_B->soft_cfm;
		pcontact->surface.motion1 = pst_ODE_B->motion1;
		pcontact->surface.motion2 = pst_ODE_B->motion2;
		pcontact->surface.slip1 = pst_ODE_B->slip1;
		pcontact->surface.slip2 = pst_ODE_B->slip2;


		contact_id = dJointCreateContact (pst_World->ode_id_world, pst_World->ode_joint_col_ode, pcontact);
		dJointAttach(contact_id, pst_A->pst_Base->pst_ODE->ode_id_body, pst_B->pst_Base->pst_ODE->ode_id_body);

		pst_World->ode_contacts_num_ode ++;
	}

	/* Create a "Fake" Collision report to allow IA to access ODE Collisions */
	if(numcol && (COL_gst_GlobalVars.ul_ReportIndex < COL_Cul_MaxNbOfCollision))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World	*pst_World;
		ULONG			ul_ReportIndex;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_ReportIndex = COL_gst_GlobalVars.ul_ReportIndex;
		pst_World = COL_gst_GlobalVars.pst_World;

		MATH_CopyVector(&pst_World->ast_Reports[ul_ReportIndex].st_CollidedPoint, (MATH_tdst_Vector *) pcontact->geom.pos);
		MATH_CopyVector(&pst_World->ast_Reports[ul_ReportIndex].st_Normal, (MATH_tdst_Vector *)  pcontact->geom.normal);
		MATH_MulVector(&pst_World->ast_Reports[ul_ReportIndex].st_Recal, (MATH_tdst_Vector *)  pcontact->geom.normal, pcontact->geom.depth);

		pst_World->ast_Reports[ul_ReportIndex].pst_A = pst_A;
		pst_World->ast_Reports[ul_ReportIndex].pst_B = pst_B;
		pst_World->ast_Reports[ul_ReportIndex].ul_Flag = COL_Cul_Wall | COL_Cul_Extra_ODE;
		pst_World->ast_Reports[ul_ReportIndex].ul_Triangle = (ULONG) pst_B->pst_Base->pst_ODE->uc_Sound;	/* Fake Triangle number to store the sound */
		pst_World->ast_Reports[ul_ReportIndex].uw_Element = 0;
		pst_World->ast_Reports[ul_ReportIndex].pst_GM = NULL;

		COL_gst_GlobalVars.ul_ReportIndex++;

		pst_World->ul_NbReports = COL_gst_GlobalVars.ul_ReportIndex;
	}
}

#endif

#if defined(__cplusplus)
}
#endif
