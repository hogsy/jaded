/*$T INOjoystick.c GC 1.138 06/24/03 19:37:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef _XBOX
#include "xtl.h"
#include "GX8/Gx8init.h"//<<<<<<<<<<<<<<
#include "ENGine/Sources/DEModisk/DEModisk.h"
#include "GDInterface/GDInterface.h"//<<<<<<<<<<<<<<<<
extern ULONG TimeBeforeVideo;
//extern BOOL STOPVIDEO;
//#endif
#endif /* _XBOX */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "INOut/INOjoystick.h"
#include "INOut/INOkeyboard.h"
#include "LINks/LINKmsg.h"

/*$1-( PS2 )-------------------------------------------------------------*/
#if defined(PSX2_TARGET)
#define TRC_PS2

#include "INOut/INOstruct.h"
#include "INOut/INO.h"
#include "MainPsx2/Sources/libscedemo.h"
/*$1-( PC : mouse & keyboard )-------------------------------------------*/
#elif defined(INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD)

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <stdarg.h>
#include "TIMer/TIMdefs.h"
#define INO_JOYSTICK_USE_CHEATS
#include "Dx9/Dx9Console.h"

/*$1-( EDITOR )----------------------------------------------------------*/
#elif defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL)

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

/*$1-( GAMECUBE )--------------------------------------------------------*/
#elif defined(_GAMECUBE)
#include <dolphin/pad.h>
#endif

/*$1-( REVOLUTION )--------------------------------------------------------*/
#if (defined _GC2REVO) || (defined _RVL)
#include <revolution/kpad.h>
#endif //_REVO


#ifdef _XBOX
extern int writeNOpad;
extern P_ResetTimer();
BOOL Bink_PlayVideo;
#endif
int ControlMode = 2;
//#define JAPANESE_VERSION
static BOOL IO_bJAPANESE_MENU = FALSE;
extern BOOL ENG_gb_InPause;
extern BOOL ENG_gb_ForcePauseEngine;
extern unsigned short		Demo_gus_language;

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	INO_tdst_JoySimulate_
{
	LONG	l_Mask;
	LONG	l_Buttons;
	float	f_Joy0[2];
	float	f_Joy1[2];
} INO_tdst_JoySimulate;

typedef enum	INO_tden_PortState_
{
	eState_Init,
	eState_InitDualShock,
	eState_EnterAnalogMode,
	eState_CheckAnalogMode,
	eState_InitDualShock2,
	eState_EnterPressMode,
	eState_CheckPressMode,
	eState_EnterVibrationMode,
	eState_CheckVibrationMode,
	eState_TreatData,
	eState_LastPortState
} INO_tden_PortState;


typedef struct	INO_tdst_GenericPad_
{
	unsigned char	ac_Button[eBtn_GenericButtonNb];
} INO_tdst_GenericPad;

typedef struct	INO_tdst_JoyInterface_
{
	void (*pfv_Joystick_Init) (HWND);
	void (*pfv_Joystick_Close) (void);
	void (*pfv_Joystick_Update) (INO_tdst_GenericPad *);
	void (*pfv_Joystick_LowVibSet) (BOOL _b_On);
	void (*pfv_Joystick_HightVibSet) (int _i_level);
	void (*pfv_Joystick_Move) (MATH_tdst_Vector * _pst_Move, int _i_Num);
	BOOL (*pfb_Joystick_IsValid) (void);
	int (*pfi_Joystick_SetCurrentPad) (int);
}
INO_tdst_JoyInterface;

// Joystick Recorder
ULONG   INO_n_NumRecord;
ULONG   INO_n_FrameCounter;
int		INO_b_RecordInput;
int		INO_b_PlayInput;
char    INO_dumpFileName[128];
L_FILE      INO_dumpFile;
extern ULONG BIG_ul_SearchKeyToFat (BIG_KEY);
extern UINT WOR_gul_WorldKey;
extern BOOL ENG_gb_NeedToReinit;
#include "ENGine/Sources/ENGvars.h"
#include "AIinterp/Sources/AIengine.h"
extern unsigned int	WOR_gul_WorldKey;
#include "BIGfiles\BIGdefs.h"
#include "BIGfiles\BIGfat.h"

#if defined(PSX2_TARGET)
#define INPUT_REC_FILE "host0:x:/INPUT_REC_%08X_PS2.ino"
#elif defined(_GAMECUBE)

#elif defined(_XBOX)

#elif defined(_XENON)
#define INPUT_REC_FILE "D:\\INPUT_REC_%08X_XE.ino"

#elif defined(INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD)

#else
JOYINFOEX	st_CurrentFrameJoy;
#define INPUT_REC_FILE "X:\\INPUT_REC_%08X.ino"
#endif


/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern float TIM_gf_dt;
extern float TIM_gf_MainClock;

/*$2- interface for multi target -------------------------------------------------------------------------------------*/

INO_tdst_JoyInterface	INO_gst_JoyInterface;

/*$2------------------------------------------------------------------------------------------------------------------*/

LONG	INO_l_JoystickMask;
LONG	INO_l_JoystickDir;
LONG	INO_l_Joystick_Enable;

/*$2- vibration ------------------------------------------------------------------------------------------------------*/


float INO_gf_PifDuration;
float INO_gf_PafDuration;

int INO_gi_PafIntensity;
int INO_gi_PifPafEnable;

/*$2- simulation -----------------------------------------------------------------------------------------------------*/

INO_tdst_JoySimulate	INO_gst_JoySimulateAsked;
INO_tdst_JoySimulate	INO_gst_JoySimulateCurrent;

/*$2------------------------------------------------------------------------------------------------------------------*/

#define INO_Cte_PabNumber	8

INO_tdst_GenericPad INO_gst_CurrPad[INO_Cte_PabNumber];
INO_tdst_GenericPad INO_gst_PrevPad[INO_Cte_PabNumber];
int					INO_gi_CurrentPadId;
int					INO_gi_CurrentFirstPadId=-1;
int					INO_gi_CurrentSecondPadId=-1;
int					INO_gi_LastPadId = -1;
int					INO_gi_PadConnectedNb=0;
int					INO_gai_AbsentPad[INO_Cte_PabNumber];

/*$2------------------------------------------------------------------------------------------------------------------*/

// Control modes
#define IO_MODE_JACK			0
#define IO_MODE_KONG			1
#define IO_MODE_MENU			2


/*
#define IO_SUBMODE_AIRCRAFT_LOOK	1
#define IO_SUBMODE_INVENTORY_MOUSE	1
#define IO_SUBMODE_INVENTORY_KEYS	2
*/

/*$2------------------------------------------------------------------------------------------------------------------*/

// Mvt for revolution pad
#define IO_MVT_None				0
#define IO_MVT_LeftToRight		1
#define IO_MVT_RightToLeft		2
#define IO_MVT_UpToDown			3
#define IO_MVT_DownToUp			4

/*$2------------------------------------------------------------------------------------------------------------------*/


BOOL	Demo_gb_Exit = FALSE;
BOOL	Demo_gb_IsActive = FALSE;

BOOL	TwoPadGame = FALSE;

/*$4
 ***********************************************************************************************************************
    prototypes for interface
 ***********************************************************************************************************************
 */

