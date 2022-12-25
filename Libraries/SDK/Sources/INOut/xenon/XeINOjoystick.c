// ********************************************************
// Xenon-Specific Gamepad Low-Level Implementation
// By Alexandre David (March 2005)
//
// WARNING : The C part of this code is "included" by
// INOjoystick.c and is not meant to be compiled separately
// ********************************************************

#if defined(_XENON)
// ***********************************************************************************************************************
//    Externs
// ***********************************************************************************************************************
extern float TIM_gf_MainClock;
extern u32 JADE_HorizontalSymetry;

#ifdef JOYRECORDER
extern int	 INO_b_RecordInput;
extern int	 INO_b_PlayInput;
extern char  INO_dumpFileName[128];
extern L_FILE INO_dumpFile;
extern ULONG ENG_ulCustomLevel;
extern UINT WOR_gul_WorldKey;
extern BOOL ENG_gb_NeedToReinit;

int INO_b_InputWillBePlayed;
int INO_b_InputWillBeRecorded;
int INO_n_NumRepeat;
static const int NUM_FRAME_BEFORE_PLAY = 3;
int INO_n_FrameOffsetDueToOtherLoading = 0;

#define INPUT_REC_FILE "D:\\INPUT_REC_%08X_XE.ino"
#define INPUT_REC_END_FILE "D:\\END_%08X.txt"
#define NUMFRAMEBEFOREPLAY 10
#endif

#ifdef XENONVIDEOSTATISTICS
#include "GEOmetric/GEOstatistics.h"
#endif

extern ULONG BIG_ul_SearchKeyToFat (BIG_KEY);

#include "ENGine/Sources/ENGvars.h"
#include "BIGfiles\BIGdefs.h"
#include "BIGfiles\BIGfat.h"
#include "Xenon/Demo/DemoLaunchData.h"


#if !defined(_FINAL_)
#include "XenonGraphics/XeTrigger.h"
#include "XenonGraphics/XeRenderer.h"
#endif

/*$2- XENON PROFILING ---------------------------------------------------------------------------------------------*/
#include "XenonGraphics/XeProfiling.h"

extern int ControlMode;

// ***********************************************************************************************************************
//    Variables
// ***********************************************************************************************************************

BOOL					xeINO_bDesktop_InputLock	= FALSE;
BOOL					xeINO_bIsControllerLost		= FALSE;
int						xeINO_bVibrationsPaused		= 0;
float					xeINO_fLastInputTimeAttract	= 0.0f;
float					xeINO_fLastInputTimeDemo	= 0.0f; 

BOOL					xeINO_bMergedInput			= TRUE;

xeINO_stPadDescriptor	xeINO_aPadDescriptors[XUSER_MAX_COUNT];

// ***********************************************************************************************************************
//    Functions - Private
// ***********************************************************************************************************************

unsigned char xeINO_uc_GetButton(int Param_iPadID, int Param_iButtonID)
{
	// Error Checking

	if ((Param_iPadID < 0) || (Param_iPadID >= XUSER_MAX_COUNT) || !xeINO_aPadDescriptors[Param_iPadID].bIsConnected)
	{
		return kXeINO_iButtonInputLevel_Min;
	}

	// Fetch Button State

    switch(Param_iButtonID)
    {
    case eBtn_Cross: return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_A)									? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_Circle:	
        if (ControlMode != 2)            
            return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_B)								? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;            
        else            
            return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)								? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;            
      case eBtn_Triangle:	
        if (ControlMode != 2)
            return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)								? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
        else 
            return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_B)								? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;                
    case eBtn_Square:	return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_X)									? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;    
    case eBtn_L1:		return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)						? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_R1:		return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)					? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_L2:		return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.bLeftTrigger >= kXeINO_iTrigger_XState_BooleanSensitivity)	? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_R2:		return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.bRightTrigger >= kXeINO_iTrigger_XState_BooleanSensitivity)	? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_Select: return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)								? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_Start: return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_START)								? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_R3: return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)						? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_L3: return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)						? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_Up: return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)							? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_Right: return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)						? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_Left: return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)							? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    case eBtn_Down: return (xeINO_aPadDescriptors[Param_iPadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)							? kXeINO_iButtonInputLevel_Max : kXeINO_iButtonInputLevel_Min;
    default: return kXeINO_iButtonInputLevel_Min;
	}
}

