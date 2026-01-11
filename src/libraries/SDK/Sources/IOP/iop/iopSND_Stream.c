/*$T iopSND_Stream.c GC 1.138 06/30/04 12:24:15 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    Headers
 ***********************************************************************************************************************
 */

/*$2- sce ------------------------------------------------------------------------------------------------------------*/

#include <libsd.h>
#include <stdio.h>
#include <memory.h>
#include <kernel.h>
#include <sys/file.h>

/*$2- iop ------------------------------------------------------------------------------------------------------------*/

#include "iopDebug.h"
#include "iopMEM.h"
#include "iopCLI.h"
#include "iop/iopBAStypes.h"
#include "CDV_Manager.h"
#include "RPC_Manager.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "Sound/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/ps2/ps2SND.h"
#include "iopSND.h"
#include "iopSND_Voice.h"
#include "iopSND_Hard.h"
#include "iopSND_Fx.h"
#include "iopSND_libsd.h"
#include "iopSND_DmaScheduler.h"
#include "iopSND_Debug.h"
#include "iopSND_Stream.h"

#include "Sound/Sources/SNDstream.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define M_VoiceForPrefetch	0

//#define TRACE_PREFETCH 
#ifdef TRACE_PREFETCH
#define M_Trace(...)	printf(__VA_ARGS__)
#else
#define M_Trace(...)
#endif

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef enum	iopSND_tden_StreamPrefetchState_
{
	Prefetch_e_Free				= 0,
	Prefetch_e_WaitingForLoading= 1,
	Prefetch_e_Loaded			= 2,
	Prefetch_e_Dummy32bits_		= 0xFFFFFFFF
} iopSND_tden_StreamPrefetchState;

typedef struct	iopSND_tdst_StreamPrefetch_
{
	iopSND_tden_StreamPrefetchState en_State;
	unsigned int					ui_Position;
	unsigned int					ui_Size;
	char							*pc_Buffer;
} iopSND_tdst_StreamPrefetch;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

