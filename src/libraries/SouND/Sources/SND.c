/*$T SND.c GC 1.138 06/22/04 15:50:04 */

#include "Precomp.h"

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$1- general ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define INITGUID

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/BAStypes.h"
#include "LINks/LINKmsg.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/IMPort/IMPbase.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "AIinterp/Sources/AIengine.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "../Main/WinEditors/Sources/EDIpaths.h"

/*$1- sound module headers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/Sources/SNDerrid.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDdirect.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDmusic.h"
#include "SouND/Sources/SNDAmbience.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDmacros.h"
#include "SouND/Sources/SNDinterface.h"
#include "SouND/Sources/SNDinsert.h"
#include "SouND/Sources/SNDbank.h"
#include "SouND/Sources/SNDloadingsound.h"

#include "Editors/Sources/SOuNd/SONutil.h"

/*$1- xenon only ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(_XENON)
#include "Xenon/xeSND_Engine.h"
#endif

/*$1- editor only ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(ACTIVE_EDITORS)
#include "Editors/Sources/SOuNd/SONutil.h"
#include "SouND/Sources/SNDconv.h"
#include "LINks/LINKtoed.h"
#include "EDItors/Sources/SOuNd/SONmsg.h"

/*$1- ps2 only ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#elif defined(PSX2_TARGET)
#include "MainPsx2/Sources/PSX2debug.h"
#include "IOP/RPC_Manager.h"

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#endif

/*$4
 ***********************************************************************************************************************
    Private functions prototypes
 ***********************************************************************************************************************
 */

static int	SND_i_SoftInit(void);
static int	SND_i_HardInit(void);
static void SND_ComputeDoppler(MATH_tdst_Vector *, MATH_tdst_Vector *, SND_tdst_SoundInstance *);
#ifndef ACTIVE_EDITORS
static void SND_PlayRq(SND_tdst_SoundInstance *, int _i_Flag, int _i_StartPos, int _i_StopPos, int _i_LoopCount);
#endif
static void SND_Null(void *);
static void SND_SetMicroPos(MATH_tdst_Matrix *);
extern void SND_FxNetworkInit(void);

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

/*$1- global ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(_XENON) && !_XENON_SOUND_ENGINE
char SND_gc_NoSound = 1; /* Forced disabled sound */
#else
char SND_gc_NoSound = 0; /* enable / disable sound */
#endif

SND_tdst_Parameters SND_gst_Params;		/* global settings */
OBJ_tdst_GameObject *SND_gpst_RqMicroPosFromAI = NULL;

/*$1- private ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

SND_tdst_LowInterface	SND_gpst_Interface[ISize];	/* sound buffer interface */

/*$4
 ***********************************************************************************************************************
    INIT/CLOSE module
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void SND_InitModule(void)
{
	/*~~*/
#ifdef ACTIVE_EDITORS
	int i;
#endif
	/*~~*/

	SND_l_InitVolumeTable();
	SND_RasterInitModule();
    SND_WaveInitModule();

	/*$2- default config ---------------------------------------------------------------------------------------------*/

	SND_gpst_RqMicroPosFromAI = NULL;

	/*$2- load variables ---------------------------------------------------------------------------------------------*/

	/*$2- global struct ----------------------------------------------------------------------------------------------*/

	L_memset(&SND_gst_Params, 0, sizeof(SND_tdst_Parameters));
	SND_gst_Params.pst_SpecificD = (SND_tdst_TargetSpecificData *) MEM_p_Alloc(sizeof(SND_tdst_TargetSpecificData));
	SND_gst_Params.f_SurroundFactor = 1.2f;
	SND_gst_Params.f_RolloffFactor = 1.0f;
	SND_gst_Params.f_DopplerFactor = 1.0f;
	SND_gst_Params.f_DopplerSoundSpeed = SND_Cte_DopplerSndSpeed;
    SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayNear;
    SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayFar;

/*$2- pr -------------------------------------------------------------------------------------------------------------*/

#ifdef ACTIVE_EDITORS
	SND_gst_Params.l_InstanceNumber = SND_gst_Params.l_MaxInstanceNumber = 400;
#else
	SND_gst_Params.l_InstanceNumber = SND_gst_Params.l_MaxInstanceNumber = 400;
#endif
	//SND_gst_Params.dst_Instance = MEM_p_Alloc(SND_gst_Params.l_MaxInstanceNumber * sizeof(SND_tdst_SoundInstance));
	SND_gst_Params.dst_Instance = (SND_tdst_SoundInstance*)MEM_p_Alloc(SND_gst_Params.l_MaxInstanceNumber * sizeof(SND_tdst_SoundInstance));
	L_memset(SND_gst_Params.dst_Instance, 0, SND_gst_Params.l_MaxInstanceNumber * sizeof(SND_tdst_SoundInstance));

	/*$2- pr ---------------------------------------------------------------------------------------------------------*/

	SND_gst_Params.l_SoundNumber = SND_Cte_SoundNbMax;
	L_memset(SND_gst_Params.dst_Sound, 0, SND_gst_Params.l_SoundNumber * sizeof(SND_tdst_OneSound));

	/*$2--------------------------------------------------------------------------------------------------------------*/

	SND_gst_Params.l_InsertNumber = SND_Cte_InsertNbMax;
	L_memset(SND_gst_Params.dst_Insert, 0, SND_gst_Params.l_InsertNumber * sizeof(SND_tdst_Insert));

	/*$2--------------------------------------------------------------------------------------------------------------*/

	SND_gst_Params.l_MetabankNumber = 20;
	SND_gst_Params.dp_Metabank = (SND_tdst_Metabank**)MEM_p_Alloc(SND_gst_Params.l_MetabankNumber * sizeof(SND_tdst_Metabank *));
//	SND_gst_Params.dp_Metabank = MEM_p_Alloc(SND_gst_Params.l_MetabankNumber * sizeof(SND_tdst_Metabank *));
	L_memset(SND_gst_Params.dp_Metabank, 0, SND_gst_Params.l_MetabankNumber * sizeof(SND_tdst_Metabank *));

	/*$2- interface --------------------------------------------------------------------------------------------------*/

	L_memset(SND_gpst_Interface, 0, ISize * sizeof(SND_tdst_LowInterface));

#ifdef ACTIVE_EDITORS

	/*$2- editor specific variables ----------------------------------------------------------------------------------*/

	SND_gb_EdiForceKill = FALSE;
	SND_gb_EdiPause = FALSE;

	i = IMP_b_AddImportCallback("wav", ESON_b_ImportWAVCallBack);
	i &= IMP_b_AddImportCallback("wac", ESON_b_ImportWACCallBack);
	i &= IMP_b_AddImportCallback("waa", ESON_b_ImportWAACallBack);
	i &= IMP_b_AddImportCallback("wad", ESON_b_ImportWADCallBack);
	i &= IMP_b_AddImportCallback("wam", ESON_b_ImportWAMCallBack);
	if(!i)
	{
		MessageBox(NULL, "Can't register sound import callbacks", "Bad Sound Init", MB_OK | MB_ICONSTOP);
		return;
	}

	ESON_InitUtilModule();
