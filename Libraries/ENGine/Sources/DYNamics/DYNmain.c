/*$T DYNmain.c GC! 1.081 07/26/02 14:33:15 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * Aim: This module countains the function called by the engine (DYN_MainCall),
 * and the main functions called by DYN_MainCall
 */
#include "Precomp.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "ENGine/Sources/DYNamics/DYNvars.h"
#include "ENGine/Sources/DYNamics/DYNBasic.h"
#include "ENGine/Sources/DYNamics/DYNconst.h"
#include "ENGine/Sources/DYNamics/DYNsolid.h"
#include "ENGine/Sources/DYNamics/DYNConstraint.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/BAStypes.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#include "TIMer/TIMdefs.h"

/*
 =======================================================================================================================
    Aim:    Main call for one gameobject

    Note:   This function calculates the speed (or more precisely the linear momentum which is m*speed) of one object
            by taking into acount the forces applied to him during f_Dt seconds
 =======================================================================================================================
 */
_inline_ void DYN_CalculateSpeed(DYN_tdst_Dyna *_pst_Dyna, float _f_Dt, ULONG _ul_DynFlags, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_P; /* Pointer to linear momentum (Speed*m) */
	MATH_tdst_Vector	st_T;	/* Sum of all constant forces */
	MATH_tdst_Vector	*pst_T;
	MATH_tdst_Vector	st_Tmp;
	MATH_tdst_Vector	st_A;

	float				f_KoverM;
	float				f_MoverK;
	float				f_k;

	MATH_tdst_Vector	*pst_k;

	BOOL				b_SkipFriction;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_P = &_pst_Dyna->st_P;

	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims) && _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0])
	{
		b_SkipFriction = ANI_b_TestPlayMode(_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0], ANI_C_AnimFlag_SpeedFromAnim);

		if(b_SkipFriction)
			b_SkipFriction = _ul_DynFlags & DYN_C_SkipFrictionWhenSpeedFromAnim;
	}
	else
		b_SkipFriction = FALSE;

	if(_ul_DynFlags & DYN_C_BasicForces)
	{
		pst_T = &st_T;

		/* Calculate the sum of all the constant forces */
		if(!(_ul_DynFlags & DYN_C_IgnoreStream))
			MATH_CopyVector(pst_T, DYN_pst_GetStream(_pst_Dyna));
		else
			MATH_CopyVector(pst_T, &MATH_gst_NulVector);

		if(!(_ul_DynFlags & DYN_C_IgnoreTraction)) MATH_AddEqualVector(pst_T, DYN_pst_GetTraction(_pst_Dyna));
		if(!(_ul_DynFlags & DYN_C_IgnoreGravity))
		{
			MATH_MulVector(&st_Tmp, DYN_pst_GetGravity(_pst_Dyna), DYN_f_GetMass(_pst_Dyna));
			MATH_AddEqualVector(pst_T, &st_Tmp);
		}

		/* We store it in the "sum of forces" of the Dyna structure */
		DYN_SetSumOfForces(_pst_Dyna, pst_T);

		/* Do we have a vector friction ? */
		if((_ul_DynFlags & DYN_C_VectorFriction) && !b_SkipFriction)
		{
			pst_k = DYN_pst_FrictionVectorGet(_pst_Dyna);

			if(MATH_b_NulVectorWithEpsilon(pst_k, Cf_EpsilonBig))
			{
				/* If no friction vector the solution has no exponential: dP=F*dt */
				MATH_MulVector(&st_Tmp, pst_T, _f_Dt);
				MATH_AddEqualVector(pst_P, &st_Tmp);
			}
			else
				DYN_CalculateSpeedWhenFrictionVector(_pst_Dyna, _f_Dt);
		}
		else
		{
			/* If no friction (k=0) the solution has no exponential: dP=F*dt */
			f_k = b_SkipFriction ? 0.0f : DYN_f_FrictionGet(_pst_Dyna);
			if(fNulWithEpsilon(f_k, Cf_EpsilonBig))
			{
				MATH_MulVector(&st_Tmp, pst_T, _f_Dt);
				MATH_AddEqualVector(pst_P, &st_Tmp);
				
			}
			else
			/* Resolve the differential equation using exact solution */
			{
				f_KoverM = fMul(f_k, DYN_f_GetInvMass(_pst_Dyna));
				f_MoverK = fInv(f_KoverM);
				MATH_MulVector(&st_A, pst_T, f_MoverK); /* A = (m/k)*T */
				MATH_SubVector(&st_Tmp, &st_A, pst_P);	/* Tmp = A-P0 */
				MATH_MulEqualVector(&st_Tmp, fExp(-_f_Dt * f_KoverM));
				MATH_SubVector(pst_P, &st_A, &st_Tmp);
			}
		}
	}
	else
	{
		if(_ul_DynFlags & DYN_C_OneConstantForce)
		{
			/*
			 * We have one constant force : £
			 * dP=m*dV £
			 * F=m*(dv/dt) £
			 * so, dP=m*(F*dt/m) = F*dt
			 */
			MATH_MulVector(&st_Tmp, &_pst_Dyna->st_F, _f_Dt);
			MATH_AddEqualVector(pst_P, &st_Tmp);
		}
		else
		{
			/* We have complex forces, to be done later... */
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Applies the calculated speed to the object in parameters
 =======================================================================================================================
 */
_inline_ void DYN_ApplySpeed
(
	OBJ_tdst_GameObject *_pst_Object,
	DYN_tdst_Dyna		*_pst_Dyna,
	float				_f_dt,
	ULONG				_ul_DynFlags,
	MATH_tdst_Vector	*_pst_OldSpeed
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_NewSpeed;
	MATH_tdst_Vector	st_DeltaDist;
	MATH_tdst_Vector	st_AvgSpeed;
	MATH_tdst_Vector	st_Temp;
	DYN_tdst_Constraint *pst_Constraint;
	MATH_tdst_Vector	st_OldPos;
	float				f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get old pos */
	MATH_CopyVector(&st_OldPos, _pst_Dyna->pst_X);

	/* Get New speed vector */
	DYN_GetSpeedVector(_pst_Dyna, &st_NewSpeed);

	/*
	 * This function replaces an euler integration by a true calculation of the
	 * position
	 */

	/* DYN_PositionAfterNseconds(_pst_Dyna,_pst_Dyna->pst_X,_f_dt); */

	/*
	 * Here, in the future, we will use the precision level to make a better
	 * integration
	 */

	/* for the moment, we use the average speed (Vavg=[v(t)+v(t+dt)]/2) */

	/* Dd=Vavg*Dt */
	
	if(_pst_OldSpeed)
	{
		MATH_AddVector(&st_AvgSpeed, &st_NewSpeed, _pst_OldSpeed);
		MATH_MulEqualVector(&st_AvgSpeed, 0.5f);
	}
	else
		MATH_CopyVector(&st_AvgSpeed, &st_NewSpeed);

	if(_ul_DynFlags & DYN_C_FuckMeca && _pst_Dyna->pst_Forces)
		MATH_CopyVector(&st_DeltaDist, &st_AvgSpeed);
	else
		MATH_MulVector(&st_DeltaDist, &st_AvgSpeed, _f_dt);

	/*
	 * Case of a constraint movement: we first project the speed vector on the
	 * tangent, than apply the speed and finally project on the curve
	 */
	if(_ul_DynFlags & DYN_C_Constraint)
	{
		/*
		 * at this point, the tangent can be null (reinit or first time). In this case, we
		 * only project the position on the curve
		 */
		pst_Constraint = DYN_pst_ConstraintGet(_pst_Dyna);

		if(!MATH_b_NulVectorWithEpsilon(&pst_Constraint->st_Tangent, Cf_EpsilonBig))
		{
			f = MATH_f_DotProduct(&pst_Constraint->st_Tangent, &st_DeltaDist);
			MATH_MulVector(&st_DeltaDist, &pst_Constraint->st_Tangent, f);
		}

		MATH_AddEqualVector(_pst_Dyna->pst_X, &st_DeltaDist);
		DYN_ApplyConstraint(_pst_Dyna, _f_dt);
	}
	else
	{
		MATH_AddEqualVector(_pst_Dyna->pst_X, &st_DeltaDist);
		if(_pst_Dyna->ul_DynFlags & DYN_C_ApplyMaxPos)
		{
			MATH_SubVector(&st_Temp, &_pst_Dyna->st_MaxPos, _pst_Dyna->pst_X);
			if(MATH_f_DotProduct(&st_Temp, &st_DeltaDist) < 0.0f)
			{
				/*
				 * f = MATH_f_NormVector(&st_Temp); MATH_NormalizeVector(&st_DeltaDist,
				 * &st_DeltaDist); MATH_ScaleVector(&st_DeltaDist, &st_DeltaDist, f);
				 * MATH_AddVector(_pst_Dyna->pst_X, &st_DeltaDist, &st_OldPos);
				 */
				MATH_SubVector(&st_DeltaDist, &_pst_Dyna->st_MaxPos, &st_OldPos);
				MATH_CopyVector(_pst_Dyna->pst_X, &_pst_Dyna->st_MaxPos);
			}
		}
	}

	if(OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Hierarchy) && _pst_Object->pst_Base && _pst_Object->pst_Base->pst_Hierarchy && _pst_Object->pst_Base->pst_Hierarchy->pst_Father)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix st_InvFatherMatrix ONLY_PSX2_ALIGNED(16);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_InvertMatrix(&st_InvFatherMatrix, OBJ_pst_GetAbsoluteMatrix(OBJ_pst_GetFather(_pst_Object)));
		MATH_TransformVector(&st_DeltaDist, &st_InvFatherMatrix, &st_DeltaDist);
	}

	ANI_UpdateFlashAfterMagicBoxTranslation(_pst_Object, &st_DeltaDist, ANI_C_UpdateAllAnims);
}