// ***********************************************************************************************************************

void xeINO_Reset(BOOL Param_bEnable)
{
	L_memset(xeINO_aPadDescriptors, 0, XUSER_MAX_COUNT * sizeof(xeINO_stPadDescriptor));

	INO_gi_CurrentPadId		= kXeINO_iPadID_Invalid;
	INO_l_Joystick_Enable	= Param_bEnable;
	xeINO_bIsControllerLost	= FALSE;

	xeINO_ResetInactivityTimer( TimerAttractMode | TimerExitDemo );
}

// ***********************************************************************************************************************
//    Functions - Platform Specific
// ***********************************************************************************************************************

float xeINO_GetInactivitySeconds( ActivityTimer timer )
{
	if( timer == TimerAttractMode )
		return TIM_gf_MainClock - xeINO_fLastInputTimeAttract;
	else if( timer == TimerExitDemo )
		return TIM_gf_MainClock - xeINO_fLastInputTimeDemo;

	ERR_X_Assert( false );
	return 0.0f;
}

// ***********************************************************************************************************************
//    Functions - Platform Specific
// ***********************************************************************************************************************

void xeINO_SetInactivitySeconds( ActivityTimer timer, float fTime )
{
	if( timer == TimerAttractMode )
		xeINO_fLastInputTimeAttract = TIM_gf_MainClock - fTime;
	else if( timer == TimerExitDemo )
		xeINO_fLastInputTimeDemo = TIM_gf_MainClock - fTime;
	else
		ERR_X_Assert( false );
}

// ***********************************************************************************************************************

void xeINO_ResetInactivityTimer( int timers )
{
	if( timers & TimerAttractMode )
		xeINO_fLastInputTimeAttract = TIM_gf_MainClock;
	if( timers & TimerExitDemo )
		xeINO_fLastInputTimeDemo = TIM_gf_MainClock;
}

// ***********************************************************************************************************************

BOOL xeINO_IsControllerLost()
{
	return xeINO_bIsControllerLost;
}

// ***********************************************************************************************************************
//    Functions - Platform Generic
// ***********************************************************************************************************************

void xeINO_Joystick_Init(HWND Param_hHandle)
{
	xeINO_Reset(TRUE);
}

// ***********************************************************************************************************************

void xeINO_Joystick_Close()
{
	xeINO_Reset(FALSE);
}

// ***********************************************************************************************************************