#if defined(_XENON)
BOOL					xeINO_b_Joystick_IsValid(void);
static void				xeINO_Joystick_Close(void);
static void				xeINO_Joystick_Init(HWND none);
static void				xeINO_Joystick_Move(MATH_tdst_Vector *, int _i_Num);
static void				xeINO_Joystick_Update(INO_tdst_GenericPad *);
static void				xeINO_PadHighVibSet(int _i_level);
static void				xeINO_PadLowVibSet(BOOL _b_On);
static int				xeINO_i_SetCurrentPad(int);
#elif defined(PSX2_TARGET)
static unsigned char	ps2INO_uc_GetButton(int _port, int _i_id);
BOOL					ps2INO_b_Joystick_IsValid(void);
static void				ps2INO_Joystick_Close(void);
static void				ps2INO_Joystick_Init(HWND none);
static void				ps2INO_Joystick_Move(MATH_tdst_Vector *, int _i_Num);
static void				ps2INO_Joystick_Update(INO_tdst_GenericPad *);
static void				ps2INO_PadHightVibSet(int _i_level);
static void				ps2INO_PadLowVibSet(BOOL _b_On);
static int				ps2INO_i_SetCurrentPad(int);
void                    ps2INO_CheckPort0(void);
BOOL                    ps2INO_b_Port0IsKo(void);
static void				ps2INO_SetControlMode(int);
#elif defined(_GAMECUBE) 
#if (defined _GC2REVO) || (defined _RVL)
static unsigned char	revoINO_uc_GetButton(int pad, int button);
static void				revoINO_Joystick_Init(HWND none);
static void				revoINO_Joystick_Move(MATH_tdst_Vector *, int _i_Num);
static void				revoINO_Joystick_Update(INO_tdst_GenericPad *);
static void				revoINO_SetControlMode(int);
static void 			revoINO_Joystick_GetPointer(MATH_tdst_Vector *);
static void 			revoINO_Joystick_GetAcc(MATH_tdst_Vector *,int);
static int	 			revoINO_Joystick_GetLastMove(int);
static float 			revoINO_Joystick_GetDistance();
static void 			revoINO_Joystick_SetVibration(int i_Num,float f_Delay);
static void 			revo_CheckMvtRecognition(); 
#endif
static unsigned char	gcINO_uc_GetButton(int pad, int button);
BOOL					gcINO_b_Joystick_IsValid(void);
static void				gcINO_Joystick_Close(void);
static void				gcINO_Joystick_Init(HWND none);
static void				gcINO_Joystick_Move(MATH_tdst_Vector *, int _i_Num);
static void				gcINO_Joystick_Update(INO_tdst_GenericPad *);
static void				gcINO_PadHightVibSet(int _i_level);
static void				gcINO_PadLowVibSet(BOOL _b_On);
static int				gcINO_i_SetCurrentPad(int);
static void				gcINO_SetControlMode(int);
#elif defined(_XBOX)
static unsigned char	xbINO_uc_GetButton(int port, int button);
static BOOL				xbINO_b_Joystick_IsValid(void);
static void				xbINO_Joystick_Close(void);
static void				xbINO_Joystick_Init(HWND none);
static void				xbINO_Joystick_Move(MATH_tdst_Vector *, int _i_Num);
static void				xbINO_Joystick_Update(INO_tdst_GenericPad *);
static void				xbINO_PadHightVibSet(int _i_level);
static void				xbINO_PadLowVibSet(BOOL _b_On);
static int				xbINO_i_SetCurrentPad(int);
static void				xbINO_SetControlMode(int);
#elif defined(INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD)
static BOOL				pcINO_b_Joystick_IsValid(void);
static void				pcINO_Joystick_Close(void);
static void				pcINO_Joystick_Init(HWND none);
static void				pcINO_Joystick_Move(MATH_tdst_Vector *, int _i_Num);
static void				pcINO_Joystick_Update(INO_tdst_GenericPad *);
static void				pcINO_PadHightVibSet(int _i_level);
static void				pcINO_PadLowVibSet(BOOL _b_On);
static int				pcINO_i_SetCurrentPad(int);
/*static*/ void			pcINO_SetControlMode(int);		// referenced from STRparse_pc.c
/*static*/ void			pcINO_SetControlSubmode(int);	// referenced from STRparse_pc.c
/*static*/ void			pcINO_ResetKeyboardFsm(int);	// referenced from WORuniverse.c
#else
static BOOL				win32INO_b_Joystick_IsValid(void);
static void				win32INO_Joystick_Close(void);
static void				win32INO_Joystick_Init(HWND none);
static void				win32INO_Joystick_Move(MATH_tdst_Vector *, int _i_Num);
static void				win32INO_Joystick_Update(INO_tdst_GenericPad *);
static void				win32INO_PadHightVibSet(int _i_level);
static void				win32INO_PadLowVibSet(BOOL _b_On);
static int				win32INO_i_SetCurrentPad(int);
#endif

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_Joystick_Init(HWND _hWnd)
{
#if defined(_XENON)
	INO_gst_JoyInterface.pfv_Joystick_Init			= xeINO_Joystick_Init;
	INO_gst_JoyInterface.pfv_Joystick_Close			= xeINO_Joystick_Close;
	INO_gst_JoyInterface.pfv_Joystick_Update		= xeINO_Joystick_Update;
	INO_gst_JoyInterface.pfv_Joystick_LowVibSet		= xeINO_PadLowVibSet;
	INO_gst_JoyInterface.pfv_Joystick_HightVibSet	= xeINO_PadHighVibSet;
	INO_gst_JoyInterface.pfv_Joystick_Move			= xeINO_Joystick_Move;
	INO_gst_JoyInterface.pfb_Joystick_IsValid		= xeINO_b_Joystick_IsValid;
	INO_gst_JoyInterface.pfi_Joystick_SetCurrentPad	= xeINO_i_SetCurrentPad;
#elif defined(PSX2_TARGET)
	INO_gst_JoyInterface.pfv_Joystick_Init = ps2INO_Joystick_Init;
	INO_gst_JoyInterface.pfv_Joystick_Close = ps2INO_Joystick_Close;
	INO_gst_JoyInterface.pfv_Joystick_Update = ps2INO_Joystick_Update;
	INO_gst_JoyInterface.pfv_Joystick_LowVibSet = ps2INO_PadLowVibSet;
	INO_gst_JoyInterface.pfv_Joystick_HightVibSet = ps2INO_PadHightVibSet;
	INO_gst_JoyInterface.pfv_Joystick_Move = ps2INO_Joystick_Move;
	INO_gst_JoyInterface.pfb_Joystick_IsValid = ps2INO_b_Joystick_IsValid;
	INO_gst_JoyInterface.pfi_Joystick_SetCurrentPad = ps2INO_i_SetCurrentPad;

    INO_n_FrameCounter = 0;    
    INO_n_NumRecord = 0;

#elif defined(_GAMECUBE)
	#if (defined _GC2REVO) || (defined _RVL)
		INO_gst_JoyInterface.pfv_Joystick_Init = revoINO_Joystick_Init;
		INO_gst_JoyInterface.pfv_Joystick_Close = gcINO_Joystick_Close;
		INO_gst_JoyInterface.pfv_Joystick_Update = revoINO_Joystick_Update;
		INO_gst_JoyInterface.pfv_Joystick_LowVibSet = gcINO_PadLowVibSet;
		INO_gst_JoyInterface.pfv_Joystick_HightVibSet = gcINO_PadHightVibSet;
		INO_gst_JoyInterface.pfv_Joystick_Move = revoINO_Joystick_Move;
		INO_gst_JoyInterface.pfb_Joystick_IsValid = gcINO_b_Joystick_IsValid;
		INO_gst_JoyInterface.pfi_Joystick_SetCurrentPad = gcINO_i_SetCurrentPad;	
	#else
		INO_gst_JoyInterface.pfv_Joystick_Init = gcINO_Joystick_Init;
		INO_gst_JoyInterface.pfv_Joystick_Close = gcINO_Joystick_Close;
		INO_gst_JoyInterface.pfv_Joystick_Update = gcINO_Joystick_Update;
		INO_gst_JoyInterface.pfv_Joystick_LowVibSet = gcINO_PadLowVibSet;
		INO_gst_JoyInterface.pfv_Joystick_HightVibSet = gcINO_PadHightVibSet;
		INO_gst_JoyInterface.pfv_Joystick_Move = gcINO_Joystick_Move;
		INO_gst_JoyInterface.pfb_Joystick_IsValid = gcINO_b_Joystick_IsValid;
		INO_gst_JoyInterface.pfi_Joystick_SetCurrentPad = gcINO_i_SetCurrentPad;
	#endif
#elif defined(_XBOX)
	INO_gst_JoyInterface.pfv_Joystick_Init = xbINO_Joystick_Init;
	INO_gst_JoyInterface.pfv_Joystick_Close = xbINO_Joystick_Close;
	INO_gst_JoyInterface.pfv_Joystick_Update = xbINO_Joystick_Update;
	INO_gst_JoyInterface.pfv_Joystick_LowVibSet = xbINO_PadLowVibSet;
	INO_gst_JoyInterface.pfv_Joystick_HightVibSet = xbINO_PadHightVibSet;
	INO_gst_JoyInterface.pfv_Joystick_Move = xbINO_Joystick_Move;
	INO_gst_JoyInterface.pfb_Joystick_IsValid = xbINO_b_Joystick_IsValid;
	INO_gst_JoyInterface.pfi_Joystick_SetCurrentPad = xbINO_i_SetCurrentPad;
#elif defined(INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD)
	INO_gst_JoyInterface.pfv_Joystick_Init = pcINO_Joystick_Init;
	INO_gst_JoyInterface.pfv_Joystick_Close = pcINO_Joystick_Close;
	INO_gst_JoyInterface.pfv_Joystick_Update = pcINO_Joystick_Update;
	INO_gst_JoyInterface.pfv_Joystick_LowVibSet = pcINO_PadLowVibSet;
	INO_gst_JoyInterface.pfv_Joystick_HightVibSet = pcINO_PadHightVibSet;
	INO_gst_JoyInterface.pfv_Joystick_Move = pcINO_Joystick_Move;
	INO_gst_JoyInterface.pfb_Joystick_IsValid = pcINO_b_Joystick_IsValid;
	INO_gst_JoyInterface.pfi_Joystick_SetCurrentPad = pcINO_i_SetCurrentPad;
#else
	INO_gst_JoyInterface.pfv_Joystick_Init = win32INO_Joystick_Init;
	INO_gst_JoyInterface.pfv_Joystick_Close = win32INO_Joystick_Close;
	INO_gst_JoyInterface.pfv_Joystick_Update = win32INO_Joystick_Update;
	INO_gst_JoyInterface.pfv_Joystick_LowVibSet = win32INO_PadLowVibSet;
	INO_gst_JoyInterface.pfv_Joystick_HightVibSet = win32INO_PadHightVibSet;
	INO_gst_JoyInterface.pfv_Joystick_Move = win32INO_Joystick_Move;
	INO_gst_JoyInterface.pfb_Joystick_IsValid = win32INO_b_Joystick_IsValid;
	INO_gst_JoyInterface.pfi_Joystick_SetCurrentPad = win32INO_i_SetCurrentPad;

    INO_n_FrameCounter = 0;    
    INO_n_NumRecord = 0;    

#endif
	INO_l_JoystickMask = INO_JoyMask_All;
	INO_l_JoystickDir = 0;
	INO_l_Joystick_Enable = 0;

	INO_gf_PifDuration = 0.0f;
	INO_gf_PafDuration = 0.0f;
	INO_gi_PafIntensity = 0;
	INO_gi_PifPafEnable = 1;

	L_memset(&INO_gst_JoySimulateAsked, 0, sizeof(INO_tdst_JoySimulate));
	L_memset(&INO_gst_JoySimulateCurrent, 0, sizeof(INO_tdst_JoySimulate));

	INO_gi_CurrentPadId = -1;
	L_memset(INO_gst_CurrPad, 0, INO_Cte_PabNumber * sizeof(INO_tdst_GenericPad));
	L_memset(INO_gst_PrevPad, 0, INO_Cte_PabNumber * sizeof(INO_tdst_GenericPad));

	INO_gst_JoyInterface.pfv_Joystick_Init(_hWnd);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int INO_i_SetCurrentPad(int id)
{
	return INO_gst_JoyInterface.pfi_Joystick_SetCurrentPad(id);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int INO_i_GetCurrentPad(void)
{
#if defined( _XENON )
	if( xeINO_IsControllerLost() )
		return -1;
#endif
	return INO_gi_CurrentPadId;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_SimulateReset(void)
{
	L_memset(&INO_gst_JoySimulateAsked, 0, sizeof(INO_tdst_JoySimulate));
	L_memset(&INO_gst_JoySimulateCurrent, 0, sizeof(INO_tdst_JoySimulate));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_Joystick_Close(void)
{
	INO_gst_JoyInterface.pfv_Joystick_Close();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_Joystick_Update(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	l_Mask[eBtn_GenericButtonNb];
	unsigned char	l_Buttons[eBtn_GenericButtonNb];
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!INO_l_Joystick_Enable) return;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    update simulate
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	L_memcpy(&INO_gst_JoySimulateCurrent, &INO_gst_JoySimulateAsked, sizeof(INO_tdst_JoySimulate));
	L_memset(&INO_gst_JoySimulateAsked, 0, sizeof(INO_tdst_JoySimulate));

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    update current value
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	L_memcpy(INO_gst_PrevPad, INO_gst_CurrPad, INO_Cte_PabNumber * sizeof(INO_tdst_GenericPad));
	INO_gst_JoyInterface.pfv_Joystick_Update(INO_gst_CurrPad);
	
	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    save real result
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */
	 L_memcpy( INO_gst_CurrPad + 4, INO_gst_CurrPad, 4 * sizeof(INO_tdst_GenericPad) );

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    special case for PS2 demo
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(!Demo_gb_Exit)
	{
		for(i = 0; i < INO_Cte_PabNumber; i++)
		{
			if(INO_gst_CurrPad[i].ac_Button[eBtn_Select] && INO_gst_CurrPad[i].ac_Button[eBtn_Start])
			{
				Demo_gb_Exit = TRUE;
				break;
			}
		}
	}

	if(INO_gi_CurrentPadId != -1)
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    apply mask
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		for(i = 0; i < eBtn_GenericButtonNb; i++)
		{
			if(!(INO_l_JoystickMask & (1 << i))) INO_gst_CurrPad[INO_gi_CurrentPadId].ac_Button[i] = 0;
		}

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    apply simulate
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		if(INO_gst_JoySimulateCurrent.l_Mask)
		{
			L_memset(l_Buttons, 0, eBtn_GenericButtonNb * sizeof(char));
			L_memset(l_Mask, 0, eBtn_GenericButtonNb * sizeof(char));

			for(i = 0; i < eBtn_GenericButtonNb; i++)
			{
				if(INO_gst_JoySimulateCurrent.l_Mask & (1 << i)) l_Mask[i] = 0xFF;
				if(INO_gst_JoySimulateCurrent.l_Buttons & (1 << i)) l_Buttons[i] = 0xFF;

				INO_gst_CurrPad[INO_gi_CurrentPadId].ac_Button[i] &= ~l_Mask[i];
				INO_gst_CurrPad[INO_gi_CurrentPadId].ac_Button[i] |= (l_Buttons[i] & l_Mask[i]);
			}
		}
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    vibration
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(INO_gst_JoyInterface.pfb_Joystick_IsValid())
	{
		if(INO_gf_PifDuration)
		{
			INO_gf_PifDuration -= TIM_gf_dt;
			if(INO_gf_PifDuration <= 0.0f) 
			{
			    INO_gst_JoyInterface.pfv_Joystick_LowVibSet(0);
			    INO_gf_PifDuration = 0.0f;
			}
		}

		if(INO_gf_PafDuration)
		{
#ifndef _XENON
// no condition on Xenon since we stop vibrations on pause anyway (TCR requirement)
			if( !ENG_gb_InPause && !ENG_gb_ForcePauseEngine )
#endif
			INO_gf_PafDuration -= TIM_gf_dt;
			if(INO_gf_PafDuration <= 0.0f) 
			{
			    INO_gst_JoyInterface.pfv_Joystick_HightVibSet(0);
			    INO_gf_PafDuration = 0.0f;
			}
		}
	}

    if (INO_b_RecordInput || INO_b_PlayInput)
    {
        INO_n_FrameCounter++;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int INO_i_PifPafEnable(int _i_Enable)
{   
	if(_i_Enable != -1)
	{
		if(_i_Enable)
			INO_gi_PifPafEnable = 1;
		else
		{
			INO_gi_PifPafEnable = 0;
			INO_gf_PifDuration = 0.0f;
			INO_gf_PafDuration = 0.0f;
			if(INO_gst_JoyInterface.pfv_Joystick_LowVibSet) INO_gst_JoyInterface.pfv_Joystick_LowVibSet(0);
			if(INO_gst_JoyInterface.pfv_Joystick_HightVibSet) INO_gst_JoyInterface.pfv_Joystick_HightVibSet(0);
		}
	}

	return INO_gi_PifPafEnable;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_PifSet(int _i_FrameDuration)
{
	if(!INO_gi_PifPafEnable) return;

	INO_gf_PifDuration = fLongToFloat(lAbs(_i_FrameDuration)) * 0.032f;
	INO_gst_JoyInterface.pfv_Joystick_LowVibSet(_i_FrameDuration ? TRUE : FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_PafSet(int _i_Intensity, int _i_FrameDuration)
{
	if(!INO_gi_PifPafEnable) return;

	INO_gf_PafDuration = fLongToFloat(lAbs(_i_FrameDuration)) * 0.032f;
	INO_gi_PafIntensity = (_i_Intensity > 0xff) ? 0xFF : _i_Intensity;
	INO_gst_JoyInterface.pfv_Joystick_HightVibSet(INO_gi_PafIntensity);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INO_b_Joystick_IsButtonDown(LONG _l_ButtonIndex)
{
#if defined(_XENON)
	if (xeINO_bDesktop_InputLock) return FALSE;
#endif
	if(INO_gi_CurrentPadId == -1) return FALSE;
	return INO_gst_CurrPad[INO_gi_CurrentPadId].ac_Button[_l_ButtonIndex];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INO_b_Joystick_IsButtonJustDown(LONG _l_ButtonIndex)
{
#if defined(_XENON)
	if (xeINO_bDesktop_InputLock)
		return FALSE;
#endif
	if(INO_gi_CurrentPadId == -1) return FALSE;
	if
	(
		INO_gst_CurrPad[INO_gi_CurrentPadId].ac_Button[_l_ButtonIndex]
	&&	!INO_gst_PrevPad[INO_gi_CurrentPadId].ac_Button[_l_ButtonIndex]
	) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INO_b_Joystick_IsButtonJustUp(LONG _l_ButtonIndex)
{
#if defined(_XENON)
	if (xeINO_bDesktop_InputLock)
		return FALSE;
#endif
	if(INO_gi_CurrentPadId == -1) return FALSE;
	if
	(
		!INO_gst_CurrPad[INO_gi_CurrentPadId].ac_Button[_l_ButtonIndex]
	&&	INO_gst_PrevPad[INO_gi_CurrentPadId].ac_Button[_l_ButtonIndex]
	) return TRUE;
	return FALSE;
}
BOOL INO_b_MultiJoystick_IsButtonJustUp(LONG _l_ButtonIndex)
{
	int iii;
	static short PrevPad[4][10]={0};
	int saveC = INO_gi_CurrentPadId;
	for( iii = 0; iii < 4; iii++)
	{
		
		INO_gi_CurrentPadId=iii;
		//INO_gi_LastPadId=iii;
		INO_Joystick_Update();	
		
		if
		(
			!INO_gst_CurrPad[iii].ac_Button[_l_ButtonIndex]
		&&	PrevPad[iii][_l_ButtonIndex]
		) 
		{
			PrevPad[iii][_l_ButtonIndex]=0;
			INO_gi_CurrentPadId = saveC;return TRUE;
		}

		PrevPad[iii][_l_ButtonIndex]=INO_gst_CurrPad[iii].ac_Button[_l_ButtonIndex];

		//{INO_gi_CurrentPadId = saveC;INO_gi_LastPadId=saveL;return FALSE;}*/

		//if (INO_gst_CurrPad[iii].ac_Button[_l_ButtonIndex])
			//return TRUE;
			
	}
	INO_gi_CurrentPadId = saveC;
	return FALSE;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 JADE_HorizontalSymetry = 0;
void INO_Joystick_Move(MATH_tdst_Vector *_pst_Move, int _i_Num)
{
	BOOL b_InvertX;

	
#if defined(_XENON)
	if (xeINO_bDesktop_InputLock)
	{
		MATH_InitVectorToZero(_pst_Move);
		return;
	}
#endif
	if(!INO_l_Joystick_Enable||(INO_gi_CurrentPadId == -1))
	{
		MATH_InitVectorToZero(_pst_Move);
		return;
	}

	b_InvertX = INO_l_JoystickDir & INO_JoyDir_X0Invert;
	if (JADE_HorizontalSymetry && (ControlMode!=2) ) b_InvertX = !b_InvertX;
	
	//if (JADE_HorizontalSymetry && (ControlMode!=2) ) INO_l_JoystickDir ^= INO_JoyDir_X0Invert;
	if ( !(INO_gi_CurrentPadId & 4) )
	{
		if(INO_gst_JoySimulateCurrent.l_Mask & (1 << (INO_JoyMask_JoyShift + _i_Num)))
		{
			if(_i_Num)
			{
				MATH_InitVector(_pst_Move, INO_gst_JoySimulateCurrent.f_Joy1[0], INO_gst_JoySimulateCurrent.f_Joy1[1], 0);
			}
			else
			{
				MATH_InitVector(_pst_Move, INO_gst_JoySimulateCurrent.f_Joy0[0], INO_gst_JoySimulateCurrent.f_Joy0[1], 0);
			}

			if(b_InvertX) _pst_Move->x *= -1;
			if(INO_l_JoystickDir & INO_JoyDir_Y0Invert) _pst_Move->y *= -1;

			//if (JADE_HorizontalSymetry) INO_l_JoystickDir ^= INO_JoyDir_X0Invert;
			return;
		}

		if(!(INO_l_JoystickMask & (1 << (INO_JoyMask_JoyShift + _i_Num))))
		{
			MATH_InitVectorToZero(_pst_Move);
			//if (JADE_HorizontalSymetry) INO_l_JoystickDir ^= INO_JoyDir_X0Invert;
			return;
		}
	}

	INO_gst_JoyInterface.pfv_Joystick_Move(_pst_Move, _i_Num);
	if(b_InvertX) _pst_Move->x *= -1;
	if(INO_l_JoystickDir & INO_JoyDir_Y0Invert) _pst_Move->y *= -1; 
	//if (JADE_HorizontalSymetry) INO_l_JoystickDir ^= INO_JoyDir_X0Invert;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_Joystick_GetPointer(MATH_tdst_Vector *_pst_Move)
{
	if(!INO_l_Joystick_Enable||(INO_gi_CurrentPadId == -1))
	{
		MATH_InitVectorToZero(_pst_Move);
		return;
	}

#if (defined _GC2REVO) || (defined _RVL)
	revoINO_Joystick_GetPointer(_pst_Move);
#else
	MATH_InitVectorToZero(_pst_Move);	
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void INO_Joystick_GetAcc(MATH_tdst_Vector *_pst_Move,int _i_Num)
{
	if(!INO_l_Joystick_Enable||(INO_gi_CurrentPadId == -1))
	{
		MATH_InitVectorToZero(_pst_Move);
		return;
	}

#if (defined _GC2REVO) || (defined _RVL)
	revoINO_Joystick_GetAcc(_pst_Move,_i_Num);
#else
	MATH_InitVectorToZero(_pst_Move);	
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

int INO_Joystick_GetLastMove(int _i_Num)
{
	if(!INO_l_Joystick_Enable||(INO_gi_CurrentPadId == -1))
	{
		return IO_MVT_None;
	}

#if (defined _GC2REVO) || (defined _RVL)
	return revoINO_Joystick_GetLastMove(_i_Num);
#else
	return IO_MVT_None;	
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

float INO_Joystick_GetDistance()
{
	if(!INO_l_Joystick_Enable||(INO_gi_CurrentPadId == -1))
	{
		return 0.0f;
	}

#if (defined _GC2REVO) || (defined _RVL)
	return revoINO_Joystick_GetDistance();
#else
	return 0.0f;	
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void INO_Joystick_SetVibration(int i_Num,float f_Delay)
{
	if(!INO_l_Joystick_Enable||(INO_gi_CurrentPadId == -1))
	{
		return;
	}

#if (defined _GC2REVO) || (defined _RVL)
	revoINO_Joystick_SetVibration(i_Num,f_Delay);
#endif
}

/*
 =======================================================================================================================
    Simulate joystick
 =======================================================================================================================
 */
void INO_JoySimulate(int _i_Mask, int _i_Buttons, MATH_tdst_Vector *_pst_Move0, MATH_tdst_Vector *_pst_Move1)
{
	INO_gst_JoySimulateAsked.l_Mask = _i_Mask;
	INO_gst_JoySimulateAsked.l_Buttons = _i_Buttons;

	if(_pst_Move0)
	{
		INO_gst_JoySimulateAsked.f_Joy0[0] = _pst_Move0->x;
		INO_gst_JoySimulateAsked.f_Joy0[1] = _pst_Move0->y;
	}

	if(_pst_Move1)
	{
		INO_gst_JoySimulateAsked.f_Joy1[0] = _pst_Move1->x;
		INO_gst_JoySimulateAsked.f_Joy1[1] = _pst_Move1->y;
	}
}

/*
 =======================================================================================================================
    Set control context
 =======================================================================================================================
 */
static void Trace(const char* s, ...);

void INO_SetControlMode(int mode)
{
	ControlMode = mode;

#ifdef INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD
	pcINO_SetControlMode(mode);
#elif defined (_GAMECUBE)
	#if defined (_GC2REVO) || defined(_RVL)
		revoINO_SetControlMode(mode);
	#else
		gcINO_SetControlMode(mode);
	#endif
#elif defined(_XBOX)
	xbINO_SetControlMode(mode);
#elif defined(PSX2_TARGET)
	ps2INO_SetControlMode(mode);
#else 
	(void)mode;
#endif
}

void INO_SetControlSubmode(int submode)
{
#ifdef INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD
	pcINO_SetControlSubmode(submode);
#else
	(void)submode;
#endif
}

/*
 =======================================================================================================================
    Check if a mouse is being used for the control
 =======================================================================================================================
 */
int INO_i_IsMouseBeingUsed()
{
#ifdef INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD
	return 1;
#else
	return 0;
#endif
}

/*$4
 ***********************************************************************************************************************
    Xenon
 ***********************************************************************************************************************
 */

#if defined(_XENON)

#include "Xenon/XeINOjoystick.c"

/*$4
 ***********************************************************************************************************************
    PSX2
 ***********************************************************************************************************************
 */

#elif defined(PSX2_TARGET)

/*$2- macros ---------------------------------------------------------------------------------------------------------*/

#define Mi_PadData_cArraySize	32
#define Mi_PadData_lArraySize	4
#define Mi_scePadSuccessCode	1
#define Mi_MaxPortNumber		2
#define Mi_PadDefault_Slot		0
#define Mi_ErrTimeOutMax        60
#define Mi_PadErrorMax          30
#define Mi_PadStartTime         2
#define Mi_PadCheckTime         5

/*$2- types ----------------------------------------------------------------------------------------------------------*/

typedef enum	ps2INO_tden_PadId_
{
	e_Nigicom					= 2,
	e_Guncon_Konami				= 3,
	e_DualShock					= 4,
	e_AnalogJoystick			= 5,
	e_Guncon_Namco				= 6,
	e_DualShock2				= 7,
	e_Tsuri_con					= 0x100,
	e_Jog_con					= 0x300,
	e_LastPadId
} ps2INO_tden_PadId;

typedef struct	ps2INO_tdst_AnalogCapacity_
{
	BOOL	Enable;
	BOOL	Valid;
} ps2INO_tdst_AnalogCapacity;

typedef enum	ps2INO_tden_StickId_ { e_Right, e_Left, e_RightButtonInfo, e_LeftButtonInfo, e_LastStickId } ps2INO_tden_StickId;

typedef struct ps2INO_tdst_DualShock_
{
	/* offset */
	u_long8 Status : 8;			/* 0 */
	u_long8 Id : 4;				/* 1-0 */
	u_long8 Size : 4;			/* 1-4 */
	u_long8 SEL : 1;			/* 2-0 */
	u_long8 L3 : 1;				/* 2-1 */
	u_long8 R3 : 1;				/* 2-2 */
	u_long8 START : 1;			/* 2-3 */
	u_long8 UP : 1;				/* 2-4 */
	u_long8 RIGHT : 1;			/* 2-5 */
	u_long8 DOWN : 1;			/* 2-6 */
	u_long8 LEFT : 1;			/* 2-7 */
	u_long8 L2 : 1;				/* 3-0 */
	u_long8 R2 : 1;				/* 3-1 */
	u_long8 L1 : 1;				/* 3-2 */
	u_long8 R1 : 1;				/* 3-3 */
	u_long8 TRIANGLE : 1;		/* 3-4 */
	u_long8 CIRCLE : 1;			/* 3-5 */
	u_long8 CROSS : 1;			/* 3-6 */
	u_long8 SQUARE : 1;			/* 3-7 */
	u_long8 AnaStickRightX : 8; /* 4 */
	u_long8 AnaStickRightY : 8; /* 5 */
	u_long8 AnaStickLeftX : 8;	/* 6 */
	u_long8 AnaStickLeftY : 8;	/* 7 */
} ps2INO_tdst_DualShock;

typedef struct	ps2INO_tdst_DualShock2_
{
	/* offset */
	u_long8 Status : 8;			/* 0 */
	u_long8 LowerUnit : 4;		/* 1-0 */
	u_long8 UpperUnit : 4;		/* 1-4 */
	u_long8 SEL : 1;			/* 2-0 */
	u_long8 L3 : 1;				/* 2-1 */
	u_long8 R3 : 1;				/* 2-2 */
	u_long8 START : 1;			/* 2-3 */
	u_long8 UP : 1;				/* 2-4 */
	u_long8 RIGHT : 1;			/* 2-5 */
	u_long8 DOWN : 1;			/* 2-6 */
	u_long8 LEFT : 1;			/* 2-7 */
	u_long8 L2 : 1;				/* 3-0 */
	u_long8 R2 : 1;				/* 3-1 */
	u_long8 L1 : 1;				/* 3-2 */
	u_long8 R1 : 1;				/* 3-3 */
	u_long8 TRIANGLE : 1;		/* 3-4 */
	u_long8 CIRCLE : 1;			/* 3-5 */
	u_long8 CROSS : 1;			/* 3-6 */
	u_long8 SQUARE : 1;			/* 3-7 */
	u_long8 AnaStickRightX : 8; /* 4 */
	u_long8 AnaStickRightY : 8; /* 5 */
	u_long8 AnaStickLeftX : 8;	/* 6 */
	u_long8 AnaStickLeftY : 8;	/* 7 */
	u_long8 InfoR : 8;			/* 8 only DualShock2 */
	u_long8 InfoL : 8;			/* 9 */
	u_long8 InfoU : 8;			/* 10 */
	u_long8 InfoD : 8;			/* 11 */
	u_long8 InfoT : 8;			/* 12 */
	u_long8 InfoC : 8;			/* 13 */
	u_long8 InfoX : 8;			/* 14 */
	u_long8 InfoS : 8;			/* 15 */
	u_long8 InfoL1 : 8;			/* 16 */
	u_long8 InfoR1 : 8;			/* 17 */
	u_long8 InfoL2 : 8;			/* 18 */
	u_long8 InfoR2 : 8;			/* 19 */
} ps2INO_tdst_DualShock2;

typedef union	ps2INO_tdun_PadData_
{
	/* for quick acces */
	unsigned char			cArray[Mi_PadData_cArraySize];
	u_long64				lArray[Mi_PadData_lArraySize];

	/* for more convenient access */
	ps2INO_tdst_DualShock	DualShock;
	ps2INO_tdst_DualShock2	DualShock2;
} ps2INO_tdun_PadData;

typedef struct	ps2INO_tdst_Actuator_
{
	int		i_Nb;
	char	asz_ActDirect[6];
	char	asz_ActAlign[6];
} ps2INO_tdst_Actuator;

/*$2- variables ------------------------------------------------------------------------------------------------------*/
void                                ps2INO_NoPad2(void);

extern int                          AI_EvalFunc_IoConsLangGet_C(void);
extern int                          TEXT_i_GetLang(void);
extern int                          TEXT_gi_ChangedLang;

extern int 	                        WOR_gi_IsLoading;    
extern void                         GSP_DisplayMessageFullScreen(unsigned char *String);
extern void                         SND_TrackPauseAll(BOOL b_Stop);
extern void                         ps2SND_FlushModif(void);
extern void                         eeRPC_FlushCommandAfterEngine(void);
extern int							PS2_MenuIsOpen;
extern BOOL                         ps2MAI_gb_VideoModeNTSC;
extern BOOL                         LOA_gb_Loading;

extern								void ps2INO_SavStop( void );
extern								void ps2INO_SavReStart( void );

static u_long128					ps2INO_sx_dma0[scePadDmaBufferMax] __attribute__((aligned(64)));
static u_long128					ps2INO_sx_dma1[scePadDmaBufferMax] __attribute__((aligned(64)));
static ps2INO_tdun_PadData			ps2INO_stdun_CurrentPadData[Mi_MaxPortNumber];
static ps2INO_tdun_PadData			ps2INO_stdun_PreviousPadData[Mi_MaxPortNumber];
static u_long64						ps2INO_sx_PadMapping[e_LastButtonId];
static INO_tden_PortState			ps2INO_sx_PortState[Mi_MaxPortNumber];
static ps2INO_tdst_AnalogCapacity	ps2INO_sx_AnalogCapacity[Mi_MaxPortNumber];

static LONG							ps2INO_l_Joystick_XLeft = 0;
static LONG							ps2INO_l_Joystick_XLeftStart = 22000;
static LONG							ps2INO_l_Joystick_XRightStart = 42000;
static LONG							ps2INO_l_Joystick_XRight = 65536;

static LONG							ps2INO_l_Joystick_YUp = 0;
static LONG							ps2INO_l_Joystick_YUpStart = 22000;
static LONG							ps2INO_l_Joystick_YDownStart = 42000;
static LONG							ps2INO_l_Joystick_YDown = 65536;

ps2INO_tdst_Actuator				ps2INO_gax_Actuator[Mi_MaxPortNumber];

int                                 ps2INO_gai_ErrTimeOut[Mi_MaxPortNumber];

int                                 ps2INO_gi_PadError = 0;
int                                 ps2INO_gi_PadConfirm = 0;
int                                 ps2INO_gi_PadStart = 0;
int                                 ps2INO_gai_Check[Mi_MaxPortNumber];
int                                 ps2INO_gi_NoStart = 0;


char                                *ps2INO_gasz_PadError[2][INO_e_MaxLangNb] =
{
    {//PAL
 /*French		*/"Aucune manette analogique (DUALSHOCK®2)\ndétectée. Veuillez insérer\nune manette analogique (DUALSHOCK®2)\ndans le port de manette N°1.",
 /*English		*/"The analog controller (DUALSHOCK®2)\nin controller port 1 is disconnected.\n\nPlease reconnect the analog\ncontroller (DUALSHOCK®2)\nin controller port 1.",
 /*Danish		*/"Den analog controller (DUALSHOCK®2)\ni controller-port 1 er blevet\nfrakoblet. Tilslut venligst den\nanalog controller (DUALSHOCK®2)\ni controller-port 1.",
 /*Dutch		*/"Geen analoge controller\n(DUALSHOCK®2) gedetecteerd. Plaats een\nanaloge controller (DUALSHOCK®2)\nin controller-aansluiting 1.",
 /*Finnish		*/"Ohjainportti 1:ssä oleva analoginen\nohjain (DUALSHOCK®2) on irrotettu.\nKytke analoginen ohjain\n (DUALSHOCK®2) ohjainportti 1:een.",
 /*German		*/"Kein Analog Controller (DUALSHOCK®2)\nerkannt. Bitte schließen Sie\neinen Analog Controller (DUALSHOCK®2)\nan Controller-Anschluss 1 an.",
 /*Italian		*/"Nessun controller analogico\n(DUALSHOCK®2) rilevato. Inserisci\nun controller analogico (DUALSHOCK®2)\nnell’ingresso controller 1.",
 /*Spanish		*/"El mando analógico (DUALSHOCK®2) está\ndesconectado. Por favor, inserta\nun mando analógico (DUALSHOCK®2)\nen el puerto de mando 1.",
 /*Portuguese	*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Swedish		*/"Den analog handkontroll\n(DUALSHOCK®2) i handkontrolls port 1\när urkopplad. Koppla in den analog\nhandkontroll (DUALSHOCK®2)\ni handkontrolls port 1.",
 /*Polish		*/"POL:Kontroler analogowy\n(DUALSHOCK®2) w porcie 1 jest od³¹czony.\nPod³¹cz ponownie kontroler\nanalogowy (DUALSHOCK®2)w porcie 1.",
 /*Russian		*/"RUS:Àíàëîãîâûé êîíòðîëëåð (DUALSHOCK®2)\nîòñóòñòâóåò â ïîðòå êîíòðîëëåðà 1.\nÂñòàâüòå àíàëîãîâûé\nêîíòðîëëåð (DUALSHOCK®2)\nâ ïîðò êîíòðîëëåðà 1.",
 /*Japanese		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Chinese		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Albanian		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Arabic		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Bulgarian	*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",	
 /*Byelorussian */"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Greek		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",	
 /*Korean		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Norwegian	*/"Den analog håndkontroll\n(DUALSHOCK®2) i kontrollerport 1\ner frakoblet.Sett inn igjen den\nanalog håndkontroll\n(DUALSHOCK®2) i kontrollerport 1.",	
 /*Romanian		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Serbian		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Slovak		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Slovenian	*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Turkish		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Czech		*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Hungarian	*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1.",
 /*Taiwanese	*/"No analog controller (DUALSHOCK®2) \ndetected. Please insert an \nanalog controller (DUALSHOCK®2)\nin controller port 1."
    },
 /*-----------------------------------*/   
    {//NTSC
 /*French		*/"Aucune manette analogique (DUALSHOCK®2)\ndétectée. Veuillez insérer\nune manette analogique (DUALSHOCK®2)\ndans le port de manette N°1.",
 /*English		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",        
 /*Danish		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",        
 /*Dutch		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Finnish		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",        
 /*German		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",        
 /*Italian		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",        
 /*Spanish		*/"No control analógico (DUALSHOCK®2)\ndetetado. Por favor inserta\nun control analógico (DUALSHOCK®2)\n en el puerto de control 1.",
 /*Portuguese	*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",        
 /*Swedish		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",        
 /*Polish		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",        
 /*Russian		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Japanese		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",       
 /*Chinese		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Albanian		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Arabic		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Bulgarian	*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Byelorussian */"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Greek		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Korean		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Norwegian	*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Romanian		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Serbian		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Slovak		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Slovenian	*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Turkish		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Czech		*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Hungarian	*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1.",
 /*Taiwanese	*/"No DUALSHOCK®2 analog controller \ndetected. Please insert a\nDUALSHOCK®2 analog controller\nin controller port 1."
    }
};



char                                *ps2INO_gasz_PadConfirm[INO_e_MaxLangNb] =
{
/*French		*/"Appuyer\nsur la touche START pour continuer.",
/*English		*/"Press\nSTART button to continue.",
/*Danish		*/"Tryk\npå START tast for at fortsætte.",
/*Dutch		    */"Druk op de\nSTART-toets om verder te gaan.",
/*Finnish		*/"Jatka painamalla START-näppäin.",
/*German		*/"Zum Fortsetzen\nSTART-Taste drücken.",
/*Italian		*/"Premi il tasto\nSTART per continuare.",
/*Spanish		*/"Pulsa el botón\nSTART para continuar.",
/*Portuguese	*/"Press\nSTART button to continue.",
/*Swedish		*/"Tryck på\nSTART-knapp för att fortsätta.",
/*Polish		*/"POL:Wciœnij przycisk\nSTART by kontynuowaæ.",
/*Russian		*/"RUS:Íàæìèòå êíîïêó START (ïóñê),\n÷òîáû ïðîäîëæèòü.",
/*Japanese		*/"Press\nSTART button to continue.",
/*Chinese		*/"Press\nSTART button to continue.",
/*Albanian		*/"Press\nSTART button to continue.",
/*Arabic		*/"Press\nSTART button to continue.",
/*Bulgarian	    */"Press\nSTART button to continue.",
/*Byelorussian  */"Press\nSTART button to continue.",
/*Greek		    */"Press\nSTART button to continue.",
/*Korean		*/"Press\nSTART button to continue.",
/*Norwegian	    */"Trykk på\nSTART-knapp for å fortsette.",
/*Romanian		*/"Press\nSTART button to continue.",
/*Serbian		*/"Press\nSTART button to continue.",
/*Slovak		*/"Press\nSTART button to continue.",
/*Slovenian	    */"Press\nSTART button to continue.",
/*Turkish		*/"Press\nSTART button to continue.",
/*Czech		    */"Press\nSTART button to continue.",
/*Hungarian	    */"Press\nSTART button to continue.",
/*Taiwanese	    */"Press\nSTART button to continue."
};

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ps2INO_Joystick_Init(HWND none)
{
	/*~~~~~~~~*/
	INT i, code;
	INT port;
	/*~~~~~~~~*/

	INO_gi_CurrentPadId = -1;
	ps2INO_gi_PadError = 0;
	ps2INO_gi_PadConfirm = 0;
	L_memset(ps2INO_gai_ErrTimeOut, 0, Mi_MaxPortNumber*sizeof(int));
	L_memset(ps2INO_gai_Check, 0, Mi_MaxPortNumber*sizeof(int));

	/* reset data variables */
	for(port = 0; port < Mi_MaxPortNumber; port++)
	{
		/* state init */
		ps2INO_sx_PortState[port] = eState_Init;

		/* reset analog data validity */
		ps2INO_sx_AnalogCapacity[port].Enable = FALSE;
		ps2INO_sx_AnalogCapacity[port].Valid = FALSE;

		/* data init */
		for(i = 0; i < Mi_PadData_lArraySize; i++)
		{
			ps2INO_stdun_CurrentPadData[port].lArray[i] = 0;	/* 0xffffffffffffffff; */
			ps2INO_stdun_PreviousPadData[port].lArray[i] = 0;	/* 0xffffffffffffffff; */
		}

		ps2INO_gax_Actuator[port].i_Nb = 0;
		for(i = 0; i < 6; i++)
		{
			ps2INO_gax_Actuator[port].asz_ActDirect[i] = 0;
			ps2INO_gax_Actuator[port].asz_ActAlign[i] = 0;
		}
	}

	/*
	 * Pad mapping £
	 * first 64 bits word
	 */
	ps2INO_sx_PadMapping[e_Status] = 0x00000000000000ff;
	ps2INO_sx_PadMapping[e_LowerUnit] = 0x0000000000000f00;
	ps2INO_sx_PadMapping[e_UpperUnit] = 0x000000000000f000;
	ps2INO_sx_PadMapping[e_SEL] = 0x0000000000010000;
	ps2INO_sx_PadMapping[e_L3] = 0x0000000000020000;
	ps2INO_sx_PadMapping[e_R3] = 0x0000000000040000;
	ps2INO_sx_PadMapping[e_START] = 0x0000000000080000;
	ps2INO_sx_PadMapping[e_UP] = 0x0000000000100000;
	ps2INO_sx_PadMapping[e_RIGHT] = 0x0000000000200000;
	ps2INO_sx_PadMapping[e_DOWN] = 0x0000000000400000;
	ps2INO_sx_PadMapping[e_LEFT] = 0x0000000000800000;
	ps2INO_sx_PadMapping[e_L2] = 0x0000000001000000;
	ps2INO_sx_PadMapping[e_R2] = 0x0000000002000000;
	ps2INO_sx_PadMapping[e_L1] = 0x0000000004000000;
	ps2INO_sx_PadMapping[e_R1] = 0x0000000008000000;
	ps2INO_sx_PadMapping[e_TRIANGLE] = 0x0000000010000000;
	ps2INO_sx_PadMapping[e_CIRCLE] = 0x0000000020000000;
	ps2INO_sx_PadMapping[e_CROSS] = 0x0000000040000000;
	ps2INO_sx_PadMapping[e_SQUARE] = 0x0000000080000000;
	ps2INO_sx_PadMapping[e_AnaStickRightX] = 0x000000ff00000000;
	ps2INO_sx_PadMapping[e_AnaStickRightY] = 0x0000ff0000000000;
	ps2INO_sx_PadMapping[e_AnaStickLeftX] = 0x00ff000000000000;
	ps2INO_sx_PadMapping[e_AnaStickLeftY] = 0xff00000000000000;

	/* second 64 bits word */
	ps2INO_sx_PadMapping[e_InfoR] = 0x00000000000000ff;
	ps2INO_sx_PadMapping[e_InfoL] = 0x000000000000ff00;
	ps2INO_sx_PadMapping[e_InfoU] = 0x0000000000ff0000;
	ps2INO_sx_PadMapping[e_InfoD] = 0x00000000ff000000;
	ps2INO_sx_PadMapping[e_InfoT] = 0x000000ff00000000;
	ps2INO_sx_PadMapping[e_InfoC] = 0x0000ff0000000000;
	ps2INO_sx_PadMapping[e_InfoX] = 0x00ff000000000000;
	ps2INO_sx_PadMapping[e_InfoS] = 0xff00000000000000;

	/* third 64 bits word */
	ps2INO_sx_PadMapping[e_InfoL1] = 0x00000000000000ff;
	ps2INO_sx_PadMapping[e_InfoR1] = 0x000000000000ff00;
	ps2INO_sx_PadMapping[e_InfoL2] = 0x0000000000ff0000;
	ps2INO_sx_PadMapping[e_InfoR2] = 0x00000000ff000000;

	/*
	 * RPC init + IRX loading £
	 * is done in main £
	 * initialize pad lib
	 */
	code = scePadInit(0);
	ERR_X_Assert(code == Mi_scePadSuccessCode);

	/* open all ports */
	code = scePadPortOpen(0, Mi_PadDefault_Slot, ps2INO_sx_dma0);
	ERR_X_Assert(code == Mi_scePadSuccessCode);

	code = scePadPortOpen(1, Mi_PadDefault_Slot, ps2INO_sx_dma1);
	ERR_X_Assert(code == Mi_scePadSuccessCode);

	INO_l_Joystick_Enable = 1;
	
	INO_Update();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ps2INO_Joystick_Close(void)
{
	/*~~~~~*/
	int port;
	int code;
	/*~~~~~*/

	/* close all opened ports */
	for(port = 0; port < Mi_MaxPortNumber; port++)
	{
		code = scePadPortClose(port, Mi_PadDefault_Slot);
		ERR_X_Assert(code == Mi_scePadSuccessCode);
	}

	/* close the pad library */
	code = scePadEnd();
	ERR_X_Assert(code == Mi_scePadSuccessCode);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ps2INO_tdun_PadData data __attribute__((aligned(64)));
void ps2INO_Pad_Move(int _port, ps2INO_tden_StickId _stick_id, MATH_tdst_Vector *_pst_Move);
u32 ps2INO_IsJOYTouched(u32 NoMushrooms)
{
	u_long64 DualShock2;
	MATH_tdst_Vector st_Move;
	
    if( ps2INO_gi_PadStart < Mi_PadStartTime) return 0;
    
    
    if (!NoMushrooms)
    {
		ps2INO_Pad_Move(0, e_Right , &st_Move);
		if (st_Move.x * st_Move.x + st_Move.y * st_Move.y > 0.45f) 
			return 1;
		ps2INO_Pad_Move(0, e_Left, &st_Move);
		if (st_Move.x * st_Move.x + st_Move.y * st_Move.y > 0.45f) 
			return 1;
    }
    if (NoMushrooms == 2)
		DualShock2 = ps2INO_stdun_CurrentPadData[0].lArray[0] & (ps2INO_sx_PadMapping[e_START]|ps2INO_sx_PadMapping[e_CROSS]);
    else
		DualShock2 = ps2INO_stdun_CurrentPadData[0].lArray[0] & 
						(ps2INO_sx_PadMapping[e_SEL]
						|ps2INO_sx_PadMapping[e_L3]
						|ps2INO_sx_PadMapping[e_R3]
						|ps2INO_sx_PadMapping[e_START]
						|ps2INO_sx_PadMapping[e_UP]
						|ps2INO_sx_PadMapping[e_RIGHT]
						|ps2INO_sx_PadMapping[e_DOWN]
						|ps2INO_sx_PadMapping[e_LEFT]
						|ps2INO_sx_PadMapping[e_L2]
						|ps2INO_sx_PadMapping[e_R2]
						|ps2INO_sx_PadMapping[e_L1]
						|ps2INO_sx_PadMapping[e_R1]
						|ps2INO_sx_PadMapping[e_TRIANGLE]
						|ps2INO_sx_PadMapping[e_CIRCLE]
						|ps2INO_sx_PadMapping[e_CROSS]
						|ps2INO_sx_PadMapping[e_SQUARE]);
	
	return DualShock2 != 0;
}
u32 ps2INO_Transform_SONYLAND_2_JADELAND(u32 SONYLAND)
{
	if (ps2MAI_gb_VideoModeNTSC)
	switch (SONYLAND)
	{
		case SCE_DEMO_LANGUAGE_ENGLISH    :return INO_e_English;
		case SCE_DEMO_LANGUAGE_FRENCH     :return INO_e_English;
		case SCE_DEMO_LANGUAGE_GERMAN     :return INO_e_English;
		case SCE_DEMO_LANGUAGE_SPANISH    :return INO_e_Spanish;
		case SCE_DEMO_LANGUAGE_ITALIAN    :return INO_e_English;
		case SCE_DEMO_LANGUAGE_DUTCH      :return INO_e_English;
	}
	else
	switch (SONYLAND)
	{
		case SCE_DEMO_LANGUAGE_ENGLISH    :return INO_e_English;
		case SCE_DEMO_LANGUAGE_FRENCH     :return INO_e_French;
		case SCE_DEMO_LANGUAGE_GERMAN     :return INO_e_German;
		case SCE_DEMO_LANGUAGE_SPANISH    :return INO_e_Spanish;
		case SCE_DEMO_LANGUAGE_ITALIAN    :return INO_e_Italian;
		case SCE_DEMO_LANGUAGE_DUTCH      :return INO_e_Dutch;
	}
	return INO_e_English;
}

void ps2INO_NoPad1(void)
{
    int lang, region;
    char*p;
    extern void SND_MuteAll(BOOL);
    extern void SND_Update(MATH_tdst_Matrix*);
    
    lang = (TEXT_gi_ChangedLang>1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
    if(!INO_b_LanguageIsPresent(lang)) lang = INO_e_English;
    region = ps2MAI_gb_VideoModeNTSC ? 1 : 0;

	if (Demo_gb_IsActive)
	{
		// Alors ça change tout. DEBILODROME.
		lang = ps2INO_Transform_SONYLAND_2_JADELAND(Demo_gus_language);
	}

	ps2INO_SavStop();       
    SND_MuteAll(TRUE);
    SND_TrackPauseAll(TRUE);
    SND_Update(NULL);
    
    ps2SND_FlushModif();
    eeRPC_FlushCommandAfterEngine();
    
    do 
    {
        INO_Joystick_Update();
        if(ps2INO_sx_PortState[0] == eState_TreatData) break;
                
        GSP_DisplayMessageFullScreen(ps2INO_gasz_PadError[region][lang]);
    } while (1);


    do 
    {
        INO_Joystick_Update();
        if(ps2INO_uc_GetButton(0, e_START)) break;
                
        GSP_DisplayMessageFullScreen(ps2INO_gasz_PadConfirm[lang]);
    } while (1);

    SND_MuteAll(FALSE);
    SND_TrackPauseAll(FALSE);
    SND_Update(NULL);
    ps2INO_SavReStart();
    
    ps2SND_FlushModif();
    eeRPC_FlushCommandAfterEngine();
}

void ps2INO_NoPad2(void)
{
    int lang, region;
    char asz[512];
    char*p;
    extern void SND_MuteAll(BOOL);
    extern void SND_Update(MATH_tdst_Matrix*);
    
    lang = (TEXT_gi_ChangedLang>1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
    if(!INO_b_LanguageIsPresent(lang)) lang = INO_e_English;
    region = ps2MAI_gb_VideoModeNTSC ? 1 : 0;

	if (Demo_gb_IsActive)
	{
		// Alors ça change tout. DEBILODROME.
		lang = ps2INO_Transform_SONYLAND_2_JADELAND(Demo_gus_language);
	}

    L_strcpy(asz, ps2INO_gasz_PadError[region][lang]);
    do
    {
        p = L_strchr(asz, '1');
        if(p) *p = '2';
    } while(p);
    
    SND_MuteAll(TRUE);
    SND_TrackPauseAll(TRUE);
    SND_Update(NULL);
    ps2INO_SavStop();       
    
    ps2SND_FlushModif();
    eeRPC_FlushCommandAfterEngine();
    
    do 
    {
        INO_Joystick_Update();
        if(ps2INO_sx_PortState[1] == eState_TreatData) break;
                
        GSP_DisplayMessageFullScreen(asz);
    } while (1);


    do 
    {
        INO_Joystick_Update();
        if(ps2INO_uc_GetButton(1, e_START)) break;
                
        GSP_DisplayMessageFullScreen(ps2INO_gasz_PadConfirm[lang]);
    } while (1);

    SND_MuteAll(FALSE);
    SND_TrackPauseAll(FALSE);
    SND_Update(NULL);
    ps2INO_SavReStart();
    
    ps2SND_FlushModif();
    eeRPC_FlushCommandAfterEngine();
}

static void ps2INO_Joystick_Update(INO_tdst_GenericPad *pCurr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					state, i;
	int					port;
	int					id, exid;
	char				error_msg[100];
	extern int			GSP_scePadRead(int port, int slot, unsigned char *);
	static int          si_EnablePad[Mi_MaxPortNumber]={0};
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef _FINAL_
#define M_scePadRead scePadRead
#else
#define M_scePadRead GSP_scePadRead
#endif
        

    if(LOA_gb_Loading) ps2INO_gi_PadStart=0;
    LOA_gb_Loading = FALSE;
    
    do {
        	for(port = 0; port < Mi_MaxPortNumber; port++)
        	{
        		for(i = 0; i < Mi_PadData_lArraySize; i++)
        		{
        			ps2INO_stdun_PreviousPadData[port].lArray[i] = ps2INO_stdun_CurrentPadData[port].lArray[i];
        		}

        		/* read current state */
        		state = scePadGetState(port, 0);
        		if(state == scePadStateDiscon)
        		{
        		    ps2INO_sx_PortState[port] = eState_Init;        		    
        		}
        		else if(state == scePadStateClosed)
        		{
        		    ps2INO_sx_PortState[port] = eState_Init;        		    
        		}
        		

        		if(ps2INO_gai_ErrTimeOut[port] > Mi_ErrTimeOutMax) 
        		    ps2INO_sx_PortState[port] = eState_Init;
        		
        		if(state == scePadStateExecCmd) continue;

        		/* if disconnected then re-enter in the automat */
                if(state != scePadStateStable && state != scePadStateFindCTP1)
                {
        		    ps2INO_sx_PortState[port] = eState_Init;
        		}

        		switch(ps2INO_sx_PortState[port])
        		{
        		/* first state */
        		case eState_Init:
        		    ps2INO_gai_ErrTimeOut[port]=0;
        			if(INO_gi_CurrentPadId == port) 
        			{
        			    INO_gi_CurrentPadId = -1;
        			    si_EnablePad[port] = 0;
        			}

        			/* if we can access to the port; then go next one */
        			if(state != scePadStateStable) break;

        			/* read its ID */
        			if((id = scePadInfoMode(port, Mi_PadDefault_Slot, InfoModeCurID, 0)) == 0) break;

        			/* get its extended ID */
        			if((exid = scePadInfoMode(port, Mi_PadDefault_Slot, InfoModeCurExID, 0)) == 0) id = exid;

        			/* treat the pad ID */
        			if(id == (int) e_DualShock)
        				ps2INO_sx_PortState[port] = eState_InitDualShock;
        			else if(id == (int) e_DualShock2)
        				ps2INO_sx_PortState[port] = eState_InitDualShock2;
        			else
        				ps2INO_sx_PortState[port] = eState_TreatData;

        			break;
        		
        		case eState_InitDualShock:
        			/* test the analog mode */
        			if(scePadInfoMode(port, Mi_PadDefault_Slot, InfoModeIdTable, -1) == 0)
        			{
        				ps2INO_sx_PortState[port] = eState_TreatData;
        				break;
        			}

        			ps2INO_sx_PortState[port] = eState_EnterAnalogMode;

        		case eState_EnterAnalogMode:
        			if(scePadSetMainMode(port, Mi_PadDefault_Slot, InfoModeCurID, 3) == Mi_scePadSuccessCode)
        			{
        				ps2INO_sx_PortState[port] = eState_CheckAnalogMode;
        			}
        			break;

        		case eState_CheckAnalogMode:
        			if(scePadGetReqState(port, Mi_PadDefault_Slot) == scePadReqStateFailed)
        			{			    
        				ps2INO_sx_PortState[port] = eState_EnterAnalogMode;
        			}

        			if(scePadGetReqState(port, Mi_PadDefault_Slot) == scePadReqStateComplete)
        			{
        				ps2INO_sx_PortState[port] = eState_Init;
        				ps2INO_sx_AnalogCapacity[port].Enable = TRUE;
        			}
        			break;

        		case eState_InitDualShock2:
        			if(scePadInfoPressMode(port, Mi_PadDefault_Slot) == Mi_scePadSuccessCode)
        			{
        				ps2INO_sx_PortState[port] = eState_EnterPressMode;
        				si_EnablePad[port] = 1;
        				break;
        			}
        			else
        			{
        			    ps2INO_sx_PortState[port] = eState_Init;
        			}
        			break;

        		case eState_EnterPressMode:
        			if(scePadEnterPressMode(port, Mi_PadDefault_Slot) == Mi_scePadSuccessCode)
        			{
        				ps2INO_sx_PortState[port] = eState_CheckPressMode;
        			}
        			break;

        		case eState_CheckPressMode:
        			if(scePadGetReqState(port, Mi_PadDefault_Slot) == scePadReqStateFailed)
        			{
        				ps2INO_sx_PortState[port] = eState_EnterPressMode;
        			}

        			if(scePadGetReqState(port, Mi_PadDefault_Slot) == scePadReqStateComplete)
        			{
        				ps2INO_sx_PortState[port] = eState_EnterVibrationMode;
        				ps2INO_sx_AnalogCapacity[port].Enable = TRUE;
        			}
        			break;

        		case eState_EnterVibrationMode:
        			ps2INO_gax_Actuator[port].i_Nb = scePadInfoAct(port, Mi_PadDefault_Slot, -1, 0);
        			if(ps2INO_gax_Actuator[port].i_Nb == 0)
        			{
        				ps2INO_sx_PortState[port] = eState_TreatData;
        			}
        			else
        			{
        				ps2INO_gax_Actuator[port].asz_ActAlign[0] = 0;
        				ps2INO_gax_Actuator[port].asz_ActAlign[1] = 1;
        				for(i = 2; i < 6; i++) ps2INO_gax_Actuator[port].asz_ActAlign[i] = 0xff;
        				if(scePadSetActAlign(port, Mi_PadDefault_Slot, ps2INO_gax_Actuator[port].asz_ActAlign))
        				{
        					ps2INO_sx_PortState[port] = eState_CheckVibrationMode;
        				}
        			}
        			break;

        		case eState_CheckVibrationMode:
        			if(scePadGetReqState(port, Mi_PadDefault_Slot) == scePadReqStateFaild)
        				ps2INO_sx_PortState[port] = eState_EnterVibrationMode;
        			if(scePadGetReqState(port, Mi_PadDefault_Slot) == scePadReqStateComplete)
        			{			
        				ps2INO_sx_PortState[port] = eState_TreatData;
        			}
        			break;

        		case eState_TreatData:
        			if(state == scePadStateError || !si_EnablePad[port])
        			{
        				ps2INO_sx_PortState[port] = eState_Init;
        				break;
        			}

        			if(state == scePadStateStable || state == scePadStateFindCTP1)
        			{
        			    if((port == 0) && (INO_gi_CurrentPadId != port)) 
        			    {
        			        INO_gi_CurrentPadId = port;
        			    }
        			    
        				if(INO_gi_CurrentPadId == -1) 
        				{
        				    INO_gi_CurrentPadId = port;
        				}
        				
        				if(ps2INO_sx_AnalogCapacity[port].Enable) ps2INO_sx_AnalogCapacity[port].Valid = TRUE;

        				/* refresh data if necessary */
            			if(M_scePadRead(port, Mi_PadDefault_Slot, data.cArray) == SCE_PAD_BUTTON_BUFFER_SIZE)
        				{
        				    ps2INO_gai_Check[port]++;
        				    if(ps2INO_gai_Check[port] > Mi_PadCheckTime) 
        				    {
        				        int ret;

        				        ret = scePadInfoMode(port, Mi_PadDefault_Slot, InfoModeCurID, -1);
        				        if( (ret & 0xF) != 0x7) ps2INO_sx_PortState[port] = eState_Init;
                    		    ps2INO_gai_Check[port] = 0;		
        				    }
        				    
        				    ps2INO_gai_ErrTimeOut[port] = 0 ;
        				    
        				    
        					/* 0 */
        					ps2INO_stdun_CurrentPadData[port].lArray[0] = 0;
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (data.lArray[0] & ps2INO_sx_PadMapping[e_Status]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (data.lArray[0] & ps2INO_sx_PadMapping[e_LowerUnit]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (data.lArray[0] & ps2INO_sx_PadMapping[e_UpperUnit]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (data.lArray[0] & ps2INO_sx_PadMapping[e_AnaStickRightX]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (data.lArray[0] & ps2INO_sx_PadMapping[e_AnaStickRightY]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (data.lArray[0] & ps2INO_sx_PadMapping[e_AnaStickLeftX]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (data.lArray[0] & ps2INO_sx_PadMapping[e_AnaStickLeftY]);

        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_SEL]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_L3]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_R3]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_START]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_UP]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_RIGHT]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_DOWN]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_LEFT]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_L2]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_R2]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_L1]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_R1]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_TRIANGLE]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_CIRCLE]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_CROSS]);
        					ps2INO_stdun_CurrentPadData[port].lArray[0] |= (~data.lArray[0] & ps2INO_sx_PadMapping[e_SQUARE]);
        					
        					//Demo_gb_IsActive = TRUE;
//        					if (Demo_gb_IsActive)
/*        					{
	        					if ((~data.lArray[0] & ps2INO_sx_PadMapping[e_SEL]) && (~data.lArray[0] & ps2INO_sx_PadMapping[e_START]))
	        					{
									extern BOOL ENG_gb_ExitApplication;
									extern BOOL ENG_gb_ForceEndEngine;
	        						ENG_gb_ExitApplication = TRUE; 
	        						ENG_gb_ForceEndEngine = TRUE; 
	        					}
        					}//*/


        					/* 1-2-3 */
        					ps2INO_stdun_CurrentPadData[port].lArray[1] = data.lArray[1];
        					ps2INO_stdun_CurrentPadData[port].lArray[2] = data.lArray[2];
        					ps2INO_stdun_CurrentPadData[port].lArray[3] = data.lArray[3];
#ifndef _FINAL_
                            if (INO_b_RecordInput)
                            {
                                if (INO_n_FrameCounter == 0) 
								{
									/* Create file at the same pos as exe */
									sprintf(INO_dumpFileName,INPUT_REC_FILE,WOR_gul_WorldKey);
									if(!(L_access(INO_dumpFileName, 0))) L_chmod(INO_dumpFileName, L_S_IREAD | L_S_IWRITE);

                                    INO_dumpFile = L_fopen(INO_dumpFileName, SCE_WRONLY | SCE_CREAT); 
									L_fwriteA(&INO_n_FrameCounter, sizeof(INO_n_FrameCounter), 1, INO_dumpFile);   
								}

                                L_fwriteA((void *) &ps2INO_stdun_CurrentPadData[port], sizeof(ps2INO_tdun_PadData), 1, INO_dumpFile);    

                                INO_n_NumRecord = INO_n_FrameCounter;
                            }
                            else if (INO_b_PlayInput)
                            {
                                if (INO_n_FrameCounter == 0)    
                                { 
                                    INO_dumpFile = L_fopen(INO_dumpFileName, L_fopen_RB);       
                                    L_freadA(&INO_n_NumRecord, sizeof(INO_n_NumRecord), 1, INO_dumpFile);  
                                }

                                L_freadA((void *) &ps2INO_stdun_CurrentPadData[port], sizeof(ps2INO_tdun_PadData), 1, INO_dumpFile);    

                                if (INO_n_FrameCounter >= INO_n_NumRecord)   
                                    INO_b_PlayInput = FALSE;
                            }
                            else if (INO_n_FrameCounter != 0)
                            {
								if ( INO_dumpFile ) 
								{
									// going to the begining of the file
									L_fseek(INO_dumpFile,0,0);
									// overwriting number of frames
									L_fwriteA(&INO_n_FrameCounter, sizeof(INO_n_FrameCounter), 1, INO_dumpFile);   
									L_fclose(INO_dumpFile);        
								}
                                INO_n_FrameCounter = 0;
                            }
#endif                            
        				}
            			else
            			{
            				ps2INO_sx_PortState[port] = eState_Init;
            				break;
            			}
        			}
        			else
        			{
        				ps2INO_sx_PortState[port] = eState_Init;
        				break;
        			}
        			break;

        		default:
        			sscanf(error_msg, "** Unknown Pad state, port %d", port);
        			ERR_X_ForceError(error_msg, NULL);
        			break;
        		}
        	}

    	for(port = 0; port < Mi_MaxPortNumber; port++)
    	{
    		for(i = 0; i < eBtn_GenericButtonNb; i++)
    		{
    			pCurr[port].ac_Button[i] = ps2INO_uc_GetButton(port, i);    			
    		}
    		if(ps2INO_gi_NoStart) pCurr[port].ac_Button[eBtn_Start] = 0;
    	}
    	if(ps2INO_gi_NoStart) ps2INO_gi_NoStart--;
    	
        ps2INO_CheckPort0();
        
        } while( ps2INO_b_Port0IsKo() );

#undef M_scePadRead	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ps2INO_b_Port0IsKo(void) 
{
#ifdef TRC_PS2
    return  ((ps2INO_gi_PadError > Mi_PadErrorMax) || ps2INO_gi_PadConfirm);
#else
	return  FALSE;
#endif
}

void ps2INO_SetControlMode(int mode)
{
#ifdef JAPANESE_VERSION
	switch(mode)
	{
	case IO_MODE_JACK:
	case IO_MODE_KONG:
		IO_bJAPANESE_MENU = FALSE;
		break;

	case IO_MODE_MENU:
		IO_bJAPANESE_MENU = TRUE;
		break;
	}
#endif // JAPANESE_VERSION
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2INO_CheckPort0(void)
{
    extern unsigned int     ui_LastVideoKey;
   	extern ULONG            ulVideoScreentValid;
   	static unsigned int     ui_VideoToLaunch=-1;
   	extern void             SND_MuteAll(BOOL b_Mute);
   	extern void             SND_Update(MATH_tdst_Matrix *_pst_Camera);
    extern void             GFX_FS_VideoStart();
    extern void             GFX_FS_VideoStop();
    int lang, region;



#ifdef TRC_PS2
    if(ps2INO_gi_PadStart++ < Mi_PadStartTime) return;

    if(ps2INO_sx_PortState[0] != eState_TreatData)
    {
        
        ps2INO_gi_PadError++;
        if(WOR_gi_IsLoading) ps2INO_gi_PadError = 0;
        
        if(ps2INO_gi_PadError > Mi_PadErrorMax)
        {
            
            
            lang = (TEXT_gi_ChangedLang>1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
            if(!INO_b_LanguageIsPresent(lang)) lang = INO_e_English;
            region = ps2MAI_gb_VideoModeNTSC ? 1 : 0;

			if (Demo_gb_IsActive)
			{
				// Alors ça change tout. DEBILODROME.
				lang = ps2INO_Transform_SONYLAND_2_JADELAND(Demo_gus_language);
			}
            
            
            GSP_DisplayMessageFullScreen(ps2INO_gasz_PadError[region][lang]);
            ps2INO_gi_PadConfirm++;
        }
            
        if(ps2INO_gi_PadError == Mi_PadErrorMax)
        {
            if(ulVideoScreentValid) 
            {
                ui_VideoToLaunch = ui_LastVideoKey;
                GFX_FS_VideoStop();
            }
            else
            {
                ui_VideoToLaunch = -1;
            }
            
            
            SND_MuteAll(TRUE);
            SND_TrackPauseAll(TRUE);
            SND_Update(NULL);
            ps2INO_SavStop();
            
            ps2SND_FlushModif();
            eeRPC_FlushCommandAfterEngine();
        }

    }
    else
    {
        if(ps2INO_gi_PadConfirm > 15)
        {
          
            lang = (TEXT_gi_ChangedLang>1) ? TEXT_i_GetLang() : AI_EvalFunc_IoConsLangGet_C();
            if(!INO_b_LanguageIsPresent(lang)) lang = INO_e_English;
            
			if (Demo_gb_IsActive)
			{
				// Alors ça change tout. DEBILODROME.
				lang = ps2INO_Transform_SONYLAND_2_JADELAND(Demo_gus_language);
			}
            
            
            GSP_DisplayMessageFullScreen(ps2INO_gasz_PadConfirm[lang]);
            
            if(ps2INO_uc_GetButton(0, e_START))
            {
                INO_gst_CurrPad[0].ac_Button[eBtn_Start] = 0;
                INO_gst_PrevPad[0].ac_Button[eBtn_Start] = 0;
                ps2INO_gi_PadConfirm = 0;
                ps2INO_gi_NoStart = 10;
                
                if(ui_VideoToLaunch != -1)
                {
                    GFX_FS_VideoStart(ui_VideoToLaunch);
                    ui_VideoToLaunch = -1;
                }
                    
                SND_MuteAll(FALSE);
                SND_TrackPauseAll(FALSE);
                SND_Update(NULL);
                ps2INO_SavReStart();       
                
                ps2SND_FlushModif();
                eeRPC_FlushCommandAfterEngine();
            }
        }
        else
        {
            ps2INO_gi_PadConfirm = 0;
        }
        
        ps2INO_gi_PadError = 0;
    }
#endif    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ps2INO_b_Joystick_IsValid(void)
{
	if(INO_gi_CurrentPadId == -1) return FALSE;

	if
	(
		(ps2INO_sx_PortState[INO_gi_CurrentPadId] == eState_TreatData)
	&&	(ps2INO_gax_Actuator[INO_gi_CurrentPadId].i_Nb == 2)
	) return TRUE;
	else
		return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int ps2INO_i_SetCurrentPad(int id)
{
	switch(id)
	{
	case 0:
	case 1:
		if(ps2INO_sx_PortState[id] == eState_TreatData)
			INO_gi_CurrentPadId = id;
		else
			return -1;
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
static void ps2INO_PadLowVibSet(BOOL _b_On)
{
	if(INO_gi_CurrentPadId == -1) return;

	if(ps2INO_gax_Actuator[INO_gi_CurrentPadId].i_Nb == 2)
	{
		ps2INO_gax_Actuator[INO_gi_CurrentPadId].asz_ActDirect[0] = (_b_On) ? 1 : 0;
		scePadSetActDirect
		(
			INO_gi_CurrentPadId,
			Mi_PadDefault_Slot,
			ps2INO_gax_Actuator[INO_gi_CurrentPadId].asz_ActDirect
		);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ps2INO_PadHightVibSet(int _i_level)
{
	if(INO_gi_CurrentPadId == -1) return;
	
	
	
	if(ps2INO_gax_Actuator[INO_gi_CurrentPadId].i_Nb == 2)
	{
		_i_level = lAbs(_i_level);
		ps2INO_gax_Actuator[INO_gi_CurrentPadId].asz_ActDirect[1] = (_i_level > 0xFF) ? 0xff : _i_level;
		
		
		//
		// => anti bug : le gros moteur ne semble plus repondre...
		//
		ps2INO_PadLowVibSet(_i_level);
		return;
		
		    
		scePadSetActDirect
		(
			INO_gi_CurrentPadId,
			Mi_PadDefault_Slot,
			ps2INO_gax_Actuator[INO_gi_CurrentPadId].asz_ActDirect
		);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ps2INO_b_Joystick_IsButtonDown(LONG _l_ButtonIndex)
{
	if(INO_gi_CurrentPadId == -1) return FALSE;
	return ps2INO_uc_GetButton(INO_gi_CurrentPadId, _l_ButtonIndex);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static unsigned char ps2INO_uc_GetButton(int _port, int _i_id)
{
	/*~~~~~~~~~~~~~~~~~*/
	int		array, shift;
	long64	ll;
	int		l;
	int		analog;
	/*~~~~~~~~~~~~~~~~~*/

	if(_port < 0) return 0;
	if(_port >= Mi_MaxPortNumber) return 0;
	if(ps2INO_sx_PortState[_port] != eState_TreatData) return 0;
	if(ps2INO_gax_Actuator[_port].i_Nb != 2) return 0;

	shift = 0;
	analog = 1;

	switch(_i_id)
	{
	case e_SEL:
	case e_Status:
	case e_LowerUnit:
	case e_UpperUnit:
	case e_L3:
	case e_R3:
	case e_START:
	case e_AnaStickRightX:
	case e_AnaStickRightY:
	case e_AnaStickLeftX:
	case e_AnaStickLeftY:
		array = 0;
		analog = 0;
		break;

	case e_RIGHT:
		_i_id = e_InfoR;

	case e_InfoR:
		array = 1;
		break;

	case e_LEFT:
		_i_id = e_InfoL;

	case e_InfoL:
		array = 1;
		shift = 8;
		break;

	case e_UP:
		_i_id = e_InfoU;

	case e_InfoU:
		array = 1;
		shift = 16;
		break;

	case e_DOWN:
		_i_id = e_InfoD;

	case e_InfoD:
		array = 1;
		shift = 24;
		break;

	case e_TRIANGLE:
	if(IO_bJAPANESE_MENU)
	{
		_i_id = e_InfoX;
		array = 1;
		shift = 48;
		break;
	}
	else
		_i_id = e_InfoT;

	case e_InfoT:
		array = 1;
		shift = 32;
		break;

	case e_CIRCLE:
	if(IO_bJAPANESE_MENU)
	{
		_i_id = e_InfoT;	
		array = 1;
		shift = 32;
		break;
	}
	else
		_i_id = e_InfoC;

	case e_InfoC:
		array = 1;
		shift = 40;
		break;

	case e_CROSS:
	if(IO_bJAPANESE_MENU)
	{
		_i_id = e_InfoC;
		array = 1;
		shift = 40;
		break;
	}
	else
		_i_id = e_InfoX;

	case e_InfoX:
		array = 1;
		shift = 48;
		break;

	case e_SQUARE:
		_i_id = e_InfoS;

	case e_InfoS:
		array = 1;
		shift = 54;
		break;

	case e_L1:
		_i_id = e_InfoL1;

	case e_InfoL1:
		array = 2;
		shift = 0;
		break;

	case e_R1:
		_i_id = e_InfoR1;

	case e_InfoR1:
		array = 2;
		shift = 8;
		break;

	case e_L2:
		_i_id = e_InfoL2;

	case e_InfoL2:
		array = 2;
		shift = 16;
		break;

	case e_R2:
		_i_id = e_InfoR2;

	case e_InfoR2:
		array = 2;
		shift = 24;
		break;

	default:
		return 0;
	}

	if(analog)
	{
		ll = ps2INO_stdun_CurrentPadData[_port].lArray[array] & ps2INO_sx_PadMapping[_i_id];
		ll >>= shift;
		l = (int) (0x00000000FFFFFFFF & ll);
		l <<= 1;
		if(l > 0xff) l = 0xff;
	}
	else
	{
		l = (ps2INO_stdun_CurrentPadData[_port].lArray[array] & ps2INO_sx_PadMapping[_i_id]) ? 0xFF : 0;
	}

	if((_i_id == e_InfoR2) && l) return 0xFF;
	if(l > 0xff) l = 0xff;
	return (unsigned char) l;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ps2INO_Pad_Move(int _port, ps2INO_tden_StickId _stick_id, MATH_tdst_Vector *_pst_Move)
{
	/*~~~~~~~~~~~~~~~~~*/
	LONG	 X, Y;
	/*~~~~~~~~~~~~~~~~~*/

	if(ps2INO_sx_AnalogCapacity[_port].Valid == FALSE)
	{
		MATH_InitVectorToZero(_pst_Move);
		return;
	}

	if(_stick_id == e_Right)
	{
		X = ((LONG) ps2INO_stdun_CurrentPadData[_port].DualShock.AnaStickRightX) ;
		Y = ((LONG) ps2INO_stdun_CurrentPadData[_port].DualShock.AnaStickRightY) ;
	}
	else if(_stick_id == e_Left)
	{
		X = ((LONG) ps2INO_stdun_CurrentPadData[_port].DualShock.AnaStickLeftX) ;
		Y = ((LONG) ps2INO_stdun_CurrentPadData[_port].DualShock.AnaStickLeftY) ;
	}
	else if(_stick_id == e_RightButtonInfo)
	{
		X = 0x7F - (ps2INO_stdun_CurrentPadData[_port].DualShock2.InfoX >> 1) + (ps2INO_stdun_CurrentPadData[_port].DualShock2.InfoT >> 1) ;
		Y = 0x7F - (ps2INO_stdun_CurrentPadData[_port].DualShock2.InfoS >> 1) + (ps2INO_stdun_CurrentPadData[_port].DualShock2.InfoC >> 1) ;
	}
	else if(_stick_id == e_LeftButtonInfo)
	{
		X = 0x7F - (ps2INO_stdun_CurrentPadData[_port].DualShock2.InfoD >> 1) + (ps2INO_stdun_CurrentPadData[_port].DualShock2.InfoU >> 1) ;
		Y = 0x7F - (ps2INO_stdun_CurrentPadData[_port].DualShock2.InfoL >> 1) + (ps2INO_stdun_CurrentPadData[_port].DualShock2.InfoR >> 1) ;
	}
	else
	{
		_pst_Move->x = 0.0f;
		_pst_Move->y = 0.0f;
		_pst_Move->z = 0.0f;
		return;
	}
	
	X *= 256;
	Y *= 256;

	/*
    if(X < ps2INO_l_Joystick_XRightStart && X > ps2INO_l_Joystick_XLeftStart)
		_pst_Move->x = 0;
	else
    */
	{
		_pst_Move->x = (float) X / (float) (ps2INO_l_Joystick_XRight - ps2INO_l_Joystick_XLeft);
		_pst_Move->x = (_pst_Move->x * 2.0f) - 1.0f;
	}

    /*
	if(Y < ps2INO_l_Joystick_YDownStart && Y > ps2INO_l_Joystick_YUpStart)
		_pst_Move->y = 0;
	else
    */
	{
		_pst_Move->y = (float) Y / (float) (ps2INO_l_Joystick_YDown - ps2INO_l_Joystick_YUp);
		_pst_Move->y = (_pst_Move->y * 2.0f) - 1.0f;
		_pst_Move->y = -_pst_Move->y;
	}

	_pst_Move->z = 0.0f;

	_pst_Move->z = 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ps2INO_Joystick_Move(MATH_tdst_Vector *_pst_Move, int _i_Num)
{
	/*~~~~~~*/
	int JoyId;
	/*~~~~~~*/

	if(INO_gi_CurrentPadId == -1)
	{
		L_memset(_pst_Move, 0, sizeof(MATH_tdst_Vector));
		return;
	}

	switch(_i_Num)
	{
	case 0:		JoyId = (INO_l_JoystickMask & INO_JoyMask_Joy0) ? e_Left : e_LastStickId; break;
	case 1:		JoyId = (INO_l_JoystickMask & INO_JoyMask_Joy1) ? e_Right : e_LastStickId; break;
	case 2:		JoyId = (INO_l_JoystickMask & INO_JoyMask_Joy2) ? e_LeftButtonInfo : e_LastStickId; break;
	case 3:		JoyId = (INO_l_JoystickMask & INO_JoyMask_Joy3) ? e_RightButtonInfo : e_LastStickId; break;
	default:	JoyId = e_LastStickId; break;
	}

	ps2INO_Pad_Move(INO_gi_CurrentPadId, JoyId, _pst_Move);
}

/*$4
 ***********************************************************************************************************************
    GAMECUBE
 ***********************************************************************************************************************
 */

#elif defined(_GAMECUBE)

/*$2- types ----------------------------------------------------------------------------------------------------------*/

typedef enum	gcINO_tden_PortState_
{
	e_WaitingNewPad = 0,
	e_DoingReset,			
	e_DoingRecalibrate,
	e_FirstRead,
	e_StateIsValid
} gcINO_tden_PortState;

/*$2- variables ------------------------------------------------------------------------------------------------------*/

static PADStatus			gcINO_gax_CurrentPadData[PAD_MAX_CONTROLLERS];
static PADStatus			gcINO_gax_PreviousPadData[PAD_MAX_CONTROLLERS];
static gcINO_tden_PortState gcINO_gax_PortState[PAD_MAX_CONTROLLERS];
static BOOL  				gcINO_gax_StableState[PAD_MAX_CONTROLLERS];
static u16					gcINO_au16_PadMapping[e_LastButtonId];
//static u8 					gcINO_gaiZTriggerToggle[PAD_MAX_CONTROLLERS];
//static u8 					gcINO_gabPreviousZTrigger[PAD_MAX_CONTROLLERS];



#define ActiveL2	0x00000001
int							gcINO_gi_Special;
int 						gcINO_gi_Recalibrating=0;


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcINO_SetMappingJack()
{
	gcINO_au16_PadMapping[e_START] = PAD_BUTTON_START;

	gcINO_au16_PadMapping[e_L1] = PAD_TRIGGER_L; 
	gcINO_au16_PadMapping[e_R1] = PAD_BUTTON_X; 

	gcINO_au16_PadMapping[e_L2] = PAD_TRIGGER_L;
	gcINO_au16_PadMapping[e_R2] = PAD_TRIGGER_R;
	
	gcINO_au16_PadMapping[e_R3] = PAD_TRIGGER_Z;
	
	gcINO_au16_PadMapping[e_TRIANGLE] = PAD_BUTTON_RIGHT | PAD_BUTTON_LEFT; 
	gcINO_au16_PadMapping[e_CIRCLE] = PAD_BUTTON_Y; 
	gcINO_au16_PadMapping[e_CROSS] = PAD_BUTTON_A;
	gcINO_au16_PadMapping[e_SQUARE] = PAD_BUTTON_B;

	gcINO_au16_PadMapping[e_UP] = PAD_BUTTON_UP;
	gcINO_au16_PadMapping[e_DOWN] = PAD_BUTTON_DOWN;
	gcINO_au16_PadMapping[e_LEFT] = PAD_BUTTON_LEFT;
	gcINO_au16_PadMapping[e_RIGHT] = PAD_BUTTON_RIGHT;
}

void gcINO_SetMappingKong()
{
	gcINO_au16_PadMapping[e_START] = PAD_BUTTON_START;

	gcINO_au16_PadMapping[e_L2] = PAD_TRIGGER_L;
	gcINO_au16_PadMapping[e_R2] = PAD_TRIGGER_R;

	gcINO_au16_PadMapping[e_L1] = PAD_TRIGGER_L;
	gcINO_au16_PadMapping[e_R1] = PAD_TRIGGER_R; 

	gcINO_au16_PadMapping[e_R3] = 0x4000;
	
	gcINO_au16_PadMapping[e_TRIANGLE] = PAD_BUTTON_Y; 
	gcINO_au16_PadMapping[e_CIRCLE] = PAD_BUTTON_X; 
	gcINO_au16_PadMapping[e_CROSS] = PAD_BUTTON_A;
	gcINO_au16_PadMapping[e_SQUARE] = PAD_BUTTON_B;
	
	gcINO_au16_PadMapping[e_UP] = PAD_BUTTON_UP;
	gcINO_au16_PadMapping[e_DOWN] = PAD_BUTTON_DOWN;
	gcINO_au16_PadMapping[e_LEFT] = PAD_BUTTON_LEFT;
	gcINO_au16_PadMapping[e_RIGHT] = PAD_BUTTON_RIGHT;
}

void gcINO_SetMappingMenu()
{
	
	gcINO_au16_PadMapping[e_START] = PAD_BUTTON_START;
	
	gcINO_au16_PadMapping[e_L1] = PAD_TRIGGER_L;
	gcINO_au16_PadMapping[e_R1] = PAD_TRIGGER_R; 
	
	gcINO_au16_PadMapping[e_L2] = PAD_TRIGGER_L;
	gcINO_au16_PadMapping[e_R2] = PAD_TRIGGER_R;

	gcINO_au16_PadMapping[e_R3] = 0x4000;

	gcINO_au16_PadMapping[e_TRIANGLE] = PAD_BUTTON_B;
	gcINO_au16_PadMapping[e_CIRCLE] = PAD_BUTTON_X;
	gcINO_au16_PadMapping[e_CROSS] = PAD_BUTTON_A;
	gcINO_au16_PadMapping[e_SQUARE] = PAD_BUTTON_Y;
	
	gcINO_au16_PadMapping[e_UP] = PAD_BUTTON_UP;
	gcINO_au16_PadMapping[e_DOWN] = PAD_BUTTON_DOWN;
	gcINO_au16_PadMapping[e_LEFT] = PAD_BUTTON_LEFT;
	gcINO_au16_PadMapping[e_RIGHT] = PAD_BUTTON_RIGHT;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcINO_Joystick_Init(HWND none)
{
#pragma unused(none)
	gcINO_gi_Special = 0;
	INO_gi_CurrentPadId = -1;

	L_memset(gcINO_gax_PreviousPadData, 0, PAD_MAX_CONTROLLERS * sizeof(PADStatus));
	L_memset(gcINO_gax_CurrentPadData, 0, PAD_MAX_CONTROLLERS * sizeof(PADStatus));
	L_memset(gcINO_gax_PortState, 0, PAD_MAX_CONTROLLERS * sizeof(gcINO_tden_PortState));
//	L_memset(gcINO_gaiZTriggerToggle, 0, PAD_MAX_CONTROLLERS * sizeof(u8));
//	L_memset(gcINO_gabPreviousZTrigger, 0, PAD_MAX_CONTROLLERS * sizeof(u8));

	gcINO_au16_PadMapping[e_Status] = 0x4000;
	gcINO_au16_PadMapping[e_LowerUnit] = 0x4000;
	gcINO_au16_PadMapping[e_UpperUnit] = 0x4000;
	gcINO_au16_PadMapping[e_SEL] = 0x4000;
	gcINO_au16_PadMapping[e_L3] = 0x4000;
	gcINO_au16_PadMapping[e_R3] = 0x4000;
	gcINO_au16_PadMapping[e_UP] = 0x4000; // old : PAD_BUTTON_UP;
	gcINO_au16_PadMapping[e_RIGHT] = 0x4000; // PAD_BUTTON_RIGHT;
	gcINO_au16_PadMapping[e_DOWN] = 0x4000; // PAD_BUTTON_DOWN;
	gcINO_au16_PadMapping[e_LEFT] = 0x4000; // PAD_BUTTON_LEFT;

	gcINO_au16_PadMapping[e_AnaStickRightX] = 0;
	gcINO_au16_PadMapping[e_AnaStickRightY] = 0;
	gcINO_au16_PadMapping[e_AnaStickLeftX] = 0;
	gcINO_au16_PadMapping[e_AnaStickLeftY] = 0;

	gcINO_au16_PadMapping[e_InfoR] = 0;
	gcINO_au16_PadMapping[e_InfoL] = 0;
	gcINO_au16_PadMapping[e_InfoU] = 0;
	gcINO_au16_PadMapping[e_InfoD] = 0;
	gcINO_au16_PadMapping[e_InfoT] = 0;
	gcINO_au16_PadMapping[e_InfoC] = 0;
	gcINO_au16_PadMapping[e_InfoX] = 0;
	gcINO_au16_PadMapping[e_InfoS] = 0;

	gcINO_au16_PadMapping[e_InfoL1] = 0;
	gcINO_au16_PadMapping[e_InfoR1] = 0;
	gcINO_au16_PadMapping[e_InfoL2] = 0;
	gcINO_au16_PadMapping[e_InfoR2] = 0;
	
	gcINO_SetMappingMenu();

	INO_l_Joystick_Enable = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcINO_Joystick_Close(void)
{
	INO_l_Joystick_Enable = 0;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcINO_SetControlMode(int mode)
{
	switch(mode)
	{
	case IO_MODE_JACK:
		gcINO_SetMappingJack();
		break;

	case IO_MODE_KONG:
		gcINO_SetMappingKong();
		break;

	case IO_MODE_MENU:
		gcINO_SetMappingMenu();
		break;
	}
}

BOOL gcINO_gb_RecalibDetected = FALSE;

PADStatus gcINO_gstPadStatus[PAD_MAX_CONTROLLERS];

u16 gcINO_usGetButtonState() 
{
	if (INO_gi_CurrentPadId == -1)
		return 0;
	else 
		return gcINO_gstPadStatus[INO_gi_CurrentPadId].button;
}

BOOL gcINO_bGetButton(int _iButton) // Utiliser PAD_BUTTON_LEFT, PAD_BUTTON_A, PAD_TRIGGER_Z, etc ...
{
	if (INO_gi_CurrentPadId == -1)
		return FALSE;
	else 
		return gcINO_gstPadStatus[INO_gi_CurrentPadId].button & _iButton;
}

static void gcINO_Joystick_UpdateStatus()
{
	int i;
	BOOL bAllPadsAreInStableState = TRUE;

	PADRead(gcINO_gstPadStatus);
	PADClamp(gcINO_gstPadStatus);

	if (INO_gi_CurrentPadId == -1)
	{
		for (i=0; i<PAD_MAX_CONTROLLERS; i++)
		{
			if (!gcINO_gax_StableState[i])
			{
				bAllPadsAreInStableState = FALSE;
				break;
			}
		} 
	}

	for(i = 0; i < PAD_MAX_CONTROLLERS; i++)
	{
		L_memcpy(&gcINO_gax_PreviousPadData[i], &gcINO_gax_CurrentPadData[i], sizeof(PADStatus));

		switch(gcINO_gax_PortState[i])
		{
		case e_WaitingNewPad:
			if(!gcINO_gb_RecalibDetected && (INO_gi_CurrentPadId == i))
			{
				INO_gi_CurrentPadId = -1;
			} 
			PADReset(PAD_CHAN0_BIT >> i);
			gcINO_gax_PortState[i] = e_DoingReset;
			
			break;
			
		case e_DoingReset:
			if(gcINO_gstPadStatus[i].err == PAD_ERR_NO_CONTROLLER)
			{
				gcINO_gax_PortState[i] = e_WaitingNewPad;
				gcINO_gax_StableState[i] = TRUE;
			}
			else
			{
				if (PADRecalibrate(PAD_CHAN0_BIT >> i))
				{
					gcINO_gax_PortState[i] = e_DoingRecalibrate;
					gcINO_gax_StableState[i] = FALSE;
				}
				else
				{
					gcINO_gax_PortState[i] = e_WaitingNewPad;
					gcINO_gax_StableState[i] = FALSE;
				}
			}
			break;

		case e_DoingRecalibrate:
			switch(gcINO_gstPadStatus[i].err)
			{
			case PAD_ERR_NOT_READY:
			case PAD_ERR_TRANSFER:
			case PAD_ERR_NONE:
				gcINO_gax_PortState[i] = e_FirstRead;
				break;

			case PAD_ERR_NO_CONTROLLER:
			default:
				gcINO_gax_PortState[i] = e_WaitingNewPad;
				break;
			}
			break;

		case e_FirstRead:
			switch(gcINO_gstPadStatus[i].err)
			{
			case PAD_ERR_NOT_READY:
			case PAD_ERR_TRANSFER:
				break;

			case PAD_ERR_NONE:
				{
					gcINO_gax_PortState[i] = e_StateIsValid;
					L_memcpy(&gcINO_gax_CurrentPadData[i], &gcINO_gstPadStatus[i], sizeof(PADStatus));
					L_memset(&gcINO_gax_CurrentPadData[i], 0, sizeof(PADStatus));
				}
				break;

			case PAD_ERR_NO_CONTROLLER:
			default:
				gcINO_gax_PortState[i] = e_WaitingNewPad;
				break;
			}
			break;

		case e_StateIsValid:
			switch(gcINO_gstPadStatus[i].err)
			{
			case PAD_ERR_NOT_READY:
			case PAD_ERR_TRANSFER:
				break;

			case PAD_ERR_NONE:
				gcINO_gax_StableState[i] = TRUE;
				// Set this pad as current pad if other pads are in a stable state.
				if(bAllPadsAreInStableState && ((INO_gi_CurrentPadId == -1) || (INO_gi_CurrentPadId > i)) ) 
				{
					INO_gi_CurrentPadId = i;
				}
					
				L_memcpy(&gcINO_gax_CurrentPadData[i], &gcINO_gstPadStatus[i], sizeof(PADStatus));
				
				break;

			case PAD_ERR_NO_CONTROLLER:	
			default:
				gcINO_gax_StableState[i] = FALSE;
				gcINO_gax_PortState[i] = e_WaitingNewPad;
				L_memset(&gcINO_gax_CurrentPadData[i], 0, sizeof(PADStatus));
				break;
			}
			break;
			
		default:
			gcINO_gax_StableState[i] = FALSE;
			gcINO_gax_PortState[i] = e_WaitingNewPad;
			break;
		}

/*#ifndef _FINAL
		if(gcINO_gax_PreviousPadData[i].button & gcINO_au16_PadMapping[e_L2])
			gcINO_gax_CurrentPadData[i].button |= gcINO_au16_PadMapping[e_L2];
#endif*/
	}

}

BOOL gcINO_bIsJoystickTouched()
{
	BOOL bResult;
	gcINO_Joystick_UpdateStatus();
	bResult = (INO_gi_CurrentPadId != -1) && gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button;

	return bResult;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcINO_Joystick_Update(INO_tdst_GenericPad *pCurr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i, j;
	extern int		GXI_gb_MenuMode;
	extern int		GXI_gi_EnginePadState;
	extern BOOL	    GC_b_ResetShortCutPresent;
	static int		si_BootUp = 10;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gcINO_Joystick_UpdateStatus();

	if(INO_gi_CurrentPadId != -1)
	{

		/*$1- get cursor postion for the debug menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef _FINAL_
		GXI_gi_EnginePadState = 0;
		if(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_RIGHT) GXI_gi_EnginePadState |= 1;
		if(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_LEFT) GXI_gi_EnginePadState |= 2;
		if(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_DOWN) GXI_gi_EnginePadState |= 4;
		if(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_UP) GXI_gi_EnginePadState |= 8;
		if(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_TRIGGER_L) GXI_gi_EnginePadState |= 16;
		if(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_TRIGGER_R) GXI_gi_EnginePadState |= 32;

		if(GXI_gb_MenuMode)
		{
			gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button &= ~(PAD_BUTTON_RIGHT | PAD_BUTTON_LEFT | PAD_BUTTON_UP | PAD_BUTTON_DOWN | PAD_TRIGGER_R | PAD_TRIGGER_L);
		}
#endif
		/*$1- simulate L2 for cheating mode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*#ifndef _FINAL_
		if(gcINO_gi_Special)
		{
			if(gcINO_gi_Special & 1)
				gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button |= (gcINO_au16_PadMapping[e_L2]);
		}

		if(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & gcINO_au16_PadMapping[e_L2])
		{
			if((gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & (PAD_TRIGGER_L)) == (0))
			{
				gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button &= ~gcINO_au16_PadMapping[e_L2];
				gcINO_au16_PadMapping[e_SEL] = 0;
			}
		}
		else
		{
			if
			(
				(
					gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button &
						(PAD_TRIGGER_L | PAD_TRIGGER_R | PAD_TRIGGER_Z)
				) == (PAD_TRIGGER_L | PAD_TRIGGER_R | PAD_TRIGGER_Z)
			)
			{
				gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button |= gcINO_au16_PadMapping[e_L2];
				gcINO_au16_PadMapping[e_SEL] = 0;
			}
		}
#endif*/

		/*$1- now transform internal format to the generic one ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(j = 0; j < INO_Cte_PabNumber; j++)
		{
			for(i = 0; i < eBtn_GenericButtonNb; i++)
			{
				pCurr[j].ac_Button[i] = gcINO_uc_GetButton(j, i);
			}
		}
	}
	

	// Reset avec la manette (validée après 0.5 secondes).
	if ((INO_gi_CurrentPadId != -1) && 
	    (gcINO_gax_PortState[INO_gi_CurrentPadId] == e_StateIsValid) &&
		(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_B) && 
		(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_X) && 
		(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_START))
		GC_b_ResetShortCutPresent = TRUE;
	else
		GC_b_ResetShortCutPresent = FALSE;


	if((INO_gi_CurrentPadId != -1) && !GC_b_ResetShortCutPresent)
	{
		si_BootUp = 0;
	}
	else if(si_BootUp && GC_b_ResetShortCutPresent)
	{
		GC_b_ResetShortCutPresent = FALSE;
	}		
	

	// Calibration de la manette (validée après 3 secondes).
	// Inutile car déclenché  en hardware
	/*{
	static u64 RecalibrationStartTime = 0;
	gcINO_gb_RecalibDetected = FALSE;
	if((INO_gi_CurrentPadId != -1) && (gcINO_gax_PortState[INO_gi_CurrentPadId] == e_StateIsValid))
	{
		if ((gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_Y) && 
			(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_X) && 
			(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_START))
		{
			if (!RecalibrationStartTime)
				RecalibrationStartTime = OSTicksToMicroseconds(OSGetTime());
			else if ((OSTicksToMicroseconds(OSGetTime()) - RecalibrationStartTime) > 3000000)
				gcINO_gb_RecalibDetected = TRUE;			
		}
		else
			RecalibrationStartTime = 0;
	}
	else
		RecalibrationStartTime = 0;
	}*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static unsigned char gcINO_uc_GetButton(int pad, int _l_ButtonIndex)
{
	u16 uGCButton = gcINO_au16_PadMapping[_l_ButtonIndex];
	
	if(pad < 0) return 0;
	if(pad >= PAD_MAX_CONTROLLERS) return 0;
	if(gcINO_gax_PortState[pad] != e_StateIsValid) return 0;

/*	if (uGCButton == PAD_TRIGGER_Z)
	{			
		int bZTrigger = ((gcINO_gax_CurrentPadData[pad].button & uGCButton) == uGCButton);
		
		// If Z trigger was not pressed and is now pressed, change toggle state.
		if (bZTrigger && !gcINO_gabPreviousZTrigger[pad])
			gcINO_gaiZTriggerToggle[pad] = !gcINO_gaiZTriggerToggle[pad];
		
		gcINO_gabPreviousZTrigger[pad] = bZTrigger;
		
		// Return toggle state.
		return (gcINO_gaiZTriggerToggle[pad] ? 0xFF : 0);
	}*/
	else if (uGCButton == PAD_TRIGGER_L)
		return (unsigned char) ((float) gcINO_gax_CurrentPadData[pad].triggerLeft * 255.0f / 150.0f);
	else if (uGCButton == PAD_TRIGGER_R)
		return (unsigned char) ((float) gcINO_gax_CurrentPadData[pad].triggerRight * 255.0f / 150.0f);
	else 
		return ((gcINO_gax_CurrentPadData[pad].button & uGCButton) ? 0xFF : 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcINO_Joystick_Move(MATH_tdst_Vector *_pst_Move, int _i_Num)
{
	/*~~~~~*/
	s8	X, Y;
	/*~~~~~*/

	if(INO_gi_CurrentPadId == -1) return;

	switch(_i_Num)
	{
	case 0:
	case 2:
		X = gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].stickX;
		Y = gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].stickY;
		_pst_Move->x = X / 40.0f;
		_pst_Move->y = Y / 40.0f;
		break;

	case 1:
	case 3:
	default:
		// Yellow right stick
		X = gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].substickX;
		Y = gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].substickY;
		_pst_Move->x = X / 30.f; //59.0f;
		_pst_Move->y = Y / 30.f; //59.0f;
		break;
	}

	_pst_Move->z = 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcINO_PadLowVibSet(BOOL _b_On)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	u32 array[PAD_MAX_CONTROLLERS];
	u32 cmd;
	int i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_b_On)
	{
		cmd = PAD_MOTOR_RUMBLE;
		if(INO_gf_PifDuration < 0.128f) INO_gf_PifDuration = 0.128f;
	}
	else
		cmd = PAD_MOTOR_STOP;

	for(i = 0; i < PAD_MAX_CONTROLLERS; i++)
	{
		if(i==INO_gi_CurrentPadId)
			array[i] = cmd;
		else
			array[i] = PAD_MOTOR_STOP;
	}

	PADControlAllMotors(array);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcINO_PadHightVibSet(int _i_level)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	u32 array[PAD_MAX_CONTROLLERS];
	u32 cmd;
	int i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_level)
	{
		cmd = PAD_MOTOR_RUMBLE;
		if(INO_gf_PafDuration < 0.224f) INO_gf_PafDuration = 0.224f;
	}
	else
		cmd = PAD_MOTOR_STOP_HARD;

	for(i = 0; i < PAD_MAX_CONTROLLERS; i++)
	{
		if(i==INO_gi_CurrentPadId)
			array[i] = cmd;
		else
			array[i] = PAD_MOTOR_STOP;
	}

	PADControlAllMotors(array);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL gcINO_b_Joystick_IsValid(void)
{
	if(INO_gi_CurrentPadId == -1) return FALSE;
	if(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].err == PAD_ERR_NO_CONTROLLER) return FALSE;
	if(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].err == PAD_ERR_NOT_READY) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int gcINO_i_SetCurrentPad(int id)
{
	switch(id)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		if(gcINO_gax_PortState[id] == e_StateIsValid)
		{
			INO_gi_CurrentPadId = id;
			break;
		}
		else
			return -1;

	default:
		return -1;
	}

	return 0;
}

#if defined (_GC2REVO) || defined(_RVL)

KPADStatus			revoINO_gax_CurrentPadData[PAD_MAX_CONTROLLERS][KPAD_MAX_READ_BUFS];
KPADStatus			revoINO_gax_PreviousRevoPadData[PAD_MAX_CONTROLLERS][KPAD_MAX_READ_BUFS];
static int iCurrentControlMode;

static float fMainPadVibrationDelay = 0.0f;
static float fFSPadVibrationDelay = 0.0f;
static bool bMainPadIsVibrating	= 0;
static bool bFSPadIsVibrating	= 0;

/*$4
 ***********************************************************************************************************************
    REVOLUTION
 ***********************************************************************************************************************
 */


/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
KPADStatus revoINO_gstPadStatus[PAD_MAX_CONTROLLERS][KPAD_MAX_READ_BUFS];

static int NumReads;

static void revoINO_Joystick_UpdateStatus()
{
	int i;
	BOOL bAllPadsAreInStableState = TRUE;

	NumReads = KPADRead( WPAD_CHAN0, &revoINO_gstPadStatus[0][0], KPAD_MAX_READ_BUFS ) ;

	if (INO_gi_CurrentPadId == -1)
	{
		for (i=0; i<PAD_MAX_CONTROLLERS; i++)
		{
			if (!gcINO_gax_StableState[i])
			{
				bAllPadsAreInStableState = FALSE;
				break;
			}
		} 
	}

	for(i = 0; i < PAD_MAX_CONTROLLERS; i++)
	{
		L_memcpy(&revoINO_gax_PreviousRevoPadData[i], &revoINO_gax_CurrentPadData[i], KPAD_MAX_READ_BUFS * sizeof(KPADStatus));

		switch(gcINO_gax_PortState[i])
		{
		case e_WaitingNewPad:
			if(!gcINO_gb_RecalibDetected && (INO_gi_CurrentPadId == i))
			{
				INO_gi_CurrentPadId = -1;
			} 
			
			KPADReset(); 
			gcINO_gax_PortState[i] = e_DoingReset;
			
			break;
			
		case e_DoingReset:
			if(revoINO_gstPadStatus[i][0].wpad_err == PAD_ERR_NO_CONTROLLER)
			{
				gcINO_gax_PortState[i] = e_WaitingNewPad;
				gcINO_gax_StableState[i] = TRUE;
			}
			else
			{
				if (WPADRecalibrate(WPAD_CHAN0 >> i))
				{
					gcINO_gax_PortState[i] = e_DoingRecalibrate;
					gcINO_gax_StableState[i] = FALSE;
				}
				else
				{
					gcINO_gax_PortState[i] = e_WaitingNewPad;
					gcINO_gax_StableState[i] = FALSE;
				}
			}
			break;

		case e_DoingRecalibrate:
			switch(revoINO_gstPadStatus[i][0].wpad_err)
			{
			case PAD_ERR_NOT_READY:
			case PAD_ERR_TRANSFER:
			case PAD_ERR_NONE:
				gcINO_gax_PortState[i] = e_FirstRead;
				break;

			case PAD_ERR_NO_CONTROLLER:
			default:
				gcINO_gax_PortState[i] = e_WaitingNewPad;
				break;
			}
			break;

		case e_FirstRead:
			switch(revoINO_gstPadStatus[i][0].wpad_err)
			{
			case PAD_ERR_NOT_READY:
			case PAD_ERR_TRANSFER:
				break;

			case PAD_ERR_NONE:
				{
					gcINO_gax_PortState[i] = e_StateIsValid;
					L_memcpy(&revoINO_gax_CurrentPadData[i], &revoINO_gstPadStatus[i], KPAD_MAX_READ_BUFS * sizeof(KPADStatus));
					//L_memset(&revoINO_gax_CurrentPadData[i], 0, KPAD_MAX_READ_BUFS * sizeof(KPADStatus));
				}
				break;

			case PAD_ERR_NO_CONTROLLER:
			default:
				gcINO_gax_PortState[i] = e_WaitingNewPad;
				break;
			}
			break;

		case e_StateIsValid:
			switch(revoINO_gstPadStatus[i][0].wpad_err)
			{
			case PAD_ERR_NOT_READY:
			case PAD_ERR_TRANSFER:
				break;

			case PAD_ERR_NONE:
				gcINO_gax_StableState[i] = TRUE;
				// Set this pad as current pad if other pads are in a stable state.
				if(bAllPadsAreInStableState && ((INO_gi_CurrentPadId == -1) || (INO_gi_CurrentPadId > i)) ) 
				{
					INO_gi_CurrentPadId = i;
				}
					
				L_memcpy(&revoINO_gax_CurrentPadData[i], &revoINO_gstPadStatus[i], KPAD_MAX_READ_BUFS * sizeof(KPADStatus));
								
				break;

			case PAD_ERR_NO_CONTROLLER:	
			default:
				gcINO_gax_StableState[i] = FALSE;
				gcINO_gax_PortState[i] = e_WaitingNewPad;
				//L_memset(&revoINO_gax_CurrentPadData[i], 0, KPAD_MAX_READ_BUFS * sizeof(KPADStatus));
				break;
			}
			break;
			
		default:
			gcINO_gax_StableState[i] = FALSE;
			gcINO_gax_PortState[i] = e_WaitingNewPad;
			break;
		}

/*#ifndef _FINAL
		if(gcINO_gax_PreviousPadData[i].button & gcINO_au16_PadMapping[e_L2])
			gcINO_gax_CurrentPadData[i].button |= gcINO_au16_PadMapping[e_L2];
#endif*/
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void revoINO_Joystick_Update(INO_tdst_GenericPad *pCurr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i, j;
	extern int		GXI_gb_MenuMode;
	extern int		GXI_gi_EnginePadState;
	extern BOOL	    GC_b_ResetShortCutPresent;
	static int		si_BootUp = 10;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	revoINO_Joystick_UpdateStatus();

	if(INO_gi_CurrentPadId != -1)
	{

		/*$1- get cursor postion for the debug menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef _FINAL_
		GXI_gi_EnginePadState = 0;
		if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold & KPAD_BUTTON_RIGHT) GXI_gi_EnginePadState |= 1;
		if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold & KPAD_BUTTON_LEFT) GXI_gi_EnginePadState |= 2;
		if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold & KPAD_BUTTON_DOWN) GXI_gi_EnginePadState |= 4;
		if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold & KPAD_BUTTON_UP) GXI_gi_EnginePadState |= 8;
		if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold & KPAD_BUTTON_SMALL_A) GXI_gi_EnginePadState |= 16;
		if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold & KPAD_BUTTON_SMALL_B) GXI_gi_EnginePadState |= 32;

		if(GXI_gb_MenuMode)
		{
			revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold &= ~(KPAD_BUTTON_RIGHT | KPAD_BUTTON_LEFT | KPAD_BUTTON_UP | KPAD_BUTTON_DOWN | KPAD_BUTTON_SMALL_A | KPAD_BUTTON_SMALL_B);
		}
#endif
		/*$1- simulate L2 for cheating mode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*#ifndef _FINAL_
		if(gcINO_gi_Special)
		{
			if(gcINO_gi_Special & 1)
				gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button |= (gcINO_au16_PadMapping[e_L2]);
		}

		if(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & gcINO_au16_PadMapping[e_L2])
		{
			if((gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & (PAD_TRIGGER_L)) == (0))
			{
				gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button &= ~gcINO_au16_PadMapping[e_L2];
				gcINO_au16_PadMapping[e_SEL] = 0;
			}
		}
		else
		{
			if
			(
				(
					gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button &
						(PAD_TRIGGER_L | PAD_TRIGGER_R | PAD_TRIGGER_Z)
				) == (PAD_TRIGGER_L | PAD_TRIGGER_R | PAD_TRIGGER_Z)
			)
			{
				gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button |= gcINO_au16_PadMapping[e_L2];
				gcINO_au16_PadMapping[e_SEL] = 0;
			}
		}
#endif*/

		/*$1- now transform internal format to the generic one ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(j = 0; j < INO_Cte_PabNumber; j++)
		{
			for(i = 0; i < eBtn_GenericButtonNb; i++)
			{
				pCurr[j].ac_Button[i] = revoINO_uc_GetButton(j, i);
			}
		}
	}
	

	// Reset avec la manette (validée après 0.5 secondes).
	if ((INO_gi_CurrentPadId != -1) && 
	    (gcINO_gax_PortState[INO_gi_CurrentPadId] == e_StateIsValid) &&
		(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_B) && 
		(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_X) && 
		(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_START))
		GC_b_ResetShortCutPresent = TRUE;
	else
		GC_b_ResetShortCutPresent = FALSE;


	if((INO_gi_CurrentPadId != -1) && !GC_b_ResetShortCutPresent)
	{
		si_BootUp = 0;
	}
	else if(si_BootUp && GC_b_ResetShortCutPresent)
	{
		GC_b_ResetShortCutPresent = FALSE;
	}		
	

	// Calibration de la manette (validée après 3 secondes).
	// Inutile car déclenché  en hardware
	/*{
	static u64 RecalibrationStartTime = 0;
	gcINO_gb_RecalibDetected = FALSE;
	if((INO_gi_CurrentPadId != -1) && (gcINO_gax_PortState[INO_gi_CurrentPadId] == e_StateIsValid))
	{
		if ((gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_Y) && 
			(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_X) && 
			(gcINO_gax_CurrentPadData[INO_gi_CurrentPadId].button & PAD_BUTTON_START))
		{
			if (!RecalibrationStartTime)
				RecalibrationStartTime = OSTicksToMicroseconds(OSGetTime());
			else if ((OSTicksToMicroseconds(OSGetTime()) - RecalibrationStartTime) > 3000000)
				gcINO_gb_RecalibDetected = TRUE;			
		}
		else
			RecalibrationStartTime = 0;
	}
	else
		RecalibrationStartTime = 0;
	}*/
	
	// main pad vibration gestion
	if (fMainPadVibrationDelay > 0.0f)
	{
		if (!bMainPadIsVibrating)
		{
			bMainPadIsVibrating = 1;
			WPADStartMotor(WPAD_CHAN0);
		}
		fMainPadVibrationDelay -= TIM_gf_dt;
	}
	else
	{
		if (bMainPadIsVibrating)
		{
			bMainPadIsVibrating = 0;
			WPADStopMotor(WPAD_CHAN0);
		}		
	}

	// free style pad vibration gestion
	if (fFSPadVibrationDelay > 0.0f)
	{
		if (!bFSPadIsVibrating)
		{
			bFSPadIsVibrating = 1;
			WPADStartMotor(WPAD_CHAN0);
		}
		fMainPadVibrationDelay -= TIM_gf_dt;
	}
	else
	{
		if (bFSPadIsVibrating)
		{
			bFSPadIsVibrating = 0;
			WPADStopMotor(WPAD_CHAN0);
		}		
	}
	
	// mvt recognition
	revo_CheckMvtRecognition();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static unsigned char revoINO_uc_GetButton(int pad, int _l_ButtonIndex)
{
	u16 uGCButton = gcINO_au16_PadMapping[_l_ButtonIndex];
	
	if(pad < 0) return 0;
	if(pad >= PAD_MAX_CONTROLLERS) return 0;
	if(gcINO_gax_PortState[pad] != e_StateIsValid) return 0;

/*	if (uGCButton == PAD_TRIGGER_Z)
	{			
		int bZTrigger = ((gcINO_gax_CurrentPadData[pad].button & uGCButton) == uGCButton);
		
		// If Z trigger was not pressed and is now pressed, change toggle state.
		if (bZTrigger && !gcINO_gabPreviousZTrigger[pad])
			gcINO_gaiZTriggerToggle[pad] = !gcINO_gaiZTriggerToggle[pad];
		
		gcINO_gabPreviousZTrigger[pad] = bZTrigger;
		
		// Return toggle state.
		return (gcINO_gaiZTriggerToggle[pad] ? 0xFF : 0);
	}*/
	//else if (uGCButton == PAD_TRIGGER_L)
	//	return (unsigned char) ((float) gcINO_gax_CurrentPadData[pad].triggerLeft * 255.0f / 150.0f);
	//else if (uGCButton == PAD_TRIGGER_R)
	//	return (unsigned char) ((float) gcINO_gax_CurrentPadData[pad].triggerRight * 255.0f / 150.0f);
	else 
		// Default behavior
		return ((revoINO_gax_CurrentPadData[pad][0].hold & uGCButton) ? 0xFF : 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#define blend(o,n,t) ((o)*(1.f-(t))+((n)*(t)))
static void revoINO_Joystick_Move(MATH_tdst_Vector *_pst_Move, int _i_Num)
{
	static OSTick validGoodDPD = 0;
	/*static OSTick lastTick = 0;	
	static float lastX = 0.f;
	static float lastY = 0.f;*/
	OSTick curTick;
	float f;
	s32 s;
	if(INO_gi_CurrentPadId == -1) return;

	switch(_i_Num)
	{
	case 0:
	case 2:
		switch(iCurrentControlMode)
		{
		case IO_MODE_JACK:
			_pst_Move->x = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].ex_status.fs.stick.x * 3.0f;
			_pst_Move->y = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].ex_status.fs.stick.y * 3.0f;
			break;

		case IO_MODE_KONG:
			_pst_Move->x = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].acc.x * -1.0f;
			_pst_Move->y = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].acc.z * 2.0f;
			break;

		case IO_MODE_MENU:
			if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].dev_type!=WPAD_DEV_CORE)
			{
				_pst_Move->x = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].ex_status.fs.stick.x * 3.0f;
				_pst_Move->y = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].ex_status.fs.stick.y * 3.0f;
			}
			else
			{
				_pst_Move->x = (revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold&WPAD_BUTTON_RIGHT)?1.f:0.f;
				_pst_Move->x-= (revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold&WPAD_BUTTON_LEFT)?1.f:0.f;
				_pst_Move->x *= 3.f;
				_pst_Move->y = (revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold&WPAD_BUTTON_UP)?1.f:0.f;
				_pst_Move->y-= (revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].hold&WPAD_BUTTON_DOWN)?1.f:0.f;
				_pst_Move->y *= 3.f;
			}
			break;
		}		
		break;

	case 1:
	case 3:
	default:
		{
			if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].dev_type!=WPAD_DEV_CORE)
			{
				_pst_Move->x = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].ex_status.fs.acc.x *-1.f;				
				_pst_Move->y = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].ex_status.fs.acc.z * -1.0f;			
			}
			else
			{		
				curTick = OSGetTick();
				_pst_Move->x = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].pos.x;
				_pst_Move->y =-revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].pos.y;

				if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].dpd_valid_fg>0)
					validGoodDPD = curTick;
				else
				{					
					s = OSTicksToMilliseconds(OSDiffTick(curTick,validGoodDPD));
					f = (float)s/2000.f;
					f = fMin(f,1.f);

					_pst_Move->x = blend(_pst_Move->x, 0.f, f);
					//_pst_Move->y = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].acc.z * -2.f;
				}
				/*f = OSTicksToMilliseconds(OSDiffTick(curTick,lastTick)) / 100.f;
				f = fMin(f,1.f);
				_pst_Move->y = blend(lastY,_pst_Move->y,f);*/			
				_pst_Move->y = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].acc.z * -2.f;				
			}
			
			/*lastX = _pst_Move->x;
			lastY = _pst_Move->y;
			lastTick = curTick;*/
		}
		break;
	}

	_pst_Move->z = 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

static void revoINO_Joystick_GetPointer(MATH_tdst_Vector *_pst_Move)
{
	if(INO_gi_CurrentPadId == -1) return;
		
	_pst_Move->x = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].pos.x;
	_pst_Move->y =-revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].pos.y;
	if (revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].dpd_valid_fg < 2)
		_pst_Move->z = -1.0f;
	else
		_pst_Move->z = 1.0f;		
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void revoINO_Joystick_GetAcc(MATH_tdst_Vector *_pst_Move, int _i_Num)
{
	if(INO_gi_CurrentPadId == -1) return;

	if (_i_Num == 0)
	{
		_pst_Move->x = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].acc.x;
		_pst_Move->y = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].acc.y;
		_pst_Move->z = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].acc.z;
	}
	else
	{
		_pst_Move->x = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].ex_status.fs.acc.x; 
		_pst_Move->y = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].ex_status.fs.acc.y; 
		_pst_Move->z = revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].ex_status.fs.acc.z;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

#define InertiaThreshold 1.1f
#define ACC_BUFFER_SIZE 30	//Small buffers will favor the tail end of the slice
#define ACC_CONTINUOUS_BUFFER_SIZE 5	//Must be smaller than ACC_BUFFER_SIZE

static char 	Pad_CurrentMvt[2]  		= {IO_MVT_None,IO_MVT_None};
static bool		Pad_MvtIsOn[2] 			= {0,0};
static u32 accBufferValidCount[2] 		= {0,0};
static MATH_tdst_Vector accBuffer[2][ACC_BUFFER_SIZE];
static MATH_tdst_Vector gravity[2];
static MATH_tdst_Vector accContinuousBuffer[2][ACC_CONTINUOUS_BUFFER_SIZE];

static float TimerForNextRecognition[2] = {0.0f,0.0f};

void InitAccBuffer(int PadNum)
{
	u32 i;

	//ASSERT(ACC_BUFFER_SIZE > ACC_CONTINUOUS_BUFFER_SIZE);
	for(i=0; i<ACC_BUFFER_SIZE; i++)
	{
		accBuffer[PadNum][i].x = accBuffer[PadNum][i].y = accBuffer[PadNum][i].z = 0.0f;
	}

	accBufferValidCount[PadNum] = 0;

	gravity[PadNum].x = 0.0f;
	gravity[PadNum].y = 0.0f;
	gravity[PadNum].z = 0.0f;

} 

void AddAccContinuousBuffer(int PadNum, MATH_tdst_Vector* acc)
{
	s32 i;
	for(i=ACC_CONTINUOUS_BUFFER_SIZE-1; i>0; i--)
		accContinuousBuffer[PadNum][i] = accContinuousBuffer[PadNum][i-1];
	accContinuousBuffer[PadNum][0] = *acc;
}

void PreloadAccBuffer(int PadNum)
{
	u32 i;
	for(i=0; i<ACC_CONTINUOUS_BUFFER_SIZE; i++)
		accBuffer[PadNum][i] = accContinuousBuffer[PadNum][i];
	accBufferValidCount[PadNum] = ACC_CONTINUOUS_BUFFER_SIZE;
}

void AddAccBuffer(int PadNum,MATH_tdst_Vector* acc)
{
	s32 i;
	for(i=ACC_BUFFER_SIZE-1; i>0; i--)
	{
		MATH_CopyVector(&accBuffer[PadNum][i],&accBuffer[PadNum][i-1]);
	}
	MATH_CopyVector(&accBuffer[PadNum][0],acc);
	accBufferValidCount[PadNum]++;

	gravity[PadNum].x += acc->x;
	gravity[PadNum].y += acc->y;

}

void ComputeAverageVector(int PadNum)
{
	u32 i;
	BOOL validSwing = FALSE;
	MATH_tdst_Vector ave;
	MATH_tdst_Vector diff;
	MATH_tdst_Vector finalSlice;
	u32 end = accBufferValidCount[PadNum]-1;
	
	/*MATH_tdst_Vector vVTmp;
	int j;
	int CorrectData_P0,CorrectData_P3,NbIndex;
	float u,Bez0,Bez1,Bez2,Bez3;
	MATH_tdst_Vector vA,vB,vP0,vP1,vP2,vP3;

	// correct data now with BEZIER
	for(i=0; i<end; i++)
	{
		if ((fabs(accBuffer[PadNum][i].x) > 2.2f) || (fabs(accBuffer[PadNum][i].y) > 2.2f))
		{
			// check error size
			CorrectData_P0 = i - 1;
			for(j=i; j<end; j++)
			{
				if ((fabs(accBuffer[PadNum][j].x) < 2.2f) && (fabs(accBuffer[PadNum][j].y) < 2.2f))
					break;	
			}
			
			CorrectData_P3 = j;
			NbIndex = CorrectData_P3 - CorrectData_P0;
			MATH_CopyVector(&vP0,&accBuffer[PadNum][CorrectData_P0]);
			MATH_CopyVector(&vA,&accBuffer[PadNum][CorrectData_P0 - 1]);
			MATH_CopyVector(&vP3,&accBuffer[PadNum][CorrectData_P3]);
			MATH_CopyVector(&vB,&accBuffer[PadNum][CorrectData_P3 + 1]);
		
			//vP1 = (vP0 - vA) *  |vP0 - vA| * nbPoint * coeff
			MATH_SubVector(&vVTmp,&vP0,&vA);					
			MATH_ScaleEqualVector(&vVTmp,NbIndex);
			MATH_AddVector(&vP1,&vP0,&vVTmp);

			//vP2 = (vP3 - vB) *  |vP3 - vB| * nbPoint * coeff
			MATH_SubVector(&vVTmp,&vP3,&vB);					
			MATH_ScaleEqualVector(&vVTmp,NbIndex);
			MATH_AddVector(&vP2,&vP3,&vVTmp);	
		
			// Bezier
			for (j=CorrectData_P0 + 1;j<CorrectData_P3;j++)					
			{
				u = (j - CorrectData_P0) / NbIndex;
				Bez0 = (1 - u) * (1 - u) * (1 - u);
				Bez1 = 3 * u * (1 - u) * (1 - j);
				Bez2 = 3 * u * u * (1 - u);
				Bez3 = u * u * u;
				accBuffer[PadNum][j].x = Bez0 * vP0.x + Bez1 * vP1.x + Bez2 * vP2.x + Bez3 * vP3.x;
				accBuffer[PadNum][j].z = Bez0 * vP0.z + Bez1 * vP1.z + Bez2 * vP2.z + Bez3 * vP3.z;
			}
		}
	}*/	

	MATH_InitVectorToZero(&ave);
	for(i=0; i<end; i++)
	{
		diff.x = accBuffer[PadNum][i].x - accBuffer[PadNum][i+1].x;
		diff.y = accBuffer[PadNum][i].y - accBuffer[PadNum][i+1].y;
		diff.z = 0.0f;

		ave.x += diff.x;
		ave.y += diff.y;

		validSwing = TRUE;
	}

	if(validSwing)
	{
		VECNormalize(&ave, &ave);

		//Normalize final gravity and save it off
		VECNormalize(&gravity[PadNum], &gravity[PadNum]);

		//Rotate the slice direction based on gravity
		finalSlice.x = -gravity[PadNum].y*ave.x + gravity[PadNum].x*ave.y;
		finalSlice.y = -gravity[PadNum].x*ave.x - gravity[PadNum].y*ave.y;
		
		if (fabs(finalSlice.x) > (0.75 * fabs(finalSlice.y)))
		{
			if (finalSlice.x > 0.0f)
				Pad_CurrentMvt[PadNum] = IO_MVT_RightToLeft;
			else
				Pad_CurrentMvt[PadNum] = IO_MVT_LeftToRight;				
		}
		else
		{
			if (finalSlice.y > 0.0f)
				Pad_CurrentMvt[PadNum] = IO_MVT_DownToUp;
			else
				Pad_CurrentMvt[PadNum] = IO_MVT_UpToDown;				
		}	
	}
}
 
// need filter to be improved !!!
void revo_CheckMvtRecognition() 
{
	int i = 0;
	MATH_tdst_Vector vec;
	
	if(INO_gi_CurrentPadId == -1) 
	{
		Pad_CurrentMvt[0] = IO_MVT_None;
		Pad_CurrentMvt[1]   = IO_MVT_None;	
		return;
	}
	
	if (TimerForNextRecognition[0] > 0.0f)
		TimerForNextRecognition[0] -= TIM_gf_dt;
	if (TimerForNextRecognition[1] > 0.0f)
		TimerForNextRecognition[1] -= TIM_gf_dt;
	
	// check all burrefised acc value
	for(i=NumReads - 1;i>=0;i--)
	{
		// MAIN pad
		MATH_CopyVector(&vec,&revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][i].acc);
		AddAccContinuousBuffer(0,&vec);					
		if (TimerForNextRecognition[0] <= 0.0f)
		{
			if (vec.z > InertiaThreshold)
			{
				if(!Pad_MvtIsOn[0])
				{
					Pad_MvtIsOn[0] = 1;
					PreloadAccBuffer(0);
				}
				AddAccBuffer(0,&vec);
			}			
			else
			{
				if(Pad_MvtIsOn[0] && accBufferValidCount[0] >= 3)//(ACC_CONTINUOUS_BUFFER_SIZE + 2))
					ComputeAverageVector(0);
				Pad_MvtIsOn[0] = 0;
				InitAccBuffer(0);
			}
		}			
		
		// FS pad
		MATH_CopyVector(&vec,&revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][i].ex_status.fs.acc);
		AddAccContinuousBuffer(1,&vec);	
		if (TimerForNextRecognition[1] <= 0.0f)
		{				
			if (fabs(vec.z) > InertiaThreshold)
			{
				if(!Pad_MvtIsOn[1])
				{
					Pad_MvtIsOn[1] = 1;
					PreloadAccBuffer(1);
				}
				
				AddAccBuffer(1,&vec);
			}			
			else
			{
				if(Pad_MvtIsOn[1] && accBufferValidCount[1] >= (ACC_CONTINUOUS_BUFFER_SIZE + 2))
					ComputeAverageVector(1);				
				Pad_MvtIsOn[1] = 0;
				InitAccBuffer(1);
			}
		}			
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
 
int revoINO_Joystick_GetLastMove(int _i_Num) 
{
	int CurrentMove = IO_MVT_None;
	
	if(INO_gi_CurrentPadId == -1) 
		return IO_MVT_None;

	if (_i_Num == 0)
	{
		// reinit if a mvt has been done
		CurrentMove = Pad_CurrentMvt[0];
		if (CurrentMove != IO_MVT_None)
			TimerForNextRecognition[0] = 0.15f;
		Pad_CurrentMvt[0] = IO_MVT_None;
	}
	else
	{
		// reinit if a mvt has been done
		CurrentMove = Pad_CurrentMvt[1];
		if (CurrentMove != IO_MVT_None)
			TimerForNextRecognition[1] = 0.15f;
		Pad_CurrentMvt[1] = IO_MVT_None;
	}
	return CurrentMove;	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
 
float revoINO_Joystick_GetDistance() 
{
	if(INO_gi_CurrentPadId == -1) 
		return 0.0f;

	if (revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].dpd_valid_fg >= 2)
		return revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].dist;
	else
		return 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
 
void revoINO_Joystick_SetVibration(int i_Num,float f_Delay) 
{
	if(INO_gi_CurrentPadId == -1) 
		return;

	if (i_Num == 0)
		fMainPadVibrationDelay = f_Delay;
	else
		fFSPadVibrationDelay = f_Delay;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void revoINO_SetMappingJack()
{
	gcINO_au16_PadMapping[e_SEL] = KPAD_BUTTON_SELECT;
	gcINO_au16_PadMapping[e_START] = PAD_BUTTON_START;

	gcINO_au16_PadMapping[e_L1] = PAD_TRIGGER_L; 
	gcINO_au16_PadMapping[e_R1] = PAD_BUTTON_X; 

	gcINO_au16_PadMapping[e_L2] = PAD_TRIGGER_L;
	gcINO_au16_PadMapping[e_R2] = PAD_TRIGGER_R;
	
	gcINO_au16_PadMapping[e_R3] = PAD_TRIGGER_Z;
	
	gcINO_au16_PadMapping[e_TRIANGLE] = PAD_BUTTON_RIGHT | PAD_BUTTON_LEFT; 
	gcINO_au16_PadMapping[e_CIRCLE] = PAD_BUTTON_Y; 
	gcINO_au16_PadMapping[e_CROSS] = PAD_BUTTON_A;
	gcINO_au16_PadMapping[e_SQUARE] = PAD_BUTTON_B;

	gcINO_au16_PadMapping[e_UP] = PAD_BUTTON_UP;
	gcINO_au16_PadMapping[e_DOWN] = PAD_BUTTON_DOWN;
	gcINO_au16_PadMapping[e_LEFT] = PAD_BUTTON_LEFT;
	gcINO_au16_PadMapping[e_RIGHT] = PAD_BUTTON_RIGHT;
}

void revoINO_SetMappingKong()
{
	gcINO_au16_PadMapping[e_SEL] = KPAD_BUTTON_SELECT;
	gcINO_au16_PadMapping[e_START] = KPAD_BUTTON_START;

	gcINO_au16_PadMapping[e_L2] = 0; 
	gcINO_au16_PadMapping[e_R2] = KPAD_BUTTON_B; 
 
	gcINO_au16_PadMapping[e_L1] = 0; 
	gcINO_au16_PadMapping[e_R1] = 0;  
 
	gcINO_au16_PadMapping[e_L3] = 0;
	gcINO_au16_PadMapping[e_R3] = 0;
	
	gcINO_au16_PadMapping[e_TRIANGLE] = KPAD_BUTTON_SMALL_B;  
	gcINO_au16_PadMapping[e_CIRCLE] = KPAD_BUTTON_A;  
	gcINO_au16_PadMapping[e_CROSS] = KPAD_BUTTON_Z1; 
	gcINO_au16_PadMapping[e_SQUARE] = 0; 
	
	gcINO_au16_PadMapping[e_UP] = KPAD_BUTTON_UP; 
	gcINO_au16_PadMapping[e_DOWN] = KPAD_BUTTON_DOWN; 
	gcINO_au16_PadMapping[e_LEFT] = KPAD_BUTTON_LEFT; 
	gcINO_au16_PadMapping[e_RIGHT] = KPAD_BUTTON_RIGHT; 
}

void revoINO_SetMappingMenu()
{ 
	gcINO_au16_PadMapping[e_SEL] = KPAD_BUTTON_HOME;
	gcINO_au16_PadMapping[e_START] = KPAD_BUTTON_START;
	
	gcINO_au16_PadMapping[e_L1] = KPAD_BUTTON_Z2; // FPS Mode 
	gcINO_au16_PadMapping[e_R1] = 0;  // nothing
	
	gcINO_au16_PadMapping[e_L2] = KPAD_BUTTON_Z2; // FPS mode
	gcINO_au16_PadMapping[e_R2] = KPAD_BUTTON_Z1; // Roll and boost

	gcINO_au16_PadMapping[e_L3] = KPAD_BUTTON_SELECT;  // nothing
	gcINO_au16_PadMapping[e_R3] = 0; // nothing

	gcINO_au16_PadMapping[e_TRIANGLE] = KPAD_BUTTON_SMALL_B; // costume
	gcINO_au16_PadMapping[e_CIRCLE] = KPAD_BUTTON_SMALL_A; // zic
	gcINO_au16_PadMapping[e_CROSS] = KPAD_BUTTON_A;	// Jump
	gcINO_au16_PadMapping[e_SQUARE] = KPAD_BUTTON_B;// grab/Use
	
	gcINO_au16_PadMapping[e_UP] = KPAD_BUTTON_UP;
	gcINO_au16_PadMapping[e_DOWN] = KPAD_BUTTON_DOWN;
	gcINO_au16_PadMapping[e_LEFT] = KPAD_BUTTON_LEFT;
	gcINO_au16_PadMapping[e_RIGHT] = KPAD_BUTTON_RIGHT;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 
static void revoINO_Joystick_Init(HWND none)
{
#pragma unused(none)
	gcINO_gi_Special = 0;
	INO_gi_CurrentPadId = -1;

//	L_memset(gcINO_gax_PreviousPadData, 0, PAD_MAX_CONTROLLERS * sizeof(PADStatus));
//	L_memset(gcINO_gax_CurrentPadData, 0, PAD_MAX_CONTROLLERS * sizeof(PADStatus));
//	L_memset(gcINO_gaiZTriggerToggle, 0, PAD_MAX_CONTROLLERS * sizeof(u8));
//	L_memset(gcINO_gabPreviousZTrigger, 0, PAD_MAX_CONTROLLERS * sizeof(u8));

	L_memset(gcINO_gax_PortState, 0, PAD_MAX_CONTROLLERS * sizeof(gcINO_tden_PortState));
	
	// Revolution Pad init
	L_memset(revoINO_gax_CurrentPadData, 0,KPAD_MAX_READ_BUFS * PAD_MAX_CONTROLLERS * sizeof(KPADStatus));
	L_memset(revoINO_gax_PreviousRevoPadData, 0, KPAD_MAX_READ_BUFS * PAD_MAX_CONTROLLERS * sizeof(KPADStatus));
	KPADInit() ;
	
	KPADSetPosParam ( 0,  0.2f,  1.0f ) ;
	KPADSetHoriParam( 0, 0.2f, 1.0f ) ;
	KPADSetDistParam( 0, 0.2f, 1.0f ) ;
	KPADSetAccParam ( 0,  0.2f,  1.0f ) ;
	KPADSetBtnRepeat( 0, 0.75f, 0.25f ) ;	
	KPADSetAccParam(0,0,1); //Acc values are not altered
	InitAccBuffer(0);
	InitAccBuffer(1);
				
	gcINO_au16_PadMapping[e_Status] = 0x4000;
	gcINO_au16_PadMapping[e_LowerUnit] = 0x4000;
	gcINO_au16_PadMapping[e_UpperUnit] = 0x4000;
	gcINO_au16_PadMapping[e_SEL] = 0x4000;
	gcINO_au16_PadMapping[e_L3] = 0x4000;
	gcINO_au16_PadMapping[e_R3] = 0x4000;
	gcINO_au16_PadMapping[e_UP] = 0x4000; // old : PAD_BUTTON_UP;
	gcINO_au16_PadMapping[e_RIGHT] = 0x4000; // PAD_BUTTON_RIGHT;
	gcINO_au16_PadMapping[e_DOWN] = 0x4000; // PAD_BUTTON_DOWN;
	gcINO_au16_PadMapping[e_LEFT] = 0x4000; // PAD_BUTTON_LEFT;

	gcINO_au16_PadMapping[e_AnaStickRightX] = 0;
	gcINO_au16_PadMapping[e_AnaStickRightY] = 0;
	gcINO_au16_PadMapping[e_AnaStickLeftX] = 0;
	gcINO_au16_PadMapping[e_AnaStickLeftY] = 0;

	gcINO_au16_PadMapping[e_InfoR] = 0;
	gcINO_au16_PadMapping[e_InfoL] = 0;
	gcINO_au16_PadMapping[e_InfoU] = 0;
	gcINO_au16_PadMapping[e_InfoD] = 0;
	gcINO_au16_PadMapping[e_InfoT] = 0;
	gcINO_au16_PadMapping[e_InfoC] = 0;
	gcINO_au16_PadMapping[e_InfoX] = 0;
	gcINO_au16_PadMapping[e_InfoS] = 0;

	gcINO_au16_PadMapping[e_InfoL1] = 0;
	gcINO_au16_PadMapping[e_InfoR1] = 0;
	gcINO_au16_PadMapping[e_InfoL2] = 0;
	gcINO_au16_PadMapping[e_InfoR2] = 0;
	
	revoINO_SetMappingMenu();

	INO_l_Joystick_Enable = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL revoINO_b_Joystick_IsValid(void)
{
	if(INO_gi_CurrentPadId == -1) return FALSE;
	if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].wpad_err == PAD_ERR_NO_CONTROLLER) return FALSE;
	if(revoINO_gax_CurrentPadData[INO_gi_CurrentPadId][0].wpad_err == PAD_ERR_NOT_READY) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void revoINO_SetControlMode(int mode)
{
	iCurrentControlMode = mode;
	switch(mode)
	{
	case IO_MODE_JACK:
		revoINO_SetMappingJack();
		break;

	case IO_MODE_KONG:
		revoINO_SetMappingKong();
		break;

	case IO_MODE_MENU:
		revoINO_SetMappingMenu();
		break;
	}
}

#endif // REVO

#elif defined(_XBOX)

/*$4
 ***********************************************************************************************************************
    XBOX
 ***********************************************************************************************************************
 */

/*$2------------------------------------------------------------------------------------------------------------------*/

#define xbINO_Cte_PortNb	4
#define xbINO_Cte_SlotNb	2

/*$2------------------------------------------------------------------------------------------------------------------*/

typedef enum	xbINO_tden_PortState_
{
	eState_Disconnected			= 0,
	eState_WaitForOpening,
	eState_WaitForGettingCapa,
	eState_WaitForReading,
	eState_Valid,
	eState_Invalid,
	eState_WaitForClosing,
	eState_WaitForReconnecting
} xbINO_tden_PortState;

typedef struct	xbINO_tdst_PadDesc_
{
	xbINO_tden_PortState	en_State;
	HANDLE					h_Handle;
	XINPUT_STATE			x_Data;
	XINPUT_CAPABILITIES		x_Capa;
	XINPUT_FEEDBACK			x_Feedback;
	int						i_Error;
} xbINO_tdst_PadDesc;

/*$2------------------------------------------------------------------------------------------------------------------*/

static xbINO_tdst_PadDesc	xbINO_ax_PadDesc[xbINO_Cte_PortNb];


bool INO_gb_PadDetected = FALSE;

BOOL xbINO_b_Joystick_IsAssigned(void)
{
    return INO_gb_PadDetected;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*void xbINO_SetMappingJack()
{
}

void xbINO_SetMappingKong()
{
}*/

void xbINO_SetControlMode(int mode)
{
ControlMode = mode;
/*	switch(mode)
	{
	case IO_MODE_JACK:
		xbINO_SetMappingJack();
		break;

	case IO_MODE_KONG:
		xbINO_SetMappingKong();
		break;
	}*/
}

static void xbINO_Joystick_Init(HWND _hWnd)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DWORD					dwConnectedPads;
	int						i;
	XDEVICE_PREALLOC_TYPE	xdpt[] = { { XDEVICE_TYPE_GAMEPAD, 4 }, { XDEVICE_TYPE_MEMORY_UNIT, 8 } };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- reset globals ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(xbINO_ax_PadDesc, 0, xbINO_Cte_PortNb * sizeof(xbINO_tdst_PadDesc));
	INO_gi_CurrentPadId = -1;

	/*$1- init devices ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	XInitDevices(sizeof(xdpt) / sizeof(XDEVICE_PREALLOC_TYPE), xdpt);

	/*$1- get first image of connected devices ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dwConnectedPads = XGetDevices(XDEVICE_TYPE_GAMEPAD);
	for(i = 0; i < xbINO_Cte_PortNb; i++)
	{
		if(dwConnectedPads & (1 << i)) xbINO_ax_PadDesc[i].en_State = eState_WaitForOpening;
	}

	INO_l_Joystick_Enable = 1;
    INO_gb_PadDetected = FALSE;
	ControlMode=1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void xbINO_Joystick_Close(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < xbINO_Cte_PortNb; i++)
	{
		if(xbINO_ax_PadDesc[i].h_Handle) XInputClose(xbINO_ax_PadDesc[i].h_Handle);
		xbINO_ax_PadDesc[i].h_Handle = NULL;
		xbINO_ax_PadDesc[i].en_State = eState_Init;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */


bool xbINO_Joystick_IsJoyValid(void)
{
	int i;
	for(i = 0; i < xbINO_Cte_PortNb; i++)
	{
		if(xbINO_ax_PadDesc[i].en_State==eState_Valid)
			return true;
	}

	return false;

}

// this function count how many pads are connected. Also update the second pad id variable.
int xbINO_Joystick_IsJoyValid2(void)
{
	int i;
	int padCount=0;

	for(i = 0; i < xbINO_Cte_PortNb; i++)
	{
		if(xbINO_ax_PadDesc[i].en_State==eState_Valid)
		{
			padCount++;
			// if this pad is not already a first or second pad
			if ((i!=INO_gi_CurrentFirstPadId) && (i!=INO_gi_CurrentSecondPadId))
				// the first pad is connected
				if (xbINO_ax_PadDesc[INO_gi_CurrentFirstPadId].en_State==eState_Valid)
					// the second pad id is not valid
					if ((INO_gi_CurrentSecondPadId==-1) || (xbINO_ax_PadDesc[INO_gi_CurrentSecondPadId].en_State==eState_Disconnected))
						// update the second pad id variable
						INO_gi_CurrentSecondPadId=i;
		}
	}

	/*
	if (padCount<2)
		INO_gi_CurrentSecondPadId=-1;
	*/

	return (padCount);
}

bool xbINO_CheckFirstPadConnected(void)
{
	return (xbINO_ax_PadDesc[INO_gi_CurrentFirstPadId].en_State==eState_Disconnected);
}

HANDLE xbINO_Joystick_GetHandles(int no)
{
	return xbINO_ax_PadDesc[no].h_Handle;
}

static void xbINO_Joystick_Update(INO_tdst_GenericPad *pCurr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	DWORD	dwInsert, dwRemove;
	int		i, j;
	DWORD	dw;
	int		PadNb;
	int		p;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	
	PadNb = 0;
	
	if(XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsert, &dwRemove))
	{
		for(i = 0; i < xbINO_Cte_PortNb; i++)
		{
			if((dwRemove & (1 << i)) && (dwInsert & (1 << i)))
				xbINO_ax_PadDesc[i].en_State = eState_WaitForReconnecting;
			else if(dwRemove & (1 << i))
				xbINO_ax_PadDesc[i].en_State = eState_WaitForClosing;
			else if(dwInsert & (1 << i))
				xbINO_ax_PadDesc[i].en_State = eState_WaitForOpening;
		}
	}

	p=INO_gi_CurrentFirstPadId;
//	INO_gi_CurrentPadId=-1;

	if (xbINO_ax_PadDesc[p].en_State == eState_WaitForClosing && !Bink_PlayVideo
		|| (p != -1 && xbINO_ax_PadDesc[p].en_State == eState_Disconnected && INO_gi_CurrentPadId == p && !Bink_PlayVideo)
		)
	{
        BOOL ToutOk=0;
		SND_MuteAll(TRUE);
        SND_TrackPauseAll(TRUE);
		SND_Update(NULL);

		if(xbINO_ax_PadDesc[p].h_Handle) XInputClose(xbINO_ax_PadDesc[p].h_Handle);
		xbINO_ax_PadDesc[p].h_Handle = NULL;//ferme
		//-----------------------------------------


//		while (xbINO_ax_PadDesc[p].en_State!=eState_WaitForOpening && !ToutOk)
		while (!ToutOk)
		{
			
			if(XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsert, &dwRemove))
			{
		//		for(i = 0; i < xbINO_Cte_PortNb; i++)
				{
					if((dwRemove & (1 << p)) && (dwInsert & (1 << p)))
						xbINO_ax_PadDesc[p].en_State = eState_WaitForReconnecting;
					else if(dwRemove & (1 << p))
						xbINO_ax_PadDesc[p].en_State = eState_WaitForClosing;
					else if(dwInsert & (1 << p))
						xbINO_ax_PadDesc[p].en_State = eState_WaitForOpening;
				}
			}
				 
		//}

		if (xbINO_ax_PadDesc[p].en_State == eState_WaitForClosing || xbINO_ax_PadDesc[p].en_State==eState_Disconnected) 
		{
			if(xbINO_ax_PadDesc[p].h_Handle)
			{
				XInputClose(xbINO_ax_PadDesc[p].h_Handle);
				xbINO_ax_PadDesc[p].h_Handle = NULL;//ferme
			}
			Gx8_HandleNoPad_setMessage(1);
			Gx8_HandleNoPadPresent();
		}

		if (xbINO_ax_PadDesc[p].en_State == eState_WaitForOpening)
		{
			xbINO_ax_PadDesc[p].h_Handle = XInputOpen(XDEVICE_TYPE_GAMEPAD, p, XDEVICE_NO_SLOT, NULL);
			if(xbINO_ax_PadDesc[p].h_Handle)
			{
				xbINO_ax_PadDesc[p].i_Error = 0;
				xbINO_ax_PadDesc[p].en_State = eState_WaitForGettingCapa;
			}
		
			dw = XInputGetCapabilities(xbINO_ax_PadDesc[p].h_Handle, &xbINO_ax_PadDesc[p].x_Capa);
			if(dw == ERROR_SUCCESS)
			{
				xbINO_ax_PadDesc[p].i_Error = 0;
				xbINO_ax_PadDesc[p].en_State = eState_WaitForReading;
			}
		}
		
		if (xbINO_ax_PadDesc[p].en_State == eState_WaitForReading)
		{
			//bool startok=0;
				//while (!startok)
			{
				Gx8_HandleNoPad_setMessage(2);
				Gx8_HandleNoPadPresent();

				// alors desactive tout les test start des autres port 
				//for(i = 0; i < xbINO_Cte_PortNb; i++)
				{
					if(xbINO_ax_PadDesc[p].h_Handle)//donc pas la variable i mais p=current pad
					{
						static int okstart=0;
						XInputGetState(xbINO_ax_PadDesc[p].h_Handle, &xbINO_ax_PadDesc[p].x_Data);
						if ((xbINO_ax_PadDesc[p].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_START) && okstart==0) okstart=1;
						else if (!(xbINO_ax_PadDesc[p].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_START) && okstart==1) 
						okstart=2;

						if (okstart==2)
						{
							okstart=0;
							// donc pas toucher au autre pad
							//xbINO_i_SetCurrentPad(i);
							//INO_gi_CurrentFirstPadId=i;
							
							//startok=1;

							SND_MuteAll(FALSE);
							SND_TrackPauseAll(FALSE);
							SND_Update(NULL);
							Gx8_HandleNoPad_setMessage(0);
							ToutOk=1;
							break;
							
						}
					}
				}//for
			}	
		}//reading
	}
}
	for(i = 0; i < xbINO_Cte_PortNb; i++)
	{
		switch(xbINO_ax_PadDesc[i].en_State)
		{
		case eState_WaitForOpening:
			xbINO_ax_PadDesc[i].h_Handle = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL);
			if(xbINO_ax_PadDesc[i].h_Handle)
			{
				xbINO_ax_PadDesc[i].i_Error = 0;
				xbINO_ax_PadDesc[i].en_State = eState_WaitForGettingCapa;
			}
			else
			{
				xbINO_ax_PadDesc[i].i_Error++;
				if(xbINO_ax_PadDesc[i].i_Error > 10) 
				xbINO_ax_PadDesc[i].en_State = eState_Disconnected;
			}
			break;

		case eState_WaitForGettingCapa:
			dw = XInputGetCapabilities(xbINO_ax_PadDesc[i].h_Handle, &xbINO_ax_PadDesc[i].x_Capa);
			if(dw == ERROR_SUCCESS)
			{
				xbINO_ax_PadDesc[i].i_Error = 0;
				xbINO_ax_PadDesc[i].en_State = eState_WaitForReading;
			}
			else
			{
				xbINO_ax_PadDesc[i].i_Error++;
				if(xbINO_ax_PadDesc[i].i_Error > 10) 
				xbINO_ax_PadDesc[i].en_State = eState_WaitForClosing;
			}
			break;

		case eState_WaitForReading:
		case eState_Valid:
		case eState_Invalid:
			dw = XInputGetState(xbINO_ax_PadDesc[i].h_Handle, &xbINO_ax_PadDesc[i].x_Data);
			if(dw == ERROR_SUCCESS)
			{
				int iii;
				PadNb++;
				xbINO_ax_PadDesc[i].i_Error = 0;
				xbINO_ax_PadDesc[i].en_State = eState_Valid;

				if(INO_gi_CurrentPadId == -1)
				{
					if(INO_gb_PadDetected && (INO_gi_LastPadId == i))
					{
						INO_gi_CurrentPadId = INO_gi_CurrentFirstPadId = i; 

						for(iii = 0; iii < xbINO_Cte_PortNb; iii++)
						{
							if(xbINO_ax_PadDesc[iii].en_State == eState_Valid)
								INO_gai_AbsentPad[iii] = 0;
							else
								INO_gai_AbsentPad[iii] = 1;
						}
					}
					else if(INO_gb_PadDetected && INO_gai_AbsentPad[i])
					{
						INO_gi_LastPadId = INO_gi_CurrentFirstPadId = INO_gi_CurrentPadId = i; 

						for(iii = 0; iii < xbINO_Cte_PortNb; iii++)
						{
							if(xbINO_ax_PadDesc[iii].en_State == eState_Valid)
								INO_gai_AbsentPad[iii] = 0;
							else
								INO_gai_AbsentPad[iii] = 1;
						}
					}
				}
			}
			else
			{
				xbINO_ax_PadDesc[i].i_Error++;
				xbINO_ax_PadDesc[i].en_State = eState_Invalid;
				if(xbINO_ax_PadDesc[i].i_Error > 10) xbINO_ax_PadDesc[i].en_State = eState_WaitForClosing;
			}
			break;

		case eState_WaitForClosing:
			//if (!Bink_PlayVideo)
			{
			if(xbINO_ax_PadDesc[i].h_Handle) XInputClose(xbINO_ax_PadDesc[i].h_Handle);
			xbINO_ax_PadDesc[i].h_Handle = NULL;
			xbINO_ax_PadDesc[i].en_State = eState_Disconnected;
			}
			break;

		case eState_WaitForReconnecting:
			if(xbINO_ax_PadDesc[i].h_Handle) XInputClose(xbINO_ax_PadDesc[i].h_Handle);
			xbINO_ax_PadDesc[i].h_Handle = NULL;
			xbINO_ax_PadDesc[i].en_State = eState_WaitForOpening;
			break;

		case eState_Disconnected:
			if(INO_gi_CurrentPadId == i) INO_gi_CurrentPadId = -1;
			break;

		default:
			if(INO_gi_CurrentPadId == i) INO_gi_CurrentPadId = -1;
			xbINO_ax_PadDesc[i].en_State = eState_Disconnected;
			break;
		}

		for(j = 0; j < eBtn_GenericButtonNb; j++)
		{
			extern ULONG h_SaveWorldKey;
			
			pCurr[i].ac_Button[j] = xbINO_uc_GetButton(i, j);
			pCurr[i].ac_Button[j] = pCurr[i].ac_Button[j] < 10 ? 0 : pCurr[i].ac_Button[j];

			if(xbINO_ax_PadDesc[i].en_State != eState_Valid) continue;
			if(!INO_gb_PadDetected  && (INO_gi_CurrentPadId == -1) && pCurr[i].ac_Button[j])
			// ONLY START BUTTON et Maintenant si on est dans un monde special->¨Logo
			//if (i!=9) continue;
			//Voir aussi si le no ne change pas en fonction du setmodecontrol
			if((!INO_gb_PadDetected  && (INO_gi_CurrentPadId == -1) && pCurr[i].ac_Button[9]) && !Bink_PlayVideo)//&& h_SaveWorldKey!=0x3d00c456)
			{
				int iii;
				INO_gi_LastPadId  = INO_gi_CurrentFirstPadId = INO_gi_CurrentPadId = i;
				INO_gb_PadDetected = TRUE;
				//Bink_PlayVideo = false;
				INO_gi_PadConnectedNb = 0;
				for(iii = 0; iii < xbINO_Cte_PortNb; iii++)
				{
					if(xbINO_ax_PadDesc[iii].en_State == eState_Valid)
						INO_gai_AbsentPad[iii] = 0;
					else
						INO_gai_AbsentPad[iii] = 1;
				}
			}

#ifdef _XBOX
/*			if(pCurr[i].ac_Button[9]==0)
			{
				if ( Bink_PlayVideo ) 
				Bink_PlayVideo = FALSE;
			}*/

			//reset the demo timer if an input is occured...
			//#ifdef ATTRACTMODE

			if(pCurr[i].ac_Button[j]!=0)
			{
				P_ResetTimer();//P_ResetTimer();//TimeBeforeVideo=0;//DEM_ResetTimer();
				DEM_ResetTimer();
			}
				//#endif
			/*if(xbINO_uc_GetButton(i,eBtn_Cross) || xbINO_uc_GetButton(i, eBtn_Start))
				STOPVIDEO=1;//DEM_StopAttractMode();*/
#endif
		}

		if(!INO_gb_PadDetected  && (INO_gi_CurrentPadId == -1))
		{
			if(xbINO_ax_PadDesc[i].en_State != eState_Valid) continue;

			if(	fAbs(xbINO_ax_PadDesc[i].x_Data.Gamepad.sThumbLX/ 32767.0f)> 0.3f||
				fAbs(xbINO_ax_PadDesc[i].x_Data.Gamepad.sThumbLY/ 32767.0f)> 0.3f||
				fAbs(xbINO_ax_PadDesc[i].x_Data.Gamepad.sThumbRX/ 32767.0f)> 0.3f||
				fAbs(xbINO_ax_PadDesc[i].x_Data.Gamepad.sThumbRY/ 32767.0f)> 0.3f )
			{
				int iii;

				//tempo deactive<<<<<<<<<<<<<<<<<<car il faut presser un boutton pour detecter
				// a virer only demo
				//INO_gi_LastPadId = INO_gi_CurrentFirstPadId = INO_gi_CurrentPadId = i;
				//INO_gb_PadDetected = TRUE;			
				
				INO_gi_PadConnectedNb = 0;
				for(iii = 0; iii < xbINO_Cte_PortNb; iii++)
				{
					if(xbINO_ax_PadDesc[iii].en_State == eState_Valid)
						INO_gai_AbsentPad[iii] = 0;
					else
						INO_gai_AbsentPad[iii] = 1;
				}
			}
		}
	}
/*
	if(INO_gb_PadDetected && (INO_gi_CurrentPadId ==-1)&& (PadNb != INO_gi_PadConnectedNb))
	{
		INO_gi_CurrentPadId = -1;
		INO_gi_LastPadId = -1;
		INO_gb_PadDetected = FALSE;
	}*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned char xbINO_uc_GetButton(int port, int button)
{
	/*static int LastToggle=0;//--> Temporaire <--
	int Toggletemp=0;
	static bool Toggle=0;*/

	if(port < 0) return 0;
	if(port >= xbINO_Cte_PortNb) return 0;
	if(xbINO_ax_PadDesc[port].en_State != eState_Valid) return 0;

/*
	xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A];
	xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B];
	xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X];
	xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y];
	xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE];
	xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK];
	xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
	xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER];
	(xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 0xFF : 0;
	(xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 0xFF : 0;
	(xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 0xFF : 0;
	(xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 0xFF : 0;
	(xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 0xFF : 0;
	(xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 0xFF : 0;
	(xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 0xFF : 0;
	(xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 0xFF : 0;

	
*/

if ( ControlMode==0 ) //JACK
{
	switch(button)
	{
	case eBtn_Cross:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A];
	case eBtn_Circle:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B];
	case eBtn_Square:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK];
	case eBtn_Triangle:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X];
	case eBtn_L1:
/*
		Toggletemp=(xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 0xFF : 0;		
		if (Toggletemp && !LastToggle) Toggle = !Toggle;
		LastToggle=Toggletemp;
		return Toggle ? 0xff : 0;
*/
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 0xFF : 0;
	case eBtn_R1:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y];
	case eBtn_L2:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
	case eBtn_R2:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER];
	case eBtn_Select:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 0xFF : 0;
	case eBtn_Start:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 0xFF : 0;
	case eBtn_R3://ACTIVE RASTER
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 0xFF : 0;
	case eBtn_L3://ACTIVE RASTER
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 0xFF : 0;

	case eBtn_Up:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 0xFF : 0;
	case eBtn_Right:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 0xFF : 0;
	case eBtn_Left:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 0xFF : 0;
	case eBtn_Down:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 0xFF : 0;
	default: return 0;
	}
}

else if ( ControlMode==1 ) //KONG
{
	switch(button)
	{
	case eBtn_Cross://
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A];
	case eBtn_Circle://
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B];
	case eBtn_Square://
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X];
	case eBtn_Triangle://
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y];
	case eBtn_L1://
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
	case eBtn_R1://
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
	case eBtn_L2:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE];
	case eBtn_R2:
		//return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 0xFF : 0;
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER];
	case eBtn_Select:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 0xFF : 0;
	case eBtn_Start:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 0xFF : 0;
	case eBtn_R3://ACTIVE RASTER
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 0xFF : 0;
	case eBtn_L3://ACTIVE RASTER
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK];
	case eBtn_Up:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 0xFF : 0;
	case eBtn_Right:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 0xFF : 0;
	case eBtn_Left:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 0xFF : 0;
	case eBtn_Down:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 0xFF : 0;
	default: return 0;
	}
}
else if ( ControlMode>1 )//2 = MENU
{
	//Toggle=LastToggle=0;

	switch(button)
	{
	case eBtn_Cross:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A];
	case eBtn_Circle:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X];
	case eBtn_Square:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y];
	case eBtn_Triangle:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B];
	case eBtn_L1:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
	case eBtn_R1:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER];
	case eBtn_L2:
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE];
	case eBtn_R2:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 0xFF : 0;
	case eBtn_Select:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 0xFF : 0;
	case eBtn_Start:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 0xFF : 0;
	case eBtn_R3://ACTIVE RASTER
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 0xFF : 0;
	case eBtn_L3://ACTIVE RASTER
		return xbINO_ax_PadDesc[port].x_Data.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK];
	case eBtn_Up:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 0xFF : 0;
	case eBtn_Right:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 0xFF : 0;
	case eBtn_Left:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 0xFF : 0;
	case eBtn_Down:
		return (xbINO_ax_PadDesc[port].x_Data.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 0xFF : 0;
	default: return 0;
	}
}