#endif

	/*$2- interface assignation --------------------------------------------------------------------------------------*/

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_Initialize();
#endif

	if(SND_i_SoftInit()) return;
	SND_i_ChangeRenderMode(SND_Cte_RenderUseConsoleSettings);

	/*$2- hardware init ----------------------------------------------------------------------------------------------*/

	if(SND_gc_NoSound) return;
	if(SND_i_HardInit()) return;
	

	/*$2- signal sound enabled for su --------------------------------------------------------------------------------*/

	SND_gst_Params.l_Available = 1;

	/*$2- sub module init --------------------------------------------------------------------------------------------*/

	SND_InitMdFModule();
	SND_TrackInitModule();
	SND_StreamInitModule();
	SND_LoadingSoundInitModule();
	SND_InsertInitModule();

	SND_i_FxInitModule();
	SND_SModifierInitModule();
    SND_FxNetworkInit();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_SoftInit(void)
{
#if defined( ACTIVE_EDITORS )
	if(MAI_gh_MainWindow == NULL) return -1;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    EDITOR
	 -------------------------------------------------------------------------------------------------------------------
	 */

	SND_gst_Params.ul_RenderModeCapacity |= SND_Cte_RenderMono;
	SND_gst_Params.ul_RenderModeCapacity |= SND_Cte_RenderStereo;
	SND_gpst_Interface[ISound].pfi_ChangeRenderMode = ediSND_i_ChangeRenderMode;

	SND_gpst_Interface[ISound].pfi_SndBuffGetStatus = ediSND_SB_GetStatus;
	SND_gpst_Interface[ISound].pfi_SndBuffPlay = ediSND_SB_Play;
	SND_gpst_Interface[ISound].pfp_SndBuffCreate = ediSND_pst_SB_Create;
	SND_gpst_Interface[ISound].pfv_Close = ediSND_Close;
	SND_gpst_Interface[ISound].pfl_Init = ediSND_l_Init;
	SND_gpst_Interface[ISound].pfv_SndBuffDuplicate = ediSND_SB_Duplicate;
	SND_gpst_Interface[ISound].pfv_SndBuffGetFreq = ediSND_SB_GetFrequency;
	SND_gpst_Interface[ISound].pfv_SndBuffGetPan = ediSND_SB_GetPan;
	SND_gpst_Interface[ISound].pfv_SndBuffGetPos = ediSND_SB_GetCurrPos;
	SND_gpst_Interface[ISound].pfv_SndBuffPause = ediSND_SB_Pause;
	SND_gpst_Interface[ISound].pfv_SndBuffRelease = ediSND_SB_Release;
	SND_gpst_Interface[ISound].pfv_SndBuffSetFreq = ediSND_SB_SetFrequency;
	SND_gpst_Interface[ISound].pfv_SndBuffSetPan = ediSND_SB_SetPan;
	SND_gpst_Interface[ISound].pfv_SndBuffSetPos = ediSND_SB_SetCurrPos;
	SND_gpst_Interface[ISound].pfv_SndBuffSetVol = ediSND_SB_SetVolume;
	SND_gpst_Interface[ISound].pfv_SndBuffStop = ediSND_SB_Stop;

	SND_gpst_Interface[ISound].pfp_SndBuffReLoad = ediSND_SB_ReLoad;

	SND_gpst_Interface[ISound].pfv_StreamPrefetch = ediSND_StreamPrefetch;
	SND_gpst_Interface[ISound].pfv_StreamPrefetchArray = ediSND_StreamPrefetchArray;
	SND_gpst_Interface[ISound].pfv_StreamFlush = ediSND_StreamFlush;
	SND_gpst_Interface[ISound].pfv_StreamGetPrefetchStatus = ediSND_StreamGetPrefetchStatus;
	SND_gpst_Interface[ISound].pfv_StreamReinitAndPlay = ediSND_StreamReinitAndPlay;
	SND_gpst_Interface[ISound].pfv_SndBuffCreateTrack = ediSND_CreateTrackSB;
	SND_gpst_Interface[ISound].pfv_SndBuffReleaseTrack = ediSND_ReleaseTrackSB;
	SND_gpst_Interface[ISound].pfv_StreamShutDownAll = ediSND_ShutDownStreams;
	SND_gpst_Interface[ISound].pfv_StreamChain = ediSND_StreamChain;
	SND_gpst_Interface[ISound].pfv_StreamLoopCountGet = ediSND_StreamLoopCountGet;
    SND_gpst_Interface[ISound].pfv_StreamChainDelayGet = ediSND_StreamChainDelayGet;

	SND_gpst_Interface[ISound].pfl_GlobalGetVol = ediSND_l_GetGlobalVol;
	SND_gpst_Interface[ISound].pfv_GlobalSetVol = ediSND_SetGlobalVol;

	SND_gpst_Interface[ISound].pfv_ComputeDoppler = SND_ComputeDoppler;
	SND_gpst_Interface[ISound].pff_Compute3DVol = SND_f_Compute3DVol;
	SND_gpst_Interface[ISound].pfv_CommitChange = ediSND_CommitChange;

	SND_gpst_Interface[ISound].pfi_FxInit = ediSND_i_FxInit;
	SND_gpst_Interface[ISound].pfv_FxClose = ediSND_FxClose;
	SND_gpst_Interface[ISound].pfi_FxSetMode = ediSND_i_FxSetMode;
	SND_gpst_Interface[ISound].pfi_FxSetFeedback = ediSND_i_FxSetFeedback;
	SND_gpst_Interface[ISound].pfi_FxSetDelay = ediSND_i_FxSetDelay;
	SND_gpst_Interface[ISound].pfi_FxSetWetVolume = ediSND_i_FxSetWetVolume;
	SND_gpst_Interface[ISound].pfi_FxSetWetPan = ediSND_i_FxSetWetPan;
	SND_gpst_Interface[ISound].pfi_FxEnable = ediSND_i_FxEnable;
	SND_gpst_Interface[ISound].pfi_FxDisable = ediSND_i_FxDisable;
#elif defined(PCWIN_TOOL)
	if(MAI_gh_MainWindow == NULL) return -1;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    WIN32
	 -------------------------------------------------------------------------------------------------------------------
	 */

	SND_gst_Params.ul_RenderModeCapacity |= SND_Cte_RenderMono;
	SND_gst_Params.ul_RenderModeCapacity |= SND_Cte_RenderStereo;
	SND_gpst_Interface[ISound].pfi_ChangeRenderMode = win32SND_i_ChangeRenderMode;

	SND_gpst_Interface[ISound].pfi_SndBuffGetStatus = win32SND_SB_GetStatus;
	SND_gpst_Interface[ISound].pfi_SndBuffPlay = win32SND_SB_Play;
	SND_gpst_Interface[ISound].pfp_SndBuffCreate = win32SND_pst_SB_Create;
	SND_gpst_Interface[ISound].pfv_Close = win32SND_Close;
	SND_gpst_Interface[ISound].pfl_Init = win32SND_l_Init;
	SND_gpst_Interface[ISound].pfv_SndBuffDuplicate = win32SND_SB_Duplicate;
	SND_gpst_Interface[ISound].pfv_SndBuffGetFreq = win32SND_SB_GetFrequency;
	SND_gpst_Interface[ISound].pfv_SndBuffGetPan = win32SND_SB_GetPan;
	SND_gpst_Interface[ISound].pfv_SndBuffGetPos = win32SND_SB_GetCurrPos;
	SND_gpst_Interface[ISound].pfv_SndBuffPause = win32SND_SB_Pause;
	SND_gpst_Interface[ISound].pfv_SndBuffRelease = win32SND_SB_Release;
	SND_gpst_Interface[ISound].pfv_SndBuffSetFreq = win32SND_SB_SetFrequency;
	SND_gpst_Interface[ISound].pfv_SndBuffSetPan = win32SND_SB_SetPan;
	SND_gpst_Interface[ISound].pfv_SndBuffSetPos = win32SND_SB_SetCurrPos;
	SND_gpst_Interface[ISound].pfv_SndBuffSetVol = win32SND_SB_SetVolume;
	SND_gpst_Interface[ISound].pfv_SndBuffStop = win32SND_SB_Stop;

	SND_gpst_Interface[ISound].pfp_SndBuffReLoad = win32SND_SB_ReLoad;

	SND_gpst_Interface[ISound].pfv_StreamPrefetch = win32SND_StreamPrefetch;
	SND_gpst_Interface[ISound].pfv_StreamPrefetchArray = win32SND_StreamPrefetchArray;
	SND_gpst_Interface[ISound].pfv_StreamFlush = win32SND_StreamFlush;
	SND_gpst_Interface[ISound].pfv_StreamGetPrefetchStatus = win32SND_StreamGetPrefetchStatus;
	SND_gpst_Interface[ISound].pfv_StreamReinitAndPlay = win32SND_StreamReinitAndPlay;
	SND_gpst_Interface[ISound].pfv_SndBuffCreateTrack = win32SND_CreateTrackSB;
	SND_gpst_Interface[ISound].pfv_SndBuffReleaseTrack = win32SND_ReleaseTrackSB;
	SND_gpst_Interface[ISound].pfv_StreamShutDownAll = win32SND_ShutDownStreams;
	SND_gpst_Interface[ISound].pfv_StreamChain = win32SND_StreamChain;
	SND_gpst_Interface[ISound].pfv_StreamLoopCountGet = win32SND_StreamLoopCountGet;
    SND_gpst_Interface[ISound].pfv_StreamChainDelayGet = win32SND_StreamChainDelayGet;

	SND_gpst_Interface[ISound].pfl_GlobalGetVol = win32SND_l_GetGlobalVol;
	SND_gpst_Interface[ISound].pfv_GlobalSetVol = win32SND_SetGlobalVol;

	SND_gpst_Interface[ISound].pfv_ComputeDoppler = SND_ComputeDoppler;
	SND_gpst_Interface[ISound].pff_Compute3DVol = SND_f_Compute3DVol;
#ifdef JADEFUSION
	SND_gpst_Interface[ISound].pfv_CommitChange = win32SND_CommitChange;//NULL;/*MSND_Null*/
#else
	SND_gpst_Interface[ISound].pfv_CommitChange = SND_Null;
#endif

	SND_gpst_Interface[ISound].pfi_FxInit = win32SND_i_FxInit;
	SND_gpst_Interface[ISound].pfv_FxClose = win32SND_FxClose;
	SND_gpst_Interface[ISound].pfi_FxSetMode = win32SND_i_FxSetMode;
	SND_gpst_Interface[ISound].pfi_FxSetFeedback = win32SND_i_FxSetFeedback;
	SND_gpst_Interface[ISound].pfi_FxSetDelay = win32SND_i_FxSetDelay;
	SND_gpst_Interface[ISound].pfi_FxSetWetVolume = win32SND_i_FxSetWetVolume;
	SND_gpst_Interface[ISound].pfi_FxSetWetPan = win32SND_i_FxSetWetPan;
	SND_gpst_Interface[ISound].pfi_FxEnable = win32SND_i_FxEnable;
	SND_gpst_Interface[ISound].pfi_FxDisable = win32SND_i_FxDisable;
#endif

	/*$2- stream interface -------------------------------------------------------------------------------------------*/

	L_memcpy(&SND_gpst_Interface[IStream], &SND_gpst_Interface[ISound], sizeof(SND_tdst_LowInterface));
	SND_gpst_Interface[IStream].pfv_Close = NULL;
	SND_gpst_Interface[IStream].pfl_Init = NULL;

	SND_gpst_Interface[IStream].pfi_SndBuffGetStatus = SND_Stream_GetStatus;
	SND_gpst_Interface[IStream].pfi_SndBuffPlay = SND_Stream_Play;
	SND_gpst_Interface[IStream].pfp_SndBuffCreate = SND_Stream_Create;
	SND_gpst_Interface[IStream].pfv_SndBuffDuplicate = SND_Stream_Duplicate; 
	SND_gpst_Interface[IStream].pfv_SndBuffGetFreq = SND_Stream_GetFrequency;
	SND_gpst_Interface[IStream].pfv_SndBuffGetPan = SND_Stream_GetPan;
	SND_gpst_Interface[IStream].pfv_SndBuffGetPos = SND_Stream_GetCurrPos;
	SND_gpst_Interface[IStream].pfv_SndBuffPause = SND_Stream_Pause;
	SND_gpst_Interface[IStream].pfv_SndBuffRelease = SND_Stream_Release;
	SND_gpst_Interface[IStream].pfv_SndBuffSetFreq = SND_Stream_SetFrequency;
	SND_gpst_Interface[IStream].pfv_SndBuffSetPan = SND_Stream_SetPan;
	SND_gpst_Interface[IStream].pfv_SndBuffSetPos = SND_Stream_SetCurrPos;
	SND_gpst_Interface[IStream].pfv_SndBuffSetVol = SND_Stream_SetVolume;
	SND_gpst_Interface[IStream].pfv_SndBuffStop = SND_Stream_Stop;

	/*$2- track interface --------------------------------------------------------------------------------------------*/

	L_memcpy(&SND_gpst_Interface[ITrack], &SND_gpst_Interface[ISound], sizeof(SND_tdst_LowInterface));
	SND_gpst_Interface[ITrack].pfv_Close = NULL;
	SND_gpst_Interface[ITrack].pfl_Init = NULL;

	SND_gpst_Interface[ITrack].pfi_SndBuffGetStatus = SND_iTrack_GetStatus;
	SND_gpst_Interface[ITrack].pfi_SndBuffPlay = SND_iTrack_Play;
	SND_gpst_Interface[ITrack].pfp_SndBuffCreate = NULL;
	SND_gpst_Interface[ITrack].pfv_SndBuffDuplicate = NULL;
	SND_gpst_Interface[ITrack].pfv_SndBuffGetFreq = SND_Track_GetFrequency;
	SND_gpst_Interface[ITrack].pfv_SndBuffGetPan = SND_Track_GetPan;
	SND_gpst_Interface[ITrack].pfv_SndBuffGetPos = SND_Track_GetCurrPos;
	SND_gpst_Interface[ITrack].pfv_SndBuffPause = SND_Track_Pause;
	SND_gpst_Interface[ITrack].pfv_SndBuffRelease = SND_Track_Release;
	SND_gpst_Interface[ITrack].pfv_SndBuffSetFreq = SND_Track_SetFrequency;
	SND_gpst_Interface[ITrack].pfv_SndBuffSetPan = SND_Track_SetPan;
	SND_gpst_Interface[ITrack].pfv_SndBuffSetPos = SND_Track_SetCurrPos;
	SND_gpst_Interface[ITrack].pfv_SndBuffSetVol = SND_Track_SetVolume;
	SND_gpst_Interface[ITrack].pfv_SndBuffStop = SND_Track_Stop;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_HardInit(void)
{
	if(SND_gpst_Interface[ISound].pfl_Init(SND_gst_Params.pst_SpecificD) == 0)
	{
		SND_CloseModule();
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_CloseModule(void)
{
    LONG i;

	if(SND_gst_Params.l_Available)
	{
        for(i=0; i<SND_gst_Params.l_SoundNumber; i++)
            while(SND_gst_Params.dst_Sound[i].ul_CptUsed) SND_DeleteByIndex(i);

		SND_InsertCloseModule();
        SND_LoadingSoundCloseModule();

#ifdef ACTIVE_EDITORS

		/*$2- only editor --------------------------------------------------------------------------------------------*/

		ESON_KillAllSounds();
		ESON_CloseUtilModule();
#endif

		/*$2- submodule close ----------------------------------------------------------------------------------------*/

		SND_RasterCloseModule();
		SND_SModifierCloseModule();
		SND_FxCloseModule();
		SND_CloseMdFModule();

		/*$2- hard close ---------------------------------------------------------------------------------------------*/

		if(SND_gst_Params.pst_SpecificD)
		{
			SND_TrackCloseModule();
			SND_StreamCloseModule();
			SND_gpst_Interface[ISound].pfv_Close(SND_gst_Params.pst_SpecificD);
		}

		SND_gst_Params.l_Available = 0;
	}

	/*$2- soft close -------------------------------------------------------------------------------------------------*/

	if(SND_gst_Params.pst_SpecificD) MEM_Free(SND_gst_Params.pst_SpecificD);
	SND_gst_Params.pst_SpecificD = NULL;

	if(SND_gst_Params.dst_Instance)
	{
		MEM_Free(SND_gst_Params.dst_Instance);
		SND_gst_Params.dst_Instance = NULL;
		SND_gst_Params.l_InstanceNumber = 0;
		SND_gst_Params.l_MaxInstanceNumber = 0;
	}

	if(SND_gst_Params.dp_Metabank)
	{
		MEM_Free(SND_gst_Params.dp_Metabank);
		SND_gst_Params.dp_Metabank = NULL;
		SND_gst_Params.l_MetabankNumber = 0;
	}

	L_memset(SND_gpst_Interface, 0, ISize * sizeof(SND_tdst_LowInterface));

	// Destroy Audio Console

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_Uninit();
#endif
}

/*$4
 ***********************************************************************************************************************
    List of sound
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_GetSoundIndex(BIG_KEY _ul_WavKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_OneSound	*pst_Sound, *pst_Last;
	LONG				l_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_ul_WavKey == BIG_C_InvalidKey) return -1;

	pst_Sound = SND_gst_Params.dst_Sound;
	pst_Last = pst_Sound + SND_gst_Params.l_SoundNumber;
	for(l_Index = 0; pst_Sound < pst_Last; pst_Sound++, l_Index++)
	{
		if(!(pst_Sound->ul_Flags & SND_Cul_DSF_Used)) continue;
		if(pst_Sound->ul_FileKey == _ul_WavKey) return l_Index;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_GetSoundFromInstance(LONG _l_Instance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Instance, pst_SI, -1);

	return pst_SI->l_Sound;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Delete(BIG_KEY _ul_WavKey)
{
	/*~~~~~~~~~~~~*/
	LONG	l_Index;
	/*~~~~~~~~~~~~*/

	l_Index = SND_l_GetSoundIndex(_ul_WavKey);
	if(l_Index == -1) return;
	SND_DeleteByIndex(l_Index);
}

/*$4
 ***********************************************************************************************************************
    List of instance
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static LONG SND_l_GetUnusedInstance(void)
{
	/*~~~~~~*/
	LONG	i;
	/*~~~~~~*/

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		if((SND_gst_Params.dst_Instance[i].ul_Flags & SND_Cul_DSF_Used) == 0) return i;
	}

	if(SND_gst_Params.l_InstanceNumber < SND_gst_Params.l_MaxInstanceNumber) return SND_gst_Params.l_InstanceNumber++;

	if(SND_gst_Params.l_MaxInstanceNumber)
	{
		ERR_X_Warning(0,"[SND] vous depassez les 400 instances de son !!!",NULL);
		return -1;

		SND_gst_Params.l_MaxInstanceNumber += SND_Cte_MinAllocInst;
		SND_gst_Params.dst_Instance = (SND_tdst_SoundInstance *) MEM_p_Realloc
			(
				SND_gst_Params.dst_Instance,
				SND_gst_Params.l_MaxInstanceNumber * sizeof(SND_tdst_SoundInstance)
			);
	}
	else
	{
		SND_gst_Params.l_MaxInstanceNumber = SND_Cte_MinAllocInst;
		SND_gst_Params.dst_Instance = (SND_tdst_SoundInstance *) MEM_p_Alloc(SND_Cte_MinAllocInst * sizeof(SND_tdst_SoundInstance));
	}

	L_memset
	(
		SND_gst_Params.dst_Instance + (SND_gst_Params.l_MaxInstanceNumber - SND_Cte_MinAllocInst),
		0,
		SND_Cte_MinAllocInst * sizeof(SND_tdst_SoundInstance)
	);

	return SND_gst_Params.l_InstanceNumber++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_Request(void *p_GO, LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG					l_Instance;
	SND_tdst_OneSound		*pst_Sound;
	SND_tdst_SoundInstance	*pst_Instance;
	OBJ_tdst_GameObject		*pst_GAO;
	SND_tdst_Bank			*pBank;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!SND_gst_Params.l_Available) return -1;
	if(SND_gc_NoSound) return -1;

#ifndef _FINAL_
	if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoRq) return -1;
