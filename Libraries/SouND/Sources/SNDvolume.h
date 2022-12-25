/*$T SNDvolume.h GC 1.138 03/08/04 16:46:02 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDvolume_h__
#define __SNDvolume_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define SND_Cte_FadingIn	0x00000001
#define SND_Cte_FadingOut	0x00000002

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef enum	SND_tden_GroupId_
{
	SND_e_GrpSpecialFX		= 0,
	SND_e_GrpMusic			= 1,
	SND_e_GrpAmbience		= 2,
	SND_e_GrpDialog			= 3,
	SND_e_GrpCutScene		= 4,
	SND_e_GrpInterface		= 5,
	/**/
	SND_e_GrpA				= 6,
	SND_e_GrpB				= 7,
	/**/
	SND_e_MasterGrp			= 8,
	/**/
	SND_e_UserGrpMusic		= 9,
	SND_e_UserGrpDialog		= 10,
	SND_e_UserGrpSpecialFX	= 11,
	SND_e_UserGrpMaster		= 12,
	/**/
	SND_e_GrpNumber			= 13,
	SND_e_GrpDummy			= 0xFFFFFFFF
} SND_tden_GroupId;

#define SND_Cte_GrpSpecialFX	0x00000001
#define SND_Cte_GrpMusic		0x00000002
#define SND_Cte_GrpAmbience		0x00000004
#define SND_Cte_GrpDialog		0x00000008
#define SND_Cte_GrpCutScene		0x00000010
#define SND_Cte_GrpInterface	0x00000020

#define SND_Cte_GrpMask			0x0000003F
#define SND_M_GetGrp(a)			((a) & SND_Cte_GrpMask)
#define SND_M_SetGrp(a, b) \
	do \
	{ \
		(a) &= ~SND_Cte_GrpMask; \
		(a) |= (b) & SND_Cte_GrpMask; \
	} while(0);

#define SND_Cte_GrpA				0x00000040
#define SND_Cte_GrpB				0x00000080

#define SND_Cte_UserGrpMusic		(SND_Cte_GrpMusic)
#define SND_Cte_UserGrpDialog		(SND_Cte_GrpCutScene | SND_Cte_GrpDialog)
#define SND_Cte_UserGrpSpecialFX	(SND_Cte_GrpSpecialFX | SND_Cte_GrpAmbience | SND_Cte_GrpInterface)

#define SND_Cte_GrpVolRqMax 32   

typedef struct	SND_tdst_Group_
{
	float	f_Volume;
	float	f_FxVolume;

    ULONG   ul_VolRqFlags;
    ULONG   ul_VolRqChanged;
    float  af_VolRq[SND_Cte_GrpVolRqMax];

#ifdef ACTIVE_EDITORS
	BOOL	b_Mute;
	BOOL	b_Solo;
	BOOL	b_ByPass;
#endif

#ifdef _DEBUG
	BOOL	b_VolWatch;
	BOOL	b_FxVolWatch;
	ULONG	ul_WatchRq;
#endif
} SND_tdst_Group;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */
#ifdef JADEFUSION
extern __declspec(align(32)) SND_tdst_Group	SND_gdst_Group[SND_e_GrpNumber];
#else
extern SND_tdst_Group	SND_gdst_Group[SND_e_GrpNumber];
#endif
extern float			SND_gf_AutoVolumeOff;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    volume <-> attenuation
 -----------------------------------------------------------------------------------------------------------------------
 */

void	SND_l_InitVolumeTable(void);
LONG	SND_l_GetAttFromVol(float f);
float	SND_f_GetVolFromAtt(LONG _l_Vol);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    global vol / pan
 -----------------------------------------------------------------------------------------------------------------------
 */

void	SND_SetGlobalVol(float);
float	SND_f_GetGlobalVol(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    instance
 -----------------------------------------------------------------------------------------------------------------------
 */

void	SND_SetInstVolume(struct SND_tdst_SoundInstance_ *);
float	SND_f_GetInstVolume(struct SND_tdst_SoundInstance_ *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    3D volume
 -----------------------------------------------------------------------------------------------------------------------
 */

float	SND_f_Compute3DVol(struct SND_tdst_SoundInstance_ *);
BOOL	SND_b_IsInActiveVolume(LONG _i_Index);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    groups
 -----------------------------------------------------------------------------------------------------------------------
 */
void SND_GroupFreeRequestId(int Id, int i);
void SND_GroupSendRequest(int Id, int RqId, float f_Volume);
int     SND_i_GroupGetRequestId(int Id);
void    SND_ReinitGroupRequest(void);


void	SND_GroupVolumeSet(int, float);
float	SND_f_GroupVolumeGet(int);

void	SND_GroupFxVolumeSet(int, float);
float	SND_f_GroupFxVolumeGet(int);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SND_H__ */
