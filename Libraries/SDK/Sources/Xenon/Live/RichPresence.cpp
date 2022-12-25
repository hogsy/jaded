//////////////////////////////////////////////////////////////////////////
// Xenon Rich Presence
// ------------------------
// Updates player's Rich Presence state on XBox Live!
// Changes occur when entering new map and when reaching new check point
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"

#include "RichPresence.h"
#include "Session.h"
#include "..\Profile\Profile.h"
#include "Engine\Sources\WORld\WORstruct.h"

#if defined(_XENON)
#include "..\XLAST\KingKong.spa.h"
#endif

//////////////////////////////////////////////////////////////////////////
// external symbols

extern WOR_tdst_World*	WOR_gpst_CurrentWorld;

static int g_nMap = -1;

//////////////////////////////////////////////////////////////////////////
// module functions 

namespace // anonymous
{
#if defined(_XENON)

	int get_map_id( )
	{
		if( WOR_gpst_CurrentWorld )
		{
			switch( WOR_gpst_CurrentWorld->h_WorldKey )
			{
				case 0x3D00C456:  // menus
				case 0x3d00c45a:
					return CONTEXT_MAP_NAME_NO_MAP;
			}

			switch( g_nMap )
			{
				case 100:
					return CONTEXT_MAP_NAME_01A;
				case 101:
					return CONTEXT_MAP_NAME_01B;
				case 201:
					return CONTEXT_MAP_NAME_02A;
				case 203:
					return CONTEXT_MAP_NAME_02C1;
				case 204:
					return CONTEXT_MAP_NAME_02C2;
				case 301:
					return CONTEXT_MAP_NAME_03A;
				case 302:
					return CONTEXT_MAP_NAME_03B;
				case 303:
					return CONTEXT_MAP_NAME_03C;
				case 305:
					return CONTEXT_MAP_NAME_03E;
				case 306:
					return CONTEXT_MAP_NAME_03F;
				case 304:
					return CONTEXT_MAP_NAME_03D;
				case 401:
					return CONTEXT_MAP_NAME_04A;
				case 402:
					return CONTEXT_MAP_NAME_04A1;
				case 403:
					return CONTEXT_MAP_NAME_04A2;
				case 404:
					return CONTEXT_MAP_NAME_04B;
				case 501:
					return CONTEXT_MAP_NAME_05A1;
				case 502:
					return CONTEXT_MAP_NAME_05A2;
				case 504:
					return CONTEXT_MAP_NAME_05C;
				case 702:
					return CONTEXT_MAP_NAME_07B;
				case 705:
					return CONTEXT_MAP_NAME_07D;
				case 901:
					return CONTEXT_MAP_NAME_09B;
				case 1002:
					return CONTEXT_MAP_NAME_10B;
				case 1101:
					return CONTEXT_MAP_NAME_11A;
				case 1200:
					return CONTEXT_MAP_NAME_12A;
				case 1201:
					return CONTEXT_MAP_NAME_12B;
				case 703:
					return CONTEXT_MAP_NAME_07B2;
				case 1401:
					return CONTEXT_MAP_NAME_14A;
				case 1402:
					return CONTEXT_MAP_NAME_14B;
				case 1501:
					return CONTEXT_MAP_NAME_15A;
				case 1701:
					return CONTEXT_MAP_NAME_17A;
				case 1702:
					return CONTEXT_MAP_NAME_17A2;
				case 1703:
					return CONTEXT_MAP_NAME_17B;
				case 1801:
					return CONTEXT_MAP_NAME_18A;
				case 1803:
					return CONTEXT_MAP_NAME_03CR;
				case 1804:
					return CONTEXT_MAP_NAME_03BR;
				case 1805:
					return CONTEXT_MAP_NAME_03AR;
				case 1901:
					return CONTEXT_MAP_NAME_19A;
				case 1902:
					return CONTEXT_MAP_NAME_19B;
				case 2001:
					return CONTEXT_MAP_NAME_20A;
				case 2003:
					return CONTEXT_MAP_NAME_20C;
				case 2004:
					return CONTEXT_MAP_NAME_20D;
				case 2100:
					return CONTEXT_MAP_NAME_21S;
			}
		}

		return CONTEXT_MAP_NAME_NO_MAP;
	}

#endif // defined(_XENON)
}

//////////////////////////////////////////////////////////////////////////
//

void SetMap( int nMap )
{
	g_nMap = nMap;
}

void UpdateRichPresence( )
{
#if defined(_XENON)

	// get user id
	const DWORD dwUserIndex = g_XeProfile.GetUserIndex( );

	// must be signed in to Xbox Live
	if( !g_XeLiveConnected )
		return;

	XUSER_SIGNIN_STATE state = XUserGetSigninState( dwUserIndex );
	if( state != eXUserSigninState_SignedInToLive )
		return;

	// update Rich Presence information
	const int nMapID = get_map_id( );
	XUserSetContext( dwUserIndex, CONTEXT_MAP_NAME, nMapID );

	XUserSetContext( dwUserIndex, X_CONTEXT_PRESENCE, CONTEXT_PRESENCE_CURRENT_MAP );

#endif
}
