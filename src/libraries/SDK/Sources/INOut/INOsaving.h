/*$T INOsaving.h GC 1.138 04/12/05 15:33:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __INOsaving_h__
#define __INOsaving_h__

#include "AIinterp/Sources/AIstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macro / cte
 ***********************************************************************************************************************
 */

#define INO_Cte_SavUniverseMaxSize	(20 * 1024)
#define INO_Cte_SavOneSlotMaxSize	((SFDYN_MAX * sizeof(SCR_tt_SFDyn)) + INO_Cte_SavUniverseMaxSize)
#define INO_Cte_SavSlotNbMax		5
#define INO_Cte_SavHeaderSize		sizeof(INO_tdst_SavSlotDesc)

#if defined(_GAMECUBE)

#define INO_Cte_GC_CRCSize			32	
#define INO_Cte_GC_CommentSize		CARD_COMMENT_SIZE
#define INO_Cte_GC_BannerSize		96 * 32 * 2
#define INO_Cte_GC_IconSize			32 * 32 * 2
#define INO_Cte_GC_SystemSize		(INO_Cte_GC_CRCSize + INO_Cte_GC_CommentSize + INO_Cte_GC_BannerSize + INO_Cte_GC_IconSize)
#define INO_Cte_SavBufferSize		(INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize + INO_Cte_GC_SystemSize)

#define GC_USE_ENCRYPTION
#ifdef GC_USE_ENCRYPTION
#define INO_Cte_SavFileSize			(INO_Cte_SavBufferSize + ENCRYPT_ADD_SIZE)
#else // GC_USE_ENCRYPTION
#define INO_Cte_SavFileSize			INO_Cte_SavBufferSize
#endif // GC_USE_ENCRYPTION

#elif defined (_XBOX)
#define INO_Cte_SavFileSize			(INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize)
#else
#define INO_Cte_SavFileSize			(INO_Cte_SavSlotNbMax * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize))
#endif

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	INO_tdst_SavSlotDesc_
{
	char	asz_Name[32];
	ULONG	ul_GameTime;
	int		i_Slot;
	int		i_Map;
	int		i_WP;
	int		i_Time;
	int		i_Progress;
	int		ai_Score[ 40 ];
	int		i_ID;
	int		i_Tick;
	int		i_Jack_Death;
	int		i_Jack_Bullet;
	int		i_Jack_Kill;
	int		i_Kong_Death;
	int		i_Kong_Time;
	int		i_Kong_Kill;
	int		i_Dummy[2];
#ifdef _XBOX
	int		i_Language;
#endif
} INO_tdst_SavSlotDesc;

typedef struct	INO_tdst_SavManager_
{
	/* low level interface */
	void (*pfv_ModuleInit) (void);
	void (*pfv_ModuleClose) (void);
	void (*pfv_Update) (void);
	int (*pfi_Seti) (int, int);
	int (*pfi_DbgSeti) (int, int);

	/* temp buffer */
	char					*p_Temp;
	int						i_TempSize;
	ULONG					ul_CRC[12];
	
	/* slot descriptor */
	INO_tdst_SavSlotDesc	ast_SlotDesc[INO_Cte_SavSlotNbMax];
	int						i_CurrentSlot;
	char					ac_CurrSlotBuffer[INO_Cte_SavOneSlotMaxSize];
	char					ac_SaveSlotBuffer[INO_Cte_SavOneSlotMaxSize];
	INO_tdst_SavSlotDesc	ast_SaveSlotDesc[INO_Cte_SavSlotNbMax];
	/**/
} INO_tdst_SavManager;

