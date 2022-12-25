/*$T gcSND_Soft.c GC 1.138 12/06/04 10:02:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <dolphin/mix.h>

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"

#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGopen.h"

#include "GameCube/GC_File.h"
#include "GameCube/GC_Stream.h"

#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDconv.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/gc/gcSND.h"
#include "SouND/Sources/gc/gcSND_Debug.h"
#include "SouND/Sources/gc/gcSND_ARAM.h"
#include "SouND/Sources/gc/gcSND_AudioFrame.h"
#include "SouND/Sources/gc/gcSND_Soft.h"
#include "SouND/Sources/SNDfx.h"

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

static void gcSND_DropCallBack(void *pv);
extern		BIG_ReadNoSeek(ULONG, void *, ULONG);
void		gcSND_RamLoadingCallBack(s32 result, DVDFileInfo *fileInfo, unsigned int context);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

gcSND_tdst_SoftBuffer	gcSND_gax_SoftBuffer[SND_Cte_MaxSoftBufferNb];

#ifdef _DEBUG
unsigned int			gcSND_gui_AllocSize = 0;
unsigned int			gcSND_gui_AllocMaxSize = 0;
#endif

/*$4
 ***********************************************************************************************************************
    private functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_ARQCallBack(u32 task)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ARQRequest				*p;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p = (ARQRequest *) task;
	pst_Soft = (gcSND_tdst_SoftBuffer *) p->owner;

	if(pst_Soft->pc_RamBuffer)
	{
		MEM_Free((void *) pst_Soft->pc_RamBuffer);
#ifdef _DEBUG
		if(!MEM_IsTmpPointer(pst_Soft->pc_RamBuffer))
		{
			gcSND_gui_AllocSize -= lRoudUp64(pst_Soft->ui_DataSize);
		}
#endif
	}

	pst_Soft->pc_RamBuffer = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_DropCallBack(void *pv)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AXVPB					*pst_AXVPB;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	BOOL					old;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	old = OSDisableInterrupts();

	pst_AXVPB = (AXVPB *) pv;
	pst_Soft = (gcSND_tdst_SoftBuffer *) pst_AXVPB->userContext;
	gcSND_M_ErrX(gcSND_DropCallBack, "One voice is dropped : %x", (int) pst_Soft);

	if(pst_AXVPB == pst_Soft->ast_Hard[0].pst_AxBuffer) pst_Soft->ast_Hard[0].pst_AxBuffer = NULL;
	if(pst_AXVPB == pst_Soft->ast_Hard[1].pst_AxBuffer) pst_Soft->ast_Hard[1].pst_AxBuffer = NULL;

	OSRestoreInterrupts(old);
}

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
gcSND_tdst_SoftBuffer *gcSND_pst_AllocSoftBuffer(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	gcSND_tdst_SoftBuffer	*pst_SoftBuffer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SoftBuffer = gcSND_gax_SoftBuffer;
	for(i = 0; i < SND_Cte_MaxSoftBufferNb; i++)
	{
		if((pst_SoftBuffer->ui_Flags & SND_Cul_DSF_Used) == 0)
		{
			pst_SoftBuffer->ui_Flags = SND_Cul_DSF_Used;
			return pst_SoftBuffer;
		}

		pst_SoftBuffer++;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_FreeSoftBuffer(gcSND_tdst_SoftBuffer *_pst_SoftBuffer)
{
	if(_pst_SoftBuffer) L_memset(_pst_SoftBuffer, 0, sizeof(gcSND_tdst_SoftBuffer));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *gcSND_pst_CreateSoftBuffer
(
	int				_i_InitFreq,
	unsigned int	_ui_DataPosition,
	unsigned int	_ui_DataSize
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*pc_Ram, *pc_Aram;
	gcSND_tdst_SoftBuffer	*pst_Soft;
	int i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- get ressources ---------------------------------------------------------------------------------------------*/

	if(_ui_DataPosition)
		pc_Ram = MEM_p_AllocTmp(lRoudUp64(_ui_DataSize));
	else
		pc_Ram = MEM_p_AllocAlign(lRoudUp64(_ui_DataSize), 64);

	if(!pc_Ram)
	{
		gcSND_M_Err(gcSND_pst_CreateSoftBuffer, "No more RAM");
		return NULL;
	}

	pc_Aram = (char *) gcSND_pv_AllocARAM(lRoudUp64(_ui_DataSize));
	if(!pc_Aram)
	{
		gcSND_M_Err(gcSND_pst_CreateSoftBuffer, "No more ARAM");
		MEM_Free(pc_Ram);
		return NULL;
	}

	pst_Soft = gcSND_pst_AllocSoftBuffer();
	if(!pst_Soft)
	{
		gcSND_M_Err(gcSND_pst_CreateSoftBuffer, "No more SoftBuffer");
		MEM_Free(pc_Ram);
		gcSND_FreeARAM(pc_Aram);
		return NULL;
	}

