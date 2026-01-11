/*$T AIfunctions_Xenon.c GC!1.100 04/19/05 14:20:39 */

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include "Precomp.h"
#include "SDK\Sources\Xenon\Live\RichPresence.h"
#include "SDK\Sources\Xenon\Live\Achievements.h"
#include "SDK\Sources\Xenon\Profile\Profile.h"
#include "SDK\Sources\Xenon\Interf\BuiltInInterf.h"
#include "SDK\Sources\Xenon\Profile\Profile.h"
#include "SDK\Sources\Xenon\MenuManager\MenuManager.h"
#include "SDK\Sources\Xenon\Demo\DemoLaunchData.h"
#include "SDK\Sources\INOut\INOjoystick.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "XenonGraphics\XeSimpleRenderer.h"
#include "XenonGraphics\XeRenderer.h"

#ifdef _XENON
#include "SDK\Sources\INOut\Xenon\XeINOjoystick.h"
#include "SDK\Sources\INOut\Xenon\xeINOsaving.h"
#include "SDK\Sources\INOut\INOsaving.h"
#endif

#ifdef _XENON
int g_bForceProfileRefresh = 0;
bool g_bFoundNewDefaultDevice = false;
#endif

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_SetCheckPoint_C(int _i_Val)
{
	SetMap( _i_Val );
}
//
AI_tdst_Node *AI_EvalFunc_XE_SetCheckPoint(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_SetCheckPoint_C(AI_PopInt());
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_AddAchievement_C(int _i_Val)
{
	AddAchievement( _i_Val );
}
//
AI_tdst_Node *AI_EvalFunc_XE_AddAchievement(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_AddAchievement_C(AI_PopInt());
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_StartSession_C( )
{
#ifdef _XENON
	//g_XeLiveSession.ActivateSession( true );
#endif // _XENON
}
//
AI_tdst_Node *AI_EvalFunc_XE_StartSession(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_StartSession_C( );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_EndSession_C( )
{
#ifdef _XENON
	//g_XeLiveSession.ActivateSession( false );
#endif // _XENON
}
//
AI_tdst_Node *AI_EvalFunc_XE_EndSession(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_EndSession_C( );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_ChooseStorageDevice_C( )
{
#ifdef _XENON
	g_XeBuiltInInterf.ShowDeviceSelector( xeINO_GetSavegameSize( ) );
#endif // _XENON
}
//
AI_tdst_Node *AI_EvalFunc_XE_ChooseStorageDevice(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_ChooseStorageDevice_C( );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_ChooseStorageMode_C( int nMode )
{
#ifdef _XENON
	g_XeBuiltInInterf.SetDeviceSelectorMode( nMode );
#endif // _XENON
}
//
AI_tdst_Node *AI_EvalFunc_XE_ChooseStorageMode(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_ChooseStorageMode_C( AI_PopInt( ) );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_XE_GetProfileID_C( )
{
#ifdef _XENON
	const int nID = g_XeProfile.GetID( );
	return nID;
#endif // _XENON

	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_XE_GetProfileID(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_XE_GetProfileID_C( ));
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_XE_GetProfileSetting_C( int nParamID )
{
#ifdef _XENON

	// check if settings are available
	if( !g_XeProfile.IsReady( ) )
		return -1;

	// get requested setting
	switch( nParamID )
	{
		case 1: // difficulty
			return g_XeProfile.GetDifficulty( );

		case 2: // controller vibration
			return g_XeProfile.IsControllerVibrationEnabled( ) ? 1 : 0;

		case 3: // Y axis invertion
			return g_XeProfile.IsYAxisInverted( ) ? 1 : 0;
	}

	ERR_X_Assert( false );
	return -1; // unknown setting requested

#endif // _XENON

	return -1; // not defined if not running on Xenon
}
//
AI_tdst_Node *AI_EvalFunc_XE_GetProfileSetting(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~*/
	int		nParamID;
	int		nValue;
	/*~~~~~~~~~~*/

	nParamID = AI_PopInt();

	nValue = AI_EvalFunc_XE_GetProfileSetting_C( nParamID );
	AI_PushInt( nValue );

	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_StartProfiling_C( )
{
#ifdef _XENON
	ENG_gb_ProfileRunning = TRUE;
	OutputDebugStringA("Start Profiling\n");
#endif // _XENON
}
//
AI_tdst_Node *AI_EvalFunc_XE_StartProfiling(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_StartProfiling_C( );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_EndProfiling_C( )
{
#ifdef _XENON
	ENG_gb_ProfileRunning = FALSE;
	OutputDebugStringA("Stop Profiling\n");
#endif // _XENON
}
//
AI_tdst_Node *AI_EvalFunc_XE_EndProfiling(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_EndProfiling_C( );
	return ++_pst_Node;
}


/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_ChangeGamePhase_C( int nPhase )
{
#ifdef _XENON

	static int nCurrentPhase = -1;

	if( nPhase != nCurrentPhase )
	{
		switch( nPhase )
		{
			// in game menus
			case 0:
				// end session if one is active
				//g_XeLiveSession.ActivateSession( false );

				break;

			// in gameplay
			case 1:
				// start session
			//	const XUSER_SIGNIN_STATE nCurrentState = XUserGetSigninState( g_XeLiveSession.GetUserIndex( ) );
			//	if( nCurrentState != eXUserSigninState_NotSignedIn )
			//		g_XeLiveSession.ActivateSession( true );
			//	else
			//		g_XeLiveSession.ActivateSession( false );

				break;
		}

		nCurrentPhase = nPhase;
	}

#endif // _XENON
}
//
AI_tdst_Node *AI_EvalFunc_XE_ChangeGamePhase(AI_tdst_Node *_pst_Node)
{
	int nPhase = AI_PopInt( );
	AI_EvalFunc_XE_ChangeGamePhase_C( nPhase );

	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void AI_EvalFunc_XE_BeginMapLoad_C( )
{
#ifdef _XENON
	g_oXeSimpleRenderer.Activate( );
#endif
}

//
AI_tdst_Node *AI_EvalFunc_XE_BeginMapLoad(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_BeginMapLoad_C( );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_EndMapLoad_C( )
{
#ifdef _XENON
	if( !g_oXeSimpleRenderer.IsActive( ) )
		return;

	g_oXeSimpleRenderer.Deactivate( );
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_EndMapLoad(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_EndMapLoad_C( );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_XE_IsDemo_C( )
{
#ifdef _XENON
	return g_MenuManager.IsInDemoMode_NoStorageAccess( );
#else
	return FALSE;
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_IsDemo(AI_tdst_Node *_pst_Node)
{
	AI_PushInt( AI_EvalFunc_XE_IsDemo_C( ) );
	return ++_pst_Node;
}

//XE_Is_GameStatedInDemoMode
int AI_EvalFunc_XE_Is_GameStartedInDemoMode_C( )
{
#ifdef _XENON
    return CXeDemoLaunchData::IsLaunchedInDemoMode();
#else
    return FALSE;
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_Is_GameStartedInDemoMode(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( AI_EvalFunc_XE_Is_GameStartedInDemoMode_C( ) );
    return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_XE_IsHiDef_C( )
{
#ifdef _XENON
    if (g_oXeRenderer.GetVideoMode()->fIsWideScreen)
        return TRUE;    
#endif
    return FALSE;
}
//
AI_tdst_Node *AI_EvalFunc_XE_IsHiDef(AI_tdst_Node *_pst_Node)
{
    AI_PushInt( AI_EvalFunc_XE_IsHiDef_C() );
    return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_XE_GetScreenHeight_C( )
{
#ifdef _XENON
	UINT iResWidth;
	UINT iResHeight;

	g_oXeRenderer.GetBackbufferResolution(&iResWidth,&iResHeight);
	return iResHeight;
#else
	return 480;
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_GetScreenHeight(AI_tdst_Node *_pst_Node)
{
	AI_PushInt( AI_EvalFunc_XE_GetScreenHeight_C() );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_OpenSignInInterf_C( )
{
#ifdef _XENON
	g_XeBuiltInInterf.ShowProfileSelector( );
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_OpenSignInInterf(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_OpenSignInInterf_C( );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_XE_GetStorageDevice_C( )
{
#ifdef _XENON
	return g_XeBuiltInInterf.GetSelectedDeviceID( );
#else
	return 0;
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_GetStorageDevice(AI_tdst_Node *_pst_Node)
{
	AI_PushInt( AI_EvalFunc_XE_GetStorageDevice_C( ) );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
char* AI_EvalFunc_XE_GetStorageDeviceName_C( )
{
#ifdef _XENON
	static char szAnsiName[ 128 ];

	if( g_XeBuiltInInterf.GetSelectedDeviceID( ) != -1 )
	{
		WCHAR szName[ 128 ];
		if( XContentGetDeviceName( g_XeBuiltInInterf.GetSelectedDeviceID( ), szName, 128 ) == ERROR_SUCCESS )
		{
			WideCharToMultiByte(CP_ACP, 0, szName, -1, szAnsiName, sizeof(szAnsiName), NULL, NULL);
			return szAnsiName;
		}
	}
#endif

	return "";
}
//
AI_tdst_Node *AI_EvalFunc_XE_GetStorageDeviceName(AI_tdst_Node *_pst_Node)
{
	AI_PushString( AI_EvalFunc_XE_GetStorageDeviceName_C( ) );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_XE_ForceProfileRefresh_C( )
{
#ifdef _XENON
	int nValue = g_bForceProfileRefresh;
	g_bForceProfileRefresh = 0;
	return nValue;
#endif

	return 0;
}
//
AI_tdst_Node *AI_EvalFunc_XE_ForceProfileRefresh(AI_tdst_Node *_pst_Node)
{
	AI_PushInt( AI_EvalFunc_XE_ForceProfileRefresh_C( ) );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_LockGamePad_C( )
{
#ifdef _XENON
	// stop merged input
	xeINO_EnableMergedInput( false );

	// use last controller where there was some activity for duration of gameplay
	const int nUser = INO_i_GetCurrentPad( );
	g_XeProfile.SetUserIndex( nUser );
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_LockGamePad(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_LockGamePad_C( );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_OutputDebugString_C( const char* szText )
{
	const char* szRemaining = szText;

	const char* szReturn = NULL;
	while( ( szReturn = strstr( szRemaining, "\\n" ) ) != NULL )
	{
		std::string str;
		str.assign( szRemaining, szReturn );
		OutputDebugString( str.c_str( ) );
		OutputDebugString( "\n" );
		szRemaining = szReturn + 2;
	}

	if( *szRemaining != '\0' )
        OutputDebugString( szRemaining );
}
//
AI_tdst_Node *AI_EvalFunc_XE_OutputDebugString(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_OutputDebugString_C( AI_PopStringPtr( ) );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_OutputDebugInt_C( int nNumber )
{
	char szNumber[ 64 ];
	itoa( nNumber, szNumber, 10 );

	OutputDebugString( szNumber );
}
//
AI_tdst_Node *AI_EvalFunc_XE_OutputDebugInt(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_OutputDebugInt_C( AI_PopInt( ) );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_QuitGame_C( )
{
#ifdef _XENON
	CXeDemoLaunchData::ReturnToLauncher( );
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_QuitGame(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_QuitGame_C( );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_PauseVibrations_C( int nEnablePause )
{
#ifdef _XENON
	if( nEnablePause )
		xeINO_PauseVibration( );
	else
		xeINO_ResumeVibration( );
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_PauseVibrations(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_PauseVibrations_C( AI_PopInt( ) );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_XE_DeviceHasEnoughFreeSpace_C( void )
{
#ifdef _XENON
	if( g_XeBuiltInInterf.GetSelectedDeviceID() != -1 )
	{
		XDEVICE_DATA devicedata;
		XContentGetDeviceData( g_XeBuiltInInterf.GetSelectedDeviceID(), &devicedata );

		if( xeINO_SavegameExists( INO_gst_SavManager.i_CurrentSlot ) )
			devicedata.ulDeviceFreeBytes += xeINO_GetSavegameSize( );

		if( devicedata.ulDeviceFreeBytes < xeINO_GetSavegameSize( ) )
			return false;

		return true;
	}

	return false;
#else
	return true;
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_DeviceHasEnoughFreeSpace(AI_tdst_Node *_pst_Node)
{
	AI_PushInt( AI_EvalFunc_XE_DeviceHasEnoughFreeSpace_C( ) );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int AI_EvalFunc_XE_NewDefaultDevice_C( void )
{
#ifdef _XENON
	bool bNewDefaultDevice = g_bFoundNewDefaultDevice;

	if( g_XeBuiltInInterf.GetDeviceSelectorMode( ) != 1 )
		g_bFoundNewDefaultDevice = false;

	return bNewDefaultDevice;
#else
	return false;
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_NewDefaultDevice(AI_tdst_Node *_pst_Node)
{
	AI_PushInt( AI_EvalFunc_XE_NewDefaultDevice_C( ) );
	return ++_pst_Node;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void AI_EvalFunc_XE_SetWorldID_C( int nWorldID )
{
#ifdef _XENON
	g_oXeSimpleRenderer.SetLoadingWorldID( nWorldID );
#endif
}
//
AI_tdst_Node *AI_EvalFunc_XE_SetWorldID(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_XE_SetWorldID_C( AI_PopInt( ) );
	return ++_pst_Node;
}
