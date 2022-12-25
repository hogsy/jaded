/*$T SONutil.h GC 1.138 12/08/03 14:49:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SONutil_h__
#define __SONutil_h__

#define ESON_Cte_AllSoundKey	0x80000000

/*$4
 ***********************************************************************************************************************
    CPP prototypes
 ***********************************************************************************************************************
 */

/*$2- SModifier tools ------------------------------------------------------------------------------------------------*/

void	ESON_OrderMdF(struct MDF_tdst_Modifier_ *);

/*$4
 ***********************************************************************************************************************
    C prototypes
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS

/*$2------------------------------------------------------------------------------------------------------------------*/

void	ESON_InitUtilModule(void);
void	ESON_CloseUtilModule(void);

/*$2- mute -----------------------------------------------------------------------------------------------------------*/

void	ESON_NotifyGaoIsDeleted(OBJ_tdst_GameObject *);
void	ESON_NotifyInstanceIsDeleted(ULONG);

BOOL	ESON_b_MuteIsActive(void);
BOOL	ESON_b_SoloIsActive(void);
BOOL	ESON_b_MuteInstIsActive(void);
BOOL	ESON_b_SoloInstIsActive(void);

void	ESON_SetSoloGao(OBJ_tdst_GameObject *);
void	ESON_SetMuteGao(OBJ_tdst_GameObject *);
void	ESON_SetSoloInstance(ULONG);
void	ESON_SetMuteInstance(ULONG);

void	ESON_SoloOff(void);
void	ESON_MuteOff(void);

BOOL	ESON_b_GaoIsMuted(OBJ_tdst_GameObject *);
BOOL	ESON_b_GaoIsSolo(OBJ_tdst_GameObject *);
BOOL	ESON_b_InstanceIsMuted(ULONG);
BOOL	ESON_b_InstanceIsSolo(ULONG);

/*$2- variables ------------------------------------------------------------------------------------------------------*/

extern BOOL SND_gb_EdiForceKill;	/* F5, F6, F10... manager */
extern BOOL SND_gb_EdiPause;

/*$2- spy tools ------------------------------------------------------------------------------------------------------*/

void	ESON_ResetSpy(void);
void	ESON_SetSpy(ULONG _ul_Key);

/*$2- pause all sounds F5 toggles/F10 steps --------------------------------------------------------------------------*/

void	ESON_PauseAll(BOOL);
void	ESON_KillAllSounds(void);

/*$2- import sound ---------------------------------------------------------------------------------------------------*/

BOOL	ESON_b_ImportWACCallBack(char *, char *, char *);
BOOL	ESON_b_ImportWAVCallBack(char *, char *, char *);
BOOL	ESON_b_ImportWAACallBack(char *, char *, char *);
BOOL	ESON_b_ImportWADCallBack(char *, char *, char *);
BOOL	ESON_b_ImportWAMCallBack(char *, char *, char *);
BOOL	ESON_b_ImportCallBack(char *, char *, char *, int);
void	ESON_LoadAllSounds(void);

/*$2- bank tools -----------------------------------------------------------------------------------------------------*/

void	ESON_BankReport(void);
void	ESON_BankUpdate(ULONG);

/*$2- sound tools ----------------------------------------------------------------------------------------------------*/

void	ESON_SoundReport(void);
void	ESON_SaveSoundReport(void);

/*$2- display in 3D --------------------------------------------------------------------------------------------------*/

void    ESON_OnDisplayFilter(ULONG *);

/*$2------------------------------------------------------------------------------------------------------------------*/

BOOL	ESON_b_IsFileTruncated(ULONG ul_Key);

#define ESON_M_ReturnIfFileIsTruncated(__key, __return) \
	do \
	{ \
		if(ESON_b_IsFileTruncated(__key)) return __return; \
	} while(0);

extern union SND_tdun_Main_ **SND_gap_MainRefList;
extern ULONG				SND_gul_MainRefListSize;

void						ESON_BankRegister(union SND_tdun_Main_ *pMain);
void						ESON_BankDelete(union SND_tdun_Main_ *pMain);

#define ESON_SetLoadedFile(_key) \
	do \
	{ \
		ULONG	ul_Fat; \
		ul_Fat = BIG_ul_SearchKeyToFat((_key)); \
		if(ul_Fat != BIG_C_InvalidIndex) BIG_FileChanged(ul_Fat) |= EDI_FHC_Loaded; \
	} while(0);

void ESON_ReinitOneWorld(void);

#else

/*$2------------------------------------------------------------------------------------------------------------------*/

#define ESON_BankRegister(a)
#define ESON_BankDelete(a)
#define ESON_M_ReturnIfFileIsTruncated(a, b)
#define ESON_SetLoadedFile(_key)
#define ESON_ReinitOneWorld()

#endif

/*$2------------------------------------------------------------------------------------------------------------------*/

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* __SONutil_h__ */
