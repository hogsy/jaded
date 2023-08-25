/*$T INOjoystick.c GC 1.138 06/24/03 19:37:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include <SDL.h>

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

#if defined( INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD )

#	define DIRECTINPUT_VERSION 0x0800
#	include <dinput.h>
#	include <stdarg.h>
#	include "TIMer/TIMdefs.h"
#	define INO_JOYSTICK_USE_CHEATS
#	include "Dx9/Dx9Console.h"

#endif

int ControlMode = 2;
//#define JAPANESE_VERSION
static BOOL           IO_bJAPANESE_MENU = FALSE;
extern BOOL           ENG_gb_InPause;
extern BOOL           ENG_gb_ForcePauseEngine;
extern unsigned short Demo_gus_language;

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct INO_tdst_JoySimulate_
{
	LONG  l_Mask;
	LONG  l_Buttons;
	float f_Joy0[ 2 ];
	float f_Joy1[ 2 ];
} INO_tdst_JoySimulate;

typedef struct INO_tdst_GenericPad_
{
	unsigned char ac_Button[ eBtn_GenericButtonNb ];
} INO_tdst_GenericPad;

// Joystick Recorder
static ULONG INO_n_NumRecord;
ULONG        INO_n_FrameCounter;
int          INO_b_RecordInput;
int          INO_b_PlayInput;
char         INO_dumpFileName[ 128 ];
L_FILE       INO_dumpFile;
extern ULONG BIG_ul_SearchKeyToFat( BIG_KEY );
extern UINT  WOR_gul_WorldKey;
extern BOOL  ENG_gb_NeedToReinit;
#include "ENGine/Sources/ENGvars.h"
#include "AIinterp/Sources/AIengine.h"
extern unsigned int WOR_gul_WorldKey;
#include "BIGfiles\BIGdefs.h"
#include "BIGfiles\BIGfat.h"

#if defined( _XENON )
#	define INPUT_REC_FILE "D:\\INPUT_REC_%08X_XE.ino"

#elif defined( INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD )

#else
JOYINFOEX st_CurrentFrameJoy;
#	define INPUT_REC_FILE "X:\\INPUT_REC_%08X.ino"
#endif


/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern float TIM_gf_dt;
extern float TIM_gf_MainClock;

/*$2------------------------------------------------------------------------------------------------------------------*/

LONG INO_l_JoystickMask;
LONG INO_l_JoystickDir;

static bool INO_l_Joystick_Enable;

/*$2- vibration ------------------------------------------------------------------------------------------------------*/

static float INO_gf_PifDuration;
static float INO_gf_PafDuration;

static int  INO_gi_PafIntensity;
static bool INO_gi_PifPafEnable;

/*$2- simulation -----------------------------------------------------------------------------------------------------*/

static INO_tdst_JoySimulate INO_gst_JoySimulateAsked;
static INO_tdst_JoySimulate INO_gst_JoySimulateCurrent;

/*$2------------------------------------------------------------------------------------------------------------------*/

#define INO_Cte_PabNumber 8

static INO_tdst_GenericPad INO_gst_CurrPad[ INO_Cte_PabNumber ];
static INO_tdst_GenericPad INO_gst_PrevPad[ INO_Cte_PabNumber ];
static int                 INO_gi_CurrentPadId;
int                        INO_gi_PadConnectedNb = 0;

static SDL_GameController *sdlGameControllers[ INO_Cte_PabNumber ];

// assorted crap that can eventually be removed ~hogsy
LONG win32INO_l_Joystick_Mode = INO_Joy_Ps2Mode;
LONG win32INO_l_Joystick_XLeft[ 2 ] = { 0, 0 };
LONG win32INO_l_Joystick_XLeftStart[ 2 ] = { 22000, 22000 };
LONG win32INO_l_Joystick_XRightStart[ 2 ] = { 42000, 42000 };
LONG win32INO_l_Joystick_XRight[ 2 ] = { 65536, 65536 };
LONG win32INO_l_Joystick_YUp[ 2 ] = { 0, 0 };
LONG win32INO_l_Joystick_YUpStart[ 2 ] = { 22000, 22000 };
LONG win32INO_l_Joystick_YDownStart[ 2 ] = { 42000, 42000 };
LONG win32INO_l_Joystick_YDown[ 2 ] = { 65536, 65536 };
LONG win32INO_l_JoyUSBPS2 = 0;

