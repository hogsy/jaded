/*$T SNDmodifier.c GC 1.138 09/15/04 10:49:02 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- SDK ------------------------------------------------------------------------------------------------------------*/

#ifdef PSX2_TARGET
#define _WINSOCK_H
#endif
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "TIMer/TIMdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "LINks/LINKtoed.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#include "../Main/WinEditors/Sources/EDIpaths.h"
#endif

/*$2- SND ------------------------------------------------------------------------------------------------------------*/

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDmusic.h"
#include "SouND/Sources/SNDambience.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDinterface.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDinsert.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

SND_tdst_SModifier	SND_gast_SModifierModel[SND_Cte_SModifierModelNumber];

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

static void					SND_SModifierSetCurrY
							(
								SND_tdst_SoundInstance		*_pst_SI,
								SND_tdst_InsertState		*pInsertState,
								SND_tdst_Insert				*pInsert,
								SND_tdst_SModifierExtPlayer *pExtPlayer,
								float						fY
							);
static float				SND_f_SModifierGetCurrX
							(
								SND_tdst_SoundInstance	*_pst_SI,
								SND_tdst_InsertState	*pInsertState,
								SND_tdst_Insert			*pInsert
							);
static SND_tdst_SModifier	*SND_pst_SModifierInsertOneSModifier(SND_tdst_SModifier **, int, void *);

/*$2- ext player interface -------------------------------------------------------------------------------------------*/

static void SND_SModifierExtPlayerInit(SND_tdst_SModifier *, void *);
static int	SND_i_SModifierExtPlayerNotifyPlay(SND_tdst_SModifier *, SND_tdst_SoundInstance *);
static int	SND_i_SModifierExtPlayerNotifyStop(SND_tdst_SModifier *, SND_tdst_SoundInstance *);
static int	SND_i_SModifierExtPlayerNotifyPause(SND_tdst_SModifier *, SND_tdst_SoundInstance *);
static void SND_SModifierExtPlayerUpdate(SND_tdst_SModifier *_pst_Ext, SND_tdst_SoundInstance *pst_SI);
static void SND_SModifierExtPlayerDestroy(SND_tdst_SModifier *, void *, int _i_DataSize, BOOL);
static void SND_SModifierExtPlayerApplyOnInstance
			(
				SND_tdst_SModifier *,
				void *,
				int _i_DataSize,
				SND_tdst_SoundInstance *
			);
static void SND_SModifierExtPlayerLoad(SND_tdst_SModifier **_pp_SMd, char **_ppc);
static void SND_SModifierExtPlayerSave(SND_tdst_SModifier *pSMd);

/*$2- generic interface ----------------------------------------------------------------------------------------------*/

static void SND_SModifierGenericDestroy(SND_tdst_SModifier *, void *, int _i_DataSize, BOOL b_Instance);

/*$4
 ***********************************************************************************************************************
    GENERIC
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SModifierInitModule(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(SND_gast_SModifierModel, 0, SND_Cte_SModifierModelNumber * sizeof(SND_tdst_SModifier));

	/*$1- extended player ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].i = (SND_tdst_SModifierInterface *) MEM_p_Alloc(sizeof(SND_tdst_SModifierInterface));
	L_memset(SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].i, 0, sizeof(SND_tdst_SModifierInterface));

	SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].i->id = SND_Cte_SModifierExtPlayer;
	SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].i->pfv_Destroy = SND_SModifierExtPlayerDestroy;
	SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].i->pfv_ApplyOnInstance = SND_SModifierExtPlayerApplyOnInstance;
	SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].i->pfi_NotifyInstancePlay = SND_i_SModifierExtPlayerNotifyPlay;
	SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].i->pfi_NotifyInstanceStop = SND_i_SModifierExtPlayerNotifyStop;
	SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].i->pfi_NotifyInstancePause = SND_i_SModifierExtPlayerNotifyPause;
	SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].i->pfv_Update = SND_SModifierExtPlayerUpdate;

	SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].pv_Data = MEM_p_Alloc(sizeof(SND_tdst_SModifierExtPlayer));
	SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].ui_DataSize = sizeof(SND_tdst_SModifierExtPlayer);

	pst_ExtPlay = (SND_tdst_SModifierExtPlayer *) SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].pv_Data;
/**/
	pst_ExtPlay->ul_SndKey = -1;
	pst_ExtPlay->ul_SndFlags = SND_Cul_SF_DynPan|SND_Cul_SF_DynVolSpheric;
	pst_ExtPlay->i_SndIndex = -1;
/**/
	pst_ExtPlay->ui_Version = SND_Cte_SModifierExtPlayerVersion;
/**/
	pst_ExtPlay->ui_SndExtFlags = 0;
	pst_ExtPlay->p_Template = NULL;
/**/
	pst_ExtPlay->ui_PlayerFlag = SND_Cte_SModifierFxA;
/**/
	pst_ExtPlay->f_DryVol = 1.0f;
	pst_ExtPlay->f_DryVol_FactMin = 0.90f;
	pst_ExtPlay->f_DryVol_FactMax = 1.0f;
	pst_ExtPlay->f_FxVolRight = 0.5f;
	pst_ExtPlay->f_FxVolLeft = 0.5f;
/**/
	pst_ExtPlay->ul_FadeInKey = -1;
	pst_ExtPlay->pst_FadeIn = NULL;
	pst_ExtPlay->ul_FadeOutKey = -1;
	pst_ExtPlay->pst_FadeOut = NULL;
/**/
	pst_ExtPlay->i_Pan = 0;
	pst_ExtPlay->i_Span = 0;
	pst_ExtPlay->f_MinPan = 0.0f;
/**/
	pst_ExtPlay->f_FreqCoef = 0.f;
	pst_ExtPlay->f_Freq_FactMin = 0.95f;
	pst_ExtPlay->f_Freq_FactMax = 1.0f;
	pst_ExtPlay->f_Doppler = 0.0f;
/**/
	pst_ExtPlay->af_Near[0] = 2.0f;
	pst_ExtPlay->af_Near[1] = 2.0f;
	pst_ExtPlay->af_Near[2] = 2.0f;
	pst_ExtPlay->af_Far[0] = 12.0f;
	pst_ExtPlay->af_Far[1] = 12.0f;
	pst_ExtPlay->af_Far[2] = 12.0f;
	pst_ExtPlay->af_MiddleBlend[0] = 0.33f;
	pst_ExtPlay->af_MiddleBlend[1] = 0.33f;
	pst_ExtPlay->af_MiddleBlend[2] = 0.33f;
	pst_ExtPlay->f_FarCoeff = 0.0f;
	pst_ExtPlay->f_MiddleCoeff = 0.33f;
	pst_ExtPlay->f_CylinderHeight = 0.0f;
/**/
	pst_ExtPlay->st_PlayList.us_Size = 0;
	pst_ExtPlay->st_PlayList.us_Flags = 0;
	pst_ExtPlay->st_PlayList.aul_FlagList = NULL;
	pst_ExtPlay->st_PlayList.aul_KeyList = NULL;
	pst_ExtPlay->st_PlayList.al_IdxList = NULL;
/**/
	pst_ExtPlay->st_InsertList.us_Size = 0;
	pst_ExtPlay->st_InsertList.us_Flags = 0;
	pst_ExtPlay->st_InsertList.app_PtrList = NULL;
	pst_ExtPlay->st_InsertList.aul_KeyList = NULL;
	pst_ExtPlay->st_InsertList.ast_State = NULL;
/**/
	L_memset(pst_ExtPlay->ac_Reserved, 0, SND_Cte_SModifierExtPlayerReservedSize * sizeof(char));

    SND_M_SModifierSetGroup(pst_ExtPlay, SND_Cte_SModifierGroupSFX);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SModifierCloseModule(void)
{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_Free(SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].pv_Data);
	MEM_Free(SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].i);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(SND_gast_SModifierModel, 0, SND_Cte_SModifierModelNumber * sizeof(SND_tdst_SModifier));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static SND_tdst_SModifier *SND_pst_SModifierInsertOneSModifier
