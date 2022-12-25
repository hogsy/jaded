/*$T eeRPC_Manager.c GC 1.138 09/26/05 14:32:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <sifdev.h>
#include <sifrpc.h>
#include <stdio.h>
#include <string.h>
#include <eekernel.h>
#include <libcdvd.h>

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATHfloat.h"

#include "IOP/ee/eeDebug.h"
#include "IOP/RPC_Manager.h"
#include "IOP/CDV_Manager.h"
#include "IOP/iop/iopMain.h"
#include "IOP/ee/eeRPC_Debug.h"
#include "IOP/ee/eeIOP_RamManager.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SND.h"
#include "IOP/iop/iopCLI.h"
#include "SouND/Sources/MTX.h"
#include "MainPsx2/Sources/libscedemo.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    RPC wait/no wait
 -----------------------------------------------------------------------------------------------------------------------
 */
sceSifClientData	eeRPC_gst_ClientData;

#define C_NoWaitFlag	SIF_RPCM_NOWAIT

//

#ifdef _FINAL_
#define M_sceSifCallRpc sceSifCallRpc
#define M_WaitRPC() \
	while(sceSifCheckStatRpc((sceSifRpcData *) &eeRPC_gst_ClientData) == 1) \
	{ \
	};
#else

volatile char   sceSifCallRpc_gc_IsRunning=0;
volatile int    sceSifCallRpc_gi_LastRetCode=0xcafedeca;

volatile char           sceSifCheckStatRpc_gc_IsRunning=0;
volatile unsigned int   sceSifCheckStatRpc_gui_LastCount=-1;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ int M_sceSifCallRpc
(
	sceSifClientData	*p_client,
	unsigned int		ui_cmd,
	unsigned int		ui_mode,
	void				*pv_sbuffer,
	int					i_ssize,
	void				*pv_rbuffer,
	int					i_rsize,
	sceSifEndFunc		callback,
	void				*callback_param
)
{
    sceSifCallRpc_gc_IsRunning = 1;
	sceSifCallRpc_gi_LastRetCode = sceSifCallRpc
		(
			p_client,
			ui_cmd,
			ui_mode,
			pv_sbuffer,
			i_ssize,
			pv_rbuffer,
			i_rsize,
			callback,
			callback_param
		);
    sceSifCallRpc_gc_IsRunning = 0;
    
    if(sceSifCallRpc_gi_LastRetCode < 0)
    {
        printf("[EE] ** sceSifCallRpc : error code %d\n", sceSifCallRpc_gi_LastRetCode);
    }
    return sceSifCallRpc_gi_LastRetCode;
}


_inline_ void M_WaitRPC(void)
{
    sceSifCheckStatRpc_gui_LastCount = 0;
    sceSifCheckStatRpc_gc_IsRunning = 1;
	while(sceSifCheckStatRpc((sceSifRpcData *) &eeRPC_gst_ClientData) == 1) 
	{ 
	    sceSifCheckStatRpc_gui_LastCount ++ ;
	    if(sceSifCheckStatRpc_gui_LastCount > 0xFF00000) *(int*)0xfefefefe = 55;
	};
	sceSifCheckStatRpc_gc_IsRunning = 0;
}
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifndef L_MAX_PATH
#define L_MAX_PATH	FILENAME_MAX
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CD-DVD flags
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef PSX2_USE_iopCDV
#define PSX2_BOOT_ON_DISK
#define PSX2_USE_IRX_ON_DISK
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CD-DVD paths
 -----------------------------------------------------------------------------------------------------------------------
 */

#define Cte_SysModulePath	JADEPS2_GETNAME(6)
#define Cte_OurModulePath	JADEPS2_GETNAME(6)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CD-DVD file name
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef PSX2_USE_IRX_ON_DISK
#define Cte_Image		IOP_IMAGE_FILE ";1"
#define Cte_IRX_io		"SIO2MAN.IRX;1"
#define Cte_IRX_pad		"PADMAN.IRX;1"
#define Cte_IRX_sd		"LIBSD.IRX;1"
#define Cte_IRX_mcman	"MCMAN.IRX;1"
#define Cte_IRX_mcserv	"MCSERV.IRX;1"
#else
#define Cte_Image		IOP_IMAGE_file
#define Cte_IRX_io		"sio2man.irx"
#define Cte_IRX_pad		"padman.irx"
#define Cte_IRX_sd		"libsd.irx"
#define Cte_IRX_mcman	"mcman.irx"
#define Cte_IRX_mcserv	"mcserv.irx"
#endif

/*$1- IRX on CD ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef PSX2_USE_IRX_ON_DISK
#if defined(_DEBUG)
#define Cte_IRX_iopprg	"IOPCD_CD.IRX;1"
#elif defined(_FINAL_)
#define Cte_IRX_iopprg	"IOPCD_CF.IRX;1"
#else
#define Cte_IRX_iopprg	"IOPCD_CR.IRX;1"
#endif /* _DEBUG */

/*$1- IRX for CD ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#elif defined(PSX2_USE_iopCDV)
#if defined(_DEBUG)
#define Cte_IRX_iopprg	"iopcd_cd.irx"
#elif defined(_FINAL_)
#define Cte_IRX_iopprg	"iopcd_cf.irx"
#else
#define Cte_IRX_iopprg	"iopcd_cr.irx"
#endif /* _DEBUG */

/*$1- IRX on HDD ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#else
#if defined(_DEBUG)
#define Cte_IRX_iopprg	"iop___cd.irx"
#elif defined(_FINAL_)
#define Cte_IRX_iopprg	"iop___cf.irx"
#else
#define Cte_IRX_iopprg	"iop___cr.irx"
#endif /* _DEBUG */

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#define M_RoundUpTo16(_a)	((_a & 0xFFFFFFF0) + 0x00000010)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	eeRPC_tdst_ReadRequest_
{
	int				i_FileHandler;
	unsigned int	ui_Position;
	char			*pc_FileBuffer;
	int				i_BufferSize;
	int				i_Status;
	int				i_Time;
} eeRPC_tdst_ReadRequest;

