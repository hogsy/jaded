/*$T SNDtrack.h GC 1.138 02/17/05 15:05:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDtrack_h__
#define __SNDtrack_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    track flags
 -----------------------------------------------------------------------------------------------------------------------
 */

#define SND_Cte_TrackIsStereo	0x00000004

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    const
 -----------------------------------------------------------------------------------------------------------------------
 */

#define SND_XFADE_CHAINING
#define SND_Cte_MaxSimultaneousStream		4
#define SND_Cte_MaxSimultaneousStereoStream 2
#define SND_Cte_StreamFrequency				24000

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    track request id
 -----------------------------------------------------------------------------------------------------------------------
 */

#define SND_Cte_TrackRq_ChainInstance	0x10000001
#define SND_Cte_TrackRq_ChainDelay		0x10000002
#define SND_Cte_TrackRq_Instance		0x10000003
#define SND_Cte_TrackRq_PauseAll		0x10000004
#define SND_Cte_TrackRq_UnpauseAll		0x10000005
/**/
#define SND_Cte_TrackRq_ModeWaitMarker	0x20000001
#define SND_Cte_TrackRq_ModeWaitRegion	0x20000002
#define SND_Cte_TrackRq_ModeNoWait		0x20000003
/**/
#define SND_Cte_TrackRq_TagStart	0x30000001
#define SND_Cte_TrackRq_TagLoop		0x30000002
#define SND_Cte_TrackRq_TagEnd		0x30000004

/**/
#define SND_M_CheckTrackIdOrReturn(_id, _retcode) \
	do \
	{ \
		if((_id) < SND_e_TrackA) return _retcode; \
		if((_id) >= SND_e_TrackNb) return _retcode; \
	} while(0);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    macros
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef enum	SND_tden_TrackId_
{
	SND_e_TrackA	= 0,
	SND_e_TrackB	= 1,
	SND_e_TrackC	= 2,
	SND_e_TrackD	= 3,
	SND_e_TrackNb	= 4,
	SND_e_TrackDummy= 0xFFFFFFFF
} SND_tden_TrackId;

/*$4
 ***********************************************************************************************************************
    structure
 ***********************************************************************************************************************
 */

#ifndef PSX2_IOP

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern int						SND_gi_TrackNb; /* track list size */
extern struct BAS_tdst_barray_	SND_gst_TrackId;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*$2- module fct -----------------------------------------------------------------------------------------------------*/

void	SND_TrackInitModule(void);
void	SND_TrackCloseModule(void);

/*$2- one track ------------------------------------------------------------------------------------------------------*/

int		SND_i_TrackSeti(int, int, int);
int		SND_i_TrackGeti(int, int);
/**/
int		SND_i_IsTrackPlaying(int _i_VirtualId);
void	SND_TrackStop(int _i_VirtualId);
void	SND_TrackAttachAndPlayInstance(int, int, int, int, int);
void	SND_TrackDetachInstance(int);
void	SND_InstTrackSet(int _l_Index, int _i_Track);
BOOL	SND_DialogInstChooseTrack();
/**/
void	SND_TrackFreeAll(void);
void	SND_TrackPauseAll(BOOL b_Stop);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void	SND_TrackUpdate(void);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

float	SND_f_Track_ChainPlayer
		(
			int				iOldInstance,
			int				iNewInstance,
			unsigned int	uiMode,
			unsigned int	uiStartRegionTag,
			int				iLoop
		);
int		SND_i_SignalIsActive(char *_pz_Label);

/*$2- low interface --------------------------------------------------------------------------------------------------*/

int		SND_iTrack_Play(struct SND_tdst_SoundBuffer_ *, int _i_Flag, int l, int r);
void	SND_Track_Stop(struct SND_tdst_SoundBuffer_ *);
void	SND_Track_Pause(struct SND_tdst_SoundBuffer_ *);
void	SND_Track_Release(struct SND_tdst_SoundBuffer_ *);
void	SND_Track_SetCurrPos(struct SND_tdst_SoundBuffer_ *, int _i_Pos);
void	SND_Track_SetVolume(struct SND_tdst_SoundBuffer_ *, int _i_Vol);
void	SND_Track_SetFrequency(struct SND_tdst_SoundBuffer_ *, int _i_Freq);
void	SND_Track_SetPan(struct SND_tdst_SoundBuffer_ *, int _i_Pan, int _i_FrontRear);
int		SND_iTrack_GetStatus(struct SND_tdst_SoundBuffer_ *, int *);
void	SND_Track_GetFrequency(struct SND_tdst_SoundBuffer_ *, int *);
void	SND_Track_GetPan(struct SND_tdst_SoundBuffer_ *, int *, int *);
void	SND_Track_GetCurrPos(struct SND_tdst_SoundBuffer_ *, int *, int *);
#define SND_TrackModulateFrequency(a)	(0)
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDtrack_h__ */
