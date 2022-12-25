//////////////////////////////////////////////////////////////////////////
// Demo Launch Data
// ----------------
// Access to parameters passed in by demo disk launcher
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"
#include "DemoLaunchData.h"

#ifdef _XENON

//////////////////////////////////////////////////////////////////////////
// static data

static bool g_bReadData = false;
static BYTE* g_pLaunchData = { 0 };
static LD_DEMO* g_pDemoData = NULL;
static DWORD g_dwLaunchDataSize = 0;
static bool g_bDisableReboot = false;
static DemoRunMode g_nDemoMode = DEMO_MODE_NONE;

//////////////////////////////////////////////////////////////////////////
// static functions

static int read_timeout_value( )
{
	if( g_pDemoData == NULL )
		return 0;

	// get timeout info
	int nTimeout = 0;
	if( g_pDemoData->dwID == LAUNCH_DATA_DEMO_ID )
		nTimeout = g_pDemoData->dwTimeout / 1000;

	// return value
	return nTimeout;
}

//////////////////////////////////////////////////////////////////////////
// CXeDemoLaunchData

void CXeDemoLaunchData::ReadLaunchData( )
{
	if( !g_bReadData )
	{
		g_bReadData = true;

		// get launch data size
		if( XGetLaunchDataSize( &g_dwLaunchDataSize ) != ERROR_SUCCESS )
		{
			// no launch data
			g_dwLaunchDataSize = 0;
			return;
		}

		g_pLaunchData = new BYTE[ g_dwLaunchDataSize ];
		ZeroMemory( g_pLaunchData, g_dwLaunchDataSize );

		if( XGetLaunchData( g_pLaunchData, g_dwLaunchDataSize ) == ERROR_SUCCESS )
		{
			g_pDemoData = (LD_DEMO*)g_pLaunchData;
			g_nDemoMode = (g_pDemoData->dwRunmode == XLDEMO_RUNMODE_KIOSKMODE) ? DEMO_MODE_KIOSK : DEMO_MODE_USER_SELECTED;
		}
	}
}

int CXeDemoLaunchData::GetTimeoutValue( )
{
	// get launch data
	ReadLaunchData( );

	return read_timeout_value( );
}

void CXeDemoLaunchData::ReturnToLauncher( )
{
	// get launch data
	ReadLaunchData( );

	// no reboot while UI in shown
	if( g_bDisableReboot )
		return;

	// get timeout info
	if( g_pDemoData != NULL )
	{
		XSetLaunchData( g_pLaunchData, g_dwLaunchDataSize );
		XLaunchNewImage( g_pDemoData->szLauncherXEX, 0 );
	}
	else
		XLaunchNewImage( NULL, 0 );
}

void CXeDemoLaunchData::DisableReboot( bool bDisable )
{
	g_bDisableReboot = bDisable;
}

int CXeDemoLaunchData::IsLaunchedInDemoMode()
{
    return false;
}

DemoRunMode CXeDemoLaunchData::GetDemoMode( )
{
	return g_nDemoMode;
}

void CXeDemoLaunchData::SetDemoMode( DemoRunMode mode )
{
	g_nDemoMode = mode;
}

#endif // _XENON