#define eeRPC_Cte_Free		0
#define eeRPC_Cte_Busy		1
#define eeRPC_Cte_Finish	2
#define eeRPC_Cte_Error		3
#define eeRPC_Cte_Waiting	4

typedef struct	eeRPC_tdst_Streaming_
{
	BOOL			b_Valid;
	int				i_FileHandler;
	unsigned int	ui_FilePosition;
	unsigned int	ui_FileSize;
} eeRPC_tdst_Streaming;

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

extern char			PSX2_gasz_BigFile[L_MAX_PATH] __attribute__((aligned(64)));
extern char			SND_gc_NoSound;
extern ULONG		ENG_gul_Loop;
extern unsigned int GSP_ui_Video;
extern int			PS2_gi_SpecialDebug;

/*$2- RPC server -----------------------------------------------------------------------------------------------------*/

char				eeRPC_ac_BuffPrioAsync[RPC_Cte_BuffSize_Srv] __attribute__((aligned(64)));
char				eeRPC_ac_BuffPrioHight[RPC_Cte_BuffSize_Srv] __attribute__((aligned(64)));
char				eeRPC_ac_BuffPrioNormal[RPC_Cte_BuffSize_Srv] __attribute__((aligned(64)));
char				eeRPC_ac_BuffPrioNormal2[RPC_Cte_BuffSize_Srv] __attribute__((aligned(64)));
char				eeRPC_ac_BuffSending[RPC_Cte_BuffSize_Srv] __attribute__((aligned(64)));
int					eeRPC_gi_CmdNbPrioHigth;
int					eeRPC_gi_CmdNbPrioNormal;
int					eeRPC_gi_CmdNbPrioNormal2;
char				*eeRPC_gp_CurrentCmdPrioHigth;
char				*eeRPC_gp_CurrentCmdPrioNormal;
char				*eeRPC_gp_CurrentCmdPrioNormal2;

BOOL				eeRPC_gb_TransfertEnabled;

/*$2- file server ----------------------------------------------------------------------------------------------------*/

eeRPC_tdst_Streaming	eeRPC_gst_Streaming;
unsigned int			eeRPC_gui_PreloadDelay;

eeRPC_tdst_ReadRequest	eeRPC_gast_ReadRequest[RPC_Cte_CLIMaxRequestNumber];
volatile int			eeRPC_gi_NbOfBusyReadRq;
int						eeRPC_gi_SizeOfRunningRq;
int						eeRPC_gi_NbOfPendingReadRq;
int						eeRPC_gi_LastBusyRq;

/*$2- file path ------------------------------------------------------------------------------------------------------*/

char	PSX2_gasz_SysModulePath[L_MAX_PATH];
char	PSX2_gasz_ModulePath[L_MAX_PATH];
int		eeRPC_gi_BF = -1;

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

static int		si_LoadModule(char *);
void			eeRPC_AsyncRqFlush(void);
static void		eeRPC_FlushCommandBuffer(int *, char **, char *);
int				eeRPC_i_DeleteCommand(int _i_Command);
static void		eeRPC_InitExecAsync(void);
void			eeRPC_ExecAsyncCallback(void *);
extern float	ps2TIM_f_RealTimeClock(void);

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_InitModule(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef PSX2_BOOT_ON_DISK
	char		path[2 * L_MAX_PATH];
	extern BOOL Demo_gb_IsActive;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- begin of sce init procedure --------------------------------------------------------------------------------*/

	sceSifInitRpc(0);
	sceCdInit(SCECdINIT);
	switch(sceCdGetDiskType())
	{
	case SCECdPS2DVD:	sceCdMmode(SCECdDVD); break;
	case SCECdPS2CD:	sceCdMmode(SCECdCD); break;
	default:			sceCdMmode(SCECdCD); break;
	}

#ifdef PSX2_USE_IRX_ON_DISK
	L_strcpy(PSX2_gasz_SysModulePath, Cte_SysModulePath);
	L_strcpy(PSX2_gasz_ModulePath, Cte_OurModulePath);
#endif /* PSX2_USE_IRX_ON_DISK */

	sprintf(path, "%s%s", PSX2_gasz_SysModulePath, Cte_Image);
	while(!sceSifRebootIop(path))
	{
	};
	while(!sceSifSyncIop())
	{
	};
#endif /* PSX2_BOOT_ON_DISK */

	sceSifInitRpc(0);
	sceSifLoadFileReset();
	sceFsReset();

#ifdef PSX2_USE_iopCDV
	sceCdInit(SCECdINIT);
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		extern BOOL Demo_gb_IsActive;
		extern u16	Demo_gus_mediatype;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/*
		 * if (Demo_gb_IsActive) { if (Demo_gus_mediatype == SCE_DEMO_MEDIATYPE_DVD)
		 * sceCdMmode(SCECdDVD); else sceCdMmode(SCECdCD); } else
		 */
		switch(sceCdGetDiskType())
		{
		case SCECdPS2DVD:	sceCdMmode(SCECdDVD); break;
		case SCECdPS2CD:	sceCdMmode(SCECdCD); break;
		default:			sceCdMmode(SCECdCD); break;
		}
	}
