/*$T gcSND.c GC! 1.081 02/03/03 17:13:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE
#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Headers
 ***********************************************************************************************************************
 */

#include <dolphin/mix.h>
#include <dolphin/ax.h>

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"

#include "BIGfiles/BIGopen.h"

#include "GameCube/GC_File.h"
#include "GameCube/GC_Stream.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/gc/gcSND.h"
#include "SouND/Sources/gc/gcSND_Debug.h"
#include "SouND/Sources/gc/gcSND_ARAM.h"
#include "SouND/Sources/gc/gcSND_AudioFrame.h"
#include "SouND/Sources/gc/gcSND_Soft.h"
#include "SouND/Sources/gc/gcSND_Stream.h"
#include "SouND/Sources/gc/gcSND_Profile.h"
#include "SouND/Sources/SNDvolume.h"

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */
 
#define Cte_Bias                  (-60)
int		gcSND_gi_MasterVolume;


char					*gcSND_gp_BufferForLoading;
extern gcSND_tdst_Stream*gcSND_p_GetStreamFromSoftBuffer(gcSND_tdst_SoftBuffer *_pst_Soft);
void 	gcSND_RamLoadingCallBack(s32 result, DVDFileInfo *fileInfo, unsigned int context);


/*$off*/
int gcSNDPanTableL[128] =
{
	0,		0,		-1,		-1,		-1,		-2,		-2,		-2,		-3,		-3,
	-4,		-4,		-4,		-5,		-5,		-5,		-6,		-6,		-7,		-7,
	-7,		-8,		-8,		-9,		-9,		-10,	-10,	-10,	-11,	-11,
	-12,	-12,	-13,	-13,	-14,	-14,	-14,	-15,	-15,	-16,
	-16,	-17,	-17,	-18,	-18,	-19,	-20,	-20,	-21,	-21,
	-22,	-22,	-23,	-23,	-24,	-25,	-25,	-26,	-26,	-27,
	-28,	-28,	-29,	-30,	-30,	-31,	-32,	-33,	-33,	-34,
	-35,	-36,	-36,	-37,	-38,	-39,	-40,	-40,	-41,	-42,
	-43,	-44,	-45,	-46,	-47,	-48,	-49,	-50,	-51,	-52,
	-54,	-55,	-56,	-57,	-59,	-60,	-61,	-63,	-64,	-66,
	-67,	-69,	-71,	-72,	-74,	-76,	-78,	-80,	-83,	-85,
	-87,	-90,	-93,	-96,	-99,	-102,	-106,	-110,	-115,	-120,
	-126,	-133,	-140,	-150,	-163,	-180,	-210,	-904
};

/*$on*/
SND_tdst_GameCubeADPCMInfo gcSND_gst_GameCubeCoefficientsTable = 
{
    {
        (short)0x4ab,
        (short)0xfced,
        (short)0x789,
        (short)0xfedf,
        (short)0x9a2,
        (short)0xfae5,
        (short)0xc90,
        (short)0xfac1,
        (short)0x84d,
        (short)0xfaa4,
        (short)0x982,
        (short)0xfdf7,
        (short)0xaf6,
        (short)0xfafa,
        (short)0xbe6,
        (short)0xfbf5
    }, //coef
    0, //gain
    0, //pred_scale
    0, //yn1
    0, //yn2
    0, //loop_pred_scale
    0, //loop_yn1
    0  //loop_yn2
};


