/*$T SNDloadingsound.c GC 1.138 12/16/04 12:08:23 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

/*$4
 ***********************************************************************************************************************
    Headers
 ***********************************************************************************************************************
 */

#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDmacros.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/Sources/SNDspecific.h"

#include "EDItors/Sources/SOuNd/SONutil.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SOUNDLOADING.h"
#include "TIMer/TIMdefs.h"
#include "BASe/MEMory/MEM.h"

#ifdef ACTIVE_EDITORS
#include "SouND/Sources/SNDconv_xboxadpcm.h"
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define SND_Cte_WacMdFRefMaxNb	10

/*$4
 ***********************************************************************************************************************
    type
 ***********************************************************************************************************************
 */

typedef enum	SND_tden_WacManagerState_ 
{ 
	Wac_e_MapIsRunning, 
	Wac_e_StartLoadingMap, 
	Wac_e_EndLoadingMap, 
	Wac_e_Force32bits=0xFFFFFFFF 
} SND_tden_WacManagerState;

typedef struct SND_tdst_WacManager_
{
	MDF_tdst_Modifier			*dst_WacReference[SND_Cte_WacMdFRefMaxNb];
	int							i_RefNumber;
	int							i_CurrentSoundIndex;
	int							i_CurrentInstanceIndex;
	int							i_CurrentModifierIndex;
	SND_tden_WacManagerState	en_State;
	SND_tdst_SoundBuffer		*pst_SB;
	/**/
	float						f_FadeInTime;
	float						f_FadeOutTime;
	float						f_FadeFactor;
	float						f_VolumeFactor;
	/**/
	BOOL						b_NewLoading;
	BOOL						b_NoWacIntoTheMap;
} SND_tdst_WacManager;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

static void			SND_CleanOldWac(void);
static void			SND_DeleteWacSound(void);
/**/
SND_tdst_WacManager SND_gst_WacManager;

/*$4
 ***********************************************************************************************************************
    module
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_LoadingSoundInitModule(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_WaveData	st_WaveFile;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&SND_gst_WacManager, 0, sizeof(SND_tdst_WacManager));
	SND_gst_WacManager.i_CurrentSoundIndex = -1;
	SND_gst_WacManager.i_CurrentModifierIndex = -1;
	SND_gst_WacManager.i_CurrentInstanceIndex = -1;
	SND_gst_WacManager.en_State = Wac_e_MapIsRunning;
	SND_gst_WacManager.f_FadeInTime = 1.5f;		/* sec */
	SND_gst_WacManager.f_FadeOutTime = 0.5f;	/* sec */
	SND_gst_WacManager.f_FadeFactor = 0.0f;
	SND_gst_WacManager.f_VolumeFactor = 1.0f;

	/*$2- reserve SB -------------------------------------------------------------------------------------------------*/

	st_WaveFile.st_WaveFmtx.wFormatTag = SND_Cte_DefaultWaveFormat;
	st_WaveFile.st_WaveFmtx.nChannels = 1;			/* mono */
	st_WaveFile.st_WaveFmtx.nSamplesPerSec = 48000; /* 48 kHz */
	st_WaveFile.st_WaveFmtx.wBitsPerSample = 16;	/* 16 bits */
	st_WaveFile.st_WaveFmtx.nBlockAlign = st_WaveFile.st_WaveFmtx.nChannels * (st_WaveFile.st_WaveFmtx.wBitsPerSample / 8);
	st_WaveFile.st_WaveFmtx.nAvgBytesPerSec = st_WaveFile.st_WaveFmtx.nSamplesPerSec * st_WaveFile.st_WaveFmtx.nBlockAlign;
	st_WaveFile.st_WaveFmtx.cbSize = 0;
	st_WaveFile.i_Size = SND_Cte_LoadingSoundMaxSize;

	st_WaveFile.pbData = 0;

	SND_M_RasterRegisterFile(SND_Cul_SF_LoadingSound);
	SND_gst_WacManager.pst_SB = SND_gpst_Interface[ISound].pfp_SndBuffCreate
		(
			SND_gst_Params.pst_SpecificD,
			&st_WaveFile,
			0
		);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_LoadingSoundCloseModule(void)
{
	/*~~~~~~*/
	LONG	l;
	/*~~~~~~*/

	if(SND_gst_WacManager.i_CurrentInstanceIndex != -1)
	{
		l = SND_gst_WacManager.i_CurrentInstanceIndex;
		SND_gst_WacManager.i_CurrentInstanceIndex = -1;
		SND_Release(l);
	}

	if(SND_gst_WacManager.i_CurrentSoundIndex != -1)
	{
		l = SND_gst_WacManager.i_CurrentSoundIndex;
		SND_gst_WacManager.i_CurrentSoundIndex = -1;
		SND_DeleteByIndex(SND_gst_WacManager.i_CurrentSoundIndex);
	}
	else
		SND_gpst_Interface[ISound].pfv_SndBuffRelease(SND_gst_WacManager.pst_SB);

	SND_gst_WacManager.pst_SB = NULL;
}

