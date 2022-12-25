/*$T win32SND_Stream.h GC 1.138 06/29/04 08:59:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __win32SND_Stream_h__
#define __win32SND_Stream_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void	win32SND_StreamInitModule(void);
void	win32SND_StreamCloseModule(void);

void	win32SND_CreateTrackSB(struct SND_tdst_SoundBuffer_ **, struct SND_tdst_SoundBuffer_ **);
void	win32SND_ReleaseTrackSB(struct SND_tdst_SoundBuffer_ *, struct SND_tdst_SoundBuffer_ *);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	win32SND_StreamStop(SND_tdst_SoundBuffer *);
void	win32SND_StreamPause(SND_tdst_SoundBuffer *);
void	win32SND_StreamRelease(SND_tdst_SoundBuffer *);
void	win32SND_StreamReinitAndPlay
		(
			SND_tdst_SoundBuffer	*_pst_SB,
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
void	win32SND_StreamGetCurrPos(SND_tdst_SoundBuffer *, int *, int *);
void	win32SND_StreamChain
		(
			SND_tdst_SoundBuffer	*pSB,
			int						iFlag,
			int						iLoop,
			unsigned int			uiExitPoint,
			unsigned int			uiEnterPoint,
			unsigned int			uiNewSize,
			unsigned int			uiNewPosition,
			unsigned int			uiLoopBegin,
			unsigned int			uiLoopEnd
		);
void	win32SND_StreamLoopCountGet(SND_tdst_SoundBuffer *_pSB, int *pi);
void	win32SND_StreamChainDelayGet(SND_tdst_SoundBuffer *_pSB, float *pf);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	win32SND_StreamPrefetch(unsigned int, unsigned int);
void	win32SND_StreamPrefetchArray(unsigned int *, unsigned int *);
void	win32SND_StreamFlush(unsigned int);
void	win32SND_StreamGetPrefetchStatus(unsigned int *);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	win32SND_ShutDownStreams(void);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
