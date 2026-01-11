/*$T iopSND_Voice.c GC 1.138 05/27/04 16:15:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- sce ------------------------------------------------------------------------------------------------------------*/

#include <libsd.h>
#include <stdio.h>
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
#include "iopKernel.h"

/*$4
 ***********************************************************************************************************************
    constantes
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    public variables
 ***********************************************************************************************************************
 */

int										iopSND_gi_SoftLock;
volatile iopSND_tdst_SoftBuffer			iopSND_sa_SoftBuffer[SND_Cte_MaxSoftBufferNb];
volatile iopSND_tdst_ReinitAndPlayArg	iopSND_sa_StreamBuffer[SND_Cte_MaxSimultaneousStream + 1];
volatile int							iopSND_vi_RPCThreadIsWaiting;
extern volatile int						iopSND_gi_SeekLock;
volatile int                            iopSND_gi_OneShotPending;

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

/*$2- soft buffer ----------------------------------------------------------------------------------------------------*/

void	iopSND_CleanBuffer(int _i_Snd);

/*$2- recursive fct --------------------------------------------------------------------------------------------------*/

int			iopSND_i_CreateVoiceBuffer_r
			(
				int				ID,
				unsigned int	ui_Position,
				unsigned int	ui_Size,
				char *,
				char *,
				unsigned int ui_Frequency,
				int i_LoadNow
			);
