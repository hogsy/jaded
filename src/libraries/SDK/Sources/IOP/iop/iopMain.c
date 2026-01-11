/*$T iopMain.c GC! 1.097 05/11/02 11:04:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP

/*$T iopMain.c GC! 1.097 05/11/02 11:04:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$off*/
/*
 * thread priorities lib2.04 
 *
 *  8  loader 
 * 10  reboot 
 * 16  sio2man 
 * 18  (reserved) 
 * 20  padman, mtapman(high) 
 * 24  rspu2drv, sdrdrv 
 * 28  i.link(high) 
 * 30  usb(high) 
 * 34  i.link(low) 
 * 36  usb(low) 
 * 40  an986 (reserved) 
 * 42  mcman,mcxman 
 * 46  padman, mtapman(low)
 * 48  inet (reserved) 
 * 68  ppp 
 * 81  cdvdfsv 
 * 88  loadmodule, (usbmload)
 * 96  fileio 
 * 104 mcserv,mcxserv 
 *
 * use priorities between USER_HIGHEST_PRIORITY & USER_LOWEST_PRIORITY
 *
 * < Specifying the thread priority at booting > 
 * The priority can be specified by passing a parameter when loading a module with the
 * sceSifLoadModule() function. This feature is not incorporated in cdvdfsv.irx.
 * In the case of padman.irx: 
 *   char* mes = "thpri=32,34"; <- Specify the value you want to change 
 *   sceSifLoadModule( "host0:/usr/local/sce/iop/modules/padman.irx",
 *      strlen(mes)+1, mes ); 
 *
 * < Changing the thread priority during operation > 
 * The scexxxChangeThreadPriority() function can be used to change threads in each
 * module. Replace xxx with the applicable module name. This feature is not
 * incorporated in sio2man.irx and padman.irx. For details, refer to the SIF
 * system (sif.doc) document and/or the applicable library reference.
 *
 */
/*$on*/

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

/*$2- sce ------------------------------------------------------------------------------------------------------------*/

#include <kernel.h>
#include <sif.h>
#include <sifrpc.h>
#include <stdio.h>
#include <libcdvd.h>
#include <sdmacro.h>
#include <sys/fcntl.h>


/*$2- IOP ------------------------------------------------------------------------------------------------------------*/

#include "iop/iopBAStypes.h"
#include "iop/iopDebug.h"
#include "RPC_Manager.h"
#include "iop/iopMEM.h"
#include "iop/iopCLI.h"
#include "iop/iopKernel.h"

#ifdef PSX2_USE_iopCDV
#include "CDV_Manager.h"
#endif
#include "iop/iopMain.h"

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/ps2/ps2SND.h"
#include "iop/iopSND.h"
#include "iop/iopSND_Voice.h"
#include "iop/iopSND_Fx.h"
#include "iop/iopSND_Hard.h"
#include "iop/iopSND_DmaScheduler.h"
#include "iop/iopSND_Debug.h"
#include "iop/iopSND_Stream.h"

/*$2- mux ------------------------------------------------------------------------------------------------------------*/

#include "iop/iopMTX.h"


/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

static int		si_iopSupervisorThread(void);
static int		si_iopRPCThread(void);
static void		*sv_Server(unsigned int _command, void *data, int size);
static int		si_GetMaxSize(RPC_tdst_VArg_Check *);
int				start(void);

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

ModuleInfo		Module = { "iopprg", 0x0101 };
char			*s_iopSifBuffer = NULL;
char			*si_SrvRet = NULL;
volatile int	si_SupThId;
int				IOP_gi_RPC_Thread;
int				iopMEM_Sema;
int				iopCLI_Sema;
int				iopCLI_FileStreamingSema;
volatile int	IOP_vi_StackSize[ThIdxNumber];
int IOP_gi_Intr;
int							iopCDV_gi_DiscType = SCECdPS2CD;

