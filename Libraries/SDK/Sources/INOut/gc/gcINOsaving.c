/*$T gcINOsaving.c GC 1.138 07/23/03 10:41:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/MEMory/MEM.h"
#include "INOut/INOsaving.h"
#include "INOut/INOJoystick.h"
#include "INOut/gc/gcINOsaving.h"
//#include <charPipeline/texPalette.h>
//#include <dvd/DVDBanner.h>
#include "AIinterp/Sources/AIengine.h"
#include "BIGfiles/BIGspecial.h"


/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */
BOOL                        INO_gb_Writing = FALSE;
BOOL						INO_gb_SavingScreen=FALSE;


#define gcINO_Cte_SavFileName	"KINGKONGFILE"
#define gcINO_Cte_SavComment1	"Peter Jackson's King Kong"
#define gcINO_Cte_SavComment2	"King Kong Save Data"
#define gcINO_Cte_SavIconFile	"/KingKingIcon.tpl"
#define gcINO_Cte_SavIconFlag	(gcINO_Cte_SavBannerExist | gcINO_Cte_SavIconNb1)
#define gcINO_Cte_SavBannerFile "/opening.bnr"

void *gc_p_CardWorkArea=NULL;

#define gcINO_Cte_SaveFileInBlocks			55
#define gcINO_Cte_SaveFileInBytes			(gcINO_Cte_SaveFileInBlocks * 8192)
#define gcINO_Cte_SavClutSize				512

#define gcINO_Cte_SavIcon1Speed8			0x00000001
#define gcINO_Cte_SavIcon1Speed12			0x00000002
#define gcINO_Cte_SavIcon2Speed8			0x00000004
#define gcINO_Cte_SavIcon2Speed12			0x00000008
#define gcINO_Cte_SavIcon3Speed8			0x00000010
#define gcINO_Cte_SavIcon3Speed12			0x00000020
#define gcINO_Cte_SavIcon4Speed8			0x00000040
#define gcINO_Cte_SavIcon4Speed12			0x00000080
#define gcINO_Cte_SavIcon5Speed8			0x00000100
#define gcINO_Cte_SavIcon5Speed12			0x00000200
#define gcINO_Cte_SavIcon6Speed8			0x00000400
#define gcINO_Cte_SavIcon6Speed12			0x00000800
#define gcINO_Cte_SavIcon7Speed8			0x00001000
#define gcINO_Cte_SavIcon7Speed12			0x00002000
#define gcINO_Cte_SavIcon8Speed8			0x00004000
#define gcINO_Cte_SavIcon8Speed12			0x00008000

#define gcINO_Cte_SavIcon1IsC8				0x00010000
#define gcINO_Cte_SavIcon2IsC8				0x00020000
#define gcINO_Cte_SavIcon3IsC8				0x00040000
#define gcINO_Cte_SavIcon4IsC8				0x00080000
#define gcINO_Cte_SavIcon5IsC8				0x00100000
#define gcINO_Cte_SavIcon6IsC8				0x00200000
#define gcINO_Cte_SavIcon7IsC8				0x00400000
#define gcINO_Cte_SavIcon8IsC8				0x00800000

#define gcINO_Cte_SavIconAnimLoop			0x01000000
#define gcINO_Cte_SavBannerExist			0x02000000
#define gcINO_Cte_SavBannerIsC8				0x04000000

#define gcINO_Cte_SavIconNb1				0x10000000
#define gcINO_Cte_SavIconNb2				0x20000000
#define gcINO_Cte_SavIconNb3				0x30000000
#define gcINO_Cte_SavIconNb4				0x40000000
#define gcINO_Cte_SavIconNb5				0x50000000
#define gcINO_Cte_SavIconNb6				0x60000000
#define gcINO_Cte_SavIconNb7				0x70000000
#define gcINO_Cte_SavIconNb8				0x80000000

#define gcINO_M_SavIconIsC8(_icon, _flag)	(((_flag) & (1 << (16 + (_icon)))) != 0)
#define gcINO_M_SavIconGetNb(_flag)			(((_flag) & 0xF0000000) >> 28)

#define RoundUp(x,RoundUpValue) (((u32)(x) + (RoundUpValue) - 1) & ~((RoundUpValue) - 1))
#define RoundDown(x,RoundDownValue) ((u32)(x)  & ~((RoundDownValue) - 1))

// Internal state of the state machine
typedef enum	gcINO_tden_SavManagerState_
{
	SavState_e_Inactive = 0,
	SavState_e_BeforeAccessing,
	SavState_e_Connect,
	SavState_e_Mount,
	SavState_e_Mounting,
	SavState_e_Check,
	SavState_e_Checking,
	SavState_e_GetFreeBlocks,
	SavState_e_Open,
	SavState_e_Create,
	SavState_e_Creating,
	SavState_e_SetStatus,
	SavState_e_Read,
	SavState_e_Reading,
	SavState_e_Write,
	SavState_e_Writing,
	SavState_e_Delete,
	SavState_e_Deleting,
	SavState_e_Close,
	SavState_e_Unmount,
	SavState_e_TestMemCardWhileWaitingUser,
	SavState_e_WaitingUserResponseForFormatFailure,
	SavState_e_WaitingUserResponseForFormatSuccess,
	SavState_e_WaitingUserResponseForUnformatted,
	SavState_e_WaitingUserResponseForCancelSave,
	SavState_e_WaitingUserResponseForWrongFormatMc,
	SavState_e_WaitingUserResponseForFormat,
	SavState_e_Formating_1,
	SavState_e_Formating_2,
	SavState_e_WaitingUserResponseForMcAbsent,
	SavState_e_WaitingUserResponseForNoSpace,
	SavState_e_WaitingUserResponseForNoSaving,
	SavState_e_WaitingUserResponseForSavingFailure,
	SavState_e_WaitingUserResponseForWrongDevice,
	SavState_e_WaitingUserResponseForBrokenDevice,
	SavState_e_WaitingUserResponseForNotFuncionnalMc,
	SavState_e_WaitingUserResponseForLoadingFailure,
	SavState_e_WaitingUserResponseForOverwriting,
	SavState_e_WaitingUserResponseForBadFileOnMc,
	SavState_e_WaitingUserResponseForLoadingSuccess,
	SavState_e_WaitingUserResponseForSavingSuccess,
	SavState_e_WaitingUserResponseForDeletingSuccess,
	/*SavState_e_Connect,
	SavState_e_TestingConnected,
	SavState_e_TestFormated,
	SavState_e_Formating_1,
	SavState_e_Formating_2,
	SavState_e_TestData,
	SavState_e_WaitingOverwriteConfirmation,
	SavState_e_WaitingSaveConfirmation,
	SavState_e_GoingToRoot,
	SavState_e_GoingToHome,
	SavState_e_MakeTree,
	SavState_e_OpeningIconFile,
	SavState_e_CreateIconFile,
	SavState_e_ClosingIconFile,
	SavState_e_OpeningIconSysFile,
	SavState_e_CreateIconSysFile,
	SavState_e_ClosingIconSysFile,
	SavState_e_OpenMainFile,
	SavState_e_WritingMainFile,
	SavState_e_ReadingMainFile,
	SavState_e_ClosingMainFile,
	SavState_e_MakingDir,
	SavState_e_GoingToHome2,
	SavState_e_CreatingIconSys,
	SavState_e_WritingIconSys,
	SavState_e_ClosingIconSys,
	SavState_e_CreatingIcon,
	SavState_e_WritingIcon,
	SavState_e_ClosingIcon,
	SavState_e_OpeningMainFile,
	SavState_e_WaitingUserResponseForMcAbsent,
	SavState_e_WaitingUserResponseForNoSpace,
	SavState_e_WaitingUserResponseForUnformatted,
	SavState_e_WaitingUserResponseForFormat,
	SavState_e_WaitingUserResponseForFormatFailure,
	SavState_e_WaitingUserResponseForFormatSuccess,
	SavState_e_WaitingUserResponseForSavingFailure,
	SavState_e_WaitingUserResponseForSavingSuccess,
	SavState_e_WaitingUserResponseForCancelSave,
	SavState_e_WaitingUserResponseForLoadingFailure,
	SavState_e_WaitingUserResponseForLoadingSuccess,
	SavState_e_WaitingUserResponseForNoSaving,*/
	SavState_e_Dummy								= 0xFFFFFFFF
} gcINO_tden_SavManagerState;

// Command delivered from high level API to here
typedef enum	gcINO_tden_SavCommand_
{
	SavCmd_e_None									= 0,
	SavCmd_e_ReadAllHeaders,
	SavCmd_e_Read,
	SavCmd_e_Write,
	SavCmd_e_Delete,
	SavCmd_e_BootupTest,
	SavCmd_e_TestMemCard,
	SavCmd_e_Dummy									= 0xFFFFFFFF
} gcINO_tden_SavCommand;


// Internal data used to manage the card
typedef struct	gcINO_tdst_SavManager_
{
	gcINO_tden_SavCommand		en_UserCommand;
	INO_tden_SavUserMessage		en_UserMessage;
	gcINO_tden_SavManagerState  en_State;
	gcINO_tden_SavManagerState  en_StateAfterClose;
	int							i_ConfirmOnSuccess;
	
	int 						i_UserAction;
	
	char 						s_Filename[32];
	
	/**/
	int							i_CurrentCardId;
	
	/**/
	BOOL						b_IsWorking;
	int							i_TimeOut;
	float						i_TimeTest;
	
	/**/
	BOOL						b_ChangingThreadPrio;
	int							i_CurrPriority;
	
	/**/
	int							i_gcnMcSync_Result;
	int							i_gcnMcxxxx_Result;
	
	/**/
	BOOL						b_CardIsMounted;
	BOOL						b_FileIsOpened;
	
	/**/
	BOOL 						b_TestCRC;
	
	/**/
	int 						i_WriteOffset;
	int 						i_WriteStart;
	int 						i_WriteSize;
	
	int 						i_UsedSlotNb; 	
	
	/**/
	int							i_FreeFiles;
	int							i_FreeBytes;
	int							i_MemSize;
	int 						i_SectorSize;
	
	/**/
	CARDStat					st_CardFileManagerStat;
	CARDFileInfo				st_CardFileManagerInfo;
	
	/**/
	u16							us_Button;
	u16							us_LastButton;
} gcINO_tdst_SavManager;

gcINO_tdst_SavManager	gcINO_gst_SavManager;


// Stucture for one slot (header + data) 
typedef struct	INO_tdst_SavSlot_
{
	INO_tdst_SavSlotDesc 	st_Header;
	u8 						data[INO_Cte_SavOneSlotMaxSize];
} INO_tdst_SavSlot;




// Stucture for the data that is on the memory card 
typedef struct	gcINO_tdst_SavData_
{
	// CRC (used to check data consistency).
	ULONG 	ulCRC;
	u8 		padding[INO_Cte_GC_CRCSize-4];
	
	// System data.
	u8 		sComment1[32];
	u8 		sComment2[32];
	u8 		banner[INO_Cte_GC_BannerSize];
	u8 		icon[INO_Cte_GC_IconSize];
	
	// Slots (Header + slot data)
	INO_tdst_SavSlot st_Slot;
} gcINO_tdst_SavData;

gcINO_tdst_SavData *gcINO_gpst_SavData = NULL;


/*
 =======================================================================================================================
 =======================================================================================================================
 */
// Card Callbacks
 
		
void gcCARDFormatCallback( s32 chan, s32 result )
{	
	ERR_X_Assert(result != CARD_RESULT_BUSY);
	INO_gb_Writing = FALSE;
	gcINO_gst_SavManager.i_gcnMcxxxx_Result = result;
} 

