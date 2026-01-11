#include "Precomp.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "ode_jade.h"
#include "OBJects/OBJstruct.h"
#include "DYNamics/DYNstruct.h"
#include "DYNamics/DYNaccess.h"
#include "WORld/WORstruct.h"

#define STEP_DELAY  0.04f
#define STEP_QTY    10

BOOL	ODE_b_ActiveODE = FALSE;
#if defined(_XENON_RENDER) && defined(VIDEOCONSOLE_ENABLE)
    extern unsigned long NoODE;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ODE_ComputeODEMatrixFromJade(MATH_tdst_Matrix *_pst_Matrix, MATH_tdst_Matrix *_pst_ODEMatrix, float *af_ode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_Matrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_Mul33MatrixMatrix(&st_Matrix, _pst_ODEMatrix, _pst_Matrix, 1);

	af_ode[0] = st_Matrix.Ix;
	af_ode[1] = st_Matrix.Jx;
	af_ode[2] = st_Matrix.Kx;
	af_ode[3] = 0.0f;
	af_ode[4] = st_Matrix.Iy;
	af_ode[5] = st_Matrix.Jy;
	af_ode[6] = st_Matrix.Ky;
	af_ode[7] = 0.0f;
	af_ode[8] = st_Matrix.Iz;
	af_ode[9] = st_Matrix.Jz;
	af_ode[10] = st_Matrix.Kz;
	af_ode[11] = 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ODE_ComputeJadeMatrixFromODE(MATH_tdst_Matrix *_pst_Matrix, MATH_tdst_Matrix *_pst_ODEMatrix, float *af_ode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	st_Matrix, st_ODEMatrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CopyMatrix(&st_ODEMatrix, _pst_ODEMatrix);
	MATH_TranspEq33Matrix(&st_ODEMatrix);

	MATH_SetIdentityMatrix(&st_Matrix);
	MATH_SetRotationType(&st_Matrix);

	st_Matrix.Ix = af_ode[0];
	st_Matrix.Iy = af_ode[4];
	st_Matrix.Iz = af_ode[8];
	st_Matrix.Jx = af_ode[1];
	st_Matrix.Jy = af_ode[5];
	st_Matrix.Jz = af_ode[9];
	st_Matrix.Kx = af_ode[2];
	st_Matrix.Ky = af_ode[6];
	st_Matrix.Kz = af_ode[10];

	MATH_Mul33MatrixMatrix(_pst_Matrix, &st_ODEMatrix, &st_Matrix, 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ODE_CreateAndInitBody(DYN_tdst_ODE *pst_ODE, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	dMass				Mass;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ODE->ode_id_body = dBodyCreate(_pst_World->ode_id_world);
	pst_ODE->uc_Flags |=  ODE_FLAGS_ENABLE;

	switch(pst_ODE->uc_Type)
	{
	case ODE_TYPE_SPHERE:
		{
			dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
			dMassSetSphereTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X);
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		}
		break;

	case ODE_TYPE_BOX:
		{
			dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
			dMassSetBoxTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		}
		break;

	case ODE_TYPE_CYLINDER:
		{
			dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
			dMassSetCylinderTotal(&Mass, pst_ODE->mass_init, 2, pst_ODE->f_X, pst_ODE->f_Y);
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
		}
		break;
	}

	if(pst_ODE->uc_Flags & ODE_FLAGS_AUTODISABLE)
	{
		dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 1);
		dBodySetAutoDisableLinearThreshold(pst_ODE->ode_id_body, pst_ODE->f_LinearThres);
		dBodySetAutoDisableAngularThreshold(pst_ODE->ode_id_body, pst_ODE->f_AngularThres);
		dBodySetAutoDisableSteps(pst_ODE->ode_id_body, 10);
	}
	else 
	{
		dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ODE_MainCall(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject	*pst_GO;
	dGeomID				ode_id_geom;
	float				rot[12];
	MATH_tdst_Vector	st_Offset;
	MATH_tdst_Vector	st_Move;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	MATH_tdst_Vector	st_Speed;
	DYN_tdst_ODE		*pst_ODE;
	static float		fAccumulator = 0.0f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ODE_b_ActiveODE = FALSE;

    if (INO_b_RecordInput || INO_b_PlayInput)
        dRandSetSeed(INO_n_FrameCounter);

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ODE);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ODE);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)) continue;

		pst_ODE = pst_GO->pst_Base->pst_ODE;

		if(!(pst_ODE->uc_Flags & ODE_FLAGS_ENABLE) || !(pst_ODE->ode_id_geom)) continue;
#ifndef ACTIVE_EDITORS
		if(!pst_ODE->ode_id_body || !dBodyIsEnabled(pst_ODE->ode_id_body))
			continue;
#endif

#ifdef ACTIVE_EDITORS
		MATH_CheckVector(&pst_GO->pst_GlobalMatrix->T, "ODE NaN");
