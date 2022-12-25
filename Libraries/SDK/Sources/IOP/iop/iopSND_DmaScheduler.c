/*$T iopSND_DmaScheduler.c GC 1.138 06/02/04 11:56:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- sce ------------------------------------------------------------------------------------------------------------*/

#include <libsd.h>
#include <stdio.h>
#include <kernel.h>
#include <sys/fcntl.h>

/*$2- iop ------------------------------------------------------------------------------------------------------------*/

#include "iop/iopDebug.h"
#include "iop/iopMEM.h"
#include "iop/iopCLI.h"
#include "iop/iopBAStypes.h"
#include "CDV_Manager.h"
#include "iop/iopMain.h"
#include "iop/iopKernel.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "Sound/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/ps2/ps2SND.h"
#include "iop/iopSND.h"
#include "iop/iopSND_Voice.h"
#include "iop/iopSND_Hard.h"
#include "iop/iopSND_Fx.h"
#include "iop/iopSND_libsd.h"
#include "iop/iopSND_DmaScheduler.h"
#include "iop/iopSND_Debug.h"
#include "iop/iopSND_Stream.h"

/*$2------------------------------------------------------------------------------------------------------------------*/

#include "iop/iopMTX.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

//#define TRACE_SEQ
/**/
#ifdef TRACE_SEQ
#define dbgM_TraceSeq(_a_)	printf _a_
#define __dbgH				">> "
#else
#define dbgM_TraceSeq(_a_)
#define __dbgH
#endif 




static void iopSND_StreamRead(int iVoice,unsigned int ui_FileSeek,unsigned int ui_DbleBuffSize,char* pc_DbleBuff,int id) 
{ 
	unsigned int uiRead;


	if(ui_DbleBuffSize)
	{
		if((id) != -1)
		{
			// stereo file ?
			IOP_StrFileRead((id), (ui_FileSeek), (pc_DbleBuff), (ui_DbleBuffSize)); 
		}
		else if(!iopMTX_gpst_MuxStream)
		{
		    // normal mono file
        	// any prefetch ?
        	uiRead = iopSND_ui_StreamPrefetchGet(pc_DbleBuff, ui_FileSeek, ui_DbleBuffSize);
        	if(uiRead<ui_DbleBuffSize)
        	{
			    L_seekandread(iopSND_vi_BigFd, ui_FileSeek+uiRead , pc_DbleBuff+uiRead , ui_DbleBuffSize-uiRead); 
        	}
		}
		else
		{
			// multiplexed file ?
			iopMTX_i_Read(iopSND_vi_BigFd, (ui_FileSeek), (pc_DbleBuff), (ui_DbleBuffSize), (iVoice)); 
		}
	}
}


/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	iopSND_tdst_Request_
{
	int volatile	i_SoftBufferIndex;
	int volatile	i_Request;
} iopSND_tdst_Request;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- dma callback ---------------------------------------------------------------------------------------------------*/

int			iopSND_i_Dma0CallBack(int ch, void *data);
int			iopSND_i_Dma1CallBack(int ch, void *data);
static void iopSND_SignalDmaEvent(int ch);

/*$2------------------------------------------------------------------------------------------------------------------*/

static int	iopSND_i_RequestRead(int *_pi_voice, int *_pi_request);

/*$2------------------------------------------------------------------------------------------------------------------*/

static int	iopSND_i_SchedulerTransfert(int idma, int iVoice, int iMode);
static int	iopSND_i_SchedulerReadFile(int dma, int iVoice);
static int	iopSND_i_SchedulerPlayTest(int iVoice);
static void iopSND_SchedulerCodeLoopBuff(char *pBuff, int iSize, int part);
static void iopSND_LoopAlways(char *pBuff, int iSize);

/*$2- thread case ----------------------------------------------------------------------------------------------------*/

static void case__DmaStopStream(int i_voice, int dma);
static int	case__DmaWaitingStream0(int i_voice, int dma);
static int	case__DmaWaitingStream1(int i_voice, int dma);
static void case__DmaRunningStreamLoadAndPlay(int i_voice, int dma);
static void case__DmaPlayRq(int i_voice, int dma);

static int	case__DmaWaitingStreamLoadAndPlay(int i_voice, int dma);
static int	case__DmaStreamLoading0AndPlayStereo(int i_voice, int dma);
static void case__DmaStreamLoading2AndPlayStereo(int i_voice, int dma);
static int	case__DmaStreamLoading0(int i_voice, int dma);
static int	case__DmaStreamLoading1(int i_voice, int dma);
static int	case__DmaStreamLoading2(int i_voice, int dma);
static int	case__DmaStreamLoading3(int i_voice, int dma);

/*$2------------------------------------------------------------------------------------------------------------------*/

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

static volatile iopSND_tdst_Request * volatile pst_ReadVoiceRq;
static volatile iopSND_tdst_Request iopSND_ast_VoiceRqQueueOnDma[iopSND_Cte_DmaRqNb];
static volatile iopSND_tdst_Request * volatile pst_WriteVoiceRq;
int								iopSND_gi_RequestLock;
int								iopSND_gi_PendingDmaRqSema;

char							*iopSND_gac_Dma0Buff;
char							*iopSND_gac_Dma1Buff;

char							*iopSND_gac_Dma02Buff;
char							*iopSND_gac_Dma12Buff;

int								iopSND_gi_DmaSignalSema;
int volatile					iopSND_gi_DmaEvent[iopSND_Cte_CoreNb];

volatile int					iopSND_vb_SchedulerLocked;
iopSND_tdst_StreamBufferRefresh iopSND_vai_StreamBufferCpt[SND_Cte_MaxSimultaneousStream];
int volatile					iopSND_TransferingVoice[iopSND_Cte_CoreNb];

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_DmaSchedulerInitModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	/*$2- dma transfert ----------------------------------------------------------------------------------------------*/

	iopSND_TransferingVoice[0] = iopSND_TransferingVoice[1] = -1;

	iopSND_gi_DmaEvent[DMA_ID0] = iopSND_gi_DmaEvent[DMA_ID1] = 0;
	iopSND_gac_Dma0Buff = iopMEM_pv_iopAllocAlign(SND_Cte_MaxBufferSize >> 1, 64);
	iopSND_gac_Dma1Buff = iopMEM_pv_iopAllocAlign(SND_Cte_MaxBufferSize >> 1, 64);
	iopSND_gac_Dma02Buff = iopMEM_pv_iopAllocAlign(SND_Cte_MaxBufferSize >> 1, 64);
	iopSND_gac_Dma12Buff = iopMEM_pv_iopAllocAlign(SND_Cte_MaxBufferSize >> 1, 64);
	L_memset(iopSND_gac_Dma0Buff, 0, (SND_Cte_MaxBufferSize >> 1));
	L_memset(iopSND_gac_Dma1Buff, 0, (SND_Cte_MaxBufferSize >> 1));
	L_memset(iopSND_gac_Dma02Buff, 0, (SND_Cte_MaxBufferSize >> 2));
	L_memset(iopSND_gac_Dma12Buff, 0, (SND_Cte_MaxBufferSize >> 2));
	L_memset(iopSND_vai_StreamBufferCpt, 0, SND_Cte_MaxSimultaneousStream * sizeof(iopSND_tdst_StreamBufferRefresh));

	L_sceSdSetTransIntrHandler
	(
		DMA_ID0,
		(sceSdSpu2IntrHandler) iopSND_i_Dma0CallBack,
		(void *) &iopSND_gi_DmaEvent[DMA_ID0]
	);
	L_sceSdSetTransIntrHandler
	(
		DMA_ID1,
		(sceSdSpu2IntrHandler) iopSND_i_Dma1CallBack,
		(void *) &iopSND_gi_DmaEvent[DMA_ID1]
	);

	/*$2- request init -----------------------------------------------------------------------------------------------*/

	for(i = 0; i < iopSND_Cte_DmaRqNb; i++)
	{
		iopSND_ast_VoiceRqQueueOnDma[i].i_Request = iopSND_Cte_DmaFree;
		iopSND_ast_VoiceRqQueueOnDma[i].i_SoftBufferIndex = -1;
	}

	pst_WriteVoiceRq = pst_ReadVoiceRq = &iopSND_ast_VoiceRqQueueOnDma[0];

	iopSND_SignalDmaEvent(0);

	/* iopSND_SignalDmaEvent(1); -> 1st rq is done on channel 1 */
	CLI_SignalSema(iopSND_gi_RequestLock);

	/*$2- lock thread it ---------------------------------------------------------------------------------------------*/

	iopSND_vb_SchedulerLocked = 1;
}