iopSND_tdst_StreamPrefetch	iopSDN_gast_StreamPrefetchArray[SND_Cte_StreamPrefetchMax];

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    module
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_InitStreamModule(void)
{
	L_memset(iopSDN_gast_StreamPrefetchArray, 0, SND_Cte_MaxSimultaneousStream * sizeof(iopSND_tdst_StreamPrefetch));
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_CloseStreamModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	iopSND_RequestDelete(M_VoiceForPrefetch);
	for(i = 0; i < SND_Cte_MaxSimultaneousStream; i++)
	{
		if(iopSDN_gast_StreamPrefetchArray[i].pc_Buffer)
			iopMEM_iopFreeAlign(iopSDN_gast_StreamPrefetchArray[i].pc_Buffer);

		iopSDN_gast_StreamPrefetchArray[i].pc_Buffer = NULL;
	}

	L_memset(iopSDN_gast_StreamPrefetchArray, 0, SND_Cte_MaxSimultaneousStream * sizeof(iopSND_tdst_StreamPrefetch));

	return 0;
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	iopSND_i_CreateStreamBuffer_VArgs_
{
	int		i_ID;
	char	*pc_SpuBuffer;
} iopSND_i_CreateStreamBuffer_VArgs;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_CreateStreamBuffer(void *_pv_VArgs)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_i_CreateStreamBuffer_VArgs	*pVArgs;
	iopSND_tdst_SoftBuffer				*pSB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pVArgs = (iopSND_i_CreateStreamBuffer_VArgs *) _pv_VArgs;
	iopSND_M_CheckStreamBuffIdxOrReturn(pVArgs->i_ID, -1);

	iopDbg_M_AssertX((pVArgs->pc_SpuBuffer != NULL), iopDbg_Err_0078 "-stream1- S%08X", pVArgs->i_ID);
	pVArgs->i_ID = ps2SND_M_GetIdxFromSB(pVArgs->i_ID);

	__SoftLock__;
	pSB = (iopSND_tdst_SoftBuffer *) &iopSND_sa_SoftBuffer[pVArgs->i_ID];
	pSB->pc_SpuMemAddr = pVArgs->pc_SpuBuffer;
	pSB->ui_Flag = iopSND_Cte_Stream;
	iopSND_M_SetState(pSB->ui_Flag, iopSND_Cte_SndFlg_Stop);
	pSB->us_Pitch = 0;
	pSB->i_LoopCount = 0;
	pSB->ui_DuplicateNb = 0;
	pSB->p_DuplicateSrc = NULL;
	pSB->pc_DbleBuff = NULL;
	pSB->ui_DbleBuffSize = SND_Cte_MaxBufferSize >> 1;
	pSB->ui_DataPos = 0;
	pSB->ui_FileSeek = 0;
	pSB->pst_HardBuffer = NULL;
	pSB->us_LeftVolume = 0;
	pSB->us_RightVolume = 0;
	pSB->pst_HardBufferLeft = NULL;
	pSB->ui_DataSize = 0;
	pSB->i_LastDma = -1;
	pSB->s_StrFileIdx = -1;
	__SoftUnlock__;

	iopDbg_M_MsgX(iopDbg_Msg_000C "-stream- S%08X", pVArgs->i_ID);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_ReinitAndPlayStreamLong(void *_pv_VArgs)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_ReinitAndPlayStreamLong_VArgs	*pVArgs;
	int										id;
	iopSND_tdst_ReinitAndPlayArg			*pSB;
	unsigned int							uiMask;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pVArgs = (iopSND_ReinitAndPlayStreamLong_VArgs *) _pv_VArgs;
	iopSND_M_CheckStreamBuffIdxOrReturn(pVArgs->i_ID, ;);

	id = ps2SND_M_GetIdxFromSB(pVArgs->i_ID);
	SPY(id, iopSND_ReinitAndPlayStreamLong);

	if((id <= 0) || (id >= (SND_Cte_MaxSimultaneousStream + 1)))
	{
		iopDbg_M_ErrX(iopDbg_Err_007B " S%08X", id);
		return;
	}

	uiMask = 0;
	if(pVArgs->c_DryMixL) uiMask |= iopSND_Cte_MixL;
	if(pVArgs->c_WetMixL) uiMask |= iopSND_Cte_FxMixL;
	if(pVArgs->c_DryMixR) uiMask |= iopSND_Cte_MixR;
	if(pVArgs->c_WetMixR) uiMask |= iopSND_Cte_FxMixR;

	__SoftLock__;
	pSB = (iopSND_tdst_ReinitAndPlayArg *) &iopSND_sa_StreamBuffer[id];
	pSB->ui_DataPos = pVArgs->ui_RPosition;
	pSB->ui_DataSize = pVArgs->ui_RSize;
	pSB->b_Stereo = ps2SND_M_IdIsStereo(pVArgs->i_ID) ? 1 : 0;
	pSB->i_LoopNb = pVArgs->i_LoopNb;
	pSB->ui_PlayFlags = (unsigned int) pVArgs->i_Flag;
	pSB->ui_ChainDelaySize = 0;
	pSB->i_Freq = pVArgs->ui_Frequency;
	pSB->i_LVol = pVArgs->ui_LVol;
	pSB->i_RVol = pVArgs->ui_RVol;
	pSB->ui_StartPos = pVArgs->ui_RStartPos;
	pSB->ui_StopPos = pVArgs->ui_RStopPos;
	pSB->ui_LoopBegin = pVArgs->ui_RLoopStart;
	pSB->ui_LoopEnd = pVArgs->ui_RLoopStop;
	pSB->b_Chain = 0;
	iopSND_sa_SoftBuffer[id].ui_Flag &= ~iopSND_Cte_StreamIsPlayingNow;
	iopSND_sa_SoftBuffer[id].ui_Flag &= ~iopSND_Cte_SndMsk_Mix;
	iopSND_sa_SoftBuffer[id].ui_Flag |= uiMask;
	__SoftUnlock__;

	iopSND_RequestRegister(id, iopSND_Cte_DmaWaitingStreamLoadAndPlay);

	iopDbg_M_MsgX(iopDbg_Msg_002A " S0x%08X loop %d", pVArgs->i_ID, pVArgs->i_Loop);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_ReinitAndPlayStream_r(int dma, int _i_SndBuffId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int							ui_Start, ui_Flags;
	int										i_Loop, i_LVol, i_RVol, i_Freq, i_Stereo;
	unsigned int							ui_DataSize, ui_DataPos;
	volatile iopSND_tdst_ReinitAndPlayArg	*pReiArg;
	volatile iopSND_tdst_SoftBuffer			*pSB;
	unsigned int							uiFx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#pragma unused(dma)
	SPY(_i_SndBuffId, iopSND_i_ReinitAndPlayStream_r);
	iopSND_RequestDelete(_i_SndBuffId);

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	__SoftLock__;
	pReiArg = &iopSND_sa_StreamBuffer[_i_SndBuffId];
	ui_DataPos = pReiArg->ui_DataPos;
	ui_DataSize = pReiArg->ui_DataSize;
	i_Loop = pReiArg->i_LoopNb;
	ui_Flags = pReiArg->ui_PlayFlags;
	i_Freq = pReiArg->i_Freq;
	i_LVol = pReiArg->i_LVol;
	i_RVol = pReiArg->i_RVol;
	i_Stereo = pReiArg->b_Stereo;
	ui_Start = pReiArg->ui_StartPos;

	pSB = &iopSND_sa_SoftBuffer[_i_SndBuffId];

	/*$2- reset the file param ---------------------------------------------------------------------------------------*/

	pSB->ui_DataPos = ui_DataPos;
	pSB->ui_DataSize = ui_DataSize;
	pSB->ui_FileSeek = pSB->ui_DataPos + ui_Start;
	pSB->ui_LoopBegin = pReiArg->ui_LoopBegin + pSB->ui_DataPos;
	pSB->ui_LoopEnd = pReiArg->ui_LoopEnd + pSB->ui_DataPos;
	/*$2- reset the sound buffer param -------------------------------------------------------------------------------*/

	pSB->us_Pitch = M_Freq2Pitch(i_Freq);
	pSB->us_LeftVolume = 0xFFFF & i_LVol;
	pSB->us_RightVolume = 0xFFFF & i_RVol;

	uiFx = pSB->ui_Flag;
	uiFx &= (iopSND_Cte_MixL | iopSND_Cte_MixR | iopSND_Cte_FxMixL | iopSND_Cte_FxMixR);

	pSB->ui_Flag = iopSND_Cte_Stream | uiFx;

	if(i_Stereo) pSB->ui_Flag |= iopSND_Cte_Stereo;
	pSB->i_LoopCount = i_Loop;
	if(ui_Flags & SND_Cul_SBC_PlayFxA) pSB->ui_Flag |= iopSND_Cte_UseFxA;
	if(ui_Flags & SND_Cul_SBC_PlayFxB) pSB->ui_Flag |= iopSND_Cte_UseFxB;
	__SoftUnlock__;
	

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(pSB->ui_Flag & iopSND_Cte_UseFxA)
		iopSND_FxAddVoice(_i_SndBuffId, SD_CORE_0);
	else if(pSB->ui_Flag & iopSND_Cte_UseFxB)
		iopSND_FxAddVoice(_i_SndBuffId, SD_CORE_1);
	else
		iopSND_FxDelVoice(_i_SndBuffId);

	return i_Stereo;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StreamChain(RPC_tdst_SndStreamChain *pChain)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int								iR;
	iopSND_tdst_ReinitAndPlayArg	*pReinit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	iopSND_M_CheckStreamBuffIdxOrReturn(pChain->i_ID, ;);
	iR = ps2SND_M_GetIdxFromSB(pChain->i_ID);
	SPY(iR, iopSND_StreamChain);

	if((iR <= 0) || (iR >= (SND_Cte_MaxSimultaneousStream + 1)))
	{
		iopDbg_M_ErrX(iopDbg_Err_007B " S%08X", iR);
		return;
	}

	__SoftLock__;
	pReinit = (iopSND_tdst_ReinitAndPlayArg *) &iopSND_sa_StreamBuffer[iR];
	pReinit->b_Chain = 1;
	pReinit->ui_DataPos = pChain->ui_NewPosition;
	pReinit->ui_DataSize = pChain->ui_NewSize;
	pReinit->i_LoopNb = pChain->i_LoopNb;
	pReinit->ui_PlayFlags = pChain->i_NewFlag;
	pReinit->ui_StopPos = pChain->ui_ExitPosition;
	pReinit->ui_StartPos = pChain->ui_EnterPosition;
	pReinit->ui_LoopBegin = pReinit->ui_DataPos + pChain->ui_NewLoopBegin;
	pReinit->ui_LoopEnd = pReinit->ui_DataPos + pChain->ui_NewLoopEnd;
	pReinit->ui_ChainDelaySize = 0;
	__SoftUnlock__;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StopAllStream(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < SND_Cte_MaxSimultaneousStream; i++)
	{
		iopSND_FxDelVoice(i + 1);
		iopSND_i_StopVoice(i + 1);
	}

	iopSND_StreamIopFlushAll();
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    PREFETCH
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StreamIopPrefetch(RPC_tdst_VArg_SndStreamPrefetch *pVArg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_tdst_StreamPrefetch	*pst_Prefetch;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    check arg
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(!pVArg) return;
	if(!pVArg->ui_Size) return;
	if(!pVArg->ui_Position) return;
	if(pVArg->ui_Position == -1) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    alloc elem
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = iopSDN_gast_StreamPrefetchArray + i;
		if(pst_Prefetch->en_State == Prefetch_e_Free) break;
	}

	if(i >= SND_Cte_StreamPrefetchMax)
	{
		iopDbg_M_Err(iopDbg_Err_0089);
		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    register the rq
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(pVArg->ui_Size > (SND_Cte_MaxBufferSize / 2))
	{
		pVArg->ui_Size = (SND_Cte_MaxBufferSize / 2);
		iopDbg_M_Err(iopDbg_Err_0089);
	}

	pst_Prefetch->pc_Buffer = iopMEM_pv_iopAllocAlign(pVArg->ui_Size, 64);

	if(!pst_Prefetch->pc_Buffer)
	{
		iopDbg_M_Err(iopDbg_Err_0089);
		return;
	}

	pst_Prefetch->ui_Position = pVArg->ui_Position;
	pst_Prefetch->ui_Size = pVArg->ui_Size;
	pst_Prefetch->en_State = Prefetch_e_WaitingForLoading;

	M_Trace("IOP prefetch pos %d\n", pst_Prefetch->ui_Position);
	iopSND_RequestWrite(M_VoiceForPrefetch, iopSND_Cte_DmaWaitingStreamPrefetch);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StreamIopPrefetchStatus(unsigned int *auiPos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_tdst_StreamPrefetch	*pst_Prefetch;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = iopSDN_gast_StreamPrefetchArray + i;
		if(pst_Prefetch->en_State == Prefetch_e_Loaded)
		{
			auiPos[i] = pst_Prefetch->ui_Position;
		}
		else
		{
			auiPos[i] = 0;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StreamIopFlush(RPC_tdst_VArg_SndStreamFlush *pVArg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_tdst_StreamPrefetch	*pst_Flush;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pVArg) return;
	if(!pVArg->ui_Position) return;
	if(pVArg->ui_Position == -1) return;

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Flush = iopSDN_gast_StreamPrefetchArray + i;
		if(pst_Flush->ui_Position == pVArg->ui_Position)
		{
			M_Trace("IOP flush pos %d\n", pst_Flush->ui_Position);
			CLI_WaitSema(iopCLI_Sema);
			pst_Flush->en_State = Prefetch_e_Free;
			pst_Flush->ui_Position = 0;
			CLI_SignalSema(iopCLI_Sema);
			iopMEM_iopFreeAlign(pst_Flush->pc_Buffer);
			pst_Flush->pc_Buffer = NULL;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StreamPrefetchDo(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_tdst_StreamPrefetch	*pst_Prefetch;
	int							i;
	int							res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CLI_WaitSema(iopCLI_Sema);
	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = iopSDN_gast_StreamPrefetchArray + i;
		if((pst_Prefetch->en_State == Prefetch_e_WaitingForLoading) && pst_Prefetch->pc_Buffer)
		{
			L_fseeknosema(iopSND_vi_BigFd, SEEK_SET, pst_Prefetch->ui_Position);
			res = L_freadnosema(iopSND_vi_BigFd, pst_Prefetch->pc_Buffer, pst_Prefetch->ui_Size);
			if(res) L_memset(pst_Prefetch->pc_Buffer, 0, pst_Prefetch->ui_Size);
			pst_Prefetch->en_State = Prefetch_e_Loaded;
			break;
		}
	}

	CLI_SignalSema(iopCLI_Sema);
	M_Trace("IOP prefetch done %d\n", pst_Prefetch->ui_Position);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int iopSND_ui_StreamPrefetchGet(char *_pc_Buffer, unsigned int _ui_Position, unsigned int _ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_tdst_StreamPrefetch	*pst_Prefetch;
	int							i;
	unsigned int				uiOffset, uiSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pc_Buffer) return 0;
	if(!_ui_Position) return 0;

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = iopSDN_gast_StreamPrefetchArray + i;

		if(pst_Prefetch->en_State != Prefetch_e_Loaded) continue;
		if(_ui_Position < pst_Prefetch->ui_Position) continue;
		if((pst_Prefetch->ui_Position + pst_Prefetch->ui_Size) < _ui_Position) continue;

		uiOffset = _ui_Position - pst_Prefetch->ui_Position;
		uiSize = pst_Prefetch->ui_Position + pst_Prefetch->ui_Size - _ui_Position;
		uiSize = _ui_Size < uiSize ? _ui_Size : uiSize;

		M_Trace("IOP prefetch get %d\n", uiSize);

		L_memcpy(_pc_Buffer, pst_Prefetch->pc_Buffer + uiOffset, uiSize);

		pst_Prefetch->en_State = Prefetch_e_Free;
		pst_Prefetch->ui_Position = 0;
		iopMEM_iopFreeAlign(pst_Prefetch->pc_Buffer);
		pst_Prefetch->pc_Buffer = NULL;

		return uiSize;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_StreamPrefetchArray(RPC_tdst_VArg_SndStreamPrefetch *ppVArg, unsigned int *pui_Res)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_tdst_StreamPrefetch	*pst_Prefetch;
	int							i, j;
	int							res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(pui_Res, 0, SND_Cte_StreamPrefetchMax * sizeof(unsigned int));

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    check arg
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(!ppVArg[i].ui_Size) continue;
		if(!ppVArg[i].ui_Position) continue;

		if(ppVArg[i].ui_Size > (SND_Cte_MaxBufferSize / 2))
		{
			ppVArg[i].ui_Size > (SND_Cte_MaxBufferSize / 2);
			iopDbg_M_Err(iopDbg_Err_0089);
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    alloc elem
		 ---------------------------------------------------------------------------------------------------------------
		 */

		for(j = 0; j < SND_Cte_StreamPrefetchMax; j++)
		{
			pst_Prefetch = iopSDN_gast_StreamPrefetchArray + j;
			if(pst_Prefetch->en_State == Prefetch_e_Free) break;
		}

		if(j >= SND_Cte_StreamPrefetchMax)
		{
			pui_Res[i] = 3;
			iopDbg_M_Err(iopDbg_Err_0089);
			continue;
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    register the rq
		 ---------------------------------------------------------------------------------------------------------------
		 */

		pst_Prefetch->pc_Buffer = iopMEM_pv_iopAllocAlign(ppVArg[i].ui_Size, 64);
		if(!pst_Prefetch->pc_Buffer)
		{
			pui_Res[i] = 4;
			iopDbg_M_Err(iopDbg_Err_0089);
			continue;
		}

		pst_Prefetch->ui_Position = ppVArg[i].ui_Position;
		pst_Prefetch->ui_Size = ppVArg[i].ui_Size;
		pst_Prefetch->en_State = Prefetch_e_WaitingForLoading;

		M_Trace("IOP prefetchA pos %d\n", pst_Prefetch->ui_Position);

		CLI_WaitSema(iopCLI_Sema);
		L_fseeknosema(iopSND_vi_BigFd, SEEK_SET, pst_Prefetch->ui_Position);
		res = L_freadnosema(iopSND_vi_BigFd, pst_Prefetch->pc_Buffer, pst_Prefetch->ui_Size);
		CLI_SignalSema(iopCLI_Sema);

		if(res) L_memset(pst_Prefetch->pc_Buffer, 0, pst_Prefetch->ui_Size);
		pst_Prefetch->en_State = Prefetch_e_Loaded;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StreamIopFlushAll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_tdst_StreamPrefetch	*pst_Prefetch;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CLI_WaitSema(iopCLI_Sema);
	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = iopSDN_gast_StreamPrefetchArray + i;
		if(pst_Prefetch->pc_Buffer)
		{
			iopMEM_iopFreeAlign(pst_Prefetch->pc_Buffer);
		}

		M_Trace("IOP flushall pos %d\n", pst_Prefetch->ui_Position);
		pst_Prefetch->pc_Buffer = NULL;
		pst_Prefetch->ui_Position = 0;
		pst_Prefetch->ui_Size = 0;
		pst_Prefetch->en_State = Prefetch_e_Free;
	}

	CLI_SignalSema(iopCLI_Sema);

	iopSND_RequestReplace(M_VoiceForPrefetch, iopSND_Cte_DmaWaitingStreamPrefetch, iopSND_Cte_DmaCancel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StreamSetPos(void *p)
{
#pragma unused(p)
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StreamGetPos(int id, void *p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RPC_tdst_SndStreamSetPos	*pArg;
	int							idR;
	unsigned int				rNAX;
	unsigned int				ui_Flag, ui_DbleBuffSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	idR = ps2SND_M_GetIdxFromSB(id);
	iopSND_M_CheckSoftBuffIdxOrReturn(idR, ;);

	ui_Flag = iopSND_sa_SoftBuffer[idR].ui_Flag;
	ui_DbleBuffSize = iopSND_sa_SoftBuffer[idR].ui_DbleBuffSize;

	pArg = (RPC_tdst_SndStreamSetPos *) p2;
	pArg->ui_PlayLpos = 0;
	pArg->ui_PlayRpos = 0;
	pArg->ui_WriteLpos = 0;
	pArg->ui_WriteRpos = 0;

	rNAX = (unsigned int) iopSND_i_HardGetNAX((iopSND_tdst_HardBuffer *) iopSND_sa_SoftBuffer[idR].pst_HardBuffer);
	if(rNAX != -1)
	{
		rNAX = rNAX - (unsigned int) iopSND_sa_SoftBuffer[idR].pc_SpuMemAddr;

		pArg->ui_WriteRpos = iopSND_sa_SoftBuffer[idR].ui_FileSeek - iopSND_sa_SoftBuffer[idR].ui_DataPos;
		pArg->ui_PlayRpos = rNAX;

		if((idR <= SND_Cte_MaxSimultaneousStream) && (iopSND_vai_StreamBufferCpt[idR - 1].i_ChangeCount))
		{
			pArg->ui_PlayRpos += iopSND_vai_StreamBufferCpt[idR - 1].i_ChangeCount * ui_DbleBuffSize;
			if(rNAX >= ui_DbleBuffSize) pArg->ui_PlayRpos -= ui_DbleBuffSize;

			while(pArg->ui_WriteRpos > (pArg->ui_PlayRpos + ui_DbleBuffSize))
			{
				pArg->ui_PlayRpos += ui_DbleBuffSize;
			}
		}
	}
	
	if(ps2SND_M_IdIsStereo(id))
	{
		pArg->ui_PlayLpos = pArg->ui_PlayRpos;
		pArg->ui_WriteLpos = pArg->ui_WriteRpos;
	}
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StreamLoopCountGet(int id, int *piLoop)
{
	id = ps2SND_M_GetIdxFromSB(id);
	iopSND_M_CheckSoftBuffIdxOrReturn(id, ;);
	*piLoop = iopSND_sa_SoftBuffer[id].i_LoopCount;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_StreamChainDelayGet(int id, int *piSize)
{
	id = ps2SND_M_GetIdxFromSB(id);
	iopSND_M_CheckSoftBuffIdxOrReturn(id, ;);
	*piSize = *(int*)&iopSND_sa_StreamBuffer[id].ui_ChainDelaySize;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