#endif


		ODE_b_ActiveODE = TRUE;
		ode_id_geom = pst_ODE->ode_id_geom;

		if
		(
			!(pst_ODE->ode_id_body)
		&&	!(pst_ODE->uc_Flags & ODE_FLAGS_FORCEIMMOVABLE)
		&&	(pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
		)
		{
			ODE_CreateAndInitBody(pst_ODE, _pst_World);
			dBodySetData(pst_ODE->ode_id_body, pst_GO);
		}


		MATH_TransformVector(&st_Offset, pst_GO->pst_GlobalMatrix, &pst_ODE->st_Offset);

		ODE_ComputeODEMatrixFromJade(pst_GO->pst_GlobalMatrix, &pst_ODE->st_RotMatrix,  rot); 

		/* Update radius real time */
		if(pst_ODE->ode_id_body && (pst_ODE->uc_Type == ODE_TYPE_SPHERE))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			dMass				Mass; 
			float				f_Radius;
			MATH_tdst_Vector	st_Scale;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			dBodyGetMass(pst_ODE->ode_id_body, &Mass);
			MATH_GetScale(&st_Scale, pst_GO->pst_GlobalMatrix);
			f_Radius = pst_ODE->f_X * fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
			dMassSetSphereTotal(&Mass, Mass.mass, f_Radius);
			dBodySetMass(pst_ODE->ode_id_body, &Mass);
			if(pst_ODE->ode_id_geom)
				dGeomSphereSetRadiusNoDirty(pst_ODE->ode_id_geom, f_Radius);
		}

		if((pst_ODE->uc_Flags & ODE_FLAGS_MATHCOLMAP) && (pst_ODE->uc_Type != ODE_TYPE_PLAN))
		{
			dGeomSetPositionNoDirty(ode_id_geom, pst_GO->pst_GlobalMatrix->T.x + st_Offset.x, pst_GO->pst_GlobalMatrix->T.y + st_Offset.y, pst_GO->pst_GlobalMatrix->T.z + st_Offset.z); 
			dGeomSetRotationNoDirty(ode_id_geom, &rot[0]);
		}


		/* Dyna Objects - Set ODE Speed - */
		if((pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) && pst_ODE->ode_id_body)
		{
			DYN_GetSpeedVector(pst_GO->pst_Base->pst_Dyna, &st_Speed);
			dBodySetLinearVel(pst_ODE->ode_id_body, st_Speed.x, st_Speed.y, st_Speed.z);
#ifdef ACTIVE_EDITORS
			MATH_CheckVector(&st_Speed, "ODE NaN");
#endif

		}
	}

	/* 0 Active ODE objects found ... return */
	if(!ODE_b_ActiveODE)
		return;

	/* Solver ODE utlime mais lente */
//	dWorldStep(_pst_World->ode_id_world, TIM_gf_dt);		/* DO NO LONGER WORK WITH UPDATES */

	/* Solver ODE itératif */
//	dWorldStepFast1(_pst_World->ode_id_world, TIM_gf_dt, 20);

	/* Solver ODE approximatif / rapide */
//	dWorldSetQuickStepNumIterations(_pst_World->ode_id_world, 20);

	// variable timestep method
/*
	dWorldSetQuickStepNumIterations(_pst_World->ode_id_world, 10);

	dWorldQuickStep(_pst_World->ode_id_world, TIM_gf_dt);
*/



	/*
	while( fAccumulator > STEP_DELAY )
	{
		dWorldSetQuickStepNumIterations(_pst_World->ode_id_world, STEP_QTY);
		dWorldQuickStep(_pst_World->ode_id_world, STEP_DELAY);
		fAccumulator -= STEP_DELAY;
	}
	*/

	// fixed timestep method
	fAccumulator = TIM_gf_dt;

	dWorldSetQuickStepNumIterations(_pst_World->ode_id_world, STEP_QTY);
  #ifdef VIDEOCONSOLE_ENABLE
    if (!NoODE)
  #endif
	dWorldQuickStep(_pst_World->ode_id_world, fAccumulator);

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ODE);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ODE);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)) continue;
		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Dyna) continue;

		pst_ODE = pst_GO->pst_Base->pst_ODE;

		if(!(pst_ODE->uc_Flags & ODE_FLAGS_ENABLE) || !(pst_ODE->ode_id_geom)) continue;
		if(!pst_ODE->ode_id_body || !dBodyIsEnabled(pst_ODE->ode_id_body))
			continue;

		if(pst_ODE->uc_Type == ODE_TYPE_PLAN) continue;

		ode_id_geom = pst_ODE->ode_id_geom;


		if(pst_ODE->uc_Flags & ODE_FLAGS_MATHCOLMAP)
		{
			/* Rotation */
			L_memcpy((char*)&rot[0], (char *) dGeomGetRotation(ode_id_geom), 12 * sizeof(float) );
			ODE_ComputeJadeMatrixFromODE(pst_GO->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot);

			/* Translation */
			MATH_TransformVector(&st_Offset, pst_GO->pst_GlobalMatrix, &pst_ODE->st_Offset);

			MATH_CopyVector(&st_Move, (MATH_tdst_Vector *) dGeomGetPosition(ode_id_geom)); 
#ifdef ACTIVE_EDITORS
			MATH_CheckVector(&st_Move, "ODE NaN");
#endif

			MATH_SubEqualVector(&st_Move, &st_Offset);

			MATH_SubEqualVector(&st_Move, &pst_GO->pst_GlobalMatrix->T);
			MATH_AddEqualVector(&pst_GO->pst_GlobalMatrix->T, &st_Move);


#ifdef ACTIVE_EDITORS
			MATH_CheckVector(&st_Move, "ODE NaN");
#endif

		}
	}
}

