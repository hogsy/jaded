/*$T xbSND_Stream.h GC 1.138 02/16/05 17:44:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __xbSND_Stream_h__
#define __xbSND_Stream_h__

#include "SNDWave.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    constantes
 ***********************************************************************************************************************
 */

#define xbSND_Cte_NotifyNb	2

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2------------------------------------------------------------------------------------------------------------------*/

int		xbSND_i_InitStreamModule(void);
void	xbSND_CloseStreamModule(void);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	xbSND_CreateTrackSB(struct SND_tdst_SoundBuffer_ **, struct SND_tdst_SoundBuffer_ **);
void	xbSND_ReleaseTrackSB(struct SND_tdst_SoundBuffer_ *, struct SND_tdst_SoundBuffer_ *);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	xbSND_StreamRelease(struct SND_tdst_SoundBuffer_ *);
void	xbSND_StreamReinitAndPlay
		(
			struct SND_tdst_SoundBuffer_ *, /* SB */
			int,			/* flag */
			int,			/* loop nb */
			unsigned int,	/* DataSize, */
			unsigned int,	/* DataPosition, */
			unsigned int,	/* LoopBeginOffset, */
			unsigned int,	/* LoopEndOffset, */
			int,			/* StartPos, */
			int,			/* StopPos, */
			int,			/* BaseFrequency, */
			int,			/* Pan, */
			int,			/* volume */
			int,			/* fx left vol */
			int				/* fx right vol */
		);
void	xbSND_StreamGetStatus(SND_tdst_SoundBuffer *, int *);
void	xbSND_StreamPrefetch(unsigned int, unsigned int);
void	xbSND_StreamPrefetchArray(unsigned int *, unsigned int *);
void	xbSND_StreamFlush(unsigned int);
void	xbSND_ShutDownStreams(void);
void	xbSND_Stream_GetCurrPos(struct SND_tdst_SoundBuffer_ *, int *, int *);
void	xbSND_StreamGetPrefetchStatus(unsigned int *a);
void	xbSND_StreamChain
		(
			struct SND_tdst_SoundBuffer_ *,
			int,
			int,
			unsigned int,
			unsigned int,
			unsigned int,
			unsigned int,
			unsigned int,
			unsigned int
		);
void	xbSND_StreamLoopCountGet(struct SND_tdst_SoundBuffer_ *, int *);
void	xbSND_StreamChainDelayGet(struct SND_tdst_SoundBuffer_ *, float *);
void	xbSND_StreamStop(struct SND_tdst_SoundBuffer_ *);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	xbSND_ThStreamReinit(int _i_ID);
void	xbSND_ThStreamReadDone(int _i_ID);
void	xbSND_ThStreamNotify(int _i_StreamId, int _i_BufferId);
void	xbSND_ThPrefetchDone(int _i_PrefetchId);
void	xbSND_ThStreamEndHit(int _i_StreamId);

/*$2------------------------------------------------------------------------------------------------------------------*/

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __xbSND_Stream_h__ */
