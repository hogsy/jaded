//////////////////////////////////////////////////////////////////////////
// Xenon Notification Manager
// --------------------------
// Checks for system notifications and dispatches them appropriately
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"
#if defined(_XENON)

#include "Notifications.h"
#include "Session.h"

#include "..\Interf\MessageBar.h"
#include "..\Interf\MessageType.h"
#include "..\Interf\BuiltInInterf.h"

#include "..\MenuManager\MenuManager.h"
#include "..\Profile\Profile.h"

#include "INOut\Xenon\XeINOjoystick.h"
#include "INOut\Xenon\XeINOsaving.h"
#include "Xenon\Demo\DemoLaunchData.h"

#include "XenonGraphics/XeSimpleRenderer.h"

using namespace MessageType;

//////////////////////////////////////////////////////////////////////////
// macros

#define CONCERNS_USER( x ) ( ulParam & ( 1 << (x) ) )

//////////////////////////////////////////////////////////////////////////
// global data

CXeNotificationManager g_XeNotificationManager;

extern BOOL     ENG_gb_ForcePauseEngine;
extern BOOL     ENG_gb_InPause;

extern void AI_EvalFunc_WORPause_C(int world, ULONG ul_ID);
extern void AI_EvalFunc_WORUnPause_C(int world, ULONG ul_ID);

//////////////////////////////////////////////////////////////////////////
// CXeNotificationManager

CXeNotificationManager::CXeNotificationManager( )
	: m_hNotification( INVALID_HANDLE_VALUE ),
      m_bForceUnPause( FALSE )
{
}

CXeNotificationManager::~CXeNotificationManager( )
{
	Shutdown( );
}

bool CXeNotificationManager::Initialize( )
{
	// notification listener
	m_hNotification = XNotifyCreateListener( XNOTIFY_SYSTEM | XNOTIFY_LIVE );
	if( m_hNotification == NULL || m_hNotification == INVALID_HANDLE_VALUE )
		return false;

	return true;
}

bool CXeNotificationManager::Shutdown( )
{
	if( m_hNotification != NULL && m_hNotification != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_hNotification );
		m_hNotification = INVALID_HANDLE_VALUE;
	}

	return true;
}

void CXeNotificationManager::CheckForNotifications( )
{
	DWORD dwNotificationId;
	ULONG ulParam;

	if( XNotifyGetNext( m_hNotification, 0, &dwNotificationId, &ulParam ) )
	{
		switch( dwNotificationId )
		{
			case XN_SYS_SIGNINCHANGED:
			{
				g_XeProfile.UpdateProfileSettings( );
				xeINO_FlagNewUser( );
				break;
			}

			case XN_LIVE_CONNECTIONCHANGED:
			{
				bool bConnected = false;

				switch( ulParam )
				{
					case XONLINE_S_LOGON_CONNECTION_ESTABLISHED:
						bConnected = true;
						break;

					/*
					case XONLINE_S_LOGON_CONNECTION_ESTABLISHED:
						bConnected = true;
						CXeMessageBarManager::Get( ).AddMessage( 2000, LIVE_CONNECTED );
						break;

					case XONLINE_E_LOGON_NO_NETWORK_CONNECTION:
						CXeMessageBarManager::Get( ).AddMessage( 3000, LIVE_CANT_CONNECT, LIVE_NO_NETWORK_CONNECTION );
						break;

					case XONLINE_E_LOGON_CANNOT_ACCESS_SERVICE:
						CXeMessageBarManager::Get( ).AddMessage( 3000, LIVE_CANT_CONNECT, LIVE_CANT_ACCESS_SERVICE );
						break;

					case XONLINE_E_LOGON_UPDATE_REQUIRED:
						CXeMessageBarManager::Get( ).AddMessage( 3000, LIVE_CANT_CONNECT, LIVE_UPDATE_REQUIRED );
						break;

					case XONLINE_E_LOGON_SERVERS_TOO_BUSY:
						CXeMessageBarManager::Get( ).AddMessage( 3000, LIVE_CANT_CONNECT, LIVE_SERVER_BUSY );
						break;

					case XONLINE_E_LOGON_INVALID_USER:
						CXeMessageBarManager::Get( ).AddMessage( 3000, LIVE_CANT_CONNECT, LIVE_INVALID_USER );
						break;

					case XONLINE_E_LOGON_CONNECTION_LOST:
						CXeMessageBarManager::Get( ).AddMessage( 2000, LIVE_CONNECTION_LOST );
						break;

					case XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON:
						CXeMessageBarManager::Get( ).AddMessage( 2000, LIVE_DUPLICATE_LOGON );
						break;
					*/
				}

				g_XeLiveConnected = bConnected;

				break;
			}

			case XN_SYS_UI:
			{
				if( ulParam != 0 )
				{
					// opened interface
					g_MenuManager.DisableAttractMode( TRUE );

					// pause game
					ENG_gb_ForcePauseEngine = TRUE;

                    if (!ENG_gb_InPause && !g_oXeSimpleRenderer.IsActive())
                    {
                        m_bForceUnPause = TRUE;

                        AI_EvalFunc_WORPause_C(0, 0xffffffff);
                    }

					// pause vibrations
					xeINO_PauseVibration();

					// don't reboot while interf is open
					CXeDemoLaunchData::DisableReboot( true );
				}
				else
				{
					// closed interface
					g_MenuManager.DisableAttractMode( FALSE );
					xeINO_ResetInactivityTimer( TimerAttractMode | TimerExitDemo );

					g_XeBuiltInInterf.SetDeviceSelectorMode( 0 );

					// resume game
					ENG_gb_ForcePauseEngine = FALSE;

                    if (m_bForceUnPause)
                    {
                        AI_EvalFunc_WORUnPause_C(0, 0xffffffff);
                    }
                    m_bForceUnPause = FALSE;

					// resume vibrations
					xeINO_ResumeVibration();

					// can now reboot properly
					CXeDemoLaunchData::DisableReboot( false );
				}
				break;
			}

			case XN_SYS_STORAGEDEVICESCHANGED:
			{
				if( g_XeBuiltInInterf.GetDeviceSelectorMode( ) != 1 )
				{
					if( g_XeBuiltInInterf.GetSelectedDeviceID( ) != -1 )
					{
						DWORD dwState = XContentGetDeviceState( g_XeBuiltInInterf.GetSelectedDeviceID( ), NULL );

						if( dwState != S_OK )
							g_XeBuiltInInterf.InvalidateSelectedDevice( );
					}
				}

				break;
			}
		}
	}
}

#endif // defined(_XENON)