/*$4
 ***********************************************************************************************************************
    Public Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_RT_Lib_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	static int	YetDone = 0;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(!YetDone)
	{
		YetDone = 1;
		AIInit(NULL);
		AXInit();
		MIXInit();
		
		AXSetCompressor(AX_COMPRESSOR_ON);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG gcSND_l_Init(SND_tdst_TargetSpecificData *_pst_SP)
{
	/* RTI lib */
	gcSND_RT_Lib_Init();

	/* init global var */
	gcSND_gi_MasterVolume = -10;

	/* soft buffer */
	L_memset(gcSND_gax_SoftBuffer, 0, SND_Cte_MaxSoftBufferNb * sizeof(gcSND_tdst_SoftBuffer));

	/* stream */
	L_memset((char *) gcSND_gax_StreamList, 0, SND_Cte_MaxSimultaneousStream * sizeof(gcSND_tdst_Stream));
	gcSND_gpst_CurrentStream = gcSND_gax_StreamList;

	/* audio callback */
	AXRegisterCallback(gcSND_AudioFrameCallBack);

	gcSND_gp_BufferForLoading = (u8 *) MEM_p_AllocAlign(2 * CDVD_BUFFER_SIZE, 32);
	L_memset(gcSND_gp_BufferForLoading, 0, 2 * CDVD_BUFFER_SIZE);
	gcSND_StreamInitModule();
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_Close(SND_tdst_TargetSpecificData *_pst_SP)
{
	gcSND_StreamCloseModule();
	L_memset(gcSND_gax_SoftBuffer, 0, SND_Cte_MaxSoftBufferNb * sizeof(gcSND_tdst_SoftBuffer));
	MIXQuit();
	AXQuit();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SetGlobalVol(LONG _i_Level)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_Soft;
	int						i;
	BOOL					old;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	_i_Level = (int) ((float) _i_Level * 0.1f);
	if(_i_Level != gcSND_gi_MasterVolume)
	{
		gcSND_gi_MasterVolume = _i_Level;
		pst_Soft = gcSND_gax_SoftBuffer;

		old = OSDisableInterrupts();
		for(i = 0; i < SND_Cte_MaxSoftBufferNb; i++, pst_Soft++)
		{
			pst_Soft->ast_Hard[0].i_MIXinput = pst_Soft->i_Volume + gcSND_gi_MasterVolume + Cte_Bias;
			pst_Soft->ast_Hard[1].i_MIXinput = pst_Soft->i_Volume + gcSND_gi_MasterVolume + Cte_Bias;
			
			if(pst_Soft->ast_Hard[0].pst_AxBuffer && (pst_Soft->ast_Hard[0].pst_AxBuffer->pb.state != AX_PB_STATE_STOP))
				MIXSetInput(pst_Soft->ast_Hard[0].pst_AxBuffer, pst_Soft->ast_Hard[0].i_MIXinput);
			
			if(pst_Soft->ast_Hard[1].pst_AxBuffer && (pst_Soft->ast_Hard[1].pst_AxBuffer->pb.state != AX_PB_STATE_STOP))
				MIXSetInput(pst_Soft->ast_Hard[1].pst_AxBuffer, pst_Soft->ast_Hard[1].i_MIXinput);
		}

		OSRestoreInterrupts(old);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG gcSND_l_GetGlobalVol(void)
{
	if(!SND_gst_Params.l_Available) return 0;
	return(gcSND_gi_MasterVolume * 10);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *gcSND_pst_SB_Create
(
	SND_tdst_TargetSpecificData *_p,
	SND_tdst_WaveData			*_pst_WI,
	void						*_pv_Position
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundBuffer	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- check format -----------------------------------------------------------------------------------------------*/

	if(_pst_WI->st_WaveFmtx.wFormatTag != SND_Cte_DefaultWaveFormat) return NULL;

	/*$2- get ressources ---------------------------------------------------------------------------------------------*/

	if(_pst_WI->st_WaveFmtx.nChannels == 1)
	{
		pst_Sound = (SND_tdst_SoundBuffer*)gcSND_pst_CreateSoftBuffer
			(
				_pst_WI->st_WaveFmtx.nSamplesPerSec,
				(unsigned int) _pv_Position,
				_pst_WI->i_Size
			);
		if(!pst_Sound)
		{
			gcSND_SoftRelease(pst_Sound);
			pst_Sound = NULL;
		}
	}
	else
	{
		gcSND_M_Err(gcSND_pst_SB_Create, "Operation failed : channel number is not 1");
		pst_Sound = NULL;
	}

	return pst_Sound;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_SB_Play(SND_tdst_SoundBuffer *_pst_SB, int _i_Flag, int _i_FxVolLeft, int _i_FxVolRight)
{
	/*~~~~~~~~~~~~~~~*/
	int i_RetR, iFxVol, fader;
	gcSND_tdst_SoftBuffer*pSoft=gcSND_M_GetBuffer(_pst_SB);
	float fFxL, fFxR, fFx; 
	/*~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;
	gcSND_M_CheckSoundBuffPtrOrReturn(pSoft, -1);

	fFxL = SND_f_GetVolFromAtt(_i_FxVolLeft);
	fFxR = SND_f_GetVolFromAtt(_i_FxVolRight);
	fFx = 0.5f*(fFxL + fFxR);
	if(fFx < 0.5f)
	{
		// 0% wet + 100% dry
		fFx = 0.0f;
		fader = gcSND_Cte_MaxVol;
	}
	else if(fFx < 0.7f)
	{
		// 100% wet + 100% dry
		fFx = 1.0f;
		fader = gcSND_Cte_MaxVol;
	}
	else
	{
		// 100% wet + 0% dry
		fFx = 1.0f;
		fader = gcSND_Cte_MinVol; 
	}
	
	iFxVol = SND_l_GetAttFromVol(fFx);
	i_RetR = gcSND_i_SoftPlay(pSoft, _i_Flag, iFxVol);
	
	if(!pSoft->ast_Hard[0].pst_AxBuffer) return -1;
	

	if(!i_RetR)
	{
		MIXSetInput(pSoft->ast_Hard[0].pst_AxBuffer, pSoft->ast_Hard[0].i_MIXinput);
		MIXSetFader(pSoft->ast_Hard[0].pst_AxBuffer, fader);
		AXSetVoiceState(pSoft->ast_Hard[0].pst_AxBuffer, AX_PB_STATE_RUN);
	}
	

	pSoft->ui_Flags |= SND_Cul_SBS_Playing | (SND_Cul_SBC_PlayLooping & _i_Flag);

	return i_RetR;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SB_Stop(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~*/
	BOOL	old;
	gcSND_tdst_SoftBuffer*pSoft=gcSND_M_GetBuffer(_pst_SB);
	/*~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	gcSND_M_CheckSoundBuffPtrOrReturn(pSoft, ;);

	if(pSoft->ui_Flags & SND_Cte_StreamedFile)
	{
		gcSND_StopStreamedSB(_pst_SB);
	}
	else
	{
		old = OSDisableInterrupts();
		gcSND_FxDelSB(pSoft);
		gcSND_SoftStop(pSoft);		
		OSRestoreInterrupts(old);
	}

	pSoft->ui_Flags &= ~(SND_Cul_DSF_StartedOnce | SND_Cul_SBS_Playing | SND_Cul_SBS_PlayingLoop);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SB_Release(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~*/
	BOOL	old;
	gcSND_tdst_SoftBuffer*pSoft=gcSND_M_GetBuffer(_pst_SB);
	gcSND_M_CheckSoundBuffPtrOrReturn(pSoft, ;);

	old = OSDisableInterrupts();
	gcSND_SoftRelease(pSoft);
	OSRestoreInterrupts(old);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SB_Duplicate
(
	SND_tdst_TargetSpecificData *_pst_SpeData,
	SND_tdst_SoundBuffer		*_pst_SrcSB,
	SND_tdst_SoundBuffer		**_ppst_DstSB
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundBuffer	*pst_New;
	BOOL					old;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_ppst_DstSB) return;
	*_ppst_DstSB = NULL;

	if(!_pst_SrcSB) return;

	if(!SND_gst_Params.l_Available) return;

	/* check entry */
	gcSND_M_CheckSoundBuffPtrOrReturn(_pst_SrcSB, ;);

	/* get ressource */

	old = OSDisableInterrupts();
	pst_New = (SND_tdst_SoundBuffer*)gcSND_pst_SoftDuplicate((gcSND_tdst_SoftBuffer*)_pst_SrcSB);
	if(!pst_New)
	{
		gcSND_SoftRelease((gcSND_tdst_SoftBuffer*)pst_New);
		pst_New = NULL;
	}

	OSRestoreInterrupts(old);

	*_ppst_DstSB = pst_New;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SB_SetVolume(SND_tdst_SoundBuffer *_pst_SB, int _i_Level)
{
	/*~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_Soft;
	/*~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	
	pst_Soft = gcSND_M_GetBuffer(_pst_SB);
	gcSND_M_CheckSoundBuffPtrOrReturn(pst_Soft, ;);

	pst_Soft->i_Volume =(int) ((float) _i_Level * 0.1f);
	gcSND_SB_SetPan(_pst_SB, pst_Soft->i_Pan, pst_Soft->i_SPan);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SB_SetFrequency(SND_tdst_SoundBuffer *_pst_SB, int _i_Freq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_Soft;
	BOOL					old;
	int 					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	pst_Soft = gcSND_M_GetBuffer(_pst_SB);
	gcSND_M_CheckSoundBuffPtrOrReturn(pst_Soft, ;);

	if(lAbs(pst_Soft->i_Frequency - _i_Freq) >= 100)
	{
		old = OSDisableInterrupts();

		pst_Soft->i_Frequency = _i_Freq;
		pst_Soft->f_SRCratio = fGetSRCRatioFromFreq(_i_Freq);
		
		for(i=0; i<2; i++)
		{
			if(pst_Soft->ast_Hard[i].pst_AxBuffer)
				AXSetVoiceSrcRatio(pst_Soft->ast_Hard[i].pst_AxBuffer, pst_Soft->f_SRCratio);
		}

		pst_Soft->i_Frequency = _i_Freq;
		OSRestoreInterrupts(old);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SB_SetPan(SND_tdst_SoundBuffer *_pst_SB, int _i_Pan, int _i_FrontRear)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_Soft;
	BOOL					old;
	float					f;
	int						idx;
	int						Sidx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	

	
	pst_Soft = gcSND_M_GetBuffer(_pst_SB);
	gcSND_M_CheckSoundBuffPtrOrReturn(pst_Soft, ;);

	// pan : [-10 000, +10 000] => [0, 127]
	if(_i_Pan > 0)
	{
		f = SND_f_GetVolFromAtt(-_i_Pan);
		idx = (int) (127.0f - (63.0f * f));
	}
	else if(_i_Pan < 0)
	{
		f = SND_f_GetVolFromAtt(_i_Pan);
		idx = (int) (64.0f * f);
	}
	else
	{
		idx = 64;
	}

	if(idx < 0) idx = 0;
	if(idx > 127) idx = 127;

	// span : [-10 000, +10 000] => [0, 127]
	if(_i_FrontRear > 0)
	{
		f = SND_f_GetVolFromAtt(-_i_FrontRear);
		Sidx = (int) (127.0f - (63.0f * f));
	}
	else if(_i_Pan < 0)
	{
		f = SND_f_GetVolFromAtt(_i_FrontRear);
		Sidx = (int) (64.0f * f);
	}
	else
	{
		Sidx = 64;
	}

	if(Sidx < 0) Sidx = 0;
	if(Sidx > 127) Sidx = 127;

	
	old = OSDisableInterrupts();
	pst_Soft->i_Pan = _i_Pan;
	pst_Soft->i_SPan = _i_FrontRear;
	
	if(gcSND_M_IsStereo(_pst_SB))
	{
		switch(SND_gst_Params.ul_RenderMode)
		{
		case SND_Cte_RenderStereo:
		case SND_Cte_RenderHeadPhone:
			// pan
			pst_Soft->ast_Hard[1].i_MIXpan = 0;
			pst_Soft->ast_Hard[0].i_MIXpan = 127;
			pst_Soft->i_StereoPanIdx = idx;
			pst_Soft->ast_Hard[1].i_MIXinput = pst_Soft->i_Volume + gcSND_gi_MasterVolume + Cte_Bias + gcSNDPanTableL[pst_Soft->i_StereoPanIdx];
			pst_Soft->ast_Hard[0].i_MIXinput = pst_Soft->i_Volume +	gcSND_gi_MasterVolume + Cte_Bias +	gcSNDPanTableL[127 - pst_Soft->i_StereoPanIdx];
			// span
			pst_Soft->i_MIXspan = gcSND_Cte_DefaultSPan;
			break;

		case SND_Cte_RenderDolbyPrologic:
		case SND_Cte_RenderDolbyPrologicII:
			// pan
			pst_Soft->ast_Hard[1].i_MIXpan = 0;
			pst_Soft->ast_Hard[0].i_MIXpan = 127;
			pst_Soft->i_StereoPanIdx = idx;
			pst_Soft->ast_Hard[1].i_MIXinput = pst_Soft->i_Volume +	gcSND_gi_MasterVolume + Cte_Bias +	gcSNDPanTableL[pst_Soft->i_StereoPanIdx];
			pst_Soft->ast_Hard[0].i_MIXinput = pst_Soft->i_Volume + gcSND_gi_MasterVolume + Cte_Bias + gcSNDPanTableL[127 - pst_Soft->i_StereoPanIdx];
			// span
			pst_Soft->i_MIXspan = Sidx;
			break;
		
		default:
			pst_Soft->ast_Hard[1].i_MIXpan = 64;
			pst_Soft->ast_Hard[0].i_MIXpan = 64;
			pst_Soft->i_StereoPanIdx = 64;
			pst_Soft->ast_Hard[1].i_MIXinput = pst_Soft->i_Volume + gcSND_gi_MasterVolume + Cte_Bias +	gcSNDPanTableL[pst_Soft->i_StereoPanIdx];
			pst_Soft->ast_Hard[0].i_MIXinput = pst_Soft->i_Volume + gcSND_gi_MasterVolume + Cte_Bias + gcSNDPanTableL[127 - pst_Soft->i_StereoPanIdx];
			// span
			pst_Soft->i_MIXspan = gcSND_Cte_DefaultSPan;
			break;
		}

		if(pst_Soft->ast_Hard[1].pst_AxBuffer)
		{
			MIXSetInput(pst_Soft->ast_Hard[1].pst_AxBuffer, pst_Soft->ast_Hard[1].i_MIXinput);
			MIXSetPan(pst_Soft->ast_Hard[1].pst_AxBuffer, pst_Soft->ast_Hard[1].i_MIXpan);
			MIXSetSPan(pst_Soft->ast_Hard[1].pst_AxBuffer, pst_Soft->i_MIXspan);
		}

		if(pst_Soft->ast_Hard[0].pst_AxBuffer)
		{
			MIXSetInput(pst_Soft->ast_Hard[0].pst_AxBuffer, pst_Soft->ast_Hard[0].i_MIXinput);
			MIXSetPan(pst_Soft->ast_Hard[0].pst_AxBuffer, pst_Soft->ast_Hard[0].i_MIXpan);
			MIXSetSPan(pst_Soft->ast_Hard[0].pst_AxBuffer, pst_Soft->i_MIXspan);
		}
	}
	else
	{
		switch(SND_gst_Params.ul_RenderMode)
		{
		case SND_Cte_RenderStereo:
		case SND_Cte_RenderHeadPhone:
			pst_Soft->ast_Hard[0].i_MIXpan = idx;
			pst_Soft->i_MIXspan = gcSND_Cte_DefaultSPan;
			pst_Soft->ast_Hard[0].i_MIXinput = pst_Soft->i_Volume + gcSND_gi_MasterVolume + Cte_Bias ;
			break;

		case SND_Cte_RenderDolbyPrologic:
		case SND_Cte_RenderDolbyPrologicII:
			pst_Soft->ast_Hard[0].i_MIXpan = idx;
			pst_Soft->i_MIXspan = Sidx;
			pst_Soft->ast_Hard[0].i_MIXinput = pst_Soft->i_Volume + gcSND_gi_MasterVolume + Cte_Bias ;
			break;

		default:
			pst_Soft->ast_Hard[0].i_MIXpan = 64;
			pst_Soft->ast_Hard[0].i_MIXinput = pst_Soft->i_Volume + gcSND_gi_MasterVolume + Cte_Bias;
			// span
			pst_Soft->i_MIXspan = gcSND_Cte_DefaultSPan;
			break;
		}

		if(pst_Soft->ast_Hard[0].pst_AxBuffer)
		{
			MIXSetInput(pst_Soft->ast_Hard[0].pst_AxBuffer, pst_Soft->ast_Hard[0].i_MIXinput);
			MIXSetPan(pst_Soft->ast_Hard[0].pst_AxBuffer, pst_Soft->ast_Hard[0].i_MIXpan);
			MIXSetSPan(pst_Soft->ast_Hard[0].pst_AxBuffer, pst_Soft->i_MIXspan);
		}
	}

	OSRestoreInterrupts(old);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_RefreshStatus(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	gcSND_tdst_SoftBuffer	*p;
	int						playing;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	
	
	//
	// we want to update the buffer playing state
	//
	for(i = 0; i < SND_Cte_MaxSoftBufferNb; i++)
	{
		p = gcSND_gax_SoftBuffer + i;
		
		//$1- skip unused + stopped buffers -----------------------------------------------------------------------------
		
		if(!(p->ui_Flags & SND_Cul_DSF_Used)) continue;
		if(!(p->ui_Flags & SND_Cul_SBS_Playing)) continue;


        //$1- eval the current state ------------------------------------------------------------------------------------
        
		playing = 0;
		if(p->ast_Hard[0].pst_AxBuffer)
		{
			if(p->ast_Hard[0].pst_AxBuffer->pb.state != AX_PB_STATE_STOP) 
			    playing = SND_Cul_SBS_Playing;
			    
			if(p->ast_Hard[0].pst_AxBuffer->pb.addr.loopFlag == AXPBADDR_LOOP_ON)
				playing |= SND_Cul_SBS_PlayingLoop;
		}

        //$1- update the soft buffer state (only if started once) -------------------------------------------------------
        
		if(SND_Cul_DSF_StartedOnce & p->ui_Flags)
		{
			if((playing & SND_Cul_SBS_Playing) == 0)
				p->ui_Flags &= ~(SND_Cul_DSF_StartedOnce | SND_Cul_SBS_Playing | SND_Cul_SBS_PlayingLoop | SND_Cul_SF_StreamIsPlayingNow);
			
			continue;
		}
		
		//$1- detect the 1st play ---------------------------------------------------------------------------------------
		
		// not playing => skip it
    	if( (playing & SND_Cul_SBS_Playing) == 0 ) continue;

		// if stream + specific flag
		if((p->ui_Flags & SND_Cte_StreamedFile) && (p->ui_Flags & SND_Cul_SF_StreamIsPlayingNow))
    		p->ui_Flags |= SND_Cul_DSF_StartedOnce;
    		
    	// if not stream
    	if((p->ui_Flags & SND_Cte_StreamedFile) == 0)
			p->ui_Flags |= SND_Cul_DSF_StartedOnce;
	}

	gcSND_UpdateProfiler();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_SB_GetStatus(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Status)
{
	gcSND_tdst_SoftBuffer	*pSoft;


	if(!SND_gst_Params.l_Available) return -1;
	pSoft = gcSND_M_GetBuffer(_pst_SB);
	gcSND_M_CheckSoundBuffPtrOrReturn(pSoft, -1);
	
	
	*_pi_Status = 0;

	if(pSoft->ui_Flags & SND_Cte_StreamedFile)
	{

	    if(pSoft->ui_Flags & SND_Cul_SF_StreamIsPlayingNow)
	    {
    		if(pSoft->ast_Hard[0].pst_AxBuffer)
    		{
    			if(pSoft->ast_Hard[0].pst_AxBuffer->pb.state != AX_PB_STATE_STOP) 
    			{
    			    *_pi_Status = SND_Cul_SBS_Playing;
    			    if(pSoft->ast_Hard[0].pst_AxBuffer->pb.addr.loopFlag == AXPBADDR_LOOP_ON)
    				    *_pi_Status |= SND_Cul_SBS_PlayingLoop;
    		    }
    		}
		}
		else if(pSoft->ui_Flags & SND_Cul_SBS_Playing)
		{
		    *_pi_Status = SND_Cul_SBS_Playing;
		}
	}
	else
	{
		if(pSoft->ast_Hard[0].pst_AxBuffer)
		{
			if(pSoft->ast_Hard[0].pst_AxBuffer->pb.state != AX_PB_STATE_STOP) 
			{
			    *_pi_Status = SND_Cul_SBS_Playing;
			    if(pSoft->ast_Hard[0].pst_AxBuffer->pb.addr.loopFlag == AXPBADDR_LOOP_ON)
				    *_pi_Status |= SND_Cul_SBS_PlayingLoop;
		    }
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SB_GetFrequency(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Freq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pSoft;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	pSoft = gcSND_M_GetBuffer(_pst_SB);
	gcSND_M_CheckSoundBuffPtrOrReturn(pSoft, ;);
	*_pi_Freq = pSoft->i_Frequency;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SB_GetPan(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pan, int *_pi_FrontRear)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pSoft=(gcSND_tdst_SoftBuffer	*)_pst_SB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	pSoft = gcSND_M_GetBuffer(_pst_SB);
	gcSND_M_CheckSoundBuffPtrOrReturn(pSoft, ;);
	*_pi_Pan = pSoft->i_Pan;
	*_pi_FrontRear = pSoft->i_SPan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SB_GetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pos, int *_pi_Write)
{
	gcSND_tdst_SoftBuffer	*pSoft;

	*_pi_Pos = 0;
	*_pi_Write = 0;
	
	if(!SND_gst_Params.l_Available) return;
	pSoft = gcSND_M_GetBuffer(_pst_SB);
	gcSND_M_CheckSoundBuffPtrOrReturn(pSoft, ;);
    
    if(pSoft->ui_Flags & SND_Cte_StreamedFile)
    {
        gcSND_Stream_GetCurrPos(_pst_SB, _pi_Pos, _pi_Write);
    }
    else
    {
    	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    	unsigned int			ui_CurrentPosition;
    	unsigned int			ui_AramStart;
    	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    	if(!pSoft->ast_Hard[0].pst_AxBuffer) return ;
    	
    	ui_CurrentPosition = uiGetAddrFromHiLo(pSoft->ast_Hard[0].pst_AxBuffer->pb.addr.currentAddressHi, pSoft->ast_Hard[0].pst_AxBuffer->pb.addr.currentAddressLo);
    	ui_AramStart = (unsigned int)pSoft->pc_AramBuffer;
    	ui_CurrentPosition = ui_CurrentPosition-ui_AramStart;
    	
    	*(unsigned int*)_pi_Pos = SND_ui_SizeToSample(SND_Cte_DefaultWaveFormat, 1, ui_CurrentPosition);
    }	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SB_SetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int _i_Pos)
{
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *gcSND_Reload
(
	SND_tdst_TargetSpecificData *_pSpecific,
	SND_tdst_SoundBuffer		*_pFromSB,
	SND_tdst_WaveData			*_pWaveData,
	unsigned int				_uiDataPosition
)
{
	gcSND_tdst_SoftBuffer	*pSoft=(gcSND_tdst_SoftBuffer	*)_pFromSB;
	if(!SND_gst_Params.l_Available) return NULL;
	gcSND_M_CheckSoundBuffPtrOrReturn(_pFromSB, NULL);

	/* check */
	if(!_pFromSB) return NULL;
	if(!_pWaveData) return NULL;
	if(!_pWaveData->i_Size) return NULL;
	if(!_uiDataPosition) return NULL;

	/* register modif */
	pSoft->ui_DataPosition = (unsigned int) _uiDataPosition ;
	pSoft->ui_DataSize = _pWaveData->i_Size;
	
	gcSND_SB_SetFrequency(_pFromSB, _pWaveData->st_WaveFmtx.nSamplesPerSec);
	gcSND_SB_SetVolume(_pFromSB, 0);
	gcSND_SB_SetPan(_pFromSB, 0, 0);

	/* is there any RAM buffer */
	if(!pSoft->pc_RamBuffer)
	{
		pSoft->pc_RamBuffer = MEM_p_AllocAlign(lRoudUp64(pSoft->ui_DataSize), 64);
		gcSND_M_Assert(pSoft->pc_RamBuffer);
	}

	/* update address */
	pSoft->ast_Hard[0].st_AxAddrSettings.format = AX_PB_FORMAT_ADPCM;
	pSoft->ast_Hard[0].st_AxAddrSettings.loopAddressHi = sGetStartAddrHi(gcSND_pv_GetZeroBuffer());
	pSoft->ast_Hard[0].st_AxAddrSettings.loopAddressLo = sGetStartAddrLo(gcSND_pv_GetZeroBuffer());
	pSoft->ast_Hard[0].st_AxAddrSettings.endAddressHi = sGetEndAddrHi
		(
			pSoft->pc_AramBuffer,
			pSoft->ui_DataSize
		);
	pSoft->ast_Hard[0].st_AxAddrSettings.endAddressLo = sGetEndAddrLo
		(
			pSoft->pc_AramBuffer,
			pSoft->ui_DataSize
		);
	pSoft->ast_Hard[0].st_AxAddrSettings.currentAddressHi = sGetStartAddrHi(pSoft->pc_AramBuffer);
	pSoft->ast_Hard[0].st_AxAddrSettings.currentAddressLo = sGetStartAddrLo(pSoft->pc_AramBuffer);

	/* load adpcm */
	if(!pSoft->pst_SndFileHandler)
		pSoft->pst_SndFileHandler = GC_pst_StreamOpen(BIG_gst.h_CLibFileHandle, 0x1002);

	GC_i_StreamRead
			(
				pSoft->pst_SndFileHandler,
				pSoft->pc_RamBuffer,
				pSoft->ui_DataSize,
				pSoft->ui_DataPosition,
				gcSND_RamLoadingCallBack,
				(unsigned int) pSoft
			);


	return _pFromSB;
}

void gcSND_RamLoadingCallBack(s32 result, DVDFileInfo *fileInfo, unsigned int context)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_Soft;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Soft = (gcSND_tdst_SoftBuffer *) context;

	switch(result)
	{
	case DVD_RESULT_FATAL_ERROR:
		gcSND_M_Err(gcSND_RamLoadingCallBack, "Bad state in during Ram transfert");
		break;
	case DVD_RESULT_CANCELED:
		gcSND_M_Err(gcSND_RamLoadingCallBack, "Ram transfert canceled");
		break;
	case DVD_RESULT_GOOD:
		pst_Soft->ast_Hard[0].pred_scale = (unsigned short)(*pst_Soft->pc_RamBuffer);
		GC_StreamClose(pst_Soft->pst_SndFileHandler);
		pst_Soft->pst_SndFileHandler = NULL;
		DCFlushRange(pst_Soft->pc_RamBuffer, lRoudUp64(pst_Soft->ui_DataSize));
		ARQPostRequest
		(
			&pst_Soft->st_ARQRequest,
			(u32) pst_Soft,
			ARQ_TYPE_MRAM_TO_ARAM,
			ARQ_PRIORITY_HIGH,
			(u32) pst_Soft->pc_RamBuffer,
			(u32) pst_Soft->pc_AramBuffer,
			lRoudUp64(pst_Soft->ui_DataSize),
			gcSND_ARQCallBack
		);
		break;
	default:
		gcSND_M_Err(gcSND_RamLoadingCallBack, "Bad state in Ram transfert");
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_CommitChange(MATH_tdst_Vector *_pst_MicorPos)
{
	gcSND_UpdateStream();
	gcSND_RefreshStatus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_ChangeRenderMode(int _i_Mode)
{
	/*~~~~~*/
	u32 mode;
	static BOOL b_DontCareMainSoundMode=FALSE;
	extern void gcSND_FxPause(void);
	extern void gcSND_FxResume(void);
	/*~~~~~*/

	gcSND_RT_Lib_Init();

	/*$2- get GC sound mode ------------------------------------------------------------------------------------------*/

	mode = OSGetSoundMode();

	/*$2- if we get the mode from the console ------------------------------------------------------------------------*/

	if(SND_Cte_RenderUseConsoleSettings == _i_Mode)
	{
		switch(mode)
		{
		case OS_SOUND_MODE_MONO:
			_i_Mode = SND_Cte_RenderMono;
			break;
		case OS_SOUND_MODE_STEREO:
			b_DontCareMainSoundMode = TRUE; // if IPL returns stereo, we can want we want
			_i_Mode = SND_Cte_RenderStereo;
			break;
		default:
			_i_Mode = SND_Cte_RenderStereo;
			break;
		}
	}

	/*$2- the console mode is preceding our game mode ----------------------------------------------------------------*/

//	if((mode == OS_SOUND_MODE_MONO) && !b_DontCareMainSoundMode) _i_Mode = SND_Cte_RenderMono;

	/*$2- Now do the action ------------------------------------------------------------------------------------------*/

	gcSND_FxPause();
	switch(_i_Mode)
	{
	case SND_Cte_RenderMono:
		SND_gst_Params.ul_RenderMode = SND_Cte_RenderMono;
		OSSetSoundMode(OS_SOUND_MODE_MONO);
		AXSetMode(AX_MODE_STEREO);
		MIXSetSoundMode(MIX_SOUND_MODE_MONO);
		break;

	case SND_Cte_RenderHeadPhone:
		SND_gst_Params.ul_RenderMode = SND_Cte_RenderHeadPhone;
		OSSetSoundMode(OS_SOUND_MODE_STEREO);
		AXSetMode(AX_MODE_STEREO);
		MIXSetSoundMode(MIX_SOUND_MODE_STEREO);
		break;

	case SND_Cte_RenderStereo:
		SND_gst_Params.ul_RenderMode = SND_Cte_RenderStereo;
		OSSetSoundMode(OS_SOUND_MODE_STEREO);
		AXSetMode(AX_MODE_STEREO);
		MIXSetSoundMode(MIX_SOUND_MODE_STEREO);
		break;

	case SND_Cte_RenderDolbyPrologic:
		SND_gst_Params.ul_RenderMode = SND_Cte_RenderDolbyPrologic;
		OSSetSoundMode(OS_SOUND_MODE_STEREO);
		AXSetMode(AX_MODE_SURROUND);
		MIXSetSoundMode(MIX_SOUND_MODE_SURROUND);
		break;

	case SND_Cte_RenderDolbyPrologicII:
		SND_gst_Params.ul_RenderMode = SND_Cte_RenderDolbyPrologicII;
		OSSetSoundMode(OS_SOUND_MODE_STEREO);
		AXSetMode(AX_MODE_DPL2);
		MIXSetSoundMode(MIX_SOUND_MODE_DPL2);
		break;

	default:
		SND_gst_Params.ul_RenderMode = SND_Cte_RenderStereo;
		OSSetSoundMode(OS_SOUND_MODE_STEREO);
		AXSetMode(AX_MODE_STEREO);
		MIXSetSoundMode(MIX_SOUND_MODE_STEREO);
		return -2;
	}
	gcSND_FxResume();

	return 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* GC */
