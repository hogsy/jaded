/*$T MDFmodifier_SOUNDFX.h GC 1.138 07/29/04 17:02:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFmodifier_SOUNDFX_h__
#define __MDFmodifier_SOUNDFX_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

#define SND_Cte_MdFFxVersion		0x00000102
#define MDF_Cte_SndFx_ReserveSize	48

/*$2- state ----------------------------------------------------------------------------------------------------------*/

#define MDF_Cte_SndFx_Activated			0x00000001
#define MDF_Cte_SndFx_EdiDontDisplay	0x00000002
#define MDF_Cte_SndFx_Sphere			0x00000004
#define MDF_Cte_SndFx_NetMaster			0x00000008
#define MDF_Cte_SndFx_NetSlave			0x00000010

typedef struct	GEN_tdst_ModifierSoundFx_
{
	/* MdF data */
	unsigned int						ui_MdFVersion;
	unsigned int						ui_MdfFlag;

	/* Fx Activation */
	float								af_Distance[3];
	float								f_Delta;

	/* Fx settings */
	int									i_CoreId;
	int									i_Mode;
	int									i_Delay;
	int									i_Feedback;
	float								f_WetVol;
	float								af_Far[3];
	int								    i_NetIdx;
    void                                *p_Next;
	char								asz_Reserved[MDF_Cte_SndFx_ReserveSize];
} GEN_tdst_ModifierSoundFx;

#define SND_Cte_FxNetworkNbMax 32

typedef struct SND_tdst_FxNetworkRoot_
{
    int     i_NetIdx;
    void    *p_Root;
} SND_tdst_FxNetworkRoot;

extern SND_tdst_FxNetworkRoot SND_gax_FxNetwork[SND_Cte_FxNetworkNbMax];

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- sound fx modifier ----------------------------------------------------------------------------------------------*/

void	GEN_ModifierSoundFx_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data);
void	GEN_ModifierSoundFx_Destroy(MDF_tdst_Modifier *_pst_Mod);
void	GEN_ModifierSoundFx_Apply(MDF_tdst_Modifier *_pst_Mod, struct GEO_tdst_Object_ *_pst_Obj);
void	GEN_ModifierSoundFx_Unapply(MDF_tdst_Modifier *_pst_Mod, struct GEO_tdst_Object_ *_pst_Obj);
void	GEN_ModifierSoundFx_Reinit(MDF_tdst_Modifier *_pst_Mod);
ULONG	GEN_ModifierSoundFx_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);
#ifdef ACTIVE_EDITORS
void	GEN_ModifierSoundFx_Save(MDF_tdst_Modifier *_pst_Mod);
int     GEN_ModifierSoundFx_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src );
#endif

/*$2------------------------------------------------------------------------------------------------------------------*/

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __MDFmodifier_GEN_h__ */