#endif
	pst_GAO = (OBJ_tdst_GameObject *) p_GO;
	

	if((SND_gst_Params.ul_Flags & SND_Cte_UseGlobalIndex) == 0)
	{
		if(pst_GAO)
		{
			if(!OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Sound)) return -1;
        	if(!SND_M_GoodPointer(pst_GAO->pst_Extended)) return -1;
			if(!SND_M_GoodPointer(pst_GAO->pst_Extended->pst_Sound)) return -1;
		
			pBank = SND_p_MainGetBank(pst_GAO->pst_Extended->pst_Sound);
			if(!SND_M_GoodPointer(pBank)) return -1;
			if(!SND_M_GoodPointer(pBank->pi_Bank)) return -1;
			if((_l_Index < 0) || (_l_Index >= pBank->i_SoundNb)) return -1;
			_l_Index = pBank->pi_Bank[_l_Index];
		}
	}
	

	if(_l_Index == -1) return -1;
	if((_l_Index < 0) || (_l_Index >= SND_gst_Params.l_SoundNumber)) return -1;
	pst_Sound = SND_gst_Params.dst_Sound + _l_Index;

	_SND_DebugSpy(pst_Sound->ul_FileKey, SND_l_Request);

	if(!(pst_Sound->ul_Flags & SND_Cul_DSF_Used)) return -1;

	if((pst_Sound->ul_Flags & SND_Cul_OSF_Loaded) == 0)
	{
		if(pst_Sound->ul_Flags & SND_Cte_StreamedFile)
			SND_l_AddStreamLoad(pst_Sound->ul_FileKey);
		else if(pst_Sound->ul_Flags & SND_Cul_SF_LoadingSound)
            SND_l_LoadWacData(_l_Index);
        else
			SND_l_LoadData(pst_Sound->ul_FileKey);
	}

    if((pst_Sound->ul_Flags & SND_Cul_OSF_Loaded) == 0) return -1;

	if(pst_Sound->pst_SModifier)
	{
        if((int) pst_Sound->pst_DSB == -1) 
        {
            SND_SModifierChangeSoundIndex(pst_Sound->pst_SModifier);
            return -1;
        }
        
		l_Instance = SND_l_Request(NULL, (int) pst_Sound->pst_DSB);
		if(l_Instance != -1)
		{
			pst_Instance = SND_gst_Params.dst_Instance + l_Instance;
			pst_Instance->p_GameObject = p_GO;

			/* apply Smodifier on Instance */
			SND_SModifierInstanciate(pst_Sound->pst_SModifier, pst_Instance);

			/* settings */
			SND_SetInstVolume(pst_Instance);
			pst_Instance->pst_LI->pfv_SndBuffSetFreq(pst_Instance->pst_DSB, pst_Instance->i_BaseFrequency);

#ifdef ACTIVE_EDITORS
			pst_Instance->ul_SmdKey = pst_Sound->ul_FileKey;
#endif
		}

		return l_Instance;
	}

	l_Instance = SND_l_GetUnusedInstance();
	if(l_Instance == -1)
	{
		return -1;
	}

#ifdef SND_RASTER
	if(pst_Sound->ul_Flags & SND_Cte_StreamedFile)
		SND_StatAddSInstance();
	else
		SND_StatAddInstance();
#endif /* SND_RASTER */

	pst_Instance = SND_gst_Params.dst_Instance + l_Instance;
	pst_Instance->l_Sound = _l_Index;
	pst_Instance->af_Near[0] = SND_Cte_DefaultNear;
	pst_Instance->af_Near[1] = SND_Cte_DefaultNear;
	pst_Instance->af_Near[2] = SND_Cte_DefaultNear;
	pst_Instance->af_Far[0] = SND_Cte_DefaultFar;
	pst_Instance->af_Far[1] = SND_Cte_DefaultFar;
	pst_Instance->af_Far[2] = SND_Cte_DefaultFar;
	pst_Instance->f_DeltaFar = SND_Cte_DefaultDeltaFar;

	pst_Instance->af_Middle[0] = SND_Cte_DefaultMiddle;
	pst_Instance->af_Middle[1] = SND_Cte_DefaultMiddle;
	pst_Instance->af_Middle[2] = SND_Cte_DefaultMiddle;
	pst_Instance->f_MiddleCoeff = 0.5f;
	pst_Instance->f_FarCoeff = 0.0f;
	pst_Instance->f_CylinderHeight = 0.0f;

	pst_Instance->pst_GlobalMatrix = NULL;
	pst_Instance->pst_GlobalPos = NULL;
	MATH_InitVector(&pst_Instance->st_LastUpdatedPos, 0.0f, 0.0f, 0.0f);
	MATH_InitVector(&pst_Instance->st_UpdatedPos, 0.0f, 0.0f, 0.0f);
	pst_Instance->ul_Flags = SND_Cul_DSF_Used | (pst_Sound->ul_Flags & SND_Cul_SF_MaskFlagsFromSoundToInstance);
	pst_Instance->ul_ExtFlags = (pst_Sound->ul_ExtFlags & SND_Cul_ESF_MaskDynExtFlags);
	pst_Instance->p_GameObject = p_GO;

    pst_Instance->i_BaseFrequency = pst_Sound->pst_Wave->dwSamplesPerSec;
	pst_Instance->i_StartFrequency = pst_Sound->pst_Wave->dwSamplesPerSec;

	pst_Instance->pst_LI = pst_Sound->pst_LI;
	pst_Instance->i_InstTrack = -1;
	pst_Instance->f_PlayingStartDate = 0.0f;
	pst_Instance->f_Volume = 1.0f;
	pst_Instance->i_Front = 0;
	pst_Instance->i_Pan = 0;
	pst_Instance->f_MinPan = 0.0f;
	pst_Instance->pst_SModifier = NULL;
	pst_Instance->f_Delay = 0.0f;
    pst_Instance->i_LoopNb = 0;
	pst_Instance->f_StartDate = 0.0f;
	pst_Instance->f_Doppler = 1.0f;
	pst_Instance->f_FxVolLeft = 0.0f;
	pst_Instance->f_FxVolRight = 0.0f;

