/*$T MDFmodifier_GEN.h GC 1.138 03/01/04 18:11:04 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFmodifier_GEN_h__
#define __MDFmodifier_GEN_h__

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
    version
 -----------------------------------------------------------------------------------------------------------------------
 */

#define SND_Cte_MdFVersion	0x00000112	/* sound modifier version */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MdF flags/status
 -----------------------------------------------------------------------------------------------------------------------
 */

/*$F
#define MDF_Cte_SndActiveWhenCreated		0x00000001
#define MDF_Cte_SndStopAfterFadeOut			0x00000010
#define MDF_Cte_SndReleaseAfterFadeOut		0x00000020
#define MDF_Cte_SndVolumePlaying			0x00000100
#define MDF_Cte_SndPlayOnlyIfTrackIsFree	0x00000400
#define MDF_Cte_SndUseBankFile				0x00000800
#define MDF_Cte_Snd_StaticPrefetch			0x00001000
#define MDF_Cte_Snd_DynamicPrefetch			0x00002000
#define MDF_Cte_Snd_ManualPrefetch			0x00004000
#define MDF_Cte_SndPrefetchDone				0x00008000
#define MDF_Cte_SndEngSynchronized			0x00010000
*/

/* ui_CurrentFlags */
#define MDF_Cte_SndStopped		0x00000001
#define MDF_Cte_SndPlaying		0x00000002
#define MDF_Cte_SndPaused		0x00000004

#define MDF_Cte_SndPrefetched  0x00000008

#define MDF_Cte_SndStateMask	0x00000003
#define MDF_M_SndGetState(a)	((a)->ui_CurrentFlags & MDF_Cte_SndStateMask)
#define MDF_M_SndSetState(a, b) \
	do \
	{ \
		(a)->ui_CurrentFlags &= ~MDF_Cte_SndStateMask; \
		(a)->ui_CurrentFlags |= (b) & MDF_Cte_SndStateMask; \
	} while(0);

/* ui_ConfigFlags */
#define MDF_Cte_SndPlayInVolume			0x00000001
#define MDF_Cte_SndPlayOnInit			0x00000002
#define MDF_Cte_SndPlayLoop				0x00000004
#define MDF_Cte_SndPlayOnTrack			0x00000008
#define MDF_Cte_SndPlayWhenTrackIsFree	0x00000010
#define MDF_Cte_SndPlayRandomPosition	0x00000020
#define MDF_Cte_SndPrefetchStatic		0x00000040
#define MDF_Cte_SndPrefetchAuto        0x00000080
#define MDF_Cte_SndPrefetchManual		0x00000100

/* ui_EdiFlags */
#define MDF_Cte_SndEdiDontDisplay	0x00000001
#define MDF_Cte_SndEdiForceDisplay	0x00000002

/* reserve */
#define MDF_Cte_SndSizeOfReserve	40

/*$4
 ***********************************************************************************************************************
    type
 ***********************************************************************************************************************
 */

typedef struct	GEN_tdst_ModifierSound_
{
	/* MdF data */
	unsigned int	ui_Version;
	unsigned int	ui_EdiFlags;
	unsigned int	ui_Id;

	/* sound file ref */
	unsigned int	ui_FileKey;
	int				i_SndIdx;
	int				i_SndInstance;

	/* prefetch */
	float			f_PrefetchDistance;

	/* activation */
	unsigned int	ui_ConfigFlags;
	unsigned int	ui_CurrentFlags;
	float			f_Delay;
	float			f_DeltaFar;
	int				i_SndTrack;

	/* reserved */
	char			asz_Reserve[MDF_Cte_SndSizeOfReserve];
} GEN_tdst_ModifierSound;

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    generic
 -----------------------------------------------------------------------------------------------------------------------
 */

void	MDF_ApplyGen(struct OBJ_tdst_GameObject_ *_pst_GO);
void	MDF_UnApplyGen(struct OBJ_tdst_GameObject_ *_pst_GO);
void	MDF_MainApplyGen(TAB_tdst_PFtable *_pst_Table);
void	MDF_MainUnApplyGen(TAB_tdst_PFtable *_pst_Table);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    sound modifier
 -----------------------------------------------------------------------------------------------------------------------
 */

void	GEN_ModifierSound_Create(struct OBJ_tdst_GameObject_ *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data);
void	GEN_ModifierSound_Destroy(MDF_tdst_Modifier *_pst_Mod);
void	GEN_ModifierSound_Apply(MDF_tdst_Modifier *_pst_Mod, struct GEO_tdst_Object_ *_pst_Obj);
void	GEN_ModifierSound_Unapply(MDF_tdst_Modifier *_pst_Mod, struct GEO_tdst_Object_ *_pst_Obj);
void	GEN_ModifierSound_Reinit(MDF_tdst_Modifier *_pst_Mod);
ULONG	GEN_ModifierSound_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer);
#ifdef ACTIVE_EDITORS
void	GEN_ModifierSound_Save(MDF_tdst_Modifier *_pst_Mod);
int     GEN_ModifierSound_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src );
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    specific for Sound MdF
 -----------------------------------------------------------------------------------------------------------------------
 */

void	SND_PlayMdF(GEN_tdst_ModifierSound *pst_SndMdF);
void	SND_StopMdF(GEN_tdst_ModifierSound *pst_SndMdF);
void	SND_PauseMdF(GEN_tdst_ModifierSound *pst_SndMdF);
void	SND_PrefetchMdF(GEN_tdst_ModifierSound *pst_SndMdF);
void	SND_FlushMdF(GEN_tdst_ModifierSound *pst_SndMdF);
void	SND_ResolveRefsForMdF(MDF_tdst_Modifier *_pst_Mod);
void	SND_InitMdFModule(void);
void	SND_CloseMdFModule(void);
void	SND_ResolveStaticPrefetchForSpecialMdF(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __MDFmodifier_GEN_h__ */
