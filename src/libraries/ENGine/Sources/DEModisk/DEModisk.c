#include "Precomp.h"

#if defined(_XBOX)
#include <xtl.h>
#include <xbox.h>
#include <xmv.h>

#include "SouND/Sources/Xbox/xbSND.h"
#include "Gx8/Gx8HandleNoPad.h"
#endif

#include "DEModisk.h"
#include "INOut/INOjoystick.h"
#include "INOut/INO.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDtrack.h"

#if defined(_XBOX)
LAUNCH_DATA gdemoLaunchData;
LD_DEMO		gDemoParams;
LARGE_INTEGER	qwElapsedTime;
LARGE_INTEGER	qwElapsedAppTime;
LARGE_INTEGER	qwAppTime;
LARGE_INTEGER	qwLastTime;
LARGE_INTEGER	qwTimerFreq;
#endif

DWORD		gLaunchDataType;
float		m_fTimerPeriod;
CHAR		gLaunchPath[64];
BOOL		gPlayAttractMode = FALSE;
int			gAttractModeLoopTime = 60*1000;
int			gEnableAttractMode = 0;
int			gDontIncrementTimer = 0;

int			gNoPadConnected = 0;


int writeNOpad = 0;
int isPadConnected;

LONG INO_l_JoystickMask_old = -1;
extern LONG INO_l_JoystickMask;

#define		ATTRACT_MODE_VIDEO_FILENAME	"d:\\attractmode.xmv"

void DEM_DontIncrementTimer(int newstate)
{
 gDontIncrementTimer= newstate;
}

DWORD DEM_GetTimeOut(void)
{
#if defined(_XBOX)
	return gDemoParams.dwTimeout/1000;
#else
	return 4*60;
#endif
}

DWORD DEM_GetAttrackMode(void)
{
#if defined(_XBOX)
	if (gDemoParams.dwRunmode == XLDEMO_RUNMODE_KIOSKMODE)
	{
		return 1;
	}
#endif
	return 0;

}

DWORD DEM_IsDemoDisk(void)
{
    return INO_isRunMode_Demo();

}



BOOL DEM_InitDemoDisk(void)
{
    //if (INO_getRunMode()==INO_tden_RunMode_Undefined)
	INO_setRunMode( INO_tden_RunMode_Game );

#if defined(_XBOX)

	memset(gLaunchPath,0,64);

	if(XGetLaunchInfo(&gLaunchDataType, &gdemoLaunchData) != ERROR_SUCCESS)
		return FALSE;


	if(gLaunchDataType == LDT_FROM_DASHBOARD)
	{
		//game launched from dashboard... no demosik
        INO_setRunMode( INO_tden_RunMode_Game );

		memset(&gDemoParams,0,sizeof(LAUNCH_DATA));

		return FALSE;
	}
	else if(gLaunchDataType == LDT_TITLE)
	{
		//demodisk valid
        INO_setRunMode( INO_tden_RunMode_Demo );
        //INO_setRunMode( INO_tden_RunMode_Game );

		memcpy(&gDemoParams,&gdemoLaunchData,sizeof(LAUNCH_DATA));
		//XBUtil_SetMediaPath();

		return TRUE;
	}
/*	else if(gLaunchDataType == 3)
	{
		//demodisk valid
        INO_setRunMode( INO_tden_RunMode_Game );

		memcpy(&gDemoParams,&gdemoLaunchData,sizeof(LAUNCH_DATA));

		return TRUE;
	}*/

#endif
	//Mode not recognized
	return FALSE;
}


BOOL DEM_ReturnToDash(void)
{
#if defined(_XBOX)
	if( INO_isRunMode_Demo() )
    {
		XLaunchNewImage(gDemoParams.szLauncherXBE,&gdemoLaunchData);
    }

	//Should not arrive there!
	return TRUE;
#else
	return FALSE;
#endif
}

void DEM_StartTimer(void)
{
#if defined(_XBOX)

	LARGE_INTEGER qwTime;

	// Get the timer frequency (ticks per second)
    QueryPerformanceFrequency( &qwTimerFreq );

    // Store as period to avoid division in GetElapsed()
    m_fTimerPeriod = 1.0f / (FLOAT)( qwTimerFreq.QuadPart );

	QueryPerformanceCounter( &qwTime );
	qwElapsedTime.QuadPart = 0;
	qwLastTime.QuadPart    = qwTime.QuadPart;
	qwAppTime.QuadPart     = 0;
#endif
}

void DEM_ResetTimer(void)
{
#if defined(_XBOX)
	qwAppTime.QuadPart     = 0;
#endif
}