void gcCARDMountDoneCallback( s32 chan, s32 result )
{
	ERR_X_Assert(result != CARD_RESULT_BUSY);
	
	gcINO_gst_SavManager.b_CardIsMounted = TRUE;
	
	INO_gb_Writing = FALSE;
	gcINO_gst_SavManager.i_gcnMcxxxx_Result = result;
}

void gcCARDDetachCallback( s32 chan, s32 result )
{
	ERR_X_Assert(result != CARD_RESULT_BUSY);
	
	gcINO_gst_SavManager.b_CardIsMounted = FALSE;
	
	if (gc_p_CardWorkArea != NULL)
	{
		MEM_Free(gc_p_CardWorkArea);
		gc_p_CardWorkArea = NULL;
	}			
	
	INO_gb_Writing = FALSE;
	gcINO_gst_SavManager.en_State = SavState_e_Unmount;
}

		
void CARDCreatingCallback( s32 chan, s32 result )
{
	ERR_X_Assert(result != CARD_RESULT_BUSY);
	INO_gb_Writing = FALSE;
	gcINO_gst_SavManager.i_gcnMcxxxx_Result = result;
}

void CARDReadCallback( s32 chan, s32 result )
{
	ERR_X_Assert(result != CARD_RESULT_BUSY);
	INO_gb_Writing = FALSE;
	gcINO_gst_SavManager.i_gcnMcxxxx_Result = result;
}

void CARDWriteCallback( s32 chan, s32 result )
{
	ERR_X_Assert(result != CARD_RESULT_BUSY);
	INO_gb_Writing = FALSE;
	gcINO_gst_SavManager.i_gcnMcxxxx_Result = result;
}

void CARDDeleteCallback( s32 chan, s32 result )
{
	ERR_X_Assert(result != CARD_RESULT_BUSY);
	INO_gb_Writing = FALSE;
	gcINO_gst_SavManager.i_gcnMcxxxx_Result = result;
}



/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
 // Error management

#ifndef _FINAL_
void M_UnknownError()
{
	while (1)
	{ 
		char	az[256]; 
		
		GXI_ErrBegin(); 
		
		sprintf(az, "Error in card management\n"); 
		GXI_ErrPrint(az);
		sprintf(az, "\n"); 
		GXI_ErrPrint(az);
		sprintf(az, "UserCommand %d\n",gcINO_gst_SavManager.en_UserCommand); 
		GXI_ErrPrint(az);
		sprintf(az, "UserMessage %d\n",gcINO_gst_SavManager.en_UserMessage); 
		GXI_ErrPrint(az);
		sprintf(az, "State %d\n",gcINO_gst_SavManager.en_State); 
		GXI_ErrPrint(az);
		sprintf(az, "GCNResult %d\n",gcINO_gst_SavManager.i_gcnMcxxxx_Result); 
		GXI_ErrPrint(az);
		
		GXI_ErrEnd(); 
	};
}
#else //_FINAL_
#define M_UnknownError()
#endif // _FINAL_


	
void M_SuccessWhileFormating() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForFormatSuccess; 
	/*gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None; */
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileFormating; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_ErrorWhileFormating() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForFormatFailure; 
	/*gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None;*/ 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_ErrorWhileFormating; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_ErrorNoMc() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForMcAbsent; 
	/*gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None; */
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_NoCard; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_ErrorWrongDevice() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForWrongDevice; 
	/*gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None; */
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_WrongDevice; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 
	
void M_ErrorNotFunctionnalMc() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForNotFuncionnalMc; 
	/*gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None; */
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_NotFunctionnalCard; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_ErrorWrongFormatMc() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForWrongFormatMc; 
	/*gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None; */
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_WrongFormatCard; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

/*void M_ErrorBrokenMc() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForBrokenDevice; 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_BrokenDevice; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} */

void M_TestMC_Error() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_Inactive; 
	gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None; 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_NoCard; 
	gcINO_gst_SavManager.b_IsWorking = FALSE; 
} 

void M_TestMC_Success()
{ 
	gcINO_gst_SavManager.en_State = SavState_e_Inactive; 
	gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None;
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_None; 
	gcINO_gst_SavManager.b_IsWorking = FALSE; 
} 
	
void M_ErrorNoSaveOnMc() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForNoSaving; 
	/*gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None;*/ 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_NoSaving; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_ErrorBadFileOnMc() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForBadFileOnMc; 
	/*gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None;*/ 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_BadFileOnMc; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_ErrorNoMoreSpaceOnMc() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForNoSpace; 
	/*gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None;*/ 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_NoMoreSpace; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_ErrorUnformattedMc() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForUnformatted; 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_UnformattedCard; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_ErrorWhileLoading() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForLoadingFailure; 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_ErrorWhileLoading; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_ErrorWhileSaving() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForSavingFailure; 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_ErrorWhileSaving; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_ConfirmOverwriting() 
{ 
	gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForOverwriting; 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_ConfirmOverwriting; 
	gcINO_gst_SavManager.b_IsWorking = TRUE; 
}

void M_SuccessWhileSaving() 
{ 
	if (gcINO_gst_SavManager.i_ConfirmOnSuccess )
	{
		gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForSavingSuccess; 
		gcINO_gst_SavManager.b_IsWorking = TRUE; 
	}
	else
	{
		gcINO_gst_SavManager.en_State = SavState_e_Inactive; 
			gcINO_gst_SavManager.b_IsWorking = FALSE; 
	}
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileSaving; 
} 

void M_SuccessWhileLoading() 
{ 
	if (gcINO_gst_SavManager.i_ConfirmOnSuccess )
	{
		gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForLoadingSuccess; 
		gcINO_gst_SavManager.b_IsWorking = TRUE; 
	}
	else
	{
		gcINO_gst_SavManager.en_State = SavState_e_Inactive; 
		gcINO_gst_SavManager.b_IsWorking = FALSE; 
	}
	/*gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None;*/ 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileLoading; 
} 


void M_Close()
{
	do 
	{
		gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARDClose(&gcINO_gst_SavManager.st_CardFileManagerInfo);
		switch (gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		case CARD_RESULT_BUSY:
			break;
		case CARD_RESULT_NOCARD:
			gcINO_gst_SavManager.b_FileIsOpened = FALSE;
			M_ErrorNoMc();
			break;
		case CARD_RESULT_READY:
			gcINO_gst_SavManager.b_FileIsOpened = FALSE;
			gcINO_gst_SavManager.en_State = gcINO_gst_SavManager.en_StateAfterClose;
			break;
		default:
			M_UnknownError();
		}
	} while (gcINO_gst_SavManager.i_gcnMcxxxx_Result == CARD_RESULT_BUSY);
}						

void M_Unmount()
{
	do 
	{
		gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARDUnmount(gcINO_gst_SavManager.i_CurrentCardId);
		
		switch (gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		case CARD_RESULT_BUSY:
			break;
			
			// Cas normal 
		case CARD_RESULT_READY:
			ERR_X_Assert(!gcINO_gst_SavManager.b_FileIsOpened);
			
			switch(gcINO_gst_SavManager.en_UserCommand)
			{
			case SavCmd_e_Write:
			case SavCmd_e_Delete:
				M_SuccessWhileSaving();
				break;
			case SavCmd_e_ReadAllHeaders:
			case SavCmd_e_Read:
				M_SuccessWhileLoading();
				break;
			default:
				break;
			}				
			break;
			
		case CARD_RESULT_NOCARD:
			// Soit la carte mémoire a été arrachée, soit on a déjà fait l'unmount -> on doit distinguer les deux cas.
			if (!M_TestMemCardWhileWaitingUser())
			{			
				// La carte est bien là -> on sort en douceur
				gcINO_gst_SavManager.en_State = SavState_e_Inactive;
			}
			gcINO_gst_SavManager.b_FileIsOpened = FALSE;
				
			break;
			
		default:
			M_UnknownError();
		}
	} while (gcINO_gst_SavManager.i_gcnMcxxxx_Result == CARD_RESULT_BUSY);
	
	// Si on est ici, c'est qu'on a unmounté la carte.
	gcINO_gst_SavManager.b_CardIsMounted = FALSE;
	
	// Libération des ressources.
	gcINO_gpst_SavData = NULL;
	if (gc_p_CardWorkArea != NULL)
	{
		MEM_Free(gc_p_CardWorkArea);
		gc_p_CardWorkArea = NULL;
	}		
		
} 
 
void M_ResetAll() 
{ 
	if (gcINO_gst_SavManager.b_FileIsOpened)
		M_Close();
		
	if (gcINO_gst_SavManager.b_CardIsMounted)
		M_Unmount();
		
	gcINO_gst_SavManager.en_State = SavState_e_Inactive; 
	gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None; 
	gcINO_gst_SavManager.b_IsWorking = FALSE; 
	gcINO_gpst_SavData = NULL;
} 
	
void M_Retrying() 
{ 
	if (gcINO_gst_SavManager.b_FileIsOpened)
		M_Close();
		
	if (gcINO_gst_SavManager.b_CardIsMounted)
		M_Unmount();
		
	gcINO_gst_SavManager.en_State = SavState_e_Inactive; 
	gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Retrying; 
	gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None; 
	gcINO_gst_SavManager.b_IsWorking = FALSE; 
} 



void M_TestBusy()
{
	int result = CARDProbeEx(
		gcINO_gst_SavManager.i_CurrentCardId,
		NULL,
		NULL
	);
	
	switch(result)
	{
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_WRONGDEVICE:
			M_ErrorWrongDevice();
			break;
		case CARD_RESULT_READY:
			gcINO_gst_SavManager.i_TimeOut++;
		case CARD_RESULT_BUSY:
		default:
			break;
	}
}
	
BOOL M_TestMemCardWhileWaitingUser() 
{
	gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARDProbeEx(
		gcINO_gst_SavManager.i_CurrentCardId,
		NULL,
		NULL
	);
	switch(gcINO_gst_SavManager.i_gcnMcxxxx_Result)
	{
		case CARD_RESULT_READY:
		case CARD_RESULT_BUSY:
			return FALSE;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			return TRUE;
		case CARD_RESULT_WRONGDEVICE:
			M_ErrorWrongDevice();
			return TRUE;
	}
	return TRUE;
}

BOOL M_TestMemCardWhileNoCard()
{
	gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARDProbeEx(
		gcINO_gst_SavManager.i_CurrentCardId,
		NULL,
		NULL
	);
	switch(gcINO_gst_SavManager.i_gcnMcxxxx_Result)
	{
		case CARD_RESULT_READY:
		case CARD_RESULT_BUSY:
			M_Retrying();
			return TRUE;
		case CARD_RESULT_NOCARD:
			return FALSE;
		case CARD_RESULT_WRONGDEVICE:
			return FALSE;
	}
	return FALSE;
}




// Utiliser PAD_BUTTON_LEFT, PAD_BUTTON_A, PAD_TRIGGER_Z, etc ...
#define M_bButtonJustPressed( _i_button ) 	((gcINO_gst_SavManager.us_Button & _i_button) && !(gcINO_gst_SavManager.us_LastButton & _i_button))
#define M_bButtonPressed( _i_button ) 	(gcINO_gst_SavManager.us_Button & _i_button)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
// Icon and banner reading
	
	
	
