/*$T SNDstream.c GC 1.138 12/13/04 14:40:17 */


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

#include "BIGfiles/BIGread.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "LINks/LINKmsg.h"

/* sound module headers */
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDmacros.h"
#include "EDItors/Sources/SOuNd/SONutil.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDmusic.h"
#include "SouND/Sources/SNDambience.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDconv_xboxadpcm.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "TIMer/TIMdefs.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define M_GetStreamOrReturn(_id, _ptr, _ret) \
	do \
	{ \
		if((int) _id <= 0) return _ret; \
		if((int) _id > SND_gi_StreamListSize) return _ret; \
		_ptr = SND_gast_StreamList + (int) _id - 1; \
	} while(0);

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	SND_tdst_VirtualStreamSettings_
{
	unsigned int	ui_Flag;
	int				i_Volume;
	int				i_Pan;
	int				i_Front;
	int				i_Frequency;
	int				i_Pos;
} SND_tdst_VirtualStreamSettings;

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	SND_tdst_StreamPrefetch_
{
	LONG					l_SoundIndex;
	SND_tden_PrefetchState	en_State;
	ULONG					ul_Position;
    float                  f_LifeTime;
} SND_tdst_StreamPrefetch;

typedef struct	SND_tdst_StreamPrefetchManager_
{
	SND_tdst_StreamPrefetch *dst_List;
	LONG					l_ListSize;
	LONG					l_CurrentPrefetchNumber;
} SND_tdst_StreamPrefetchManager;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

SND_tdst_StreamPrefetchManager	SND_gst_PrefetchManager;
SND_tdst_VirtualStreamSettings	*SND_gast_StreamList = NULL;
int								SND_gi_StreamListSize = 0;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

