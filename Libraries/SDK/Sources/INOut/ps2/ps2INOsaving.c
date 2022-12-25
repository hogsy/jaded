/*$T ps2INOsaving.c GC 1.138 04/20/05 10:52:49 */


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
#include "INOut/ps2/ps2INOsaving.h"
#include "MC/MC_Manager.h"
#include "IOP/RPC_Manager.h"
#include "IOP/iop/iopMain.h"
#include "IOP/CDV_Manager.h"
#include <libmc.h>
#include "INOut/INOjoystick.h"
#include "BIGfiles/BIGspecial.h"


/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define sceMcSync_ModeNoWait	1
#define sceMcSync_ModeWait		0

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define Csz_IconSysFileName		"icon.sys"
#define Csz_IconViewFileName	"ps2.ico"
#ifdef PSX2_USE_iopCDV
#define Csz_IconModelFileName	CDV_Cte_IcoFile
#else
#define Csz_IconModelFileName	"host:X:/PS2.PICO"
#endif

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define Cte_McCardIdNbMax	2
#define Cte_McClusterSize	1024
#define Cte_DefaultSlotNb	0
/**/
#define Cte_HighTheadPrio	47
#define Cte_LowTheadPrio	49
/**/
#define Cte_TimeOut         2000
#define Cte_TimeTest        2.0f
#define Cte_ShortTimeTest	0.5f
/**/
#define M_dbg_printf(aa)    //printf aa

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define M_ResetAll() \
	do \
	{ \
		ps2INO_gst_SavManager.i_ForceLowPriority = FALSE;\
		ps2INO_gst_SavManager.en_State = SavState_e_Inactive; \
		ps2INO_gst_SavManager.en_UserCommand = SavCmd_e_None; \
		ps2INO_gst_SavManager.b_IsWorking = FALSE; \
	} while(0);
	
#define M_Retrying() \
	do \
	{ \
		ps2INO_gst_SavManager.i_ForceLowPriority = FALSE;\
		ps2INO_gst_SavManager.en_State = SavState_e_Inactive; \
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Retrying; \
		ps2INO_gst_SavManager.en_UserCommand = SavCmd_e_None; \
		ps2INO_gst_SavManager.b_IsWorking = FALSE; \
	} while(0);
	
#define M_SuccessWhileFormating() \
	do \
	{ \
		ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForFormatSuccess; \
		ps2INO_gst_SavManager.f_TimeTest = 2.0;\
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileFormating; \
		ps2INO_gst_SavManager.b_IsWorking = TRUE; \
	} while(0);

#define M_ErrorWhileFormating() \
	do \
	{ \
		ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForFormatFailure; \
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_ErrorWhileFormating; \
		ps2INO_gst_SavManager.b_IsWorking = TRUE; \
	} while(0);

	
	
#define M_TestMC_Success()\
	do \
	{ \
		ps2INO_gst_SavManager.en_State = SavState_e_Inactive; \
		ps2INO_gst_SavManager.en_UserCommand = SavCmd_e_None;\
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_None; \
		ps2INO_gst_SavManager.b_IsWorking = FALSE; \
	} while(0);
	
#define M_ErrorNoSaveOnMc() \
	do \
	{ \
		ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForNoSaving; \
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_NoSaving; \
		ps2INO_gst_SavManager.b_IsWorking = TRUE; \
	} while(0);

#define M_ErrorNoMoreSpaceOnMc() \
	do \
	{ \
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;\
		ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForNoSpace; \
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_NoMoreSpace; \
		ps2INO_gst_SavManager.b_IsWorking = TRUE; \
	} while(0);

#define M_ErrorUnformattedMc() \
	do \
	{ \
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;\
		ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForUnformatted; \
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_UnformattedCard; \
		ps2INO_gst_SavManager.b_IsWorking = TRUE; \
	} while(0);

#define M_ErrorWhileLoading() \
	do \
	{ \
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;\
		ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForLoadingFailure; \
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_ErrorWhileLoading; \
		ps2INO_gst_SavManager.b_IsWorking = TRUE; \
	} while(0);

#define M_ErrorWhileSaving() \
	do \
	{ \
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;\
		ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForSavingFailure; \
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_ErrorWhileSaving; \
		ps2INO_gst_SavManager.b_IsWorking = TRUE; \
	} while(0);

#define M_SuccessWhileSaving() \
	do \
	{ \
		if (ps2INO_gst_SavManager.i_ConfirmOnSuccess )\
		{\
			ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForSavingSuccess; \
			ps2INO_gst_SavManager.b_IsWorking = TRUE; \
		}\
		else\
		{\
			ps2INO_gst_SavManager.en_State = SavState_e_Inactive; \
				ps2INO_gst_SavManager.b_IsWorking = FALSE; \
		}\
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileSaving; \
	} while(0);

#define M_SuccessWhileLoading() \
	do \
	{ \
		if (ps2INO_gst_SavManager.i_ConfirmOnSuccess )\
		{\
			ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForLoadingSuccess; \
			ps2INO_gst_SavManager.b_IsWorking = TRUE; \
		}\
		else\
		{\
			ps2INO_gst_SavManager.en_State = SavState_e_Inactive; \
			ps2INO_gst_SavManager.b_IsWorking = FALSE; \
		}\
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileLoading; \
	} while(0);
	
#define M_Overwrite() \
	do \
	{ \
		ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForOverwrite; \
		ps2INO_gst_SavManager.b_IsWorking = TRUE; \
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_ConfirmOverwriting; \
	} while(0);
	
#define M_ConfirmSave( _b_BeforeMakeTree ) \
	do \
	{ \
		ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForConfirmSave; \
		ps2INO_gst_SavManager.b_IsWorking = TRUE; \
		ps2INO_gst_SavManager.b_ConfirmSaveBeforeMakeTree = _b_BeforeMakeTree;\
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_ConfirmSave; \
	} while(0);
	
	
#define M_GotoLowPrio( next_state )\
{\
	ps2INO_gst_SavManager.i_ForceLowPriority = TRUE;\
	ps2INO_gst_SavManager.en_StateAfterLowPrio = next_state;\
	ps2INO_gst_SavManager.en_State = SavState_e_WaitingLowPriority; \
	ps2INO_gst_SavManager.b_IsWorking = TRUE; \
}

char	*pCryptedBuff = NULL;
int 	TestMC_i_Result;
int		TestMC_i_Type;
char	INO_gsz_DeleteName[ 64 ];

#define M_FreeEncryptionBuffer()\
{\
	if(pCryptedBuff)\
	{\
		MEM_Free(pCryptedBuff);\
		pCryptedBuff = NULL;\
	}\
}

