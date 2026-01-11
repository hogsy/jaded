/*$T RPC_Manager.h GC 1.138 05/18/04 11:33:45 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __eeRPC_Manager_h__
#define __eeRPC_Manager_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    constant
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    configuration
 -----------------------------------------------------------------------------------------------------------------------
 */

#define RPC_Cte_SrvId						0x00012345
#define RPC_Cte_UserSize_Srv				(3 * 8 * 2048)				/* keep modulo CD sector size : 2048 */
#define RPC_Cte_BuffSize_Srv				(RPC_Cte_UserSize_Srv + 64) /* keep modulo 16 */
#define RPC_Cte_MaxBufferSize				(1 * 1024 * 1024)
#define RPC_Cte_MinBufferSize				(16)
#define RPC_Cte_CLIMaxRequestNumber			3
#define RPC_Cte_CLIMaxRunningRequestNumber	3

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    mask
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 * #define RPC_Msk_SndCmd 0x000000ff £
 * #define RPC_Msk_SndBuffId 0x00003f00
 */
#define RPC_Msk_RqId		0x000000FF
#define RPC_Msk_ModuleId	0x0000FF00
#define RPC_Msk_Priority	0xF0000000
#define RPC_Msk_NotUsed		0x0FFF0000

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    modules ID
 -----------------------------------------------------------------------------------------------------------------------
 */

#define RPC_Cte_IDDbg	0x00001000
#define RPC_Cte_IDCDV	0x00002000
#define RPC_Cte_IDFile	0x00003000
#define RPC_Cte_IDSnd	0x00004000
#define RPC_Cte_IDRPC	0x00005000
#define RPC_Cte_IDNb	5

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    priority
 -----------------------------------------------------------------------------------------------------------------------
 */

#define RPC_Cte_PrioHight	0x80000000
#define RPC_Cte_PrioNormal	0x40000000
#define RPC_Cte_PrioNormal2 0x20000000
#define RPC_Cte_PrioNb		3

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    command
 -----------------------------------------------------------------------------------------------------------------------
 */