// BEGIN INCLUDE NINTENDO LIB	


	
/*---------------------------------------------------------------------------*
  Project:  character pipeline
  File:     texPalette.c

  Copyright 1998, 1999, 2000 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Dolphin/build/charPipeline/texPalette/src/texPalette.c $
    
    6     02/04/03 17:09 Hirose
    Fixed misimplementation of TexFreeFunc.
    
    5     3/22/01 3:27p John
    Removed geoPalette dependency.
    Added extern variables from fileCache.
    
    4     7/07/00 7:09p Dante
    PC Compatibility
    
    3     3/23/00 9:56a Ryan
    update for Character Pipeline cleanup and function prefixing
    
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//#include "DVDBanner.h" 
// Game banner file must be placed in the root directory
#define DVD_BANNER_FILENAME         "opening.bnr"

#define DVD_BANNER_WIDTH            96
#define DVD_BANNER_HEIGHT           32

//
// "opening.bnr" file format for JP/US console
//
typedef struct gcDVDBanner
{
    u32 id;                 // 'BNR1'
    u32 padding[7];
    u8  image[2 * DVD_BANNER_WIDTH * DVD_BANNER_HEIGHT];    // RGB5A3 96x32 texture image
    u8  shortTitle[32];     // Short game title shown in IPL menu
    u8  shortMaker[32];     // Short developer, publisher names shown in IPL menu
    u8  longTitle[64];      // Long game title shown in IPL game start screen
    u8  longMaker[64];      // Long developer, publisher names shown in IPL game start screen
    u8  comment[128];       // Game description shown in IPL game start screen in two lines.
                            // Comment can include a '\n'.
} gcDVDBanner;

typedef struct gcDVDBannerComment
{
    u8  shortTitle[32];     // Short game title shown in IPL menu
    u8  shortMaker[32];     // Short developer, publisher names shown in IPL menu
    u8  longTitle[64];      // Long game title shown in IPL game start screen
    u8  longMaker[64];      // Long developer, publisher names shown in IPL game start screen
    u8  comment[128];       // Game description shown in IPL game start screen in two lines.
                            // Comment can include a '\n'.
} gcDVDBannerComment;

//
// "opening.bnr" file format for EU console
//
typedef struct cgDVDBanner2
{
    u32 id;                 // 'BNR2'
    u32 padding[7];
    u8  image[2 * DVD_BANNER_WIDTH * DVD_BANNER_HEIGHT];    // RGB5A3 96x32 texture image
    gcDVDBannerComment comment[6];    // Comments in six languages
} gcDVDBanner2;

// DVDBanner.id
#define DVD_BANNER_ID               'BNR1'  // US/JP
#define DVD_BANNER_ID2              'BNR2'  // EU 
 
//#include <charPipeline/texPalette.h>

typedef struct
{
    u16             numEntries;
    u8              unpacked;
    u8              pad8;

    GXTlutFmt       format;
    Ptr             data;

}gcCLUTHeader, *gcCLUTHeaderPtr;

/********************************/
typedef struct
{
    u16             height;
    u16             width;

    u32             format;
    Ptr             data;

    GXTexWrapMode   wrapS;
    GXTexWrapMode   wrapT;

    GXTexFilter     minFilter;
    GXTexFilter     magFilter;

    float           LODBias;

    u8              edgeLODEnable;
    u8              minLOD;
    u8              maxLOD;
    u8              unpacked;

}gcTEXHeader, *gcTEXHeaderPtr;

/********************************/
typedef struct
{
    gcTEXHeaderPtr        textureHeader;
    gcCLUTHeaderPtr       CLUTHeader;

}gcTEXDescriptor, *gcTEXDescriptorPtr;

/********************************/
typedef struct
{
    u32                 versionNumber;

    u32                 numDescriptors;
    gcTEXDescriptorPtr    descriptorArray;

}gcTEXPalette, *gcTEXPalettePtr;


//#include <charPipeline/fileCache.h>
/********************************/
#define gcDS_AUTO_PURGE   0
#define gcDS_NO_PURGE 1

/********************************/

typedef struct 
{
	Ptr	Prev;
	Ptr	Next; 

} gcDSLink, *gcDSLinkPtr;

typedef struct 
{
	u32	Offset;
	Ptr	Head;
	Ptr	Tail;

} gcDSList, *gcDSListPtr;

typedef struct
{
    gcDSLink  Link;

    void    (*Free)(Ptr *data);

    char    *Name;

    Ptr Data;


    u16 ReferenceCount;


}gcDSCacheNode, *gcDSCacheNodePtr;

/********************************/
typedef struct
{
    u8  PurgeFlag;

    gcDSList  CacheNodeList;

}gcDSCache, *gcDSCachePtr;



#define	GET_TEXTURE_1			"GetTexture():  Texture Not Found "



/*---------------------------------------------------------------------------*
  Project: [structures]
  File:    [List.c]

  Copyright 1998-2001 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /dolphin/build/charPipeline/structures/src/List.c $
    
    2     3/21/01 2:44p John
    Updated copyright header.

  $NoKeywords: $

 *---------------------------------------------------------------------------*/


/*>*******************************(*)*******************************<*/
/*>-----------------------------------------------------------------<*/
//	Initializes the head and tail of the list to be 0 and sets the 
//	offset of the Link into the obj structure.
/*>-----------------------------------------------------------------<*/
void	gcDSInitList ( gcDSListPtr list, Ptr obj, gcDSLinkPtr link )
{
	list->Head = 0;
	list->Tail = 0;
	list->Offset = ((u32)link - (u32)obj);
}

/*>*******************************(*)*******************************<*/
/*>-----------------------------------------------------------------<*/
//	Inserts an object into the specified list BEFORE the cursor object
/*>-----------------------------------------------------------------<*/
void	gcDSInsertListObject ( gcDSListPtr list, Ptr cursor, Ptr obj )
{
	gcDSLinkPtr link = (gcDSLinkPtr)((u32)obj + list->Offset);
	gcDSLinkPtr linkNext;
	gcDSLinkPtr linkPrev;

	if(list->Head)	// IF THE LIST HAS A HEAD...
	{
		if(!cursor)	// IF THERE WAS NO CURSOR PASSED, INSERT AT THE TAIL.
		{
			linkPrev = (gcDSLinkPtr)(((u32)list->Tail) + list->Offset);
			linkPrev->Next = obj;
			link->Prev = list->Tail;
			link->Next = 0;
			list->Tail = obj;
			return;
		}

		linkNext = (gcDSLinkPtr)((u32)cursor + list->Offset);

		if(cursor == list->Head)	// IF THE CURSOR PASSED IS THE HEAD, MAKE obj THE NEW HEAD.
		{
			list->Head = obj;
			link->Next = (Ptr)cursor;
			linkNext->Prev = obj;
			return;
		}
	
		linkPrev = (gcDSLinkPtr)(((u32)linkNext->Prev) + list->Offset);	// ELSE INSERT obj BEFORE THE 
		link->Next = cursor;										// CURSOR.	
		link->Prev = linkNext->Prev;
		linkNext->Prev = obj;
		linkPrev->Next = obj;
		return;
	}				// ELSE THE LIST IS EMPTY SO MAKE HEAD AND TAIL POINT TO obj.
	
	list->Head = list->Tail = obj;
	link->Next = link->Prev = 0;
}

/*>*******************************(*)*******************************<*/
/*>-----------------------------------------------------------------<*/
//	Removes the specified obj from the list.
/*>-----------------------------------------------------------------<*/
void	gcDSRemoveListObject ( gcDSListPtr list, Ptr obj )
{
	gcDSLinkPtr link = (gcDSLinkPtr)((u32)obj + list->Offset);

	if( !obj )
		return;

	if(link->Prev)														// IF obj HAS A PREVIOUS
		((gcDSLinkPtr)((u32)link->Prev + list->Offset))->Next = link->Next;	// PREV->NEXT = OBJ->NEXT
	else																// ELSE
		list->Head = link->Next;										// LIST->HEAD = OBJ->NEXT		

	if(link->Next)														// IF obj HAS A NEXT
		((gcDSLinkPtr)((u32)link->Next + list->Offset))->Prev = link->Prev;	// NEXT->PREV = OBJ->PREV
	else																// ELSE
		list->Tail = link->Prev;										// LIST->TAIL = OBJ->PREV

	link->Prev = 0;
	link->Next = 0;
}

/*---------------------------------------------------------------------------*
  Name:         DSAttachList

  Description:  Attach a list to another one.

  Arguments:    baseList      List to which a new one will be attached
                attachList    List that will be attached

  Returns:      NONE
 *---------------------------------------------------------------------------*/
void 
gcDSAttachList( gcDSListPtr baseList, gcDSListPtr attachList )
{
	gcDSLinkPtr   link     = NULL;
	gcDSLinkPtr   linkPrev = NULL;


    // TBD add assertions
    if( baseList->Offset != attachList->Offset )
        return;

    // if the list to attach is empty, don't do anything
    if( !attachList->Head && !attachList->Tail )
        return;

    link = (gcDSLinkPtr)((u32)attachList->Head + attachList->Offset);

    // if the list has a head
	if(baseList->Head)	
    {
		linkPrev = (gcDSLinkPtr)(((u32)baseList->Tail) + baseList->Offset);
		linkPrev->Next = attachList->Head;
		link->Prev = baseList->Tail;
		baseList->Tail = attachList->Tail;
	}				
    else // Else the base list is empty, so make it the same a the attach list
    {
	    baseList->Head = attachList->Head;
	    baseList->Tail = attachList->Tail;
    }
} 

/*---------------------------------------------------------------------------*
  Name:         DSNextListObj

  Description:  Returns the next object after obj in the list

  Arguments:    list		List that contains obj (need the offset)
                obj			Find the object after obj

  Returns:      Returns the next object after obj in the list.  Otherwise NULL
 *---------------------------------------------------------------------------*/
void*
gcDSNextListObj( gcDSListPtr list, Ptr obj )
{
	if( !list )
		return NULL;

	// If object is null, return the head of the list
	if( !obj )
	{
		return list->Head;
	}

	return ((gcDSLinkPtr)((u32)obj + list->Offset))->Next;
}


/*---------------------------------------------------------------------------*
  Project:  character pipeline
  File:     fileCache.c

  Copyright 1998, 1999, 2000 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Dolphin/build/charPipeline/fileCache/src/fileCache.c $
    
    3     3/21/01 5:15p John
    Properly initialized PurgeFlag field.
    
    2     3/23/00 9:56a Ryan
    update for Character Pipeline cleanup and function prefixing
    
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/********************************/
//global character pipeline cache variables
gcDSCache	gcDODisplayCache;
u8		gcDOCacheInitialized = 0;

/********************************/
static u8			    gcAllocCacheNode	( gcDSCacheNodePtr *cacheNode, char *name );
static gcDSCacheNodePtr	gcFindCacheNode	( gcDSCachePtr cache, char *name, Ptr data );
static void			    gcFreeCacheNode	( gcDSCacheNodePtr *cacheNode );

/********************************/
gcDSCacheNodePtr	gcDSAddCacheNode ( gcDSCachePtr cache, char *name, Ptr data, Ptr OSFreeFunc )
{
	gcDSCacheNodePtr	cacheNode = 0;

	if(!gcAllocCacheNode(&cacheNode, name)) return 0;

	L_strcpy(cacheNode->Name, name);
	cacheNode->Data = data;
	cacheNode->Free = (void(*)(Ptr*))OSFreeFunc;
	cacheNode->ReferenceCount = 0;

	gcDSInsertListObject((gcDSListPtr)(&cache->CacheNodeList), 0, (Ptr)cacheNode);

	return cacheNode;
}

/********************************/
static u8	gcAllocCacheNode ( gcDSCacheNodePtr *cacheNode, char *name )
{
	if(*cacheNode) gcFreeCacheNode(cacheNode);

	(*cacheNode) = (gcDSCacheNodePtr)MEM_p_AllocAlign(sizeof(gcDSCacheNode),32);
	if(!(*cacheNode)) return 0;



	(*cacheNode)->Name = (char *)MEM_p_AllocAlign(L_strlen(name) + 1,32);

	if(!(*cacheNode)->Name) return 0;

	return 1;
}