/*
 =======================================================================================================================
    Aim:    Change the orientation of an object according to the matrix
 =======================================================================================================================
 */
_inline_ void DYN_AutoOrient
(
	OBJ_tdst_GameObject *_pst_Object,
	DYN_tdst_Dyna		*_pst_Dyna,
	ULONG				_ul_DynFlags,
	float				_f_dt
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				l_InertiaLevel;
	MATH_tdst_Vector	st_Sight;
	MATH_tdst_Vector	st_SpeedDir;
	MATH_tdst_Vector	st_NewSight;
	MATH_tdst_Vector	st_UnitK;
	float				f_InertiaValue;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* If the speed is nul, we don't orient... */
	if(!MATH_b_NulVectorWithEpsilon(DYN_pst_GetP(_pst_Dyna), Cf_Inv100))
	{
		l_InertiaLevel = _ul_DynFlags & (DYN_C_AutoOrientInertia1 + DYN_C_AutoOrientInertia2);
		if(!l_InertiaLevel)
		{
			/* No inertia, we orient directly according to the speed */
			if(_ul_DynFlags & DYN_C_AutoOrientHorizontal)
			{
				/* horizontal orientation: we check that the speed is not vertical */
				MATH_NormalizeVector(&st_UnitK, DYN_pst_GetGravity(_pst_Dyna));
				MATH_NegEqualVector(&st_UnitK);

				if(!(MATH_b_VecColinear(&st_UnitK, DYN_pst_GetP(_pst_Dyna))))
					OBJ_BankingGeneralSet(_pst_Object, DYN_pst_GetP(_pst_Dyna), &st_UnitK);
				else
					/* We keep the current sight */
					OBJ_BankingGeneralSet(_pst_Object, &MATH_gst_NulVector, &st_UnitK);
			}
			else
				OBJ_SightSet(_pst_Object, DYN_pst_GetP(_pst_Dyna));
		}
		else
		{
			/* We orient smoothly according to the inertia level */
			MATH_NormalizeVector(&st_SpeedDir, DYN_pst_GetP(_pst_Dyna));
			OBJ_SightGet(_pst_Object, &st_Sight);
			if(l_InertiaLevel == DYN_C_AutoOrientInertiaLow)
				f_InertiaValue = DYN_gf_AutoOrientLowInertiaValue;
			else if(l_InertiaLevel == DYN_C_AutoOrientInertiaMedium)
				f_InertiaValue = DYN_gf_AutoOrientMediumInertiaValue;
			else
				f_InertiaValue = DYN_gf_AutoOrientHiInertiaValue;

			/*
			 * Note: we adjust the combination depending on the time spent since the last
			 * call: the normal formula would be "inertia^(_f_dt)", which can be changed into
			 * fExp(_f_dt*ln(inertia)), so, in fact, the constants of inertia store the
			 * logarithm of the real inertia value
			 */
			MATH_VectorCombine(&st_NewSight, &st_Sight, &st_SpeedDir, fExp(f_InertiaValue * _f_dt));

			if(_ul_DynFlags & DYN_C_AutoOrientHorizontal)
			{
				/* horizontal orientation: we check that the speed is not vertical */
				MATH_NormalizeVector(&st_UnitK, DYN_pst_GetGravity(_pst_Dyna));
				MATH_NegEqualVector(&st_UnitK);

				if(!(MATH_b_VecColinear(&st_UnitK, &st_NewSight)))
					OBJ_BankingGeneralSet(_pst_Object, &st_NewSight, &st_UnitK);
				else
					/* We keep the current sight */
					OBJ_BankingGeneralSet(_pst_Object, &MATH_gst_NulVector, &st_UnitK);
			}
			else
				OBJ_SightSet(_pst_Object, &st_NewSight);
		}
	}
}

