/*$T MDFmodifier_XMEC.c GC! 1.100 08/28/01 17:06:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_XMEC.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTlinear.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    XMEC modifier:: Modifier apply XMEC on world
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void XMEC_FirstInit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	static ULONG	bFirst = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(bFirst) return;
	bFirst = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierXMEC_UpdatePointers(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group			*pst_Group;
	OBJ_tdst_GameObject		*pst_GO;
	GAO_tdst_ModifierXMEC	*p_XMEC;
	int						GOMECCOUNTER;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_XMEC = (GAO_tdst_ModifierXMEC *) _pst_Mod->p_Data;
	pst_Group = MDF_Modifier_GetCurGroup(_pst_Mod);
	if(!pst_Group) return;

	for(GOMECCOUNTER = 0; GOMECCOUNTER < p_XMEC->i_NumBones; GOMECCOUNTER++)
	{
		p_XMEC->ap_BonesObj[GOMECCOUNTER] = NULL;
		p_XMEC->ap_Bones[GOMECCOUNTER] = NULL;
		pst_GO = OBJ_pst_GroupGetByRank(pst_Group, p_XMEC->ai_Bones[GOMECCOUNTER]);

		if(pst_GO) 
		{
			p_XMEC->ap_BonesObj[GOMECCOUNTER] = pst_GO;
			p_XMEC->ap_Bones[GOMECCOUNTER] = OBJ_pst_GetAbsoluteMatrix(pst_GO);
		}
	}

	if(p_XMEC->i_RefBone != -1)
	{
		pst_GO = OBJ_pst_GroupGetByRank(pst_Group, p_XMEC->i_RefBone);
		if(pst_GO) p_XMEC->pst_TrtGO = pst_GO;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierXMEC_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierXMEC	*p_XMEC;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	XMEC_FirstInit();
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierXMEC));
	p_XMEC = (GAO_tdst_ModifierXMEC *) _pst_Mod->p_Data;
	if(p_Data == NULL)
	{
		/* First init */
		L_memset(_pst_Mod->p_Data, 0, sizeof(GAO_tdst_ModifierXMEC));
		p_XMEC->bk_GOMEC = (ULONG) - 1;
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
		p_XMEC->i_NumBones = 0;
		p_XMEC->i_RefBone = -1;
		for(i = 0; i < GOMEC_NUMBER; i++) p_XMEC->ai_Bones[i] = i;
		GAO_ModifierXMEC_UpdatePointers(_pst_Mod);
	}
	else
	{
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierXMEC));
	}

#ifdef ACTIVE_EDITORS
	p_XMEC->ulCodeKey = 0xC0DE2001;