/********************************/
void	gcDSEmptyCache ( gcDSCachePtr cache )
{
	gcDSCacheNodePtr cursor;
	gcDSCacheNodePtr cacheNode;

	cursor = (gcDSCacheNodePtr)(cache->CacheNodeList.Head);
	while(cursor)
	{
		cacheNode = cursor;
		cursor = (gcDSCacheNodePtr)(cursor->Link.Next);	

		gcDSRemoveListObject(&cache->CacheNodeList, (Ptr)(cacheNode));
		gcFreeCacheNode(&cacheNode);
	}
}

/********************************/
static gcDSCacheNodePtr	gcFindCacheNode ( gcDSCachePtr cache, char *name, Ptr data )

{
	gcDSCacheNodePtr cacheNode;

	cacheNode = (gcDSCacheNodePtr)(cache->CacheNodeList.Head);

	if(data)
	{
		while(cacheNode)
		{
			if(data == cacheNode->Data)		return cacheNode;
			
			cacheNode = (gcDSCacheNodePtr)(cacheNode->Link.Next);
		}
	}

	else if(name)
	{
		while(cacheNode)
		{
			if(!L_strcmp(name, cacheNode->Name))	return cacheNode;

			cacheNode = (gcDSCacheNodePtr)(cacheNode->Link.Next);
		}
	}

	return 0;
}

/********************************/
Ptr	gcDSGetCacheObj ( gcDSCachePtr cache, char *name )
{
	gcDSCacheNodePtr cacheNode;

	cacheNode = gcFindCacheNode(cache, name, 0);
	if(cacheNode)
	{
		cacheNode->ReferenceCount++;
		return cacheNode->Data;
	}
	
	return 0;
}

/********************************/
static void	gcFreeCacheNode ( gcDSCacheNodePtr *cacheNode )
{
	if(!(*cacheNode))return;
	if((*cacheNode)->Free)
		(*cacheNode)->Free(&(*cacheNode)->Data);


	MEM_Free((*cacheNode)->Name);
	MEM_Free(*cacheNode);
	(*cacheNode) = 0;
}

/********************************/
void	gcDSInitCache ( gcDSCachePtr cache )
{
	gcDSCacheNode cacheNode;

    cache->PurgeFlag = gcDS_AUTO_PURGE;
	gcDSInitList(&cache->CacheNodeList, (Ptr)(&cacheNode), &cacheNode.Link);
}

/********************************/
void	gcDSPurgeCache ( gcDSCachePtr cache )
{
	gcDSCacheNodePtr cursor;
	gcDSCacheNodePtr cacheNode;

	cursor = (gcDSCacheNodePtr)(cache->CacheNodeList.Head);
	while(cursor)
	{
		cacheNode = cursor;
		cursor = (gcDSCacheNodePtr)(cursor->Link.Next);		

		if(!cacheNode->ReferenceCount)
		{
			gcDSRemoveListObject(&cache->CacheNodeList, (Ptr)cacheNode);
			gcFreeCacheNode(&cacheNode);
		}
	}
}

/********************************/
void	gcDSReleaseCacheObj ( gcDSCachePtr cache, Ptr data )
{
	gcDSCacheNodePtr cacheNode;

	cacheNode = gcFindCacheNode(cache, 0, data);
	if(!cacheNode) return;

	if(cacheNode->ReferenceCount) cacheNode->ReferenceCount--;
	if((cacheNode->ReferenceCount == 0)&&(cache->PurgeFlag == gcDS_AUTO_PURGE))
	{
		gcDSRemoveListObject(&cache->CacheNodeList, (Ptr)(&cacheNode->Link));
		gcFreeCacheNode(&cacheNode);
	}
}

/********************************/
void	gcDSSetCachePurgeFlag ( gcDSCachePtr cache, u8 purgeFlag )
{
	cache->PurgeFlag = purgeFlag;
}

/*>*******************************(*)*******************************<*/
//	GLOBAL DISPLAY CACHE FUNCTIONS
/*>*******************************(*)*******************************<*/
void	gcCSHInitDisplayCache	( void )
{
	gcDSInitCache (&gcDODisplayCache);
	gcDOCacheInitialized = 1;
}



/*>*******************************(*)*******************************<*/
static void gcLoadTexPalette		( gcTEXPalettePtr *pal, char *name );
static void gcUnpackTexPalette	( gcTEXPalettePtr pal );
static void gcTexFreeFunc	        ( gcTEXPalettePtr *pal );

/*>*******************************(*)*******************************<*/
gcTEXDescriptorPtr gcTEXGet	( gcTEXPalettePtr pal, u32 id )
{
	ASSERTMSG(id < pal->numDescriptors, GET_TEXTURE_1);
	
	return &pal->descriptorArray[id];
}


/*>*******************************(*)*******************************<*/
void gcTEXGetPalette ( gcTEXPalettePtr *pal, char *name )
{
	void *p = gcTexFreeFunc;

	if(gcDOCacheInitialized) *pal = (gcTEXPalettePtr)(gcDSGetCacheObj(&gcDODisplayCache, name));
	if(!*pal)	//file was not found in cache so load it!!!
	{
		gcLoadTexPalette(pal, name);
		if(gcDOCacheInitialized) 
		{
			gcDSAddCacheNode(&gcDODisplayCache, name, (Ptr)(*pal), (Ptr)p);
			gcDSGetCacheObj(&gcDODisplayCache, name); //Increment reference count
		}
	}
}

/*>*******************************(*)*******************************<*/
static void gcLoadTexPalette ( gcTEXPalettePtr *pal, char *name )
{
/*	DVDFileInfo dfi;

	DVDOpen(name, &dfi);
	(*pal) = (gcTEXPalettePtr)MEM_p_AllocAlign(OSRoundUp32B(dfi.length),32);
	DVDRead(&dfi, (*pal), (s32)OSRoundUp32B(dfi.length), 0);
	DVDClose(&dfi);*/
	
    u32 ulFileLength;
	tdstGC_File *PalFile = GC_fOpen(name, NULL);
	
	// The file MUST exist (even if cover is open)	
	ERR_X_Assert(PalFile != NULL)
	if (!PalFile)
	{
		pal = NULL;
		return;
	}
	
	ulFileLength = OSRoundUp32B(DVDGetLength(&PalFile->stFileInfo));
	
	(*pal) = (gcTEXPalettePtr)MEM_p_AllocAlign(ulFileLength,32);
	
	GC_fRead(PalFile,(*pal),ulFileLength);
	
	GC_fClose(PalFile);

	gcUnpackTexPalette(*pal);
}

/*>*******************************(*)*******************************<*/
static void gcUnpackTexPalette ( gcTEXPalettePtr pal )
{
	u16 i;

#ifdef WIN32
	EndianSwap32(&pal->numDescriptors);
	EndianSwap32(&pal->versionNumber);
	EndianSwap32(&pal->descriptorArray);
#endif

	ERR_X_Assert(pal->versionNumber == 2142000);
	if(pal->versionNumber != 2142000 )
		return;
		//OSHalt("invalid version number for texture palette");
	
	pal->descriptorArray = (gcTEXDescriptorPtr)(((u32)(pal->descriptorArray)) + ((u32)pal));
	
	for ( i = 0; i < pal->numDescriptors; i++ )
	{

#ifdef WIN32
		EndianSwap32(&pal->descriptorArray[i].textureHeader);
#endif

		if(pal->descriptorArray[i].textureHeader)
		{
			pal->descriptorArray[i].textureHeader = (gcTEXHeaderPtr)(((u32)(pal->descriptorArray[i].textureHeader)) + ((u32)pal));	

#ifdef WIN32
			EndianSwap16(&pal->descriptorArray[i].textureHeader->height);
			EndianSwap16(&pal->descriptorArray[i].textureHeader->width);
			EndianSwap32(&pal->descriptorArray[i].textureHeader->format);
			EndianSwap32(&pal->descriptorArray[i].textureHeader->data);
			EndianSwap32(&pal->descriptorArray[i].textureHeader->wrapS);
			EndianSwap32(&pal->descriptorArray[i].textureHeader->wrapT);
			EndianSwap32(&pal->descriptorArray[i].textureHeader->minFilter);
			EndianSwap32(&pal->descriptorArray[i].textureHeader->magFilter);
			EndianSwap32(&pal->descriptorArray[i].textureHeader->LODBias);
#endif

			if(!(pal->descriptorArray[i].textureHeader->unpacked))
			{
				pal->descriptorArray[i].textureHeader->data = (Ptr)((u32)(pal->descriptorArray[i].textureHeader->data) + (u32)pal);
				pal->descriptorArray[i].textureHeader->unpacked = 1;
			}
		}

#ifdef WIN32
			EndianSwap32(&pal->descriptorArray[i].CLUTHeader);
#endif
		
		if(pal->descriptorArray[i].CLUTHeader)
		{
			pal->descriptorArray[i].CLUTHeader = (gcCLUTHeaderPtr)((u32)(pal->descriptorArray[i].CLUTHeader) + (u32)pal);		

#ifdef WIN32
			EndianSwap16(&pal->descriptorArray[i].CLUTHeader->numEntries);
			EndianSwap32(&pal->descriptorArray[i].CLUTHeader->format);
			EndianSwap32(&pal->descriptorArray[i].CLUTHeader->data);
#endif

			if(!(pal->descriptorArray[i].CLUTHeader->unpacked))
			{
				pal->descriptorArray[i].CLUTHeader->data = (Ptr)((u32)(pal->descriptorArray[i].CLUTHeader->data) + (u32)pal);
				pal->descriptorArray[i].CLUTHeader->unpacked = 1;
			}
		}
		
	}
}


/*>*******************************(*)*******************************<*/
static void gcTexFreeFunc	( gcTEXPalettePtr *pal )
{
	MEM_Free(*pal);
	*pal = 0;
}

/*>*******************************(*)*******************************<*/
void gcTEXReleasePalette ( gcTEXPalettePtr *pal )
{
	if(gcDOCacheInitialized) 
		gcDSReleaseCacheObj(&gcDODisplayCache, (Ptr)(*pal));
	else
	{
		MEM_Free(*pal);
		*pal = 0;
	}		
}

/*>*******************************(*)*******************************<*/
void gcTEXGetGXTexObjFromPalette ( gcTEXPalettePtr pal, GXTexObj *to, u32 id )
{
	gcTEXDescriptorPtr tdp = gcTEXGet(pal, id);
	GXBool mipMap;

	//initTexObj with texture values

	if(tdp->textureHeader->minLOD == tdp->textureHeader->maxLOD)
		mipMap = GX_FALSE;
	else
		mipMap = GX_TRUE;

	GXInitTexObj(to, 
				 tdp->textureHeader->data, 
				 tdp->textureHeader->width, 
    			 tdp->textureHeader->height, 
				 (GXTexFmt)tdp->textureHeader->format,
	   			 tdp->textureHeader->wrapS, 
				 tdp->textureHeader->wrapT, 
				 mipMap); 

	
	GXInitTexObjLOD(to, tdp->textureHeader->minFilter, 
					tdp->textureHeader->magFilter, 
					tdp->textureHeader->minLOD, 
					tdp->textureHeader->maxLOD, 
					tdp->textureHeader->LODBias,
					GX_DISABLE,
					tdp->textureHeader->edgeLODEnable,
					GX_ANISO_1);  
}