extern float	TIM_gf_SpeedFactor;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DYN_OneCall(OBJ_tdst_GameObject *pst_GAO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	DYN_tdst_Dyna		*pst_Dyna;	/* Dynamic of the current game object */
	ULONG				ul_DynFlags;
	MATH_tdst_Vector	st_OldSpeed;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Hierarchy = OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Hierarchy);

	pst_Dyna = pst_GAO->pst_Base->pst_Dyna;
	if(!pst_GAO->pst_Base) return;
	if(!pst_Dyna) return;

	/* Is current Object unsensitive to Speed Factor. */
	if(pst_Dyna->ul_DynFlags & DYN_C_NoSpeedFactor) TIM_gf_dt /= TIM_gf_SpeedFactor;

	/* Take care of hierarchie */
	if(b_Hierarchy) OBJ_ComputeGlobalWhenHie(pst_GAO);

	ul_DynFlags = DYN_ul_GetDynFlags(pst_Dyna);

	/* store old speed */
	DYN_GetSpeedVector(pst_Dyna, &st_OldSpeed);

	/* Solve the forces system (and don't change the speed if no forces) */
	if(!(ul_DynFlags & DYN_C_NoForces))
	{
		/* If all forces ignored we don't calculate the speed */
		if(ul_DynFlags & DYN_C_FuckMeca && pst_Dyna->pst_Forces)
		{
			MATH_CopyVector(&pst_Dyna->st_P, &pst_Dyna->pst_Forces->st_Traction);
		}
		else
		{
			if(!((ul_DynFlags & DYN_C_IgnoreForces) == DYN_C_IgnoreForces))
				DYN_CalculateSpeed(pst_Dyna, TIM_gf_dt, ul_DynFlags, pst_GAO);
		}
	}

	/* Apply the "artificial" speed limit */
	if(ul_DynFlags & DYN_C_FuckMeca && pst_Dyna->pst_Forces)
	{
	}
	else
	{
		DYN_ApplySpeedLimit(pst_Dyna);
	}

	/* Apply the final speed, using the old and the new speed */
	if(ul_DynFlags & DYN_C_FuckMeca && pst_Dyna->pst_Forces)
		DYN_ApplySpeed(pst_GAO, pst_Dyna, TIM_gf_dt, ul_DynFlags, NULL);
	else
		DYN_ApplySpeed(pst_GAO, pst_Dyna, TIM_gf_dt, ul_DynFlags, &st_OldSpeed);

	/* Orient the object in the speed direction if the flag DYN_C_AutoOrient is Set */
	if(ul_DynFlags & DYN_C_AutoOrient) DYN_AutoOrient(pst_GAO, pst_Dyna, ul_DynFlags, TIM_gf_dt);

	/* Handle solid parameters */
	if(ul_DynFlags & DYN_C_Solid)
	{
		/* Calculate rotation Speed */
		DYN_CalcRotationSpeed(pst_Dyna, TIM_gf_dt);

		/* Apply the rotation Speed limit */
		DYN_ApplyRotationSpeedLimit(pst_Dyna);

		/* Apply the final rotation speed */
		DYN_ApplyAngularVelocity(pst_Dyna, TIM_gf_dt);
	}