/*$4
 ***********************************************************************************************************************
    entry point
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int start(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	struct ThreadParam	param;
#ifdef PSX2_USE_iopCDV
	int diskType = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	

	sceCdInit(SCECdINIT);
	iopCDV_gi_DiscType = diskType = sceCdGetDiskType();
	switch(diskType)
	{
		case SCECdPS2DVD:
			sceCdMmode(SCECdDVD);
			break;
		case SCECdPS2CDDA:
			sceCdMmode(SCECdCD);
			break;
		case SCECdPS2CD:
			sceCdMmode(SCECdCD);
			break;
		case SCECdPSCDDA:
			sceCdMmode(SCECdCD);
			break;
		case SCECdPSCD:
			sceCdMmode(SCECdCD);
			break;
		default:
		    break;
	}
	
	sceCdDiskReady(0);
#endif /* PSX2_USE_iopCDV */

	param.attr = TH_C;
	param.entry = si_iopSupervisorThread;
	param.initPriority = IOP_Cte_ThPriority_Supervisor;
	IOP_vi_StackSize[ThIdxSupervisor] = param.stackSize = IOP_Cte_ThStackSize_Supervisor;
	param.option = 0;
	si_SupThId = L_CreateThread(&param);
	if(si_SupThId > 0)
	{
		StartThread(si_SupThId, 0);
		return RESIDENT_END;
	}
	else
	{
		iopDbg_M_Err(iopDbg_Err_1000);
		return NO_RESIDENT_END;
	}
}