/*>*******************************(*)*******************************<*/
void gcTEXGetGXTexObjFromPaletteCI ( gcTEXPalettePtr pal, GXTexObj *to, 
			    				   GXTlutObj *tlo, GXTlut tluts, u32 id )
{
	GXBool mipMap;
	gcTEXDescriptorPtr tdp = gcTEXGet(pal, id);

	//initTexObj with texture values

	if(tdp->textureHeader->minLOD == tdp->textureHeader->maxLOD)
		mipMap = GX_FALSE;
	else
		mipMap = GX_TRUE;

	GXInitTlutObj(tlo,
	              tdp->CLUTHeader->data,
	              (GXTlutFmt)tdp->CLUTHeader->format,
	              tdp->CLUTHeader->numEntries );

	GXInitTexObjCI(	to, 
					tdp->textureHeader->data, 
					tdp->textureHeader->width, 
    			 	tdp->textureHeader->height, 
					(GXCITexFmt)tdp->textureHeader->format,
	   			 	tdp->textureHeader->wrapS, 
					tdp->textureHeader->wrapT, 
				 	mipMap, 
					tluts );
	
	GXInitTexObjLOD(to, 
					tdp->textureHeader->minFilter, 
					tdp->textureHeader->magFilter, 
					tdp->textureHeader->minLOD, 
					tdp->textureHeader->maxLOD, 
					tdp->textureHeader->LODBias,
					GX_DISABLE,
					tdp->textureHeader->edgeLODEnable,
					GX_ANISO_1);  
}

/*>*******************************(*)*******************************<*/
// END INCLUDE NINTENDO LIB	
	
	
	
	
	
	
	
	
	
	
	
static int gcINO_i_SavLoadIcons(char *_pc_Dest, char *_az_FileName, unsigned int _ui_IconNb, unsigned int _ui_Format)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static gcTEXPalettePtr	tplIcons;
	static gcTEXDescriptorPtr tdpIcons;
	unsigned int			i;
	unsigned int			ui_ReadSize;
	unsigned int			ui_IconSize;
	char					clut[gcINO_Cte_SavClutSize];
	BOOL					b_clut;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pc_Dest) return 0;
	if(!_az_FileName) return 0;
	if(!_ui_IconNb) return 0;

	/* Load the TPL file */
	gcTEXGetPalette(&tplIcons, _az_FileName);

	ui_ReadSize = 0;
	b_clut = FALSE;
	for(i = 0; i < _ui_IconNb; i++)
	{
		gcTEXHeaderPtr  pTextureHeader; 
		char *pData;
		
		tdpIcons = gcTEXGet(tplIcons, i);
		pTextureHeader = tdpIcons->textureHeader;
		pData = pTextureHeader->data;
		
		if(gcINO_M_SavIconIsC8(i, _ui_Format))
		{
			ui_IconSize = (CARD_ICON_WIDTH * CARD_ICON_HEIGHT);
			L_memcpy(clut, tdpIcons->CLUTHeader->data, gcINO_Cte_SavClutSize);
			b_clut = TRUE;
		}
		else
			ui_IconSize = (2 * CARD_ICON_WIDTH * CARD_ICON_HEIGHT);
			
		ERR_X_Assert(pTextureHeader->height == CARD_ICON_HEIGHT);
		ERR_X_Assert(pTextureHeader->width == CARD_ICON_WIDTH);
		ERR_X_Assert(pTextureHeader->format == GX_TF_RGB5A3);

		L_memcpy(_pc_Dest + ui_ReadSize, pData, ui_IconSize);
		ui_ReadSize += ui_IconSize;
	}

	if(b_clut)
	{
		L_memcpy(_pc_Dest + ui_ReadSize, clut, gcINO_Cte_SavClutSize);
		ui_ReadSize += gcINO_Cte_SavClutSize;
	}

	/* Free the TexPalette */
	gcTEXReleasePalette(&tplIcons);

	return ui_ReadSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static gcDVDBanner *gcINO_pst_GetBanner(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	gcDVDBanner	*banner;
	tdstGC_File *pFile;
	BOOL		result;
	s32			length;
	/*~~~~~~~~~~~~~~~~~*/

	
	pFile = GC_fOpen("/"DVD_BANNER_FILENAME,0);
	if(!pFile) return NULL;

	length = (s32) OSRoundUp32B(DVDGetLength(&pFile->stFileInfo));
	if(length < sizeof(gcDVDBanner)) 
	{
		GC_fClose(pFile);
		return NULL;
	}

	banner = MEM_p_AllocAlign(sizeof(gcDVDBanner), 32);
	if(!banner) return NULL;

	result =  GC_fRead(pFile, banner, sizeof(gcDVDBanner));
	if(result != sizeof(gcDVDBanner))
	{
		GC_fClose(pFile);
		MEM_FreeAlign(banner);
		return NULL;
	}

	if(banner->id != DVD_BANNER_ID)
	{
		GC_fClose(pFile);
		MEM_FreeAlign(banner);
		return NULL;
	}

	GC_fClose(pFile);
	return banner;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int gcINO_i_SavLoadBanner(char *_pc_Dest, gcDVDBanner	*pst_Banner, unsigned int _ui_Format)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	ui_ReadSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(gcINO_Cte_SavBannerIsC8 & _ui_Format)
	{
		ui_ReadSize = CARD_BANNER_WIDTH * CARD_BANNER_HEIGHT;
		L_memcpy(_pc_Dest, pst_Banner->image, ui_ReadSize);
		L_memcpy(_pc_Dest + ui_ReadSize, pst_Banner->image + ui_ReadSize, gcINO_Cte_SavClutSize);
		ui_ReadSize += gcINO_Cte_SavClutSize;
	}
	else
	{
		ui_ReadSize = 2 * CARD_BANNER_WIDTH * CARD_BANNER_HEIGHT;
		L_memcpy(_pc_Dest, pst_Banner->image, ui_ReadSize);
	}

	return ui_ReadSize;
}