#endif
	IOP_gi_HeapInitResult = sceSifInitIopHeap();

	/*$2- end of sce init procedure ----------------------------------------------------------------------------------*/

	eeRPC_InitDebugModule();

	/*$2- module init ------------------------------------------------------------------------------------------------*/

	eeRPC_gb_TransfertEnabled = FALSE;

	L_memset(eeRPC_ac_BuffPrioHight, 0, RPC_Cte_BuffSize_Srv * sizeof(char));
	L_memset(eeRPC_ac_BuffPrioNormal, 0, RPC_Cte_BuffSize_Srv * sizeof(char));
	L_memset(eeRPC_ac_BuffPrioNormal2, 0, RPC_Cte_BuffSize_Srv * sizeof(char));
	L_memset(eeRPC_ac_BuffPrioAsync, 0, RPC_Cte_BuffSize_Srv * sizeof(char));

	eeRPC_gi_CmdNbPrioHigth = 0;
	eeRPC_gi_CmdNbPrioNormal = 0;
	eeRPC_gi_CmdNbPrioNormal2 = 0;

	eeRPC_gp_CurrentCmdPrioHigth = eeRPC_ac_BuffPrioHight + 4;
	eeRPC_gp_CurrentCmdPrioNormal = eeRPC_ac_BuffPrioNormal + 4;
	eeRPC_gp_CurrentCmdPrioNormal2 = eeRPC_ac_BuffPrioNormal2 + 4;

	L_memset(eeRPC_gast_ReadRequest, 0, RPC_Cte_CLIMaxRequestNumber * sizeof(eeRPC_tdst_ReadRequest));
	eeRPC_gi_NbOfBusyReadRq = 0;
	eeRPC_gi_NbOfPendingReadRq = 0;
	eeRPC_gi_SizeOfRunningRq = 0;
	eeRPC_gi_LastBusyRq = -1;

	L_memset(&eeRPC_gst_Streaming, 0, sizeof(eeRPC_tdst_Streaming));
	eeRPC_gst_Streaming.i_FileHandler = -1;
	eeRPC_gui_PreloadDelay = 50;
	eeRPC_InitExecAsync();

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_LoadModule(int i_flag)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			ret;
	char		path[2 * L_MAX_PATH];
	extern BOOL Demo_gb_IsActive;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	eeDbg_M_Msg(eeDbg_Msg_0008);
	ret = 0;

#ifdef PSX2_USE_IRX_ON_DISK
	L_strcpy(PSX2_gasz_SysModulePath, Cte_SysModulePath);
	L_strcpy(PSX2_gasz_ModulePath, Cte_OurModulePath);