#ifdef _DEBUG
	if(!MEM_IsTmpPointer(pc_Ram))
	{
		gcSND_gui_AllocSize += lRoudUp64(_ui_DataSize);
		if(gcSND_gui_AllocSize > gcSND_gui_AllocMaxSize) gcSND_gui_AllocMaxSize = gcSND_gui_AllocSize;
	}
#endif

	/*$2- init of SB -------------------------------------------------------------------------------------------------*/

	pst_Soft->ui_Flags = SND_Cul_DSF_Used;

	/* pan */
	pst_Soft->i_Pan = 0;
	pst_Soft->i_SPan = 0;
	pst_Soft->ast_Hard[0].i_MIXpan = gcSND_Cte_DefaultPan;
	pst_Soft->ast_Hard[1].i_MIXpan = gcSND_Cte_DefaultPan;
	pst_Soft->i_MIXspan = gcSND_Cte_DefaultSPan;

	/* volume */
	pst_Soft->i_Volume = SND_Cte_MinAtt;
	pst_Soft->i_FxVol = SND_Cte_MinAtt;

	/* sample rate converter settings */
	pst_Soft->ast_Hard[0].i_MIXinput = gcSND_Cte_DefaultInput;
	pst_Soft->ast_Hard[1].i_MIXinput = gcSND_Cte_DefaultInput;
	
	for(i = 0; i < SND_Cte_FxCoreNb;++i)
		pst_Soft->i_MIXaux[i] = gcSND_Cte_MinVol;	

	/* frequency */
	pst_Soft->i_Frequency = _i_InitFreq;
	pst_Soft->f_SRCratio = fGetSRCRatioFromFreq(pst_Soft->i_Frequency);

	pst_Soft->ui_DataPosition = (unsigned int) _ui_DataPosition;
	pst_Soft->ui_DataSize = _ui_DataSize;

	pst_Soft->pst_DuplicateSrc = NULL;
	pst_Soft->ui_UserNb = 1;

	pst_Soft->pc_AramBuffer = pc_Aram;
	pst_Soft->pc_RamBuffer = pc_Ram;

	/* no voice */
	pst_Soft->ast_Hard[0].pst_AxBuffer = NULL;
	pst_Soft->ast_Hard[1].pst_AxBuffer = NULL;

	/* init of addressing (one shot by default) */
	pst_Soft->ast_Hard[0].st_AxAddrSettings.loopFlag = AXPBADDR_LOOP_OFF;

	pst_Soft->ast_Hard[0].st_AxAddrSettings.format = AX_PB_FORMAT_ADPCM;
	pst_Soft->ast_Hard[0].st_AxAddrSettings.loopAddressHi = sGetStartAddrHi(gcSND_pv_GetZeroBuffer());
	pst_Soft->ast_Hard[0].st_AxAddrSettings.loopAddressLo = sGetStartAddrLo(gcSND_pv_GetZeroBuffer());
	pst_Soft->ast_Hard[0].st_AxAddrSettings.endAddressHi = sGetEndAddrHi
		(
			pst_Soft->pc_AramBuffer,
			pst_Soft->ui_DataSize
		);
	pst_Soft->ast_Hard[0].st_AxAddrSettings.endAddressLo = sGetEndAddrLo
		(
			pst_Soft->pc_AramBuffer,
			pst_Soft->ui_DataSize
		);
	pst_Soft->ast_Hard[0].st_AxAddrSettings.currentAddressHi = sGetStartAddrHi(pst_Soft->pc_AramBuffer);
	pst_Soft->ast_Hard[0].st_AxAddrSettings.currentAddressLo = sGetStartAddrLo(pst_Soft->pc_AramBuffer);

	if(_ui_DataPosition)
	{
		gcSND_SoftLoadData(pst_Soft);
	}

	/*$2- return code ------------------------------------------------------------------------------------------------*/

	return pst_Soft;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int gcSND_i_SoftPlay(gcSND_tdst_SoftBuffer *pst_Soft, int _i_Flag, int _i_FxVol)
{
	/*~~~~~~~~~~~~*/
	AXPBSRC st_SRC;
	/*~~~~~~~~~~~~*/

	/*$1- check ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_Soft->pc_RamBuffer)
	{
		gcSND_M_Err(gcSND_i_SoftPlay, "Play failed, trying to play an unloaded voice");
		return -1;
	}

	/*$1- ax buffer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_Soft->ast_Hard[0].pst_AxBuffer == NULL)
	{
		pst_Soft->ast_Hard[0].pst_AxBuffer = AXAcquireVoice
			(
				gcSND_Cte_VoicePriority,
				gcSND_DropCallBack,
				(u32) pst_Soft
			);
		if(pst_Soft->ast_Hard[0].pst_AxBuffer)
		{
			MIXInitChannel
			(
				pst_Soft->ast_Hard[0].pst_AxBuffer,
				gcSND_Cte_DefaultMode,
				-960,	/* pst_Soft->i_MIXinput, */
				pst_Soft->i_MIXaux[SND_Cte_FxCoreA],
				pst_Soft->i_MIXaux[SND_Cte_FxCoreB],
				#ifdef _RVL
				pst_Soft->i_MIXaux[SND_Cte_FxCoreC],				
				#endif
				pst_Soft->ast_Hard[0].i_MIXpan,
				pst_Soft->i_MIXspan,
				gcSND_Cte_DefaultFader
			);

			st_SRC.ratioHi = sGetHiRatioFromfRatio(pst_Soft->f_SRCratio);
			st_SRC.ratioLo = sGetLoRatioFromfRatio(pst_Soft->f_SRCratio);
			st_SRC.currentAddressFrac = 0;
			st_SRC.last_samples[0] = 0;
			st_SRC.last_samples[1] = 0;
			st_SRC.last_samples[2] = 0;
			st_SRC.last_samples[3] = 0;
			AXSetVoiceSrcType(pst_Soft->ast_Hard[0].pst_AxBuffer, AX_SRC_TYPE_4TAP_16K);
			AXSetVoiceSrc(pst_Soft->ast_Hard[0].pst_AxBuffer, &st_SRC);
		}
	}

	/*$1- play ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_Soft->ast_Hard[0].pst_AxBuffer)
	{
		if(_i_Flag & SND_Cul_SBC_PlayLooping)
		{
			pst_Soft->ast_Hard[0].st_AxAddrSettings.loopFlag = AXPBADDR_LOOP_ON;
			pst_Soft->ast_Hard[0].st_AxAddrSettings.loopAddressHi = sGetStartAddrHi(pst_Soft->pc_AramBuffer);
			pst_Soft->ast_Hard[0].st_AxAddrSettings.loopAddressLo = sGetStartAddrLo(pst_Soft->pc_AramBuffer);
			gcSND_gst_GameCubeCoefficientsTable.pred_scale = pst_Soft->ast_Hard[0].pred_scale;
			gcSND_gst_GameCubeCoefficientsTable.loop_pred_scale = gcSND_gst_GameCubeCoefficientsTable.pred_scale;

			AXSetVoiceType(pst_Soft->ast_Hard[0].pst_AxBuffer, AX_PB_TYPE_NORMAL);
			AXSetVoiceAdpcm(pst_Soft->ast_Hard[0].pst_AxBuffer, &gcSND_gst_GameCubeCoefficientsTable);
			AXSetVoiceAdpcmLoop
			(
				pst_Soft->ast_Hard[0].pst_AxBuffer,
				&gcSND_gst_GameCubeCoefficientsTable.loop_pred_scale
			);
		}
		else
		{
			gcSND_gst_GameCubeCoefficientsTable.pred_scale = pst_Soft->ast_Hard[0].pred_scale;
			gcSND_gst_GameCubeCoefficientsTable.loop_pred_scale = 0;

			AXSetVoiceType(pst_Soft->ast_Hard[0].pst_AxBuffer, AX_PB_TYPE_NORMAL);
			AXSetVoiceAdpcm(pst_Soft->ast_Hard[0].pst_AxBuffer, &gcSND_gst_GameCubeCoefficientsTable);
			pst_Soft->ast_Hard[0].st_AxAddrSettings.loopFlag = AXPBADDR_LOOP_OFF;
			pst_Soft->ast_Hard[0].st_AxAddrSettings.loopAddressHi = sGetStartAddrHi(gcSND_pv_GetZeroBuffer());
			pst_Soft->ast_Hard[0].st_AxAddrSettings.loopAddressLo = sGetStartAddrLo(gcSND_pv_GetZeroBuffer());
		}

		pst_Soft->ast_Hard[0].st_AxAddrSettings.currentAddressHi = sGetStartAddrHi(pst_Soft->pc_AramBuffer);
		pst_Soft->ast_Hard[0].st_AxAddrSettings.currentAddressLo = sGetStartAddrLo(pst_Soft->pc_AramBuffer);
		AXSetVoiceAddr(pst_Soft->ast_Hard[0].pst_AxBuffer, &pst_Soft->ast_Hard[0].st_AxAddrSettings);

		MIXSetInput(pst_Soft->ast_Hard[0].pst_AxBuffer, -960);
	}

	/*$1- fx ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Soft->ui_Flags &= ~(SND_Cul_SF_FxA | SND_Cul_SF_FxB);
	pst_Soft->i_FxVol = SND_Cte_MinAtt;
	if(_i_Flag & SND_Cul_SBC_PlayFxA)
	{
		pst_Soft->i_FxVol = _i_FxVol;
		pst_Soft->ui_Flags |= SND_Cul_SF_FxA;
		gcSND_FxAddSB(pst_Soft);
	}
	else if(_i_Flag & SND_Cul_SBC_PlayFxB)
	{
		pst_Soft->i_FxVol = _i_FxVol;
		pst_Soft->ui_Flags |= SND_Cul_SF_FxB;
		gcSND_FxAddSB(pst_Soft);
	}
	else
	{
		gcSND_FxDelSB(pst_Soft);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SoftStop(gcSND_tdst_SoftBuffer *pst_Soft)
{
	u16 * volatile pstate;

	if(pst_Soft->ast_Hard[0].pst_AxBuffer)
	{
		MIXSetInput(pst_Soft->ast_Hard[0].pst_AxBuffer, -960);

		if(pst_Soft->ast_Hard[0].pst_AxBuffer->pb.state != AX_PB_STATE_STOP)
		{
			AXSetVoiceState(pst_Soft->ast_Hard[0].pst_AxBuffer, AX_PB_STATE_STOP);
			pstate = &pst_Soft->ast_Hard[0].pst_AxBuffer->pb.state;
			gcSND_M_WaitWhile(*pstate != AX_PB_STATE_STOP);
		}

		gcSND_M_Assert((pst_Soft->ui_Flags & SND_Cte_StreamedFile) == 0);

		MIXReleaseChannel(pst_Soft->ast_Hard[0].pst_AxBuffer);
		AXFreeVoice(pst_Soft->ast_Hard[0].pst_AxBuffer);
		pst_Soft->ast_Hard[0].pst_AxBuffer = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SoftRelease(gcSND_tdst_SoftBuffer *pst_Soft)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < 2; i++)
	{
		if(pst_Soft->ast_Hard[i].pst_AxBuffer)
		{
			MIXReleaseChannel(pst_Soft->ast_Hard[i].pst_AxBuffer);
			AXFreeVoice(pst_Soft->ast_Hard[i].pst_AxBuffer);
			pst_Soft->ast_Hard[i].pst_AxBuffer = NULL;
		}
	}

	pst_Soft->ui_UserNb--;
	if(pst_Soft->pst_DuplicateSrc)
	{
		pst_Soft->pst_DuplicateSrc->ui_UserNb--;
	}
	else
	{
		if(pst_Soft->ui_UserNb)
		{
			gcSND_M_Err(gcSND_SoftRelease, "Bad command : try to release a duplicated SB");
		}
		else
		{
			if(pst_Soft->pc_AramBuffer) gcSND_FreeARAM(pst_Soft->pc_AramBuffer);
			if(pst_Soft->pc_RamBuffer)
			{
				MEM_Free(pst_Soft->pc_RamBuffer);
#ifdef _DEBUG
				if(!MEM_IsTmpPointer(pst_Soft->pc_RamBuffer))
				{
					gcSND_gui_AllocSize -= lRoudUp64(pst_Soft->ui_DataSize);
				}
#endif
			}
		}
	}

	gcSND_FreeSoftBuffer(pst_Soft);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SoftStreamRelease(gcSND_tdst_SoftBuffer *pst_Soft)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < 2; i++)
	{
		if(pst_Soft->ast_Hard[i].pst_AxBuffer)
		{
			MIXReleaseChannel(pst_Soft->ast_Hard[i].pst_AxBuffer);
			AXFreeVoice(pst_Soft->ast_Hard[i].pst_AxBuffer);
			pst_Soft->ast_Hard[i].pst_AxBuffer = NULL;
		}
	}

	pst_Soft->ui_UserNb--;
	gcSND_M_Assert(pst_Soft->ui_UserNb == 0);

	if(pst_Soft->pc_AramBuffer) gcSND_StreamFreeARAM(pst_Soft->pc_AramBuffer);

	if(pst_Soft->pc_RamBuffer)
	{
		MEM_Free(pst_Soft->pc_RamBuffer);
#ifdef _DEBUG
		if(!MEM_IsTmpPointer(pst_Soft->pc_RamBuffer))
		{
			gcSND_gui_AllocSize -= (SND_Cte_MaxBufferSize / 2);
		}
#endif
	}

	gcSND_FreeSoftBuffer(pst_Soft);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
gcSND_tdst_SoftBuffer *gcSND_pst_SoftDuplicate(gcSND_tdst_SoftBuffer *pst_SrcSoft)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_SoftBuffer	*pst_DstSoft;
	int 					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_SrcSoft) return NULL;

	/* get ressource */
	pst_DstSoft = gcSND_pst_AllocSoftBuffer();
	if(!pst_DstSoft)
	{
		gcSND_M_Err(gcSND_pst_SoftDuplicate, "No More ressource");
		return NULL;
	}

	/* set params */
	pst_DstSoft->i_Pan = 0;
	pst_DstSoft->i_SPan = 0;
	pst_DstSoft->i_Volume = SND_Cte_MinAtt;
	pst_DstSoft->i_Frequency = (int) fRand
		(
			0.9f * (float) pst_SrcSoft->i_Frequency,
			1.1f * (float) pst_SrcSoft->i_Frequency
		);

	pst_DstSoft->ast_Hard[0].i_MIXinput = gcSND_Cte_DefaultInput;
	pst_DstSoft->ast_Hard[1].i_MIXinput = gcSND_Cte_DefaultInput;
	for(i = 0; i < SND_Cte_FxCoreNb;++i)
		pst_DstSoft->i_MIXaux[i] = gcSND_Cte_MinVol;	

	pst_DstSoft->ast_Hard[0].i_MIXpan = gcSND_Cte_DefaultPan;
	pst_DstSoft->ast_Hard[1].i_MIXpan = gcSND_Cte_DefaultPan;
	pst_DstSoft->i_MIXspan = gcSND_Cte_DefaultSPan;
	pst_DstSoft->f_SRCratio = fGetSRCRatioFromFreq(pst_DstSoft->i_Frequency);

	pst_DstSoft->pc_AramBuffer = pst_SrcSoft->pc_AramBuffer;
	pst_DstSoft->pc_RamBuffer = NULL;

	pst_DstSoft->ui_DataPosition = pst_SrcSoft->ui_DataPosition;
	pst_DstSoft->ui_DataSize = pst_SrcSoft->ui_DataSize;

	pst_DstSoft->pst_DuplicateSrc = pst_SrcSoft;
	pst_DstSoft->ui_UserNb = 1;
	pst_SrcSoft->ui_UserNb++;

	pst_DstSoft->ast_Hard[0].pst_AxBuffer = NULL;
	pst_DstSoft->ast_Hard[1].pst_AxBuffer = NULL;

	pst_DstSoft->ast_Hard[0].st_AxAddrSettings.loopFlag = AXPBADDR_LOOP_OFF;
	pst_DstSoft->ast_Hard[0].st_AxAddrSettings.format = pst_SrcSoft->ast_Hard[0].st_AxAddrSettings.format;

	pst_DstSoft->ast_Hard[0].st_AxAddrSettings.loopAddressHi = sGetStartAddrHi(gcSND_pv_GetZeroBuffer());
	pst_DstSoft->ast_Hard[0].st_AxAddrSettings.loopAddressLo = sGetStartAddrLo(gcSND_pv_GetZeroBuffer());

	pst_DstSoft->ast_Hard[0].st_AxAddrSettings.endAddressHi = pst_SrcSoft->ast_Hard[0].st_AxAddrSettings.endAddressHi;
	pst_DstSoft->ast_Hard[0].st_AxAddrSettings.endAddressLo = pst_SrcSoft->ast_Hard[0].st_AxAddrSettings.endAddressLo;

	pst_DstSoft->ast_Hard[0].st_AxAddrSettings.currentAddressHi = sGetStartAddrHi(pst_DstSoft->pc_AramBuffer);
	pst_DstSoft->ast_Hard[0].st_AxAddrSettings.currentAddressLo = sGetStartAddrLo(pst_DstSoft->pc_AramBuffer);

	pst_DstSoft->ast_Hard[0].pred_scale = pst_SrcSoft->ast_Hard[0].pred_scale;
	return pst_DstSoft;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SoftPause(gcSND_tdst_SoftBuffer *pst_Soft)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < 2; i++)
	{
		if(pst_Soft->ast_Hard[i].pst_AxBuffer)
		{
			if(pst_Soft->ast_Hard[i].pst_AxBuffer->pb.state == AX_PB_STATE_RUN)
				AXSetVoiceState(pst_Soft->ast_Hard[i].pst_AxBuffer, AX_PB_STATE_STOP);
			else
				AXSetVoiceState(pst_Soft->ast_Hard[i].pst_AxBuffer, AX_PB_STATE_RUN);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SoftLoadData(gcSND_tdst_SoftBuffer *pst_Soft)
{
	if(pst_Soft->ui_DataPosition)
	{
		L_memset(pst_Soft->pc_RamBuffer, 0, lRoudUp64(pst_Soft->ui_DataSize));
		if(!LOA_IsBinaryData())
		{
			BIG_ReadNoSeek(pst_Soft->ui_DataPosition, pst_Soft->pc_RamBuffer, pst_Soft->ui_DataSize);
		}
		else
		{
			/*~~~~~~~~~~~~~*/
			char	*pc_Buff;
			/*~~~~~~~~~~~~~*/

			pc_Buff = LOA_FetchBuffer(pst_Soft->ui_DataSize);
			LOA_ReadCharArray(&pc_Buff, pst_Soft->pc_RamBuffer, pst_Soft->ui_DataSize);
		}

		DCFlushRange(pst_Soft->pc_RamBuffer, lRoudUp64(pst_Soft->ui_DataSize));

		pst_Soft->ast_Hard[0].pred_scale = (short) (*pst_Soft->pc_RamBuffer);
		pst_Soft->pst_SndFileHandler = NULL;

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
	}
	else
	{
		pst_Soft->pst_SndFileHandler = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_SoftWaitEndOfLoadings(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL					b_finished;
	int						i;
	gcSND_tdst_SoftBuffer	*pst_SoftBuffer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(1)
	{
		b_finished = TRUE;

		for(i = 0; i < SND_Cte_MaxSoftBufferNb; i++)
		{
			pst_SoftBuffer = gcSND_gax_SoftBuffer + i;
			
			if((pst_SoftBuffer->ui_Flags & SND_Cul_DSF_Used) == 0) continue;

			/* skip stream */
			if(pst_SoftBuffer->ui_Flags & SND_Cte_StreamedFile) continue;

			/* skip wac case */
			if(!pst_SoftBuffer->ui_DataPosition) continue;

			/* skip loaded wav */
			if(!pst_SoftBuffer->pc_RamBuffer) continue;

			/* exit loop */
			b_finished = FALSE;
			break;
		}

		if(b_finished) break;
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* _GAMECUBE */