#ifdef ACTIVE_EDITORS

	/*
	 * Refresh the display of the Speed (because the engine quantity is the linear
	 * momentum (P) and the Editor displayed value is the Speed)
	 */
	MATH_MulVector(&pst_Dyna->st_SpeedVector, DYN_pst_GetP(pst_Dyna), DYN_f_GetInvMass(pst_Dyna));
#endif
	/* Take care of hierarchie */
	if(b_Hierarchy) OBJ_ComputeLocalWhenHie(pst_GAO);
	
	
#if defined(_DEBUG) && defined(_GAMECUBE)
		if(
			((*(int *) &pst_GAO->pst_GlobalMatrix->T.x) == (int) 0xFFC00000)
		||  ((*(int *) &pst_GAO->pst_GlobalMatrix->T.y) == (int) 0xFFC00000)
		||  ((*(int *) &pst_GAO->pst_GlobalMatrix->T.z) == (int) 0xFFC00000)
		||	((*(int *) &pst_GAO->pst_GlobalMatrix->T.x) == (int) 0x7FC00000)
		||  ((*(int *) &pst_GAO->pst_GlobalMatrix->T.y) == (int) 0x7FC00000)
		||  ((*(int *) &pst_GAO->pst_GlobalMatrix->T.z) == (int) 0x7FC00000)
		
		)
		OSReport("Dynamic error \n");
