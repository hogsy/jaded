// ********************************************************
// Xenon-Specific Gamepad Low-Level Implementation
// By Alexandre David (March 2005)
//
// WARNING : The C part of this code is "included" by
// INOjoystick.c and is not meant to be compiled separately
// ********************************************************

#ifndef __XEINOJOYSTICK_H__
#define __XEINOJOYSTICK_H__

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

#define kXeINO_iPadID_Invalid						-1
#define kXeINO_iTrigger_XState_BooleanSensitivity	16
#define kXeINO_iStick_XState_BooleanSensitivity		16384
#define kXeINO_fStick_XState_MaxAmplitude			32767.0f
#define kXeINO_fStick_DeadZone						0.05f
#define kXeINO_iButtonInputLevel_Min				0x00
#define kXeINO_iButtonInputLevel_Max				0xFF

// ***********************************************************************************************************************
//    Globals
// ***********************************************************************************************************************

extern BOOL xeINO_bDesktop_InputLock;

// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************

typedef enum
{
	eXeButton_A					= 0,
	eXeButton_B					= 1,
	eXeButton_X					= 2,
	eXeButton_Y					= 3,
	eXeButton_Shoulder_Left		= 4,
	eXeButton_Shoulder_Right	= 5,
	eXeButton_Trigger_Left		= 6,
	eXeButton_Trigger_Right		= 7,
	eXeButton_Back				= 8,
	eXeButton_Start				= 9,
	eXeButton_Thumb_Right		= 10,
	eXeButton_Thumb_Left		= 11,
	eXeButton_DPad_Up			= 12,
	eXeButton_DPad_Right		= 13,
	eXeButton_DPad_Down			= 14,
	eXeButton_DPad_Left			= 15
}
xeINO_eButtonID;

enum ActivityTimer
{
	TimerAttractMode	= 1 << 0,
	TimerExitDemo		= 2 << 1
};

// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************

typedef struct
{
	BOOL				bIsConnected;
	XINPUT_STATE		xState;
	XINPUT_CAPABILITIES	xCapabilities;
	XINPUT_VIBRATION	xVibration;
}
xeINO_stPadDescriptor;

// ***********************************************************************************************************************
//    Functions - Platform Specific
// ***********************************************************************************************************************

float	xeINO_GetInactivitySeconds( ActivityTimer timer );
void	xeINO_SetInactivitySeconds( ActivityTimer timer, float fTime );
void	xeINO_ResetInactivityTimer( int timers );
BOOL	xeINO_IsControllerLost();

void	xeINO_PauseVibration();
void	xeINO_ResumeVibration();
BOOL	xeINO_IsVibrationPaused();

void	xeINO_EnableMergedInput( bool bEnabled );

// ***********************************************************************************************************************

#endif // __XEINOJOYSTICK_H