void iopSND_DmaReinit(void)
{
    L_sceSdSetTransIntrHandler
	(
		DMA_ID0,
		(sceSdSpu2IntrHandler) iopSND_i_Dma0CallBack,
		(void *) &iopSND_gi_DmaEvent[DMA_ID0]
	);
	L_sceSdSetTransIntrHandler
	(
		DMA_ID1,
		(sceSdSpu2IntrHandler) iopSND_i_Dma1CallBack,
		(void *) &iopSND_gi_DmaEvent[DMA_ID1]
	);

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_DmaSchedulerCloseModule(void)
{
	TerminateThread(iopSND_gi_SndThreadVoice);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_Dma0CallBack(int ch, void *data)
{
	/*~~~*/
	int *c;
	/*~~~*/

	c = (int *) data;
	ch = ch;
	(*c) = 1;
	CLI_iSignalSema(iopSND_gi_DmaSignalSema);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_i_Dma1CallBack(int ch, void *data)
{
	/*~~~*/
	int *c;
	/*~~~*/

	c = (int *) data;
	ch = ch;
	(*c) = 1;
	CLI_iSignalSema(iopSND_gi_DmaSignalSema);
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void iopSND_SignalDmaEvent(int ch)
{
	iopSND_gi_DmaEvent[ch] = 1;
	CLI_SignalSema(iopSND_gi_DmaSignalSema);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */ 
void iopSND_DmaThread(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int dma, i_voice, i_request;
	int dma0, dma1;
	int tmp;
	int signaldma;
	int call;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	dma = 1;
	i_voice = -1;
	i_request = -1;
	call = 0;

#ifdef INUM_DMA_4
	EnableIntr(INUM_DMA_4); /* CORE0 DMA interrupt */
	EnableIntr(INUM_DMA_7); /* CORE1 DMA interrupt */
#endif
#ifndef _FINAL_
	IOP_vi_StackSize[ThIdxSoundEvent] = CheckThreadStack();
#endif


	while(1)
	{

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    external lock
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(iopSND_vb_SchedulerLocked) 
		{
		    L_SleepThread();
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    wait dma rq
		 ---------------------------------------------------------------------------------------------------------------
		 */

		CLI_WaitSema(iopSND_gi_PendingDmaRqSema);
		
		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    get rq
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(iopSND_i_RequestRead(&i_voice, &i_request))
		{
			iopDbg_M_ErrX(iopDbg_Err_0095 " %d", dma);
			continue;
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    check transfert state
		 ---------------------------------------------------------------------------------------------------------------
		 */

		do
		{
			tmp = L_sceSdVoiceTransStatus(dma, SD_TRANS_STATUS_CHECK);
			iopDbg_M_AssertX((tmp == 1), iopDbg_Err_0068 " %d/%d", tmp, dma);
		} while(tmp != 1);
        
		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    free voice that waiting on special dma id
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(iopSND_TransferingVoice[dma] != -1)
		{
		    // dont wait any more on this channel. 
		    // the transfert is complete, but the dma is used by another one
			iopSND_sa_SoftBuffer[iopSND_TransferingVoice[dma]].i_LastDma = -1;
			iopSND_TransferingVoice[dma] = -1;
		}
		

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    treat rq
		 ---------------------------------------------------------------------------------------------------------------
		 */

		signaldma = 1;

		switch(i_request)
		{

		/*$2- preload ------------------------------------------------------------------------------------------------*/

		case iopSND_Cte_DmaCLIRead0:
		case iopSND_Cte_DmaCLIRead1:
		case iopSND_Cte_DmaCLIRead2:
		case iopSND_Cte_DmaCLIRead3:
		case iopSND_Cte_DmaCLIRead4:
		case iopSND_Cte_DmaCLIRead5:
		case iopSND_Cte_DmaCLIRead6:
		case iopSND_Cte_DmaCLIRead7:
		case iopSND_Cte_DmaCLIRead8:
		case iopSND_Cte_DmaCLIRead9:
			dbgM_TraceSeq((__dbgH "preload %04x, dma %d\n", i_voice, dma)); 
			iopCLI_FileSystemExec(i_request - iopSND_Cte_DmaCLIRead0);
			break;

		/*$2- cancel -------------------------------------------------------------------------------------------------*/

		case iopSND_Cte_DmaCancel:
			dbgM_TraceSeq((__dbgH "cancel %04x, dma %d\n", i_voice, dma));
			break;

		/*$2- one shot loading ---------------------------------------------------------------------------------------*/

		case iopSND_Cte_DmaWaitingReload:
			dbgM_TraceSeq((__dbgH "reloading %04x, dma %d\n", i_voice, dma));
			if(iopSND_i_ReloadBuffer_r(i_voice)) break;

		case iopSND_Cte_DmaWaitingOneShotLoading:
		    dbgM_TraceSeq((__dbgH "oneshot %04x, dma %d\n", i_voice, dma));
			if(dma == 0)
			{
    			signaldma = iopSND_i_SchedulerTransfert(dma, i_voice, iopSND_Cte_DmaTransOneShot);
			}
			else
			{
			    signaldma = 1;
			    iopSND_RequestWrite(i_voice, iopSND_Cte_DmaWaitingOneShotLoading);
			    break;
			}


			__SoftLock__;
			if(iopSND_sa_SoftBuffer[i_voice].p_DuplicateSrc)
			{
				iopSND_sa_SoftBuffer[i_voice].p_DuplicateSrc->ui_Flag |= iopSND_Cte_Loaded;
			}
			else
			{
				iopSND_sa_SoftBuffer[i_voice].ui_Flag |= iopSND_Cte_Loaded;
			}
			__SoftUnlock__;
			iopSND_gi_OneShotPending--;
    		
    		if(iopSND_vi_RPCThreadIsWaiting)
    		{
    		    iopSND_vi_RPCThreadIsWaiting=0;
    		    WakeupThread(IOP_gi_RPC_Thread);
    		}

			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    reinit & play of stream
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case iopSND_Cte_DmaWaitingStreamPrefetch:
			dbgM_TraceSeq((__dbgH "prefetch %04x, dma %d\n", i_voice, dma));
			iopSND_StreamPrefetchDo();
			break;

		case iopSND_Cte_DmaWaitingStreamLoadAndPlay:
			dbgM_TraceSeq((__dbgH "waiting load&play %04x, dma %d\n", i_voice, dma));
			signaldma = case__DmaWaitingStreamLoadAndPlay(i_voice, dma);
			break;

		case iopSND_Cte_DmaStreamLoading0AndPlayStereo:
			dbgM_TraceSeq((__dbgH "loading0 & play stereo %04x, dma %d\n", i_voice, dma));
			signaldma = case__DmaStreamLoading0AndPlayStereo(i_voice, dma);
			break;

		case iopSND_Cte_DmaStreamLoading2AndPlayStereo:
		    dbgM_TraceSeq((__dbgH "loading2 & play stereo %04x, dma %d\n", i_voice, dma));
			case__DmaStreamLoading2AndPlayStereo(i_voice, dma);
			break;

		case iopSND_Cte_DmaRunningStreamLoadAndPlay:
		    dbgM_TraceSeq((__dbgH "running load & play %04x, dma %d\n", i_voice, dma));
			case__DmaRunningStreamLoadAndPlay(i_voice, dma);
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    stream refresh:: wait to refresh the dble buffer
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case iopSND_Cte_DmaWaitingStream0:
			signaldma = case__DmaWaitingStream0(i_voice, dma);
			break;

		case iopSND_Cte_DmaStreamLoading0:
			signaldma = case__DmaStreamLoading0(i_voice, dma);
			break;

		case iopSND_Cte_DmaStreamLoading2:
			signaldma = case__DmaStreamLoading2(i_voice, dma);
			break;

		case iopSND_Cte_DmaWaitingStream1:
			signaldma = case__DmaWaitingStream1(i_voice, dma);
			break;

		case iopSND_Cte_DmaStreamLoading1:
			signaldma = case__DmaStreamLoading1(i_voice, dma);
			break;

		case iopSND_Cte_DmaStreamLoading3:
			signaldma = case__DmaStreamLoading3(i_voice, dma);
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    stop streaming
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case iopSND_Cte_DmaStopStream:
			dbgM_TraceSeq((__dbgH "stopstream %04x, dma %d\n", i_voice, dma));
			case__DmaStopStream(i_voice, dma);
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    try to play
		 ---------------------------------------------------------------------------------------------------------------
		 */

		case iopSND_Cte_DmaPlayRq:
			dbgM_TraceSeq((__dbgH "waiting for playing %04x, dma %d\n", i_voice, dma));
			case__DmaPlayRq(i_voice, dma);
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    default
		 ---------------------------------------------------------------------------------------------------------------
		 */

		default:
			dbgM_TraceSeq((__dbgH "bad dma state %04x, dma %d\n", i_voice, dma));
			iopDbg_M_Err(iopDbg_Err_0010);
			break;
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    signal dma if needed
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(signaldma) 
		{
		    iopSND_SignalDmaEvent(dma);
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    wait for dma IT
		 ---------------------------------------------------------------------------------------------------------------
		 */

		CLI_WaitSema(iopSND_gi_DmaSignalSema);

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    choose the dma
		 ---------------------------------------------------------------------------------------------------------------
		 */

		dma0 = iopSND_gi_DmaEvent[0];
		dma1 = iopSND_gi_DmaEvent[1];

		if(dma0 && !dma1)
			dma = 0;
		else if(!dma0 && dma1)
			dma = 1;
		else if(dma0 && dma1)
			dma = 1 - dma;
		else
		{
			iopDbg_M_Err(iopDbg_Err_0080);
		}

		iopSND_gi_DmaEvent[dma] = 0;

#ifndef _FINAL_
		if(call++ &0x40) IOP_vi_StackSize[ThIdxSoundEvent] = CheckThreadStack();
#endif
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int iopSND_i_SchedulerTransfert(int idma, int iVoice, int iMode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	ui_Flag;
	char			*pc_SpuMemAddr;
	char			*pc_DbleBuff;
	unsigned int	ui_DbleBuffSize;
	int				result, error;
	unsigned short	us_mode;
	unsigned int	ui_SpuAddr;
	unsigned int	ui_DataSize;
	unsigned int	ui_FileSeek;
	short			s_StrFileIdx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	iopSND_M_CheckSoftBuffIdxOrReturn(iVoice, -1);

	__SoftLock__;
	ui_Flag = iopSND_sa_SoftBuffer[iVoice].ui_Flag;
	pc_SpuMemAddr = iopSND_sa_SoftBuffer[iVoice].pc_SpuMemAddr;
	pc_DbleBuff = iopSND_sa_SoftBuffer[iVoice].pc_DbleBuff;
	ui_DbleBuffSize = iopSND_sa_SoftBuffer[iVoice].ui_DbleBuffSize;
	ui_DataSize = iopSND_sa_SoftBuffer[iVoice].ui_DataSize;
	ui_FileSeek  = iopSND_sa_SoftBuffer[iVoice].ui_FileSeek ;
	s_StrFileIdx = iopSND_sa_SoftBuffer[iVoice].s_StrFileIdx;
	iopSND_sa_SoftBuffer[iVoice].i_LastDma = -1;
	__SoftUnlock__;

	iopDbg_M_Assert((idma == 0) || (idma == 1), iopDbg_Err_0019 "-1");
	iopDbg_M_Assert((ui_Flag != iopSND_Cte_SndFlg_Free), iopDbg_Err_0019 "-2");
	iopDbg_M_Assert(ui_DbleBuffSize, iopDbg_Err_0019 "-4");
	iopDbg_M_Assert(pc_SpuMemAddr, iopDbg_Err_0019 "-5");

	iopSND_TransferingVoice[idma] = -1;

	if(ui_Flag & iopSND_Cte_Stream)
	{
		if(ui_Flag & iopSND_Cte_Stereo)
		{
			iopDbg_M_Assert(s_StrFileIdx != -1, iopDbg_Err_0094);
			pc_DbleBuff = (s_StrFileIdx & 1) ? iopSND_gac_Dma12Buff : iopSND_gac_Dma02Buff;
		}
		else
		{
			pc_DbleBuff = idma ? iopSND_gac_Dma1Buff : iopSND_gac_Dma0Buff;
		}
	}

	switch(iMode)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case iopSND_Cte_DmaTransStream2:
		pc_DbleBuff +=  ui_DbleBuffSize;
		pc_SpuMemAddr += (2 * ui_DbleBuffSize);

	case iopSND_Cte_DmaTransStream0:
		iopSND_SchedulerCodeLoopBuff(pc_DbleBuff, ui_DbleBuffSize, 0);
		if(ui_Flag & iopSND_Cte_EndRead) 
		{
		//printf("S%d endreached(%d)\n", iVoice, __LINE__);
		iopSND_LoopAlways(pc_DbleBuff, ui_DataSize % ui_DbleBuffSize);
		}
		FlushDcache();
		us_mode = SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA;
		ui_SpuAddr = (unsigned int) (pc_SpuMemAddr);
		break;

	case iopSND_Cte_DmaTransStream3:
		pc_DbleBuff += ui_DbleBuffSize;
		pc_SpuMemAddr += (2 * ui_DbleBuffSize);

	case iopSND_Cte_DmaTransStream1:
		iopSND_SchedulerCodeLoopBuff(pc_DbleBuff, ui_DbleBuffSize, 1);
		if(ui_Flag & iopSND_Cte_EndRead) 
		{
		//printf("S%d endreached(%d)\n", iVoice, __LINE__);
		iopSND_LoopAlways(pc_DbleBuff, ui_DataSize % ui_DbleBuffSize);
		}
		FlushDcache();
		us_mode = SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA;
		ui_SpuAddr = (unsigned int) (pc_SpuMemAddr) + ui_DbleBuffSize;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case iopSND_Cte_DmaTransOneShot:
	    
        pc_DbleBuff = iopMEM_pv_iopAllocAlign(ui_DataSize, 64);
        if(!pc_DbleBuff) 
        {
            iopDbg_M_Err(iopDbg_Err_0079);
            return -1;            
        }

        L_seekandread(iopSND_vi_BigFd_Snd, ui_FileSeek , pc_DbleBuff, ui_DataSize);
		
		
		iopSND_LoopAlways(pc_DbleBuff, ui_DbleBuffSize);
		FlushDcache();
		us_mode = SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA;
		ui_SpuAddr = (unsigned int) (pc_SpuMemAddr);
		
		L_sceSdVoiceTrans
		(
			0,
			SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA,
			(u_char *) pc_DbleBuff,
			ui_SpuAddr, 
			ui_DbleBuffSize
		);
		L_sceSdVoiceTransStatus(0, SD_TRANS_STATUS_WAIT);
		iopMEM_iopFreeAlign(pc_DbleBuff);
		return 0;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	default:
		iopDbg_M_Err(iopDbg_Err_001A);
		return -1;
	}

	result = -1;
	error = 0;

	__SoftLock__;
	iopSND_sa_SoftBuffer[iVoice].i_LastDma = idma;
	iopSND_TransferingVoice[idma] = iVoice;
	__SoftUnlock__;


	do
	{
		result = L_sceSdVoiceTrans(idma, us_mode, pc_DbleBuff, ui_SpuAddr, ui_DbleBuffSize);
	} while((result < 0) && (error++ < 9));


	__SoftLock__;
	if(result <= 0)
	{
		result = -1;
		
		iopSND_sa_SoftBuffer[iVoice].i_LastDma = -1;
    	iopSND_TransferingVoice[idma] = -1;
		iopDbg_M_ErrX(iopDbg_Err_001A " transfert failed S%08x %d oct", iVoice, ui_DbleBuffSize);
	}
	else
	    result = 0;
	__SoftUnlock__;

	
	return result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int iopSND_i_SchedulerReadFile(int idma, int iVoice)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						remain=0, b_Chain, loop;
	unsigned int			ui_Flag, ui_DbleBuffSize, ui_DbleBuffSizeRef;
	char					*pc_DbleBuff;
	unsigned int			ui_FileSeek, ui_LoopEnd, ui_LoopBegin;
	iopSND_tdst_HardBuffer	*pst_HardBuffer;
	iopSND_tdst_HardBuffer	*pst_HardBufferLeft;
	unsigned int			ui_StopPos, ui_DataPos, uiChainSize, ui_SpuMemAddr;
	short 					s_StrFileIdx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	iopSND_M_CheckSoftBuffIdxOrReturn(iVoice, -1);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    get shared variables
	 -------------------------------------------------------------------------------------------------------------------
	 */

	__SoftLock__;
	ui_Flag = iopSND_sa_SoftBuffer[iVoice].ui_Flag;
	pst_HardBuffer = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[iVoice].pst_HardBuffer;
	pst_HardBufferLeft = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[iVoice].pst_HardBufferLeft;
	ui_DbleBuffSize = iopSND_sa_SoftBuffer[iVoice].ui_DbleBuffSize;
	ui_FileSeek = iopSND_sa_SoftBuffer[iVoice].ui_FileSeek;
	ui_LoopBegin = iopSND_sa_SoftBuffer[iVoice].ui_LoopBegin;
	ui_LoopEnd = iopSND_sa_SoftBuffer[iVoice].ui_LoopEnd;
	b_Chain = iopSND_sa_StreamBuffer[iVoice].b_Chain;
	ui_StopPos = iopSND_sa_StreamBuffer[iVoice].ui_StopPos;
	ui_DataPos = iopSND_sa_SoftBuffer[iVoice].ui_DataPos;
	loop = iopSND_sa_SoftBuffer[iVoice].i_LoopCount;
	s_StrFileIdx = iopSND_sa_SoftBuffer[iVoice].s_StrFileIdx;
	ui_SpuMemAddr = (unsigned int)iopSND_sa_SoftBuffer[iVoice].pc_SpuMemAddr;
	__SoftUnlock__;
	ui_DbleBuffSizeRef = ui_DbleBuffSize;

	if(ui_Flag == iopSND_Cte_SndFlg_Free) return -1;
	
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    choose buffer
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(ui_Flag & iopSND_Cte_Stereo)
	{
		iopDbg_M_Assert(s_StrFileIdx != -1, iopDbg_Err_0094);
		pc_DbleBuff = (s_StrFileIdx & 1) ? iopSND_gac_Dma12Buff : iopSND_gac_Dma02Buff;
	}
	else
	{
		pc_DbleBuff = idma ? iopSND_gac_Dma1Buff : iopSND_gac_Dma0Buff;
	}
	iopDbg_M_Assert(pc_DbleBuff, iopDbg_Err_0057);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    treat the end detection
	 -------------------------------------------------------------------------------------------------------------------
	 */


	if(ui_Flag & iopSND_Cte_EndRead)
	{
		iopSND_sa_SoftBuffer[iVoice].ui_Flag |= iopSND_Cte_EndPlayed1;
		iopSND_sa_SoftBuffer[iVoice].ui_Flag |= iopSND_Cte_EndPlayed0;
		
		iopSND_HardSetLoopPos(pst_HardBuffer, (int) 0x5000);
		L_memset(pc_DbleBuff, 0, ui_DbleBuffSize);
		iopSND_LoopAlways(pc_DbleBuff, ui_DbleBuffSize);
		if(pst_HardBufferLeft) 
		{
			iopSND_HardSetLoopPos(pst_HardBufferLeft, (int) 0x5000);
			L_memset(pc_DbleBuff+ui_DbleBuffSize, 0, ui_DbleBuffSize);
			iopSND_LoopAlways(pc_DbleBuff+ui_DbleBuffSize, ui_DbleBuffSize);
		}
		return 1;
	}
	

	
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    chaining files
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(b_Chain)
	{
		if
		(
			((ui_FileSeek + ui_DbleBuffSize) >= (ui_StopPos + ui_DataPos))
		&&	(ui_FileSeek <= (ui_StopPos + ui_DataPos))
		)
		{
			unsigned int uiPlayer, uiWriter;
			
			b_Chain = 1; 
			remain = ui_StopPos + ui_DataPos - ui_FileSeek;
			uiChainSize = ui_DbleBuffSize - (unsigned int) remain;
			ui_DbleBuffSize = (unsigned int) remain;
			
			uiPlayer = (unsigned int)iopSND_i_HardGetNAX(pst_HardBuffer);
			if(uiPlayer != -1)
			{
				// size to play from curr pos to write pos
				if(uiPlayer > (ui_SpuMemAddr + ui_DbleBuffSizeRef) )
				{
					uiWriter = ui_SpuMemAddr;
					remain = 2*ui_DbleBuffSizeRef + uiWriter - uiPlayer;
				}
				else
				{
					uiWriter = ui_SpuMemAddr + ui_DbleBuffSizeRef;
					remain = uiWriter - uiPlayer;
				}
				
				// add the new refresh size 
				remain += ui_DbleBuffSize;
				if(ui_Flag & iopSND_Cte_Stereo) remain *= 2;
				
				iopSND_sa_StreamBuffer[iVoice].ui_ChainDelaySize = remain;
			}
		}
		else
		{
			b_Chain = 0;
		}
	}


	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    read file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	do
	{
		remain = ui_LoopEnd - ui_FileSeek;
		if(remain >= ui_DbleBuffSize)
		{
			iopSND_StreamRead(iVoice, ui_FileSeek, ui_DbleBuffSize, pc_DbleBuff, iopSND_sa_SoftBuffer[iVoice].s_StrFileIdx);
			ui_FileSeek += ui_DbleBuffSize;		
		}
		else
		{
			if(remain)
			{
				iopSND_StreamRead(iVoice, ui_FileSeek, remain, pc_DbleBuff, iopSND_sa_SoftBuffer[iVoice].s_StrFileIdx);
				ui_FileSeek += remain;
				pc_DbleBuff += remain;
			}


			if(loop)
			{
				if(loop>0) loop--;
				
				ui_FileSeek = ui_LoopBegin;
				iopSND_StreamRead
				(
					iVoice,
					ui_FileSeek,
					(ui_DbleBuffSize - remain),
					pc_DbleBuff,
					iopSND_sa_SoftBuffer[iVoice].s_StrFileIdx
				);
				
				ui_FileSeek += ui_DbleBuffSize - remain;
			}
			else
			{
				
				L_memset(pc_DbleBuff, 0, ui_DbleBuffSize - remain);
				
				if(ui_Flag & iopSND_Cte_Stereo)
				{
					L_memset(pc_DbleBuff+ui_DbleBuffSize, 0, ui_DbleBuffSize - remain);

				}

				if(!b_Chain)
				{

					__SoftLock__;
					iopSND_sa_SoftBuffer[iVoice].ui_Flag |= iopSND_Cte_EndRead;
					iopSND_sa_SoftBuffer[iVoice].ui_Flag &= ~iopSND_Cte_StreamIsPlayingNow;
					__SoftUnlock__;
				}
			}
		}


		if(b_Chain)
		{
			pc_DbleBuff += ui_DbleBuffSize;
			ui_DbleBuffSize = uiChainSize;

			__SoftLock__;
			iopSND_sa_SoftBuffer[iVoice].ui_DataPos = iopSND_sa_StreamBuffer[iVoice].ui_DataPos;
			iopSND_sa_SoftBuffer[iVoice].ui_DataSize = iopSND_sa_StreamBuffer[iVoice].ui_DataSize;
			iopSND_sa_SoftBuffer[iVoice].i_LoopCount = iopSND_sa_StreamBuffer[iVoice].i_LoopNb;

			iopSND_sa_SoftBuffer[iVoice].ui_FileSeek = iopSND_sa_StreamBuffer[iVoice].ui_StartPos + iopSND_sa_StreamBuffer[iVoice].ui_DataPos;
			iopSND_sa_SoftBuffer[iVoice].ui_LoopBegin = iopSND_sa_StreamBuffer[iVoice].ui_LoopBegin;
			iopSND_sa_SoftBuffer[iVoice].ui_LoopEnd = iopSND_sa_StreamBuffer[iVoice].ui_LoopEnd;

			iopSND_sa_StreamBuffer[iVoice].b_Chain = 0;
			iopSND_sa_StreamBuffer[iVoice].ui_DataPos = 0;
			iopSND_sa_StreamBuffer[iVoice].ui_DataSize = 0;
			iopSND_sa_StreamBuffer[iVoice].i_LoopNb = 0;
			iopSND_sa_StreamBuffer[iVoice].ui_StopPos = 0;
			iopSND_sa_StreamBuffer[iVoice].ui_StartPos = 0;
			iopSND_sa_StreamBuffer[iVoice].ui_LoopBegin = 0;
			iopSND_sa_StreamBuffer[iVoice].ui_LoopEnd = 0;
			iopSND_sa_StreamBuffer[iVoice].ui_ChainDelaySize = 0;
			
			iopSND_vai_StreamBufferCpt[iVoice - 1].i_ChangeCount = 0;

			ui_Flag = iopSND_sa_SoftBuffer[iVoice].ui_Flag;
			ui_FileSeek = iopSND_sa_SoftBuffer[iVoice].ui_FileSeek;
			ui_LoopBegin = iopSND_sa_SoftBuffer[iVoice].ui_LoopBegin;
			ui_LoopEnd = iopSND_sa_SoftBuffer[iVoice].ui_LoopEnd;
			loop = iopSND_sa_SoftBuffer[iVoice].i_LoopCount;
			__SoftUnlock__;

			IOP_i_StrFileReinit(iopSND_sa_SoftBuffer[iVoice].s_StrFileIdx, iopSND_sa_SoftBuffer[iVoice].ui_DataPos, iopSND_sa_SoftBuffer[iVoice].ui_DataSize);
		}
	} while(b_Chain--);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    update pos
	 -------------------------------------------------------------------------------------------------------------------
	 */

	__SoftLock__;
	iopSND_sa_SoftBuffer[iVoice].ui_FileSeek = ui_FileSeek;
	iopSND_sa_SoftBuffer[iVoice].i_LoopCount = loop;
	__SoftUnlock__;
	
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int iopSND_i_SchedulerPlayTest(int iVoice)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	u_int							next, middle;
	unsigned int					ui_Flag;
	iopSND_tdst_HardBuffer			*pst_HardBuffer;
	char							*pc_SpuMemAddr;
	unsigned int					ui_DbleBuffSize;
	int								value;
	iopSND_tdst_StreamBufferRefresh *pSBR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	iopSND_M_CheckSoftBuffIdxOrReturn(iVoice, -2);

	__SoftLock__;
	ui_Flag = iopSND_sa_SoftBuffer[iVoice].ui_Flag;
	pst_HardBuffer = (iopSND_tdst_HardBuffer*)iopSND_sa_SoftBuffer[iVoice].pst_HardBuffer;
	pc_SpuMemAddr = iopSND_sa_SoftBuffer[iVoice].pc_SpuMemAddr;
	ui_DbleBuffSize = iopSND_sa_SoftBuffer[iVoice].ui_DbleBuffSize;
	__SoftUnlock__;

	if((ui_Flag & iopSND_Cte_SndFlg_Play) && (pst_HardBuffer))
	{
		next = iopSND_i_HardGetNAX(pst_HardBuffer);
		middle = (u_int) (pc_SpuMemAddr) + ui_DbleBuffSize;

	
		/* auto stop */
		if(next < 0x5010)
		{
		//printf("NAX%d %x\n", iVoice, next);
			return 2;
		}
		
		

		if(next > middle)
		{
			value = 1;
		}
		else
		{
			value = 0;
		}

		pSBR = iopSND_vai_StreamBufferCpt + iVoice - 1;
		if(value != pSBR->i_BufferId)
		{
			pSBR->i_BufferId = value;
			iopDbg_M_AssertX
			(
				(pSBR->e_BufferState[pSBR->i_BufferId] == en_BuffWritten),
				iopDbg_Err_008C " S%04x: B%d=%d",
				iVoice,
				pSBR->i_BufferId,
				pSBR->e_BufferState[pSBR->i_BufferId]
			);

			pSBR->i_ChangeCount++;
		}

		pSBR->e_BufferState[value] = en_BuffReading;
	}
	else
	{
		/*
		 * iopDbg_M_ErrX(iopDbg_Err_008F " S0x%04X : flag 0x%08X, Hbuff 0x%x", iVoice,
		 * ui_Flag, pst_HardBuffer);
		 */
		value = -1;
	}

	return value;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopSND_SchedulerResetBuffer(int iVoice)
{
	iVoice = ps2SND_M_GetIdxFromSB(iVoice);
	iopSND_M_CheckSoftBuffIdxOrReturn(iVoice, -1);
	
	//printf("S%d clear (%d)\n", iVoice, __LINE__);
	__SoftLock__;
	iopSND_sa_SoftBuffer[iVoice].ui_FileSeek = iopSND_sa_SoftBuffer[iVoice].ui_DataPos;
	iopSND_sa_SoftBuffer[iVoice].ui_Flag &= ~iopSND_Cte_EndRead;
	iopSND_sa_SoftBuffer[iVoice].ui_Flag &= ~(iopSND_Cte_EndPlayed0|iopSND_Cte_EndPlayed1);
	iopSND_sa_SoftBuffer[iVoice].ui_Flag &= ~iopSND_Cte_StreamIsPlayingNow;
	__SoftUnlock__;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void iopSND_SchedulerCodeLoopBuff(char *pBuff, int iSize, int part)
{
	/*~~*/
	int i;
	/*~~*/

	if(!pBuff) return;

#ifdef _DEBUG
	if(iSize % 16)
	{
		iopDbg_M_Err(iopDbg_Err_0015);
	}
	else
#endif
	{
		pBuff++;
		*pBuff = 2 + ((1 - part) << 2);

		pBuff += 0x10;
		for(i = 0x10; i < iSize - 0x10; i += 0x10, pBuff += 0x10)
		{
			*pBuff = 2; /* loop */
		}

		*pBuff = 2 | part;
	}
}

#define _ADPCM_MARK_START	0x04
#define _ADPCM_MARK_LOOP	0x02
#define _ADPCM_MARK_END		0x01

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void iopSND_LoopAlways(char *pBuff, int iSize)
{
	iopDbg_M_Assert(pBuff, iopDbg_Err_0015 " buffer");
	iopDbg_M_Assert((iSize >= 0x30), iopDbg_Err_0015 " size");

	if(!pBuff) return;
	if(iSize < 0x30) return;

#ifdef _DEBUG
	if(iSize % 16)
	{
		iopDbg_M_Err(iopDbg_Err_0015);
	}
	else
#endif
		if(iSize >= 0x30)
		{
			*(pBuff + 1) &= ~(_ADPCM_MARK_START | _ADPCM_MARK_LOOP);
			*(pBuff + 1 + 0x10) &= ~(_ADPCM_MARK_LOOP);
			*(pBuff + 1 + 0x20) &= ~(_ADPCM_MARK_LOOP);

			/*
			*(pBuff + 1) = (_ADPCM_MARK_START | _ADPCM_MARK_LOOP);
			*(pBuff + 1 + 0x10) = (_ADPCM_MARK_LOOP);
			*(pBuff + 1 + 0x20) = (_ADPCM_MARK_LOOP);
			//*/

			*(pBuff + iSize - 0x0f - 0x20) = (_ADPCM_MARK_LOOP);
			*(pBuff + iSize - 0x0f - 0x10) = (_ADPCM_MARK_LOOP);
			*(pBuff + iSize - 0x0f) = (_ADPCM_MARK_END | _ADPCM_MARK_LOOP);
		}
		else if(iSize >= 0x20)
		{
			*(pBuff + 1) &= ~(_ADPCM_MARK_START | _ADPCM_MARK_LOOP);
			*(pBuff + 1 + 0x10) &= ~(_ADPCM_MARK_LOOP);
		/*
			*(pBuff + 1) = (_ADPCM_MARK_START | _ADPCM_MARK_LOOP);
			*(pBuff + 1 + 0x10) = (_ADPCM_MARK_LOOP);
			//*/

			*(pBuff + iSize - 0x0f - 0x10) = (_ADPCM_MARK_LOOP);
			*(pBuff + iSize - 0x0f) = (_ADPCM_MARK_END | _ADPCM_MARK_LOOP);
		}
		else if(iSize >= 0x10)
		{
			//*(pBuff + 1) = (_ADPCM_MARK_START | _ADPCM_MARK_LOOP);

			*(pBuff + iSize - 0x0f) = (_ADPCM_MARK_END | _ADPCM_MARK_LOOP);
		}
		else
		{
			iopDbg_M_Assert(0, iopDbg_Err_0015 " big pb !!");
		}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_RequestWrite(int _i_voice, int _i_request)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	ui_Flag;
	/*~~~~~~~~~~~~~~~~~~~~*/

	/*$2- check id ---------------------------------------------------------------------------------------------------*/

	iopSND_M_CheckSoftBuffIdxOrReturn(_i_voice, ;);

	if(_i_voice)
	{
		__SoftLock__;
		ui_Flag = iopSND_sa_SoftBuffer[_i_voice].ui_Flag;
		__SoftUnlock__;

		if(ui_Flag == iopSND_Cte_SndFlg_Free)
		{
			iopDbg_M_ErrX(iopDbg_Err_005D " S0x%04X", _i_voice);
			return;
		}
	}

	/*$2- write rq in fifo -------------------------------------------------------------------------------------------*/

	CLI_WaitSema(iopSND_gi_RequestLock);
	iopDbg_M_Assert((pst_WriteVoiceRq->i_Request == iopSND_Cte_DmaFree), iopDbg_Err_0024);

	pst_WriteVoiceRq->i_Request = _i_request;
	pst_WriteVoiceRq->i_SoftBufferIndex = _i_voice;

	if(++pst_WriteVoiceRq == &iopSND_ast_VoiceRqQueueOnDma[iopSND_Cte_DmaRqNb])
		pst_WriteVoiceRq = &iopSND_ast_VoiceRqQueueOnDma[0];

	iopDbg_M_Assert((pst_WriteVoiceRq != pst_ReadVoiceRq), iopDbg_Err_0025);
	CLI_SignalSema(iopSND_gi_RequestLock);

	/*$2- signal Rq --------------------------------------------------------------------------------------------------*/

	CLI_SignalSema(iopSND_gi_PendingDmaRqSema);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int iopSND_i_RequestRead(int *_pi_voice, int *_pi_request)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int				ret;
	unsigned int	ui_Flag;
	/*~~~~~~~~~~~~~~~~~~~~*/

	/*$2- read it ----------------------------------------------------------------------------------------------------*/

	CLI_WaitSema(iopSND_gi_RequestLock);

	if(pst_ReadVoiceRq->i_Request != iopSND_Cte_DmaFree)
	{
		*_pi_voice = pst_ReadVoiceRq->i_SoftBufferIndex;
		*_pi_request = pst_ReadVoiceRq->i_Request;
		pst_ReadVoiceRq->i_Request = iopSND_Cte_DmaFree;
		pst_ReadVoiceRq->i_SoftBufferIndex = -1;

		if(++pst_ReadVoiceRq == &iopSND_ast_VoiceRqQueueOnDma[iopSND_Cte_DmaRqNb])
			pst_ReadVoiceRq = &iopSND_ast_VoiceRqQueueOnDma[0];

		CLI_SignalSema(iopSND_gi_RequestLock);

		iopSND_M_CheckSoftBuffIdxOrReturn(*_pi_voice, -1);

		if(*_pi_voice)
		{
			__SoftLock__;
			ui_Flag = iopSND_sa_SoftBuffer[*_pi_voice].ui_Flag;
			__SoftUnlock__;
			if(ui_Flag == iopSND_Cte_SndFlg_Free) *_pi_request = iopSND_Cte_DmaCancel;
		}

		ret = 0;
	}
	else
	{
		CLI_SignalSema(iopSND_gi_RequestLock);
		ret = -1;
	}

	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_RequestDelete(int _i_voice)
{
	/*~~*/
	int i;
	/*~~*/

	iopSND_M_CheckSoftBuffIdxOrReturn(_i_voice, ;);

	CLI_WaitSema(iopSND_gi_RequestLock);
	for(i = 0; i < iopSND_Cte_DmaRqNb; i++)
	{
		if
		(
			(iopSND_ast_VoiceRqQueueOnDma[i].i_SoftBufferIndex == _i_voice)
		&&	(iopSND_ast_VoiceRqQueueOnDma[i].i_Request != iopSND_Cte_DmaWaitingStreamLoadAndPlay)
		)
		{
			iopSND_ast_VoiceRqQueueOnDma[i].i_Request = iopSND_Cte_DmaCancel;
		}
	}

	CLI_SignalSema(iopSND_gi_RequestLock);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_RequestReplace(int _i_voice, int iOld, int iNew)
{
	/*~~*/
	int i;
	/*~~*/

	iopSND_M_CheckSoftBuffIdxOrReturn(_i_voice, ;);

	CLI_WaitSema(iopSND_gi_RequestLock);
	for(i = 0; i < iopSND_Cte_DmaRqNb; i++)
	{
		if(iopSND_ast_VoiceRqQueueOnDma[i].i_SoftBufferIndex == _i_voice)
		{
			if(iopSND_ast_VoiceRqQueueOnDma[i].i_Request == iOld) iopSND_ast_VoiceRqQueueOnDma[i].i_Request = iNew;
		}
	}

	CLI_SignalSema(iopSND_gi_RequestLock);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopSND_RequestRegister(int _i_Voice, int i_mode)
{
	iopSND_M_CheckSoftBuffIdxOrReturn(_i_Voice, ;);
	_i_Voice &= 0x0000FFFF;
	iopSND_RequestWrite(_i_Voice, i_mode);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void case__DmaStopStream(int i_voice, int dma)
{
	dbgM_TraceSeq((__dbgH "stop-stream %04x, dma %d\n", i_voice, dma));
	dma = dma;

	iopSND_RequestDelete(i_voice);
	iopSND_SchedulerResetBuffer(i_voice);
	__SoftLock__;
	iopSND_M_SetState(iopSND_sa_SoftBuffer[i_voice].ui_Flag, iopSND_Cte_SndFlg_Stop);
	iopSND_sa_SoftBuffer[i_voice].ui_Flag &= ~iopSND_Cte_StreamIsPlayingNow;
	__SoftUnlock__;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int case__DmaWaitingStream0(int i_voice, int dma)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			ui_Flag;
	int						tmp, signaldma;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	signaldma = 1;
	__SoftLock__;
	ui_Flag = iopSND_sa_SoftBuffer[i_voice].ui_Flag;
	__SoftUnlock__;

	if(ui_Flag & iopSND_Cte_SndFlg_Play)
	{
		tmp = iopSND_i_SchedulerPlayTest(i_voice);
		switch(tmp)
		{
		case 2:
			dbgM_TraceSeq((__dbgH "autostop %04x, dma %d\n", i_voice, dma));
			iopSND_i_DirectStopVoice(i_voice);
			__SoftLock__;
			iopSND_sa_SoftBuffer[i_voice].ui_Flag |= iopSND_Cte_AutoStopped;
			iopSND_sa_SoftBuffer[i_voice].ui_Flag &= ~iopSND_Cte_Loaded;
			__SoftUnlock__;
			case__DmaStopStream(i_voice, dma);
			break;

		case 1:
			//if(iopMTX_b_EnableReading(i_voice))
			{
				dbgM_TraceSeq((__dbgH "start-loading0 %04x, dma %d\n", i_voice, dma));
				iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[0] = en_BuffWritting;
				iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading0);
				iopSND_i_SchedulerReadFile(dma, i_voice);
				signaldma = iopSND_i_SchedulerTransfert(dma, i_voice, iopSND_Cte_DmaTransStream0);
				break;
			}

		case 0:
			iopSND_RequestWrite(i_voice, iopSND_Cte_DmaWaitingStream0);
			break;

		default:
		    dbgM_TraceSeq((__dbgH "ERROR %04x, dma %d\n", i_voice, dma));
			/* iopDbg_M_ErrX(iopDbg_Err_008D "-1 S%04x : %d", i_voice, tmp); */
			break;
		}
	}
	else
	{
		/*
		 * iopDbg_M_ErrX(iopDbg_Err_008D "-2 S%04x : lastcmd %x flag %x", i_voice, tmp,
		 * ui_Flag);
		 */
		dbgM_TraceSeq((__dbgH "bad state of stream %04x, dma %d\n", i_voice, dma));
	}

	return signaldma;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int case__DmaWaitingStream1(int i_voice, int dma)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	ui_Flag;
	int				tmp, signaldma;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	signaldma = 1;

	__SoftLock__;
	ui_Flag = iopSND_sa_SoftBuffer[i_voice].ui_Flag;
	__SoftUnlock__;

	if(ui_Flag & iopSND_Cte_SndFlg_Play)
	{
		tmp = iopSND_i_SchedulerPlayTest(i_voice);
		switch(tmp)
		{
		case 2:
			dbgM_TraceSeq((__dbgH "autostop %04x, dma %d\n", i_voice, dma));
			iopSND_i_DirectStopVoice(i_voice);
			__SoftLock__;
			iopSND_sa_SoftBuffer[i_voice].ui_Flag |= iopSND_Cte_AutoStopped;
			iopSND_sa_SoftBuffer[i_voice].ui_Flag &= ~iopSND_Cte_Loaded;
			__SoftUnlock__;
			case__DmaStopStream(i_voice, dma);
			break;

		case 0:
			//if(iopMTX_b_EnableReading(i_voice))
			{
				dbgM_TraceSeq((__dbgH "start-loading1 %04x, dma %d\n", i_voice, dma));
				iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[1] = en_BuffWritting;
				iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading1);
				iopSND_i_SchedulerReadFile(dma, i_voice);
				signaldma = iopSND_i_SchedulerTransfert(dma, i_voice, iopSND_Cte_DmaTransStream1);
				break;
			}

		case 1:
			iopSND_RequestWrite(i_voice, iopSND_Cte_DmaWaitingStream1);
			break;

		default:
		    dbgM_TraceSeq((__dbgH "ERROR %04x, dma %d\n", i_voice, dma));
			/* iopDbg_M_ErrX(iopDbg_Err_008D "-3 S%04x : %d", i_voice, tmp); */
			break;
		}
	}
	else
	{
		/*
		 * iopDbg_M_ErrX(iopDbg_Err_008D "-4 S%04x : lastcmd %x flag %x", i_voice, tmp,
		 * ui_Flag);
		 */
		dbgM_TraceSeq((__dbgH "bad state of stream %04x, dma %d\n", i_voice, dma));
	}

	return signaldma;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int case__DmaStreamLoading1(int i_voice, int dma)
{
	/*~~~~~~~~~~*/
	int signaldma;
	/*~~~~~~~~~~*/

	if((iopSND_sa_SoftBuffer[i_voice].i_LastDma != -1) && (iopSND_sa_SoftBuffer[i_voice].i_LastDma != dma))
	{
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading1);
		signaldma = 1;
	}
	else if(iopSND_sa_SoftBuffer[i_voice].ui_Flag & iopSND_Cte_Stereo)
	{
	    dbgM_TraceSeq((__dbgH "end-loading1 %04x, dma %d\n", i_voice, dma));
		dbgM_TraceSeq((__dbgH "start-loading3 %04x, dma %d\n", i_voice, dma));
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[1] = en_BuffWritten;
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[3] = en_BuffWritting;
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading3);
		signaldma = iopSND_i_SchedulerTransfert(dma, i_voice, iopSND_Cte_DmaTransStream3);
	}
	else 
	{
		dbgM_TraceSeq((__dbgH "end-loading1 %04x, dma %d\n", i_voice, dma));
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[1] = en_BuffWritten;
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaWaitingStream0);
		signaldma = 1;
	}

	return signaldma;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int case__DmaStreamLoading3(int i_voice, int dma)
{
	/*~~~~~~~~~~~~~~*/
	int signaldma = 1;
	/*~~~~~~~~~~~~~~*/

	if((iopSND_sa_SoftBuffer[i_voice].i_LastDma != -1) && (iopSND_sa_SoftBuffer[i_voice].i_LastDma != dma))
	{
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading3);
	}
	else
	{
		dbgM_TraceSeq((__dbgH "end-loading3 %04x, dma %d\n", i_voice, dma));
		dbgM_TraceSeq((__dbgH "wait-loading0 %04x, dma %d\n", i_voice, dma));
		
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[3] = en_BuffWritten;
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaWaitingStream0);
	}

	return signaldma;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int case__DmaStreamLoading0(int i_voice, int dma)
{
	/*~~~~~~~~~~*/
	int signaldma;
	/*~~~~~~~~~~*/

	if((iopSND_sa_SoftBuffer[i_voice].i_LastDma != -1) && (iopSND_sa_SoftBuffer[i_voice].i_LastDma != dma))
	{
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading0);
		signaldma = 1;
	}
	else if(iopSND_sa_SoftBuffer[i_voice].ui_Flag & iopSND_Cte_Stereo)
	{
	    dbgM_TraceSeq((__dbgH "end-loading0 %04x, dma %d\n", i_voice, dma));
		dbgM_TraceSeq((__dbgH "start-loading2 %04x, dma %d\n", i_voice, dma));
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[0] = en_BuffWritten;
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[2] = en_BuffWritting;
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading2);
		signaldma = iopSND_i_SchedulerTransfert(dma, i_voice, iopSND_Cte_DmaTransStream2);
	}
	else
	{
	    dbgM_TraceSeq((__dbgH "end-loading0 %04x, dma %d\n", i_voice, dma));
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[0] = en_BuffWritten;
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaWaitingStream1);
		signaldma = 1;
	}

	return signaldma;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int case__DmaStreamLoading2(int i_voice, int dma)
{
	/*~~~~~~~~~~~~~~*/
	int signaldma = 1;
	/*~~~~~~~~~~~~~~*/

	if((iopSND_sa_SoftBuffer[i_voice].i_LastDma != -1) && (iopSND_sa_SoftBuffer[i_voice].i_LastDma != dma))
	{
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading2);
	}
	else
	{
		dbgM_TraceSeq((__dbgH "end-loading2 %04x, dma %d\n", i_voice, dma));
		dbgM_TraceSeq((__dbgH "wait-loading1 %04x, dma %d\n", i_voice, dma));
		
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[2] = en_BuffWritten;
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaWaitingStream1);
	}

	return signaldma;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int case__DmaWaitingStreamLoadAndPlay(int i_voice, int dma)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					tmp, signaldma;
	IOP_tdst_StrFile	stStr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	tmp = iopSND_i_ReinitAndPlayStream_r(dma, i_voice);
	iopSND_vai_StreamBufferCpt[i_voice - 1].i_ChangeCount = 0;
	iopSND_vai_StreamBufferCpt[i_voice - 1].i_BufferId = 0;
	iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[0] = en_BuffWritting;
	iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[1] = en_BuffInit;
	iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[2] = en_BuffInit;
	iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[3] = en_BuffInit;



	if(tmp == 0)
	{
	    dbgM_TraceSeq((__dbgH "detect mono streaming %04x, dma %d\n", i_voice, dma));
	    
		if(iopSND_sa_SoftBuffer[i_voice].s_StrFileIdx != -1)
			IOP_StrFileFree(iopSND_sa_SoftBuffer[i_voice].s_StrFileIdx);
		iopSND_sa_SoftBuffer[i_voice].s_StrFileIdx = -1;
		
		iopSND_sa_SoftBuffer[i_voice].ui_DbleBuffSize = SND_Cte_MaxBufferSize >> 1;
		
		
		iopSND_i_SchedulerReadFile(dma, i_voice);
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaRunningStreamLoadAndPlay);
		signaldma = iopSND_i_SchedulerTransfert(dma, i_voice, iopSND_Cte_DmaTransStream0);
	}
	else
	{
	    dbgM_TraceSeq((__dbgH "detect stereo streaming %04x, dma %d\n", i_voice, dma));
	
		iopSND_sa_SoftBuffer[i_voice].ui_DbleBuffSize = SND_Cte_MaxBufferSize >> 2;
		stStr.i_FileHandler = iopSND_vi_BigFd;
		stStr.ui_FilePosition = iopSND_sa_SoftBuffer[i_voice].ui_DataPos;
		stStr.ui_FileSize = iopSND_sa_SoftBuffer[i_voice].ui_DataSize;
		stStr.ui_MuxFrameSize = iopSND_sa_SoftBuffer[i_voice].ui_DbleBuffSize;
		stStr.us_Channel = 2;
		stStr.ui_BufferSize = stStr.ui_MuxFrameSize * stStr.us_Channel;
		
		if(iopSND_sa_SoftBuffer[i_voice].s_StrFileIdx == -1)
		{
		    dbgM_TraceSeq((__dbgH "call IOP_i_StrFileInit %04x, dma %d\n", i_voice, dma));
		    iopSND_sa_SoftBuffer[i_voice].s_StrFileIdx = IOP_i_StrFileInit(&stStr);
		}
		else
		{
		    dbgM_TraceSeq((__dbgH "call IOP_i_StrFileReinit %04x, dma %d\n", i_voice, dma));
		    IOP_i_StrFileReinit((int)iopSND_sa_SoftBuffer[i_voice].s_StrFileIdx, stStr.ui_FilePosition, stStr.ui_FileSize);
		}
			

		iopSND_i_SchedulerReadFile(dma, i_voice);
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading0AndPlayStereo);
		signaldma = iopSND_i_SchedulerTransfert(dma, i_voice, iopSND_Cte_DmaTransStream0);
	}

	return signaldma;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int case__DmaStreamLoading0AndPlayStereo(int i_voice, int dma)
{
	/*~~~~~~~~~~*/
	int signaldma;
	/*~~~~~~~~~~*/

	if((iopSND_sa_SoftBuffer[i_voice].i_LastDma != -1) && (iopSND_sa_SoftBuffer[i_voice].i_LastDma != dma))
	{
		dbgM_TraceSeq((__dbgH "invalid %04x, dma %d\n", i_voice, dma));
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading0AndPlayStereo);
		signaldma = 1;
	}
	else
	{
		dbgM_TraceSeq((__dbgH "valid %04x, dma %d\n", i_voice, dma));
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[0] = en_BuffWritten;
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[2] = en_BuffWritting;
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading2AndPlayStereo);
		signaldma = iopSND_i_SchedulerTransfert(dma, i_voice, iopSND_Cte_DmaTransStream2);
	}

	return signaldma;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void case__DmaStreamLoading2AndPlayStereo(int i_voice, int dma)
{
	if((iopSND_sa_SoftBuffer[i_voice].i_LastDma != -1) && (iopSND_sa_SoftBuffer[i_voice].i_LastDma != dma))
	{
		dbgM_TraceSeq((__dbgH "invalid %04x, dma %d\n", i_voice, dma));
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaStreamLoading2AndPlayStereo);
	}
	else
	{
		dbgM_TraceSeq((__dbgH "valid %04x, dma %d\n", i_voice, dma));
		iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[2] = en_BuffWritten;

		__SoftLock__;
		iopSND_sa_SoftBuffer[i_voice].ui_Flag |= iopSND_Cte_Loaded;
		__SoftUnlock__;

		iopSND_i_PlayVoice(i_voice);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void case__DmaRunningStreamLoadAndPlay(int i_voice, int dma)
{
	if((iopSND_sa_SoftBuffer[i_voice].i_LastDma != -1) && (iopSND_sa_SoftBuffer[i_voice].i_LastDma != dma))
	{
		dbgM_TraceSeq((__dbgH "end of 1stB0 delayed %04x, dma %d\n", i_voice, dma));
		iopSND_RequestWrite(i_voice, iopSND_Cte_DmaRunningStreamLoadAndPlay);
		return;
	}

	
	
	dbgM_TraceSeq((__dbgH "1stB0 loaded %04x, dma %d\n", i_voice, dma));
	iopSND_vai_StreamBufferCpt[i_voice - 1].e_BufferState[0] = en_BuffWritten;

	__SoftLock__;
	iopSND_sa_SoftBuffer[i_voice].ui_Flag |= iopSND_Cte_Loaded;
	__SoftUnlock__;

	iopSND_i_PlayVoice(i_voice);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void case__DmaPlayRq(int i_voice, int dma)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	ui_Flag;
	/*~~~~~~~~~~~~~~~~~~~~*/

#pragma unused(dma)
	__SoftLock__;
	ui_Flag = iopSND_sa_SoftBuffer[i_voice].p_DuplicateSrc ? iopSND_sa_SoftBuffer[i_voice].p_DuplicateSrc->ui_Flag : iopSND_sa_SoftBuffer[i_voice].ui_Flag;
	__SoftUnlock__;

	if(ui_Flag & iopSND_Cte_Loaded)
	{
		iopSND_i_PlayVoice(i_voice);
	}
	else
	{
		iopSND_RequestRegister(i_voice, iopSND_Cte_DmaPlayRq);
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* PSX2_IOP */
