//////////////////////////////////////////////////////////////////////////
// Xenon Gamer Profile
// ------------------------
// Manages Gamer Profile related stuff
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"

#if defined(_XENON)
#include "Profile.h"
#include "..\MenuManager\MenuManager.h"

//////////////////////////////////////////////////////////////////////////
// settings to update

const DWORD g_SettingIDs[ ] =
{
	XPROFILE_GAMER_DIFFICULTY,
	XPROFILE_GAMER_YAXIS_INVERSION,
	XPROFILE_OPTION_CONTROLLER_VIBRATION
};

//////////////////////////////////////////////////////////////////////////
// global data

CXeProfile g_XeProfile;

//////////////////////////////////////////////////////////////////////////
// CXeProfile

CXeProfile::CXeProfile( )
	: m_pSettingResults( NULL ),
	  m_eDifficulty( DIFFICULTY_NORMAL ),
	  m_bYAxisInverted( false ),
	  m_bControllerVibrationEnabled( true ),
	  m_bGameOwner( false ),
	  m_bReady( false ),
	  m_bUserSet( false ),
	  m_nUserIndex( 0 )
{
	ZeroMemory( &m_xov, sizeof( m_xov ) );

	switch( XGetLanguage( ) )
	{
		case XC_LANGUAGE_ENGLISH:
			m_eLanguage = LANGUAGE_ENGLISH;
			break;

		case XC_LANGUAGE_FRENCH:
			m_eLanguage = LANGUAGE_FRENCH;
			break;

		case XC_LANGUAGE_SPANISH:
			m_eLanguage = LANGUAGE_SPANISH;
			break;

		case XC_LANGUAGE_ITALIAN:
			m_eLanguage = LANGUAGE_ITALIAN;
			break;

		case XC_LANGUAGE_GERMAN:
			m_eLanguage = LANGUAGE_GERMAN;
			break;

//		case XC_LANGUAGE_DUTCH:  // not defined
//			m_eLanguage = LANGUAGE_DUTCH;
//			break;

		case XC_LANGUAGE_PORTUGUESE:
			m_eLanguage = LANGUAGE_PORTUGUESE;
			break;

		case XC_LANGUAGE_TCHINESE:
			m_eLanguage = LANGUAGE_CHINESE;
			break;

		case XC_LANGUAGE_KOREAN:
			m_eLanguage = LANGUAGE_KOREAN;
			break;

		case XC_LANGUAGE_JAPANESE:
			m_eLanguage = LANGUAGE_JAPANESE;
			break;

		default:
			m_eLanguage = LANGUAGE_ENGLISH;
			break;
	}
}

CXeProfile::~CXeProfile( )
{
	Cleanup( );
}

bool CXeProfile::IsReady( ) const
{
	return m_bReady;
}

int CXeProfile::GetID( ) const
{
	if( g_MenuManager.IsInDemoMode_NoStorageAccess() )
		return -1;

	if( !m_bUserSet )
		return -1;

	CHAR szName[ 64 ];
	ZeroMemory( &szName, sizeof( szName ) );

	XUserGetName( GetUserIndex( ), szName, sizeof( szName ) );

	ProfileList::const_iterator f = m_lstProfiles.find( szName );
	if( f == m_lstProfiles.end( ) )
		return -1;

	return (*f).second;
}

