/*$T MDFmodifier_GEN.c GC 1.138 03/02/04 12:37:26 */


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
#include "MoDiFier/MDFmodifier_GEN.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/BASsys.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDmacros.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDmusic.h"
#include "SouND/Sources/SNDambience.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDinterface.h"
#include "SouND/Sources/SNDbank.h"
#include "SouND/Sources/SNDvolume.h"

#include "ENGine/Sources/OBJects/OBJorient.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAdefs.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAread.h"
#include "SDK/Sources/BIGfiles/BIGfat.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#include "../Main/WinEditors/Sources/EDIpaths.h"
#include "LINks/LINKtoed.h"
#include "EDItors/Sources/SOuNd/SONmsg.h"
#endif
#include "AIinterp/Sources/Events/EVEplay.h"

#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
#define M_EdiDisplayRedWarningGAO(_strtmp, _txt, _gaoname, _gaokey) \
	{ \
		sprintf(_strtmp, "[SND(MdF)] %s, GAO \"%s\" (%x)", _txt, _gaoname, _gaokey); \
		ERR_X_Warning(0, _strtmp, NULL); \
	}
#else
#define M_EdiDisplayRedWarningGAO(_strtmp, _txt, _gaoname, _gaokey)
#endif

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	SND_tdst_MdfList_
{
	ULONG					ul_Size;
	GEN_tdst_ModifierSound	**ast_MdF;
} SND_tdst_MdfList;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern BOOL				SND_gb_EdiPause;
extern float			TIM_gf_SpeedFactor;
static SND_tdst_MdfList SND_gst_MdFList;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

BOOL		MDF_b_SndIsInPrefetchVolume(GEN_tdst_ModifierSound *pst_MS);
static void SND_AddMdF(GEN_tdst_ModifierSound *pMdF);
static void SND_DelMdF(GEN_tdst_ModifierSound *pMdF);

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_ApplyGen(struct OBJ_tdst_GameObject_ *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Modifier;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
		if((pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers) != NULL)
		{
			do
			{
#ifdef ACTIVE_EDITORS
				if (pst_Modifier->i->pfnv_Desactivate != NULL)
					pst_Modifier->i->pfnv_Desactivate(pst_Modifier, (GEO_tdst_Object *) _pst_GO);
#endif // ACTIVE_EDITORS

				if(pst_Modifier->ul_Flags & MDF_C_Modifier_Inactive) continue;

                if(pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyGen)
					pst_Modifier->i->pfnv_Apply(pst_Modifier, (GEO_tdst_Object *) _pst_GO);
			} while((pst_Modifier = pst_Modifier->pst_Next) != NULL);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MDF_UnApplyGen(struct OBJ_tdst_GameObject_ *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Modifier;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
		if((pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers) != NULL)
		{
			do
			{
				if(pst_Modifier->ul_Flags & MDF_C_Modifier_Inactive) continue;
				if(pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyGen)
					pst_Modifier->i->pfnv_Unapply(pst_Modifier, (GEO_tdst_Object *) _pst_GO);
			} while((pst_Modifier = pst_Modifier->pst_Next) != NULL);
		}
	}
}

/*
 =======================================================================================================================
    called by the 1st part of ENG_OneWorldEngineCall
 =======================================================================================================================
 */
void MDF_MainApplyGen(TAB_tdst_PFtable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		MDF_ApplyGen(pst_GO);
	}
}

/*
 =======================================================================================================================
    called by the last part of ENG_OneWorldEngineCall
 =======================================================================================================================
 */