(
	SND_tdst_SModifier	**_ppst_Ext,
	int					_i_Id,
	void				*_pv_Data
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifier	*pst_SModifier;
	SND_tdst_SModifier	*pst_Previous;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Id >= SND_Cte_SModifierModelNumber) return(*_ppst_Ext);
	if(_i_Id < 0) return(*_ppst_Ext);

	pst_Previous = *_ppst_Ext;

	while(pst_Previous && pst_Previous->pst_NextSModifier)
	{
		pst_Previous = pst_Previous->pst_NextSModifier;
	}

	pst_SModifier = (SND_tdst_SModifier *) MEM_p_Alloc(sizeof(SND_tdst_SModifier));
	L_memset(pst_SModifier, 0, sizeof(SND_tdst_SModifier));

	pst_SModifier->i = SND_gast_SModifierModel[_i_Id].i;
	pst_SModifier->ul_UserCount = 1;

	SND_SModifierExtPlayerInit(pst_SModifier, _pv_Data);

	if(pst_Previous)
		pst_Previous->pst_NextSModifier = pst_SModifier;
	else
		*_ppst_Ext = pst_SModifier;

	return pst_SModifier;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SModifier *SND_pst_SModifierGet(SND_tdst_SModifier *_pst_Ext, int _i_Id, int _i_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifier	*pst_SModifier;
	int					idx = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Id >= SND_Cte_SModifierModelNumber) return NULL;
	if(_i_Id < 0) return NULL;

	pst_SModifier = _pst_Ext;

	while(pst_SModifier && pst_SModifier->i)
	{
		if(pst_SModifier->i->id == _i_Id)
		{
			if(idx == _i_Index) return pst_SModifier;
			idx++;
		}

		pst_SModifier = pst_SModifier->pst_NextSModifier;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SModifierDeleteOneSModifier(SND_tdst_SModifier **_ppst_Ext, int _i_Id, int _i_idx, BOOL b_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifier	*pst_SModifier;
	SND_tdst_SModifier	*pst_SModifierPrec;
	int					idx = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SModifier = *_ppst_Ext;
	pst_SModifierPrec = NULL;

	while(pst_SModifier && pst_SModifier->i)
	{
		if(pst_SModifier->i->id == _i_Id)
		{
			if((_i_idx == -1) || (idx == _i_idx))
			{
				/* destroy SModifier */
				pst_SModifier->i->pfv_Destroy
					(
						pst_SModifier,
						pst_SModifier->pv_Data,
						pst_SModifier->ui_DataSize,
						b_Instance
					);

				if(pst_SModifierPrec)
				{
					pst_SModifierPrec->pst_NextSModifier = pst_SModifier->pst_NextSModifier;
				}
				else if(pst_SModifier->pst_NextSModifier)
				{
					*_ppst_Ext = pst_SModifier->pst_NextSModifier;
				}
				else
				{
					*_ppst_Ext = NULL;
				}

				MEM_Free(pst_SModifier);

				if(_i_idx == -1)
				{
					pst_SModifier = *_ppst_Ext;
					pst_SModifierPrec = NULL;
					continue;
				}
				else
				{
					return;
				}
			}

			idx++;
		}

		pst_SModifierPrec = pst_SModifier;
		pst_SModifier = pst_SModifier->pst_NextSModifier;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SModifierInstanciate(SND_tdst_SModifier *pst_Current, SND_tdst_SoundInstance *_pst_Dst)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifier	*pst_CInstance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(pst_Current)
	{
		pst_CInstance = SND_pst_SModifierInsertOneSModifier
			(
				&_pst_Dst->pst_SModifier,
				pst_Current->i->id,
				pst_Current->pv_Data
			);
		if(pst_Current->i->pfv_ApplyOnInstance)
		{
			pst_Current->i->pfv_ApplyOnInstance
				(
					pst_CInstance,
					pst_Current->pv_Data,
					pst_Current->ui_DataSize,
					_pst_Dst
				);
		}

		pst_Current = pst_Current->pst_NextSModifier;
	}

	_pst_Dst->pst_SModifier->ul_Flags = 0;
	_pst_Dst->pst_SModifier->ul_UserCount = 1;
	_pst_Dst->pst_SModifier->ul_FileKey = -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SModifierDeleteAll(SND_tdst_SModifier **_ppst_Extension, BOOL b_Instance)
{
	if(!_ppst_Extension) return;
	if(!*_ppst_Extension) return;
	if(--(*_ppst_Extension)->ul_UserCount) return;
	LOA_DeleteAddress(*_ppst_Extension);

	while(*_ppst_Extension)
	{
		ERR_X_Warning
		(
			((*_ppst_Extension)->i),
			"no interface found during SModifier destruction (SND_SModifierDeleteAll)",
			NULL
		);
		SND_SModifierDeleteOneSModifier(_ppst_Extension, (*_ppst_Extension)->i->id, -1, b_Instance);
	}
}

/*$4
 ***********************************************************************************************************************
    SModifier file
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SModifierSaveFromBuffer(ULONG _ul_Fat, SND_tdst_SModifier *_pst_SModifier)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierChunk stFileHeader;
	SND_tdst_SModifierChunk stCurr;
	char					asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	BIG_ComputeFullName(BIG_ParentFile(_ul_Fat), asz_Path);

    SAV_Begin(asz_Path, BIG_NameFile(_ul_Fat));

    stFileHeader.i_SModifierId = SND_Cte_SModifierFileVersion;
	stFileHeader.ui_DataSize = 0;
	SAV_Buffer((char *) &stFileHeader, sizeof(SND_tdst_SModifierChunk));

    stCurr.i_SModifierId = _pst_SModifier->i->id;
	stCurr.ui_DataSize = _pst_SModifier->ui_DataSize;
	SAV_Buffer((char *) &stCurr, sizeof(SND_tdst_SModifierChunk));

    ERR_X_Warning(_pst_SModifier->pv_Data, "no data found during save (SND_SModifierMakeSaveBuffer)", NULL);
	SND_SModifierExtPlayerSave(_pst_SModifier);
	
	/* add the last cluster */
	stCurr.i_SModifierId = -1;
	stCurr.ui_DataSize = 0;
	SAV_Buffer((char *) &stCurr, sizeof(SND_tdst_SModifierChunk));
	
    SAV_ul_End();
	LINK_FatHasChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SModifierCreateNewFile(ULONG _ulFat, void *_pv)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	void						*pvtemp = SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].pv_Data;
	SND_tdst_SModifierExtPlayer *pExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pv) SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].pv_Data = _pv;

	pExtPlayer = (SND_tdst_SModifierExtPlayer *) SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].pv_Data;
	pExtPlayer->ul_FileKey = BIG_FileKey(_ulFat);

	SND_SModifierSaveFromBuffer(_ulFat, &SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer]);

	if(_pv) SND_gast_SModifierModel[SND_Cte_SModifierExtPlayer].pv_Data = pvtemp;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_AddSModifier(BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				l_Size, l_Index;
	SND_tdst_OneSound	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- activation -------------------------------------------------------------------------------------------------*/

	if(!SND_gst_Params.l_Available) return -1;
	if((_ul_Key == 0) || (_ul_Key == BIG_C_InvalidKey)) return -1;

	/*$2- Test extension and existence -------------------------------------------------------------------------------*/

	l_Size = SND_l_GetSoundIndex(_ul_Key);
	if(l_Size != -1)
	{
		SND_gst_Params.dst_Sound[l_Size].ul_CptUsed++;
		return l_Size;
	}

	/*$2- Add sound to list ------------------------------------------------------------------------------------------*/

	l_Index = SND_i_AllocSound();
	pst_Sound = SND_gst_Params.dst_Sound + l_Index;

	pst_Sound->ul_FileKey = _ul_Key;
	pst_Sound->ul_Flags = SND_Cul_DSF_Used | SND_Cul_SF_SModifier | SND_Cul_OSF_Loaded;
	pst_Sound->ul_ExtFlags = 0;
	pst_Sound->ul_CptUsed = 1;
	pst_Sound->pst_Wave = NULL;
	pst_Sound->pst_DSB = (SND_tdst_SoundBuffer *) -1;
	pst_Sound->pst_LI = &SND_gpst_Interface[ISound];
	pst_Sound->pst_SModifier = NULL;

	LOA_MakeFileRef
	(
		_ul_Key,
		(ULONG *) &pst_Sound->pst_SModifier,
		SND_ul_SModifierCallback,
		LOA_C_MustExists | LOA_C_HasUserCounter
	);

	return(l_Index);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG SND_ul_SModifierCallback(ULONG _ul_FilePos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_Size;
	char					*pc_Buff;
	SND_tdst_SModifier		*pst_SModifier;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- file access ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Size = LOA_ul_SearchAddress(_ul_FilePos);
	if(ul_Size != (ULONG) - 1)
	{
		((SND_tdst_SModifier *) ul_Size)->ul_UserCount++;
		return ul_Size;
	}

	pst_SModifier = NULL;
	pc_Buff = BIG_pc_ReadFileTmp(_ul_FilePos, &ul_Size);

	/*$1- file check ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    SND_SModifierExtPlayerLoad(&pst_SModifier, &pc_Buff);

	if(pst_SModifier)
	{
		pst_SModifier->ul_Flags = 0;
		pst_SModifier->ul_UserCount = 1;
		pst_SModifier->ul_FileKey = LOA_ul_GetCurrentKey();
	}

	return(ULONG) pst_SModifier;
}

/*$4
 ***********************************************************************************************************************
    SOUND INSTANCE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SModifierUpdateInstance(SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifier	*pst_SModifier;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SModifier = pst_SI->pst_SModifier;

	while(pst_SModifier)
	{
		if(pst_SModifier->i->pfv_Update) pst_SModifier->i->pfv_Update(pst_SModifier, pst_SI);
		pst_SModifier = pst_SModifier->pst_NextSModifier;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_SModifierNotifyPlayInstance(SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifier	*pst_SModifier;
	int					ret = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SModifier = pst_SI->pst_SModifier;

	while(pst_SModifier)
	{
		if(pst_SModifier->i->pfi_NotifyInstancePlay)
			ret |= pst_SModifier->i->pfi_NotifyInstancePlay(pst_SModifier, pst_SI);
		pst_SModifier = pst_SModifier->pst_NextSModifier;
	}

	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_SModifierNotifyStopInstance(SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifier	*pst_SModifier;
	int					ret = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SModifier = pst_SI->pst_SModifier;

	while(pst_SModifier)
	{
		if(pst_SModifier->i->pfi_NotifyInstanceStop)
			ret |= pst_SModifier->i->pfi_NotifyInstanceStop(pst_SModifier, pst_SI);
		pst_SModifier = pst_SModifier->pst_NextSModifier;
	}

	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_SModifierNotifyPauseInstance(SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifier	*pst_SModifier;
	int					ret = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SModifier = pst_SI->pst_SModifier;

	while(pst_SModifier)
	{
		if(pst_SModifier->i->pfi_NotifyInstancePause)
			ret |= pst_SModifier->i->pfi_NotifyInstancePause(pst_SModifier, pst_SI);
		pst_SModifier = pst_SModifier->pst_NextSModifier;
	}

	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SModifierAddInInstance(LONG _l_Index, int _i_ExtId, void *_pv_Data, int _i_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	SND_tdst_SModifier		*p_Current;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	p_Current = SND_pst_SModifierInsertOneSModifier(&pst_SI->pst_SModifier, _i_ExtId, _pv_Data);
	if(p_Current->i->pfv_ApplyOnInstance)
		p_Current->i->pfv_ApplyOnInstance(p_Current, p_Current->pv_Data, p_Current->ui_DataSize, pst_SI);
}

/*$4
 ***********************************************************************************************************************
    EMPTY
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_SModifierGenericDestroy(SND_tdst_SModifier *_pst_Ext, void *_pv_Data, int _i_DataSize, BOOL b_Instance)
{
	MEM_Free(_pst_Ext->pv_Data);
	_pst_Ext->pv_Data = NULL;
	_pst_Ext->ui_DataSize = 0;
}

/*$4
 ***********************************************************************************************************************
    EXTENDED PLAYER
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_SModifierExtPlayerLoad(SND_tdst_SModifier **_pp_SMd, char **_ppc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer stExtPlay;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	SND_tdst_OneSound			*pst_Smd;
	unsigned short				i;
	SND_tdst_SModifierChunk stFileHeader;
	SND_tdst_SModifierChunk stCurr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&stExtPlay, 0, sizeof(stExtPlay));

	/*$2--------------------------------------------------------------------------------------------------------------*/

	stFileHeader.i_SModifierId = LOA_ReadULong(_ppc);
	stFileHeader.ui_DataSize = LOA_ReadULong(_ppc);
	stCurr.i_SModifierId = LOA_ReadULong(_ppc);
	stCurr.ui_DataSize = LOA_ReadULong(_ppc);

    stExtPlay.ul_SndKey = LOA_ReadULong(_ppc);
	stExtPlay.ul_SndFlags = LOA_ReadULong(_ppc);
	stExtPlay.i_SndIndex = LOA_ReadLong(_ppc);

	/*$2--------------------------------------------------------------------------------------------------------------*/

	stExtPlay.ui_Version = LOA_ReadULong(_ppc);
	stExtPlay.ul_FileKey = LOA_ReadULong(_ppc);
    stExtPlay.ul_FileKey = LOA_ul_GetCurrentKey();

	/*$2--------------------------------------------------------------------------------------------------------------*/

	stExtPlay.ui_SndExtFlags = LOA_ReadULong(_ppc);
    LOA_ReadLong_Ed(_ppc, NULL);
	stExtPlay.p_Template = NULL;
    
	/*$2--------------------------------------------------------------------------------------------------------------*/

	stExtPlay.ui_PlayerFlag = LOA_ReadULong(_ppc);

	/*$2--------------------------------------------------------------------------------------------------------------*/

	stExtPlay.f_DryVol = LOA_ReadFloat(_ppc);
	stExtPlay.f_DryVol_FactMin = LOA_ReadFloat(_ppc);
	stExtPlay.f_DryVol_FactMax = LOA_ReadFloat(_ppc);
	stExtPlay.f_FxVolLeft = LOA_ReadFloat(_ppc);

	/*$2--------------------------------------------------------------------------------------------------------------*/

	stExtPlay.ul_FadeInKey = LOA_ReadULong(_ppc);
    LOA_ReadULong_Ed(_ppc, NULL);
	stExtPlay.pst_FadeIn = NULL;

	stExtPlay.ul_FadeOutKey = LOA_ReadULong(_ppc);
    LOA_ReadULong_Ed(_ppc, NULL);
	stExtPlay.pst_FadeOut = NULL;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	stExtPlay.i_Pan = LOA_ReadLong(_ppc);
	stExtPlay.i_Span = LOA_ReadLong(_ppc);
	stExtPlay.f_MinPan = LOA_ReadFloat(_ppc);

	/*$2--------------------------------------------------------------------------------------------------------------*/

    if (stFileHeader.i_SModifierId < 3)
    {
	    ULONG ulFreq = LOA_ReadULong(_ppc);
	    stExtPlay.f_FreqCoef = 1.f;
    }
    else
    	stExtPlay.f_FreqCoef = LOA_ReadFloat(_ppc);

	stExtPlay.f_Freq_FactMin = LOA_ReadFloat(_ppc);
	stExtPlay.f_Freq_FactMax = LOA_ReadFloat(_ppc);
	stExtPlay.f_Doppler = LOA_ReadFloat(_ppc);

	/*$2--------------------------------------------------------------------------------------------------------------*/

	stExtPlay.af_Near[0] = LOA_ReadFloat(_ppc);
	stExtPlay.af_Near[1] = LOA_ReadFloat(_ppc);
	stExtPlay.af_Near[2] = LOA_ReadFloat(_ppc);
	stExtPlay.af_Far[0] = LOA_ReadFloat(_ppc);
	stExtPlay.af_Far[1] = LOA_ReadFloat(_ppc);
	stExtPlay.af_Far[2] = LOA_ReadFloat(_ppc);
	stExtPlay.af_MiddleBlend[0] = LOA_ReadFloat(_ppc);
	stExtPlay.af_MiddleBlend[1] = LOA_ReadFloat(_ppc);
	stExtPlay.af_MiddleBlend[2] = LOA_ReadFloat(_ppc);
	stExtPlay.f_FarCoeff = LOA_ReadFloat(_ppc);
	stExtPlay.f_MiddleCoeff = LOA_ReadFloat(_ppc);
	stExtPlay.f_CylinderHeight = LOA_ReadFloat(_ppc);

	/*$2--------------------------------------------------------------------------------------------------------------*/

	stExtPlay.f_FxVolRight = LOA_ReadFloat(_ppc);

	/*$2--------------------------------------------------------------------------------------------------------------*/

	LOA_ReadCharArray_Ed(_ppc, stExtPlay.ac_Reserved, SND_Cte_SModifierExtPlayerReservedSize * sizeof(char));

	/*$2--------------------------------------------------------------------------------------------------------------*/

	stExtPlay.st_PlayList.us_Size = LOA_ReadUShort(_ppc);
	stExtPlay.st_PlayList.us_Flags = LOA_ReadUShort(_ppc);
	stExtPlay.st_InsertList.us_Size = LOA_ReadUShort(_ppc);
	stExtPlay.st_InsertList.us_Flags = LOA_ReadUShort(_ppc);

	/*$2--------------------------------------------------------------------------------------------------------------*/

	if(stExtPlay.st_PlayList.us_Size)
	{
		stExtPlay.st_PlayList.aul_KeyList = (BIG_KEY *) MEM_p_AllocTmp(stExtPlay.st_PlayList.us_Size * sizeof(BIG_KEY));
		stExtPlay.st_PlayList.aul_FlagList = (BIG_KEY *) MEM_p_AllocTmp(stExtPlay.st_PlayList.us_Size * sizeof(ULONG));
#ifdef JADEFUSION
		stExtPlay.st_PlayList.al_IdxList = (/*BIG_KEY*/LONG *) MEM_p_AllocTmp(stExtPlay.st_PlayList.us_Size * sizeof(LONG));
#else
		stExtPlay.st_PlayList.al_IdxList = (BIG_KEY *) MEM_p_AllocTmp(stExtPlay.st_PlayList.us_Size * sizeof(LONG));
#endif
		L_memset(stExtPlay.st_PlayList.aul_KeyList, -1, stExtPlay.st_PlayList.us_Size * sizeof(BIG_KEY));
		L_memset(stExtPlay.st_PlayList.aul_FlagList, 0, stExtPlay.st_PlayList.us_Size * sizeof(ULONG));
		L_memset(stExtPlay.st_PlayList.al_IdxList, -1, stExtPlay.st_PlayList.us_Size * sizeof(LONG));

		for(i = 0; i < stExtPlay.st_PlayList.us_Size; i++)
		{
			stExtPlay.st_PlayList.aul_KeyList[i] = LOA_ReadULong(_ppc);
			stExtPlay.st_PlayList.aul_FlagList[i] = LOA_ReadULong(_ppc);
		}

		ERR_X_Warning
		(
			stExtPlay.ul_SndKey == BIG_C_InvalidKey,
			"[SND][SNDmodifier.c] smd can not contain play list AND file ref",
			NULL
		);
	}

	/*$2--------------------------------------------------------------------------------------------------------------*/

	if(stExtPlay.st_InsertList.us_Size)
	{
		stExtPlay.st_InsertList.aul_KeyList = (BIG_KEY *) MEM_p_AllocTmp(stExtPlay.st_InsertList.us_Size * sizeof(BIG_KEY));
		stExtPlay.st_InsertList.app_PtrList = (SND_tdst_Insert **) MEM_p_AllocTmp(stExtPlay.st_InsertList.us_Size * sizeof(SND_tdst_Insert *));

		L_memset(stExtPlay.st_InsertList.aul_KeyList, -1, stExtPlay.st_InsertList.us_Size * sizeof(BIG_KEY));
		L_memset(stExtPlay.st_InsertList.app_PtrList, 0, stExtPlay.st_InsertList.us_Size * sizeof(SND_tdst_Insert *));

		for(i = 0; i < stExtPlay.st_InsertList.us_Size; i++)
		{
			stExtPlay.st_InsertList.aul_KeyList[i] = LOA_ReadULong(_ppc);
		}
	}

    stCurr.i_SModifierId = LOA_ReadULong(_ppc);
	stCurr.ui_DataSize = LOA_ReadULong(_ppc);
	ERR_X_Warning
	(
		stCurr.i_SModifierId  == -1,
		"[SND][SNDmodifier.c] bad smd file format",
		NULL
	);


    //$2-
   
    if(stExtPlay.st_PlayList.us_Size)
    {
	    for(i = 0; i < stExtPlay.st_PlayList.us_Size; i++)
	    {
		    if(stExtPlay.st_PlayList.aul_FlagList[i] & SND_Cul_SF_Dialog)
		    {
			    stExtPlay.st_PlayList.al_IdxList[i] = SND_l_AddDialog(stExtPlay.st_PlayList.aul_KeyList[i]);
		    }
		    else if(stExtPlay.st_PlayList.aul_FlagList[i] & SND_Cul_SF_Music)
		    {
			    stExtPlay.st_PlayList.al_IdxList[i] = SND_l_AddMusic(stExtPlay.st_PlayList.aul_KeyList[i]);
		    }
		    else if(stExtPlay.st_PlayList.aul_FlagList[i] & SND_Cul_SF_Ambience)
		    {
			    stExtPlay.st_PlayList.al_IdxList[i] = SND_l_AddAmbience(stExtPlay.st_PlayList.aul_KeyList[i]);
		    }
		    else if(stExtPlay.st_PlayList.aul_FlagList[i] & SND_Cul_SF_LoadingSound)
		    {
			    stExtPlay.st_PlayList.al_IdxList[i] = SND_l_AddLoadingSound(stExtPlay.st_PlayList.aul_KeyList[i]);
		    }
		    else
		    {
			    stExtPlay.st_PlayList.al_IdxList[i] = SND_l_AddSound(stExtPlay.st_PlayList.aul_KeyList[i]);
		    }
	    }
    }
	else
	{
		if(stExtPlay.ul_SndFlags & SND_Cul_SF_Dialog)
		{
			stExtPlay.i_SndIndex = SND_l_AddDialog(stExtPlay.ul_SndKey);
		}
		else if(stExtPlay.ul_SndFlags & SND_Cul_SF_Music)
		{
			stExtPlay.i_SndIndex = SND_l_AddMusic(stExtPlay.ul_SndKey);
		}
		else if(stExtPlay.ul_SndFlags & SND_Cul_SF_Ambience)
		{
			stExtPlay.i_SndIndex = SND_l_AddAmbience(stExtPlay.ul_SndKey);
		}
		else if(stExtPlay.ul_SndFlags & SND_Cul_SF_LoadingSound)
		{
			stExtPlay.i_SndIndex = SND_l_AddLoadingSound(stExtPlay.ul_SndKey);
		}
		else
		{
			stExtPlay.i_SndIndex = SND_l_AddSound(stExtPlay.ul_SndKey);
		}
	}
      
    /*$2- alloc the structure ---------------------------------------------------------------------------------------------*/

	SND_pst_SModifierInsertOneSModifier(_pp_SMd, SND_Cte_SModifierExtPlayer, &stExtPlay);

	/*$2--------------------------------------------------------------------------------------------------------------*/

	if(stExtPlay.st_PlayList.us_Size)
	{
        if(stExtPlay.p_Template)
            stExtPlay.st_PlayList.us_Idx = stExtPlay.p_Template->st_PlayList.us_Idx;
        else
		    stExtPlay.st_PlayList.us_Idx = stExtPlay.st_PlayList.us_Size - 1;
		stExtPlay.st_PlayList.us_EndHitten = 0;

		stExtPlay.ul_SndKey = stExtPlay.st_PlayList.aul_KeyList[stExtPlay.st_PlayList.us_Idx];
		stExtPlay.i_SndIndex = stExtPlay.st_PlayList.al_IdxList[stExtPlay.st_PlayList.us_Idx];
		stExtPlay.ul_SndFlags |= stExtPlay.st_PlayList.aul_FlagList[stExtPlay.st_PlayList.us_Idx] & SND_Cul_SF_MaskDynFlags;
	}

	pst_Smd = SND_gst_Params.dst_Sound + SND_l_GetSoundIndex(LOA_ul_GetCurrentKey());
	pst_Smd->pst_DSB = (SND_tdst_SoundBuffer *) stExtPlay.i_SndIndex;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	pst_ExtPlay = (SND_tdst_SModifierExtPlayer *) (SND_pst_SModifierGet(*_pp_SMd, SND_Cte_SModifierExtPlayer, 0)->pv_Data);
    pst_ExtPlay->p_Template = NULL;

	pst_ExtPlay->pst_FadeIn = NULL;
	if(pst_ExtPlay->ul_FadeInKey != BIG_C_InvalidKey)
	{
		LOA_MakeFileRef
		(
			pst_ExtPlay->ul_FadeInKey,
			(ULONG *) &pst_ExtPlay->pst_FadeIn,
			SND_ul_CallbackInsertLoad,
			LOA_C_MustExists | LOA_C_HasUserCounter
		);
	}

	pst_ExtPlay->pst_FadeOut = NULL;
	if(pst_ExtPlay->ul_FadeOutKey != BIG_C_InvalidKey)
	{
		LOA_MakeFileRef
		(
			pst_ExtPlay->ul_FadeOutKey,
			(ULONG *) &pst_ExtPlay->pst_FadeOut,
			SND_ul_CallbackInsertLoad,
			LOA_C_MustExists | LOA_C_HasUserCounter
		);
	}

	/*$2--------------------------------------------------------------------------------------------------------------*/

	if(pst_ExtPlay->st_InsertList.us_Size)
	{
		for(i = 0; i < pst_ExtPlay->st_InsertList.us_Size; i++)
		{
			if(pst_ExtPlay->st_InsertList.aul_KeyList[i] == BIG_C_InvalidKey) continue;
			LOA_MakeFileRef
			(
				pst_ExtPlay->st_InsertList.aul_KeyList[i],
				(ULONG *) &pst_ExtPlay->st_InsertList.app_PtrList[i],
				SND_ul_CallbackInsertLoad,
				LOA_C_MustExists | LOA_C_HasUserCounter
			);
		}

		MEM_Free(stExtPlay.st_InsertList.aul_KeyList);
		MEM_Free(stExtPlay.st_InsertList.app_PtrList);
	}

	/*$2--------------------------------------------------------------------------------------------------------------*/

	if(stExtPlay.st_PlayList.us_Size)
	{
		pst_ExtPlay->st_PlayList.us_Idx = stExtPlay.st_PlayList.us_Idx;
		pst_ExtPlay->ul_SndKey = stExtPlay.ul_SndKey;
		pst_ExtPlay->i_SndIndex = stExtPlay.i_SndIndex;
		pst_ExtPlay->ul_SndFlags = stExtPlay.ul_SndFlags;

		MEM_Free(stExtPlay.st_PlayList.aul_KeyList);
		MEM_Free(stExtPlay.st_PlayList.aul_FlagList);
		MEM_Free(stExtPlay.st_PlayList.al_IdxList);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_SModifierExtPlayerSave(SND_tdst_SModifier *pSMd)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if ACTIVE_EDITORS
	SND_tdst_SModifierExtPlayer *p_Src;
	unsigned short				i;
	ULONG						ul_SndKey;
	int							i_SndIndex;
    int                         i_Dummy=0xCAFEDECA;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(pSMd->i->id == SND_Cte_SModifierExtPlayer);
	ERR_X_Assert(pSMd->pv_Data);
   	p_Src = (SND_tdst_SModifierExtPlayer *) pSMd->pv_Data;

	//$1-
    if(p_Src->st_PlayList.us_Size)
	{
		ul_SndKey = p_Src->ul_SndKey;
		i_SndIndex = p_Src->i_SndIndex;

		p_Src->ul_SndKey = BIG_C_InvalidKey;
		p_Src->i_SndIndex = -1;
	}
	
    /* SModifier header */
    SAV_Buffer(&p_Src->ul_SndKey, sizeof(BIG_KEY));
	SAV_Buffer(&p_Src->ul_SndFlags, sizeof(ULONG));
	SAV_Buffer(&p_Src->i_SndIndex, sizeof(int));

	/* version ID */
	SAV_Buffer(&p_Src->ui_Version, sizeof(unsigned int));
	SAV_Buffer(&p_Src->ul_FileKey, sizeof(BIG_KEY));

	/* sound/instance data */
	SAV_Buffer(&p_Src->ui_SndExtFlags, sizeof(unsigned int));
    SAV_Buffer(&i_Dummy, sizeof(int));
	
	/* player data */
	SAV_Buffer(&p_Src->ui_PlayerFlag, sizeof(unsigned int));

	/* volumes */
	SAV_Buffer(&p_Src->f_DryVol, sizeof(float));
	SAV_Buffer(&p_Src->f_DryVol_FactMin, sizeof(float));
	SAV_Buffer(&p_Src->f_DryVol_FactMax, sizeof(float));
	SAV_Buffer(&p_Src->f_FxVolLeft, sizeof(float));

	/* fade in/out */
	SAV_Buffer(&p_Src->ul_FadeInKey,sizeof(ULONG));
    SAV_Buffer(&i_Dummy, sizeof(int));
	SAV_Buffer(&p_Src->ul_FadeOutKey,sizeof(ULONG));
    SAV_Buffer(&i_Dummy, sizeof(int));
	
	/* pan */
	SAV_Buffer(&p_Src->i_Pan,sizeof(int));
	SAV_Buffer(&p_Src->i_Span,sizeof(int));
	SAV_Buffer(&p_Src->f_MinPan,sizeof(float));

	/* frequency */
	SAV_Buffer(&p_Src->f_FreqCoef,sizeof(float));
	SAV_Buffer(&p_Src->f_Freq_FactMin,sizeof(float));
	SAV_Buffer(&p_Src->f_Freq_FactMax,sizeof(float));
	SAV_Buffer(&p_Src->f_Doppler,sizeof(float));

	/* 3D settings */
	SAV_Buffer(&p_Src->af_Near[0], sizeof(float));
	SAV_Buffer(&p_Src->af_Near[1], sizeof(float));
	SAV_Buffer(&p_Src->af_Near[2], sizeof(float));
    SAV_Buffer(&p_Src->af_Far[0], sizeof(float));
	SAV_Buffer(&p_Src->af_Far[1], sizeof(float));
	SAV_Buffer(&p_Src->af_Far[2], sizeof(float));
	SAV_Buffer(&p_Src->af_MiddleBlend[0], sizeof(float));
	SAV_Buffer(&p_Src->af_MiddleBlend[1], sizeof(float));
	SAV_Buffer(&p_Src->af_MiddleBlend[2], sizeof(float));
    SAV_Buffer(&p_Src->f_FarCoeff,sizeof(float));
	SAV_Buffer(&p_Src->f_MiddleCoeff,sizeof(float));
	SAV_Buffer(&p_Src->f_CylinderHeight,sizeof(float));

	/* reserve */
	SAV_Buffer(&p_Src->f_FxVolRight,sizeof(float));
	SAV_Buffer(&p_Src->ac_Reserved, SND_Cte_SModifierExtPlayerReservedSize);

	/* play-list */
	SAV_Buffer(&p_Src->st_PlayList.us_Size, sizeof(unsigned short));
	SAV_Buffer(&p_Src->st_PlayList.us_Flags, sizeof(unsigned short));
	SAV_Buffer(&p_Src->st_InsertList.us_Size, sizeof(unsigned short));
	SAV_Buffer(&p_Src->st_InsertList.us_Flags, sizeof(unsigned short));

	for(i = 0; i < p_Src->st_PlayList.us_Size; i++)
	{
		SAV_Buffer(&p_Src->st_PlayList.aul_KeyList[i], sizeof(ULONG));
		SAV_Buffer(&p_Src->st_PlayList.aul_FlagList[i], sizeof(ULONG));
	}

	for(i = 0; i < p_Src->st_InsertList.us_Size; i++)
	{
		SAV_Buffer(&p_Src->st_InsertList.aul_KeyList[i], sizeof(ULONG));
	}

    //$1-
	if(p_Src->st_PlayList.us_Size)
	{
		p_Src->ul_SndKey = ul_SndKey;
		p_Src->i_SndIndex = i_SndIndex;
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_SModifierExtPlayerInit(SND_tdst_SModifier *_pst_Ext, void *_pv_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *p_Src, *p_Dst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Src = (SND_tdst_SModifierExtPlayer *) _pv_Data;

	/* check the source settings */
	ERR_X_Warning((p_Src->af_Far[0] >= 0.0f), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);
	ERR_X_Warning((p_Src->af_Far[1] >= 0.0f), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);
	ERR_X_Warning((p_Src->af_Far[2] >= 0.0f), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);

	ERR_X_Warning((p_Src->af_Near[0] >= 0.0f), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);
	ERR_X_Warning((p_Src->af_Near[1] >= 0.0f), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);
	ERR_X_Warning((p_Src->af_Near[2] >= 0.0f), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);

	ERR_X_Warning
	(
		(p_Src->af_Far[0] >= p_Src->af_Near[0]),
		"inconsistency in SModifier (SND_SModifierExtPlayerInit)",
		NULL
	);
	ERR_X_Warning
	(
		(p_Src->af_Far[1] >= p_Src->af_Near[1]),
		"inconsistency in SModifier (SND_SModifierExtPlayerInit)",
		NULL
	);
	ERR_X_Warning
	(
		(p_Src->af_Far[2] >= p_Src->af_Near[2]),
		"inconsistency in SModifier (SND_SModifierExtPlayerInit)",
		NULL
	);
	ERR_X_Warning((1.0f >= p_Src->af_MiddleBlend[0]), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);
	ERR_X_Warning((1.0f >= p_Src->af_MiddleBlend[1]), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);
	ERR_X_Warning((1.0f >= p_Src->af_MiddleBlend[2]), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);
	ERR_X_Warning((p_Src->af_MiddleBlend[0] >= 0.0f), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);
	ERR_X_Warning((p_Src->af_MiddleBlend[1] >= 0.0f), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);
	ERR_X_Warning((p_Src->af_MiddleBlend[2] >= 0.0f), "inconsistency in SModifier (SND_SModifierExtPlayerInit)", NULL);

	/* update the destination settings */
	if(!_pst_Ext->pv_Data) _pst_Ext->pv_Data = MEM_p_Alloc(sizeof(SND_tdst_SModifierExtPlayer));
	_pst_Ext->ui_DataSize = sizeof(SND_tdst_SModifierExtPlayer) - sizeof(SND_tdst_PlayListDesc) + 2 * sizeof(unsigned short) - sizeof(SND_tdst_InsertListDesc) + 2 * sizeof(unsigned short);

	p_Dst = (SND_tdst_SModifierExtPlayer *) _pst_Ext->pv_Data;
	L_memcpy(p_Dst, p_Src, sizeof(SND_tdst_SModifierExtPlayer));

	p_Dst->p_Template = p_Src;
	p_Dst->ul_SndFlags &= SND_Cul_SF_MaskDynFlags;
	p_Dst->ui_SndExtFlags &= SND_Cul_ESF_MaskDynExtFlags;

	if(p_Dst->st_InsertList.us_Size)
	{
		p_Dst->st_InsertList.aul_KeyList = (BIG_KEY *) MEM_p_Alloc(p_Dst->st_InsertList.us_Size * sizeof(BIG_KEY));
		L_memcpy
		(
			p_Dst->st_InsertList.aul_KeyList,
			p_Src->st_InsertList.aul_KeyList,
			p_Dst->st_InsertList.us_Size * sizeof(BIG_KEY)
		);

		p_Dst->st_InsertList.app_PtrList = (SND_tdst_Insert **) MEM_p_Alloc(p_Dst->st_InsertList.us_Size * sizeof(SND_tdst_Insert *));
		L_memcpy
		(
			p_Dst->st_InsertList.app_PtrList,
			p_Src->st_InsertList.app_PtrList,
			p_Dst->st_InsertList.us_Size * sizeof(SND_tdst_Insert **)
		);

		p_Dst->st_InsertList.ast_State = (SND_tdst_InsertState *) MEM_p_Alloc(p_Dst->st_InsertList.us_Size * sizeof(SND_tdst_InsertState));
		L_memset(p_Dst->st_InsertList.ast_State, 0, p_Dst->st_InsertList.us_Size * sizeof(SND_tdst_InsertState));

		_pst_Ext->ui_DataSize += p_Dst->st_InsertList.us_Size * sizeof(BIG_KEY);
		_pst_Ext->ui_DataSize += p_Dst->st_InsertList.us_Size * sizeof(SND_tdst_Insert *);
		_pst_Ext->ui_DataSize += p_Dst->st_InsertList.us_Size * sizeof(LONG);
	}
	else
	{
		L_memset(&p_Dst->st_InsertList, 0, sizeof(SND_tdst_InsertListDesc));
	}

	if(p_Dst->st_PlayList.us_Size)
	{
		p_Dst->st_PlayList.aul_KeyList = (BIG_KEY *) MEM_p_Alloc(p_Dst->st_PlayList.us_Size * sizeof(BIG_KEY));
		L_memcpy
		(
			p_Dst->st_PlayList.aul_KeyList,
			p_Src->st_PlayList.aul_KeyList,
			p_Dst->st_PlayList.us_Size * sizeof(BIG_KEY)
		);

		p_Dst->st_PlayList.aul_FlagList = (ULONG *) MEM_p_Alloc(p_Dst->st_PlayList.us_Size * sizeof(ULONG));
		L_memcpy
		(
			p_Dst->st_PlayList.aul_FlagList,
			p_Src->st_PlayList.aul_FlagList,
			p_Dst->st_PlayList.us_Size * sizeof(ULONG)
		);

		p_Dst->st_PlayList.al_IdxList = (LONG *) MEM_p_Alloc(p_Dst->st_PlayList.us_Size * sizeof(LONG));
		L_memcpy
		(
			p_Dst->st_PlayList.al_IdxList,
			p_Src->st_PlayList.al_IdxList,
			p_Dst->st_PlayList.us_Size * sizeof(LONG)
		);

		_pst_Ext->ui_DataSize += p_Dst->st_PlayList.us_Size * sizeof(BIG_KEY);
		_pst_Ext->ui_DataSize += p_Dst->st_PlayList.us_Size * sizeof(ULONG);


        p_Dst->st_PlayList.us_Idx = p_Src->st_PlayList.us_Idx;

        p_Dst->ul_SndKey = p_Dst->st_PlayList.aul_KeyList[p_Dst->st_PlayList.us_Idx];
		p_Dst->i_SndIndex = p_Dst->st_PlayList.al_IdxList[p_Dst->st_PlayList.us_Idx];
		p_Dst->ul_SndFlags |= p_Dst->st_PlayList.aul_FlagList[p_Dst->st_PlayList.us_Idx] & SND_Cul_SF_MaskDynFlags;
	}
	else
	{
		L_memset(&p_Dst->st_PlayList, 0, sizeof(SND_tdst_PlayListDesc));
	}

	SND_M_SModifierSetState(p_Dst, SND_Cte_SModifierInitState);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_SModifierExtPlayerApplyOnInstance
(
	SND_tdst_SModifier		*_pst_InstanceSM,
	void					*_pv_Data,
	int						_i_DataSize,
	SND_tdst_SoundInstance	*p
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *p_Play;
	unsigned short				us;
	float						middle;
	int							i_SndInstance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Play = (SND_tdst_SModifierExtPlayer *) _pst_InstanceSM->pv_Data;
	i_SndInstance = SND_M_GetInstanceFromPtr(p);

	SND_SetVol(i_SndInstance, p_Play->f_DryVol);

	SND_ResetFlags(NULL, i_SndInstance, SND_Cul_SF_MaskDynVol);
	SND_SetFlags(NULL, i_SndInstance, p_Play->ul_SndFlags);
	SND_SetFlags(NULL, i_SndInstance, SND_Cul_SF_UseExtFlag | p_Play->ui_SndExtFlags);

	if(p->p_GameObject) SND_SetObject((OBJ_tdst_GameObject *) p->p_GameObject, i_SndInstance);
    if(p_Play->f_FreqCoef != 1.f) SND_SetFreq(i_SndInstance, p->i_BaseFrequency );
	if(p_Play->i_Pan) SND_SetPan(i_SndInstance, p_Play->i_Pan, 0);
	p->f_Doppler = p_Play->f_Doppler;

	if(p_Play->ul_SndFlags & SND_Cul_SF_DynVolSpheric)
	{
		SND_SetNearFar(i_SndInstance, SND_Cte_SetInstNearFar0, p_Play->af_Near[0], p_Play->af_Far[0]);
		middle = MATH_f_FloatBlend(p_Play->af_Near[0], p_Play->af_Far[0], p_Play->af_MiddleBlend[0]);
		SND_i_InstanceSetf(i_SndInstance, SND_Cte_SetInstMiddle0, middle);
	}

	if(p_Play->ul_SndFlags & SND_Cul_SF_DynVolXaxis)
	{
		SND_SetNearFar(i_SndInstance, SND_Cte_SetInstNearFar0, p_Play->af_Near[0], p_Play->af_Far[0]);
		middle = MATH_f_FloatBlend(p_Play->af_Near[0], p_Play->af_Far[0], p_Play->af_MiddleBlend[0]);
		SND_i_InstanceSetf(i_SndInstance, SND_Cte_SetInstMiddle0, middle);
	}

	if(p_Play->ul_SndFlags & SND_Cul_SF_DynVolYaxis)
	{
		SND_SetNearFar(i_SndInstance, SND_Cte_SetInstNearFar1, p_Play->af_Near[1], p_Play->af_Far[1]);
		middle = MATH_f_FloatBlend(p_Play->af_Near[1], p_Play->af_Far[1], p_Play->af_MiddleBlend[1]);
		SND_i_InstanceSetf(i_SndInstance, SND_Cte_SetInstMiddle1, middle);
	}

	if(p_Play->ul_SndFlags & SND_Cul_SF_DynVolZaxis)
	{
		SND_SetNearFar(i_SndInstance, SND_Cte_SetInstNearFar2, p_Play->af_Near[2], p_Play->af_Far[2]);
		middle = MATH_f_FloatBlend(p_Play->af_Near[2], p_Play->af_Far[2], p_Play->af_MiddleBlend[2]);
		SND_i_InstanceSetf(i_SndInstance, SND_Cte_SetInstMiddle2, middle);
	}

	if(p_Play->ul_SndFlags & SND_Cul_SF_DynPan) p->f_MinPan = p_Play->f_MinPan;

	p->f_FarCoeff = p_Play->f_FarCoeff;
	p->f_MiddleCoeff = p_Play->f_MiddleCoeff;
	p->f_CylinderHeight = p_Play->f_CylinderHeight;
	p->f_FxVolLeft = p_Play->f_FxVolLeft;
	p->f_FxVolRight = p_Play->f_FxVolRight;

	p->ul_Flags &= ~(SND_Cul_SF_FxA | SND_Cul_SF_FxB);
	if(p_Play->ui_PlayerFlag & SND_Cte_SModifierFxA) p->ul_Flags |= SND_Cul_SF_FxA;
	if(p_Play->ui_PlayerFlag & SND_Cte_SModifierFxB) p->ul_Flags |= SND_Cul_SF_FxB;

	if((p->ul_Flags & (SND_Cul_SF_FxA | SND_Cul_SF_FxB)) == 0)
	{
		p->f_FxVolLeft = p_Play->f_FxVolLeft = 0.0f;
		p->f_FxVolRight = p_Play->f_FxVolRight = 0.0f;
	}

	if(p_Play->st_InsertList.us_Size)
	{
		for(us = 0; us < p_Play->st_InsertList.us_Size; us++)
		{
			/* init the insert */
			SND_i_SModifierActiveInsert(p, us, TRUE, TRUE);
			SND_i_SModifierActiveInsert(p, us, FALSE, TRUE);
		}
	}

	switch(SND_M_SModifierGetGroup(p_Play))
	{
	case SND_Cte_SModifierGroupMusic:		p->i_GroupId = SND_Cte_GrpMusic; break;
	case SND_Cte_SModifierGroupSFX:			p->i_GroupId = SND_Cte_GrpSpecialFX; break;
	case SND_Cte_SModifierGroupAmbience:	p->i_GroupId = SND_Cte_GrpAmbience; break;
	case SND_Cte_SModifierGroupDialog:		p->i_GroupId = SND_Cte_GrpDialog; break;
	case SND_Cte_SModifierGroupCutScene:	p->i_GroupId = SND_Cte_GrpCutScene; break;
	case SND_Cte_SModifierGroupInterface:	p->i_GroupId = SND_Cte_GrpInterface; break;
	default:								break;
	}

	if(p_Play->ui_PlayerFlag & SND_Cte_SModifierGrpA) p->i_GroupId |= SND_Cte_GrpA;
	if(p_Play->ui_PlayerFlag & SND_Cte_SModifierGrpB) p->i_GroupId |= SND_Cte_GrpB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_SModifierExtPlayerNotifyPlay(SND_tdst_SModifier *_pst_SMd, SND_tdst_SoundInstance *_pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	float						vol;
	unsigned int				uiFreq;
    int                         i;
    LONG l_Old, l_New, l_Instance;
    SND_tdst_SoundInstance      *pst_SI, st_SI;
    ULONG ul_NewFlags, ul_OldFlags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = (SND_tdst_SModifierExtPlayer *) _pst_SMd->pv_Data;
    l_Old = pst_ExtPlayer->i_SndIndex;
    l_New = SND_SModifierChangeSoundIndex(_pst_SMd);
    
    if((pst_ExtPlayer->i_SndIndex != l_Old) && (l_New != -1))
    {
        l_Instance = SND_l_Request(NULL, l_New);
        if(l_Instance == -1) 
            return -1;
        
        pst_SI = SND_gst_Params.dst_Instance + l_Instance;

        // save the old
        L_memcpy(&st_SI, _pst_SI, sizeof(SND_tdst_SoundInstance));
        ul_OldFlags = _pst_SI->ul_Flags;
        ul_NewFlags = pst_SI->ul_Flags;
        
        // new -> old
        L_memcpy(_pst_SI, pst_SI, sizeof(SND_tdst_SoundInstance));

        _pst_SI->p_GameObject = st_SI.p_GameObject;
        _pst_SI->pst_GlobalPos = st_SI.pst_GlobalPos;
        _pst_SI->pst_GlobalMatrix = st_SI.pst_GlobalMatrix;
        _pst_SI->pst_SModifier = st_SI.pst_SModifier;
        
        _pst_SI->ul_Flags = 0;
        _pst_SI->ul_Flags |= (ul_NewFlags & SND_Cul_OSF_Loaded);
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_OSF_IsPlaying);			
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_LoadingSound);			
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_OSF_Loop);				
        _pst_SI->ul_Flags |= (ul_NewFlags & SND_Cul_DSF_Instance);			
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_DSF_DestroyWhenFinished );
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_DSF_StartedOnce	);
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_DSF_Used		);
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_DSF_AutoPlay	);
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_Music		);
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_Dialog);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_Playing);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_SModifier);
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_Ambience			);	
        _pst_SI->ul_Flags |= (ul_NewFlags & SND_Cul_SF_HeaderLoaded	);		
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_FxA			);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_FxB			);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_PlayingLoop	);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_DynVolXaxis	);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_DynVolYaxis	);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_DynVolZaxis	);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_DynPan		);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_DynVolSpheric	);
        _pst_SI->ul_Flags |= (ul_NewFlags & SND_Cul_SF_EdiPause		);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cul_SF_UseExtFlag	);	
        _pst_SI->ul_Flags |= (ul_OldFlags & SND_Cte_StreamedFile		);

        _pst_SI->ul_ExtFlags = st_SI.ul_ExtFlags;

