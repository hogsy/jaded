/*$T SNDloadingsound.h GC 1.138 12/16/04 12:04:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDloadingsound_h__
#define __SNDloadingsound_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define SND_Cte_LoadingSoundMaxSizePS2	(25 * 2048)
#define SND_Cte_LoadingSoundMaxSizeGC	(SND_Cte_LoadingSoundMaxSizePS2)
#define SND_Cte_LoadingSoundMaxSizePC	(SND_Cte_LoadingSoundMaxSizePS2 * 3)
#define SND_Cte_LoadingSoundMaxSizeXENON	(36*2048*6) // [ADAVID] 2x XBOX
#define SND_Cte_LoadingSoundMaxSizeXBOX (36 * 2048 * 3)
/**/
#ifdef PSX2_TARGET
#define SND_Cte_LoadingSoundMaxSize (SND_Cte_LoadingSoundMaxSizePS2)
#elif defined _GAMECUBE
#define SND_Cte_LoadingSoundMaxSize (SND_Cte_LoadingSoundMaxSizeGC)
#elif defined _XENON
#define SND_Cte_LoadingSoundMaxSize (SND_Cte_LoadingSoundMaxSizeXENON)
#elif defined _XBOX
#define SND_Cte_LoadingSoundMaxSize (SND_Cte_LoadingSoundMaxSizeXBOX)
#else
#define SND_Cte_LoadingSoundMaxSize (SND_Cte_LoadingSoundMaxSizePC)
#endif
/**/
#define SND_Cte_LoadingSoundIsInactive		0
#define SND_Cte_LoadingSoundIsStartingLoad	1
#define SND_Cte_LoadingSoundIsEndingLoad	2
#define SND_Cte_LoadingSoundIsInBadState	(-1)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	SND_tdst_LoadingEffect_
{
	int		i_State;
	float	f_GlobalFadeOutDuration;
	float	f_GlobalFadeInDuration;
	float	f_GlobalFadeFactor;
	float	f_GlobalMute;
	float	f_LoadingTimeOut;
} SND_tdst_LoadingEffect;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern SND_tdst_LoadingEffect	SND_gst_LoadingEffect;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- module ---------------------------------------------------------------------------------------------------------*/

void	SND_LoadingSoundInitModule(void);
void	SND_LoadingSoundCloseModule(void);

/*$2- sound ----------------------------------------------------------------------------------------------------------*/

LONG	SND_l_AddLoadingSound(BIG_KEY _ul_FileKey);
LONG    SND_l_LoadWacData(LONG);

int		SND_b_IsCurrWacSound(int);
int		SND_b_IsCurrWacInstance(int);
void	SND_ReinitWacInstance(void);
void	SND_WacUpdate(void);
void	SND_WacRegisterMdF(struct MDF_tdst_Modifier_ *);
void	SND_WacUnregisterMdF(struct MDF_tdst_Modifier_ *pMdF);
void	SND_WacSetLoading(int);
void	SND_NotifyNewLoading(void);
int		SND_i_GetWacInstance(void);
int		SND_i_GetWacSound(void);
void	SND_DetachWacInstance(void);
void	SND_SetLoadingFadeInTime(float val);
float	SND_f_GetLoadingFadeInTime(void);
void	SND_SetLoadingFadeOutTime(float val);
float	SND_f_GetLoadingFadeOutTime(void);
float	SND_f_GetWacMute(struct SND_tdst_SoundInstance_ *);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDloadingsound_h__ */