//#define M_Button( _i_button ) AI_EvalFunc_IoButtonJustPressed_C( _i_button )
#define M_Button( _i_button ) 	ps2INO_gst_SavManager.i_ButtonJustPressed[ _i_button ]

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$F
typedef struct
{
	unsigned char	Head[4];		/* 'P','S','2','D' 
	unsigned short	Reserv1;		/* Reserved area, must be filled entirely with 00 
	unsigned short	OffsLF;			/* Line break position in title name 
	unsigned		Reserv2;		/* Reserved area, must be filled entirely with 00 
	unsigned		TransRate;		/* Background transparency 
	_iconVu0IVECTOR BgColor[4];		/* Background color (4 points) 
	_iconVu0FVECTOR LightDir[3];	/* Light source direction (3 light sources) 
	_iconVu0FVECTOR LightColor[3];	/* Light source color (3 light sources) 
	_iconVu0FVECTOR Ambient;		/* Ambient light 
	unsigned char	TitleName[68];	/* Title name 
	unsigned char	FnameView[64];	/* List icon file name 
	unsigned char	FnameCopy[64];	/* Copy icon file name 
	unsigned char	FnameDel[64];	/* Deletion icon file name 
	unsigned char	Reserve3[512];	/* Reserved area, must be filled entirely with 00 
} sceMcIconSys;
*/

static sceMcIconSys ps2INO_sst_DefaultIconSys =
{
	{ 'P', 'S', '2', 'D' },             //Head      => "PS2D"
	0,                                  //Reserv1   => 0
	32,                                 //OffsLF    => 2xNumChar or 32 if none
	0,                                  //Reserv2   => 0
	0x0,                                //TransRate => 0 (transparent) to 0x80 (opaque)

	{   
	                                    //BgColor   => sceVu0IVECTOR format, in {r,g,b,-} order
	                                    //             each value in the range from 0 to 0xff
	    { 0x80, 0x00, 0x00, 0x00 },     //RGB/upper left     
	    { 0x00, 0x80, 0x00, 0x00 },     //RGB/upper right     
	    { 0x00, 0x00, 0x80, 0x00 },     //RGB/lower left 
	    { 0x80, 0x80, 0x80, 0x00 }      //RGB/lower right
	},

	{ 
	    { 0.50f, 0.5f, 0.5f, 0.0f },    //LightDir  
	    { 0.0f, -0.4f, -0.1f, 0.0f }, 
	    { -0.5f, -0.5f, 0.5f, 0.0f } 
	},

	{ 
		{ 0.50f, 0.50f, 0.50f, 0.00f },
		{ 0.50f, 0.50f, 0.50f, 0.00f },
		{ 0.50f, 0.50f, 0.50f, 0.00f }
	    //{ 0.48f, 0.48f, 0.03f, 0.00f }, //LightColor=> sceVu0IVECTOR format, in {r,g,b,-} order
	    //{ 0.50f, 0.33f, 0.20f, 0.00f }, 
	    //{ 0.14f, 0.14f, 0.38f, 0.00f } 
	},

	{ 0.50, 0.50, 0.50, 0.00 },         //Ambient
	{ 0 },                              //TitleName
	{ 0 },                              //FnameView
	{ 0 },                              //FnameCopy
	{ 0 },                              //FnameDel
	{ 0 }                               //Reserve3
};

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int gi_LowPriority;
int gi_HightPriority;

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

BOOL	ps2INO_gb_EnableAutoCheck = FALSE;
char	ps2INO_gsz_GameSaveName[128];
BOOL	ps2INO_gb_SavInterrupted = FALSE;


/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern BOOL LOA_gb_SpeedMode;

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef enum	ps2INO_tden_SavManagerState_
{
	SavState_e_Inactive								= 0,
	SavState_e_TestConnected,
	SavState_e_TestingConnected,
	SavState_e_WaitingEndSaveConfirmation,
	SavState_e_TestFormated,
	SavState_e_WaitingFormatConfirmation,
	SavState_e_Formating_1,
	SavState_e_Formating_2,
	SavState_e_TestData,
	SavState_e_WaitingOverwriteConfirmation,
	SavState_e_WaitingSaveConfirmation,
	SavState_e_GoingToRoot,
	SavState_e_GoingToHome,
	SavState_e_MakeTree,
	SavState_e_MakeTree2,
	SavState_e_OpeningIconFile,
	SavState_e_ClosingIconFile,
	SavState_e_OpeningIconSysFile,
	SavState_e_ClosingIconSysFile,
	SavState_e_OpenMainFile,
	SavState_e_WritingMainFile,
	SavState_e_ReadingMainFile,
	SavState_e_ClosingMainFile,
	SavState_e_MakingDir,
	SavState_e_GoingToHome2,
	SavState_e_DeleteDir,
	SavState_e_DeletingIconSys,
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
	SavState_e_WaitingUserResponseForNoSaving,
	SavState_e_WaitingUserResponseForOverwrite,
	SavState_e_WaitingUserResponseForConfirmSave,
	SavState_e_WaitingLowPriority,
	SavState_e_Dummy								= 0xFFFFFFFF
} ps2INO_tden_SavManagerState;

typedef enum	ps2INO_tden_SavCommand_
{
	SavCmd_e_None									= 0,
	SavCmd_e_ReadAllHeaders,
	SavCmd_e_Read,
	SavCmd_e_Write,
	SavCmd_e_BootupTest,
	SavCmd_e_TestMemCard,
	SavCmd_e_Dummy									= 0xFFFFFFFF
} ps2INO_tden_SavCommand;

typedef struct	ps2INO_tdst_SavManager_
{
	ps2INO_tden_SavCommand		en_UserCommand;
	INO_tden_SavUserMessage		en_UserMessage;
	ps2INO_tden_SavManagerState en_State;
	int							i_ConfirmOnSuccess;
	int							i_UserAction;
	
	/**/
	int							i_CurrentCardId;
	int							i_FileHandler;

	/* io operations */
	char						*p_Dest;
	int							i_DestSize;
	int							*pi_EffectiveDestSize;
	/**/
	BOOL						b_IsWorking;
	int							i_TimeOut;
	float						f_TimeTest;
	float						f_TimeBetween2MCTest;
	BOOL						b_Retry;
	BOOL						b_Delete;
	BOOL						b_CardHasChanged;
	int							i_DirExist;
	/**/
	BOOL						b_ChangingThreadPrio;
	int							i_CurrPriority;
	int							i_ForceLowPriority;
	ps2INO_tden_SavManagerState	en_StateAfterLowPrio;
	/**/
	int							i_sceMcSync_Result;
	int							i_sceMcxxxx_Result;
	/**/
	int							i_CardIsFormated;
	int							i_MemoryCardType;
	int							i_FreeSpace;
	/**/
	int							ai_CanSkipCheck[Cte_McCardIdNbMax];
	/* flags for writing */
	int							i_Writing_ReadIsDone;
	int							i_Writing_OverwriteOK;
	int							i_Writing_NoFile;
	BOOL						b_ConfirmSave;
	BOOL						b_ConfirmSaveBeforeMakeTree;
	BOOL						b_EncryptionError;
	BOOL						b_IconSysErased;
	BOOL						b_CreateIconSysAfterSave;
	/* value for button */
	int							i_ButtonPressed[4];
	int							i_ButtonJustPressed[4];
} ps2INO_tdst_SavManager;

ps2INO_tdst_SavManager	ps2INO_gst_SavManager;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
void M_ErrorNoMc(void) 
{
   	ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;
	ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForMcAbsent; 
	ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_NoCard; 
	ps2INO_gst_SavManager.b_IsWorking = TRUE; 
} 

void M_TestMC_Error(void)
{ 
    ps2INO_gst_SavManager.en_State = SavState_e_Inactive; 
    ps2INO_gst_SavManager.en_UserCommand = SavCmd_e_None;
    ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_NoCard; 
    ps2INO_gst_SavManager.b_IsWorking = FALSE; 
} 

int M_TestMemCardWhileWaitingUser()
{
	int i_Res;
	ps2INO_gst_SavManager.f_TimeBetween2MCTest -= TIM_gf_dt;
	
	ps2INO_gst_SavManager.b_CardHasChanged = FALSE;
	if ( sceMcSync(sceMcSync_ModeNoWait, NULL, &TestMC_i_Result) ==  sceMcExecRun )
		return 1;
	
	if (ps2INO_gst_SavManager.f_TimeBetween2MCTest < 0.0f)
	{
		if ( sceMcGetInfo
			(
				ps2INO_gst_SavManager.i_CurrentCardId,
				Cte_DefaultSlotNb,
				&TestMC_i_Type,
				NULL,
				NULL
			) 
		 )
		 return 1;
		 
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;
		
		i_Res = sceMcSync(sceMcSync_ModeWait, NULL, &TestMC_i_Result);
		if (TestMC_i_Result == sceMcResChangedCard)
		{
			ps2INO_gst_SavManager.b_CardHasChanged = TRUE;
			TestMC_i_Result = sceMcResSucceed;
		}
		if (TestMC_i_Result == sceMcResNoFormat)
		{
			ps2INO_gst_SavManager.b_CardHasChanged = TRUE;
			TestMC_i_Result = sceMcResSucceed;
		}
		if( (TestMC_i_Result != sceMcResSucceed) || (TestMC_i_Type != sceMcTypePS2) )
		{		
			M_FreeEncryptionBuffer();
			M_ErrorNoMc();
			return 0;
		}
	}
	return 1;
}

void M_TestMemCardWhileNoCard()
{
	ps2INO_gst_SavManager.f_TimeBetween2MCTest -= TIM_gf_dt;
	
	if ( sceMcSync(sceMcSync_ModeNoWait, NULL, &TestMC_i_Result) ==  sceMcExecRun )
		return;
	
	if (ps2INO_gst_SavManager.f_TimeBetween2MCTest < 0.0f)
	{
		if ( sceMcGetInfo
			(
				ps2INO_gst_SavManager.i_CurrentCardId,
				Cte_DefaultSlotNb,
				&TestMC_i_Type,
				NULL,
				NULL
			) 
		)
		return;
		
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;
			
		sceMcSync(sceMcSync_ModeWait, NULL, &TestMC_i_Result);
		
		if (TestMC_i_Type == sceMcTypePS2)
		{
			switch( TestMC_i_Result )
			{
			case sceMcResSucceed:	
				M_Retrying(); break;
			case sceMcResChangedCard:
			case sceMcResNoFormat:
				if ( (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Read) || (ps2INO_gst_SavManager.b_Delete) )
					M_ResetAll()
				else
					M_Retrying()
				break;
			}
		}
		/*
		if( (TestMC_i_Result == sceMcResSucceed) && (TestMC_i_Type == sceMcTypePS2) )
		{		
			if ( (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Read) || (ps2INO_gst_SavManager.b_Delete) )
				M_ResetAll()
			else
				M_Retrying()
		}
		*/
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2INO_SavModuleInit(void)
{
	sceMcInit();

	if(LOA_gb_SpeedMode)
	{
		gi_LowPriority = IOP_Cte_ThPriority_SoundEventLow;
		gi_HightPriority = IOP_Cte_ThPriority_SoundEventBin;
	}
	else
	{
		gi_LowPriority = IOP_Cte_ThPriority_SoundEventLow;
		gi_HightPriority = IOP_Cte_ThPriority_SoundEvent;
	}

	L_memset(&ps2INO_gst_SavManager, 0, sizeof(ps2INO_tdst_SavManager));
	ps2INO_gst_SavManager.i_CurrentCardId = 0;
	ps2INO_gst_SavManager.i_CurrPriority = Cte_LowTheadPrio;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2INO_SavModuleClose(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ps2INO_b_ChangePriority(void)
{
    static int SND_TrackPauseAll_done = 0;
    
    if (ps2INO_gst_SavManager.i_ForceLowPriority) 
    {
    	if( ps2INO_gst_SavManager.i_CurrPriority != Cte_LowTheadPrio)
    	{
	    	M_dbg_printf(("--low prio--\n"));
			sceMcChangeThreadPriority(Cte_LowTheadPrio);
			ps2INO_gst_SavManager.i_CurrPriority = Cte_LowTheadPrio;
			ps2INO_gst_SavManager.b_ChangingThreadPrio = TRUE;
				
			if(SND_TrackPauseAll_done == 1)
			{
			    SND_TrackPauseAll_done = 0;
	            ps2SND_i_ChangePriority(-2);
	            SND_TrackPauseAll(FALSE);
			}
    	}
		return;
    }
    
	switch(ps2INO_gst_SavManager.en_UserCommand)
	{
	case SavCmd_e_Write:
		if(ps2INO_gst_SavManager.i_CurrPriority != Cte_HighTheadPrio)
		{
		    M_dbg_printf(("--higth prio--\n"));
			sceMcChangeThreadPriority(Cte_HighTheadPrio);
			ps2INO_gst_SavManager.i_CurrPriority = Cte_HighTheadPrio;
			ps2INO_gst_SavManager.b_ChangingThreadPrio = TRUE;
			
			SND_TrackPauseAll_done = 1;
            SND_TrackPauseAll(TRUE);
            ps2SND_i_ChangePriority(-1);

		}
		break;
	
	
	case SavCmd_e_Read:
	case SavCmd_e_ReadAllHeaders:
	case SavCmd_e_BootupTest:
    case SavCmd_e_TestMemCard:
    
		if(ps2INO_gst_SavManager.i_CurrPriority != Cte_HighTheadPrio)
		{
		    M_dbg_printf(("--higth prio--\n"));
			sceMcChangeThreadPriority(Cte_HighTheadPrio);
			ps2INO_gst_SavManager.i_CurrPriority = Cte_HighTheadPrio;
			ps2INO_gst_SavManager.b_ChangingThreadPrio = TRUE;
		}
		break;

	    
	default:
		if(ps2INO_gst_SavManager.i_CurrPriority != Cte_LowTheadPrio)
		{
		    M_dbg_printf(("--low prio--\n"));
			sceMcChangeThreadPriority(Cte_LowTheadPrio);
			ps2INO_gst_SavManager.i_CurrPriority = Cte_LowTheadPrio;
			ps2INO_gst_SavManager.b_ChangingThreadPrio = TRUE;
			
			if(SND_TrackPauseAll_done == 1)
			{
			    SND_TrackPauseAll_done = 0;
                ps2SND_i_ChangePriority(-2);
                SND_TrackPauseAll(FALSE);
			}
		}
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2INO_SavStop( void )
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2INO_SavReStart( void )
{
	ps2INO_gb_SavInterrupted = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2INO_SavUpdate(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int			i, i_Result, i_Interrupt;
	int			bt_ok, bt_cancel;
	int			fd2, size;
	static char *p_tmp = NULL;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

    // check param (blindax)
	if (ps2INO_gst_SavManager.f_TimeTest > 5.0f)
		    ps2INO_gst_SavManager.f_TimeTest = 5.0f;
	
	i_Interrupt = ps2INO_gb_SavInterrupted;
	ps2INO_gb_SavInterrupted = FALSE;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    handle the change priority sequence
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(ps2INO_gst_SavManager.b_ChangingThreadPrio)
	{
		/* check in progress or finished */
		if(sceMcSync(sceMcSync_ModeNoWait, NULL, &i_Result) == sceMcExecRun) 
		{
		   M_dbg_printf(("-> changing prio\n"));
           return;	/* in progress => wait next call */		    
		}


		/* finished => register the result & skip this frame */
		ps2INO_gst_SavManager.b_ChangingThreadPrio = FALSE;
		return; /* skip this time */
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    then check the result
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	switch(sceMcSync(sceMcSync_ModeNoWait, NULL, &ps2INO_gst_SavManager.i_sceMcSync_Result))
	{
	case sceMcExecRun:		/* in progress */
		if(ps2INO_gst_SavManager.i_TimeOut++ > Cte_TimeOut)
		{					/* time is out */
		
			M_dbg_printf(("** time out **\n"));
			ps2INO_gst_SavManager.i_TimeOut = 0;
			switch(ps2INO_gst_SavManager.en_UserCommand)
			{
			case SavCmd_e_Write:
				M_FreeEncryptionBuffer();			
				M_ErrorWhileSaving();
				break;

			case SavCmd_e_ReadAllHeaders:
			case SavCmd_e_Read:
				M_FreeEncryptionBuffer();			
				M_ErrorWhileLoading();
				break;

			default:
				M_FreeEncryptionBuffer();						
				M_ErrorNoMc();
				break;
			}				
				
			M_FreeEncryptionBuffer();
		}

		return;

	case sceMcExecIdle:		/* nothing todo */
		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_b_ChangePriority();
		break;

	case sceMcExecFinish:	/* last command finished */
		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_b_ChangePriority();
		break;

	default:				/* error */
	    M_dbg_printf(("***error***\n"));
		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_b_ChangePriority();
		switch(ps2INO_gst_SavManager.en_UserCommand)
		{
		case SavCmd_e_Write:
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			break;

		case SavCmd_e_ReadAllHeaders:
		case SavCmd_e_Read:
			M_FreeEncryptionBuffer();		
			M_ErrorWhileLoading();
			break;

		default:
			M_FreeEncryptionBuffer();					
			M_ErrorNoMc();
			break;
		}
		
		M_FreeEncryptionBuffer();

		return;
	}

	if(ps2INO_gst_SavManager.b_ChangingThreadPrio) 
	{
	    M_dbg_printf(("return change prio\n"));
	    return;
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 Update button state
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */
	 for (i = 0; i < 4; i++)
	 {
	 	ps2INO_gst_SavManager.i_ButtonJustPressed[ i ] = 0;
	 	if(AI_EvalFunc_IoButtonJustPressed_C( i ))
	 	{
	 		if (!ps2INO_gst_SavManager.i_ButtonPressed[ i ] )
	 		{
	 			ps2INO_gst_SavManager.i_ButtonJustPressed[ i ] = 1;
	 			//M_dbg_printf(( "bouton :%d\n", i ));
	 		}
	 		ps2INO_gst_SavManager.i_ButtonPressed[ i ] = 1;
	 	}
	 	else
	 		ps2INO_gst_SavManager.i_ButtonPressed[ i ] = 0;
	 }

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	switch(ps2INO_gst_SavManager.en_State)
	{
	case SavState_e_WaitingLowPriority:
		if (ps2INO_gst_SavManager.en_StateAfterLowPrio == SavState_e_WaitingUserResponseForSavingSuccess)
		{
			M_SuccessWhileSaving()
		}
		if (ps2INO_gst_SavManager.en_StateAfterLowPrio == SavState_e_WaitingUserResponseForLoadingSuccess)
		{
			M_SuccessWhileLoading()
		}
		
		break;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForFormatFailure:
	    //M_dbg_printf(("--wainting user\n"));
		if(M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			M_Retrying();
		}
		else if(M_Button(eBtn_Triangle))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Triangle;
			ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
			ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForFormatSuccess:
		M_dbg_printf(("--waiting 2s after format success\n"));
		if (ps2INO_gst_SavManager.f_TimeTest > 0.0f)
		{
			ps2INO_gst_SavManager.f_TimeTest -= TIM_gf_dt;
			return;
		}
		M_Retrying();
		
		/*
		if(M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			M_Retrying();
		}
		*/
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForUnformatted:
		//M_dbg_printf(("--waiting user\n"));
		
		/* sav interrupted */
		if ( i_Interrupt )
		{
			M_Retrying();
			break;
		}
		/* Test Memory Card presence */		
		if ( !M_TestMemCardWhileWaitingUser() )
			break;
		
		/* user action */
		if(M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			M_Retrying();
		}
		else if(M_Button(eBtn_Circle))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Circle;
			ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_FormatConfirmation;
			ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForFormat;
		}
		else if (M_Button(eBtn_Triangle))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Triangle;
			ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
			ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForCancelSave:
		//M_dbg_printf(("--wainting user\n"));
		if (M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			M_ResetAll();
		}
		else if ( M_Button(eBtn_Triangle) )
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Triangle;
			M_Retrying();
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForFormat:
		//M_dbg_printf(("--wainting user\n"));
		/* sav interrupted */
		if ( i_Interrupt )
		{
			M_Retrying();
			break;
		}
		/* Test Memory Card presence */
		if ( !M_TestMemCardWhileWaitingUser() )
			break;
		
		if(M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Formating;
			ps2INO_gst_SavManager.en_State = SavState_e_Formating_1;
		}
		else if(M_Button(eBtn_Triangle))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Triangle;
			M_Retrying();
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_Formating_1:
		if
		(
			(ps2INO_gst_SavManager.i_MemoryCardType == sceMcResChangedCard)
		||	(ps2INO_gst_SavManager.i_MemoryCardType != sceMcTypePS2)
		)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileFormating();
		}
		else
		{
			ps2INO_gst_SavManager.en_State = SavState_e_Formating_2;
			ps2INO_gst_SavManager.i_TimeOut = 0;
			ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcFormat
				(
					ps2INO_gst_SavManager.i_CurrentCardId,
					Cte_DefaultSlotNb
				);
				
			M_dbg_printf(("sceMcFormat\n"));
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_Formating_2:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileFormating();
		}
		else
		{
			M_SuccessWhileFormating();
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForMcAbsent:
		//M_dbg_printf(("--wainting user\n"));
		// save interrupted ?
		if ( i_Interrupt )
		{
			M_Retrying();
			break;
		}
		// test card
		M_TestMemCardWhileNoCard();
		
		// user action
		if ( ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_BootupTest )
		{
			bt_ok = eBtn_Cross;
			bt_cancel = eBtn_Triangle;
		}
		else
		{
			bt_ok = eBtn_Triangle;
			bt_cancel = eBtn_Cross;
		}
			
			
		if(M_Button(bt_cancel))
		{
			ps2INO_gst_SavManager.i_UserAction = bt_cancel;
			if ( (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Read) || (ps2INO_gst_SavManager.b_Delete) )
				M_ResetAll()
			else
				M_Retrying();
		}
		else if (M_Button(bt_ok))
		{
			if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
				ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
			}
			else
			{		
				ps2INO_gst_SavManager.i_UserAction = bt_ok;
				M_ResetAll();
			}
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForNoSpace:
		//M_dbg_printf(("--wainting user\n"));
		// save interrupted ?
		if ( i_Interrupt )
		{
			M_Retrying();
			break;
		}
		/* Test Memory Card presence */
		if ( !M_TestMemCardWhileWaitingUser() )
			break;
		
		if ( ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_BootupTest )
		{
			bt_ok = eBtn_Cross;
			bt_cancel = eBtn_Triangle;
		}
		else
		{
			bt_ok = eBtn_Triangle;
			bt_cancel = eBtn_Cross;
		}
				
		if(M_Button( bt_cancel ))
		{
			ps2INO_gst_SavManager.i_UserAction = bt_cancel;
			M_Retrying();
		}
		else if (M_Button(bt_ok))
		{
			ps2INO_gst_SavManager.i_UserAction = bt_ok;
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
				ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
			}
			else
			{
				M_ResetAll();
			}
		}
		break;
	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForNoSaving:
		//M_dbg_printf(("--wainting user\n"));
		
		// save interrupted ?
		if ( i_Interrupt )
		{
			M_Retrying();
			break;
		}
		/* Test Memory Card presence */
		if ( !M_TestMemCardWhileWaitingUser() )
			break;
				
		if(M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			M_ResetAll();
		}
		else if (M_Button(eBtn_Triangle))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Triangle;
			M_Retrying();
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForSavingFailure:
		//M_dbg_printf(("--wainting user\n"));
		if(M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			M_Retrying();
		}
		else if (M_Button(eBtn_Triangle))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Triangle;
			ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
			ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForLoadingFailure:
		//M_dbg_printf(("--wainting user\n"));
		if(M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_ReadAllHeaders)
			{
				M_Retrying();
			}
			else
			{
				M_ResetAll();
			}
		}
		else if (M_Button(eBtn_Triangle))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Triangle;
			M_ResetAll();
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForLoadingSuccess:
	case SavState_e_WaitingUserResponseForSavingSuccess:
		//M_dbg_printf(("--wainting user\n"));
		if (M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			M_ResetAll();
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForOverwrite:
		//M_dbg_printf(("--wainting user\n"));
		// save interrupted ?
		if ( i_Interrupt )
		{
			M_Retrying();
			break;
		}
		// test card
		if ( !M_TestMemCardWhileWaitingUser() )
			break;
		
		// user action
		if (M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			ps2INO_gst_SavManager.i_sceMcxxxx_Result = 0;
			ps2INO_gst_SavManager.i_sceMcSync_Result = 0;
			ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Saving;
			ps2INO_gst_SavManager.i_Writing_OverwriteOK = 1;
			ps2INO_gst_SavManager.en_State = SavState_e_OpenMainFile;
		}
		else if (M_Button(eBtn_Triangle))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Triangle;
			ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
			ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WaitingUserResponseForConfirmSave:
		//M_dbg_printf(("--wainting user\n"));
		// save interrupted ?
		if ( i_Interrupt )
		{
			M_Retrying();
			break;
		}
		// test card
		if ( !M_TestMemCardWhileWaitingUser() )
			break;
		
		// user action
		if (M_Button(eBtn_Cross))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Cross;
			ps2INO_gst_SavManager.i_sceMcxxxx_Result = 0;
			ps2INO_gst_SavManager.i_sceMcSync_Result = 0;
			ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Saving;
			ps2INO_gst_SavManager.b_ConfirmSave = TRUE;\
				if ( ps2INO_gst_SavManager.b_ConfirmSaveBeforeMakeTree )
			{
				ps2INO_gst_SavManager.en_State = SavState_e_MakeTree;
				ps2INO_gst_SavManager.b_ConfirmSaveBeforeMakeTree = FALSE;
			}
			else
				ps2INO_gst_SavManager.en_State = SavState_e_OpenMainFile;
		}
		else if (M_Button(eBtn_Triangle))
		{
			ps2INO_gst_SavManager.i_UserAction = eBtn_Triangle;
			ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_CancelSave;
			ps2INO_gst_SavManager.en_State = SavState_e_WaitingUserResponseForCancelSave;
		}
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_Inactive:
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_TestConnected:
		// wait a little (to allow user to read the message )
		if (ps2INO_gst_SavManager.f_TimeTest > 0.0f)
		{
			if ( (ps2INO_gst_SavManager.en_UserCommand != SavCmd_e_TestMemCard) && (ps2INO_gst_SavManager.en_UserCommand != SavCmd_e_BootupTest) )
			{
				if ( !M_TestMemCardWhileWaitingUser() )
					break;
			}
			ps2INO_gst_SavManager.f_TimeTest -= TIM_gf_dt;
			return;
		}
			
		/* get info */
		ps2INO_gst_SavManager.i_FreeSpace = -1;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcGetInfo
			(
				ps2INO_gst_SavManager.i_CurrentCardId,
				Cte_DefaultSlotNb,
				&ps2INO_gst_SavManager.i_MemoryCardType,
				&ps2INO_gst_SavManager.i_FreeSpace,
				&ps2INO_gst_SavManager.i_CardIsFormated
			);
		M_dbg_printf(("sceMcGetInfo\n"));
		
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result != sceMcResSucceed)
		{
			if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_TestMemCard)
			{
				M_TestMC_Error();
			}
			/*
			else if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				M_FreeEncryptionBuffer();			
				M_ErrorWhileSaving();	
			}
			*/
			else
			{
				M_FreeEncryptionBuffer();			
				M_ErrorNoMc();
			}
		}
		else
		{
			ps2INO_gst_SavManager.en_State = SavState_e_TestingConnected;
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_TestingConnected:
		if (ps2INO_gst_SavManager.f_TimeTest > 0.0f)
		{
			ps2INO_gst_SavManager.f_TimeTest -= TIM_gf_dt;
			return;
		}
		switch(ps2INO_gst_SavManager.i_MemoryCardType)
		{
		case sceMcTypeNoCard:
			if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_TestMemCard)
			{
				M_TestMC_Error();
			}
			/*
			else if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				M_FreeEncryptionBuffer();			
				M_ErrorWhileSaving();	
			}
			else if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Read)
			{
				M_FreeEncryptionBuffer();						
				M_ErrorWhileLoading();	
			}
			*/
			else
			{
				M_FreeEncryptionBuffer();						
				M_ErrorNoMc();
			}
			return;

		case sceMcTypePS2:
			if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_TestMemCard)
			{
				M_TestMC_Success();
				return;
			}
			break;

		case sceMcTypePS1:
		case sceMcTypePDA:
		default:
			if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_TestMemCard)
			{
				M_TestMC_Error();
			}
			/*
			else if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				M_FreeEncryptionBuffer();						
				M_ErrorWhileSaving();	
			}
			*/
			else
			{
				M_FreeEncryptionBuffer();						
				M_ErrorNoMc();
			}
			return;
		}
		if(ps2INO_gst_SavManager.i_CardIsFormated == 0)
		{
			switch (ps2INO_gst_SavManager.en_UserCommand)
			{
			case SavCmd_e_BootupTest:
				M_FreeEncryptionBuffer();						
				M_ResetAll()
				break;
			case SavCmd_e_ReadAllHeaders:
				M_FreeEncryptionBuffer();						
				M_ErrorNoSaveOnMc()
				break;
			default:
				M_FreeEncryptionBuffer();						
				M_ErrorUnformattedMc()
			}
		}
		else
		{
			ps2INO_gst_SavManager.i_FreeSpace *= 1024;
			ps2INO_gst_SavManager.i_TimeOut = 0;
			ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcChdir
				(
					ps2INO_gst_SavManager.i_CurrentCardId,
					Cte_DefaultSlotNb,
					"/",
					0
				);
			M_dbg_printf(("sceMcChDir\n"));	
			ps2INO_gst_SavManager.en_State = SavState_e_GoingToRoot;
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_GoingToRoot:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				M_FreeEncryptionBuffer();						
				M_ErrorWhileSaving();
			}
			else if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_BootupTest)
			{
				M_FreeEncryptionBuffer();						
				M_ResetAll()
			}
			else
			{
				M_FreeEncryptionBuffer();						
				M_ErrorWhileLoading();
			}
			return;
		}

		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcChdir
			(
				ps2INO_gst_SavManager.i_CurrentCardId,
				Cte_DefaultSlotNb,
				ps2INO_gsz_GameSaveName,
				0
			);
		M_dbg_printf(("sceMcChDir\n"));
		ps2INO_gst_SavManager.en_State = SavState_e_GoingToHome;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_GoingToHome:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
			/* the home directory doesn't exist check the free space */
			if(ps2INO_gst_SavManager.i_FreeSpace < ps2INO_i_SavGetMinSpace())
			{
				if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_ReadAllHeaders )
				{
					M_FreeEncryptionBuffer();				
					M_ErrorNoSaveOnMc()
				}
				else
				{	
					M_FreeEncryptionBuffer();							
					M_ErrorNoMoreSpaceOnMc();
				}
			}
			else if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				if (ps2INO_gst_SavManager.b_Retry && !ps2INO_gst_SavManager.b_ConfirmSave)
				{
					M_ConfirmSave(TRUE)
				}
				else
				{
					ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Saving;
					ps2INO_gst_SavManager.en_State = SavState_e_MakeTree;
				}
			}
			else if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_BootupTest)
			{
				M_ResetAll()
			}
			else
			{
				M_FreeEncryptionBuffer();						
				M_ErrorNoSaveOnMc();
			}
		}
		else
		{
		 	if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_BootupTest)
			{
				M_ResetAll()
			}
			else
			{
				ps2INO_gst_SavManager.i_TimeOut = 0;
				ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcOpen
					(
						ps2INO_gst_SavManager.i_CurrentCardId,
						Cte_DefaultSlotNb,
						Csz_IconViewFileName,
						SCE_RDONLY
					);
				M_dbg_printf(("sceMcOpen\n"));	
				ps2INO_gst_SavManager.en_State = SavState_e_OpeningIconFile;
			}
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_OpeningIconFile:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || (ps2INO_gst_SavManager.i_sceMcSync_Result < 0))
		{
			/* cannot open main file */
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				ps2INO_gst_SavManager.en_State = SavState_e_MakeTree;
			}
			else
			{
				M_FreeEncryptionBuffer();						
				M_ErrorNoSaveOnMc();
			}
		}
		else
		{
			/* close main file */
			ps2INO_gst_SavManager.i_TimeOut = 0;
			ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcClose(ps2INO_gst_SavManager.i_sceMcSync_Result);
			M_dbg_printf(("sceMcClose\n"));
			ps2INO_gst_SavManager.en_State = SavState_e_ClosingIconFile;
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_ClosingIconFile:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				M_FreeEncryptionBuffer();						
				M_ErrorWhileSaving();
			}
			else
			{
				M_FreeEncryptionBuffer();						
				M_ErrorWhileLoading();
			}
		}
		else
		{
			/* open icon sys file */
			ps2INO_gst_SavManager.i_TimeOut = 0;
			ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcOpen
				(
					ps2INO_gst_SavManager.i_CurrentCardId,
					Cte_DefaultSlotNb,
					Csz_IconSysFileName,
					SCE_RDONLY
				);
			M_dbg_printf(("sceMcOpen\n"));	
			ps2INO_gst_SavManager.en_State = SavState_e_OpeningIconSysFile;
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_OpeningIconSysFile:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || (ps2INO_gst_SavManager.i_sceMcSync_Result < 0))
		{
			/* cannot open main file */
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				ps2INO_gst_SavManager.en_State = SavState_e_MakeTree;
			}
			else
			{
				M_FreeEncryptionBuffer();						
				M_ErrorWhileLoading();
			}
		}
		else
		{
			/* close main file */
			ps2INO_gst_SavManager.i_TimeOut = 0;
			ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcClose(ps2INO_gst_SavManager.i_sceMcSync_Result);
			M_dbg_printf(("sceMcClose\n"));
			ps2INO_gst_SavManager.en_State = SavState_e_ClosingIconSysFile;
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_ClosingIconSysFile:
	case SavState_e_OpenMainFile:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				M_FreeEncryptionBuffer();						
				M_ErrorWhileSaving();
			}
			else
			{
				M_FreeEncryptionBuffer();						
				M_ErrorWhileLoading();
			}
		}
		else
		{
			/* go back root */
			ps2INO_gst_SavManager.i_TimeOut = 0;
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				int i_Open;
				if (ps2INO_gst_SavManager.i_Writing_ReadIsDone)
				{
					if (ps2INO_gst_SavManager.b_Retry && !ps2INO_gst_SavManager.b_ConfirmSave)
					{
						M_ConfirmSave(FALSE)
						break;
					}
					
					/* test overwrite : look if we have to display overwrite message */
					if ( !ps2INO_gst_SavManager.i_Writing_OverwriteOK )
					{
						if ( INO_i_Sav_SlotIsEmpty( &INO_gst_SavManager.ast_SlotDesc[ INO_gst_SavManager.i_CurrentSlot ] ) )
							ps2INO_gst_SavManager.i_Writing_OverwriteOK = 1;
						else if (INO_i_Sav_SlotIsEmpty(INO_gst_SavManager.p_Temp + INO_gst_SavManager.i_CurrentSlot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize) ))
							ps2INO_gst_SavManager.i_Writing_OverwriteOK = 1;
						if ( !ps2INO_gst_SavManager.i_Writing_OverwriteOK )
						{
							M_Overwrite()
							break;
						}
						else
							ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Saving;
					}
					
					/* 10068 */
					if (!ps2INO_gst_SavManager.b_IconSysErased )
					{
						ps2INO_gst_SavManager.b_IconSysErased = TRUE;
						sprintf( INO_gsz_DeleteName, "/%s/%s", ps2INO_gsz_GameSaveName, Csz_IconSysFileName );
						ps2INO_gst_SavManager.i_TimeOut = 0;
						ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcDelete
						(
							ps2INO_gst_SavManager.i_CurrentCardId,
							Cte_DefaultSlotNb,
							INO_gsz_DeleteName
						);
						ps2INO_gst_SavManager.en_State = SavState_e_DeletingIconSys;
						return;
					}
					/* 10068 */
					
					if (ps2INO_gst_SavManager.i_Writing_NoFile)
						i_Open = SCE_WRONLY | SCE_CREAT;
					else
						i_Open = SCE_WRONLY;
				}
				else
					i_Open = SCE_RDONLY;
				
				ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcOpen
					(
						ps2INO_gst_SavManager.i_CurrentCardId,
						Cte_DefaultSlotNb,
						ps2INO_gsz_GameSaveName,
						i_Open
					);
				M_dbg_printf(("sceMcOpen\n"));
			}
			else
			{
				if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Read)
					ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Loading;
				
				ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcOpen
					(
						ps2INO_gst_SavManager.i_CurrentCardId,
						Cte_DefaultSlotNb,
						ps2INO_gsz_GameSaveName,
						SCE_RDONLY
					);
				M_dbg_printf(("sceMcOpen\n"));
			}

			ps2INO_gst_SavManager.en_State = SavState_e_OpeningMainFile;
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_OpeningMainFile:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || (ps2INO_gst_SavManager.i_sceMcSync_Result < 0))
		{
			/* cannot open main file */
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				if ( !ps2INO_gst_SavManager.i_Writing_ReadIsDone )
				{
					ps2INO_gst_SavManager.i_sceMcxxxx_Result = 0;
					ps2INO_gst_SavManager.i_sceMcSync_Result = 0;
					ps2INO_gst_SavManager.i_Writing_ReadIsDone = 1;
					ps2INO_gst_SavManager.i_Writing_NoFile = SCE_CREAT;
					ps2INO_gst_SavManager.en_State = SavState_e_OpenMainFile;
				}
				else
				{
					M_FreeEncryptionBuffer();				
					M_ErrorWhileSaving();
				}
			}
			else
			{
				//M_ErrorNoSaveOnMc();
				M_FreeEncryptionBuffer();				
				M_ErrorWhileLoading();
			}
		}
		else if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
		{
			/* write : load all data if not done */
			if ( !ps2INO_gst_SavManager.i_Writing_ReadIsDone )
			{
				ps2INO_gst_SavManager.i_FileHandler = ps2INO_gst_SavManager.i_sceMcSync_Result;
				ps2INO_gst_SavManager.p_Dest = INO_gst_SavManager.p_Temp;
				ps2INO_gst_SavManager.i_DestSize = INO_gst_SavManager.i_TempSize;
			
				// Decryptation
				{
					M_FreeEncryptionBuffer();
					
					pCryptedBuff = (char *)MEM_p_AllocAlign(ps2INO_gst_SavManager.i_DestSize + ENCRYPT_ADD_SIZE , 64);
			
					ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcRead
						(
							ps2INO_gst_SavManager.i_FileHandler,
							pCryptedBuff,
							ps2INO_gst_SavManager.i_DestSize + ENCRYPT_ADD_SIZE
						);
						
				}
				M_dbg_printf(("sceMcRead\n"));	
				ps2INO_gst_SavManager.en_State = SavState_e_ReadingMainFile;
				break;
			}
			else
			{
				/* Reading over .. decrypt data */
				if(pCryptedBuff)
				{					
					if(BIG_b_IsBufferCrypted(pCryptedBuff))
					{											
						BIG_special_Decryption(&ps2INO_gst_SavManager.p_Dest, &pCryptedBuff, NULL, ps2INO_gst_SavManager.i_DestSize + ENCRYPT_ADD_SIZE, INO_gst_SavManager.ul_CRC);				
						BIG_ComputeCRC(&INO_gst_SavManager.ul_CRC[6], ps2INO_gst_SavManager.p_Dest, ps2INO_gst_SavManager.i_DestSize);
												
						/* CRC Check */
						if(INO_gst_SavManager.ul_CRC[0] != INO_gst_SavManager.ul_CRC[6])
						{
							M_ErrorWhileLoading();											
						}
					}
					else
					{					
						L_memcpy(ps2INO_gst_SavManager.p_Dest, pCryptedBuff, ps2INO_gst_SavManager.i_DestSize);
					}
					
					M_FreeEncryptionBuffer();
				}
			}
			
			/* copy header */			
			L_memcpy
			(
				INO_gst_SavManager.p_Temp + INO_gst_SavManager.i_CurrentSlot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
				&INO_gst_SavManager.ast_SlotDesc[INO_gst_SavManager.i_CurrentSlot],
				INO_Cte_SavHeaderSize
			);

			/* copy data */
			L_memcpy
			(
				INO_gst_SavManager.p_Temp + INO_gst_SavManager.i_CurrentSlot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize) + INO_Cte_SavHeaderSize,
				INO_gst_SavManager.ac_CurrSlotBuffer,
				INO_Cte_SavOneSlotMaxSize
			);
			
			ps2INO_gst_SavManager.p_Dest = INO_gst_SavManager.p_Temp;
			ps2INO_gst_SavManager.i_DestSize = INO_gst_SavManager.i_TempSize;

			ps2INO_gst_SavManager.i_FileHandler = ps2INO_gst_SavManager.i_sceMcSync_Result;
			
			/* Encryption */
			{
				char	ac_Key[ENCRYPT_KEY_SIZE + ENCRYPT_CRC_SIZE];							
				
				// Compute Encrypting Key ...
				BIG_ComputeKey(ac_Key, ps2INO_gst_SavManager.p_Dest, ps2INO_gst_SavManager.i_DestSize);
				
				M_FreeEncryptionBuffer();
				
				// Encryption ...
				pCryptedBuff = (char *)MEM_p_AllocAlign(ps2INO_gst_SavManager.i_DestSize + ENCRYPT_ADD_SIZE, 64);
				
				BIG_special_Encryption(&pCryptedBuff, &ps2INO_gst_SavManager.p_Dest, ac_Key, ps2INO_gst_SavManager.i_DestSize, TRUE);
													
				ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcWrite
					(
						ps2INO_gst_SavManager.i_FileHandler,
						pCryptedBuff,
						ps2INO_gst_SavManager.i_DestSize + ENCRYPT_ADD_SIZE
					);
					
				M_dbg_printf(("sceMcWrite\n"));	
				ps2INO_gst_SavManager.en_State = SavState_e_WritingMainFile;
					
													
			}
		}
		else if ( (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Read) || (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_ReadAllHeaders ) )
		{
			/* read */
			ps2INO_gst_SavManager.i_FileHandler = ps2INO_gst_SavManager.i_sceMcSync_Result;
			ps2INO_gst_SavManager.p_Dest = INO_gst_SavManager.p_Temp;
			ps2INO_gst_SavManager.i_DestSize = INO_gst_SavManager.i_TempSize;
			
			// Decryptation
			{
				M_FreeEncryptionBuffer();
				
				pCryptedBuff = (char *)MEM_p_AllocAlign(ps2INO_gst_SavManager.i_DestSize + ENCRYPT_ADD_SIZE, 64);
			
				ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcRead
					(
						ps2INO_gst_SavManager.i_FileHandler,
						pCryptedBuff,
						ps2INO_gst_SavManager.i_DestSize + ENCRYPT_ADD_SIZE
					);
																	
			}
			
			M_dbg_printf(("sceMcRead\n"));	
			ps2INO_gst_SavManager.en_State = SavState_e_ReadingMainFile;			
		}
		else
		{
			/* close main file */
			ps2INO_gst_SavManager.i_TimeOut = 0;
			ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcClose(ps2INO_gst_SavManager.i_sceMcSync_Result);
			M_dbg_printf(("sceMcClose\n"));	
			ps2INO_gst_SavManager.en_State = SavState_e_ClosingMainFile;
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WritingMainFile:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || (ps2INO_gst_SavManager.i_sceMcSync_Result <= 0))
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		M_FreeEncryptionBuffer();
		if (ps2INO_gst_SavManager.pi_EffectiveDestSize)
			*ps2INO_gst_SavManager.pi_EffectiveDestSize = ps2INO_gst_SavManager.i_sceMcSync_Result;

		/* close main file */
		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcClose(ps2INO_gst_SavManager.i_FileHandler);
		M_dbg_printf(("sceMcClose\n"));	
		ps2INO_gst_SavManager.en_State = SavState_e_ClosingMainFile;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_ReadingMainFile:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || (ps2INO_gst_SavManager.i_sceMcSync_Result <= 0))
		{
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				// reading while writing means that we are trying to get data and there is none
				if(ps2INO_gst_SavManager.i_sceMcxxxx_Result)
				{
					M_FreeEncryptionBuffer();				
					M_ErrorWhileSaving();
				}
				else
				{
					ps2INO_gst_SavManager.i_sceMcxxxx_Result = 0;
					ps2INO_gst_SavManager.i_sceMcSync_Result = 0;
					ps2INO_gst_SavManager.i_Writing_ReadIsDone = 1;
					ps2INO_gst_SavManager.i_Writing_NoFile = SCE_CREAT;
					ps2INO_gst_SavManager.en_State = SavState_e_OpenMainFile;
				}
			}
			else
			{
				M_FreeEncryptionBuffer();			
				M_ErrorWhileLoading();
			}
			return;
		}
		
		/*	Management of the crypted buffer */
		if(pCryptedBuff)
		{			
			if(BIG_b_IsBufferCrypted(pCryptedBuff))
			{											
				BIG_special_Decryption(&ps2INO_gst_SavManager.p_Dest, &pCryptedBuff, NULL, ps2INO_gst_SavManager.i_DestSize + ENCRYPT_ADD_SIZE, INO_gst_SavManager.ul_CRC);				
				BIG_ComputeCRC(&INO_gst_SavManager.ul_CRC[6], ps2INO_gst_SavManager.p_Dest, ps2INO_gst_SavManager.i_DestSize);
				
				/* CRC Check */
				if(INO_gst_SavManager.ul_CRC[0] != INO_gst_SavManager.ul_CRC[6])
				{
					ps2INO_gst_SavManager.b_EncryptionError = TRUE;
					
					/*
					if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)					
					{						
						M_FreeEncryptionBuffer();					
						M_ErrorWhileSaving();				
					}
					else
					{	
						M_FreeEncryptionBuffer();										
						M_ErrorWhileLoading();					
					}
					return;					
					*/
				}
			}
			else
			{					
				L_memcpy(ps2INO_gst_SavManager.p_Dest, pCryptedBuff, ps2INO_gst_SavManager.i_DestSize);
			}
			
			M_FreeEncryptionBuffer();
		}			

		if (ps2INO_gst_SavManager.pi_EffectiveDestSize)
			*ps2INO_gst_SavManager.pi_EffectiveDestSize = ps2INO_gst_SavManager.i_sceMcSync_Result;

		/* close main file */
		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcClose(ps2INO_gst_SavManager.i_FileHandler);
		M_dbg_printf(("sceMcClose\n"));	
		ps2INO_gst_SavManager.en_State = SavState_e_ClosingMainFile;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_ClosingMainFile:
		ps2INO_gst_SavManager.i_FileHandler = -1;
		
		// echec en cas de foirage cryptage
		/*
		if ( ps2INO_gst_SavManager.b_EncryptionError )
		{
			ps2INO_gst_SavManager.b_EncryptionError = FALSE;
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)					
			{						
				M_ErrorWhileSaving();				
			}
			else
			{	
				M_ErrorWhileLoading();					
			}
			return;					
		}
		*/
		
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result || ps2INO_gst_SavManager.b_EncryptionError)
		{
			ps2INO_gst_SavManager.b_EncryptionError = FALSE;
			if(ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
			{
				if (!ps2INO_gst_SavManager.i_Writing_ReadIsDone )
				{
					ps2INO_gst_SavManager.i_sceMcxxxx_Result = 0;
					ps2INO_gst_SavManager.i_sceMcSync_Result = 0;
					ps2INO_gst_SavManager.i_Writing_ReadIsDone = 1;
					ps2INO_gst_SavManager.i_Writing_NoFile = 0;
					ps2INO_gst_SavManager.en_State = SavState_e_OpenMainFile;
				}
				else
				{
					M_FreeEncryptionBuffer();				
					M_ErrorWhileSaving();
				}
			}
			else
			{
				M_FreeEncryptionBuffer();			
				M_ErrorWhileLoading();
			}
		}
		else
		{
			/* close successfully */
			if ( (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write) && ps2INO_gst_SavManager.i_Writing_ReadIsDone )
			{
				/* 10068 comment line */
				//M_GotoLowPrio( SavState_e_WaitingUserResponseForSavingSuccess );
				/* 10068 added lines */
				ps2INO_gst_SavManager.b_CreateIconSysAfterSave = TRUE;
				ps2INO_gst_SavManager.en_State = SavState_e_GoingToHome2;
				break;
				/* 10068 */
			}
			else
			{
				if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_ReadAllHeaders) 
				{
					for(i = 0; i < INO_Cte_SavSlotNbMax; i++)
					{
						L_memcpy
						(
							&INO_gst_SavManager.ast_SlotDesc[i],
							INO_gst_SavManager.p_Temp + i * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
							INO_Cte_SavHeaderSize
						);
					}
				}
				else if (ps2INO_gst_SavManager.en_UserCommand == SavCmd_e_Write)
				{
					/* reading before writing, have to copy only not writed slot */
					for(i = 0; i < INO_Cte_SavSlotNbMax; i++)
					{
						if ( i == INO_gst_SavManager.i_CurrentSlot )
							continue;
						L_memcpy
						(
							&INO_gst_SavManager.ast_SlotDesc[i],
							INO_gst_SavManager.p_Temp + i * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
							INO_Cte_SavHeaderSize
						);
					}
					
					ps2INO_gst_SavManager.i_sceMcxxxx_Result = 0;
					ps2INO_gst_SavManager.i_sceMcSync_Result = 0;
					ps2INO_gst_SavManager.i_Writing_ReadIsDone = 1;
					ps2INO_gst_SavManager.i_Writing_NoFile = 0;
					ps2INO_gst_SavManager.en_State = SavState_e_OpenMainFile;
					break;
				}
				else
				{
					L_memcpy
					(
						&INO_gst_SavManager.ast_SlotDesc[INO_gst_SavManager.i_CurrentSlot],
						INO_gst_SavManager.p_Temp + INO_gst_SavManager.i_CurrentSlot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
						INO_Cte_SavHeaderSize
					);

					L_memcpy
					(
						INO_gst_SavManager.ac_CurrSlotBuffer,
						INO_gst_SavManager.p_Temp + INO_gst_SavManager.i_CurrentSlot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize) + INO_Cte_SavHeaderSize,
						INO_Cte_SavOneSlotMaxSize
					);
				}
				M_GotoLowPrio( SavState_e_WaitingUserResponseForLoadingSuccess );
				//M_SuccessWhileLoading();
			}
		}
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_MakeTree:
		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcChdir
		(
			ps2INO_gst_SavManager.i_CurrentCardId,
			Cte_DefaultSlotNb,
			"/",
			0
		);
		M_dbg_printf(("sceMcChDir\n"));	
		ps2INO_gst_SavManager.en_State = SavState_e_MakeTree2;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_MakeTree2:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcMkdir
			(
				ps2INO_gst_SavManager.i_CurrentCardId,
				Cte_DefaultSlotNb,
				ps2INO_gsz_GameSaveName
			);
		M_dbg_printf(("sceMcMkDir\n"));		
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		ps2INO_gst_SavManager.i_DirExist = 0;
		ps2INO_gst_SavManager.en_State = SavState_e_MakingDir;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_MakingDir:
		switch(ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
		case sceMcResSucceed:
			break;

		case sceMcResNoFormat:
			M_FreeEncryptionBuffer();		
			M_ErrorUnformattedMc();
			return;

		default:

		/* avant modif pour bug 10337
		case sceMcResDeniedPermit:
		case sceMcResNoEntry:
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
			*/
		
		/* modif bug 10337 */	
		case sceMcResDeniedPermit:
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;	
			
		case sceMcResNoEntry:
			ps2INO_gst_SavManager.i_DirExist = 1;
			break;
		/* fin modif bug 10337 */

		case sceMcResFullDevice:
			M_FreeEncryptionBuffer();		
			M_ErrorNoMoreSpaceOnMc();
			return;
		}

		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcChdir
			(
				ps2INO_gst_SavManager.i_CurrentCardId,
				Cte_DefaultSlotNb,
				ps2INO_gsz_GameSaveName,
				0
			);
		M_dbg_printf(("sceMcChDir\n"));		
		ps2INO_gst_SavManager.en_State = SavState_e_GoingToHome2;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_GoingToHome2:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
			/* modif bug 10337 */
			if (ps2INO_gst_SavManager.i_DirExist == 1)
			{
				ps2INO_gst_SavManager.i_DirExist = 2;
				ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcDelete
				(
					ps2INO_gst_SavManager.i_CurrentCardId,
					Cte_DefaultSlotNb,
					ps2INO_gsz_GameSaveName
				);
				ps2INO_gst_SavManager.en_State = SavState_e_DeleteDir;
				return;
			}
			/* fin modif bug 10337 */
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcOpen
			(
				ps2INO_gst_SavManager.i_CurrentCardId,
				Cte_DefaultSlotNb,
				Csz_IconSysFileName,
				SCE_RDWR | SCE_CREAT
			);
		M_dbg_printf(("sceMcOpen\n"));		
		ps2INO_gst_SavManager.en_State = SavState_e_CreatingIconSys;
		break;
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* modif bug 10337 */
	case SavState_e_DeleteDir:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}
		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcMkdir
			(
				ps2INO_gst_SavManager.i_CurrentCardId,
				Cte_DefaultSlotNb,
				ps2INO_gsz_GameSaveName
			);
		M_dbg_printf(("sceMcMkDir\n"));		
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		ps2INO_gst_SavManager.en_State = SavState_e_MakingDir;
		break;
	/* fin modif bug 10337 */
	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* 10068 */
	case SavState_e_DeletingIconSys:
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = 0;
		ps2INO_gst_SavManager.i_sceMcSync_Result = 0;
		ps2INO_gst_SavManager.en_State = SavState_e_OpenMainFile;
		break;
	/* fin 10068 */

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_CreatingIconSys:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || (ps2INO_gst_SavManager.i_sceMcSync_Result < 0))
		{
			ps2INO_gst_SavManager.b_CreateIconSysAfterSave = FALSE;
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		ps2INO_gst_SavManager.i_FileHandler = ps2INO_gst_SavManager.i_sceMcSync_Result;

		/* titre de la sauvegarde King Kong */
		ps2INO_sst_DefaultIconSys.TitleName[0] = 0x82;	/* K */
		ps2INO_sst_DefaultIconSys.TitleName[1] = 0x6A;
		ps2INO_sst_DefaultIconSys.TitleName[2] = 0x82;	/* i */
		ps2INO_sst_DefaultIconSys.TitleName[3] = 0x89;
		ps2INO_sst_DefaultIconSys.TitleName[4] = 0x82;	/* n */
		ps2INO_sst_DefaultIconSys.TitleName[5] = 0x8e;
		ps2INO_sst_DefaultIconSys.TitleName[6] = 0x82;	/* g */
		ps2INO_sst_DefaultIconSys.TitleName[7] = 0x87;
		ps2INO_sst_DefaultIconSys.TitleName[8] = 0x81;	/*   */
		ps2INO_sst_DefaultIconSys.TitleName[9] = 0x40;
		ps2INO_sst_DefaultIconSys.TitleName[10] = 0x82; /* K */
		ps2INO_sst_DefaultIconSys.TitleName[11] = 0x6A;
		ps2INO_sst_DefaultIconSys.TitleName[12] = 0x82;	/* o */
		ps2INO_sst_DefaultIconSys.TitleName[13] = 0x8f;
		ps2INO_sst_DefaultIconSys.TitleName[14] = 0x82; /* n */
		ps2INO_sst_DefaultIconSys.TitleName[15] = 0x8e;
		ps2INO_sst_DefaultIconSys.TitleName[16] = 0x82; /* g */
		ps2INO_sst_DefaultIconSys.TitleName[17] = 0x87;
		
		L_strcpy(ps2INO_sst_DefaultIconSys.FnameView, Csz_IconViewFileName);
		L_strcpy(ps2INO_sst_DefaultIconSys.FnameCopy, Csz_IconViewFileName);
		L_strcpy(ps2INO_sst_DefaultIconSys.FnameDel, Csz_IconViewFileName);

		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcWrite
			(
				ps2INO_gst_SavManager.i_FileHandler,
				(char *) &ps2INO_sst_DefaultIconSys,
				sizeof(sceMcIconSys)
			);
		M_dbg_printf(("sceMcWrite\n"));		
		ps2INO_gst_SavManager.en_State = SavState_e_WritingIconSys;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WritingIconSys:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || (ps2INO_gst_SavManager.i_sceMcSync_Result < 0))
		{
			ps2INO_gst_SavManager.b_CreateIconSysAfterSave = FALSE;
			M_FreeEncryptionBuffer();
			M_ErrorWhileSaving();
			return;
		}

		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcClose(ps2INO_gst_SavManager.i_FileHandler);
		M_dbg_printf(("sceMcClose\n"));	
		ps2INO_gst_SavManager.en_State = SavState_e_ClosingIconSys;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_ClosingIconSys:
		ps2INO_gst_SavManager.i_FileHandler = -1;
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
			M_FreeEncryptionBuffer();
			M_ErrorWhileSaving();
			return;
		}
		
		/* 10068 */
		if (ps2INO_gst_SavManager.b_CreateIconSysAfterSave)
		{
			ps2INO_gst_SavManager.b_CreateIconSysAfterSave = FALSE;
			M_GotoLowPrio( SavState_e_WaitingUserResponseForSavingSuccess );
			break;
		}
		/* 10068 */

		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcOpen
			(
				ps2INO_gst_SavManager.i_CurrentCardId,
				Cte_DefaultSlotNb,
				Csz_IconViewFileName,
				SCE_RDWR | SCE_CREAT
			);
		M_dbg_printf(("sceMcOpen\n"));		
		ps2INO_gst_SavManager.en_State = SavState_e_CreatingIcon;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_CreatingIcon:
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || (ps2INO_gst_SavManager.i_sceMcSync_Result < 0))
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		ps2INO_gst_SavManager.i_FileHandler = ps2INO_gst_SavManager.i_sceMcSync_Result;