void MDF_MainUnApplyGen(TAB_tdst_PFtable *_pst_Table)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Table);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Table);

	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		MDF_UnApplyGen(pst_GO);
	}
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    sound modifier
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSound_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSound	*pst_SndMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SndMdF = (GEN_tdst_ModifierSound *) MEM_p_Alloc(sizeof(GEN_tdst_ModifierSound));

	if(pst_SndMdF)
	{
		L_memset(pst_SndMdF, 0, sizeof(GEN_tdst_ModifierSound));
		pst_SndMdF->ui_Version = SND_Cte_MdFVersion;

		pst_SndMdF->ui_EdiFlags = 0;
		pst_SndMdF->ui_Id = -1;

		pst_SndMdF->ui_FileKey = BIG_C_InvalidKey;
		pst_SndMdF->i_SndIdx = -1;
		pst_SndMdF->i_SndInstance = -1;

		pst_SndMdF->f_PrefetchDistance = 20.0f;

		pst_SndMdF->ui_ConfigFlags = 0;
		pst_SndMdF->ui_CurrentFlags = 0;
		MDF_M_SndSetState(pst_SndMdF, MDF_Cte_SndStopped);

		pst_SndMdF->f_Delay = 0.0f;
		pst_SndMdF->f_DeltaFar = SND_Cte_DefaultDeltaFar;
		pst_SndMdF->i_SndTrack = -1;

		_pst_Mod->p_Data = pst_SndMdF;

		/* duplicate operation ? */
		if(p_Data)
		{
			_pst_Mod->i->pfnul_Load(_pst_Mod, (char *) p_Data);
			pst_SndMdF->i_SndInstance = -1;
			pst_SndMdF->ui_CurrentFlags = 0;
			pst_SndMdF->ui_EdiFlags = 0;
			MDF_M_SndSetState(pst_SndMdF, MDF_Cte_SndStopped);
		}

        if(SND_gst_Params.l_Available)
		    SND_AddMdF(pst_SndMdF);
	}
	else
		_pst_Mod->p_Data = 0;
	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGen;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSound_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSound	*pst_MS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_MS = (GEN_tdst_ModifierSound *) _pst_Mod->p_Data;

	if(pst_MS)
	{
		SND_DelMdF(pst_MS);

		if(pst_MS->i_SndIdx != -1)
		{
			SND_StreamFlushOneSound(pst_MS->i_SndIdx, TRUE);
			SND_DeleteByIndex(pst_MS->i_SndIdx);
		}

		MEM_Free(_pst_Mod->p_Data);
		_pst_Mod->p_Data = NULL;
	}

	_pst_Mod->ul_Flags = MDF_C_Modifier_Inactive;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSound_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSound	*pst_MS;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_MS = (GEN_tdst_ModifierSound *) _pst_Mod->p_Data;
	if(pst_MS->i_SndIdx < 0) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    check if instance was freed during last frame
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(pst_MS->i_SndInstance >= 0)
	{
		pst_SI = SND_gst_Params.dst_Instance + pst_MS->i_SndInstance;
		if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0)
			pst_MS->i_SndInstance = -1;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    create an instance
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(pst_MS->i_SndInstance == -1)
	{
		pst_MS->i_SndInstance = SND_l_Request(NULL, pst_MS->i_SndIdx);
#ifdef ACTIVE_EDITORS
		if(pst_MS->i_SndInstance < 0) pst_MS->i_SndInstance = -2;	/* lock the request */
#endif
		if(pst_MS->ui_ConfigFlags & MDF_Cte_SndPlayInVolume)
		{
			SND_SetFlags
			(
				NULL,
				pst_MS->i_SndInstance,
				SND_Cul_SF_UseExtFlag | SND_Cul_ESF_AutoPlayInsideFar | SND_Cul_ESF_AutoStopOutsideFar
			);
		}

		if(pst_MS->ui_ConfigFlags & MDF_Cte_SndPlayRandomPosition)
			SND_SetFlags(NULL, pst_MS->i_SndInstance, SND_Cul_SF_UseExtFlag | SND_Cul_ESF_RandomPlay);


        SND_SetObject(_pst_Mod->pst_GO, pst_MS->i_SndInstance);
        SND_i_InstanceSetf(pst_MS->i_SndInstance, SND_Cte_SetInstDeltaFar, pst_MS->f_DeltaFar);
        
        if(pst_MS->ui_ConfigFlags & MDF_Cte_SndPlayOnTrack)
             SND_InstTrackSet(pst_MS->i_SndInstance, pst_MS->i_SndTrack);

		if(pst_MS->ui_ConfigFlags & MDF_Cte_SndPlayOnInit) SND_PlayMdF(pst_MS);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else if(pst_MS->i_SndInstance >= 0)
	{
		switch(MDF_M_SndGetState(pst_MS))
		{

		/*$1- nothing todo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_Cte_SndPaused:
			break;

		/*$1- check if auto-stopped ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_Cte_SndPlaying:
#ifdef ACTIVE_EDITORS
			if(SND_gb_EdiPause) break;
#endif
			if(!SND_i_IsPlaying(pst_MS->i_SndInstance))
			{
				MDF_M_SndSetState(pst_MS, MDF_Cte_SndStopped);
				SND_Stop(pst_MS->i_SndInstance);
			}
			break;

		/*$1- prefech + auto play ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case MDF_Cte_SndStopped:
			if(pst_MS->ui_ConfigFlags & MDF_Cte_SndPrefetchAuto)
			{
				if((pst_MS->ui_CurrentFlags & MDF_Cte_SndPrefetched) == 0)
				{
					if(MDF_b_SndIsInPrefetchVolume(pst_MS))
					{
						SND_StreamPrefetchOneSound(pst_MS->i_SndIdx, SND_Cte_StreamPrefetchAsync);
						pst_MS->ui_CurrentFlags |= MDF_Cte_SndPrefetched;
					}
				}
			}

			if(pst_MS->ui_ConfigFlags & MDF_Cte_SndPlayInVolume)
			{
				if(SND_b_IsInActiveVolume(pst_MS->i_SndInstance)) 
				{
					_pst_Mod->ul_Flags &= ~MDF_C_Modifier_Inactive;
					SND_PlayMdF(pst_MS);
				}
				else
				{
					/* Reinit flags so that sound will be played on next entry in active volume */
					SND_ReinitMdFPlayParam(pst_MS->i_SndInstance);
				}

			}
			break;

		/*$1- bad state !! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		default:
			ERR_X_Assert(0);
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL MDF_b_SndIsInPrefetchVolume(GEN_tdst_ModifierSound *pst_MS)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float					y;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.pst_RefForVol) return FALSE;

	SND_M_GetInstanceOrReturn(pst_MS->i_SndInstance, pst_SI, FALSE);

	/* need pos */
	if(!pst_SI->pst_GlobalPos) return FALSE;

	y = MATH_f_Distance(MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol), pst_SI->pst_GlobalPos);

	if(y < pst_MS->f_PrefetchDistance) return TRUE;

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSound_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSound_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSound	*pst_Dst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dst = (GEN_tdst_ModifierSound *) _pst_Mod->p_Data;

	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGen;

	if(pst_Dst->i_SndInstance != -1)
	{
		SND_Release(pst_Dst->i_SndInstance);
		pst_Dst->i_SndInstance = -1;
	}

	pst_Dst->ui_CurrentFlags = 0;
	MDF_M_SndSetState(pst_Dst, MDF_Cte_SndStopped);

	if(pst_Dst->ui_ConfigFlags & MDF_Cte_SndPrefetchStatic)
	{
		SND_StreamPrefetchOneSound(pst_Dst->i_SndIdx, SND_Cte_StreamPrefetchSync);
	}