#ifdef ACTIVE_EDITORS
	pst_Instance->ul_EdiFlags = 0;
	pst_Instance->ul_SmdKey = -1;
#endif
	switch(pst_Sound->ul_Flags & SND_Cul_SF_MaskFileType)
	{
	case SND_Cul_SF_Music:			pst_Instance->i_GroupId = SND_Cte_GrpMusic; break;
	case SND_Cul_SF_Dialog:			pst_Instance->i_GroupId = SND_Cte_GrpDialog; break;
	case SND_Cul_SF_Ambience:		pst_Instance->i_GroupId = SND_Cte_GrpAmbience; break;
	case SND_Cul_SF_SModifier:		pst_Instance->i_GroupId = SND_Cte_GrpSpecialFX; break;
	case SND_Cul_SF_LoadingSound:	pst_Instance->i_GroupId = SND_Cte_GrpSpecialFX; break;
	case 0:							pst_Instance->i_GroupId = SND_Cte_GrpSpecialFX; break;
	default:						pst_Instance->i_GroupId = SND_Cte_GrpSpecialFX; break;
	}

	if(pst_Sound->ul_Flags & SND_Cul_DSF_Instance)
	{
		_SND_DebugSpy(pst_Sound->ul_FileKey, SND_l_Request[duplicate]);

		pst_Sound->pst_LI->pfv_SndBuffDuplicate
			(
				SND_gst_Params.pst_SpecificD,
				pst_Sound->pst_DSB,
				&pst_Instance->pst_DSB
			);
		if(!pst_Instance->pst_DSB)
		{
			SND_FreeInstance(l_Instance);
			return -1;
		}

		pst_Instance->ul_Flags |= SND_Cul_DSF_Instance;
	}
	else
	{
		pst_Instance->pst_DSB = pst_Sound->pst_DSB;
		pst_Sound->ul_Flags |= SND_Cul_DSF_Instance;

#ifndef PSX2_TARGET
		pst_Instance->pst_LI->pfv_SndBuffSetPos(pst_Instance->pst_DSB, 0);
#endif
	}
    
    SND_SetInstVolume(pst_Instance);
	pst_Instance->pst_LI->pfv_SndBuffSetFreq(pst_Instance->pst_DSB, pst_Instance->i_BaseFrequency);
	pst_Instance->pst_LI->pfv_SndBuffSetPan(pst_Instance->pst_DSB, 0, 0);

	if(pst_Sound->ul_Flags & SND_Cul_SF_Ambience) SND_InstTrackSet(l_Instance, SND_e_TrackA);
	if(pst_Sound->ul_Flags & SND_Cul_SF_Music) SND_InstTrackSet(l_Instance, SND_e_TrackB);
	// Dialogs : TrackD or else TrackC
	if(pst_Sound->ul_Flags & SND_Cul_SF_Dialog)
		SND_InstTrackSet(l_Instance, SND_DialogInstChooseTrack() ? SND_e_TrackC : SND_e_TrackD);

	return l_Instance;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Release(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
    if(SND_b_IsCurrWacInstance(_l_Index)) return;

	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_Release);

	pst_SI->pst_LI->pfv_SndBuffStop(pst_SI->pst_DSB);
	SND_FreeInstance(_l_Index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetFlags(OBJ_tdst_GameObject *p_GO, LONG _l_Index, ULONG _ul_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);

	if(_ul_Flags & SND_Cul_SF_UseExtFlag)
	{
		if((_ul_Flags & SND_Cul_ESF_RandomPlay) && SND_gst_Params.dst_Sound[pst_SI->l_Sound].pst_Wave)
		{
			if
			(
				SND_gst_Params.dst_Sound[pst_SI->l_Sound].pst_Wave->ul_LoopBeginOffset != SND_gst_Params.dst_Sound[
					pst_SI->l_Sound].pst_Wave->ul_StartOffset
			)
			{
				_ul_Flags &= ~SND_Cul_ESF_RandomPlay;
			}
		}

		_ul_Flags &= ~SND_Cul_SF_UseExtFlag;
		pst_SI->ul_ExtFlags |= (_ul_Flags & SND_Cul_ESF_MaskDynExtFlags);
	}
	else
	{
		pst_SI->ul_Flags |= (_ul_Flags);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ResetFlags(OBJ_tdst_GameObject *p_GO, LONG _l_Index, ULONG _ul_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);

	if(_ul_Flags & SND_Cul_SF_UseExtFlag)
	{
		_ul_Flags &= ~SND_Cul_SF_UseExtFlag;
		pst_SI->ul_ExtFlags &= ~(_ul_Flags & SND_Cul_ESF_MaskDynExtFlags);
	}
	else
	{
		pst_SI->ul_Flags &= ~(_ul_Flags);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetObject(OBJ_tdst_GameObject *pst_GO, int index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(index, pst_SI, ;);
	SND_i_CheckOrInitGAO(pst_GO);

	pst_SI->p_GameObject = pst_GO;
	SND_SetPos(pst_GO, index, pst_GO->pst_GlobalMatrix);

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetPos(void *p_GO, LONG _l_Index, MATH_tdst_Matrix *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_SetPos);
	pst_SI->pst_GlobalMatrix = _pst_Pos;
	pst_SI->pst_GlobalPos = MATH_pst_GetTranslation(_pst_Pos);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetNearFar(LONG _l_Index, int _i_flag, float _f_Near, float _f_Far)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_SetNearFar);

	switch(_i_flag)
	{
	case SND_Cte_SetInstNearFar0:
		ERR_X_Warning((_f_Near <= _f_Far), "near is lower than far (SND_SetNearFar)", NULL);
		pst_SI->af_Near[0] = _f_Near;
		pst_SI->af_Far[0] = _f_Far;
		if(pst_SI->af_Middle[0] == SND_Cte_DefaultMiddle)
		{
			pst_SI->af_Middle[0] = (_f_Near + _f_Far) * 0.5f;
		}
		break;

	case SND_Cte_SetInstNearFar1:
		ERR_X_Warning((_f_Near <= _f_Far), "near is lower than far (SND_SetNearFar)", NULL);
		pst_SI->af_Near[1] = _f_Near;
		pst_SI->af_Far[1] = _f_Far;
		if(pst_SI->af_Middle[1] == SND_Cte_DefaultMiddle)
		{
			pst_SI->af_Middle[1] = (_f_Near + _f_Far) * 0.5f;
		}
		break;

	case SND_Cte_SetInstNearFar2:
		ERR_X_Warning((_f_Near <= _f_Far), "near is lower than far (SND_SetNearFar)", NULL);
		pst_SI->af_Near[2] = _f_Near;
		pst_SI->af_Far[2] = _f_Far;
		if(pst_SI->af_Middle[2] == SND_Cte_DefaultMiddle)
		{
			pst_SI->af_Middle[2] = (_f_Near + _f_Far) * 0.5f;
		}
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StopAll(int _i_Type)
{
	/*~~~~~~~~~~~~*/
	LONG	l_Index;
	int		i, j;
	/*~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(_i_Type == UNI_Cuc_InitInPause) return;

#ifdef ACTIVE_EDITORS
	LINK_SendMessageToEditors(ESON_MESSAGE_REINITENGINE, 0, 0);
#endif

	/*$2- stop FX ----------------------------------------------------------------------------------------------------*/

	for(i = 0; i < SND_Cte_FxCoreNb; i++)
	{
		SND_i_FxSeti(SND_Cte_Fx_iCoreId, i);
		for(j = 0; j < SND_Cte_FxContextNb; j++)
		{
			SND_i_FxSeti(SND_Cte_Fx_iContext, j);
			SND_i_FxSeti(SND_Cte_Fx_iEnable, 0);
		}
	}

	/*$2- stop sound -------------------------------------------------------------------------------------------------*/

	SND_ReinitWacInstance();
	for(l_Index = 0; l_Index < SND_gst_Params.l_InstanceNumber; l_Index++)
	{
		SND_Release(l_Index);
	}

	/*$2- kill track -------------------------------------------------------------------------------------------------*/

	SND_TrackFreeAll();

#ifdef SND_RASTER
	SND_gst_InstanceStat.ui_Current = 0;
	SND_gst_InstanceStat.ui_Max = 0;
	SND_gst_SInstanceStat.ui_Current = 0;
	SND_gst_SInstanceStat.ui_Max = 0;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Update(MATH_tdst_Matrix *_pst_Camera)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					l_Index;
	LONG					l_Status;
	static int				i_init = 0;
	static MATH_tdst_Vector st_last_mic_pos;
	MATH_tdst_Vector		st_micro_sp;
   	OBJ_tdst_GameObject     *pst_Father;
  	extern OBJ_tdst_GameObject *ANI_pst_GetReference(OBJ_tdst_GameObject *);
  	BOOL                    b_Active;
#ifdef JADEFUSION
	float                   fsave;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

#ifdef ACTIVE_EDITORS

	/*$1- edi pause ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(SND_gb_EdiPause) ESON_PauseAll(FALSE);
#endif

	/*$1- refresh ref ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_SetMicroPos(_pst_Camera);

	/*$1- compute micro speed ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(SND_gst_Params.pst_RefForVol)
	{
		if(i_init)
		{
			MATH_SubVector(&st_micro_sp, MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol), &st_last_mic_pos);
			MATH_CopyVector(&st_last_mic_pos, MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol));
			MATH_ScaleEqualVector(&st_micro_sp, 1.0f / TIM_gf_dt);
		}
		else
		{
			i_init = 1;
			MATH_CopyVector(&st_last_mic_pos, MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol));
			st_micro_sp.x = 0.0f;
			st_micro_sp.y = 0.0f;
			st_micro_sp.z = 0.0f;
		}
	}
	else
	{
		st_micro_sp.x = 0.0f;
		st_micro_sp.y = 0.0f;
		st_micro_sp.z = 0.0f;
	}

	/*$1- update loading effect ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_WacUpdate();
	SND_FxUpdate();
	SND_StreamPrefetchUpdate();
	SND_TrackUpdate();

#if _XENON_SOUND_ENGINE
	GetAudioEngine()->Tick();
#endif

	/*$1- update all sounds ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(l_Index = 0; l_Index < SND_gst_Params.l_InstanceNumber; l_Index++)
	{
		pst_SI = SND_gst_Params.dst_Instance + l_Index;
		if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;

		/*$2- play delay ---------------------------------------------------------------------------------------------*/

		if(pst_SI->f_Delay > 0.0f)
		{
			pst_SI->f_Delay -= TIM_gf_dt;
			if(pst_SI->f_Delay <= 0.0f)
			{
				/*~~~~~~~~~*/
				ULONG	mode;
				int		nb;
                SND_tdst_OneSound * pst_Sound;
				/*~~~~~~~~~*/
                
                pst_Sound = SND_gst_Params.dst_Sound + pst_SI->l_Sound;
                if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) && pst_Sound->pst_Wave)
                {
                    if((pst_Sound->ul_Flags & SND_Cte_StreamedFile) && pst_Sound->pst_Wave->b_HasLoop)
                    {
                        pst_SI->ul_Flags |= SND_Cul_OSF_Loop;
                        pst_SI->i_LoopNb = -1;
                    }
                }

                nb = (pst_SI->ul_Flags & SND_Cul_OSF_Loop) ? pst_SI->i_LoopNb : 0;
				mode = (pst_SI->ul_Flags & SND_Cul_OSF_Loop) ? SND_Cul_SBC_PlayLooping : 0;
				mode |= SND_Cul_SBC_PlayWithoutDelay;

                pst_SI->f_Delay = 0.0f;
                pst_SI->i_LoopNb  = 0;
                SND_PlayRq(pst_SI, mode, 0, -1, nb);
			}
		}
        else if(pst_SI->f_Delay < 0.0f)
        {
			pst_SI->f_Delay += TIM_gf_dt;
			if(pst_SI->f_Delay >= 0.0f)
			{
                pst_SI->f_Delay = 0.0f;
            }
        }


		/*$2- auto stop/destroy --------------------------------------------------------------------------------------*/

		else if
			(
				(pst_SI->ul_Flags & SND_Cul_DSF_DestroyWhenFinished)
			&&	(pst_SI->ul_Flags & SND_Cul_DSF_StartedOnce)
			&&	((pst_SI->ul_ExtFlags & SND_Cul_ESF_StopByFarTest) == 0)
			)
		{
#ifdef JADEFUSION	
			pst_SI->pst_LI->pfi_SndBuffGetStatus((SND_tdst_SoundBuffer_*)pst_SI->pst_DSB, (int*)&l_Status);
#else
			pst_SI->pst_LI->pfi_SndBuffGetStatus(pst_SI->pst_DSB, &l_Status);
#endif
			if((l_Status & SND_Cul_SBS_Playing) == 0)
			{
				_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_Update[autstop]);

				SND_FreeInstance(l_Index);

				continue;
			}
		}

		/*$2- inactive gao ----------------------------------------------------------------------------------------*/

	    b_Active = TRUE;
		if(pst_SI->p_GameObject)
		{
			if(  OBJ_b_TestControlFlag((OBJ_tdst_GameObject *) pst_SI->p_GameObject, OBJ_C_ControlFlag_ForceInactive) )
			{
                b_Active = FALSE;
    			continue;
			}
			
			if( OBJ_b_TestControlFlag((OBJ_tdst_GameObject *) pst_SI->p_GameObject, OBJ_C_ControlFlag_SectoInactive) )
			{
			    b_Active = FALSE;
        		continue;
			}
			
        	if(OBJ_b_TestIdentityFlag((OBJ_tdst_GameObject *) pst_SI->p_GameObject, OBJ_C_IdentityFlag_Bone))
            {
            	pst_Father= ANI_pst_GetReference((OBJ_tdst_GameObject *) pst_SI->p_GameObject);
                if(pst_Father && OBJ_b_TestControlFlag(pst_Father, OBJ_C_ControlFlag_SectoInactive))
                {
    			    b_Active = FALSE;
            		continue;
			    }
			
			}
		}

        if(b_Active)
            pst_SI->ul_Flags &= ~SND_Cul_InactiveInstance;
        else
            pst_SI->ul_Flags |= SND_Cul_InactiveInstance;
			
	

		/*$2- Extended update ----------------------------------------------------------------------------------------*/

		if(pst_SI->pst_SModifier)
		{
			SND_SModifierUpdateInstance(pst_SI);
		}

		/*$2- 3D volume/pan update -----------------------------------------------------------------------------------*/

		SND_SetInstVolume(pst_SI);

		if(pst_SI->ul_Flags & (SND_Cul_SF_Music)) continue;

		/*$2- 3D frequency update ------------------------------------------------------------------------------------*/

		if(pst_SI->f_Doppler)
		{
			pst_SI->pst_LI->pfv_ComputeDoppler
				(
					&st_micro_sp,
					MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol),
					pst_SI
				);
		}
	}

	/*$1- target updating ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_gpst_Interface[ISound].pfv_CommitChange(&st_micro_sp);
}


/*$4
 ***********************************************************************************************************************
    Sound parameters functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_AutoStop(SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~*/
	int tmp;
	/*~~~~*/

	if(!(pst_SI->ul_ExtFlags & SND_Cul_ESF_StopByFarTest))
	{
		pst_SI->pst_LI->pfi_SndBuffGetStatus(pst_SI->pst_DSB, &tmp);
		if(!(tmp & SND_Cul_SBS_Playing)) return;

		_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_AutoStop);

		pst_SI->ul_ExtFlags |= SND_Cul_ESF_StopByFarTest;
		SND_StopRq(SND_M_GetInstanceFromPtr(pst_SI));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_AutoPlay(SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~~~~*/
	ULONG	mode, nb;
	/*~~~~~~~~~~~~~*/

	if(pst_SI->ul_ExtFlags & SND_Cul_ESF_StopByFarTest)
	{
		_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_AutoPlay);

		mode = (pst_SI->ul_Flags & SND_Cul_OSF_Loop) ? SND_Cul_SBC_PlayLooping : 0;
		nb = (pst_SI->ul_Flags & SND_Cul_OSF_Loop) ? -1 : 0;
		pst_SI->ul_ExtFlags &= ~SND_Cul_ESF_StopByFarTest;
		SND_PlayRq(pst_SI, mode, 0, -1, nb);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetVol(LONG _l_Index, float _f_Volume)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_VolumeSaturation(_f_Volume);
	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	pst_SI->f_Volume = _f_Volume;
	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_SetVol);
	SND_SetInstVolume(pst_SI);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_GetVol(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, 0);
	return pst_SI->f_Volume;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetWetVol(LONG _l_Index, float _f_Volume)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_GetWetVol(LONG _l_Index)
{
	return 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetFreq(LONG _l_Index, LONG _l_Freq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef _FINAL_
	if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoFreq) return;
#endif
	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	pst_SI->pst_LI->pfv_SndBuffSetFreq(pst_SI->pst_DSB, _l_Freq);
	pst_SI->i_BaseFrequency = _l_Freq;
	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_SetFreq);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_GetFreq(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, 0);

	return pst_SI->i_BaseFrequency;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetPan(LONG _l_Index, LONG _l_Pan, LONG _l_FrontRear)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	pst_SI->pst_LI->pfv_SndBuffSetPan(pst_SI->pst_DSB, _l_Pan, _l_FrontRear);
	pst_SI->i_Pan = _l_Pan;
	pst_SI->i_Front = _l_FrontRear;
	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_SetPan);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_GetPan(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					l_Pan, l_FrontRear;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, 0);
