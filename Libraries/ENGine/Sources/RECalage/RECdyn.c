/*$T RECdyn.c GC! 1.081 07/30/02 16:34:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * Aim: This file handles the calculation of the impulse to give to the dynamics ,
 * after the collisions have occured
 */
#include "Precomp.h"

#include "OBJects/OBJstruct.h"
#include "OBJects/OBJconst.h"
#include "OBJects/OBJaccess.h"
#include "OBJects/OBJorient.h"

#include "ANImation/ANImain.h"

#include "WORld/WORstruct.h"
#include "COLlision/COLconst.h"
#include "COLlision/COLstruct.h"
#include "COLlision/COLreport.h"
#include "COLlision/COLvars.h"
#include "COLlision/COLaccess.h"
#include "DYNamics/DYNstruct.h"
#include "DYNamics/DYNaccess.h"
#include "DYNamics/DYNcol.h"
#include "DYNamics/DYNBasic.h"
#include "RECalage/RECconst.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
    Aim:    We calculate the normal that will be Used for the slide or the rebound

    Note:   _i_Mode must be set to DYN_C_SlideMode or DYN_C_ReboundMode
 =======================================================================================================================
 */
void REC_CalcNormalForImpulse
(
	DYN_tdst_Dyna		*_pst_Dyna,
	MATH_tdst_Vector	*_pst_OutNormal,
	MATH_tdst_Vector	*_pst_InNormal,
	MATH_tdst_Vector	*_pst_UnitGravity,
	int					_i_Mode
)
{
	/*~~~~~~~~~~*/
	float	c, c0;
	float	k;
	/*~~~~~~~~~~*/

	/* Project InNormal on the gravity vector */
	c = -MATH_f_DotProduct(_pst_InNormal, _pst_UnitGravity);
	if(_i_Mode == DYN_C_SlideMode)
		k = DYN_f_SlideHorizCosAngleGet(DYN_pst_ColGet(_pst_Dyna));
	else
		k = DYN_f_ReboundHorizCosAngleGet(DYN_pst_ColGet(_pst_Dyna));

	/* Make as if the normal is parallel to gravity if c>k */
	MATH_NegVector(_pst_OutNormal, _pst_UnitGravity);

	/* If c<k, we are in the case where the normal is not the gravity */
	if(c < k)
	{
		c0 = fHalf(3.0f * k - 1);

		/* C0 is always between 0 and k */
		if(c < c0)
		{
			/* We use the true normal */
			MATH_CopyVector(_pst_OutNormal, _pst_InNormal);
		}
		else
		{
			/* We blend the gravity normal with the true normal */
			MATH_BlendVector(_pst_OutNormal, _pst_InNormal, _pst_OutNormal, (c - c0) / (k - c0));
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Calculates the slide component of the impulse Vector
 =======================================================================================================================
 */
void DYN_CalcSlideVector
(
	MATH_tdst_Vector	*_pst_SlideVector,
	MATH_tdst_Vector	*_pst_UsedNormal,
	MATH_tdst_Vector	*_pst_SpeedVec,
	float				_f_Slide
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_OH;
	MATH_tdst_Vector	st_OH;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	f_OH = -MATH_f_DotProduct(_pst_UsedNormal, _pst_SpeedVec);
	MATH_ScaleVector(&st_OH, _pst_UsedNormal, f_OH);
	MATH_AddVector(_pst_SlideVector, &st_OH, _pst_SpeedVec);
	MATH_ScaleEqualVector(_pst_SlideVector, _f_Slide);
}

/*
 =======================================================================================================================
    Aim:    Calculates the rebound component of the impulse Vector
 =======================================================================================================================
 */
void DYN_CalcReboundVector
(
	MATH_tdst_Vector	*_pst_ReboundVector,
	MATH_tdst_Vector	*_pst_UsedNormal,
	MATH_tdst_Vector	*_pst_SpeedVec,
	float				_f_Rebound
)
{
	/*~~~~~~~~~*/
	float	f_OH;
	/*~~~~~~~~~*/

	f_OH = -MATH_f_DotProduct(_pst_UsedNormal, _pst_SpeedVec);
	MATH_ScaleVector(_pst_ReboundVector, _pst_UsedNormal, f_OH);
	MATH_ScaleEqualVector(_pst_ReboundVector, _f_Rebound);
}

/*
 =======================================================================================================================
    Aim:    Calculate the impulse (speed) after the collision contact
 =======================================================================================================================
 */
void REC_CalcImpulse
(
	OBJ_tdst_GameObject *_pst_MovObj,
	DYN_tdst_Dyna		*_pst_Dyna,
	MATH_tdst_Vector	*_pst_Impulse,
	MATH_tdst_Vector	*_pst_PseudoNormal,
	MATH_tdst_Vector	*_pst_NewDir,
	char				_c_Type,
	OBJ_tdst_GameObject *_pst_RecObj,
	BOOL				_b_Wall
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_SpeedVec;
	MATH_tdst_Vector	st_UsedSlideNormal;
	MATH_tdst_Vector	*pst_UsedSlideNormal;
	MATH_tdst_Vector	st_UsedReboundNormal;
	MATH_tdst_Vector	*pst_UsedReboundNormal;
	MATH_tdst_Vector	st_OH, st_AH, st_UnitGravity;
	float				f_Slide;
	float				f_Rebound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	DYN_GetSpeedVector(_pst_Dyna, &st_SpeedVec);

	/* We calculate the normals that will be Used for the rebound/slide */
	if(DYN_ul_TestDynFlags(_pst_Dyna, DYN_C_Col))
	{
		/* Gravity Vector can be (0,0,0). If so, we set it to (0,0,-1). Hmmmm .... boff */
		if(MATH_b_NulVector(DYN_pst_GetGravity(_pst_Dyna)))
			MATH_InitVector(&st_UnitGravity, 0.0f, 0.0f, -1.0f);
		else
			MATH_NormalizeVector(&st_UnitGravity, DYN_pst_GetGravity(_pst_Dyna));
		REC_CalcNormalForImpulse(_pst_Dyna, &st_UsedSlideNormal, _pst_PseudoNormal, &st_UnitGravity, DYN_C_ReboundMode);
		REC_CalcNormalForImpulse
		(
			_pst_Dyna,
			&st_UsedReboundNormal,
			_pst_PseudoNormal,
			&st_UnitGravity,
			DYN_C_ReboundMode
		);
		pst_UsedSlideNormal = &st_UsedSlideNormal;
		pst_UsedReboundNormal = &st_UsedReboundNormal;
		f_Slide = DYN_f_SlideGet(DYN_pst_ColGet(_pst_Dyna));
		f_Rebound = DYN_f_ReboundGet(DYN_pst_ColGet(_pst_Dyna));
	}
	else
	{
		pst_UsedSlideNormal = _pst_PseudoNormal;
		pst_UsedReboundNormal = _pst_PseudoNormal;
		f_Slide = Cf_One;
		f_Rebound = Cf_Zero;
	}

	/* If speed is null, do not change anything */
	if(!MATH_b_NulVectorWithEpsilon(&st_SpeedVec, Cf_Epsilon))
	{
		/* Case of plan: we just use the Normal */
		if(_c_Type == REC_C_Plan)
		{
			/*
			 * Let O be the contact point, AO the speed vector and, H the projection of A on
			 * the pseudo-normal. We calculate AH and OH
			 */
			DYN_CalcSlideVector(&st_AH, pst_UsedSlideNormal, &st_SpeedVec, f_Slide);

			if(!_b_Wall && (_pst_Dyna->ul_DynFlags & DYN_C_NoReboundOnGround))
				MATH_InitVector(&st_OH, 0.0f, 0.0f, 0.0f);
			else
				DYN_CalcReboundVector(&st_OH, pst_UsedReboundNormal, &st_SpeedVec, f_Rebound);

			/* If rebound is enough significant we apply it */
			if(MATH_f_NormVector(&st_OH) > DYN_C_MinRebound)
			{
				MATH_AddVector(_pst_Impulse, &st_OH, &st_AH);
			}
			else
				MATH_CopyVector(_pst_Impulse, &st_AH);
		}
		else
			_pst_Impulse->z = 0;
	}
	else
		MATH_SetNulVector(_pst_Impulse);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void REC_Apply
(
	OBJ_tdst_GameObject *pst_MovObj,
	OBJ_tdst_GameObject *pst_RecObj,
	MATH_tdst_Vector	*pst_Normal,
	BOOL				_b_Wall
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna		*pst_Dyna;
	MATH_tdst_Vector	st_PseudoNormal;
	char				c_ImpulseType;
	MATH_tdst_Vector	st_NewDir;
	MATH_tdst_Vector	st_Impulse;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* If(!pst_RecObj) continue; */
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_MovObj);
	if((pst_MovObj->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) && pst_Dyna)
	{

#ifdef ACTIVE_EDITORS
	MATH_CheckVector(pst_Normal, "Dyn NOT-REC normal");
#endif


		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    We calculate the impulse and Apply it
		 ---------------------------------------------------------------------------------------------------------------
		 */

		/* Temporary, we calculate all impulses as if we were on plans */
		c_ImpulseType = REC_C_Plan;

		MATH_CopyVector(&st_PseudoNormal, pst_Normal);

		/* Horizontal Walls */
		if(_b_Wall)
		{
			st_PseudoNormal.z = 0.0f;
			MATH_NormalizeEqualVector(&st_PseudoNormal);
		}

		MATH_CopyVector(&st_NewDir, &MATH_gst_NulVector);

		REC_CalcImpulse
		(
			pst_MovObj,
			pst_Dyna,
			&st_Impulse,
			&st_PseudoNormal,
			&st_NewDir,
			c_ImpulseType,
			pst_RecObj,
			_b_Wall
		);

		/* Sets the new impulse */
		DYN_SetSpeedVector(pst_Dyna, &st_Impulse);
		DYN_ApplySpeedLimit(pst_Dyna);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void REC_ApplyDynObj
(
	MATH_tdst_Vector	*_pst_Speed1,
	MATH_tdst_Vector	*_pst_Speed2,
	MATH_tdst_Vector	*_pst_NewSpeed1,
	MATH_tdst_Vector	*_pst_NewSpeed2,
	MATH_tdst_Vector	*_pst_Normal,
	BOOL				_b_Wall
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Y1, st_Y2;
	MATH_tdst_Vector	st_X1_Init, st_X2_Init;
	MATH_tdst_Vector	st_X1, st_X2;
	MATH_tdst_Vector	st_Normal;
	float				dot1, dot2;
	float				V1, V2;
	float				Inertia1, Inertia2, Bound1, Bound2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_NegVector(&st_Normal, _pst_Normal);

	V1 = MATH_f_NormVector(_pst_Speed1);
	V2 = MATH_f_NormVector(_pst_Speed2);

#ifdef ACTIVE_EDITORS
	MATH_CheckVector(_pst_Speed1, "Dyn REC speed 1");
	MATH_CheckVector(_pst_Speed2, "Dyn REC speed 2");
	MATH_CheckVector(_pst_Normal, "Dyn REC normal");
#endif


	dot1 = MATH_f_DotProduct(_pst_Speed1, &st_Normal);
	dot2 = MATH_f_DotProduct(_pst_Speed2, &st_Normal);

	MATH_ScaleVector(&st_X1_Init, &st_Normal, dot1);
	MATH_ScaleVector(&st_X2_Init, &st_Normal, dot2);

	MATH_SubVector(&st_Y1, _pst_Speed1, &st_X1_Init);
	MATH_SubVector(&st_Y2, _pst_Speed2, &st_X2_Init);

	if(V1 > 5.0f * V2)
	{
		Inertia1 = 0.8f;
		Bound1 = 0.2f;

		Inertia2 = 0.1f;
		Bound2 = 1.5f;
	}
	else
	{
		if(V2 > 5.0f * V1)
		{
			Inertia2 = 0.8f;
			Bound2 = 0.2f;

			Inertia1 = 0.1f;
			Bound1 = 1.5;
		}
		else
		{
			Inertia1 = 0.2f;
			Bound1 = 0.8f;

			Inertia2 = 0.2f;
			Bound2 = 0.8f;
		}
	}

	MATH_ScaleVector(&st_X1, &st_X1_Init, Inertia1);
	MATH_AddScaleVector(&st_X1, &st_X1, &st_X2_Init, Bound1);

	MATH_ScaleVector(&st_X2, &st_X2_Init, Inertia2);
	MATH_AddScaleVector(&st_X2, &st_X2, &st_X1_Init, Bound2);

	if((dot1 * dot2) > 0.0f)
	{
		if(dot1 > dot2)
		{
			MATH_AddVector(_pst_NewSpeed1, &st_Y1, &st_X1);
			MATH_AddVector(_pst_NewSpeed2, &st_Y2, &st_X2);
		}
		else
		{
			MATH_CopyVector(_pst_NewSpeed1, _pst_Speed1);
			MATH_CopyVector(_pst_NewSpeed2, _pst_Speed2);
		}
	}
	else
	{
		MATH_AddVector(_pst_NewSpeed1, &st_Y1, &st_X1);
		MATH_AddVector(_pst_NewSpeed2, &st_Y2, &st_X2);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void REC_ApplyDyn
(
	OBJ_tdst_GameObject *_pst_Obj1,
	OBJ_tdst_GameObject *_pst_Obj2,
	MATH_tdst_Vector	*_pst_Normal,
	BOOL				_b_Wall
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna		*pst_Dyna1;
	DYN_tdst_Dyna		*pst_Dyna2;
	MATH_tdst_Vector	st_Speed1;
	MATH_tdst_Vector	st_Speed2;
	MATH_tdst_Vector	st_NewSpeed1;
	MATH_tdst_Vector	st_NewSpeed2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna1 = OBJ_pst_GetDynaStruct(_pst_Obj1);
	pst_Dyna2 = OBJ_pst_GetDynaStruct(_pst_Obj2);

	DYN_GetSpeedVector(pst_Dyna1, &st_Speed1);
	DYN_GetSpeedVector(pst_Dyna2, &st_Speed2);

	REC_ApplyDynObj(&st_Speed1, &st_Speed2, &st_NewSpeed1, &st_NewSpeed2, _pst_Normal, _b_Wall);

	DYN_SetSpeedVector(pst_Dyna1, &st_NewSpeed1);
	DYN_ApplySpeedLimit(pst_Dyna1);

	DYN_SetSpeedVector(pst_Dyna2, &st_NewSpeed2);
	DYN_ApplySpeedLimit(pst_Dyna2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void REC_OrderReports(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report		*pst_CurrentReport, *pst_Report, *pst_ChangeReport, *pst_LastReport;
	COL_tdst_Report		st_TempReport;
	DYN_tdst_Dyna		*pst_Dyna;
	MATH_tdst_Vector	st_Speed;
	OBJ_tdst_GameObject *pst_A;
	float				f_Dot, f_Dot2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentReport = _pst_World->ast_Reports;
	pst_LastReport = pst_CurrentReport + _pst_World->ul_NbReports;

	for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
	{
		if(COL_b_Report_TestFlag(pst_CurrentReport, COL_Cul_Invalid | COL_Cul_Inactive | COL_Cul_Crossable)) continue;

		pst_A = pst_CurrentReport->pst_A;
		pst_Dyna = OBJ_pst_GetDynaStruct(pst_A);
		if(!pst_Dyna) continue;

		DYN_GetSpeedVector(pst_Dyna, &st_Speed);
		f_Dot = MATH_f_DotProduct(&st_Speed, &pst_CurrentReport->st_Normal);
		f_Dot -= 1E-3f;

		pst_ChangeReport = NULL;
		for(pst_Report = pst_CurrentReport + 1; pst_Report < pst_LastReport; pst_Report++)
		{
			if(COL_b_Report_TestFlag(pst_Report, COL_Cul_Invalid | COL_Cul_Inactive | COL_Cul_Crossable)) continue;

			if(pst_Report->pst_A != pst_A) continue;

			f_Dot2 = MATH_f_DotProduct(&st_Speed, &pst_Report->st_Normal);

			if(f_Dot2 < f_Dot)
			{
				pst_ChangeReport = pst_Report;
				f_Dot = f_Dot2 - 1E-3f;
			}
		}

		if(pst_ChangeReport)
		{
			L_memcpy(&st_TempReport, pst_CurrentReport, sizeof(COL_tdst_Report));
			L_memcpy(pst_CurrentReport, pst_ChangeReport, sizeof(COL_tdst_Report));
			L_memcpy(pst_ChangeReport, &st_TempReport, sizeof(COL_tdst_Report));
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void REC_PhysicalCall(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_World *_pst_World, ULONG _ul_FirstReport)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Box		*pst_Box;
	COL_tdst_Report		*pst_CurrentReport, *pst_LastReport;
	DYN_tdst_Dyna		*pst_Dyna;
	MATH_tdst_Vector	st_ACS_hit, st_ACS_Barycentre, st_ACS_Normal;
	MATH_tdst_Vector	st_ACS_DeltaAngularVelocity, st_ACS_BrasDeLevier;
	MATH_tdst_Vector	st_ACS_Speed, st_ACS_Cross, st_ACS_Cross2;
	MATH_tdst_Vector	st_ACS_Impulse, st_Global_Impulse;
	MATH_tdst_Vector	st_ACS_AngularVelocity, st_ACS_NormalG;
	MATH_tdst_Vector	st_ACS_LinearVelocity, st_Global_LinearVelocity;
/*	MATH_tdst_Vector	st_P_Save, st_Tmp;*/
	MATH_tdst_Matrix	st_InvertAMatrix;
	float				f_Num, f_Den, f_Impulse, f_GreatestImpulse;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_GreatestImpulse = -100000.0f;

	pst_Dyna = _pst_GO->pst_Base->pst_Dyna;
	if(!pst_Dyna || !pst_Dyna->pst_Solid) return;

	/* Data needed and misc. init */
	MATH_InvertMatrix(&st_InvertAMatrix, _pst_GO->pst_GlobalMatrix);
	MATH_InitVector(&pst_Dyna->pst_Solid->st_T, 0.0f, 0.0f, 0.0f);

	/*
	MATH_CopyVector(&st_P_Save, &pst_Dyna->st_P);
	MATH_MulVector(&st_Tmp, &pst_Dyna->st_F, TIM_gf_dt);
	MATH_SubEqualVector(&pst_Dyna->st_P, &st_Tmp);
	MATH_MulVector(&st_Tmp, &pst_Dyna->st_F, 1.0f / 60.0f);
	MATH_AddEqualVector(&pst_Dyna->st_P, &st_Tmp);
	*/

	/* Fake Angular Friction to reduce Angular speed per frame */
	pst_Dyna->pst_Solid->st_w.x *= 0.95f ;
	pst_Dyna->pst_Solid->st_w.y *= 0.95f;
	pst_Dyna->pst_Solid->st_w.z *= 0.95f;

	/* Linear Velocity in ACS */
	DYN_GetSpeedVector(pst_Dyna, &st_Global_LinearVelocity);
	MATH_TransformVector(&st_ACS_LinearVelocity, &st_InvertAMatrix, &st_Global_LinearVelocity);

	/* Barycentre of Box (Mass Center) */
	pst_Box = (COL_tdst_Box *) (((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->dpst_ZDx[0]->p_Shape);
	MATH_AddVector(&st_ACS_Barycentre, &pst_Box->st_Min, &pst_Box->st_Max);
	MATH_ScaleEqualVector(&st_ACS_Barycentre, 0.5f);

	pst_CurrentReport = _pst_World->ast_Reports + 0;
	pst_LastReport = _pst_World->ast_Reports + _pst_World->ul_NbReports;
	for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
	{
		/* Handle only valid reports */
		if
		(
			COL_b_Report_TestFlag
			(
				pst_CurrentReport,
				COL_Cul_Inactive | COL_Cul_Invalid | COL_Cul_Crossable | COL_Cul_SlipperyEdge |COL_Cul_Extra_Minor
			)
		) continue;

		if(pst_CurrentReport->pst_A != _pst_GO) continue;

		if((pst_CurrentReport->st_Recal.x == 0.0f) && (pst_CurrentReport->st_Recal.y == 0.0f) && (pst_CurrentReport->st_Recal.z == 0.0f)) 
			continue;

		/* Collided Point and Normal in ACS */
		MATH_TransformVertex(&st_ACS_hit, &st_InvertAMatrix, &pst_CurrentReport->st_CollidedPoint);
		MATH_TransformVector(&st_ACS_Normal, &st_InvertAMatrix, &pst_CurrentReport->st_Normal);

		/* "Bras de Levier" in ACS */
		MATH_SubVector(&st_ACS_BrasDeLevier, &st_ACS_hit, &st_ACS_Barycentre);

		/* Angular Velocity */
		MATH_CrossProduct(&st_ACS_AngularVelocity, &pst_Dyna->pst_Solid->st_w, &st_ACS_BrasDeLevier);

		/* Total Velocity */
		MATH_AddVector(&st_ACS_Speed, &st_ACS_AngularVelocity, &st_ACS_LinearVelocity);

		f_Num = -(1.0f + pst_Dyna->pst_Solid->f_Factor) * MATH_f_DotProduct(&st_ACS_Speed, &st_ACS_Normal);

		MATH_CrossProduct(&st_ACS_Cross, &st_ACS_BrasDeLevier, &st_ACS_Normal);
		MATH_ScaleEqualVector(&st_ACS_Cross, DYN_f_GetInvMass(pst_Dyna)); 
		MATH_CrossProduct(&st_ACS_Cross2, &st_ACS_Cross, &st_ACS_BrasDeLevier);

		f_Den = DYN_f_GetInvMass(pst_Dyna) + MATH_f_DotProduct(&st_ACS_Normal, &st_ACS_Cross2);

		f_Impulse = f_Num / f_Den;

		if(f_Impulse > f_GreatestImpulse)
		{
			f_GreatestImpulse = f_Impulse;
			MATH_CopyVector(&st_ACS_NormalG, &st_ACS_Normal);
		}

		/* Impulse */
		MATH_MulVector(&st_ACS_Impulse, &st_ACS_Normal, f_Impulse);

		/* Update in Angular Velocity */
		MATH_CrossProduct(&st_ACS_DeltaAngularVelocity, &st_ACS_BrasDeLevier, &st_ACS_Impulse);
		MATH_MulEqualVector(&st_ACS_DeltaAngularVelocity, DYN_f_GetInvMass(pst_Dyna));
		MATH_AddEqualVector(&pst_Dyna->pst_Solid->st_w, &st_ACS_DeltaAngularVelocity);

		/* Update in Linear Momentum */
		MATH_TransformVector(&st_Global_Impulse, _pst_GO->pst_GlobalMatrix, &st_ACS_Impulse);
		MATH_AddEqualVector(&pst_Dyna->st_P, &st_Global_Impulse);
#if defined(_DEBUG) && defined(_GAMECUBE)
		if(
			((*(int *) &pst_Dyna->st_P.x) == (int) 0xFFC00000)
		||  ((*(int *) &pst_Dyna->st_P.y) == (int) 0xFFC00000)
		||  ((*(int *) &pst_Dyna->st_P.z) == (int) 0xFFC00000)
		||	((*(int *) &pst_Dyna->st_P.x) == (int) 0x7FC00000)
		||  ((*(int *) &pst_Dyna->st_P.y) == (int) 0x7FC00000)
		||  ((*(int *) &pst_Dyna->st_P.z) == (int) 0x7FC00000)		
		)
		OSReport("Dynamic error \n");
#endif						
		

	}

	
	if(0 && (f_GreatestImpulse != -100000.0f))
	{
		MATH_MulVector(&st_ACS_Impulse, &st_ACS_NormalG, f_GreatestImpulse);
		
		/* Update in Linear Momentum */
		MATH_TransformVector(&st_Global_Impulse, _pst_GO->pst_GlobalMatrix, &st_ACS_Impulse);
		MATH_AddEqualVector(&pst_Dyna->st_P, &st_Global_Impulse);
#if defined(_DEBUG) && defined(_GAMECUBE)
		if(
			((*(int *) &pst_Dyna->st_P.x) == (int) 0xFFC00000)
		||  ((*(int *) &pst_Dyna->st_P.y) == (int) 0xFFC00000)
		||  ((*(int *) &pst_Dyna->st_P.z) == (int) 0xFFC00000)
		||	((*(int *) &pst_Dyna->st_P.x) == (int) 0x7FC00000)
		||  ((*(int *) &pst_Dyna->st_P.y) == (int) 0x7FC00000)
		||  ((*(int *) &pst_Dyna->st_P.z) == (int) 0x7FC00000)		
		)
		OSReport("Dynamic error \n");
#endif						
		
	}

	/* Copy Position in the OldPos solid structure */
	MATH_CopyVector(&pst_Dyna->pst_Solid->st_OldPosition, &_pst_GO->pst_GlobalMatrix->T);
}

/*
 =======================================================================================================================
    Aim:    Main call of the REC module

    Note:   Loops thru the collision report of the world to calculate each impulse. DO NOT MOVE THE OBJECTS. JUST
            UPDATE THE SPEED VECTORS.
 =======================================================================================================================
 */
void REC_MainCall(WOR_tdst_World *_pst_World, ULONG _ul_FirstReport)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Report		*pst_CurrentReport, *pst_LastReport;
	COL_tdst_Report		*pst_CurrentReport1;
	OBJ_tdst_GameObject *pst_MovObj;
	OBJ_tdst_GameObject *pst_RecObj;
	DYN_tdst_Dyna		*pst_DynaMov;
	DYN_tdst_Dyna		*pst_DynaRec;
	MATH_tdst_Vector	st_SpeedMov;
	MATH_tdst_Vector	st_Normal;
	char				c_Recom;
	BOOL				b_SameColType;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	REC_OrderReports(_pst_World);

	pst_CurrentReport = _pst_World->ast_Reports + _ul_FirstReport;
	pst_LastReport = _pst_World->ast_Reports + _pst_World->ul_NbReports;

	for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
	{
		b_SameColType = FALSE;

		/* Handle only valid reports */
		if
		(
			COL_b_Report_TestFlag
			(
				pst_CurrentReport,
				COL_Cul_Invalid | COL_Cul_Inactive | COL_Cul_Crossable | COL_Cul_Extra_SlipperyEdge | COL_Cul_Extra_Minor
			)
		) continue;

#ifdef ODE_INSIDE
		if(COL_b_Report_TestFlag(pst_CurrentReport, COL_Cul_Extra_ODE))
			continue;
#endif

		if(COL_b_Report_TestFlag(pst_CurrentReport, COL_Cul_SlipperyEdge) && COL_b_Report_TestFlag(pst_CurrentReport, COL_Cul_Ground))
			continue;

		pst_MovObj = pst_CurrentReport->pst_A;
		pst_RecObj = pst_CurrentReport->pst_B;

#ifdef ODE_INSIDE2
			if((pst_MovObj->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && !(pst_MovObj->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna))
				continue;
#endif


		/* ZDM Box */
		/*
		if(pst_CurrentReport->ul_Flag & COL_Cul_ZDMBox)
		{
			REC_PhysicalCall(pst_MovObj, _pst_World, pst_CurrentReport - _pst_World->ast_Reports);
			continue;
		}
		*/

#ifdef ACTIVE_EDITORS
		ERR_gpst_ContextGAO = pst_MovObj;
		ERR_gpsz_ContextString = "Recalage";
#endif
		/* Search for a "dual" report that will cancel process of current one */
		c_Recom = 0;
		for(pst_CurrentReport1 = pst_CurrentReport + 1; pst_CurrentReport1 < pst_LastReport; pst_CurrentReport1++)
		{
			if
			(
				COL_b_Report_TestFlag
				(
					pst_CurrentReport1,
					COL_Cul_Inactive | COL_Cul_Crossable | COL_Cul_Extra_SlipperyEdge | COL_Cul_Extra_Minor
				)
			) continue;

#ifdef ODE_INSIDE
			if(COL_b_Report_TestFlag(pst_CurrentReport1, COL_Cul_Extra_ODE))
				continue;
#endif
	
			if(COL_b_Report_TestFlag(pst_CurrentReport1, COL_Cul_SlipperyEdge) && COL_b_Report_TestFlag(pst_CurrentReport1, COL_Cul_Ground))
				continue;



			b_SameColType = ((pst_CurrentReport->ul_Flag & COL_Cul_Wall) && (pst_CurrentReport1->ul_Flag & COL_Cul_Wall))
			||	((pst_CurrentReport->ul_Flag & COL_Cul_Ground) && (pst_CurrentReport1->ul_Flag & COL_Cul_Ground));

			if
			(
				b_SameColType
			&&	(
					((pst_CurrentReport1->pst_A == pst_MovObj) && (pst_CurrentReport1->pst_B == pst_RecObj))
				||	((pst_CurrentReport1->pst_A == pst_RecObj) && (pst_CurrentReport1->pst_B == pst_MovObj))
				)
			)
			{
				c_Recom = 1;
				break;
			}
		}

		if(c_Recom) continue;



		/* Speed moving object */
		pst_DynaMov = OBJ_pst_GetDynaStruct(pst_MovObj);
		if(!pst_DynaMov) continue;

		if(pst_RecObj && pst_RecObj->pst_Base)
		{
			pst_DynaRec = OBJ_pst_GetDynaStruct(pst_RecObj);
			if
			(
				pst_DynaRec
			&&	DYN_ul_TestDynFlags(pst_DynaRec, DYN_C_ApplyRec)
			&&	DYN_ul_TestDynFlags(pst_DynaMov, DYN_C_ApplyRec)
			&&	!(pst_RecObj->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			&&	!(pst_MovObj->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
#ifdef ODE_INSIDE2
			&&	!(pst_MovObj->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
			&&	!(pst_MovObj->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
			&&	!(pst_RecObj->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
			&&	!(pst_RecObj->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna)
#endif
			)
			{
				MATH_CopyVector(&st_Normal, &pst_CurrentReport->st_Normal);
				REC_ApplyDyn(pst_MovObj, pst_RecObj, &st_Normal, (pst_CurrentReport->ul_Flag & COL_Cul_Wall));
				continue;
			}
		}


		DYN_GetSpeedVector(pst_DynaMov, &st_SpeedMov);

		if((pst_CurrentReport->ul_Flag & COL_Cul_Ground) && (pst_DynaMov->ul_DynFlags & DYN_C_HorizontalGrounds))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_PseudoNormal;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(pst_DynaMov->pst_Forces && !MATH_b_NulVector(&pst_DynaMov->pst_Forces->st_Gravity))
				MATH_NegVector(&st_PseudoNormal, &pst_DynaMov->pst_Forces->st_Gravity);
			else
				MATH_InitVector(&st_PseudoNormal, 0.0f, 0.0f, 1.0f);
			MATH_NormalizeEqualVector(&st_PseudoNormal);
			REC_Apply(pst_MovObj, pst_RecObj, &st_PseudoNormal, (pst_CurrentReport->ul_Flag & COL_Cul_Wall));
		}
		else
		{
			REC_Apply
			(
				pst_MovObj,
				pst_RecObj,
				&(pst_CurrentReport->st_Normal),
				(pst_CurrentReport->ul_Flag & COL_Cul_Wall)
			);
		}
	}
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
