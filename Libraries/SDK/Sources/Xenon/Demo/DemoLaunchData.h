//////////////////////////////////////////////////////////////////////////
// Demo Launch Data
// ----------------
// Access to parameters passed in by demo disk launcher
//////////////////////////////////////////////////////////////////////////

#ifndef __DEMOLAUNCHDATA_H__
#define __DEMOLAUNCHDATA_H__

#if defined(_XENON)

//////////////////////////////////////////////////////////////////////////
// definitions

enum DemoRunMode
{
	DEMO_MODE_NONE,
	DEMO_MODE_USER_SELECTED,
	DEMO_MODE_KIOSK
};

//////////////////////////////////////////////////////////////////////////
// CXeDemoLaunchData

class CXeDemoLaunchData
{
// interface
public:
	static int			GetTimeoutValue( );
	static void			ReturnToLauncher( );
	static void			DisableReboot( bool bDisable );
    static int			IsLaunchedInDemoMode( );
	static DemoRunMode	GetDemoMode( );
	static void			SetDemoMode( DemoRunMode mode );

private:
	static void			ReadLaunchData( );
};

#endif // defined(_XENON)

#endif // __DEMOLAUNCHDATA_H__