#if !defined(PSX2_USE_iopCDV)
		fd2 = sceOpen(Csz_IconModelFileName, SCE_RDONLY);
		if(fd2 < 0)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		size = sceLseek(fd2, 0, SCE_SEEK_END);
		sceLseek(fd2, 0, SCE_SEEK_SET);
		p_tmp = (char *) MEM_p_Alloc(size);
		if(p_tmp == NULL)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		sceRead(fd2, p_tmp, size);
		sceClose(fd2);
#else
		fd2 = eeCDV_i_OpenFile(Csz_IconModelFileName);
		if(fd2 < 0)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		size = eeCDV_i_GetFileSize(fd2);
		p_tmp = (char *) MEM_p_Alloc(size);
		if(p_tmp == NULL)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		eeCDV_i_ReadFile(fd2, p_tmp, size);
		eeCDV_i_CloseFile(fd2);
#endif
		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcWrite(ps2INO_gst_SavManager.i_FileHandler, p_tmp, size);
		M_dbg_printf(("sceMcWrite\n"));	
		ps2INO_gst_SavManager.en_State = SavState_e_WritingIcon;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_WritingIcon:
		if(p_tmp) MEM_Free(p_tmp);
		p_tmp = NULL;

		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || (ps2INO_gst_SavManager.i_sceMcSync_Result < 0))
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		ps2INO_gst_SavManager.i_TimeOut = 0;
		ps2INO_gst_SavManager.i_sceMcxxxx_Result = sceMcClose(ps2INO_gst_SavManager.i_FileHandler);
		M_dbg_printf(("sceMcClose\n"));	
		ps2INO_gst_SavManager.en_State = SavState_e_ClosingIcon;
		break;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case SavState_e_ClosingIcon:
		ps2INO_gst_SavManager.i_FileHandler = -1;
		if(ps2INO_gst_SavManager.i_sceMcxxxx_Result || ps2INO_gst_SavManager.i_sceMcSync_Result)
		{
			M_FreeEncryptionBuffer();		
			M_ErrorWhileSaving();
			return;
		}

		ps2INO_gst_SavManager.en_State = SavState_e_OpenMainFile;
		break;

	default:
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2INO_i_SavSeti(int _i_RequestId, int _i_Value)
{
	int i;
	
	switch(_i_RequestId)
	{
	case INO_e_SavRq_CardId:
		if(ps2INO_gst_SavManager.b_IsWorking) return -1;

		if((_i_Value == 0) || (_i_Value == 1))
		{
			ps2INO_gst_SavManager.i_CurrentCardId = _i_Value;
			return 0;
		}
		else
		{
			ps2INO_gst_SavManager.i_CurrentCardId = -1;
			return -1;
		}

	case INO_e_SavRq_Space:
		return ps2INO_gst_SavManager.i_FreeSpace;

	case INO_e_SavRq_Status:
		return (ps2INO_gst_SavManager.b_IsWorking || ps2INO_gst_SavManager.b_ChangingThreadPrio) ? 1 : 0;

	case INO_e_SavRq_UserMessageId:
		return ps2INO_gst_SavManager.en_UserMessage;
		
	case INO_e_SavRq_UserAction:
		i = ps2INO_gst_SavManager.i_UserAction;
		ps2INO_gst_SavManager.i_UserAction = -1;
		return i;

	case INO_e_SavRq_ClearMessageId:
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_None;
		ps2INO_gst_SavManager.i_UserAction = -1;
		break;

	case INO_e_SavRq_ReadAllHeaders:
		ps2INO_gst_SavManager.b_IconSysErased = FALSE;
		ps2INO_gst_SavManager.b_CreateIconSysAfterSave = FALSE;
		ps2INO_gst_SavManager.i_ForceLowPriority = FALSE;
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
		ps2INO_gst_SavManager.en_State = SavState_e_TestConnected;
		ps2INO_gst_SavManager.en_UserCommand = SavCmd_e_ReadAllHeaders;
		ps2INO_gst_SavManager.i_ConfirmOnSuccess = FALSE;
		ps2INO_gst_SavManager.f_TimeTest = Cte_TimeTest;
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;
		ps2INO_gst_SavManager.i_UserAction = -1;
		ps2INO_gst_SavManager.b_IsWorking = TRUE;
		ps2INO_gst_SavManager.b_EncryptionError = FALSE;
		break;

	case INO_e_SavRq_ReadOneProfile:
		ps2INO_gst_SavManager.b_IconSysErased = FALSE;
		ps2INO_gst_SavManager.b_CreateIconSysAfterSave = FALSE;
		ps2INO_gst_SavManager.i_ForceLowPriority = FALSE;
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
		ps2INO_gst_SavManager.en_State = SavState_e_TestConnected;
		ps2INO_gst_SavManager.en_UserCommand = SavCmd_e_Read;
		ps2INO_gst_SavManager.i_ConfirmOnSuccess = TRUE;
		ps2INO_gst_SavManager.f_TimeTest = Cte_TimeTest;
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;
		ps2INO_gst_SavManager.i_UserAction = -1;
		ps2INO_gst_SavManager.b_IsWorking = TRUE;
		ps2INO_gst_SavManager.b_EncryptionError = FALSE;
		break;

	case INO_e_SavRq_WriteOneProfile:
		ps2INO_gst_SavManager.b_IconSysErased = FALSE;
		ps2INO_gst_SavManager.b_CreateIconSysAfterSave = FALSE;
		ps2INO_gst_SavManager.i_ForceLowPriority = FALSE;
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
		ps2INO_gst_SavManager.en_State = SavState_e_TestConnected;
		ps2INO_gst_SavManager.en_UserCommand = SavCmd_e_Write;
		ps2INO_gst_SavManager.i_ConfirmOnSuccess = TRUE;
		ps2INO_gst_SavManager.b_IsWorking = TRUE;
		ps2INO_gst_SavManager.f_TimeTest = Cte_TimeTest;
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;
		ps2INO_gst_SavManager.i_UserAction = -1;
		ps2INO_gst_SavManager.i_Writing_ReadIsDone = FALSE;
		ps2INO_gst_SavManager.i_Writing_OverwriteOK = 0;
		ps2INO_gst_SavManager.i_Writing_NoFile = 0;
		ps2INO_gst_SavManager.b_EncryptionError = FALSE;
		break;
		
	case INO_e_SavRq_FreeRessource:
		for (i = 0; i < 4; i++)
		{
			ps2INO_gst_SavManager.i_ButtonPressed[ 1 ] = 0;
			ps2INO_gst_SavManager.i_ButtonJustPressed[ 1 ] = 0;
		}
		ps2INO_gst_SavManager.b_IconSysErased = FALSE;
		ps2INO_gst_SavManager.b_CreateIconSysAfterSave = FALSE;
		ps2INO_gst_SavManager.i_ForceLowPriority = FALSE;
		ps2INO_gst_SavManager.i_UserAction = -1;
		ps2INO_gst_SavManager.i_Writing_OverwriteOK = 0;
		ps2INO_gst_SavManager.b_Retry = FALSE;
		ps2INO_gst_SavManager.b_Delete = FALSE;
		ps2INO_gst_SavManager.b_ConfirmSave = FALSE;
		ps2INO_gst_SavManager.b_ConfirmSaveBeforeMakeTree = FALSE;
		ps2INO_gst_SavManager.b_EncryptionError = FALSE;
		break;
		
	case INO_e_SavRq_BootupTest:
		ps2INO_gst_SavManager.b_IconSysErased = FALSE;
		ps2INO_gst_SavManager.b_CreateIconSysAfterSave = FALSE;
		ps2INO_gst_SavManager.i_ForceLowPriority = FALSE;
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
		ps2INO_gst_SavManager.en_State = SavState_e_TestConnected;
		ps2INO_gst_SavManager.en_UserCommand = SavCmd_e_BootupTest;
		ps2INO_gst_SavManager.f_TimeTest = Cte_TimeTest;
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = Cte_ShortTimeTest;
		ps2INO_gst_SavManager.i_ConfirmOnSuccess = FALSE;
		ps2INO_gst_SavManager.b_IsWorking = TRUE;
		ps2INO_gst_SavManager.i_UserAction = -1;
		break;
		
	case INO_e_SavRq_TestMemCard:
		ps2INO_gst_SavManager.b_IconSysErased = FALSE;
		ps2INO_gst_SavManager.b_CreateIconSysAfterSave = FALSE;
		ps2INO_gst_SavManager.i_ForceLowPriority = FALSE;
		ps2INO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
		ps2INO_gst_SavManager.en_State = SavState_e_TestConnected;
		ps2INO_gst_SavManager.en_UserCommand = SavCmd_e_TestMemCard;
		ps2INO_gst_SavManager.i_ConfirmOnSuccess = FALSE;
		ps2INO_gst_SavManager.f_TimeTest = 0.0f;
		ps2INO_gst_SavManager.f_TimeBetween2MCTest = 0.0f;
		ps2INO_gst_SavManager.b_IsWorking = TRUE;
		break;
		
	case  INO_e_SavRq_SetRetryFlag:
		ps2INO_gst_SavManager.b_Retry = _i_Value;
		break;
		
	case INO_e_SavRq_SetDeleteFlag:
		ps2INO_gst_SavManager.b_Delete = _i_Value;
		break;
		 

	default:
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int ps2INO_i_SavGetMinSpace(void)
{
	/*~~~~~~~~*/
	int minsize;
	int icosize;
	/*~~~~~~~~*/

	minsize = 0;

	/*$2- default.ico file size --------------------------------------------------------------------------------------*/

	icosize = 29 * 1024;	/* ico file is about 80Ko */
	icosize = (icosize + Cte_McClusterSize - 1) / Cte_McClusterSize;
	minsize += icosize;

	/*$2- ico.sys file size ------------------------------------------------------------------------------------------*/

	icosize = (sizeof(sceMcIconSys) + Cte_McClusterSize - 1) / Cte_McClusterSize;
	minsize += icosize;

	/*$2- main file size (oct) ---------------------------------------------------------------------------------------*/

	icosize = ((INO_Cte_SavOneSlotMaxSize * INO_Cte_SavSlotNbMax) + Cte_McClusterSize - 1) / Cte_McClusterSize;
	minsize += icosize;

	/*$2- file entry : 3 files ---------------------------------------------------------------------------------------*/

	icosize = (3 + 1) / 2;
	minsize += icosize;

	/*$2- folder entry : 1 main folder -------------------------------------------------------------------------------*/

	icosize = 1 * 2;
	minsize += icosize;
	
	/*$2- don't know why but QA tools indicate a save has an extra 1ko -----------------------------------------------*/
	minsize += 1;
	
	/*$2- total ------------------------------------------------------------------------------------------------------*/

	minsize *= Cte_McClusterSize;

	return minsize;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if 0

/*$2-error -----------------------------------------------------------------------------------------------------------*/

#define M_MsgAndReturn_LoadFailed() \
	do \
	{ \
		ps2INO_gi_Retrying = 1; \
		ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_LoadFailed; \
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_Retry); \
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_Cancel, Sav_e_CancelAll); \
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None); \
		INO_ge_SavSetNextState = INO_ge_SavCommand; \
		INO_ge_SavState = Sav_e_WaitingForUser; \
		return Sav_e_WaitingForUser; \
	} while(0);

