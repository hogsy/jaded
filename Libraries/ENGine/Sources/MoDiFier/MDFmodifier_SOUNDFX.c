/*$T MDFmodifier_SOUNDFX.c GC 1.138 07/30/04 17:45:19 */


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
#include "MoDiFier/MDFmodifier_SOUNDFX.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BASsys.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDfx.h"

#include "ENGine/Sources/OBJects/OBJorient.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAdefs.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAread.h"
#include "SDK/Sources/BIGfiles/BIGfat.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

static BOOL b_TestActiveVol(GEN_tdst_ModifierSoundFx *pst_MS, OBJ_tdst_GameObject *pGO, BOOL bInside, float *);

/*$4
 ***********************************************************************************************************************
    static var
 ***********************************************************************************************************************
 */

SND_tdst_FxNetworkRoot	SND_gax_FxNetwork[SND_Cte_FxNetworkNbMax];

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_FxNetworkInit(void)
{
	/*~~*/
	int i;
	/*~~*/

	L_memset(SND_gax_FxNetwork, 0, sizeof(SND_tdst_FxNetworkRoot) * SND_Cte_FxNetworkNbMax);
	for(i = 0; i < SND_Cte_FxNetworkNbMax; i++)
	{
		SND_gax_FxNetwork[i].i_NetIdx = -1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_FxNetworkGet(int id)
{
	/*~~~~~*/
	int i, j;
	/*~~~~~*/

	j = -1;

	for(i = 0; i < SND_Cte_FxNetworkNbMax; i++)
	{
		if(SND_gax_FxNetwork[i].i_NetIdx == id) return i;
		if(SND_gax_FxNetwork[i].i_NetIdx == -1) j = i;
	}

	ERR_X_Assert(j != -1);

	SND_gax_FxNetwork[j].i_NetIdx = id;
	return j;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEN_tdst_ModifierSoundFx *SND_p_FxNetworkGetMaster(int id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	MDF_tdst_Modifier			*pNode;
	GEN_tdst_ModifierSoundFx	*pFxNode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < SND_Cte_FxNetworkNbMax; i++)
	{
		if(SND_gax_FxNetwork[i].i_NetIdx != id) continue;
		pNode = (MDF_tdst_Modifier*)SND_gax_FxNetwork[i].p_Root;
		pFxNode = (GEN_tdst_ModifierSoundFx *) pNode->p_Data;
		if(pFxNode->ui_MdfFlag & MDF_Cte_SndFx_NetMaster)
			return pFxNode;
		else
			return NULL;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_FxNetworkDeleteNode(GEN_tdst_ModifierSoundFx *pOldNode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	MDF_tdst_Modifier			*pNode, *pPrevNode;
	GEN_tdst_ModifierSoundFx	*pFxNode, *pFxPrevNode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < SND_Cte_FxNetworkNbMax; i++)
	{
		if(SND_gax_FxNetwork[i].i_NetIdx == -1) continue;

		pNode = pPrevNode = NULL;
		pFxNode = pFxPrevNode = NULL;

		pNode = (MDF_tdst_Modifier*)SND_gax_FxNetwork[i].p_Root;
		if (pNode == NULL)
			return;
		pFxNode = (GEN_tdst_ModifierSoundFx *) pNode->p_Data;

		while(pNode)
		{
			if(pFxNode == pOldNode)
			{
				if(pFxPrevNode)
					pFxPrevNode->p_Next = pOldNode->p_Next;
				else
					SND_gax_FxNetwork[i].p_Root = pOldNode->p_Next;
			}

			pPrevNode = pNode;
			pFxPrevNode = pFxNode;

			pNode = (MDF_tdst_Modifier *) pFxNode->p_Next;

			if(pNode)
				pFxNode = (GEN_tdst_ModifierSoundFx *) pNode->p_Data;
			else
				break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSoundFx_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSoundFx	*pst_SndFxMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SndFxMdF = (GEN_tdst_ModifierSoundFx *) MEM_p_Alloc(sizeof(GEN_tdst_ModifierSoundFx));

	if(pst_SndFxMdF)
	{
		L_memset(pst_SndFxMdF, 0, sizeof(GEN_tdst_ModifierSoundFx));

		pst_SndFxMdF->ui_MdFVersion = SND_Cte_MdFFxVersion;
		pst_SndFxMdF->ui_MdfFlag = 0;

		pst_SndFxMdF->af_Distance[0] = 5.0f;
		pst_SndFxMdF->af_Distance[1] = 5.0f;
		pst_SndFxMdF->af_Distance[2] = 5.0f;
		pst_SndFxMdF->f_Delta = 0.5f;
		pst_SndFxMdF->af_Far[0] = 10.0f;
		pst_SndFxMdF->af_Far[1] = 10.0f;
		pst_SndFxMdF->af_Far[2] = 10.0f;

		pst_SndFxMdF->i_CoreId = 0;
		pst_SndFxMdF->i_Mode = SND_Cte_FxMode_Off;
		pst_SndFxMdF->f_WetVol = 0.5f;
		pst_SndFxMdF->i_Delay = 300;
		pst_SndFxMdF->i_Feedback = 50;

		pst_SndFxMdF->i_NetIdx = -1;
		pst_SndFxMdF->p_Next = NULL;

		_pst_Mod->p_Data = pst_SndFxMdF;

		/* duplicate operation ? */
		if(p_Data)
		{
			_pst_Mod->i->pfnul_Load(_pst_Mod, (char *) p_Data);
			pst_SndFxMdF->ui_MdfFlag &= ~MDF_Cte_SndFx_Activated;
			pst_SndFxMdF->ui_MdfFlag &= ~MDF_Cte_SndFx_EdiDontDisplay;
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
void GEN_ModifierSoundFx_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSoundFx	*pMdF;
	int							id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- destroy the network ----------------------------------------------------------------------------------------*/

	pMdF = (GEN_tdst_ModifierSoundFx *) _pst_Mod->p_Data;
	if(pMdF->i_NetIdx != -1)
	{
		id = SND_i_FxNetworkGet(pMdF->i_NetIdx);
		if(id != -1)
		{
			SND_gax_FxNetwork[id].i_NetIdx = -1;
			SND_gax_FxNetwork[id].p_Root = NULL;
		}
	}

	/*$2- free data --------------------------------------------------------------------------------------------------*/

	if(_pst_Mod->p_Data) MEM_Free(_pst_Mod->p_Data);
	_pst_Mod->p_Data = NULL;
	_pst_Mod->ul_Flags = MDF_C_Modifier_Inactive;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSoundFx_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSoundFx	*pst_MS;
	float						f_vol, f_maxvol;
	int							id;
	MDF_tdst_Modifier			*pNode;
	GEN_tdst_ModifierSoundFx	*pFxNode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(!SND_gst_Params.l_Available) return;
	if(!SND_gst_Params.pst_RefForVol) return;
	pst_MS = (GEN_tdst_ModifierSoundFx *) _pst_Mod->p_Data;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    compute network (if any)
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(pst_MS->i_NetIdx != -1)
	{
		if(pst_MS->ui_MdfFlag & MDF_Cte_SndFx_NetMaster)
		{
			id = SND_i_FxNetworkGet(pst_MS->i_NetIdx);
			pNode = (MDF_tdst_Modifier*)SND_gax_FxNetwork[id].p_Root;

			f_maxvol = -1.0f;
			f_vol = 0.0f;

			if(pst_MS->ui_MdfFlag & MDF_Cte_SndFx_Activated)
			{
				/*~~~~~~~~~~~~~~~~~~~~~*/
				BOOL	b_Inside = FALSE;
				/*~~~~~~~~~~~~~~~~~~~~~*/

				while(pNode)
				{
					pFxNode = (GEN_tdst_ModifierSoundFx *) pNode->p_Data;

					if(!b_TestActiveVol(pFxNode, (OBJ_tdst_GameObject *) _pst_Obj, FALSE, &f_vol))
					{
						b_Inside = TRUE;
					}

					if(f_vol > f_maxvol) f_maxvol = f_vol;
					pNode = (MDF_tdst_Modifier*)pFxNode->p_Next;
				}

				if(b_Inside)
				{
					/* update the fx vol */
#ifdef JADEFSUION
					SND_FxLocalRequest(pst_MS->i_CoreId, pst_MS->f_WetVol, pst_MS->i_Mode, pst_MS->i_Delay, pst_MS->i_Feedback);
#else
				   SND_FxLocalRequest(pst_MS->i_CoreId, f_vol, pst_MS->i_Mode, pst_MS->i_Delay, pst_MS->i_Feedback);
#endif
				}
				else
				{
					/* desactivate the current fx */
					pst_MS->ui_MdfFlag &= ~MDF_Cte_SndFx_Activated;
				}
			}
			else
			{
				while(pNode)
				{
					pFxNode = (GEN_tdst_ModifierSoundFx *) pNode->p_Data;

					if(b_TestActiveVol(pFxNode, (OBJ_tdst_GameObject *) _pst_Obj, TRUE, &f_vol) && (f_vol > f_maxvol))
					{
						/* activate the current fx */
						pst_MS->ui_MdfFlag |= MDF_Cte_SndFx_Activated;
                        SND_FxLocalRequest(pst_MS->i_CoreId, pst_MS->f_WetVol, pst_MS->i_Mode, pst_MS->i_Delay, pst_MS->i_Feedback);
						f_maxvol = f_vol;
					}

					pNode = (MDF_tdst_Modifier*)pFxNode->p_Next;
				}
			}
		}
	}
	else
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    compute fx activation
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		if(pst_MS->ui_MdfFlag & MDF_Cte_SndFx_Activated)
		{
			if(b_TestActiveVol(pst_MS, (OBJ_tdst_GameObject *) _pst_Obj, FALSE, &f_vol))
			{
				/* desactivate the current fx */
				pst_MS->ui_MdfFlag &= ~MDF_Cte_SndFx_Activated;
			}
			else
			{
				/* update the fx vol */
#ifdef JADEFUSION
				SND_FxLocalRequest(pst_MS->i_CoreId, pst_MS->f_WetVol, pst_MS->i_Mode, pst_MS->i_Delay, pst_MS->i_Feedback);
#else
				SND_FxLocalRequest(pst_MS->i_CoreId, f_vol, pst_MS->i_Mode, pst_MS->i_Delay, pst_MS->i_Feedback);
#endif
			}
		}
		else
		{
			if(b_TestActiveVol(pst_MS, (OBJ_tdst_GameObject *) _pst_Obj, TRUE, &f_vol))
			{
				/* activate the current fx */
				pst_MS->ui_MdfFlag |= MDF_Cte_SndFx_Activated;
                SND_FxLocalRequest(pst_MS->i_CoreId, pst_MS->f_WetVol, pst_MS->i_Mode, pst_MS->i_Delay, pst_MS->i_Feedback);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL b_TestActiveVol
(
	GEN_tdst_ModifierSoundFx	*pst_MS,
	OBJ_tdst_GameObject			*pGO,
	BOOL						bInside,
	float						*_pf_Attenuation
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_dist;
	int					ai_Command[3];
	MATH_tdst_Vector	st_Vect;
	int					axis;
	float				af_Factor[3];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_MS->ui_MdfFlag & MDF_Cte_SndFx_Sphere)
	{
		f_dist = MATH_f_Distance
			(
				MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol),
				MATH_pst_GetTranslation(pGO->pst_GlobalMatrix)
			);

		if(f_dist >= pst_MS->af_Far[0])
			*_pf_Attenuation = 0.0f;
		else if(f_dist <= pst_MS->af_Distance[0])
			*_pf_Attenuation = 1.0f;
		else
			*_pf_Attenuation = (pst_MS->af_Far[0] - f_dist) / (pst_MS->af_Far[0] - pst_MS->af_Distance[0]);

		if(bInside && (f_dist < pst_MS->af_Far[0])) return TRUE;
		if(!bInside && (f_dist > (pst_MS->af_Far[0] + pst_MS->f_Delta))) return TRUE;
	}
	else
	{
		MATH_SubVector
		(
			&st_Vect,
			MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol),
			MATH_pst_GetTranslation(pGO->pst_GlobalMatrix)
		);

		for(axis = 0; axis < 3; axis++)
		{
			ai_Command[axis] = 0;

			switch(axis)
			{
			case 0:		f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetXAxis(pGO->pst_GlobalMatrix))); break;
			case 1:		f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetYAxis(pGO->pst_GlobalMatrix))); break;
			case 2:		f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetZAxis(pGO->pst_GlobalMatrix))); break;
			default:	f_dist = 0.0f; break;
			}

			if(f_dist >= pst_MS->af_Far[axis])
				af_Factor[axis] = 0.0f;
			else if(f_dist <= pst_MS->af_Distance[axis])
				af_Factor[axis] = 1.0f;
			else
				af_Factor[axis] = (pst_MS->af_Far[axis] - f_dist) / (pst_MS->af_Far[axis] - pst_MS->af_Distance[axis]);

			if(bInside && (f_dist < pst_MS->af_Far[axis])) ai_Command[axis] = 1;
			if(!bInside && (f_dist > (pst_MS->af_Far[axis] + pst_MS->f_Delta))) ai_Command[axis] = 1;
		}

		*_pf_Attenuation = af_Factor[0] * af_Factor[1] * af_Factor[2];

		if(bInside && (ai_Command[0] & ai_Command[1] & ai_Command[2])) return TRUE;
		if(!bInside && (ai_Command[0] || ai_Command[1] || ai_Command[2])) return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSoundFx_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSoundFx_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSoundFx	*pst_Dst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dst = (GEN_tdst_ModifierSoundFx *) _pst_Mod->p_Data;
	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGen;
	pst_Dst->ui_MdfFlag &= ~MDF_Cte_SndFx_Activated;
	pst_Dst->ui_MdfFlag &= ~MDF_Cte_SndFx_EdiDontDisplay;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEN_ModifierSoundFx_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSoundFx	*pst_Dst;
	char						*pc_Buf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Buf = _pc_Buffer;
	pst_Dst = (GEN_tdst_ModifierSoundFx *) _pst_Mod->p_Data;

	if(pst_Dst)
	{
		pst_Dst->ui_MdFVersion = LOA_ReadUInt(&pc_Buf);
		switch(pst_Dst->ui_MdFVersion)
		{
		case SND_Cte_MdFFxVersion:	break;
		default:					ERR_X_Warning(0, "[SND][MDFmodifier_SOUNDFX.c] bad SND FX MDF", NULL); break;
		}
	}

	/*$2- load all struct fields -------------------------------------------------------------------------------------*/

	pst_Dst->ui_MdfFlag = LOA_ReadUInt(&pc_Buf);
	pst_Dst->af_Distance[0] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Distance[1] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Distance[2] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->f_Delta = LOA_ReadFloat(&pc_Buf);
	pst_Dst->i_CoreId = LOA_ReadInt(&pc_Buf);
	pst_Dst->i_Mode = LOA_ReadInt(&pc_Buf);
	pst_Dst->i_Delay = LOA_ReadInt(&pc_Buf);
	pst_Dst->i_Feedback = LOA_ReadInt(&pc_Buf);
	pst_Dst->f_WetVol = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Far[0] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Far[1] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->af_Far[2] = LOA_ReadFloat(&pc_Buf);
	pst_Dst->i_NetIdx = LOA_ReadInt(&pc_Buf);
	pst_Dst->p_Next = (void *) LOA_ReadInt_Ed(&pc_Buf, 0);
	pst_Dst->p_Next = NULL;

	LOA_ReadCharArray_Ed(&pc_Buf, NULL, MDF_Cte_SndFx_ReserveSize); /* skip asz_Reserve */

	/*$2- network ----------------------------------------------------------------------------------------------------*/

	if(pst_Dst->i_NetIdx != -1)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		int					id;
		MDF_tdst_Modifier	*pNode;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		id = SND_i_FxNetworkGet(pst_Dst->i_NetIdx);
		ERR_X_Assert(id != -1);

		pNode = (MDF_tdst_Modifier*)SND_gax_FxNetwork[id].p_Root;

		if(pst_Dst->ui_MdfFlag & MDF_Cte_SndFx_NetMaster)
		{
			SND_gax_FxNetwork[id].p_Root = _pst_Mod;

			/*
			 * ( (GEN_tdst_ModifierSoundFx*)SND_gax_FxNetwork[id].p_Root->p_Data )->p_Next =
			 * pNode;
			 */
			((GEN_tdst_ModifierSoundFx *) ((MDF_tdst_Modifier *) SND_gax_FxNetwork[id].p_Root)->p_Data)->p_Next = pNode;
		}
		else if(pst_Dst->ui_MdfFlag & MDF_Cte_SndFx_NetSlave)
		{
			if(pNode)
			{
				((GEN_tdst_ModifierSoundFx *) _pst_Mod->p_Data)->p_Next = ((GEN_tdst_ModifierSoundFx *) pNode->p_Data)->p_Next;
				((GEN_tdst_ModifierSoundFx *) pNode->p_Data)->p_Next = _pst_Mod;
			}
			else
				SND_gax_FxNetwork[id].p_Root = _pst_Mod;
		}
		else
		{
			pst_Dst->i_NetIdx = -1;
		}
	}

	/*$2- check data -------------------------------------------------------------------------------------------------*/

	if(pst_Dst->af_Distance[0] > pst_Dst->af_Far[0]) pst_Dst->af_Distance[0] = pst_Dst->af_Far[0];
	if(pst_Dst->af_Distance[1] > pst_Dst->af_Far[1]) pst_Dst->af_Distance[1] = pst_Dst->af_Far[1];
	if(pst_Dst->af_Distance[2] > pst_Dst->af_Far[2]) pst_Dst->af_Distance[2] = pst_Dst->af_Far[2];

	ERR_X_Warning(pst_Dst->af_Distance[0] >= 0.0f, "[SND][MDFmodifier_SOUNDFX.c] bad distance value", NULL);
	ERR_X_Warning(pst_Dst->af_Distance[1] >= 0.0f, "[SND][MDFmodifier_SOUNDFX.c] bad distance value", NULL);
	ERR_X_Warning(pst_Dst->af_Distance[2] >= 0.0f, "[SND][MDFmodifier_SOUNDFX.c] bad distance value", NULL);
	ERR_X_Warning(pst_Dst->f_Delta >= 0.0f, "[SND][MDFmodifier_SOUNDFX.c] bad delta value", NULL);

	switch(pst_Dst->i_Mode)
	{
	case SND_Cte_FxMode_Off:
	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_City:
	case SND_Cte_FxMode_Mountains:
		break;

	default:
		pst_Dst->i_Mode = SND_Cte_FxMode_Off;
		break;
	}

	ERR_X_Warning
	(
		(0 <= pst_Dst->i_Delay) && (pst_Dst->i_Delay <= 1000),
		"[SND][MDFmodifier_SOUNDFX.c] bad delay value",
		NULL
	);
	ERR_X_Warning
	(
		(0 <= pst_Dst->i_Feedback) && (pst_Dst->i_Feedback <= 100),
		"[SND][MDFmodifier_SOUNDFX.c] bad feedback value",
		NULL
	);
	ERR_X_Warning
	(
		(0.0f <= pst_Dst->f_WetVol) && (pst_Dst->f_WetVol <= 1.0f),
		"[SND][MDFmodifier_SOUNDFX.c] bad wetvol value",
		NULL
	);

	/*$2--------------------------------------------------------------------------------------------------------------*/

	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGen;

	return(pc_Buf - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSoundFx_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSoundFx	st_Tmp;
	GEN_tdst_ModifierSoundFx	*pst_Src;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Src = (GEN_tdst_ModifierSoundFx *) _pst_Mod->p_Data;

	if(pst_Src)
	{
		if(pst_Src->af_Distance[0] > pst_Src->af_Far[0]) pst_Src->af_Distance[0] = pst_Src->af_Far[0];
		if(pst_Src->af_Distance[1] > pst_Src->af_Far[1]) pst_Src->af_Distance[1] = pst_Src->af_Far[1];
		if(pst_Src->af_Distance[2] > pst_Src->af_Far[2]) pst_Src->af_Distance[2] = pst_Src->af_Far[2];

		L_memcpy(&st_Tmp, pst_Src, sizeof(GEN_tdst_ModifierSoundFx));

		/* clear dyn fields */
		st_Tmp.ui_MdfFlag &= ~MDF_Cte_SndFx_Activated;
		st_Tmp.ui_MdfFlag &= ~MDF_Cte_SndFx_EdiDontDisplay;

		SAV_Buffer(&st_Tmp, sizeof(GEN_tdst_ModifierSoundFx));
	}
}

int GEN_ModifierSoundFx_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src )
{
    GEN_ModifierSoundFx_Load(_pst_Dst, (char* )_pst_Src->p_Data);
	return sizeof( GEN_tdst_ModifierSoundFx );
}

#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