/*$1- RPC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define RPC_Cmd_RPCFlushCommand (RPC_Cte_IDRPC | RPC_Cte_PrioHight | 0x00)

/*$1- debug ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define RPC_Cmd_AwakeSupervisor (RPC_Cte_IDDbg | RPC_Cte_PrioHight | 0x00)
#define RPC_Cmd_Check			(RPC_Cte_IDDbg | RPC_Cte_PrioHight | 0x01)

/*$1- sound cmd ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define RPC_Cmd_SndInit						(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x00)
#define RPC_Cmd_SndBigOpen					(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x01)
#define RPC_Cmd_SndClose					(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x02)
#define RPC_Cmd_SndSetMasterVol				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x03)
#define RPC_Cmd_SndSetMasterVol_spe			(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x04)
#define RPC_Cmd_BeginSpeedMode				(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x05)
#define RPC_Cmd_SndStartScheduler			(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x07)
#define RPC_Cmd_SndStopScheduler			(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x08)
#define RPC_Cmd_SndDebugInfos				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x09)
#define RPC_Cmd_SndGetStatus				(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x0A)
#define RPC_Cmd_SndCreateBuffer				(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x0B)
#define RPC_Cmd_SndDuplicateBuffer			(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x0C)
#define RPC_Cmd_SndReloadBuffer				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x0D)
#define RPC_Cmd_SndPlay						(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x0E)
#define RPC_Cmd_SndPlayLoop					(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x11)
#define RPC_Cmd_SndStop						(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x12)
#define RPC_Cmd_SndPause					(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x13)
#define RPC_Cmd_SndSetVol					(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x14)
#define RPC_Cmd_SndSetFreq					(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x15)
#define RPC_Cmd_SndSetFx					(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x18)
#define RPC_Cmd_SndStopStream				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x19)
#define RPC_Cmd_SndPauseStream				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x1A)
#define RPC_Cmd_SndSetVolStream				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x1B)
#define RPC_Cmd_SndSetFreqStream			(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x1C)
#define RPC_Cmd_SndCreateBufferForceStream	(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x1D)
#define RPC_Cmd_SndStreamReinitAndPlayLong	(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x1E)
#define RPC_Cmd_SndStopAllStream			(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x1F)
#define RPC_Cmd_SndPriority					(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x20)
#define RPC_Cmd_SndStreamPrefetch			(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x21)
#define RPC_Cmd_SndStreamFlush				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x22)
#define RPC_Cmd_SndStreamPrefetchArray		(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x23)
#define RPC_Cmd_SndUnlockThread				(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x24)
#define RPC_Cmd_SndStreamSetPos				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x26)
#define RPC_Cmd_SndStreamGetPos				(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x27)
#define RPC_Cmd_SndRenderMode				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x28)
#define RPC_Cmd_SndPlayFxA					(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x29)
#define RPC_Cmd_SndPlayLoopFxA				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x2A)
#define RPC_Cmd_SndPlayFxB					(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x2B)
#define RPC_Cmd_SndPlayLoopFxB				(RPC_Cte_IDSnd | RPC_Cte_PrioNormal | 0x2C)
#define RPC_Cmd_SndStreamPrefetchStatus		(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x2D)
#define RPC_Cmd_SndStreamChain				(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x2E)
#define RPC_Cmd_SndStreamLoopCountGet		(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x2F)
#define RPC_Cmd_SndStreamChainDelayGet		(RPC_Cte_IDSnd | RPC_Cte_PrioHight | 0x30)

/*$1- CDV cmd ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define RPC_Cmd_CDVInitModule		(RPC_Cte_IDCDV | RPC_Cte_PrioHight | 0x00)
#define RPC_Cmd_CDVCloseModule		(RPC_Cte_IDCDV | RPC_Cte_PrioHight | 0x01)
#define RPC_Cmd_CDVOpenFile			(RPC_Cte_IDCDV | RPC_Cte_PrioHight | 0x02)
#define RPC_Cmd_CDVReadFile			(RPC_Cte_IDCDV | RPC_Cte_PrioHight | 0x03)
#define RPC_Cmd_CDVCloseFile		(RPC_Cte_IDCDV | RPC_Cte_PrioHight | 0x04)
#define RPC_Cmd_CDVSetBufferSize	(RPC_Cte_IDCDV | RPC_Cte_PrioHight | 0x05)
#define RPC_Cmd_CDVGetBufferSize	(RPC_Cte_IDCDV | RPC_Cte_PrioHight | 0x06)
#define RPC_Cmd_CDVAsyncRead		(RPC_Cte_IDCDV | RPC_Cte_PrioHight | 0x07)
#define RPC_Cmd_CDVAsyncReadStatus	(RPC_Cte_IDCDV | RPC_Cte_PrioHight | 0x08)

/*$1- file server ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define RPC_Cmd_FileOpen			(RPC_Cte_IDFile | RPC_Cte_PrioHight | 0x00)
#define RPC_Cmd_FileClose			(RPC_Cte_IDFile | RPC_Cte_PrioNormal | 0x01)
#define RPC_Cmd_FileAsyncRead		(RPC_Cte_IDFile | RPC_Cte_PrioHight | 0x10)
#define RPC_Cmd_GetFileAsyncStatus	(RPC_Cte_IDFile | RPC_Cte_PrioHight | 0x11)
#define RPC_Cmd_FileExecAsyncRead	(RPC_Cte_IDFile | RPC_Cte_PrioHight | 0x12)
#define RPC_Cmd_FileAsyncReset		(RPC_Cte_IDFile | RPC_Cte_PrioHight | 0x13)
#define RPC_Cmd_FileStreamCreate	(RPC_Cte_IDFile | RPC_Cte_PrioHight | 0x30)
#define RPC_Cmd_FileStreamDestroy	(RPC_Cte_IDFile | RPC_Cte_PrioHight | 0x31)
#define RPC_Cmd_FileStreamReinit	(RPC_Cte_IDFile | RPC_Cte_PrioHight | 0x32)

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

/*$1- debug ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	RPC_tdst_VArg_Check
{
	unsigned int	ui_StackSizeOfSupervisorThread;
	unsigned int	ui_StackSizeOfRPCThread;
	unsigned int	ui_StackSizeOfSoundThread;
	unsigned int	ui_SysMemoryFreeMaxSize;
	unsigned int	ui_MemoryAlloc;
	unsigned int	ui_MemoryAllocMax;
	int				i_MemoryFreeSpaceMin;
	int				i_LastError;
	int				i_ErrorNumber;
	int				i_LastMapError;
	int				i_MapErrorNumber;
} RPC_tdst_VArg_Check;

/*$1- CDV ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	RPC_tdst_VArg_CDVSetSeek_
{
	unsigned int	ui_Seek;
} RPC_tdst_VArg_CDVSetSeek;

/*$1- FileStream ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	RPC_tdst_VArg_FileStreamCreate_
{
	int				i_FileHandler;
	unsigned int	ui_FilePos;
	unsigned int	ui_FileSize;

	int				i_FileIsMTX;

	unsigned int	ui_VideoPosition;
	unsigned int	ui_VideoSize;
	unsigned int	ui_VideoBufferSize;

	unsigned int	ui_SoundPosition;
	unsigned int	ui_SoundSize;
	unsigned int	ui_SoundBufferSize;
	unsigned int	ui_SoundChannel;
	unsigned int	ui_Frequency;
	int				i_Volume;
} RPC_tdst_VArg_FileStreamCreate;

typedef struct	RPC_tdst_VArg_FileStreamReinit_
{
	unsigned int	ui_StartPos;
} RPC_tdst_VArg_FileStreamReinit;

/*$1- sound ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	RPC_tdst_VArg_SndStreamPrefetch_
{
	unsigned int	ui_Position;
	unsigned int	ui_Size;
} RPC_tdst_VArg_SndStreamPrefetch;

typedef struct	RPC_tdst_VArg_SndStreamFlush_
{
	unsigned int	ui_Position;
} RPC_tdst_VArg_SndStreamFlush;

typedef struct	RPC_tdst_FileExecAsyncRead_
{
	int				i_FileId;
	unsigned int	ui_Position;
	unsigned int	ui_Size;
} RPC_tdst_FileExecAsyncRead;

typedef struct	iopSND_ReinitAndPlayStreamLong_VArgs_
{
	int				i_ID;
	int				i_Flag;
	int				i_LoopNb;
	unsigned int	ui_Frequency;
	unsigned int	ui_LVol;
	unsigned int	ui_RVol;
	/**/
	unsigned int	ui_RPosition;
	unsigned int	ui_RSize;
	unsigned int	ui_LPosition;
	unsigned int	ui_LSize;
	/**/
	unsigned int	ui_RStartPos;
	unsigned int	ui_RStopPos;
	unsigned int	ui_LStartPos;
	unsigned int	ui_LStopPos;
	/**/
	unsigned int	ui_RLoopStart;
	unsigned int	ui_RLoopStop;
	unsigned int	ui_LLoopStart;
	unsigned int	ui_LLoopStop;
	/**/
	char			c_DryMixL;
	char			c_DryMixR;
	char			c_WetMixL;
	char			c_WetMixR;
} iopSND_ReinitAndPlayStreamLong_VArgs;