/* Fouziya & boujemaa fin */
#ifdef ACTIVE_EDITORS
LONG INO_l_Joystick_X[ 2 ];
LONG INO_l_Joystick_Y[ 2 ];
#endif /* ACTIVE_EDITORS */

/*$2------------------------------------------------------------------------------------------------------------------*/

// Control modes
#define IO_MODE_JACK 0
#define IO_MODE_KONG 1
#define IO_MODE_MENU 2

/*$2------------------------------------------------------------------------------------------------------------------*/

// Mvt for revolution pad
#define IO_MVT_None        0
#define IO_MVT_LeftToRight 1
#define IO_MVT_RightToLeft 2
#define IO_MVT_UpToDown    3
#define IO_MVT_DownToUp    4

/*$2------------------------------------------------------------------------------------------------------------------*/


BOOL Demo_gb_Exit = FALSE;
BOOL Demo_gb_IsActive = FALSE;

BOOL TwoPadGame = FALSE;

/*$4
 ***********************************************************************************************************************
    prototypes for interface
 ***********************************************************************************************************************
 */

#if defined( INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD )
/*static*/ void pcINO_SetControlMode( int );   // referenced from STRparse_pc.c
/*static*/ void pcINO_SetControlSubmode( int );// referenced from STRparse_pc.c
/*static*/ void pcINO_ResetKeyboardFsm( int ); // referenced from WORuniverse.c
#endif

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

void INO_Joystick_Init( HWND _hWnd )
{
#if defined( INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD )
	INO_gst_JoyInterface.pfv_Joystick_Init = pcINO_Joystick_Init;
	INO_gst_JoyInterface.pfv_Joystick_Close = pcINO_Joystick_Close;
	INO_gst_JoyInterface.pfv_Joystick_Update = pcINO_Joystick_Update;
	INO_gst_JoyInterface.pfv_Joystick_LowVibSet = pcINO_PadLowVibSet;
	INO_gst_JoyInterface.pfv_Joystick_HightVibSet = pcINO_PadHightVibSet;
	INO_gst_JoyInterface.pfv_Joystick_Move = pcINO_Joystick_Move;
	INO_gst_JoyInterface.pfb_Joystick_IsValid = pcINO_b_Joystick_IsValid;
	INO_gst_JoyInterface.pfi_Joystick_SetCurrentPad = pcINO_i_SetCurrentPad;
#endif

	INO_n_FrameCounter = 0;
	INO_n_NumRecord = 0;

	INO_l_JoystickMask = INO_JoyMask_All;
	INO_l_JoystickDir = 0;
	INO_l_Joystick_Enable = FALSE;

	INO_gf_PifDuration = 0.0f;
	INO_gf_PafDuration = 0.0f;
	INO_gi_PafIntensity = 0;
	INO_gi_PifPafEnable = true;

	L_memset( &INO_gst_JoySimulateAsked, 0, sizeof( INO_tdst_JoySimulate ) );
	L_memset( &INO_gst_JoySimulateCurrent, 0, sizeof( INO_tdst_JoySimulate ) );

	INO_gi_PadConnectedNb = 0;
	INO_gi_CurrentPadId = -1;
	L_memset( INO_gst_CurrPad, 0, INO_Cte_PabNumber * sizeof( INO_tdst_GenericPad ) );
	L_memset( INO_gst_PrevPad, 0, INO_Cte_PabNumber * sizeof( INO_tdst_GenericPad ) );

	L_zero( sdlGameControllers, sizeof( SDL_GameController * ) * INO_Cte_PabNumber );

	if ( SDL_GameControllerAddMappingsFromFile( "mappings/gamecontrollerdb.txt" ) == -1 )
	{
		ERR_X_Warning( 0, "Failed to fetch game controller mappings!", SDL_GetError() );
		// not a problem if it's missing, mappings are just going to be lame ~hogsy
	}

	int numControllers = SDL_NumJoysticks();
	if ( numControllers > INO_Cte_PabNumber )
	{
		numControllers = INO_Cte_PabNumber;
	}

	for ( int i = 0; i < numControllers; ++i )
	{
		if ( !SDL_IsGameController( i ) )
		{
			continue;
		}

		if ( ( sdlGameControllers[ i ] = SDL_GameControllerOpen( i ) ) == NULL )
		{
			ERR_X_Warning( 0, "Failed to open game controller!", SDL_GetError() );
			continue;
		}

		char tmp[ 512 ];
		snprintf( tmp, sizeof( tmp ), "Opened controller %i: %s (%s)", i,
		          SDL_GameControllerName( sdlGameControllers[ i ] ),
		          SDL_GameControllerGetSerial( sdlGameControllers[ i ] ) );
		LINK_PrintStatusMsg( tmp );

		INO_gi_PadConnectedNb++;
		if ( INO_gi_CurrentPadId == -1 )
		{
			INO_gi_CurrentPadId = i;
		}
	}

	INO_l_Joystick_Enable = TRUE;

	LINK_PrintStatusMsg( "SDL controller input initialized." );
}