#endif

	p_XMEC->pst_GO = p_XMEC->pst_TrtGO = _pst_GO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierXMEC_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	if(_pst_Mod->p_Data)
	{
		MEM_Free(_pst_Mod->p_Data);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierXMEC_Normalize
(
	MATH_tdst_Vector	*p_Pos0,
	MATH_tdst_Vector	*p_PosZGEG,
	MATH_tdst_Vector	**p_AllPos,
	ULONG				Numbers,
	float				BoneLenght,
	float				fGround,
	float				tension
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ITER, GOMECCOUNTER;
	MATH_tdst_Vector	*p_stPreviousPos;
	MATH_tdst_Vector	stWantedPos;
	MATH_tdst_Vector	stForces[GOMEC_NUMBER];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ITER = 10;
	while(ITER--)
	{
		p_stPreviousPos = p_Pos0;
		for(GOMECCOUNTER = 0; GOMECCOUNTER < Numbers; GOMECCOUNTER++)
		{
			stForces[GOMECCOUNTER].x = 0.0f;
			stForces[GOMECCOUNTER].y = 0.0f;
			stForces[GOMECCOUNTER].z = 0.0f;
		}

		p_stPreviousPos = p_Pos0;
		MATH_AddScaleVector(&stWantedPos, p_stPreviousPos, p_PosZGEG, BoneLenght);
		for(GOMECCOUNTER = 0; GOMECCOUNTER < Numbers; GOMECCOUNTER++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Dist;
			float				fBF;
			/*~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_SubVector(&st_Dist, p_stPreviousPos, p_AllPos[GOMECCOUNTER]);
			fBF = MATH_f_NormVector(&st_Dist);
			fBF -= BoneLenght;

			/* if (fBF < 0.0f) fBF = 0.0f; */
			MATH_NormalizeAnyVector(&st_Dist, &st_Dist);
			if(GOMECCOUNTER != 0)
			{
				MATH_AddScaleVector(&stForces[GOMECCOUNTER - 1], &stForces[GOMECCOUNTER - 1], &st_Dist, -fBF * 0.5f);
				MATH_AddScaleVector(&stForces[GOMECCOUNTER], &stForces[GOMECCOUNTER], &st_Dist, fBF * 0.5f);
			}
			else
				MATH_AddScaleVector(&stForces[GOMECCOUNTER], &stForces[GOMECCOUNTER], &st_Dist, fBF);
			p_stPreviousPos = p_AllPos[GOMECCOUNTER];
		}

		for(GOMECCOUNTER = 0; GOMECCOUNTER < Numbers - 0; GOMECCOUNTER++)
		{
			MATH_AddVector(p_AllPos[GOMECCOUNTER], p_AllPos[GOMECCOUNTER], &stForces[GOMECCOUNTER]);
			if(p_AllPos[GOMECCOUNTER]->z < fGround) p_AllPos[GOMECCOUNTER]->z = fGround;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierXMEC_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierXMEC	*p_XMEC;
	MATH_tdst_Vector		*p_stPreviousPos;
	MATH_tdst_Vector		OldPos[GOMEC_NUMBER];
	MATH_tdst_Vector		StarGlider;
	MATH_tdst_Matrix		AllMatrix[GOMEC_NUMBER];
	MATH_tdst_Matrix		*p_AllMatrix[GOMEC_NUMBER];
	float					DT;
	ULONG					GOMECCOUNTER, GOME_NUM;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	DT = TIM_gf_dt;
	DT = 3.0f / 60.0f;

	if(DT == 0.0f) DT = 1.0f / 50.0f;
	GOMECCOUNTER = GOMEC_NUMBER;

	p_XMEC = (GAO_tdst_ModifierXMEC *) _pst_Mod->p_Data;
	OBJ_ComputeGlobalWithLocal(p_XMEC->pst_GO, OBJ_pst_GetAbsoluteMatrix(p_XMEC->pst_GO), 1);
	if(p_XMEC->pst_GO != p_XMEC->pst_TrtGO)
		OBJ_ComputeGlobalWithLocal(p_XMEC->pst_TrtGO, OBJ_pst_GetAbsoluteMatrix(p_XMEC->pst_TrtGO), 1);

	GOME_NUM = p_XMEC->i_NumBones;
	for(GOMECCOUNTER = 0; GOMECCOUNTER < GOME_NUM; GOMECCOUNTER++)
	{
		p_AllMatrix[GOMECCOUNTER] = &AllMatrix[GOMECCOUNTER];
		MATH_SetIdentityMatrix(p_AllMatrix[GOMECCOUNTER]);
		if(!p_XMEC->ap_Bones[GOMECCOUNTER]) 
			GOME_NUM = GOMECCOUNTER;
		else
			MATH_CopyMatrix(p_AllMatrix[GOMECCOUNTER], p_XMEC->ap_Bones[GOMECCOUNTER]);
	}

	if(!GOME_NUM) return;

	for(GOMECCOUNTER = 0; GOMECCOUNTER < GOME_NUM; GOMECCOUNTER++)
	{
		p_XMEC->stSpeed[GOMECCOUNTER].z += p_XMEC->fGravity * DT;
		OldPos[GOMECCOUNTER] = p_AllMatrix[GOMECCOUNTER]->T;
	}

	/* Bones Tensions Elastic! */
#if 0
	p_stPreviousPos = &p_XMEC->pst_TrtGO->pst_GlobalMatrix->T;
	for(GOMECCOUNTER = 0; GOMECCOUNTER < GOME_NUM; GOMECCOUNTER++)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Dist;
		float				fBF;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_SubVector(&st_Dist, &p_XMEC->pst_GOMEC[GOMECCOUNTER]->pst_GlobalMatrix->T, p_stPreviousPos);
		fBF = MATH_f_NormVector(&st_Dist);
		fBF -= p_XMEC->fDistance;
		fBF *= p_XMEC->fElasticity;
		if(fBF < 0.0f) fBF = 0.0f;
		MATH_NormalizeAnyVector(&st_Dist, &st_Dist);
		if(GOMECCOUNTER != 0)
		{
			MATH_AddScaleVector(&p_XMEC->stSpeed[GOMECCOUNTER], &p_XMEC->stSpeed[GOMECCOUNTER], &st_Dist, -fBF * DT);
			MATH_AddScaleVector
			(
				&p_XMEC->stSpeed[GOMECCOUNTER - 1],
				&p_XMEC->stSpeed[GOMECCOUNTER - 1],
				&st_Dist,
				fBF * DT
			);
		}
		else
		{
			MATH_AddScaleVector
			(
				&p_XMEC->stSpeed[GOMECCOUNTER],
				&p_XMEC->stSpeed[GOMECCOUNTER],
				&st_Dist,
				-2.0f * fBF * DT
			);
		}

		p_stPreviousPos = &p_XMEC->pst_GOMEC[GOMECCOUNTER]->pst_GlobalMatrix->T;
	}

#endif
	/* Tensions */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	*p_stPreviousPosM2;
		float				Disprop, DispropAdd;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		p_stPreviousPos = &StarGlider;
		if(p_XMEC->ulFlags & XMEC_C_TakeY)
			StarGlider = *MATH_pst_GetYAxis(p_XMEC->pst_TrtGO->pst_GlobalMatrix);
		else
			StarGlider = *MATH_pst_GetZAxis(p_XMEC->pst_TrtGO->pst_GlobalMatrix);
		MATH_AddScaleVector(&StarGlider, &p_XMEC->pst_TrtGO->pst_GlobalMatrix->T, &StarGlider, -p_XMEC->fDistance);
		p_stPreviousPosM2 = &StarGlider;
		p_stPreviousPos = &p_XMEC->pst_TrtGO->pst_GlobalMatrix->T;

		Disprop = p_XMEC->fTension * DT;
		DispropAdd = Disprop * p_XMEC->fElasticity / (float) GOME_NUM;

		for(GOMECCOUNTER = 0; GOMECCOUNTER < GOME_NUM; GOMECCOUNTER++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	stWantedPos;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(p_XMEC->ulFlags & XMEC_C_TakeY)
				stWantedPos = *MATH_pst_GetYAxis(p_XMEC->pst_TrtGO->pst_GlobalMatrix);
			else
				stWantedPos = *MATH_pst_GetZAxis(p_XMEC->pst_TrtGO->pst_GlobalMatrix);
			MATH_AddScaleVector(&p_XMEC->stSpeed[GOMECCOUNTER], &p_XMEC->stSpeed[GOMECCOUNTER], &stWantedPos, Disprop);
			Disprop -= DispropAdd;
			p_stPreviousPosM2 = p_stPreviousPos;
			p_stPreviousPos = &p_AllMatrix[GOMECCOUNTER]->T;
		}
	}

	/* Normal moves */
	for(GOMECCOUNTER = 0; GOMECCOUNTER < GOME_NUM - 0; GOMECCOUNTER++)
	{
		if(p_AllMatrix[GOMECCOUNTER]->T.z <= p_XMEC->fGround)
		{
			p_AllMatrix[GOMECCOUNTER]->T.z = p_XMEC->fGround;
			p_XMEC->stSpeed[GOMECCOUNTER].x *= p_XMEC->fGroundFriction;
			p_XMEC->stSpeed[GOMECCOUNTER].y *= p_XMEC->fGroundFriction;

			/* p_XMEC->stSpeed[GOMECCOUNTER].z = 0.0f; */
		}

		MATH_AddScaleVector
		(
			&p_AllMatrix[GOMECCOUNTER]->T,
			&p_AllMatrix[GOMECCOUNTER]->T,
			&p_XMEC->stSpeed[GOMECCOUNTER],
			DT
		);
	}

	/* Normalize */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	*AllPos[GOMEC_NUMBER];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(GOMECCOUNTER = 0; GOMECCOUNTER < GOME_NUM; GOMECCOUNTER++)
			AllPos[GOMECCOUNTER] = &p_AllMatrix[GOMECCOUNTER]->T;
		if(p_XMEC->ulFlags & XMEC_C_TakeY)
		{
			GAO_ModifierXMEC_Normalize
			(
				&p_XMEC->pst_TrtGO->pst_GlobalMatrix->T,
				MATH_pst_GetYAxis(p_XMEC->pst_TrtGO->pst_GlobalMatrix),
				AllPos,
				GOME_NUM,
				p_XMEC->fDistance,
				p_XMEC->fGround,
				p_XMEC->fTension
			);
		}
		else
		{
			GAO_ModifierXMEC_Normalize
			(
				&p_XMEC->pst_TrtGO->pst_GlobalMatrix->T,
				MATH_pst_GetZAxis(p_XMEC->pst_TrtGO->pst_GlobalMatrix),
				AllPos,
				GOME_NUM,
				p_XMEC->fDistance,
				p_XMEC->fGround,
				p_XMEC->fTension
			);
		}
	}

	/* Recompute Real Speeds */
	p_stPreviousPos = &p_XMEC->pst_TrtGO->pst_GlobalMatrix->T;
	for(GOMECCOUNTER = 0; GOMECCOUNTER < GOME_NUM; GOMECCOUNTER++)
	{
		MATH_SubVector(&p_XMEC->stSpeed[GOMECCOUNTER], &p_AllMatrix[GOMECCOUNTER]->T, &OldPos[GOMECCOUNTER]);
		MATH_ScaleEqualVector(&p_XMEC->stSpeed[GOMECCOUNTER], 1.0F / DT);
	}

	/* Orient son Matrixes as Father */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	*p_stLastBody;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		p_stPreviousPos = &p_XMEC->pst_TrtGO->pst_GlobalMatrix->T;
		p_stLastBody = p_XMEC->pst_TrtGO->pst_GlobalMatrix;
		for(GOMECCOUNTER = 0; GOMECCOUNTER < GOME_NUM; GOMECCOUNTER++)
		{
			if(p_XMEC->ulFlags & XMEC_C_TakeY)
			{
				MATH_SubVector
				(
					MATH_pst_GetYAxis(p_AllMatrix[GOMECCOUNTER]),
					p_stPreviousPos,
					&p_AllMatrix[GOMECCOUNTER]->T
				);
				MATH_NormalizeAnyVector
				(
					MATH_pst_GetYAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetYAxis(p_AllMatrix[GOMECCOUNTER])
				);
				MATH_CrossProduct
				(
					MATH_pst_GetZAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetXAxis(p_stLastBody),
					MATH_pst_GetYAxis(p_AllMatrix[GOMECCOUNTER])
				);
				MATH_NormalizeAnyVector
				(
					MATH_pst_GetZAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetZAxis(p_AllMatrix[GOMECCOUNTER])
				);
				MATH_CrossProduct
				(
					MATH_pst_GetXAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetYAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetZAxis(p_AllMatrix[GOMECCOUNTER])
				);
			}
			else
			{
				MATH_SubVector
				(
					MATH_pst_GetZAxis(p_AllMatrix[GOMECCOUNTER]),
					&p_AllMatrix[GOMECCOUNTER]->T,
					p_stPreviousPos
				);
				MATH_NormalizeAnyVector
				(
					MATH_pst_GetZAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetZAxis(p_AllMatrix[GOMECCOUNTER])
				);
				MATH_CrossProduct
				(
					MATH_pst_GetYAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetZAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetXAxis(p_stLastBody)
				);
				MATH_NormalizeAnyVector
				(
					MATH_pst_GetYAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetYAxis(p_AllMatrix[GOMECCOUNTER])
				);
				MATH_CrossProduct
				(
					MATH_pst_GetXAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetYAxis(p_AllMatrix[GOMECCOUNTER]),
					MATH_pst_GetZAxis(p_AllMatrix[GOMECCOUNTER])
				);
			}
			p_stLastBody = p_AllMatrix[GOMECCOUNTER];
			p_stPreviousPos = &p_AllMatrix[GOMECCOUNTER]->T;
		}
	}

	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix stInverted ONLY_PSX2_ALIGNED(16);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_InvertMatrix(&stInverted, p_XMEC->pst_TrtGO->pst_GlobalMatrix);
		for(GOMECCOUNTER = 0; GOMECCOUNTER < GOME_NUM; GOMECCOUNTER++)
		{
			MATH_CopyMatrix(p_XMEC->ap_Bones[GOMECCOUNTER], p_AllMatrix[GOMECCOUNTER]);
			OBJ_ComputeLocalWhenHie(p_XMEC->ap_BonesObj[GOMECCOUNTER]);
		}
	}

	p_XMEC->stSavedMatrix = *p_XMEC->pst_TrtGO->pst_GlobalMatrix;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierXMEC_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierXMEC_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierXMEC	*p_XMEC;
	int						GOMECCOUNTER;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_XMEC = (GAO_tdst_ModifierXMEC *) _pst_Mod->p_Data;
	if(p_XMEC->i_NumBones > 32) p_XMEC->i_NumBones = 32;

	/* Locals forces */
	for(GOMECCOUNTER = 0; GOMECCOUNTER < GOMEC_NUMBER; GOMECCOUNTER++)
	{
		p_XMEC->stForces[GOMECCOUNTER].x = 0.0f;
		p_XMEC->stForces[GOMECCOUNTER].y = 0.0f;
		p_XMEC->stForces[GOMECCOUNTER].z = 0.0f;
		p_XMEC->stSpeed[GOMECCOUNTER].x = 0.0f;
		p_XMEC->stSpeed[GOMECCOUNTER].y = 0.0f;
		p_XMEC->stSpeed[GOMECCOUNTER].z = 0.0f;
	}

	GAO_ModifierXMEC_UpdatePointers(_pst_Mod);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GAO_ModifierXMEC_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierXMEC	*p_XMEC;
	char					*_pc_BufferSave;
	ULONG					ulVersion;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	XMEC_FirstInit();
	_pc_BufferSave = _pc_Buffer;
	p_XMEC = (GAO_tdst_ModifierXMEC *) _pst_Mod->p_Data;

	/* size */
	LOA_ReadLong_Ed(&_pc_Buffer, NULL); // skip size

	/* version */
	ulVersion = LOA_ReadULong(&_pc_Buffer);
	if(ulVersion > 0)
	{
		LONG i;
		INT * p;
		p_XMEC->fDistance = LOA_ReadFloat(&_pc_Buffer);
		p_XMEC->fGravity = LOA_ReadFloat(&_pc_Buffer);
		p_XMEC->fElasticity = LOA_ReadFloat(&_pc_Buffer);
		p_XMEC->fGround = LOA_ReadFloat(&_pc_Buffer);
		p_XMEC->fGroundFriction = LOA_ReadFloat(&_pc_Buffer);
		p_XMEC->fTension = LOA_ReadFloat(&_pc_Buffer);
		p_XMEC->ulFlags = LOA_ReadULong(&_pc_Buffer);
		p_XMEC->i_NumBones = LOA_ReadInt(&_pc_Buffer);
		p_XMEC->i_RefBone = LOA_ReadInt(&_pc_Buffer);
		for(i = 0, p = p_XMEC->ai_Bones; i < GOMEC_NUMBER; ++i, ++p)
		{
			*p = LOA_ReadInt(&_pc_Buffer);
		}
	}

	p_XMEC->pst_GO = p_XMEC->pst_TrtGO = _pst_Mod->pst_GO;