#ifdef ACTIVE_EDITORS
    if(pst_Dst->i_SndIdx == -1)
	{
        pst_Dst->i_SndIdx = SND_l_AddSModifier(pst_Dst->ui_FileKey);
        LOA_Resolve();
    }
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEN_ModifierSound_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSound	*pst_Dst;
	BIG_KEY					ul_key;
#ifdef ACTIVE_EDITORS
	char					az_msg[1024];
	ULONG					ul_Fat, ulFat2;
	char					az_name[256];
#endif
	BOOL					b_3D;
	char					*pc_Buf;
	UINT					ui_Version;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Buf = _pc_Buffer;
	b_3D = FALSE;
	pst_Dst = (GEN_tdst_ModifierSound *) _pst_Mod->p_Data;
	ul_key = LOA_ul_GetCurrentKey();

#ifdef ACTIVE_EDITORS
	sprintf(az_name, "0x%x", ul_key);
	ul_Fat = BIG_ul_SearchKeyToFat(ul_key);
#if !defined(XML_CONV_TOOL)
	if(ul_Fat != BIG_C_InvalidIndex) sprintf(az_name, "%s", BIG_NameFile(ul_Fat));
#endif
#endif
	ui_Version = LOA_ReadUInt(&pc_Buf);
	if(pst_Dst)
	{
		if(ui_Version == SND_Cte_MdFVersion)
		{
			LOA_ReadUInt_Ed(&pc_Buf, &pst_Dst->ui_EdiFlags);
			pst_Dst->ui_Id = LOA_ReadUInt(&pc_Buf);
			pst_Dst->ui_FileKey = LOA_ReadUInt(&pc_Buf);
			LOA_ReadInt_Ed(&pc_Buf, &pst_Dst->i_SndIdx);
			LOA_ReadInt_Ed(&pc_Buf, &pst_Dst->i_SndInstance);
			pst_Dst->f_PrefetchDistance = LOA_ReadFloat(&pc_Buf);;
			pst_Dst->ui_ConfigFlags = LOA_ReadUInt(&pc_Buf);;
			pst_Dst->ui_CurrentFlags = LOA_ReadUInt(&pc_Buf);;
			pst_Dst->f_Delay = LOA_ReadFloat(&pc_Buf);;
			pst_Dst->f_DeltaFar = LOA_ReadFloat(&pc_Buf);;
			pst_Dst->i_SndTrack = LOA_ReadInt(&pc_Buf);;
			LOA_ReadCharArray_Ed(&pc_Buf, NULL, MDF_Cte_SndSizeOfReserve);	/* skip asz_Reserve */

			pst_Dst->ui_EdiFlags = 0;
			pst_Dst->i_SndInstance = -1;

#ifdef ACTIVE_EDITORS
			ulFat2 = BIG_ul_SearchKeyToFat(pst_Dst->ui_FileKey);
#if !defined(XML_CONV_TOOL)
			if(!BIG_b_IsFileExtension(ulFat2, EDI_Csz_ExtSModifier))
			{
				M_EdiDisplayRedWarningGAO(az_msg, "Bad file type ref. by sound MdF", az_name, ul_key);
			}
#endif
#endif
			pst_Dst->i_SndIdx = SND_l_AddSModifier(pst_Dst->ui_FileKey);

			_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGen;
		}
		else
		{
			M_EdiDisplayRedWarningGAO(az_msg, "Bad verion of Sound modifier detected in your BigFile", az_name, ul_key);
		}
	}

	return(pc_Buf - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEN_ModifierSound_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEN_tdst_ModifierSound	st_Tmp;
	GEN_tdst_ModifierSound	*pst_Src;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Src = (GEN_tdst_ModifierSound *) _pst_Mod->p_Data;

	if(pst_Src)
	{
		L_memcpy(&st_Tmp, pst_Src, sizeof(GEN_tdst_ModifierSound));

		/* clear dyn fields */
		st_Tmp.i_SndIdx = -1;
		st_Tmp.i_SndInstance = -1;
		st_Tmp.ui_CurrentFlags = 0;
		st_Tmp.ui_EdiFlags = 0;

		SAV_Buffer(&st_Tmp, sizeof(GEN_tdst_ModifierSound));
	}
}