static int	iopSND_i_PlayVoice_r(int _i_SoftId);
int			iopSND_i_GetVoiceStatus_r(int _i_Voice);

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_InitVoiceModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	iopDbg_M_Msg(iopDbg_Msg_000A);

	L_memset(iopSND_sa_StreamBuffer, 0, (SND_Cte_MaxSimultaneousStream + 1) * sizeof(iopSND_tdst_ReinitAndPlayArg));
	L_memset(iopSND_sa_SoftBuffer, 0, SND_Cte_MaxSoftBufferNb * sizeof(iopSND_tdst_SoftBuffer));

	/* special case for 0 */
	iopSND_sa_SoftBuffer[0].ui_Flag = iopSND_Cte_SndFlg_Free;

	/* all other id : */
	for(i = 1; i < SND_Cte_MaxSoftBufferNb; i++)
	{
		iopSND_CleanBuffer(i);
	}

	iopSND_FxInitModule();
	iopSND_DmaSchedulerInitModule();
	iopSND_HardInitModule();
	iopSND_vi_RPCThreadIsWaiting = 0;
	iopSND_gi_OneShotPending =0;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_CloseVoiceModule(void)
{
	iopDbg_M_Msg(iopDbg_Msg_0018);
	iopSND_DmaSchedulerCloseModule();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_ReloadBuffer_r(int i_voice)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			ui_Position;
	unsigned int			ui_Size;
	char					*pc;
	iopSND_tdst_SoftBuffer	*pSB;
	int						res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	__SoftLock__;
	pSB = (iopSND_tdst_SoftBuffer *) &iopSND_sa_SoftBuffer[i_voice];
	ui_Position = pSB->ui_DataPos;
	ui_Size = pSB->ui_DataSize;
	pc = pSB->pc_DbleBuff;
	__SoftUnlock__;

	if(!pc) return -1;
	res = L_seekandread(iopSND_vi_BigFd_Snd, ui_Position, pc, ui_Size);
	if(res)
	{
		L_memset(pc, 0, ui_Size);
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_ReloadBuffer(RPC_tdst_SndCreateBuffer *pVArgs)
{
	/*~~~*/
	int R;
	int nR;
	/*~~~*/

	iopSND_M_CheckSoftBuffIdxOrReturn(pVArgs->ui_ID, -1);
	R = pVArgs->ui_ID;
	nR = iopSND_i_CreateVoiceBuffer_r
		(
			R,
			pVArgs->ui_RPosition,
			pVArgs->ui_RSize,
			pVArgs->pc_RSpuBuffer,
			pVArgs->pc_RIopBuffer,
			pVArgs->ui_Frequency,
			0
		);

	if(nR < 0)
	{
		iopSND_CleanBuffer(R);
		iopDbg_M_Err(iopDbg_Err_007A "-2");
		return -1;
	}

	iopDbg_M_MsgX(iopDbg_Msg_000C "S0x%08X", R);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_CreateVoiceBuffer(RPC_tdst_SndCreateBuffer *pVArgs)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int			R;
	int			nR;
	static int	sleep = 0;
	/*~~~~~~~~~~~~~~~~~~*/

	/*$2- check args -------------------------------------------------------------------------------------------------*/

	iopSND_M_CheckSoftBuffIdxOrReturn(pVArgs->ui_ID, -1);

	R = pVArgs->ui_ID;
	nR = iopSND_i_CreateVoiceBuffer_r
		(
			R,
			pVArgs->ui_RPosition,
			pVArgs->ui_RSize,
			pVArgs->pc_RSpuBuffer,
			pVArgs->pc_RIopBuffer,
			pVArgs->ui_Frequency,
			1
		);

	if(nR < 0)
	{
		iopSND_CleanBuffer(R);
		iopDbg_M_Err(iopDbg_Err_007A "-2");
		return -1;
	}


	while((iopSND_sa_SoftBuffer[R].ui_Flag & iopSND_Cte_Loaded) == 0)
	{
		iopSND_vi_RPCThreadIsWaiting = 1;
		L_SleepThread();
	}

	iopDbg_M_MsgX(iopDbg_Msg_000C "S0x%08X", R);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_CreateVoiceBuffer_r
(
	int				ID,
	unsigned int	ui_Position,
	unsigned int	ui_Size,
	char			*pc_SpuBuffer,
	char			*pc_IopBuffer,
	unsigned int	ui_Frequency,
	int				i_LoadNow
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*pc;
	iopSND_tdst_SoftBuffer	*pSB;
	iopSND_tdst_HardBuffer *pst_HardBuffer;
	unsigned int	ui_FileSeek = ui_Position;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

#pragma unused(pc_IopBuffer)
	SPY(ID, iopSND_i_CreateVoiceBuffer_r);
	iopDbg_M_AssertX((pc_SpuBuffer != NULL), iopDbg_Err_0078 " S%08X", ID);
	iopDbg_M_AssertX((ui_Position != 0), iopDbg_Err_0078 "-1 S%08X", ID);
	iopDbg_M_AssertX((ui_Size != 0), iopDbg_Err_0078 "-2 S%08X", ID);

	if(iopSND_i_LoadData(&ui_FileSeek , ui_Size, &pc, i_LoadNow) < 0)
	{
   		iopDbg_M_Err(iopDbg_Err_007A "-1");
   		return -1;
	}

    /* then go ahead */
	__SoftLock__;
	pSB = (iopSND_tdst_SoftBuffer *) &iopSND_sa_SoftBuffer[ID];
	pSB->pc_SpuMemAddr = pc_SpuBuffer;
	pSB->ui_Flag = 0;
	iopSND_M_SetState(pSB->ui_Flag, iopSND_Cte_SndFlg_Stop);
	pSB->us_Pitch = M_Freq2Pitch(ui_Frequency);
	pSB->ui_DuplicateNb = 0;
	pSB->i_LoopCount = 0;
	pSB->p_DuplicateSrc = NULL;
	pst_HardBuffer = (iopSND_tdst_HardBuffer*)pSB->pst_HardBuffer;
	
	if(pSB->pc_DbleBuff)
	{
		/* iopDbg_M_Err(iopDbg_Err_0086"-create"); */
		iopSND_RequestDelete(ID);
		iopMEM_iopFreeAlign(pSB->pc_DbleBuff);
	}

	pSB->pc_DbleBuff = NULL;//(char *) pc;
	pSB->ui_DbleBuffSize = ui_Size;
	pSB->ui_DataPos = ui_Position;
	pSB->ui_DataSize = ui_Size;
	pSB->ui_FileSeek = ui_FileSeek ; 
	if(pst_HardBuffer)
	{
		iopSND_HardSetVol(pst_HardBuffer, 0, 0);
		iopSND_HardStop(NULL, pst_HardBuffer);
		iopSND_HardFreeBuffer(pst_HardBuffer);
	}

	pSB->pst_HardBuffer = NULL;
	pSB->us_LeftVolume = 0;
	pSB->us_RightVolume = 0;
	pSB->pst_HardBufferLeft = NULL;
	pSB->i_LastDma = -1;
	__SoftUnlock__;

	if(i_LoadNow)
	{
	    iopSND_gi_OneShotPending++;
	    iopSND_RequestRegister(ID, iopSND_Cte_DmaWaitingOneShotLoading);
	}
	else
		iopSND_RequestRegister(ID, iopSND_Cte_DmaWaitingReload);
	return 0;
}

typedef struct	iopSND_i_DuplicateVoiceBuffer_VArgs_
{
	int i_Src;
	int i_Dst;
} iopSND_i_DuplicateVoiceBuffer_VArgs;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_DuplicateVoiceBuffer(void *_pv_VArgs)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_i_DuplicateVoiceBuffer_VArgs *pVArgs;
	int									iR;
	int									iDst;
	iopSND_tdst_SoftBuffer				*_Dst, *_Src;
	iopSND_tdst_HardBuffer				*pst_HardBuffer;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pVArgs = (iopSND_i_DuplicateVoiceBuffer_VArgs *) _pv_VArgs;
	iopSND_M_CheckSoftBuffIdxOrReturn(pVArgs->i_Src, -1);
	iopSND_M_CheckSoftBuffIdxOrReturn(pVArgs->i_Dst, -1);

	iR = ps2SND_M_GetIdxFromSB(pVArgs->i_Src);
	iDst = ps2SND_M_GetIdxFromSB(pVArgs->i_Dst);

	SPY(iR, iopSND_i_DuplicateVoiceBufferi);
	SPY(iDst, iopSND_i_DuplicateVoiceBuffero);

	_Dst = (iopSND_tdst_SoftBuffer *) &iopSND_sa_SoftBuffer[iDst];
	_Src = (iopSND_tdst_SoftBuffer *) &iopSND_sa_SoftBuffer[iR];

	__SoftLock__;

	_Dst->pc_SpuMemAddr = _Src->pc_SpuMemAddr;
	_Dst->ui_Flag = _Src->ui_Flag & (iopSND_Cte_Loaded | iopSND_Cte_UseFxA | iopSND_Cte_UseFxB);
	iopSND_M_SetState(_Dst->ui_Flag, iopSND_Cte_SndFlg_Stop);
	_Dst->us_Pitch = _Src->us_Pitch;
	_Dst->ui_DuplicateNb = 0;
	_Dst->i_LoopCount = 0;
	_Src->ui_DuplicateNb++;
	_Dst->p_DuplicateSrc = _Src;
	pst_HardBuffer = (iopSND_tdst_HardBuffer*)_Dst->pst_HardBuffer;
	
	if(_Dst->pc_DbleBuff)
	{
		iopDbg_M_Err(iopDbg_Err_0086"-duplicate"); 
		iopSND_RequestDelete(iDst);
		iopMEM_iopFreeAlign(_Dst->pc_DbleBuff);
	}

	_Dst->pc_DbleBuff = NULL;	/* _Src->pc_DbleBuff; */
	_Dst->ui_DbleBuffSize = _Src->ui_DbleBuffSize;
	_Dst->ui_DataPos = _Src->ui_DataPos;
	_Dst->ui_FileSeek = _Src->ui_FileSeek;
	if(pst_HardBuffer)
	{
		iopSND_HardSetVol(pst_HardBuffer, 0, 0);
		iopSND_HardStop(NULL, pst_HardBuffer);
		iopSND_HardFreeBuffer(pst_HardBuffer);
	}

	_Dst->pst_HardBuffer = NULL;
	_Dst->us_LeftVolume = 0;
	_Dst->us_RightVolume = 0;
	_Dst->pst_HardBufferLeft = NULL;
	_Dst->ui_DataSize = _Src->ui_DataSize;
	_Dst->i_LastDma = -1;
	__SoftUnlock__;

	iopDbg_M_MsgX(iopDbg_Msg_001B " S%08X <- S%08X", pVArgs->i_Dst, pVArgs->i_Src);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_StopVoice(int _i_Snd)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int					ui_Flag;
	volatile iopSND_tdst_SoftBuffer *pSB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_i_Snd = ps2SND_M_GetIdxFromSB(_i_Snd);
	iopSND_M_CheckSoftBuffIdxOrReturn(_i_Snd, -1);

	
	SPY(_i_Snd, iopSND_i_StopVoice);

	__SoftLock__;
	pSB = &iopSND_sa_SoftBuffer[_i_Snd];
	ui_Flag = pSB->ui_Flag;
	__SoftUnlock__;

	if(ui_Flag & iopSND_Cte_AutoStopped)
	{
		__SoftLock__;
		pSB->ui_Flag &= ~(iopSND_Cte_AutoStopped | iopSND_Cte_StreamIsPlayingNow);
		__SoftUnlock__;
		return 0;
	}


	if(ui_Flag & iopSND_Cte_SndFlg_Stop) return 0;

	iopSND_i_DirectStopVoice(_i_Snd);
	if(ui_Flag & iopSND_Cte_Stream)
	{
	//printf("stop demande %d\n", _i_Snd);
		iopSND_RequestRegister(_i_Snd, iopSND_Cte_DmaStopStream);
		iopSND_SchedulerResetBuffer(_i_Snd);
		__SoftLock__;
		pSB->ui_Flag &= ~(iopSND_Cte_Loaded | iopSND_Cte_StreamIsPlayingNow);
		__SoftUnlock__;
	}

	iopDbg_M_MsgX(iopDbg_Msg_000E " S0x%08X", _i_Snd);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_DirectStopVoice(int _i_Snd)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			ui_Flag;
	iopSND_tdst_HardBuffer	*pst_HardBuffer;
	iopSND_tdst_HardBuffer	*pst_HardBufferLeft;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//$1- get buffer + spy
	_i_Snd = ps2SND_M_GetIdxFromSB(_i_Snd);
	iopSND_M_CheckSoftBuffIdxOrReturn(_i_Snd, -1);
	
	iopDbg_M_MsgX(iopDbg_Msg_000E " direct S0x%08X", _i_Snd);
	SPY(_i_Snd, iopSND_i_DirectStopVoice);
	
	//$1- get buffer attributes
	__SoftLock__;
	ui_Flag = iopSND_sa_SoftBuffer[_i_Snd].ui_Flag;
	pst_HardBuffer = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[_i_Snd].pst_HardBuffer;
	pst_HardBufferLeft = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[_i_Snd].pst_HardBufferLeft;
	__SoftUnlock__;

	//$1- set vol to 0
	iopSND_HardSetVol(pst_HardBuffer, 0, 0);
	if(pst_HardBufferLeft) iopSND_HardSetVol(pst_HardBufferLeft, 0, 0);
	
	//$1- free stereo buffer
	if(ui_Flag & iopSND_Cte_Stream)
	{
		if(iopSND_sa_SoftBuffer[_i_Snd].s_StrFileIdx != -1)
		{
			IOP_StrFileFree(iopSND_sa_SoftBuffer[_i_Snd].s_StrFileIdx);
			__SoftLock__;
			iopSND_sa_SoftBuffer[_i_Snd].ui_Flag &= ~iopSND_Cte_Stereo;
			iopSND_sa_SoftBuffer[_i_Snd].s_StrFileIdx = -1;
			__SoftUnlock__;
		}
	}
	
	//$1- exit if yet stopped
	if(ui_Flag & iopSND_Cte_SndFlg_Stop) return 0;
	
	//$1- delete rq if buffer is streamed
	if(ui_Flag & iopSND_Cte_Stream)	iopSND_RequestDelete(_i_Snd);
		
	//$1- stop+free the (hard)buffer
	iopSND_HardStop(pst_HardBufferLeft, pst_HardBuffer);
	iopSND_FxDelHardBuffer(pst_HardBufferLeft, pst_HardBuffer);
	iopSND_HardFreeBuffer(pst_HardBuffer);
	if(pst_HardBufferLeft) iopSND_HardFreeBuffer(pst_HardBufferLeft);
	
	//$1- save the modif
	__SoftLock__;
	iopSND_sa_SoftBuffer[_i_Snd].ui_Flag &= ~(iopSND_Cte_EndRead|iopSND_Cte_EndPlayed0|iopSND_Cte_EndPlayed1 | iopSND_Cte_StreamIsPlayingNow);
	iopSND_M_SetState(iopSND_sa_SoftBuffer[_i_Snd].ui_Flag, iopSND_Cte_SndFlg_Stop);
	iopSND_sa_SoftBuffer[_i_Snd].pst_HardBuffer = NULL;
	iopSND_sa_SoftBuffer[_i_Snd].pst_HardBufferLeft = NULL;
	__SoftUnlock__;


	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_PlayNoLoopVoice(RPC_tdst_Play *pPlay)
{
	/*~~~~~~~*/
	int rid;
	int uiMask;
	/*~~~~~~~*/

	rid = ps2SND_M_GetIdxFromSB(pPlay->i_ID);
	iopSND_M_CheckSoftBuffIdxOrReturn(rid, -1);
	SPY(rid, iopSND_i_PlayNoLoopVoice);

	iopDbg_M_Assert(!(iopSND_sa_SoftBuffer[rid].ui_Flag & iopSND_Cte_Stream), iopDbg_Err_0081);

	uiMask = 0;
	if(pPlay->c_DryMixL) uiMask |= iopSND_Cte_MixL;
	if(pPlay->c_WetMixL) uiMask |= iopSND_Cte_FxMixL;
	if(pPlay->c_DryMixR) uiMask |= iopSND_Cte_MixR;
	if(pPlay->c_WetMixR) uiMask |= iopSND_Cte_FxMixR;

	__SoftLock__;
	iopSND_sa_SoftBuffer[rid].i_LoopCount = 0;
	iopSND_sa_SoftBuffer[rid].ui_Flag &= ~iopSND_Cte_SndMsk_Mix;
	iopSND_sa_SoftBuffer[rid].ui_Flag |= uiMask;
	__SoftUnlock__;

	return iopSND_i_PlayVoice(pPlay->i_ID);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_PlayLoopVoice(RPC_tdst_Play *pPlay)
{
	/*~~~~~~~*/
	int rid;
	int uiMask;
	/*~~~~~~~*/

	iopSND_M_CheckSoftBuffIdxOrReturn(pPlay->i_ID, -1);
	rid = ps2SND_M_GetIdxFromSB(pPlay->i_ID);

	SPY(rid, iopSND_i_PlayLoopVoice);
	iopDbg_M_Assert(!(iopSND_sa_SoftBuffer[rid].ui_Flag & iopSND_Cte_Stream), iopDbg_Err_0081);

	uiMask = 0;
	if(pPlay->c_DryMixL) uiMask |= iopSND_Cte_MixL;
	if(pPlay->c_WetMixL) uiMask |= iopSND_Cte_FxMixL;
	if(pPlay->c_DryMixR) uiMask |= iopSND_Cte_MixR;
	if(pPlay->c_WetMixR) uiMask |= iopSND_Cte_FxMixR;

	__SoftLock__;
	iopSND_sa_SoftBuffer[rid].i_LoopCount = -1;
	iopSND_sa_SoftBuffer[rid].ui_Flag &= ~iopSND_Cte_SndMsk_Mix;
	iopSND_sa_SoftBuffer[rid].ui_Flag |= uiMask;
	__SoftUnlock__;

	return iopSND_i_PlayVoice(pPlay->i_ID);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_PlayVoice(int _i_SoftId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_tdst_HardBuffer	*pst_HardBuffer;
	iopSND_tdst_HardBuffer	*pst_HardBufferLeft;
	unsigned int			ui_Flag;
	unsigned short			us_LeftVolume, us_RightVolume;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_i_SoftId = ps2SND_M_GetIdxFromSB(_i_SoftId);
	iopSND_M_CheckSoftBuffIdxOrReturn(_i_SoftId, -1);

	if(iopSND_sa_SoftBuffer[_i_SoftId].ui_Flag == iopSND_Cte_SndFlg_Free)
	{
		iopDbg_M_ErrX(iopDbg_Err_005E " S0x%08X", _i_SoftId);
		return -1;
	}

	SPY(_i_SoftId, iopSND_i_PlayVoice);
	if(!iopSND_i_PlayVoice_r(_i_SoftId))
	{
		__SoftLock__;
		pst_HardBuffer = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[_i_SoftId].pst_HardBuffer;
		pst_HardBufferLeft = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[_i_SoftId].pst_HardBufferLeft;
		ui_Flag = iopSND_sa_SoftBuffer[_i_SoftId].ui_Flag;
		us_LeftVolume = iopSND_sa_SoftBuffer[_i_SoftId].us_LeftVolume;
		us_RightVolume = iopSND_sa_SoftBuffer[_i_SoftId].us_RightVolume;
		__SoftUnlock__;

		iopSND_FxAddHardBuffer(ui_Flag, pst_HardBufferLeft, pst_HardBuffer);
		iopSND_HardPlay(pst_HardBufferLeft, pst_HardBuffer);
		iopDbg_M_MsgX(iopDbg_Msg_000F " S0x%08X", _i_SoftId);

		if(ui_Flag & iopSND_Cte_Stream)
		{
			__SoftLock__;
			iopSND_sa_SoftBuffer[_i_SoftId].ui_Flag |= iopSND_Cte_StreamIsPlayingNow;
			__SoftUnlock__;
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int iopSND_i_PlayVoice_r(int _i_SoftId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int					ui_Flag;
	unsigned int					ui_TrueFlag, ui_DbleBufferSize;
	iopSND_tdst_HardBuffer			*pst_HardBuffer;
	iopSND_tdst_HardBuffer			*pst_HardBufferLeft;
	unsigned short					us_Pitch, us_LeftVolume, us_RightVolume;
	char							*pc_SpuMemAddr;
	volatile iopSND_tdst_SoftBuffer *pSB;
	int								core, loop;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	__SoftLock__;
	pSB = &iopSND_sa_SoftBuffer[_i_SoftId];
	ui_Flag = pSB->ui_Flag;
	pst_HardBuffer = (iopSND_tdst_HardBuffer*)pSB->pst_HardBuffer;
	pst_HardBufferLeft = (iopSND_tdst_HardBuffer*)pSB->pst_HardBufferLeft;
	us_Pitch = pSB->us_Pitch;
	us_LeftVolume = pSB->us_LeftVolume;
	us_RightVolume = pSB->us_RightVolume;
	pc_SpuMemAddr = pSB->pc_SpuMemAddr;
	ui_DbleBufferSize = pSB->ui_DbleBuffSize;
	loop = pSB->i_LoopCount;
	__SoftUnlock__;

	ui_TrueFlag = ui_Flag;

	if(ui_Flag & iopSND_Cte_Stream)
	{
		if((ui_Flag & iopSND_Cte_Loaded) == 0)
		{
			iopSND_RequestRegister(_i_SoftId, iopSND_Cte_DmaPlayRq);
			return 1;
		}
	}
	else
	{
		__SoftLock__;
		ui_Flag = (pSB->p_DuplicateSrc) ? pSB->p_DuplicateSrc->ui_Flag : pSB->ui_Flag;
		__SoftUnlock__;
		if((ui_Flag & iopSND_Cte_Loaded) == 0)
		{
			iopSND_RequestRegister(_i_SoftId, iopSND_Cte_DmaPlayRq);
			return 1;
		}
	}

	
	//$1-
	core = (ui_TrueFlag & iopSND_Cte_UseFxA) ? SD_CORE_0 : ((ui_TrueFlag & iopSND_Cte_UseFxB) ? SD_CORE_1 : -1);
	
	if(!pst_HardBuffer) 
		pst_HardBuffer = iopSND_pst_HardAllocBuffer(core);
		
	if(!pst_HardBuffer)
	{
		iopDbg_M_ErrX(iopDbg_Err_0026 " for S0x%08X", _i_SoftId);
		return -1;
	}
	
	if(!pst_HardBufferLeft && (ui_Flag & iopSND_Cte_Stereo)) 
		pst_HardBufferLeft = iopSND_pst_HardAllocBuffer(pst_HardBuffer->i_Core);
		
	if(!pst_HardBufferLeft && (ui_Flag & iopSND_Cte_Stereo))
	{
		iopDbg_M_ErrX(iopDbg_Err_0026 " for S0x%08X", _i_SoftId);
		return -1;
	}

	//$1-
	if(ui_Flag & iopSND_Cte_Stereo)
	{
		iopSND_HardSetPitch(pst_HardBuffer, us_Pitch);
		iopSND_HardSetPitch(pst_HardBufferLeft, us_Pitch);
		
		iopSND_HardSetVol(pst_HardBufferLeft, us_LeftVolume, 0);
		iopSND_HardSetVol(pst_HardBuffer, 0, us_RightVolume);
		
		iopSND_HardSetStartPos(pst_HardBuffer, (int) (pc_SpuMemAddr));
		iopSND_HardSetStartPos(pst_HardBufferLeft, (int) (pc_SpuMemAddr) + (2*ui_DbleBufferSize));
		
	}
	else
	{
		iopSND_HardSetPitch(pst_HardBuffer, us_Pitch);
		iopSND_HardSetVol(pst_HardBuffer, us_LeftVolume, us_RightVolume);
		iopSND_HardSetStartPos(pst_HardBuffer, (int) (pc_SpuMemAddr));

	}


	__SoftLock__;
	pSB->pst_HardBuffer = pst_HardBuffer;
	pSB->pst_HardBufferLeft = pst_HardBufferLeft;
	iopSND_M_SetState(pSB->ui_Flag, iopSND_Cte_SndFlg_Play);
	__SoftUnlock__;


	if(ui_Flag & iopSND_Cte_Stream)
	{
/*		if(ui_Flag & iopSND_Cte_EndReached)
		{
			iopSND_HardSetLoopPos(pst_HardBuffer, (int) 0x5000);
			if(ui_Flag & iopSND_Cte_Stereo) iopSND_HardSetLoopPos(pst_HardBufferLeft, (int) 0x5000);
		}
		else*/
		{
			iopSND_HardSetLoopPos(pst_HardBuffer, (int) (pc_SpuMemAddr));
			if(ui_Flag & iopSND_Cte_Stereo) iopSND_HardSetLoopPos(pst_HardBufferLeft, (int) (pc_SpuMemAddr) + (2*ui_DbleBufferSize));
		}

		iopSND_RequestRegister(_i_SoftId, iopSND_Cte_DmaWaitingStream1);
	}
	else
	{
		if(loop)
			iopSND_HardSetLoopPos(pst_HardBuffer, (int) (pc_SpuMemAddr));
		else
			iopSND_HardSetLoopPos(pst_HardBuffer, (int) 0x5000);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_SetVoiceVol(int _i_SoftId, int _i_Left, int _i_Right)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned short			us_LeftVolume, us_RightVolume;
	int						idx;
	iopSND_tdst_HardBuffer	*pst_HardBuffer;
	iopSND_tdst_HardBuffer	*pst_HardBufferLeft;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	idx = ps2SND_M_GetIdxFromSB(_i_SoftId);
	iopSND_M_CheckSoftBuffIdxOrReturn(idx, -1);

	us_LeftVolume = (unsigned short) _i_Left;
	us_RightVolume = (unsigned short) _i_Right;
	us_LeftVolume &= 0x7FFF;
	us_RightVolume &= 0x7FFF;

	pst_HardBuffer = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[idx].pst_HardBuffer;
	pst_HardBufferLeft = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[idx].pst_HardBufferLeft;
	iopSND_sa_SoftBuffer[idx].us_LeftVolume = us_LeftVolume;
	iopSND_sa_SoftBuffer[idx].us_RightVolume = us_RightVolume;

	if(iopSND_sa_SoftBuffer[idx].ui_Flag & iopSND_Cte_Stream)
	{
		iopSND_sa_StreamBuffer[idx].i_LVol = us_LeftVolume;
		iopSND_sa_StreamBuffer[idx].i_RVol = us_RightVolume;
	}

	if(pst_HardBuffer && pst_HardBufferLeft)
	{
		iopSND_HardSetVol(pst_HardBufferLeft, us_LeftVolume, 0);
		iopSND_HardSetVol(pst_HardBuffer, 0, us_RightVolume);
	}
	else if(pst_HardBuffer)
		iopSND_HardSetVol(pst_HardBuffer, us_LeftVolume, us_RightVolume);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_GetVoiceStatus(int *_pi, int *_po)
{
	/*~~~~~~~*/
	int i_SBNb;
	int i;
	/*~~~~~~~*/

	if(!_pi) return -1;
	if(!_po) return -1;

	i_SBNb = *_pi;
	_pi++;

	for(i = 0; i < i_SBNb; i++)
	{
		_po[i] = iopSND_i_GetVoiceStatus_r(_pi[i]);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_GetVoiceStatus_r(int _i_Voice)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						ret;
	iopSND_tdst_HardBuffer	*pst_HardBuffer;
	unsigned int			ui_Flag;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_i_Voice = ps2SND_M_GetIdxFromSB(_i_Voice);
	iopSND_M_CheckSoftBuffIdxOrReturn(_i_Voice, -1);

	/* __SoftLock__; */
	pst_HardBuffer = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[_i_Voice].pst_HardBuffer;
	ui_Flag = iopSND_sa_SoftBuffer[_i_Voice].ui_Flag;

	/* __SoftUnlock__; */
	ret = 0;

	if(pst_HardBuffer)
	{
		if(ui_Flag & iopSND_Cte_Stream)
		{
			if(pst_HardBuffer->ui_Flag & iopSND_Cte_HardPlay) ret = SND_Cul_SBS_Playing;
			if(pst_HardBuffer->ui_Flag & iopSND_Cte_AutoStopped) ret = 0;
		}
		else if(iopSND_i_HardGetPlayingStatus(pst_HardBuffer))
		{
			ret = SND_Cul_SBS_Playing;
		}
	}

	if(ui_Flag & iopSND_Cte_StreamIsPlayingNow) ret |= SND_Cul_SF_StreamIsPlayingNow;
	if(ui_Flag & iopSND_Cte_AutoStopped) ret = iopSND_Cte_AutoStopped;

	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_SetVoiceFreq(int _i_Snd, int _i_Freq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_tdst_HardBuffer	*pst_HardBuffer;
	iopSND_tdst_HardBuffer	*pst_HardBufferLeft;
	unsigned short			us_Pitch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_i_Snd = ps2SND_M_GetIdxFromSB(_i_Snd);
	iopSND_M_CheckSoftBuffIdxOrReturn(_i_Snd, -1);

	us_Pitch = M_Freq2Pitch(_i_Freq);
	pst_HardBuffer = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[_i_Snd].pst_HardBuffer;
	pst_HardBufferLeft = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[_i_Snd].pst_HardBufferLeft;
	
	iopSND_sa_SoftBuffer[_i_Snd].us_Pitch = us_Pitch;

	if(pst_HardBuffer) iopSND_HardSetPitch(pst_HardBuffer, us_Pitch);
	if(pst_HardBufferLeft) iopSND_HardSetPitch(pst_HardBufferLeft, us_Pitch);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_CleanBuffer(int _i_Snd)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	iopSND_tdst_SoftBuffer	*pSB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_i_Snd = ps2SND_M_GetIdxFromSB(_i_Snd);
	iopSND_M_CheckSoftBuffIdxOrReturn(_i_Snd, ;);
	SPY(_i_Snd, iopSND_CleanBuffer);

	__SoftLock__;
	pSB = (iopSND_tdst_SoftBuffer *) &iopSND_sa_SoftBuffer[_i_Snd];
	pSB->pc_SpuMemAddr = NULL;
	pSB->ui_Flag = iopSND_Cte_SndFlg_Free;
	pSB->us_Pitch = 0;
	pSB->i_LoopCount = 0;
	pSB->ui_DuplicateNb = 0;
	pSB->p_DuplicateSrc = NULL;
	pSB->pc_DbleBuff = NULL;
	pSB->ui_DbleBuffSize = 0;
	pSB->ui_DataPos = 0;
	pSB->ui_DataSize = 0;
	pSB->ui_FileSeek = 0;
	pSB->pst_HardBuffer = NULL;
	pSB->us_LeftVolume = 0;
	pSB->us_RightVolume = 0;
	pSB->pst_HardBufferLeft = 0;
	pSB->i_LastDma = -1;
	__SoftUnlock__;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
