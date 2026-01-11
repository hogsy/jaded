/*$T SNDtrack.c GC 1.138 02/24/05 19:01:07 */


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

#include "Precomp.h"

/*$1- low level ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"

/*$1- sound functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDmacros.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/Sources/SNDmodifier.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
#define Dbg_LocalDisplay(str)	ERR_X_Warning(0, str, NULL)
#elif defined(_XBOX)
#define Dbg_LocalDisplay(str)	OutputDebugString(str)
#elif defined(PSX2_TARGET)
#define Dbg_LocalDisplay(str)	printf("%s\n", str)
#elif defined(_GAMECUBE)
#define Dbg_LocalDisplay(str)	OSReport("%s\n", str)
#else
#define Dbg_LocalDisplay(str)
#endif

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	SND_tdst_Track_
{
	unsigned int					ui_Flag;
	int								i_PlayingInstance;
	struct SND_tdst_SoundBuffer_	*pst_PlayerSB;
	int								i_Ressource;

	int								i_ChainInstance;
	float							f_ChainInstDelay;

	ULONG							ul_PrevPosition;
} SND_tdst_Track;

typedef struct	SND_tdst_TrackRessources_
{
	SND_tdst_SoundBuffer	*pst_MonoSB;
	SND_tdst_SoundBuffer	*pst_StereoSB;
	BOOL					b_Busy;
} SND_tdst_TrackRessources;

typedef struct	SND_tdst_ActiveSignalList_
{
	ULONG	ul_Size;
	ULONG	ul_Element;
	ULONG	*dul_Table;
} SND_tdst_ActiveSignalList;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

SND_tdst_Track				SND_gdst_TrackPlayingList[SND_e_TrackNb];
SND_tdst_TrackRessources	SND_gast_TrackRessources[SND_Cte_MaxSimultaneousStream];
SND_tdst_ActiveSignalList	SND_gst_CurrentSignal;
SND_tdst_ActiveSignalList	SND_gst_CurrentRegion;

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

static void SND_Track_Merge
			(
				int _i_TrueTrackId,
				SND_tdst_SoundInstance *,
				SND_tdst_SoundInstance *,
				int _i_NewFlag,
				int _i_LoopNb,
				int pos,
				int pp
			);

static int	SND_i_TrackSwapMonoToStereo(int _i_TrueTrackId);
static int	SND_i_TrackSwapStereoToMono(int _i_TrueTrackId);
static void SND_TrackAllocRessources(int _i_TrueTrack);
static void SND_TrackFreeRessources(int _i_TrueTrack);
static void SND_TrackFree(int _i_Track);
static void SND_DetectMarker(SND_tdst_WaveDesc *pWave, ULONG ulPos1, ULONG ulPos2);
static void SND_DetectRegion(SND_tdst_WaveDesc *pWave, ULONG ulPos1, ULONG ulPos2);


/*$4
 ***********************************************************************************************************************
    module
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_TrackInitModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	/*$1- special track init ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_RasterRegisterFile(SND_Cul_SF_Music);
	for(i = 0; i < SND_Cte_MaxSimultaneousStream; i++)
	{
		SND_gpst_Interface[ISound].pfv_SndBuffCreateTrack
			(
				&SND_gast_TrackRessources[i].pst_MonoSB,
				&SND_gast_TrackRessources[i].pst_StereoSB
			);
		SND_gast_TrackRessources[i].b_Busy = FALSE;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(SND_gdst_TrackPlayingList, 0, SND_e_TrackNb * sizeof(SND_tdst_Track));
	for(i = SND_e_TrackA; i < SND_e_TrackNb; i++)
	{
		SND_gdst_TrackPlayingList[i].i_PlayingInstance = -1;
		SND_gdst_TrackPlayingList[i].ul_PrevPosition = -1;
		SND_gdst_TrackPlayingList[i].ui_Flag = 0;
		SND_gdst_TrackPlayingList[i].pst_PlayerSB = NULL;
		SND_gdst_TrackPlayingList[i].i_Ressource = -1;
		SND_gdst_TrackPlayingList[i].i_ChainInstance = -1;
		SND_gdst_TrackPlayingList[i].f_ChainInstDelay = 0.0f;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&SND_gst_CurrentSignal, 0, sizeof(SND_tdst_ActiveSignalList));
	L_memset(&SND_gst_CurrentRegion, 0, sizeof(SND_tdst_ActiveSignalList));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_TrackCloseModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < SND_Cte_MaxSimultaneousStream; i++)
	{
		SND_gpst_Interface[ISound].pfv_SndBuffReleaseTrack
			(
				SND_gast_TrackRessources[i].pst_MonoSB,
				SND_gast_TrackRessources[i].pst_StereoSB
			);
	}

	if(SND_gst_CurrentSignal.dul_Table) MEM_Free(SND_gst_CurrentSignal.dul_Table);
	SND_gst_CurrentSignal.dul_Table = NULL;
	SND_gst_CurrentSignal.ul_Element = 0;
	SND_gst_CurrentSignal.ul_Size = 0;

	if(SND_gst_CurrentRegion.dul_Table) MEM_Free(SND_gst_CurrentRegion.dul_Table);
	SND_gst_CurrentRegion.dul_Table = NULL;
	SND_gst_CurrentRegion.ul_Element = 0;
	SND_gst_CurrentRegion.ul_Size = 0;
}

/*$4
 ***********************************************************************************************************************
    one track
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_TrackFree(int i_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_CheckTrackIdOrReturn(i_Track, ;);
	

	/*$1- delete instance reference ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SI = SND_gst_Params.dst_Instance;
	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++, pst_SI++)
	{
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;
		if(pst_SI->i_InstTrack != i_Track) continue;

		pst_SI->pst_LI = &SND_gpst_Interface[IStream];
		pst_SI->ul_Flags &= ~SND_Cul_SF_StreamIsPlayingNow;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_TrackDetachInstance(SND_gdst_TrackPlayingList[i_Track].i_PlayingInstance);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(SND_gdst_TrackPlayingList[i_Track].ui_Flag & SND_Cte_TrackIsStereo) SND_i_TrackSwapStereoToMono(i_Track);
	SND_TrackFreeRessources(i_Track);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_gdst_TrackPlayingList[i_Track].pst_PlayerSB = NULL;
	SND_gdst_TrackPlayingList[i_Track].i_Ressource = -1;
	SND_gdst_TrackPlayingList[i_Track].i_PlayingInstance = -1;
	SND_gdst_TrackPlayingList[i_Track].ul_PrevPosition = -1;
	SND_gdst_TrackPlayingList[i_Track].ui_Flag = 0;
	SND_gdst_TrackPlayingList[i_Track].i_ChainInstance = -1;
	SND_gdst_TrackPlayingList[i_Track].f_ChainInstDelay = 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_TrackFreeAll(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = SND_e_TrackA; i < SND_e_TrackNb; i++)
	{
		SND_TrackFree(i);
	}
}

/*$4
 ***********************************************************************************************************************
    track list
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_InstTrackSet(int _l_Index, int _i_Track)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	SND_M_CheckTrackIdOrReturn(_i_Track, ;);

	if(_i_Track == pst_SI->i_InstTrack) return;
	if(pst_SI->i_InstTrack != -1) SND_TrackDetachInstance(_l_Index);
	pst_SI->i_InstTrack = _i_Track;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

// TRUE  => Next dialog should be played on track C.
// FALSE => Next dialog should be played on track D.
BOOL SND_DialogInstChooseTrack()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_InstanceC, *pst_InstanceD;
	SND_tdst_OneSound		*pst_SoundC;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Is TrackD occupied ?
	if(SND_gdst_TrackPlayingList[SND_e_TrackD].i_PlayingInstance != -1)
	{
		SND_M_GetInstanceOrReturn(
			SND_gdst_TrackPlayingList[SND_e_TrackC].i_PlayingInstance,
			pst_InstanceC,
			TRUE;);
		SND_M_GetSoundOrReturn(pst_InstanceC->l_Sound, pst_SoundC, TRUE;);

		if (!(pst_SoundC->ul_Flags & SND_Cul_SF_Dialog))
			return TRUE;
		else
		{
			// Virer le dialog le plus vieux
			SND_M_GetInstanceOrReturn(
				SND_gdst_TrackPlayingList[SND_e_TrackD].i_PlayingInstance,
				pst_InstanceD,
				FALSE;);
			return pst_InstanceC->f_StartDate < pst_InstanceD->f_StartDate;
		}
	}
	return FALSE;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_TrackDetachInstance(int _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						real;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	pst_SI->ul_Flags &= ~(SND_Cul_SF_StreamIsPlayingNow);

	real = pst_SI->i_InstTrack;
	SND_M_CheckTrackIdOrReturn(real, ;);

	if(SND_gdst_TrackPlayingList[real].i_PlayingInstance == _l_Index)
	{
		SND_gpst_Interface[ISound].pfv_SndBuffStop(SND_gdst_TrackPlayingList[real].pst_PlayerSB);
		pst_SI->pst_LI = &SND_gpst_Interface[IStream];

		SND_gdst_TrackPlayingList[real].i_PlayingInstance = -1;
		SND_gdst_TrackPlayingList[real].ul_PrevPosition = -1;

		if(SND_gdst_TrackPlayingList[real].ui_Flag & SND_Cte_TrackIsStereo) SND_i_TrackSwapStereoToMono(real);
		SND_TrackFreeRessources(real);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_TrackAttachAndPlayInstance
(
	int _l_Index,
	int _i_Flag,
	int _i_LoopCount,
	int _i_StartPosition,
	int _i_StopPosition
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i_InstTrueTrack;
	SND_tdst_SoundInstance	*pst_SI, *pst_PrevSI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	/* init */
	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	pst_PrevSI = NULL;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((pst_SI->ul_Flags & (SND_Cul_SF_Music | SND_Cul_SF_Ambience | SND_Cul_SF_Dialog)) == 0)
	{
		SND_EdiRedWarningMsgKey
		(
			"SND_TrackAttachAndPlayInstance",
			"Prohibited action : trying to play one WAV file on track",
			SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey
		);
		return;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_InstTrueTrack = pst_SI->i_InstTrack;
	ERR_X_Warning(i_InstTrueTrack != -1, "[SND] unknown track id", NULL);
	ERR_X_Warning(i_InstTrueTrack >= SND_e_TrackA, "[SND] unknown track id", NULL);
	ERR_X_Warning(i_InstTrueTrack < SND_e_TrackNb, "[SND] unknown track id", NULL);
	SND_M_CheckTrackIdOrReturn(i_InstTrueTrack, ;);

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(SND_gdst_TrackPlayingList[i_InstTrueTrack].i_PlayingInstance != -1)
	{
		/* track is yet registered and there is yet an another one */
		pst_PrevSI = SND_gst_Params.dst_Instance + SND_gdst_TrackPlayingList[i_InstTrueTrack].i_PlayingInstance;
		pst_PrevSI->ul_Flags &= ~(SND_Cul_SF_PlayingLoop | SND_Cul_SF_Playing | SND_Cul_SF_StreamIsPlayingNow);
		pst_PrevSI->pst_LI = &SND_gpst_Interface[IStream];
	}

	SND_gdst_TrackPlayingList[i_InstTrueTrack].i_PlayingInstance = _l_Index;
	SND_gdst_TrackPlayingList[i_InstTrueTrack].ul_PrevPosition = -1;
	pst_SI->pst_LI = &SND_gpst_Interface[ITrack];

	SND_Track_Merge(i_InstTrueTrack, pst_PrevSI, pst_SI, _i_Flag, _i_LoopCount, _i_StartPosition, _i_StopPosition);

	pst_SI->ul_Flags &= ~(SND_Cul_SF_StreamIsPlayingNow | SND_Cul_SF_Playing | SND_Cul_SF_PlayingLoop);
	if(_i_Flag & SND_Cul_SBC_PlayLooping)
		pst_SI->ul_Flags |= SND_Cul_SF_PlayingLoop;
	else
		pst_SI->ul_Flags |= SND_Cul_SF_Playing;
}