void CXeProfile::UpdateProfileSettings( )
{
	// quick-out
	if( m_xov.hEvent != NULL )
		return;
	if( !m_bUserSet )
		return;

	if( XUserGetSigninState( GetUserIndex( ) ) == eXUserSigninState_NotSignedIn )
		return;

	// flag as not ready while updating
	m_bReady = false;

	// determine size required
	const DWORD dwQtySettings = sizeof( g_SettingIDs ) / sizeof( g_SettingIDs[0] );
	DWORD dwSettingSizeMax = 0;

	DWORD dwErr = XUserReadProfileSettings
		(
		0,
		GetUserIndex( ),
		dwQtySettings,
		g_SettingIDs,
		&dwSettingSizeMax,
		NULL,
		NULL
		);

	ERR_X_Assert( dwErr == ERROR_INSUFFICIENT_BUFFER );
	ERR_X_Assert( dwSettingSizeMax > 0 );

	// do actual read
	if( dwSettingSizeMax <= 0 )
	{
		Cleanup( );
		return;
	}
	else
	{
		// prepare for asynchronous operation
		m_xov.hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
		if( m_xov.hEvent == INVALID_HANDLE_VALUE )
		{
			ERR_X_Assert( false );
			Cleanup( );
			return;
		}

		// allocate memory
		m_pSettingResults = (XUSER_READ_PROFILE_SETTING_RESULT*)new BYTE[ dwSettingSizeMax ];

		// read settings asynchronously
		dwErr = XUserReadProfileSettings
			(
			0,
			GetUserIndex( ),
			dwQtySettings,
			g_SettingIDs,
			&dwSettingSizeMax,
			m_pSettingResults,
			&m_xov
			);

		if( dwErr != ERROR_IO_PENDING )
		{
			ERR_X_Assert( false );
			Cleanup( );
			return;
		}
	}
}

void CXeProfile::Cleanup( )
{
	if( m_xov.hEvent != NULL && m_xov.hEvent != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_xov.hEvent );
		ZeroMemory( &m_xov, sizeof( m_xov ) );
	}

	if( m_pSettingResults != NULL )
	{
		delete[] m_pSettingResults;
		m_pSettingResults = NULL;
	}
}

void CXeProfile::InternalPump( )
{
	if( !m_bUserSet )
		return;

	ERR_X_Assert( m_xov.hEvent != NULL );

	if( !XHasOverlappedIoCompleted( &m_xov ) )
		return;

	const DWORD dwStatus = XGetOverlappedResult( &m_xov, NULL, FALSE );
	ERR_X_Assert( dwStatus != ERROR_IO_INCOMPLETE );

	if( dwStatus == ERROR_SUCCESS )
	{
		// parse results
		for( DWORD i = 0; i < m_pSettingResults->dwSettingsLen; ++i )
		{
			const int nData = m_pSettingResults->pSettings[ i ].data.nData;

			switch( m_pSettingResults->pSettings[ i ].dwSettingId )
			{
				case XPROFILE_GAMER_DIFFICULTY:
					if( nData == XPROFILE_GAMER_DIFFICULTY_EASY )
						m_eDifficulty = DIFFICULTY_EASY;
					else if( nData == XPROFILE_GAMER_DIFFICULTY_NORMAL )
						m_eDifficulty = DIFFICULTY_NORMAL;
					else
						m_eDifficulty = DIFFICULTY_HARD;
					break;

				case XPROFILE_GAMER_YAXIS_INVERSION:
					m_bYAxisInverted = ( nData == XPROFILE_YAXIS_INVERSION_ON );
					break;

				case XPROFILE_OPTION_CONTROLLER_VIBRATION:
					m_bControllerVibrationEnabled = ( nData == XPROFILE_CONTROLLER_VIBRATION_ON );
					break;
			}
		}

		// get profile name
		CHAR szName[ 64 ];
		ZeroMemory( &szName, sizeof( szName ) );

		XUserGetName( GetUserIndex( ), szName, sizeof( szName ) );

		if( L_strlen( szName ) > 0 && m_lstProfiles.find( szName ) == m_lstProfiles.end( ) )
		{
			// new user
			m_lstProfiles[ szName ] = m_lstProfiles.size( ) + 1;
		}

		m_bReady = true;
	}
	else
	{
		// identify cause of error (possibly no active profile)
		const DWORD dwError = XGetOverlappedExtendedError( &m_xov );
	}

	// get rid of work structures
	Cleanup( );
}

#endif // defined(_XENON)
