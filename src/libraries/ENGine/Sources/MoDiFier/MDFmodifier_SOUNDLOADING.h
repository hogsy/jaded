/*$T MDFmodifier_SOUNDLOADING.h GC 1.138 01/11/05 10:31:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFmodifier_SOUNDLOADING_h__
#define __MDFmodifier_SOUNDLOADING_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define SND_Cte_MdFLSVersion		0x00000101
#define MDF_Cte_SndLS_ReserveSize	256

/*$2- state ----------------------------------------------------------------------------------------------------------*/

#define MDF_Cte_SndLS_EdiDontDisplay	0x00000001

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	MDF_tdst_LoadingSound_
{
	/* MdF data */
	unsigned int	ui_MdFVersion;
	unsigned int	ui_MdfFlag;
	/**/
	ULONG			ul_FileKey;
	ULONG			ui_SndFlags;
	int				i_SndIdx;
	float			f_LoadingDistance;

	/* Sound manager */
	char			asz_Reserved[MDF_Cte_SndLS_ReserveSize];
} MDF_tdst_LoadingSound;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- sound modifier -------------------------------------------------------------------------------------------------*/

void	MDF_LoadingSound_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data);
void	MDF_LoadingSound_Destroy(MDF_tdst_Modifier *_pst_Mod);
void	MDF_LoadingSound_Apply(MDF_tdst_Modifier *_pst_Mod, struct GEO_tdst_Object_ *_pst_Obj);
void	MDF_LoadingSound_Unapply(MDF_tdst_Modifier *_pst_Mod, struct GEO_tdst_Object_ *_pst_Obj);
void	MDF_LoadingSound_Reinit(MDF_tdst_Modifier *_pst_Mod);
ULONG	MDF_LoadingSound_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);
#ifdef ACTIVE_EDITORS
void	MDF_LoadingSound_Save(MDF_tdst_Modifier *_pst_Mod);
int		MDF_LoadingSound_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src);
#endif

/*$2------------------------------------------------------------------------------------------------------------------*/

void	MDF_LoadingSoundReleaseCallback(MDF_tdst_LoadingSound *pst_MS);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __MDFmodifier_SOUNDLOADING_h__ */
