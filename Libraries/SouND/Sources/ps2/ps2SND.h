/*$T ps2SND.h GC 1.138 06/30/04 09:10:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_TARGET
#ifndef __ps2SND_h__
#define __ps2SND_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define ps2SND_Cte_MaxVol		0x3FFF
#define ps2SND_Cte_InvMaxVol	6.10388817E-5f
#define eeSND_M_CheckSoftBuffIdxOrReturn(_id, _ret) \
	{ \
		if \
		( \
			(_id <= 0) \
		||	((_id & 0x0000FFFF) == 0x0000FFFF) \
		||	((_id & 0xFFFF0000) == 0xFFFF0000) \
		||	((_id & 0x0000FFFF) >= SND_Cte_MaxSoftBufferNb) \
		||	(((_id & 0xFFFF0000) >> 16) >= SND_Cte_MaxSoftBufferNb) \
		) return _ret; \
	}

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define ps2SND_Cte_ModifyVol		0x00000001
#define ps2SND_Cte_ModifyFreq		0x00000002

#define ps2SND_Cte_FxModifyMode		0x00000001
#define ps2SND_Cte_FxModifyWetVol	0x00000002
#define ps2SND_Cte_FxModifyDelay	0x00000004
#define ps2SND_Cte_FxModifyFeedback 0x00000008

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define ps2SND_Cte_MonoIdMask		0x00010000
#define ps2SND_Cte_StereoIdMask		0x00020000

#define ps2SND_M_IdIsStereo(ptr)	((int) (ptr) & ps2SND_Cte_StereoIdMask)
#define ps2SND_M_GetIdxFromSB(ptr)	((int) (ptr) & 0xFFFF)

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	ps2SND_tdst_Snd_
{
	unsigned int			ui_Flags;
	unsigned int			ui_CurrentStatus;
	int						i_LastCmd;
	int						i_LastArg;

	int						i_Pan;
	float					f_Volume;
	float					f_LeftCoeff;
	float					f_RightCoeff;
	short					s_LeftVol;
	short					s_RightVol;
	int						i_Frequency;

	char					*pc_SpuBuffer;
	char					*pc_IopBuffer;
	unsigned int			ui_DataPosition;
	unsigned int			ui_DataSize;

	struct ps2SND_tdst_Snd_ *pst_DuplicateSrc;
	unsigned int			ui_UserNb;

	unsigned int			ui_ModifyFlags;
	float                   f_PlayTimeOut;
} ps2SND_tdst_Snd;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern ps2SND_tdst_Snd	ps2SND_sax_Snd[SND_Cte_MaxSoftBufferNb];
extern int				ps2SND_gi_StreamPlayOnce;
extern ps2SND_tdst_Snd	ps2SND_sx_MasterSnd;


/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*$2- prototypes for master functions --------------------------------------------------------------------------------*/

int		ps2SND_l_Init(struct SND_tdst_TargetSpecificData_ *);
void	ps2SND_Close(struct SND_tdst_TargetSpecificData_ *);
void	ps2SND_SetGlobalVol(LONG);
LONG	ps2SND_l_GetGlobalVol(void);
void	eeSND_StartScheduler(int);
void	eeSND_StopScheduler(int);
void	ps2SND_RefreshStatus(void);
BOOL	ps2SND_b_IsLoading(void);
int		ps2SND_i_ChangeRenderMode(int _i_Mode);

/*$2- prototypes for stream ------------------------------------------------------------------------------------------*/

void	ps2SND_CreateTrackSB(struct SND_tdst_SoundBuffer_ **, struct SND_tdst_SoundBuffer_ **);
void	ps2SND_ReleaseTrackSB(struct SND_tdst_SoundBuffer_ *, struct SND_tdst_SoundBuffer_ *);
void	ps2SND_StreamReinitAndPlay
		(
			SND_tdst_SoundBuffer	*_pSB,
			int						_i_Flag,
			int						_i_LoopNb,
			unsigned int			_ui_Size,
			unsigned int			_ui_Position,
			unsigned int			_ui_LoopBegin,
			unsigned int			_ui_LoopEnd,
			int						_i_StartPos,
			int						_i_StopPos,
			int						_i_Freq,
			int						_i_Pan,
			int						_i_Vol,
			int						_i_FxL,
			int						_i_FxR
		);