typedef enum	INO_tden_SavUserMessage_
{
	INO_e_SavMsg_None					= 0x10000000,
	INO_e_SavMsg_Accessing				= 0x10000010,
	INO_e_SavMsg_NoCard					= 0x10000020,
	INO_e_SavMsg_UnformattedCard		= 0x10000030,
	INO_e_SavMsg_NoSaving				= 0x10000040,
	INO_e_SavMsg_NoMoreSpace			= 0x10000050,
	INO_e_SavMsg_ErrorWhileSaving		= 0x10000060,
	INO_e_SavMsg_ErrorWhileLoading		= 0x10000070,
	INO_e_SavMsg_ErrorWhileFormating	= 0x10000080,
	INO_e_SavMsg_SuccessWhileSaving		= 0x10000090,
	INO_e_SavMsg_SuccessWhileLoading	= 0x100000A0,
	INO_e_SavMsg_SuccessWhileFormating	= 0x100000B0,
	INO_e_SavMsg_Retrying				= 0x100000C0,
	INO_e_SavMsg_Formating				= 0x100000D0,
	INO_e_SavMsg_FormatConfirmation		= 0x100000E0,
	INO_e_SavMsg_CancelSave				= 0x100000F0,
	INO_e_SavMsg_WrongFormatCard		= 0x10000100,
	INO_e_SavMsg_WrongDevice			= 0x10000110,
	INO_e_SavMsg_NotFunctionnalCard		= 0x10000120,
	INO_e_SavMsg_BeforeAccessing		= 0x10000130, 
	INO_e_SavMsg_ConfirmSave			= 0x10000140,
	//INO_e_SavMsg_AskSelectNewCard		= 0x10000150, (unused)
	//INO_e_SavMsg_AskManageCard		= 0x10000160, (unused)
	INO_e_SavMsg_ConfirmOverwriting		= 0x10000170,
	INO_e_SavMsg_BadFileOnMc			= 0x10000180,
	INO_e_SavMsg_Saving					= 0x10000190,
	INO_e_SavMsg_Loading				= 0x100001A0,
	INO_e_SavMsg_SuccessSavingFailIcon	= 0x10000500,
	INO_e_SavMsg_Dummy					= 0xFFFFFFFF
} INO_tden_SavUserMessage;

typedef enum	INO_tden_SavRequestId_
{
	INO_e_SavRq_SaveSaveBuffer			= 1,
	INO_e_SavRq_RestoreSaveBuffer		= 2,
	INO_e_SavRq_CardId					= 0x20000000,
	INO_e_SavRq_Space					= 0x20000010,
	INO_e_SavRq_Status					= 0x20000020,
	INO_e_SavRq_UserMessageId			= 0x20000030,
	INO_e_SavRq_ReadAllHeaders			= 0x20000040,
	INO_e_SavRq_ReadOneProfile			= 0x20000050,
	INO_e_SavRq_WriteOneProfile			= 0x20000060,
	INO_e_SavRq_ClearMessageId			= 0x20000070,
	INO_e_SavRq_FreeRessource			= 0x20000080,
	INO_e_SavRq_BootupTest				= 0x20000090,
	INO_e_SavRq_TestMemCard				= 0x200000A0,
	INO_e_SavRq_UserAction				= 0x200000B0,
	INO_e_SavRq_SetRetryFlag			= 0x200000C0,
	INO_e_SavRq_SetDeleteFlag			= 0x200000D0,
	INO_e_SavRq_SetAsProfileOwner		= 0x30000000,
	INO_e_SavRq_Dummy					= 0xFFFFFFFF
} INO_tden_SavRequestId;

typedef enum	INO_tden_SavDbgRequestId_
{
	INO_e_SavDbgRq_NoCardInserted		= 0x30000000,
	INO_e_SavDbgRq_NoSpace				= 0x30000010,
	INO_e_SavDbgRq_NoSave				= 0x30000020,
	INO_e_SavDbgRq_Unformatted			= 0x30000030,
	INO_e_SavDbgRq_AccessError			= 0x30000040,
	INO_e_SavDbgRq_ErrorWhileFormating	= 0x30000050,
	INO_e_SavDbgRq_Dummy				= 0xFFFFFFFF
} INO_tden_SavDbgRequestId;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern INO_tdst_SavManager	INO_gst_SavManager;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

void	INO_SavInitModule(void);
void	INO_SavCloseModule(void);

/*$2------------------------------------------------------------------------------------------------------------------*/

void	INO_Save_Update(void);
void	INO_SavRequestSaving(void);
void	INO_SavRequestLoading(void);
int		INO_i_SavRequest(int _i_RequestId, int _i_Value);
int		INO_i_SavGeti(int _i_RequestId);
int		INO_i_SavDbgRequest(int _i_RequestId, int _i_Value);
int		INO_i_Sav_SlotIsEmpty( INO_tdst_SavSlotDesc *);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __INOsaving_h__ */