int GEN_ModifierSound_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src )
{
    GEN_ModifierSound_Load(_pst_Dst, (char* )_pst_Src->p_Data);
	return sizeof( GEN_tdst_ModifierSound );
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_PlayMdF(GEN_tdst_ModifierSound *pst_SndMdF)
{
	if(!SND_gst_Params.l_Available) return;
	if(pst_SndMdF->i_SndInstance < 0) return;

	switch(MDF_M_SndGetState(pst_SndMdF))
	{

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case MDF_Cte_SndStopped:
		/* play delay */
		if(pst_SndMdF->f_Delay)
			SND_i_InstanceSetf(pst_SndMdF->i_SndInstance, SND_Cte_SetInstDelay, pst_SndMdF->f_Delay);

		/* =>play */
		if((pst_SndMdF->ui_ConfigFlags & MDF_Cte_SndPlayOnTrack) && (pst_SndMdF->ui_ConfigFlags & MDF_Cte_SndPlayWhenTrackIsFree))
		{
		    if(SND_i_IsTrackPlaying(pst_SndMdF->i_SndTrack)) break;
		}

		if(pst_SndMdF->ui_ConfigFlags & MDF_Cte_SndPlayLoop)
		{
			SND_PlaySoundLooping(pst_SndMdF->i_SndInstance, -1);
		}
		else
		{
			if (pst_SndMdF->ui_ConfigFlags & MDF_Cte_SndPlayInVolume)
				SND_PlaySoundInVolume(pst_SndMdF->i_SndInstance);
			else 
				SND_PlaySound(pst_SndMdF->i_SndInstance);
		}

		MDF_M_SndSetState(pst_SndMdF, MDF_Cte_SndPlaying);
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case MDF_Cte_SndPlaying:
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case MDF_Cte_SndPaused:
		SND_Pause(pst_SndMdF->i_SndInstance);
		MDF_M_SndSetState(pst_SndMdF, MDF_Cte_SndPlaying);
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	default:
		ERR_X_Assert(0);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StopMdF(GEN_tdst_ModifierSound *pst_SndMdF)
{
	if(!SND_gst_Params.l_Available) return;
	if(pst_SndMdF->i_SndInstance < 0) return;

	switch(MDF_M_SndGetState(pst_SndMdF))
	{
	case MDF_Cte_SndStopped:
		break;

	case MDF_Cte_SndPaused:
		SND_Stop(pst_SndMdF->i_SndInstance);
		MDF_M_SndSetState(pst_SndMdF, MDF_Cte_SndStopped);
		break;

	case MDF_Cte_SndPlaying:
		SND_StopRq(pst_SndMdF->i_SndInstance);
		MDF_M_SndSetState(pst_SndMdF, MDF_Cte_SndStopped);
		break;

	default:
		ERR_X_Assert(0);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_PauseMdF(GEN_tdst_ModifierSound *pst_SndMdF)
{
	if(!SND_gst_Params.l_Available) return;
	if(pst_SndMdF->i_SndInstance < 0) return;

	switch(MDF_M_SndGetState(pst_SndMdF))
	{
	case MDF_Cte_SndPaused:
	case MDF_Cte_SndStopped:
		break;

	case MDF_Cte_SndPlaying:
		SND_Pause(pst_SndMdF->i_SndInstance);
		MDF_M_SndSetState(pst_SndMdF, MDF_Cte_SndPaused);
		break;

	default:
		ERR_X_Assert(0);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_PrefetchMdF(GEN_tdst_ModifierSound *pst_SndMdF)
{
	if(!SND_gst_Params.l_Available) return;
	if(pst_SndMdF->i_SndIdx < 0) return;
	SND_StreamPrefetchOneSound(pst_SndMdF->i_SndIdx, SND_Cte_StreamPrefetchAsync);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_FlushMdF(GEN_tdst_ModifierSound *pst_SndMdF)
{
	if(!SND_gst_Params.l_Available) return;
	if(pst_SndMdF->i_SndIdx < 0) return;
	SND_StreamFlushOneSound(pst_SndMdF->i_SndIdx, TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ResolveRefsForMdF(MDF_tdst_Modifier *_pst_Mod)
{
	/*
	 * not used today £
	 * called just after loading
	 */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_AddMdF(GEN_tdst_ModifierSound *pMdF)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	for(i = 0; i < SND_gst_MdFList.ul_Size; i++)
	{
		if(SND_gst_MdFList.ast_MdF[i]) continue;
		
		SND_gst_MdFList.ast_MdF[i] = pMdF;
		return;		
	}
	
	if(SND_gst_MdFList.ul_Size)
	{
		SND_gst_MdFList.ast_MdF = (GEN_tdst_ModifierSound **) MEM_p_Realloc
			(
				SND_gst_MdFList.ast_MdF,
				(200 + SND_gst_MdFList.ul_Size) * sizeof(GEN_tdst_ModifierSound *)
			);
	}
	else
	{
		SND_gst_MdFList.ast_MdF = (GEN_tdst_ModifierSound**)MEM_p_Alloc(200 * sizeof(GEN_tdst_ModifierSound *));
	}
	
	L_memset(&SND_gst_MdFList.ast_MdF[SND_gst_MdFList.ul_Size], 0, 200 * sizeof(GEN_tdst_ModifierSound *));
	SND_gst_MdFList.ast_MdF[SND_gst_MdFList.ul_Size] = pMdF;
	SND_gst_MdFList.ul_Size += 200;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_DelMdF(GEN_tdst_ModifierSound *pMdF)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	for(i = 0; i < SND_gst_MdFList.ul_Size; i++)
	{
		if(SND_gst_MdFList.ast_MdF[i] == pMdF)
		{
			SND_gst_MdFList.ast_MdF[i] = NULL;
			break;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_InitMdFModule(void)
{
	L_memset(&SND_gst_MdFList, 0, sizeof(SND_gst_MdFList));
    if(SND_gst_Params.l_Available)
	    SND_AddMdF(NULL);	/* force allocation of the array */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_CloseMdFModule(void)
{
	if(SND_gst_MdFList.ast_MdF) MEM_Free(SND_gst_MdFList.ast_MdF);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ResolveStaticPrefetchForSpecialMdF(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					i;
	GEN_tdst_ModifierSound	*pst_MS;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	for(i = 0; i < SND_gst_MdFList.ul_Size; i++)
	{
		pst_MS = SND_gst_MdFList.ast_MdF[i];

		if(!pst_MS) continue;
		if(!(pst_MS->ui_ConfigFlags & MDF_Cte_SndPrefetchStatic)) continue;
		if(pst_MS->i_SndIdx < 0) continue;

		SND_StreamPrefetchOneSound(pst_MS->i_SndIdx, SND_Cte_StreamPrefetchSync);
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