void	ps2SND_StreamChain
		(
			SND_tdst_SoundBuffer	*pSB,
			int						iNewFlag,
			int						iLoopNb,
			unsigned int			uiExitPosition,
			unsigned int			uiEnterPosition,
			unsigned int			uiNewSize,
			unsigned int			uiNewPosition,
			unsigned int			uiLoopBegin,
			unsigned int			uiLoopEnd
		);
void	ps2SND_StreamPrefetch(unsigned int, unsigned int);
void	ps2SND_StreamPrefetchArray(unsigned int *, unsigned int *);
void	ps2SND_StreamGetPrefetchStatus(unsigned int *auiPos);
void	ps2SND_StreamFlush(unsigned int _ui_Pos);
void	ps2SND_StreamLoopCountGet(SND_tdst_SoundBuffer *pSB, int *piLoop);
void	ps2SND_StreamChainDelayGet(SND_tdst_SoundBuffer *pSB, float *pfDelay);
void	ps2SND_ShutDownStreams(void);

/*$2- prototypes for sound buffer functions --------------------------------------------------------------------------*/

struct SND_tdst_SoundBuffer_	*ps2SND_pst_SB_Create
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_WaveData_ *,
									unsigned int
								);
struct SND_tdst_SoundBuffer_	*ps2SND_SB_Reload
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_SoundBuffer_ *,
									struct SND_tdst_WaveData_ *,
									unsigned int
								);
int								ps2SND_SB_Play(struct SND_tdst_SoundBuffer_ *, int, int, int);
void							ps2SND_SB_Stop(struct SND_tdst_SoundBuffer_ *);
void							ps2SND_SB_Pause(struct SND_tdst_SoundBuffer_ *);
void							ps2SND_SB_Release(struct SND_tdst_SoundBuffer_ *);
void							ps2SND_SB_Duplicate
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_SoundBuffer_ *,
									struct SND_tdst_SoundBuffer_ **
								);
void							ps2SND_SB_SetCurrPos(struct SND_tdst_SoundBuffer_ *, int);
void							ps2SND_SB_SetVolume(struct SND_tdst_SoundBuffer_ *, int);
void							ps2SND_SB_SetFrequency(struct SND_tdst_SoundBuffer_ *, int);
void							ps2SND_SB_SetPan(struct SND_tdst_SoundBuffer_ *, int, int);
int								ps2SND_SB_GetStatus(struct SND_tdst_SoundBuffer_ *, int *);
void							ps2SND_SB_GetFrequency(struct SND_tdst_SoundBuffer_ *, int *);
void							ps2SND_SB_GetPan(struct SND_tdst_SoundBuffer_ *, int *, int *);
void							ps2SND_SB_GetCurrPos(struct SND_tdst_SoundBuffer_ *, int *, int *);
void							ps2SND_FlushModif(void);

/*$2- prototypes for FX ----------------------------------------------------------------------------------------------*/

int		ps2SND_i_FxInit(void);
void	ps2SND_FxClose(void);
int		ps2SND_i_FxSetMode(int core, int);
int		ps2SND_i_FxSetDelay(int core, int);
int		ps2SND_i_FxSetFeedback(int core, int);
void	ps2SND_i_FxSetWetVolume(int core, int);
void	ps2SND_i_FxSetWetPan(int core, int);
int		ps2SND_i_FxEnable(int core);
int		ps2SND_i_FxDisable(int core);

#ifndef _FINAL_
void	SND_DebugSetRequest(int _i_rq);
int		SND_i_DebugGetStatus(int _i_MenuId);
void	SND_ResetForceNoSound(void);
void	SND_ForceNo(int _i_mask);
void	SND_ResetForceNo(int _i_mask);
void	SND_ForceNoSound(void);
#endif /* _FINAL_ */

/*$2- intern ---------------------------------------------------------------------------------------------------------*/

int		ps2SND_i_AllocSB(void);
void	ps2SND_FreeSB(int _i_ID);

/*$2------------------------------------------------------------------------------------------------------------------*/

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __ps2SND_h__ */
#endif /* PSX2_TARGET */