static void gcINO_SavLoadIconAndBanner()
{
	gcDVDBanner	*pst_Banner;

	// Read banner
	pst_Banner = gcINO_pst_GetBanner();
	gcINO_i_SavLoadBanner(gcINO_gpst_SavData->banner, pst_Banner, gcINO_Cte_SavIconFlag);
	MEM_FreeAlign(pst_Banner);

	// Read icon
	gcINO_i_SavLoadIcons(gcINO_gpst_SavData->icon,gcINO_Cte_SavIconFile,1,gcINO_Cte_SavIconFlag);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG gcINO_ul_SavComputeDataCRC()
{
	// CRC (check sum) is computed with the data in gcINO_tdst_SavData, except the CRC itself.
	// We don't use the whole INO_tdst_SavManager.p_Temp buffer, the end of which is garbage.
		
	ULONG ulCRC = 1; // All zeroes is not valid.
	
	ULONG *pData 	= (ULONG *)(((u8 *) gcINO_gpst_SavData) + INO_Cte_GC_CRCSize);
	ULONG *pDataEnd = (ULONG *)(((u8 *) gcINO_gpst_SavData) + INO_Cte_SavBufferSize);
	
	while (pData < pDataEnd)
	{
		ulCRC += (*pData);
		pData++;
	}
	
	return ulCRC;
}

#ifdef GC_USE_ENCRYPTION

// Decrypt buffer (INO_gst_SavManager.p_Temp)
BOOL gcINO_bDecryptBuffer()
{
	BOOL bSuccess = TRUE;
	char *pCryptedBuffer = INO_gst_SavManager.p_Temp + INO_Cte_GC_SystemSize;
	
	if(BIG_b_IsBufferCrypted(pCryptedBuffer))
	{
		char * pBuffer = (char *)MEM_p_Alloc(INO_Cte_SavFileSize-INO_Cte_GC_SystemSize);
		char * pDecrypted = pBuffer;
		
		// Decrypt
		BIG_special_Decryption(&pDecrypted, &pCryptedBuffer, NULL, INO_Cte_SavFileSize-INO_Cte_GC_SystemSize, INO_gst_SavManager.ul_CRC);

		BIG_ComputeCRC(&INO_gst_SavManager.ul_CRC[6], pDecrypted, INO_Cte_SavBufferSize-INO_Cte_GC_SystemSize);
		
		if (INO_gst_SavManager.ul_CRC[0] != INO_gst_SavManager.ul_CRC[6] ||
			pBuffer != pDecrypted)
			bSuccess = FALSE;
		else
			L_memcpy(pCryptedBuffer,pDecrypted,INO_Cte_SavBufferSize-INO_Cte_GC_SystemSize);


		MEM_Free(pBuffer);
	}
	return bSuccess;
}

// Encrypt buffer (INO_gst_SavManager.p_Temp)
void gcINO_EncryptBuffer()
{
	BOOL	bResult;
    DWORD   dwSize;
	char	ac_Key[ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE];
	char	*pEncrypted, *pBuffer;
	char    *pCryptedBuffer = INO_gst_SavManager.p_Temp + INO_Cte_GC_SystemSize;

	// Compute Encrypting Key ...
	BIG_ComputeKey(ac_Key,pCryptedBuffer, INO_Cte_SavBufferSize-INO_Cte_GC_SystemSize);

	// Encryption ...
	pBuffer = pEncrypted = (char *)MEM_p_Alloc(INO_Cte_SavFileSize-INO_Cte_GC_SystemSize);
	BIG_special_Encryption(&pEncrypted, &pCryptedBuffer , ac_Key, INO_Cte_SavBufferSize-INO_Cte_GC_SystemSize, TRUE);
	
	if (pBuffer == pEncrypted)
		L_memcpy(pCryptedBuffer,pEncrypted,(INO_Cte_SavFileSize-INO_Cte_GC_SystemSize));
			
	MEM_Free(pBuffer);
}
#endif // GC_USE_ENCRYPTION



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcINO_SavModuleInit(void)
{
	CARDInit();
	//L_memset(gcINo_gax_SavPortDescriptor, 0, gcINO_Cte_SavPortNbMax * sizeof(gcINO_tdst_SavPortDescriptor));
	gcINO_gst_SavManager.b_CardIsMounted == FALSE;
	gcINO_gst_SavManager.b_FileIsOpened = FALSE;
	gcINO_gst_SavManager.b_TestCRC = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcINO_SavModuleClose(void)
{
	/*~~*/
	int i;
	/*~~*/ 
	
	for(i = 0; i < 2; i++)
	{
		while(CARDGetResultCode(i) == CARD_RESULT_BUSY)
		{
		}
	}

}

extern u16 gcINO_usGetButtonState(); // Utiliser PAD_BUTTON_LEFT, PAD_BUTTON_A, PAD_TRIGGER_Z, etc ...
 
 // Called each frame to update the state of the memory card manager.
void gcINO_SavUpdate(void)
{
	int i;
	
	// Update button state
	gcINO_gst_SavManager.us_LastButton = gcINO_gst_SavManager.us_Button;
	gcINO_gst_SavManager.us_Button = gcINO_usGetButtonState();
	
 
	// State machine switch.
	switch(gcINO_gst_SavManager.en_State)
	{
	
	// Default inactive state						
	case SavState_e_Inactive:
		gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None; 
		gcINO_gst_SavManager.b_IsWorking = FALSE; 
		INO_gb_Writing = FALSE;
		if (gc_p_CardWorkArea != NULL)
		{
			MEM_Free(gc_p_CardWorkArea);
			gc_p_CardWorkArea = NULL;
		}
		break;		

	// Wait user input before accessing to card.
/*	case SavState_e_BeforeAccessing:
		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			gcINO_gst_SavManager.en_State = SavState_e_Connect;
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
		}		
		break;*/

	// NORMAL STATES
	
	case SavState_e_Connect: // Start all card operations here.
	
		// wait a little (to allow user to read the message )
		if (gcINO_gst_SavManager.i_TimeTest > 0)
		{
			gcINO_gst_SavManager.i_TimeTest--;
			return;
		}
			

		gcINO_gst_SavManager.b_TestCRC = FALSE;
		gcINO_gst_SavManager.en_StateAfterClose = SavState_e_Unmount;
		gcINO_gpst_SavData = INO_gst_SavManager.p_Temp;
		
/*		if (gcINO_gst_SavManager.b_CardIsMounted)
			gcINO_gst_SavManager.en_State = SavState_e_Check;
		else
		{*/
		ERR_X_Assert(gcINO_gst_SavManager.b_FileIsOpened == FALSE);
		
		if (gcINO_gst_SavManager.en_UserCommand == SavCmd_e_ReadAllHeaders)
		{
			gcINO_gst_SavManager.i_UsedSlotNb = 0;
			INO_gst_SavManager.i_CurrentSlot = -1;
			gcINO_gst_SavManager.en_StateAfterClose = SavState_e_Open;
			
			L_memset(INO_gst_SavManager.ast_SlotDesc,0,sizeof(INO_tdst_SavSlotDesc) *INO_Cte_SavSlotNbMax);
		}
			
		// get info
		gcINO_gst_SavManager.i_FreeFiles = -1;
		gcINO_gst_SavManager.i_FreeBytes = -1;
		
		gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARDProbeEx(
			gcINO_gst_SavManager.i_CurrentCardId,
			&gcINO_gst_SavManager.i_MemSize,
			&gcINO_gst_SavManager.i_SectorSize);
		
		switch (gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		case CARD_RESULT_BUSY:
			return;
		case CARD_RESULT_READY:
			gcINO_gst_SavManager.en_State = SavState_e_Mount;
			break;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_WRONGDEVICE:
			M_ErrorWrongDevice();
			break;
		default:
			M_UnknownError();
		}
//		}
		break;		
		

	case SavState_e_Mount:
		if (gc_p_CardWorkArea == NULL)
			gc_p_CardWorkArea = MEM_p_AllocAlign(CARD_WORKAREA_SIZE,32);
		L_memset(gc_p_CardWorkArea,0,CARD_WORKAREA_SIZE);
			
		gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARD_RESULT_BUSY;
		gcINO_gst_SavManager.i_TimeOut = 0;
		
		INO_gb_Writing = TRUE;
		CARDMountAsync(
			gcINO_gst_SavManager.i_CurrentCardId,
			gc_p_CardWorkArea, 
			&gcCARDDetachCallback, 
			&gcCARDMountDoneCallback);
		gcINO_gst_SavManager.en_State = SavState_e_Mounting;
		break;
	
	case SavState_e_Mounting:
		if (gcINO_gst_SavManager.i_gcnMcxxxx_Result != CARD_RESULT_BUSY)
			INO_gb_Writing = FALSE;

		switch (gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		// memory card is not mounted
		case CARD_RESULT_BUSY:
			M_TestBusy();
			if (gcINO_gst_SavManager.i_TimeOut > 60 * 10)
				gcINO_gst_SavManager.en_State = SavState_e_Mount;
			break;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_WRONGDEVICE:
			M_ErrorWrongDevice();
			break;
		case CARD_RESULT_IOERROR:
			M_ErrorNotFunctionnalMc();
			break;
			
		// memory card is mounted
		case CARD_RESULT_READY:
			gcINO_gst_SavManager.en_State = SavState_e_Check;
			break;
		case CARD_RESULT_BROKEN:
			// Card is broken -> try to repair it with CARDCheckAsync
			gcINO_gst_SavManager.en_State = SavState_e_Check;
			break;
		case CARD_RESULT_ENCODING:
			M_ErrorWrongFormatMc();
			break;
			
		default:
			M_UnknownError();
		}
		break;

	case SavState_e_Check:
		gcINO_gst_SavManager.i_gcnMcxxxx_Result = 
			CARDCheck(gcINO_gst_SavManager.i_CurrentCardId);

		switch (gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		// memory card is not mounted
		case CARD_RESULT_BUSY:
			return;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_WRONGDEVICE:
			M_ErrorWrongDevice();
			break;
		case CARD_RESULT_IOERROR:
			M_ErrorNotFunctionnalMc();
			break;
			
		// memory card is mounted
		case CARD_RESULT_READY:
			gcINO_gst_SavManager.en_State = SavState_e_GetFreeBlocks;
			break;
		case CARD_RESULT_BROKEN:
			// Card is broken -> try to format
			M_ErrorUnformattedMc();
			break;
		case CARD_RESULT_ENCODING:
			M_ErrorWrongFormatMc();
			break;
			
		default:
			M_UnknownError();
		}
		break;

	case SavState_e_GetFreeBlocks:
		if (gcINO_gst_SavManager.i_TimeTest > 0)
		{
			gcINO_gst_SavManager.i_TimeTest--;
			return;
		}
		
		gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARDFreeBlocks(
			gcINO_gst_SavManager.i_CurrentCardId, 
			&gcINO_gst_SavManager.i_FreeBytes,
			&gcINO_gst_SavManager.i_FreeFiles);
			
		switch(gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		case CARD_RESULT_BUSY:
			return;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_READY:
			gcINO_gst_SavManager.en_State = SavState_e_Open;
			break;
		case CARD_RESULT_BROKEN:
			M_ErrorUnformattedMc();
			break;
		default:
			M_UnknownError();
		}
		break;
		
	case SavState_e_Open:
	{
		
		if (gcINO_gst_SavManager.en_UserCommand == SavCmd_e_ReadAllHeaders)
		{
			INO_gst_SavManager.i_CurrentSlot++;
			
			// Stop reading headers (max nb of headers reached).
			if (INO_gst_SavManager.i_CurrentSlot >= INO_Cte_SavSlotNbMax)
			{
				gcINO_gst_SavManager.en_State = SavState_e_Unmount;
				break;
			}
		}		

		sprintf(gcINO_gst_SavManager.s_Filename,"%s%d",gcINO_Cte_SavFileName,INO_gst_SavManager.i_CurrentSlot);

		gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARDOpen(
			gcINO_gst_SavManager.i_CurrentCardId,
			gcINO_gst_SavManager.s_Filename,
			&gcINO_gst_SavManager.st_CardFileManagerInfo);
			
		switch(gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		case CARD_RESULT_BUSY:
			return;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_READY:
			gcINO_gst_SavManager.b_FileIsOpened = TRUE;
			switch(gcINO_gst_SavManager.en_UserCommand)
			{
			case SavCmd_e_ReadAllHeaders:
				gcINO_gst_SavManager.i_UsedSlotNb++;
			case SavCmd_e_Read:
				gcINO_gst_SavManager.en_State = SavState_e_Read;
				break;
			case SavCmd_e_Write:
				M_ConfirmOverwriting();
				break;
			case SavCmd_e_Delete:
				gcINO_gst_SavManager.en_State = SavState_e_Delete;
				break;
			case SavCmd_e_BootupTest:
				gcINO_gst_SavManager.en_State = SavState_e_Close;
				break;
			default:
				M_UnknownError();
			}
			break;
		case CARD_RESULT_BROKEN:
			M_ErrorUnformattedMc();
			break;
		case CARD_RESULT_NOFILE:
			{
				if ((gcINO_gst_SavManager.en_UserCommand == SavCmd_e_Write) || 
					((gcINO_gst_SavManager.en_UserCommand == SavCmd_e_ReadAllHeaders) && 
					(INO_gst_SavManager.i_CurrentSlot == INO_Cte_SavSlotNbMax-1) && 
					(gcINO_gst_SavManager.i_UsedSlotNb == 0)
					))
				{
					// Test if there is enough space.
				    u32 uNeededSize = RoundUp(INO_Cte_SavFileSize,gcINO_gst_SavManager.i_SectorSize);
				    
					if ((gcINO_gst_SavManager.i_FreeFiles < 1) || 
						(gcINO_gst_SavManager.i_FreeBytes < uNeededSize)) // Not enough space to save game
					{
						M_ErrorNoMoreSpaceOnMc();
						break;
					}
						
				}
				
				switch(gcINO_gst_SavManager.en_UserCommand)
				{
				case SavCmd_e_Write:
					gcINO_gst_SavManager.en_State = SavState_e_Create;
					break;
					
				case SavCmd_e_ReadAllHeaders:
					gcINO_gst_SavManager.en_State = SavState_e_Open;
					break;
				
				case SavCmd_e_Read:
					M_ErrorNoSaveOnMc();
					break;
					
				case SavCmd_e_BootupTest:
				case SavCmd_e_Delete:
					gcINO_gst_SavManager.en_State = SavState_e_Unmount;
					break;
					
				default:
					M_UnknownError();
				}
			}
			break;
		default:
			M_UnknownError();
		}
		break;	
	}
					
	case SavState_e_Create:
	    {                
	    	// Test size before creation.
	    	s32 byteNotUsed,  filesNotUsed;
		    
		    gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARDFreeBlocks(
		    	gcINO_gst_SavManager.i_CurrentCardId, 
		    	& byteNotUsed,
		    	& filesNotUsed);
		    	
			switch(gcINO_gst_SavManager.i_gcnMcxxxx_Result)
			{		    	
			case CARD_RESULT_BUSY:
				break;
			case CARD_RESULT_NOCARD:
				M_ErrorNoMc();
				break;
			case CARD_RESULT_READY:
			{
			
			    u32 uSize = RoundUp(INO_Cte_SavFileSize,gcINO_gst_SavManager.i_SectorSize);
				if ((byteNotUsed >= uSize) && (filesNotUsed >= 1))
				{
					gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARD_RESULT_BUSY;
					gcINO_gst_SavManager.i_TimeOut = 0;
					INO_gb_Writing = TRUE;
					CARDCreateAsync(
						gcINO_gst_SavManager.i_CurrentCardId,
						gcINO_gst_SavManager.s_Filename,
						uSize,
						&gcINO_gst_SavManager.st_CardFileManagerInfo,
						&CARDCreatingCallback
						);
					
					gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Saving;
					gcINO_gst_SavManager.en_State = SavState_e_Creating;
				}
				else
				{
					M_ErrorNoMoreSpaceOnMc();
				}
				break;
			}
			case CARD_RESULT_BROKEN:
				M_ErrorUnformattedMc();
				break;
			default:
				M_UnknownError();
			}
			break;		    	
		}		
				
	case SavState_e_Creating:
		if (gcINO_gst_SavManager.i_gcnMcxxxx_Result != CARD_RESULT_BUSY)
			INO_gb_Writing = FALSE;
	
		switch(gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		case CARD_RESULT_BUSY:
			M_TestBusy();
			if (gcINO_gst_SavManager.i_TimeOut > 60 * 10)
				gcINO_gst_SavManager.en_State = SavState_e_Create;
			break;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_READY:
			gcINO_gst_SavManager.en_State = SavState_e_SetStatus;
			break;
		case CARD_RESULT_BROKEN:
			M_ErrorUnformattedMc();
			break;
		case CARD_RESULT_IOERROR:
			M_ErrorNotFunctionnalMc();
			break;
		case CARD_RESULT_NOENT:
		case CARD_RESULT_INSSPACE:
			M_ErrorNoMoreSpaceOnMc();
			break;
		case CARD_RESULT_EXIST:
			M_ConfirmOverwriting();
			break;
		default:
			M_UnknownError();
		}
		break;
		
	case SavState_e_SetStatus:
	
		CARDSetBannerFormat(&gcINO_gst_SavManager.st_CardFileManagerStat,CARD_STAT_BANNER_RGB5A3);
		CARDSetCommentAddress(&gcINO_gst_SavManager.st_CardFileManagerStat,INO_Cte_GC_CRCSize);
		CARDSetIconAddress(&gcINO_gst_SavManager.st_CardFileManagerStat,INO_Cte_GC_CRCSize+INO_Cte_GC_CommentSize);
		CARDSetIconAnim(&gcINO_gst_SavManager.st_CardFileManagerStat,CARD_STAT_ANIM_BOUNCE);
		CARDSetIconFormat(&gcINO_gst_SavManager.st_CardFileManagerStat,0,CARD_STAT_ICON_RGB5A3);
		CARDSetIconSpeed(&gcINO_gst_SavManager.st_CardFileManagerStat,0,CARD_STAT_SPEED_MIDDLE);
	
		gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARDSetStatus(
			gcINO_gst_SavManager.i_CurrentCardId,
			gcINO_gst_SavManager.st_CardFileManagerInfo.fileNo, 
			&gcINO_gst_SavManager.st_CardFileManagerStat
			);
			
		switch(gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		case CARD_RESULT_BUSY:
			return;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_READY:
			// Write to buffer the system data (the buffer has already been memset to 0).
			gcINO_SavLoadIconAndBanner();
			sprintf(gcINO_gpst_SavData->sComment1,gcINO_Cte_SavComment1);
			sprintf(gcINO_gpst_SavData->sComment2,"Profile %d: %s",1+INO_gst_SavManager.i_CurrentSlot,INO_gst_SavManager.ast_SlotDesc[INO_gst_SavManager.i_CurrentSlot].asz_Name);
			gcINO_gst_SavManager.en_State = SavState_e_Write;
			break;
		case CARD_RESULT_IOERROR:
			M_ErrorNotFunctionnalMc();
			break;
		default:
			M_UnknownError();
		}
		break;

	case SavState_e_Read:
		{
			// Read all data from card to buffer (read is fast, so we always read all).
			// Read a multiple of CARD_READ_SIZE that contains the data.
			ULONG ulReadSize = RoundUp(INO_Cte_SavFileSize,CARD_READ_SIZE);
			ERR_X_Assert(INO_gst_SavManager.i_TempSize >= ulReadSize);

			// Make sure that the cache is flushed to RAM (so that we will read data from RAM, and not from invalid cache).
			DCFlushRange(INO_gst_SavManager.p_Temp, ulReadSize);
			
			gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARD_RESULT_BUSY;
			gcINO_gst_SavManager.i_TimeOut = 0;
			INO_gb_Writing = TRUE;
			CARDReadAsync(
				&gcINO_gst_SavManager.st_CardFileManagerInfo,
				INO_gst_SavManager.p_Temp,
				ulReadSize,
				0,
	            &CARDReadCallback);
	            
	        gcINO_gst_SavManager.en_State = SavState_e_Reading;
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
	    }
            
		break;	
		
	case SavState_e_Reading:
		if (gcINO_gst_SavManager.i_gcnMcxxxx_Result != CARD_RESULT_BUSY)
			INO_gb_Writing = FALSE;
	
		switch (gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		case CARD_RESULT_BUSY:
			M_TestBusy();
			if (gcINO_gst_SavManager.i_TimeOut > 60 * 10)
				gcINO_gst_SavManager.en_State = SavState_e_Read;
			return;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_READY:
		
						
			// Check that the data that we just read is valid (CRC).
			if (
#ifdef GC_USE_ENCRYPTION
			gcINO_bDecryptBuffer() && 
#endif // GC_USE_ENCRYPTION
			gcINO_ul_SavComputeDataCRC() == gcINO_gpst_SavData->ulCRC)
			{	
				// Valid CRC 
				switch(gcINO_gst_SavManager.en_UserCommand)
				{
				case SavCmd_e_ReadAllHeaders:
					{
						L_memcpy(&(INO_gst_SavManager.ast_SlotDesc[INO_gst_SavManager.i_CurrentSlot]),
							&(gcINO_gpst_SavData->st_Slot.st_Header),
							sizeof(INO_tdst_SavSlotDesc));
						gcINO_gst_SavManager.en_State = SavState_e_Close;
					}
					break;
				case SavCmd_e_Read:
					// Read header from work buffer to result buffer
					L_memcpy(&(INO_gst_SavManager.ast_SlotDesc[INO_gst_SavManager.i_CurrentSlot]),
						&(gcINO_gpst_SavData->st_Slot.st_Header),
						sizeof(INO_tdst_SavSlotDesc));
						
					// Read data from work buffer to result buffer
					L_memcpy(&(INO_gst_SavManager.ac_CurrSlotBuffer),
						&(gcINO_gpst_SavData->st_Slot.data),
						INO_Cte_SavOneSlotMaxSize);				
					gcINO_gst_SavManager.en_State = SavState_e_Close;
					break;
				case SavCmd_e_Delete:
				case SavCmd_e_Write:
					gcINO_gst_SavManager.en_State = SavState_e_Close;
					break;
				default:
					M_UnknownError();
				}
			}
			else
			{
				// Unvalid CRC
				
				// On efface le buffer interne (qui doit refléter l'état du fichier)
				L_memset(gcINO_gpst_SavData,0,sizeof(gcINO_tdst_SavData));
			
				M_ErrorBadFileOnMc();
			}
			break;
		default:
			M_UnknownError();
		}
		break;
		
	case SavState_e_Write:
		{
		u32 uWriteSize;
		
		// Read header from work buffer to result buffer
		L_memcpy(&(gcINO_gpst_SavData->st_Slot.st_Header),
			&(INO_gst_SavManager.ast_SlotDesc[INO_gst_SavManager.i_CurrentSlot]),
			sizeof(INO_tdst_SavSlotDesc));
			
		// Read data from work buffer to result buffer
		L_memcpy(&(gcINO_gpst_SavData->st_Slot.data),
			&(INO_gst_SavManager.ac_CurrSlotBuffer),
			INO_Cte_SavOneSlotMaxSize);				

		// Compute CRC
		gcINO_gpst_SavData->ulCRC = gcINO_ul_SavComputeDataCRC();
		
#ifdef GC_USE_ENCRYPTION
		gcINO_EncryptBuffer();
#endif // GC_USE_ENCRYPTION	
		
		// Make sure that the cache is flushed to RAM (so that written data is really in RAM, and not only in cache).
		DCFlushRange(INO_gst_SavManager.p_Temp,INO_Cte_SavFileSize);

		// We must write a multiple of SectorSize.
		// The last bytes that are written contain random data, because uWriteSize >= INO_tdst_SavManager.i_TempSize
		// This random data will not be read. 
				
		uWriteSize = RoundUp(INO_Cte_SavFileSize,gcINO_gst_SavManager.i_SectorSize);
		
		gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARD_RESULT_BUSY;
		gcINO_gst_SavManager.i_TimeOut = 0;
		INO_gb_Writing = TRUE;
		CARDWriteAsync(
			&gcINO_gst_SavManager.st_CardFileManagerInfo,
			INO_gst_SavManager.p_Temp,
			uWriteSize,
			0,
            &CARDWriteCallback);
            
		gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Saving;
        gcINO_gst_SavManager.en_State = SavState_e_Writing;
        }
		
		break;

		
	case SavState_e_Writing:
		if (gcINO_gst_SavManager.i_gcnMcxxxx_Result != CARD_RESULT_BUSY)
			INO_gb_Writing = FALSE;
	
		switch (gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		case CARD_RESULT_BUSY:
			M_TestBusy();
			if (gcINO_gst_SavManager.i_TimeOut > 60 * 10)
				gcINO_gst_SavManager.en_State = SavState_e_Write;
			return;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_IOERROR:
			M_ErrorNotFunctionnalMc();
			break;
		case CARD_RESULT_READY:
			gcINO_gst_SavManager.b_TestCRC = TRUE;
			gcINO_gst_SavManager.en_State = SavState_e_Read;
			break;
		default:
			M_UnknownError();
		}
		break;
		
	case SavState_e_Delete:
	
		gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARD_RESULT_BUSY;
		gcINO_gst_SavManager.i_TimeOut = 0;
		INO_gb_Writing = TRUE;
		CARDDeleteAsync(
			gcINO_gst_SavManager.i_CurrentCardId,
			gcINO_gst_SavManager.s_Filename,
			&CARDDeleteCallback);
			
		gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Saving;
	    gcINO_gst_SavManager.en_State = SavState_e_Deleting;
			
		break;
	
	case SavState_e_Deleting:
		if (gcINO_gst_SavManager.i_gcnMcxxxx_Result != CARD_RESULT_BUSY)
			INO_gb_Writing = FALSE;
	
		switch (gcINO_gst_SavManager.i_gcnMcxxxx_Result)
		{
		case CARD_RESULT_BUSY:
			M_TestBusy();
			if (gcINO_gst_SavManager.i_TimeOut > 60 * 10)
				gcINO_gst_SavManager.en_State = SavState_e_Delete;
			return;
		case CARD_RESULT_NOCARD:
			M_ErrorNoMc();
			break;
		case CARD_RESULT_IOERROR:
			M_ErrorNotFunctionnalMc();
			break;
		case CARD_RESULT_READY:		
			// On vient d'effacer le fichier
			// -> soit on le crée à nouveau (mode écriture), 
			// -> soit on abandonne (mode lecture).
			gcINO_gst_SavManager.en_StateAfterClose = SavState_e_Unmount;
			
			if (gcINO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
				gcINO_gst_SavManager.en_State = SavState_e_Create;
			else if (gcINO_gst_SavManager.en_UserCommand == SavCmd_e_Delete)
				gcINO_gst_SavManager.en_State = SavState_e_Close;
			else
			{
				gcINO_gst_SavManager.en_State = SavState_e_Open;
				//gcINO_gst_SavManager.b_IsWorking = FALSE;
				//gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForDeletingSuccess;
				//gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileSaving; 
				//gcINO_gst_SavManager.i_ConfirmOnSuccess = FALSE;
			}
				
			break;
		default:
			M_UnknownError();
		}
		break;
		
	case SavState_e_Close:
		M_Close();
		break;
						
	case SavState_e_Unmount:
		M_Unmount();
		break;

	// Stay in this state until card is extracted or high level API changes state.
	case SavState_e_TestMemCardWhileWaitingUser:
		if (gc_p_CardWorkArea != NULL)
		{
			MEM_Free(gc_p_CardWorkArea);
			gc_p_CardWorkArea = NULL;
		}
		if (M_TestMemCardWhileWaitingUser())
			gcINO_gst_SavManager.b_IsWorking = FALSE; 
		break;
	
		
		
		
	// ERROR STATES	
	
	// Format card ? 3 answers.
	case SavState_e_WaitingUserResponseForWrongFormatMc:
	case SavState_e_WaitingUserResponseForUnformatted:
		/* Test Memory Card presence */
		if (M_TestMemCardWhileWaitingUser())
			break;
			
		/* retry */
		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			M_Retrying();
		}
		/* format */
		else if(M_bButtonJustPressed(PAD_BUTTON_X))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_X;
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_FormatConfirmation;
			gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForFormat;
		}
		/* cancel : demande confirmation of continue without saving */		
		else if (M_bButtonJustPressed(PAD_BUTTON_B))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_B;
			M_ResetAll();
			
			// Si on met cette ligne, et qu'on fait un 
			// "continue withour saving" in-game après un "voulez-vous formater ?",
			// alors on tombe sur un écran noir.
			//gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_None; 
		}
		break;
	
	// Really format ?		
	case SavState_e_WaitingUserResponseForFormat:
		/* Test Memory Card presence */
		if (M_TestMemCardWhileWaitingUser())
			break;
			
		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Formating;
			gcINO_gst_SavManager.en_State = SavState_e_Formating_1;
		}
		else if(M_bButtonJustPressed(PAD_BUTTON_B))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_B;
			M_Retrying();
		}
		break;
		

	case SavState_e_Formating_1:
		if
		( 
			(gcINO_gst_SavManager.i_gcnMcxxxx_Result != CARD_RESULT_READY)
		&&	(gcINO_gst_SavManager.i_gcnMcxxxx_Result != CARD_RESULT_BUSY)
		)
		{
			M_ErrorWhileFormating();
		}
		else
		{
			gcINO_gst_SavManager.en_State = SavState_e_Formating_2;
			gcINO_gst_SavManager.i_TimeOut = 0;
			
			gcINO_gst_SavManager.i_gcnMcxxxx_Result = CARD_RESULT_BUSY;
			INO_gb_Writing = TRUE;
			CARDFormatAsync
				(
					gcINO_gst_SavManager.i_CurrentCardId,
					&gcCARDFormatCallback
				);
		}
		break;

	case SavState_e_Formating_2:
		if (gcINO_gst_SavManager.i_gcnMcxxxx_Result != CARD_RESULT_BUSY)
			INO_gb_Writing = FALSE;
	
		if (gcINO_gst_SavManager.i_gcnMcxxxx_Result == CARD_RESULT_BUSY)
		{
			M_TestBusy();
			if (gcINO_gst_SavManager.i_TimeOut > 60 * 10)
				gcINO_gst_SavManager.en_State = SavState_e_Formating_1;
			return; // waiting while formating.
		}
		else if (gcINO_gst_SavManager.i_gcnMcxxxx_Result != CARD_RESULT_READY)
		{
			M_ErrorWhileFormating();
		}
		else
		{
			M_SuccessWhileFormating();
		}
		break;
	
	// Errors during normal process. We may need to confirm that we cancel save.
	case SavState_e_WaitingUserResponseForMcAbsent:
	case SavState_e_WaitingUserResponseForWrongDevice:
	case SavState_e_WaitingUserResponseForBrokenDevice:
	case SavState_e_WaitingUserResponseForNotFuncionnalMc:
	case SavState_e_WaitingUserResponseForLoadingFailure:
			
		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			M_Retrying();
		}
		else if (M_bButtonJustPressed(PAD_BUTTON_B))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_B;
			if (gcINO_gst_SavManager.en_UserCommand == SavCmd_e_Write ||
				gcINO_gst_SavManager.en_UserCommand == SavCmd_e_Delete)
				//|| gcINO_gst_SavManager.en_UserCommand == SavCmd_e_TestMemCard)
			{
				// Annulation de la sauvegarde ?
				gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
				gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
			}
			else
			{
				M_ResetAll();
				//gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_None; 
			}
		}
		break;
		
	// Waiting for user response confirmation of action. We always need to confirm that we cancel save.
	case SavState_e_WaitingUserResponseForFormatFailure:
		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			M_Retrying();
		}
		else if(M_bButtonJustPressed(PAD_BUTTON_B))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_B;
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
			gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
		}
		break;
		
	
	// Not enough space error. 3 answers.
	case SavState_e_WaitingUserResponseForNoSpace:
		/* Test Memory Card presence */
		if (M_TestMemCardWhileWaitingUser())
			break;
				
		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			M_Retrying();
		}
		else if (M_bButtonJustPressed(PAD_BUTTON_X))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_X;
			// Reboot to IPL
			VISetBlack(TRUE); 
			VIFlush(); 
			VIWaitForRetrace(); 
			
			while (CARD_RESULT_BUSY == CARDUnmount(gcINO_gst_SavManager.i_CurrentCardId))
				;
				
			OSResetSystem(OS_RESET_HOTRESET, 0x01, TRUE);
		}
		else if (M_bButtonJustPressed(PAD_BUTTON_B))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_B;
			if(gcINO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
				gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
			}
			else
			{
				M_ResetAll();
			}
		}
		break;
	
		
	// Just wait for user to press A.
	case SavState_e_WaitingUserResponseForLoadingSuccess:
	case SavState_e_WaitingUserResponseForSavingSuccess:
		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			M_ResetAll();
		}
		break;
		
	case SavState_e_WaitingUserResponseForFormatSuccess:
		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			// Start again the process
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
			gcINO_gst_SavManager.en_State = SavState_e_Connect; 
			gcINO_gst_SavManager.i_ConfirmOnSuccess = TRUE;
			gcINO_gst_SavManager.b_IsWorking = TRUE;
			gcINO_gst_SavManager.i_TimeTest = 30;			
		}
		break;
		
	// After delete, restart operation
	case SavState_e_WaitingUserResponseForDeletingSuccess:
		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			gcINO_gst_SavManager.en_State = SavState_e_Open;
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing; 
			gcINO_gst_SavManager.b_IsWorking = TRUE;
			gcINO_gst_SavManager.i_ConfirmOnSuccess = FALSE;			
		}
		break;

	// Overwrite saved game ?
	case SavState_e_WaitingUserResponseForOverwriting:
		if (M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			gcINO_gst_SavManager.en_State = SavState_e_SetStatus;
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing; 
			gcINO_gst_SavManager.b_IsWorking = TRUE; 
		}
		else if ( M_bButtonJustPressed(PAD_BUTTON_B) )
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_B;
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
			gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
		}
		break;
	// Yes or No ?
	case SavState_e_WaitingUserResponseForCancelSave:
	case SavState_e_WaitingUserResponseForNoSaving: 
		if (M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			M_ResetAll();
		}
		else if ( M_bButtonJustPressed(PAD_BUTTON_B) )
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_B;
			M_Retrying();
		}
		break;
		
	case SavState_e_WaitingUserResponseForSavingFailure:
		if (M_TestMemCardWhileWaitingUser())
			break;

		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			M_Retrying();
		}
		else if (M_bButtonJustPressed(PAD_BUTTON_B))
		{
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_B;
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
			gcINO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
		}
		break;
		
	case SavState_e_WaitingUserResponseForBadFileOnMc:
		if(M_bButtonJustPressed(PAD_BUTTON_A))
		{
			// Retry 
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_A;
			M_Retrying();
		}
		else if (M_bButtonJustPressed(PAD_BUTTON_B))
		{
			// Continue Without Saving
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_X;
			gcINO_gst_SavManager.en_State = SavState_e_Close; 
			gcINO_gst_SavManager.b_IsWorking = FALSE; 
		}
		else if (M_bButtonJustPressed(PAD_BUTTON_X))
		{
			// Delete
			gcINO_gst_SavManager.i_UserAction = PAD_BUTTON_B;
			gcINO_gst_SavManager.en_State = SavState_e_Close;
			gcINO_gst_SavManager.en_StateAfterClose = SavState_e_Delete;
			gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Saving;
		}
		break;		
		
	default:
		break;
	}
}








