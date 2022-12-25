/*$T MDFmodifier_SNDVOL.c GC 1.138 02/21/05 14:24:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "Precomp.h"

#include "GEOmetric/GEOobject.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_SNDVOL.h"
#include "MoDiFier/MDFmodifier_GEN.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BASsys.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDvolume.h"

#include "ENGine/Sources/OBJects/OBJorient.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAdefs.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAread.h"
#include "SDK/Sources/BIGfiles/BIGfat.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ModifierSoundVol_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_ModifierSoundVol	*pst_SndVol;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SndVol = (SND_tdst_ModifierSoundVol *) MEM_p_Alloc(sizeof(SND_tdst_ModifierSoundVol));

	if(pst_SndVol)
	{
		L_memset(pst_SndVol, 0, sizeof(SND_tdst_ModifierSoundVol));

		pst_SndVol->ul_Version = SND_Cte_MdFSoundVol_Version;

		/* settings */
		pst_SndVol->ul_Flags = SND_Cte_MdFSoundVol_Spheric;
		pst_SndVol->i_GroupId = SND_e_GrpMusic;
		pst_SndVol->f_NearVol = 0.5f;
		pst_SndVol->af_Near[0] = 30.0f;
		pst_SndVol->af_Near[1] = 30.0f;
		pst_SndVol->af_Near[2] = 30.0f;
		pst_SndVol->f_FarVol = 1.0f;
		pst_SndVol->af_Far[0] = 100.0f;
		pst_SndVol->af_Far[1] = 100.0f;
		pst_SndVol->af_Far[2] = 100.0f;
		pst_SndVol->f_ActivationRadius = 100.0f + SND_Cte_MdFSoundVol_Delta;	/* active under 120 meters */

		L_memset(pst_SndVol->asz_Reserved, 0, SND_Cte_MdFSoundVol_Reserve);

		/* dynamique data */
		pst_SndVol->i_VolRqId = -1;

		_pst_Mod->p_Data = pst_SndVol;

		/* duplicate operation ? */
		if(p_Data)
		{
			_pst_Mod->i->pfnul_Load(_pst_Mod, (char *) p_Data);
		}
	}
	else
		_pst_Mod->p_Data = 0;
	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGen;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ModifierSoundVol_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_ModifierSoundVol	*pMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pMdF = (SND_tdst_ModifierSoundVol *) _pst_Mod->p_Data;
	if(pMdF->i_VolRqId != -1) SND_GroupFreeRequestId(pMdF->i_GroupId, pMdF->i_VolRqId);

	/*$2- free data --------------------------------------------------------------------------------------------------*/

	if(_pst_Mod->p_Data) MEM_Free(_pst_Mod->p_Data);
	_pst_Mod->p_Data = NULL;
	_pst_Mod->ul_Flags = MDF_C_Modifier_Inactive;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ModifierSoundVol_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_ModifierSoundVol	*pMdF;
	OBJ_tdst_GameObject			*pGO;
	float						f_dist;
	float						f_Vn, f_Dn;
	float						f_Vf, f_Df;
	float						f_Factor;
	MATH_tdst_Vector			st_MdF, st_Vect;
	int							axis;
	extern void					AI_EvalFunc_OBJGetPos_C(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(!SND_gst_Params.pst_RefForVol) return;

	pMdF = (SND_tdst_ModifierSoundVol *) _pst_Mod->p_Data;
	pGO = (OBJ_tdst_GameObject *) _pst_Obj;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	f_dist = MATH_f_Distance
		(
			MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol),
			MATH_pst_GetTranslation(pGO->pst_GlobalMatrix)
		);

	if(f_dist > pMdF->f_ActivationRadius) return;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(pMdF->i_VolRqId < 0) pMdF->i_VolRqId = SND_i_GroupGetRequestId(pMdF->i_GroupId);
	if(pMdF->i_VolRqId < 0) return;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	AI_EvalFunc_OBJGetPos_C(pGO, &st_MdF);
	if(pMdF->ul_Flags & SND_Cte_MdFSoundVol_Spheric)
	{
		f_dist = MATH_f_Distance(MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol), &st_MdF);

		if(f_dist <= pMdF->af_Near[0])
			f_Factor = pMdF->f_NearVol;
		else if(f_dist < pMdF->af_Far[0])
		{
			f_Vn = pMdF->f_NearVol;
			f_Dn = pMdF->af_Near[0];
			f_Vf = pMdF->f_FarVol;
			f_Df = pMdF->af_Far[0];
			f_Factor = (fInv(fSqr(f_dist)) - fInv(fSqr(f_Df))) * (fSqr(f_Df) * fSqr(f_Dn)) / (fSqr(f_Df) - fSqr(f_Dn)) * (f_Vn - f_Vf) + f_Vf;
		}
		else
			f_Factor = pMdF->f_FarVol;
	}
	else
	{
		f_Factor = 0.0f;

		for(axis = 0; axis < 3; axis++)
		{
			if(pMdF->ul_Flags & (SND_Cte_MdFSoundVol_Xaxis << axis))
			{
				MATH_SubVector(&st_Vect, MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol), &st_MdF);
				switch(axis)
				{
				case 0:
					f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetXAxis(pGO->pst_GlobalMatrix)));
					break;

				case 1:
					f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetYAxis(pGO->pst_GlobalMatrix)));
					break;

				case 2:
					f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetZAxis(pGO->pst_GlobalMatrix)));
					break;

				default:
					ERR_X_Warning(0, "Bad axis id (SND_ModifierSoundVol_Apply)", NULL);
					f_dist = 0.0f;
					break;
				}

				if(f_dist <= pMdF->af_Near[axis])
					f_Factor = fMax(f_Factor , pMdF->f_NearVol);
				else if(f_dist < pMdF->af_Far[axis])
				{
					f_Vn = pMdF->f_NearVol;
					f_Dn = pMdF->af_Near[axis];
					f_Vf = pMdF->f_FarVol;
					f_Df = pMdF->af_Far[axis];
					
                    f_dist = (fInv(fSqr(f_dist)) - fInv(fSqr(f_Df))) * (fSqr(f_Df) * fSqr(f_Dn)) / (fSqr(f_Df) - fSqr(f_Dn)) * (f_Vn - f_Vf) + f_Vf;
                    
                    f_Factor = fMax(f_Factor , f_dist);
				}
				else
					f_Factor = fMax(f_Factor, pMdF->f_FarVol);
			}
		}

        f_Factor = f_Factor > 1.0f ? 1.0f : f_Factor;
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	SND_GroupSendRequest(pMdF->i_GroupId, pMdF->i_VolRqId, f_Factor);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ModifierSoundVol_Desactivate(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_ModifierSoundVol	*pMdF;
	OBJ_tdst_GameObject			*pGO;
	float						f_dist;
	MATH_tdst_Vector			st_MdF, st_Vect;
	int							axis;
	extern void					AI_EvalFunc_OBJGetPos_C(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(!SND_gst_Params.pst_RefForVol) return;

	pMdF = (SND_tdst_ModifierSoundVol *) _pst_Mod->p_Data;
	pGO = (OBJ_tdst_GameObject *) _pst_Obj;

	_pst_Mod->ul_Flags &= ~MDF_C_Modifier_Inactive;

	AI_EvalFunc_OBJGetPos_C(pGO, &st_MdF);
	if(pMdF->ul_Flags & SND_Cte_MdFSoundVol_Spheric)
	{
		f_dist = MATH_f_Distance
				(
					MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol),
					MATH_pst_GetTranslation(pGO->pst_GlobalMatrix)
				);
	
		if (f_dist > pMdF->af_Far[0])
		{
			SND_GroupSendRequest(pMdF->i_GroupId, pMdF->i_VolRqId, pMdF->f_FarVol);
			if(pMdF->i_VolRqId != -1) SND_GroupFreeRequestId(pMdF->i_GroupId, pMdF->i_VolRqId);
			pMdF->i_VolRqId = -1;
			_pst_Mod->ul_Flags |= MDF_C_Modifier_Inactive;
		}
	}
	else
	{
		for(axis = 0; axis < 3; axis++)
		{
			if(pMdF->ul_Flags & (SND_Cte_MdFSoundVol_Xaxis << axis))
			{
				MATH_SubVector(&st_Vect, MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol), &st_MdF);
				switch(axis)
				{
				case 0:
					f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetXAxis(pGO->pst_GlobalMatrix)));
					break;

				case 1:
					f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetYAxis(pGO->pst_GlobalMatrix)));
					break;

				case 2:
					f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetZAxis(pGO->pst_GlobalMatrix)));
					break;

				default:
					ERR_X_Warning(0, "Bad axis id (SND_ModifierSoundVol_Apply)", NULL);
					f_dist = 0.0f;
					break;
				}

				if(f_dist > pMdF->af_Far[axis])
				{
					SND_GroupSendRequest(pMdF->i_GroupId, pMdF->i_VolRqId, pMdF->f_FarVol);
					if(pMdF->i_VolRqId != -1) SND_GroupFreeRequestId(pMdF->i_GroupId, pMdF->i_VolRqId);
					pMdF->i_VolRqId = -1;
					_pst_Mod->ul_Flags |= MDF_C_Modifier_Inactive;
				}
			}
		}
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ModifierSoundVol_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ModifierSoundVol_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_ModifierSoundVol	*pMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pMdF = (SND_tdst_ModifierSoundVol *) _pst_Mod->p_Data;
	if(pMdF->i_VolRqId != -1) SND_GroupFreeRequestId(pMdF->i_GroupId, pMdF->i_VolRqId);
	pMdF->i_VolRqId = -1;

	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGen;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG SND_ModifierSoundVol_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_ModifierSoundVol	*pst_Dst;
	char						*pc_Buf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Buf = _pc_Buffer;
	pst_Dst = (SND_tdst_ModifierSoundVol *) _pst_Mod->p_Data;

	pst_Dst->ul_Version = LOA_ReadULong(&pc_Buf);
	pst_Dst->ul_Flags = LOA_ReadULong(&pc_Buf);

	pst_Dst->i_GroupId = LOA_ReadInt(&pc_Buf);
	pst_Dst->f_NearVol = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Near[0] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Near[1] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Near[2] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->f_FarVol = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Far[0] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Far[1] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Far[2] = LOA_ReadFloat(&pc_Buf);

	pst_Dst->f_ActivationRadius = LOA_ReadFloat_Ed(&pc_Buf, NULL);
	pst_Dst->i_VolRqId = LOA_ReadInt_Ed(&pc_Buf, NULL);

	LOA_ReadCharArray_Ed(&pc_Buf, NULL, SND_Cte_MdFSoundVol_Reserve);	/* skip asz_Reserve */