#ifdef ACTIVE_EDITORS
        _pst_SI->ul_SmdKey = st_SI.ul_SmdKey;
        _pst_SI->ul_EdiFlags = st_SI.ul_EdiFlags;
#endif

        // saved old -> new
        st_SI.p_GameObject = pst_SI->p_GameObject;
        st_SI.pst_SModifier = pst_SI->pst_SModifier;
        st_SI.pst_GlobalMatrix = pst_SI->pst_GlobalMatrix;
        st_SI.pst_GlobalPos = pst_SI->pst_GlobalPos;

        st_SI.ul_Flags = 0;
        st_SI.ul_Flags |= (ul_OldFlags & SND_Cul_OSF_Loaded);
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_OSF_IsPlaying);			
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_LoadingSound);			
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_OSF_Loop);				
        st_SI.ul_Flags |= (ul_OldFlags & SND_Cul_DSF_Instance);			
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_DSF_DestroyWhenFinished );
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_DSF_StartedOnce			);
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_DSF_Used				);
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_DSF_AutoPlay			);
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_Music				);
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_Dialog			);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_Playing			);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_SModifier			);
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_Ambience			);	
        st_SI.ul_Flags |= (ul_OldFlags & SND_Cul_SF_HeaderLoaded);		
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_FxA			);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_FxB			);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_PlayingLoop	);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_DynVolXaxis	);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_DynVolYaxis	);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_DynVolZaxis	);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_DynPan		);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_DynVolSpheric		);
        st_SI.ul_Flags |= (ul_OldFlags & SND_Cul_SF_EdiPause			);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cul_SF_UseExtFlag		);	
        st_SI.ul_Flags |= (ul_NewFlags & SND_Cte_StreamedFile			);

        st_SI.ul_ExtFlags = pst_SI->ul_ExtFlags;