return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void xbINO_Joystick_Move(MATH_tdst_Vector *_pst_Move, int _i_Num)
{
	static float LastP1x;
	static float LastP1y;
	static float LastP2x;
	static float LastP2y;

	if(INO_gi_CurrentPadId == -1) return;
	if(xbINO_ax_PadDesc[INO_gi_CurrentPadId].en_State != eState_Valid) return;

	switch(_i_Num)
	{
	case 0:
	case 2:
		_pst_Move->x = (xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Data.Gamepad.sThumbLX) / 32767.0f;
		_pst_Move->y = (xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Data.Gamepad.sThumbLY) / 32767.0f;
		if((fabs(LastP1x-_pst_Move->x)>=0.5f) || ((fabs(LastP1y-_pst_Move->y)>=0.5f)))
		{
			//#ifdef ATTRACTMODE
			P_ResetTimer();//TimeBeforeVideo=0;//DEM_ResetTimer();
			DEM_ResetTimer();
			//#endif
			LastP1x=_pst_Move->x;
			LastP1y=_pst_Move->y;
		}
		break;

	case 1:
	case 3:
	default:
		_pst_Move->x = (xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Data.Gamepad.sThumbRX) / 32767.0f;
		_pst_Move->y = (xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Data.Gamepad.sThumbRY) / 32767.0f;
		if((fabs(LastP2x-_pst_Move->x)>=0.5f) || ((fabs(LastP2y-_pst_Move->y)>=0.5f)))
		{
			//#ifdef ATTRACTMODE
			P_ResetTimer();//TimeBeforeVideo=0;//DEM_ResetTimer();
			DEM_ResetTimer();
			//#endif
			LastP2x=_pst_Move->x;
			LastP2y=_pst_Move->y;
		}
		break;
	}

    if((_pst_Move->x < 0.1f) && (-0.1f < _pst_Move->x)) _pst_Move->x = 0.0f;
    if((_pst_Move->y < 0.1f) && (-0.1f < _pst_Move->y)) _pst_Move->y = 0.0f;
	_pst_Move->z = 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void xbINO_PadLowVibSet(BOOL _b_On)
{
	if(INO_gi_CurrentPadId == -1) return;
	if(xbINO_ax_PadDesc[INO_gi_CurrentPadId].en_State != eState_Valid) return;
	if(!xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Capa.Out.Rumble.wRightMotorSpeed) return;

	if(_b_On)
	{
		xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Feedback.Rumble.wRightMotorSpeed = xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Capa.Out.Rumble.wRightMotorSpeed;
	}
	else
		xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Feedback.Rumble.wRightMotorSpeed = 0;

	if(xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Feedback.Header.dwStatus==ERROR_IO_PENDING) return;

	XInputSetState( xbINO_ax_PadDesc[INO_gi_CurrentPadId].h_Handle, &xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Feedback);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void xbINO_PadHightVibSet(int _i_level)
{
	if(INO_gi_CurrentPadId == -1) return;
	if(xbINO_ax_PadDesc[INO_gi_CurrentPadId].en_State != eState_Valid) return;
	if(!xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Capa.Out.Rumble.wLeftMotorSpeed) return;

	_i_level = lAbs(_i_level);
	xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Feedback.Rumble.wLeftMotorSpeed =
		(WORD)((float)_i_level * (float)xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Capa.Out.Rumble.wLeftMotorSpeed / 255.0f);

	XInputSetState( xbINO_ax_PadDesc[INO_gi_CurrentPadId].h_Handle, &xbINO_ax_PadDesc[INO_gi_CurrentPadId].x_Feedback);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL xbINO_b_Joystick_IsValid(void)
{
	if(INO_gi_CurrentPadId == -1) return FALSE;
	if(xbINO_ax_PadDesc[INO_gi_CurrentPadId].en_State == eState_Valid) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbINO_i_SetCurrentPad(int id)
{
	switch(id)
	{
	case 0:
		// in this case the current pad became the first pad
		/*if(xbINO_ax_PadDesc[INO_gi_CurrentFirstPadId].en_State == eState_Valid)
			INO_gi_CurrentPadId=INO_gi_CurrentFirstPadId;
		else
			return -1;*/
		INO_gi_CurrentPadId=id;
		break;
	case 1:
		// if the second pad is currently valid
 		/*if ((INO_gi_CurrentSecondPadId>=0) && (xbINO_ax_PadDesc[INO_gi_CurrentSecondPadId].en_State == eState_Valid) && (INO_gi_CurrentSecondPadId!=INO_gi_CurrentFirstPadId))
		{
			// the current pad became the second pad
			INO_gi_CurrentPadId=INO_gi_CurrentSecondPadId;
			// two pads are required by the AI
			TwoPadGame=TRUE;
		}
		else
			return -1;*/
		INO_gi_CurrentPadId=id;
		break;
	case 2:
	case 3:
		//if(xbINO_ax_PadDesc[INO_gi_CurrentFirstPadId].en_State == eState_Valid)
			INO_gi_CurrentPadId=id;//INO_gi_CurrentFirstPadId;
		//else
		//	return -1;
		/*
		if(xbINO_ax_PadDesc[id].en_State == eState_Valid)
			INO_gi_CurrentPadId = id;
		else
			return -1;
        break;
		*/
	default:
		return -1;
	}

	return 0;
	}

#elif defined(INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD)

/*$4
 ***********************************************************************************************************************
    PC FOR RETAIL GAME (via DirectInput8)
 ***********************************************************************************************************************
 */

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

// this variables are not used but are required to make the editor link
LONG win32INO_l_JoyUSBPS2;
LONG win32INO_l_Joystick_Init = 0;
LONG win32INO_l_Joystick_Mode = INO_Joy_Ps2Mode;
LONG win32INO_l_Joystick_XLeft[2] = { 0, 0};
LONG win32INO_l_Joystick_XLeftStart[2] = {22000, 22000};
LONG win32INO_l_Joystick_XRightStart[2] = {42000,42000};
LONG win32INO_l_Joystick_XRight[2] = {65536, 65536};
LONG win32INO_l_Joystick_YUp[2] = {0,0};
LONG win32INO_l_Joystick_YUpStart[2] = {22000,22000};
LONG win32INO_l_Joystick_YDownStart[2] = {42000,42000};
LONG win32INO_l_Joystick_YDown[2] = {65536,65536};
LONG INO_l_Joystick_X[2];
LONG INO_l_Joystick_Y[2];

// this function are not used but are required to make the editor link
void INO_SelectJoyUSBPS2(int _dummy) {}
void win32INO_GetjoyUSBPS2(void) {}

HWND					pcINO_hwndWindow;
IDirectInput8*			pcINO_gp_DirectInput;
IDirectInputDevice8*	pcINO_gp_Mouse;
IDirectInputDevice8*	pcINO_gp_Keyboard;

MATH_tdst_Vector pcINO_gast_Pad[2];
MATH_tdst_Vector pcINO_ast_PadPosForInertia;
LONG pcINO_gui_Mouse_Z_Granularity;
unsigned char pcINO_gab_LastKeyboardState[256];
int pcINO_gi_MenuPos;
float pcINO_gf_KeyRepeatDelayTimer;
void (*pcINO_gpf_JoystickUpdate)(INO_tdst_GenericPad*, DIMOUSESTATE*, unsigned char*);
int pcINO_gi_ControlSubmode;
BOOL pcINO_gb_NumLock;


// tunable time constants
#define pcINO_C_VERTICAL_MOUSE_AUTOREST_DELAY	1.5f
#define pcINO_C_VERTICAL_MOUSE_STATE_DELAY		0.3f
#define pcINO_C_KEY_REPEAT_DELAY				0.05f
#define pcINO_C_WHEEL_PERSISTANCE				0.04f

// in order to simplify key mappings, we create new "virtual" keys for the mouse buttons
// and the mouse wheel. The values for this constants are chosen arbitrary in a way
// that they do not clash with any DirectInput DIK_ constant.
#define INO_DIK_LMOUSE		0x80
#define INO_DIK_CMOUSE		0x81
#define INO_DIK_RMOUSE		0x82
#define INO_DIK_WHEELUP		0x83
#define INO_DIK_WHEELDOWN	0x84

// key mappings
#define INO_KEY_PRIMARY_ACTION	0
#define INO_KEY_USE_OBJECT		1
#define INO_KEY_JUMP			2
#define INO_KEY_SUPER_ACTION	3 // same as INO_KEY_COMPASS
#define INO_KEY_COMPASS			3 // same as INO_KEY_SUPER_ACTION
#define INO_KEY_LOOK_MODE		4
#define INO_KEY_CENTER_VIEW		5
#define INO_KEY_CROUCH			6
#define INO_KEY_INVENTORY_UP	7
#define INO_KEY_INVENTORY_DOWN	8
#define INO_KEY_UP				9
#define INO_KEY_DOWN			10
#define INO_KEY_LEFT			11
#define INO_KEY_RIGHT			12
#define INO_KEY_RUN				13
#define INO_KEY_MAP				14
#define INO_KEY_MENU			15
#define INO_KEY_MENU_BACK		16
#define INO_KEY_MENU_INFO		17
#define INO_NUM_KEYS			18

static const char* pcINO_gs_RegKeyName = "Software\\Ubi Soft\\BGE PC\\SettingsApplication.INI\\Key bindings";
static const char* pcINO_gas_KeyNames[] =
{
	"Primary action",
	"Use object",
	"Jump",
	"Super action",
	"Look mode",
	"Center view",
	"Crouch",
	"Inventory up",
	"Inventory down",
	"Up",
	"Down",
	"Left",
	"Right",
	"Run",
	"Map",
	"Menu",
	"Menu back",
	"Menu info"
};

static unsigned pcINO_gai_KeyMappings[INO_NUM_KEYS][2] =
{
	{ INO_DIK_LMOUSE, 0 },			// INO_KEY_PRIMARY_ACTION
	{ DIK_Q, DIK_RETURN },			// INO_KEY_USE_OBJECT
	{ DIK_SPACE, DIK_RCONTROL },	// INO_KEY_JUMP	
	{ DIK_E, DIK_NUMPAD0 },			// INO_KEY_SUPER_ACTION
	{ DIK_LSHIFT, DIK_RSHIFT },		// INO_KEY_LOOK_MODE
	{ DIK_C, DIK_NUMPAD1 },			// INO_KEY_CENTER_VIEW
	{ DIK_LCONTROL,	DIK_APPS },		// INO_KEY_CROUCH	
	{ DIK_2, DIK_PRIOR },			// INO_KEY_INVENTORY_UP
	{ DIK_3, DIK_NEXT },			// INO_KEY_INVENTORY_DOWN
	{ DIK_W, DIK_UP },				// INO_KEY_UP
	{ DIK_S, DIK_DOWN },			// INO_KEY_DOWN
	{ DIK_A, DIK_LEFT },			// INO_KEY_LEFT
	{ DIK_D, DIK_RIGHT },			// INO_KEY_RIGHT
	{ INO_DIK_RMOUSE, 0 },			// INO_KEY_RUN
	{ DIK_TAB, DIK_BACK },			// INO_KEY_MAP
	{ DIK_ESCAPE, DIK_PAUSE },		// INO_KEY_MENU
	{ INO_DIK_RMOUSE, DIK_ESCAPE },	// INO_KEY_MENU_BACK
	{ INO_DIK_CMOUSE, DIK_SPACE }	// INO_KEY_MENU_INFO
};

#define KEYPRESSED_(key, i) _pb_KeyboardState[pcINO_gai_KeyMappings[key][i]]
#define KEYTOGGLED_(key, i) (_pb_KeyboardState[pcINO_gai_KeyMappings[key][i]] \
							&& !pcINO_gab_LastKeyboardState[pcINO_gai_KeyMappings[key][i]])

#ifdef INO_JOYSTICK_USE_CHEATS
#define KEYPRESSED(key) (KEYPRESSED_(key, 0) ||	(pcINO_gb_NumLock && KEYPRESSED_(key, 1)))
#define KEYTOGGLED(key) (KEYTOGGLED_(key, 0) ||	(pcINO_gb_NumLock && KEYTOGGLED_(key, 1)))
#else
#define KEYPRESSED(key) (KEYPRESSED_(key, 0) ||	KEYPRESSED_(key, 1))
#define KEYTOGGLED(key) (KEYTOGGLED_(key, 0) ||	KEYTOGGLED_(key, 1))
#endif

#define SET_BUTTON_ON_KEYPRESS(btn, key) \
	do { \
		if(KEYPRESSED(key)) \
			_pst_Curr->ac_Button[btn] = 0xff; \
	} while(0)

#define TOGGLE_BUTTON_ON_KEYPRESS(btn, key) \
	do { \
		if(KEYTOGGLED(key)) \
			_pst_Curr->ac_Button[btn] = _pst_Curr->ac_Button[btn] ? 0 : 0xff; \
	} while(0)

#define SET_BUTTON_ON_RAW_KEYPRESS(btn, key) \
	do { \
		if(_pb_KeyboardState[key]) \
			_pst_Curr->ac_Button[btn] = 0xff; \
	} while(0)

BOOL pcINO_b_Joystick_IsValid(void)
{
	return TRUE;
}

#ifdef _DEBUG
static void LogDxError(HRESULT hr, const char* file, int line)
{
	char buf[256], buf2[512];
	unsigned fm = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr, 0, buf, sizeof(buf), 0);
	sprintf(buf2, "%s(%d): DirectX error 0x%08x: %s",
		file, line, hr, fm ? buf : "<no message available>\n");
	OutputDebugString(buf2);
}

static bool DxFailed(HRESULT hr, const char* file, int line)
{
	if(FAILED(hr))
	{
		LogDxError(hr, file, line);
		return true;
	}
	else
		return false;
}

#define DX_FAILED(hr) DxFailed((hr), __FILE__, __LINE__)

static bool DxSucceeded(HRESULT hr, const char* file, int line)
{
	if(FAILED(hr))
	{
		LogDxError(hr, file, line);
		return false;
	}
	else
		return true;
}

#define DX_SUCCEEDED(hr) DxSucceeded((hr), __FILE__, __LINE__)

#else

#define DX_FAILED(hr)		FAILED((hr))
#define DX_SUCCEEDED(hr)	SUCCEEDED((hr))

#endif

__inline HRESULT IDirectInputDevice8_SetDwordProperty(IDirectInputDevice8* pDevice,
													  DWORD obj,
													  DWORD how,
													  REFGUID rguidProp,
													  DWORD dwData)
{
	DIPROPDWORD dipdw =
	{
		// the header
		{
			sizeof(DIPROPDWORD), sizeof(DIPROPHEADER), obj, how
		},

		// the data
		dwData
	};

	return IDirectInputDevice8_SetProperty(pDevice, rguidProp, &dipdw.diph);
}

__inline HRESULT IDirectInputDevice8_GetDwordProperty(IDirectInputDevice8* pDevice,
													  DWORD obj,
													  DWORD how,
													  REFGUID rguidProp,
													  DWORD* dwData)
{
	DIPROPDWORD dipdw =
	{
		// the header
		{
			sizeof(DIPROPDWORD), sizeof(DIPROPHEADER), obj, how
		}
	};

	HRESULT hr = IDirectInputDevice8_GetProperty(pDevice, rguidProp, &dipdw.diph);
	*dwData = dipdw.dwData;
	return hr;
}

static __inline float Clampf(float x, float min, float max)
{
	return x < min ? min : x > max ? max : x;
}

static void Trace(const char* s, ...)
{
	va_list va;
	char buf[256];
	va_start(va, s);
	vsprintf(buf, s, va);
	OutputDebugString(buf);
	va_end(va);
}

void pcINO_Joystick_Init(HWND hWnd)
{
	HKEY hkey;

	ERR_X_Assert(pcINO_gp_DirectInput == 0);
	ERR_X_Assert(pcINO_gp_Mouse == 0);

	pcINO_hwndWindow = hWnd;

	if(DX_FAILED(
		DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, &IID_IDirectInput8, &pcINO_gp_DirectInput, 0)))
	{
		return;
	}

	ERR_X_Assert(pcINO_gp_DirectInput != 0);

	if(DX_SUCCEEDED(
		IDirectInput8_CreateDevice(pcINO_gp_DirectInput, &GUID_SysMouse, &pcINO_gp_Mouse, 0)))
	{
		ERR_X_Assert(pcINO_gp_Mouse != 0);

		if(	DX_FAILED(IDirectInputDevice8_SetDataFormat(pcINO_gp_Mouse, &c_dfDIMouse))
		||	DX_FAILED(IDirectInputDevice8_SetCooperativeLevel(pcINO_gp_Mouse, hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
		{
			IDirectInput8_Release(pcINO_gp_Mouse);
			pcINO_gp_Mouse = 0;
		}

		if(DX_FAILED(IDirectInputDevice8_GetDwordProperty(
			pcINO_gp_Mouse, DIMOFS_Z, DIPH_BYOFFSET, DIPROP_GRANULARITY, (DWORD*)&pcINO_gui_Mouse_Z_Granularity)))
		{
			pcINO_gui_Mouse_Z_Granularity = 1; // avoid division by 0
		}
	}

	if(DX_SUCCEEDED(
		IDirectInput8_CreateDevice(pcINO_gp_DirectInput, &GUID_SysKeyboard, &pcINO_gp_Keyboard, 0)))
	{
		ERR_X_Assert(pcINO_gp_Keyboard != 0);

		if( DX_FAILED(IDirectInputDevice8_SetDataFormat(pcINO_gp_Keyboard, &c_dfDIKeyboard))
		||	DX_FAILED(IDirectInputDevice8_SetCooperativeLevel(pcINO_gp_Keyboard, hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY)))
		{
			IDirectInput8_Release(pcINO_gp_Keyboard);
			pcINO_gp_Keyboard = 0;
		}
	}

	INO_l_Joystick_Enable = 1;
	INO_gi_CurrentPadId = 0;

	// read mappings from the registry
	if(RegOpenKeyEx(HKEY_CURRENT_USER, pcINO_gs_RegKeyName, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		int i;
		DWORD type, size, data;
		for(i = 0; i < INO_NUM_KEYS; ++i)
		{
			size = sizeof(data);
			if(RegQueryValueEx(hkey, pcINO_gas_KeyNames[i], 0, &type, (BYTE*)&data, &size) == ERROR_SUCCESS
				&& type == REG_DWORD && size == sizeof(data))
			{
#ifdef _DEBUG
				if(pcINO_gai_KeyMappings[i][1] != (data & 0xff))
					Trace("Key mapping A for '%s' changed from %d to %d\n", pcINO_gas_KeyNames[i], pcINO_gai_KeyMappings[i][1], data & 0xff);
				if(pcINO_gai_KeyMappings[i][0] != ((data >> 16) & 0xff))
					Trace("Key mapping B for '%s' changed from %d to %d\n", pcINO_gas_KeyNames[i], pcINO_gai_KeyMappings[i][0], (data >> 16) & 0xff);
#endif

				pcINO_gai_KeyMappings[i][1] = data & 0xff;
				pcINO_gai_KeyMappings[i][0] = (data >> 16) & 0xff;
			}
		}
		RegCloseKey(hkey);
	}

	// provide a reasonable default in case pcINO_Joystick_Update is called before pcINO_SetControlMode
	pcINO_SetControlMode(IO_MODE_INVENTORY);
	pcINO_SetControlSubmode(IO_SUBMODE_INVENTORY_KEYS);
}

void pcINO_Joystick_Close(void)
{
	if(pcINO_gp_Mouse != 0)
	{
		IDirectInputDevice8_Release(pcINO_gp_Mouse);
		pcINO_gp_Mouse = 0;
	}

	if(pcINO_gp_Keyboard != 0)
	{
		IDirectInputDevice8_Release(pcINO_gp_Keyboard);
		pcINO_gp_Keyboard = 0;
	}

	if(pcINO_gp_DirectInput != 0)
	{
		IDirectInput8_Release(pcINO_gp_DirectInput);
		pcINO_gp_DirectInput = 0;
	}

	INO_l_Joystick_Enable = 0;
}

void pcINO_Joystick_Acquire(void)
{
	if(pcINO_gp_Mouse != 0)
		IDirectInputDevice8_Acquire(pcINO_gp_Mouse);

	if(pcINO_gp_Keyboard != 0)
		IDirectInputDevice8_Acquire(pcINO_gp_Keyboard);

	pcINO_gb_NumLock = GetKeyState(VK_NUMLOCK) != 0;
	ZeroMemory(pcINO_gab_LastKeyboardState, sizeof(pcINO_gab_LastKeyboardState));
}

void pcINO_Joystick_Unacquire(void)
{
	if(pcINO_gp_Mouse != 0)
		IDirectInputDevice8_Unacquire(pcINO_gp_Mouse);

	if(pcINO_gp_Keyboard != 0)
		IDirectInputDevice8_Unacquire(pcINO_gp_Keyboard);
}

void pcINO_Joystick_Move(MATH_tdst_Vector* _pst_Move, int _i_Num)
{
	*_pst_Move = pcINO_gast_Pad[_i_Num];
}

void pcINO_Joystick_Update(INO_tdst_GenericPad* _pst_Curr)
{
	int i;
	DIMOUSESTATE st_MouseState;
	unsigned char ab_KeyboardState[256];
	unsigned char* _pb_KeyboardState = ab_KeyboardState; // for SET_BUTTON_XXX macros

	// clear all generic buttons except L1 (crouch) which is a toggle
	for(i = 0; i < eBtn_GenericButtonNb; ++i)
	{
		if(i != eBtn_L1)
			_pst_Curr->ac_Button[i] = 0;
	}

	// read mouse state
	if(pcINO_gp_Mouse != 0)
	{
		HRESULT hr = IDirectInputDevice8_GetDeviceState(
			pcINO_gp_Mouse, sizeof(st_MouseState), &st_MouseState);

		if(hr == DIERR_INPUTLOST)
		{
			hr = IDirectInputDevice8_Acquire(pcINO_gp_Mouse);
			if(SUCCEEDED(hr))
			{
				Trace("Mouse input lost & re-acquired\n");
				hr = IDirectInputDevice8_GetDeviceState(
					pcINO_gp_Mouse, sizeof(st_MouseState), &st_MouseState);
			}
			else
				Trace("Mouse input lost but acquire fails\n");
		}

		if(FAILED(hr))
		{
			// Panic! Assume mouse hasn't moved at all
			ZeroMemory(&st_MouseState, sizeof(st_MouseState));
		}
	}
	else
	{
		// we have no mouse at all (is this possible?)
		ZeroMemory(&st_MouseState, sizeof(st_MouseState));
	}


	// read keyboard state
	if(pcINO_gp_Keyboard != 0)
	{
		HRESULT hr = IDirectInputDevice8_GetDeviceState(
			pcINO_gp_Keyboard, sizeof(ab_KeyboardState), ab_KeyboardState);

		if(hr == DIERR_INPUTLOST)
		{
			hr = IDirectInputDevice8_Acquire(pcINO_gp_Keyboard);
			if(SUCCEEDED(hr))
			{
				Trace("Keyboard input lost & re-acquired\n");
				hr = IDirectInputDevice8_GetDeviceState(
					pcINO_gp_Keyboard, sizeof(ab_KeyboardState), ab_KeyboardState);
			}
			else
				Trace("Keyboard input lost but acquire fails\n");
		}

		if(FAILED(hr))
		{
			// Panic! Assume no key is being pressed
			ZeroMemory(ab_KeyboardState, sizeof(ab_KeyboardState));
		}
	}
	else
	{
		// we have no keyboard at all (is this possible?)
		ZeroMemory(ab_KeyboardState, sizeof(ab_KeyboardState));
	}

	// set "virtual" keys
	ab_KeyboardState[0]					= 0; // paranoid, should be 0 anyway
	ab_KeyboardState[INO_DIK_LMOUSE]	= st_MouseState.rgbButtons[0] ? 0x80 : 0;
	ab_KeyboardState[INO_DIK_RMOUSE]	= st_MouseState.rgbButtons[1] ? 0x80 : 0;
	ab_KeyboardState[INO_DIK_CMOUSE]	= st_MouseState.rgbButtons[2] ? 0x80 : 0;
	ab_KeyboardState[INO_DIK_WHEELUP]	= st_MouseState.lZ > 0 ? 0x80 : 0;
	ab_KeyboardState[INO_DIK_WHEELDOWN] = st_MouseState.lZ < 0 ? 0x80 : 0;

	// call the correct update depending on context
	pcINO_gpf_JoystickUpdate(_pst_Curr, &st_MouseState, ab_KeyboardState);

	// keys common to all modes
	SET_BUTTON_ON_KEYPRESS(eBtn_Start,	INO_KEY_MENU);
	SET_BUTTON_ON_KEYPRESS(eBtn_Select,	INO_KEY_MAP);

#ifdef INO_JOYSTICK_USE_CHEATS
	// D-Pad and all keys (for cheats only!)
	if(ab_KeyboardState[DIK_NUMLOCK] && !pcINO_gab_LastKeyboardState[DIK_NUMLOCK])
	{
		pcINO_gb_NumLock = !pcINO_gb_NumLock;
		IDirectInputDevice8_Unacquire(pcINO_gp_Keyboard);
		keybd_event(VK_NUMLOCK, 0, KEYEVENTF_EXTENDEDKEY, 0);
		keybd_event(VK_NUMLOCK, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		IDirectInputDevice8_Acquire(pcINO_gp_Keyboard);
	}

	if(pcINO_gb_NumLock == 0)
	{
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Left,		DIK_LEFT);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Right,		DIK_RIGHT);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Up,			DIK_UP);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Down,		DIK_DOWN);

		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_L1,			DIK_NUMPAD1);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_L2,			DIK_NUMPAD4);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_L3,			DIK_NUMPAD7);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_R1,			DIK_NUMPAD3);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_R2,			DIK_NUMPAD6);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_R3,			DIK_NUMPAD9);

		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Cross,		DIK_NUMPADPLUS);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Circle,		DIK_NUMPADSTAR);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Square,		DIK_NUMPADMINUS);
		SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Triangle,	DIK_NUMPADSLASH);
	}