#ifdef JADEFUSION	
	pst_SI->pst_LI->pfv_SndBuffGetPan((SND_tdst_SoundBuffer_*)pst_SI->pst_DSB, (int*)&l_Pan, (int*)&l_FrontRear);
#else
	pst_SI->pst_LI->pfv_SndBuffGetPan(pst_SI->pst_DSB, &l_Pan, &l_FrontRear);
#endif
	return l_Pan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_GetFront(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					l_Pan, l_FrontRear;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, 0);
#ifdef JADEFUSION
	pst_SI->pst_LI->pfv_SndBuffGetPan((SND_tdst_SoundBuffer_*)pst_SI->pst_DSB, (int*)&l_Pan, (int*)&l_FrontRear);
#else
	pst_SI->pst_LI->pfv_SndBuffGetPan(pst_SI->pst_DSB, &l_Pan, &l_FrontRear);
#endif
	return l_FrontRear;
}

/*$4
 ***********************************************************************************************************************
    Play / stop functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    tries to play a sound. If request fails, it will look for an old sound to replace it.
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
void SND_PlayRq
#else
static void SND_PlayRq
#endif
(
	SND_tdst_SoundInstance	*_pst_SI,
	int						_i_Flag,
	int						_i_StartPosition,
	int						_i_StopPosition,
	int						_i_LoopCount
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						l_Index, l_Status;
	SND_tdst_SoundInstance	*pst_localSI;
	SND_tdst_SoundInstance	*pst_OldestSI;
	float					f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- flags ------------------------------------------------------------------------------------------------------*/

	if(_pst_SI->ul_Flags & SND_Cul_SF_FxA) _i_Flag |= SND_Cul_SBC_PlayFxA;
	if(_pst_SI->ul_Flags & SND_Cul_SF_FxB) _i_Flag |= SND_Cul_SBC_PlayFxB;

	_pst_SI->ul_ExtFlags &= ~(SND_Cul_ESF_StopRq | SND_Cul_ESF_StopByFarTest);

	/*$2- start pos --------------------------------------------------------------------------------------------------*/

	if(_pst_SI->ul_ExtFlags & SND_Cul_ESF_RandomPlay)
	{
		if(_pst_SI->i_BaseFrequency)
		{
			// get samples
            f = fLongToFloat(SND_ui_SizeToSample
                (
                    SND_gst_Params.dst_Sound[_pst_SI->l_Sound].pst_Wave->wFormatTag, 
                    SND_gst_Params.dst_Sound[_pst_SI->l_Sound].pst_Wave->wChannels, 
                    SND_gst_Params.dst_Sound[_pst_SI->l_Sound].pst_Wave->ul_DataSize
                ));

            // get duration
            f = f / fLongToFloat(_pst_SI->i_BaseFrequency);

	    	// get a random date
            _pst_SI->f_StartDate = fRand(0.0f, f);
    	}
	}

	if(!_i_StartPosition && _pst_SI->f_StartDate)
	{
        // get sample
        f = _pst_SI->f_StartDate * fLongToFloat(_pst_SI->i_BaseFrequency);
        
        // get size
        _i_StartPosition = SND_ui_SampleToSize
                (
                    SND_gst_Params.dst_Sound[_pst_SI->l_Sound].pst_Wave->wFormatTag, 
                    SND_gst_Params.dst_Sound[_pst_SI->l_Sound].pst_Wave->wChannels, 
                    lFloatToLong(f)
                );      

        if( (ULONG)_i_StartPosition > SND_gst_Params.dst_Sound[_pst_SI->l_Sound].pst_Wave->ul_DataSize)
            _i_StartPosition = 0;
	}

	/*$2- smodifier --------------------------------------------------------------------------------------------------*/

	if(SND_i_SModifierNotifyPlayInstance(_pst_SI)) return;

