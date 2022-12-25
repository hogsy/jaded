/*$T SNDstream.h GC 1.138 05/05/04 10:03:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDstream_h__
#define __SNDstream_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#ifdef _XBOX
#define SND_Cte_StreamPrefetchMax	4
#else
#define SND_Cte_StreamPrefetchMax	6
#endif

#define SND_Cte_StreamPrefetchSync	0
#define SND_Cte_StreamPrefetchAsync 1

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef enum	SND_tden_PrefetchState_
{
	e_PrefecthRequestFree					= 0,
	e_PrefecthRequestWaitingForSyncAction	= 1,
	e_PrefecthRequestRunningAsync			= 2,
	e_PrefecthRequestWaitingForUsing		= 3,
	e_Dummy32bits							= 0xFFFFFFFF
} SND_tden_PrefetchState;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- module ---------------------------------------------------------------------------------------------------------*/

void	SND_StreamInitModule(void);
void	SND_StreamCloseModule(void);

/*$2- load interface -------------------------------------------------------------------------------------------------*/

LONG	SND_l_AddStreamNoLoad(BIG_KEY _ul_WavKey);
LONG	SND_l_AddStreamLoad(BIG_KEY _ul_WavKey);
void	SND_StreamPrefetch(LONG _l_Instance);
void	SND_StreamFlush(LONG _l_Instance);
int	    SND_StreamPrefetchOneSound(LONG _l_SoundIndex, int i_mode);
void    SND_StreamPrefetchSetLife(int idx, float fTime);
void	SND_StreamResolveSyncPrefetch(void);
void	SND_StreamFlushOneSound(LONG, BOOL);
void	SND_StreamPrefetchFlushAll(void);
int		SND_i_StreamSoundIsPrefetched(LONG _l_Sound);
void    SND_StreamPrefetchUpdate(void);


/*$2- Low Interface --------------------------------------------------------------------------------------------------*/

void	SND_Stream_Stop(struct SND_tdst_SoundBuffer_ *);
void	SND_Stream_Release(struct SND_tdst_SoundBuffer_ *);
void	SND_Stream_SetCurrPos(struct SND_tdst_SoundBuffer_ *, int _i_Pos);
void	SND_Stream_SetVolume(struct SND_tdst_SoundBuffer_ *, int _i_Vol);
void	SND_Stream_SetFrequency(struct SND_tdst_SoundBuffer_ *, int _i_Freq);
void	SND_Stream_SetPan(struct SND_tdst_SoundBuffer_ *, int _i_Pan, int _i_FrontRear);
int		SND_Stream_GetStatus(struct SND_tdst_SoundBuffer_ *, int *);
void	SND_Stream_GetFrequency(struct SND_tdst_SoundBuffer_ *, int *);
void	SND_Stream_GetPan(struct SND_tdst_SoundBuffer_ *, int *, int *);
void	SND_Stream_GetCurrPos(struct SND_tdst_SoundBuffer_ *, int *, int *);

int     SND_Stream_Play(struct SND_tdst_SoundBuffer_ *p, int i1, int i2, int i3);
void    SND_Stream_Pause(struct SND_tdst_SoundBuffer_ *p);
void    SND_Stream_Duplicate(struct SND_tdst_TargetSpecificData_ *p,struct SND_tdst_SoundBuffer_ *pp, struct SND_tdst_SoundBuffer_ **ppp);
struct SND_tdst_SoundBuffer_ *SND_Stream_Create(struct SND_tdst_TargetSpecificData_ *p, struct SND_tdst_WaveData_ *pp, unsigned int ui);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	SND_StreamStopAll(void);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDstream_h__ */
