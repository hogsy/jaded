/*$T MDFmodifier_SNDVOL.h GC 1.138 02/21/05 11:28:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFmodifier_SOUNDVOL_h__
#define __MDFmodifier_SOUNDVOL_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros + constants
 ***********************************************************************************************************************
 */

#define SND_Cte_MdFSoundVol_Version 0
#define SND_Cte_MdFSoundVol_Reserve 128
#define SND_Cte_MdFSoundVol_Delta  20.0f
/**/
#define SND_Cte_MdFSoundVol_Spheric 0x00000001
#define SND_Cte_MdFSoundVol_Xaxis   0x00000002
#define SND_Cte_MdFSoundVol_Yaxis   0x00000004
#define SND_Cte_MdFSoundVol_Zaxis   0x00000008

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	SND_tdst_ModifierSoundVol_
{
	/* data */
	ULONG	ul_Version;

	/* settings */
	ULONG	ul_Flags;
	int     i_GroupId;
	float	f_NearVol;
	float	af_Near[3];
	float	f_FarVol;
	float	af_Far[3];
    
    /* dynamique data */
    float   f_ActivationRadius;
    int     i_VolRqId;

    /* */
	char	asz_Reserved[SND_Cte_MdFSoundVol_Reserve];
} SND_tdst_ModifierSoundVol;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- sound fx modifier ----------------------------------------------------------------------------------------------*/

void	SND_ModifierSoundVol_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data);
void	SND_ModifierSoundVol_Destroy(MDF_tdst_Modifier *_pst_Mod);
void	SND_ModifierSoundVol_Apply(MDF_tdst_Modifier *_pst_Mod, struct GEO_tdst_Object_ *_pst_Obj);
void	SND_ModifierSoundVol_Unapply(MDF_tdst_Modifier *_pst_Mod, struct GEO_tdst_Object_ *_pst_Obj);
void	SND_ModifierSoundVol_Reinit(MDF_tdst_Modifier *_pst_Mod);
ULONG	SND_ModifierSoundVol_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);

#ifdef ACTIVE_EDITORS
void	SND_ModifierSoundVol_Save(MDF_tdst_Modifier *_pst_Mod);
int		SND_ModifierSoundVol_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src);
void	SND_ModifierSoundVol_Desactivate(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj);
#endif

/*$2------------------------------------------------------------------------------------------------------------------*/

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __MDFmodifier_GEN_h__ */