/*$4
 ***********************************************************************************************************************
    private functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int si_iopSupervisorThread(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	struct ThreadParam	param;
	struct SemaParam	sem;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	iopDbg_M_Msg(iopDbg_Msg_0000);

	if(!sceSifCheckInit()) sceSifInit();
	sceSifInitRpc(0);

	/*$2- create all semaphores --------------------------------------------------------------------------------------*/

	sem.initCount = 0;
	sem.maxCount = 1;
	sem.attr = AT_THPRI;
	iopSND_gi_RequestLock = L_CreateSema(&sem);
	iopDbg_M_Assert((iopSND_gi_RequestLock > 0), iopDbg_Err_005B " iopSND_gi_RequestLock");

	sem.initCount = 0;
	sem.maxCount = iopSND_Cte_DmaRqNb;
	sem.attr = AT_THPRI;
	iopSND_gi_PendingDmaRqSema = L_CreateSema(&sem);
	iopDbg_M_Assert((iopSND_gi_PendingDmaRqSema > 0), iopDbg_Err_005B " iopSND_gi_PendingDmaRqSema");

	sem.initCount = 0;
	sem.maxCount = 2;
	sem.attr = AT_THPRI;
	iopSND_gi_DmaSignalSema = L_CreateSema(&sem);
	iopDbg_M_Assert((iopSND_gi_DmaSignalSema > 0), iopDbg_Err_005B " iopSND_gi_DmaSignalSema");

	sem.initCount = 0;
	sem.maxCount = 1;
	sem.attr = AT_THPRI;
	iopMEM_Sema = L_CreateSema(&sem);
	iopDbg_M_Assert((iopMEM_Sema > 0), iopDbg_Err_005B " iopMEM_Sema");

	sem.initCount = 0;
	sem.maxCount = 1;
	sem.attr = AT_THPRI;
	iopCLI_Sema = L_CreateSema(&sem);
	iopDbg_M_Assert((iopCLI_Sema > 0), iopDbg_Err_005B " iopCLI_Sema");

	sem.initCount = 0;
	sem.maxCount = 1;
	sem.attr = AT_THPRI;
	iopCLI_FileStreamingSema = L_CreateSema(&sem);
	iopDbg_M_Assert((iopCLI_FileStreamingSema > 0), iopDbg_Err_005B " iopCLI_FileStreamingSema");

	sem.initCount = 0;
	sem.maxCount = 1;
	sem.attr = AT_THPRI;
	iopSND_gi_HardLock = L_CreateSema(&sem);
	iopDbg_M_Assert((iopSND_gi_HardLock > 0), iopDbg_Err_005B " iopSND_gi_HardLock");

	sem.initCount = 0;
	sem.maxCount = 1;
	sem.attr = AT_THPRI;
	iopSND_gi_SoftLock = L_CreateSema(&sem);
	iopDbg_M_Assert((iopSND_gi_SoftLock > 0), iopDbg_Err_005B " iopSND_gi_SoftLock");

	/*$2- RPC server thread ------------------------------------------------------------------------------------------*/

	param.attr = TH_C;
	param.entry = si_iopRPCThread;
	param.initPriority = IOP_Cte_ThPriority_RPCServer;
	IOP_vi_StackSize[ThIdxRPCServer] = param.stackSize = IOP_Cte_ThStackSize_RPCServer;
	param.option = 0;
	IOP_gi_RPC_Thread = L_CreateThread(&param);
	if(IOP_gi_RPC_Thread > 0)
	{
		iopDbg_M_Msg(iopDbg_Msg_0008);
		StartThread(IOP_gi_RPC_Thread, 0);
	}
	else
	{
		iopDbg_M_Err(iopDbg_Err_0001);
	}

	/*$2- unlock data ------------------------------------------------------------------------------------------------*/

	CLI_SignalSema(iopCLI_FileStreamingSema);
	CLI_SignalSema(iopCLI_Sema);
	CLI_SignalSema(iopSND_gi_HardLock);
	CLI_SignalSema(iopSND_gi_SoftLock);

	iopMEM_i_InitModule();

	/*$2- CLI module -------------------------------------------------------------------------------------------------*/

	iopCLI_InitModule();
	iopMTX_InitModule();

	/*$2- supervisor thread becomes the sound one -----------------------------------------------------------------------------------------------*/

	iopSND_gi_SndThreadVoice = si_SupThId;
	iopSND_gi_SndThreadVoicePrio = IOP_Cte_ThPriority_SoundEvent;

	L_ChangeThreadPriority(iopSND_gi_SndThreadVoice, iopSND_gi_SndThreadVoicePrio);
	iopSND_DmaThread();


	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int si_iopRPCThread(void)
{
	/*~~~~~~~~~~~~~~~*/
	sceSifQueueData qd;
	sceSifServeData sd;
	/*~~~~~~~~~~~~~~~*/

#ifdef INUM_DMA_4
	EnableIntr(INUM_DMA_4); /* CORE0 DMA interrupt */
	EnableIntr(INUM_DMA_7); /* CORE1 DMA interrupt */
#endif	

	s_iopSifBuffer = iopMEM_pv_iopAllocAlign(RPC_Cte_BuffSize_Srv, 64);
	si_SrvRet = iopMEM_pv_iopAllocAlign(RPC_Cte_BuffSize_Srv, 64);

	sceSifSetRpcQueue(&qd, GetThreadId());
	sceSifRegisterRpc(&sd, RPC_Cte_SrvId, sv_Server, (void *) s_iopSifBuffer, NULL, NULL, &qd);
	sceSifRpcLoop(&qd);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void *sv_Server(unsigned int _command, void *data, int _size)
{
	/*~~~~~~~~~~~~~~~~~*/
	void		*pv;
	int			tmp;
#ifndef _FINAL_
	static int	call = 0;
#endif
    /*~~~~~~~~~~~~~~~~~*/

	/* only for silly codewarrior warning */
	_size = _size;

	*(int *) si_SrvRet = 0;
	pv = (void *) si_SrvRet;

	tmp = *(int *) data;

//    printf("iop:%x\n", _command);
#if defined(_FINAL_) && defined(PSX2_USE_iopCDV)
//    printf(" ");
#endif    
	switch(_command)
	{

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    RPC
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case RPC_Cmd_RPCFlushCommand:
		{
			/*~~~~~~~~~~~~~~*/
			int		i_RqNb;
			char	*p_RqBuff;
			int		size, rq, total;
			/*~~~~~~~~~~~~~~*/

			i_RqNb = *(int *) data;
			p_RqBuff = (char *) data + 4;
			total = 0;

			while(i_RqNb)
			{
				rq = *(int *) p_RqBuff;
				p_RqBuff += 4;
				size = *(int *) p_RqBuff;
				p_RqBuff += 4;
				sv_Server(rq, p_RqBuff, size);
				p_RqBuff += size;
				i_RqNb--;
				total += size;
#ifndef _FINAL_
				if(total > RPC_Cte_BuffSize_Srv)
				{
				    printf("*** FLUSH OVERFLOW\n");
				}
#endif				
			}
		}
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    sound
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/*$1- module init/close ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case RPC_Cmd_SndInit:
		*(int *) si_SrvRet = iopSND_i_InitModule();
		break;
	case RPC_Cmd_SndClose:
		*(int *) si_SrvRet = iopSND_i_CloseModule();
		break;
	case RPC_Cmd_SndBigOpen:
		*(int *) si_SrvRet = iopSND_i_BigOpen((char *) data);
		break;
		
	case RPC_Cmd_SndRenderMode:
	    iopSND_gi_RenderMode = 	*(int*)data;
	    break;

	/*$1- specialy for stream ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case RPC_Cmd_SndStreamReinitAndPlayLong:
		iopSND_ReinitAndPlayStreamLong(data);
		break;

	case RPC_Cmd_SndStreamChain:
		iopSND_StreamChain((RPC_tdst_SndStreamChain*)data);
		break;

	case RPC_Cmd_SndCreateBufferForceStream:
		*(int *) si_SrvRet = iopSND_i_CreateStreamBuffer(data);
		break;

	case RPC_Cmd_SndStreamPrefetchArray:
		*(int *) si_SrvRet = iopSND_i_StreamPrefetchArray((RPC_tdst_VArg_SndStreamPrefetch *) data, &si_SrvRet[4]);
		break;

	case RPC_Cmd_SndStreamPrefetch:
		iopSND_StreamIopPrefetch((RPC_tdst_VArg_SndStreamPrefetch *) data);
		break;
	case RPC_Cmd_SndStreamPrefetchStatus:
		iopSND_StreamIopPrefetchStatus((unsigned int*)&si_SrvRet[4]);
		break;
	case RPC_Cmd_SndStreamFlush:
		iopSND_StreamIopFlush((RPC_tdst_VArg_SndStreamFlush *) data);
		break;
		
	case RPC_Cmd_SndStreamSetPos:
	    iopSND_StreamSetPos(data);
	    break;

	case RPC_Cmd_SndStreamGetPos:
	    iopSND_StreamGetPos(*(int*)data, &si_SrvRet[4]);
	    break;

	case RPC_Cmd_SndStreamLoopCountGet:
		iopSND_StreamLoopCountGet(*(int*)data, &si_SrvRet[4]);
		break;

	case RPC_Cmd_SndStreamChainDelayGet:
		iopSND_StreamChainDelayGet(*(int*)data, &si_SrvRet[4]);
		break;

	/*$1- sound buffer operations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case RPC_Cmd_SndReloadBuffer:
		*(int *) si_SrvRet = iopSND_i_ReloadBuffer(data);
		break;

	case RPC_Cmd_SndCreateBuffer:
		*(int *) si_SrvRet = iopSND_i_CreateVoiceBuffer(data);
		break;

	case RPC_Cmd_SndDuplicateBuffer:
		*(int *) si_SrvRet = iopSND_i_DuplicateVoiceBuffer(data);
		break;


	case RPC_Cmd_SndPauseStream:
	case RPC_Cmd_SndPause:
		break;

	case RPC_Cmd_SndStopStream:
	case RPC_Cmd_SndStop:
		iopSND_FxDelVoice(*(int *) data);
		*(int *) si_SrvRet = iopSND_i_StopVoice(*(int *) data);
		break;

	case RPC_Cmd_SndSetVolStream:
	case RPC_Cmd_SndSetVol:
		*(int *) si_SrvRet = iopSND_i_SetVoiceVol(*(int *) data, ((int *) data)[1], ((int *) data)[2]);
		break;

	case RPC_Cmd_SndSetMasterVol_spe:
	case RPC_Cmd_SndSetMasterVol:
		*(int *) si_SrvRet = iopSND_i_SetMasterVol(*(int *) data, ((int *) data)[1]);
		break;

	case RPC_Cmd_SndGetStatus:
		if(!iopSND_gb_EnableSound)
		{
			*(int *) si_SrvRet = -1;
			break;
		}

		*(int *) si_SrvRet = iopSND_i_GetVoiceStatus(data, &si_SrvRet[4]);
		break;

	case RPC_Cmd_SndSetFreqStream:
	case RPC_Cmd_SndSetFreq:
		*(int *) si_SrvRet = iopSND_i_SetVoiceFreq(*(int *) data, ((int *) data)[1]);
		break;
	
	/*$1- PLAY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case RPC_Cmd_SndPlay:
		iopSND_FxDelVoice(*(int *) data);
		*(int *) si_SrvRet = iopSND_i_PlayNoLoopVoice((RPC_tdst_Play*) data);
		break;

	case RPC_Cmd_SndPlayLoop:
		iopSND_FxDelVoice(*(int *) data);
		*(int *) si_SrvRet = iopSND_i_PlayLoopVoice((RPC_tdst_Play*) data);
		break;
		
	case RPC_Cmd_SndPlayFxA:
		iopSND_FxAddVoice(*(int *) data, SD_CORE_0);
		*(int *) si_SrvRet = iopSND_i_PlayNoLoopVoice((RPC_tdst_Play*) data);
		break;

	case RPC_Cmd_SndPlayLoopFxA:
		iopSND_FxAddVoice(*(int *) data, SD_CORE_0);
		*(int *) si_SrvRet = iopSND_i_PlayLoopVoice((RPC_tdst_Play*) data);
		break;
	
	case RPC_Cmd_SndPlayFxB:
		iopSND_FxAddVoice(*(int *) data, SD_CORE_1);
		*(int *) si_SrvRet = iopSND_i_PlayNoLoopVoice((RPC_tdst_Play*) data);
		break;

	case RPC_Cmd_SndPlayLoopFxB:
		iopSND_FxAddVoice(*(int *) data, SD_CORE_1);
		*(int *) si_SrvRet = iopSND_i_PlayLoopVoice((RPC_tdst_Play*) data);
		break;
	
	/*$1- sound FX ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case RPC_Cmd_SndSetFx:
		*(int *) si_SrvRet = iopSND_i_FxSet((RPC_tdst_FxSettings *) data);
		break;

	/*$1- specialy for bin mode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case RPC_Cmd_SndStopAllStream:
	    {
	        extern unsigned int KKui_seek;
	        KKui_seek = 0;
	    }
		iopSND_StopAllStream();
		break;

	case RPC_Cmd_SndStopScheduler:
		iopDBG_MapResetError(*(int *) data);
		break;

	case RPC_Cmd_SndStartScheduler:
		if(iopSND_vb_SchedulerLocked)
		{
			L_WakeupThread(iopSND_gi_SndThreadVoice);
			iopSND_vb_SchedulerLocked = 0;
		}
		break;
		
	case RPC_Cmd_BeginSpeedMode:
		iopSND_gb_UseBinSeek = (*(unsigned int *) data) ? TRUE : FALSE;
		iopSND_gui_BinSeek = *(unsigned int *) data;
		break;

    case RPC_Cmd_SndUnlockThread:
		if((iopSND_gi_SndThreadVoice > 0) && (iopSND_gi_SndThreadVoicePrio != *(int *) data))
		{			
			if(*(int *) data > iopSND_gi_SndThreadVoicePrio)
			{
			    // block streaming
			    iopSND_vb_SchedulerLocked = 1;			    			    
			}
			else
			{
			    // unblock streaming
			    iopSND_vb_SchedulerLocked = 0;
			    L_WakeupThread(iopSND_gi_SndThreadVoice);
			}
			
			iopSND_gi_SndThreadVoicePrio = *(int *) data;
			L_ChangeThreadPriority(iopSND_gi_SndThreadVoice, iopSND_gi_SndThreadVoicePrio);
		}
        break;
        
	case RPC_Cmd_SndPriority:
	    if(*(int*)data == -5)
	    {
	        extern void iopSND_DmaReinit(void);
	        iopSND_DmaReinit();
	        break;
	    }
	    
	    if(*(int*)data & 0x80000000)
	    {
	        extern void iopSND_DmaReinit(void);
	        *(int*)data &= 0x7FFFFFFF;
	        iopSND_DmaReinit();
	        
	    }
		if((iopSND_gi_SndThreadVoice > 0) && (iopSND_gi_SndThreadVoicePrio != *(int *) data))
		{			
			if(*(int *) data > iopSND_gi_SndThreadVoicePrio)
			{
			    // block streaming
			    iopSND_vb_SchedulerLocked = 1;			    
			}
			else
			{
			    // unblock streaming
			    iopSND_vb_SchedulerLocked = 0;
			    L_WakeupThread(iopSND_gi_SndThreadVoice);
			}
			
			iopSND_gi_SndThreadVoicePrio = *(int *) data;
			L_ChangeThreadPriority(iopSND_gi_SndThreadVoice, iopSND_gi_SndThreadVoicePrio);
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case RPC_Cmd_SndDebugInfos:
		iopDbg_M_Msg(iopDbg_Msg_0001);
		iopSND_DebugInfos(*(int *) data);
		break;
#ifdef PSX2_USE_iopCDV

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    CDV
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case RPC_Cmd_CDVInitModule:
		*(int *) si_SrvRet = iopCDV_i_InitModule();
		break;

	case RPC_Cmd_CDVCloseModule:
		*(int *) si_SrvRet = iopCDV_i_CloseModule();
		break;

	case RPC_Cmd_CDVOpenFile:
		*(int *) si_SrvRet = iopCDV_i_RemoteOpenFile((char *) data, (CDV_tdst_FileHandler *) &si_SrvRet[4]);
		break;

	case RPC_Cmd_CDVReadFile:
		CDV_Dbg_Trace("ee");
		*(int *) si_SrvRet = iopCDV_i_SyncRead(*(int *) data, ((int *) data)[1], (char *) &si_SrvRet[4]);
		CDV_Dbg_Trace("iop");
		break;

	case RPC_Cmd_CDVCloseFile:
		*(int *) si_SrvRet = iopCDV_i_CloseFile(*(int *) data);
		break;

	case RPC_Cmd_CDVGetBufferSize:
		*(int *) si_SrvRet = iopCDV_i_GetiopBufferSize(*(int *) data);
		break;

	case RPC_Cmd_CDVSetBufferSize:
		*(int *) si_SrvRet = iopCDV_i_DirectSetSize(*(int *) data, ((int *) data)[1]);
		break;

#endif /* PSX2_USE_iopCDV */

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    File
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case RPC_Cmd_FileOpen:
		*(int *) si_SrvRet = iopCLI_OpenFile((char *) data);
		break;

	case RPC_Cmd_FileClose:
		iopCLI_CloseFile(*(int *) data);
		break;

    case RPC_Cmd_FileExecAsyncRead:
        *(int *) si_SrvRet = iopCLI_i_FileExecRead((void *)data, &si_SrvRet[4]);
        break;
        
	case RPC_Cmd_FileAsyncRead:
		if(iopSND_vb_SchedulerLocked)
		{
			L_WakeupThread(iopSND_gi_SndThreadVoice);
			iopSND_vb_SchedulerLocked = 0;
		}

		*(int *) si_SrvRet = iopCLI_i_AddRequestReadFile
			(
				*(int *) data,
				((int *) data)[1],
				((unsigned int *) data)[2],
				((int *) data)[3]
			);
		break;

	case RPC_Cmd_GetFileAsyncStatus:
		*(int *) si_SrvRet = iopCLI_i_GetFileRequest(*(int *) data, &pv);
		break;
	
	case RPC_Cmd_FileAsyncReset:
	    iopCLI_ResetAllRequestReadFile();
	    break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    file streaming
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case RPC_Cmd_FileStreamDestroy:
		iopMTX_Close();
		break;

	case RPC_Cmd_FileStreamReinit:
		iopMTX_Reinit((RPC_tdst_VArg_FileStreamReinit *) data);
		break;

	case RPC_Cmd_FileStreamCreate:
		*(int *) si_SrvRet = iopMTX_i_Open((RPC_tdst_VArg_FileStreamCreate *) data);
		break;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    debug
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case RPC_Cmd_Check:
		*(int *) si_SrvRet = si_GetMaxSize((RPC_tdst_VArg_Check *) &si_SrvRet[4]);
		break;
#ifdef _DEBUG
	case RPC_Cmd_AwakeSupervisor:
		L_WakeupThread(si_SupThId);
		break;
#endif /* _DEBUG */

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    error
	 -------------------------------------------------------------------------------------------------------------------
	 */

	default:
		*(int *) si_SrvRet = -1;
		pv = (void *) si_SrvRet;
		iopDbg_M_ErrX(iopDbg_Err_001E " 0x%x", _command);
		break;
	}

#ifndef _FINAL_
	if(call++ &0x200) IOP_vi_StackSize[ThIdxRPCServer] = CheckThreadStack();
#endif
	return pv;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int si_GetMaxSize(RPC_tdst_VArg_Check *pVArg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef _FINAL_
	unsigned int	freesize;
	extern int		MEM_gi_AllocMaxSize;
	extern int		MEM_gi_FreeSizeMin;
	extern int		MEM_gi_RamSize;
	extern int		MEM_gi_AllocSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pVArg->ui_StackSizeOfSupervisorThread = IOP_vi_StackSize[ThIdxSupervisor];
	pVArg->ui_StackSizeOfRPCThread = IOP_vi_StackSize[ThIdxRPCServer];
	pVArg->ui_StackSizeOfSoundThread = IOP_vi_StackSize[ThIdxSoundEvent];

	CLI_WaitSema(iopMEM_Sema);
	freesize = QueryTotalFreeMemSize();
	freesize -= (MEM_gi_RamSize - iopMEM_Cte_RAMSize);
	pVArg->ui_SysMemoryFreeMaxSize = freesize;
	pVArg->ui_MemoryAlloc = MEM_gi_AllocSize;
	pVArg->ui_MemoryAllocMax = MEM_gi_AllocMaxSize;
	pVArg->i_MemoryFreeSpaceMin = MEM_gi_FreeSizeMin;
	CLI_SignalSema(iopMEM_Sema);

#endif
#ifdef __iopDbg_Err__
	pVArg->i_LastError = iopDBG_gi_LastErrorCode;
	pVArg->i_ErrorNumber = iopDBG_gi_ErrorNumber;
	pVArg->i_LastMapError = iopDBG_gi_LastMapErrorCode;
	pVArg->i_MapErrorNumber = iopDBG_gi_MapErrorNumber;
#else
	pVArg->i_LastError = 0;
	pVArg->i_ErrorNumber = 0;
	pVArg->i_LastMapError = 0;
	pVArg->i_MapErrorNumber = 0;
#endif
	return 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* PSX2_IOP */