void xeINO_Joystick_Update(INO_tdst_GenericPad* Param_pPad)
{
	BOOL bInputActive = FALSE;

	// Loop Through Gamepads
	for (int iLoop_PadID = 0; iLoop_PadID < XUSER_MAX_COUNT; iLoop_PadID++)
	{
		// Fetch State

        if (XInputGetState(iLoop_PadID, &xeINO_aPadDescriptors[iLoop_PadID].xState) == ERROR_SUCCESS)
		{
#ifdef JOYRECORDER
		    DWORD dwNbBytesWritten;

            if ((INO_b_RecordInput || INO_b_PlayInput) && INO_n_FrameCounter == 0) 
            {
                /* Create file at the same pos as exe */
                sprintf(INO_dumpFileName,INPUT_REC_FILE,WOR_gul_WorldKey);
                if(!(L_access(INO_dumpFileName, 0))) L_chmod(INO_dumpFileName, L_S_IREAD | L_S_IWRITE);
                
                if (INO_b_RecordInput)
                {
                    INO_dumpFile = CreateFile(  INO_dumpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );                                        
                    WriteFile( INO_dumpFile, &INO_n_FrameCounter, sizeof(INO_n_FrameCounter), &dwNbBytesWritten, NULL);                    
                }
                else
                {
                    INO_dumpFile = CreateFile(  INO_dumpFileName, GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL );
                    ReadFile(INO_dumpFile,&INO_n_NumRecord, sizeof(INO_n_NumRecord), &dwNbBytesWritten, NULL);

                    // Particular case when the recording was not stopped correctly
                    if (INO_n_NumRecord == 0)
                    {
                        DWORD dwFileSize = 0;
                        int nFileSize;

                        dwFileSize = GetFileSize(INO_dumpFile,LPDWORD(NULL));
                        nFileSize = (int)dwFileSize;
                        nFileSize -= sizeof(INO_n_NumRecord);

                        INO_n_NumRecord = nFileSize / sizeof(XINPUT_STATE);
                    }
                }
            }

            if (INO_b_RecordInput)
            {
                WriteFile( INO_dumpFile, (void *) &xeINO_aPadDescriptors[iLoop_PadID].xState, sizeof(XINPUT_STATE), &dwNbBytesWritten, NULL );                
                INO_n_NumRecord = INO_n_FrameCounter;                   
            }
            else if (INO_b_PlayInput)
            {   
                // Stop play if x is pressed 
                if (xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_X)
                {
                    INO_StopPlay();                    
                    CloseHandle( INO_dumpFile );
                    INO_dumpFile = NULL;
                }
                else
                {
                    ReadFile(INO_dumpFile,(void *) &xeINO_aPadDescriptors[iLoop_PadID].xState, sizeof(XINPUT_STATE), &dwNbBytesWritten, NULL);                                

                    if (INO_n_FrameCounter >= INO_n_NumRecord)
                    {
                        char INO_EndFile[128];
                        L_FILE INO_dumpEndFile;

                        /* Create file at the same pos as exe */
                        sprintf(INO_EndFile,INPUT_REC_END_FILE,WOR_gul_WorldKey);
                        if(!(L_access(INO_EndFile, 0))) L_chmod(INO_EndFile, L_S_IREAD | L_S_IWRITE);

                        INO_dumpEndFile = CreateFile(  INO_EndFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );        
                        WriteFile( INO_dumpEndFile, &INO_n_FrameCounter, sizeof(INO_n_FrameCounter), &dwNbBytesWritten, NULL);   
                        CloseHandle(INO_dumpEndFile);

                        INO_b_PlayInput = FALSE;
                    }
                }
            }
            else if (INO_n_FrameCounter != 0)
            {              
                if (INO_dumpFile != NULL)
                {          
                    // Seek at the begining of the file 
                    SetFilePointer(INO_dumpFile,0,0,FILE_BEGIN);
                    WriteFile( INO_dumpFile, &INO_n_FrameCounter, sizeof(INO_n_FrameCounter), &dwNbBytesWritten, NULL );
                    CloseHandle( INO_dumpFile );
                    INO_dumpFile = NULL;

#if !defined(_FINAL_)
					//close log file of TriggerManager
					if(INO_b_InputWillBePlayed)
						g_XeTriggerManager.b_CloseLogFile();                    
#endif	//_FINAL_

#ifdef XENONVIDEOSTATISTICS
                    XeGOStatistics *XeStats = XeGOStatistics::Instance();

                    if (XeStats->GetTriggerDumpStat())
                    {
                        XeStats->DumpStatToFile();
                    }                    
#endif
                }

                INO_n_FrameCounter = 0;

                if (INO_n_NumRepeat > 0)
                {
                    INO_StartPlay();
                    INO_n_NumRepeat--;
                }
            }
            
            if (INO_b_InputWillBePlayed || INO_b_InputWillBeRecorded)
            {                           
                if (ENG_ulCustomLevel == WOR_gul_WorldKey)
                {
                    INO_n_FrameOffsetDueToOtherLoading = g_oXeRenderer.GetFrameCounter();
                    ENG_ulCustomLevel = -1;
                }  

                if (ENG_ulCustomLevel == -1 && g_oXeRenderer.GetFrameCounter() == NUM_FRAME_BEFORE_PLAY + INO_n_FrameOffsetDueToOtherLoading)
                {                
                    if (INO_b_InputWillBePlayed)                    
					{
                        if (INO_n_NumRepeat == 999)
                        {
                            char INO_EndFile[128];
                            L_FILE INO_dumpEndFile;

                            /* Create file at the same pos as exe */
                            sprintf(INO_EndFile,INPUT_REC_END_FILE,WOR_gul_WorldKey);
                            if(!(L_access(INO_EndFile, 0))) L_chmod(INO_EndFile, L_S_IREAD | L_S_IWRITE);

                            INO_dumpEndFile = CreateFile(  INO_EndFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );        
                            WriteFile( INO_dumpEndFile, &INO_n_FrameCounter, sizeof(INO_n_FrameCounter), &dwNbBytesWritten, NULL);   
                            CloseHandle(INO_dumpEndFile);
                            INO_b_PlayInput = false;
                            INO_n_NumRepeat = 0;
                        }
                        else
                        {
                            INO_StartPlay();
                            INO_n_FrameCounter = -1;
                        }
					}
                    else
					{
						INO_StartRecord();                                          
						INO_n_FrameCounter = -1;
					}
                }    
            }
#endif

#if !defined(_FINAL_)
			// Hack for screen capture
			// We can't do this in AI since data merge from MTP erase our AI modification
			if (xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_Y &&
				xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
			{
				g_oXeRenderer.SetScreenCapture();
			}

			//Hack for dump of profiling data
			if (xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_Y &&
				xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			{
				g_oXeProfileManager.DumpProfilingData();
			}
#endif //_FINAL
			// Fetch Capabilities (If Applicable)

			if (XInputGetCapabilities(iLoop_PadID, XINPUT_FLAG_GAMEPAD, &xeINO_aPadDescriptors[iLoop_PadID].xCapabilities) == ERROR_SUCCESS)
			{
				// Flag Gamepad as "Connected"

				xeINO_aPadDescriptors[iLoop_PadID].bIsConnected = TRUE;

				// Assign Gamepad as Current (If applicable)

				if (INO_gi_CurrentPadId == kXeINO_iPadID_Invalid)
				{
					xeINO_bIsControllerLost	= FALSE;
					INO_gi_CurrentPadId		= iLoop_PadID;
				}
			}
		}
		else
		{
			// Flag Gamepad as "Not Connected"

			xeINO_aPadDescriptors[iLoop_PadID].bIsConnected = FALSE;

			// Remove Current Gamepad Assignment (If applicable)

			if (INO_gi_CurrentPadId == iLoop_PadID )
			{
				if( xeINO_bIsControllerLost == FALSE )
					xeINO_bIsControllerLost	= TRUE;
			}
		}

		// Update Button States (If Applicable)
		if (xeINO_aPadDescriptors[iLoop_PadID].bIsConnected)
		{
			for (int iLoop_Button = 0; iLoop_Button < eBtn_GenericButtonNb; iLoop_Button++)
				Param_pPad[iLoop_PadID].ac_Button[iLoop_Button] = xeINO_uc_GetButton(iLoop_PadID, iLoop_Button);

			// Detect Gamepad Activity
			BOOL bControllerHasActivity = FALSE;

			// - Digital Buttons
			if (xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.wButtons != 0)
				bControllerHasActivity = TRUE;

			// - Analog Triggers
			if ((xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.bLeftTrigger >= kXeINO_iTrigger_XState_BooleanSensitivity) || (xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.bRightTrigger >= kXeINO_iTrigger_XState_BooleanSensitivity))
				bControllerHasActivity = TRUE;

			// - Thumbstick (Left)
			if ((abs(xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.sThumbLX) >= kXeINO_iStick_XState_BooleanSensitivity) || (abs(xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.sThumbLY) >= kXeINO_iStick_XState_BooleanSensitivity))
				bControllerHasActivity = TRUE;

			// - Thumbstick (Right)
			if ((abs(xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.sThumbRX) >= kXeINO_iStick_XState_BooleanSensitivity) || (abs(xeINO_aPadDescriptors[iLoop_PadID].xState.Gamepad.sThumbRY) >= kXeINO_iStick_XState_BooleanSensitivity))
				bControllerHasActivity = TRUE;

			// flag input activity
			if( bControllerHasActivity )
				bInputActive = TRUE;

			if( iLoop_PadID == INO_gi_CurrentPadId && xeINO_bIsControllerLost )
				xeINO_bIsControllerLost	= FALSE;

			// if using merged input, keep track of last controller that was used for use as current controller
			if( xeINO_bMergedInput && bControllerHasActivity )
				INO_gi_CurrentPadId = iLoop_PadID;
		}
	}

	// Update Last Input Time (If Applicable)
	if (bInputActive)
	{
		xeINO_ResetInactivityTimer( TimerAttractMode | TimerExitDemo );

		if( CXeDemoLaunchData::GetDemoMode( ) == DEMO_MODE_KIOSK )
			CXeDemoLaunchData::SetDemoMode( DEMO_MODE_USER_SELECTED );
	}
}

// ***********************************************************************************************************************

void xeINO_Joystick_Move(MATH_tdst_Vector* Param_pMove, int Param_iStickNumber)
{
	// Param init
	Param_pMove->x = Param_pMove->y = Param_pMove->z = 0.0f;

	// Error Checking

	if (!xeINO_b_Joystick_IsValid())
	{
		return;
	}

	int nFirstController = INO_gi_CurrentPadId;
	int nLastController  = INO_gi_CurrentPadId;

	if( xeINO_bMergedInput )
	{
		nFirstController = 0;
		nLastController  = XUSER_MAX_COUNT - 1;
	}

	for( int nController = nFirstController; nController <= nLastController; ++nController )
	{
		if( !xeINO_aPadDescriptors[nController].bIsConnected )
			continue;

		// Fetch Thumbstick Position
		float x = 0.0f;
		float y = 0.0f;

		if ((Param_iStickNumber & 1) == 0)
		{
			// Left (ID 0 and all "Even" IDs)
			x = (xeINO_aPadDescriptors[nController].xState.Gamepad.sThumbLX) / kXeINO_fStick_XState_MaxAmplitude;
			y = (xeINO_aPadDescriptors[nController].xState.Gamepad.sThumbLY) / kXeINO_fStick_XState_MaxAmplitude;
		}
		else
		{
			// Right (ID 1 and all "Odd" IDs)
			x = (xeINO_aPadDescriptors[nController].xState.Gamepad.sThumbRX) / kXeINO_fStick_XState_MaxAmplitude;
			y = (xeINO_aPadDescriptors[nController].xState.Gamepad.sThumbRY) / kXeINO_fStick_XState_MaxAmplitude;
		}
        
        // In "special" mode, the horizontal controls are inversed. 
        if (GDI_gpst_CurDD->GlobalXInvert && (ControlMode!=2)) 
        {
            x = -x;
        }         

		if( abs(x) > abs(Param_pMove->x) )
			Param_pMove->x = x;

		if( abs(y) > abs(Param_pMove->y) )
			Param_pMove->y = y;
	}

	// Low-Level DeadZone (Prevents Tiny Stick Motion From Registering)
	if ((Param_pMove->x >= -kXeINO_fStick_DeadZone) && (Param_pMove->x <= kXeINO_fStick_DeadZone))
		Param_pMove->x = 0.0f;

	if ((Param_pMove->y >= -kXeINO_fStick_DeadZone) && (Param_pMove->y <= kXeINO_fStick_DeadZone))
		Param_pMove->y = 0.0f;
}

// ***********************************************************************************************************************

void xeINO_PadLowVibSet(BOOL Param_bEnable)
{
	// Error Checking

	if (!xeINO_b_Joystick_IsValid() || (xeINO_aPadDescriptors[INO_gi_CurrentPadId].xCapabilities.Vibration.wRightMotorSpeed == 0))
	{
		return;
	}

	// No vibration if paused
	if( xeINO_IsVibrationPaused( ) )
	{
		return;
	}

	// Determine Level

	if (Param_bEnable)
	{
		// Maximum

		xeINO_aPadDescriptors[INO_gi_CurrentPadId].xVibration.wRightMotorSpeed = 65535;
	}
	else
	{
		// Minimum

		xeINO_aPadDescriptors[INO_gi_CurrentPadId].xVibration.wRightMotorSpeed = 0;
	}

	// Update Gamepad Vibration

	bool bRetry = false;
	do
	{
		const DWORD dwResult = XInputSetState(INO_gi_CurrentPadId, &xeINO_aPadDescriptors[INO_gi_CurrentPadId].xVibration);
		bRetry = ( dwResult == ERROR_BUSY );
	}
	while( bRetry );
}

// ***********************************************************************************************************************

void xeINO_PadHighVibSet(int Param_iLevel)
{
	// Error Checking

	if (!xeINO_b_Joystick_IsValid() || (xeINO_aPadDescriptors[INO_gi_CurrentPadId].xCapabilities.Vibration.wLeftMotorSpeed == 0))
	{
		return;
	}

	// No vibration if paused
	if( xeINO_IsVibrationPaused( ) )
	{
		return;
	}

	// Calculate Level with a "pseudo" logarithmic interpolation
	Param_iLevel = lAbs(Param_iLevel);
	FLOAT fTemp  = ( 1.0f - ( ( FLOAT ) Param_iLevel / 255.0f) );
	int iLevel   = ( int ) ( 65535.0f * ( 1.0f - fTemp * fTemp * fTemp ) );

	xeINO_aPadDescriptors[INO_gi_CurrentPadId].xVibration.wLeftMotorSpeed = iLevel;
		//(WORD)((float)Param_iLevel * (float)xeINO_aPadDescriptors[INO_gi_CurrentPadId].xCapabilities.Vibration.wLeftMotorSpeed / 255.0f);

	// Update Gamepad Vibration

	bool bRetry = false;
	do
	{
		const DWORD dwResult = XInputSetState(INO_gi_CurrentPadId, &xeINO_aPadDescriptors[INO_gi_CurrentPadId].xVibration);
		bRetry = ( dwResult == ERROR_BUSY );
	}
	while( bRetry );
}

// ***********************************************************************************************************************

BOOL xeINO_b_Joystick_IsValid()
{
	// Error Checking

	if ((INO_gi_CurrentPadId == kXeINO_iPadID_Invalid) || !xeINO_aPadDescriptors[INO_gi_CurrentPadId].bIsConnected)
	{
		return FALSE;
	}

	return TRUE;
}

// ***********************************************************************************************************************

int xeINO_i_SetCurrentPad(int Param_iPadID)
{
	// Error Checking

	if ((Param_iPadID < 0) || (Param_iPadID >= XUSER_MAX_COUNT) || !xeINO_aPadDescriptors[Param_iPadID].bIsConnected)
	{
		return kXeINO_iPadID_Invalid;
	}

	// Assign New Pad ID

	INO_gi_CurrentPadId = Param_iPadID;
	return 0;
}

// ***********************************************************************************************************************

void xeINO_PauseVibration( )
{
	// Stop current vibrations
	ZeroMemory( &xeINO_aPadDescriptors[INO_gi_CurrentPadId].xVibration, sizeof( XINPUT_VIBRATION ) );
	XInputSetState( INO_gi_CurrentPadId, &xeINO_aPadDescriptors[INO_gi_CurrentPadId].xVibration );

	// Increment pause counter
	++xeINO_bVibrationsPaused;
}

// ***********************************************************************************************************************

void xeINO_ResumeVibration( )
{
	// Increment pause counter
	if( xeINO_bVibrationsPaused > 0 )
		--xeINO_bVibrationsPaused;
}

// ***********************************************************************************************************************

BOOL xeINO_IsVibrationPaused( )
{
	return xeINO_bVibrationsPaused > 0;
}

// ***********************************************************************************************************************

void xeINO_EnableMergedInput( bool bEnabled )
{
	xeINO_bMergedInput = bEnabled;
}

// ***********************************************************************************************************************

#endif // defined(_XENON)