#endif			
	
}

/*
 =======================================================================================================================
    Aim:    Main call of the dynamic
 =======================================================================================================================
 */
void DYN_MainCall(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;	/* Current Game object */
	BOOL				b_Father;
	TAB_tdst_PFtable	*pst_DynamEOT;
	float				f_SaveDt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	ERR_gpst_ContextGAO = NULL;
	ERR_gpsz_ContextString = "Dynamic";
#endif
	/* We loop thru the EOT of objects with dynamics */
	pst_DynamEOT = &_pst_World->st_EOT.st_Dyna;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_DynamEOT);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_DynamEOT);

	f_SaveDt = TIM_gf_dt;

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GAO)) continue;

#ifdef ACTIVE_EDITORS
		ERR_gpst_ContextGAO = pst_GAO;
#endif
		TIM_gf_dt = f_SaveDt;

		/*
		 * Teleportation Case: If we are in the First Frame, we update the Last Global
		 * Matrix to avoid collision incoherence.
		 */
		if
		(
			ENG_gb_FirstFrame
		&&	(OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
		&&	pst_GAO->pst_Extended
		&&	pst_GAO->pst_Extended->pst_Col
		&&	((COL_tdst_Base *) pst_GAO->pst_Extended->pst_Col)->pst_Instance
		)
		{
			if(pst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				MATH_CopyMatrix
				(
					((COL_tdst_Base *) pst_GAO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					&pst_GAO->pst_Base->pst_Hierarchy->st_LocalMatrix
				);
			}
			else
			{
				MATH_CopyMatrix
				(
					((COL_tdst_Base *) pst_GAO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
					pst_GAO->pst_GlobalMatrix
				);
			}
		}

		/* Hierarchy */
		if(OBJ_b_HasBeenProcess(pst_GAO, _pst_World->ul_ProcessCounterDyn, OBJ_C_ProcessedDyn)) continue;
		b_Father = FALSE;
		while
		(
			pst_GAO->pst_Base
		&&	pst_GAO->pst_Base->pst_Hierarchy
		&&	pst_GAO->pst_Base->pst_Hierarchy->pst_Father
		&&	!OBJ_b_HasBeenProcess
			(
				pst_GAO->pst_Base->pst_Hierarchy->pst_Father,
				_pst_World->ul_ProcessCounterDyn,
				OBJ_C_ProcessedDyn
			)
		)
		{
			pst_GAO = pst_GAO->pst_Base->pst_Hierarchy->pst_Father;
			b_Father = TRUE;
		}

		if(b_Father) pst_CurrentElem--;
		OBJ_SetProcess(pst_GAO, _pst_World->ul_ProcessCounterDyn, OBJ_C_ProcessedDyn);

		/*
		 * Dynamics can be cleared in real time in the AI, and the object be still in the
		 * EOT so we need to check the Dynamics pointer
		 */
		if
		(
			pst_GAO->pst_Base
		&&	pst_GAO->pst_Base->pst_Dyna
		&&	!(pst_GAO->pst_Base->pst_Dyna->ul_DynFlags & DYN_C_AfterEngineCall)
		&&	(!OBJ_b_TestControlFlag(pst_GAO, OBJ_C_ControlFlag_ForceInactive))
		&&	!(OBJ_b_TestControlFlag(pst_GAO, OBJ_C_ControlFlag_InPause))
//#ifdef ODE_INSIDE
//		&&  (!(OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_ODE)) || !(pst_GAO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_ENABLE))
//#endif
		)
		{
			DYN_OneCall(pst_GAO);
		}
	}

	/* For hierarchy */
	_pst_World->ul_ProcessCounterDyn++;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