/*$2- check delay ----------------------------------------------------------------------------------------------------*/

#ifdef ACTIVE_EDITORS
	if(!ENG_gb_EngineRunning) 
    {
        _pst_SI->f_Delay = 0.0f;
        _pst_SI->i_LoopNb = 0;
    }
#endif
	if(_i_Flag & SND_Cul_SBC_PlayWithoutDelay)
    {
		_pst_SI->f_Delay = 0.0f;
    }
    else if(_pst_SI->f_Delay)
		return;

	/*$2- on track ---------------------------------------------------------------------------------------------------*/

	_SND_DebugSpy(SND_gst_Params.dst_Sound[_pst_SI->l_Sound].ul_FileKey, SND_PlayRq);

	if
	(
		(_pst_SI->i_InstTrack != -1)
	||	(_pst_SI->ul_Flags & (SND_Cul_SF_Dialog | SND_Cul_SF_Ambience | SND_Cul_SF_Music))
	)
	{
		_pst_SI->f_PlayingStartDate = TIM_f_Clock_Read();
		SND_TrackAttachAndPlayInstance
		(
			SND_M_GetInstanceFromPtr(_pst_SI),
			_i_Flag,
			_i_LoopCount,
			_i_StartPosition,
			_i_StopPosition
		);
		return;
	}

	pst_OldestSI = NULL;
	_pst_SI->f_PlayingStartDate = TIM_f_Clock_Read();
	_pst_SI->i_InstTrack = -1;

	/*$2- normal playing ---------------------------------------------------------------------------------------------*/

	if
	(
		_pst_SI->pst_LI->pfi_SndBuffPlay
			(
				_pst_SI->pst_DSB,
				_i_Flag,
				SND_l_GetAttFromVol(_pst_SI->f_FxVolLeft),
				SND_l_GetAttFromVol(_pst_SI->f_FxVolRight)
			) < 0
	)
	{

		/*$1- looking for an old sound (not a music, nor loop, nor stream) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(l_Index = 0; l_Index < SND_gst_Params.l_InstanceNumber; l_Index++)
		{
			pst_localSI = SND_gst_Params.dst_Instance + l_Index;

			if(!(pst_localSI->ul_Flags & SND_Cul_DSF_Used)) continue;
			if(pst_localSI->ul_Flags & SND_Cul_SF_Music) continue;
			if(pst_localSI->ul_Flags & SND_Cul_SF_Ambience) continue;
			if(pst_localSI->ul_Flags & SND_Cul_SF_Dialog) continue;

			pst_localSI->pst_LI->pfi_SndBuffGetStatus(pst_localSI->pst_DSB, &l_Status);
			if(l_Status & SND_Cul_SBS_PlayingLoop) continue;

			if(l_Status & SND_Cul_SBS_Playing)
			{
				if(!pst_OldestSI)
				{
					pst_OldestSI = pst_localSI;
					continue;
				}

				if(pst_localSI->f_PlayingStartDate < pst_OldestSI->f_PlayingStartDate) pst_OldestSI = pst_localSI;
			}
		}

		if(!pst_OldestSI)
		{

			/*$1- 2nd chance : looking for an old sound (not a music) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			for(l_Index = 0; l_Index < SND_gst_Params.l_InstanceNumber; l_Index++)
			{
				pst_localSI = SND_gst_Params.dst_Instance + l_Index;

				if(!(pst_localSI->ul_Flags & SND_Cul_DSF_Used)) continue;
				if(pst_localSI->ul_Flags & SND_Cul_SF_Music) continue;
				if(pst_localSI->ul_Flags & SND_Cul_SF_Ambience) continue;
				if(pst_localSI->ul_Flags & SND_Cul_SF_Dialog) continue;

				pst_localSI->pst_LI->pfi_SndBuffGetStatus(pst_localSI->pst_DSB, &l_Status);
				if(l_Status & SND_Cul_SBS_Playing)
				{
					if(!pst_OldestSI)
					{
						pst_OldestSI = pst_localSI;
						continue;
					}

					if(pst_localSI->f_PlayingStartDate < pst_OldestSI->f_PlayingStartDate) pst_OldestSI = pst_localSI;
				}
			}
		}

		/*$1- so bad can not play ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ERR_X_Warning(pst_OldestSI, "[SND] No more ressources for playing one sound !", NULL);
		if(!pst_OldestSI) return;

		/*$1- stop the oldest ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_OldestSI->pst_LI->pfv_SndBuffStop(pst_OldestSI->pst_DSB);
		pst_OldestSI->ul_Flags &= ~(SND_Cul_SF_PlayingLoop | SND_Cul_SF_Playing);

		_pst_SI->f_PlayingStartDate = TIM_f_Clock_Read();
		l_Index = _pst_SI->pst_LI->pfi_SndBuffPlay
			(
				_pst_SI->pst_DSB,
				_i_Flag,
				SND_l_GetAttFromVol(_pst_SI->f_FxVolLeft),
				SND_l_GetAttFromVol(_pst_SI->f_FxVolRight)
			);

#ifndef _GAMECUBE
		ERR_X_Warning((l_Index == 0), "One sound can't be played (SND_PlayRq)", NULL);
#endif
		if(l_Index < 0) return;
	}

	/*$2- regiter state ----------------------------------------------------------------------------------------------*/

	if(_i_Flag & SND_Cul_SBC_PlayLooping)
		_pst_SI->ul_Flags |= SND_Cul_SF_PlayingLoop;
	else
		_pst_SI->ul_Flags |= SND_Cul_SF_Playing;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_PlaySound(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance			*pst_SI;
    SND_tdst_OneSound				*pst_Sound;
	LONG							l_Status;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
    SND_M_GetSoundOrReturn(pst_SI->l_Sound, pst_Sound, ;);
              
    if(!pst_Sound->pst_Wave) return;
       
    if((pst_Sound->ul_Flags & SND_Cte_StreamedFile) && pst_Sound->pst_Wave->b_HasLoop)
    {
        SND_PlaySoundLooping(_l_Index, -1);
        return;    
    }

	if(!(pst_SI->ul_ExtFlags & SND_Cul_ESF_StopRq))
	{
#ifdef JADEFUSION
		pst_SI->pst_LI->pfi_SndBuffGetStatus((SND_tdst_SoundBuffer_*)pst_SI->pst_DSB, (int*)&l_Status);
#else
		pst_SI->pst_LI->pfi_SndBuffGetStatus(pst_SI->pst_DSB, &l_Status);
#endif
		if(l_Status & SND_Cul_SBS_Playing) return;

	}
	
	pst_SI->ul_Flags |= SND_Cul_DSF_StartedOnce;
	pst_SI->ul_Flags &= ~SND_Cul_OSF_Loop;

#ifdef ACTIVE_EDITORS
	if(!ENG_gb_EngineRunning) 
    {
        pst_SI->f_Delay = 0.0f;
        pst_SI->i_LoopNb = 0;
    }
#endif
	if(pst_SI->f_Delay) return;

	l_Status = 1;
	if(pst_SI->ul_Flags & SND_Cul_SF_MaskDynVol)
		if(!SND_b_IsInActiveVolume(_l_Index)) 
		{
			l_Status = 0;
		}

	if((pst_SI->i_InstTrack != -1) || (l_Status))
	{
	    _SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_PlaySound);
	    SND_SetInstVolume(pst_SI);
	    SND_PlayRq(pst_SI, 0, 0, -1, 0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_PlaySoundInVolume(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance			*pst_SI;
    SND_tdst_OneSound				*pst_Sound;
	LONG							l_Status;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
    SND_M_GetSoundOrReturn(pst_SI->l_Sound, pst_Sound, ;);
              
    if(!pst_Sound->pst_Wave) return;
       
    if((pst_Sound->ul_Flags & SND_Cte_StreamedFile) && pst_Sound->pst_Wave->b_HasLoop)
    {
        SND_PlaySoundLooping(_l_Index, -1);
        return;    
    }

	if(!(pst_SI->ul_ExtFlags & SND_Cul_ESF_StopRq))
	{
#ifdef JADEFUSION
		pst_SI->pst_LI->pfi_SndBuffGetStatus((SND_tdst_SoundBuffer_*)pst_SI->pst_DSB, (int*)&l_Status);
#else
		pst_SI->pst_LI->pfi_SndBuffGetStatus(pst_SI->pst_DSB, &l_Status);
#endif
		if(l_Status & SND_Cul_SBS_Playing) return;

	}
	
	
	/* If it's an already played non-looped sound, don't play it again */
	l_Status = 1;
	if (!pst_Sound->pst_Wave->b_HasLoop && (pst_SI->ul_ExtFlags & SND_Cul_ESF_StartedOnceInVol))
		l_Status = 0;

	pst_SI->ul_ExtFlags |= SND_Cul_ESF_StartedOnceInVol;
	//pst_SI->ul_Flags |= SND_Cul_DSF_StartedOnce;
	pst_SI->ul_Flags &= ~SND_Cul_OSF_Loop;

#ifdef ACTIVE_EDITORS
	if(!ENG_gb_EngineRunning) 
    {
        pst_SI->f_Delay = 0.0f;
        pst_SI->i_LoopNb = 0;
    }
#endif
	if(pst_SI->f_Delay) return;

	//l_Status = 1;
	if(pst_SI->ul_Flags & SND_Cul_SF_MaskDynVol)
		if(!SND_b_IsInActiveVolume(_l_Index)) 
		{
			l_Status = 0;
			pst_SI->ul_ExtFlags &= ~SND_Cul_ESF_StartedOnceInVol;
		}

	if((pst_SI->i_InstTrack != -1) || (l_Status))
	{
	    _SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_PlaySound);
	    SND_SetInstVolume(pst_SI);
	    SND_PlayRq(pst_SI, 0, 0, -1, 0);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ReinitMdFPlayParam(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance			*pst_SI;
    SND_tdst_OneSound				*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
    SND_M_GetSoundOrReturn(pst_SI->l_Sound, pst_Sound, ;);

	if((pst_Sound->ul_Flags & SND_Cte_StreamedFile) && pst_Sound->pst_Wave->b_HasLoop)
		return;

	pst_SI->ul_ExtFlags &= ~SND_Cul_ESF_StartedOnceInVol;
}
	

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_PlaySoundLooping(LONG _l_Index, LONG _l_LoopCount)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	SND_tdst_OneSound       *pst_Sound;
	LONG					l_Status;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
    SND_M_GetSoundOrReturn(pst_SI->l_Sound, pst_Sound, ;);
              
    if(!pst_Sound->pst_Wave) return;
       
    if((pst_Sound->ul_Flags & SND_Cte_StreamedFile) && !pst_Sound->pst_Wave->b_HasLoop)
    {
        SND_PlaySound(_l_Index);
        return;    
    }

    _l_LoopCount = -1;
	pst_SI->ul_Flags |= SND_Cul_DSF_StartedOnce;
	pst_SI->ul_Flags |= SND_Cul_OSF_Loop;
	//pst_SI->ul_Flags &= ~SND_Cul_DSF_DestroyWhenFinished;
	if(!(pst_SI->ul_ExtFlags & SND_Cul_ESF_StopRq))
	{
#ifdef JADEFUSION
		pst_SI->pst_LI->pfi_SndBuffGetStatus((SND_tdst_SoundBuffer_*)pst_SI->pst_DSB, (int*)&l_Status);
#else
		pst_SI->pst_LI->pfi_SndBuffGetStatus(pst_SI->pst_DSB, &l_Status);
#endif
		if(l_Status & SND_Cul_SBS_Playing) return;
	}

	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_PlaySoundLooping);
	SND_SetInstVolume(pst_SI);
	SND_PlayRq(pst_SI, SND_Cul_SBC_PlayLooping, 0, -1, _l_LoopCount);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_PlaySoundLoopingFrom(LONG _l_Index, LONG _l_LoopCount, unsigned int uiPos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					l_Status;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);

	if(_l_LoopCount)
		pst_SI->ul_Flags |= SND_Cul_OSF_Loop;
	else
		pst_SI->ul_Flags &= ~SND_Cul_OSF_Loop;

	pst_SI->ul_Flags |= SND_Cul_DSF_StartedOnce;
	//pst_SI->ul_Flags &= ~SND_Cul_DSF_DestroyWhenFinished;
	if(!(pst_SI->ul_ExtFlags & SND_Cul_ESF_StopRq))
	{
#ifdef JADEFUSION
		pst_SI->pst_LI->pfi_SndBuffGetStatus((SND_tdst_SoundBuffer_*)pst_SI->pst_DSB, (int*)&l_Status);
#else
		pst_SI->pst_LI->pfi_SndBuffGetStatus(pst_SI->pst_DSB, &l_Status);
#endif
		if(l_Status & SND_Cul_SBS_Playing) return;
	}

	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_PlaySoundLooping);
	SND_SetInstVolume(pst_SI);
	SND_PlayRq(pst_SI, SND_Cul_SBC_PlayLooping, uiPos, -1, _l_LoopCount);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_StopRq(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_StopRq);

	if(pst_SI->ul_ExtFlags & SND_Cul_ESF_StopRq) return;

	if(SND_i_SModifierNotifyStopInstance(pst_SI) == 0)
	{
		SND_Stop(_l_Index);
	}
	else
	{
		pst_SI->ul_ExtFlags |= SND_Cul_ESF_StopRq;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Stop(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_Stop);

	pst_SI->pst_LI->pfv_SndBuffStop(pst_SI->pst_DSB);
	pst_SI->ul_Flags &= ~(SND_Cul_SF_PlayingLoop | SND_Cul_SF_Playing);

	if(pst_SI->i_InstTrack != -1) SND_TrackDetachInstance(_l_Index);
	pst_SI->pst_LI->pfv_SndBuffSetPos(pst_SI->pst_DSB, 0);
	pst_SI->f_Delay = 0.0f;
	pst_SI->i_LoopNb= 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Pause(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);

	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_Pause);

	SND_i_SModifierNotifyPauseInstance(pst_SI);
	pst_SI->pst_LI->pfv_SndBuffPause(pst_SI->pst_DSB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_IsPlaying(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					l_Status;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, 0);

#ifdef ACTIVE_EDITORS
	if(!ENG_gb_EngineRunning) 
    {
        pst_SI->f_Delay = 0.0f;
        pst_SI->i_LoopNb = 0;
    }
#endif
	if(pst_SI->f_Delay)
	{
		pst_SI->ul_Flags |= SND_Cul_DSF_StartedOnce;
		return 1;
	}
#ifdef JADEFUSION
	pst_SI->pst_LI->pfi_SndBuffGetStatus((SND_tdst_SoundBuffer_*)pst_SI->pst_DSB, (int*)&l_Status);
#else
	pst_SI->pst_LI->pfi_SndBuffGetStatus(pst_SI->pst_DSB, &l_Status);
#endif
	if(l_Status & SND_Cul_SBS_Playing)
	{
		if(pst_SI->ul_Flags & SND_Cul_SF_PlayingLoop)
			return 2;
		else
			return 1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_l_ActiveInstanceInsert(LONG _l_Index, LONG _idx, BOOL _b_Active, BOOL _b_Reinit)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, -1);
	return SND_i_SModifierActiveInsert(pst_SI, _idx, _b_Active, _b_Reinit);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_Duplicate(OBJ_tdst_GameObject *_pst_Src, OBJ_tdst_GameObject *_pst_Dest, BOOL _b_CreateFile)
{
	if(!SND_M_GoodPointer(_pst_Src)) return;
	if(!SND_M_GoodPointer(_pst_Src->pst_Extended)) return;
    if(!SND_M_GoodPointer(_pst_Src->pst_Extended->pst_Sound)) return;

	if(!SND_M_GoodPointer(_pst_Dest)) return;

	if(!OBJ_b_TestIdentityFlag(_pst_Dest, OBJ_C_IdentityFlag_Sound))
	    OBJ_SetIdentityFlag(_pst_Dest, OBJ_C_IdentityFlag_Sound);
	
	OBJ_GameObject_CreateExtendedIfNot(_pst_Dest);

	_pst_Dest->pst_Extended->pst_Sound = _pst_Src->pst_Extended->pst_Sound;
	_pst_Src->pst_Extended->pst_Sound->st_Bank.ul_UserCount++;
}

/*
 =======================================================================================================================
    compute doppler effect from one sound source (instance) called once by frame by instance
 =======================================================================================================================
 */
static void SND_ComputeDoppler
(
	MATH_tdst_Vector		*_pst_micro_speed,
	MATH_tdst_Vector		*_pst_micro_pos,
	SND_tdst_SoundInstance	*_pst_SI
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_src_speed;
	MATH_tdst_Vector	st_dist;
	MATH_tdst_Vector	st_Tmp;
	OBJ_tdst_GameObject *pst_GO_src;
	float				f_dist;
	float				f_doppler;
	float				f_freq;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SI->pst_GlobalMatrix && _pst_SI->p_GameObject)
	{
		pst_GO_src = (OBJ_tdst_GameObject *) _pst_SI->p_GameObject;
		if(!SND_M_GoodPointer(pst_GO_src)) return;
		if(!OBJ_b_TestIdentityFlag(pst_GO_src, OBJ_C_IdentityFlag_Sound)) return;
		if(!SND_M_GoodPointer(pst_GO_src->pst_Extended)) return;
		if(!SND_M_GoodPointer(pst_GO_src->pst_Extended->pst_Sound)) return;
		_pst_SI->pst_GlobalMatrix = pst_GO_src->pst_GlobalMatrix;
	}

	if(_pst_SI->pst_GlobalMatrix)
	{
		pst_GO_src = (OBJ_tdst_GameObject *) _pst_SI->p_GameObject;
		if(!SND_M_GoodPointer(pst_GO_src)) return;
		if(!OBJ_b_TestIdentityFlag(pst_GO_src, OBJ_C_IdentityFlag_Sound)) return;
		if(!SND_M_GoodPointer(pst_GO_src->pst_Extended)) return;
		if(!SND_M_GoodPointer(pst_GO_src->pst_Extended->pst_Sound)) return;

		/* distance */
		MATH_SubVector(&st_dist, MATH_pst_GetTranslation(pst_GO_src->pst_GlobalMatrix), _pst_micro_pos);
		f_dist = MATH_f_NormVector(&st_dist);
		if(f_dist < 0.00001f) return;

		/* src speed */
		if(OBJ_pst_GetDynaStruct(pst_GO_src))
		{
			DYN_GetSpeedVector(OBJ_pst_GetDynaStruct(pst_GO_src), &st_src_speed);
		}
		else
		{
			MATH_InitVector(&st_src_speed, 0.0f, 0.0f, 0.0f);
		}
	}
	else
	{
		/* check consistency */
		if(_pst_SI->pst_GlobalPos != &_pst_SI->st_UpdatedPos) return;

		/* eval the speed */
		MATH_SubVector(&st_src_speed, &_pst_SI->st_UpdatedPos, &_pst_SI->st_LastUpdatedPos);
		MATH_MulEqualVector(&st_src_speed, 1.0f / TIM_gf_dt);
		MATH_CopyVector(&_pst_SI->st_LastUpdatedPos, &_pst_SI->st_UpdatedPos);

		MATH_SubVector(&st_dist, &_pst_SI->st_UpdatedPos, _pst_micro_pos);
		f_dist = MATH_f_NormVector(&st_dist);
		if(f_dist < 0.00001f) return;
	}

	/* doppler coeff : freq = 1 + (Vm-Vb).D / (VsD - Vm.Vb) */
	f_doppler = SND_gst_Params.f_DopplerSoundSpeed * f_dist - MATH_f_DotProduct(_pst_micro_speed, &st_dist);
	if(f_doppler)
	{
		MATH_SubVector(&st_Tmp, _pst_micro_speed, &st_src_speed);
		f_doppler = SND_gst_Params.f_DopplerFactor * MATH_f_DotProduct(&st_Tmp, &st_dist) / f_doppler;
		f_doppler *= _pst_SI->f_Doppler;

		/* saturation */
		if(f_doppler < -1.0f) f_doppler = -0.999f;
		if(f_doppler > 1000.0f) f_doppler = 1000.0f;

		/* new freq */
		f_freq = fLongToFloat(_pst_SI->i_BaseFrequency) * (1.0f + f_doppler);
		_pst_SI->pst_LI->pfv_SndBuffSetFreq(_pst_SI->pst_DSB, lFloatToLong(f_freq));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_Null(void *vv)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_CheckOrInitGAO(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdun_Main	*pst_Sound;
	extern char		MEM_b_CheckBlockHasBeenFreed(void *);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO) return 1;
	if((int) _pst_GO & 0x3) return 1;

	if(MEM_b_CheckBlockHasBeenFreed(_pst_GO))
	{
		return 1;
	}

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Sound))
	{
		return 1;
	}

	OBJ_GameObject_CreateExtendedIfNot(_pst_GO);

	pst_Sound = (SND_tdun_Main *) MEM_p_Alloc(sizeof(SND_tdun_Main));
	L_memset(pst_Sound, 0, sizeof(SND_tdun_Main));

	pst_Sound->st_Bank.ul_UserCount = 1;
	pst_Sound->st_Bank.ul_FileKey = -1;

	_pst_GO->pst_Extended->pst_Sound = pst_Sound;
	_pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_Sound;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_FreeInstance(LONG _l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SI = &SND_gst_Params.dst_Instance[_l_Index];
	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_FreeInstance);

#ifdef SND_RASTER
	if(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_Flags & SND_Cte_StreamedFile)
		SND_StatSubSInstance();
	else
		SND_StatSubInstance();
#endif /* SND_RASTER */

	if(pst_SI->i_InstTrack != -1) SND_TrackDetachInstance(_l_Index);
	
	if(pst_SI->ul_Flags & SND_Cul_DSF_Instance)
	{
		_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_FreeInstance[release]);
		pst_SI->pst_LI->pfv_SndBuffStop(pst_SI->pst_DSB);
		pst_SI->pst_LI->pfv_SndBuffRelease(pst_SI->pst_DSB);
	}
	else
	{
		pst_SI->pst_LI->pfv_SndBuffStop(pst_SI->pst_DSB);
		SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_Flags &= ~SND_Cul_DSF_Instance;
	}

	SND_SModifierDeleteAll(&pst_SI->pst_SModifier, TRUE);
    SND_InsertDestroyInstanceCallback(_l_Index);

	pst_SI->l_Sound = -1;
	pst_SI->ul_Flags = 0;
	pst_SI->ul_ExtFlags = 0;
	pst_SI->i_InstTrack = -1;
	pst_SI->i_BaseFrequency = 0;
	pst_SI->f_Volume = 0.0f;
	pst_SI->i_Front = 0;
	pst_SI->i_Pan = 0;
	pst_SI->f_MinPan = 0.0f;
	pst_SI->af_Near[0] = 0.0f;
	pst_SI->af_Near[1] = 0.0f;
	pst_SI->af_Near[2] = 0.0f;
	pst_SI->af_Far[0] = 0.0f;
	pst_SI->af_Far[1] = 0.0f;
	pst_SI->af_Far[2] = 0.0f;
	pst_SI->f_DeltaFar = 0.0f;
	pst_SI->af_Middle[0] = 0.0f;
	pst_SI->af_Middle[1] = 0.0f;
	pst_SI->af_Middle[2] = 0.0f;
	pst_SI->f_FarCoeff = 0.0f;
	pst_SI->f_MiddleCoeff = 0.0f;
	pst_SI->f_CylinderHeight = 0.0f;
	pst_SI->pst_GlobalMatrix = NULL;
	pst_SI->pst_GlobalPos = NULL;
	MATH_InitVector(&pst_SI->st_LastUpdatedPos, 0.0f, 0.0f, 0.0f);
	MATH_InitVector(&pst_SI->st_UpdatedPos, 0.0f, 0.0f, 0.0f);
	pst_SI->pst_DSB = NULL;
	pst_SI->f_PlayingStartDate = 0.0f;
	pst_SI->p_GameObject = NULL;
	pst_SI->pst_LI = NULL;
	pst_SI->i_GroupId = 0;
	pst_SI->pst_SModifier = NULL;
	pst_SI->f_Delay = 0.0f;
	pst_SI->i_LoopNb= 0;
	pst_SI->f_StartDate = 0.0f;
	pst_SI->f_Doppler = 0.0f;
	pst_SI->f_FxVolLeft = 0.0f;
	pst_SI->f_FxVolRight = 0.0f;

#ifdef ACTIVE_EDITORS
	ESON_NotifyInstanceIsDeleted((ULONG) _l_Index);
	pst_SI->ul_EdiFlags = 0;
	pst_SI->ul_SmdKey = -1;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_AllocSound(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					l_Index;
	SND_tdst_OneSound	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(l_Index = 0; l_Index < SND_gst_Params.l_SoundNumber; l_Index++)
	{
		pst_Sound = SND_gst_Params.dst_Sound + l_Index;
		if(!(pst_Sound->ul_Flags & SND_Cul_DSF_Used)) return l_Index;
	}

	SND_EdiRedWarningMsg("SND_i_AllocSound", "Maximum of sound is reached");
	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gi_1=3;

void SND_RegisterInactiveGAO(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	SND_tdst_SoundInstance	*pst_SI;
	float					f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	if(!_pst_GO) return;
	if(!_pst_GO->pst_Extended) return;
	if(!_pst_GO->pst_Extended->pst_Sound) return;
	
    if(gi_1 & 1)
    {
        
    
	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;

		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;
		if(!pst_SI->p_GameObject) continue;
		if((int) pst_SI->p_GameObject != (int) _pst_GO) continue;

		f = pst_SI->f_Volume;
		pst_SI->f_Volume = 0.0f;
		SND_SetInstVolume(pst_SI);
		pst_SI->f_Volume = f;
	}
    }

    if(gi_1 & 2)
    {
        
    if(
        OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims) &&
        _pst_GO->pst_Base && 
        _pst_GO->pst_Base->pst_GameObjectAnim  && 
        _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton &&
        _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects
        )
    {
        TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
        OBJ_tdst_GameObject     *pst_BoneGO;

        pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
        pst_EndBone = TAB_pst_PFtable_GetLastElem(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
        for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
	    {
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
			ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));
            SND_RegisterInactiveGAO(pst_BoneGO);
        }
    }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_DestroyInstanceOfGao(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	SND_tdst_SoundInstance	*pst_SI;
	MDF_tdst_Modifier		*pst_MdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO) return;
	if(!SND_gst_Params.l_Available) return;

	/*$1- search MdF ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GO->pst_Extended)
	{
		pst_MdF = _pst_GO->pst_Extended->pst_Modifiers;
		while(pst_MdF)
		{
			switch(pst_MdF->i->ul_Type)
			{
			case MDF_C_Modifier_Sound:
			case MDF_C_Modifier_SoundFx:
			case MDF_C_Modifier_SoundLoading:
				pst_MdF->i->pfnv_Reinit(pst_MdF);
				break;

			default:
				break;
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}

	/*$1- search linked instances ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;

		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;
		if(!pst_SI->p_GameObject) continue;
		if((int) pst_SI->p_GameObject != (int) _pst_GO) continue;

		SND_Release(i);
	}
    
#ifdef ACTIVE_EDITORS
	ESON_NotifyGaoIsDeleted(_pst_GO);
#endif

    if(
        OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims) &&
        _pst_GO->pst_Base && 
        _pst_GO->pst_Base->pst_GameObjectAnim  && 
        _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton &&
        _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects
        )
    {
        TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
        OBJ_tdst_GameObject     *pst_BoneGO;

        pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
        pst_EndBone = TAB_pst_PFtable_GetLastElem(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
        for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
	    {
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
			ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));
            SND_DestroyInstanceOfGao(pst_BoneGO);
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_SetMicroPos(MATH_tdst_Matrix *_pst_Camera)
{
	if(SND_gpst_RqMicroPosFromAI && SND_gpst_RqMicroPosFromAI->pst_GlobalMatrix)
	{

		/*$2- get micro pos from IA ----------------------------------------------------------------------------------*/

		SND_gst_Params.pst_RefForVol = SND_gpst_RqMicroPosFromAI->pst_GlobalMatrix;
		SND_gst_Params.ul_Flags &= ~SND_Cte_Freeze3DVol;
#ifdef ACTIVE_EDITORS
		SND_gst_Params.pst_EdiMicroGao = SND_gpst_RqMicroPosFromAI;
#endif
	}
	else if(SND_gst_Params.ul_Flags & SND_Cte_Freeze3DVol)
	{

		/*$2- else is it frozen ? ------------------------------------------------------------------------------------*/

		SND_gst_Params.pst_RefForVol = &SND_gst_Params.st_FreezeRefForVol;
	}
	else if(AI_gpst_MainActors[0])
	{

		/*$2- else get main actor ------------------------------------------------------------------------------------*/

		SND_gst_Params.pst_RefForVol = AI_gpst_MainActors[0]->pst_GlobalMatrix;
#ifdef ACTIVE_EDITORS
		SND_gst_Params.pst_EdiMicroGao = AI_gpst_MainActors[0];
#endif
	}
	else
	{

		/*$2- else get camera ----------------------------------------------------------------------------------------*/

		SND_gst_Params.pst_RefForVol = _pst_Camera;
#ifdef ACTIVE_EDITORS
		SND_gst_Params.pst_EdiMicroGao = NULL;
#endif
	}

	/*$2- reset AI Rq ------------------------------------------------------------------------------------------------*/

	SND_gpst_RqMicroPosFromAI = NULL;

	/*$2- ref for pan ------------------------------------------------------------------------------------------------*/

	if(SND_gst_Params.ul_Flags & SND_Cte_FreezeDynPan)
	{
		SND_gst_Params.pst_RefForPan = &SND_gst_Params.st_FreezeRefForPan;
	}
	else
	{
		SND_gst_Params.pst_RefForPan = _pst_Camera;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_ChangeRenderMode(int _i_Mode)
{
	if(_i_Mode == -1) return (int) SND_gst_Params.ul_RenderMode;	/* to know the current mode */

	/* mode is compatible */
	if((_i_Mode & (int) SND_gst_Params.ul_RenderModeCapacity) != _i_Mode) return -1;

	/* mode already set */
	if(_i_Mode == (int) SND_gst_Params.ul_RenderMode) return 0;

	/* else do */
	switch(_i_Mode)
	{
    case SND_Cte_RenderCapacity:
        return (int) SND_gst_Params.ul_RenderModeCapacity;

	case SND_Cte_RenderCurrent:
		return (int) SND_gst_Params.ul_RenderMode;					/* to know the current mode */

	case SND_Cte_RenderMono:
	case SND_Cte_RenderStereo:
	case SND_Cte_RenderHeadPhone:
	case SND_Cte_RenderDolbyPrologic:
	case SND_Cte_RenderDolbyPrologicII:
	case SND_Cte_RenderDolbyDigital:
	case SND_Cte_RenderUseConsoleSettings:
		return SND_gpst_Interface[ISound].pfi_ChangeRenderMode(_i_Mode);

	default:
		return -2;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_ReinitOneWorld(int _i_Type)
{
	ESON_ReinitOneWorld();
	SND_StopAll(_i_Type);
    SND_StreamStopAll();
	SND_StreamPrefetchFlushAll();
	SND_StreamResolveSyncPrefetch();
    SND_i_FxSeti(SND_Cte_Fx_iReinit, 1);
    SND_ReinitGroupRequest();
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