#endif
	sprintf(path, "%s%s", PSX2_gasz_SysModulePath, Cte_IRX_io);
	ret = si_LoadModule(path);

	sprintf(path, "%s%s", PSX2_gasz_SysModulePath, Cte_IRX_pad);
	ret |= si_LoadModule(path);

	sprintf(path, "%s%s", PSX2_gasz_SysModulePath, Cte_IRX_mcman);
	ret |= si_LoadModule(path);

	sprintf(path, "%s%s", PSX2_gasz_SysModulePath, Cte_IRX_mcserv);
	ret |= si_LoadModule(path);

	sprintf(path, "%s%s", PSX2_gasz_SysModulePath, Cte_IRX_sd);
	ret |= si_LoadModule(path);

	sprintf(path, "%s%s", PSX2_gasz_ModulePath, Cte_IRX_iopprg);
	ret |= si_LoadModule(path);

	while(1)
	{
		if(sceSifBindRpc(&eeRPC_gst_ClientData, RPC_Cte_SrvId, 0) < 0)
		{
			eeDbg_M_Err(eeDbg_Err_0002);
			return -1;
		}

		if(eeRPC_gst_ClientData.serve) break;
	}

	eeRPC_gb_TransfertEnabled = TRUE;

	IOP_i_InitModule();

	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_OpenBigfile(void)
{
	return eeRPC_i_PushCommand
		(
			RPC_Cmd_FileOpen,
			PSX2_gasz_BigFile,
			((L_strlen(PSX2_gasz_BigFile) + 1) + 4) &~3,
			NULL,
			0
		);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_AsyncRead(int _i_File, unsigned int _ui_Pos, char *_pc_Buff, int _i_Size, int prio)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i, j;
	static u_int	uiLast = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gc_NoSound)
	{
		if(!SND_gst_Params.l_Available) return -1;
		if(ps2SND_b_IsLoading()) return -1;
	}

	if(MTX_b_CurrentFileIsMTX())
	{
		if(_ui_Pos > MTX_ui_GetVideoFilePosition() + MTX_ui_GetVideoFileSize())
		{
			return -1;
		}

		if((_ui_Pos + _i_Size) > MTX_ui_GetVideoFilePosition() + MTX_ui_GetVideoFileSize())
		{
			_i_Size = MTX_ui_GetVideoFilePosition() + MTX_ui_GetVideoFileSize() - _ui_Pos;
		}
	}

	if(GSP_ui_Video)
	{
		if(!prio)
		{
			if((ENG_gul_Loop - uiLast) < eeRPC_gui_PreloadDelay) return -1;
			uiLast = ENG_gul_Loop;
		}
	}

	if(_i_Size > iopCLI_Cte_PreLoadSize)
	{
		return -1;
	}
	else
	{
		for(i = 0; i < RPC_Cte_CLIMaxRequestNumber; i++)
		{
			if(eeRPC_gast_ReadRequest[i].i_Status == eeRPC_Cte_Free) break;
		}

		if(i == RPC_Cte_CLIMaxRequestNumber) return -1;
	}

	for(j = 0; j < RPC_Cte_CLIMaxRequestNumber; j++)
	{
		if(eeRPC_gast_ReadRequest[j].i_Status != eeRPC_Cte_Waiting) continue;
		eeRPC_gast_ReadRequest[j].i_Time++;
	}

	eeRPC_gast_ReadRequest[i].i_FileHandler = _i_File;
	eeRPC_gast_ReadRequest[i].ui_Position = _ui_Pos;
	eeRPC_gast_ReadRequest[i].pc_FileBuffer = _pc_Buff;
	eeRPC_gast_ReadRequest[i].i_BufferSize = _i_Size;
	eeRPC_gast_ReadRequest[i].i_Status = eeRPC_Cte_Waiting;
	eeRPC_gast_ReadRequest[i].i_Time = 0;
	eeRPC_gi_NbOfPendingReadRq++;

	eeRPC_AsyncRqFlush();

	return i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_GetAsyncStatus(int rq)
{
	/*~~~~~~*/
	int rcode;
	/*~~~~~~*/

	if(rq >= RPC_Cte_CLIMaxRequestNumber) return -1;
	if(rq < 0) return -1;

	switch(eeRPC_gast_ReadRequest[rq].i_Status)
	{
	case eeRPC_Cte_Finish:
		eeRPC_gast_ReadRequest[rq].i_Status = eeRPC_Cte_Free;
		rcode = 1;
		break;

	default:

	case eeRPC_Cte_Error:
		eeRPC_gast_ReadRequest[rq].i_Status = eeRPC_Cte_Free;
		rcode = -1;
		break;

	case eeRPC_Cte_Busy:
		eeRPC_gast_ReadRequest[rq].i_Status = eeRPC_Cte_Busy;
		rcode = 0;
		break;

	case eeRPC_Cte_Waiting:
		eeRPC_gast_ReadRequest[rq].i_Status = eeRPC_Cte_Waiting;
		rcode = 0;
		break;
	}

	return rcode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_CancelAsyncRq(int rq)
{
	/*~~~~~~*/
	int rcode;
	/*~~~~~~*/

	if(rq < 0) return -1;
	if(rq >= RPC_Cte_CLIMaxRequestNumber) return -1;

	switch(eeRPC_gast_ReadRequest[rq].i_Status)
	{
	case eeRPC_Cte_Busy:
		eeRPC_gast_ReadRequest[rq].pc_FileBuffer = NULL;
		rcode = 0;
		break;

	case eeRPC_Cte_Waiting:
		eeRPC_gi_NbOfPendingReadRq--;
		eeRPC_gi_SizeOfRunningRq -= eeRPC_gast_ReadRequest[rq].i_BufferSize;

	case eeRPC_Cte_Error:
	case eeRPC_Cte_Finish:
	default:
		eeRPC_gast_ReadRequest[rq].i_Status = eeRPC_Cte_Free;
		rcode = 0;
		break;
	}

	return rcode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_RefreshAsyncStatus(void)
{
	/*~~~~~~~*/
	int status;
	int rq;
	/*~~~~~~~*/

	if(!SND_gc_NoSound)
	{
		if(!SND_gst_Params.l_Available) return;
	}

	for(rq = 0; rq < RPC_Cte_CLIMaxRequestNumber; rq++)
	{
		if(eeRPC_gast_ReadRequest[rq].i_Status != eeRPC_Cte_Busy) continue;

		status = eeRPC_i_PushCommand
			(
				RPC_Cmd_GetFileAsyncStatus,
				(char *) &rq,
				sizeof(int),
				eeRPC_gast_ReadRequest[rq].pc_FileBuffer,
				eeRPC_gast_ReadRequest[rq].i_BufferSize
			);

		switch(status)
		{
		case 0:		/* Rq terminee */
			eeRPC_gast_ReadRequest[rq].i_Status = eeRPC_Cte_Finish;
			eeRPC_gi_SizeOfRunningRq -= eeRPC_gast_ReadRequest[rq].i_BufferSize;
			eeRPC_gi_NbOfBusyReadRq--;
			break;

		default:

		case -1:	/* Rq mal terminee */
			eeRPC_gast_ReadRequest[rq].i_Status = eeRPC_Cte_Error;
			eeRPC_gi_SizeOfRunningRq -= eeRPC_gast_ReadRequest[rq].i_BufferSize;
			eeRPC_gi_NbOfBusyReadRq--;
			break;

		case 1:		/* Rq en cours */
			eeRPC_gast_ReadRequest[rq].i_Status = eeRPC_Cte_Busy;
			break;
		}

		/* treat canceled Rq */
		if(!eeRPC_gast_ReadRequest[rq].pc_FileBuffer)
		{
			switch(eeRPC_gast_ReadRequest[rq].i_Status)
			{
			case eeRPC_Cte_Free:
				break;

			case eeRPC_Cte_Finish:
				eeRPC_gast_ReadRequest[rq].i_Status = eeRPC_Cte_Free;
				break;

			default:

			case eeRPC_Cte_Error:
				eeRPC_gast_ReadRequest[rq].i_Status = eeRPC_Cte_Free;
				break;

			case eeRPC_Cte_Busy:
				break;
			}
		}
	}

	eeRPC_AsyncRqFlush();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_AsyncRqFlush(void)
{
	/*~~~~~~~~~~~~~*/
	int tmp[4];
	int j;
	int oldest, time;
	/*~~~~~~~~~~~~~*/

	while
	(
		(eeRPC_gi_NbOfBusyReadRq < RPC_Cte_CLIMaxRunningRequestNumber)
	&&	eeRPC_gi_NbOfPendingReadRq
	&&	(eeRPC_gi_SizeOfRunningRq < RPC_Cte_UserSize_Srv)
	)
	{
		time = oldest = -1;
		for(j = 0; j < RPC_Cte_CLIMaxRequestNumber; j++)
		{
			if(eeRPC_gast_ReadRequest[j].i_Status != eeRPC_Cte_Waiting) continue;
			if(time < eeRPC_gast_ReadRequest[j].i_Time)
			{
				time = eeRPC_gast_ReadRequest[j].i_Time;
				oldest = j;
			}
		}

		/* no rq ? */
		if(oldest == -1)
		{
			printf("ERROR\n");
			return;
		}

		/* not this time */
		if((eeRPC_gi_SizeOfRunningRq + eeRPC_gast_ReadRequest[oldest].i_BufferSize) >= RPC_Cte_UserSize_Srv) return;

		tmp[0] = oldest;
		tmp[1] = eeRPC_gast_ReadRequest[oldest].i_FileHandler;
		*(unsigned int *) &tmp[2] = eeRPC_gast_ReadRequest[oldest].ui_Position;
		tmp[3] = eeRPC_gast_ReadRequest[oldest].i_BufferSize;
		eeRPC_gast_ReadRequest[oldest].i_Status = eeRPC_Cte_Busy;
		eeRPC_gi_NbOfBusyReadRq++;
		eeRPC_gi_NbOfPendingReadRq--;
		eeRPC_gi_SizeOfRunningRq += eeRPC_gast_ReadRequest[oldest].i_BufferSize;

		eeRPC_i_PushCommand(RPC_Cmd_FileAsyncRead, (char *) tmp, 4 * sizeof(int), NULL, 0);
		eeRPC_gi_LastBusyRq = oldest;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int si_LoadModule(char *_str_name)
{
	/* printf("[EE] Load module %s\n", _str_name); */
	while(sceSifLoadModule(_str_name, 0, NULL) < 0)
	{
		eeDbg_M_ErrX(eeDbg_Err_001F " %s", _str_name);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_PushCommand(int _i_Command, void *_pv_InData, int _i_InSize, void *_pv_OutData, int _i_OutSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	char	**ppc_CurrentCmd;
	int		*pi_CmdNb;
	char	*pc_Buff;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- check server -----------------------------------------------------------------------------------------------*/

	if(!eeRPC_gb_TransfertEnabled) return -1;

	/*$2- debug scan -------------------------------------------------------------------------------------------------*/

	eeRPC_DbgScanCommand(_i_Command, _pv_InData, _i_InSize, _pv_OutData, _i_OutSize);

	/*$2- check in data ----------------------------------------------------------------------------------------------*/

	if(_i_InSize && !_pv_InData)
	{
		eeDbg_M_ErrX(eeDbg_Err_002B " 0x%x", _i_Command);
		return -1;
	}

	/*$2- check prio -------------------------------------------------------------------------------------------------*/

	if(_i_Command & RPC_Cte_PrioHight)
	{
		/*~~~~~~~~~~~~~~~*/
		int iround, oround;
		/*~~~~~~~~~~~~~~~*/

		iround = M_RoundUpTo16(_i_InSize);
		oround = M_RoundUpTo16(_i_OutSize + 4);

		if(iround > RPC_Cte_BuffSize_Srv)
		{
			eeDbg_M_Err(eeDbg_Err_0024 "(1)");
			return -1;
		}

		if(oround > RPC_Cte_BuffSize_Srv)
		{
			eeDbg_M_ErrX(eeDbg_Err_0024 "(2) -> %d oct", oround * 16);

			return -1;
		}

		M_WaitRPC();
		
		L_memcpy(eeRPC_ac_BuffPrioHight, _pv_InData, _i_InSize);
		L_memset(eeRPC_ac_BuffPrioNormal2, 0, RPC_Cte_BuffSize_Srv);
		
		if
		(
			M_sceSifCallRpc
				(
					&eeRPC_gst_ClientData,
					_i_Command,
					0,
					(void *) eeRPC_ac_BuffPrioHight,
					_i_InSize,
					(void *) eeRPC_ac_BuffPrioNormal2,
					_i_OutSize + 4,
					NULL,
					NULL
				) < 0
		)
		{
			eeDbg_M_Err(eeDbg_Err_0022);
			return -1;
		}

		if(_pv_OutData) L_memcpy(_pv_OutData, &eeRPC_ac_BuffPrioNormal2[4], _i_OutSize);

		eeRPC_DbgScanFlush(eeRPC_ac_BuffPrioHight, eeRPC_ac_BuffPrioHight + _i_InSize);

		return * (int *) eeRPC_ac_BuffPrioNormal2;
	}
	else if(_i_Command & RPC_Cte_PrioNormal)
	{
		ppc_CurrentCmd = &eeRPC_gp_CurrentCmdPrioNormal;
		pi_CmdNb = &eeRPC_gi_CmdNbPrioNormal;
		pc_Buff = eeRPC_ac_BuffPrioNormal;
	}
	else if(_i_Command & RPC_Cte_PrioNormal2)
	{
		ppc_CurrentCmd = &eeRPC_gp_CurrentCmdPrioNormal2;
		pi_CmdNb = &eeRPC_gi_CmdNbPrioNormal2;
		pc_Buff = eeRPC_ac_BuffPrioNormal2;
	}

	/*$2- flush if needed --------------------------------------------------------------------------------------------*/

	if((int) ((*ppc_CurrentCmd) + _i_InSize + 4 + 4) >= (int) (pc_Buff + RPC_Cte_UserSize_Srv))
		eeRPC_FlushCommandBuffer(pi_CmdNb, ppc_CurrentCmd, pc_Buff);

	/*$2- push command + data ----------------------------------------------------------------------------------------*/

	*(int *) (*ppc_CurrentCmd) = _i_Command;
	(*ppc_CurrentCmd) += 4;

	*(int *) (*ppc_CurrentCmd) = _i_InSize;
	(*ppc_CurrentCmd) += 4;

	L_memcpy((*ppc_CurrentCmd), _pv_InData, _i_InSize);
	(*ppc_CurrentCmd) += _i_InSize;
	(*pi_CmdNb)++;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_DeleteCommand(int _i_Command)
{
	/*~~~~~~~~~~*/
	int size;
	int i, k;
	int *pi_Read;
	int *pi_Write;
	int *pi_Tmp;
	/*~~~~~~~~~~*/

	k = -1;
	if(_i_Command & RPC_Cte_PrioNormal)
	{
		k = 0;
		pi_Tmp = eeRPC_ac_BuffPrioNormal2;
		L_memset(eeRPC_ac_BuffPrioNormal2, 0, RPC_Cte_BuffSize_Srv * sizeof(char));

		pi_Read = (int *) ((int) eeRPC_ac_BuffPrioNormal + 4);
		pi_Write = (int *) ((int) eeRPC_ac_BuffPrioNormal + 4);

		for(i = 0; i < eeRPC_gi_CmdNbPrioNormal; i++)
		{
			if(*pi_Read == _i_Command)
			{
				/* ship cmd */
				pi_Read++;
				k += 4;

				/* skip size */
				pi_Read++;
				*pi_Tmp++ = size;
				k += 4;

				/* skip data */
				pi_Read = (int *) ((char *) pi_Read + size);
				k += size;
				continue;
			}

			/* copy cmd */
			*pi_Write++ = *pi_Read++;

			/* copy size */
			size = *pi_Read++;
			*pi_Write++ = size;

			/* copy data */
			L_memcpy(pi_Write, pi_Read, size);
			pi_Write = (int *) ((char *) pi_Write + size);
			pi_Read = (int *) ((char *) pi_Read + size);
		}
	}
	else if(_i_Command & RPC_Cte_PrioHight)
	{
		k = 0;
		pi_Tmp = eeRPC_ac_BuffPrioNormal2;
		L_memset(eeRPC_ac_BuffPrioNormal2, 0, RPC_Cte_BuffSize_Srv * sizeof(char));

		pi_Read = (int *) ((int) eeRPC_ac_BuffPrioHight + 4);
		pi_Write = (int *) ((int) eeRPC_ac_BuffPrioHight + 4);

		for(i = 0; i < eeRPC_gi_CmdNbPrioHigth; i++)
		{
			if(*pi_Read == _i_Command)
			{
				/* ship cmd */
				pi_Read++;
				k += 4;

				/* skip size */
				pi_Read++;
				*pi_Tmp++ = size;
				k += 4;

				/* skip data */
				pi_Read = (int *) ((char *) pi_Read + size);
				k += size;
				continue;
			}

			/* copy cmd */
			*pi_Write++ = *pi_Read++;

			/* copy size */
			size = *pi_Read++;
			*pi_Write++ = size;

			/* copy data */
			L_memcpy(pi_Write, pi_Read, size);
			pi_Write = (int *) ((char *) pi_Write + size);
			pi_Read = (int *) ((char *) pi_Read + size);
		}
	}

	return(k);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void eeRPC_FlushCommandBuffer(int *_pi_CmdNb, char **_ppc_CurrentCmd, char *_pc_Buff)
{
	if(*_pi_CmdNb)
	{
		eeRPC_DbgScanFlush(_pc_Buff, *_ppc_CurrentCmd);

		*(int *) _pc_Buff = *_pi_CmdNb;

		M_WaitRPC();
		L_memcpy(eeRPC_ac_BuffSending, _pc_Buff, M_RoundUpTo16((unsigned int) (*_ppc_CurrentCmd) - (unsigned int) (_pc_Buff) + 1 + 4 + 4));
		if
		(
			M_sceSifCallRpc
				(
					&eeRPC_gst_ClientData,
					RPC_Cmd_RPCFlushCommand,
					C_NoWaitFlag,
					(void *) eeRPC_ac_BuffSending,
					M_RoundUpTo16((unsigned int) (*_ppc_CurrentCmd) - (unsigned int) (_pc_Buff) + 1 + 4 + 4),
					NULL,
					0,
					NULL,
					NULL
				) < 0
		)
		{
			eeDbg_M_Err(eeDbg_Err_0022);
		}
	}
	(*_ppc_CurrentCmd) = _pc_Buff + 4;
	*_pi_CmdNb = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_FlushCommandBeforeEngine(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_FlushCommandAfterEngine(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern void ps2SND_FlushModif(void);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	eeRPC_RefreshAsyncStatus();
	ps2SND_FlushModif();
	eeRPC_FlushCommandBuffer(&eeRPC_gi_CmdNbPrioNormal, &eeRPC_gp_CurrentCmdPrioNormal, eeRPC_ac_BuffPrioNormal);
	eeRPC_DbgScanUpdate();
}

/*$4
 ***********************************************************************************************************************
    File Streaming
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_CreateFileStreaming(unsigned int _ui_FilePosition, unsigned int _ui_FileSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RPC_tdst_VArg_FileStreamCreate	stVArg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ui_FilePosition == -1) return -1;
	if(eeRPC_gst_Streaming.b_Valid) eeRPC_DestroyFileStreaming(FALSE);
	if(eeRPC_gst_Streaming.i_FileHandler < 0)
	{
		eeRPC_gst_Streaming.i_FileHandler = eeRPC_gi_BF;
	}

	if(eeRPC_gst_Streaming.i_FileHandler < 0) return -1;
	eeRPC_gst_Streaming.ui_FilePosition = _ui_FilePosition;
	eeRPC_gst_Streaming.ui_FileSize = _ui_FileSize;

	L_memset(&stVArg, 0, sizeof(RPC_tdst_VArg_FileStreamCreate));
	stVArg.i_FileHandler = eeRPC_gst_Streaming.i_FileHandler;
	stVArg.ui_FilePos = eeRPC_gst_Streaming.ui_FilePosition;
	stVArg.ui_FileSize = eeRPC_gst_Streaming.ui_FileSize;

	if(MTX_b_CurrentFileIsMTX())
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		extern int	ps2SND_i_GetiVolumeFromfVolume(float);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		stVArg.i_FileIsMTX = 1;
		stVArg.ui_VideoPosition = MTX_ui_GetVideoFilePosition();
		stVArg.ui_VideoSize = MTX_ui_GetVideoFileSize();
		stVArg.ui_VideoBufferSize = MTX_ui_GetVideoBufferSize();
		stVArg.ui_SoundPosition = 0;
		stVArg.ui_SoundSize = MTX_ui_GetSoundDataSize();
		stVArg.ui_SoundBufferSize = MTX_ui_GetSoundBufferSize();
		stVArg.ui_Frequency = MTX_ui_GetSoundFrequency();
		stVArg.ui_SoundChannel = MTX_ui_GetSoundChannelNb();
		stVArg.i_Volume = ps2SND_i_GetiVolumeFromfVolume(MTX_f_GetSoundVolume());

		eeRPC_gst_Streaming.ui_FilePosition = MTX_ui_GetStreamPosition();
		eeRPC_gst_Streaming.ui_FileSize = MTX_ui_GetStreamSize();
		stVArg.ui_FilePos = eeRPC_gst_Streaming.ui_FilePosition;
		stVArg.ui_FileSize = eeRPC_gst_Streaming.ui_FileSize;
	}
	else
	{
		stVArg.i_FileIsMTX = 0;
	}

	eeRPC_i_PushCommand(RPC_Cmd_FileStreamCreate, &stVArg, sizeof(RPC_tdst_VArg_FileStreamCreate), NULL, 0);
	eeRPC_gst_Streaming.b_Valid = TRUE;

	return eeRPC_gst_Streaming.ui_FilePosition;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_DestroyFileStreaming(BOOL _b_CloseFile)
{
	/*~~~*/
	int rq;
	/*~~~*/

	if(!eeRPC_gst_Streaming.b_Valid) return;

	if(_b_CloseFile && (eeRPC_gst_Streaming.i_FileHandler >= 0))
	{
		/*
		 * eeRPC_i_PushCommand(RPC_Cmd_FileClose, &eeRPC_gst_Streaming.i_FileHandler,
		 * sizeof(int), NULL, 0);
		 */
		eeRPC_gst_Streaming.i_FileHandler = -1;
	}

	eeRPC_i_PushCommand(RPC_Cmd_FileStreamDestroy, NULL, 0, NULL, 0);
	for(rq = 0; rq < RPC_Cte_CLIMaxRequestNumber; rq++)
	{
		if(eeRPC_gast_ReadRequest[rq].ui_Position < eeRPC_gst_Streaming.ui_FilePosition) continue;
		if
		(
			eeRPC_gast_ReadRequest[rq].ui_Position >
				(eeRPC_gst_Streaming.ui_FilePosition + eeRPC_gst_Streaming.ui_FileSize)
		) continue;
		eeRPC_i_CancelAsyncRq(rq);
	}

	eeRPC_gst_Streaming.ui_FilePosition = 0;
	eeRPC_gst_Streaming.ui_FileSize = 0;
	eeRPC_gst_Streaming.b_Valid = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_ReinitFileStreaming(unsigned int _ui_ReinitFilePosition)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	RPC_tdst_VArg_FileStreamReinit	stVArg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!eeRPC_gst_Streaming.b_Valid) return;
	if(!MTX_b_CurrentFileIsMTX()) return;

	stVArg.ui_StartPos = eeRPC_gst_Streaming.ui_FilePosition;
	eeRPC_i_PushCommand(RPC_Cmd_FileStreamReinit, &stVArg, sizeof(RPC_tdst_VArg_FileStreamReinit), NULL, 0);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$2------------------------------------------------------------------------------------------------------------------*/

#define eeRPC_Cte_ExecAsyncMaxCommand	64

/*$2------------------------------------------------------------------------------------------------------------------*/

typedef struct	eeRPC_tdst_ExecAsyncCallbackRef_
{
	int				i_Cmd;
	int				b_IsRunning;
	char			*pc_OutBuffer;
	unsigned int	ui_OutBufferSize;
	sceSifEndFunc	pfv_Callback;
	void			*pv_Arg;
} eeRPC_tdst_ExecAsyncCallbackRef;

/*$2------------------------------------------------------------------------------------------------------------------*/

eeRPC_tdst_ExecAsyncCallbackRef eeRPC_gax_ExecAsyncCallbackRef[eeRPC_Cte_ExecAsyncMaxCommand];

/*$2------------------------------------------------------------------------------------------------------------------*/

volatile int	TheEnd;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

static void eeRPC_InitExecAsync(void)
{
	/*~~*/
	int i;
	/*~~*/

	TheEnd = 1;
	L_memset
	(
		eeRPC_gax_ExecAsyncCallbackRef,
		0,
		eeRPC_Cte_ExecAsyncMaxCommand * sizeof(eeRPC_tdst_ExecAsyncCallbackRef)
	);
	i = 0;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_RPCFlushCommand;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_AwakeSupervisor;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_Check;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndInit;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndBigOpen;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndClose;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndSetMasterVol;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndSetMasterVol_spe;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_BeginSpeedMode;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndStartScheduler;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndStopScheduler;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndDebugInfos;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndGetStatus;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndCreateBuffer;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndDuplicateBuffer;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndReloadBuffer;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndPlay;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndPlayLoop;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndPlayFxA;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndPlayLoopFxA;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndPlayFxB;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndPlayLoopFxB;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndStop;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndPause;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndSetVol;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndSetFreq;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndSetFx;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndStopStream;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndPauseStream;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndSetVolStream;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndSetFreqStream;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndCreateBufferForceStream;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndStreamReinitAndPlayLong;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndStopAllStream;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndPriority;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndStreamPrefetch;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndStreamFlush;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_SndStreamPrefetchArray;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_CDVInitModule;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_CDVCloseModule;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_CDVOpenFile;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_CDVReadFile;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_CDVCloseFile;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_CDVSetBufferSize;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_CDVGetBufferSize;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_CDVAsyncRead;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_CDVAsyncReadStatus;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_FileOpen;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_FileClose;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_FileAsyncRead;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_GetFileAsyncStatus;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_FileStreamCreate;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_FileStreamDestroy;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_FileStreamReinit;
	eeRPC_gax_ExecAsyncCallbackRef[i++].i_Cmd = RPC_Cmd_FileExecAsyncRead;
}

volatile float	eeRPC_LastAsyncDate;
volatile float	eeRPC_LastAsyncReadDuration;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_ExecAsyncCallback(void *pv)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	eeRPC_tdst_ExecAsyncCallbackRef *pRef;
	float							ff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pRef = (eeRPC_tdst_ExecAsyncCallbackRef *) pv;

	pRef->b_IsRunning = 0;

	if(pRef->pc_OutBuffer && pRef->ui_OutBufferSize)
		L_memcpy(pRef->pc_OutBuffer, eeRPC_ac_BuffPrioAsync + 4, pRef->ui_OutBufferSize);

	if(pRef->pfv_Callback) pRef->pfv_Callback(pRef->pv_Arg);

	ff = ps2TIM_f_RealTimeClock();
	eeRPC_LastAsyncDate = ff;
	eeRPC_LastAsyncReadDuration = ff - eeRPC_LastAsyncReadDuration;

	TheEnd = 1;
	if(*(int *) eeRPC_ac_BuffPrioAsync < 0) TheEnd = -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_ExecAsyncCommand
(
	unsigned int	_ui_Cmd,
	char			*pc_InBuffer,
	unsigned int	_ui_InBufferSize,
	char			*pc_OutBuffer,
	unsigned int	_ui_OutBufferSize,
	void			*_pfv_callback,
	void			*_pv_CallbackArg
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int								i;
	int								iround, oround;
	eeRPC_tdst_ExecAsyncCallbackRef *pRef;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	eeRPC_LastAsyncDate = ps2TIM_f_RealTimeClock() - eeRPC_LastAsyncDate;

	/* printf("Idle %f read %f \n", eeRPC_LastAsyncDate, eeRPC_LastAsyncReadDuration); */
	for(i = 0, pRef = eeRPC_gax_ExecAsyncCallbackRef; i < eeRPC_Cte_ExecAsyncMaxCommand; i++, pRef++)
	{
		if(pRef->i_Cmd == _ui_Cmd)
		{
			pRef->pfv_Callback = _pfv_callback;
			pRef->pv_Arg = (sceSifEndFunc) _pv_CallbackArg;
			pRef->pc_OutBuffer = pc_OutBuffer;
			pRef->ui_OutBufferSize = _ui_OutBufferSize;
			break;
		}
	}

	if(i >= eeRPC_Cte_ExecAsyncMaxCommand) return -1;

	iround = M_RoundUpTo16(_ui_InBufferSize);
	oround = M_RoundUpTo16(_ui_OutBufferSize + 4);
	if(iround > RPC_Cte_BuffSize_Srv)
	{
		eeDbg_M_Err(eeDbg_Err_0024 "(11)");
		return -1;
	}

	if(oround > RPC_Cte_BuffSize_Srv)
	{
		eeDbg_M_ErrX(eeDbg_Err_0024 "(22) -> %d oct", oround * 16);

		return -1;
	}

	if(pc_InBuffer && _ui_InBufferSize) L_memcpy(eeRPC_ac_BuffPrioAsync, pc_InBuffer, _ui_InBufferSize);

	M_WaitRPC();

	TheEnd = 0;
	if
	(
		M_sceSifCallRpc
			(
				&eeRPC_gst_ClientData,
				_ui_Cmd,
				C_NoWaitFlag,
				(void *) eeRPC_ac_BuffPrioAsync,
				iround,
				(void *) eeRPC_ac_BuffPrioAsync,
				oround,
				eeRPC_ExecAsyncCallback,
				(void *) pRef
			) < 0
	)
	{
		eeDbg_M_Err(eeDbg_Err_0022);
		return -1;
	}

	eeRPC_LastAsyncReadDuration = ps2TIM_f_RealTimeClock();

	pRef->b_IsRunning = 1;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_LastExecAsyncReadIsFinished(void)
{
	if(TheEnd < 0)
	{
	}

	return(TheEnd);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int eeRPC_i_ExecAsyncRead(int _i_File, unsigned int _ui_Pos, char *_pc_Buff, int _i_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	RPC_tdst_FileExecAsyncRead	tmp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Size > iopCLI_Cte_PreLoadSize)
	{
		return -1;
	}

	/*
	 * add Rq £
	 * flush now
	 */
	tmp.i_FileId = _i_File;
	tmp.ui_Position = _ui_Pos;
	tmp.ui_Size = _i_Size;

	i = eeRPC_i_ExecAsyncCommand
		(
			RPC_Cmd_FileExecAsyncRead,
			(char *) &tmp,
			sizeof(RPC_tdst_FileExecAsyncRead),
			_pc_Buff,
			_i_Size,
			NULL,
			0
		);

	return i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void eeRPC_AsyncReadReset(void)
{
	eeRPC_i_PushCommand(RPC_Cmd_FileAsyncReset, NULL, 0, NULL, 0);
	L_memset(eeRPC_gast_ReadRequest, 0, RPC_Cte_CLIMaxRequestNumber * sizeof(eeRPC_tdst_ReadRequest));
	eeRPC_gi_NbOfBusyReadRq = 0;
	eeRPC_gi_NbOfPendingReadRq = 0;
	eeRPC_gi_SizeOfRunningRq = 0;
	eeRPC_gi_LastBusyRq = -1;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