#endif // INO_JOYSTICK_USE_CHEATS

#ifdef ACTIVE_EDITORS
	if(ab_KeyboardState[DIK_F5])
	{
		// As we have exclusive control of the keyboard, the editor is not getting keypresses.
		// Unacquire the keyboard and emulate an F5 keypress so that the editor get it
		IDirectInputDevice8_Unacquire(pcINO_gp_Keyboard);
		keybd_event(VK_F5, 0, KEYEVENTF_EXTENDEDKEY, 0);
		keybd_event(VK_F5, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	}
#else
	if((ab_KeyboardState[DIK_LALT] || ab_KeyboardState[DIK_RALT]) && ab_KeyboardState[DIK_F4])
	{
		// As we have exclusive control of the keyboard, the system is not getting
		// the ALT-F4 sequence. Unacquire the keyboard and emulate the sequence
		IDirectInputDevice8_Unacquire(pcINO_gp_Keyboard);
		keybd_event(VK_LMENU, 0, KEYEVENTF_EXTENDEDKEY, 0);
		keybd_event(VK_F4, 0, KEYEVENTF_EXTENDEDKEY, 0);
		keybd_event(VK_F4, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		keybd_event(VK_LMENU, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	}
#endif

	if(ab_KeyboardState[DIK_F12] && !pcINO_gab_LastKeyboardState[DIK_F12])
	{
		IDirectInputDevice8_Unacquire(pcINO_gp_Keyboard);
		DX9_ShowConsole(pcINO_hwndWindow);
		IDirectInputDevice8_Acquire(pcINO_gp_Keyboard);
	}

	CopyMemory(pcINO_gab_LastKeyboardState, ab_KeyboardState, sizeof(pcINO_gab_LastKeyboardState));
}

static __inline bool BoolXor(int x, int y)
{
	return (x && !y) || (!x && y);
}

static float pcINO_gf_VerticalMouseAutoRestTimer;
static float pcINO_gf_VerticalMouseStateDelayTimer;
static int pcINO_gi_VerticalMouseState = 1;
static int pcINO_gi_VerticalMouseStateTransition;

static void pcINO_VerticalMouseRest()
{
	if(TIM_f_Clock_Read() > pcINO_gf_VerticalMouseAutoRestTimer)
	{
		pcINO_gast_Pad[1].y = 0;
		//pcINO_gi_VerticalMouseState = 1;
		pcINO_gi_VerticalMouseStateTransition = 0;
		//Trace("State changed to 1 because of autorest\n");
	}
}

static void pcINO_VerticalMouseCompute(float deltaY)
{
	if(pcINO_gi_VerticalMouseStateTransition)
	{
		if(TIM_f_Clock_Read() > pcINO_gf_VerticalMouseStateDelayTimer)
		{
			pcINO_gi_VerticalMouseState += pcINO_gi_VerticalMouseStateTransition;
			pcINO_gi_VerticalMouseStateTransition = 0;
			//Trace("State changed to %d\n", pcINO_gi_VerticalMouseState);
		}
	}
	else
	{
		if(pcINO_gi_VerticalMouseState < 0)
		{
			pcINO_gast_Pad[1].y = Clampf(pcINO_gast_Pad[1].y + deltaY, -1.0f, 0.5f);
			if(pcINO_gast_Pad[1].y > 0.0f)
			{
				pcINO_gi_VerticalMouseStateTransition = 2;
				pcINO_gf_VerticalMouseStateDelayTimer = TIM_f_Clock_Read() + pcINO_C_VERTICAL_MOUSE_STATE_DELAY;
				//Trace("Requested state change to 1\n");
			}
		}
		else
		{
			pcINO_gast_Pad[1].y = Clampf(pcINO_gast_Pad[1].y + deltaY, -0.5f, 1.0f);
			if(pcINO_gast_Pad[1].y < 0.0f)
			{
				pcINO_gi_VerticalMouseStateTransition = -2;
				pcINO_gf_VerticalMouseStateDelayTimer = TIM_f_Clock_Read() + pcINO_C_VERTICAL_MOUSE_STATE_DELAY;
				//Trace("Requested state change to -1\n");
			}
		}

		pcINO_gf_VerticalMouseAutoRestTimer = TIM_f_Clock_Read() + pcINO_C_VERTICAL_MOUSE_AUTOREST_DELAY;
	}
}

void pcINO_Joystick_Update_Normal(INO_tdst_GenericPad* _pst_Curr,
								DIMOUSESTATE* _pst_MouseState,
								unsigned char* _pb_KeyboardState)
{
	float norm;

	SET_BUTTON_ON_KEYPRESS(eBtn_Cross,		INO_KEY_PRIMARY_ACTION);
	SET_BUTTON_ON_KEYPRESS(eBtn_R3,			INO_KEY_CENTER_VIEW);
	SET_BUTTON_ON_KEYPRESS(eBtn_Left,		INO_KEY_INVENTORY_DOWN);
	SET_BUTTON_ON_KEYPRESS(eBtn_Right,		INO_KEY_INVENTORY_UP);
	SET_BUTTON_ON_KEYPRESS(eBtn_Circle,		INO_KEY_USE_OBJECT);
	SET_BUTTON_ON_KEYPRESS(eBtn_Square,		INO_KEY_JUMP);
	SET_BUTTON_ON_KEYPRESS(eBtn_Triangle,	INO_KEY_SUPER_ACTION);
	SET_BUTTON_ON_KEYPRESS(eBtn_R1,			INO_KEY_LOOK_MODE);
	TOGGLE_BUTTON_ON_KEYPRESS(eBtn_L1,		INO_KEY_CROUCH);
	SET_BUTTON_ON_RAW_KEYPRESS(eBtn_R3,		INO_DIK_CMOUSE);
	SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Right,	INO_DIK_WHEELUP);
	SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Left,	INO_DIK_WHEELDOWN);

	// ASWD keys mapped to left stick
	pcINO_gast_Pad[0].x = 0;
	if(KEYPRESSED(INO_KEY_RIGHT))
		pcINO_gast_Pad[0].x += 1.0f;
	if(KEYPRESSED(INO_KEY_LEFT))
		pcINO_gast_Pad[0].x -= 1.0f;

	pcINO_gast_Pad[0].y = 0;
	if(KEYPRESSED(INO_KEY_UP))
		pcINO_gast_Pad[0].y += 1.0f;
	if(KEYPRESSED(INO_KEY_DOWN))
		pcINO_gast_Pad[0].y -= 1.0f;


	if(pcINO_gast_Pad[0].x > 0)
	{
		// Jade is moving forward, the camera switches to chasecam, the mouse input is used
		// to make the Jade turn left/right (the y axis is ignored in this case)
		pcINO_gast_Pad[0].x = Clampf(pcINO_gast_Pad[0].x + (float)_pst_MouseState->lX * 0.05f, -1.0f, 1.0f);
	}

	// auto-rest the Y axis of the mouse, which is state-dependent
	pcINO_VerticalMouseRest();

	// keep only the axis with biggest variation 
	if(abs(_pst_MouseState->lX) >= abs(_pst_MouseState->lY))
	{
		// consider x axis only
		pcINO_gast_Pad[1].x = Clampf((float)_pst_MouseState->lX * -0.03f, -1.0f, 1.0f);
	}
	else
	{
		// consider y axis only, which is state-dependent
		pcINO_VerticalMouseCompute((float)_pst_MouseState->lY * -0.003f);
	}

	norm = (float)hypot(pcINO_gast_Pad[0].x, pcINO_gast_Pad[0].y);
	if(norm)
	{
		// use mouse right button to decide the walk speed (in crouch, the speed control is reversed)
		float walkSpeed = BoolXor(KEYPRESSED(INO_KEY_RUN), _pst_Curr->ac_Button[eBtn_L1]) ? 1.0f : 0.8f;
		pcINO_gast_Pad[0].x *= walkSpeed / norm;
		pcINO_gast_Pad[0].y *= walkSpeed / norm;
	}
}