typedef struct	RPC_tdst_SndStreamSetPos_
{
	unsigned int	ui_PlayRpos;
	unsigned int	ui_PlayLpos;
	unsigned int	ui_WriteRpos;
	unsigned int	ui_WriteLpos;
} RPC_tdst_SndStreamSetPos;

typedef struct	RPC_tdst_FxSettings_
{
	unsigned int	ul_Modif;
	int				core;
	int				mode;
	short			depth_L;
	short			depth_R;
	int				delay;
	int				feedback;
} RPC_tdst_FxSettings;

typedef struct	RPC_tdst_Play_
{
	int		i_ID;
	int		i_Flag;
	char	c_DryMixL;
	char	c_DryMixR;
	char	c_WetMixL;
	char	c_WetMixR;
} RPC_tdst_Play;

typedef struct	RPC_tdst_SndStreamChain_
{
	int				i_ID;
	int				i_NewFlag;
	int				i_LoopNb;
	unsigned int	ui_ExitPosition;
	unsigned int	ui_EnterPosition;
	unsigned int	ui_NewSize;
	unsigned int	ui_NewPosition;
	unsigned int	ui_NewLoopBegin;
	unsigned int	ui_NewLoopEnd;
} RPC_tdst_SndStreamChain;

typedef struct	RPC_tdst_SndCreateBuffer_
{
	unsigned int	ui_ID;
	unsigned int	ui_Frequency;
	char			*pc_RSpuBuffer;
	char			*pc_RIopBuffer;
	unsigned int	ui_RPosition;
	unsigned int	ui_RSize;
} RPC_tdst_SndCreateBuffer;


/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*$4
 ***********************************************************************************************************************
    ee/iop dependant part
 ***********************************************************************************************************************
 */

#ifndef PSX2_IOP
#define _include_eeRPC_Manager_h_
#include "IOP/ee/eeRPC_Manager.h"
#undef _include_eeRPC_Manager_h_
#endif

/*$4
 ***********************************************************************************************************************
    end of ee/iop dependant part
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif
