/*$T SNDmodifier.h GC 1.138 09/15/04 10:27:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDmodifier_h__
#define __SNDmodifier_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#ifdef PSX2_TARGET
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/* file version */
#define SND_Cte_SModifierFileVersion	0x00000003

/* chunck id == smd type */
#define SND_Cte_SModifierExtPlayer		0
#define SND_Cte_SModifierModelNumber	1

typedef struct	SND_tdst_SModifierChunk_
{
	int				i_SModifierId;
	unsigned int	ui_DataSize;
} SND_tdst_SModifierChunk;

/*$2- interface ------------------------------------------------------------------------------------------------------*/

typedef struct	SND_tdst_SModifierInterface_
{
	int id;
	void (*pfv_Update) (struct SND_tdst_SModifier_ *, struct SND_tdst_SoundInstance_ *);
	void (*pfv_Destroy) (struct SND_tdst_SModifier_ *, void *, int, BOOL);
	void (*pfv_ApplyOnSound) (struct SND_tdst_SModifier_ *, void *, int, struct SND_tdst_OneSound_ *);
	void (*pfv_ApplyOnInstance) (struct SND_tdst_SModifier_ *, void *, int, struct SND_tdst_SoundInstance_ *);
	int (*pfi_NotifyInstancePlay) (struct SND_tdst_SModifier_ *, struct SND_tdst_SoundInstance_ *);
	int (*pfi_NotifyInstancePause) (struct SND_tdst_SModifier_ *, struct SND_tdst_SoundInstance_ *);
	int (*pfi_NotifyInstanceStop) (struct SND_tdst_SModifier_ *, struct SND_tdst_SoundInstance_ *);
}
SND_tdst_SModifierInterface;

/*$2------------------------------------------------------------------------------------------------------------------*/

typedef struct	SND_tdst_SModifier_
{
	ULONG						ul_Flags;
	ULONG						ul_UserCount;
	ULONG						ul_FileKey;

	SND_tdst_SModifierInterface *i;
	struct SND_tdst_SModifier_	*pst_NextSModifier;
	unsigned int				ui_DataSize;
	void						*pv_Data;
} SND_tdst_SModifier;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    extended player
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	SND_tdst_PlayListDesc_
{
	unsigned short	us_Size;
	unsigned short	us_Flags;
	unsigned short	us_Idx;
	unsigned short	us_EndHitten;
	BIG_KEY			*aul_KeyList;
	ULONG			*aul_FlagList;
	LONG			*al_IdxList;
} SND_tdst_PlayListDesc;

typedef struct	SND_tdst_InsertState_
{
	BOOL						b_IsActive;
	float						f_CurrX;
	float						f_SavY;
	struct OBJ_tdst_GameObject_ *pst_Gao;
} SND_tdst_InsertState;

typedef struct	SND_tdst_InsertListDesc_
{
	unsigned short			us_Size;
	unsigned short			us_Flags;
	BIG_KEY					*aul_KeyList;
	struct SND_tdst_Insert_ **app_PtrList;
	SND_tdst_InsertState	*ast_State;
} SND_tdst_InsertListDesc;

typedef struct	SND_tdst_SModifierExtPlayerDyn_
{
	float			f_FadeInDate;
	float			f_FadeOutDate;
	float			f_DryVol_Curr;
	unsigned int	ui_Freq_Curr;
} SND_tdst_SModifierExtPlayerDyn;

/* ui_Version */
#define SND_Cte_SModifierExtPlayerVersion		0x00000004
#define SND_Cte_SModifierExtPlayerReservedSize	72

/* ui_PlayerFlag */
#define SND_Cte_SModifierStateMask	0x0000000F
#define SND_Cte_SModifierInitState	0x00000000
#define SND_Cte_SModifierFadingIn	0x00000001
#define SND_Cte_SModifierPlaying	0x00000002
#define SND_Cte_SModifierFadingOut	0x00000003
#define SND_Cte_SModifierPause		0x00000004
#define SND_M_SModifierGetState(p)	((p)->ui_PlayerFlag & SND_Cte_SModifierStateMask)
#define SND_M_SModifierSetState(p, s) \
	do \
	{ \
		(p)->ui_PlayerFlag &= ~SND_Cte_SModifierStateMask; \
		(p)->ui_PlayerFlag |= (SND_Cte_SModifierStateMask & (s)); \
	} while(0);

/**/
#define SND_Cte_SModifierGroupMask		0x00000070
#define SND_Cte_SModifierGroupMusic		0x00000010
#define SND_Cte_SModifierGroupSFX		0x00000020
#define SND_Cte_SModifierGroupAmbience	0x00000030
#define SND_Cte_SModifierGroupDialog	0x00000040
#define SND_Cte_SModifierGroupCutScene	0x00000050
#define SND_Cte_SModifierGroupInterface 0x00000060
#define SND_M_SModifierGetGroup(p)		((p)->ui_PlayerFlag & SND_Cte_SModifierGroupMask)
#define SND_M_SModifierSetGroup(p, g) \
	do \
	{ \
		(p)->ui_PlayerFlag &= ~SND_Cte_SModifierGroupMask; \
		(p)->ui_PlayerFlag |= (SND_Cte_SModifierGroupMask & (g)); \
	} while(0);
#define SND_M_SModifierSetGroup_st(p, g) \
	do \
	{ \
		(p).ui_PlayerFlag &= ~SND_Cte_SModifierGroupMask; \
		(p).ui_PlayerFlag |= (SND_Cte_SModifierGroupMask & (g)); \
	} while(0);
/**/
#define SND_Cte_SModifierFlagMask		0xFFFFFF80
#define SND_Cte_SModifierRandomPlayList 0x80000000
#define SND_Cte_SModifierGrpA			0x40000000
#define SND_Cte_SModifierGrpB			0x20000000
#define SND_Cte_SModifierFxA			0x10000000
#define SND_Cte_SModifierFxB			0x08000000
#define SND_Cte_SModifierStopOnLastHit	0x04000000

typedef struct	SND_tdst_SModifierExtPlayer_
{
	/* SModifier header */
	BIG_KEY							ul_SndKey;
	ULONG							ul_SndFlags;
	int								i_SndIndex;

	/* version ID */
	unsigned int					ui_Version;
	BIG_KEY							ul_FileKey;

	/* sound/instance data */
	unsigned int					ui_SndExtFlags;
	struct	SND_tdst_SModifierExtPlayer_*p_Template;

	/* player data */
	unsigned int					ui_PlayerFlag;

	/* volumes */
	float							f_DryVol;
	float							f_DryVol_FactMin;
	float							f_DryVol_FactMax;
	float							f_FxVolLeft;

	/* fade in/out */
	ULONG							ul_FadeInKey;
	struct SND_tdst_Insert_			*pst_FadeIn;
	ULONG							ul_FadeOutKey;
	struct SND_tdst_Insert_			*pst_FadeOut;

	/* pan */
	int								i_Pan;
	int								i_Span;
	float							f_MinPan;

	/* frequency */
    float                           f_FreqCoef;
	float							f_Freq_FactMin;
	float							f_Freq_FactMax;
	float							f_Doppler;

	/* 3D settings */
	float							af_Near[3];
	float							af_Far[3];
	float							af_MiddleBlend[3];
	float							f_FarCoeff;
	float							f_MiddleCoeff;
	float							f_CylinderHeight;

	/* reserve */
	float							f_FxVolRight;
	char							ac_Reserved[SND_Cte_SModifierExtPlayerReservedSize];

	/* play-list */
	SND_tdst_PlayListDesc			st_PlayList;

	/* insert */
	SND_tdst_InsertListDesc			st_InsertList;

	/* unsaved data */
	SND_tdst_SModifierExtPlayerDyn	st_Dyn;
} SND_tdst_SModifierExtPlayer;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern SND_tdst_SModifier	SND_gast_SModifierModel[SND_Cte_SModifierModelNumber];

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void				SND_SModifierInitModule(void);
void				SND_SModifierCloseModule(void);

ULONG				SND_ul_SModifierCallback(ULONG _ul_FilePos);
LONG				SND_l_AddSModifier(BIG_KEY _ul_Key);
void				SND_SModifierDeleteAll(SND_tdst_SModifier **, BOOL);
int				    SND_SModifierChangeSoundIndex(struct SND_tdst_SModifier_ *_pst_SMd);

SND_tdst_SModifier	*SND_pst_SModifierGet(SND_tdst_SModifier *, int _i_Id, int _i_Idx);
void				SND_SModifierDeleteOneSModifier(SND_tdst_SModifier **, int, int, BOOL);

void				SND_SModifierInstanciate(struct SND_tdst_SModifier_ *, struct SND_tdst_SoundInstance_ *);
void				SND_SModifierUpdateInstance(struct SND_tdst_SoundInstance_ *);
void				SND_SModifierAddInInstance(LONG _l_Index, int _i_ExtId, void *, int _i_Size);

int					SND_i_SModifierNotifyPlayInstance(struct SND_tdst_SoundInstance_ *);
int					SND_i_SModifierNotifyStopInstance(struct SND_tdst_SoundInstance_ *);
int					SND_i_SModifierNotifyPauseInstance(struct SND_tdst_SoundInstance_ *);

#ifdef ACTIVE_EDITORS
void				SND_SModifierSaveFromBuffer(ULONG, struct SND_tdst_SModifier_ *);
void				SND_SModifierCreateNewFile(ULONG, void *);
#endif
int					SND_i_SModifierActiveInsert(struct SND_tdst_SoundInstance_ *, int, BOOL, BOOL);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET
#pragma pack(1)
#else
#pragma pack(pop, 1)
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDext_h__ */
