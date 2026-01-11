/*$T win32SND.h GC 1.138 05/11/04 11:11:12 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __win32SND_h__
#define __win32SND_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define GetFxSB(p)				((SND_tdst_SoundBuffer *) ((p)->pst_Fx))
#define GetFxInterface(p)	    (((SND_tdst_SoundBuffer *) ((p)->pst_Fx))->pst_Fx)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	win32SND_tdst_Manager_
{
	DSCAPS	st_DriverCapacity;

	int		i_SPlaying;
	int		i_SPlayingMax;
	int		i_Playing;
	int		i_PlayingMax;

	LONG	l_GlobalVol;
} win32SND_tdst_Manager;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- prototypes for master functions --------------------------------------------------------------------------------*/

int		win32SND_l_Init(struct SND_tdst_TargetSpecificData_ *);
void	win32SND_Close(struct SND_tdst_TargetSpecificData_ *);
void	win32SND_SetGlobalVol(LONG);
LONG	win32SND_l_GetGlobalVol(void);
int		win32SND_i_ChangeRenderMode(int);

/*$2- prototypes for sound buffer functions --------------------------------------------------------------------------*/

struct SND_tdst_SoundBuffer_	*win32SND_pst_SB_Create
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_WaveData_ *,
									unsigned int
								);
struct SND_tdst_SoundBuffer_	*win32SND_SB_ReLoad
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_SoundBuffer_ *,
									struct SND_tdst_WaveData_ *,
									unsigned int
								);
int								win32SND_SB_Play(struct SND_tdst_SoundBuffer_ *, int, int, int);
void							win32SND_SB_Stop(struct SND_tdst_SoundBuffer_ *);
void							win32SND_SB_Pause(struct SND_tdst_SoundBuffer_ *);
void							win32SND_SB_Release(struct SND_tdst_SoundBuffer_ *);
void							win32SND_SB_Duplicate
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_SoundBuffer_ *,
									struct SND_tdst_SoundBuffer_ **
								);
void							win32SND_SB_SetCurrPos(struct SND_tdst_SoundBuffer_ *, int);
void							win32SND_SB_SetVolume(struct SND_tdst_SoundBuffer_ *, int);
void							win32SND_SB_SetFrequency(struct SND_tdst_SoundBuffer_ *, int);
void							win32SND_SB_SetPan(struct SND_tdst_SoundBuffer_ *, int, int);
int								win32SND_SB_GetStatus(struct SND_tdst_SoundBuffer_ *, int *);
void							win32SND_SB_GetFrequency(struct SND_tdst_SoundBuffer_ *, int *);
void							win32SND_SB_GetPan(struct SND_tdst_SoundBuffer_ *, int *, int *);
void							win32SND_SB_GetCurrPos(struct SND_tdst_SoundBuffer_ *, int *, int *);

/*$2- prototypes for FX ----------------------------------------------------------------------------------------------*/

int		win32SND_i_FxInit(void);
void	win32SND_FxClose(void);
int		win32SND_i_FxSetMode(int, int);
int		win32SND_i_FxSetDelay(int, int);
int		win32SND_i_FxSetFeedback(int, int);
int		win32SND_i_FxSetWetVolume(int, int);
int		win32SND_i_FxSetWetPan(int, int);
int		win32SND_i_FxEnable(int);
int		win32SND_i_FxDisable(int);

/*$2------------------------------------------------------------------------------------------------------------------*/

int		win32SND_AllocCoreBuffer(struct SND_tdst_SoundBuffer_ *, int core);
void	win32SND_FreeCoreBuffer(struct SND_tdst_SoundBuffer_ *);
void	win32SND_CommitChange(MATH_tdst_Vector *);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern win32SND_tdst_Manager	win32SND_gst_SoundManager;

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __win32SND_h__ */