#ifdef ACTIVE_EDITORS
	p_XMEC->ulCodeKey = 0xC0DE2001;
#endif
	return _pc_Buffer - _pc_BufferSave;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierXMEC_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierXMEC	*p_XMEC;
	ULONG					ulSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_XMEC = (GAO_tdst_ModifierXMEC *) _pst_Mod->p_Data;

	/* Save Size */
	ulSize = 24;
	SAV_Buffer(&ulSize, 4);

	/* Save version */
	ulSize = 1;
	SAV_Buffer(&ulSize, 4);

	SAV_Buffer(&p_XMEC->fDistance, 4);
	SAV_Buffer(&p_XMEC->fGravity, 4);
	SAV_Buffer(&p_XMEC->fElasticity, 4);
	SAV_Buffer(&p_XMEC->fGround, 4);
	SAV_Buffer(&p_XMEC->fGroundFriction, 4);
	SAV_Buffer(&p_XMEC->fTension, 4);
	SAV_Buffer(&p_XMEC->ulFlags, 4);
	SAV_Buffer(&p_XMEC->i_NumBones, 4);
	SAV_Buffer(&p_XMEC->i_RefBone, 4);
	SAV_Buffer(p_XMEC->ai_Bones, 4 * GOMEC_NUMBER);
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