static __inline float SignedSquareRoot(float x)
{
	if(x > 0)
		return (float)sqrt(x);
	else
		return (float)-sqrt(-x);
}

static __inline float StripInertia(float* x)
{
	if(*x < -1.0f)
	{
		*x /= 2.0f;
		return -1.0f;
	}
	else if(*x > 1.0f)
	{
		*x /= 2.0f;
		return 1.0f;
	}
	else
	{
		float t = *x;
		*x = 0.0f;
		return t;
	}		 
}

void pcINO_Joystick_Update_Look(INO_tdst_GenericPad* _pst_Curr,
								DIMOUSESTATE* _pst_MouseState,
								unsigned char* _pb_KeyboardState)
{
	static int si_Wheel;

	SET_BUTTON_ON_KEYPRESS(eBtn_Cross,		INO_KEY_PRIMARY_ACTION);
	SET_BUTTON_ON_KEYPRESS(eBtn_R1,			INO_KEY_LOOK_MODE);

	pcINO_ast_PadPosForInertia.x += SignedSquareRoot((float)_pst_MouseState->lX * 0.03f);
	pcINO_ast_PadPosForInertia.y += SignedSquareRoot((float)_pst_MouseState->lY * 0.03f);

	pcINO_gast_Pad[0].x = StripInertia(&pcINO_ast_PadPosForInertia.x);
	pcINO_gast_Pad[0].y = StripInertia(&pcINO_ast_PadPosForInertia.y);

	pcINO_gast_Pad[1].x = 0;
	pcINO_gast_Pad[1].y = 0;

	if(_pst_MouseState->lZ)
	{
		si_Wheel += _pst_MouseState->lZ;
		pcINO_gf_KeyRepeatDelayTimer = TIM_f_Clock_Read() + pcINO_C_WHEEL_PERSISTANCE;
	}

	if(KEYPRESSED(INO_KEY_UP) || si_Wheel > 0)
		pcINO_gast_Pad[1].y += 1.0f;
	if(KEYPRESSED(INO_KEY_DOWN) || si_Wheel < 0)
		pcINO_gast_Pad[1].y -= 1.0f;

	if(si_Wheel && TIM_f_Clock_Read() > pcINO_gf_KeyRepeatDelayTimer)
	{
		if(si_Wheel > 0)
			si_Wheel -= pcINO_gui_Mouse_Z_Granularity;
		else
			si_Wheel += pcINO_gui_Mouse_Z_Granularity;
	}
}