#define M_MsgAndReturn_SaveFailed() \
	do \
	{ \
		ps2INO_gi_Retrying = 1; \
		ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_SaveFailed; \
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_Retry); \
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_Cancel, Sav_e_CancelAll); \
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None); \
		INO_ge_SavSetNextState = INO_ge_SavCommand; \
		INO_ge_SavState = Sav_e_WaitingForUser; \
		return Sav_e_WaitingForUser; \
	} while(0);

#define M_MsgAndReturn_FormatSucceed() \
	do \
	{ \
		ps2INO_gi_Retrying = 1; \
		ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_FormatSuccessFull; \
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Continue, Sav_e_TestPresence); \
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None); \
		INO_ge_SavState = Sav_e_WaitingForUser; \
		return Sav_e_WaitingForUser; \
	} while(0);

#define M_MsgAndReturn_FormatFailed() \
	do \
	{ \
		ps2INO_gi_Retrying = 1; \
		ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_FormatFailed; \
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_Retry); \
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_Cancel, Sav_e_CancelAll); \
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None); \
		INO_ge_SavState = Sav_e_WaitingForUser; \
		return Sav_e_WaitingForUser; \
	} while(0);

#define M_MsgAndReturn_CheckFailed() \
	do \
	{ \
		ps2INO_gi_Retrying = 1; \
		ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_CheckFailed; \
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_Retry); \
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_Cancel, Sav_e_CancelAll); \
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None); \
		INO_ge_SavState = Sav_e_WaitingForUser; \
		return Sav_e_WaitingForUser; \
	} while(0);