float DEM_GetTimer(void)
{
#if defined(_XBOX)

	LARGE_INTEGER qwTime;
	FLOAT fSeconds;

	QueryPerformanceCounter( &qwTime );
	qwElapsedTime.QuadPart = qwTime.QuadPart - qwLastTime.QuadPart;
	qwLastTime.QuadPart    = qwTime.QuadPart;
	qwElapsedAppTime.QuadPart = qwElapsedTime.QuadPart;

	if (!gDontIncrementTimer)
	{
	 qwAppTime.QuadPart    += qwElapsedAppTime.QuadPart;
	}

	 // Convert to floating pt
    fSeconds = (FLOAT)( qwAppTime.QuadPart ) * m_fTimerPeriod;

	//convert to milliseconds
	return fSeconds * 1000.0f;
#else
	return 0.0f;
#endif
}

BOOL DEM_ManageDemoDisk(void)
{
#if defined(_XBOX)
		

/*	if (gEnableAttractMode)
	{
			DEM_PlayVideo(ATTRACT_MODE_VIDEO_FILENAME);
			gEnableAttractMode = 0;
	}*/
    //-------------------------------------------------------------------------------
	//DEMO DISK management
	if(! INO_isRunMode_Demo() )
		return FALSE;

	//Check if it's time to come back to the launcher
	if((DEM_GetTimer()>gDemoParams.dwTimeout) && (gDemoParams.dwTimeout!=0))
	{
		//time to go back...
		DEM_ReturnToDash();
	}

#endif

	return TRUE;
}

BOOL TIZ_DEM_ManageDemoDisk(void)
{
#if defined(_XBOX)
	//Normal case: game loop with attract mode
	//this is the normal loop for attract mode in game

	
	if((DEM_GetTimer()>gAttractModeLoopTime) && (gDemoParams.dwRunmode == 0))
	{
		DEM_PlayVideo( ATTRACT_MODE_VIDEO_FILENAME );
		//Reset the demo timer
		DEM_ResetTimer();

	return TRUE;
}


	//-------------------------------------------------------------------------------
	//DEMO DISK management
	if(!INO_isRunMode_Demo() )
		return FALSE;

	if(gDemoParams.dwRunmode == XLDEMO_RUNMODE_KIOSKMODE )
	{
		DEM_PlayVideo( ATTRACT_MODE_VIDEO_FILENAME );
	} 
	else if(gDemoParams.dwRunmode == XLDEMO_RUNMODE_USERSELECTED )
	{
	}

	//Check if it's time to come back to the launcher
	if((DEM_GetTimer()>gDemoParams.dwTimeout) && (gDemoParams.dwTimeout!=0))
	{
		//time to go back...
		DEM_ReturnToDash();
	}

#endif
	return TRUE;
}

char *DEM_GetLaunchPath()
{
#if defined(_XBOX)
	INT	StrLen = strlen(gDemoParams.szLaunchedXBE);

	if(! INO_isRunMode_Demo() )
	return NULL;

	if(StrLen<=0)
		return NULL;

	while(((StrLen--)>0) && gDemoParams.szLaunchedXBE[StrLen]!='\\' && gDemoParams.szLaunchedXBE[StrLen]!='/');

	if(StrLen>0)
		strncpy(gLaunchPath,gDemoParams.szLaunchedXBE,StrLen+1);

	return gLaunchPath;
#else
	return NULL;
#endif
}

#if defined(_XBOX)

DWORD __stdcall MoviePlayerThread( void* pMovieData )
{
	XMVDecoder *pDecoder;
    pDecoder = ( XMVDecoder* )pMovieData;

    // Play the movie
    XMVDecoder_Play( pDecoder, XMVFLAG_NONE, NULL );

    return 0;
}
#endif

