/*$T ediSND_Stream.h GC 1.138 06/29/04 08:59:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __ediSND_Stream_h__
#define __ediSND_Stream_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void	ediSND_StreamInitModule(void);
void	ediSND_StreamCloseModule(void);

void	ediSND_CreateTrackSB(struct SND_tdst_SoundBuffer_ **, struct SND_tdst_SoundBuffer_ **);
void	ediSND_ReleaseTrackSB(struct SND_tdst_SoundBuffer_ *, struct SND_tdst_SoundBuffer_ *);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	ediSND_StreamStop(SND_tdst_SoundBuffer *);
void	ediSND_StreamPause(SND_tdst_SoundBuffer *);
void	ediSND_StreamRelease(SND_tdst_SoundBuffer *);
void	ediSND_StreamReinitAndPlay
		(
			SND_tdst_SoundBuffer	*_pst_SB,
			int						_i_Flag,
			int						_i_FormatTag,
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
void	ediSND_StreamGetCurrPos(SND_tdst_SoundBuffer *, int *, int *);
void	ediSND_StreamChain
		(
			SND_tdst_SoundBuffer	*pSB,
			int						iFlag,
			int						iFormatTag,
			int						iLoop,
			unsigned int			uiExitPoint,
			unsigned int			uiEnterPoint,
			unsigned int			uiNewSize,
			unsigned int			uiNewPosition,
			unsigned int			uiLoopBegin,
			unsigned int			uiLoopEnd
		);
void	ediSND_StreamLoopCountGet(SND_tdst_SoundBuffer *_pSB, int *pi);
void	ediSND_StreamChainDelayGet(SND_tdst_SoundBuffer *_pSB, float *pf);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	ediSND_StreamPrefetch(unsigned int, unsigned int);
void	ediSND_StreamPrefetchArray(unsigned int *, unsigned int *);
void	ediSND_StreamFlush(unsigned int);
void	ediSND_StreamGetPrefetchStatus(unsigned int *);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	ediSND_ShutDownStreams(void);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
