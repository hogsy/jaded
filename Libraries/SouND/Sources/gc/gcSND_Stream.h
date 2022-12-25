/*$T gcSND_Stream.h GC 1.138 12/02/04 11:08:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE
#ifndef __gcSND_Stream_h__
#define __gcSND_Stream_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define gsSND_Cmd_StreamPlay	0x00000001
#define gsSND_Cmd_StreamStop	0x00000002
#define gsSND_Cmd_StreamChain	0x00000004

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef enum	gcSND_tden_StreamState_
{
	e_StreamFree			= 0,
	e_StreamInit,
	e_StreamRamLoading1st,
	e_StreamAramLoading1st,
	e_StreamAramLoaded1st,
	e_StreamWaitingPlayB0,
	e_StreamRamLoadingB1,
	e_StreamAramLoadingB1,
	e_StreamAramLoadedB1,
	e_StreamWaitingPlayB1,
	e_StreamRamLoadingB0,
	e_StreamAramLoadingB0,
	e_StreamAramLoadedB0,
	e_StreamStopRequest,
	e_StreamAutoStopRequest,
	e_StreamDummy			= 0xFFFFFFFF
} gcSND_tden_StreamState;

typedef struct	gcSND_tdst_StereoFile_
{
	struct gcSND_tdst_Stream_	*p_Owner;
	unsigned int				ui_FilePosition;
	unsigned int				ui_FileSize;
	unsigned int				ui_BufferPosition;
	char						*pc_StereoFrameBuffer;

	unsigned int				ui_Block;
	unsigned int				ui_Read;
	unsigned int				ui_Offset;

	char						*pc_UserBuffer;
	unsigned int				ui_UserBufferSize;

	void						*pfv_UserCallback;
	unsigned int				ui_UserContext;
} gcSND_tdst_StereoFile;

typedef struct	SND_tdst_StreamSettings_
{
	volatile unsigned char	uc_IsOneChainOrder;
	volatile unsigned char	uc_SendToFxA;
	volatile unsigned char	uc_SendToFxB;
	volatile unsigned char	uc_Stereo;

	volatile int			i_LoopNb;
	volatile unsigned int	ui_DataSize;
	volatile unsigned int	ui_DataPosition;
	volatile unsigned int	ui_LoopBeginOffset;
	volatile unsigned int	ui_LoopEndOffset;
	volatile unsigned int	ui_StartOffset;
	volatile unsigned int	ui_ExitOffset;
	volatile int			i_Freq;
	volatile int			i_Pan;
	volatile int			i_Vol;
	volatile int			i_FxL;
	volatile int			i_FxR;
} SND_tdst_StreamSettings;

typedef struct	gcSND_tdst_Stream_
{
	int								i_LastCommand;
	int								i_LastArg;
	volatile gcSND_tden_StreamState en_State;
	struct gcSND_tdst_SoftBuffer_	*pst_SoftBuffer;
	volatile unsigned int			ui_AramBufferHalfSize;
	volatile unsigned int			ui_CurrentDataRead;
	volatile unsigned int			ui_LastDvdReadSize;
	gcSND_tdst_StereoFile			*p_StereoFileCache;

	volatile char					b_Dummy;
	volatile char					b_EndReached;
	volatile char					b_PendingDvdAction;
	volatile char					b_PendingAramAction;

	volatile unsigned int			ui_LastPlayingCursor;
	volatile int					i_LastBufferId;
	volatile int					i_Loop;
	volatile unsigned int			ui_LoopBeginOffset;
	volatile unsigned int			ui_LoopEndOffset;
	volatile unsigned int			ui_ChainSize;
	volatile unsigned int			ui_UnloopB0;

	/* new params */
	SND_tdst_StreamSettings			st_NewSettings;
} gcSND_tdst_Stream;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern gcSND_tdst_Stream volatile	gcSND_gax_StreamList[];
extern volatile gcSND_tdst_Stream	*gcSND_gpst_CurrentStream;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void	gcSND_StreamInitModule(void);
void	gcSND_StreamCloseModule(void);
void	gcSND_UpdateStream(void);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void	gcSND_CreateTrackSB(struct SND_tdst_SoundBuffer_ **_ppst_Mono, struct SND_tdst_SoundBuffer_ **_ppst_Stereo);
void	gcSND_ReleaseTrackSB(struct SND_tdst_SoundBuffer_ *_pst_MonoSB, struct SND_tdst_SoundBuffer_ *_pst_StereoSB);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void	gcSND_ShutDownStreams(void);
void	gcSND_StopStreamedSB(struct SND_tdst_SoundBuffer_ *pst_Sound);
void	gcSND_StreamReinitAndPlay
		(
			struct SND_tdst_SoundBuffer_	*_pSB,
			int								_i_Flag,
			int								_i_LoopNb,
			unsigned int					_ui_Size,
			unsigned int					_ui_Position,
			unsigned int					_ui_LoopBegin,
			unsigned int					_ui_LoopEnd,
			int								_i_StartPos,
			int								_i_StopPos,
			int								_i_Freq,
			int								_i_Pan,
			int								_i_Vol,
			int								_i_FxL,
			int								_i_FxR
		);
void	gcSND_Stream_GetCurrPos(struct SND_tdst_SoundBuffer_ *_pst_SB, int *_pi_Pos, int *_pi_Write);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void	gcSND_StreamChain
		(
			struct SND_tdst_SoundBuffer_ *, /* SB */
			int,			/* flag */
			int,			/* loop number */
			unsigned int,	/* exit point (oct) */
			unsigned int,	/* StartOffset (oct) */
			unsigned int,	/* DataSize (oct) */
			unsigned int,	/* DataPosition (oct) */
			unsigned int,	/* LoopBeginOffset (oct) */
			unsigned int	/* LoopEndOffset (oct) */
		);
void	gcSND_StreamLoopCountGet(struct SND_tdst_SoundBuffer_ *, int *);
void	gcSND_StreamChainDelayGet(struct SND_tdst_SoundBuffer_ *, float *);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

int gcSND_i_StreamIGetAvailableRessources(void);
int gcSND_i_StreamITransfertToAudioMemory(struct SND_tdst_Stream_ *pStream);
int gcSND_i_StreamIDiscRead(struct SND_tdst_Stream_ *pStream, char *pBuffer, unsigned int uiSize, unsigned int uiSeek);
int gcSND_i_StreamIUpdatePointers(struct SND_tdst_Stream_ *pStream);
int gcSND_b_StreamIIsPlayingZeroBuffer(struct SND_tdst_Stream_ *pStream);
int gcSND_i_StreamILoopToZeroBuffer(struct SND_tdst_Stream_ *pStream);
int gcSND_i_StreamIStop(struct SND_tdst_Stream_ *pStream);
int gcSND_i_StreamIPlay(struct SND_tdst_Stream_ *pStream);
int gcSND_i_StreamIInit(struct SND_tdst_Stream_ *pStream);
int gcSND_i_StreamICallback1stReadDone(struct SND_tdst_Stream_ *pStream);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __gcSND_Stream_h__ */
#endif /* _GAMECUBE */