int INO_i_SetCurrentPad( int id )
{
	if ( INO_gi_CurrentPadId >= INO_gi_PadConnectedNb )
	{
		id = ( INO_gi_PadConnectedNb - 1 );
	}

	return ( INO_gi_CurrentPadId = id );
}

int INO_i_GetCurrentPad( void )
{
	return INO_gi_CurrentPadId;
}

void INO_SimulateReset( void )
{
	L_memset( &INO_gst_JoySimulateAsked, 0, sizeof( INO_tdst_JoySimulate ) );
	L_memset( &INO_gst_JoySimulateCurrent, 0, sizeof( INO_tdst_JoySimulate ) );
}

void INO_Joystick_Close( void )
{
	if ( !INO_l_Joystick_Enable )
	{
		return;
	}

	for ( int i = 0; i < INO_Cte_PabNumber; ++i )
	{
		if ( sdlGameControllers[ i ] == NULL )
		{
			continue;
		}

		SDL_GameControllerClose( sdlGameControllers[ i ] );
		sdlGameControllers[ i ] = NULL;
	}

	INO_l_Joystick_Enable = false;
}

static bool GetSDLButtonState( SDL_GameController *gameController, INO_tden_GenericButtonId button )
{
	if ( button == eBtn_L2 || button == eBtn_R2 )
	{
		if ( SDL_GameControllerGetAxis( gameController, ( button == eBtn_L2 ) ? SDL_CONTROLLER_AXIS_TRIGGERLEFT : SDL_CONTROLLER_AXIS_TRIGGERRIGHT ) != 0 )
		{
			return true;
		}

		return false;
	}

	static const SDL_GameControllerButton buttonMappings[ eBtn_GenericButtonNb ] =
	        {
	                SDL_CONTROLLER_BUTTON_A,            //eBtn_Cross
	                SDL_CONTROLLER_BUTTON_B,            //eBtn_Circle
	                SDL_CONTROLLER_BUTTON_X,            //eBtn_Square
	                SDL_CONTROLLER_BUTTON_Y,            //eBtn_Triangle
	                SDL_CONTROLLER_BUTTON_LEFTSHOULDER, //eBtn_L1
	                SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,//eBtn_R1
	                SDL_CONTROLLER_BUTTON_INVALID,
	                SDL_CONTROLLER_BUTTON_INVALID,   // L+R triggers are special cases
	                SDL_CONTROLLER_BUTTON_BACK,      //eBtn_Select
	                SDL_CONTROLLER_BUTTON_START,     //eBtn_Start
	                SDL_CONTROLLER_BUTTON_RIGHTSTICK,//eBtn_R3
	                SDL_CONTROLLER_BUTTON_LEFTSTICK, //eBtn_L3
	                SDL_CONTROLLER_BUTTON_DPAD_UP,   //eBtn_Up
	                SDL_CONTROLLER_BUTTON_DPAD_RIGHT,//eBtn_Right
	                SDL_CONTROLLER_BUTTON_DPAD_DOWN, //eBtn_Down
	                SDL_CONTROLLER_BUTTON_DPAD_LEFT, //eBtn_Left
	        };

	return SDL_GameControllerGetButton( gameController, buttonMappings[ button ] );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_Joystick_Update( void )
{
	if ( !INO_l_Joystick_Enable ) return;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    update simulate
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	L_memcpy( &INO_gst_JoySimulateCurrent, &INO_gst_JoySimulateAsked, sizeof( INO_tdst_JoySimulate ) );
	L_memset( &INO_gst_JoySimulateAsked, 0, sizeof( INO_tdst_JoySimulate ) );

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    update current value
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	L_memcpy( INO_gst_PrevPad, INO_gst_CurrPad, INO_Cte_PabNumber * sizeof( INO_tdst_GenericPad ) );

#if 0//TODO
	if ( INO_b_RecordInput )
	{
		if ( INO_n_FrameCounter == 0 )
		{
			/* Create file at the same pos as exe */
			sprintf( INO_dumpFileName, INPUT_REC_FILE, WOR_gul_WorldKey );
			if ( !( L_access( INO_dumpFileName, 0 ) ) ) L_chmod( INO_dumpFileName, L_S_IREAD | L_S_IWRITE );

			INO_dumpFile = L_fopen( INO_dumpFileName, L_fopen_WB );
			L_fwriteA( &INO_n_FrameCounter, sizeof( INO_n_FrameCounter ), 1, INO_dumpFile );
		}

		L_fwriteA( ( void * ) &st_Joy, sizeof( JOYINFOEX ), 1, INO_dumpFile );
		INO_n_NumRecord = INO_n_FrameCounter;

		st_CurrentFrameJoy = st_Joy;
	}
	else if ( INO_b_PlayInput )
	{
		if ( INO_n_FrameCounter == 0 )
		{
			sprintf( INO_dumpFileName, "X:\\INPUT_REC_%08X.ino", WOR_gul_WorldKey );
			if ( !( L_access( INO_dumpFileName, 0 ) ) ) L_chmod( INO_dumpFileName, L_S_IREAD | L_S_IWRITE );

			INO_dumpFile = L_fopen( INO_dumpFileName, L_fopen_RB );
			L_freadA( &INO_n_NumRecord, sizeof( INO_n_NumRecord ), 1, INO_dumpFile );
		}

		L_freadA( ( void * ) &st_Joy, sizeof( JOYINFOEX ), 1, INO_dumpFile );
		st_CurrentFrameJoy = st_Joy;

		if ( INO_n_FrameCounter >= INO_n_NumRecord )
			INO_b_PlayInput = FALSE;
	}
	else if ( INO_n_FrameCounter != 0 )
	{
		if ( INO_dumpFile != NULL )
		{
			// going to the begining of the file
			fseek( INO_dumpFile, 0, 0 );
			// overwriting number of frames
			L_fwriteA( &INO_n_FrameCounter, sizeof( INO_n_FrameCounter ), 1, INO_dumpFile );
			L_fclose( INO_dumpFile );
		}
		INO_n_FrameCounter = 0;
	}
#endif

	INO_gi_CurrentPadId = 0;//TODO: temporary crud... ~hogsy
	SDL_GameController *gameController = sdlGameControllers[ INO_gi_CurrentPadId ];

	LONG      buttonState = 0;
	SDL_Event sdlEvent;
	while ( SDL_PollEvent( &sdlEvent ) )
	{
		switch ( sdlEvent.type )
		{
			case SDL_CONTROLLERDEVICEREMOVED:
			{
				if ( gameController == NULL )
				{
					break;
				}

				if ( !SDL_GameControllerGetAttached( gameController ) )
				{
					SDL_GameControllerClose( gameController );
					gameController = NULL;
				}
				break;
			}
			case SDL_CONTROLLERDEVICEADDED:
			{
				if ( gameController != NULL )
				{
					break;
				}

				gameController = SDL_GameControllerOpen( sdlEvent.cdevice.which );
				break;
			}
			default:
				break;
		}
	}

	sdlGameControllers[ INO_gi_CurrentPadId ] = gameController;
	if ( gameController == NULL )
	{
		return;
	}

#if defined( ACTIVE_EDITORS )
	// left stick
	INO_l_Joystick_X[ 0 ] = INT16_MAX + SDL_GameControllerGetAxis( gameController, SDL_CONTROLLER_AXIS_LEFTX );
	INO_l_Joystick_Y[ 0 ] = INT16_MAX + SDL_GameControllerGetAxis( gameController, SDL_CONTROLLER_AXIS_LEFTY );
	// right stick
	INO_l_Joystick_X[ 1 ] = INT16_MAX + SDL_GameControllerGetAxis( gameController, SDL_CONTROLLER_AXIS_RIGHTX );
	INO_l_Joystick_Y[ 1 ] = INT16_MAX + SDL_GameControllerGetAxis( gameController, SDL_CONTROLLER_AXIS_RIGHTY );
#endif

	for ( unsigned int i = 0; i < eBtn_GenericButtonNb; ++i )
	{
		INO_gst_CurrPad[ INO_gi_CurrentPadId ].ac_Button[ i ] = GetSDLButtonState( gameController, ( INO_tden_GenericButtonId ) i );
#if 0 // hogsy: removed for now
		if ( INO_b_IsKeyPressed( INO_gauc_KeyToPad[ INO_CKPT_ButtonsFirst + i ] ) )
		{
			INO_gst_CurrPad[ INO_gi_CurrentPadId ].ac_Button[ i ] = 0xFF;
		}
#endif
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    save real result
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */
	L_memcpy( INO_gst_CurrPad + 4, INO_gst_CurrPad, 4 * sizeof( INO_tdst_GenericPad ) );

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    special case for PS2 demo
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if ( !Demo_gb_Exit )
	{
		for ( unsigned int i = 0; i < INO_Cte_PabNumber; i++ )
		{
			if ( INO_gst_CurrPad[ i ].ac_Button[ eBtn_Select ] && INO_gst_CurrPad[ i ].ac_Button[ eBtn_Start ] )
			{
				Demo_gb_Exit = TRUE;
				break;
			}
		}
	}

	if ( INO_gi_CurrentPadId != -1 )
	{

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    apply mask
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		for ( unsigned int i = 0; i < eBtn_GenericButtonNb; i++ )
		{
			if ( !( INO_l_JoystickMask & ( 1 << i ) ) ) INO_gst_CurrPad[ INO_gi_CurrentPadId ].ac_Button[ i ] = 0;
		}

		/*$1
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		    apply simulate
		 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 */

		if ( INO_gst_JoySimulateCurrent.l_Mask )
		{
			unsigned char l_Mask[ eBtn_GenericButtonNb ];
			L_memset( l_Mask, 0, eBtn_GenericButtonNb * sizeof( char ) );
			unsigned char l_Buttons[ eBtn_GenericButtonNb ];
			L_memset( l_Buttons, 0, eBtn_GenericButtonNb * sizeof( char ) );

			for ( unsigned int i = 0; i < eBtn_GenericButtonNb; i++ )
			{
				if ( INO_gst_JoySimulateCurrent.l_Mask & ( 1 << i ) ) l_Mask[ i ] = 0xFF;
				if ( INO_gst_JoySimulateCurrent.l_Buttons & ( 1 << i ) ) l_Buttons[ i ] = 0xFF;

				INO_gst_CurrPad[ INO_gi_CurrentPadId ].ac_Button[ i ] &= ~l_Mask[ i ];
				INO_gst_CurrPad[ INO_gi_CurrentPadId ].ac_Button[ i ] |= ( l_Buttons[ i ] & l_Mask[ i ] );
			}
		}
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    vibration
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if ( INO_gf_PifDuration )
	{
		INO_gf_PifDuration -= TIM_gf_dt;
		if ( INO_gf_PifDuration <= 0.0f )
		{
			SDL_GameControllerRumble( sdlGameControllers[ INO_gi_CurrentPadId ], 0, 0, 0 );
			INO_gf_PifDuration = 0.0f;
		}
	}

	if ( INO_gf_PafDuration )
	{
#ifndef _XENON
		// no condition on Xenon since we stop vibrations on pause anyway (TCR requirement)
		if ( !ENG_gb_InPause && !ENG_gb_ForcePauseEngine )
#endif
			INO_gf_PafDuration -= TIM_gf_dt;
		if ( INO_gf_PafDuration <= 0.0f )
		{
			SDL_GameControllerRumble( sdlGameControllers[ INO_gi_CurrentPadId ], 0, 0, 0 );
			INO_gf_PafDuration = 0.0f;
		}
	}

	if ( INO_b_RecordInput || INO_b_PlayInput )
	{
		INO_n_FrameCounter++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int INO_i_PifPafEnable( int _i_Enable )
{
	if ( _i_Enable != -1 )
	{
		if ( _i_Enable )
			INO_gi_PifPafEnable = 1;
		else
		{
			INO_gi_PifPafEnable = 0;
			INO_gf_PifDuration = 0.0f;
			INO_gf_PafDuration = 0.0f;
			SDL_GameControllerRumble( sdlGameControllers[ INO_gi_CurrentPadId ], 0, 0, 0 );
		}
	}

	return INO_gi_PifPafEnable;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_PifSet( int _i_FrameDuration )
{
#if 0// wtf does pif mean in this context?? ~hogsy
	if ( !INO_gi_PifPafEnable || INO_gi_CurrentPadId == -1 ) 
	{
		return;
	}

	SDL_GameController *controller = sdlGameControllers[ INO_gi_CurrentPadId ];
	if ( controller == NULL )
	{
		return;
	}

	SDL_Joystick *joystick = SDL_GameControllerGetJoystick( controller );
	if ( joystick == NULL )
	{
		return;
	}

	if ( _i_FrameDuration > 0 )
	{
		INO_gf_PifDuration = fLongToFloat( lAbs( _i_FrameDuration ) ) * 0.032f;
		SDL_JoystickRumble( joystick, lFloatToLong( 0.5f ), lFloatToLong( 0.5f ), INO_gf_PifDuration );
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_PafSet( int _i_Intensity, int _i_FrameDuration )
{
	if ( !INO_gi_PifPafEnable || INO_gi_CurrentPadId == -1 )
	{
		return;
	}

	SDL_GameController *controller = sdlGameControllers[ INO_gi_CurrentPadId ];
	if ( controller == NULL )
	{
		return;
	}

	if ( _i_FrameDuration > 0 )
	{
		INO_gf_PafDuration = fLongToFloat( lAbs( _i_FrameDuration ) ) * 0.032f;
		INO_gi_PafIntensity = ( _i_Intensity > 0xff ) ? 0xFF : _i_Intensity;
		SDL_GameControllerRumble( controller, _i_Intensity, _i_Intensity, _i_FrameDuration );
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INO_b_Joystick_IsButtonDown( LONG _l_ButtonIndex )
{
	if ( INO_gi_CurrentPadId == -1 ) return FALSE;
	return INO_gst_CurrPad[ INO_gi_CurrentPadId ].ac_Button[ _l_ButtonIndex ];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INO_b_Joystick_IsButtonJustDown( LONG _l_ButtonIndex )
{
	if ( INO_gi_CurrentPadId == -1 ) return FALSE;
	if ( INO_gst_CurrPad[ INO_gi_CurrentPadId ].ac_Button[ _l_ButtonIndex ] && !INO_gst_PrevPad[ INO_gi_CurrentPadId ].ac_Button[ _l_ButtonIndex ] ) 
		return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL INO_b_Joystick_IsButtonJustUp( LONG _l_ButtonIndex )
{
	if ( INO_gi_CurrentPadId == -1 ) return FALSE;
	if (
	        !INO_gst_CurrPad[ INO_gi_CurrentPadId ].ac_Button[ _l_ButtonIndex ] && INO_gst_PrevPad[ INO_gi_CurrentPadId ].ac_Button[ _l_ButtonIndex ] ) return TRUE;
	return FALSE;
}
BOOL INO_b_MultiJoystick_IsButtonJustUp( LONG _l_ButtonIndex )
{
	int          iii;
	static short PrevPad[ 4 ][ 10 ] = { 0 };
	int          saveC = INO_gi_CurrentPadId;
	for ( iii = 0; iii < 4; iii++ )
	{

		INO_gi_CurrentPadId = iii;
		//INO_gi_LastPadId=iii;
		INO_Joystick_Update();

		if (
		        !INO_gst_CurrPad[ iii ].ac_Button[ _l_ButtonIndex ] && PrevPad[ iii ][ _l_ButtonIndex ] )
		{
			PrevPad[ iii ][ _l_ButtonIndex ] = 0;
			INO_gi_CurrentPadId = saveC;
			return TRUE;
		}

		PrevPad[ iii ][ _l_ButtonIndex ] = INO_gst_CurrPad[ iii ].ac_Button[ _l_ButtonIndex ];

		//{INO_gi_CurrentPadId = saveC;INO_gi_LastPadId=saveL;return FALSE;}*/

		//if (INO_gst_CurrPad[iii].ac_Button[_l_ButtonIndex])
		//return TRUE;
	}
	INO_gi_CurrentPadId = saveC;
	return FALSE;
}

u32  JADE_HorizontalSymetry = 0;
void INO_Joystick_Move( MATH_tdst_Vector *_pst_Move, int _i_Num )
{
	BOOL b_InvertX;

	if ( !INO_l_Joystick_Enable || ( INO_gi_CurrentPadId == -1 ) )
	{
		MATH_InitVectorToZero( _pst_Move );
		return;
	}

	b_InvertX = INO_l_JoystickDir & INO_JoyDir_X0Invert;
	if ( JADE_HorizontalSymetry && ( ControlMode != 2 ) ) b_InvertX = !b_InvertX;

	//if (JADE_HorizontalSymetry && (ControlMode!=2) ) INO_l_JoystickDir ^= INO_JoyDir_X0Invert;
	if ( !( INO_gi_CurrentPadId & 4 ) )
	{
		if ( INO_gst_JoySimulateCurrent.l_Mask & ( 1 << ( INO_JoyMask_JoyShift + _i_Num ) ) )
		{
			if ( _i_Num )
			{
				MATH_InitVector( _pst_Move, INO_gst_JoySimulateCurrent.f_Joy1[ 0 ], INO_gst_JoySimulateCurrent.f_Joy1[ 1 ], 0 );
			}
			else
			{
				MATH_InitVector( _pst_Move, INO_gst_JoySimulateCurrent.f_Joy0[ 0 ], INO_gst_JoySimulateCurrent.f_Joy0[ 1 ], 0 );
			}

			if ( b_InvertX ) _pst_Move->x *= -1;
			if ( INO_l_JoystickDir & INO_JoyDir_Y0Invert ) _pst_Move->y *= -1;

			//if (JADE_HorizontalSymetry) INO_l_JoystickDir ^= INO_JoyDir_X0Invert;
			return;
		}

		if ( !( INO_l_JoystickMask & ( 1 << ( INO_JoyMask_JoyShift + _i_Num ) ) ) )
		{
			MATH_InitVectorToZero( _pst_Move );
			//if (JADE_HorizontalSymetry) INO_l_JoystickDir ^= INO_JoyDir_X0Invert;
			return;
		}
	}

	//INO_gst_JoyInterface.pfv_Joystick_Move( _pst_Move, _i_Num );

	if ( sdlGameControllers[ INO_gi_CurrentPadId ] != NULL )
	{
		int x, y;
		if ( _i_Num == 0 )
		{
			x = INT16_MAX + SDL_GameControllerGetAxis( sdlGameControllers[ INO_gi_CurrentPadId ], SDL_CONTROLLER_AXIS_LEFTX );
			y = INT16_MAX + SDL_GameControllerGetAxis( sdlGameControllers[ INO_gi_CurrentPadId ], SDL_CONTROLLER_AXIS_LEFTY );
		}
		else
		{
			x = INT16_MAX + SDL_GameControllerGetAxis( sdlGameControllers[ INO_gi_CurrentPadId ], SDL_CONTROLLER_AXIS_RIGHTX );
			y = INT16_MAX + SDL_GameControllerGetAxis( sdlGameControllers[ INO_gi_CurrentPadId ], SDL_CONTROLLER_AXIS_RIGHTY );
		}

		_pst_Move->x = ( float ) x / ( float ) ( win32INO_l_Joystick_XRight[ _i_Num ] - win32INO_l_Joystick_XLeft[ _i_Num ] );
		_pst_Move->x = ( _pst_Move->x * 2.0f ) - 1.0f;

		_pst_Move->y = ( float ) y / ( float ) ( win32INO_l_Joystick_YDown[ _i_Num ] - win32INO_l_Joystick_YUp[ _i_Num ] );
		_pst_Move->y = ( _pst_Move->y * 2.0f ) - 1.0f;
		_pst_Move->y = -_pst_Move->y;

		_pst_Move->z = 0.0f;
	}

#if 0 // hogsy: removed for now
	if ( !_i_Num )
	{
		if ( INO_b_IsKeyPressed( INO_gauc_KeyToPad[ INO_CKTP_Left0 ] ) ) _pst_Move->x = -1.0f;
		if ( INO_b_IsKeyPressed( INO_gauc_KeyToPad[ INO_CKTP_Up0 ] ) ) _pst_Move->y = 1.0f;
		if ( INO_b_IsKeyPressed( INO_gauc_KeyToPad[ INO_CKTP_Right0 ] ) ) _pst_Move->x = 1.0f;
		if ( INO_b_IsKeyPressed( INO_gauc_KeyToPad[ INO_CKTP_Down0 ] ) ) _pst_Move->y = -1.0f;
		if ( INO_b_IsKeyPressed( INO_gauc_KeyToPad[ INO_CKTP_SpeedUp0 ] ) )
		{
			_pst_Move->x *= 0.5f;
			_pst_Move->y *= 0.5f;
		}
	}
	else
	{
		if ( INO_b_IsKeyPressed( INO_gauc_KeyToPad[ INO_CKTP_Left1 ] ) ) _pst_Move->x = -1.0f;
		if ( INO_b_IsKeyPressed( INO_gauc_KeyToPad[ INO_CKTP_Up1 ] ) ) _pst_Move->y = 1.0f;
		if ( INO_b_IsKeyPressed( INO_gauc_KeyToPad[ INO_CKTP_Right1 ] ) ) _pst_Move->x = 1.0f;
		if ( INO_b_IsKeyPressed( INO_gauc_KeyToPad[ INO_CKTP_Down1 ] ) ) _pst_Move->y = -1.0f;
	}
#endif

	if ( b_InvertX ) _pst_Move->x *= -1;
	if ( INO_l_JoystickDir & INO_JoyDir_Y0Invert ) _pst_Move->y *= -1;
	//if (JADE_HorizontalSymetry) INO_l_JoystickDir ^= INO_JoyDir_X0Invert;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void INO_Joystick_GetPointer( MATH_tdst_Vector *_pst_Move )
{
	if ( !INO_l_Joystick_Enable || ( INO_gi_CurrentPadId == -1 ) )
	{
		MATH_InitVectorToZero( _pst_Move );
		return;
	}

	MATH_InitVectorToZero( _pst_Move );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void INO_Joystick_GetAcc( MATH_tdst_Vector *_pst_Move, int _i_Num )
{
	if ( !INO_l_Joystick_Enable || ( INO_gi_CurrentPadId == -1 ) )
	{
		MATH_InitVectorToZero( _pst_Move );
		return;
	}

	MATH_InitVectorToZero( _pst_Move );
}

int INO_Joystick_GetLastMove( int _i_Num )
{
	if ( !INO_l_Joystick_Enable || ( INO_gi_CurrentPadId == -1 ) )
	{
		return IO_MVT_None;
	}

	return IO_MVT_None;
}

float INO_Joystick_GetDistance()
{
	if ( !INO_l_Joystick_Enable || ( INO_gi_CurrentPadId == -1 ) )
	{
		return 0.0f;
	}

	return 0.0f;
}

void INO_Joystick_SetVibration( int i_Num, float f_Delay )
{
	if ( !INO_l_Joystick_Enable || ( INO_gi_CurrentPadId == -1 ) )
	{
		return;
	}
}

/*
 =======================================================================================================================
    Simulate joystick
 =======================================================================================================================
 */
void INO_JoySimulate( int _i_Mask, int _i_Buttons, MATH_tdst_Vector *_pst_Move0, MATH_tdst_Vector *_pst_Move1 )
{
	INO_gst_JoySimulateAsked.l_Mask = _i_Mask;
	INO_gst_JoySimulateAsked.l_Buttons = _i_Buttons;

	if ( _pst_Move0 )
	{
		INO_gst_JoySimulateAsked.f_Joy0[ 0 ] = _pst_Move0->x;
		INO_gst_JoySimulateAsked.f_Joy0[ 1 ] = _pst_Move0->y;
	}

	if ( _pst_Move1 )
	{
		INO_gst_JoySimulateAsked.f_Joy1[ 0 ] = _pst_Move1->x;
		INO_gst_JoySimulateAsked.f_Joy1[ 1 ] = _pst_Move1->y;
	}
}

/*
 =======================================================================================================================
    Set control context
 =======================================================================================================================
 */
static void Trace( const char *s, ... );

void INO_SetControlMode( int mode )
{
	ControlMode = mode;

#ifdef INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD
	pcINO_SetControlMode( mode );
#else
	( void ) mode;
#endif
}

void INO_SetControlSubmode( int submode )
{
#ifdef INO_JOYSTICK_USE_MOUSE_AND_KEYBOARD
	pcINO_SetControlSubmode( submode );
#else
	( void ) submode;
#endif
}

/*
 =======================================================================================================================
    Check if a mouse is being used for the control
 =======================================================================================================================
 */
int INO_i_IsMouseBeingUsed()
{
	//TODO: check if mouse is grabbed for current context?
	return true;
}

#ifdef JOYRECORDER
// ***********************************************************************************************************************
void INO_StartRecord()
{
	AI_ReinitUniverse();
	INO_b_RecordInput = TRUE;
	MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 2;
	ENG_gb_NeedToReinit = TRUE;
	INO_n_FrameCounter = 0;
}

// ***********************************************************************************************************************

void INO_StartPlay()
{
	AI_ReinitUniverse();
	INO_b_PlayInput = TRUE;
	MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 2;
	ENG_gb_NeedToReinit = TRUE;
	INO_n_FrameCounter = 0;
}

// ***********************************************************************************************************************

void INO_StopRecord()
{
	INO_b_RecordInput = FALSE;
	MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 0;
}

// ***********************************************************************************************************************

void INO_StopPlay()
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