void pcINO_Joystick_Update_Hovercraft(INO_tdst_GenericPad* _pst_Curr,
									  DIMOUSESTATE* _pst_MouseState,
									  unsigned char* _pb_KeyboardState)
{

	SET_BUTTON_ON_KEYPRESS(eBtn_Cross,		INO_KEY_PRIMARY_ACTION);
	SET_BUTTON_ON_KEYPRESS(eBtn_R3,			INO_KEY_CENTER_VIEW);
	SET_BUTTON_ON_KEYPRESS(eBtn_Left,		INO_KEY_INVENTORY_DOWN);
	SET_BUTTON_ON_KEYPRESS(eBtn_Right,		INO_KEY_INVENTORY_UP);
	SET_BUTTON_ON_KEYPRESS(eBtn_Circle,		INO_KEY_USE_OBJECT);
	SET_BUTTON_ON_KEYPRESS(eBtn_Square,		INO_KEY_JUMP);
	SET_BUTTON_ON_KEYPRESS(eBtn_Triangle,	INO_KEY_SUPER_ACTION);
	SET_BUTTON_ON_KEYPRESS(eBtn_R1,			INO_KEY_LOOK_MODE);
	SET_BUTTON_ON_KEYPRESS(eBtn_R2,			INO_KEY_RUN);
	SET_BUTTON_ON_RAW_KEYPRESS(eBtn_R3,		INO_DIK_CMOUSE);
	SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Right,	INO_DIK_WHEELUP);
	SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Left,	INO_DIK_WHEELDOWN);

	pcINO_gast_Pad[0].x = 0;
	if(KEYPRESSED(INO_KEY_RIGHT))
		pcINO_gast_Pad[0].x += 1.0f;
	if(KEYPRESSED(INO_KEY_LEFT))
		pcINO_gast_Pad[0].x -= 1.0f;

	if(pcINO_gast_Pad[0].x == 0) // ignore UP and DOWN when turning
	{
		pcINO_gast_Pad[0].y = 0;
		if(KEYPRESSED(INO_KEY_UP))
			pcINO_gast_Pad[0].y += 1.0f;
		if(KEYPRESSED(INO_KEY_DOWN))
			pcINO_gast_Pad[0].y -= 1.0f;
	}

	if(pcINO_gast_Pad[0].x && pcINO_gast_Pad[0].y)
	{
		pcINO_gast_Pad[0].x *= 0.707106f; // 1/sqrt(2)
		pcINO_gast_Pad[0].y *= 0.707106f; // 1/sqrt(2)
	}

	// auto-rest the Y axis of the mouse, which is state-dependent
	pcINO_VerticalMouseRest();

	if(pcINO_gi_ControlSubmode == IO_SUBMODE_AIRCRAFT_LOOK)
	{
		// we are in first-person (compass is displayed)
		// the mouse input is used to make the hovercraft turn left/right
		pcINO_gast_Pad[0].x = Clampf(pcINO_gast_Pad[0].x + (float)_pst_MouseState->lX * 0.02f, -1.0f, 1.0f);
	}
	else if(_pst_MouseState->rgbButtons[1] || pcINO_gast_Pad[0].x || pcINO_gast_Pad[0].y)
	{
		// the hovercraft is moving, the camera switches to chasecam,
		// the mouse input is used to make the hovercraft turn left/right
		pcINO_gast_Pad[0].x = Clampf(pcINO_gast_Pad[0].x + (float)_pst_MouseState->lX * 0.03f, -1.0f, 1.0f);
	}
	else
	{
		// keep only the axis with biggest variation in this frame
		if(abs(_pst_MouseState->lX) >= abs(_pst_MouseState->lY))
		{
			// consider x axis only
			pcINO_gast_Pad[1].x = Clampf((float)_pst_MouseState->lX * -0.01f, -1.0f, 1.0f);
		}
		else
		{
			// consider y axis only, which is state-dependent
			pcINO_VerticalMouseCompute((float)_pst_MouseState->lY * -0.003f);
		}
	}
}

