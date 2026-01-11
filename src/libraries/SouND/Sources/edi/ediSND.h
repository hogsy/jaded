/*$T ediSND.h GC 1.138 12/14/04 08:55:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __ediSND_h__
#define __ediSND_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define GetFxSB(p)				((SND_tdst_SoundBuffer *) ((p)->pst_Fx))
#define GetFxInterface(p)	    (((SND_tdst_SoundBuffer *) ((p)->pst_Fx))->pst_Fx)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	ediSND_tdst_Manager_
{
	DSCAPS	st_DriverCapacity;

	int		i_SPlaying;
	int		i_SPlayingMax;
	int		i_Playing;
	int		i_PlayingMax;

	LONG	l_GlobalVol;
} ediSND_tdst_Manager;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- prototypes for master functions --------------------------------------------------------------------------------*/

int		ediSND_l_Init(struct SND_tdst_TargetSpecificData_ *);
void	ediSND_Close(struct SND_tdst_TargetSpecificData_ *);
void	ediSND_SetGlobalVol(LONG);
LONG	ediSND_l_GetGlobalVol(void);
int		ediSND_i_ChangeRenderMode(int);

/*$2- prototypes for sound buffer functions --------------------------------------------------------------------------*/

struct SND_tdst_SoundBuffer_	*ediSND_pst_SB_Create
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_WaveData_ *,
									unsigned int
								);
struct SND_tdst_SoundBuffer_	*ediSND_SB_ReLoad
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_SoundBuffer_ *,
									struct SND_tdst_WaveData_ *,
									unsigned int
								);
int								ediSND_SB_Play(struct SND_tdst_SoundBuffer_ *, int, int, int);
void							ediSND_SB_Stop(struct SND_tdst_SoundBuffer_ *);
void							ediSND_SB_Pause(struct SND_tdst_SoundBuffer_ *);
void							ediSND_SB_Release(struct SND_tdst_SoundBuffer_ *);
void							ediSND_SB_Duplicate
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_SoundBuffer_ *,
									struct SND_tdst_SoundBuffer_ **
								);
void							ediSND_SB_SetCurrPos(struct SND_tdst_SoundBuffer_ *, int);
void							ediSND_SB_SetVolume(struct SND_tdst_SoundBuffer_ *, int);
void							ediSND_SB_SetFrequency(struct SND_tdst_SoundBuffer_ *, int);
void							ediSND_SB_SetPan(struct SND_tdst_SoundBuffer_ *, int, int);
int								ediSND_SB_GetStatus(struct SND_tdst_SoundBuffer_ *, int *);
void							ediSND_SB_GetFrequency(struct SND_tdst_SoundBuffer_ *, int *);
void							ediSND_SB_GetPan(struct SND_tdst_SoundBuffer_ *, int *, int *);
void							ediSND_SB_GetCurrPos(struct SND_tdst_SoundBuffer_ *, int *, int *);

/*$2- prototypes for FX ----------------------------------------------------------------------------------------------*/

int		ediSND_i_FxInit(void);
void	ediSND_FxClose(void);
int		ediSND_i_FxSetMode(int, int);
int		ediSND_i_FxSetDelay(int, int);
int		ediSND_i_FxSetFeedback(int, int);
int		ediSND_i_FxSetWetVolume(int, int);
int		ediSND_i_FxSetWetPan(int, int);
int		ediSND_i_FxEnable(int);
int		ediSND_i_FxDisable(int);
void    ediSND_FxDel(struct SND_tdst_SoundBuffer_*_pst_SB);
void    ediSND_FxAdd(struct SND_tdst_SoundBuffer_*_pst_SB, int _i_Core, int _i_FxVol);

/*$2------------------------------------------------------------------------------------------------------------------*/

int		ediSND_AllocCoreBuffer(struct SND_tdst_SoundBuffer_ *, int core);
void	ediSND_FreeCoreBuffer(struct SND_tdst_SoundBuffer_ *);
void	ediSND_CommitChange(MATH_tdst_Vector *);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern ediSND_tdst_Manager	ediSND_gst_SoundManager;

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __ediSND_h__ */