/*$4
 ***********************************************************************************************************************
    AI interface
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_IsTrackPlaying(int track)
{
	LONG l_Index;
	SND_tdst_SoundInstance*pst_SI;

	if(!SND_gst_Params.l_Available) return 0;
	SND_M_CheckTrackIdOrReturn(track, 0);

	
	if(SND_gdst_TrackPlayingList[track].i_PlayingInstance >= 0)
	{
		return SND_i_IsPlaying(SND_gdst_TrackPlayingList[track].i_PlayingInstance);
	}
	else
	{
		// looking for any one pending instance on the track
		for(l_Index=0; l_Index<SND_gst_Params.l_InstanceNumber; l_Index++)
		{
			pst_SI = SND_gst_Params.dst_Instance + l_Index;
			if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
			if(pst_SI->i_InstTrack != track) continue;
			if(pst_SI->f_Delay)
				return 1;
		}
		return 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_TrackStop(int track)
{
	if(!SND_gst_Params.l_Available) return;
	SND_M_CheckTrackIdOrReturn(track, ;);
	SND_StopRq(SND_gdst_TrackPlayingList[track].i_PlayingInstance);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_TrackAllocRessources(int _i_TrueTrackId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = &SND_gdst_TrackPlayingList[_i_TrueTrackId];
	ERR_X_Assert(pst_Track->i_Ressource < 0);

	pst_Track->pst_PlayerSB = NULL;
	pst_Track->i_Ressource = -1;

	for(i = 0; i < SND_Cte_MaxSimultaneousStream; i++)
	{
		if(!SND_gast_TrackRessources[i].b_Busy)
		{
			SND_gast_TrackRessources[i].b_Busy = TRUE;
			pst_Track->pst_PlayerSB = SND_gast_TrackRessources[i].pst_MonoSB;
			pst_Track->i_Ressource = i;
			return;
		}
	}

	ERR_X_Warning(0, "[SND] No more ressource for streaming", NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_TrackFreeRessources(int _i_TrueTrackId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = &SND_gdst_TrackPlayingList[_i_TrueTrackId];

	if(pst_Track->i_Ressource >= 0) SND_gast_TrackRessources[pst_Track->i_Ressource].b_Busy = FALSE;
	pst_Track->i_Ressource = -1;

	pst_Track->pst_PlayerSB = NULL;
	pst_Track->ui_Flag = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_TrackSwapMonoToStereo(int _i_TrueTrackId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	int				nb, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* check ressource */
	pst_Track = &SND_gdst_TrackPlayingList[_i_TrueTrackId];
	if(pst_Track->i_Ressource < 0) return -1;

	/* check only 2 stereo at the same time */
	for(nb = i = 0; i < SND_Cte_MaxSimultaneousStream; i++)
	{
		if(SND_gdst_TrackPlayingList[i].i_Ressource < 0) continue;
		if(!(SND_gdst_TrackPlayingList[i].ui_Flag & SND_Cte_TrackIsStereo)) continue;
		nb++;
	}

	ERR_X_Warning
	(
		(nb + 1) <= SND_Cte_MaxSimultaneousStereoStream,
		"[SND] Try to play more than 2 stereo stream !",
		NULL
	);
	if((nb + 1) > SND_Cte_MaxSimultaneousStereoStream) return -1;

	pst_Track->pst_PlayerSB = SND_gast_TrackRessources[pst_Track->i_Ressource].pst_StereoSB;
	pst_Track->ui_Flag |= SND_Cte_TrackIsStereo;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_TrackSwapStereoToMono(int _i_TrueTrackId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = &SND_gdst_TrackPlayingList[_i_TrueTrackId];
	if(pst_Track->i_Ressource < 0) return -1;

	pst_Track->pst_PlayerSB = SND_gast_TrackRessources[pst_Track->i_Ressource].pst_MonoSB;
	pst_Track->ui_Flag &= ~SND_Cte_TrackIsStereo;
	return 0;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_Track_Merge
(
	int						_i_TrueTrackId,
	SND_tdst_SoundInstance	*_pst_CurrentPlaying,
	SND_tdst_SoundInstance	*_pst_NextPlaying,
	int						_i_NewFlag,
	int						_i_LoopNb,
	int						_i_StartPos,
	int						_i_StopPos
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					l_Status;
	SND_tdst_OneSound	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    stop current (if any)
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_pst_CurrentPlaying)
	{
		SND_gpst_Interface[ISound].pfi_SndBuffGetStatus
			(
				SND_gdst_TrackPlayingList[_i_TrueTrackId].pst_PlayerSB,
				&l_Status
			);
		if(l_Status & SND_Cul_SBS_Playing)
		{
			SND_gpst_Interface[ISound].pfv_SndBuffStop(SND_gdst_TrackPlayingList[_i_TrueTrackId].pst_PlayerSB);
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    get ressources
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(SND_gdst_TrackPlayingList[_i_TrueTrackId].i_Ressource < 0) SND_TrackAllocRessources(_i_TrueTrackId);

	pst_Sound = SND_gst_Params.dst_Sound + _pst_NextPlaying->l_Sound;

	if
	(
		(pst_Sound->pst_Wave->wChannels == 2)
	&&	(!(SND_gdst_TrackPlayingList[_i_TrueTrackId].ui_Flag & SND_Cte_TrackIsStereo))
	)
	{
		if(SND_i_TrackSwapMonoToStereo(_i_TrueTrackId) < 0)
		{
			return;
		}
	}
	else if
		(
			(pst_Sound->pst_Wave->wChannels == 1)
		&&	(SND_gdst_TrackPlayingList[_i_TrueTrackId].ui_Flag & SND_Cte_TrackIsStereo)
		)
	{
		if(SND_i_TrackSwapStereoToMono(_i_TrueTrackId) < 0)
		{
			return;
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    reinit SB
	 -------------------------------------------------------------------------------------------------------------------
	 */

	SND_StreamFlushOneSound(_pst_NextPlaying->l_Sound, FALSE);

#if defined(_XENON)
	// Set Stream Format (Xenon Only - To Support PCM, ADPCM and XMA)

	xeSND_StreamSetReinitFormat(SND_gdst_TrackPlayingList[_i_TrueTrackId].pst_PlayerSB, SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->wFormatTag);

	// Extension to high->low-level sound engine interface, so voices can display their keys (for debugging purposes)

	xeSND_SndBuffSetKey(SND_gdst_TrackPlayingList[_i_TrueTrackId].pst_PlayerSB, SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].ul_FileKey, SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->cSoundBFIndex);
#endif


/*
#ifdef _XBOX
	if(SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].ul_FileKey != 0x0600021f)
#endif
    */
	

#if 0
	//if((SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].ul_FileKey ) != 0x060002bb) return;
//    if((SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].ul_FileKey ) == 0x060002bb)
	//if(SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->wChannels == 2)
    {
        char log[256];
	    sprintf
	    (
            log,
		    "[EE] reinitplay S%08x %08x, flag %d, loop %d, size %d, pos %d, loopB %d, loopE %d, start %d, stop %d, freq %d, pan %d, vol %d, fxL %d, fxR %d\n",
		    (int) SND_gdst_TrackPlayingList[_i_TrueTrackId].pst_PlayerSB,
		    SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].ul_FileKey,
		    _i_NewFlag,
		    _i_LoopNb,
		    SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->ul_DataSize,
		    SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->ul_DataPosition,
		    SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->ul_LoopBeginOffset,
		    SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->ul_LoopEndOffset,
		    _i_StartPos,
		    _i_StopPos,
		    _pst_NextPlaying->i_BaseFrequency,
		    _pst_NextPlaying->i_Pan,
		    SND_l_GetAttFromVol(SND_f_GetInstVolume(_pst_NextPlaying)),
		    SND_l_GetAttFromVol(_pst_NextPlaying->f_FxVolLeft),
		    SND_l_GetAttFromVol(_pst_NextPlaying->f_FxVolRight)
	    );
        Dbg_LocalDisplay(log);

    }
#endif

	SND_gpst_Interface[ISound].pfv_StreamReinitAndPlay
	(
		SND_gdst_TrackPlayingList[_i_TrueTrackId].pst_PlayerSB,
		_i_NewFlag,
	    SND_gst_Params.dst_Sound[ _pst_NextPlaying->l_Sound ].pst_Wave->wFormatTag,
		_i_LoopNb,
		SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->ul_DataSize,
		SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->ul_DataPosition,
		SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->ul_LoopBeginOffset,
		SND_gst_Params.dst_Sound[_pst_NextPlaying->l_Sound].pst_Wave->ul_LoopEndOffset,
		_i_StartPos,
		_i_StopPos,
		_pst_NextPlaying->i_BaseFrequency,
		_pst_NextPlaying->i_Pan,
		SND_l_GetAttFromVol(SND_f_GetInstVolume(_pst_NextPlaying)),
		SND_l_GetAttFromVol(_pst_NextPlaying->f_FxVolLeft),
		SND_l_GetAttFromVol(_pst_NextPlaying->f_FxVolRight)
	);
    SND_SetPan(SND_M_GetInstanceFromPtr(_pst_NextPlaying), _pst_NextPlaying->i_Pan, _pst_NextPlaying->i_Front);
}

/*$4
 ***********************************************************************************************************************
    Low Interface
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static SND_tdst_Track *SND_p_TrackGetFromSB(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_SB == NULL) return NULL;

	for(i = SND_e_TrackA; i < SND_e_TrackNb; i++)
	{
		if(SND_gdst_TrackPlayingList[i].i_PlayingInstance == -1) continue;

		pst_SI = SND_gst_Params.dst_Instance + SND_gdst_TrackPlayingList[i].i_PlayingInstance;
		if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if(pst_SI->i_InstTrack != i) continue;
		if(pst_SI->pst_DSB != _pst_SB) continue;

		return &SND_gdst_TrackPlayingList[i];
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Track_Stop(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track			*pst_Track;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return;

	SND_gpst_Interface[ISound].pfv_SndBuffStop(pst_Track->pst_PlayerSB);

	SND_M_GetInstanceOrReturn(pst_Track->i_PlayingInstance, pst_SI, ;);
	pst_SI->ul_Flags &= ~(SND_Cul_SF_StreamIsPlayingNow);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_iTrack_Play(SND_tdst_SoundBuffer *_pst_SB, int _i_Flag, int l, int r)
{
	ERR_X_Warning(0, "[SND] Bad \"OnTrack\" : try to call play operation", NULL);
	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Track_Pause(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track			*pst_Track;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return;

	SND_gpst_Interface[ISound].pfv_SndBuffPause(pst_Track->pst_PlayerSB);

	SND_M_GetInstanceOrReturn(pst_Track->i_PlayingInstance, pst_SI, ;);
	pst_SI->ul_Flags &= ~(SND_Cul_SF_StreamIsPlayingNow);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Track_Release(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return;

	SND_gpst_Interface[ISound].pfv_SndBuffStop(pst_Track->pst_PlayerSB);
	SND_TrackDetachInstance(pst_Track->i_PlayingInstance);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Track_SetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int _i_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return;

	SND_gpst_Interface[ISound].pfv_SndBuffSetPos(pst_Track->pst_PlayerSB, _i_Pos);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Track_SetVolume(SND_tdst_SoundBuffer *_pst_SB, int _i_Vol)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return;

	SND_gpst_Interface[ISound].pfv_SndBuffSetVol(pst_Track->pst_PlayerSB, _i_Vol);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Track_SetFrequency(SND_tdst_SoundBuffer *_pst_SB, int _i_Freq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return;

	SND_gpst_Interface[ISound].pfv_SndBuffSetFreq(pst_Track->pst_PlayerSB, _i_Freq);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Track_SetPan(SND_tdst_SoundBuffer *_pst_SB, int _i_Pan, int _i_FrontRear)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return;

	SND_gpst_Interface[ISound].pfv_SndBuffSetPan(pst_Track->pst_PlayerSB, _i_Pan, _i_FrontRear);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_iTrack_GetStatus(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Status)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return 0;

	return SND_gpst_Interface[ISound].pfi_SndBuffGetStatus(pst_Track->pst_PlayerSB, _pi_Status);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Track_GetFrequency(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Freq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return;

	SND_gpst_Interface[ISound].pfv_SndBuffGetFreq(pst_Track->pst_PlayerSB, _pi_Freq);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Track_GetPan(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pan, int *pi)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return;

	SND_gpst_Interface[ISound].pfv_SndBuffGetPan(pst_Track->pst_PlayerSB, _pi_Pan, pi);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Track_GetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pos, int *_pi_Write)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Track = SND_p_TrackGetFromSB(_pst_SB);
	if(pst_Track == NULL) return;

	SND_gpst_Interface[ISound].pfv_SndBuffGetPos(pst_Track->pst_PlayerSB, _pi_Pos, _pi_Write);
}

/*$4
 ***********************************************************************************************************************
    INTERFACE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_TrackSeti(int i_TrueTrack, int _i_RqId, int _i_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track			*pst_Track;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;


	switch(_i_RqId)
	{
	case SND_Cte_TrackRq_ChainInstance:
		SND_M_CheckTrackIdOrReturn(i_TrueTrack, -1);
		pst_Track = SND_gdst_TrackPlayingList + i_TrueTrack;
        SND_M_GetInstanceOrReturn(_i_Value, pst_SI, -1);
		pst_Track->i_ChainInstance = _i_Value;
		break;

	case SND_Cte_TrackRq_PauseAll:SND_TrackPauseAll(TRUE);break;
	case SND_Cte_TrackRq_UnpauseAll:SND_TrackPauseAll(FALSE);break;

	default:
		SND_EdiRedWarningMsg("SND_i_TrackSeti", "Bad Rq ID");
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_TrackGeti(int i_TrueTrack, int _i_RqId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Track	*pst_Track;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;

	SND_M_CheckTrackIdOrReturn(i_TrueTrack, -1);
	pst_Track = SND_gdst_TrackPlayingList + i_TrueTrack;

	switch(_i_RqId)
	{
	case SND_Cte_TrackRq_ChainDelay:	return lFloatToLong(1000.0f * pst_Track->f_ChainInstDelay);
	case SND_Cte_TrackRq_ChainInstance: return pst_Track->i_ChainInstance;
	case SND_Cte_TrackRq_Instance:		return pst_Track->i_PlayingInstance;
	default:							SND_EdiRedWarningMsg("SND_i_TrackGeti", "Bad Rq ID"); return -1;
	}
}

BOOL			SND_gb_PauseAll = FALSE;
volatile BOOL	SND_gb_PausingAll = FALSE;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_TrackPauseAll(BOOL b_Stop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i, i_BaseFrequency;
	LONG					l_status;
	SND_tdst_SoundInstance	*pst_SI;
	float					_f_Factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	
	if(b_Stop)
	{
		SND_gb_PauseAll = TRUE;
		_f_Factor = 0.0f;
	}
	else
	{
		SND_gb_PauseAll = FALSE;
		_f_Factor = 1.0f;
	}

	
	SND_gb_PausingAll = TRUE;
	for(i = SND_e_TrackA; i < SND_e_TrackNb; i++)
	{
		if((int) SND_gdst_TrackPlayingList[i].i_PlayingInstance == -1) continue;
		pst_SI = SND_gst_Params.dst_Instance + SND_gdst_TrackPlayingList[i].i_PlayingInstance;
		if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;

		/* this is a stream ? */
		l_status = SND_Cul_SF_Dialog | SND_Cul_SF_Ambience | SND_Cul_SF_Music;
		if((pst_SI->ul_Flags & l_status) == 0) continue;

		/* sure this a stream ? */
		l_status = SND_Cul_DSF_Used | SND_Cte_StreamedFile;
		if(((LONG) SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_Flags & l_status) != l_status) continue;
#ifdef JADEFUSION
		SND_gpst_Interface[ISound].pfi_SndBuffGetStatus((SND_tdst_SoundBuffer_*)SND_gdst_TrackPlayingList[i].pst_PlayerSB, (int*)&l_status);
#else
		SND_gpst_Interface[ISound].pfi_SndBuffGetStatus(SND_gdst_TrackPlayingList[i].pst_PlayerSB, &l_status);
#endif
		/* is it playing ? */
		if(l_status & SND_Cul_SBS_Playing)
		{
			i_BaseFrequency = lFloatToLong(fLongToFloat(pst_SI->i_BaseFrequency) * _f_Factor);
			SND_gpst_Interface[ISound].pfv_SndBuffSetFreq(SND_gdst_TrackPlayingList[i].pst_PlayerSB, i_BaseFrequency);
		}
	}
	SND_gb_PausingAll = FALSE;
}

/*$4
 ***********************************************************************************************************************
    CHAINING
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_Track_ChainPlayer
(
	int				iOldInstance,
	int				iNewInstance,
	unsigned int	uiMode,
	unsigned int	uiStartRegionTag,
	int				iLoop
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			uiPos, uiExit;
	SND_tdst_SoundInstance	*pst_SIold, *pst_SInew, *pst_SIchain;
	SND_tdst_WaveDesc		*pOldWave, *pNewWave;
	float					fLatency;
	int						iMarker;
	SND_tdst_Track			*pst_Track;
	int						iFlag, loop;
	unsigned int			uiBeginLoopSample, uiEndLoopSample, uiTotalSample;
	unsigned int			uiStart, uiLoop = 0;
	unsigned int			uiTemp, uiStartEnd, uiSampleNb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    //iLoop = -1;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    check tag + mode
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	switch(uiMode)
	{
	case SND_Cte_TrackRq_ModeNoWait:
	case SND_Cte_TrackRq_ModeWaitRegion:
	case SND_Cte_TrackRq_ModeWaitMarker:
		break;

	default:
		ERR_X_Assert(0);
		return 0.0f;
	}

	switch(uiStartRegionTag)
	{
	case SND_Cte_TrackRq_TagStart:
	case SND_Cte_TrackRq_TagLoop:
	case SND_Cte_TrackRq_TagEnd:
		break;

	default:
		ERR_X_Assert(0);
		return 0.0f;
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    get new instance + new wave
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	SND_M_GetInstanceOrReturn(iNewInstance, pst_SInew, 0.0f);
	pNewWave = SND_gst_Params.dst_Sound[pst_SInew->l_Sound].pst_Wave;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    get track
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	SND_M_CheckTrackIdOrReturn(pst_SInew->i_InstTrack, 0.0f);
	pst_Track = SND_gdst_TrackPlayingList + pst_SInew->i_InstTrack;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    get old instance
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	iOldInstance = pst_Track->i_PlayingInstance;
	if(iOldInstance == -1)
	{
		/* force mode => no wait */
		uiMode = SND_Cte_TrackRq_ModeNoWait;
	}
	else
	{
		SND_M_GetInstanceOrReturn(iOldInstance, pst_SIold, 0.0f);
		pOldWave = SND_gst_Params.dst_Sound[pst_SIold->l_Sound].pst_Wave;

		uiBeginLoopSample = SND_ui_SizeToSample
			(
				pOldWave->wFormatTag,
				pOldWave->wChannels,
				pOldWave->ul_LoopBeginOffset
			);
		uiEndLoopSample = SND_ui_SizeToSample(pOldWave->wFormatTag, pOldWave->wChannels, pOldWave->ul_LoopEndOffset);
		uiTotalSample = SND_ui_SizeToSample(pOldWave->wFormatTag, pOldWave->wChannels, pOldWave->ul_DataSize);

		ERR_X_Assert(pst_SIold->i_InstTrack == pst_SInew->i_InstTrack);
		ERR_X_Assert(pst_SIold->i_StartFrequency == pst_SInew->i_StartFrequency);
		ERR_X_Assert(pst_SIold->i_Pan == pst_SInew->i_Pan);
		ERR_X_Assert(pst_SIold->i_InstTrack == pst_SInew->i_InstTrack);

		SND_gpst_Interface[ISound].pfv_SndBuffGetPos(pst_Track->pst_PlayerSB, (int *) &uiPos, (int *) &uiExit);
		uiPos = uiExit;

		SND_gpst_Interface[ISound].pfv_StreamLoopCountGet(pst_Track->pst_PlayerSB, &loop);
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    action
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	switch(uiMode)
	{
	case SND_Cte_TrackRq_ModeNoWait:
		SND_Stop(iOldInstance);
		switch(uiStartRegionTag)
		{
		case SND_Cte_TrackRq_TagStart:
			SND_PlaySoundLooping(iNewInstance, iLoop);
			break;

		case SND_Cte_TrackRq_TagLoop:
            SND_PlaySoundLoopingFrom(iNewInstance, pNewWave->b_HasLoop ? -1 : 0, pNewWave->ul_LoopBeginOffset);
			break;

		case SND_Cte_TrackRq_TagEnd:
			SND_PlaySoundLoopingFrom(iNewInstance, 0, pNewWave->ul_LoopEndOffset);
			break;

		default:
			break;
		}

		return 0.0f;

	case SND_Cte_TrackRq_ModeWaitRegion:
		uiExit += pst_SIold->i_BaseFrequency;		/* security */
		if(uiExit < uiBeginLoopSample)
			uiExit = uiBeginLoopSample;
		else
			uiExit = uiEndLoopSample;
		break;

	case SND_Cte_TrackRq_ModeWaitMarker:
		uiExit += pst_SIold->i_BaseFrequency / 2;	/* security */
		if(pOldWave->ul_ExitPointNb > 1)
		{
			uiLoop = 0;
			while((uiExit > pOldWave->aul_ExitPoint[pOldWave->ul_ExitPointNb - 1]) && loop)
			{
				uiExit -= pOldWave->aul_ExitPoint[pOldWave->ul_ExitPointNb - 1] - pOldWave->aul_ExitPoint[0];
				uiLoop += pOldWave->aul_ExitPoint[pOldWave->ul_ExitPointNb - 1] - pOldWave->aul_ExitPoint[0];
				if(loop > 0) loop--;
			}

			for(iMarker = 0; iMarker < (int) pOldWave->ul_ExitPointNb; iMarker++)
			{
				if(uiExit < pOldWave->aul_ExitPoint[iMarker]) break;
			}

			if(iMarker == (int) pOldWave->ul_ExitPointNb)
			{
				uiExit = uiTotalSample + 20;		/* there is no enougth loops, we will stanby the end of the sound */
			}
			else
			{
				uiExit = pOldWave->aul_ExitPoint[iMarker];
			}
		}
		else
		{
			uiExit = uiExit / 448;
			uiExit = uiExit * 448;
			if(uiExit > SND_ui_SizeToSample(pOldWave->wFormatTag, pOldWave->wChannels, pOldWave->ul_LoopEndOffset))
			{
				uiExit = SND_ui_SizeToSample(pOldWave->wFormatTag, pOldWave->wChannels, pOldWave->ul_LoopEndOffset);
			}
		}
		break;

	default:
		ERR_X_Assert(0);
		return 0.0f;
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    eval the latency
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	uiTemp = SND_ui_SizeToSample
		(
			pOldWave->wFormatTag,
			pOldWave->wChannels,
			pOldWave->ul_LoopEndOffset - pOldWave->ul_LoopBeginOffset
		);
	uiStartEnd = SND_ui_SizeToSample(pOldWave->wFormatTag, pOldWave->wChannels, pOldWave->ul_LoopEndOffset);
	uiSampleNb = SND_ui_SizeToSample(pOldWave->wFormatTag, pOldWave->wChannels, pOldWave->ul_DataSize);

	if((uiLoop + uiExit) >= uiPos)
	{
		uiPos = uiExit + uiLoop - uiPos;
	}
	else
	{
		uiExit = uiTotalSample + 20;
		if(uiStartRegionTag == SND_Cte_TrackRq_TagEnd) return 0.0f;

		uiPos = uiExit - uiPos;
		fLatency = fLongToFloat(uiPos) / fLongToFloat(pst_SIold->i_BaseFrequency);
		pst_SInew->f_Delay = fLatency;

		if(iLoop) pst_SInew->ul_Flags |= SND_Cul_OSF_Loop;
		pst_SInew->i_LoopNb = iLoop;
		return pst_SInew->f_Delay;
	}

	fLatency = fLongToFloat(uiPos) / fLongToFloat(pst_SIold->i_BaseFrequency);
	pst_SInew->f_Delay = -fLatency;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    convert sample to size
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	uiExit = SND_ui_SampleToSize(pOldWave->wFormatTag, pOldWave->wChannels, uiExit);

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    play the chaining transition wave
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(pst_Track->i_ChainInstance != -1)
	{
		pst_SIchain = SND_gst_Params.dst_Instance + pst_Track->i_ChainInstance;
		pst_SIchain->f_Delay = fLatency - pst_Track->f_ChainInstDelay;
		if(pst_SIchain->f_Delay < 0.0f) pst_SIchain->f_Delay = 0.0f;
		SND_PlaySound(pst_Track->i_ChainInstance);
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    eval the start pos
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	switch(uiStartRegionTag)
	{
	case SND_Cte_TrackRq_TagStart:	uiStart = pNewWave->ul_StartOffset; break;
	case SND_Cte_TrackRq_TagLoop:	uiStart = pNewWave->ul_LoopBeginOffset; break;
	case SND_Cte_TrackRq_TagEnd:	uiStart = pNewWave->ul_LoopEndOffset; iLoop = 0; break;
	default:						uiStart = pNewWave->ul_StartOffset; break;
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    update the last instance
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	SND_i_SModifierNotifyStopInstance(pst_SIold);
	pst_SIold->ul_Flags &= ~(SND_Cul_SF_PlayingLoop | SND_Cul_SF_Playing | SND_Cul_SF_StreamIsPlayingNow | SND_Cul_ESF_StopRq);
	pst_SIold->ul_ExtFlags &= ~(SND_Cul_ESF_StopRq);
	pst_SIold->f_Delay = 0.0f;
	pst_SIold->i_LoopNb = 0;
	pst_SIold->pst_LI = &SND_gpst_Interface[IStream];

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	pst_Track->i_PlayingInstance = iNewInstance;
	pst_Track->ul_PrevPosition = -1;
	pst_SInew->pst_LI = &SND_gpst_Interface[ITrack];
	pst_SInew->ul_Flags &= ~(SND_Cul_SF_StreamIsPlayingNow | SND_Cul_SF_Playing | SND_Cul_SF_PlayingLoop );
	pst_SInew->ul_ExtFlags &= ~(SND_Cul_ESF_StopRq);
	if(iLoop)
		pst_SInew->ul_Flags |= SND_Cul_SF_PlayingLoop;
	else
		pst_SInew->ul_Flags |= SND_Cul_SF_Playing;
	
	SND_i_SModifierNotifyPlayInstance(pst_SInew);

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    eval the mode
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	iFlag = iLoop ? SND_Cul_SBC_PlayLooping : 0;
	if(pst_SInew->ul_Flags & SND_Cul_SF_FxA) iFlag |= SND_Cul_SBC_PlayFxA;
	if(pst_SInew->ul_Flags & SND_Cul_SF_FxB) iFlag |= SND_Cul_SBC_PlayFxB;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    chain
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */
#if defined(_XENON)
	// Set Stream Format (Xenon Only - To Support PCM, ADPCM and XMA)

	xeSND_StreamSetReinitFormat(pst_Track->pst_PlayerSB, pNewWave->wFormatTag);

	// Extension to high->low-level sound engine interface, so voices can display their keys (for debugging purposes)

	xeSND_SndBuffSetKey(pst_Track->pst_PlayerSB, pNewWave->ul_FileKey, pNewWave->cSoundBFIndex);
#endif

	SND_gpst_Interface[ISound].pfv_StreamChain
		(
			pst_Track->pst_PlayerSB,
			iFlag,
	        pNewWave->wFormatTag,
			iLoop,
			uiExit,
			uiStart,
			pNewWave->ul_DataSize,
			pNewWave->ul_DataPosition,
			pNewWave->ul_LoopBeginOffset,
			pNewWave->ul_LoopEndOffset
		);

#if 0
//    if((SND_gst_Params.dst_Sound[pst_SInew->l_Sound].ul_FileKey & 0xFF000000) == 0x06000000)
    {
        char log[256];
	    sprintf
	    (
            log,
		    "[EE] chain S%08x %08x flag %x loop %d exit %d start %d size %d loopB %d loopE %d time %.2f\n",
		    (int) pst_Track->pst_PlayerSB,
		    SND_gst_Params.dst_Sound[pst_SInew->l_Sound].ul_FileKey,
		    iFlag,
		    iLoop,
		    uiExit,
		    uiStart,
		    pNewWave->ul_DataSize,
		    pNewWave->ul_LoopBeginOffset,
		    pNewWave->ul_LoopEndOffset,
		    fLatency
	    );
        Dbg_LocalDisplay(log);
    }
#endif

	return -fLatency;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_DetectMarker(SND_tdst_WaveDesc *pWave, ULONG ulPos1, ULONG ulPos2)
{
	/*~~~~~~*/
	ULONG	j;
	/*~~~~~~*/

	for(j = 0; j < pWave->ul_SignalPointNb; j++)
	{
		if(pWave->dst_SignalTable[j].ul_Position < ulPos1) continue;
		if(pWave->dst_SignalTable[j].ul_Position > ulPos2) continue;

		if(SND_gst_CurrentSignal.ul_Size == 0)
		{
			SND_gst_CurrentSignal.ul_Size = 10;
			SND_gst_CurrentSignal.dul_Table = (ULONG *)MEM_p_Alloc(SND_gst_CurrentSignal.ul_Size * sizeof(ULONG));
		}
		else if(SND_gst_CurrentSignal.ul_Element >= SND_gst_CurrentSignal.ul_Size)
		{
			SND_gst_CurrentSignal.ul_Size += 10;
			SND_gst_CurrentSignal.dul_Table = (ULONG *)MEM_p_Realloc
				(
					SND_gst_CurrentSignal.dul_Table,
					SND_gst_CurrentSignal.ul_Size * sizeof(ULONG)
				);
		}

		SND_gst_CurrentSignal.dul_Table[SND_gst_CurrentSignal.ul_Element] = pWave->dst_SignalTable[j].ul_Label;
		SND_gst_CurrentSignal.ul_Element++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_DetectRegion(SND_tdst_WaveDesc *pWave, ULONG ulPos1, ULONG ulPos2)
{
	/*~~~~~~*/
	ULONG	j;
	/*~~~~~~*/

	for(j = 0; j < pWave->ul_RegionNb; j++)
	{
		if(ulPos2 < pWave->dst_RegionTable[j].ul_StartPosition) continue;
		if(pWave->dst_RegionTable[j].ul_StopPosition < ulPos1) continue;

		if(SND_gst_CurrentRegion.ul_Size == 0)
		{
			SND_gst_CurrentRegion.ul_Size = 10;
			SND_gst_CurrentRegion.dul_Table = (ULONG *)MEM_p_Alloc(SND_gst_CurrentRegion.ul_Size * sizeof(ULONG));
		}
		else if(SND_gst_CurrentRegion.ul_Element >= SND_gst_CurrentRegion.ul_Size)
		{
			SND_gst_CurrentRegion.ul_Size += 10;
			SND_gst_CurrentRegion.dul_Table = (ULONG *)MEM_p_Realloc
				(
					SND_gst_CurrentRegion.dul_Table,
					SND_gst_CurrentRegion.ul_Size * sizeof(ULONG)
				);
		}

		SND_gst_CurrentRegion.dul_Table[SND_gst_CurrentRegion.ul_Element] = pWave->dst_RegionTable[j].ul_Label;
		SND_gst_CurrentRegion.ul_Element++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_TrackUpdate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	SND_tdst_Track			*pTrack;
	SND_tdst_SoundInstance	*pst_SI;
	SND_tdst_OneSound		*pst_Snd;
	SND_tdst_WaveDesc		*pWave;
	unsigned int			uiPos, uiWrite, uiLoop, uiLoopSize;
	static int				iFrame = 0;
    int                     iFrameMask = 0x4;
	ULONG					ul_LoopBeginOffset, ul_LoopEndOffset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(SND_gst_CurrentSignal.dul_Table)
		L_memset(SND_gst_CurrentSignal.dul_Table, 0, SND_gst_CurrentSignal.ul_Size * sizeof(ULONG));
	SND_gst_CurrentSignal.ul_Element = 0;

    iFrame++;

    if(iFrame & iFrameMask)  
    {
		if(SND_gst_CurrentRegion.dul_Table)
			L_memset(SND_gst_CurrentRegion.dul_Table, 0, SND_gst_CurrentRegion.ul_Size * sizeof(ULONG));
		SND_gst_CurrentRegion.ul_Element = 0;
    }


    for(i = SND_e_TrackA; i < SND_e_TrackNb; i++)
	{

		/*$1- skip free and stopped track ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pTrack = SND_gdst_TrackPlayingList + i;
		if(pTrack->i_PlayingInstance < 0) continue;

		/*$1- update the chain delay ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		SND_gpst_Interface[ISound].pfv_StreamChainDelayGet(pTrack->pst_PlayerSB, &pTrack->f_ChainInstDelay);

		/*$1- update the position, and signal state ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if((iFrame & iFrameMask) == 0) continue;



		pst_SI = SND_gst_Params.dst_Instance + pTrack->i_PlayingInstance;
		if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if(pst_SI->i_InstTrack != i) continue;

		pst_Snd = SND_gst_Params.dst_Sound + pst_SI->l_Sound;
		if((pst_Snd->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if(pst_Snd->pst_Wave == NULL) continue;

		pWave = pst_Snd->pst_Wave;
		if(!pWave->dst_SignalTable && !pWave->dst_RegionTable) continue;

		SND_gpst_Interface[ISound].pfv_SndBuffGetPos(pTrack->pst_PlayerSB, (int *) &uiPos, (int *) &uiWrite);

		uiLoop = SND_ui_SizeToSample(pWave->wFormatTag, pWave->wChannels, pWave->ul_LoopEndOffset);
        
        uiLoopSize = SND_ui_SizeToSample(pWave->wFormatTag, pWave->wChannels, pWave->ul_LoopBeginOffset);
        uiLoopSize = uiLoop - uiLoopSize;

		while(uiPos >= uiLoop) uiPos -= uiLoopSize;

		if(pTrack->ul_PrevPosition != -1)
		{
			if((pTrack->ul_PrevPosition) < uiPos)
			{
				SND_DetectMarker(pWave, pTrack->ul_PrevPosition, uiPos);
				SND_DetectRegion(pWave, pTrack->ul_PrevPosition, uiPos);
                pTrack->ul_PrevPosition = uiPos;
			}
            else if(pTrack->ul_PrevPosition > uiPos)
			{
				/* we are looping */
				ul_LoopBeginOffset = SND_ui_SizeToSample(pWave->wFormatTag, pWave->wChannels, pWave->ul_LoopBeginOffset);
				ul_LoopEndOffset = SND_ui_SizeToSample(pWave->wFormatTag, pWave->wChannels, pWave->ul_LoopEndOffset);

                //ERR_X_Assert(pTrack->ul_PrevPosition <= ul_LoopEndOffset);
                //ERR_X_Assert(ul_LoopBeginOffset <= uiPos);

				SND_DetectMarker(pWave, pTrack->ul_PrevPosition, ul_LoopEndOffset);
				SND_DetectMarker(pWave, ul_LoopBeginOffset, uiPos);
				SND_DetectRegion(pWave, pTrack->ul_PrevPosition, ul_LoopEndOffset);
				SND_DetectRegion(pWave, ul_LoopBeginOffset, uiPos);
                pTrack->ul_PrevPosition = uiPos;
			}
		}
        else
        {
		    pTrack->ul_PrevPosition = uiPos;
        }
	}

	if(iFrame & iFrameMask)  iFrame=0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_SignalIsActive(char *_pz_Label)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	i;
	ULONG	ul_Label;
	ULONG	*pul_Label;
	/*~~~~~~~~~~~~~~~*/
	((char *) &ul_Label)[0] = _pz_Label[0];
	((char *) &ul_Label)[1] = _pz_Label[1];
	((char *) &ul_Label)[2] = _pz_Label[2];
	((char *) &ul_Label)[3] = _pz_Label[3];

    for(i = 0, pul_Label = SND_gst_CurrentSignal.dul_Table; i < SND_gst_CurrentSignal.ul_Element; i++, pul_Label++)
	{
		if(*pul_Label == ul_Label) return 1;
	}

	for(i = 0, pul_Label = SND_gst_CurrentRegion.dul_Table; i < SND_gst_CurrentRegion.ul_Element; i++, pul_Label++)
	{
		if(*pul_Label == ul_Label) return 1;
	}

	return 0;
}

void SND_Reinit(void)
{
    if(SND_gst_CurrentRegion.dul_Table) MEM_Free(SND_gst_CurrentRegion.dul_Table);
    SND_gst_CurrentRegion.dul_Table = NULL;
    SND_gst_CurrentRegion.ul_Element = 0;
    SND_gst_CurrentRegion.ul_Size = 0;
    //
    if(SND_gst_CurrentSignal.dul_Table) MEM_Free(SND_gst_CurrentSignal.dul_Table);
    SND_gst_CurrentSignal.dul_Table = NULL;
    SND_gst_CurrentSignal.ul_Element = 0;
    SND_gst_CurrentSignal.ul_Size = 0;

    SND_StreamPrefetchFlushAll();
	SND_ReinitGroupRequest();
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