void pcINO_Joystick_Update_Spaceship(INO_tdst_GenericPad* _pst_Curr,
									 DIMOUSESTATE* _pst_MouseState,
									 unsigned char* _pb_KeyboardState)
{
	SET_BUTTON_ON_KEYPRESS(eBtn_Cross,		INO_KEY_PRIMARY_ACTION);
	SET_BUTTON_ON_KEYPRESS(eBtn_R3,			INO_KEY_CENTER_VIEW);
	SET_BUTTON_ON_KEYPRESS(eBtn_Left,		INO_KEY_INVENTORY_DOWN);
	SET_BUTTON_ON_KEYPRESS(eBtn_Right,		INO_KEY_INVENTORY_UP);
	SET_BUTTON_ON_KEYPRESS(eBtn_Circle,		INO_KEY_USE_OBJECT);
	SET_BUTTON_ON_KEYPRESS(eBtn_Square,		INO_KEY_JUMP);
	SET_BUTTON_ON_KEYPRESS(eBtn_Triangle,	INO_KEY_SUPER_ACTION);
	SET_BUTTON_ON_KEYPRESS(eBtn_R1,			INO_KEY_LOOK_MODE);
	SET_BUTTON_ON_KEYPRESS(eBtn_R2,			INO_KEY_RUN);
	SET_BUTTON_ON_KEYPRESS(eBtn_R2,			INO_KEY_UP);
	SET_BUTTON_ON_RAW_KEYPRESS(eBtn_R3,		INO_DIK_CMOUSE);
	SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Right,	INO_DIK_WHEELUP);
	SET_BUTTON_ON_RAW_KEYPRESS(eBtn_Left,	INO_DIK_WHEELDOWN);

	// the mouse and the 'A' and 'D' keys only contribute on the left stick
	pcINO_gast_Pad[0].x = Clampf(
		(float)_pst_MouseState->lX * 0.05f +
		(KEYPRESSED(INO_KEY_LEFT) ? -1.0f : 0.0f) +
		(KEYPRESSED(INO_KEY_RIGHT) ? +1.0f : 0.0f),
		-1.0f, 1.0f);
	pcINO_gast_Pad[0].y = Clampf((float)_pst_MouseState->lY * 0.05f, -1.0f, 1.0f);

	// camera is a chasecam so right stick is left at 0
	pcINO_gast_Pad[1].x = 0;
	pcINO_gast_Pad[1].y = 0;
}

void pcINO_Joystick_Update_Menu(INO_tdst_GenericPad* _pst_Curr,
								DIMOUSESTATE* _pst_MouseState,
								unsigned char* _pb_KeyboardState)
{
	float norm;

	SET_BUTTON_ON_KEYPRESS(eBtn_Cross,		INO_KEY_PRIMARY_ACTION);
	SET_BUTTON_ON_KEYPRESS(eBtn_Cross,		INO_KEY_USE_OBJECT); // extra way to say 'ok'
	SET_BUTTON_ON_KEYPRESS(eBtn_Square,		INO_KEY_MENU_BACK);
	SET_BUTTON_ON_KEYPRESS(eBtn_Circle,		INO_KEY_MENU_INFO);
	SET_BUTTON_ON_KEYPRESS(eBtn_R1,			INO_KEY_MENU_INFO);	// to delete a picture
	SET_BUTTON_ON_KEYPRESS(eBtn_R2,			INO_KEY_MENU_INFO);	// to zoom a map

	switch(pcINO_gi_ControlSubmode)
	{
	case IO_SUBMODE_INVENTORY_MOUSE:
	pcINO_gast_Pad[0].x += _pst_MouseState->lX * 0.02f;
	pcINO_gast_Pad[0].y -= _pst_MouseState->lY * 0.02f;

	norm = (float)hypot(pcINO_gast_Pad[0].x, pcINO_gast_Pad[0].y);
	if(norm > 0.1f)
	{
		pcINO_gast_Pad[0].x /= norm;
		pcINO_gast_Pad[0].y /= norm;
	}
	else
	{
		// panic!
		pcINO_gast_Pad[0].x = 1.0f;
		pcINO_gast_Pad[0].y = 0.0f;
	}
		break;

	case IO_SUBMODE_INVENTORY_KEYS:
		// directional keys are mapped to both sticks
		pcINO_gast_Pad[0].x = 0;
		if(KEYPRESSED(INO_KEY_RIGHT))
			pcINO_gast_Pad[0].x += 1.0f;
		if(KEYPRESSED(INO_KEY_LEFT))
			pcINO_gast_Pad[0].x -= 1.0f;
		pcINO_gast_Pad[1].x = pcINO_gast_Pad[0].x;

		pcINO_gast_Pad[0].y = 0;
		if(KEYPRESSED(INO_KEY_UP))
			pcINO_gast_Pad[0].y += 1.0f;
		if(KEYPRESSED(INO_KEY_DOWN))
			pcINO_gast_Pad[0].y -= 1.0f;
		pcINO_gast_Pad[1].y = pcINO_gast_Pad[0].y;
		break;
	}
}

void pcINO_Joystick_Update_Dialogue(INO_tdst_GenericPad* _pst_Curr,
									DIMOUSESTATE* _pst_MouseState,
									unsigned char* _pb_KeyboardState)
{
	SET_BUTTON_ON_KEYPRESS(eBtn_Cross,		INO_KEY_PRIMARY_ACTION);
	SET_BUTTON_ON_KEYPRESS(eBtn_Square,		INO_KEY_MENU_BACK);

	pcINO_gast_Pad[0].x = 0.0f;
	pcINO_gast_Pad[0].y = 0.0f;

	if(TIM_f_Clock_Read() > pcINO_gf_KeyRepeatDelayTimer)
	{
		// up-down are used in "real" dialogues
		if(KEYPRESSED(INO_KEY_UP))
		{
			pcINO_gast_Pad[0].y = 1.0f;
			pcINO_gf_KeyRepeatDelayTimer = TIM_f_Clock_Read() + pcINO_C_KEY_REPEAT_DELAY;
		}
		else if(KEYPRESSED(INO_KEY_DOWN))
		{
			pcINO_gast_Pad[0].y = -1.0f;
			pcINO_gf_KeyRepeatDelayTimer = TIM_f_Clock_Read() + pcINO_C_KEY_REPEAT_DELAY;
		}

		// left-right are used in some menus (for instance the disk selection menu)
		if(KEYPRESSED(INO_KEY_RIGHT))
		{
			pcINO_gast_Pad[0].x = 1.0f;
			pcINO_gf_KeyRepeatDelayTimer = TIM_f_Clock_Read() + pcINO_C_KEY_REPEAT_DELAY;
		}
		else if(KEYPRESSED(INO_KEY_LEFT))
		{
			pcINO_gast_Pad[0].x = -1.0f;
			pcINO_gf_KeyRepeatDelayTimer = TIM_f_Clock_Read() + pcINO_C_KEY_REPEAT_DELAY;
		}
	}
}

void pcINO_SetControlMode(int mode)
{
	switch(mode)
	{
	case IO_MODE_JACK:
		pcINO_gpf_JoystickUpdate = pcINO_Joystick_Update_Normal;
		break;

	case IO_MODE_KONG:
		pcINO_gpf_JoystickUpdate = pcINO_Joystick_Update_Normal;
		break;
	}
}

void pcINO_SetControlSubmode(int submode)
{
	pcINO_gi_ControlSubmode = submode;
}

void pcINO_PadHightVibSet(int _i_level)
{}

void pcINO_PadLowVibSet(BOOL _b_On)
{}

int pcINO_i_SetCurrentPad(int _i_id)
{
	INO_gi_CurrentPadId = 0;
	return (_i_id == 0) ? 0 : -1;
}


