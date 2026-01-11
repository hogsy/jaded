/*$T xbSND.h GC 1.138 01/14/05 15:18:10 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __xbSND_h__
#define __xbSND_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define xbSND_Cte_AlignSize 64

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define xbSND_Idx_FrontLeft		0
#define xbSND_Idx_FrontRight	1
#define xbSND_Idx_BackLeft		2
#define xbSND_Idx_BackRight		3
#define xbSND_Idx_CenterLeft	4
#define xbSND_Idx_CenterRight	5
#define xbSND_Idx_FxLeft		6
#define xbSND_Idx_FxRight		7
#define xbSND_Idx_WithFxNb		8
#define xbSND_Idx_NoFxNb		6

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define xbSND_Cte_SendFrontLeft			DSMIXBIN_3D_FRONT_LEFT
#define xbSND_Cte_SendFrontRight		DSMIXBIN_3D_FRONT_RIGHT
#define xbSND_Cte_SendBackLeft			DSMIXBIN_3D_BACK_LEFT
#define xbSND_Cte_SendBackRight			DSMIXBIN_3D_BACK_RIGHT
#define xbSND_Cte_SendCenterLeft		DSMIXBIN_FXSEND_6
#define xbSND_Cte_SendCenterRight		DSMIXBIN_FXSEND_7
#define xbSND_Cte_SendFxReverbALeft		DSMIXBIN_FXSEND_0
#define xbSND_Cte_SendFxReverbARight	DSMIXBIN_FXSEND_0
#define xbSND_Cte_SendFxEchoALeft		DSMIXBIN_FXSEND_1
#define xbSND_Cte_SendFxEchoARight		DSMIXBIN_FXSEND_2
#define xbSND_Cte_SendFxReverbBLeft		DSMIXBIN_FXSEND_5
#define xbSND_Cte_SendFxReverbBRight	DSMIXBIN_FXSEND_5
#define xbSND_Cte_SendFxEchoBLeft		DSMIXBIN_FXSEND_3
#define xbSND_Cte_SendFxEchoBRight		DSMIXBIN_FXSEND_4

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define xbSND_M_GetVolume(__v)	((__v) > DSBVOLUME_MAX ? DSBVOLUME_MAX : ((__v) < DSBVOLUME_MIN ? DSBVOLUME_MIN : (__v)))
#define l_6dB					600
#define l_12dB					1200

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern struct SND_tdst_SoundBuffer_ * volatile xbSND_gp_ReloadingBuffer;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- module functions -----------------------------------------------------------------------------------------------*/

int		xbSND_l_Init(struct SND_tdst_TargetSpecificData_ *);
void	xbSND_Close(struct SND_tdst_TargetSpecificData_ *);

/*$2- Master functions -----------------------------------------------------------------------------------------------*/

void	xbSND_SetGlobalVol(LONG _i_Level);
LONG	xbSND_l_GetGlobalVol(void);
void	xbSND_SetGlobalPan(LONG _i_Pan);
LONG	xbSND_l_GetGlobalPan(void);
int		xbSND_i_ChangeRenderMode(int _i_Mode);

/*$2- SB functions ---------------------------------------------------------------------------------------------------*/

struct SND_tdst_SoundBuffer_	*xbSND_pst_SB_Create
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_WaveData_ *,
									unsigned int
								);
int								xbSND_SB_Play
								(
									struct SND_tdst_SoundBuffer_	*_pst_SB,
									int								_i_Flag,
									int								fxvol1,
									int								fxvol2
								);
void							xbSND_SB_Stop(struct SND_tdst_SoundBuffer_ *);
void							xbSND_SB_Release(struct SND_tdst_SoundBuffer_ *);
void							xbSND_SB_Duplicate
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_SoundBuffer_ *,
									struct SND_tdst_SoundBuffer_ **
								);
void							xbSND_SB_SetVolume(struct SND_tdst_SoundBuffer_ *, int _i_Level);
void							xbSND_SB_SetFrequency(struct SND_tdst_SoundBuffer_ *, int _i_Freq);
void							xbSND_SB_SetPan(struct SND_tdst_SoundBuffer_ *, int _i_Pan, int _i_FrontRear);
int								xbSND_SB_GetStatus(struct SND_tdst_SoundBuffer_ *, int *);
void							xbSND_SB_GetFrequency(struct SND_tdst_SoundBuffer_ *, int *);
void							xbSND_SB_GetPan(struct SND_tdst_SoundBuffer_ *, int *, int *);
void							xbSND_SB_GetCurrPos(struct SND_tdst_SoundBuffer_ *, int *, int *);
void							xbSND_SB_Pause(struct SND_tdst_SoundBuffer_ *);
void							xbSND_SB_SetCurrPos(struct SND_tdst_SoundBuffer_ *, int _i_Pos);
void							xbSND_SetFilter(struct SND_tdst_SoundBuffer_ *pst_SB);

/*$2------------------------------------------------------------------------------------------------------------------*/

struct SND_tdst_SoundBuffer_	*xbSND_SB_ReLoad
								(
									struct SND_tdst_TargetSpecificData_ *,
									struct SND_tdst_SoundBuffer_ *,
									struct SND_tdst_WaveData_ *,
									unsigned int i
								);

/*$2- FX functions ---------------------------------------------------------------------------------------------------*/

int		xbSND_i_FxInit(void);
void	xbSND_FxClose(void);
int		xbSND_i_FxSetMode(int, int _i_Value);
int		xbSND_i_FxGetMode(int);
int		xbSND_i_FxSetDelay(int, int _i_Value);
int		xbSND_i_FxGetDelay(int);
int		xbSND_i_FxSetFeedback(int, int _i_Value);
int		xbSND_i_FxGetFeedback(int);
int		xbSND_i_FxSetWetVolume(int, int _i_Level);
int		xbSND_i_FxGetWetVolume(int);
int		xbSND_i_FxSetWetPan(int i1, int i2);
int		xbSND_i_FxGetWetPan(int);
int		xbSND_i_FxEnable(int);
int		xbSND_i_FxDisable(int);

int		xbSND_i_FxAddSB(int core, struct SND_tdst_SoundBuffer_ *);
int		xbSND_i_FxDelSB(struct SND_tdst_SoundBuffer_ *);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	xbSND_CommitChange(MATH_tdst_Vector *);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	xbSND_EventReloadDone(void);
void	xbSND_EventDoReload(void);

/*$2------------------------------------------------------------------------------------------------------------------*/

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __xbSND_h__ */