#define M_MsgAndReturn_NoMemoryCard() \
	do \
	{ \
		ps2INO_gi_Retrying = 1; \
		ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_NoMemoryCard; \
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_Retry); \
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_Cancel, Sav_e_CancelAll); \
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None); \
		INO_ge_SavState = Sav_e_WaitingForUser; \
		return Sav_e_WaitingForUser; \
	} while(0);

#define M_MsgAndReturn_NoSufficientSpace() \
	do \
	{ \
		ps2INO_gi_Retrying = 1; \
		if(ps2INO_gi_CurrentDirOk) \
			ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_NoSufficientSpace2; \
		else \
			ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_NoSufficientSpace; \
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_Retry); \
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_Cancel, Sav_e_CancelAll); \
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None); \
		INO_ge_SavState = Sav_e_WaitingForUser; \
		return Sav_e_WaitingForUser; \
	} while(0);

#define M_MsgAndReturn_UnformattedCard() \
	do \
	{ \
		ps2INO_gi_Retrying = 1; \
		ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_UnformatedCard; \
		L_memset(SAV_gst_Data, 0, sizeof(SAV_gst_Data)); \
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_Retry, Sav_e_Retry); \
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_Cancel, Sav_e_CancelAll); \
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_Format, Sav_e_FormatCard); \
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None); \
		INO_ge_SavState = Sav_e_WaitingForUser; \
		return Sav_e_WaitingForUser; \
	} while(0);

