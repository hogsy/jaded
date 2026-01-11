/*$T iopSND_Voice.h GC 1.138 06/15/04 11:54:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP

/*$T iopSND_Voice.h GC 1.138 06/15/04 11:54:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __iopSND_VOICE_H__
#define __iopSND_VOICE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define iopSND_M_CheckStreamBuffIdxOrReturn(_id, _ret) \
	do \
	{ \
		if \
		( \
			(_id <= 0) \
		||	((_id & 0x0000FFFF) == 0x0000FFFF) \
		||	((_id & 0x0000FFFF) == 0x00000000) \
		||	((_id & 0x0000FFFF) > SND_Cte_MaxSimultaneousStream) \
		||	((_id & 0xFFFF0000) == 0xFFFF0000) \
		||	(((_id & 0xFFFF0000) >> 16) > SND_Cte_MaxSimultaneousStream) \
		) \
		{ \
			iopDbg_M_ErrX(iopDbg_Err_0054 " "__FILE__ "(%d) : %d", __LINE__, _id); \
			return _ret; \
		} \
	} while(0);

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	iopSND_tdst_SoftBuffer_
{
	char *volatile pc_SpuMemAddr;
	volatile unsigned int	ui_Flag;
	unsigned int			ui_DuplicateNb;
	struct iopSND_tdst_SoftBuffer_ * volatile p_DuplicateSrc;
	char *volatile pc_DbleBuff;
	volatile unsigned int					ui_DbleBuffSize;
	volatile unsigned int					ui_DataPos;
	volatile unsigned int					ui_DataSize;
	volatile unsigned int					ui_FileSeek;
	volatile unsigned int					ui_LoopBegin;
	volatile unsigned int					ui_LoopEnd;
	volatile struct iopSND_tdst_HardBuffer_ *pst_HardBuffer;
	volatile unsigned short					us_LeftVolume;
	volatile unsigned short					us_RightVolume;
	volatile struct iopSND_tdst_HardBuffer_ *pst_HardBufferLeft;
	volatile int							i_LastDma;
	volatile unsigned short					us_Pitch;
	volatile short							s_StrFileIdx;
	volatile int							i_LoopCount;
} iopSND_tdst_SoftBuffer;

typedef struct	iopSND_tdst_ReinitAndPlayArg_
{
	unsigned int	ui_DataPos;
	unsigned int	ui_DataSize;
	int				b_Stereo;
	int				i_LoopNb;
	unsigned int	ui_PlayFlags;
	int				i_Freq;
	int				i_LVol;
	int				i_RVol;
	unsigned int	ui_StartPos;
	unsigned int	ui_StopPos;
	unsigned int	ui_LoopBegin;
	unsigned int	ui_LoopEnd;
	int				b_Chain;
	unsigned int	ui_ChainDelaySize;
} iopSND_tdst_ReinitAndPlayArg;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern int										iopSND_gi_SoftLock;
extern volatile iopSND_tdst_SoftBuffer			iopSND_sa_SoftBuffer[];
extern volatile iopSND_tdst_ReinitAndPlayArg	iopSND_sa_StreamBuffer[];
extern volatile int								iopSND_vi_RPCThreadIsWaiting;
extern volatile int                             iopSND_gi_OneShotPending;
/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

#define __SoftLock__	do \
	{ \
		CLI_WaitSema(iopSND_gi_SoftLock); \
	} while(0);

#define __SoftUnlock__	do \
	{ \
		CLI_SignalSema(iopSND_gi_SoftLock); \
	} while(0);

#define M_Freq2Pitch(x) ((x * 4096) / 48000)
#define M_Pitch2Freq(x) ((x * 48000) / 4096)

/*$2- module ---------------------------------------------------------------------------------------------------------*/

int iopSND_i_InitVoiceModule(void);
int iopSND_i_CloseVoiceModule(void);

/*$2- SB -------------------------------------------------------------------------------------------------------------*/

int iopSND_i_CreateVoiceBuffer(struct RPC_tdst_SndCreateBuffer_ *);
int iopSND_i_DuplicateVoiceBuffer(void *);
int iopSND_i_PlayVoice(int _i_Voice);
int iopSND_i_PlayLoopVoice(struct RPC_tdst_Play_ *);
int iopSND_i_PlayNoLoopVoice(struct RPC_tdst_Play_ *);
int iopSND_i_StopVoice(int _i_Voice);
int iopSND_i_SetVoiceVol(int _i_Snd, int _i_Left, int _i_Right);
int iopSND_i_GetVoiceStatus(int *, int *);
int iopSND_i_SetVoiceFreq(int _i_Snd, int _i_Freq);
int iopSND_i_DirectStopVoice(int _i_Snd);
int iopSND_i_ReloadBuffer(struct RPC_tdst_SndCreateBuffer_ *pVArgs);
int iopSND_i_ReloadBuffer_r(int i_voice);

/*$2------------------------------------------------------------------------------------------------------------------*/

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __iopSND_VOICE_H__ */
#endif /* PSX2_IOP */