/*$4
 ***********************************************************************************************************************
    loading
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_AddLoadingSound(BIG_KEY _ul_FileKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				l_file;
	SND_tdst_OneSound	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_file = SND_l_AddSound(_ul_FileKey);
	if(l_file != -1)
	{
		pst_Sound = SND_gst_Params.dst_Sound + l_file;
		pst_Sound->ul_Flags |= SND_Cul_SF_LoadingSound;
	}

	return l_file;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_LoadWacData(LONG l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_OneSound	*pst_Sound;
	SND_tdst_WaveData	st_WaveFile;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;
	if(l_Index == -1) return -1;
	if(SND_gst_WacManager.i_CurrentSoundIndex == l_Index) return l_Index;

	if(SND_gst_WacManager.i_CurrentSoundIndex != -1)
	{
		pst_Sound = SND_gst_Params.dst_Sound + SND_gst_WacManager.i_CurrentSoundIndex;
		pst_Sound->ul_Flags &= ~SND_Cul_OSF_Loaded;
	}

    SND_gst_WacManager.i_CurrentSoundIndex = l_Index;
	pst_Sound = SND_gst_Params.dst_Sound + l_Index;
	if(pst_Sound->ul_Flags & SND_Cul_OSF_Loaded) return l_Index;

	/*$2- load the sound buffer --------------------------------------------------------------------------------------*/

	ESON_M_ReturnIfFileIsTruncated(pst_Sound->ul_FileKey, -1);
	if(!SND_M_IsGoodFormat(pst_Sound->pst_Wave->wFormatTag)) return -1;

	/*$2- get SB -----------------------------------------------------------------------------------------------------*/

	st_WaveFile.st_WaveFmtx.wFormatTag = pst_Sound->pst_Wave->wFormatTag;
	st_WaveFile.st_WaveFmtx.nChannels = pst_Sound->pst_Wave->wChannels;
	st_WaveFile.st_WaveFmtx.nSamplesPerSec = pst_Sound->pst_Wave->dwSamplesPerSec;
	st_WaveFile.st_WaveFmtx.nAvgBytesPerSec = pst_Sound->pst_Wave->dwAvgBytesPerSec;
	st_WaveFile.st_WaveFmtx.nBlockAlign = pst_Sound->pst_Wave->wBlockAlign;
	st_WaveFile.st_WaveFmtx.wBitsPerSample = pst_Sound->pst_Wave->wBitsPerSample;
	st_WaveFile.st_WaveFmtx.cbSize = pst_Sound->pst_Wave->cbSize;
	st_WaveFile.i_Size = pst_Sound->pst_Wave->ul_DataSize;
#ifdef JADEFUSION
	st_WaveFile.pbData = (BYTE *) pst_Sound->pst_Wave->ul_DataPosition;
#else
	st_WaveFile.pbData = (void *) pst_Sound->pst_Wave->ul_DataPosition;
#endif
	SND_M_RasterRegisterFile(SND_Cul_SF_LoadingSound);

	pst_Sound->pst_DSB = SND_gpst_Interface[ISound].pfp_SndBuffReLoad
		(
			SND_gst_Params.pst_SpecificD,
			SND_gst_WacManager.pst_SB,
			&st_WaveFile,
			pst_Sound->pst_Wave->ul_DataPosition
		);
	SND_gst_WacManager.pst_SB = pst_Sound->pst_DSB;

	if(!pst_Sound->pst_DSB) return -1;

#if defined(_XENON)
	// Extension to high->low-level sound engine interface, so voices can display their keys (for debugging purposes)

	xeSND_SndBuffSetKey(pst_Sound->pst_DSB, pst_Sound->ul_FileKey, pst_Sound->pst_Wave->cSoundBFIndex);
#endif

	/*$2- register the new SB ----------------------------------------------------------------------------------------*/

	pst_Sound->ul_Flags = SND_Cul_SF_LoadingSound | SND_Cul_DSF_Used | SND_Cul_OSF_Loaded | SND_Cul_SF_HeaderLoaded;

	return l_Index;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_WacRegisterMdF(MDF_tdst_Modifier *pMdF)
{
	if(SND_gst_WacManager.i_RefNumber < SND_Cte_WacMdFRefMaxNb)
	{
		SND_gst_WacManager.dst_WacReference[SND_gst_WacManager.i_RefNumber] = pMdF;
		SND_gst_WacManager.i_RefNumber++;
	}
	else
	{
		ERR_X_Warning(0, "[SND] overflow the max number of WAC MDF", NULL);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_WacUnregisterMdF(MDF_tdst_Modifier *pMdF)
{
	/*~~~~~*/
	int i, j;
	/*~~~~~*/

	for(i = 0; i < SND_gst_WacManager.i_RefNumber; i++)
	{
		if(SND_gst_WacManager.dst_WacReference[i] != pMdF) continue;

		SND_gst_WacManager.i_RefNumber--;

		for(j = i; j < SND_gst_WacManager.i_RefNumber; j++)
			SND_gst_WacManager.dst_WacReference[j] = SND_gst_WacManager.dst_WacReference[j + 1];

		break;
	}

	SND_gst_WacManager.dst_WacReference[SND_gst_WacManager.i_RefNumber] = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_WacUpdate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i, iBest;
	MDF_tdst_LoadingSound	*pWacMdF;
	OBJ_tdst_GameObject		*pGao;
	float					fDistMin, fDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_CleanOldWac();

	if(SND_gst_WacManager.en_State == Wac_e_MapIsRunning)
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    the map is running
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		/*$2----------------------------------------------------------------------------------------------------------*/

		if(SND_gst_WacManager.b_NoWacIntoTheMap)
		{
			SND_ReinitWacInstance();
			SND_DeleteWacSound();
		}

		/*$2- get the best mdf ---------------------------------------------------------------------------------------*/

		fDistMin = -1.0f;
		iBest = -1;
		for(i = 0; i < SND_gst_WacManager.i_RefNumber; i++)
		{
			pWacMdF = (MDF_tdst_LoadingSound *) SND_gst_WacManager.dst_WacReference[i]->p_Data;
			pGao = SND_gst_WacManager.dst_WacReference[i]->pst_GO;

			fDist = MATH_f_Distance
				(
					MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol),
					MATH_pst_GetTranslation(pGao->pst_GlobalMatrix)
				);

			/* skip if outside the loading volume */
			if(fDist > pWacMdF->f_LoadingDistance) continue;

			/* skip if another one is closer */
			if(fDistMin >= fDist) continue;

			/* register the best */
			fDistMin = fDist;
			iBest = i;
		}

		/*$2- get the wac instance -----------------------------------------------------------------------------------*/

		if((iBest != -1) && (iBest != SND_gst_WacManager.i_CurrentModifierIndex))
		{
			if(SND_gst_WacManager.i_CurrentInstanceIndex != -1)
			{
				/* stop any previous instance */
				SND_Stop(SND_gst_WacManager.i_CurrentInstanceIndex);
				SND_Release(SND_gst_WacManager.i_CurrentInstanceIndex);
				SND_gst_WacManager.i_CurrentInstanceIndex = -1;
			}

			pWacMdF = (MDF_tdst_LoadingSound *) SND_gst_WacManager.dst_WacReference[iBest]->p_Data;
			SND_gst_WacManager.i_CurrentModifierIndex = iBest;
			
            SND_gst_WacManager.i_CurrentInstanceIndex = SND_l_Request(NULL, pWacMdF->i_SndIdx);
			SND_SetObject
			(
				SND_gst_WacManager.dst_WacReference[iBest]->pst_GO,
				SND_gst_WacManager.i_CurrentInstanceIndex
			);            
		}

		/*$2- auto play the wac instance -----------------------------------------------------------------------------*/

		if(SND_gst_WacManager.i_CurrentInstanceIndex != -1)
		{
			if
			(
				(!SND_i_IsPlaying(SND_gst_WacManager.i_CurrentInstanceIndex))
			&&	SND_b_IsInActiveVolume(SND_gst_WacManager.i_CurrentInstanceIndex)
			) SND_PlaySoundLooping(SND_gst_WacManager.i_CurrentInstanceIndex, -1);
		}
	}
	else if(SND_gst_WacManager.en_State == Wac_e_StartLoadingMap)
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    the map is loading
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		SND_gst_WacManager.f_VolumeFactor += (SND_gst_WacManager.f_FadeFactor * TIM_gf_dt);//0.02f);
		if(SND_gst_WacManager.f_VolumeFactor <= 0.0f) SND_gst_WacManager.f_VolumeFactor = 0.0f;
		


	}
	else if(SND_gst_WacManager.en_State == Wac_e_EndLoadingMap)
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    the map is loaded
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		SND_gst_WacManager.f_VolumeFactor += (SND_gst_WacManager.f_FadeFactor * TIM_gf_dt);//0.02f);


		if(SND_gst_WacManager.f_VolumeFactor >= 1.0f)
		{
			SND_gst_WacManager.f_VolumeFactor = 1.0f;
			SND_gst_WacManager.en_State = Wac_e_MapIsRunning;

			if(SND_gst_WacManager.b_NoWacIntoTheMap)
			{
				SND_ReinitWacInstance();
				SND_DeleteWacSound();
			}
			else
			{
				SND_Stop(SND_gst_WacManager.i_CurrentInstanceIndex);
			}
		}
		
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_WacSetLoading(int b_Start)
{
	if(b_Start)
	{
		if(SND_gst_WacManager.en_State != Wac_e_StartLoadingMap)
		{
			// auto play the wac
			if((SND_gst_WacManager.i_CurrentInstanceIndex != -1) && !SND_i_IsPlaying(SND_gst_WacManager.i_CurrentInstanceIndex))
			{
				SND_PlaySoundLooping(SND_gst_WacManager.i_CurrentInstanceIndex, -1);
			}

			SND_gst_WacManager.f_FadeFactor = (0.0f - SND_gst_WacManager.f_VolumeFactor) / SND_gst_WacManager.f_FadeOutTime;
		}

		SND_gst_WacManager.en_State = Wac_e_StartLoadingMap;
	}
	else
	{
		if(SND_gst_WacManager.en_State != Wac_e_EndLoadingMap)
		{
		    SND_gst_WacManager.f_VolumeFactor = 0.0f;
			SND_gst_WacManager.f_FadeFactor = (1.0f - SND_gst_WacManager.f_VolumeFactor) / SND_gst_WacManager.f_FadeInTime;
		}

		SND_gst_WacManager.en_State = Wac_e_EndLoadingMap;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_b_IsCurrWacSound(int index)
{
	return(SND_gst_WacManager.i_CurrentSoundIndex == index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_b_IsCurrWacInstance(int index)
{
	return(SND_gst_WacManager.i_CurrentInstanceIndex == index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_DetachWacInstance(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(SND_gst_WacManager.i_CurrentInstanceIndex, pst_SI, ;);

	if(pst_SI->ul_Flags & SND_Cul_SF_MaskDynVol)
	{
		pst_SI->ul_Flags &= ~SND_Cul_SF_MaskDynVol;
	}

	pst_SI->p_GameObject = NULL;
	pst_SI->pst_GlobalMatrix = NULL;
	pst_SI->pst_GlobalPos = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ReinitWacInstance(void)
{
	/*~~~~~~~~~~~~~~~*/
	LONG	l_Instance;
	/*~~~~~~~~~~~~~~~*/

	if(SND_gst_WacManager.i_CurrentInstanceIndex != -1)
	{
		l_Instance = SND_gst_WacManager.i_CurrentInstanceIndex;
		SND_gst_WacManager.i_CurrentInstanceIndex = -1;
		SND_Release(l_Instance);
	}

    SND_gst_WacManager.i_CurrentModifierIndex = -1;
    SND_gst_WacManager.i_RefNumber = 0;
    L_memset(SND_gst_WacManager.dst_WacReference, 0, sizeof(MDF_tdst_Modifier*) * SND_Cte_WacMdFRefMaxNb);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_NotifyNewLoading(void)
{
	SND_gst_WacManager.b_NewLoading = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_CleanOldWac(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	SND_tdst_OneSound	*pst_Sound;
	int					i_wacNb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_WacManager.b_NewLoading) return;
	SND_gst_WacManager.i_CurrentModifierIndex = -1;

	i_wacNb = 0;

	/*
	 * the aim is to delete the wac it is not referenced by the mapif the new map does
	 * not contain any wac file
	 */
	for(i = 0; i < SND_gst_Params.l_SoundNumber; i++)
	{
		pst_Sound = SND_gst_Params.dst_Sound + i;
		if(!(pst_Sound->ul_Flags & SND_Cul_DSF_Used)) continue;
		if(!(pst_Sound->ul_Flags & SND_Cul_SF_LoadingSound)) continue;
		i_wacNb++;

		/* if wac is referenced by the map continue */
        if(i==SND_gst_WacManager.i_CurrentSoundIndex) continue;
		if((int) pst_Sound->ul_CptUsed > 0) continue;
		i_wacNb--;

		pst_Sound->ul_CptUsed = 1;
		SND_DeleteByIndex(i);
	}

	SND_gst_WacManager.b_NoWacIntoTheMap = (i_wacNb > 0) ? FALSE : TRUE;
	SND_gst_WacManager.b_NewLoading = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_DeleteWacSound(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_OneSound	*pst_Sound;
	LONG				l_SoundIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(SND_gst_WacManager.i_CurrentSoundIndex < 0) return;

	pst_Sound = SND_gst_Params.dst_Sound + SND_gst_WacManager.i_CurrentSoundIndex;

	if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) return;
	if((pst_Sound->ul_Flags & SND_Cul_SF_LoadingSound) == 0) return;

	l_SoundIndex = SND_gst_WacManager.i_CurrentSoundIndex;
	pst_Sound->ul_CptUsed = 1;
	SND_gst_WacManager.i_CurrentSoundIndex = -1;
	SND_DeleteByIndex(l_SoundIndex);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_GetWacInstance(void)
{
	return SND_gst_WacManager.i_CurrentInstanceIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_GetWacSound(void)
{
	return SND_gst_WacManager.i_CurrentSoundIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetLoadingFadeInTime(float val)
{
	//SND_gst_WacManager.f_FadeInTime = val;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_GetLoadingFadeInTime(void)
{
	return SND_gst_WacManager.f_FadeInTime;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetLoadingFadeOutTime(float val)
{
	if(val)
		SND_gst_WacManager.f_FadeOutTime = val;
	else
	    SND_gst_WacManager.f_FadeOutTime = 0.01f; // cut in one frame	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_GetLoadingFadeOutTime(void)
{
	return SND_gst_WacManager.f_FadeOutTime;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_GetWacMute(SND_tdst_SoundInstance *_pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_WacSI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(SND_gst_WacManager.en_State == Wac_e_MapIsRunning) return 1.0f;

    if(SND_gst_WacManager.i_CurrentInstanceIndex != -1)
    {
	    SND_M_GetInstanceOrReturn(SND_gst_WacManager.i_CurrentInstanceIndex, pst_WacSI, 1.0f);
	    if(pst_WacSI == _pst_SI)
		    return 1.0f;
    }

    return SND_gst_WacManager.f_VolumeFactor;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