#define M_MsgAndReturn_CardHasChanged() \
	do \
	{ \
		ps2INO_gi_Retrying = 1; \
		ps2INO_gi_CardHasChanged = 1; \
		ps2INO_gi_JustFormated = 0; \
		ps2INO_gi_CurrentDirOk = 0; \
		ps2INO_gi_LastError = Sav_e_None; \
		ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_CardHasChanged; \
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None); \
		INO_ge_SavState = Sav_e_WaitingForUser; \
		return Sav_e_WaitingForUser; \
	} while(0);

#define M_Msg_CardHasChanged() \
	do \
	{ \
		ps2INO_gi_Retrying = 1; \
		ps2INO_gi_CardHasChanged = 1; \
		ps2INO_gi_JustFormated = 0; \
		ps2INO_gi_CurrentDirOk = 0; \
		ps2INO_gi_LastError = Sav_e_None; \
		ps2INO_gi_LastError = INO_ge_SavErrorId = INO_SavErr_CardHasChanged; \
		INO_M_SavBtn_Assign(INO_Btn_Cross, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Triangle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Circle, INO_Id_None, Sav_e_None); \
		INO_M_SavBtn_Assign(INO_Btn_Square, INO_Id_None, Sav_e_None); \
		INO_ge_SavState = Sav_e_WaitingForUser; \
	} while(0);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2INO_i_FormatCard(int _i_Slot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static int	si_InternalState = 0;
	int			sync;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ps2INO_ge_LastState != Sav_e_FormatCard) si_InternalState = 0;

	ps2INO_ge_LastState = Sav_e_FormatCard;

	switch(si_InternalState)
	{
	case 0:
		ps2INO_gi_TimeOut = 0;
		sync = sceMcGetInfo(_i_Slot, 0, (int *) &ps2INO_gi_LastResult, NULL, &ps2INO_gi_CardIsFormated);
		if(sync == sceMcResSucceed) si_InternalState = 1;
		return Sav_e_FormatCard;

	case 1:
		if((ps2INO_gi_Async == sceMcResChangedCard) || (ps2INO_gi_LastResult != sceMcTypePS2))
		{
			if(INO_ge_SavSetNextState != -1)
			{
				M_MsgAndReturn_CardHasChanged();	/* INO_ge_SavState = Sav_e_WaitingForUser; INO_ge_SavSetNextState
													 * -1; INO_SavErr_CardHasChanged return INO_ge_SavState; */
			}

			M_MsgAndReturn_CardHasChanged();
		}
		else
		{
			si_InternalState = 2;
		}

	case 2:
		_Trace("format_card");
		si_InternalState = 3;
		ps2INO_gi_TimeOut = 0;
		ps2INO_ge_LastState = Sav_e_FormatCard;
		ps2INO_gi_LastResult = sceMcFormat(_i_Slot, 0);
		return Sav_e_FormatCard;

	case 3:
		si_InternalState = 0;
		if(ps2INO_gi_LastResult || ps2INO_gi_Async)
		{
			M_MsgAndReturn_FormatFailed();
		}

		ps2INO_gi_JustFormated = 1;
		M_MsgAndReturn_FormatSucceed();
	}

	si_InternalState = 0;
	return Sav_e_FormatCard;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2INO_i_TestCard(int _i_Slot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static int	si_InternalState = 0;
	int			sync;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ps2INO_ge_LastState != Sav_e_TestCard) si_InternalState = 0;
	ps2INO_ge_LastState = Sav_e_TestCard;

	switch(si_InternalState)
	{
	case 0:
		ps2INO_gi_TimeOut = 0;
		sync = sceMcGetInfo(_i_Slot, 0, (int *) &ps2INO_gi_LastResult, NULL, &ps2INO_gi_CardIsFormated);
		if(sync == sceMcResSucceed) si_InternalState = 1;
		break;

	case 1:
		if(ps2INO_gi_Async == sceMcResChangedCard)
		{
			M_MsgAndReturn_CardHasChanged();
			ps2INO_ge_LastState = Sav_e_WaitingForUser;
			return Sav_e_WaitingForUser;
		}
		else if(ps2INO_gi_LastResult != sceMcTypePS2)
		{
			M_MsgAndReturn_CardHasChanged();
			ps2INO_ge_LastState = Sav_e_WaitingForUser;
			return Sav_e_WaitingForUser;
		}
		else
		{
			INO_ge_SavErrorId = Sav_e_None;
			ps2INO_ge_LastState = Sav_e_None;
			return Sav_e_None;
		}
		break;
	}

	return Sav_e_TestCard;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ps2INO_i_SavDbgSeti(int _i_RequestId, int _i_Value)
{
	return 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