HRESULT DEM_PlayVideo( const CHAR* strFilename )
{
/*#if defined(_XBOX) 

	HRESULT hr;
    XMVVIDEO_DESC VideoDescriptor;
	HANDLE hThread;
    // Greatly simplified function for playing an XMV
    XMVDecoder* pDecoder = NULL;
	char	FinalFilename[255];

	//First check if the attract mode is in the right state
	if(gEnableAttractMode==0)
	return 0;

	memset(FinalFilename,0,sizeof(FinalFilename));
	if(DEM_GetLaunchPath()==NULL)
	{
		strcpy(FinalFilename,strFilename);
	}
	else
	{
		strcpy(FinalFilename,gLaunchPath);
		strcat(FinalFilename,&strFilename[3]);
}

	gPlayAttractMode = TRUE;

    hr = XMVDecoder_CreateDecoderForFile( 0, FinalFilename, &pDecoder );
    if( FAILED( hr ) )  
        return hr;

    // Play the video
    XMVDecoder_GetVideoDescriptor( pDecoder, &VideoDescriptor );

    // Enable audio if there is any.
    if( VideoDescriptor.AudioStreamCount )
    {
        XMVDecoder_EnableAudioStream( pDecoder, 0, 0, NULL, NULL );
    }

    // Start the movie playing thread - this should always succeed.
    hThread = CreateThread( 0, 0, &MoviePlayerThread, pDecoder, 0, 0 );

    // Loop waiting for the user to press A or the movie to exit.
    // Resource loading or other activity can be placed here.
    for ( ;; )
    {
        // Wait a little while, or until the movie thread exits.
        // Can do useful work here.
        DWORD waitResult = WaitForSingleObject( hThread, 1000 / 60 );

        // WAIT_OBJECT_0 means the thread exited and we should exit.
        if ( waitResult == WAIT_OBJECT_0 )
            break;

		if(gPlayAttractMode == FALSE)
		{
			XMVDecoder_TerminatePlayback(pDecoder);
			goto exit;
		}

        // Refresh the input data.
		INO_Joystick_Update();
    }

exit:
    // We have to make sure the thread is terminated *before* we close the movie
    // decoder, to make sure it has stopped referencing the movie player object.

    // Wait for the thread to terminate.
    WaitForSingleObject( hThread, INFINITE );

    // Clean up our thread handles to free all thread resources.
    // This has to be done after we finish waiting on the handle.
    CloseHandle( hThread );

    // Free all of the movie data.
    XMVDecoder_CloseDecoder( pDecoder );

    return hr;
#else
	return 0;
#endif
*/
return 0; //a vier
}

void DEM_StopAttractMode()
{
#if defined( _XBOX )
	gPlayAttractMode = FALSE;
#endif
}

void DEM_AttractMode(int OnOff)
{
#if defined( _XBOX )
	gEnableAttractMode = OnOff;
#endif
}


#define XBSAVE_BLOCK_NEEDED 1

void DEM_GoToDashBoard(void)
{
#ifdef _XBOX
	//LAUNCH THE DASHBOARD
	LD_LAUNCH_DASHBOARD LaunchDash;
    LaunchDash.dwReason = XLD_LAUNCH_DASHBOARD_MEMORY;
    LaunchDash.dwContext = 0;
    LaunchDash.dwParameter1 = (int)('U');
    LaunchDash.dwParameter2 = XBSAVE_BLOCK_NEEDED;
	XLaunchNewImage( NULL, (PLAUNCH_DATA)(&LaunchDash) );
#endif
}





void DEM_NoPadConnected(int OnOff)
{
#if defined( _XBOX )
	gNoPadConnected = OnOff;
#endif
}

static bool DEM_IsStartPressed(void)
{
	return INO_b_Joystick_IsButtonJustDown(9);
}

static bool DEM_IsStartReleased( void )
{
	return INO_b_Joystick_IsButtonJustUp(9);
}

extern bool xbINO_Joystick_IsJoyValid();
extern bool xbINO_Joystick_IsJoyValid2();
extern bool xbINO_CheckFirstPadConnected();
extern BOOL TwoPadGame;

static bool DEM_IsJoyValid(void)
{
	/*if(INO_i_GetCurrentPad()<0)
		return false;
	else
		return true;
	*/

	/*
	if(xbINO_Joystick_IsJoyValid())
		return true;
	else
		return false;
	*/

	if (TwoPadGame)
	// if a two pads are required check if at least 2 pads are inserted
		return (xbINO_Joystick_IsJoyValid2()>1);
	else
	// if a single pad is required check if at least 1 pad is inserted
		return (xbINO_Joystick_IsJoyValid2()>0);
}


#if defined( _XBOX )

unsigned char xbINO_uc_GetButton(int port, int button);

int GetStartPressed()
{
	int counter;
	unsigned char button;
	for(counter=0;counter<4;++counter)
	{
		button=xbINO_uc_GetButton(counter, 9);
		if(button)
			return counter;
	}
	return -1;
}

#endif

enum NoPadState
{
    NO_PAD_STATE_INIT,
    NO_PAD_STATE_OK,
    NO_PAD_STATE_WAIT_RECONNECT,
    NO_PAD_STATE_WAIT_START,
    NO_PAD_STATE_START
};

