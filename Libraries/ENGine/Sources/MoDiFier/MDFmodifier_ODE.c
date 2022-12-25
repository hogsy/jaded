#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_ODE.h"
#include "WORld/WORstruct.h"
#include "WORld/WORaccess.h"
#include "GEOmetric/GEOobject.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "ENGine/Sources/OBJects/OBJload.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif


#ifdef ODE_INSIDE
#include "ode_jade.h"
#endif


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierODE_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
#ifdef ODE_INSIDE
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierODE));

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	if(!p_Data)
	{
		((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->uc_Version = 2;
		((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->uc_Type = ODE_JointTypeBall;
		((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->pst_GO1 = NULL;
		((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->pst_GO2 = NULL;
		((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->i_ode_joint_id = 0;

		((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->f_LoLimit = -Cf_Pi;
		((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->f_HiLimit = +Cf_Pi;
		((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->f_BounceStop = 0.0f;
		((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->f_Friction = 0.0f;

	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierODE));
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierODE_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
#ifdef ODE_INSIDE
	/* We do not destroy the joint because we would have to destroy/empty the entire joint group */

	if(_pst_Mod->p_Data) 
	{
		if(((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->i_ode_joint_id)
			dJointAttach(((GAO_tdst_ModifierODE *) _pst_Mod->p_Data)->i_ode_joint_id, 0, 0);

		MEM_Free(_pst_Mod->p_Data);
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierODE_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierODE *pst_Mod_ODE;
	OBJ_tdst_GameObject	*pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Mod->p_Data || !_pst_Mod->pst_GO) return;

	pst_GO = _pst_Mod->pst_GO;
	pst_Mod_ODE = (GAO_tdst_ModifierODE *)(_pst_Mod->p_Data);

	if(!pst_Mod_ODE->i_ode_joint_id) return;

	if
	(
		(
			pst_Mod_ODE->pst_GO1
		&&	OBJ_b_TestIdentityFlag(pst_Mod_ODE->pst_GO1, OBJ_C_IdentityFlag_ODE) 
		&&	pst_Mod_ODE->pst_GO1->pst_Base->pst_ODE->ode_id_body
//		&&	dBodyIsEnabled(pst_Mod_ODE->pst_GO1->pst_Base->pst_ODE->ode_id_body)
		)
	||
		(
			pst_Mod_ODE->pst_GO2
		&&	OBJ_b_TestIdentityFlag(pst_Mod_ODE->pst_GO2, OBJ_C_IdentityFlag_ODE) 
		&&	pst_Mod_ODE->pst_GO2->pst_Base->pst_ODE->ode_id_body
//		&&	dBodyIsEnabled(pst_Mod_ODE->pst_GO2->pst_Base->pst_ODE->ode_id_body)
		)
	)
	{
		switch(pst_Mod_ODE->uc_Type)
		{
		case ODE_JointTypeBall:
			{
//				dJointSetBallAnchor(pst_Mod_ODE->i_ode_joint_id, _pst_Mod->pst_GO->pst_GlobalMatrix->T.x , _pst_Mod->pst_GO->pst_GlobalMatrix->T.y, _pst_Mod->pst_GO->pst_GlobalMatrix->T.z);
				break;
			}

		case ODE_JointTypeHinge:
			{
				/*
				dJointSetHingeAnchor(pst_Mod_ODE->i_ode_joint_id, _pst_Mod->pst_GO->pst_GlobalMatrix->T.x, _pst_Mod->pst_GO->pst_GlobalMatrix->T.y, _pst_Mod->pst_GO->pst_GlobalMatrix->T.z);
				dJointSetHingeAxis(pst_Mod_ODE->i_ode_joint_id, _pst_Mod->pst_GO->pst_GlobalMatrix->Kx, _pst_Mod->pst_GO->pst_GlobalMatrix->Ky, _pst_Mod->pst_GO->pst_GlobalMatrix->Kz);

				dJointSetHingeParam(pst_Mod_ODE->i_ode_joint_id,dParamLoStop, -3.14159f / 2.0f);
				dJointSetHingeParam(pst_Mod_ODE->i_ode_joint_id,dParamHiStop, 3.14159f / 2.0f);
				*/
				break;
			}
		}
	}

#ifdef ACTIVE_EDITORS
	pst_Mod_ODE->pst_GO = _pst_Mod->pst_GO;
#endif

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierODE_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierODE *pst_Mod_ODE;
	OBJ_tdst_GameObject	*pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Mod->p_Data || !_pst_Mod->pst_GO) return;

	pst_GO = _pst_Mod->pst_GO;
	pst_Mod_ODE = (GAO_tdst_ModifierODE *)(_pst_Mod->p_Data);

	if(!pst_Mod_ODE->i_ode_joint_id) return;

	if(!(_pst_Mod->ul_Flags & (MDF_C_Modifier_Inactive | MDF_C_Modifier_NoApply)))
		return;

	if(!dJointGetBody(pst_Mod_ODE->i_ode_joint_id, 0) && !dJointGetBody(pst_Mod_ODE->i_ode_joint_id, 1))
		return;
	
	switch(pst_Mod_ODE->uc_Type)
	{
		case ODE_JointTypeBall:
		{
			dJointAttach(pst_Mod_ODE->i_ode_joint_id, 0, 0);
			break;
		}

		case ODE_JointTypeHinge:
		{
			dJointAttach(pst_Mod_ODE->i_ode_joint_id, 0, 0);
			break;
		}

		case ODE_JointTypeAMotor:
		{
			dJointAttach(pst_Mod_ODE->i_ode_joint_id, 0, 0);
			break;
		}

	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierODE_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	dBodyID			ode_body_1, ode_body_2;
	GAO_tdst_ModifierODE *pst_Mod_ODE;
	DYN_tdst_ODE		*pst_ODE;
	float				rot[12];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Mod->p_Data || !_pst_Mod->pst_GO) return;

	pst_World = WOR_World_GetWorldOfObject(_pst_Mod->pst_GO);

	pst_Mod_ODE = (GAO_tdst_ModifierODE *) _pst_Mod->p_Data;

	ode_body_1 = (pst_Mod_ODE->pst_GO1 && pst_Mod_ODE->pst_GO1->pst_Base && pst_Mod_ODE->pst_GO1->pst_Base->pst_ODE) ? pst_Mod_ODE->pst_GO1->pst_Base->pst_ODE->ode_id_body : 0;
	ode_body_2 = (pst_Mod_ODE->pst_GO2 && pst_Mod_ODE->pst_GO2->pst_Base && pst_Mod_ODE->pst_GO2->pst_Base->pst_ODE) ? pst_Mod_ODE->pst_GO2->pst_Base->pst_ODE->ode_id_body : 0;

#ifdef ACTIVE_EDITORS
	pst_Mod_ODE->pst_GO = _pst_Mod->pst_GO;
#endif

	if(ode_body_1)
	{
		pst_ODE = pst_Mod_ODE->pst_GO1->pst_Base->pst_ODE;
		ODE_ComputeODEMatrixFromJade(pst_Mod_ODE->pst_GO1->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot); 
		dBodySetPosition(ode_body_1, pst_Mod_ODE->pst_GO1->pst_GlobalMatrix->T.x + pst_ODE->st_Offset.x, pst_Mod_ODE->pst_GO1->pst_GlobalMatrix->T.y + pst_ODE->st_Offset.y, pst_Mod_ODE->pst_GO1->pst_GlobalMatrix->T.z + pst_ODE->st_Offset.z);
		dBodySetRotation(ode_body_1, &rot[0]);
	}

	if(ode_body_2)
	{
		pst_ODE = pst_Mod_ODE->pst_GO2->pst_Base->pst_ODE;
		ODE_ComputeODEMatrixFromJade(pst_Mod_ODE->pst_GO2->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot); 
		dBodySetPosition(ode_body_2, pst_Mod_ODE->pst_GO2->pst_GlobalMatrix->T.x + pst_ODE->st_Offset.x, pst_Mod_ODE->pst_GO2->pst_GlobalMatrix->T.y + pst_ODE->st_Offset.y, pst_Mod_ODE->pst_GO2->pst_GlobalMatrix->T.z + pst_ODE->st_Offset.z);
		dBodySetRotation(ode_body_2, &rot[0]);
	}


	switch(pst_Mod_ODE->uc_Type)
	{
		case ODE_JointTypeBall:
		{
			pst_Mod_ODE->i_ode_joint_id = dJointCreateBall(pst_World->ode_id_world, pst_World->ode_joint_ode);
			dJointAttach(pst_Mod_ODE->i_ode_joint_id, ode_body_1, ode_body_2);
#ifdef JADEFUSION
			dJointSetBallAnchor((dxJointBall* )pst_Mod_ODE->i_ode_joint_id, _pst_Mod->pst_GO->pst_GlobalMatrix->T.x , _pst_Mod->pst_GO->pst_GlobalMatrix->T.y, _pst_Mod->pst_GO->pst_GlobalMatrix->T.z);
#else
			dJointSetBallAnchor(pst_Mod_ODE->i_ode_joint_id, _pst_Mod->pst_GO->pst_GlobalMatrix->T.x , _pst_Mod->pst_GO->pst_GlobalMatrix->T.y, _pst_Mod->pst_GO->pst_GlobalMatrix->T.z);
#endif
			break;
		}

		case ODE_JointTypeHinge:
		{
			pst_Mod_ODE->i_ode_joint_id = dJointCreateHinge(pst_World->ode_id_world, pst_World->ode_joint_ode);

			dJointAttach(pst_Mod_ODE->i_ode_joint_id, ode_body_1, ode_body_2);
#ifdef JADEFUSION
			dJointSetHingeAnchor((dxJointHinge* )pst_Mod_ODE->i_ode_joint_id, _pst_Mod->pst_GO->pst_GlobalMatrix->T.x, _pst_Mod->pst_GO->pst_GlobalMatrix->T.y, _pst_Mod->pst_GO->pst_GlobalMatrix->T.z);
			dJointSetHingeAxis((dxJointHinge* )pst_Mod_ODE->i_ode_joint_id, _pst_Mod->pst_GO->pst_GlobalMatrix->Kx, _pst_Mod->pst_GO->pst_GlobalMatrix->Ky, _pst_Mod->pst_GO->pst_GlobalMatrix->Kz);
#else
			dJointSetHingeAnchor(pst_Mod_ODE->i_ode_joint_id, _pst_Mod->pst_GO->pst_GlobalMatrix->T.x, _pst_Mod->pst_GO->pst_GlobalMatrix->T.y, _pst_Mod->pst_GO->pst_GlobalMatrix->T.z);
			dJointSetHingeAxis(pst_Mod_ODE->i_ode_joint_id, _pst_Mod->pst_GO->pst_GlobalMatrix->Kx, _pst_Mod->pst_GO->pst_GlobalMatrix->Ky, _pst_Mod->pst_GO->pst_GlobalMatrix->Kz);
#endif
			if(pst_Mod_ODE->f_LoLimit)
				dJointSetHingeParam(pst_Mod_ODE->i_ode_joint_id,dParamLoStop, pst_Mod_ODE->f_LoLimit);

			if(pst_Mod_ODE->f_HiLimit)
				dJointSetHingeParam(pst_Mod_ODE->i_ode_joint_id,dParamHiStop, pst_Mod_ODE->f_HiLimit);

			break;
		}

		case ODE_JointTypeFixed:
		{
			pst_Mod_ODE->i_ode_joint_id = dJointCreateFixed(pst_World->ode_id_world, pst_World->ode_joint_ode);
			dJointAttach(pst_Mod_ODE->i_ode_joint_id, ode_body_1, ode_body_2);
#ifdef JADEFUSION
			dJointSetFixed((dxJointFixed*)pst_Mod_ODE->i_ode_joint_id);
#else
			dJointSetFixed(pst_Mod_ODE->i_ode_joint_id);
#endif
			break;
		}


		case ODE_JointTypeAMotor:
		{
			pst_Mod_ODE->i_ode_joint_id = dJointCreateAMotor (pst_World->ode_id_world, pst_World->ode_joint_ode);

			dJointAttach(pst_Mod_ODE->i_ode_joint_id, ode_body_1, ode_body_2);

			dJointSetAMotorMode(pst_Mod_ODE->i_ode_joint_id, dAMotorUser);
			dJointSetAMotorNumAxes (pst_Mod_ODE->i_ode_joint_id, 1);
			dJointSetAMotorAxis(pst_Mod_ODE->i_ode_joint_id, 0, 0, _pst_Mod->pst_GO->pst_GlobalMatrix->Kx, _pst_Mod->pst_GO->pst_GlobalMatrix->Ky, _pst_Mod->pst_GO->pst_GlobalMatrix->Kz);

			if(pst_Mod_ODE->f_LoLimit)
				dJointSetAMotorParam(pst_Mod_ODE->i_ode_joint_id,dParamLoStop, pst_Mod_ODE->f_LoLimit);

			if(pst_Mod_ODE->f_HiLimit)
				dJointSetAMotorParam(pst_Mod_ODE->i_ode_joint_id,dParamHiStop, pst_Mod_ODE->f_HiLimit);


			if(pst_Mod_ODE->f_Friction)
			{
				dJointSetAMotorParam (pst_Mod_ODE->i_ode_joint_id, dParamVel, 0);
				dJointSetAMotorParam(pst_Mod_ODE->i_ode_joint_id, dParamFMax, pst_Mod_ODE->f_Friction);
			}
			break;
		}

	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GAO_ModifierODE_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char							*pc_Cur;
	ULONG							ul_Key;
	GAO_tdst_ModifierODE			*pst_Mod_ODE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	pst_Mod_ODE = (GAO_tdst_ModifierODE *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	pst_Mod_ODE->uc_Version = LOA_ReadUChar(&pc_Cur);
	pst_Mod_ODE->uc_Type = LOA_ReadUChar(&pc_Cur);
	pst_Mod_ODE->uw_Dummy = LOA_ReadUShort(&pc_Cur);

	ul_Key = LOA_ReadULong(&pc_Cur);
#if defined(XML_CONV_TOOL)
	*(ULONG *) &pst_Mod_ODE->pst_GO1 = ul_Key;
#else
	if(ul_Key != BIG_C_InvalidIndex)
	{
		if((int) BIG_ul_SearchKeyToPos(ul_Key) != -1)
			LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Mod_ODE->pst_GO1, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
		else
			pst_Mod_ODE->pst_GO1 = NULL;
	}
	else
		pst_Mod_ODE->pst_GO1 = NULL;
#endif

	ul_Key = LOA_ReadULong(&pc_Cur);
#if defined(XML_CONV_TOOL)
	*(ULONG *) &pst_Mod_ODE->pst_GO2 = ul_Key;
#else
	if(ul_Key != BIG_C_InvalidIndex)
	{
		if((int) BIG_ul_SearchKeyToPos(ul_Key) != -1)
			LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Mod_ODE->pst_GO2, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
		else
			pst_Mod_ODE->pst_GO2 = NULL;
	}
	else
		pst_Mod_ODE->pst_GO2 = NULL;
#endif

	pst_Mod_ODE->i_ode_joint_id = 0;


	if(pst_Mod_ODE->uc_Version > 1)
	{
		pst_Mod_ODE->f_LoLimit = LOA_ReadFloat(&pc_Cur);
		pst_Mod_ODE->f_HiLimit = LOA_ReadFloat(&pc_Cur);
		pst_Mod_ODE->f_BounceStop = LOA_ReadFloat(&pc_Cur);
		pst_Mod_ODE->f_Friction = LOA_ReadFloat(&pc_Cur);
	}

#ifdef ACTIVE_EDITORS
	pst_Mod_ODE->pst_GO = _pst_Mod->pst_GO;
#endif

	return(pc_Cur - _pc_Buffer);

#else
	UCHAR		Version;
	char		*pc_Cur;
	pc_Cur = _pc_Buffer;

	Version = LOA_ReadUChar(&pc_Cur);
	LOA_ReadUChar(&pc_Cur);
	LOA_ReadUShort(&pc_Cur);

	LOA_ReadULong(&pc_Cur);
	LOA_ReadULong(&pc_Cur);

	if(Version > 1)
	{
		LOA_ReadFloat(&pc_Cur);
		LOA_ReadFloat(&pc_Cur);
		LOA_ReadFloat(&pc_Cur);
		LOA_ReadFloat(&pc_Cur);
	}

	return(pc_Cur - _pc_Buffer);
#endif

}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierODE_Save(MDF_tdst_Modifier *_pst_Mod)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierODE			*pst_Mod_ODE;
	ULONG							ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod_ODE = (GAO_tdst_ModifierODE *) _pst_Mod->p_Data;

	/* Save Version */
#if !defined(XML_CONV_TOOL)
	pst_Mod_ODE->uc_Version = 2;
#endif
	SAV_Buffer(&pst_Mod_ODE->uc_Version, 1);

	/* Save Type */
	SAV_Buffer(&pst_Mod_ODE->uc_Type, 1);

	/* Save Dummy */
	SAV_Buffer(&pst_Mod_ODE->uw_Dummy, 2);


	/* Save game object 1*/
	ul_Size = (pst_Mod_ODE->pst_GO1) ? LOA_ul_SearchKeyWithAddress((ULONG) pst_Mod_ODE->pst_GO1) : 0;
	SAV_Buffer(&ul_Size, 4);

	/* Save game object 2*/
	ul_Size = (pst_Mod_ODE->pst_GO2) ? LOA_ul_SearchKeyWithAddress((ULONG) pst_Mod_ODE->pst_GO2) : 0;
	SAV_Buffer(&ul_Size, 4);


	if(pst_Mod_ODE->uc_Version > 1)
	{
		SAV_Buffer(&pst_Mod_ODE->f_LoLimit, 4);
		SAV_Buffer(&pst_Mod_ODE->f_HiLimit, 4);
		SAV_Buffer(&pst_Mod_ODE->f_BounceStop, 4);
		SAV_Buffer(&pst_Mod_ODE->f_Friction, 4);
	}

#endif
}

#endif