int			SND_i_StreamPrefetchAlloc(void);
void		SND_StreamPrefetchFree(int _i_Index);
int			SND_i_StreamAlloc(void);
void		SND_StreamFree(int _i_Index);
static void SND_StreamFlushOneRq(SND_tdst_StreamPrefetch *pst_Prefetch);

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StreamInitModule(void)
{
	SND_gast_StreamList = NULL;
	SND_gi_StreamListSize = 0;
	SND_gast_StreamList[SND_i_StreamAlloc()].ui_Flag = 0;
	L_memset(&SND_gst_PrefetchManager, 0, sizeof(SND_tdst_StreamPrefetchManager));
	SND_gst_PrefetchManager.dst_List = (SND_tdst_StreamPrefetch*)MEM_p_Alloc(SND_Cte_StreamPrefetchMax * sizeof(SND_tdst_StreamPrefetch));
	L_memset(SND_gst_PrefetchManager.dst_List, 0, SND_Cte_StreamPrefetchMax * sizeof(SND_tdst_StreamPrefetch));
	SND_gst_PrefetchManager.l_ListSize = SND_Cte_StreamPrefetchMax;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StreamCloseModule(void)
{
	if(SND_gast_StreamList) MEM_Free(SND_gast_StreamList);
	SND_gast_StreamList = NULL;
	SND_gi_StreamListSize = 0;
	if(SND_gst_PrefetchManager.dst_List) MEM_Free(SND_gst_PrefetchManager.dst_List);
	L_memset(&SND_gst_PrefetchManager, 0, sizeof(SND_tdst_StreamPrefetchManager));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_StreamAlloc(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < SND_gi_StreamListSize; i++)
	{
		if(SND_gast_StreamList[i].ui_Flag == 0) break;
	}

	if(i == SND_gi_StreamListSize)
	{
		if(SND_gi_StreamListSize)
		{
			SND_gast_StreamList = (SND_tdst_VirtualStreamSettings *) MEM_p_Realloc
				(
					SND_gast_StreamList,
					(SND_gi_StreamListSize + SND_Cte_MinAllocInst) * sizeof(SND_tdst_VirtualStreamSettings)
				);
			L_memset
			(
				&SND_gast_StreamList[SND_gi_StreamListSize],
				0,
				SND_Cte_MinAllocInst * sizeof(SND_tdst_VirtualStreamSettings)
			);
			
			SND_gi_StreamListSize += SND_Cte_MinAllocInst;
		}
		else
		{
			SND_gi_StreamListSize = 2*SND_Cte_MinAllocSound ;
			SND_gast_StreamList = (SND_tdst_VirtualStreamSettings *) MEM_p_Alloc(SND_gi_StreamListSize  * sizeof(SND_tdst_VirtualStreamSettings));
			L_memset(SND_gast_StreamList, 0, SND_gi_StreamListSize  * sizeof(SND_tdst_VirtualStreamSettings));
		}

	}

	SND_gast_StreamList[i].ui_Flag = SND_Cul_DSF_Used;
	return(i + 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StreamFree(int _i_Index)
{
	if(_i_Index <= 0) return;
	if(_i_Index > SND_gi_StreamListSize) return;
	L_memset(&SND_gast_StreamList[_i_Index - 1], 0, sizeof(SND_tdst_VirtualStreamSettings));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_AddStreamLoad(BIG_KEY _ul_WavKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				l_Index;
	SND_tdst_OneSound	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;

	/*$2- Test existence ---------------------------------------------------------------------------------------------*/

	l_Index = SND_l_GetSoundIndex(_ul_WavKey);
	if(l_Index == -1) return -1;

	pst_Sound = SND_gst_Params.dst_Sound + l_Index;
	if(pst_Sound->ul_Flags & SND_Cul_OSF_Loaded) return l_Index;

	/*$2- parse header & send load request ---------------------------------------------------------------------------*/

	if(!SND_M_IsGoodFormat(pst_Sound->pst_Wave->wFormatTag)) return -1;

	/*$2- update sound struct ----------------------------------------------------------------------------------------*/

	pst_Sound->pst_DSB = (SND_tdst_SoundBuffer *) SND_i_StreamAlloc();
	if((int) pst_Sound->pst_DSB <= 0) return -1;
	pst_Sound->ul_Flags |= SND_Cul_OSF_Loaded;

	return(l_Index);
}

/*$4
 ***********************************************************************************************************************
    Low Interface:: in normal case, there is no action to do, because it is done by track interface when playing
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_Stream_Play(SND_tdst_SoundBuffer *p, int i1, int i2, int i3)
{
	ERR_X_Warning(0, "[SND] bad stream action", NULL);
	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_Pause(SND_tdst_SoundBuffer *p)
{
	ERR_X_Warning(0, "[SND] bad stream action", NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_Duplicate
(
	SND_tdst_TargetSpecificData *_pst_SpeData,
	SND_tdst_SoundBuffer		*_pst_SrcSB,
	SND_tdst_SoundBuffer		**_ppst_DstSB
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_VirtualStreamSettings *pst_Stream, *pst_StreamD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_ppst_DstSB = NULL;
	M_GetStreamOrReturn(_pst_SrcSB, pst_Stream, ;);

	*_ppst_DstSB = (SND_tdst_SoundBuffer *) SND_i_StreamAlloc();
	M_GetStreamOrReturn(*_ppst_DstSB, pst_StreamD, ;);

	pst_StreamD->i_Frequency = pst_Stream->i_Frequency;
	pst_StreamD->i_Front = pst_Stream->i_Front;
	pst_StreamD->i_Pan = pst_Stream->i_Pan;
	pst_StreamD->i_Pos = 0;
	pst_StreamD->i_Volume = pst_Stream->i_Volume;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *SND_Stream_Create(SND_tdst_TargetSpecificData *p, SND_tdst_WaveData *pp, unsigned int ui)
{
	ERR_X_Warning(0, "[SND] bad stream action", NULL);
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_Stop(SND_tdst_SoundBuffer *_pst_SB)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_Release(SND_tdst_SoundBuffer *_pst_SB)
{
	SND_StreamFree((int) _pst_SB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_SetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int _i_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_VirtualStreamSettings	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_GetStreamOrReturn(_pst_SB, pst_Stream, ;);
	pst_Stream->i_Pos = _i_Pos;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_SetVolume(SND_tdst_SoundBuffer *_pst_SB, int _i_Vol)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_VirtualStreamSettings	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_GetStreamOrReturn(_pst_SB, pst_Stream, ;);
	pst_Stream->i_Volume = _i_Vol;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_SetFrequency(SND_tdst_SoundBuffer *_pst_SB, int _i_Freq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_VirtualStreamSettings	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_GetStreamOrReturn(_pst_SB, pst_Stream, ;);
	pst_Stream->i_Frequency = _i_Freq;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_SetPan(SND_tdst_SoundBuffer *_pst_SB, int _i_Pan, int _i_FrontRear)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_VirtualStreamSettings	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_GetStreamOrReturn(_pst_SB, pst_Stream, ;);
	pst_Stream->i_Pan = _i_Pan;
	pst_Stream->i_Front = _i_FrontRear;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_Stream_GetStatus(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Status)
{
	*_pi_Status = 0;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_GetFrequency(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Freq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_VirtualStreamSettings	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_GetStreamOrReturn(_pst_SB, pst_Stream, ;);
	*_pi_Freq = pst_Stream->i_Frequency;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_GetPan(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pan, int *_pi_Front)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_VirtualStreamSettings	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_GetStreamOrReturn(_pst_SB, pst_Stream, ;);
	*_pi_Pan = pst_Stream->i_Pan;
	*_pi_Front = pst_Stream->i_Front;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stream_GetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pos, int *_pi_Write)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_VirtualStreamSettings	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_GetStreamOrReturn(_pst_SB, pst_Stream, ;);
	*_pi_Pos = pst_Stream->i_Pos;
	*_pi_Write = 0;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StreamPrefetch(LONG _l_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Instance, pst_SI, ;);
	SND_StreamPrefetchOneSound(pst_SI->l_Sound, SND_Cte_StreamPrefetchAsync);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StreamFlush(LONG _l_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	SND_tdst_OneSound		*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Instance, pst_SI, ;);
	pst_Sound = SND_gst_Params.dst_Sound + pst_SI->l_Sound;

	if((pst_Sound->ul_Flags & (SND_Cul_SF_Music | SND_Cul_SF_Ambience | SND_Cul_SF_Dialog)) == 0)
	{
		SND_EdiRedWarningMsgKey("SND_StreamFluch", "file is not streamed", pst_Sound->ul_FileKey);
		return;
	}

	if(!pst_Sound->pst_Wave->ul_DataPosition)
	{
		SND_EdiRedWarningMsgKey("SND_StreamFluch", "bad position", pst_Sound->ul_FileKey);
		return;
	}

	SND_StreamFlushOneSound(pst_SI->l_Sound, TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_StreamPrefetchOneSound(LONG _l_SoundIndex, int i_mode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_OneSound		*pst_Sound;
	SND_tdst_StreamPrefetch *pst_Prefetch;
	int						i, iFree;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    check sound
	 -------------------------------------------------------------------------------------------------------------------
	 */
	SND_M_GetSoundOrReturn(_l_SoundIndex, pst_Sound, -1);
    if(pst_Sound->pst_SModifier)
	{
        _l_SoundIndex = (LONG)pst_Sound->pst_DSB ;
        SND_M_GetSoundOrReturn(_l_SoundIndex, pst_Sound, -1);
    }

	if((pst_Sound->ul_Flags & (SND_Cul_SF_Music | SND_Cul_SF_Ambience | SND_Cul_SF_Dialog)) == 0)
	{
		SND_EdiRedWarningMsgKey("SND_StreamPrefetch", "file is not streamed", pst_Sound->ul_FileKey);
		return -1;
	}

	if(!pst_Sound->pst_Wave->ul_DataPosition)
	{
		SND_EdiRedWarningMsgKey("SND_StreamPrefetch", "bad position", pst_Sound->ul_FileKey);
		return -1;
	}

	if(!pst_Sound->pst_Wave->ul_DataSize)
	{
		SND_EdiBlackWarningMsgKey("SND_StreamPrefetch", "bad size", pst_Sound->ul_FileKey);
		return -1;
	}
/*
#ifdef ACTIVE_EDITORS
	if(SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
	{
		if(SND_ui_GetDecompressedSize(pst_Sound->pst_Wave->ul_DataSize) < (SND_Cte_MaxBufferSize / 2))
		{
			SND_EdiRedWarningMsgKey("SND_StreamPrefetch", "too small size", pst_Sound->ul_FileKey);
			return -1;
		}
	}
	else if(pst_Sound->pst_Wave->ul_DataSize < (SND_Cte_MaxBufferSize / 2))
	{
		SND_EdiRedWarningMsgKey("SND_StreamPrefetch", "too small size", pst_Sound->ul_FileKey);
		return -1;
	}

#else
	if(pst_Sound->pst_Wave->ul_DataSize < (SND_Cte_MaxBufferSize / 2))
	{
		SND_EdiRedWarningMsgKey("SND_StreamPrefetch", "too small size", pst_Sound->ul_FileKey);
		return -1;
	}
#endif
	*/

	if(pst_Sound->ul_Flags & SND_Cul_SF_SModifier)
	{
		SND_M_GetSoundOrReturn((int) pst_Sound->pst_DSB, pst_Sound, -1);
	}

#ifdef ACTIVE_EDITORS
	if(!(pst_Sound->ul_Flags & (SND_Cul_SF_Music | SND_Cul_SF_Dialog | SND_Cul_SF_Ambience)))
	{
		SND_EdiRedWarningMsgKey("SND_StreamPrefetchOneSound", "Try to prefetch one wav file", pst_Sound->ul_FileKey);
		return -1;
	}
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    get free or quit if yet registered
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_Prefetch = SND_gst_PrefetchManager.dst_List;
	iFree = -1;
	for(i = 0; i < SND_gst_PrefetchManager.l_ListSize; i++, pst_Prefetch++)
	{
		if(pst_Prefetch->en_State == e_PrefecthRequestFree)
		{
			if(iFree == -1) iFree = i;
			continue;
		}

		if(pst_Prefetch->l_SoundIndex == _l_SoundIndex)
		{
			return i;
		}
	}

	if(iFree == -1)
	{
		SND_EdiRedWarningMsg("SND_StreamPrefetchOneSound", "Caution you are trying to prefecth too much stream file");
		return -1;
	}

	pst_Prefetch = SND_gst_PrefetchManager.dst_List + iFree;
	switch(i_mode)
	{
	case SND_Cte_StreamPrefetchSync:
		SND_gst_PrefetchManager.l_CurrentPrefetchNumber++;
		pst_Prefetch->en_State = e_PrefecthRequestWaitingForSyncAction;
		pst_Prefetch->l_SoundIndex = _l_SoundIndex;
        pst_Prefetch->f_LifeTime = 0.0f;
		break;

	case SND_Cte_StreamPrefetchAsync:
		if(!pst_Sound->pst_Wave->ul_DataPosition)
		{
			SND_EdiBlackWarningMsgKey
			(
				"SND_StreamPrefetchOneSound",
				"Caution you are trying to prefecth one unloaded stream file",
				pst_Sound->ul_FileKey
			);
			return -1;
		}

		_SND_DebugSpy(pst_Sound->ul_FileKey, SND_StreamPrefetchOneSound);
#if defined(_XENON)
		xeSND_StreamPrefetch(pst_Sound->pst_Wave->ul_DataPosition, pst_Sound->pst_Wave->ul_DataSize, pst_Sound->pst_Wave->cSoundBFIndex);
#else
		SND_gpst_Interface[ISound].pfv_StreamPrefetch(pst_Sound->pst_Wave->ul_DataPosition, pst_Sound->pst_Wave->ul_DataSize);
#endif
		pst_Prefetch->ul_Position = pst_Sound->pst_Wave->ul_DataPosition;
		SND_gst_PrefetchManager.l_CurrentPrefetchNumber++;

		pst_Prefetch->en_State = e_PrefecthRequestRunningAsync;
		pst_Prefetch->l_SoundIndex = _l_SoundIndex;
        pst_Prefetch->f_LifeTime = 0.0f;
		break;

	default:
		return -1;
	}

    return iFree;
}

void SND_StreamPrefetchSetLife(int idx, float fTime)
{
    SND_tdst_StreamPrefetch *pst_Prefetch;
    
    if(idx <0) return;
    if(idx >= SND_gst_PrefetchManager.l_ListSize) return;

    pst_Prefetch = SND_gst_PrefetchManager.dst_List + idx;
    if(pst_Prefetch->en_State == e_PrefecthRequestFree) return;

    pst_Prefetch->f_LifeTime = fTime;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StreamResolveSyncPrefetch(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	SND_tdst_StreamPrefetch *pst_Prefetch;
	SND_tdst_OneSound		*pst_Sound;
	ULONG					aul_Position[SND_Cte_StreamPrefetchMax];
#if defined(_XENON)
	char					ac_SoundBFIndex[SND_Cte_StreamPrefetchMax];
#endif
	ULONG					aul_Size[SND_Cte_StreamPrefetchMax];
	ULONG					aul_Index[SND_Cte_StreamPrefetchMax];
	int						rqnb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	L_memset(aul_Position, 0, SND_Cte_StreamPrefetchMax * sizeof(ULONG));
	L_memset(aul_Size, 0, SND_Cte_StreamPrefetchMax * sizeof(ULONG));
	rqnb = 0;

	for(i = 0; i < SND_gst_PrefetchManager.l_ListSize; i++)
	{
		pst_Prefetch = SND_gst_PrefetchManager.dst_List + i;
		if(pst_Prefetch->en_State != e_PrefecthRequestWaitingForSyncAction) continue;
		pst_Sound = SND_gst_Params.dst_Sound + pst_Prefetch->l_SoundIndex;

		/*$2- check sound state --------------------------------------------------------------------------------------*/

		if(!pst_Sound->pst_Wave->ul_DataPosition)
		{
			SND_EdiRedWarningMsgKey("SND_StreamResolveSyncPrefetch", "Bad file type", pst_Sound->ul_FileKey);
			continue;
		}

		/*$2----------------------------------------------------------------------------------------------------------*/

		_SND_DebugSpy(pst_Sound->ul_FileKey, SND_StreamResolveSyncPrefetch);
		pst_Prefetch->ul_Position = pst_Sound->pst_Wave->ul_DataPosition;
		aul_Index[rqnb] = i;
		aul_Size[rqnb] = pst_Sound->pst_Wave->ul_DataSize;
#if defined(_XENON)
		ac_SoundBFIndex[rqnb] = pst_Sound->pst_Wave->cSoundBFIndex;
#endif
		aul_Position[rqnb++] = pst_Prefetch->ul_Position;
		pst_Prefetch->en_State = e_PrefecthRequestWaitingForUsing;
        pst_Prefetch->f_LifeTime = 0.0f;
	}

	if(rqnb)
	{

		/*$2- add array of position ----------------------------------------------------------------------------------*/

#if defined(_XENON)
		xeSND_StreamPrefetchArray((unsigned int*)aul_Position, (unsigned int*)aul_Size, ac_SoundBFIndex);
#else
		SND_gpst_Interface[ISound].pfv_StreamPrefetchArray((unsigned int*)aul_Position, (unsigned int*)aul_Size);
#endif

		for(i = 0; i < rqnb; i++)
		{
			pst_Prefetch = SND_gst_PrefetchManager.dst_List + aul_Index[i];

			if(aul_Position[i])
			{
				/* request has failed */
				SND_gst_PrefetchManager.l_CurrentPrefetchNumber--;
				pst_Prefetch->en_State = e_PrefecthRequestFree;
				pst_Prefetch->ul_Position = 0;
				pst_Prefetch->l_SoundIndex = -1;
                pst_Prefetch->f_LifeTime = 0.0f;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StreamFlushOneSound(LONG _l_Sound, BOOL b_DoFlush)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_OneSound		*pst_Sound;
	SND_tdst_StreamPrefetch *pst_Prefetch;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(!SND_gst_PrefetchManager.l_CurrentPrefetchNumber) return;

	SND_M_GetSoundOrReturn(_l_Sound, pst_Sound, ;);

	for(i = 0; i < SND_gst_PrefetchManager.l_ListSize; i++)
	{
		pst_Prefetch = SND_gst_PrefetchManager.dst_List + i;
		if(pst_Prefetch->en_State == e_PrefecthRequestFree) continue;
		if(pst_Prefetch->l_SoundIndex != _l_Sound) continue;

		SND_gst_PrefetchManager.l_CurrentPrefetchNumber--;
		switch(pst_Prefetch->en_State)
		{
		case e_PrefecthRequestWaitingForUsing:
		case e_PrefecthRequestRunningAsync:
			if(b_DoFlush) SND_gpst_Interface[ISound].pfv_StreamFlush(pst_Prefetch->ul_Position);
			break;

		default:
			if(!b_DoFlush)
			{
				SND_EdiBlackWarningMsgKey
				(
					"SND_StreamDeletePrefetchReference",
					"play one sound without using prefetch",
					pst_Sound->ul_FileKey
				);
			}
			break;
		}

		pst_Prefetch->en_State = e_PrefecthRequestFree;
		pst_Prefetch->ul_Position = 0;
		pst_Prefetch->l_SoundIndex = -1;
        pst_Prefetch->f_LifeTime = 0.0f;
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_StreamFlushOneRq(SND_tdst_StreamPrefetch *pst_Prefetch)
{
	if(!SND_gst_Params.l_Available) return;
	if(!SND_gst_PrefetchManager.l_CurrentPrefetchNumber) return;
	if(pst_Prefetch->en_State == e_PrefecthRequestFree) return;

	SND_gst_PrefetchManager.l_CurrentPrefetchNumber--;

	switch(pst_Prefetch->en_State)
	{
	case e_PrefecthRequestWaitingForUsing:
	case e_PrefecthRequestRunningAsync:
		SND_gpst_Interface[ISound].pfv_StreamFlush(pst_Prefetch->ul_Position);
		break;

	default:
		break;
	}

	pst_Prefetch->en_State = e_PrefecthRequestFree;
	pst_Prefetch->ul_Position = 0;
	pst_Prefetch->l_SoundIndex = -1;
    pst_Prefetch->f_LifeTime = 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_StreamSoundIsPrefetched(LONG _l_Sound)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_OneSound		*pst_Sound;
	SND_tdst_StreamPrefetch *pst_Prefetch;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return 0;
	if(!SND_gst_PrefetchManager.l_CurrentPrefetchNumber) return 0;

	SND_M_GetSoundOrReturn(_l_Sound, pst_Sound, 0);

	for(i = 0; i < SND_gst_PrefetchManager.l_ListSize; i++)
	{
		pst_Prefetch = SND_gst_PrefetchManager.dst_List + i;
		if(pst_Prefetch->en_State == e_PrefecthRequestFree) continue;
		if(pst_Prefetch->l_SoundIndex == _l_Sound)
		{
			return(pst_Prefetch->en_State);
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StreamPrefetchUpdate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i, j;
	SND_tdst_StreamPrefetch *pst_Prefetch;
	ULONG					aul_Position[SND_Cte_StreamPrefetchMax];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(aul_Position, 0, SND_Cte_StreamPrefetchMax * sizeof(ULONG));
	SND_gpst_Interface[ISound].pfv_StreamGetPrefetchStatus((unsigned int*)aul_Position);



	for(i = 0; i < SND_gst_PrefetchManager.l_ListSize; i++)
	{
		pst_Prefetch = SND_gst_PrefetchManager.dst_List + i;
		if(!pst_Prefetch->ul_Position) continue;

        
        if((pst_Prefetch->en_State == e_PrefecthRequestWaitingForUsing) && (pst_Prefetch->f_LifeTime > 0.0f))
        {
            // update life
            pst_Prefetch->f_LifeTime -= TIM_gf_dt;
            
            // time out !
            if(pst_Prefetch->f_LifeTime <= 0.0f)
            {
                SND_StreamFlushOneRq(pst_Prefetch);
                continue;
            }
        }

		if
		(
			(pst_Prefetch->en_State != e_PrefecthRequestWaitingForSyncAction)
		&&	(pst_Prefetch->en_State != e_PrefecthRequestRunningAsync)
		) continue;

	
        for(j = 0; j < SND_Cte_StreamPrefetchMax; j++)
		{
			if(aul_Position[j] != pst_Prefetch->ul_Position) continue;
			pst_Prefetch->en_State = e_PrefecthRequestWaitingForUsing;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StreamPrefetchFlushAll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_StreamPrefetch *pst_Prefetch;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	for(i = 0; i < SND_gst_PrefetchManager.l_ListSize; i++)
	{
		pst_Prefetch = SND_gst_PrefetchManager.dst_List + i;
		if(pst_Prefetch->en_State == e_PrefecthRequestFree) continue;

		SND_StreamFlushOneRq(pst_Prefetch);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StreamStopAll(void)
{
	if(!SND_gst_Params.l_Available) return;
	SND_gpst_Interface[ISound].pfv_StreamShutDownAll();
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