#if !defined(XML_CONV_TOOL)
	/*$1- consistency + init ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((pst_Dst->i_GroupId < 0) || (pst_Dst->i_GroupId > SND_e_UserGrpMaster)) pst_Dst->i_GroupId = SND_e_GrpMusic;
	if(pst_Dst->af_Near[0] > pst_Dst->af_Far[0]) pst_Dst->af_Near[0] = pst_Dst->af_Far[0];
	if(pst_Dst->af_Near[1] > pst_Dst->af_Far[1]) pst_Dst->af_Near[1] = pst_Dst->af_Far[1];
	if(pst_Dst->af_Near[2] > pst_Dst->af_Far[2]) pst_Dst->af_Near[2] = pst_Dst->af_Far[2];

	switch
	(
		pst_Dst->ul_Flags &
			(
				SND_Cte_MdFSoundVol_Spheric |
				SND_Cte_MdFSoundVol_Xaxis |
				SND_Cte_MdFSoundVol_Yaxis |
				SND_Cte_MdFSoundVol_Zaxis
			)
	)
	{
	case SND_Cte_MdFSoundVol_Spheric:
		pst_Dst->f_ActivationRadius = pst_Dst->af_Far[0] + SND_Cte_MdFSoundVol_Delta;
		break;

	case SND_Cte_MdFSoundVol_Xaxis:
		pst_Dst->f_ActivationRadius = pst_Dst->af_Far[0] + SND_Cte_MdFSoundVol_Delta;
		break;

	case SND_Cte_MdFSoundVol_Yaxis:
		pst_Dst->f_ActivationRadius = pst_Dst->af_Far[1] + SND_Cte_MdFSoundVol_Delta;
		break;

	case SND_Cte_MdFSoundVol_Zaxis:
		pst_Dst->f_ActivationRadius = pst_Dst->af_Far[2] + SND_Cte_MdFSoundVol_Delta;
		break;

	case (SND_Cte_MdFSoundVol_Xaxis | SND_Cte_MdFSoundVol_Yaxis):
		pst_Dst->f_ActivationRadius = fMax(pst_Dst->af_Far[0], pst_Dst->af_Far[1]);
		pst_Dst->f_ActivationRadius += SND_Cte_MdFSoundVol_Delta;
		break;

	case (SND_Cte_MdFSoundVol_Xaxis | SND_Cte_MdFSoundVol_Zaxis):
		pst_Dst->f_ActivationRadius = fMax(pst_Dst->af_Far[0], pst_Dst->af_Far[2]);
		pst_Dst->f_ActivationRadius += SND_Cte_MdFSoundVol_Delta;
		break;

	case (SND_Cte_MdFSoundVol_Yaxis | SND_Cte_MdFSoundVol_Zaxis):
		pst_Dst->f_ActivationRadius = fMax(pst_Dst->af_Far[1], pst_Dst->af_Far[2]);
		pst_Dst->f_ActivationRadius += SND_Cte_MdFSoundVol_Delta;
		break;

	case (SND_Cte_MdFSoundVol_Xaxis | SND_Cte_MdFSoundVol_Yaxis | SND_Cte_MdFSoundVol_Zaxis):
		pst_Dst->f_ActivationRadius = fMax(pst_Dst->af_Far[0], pst_Dst->af_Far[1]);
		pst_Dst->f_ActivationRadius = fMax(pst_Dst->f_ActivationRadius, pst_Dst->af_Far[2]);
		pst_Dst->f_ActivationRadius += SND_Cte_MdFSoundVol_Delta;
		break;

	default:
		pst_Dst->ul_Flags |= SND_Cte_MdFSoundVol_Spheric;
		pst_Dst->f_ActivationRadius = 100.0f + SND_Cte_MdFSoundVol_Delta;
		break;
	}

	pst_Dst->i_VolRqId = -1;

	/*$1- activation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGen;
#endif	// !XML_CONV_TOOL

	return(pc_Buf - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ModifierSoundVol_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_ModifierSoundVol	*pst_Src;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Src = (SND_tdst_ModifierSoundVol *) _pst_Mod->p_Data;

	if(pst_Src)
	{
		if(pst_Src->af_Near[0] > pst_Src->af_Far[0]) pst_Src->af_Near[0] = pst_Src->af_Far[0];
		if(pst_Src->af_Near[1] > pst_Src->af_Far[1]) pst_Src->af_Near[1] = pst_Src->af_Far[1];
		if(pst_Src->af_Near[2] > pst_Src->af_Far[2]) pst_Src->af_Near[2] = pst_Src->af_Far[2];

		SAV_Buffer(pst_Src, sizeof(SND_tdst_ModifierSoundVol));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_ModifierSoundVol_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src)
{
	SND_ModifierSoundVol_Load(_pst_Dst, (char* )_pst_Src->p_Data);
	return sizeof(SND_tdst_ModifierSoundVol);
}
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