#ifdef ACTIVE_EDITORS
        st_SI.ul_SmdKey = pst_SI->ul_SmdKey;
        st_SI.ul_EdiFlags = pst_SI->ul_EdiFlags;
#endif

        L_memcpy(pst_SI, &st_SI, sizeof(SND_tdst_SoundInstance));

        SND_Release(l_Instance);
    }

	/*$1- random vol ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_ExtPlayer->f_DryVol_FactMax + pst_ExtPlayer->f_DryVol_FactMin)
	{
		pst_ExtPlayer->st_Dyn.f_DryVol_Curr = pst_ExtPlayer->f_DryVol * fRand
			(
				pst_ExtPlayer->f_DryVol_FactMin,
				pst_ExtPlayer->f_DryVol_FactMax
			);
	}
	else
		pst_ExtPlayer->st_Dyn.f_DryVol_Curr = pst_ExtPlayer->f_DryVol;

	/*$1- random freq ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uiFreq = (unsigned int) (pst_ExtPlayer->f_FreqCoef * (float) _pst_SI->i_StartFrequency);
	if(pst_ExtPlayer->f_Freq_FactMin + pst_ExtPlayer->f_Freq_FactMax)
	{
		pst_ExtPlayer->st_Dyn.ui_Freq_Curr = lFloatToLong(fLongToFloat(uiFreq) * fRand(pst_ExtPlayer->f_Freq_FactMin, pst_ExtPlayer->f_Freq_FactMax));
	}
	else
		pst_ExtPlayer->st_Dyn.ui_Freq_Curr = uiFreq;

	/*$1- fade in ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_SetVol(SND_M_GetInstanceFromPtr(_pst_SI), pst_ExtPlayer->st_Dyn.f_DryVol_Curr);

    if(pst_ExtPlayer->pst_FadeIn)
	{
		switch(SND_M_SModifierGetState(pst_ExtPlayer))
		{
		case SND_Cte_SModifierFadingIn:
		case SND_Cte_SModifierPlaying:
			/* do nothing */
			break;

		case SND_Cte_SModifierInitState:
			/* set vol + start fade in */
			vol = pst_ExtPlayer->st_Dyn.f_DryVol_Curr * SND_f_InsertGetYmin(pst_ExtPlayer->pst_FadeIn);
			SND_SetVol(SND_M_GetInstanceFromPtr(_pst_SI), vol);
			SND_M_SModifierSetState(pst_ExtPlayer, SND_Cte_SModifierFadingIn);
			pst_ExtPlayer->st_Dyn.f_FadeInDate = 0.0f;
			break;

		case SND_Cte_SModifierFadingOut:
		case SND_Cte_SModifierPause:
			/* restart fade in from curr vol */
			SND_M_SModifierSetState(pst_ExtPlayer, SND_Cte_SModifierFadingIn);
			pst_ExtPlayer->st_Dyn.f_FadeInDate = SND_f_InsertGetXvsY(pst_ExtPlayer->pst_FadeIn, _pst_SI->f_Volume);
			break;

		default:
			ERR_X_Warning(0, "[SND][SNDmodifier.c] bad smd state", NULL);
			break;
		}
	}
	else
	{
		SND_M_SModifierSetState(pst_ExtPlayer, SND_Cte_SModifierPlaying);
	}

	if(pst_ExtPlayer->st_Dyn.ui_Freq_Curr)
		SND_SetFreq(SND_M_GetInstanceFromPtr(_pst_SI), pst_ExtPlayer->st_Dyn.ui_Freq_Curr);

	for(i = 0; i < (int)pst_ExtPlayer->st_InsertList.us_Size; i++)
	{
        SND_i_SModifierActiveInsert(_pst_SI, i, TRUE, TRUE);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_SModifierChangeSoundIndex(SND_tdst_SModifier *_pst_SMd)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	unsigned short				us;
	SND_tdst_OneSound			*pst_Smd;
    LONG                        l_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = (SND_tdst_SModifierExtPlayer *) _pst_SMd->pv_Data;
	if(pst_ExtPlayer->st_PlayList.us_Size <= 1) return -1;

    if(pst_ExtPlayer->p_Template)
	{
        pst_ExtPlayer->st_PlayList.us_Idx = pst_ExtPlayer->p_Template->st_PlayList.us_Idx;
        pst_ExtPlayer->st_PlayList.us_EndHitten = pst_ExtPlayer->p_Template->st_PlayList.us_EndHitten;
	}


	if(pst_ExtPlayer->ui_PlayerFlag & SND_Cte_SModifierStopOnLastHit)
	{
		// play only once the list
		if(pst_ExtPlayer->st_PlayList.us_Idx == (pst_ExtPlayer->st_PlayList.us_Size - 1))
		{
			if(pst_ExtPlayer->st_PlayList.us_EndHitten)
			return -1;
			pst_ExtPlayer->st_PlayList.us_EndHitten = 1;
		}
	}

	if(pst_ExtPlayer->ui_PlayerFlag & SND_Cte_SModifierRandomPlayList)
	{
		// random
		do
		{
			us = (unsigned short) lRand(0, pst_ExtPlayer->st_PlayList.us_Size);
            if (us == pst_ExtPlayer->st_PlayList.us_Idx)
            {
                us = (us + 1) % pst_ExtPlayer->st_PlayList.us_Size;
            }
		} while(us == pst_ExtPlayer->st_PlayList.us_Idx);
		pst_ExtPlayer->st_PlayList.us_Idx = us;
	}
	else
	{
		// sequence
		pst_ExtPlayer->st_PlayList.us_Idx++;
		pst_ExtPlayer->st_PlayList.us_Idx %= pst_ExtPlayer->st_PlayList.us_Size;
		us = pst_ExtPlayer->st_PlayList.us_Idx;
	}

    l_Sound = SND_l_GetSoundIndex(pst_ExtPlayer->ul_FileKey);
	pst_Smd = SND_gst_Params.dst_Sound + l_Sound;

	pst_ExtPlayer->ul_SndKey = pst_ExtPlayer->st_PlayList.aul_KeyList[us];
	pst_ExtPlayer->i_SndIndex = pst_ExtPlayer->st_PlayList.al_IdxList[us];
	pst_ExtPlayer->ul_SndFlags |= pst_ExtPlayer->st_PlayList.aul_FlagList[us] & SND_Cul_SF_MaskDynFlags;
    
    pst_Smd->pst_DSB = (SND_tdst_SoundBuffer *) pst_ExtPlayer->i_SndIndex;

    if(pst_ExtPlayer->p_Template)
	{
        pst_ExtPlayer->p_Template->st_PlayList.us_Idx = pst_ExtPlayer->st_PlayList.us_Idx;
        pst_ExtPlayer->p_Template->st_PlayList.us_EndHitten= pst_ExtPlayer->st_PlayList.us_EndHitten;
	}

    return l_Sound;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_SModifierExtPlayerNotifyStop(SND_tdst_SModifier *_pst_SMd, SND_tdst_SoundInstance *_pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	float						vol;
	int							ret = 0, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = (SND_tdst_SModifierExtPlayer *) _pst_SMd->pv_Data;

	for(i = 0; i < (int)pst_ExtPlayer->st_InsertList.us_Size; i++)
        SND_i_SModifierActiveInsert(_pst_SI, i, FALSE, FALSE);

	if(pst_ExtPlayer->pst_FadeOut)
	{
		switch(SND_M_SModifierGetState(pst_ExtPlayer))
		{
		case SND_Cte_SModifierInitState:
		case SND_Cte_SModifierPause:
		case SND_Cte_SModifierFadingOut:
			/* nothing to do */
			break;

		case SND_Cte_SModifierPlaying:
			// try to eval the current vol
			vol =  SND_f_InsertGetYmax(pst_ExtPlayer->pst_FadeOut);
            if(vol) pst_ExtPlayer->st_Dyn.f_DryVol_Curr = _pst_SI->f_Volume / vol;

            /* start vol */
			vol = pst_ExtPlayer->st_Dyn.f_DryVol_Curr * SND_f_InsertGetYmax(pst_ExtPlayer->pst_FadeOut);
			SND_SetVol(SND_M_GetInstanceFromPtr(_pst_SI), vol);
			SND_M_SModifierSetState(pst_ExtPlayer, SND_Cte_SModifierFadingOut);
			pst_ExtPlayer->st_Dyn.f_FadeOutDate = 0.0f;
			ret = 1;
			break;

		case SND_Cte_SModifierFadingIn:
			/* start fade out from curr vol */
			SND_M_SModifierSetState(pst_ExtPlayer, SND_Cte_SModifierFadingOut);
			pst_ExtPlayer->st_Dyn.f_FadeOutDate = SND_f_InsertGetXvsY(pst_ExtPlayer->pst_FadeOut, _pst_SI->f_Volume);
			SND_M_SModifierSetState(pst_ExtPlayer, SND_Cte_SModifierFadingOut);
			ret = 1;
			break;

		default:
			ERR_X_Warning(0, "[SND][SNDmodifier.c] bad smd state", NULL);
			break;
		}
	}
	else
	{
		SND_M_SModifierSetState(pst_ExtPlayer, SND_Cte_SModifierInitState);
	}



    return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_SModifierExtPlayerNotifyPause(SND_tdst_SModifier *_pst_SMd, SND_tdst_SoundInstance *_pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pst_ExtPlayer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ExtPlayer = (SND_tdst_SModifierExtPlayer *) _pst_SMd->pv_Data;
	SND_M_SModifierSetState(pst_ExtPlayer, SND_Cte_SModifierPause);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_SModifierExtPlayerUpdate(SND_tdst_SModifier *_pst_Ext, SND_tdst_SoundInstance *_pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *pExtPlayer;
	float						vol;
	unsigned short				idx;
	SND_tdst_InsertState		*pInsertState;
	SND_tdst_Insert				*pInsert;
	float						fX, fY;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(_pst_Ext->i);
	ERR_X_Assert(_pst_Ext->i->id == SND_Cte_SModifierExtPlayer);
	pExtPlayer = (SND_tdst_SModifierExtPlayer *) _pst_Ext->pv_Data;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    FADE IN/OUT
	 -------------------------------------------------------------------------------------------------------------------
	 */

	switch(SND_M_SModifierGetState(pExtPlayer))
	{
	case SND_Cte_SModifierInitState:
	case SND_Cte_SModifierPause:
	case SND_Cte_SModifierPlaying:
		/* nothing to do */
		break;

	case SND_Cte_SModifierFadingIn:
		/* eval fade in */
		if(pExtPlayer->pst_FadeIn)
		{
			pExtPlayer->st_Dyn.f_FadeInDate += TIM_gf_dt;
			if(pExtPlayer->st_Dyn.f_FadeInDate < pExtPlayer->pst_FadeIn->Xaxis.pst_TimeDef->f_Max)
			{
				vol = pExtPlayer->st_Dyn.f_DryVol_Curr * SND_f_InsertGetYvsX
					(
                        pExtPlayer->pst_FadeIn,
						pExtPlayer->st_Dyn.f_FadeInDate,
                        SND_M_GetInstanceFromPtr(_pst_SI)
					);
				SND_SetVol(SND_M_GetInstanceFromPtr(_pst_SI), vol);
			}
			else
			{
				SND_SetVol(SND_M_GetInstanceFromPtr(_pst_SI), pExtPlayer->st_Dyn.f_DryVol_Curr);
				SND_M_SModifierSetState(pExtPlayer, SND_Cte_SModifierPlaying);
			}
		}
		else
		{
			SND_SetVol(SND_M_GetInstanceFromPtr(_pst_SI), pExtPlayer->st_Dyn.f_DryVol_Curr);
			SND_M_SModifierSetState(pExtPlayer, SND_Cte_SModifierPlaying);
		}
		break;

	case SND_Cte_SModifierFadingOut:
		/* eval fade out */
		if(pExtPlayer->pst_FadeOut)
		{
			pExtPlayer->st_Dyn.f_FadeOutDate += TIM_gf_dt;
			if(pExtPlayer->st_Dyn.f_FadeOutDate < pExtPlayer->pst_FadeOut->Xaxis.pst_TimeDef->f_Max)
			{
				vol = pExtPlayer->st_Dyn.f_DryVol_Curr * SND_f_InsertGetYvsX
					(
                        pExtPlayer->pst_FadeOut,
						pExtPlayer->st_Dyn.f_FadeOutDate,
                        SND_M_GetInstanceFromPtr(_pst_SI)
					);
				SND_SetVol(SND_M_GetInstanceFromPtr(_pst_SI), vol);
			}
			else
			{
				SND_SetVol(SND_M_GetInstanceFromPtr(_pst_SI), pExtPlayer->st_Dyn.f_DryVol_Curr);
				SND_Stop(SND_M_GetInstanceFromPtr(_pst_SI));
				SND_M_SModifierSetState(pExtPlayer, SND_Cte_SModifierInitState);
			}
		}
		else
		{
			SND_SetVol(SND_M_GetInstanceFromPtr(_pst_SI), pExtPlayer->st_Dyn.f_DryVol_Curr);
			SND_M_SModifierSetState(pExtPlayer, SND_Cte_SModifierInitState);
		}
		break;

	default:
		ERR_X_Warning(0, "[SND][SNDmodifier.c] bad smd state", NULL);
		break;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    INSERT
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(pExtPlayer->st_InsertList.us_Size)
	{
		pInsertState = pExtPlayer->st_InsertList.ast_State;
		for(idx = 0; idx < pExtPlayer->st_InsertList.us_Size; idx++, pInsertState++)
		{
			if(!pInsertState->b_IsActive) continue;
			pInsert = pExtPlayer->st_InsertList.app_PtrList[idx];

			fX = SND_f_SModifierGetCurrX(_pst_SI, pInsertState, pInsert);
			if(fX < 0.0f) continue;

            fY = SND_f_InsertGetYvsX(pInsert, fX, SND_M_GetInstanceFromPtr(_pst_SI));
			SND_SModifierSetCurrY(_pst_SI, pInsertState, pInsert, pExtPlayer, fY);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_SModifierSetCurrY
(
	SND_tdst_SoundInstance		*_pst_SI,
	SND_tdst_InsertState		*pInsertState,
	SND_tdst_Insert				*pInsert,
	SND_tdst_SModifierExtPlayer *pExtPlayer,
	float						fY
)
{
	/*
	 * sauver les valeurs vol+pan+freq dans l'insert state pour faire des modulations
	 * de la valeur
	 */
	switch(pInsert->Yaxis.pst_InsVoid->e_ChunkId)
	{
	case en_InsYaxisVol:
		if(pInsert->Yaxis.pst_VolDef->b_Wet)
			SND_SetWetVol(SND_M_GetInstanceFromPtr(_pst_SI), pExtPlayer->st_Dyn.f_DryVol_Curr * fY);
		else
			SND_SetVol(SND_M_GetInstanceFromPtr(_pst_SI), pExtPlayer->st_Dyn.f_DryVol_Curr * fY);
		break;

	case en_InsYaxisPan:
		fY = 20000.0f * (fY - 0.5f);
		if(pInsert->Yaxis.pst_PanDef->b_Surround)
			SND_SetPan(SND_M_GetInstanceFromPtr(_pst_SI), _pst_SI->i_Pan, lFloatToLong(fY));
		else
			SND_SetPan(SND_M_GetInstanceFromPtr(_pst_SI), lFloatToLong(fY), _pst_SI->i_Front);
		break;

	case en_InsYaxisFreq:
		SND_SetFreq(SND_M_GetInstanceFromPtr(_pst_SI), lFloatToLong(fY * pInsertState->f_SavY));
		break;

	default:
		ERR_X_Warning(0, "[SND][SNDmodifier.c] bad Insert config (Yaxis)", NULL);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static float SND_f_SModifierGetCurrX
(
	SND_tdst_SoundInstance	*_pst_SI,
	SND_tdst_InsertState	*pInsertState,
	SND_tdst_Insert			*pInsert
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				fX;
	MATH_tdst_Vector	stVector;
	MATH_tdst_Vector	stRef;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(pInsert->Xaxis.pst_InsVoid->e_ChunkId)
	{
	case en_InsXaxisDistance:
		if(!_pst_SI->pst_GlobalPos) return -1.0f;

		if(!pInsertState->pst_Gao)
		{
			pInsertState->pst_Gao = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_ul_SearchKeyToPos(pInsert->Xaxis.pst_DistDef->ui_GaoKey));
		}

		if((int) pInsertState->pst_Gao == -1)
			MATH_InitVectorToZero(&stRef);
		else
			MATH_CopyVector(&stRef, MATH_pst_GetTranslation(pInsertState->pst_Gao->pst_GlobalMatrix));

		MATH_SubVector(&stVector, &stRef, _pst_SI->pst_GlobalPos);
		switch(pInsert->Xaxis.pst_DistDef->ui_Axis)
		{
		case SND_Cte_InsAxisX:		fX = fAbs(stVector.x); break;
		case SND_Cte_InsAxisY:		fX = fAbs(stVector.y); break;
		case SND_Cte_InsAxisZ:		fX = fAbs(stVector.z); break;
		case SND_Cte_InsAxisXYZ:	fX = MATH_f_Distance(&stRef, _pst_SI->pst_GlobalPos); break;
		default:					ERR_X_Warning(0, "[SND][SNDmodifier.c] bad Insert config (Xaxis)", NULL); return -1.0f;
		}
		break;

	case en_InsXaxisTime:
		pInsertState->f_CurrX += TIM_gf_dt;
		fX = pInsertState->f_CurrX;
		break;

	case en_InsXaxisVar:
        fX = SND_f_InsertVarGet(SND_M_GetInstanceFromPtr(_pst_SI), pInsert->Xaxis.pst_VarDef->ui_VarId);
		break;

	default:
		ERR_X_Warning(0, "[SND] SNDmodifier.c : bad Insert config", NULL);
		return -1.0f;
	}

	return fX;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_SModifierExtPlayerDestroy
(
	SND_tdst_SModifier	*_pst_Ext,
	void				*_pv_Data,
	int					_i_DataSize,
	BOOL				b_Instance
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierExtPlayer *p_Src;
	unsigned short				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Src = (SND_tdst_SModifierExtPlayer *) _pv_Data;
	if(!b_Instance)
	{
		SND_InsertUnload(SND_i_InsertGetIndex(p_Src->ul_FadeInKey));
		SND_InsertUnload(SND_i_InsertGetIndex(p_Src->ul_FadeOutKey));

		if(p_Src->st_PlayList.us_Size)
		{
			for(i = 0; i < p_Src->st_PlayList.us_Size; i++)
			{
				SND_DeleteByIndex(p_Src->st_PlayList.al_IdxList[i]);
			}
		}
		else
		{
			SND_DeleteByIndex(p_Src->i_SndIndex);
		}

		if(p_Src->st_InsertList.us_Size)
		{
			for(i = 0; i < p_Src->st_InsertList.us_Size; i++)
			{
				SND_InsertUnload(SND_i_InsertGetIndex(p_Src->st_InsertList.aul_KeyList[i]));
			}
		}
	}

	if(p_Src->st_PlayList.al_IdxList) MEM_Free(p_Src->st_PlayList.al_IdxList);
	p_Src->st_PlayList.al_IdxList = NULL;

	if(p_Src->st_PlayList.aul_FlagList) MEM_Free(p_Src->st_PlayList.aul_FlagList);
	p_Src->st_PlayList.aul_FlagList = NULL;

	if(p_Src->st_PlayList.aul_KeyList) MEM_Free(p_Src->st_PlayList.aul_KeyList);
	p_Src->st_PlayList.aul_KeyList = NULL;

	if(p_Src->st_InsertList.app_PtrList) MEM_Free(p_Src->st_InsertList.app_PtrList);
	p_Src->st_InsertList.app_PtrList = NULL;

	if(p_Src->st_InsertList.aul_KeyList) MEM_Free(p_Src->st_InsertList.aul_KeyList);
	p_Src->st_InsertList.aul_KeyList = NULL;

	if(p_Src->st_InsertList.ast_State) MEM_Free(p_Src->st_InsertList.ast_State);
	p_Src->st_InsertList.ast_State = NULL;

	MEM_Free(_pst_Ext->pv_Data);
	_pst_Ext->pv_Data = NULL;
	_pst_Ext->ui_DataSize = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_SModifierActiveInsert(SND_tdst_SoundInstance *p, int idx, BOOL b_Active, BOOL b_Reinit)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifier			*pSMd;
	SND_tdst_SModifierExtPlayer *pExtPlayer;
	SND_tdst_Insert				*pInsert;
	SND_tdst_InsertState		*pInsertState;
	SND_tdst_SModifier			*_pSMd = p->pst_SModifier;
    float                      ff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pSMd = SND_pst_SModifierGet(_pSMd, SND_Cte_SModifierExtPlayer, 0);
	pExtPlayer = (SND_tdst_SModifierExtPlayer *) pSMd->pv_Data;

	if(!pExtPlayer) return -1;
	if(idx < 0) return -2;
	if(idx >= (int) pExtPlayer->st_InsertList.us_Size) return -2;

	pInsert = pExtPlayer->st_InsertList.app_PtrList[idx];
	pInsertState = &pExtPlayer->st_InsertList.ast_State[idx];

	if(b_Active)
	{
		if(b_Reinit)
		{
			switch(pInsert->Yaxis.pst_InsVoid->e_ChunkId)
			{
			case en_InsYaxisVol:
				break;

			case en_InsYaxisPan:
				pInsertState->f_SavY = pInsert->Yaxis.pst_PanDef->b_Surround ? fLongToFloat(p->i_Front) : fLongToFloat(p->i_Pan);
				if(pInsertState->f_SavY >= 0.0f)
					pInsertState->f_SavY = -SND_f_GetVolFromAtt(-lFloatToLong(pInsertState->f_SavY));
				else
					pInsertState->f_SavY = SND_f_GetVolFromAtt(lFloatToLong(pInsertState->f_SavY));
				break;

			case en_InsYaxisFreq:
				pInsertState->f_SavY = fLongToFloat(p->i_BaseFrequency);
				break;

			default:
				ERR_X_Warning(0, "[SND][SNDmodifier.c]", NULL);
				break;
			}

			pInsertState->f_CurrX = 0.0f;
		}
		pInsertState->b_IsActive = TRUE;
	}
	else
	{
		if(b_Reinit)
		{
			switch(pInsert->Yaxis.pst_InsVoid->e_ChunkId)
			{
			case en_InsYaxisVol:
				break;

			case en_InsYaxisPan:
				if(pInsertState->f_SavY < 0.0f)
					ff = -fLongToFloat(SND_l_GetAttFromVol(-pInsertState->f_SavY)) ;
				else
					ff = fLongToFloat(SND_l_GetAttFromVol(pInsertState->f_SavY)) ;
				                
                if(pInsert->Yaxis.pst_PanDef->b_Surround)
                    p->i_Front = lFloatToLong(ff);
                else
                    p->i_Pan = lFloatToLong(ff);
				break;

			case en_InsYaxisFreq:
				p->i_BaseFrequency = lFloatToLong(pInsertState->f_SavY);
				break;

			default:
				ERR_X_Warning(0, "[SND][SNDmodifier.c]", NULL);
				break;
			}

			pInsertState->f_CurrX = 0.0f;
		}
		pInsertState->b_IsActive = FALSE;
	}

	return 0;
}

/*$4
 ***********************************************************************************************************************
    INSERT
 ***********************************************************************************************************************
 */

#if 0
typedef struct	SND_tdst_SModifierInsert_
{
	ULONG					ul_Flags;
	BIG_KEY					ul_InsertKey;
	struct SND_tdst_Insert_ *pst_Insert;
	float					f_Date;
	void					*pGao;
} SND_tdst_SModifierInsert;
#define SND_Cte_SMdInsActive		0x00000001
#define SND_Cte_SMdInsPause			0x00000002
#define SND_Cte_SMdInsOnPlayActive	0x00000004
#define SND_Cte_SMdInsOnStopActive	0x00000008

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_SModifierInsertUpdate(SND_tdst_SModifier *_pst_Ext, SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SModifierInsert	*pSMdInsert;
	SND_tdst_Insert				*pInsert;
	float						fX, fY, tx, ty, prev_tx, prev_ty;
	MATH_tdst_Vector			stVector;
	unsigned int				i;
	SND_tdst_InsKey				*pKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(_pst_Ext->i);
	ERR_X_Assert(_pst_Ext->ui_DataSize == sizeof(SND_tdst_SModifierInsert));

	pSMdInsert = (SND_tdst_SModifierInsert *) _pst_Ext->pv_Data;
	pInsert = pSMdInsert->pst_Insert;

	/* active ? */
	if((pSMdInsert->ul_Flags & SND_Cte_SMdInsActive) == 0) return;

	/* in pause ? */
	if(pSMdInsert->ul_Flags & SND_Cte_SMdInsPause) return;

	switch(pInsert->Xaxis.pst_InsVoid->e_ChunkId)
	{
	case en_InsXaxisDistance:
		if(!pst_SI->pst_GlobalPos) return;
		if(!pSMdInsert->pGao)
		{
			pSMdInsert->pGao = (void *) LOA_ul_SearchAddress(BIG_ul_SearchKeyToPos(pInsert->Xaxis.pst_DistDef->ui_GaoKey));
		}

		if((int) pSMdInsert->pGao == -1)
		{
			switch(pInsert->Xaxis.pst_DistDef->ui_Axis)
			{
			case SND_Cte_InsAxisX:
				fX = fAbs(pst_SI->pst_GlobalPos->x);
				break;

			case SND_Cte_InsAxisY:
				fX = fAbs(pst_SI->pst_GlobalPos->y);
				break;

			case SND_Cte_InsAxisZ:
				fX = fAbs(pst_SI->pst_GlobalPos->z);
				break;

			default:

			case SND_Cte_InsAxisXYZ:
				fX = MATH_f_NormVector(pst_SI->pst_GlobalPos);
				break;
			}
		}
		else
		{
			MATH_SubVector
			(
				&stVector,
				MATH_pst_GetTranslation(((OBJ_tdst_GameObject *) pSMdInsert->pGao)->pst_GlobalMatrix),
				pst_SI->pst_GlobalPos
			);
			switch(pInsert->Xaxis.pst_DistDef->ui_Axis)
			{
			case SND_Cte_InsAxisX:
				fX = fAbs(stVector.x);
				break;

			case SND_Cte_InsAxisY:
				fX = fAbs(stVector.y);
				break;

			case SND_Cte_InsAxisZ:
				fX = fAbs(stVector.z);
				break;

			default:

			case SND_Cte_InsAxisXYZ:
				fX = MATH_f_Distance
					(
						MATH_pst_GetTranslation(((OBJ_tdst_GameObject *) pSMdInsert->pGao)->pst_GlobalMatrix),
						pst_SI->pst_GlobalPos
					);
				break;
			}
		}

		if(fX < pInsert->Xaxis.pst_DistDef->f_Min) return;
		if(fX > pInsert->Xaxis.pst_DistDef->f_Max) return;

		fX = (fX - pInsert->Xaxis.pst_DistDef->f_Min) / (pInsert->Xaxis.pst_DistDef->f_Max - pInsert->Xaxis.pst_DistDef->f_Min);
		break;

	case en_InsXaxisTime:
		if(pSMdInsert->f_Date < 0.0f) return;
		pSMdInsert->f_Date += TIM_gf_dt;

		if(pSMdInsert->f_Date < pInsert->Xaxis.pst_TimeDef->f_Min) return;
		if(pSMdInsert->f_Date > pInsert->Xaxis.pst_TimeDef->f_Max) return;

		fX = (pSMdInsert->f_Date - pInsert->Xaxis.pst_TimeDef->f_Min) / (pInsert->Xaxis.pst_TimeDef->f_Max - pInsert->Xaxis.pst_TimeDef->f_Min);
		break;

	case en_InsXaxisVar:
        fX = SND_f_InsertVarGet(SND_M_GetInstanceFromPtr(pst_SI), pInsert->Xaxis.pst_VarDef->ui_VarId);
		break;

	default:
		return;
	}

	pKey = pInsert->pst_KeyArray->dst_Key;
	tx = prev_tx = 0.0f;
	ty = prev_ty = pKey->tx;

	for(i = 1; i < pInsert->pst_KeyArray->ui_Size - 1; i++, pKey++)
	{
		tx = pKey->tx;
		ty = prev_ty + (tx - prev_tx) * pKey[-1].ty;

		prev_tx = tx;
		prev_ty = ty;

		if((prev_tx <= fX) && (fX <= tx)) break;
	}

	/* get last point */
	if(!((prev_tx <= fX) && (fX <= tx)))
	{
		tx = 1.0f;
		ty = pKey->tx;

		/* go out if the last point is not ok */
		if(!((prev_tx <= fX) && (fX <= tx))) return;
	}

	fY = MATH_f_FloatBlend(prev_ty, ty, (fX - prev_tx) / (tx - prev_tx));

	switch(pInsert->Yaxis.pst_InsVoid->e_ChunkId)
	{
	default:

	case en_InsYaxisVol:
		fY = MATH_f_FloatBlend(pInsert->Yaxis.pst_VolDef->f_Min, pInsert->Yaxis.pst_VolDef->f_Max, fY);
		if(pInsert->Yaxis.pst_VolDef->b_Wet)
			SND_SetWetVol(SND_M_GetInstanceFromPtr(pst_SI), fY);
		else
			SND_SetVol(SND_M_GetInstanceFromPtr(pst_SI), fY);
		break;

	case en_InsYaxisPan:
		fY = MATH_f_FloatBlend(pInsert->Yaxis.pst_PanDef->f_Min, pInsert->Yaxis.pst_PanDef->f_Max, fY);
		if(pInsert->Yaxis.pst_PanDef->b_Surround)
			SND_SetPan(SND_M_GetInstanceFromPtr(pst_SI), pst_SI->i_Pan, (int) fY);
		else
			SND_SetPan(SND_M_GetInstanceFromPtr(pst_SI), (int) fY, pst_SI->i_Front);
		break;

	case en_InsYaxisFreq:
		fY = MATH_f_FloatBlend(pInsert->Yaxis.pst_FreqDef->f_Min, pInsert->Yaxis.pst_FreqDef->f_Max, fY);
		SND_SetFreq(SND_M_GetInstanceFromPtr(pst_SI), (int) fY);
		break;
	}
}
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