void DEM_ManageNO_PAD(void)
{
#if defined( _XBOX )
//	static int NoValid=0;
static enum NoPadState status = NO_PAD_STATE_INIT;
extern BOOL ENG_gb_ForcePauseEngine;
extern int					INO_gi_CurrentPadId;
extern int					INO_gi_CurrentFirstPadId;
extern int					INO_gi_CurrentSecondPadId;

static int currPadID=999;
int numPad;
static bool UpdateFirstPad=false;
static int  SavedSecondPadId=-1;
int nOfPads;

    switch( status )
    {
        case NO_PAD_STATE_INIT:
            if( xbINO_b_Joystick_IsAssigned() )
            {
                status = NO_PAD_STATE_OK;
            }
            break;
        case NO_PAD_STATE_OK:

			if(currPadID==999)
				currPadID=INO_i_GetCurrentPad();
            
			// if not enough pads or pad1 has changed or pad2 has changed
			if( !(nOfPads=DEM_IsJoyValid()) || currPadID!=INO_i_GetCurrentPad() || (TwoPadGame && (INO_gi_CurrentSecondPadId!=SavedSecondPadId)) )
            {
				
                Gx8_HandleNoPad_setMessage(GX8_NO_PAD_RECONNECT);
				INO_l_JoystickMask_old = INO_l_JoystickMask;
				INO_l_JoystickMask |= 0x200;

                SND_MuteAll(TRUE);
                SND_TrackPauseAll(TRUE);
                SND_Update(NULL);
                ENG_gb_ForcePauseEngine = true;
                writeNOpad = 1;

				// verify if the disconnected pad is the first or the second
				UpdateFirstPad=xbINO_CheckFirstPadConnected();

                status = NO_PAD_STATE_WAIT_RECONNECT;
            }
			// update second pad id memory (to check when it change)
			if (nOfPads>1)
				SavedSecondPadId=INO_gi_CurrentSecondPadId;
            break;
        case NO_PAD_STATE_WAIT_RECONNECT:
            if( DEM_IsJoyValid() )
            {
				currPadID=INO_i_GetCurrentPad();
                Gx8_HandleNoPad_setMessage(GX8_NO_PAD_PRESS_START);
                status = NO_PAD_STATE_WAIT_START;
            }
            break;
        case NO_PAD_STATE_WAIT_START:
			numPad=GetStartPressed();
            if( !DEM_IsJoyValid() )
            {
                Gx8_HandleNoPad_setMessage(GX8_NO_PAD_RECONNECT);
                status = NO_PAD_STATE_WAIT_RECONNECT;
            }
           // else if( DEM_IsStartPressed() )
			else if(numPad>=0)
			// control that the start button pressed is the right one (not of already valid pads)
			if ((!TwoPadGame) || (UpdateFirstPad && numPad!=INO_gi_CurrentSecondPadId) || (!UpdateFirstPad && numPad!=INO_gi_CurrentFirstPadId))
			{
                Gx8_HandleNoPad_setMessage(GX8_NO_PAD_NULL);
				INO_l_JoystickMask= INO_l_JoystickMask_old;
                SND_MuteAll(FALSE);
                SND_TrackPauseAll(FALSE);
                SND_Update(NULL);
                status = NO_PAD_STATE_START;
				
				//if single pad is required
				if (!TwoPadGame)
				{
					currPadID=INO_gi_CurrentFirstPadId=INO_gi_CurrentPadId=numPad;
					// if first pad is equal second pad id, invalidate second pad id
					if (INO_gi_CurrentFirstPadId==INO_gi_CurrentSecondPadId)
						INO_gi_CurrentSecondPadId=-1;
				}
				else
				// if the first pad has to be updated
				if (UpdateFirstPad) 
				{
					currPadID=INO_gi_CurrentFirstPadId=INO_gi_CurrentPadId=numPad;
					// if first pad is equal second pad id, invalidate second pad id
					if (INO_gi_CurrentFirstPadId==INO_gi_CurrentSecondPadId)
						INO_gi_CurrentSecondPadId=-1;
				}
				// if the second pad has to be updated
				else
					INO_gi_CurrentSecondPadId=SavedSecondPadId=numPad;
            }
            break;
        case NO_PAD_STATE_START:
            //if( DEM_IsStartReleased() )
            {
                ENG_gb_ForcePauseEngine = false;
                status = NO_PAD_STATE_OK;
                writeNOpad = 0;

            }
            break;
        default:
            ERR_X_Assert( false );
    }
	
	if (status==NO_PAD_STATE_OK)
		TwoPadGame=FALSE;

#endif
}