/*$4
 ***********************************************************************************************************************
    PC FOR EDITOR (via Multimedia functions)
 ***********************************************************************************************************************
 */

#else

/*$2- init -----------------------------------------------------------------------------------------------------------*/

LONG	win32INO_l_Joystick_Init = 0;
LONG	win32INO_l_Joystick_Mode = INO_Joy_Ps2Mode;
LONG	win32INO_l_Joystick_XLeft[2] = { 0, 0};
LONG	win32INO_l_Joystick_XLeftStart[2] = {22000, 22000};
LONG	win32INO_l_Joystick_XRightStart[2] = {42000,42000};
LONG	win32INO_l_Joystick_XRight[2] = {65536, 65536};
LONG	win32INO_l_Joystick_YUp[2] = {0,0};
LONG	win32INO_l_Joystick_YUpStart[2] = {22000,22000};
LONG	win32INO_l_Joystick_YDownStart[2] = {42000,42000};
LONG	win32INO_l_Joystick_YDown[2] = {65536,65536};

/* Fouziya & boujemaa debut */
UINT	win32INO_l_Joystick_ID = JOYSTICKID1;

/* Fouziya & boujemaa fin */
#ifdef ACTIVE_EDITORS
LONG	INO_l_Joystick_X[2];
LONG	INO_l_Joystick_Y[2];
#endif /* ACTIVE_EDITORS */

LONG	win32INO_l_JoyUSBPS2 = 0;

void	win32INO_GetjoyUSBPS2(void);

/* DIRECT INPUT for Vibrations */
BOOL					win32INO_gi_DirectInputEnable = 0;
IDirectInput8*			win32INO_gp_DirectInput;
LPDIRECTINPUTDEVICE8	win32INO_gp_FFDevice = NULL;
DIEFFECT				win32INO_st_EffectParam;
DWORD					win32INO_dw_EffectParam_Axes[2] = { DIJOFS_X, DIJOFS_Y };
LONG					win32INO_l_EffectParam_Direction[2] = { 18000, 0 };
DIPERIODIC				win32INO_st_EffectParam_Periodic;

void					win32INO_InitDirectInput( HWND );
void					win32INO_FreeDirectInput( void );


/*
 =======================================================================================================================
    LPDIRECTINPUT8 win32INO_glp_DI;
 =======================================================================================================================
 */
static void win32INO_Joystick_Init(HWND _hWnd)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	int		i_Error;
	char	sErrorTxt[255];
	/*~~~~~~~~~~~~~~~~~~~*/

	INO_gi_CurrentPadId = 0;
	win32INO_GetjoyUSBPS2();

	if(_hWnd == NULL) return;
	win32INO_l_Joystick_Init = 1;
	if(!joyGetNumDevs()) return;
	i_Error = joySetCapture(_hWnd, JOYSTICKID1, 0, FALSE);
	if(i_Error != JOYERR_NOERROR)
	{
		if(i_Error == JOYERR_UNPLUGGED) strcpy(sErrorTxt, "Joystick unpluged - JOYSTICKID1");
		if(i_Error == JOYERR_NOCANDO)
			strcpy(sErrorTxt, "A required driver (such as windows timer) is unavailable - JOYSTICKID1");
		if(i_Error == MMSYSERR_NODRIVER) strcpy(sErrorTxt, "Joystick driver is not present - JOYSTICKID1");

		i_Error = joySetCapture(_hWnd, JOYSTICKID2, 0, FALSE);
		if(i_Error != JOYERR_NOERROR)
		{
			if(i_Error == JOYERR_UNPLUGGED) strcpy(sErrorTxt, "Joystick unpluged - JOYSTICKID2");
			if(i_Error == JOYERR_NOCANDO)
				strcpy(sErrorTxt, "A required driver (such as windows timer) is unavailable - JOYSTICKID2");
			if(i_Error == MMSYSERR_NODRIVER) strcpy(sErrorTxt, "Joystick driver is not present - JOYSTICKID2");
			INO_l_Joystick_Enable = 0;
		}
		else
		{
			INO_l_Joystick_Enable = 1;
			win32INO_l_Joystick_ID = JOYSTICKID2;
		}
	}
	else
	{
		INO_l_Joystick_Enable = 1;
		win32INO_l_Joystick_ID = JOYSTICKID1;
	}

	//if(win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version2) 
	win32INO_gi_DirectInputEnable = 0;
	win32INO_InitDirectInput( _hWnd );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32INO_Joystick_Close(void)
{
	if(INO_l_Joystick_Enable)
	{
		joyReleaseCapture(win32INO_l_Joystick_ID);
		INO_l_Joystick_Enable = 0;
	}
	win32INO_l_Joystick_Init = 0;
	
	win32INO_FreeDirectInput();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32INO_Joystick_Update(INO_tdst_GenericPad *pCurr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	JOYINFOEX	st_Joy;
	int			i;
	LONG		win32INO_gsl_CurrentState;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	INO_gi_CurrentPadId = 0;
	win32INO_gsl_CurrentState = 0;

	st_Joy.dwSize = sizeof(JOYINFOEX);
	st_Joy.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNZ | JOY_RETURNR | JOY_RETURNX | JOY_RETURNY;
	joyGetPosEx(win32INO_l_Joystick_ID, &st_Joy);


    if (INO_b_RecordInput)
    {
        if (INO_n_FrameCounter == 0)   
		{
			/* Create file at the same pos as exe */
			sprintf(INO_dumpFileName,INPUT_REC_FILE,WOR_gul_WorldKey);
			if(!(L_access(INO_dumpFileName, 0))) L_chmod(INO_dumpFileName, L_S_IREAD | L_S_IWRITE);

			INO_dumpFile = L_fopen(INO_dumpFileName, L_fopen_WB);            
			L_fwriteA(&INO_n_FrameCounter, sizeof(INO_n_FrameCounter), 1, INO_dumpFile);   
		}
        
        L_fwriteA((void *) &st_Joy, sizeof(JOYINFOEX), 1, INO_dumpFile);    
        INO_n_NumRecord = INO_n_FrameCounter;         

        st_CurrentFrameJoy = st_Joy;  
    }
    else if (INO_b_PlayInput)
    {                
        if (INO_n_FrameCounter == 0)    
        {            
			sprintf(INO_dumpFileName,"X:\\INPUT_REC_%08X.ino",WOR_gul_WorldKey);
			if(!(L_access(INO_dumpFileName, 0))) L_chmod(INO_dumpFileName, L_S_IREAD | L_S_IWRITE);

            INO_dumpFile = L_fopen(INO_dumpFileName, L_fopen_RB);       
            L_freadA(&INO_n_NumRecord, sizeof(INO_n_NumRecord), 1, INO_dumpFile);  
        }
        
        L_freadA((void *) &st_Joy, sizeof(JOYINFOEX), 1, INO_dumpFile);    
        st_CurrentFrameJoy = st_Joy;        

        if (INO_n_FrameCounter >= INO_n_NumRecord)   
            INO_b_PlayInput = FALSE;
    }
    else if (INO_n_FrameCounter != 0)
    {
		if (INO_dumpFile != NULL)
		{
			// going to the begining of the file
			fseek(INO_dumpFile,0,0);
			// overwriting number of frames
			L_fwriteA(&INO_n_FrameCounter, sizeof(INO_n_FrameCounter), 1, INO_dumpFile);   
			L_fclose(INO_dumpFile);
		}
        INO_n_FrameCounter = 0;
    }

	switch(win32INO_l_Joystick_Mode)
	{
	case INO_Joy_PCMode:
		win32INO_gsl_CurrentState = st_Joy.dwButtons & 0xF;
		if(!(st_Joy.dwZpos & 0x8000)) win32INO_gsl_CurrentState |= 0x10;
		if(!(st_Joy.dwRpos & 0x8000)) win32INO_gsl_CurrentState |= 0x20;
		break;
	case INO_Joy_XenonMode:
		if (st_Joy.dwButtons &    1) win32INO_gsl_CurrentState |= (1 << eBtn_Cross);
        if (ControlMode != 2)
        {
            if (st_Joy.dwButtons &    2) win32INO_gsl_CurrentState |= (1 << eBtn_Circle);
            if (st_Joy.dwButtons &    8) win32INO_gsl_CurrentState |= (1 << eBtn_Triangle);
        }
        else
        {
            if (st_Joy.dwButtons &    2) win32INO_gsl_CurrentState |= (1 << eBtn_Triangle);
            if (st_Joy.dwButtons &    8) win32INO_gsl_CurrentState |= (1 << eBtn_Circle);
        }
		if (st_Joy.dwButtons &    4) win32INO_gsl_CurrentState |= (1 << eBtn_Square);
		//if (st_Joy.dwButtons &    8) win32INO_gsl_CurrentState |= (1 << eBtn_Triangle);
		if (st_Joy.dwButtons &   32) win32INO_gsl_CurrentState |= (1 << eBtn_L1);
		if (st_Joy.dwButtons &   16) win32INO_gsl_CurrentState |= (1 << eBtn_R1);
		if (st_Joy.dwButtons & 1024) win32INO_gsl_CurrentState |= (1 << eBtn_L2);
		if (st_Joy.dwButtons & 2048) win32INO_gsl_CurrentState |= (1 << eBtn_R2);
		if (st_Joy.dwButtons &  128) win32INO_gsl_CurrentState |= (1 << eBtn_Select);
		if (st_Joy.dwButtons &   64) win32INO_gsl_CurrentState |= (1 << eBtn_Start);
		if (st_Joy.dwButtons &  512) win32INO_gsl_CurrentState |= (1 << eBtn_R3);
		if (st_Joy.dwButtons &  256) win32INO_gsl_CurrentState |= (1 << eBtn_L3);
		if (st_Joy.dwPOV   ==     0) win32INO_gsl_CurrentState |= (1 << eBtn_Up);
		if (st_Joy.dwPOV   ==  9000) win32INO_gsl_CurrentState |= (1 << eBtn_Right);
		if (st_Joy.dwPOV   == 18000) win32INO_gsl_CurrentState |= (1 << eBtn_Down);
		if (st_Joy.dwPOV   == 27000) win32INO_gsl_CurrentState |= (1 << eBtn_Left);
		break;

	case INO_Joy_Ps2Mode:
		win32INO_gsl_CurrentState = 0;
		if(st_Joy.dwButtons & 1) win32INO_gsl_CurrentState |= 8;
		if(st_Joy.dwButtons & 2) win32INO_gsl_CurrentState |= 2;
		if(st_Joy.dwButtons & 4) win32INO_gsl_CurrentState |= 1;
		if(st_Joy.dwButtons & 8) win32INO_gsl_CurrentState |= 4;
		if(st_Joy.dwButtons & 0x40) win32INO_gsl_CurrentState |= 0x10;
		if(st_Joy.dwButtons & 0x80) win32INO_gsl_CurrentState |= 0x20;
		if(st_Joy.dwButtons & 0x10) win32INO_gsl_CurrentState |= 0x40;
		if(st_Joy.dwButtons & 0x20) win32INO_gsl_CurrentState |= 0x80;
		win32INO_gsl_CurrentState += st_Joy.dwButtons & 0xFFFFFF00;

		if(win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version1)
		{
			win32INO_gsl_CurrentState &= 0XFFFFF5FF;
			win32INO_gsl_CurrentState |= (st_Joy.dwButtons & 0x200) << 2;
			win32INO_gsl_CurrentState |= (st_Joy.dwButtons & 0x800) >> 2;
		}
		else if(win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version2)
		{
			win32INO_gsl_CurrentState &= 0XFFFFF3FF;
			win32INO_gsl_CurrentState |= (st_Joy.dwButtons & 0x400) << 1;
			win32INO_gsl_CurrentState |= (st_Joy.dwButtons & 0x800) >> 1;
		}
		else if (win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version3 )
		{
			win32INO_gsl_CurrentState &= 0XFFFFF5FF;
			win32INO_gsl_CurrentState |= (st_Joy.dwButtons & 0x200) << 2;
			win32INO_gsl_CurrentState |= (st_Joy.dwButtons & 0x800) >> 2;
			
			if (st_Joy.dwPOV != 0xFFFF)
			{
				if ( (st_Joy.dwPOV < 5000) || (st_Joy.dwPOV > 30000) )
					win32INO_gsl_CurrentState |= 0x1000;
				if ( (st_Joy.dwPOV > 4000) && (st_Joy.dwPOV < 14000) )
					win32INO_gsl_CurrentState |= 0x2000;
				if ( (st_Joy.dwPOV > 13000) && (st_Joy.dwPOV < 23000) )
					win32INO_gsl_CurrentState |= 0x4000;
				if ( (st_Joy.dwPOV > 22000) && (st_Joy.dwPOV < 32000) )
					win32INO_gsl_CurrentState |= 0x8000;
			}
		}
		else if (win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version4 )
		{
			win32INO_gsl_CurrentState &= 0XFFFFF3FF;
			win32INO_gsl_CurrentState |= (st_Joy.dwButtons & 0x400) << 1;
			win32INO_gsl_CurrentState |= (st_Joy.dwButtons & 0x800) >> 1;
		}
		break;
	}

#ifdef ACTIVE_EDITORS
	INO_l_Joystick_X[0] = st_Joy.dwXpos;
	INO_l_Joystick_Y[0] = st_Joy.dwYpos;
	if ( (win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version2) || (win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version3) || (win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version4) )
	{
		INO_l_Joystick_X[1] = st_Joy.dwRpos;
		INO_l_Joystick_Y[1] = st_Joy.dwZpos;
	}
	else
	{
		INO_l_Joystick_X[1] = st_Joy.dwZpos;
		INO_l_Joystick_Y[1] = st_Joy.dwRpos;
	}

	if (win32INO_l_Joystick_Mode == INO_Joy_XenonMode)
	{
		INO_l_Joystick_X[1] = st_Joy.dwUpos;
		INO_l_Joystick_Y[1] = st_Joy.dwRpos;
	}

#endif

	for(i = 0; i <= eBtn_GenericButtonNb; i++)
	{
		pCurr[INO_gi_CurrentPadId].ac_Button[i] = (win32INO_gsl_CurrentState & (1 << i)) ? 0xFF : 0;
		if(INO_b_IsKeyPressed(INO_gauc_KeyToPad[INO_CKPT_ButtonsFirst + i]))
			pCurr[INO_gi_CurrentPadId].ac_Button[i] = 0xFF;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL win32INO_b_Joystick_IsValid(void)
{
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int win32INO_i_SetCurrentPad(int id)
{
	INO_gi_CurrentPadId = (id & 4);
	return ( ((id == 0) || (id == 4)) ? id : -1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32INO_GetjoyUSBPS2(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	HKEY	h_Key;
	char	asz_Value[10];
	LONG	l_Len;
	/*~~~~~~~~~~~~~~~~~~*/

	if(RegOpenKey(HKEY_CURRENT_USER, "JADE_Joy", &h_Key) != ERROR_SUCCESS)
	{
		win32INO_l_JoyUSBPS2 = INO_JoyUSBPS2_Version0;
		return;
	}

	l_Len = 9;
	RegQueryValue(h_Key, "USBPS2Version", asz_Value, &l_Len);
	RegCloseKey(h_Key);
	win32INO_l_JoyUSBPS2 = L_atol(asz_Value);

	if((win32INO_l_JoyUSBPS2 < INO_JoyUSBPS2_Version0) || (win32INO_l_JoyUSBPS2 > INO_JoyUSBPS2_Version4))
		win32INO_l_JoyUSBPS2 = INO_JoyUSBPS2_Version0;
}

/*
 =======================================================================================================================
 Initialize the DirectInput variables
 =======================================================================================================================
 */
BOOL CALLBACK EnumFFDevicesCallback( LPCDIDEVICEINSTANCE pDDI, VOID* pvRef )
{
	static int num = 0;
#ifdef JADEFUSION
	if( FAILED( IDirectInput8_CreateDevice( win32INO_gp_DirectInput, pDDI->guidInstance, &win32INO_gp_FFDevice, NULL ) ) )
#else
		if( FAILED( IDirectInput8_CreateDevice( win32INO_gp_DirectInput, &pDDI->guidInstance, &win32INO_gp_FFDevice, NULL ) ) )
#endif
		return DIENUM_CONTINUE; 
    //if ( ++num == 1)
	//	return DIENUM_CONTINUE; 
    return DIENUM_STOP;
}
/**/
void win32INO_InitDirectInput( HWND _hWnd )
{
    DIPROPDWORD		dipdw;

    // Setup the g_EffectsList circular linked list
    //ZeroMemory( &g_EffectsList, sizeof( EFFECTS_NODE ) );
    //g_EffectsList.pNext = &g_EffectsList;

    // Create a DInput object
#ifdef JADEFUSION
	if( FAILED( DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&win32INO_gp_DirectInput, NULL ) ) )
#else
		if( FAILED( DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, &IID_IDirectInput8, (VOID**)&win32INO_gp_DirectInput, NULL ) ) )
#endif
		return;

    // Get the first enumerated force feedback device
    if( FAILED( IDirectInput8_EnumDevices( win32INO_gp_DirectInput, 0, EnumFFDevicesCallback, 0, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK ) ) )
        return;
    
    if( win32INO_gp_FFDevice == NULL )
    {
		/* no feedback device found */
        return;
    }

    // Set the data format
#ifdef JADEFUSION
    if( FAILED( win32INO_gp_FFDevice->SetDataFormat( &c_dfDIJoystick ) ) )
#else
	if( FAILED( win32INO_gp_FFDevice->lpVtbl->SetDataFormat( win32INO_gp_FFDevice, &c_dfDIJoystick ) ) )
#endif
		return;

    // Set the coop level
#ifdef JADEFUSION
    if( FAILED( win32INO_gp_FFDevice->SetCooperativeLevel( _hWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND ) ) )
#else
	if( FAILED( win32INO_gp_FFDevice->lpVtbl->SetCooperativeLevel( win32INO_gp_FFDevice, _hWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND ) ) )
#endif
		return;

    // Disable auto-centering spring
    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = FALSE;

#ifdef JADEFUSION
    if( FAILED( win32INO_gp_FFDevice->SetProperty( DIPROP_AUTOCENTER, &dipdw.diph ) ) )
#else
	if( FAILED( win32INO_gp_FFDevice->lpVtbl->SetProperty( win32INO_gp_FFDevice, DIPROP_AUTOCENTER, &dipdw.diph ) ) )
#endif
		return;

    // Acquire the device
#ifdef JADEFUSION
	if( FAILED( win32INO_gp_FFDevice->Acquire() ) )
#else
	if( FAILED( win32INO_gp_FFDevice->lpVtbl->Acquire(win32INO_gp_FFDevice) ) )
#endif
		return;
        
    // init effect params
    L_memset( &win32INO_st_EffectParam, 0, sizeof( DIEFFECT ) );
	win32INO_st_EffectParam.dwSize					= sizeof( DIEFFECT );
	win32INO_st_EffectParam.dwFlags					= DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
	win32INO_st_EffectParam.dwDuration				= (DWORD)(0.5 * DI_SECONDS);
	win32INO_st_EffectParam.dwSamplePeriod			= 0;
	win32INO_st_EffectParam.dwGain					= DI_FFNOMINALMAX;
	win32INO_st_EffectParam.dwTriggerButton			= DIEB_NOTRIGGER;
	win32INO_st_EffectParam.dwTriggerRepeatInterval	= 0;
	win32INO_st_EffectParam.cAxes					= 2;
	win32INO_st_EffectParam.rgdwAxes				= win32INO_dw_EffectParam_Axes;
	win32INO_st_EffectParam.rglDirection			= win32INO_l_EffectParam_Direction;
	win32INO_st_EffectParam.lpEnvelope				= NULL;
	win32INO_st_EffectParam.cbTypeSpecificParams	= sizeof(DIPERIODIC);
	win32INO_st_EffectParam.lpvTypeSpecificParams	= &win32INO_st_EffectParam_Periodic;  

	win32INO_gi_DirectInputEnable = 1;
}


/*
 =======================================================================================================================
	Free the DirectInput variables
 =======================================================================================================================
 */
void win32INO_FreeDirectInput( void )
{
	if ( !win32INO_gi_DirectInputEnable  ) return;
	
    // Release any DirectInputEffect objects.
    if( win32INO_gp_FFDevice ) 
    {
#ifdef JADEFUSION
		win32INO_gp_FFDevice->Unacquire();
        win32INO_gp_FFDevice->Release();
#else
		win32INO_gp_FFDevice->lpVtbl->Unacquire( win32INO_gp_FFDevice );
        win32INO_gp_FFDevice->lpVtbl->Release(win32INO_gp_FFDevice);
#endif
		win32INO_gp_FFDevice = NULL;
    }

    // Release any DirectInput objects.
    if (win32INO_gp_DirectInput )
	{
#ifdef JADEFUSION
		win32INO_gp_DirectInput->Release();
#else
		win32INO_gp_DirectInput->lpVtbl->Release( win32INO_gp_DirectInput );
#endif
		win32INO_gp_DirectInput = NULL;
	}

    win32INO_gi_DirectInputEnable  = 0;
}



/*$4
 ***********************************************************************************************************************
    Functions Win32 pour adaptateur USB pour joy PS2
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_SelectJoyUSBPS2(int i)
{
	/*~~~~~~~~~~~~~~~~~~*/
	HKEY	h_Key;
	char	asz_Value[10];
	/*~~~~~~~~~~~~~~~~~~*/

	if((i < INO_JoyUSBPS2_Version0) || (i > INO_JoyUSBPS2_Version4)) i = INO_JoyUSBPS2_Version0;

	ERR_X_Error
	(
		RegCreateKey(HKEY_CURRENT_USER, "JADE_Joy", &h_Key) == ERROR_SUCCESS,
		"Can't open joy key in register",
		NULL
	);
	sprintf(asz_Value, "%d", i);
	RegSetValue(h_Key, "USBPS2Version", REG_SZ, asz_Value, L_strlen(asz_Value));
	RegCloseKey(h_Key);

	win32INO_l_JoyUSBPS2 = i;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32INO_Joystick_Move(MATH_tdst_Vector *_pst_Move, int _i_Num)
{
	/*~~~~~~~~~~~~~~~*/
	JOYINFOEX	st_Joy;
	LONG		X, Y;
	/*~~~~~~~~~~~~~~~*/

	if(_i_Num && win32INO_l_Joystick_Mode == INO_Joy_PCMode)
	{
		MATH_InitVectorToZero(_pst_Move);
		return;
	}

	st_Joy.dwSize = sizeof(JOYINFOEX);
	st_Joy.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNR | JOY_RETURNZ;
	joyGetPosEx(win32INO_l_Joystick_ID, &st_Joy);

    if (INO_b_PlayInput || INO_b_RecordInput)
    {            
        st_Joy = st_CurrentFrameJoy;                 
    }


	if(_i_Num)
	{
		if ( (win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version2) || (win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version3) || (win32INO_l_JoyUSBPS2 == INO_JoyUSBPS2_Version4))
		{
			st_Joy.dwXpos = st_Joy.dwRpos;
			st_Joy.dwYpos = st_Joy.dwZpos;
		}
		else
		{
			st_Joy.dwXpos = st_Joy.dwZpos;
			st_Joy.dwYpos = st_Joy.dwRpos;
		}
	
		if (win32INO_l_Joystick_Mode == INO_Joy_XenonMode)
		{
			st_Joy.dwXpos = st_Joy.dwUpos;
			st_Joy.dwYpos = st_Joy.dwRpos;
		}

	}

	X = (LONG) st_Joy.dwXpos;
	Y = (LONG) st_Joy.dwYpos;

    /*
    if(X < win32INO_l_Joystick_XRightStart[_i_Num] && X > win32INO_l_Joystick_XLeftStart[_i_Num])
		_pst_Move->x = 0;
	else
    */
	{
		_pst_Move->x = (float) X / (float) (win32INO_l_Joystick_XRight[_i_Num] - win32INO_l_Joystick_XLeft[_i_Num]);
		_pst_Move->x = (_pst_Move->x * 2.0f) - 1.0f;
	}

    /*
	if(Y < win32INO_l_Joystick_YDownStart[_i_Num] && Y > win32INO_l_Joystick_YUpStart[_i_Num])
		_pst_Move->y = 0;
	else
    */
	{	
		_pst_Move->y = (float) Y / (float) (win32INO_l_Joystick_YDown[_i_Num] - win32INO_l_Joystick_YUp[_i_Num]);
		_pst_Move->y = (_pst_Move->y * 2.0f) - 1.0f;
		_pst_Move->y = -_pst_Move->y;
	}

	_pst_Move->z = 0.0f;

	if(!_i_Num)
	{
		if(INO_b_IsKeyPressed(INO_gauc_KeyToPad[INO_CKTP_Left0])) _pst_Move->x = -1.0f;
		if(INO_b_IsKeyPressed(INO_gauc_KeyToPad[INO_CKTP_Up0])) _pst_Move->y = 1.0f;
		if(INO_b_IsKeyPressed(INO_gauc_KeyToPad[INO_CKTP_Right0])) _pst_Move->x = 1.0f;
		if(INO_b_IsKeyPressed(INO_gauc_KeyToPad[INO_CKTP_Down0])) _pst_Move->y = -1.0f;
		if(INO_b_IsKeyPressed(INO_gauc_KeyToPad[INO_CKTP_SpeedUp0]))
		{
			_pst_Move->x *= 0.5f;
			_pst_Move->y *= 0.5f;
		}
	}
	else
	{
		if(INO_b_IsKeyPressed(INO_gauc_KeyToPad[INO_CKTP_Left1])) _pst_Move->x = -1.0f;
		if(INO_b_IsKeyPressed(INO_gauc_KeyToPad[INO_CKTP_Up1])) _pst_Move->y = 1.0f;
		if(INO_b_IsKeyPressed(INO_gauc_KeyToPad[INO_CKTP_Right1])) _pst_Move->x = 1.0f;
		if(INO_b_IsKeyPressed(INO_gauc_KeyToPad[INO_CKTP_Down1])) _pst_Move->y = -1.0f;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32INO_PadVibSet( void )
{
	LPDIRECTINPUTEFFECT	pDI_Effect = NULL;
	float				f_Amplitude;

	if ( !win32INO_gi_DirectInputEnable ) 
		return;

	/* stop effect */
#ifdef JADEFUSION
	win32INO_gp_FFDevice->SendForceFeedbackCommand( DISFFC_STOPALL );
	win32INO_gp_FFDevice->SendForceFeedbackCommand( DISFFC_RESET );
	win32INO_gp_FFDevice->SendForceFeedbackCommand( DISFFC_SETACTUATORSON );
#else
	win32INO_gp_FFDevice->lpVtbl->SendForceFeedbackCommand( win32INO_gp_FFDevice, DISFFC_STOPALL );
	win32INO_gp_FFDevice->lpVtbl->SendForceFeedbackCommand( win32INO_gp_FFDevice, DISFFC_RESET );
	win32INO_gp_FFDevice->lpVtbl->SendForceFeedbackCommand( win32INO_gp_FFDevice, DISFFC_SETACTUATORSON );
#endif	
	/* pif */
	if ( INO_gf_PifDuration > 0 )
	{
		win32INO_st_EffectParam.dwDuration	= (DWORD)(INO_gf_PifDuration * DI_SECONDS);
	
		win32INO_st_EffectParam_Periodic.dwMagnitude = DI_FFNOMINALMAX;
		win32INO_st_EffectParam_Periodic.lOffset = 0;
		win32INO_st_EffectParam_Periodic.dwPhase = 0;
		win32INO_st_EffectParam_Periodic.dwPeriod = 33333;
		
#ifdef JADEFUSION
		win32INO_gp_FFDevice->CreateEffect( GUID_Square, &win32INO_st_EffectParam, &pDI_Effect, NULL );
		if (pDI_Effect)
			pDI_Effect->Start( 1, 0 );
		pDI_Effect = NULL;
#else
		win32INO_gp_FFDevice->lpVtbl->CreateEffect( win32INO_gp_FFDevice, &GUID_Square, &win32INO_st_EffectParam, &pDI_Effect, NULL );
		if (pDI_Effect)
			pDI_Effect->lpVtbl->Start( pDI_Effect, 1, 0 );
		pDI_Effect = NULL;
#endif
	}
	
	/* paf */
	if ( INO_gf_PafDuration > 0 )
	{
		f_Amplitude = (float) INO_gi_PafIntensity / 255;
		f_Amplitude *= DI_FFNOMINALMAX;
			
		if(INO_gf_PafDuration < 0.224f) INO_gf_PafDuration = 0.224f;
		win32INO_st_EffectParam.dwDuration	= (DWORD)(INO_gf_PafDuration * DI_SECONDS);
		
		/* pif */
		win32INO_st_EffectParam_Periodic.dwMagnitude = (DWORD) f_Amplitude;
		win32INO_st_EffectParam_Periodic.lOffset = 0;
		win32INO_st_EffectParam_Periodic.dwPhase = 0;
		win32INO_st_EffectParam_Periodic.dwPeriod = 1000000;
		
#ifdef JADEFUSION
		win32INO_gp_FFDevice->CreateEffect( GUID_Square, &win32INO_st_EffectParam, &pDI_Effect, NULL );
		if (pDI_Effect)
			pDI_Effect->Start( 1, 0 );
#else
		win32INO_gp_FFDevice->lpVtbl->CreateEffect( win32INO_gp_FFDevice, &GUID_Square, &win32INO_st_EffectParam, &pDI_Effect, NULL );
		if (pDI_Effect)
			pDI_Effect->lpVtbl->Start( pDI_Effect, 1, 0 );
#endif
	}
	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32INO_PadLowVibSet(BOOL _b_On)
{
	if ( !_b_On )
		INO_gf_PifDuration = 0;
	
	win32INO_PadVibSet();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32INO_PadHightVibSet(int _i_Level)
{
	if (_i_Level <= 0)
		INO_gf_PafDuration = 0;
	
	win32INO_PadVibSet();
}
#endif

#ifdef JOYRECORDER
// ***********************************************************************************************************************
void INO_StartRecord(  )
{
    AI_ReinitUniverse();
    INO_b_RecordInput = TRUE;
    MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 2;
    ENG_gb_NeedToReinit = TRUE;                    
    INO_n_FrameCounter = 0;
}

// ***********************************************************************************************************************

void INO_StartPlay(  )
{
    AI_ReinitUniverse();
    INO_b_PlayInput = TRUE;
    MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 2;
    ENG_gb_NeedToReinit = TRUE;                    
    INO_n_FrameCounter = 0;
}

// ***********************************************************************************************************************

void INO_StopRecord( )
{
    INO_b_RecordInput = FALSE;   
    MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 0;
}

// ***********************************************************************************************************************

void INO_StopPlay( )
{
    INO_b_PlayInput = FALSE;
    MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 0;
}
#endif
/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
