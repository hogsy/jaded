//////////////////////////////////////////////////////////////////////////
// Xenon Session
// ------------------------
// Manages the Xbox Live session
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"
#if defined(_XENON)

#include "Session.h"

bool g_XeLiveConnected = false;


// TCR modification (August 2005)
// it is no longer necessary to create a session for single player games


/*

#if defined(_XENON)
#include "..\XLAST\KingKong.spa.h"
#endif

#include "..\Interf\MessageBar.h"

//--------------------------------------------------------------------------------------
// Title ID used to log onto Live.
//--------------------------------------------------------------------------------------
#include <xtitleidbegin.h>
	XEX_TITLE_ID(0x55530050)
#include <xtitleidend.h>

//////////////////////////////////////////////////////////////////////////
// global data

CXeLiveSession g_XeLiveSession;

//////////////////////////////////////////////////////////////////////////
// type definitions

enum
{
	NO_SESSION,
	CREATING_SESSION,
	JOINING_SESSION,
	STARTING_SESSION,
	SESSION_ACTIVE,
	ENDING_SESSION,
	LEAVING_SESSION,
	DELETING_SESSION
};

//////////////////////////////////////////////////////////////////////////
// CXeLiveSession

CXeLiveSession::CXeLiveSession( )
	: m_hSession( INVALID_HANDLE_VALUE ),
	  m_SessionNonce( 0 ),
	  m_nState( NO_SESSION ),
	  m_bSessionCreated( false ),
	  m_bProcessingCommand( false ),
	  m_dwUserIndex( 0 ),
	  m_bPrivateSlot( TRUE ),
	  m_bLiveConnected( false ),
	  m_nUserIndex( 0 )
{
	ZeroMemory( &m_SessionInfo, sizeof( m_SessionInfo ) );

	ZeroMemory( &m_xov, sizeof( m_xov ) );
	m_xov.hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	ZeroMemory( m_szUserName, sizeof( m_szUserName ) );
}

CXeLiveSession::~CXeLiveSession( )
{
	InternalCleanUp( );

	if( m_xov.hEvent != NULL && m_xov.hEvent != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_xov.hEvent );
		m_xov.hEvent = NULL;
	}

	Shutdown( );
}

bool CXeLiveSession::Initialize( )
{
	// init online system
	if( XOnlineStartup( ) != ERROR_SUCCESS )
		return false;

	return true;
}

bool CXeLiveSession::Shutdown( )
{
	// shutdown online system
	if( XOnlineCleanup( ) != ERROR_SUCCESS )
		return false;

	return true;
}

void CXeLiveSession::ActivateSession( bool bActivate )
{
	if( bActivate )
	{
		char szUserName[ 128 ];
		XUserGetName( GetUserIndex( ), szUserName, sizeof( szUserName ) );

		if( m_szUserName[0] != '\0' && strcmp( szUserName, m_szUserName ) != 0 )
			QueueCommand( XE_SESSION_DISCONNECT );

		QueueCommand( XE_SESSION_CONNECT );
	}
	else
	{
		QueueCommand( XE_SESSION_DISCONNECT );
	}
}

void CXeLiveSession::InternalCleanUp( )
{
	// close existing session synchronously if present
	if( m_hSession != NULL && m_hSession != INVALID_HANDLE_VALUE )
	{
		if( m_bSessionCreated )
		{
			if( XSessionEnd( m_hSession, NULL ) )
			{
				m_dwUserIndex = GetUserIndex( );
				XSessionLeaveLocal( m_hSession, 1, &m_dwUserIndex, NULL );
				XSessionDelete( m_hSession, NULL );
				m_bSessionCreated = false;
			}
		}

		const BOOL bOk = CloseHandle( m_hSession );
		ERR_X_Assert( bOk );
		m_hSession = INVALID_HANDLE_VALUE;
	}

	// reset event
	if( m_xov.hEvent != NULL && m_xov.hEvent != INVALID_HANDLE_VALUE )
		ResetEvent( m_xov.hEvent );

	// forget user name
	ZeroMemory( m_szUserName, sizeof( m_szUserName ) );

	m_nState = NO_SESSION;
	m_bProcessingCommand = false;
}

void CXeLiveSession::QueueCommand( XeSessionCommand nCommand )
{
	m_lstCommands.push_back( nCommand );
}

void CXeLiveSession::UpdateSession( )
{
	// quick out
	if( m_nState == SESSION_ACTIVE ||
		m_nState == NO_SESSION      )
	{
		if( m_lstCommands.empty( ) )
			return;
	}

	if( m_bProcessingCommand )
	{
		if( !XHasOverlappedIoCompleted( &m_xov ) )
			return;

		m_bProcessingCommand = false;

		// operation has completed if we get to this point, so get result and make sure event is reset for reuse
		const DWORD dwResult = XGetOverlappedResult( &m_xov, NULL, FALSE );
		ResetEvent( m_xov.hEvent );

		// handle errors
		if( dwResult != ERROR_SUCCESS )
		{
			InternalCleanUp( );
			return;
		}

		// remaining processing depends on current state
		switch( m_nState )
		{
			case CREATING_SESSION:
			{
				m_bSessionCreated = true;
				XUserGetName( GetUserIndex( ), m_szUserName, sizeof( m_szUserName ) );
				JoinSession( );
				break;
			}

			case JOINING_SESSION:
			{
				StartSession( );
				break;
			}

			case STARTING_SESSION:
			{
				//CXeMessageBarManager::Get( ).AddMessage( 2000, LIVE_SESSION_STARTED_OK );

				m_nState = SESSION_ACTIVE;
				break;
			}

			case ENDING_SESSION:
			{
				LeaveSession( );
				break;
			}

			case LEAVING_SESSION:
			{
				DeleteSession( );
				break;
			}

			case DELETING_SESSION:
			{
				m_bSessionCreated = false;

				const BOOL bOk = CloseHandle( m_hSession );
				ERR_X_Assert( bOk );
				m_hSession = INVALID_HANDLE_VALUE;

				ZeroMemory( m_szUserName, sizeof( m_szUserName ) );

				m_nState = NO_SESSION;

				break;
			}

			default:
				ERR_X_Assert( false ); // state not supported
		}
	}
	else
	{
		// start next command
		XeSessionCommand nCommand = m_lstCommands.front( );
		m_lstCommands.pop_front( );

		switch( nCommand )
		{
			case XE_SESSION_CONNECT:
				CreateSession( );
				break;

			case XE_SESSION_DISCONNECT:
				DestroySession( );
				break;

			default:
				ERR_X_Assert( false );
				break;
		}
	}
}

bool CXeLiveSession::CreateSession( )
{
	// quick-out
	if( m_hSession != INVALID_HANDLE_VALUE )
		// already created
		return false;

	// get user index
	DWORD dwUserIndex = GetUserIndex( );
	if( dwUserIndex < 0 || dwUserIndex > 3 )
		return false;

	// XDK bug workaround: should be fixed in May 2005 XDK
	XUSER_SIGNIN_STATE state = XUserGetSigninState( dwUserIndex );
	if( state == eXUserSigninState_NotSignedIn )
		return false;

	// set up game-related properties
	XUserSetContext( dwUserIndex, X_CONTEXT_GAME_MODE, CONTEXT_GAME_MODE_CAMPAIGN_MODE );
	XUserSetContext( dwUserIndex, X_CONTEXT_GAME_TYPE, X_CONTEXT_GAME_TYPE_STANDARD    );

	// advertise the session
	ZeroMemory( &m_SessionInfo, sizeof( m_SessionInfo ) );
	ResetEvent( m_xov.hEvent );

	const DWORD dwFlags =
		XSESSION_CREATE_USES_PRESENCE              |
		XSESSION_CREATE_INVITES_DISABLED           |
		XSESSION_CREATE_JOIN_VIA_PRESENCE_DISABLED |
		XSESSION_CREATE_JOIN_IN_PROGRESS_DISABLED;

	const DWORD dwPublicSlots = 0;
	const DWORD dwPrivateSlots = 1;

	const DWORD dwRetVal = XSessionCreate
		(
		dwFlags,
		dwUserIndex,
		dwPublicSlots,
		dwPrivateSlots,
		&m_SessionNonce,
		&m_SessionInfo,
		&m_xov,
		&m_hSession
		);

	const bool bSuccess = ( dwRetVal == ERROR_IO_PENDING );
	ERR_X_Assert( bSuccess );

	if( bSuccess )
	{
		m_nState = CREATING_SESSION;
		m_bProcessingCommand = true;
	}
	else
		InternalCleanUp( );

	return bSuccess;
}

bool CXeLiveSession::JoinSession( )
{
	if( m_hSession == INVALID_HANDLE_VALUE )
		return false;

	m_dwUserIndex = GetUserIndex( );
	const DWORD dwRetVal = XSessionJoinLocal( m_hSession, 1, &m_dwUserIndex, &m_bPrivateSlot, &m_xov );

	const bool bSuccess = ( dwRetVal == ERROR_IO_PENDING );
	ERR_X_Assert( bSuccess );

	if( bSuccess )
	{
		m_nState = JOINING_SESSION;
		m_bProcessingCommand = true;
	}
	else
		InternalCleanUp( );

	return bSuccess;
}

bool CXeLiveSession::StartSession( )
{
	if( m_hSession == INVALID_HANDLE_VALUE )
		return false;

	const DWORD dwRetVal = XSessionStart( m_hSession, 0, &m_xov );

	const bool bSuccess = ( dwRetVal == ERROR_IO_PENDING );
	ERR_X_Assert( bSuccess );

	if( bSuccess )
	{
		m_nState = STARTING_SESSION;
		m_bProcessingCommand = true;
	}
	else
		InternalCleanUp( );

	return bSuccess;
}

bool CXeLiveSession::DestroySession( )
{
	if( m_hSession == INVALID_HANDLE_VALUE )
		return false;

	const DWORD dwRetVal = XSessionEnd( m_hSession, &m_xov );

	const bool bSuccess = ( dwRetVal == ERROR_IO_PENDING );
	ERR_X_Assert( bSuccess );

	if( bSuccess )
	{
		m_nState = ENDING_SESSION;
		m_bProcessingCommand = true;
	}
	else
		InternalCleanUp( );

	return bSuccess;
}

bool CXeLiveSession::LeaveSession( )
{
	if( m_hSession == INVALID_HANDLE_VALUE )
		return false;

	m_dwUserIndex = GetUserIndex( );
	const DWORD dwRetVal = XSessionLeaveLocal( m_hSession, 1, &m_dwUserIndex, &m_xov );

	const bool bSuccess = ( dwRetVal == ERROR_IO_PENDING );
	ERR_X_Assert( bSuccess );

	if( bSuccess )
	{
		m_nState = LEAVING_SESSION;
		m_bProcessingCommand = true;
	}
	else
		InternalCleanUp( );

	return bSuccess;
}

bool CXeLiveSession::DeleteSession( )
{
	// quick-out
	if( m_hSession == INVALID_HANDLE_VALUE )
		return false;

	const DWORD dwRetVal = XSessionDelete( m_hSession, &m_xov );

	const bool bSuccess = ( dwRetVal == ERROR_IO_PENDING );
	ERR_X_Assert( bSuccess );

	if( bSuccess )
	{
		m_nState = DELETING_SESSION;
		m_bProcessingCommand = true;
	}
	else
		InternalCleanUp( );

	return bSuccess;
}

*/

#endif // defined(_XENON)