// Called by high level API.
int gcINO_SavSeti(int _i_RequestId, int _i_Value)
{ 
	int i;
	
	ERR_X_Assert((INO_Cte_SavBufferSize) == sizeof(gcINO_tdst_SavData));	
	
	switch(_i_RequestId)
	{
	case INO_e_SavRq_CardId:
		if(gcINO_gst_SavManager.b_IsWorking) return -1;

		if((_i_Value == 0) || (_i_Value == 1))
		{
			gcINO_gst_SavManager.i_CurrentCardId = _i_Value;
			return 0;
		}
		else
		{
			gcINO_gst_SavManager.i_CurrentCardId = -1;
			return -1;
		}

	case INO_e_SavRq_Space:
		return gcINO_gst_SavManager.i_FreeBytes;

	case INO_e_SavRq_Status:
		return gcINO_gst_SavManager.b_IsWorking ? 1 : 0;

	case INO_e_SavRq_UserMessageId:
		return gcINO_gst_SavManager.en_UserMessage;

	case INO_e_SavRq_ClearMessageId:
		gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_None;
		break;
	case INO_e_SavRq_UserAction:
		{
			int iButton = -1;
			switch (gcINO_gst_SavManager.i_UserAction)
			{
				case PAD_BUTTON_A:
					iButton = eBtn_Cross;
					break;
				case PAD_BUTTON_B:
					iButton = eBtn_Triangle;
					break;
				case PAD_BUTTON_X:
					iButton = eBtn_Circle;
					break;
				default :
					iButton = -1;
			}
			gcINO_gst_SavManager.i_UserAction = -1;	
			return iButton;				
		}
	case INO_e_SavRq_ReadAllHeaders:
		gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;// INO_e_SavMsg_BeforeAccessing;
		gcINO_gst_SavManager.en_State = SavState_e_Connect; //SavState_e_BeforeAccessing;
		gcINO_gst_SavManager.en_UserCommand = SavCmd_e_ReadAllHeaders;
		gcINO_gst_SavManager.i_ConfirmOnSuccess = FALSE;
		gcINO_gst_SavManager.i_TimeTest = 30;
		gcINO_gst_SavManager.b_IsWorking = TRUE;
		break;

	case INO_e_SavRq_ReadOneProfile:
		gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;// INO_e_SavMsg_BeforeAccessing;
		gcINO_gst_SavManager.en_State = SavState_e_Connect; //SavState_e_BeforeAccessing;
		gcINO_gst_SavManager.en_UserCommand = SavCmd_e_Read;
		gcINO_gst_SavManager.i_ConfirmOnSuccess = TRUE;
		gcINO_gst_SavManager.i_TimeTest = 30;
		gcINO_gst_SavManager.b_IsWorking = TRUE;
		break;

	case INO_e_SavRq_WriteOneProfile:
		gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;// INO_e_SavMsg_BeforeAccessing;
		gcINO_gst_SavManager.en_State = SavState_e_Connect; //SavState_e_BeforeAccessing;
		if (INO_gst_SavManager.ast_SlotDesc[INO_gst_SavManager.i_CurrentSlot].ul_GameTime)
			gcINO_gst_SavManager.en_UserCommand = SavCmd_e_Write;
		else
			gcINO_gst_SavManager.en_UserCommand = SavCmd_e_Delete;
		gcINO_gst_SavManager.i_ConfirmOnSuccess = TRUE;
		gcINO_gst_SavManager.b_IsWorking = TRUE;
		gcINO_gst_SavManager.i_TimeTest = 30;
		//gcINO_gst_SavManager.i_Writing_ReadIsDone = FALSE;
		break;

	case INO_e_SavRq_FreeRessource:
		gcINO_gst_SavManager.us_Button = 0;
		gcINO_gst_SavManager.us_LastButton = 0;
		break;
		
	case INO_e_SavRq_BootupTest:
		gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_None;
		gcINO_gst_SavManager.en_State = SavState_e_Inactive;
		gcINO_gst_SavManager.en_UserCommand = SavCmd_e_None;
		gcINO_gst_SavManager.i_TimeTest = 0;
		gcINO_gst_SavManager.i_ConfirmOnSuccess = FALSE;
		gcINO_gst_SavManager.b_IsWorking = FALSE;
		/*gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
		gcINO_gst_SavManager.en_State = SavState_e_Connect;
		gcINO_gst_SavManager.en_UserCommand = SavCmd_e_BootupTest;
		gcINO_gst_SavManager.i_TimeTest = 30;
		gcINO_gst_SavManager.i_ConfirmOnSuccess = FALSE;
		gcINO_gst_SavManager.b_IsWorking = TRUE;*/
		break;
		
	case INO_e_SavRq_TestMemCard:
		gcINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
		gcINO_gst_SavManager.en_State = SavState_e_TestMemCardWhileWaitingUser;
		gcINO_gst_SavManager.en_UserCommand = SavCmd_e_TestMemCard;
		gcINO_gst_SavManager.i_ConfirmOnSuccess = FALSE;
		gcINO_gst_SavManager.i_TimeTest = 0;
		gcINO_gst_SavManager.b_IsWorking = TRUE;
		break;

	case INO_e_SavRq_SaveSaveBuffer:
	case INO_e_SavRq_RestoreSaveBuffer:
	default:
		return -1;
	}

	return 0;
}


