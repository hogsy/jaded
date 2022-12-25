//////////////////////////////////////////////////////////////////////////
// Xenon Achievements
// ------------------------
// Adds achievements to the player's gamer profile on XBox Live!
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"
#if defined(_XENON) || defined(ACTIVE_EDITORS) || defined(WIN32)

#include <list>

#include "Achievements.h"

#if defined(_XENON)
#include "..\XLAST\KingKong.spa.h"

#include "..\Interf\MessageBar.h"
#include "..\MenuManager\MenuManager.h"
#include "..\Profile\Profile.h"

//////////////////////////////////////////////////////////////////////////
// type definitions

namespace // anonymous
{
	struct OperationInfo
	{
		XUSER_ACHIEVEMENT	achievement;
		XOVERLAPPED			xov;
	};
}

//////////////////////////////////////////////////////////////////////////
// module data

namespace
{
	std::list< OperationInfo* > g_lstPendingAchievements;
}

//////////////////////////////////////////////////////////////////////////
// module functions 

namespace // anonymous
{
	void OnEndWriteAchievement( DWORD dwAchievementID, bool bSuccess )
	{
//		if( bSuccess )
//			CXeMessageBarManager::Get( ).AddMessage( 3000, ACHIEVEMENT_GAINED );
//		else
//			CXeMessageBarManager::Get( ).AddMessage( 2000, ACHIEVEMENT_CANT_WRITE );
	}
}

#endif // defined(_XENON)

//////////////////////////////////////////////////////////////////////////
//

void AddAchievement( int nAchievementID )
{
#if defined(_XENON)

	// no achievements for demo
	if( g_MenuManager.IsInDemoMode_NoAchievements() )
		return;

	// check if we can write an achievement
	XUSER_SIGNIN_STATE state = XUserGetSigninState( g_XeProfile.GetUserIndex( ) );
	if( state == eXUserSigninState_NotSignedIn )
		return;

	// check that user is allowed to receive this achievement
	XUID xuid;
	DWORD dwRetVal = XUserGetXUID( g_XeProfile.GetUserIndex( ), &xuid );
	ERR_X_Assert( dwRetVal == ERROR_SUCCESS );

	if( g_XeProfile.GetGameOwner( ) == false )
		// we're not the game owner, so don't give achievement
		return;

	// setup Xbox Live operation as asynchronous write
	OperationInfo* pInfo = new OperationInfo;
		ZeroMemory( pInfo, sizeof( OperationInfo ) );

	pInfo->achievement.dwUserIndex     = g_XeProfile.GetUserIndex( );
	pInfo->achievement.dwAchievementId = nAchievementID;
	pInfo->xov.hEvent                  = CreateEvent( NULL, FALSE, FALSE, NULL );

	ERR_X_Assert( pInfo->achievement.dwUserIndex >= 0 && pInfo->achievement.dwUserIndex <= 3 );
	ERR_X_Assert( pInfo->xov.hEvent != NULL && pInfo->xov.hEvent != INVALID_HANDLE_VALUE );

	// write new achievement
	dwRetVal = XUserWriteAchievements( 1, &pInfo->achievement, &pInfo->xov );
	if( dwRetVal == ERROR_IO_PENDING )
	{
		// add to pending list
		g_lstPendingAchievements.push_back( pInfo );
	}
	else
	{
		// asynchronous write failed
		if( pInfo->xov.hEvent != NULL && pInfo->xov.hEvent != INVALID_HANDLE_VALUE )
			CloseHandle( pInfo->xov.hEvent );

		delete pInfo;

		ERR_X_Assert( false );
	}

#endif
}

void UpdateAchievements( )
{
#if defined(_XENON)

	if( g_lstPendingAchievements.empty( ) )
		return;

	std::list< OperationInfo* >::iterator i = g_lstPendingAchievements.begin( );
	std::list< OperationInfo* >::iterator e = g_lstPendingAchievements.end( );

	for( ; i != e; )
	{
		OperationInfo* pInfo = *i;

		// test if IO opration has completed
		if( !XHasOverlappedIoCompleted( &pInfo->xov ) )
		{
			++i;
			continue;
		}

		// IO has completed
		DWORD dwStatus = XGetOverlappedResult( &pInfo->xov, NULL, FALSE );
		ERR_X_Assert( dwStatus != ERROR_IO_INCOMPLETE );

		if( dwStatus == ERROR_SUCCESS )
		{
			OnEndWriteAchievement( pInfo->achievement.dwAchievementId, true );
		}
		else
		{
			DWORD dwError = XGetOverlappedExtendedError( &pInfo->xov );
			OnEndWriteAchievement( pInfo->achievement.dwAchievementId, false );
		}

		// clean up
		if( pInfo->xov.hEvent != NULL && pInfo->xov.hEvent != INVALID_HANDLE_VALUE )
			CloseHandle( pInfo->xov.hEvent );

		delete pInfo;
		g_lstPendingAchievements.erase( i++ );
	}

#endif // defined(_XENON)
}

#endif // defined(_XENON) && !defined(ACTIVE_EDITORS) || defined(WIN32)
