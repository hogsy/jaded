/*$T SNDinterface.h GC 1.138 12/16/04 10:27:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDinterface_h__
#define __SNDinterface_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2- global ---------------------------------------------------------------------------------------------------------*/

#define SND_Cte_SetDopplerFactor		0x00000000
#define SND_Cte_SetRolloffFactor		0x00000001
#define SND_Cte_SetSurroundFactor		0x00000002
#define SND_Cte_SetFreeze3DVol			0x00000007
#define SND_Cte_SetFreezeDynamicPan		0x00000008
#define SND_Cte_SetLoading				0x00000009
#define SND_Cte_SetLoadingFadeInTime	0x0000000B
#define SND_Cte_SetLoadingFadeOutTime	0x0000000C
#define SND_Cte_SetDopplerSoundSpeed	0x0000000D
#define SND_Cte_SetRenderMode			0x0000000E
#define SND_Cte_GetRenderModeCapacity	0x0000000F

/*$2- instance -------------------------------------------------------------------------------------------------------*/

#define SND_Cte_SetInstNearFar0				0x00001000
#define SND_Cte_SetInstNearFar1				0x00001001
#define SND_Cte_SetInstNearFar2				0x00001002
#define SND_Cte_SetInstMiddle0				0x00001003
#define SND_Cte_SetInstMiddle1				0x00001004
#define SND_Cte_SetInstMiddle2				0x00001005
#define SND_Cte_SetInstFarCoeff				0x00001006
#define SND_Cte_SetInstMiddleCoeff			0x00001007
#define SND_Cte_SetInstDeltaFar				0x00001008
#define SND_Cte_SetInstMinPan				0x00001009
#define SND_Cte_SetInstNear0				0x0000100A
#define SND_Cte_SetInstFar0					0x0000100B
#define SND_Cte_SetInstNear1				0x0000100C
#define SND_Cte_SetInstFar1					0x0000100D
#define SND_Cte_SetInstNear2				0x0000100E
#define SND_Cte_SetInstFar2					0x0000100F
#define SND_Cte_GetInstStreamPlayingStatus	0x00001010
#define SND_Cte_SetInstCylinderHeight		0x00001011
#define SND_Cte_SetInstDelay				0x00001012
#define SND_Cte_SetInstGao					0x00001013
#define SND_Cte_GetInstDuration				0x00001014
#define SND_Cte_SetInstStartDate			0x00001015
#define SND_Cte_GetInstFadeIn				0x00001016
#define SND_Cte_GetInstFadeOut				0x00001017
#define SND_Cte_GetInstFxCore				0x00001018
#define SND_Cte_GetInstTrack				0x00001019
#define SND_Cte_SetInstDoppler				0x0000101A
#define SND_Cte_SetInstGroup				0x0000101B
#define SND_Cte_SetInstFxVolLeft			0x0000101C
#define SND_Cte_SetInstFxVolRight			0x0000101D
#define SND_Cte_GetInstSoundKey				0x0000101E
#define SND_Cte_GetInstFlags				0x0000101F
#define SND_Cte_GetInstLoop                 0x00001020

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

int		SND_i_Setf(int, float);
float	SND_f_Getf(int);
int		SND_i_Seti(int, int);
int		SND_i_Geti(int);

/*$2------------------------------------------------------------------------------------------------------------------*/

int		SND_i_InstanceSetf(int, int, float);
float	SND_f_InstanceGetf(int, int);
int		SND_i_InstanceSeti(int, int, int);
int		SND_i_InstanceGeti(int, int);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDinterface_h__ */
